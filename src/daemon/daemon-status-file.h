// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef NETDATA_DAEMON_STATUS_FILE_H
#define NETDATA_DAEMON_STATUS_FILE_H

#include "libnetdata/libnetdata.h"
#include "daemon/config/netdata-conf-profile.h"

typedef enum {
    DAEMON_STATUS_NONE,
    DAEMON_STATUS_INITIALIZING,
    DAEMON_STATUS_RUNNING,
    DAEMON_STATUS_EXITING,
    DAEMON_STATUS_EXITED,
} DAEMON_STATUS;
ENUM_STR_DEFINE_FUNCTIONS_EXTERN(DAEMON_STATUS);

typedef enum {
    DAEMON_OS_TYPE_UNKNOWN,
    DAEMON_OS_TYPE_LINUX,
    DAEMON_OS_TYPE_FREEBSD,
    DAEMON_OS_TYPE_MACOS,
    DAEMON_OS_TYPE_WINDOWS,
} DAEMON_OS_TYPE;
ENUM_STR_DEFINE_FUNCTIONS_EXTERN(DAEMON_OS_TYPE);

typedef struct daemon_status_file {
    char version[32];           // the netdata version
    DAEMON_STATUS status;       // the daemon status
    EXIT_REASON exit_reason;    // the exit reason (maybe empty)
    ND_PROFILE profile;         // the profile of the agent
    DAEMON_OS_TYPE os_type;

    time_t boottime;            // system boottime
    time_t uptime;              // netdata uptime
    usec_t timestamp_ut;        // the timestamp of the status file
    size_t restarts;            // the number of times this agent has restarted

    ND_UUID boot_id;            // the boot id of the system
    ND_UUID invocation;         // the netdata invocation id generated the file
    ND_UUID host_id;            // the machine guid of the agent
    ND_UUID node_id;            // the Netdata Cloud node id of the agent
    ND_UUID claim_id;           // the Netdata Cloud claim id of the agent

    struct {
        time_t init;
        time_t exit;
    } timings;

    OS_SYSTEM_MEMORY memory;
    OS_SYSTEM_DISK_SPACE var_cache;

    const char *install_type;
    const char *architecture;   // ECS: host.architecture
    const char *virtualization;
    const char *container;
    const char *kernel_version; // ECS: os.kernel
    const char *os_name;        // ECS: os.name
    const char *os_version;     // ECS: os.version
    const char *os_id;          // ECS: os.family
    const char *os_id_like;     // ECS: os.platform
    bool read_system_info;

    struct {
        long line;
        const char *filename;
        const char *function;
        const char *errno_str;
        const char *message;
        char stack_trace[4096];
        char thread[ND_THREAD_TAG_MAX + 1];
    } fatal;

    struct {
        XXH64_hash_t hash;
        usec_t timestamp_ut;
    } dedup[20];
} DAEMON_STATUS_FILE;

// loads the last status saved
DAEMON_STATUS_FILE daemon_status_file_load(void);

// saves the current status
void daemon_status_file_update_status(DAEMON_STATUS status);
void daemon_status_file_exit_reason_save(EXIT_REASON reason);
void daemon_status_file_deadly_signal_received(EXIT_REASON reason);

// check for a crash
void daemon_status_file_check_crash(void);

bool daemon_status_file_has_last_crashed(void);
bool daemon_status_file_was_incomplete_shutdown(void);

void daemon_status_file_startup_step(const char *step);
void daemon_status_file_shutdown_step(const char *step);

void daemon_status_file_register_fatal(const char *filename, const char *function, const char *message, const char *errno_str, const char *stack_trace, long line);

#endif //NETDATA_DAEMON_STATUS_FILE_H

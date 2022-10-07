#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#include "log.h"
#include "pid.h"
#include "cmdsocket.h"
#include "command.h"

static char *cmd_file = NULL;
static int cmd_fd = -1;

static int run_as_daemon = 0;
static char *pid_file = NULL;

static int log_fd = -1;

static int exiting = 0;

static void show_help(void)
{
	log_error("Usage:\n" );
	log_error("  --daemon           Run as daemon process\n" );
	log_error("  --socket <socket>  Read commands from socket\n" );
    log_error("  --log <filename>   Log to file\n" );
    log_error("  --verbose          Enable verbose mode\n" );
}

static void handle_signal(int sig)
{
    log_info("got signal: %d", sig);

    switch (sig) {
        case SIGINT:
        case SIGTERM:
            exiting = 1;
            break;
    }
}

void cleanup(void)
{
    log_info("cleanup...");

    if (cmd_file) {
        if (cmd_fd != -1)
            close(cmd_fd);
        unlink(cmd_file);
        cmd_fd = -1;
    }

    if (pid_file)
        unlink(pid_file);
}

static int init_logger(const char *log_file, int verbose)
{
    if (log_file == NULL) {
        log_set_quiet(0);
        log_set_level(verbose > 0 ? LOG_DEBUG : LOG_INFO);
    } else {
        log_set_quiet(1);
        FILE *fp;
        fp = fopen(log_file, "w");
        if(fp == NULL) {
            log_error("fopen() fail:%s\n", log_file);
            return -1;
        }
        log_add_fp(fp, verbose > 0 ? LOG_DEBUG : LOG_INFO);
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int c, option_index = 0;
    int verbose = 0;
    char *log_file = NULL;

    struct option long_options[] = {
        {"daemon", no_argument, &run_as_daemon, 1},
        {"pid", required_argument, 0, 'p'},
        {"socket", required_argument, 0, 's'},
        {"log", required_argument, 0, 'l'},
        {"verbose", no_argument, 0, 'v'},
        {0, 0, 0, 0}
    };

    while ((c = getopt_long(argc, argv, "dhp:s:l:v", long_options, &option_index)) != -1) {
        switch(c) {
            case 'd':
                run_as_daemon = 1;
                break;
            case 'p':
                pid_file = optarg;
                break;
            case 's':
                cmd_file = optarg;
                break;
            case 'l':
                log_file = optarg;
                break;
            case 'v':
                verbose = 1;
                break;
            case 'h':
                show_help();
                return 0;
            case '?':
                printf("unknown option: %c\n", optopt);
                return 1;
        }
    }
    init_logger(log_file, verbose);
    log_debug("msg1");
    log_info("msg2");
    log_warn("msg3");

    struct sigaction handler;
    handler.sa_handler = handle_signal;
    sigfillset(&handler.sa_mask);
    handler.sa_flags = 0;
    sigaction(SIGINT, &handler, 0);
    sigaction(SIGTERM, &handler, 0);

    if (cmd_file) {
        cmd_fd = bind_cmdsocket(cmd_file);
        if (cmd_fd < 0) {
            log_error("bind_cmdsocket()\n");
            return 1;
        }
    }

    if (run_as_daemon) {
        if (daemon(0, 0))
            log_error("daemon()");
    }

    if (pid_file) {
        if (create_pid_file(argv[0], pid_file, 0) == -1)
            log_error("create_pid_file()");
    }
    
    fd_set rfds;
    int max_fd = -1;
    struct timeval tv;
    int retval;
    while(cmd_fd != -1) {
        FD_ZERO(&rfds);
        if (cmd_fd != -1) {
            FD_SET(cmd_fd, &rfds);
            max_fd = cmd_fd > max_fd ? cmd_fd : max_fd;
            tv.tv_sec = 5;
            tv.tv_usec = 0;
            retval = select(max_fd + 1, &rfds, NULL, NULL, &tv);
            if (retval == -1 && errno != EINTR) {
                perror("select()");
                continue;
            } else if (exiting) {
                break;
            } else if (retval) {
                // do the real job
                // ...
                if (cmd_fd != -1 && FD_ISSET(cmd_fd, &rfds)) {
                    struct command *cmd = read_command(cmd_fd);
                    if (cmd != NULL) {
                        run_command(cmd);
                        free(cmd);
                    }
                }
            }
        }
    }

    cleanup();
}
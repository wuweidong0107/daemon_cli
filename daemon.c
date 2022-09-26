#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "cmdsocket.h"
#include "command.h"

static char *cmd_file = NULL;
static int cmd_fd = -1;

static int run_as_daemon = 0;
static char *pid_file = NULL;
static int exiting = 0;

static struct option long_options[] = {
    {"daemon", no_argument, &run_as_daemon, 1},
    {"pidfile", required_argument, 0, 'p'},
    {"socket", required_argument, 0, 's'},
    {0, 0, 0, 0}
};

static void show_help(void)
{
	printf( "Usage:\n" );
	printf( "  --daemon           Run as daemon process\n" );
	printf( "  --socket <socket>  Read commands from socket\n" );
}

static void handle_signal(int sig)
{
    switch (sig) {
        case SIGINT:
        case SIGTERM:
            exiting = 1;
            break;
    }
}

static void cleanup(void)
{
    if (cmd_file) {
        if (cmd_fd != -1)
            close(cmd_fd);
        unlink(cmd_file);
        cmd_fd = -1;
    }

    if (pid_file)
        unlink(pid_file);
}

static int write_pidfile(char *pidfile)
{
    FILE *pid = fopen(pidfile, "w+");
    if (pid == NULL)
        return 1;
    fprintf(pid, "%u\n", (unsigned int)getpid());
    return fclose(pid);
}
int main(int argc, char *argv[])
{
    int c, option_index = 0;

    while ((c = getopt_long(argc, argv, "dhp:s:", long_options, &option_index)) != -1) {
        printf("c:%c\n",c);
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
            case 'h':
                show_help();
                return 0;
            case '?':
                printf("unknown option: %c\n", optopt);
                return 1;
        }
    }

    struct sigaction handler;
    handler.sa_handler = handle_signal;
    sigfillset(&handler.sa_mask);
    handler.sa_flags = 0;
    sigaction(SIGINT, &handler, 0);
    sigaction(SIGTERM, &handler, 0);

    if (cmd_file) {
        cmd_fd = bind_cmdsocket(cmd_file);
        if (cmd_fd < 0)
            return 1;
    }

    if (run_as_daemon) {
        int err = daemon(0, 0);
        if (err) {
            perror("daemon()");
        }
    }

    if (pid_file)
        write_pidfile(pid_file);

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
            if (retvl == -1 && errno != EINTR) {
                perror("select()");
                continue;
            } else if (exiting) {
                break;
            } else if (retval) {
                // do the real job
                // ...

                if (cmd_fd != -1 && FD_ISSET(cmd_fd, &rfds)) {
                    
                }
            }
        }
    }
}
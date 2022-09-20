#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>

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

static cleanup(void)
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
}
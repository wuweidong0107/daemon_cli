#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include "command.h"
#include "cmdsocket.h"

void show_help(void)
{
	fprintf(stderr, "Usage:\n" );
	fprintf(stderr, "  --socket <socket>\n");
    fprintf(stderr, "  --socket <socket> --cmd1\n");
    fprintf(stderr, "  --socket <socket> --cmd2 <arg1> <arg2> ...\n");
    fprintf(stderr, "  --socket <socket> --quit\n");
}

int main(int argc, char *argv[])
{
    char *socket = NULL;
    int op_cmd1 = 0;
    int op_cmd2 = 0;
    int op_quit = 0;
    enum command_type ctype = CMD_NOP;

    struct option long_options[] = {
        {"socket", required_argument, 0, 's'},
        {"cmd1", no_argument, &op_cmd1, 1},
        {"cmd2", no_argument, &op_cmd2, 1},
        {"quit", no_argument, &op_quit, 1},
        {0, 0, 0, 0 }
    };

    int c;
    while(1) {
        int option_index = 0;
        c = getopt_long(argc, argv, "s:h", long_options, &option_index);
        if (c == -1)
            break;
        switch(c) {
            case 's':
                socket = optarg;
                break;
            case 'h':
            case '?':
                show_help();
                return 1;
        }
    }
    
    if ((op_cmd1 + op_cmd2 + op_quit) == 0) {
        fprintf(stderr, "At least one command must be specified\n");
        show_help();
        return 1;
    }

    if (!socket) {
        fprintf(stderr, "No socket specified\n");
        show_help();
        return 1;
    }

    int s = connect_cmdsocket(socket);
    if (s<0) {
        perror("connect_cmdsocket() fail");
        return 1;
    }

    if (op_cmd1)
        ctype = CMD_1;
    else if (op_cmd2)
        ctype = CMD_2;
    else if (op_quit)
        ctype = CMD_QUIT;

    if (ctype == CMD_NOP) {
        show_help();
        goto fail;
    }

    int err = 0;
    switch(ctype) {
        case CMD_1:
            err = send_command(s, ctype, "");
            break;
        case CMD_2:
            while(optind < argc && err == 0) {
                err = send_command(s, ctype, argv[optind++]);
            }
            break;
        case CMD_QUIT:
            err = send_command(s, ctype, "");
            break;
        default:
            err = 1;
    }
    if (err != 0) {
        fprintf(stderr, "send_command() fail\n");
        goto fail;
    }

fail:
    close(s);
    return err;
}
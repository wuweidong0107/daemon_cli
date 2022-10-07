#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>

#include "log.h"
#include "command.h"

int bind_cmdsocket(const char *name)
{
    int cmd_fd;
    struct sockaddr_un addr;

    struct stat sb;
    int err = stat(name, &sb);
    if (!err && S_ISSOCK(sb.st_mode))
        unlink(name);

    if ((cmd_fd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
        log_error("socket() fail");
        return -1;
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    strncpy(addr.sun_path, name, sizeof(addr.sun_path) - 1);
    addr.sun_family = AF_UNIX;
    if (bind(cmd_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        log_error("socket() fail");
        return -1;
    }

    return cmd_fd;
}

int connect_cmdsocket(char *name)
{
    int fd;
    struct sockaddr_un server;

    fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    strcpy(server.sun_path, name);
    server.sun_family = AF_UNIX;

    if (connect(fd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        log_error("connect() fail");
        return -1;
    }
    return fd;
}

int send_command(int cmd_fd, enum command_type type, char *param)
{
    struct command cmd = {
        .type = type,
        .param = {0},
    };

    if (param != NULL) {
        strncpy(cmd.param, param, COMMAND_PARAM_LENGTH),
        cmd.param[COMMAND_PARAM_LENGTH-1] = '\0';
    }
    
    struct iovec v = {
        .iov_base = &cmd,
        .iov_len = sizeof(cmd)
    };
    struct msghdr m = {
        .msg_iov = &v,
        .msg_iovlen = 1,
    };

    int ret = sendmsg(cmd_fd, &m, 0);
    return (ret == -1);
}

struct command *read_command(int cmd_fd)
{
    struct command *cmd = malloc(sizeof(struct command));
    if (!cmd) {
        log_error("malloc() fail");
        return NULL;
    }

    struct iovec v = {
        .iov_base = cmd,
        .iov_len = sizeof(*cmd)
    };
    
    struct msghdr msg = {
        .msg_iov = &v,
        .msg_iovlen = 1,
    };

    int ret = recvmsg(cmd_fd, &msg, 0);
    if (ret == -1) {
        log_error("recvmsg() fail");
        free(cmd);
        return NULL;
    }
    return cmd;
}
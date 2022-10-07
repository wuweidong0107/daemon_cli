#ifndef __CMD_SOCKET_H__
#define __CMD_SOCKET_H__

#include "command.h"

int bind_cmdsocket(char *name);
int connect_cmdsocket(char *name);
int send_command(int cmd_fd, enum command_type type, char *param);
struct command *read_command(int cmd_fd);

#endif
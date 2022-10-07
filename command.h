#ifndef __COMMAND_H__
#define __COMMAND_H__

#define COMMAND_PARAM_LENGTH 256

enum command_type {
    CMD_NOP,
    CMD_1,
    CMD_2,
    CMD_QUIT,
};

struct command {
    enum command_type type;
    char param[COMMAND_PARAM_LENGTH];
};

struct type_name {
	unsigned int type;
	const char *name;
};

int run_command(struct command *cmd);

#endif
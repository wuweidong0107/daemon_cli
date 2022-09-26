#ifndef __COMMAND_H__
#define __COMMAND_H__

#define COMMAND_PARAM_LENGTH 256

enum command_type {
    CMD_1,
    CMD_2,
};

struct command {
    enum command_type type;
    char param[COMMAND_PARAM_LENGTH];
};

#endif
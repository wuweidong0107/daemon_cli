#include <stdlib.h>
#include "daemon.h"
#include "command.h"
#include "log.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#endif

static const struct type_name command_type_names[] = {
	{ CMD_NOP, "NOP" },
	{ CMD_1, "CMD_1" },
	{ CMD_2, "CMD_2" },
	{ CMD_QUIT, "CMD_QUIT" },
};

static const char *util_lookup_type_name(unsigned int type,
					 const struct type_name *table,
					 unsigned int count)
{
	unsigned int i;

	for (i = 0; i < count; i++)
		if (table[i].type == type)
			return table[i].name;

	return NULL;
}

int run_command(struct command *cmd)
{ 
    if (cmd->type == CMD_1) {
        log_info("cmd: %s", util_lookup_type_name(cmd->type, command_type_names,ARRAY_SIZE(command_type_names)));
        return 0;
    }

    if (cmd->type == CMD_2) {
        log_info("cmd=%s, param=%s", 
            util_lookup_type_name(cmd->type, command_type_names,ARRAY_SIZE(command_type_names)),
            cmd->param);
        return 0;
    }

    if (cmd->type == CMD_QUIT) {
        log_info("cmd=%s, param=%s", 
            util_lookup_type_name(cmd->type, command_type_names,ARRAY_SIZE(command_type_names)),
            cmd->param);
        cleanup();
        exit(0);
    }
}
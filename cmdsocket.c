#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int bind_cmdsocket(const char *name)
{
    int cmd_fd;
    struct sockaddr_un addr;

    struct stat sb;
    int err = stat(name, &sb);
    if (!err && S_ISSOCK(sb.st_mode))
        unlink(name);

    if ((cmd_fd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
        perror("socket()");
        return -1;
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    strncpy(addr.sun_path, name, sizeof(addr.sun_path) - 1);
    addr.sun_family = AF_UNIX;
    if (bind(cmd_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1) {
        perror("socket()");
        return -1;
    }

    return cmd_fd;
}
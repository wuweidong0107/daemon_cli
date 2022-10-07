#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "log.h"

#define err_exit(msg)    do { log_error(msg); exit(EXIT_FAILURE); \
                        } while (0)

#define BUF_SIZE 128
#define CPF_CLOEXEC 1

/* Lock a file region using nonblocking F_SETLK */
static int lock_region(int fd, int type, int whence, int start, int len)
{
    struct flock fl;

    fl.l_type = type;
    fl.l_whence = whence;
    fl.l_start = start;
    fl.l_len = len;

    return fcntl(fd, F_SETLK, &fl);
    //return fcntl(fd, F_SETLKW, &fl);      // will block
}

int create_pid_file(const char *prog_name, const char *pid_file, int flags)
{
    int fd;
    char buf[BUF_SIZE];

    fd = open(pid_file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1)
        err_exit("Could not open PID file");

    if (flags & CPF_CLOEXEC) {

        /* Set the close-on-exec file descriptor flag */

        /* Instead of the following steps, we could (on Linux) have opened the
           file with O_CLOEXEC flag. However, not all systems support open()
           O_CLOEXEC (which was standardized only in SUSv4), so instead we use
           fcntl() to set the close-on-exec flag after opening the file */

        flags = fcntl(fd, F_GETFD);                     /* Fetch flags */
        if (flags == -1)
            err_exit("Could not get flags for PID file");

        flags |= FD_CLOEXEC;                            /* Turn on FD_CLOEXEC */

        if (fcntl(fd, F_SETFD, flags) == -1)            /* Update flags */
            err_exit("Could not set flags for PID file");
    }

    if (lock_region(fd, F_WRLCK, SEEK_SET, 0, 0) == -1) {
        if (errno  == EAGAIN || errno == EACCES) {
            log_error("PID file '%s' is locked. %s is already running\n", \
                pid_file, prog_name);
            exit(EXIT_FAILURE);
        } else {
            err_exit("Unable to lock PID file");
        }
    }

    if (ftruncate(fd, 0) == -1)
        err_exit("Could not truncate PID file");

    snprintf(buf, BUF_SIZE, "%ld\n", (long) getpid());
    if (write(fd, buf, strlen(buf)) != strlen(buf)) {
        log_error("Writing to PID file");
        exit(EXIT_FAILURE);
    }

    return fd;
}
#ifndef _ERRNO_H
#define _ERRNO_H

extern int errno;

#define ENOSYS 1	/* No syscall */
#define ENOENT 2	/* No entry   */
#define ESRCH  3        /* No process */
#define EAGAIN 4        /* (busy) Try again */
#define EINTR  5        /* Interrupted system call */
#define EINVAL 6        /* Invalid argument */

#endif /* _ERRNO_H */

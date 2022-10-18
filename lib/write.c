// Copyright 2020 RnD Center "ELVEES", JSC

#include <errno.h>
#include <sys/types.h>

#undef errno
extern int errno;

int _syscall_write(int file, char *ptr, int len);

int write(int file, char *ptr, int len) {
    if (ptr == NULL) {
        errno = EFAULT;
        return -1;
    }
    int ret = _syscall_write(file, ptr, len);
    if (ret < 0) {
        errno = -ret;
        ret = -1;
    } else
        errno = 0;
    return ret;
}

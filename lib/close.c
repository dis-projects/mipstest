// Copyright 2020 RnD Center "ELVEES", JSC

#include <errno.h>

#undef errno
extern int errno;

int _syscall_close(int fildes);

int close(int fildes) {
    int ret = _syscall_close(fildes);
    if (ret < 0) {
        errno = -ret;
        ret = -1;
    } else
        errno = 0;
    return ret;
}

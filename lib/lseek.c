// Copyright 2020 RnD Center "ELVEES", JSC

#include <errno.h>

#undef errno
extern int errno;

int _syscall_lseek(int file, int ptr, int dir);

int lseek(int file, int ptr, int dir) {
    int ret = _syscall_lseek(file, ptr, dir);
    if (ret < 0) {
        errno = -ret;
        ret = -1;
    } else
        errno = 0;
    return ret;
}

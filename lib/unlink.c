// Copyright 2020-2022 RnD Center "ELVEES", JSC

#include <errno.h>
#include <string.h>

#undef errno
extern int errno;

int _syscall_unlink(char *name, int nlen);

int unlink(char *name) {
    if (name == NULL) {
        errno = EFAULT;
        return -1;
    }

    int ret = _syscall_unlink(name, strlen(name));
    if (ret < 0) {
        errno = -ret;
        ret = -1;
    } else
        errno = 0;
    return ret;
}

// Copyright 2020-2022 RnD Center "ELVEES", JSC

#include <errno.h>
#include <string.h>

#undef errno
extern int errno;

int _syscall_link(char *oldpath, char *newpath, int oldlen, int newlen);

int link(char *oldpath, char *newpath) {
    if (oldpath == NULL || newpath == NULL) {
        errno = EFAULT;
        return -1;
    }
    int ret = _syscall_link(oldpath, newpath, strlen(oldpath), strlen(newpath));
    if (ret < 0) {
        errno = -ret;
        ret = -1;
    } else
        errno = 0;
    return ret;
}

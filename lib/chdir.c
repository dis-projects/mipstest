// Copyright 2020-2022 RnD Center "ELVEES", JSC

#include <errno.h>
#include <sys/types.h>
#include <string.h>

#undef errno
extern int errno;

int _syscall_chdir(const char *file, int flen);

int chdir(const char *file) {
    if (file == NULL) {
        errno = EFAULT;
        return -1;
    }
    int ret = _syscall_chdir(file, strlen(file));
    if (ret < 0) {
        errno = -ret;
        ret = -1;
    } else
        errno = 0;
    return ret;
}

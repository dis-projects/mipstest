// Copyright 2020 RnD Center "ELVEES", JSC

#include <errno.h>
#include <sys/time.h>

#include <syscall-runtime.h>

#undef errno
extern int errno;

int _syscall_gettimeofday(timeval_compat *tv);

int gettimeofday(struct timeval *ptimeval, void *ptimezone) {
    if (ptimeval == NULL) {
        errno = EFAULT;
        return -1;
    }
    timeval_compat tv;
    int ret = _syscall_gettimeofday(&tv);
    ptimeval->tv_sec = tv.sec;
    ptimeval->tv_usec = tv.usec;
    if (ret < 0) {
        errno = -ret;
        ret = -1;
    } else
        errno = 0;
    return ret;
}

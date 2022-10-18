// Copyright 2020 RnD Center "ELVEES", JSC

#include <errno.h>
#include <sys/times.h>

#include <syscall-runtime.h>

#undef errno
extern int errno;

int _syscall_times(tms_compat *buf);

clock_t times(struct tms *buf) {
    if (buf == NULL) {
        errno = EFAULT;
        return -1;
    }
    tms_compat tms = {0};
    int ret = _syscall_times(&tms);
    buf->tms_utime = tms.tms_utime;
    buf->tms_stime = tms.tms_stime;
    buf->tms_cutime = tms.tms_cutime;
    buf->tms_cstime = tms.tms_cstime;
    if (ret < 0) {
        errno = -ret;
        ret = (clock_t) -1;
    } else
        errno = 0;
    return (clock_t) ret;
}

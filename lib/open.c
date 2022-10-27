// Copyright 2020-2022 RnD Center "ELVEES", JSC

#include <errno.h>
#include <sys/types.h>
#include <string.h>

#undef errno
extern int errno;

#define CPU_O_CREAT 64
#define CPU_O_EXCL 128
#define CPU_O_NOCTTY 256
#define CPU_O_TRUNC 512
#define CPU_O_APPEND 1024
#define CPU_O_NONBLOCK 2048
#define CPU_O_SYNC 1052672
// TODO: linkage error after fcntl.h is included
#define O_CREAT 512
#define O_EXCL 2048
#define O_NOCTTY 32768
#define O_TRUNC 1024
#define O_APPEND 8
#define O_NONBLOCK 16384
#define O_SYNC 8192

int _syscall_open(char *file, int flags, int mode, size_t flen);

int open(char *file, int flags, int mode) {
    if (file == NULL) {
        errno = EFAULT;
        return -1;
    }
    int cpu_flags = flags;
    if (cpu_flags & O_CREAT) {
        cpu_flags &= ~O_CREAT;
        cpu_flags |= CPU_O_CREAT;
    }
    if (cpu_flags & O_EXCL) {
        cpu_flags &= ~O_EXCL;
        cpu_flags |= CPU_O_EXCL;
    }
    if (cpu_flags & O_NOCTTY) {
        cpu_flags &= ~O_NOCTTY;
        cpu_flags |= CPU_O_NOCTTY;
    }
    if (cpu_flags & O_TRUNC) {
        cpu_flags &= ~O_TRUNC;
        cpu_flags |= CPU_O_TRUNC;
    }
    if (cpu_flags & O_APPEND) {
        cpu_flags &= ~O_APPEND;
        cpu_flags |= CPU_O_APPEND;
    }
    if (cpu_flags & O_NONBLOCK) {
        cpu_flags &= ~O_NONBLOCK;
        cpu_flags |= CPU_O_NONBLOCK;
    }
    if (cpu_flags & O_SYNC) {
        cpu_flags &= ~O_SYNC;
        cpu_flags |= CPU_O_SYNC;
    }

    int ret = _syscall_open(file, cpu_flags, mode, strlen(file));
    if (ret < 0) {
        errno = -ret;
        ret = -1;
    } else
        errno = 0;
    return ret;
}

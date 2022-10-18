// Copyright 2020 RnD Center "ELVEES", JSC

#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include <syscall-runtime.h>

#undef errno
extern int errno;

int _syscall_stat(const char *fildes, stat_compat *st, int flen);

int stat(const char *file, struct stat *st) {
    if (file == NULL || st == NULL) {
        errno = EFAULT;
        return -1;
    }
    //asm volatile("TRL1 %0, R0" ::"r"(strlen(file)));
    stat_compat *st_compat = (stat_compat *) malloc(sizeof(stat_compat));
    int ret = _syscall_stat(file, st_compat, strlen(file));
    if (ret < 0) {
        errno = -ret;
        ret = -1;
    } else
        errno = 0;
    st->st_dev = st_compat->st_dev;
    st->st_ino = st_compat->st_ino;
    st->st_mode = st_compat->st_mode;
    st->st_nlink = st_compat->st_nlink;
    st->st_uid = st_compat->st_uid;
    st->st_gid = st_compat->st_gid;
    st->st_rdev = st_compat->st_rdev;
    st->st_size = st_compat->st_size;
    st->st_blksize = st_compat->st_blksize;
    st->st_blocks = st_compat->st_blocks;
    st->st_atime = st_compat->st_atim;
    st->st_mtime = st_compat->st_mtim;
    st->st_ctime = st_compat->st_ctim;
    free(st_compat);
    return ret;
}

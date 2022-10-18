// Copyright 2020 RnD Center "ELVEES", JSC

#ifndef _SYSCALL_RUNTIME_H_
#define _SYSCALL_RUNTIME_H_

#include <stdint.h>

typedef struct __attribute__((packed)) {
    uint64_t sec;
    uint64_t usec;
} timeval_compat;

typedef struct __attribute__((packed)) {
    int16_t    st_dev;
    uint16_t   st_ino;
    uint32_t   st_mode;
    uint16_t   st_nlink;
    uint16_t   st_uid;
    uint16_t   st_gid;
    int16_t    st_rdev;
    int32_t    st_size;
    int32_t    st_atim;
    int32_t    st_spare1;
    int32_t    st_mtim;
    int32_t    st_spare2;
    int32_t    st_ctim;
    int32_t    st_spare3;
    int32_t    st_blksize;
    int32_t    st_blocks;

    int32_t st_spare4[2];
} stat_compat;

typedef struct __attribute__((packed)) {
    uint64_t tms_utime;
    uint64_t tms_stime;
    uint64_t tms_cutime;
    uint64_t tms_cstime;
} tms_compat;

#endif

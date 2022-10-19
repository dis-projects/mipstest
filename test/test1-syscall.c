// Copyright 2020 RnD Center "ELVEES", JSC

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/time.h>

typedef struct {
    int mode;
    int len;
    int pos;
    int size;
    char *ethalon;
    FILE *file;
    struct stat s;
} file_t;

static uint32_t rand32()
{
    static int seed = 0;
    uint32_t hi, lo;
    hi = (seed = seed * 1103515245 + 12345) >> 16;
    lo = (seed = seed * 1103515245 + 12345) >> 16;
    return (hi << 16) + lo;
}

#define rand rand32

void rnd_fill(int32_t *buf, int len, int str_len) {
    for (int i = 0; i < len; i++)
        buf[i] = (rand() % 26) + 'A';
    for (int i = rand() % str_len; i < len; i += rand() % str_len) buf[i] = '\n';
    buf[len - 1] = '\n';
    buf[len] = 0;
}

int test_syscall(int iCore, int nActions, int nFiles, int nBlocks, int blkSize) {
    printf("info: test1-syscall iCore=%d nActions=%d nFiles=%d nBlocks=%d blkSize=%d\n", iCore,
           nActions, nFiles, nBlocks, blkSize);
    int shift_size = blkSize;
    int file_size = nBlocks * blkSize;
    int ethalon_size = file_size + shift_size;
    char *buf = malloc(blkSize + 1);
    char *ethalon = malloc(ethalon_size + 1);
    int files_size = nFiles * sizeof(file_t);
    file_t *files = malloc(files_size);
    int err = 0;
    if (!ethalon || !files) {
        if (ethalon) free(ethalon);
        if (files) free(files);
        if (buf) free(buf);
        int total_size = ethalon_size + files_size + blkSize;
        printf("error: test1-syscall can't allocate %d bytes\n", total_size);
        return -1;
    }
    rnd_fill((int32_t *)ethalon, (file_size + shift_size) / 4, blkSize);
    memset(files, 0, files_size);
    // random actions
    for (int act_idx = 0; act_idx < nActions; ++act_idx) {
        int iFile = rand() % nFiles;
        file_t *file = files + iFile;
        enum {
            MODE_WRITE = 0,
            MODE_READ = 1,
        };
        enum {
            ACT_TIME = 0,
            ACT_TELL = 1,
            ACT_SEEK = 2,
            ACT_STAT = 3,
            ACT_LAST = 9
        };
        int act = rand() % (ACT_LAST + 1);
        if (act == ACT_TIME) {
            static struct timeval rv = {0};
            static struct timezone rz = {0};
            struct timeval tv;
            struct timezone tz;
            gettimeofday(&tv, &tz);
            if (!rv.tv_sec) {
                rv = tv;
                rz = tz;
            }
            char buf[256];
            char *str = buf;
            int flag = 0;
            str += sprintf(str, " %d %06d", (int)tv.tv_sec, (int)tv.tv_usec);
            if (tv.tv_sec < rv.tv_sec || (tv.tv_sec == rv.tv_sec && tv.tv_usec < rv.tv_usec)) {
                str += sprintf(str, " < %d.%06d", (int)rv.tv_sec, (int)rv.tv_usec);
                ++flag;
            }
            str += sprintf(str, " zone %d %d", (int)tz.tz_minuteswest, (int)tz.tz_dsttime);
            if (tz.tz_minuteswest != tz.tz_minuteswest || tz.tz_dsttime != tz.tz_dsttime) {
                str += sprintf(str, " != %d %d", (int)tz.tz_minuteswest, (int)tz.tz_dsttime);
                ++flag;
            }
            if (flag)
                printf("gettime bad %s, err=%d\n", buf, ++err);
            else
                printf("gettime %s\n", buf);
            rv = tv;
            rz = tz;
        } else if (!file->file) {
            char name[32];
            sprintf(name, "/tmp/core-%d-file-%d", iCore, iFile);
            file->file = fopen(name, file->mode == MODE_WRITE ? "wb" : "rb");
            if (!file->file)
                printf("file %d bad open, err=%d\n", iFile, ++err);
            else if (file->mode == MODE_READ)
                printf("file %d open for read %d bytes\n", iFile, file->size);
            else {
                file->size = rand() % file_size;
                file->ethalon = ethalon + rand() % shift_size;
                printf("file %d open for write %d bytes\n", iFile, file->size);
            }
        } else if (file->len >= file->size) {
            fclose(file->file);
            printf("file %d close\n", iFile);
            file->file = 0;
            file->pos = 0;
            file->len = 0;
            file->mode = (rand() % 3) ? !file->mode : file->mode;
        } else if (act == ACT_TELL) {
            int pos = ftell(file->file);
            if (pos != file->pos)
                printf("file %d bad pos %d must be %d, err=%d\n", iFile, pos, file->pos, ++err);
            else
                printf("file %d position %d\n", iFile, pos);
        } else if (act == ACT_SEEK) {
            int pos = file->len - rand() % blkSize;
            if (pos < 0) pos = 0;
            if (file->mode == MODE_READ) {
                pos += rand() % blkSize;
                if (pos > file->size) pos = file->size;
            }
            int rnd = rand() % 3;
            if (rnd == 0) {
                fseek(file->file, pos, SEEK_SET);
                printf("file %d seek %d set\n", iFile, pos);
            } else if (rnd == 1) {
                int ofs = pos - file->pos;
                fseek(file->file, ofs, SEEK_CUR);
                printf("file %d seek %+d cur\n", iFile, ofs);
            } else {
                int ofs = (file->mode == MODE_READ) ? pos - file->size : pos - file->len;
                fseek(file->file, ofs, SEEK_END);
                printf("file %d seek %+d end\n", iFile, ofs);
            }
            file->pos = pos;
            if (file->len < file->pos) file->len = file->pos;
        } else if (act == ACT_STAT) {
            struct stat s = {0};
            int size = file->size;
            if (file->mode == MODE_WRITE) {
                size = file->len;
                fflush(file->file);
            }
            fstat(fileno(file->file), &s);
            struct stat *p = &(file->s);
            if (!p->st_mode) *p = s;
            char buf[1024];
            char *str = buf;
            int flag = 0;
            str += sprintf(str, " dev %X", (unsigned)s.st_dev);
            if (s.st_dev != p->st_dev) {
                str += sprintf(str, "!=%X", (unsigned)p->st_dev);
                ++flag;
            }
            str += sprintf(str, " ino %X", (unsigned)s.st_ino);
            if (s.st_ino != p->st_ino) {
                str += sprintf(str, "!=%X", (unsigned)p->st_ino);
                ++flag;
            }
            str += sprintf(str, " mode %X", (unsigned)s.st_mode);
            if (s.st_mode != p->st_mode) {
                str += sprintf(str, "!=%X", (unsigned)p->st_mode);
                ++flag;
            }
            str += sprintf(str, " nlink %X", (unsigned)s.st_nlink);
            if (s.st_nlink != p->st_nlink) {
                str += sprintf(str, "!=%X", (unsigned)p->st_nlink);
                ++flag;
            }
            str += sprintf(str, " uid %X", (unsigned)s.st_uid);
            if (s.st_uid != p->st_uid) {
                str += sprintf(str, "!=%X", (unsigned)p->st_uid);
                ++flag;
            }
            str += sprintf(str, " gid %X", (unsigned)s.st_gid);
            if (s.st_gid != p->st_gid) {
                str += sprintf(str, "!=%X", (unsigned)p->st_gid);
                ++flag;
            }
            str += sprintf(str, " rdev %X", (unsigned)s.st_rdev);
            if (s.st_rdev != p->st_rdev) {
                str += sprintf(str, "!=%X", (unsigned)p->st_rdev);
                ++flag;
            }
            str += sprintf(str, " size %X", (unsigned)s.st_size);
            if (s.st_size > size) {
                str += sprintf(str, ">%X", (unsigned)size);
                ++flag;
            }
            str += sprintf(str, " blksize %X", (unsigned)s.st_blksize);
            if (s.st_blksize != p->st_blksize) {
                str += sprintf(str, "!=%X", (unsigned)p->st_blksize);
                ++flag;
            }
            str += sprintf(str, " blocks %X", (unsigned)s.st_blocks);
            int minBlocks = (s.st_size + 511) / 512;
            if (s.st_blocks < minBlocks) {
                str += sprintf(str, "<%X", (unsigned)minBlocks);
                ++flag;
            }
            str += sprintf(str, " atime %X", (unsigned)s.st_atime);
            if (s.st_atime < p->st_atime) {
                str += sprintf(str, "<%X", (unsigned)p->st_atime);
                ++flag;
            }
            str += sprintf(str, " mtime %X", (unsigned)s.st_mtime);
            if (s.st_mtime < p->st_mtime) {
                str += sprintf(str, "<%X", (unsigned)p->st_mtime);
                ++flag;
            }
            str += sprintf(str, " ctime %X", (unsigned)s.st_ctime);
            if (s.st_ctime < p->st_ctime) {
                str += sprintf(str, "<%X", (unsigned)p->st_ctime);
                ++flag;
            }
            if (flag)
                printf("file %d bad stat %s, err=%d\n", iFile, buf, ++err);
            else
                printf("file %d stat (hex)%s \n", iFile, buf);
            *p = s;
        } else {
            int max = file->size - file->pos;
            int len = rand() % blkSize;
            if (len > max) len = max;
            if (file->mode == MODE_WRITE) {
                int res = fwrite(file->ethalon + file->pos, 1, len, file->file);
                if (res != len)
                    printf("file %d bad write %d bytes, must be %d, err=%d\n", iFile, res, len,
                           ++err);
                else
                    printf("file %d write %d bytes\n", iFile, res);
            } else {
                int res = fread(buf, 1, len, file->file);
                if (res != len) {
                    printf("file %d bad read %d bytes, must be %d, err=%d\n", iFile, res, len,
                           ++err);
                    printf("file %d pos=%d len=%d size=%d, err=%d\n", iFile, file->pos, file->len,
                           file->size, err);
                } else if (memcmp(file->ethalon + file->pos, buf, len))
                    printf("file %d read %d bad bytes, err=%d\n", iFile, res, ++err);
                else
                    printf("file %d read %d bytes\n", iFile, res);
            }
            file->pos += len;
            if (file->len < file->pos) file->len = file->pos;
        }
    }
    for (int i = 0; i < nFiles; ++i)
        if (files[i].file) {
            fclose(files[i].file);
            printf("file %d close\n", i);
        }
    free(ethalon);
    free(files);
    free(buf);
    if (err)
        printf("error: test1-syscall return %d\n", err);
    else
        printf("info: test1-syscall return OK\n");
    return err;
}

int main(int32_t argc, char *argv[]) {
    int nActions = 1024;
    int nFiles = 1;
    int nBlocks = 8;
    int blkSize = 8192;
    int iCore = 1;

    return test_syscall(iCore, nActions, nFiles, nBlocks, blkSize);
}

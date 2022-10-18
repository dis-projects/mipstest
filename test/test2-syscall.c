// Copyright 2020 RnD Center "ELVEES", JSC

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>

#include <main.h>
#include <errno.h>

#define get_core_id() 1

int test_syscall(int corenum) {
    char *test_file_name = "/tmp/test-core15.txt";
    char *test_file_link = "/tmp/test-link-core15.txt";
    const char *test_file_data0 = "TEST\n";
    const char *test_file_data1 = "test\n";
    int test_fd = -1;
    struct stat test_stat;
    char test_buf[128];
    struct tms test_tms;
    struct timeval test_timeval;
    struct timezone test_timezone;
    int err = 0; int ret;

    sprintf(test_file_name, "/tmp/test-core%d.txt", corenum);
    sprintf(test_file_link, "/tmp/test-link-core%d.txt", corenum);

    if ((test_fd = open(test_file_name, O_WRONLY | O_CREAT)) == -1)
        printf("core%d: Error while file %s opening: %d\n", corenum, test_file_name, ++err);
    else
        printf("core%d: %s opened with O_WRONLY | O_CREAT %d\n", corenum, test_file_name, test_fd);
    ret = write(test_fd, test_file_data0, strlen(test_file_data0));
    if (ret != strlen(test_file_data0))
        printf("core%d: Error while file %s writing data0: %d : %d != %d\n",
		 corenum, test_file_name, ++err, ret, strlen(test_file_data0));
    if (close(test_fd) != 0)
        printf("core%d: Error while file %s closing: %d\n", corenum, test_file_name, ++err);

    if (stat("AAA", 0) != -1)
        printf("core%d: No errors while stat with nullptr: %d\n", corenum, ++err);
    if (stat(test_file_name, &test_stat) != 0)
        printf("core%d: Error while file %s stat: %d\n", corenum, test_file_name, ++err);
    if (test_stat.st_size != strlen(test_file_data0))
        printf("core%d: Bad file size from stat, got: %d, expected: %d %d\n", corenum,
               test_stat.st_size, strlen(test_file_data0), ++err);

    if (fstat(-1, 0) != -1)
        printf("core%d: No errors while stat with nullptr and fd = -1: %d\n", corenum, ++err);
    if (fstat(1, &test_stat) != 0)
        printf("core%d: Error while stdout fstat: %d\n", corenum, ++err);

    if (open("AAA/bbb", O_WRONLY) != -1)
        printf("core%d: No errors while %s opening: %d\n", corenum, "AAA/bbb", ++err);
    if (open("AAA", O_WRONLY) != -1)
        printf("core%d: No errors while %s opening: %d\n", corenum, "AAA", ++err);

    if (open(test_file_name, O_RDWR | O_CREAT | O_EXCL) != -1)
        printf("core%d: No errors while %s opening with O_RDWR | O_CREAT | O_EXCL: %d\n", corenum,
               "AAA", ++err);
    if ((test_fd = open(test_file_name, O_RDWR | O_CREAT | O_APPEND)) == -1)
        printf("core%d: Error while file %s opening with O_RDWR | O_CREAT | O_APPEND: %d\n",
               corenum, test_file_name, ++err);
    else
        printf("core%d: %s opened with O_RDWR | O_CREAT | O_APPEND\n", corenum, test_file_name);
    if (write(test_fd, test_file_data1, strlen(test_file_data1)) != strlen(test_file_data1))
        printf("core%d: Error while file %s writing data1: %d\n", corenum, test_file_name, ++err);
    if (close(test_fd) != 0)
        printf("core%d: Error while file %s closing: %d\n", corenum, test_file_name, ++err);

    if ((test_fd = open(test_file_name, O_RDWR)) == -1)
        printf("core%d: Error while file %s opening with O_RDWR: %d\n", corenum, test_file_name,
               ++err);
    else
        printf("core%d: %s opened with O_RDWR\n", corenum, test_file_name);
    if (read(test_fd, test_buf, sizeof(test_buf)) == sizeof(test_buf))
        printf("core%d: Error while file %s reading: %d\n", corenum, test_file_name, ++err);
    if (close(test_fd) != 0)
        printf("core%d: Error while file %s closing: %d\n", corenum, test_file_name, ++err);
    if (memcmp(test_buf, test_file_data0, strlen(test_file_data0)) != 0)
        printf("core%d: Error while file data comparing: %d\n", corenum, ++err);
    if (memcmp(test_buf + strlen(test_file_data1), test_file_data1, strlen(test_file_data1)) != 0)
        printf("core%d: Error while file data comparing: %d\n", corenum, ++err);

    if (close(-1) != -1)
        printf("core%d: No errors while opening -1: %d\n", corenum, ++err);

    if ((test_fd = open(test_file_name, O_RDWR | O_CREAT | O_TRUNC)) == -1)
        printf("core%d: Error while %s opening with O_RDWR | O_CREAT | O_TRUNC: %d\n", corenum,
               test_file_name, ++err);
    else
        printf("core%d: %s opened with O_RDWR | O_CREAT | O_TRUNC\n", corenum, test_file_name);
    if (write(test_fd, test_file_data0, strlen(test_file_data0)) != strlen(test_file_data0))
        printf("core%d: Error while file %s writing data0: %d\n", corenum, test_file_name, ++err);
    if (close(test_fd) != 0)
        printf("core%d: Error while file %s closing: %d\n", corenum, test_file_name, ++err);
    if (stat(test_file_name, &test_stat) != 0)
        printf("core%d: Error while file %s stat: %d\n", corenum, test_file_name, ++err);
    if (test_stat.st_size != strlen(test_file_data0))
        printf("core%d: Bad file size from stat, got: %d, expected: %d %d\n", corenum,
               test_stat.st_size, strlen(test_file_data0), ++err);

    if ((test_fd = open(test_file_name, O_RDWR | O_APPEND)) == -1)
        printf("core%d: Error while %s opening with O_RDWR | O_APPEND: %d\n", corenum,
               test_file_name, ++err);
    else
        printf("core%d: %s opened with O_RDWR | O_APPEND\n", corenum, test_file_name);
    if (write(test_fd, test_file_data1, strlen(test_file_data1)) != strlen(test_file_data1))
        printf("core%d: Error while writing to %s: %d\n", corenum, test_file_name, ++err);
    if (close(test_fd) != 0)
        printf("core%d: Error while file %s closing: %d\n", corenum, test_file_name, ++err);
    if ((test_fd = open(test_file_name, O_RDONLY)) == -1)
        printf("core%d: Error while %s opening with O_RDONLY: %d\n", corenum, test_file_name,
               ++err);
    else
        printf("core%d: %s opened with O_RDONLY\n", corenum, test_file_name);
    if (read(test_fd, test_buf, sizeof(test_buf)) == sizeof(test_buf))
        printf("core%d: Error while reading from %s: %d\n", corenum, test_file_name, ++err);
    if (close(test_fd) != 0)
        printf("core%d: Error while file %s closing: %d\n", corenum, test_file_name, ++err);
    if (memcmp(test_buf, test_file_data0, strlen(test_file_data0)) != 0)
        printf("core%d: Error while reading from %s: %d\n", corenum, test_file_name, ++err);
    if (memcmp(test_buf + strlen(test_file_data1), test_file_data1, strlen(test_file_data1)) != 0)
        printf("core%d: Error while reading from %s: %d\n", corenum, test_file_name, ++err);

    if (open(test_file_data0, O_WRONLY) != -1)
        printf("core%d: No errors while %s opening with O_WRONLY: %d\n", corenum, test_file_data0,
               ++err);
    if (write(-1, test_file_data0, strlen(test_file_data0)) != -1)
        printf("core%d: No errors while writing to fd = -1: %d\n", corenum, ++err);
    if (write(1, 0, strlen(test_file_data0)) != -1)
        printf("core%d: No errors while writing to stdout NULL buffer: %d\n", corenum, ++err);
    if (write(1, test_file_data0, strlen(test_file_data0)) != strlen(test_file_data0))
        printf("core%d: No errors while writing to stdout NULL buffer: %d\n", corenum, ++err);
    if (read(-1, 0, 0) != -1)
        printf("core%d: No errors while reading from fd = -1 to NULL buffer: %d\n", corenum,
               ++err);
    if (read(0, 0, 0) != -1)
        printf("core%d: No errors while reading stdin to NULL buffer: %d\n", corenum, ++err);

    if (link(test_file_name, test_file_link) != 0)
        printf("core%d: Error while linking %s to %s: %d\n", corenum, test_file_name,
               test_file_link, ++err);
    if (unlink(test_file_link) != 0)
        printf("core%d: Error while unlinking %s: %d\n", corenum, test_file_link, ++err);
    if (link("AAA", "BBB") != -1)
        printf("core%d: No errors while linking %s to %s: %d\n", corenum, "AAA", "BBB", ++err);
    if (unlink("AAA") != -1)
        printf("core%d: No errors while unlinking %s to %s: %d\n", corenum, "AAA", ++err);

    if (chdir("AAA") != -1) printf("core%d: No errors while chdir to %s: %d\n", corenum, "AAA",
                                   ++err);
    if (chdir("/tmp/test") != 0) printf("core%d: Error while chdir to %s: %d\n", corenum,
                                        "/tmp/test", ++err);
    if (stat(test_file_name, 0) != -1)
        printf("core%d: No errors while stat %s with NULL buffer: %d\n", corenum, test_file_name,
               ++err);
    if (chdir("..") != 0) printf("core%d: Error while chdir to %s: %d\n", corenum, "..", ++err);
    if (stat(test_file_name, 0) != -1)
        printf("core%d: Error while stat %s with NULL buffer: %d\n", corenum, "..", ++err);

    if ((test_fd = open(test_file_name, O_RDONLY)) == -1)
        printf("core%d: Error while %s opening with O_RDONLY: %d\n", corenum, test_file_name,
               ++err);
    if (lseek(-1, 0, SEEK_SET) != -1)
        printf("core%d: No errors while lseek fd = -1: %d\n", corenum, ++err);
    if (lseek(test_fd, 0, 5) != -1)
        printf("core%d: No errors while wrong whence = 5: %d\n", corenum, ++err);
    if (lseek(test_fd, -strlen(test_file_data1), SEEK_END) == -1)
        printf("core%d: Error while lseek %s: %d\n", corenum, test_file_name, ++err);
    if (read(test_fd, test_buf, strlen(test_file_data1)) != strlen(test_file_data1))
        printf("core%d: Error while reading %s: %d\n", corenum, test_file_name, ++err);
    if (memcmp(test_buf, test_file_data1, strlen(test_file_data1)) != 0)
        printf("core%d: Error while reading %s: %d\n", corenum, test_file_name, ++err);
    if (close(test_fd) != 0)
        printf("core%d: Error while closing %s: %d\n", corenum, test_file_name, ++err);

    // Data in the stdin should appear with a delay of at least 10 us since
    // times(2) measures time in tens of microseconds (MCOM03BUP-119).
    scanf("%d\n", &ret);

    if (times(0) == 0)
        printf("core%d: No errors while times with NULL buffer: %d\n", corenum, ++err);
    if (times(&test_tms) == -1) printf("core%d: Error while times: %d %d\n", corenum, ++err, errno);
    if ((test_tms.tms_stime == 0) && (test_tms.tms_utime == 0) && (test_tms.tms_cutime == 0) &&
        (test_tms.tms_cstime == 0))
        printf("core%d: Error while times: %d\n", corenum, ++err);

    if (gettimeofday(0, 0) != -1)
        printf("core%d: Errors while gettimeofday with NULL timeval: %d\n", corenum, ++err);
    if (gettimeofday(&test_timeval, &test_timezone) != 0)
        printf("core%d: Errors while gettimeofday: %d\n", corenum, ++err);
    if (test_timeval.tv_sec == 0)
        printf("core%d: Errors while gettimeofday: %d\n", corenum, ++err);

    if (isatty(-1) == 1)
        printf("core%d: No errors while isatty with fd = -1: %d\n", corenum, ++err);
    if (isatty(1) == -1)
        printf("core%d: Error while isatty with stdin: %d\n", corenum, ++err);

    if (unlink(test_file_name) != 0)
        printf("core%d: Error while unlink %s: %d\n", corenum, test_file_name, ++err);
    if (err)
        printf("core%d: error: test2-syscall return %d\n", corenum, err);
    else
        printf("core%d: info: test2-syscall return OK\n", corenum);
    return err;
}

int main(int32_t argc, char *argv[]) {
    int iCore = get_core_id();

    return test_syscall(iCore);
}

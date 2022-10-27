/* Copyright 2022 RnD Center "ELVEES", JSC */
	.text

	.macro	scall, name, code
	.globl	\name
\name:
	sync
	nop
	syscall \code
	nop
	nop
	j $31
	nop
	.endm

/*__attribute__((naked)) void _syscall_gettimeofday() {  // (struct timeval *tv, struct timezone *tz)*/
	scall	_syscall_gettimeofday,1
/*__attribute__((naked)) void _syscall_write() {  // (int fd, void *buf, size_t cout)*/
	scall	_syscall_write,2
/*__attribute__((naked)) void _syscall_read() {  // (int fd, void *buf, size_t cout)*/
	scall	_syscall_read,3
/*__attribute__((naked)) void _syscall_open() {  // (char *filename, int flags, int mode)*/
	scall	_syscall_open,4
/*__attribute__((naked)) void _syscall_close() {  // (int file)*/
	scall	_syscall_close,5
/*__attribute__((naked)) void _syscall_fstat() {  // (int file, struct stat *st)*/
	scall	_syscall_fstat,6
/*__attribute__((naked)) void _syscall_lseek() {  // (int file, int offset, int dir)*/
	scall	_syscall_lseek,7
/*__attribute__((naked)) void _syscall_isatty() {  // (int file)*/
	scall	_syscall_isatty,8
/*__attribute__((naked)) void _syscall_chdir() {  // (const char *path)*/
	scall	_syscall_chdir,9
/*__attribute__((naked)) void _syscall_stat() {  // (const char *filename, struct stat *buf)*/
	scall	_syscall_stat,10
/*__attribute__((naked)) void _syscall_times() {  // (struct tms *buf)*/
	scall	_syscall_times,11
/*__attribute__((naked)) void _syscall_link() {  // (const char *oldpath, const char *newpath)*/
	scall	_syscall_link,12
/*__attribute__((naked)) void _syscall_unlink() {  // (const char *pathname)*/
	scall	_syscall_unlink,13
/*__attribute__((naked)) void _syscall_exit() {  // (int rc)*/
	scall	_syscall_exit,99
/*__attribute__((naked)) void _syscall_get_env() {  // (char *, uint32_t *)*/
	scall	_syscall_get_env,100






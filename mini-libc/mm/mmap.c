// SPDX-License-Identifier: BSD-3-Clause

#include <sys/mman.h>
#include <errno.h>
#include <internal/syscall.h>

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
	/* TODO: Implement mmap(). */
	long ret = syscall(9, addr, length, prot, flags, fd, offset);
	if(ret < 0) {
		errno = -ret;
		ret = -1;
	}
	return (void*)ret;
}

void *mremap(void *old_address, size_t old_size, size_t new_size, int flags)
{
	/* TODO: Implement mremap(). */
	return (void*)syscall(25, old_address, old_size, new_size, flags);
}

int munmap(void *addr, size_t length)
{
	/* TODO: Implement munmap(). */
	return syscall(11, addr, length);
}

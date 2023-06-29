// SPDX-License-Identifier: BSD-3-Clause

#include <sys/stat.h>
#include <errno.h>

int fstat(int fd, struct stat *st)
{
	/* TODO: Implement fstat(). */
	int ret = syscall(5,fd,st);
	if (ret < 0) {
		errno = -ret;
		ret = -1;
	}
	return ret;
}

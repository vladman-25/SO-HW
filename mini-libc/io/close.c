// SPDX-License-Identifier: BSD-3-Clause

#include <unistd.h>
#include <internal/syscall.h>
#include <stdarg.h>
#include <errno.h>

int close(int fd)
{
	/* TODO: Implement close(). */
	int ret = syscall(3,fd);
	if (ret < 0) {
		errno = -ret;
		ret = -1;
	}
	return ret;
}

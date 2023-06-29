// SPDX-License-Identifier: BSD-3-Clause

#include <fcntl.h>
#include <internal/syscall.h>
#include <stdarg.h>
#include <errno.h>

int open(const char *filename, int flags, ...)
{
	/* TODO: Implement open system call. */
	va_list args;
	va_start(args, flags);
	int p;
	if ((flags & O_CREAT)) {
		p = va_arg(args, int);
	}
	int res = syscall(2, filename, flags, p);
	va_end(args);

	if (res < 0) {
		errno = -res;
		res = -1;
	}

	return res;
}

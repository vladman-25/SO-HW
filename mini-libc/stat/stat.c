// SPDX-License-Identifier: BSD-3-Clause

#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int stat(const char *restrict path, struct stat *restrict buf)
{
	/* TODO: Implement stat(). */
	int ret = syscall(4,path,buf);
	if (ret < 0) {
		errno = -ret;
		ret = -1;
	}
	return ret;
}

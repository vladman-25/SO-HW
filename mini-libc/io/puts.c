/* SPDX-License-Identifier: BSD-3-Clause */

#include <fcntl.h>
#include <internal/syscall.h>
#include <stdarg.h>
#include <errno.h>


int puts(const char *str) {


    char *s = str;
    size_t i = 0;
	for (; *s != '\0'; s++, i++)
		;
    int ret = write(1, str, i);
    int ret2 = write(1, "\n", 1);
    return ret;
}
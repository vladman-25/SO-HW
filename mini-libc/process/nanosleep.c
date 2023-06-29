#include <internal/syscall.h>
#include <stdlib.h>
#include <time.h>

int nanosleep(const struct timespec *req, struct timespec *rem) {
    int ret = syscall(35,req,rem);
    return ret;
}
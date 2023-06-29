#include <internal/syscall.h>
#include <stdlib.h>
#include <time.h>


unsigned int sleep(unsigned int seconds) {

    struct timespec wait = {seconds, 0};
	int ret = nanosleep(&wait, NULL);
    return ret;
}
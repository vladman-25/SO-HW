#ifndef __TIME_H__
#define __TIME_H__	1

#ifdef __cplusplus
extern "C" {
#endif


struct timespec {
               long tv_sec;        /* seconds */
               long   tv_nsec;       /* nanoseconds */
           };
unsigned int sleep(unsigned int seconds);
int nanosleep(const struct timespec *req, struct timespec *rem);
#ifdef __cplusplus
}
#endif

#endif
#pragma once

/*
include windows before this
*/
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/* Windows doesn't have gettimeofday. While Cygwin and some
 * versions of MinGW supply one, it is very coarse. This substitute
 * gives much more accurate elapsed times under Windows. */
static void win_gettimeofday(struct timeval* p, void* tz /* IGNORED */)
{
    LARGE_INTEGER q;
    static long long freq;
    static long long cyg_timer;
    QueryPerformanceFrequency(&q);
    freq = q.QuadPart;
    QueryPerformanceCounter(&q);
    cyg_timer = q.QuadPart;
    p->tv_sec = (long)(cyg_timer / freq);
    p->tv_usec = (long)(((cyg_timer % freq) * 1000000) / freq);
}


#ifdef __cplusplus
} // "C"
#endif // __cplusplus

#define gettimeofday win_gettimeofday

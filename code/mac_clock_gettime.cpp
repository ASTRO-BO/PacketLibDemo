/***************************************************************************
                          clock_time cross plattform
                             -------------------
    begin                : Sept 2013
    copyright            : (C) 2013 by Andrea Bulgarelli
    email                : bulgarelli@iasfbo.inaf.it
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software for non commercial purpose              *
 *   and for public research institutes; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License.          *
 *   For commercial purpose see appropriate license terms                  *
 *                                                                         *
 ***************************************************************************/

#include "mac_clock_gettime.h"

//clock_gettime is not implemented on OSX
#ifdef __MACH__
int clock_gettime(int clk_id, struct timespec* t) {
    struct timeval now;
    int rv = gettimeofday(&now, NULL);
    if (rv) return rv;
    t->tv_sec  = now.tv_sec;
    t->tv_nsec = now.tv_usec * 1000;
    return 0;
}
#endif

double timediff(struct timespec start, struct timespec stop){
	double secs = (stop.tv_sec - start.tv_sec) + (stop.tv_nsec - start.tv_nsec) / (double)NANO;
	return secs;
}

long gettimensec() {
	struct timespec start;
	clock_gettime( CLOCK_MONOTONIC, &start);
	double nsecs =  start.tv_sec * NANO +  start.tv_nsec;
	return (long) nsecs;
}


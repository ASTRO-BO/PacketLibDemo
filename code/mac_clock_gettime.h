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

#ifndef _MACCLOCKGETTIME_H
#define _MACCLOCKGETTIME_H

#define NANO 1000000000L

#ifdef __MACH__
#include <sys/time.h>
#define CLOCK_REALTIME 0
#define CLOCK_MONOTONIC 0
//clock_gettime is not implemented on OSX
int clock_gettime(int clk_id, struct timespec* t);
#else
#include <time.h>
#endif

double timediff(struct timespec start, struct timespec stop);

long gettimensec();

#endif

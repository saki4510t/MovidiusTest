/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef INTERNAL_H_
#define INTERNAL_H_

#include <unistd.h>
#include <inttypes.h>
#include <cinttypes>
//#include <endian.h>
//#include <asm/byteorder.h>
#include "endian_unaligned.h"
#include "Mutex.h"
#include "Condition.h"
#include "Timers.h"

using namespace android;

#ifndef TIMESPEC_TO_TIMEVAL
#define TIMESPEC_TO_TIMEVAL(tv, ts)           \
	{                                         \
		(tv)->tv_sec = (ts)->tv_sec;          \
		(tv)->tv_usec = (ts)->tv_nsec / 1000; \
	}
#endif

#if defined(__LP64__)
#define FMT_SIZE_T "lu"
#else
#define FMT_SIZE_T "u"
#endif

#endif /* INTERNAL_H_ */

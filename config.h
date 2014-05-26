/* This file is part of lsf_watchdog.
 *
 * (C) 2014 Alexander Haase <alexander.haase@rwth-aachen.de>
 */

#ifndef lsf_watchdog_config_h
#define lsf_watchdog_config_h

#include <lsf/lsf.h>


// flag for verbose output
extern bool verbose;

// search filter for lsf_openjobinfo
typedef struct lsf_filter_ {
	char *host;
	LS_LONG_INT job_id;
	char *job_name;
	char *user;
	char *queue;
} lsf_filter_t;


#endif

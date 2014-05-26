/* This file is part of lsf_watchdog.
 *
 * (C) 2014 Alexander Haase <alexander.haase@rwth-aachen.de>
 */

#include <stdlib.h>
#include <lsf/lsbatch.h>

#include "lsf_helper.h"


/* lsb_str2jobid (char *str)
 *
 * function to translate a cstring to a LSB_JOBID
 */
LS_LONG_INT lsb_str2jobid (char *str) {
	char *p;
	for (p = str; *p; p++) {
		if (*p == '[') {
			p++;
			break;
		}
	}

	return LSB_JOBID(atoi(str), atoi(p));
}

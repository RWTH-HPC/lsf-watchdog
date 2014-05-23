/* This file is part of lsf_watchdog.
 *
 * (C) 2014 Alexander Haase <alexander.haase@rwth-aachen.de>
 */

#ifndef lsf_watchdog_config_h
#define lsf_watchdog_config_h

#include <lsf/lsf.h>

// struct for configuration
typedef struct lsf_watchdog_config_ {
	bool verbose;

	const char *check_dir;

	struct filter {
		char *host;
		LS_LONG_INT job_id;
		char *job_name; 
		char *user;
		char *queue;
	} filter;

	bool print_job_conf;
	bool print_job_script;
	bool wait_after_match_for_enter;
} lsf_watchdog_config_t;


// function to parse cmd-options
lsf_watchdog_config_t config_parse_cmd (int argc, char **argv);


#endif

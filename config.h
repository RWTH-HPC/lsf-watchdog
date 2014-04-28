/* This file is part of lsf_watchdog.
 *
 * (C) 2014 Alexander Haase <alexander.haase@rwth-aachen.de>
 */

#include <lsf/lsf.h>


// struct for configuration
struct config {
	bool verbose;

	char *check_dir;

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
};



// function to parse cmd-options
struct config config_parse_cmd (int argc, char **argv);

/* This file is part of lsf_watchdog.
 *
 * (C) 2014 Alexander Haase <alexander.haase@rwth-aachen.de>
 */

#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#include <lsf/lsf.h>
#include <lsf/lsbatch.h>

#include "config.h"
#include "lsf_helper.h"


/* config_parse_cmd
 *
 * function to parse command-line options.
 */
lsf_watchdog_config_t config_parse_cmd (int argc, char **argv) {
	lsf_watchdog_config_t conf = {
		false,
		"./checks",
		{
			NULL,
			0,
			NULL,
			ALL_USERS,
			NULL
		},
		false,
		false,
		false
	};

	// long options
	static struct option long_options[] = {
		// filter options
		{"host", required_argument, 0, 'h'},
		{"jobid", required_argument, 0, 'j'},
		{"jobname", required_argument, 0, 'J'},
		{"queue", required_argument, 0, 'q'},
		{"user", required_argument, 0, 'u'},

		// display options
		{"verbose", no_argument, 0, 'v'},
		{"wait", no_argument, 0, 'w'},
		{"print-jobconf", no_argument, 0, 'c'},
		{"print-jobscript", no_argument, 0, 's'},

		// configuration
		{"checkdir", required_argument, 0, 'd'}
	};

	int option_index = 0;
	while (true) {
		int c = getopt_long (argc, argv, "h:j:J:q:u:d:vwcs", long_options, &option_index);

		// abort, if all arguments were passed
		if (c < 0) break;

		// evaluate option
		switch (c) {
			// filter options
			case 'h': conf.filter.host = optarg; break;
			case 'j': conf.filter.job_id = lsb_str2jobid(optarg); break;
			case 'J': conf.filter.job_name = optarg; break;
			case 'q': conf.filter.queue = optarg; break;
			case 'u': conf.filter.user = optarg; break;

			// display options
			case 'v': conf.verbose = true; break;
			case 'w': conf.wait_after_match_for_enter = true; break;
			case 'c': conf.print_job_conf = true; break;
			case 's': conf.print_job_script = true; break;

			// configuration
			case 'd': conf.check_dir = optarg; break;

			// exit if unknown flag (getopt already printed an error message)
			case '?':
			default:
				exit(EXIT_FAILURE);
		}
	}

	return conf;
}

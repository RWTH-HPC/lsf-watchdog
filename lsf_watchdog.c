/* This file is part of lsf_watchdog.
 *
 * (C) 2014 Alexander Haase <alexander.haase@rwth-aachen.de>
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <lsf/lsbatch.h>
#include <lsf/lsf.h>

#include "config.h"
#include "lsf_helper.h"
#include "check_scripts.h"


// verbose flag will be only declared here
bool verbose;

int main (int argc, char **argv) {
	/*
	 * read config from comand line
	 */

	// default settings
	verbose = false;
	script_list_t *check_scripts = NULL;
	lsf_filter_t job_filter = {
		NULL,
		0,
		NULL,
		(char*) ALL_USERS,
		NULL
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

		// check files
		{"checkdir", required_argument, 0, 'd'},
		{"check", required_argument, 0, 'c'}
	};

	int option_index = 0;
	while (true) {
		int c = getopt_long (argc, argv, "c:d:h:j:J:q:u:v", long_options, &option_index);

		// abort, if all arguments were passed
		if (c < 0) break;

		// evaluate option
		switch (c) {
			// filter options
			case 'h': job_filter.host = optarg; break;
			case 'j': job_filter.job_id = lsb_str2jobid(optarg); break;
			case 'J': job_filter.job_name = optarg; break;
			case 'q': job_filter.queue = optarg; break;
			case 'u': job_filter.user = optarg; break;

			// display options
			case 'v': verbose = true; break;

			// configuration
			case 'c':
				if (!search_check_script(optarg, &check_scripts)) {
					fprintf(stderr, "error while looking adding check-script '%s'!\n", optarg);
					exit(EXIT_FAILURE);
				}
				break;

			case 'd':
				if (!search_check_script_dir(optarg, &check_scripts)) {
					fprintf(stderr, "error while looking for check-scripts in '%s'!\n", optarg);
					exit(EXIT_FAILURE);
				}
				break;

			// exit if unknown flag (getopt already printed an error message)
			case '?':
			default:
				exit(EXIT_FAILURE);
		}
	}

	/*
	 * search for all checks
	 */
	if (!check_scripts) {
		fprintf(stderr, "no check-scripts found!\n");
		exit(EXIT_FAILURE);
	}

	// init connection to mbatchd
	lsb_init(argv[0]);

	// search for all jobs
	if (lsb_openjobinfo(job_filter.job_id, job_filter.job_name, job_filter.user, job_filter.queue, job_filter.host, CUR_JOB) < 0) {
		fprintf(stderr, "No jobs found. Search pattern: jobid=%s, jobname=%s, opt_user=%s, queue=%s, host=\"%s\"\n", lsb_jobid2str(job_filter.job_id), job_filter.job_name, job_filter.user, job_filter.queue, job_filter.host);
		exit(EXIT_FAILURE);
	}

	// check jobs
	int more;
	struct jobInfoEnt *job;
	while ((job = lsb_readjobinfo(&more))) {
		script_list_t *check = check_scripts;
		while (check) {
			if (system(check->filename)) {
				printf("%s:\t%s failed\n", lsb_jobid2str(job_filter.job_id), check->filename);
			}
			check = check->next;
		}
	}

	// close connection to mbatchd
	lsb_closejobinfo();
}

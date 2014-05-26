/* This file is part of lsf_watchdog.
 *
 * (C) 2014 Alexander Haase <alexander.haase@rwth-aachen.de>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <lsf/lsbatch.h>
#include <lsf/lsf.h>

#include "config.h"
#include "lsf_helper.h"
#include "check_scripts.h"

#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>


// colors for check output
#define ANSI_COLOR_RED		"\x1b[31m"
#define ANSI_COLOR_GREEN	"\x1b[32m"
#define ANSI_COLOR_YELLOW	"\x1b[33m"
#define ANSI_COLOR_CYAN		"\x1b[36m"
#define ANSI_COLOR_RESET	"\x1b[0m"

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
	bool check_scripts_set = false;

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
		{"check", required_argument, 0, 'c'},
		{"checkdir", required_argument, 0, 'd'}
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
				if (!check_scripts_add_file(optarg, &check_scripts)) {
					fprintf(stderr, "error while looking adding check-script '%s'!\n", optarg);
					exit(EXIT_FAILURE);
				}
				check_scripts_set = true;
				break;

			case 'd':
				if (!check_scripts_add_dir(optarg, &check_scripts)) {
					fprintf(stderr, "error while looking for check-scripts in '%s'!\n", optarg);
					exit(EXIT_FAILURE);
				}
				check_scripts_set = true;
				break;

			// exit if unknown flag (getopt already printed an error message)
			case '?':
			default:
				exit(EXIT_FAILURE);
		}
	}

	// search for check-scripts in default directory, if no other dir set via cmd
	if (!check_scripts_set) {
		if (!check_scripts_add_dir("./checks", &check_scripts)) {
			fprintf(stderr, "error while looking for check-scripts in '%s'!\n", optarg);
			exit(EXIT_FAILURE);
		}
	}

	// print warning, if no checks were found
	if (!check_scripts) {
		fprintf(stderr, "no check-scripts found!\n");
		exit(EXIT_FAILURE);
	}



	/*
	 * search for all jobs at mbatchd, that match our filter and check this
	 * jobs with our check-scripts
	 */

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
	static const char padder[] = "................................................";
	while ((job = lsb_readjobinfo(&more))) {
		if (verbose)
			printf("checking %s:\n", lsb_jobid2str(job->jobId));
	
		/*
		 * set environment for checks
		 */
		setenv("LSF_WATCHDOG_JOBID", lsb_jobid2str(job->jobId), true);
		setenv("LSF_WATCHDOG_USER", job->user, true);

		// Job status - only running & pending jobs will be checked!
		switch (job->status) {
			case JOB_STAT_PEND: setenv("LSF_WATCHDOG_STATUS", "PENDING", true); break;
			case JOB_STAT_RUN: setenv("LSF_WATCHDOG_STATUS", "RUNNING", true); break;
			
			// PSUSP, SSUSP, USUSP will be ignored
			default:
				if (verbose)
					printf("skipped: States was %d\n", job->status);
					continue;
		}

		char buffer [40];
		sprintf(buffer, "%d", (int) job->startTime);
		setenv("LSF_WATCHDOG_START_TIME", buffer, true);

		sprintf(buffer, "%d", job->submit.numProcessors);
		setenv("LSF_WATCHDOG_PROCESSORS_MIN", buffer, true);

		sprintf(buffer, "%d", job->submit.maxNumProcessors);
		setenv("LSF_WATCHDOG_PROCESSORS_MAX", buffer, true);

		sprintf(buffer, "%d", job->submit.rLimits[RLIMIT_RSS]);
		setenv("LSF_WATCHDOG_MEM_PER_SLOT", buffer, true);


		/*
		 * run all check-scripts for this job
		 */
		script_list_t *check = check_scripts;
		while (check) {
			if (verbose)
				printf("\t%s %*.s ", check->filename, (int) (sizeof(padder) - strlen(check->filename) - 1), padder);

			int ret = system(check->filename);
			switch (WEXITSTATUS(ret)) {
				case 0:
					if (verbose) printf(ANSI_COLOR_GREEN "passed" ANSI_COLOR_RESET "\n");
					break;
				
				case 1:
					if (verbose) printf(ANSI_COLOR_RED "failed" ANSI_COLOR_RESET "\n");
					else printf("%s:\t%s " ANSI_COLOR_RED "failed" ANSI_COLOR_RESET "\n", lsb_jobid2str(job->jobId), check->filename);
					break;

				case 255:
					if (verbose) printf(ANSI_COLOR_YELLOW "skipped" ANSI_COLOR_RESET "\n");
					break;
					
				default:
					printf(ANSI_COLOR_CYAN "unknown" ANSI_COLOR_RESET " (%d)\n", WEXITSTATUS(ret));
					break;
			}

			check = check->next;
		}
	}

	// close connection to mbatchd
	lsb_closejobinfo();
}

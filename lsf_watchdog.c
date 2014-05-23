/* This file is part of lsf_watchdog.
 *
 * (C) 2014 Alexander Haase <alexander.haase@rwth-aachen.de>
 */

#include <stdio.h>
#include <stdlib.h>

#include <lsf/lsbatch.h>

#include "config.h"
#include "check_scripts.h"


int main (int argc, char **argv) {
	// parse command line arguments
	lsf_watchdog_config_t conf = config_parse_cmd(argc, argv);

	// search for all checks
	struct script_list *check_scripts = search_check_scripts(conf.check_dir);
	if (!check_scripts) {
		fprintf(stderr, "error while looking for check-scripts!\n");
		exit(EXIT_FAILURE);
	}

	// print filenames, if verbose enabled
	if (conf.verbose) {
		printf("the following check-scripts were found:\n");

		struct script_list *iter = check_scripts;
		while (iter) {
			printf("\t%s\n", iter->filename);
			iter = iter->next;
		}
	}

	// init connection to mbatchd
	lsb_init(argv[0]);

	// search for all jobs
	if (lsb_openjobinfo(conf.filter.job_id, conf.filter.job_name, conf.filter.user, conf.filter.queue, conf.filter.host, CUR_JOB) < 0) {
		fprintf(stderr, "No jobs found. Search pattern: jobid=%s, jobname=%s, opt_user=%s, queue=%s, host=\"%s\"\n", lsb_jobid2str(conf.filter.job_id), conf.filter.job_name, conf.filter.user, conf.filter.queue, conf.filter.host);
		exit(EXIT_FAILURE);
	}

	// check jobs
	int more;
	struct jobInfoEnt *job;
	while ((job = lsb_readjobinfo(&more))) {
		// if (job->status != JOB_STAT_RUN) continue;
		// printf("%s\n", lsb_jobid2str(job->jobId));

		struct script_list *check = check_scripts;
		while (check) {
			if (system(check->filename)) {
				printf("%s:\t%s failed\n", lsb_jobid2str(conf.filter.job_id), check->filename);
			}
			check = check->next;
		}
	}

	// close connection to mbatchd
	lsb_closejobinfo();
}

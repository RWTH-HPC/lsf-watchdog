# LSF-Watchdog
*Application for LSF-Administrators and -Users to optimize the management of their job-resources* 

## About
-------
lsf-watchdog is a command-line application, which fetches all currently submitted Jobs of your local LSF-environment and executes tests on the resource-requirements of the job.

As check-script there could be any executable that is able to handle environment variables and throw an exit-code. So there could be any test you like to implement in a language you prefer to use.

Every failed test will be printed to stdout with JOB-ID and name of the failed check, so you are able to look for that job.


## Options
You are able to use the following cmd-options:

#### Filter options:

| short option  | long option   | meaning |
| ------------- | ------------- | ------- |
| -h [hostlist] | --host=[hostlist] | Check only jobs on specified hosts. If more than one host should be matched, separate them by comma. |
| -j [jobid] | --jobid=[jobid] | Check only jobs with specified Job-ID. |
| -J [jobname] | --jobname=[jobname] | Check only jobs with specified Jobname. |
| -q [queue] | --queue=[queue] | Check only jobs in specified queue. |
| -u [user] | --user=[user] | Check only jobs from specified user. |

#### Check-Files:

| short option  | long option   | meaning |
| ------------- | ------------- | ------- |
| -c [path] | --check=[path] | Add executable [path] to list of check-scripts. |
| -d [path] | --checkdir=[path] | Search recursive in [path] for check-scripts. |
*If neither -c or -d is set, check-scripts will be searched in ./checks*

#### Display options:

| short option  | long option   | meaning |
| ------------- | ------------- | ------- |
| -v | --verbose | Print verbose Output. |



## Check-Scripts
You are able to use any executable as check-scripts, that is able to read environment variables and set an exit code.

You could use the following environment variables:

| name  | value |
| ------------- | ------------- |
| LSF_WATCHDOG_JOBID | ID of JOB, including array-index, if it is an array-job |
| LSF_WATCHDOG_USER | Owner of the job |
| LSF_WATCHDOG_STATUS | Current job-status: {PENDING|RUNNING} |
| LSF_WATCHDOG_START_TIME | Unix-timestamp, when job started execution |
| LSF_WATCHDOG_PROCESSORS_MIN | Minimum number of requested processors |
| LSF_WATCHDOG_PROCESSORS_MAX | Maximum number of requested processors |
| LSF_WATCHDOG_MEM_PER_SLOT | Memory that was requested per slot |


You should use the following exit-codes:

| exit-code  | meaning |
| ---------- | ------- |
| 0 | Your check passed without any problems |
| 1 | Your check failed |
| 255 | Your check was skipped (e.g. only running jobs will be checked by this script and the actual jub-status is PENDING) |

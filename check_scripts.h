/* This file is part of lsf_watchdog.
 *
 * (C) 2014 Alexander Haase <alexander.haase@rwth-aachen.de>
 */

#ifndef lsf_watchdog_check_scripts_h
#define lsf_watchdog_check_scripts_h


// struct for list of executables for checks
typedef struct script_list {
	const char *filename;
	struct script_list *next;
} script_list_t;


// function to append a new string to list
void script_list_push (script_list_t **list, const char *filename);

// function to add a single binary to list
bool check_scripts_add_file (const char *path, script_list_t **list);

// function to search recusive for all executables in a givven directory
bool check_scripts_add_dir (const char *path, script_list_t **list);


#endif

/* This file is part of lsf_watchdog.
 *
 * (C) 2014 Alexander Haase <alexander.haase@rwth-aachen.de>
 */


// struct for list of executables for checks
struct script_list {
	char *filename;
	struct script_list *next;
};

// function to append a new string to list
struct script_list * script_list_push (struct script_list *list, char *filename);

// function to merge two lists
struct script_list * script_list_merge (struct script_list *a, struct script_list *b);

// function to search recusive for all executables in a givven directory
struct script_list * search_check_scripts (const char *dir);
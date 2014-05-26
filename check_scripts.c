/* This file is part of lsf_watchdog.
 *
 * (C) 2014 Alexander Haase <alexander.haase@rwth-aachen.de>
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#include "check_scripts.h"
#include "config.h"


/* script_list_push
 *
 * function to add a new filename to script_list
 * returns a pointer to the beginning of the whole list
 */
void script_list_push (script_list_t **list, const char *filename) {
	// ignore empty filename
	if (!filename) return;

	// add a new element in front of the list
	script_list_t *temp = new script_list_t;
	temp->filename = filename;
	temp->next = *list;
	*list = temp;
}


/* check_scripts_add_file
 *
 * Function to add a executeable to list.
 * Executeable will be prepended to list.
 */
bool check_scripts_add_file (const char *path, script_list_t **list) {
	// get information about filename
	struct stat st;
	if (lstat(path, &st) >= 0) {
		if (!S_ISDIR(st.st_mode) && ((st.st_mode & S_IEXEC) != 0)) {
			if (verbose)
				printf("added check-script: %s\n", path);

			// push file to list
			script_list_push(list, path);

			return true;
		}
	}

	return false;
}


/* check_scripts_add_dir
 *
 * Function to search for all executeables recursive in a given directory.
 * Found checks will be prepended to list.
 */
bool check_scripts_add_dir (const char *path, script_list_t **list) {
	if (verbose)
		printf("searching for check-scripts in '%s'\n", path);

	struct dirent **files;
	int n;
	if ((n = scandir(path, &files, 0, alphasort)) < 0)
		return false;

	while (n--) {
		// ignore hidden files
		if (files[n]->d_name[0] != '.') {
			// prepare filename
			char *filename = new char[strlen(path) + strlen(files[n]->d_name) + 2];
			sprintf(filename, "%s/%s", path, files[n]->d_name);

			// get information about filename
			struct stat st;
			if (lstat(filename, &st) < 0)
				return false;

			// go recursive into directorys
			if (S_ISDIR(st.st_mode))
				check_scripts_add_dir(filename, list);

			// add executables to our list
			else if ((st.st_mode & S_IEXEC) != 0) {
				if (verbose)
					printf("added check-script: %s\n", filename);

				// push file to list
				script_list_push(list, filename);
			}
		}

		// free memory
		free(files[n]);
	}

	// free memory
	free(files);

	return true;
}

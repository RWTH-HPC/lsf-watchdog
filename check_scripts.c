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


/* script_list_push
 *
 * function to add a new filename to script_list
 * returns a pointer to the beginning of the whole list
 */
struct script_list * script_list_push (struct script_list *list, char *filename) {
	// ignore empty strings
	if (!filename) return list;

	// add a new element in front of the list
	struct script_list *temp = new script_list;
	temp->filename = filename;
	temp->next = list;

	return temp;
}


/* script_list_merge
 *
 * function to merge two script_lists
 * returns a pointer to the beginning of the whole list
 */
struct script_list * script_list_merge (struct script_list *a, struct script_list *b) {
	// do nothing if a or b is empty
	if (!a) return b;
	if (!b) return a;

	// go to end of a and concat b
	struct script_list *iter = a;
	while ((iter = iter->next));
	iter->next = b;

	return a;
}


/* search_check_scripts
 *
 * function to search for all executeables in a givven directory
 * returns a list of all found executeables
 */
struct script_list * search_check_scripts (const char *dir) {
	struct dirent **files;
	int n;
	if ((n = scandir(dir, &files, 0, 0)) < 0)
		return NULL;

	struct script_list *temp = NULL;
	while (n--) {
		// ignore hidden files
		if (files[n]->d_name[0] != '.') {
			// prepare filename
			int l = strlen(dir) + strlen(files[n]->d_name) + 2;
			char *filename = new char[l];
			sprintf(filename, "%s/%s", dir, files[n]->d_name);

			// get information about filename
			struct stat st;
			if (lstat(filename, &st) < 0)
				return NULL;

			// go recursive into directorys
			if (S_ISDIR(st.st_mode))
				temp = script_list_merge(search_check_scripts(filename), temp);

			// add executables to our list
			else if ((st.st_mode & S_IEXEC) != 0)
				temp = script_list_push(temp, filename);
		}

		// free memory
		free(files[n]);
	}

	// free memory
	free(files);

	return temp;
}

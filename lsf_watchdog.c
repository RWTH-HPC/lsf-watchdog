/* This file is part of lsf_watchdog.
 *
 * (C) 2014 Alexander Haase <alexander.haase@rwth-aachen.de>
 */

#include <stdio.h>

#include "config.h"

int main (int argc, char **argv) {
	struct config conf = config_parse_cmd(argc, argv);

	if (conf.verbose) printf("verbose\n");
}
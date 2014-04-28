/* This file is part of lsf_watchdog.
 *
 * (C) 2014 Alexander Haase <alexander.haase@rwth-aachen.de>
 */

#include <getopt.h>

#include "config.h"


/* config_parse_cmd
 *
 * function to parse command-line options.
 */
struct config config_parse_cmd (int argc, char **argv) {
	struct config conf;
	conf.verbose = true;

	return conf;
}
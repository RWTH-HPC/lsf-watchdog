/* This file is part of lsf_watchdog.
 *
 * (C) 2014 Alexander Haase <alexander.haase@rwth-aachen.de>
 */

// struct for configuration
struct config {
	bool verbose;
};



// function to parse cmd-options
struct config config_parse_cmd (int argc, char **argv);
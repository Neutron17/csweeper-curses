#include "args.h"
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "common.h"
#include "errors.h"

struct Args parseArgs(int argc, char *argv[]) {
	struct Args ret = { 10, 10, 7, false };
	int choice;
	while (1) {
		struct option long_options[] = {
			/* Use flags like so:
			{"verbose",	no_argument,	&verbose_flag, 'V'}*/
			/* Argument styles: no_argument, required_argument, optional_argument */
			{"version",	no_argument,		0,	'V'},
			{"debug",	no_argument,		0,	'd'},
			{"help",	no_argument,		0,	'H'},
			{"width",	required_argument,	0,	'w'},
			{"height",	required_argument,	0,	'h'},
			{"mines",	required_argument,	0,	'm'},

			{0,0,0,0}
		};

		int option_index = 0;

		/* Argument parameters:
			no_argument: " "
			required_argument: ":"
			optional_argument: "::" */

		choice = getopt_long( argc, argv, "VdHw:h:m:",
					long_options, &option_index);

		if (choice == -1)
			break;

		switch( choice ) {
			case 'V':
				printf("Version: %d.%d", VERSION_MAJOR, VERSION_MINOR);
				break;

			case 'H':
				printf(
					"Minesweeper\n"
					"\t-V: version\n"
					"\t-d: debug\n"
					"\t-H: help\n"
					"\t-w: width\n"
					"\t-h: height\n"
				);
				break;
			case 'd':
				ret.isDebug = true;
				break;
			case 'w':
				ret.w = strtoul(optarg, NULL, 10);
				if(errno == EINVAL || errno == ERANGE) {
					fprintf(stderr, "ERROR: Couldn't convert width to a number, '%s'", strerror(errno));
					exit(E_ARG);
				}
				break;
			case 'h':
				ret.h = strtoul(optarg, NULL, 10);
				if(errno == EINVAL || errno == ERANGE) {
					fprintf(stderr, "ERROR: Couldn't convert height to a number, '%s'", strerror(errno));
					exit(E_ARG);
				}
				break;
			case 'm':
				ret.m = strtoul(optarg, NULL, 10);
				if(errno == EINVAL || errno == ERANGE) {
					fprintf(stderr, "ERROR: Couldn't convert height to a number, '%s'", strerror(errno));
					exit(E_ARG);
				}
				break;
			case '?':
				/* getopt_long will have already printed an error */
				exit(E_UNKNOWN);
				break;

			default:
				/* Not sure how to get here... */
				return (struct Args){0};
		}
	}

	/* Deal with non-option arguments here */
	if ( optind < argc ) {
		while ( optind < argc ) {
		}
	}
	return ret;
}

#ifndef _NTR_ARG_H_
#define _NTR_ARG_H_ 1
#include <stdbool.h>
#include <stdint.h>

struct Args {
	uint8_t w, h;
	uint8_t m;
	bool isDebug;
};

struct Args parseArgs(int argc, char *argv[]);

#endif//_NTR_ARG_H_


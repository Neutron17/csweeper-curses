#include <bits/strcasecmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "args.h"
#include "errors.h"
#include "common.h"

#define MINE 'X'

typedef struct {
	uint8_t w, h;
	char *fields;
} Board_t;

enum CmdType {
	C_QUIT,
	C_HELP,
	C_NONE,
	C_ERR,
	C_UNKNOWN
};

Board_t createBoard(uint8_t w, uint8_t h);
void destroyBoard(Board_t *board);

// Board manipulation
char getField(Board_t board, uint8_t w, uint8_t h);
char *getField_ref(const Board_t *board, uint8_t w, uint8_t h);
void setFieldAt(Board_t *board, uint8_t w, uint8_t h, char v);

void printBoard(Board_t board);
void spawnMines(Board_t *board, size_t count);
int countNeighbours(Board_t board, uint8_t x, uint8_t y);
void clearBoard(Board_t *board);
enum CmdType parseInput(const char *input);
/*
 * returns: true: fail, false: success
*/
bool handleInput(enum CmdType type);

const char *cmdHelp =
	"Minesweeper - command help\n"
	"\th,?: help\n"
	"\tq: quit\n";
bool isDebug = false;


/* TODO
 * fox double printig "> " with invalid input
 *
*/

int main(int argc, char *argv[]) {
	srand(time(NULL));
	struct Args args = parseArgs(argc, argv);
	isDebug = args.isDebug;

	Board_t board = createBoard(args.w, args.h);

	spawnMines(&board, args.m);

	for(int y = 0; y < board.h; y++) {
		for(int x = 0; x < board.w; x++) {
			if(getField(board, x, y) == MINE)
				continue;
			if(countNeighbours(board, x, y) > 0) {
				setFieldAt(&board, x, y, '0' + countNeighbours(board, x, y));
			}
		}
	}
	bool running = true;
	char inpBuff[16];
	while(running) {
		printBoard(board);
inp:;
    		printf("> ");
		fflush(stdout);
		//int c = fgetc(stdin);
		if(fgets(inpBuff, 15, stdin) == NULL) {
			fprintf(stderr, "ERROR: Error while getting input\n");
			goto cleanUp;
		}
		enum CmdType type = parseInput(inpBuff);
		switch(type) {
			case C_QUIT:
				running = false;
				break;
			default:
				if(handleInput(type))
					goto inp;
				break;
		}
	}
cleanUp:;
	if(isDebug)
		puts("DEBUG: Cleaning up");
	destroyBoard(&board);
	if(isDebug)
		puts("DEBUG: Exiting");
	return E_SUCC;
}

bool handleInput(enum CmdType type) {
	switch(type) {
		case C_HELP:
			printf("%s", cmdHelp);
			break;
		case C_ERR:
			/*Unreachable*/
			fprintf(stderr, "ERROR: error while getting input\n");
			break;
		case C_NONE:
			return true;
			break;
		case C_UNKNOWN:
			fprintf(stderr, "ERROR: Unknown command\n");
			printf("%s", cmdHelp);
			return true;
			break;
		case C_QUIT:
			// Unreachable
			break;
	}
	return false;
}

enum CmdType parseInput(const char *input) {
	if(input == NULL)
		return C_ERR;
	char b[16];
	strncpy(b, input, 16);
	char *buff = &b[0];
	int shift = 0;

	while(isspace(*buff)) {
		buff++;
		shift++;
	}
	if(*buff == '\0' || strnlen(buff, 16) < 2)
		return C_NONE;
	if(strnlen(buff, 16) == 2) {
		switch(*buff) {
			case 'q':
				return C_QUIT;
			case '?':
			case'h':
				return C_HELP;
			/*case EOF:
				return C_ERR;*/
			default:
				return C_UNKNOWN;
		}
	} else {
		if(strncasecmp(buff, "help", 4) == 0) {
			return C_HELP;
		} else if (strncasecmp(buff, "quit", 4) == 0) {
			return C_QUIT;
		} else if (strncasecmp(buff, "exit", 4) == 0) {
			return C_QUIT;
		} else {
			return C_UNKNOWN;
		}
	}
	return C_ERR;
}

void spawnMines(Board_t *board, size_t count) {
	if(count > (board->h*board->w))
		return;
	for(size_t i = 0; i < count; i++) {
up:;
		int rnd = rand() % (board->h * board->w);
		if(board->fields[rnd] == MINE)
			goto up;
		board->fields[rnd] = MINE;
	}
}

int countNeighbours(Board_t board, uint8_t x, uint8_t y) {
	int count = 0;
	if(x < board.w) {
		if(getField(board, x+1, y) == MINE) count++;
		if(y < board.h)
			if(getField(board, x+1, y+1) == MINE) count++;
		if(y > 0)
			if(getField(board, x+1, y-1) == MINE) count++;
	}
	if(x > 0) {
		if(getField(board, x-1, y) == MINE) count++;
		if(y < board.h)
			if(getField(board, x-1, y+1) == MINE) count++;
		if(y > 0)
			if(getField(board, x-1, y-1) == MINE) count++;
	}
	if(y < board.h)
		if(getField(board, x, y+1) == MINE) count++;
	if(y > 0)
		if(getField(board, x, y-1) == MINE) count++;
	return count;
}

Board_t createBoard(uint8_t w, uint8_t h) {
	Board_t ret = {
		w, h,
		(char *) calloc(sizeof(char), w * h)
	};
	if(!ret.fields) {
		fprintf(stderr, "ERROR: Couldn't allocate for board, '%s'\n",
				strerror(errno));
		exit(E_ALLOC);
	}
	memset(ret.fields, ' ', w*h);
	return ret;
}

void destroyBoard(Board_t *board) {
	free(board->fields);
	board->fields = NULL;
}

char getField(Board_t board, uint8_t w, uint8_t h) {
	if(w >= board.w || h >= board.h)
		return ' ';
	return board.fields[h * board.w + w];
}
char *getField_ref(const Board_t *board, uint8_t w, uint8_t h) {
	return &board->fields[h * board->w + w];
}
void setFieldAt(Board_t *board, uint8_t w, uint8_t h, char v) {
	if(w >= board->w || h >= board->h)
		return;
	board->fields[h * board->w + w] = v;
}

void printBoard(Board_t board) {
	for(int i = 0; i < board.w + 2; i++)
		printf("-");
	puts("");
	for(int y = 0; y < board.h; y++) {
		printf("|");
		for(int x = 0; x < board.w; x++) {
			printf("%c", getField(board, x, y));
		}
		puts("|");
	}
	for(int i = 0; i < board.w + 2; i++)
		printf("-");
	puts("");
}


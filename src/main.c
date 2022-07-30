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

#include <ncurses.h>

#define MINE 'X'
#define CLOSED 'C'

typedef struct {
	uint8_t value;
	bool isMine;
	bool isOpen;
} Cell;

typedef struct {
	uint8_t w, h;
	int cursorPtr;
	Cell *cells;
	//char *cells;
} Board;

enum CmdType {
	C_QUIT,
	C_HELP,
	C_NONE,
	C_ERR,
	C_UNKNOWN,

	C_UP,
	C_DOWN,
	C_LEFT,
	C_RIGHT,
	C_SELECT
};

Board createBoard(uint8_t w, uint8_t h);
void destroyBoard(Board *board);

// Board manipulation
Cell getCell(Board board, uint8_t x, uint8_t y);
Cell getCell_checked(Board board, uint8_t x, uint8_t y);
Cell *getCell_ref(const Board *board, uint8_t x, uint8_t y);
void setCell_checked(Board *board, uint8_t x, uint8_t y, Cell v);
void setCell(Board *board, uint8_t x, uint8_t y, Cell v);

void printBoard(Board board);
void spawnMines(Board *board, size_t count);
int countNeighbours(Board board, uint8_t x, uint8_t y);
void clearBoard(Board *board);
enum CmdType parseInput(char input);
/* returns: true: fail, false: success */
bool handleInput(enum CmdType type, Board *board);

const char *cmdHelp =
	"Minesweeper - command help\n"
	"\th,?: help\n"
	"\tq: quit\n";
bool isDebug = false;


/* TODO
 * fox double printing "> " with invalid input
 *
*/

int main(int argc, char *argv[]) {

	srand(time(NULL));
	struct Args args = parseArgs(argc, argv);
	isDebug = args.isDebug;

	Board board = createBoard(args.w, args.h);

	spawnMines(&board, args.m);

	for(int y = 0; y < board.h; y++) {
		for(int x = 0; x < board.w; x++) {
			if(getCell(board, x, y).isMine)
				continue;
			if(countNeighbours(board, x, y) > 0) {
				setCell(&board, x, y,
					(Cell) { countNeighbours(board, x, y), false, true }
				);
			}
		}
	}
	bool running = true;
	char input;

	initscr();
	cbreak();
	clear();
	while(running) {
		clear();
		printBoard(board);
inp:;
		input = getch();
		//int c = fgetc(stdin);

		enum CmdType type = parseInput(input);
		switch(type) {
			case C_QUIT:
				running = false;
				break;
			case C_SELECT:
				if(board.cells[board.cursorPtr].isOpen) {
					if(board.cells[board.cursorPtr].isMine) {
						printw("GAME OVER, press any key\n");
						running = false;
					}
				} else { // not open
					board.cells[board.cursorPtr].isOpen = true;
					if(board.cells[board.cursorPtr].isMine) {
						printw("GAME OVER, press any key\n");
						running = false;
					}
				}
				break;
			default:
				if(handleInput(type, &board))
					goto inp;
				break;
		}
		refresh();
	}
	getch();
	endwin();
cleanUp:;
	if(isDebug)
		puts("DEBUG: Cleaning up");
	destroyBoard(&board);
	if(isDebug)
		puts("DEBUG: Exiting");
	return E_SUCC;
}

bool handleInput(enum CmdType type, Board *board) {
	switch(type) {
		case C_HELP:
			printw("%s", cmdHelp);
			break;
		case C_ERR:
			printw("ERROR: error while getting input\n");
			break;
		case C_NONE:
			return true;
			break;
		case C_UNKNOWN:
			printw("ERROR: Unknown command\n");
			printw("%s", cmdHelp);
			return true;
			break;

		case C_UP:
			board->cursorPtr -= board->w;
			break;
		case C_DOWN:
			board->cursorPtr += board->w;
			break;
		case C_LEFT:
			board->cursorPtr--;
			break;
		case C_RIGHT:
			board->cursorPtr++;
			break;

		case C_SELECT:
		case C_QUIT:
			// Unreachable
			break;
	}
	return false;
}

enum CmdType parseInput(char input) {
	if(input == '\n')
		return C_SELECT;

	if(input == '\0')
		return C_NONE;
	switch(input) {
		case 'q':
			return C_QUIT;
		case '?':
		case'h':
			return C_HELP;
		case 'w':
			return C_UP;
		case 'a':
			return C_LEFT;
		case 's':
			return C_DOWN;
		case 'd':
			return C_RIGHT;
		/*case EOF:
			return C_ERR;*/
		default:
			return C_UNKNOWN;
	}
	return C_ERR;
}

void spawnMines(Board *board, size_t count) {
	if(count > (board->h*board->w))
		return;
	for(size_t i = 0; i < count; i++) {
up:;
		int rnd = rand() % (board->h * board->w);
		if(board->cells[rnd].isMine)
			goto up;
		board->cells[rnd].isMine = true;
	}
}

int countNeighbours(Board board, uint8_t x, uint8_t y) {
	int count = 0;
	if(x < board.w) {
		if(getCell_checked(board, x+1, y).isMine) count++;
		if(y < board.h)
			if(getCell_checked(board, x+1, y+1).isMine) count++;
		if(y > 0)
			if(getCell_checked(board, x+1, y-1).isMine) count++;
	}
	if(x > 0) {
		if(getCell(board, x-1, y).isMine) count++;
		if(y < board.h)
			if(getCell(board, x-1, y+1).isMine) count++;
		if(y > 0)
			if(getCell(board, x-1, y-1).isMine) count++;
	}
	if(y < board.h)
		if(getCell(board, x, y+1).isMine) count++;
	if(y > 0)
		if(getCell(board, x, y-1).isMine) count++;
	return count;
}

Board createBoard(uint8_t w, uint8_t h) {
	Board ret = {
		w, h,
		0,
		(Cell *) calloc(sizeof(Cell), w * h)
	};
	if(!ret.cells) {
		fprintf(stderr, "ERROR: Couldn't allocate for board, '%s'\n",
				strerror(errno));
		exit(E_ALLOC);
	}
	//memset(ret.cells, (Cell){ 0, false, false }, w*h);
	return ret;
}

void destroyBoard(Board *board) {
	free(board->cells);
	board->cells = NULL;
}

Cell getCell_checked(Board board, uint8_t x, uint8_t y) {
	if(x >= board.w || y >= board.h)
		return (Cell){ 0,false,false };
	return board.cells[y * board.w + x];
}
Cell getCell(Board board, uint8_t x, uint8_t y) {
	return board.cells[y * board.w + x];
}
Cell *getCell_ref(const Board *board, uint8_t x, uint8_t y) {
	return &board->cells[y * board->w + x];
}
void setCell_checked(Board *board, uint8_t x, uint8_t y, Cell v) {
	if(x >= board->w || y >= board->h)
		return;
	board->cells[y * board->w + x] = v;
}
void setCell(Board *board, uint8_t x, uint8_t y, Cell v) {
	board->cells[y * board->w + x] = v;
}

void printBoard(Board board) {
	for(int i = 0; i < board.w + 1; i++)
		mvaddch(0, i, '-');
	printf("\n");
	for(int y = 1; y < board.h; y++) {
		mvaddch(y, 0,'|');
		for(int x = 1; x < board.w; x++) {
			if(((y-1)*board.w + x-1) == board.cursorPtr) {
				mvaddch(y,x,'P');
				continue;
			}
			Cell c = getCell(board, x-1, y-1);
			if(c.isMine) {
				mvaddch(y,x,MINE);
			} else if(!c.isOpen) {
				mvaddch(y,x,CLOSED);
			} else if(c.value == 0) {
				mvaddch(y,x,' ');
			} else {
				mvaddch(y,x, '0' + c.value);
			}
			//printw("%c", getCell(board, x, y).value);
		}
		mvaddch(y,board.w,'|');

	}
	for(int i = 0; i < board.w + 1; i++)
		mvaddch(board.h, i, '-');
	mvaddch(board.h, board.w+1, '\r');
	mvaddch(board.h, board.w+2, '\n');
}


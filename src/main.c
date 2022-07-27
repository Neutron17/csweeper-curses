#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#define MINE 'X'

typedef struct {
	uint8_t w, h;
	char *fields;
	bool isValid;
} Board_t;

Board_t createBoard(uint8_t w, uint8_t h);
void destroyBoard(Board_t *board);

// Board manipulation
char getFieldAt(Board_t board, uint8_t w, uint8_t h);
char *getFieldAt_ref(const Board_t *board, uint8_t w, uint8_t h);
void setFieldAt(Board_t *board, uint8_t w, uint8_t h, char v);

void printBoard(Board_t board);
void spawnMines(Board_t *board, size_t count);
int countNeighbours(Board_t board, uint8_t x, uint8_t y);

int main(int argc, char *argv[]) {
	srand(time(NULL));
	Board_t board = createBoard(10, 10);

	spawnMines(&board, 7);

	for(int y = 0; y < board.h; y++) {
		for(int x = 0; x < board.w; x++) {
			if(getFieldAt(board, x, y) == MINE)
				continue;
			if(countNeighbours(board, x, y) > 0) {
				setFieldAt(&board, x, y, '0' + countNeighbours(board, x, y));
			}
		}
	}
	printBoard(board);

	destroyBoard(&board);
	return 0;
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
		if(getFieldAt(board, x+1, y) == MINE) count++;
		if(y < board.h)
			if(getFieldAt(board, x+1, y+1) == MINE) count++;
		if(y > 0)
			if(getFieldAt(board, x+1, y-1) == MINE) count++;
	}
	if(x > 0) {
		if(getFieldAt(board, x-1, y) == MINE) count++;
		if(y < board.h)
			if(getFieldAt(board, x-1, y+1) == MINE) count++;
		if(y > 0)
			if(getFieldAt(board, x-1, y-1) == MINE) count++;
	}
	if(y < board.h)
		if(getFieldAt(board, x, y+1) == MINE) count++;
	if(y > 0)
		if(getFieldAt(board, x, y-1) == MINE) count++;
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
		exit(-1);
	}
	memset(ret.fields, ' ', w*h);
	ret.isValid = true;
	return ret;
}

void destroyBoard(Board_t *board) {
	free(board->fields);
	board->fields = NULL;
	board->isValid = false;
}

char getFieldAt(Board_t board, uint8_t w, uint8_t h) {
	return board.fields[h * board.w + w];
}
char *getFieldAt_ref(const Board_t *board, uint8_t w, uint8_t h) {
	return &board->fields[h * board->w + w];
}
void setFieldAt(Board_t *board, uint8_t w, uint8_t h, char v) {
	board->fields[h * board->w + w] = v;
}
void printBoard(Board_t board) {
	for(int i = 0; i < board.w + 2; i++)
		printf("-");
	puts("");
	for(int y = 0; y < board.h; y++) {
		printf("|");
		for(int x = 0; x < board.w; x++) {
			printf("%c", getFieldAt(board, x, y));
		}
		puts("|");
	}
	for(int i = 0; i < board.w + 2; i++)
		printf("-");
	puts("");
}


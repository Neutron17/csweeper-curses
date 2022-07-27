CC = clang

PNAME = c

SRC  = $(shell find src -name "*.c")
OBJ  = $(SRC:.c=.o)
BIN = build

EXEC = $(BIN)/$(PNAME)
INCFLAGS  = -Isrc/

CCFLAGS += $(INCFLAGS)
CCFLAGS += -O0
CCFLAGS += -Wall
CCFLAGS += -pedantic

LDFLAGS  = $(INCFLAGS)
LDFLAGS += -lm

INSTALL_PATH = /usr/local/bin

all: build

run: build
	$(BIN)/mine $*

build: $(OBJ)
	$(CC) $(CCFLAGS) -ggdb -o $(BIN)/mine $(filter %.o,$^) $(LDFLAGS)

clean:
	rm $(BIN)/* $(OBJ)

install: build
	cp build/mine $(INSTALL_PATH)

%.o: %.c
	$(CC) -ggdb -o $@ -c $< $(CCFLAGS)


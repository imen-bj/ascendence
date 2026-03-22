CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lSDL -lSDL_image -lSDL_ttf -lm

SRC = mainG.c sourceG.c gameplay.c
OBJ = $(SRC:.c=.o)
EXEC = game

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

run: $(EXEC)
	./$(EXEC)

clean:
	rm -f $(OBJ) $(EXEC)

.PHONY: all clean run 

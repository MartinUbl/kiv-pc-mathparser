CC = gcc
CFLAGS = -Wextra -Wall -pedantic -ansi
BIN = graph
OBJ = drawing.o main.o postscript.o rpn.o shunting_yard.o stack.o test.o
LIBS = -lm

%.o: %.c
	$(CC) -c $(CFLAGS) $(LIBS) $< -o $@

$(BIN): $(OBJ)
	$(CC) $(LIBS) $^ -o $@

clean:
	rm -rf *.o
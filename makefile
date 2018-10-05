# Which compiler
CC = gcc

# Options for development
CFLAGS = -g -std=gnu11 -Wall # -Wextra -Werror -Wmissing-declarations \
 -Wmissing-prototypes -Werror-implicit-function-declaration -Wreturn-type \
 -Wparentheses -Wunused -Wold-style-definition -Wundef -Wshadow \
 -Wstrict-prototypes -Wswitch-default -Wunreachable-code

OBJ = parser.o mish.o execute.o

#make program
all:mish

mish: parser.o mish.o execute.o
	$(CC) $(OBJ) -o mish

mish.o: mish.c parser.h execute.h
	$(CC) $(CFLAGS) mish.c -c

execute.o: execute.c execute.h
	$(CC) $(CFLAGS) execute.c -c

parser.o: parser.c parser.h
	$(CC) $(CFLAGS) parser.c -c

#Other options
.PHONY: clean valgrind nemiver

clean:
	rm -f $(OBJ)

valgrind: all
	valgrind --leak-check=full --track-origins=yes ./mish

nemiver: all
	nemiver ./mish
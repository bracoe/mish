# The makefile used to compile mish
#
# Created on: 5 Oct 2018
#     Author: Bram Coenen (tfy15bcn)
#

# Which compiler
CC = gcc

# Options for development
CFLAGS = -g -std=gnu11 -Wall -Wextra -Werror -Wmissing-declarations \
 -Wmissing-prototypes -Werror-implicit-function-declaration -Wreturn-type \
 -Wparentheses -Wunused -Wold-style-definition -Wundef -Wshadow \
 -Wstrict-prototypes -Wswitch-default -Wunreachable-code

OBJ = parser.o mish.o execute.o list.o sighant.o

#make program
all:mish

mish: $(OBJ)
	$(CC) $(OBJ) -o mish

mish.o: mish.c parser.h execute.h list.h sighant.h
	$(CC) $(CFLAGS) mish.c -c

execute.o: execute.c execute.h
	$(CC) $(CFLAGS) execute.c -c

parser.o: parser.c parser.h
	$(CC) $(CFLAGS) parser.c -c
	
list.o: list.c list.h
	$(CC) $(CFLAGS) list.c -c
	
sighant.o: sighant.c sighant.h list.h
	$(CC) $(CFLAGS) sighant.c -c

#Other options
.PHONY: clean valgrind

clean:
	rm -f $(OBJ)

valgrind: all
	valgrind --leak-check=full --track-origins=yes ./mish
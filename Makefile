
CFLAGS = --std=c99 -ggdb -Wall -Werror -I.
LDFLAGS = -ggdb
OBJECTS = parser.o evaluator.o printer.o repl.o expressions.o functions.o \
  context.o alist.o higher.o

main: main.o $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $< $(OBJECTS)

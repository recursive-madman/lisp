
CFLAGS = --std=c99 -ggdb -Wall -Werror -I.
LDFLAGS = -ggdb -lreadline
OBJECTS = parser.o evaluator.o printer.o repl.o expressions.o functions.o \
  context.o alist.o higher.o

lisp: main.o $(OBJECTS)
	$(CC) -o $@ $< $(OBJECTS) $(LDFLAGS)

clean:
	rm -f lisp *.o *~

.PHONY: clean


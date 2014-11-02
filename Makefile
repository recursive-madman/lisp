
CFLAGS = --std=c99 -ggdb -Wall -Werror -I.
LDFLAGS = -ggdb -lreadline
OBJECTS = parser.o evaluator.o printer.o repl.o expressions.o functions.o \
  context.o alist.o higher.o

lisp: main.o $(OBJECTS)
	$(CC) -o $@ $< $(OBJECTS) $(LDFLAGS)

clean:
	rm -f lisp *.o *~ test/*.o test/*~

test: clean test/parser test/evaluator
	@test/parser
	@test/evaluator
	@rm -f test/{parser,evaluator}

test/parser: test/parser-test.o $(OBJECTS)
	$(CC) -o $@ $< $(OBJECTS) $(LDFLAGS)

test/evaluator: test/evaluator-test.o $(OBJECTS)
	$(CC) -o $@ $< $(OBJECTS) $(LDFLAGS)

.PHONY: clean


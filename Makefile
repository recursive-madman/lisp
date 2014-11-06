
CFLAGS = --std=c99 -ggdb -Wall -Werror -I.
LDFLAGS = -ggdb -lreadline -lm
OBJECTS = parser.o evaluator.o printer.o repl.o expressions.o functions.o \
  context.o alist.o higher.o symbol_table.o

lisp: main.o $(OBJECTS)
	$(CC) -o $@ $< $(OBJECTS) $(LDFLAGS)

clean:
	rm -f lisp *.o *~ test/*.o test/*~

test: clean test/parser test/evaluator test/symbol_table
	@test/parser
	@test/evaluator
	@test/symbol_table
	@rm -f test/{parser,evaluator}

test/%: test/%-test.o $(OBJECTS)
	$(CC) -o $@ $< $(OBJECTS) $(LDFLAGS)

.PHONY: clean


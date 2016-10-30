OBJS = interpreter.o parser.o environment.o lexer.o lexeme.o types.o list.o evaluator.o

flannel: $(OBJS)
	gcc -Wall $(OBJS) -o flannel

interpreter.o: interpreter.c lexeme.h lexer.h parser.h environment.h evaluator.h list.h types.h
	gcc -Wall -c interpreter.c

evaluator.o: evaluator.c lexeme.h environment.h types.h list.h
	gcc -Wall -c evaluator.c

parser.o: parser.c lexeme.h types.h lexer.h environment.h list.h
	gcc -Wall -c parser.c

environment.o: environment.c lexeme.h types.h list.h
	gcc -Wall -c environment.c

lexer.o: lexer.c lexeme.h types.h
	gcc -Wall -c lexer.c

lexeme.o: lexeme.c lexeme.h types.h
	gcc -Wall -c lexeme.c

list.o: list.c lexeme.h
	gcc -Wall -c list.c

types.o: types.c
	gcc -Wall -c types.c

clean:
	rm -rf *.o flannel


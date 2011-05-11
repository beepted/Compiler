all: compile

compile: lexer.o parser.o symboltable.o ast.o codegen.o frontend.o 
	  gcc -g -lm lexer.o parser.o symboltable.o ast.o codegen.o frontend.o -o compile

lexer.o: lexer.c
	 gcc -g -c lexer.c

parser.o: parser.c
	  gcc -g -c parser.c

symboltable.o: symboltable.c
	  gcc -g -c symboltable.c

ast.o: ast.c
	gcc -g -c ast.c

codegen.o: codegen.c
	gcc -g -c codegen.c

frontend.o: frontend.c
	    gcc -g -c frontend.c

clean: 
	rm -rf *o compile

deepclean:
	rm -rf *o compile
	rm -rf parsingtable
	rm -rf *.c.*

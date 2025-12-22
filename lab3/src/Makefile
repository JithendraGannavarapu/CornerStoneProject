all: parser

parser: lex.yy.c parser.tab.c ast.c main.c
	gcc -o parser lex.yy.c parser.tab.c ast.c main.c -lfl

lex.yy.c: lexer.l
	flex lexer.l

parser.tab.c: parser.y
	bison -d parser.y

clean:
	rm -f parser lex.yy.c parser.tab.c parser.tab.h
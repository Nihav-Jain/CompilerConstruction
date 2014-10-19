all: driver
	gcc lexer.o parser.o driver.o -o stage1exe

driver: lexer.o parser.o driver.o

driver.o: driver.c
	gcc -c driver.c

lexer.o: lexer.c
	gcc -c lexer.c

parser.o: parser.c
	gcc -c parser.c


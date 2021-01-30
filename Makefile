all: lang

lang.tab.cc lang.tab.hh:	lang.yy compiler.hpp assembler.hpp
	bison -t -v -d lang.yy

lex.yy.c: lang.l lang.tab.hh
	flex lang.l

lang: lex.yy.c lang.tab.cc lang.tab.hh
	g++ -std=c++17 -fmax-errors=1 -g -O0 -o lang lang.tab.cc lex.yy.c

mars:
	java -jar Mars4_5.jar t0 a.asm

clean:
	rm lang lang.tab.cc lex.yy.c lang.tab.hh lang.output

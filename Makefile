all: lang

lang.tab.cc lang.tab.hh:	lang.yy
	bison -t -v -d lang.yy

lex.yy.c: lang.l lang.tab.hh
	flex lang.l

lang: lex.yy.c lang.tab.cc lang.tab.hh include/helpers.hpp include/compiler.hpp src/compiler.cpp include/assembler.hpp
	clang++ -std=c++17 -fmax-errors=1 -g -O0 -Iinclude -o lang src/compiler.cpp lang.tab.cc lex.yy.c && clear && ./lang < code.txt

compile:
	./lang < code.txt

mars:
	java -jar Mars4_5.jar nc dec dump .data BinaryText mem.txt a.asm

mem: mars
	clear && cat -n mem.txt | head -10

run: compile mars mem

clean:
	rm lang lang.tab.cc lex.yy.c lang.tab.hh lang.output

gcc -c ../ast_pretty_printer.c ../lex.yy.c ../driver.c -g
g++ -c cfg.cpp
g++ ast_pretty_printer.o lex.yy.o driver.o cfg.o -o cfg

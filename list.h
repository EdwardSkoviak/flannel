// list.h
// written by Edward 'Cole' Skoviak
//
// List data structure

extern lexeme* cons(char*,lexeme*,lexeme*);
extern lexeme* car(lexeme*);
extern lexeme* setCar(lexeme*,lexeme*);
extern lexeme* cdr(lexeme*);
extern lexeme* setCdr(lexeme*);

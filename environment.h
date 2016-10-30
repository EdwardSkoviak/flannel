// environment.h
// written by Edward 'Cole' Skoviak
//
// Environment functions

extern lexeme * createEnv();
extern lexeme * extendEnv(lexeme *, lexeme *, lexeme *);
extern lexeme * defineVariable(lexeme*,lexeme*,lexeme*);
extern lexeme * findVariable(lexeme*,lexeme*);
extern lexeme * updateVariable(lexeme*,lexeme*,lexeme*);

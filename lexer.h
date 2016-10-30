// lexer.h
// written by Edward 'Cole' Skoviak (ecskoviak@crimson.ua.edu)
//
// Lexical Analyzer for reading and tokenizing input

extern int line;
extern void lexInit();
extern lexeme* lex();
extern FILE *file;

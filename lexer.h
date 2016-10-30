// lexer.h
// written by Edward 'Cole' Skoviak
//
// Lexical Analyzer for reading and tokenizing input

extern int line;
extern void lexInit();
extern lexeme* lex();
extern FILE *file;

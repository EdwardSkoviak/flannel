// lexeme.c
// written by Edward 'Cole' Skoviak (ecskoviak@crimson.ua.edu)
//
// Lexemes to hold token information

#include <stdio.h>
#include <stdlib.h>
#include "lexeme.h"
#include "types.h"

static lexeme* newLexeme(void);

lexeme* newIntegerLexeme(int v)
    {
    lexeme *p = newLexeme();

    p->type = INTEGER;
    p->ival = v;

    return p;
    }

lexeme* newStringLexeme(char* v)
    {
    lexeme *p = newLexeme();

    p->type = STRING;
    p->sval = v;

    return p;
    }
    
lexeme* newIdentifierLexeme(char* v)
    {
    lexeme *p = newLexeme();

    p->type = IDENTIFIER;
    p->sval = v;

    return p;
    }
    
lexeme* newKeywordLexeme(char* type)
    {
    lexeme *p = newLexeme();

    p->type = type;

    return p;
    }

lexeme * newArrayLexeme(size)
    {
    lexeme *p = newLexeme();

    p->type = ARRAY;
    p->aval = (lexeme **) malloc(sizeof(lexeme *) * size);

    return p;
    }

lexeme* newEmptyLexeme()
    {
    lexeme *p = newLexeme();

    return p;
    }

static lexeme* newLexeme()
    {
    lexeme *lex = (lexeme *) malloc(sizeof(lexeme));
    if (lex == 0) fprintf(stderr,"Out of memory");
    return lex;
    }

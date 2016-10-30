// list.c
// written by Edward 'Cole' Skoviak
//
// List data structure

#include <stdlib.h>
#include "lexeme.h"

lexeme* cons(char* type, lexeme* left, lexeme* right) {
    lexeme *p = newEmptyLexeme();

    p->type = type;
    p->left = left;
    p->right = right;

    return p;
}

lexeme* car(lexeme* p) {
    return p->left;
}

void setCar(lexeme* p, lexeme* newCar) {
    p->left = newCar;
}

lexeme* cdr(lexeme* p) {
    return p->right;
}

void setCdr(lexeme* p, lexeme* newCdr) {
    p->right = newCdr;
}


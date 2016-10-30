// lexeme.h
// written by Edward 'Cole' Skoviak
//
// Lexemes to hold token information

typedef struct lexemeStruct
    {
    char *type;

    // Value Part
    char *sval;
    int ival;

    struct lexemeStruct **aval;
    struct lexemeStruct *left;
    struct lexemeStruct *right;
    } lexeme;

extern lexeme* newIntegerLexeme(int value);
extern lexeme* newStringLexeme(char* value);
extern lexeme* newKeywordLexeme(char* type);
extern lexeme* newIdentifierLexeme(char* type);
extern lexeme* newArrayLexeme(int size);
extern lexeme* newEmptyLexeme();


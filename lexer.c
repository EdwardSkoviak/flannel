// lexer.c
// written by Edward 'Cole' Skoviak (ecskoviak@crimson.ua.edu)
//
// Lexical Analyzer for reading and tokenizing input

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexeme.h"
#include "types.h"

// Establish boolean values
#define true 1
#define false 0

// Create global variables to track line number and hold source file
int line = 1;
FILE *file;

// Function declarations
static void skipWhiteSpace();
static void skipComment();
static lexeme* getInteger();
static lexeme* getIdentifier();
static lexeme* getString();
static lexeme* checkKeyword(char []);

// Set source file
void lexInit(char *f)
	{
    file = fopen(f, "r" );

	// If 0 returns, file does not exist
    if (file == 0)
        {
	    printf( "Could not open file \"%s\".\n", f);
        exit(EXIT_FAILURE);
	    }
    }

// Lexical Analyzer
lexeme* lex()
    {
    int c;
    skipWhiteSpace();
    c = getc(file);
    
    // Switch for processing token
    switch (c)
        {
        case EOF:
            return newKeywordLexeme(ENDFILE);
        case '(':
            return newKeywordLexeme(LPAREN);
        case ')':
            return newKeywordLexeme(RPAREN);
        case '[':
            return newKeywordLexeme(LBRACK);
        case ']':
            return newKeywordLexeme(RBRACK);
        case '{':
            return newKeywordLexeme(LCURLY);
        case '}':
            return newKeywordLexeme(RCURLY);
        case ';':
            return newKeywordLexeme(SEMIC);
        case ',':
            return newKeywordLexeme(COMMA);
        case '+':
            return newKeywordLexeme(PLUS);
        case '-':
            return newKeywordLexeme(MINUS);
        case '*':
            return newKeywordLexeme(TIMES);
        case '/':
            return newKeywordLexeme(DIV);
        case '%':
            return newKeywordLexeme(MOD);
        case '&':
            c = getc(file);
            if (c == '&')
                return newKeywordLexeme(AND);
            else
                {
                ungetc(c,file);
                return newKeywordLexeme(ADDR);
                }
        case '|':
            c = getc(file);
            if (c == '|')
                return newKeywordLexeme(OR);
            else
                {
                printf("| on line %d is an invalid token.\n",line);
                exit(EXIT_FAILURE);
                }
        case '!':
            c = getc(file);
            if (c == '=') 
                return newKeywordLexeme(NE);
            else
                {
                ungetc(c,file);
                return newKeywordLexeme(NOT);
                }
        case '=':
            c = getc(file);
            if (c == '=')
                return newKeywordLexeme(EQ);
            else
                {
                ungetc(c,file);
                return newKeywordLexeme(IS);
                }
        case '<':
            c = getc(file);
            if (c == '=')
                return newKeywordLexeme(LE);
            else
                {
                ungetc(c,file);
                return newKeywordLexeme(LT);
                }
        case '>':
            c = getc(file);
            if (c == '=')
                return newKeywordLexeme(GE);
            else
                {
                ungetc(c,file);
                return newKeywordLexeme(GT);
                }
        default:
            // Integer token
            if (isdigit(c))
                {
                ungetc(c,file);
                return getInteger();
                }
            // Identifier or keyword token
            else if (isalpha(c))
                {
                ungetc(c,file);
                return getIdentifier();
                }
            // String token
            else if (c == '"')
                {
                return getString();
                }
            // Character does not exist in language
            else 
                {
                printf("%c on line %d is an invalid token.\n", c,line);
                exit(EXIT_FAILURE);
                }
        }
    }
    
static void skipWhiteSpace()
    {
    int c;
    
    // Loop continuously until next token begins
    while (true)
        {
        c = getc(file);
        
        // Switch for processing white space
        switch (c)
            {
            case ' ':
                break;
            case '\t':
                break;
            case '\n':
                line++;
                break;
            case '\r':
                break;
            case '/':
                c = getc(file);
                
                // Comment found
                if (c == '*')
                    {
                    skipComment();

                    // Skip any white space after comment
                    return skipWhiteSpace();
                    }
                    
                // Comment not found, put characters back in stream and return
                else
                    {
                    ungetc(c,file);
                    ungetc('/',file);
                    return;
                    }
                    
            // No white space left, return to lex
            default:
                ungetc(c,file);
                return;
            }
        }
    }
    
static void skipComment()
    {
    int c = getc(file);
    
    // Wait until * is found to signal potential end of comment
    while (c != '*')
        {
        if (c == '\n')
            line++;
        else if (c == EOF)
            {
            ungetc(c,file);
            return;
            }
        c = getc(file);
        }

    c = getc(file);
    
    // If next character is '/', comment is completed
    if (c == '/')
        return;
        
    // Comment is not completed, continue searching for end
    else
        {
        ungetc(c,file);
        skipComment();
        }
    }

static lexeme* getInteger()
    {
    int c = getc(file);
    int iValue = 0;
    
    // Retrieve rest of token
    while (isdigit(c))
        {
        // Subtract '0' to convert to integer
        iValue = iValue*10+c-'0';
        c = getc(file);
        }
    ungetc(c,file);
    return newIntegerLexeme(iValue);
    }

static lexeme* getIdentifier()
    {
    int c = getc(file);
    char tempIdArray[55];
    int idSize = 0;
    
    // Retrieve rest of token
    while (isalpha(c) || isdigit(c) || c == '_')
        {
        tempIdArray[idSize] = c;
        idSize++;
        if (idSize >= 55)
            {
            printf("Identifier on line %d is an invalid length, it ", line); 
            printf("must not exceed 55 characters.\n");
            exit(EXIT_FAILURE);
            }
        c = getc(file);
        }
    ungetc(c,file);
    
    // Add null character
    tempIdArray[idSize] = '\0';
    
    // Allocate memory for character array, copy idValue to new array
    char *idValue = (char *) malloc(sizeof(char) * (idSize + 1));
    if (idValue == 0) fprintf(stderr,"Out of memory");
    strcpy(idValue, tempIdArray);
    return checkKeyword(idValue);
    }
    
static lexeme* getString()
    {
    int c = getc(file);
    char tempStrArray[1000];
    int strSize = 0;
    
    // Retrieve rest of token
    while (c != '"')
        {
        if (c == EOF)
            {
            printf("String ending on line %d doesn't have closing \".\n", line);
            exit(EXIT_FAILURE);
            }
        tempStrArray[strSize] = c;
        strSize++;
        if (strSize >= 1000)
            {
            printf("String on line %d is an invalid length, it ", line); 
            printf("must not exceed 1000 characters.\n");
            exit(EXIT_FAILURE);
            }
        c = getc(file);
        }
    
    // Add null character
    tempStrArray[strSize] = '\0';

    // Allocate memory for character array, copy sValue to new array
    char *sValue = (char *) malloc(sizeof(char) * (strSize + 1));
    if (sValue == 0) fprintf(stderr,"Out of memory");
    strcpy(sValue, tempStrArray);
    return newStringLexeme(sValue);
    }
    
static lexeme* checkKeyword(char idValue[])
    {
    // Use string compare to check token against keywords
    if (strcmp(idValue,IF) == 0)
        {
	free(idValue);
        return newKeywordLexeme(IF);
	}
    else if (strcmp(idValue,ELSE) == 0)
        {
        free(idValue);
        return newKeywordLexeme(ELSE);
	}
    else if (strcmp(idValue,TRUE) == 0)
        {
        free(idValue);
        return newKeywordLexeme(TRUE);
	}
    else if (strcmp(idValue,FALSE) == 0)
        {
        free(idValue);
        return newKeywordLexeme(FALSE);
	}
    else if (strcmp(idValue,WHILE) == 0)
        {
        free(idValue);
        return newKeywordLexeme(WHILE);
	}
    else if (strcmp(idValue,DO) == 0)
        {
        free(idValue);
        return newKeywordLexeme(DO);
	}
    else if (strcmp(idValue,PRINT) == 0)
        {
        free(idValue);
        return newKeywordLexeme(PRINT);
	}
    else if (strcmp(idValue,VAR) == 0)
        {
        free(idValue);
        return newKeywordLexeme(VAR);
	}
    else if (strcmp(idValue,FUNCTION) == 0)
        {
        free(idValue);
        return newKeywordLexeme(FUNCTION);
	}
    else if (strcmp(idValue,ARRAY) == 0)
        {
        free(idValue);
        return newKeywordLexeme(ARRAY);
	}
        
    // Keyword not matched
    else
        return newIdentifierLexeme(idValue);
    }

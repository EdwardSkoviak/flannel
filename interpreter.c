// interpreter.c
// written by Edward 'Cole' Skoviak
//
// Interpreter

#include <stdio.h>
#include <stdlib.h>
#include "lexeme.h"
#include "lexer.h"
#include "parser.h"
#include "environment.h"
#include "evaluator.h"
#include "list.h"
#include "types.h"

#define null ((void *) 0)

int main(int argc, char *argv[]) {
    lexeme *tree, *env, *last;

    // If no file passed as param
    if (argc != 2) {
		printf("No file provided. Use: \"recognizer <file>\"\n");
		exit(EXIT_FAILURE);
    }

    // Send file name to lex
    lexInit(argv[1]);

    // Begin parsing
    tree = parse();

    // Create new environment
    env = createEnv();

    // Evaluate Parse Tree
    while(tree != null) {
        last = eval(tree->left,env);
        tree = tree->right;
    }

    // Call last function defined
    eval(cons(FUNC_CALL,last,null),env);

    // Close file
    fclose(file);

    return 0;
}

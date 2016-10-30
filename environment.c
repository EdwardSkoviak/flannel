// environment.c
// written by Edward 'Cole' Skoviak (ecskoviak@crimson.ua.edu)
//
// Environment functions

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexeme.h"
#include "types.h"
#include "list.h"

#define null ((void *) 0)

lexeme* createEnv()
    {
    return cons(ENVIRONMENT,cons(FRAME,null,null),null);
    }

lexeme * extendEnv(lexeme *env, lexeme *vars, lexeme *vals)
    {
    return cons(ENVIRONMENT,cons(FRAME,vars,vals),env);
    }

lexeme * defineVariable(lexeme* variable, lexeme* value, lexeme* env)
    {
    lexeme* frame;
    lexeme* vars = car(car(env));
    lexeme* vals = cdr(car(env));
    vars = cons(LIST,variable,vars);
    vals = cons(LIST,value,vals);
    frame = cons(FRAME,vars,vals);
    env->left = frame;
    return variable;
    }

lexeme* findVariable(lexeme* var, lexeme* env)
    {
    while (env != null)
        {
        lexeme* table = car(env);
        lexeme* vars = car(table);
        lexeme* vals = cdr(table);
        while (vars != null)
            {
            if (strcmp(var->sval,car(vars)->sval) == 0)
                return car(vals);
            vars = cdr(vars);
            vals = cdr(vals);
            }
        env = cdr(env);
        }
    printf("%s is not defined.\n", var->sval);
    exit(EXIT_FAILURE);

    return null;
    }

lexeme* updateVariable(lexeme* var, lexeme* val, lexeme* env)
    {
    while (env != null)
        {
        lexeme* table = car(env);
        lexeme* vars = car(table);
        lexeme* vals = cdr(table);
        while (vars != null)
            {
            if (strcmp(var->sval,car(vars)->sval) == 0)
                {
                setCar(vals,val);
                return val;
                }
            vars = cdr(vars);
            vals = cdr(vals);
            }
        env = cdr(env);
        }
    printf("%s is not defined.\n", var->sval);
    exit(EXIT_FAILURE);

    return null;
    }


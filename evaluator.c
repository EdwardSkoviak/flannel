// evaluator.c
// written by Edward 'Cole' Skoviak (ecskoviak@crimson.ua.edu)
//
// Evaluator

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexeme.h"
#include "environment.h"
#include "types.h"
#include "list.h"

#define null ((void *) 0)

static lexeme * lookup(lexeme *, lexeme *, lexeme *);
static lexeme * uminus(lexeme *);
static lexeme * not(lexeme *);
static lexeme * multiply(lexeme *, lexeme *);
static lexeme * divide(lexeme *, lexeme *);
static lexeme * mod(lexeme *, lexeme *);
static lexeme * add(lexeme *, lexeme *);
static lexeme * subtract(lexeme *, lexeme *);
static lexeme * lessThan(lexeme *, lexeme *);
static lexeme * greaterThan(lexeme *, lexeme *);
static lexeme * lessEqual(lexeme *, lexeme *);
static lexeme * greaterEqual(lexeme *, lexeme *);
static lexeme * equal(lexeme *, lexeme *);
static lexeme * notEqual(lexeme *, lexeme *);
static lexeme * evalAnd(lexeme *, lexeme *);
static lexeme * evalOr(lexeme *, lexeme *);
static lexeme * print(lexeme *, lexeme *);
static lexeme * evalCall(lexeme *, lexeme *);
static lexeme * evalArgs(lexeme *, lexeme *);
static lexeme * evalDo(lexeme *, lexeme *, lexeme *);
static lexeme * evalWhile(lexeme *, lexeme *, lexeme *);
static lexeme * evalIf(lexeme *, lexeme *, lexeme *, lexeme *);
static lexeme * evalBlock(lexeme *, lexeme *);
static lexeme * evalVarDef(lexeme *, lexeme *);
static lexeme * evalIs(lexeme *, lexeme *);

lexeme* eval(lexeme *tree, lexeme *env) {
    if(tree == null)
        return null;
    else if(tree->type == INTEGER)
        return tree;
    else if(tree->type == ARRAY)
        return newArrayLexeme(eval(tree->left,env)->ival);
    else if(tree->type == REF)
        return lookup(tree->left,eval(tree->right,env),env);
    else if(tree->type == TRUE)
        return tree;
    else if(tree->type == FALSE)
        return tree;
    else if(tree->type == STRING)
        return tree;
    else if(tree->type == UMINUS)
        return uminus(eval(tree->left,env));
    else if(tree->type == NOT)
        return not(eval(tree->left,env));
    else if(tree->type == TIMES)
        return multiply(eval(tree->left,env),eval(tree->right,env));
    else if(tree->type == DIV)
        return divide(eval(tree->left,env),eval(tree->right,env));
    else if(tree->type == MOD)
        return mod(eval(tree->left,env),eval(tree->right,env));
    else if(tree->type == PLUS)
        return add(eval(tree->left,env),eval(tree->right,env));
    else if(tree->type == MINUS)
        return subtract(eval(tree->left,env),eval(tree->right,env));
    else if(tree->type == LT)
        return lessThan(eval(tree->left,env),eval(tree->right,env));
    else if(tree->type == GT)
        return greaterThan(eval(tree->left,env),eval(tree->right,env));
    else if(tree->type == LE)
        return lessEqual(eval(tree->left,env),eval(tree->right,env));
    else if(tree->type == GE)
        return greaterEqual(eval(tree->left,env),eval(tree->right,env));
    else if(tree->type == EQ)
        return equal(eval(tree->left,env),eval(tree->right,env));
    else if(tree->type == NE)
        return notEqual(eval(tree->left,env),eval(tree->right,env));
    else if(tree->type == AND)
        return evalAnd(eval(tree->left,env),eval(tree->right,env));
    else if(tree->type == OR)
        return evalOr(eval(tree->left,env),eval(tree->right,env));
    else if(tree->type == IS)
        return evalIs(tree,env);
    else if(tree->type == PRINT_CALL)
        print(tree->left,env);
    else if(tree->type == FUNC_CALL)
        return evalCall(tree,env);
    else if(tree->type == DO)
        return evalDo(tree->left,tree->right,env);
    else if(tree->type == WHILE)
        return evalWhile(tree->left,tree->right,env);
    else if(tree->type == IF)
        return evalIf(eval(tree->left,env),tree->right->left,tree->right->right,env);
    else if(tree->type == VAR_DECL)
        return evalVarDef(tree,env);
    else if(tree->type == FUNC_DECL)
        return defineVariable(tree->left,cons(CLOSURE,env,tree),env);
    else if(tree->type == BLOCK)
        return evalBlock(tree,extendEnv(env,null,null));
    return null;
}

// Lookup and return variable

static lexeme * lookup(lexeme *ident, lexeme *index, lexeme *env) {
    lexeme *variable;
    variable = findVariable(ident,env);
    if(index != null)
        return variable->aval[index->ival];
    return variable;
}

// Unary minus

static lexeme * uminus(lexeme *subtrahend) {
    return newIntegerLexeme(-subtrahend->ival);
}

static lexeme *not(lexeme *condition) {
    if(condition->type == FALSE)
        return newKeywordLexeme(TRUE);
    else
        return newKeywordLexeme(FALSE);
}

static lexeme * multiply(lexeme *leftFactor, lexeme *rightFactor) {
    return newIntegerLexeme(leftFactor->ival * rightFactor->ival);
}

static lexeme * divide(lexeme *leftTerm, lexeme *rightTerm) {
    return newIntegerLexeme(leftTerm->ival / rightTerm->ival);
}

static lexeme * mod(lexeme *leftTerm, lexeme *rightTerm) {
    return newIntegerLexeme(leftTerm->ival % rightTerm->ival);
}

static lexeme * add(lexeme *augend, lexeme *addend) {
    return newIntegerLexeme(augend->ival + addend->ival);
}

static lexeme * subtract(lexeme *minuend, lexeme *subtrahend) {
    return newIntegerLexeme(minuend->ival - subtrahend->ival);
}

static lexeme * lessThan(lexeme *leftTerm, lexeme *rightTerm) {
    if(leftTerm->ival < rightTerm->ival)
        return newKeywordLexeme(TRUE);
    else
        return newKeywordLexeme(FALSE);
}

static lexeme * greaterThan(lexeme *leftTerm, lexeme *rightTerm) {
    if(leftTerm->ival > rightTerm->ival)
        return newKeywordLexeme(TRUE);
    else
        return newKeywordLexeme(FALSE);
}

static lexeme * lessEqual(lexeme *leftTerm, lexeme *rightTerm) {
    if(leftTerm->ival <= rightTerm->ival)
        return newKeywordLexeme(TRUE);
    else
        return newKeywordLexeme(FALSE);
}

static lexeme * greaterEqual(lexeme *leftTerm, lexeme *rightTerm) {
    if(leftTerm->ival >= rightTerm->ival)
        return newKeywordLexeme(TRUE);
    else
        return newKeywordLexeme(FALSE);
}

static lexeme * equal(lexeme *leftTerm, lexeme *rightTerm) {
    if(leftTerm->type == INTEGER) {
        if(leftTerm->ival == rightTerm->ival)
            return newKeywordLexeme(TRUE);
        else
            return newKeywordLexeme(FALSE);
    }
    else if(leftTerm->type == STRING) {
        if(strcmp(leftTerm->sval,rightTerm->sval) == 0)
            return newKeywordLexeme(TRUE);
        else
            return newKeywordLexeme(FALSE);
    }
    else if(leftTerm->type == TRUE && rightTerm->type == TRUE)
        return newKeywordLexeme(TRUE);
    else if(leftTerm->type == FALSE && rightTerm->type == FALSE)
        return newKeywordLexeme(TRUE);
    else
        return newKeywordLexeme(FALSE);
}

static lexeme * notEqual(lexeme *leftTerm, lexeme *rightTerm) {
    if(leftTerm->type == INTEGER) {
        if(leftTerm->ival == rightTerm->ival)
            return newKeywordLexeme(FALSE);
        else
            return newKeywordLexeme(TRUE);
    }
    else if(leftTerm->type == STRING) {
        if(strcmp(leftTerm->sval,rightTerm->sval) == 0)
            return newKeywordLexeme(FALSE);
        else
            return newKeywordLexeme(TRUE);
    }
    else if(leftTerm->type == TRUE && rightTerm->type == FALSE)
        return newKeywordLexeme(TRUE);
    else if(leftTerm->type == FALSE && rightTerm->type == TRUE)
        return newKeywordLexeme(TRUE);
    else
        return newKeywordLexeme(FALSE);
}

static lexeme * evalAnd(lexeme *leftTerm, lexeme *rightTerm) {
    if(leftTerm->type == TRUE) {
        if(rightTerm->type == TRUE)
            return newKeywordLexeme(TRUE);
        else
            return newKeywordLexeme(FALSE);
    }
    else
        return newKeywordLexeme(FALSE);
}

static lexeme * evalOr(lexeme *leftTerm, lexeme *rightTerm) {
    if(leftTerm->type == TRUE)
        return newKeywordLexeme(TRUE);
    else if (rightTerm->type == TRUE)
        return newKeywordLexeme(TRUE);
    else
        return newKeywordLexeme(FALSE);
}

static lexeme * print(lexeme *args, lexeme *env) {
    lexeme *arg;
    while(args != null) {
        arg = eval(args->left,env);
        if(arg->type == STRING)
            printf("%s",arg->sval);
        else if(arg->type == INTEGER)
            printf("%d",arg->ival);
        else if(arg->type == TRUE)
            printf("true");
        else if(arg->type == FALSE)
            printf("false");
        args = args->right;
    }
    printf("\n");
    return arg;
}

static lexeme * evalCall(lexeme *tree, lexeme *env) {
    lexeme *closure, *params, *args, *body, *denv, *eargs, *eenv;
    closure = lookup(tree->left,null,env);
    params = closure->right->right->left;
    args = tree->right;
    body = closure->right->right->right;
    denv = closure->left;
    eargs = evalArgs(args,env);
    eenv = extendEnv(denv,params,eargs);
    return evalBlock(body,eenv);
}

static lexeme * evalArgs(lexeme *args, lexeme *env) {
    if(args != null) {
        lexeme *arg = eval(args->left,env);
        return cons(LIST,arg,evalArgs(args->right,env));
    }
    else
        return null;
}

static lexeme * evalDo(lexeme *doBlock, lexeme *testExpr, lexeme *env) {
    lexeme *last;
    eval(doBlock,env);
    if(eval(testExpr,env)->type == TRUE) {
        last = evalDo(doBlock,testExpr,env);
    }
    return last;
}

static lexeme * evalWhile(lexeme *testExpr, lexeme *doBlock, lexeme *env) {
    lexeme *last;
    while(eval(testExpr,env)->type == TRUE) {
        last = eval(doBlock,env);
    }
    return last;
}

static lexeme * evalIf(lexeme *testExpr, lexeme *thenExpr, lexeme *elseExpr, lexeme *env) {
    if(testExpr->type == TRUE)
        return eval(thenExpr,env);
    else
        return eval(elseExpr,env);
}

static lexeme * evalBlock(lexeme *tree, lexeme *env) {
    lexeme *varDecls, *funcDecls, *statements, *last;
    varDecls = tree->left->left;
    funcDecls = tree->left->right;
    statements = tree->right;
    while(varDecls != null) {
        last = evalVarDef(varDecls->left,env);
        varDecls = varDecls->right;
    }
    while(funcDecls != null) {
        last = defineVariable(funcDecls->left->left,cons(CLOSURE,env,funcDecls->left),env);
        funcDecls = funcDecls->right;
    }
    while(statements != null) {
        last = eval(statements->left,env);
        statements = statements->right;
    }
    return last;
}

static lexeme * evalVarDef(lexeme *tree, lexeme *env) {
    lexeme *last;
    while(tree != null) {
        defineVariable(tree->left,null,env);
        last = tree->left;
        tree = tree->right;
    }
    return last;
}

static lexeme * evalIs(lexeme *tree, lexeme *env) {
    if(tree->left->right != null) {
        lexeme *ref;
        ref = lookup(tree->left->left,null,env);
        ref->aval[eval(tree->left->right,env)->ival] = eval(tree->right,env);
        return ref->aval[eval(tree->left->right,env)->ival];
    }
    return updateVariable(tree->left->left,eval(tree->right,env),env);
}

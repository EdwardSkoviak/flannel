// parser.c
// written by Edward 'Cole' Skoviak (ecskoviak@crimson.ua.edu)
//
// Parser for checking syntax

#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include "lexeme.h"
#include "types.h"
#include "lexer.h"
#include "list.h"
#include "environment.h"

#define null ((void *) 0)

static int check(char *type);
static lexeme * match(char *type);
static lexeme * program(lexeme*);
static int variableDeclarationPending();
static lexeme * variableDeclaration(lexeme*);
static lexeme * variableList(lexeme*);
static int functionDeclarationPending();
static lexeme * functionDeclaration(lexeme*);
static lexeme * parameterList();
static lexeme * parameterListTail();
static int blockPending();
static lexeme * block();
static lexeme * blockFD();
static lexeme * blockVD();
static lexeme * blockStatement();
static int statementPending();
static lexeme * statement();
static int ifStatementPending();
static lexeme * ifStatement();
static lexeme * ifStatementTail();
static int whileStatementPending();
static lexeme * whileStatement();
static int doStatementPending();
static lexeme * doStatement();
static int assignmentPending();
static lexeme * assignment();
static int functionCallPending();
static lexeme * functionCall();
static int printCallPending();
static lexeme * printCall();
static lexeme * expressionList();
static lexeme * expressionListTail();
static int expressionPending();
static lexeme * expression();
static int conjunctionPending();
static lexeme * conjunction();
static int equalityPending();
static lexeme * equality();
static int inequalityPending();
static lexeme * inequality();
static int sumPending();
static lexeme * sum();
static int productPending();
static lexeme * product();
static int factorPending();
static lexeme * factor();
static int factorHeadPending();
static lexeme * factorHead();
static int primaryPending();
static lexeme * primary();
static int identifierIndexPending();
static lexeme * identifierIndex();

static lexeme *currentLexeme;
static jmp_buf matchError;

lexeme * parse()
    {
    lexeme *parsedProgram;
    lexeme *env = createEnv();
    if (setjmp(matchError) != 0)
        {
        printf("Illegal\n");
        exit(EXIT_FAILURE);
        }
    currentLexeme = lex();
    parsedProgram = program(env);
    match(ENDFILE);
    return parsedProgram;
    }

static int check(char *type) 
    { 
    return currentLexeme->type == type; 
    }
  
static lexeme* match(char *type) 
    { 
    if(check(type)) 
        {
        lexeme *oldLexeme = currentLexeme;
        currentLexeme = lex();
        return oldLexeme;
        }
    printf("Match Error: Expected %s but got %s on line %d.\n",
		type, currentLexeme->type, line);
    longjmp(matchError, 1);
    }

static lexeme* program(lexeme* env)
    {
    lexeme *prog;
    if (variableDeclarationPending())
        {
        prog = variableDeclaration(env);
        return cons(NO_TYPE,prog,program(env));
        }
    else if (functionDeclarationPending())
        {
        prog = functionDeclaration(env);
        return cons(NO_TYPE,prog,program(env));
        }
    return null;
    }

static int variableDeclarationPending()
    {
    return check(VAR);
    }

static lexeme* variableDeclaration(lexeme* env)
    {
    lexeme *var1, *var2;
    match(VAR);
    var1 = match(IDENTIFIER);
    defineVariable(var1,null,env);
    var2 = variableList(env);
    match(SEMIC);
    return cons(VAR_DECL,var1,var2);
    }

static lexeme* variableList(lexeme* env)
    {
    if(check(COMMA))
        {
        lexeme *var1, *var2;
        match(COMMA);
        var1 = match(IDENTIFIER);
        defineVariable(var1,null,env);
        var2 = variableList(env);
        return cons(VAR_DECL,var1,var2);
        }
    return null;
    }

static int functionDeclarationPending()
    {
    return check(FUNCTION);
    }

static lexeme* functionDeclaration(lexeme* env)
    {
    lexeme *funcName, *function, *newEnvironment, *body, *params;
    match(FUNCTION);
    newEnvironment = extendEnv(env,null,null);
    funcName = match(IDENTIFIER);
    defineVariable(funcName,null,env);
    match(LPAREN);
    params = parameterList(newEnvironment);
    match(RPAREN);
    body = block(newEnvironment);
    function = cons(NO_TYPE,params,body);
    return cons(FUNC_DECL,funcName,function);
    }

static lexeme* parameterList(lexeme* env)
    {
    if (check(IDENTIFIER))
        {
        lexeme *param1, *param2;
        param1 = match(IDENTIFIER);
        defineVariable(param1,null,env);
        param2 = parameterListTail(env);
        return cons(LIST,param1,param2);
        }
    return null;
    }

static lexeme* parameterListTail(lexeme* env)
    {
    if (check(COMMA))
        {
        lexeme *param1, *param2;
        match(COMMA);
        param1 = match(IDENTIFIER);
        defineVariable(param1,null,env);
        param2 = parameterListTail(env);
        return cons(LIST,param1,param2);
        }
    return null;
    }

static int blockPending()
    {
    return check(LCURLY);
    }

static lexeme* block(lexeme* env)
    {
    lexeme *vd, *fd, *declarations, *statements;
    match(LCURLY);
    vd = blockVD(env);
    fd = blockFD(env);
    declarations = cons(NO_TYPE,vd,fd);
    statements = blockStatement(env);
    match(RCURLY);
    return cons(BLOCK,declarations,statements);
    }

static lexeme* blockVD(lexeme *env)
    {
    if (variableDeclarationPending())
        {
        lexeme *var1, *var2;
        var1 = variableDeclaration(env);
        var2 = blockVD(env);
        return cons(NO_TYPE,var1,var2);
        }
    return null;
    }

static lexeme* blockFD(lexeme *env)
    {
    if (functionDeclarationPending())
        {
        lexeme *func1, *func2;
        func1 = functionDeclaration(env);
        func2 = blockFD(env);
        return cons(NO_TYPE,func1,func2);
        }
    return null;
    }

static lexeme* blockStatement(lexeme *env)
    {
    if (statementPending())
        {
        lexeme *state1, *state2;
        state1 = statement(env);
        state2 = blockStatement(env);
        return cons(NO_TYPE,state1,state2);
        }
    return null;
    }

static int statementPending()
    {
    return blockPending() || ifStatementPending() || whileStatementPending() ||
        doStatementPending() || check(IDENTIFIER) || printCallPending();
    }

static lexeme* statement(lexeme *env)
    {
    if (blockPending())
        {
        lexeme *newEnvironment;
        newEnvironment = extendEnv(env,null,null);
        return block(newEnvironment);
        }
    else if (ifStatementPending())
        return ifStatement(env);
    else if (whileStatementPending())
        return whileStatement(env);
    else if (doStatementPending())
        return doStatement(env);
    else if (printCallPending())
        return printCall(env);
    else
        {
        lexeme *ident;
        ident = match(IDENTIFIER);
        if (assignmentPending())
            return assignment(ident,env);
        else
            {
            lexeme *func;
            func = functionCall(ident,env);
            match(SEMIC);
            return func;
            }
        }
    }

static int ifStatementPending()
    {
    return check(IF);
    }

static lexeme* ifStatement(lexeme *env)
    {
    lexeme *thenElse, *testExpr, *thenExpr, *elseExpr;
    match(IF);
    match(LPAREN);
    testExpr = expression(env);
    match(RPAREN);
    thenExpr = statement(env);
    elseExpr = ifStatementTail(env);
    thenElse = cons(NO_TYPE,thenExpr,elseExpr);
    return cons(IF,testExpr,thenElse);
    }

static lexeme* ifStatementTail(lexeme *env)
    {
    if(check(ELSE))
        {
        match(ELSE);
        return statement(env);
        }
    return null;
    }
    
static int whileStatementPending()
    {
    return check(WHILE);
    }

static lexeme* whileStatement(lexeme *env)
    {
    lexeme *testExpr, *state;
    match(WHILE);
    match(LPAREN);
    testExpr = expression(env);
    match(RPAREN);
    state = statement(env);
    return cons(WHILE,testExpr,state);
    }

static int doStatementPending()
    {
    return check(DO);
    }

static lexeme* doStatement(lexeme *env)
    {
    lexeme *state, *testExpr;
    match(DO);
    state = statement(env);
    match(WHILE);
    match(LPAREN);
    testExpr = expression(env);
    match(RPAREN);
    match(SEMIC);
    return cons(DO,state,testExpr);
    }

static int assignmentPending()
    {
    return identifierIndexPending() || check(IS);
    }

static lexeme* assignment(lexeme *ident, lexeme *env)
    {
    lexeme *value, *index;
    index = identifierIndex(env);
    findVariable(ident,env);
    match(IS);
    value = expression(env);
    match(SEMIC);
    return cons(IS,cons(REF,ident,index),value);
    }

static int functionCallPending()
    {
    return check(LPAREN);
    }

static lexeme* functionCall(lexeme *ident,lexeme *env)
    {
    lexeme *args;
    findVariable(ident,env);
    match(LPAREN);
    args = expressionList(env);
    match(RPAREN);
    return cons(FUNC_CALL,ident,args);
    }

static int printCallPending()
    {
    return check(PRINT);
    }

static lexeme* printCall(lexeme *env)
    {
    lexeme *args;
    match(PRINT);
    match(LPAREN);
    args = expressionList(env);
    match(RPAREN);
    match(SEMIC);
    return cons(PRINT_CALL,args,null);
    }

static lexeme* expressionList(lexeme *env)
    {
    if (expressionPending())
        {
        lexeme *arg1, *arg2;
        arg1 = expression(env);
        arg2 = expressionListTail(env);
        return cons(LIST,arg1,arg2);
        }
    return null;
    }

static lexeme* expressionListTail(lexeme *env)
    {
    if (check(COMMA))
        {
        lexeme *arg1, *arg2;
        match(COMMA);
        arg1 = expression(env);
        arg2 = expressionListTail(env);
        return cons(LIST,arg1,arg2);
        }
    return null;
    }

static int expressionPending()
    {
    return conjunctionPending();
    }

static lexeme* expression(lexeme *env)
    {
    lexeme *conj;
    conj = conjunction(env);
    if (check(OR))
        {
        lexeme *expr;
        match(OR);
        expr = expression(env);
        return cons(OR,conj,expr);
        }
    return conj;
    }

static int conjunctionPending()
    {
    return equalityPending();
    }

static lexeme* conjunction(lexeme *env)
    {
    lexeme *equ;
    equ = equality(env);
    if (check(AND))
        {
        lexeme *conj;
        match(AND);
        conj = conjunction(env);
        return cons(AND,equ,conj);
        }
    return equ;
    }

static int equalityPending()
    {
    return inequalityPending();
    }

static lexeme* equality(lexeme *env)
    {
    lexeme *ineq;
    ineq = inequality(env);
    if (check(EQ))
        {
        lexeme *equ;
        match(EQ);
        equ = equality(env);
        return cons(EQ,ineq,equ);
        }
    else if (check(NE))
        {
        lexeme *equ;
        match(NE);
        equ = equality(env);
        return cons(NE,ineq,equ);
        }
    return ineq;
    }

static int inequalityPending()
    {
    return sumPending();
    }

static lexeme* inequality(lexeme *env)
    {
    lexeme* summ;
    summ = sum(env);
    if (check(LT))
        {
        lexeme *ineq;
        match(LT);
        ineq = inequality(env);
        return cons(LT,summ,ineq);
        }
    else if (check(GT))
        {
        lexeme *ineq;
        match(GT);
        ineq = inequality(env);
        return cons(GT,summ,ineq);
        }
    else if (check(LE))
        {
        lexeme *ineq;
        match(LE);
        ineq = inequality(env);
        return cons(LE,summ,ineq);
        }
    else if (check(GE))
        {
        lexeme *ineq;
        match(GE);
        ineq = inequality(env);
        return cons(GE,summ,ineq);
        }
    return summ;
    }

static int sumPending()
    {
    return productPending();
    }

static lexeme* sum(lexeme *env)
    {
    lexeme *prod;
    prod = product(env);
    if (check(PLUS))
        {
        lexeme *summ;
        match(PLUS);
        summ = sum(env);
        return cons(PLUS,prod,summ);
        }
    else if (check(MINUS))
        {
        lexeme *summ;
        match(MINUS);
        summ = sum(env);
        return cons(MINUS,prod,summ);
        }
    return prod;
    }

static int productPending()
    {
    return factorPending();
    }

static lexeme* product(lexeme *env)
    {
    lexeme* fact;
    fact = factor(env);
    if (check(TIMES))
        {
        lexeme *prod;
        match(TIMES);
        prod = product(env);
        return cons(TIMES,fact,prod);
        }
    else if (check(DIV))
        {
        lexeme *prod;
        match(DIV);
        prod = product(env);
        return cons(DIV,fact,prod);
        }
    else if (check(MOD))
        {
        lexeme *prod;
        match(MOD);
        prod = product(env);
        return cons(MOD,fact,prod);
        }
    return fact;
    }

static int factorPending()
    {
    return factorHeadPending() || primaryPending();
    }

static lexeme* factor(lexeme *env)
    {
    lexeme *prim, *factorMod;
    factorMod = factorHead();
    prim = primary(env);
    if(factorMod != null)
        {
        if(factorMod->type == MINUS)
            return cons(UMINUS,prim,null);
        else if (factorMod->type == NOT)
            return cons(NOT,prim,null);
        }
    return prim;
    }

static int factorHeadPending()
    {
    return check(PLUS) || check(MINUS) || check(NOT);
    }

static lexeme* factorHead()
    {
    if (check(PLUS))
        return match(PLUS);
    else if (check(MINUS))
        return match(MINUS);
    else if (check(NOT))
        return match(NOT);
    return null;
    }

static int primaryPending()
    {
    return check(IDENTIFIER) || check(LPAREN) || check(INTEGER) ||
        check(TRUE) || check(FALSE) || check(STRING) || check(ARRAY);
    }

static lexeme* primary(lexeme *env)
    {
    if (check(IDENTIFIER))
        {
        lexeme *ident;
        ident = match(IDENTIFIER);
        if (functionCallPending())
            {
            return functionCall(ident,env);
            }
        else
            {
            lexeme *index;
            findVariable(ident,env);
            index = identifierIndex(env);
            return cons(REF,ident,index);
            }
        }
    else if (check(INTEGER))
        return match(INTEGER);
    else if (check(LPAREN))
        {
        lexeme *expr;
        match(LPAREN);
        expr = expression(env);
        match(RPAREN);
        return expr;
        }
    else if (check(TRUE))
        return match(TRUE);
    else if (check(FALSE))
        return match(FALSE);
    else if (check(ARRAY))
        {
        lexeme *size;
        match(ARRAY);
        match(LPAREN);
        size = expression(env);
        match(RPAREN);
        return cons(ARRAY,size,null);
        }
    else
        return match(STRING);
    }

static int identifierIndexPending()
    {
    return check(LBRACK);
    }

static lexeme* identifierIndex(lexeme *env)
    {
    if (check(LBRACK))
        {
        lexeme *index;
        match(LBRACK);
        index = expression(env);
        match(RBRACK);
        return index;
        }
    return null;
    }


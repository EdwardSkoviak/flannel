# Flannel

Flannel is an experimental interpreted language written in C.

## Features

Flannel is a C-style lanauge with the following features:

* There is no typing of variables. All variables are declared with the reserved word `var`, for ex:
```
var a;
var x, y, z;
```
* Functions in Flannel do not have a return type and are declared with `function`, for ex:
```
function main() {
  var x = 1;
}
```
* The entry point/main function in Flannel is the last function defined in the source file. It is ran immedatiely after parsing and cannot take any arguments
* You can not define multiple returns inside of a function, and the last statement executed is returned to the calling function.
* Functions can be used as first class objects

## Grammar

The following is the grammar for the Flannel language:

```
program : variableDeclaration program | functionDeclaration program | *empty*

variableDeclaration : VAR IDENTIFIER variableList SEMIC

variableList : COMMA IDENTIFIER variableList | *empty*

functionDeclaration : FUNCTION IDENTIFIER LPAREN parameterList RPAREN block

parameterList : IDENTIFIER parameterListTail | *empty*

parameterListTail : COMMA IDENTIFIER parameterListTail | *empty*

block : LCURLY blockFD blockVD blockStatement RCURLY

blockFD : functionDeclaration blockFD | *empty*

blockVD : variableDeclaration blockVD | *empty*

blockStatement : statement blockStatement | *empty*

statement : block | ifStatement | whileStatement | doStatement
            | IDENTIFIER assignment | IDENTIFIER functionCall SEMIC
            | printCall

ifStatement : IF LPAREN expression RPAREN statement ifStatementTail

ifStatementTail : ELSE statement | *empty*

whileStatement : WHILE LPAREN expression RPAREN statement

doStatement : DO statement WHILE LPAREN expression RPAREN SEMIC

assignment : identifierIndex IS expression SEMIC

functionCall : LPAREN expressionList RPAREN

printCall : PRINT LPAREN expressionList RPAREN SEMIC

expressionList : expression expressionListTail | *empty*

expressionListTail : COMMA expression expressionListTail | *empty*

expression : conjunction OR expression | *empty*

conjunction : equality AND conjunction | *empty*

equality : inequality EQ equality | inequality NE equality | *empty*

inequality : sum LT inequality | sum GT inequality | sum LE inequality
             | sum GE inequality | *empty*

sum : product PLUS sum | product MINUS sum | *empty*

product : factor TIMES product | factor DIV product
          | factor MOD product | *empty*

factor : factorHead primary

factorHead : PLUS | MINUS | NOT | *empty*

primary : IDENTIFIER identifierIndex | IDENTIFIER functionCall
          | LPAREN expression RPAREN | ARRAY LPAREN expression RPAREN
          | INTEGER | TRUE | FALSE | STRING

identifierIndex : LBRACK expression RBRACK | *empty*
```

## Compiling Flannel

Compiling flannel is done by executing `make` at the command line:

```
$ make
```

## Executing a program

To run a program in flannel, simply type `flannel ${file_name}`:

```
$ ./flannel hello_world.fibre
```

## Example programs

The following programs, found in `test_files/`, demonstrate the power of flannel:

* expression.fibre - illustrates evaluation of boolean and arithmetic operators
* fibonacci.fibre - gives the fibonacci seq up to a desired stopping point, modify halt_value to see more fib numbers
* function_play.fibre - demonstrates using functions as first class objects
* hello_world.fibre - your typical helloworld program
* pouring_path.fibre - pouring path solution
* quicksort.fibre - quicksorts an array of numbers, add numbers and increase array size (inside main function) as you wish
* sqrt.fibre - computes the square root of a number using bisection algorithm, change N as you wish
* stack.fibre - demonstrates a stack represented as an array
* test.fibre - shows an invalid program, gives proper line number for error message

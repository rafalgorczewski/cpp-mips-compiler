%{
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Formatters/FuncMessageFormatter.h>
#include <plog/Log.h>

#include <stdio.h>
#include <stdlib.h>

#include "compiler.hpp"
#include "helpers.hpp"

extern int yylex();
extern int yyparse();
extern FILE* yyin;

void yyerror(const char* s);

Compiler compiler;

%}

%union {
    char *text;
    int	ival;
    float fval;
}

%token <text> ID
%token <ival> IDWORD_T
%token <fval> FDWORD_T

%token BOOL_T BYTE_T IBYTE_T IWORD_T IQWORD_T
%token UBYTE_T UWORD_T UDWORD_T UQWORD_T FQWORD_T
%token ADDR_T ARR_T ARRAY_T STR_T STRING_T SHARED_T UNIQUE_T
%token CONST_Q CP_Q MUT_Q REF_Q
%token VAR_D LOOP_D INV_D TYPE_D
%token MAIN_SI IF_SI ELSE_SI FOR_SI WHILE_SI ENDLESS_SI STRUCT_SI ENUM_SI
%token FUNC_SI LAMBDA_SI QUIT_SI BREAK_SI CONTINUE_SI RETURN_SI
%token '+' '-' '*' '/'
%token '{' '}' '(' ')' '<' '>' '[' ']'
%token '?' '.' ':' ';' '=' ARROW_S
%token INC_OP DEC_OP LE_OP GE_OP EQ_OP NEQ_OP AND_OP OR_OP NOT_OP
%token BITAND_OP BITOR_OP BITNOT_OP BITSHL_OP BITSHR_OP
%token PRINT_OP READ_OP MODULO_OP
%token NEWLINE

%right '='
%right PRINT_OP
%left '+' '-'
%left '*' '/'

%type <text> variable
%type <text> type

%start lines
%%

lines: line
| lines line
;

line: expression ';'
| assignment ';'
| initialization ';'
| print ';'
| NEWLINE
;

expression: '(' expression ')'
| expression '+' element            {compiler.triplet(Op::Add);}
| expression '-' element            {compiler.triplet(Op::Sub);}
| element
;

element: '(' element ')'
| element '*' entity            {compiler.triplet(Op::Mul);}
| element '/' entity            {compiler.triplet(Op::Div);}
| entity

entity: ID            {compiler.push($1);}
| FDWORD_T            {compiler.push(Data(static_cast<float>($1)));}
| IDWORD_T            {compiler.push(Data(static_cast<int>($1)));}
| '(' entity ')'
;


variable: ID
;

assignment: variable '=' expression   {compiler.assign($1);}
;

type: ID
;

initialization: type assignment       {}
;

print: PRINT_OP expression            {compiler.print();}
;

%%
int main() {
  plog::ColorConsoleAppender<plog::FuncMessageFormatter> consoleAppender;
  plog::init(plog::verbose, &consoleAppender);

	yyin = stdin;

	do {
		yyparse();
	} while(!feof(yyin));

  compiler.compile();

	return 0;
}

void yyerror(const char* s) {
	PLOGE << "Parse error: " << s;
	exit(1);
}

%option noyywrap

%{
#include <stdio.h>

#define YY_DECL int yylex()

#include "lang.tab.hh"
%}

INT [0]+|[1-9][0-9]*
FLOAT [0-9]+\.[0-9]+
ID [A-Za-z][A-Za-z0-9_]*

%%
"bool" {return BOOL_T;}
"byte" {return BYTE_T;}
"i8" {return IBYTE_T;}
"i16" {return IWORD_T;}
"i32" {return IDWORD_T;}
"i64" {return IQWORD_T;}
"u8" {return UBYTE_T;}
"u16" {return UWORD_T;}
"u32" {return UDWORD_T;}
"u64" {return UQWORD_T;}
"f32" {return FDWORD_T;}
"f64" {return FQWORD_T;}

"addr" {return ADDR_T;}
"arr" {return ARR_T;}
"array" {return ARRAY_T;}
"str" {return STR_T;}
"string" {return STRING_T;}
"shared" {return SHARED_T;}
"unique" {return UNIQUE_T;}

"const" {return CONST_Q;}
"cp" {return CP_Q;}
"mut" {return MUT_Q;}
"ref" {return REF_Q;}

"var" {return VAR_D;}
"loop" {return LOOP_D;}
"inv" {return INV_D;}
"type" {return TYPE_D;}

"main" {return MAIN_SI;}
"if" {return IF_SI;}
"else" {return ELSE_SI;}
"for" {return FOR_SI;}
"while" {return WHILE_SI;}
"endless" {return ENDLESS_SI;}
"struct" {return STRUCT_SI;}
"enum" {return ENUM_SI;}
"func" {return FUNC_SI;}
"lambda" {return LAMBDA_SI;}
"quit" {return QUIT_SI;}
"break" {return BREAK_SI;}
"continue" {return CONTINUE_SI;}
"return" {return RETURN_SI;}

\+		{return '+';}
\-		{return '-';}
\*		{return '*';}
\/		{return '/';}

\{		{return '{';}
\}		{return '}';}
\(		{return '(';}
\<		{return '<';}
\)		{return ')';}
\>		{return '>';}
\[		{return '[';}
\]		{return ']';}

\? {return '?';}
\. {return '.';}
\: {return ':';}
\; {return ';';}
\= {return '=';}
"->" {return ARROW_S;}

"++" {return INC_OP;}
"--" {return DEC_OP;}

"<=" {return LE_OP;}
">=" {return GE_OP;}
"==" {return EQ_OP;}
"!=" {return NEQ_OP;}

"&&" {return AND_OP;}
"||" {return OR_OP;}
"!" {return NOT_OP;}

"&" {return BITAND_OP;}
"|" {return BITOR_OP;}
"~" {return BITNOT_OP;}
"<<" {return BITSHL_OP;}
">>" {return BITSHR_OP;}

"print" {return PRINT_OP;}
"read" {return READ_OP;}

"%" {return MODULO_OP;}


{ID}      {yylval.text = strdup(yytext); return ID;}
{INT}		  {yylval.ival = atoi(yytext); return IDWORD_T;}
{FLOAT} 	{yylval.fval = atof(yytext); return FDWORD_T;}

"\n" {yylineno++; return NEWLINE;}
[ \t] ;
"//" ;

. {printf( "Could not recognize: %s\n", yytext );}
%%

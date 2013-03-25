%{

#include "loady.h"
#define yylval load_lval

#include <ctype.h>
#include <string.h>

int yylineno;

/* Keep -Wall quiet */
#define YY_NO_UNPUT

/*

This is needed for implementing "include", otherwise
flex buffer optimization break the includes.

*/

#define YY_INPUT(buf,result,max_size) \
        { \
        int c = fgetc(yyin); \
        result = (c == EOF) ? YY_NULL : (buf[0] = c, 1); \
        }


%}


SIGN      [\-\+]
E         [eE]
DIGIT     [0-9]
SIGNED    {SIGN}?{DIGIT}+
NUMB      {DIGIT}+
EXP       {E}{SIGNED}
IDENT     [_A-Za-z]+[_0-9\.A-Za-z]*
FLOAT1    {DIGIT}+"."{DIGIT}*{EXP}?
FLOAT2    {DIGIT}*"."{DIGIT}+{EXP}?
FLOAT3    {DIGIT}+{EXP}?

%%


\"|\'  {
           int c,q = yytext[0];

           yyleng = 0;

           while((c = input()) && c != q && c != '\n')
           {
               if(c == '\\') yytext[yyleng++] = input();
               else yytext[yyleng++] =  c;
            }

            yytext[yyleng++] = 0;
            yylval.str = strdup(yytext);
            return STRING;
        }

"missing"  return MISSING;
{IDENT}       { yylval.str = strdup(yytext); return IDENT; }
{NUMB}        { yylval.lval = atol((const char *)yytext); return INTEGER; }
{FLOAT1}      { yylval.dval = atof((const char *)yytext); return FLOAT; }
{FLOAT2}      { yylval.dval = atof((const char *)yytext); return FLOAT; }
{FLOAT3}      { yylval.dval = atof((const char *)yytext); return FLOAT; }


\#      {
           int c;
           while((c = input()) && (c != '\n')){}

        yylineno++;
        }
[ \t\r]*  ;
\n     yylineno++;


.       return *yytext;

%%


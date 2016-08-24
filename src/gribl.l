/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */


%{
#include "grib_api_internal.h"
#include "grib_yacc.h"
#include <ctype.h>

extern int yylineno;

/* Keep -Wall quiet */


/*

This is needed for implementing "include", otherwise
flex buffer optimization break the includes.

*/

#define YY_INPUT(buf,result,max_size) \
        { \
        int c = fgetc(yyin); \
        result = (c == EOF) ? YY_NULL : (buf[0] = c, 1); \
        }


/*
#ifdef __GNUC__
#ifdef FLEX_SCANNER
static void yyunput (int c,char *buf_ptr  );
void _grib_ignore_yyunput_unused_error() { yyunput(0,0); }
#endif
#endif
*/

#ifdef __cplusplus
 #define GET_INPUT yyinput
#else
 #define GET_INPUT input
#endif

%}


SIGN      [\-\+]
E         [eE]
DIGIT     [0-9]
NSIGNED   {SIGN}?{DIGIT}+
NUMB      {DIGIT}+
EXP       {E}{NSIGNED}
FLOAT1    {NSIGNED}+"."{DIGIT}*{EXP}?
FLOAT2    {NSIGNED}*"."{DIGIT}+{EXP}?
FLOAT3    {NSIGNED}+{EXP}?
IDENT1     [_A-Za-z]+[_0-9A-Za-z]*
IDENT2     "\/"{IDENT1}"\="({NSIGNED}|{FLOAT1}|{FLOAT2}|{FLOAT3})"\/"{IDENT1}
IDENT3     "\#"{NUMB}"\#"{IDENT1}
IDENT4     {NUMB}+{IDENT1}
IDENT_ATTR  "->"{IDENT1}
IDENT5    ({IDENT1}|{IDENT3}|{IDENT4})({IDENT_ATTR})+
IDENT     {IDENT1}|{IDENT2}|{IDENT3}|{IDENT4}|{IDENT5}

%%


"=="           return EQ       ;
">="           return GE       ;
">"            return GT       ;
"<="           return LE       ;
"<"            return LT       ;
"!="           return NE       ;
"<>"           return NE       ;
"bit"          return BIT      ;
"notbit"       return BITOFF   ;

"is"       return IS   ;
"not"       return NOT   ;
"!"         return NOT   ;
"and"       return AND   ;
"&&"        return AND   ;
"or"        return OR   ;
"||"        return OR   ;

"null"        return NIL   ;
"~"           return DUMMY   ;

"substr"        return SUBSTR   ;
"close"        return CLOSE   ;
"length"        return LENGTH   ;
"lowercase" return LOWERCASE;
"if"        return IF       ;
"_if"        return IF_TRANSIENT       ;
"else"      return ELSE       ;
"unsigned"  return UNSIGNED ;
"ascii"     return ASCII    ;
"byte"     return BYTE    ;
"label"     return LABEL    ;
"list"      return LIST     ;
"while"      return WHILE     ;
"template"  return TEMPLATE ;
"template_nofail"  return TEMPLATE_NOFAIL ;
"trigger"  return TRIGGER ;
"end"       return END      ;
"ibmfloat"  return IBMFLOAT ;
"ieeefloat" return FLOAT ;
"signed"    return SIGNED   ;
"codetable" return CODETABLE;
"getenv" return GETENV;
"smart_table" return SMART_TABLE;
"dict_search" return DICTIONARY;
"complex_codetable" return CODETABLE;
"flags"     return FLAG     ;
"lookup"    return LOOKUP   ;
"group"    return GROUP  ;
"non_alpha"    return NON_ALPHA   ;
"meta"      return META     ;
"padtoeven"     return PADTOEVEN    ;
"padto"     return PADTO    ;
"padtomultiple"     return PADTOMULTIPLE    ;
"pad"       return PAD      ;
"section_padding"       return SECTION_PADDING      ;
"message"       return MESSAGE      ;
"message_copy"       return MESSAGE_COPY      ;
"alias"     return ALIAS    ;
"unalias"     return UNALIAS    ;
"position"  return POS      ;
"constant"  return INTCONST ;
"transient" return TRANS    ;
"string_type" return STRING_TYPE   ;
"long_type" return LONG_TYPE   ;
"double_type" return DOUBLE_TYPE   ;
"iterator"  return ITERATOR ;
"nearest"  return NEAREST ;
"box"  return BOX ;
"ksec"      return KSEC    ;
"flagbit"      return FLAGBIT    ;
"ksec1expver" return KSEC1EXPVER    ;
"modify"      return MODIFY    ;

"g1_half_byte_codeflag" return G1_HALF_BYTE    ;
"g1_message_length" return G1_MESSAGE_LENGTH    ;
"g1_section4_length" return G1_SECTION4_LENGTH    ;

"export"     return EXPORT;
"remove"     return REMOVE;
"rename"     return RENAME;
"is_in_list"     return IS_IN_LIST;
"is_in_dict"     return IS_IN_DICT;
"to_integer"     return TO_INTEGER;
"to_string"     return TO_STRING;
"sexagesimal2decimal" return SEX2DEC;
"is_integer"     return IS_INTEGER;
"is_number"     return IS_INTEGER;

"section_length"          return SECTION_LENGTH     ;

"assert"     return ASSERT    ;

"read_only"           return READ_ONLY;
"no_copy"           return NO_COPY;
"edition_specific"    return EDITION_SPECIFIC;
"dump"                return DUMP;
"no_fail"            return NO_FAIL;
"hidden"              return HIDDEN;
"can_be_missing"      return CAN_BE_MISSING;
"MISSING"      return MISSING;
"constraint"      return CONSTRAINT;
"override"      return OVERRIDE;
"copy_ok"      return COPY_OK;

"uint8"                return UINT8;
"int"                  return INT8;

"uint16"               return UINT16;
"int16"                return INT16;
"uint16_little_endian" return UINT16_LITTLE_ENDIAN;
"int16_little_endian"  return INT16_LITTLE_ENDIAN;

"uint32"               return UINT32;
"int32"                return INT32;
"uint32_little_endian" return UINT32_LITTLE_ENDIAN;
"int32_little_endian"  return INT32_LITTLE_ENDIAN;

"uint64"               return UINT64;
"int64"                return INT64;
"uint64_little_endian" return UINT64_LITTLE_ENDIAN;
"int64_little_endian"  return INT64_LITTLE_ENDIAN;

"blob"               return BLOB;

"set"         return SET;
"set_nofail"         return SET_NOFAIL;
"when"        return WHEN;
"case"     return CASE;
"switch"     return SWITCH;
"default"     return DEFAULT;
"concept"     return CONCEPT;
"concept_nofail"     return CONCEPT_NOFAIL;
"hash_array"     return HASH_ARRAY;
"write"       return WRITE;
"append"       return APPEND;
"print"       return PRINT;
"skip"         return SKIP;

"include"       {
          int c,q;
          while((c = GET_INPUT()) && isspace(c) && c != '\n') ;
          q = c; /* the quote */

          yyleng = 0;
          while((c = GET_INPUT()) && c != q && c != '\n')
                {
                    if(c == '\\') yytext[yyleng++] = GET_INPUT();
                    else yytext[yyleng++] =  c;
                }

                yytext[yyleng++] = 0;

          grib_parser_include(yytext);
         }

\"|\'  {
           int c,q = yytext[0];

           yyleng = 0;

           while((c = GET_INPUT()) && c != q )
           {
               if(c == '\\') yytext[yyleng++] = GET_INPUT();
               else {
									 yytext[yyleng++] =  c;
									 if (c =='\n') yylineno++;
							 }
            }

            yytext[yyleng++] = 0;
            yylval.str = strdup(yytext);
            return STRING;
        }

`       {
           int c;
           unsigned long val = 0;

           while((c = GET_INPUT()) && c != '`' && c != '\n')
           {
                val <<= 8;
                val |= c;
           }
           yylval.lval = val;
           return INTEGER;
        }


{IDENT}       { yylval.str = strdup(yytext); return IDENT; }
{NUMB}        { yylval.lval = atol((const char *)yytext); return INTEGER; }
{NSIGNED}     { yylval.lval = atol((const char *)yytext); return INTEGER; }
{FLOAT1}      { yylval.dval = atof((const char *)yytext); return FLOAT; }
{FLOAT2}      { yylval.dval = atof((const char *)yytext); return FLOAT; }
{FLOAT3}      { yylval.dval = atof((const char *)yytext); return FLOAT; }


\#      {
           int c;
           while((c = GET_INPUT()) && (c != '\n')){}

        yylineno++;
        }
[ \t\r]*  ;
\n     yylineno++;



.       return *yytext;

%%


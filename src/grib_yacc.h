
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum grib_yytokentype {
     LOWERCASE = 258,
     IF = 259,
     IF_TRANSIENT = 260,
     ELSE = 261,
     END = 262,
     UNSIGNED = 263,
     TEMPLATE = 264,
     TEMPLATE_NOFAIL = 265,
     TRIGGER = 266,
     ASCII = 267,
     KSEC1EXPVER = 268,
     LABEL = 269,
     LIST = 270,
     WHILE = 271,
     IBMFLOAT = 272,
     SIGNED = 273,
     BYTE = 274,
     CODETABLE = 275,
     COMPLEX_CODETABLE = 276,
     LOOKUP = 277,
     ALIAS = 278,
     UNALIAS = 279,
     META = 280,
     POS = 281,
     INTCONST = 282,
     TRANS = 283,
     FLAGBIT = 284,
     CONCEPT = 285,
     CONCEPT_NOFAIL = 286,
     NIL = 287,
     DUMMY = 288,
     MODIFY = 289,
     READ_ONLY = 290,
     STRING_TYPE = 291,
     LONG_TYPE = 292,
     NO_COPY = 293,
     DUMP = 294,
     NO_FAIL = 295,
     EDITION_SPECIFIC = 296,
     OVERRIDE = 297,
     HIDDEN = 298,
     CAN_BE_MISSING = 299,
     MISSING = 300,
     CONSTRAINT = 301,
     COPY_OK = 302,
     WHEN = 303,
     SET = 304,
     SET_NOFAIL = 305,
     WRITE = 306,
     APPEND = 307,
     PRINT = 308,
     EXPORT = 309,
     REMOVE = 310,
     SKIP = 311,
     PAD = 312,
     SECTION_PADDING = 313,
     MESSAGE = 314,
     PADTO = 315,
     PADTOEVEN = 316,
     PADTOMULTIPLE = 317,
     G1_HALF_BYTE = 318,
     G1_MESSAGE_LENGTH = 319,
     G1_SECTION4_LENGTH = 320,
     SECTION_LENGTH = 321,
     FLAG = 322,
     ITERATOR = 323,
     NEAREST = 324,
     BOX = 325,
     KSEC = 326,
     ASSERT = 327,
     CASE = 328,
     SWITCH = 329,
     DEFAULT = 330,
     EQ = 331,
     NE = 332,
     GE = 333,
     LE = 334,
     BIT = 335,
     BITOFF = 336,
     AND = 337,
     OR = 338,
     NOT = 339,
     IS = 340,
     IDENT = 341,
     STRING = 342,
     INTEGER = 343,
     FLOAT = 344
   };
#endif
/* Tokens.  */
#define LOWERCASE 258
#define IF 259
#define IF_TRANSIENT 260
#define ELSE 261
#define END 262
#define UNSIGNED 263
#define TEMPLATE 264
#define TEMPLATE_NOFAIL 265
#define TRIGGER 266
#define ASCII 267
#define KSEC1EXPVER 268
#define LABEL 269
#define LIST 270
#define WHILE 271
#define IBMFLOAT 272
#define SIGNED 273
#define BYTE 274
#define CODETABLE 275
#define COMPLEX_CODETABLE 276
#define LOOKUP 277
#define ALIAS 278
#define UNALIAS 279
#define META 280
#define POS 281
#define INTCONST 282
#define TRANS 283
#define FLAGBIT 284
#define CONCEPT 285
#define CONCEPT_NOFAIL 286
#define NIL 287
#define DUMMY 288
#define MODIFY 289
#define READ_ONLY 290
#define STRING_TYPE 291
#define LONG_TYPE 292
#define NO_COPY 293
#define DUMP 294
#define NO_FAIL 295
#define EDITION_SPECIFIC 296
#define OVERRIDE 297
#define HIDDEN 298
#define CAN_BE_MISSING 299
#define MISSING 300
#define CONSTRAINT 301
#define COPY_OK 302
#define WHEN 303
#define SET 304
#define SET_NOFAIL 305
#define WRITE 306
#define APPEND 307
#define PRINT 308
#define EXPORT 309
#define REMOVE 310
#define SKIP 311
#define PAD 312
#define SECTION_PADDING 313
#define MESSAGE 314
#define PADTO 315
#define PADTOEVEN 316
#define PADTOMULTIPLE 317
#define G1_HALF_BYTE 318
#define G1_MESSAGE_LENGTH 319
#define G1_SECTION4_LENGTH 320
#define SECTION_LENGTH 321
#define FLAG 322
#define ITERATOR 323
#define NEAREST 324
#define BOX 325
#define KSEC 326
#define ASSERT 327
#define CASE 328
#define SWITCH 329
#define DEFAULT 330
#define EQ 331
#define NE 332
#define GE 333
#define LE 334
#define BIT 335
#define BITOFF 336
#define AND 337
#define OR 338
#define NOT 339
#define IS 340
#define IDENT 341
#define STRING 342
#define INTEGER 343
#define FLOAT 344




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 32 "griby.y"

    char                    *str;
    long                    lval;
    double                  dval;
    grib_darray             *dvalue;
    grib_iarray             *ivalue;
    grib_action             *act;
    grib_arguments          *explist;
    grib_expression         *exp;
    grib_concept_condition  *concept_condition;
    grib_concept_value      *concept_value;
	grib_case               *case_value;
  grib_rule               *rules;
  grib_rule_entry         *rule_entry;



/* Line 1676 of yacc.c  */
#line 248 "y.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define grib_yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE grib_yylval;



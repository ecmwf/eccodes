
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
     GETENV = 286,
     CONCEPT_NOFAIL = 287,
     NIL = 288,
     DUMMY = 289,
     MODIFY = 290,
     READ_ONLY = 291,
     STRING_TYPE = 292,
     LONG_TYPE = 293,
     NO_COPY = 294,
     DUMP = 295,
     NO_FAIL = 296,
     EDITION_SPECIFIC = 297,
     OVERRIDE = 298,
     HIDDEN = 299,
     CAN_BE_MISSING = 300,
     MISSING = 301,
     CONSTRAINT = 302,
     COPY_OK = 303,
     WHEN = 304,
     SET = 305,
     SET_NOFAIL = 306,
     WRITE = 307,
     APPEND = 308,
     PRINT = 309,
     EXPORT = 310,
     REMOVE = 311,
     SKIP = 312,
     PAD = 313,
     SECTION_PADDING = 314,
     MESSAGE = 315,
     PADTO = 316,
     PADTOEVEN = 317,
     PADTOMULTIPLE = 318,
     G1_HALF_BYTE = 319,
     G1_MESSAGE_LENGTH = 320,
     G1_SECTION4_LENGTH = 321,
     SECTION_LENGTH = 322,
     FLAG = 323,
     ITERATOR = 324,
     NEAREST = 325,
     BOX = 326,
     KSEC = 327,
     ASSERT = 328,
     CASE = 329,
     SWITCH = 330,
     DEFAULT = 331,
     EQ = 332,
     NE = 333,
     GE = 334,
     LE = 335,
     LT = 336,
     GT = 337,
     BIT = 338,
     BITOFF = 339,
     AND = 340,
     OR = 341,
     NOT = 342,
     IS = 343,
     IDENT = 344,
     STRING = 345,
     INTEGER = 346,
     FLOAT = 347
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
#define GETENV 286
#define CONCEPT_NOFAIL 287
#define NIL 288
#define DUMMY 289
#define MODIFY 290
#define READ_ONLY 291
#define STRING_TYPE 292
#define LONG_TYPE 293
#define NO_COPY 294
#define DUMP 295
#define NO_FAIL 296
#define EDITION_SPECIFIC 297
#define OVERRIDE 298
#define HIDDEN 299
#define CAN_BE_MISSING 300
#define MISSING 301
#define CONSTRAINT 302
#define COPY_OK 303
#define WHEN 304
#define SET 305
#define SET_NOFAIL 306
#define WRITE 307
#define APPEND 308
#define PRINT 309
#define EXPORT 310
#define REMOVE 311
#define SKIP 312
#define PAD 313
#define SECTION_PADDING 314
#define MESSAGE 315
#define PADTO 316
#define PADTOEVEN 317
#define PADTOMULTIPLE 318
#define G1_HALF_BYTE 319
#define G1_MESSAGE_LENGTH 320
#define G1_SECTION4_LENGTH 321
#define SECTION_LENGTH 322
#define FLAG 323
#define ITERATOR 324
#define NEAREST 325
#define BOX 326
#define KSEC 327
#define ASSERT 328
#define CASE 329
#define SWITCH 330
#define DEFAULT 331
#define EQ 332
#define NE 333
#define GE 334
#define LE 335
#define LT 336
#define GT 337
#define BIT 338
#define BITOFF 339
#define AND 340
#define OR 341
#define NOT 342
#define IS 343
#define IDENT 344
#define STRING 345
#define INTEGER 346
#define FLOAT 347




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 34 "griby.y"

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
#line 254 "y.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define grib_yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE grib_yylval;




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
     DOUBLE_TYPE = 294,
     NO_COPY = 295,
     DUMP = 296,
     NO_FAIL = 297,
     EDITION_SPECIFIC = 298,
     OVERRIDE = 299,
     HIDDEN = 300,
     CAN_BE_MISSING = 301,
     MISSING = 302,
     CONSTRAINT = 303,
     COPY_OK = 304,
     WHEN = 305,
     SET = 306,
     SET_NOFAIL = 307,
     WRITE = 308,
     APPEND = 309,
     PRINT = 310,
     EXPORT = 311,
     REMOVE = 312,
     SKIP = 313,
     PAD = 314,
     SECTION_PADDING = 315,
     MESSAGE = 316,
     PADTO = 317,
     PADTOEVEN = 318,
     PADTOMULTIPLE = 319,
     G1_HALF_BYTE = 320,
     G1_MESSAGE_LENGTH = 321,
     G1_SECTION4_LENGTH = 322,
     SECTION_LENGTH = 323,
     FLAG = 324,
     ITERATOR = 325,
     NEAREST = 326,
     BOX = 327,
     KSEC = 328,
     ASSERT = 329,
     CASE = 330,
     SWITCH = 331,
     DEFAULT = 332,
     EQ = 333,
     NE = 334,
     GE = 335,
     LE = 336,
     LT = 337,
     GT = 338,
     BIT = 339,
     BITOFF = 340,
     AND = 341,
     OR = 342,
     NOT = 343,
     IS = 344,
     IDENT = 345,
     STRING = 346,
     INTEGER = 347,
     FLOAT = 348
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
#define DOUBLE_TYPE 294
#define NO_COPY 295
#define DUMP 296
#define NO_FAIL 297
#define EDITION_SPECIFIC 298
#define OVERRIDE 299
#define HIDDEN 300
#define CAN_BE_MISSING 301
#define MISSING 302
#define CONSTRAINT 303
#define COPY_OK 304
#define WHEN 305
#define SET 306
#define SET_NOFAIL 307
#define WRITE 308
#define APPEND 309
#define PRINT 310
#define EXPORT 311
#define REMOVE 312
#define SKIP 313
#define PAD 314
#define SECTION_PADDING 315
#define MESSAGE 316
#define PADTO 317
#define PADTOEVEN 318
#define PADTOMULTIPLE 319
#define G1_HALF_BYTE 320
#define G1_MESSAGE_LENGTH 321
#define G1_SECTION4_LENGTH 322
#define SECTION_LENGTH 323
#define FLAG 324
#define ITERATOR 325
#define NEAREST 326
#define BOX 327
#define KSEC 328
#define ASSERT 329
#define CASE 330
#define SWITCH 331
#define DEFAULT 332
#define EQ 333
#define NE 334
#define GE 335
#define LE 336
#define LT 337
#define GT 338
#define BIT 339
#define BITOFF 340
#define AND 341
#define OR 342
#define NOT 343
#define IS 344
#define IDENT 345
#define STRING 346
#define INTEGER 347
#define FLOAT 348




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
#line 256 "y.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define grib_yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE grib_yylval;




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
     CLOSE = 263,
     UNSIGNED = 264,
     TEMPLATE = 265,
     TEMPLATE_NOFAIL = 266,
     TRIGGER = 267,
     ASCII = 268,
     GROUP = 269,
     NON_ALPHA = 270,
     KSEC1EXPVER = 271,
     LABEL = 272,
     LIST = 273,
     IS_IN_LIST = 274,
     IS_IN_DICT = 275,
     IS_INTEGER = 276,
     TO_INTEGER = 277,
     TO_STRING = 278,
     SEX2DEC = 279,
     WHILE = 280,
     IBMFLOAT = 281,
     SIGNED = 282,
     BYTE = 283,
     CODETABLE = 284,
     SMART_TABLE = 285,
     DICTIONARY = 286,
     COMPLEX_CODETABLE = 287,
     LOOKUP = 288,
     ALIAS = 289,
     UNALIAS = 290,
     META = 291,
     POS = 292,
     INTCONST = 293,
     TRANS = 294,
     FLAGBIT = 295,
     CONCEPT = 296,
     GETENV = 297,
     HASH_ARRAY = 298,
     CONCEPT_NOFAIL = 299,
     NIL = 300,
     DUMMY = 301,
     MODIFY = 302,
     READ_ONLY = 303,
     STRING_TYPE = 304,
     LONG_TYPE = 305,
     DOUBLE_TYPE = 306,
     NO_COPY = 307,
     DUMP = 308,
     JSON = 309,
     XML = 310,
     NO_FAIL = 311,
     EDITION_SPECIFIC = 312,
     OVERRIDE = 313,
     HIDDEN = 314,
     CAN_BE_MISSING = 315,
     MISSING = 316,
     CONSTRAINT = 317,
     COPY_OK = 318,
     WHEN = 319,
     SET = 320,
     SET_NOFAIL = 321,
     WRITE = 322,
     APPEND = 323,
     PRINT = 324,
     EXPORT = 325,
     REMOVE = 326,
     RENAME = 327,
     SKIP = 328,
     PAD = 329,
     SECTION_PADDING = 330,
     MESSAGE = 331,
     MESSAGE_COPY = 332,
     PADTO = 333,
     PADTOEVEN = 334,
     PADTOMULTIPLE = 335,
     G1_HALF_BYTE = 336,
     G1_MESSAGE_LENGTH = 337,
     G1_SECTION4_LENGTH = 338,
     SECTION_LENGTH = 339,
     LENGTH = 340,
     FLAG = 341,
     ITERATOR = 342,
     NEAREST = 343,
     BOX = 344,
     KSEC = 345,
     ASSERT = 346,
     SUBSTR = 347,
     CASE = 348,
     SWITCH = 349,
     DEFAULT = 350,
     EQ = 351,
     NE = 352,
     GE = 353,
     LE = 354,
     LT = 355,
     GT = 356,
     BIT = 357,
     BITOFF = 358,
     AND = 359,
     OR = 360,
     NOT = 361,
     IS = 362,
     IDENT = 363,
     STRING = 364,
     INTEGER = 365,
     FLOAT = 366
   };
#endif
/* Tokens.  */
#define LOWERCASE 258
#define IF 259
#define IF_TRANSIENT 260
#define ELSE 261
#define END 262
#define CLOSE 263
#define UNSIGNED 264
#define TEMPLATE 265
#define TEMPLATE_NOFAIL 266
#define TRIGGER 267
#define ASCII 268
#define GROUP 269
#define NON_ALPHA 270
#define KSEC1EXPVER 271
#define LABEL 272
#define LIST 273
#define IS_IN_LIST 274
#define IS_IN_DICT 275
#define IS_INTEGER 276
#define TO_INTEGER 277
#define TO_STRING 278
#define SEX2DEC 279
#define WHILE 280
#define IBMFLOAT 281
#define SIGNED 282
#define BYTE 283
#define CODETABLE 284
#define SMART_TABLE 285
#define DICTIONARY 286
#define COMPLEX_CODETABLE 287
#define LOOKUP 288
#define ALIAS 289
#define UNALIAS 290
#define META 291
#define POS 292
#define INTCONST 293
#define TRANS 294
#define FLAGBIT 295
#define CONCEPT 296
#define GETENV 297
#define HASH_ARRAY 298
#define CONCEPT_NOFAIL 299
#define NIL 300
#define DUMMY 301
#define MODIFY 302
#define READ_ONLY 303
#define STRING_TYPE 304
#define LONG_TYPE 305
#define DOUBLE_TYPE 306
#define NO_COPY 307
#define DUMP 308
#define JSON 309
#define XML 310
#define NO_FAIL 311
#define EDITION_SPECIFIC 312
#define OVERRIDE 313
#define HIDDEN 314
#define CAN_BE_MISSING 315
#define MISSING 316
#define CONSTRAINT 317
#define COPY_OK 318
#define WHEN 319
#define SET 320
#define SET_NOFAIL 321
#define WRITE 322
#define APPEND 323
#define PRINT 324
#define EXPORT 325
#define REMOVE 326
#define RENAME 327
#define SKIP 328
#define PAD 329
#define SECTION_PADDING 330
#define MESSAGE 331
#define MESSAGE_COPY 332
#define PADTO 333
#define PADTOEVEN 334
#define PADTOMULTIPLE 335
#define G1_HALF_BYTE 336
#define G1_MESSAGE_LENGTH 337
#define G1_SECTION4_LENGTH 338
#define SECTION_LENGTH 339
#define LENGTH 340
#define FLAG 341
#define ITERATOR 342
#define NEAREST 343
#define BOX 344
#define KSEC 345
#define ASSERT 346
#define SUBSTR 347
#define CASE 348
#define SWITCH 349
#define DEFAULT 350
#define EQ 351
#define NE 352
#define GE 353
#define LE 354
#define LT 355
#define GT 356
#define BIT 357
#define BITOFF 358
#define AND 359
#define OR 360
#define NOT 361
#define IS 362
#define IDENT 363
#define STRING 364
#define INTEGER 365
#define FLOAT 366




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 37 "griby.y"

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
    grib_hash_array_value      *hash_array_value;
	grib_case               *case_value;
  grib_rule               *rules;
  grib_rule_entry         *rule_entry;



/* Line 1676 of yacc.c  */
#line 293 "y.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define grib_yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE grib_yylval;



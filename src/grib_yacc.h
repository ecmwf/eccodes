/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or grib_yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int grib_yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum grib_yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    LOWERCASE = 258,               /* LOWERCASE  */
    IF = 259,                      /* IF  */
    IF_TRANSIENT = 260,            /* IF_TRANSIENT  */
    ELSE = 261,                    /* ELSE  */
    END = 262,                     /* END  */
    CLOSE = 263,                   /* CLOSE  */
    UNSIGNED = 264,                /* UNSIGNED  */
    TEMPLATE = 265,                /* TEMPLATE  */
    TEMPLATE_NOFAIL = 266,         /* TEMPLATE_NOFAIL  */
    TRIGGER = 267,                 /* TRIGGER  */
    ASCII = 268,                   /* ASCII  */
    GROUP = 269,                   /* GROUP  */
    NON_ALPHA = 270,               /* NON_ALPHA  */
    KSEC1EXPVER = 271,             /* KSEC1EXPVER  */
    LABEL = 272,                   /* LABEL  */
    LIST = 273,                    /* LIST  */
    IS_IN_LIST = 274,              /* IS_IN_LIST  */
    IS_IN_DICT = 275,              /* IS_IN_DICT  */
    IS_INTEGER = 276,              /* IS_INTEGER  */
    TO_INTEGER = 277,              /* TO_INTEGER  */
    TO_STRING = 278,               /* TO_STRING  */
    SEX2DEC = 279,                 /* SEX2DEC  */
    WHILE = 280,                   /* WHILE  */
    IBMFLOAT = 281,                /* IBMFLOAT  */
    SIGNED = 282,                  /* SIGNED  */
    UINT8 = 283,                   /* UINT8  */
    INT8 = 284,                    /* INT8  */
    UINT16 = 285,                  /* UINT16  */
    INT16 = 286,                   /* INT16  */
    UINT16_LITTLE_ENDIAN = 287,    /* UINT16_LITTLE_ENDIAN  */
    INT16_LITTLE_ENDIAN = 288,     /* INT16_LITTLE_ENDIAN  */
    UINT32 = 289,                  /* UINT32  */
    INT32 = 290,                   /* INT32  */
    UINT32_LITTLE_ENDIAN = 291,    /* UINT32_LITTLE_ENDIAN  */
    INT32_LITTLE_ENDIAN = 292,     /* INT32_LITTLE_ENDIAN  */
    UINT64 = 293,                  /* UINT64  */
    INT64 = 294,                   /* INT64  */
    UINT64_LITTLE_ENDIAN = 295,    /* UINT64_LITTLE_ENDIAN  */
    INT64_LITTLE_ENDIAN = 296,     /* INT64_LITTLE_ENDIAN  */
    BLOB = 297,                    /* BLOB  */
    BYTE = 298,                    /* BYTE  */
    CODETABLE = 299,               /* CODETABLE  */
    SMART_TABLE = 300,             /* SMART_TABLE  */
    DICTIONARY = 301,              /* DICTIONARY  */
    COMPLEX_CODETABLE = 302,       /* COMPLEX_CODETABLE  */
    LOOKUP = 303,                  /* LOOKUP  */
    ALIAS = 304,                   /* ALIAS  */
    UNALIAS = 305,                 /* UNALIAS  */
    META = 306,                    /* META  */
    POS = 307,                     /* POS  */
    INTCONST = 308,                /* INTCONST  */
    TRANS = 309,                   /* TRANS  */
    FLAGBIT = 310,                 /* FLAGBIT  */
    CONCEPT = 311,                 /* CONCEPT  */
    GETENV = 312,                  /* GETENV  */
    HASH_ARRAY = 313,              /* HASH_ARRAY  */
    CONCEPT_NOFAIL = 314,          /* CONCEPT_NOFAIL  */
    NIL = 315,                     /* NIL  */
    DUMMY = 316,                   /* DUMMY  */
    MODIFY = 317,                  /* MODIFY  */
    READ_ONLY = 318,               /* READ_ONLY  */
    STRING_TYPE = 319,             /* STRING_TYPE  */
    LONG_TYPE = 320,               /* LONG_TYPE  */
    DOUBLE_TYPE = 321,             /* DOUBLE_TYPE  */
    NO_COPY = 322,                 /* NO_COPY  */
    DUMP = 323,                    /* DUMP  */
    JSON = 324,                    /* JSON  */
    XML = 325,                     /* XML  */
    NO_FAIL = 326,                 /* NO_FAIL  */
    EDITION_SPECIFIC = 327,        /* EDITION_SPECIFIC  */
    OVERRIDE = 328,                /* OVERRIDE  */
    HIDDEN = 329,                  /* HIDDEN  */
    CAN_BE_MISSING = 330,          /* CAN_BE_MISSING  */
    MISSING = 331,                 /* MISSING  */
    CONSTRAINT = 332,              /* CONSTRAINT  */
    COPY_OK = 333,                 /* COPY_OK  */
    COPY_IF_CHANGING_EDITION = 334, /* COPY_IF_CHANGING_EDITION  */
    WHEN = 335,                    /* WHEN  */
    SET = 336,                     /* SET  */
    SET_NOFAIL = 337,              /* SET_NOFAIL  */
    WRITE = 338,                   /* WRITE  */
    APPEND = 339,                  /* APPEND  */
    PRINT = 340,                   /* PRINT  */
    EXPORT = 341,                  /* EXPORT  */
    REMOVE = 342,                  /* REMOVE  */
    RENAME = 343,                  /* RENAME  */
    SKIP = 344,                    /* SKIP  */
    PAD = 345,                     /* PAD  */
    SECTION_PADDING = 346,         /* SECTION_PADDING  */
    MESSAGE = 347,                 /* MESSAGE  */
    MESSAGE_COPY = 348,            /* MESSAGE_COPY  */
    PADTO = 349,                   /* PADTO  */
    PADTOEVEN = 350,               /* PADTOEVEN  */
    PADTOMULTIPLE = 351,           /* PADTOMULTIPLE  */
    G1_HALF_BYTE = 352,            /* G1_HALF_BYTE  */
    G1_MESSAGE_LENGTH = 353,       /* G1_MESSAGE_LENGTH  */
    G1_SECTION4_LENGTH = 354,      /* G1_SECTION4_LENGTH  */
    SECTION_LENGTH = 355,          /* SECTION_LENGTH  */
    LENGTH = 356,                  /* LENGTH  */
    FLAG = 357,                    /* FLAG  */
    ITERATOR = 358,                /* ITERATOR  */
    NEAREST = 359,                 /* NEAREST  */
    BOX = 360,                     /* BOX  */
    KSEC = 361,                    /* KSEC  */
    ASSERT = 362,                  /* ASSERT  */
    SUBSTR = 363,                  /* SUBSTR  */
    CASE = 364,                    /* CASE  */
    SWITCH = 365,                  /* SWITCH  */
    DEFAULT = 366,                 /* DEFAULT  */
    EQ = 367,                      /* EQ  */
    NE = 368,                      /* NE  */
    GE = 369,                      /* GE  */
    LE = 370,                      /* LE  */
    LT = 371,                      /* LT  */
    GT = 372,                      /* GT  */
    BIT = 373,                     /* BIT  */
    BITOFF = 374,                  /* BITOFF  */
    AND = 375,                     /* AND  */
    OR = 376,                      /* OR  */
    NOT = 377,                     /* NOT  */
    IS = 378,                      /* IS  */
    ISNOT = 379,                   /* ISNOT  */
    IDENT = 380,                   /* IDENT  */
    STRING = 381,                  /* STRING  */
    INTEGER = 382,                 /* INTEGER  */
    FLOAT = 383                    /* FLOAT  */
  };
  typedef enum grib_yytokentype grib_yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
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
#define UINT8 283
#define INT8 284
#define UINT16 285
#define INT16 286
#define UINT16_LITTLE_ENDIAN 287
#define INT16_LITTLE_ENDIAN 288
#define UINT32 289
#define INT32 290
#define UINT32_LITTLE_ENDIAN 291
#define INT32_LITTLE_ENDIAN 292
#define UINT64 293
#define INT64 294
#define UINT64_LITTLE_ENDIAN 295
#define INT64_LITTLE_ENDIAN 296
#define BLOB 297
#define BYTE 298
#define CODETABLE 299
#define SMART_TABLE 300
#define DICTIONARY 301
#define COMPLEX_CODETABLE 302
#define LOOKUP 303
#define ALIAS 304
#define UNALIAS 305
#define META 306
#define POS 307
#define INTCONST 308
#define TRANS 309
#define FLAGBIT 310
#define CONCEPT 311
#define GETENV 312
#define HASH_ARRAY 313
#define CONCEPT_NOFAIL 314
#define NIL 315
#define DUMMY 316
#define MODIFY 317
#define READ_ONLY 318
#define STRING_TYPE 319
#define LONG_TYPE 320
#define DOUBLE_TYPE 321
#define NO_COPY 322
#define DUMP 323
#define JSON 324
#define XML 325
#define NO_FAIL 326
#define EDITION_SPECIFIC 327
#define OVERRIDE 328
#define HIDDEN 329
#define CAN_BE_MISSING 330
#define MISSING 331
#define CONSTRAINT 332
#define COPY_OK 333
#define COPY_IF_CHANGING_EDITION 334
#define WHEN 335
#define SET 336
#define SET_NOFAIL 337
#define WRITE 338
#define APPEND 339
#define PRINT 340
#define EXPORT 341
#define REMOVE 342
#define RENAME 343
#define SKIP 344
#define PAD 345
#define SECTION_PADDING 346
#define MESSAGE 347
#define MESSAGE_COPY 348
#define PADTO 349
#define PADTOEVEN 350
#define PADTOMULTIPLE 351
#define G1_HALF_BYTE 352
#define G1_MESSAGE_LENGTH 353
#define G1_SECTION4_LENGTH 354
#define SECTION_LENGTH 355
#define LENGTH 356
#define FLAG 357
#define ITERATOR 358
#define NEAREST 359
#define BOX 360
#define KSEC 361
#define ASSERT 362
#define SUBSTR 363
#define CASE 364
#define SWITCH 365
#define DEFAULT 366
#define EQ 367
#define NE 368
#define GE 369
#define LE 370
#define LT 371
#define GT 372
#define BIT 373
#define BITOFF 374
#define AND 375
#define OR 376
#define NOT 377
#define IS 378
#define ISNOT 379
#define IDENT 380
#define STRING 381
#define INTEGER 382
#define FLOAT 383

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 69 "griby.y"

    char                    *str;
    long                    lval;
    double                  dval;
    grib_darray             *dvalue;
    grib_sarray             *svalue;
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

#line 341 "y.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE grib_yylval;


int grib_yyparse (void);


#endif /* !YY_YY_Y_TAB_H_INCLUDED  */

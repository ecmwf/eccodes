
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with grib_yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 12 "griby.y"


#include "grib_api_internal.h"
/* #include "grib_parser.h" */

extern int grib_yylex();
extern int grib_yyerror(const char*);

extern   grib_action*           grib_parser_all_actions;
extern   grib_concept_value*    grib_parser_concept;
extern   grib_context*          grib_parser_context;
extern   grib_rule*             grib_parser_rules;

static grib_concept_value* reverse(grib_concept_value* r);
static grib_concept_value *reverse_concept(grib_concept_value *r,grib_concept_value *s);

/* typedef int (*testp_proc)(long,long); */
/* typedef long (*grib_op_proc)(long,long);   */




/* Line 189 of yacc.c  */
#line 96 "y.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


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

/* Line 214 of yacc.c  */
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



/* Line 214 of yacc.c  */
#line 336 "y.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define grib_yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 348 "y.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 grib_yytype_uint8;
#else
typedef unsigned char grib_yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 grib_yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char grib_yytype_int8;
#else
typedef short int grib_yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 grib_yytype_uint16;
#else
typedef unsigned short int grib_yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 grib_yytype_int16;
#else
typedef short int grib_yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int grib_yyi)
#else
static int
YYID (grib_yyi)
    int grib_yyi;
#endif
{
  return grib_yyi;
}
#endif

#if ! defined grib_yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined grib_yyoverflow || YYERROR_VERBOSE */


#if (! defined grib_yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union grib_yyalloc
{
  grib_yytype_int16 grib_yyss_alloc;
  YYSTYPE grib_yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union grib_yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (grib_yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T grib_yyi;				\
	  for (grib_yyi = 0; grib_yyi < (Count); grib_yyi++)	\
	    (To)[grib_yyi] = (From)[grib_yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T grib_yynewbytes;						\
	YYCOPY (&grib_yyptr->Stack_alloc, Stack, grib_yysize);			\
	Stack = &grib_yyptr->Stack_alloc;					\
	grib_yynewbytes = grib_yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	grib_yyptr += grib_yynewbytes / sizeof (*grib_yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  162
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1307

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  111
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  45
/* YYNRULES -- Number of rules.  */
#define YYNRULES  217
/* YYNRULES -- Number of states.  */
#define YYNSTATES  689

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   348

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? grib_yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const grib_yytype_uint8 grib_yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,   109,     2,     2,
      98,    99,   107,   110,    94,   105,   101,   108,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   104,    95,
       2,   100,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    96,     2,    97,   106,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   102,     2,   103,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const grib_yytype_uint16 grib_yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    11,    13,    14,    16,
      20,    22,    26,    28,    31,    35,    38,    41,    43,    45,
      47,    49,    51,    53,    55,    57,    60,    62,    64,    66,
      70,    72,    80,    91,    99,   110,   118,   126,   134,   145,
     153,   161,   172,   180,   191,   200,   213,   224,   233,   242,
     253,   262,   272,   280,   283,   286,   291,   298,   306,   309,
     315,   321,   326,   333,   341,   344,   350,   359,   368,   372,
     378,   384,   390,   396,   403,   407,   411,   415,   421,   424,
     432,   437,   446,   457,   463,   469,   475,   481,   484,   489,
     493,   498,   503,   510,   515,   518,   520,   526,   531,   534,
     536,   542,   547,   550,   556,   558,   566,   578,   586,   598,
     605,   613,   625,   630,   635,   638,   642,   644,   647,   649,
     652,   654,   658,   660,   662,   664,   666,   668,   670,   672,
     674,   676,   678,   680,   682,   684,   686,   688,   697,   705,
     713,   720,   730,   743,   758,   769,   784,   797,   806,   818,
     825,   835,   848,   859,   874,   887,   896,   908,   910,   913,
     915,   918,   923,   934,   944,   952,   958,   964,   970,   976,
     978,   981,   986,   988,   990,   992,   994,   996,   998,  1000,
    1004,  1007,  1011,  1016,  1020,  1022,  1026,  1030,  1034,  1038,
    1042,  1044,  1048,  1052,  1054,  1058,  1062,  1066,  1070,  1074,
    1078,  1082,  1085,  1087,  1091,  1093,  1097,  1099,  1101,  1103,
    1105,  1110,  1113,  1115,  1118,  1120,  1128,  1130
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const grib_yytype_int16 grib_yyrhs[] =
{
     112,     0,    -1,   113,    -1,   134,    -1,   115,    -1,   155,
      -1,     1,    -1,    -1,    93,    -1,   114,    94,    93,    -1,
      92,    -1,   114,    94,    92,    -1,   116,    -1,   116,   115,
      -1,   116,    95,   115,    -1,   116,    95,    -1,   121,    95,
      -1,   122,    -1,   130,    -1,   131,    -1,   132,    -1,   133,
      -1,   123,    -1,   137,    -1,    95,    -1,   117,    95,    -1,
     113,    -1,   119,    -1,   120,    -1,   120,    94,   119,    -1,
     149,    -1,     8,    96,    92,    97,    90,   126,   127,    -1,
       8,    96,    92,    97,    90,    96,   118,    97,   126,   127,
      -1,     8,    98,    92,    99,    90,   126,   127,    -1,     8,
      98,    92,    99,    90,    96,   118,    97,   126,   127,    -1,
      12,    96,    92,    97,    90,   126,   127,    -1,    12,    96,
      92,    97,    91,   126,   127,    -1,    19,    96,    92,    97,
      90,   126,   127,    -1,    19,    96,    92,    97,    90,    96,
     118,    97,   126,   127,    -1,    13,    96,    92,    97,    90,
     126,   127,    -1,    18,    96,    92,    97,    90,   126,   127,
      -1,    18,    96,    92,    97,    90,    96,   118,    97,   126,
     127,    -1,    18,    98,    92,    99,    90,   126,   127,    -1,
      18,    98,    92,    99,    90,    96,   118,    97,   126,   127,
      -1,    20,    96,    92,    97,    90,   120,   126,   127,    -1,
      20,    96,    92,    97,    90,   120,   126,    51,    98,    90,
      99,   127,    -1,    20,    96,    92,    97,    90,    98,   118,
      99,   126,   127,    -1,    90,   100,    31,    98,   118,    99,
     126,   127,    -1,    21,    96,    92,    97,    90,   120,   126,
     127,    -1,    21,    96,    92,    97,    90,    98,   118,    99,
     126,   127,    -1,    69,    96,    92,    97,    90,   120,   126,
     127,    -1,    22,    96,    92,    97,    90,    98,   118,    99,
     127,    -1,    29,    90,    98,   118,    99,   126,   127,    -1,
      14,    90,    -1,    14,    91,    -1,    17,    90,   126,   127,
      -1,    17,    90,   101,    90,   126,   127,    -1,    17,    90,
      96,   120,    97,   126,   127,    -1,    26,    90,    -1,    27,
      90,   100,   120,   127,    -1,    28,    90,   100,   120,   127,
      -1,    93,    90,   126,   127,    -1,    93,    90,   101,    90,
     126,   127,    -1,    93,    90,    96,   120,    97,   126,   127,
      -1,    65,    90,    -1,    68,    96,    92,    97,    90,    -1,
      66,    96,    92,    97,    90,    98,   118,    99,    -1,    67,
      96,    92,    97,    90,    98,   118,    99,    -1,    73,    90,
     120,    -1,    59,    90,    98,   118,    99,    -1,    62,    90,
      98,   118,    99,    -1,    63,    90,    98,   118,    99,    -1,
      64,    90,    98,   118,    99,    -1,    61,    96,    92,    97,
      90,   127,    -1,    60,    90,   127,    -1,     9,    90,    91,
      -1,    10,    90,    91,    -1,    23,    90,   100,    90,   127,
      -1,    24,    90,    -1,    23,    90,   101,    90,   100,    90,
     127,    -1,    24,    90,   101,    90,    -1,    25,    90,    90,
      98,   118,    99,   126,   127,    -1,    25,    90,   101,    90,
      90,    98,   118,    99,   126,   127,    -1,    70,    90,    98,
     118,    99,    -1,    71,    90,    98,   118,    99,    -1,    72,
      90,    98,   118,    99,    -1,    56,    90,    98,   118,    99,
      -1,    57,   118,    -1,    74,    98,   149,    99,    -1,    35,
      90,   127,    -1,    51,    90,   100,    47,    -1,    51,    90,
     100,   149,    -1,    51,    90,   100,   102,   114,   103,    -1,
      52,    90,   100,   149,    -1,    53,    91,    -1,    53,    -1,
      53,    98,    92,    99,    91,    -1,    53,    98,    92,    99,
      -1,    54,    91,    -1,    54,    -1,    54,    98,    92,    99,
      91,    -1,    54,    98,    92,    99,    -1,    55,    91,    -1,
      55,    98,    91,    99,    91,    -1,    55,    -1,     4,    98,
     149,    99,   102,   115,   103,    -1,     4,    98,   149,    99,
     102,   115,   103,     6,   102,   115,   103,    -1,     5,    98,
     149,    99,   102,   115,   103,    -1,     5,    98,   149,    99,
     102,   115,   103,     6,   102,   115,   103,    -1,    50,    98,
     149,    99,   124,   117,    -1,    50,    98,   149,    99,   102,
     125,   103,    -1,    50,    98,   149,    99,   102,   125,   103,
       6,   102,   125,   103,    -1,    51,    90,   100,   149,    -1,
      52,    90,   100,   149,    -1,   124,   117,    -1,   125,   124,
     117,    -1,   113,    -1,   100,   118,    -1,   113,    -1,   104,
     128,    -1,   129,    -1,   128,    94,   129,    -1,    36,    -1,
       3,    -1,    41,    -1,    40,    -1,    42,    -1,    45,    -1,
      43,    -1,    46,    -1,    48,    -1,    44,    -1,    49,    -1,
      28,    -1,    37,    -1,    38,    -1,    39,    -1,    90,    15,
      98,   149,    99,   102,   115,   103,    -1,    16,    98,   149,
      99,   102,   115,   103,    -1,    11,    98,   118,    99,   102,
     115,   103,    -1,    30,    90,   102,   134,   103,   127,    -1,
      30,    90,    98,    90,    99,   102,   134,   103,   127,    -1,
      30,    90,    98,    90,    94,    91,    94,    90,    94,    90,
      99,   127,    -1,    30,    90,    98,    90,    94,    91,    94,
      90,    94,    90,    94,    90,    99,   127,    -1,    30,    90,
      98,    90,    94,    91,    94,    90,    99,   127,    -1,    30,
      90,   101,    90,    98,    90,    94,    91,    94,    90,    94,
      90,    99,   127,    -1,    30,    90,   101,    90,    98,    90,
      94,    91,    94,    90,    99,   127,    -1,    30,    90,   101,
      90,   102,   134,   103,   127,    -1,    30,    90,   101,    90,
      98,    90,    99,   102,   134,   103,   127,    -1,    32,    90,
     102,   134,   103,   127,    -1,    32,    90,    98,    90,    99,
     102,   134,   103,   127,    -1,    32,    90,    98,    90,    94,
      91,    94,    90,    94,    90,    99,   127,    -1,    32,    90,
      98,    90,    94,    91,    94,    90,    99,   127,    -1,    32,
      90,   101,    90,    98,    90,    94,    91,    94,    90,    94,
      90,    99,   127,    -1,    32,    90,   101,    90,    98,    90,
      94,    91,    94,    90,    99,   127,    -1,    32,    90,   101,
      90,   102,   134,   103,   127,    -1,    32,    90,   101,    90,
      98,    90,    99,   102,   134,   103,   127,    -1,   138,    -1,
     134,   138,    -1,   136,    -1,   135,   136,    -1,    75,   119,
     104,   115,    -1,    76,    98,   118,    99,   102,   135,    77,
     104,   115,   103,    -1,    76,    98,   118,    99,   102,   135,
      77,   104,   103,    -1,    76,    98,   118,    99,   102,   135,
     103,    -1,    91,   100,   102,   139,   103,    -1,    90,   100,
     102,   139,   103,    -1,    92,   100,   102,   139,   103,    -1,
      93,   100,   102,   139,   103,    -1,   140,    -1,   140,   139,
      -1,    90,   100,   149,    95,    -1,    90,    -1,    91,    -1,
     141,    -1,    92,    -1,    93,    -1,    33,    -1,    34,    -1,
      98,   149,    99,    -1,   105,   142,    -1,    90,    98,    99,
      -1,    90,    98,   118,    99,    -1,   142,   106,   143,    -1,
     142,    -1,   144,   107,   143,    -1,   144,   108,   143,    -1,
     144,   109,   143,    -1,   144,    84,   143,    -1,   144,    85,
     143,    -1,   143,    -1,   145,   110,   144,    -1,   145,   105,
     144,    -1,   144,    -1,   146,    83,   145,    -1,   146,    78,
     145,    -1,   146,    82,   145,    -1,   146,    80,   145,    -1,
     146,    81,   145,    -1,   146,    79,   145,    -1,   141,    89,
     141,    -1,    88,   146,    -1,   145,    -1,   147,    86,   146,
      -1,   146,    -1,   148,    87,   147,    -1,   147,    -1,   148,
      -1,   153,    -1,   154,    -1,    90,   100,   149,    95,    -1,
      58,    95,    -1,   151,    -1,   151,   152,    -1,   151,    -1,
       4,    98,   149,    99,   102,   152,   103,    -1,   150,    -1,
     150,   155,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const grib_yytype_uint16 grib_yyrline[] =
{
       0,   210,   210,   211,   212,   213,   215,   218,   221,   222,
     223,   224,   226,   227,   228,   229,   232,   233,   234,   235,
     236,   237,   238,   239,   243,   244,   247,   248,   251,   252,
     255,   259,   262,   265,   268,   271,   275,   278,   281,   284,
     287,   290,   293,   296,   299,   302,   306,   309,   312,   315,
     318,   321,   324,   327,   330,   333,   336,   339,   342,   345,
     348,   351,   354,   357,   360,   363,   366,   369,   372,   375,
     378,   381,   384,   387,   390,   392,   394,   397,   400,   403,
     407,   411,   414,   417,   429,   441,   453,   456,   459,   462,
     465,   466,   467,   469,   472,   473,   474,   475,   476,   477,
     478,   479,   481,   482,   483,   487,   488,   489,   490,   494,
     495,   496,   499,   500,   503,   504,   508,   509,   512,   513,
     516,   517,   520,   521,   522,   523,   524,   525,   526,   527,
     528,   529,   530,   531,   532,   533,   534,   537,   540,   543,
     546,   547,   548,   549,   550,   551,   552,   553,   554,   555,
     556,   557,   558,   559,   560,   561,   562,   566,   567,   570,
     571,   574,   578,   579,   580,   583,   585,   587,   589,   593,
     594,   597,   600,   601,   604,   605,   606,   608,   609,   610,
     611,   612,   613,   617,   618,   621,   622,   623,   624,   625,
     626,   629,   630,   631,   634,   636,   637,   638,   639,   640,
     641,   646,   647,   650,   651,   654,   655,   658,   664,   665,
     668,   669,   672,   673,   676,   680,   683,   684
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const grib_yytname[] =
{
  "$end", "error", "$undefined", "LOWERCASE", "IF", "IF_TRANSIENT",
  "ELSE", "END", "UNSIGNED", "TEMPLATE", "TEMPLATE_NOFAIL", "TRIGGER",
  "ASCII", "KSEC1EXPVER", "LABEL", "LIST", "WHILE", "IBMFLOAT", "SIGNED",
  "BYTE", "CODETABLE", "COMPLEX_CODETABLE", "LOOKUP", "ALIAS", "UNALIAS",
  "META", "POS", "INTCONST", "TRANS", "FLAGBIT", "CONCEPT", "GETENV",
  "CONCEPT_NOFAIL", "NIL", "DUMMY", "MODIFY", "READ_ONLY", "STRING_TYPE",
  "LONG_TYPE", "DOUBLE_TYPE", "NO_COPY", "DUMP", "NO_FAIL",
  "EDITION_SPECIFIC", "OVERRIDE", "HIDDEN", "CAN_BE_MISSING", "MISSING",
  "CONSTRAINT", "COPY_OK", "WHEN", "SET", "SET_NOFAIL", "WRITE", "APPEND",
  "PRINT", "EXPORT", "REMOVE", "SKIP", "PAD", "SECTION_PADDING", "MESSAGE",
  "PADTO", "PADTOEVEN", "PADTOMULTIPLE", "G1_HALF_BYTE",
  "G1_MESSAGE_LENGTH", "G1_SECTION4_LENGTH", "SECTION_LENGTH", "FLAG",
  "ITERATOR", "NEAREST", "BOX", "KSEC", "ASSERT", "CASE", "SWITCH",
  "DEFAULT", "EQ", "NE", "GE", "LE", "LT", "GT", "BIT", "BITOFF", "AND",
  "OR", "NOT", "IS", "IDENT", "STRING", "INTEGER", "FLOAT", "','", "';'",
  "'['", "']'", "'('", "')'", "'='", "'.'", "'{'", "'}'", "':'", "'-'",
  "'^'", "'*'", "'/'", "'%'", "'+'", "$accept", "all", "empty", "dvalues",
  "instructions", "instruction", "semi", "argument_list", "arguments",
  "argument", "simple", "if_block", "when_block", "set", "set_list",
  "default", "flags", "flag_list", "flag", "list_block", "while_block",
  "trigger_block", "concept_block", "concept_list", "case_list",
  "case_value", "switch_block", "concept_value", "concept_conditions",
  "concept_condition", "string_or_ident", "atom", "power", "factor",
  "term", "condition", "conjonction", "disjonction", "expression", "rule",
  "rule_entry", "rule_entries", "fact", "conditional_rule", "rules", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const grib_yytype_uint16 grib_yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,    44,    59,    91,    93,    40,    41,
      61,    46,   123,   125,    58,    45,    94,    42,    47,    37,
      43
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const grib_yytype_uint8 grib_yyr1[] =
{
       0,   111,   112,   112,   112,   112,   112,   113,   114,   114,
     114,   114,   115,   115,   115,   115,   116,   116,   116,   116,
     116,   116,   116,   116,   117,   117,   118,   118,   119,   119,
     120,   121,   121,   121,   121,   121,   121,   121,   121,   121,
     121,   121,   121,   121,   121,   121,   121,   121,   121,   121,
     121,   121,   121,   121,   121,   121,   121,   121,   121,   121,
     121,   121,   121,   121,   121,   121,   121,   121,   121,   121,
     121,   121,   121,   121,   121,   121,   121,   121,   121,   121,
     121,   121,   121,   121,   121,   121,   121,   121,   121,   121,
     121,   121,   121,   121,   121,   121,   121,   121,   121,   121,
     121,   121,   121,   121,   121,   122,   122,   122,   122,   123,
     123,   123,   124,   124,   125,   125,   126,   126,   127,   127,
     128,   128,   129,   129,   129,   129,   129,   129,   129,   129,
     129,   129,   129,   129,   129,   129,   129,   130,   131,   132,
     133,   133,   133,   133,   133,   133,   133,   133,   133,   133,
     133,   133,   133,   133,   133,   133,   133,   134,   134,   135,
     135,   136,   137,   137,   137,   138,   138,   138,   138,   139,
     139,   140,   141,   141,   142,   142,   142,   142,   142,   142,
     142,   142,   142,   143,   143,   144,   144,   144,   144,   144,
     144,   145,   145,   145,   146,   146,   146,   146,   146,   146,
     146,   146,   146,   147,   147,   148,   148,   149,   150,   150,
     151,   151,   152,   152,   153,   154,   155,   155
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const grib_yytype_uint8 grib_yyr2[] =
{
       0,     2,     1,     1,     1,     1,     1,     0,     1,     3,
       1,     3,     1,     2,     3,     2,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     1,     1,     1,     3,
       1,     7,    10,     7,    10,     7,     7,     7,    10,     7,
       7,    10,     7,    10,     8,    12,    10,     8,     8,    10,
       8,     9,     7,     2,     2,     4,     6,     7,     2,     5,
       5,     4,     6,     7,     2,     5,     8,     8,     3,     5,
       5,     5,     5,     6,     3,     3,     3,     5,     2,     7,
       4,     8,    10,     5,     5,     5,     5,     2,     4,     3,
       4,     4,     6,     4,     2,     1,     5,     4,     2,     1,
       5,     4,     2,     5,     1,     7,    11,     7,    11,     6,
       7,    11,     4,     4,     2,     3,     1,     2,     1,     2,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     8,     7,     7,
       6,     9,    12,    14,    10,    14,    12,     8,    11,     6,
       9,    12,    10,    14,    12,     8,    11,     1,     2,     1,
       2,     4,    10,     9,     7,     5,     5,     5,     5,     1,
       2,     4,     1,     1,     1,     1,     1,     1,     1,     3,
       2,     3,     4,     3,     1,     3,     3,     3,     3,     3,
       1,     3,     3,     1,     3,     3,     3,     3,     3,     3,
       3,     2,     1,     3,     1,     3,     1,     1,     1,     1,
       4,     2,     1,     2,     1,     7,     1,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const grib_yytype_uint8 grib_yydefact[] =
{
       0,     6,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    95,    99,   104,     0,     7,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     2,
       4,    12,     0,    17,    22,    18,    19,    20,    21,     3,
      23,   157,   216,   214,   208,   209,     5,     0,     0,     0,
       0,     0,     0,     7,     0,     0,    53,    54,     0,     7,
       0,     0,     0,     0,     0,     0,     0,    78,     0,    58,
       0,     0,     0,     0,     0,     7,     0,     0,     0,    94,
       0,    98,     0,   102,     0,     0,   177,   178,     0,   172,
     173,   175,   176,     0,     0,    26,    87,    27,    28,   174,
     184,   190,   193,   202,   204,   206,   207,    30,   211,     0,
       7,     0,     0,     0,     0,    64,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     7,     0,     0,     0,     0,
       7,     0,     1,     0,     0,     0,    15,    13,    16,     0,
       0,   158,     0,     0,   217,     0,     0,     0,     0,    75,
      76,     0,     0,     0,     0,     0,     7,     0,   116,     7,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     7,     0,     0,     0,     0,     0,     0,
       0,   118,    89,     0,     0,     0,     0,     0,     0,     7,
     201,     0,     0,   174,   180,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     7,    74,     0,     7,     7,     7,     0,
       0,     0,     0,     7,     7,     7,    68,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     7,     0,     0,
       0,    14,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   117,     7,    55,     0,     0,     0,
       0,     0,     0,     7,     0,    80,     7,     0,     7,     7,
       0,     0,     0,     0,     0,     0,     0,   123,   133,   122,
     134,   135,   136,   125,   124,   126,   128,   131,   127,   129,
     130,   132,   119,   120,     0,    90,     0,    91,    93,    97,
     101,     0,     0,   181,     0,   179,    29,   172,   200,   183,
     188,   189,   185,   186,   187,   192,   191,   195,   199,   197,
     198,   196,   194,   203,   205,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    88,     0,     0,
       7,     0,     0,   169,   210,     0,     0,     0,     7,    61,
       0,     0,     0,     0,     0,     7,     7,     0,     7,     7,
       7,     0,     7,     7,     7,     7,     7,     0,     0,     0,
      77,     0,     0,     0,    59,    60,     7,     0,     0,     0,
       0,     7,     0,     0,     0,     0,     7,     0,     0,     0,
       0,     0,    10,     8,     0,    96,   100,   103,    86,   182,
      69,     7,    70,    71,    72,     0,     0,    65,     0,    83,
      84,    85,     0,     0,     0,     0,   166,   170,   165,   167,
       7,     7,   168,     0,     0,     0,     0,   212,     0,     0,
       7,     7,     7,     7,     0,     7,     7,     7,     0,     7,
      56,     7,     7,     7,     7,     7,     7,     7,     7,     7,
       7,     7,     7,     7,     7,     7,     0,     0,     0,     0,
     140,     0,     0,     0,     0,   149,   121,     0,     0,     0,
       0,    24,   109,     0,    92,    73,     7,     7,     7,     0,
       0,   159,     0,     7,     0,     7,    62,     0,     0,     0,
     105,   213,   215,   107,     0,    31,     0,    33,   139,    35,
      36,    39,   138,    57,     0,    40,     0,    42,     0,    37,
       0,    30,     7,     0,     7,     0,    79,     7,     0,    52,
       0,     0,     0,     0,     7,     0,     0,     0,     0,     7,
       0,     0,   114,   110,     0,    25,    11,     9,     0,     0,
       7,     0,     0,   164,   160,     0,     7,   171,    63,     0,
       0,     7,     7,     7,     7,     7,     7,     0,    44,     7,
      48,     7,    81,     7,     0,     7,     0,     0,   147,     0,
       7,     0,     0,   155,   112,   113,     0,   115,    66,    67,
      50,     0,     0,   137,    47,     0,     0,     7,     7,     7,
       7,     7,     7,     0,     7,    51,     7,     0,     7,   141,
       0,     0,     0,     7,   150,     0,     0,     0,   161,   163,
       0,     0,     0,    32,    34,    41,    43,    38,    46,     0,
      49,    82,     0,   144,     0,     7,     0,   152,     0,     7,
       0,   162,   106,   108,     7,     0,     7,     0,     7,   148,
       7,     0,     7,   156,   111,    45,     0,   142,     0,   146,
     151,     0,   154,     7,     7,     7,   143,   145,   153
};

/* YYDEFGOTO[NTERM-NUM].  */
static const grib_yytype_int16 grib_yydefgoto[] =
{
      -1,    58,   211,   424,   456,    61,   502,   126,   127,   128,
      62,    63,    64,   499,   500,   189,   212,   322,   323,    65,
      66,    67,    68,    69,   510,   511,    70,    71,   372,   373,
     129,   130,   131,   132,   133,   134,   135,   136,   137,    72,
     457,   458,    74,    75,    76
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -489
static const grib_yytype_int16 grib_yypact[] =
{
     945,  -489,   -33,   -31,   -50,   -52,     5,   -14,     9,    28,
      -8,     4,    61,   -41,    58,    73,   100,   109,    71,    99,
     125,   137,   140,   155,   168,   176,   193,   195,   189,   214,
     215,   -47,   -23,   -21,   222,   179,   219,   223,   225,   221,
     229,   230,   231,   232,   235,   237,   242,   255,   234,   249,
     262,   263,   269,   270,    -7,   227,   261,   -57,   369,  -489,
    -489,  1035,   277,  -489,  -489,  -489,  -489,  -489,  -489,   206,
    -489,  -489,     1,  -489,  -489,  -489,  -489,   179,   179,   281,
     282,   284,   287,   179,   289,   290,  -489,  -489,   179,    19,
     291,   294,   297,   302,   306,   308,   121,   278,   -64,  -489,
     307,   309,   310,   105,   139,   311,   179,   316,   317,  -489,
     319,  -489,   327,  -489,   330,   324,  -489,  -489,   179,   331,
    -489,  -489,  -489,   179,   183,  -489,  -489,  -489,   342,   358,
     343,  -489,   -45,   -39,   363,   364,   365,  -489,  -489,   353,
     311,   366,   357,   361,   370,  -489,   371,   372,   384,   385,
     381,   391,   395,   179,   179,   179,   396,    16,   354,   359,
      72,   360,  -489,   400,    -6,   393,  1214,  -489,  -489,   399,
     401,  -489,   402,   409,  -489,   411,   412,   419,   418,  -489,
    -489,   420,   421,   431,   432,   179,   179,   440,  -489,   311,
     435,   447,   437,   452,   453,   454,   462,   463,   464,   458,
     467,   179,   179,   179,   468,   469,   206,   471,   472,   206,
     499,  -489,  -489,   465,   141,   179,   466,   470,   474,   179,
     363,    65,   475,  -489,  -489,   179,   145,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   179,   179,   179,  -489,   473,   179,   179,   179,   478,
     479,   482,   483,   179,   179,   179,  -489,   485,   486,   179,
     484,   476,   491,   476,   476,   179,   477,   311,   476,   179,
     532,  -489,   492,   179,   179,   493,   495,   481,   497,   500,
     166,   509,   501,   504,  -489,   505,  -489,   514,   516,   517,
     518,   519,   520,   311,   513,  -489,   179,   524,   311,   311,
     521,   -25,   -67,    20,   -13,    -1,    88,  -489,  -489,  -489,
    -489,  -489,  -489,  -489,  -489,  -489,  -489,  -489,  -489,  -489,
    -489,  -489,   487,  -489,   -22,  -489,   172,  -489,  -489,   526,
     528,   530,   523,  -489,   525,  -489,  -489,  -489,  -489,  -489,
    -489,  -489,  -489,  -489,  -489,   -45,   -45,   -39,   -39,   -39,
     -39,   -39,   -39,   363,   364,   527,   533,   536,   538,   539,
     535,   537,   541,   550,   543,   545,   547,  -489,   546,   551,
     179,   529,   548,   476,  -489,   552,   554,   556,   505,  -489,
     558,   555,   560,  1127,  1214,    90,   124,  1214,   505,   505,
     505,  1214,   505,   311,   142,   163,   182,   218,   257,   565,
    -489,   575,   568,   570,  -489,  -489,   505,   579,   569,   582,
     206,   311,   583,   571,   585,   206,   311,   499,   586,   587,
     228,   584,  -489,  -489,   -49,  -489,  -489,  -489,  -489,  -489,
    -489,   311,  -489,  -489,  -489,   580,   588,  -489,   179,  -489,
    -489,  -489,   605,   581,   589,   179,  -489,  -489,  -489,  -489,
     505,   311,  -489,   591,   592,     3,   578,   -34,   593,   594,
     179,   311,   179,   311,   595,   311,   311,   311,   596,   311,
    -489,   179,   311,   179,   311,   179,   311,   179,   505,   179,
     505,   179,   311,   505,   179,   311,   590,   206,    23,    92,
    -489,   601,   206,    49,   108,  -489,  -489,   600,   602,   584,
     -30,  -489,   606,   201,  -489,  -489,   179,   179,   505,   179,
     -43,  -489,  1214,   505,   608,   311,  -489,  1214,   -34,    54,
     562,  -489,  -489,   679,   607,  -489,   609,  -489,  -489,  -489,
    -489,  -489,  -489,  -489,   610,  -489,   611,  -489,   613,  -489,
     614,   475,   -32,   616,   311,   617,  -489,   311,   618,  -489,
     597,   251,   598,   603,   311,   622,   267,   599,   620,   311,
     179,   179,   606,   712,   584,  -489,  -489,  -489,   621,   624,
     311,   615,   623,  -489,  -489,   625,   311,  -489,  -489,   627,
     628,   505,   505,   505,   505,   505,   505,   626,  -489,   505,
    -489,   311,  -489,   505,    66,   311,   631,   206,  -489,    77,
     311,   637,   206,  -489,  -489,  -489,   630,   606,  -489,  -489,
    -489,  1214,   849,  -489,  -489,  1214,  1214,   311,   311,   311,
     311,   311,   311,   643,   311,  -489,   311,   644,   311,  -489,
     645,   273,   646,   311,  -489,   648,   334,   228,  -489,  -489,
     636,   638,   639,  -489,  -489,  -489,  -489,  -489,  -489,   641,
    -489,  -489,    93,  -489,   103,   311,   647,  -489,   120,   311,
     -24,  -489,  -489,  -489,   311,   654,   311,   655,   311,  -489,
     311,   657,   311,  -489,  -489,  -489,   649,  -489,   650,  -489,
    -489,   651,  -489,   311,   311,   311,  -489,  -489,  -489
};

/* YYPGOTO[NTERM-NUM].  */
static const grib_yytype_int16 grib_yypgoto[] =
{
    -489,  -489,     7,  -489,     0,  -489,  -488,   185,  -223,  -149,
    -489,  -489,  -489,  -323,   114,   -60,   167,  -489,   335,  -489,
    -489,  -489,  -489,  -192,  -489,   244,  -489,   -59,  -248,  -489,
     -99,   629,   243,    68,   268,  -115,   515,  -489,   -65,  -489,
       6,   298,  -489,  -489,   684
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -8
static const grib_yytype_int16 grib_yytable[] =
{
      60,   421,   336,   220,   256,   172,    73,    59,   156,   156,
     171,   562,   175,   176,   303,   375,   376,   306,   156,   587,
     380,   418,   419,   184,    36,   223,   199,   418,   419,   418,
     419,   409,   509,   160,   572,   410,   283,   200,    81,   228,
     229,   213,   125,   161,   109,   503,    79,   260,    80,   116,
     117,   110,   298,   299,   504,    90,   173,    91,   222,    36,
     573,   167,   230,   231,   232,    77,   233,    78,   111,   407,
     113,   234,   210,   563,   408,   112,   607,   114,    73,   674,
     420,   412,    86,    87,    83,   260,   413,   116,   117,   257,
     125,   173,   262,   157,   270,    82,   188,   414,   116,   117,
     267,   415,    88,   519,   118,    84,   119,   120,   121,   122,
     169,    55,    56,   170,   123,   185,   377,   552,   261,   186,
     187,   124,   553,   411,    85,   447,   353,   338,   223,   223,
     223,   223,   223,   223,   223,   223,   223,   223,   223,   223,
     223,   223,   118,   557,   119,   120,   121,   122,   558,   327,
     328,    89,   123,   118,    92,   119,   120,   121,   122,   124,
     627,    96,   125,   123,   333,   628,   271,   188,   265,    93,
     124,   632,   186,   266,   116,   117,   633,   564,   169,    55,
      56,   170,   169,    55,    56,   170,   460,   665,   325,    97,
     186,   416,   666,   125,   369,   554,    94,   667,   169,    55,
      56,   170,   668,   204,   381,    95,   205,   206,   382,   262,
     125,   559,   116,   117,   671,    98,   116,   117,   489,   672,
     462,   196,   197,   494,   186,   393,   125,    99,   125,   118,
     100,   119,   120,   121,   122,   337,   120,   207,   471,   123,
     208,   209,   186,   326,   171,   101,   124,   171,   478,   480,
     125,   116,   117,   125,   125,   125,   388,   389,   102,   473,
     125,   125,   125,   186,   422,   423,   103,   118,   181,   119,
     120,   121,   122,   119,   120,   121,   122,   123,   475,   418,
     419,   123,   186,   104,   124,   105,   571,   106,   124,   508,
     116,   117,   188,   566,   567,   551,   169,    55,    56,   170,
     556,   345,   346,   125,   107,   108,   118,   244,   119,   120,
     121,   122,   115,   139,   138,   140,   477,   141,   451,   142,
     143,   144,   145,   124,   150,   461,   463,   158,   465,   466,
     467,   146,   469,   147,   472,   474,   476,   564,   148,   151,
     258,   169,    55,    56,   170,   118,   485,   119,   120,   121,
     122,   149,   152,   153,   595,   479,   286,   169,    55,    56,
     170,   159,   124,   169,    55,    56,   170,   154,   155,   162,
     600,   284,   168,   177,   178,   179,   655,   125,   180,   198,
     514,   182,   183,   190,   459,   188,   191,   464,   300,   192,
     515,   468,   188,   188,   193,   188,   188,   188,   194,   188,
     195,   188,   188,   188,   332,   631,   334,   201,   203,   202,
     636,   216,   541,   188,   541,   210,   214,   215,   542,   217,
     544,   218,   219,   547,   169,    55,    56,   170,   355,   221,
     171,   357,   358,   359,   379,   171,   225,   659,   364,   365,
     366,   235,   236,   237,   238,   239,   240,   226,   570,   227,
     241,   243,   242,   576,   262,   246,   263,   188,   245,   247,
     400,   264,   268,   249,   250,   404,   405,   125,   248,   125,
     339,   340,   341,   342,   343,   344,   251,   252,   125,   253,
     125,   402,   125,   160,   125,   188,   125,   188,   125,   254,
     188,   125,   171,   255,   259,   604,   605,   171,   269,   272,
     273,   161,   307,   347,   348,   349,   350,   351,   352,   274,
     275,   276,   575,   125,   125,   188,   277,   278,   280,   279,
     188,   617,   618,   619,   620,   621,   622,   308,   281,   624,
     285,   282,   287,   626,   289,   309,   310,   311,   312,   313,
     314,   315,   316,   317,   318,   319,   288,   320,   321,   290,
     291,   292,   293,   294,   295,   444,   296,   297,   301,   302,
     470,   304,   305,   260,   324,   329,   371,   378,   579,   330,
     356,   385,   171,   331,   335,   360,   361,   171,   490,   362,
     363,   417,   370,   495,   367,   368,   374,   386,   188,   188,
     188,   188,   188,   188,   261,   383,   188,   384,   505,   390,
     188,   392,   387,   391,   394,   186,   395,   396,   397,   398,
     399,   638,   640,   401,   403,   641,   642,   425,   516,   426,
     406,   427,   428,   431,   429,   435,   430,   436,   525,   445,
     527,   437,   529,   530,   531,   432,   533,   433,   434,   535,
     438,   537,   439,   539,   440,   524,   441,   526,   442,   546,
     443,   446,   549,   450,   453,   448,   534,   449,   536,   454,
     538,   452,   540,   481,   543,   482,   545,   483,   484,   548,
     486,   487,   488,   492,   491,   493,   497,   498,   506,   501,
     509,   520,   578,   512,   550,   580,   507,   594,   513,   596,
     601,   568,   569,   517,   518,   555,   522,   523,   528,   532,
     560,   565,   561,   577,   581,   597,   582,   583,   584,   588,
     585,   590,   599,   586,   592,   589,   591,   593,   606,   611,
     608,   598,   602,   609,   623,   630,   603,   612,   613,   615,
     616,   635,   637,   649,   652,   654,   656,   610,   658,   661,
     664,   662,   663,   614,   676,   678,   670,   681,   683,   684,
     685,   660,   496,   224,   574,   521,   174,   354,   625,     0,
       0,     0,   629,     0,     0,     0,     0,   634,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   643,   644,   645,   646,   647,   648,
       0,   650,     0,   651,     0,   653,     0,     0,     0,     0,
     657,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   669,     0,     0,     0,   673,     0,     0,     0,
       0,   675,     0,   677,     0,   679,     0,   680,     0,   682,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     686,   687,   688,   163,     3,     0,     0,     4,     5,     6,
       7,     8,     9,    10,     0,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
       0,    26,     0,     0,    27,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    28,
      29,    30,    31,    32,    33,    34,    35,     0,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,     0,    53,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   164,
       0,     0,   165,     0,     0,    -7,     1,     0,     0,     2,
       3,     0,   639,     4,     5,     6,     7,     8,     9,    10,
       0,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,     0,    26,     0,     0,
      27,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
       0,    53,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    54,    55,    56,    57,   163,
       3,     0,     0,     4,     5,     6,     7,     8,     9,    10,
       0,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,     0,    26,     0,     0,
      27,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    28,    29,    30,    31,    32,
      33,    34,    35,     0,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
       0,    53,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   164,     0,     0,   165,     0,
     166,   163,     3,     0,     0,     4,     5,     6,     7,     8,
       9,    10,     0,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,     0,    26,
       0,     0,    27,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,     0,    53,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   455,   163,     3,
     165,     0,     4,     5,     6,     7,     8,     9,    10,     0,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,     0,    26,     0,     0,    27,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    28,    29,    30,    31,    32,    33,
      34,    35,     0,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,     0,
      53,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   164,     0,     0,   165
};

static const grib_yytype_int16 grib_yycheck[] =
{
       0,   324,   225,   118,   153,     4,     0,     0,    15,    15,
      69,   499,    77,    78,   206,   263,   264,   209,    15,    51,
     268,    51,    52,    88,    58,   124,    90,    51,    52,    51,
      52,    98,    75,    90,    77,   102,   185,   101,    90,    84,
      85,   106,    35,   100,    91,    94,    96,    31,    98,    33,
      34,    98,   201,   202,   103,    96,    90,    98,   123,    58,
     103,    61,   107,   108,   109,    98,   105,    98,    91,    94,
      91,   110,   104,   103,    99,    98,   564,    98,    72,   103,
     102,    94,    90,    91,    98,    31,    99,    33,    34,   154,
      83,    90,   157,   100,   100,    90,    89,    98,    33,    34,
     160,   102,    98,   100,    88,    96,    90,    91,    92,    93,
      90,    91,    92,    93,    98,    96,   265,    94,   102,   100,
     101,   105,    99,   103,    96,   373,   241,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,   238,
     239,   240,    88,    94,    90,    91,    92,    93,    99,   214,
     215,    90,    98,    88,    96,    90,    91,    92,    93,   105,
      94,    90,   155,    98,    99,    99,   166,   160,    96,    96,
     105,    94,   100,   101,    33,    34,    99,   500,    90,    91,
      92,    93,    90,    91,    92,    93,    96,    94,    47,    90,
     100,   103,    99,   186,   259,   103,    96,    94,    90,    91,
      92,    93,    99,    98,   269,    96,   101,   102,   273,   274,
     203,   103,    33,    34,    94,    90,    33,    34,   410,    99,
      96,   100,   101,   415,   100,   285,   219,    90,   221,    88,
      90,    90,    91,    92,    93,    90,    91,    98,    96,    98,
     101,   102,   100,   102,   303,    90,   105,   306,   397,   398,
     243,    33,    34,   246,   247,   248,    90,    91,    90,    96,
     253,   254,   255,   100,    92,    93,    90,    88,    83,    90,
      91,    92,    93,    90,    91,    92,    93,    98,    96,    51,
      52,    98,   100,    90,   105,    90,   509,    98,   105,   438,
      33,    34,   285,    92,    93,   487,    90,    91,    92,    93,
     492,   233,   234,   296,    90,    90,    88,   140,    90,    91,
      92,    93,    90,    90,    95,    90,    98,    96,   378,    90,
      90,    90,    90,   105,    90,   385,   386,   100,   388,   389,
     390,    96,   392,    96,   394,   395,   396,   660,    96,    90,
     155,    90,    91,    92,    93,    88,   406,    90,    91,    92,
      93,    96,    90,    90,   103,    98,   189,    90,    91,    92,
      93,   100,   105,    90,    91,    92,    93,    98,    98,     0,
     103,   186,    95,    92,    92,    91,   103,   370,    91,   101,
     445,    92,    92,    92,   384,   378,    92,   387,   203,    92,
     450,   391,   385,   386,    92,   388,   389,   390,    92,   392,
      92,   394,   395,   396,   219,   597,   221,   100,    98,   100,
     602,    92,   477,   406,   479,   104,   100,   100,   478,    92,
     480,    91,    98,   483,    90,    91,    92,    93,   243,    98,
     489,   246,   247,   248,   267,   494,    94,   103,   253,   254,
     255,    78,    79,    80,    81,    82,    83,    89,   508,   106,
      86,    98,    87,   513,   519,    98,   102,   450,    92,    98,
     293,   102,   102,    92,    92,   298,   299,   460,    98,   462,
     227,   228,   229,   230,   231,   232,    92,    92,   471,    98,
     473,   296,   475,    90,   477,   478,   479,   480,   481,    98,
     483,   484,   551,    98,    98,   560,   561,   556,    98,   100,
      98,   100,     3,   235,   236,   237,   238,   239,   240,   100,
      99,    99,   512,   506,   507,   508,    97,    99,    97,    99,
     513,   581,   582,   583,   584,   585,   586,    28,    97,   589,
      90,    99,    97,   593,    97,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    99,    48,    49,    97,
      97,    97,    90,    90,    90,   370,    98,    90,    90,    90,
     393,    90,    90,    31,    99,    99,    90,    90,     6,    99,
      97,    90,   631,    99,    99,    97,    97,   636,   411,    97,
      97,    94,    98,   416,    99,    99,    95,    90,   581,   582,
     583,   584,   585,   586,   102,   102,   589,   102,   431,    90,
     593,    97,   102,   102,    90,   100,    90,    90,    90,    90,
      90,   611,   612,   100,    90,   615,   616,    91,   451,    91,
      99,    91,    99,    90,    99,    90,    99,    90,   461,   100,
     463,    90,   465,   466,   467,    99,   469,    99,    99,   472,
      90,   474,    99,   476,    99,   460,    99,   462,   102,   482,
      99,   103,   485,    97,    99,   103,   471,   103,   473,    99,
     475,   103,   477,    98,   479,    90,   481,    99,    98,   484,
      91,   102,    90,   102,    91,    90,    90,    90,    98,    95,
      75,   103,   515,   102,    94,     6,    98,    90,    99,    91,
      91,   506,   507,   102,   102,    94,   103,   103,   103,   103,
     100,    95,   100,    95,    97,   102,    97,    97,    97,   542,
      97,   544,    90,    99,   547,    99,    99,    99,     6,   104,
      99,   554,   102,    99,    98,    94,   559,   104,   103,   102,
     102,    94,   102,    90,    90,    90,    90,   570,    90,   103,
      99,   103,   103,   576,    90,    90,    99,    90,    99,    99,
      99,   637,   417,   124,   510,   457,    72,   242,   591,    -1,
      -1,    -1,   595,    -1,    -1,    -1,    -1,   600,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   617,   618,   619,   620,   621,   622,
      -1,   624,    -1,   626,    -1,   628,    -1,    -1,    -1,    -1,
     633,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   655,    -1,    -1,    -1,   659,    -1,    -1,    -1,
      -1,   664,    -1,   666,    -1,   668,    -1,   670,    -1,   672,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     683,   684,   685,     4,     5,    -1,    -1,     8,     9,    10,
      11,    12,    13,    14,    -1,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      -1,    32,    -1,    -1,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    50,
      51,    52,    53,    54,    55,    56,    57,    -1,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    -1,    76,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    90,
      -1,    -1,    93,    -1,    -1,     0,     1,    -1,    -1,     4,
       5,    -1,   103,     8,     9,    10,    11,    12,    13,    14,
      -1,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    -1,    32,    -1,    -1,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      -1,    76,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    90,    91,    92,    93,     4,
       5,    -1,    -1,     8,     9,    10,    11,    12,    13,    14,
      -1,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    -1,    32,    -1,    -1,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    50,    51,    52,    53,    54,
      55,    56,    57,    -1,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      -1,    76,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    90,    -1,    -1,    93,    -1,
      95,     4,     5,    -1,    -1,     8,     9,    10,    11,    12,
      13,    14,    -1,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    -1,    32,
      -1,    -1,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    -1,    76,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    90,     4,     5,
      93,    -1,     8,     9,    10,    11,    12,    13,    14,    -1,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    -1,    32,    -1,    -1,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    50,    51,    52,    53,    54,    55,
      56,    57,    -1,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    -1,
      76,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    90,    -1,    -1,    93
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const grib_yytype_uint8 grib_yystos[] =
{
       0,     1,     4,     5,     8,     9,    10,    11,    12,    13,
      14,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    32,    35,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    76,    90,    91,    92,    93,   112,   113,
     115,   116,   121,   122,   123,   130,   131,   132,   133,   134,
     137,   138,   150,   151,   153,   154,   155,    98,    98,    96,
      98,    90,    90,    98,    96,    96,    90,    91,    98,    90,
      96,    98,    96,    96,    96,    96,    90,    90,    90,    90,
      90,    90,    90,    90,    90,    90,    98,    90,    90,    91,
      98,    91,    98,    91,    98,    90,    33,    34,    88,    90,
      91,    92,    93,    98,   105,   113,   118,   119,   120,   141,
     142,   143,   144,   145,   146,   147,   148,   149,    95,    90,
      90,    96,    90,    90,    90,    90,    96,    96,    96,    96,
      90,    90,    90,    90,    98,    98,    15,   100,   100,   100,
      90,   100,     0,     4,    90,    93,    95,   115,    95,    90,
      93,   138,     4,    90,   155,   149,   149,    92,    92,    91,
      91,   118,    92,    92,   149,    96,   100,   101,   113,   126,
      92,    92,    92,    92,    92,    92,   100,   101,   101,    90,
     101,   100,   100,    98,    98,   101,   102,    98,   101,   102,
     104,   113,   127,   149,   100,   100,    92,    92,    91,    98,
     146,    98,   149,   141,   142,    94,    89,   106,    84,    85,
     107,   108,   109,   105,   110,    78,    79,    80,    81,    82,
      83,    86,    87,    98,   127,    92,    98,    98,    98,    92,
      92,    92,    92,    98,    98,    98,   120,   149,   118,    98,
      31,   102,   149,   102,   102,    96,   101,   126,   102,    98,
     100,   115,   100,    98,   100,    99,    99,    97,    99,    99,
      97,    97,    99,   120,   118,    90,   127,    97,    99,    97,
      97,    97,    97,    90,    90,    90,    98,    90,   120,   120,
     118,    90,    90,   134,    90,    90,   134,     3,    28,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      48,    49,   128,   129,    99,    47,   102,   149,   149,    99,
      99,    99,   118,    99,   118,    99,   119,    90,   141,   143,
     143,   143,   143,   143,   143,   144,   144,   145,   145,   145,
     145,   145,   145,   146,   147,   118,    97,   118,   118,   118,
      97,    97,    97,    97,   118,   118,   118,    99,    99,   149,
      98,    90,   139,   140,    95,   139,   139,   120,    90,   127,
     139,   149,   149,   102,   102,    90,    90,   102,    90,    91,
      90,   102,    97,   126,    90,    90,    90,    90,    90,    90,
     127,   100,   118,    90,   127,   127,    99,    94,    99,    98,
     102,   103,    94,    99,    98,   102,   103,    94,    51,    52,
     102,   124,    92,    93,   114,    91,    91,    91,    99,    99,
      99,    90,    99,    99,    99,    90,    90,    90,    90,    99,
      99,    99,   102,    99,   118,   100,   103,   139,   103,   103,
      97,   126,   103,    99,    99,    90,   115,   151,   152,   115,
      96,   126,    96,   126,   115,   126,   126,   126,   115,   126,
     127,    96,   126,    96,   126,    96,   126,    98,   120,    98,
     120,    98,    90,    99,    98,   126,    91,   102,    90,   134,
     127,    91,   102,    90,   134,   127,   129,    90,    90,   124,
     125,    95,   117,    94,   103,   127,    98,    98,   120,    75,
     135,   136,   102,    99,   149,   126,   127,   102,   102,   100,
     103,   152,   103,   103,   118,   127,   118,   127,   103,   127,
     127,   127,   103,   127,   118,   127,   118,   127,   118,   127,
     118,   149,   126,   118,   126,   118,   127,   126,   118,   127,
      94,   134,    94,    99,   103,    94,   134,    94,    99,   103,
     100,   100,   117,   103,   124,    95,    92,    93,   118,   118,
     126,   119,    77,   103,   136,   115,   126,    95,   127,     6,
       6,    97,    97,    97,    97,    97,    99,    51,   127,    99,
     127,    99,   127,    99,    90,   103,    91,   102,   127,    90,
     103,    91,   102,   127,   149,   149,     6,   117,    99,    99,
     127,   104,   104,   103,   127,   102,   102,   126,   126,   126,
     126,   126,   126,    98,   126,   127,   126,    94,    99,   127,
      94,   134,    94,    99,   127,    94,   134,   102,   115,   103,
     115,   115,   115,   127,   127,   127,   127,   127,   127,    90,
     127,   127,    90,   127,    90,   103,    90,   127,    90,   103,
     125,   103,   103,   103,    99,    94,    99,    94,    99,   127,
      99,    94,    99,   127,   103,   127,    90,   127,    90,   127,
     127,    90,   127,    99,    99,    99,   127,   127,   127
};

#define grib_yyerrok		(grib_yyerrstatus = 0)
#define grib_yyclearin	(grib_yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto grib_yyacceptlab
#define YYABORT		goto grib_yyabortlab
#define YYERROR		goto grib_yyerrorlab


/* Like YYERROR except do call grib_yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto grib_yyerrlab

#define YYRECOVERING()  (!!grib_yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (grib_yychar == YYEMPTY && grib_yylen == 1)				\
    {								\
      grib_yychar = (Token);						\
      grib_yylval = (Value);						\
      grib_yytoken = YYTRANSLATE (grib_yychar);				\
      YYPOPSTACK (1);						\
      goto grib_yybackup;						\
    }								\
  else								\
    {								\
      grib_yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `grib_yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX grib_yylex (YYLEX_PARAM)
#else
# define YYLEX grib_yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (grib_yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (grib_yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      grib_yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
grib_yy_symbol_value_print (FILE *grib_yyoutput, int grib_yytype, YYSTYPE const * const grib_yyvaluep)
#else
static void
grib_yy_symbol_value_print (grib_yyoutput, grib_yytype, grib_yyvaluep)
    FILE *grib_yyoutput;
    int grib_yytype;
    YYSTYPE const * const grib_yyvaluep;
#endif
{
  if (!grib_yyvaluep)
    return;
# ifdef YYPRINT
  if (grib_yytype < YYNTOKENS)
    YYPRINT (grib_yyoutput, grib_yytoknum[grib_yytype], *grib_yyvaluep);
# else
  YYUSE (grib_yyoutput);
# endif
  switch (grib_yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
grib_yy_symbol_print (FILE *grib_yyoutput, int grib_yytype, YYSTYPE const * const grib_yyvaluep)
#else
static void
grib_yy_symbol_print (grib_yyoutput, grib_yytype, grib_yyvaluep)
    FILE *grib_yyoutput;
    int grib_yytype;
    YYSTYPE const * const grib_yyvaluep;
#endif
{
  if (grib_yytype < YYNTOKENS)
    YYFPRINTF (grib_yyoutput, "token %s (", grib_yytname[grib_yytype]);
  else
    YYFPRINTF (grib_yyoutput, "nterm %s (", grib_yytname[grib_yytype]);

  grib_yy_symbol_value_print (grib_yyoutput, grib_yytype, grib_yyvaluep);
  YYFPRINTF (grib_yyoutput, ")");
}

/*------------------------------------------------------------------.
| grib_yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
grib_yy_stack_print (grib_yytype_int16 *grib_yybottom, grib_yytype_int16 *grib_yytop)
#else
static void
grib_yy_stack_print (grib_yybottom, grib_yytop)
    grib_yytype_int16 *grib_yybottom;
    grib_yytype_int16 *grib_yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; grib_yybottom <= grib_yytop; grib_yybottom++)
    {
      int grib_yybot = *grib_yybottom;
      YYFPRINTF (stderr, " %d", grib_yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (grib_yydebug)							\
    grib_yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
grib_yy_reduce_print (YYSTYPE *grib_yyvsp, int grib_yyrule)
#else
static void
grib_yy_reduce_print (grib_yyvsp, grib_yyrule)
    YYSTYPE *grib_yyvsp;
    int grib_yyrule;
#endif
{
  int grib_yynrhs = grib_yyr2[grib_yyrule];
  int grib_yyi;
  unsigned long int grib_yylno = grib_yyrline[grib_yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     grib_yyrule - 1, grib_yylno);
  /* The symbols being reduced.  */
  for (grib_yyi = 0; grib_yyi < grib_yynrhs; grib_yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", grib_yyi + 1);
      grib_yy_symbol_print (stderr, grib_yyrhs[grib_yyprhs[grib_yyrule] + grib_yyi],
		       &(grib_yyvsp[(grib_yyi + 1) - (grib_yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (grib_yydebug)				\
    grib_yy_reduce_print (grib_yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int grib_yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef grib_yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define grib_yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
grib_yystrlen (const char *grib_yystr)
#else
static YYSIZE_T
grib_yystrlen (grib_yystr)
    const char *grib_yystr;
#endif
{
  YYSIZE_T grib_yylen;
  for (grib_yylen = 0; grib_yystr[grib_yylen]; grib_yylen++)
    continue;
  return grib_yylen;
}
#  endif
# endif

# ifndef grib_yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define grib_yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
grib_yystpcpy (char *grib_yydest, const char *grib_yysrc)
#else
static char *
grib_yystpcpy (grib_yydest, grib_yysrc)
    char *grib_yydest;
    const char *grib_yysrc;
#endif
{
  char *grib_yyd = grib_yydest;
  const char *grib_yys = grib_yysrc;

  while ((*grib_yyd++ = *grib_yys++) != '\0')
    continue;

  return grib_yyd - 1;
}
#  endif
# endif

# ifndef grib_yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for grib_yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from grib_yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
grib_yytnamerr (char *grib_yyres, const char *grib_yystr)
{
  if (*grib_yystr == '"')
    {
      YYSIZE_T grib_yyn = 0;
      char const *grib_yyp = grib_yystr;

      for (;;)
	switch (*++grib_yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++grib_yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (grib_yyres)
	      grib_yyres[grib_yyn] = *grib_yyp;
	    grib_yyn++;
	    break;

	  case '"':
	    if (grib_yyres)
	      grib_yyres[grib_yyn] = '\0';
	    return grib_yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! grib_yyres)
    return grib_yystrlen (grib_yystr);

  return grib_yystpcpy (grib_yyres, grib_yystr) - grib_yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
grib_yysyntax_error (char *grib_yyresult, int grib_yystate, int grib_yychar)
{
  int grib_yyn = grib_yypact[grib_yystate];

  if (! (YYPACT_NINF < grib_yyn && grib_yyn <= YYLAST))
    return 0;
  else
    {
      int grib_yytype = YYTRANSLATE (grib_yychar);
      YYSIZE_T grib_yysize0 = grib_yytnamerr (0, grib_yytname[grib_yytype]);
      YYSIZE_T grib_yysize = grib_yysize0;
      YYSIZE_T grib_yysize1;
      int grib_yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *grib_yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int grib_yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *grib_yyfmt;
      char const *grib_yyf;
      static char const grib_yyunexpected[] = "syntax error, unexpected %s";
      static char const grib_yyexpecting[] = ", expecting %s";
      static char const grib_yyor[] = " or %s";
      char grib_yyformat[sizeof grib_yyunexpected
		    + sizeof grib_yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof grib_yyor - 1))];
      char const *grib_yyprefix = grib_yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int grib_yyxbegin = grib_yyn < 0 ? -grib_yyn : 0;

      /* Stay within bounds of both grib_yycheck and grib_yytname.  */
      int grib_yychecklim = YYLAST - grib_yyn + 1;
      int grib_yyxend = grib_yychecklim < YYNTOKENS ? grib_yychecklim : YYNTOKENS;
      int grib_yycount = 1;

      grib_yyarg[0] = grib_yytname[grib_yytype];
      grib_yyfmt = grib_yystpcpy (grib_yyformat, grib_yyunexpected);

      for (grib_yyx = grib_yyxbegin; grib_yyx < grib_yyxend; ++grib_yyx)
	if (grib_yycheck[grib_yyx + grib_yyn] == grib_yyx && grib_yyx != YYTERROR)
	  {
	    if (grib_yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		grib_yycount = 1;
		grib_yysize = grib_yysize0;
		grib_yyformat[sizeof grib_yyunexpected - 1] = '\0';
		break;
	      }
	    grib_yyarg[grib_yycount++] = grib_yytname[grib_yyx];
	    grib_yysize1 = grib_yysize + grib_yytnamerr (0, grib_yytname[grib_yyx]);
	    grib_yysize_overflow |= (grib_yysize1 < grib_yysize);
	    grib_yysize = grib_yysize1;
	    grib_yyfmt = grib_yystpcpy (grib_yyfmt, grib_yyprefix);
	    grib_yyprefix = grib_yyor;
	  }

      grib_yyf = YY_(grib_yyformat);
      grib_yysize1 = grib_yysize + grib_yystrlen (grib_yyf);
      grib_yysize_overflow |= (grib_yysize1 < grib_yysize);
      grib_yysize = grib_yysize1;

      if (grib_yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (grib_yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *grib_yyp = grib_yyresult;
	  int grib_yyi = 0;
	  while ((*grib_yyp = *grib_yyf) != '\0')
	    {
	      if (*grib_yyp == '%' && grib_yyf[1] == 's' && grib_yyi < grib_yycount)
		{
		  grib_yyp += grib_yytnamerr (grib_yyp, grib_yyarg[grib_yyi++]);
		  grib_yyf += 2;
		}
	      else
		{
		  grib_yyp++;
		  grib_yyf++;
		}
	    }
	}
      return grib_yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
grib_yydestruct (const char *grib_yymsg, int grib_yytype, YYSTYPE *grib_yyvaluep)
#else
static void
grib_yydestruct (grib_yymsg, grib_yytype, grib_yyvaluep)
    const char *grib_yymsg;
    int grib_yytype;
    YYSTYPE *grib_yyvaluep;
#endif
{
  YYUSE (grib_yyvaluep);

  if (!grib_yymsg)
    grib_yymsg = "Deleting";
  YY_SYMBOL_PRINT (grib_yymsg, grib_yytype, grib_yyvaluep, grib_yylocationp);

  switch (grib_yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int grib_yyparse (void *YYPARSE_PARAM);
#else
int grib_yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int grib_yyparse (void);
#else
int grib_yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int grib_yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE grib_yylval;

/* Number of syntax errors so far.  */
int grib_yynerrs;



/*-------------------------.
| grib_yyparse or grib_yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
grib_yyparse (void *YYPARSE_PARAM)
#else
int
grib_yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
grib_yyparse (void)
#else
int
grib_yyparse ()

#endif
#endif
{


    int grib_yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int grib_yyerrstatus;

    /* The stacks and their tools:
       `grib_yyss': related to states.
       `grib_yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow grib_yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    grib_yytype_int16 grib_yyssa[YYINITDEPTH];
    grib_yytype_int16 *grib_yyss;
    grib_yytype_int16 *grib_yyssp;

    /* The semantic value stack.  */
    YYSTYPE grib_yyvsa[YYINITDEPTH];
    YYSTYPE *grib_yyvs;
    YYSTYPE *grib_yyvsp;

    YYSIZE_T grib_yystacksize;

  int grib_yyn;
  int grib_yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int grib_yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE grib_yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char grib_yymsgbuf[128];
  char *grib_yymsg = grib_yymsgbuf;
  YYSIZE_T grib_yymsg_alloc = sizeof grib_yymsgbuf;
#endif

#define YYPOPSTACK(N)   (grib_yyvsp -= (N), grib_yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int grib_yylen = 0;

  grib_yytoken = 0;
  grib_yyss = grib_yyssa;
  grib_yyvs = grib_yyvsa;
  grib_yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  grib_yystate = 0;
  grib_yyerrstatus = 0;
  grib_yynerrs = 0;
  grib_yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  grib_yyssp = grib_yyss;
  grib_yyvsp = grib_yyvs;

  goto grib_yysetstate;

/*------------------------------------------------------------.
| grib_yynewstate -- Push a new state, which is found in grib_yystate.  |
`------------------------------------------------------------*/
 grib_yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  grib_yyssp++;

 grib_yysetstate:
  *grib_yyssp = grib_yystate;

  if (grib_yyss + grib_yystacksize - 1 <= grib_yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T grib_yysize = grib_yyssp - grib_yyss + 1;

#ifdef grib_yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *grib_yyvs1 = grib_yyvs;
	grib_yytype_int16 *grib_yyss1 = grib_yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if grib_yyoverflow is a macro.  */
	grib_yyoverflow (YY_("memory exhausted"),
		    &grib_yyss1, grib_yysize * sizeof (*grib_yyssp),
		    &grib_yyvs1, grib_yysize * sizeof (*grib_yyvsp),
		    &grib_yystacksize);

	grib_yyss = grib_yyss1;
	grib_yyvs = grib_yyvs1;
      }
#else /* no grib_yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto grib_yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= grib_yystacksize)
	goto grib_yyexhaustedlab;
      grib_yystacksize *= 2;
      if (YYMAXDEPTH < grib_yystacksize)
	grib_yystacksize = YYMAXDEPTH;

      {
	grib_yytype_int16 *grib_yyss1 = grib_yyss;
	union grib_yyalloc *grib_yyptr =
	  (union grib_yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (grib_yystacksize));
	if (! grib_yyptr)
	  goto grib_yyexhaustedlab;
	YYSTACK_RELOCATE (grib_yyss_alloc, grib_yyss);
	YYSTACK_RELOCATE (grib_yyvs_alloc, grib_yyvs);
#  undef YYSTACK_RELOCATE
	if (grib_yyss1 != grib_yyssa)
	  YYSTACK_FREE (grib_yyss1);
      }
# endif
#endif /* no grib_yyoverflow */

      grib_yyssp = grib_yyss + grib_yysize - 1;
      grib_yyvsp = grib_yyvs + grib_yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) grib_yystacksize));

      if (grib_yyss + grib_yystacksize - 1 <= grib_yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", grib_yystate));

  if (grib_yystate == YYFINAL)
    YYACCEPT;

  goto grib_yybackup;

/*-----------.
| grib_yybackup.  |
`-----------*/
grib_yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  grib_yyn = grib_yypact[grib_yystate];
  if (grib_yyn == YYPACT_NINF)
    goto grib_yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (grib_yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      grib_yychar = YYLEX;
    }

  if (grib_yychar <= YYEOF)
    {
      grib_yychar = grib_yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      grib_yytoken = YYTRANSLATE (grib_yychar);
      YY_SYMBOL_PRINT ("Next token is", grib_yytoken, &grib_yylval, &grib_yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  grib_yyn += grib_yytoken;
  if (grib_yyn < 0 || YYLAST < grib_yyn || grib_yycheck[grib_yyn] != grib_yytoken)
    goto grib_yydefault;
  grib_yyn = grib_yytable[grib_yyn];
  if (grib_yyn <= 0)
    {
      if (grib_yyn == 0 || grib_yyn == YYTABLE_NINF)
	goto grib_yyerrlab;
      grib_yyn = -grib_yyn;
      goto grib_yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (grib_yyerrstatus)
    grib_yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", grib_yytoken, &grib_yylval, &grib_yylloc);

  /* Discard the shifted token.  */
  grib_yychar = YYEMPTY;

  grib_yystate = grib_yyn;
  *++grib_yyvsp = grib_yylval;

  goto grib_yynewstate;


/*-----------------------------------------------------------.
| grib_yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
grib_yydefault:
  grib_yyn = grib_yydefact[grib_yystate];
  if (grib_yyn == 0)
    goto grib_yyerrlab;
  goto grib_yyreduce;


/*-----------------------------.
| grib_yyreduce -- Do a reduction.  |
`-----------------------------*/
grib_yyreduce:
  /* grib_yyn is the number of a rule to reduce with.  */
  grib_yylen = grib_yyr2[grib_yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  grib_yyval = grib_yyvsp[1-grib_yylen];


  YY_REDUCE_PRINT (grib_yyn);
  switch (grib_yyn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 210 "griby.y"
    { grib_parser_all_actions = 0;grib_parser_concept=0; grib_parser_rules=0; }
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 211 "griby.y"
    { grib_parser_concept     = reverse((grib_yyvsp[(1) - (1)].concept_value)); }
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 212 "griby.y"
    { grib_parser_all_actions = (grib_yyvsp[(1) - (1)].act); }
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 213 "griby.y"
    { grib_parser_rules       = (grib_yyvsp[(1) - (1)].rules); }
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 215 "griby.y"
    { grib_parser_all_actions = 0; grib_parser_concept=0; grib_parser_rules=0; }
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 221 "griby.y"
    { (grib_yyval.dvalue)=grib_darray_push(0,0,(grib_yyvsp[(1) - (1)].dval));}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 222 "griby.y"
    { (grib_yyval.dvalue)=grib_darray_push(0,(grib_yyvsp[(1) - (3)].dvalue),(grib_yyvsp[(3) - (3)].dval));}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 223 "griby.y"
    { (grib_yyval.dvalue)=grib_darray_push(0,0,(grib_yyvsp[(1) - (1)].lval));}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 224 "griby.y"
    { (grib_yyval.dvalue)=grib_darray_push(0,(grib_yyvsp[(1) - (3)].dvalue),(grib_yyvsp[(3) - (3)].lval));}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 227 "griby.y"
    { (grib_yyvsp[(1) - (2)].act)->next = (grib_yyvsp[(2) - (2)].act); (grib_yyval.act) = (grib_yyvsp[(1) - (2)].act); }
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 228 "griby.y"
    { (grib_yyvsp[(1) - (3)].act)->next = (grib_yyvsp[(3) - (3)].act); (grib_yyval.act) = (grib_yyvsp[(1) - (3)].act); }
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 229 "griby.y"
    {  (grib_yyval.act) = (grib_yyvsp[(1) - (2)].act);}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 247 "griby.y"
    { (grib_yyval.explist) = 0; }
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 252 "griby.y"
    { (grib_yyvsp[(1) - (3)].explist)->next = (grib_yyvsp[(3) - (3)].explist); (grib_yyval.explist) = (grib_yyvsp[(1) - (3)].explist); }
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 255 "griby.y"
    { (grib_yyval.explist) = grib_arguments_new(grib_parser_context,(grib_yyvsp[(1) - (1)].exp),NULL); }
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 260 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (7)].str),"unsigned",(grib_yyvsp[(3) - (7)].lval),NULL,(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL,NULL);        free((grib_yyvsp[(5) - (7)].str));  }
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 263 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (10)].str),"unsigned",(grib_yyvsp[(3) - (10)].lval),(grib_yyvsp[(7) - (10)].explist),(grib_yyvsp[(9) - (10)].explist),(grib_yyvsp[(10) - (10)].lval),NULL,NULL);        free((grib_yyvsp[(5) - (10)].str));  }
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 266 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (7)].str),"unsigned_bits",(grib_yyvsp[(3) - (7)].lval),NULL,(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL,NULL);        free((grib_yyvsp[(5) - (7)].str));  }
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 269 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (10)].str),"unsigned_bits",(grib_yyvsp[(3) - (10)].lval),(grib_yyvsp[(7) - (10)].explist),(grib_yyvsp[(9) - (10)].explist),(grib_yyvsp[(10) - (10)].lval),NULL,NULL);        free((grib_yyvsp[(5) - (10)].str));  }
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 272 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (7)].str),"ascii",(grib_yyvsp[(3) - (7)].lval),NULL,(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL,NULL);  free((grib_yyvsp[(5) - (7)].str));  }
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 276 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (7)].str),"ascii",(grib_yyvsp[(3) - (7)].lval),NULL,(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL,NULL);  free((grib_yyvsp[(5) - (7)].str));  }
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 279 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (7)].str),"bytes",(grib_yyvsp[(3) - (7)].lval),NULL,(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL,NULL);      free((grib_yyvsp[(5) - (7)].str));  }
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 282 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (10)].str),"bytes",(grib_yyvsp[(3) - (10)].lval),(grib_yyvsp[(7) - (10)].explist),(grib_yyvsp[(9) - (10)].explist),(grib_yyvsp[(10) - (10)].lval),NULL,NULL);      free((grib_yyvsp[(5) - (10)].str));  }
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 285 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (7)].str),"ksec1expver",(grib_yyvsp[(3) - (7)].lval),NULL,(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL,NULL);  free((grib_yyvsp[(5) - (7)].str));  }
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 288 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (7)].str),"signed",(grib_yyvsp[(3) - (7)].lval),NULL,(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL,NULL);      free((grib_yyvsp[(5) - (7)].str));  }
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 291 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (10)].str),"signed",(grib_yyvsp[(3) - (10)].lval),(grib_yyvsp[(7) - (10)].explist),(grib_yyvsp[(9) - (10)].explist),(grib_yyvsp[(10) - (10)].lval),NULL,NULL);      free((grib_yyvsp[(5) - (10)].str));  }
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 294 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (7)].str),"signed_bits",(grib_yyvsp[(3) - (7)].lval),NULL,(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL,NULL);      free((grib_yyvsp[(5) - (7)].str));  }
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 297 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (10)].str),"signed_bits",(grib_yyvsp[(3) - (10)].lval),(grib_yyvsp[(7) - (10)].explist),(grib_yyvsp[(9) - (10)].explist),(grib_yyvsp[(10) - (10)].lval),NULL,NULL);      free((grib_yyvsp[(5) - (10)].str));  }
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 300 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (8)].str),"codetable",(grib_yyvsp[(3) - (8)].lval), (grib_yyvsp[(6) - (8)].explist),(grib_yyvsp[(7) - (8)].explist),(grib_yyvsp[(8) - (8)].lval),NULL,NULL);    free((grib_yyvsp[(5) - (8)].str)); }
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 303 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (12)].str),"codetable",(grib_yyvsp[(3) - (12)].lval), (grib_yyvsp[(6) - (12)].explist),(grib_yyvsp[(7) - (12)].explist),(grib_yyvsp[(12) - (12)].lval),NULL,(grib_yyvsp[(10) - (12)].str));
					free((grib_yyvsp[(5) - (12)].str));free((grib_yyvsp[(10) - (12)].str)); }
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 307 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (10)].str),"codetable",(grib_yyvsp[(3) - (10)].lval), (grib_yyvsp[(7) - (10)].explist),(grib_yyvsp[(9) - (10)].explist),(grib_yyvsp[(10) - (10)].lval),NULL,NULL);    free((grib_yyvsp[(5) - (10)].str)); }
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 310 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(1) - (8)].str),"getenv",0,(grib_yyvsp[(5) - (8)].explist),(grib_yyvsp[(7) - (8)].explist),(grib_yyvsp[(8) - (8)].lval),NULL,NULL);    free((grib_yyvsp[(1) - (8)].str)); }
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 313 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (8)].str),"complex_codetable",(grib_yyvsp[(3) - (8)].lval), (grib_yyvsp[(6) - (8)].explist),(grib_yyvsp[(7) - (8)].explist),(grib_yyvsp[(8) - (8)].lval),NULL,NULL);    free((grib_yyvsp[(5) - (8)].str)); }
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 316 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (10)].str),"complex_codetable",(grib_yyvsp[(3) - (10)].lval), (grib_yyvsp[(7) - (10)].explist),(grib_yyvsp[(9) - (10)].explist),(grib_yyvsp[(10) - (10)].lval),NULL,NULL);    free((grib_yyvsp[(5) - (10)].str)); }
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 319 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (8)].str),"codeflag",(grib_yyvsp[(3) - (8)].lval), (grib_yyvsp[(6) - (8)].explist),(grib_yyvsp[(7) - (8)].explist),(grib_yyvsp[(8) - (8)].lval),NULL,NULL);  free((grib_yyvsp[(5) - (8)].str)); }
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 322 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (9)].str),"lookup",(grib_yyvsp[(3) - (9)].lval),(grib_yyvsp[(7) - (9)].explist),NULL,(grib_yyvsp[(9) - (9)].lval),NULL,NULL); free((grib_yyvsp[(5) - (9)].str)); }
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 325 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (7)].str),"bit",0,(grib_yyvsp[(4) - (7)].explist),(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL,NULL); free((grib_yyvsp[(2) - (7)].str)); }
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 328 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (2)].str),"label",0,NULL,NULL,0,NULL,NULL);   free((grib_yyvsp[(2) - (2)].str));  }
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 331 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (2)].str),"label",0,NULL,NULL,0,NULL,NULL);   free((grib_yyvsp[(2) - (2)].str));  }
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 334 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (4)].str),"ibmfloat",4,NULL,(grib_yyvsp[(3) - (4)].explist),(grib_yyvsp[(4) - (4)].lval),NULL,NULL);free((grib_yyvsp[(2) - (4)].str));  }
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 337 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(4) - (6)].str),"ibmfloat",4,NULL,(grib_yyvsp[(5) - (6)].explist),(grib_yyvsp[(6) - (6)].lval),(grib_yyvsp[(2) - (6)].str),NULL);free((grib_yyvsp[(4) - (6)].str)); free((grib_yyvsp[(2) - (6)].str)); }
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 340 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (7)].str),"ibmfloat",4,(grib_yyvsp[(4) - (7)].explist),(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL,NULL);free((grib_yyvsp[(2) - (7)].str));  }
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 343 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (2)].str),"position",0,NULL,NULL,0,NULL,NULL);     free((grib_yyvsp[(2) - (2)].str));  }
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 346 "griby.y"
    { (grib_yyval.act) = grib_action_create_variable(grib_parser_context,(grib_yyvsp[(2) - (5)].str),"constant",0,(grib_yyvsp[(4) - (5)].explist),NULL,(grib_yyvsp[(5) - (5)].lval),NULL);free((grib_yyvsp[(2) - (5)].str)); }
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 349 "griby.y"
    { (grib_yyval.act) = grib_action_create_variable(grib_parser_context,(grib_yyvsp[(2) - (5)].str),"transient",0,(grib_yyvsp[(4) - (5)].explist),(grib_yyvsp[(4) - (5)].explist),(grib_yyvsp[(5) - (5)].lval),NULL);   free((grib_yyvsp[(2) - (5)].str)); }
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 352 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (4)].str),"ieeefloat",4,NULL,(grib_yyvsp[(3) - (4)].explist),(grib_yyvsp[(4) - (4)].lval),NULL,NULL);   free((grib_yyvsp[(2) - (4)].str));  }
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 355 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(4) - (6)].str),"ieeefloat",4,NULL,(grib_yyvsp[(5) - (6)].explist),(grib_yyvsp[(6) - (6)].lval),(grib_yyvsp[(2) - (6)].str),NULL);  free((grib_yyvsp[(4) - (6)].str));free((grib_yyvsp[(2) - (6)].str));}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 358 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (7)].str),"ieeefloat",4,(grib_yyvsp[(4) - (7)].explist),(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL,NULL);free((grib_yyvsp[(2) - (7)].str));  }
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 361 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (2)].str),"g1_half_byte_codeflag",0,NULL,NULL,0,NULL,NULL);free((grib_yyvsp[(2) - (2)].str));  }
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 364 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (5)].str),"section_length",(grib_yyvsp[(3) - (5)].lval),NULL,NULL,0,NULL,NULL);free((grib_yyvsp[(5) - (5)].str));  }
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 367 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (8)].str),"g1_message_length",(grib_yyvsp[(3) - (8)].lval),(grib_yyvsp[(7) - (8)].explist),NULL,0,NULL,NULL);free((grib_yyvsp[(5) - (8)].str));  }
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 370 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (8)].str),"g1_section4_length",(grib_yyvsp[(3) - (8)].lval),(grib_yyvsp[(7) - (8)].explist),NULL,0,NULL,NULL);free((grib_yyvsp[(5) - (8)].str));  }
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 373 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (3)].str),"ksec",0,(grib_yyvsp[(3) - (3)].explist),NULL,0,NULL,NULL);free((grib_yyvsp[(2) - (3)].str)); }
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 376 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (5)].str),"pad",0,(grib_yyvsp[(4) - (5)].explist),0,0,NULL,NULL);   free((grib_yyvsp[(2) - (5)].str)); }
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 379 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (5)].str),"padto",0,(grib_yyvsp[(4) - (5)].explist),0,0,NULL,NULL);   free((grib_yyvsp[(2) - (5)].str)); }
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 382 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (5)].str),"padtoeven",0,(grib_yyvsp[(4) - (5)].explist),0,0,NULL,NULL);   free((grib_yyvsp[(2) - (5)].str)); }
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 385 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (5)].str),"padtomultiple",0,(grib_yyvsp[(4) - (5)].explist),0,0,NULL,NULL);   free((grib_yyvsp[(2) - (5)].str)); }
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 388 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (6)].str),"message",(grib_yyvsp[(3) - (6)].lval),0,0,(grib_yyvsp[(6) - (6)].lval),NULL,NULL);   free((grib_yyvsp[(5) - (6)].str));  }
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 391 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (3)].str),"section_padding",0,0,0,(grib_yyvsp[(3) - (3)].lval),NULL,NULL);   free((grib_yyvsp[(2) - (3)].str));  }
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 393 "griby.y"
    { (grib_yyval.act) = grib_action_create_template(grib_parser_context,0,(grib_yyvsp[(2) - (3)].str),(grib_yyvsp[(3) - (3)].str)); free((grib_yyvsp[(2) - (3)].str)); free((grib_yyvsp[(3) - (3)].str));}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 395 "griby.y"
    { (grib_yyval.act) = grib_action_create_template(grib_parser_context,1,(grib_yyvsp[(2) - (3)].str),(grib_yyvsp[(3) - (3)].str)); free((grib_yyvsp[(2) - (3)].str)); free((grib_yyvsp[(3) - (3)].str));}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 398 "griby.y"
    { (grib_yyval.act) = grib_action_create_alias(grib_parser_context,(grib_yyvsp[(2) - (5)].str),(grib_yyvsp[(4) - (5)].str),NULL,(grib_yyvsp[(5) - (5)].lval));  free((grib_yyvsp[(2) - (5)].str)); free((grib_yyvsp[(4) - (5)].str)); }
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 401 "griby.y"
    { (grib_yyval.act) = grib_action_create_alias(grib_parser_context,(grib_yyvsp[(2) - (2)].str),NULL,NULL,0);  free((grib_yyvsp[(2) - (2)].str)); }
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 404 "griby.y"
    {
         (grib_yyval.act) = grib_action_create_alias(grib_parser_context,(grib_yyvsp[(4) - (7)].str),(grib_yyvsp[(6) - (7)].str),(grib_yyvsp[(2) - (7)].str),(grib_yyvsp[(7) - (7)].lval));  free((grib_yyvsp[(2) - (7)].str)); free((grib_yyvsp[(4) - (7)].str)); free((grib_yyvsp[(6) - (7)].str));
    }
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 408 "griby.y"
    {
         (grib_yyval.act) = grib_action_create_alias(grib_parser_context,(grib_yyvsp[(4) - (4)].str),NULL,(grib_yyvsp[(2) - (4)].str),0);  free((grib_yyvsp[(2) - (4)].str)); free((grib_yyvsp[(4) - (4)].str)); 
    }
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 412 "griby.y"
    { (grib_yyval.act) = grib_action_create_meta(grib_parser_context,(grib_yyvsp[(2) - (8)].str),(grib_yyvsp[(3) - (8)].str),(grib_yyvsp[(5) - (8)].explist),(grib_yyvsp[(7) - (8)].explist),(grib_yyvsp[(8) - (8)].lval),NULL); free((grib_yyvsp[(2) - (8)].str));free((grib_yyvsp[(3) - (8)].str));}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 415 "griby.y"
    { (grib_yyval.act) = grib_action_create_meta(grib_parser_context,(grib_yyvsp[(4) - (10)].str),(grib_yyvsp[(5) - (10)].str),(grib_yyvsp[(7) - (10)].explist),(grib_yyvsp[(9) - (10)].explist),(grib_yyvsp[(10) - (10)].lval),(grib_yyvsp[(2) - (10)].str)); free((grib_yyvsp[(4) - (10)].str));free((grib_yyvsp[(5) - (10)].str));free((grib_yyvsp[(2) - (10)].str));}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 418 "griby.y"
    {
      grib_arguments* a = grib_arguments_new(
        grib_parser_context,
        new_accessor_expression(grib_parser_context,(grib_yyvsp[(2) - (5)].str)),
		NULL
        );
      a->next=(grib_yyvsp[(4) - (5)].explist);
      (grib_yyval.act) = grib_action_create_meta(grib_parser_context,
      "ITERATOR","iterator",a,NULL,
      GRIB_ACCESSOR_FLAG_HIDDEN|GRIB_ACCESSOR_FLAG_READ_ONLY,NULL); free((grib_yyvsp[(2) - (5)].str));
    }
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 430 "griby.y"
    {
      grib_arguments* a = grib_arguments_new(
        grib_parser_context,
        new_accessor_expression(grib_parser_context,(grib_yyvsp[(2) - (5)].str)),
		NULL
        );
      a->next=(grib_yyvsp[(4) - (5)].explist);
      (grib_yyval.act) = grib_action_create_meta(grib_parser_context,
      "NEAREST","nearest",a,NULL,
      GRIB_ACCESSOR_FLAG_HIDDEN|GRIB_ACCESSOR_FLAG_READ_ONLY,NULL); free((grib_yyvsp[(2) - (5)].str));
    }
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 442 "griby.y"
    {
      grib_arguments* a = grib_arguments_new(
        grib_parser_context,
        new_accessor_expression(grib_parser_context,(grib_yyvsp[(2) - (5)].str)),
		NULL
        );
      a->next=(grib_yyvsp[(4) - (5)].explist);
      (grib_yyval.act) = grib_action_create_meta(grib_parser_context,
      "BOX","box",a,NULL,
      GRIB_ACCESSOR_FLAG_HIDDEN|GRIB_ACCESSOR_FLAG_READ_ONLY,NULL); free((grib_yyvsp[(2) - (5)].str));
    }
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 454 "griby.y"
    { (grib_yyval.act) = grib_action_create_put(grib_parser_context,(grib_yyvsp[(2) - (5)].str),(grib_yyvsp[(4) - (5)].explist));free((grib_yyvsp[(2) - (5)].str));}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 457 "griby.y"
    { (grib_yyval.act) = grib_action_create_remove(grib_parser_context,(grib_yyvsp[(2) - (2)].explist));}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 460 "griby.y"
    { (grib_yyval.act) = grib_action_create_assert(grib_parser_context,(grib_yyvsp[(3) - (4)].exp));}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 463 "griby.y"
    { (grib_yyval.act) = grib_action_create_modify(grib_parser_context,(grib_yyvsp[(2) - (3)].str),(grib_yyvsp[(3) - (3)].lval)); free((grib_yyvsp[(2) - (3)].str));}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 465 "griby.y"
    { (grib_yyval.act) = grib_action_create_set_missing(grib_parser_context,(grib_yyvsp[(2) - (4)].str)); free((grib_yyvsp[(2) - (4)].str)); }
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 466 "griby.y"
    { (grib_yyval.act) = grib_action_create_set(grib_parser_context,(grib_yyvsp[(2) - (4)].str),(grib_yyvsp[(4) - (4)].exp),0); free((grib_yyvsp[(2) - (4)].str)); }
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 467 "griby.y"
    { (grib_yyval.act) = grib_action_create_set_darray(grib_parser_context,(grib_yyvsp[(2) - (6)].str),(grib_yyvsp[(5) - (6)].dvalue)); free((grib_yyvsp[(2) - (6)].str)); }
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 469 "griby.y"
    { (grib_yyval.act) = grib_action_create_set(grib_parser_context,(grib_yyvsp[(2) - (4)].str),(grib_yyvsp[(4) - (4)].exp),1); free((grib_yyvsp[(2) - (4)].str)); }
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 472 "griby.y"
    { (grib_yyval.act) = grib_action_create_write(grib_parser_context,(grib_yyvsp[(2) - (2)].str),0,0); free((grib_yyvsp[(2) - (2)].str));}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 473 "griby.y"
    { (grib_yyval.act) = grib_action_create_write(grib_parser_context,"",0,0); }
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 474 "griby.y"
    { (grib_yyval.act) = grib_action_create_write(grib_parser_context,(grib_yyvsp[(5) - (5)].str),0,(grib_yyvsp[(3) - (5)].lval)); free((grib_yyvsp[(5) - (5)].str));}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 475 "griby.y"
    { (grib_yyval.act) = grib_action_create_write(grib_parser_context,"",0,(grib_yyvsp[(3) - (4)].lval)); }
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 476 "griby.y"
    { (grib_yyval.act) = grib_action_create_write(grib_parser_context,(grib_yyvsp[(2) - (2)].str),1,0); free((grib_yyvsp[(2) - (2)].str));}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 477 "griby.y"
    { (grib_yyval.act) = grib_action_create_write(grib_parser_context,"",1,0); }
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 478 "griby.y"
    { (grib_yyval.act) = grib_action_create_write(grib_parser_context,(grib_yyvsp[(5) - (5)].str),1,(grib_yyvsp[(3) - (5)].lval)); free((grib_yyvsp[(5) - (5)].str));}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 479 "griby.y"
    { (grib_yyval.act) = grib_action_create_write(grib_parser_context,"",1,(grib_yyvsp[(3) - (4)].lval)); }
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 481 "griby.y"
    { (grib_yyval.act) = grib_action_create_print(grib_parser_context,(grib_yyvsp[(2) - (2)].str),0); free((grib_yyvsp[(2) - (2)].str)); }
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 482 "griby.y"
    { (grib_yyval.act) = grib_action_create_print(grib_parser_context,(grib_yyvsp[(5) - (5)].str),(grib_yyvsp[(3) - (5)].str)); free((grib_yyvsp[(5) - (5)].str)); free((grib_yyvsp[(3) - (5)].str));}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 483 "griby.y"
    { (grib_yyval.act) = grib_action_create_print(grib_parser_context,"",0);  }
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 487 "griby.y"
    { (grib_yyval.act) = grib_action_create_if(grib_parser_context,(grib_yyvsp[(3) - (7)].exp),(grib_yyvsp[(6) - (7)].act),0,0); }
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 488 "griby.y"
    { (grib_yyval.act) = grib_action_create_if(grib_parser_context,(grib_yyvsp[(3) - (11)].exp),(grib_yyvsp[(6) - (11)].act),(grib_yyvsp[(10) - (11)].act),0); }
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 489 "griby.y"
    { (grib_yyval.act) = grib_action_create_if(grib_parser_context,(grib_yyvsp[(3) - (7)].exp),(grib_yyvsp[(6) - (7)].act),0,1); }
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 490 "griby.y"
    { (grib_yyval.act) = grib_action_create_if(grib_parser_context,(grib_yyvsp[(3) - (11)].exp),(grib_yyvsp[(6) - (11)].act),(grib_yyvsp[(10) - (11)].act),1); }
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 494 "griby.y"
    { (grib_yyval.act) = grib_action_create_when(grib_parser_context,(grib_yyvsp[(3) - (6)].exp),(grib_yyvsp[(5) - (6)].act),NULL); }
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 495 "griby.y"
    { (grib_yyval.act) = grib_action_create_when(grib_parser_context,(grib_yyvsp[(3) - (7)].exp),(grib_yyvsp[(6) - (7)].act),NULL); }
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 496 "griby.y"
    { (grib_yyval.act) = grib_action_create_when(grib_parser_context,(grib_yyvsp[(3) - (11)].exp),(grib_yyvsp[(6) - (11)].act),(grib_yyvsp[(10) - (11)].act)); }
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 499 "griby.y"
    { (grib_yyval.act) = grib_action_create_set(grib_parser_context,(grib_yyvsp[(2) - (4)].str),(grib_yyvsp[(4) - (4)].exp),0); free((grib_yyvsp[(2) - (4)].str)); }
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 500 "griby.y"
    { (grib_yyval.act) = grib_action_create_set(grib_parser_context,(grib_yyvsp[(2) - (4)].str),(grib_yyvsp[(4) - (4)].exp),1); free((grib_yyvsp[(2) - (4)].str)); }
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 504 "griby.y"
    { (grib_yyvsp[(1) - (3)].act)->next = (grib_yyvsp[(2) - (3)].act); (grib_yyval.act) = (grib_yyvsp[(1) - (3)].act); }
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 508 "griby.y"
    { (grib_yyval.explist) = NULL ;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 509 "griby.y"
    { (grib_yyval.explist) = (grib_yyvsp[(2) - (2)].explist) ;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 512 "griby.y"
    { (grib_yyval.lval) = 0 ; }
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 513 "griby.y"
    { (grib_yyval.lval) = (grib_yyvsp[(2) - (2)].lval); }
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 517 "griby.y"
    { (grib_yyval.lval) = (grib_yyvsp[(1) - (3)].lval) | (grib_yyvsp[(3) - (3)].lval); }
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 520 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_READ_ONLY; }
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 521 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_LOWERCASE; }
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 522 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_DUMP; }
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 523 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_NO_COPY; }
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 524 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_NO_FAIL; }
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 525 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_HIDDEN; }
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 526 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_EDITION_SPECIFIC; }
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 527 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_CAN_BE_MISSING; }
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 528 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_CONSTRAINT; }
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 529 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_OVERRIDE; }
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 530 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_COPY_OK; }
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 531 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_TRANSIENT; }
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 532 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_STRING_TYPE; }
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 533 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_LONG_TYPE; }
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 534 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_DOUBLE_TYPE; }
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 537 "griby.y"
    { (grib_yyval.act) = grib_action_create_list(grib_parser_context,(grib_yyvsp[(1) - (8)].str),(grib_yyvsp[(4) - (8)].exp),(grib_yyvsp[(7) - (8)].act)); free((grib_yyvsp[(1) - (8)].str)); }
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 540 "griby.y"
    { (grib_yyval.act) = grib_action_create_while(grib_parser_context,(grib_yyvsp[(3) - (7)].exp),(grib_yyvsp[(6) - (7)].act));  }
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 543 "griby.y"
    { (grib_yyval.act) = grib_action_create_trigger(grib_parser_context,(grib_yyvsp[(3) - (7)].explist),(grib_yyvsp[(6) - (7)].act));  }
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 546 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(2) - (6)].str),(grib_yyvsp[(4) - (6)].concept_value),0,0,0,0,0,0,(grib_yyvsp[(6) - (6)].lval),0);  free((grib_yyvsp[(2) - (6)].str)); }
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 547 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(2) - (9)].str),(grib_yyvsp[(7) - (9)].concept_value),0,0,(grib_yyvsp[(4) - (9)].str),0,0,0,(grib_yyvsp[(9) - (9)].lval),0);  free((grib_yyvsp[(2) - (9)].str));free((grib_yyvsp[(4) - (9)].str)); }
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 548 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(2) - (12)].str),0,(grib_yyvsp[(6) - (12)].str),0,(grib_yyvsp[(4) - (12)].str),(grib_yyvsp[(8) - (12)].str),(grib_yyvsp[(10) - (12)].str),0,(grib_yyvsp[(12) - (12)].lval),0);  free((grib_yyvsp[(2) - (12)].str));free((grib_yyvsp[(6) - (12)].str));free((grib_yyvsp[(4) - (12)].str));free((grib_yyvsp[(8) - (12)].str));free((grib_yyvsp[(10) - (12)].str)); }
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 549 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(2) - (14)].str),0,(grib_yyvsp[(6) - (14)].str),0,(grib_yyvsp[(4) - (14)].str),(grib_yyvsp[(8) - (14)].str),(grib_yyvsp[(10) - (14)].str),(grib_yyvsp[(12) - (14)].str),(grib_yyvsp[(14) - (14)].lval),0);  free((grib_yyvsp[(2) - (14)].str));free((grib_yyvsp[(6) - (14)].str));free((grib_yyvsp[(4) - (14)].str));free((grib_yyvsp[(8) - (14)].str));free((grib_yyvsp[(10) - (14)].str));free((grib_yyvsp[(12) - (14)].str)); }
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 550 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(2) - (10)].str),0,(grib_yyvsp[(6) - (10)].str),0,(grib_yyvsp[(4) - (10)].str),(grib_yyvsp[(8) - (10)].str),0,0,(grib_yyvsp[(10) - (10)].lval),0);  free((grib_yyvsp[(2) - (10)].str));free((grib_yyvsp[(6) - (10)].str));free((grib_yyvsp[(4) - (10)].str));free((grib_yyvsp[(8) - (10)].str)); }
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 551 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(4) - (14)].str),0,(grib_yyvsp[(8) - (14)].str),(grib_yyvsp[(2) - (14)].str),(grib_yyvsp[(6) - (14)].str),(grib_yyvsp[(10) - (14)].str),(grib_yyvsp[(12) - (14)].str),0,(grib_yyvsp[(14) - (14)].lval),0);  free((grib_yyvsp[(4) - (14)].str));free((grib_yyvsp[(8) - (14)].str));free((grib_yyvsp[(6) - (14)].str));free((grib_yyvsp[(10) - (14)].str)); free((grib_yyvsp[(2) - (14)].str));}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 552 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(4) - (12)].str),0,(grib_yyvsp[(8) - (12)].str),(grib_yyvsp[(2) - (12)].str),(grib_yyvsp[(6) - (12)].str),(grib_yyvsp[(10) - (12)].str),0,0,(grib_yyvsp[(12) - (12)].lval),0);  free((grib_yyvsp[(4) - (12)].str));free((grib_yyvsp[(8) - (12)].str));free((grib_yyvsp[(6) - (12)].str));free((grib_yyvsp[(10) - (12)].str)); free((grib_yyvsp[(2) - (12)].str));}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 553 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(4) - (8)].str),(grib_yyvsp[(6) - (8)].concept_value),0,(grib_yyvsp[(2) - (8)].str),0,0,0,0,(grib_yyvsp[(8) - (8)].lval),0);  free((grib_yyvsp[(2) - (8)].str));free((grib_yyvsp[(4) - (8)].str)); }
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 554 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(4) - (11)].str),(grib_yyvsp[(9) - (11)].concept_value),0,(grib_yyvsp[(2) - (11)].str),(grib_yyvsp[(6) - (11)].str),0,0,0,(grib_yyvsp[(11) - (11)].lval),0);  free((grib_yyvsp[(2) - (11)].str));free((grib_yyvsp[(4) - (11)].str));free((grib_yyvsp[(6) - (11)].str)); }
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 555 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(2) - (6)].str),(grib_yyvsp[(4) - (6)].concept_value),0,0,0,0,0,0,(grib_yyvsp[(6) - (6)].lval),1);  free((grib_yyvsp[(2) - (6)].str)); }
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 556 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(2) - (9)].str),(grib_yyvsp[(7) - (9)].concept_value),0,0,(grib_yyvsp[(4) - (9)].str),0,0,0,(grib_yyvsp[(9) - (9)].lval),1);  free((grib_yyvsp[(2) - (9)].str));free((grib_yyvsp[(4) - (9)].str)); }
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 557 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(2) - (12)].str),0,(grib_yyvsp[(6) - (12)].str),0,(grib_yyvsp[(4) - (12)].str),(grib_yyvsp[(8) - (12)].str),(grib_yyvsp[(10) - (12)].str),0,(grib_yyvsp[(12) - (12)].lval),1);  free((grib_yyvsp[(2) - (12)].str));free((grib_yyvsp[(6) - (12)].str));free((grib_yyvsp[(4) - (12)].str));free((grib_yyvsp[(8) - (12)].str));free((grib_yyvsp[(10) - (12)].str)); }
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 558 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(2) - (10)].str),0,(grib_yyvsp[(6) - (10)].str),0,(grib_yyvsp[(4) - (10)].str),(grib_yyvsp[(8) - (10)].str),0,0,(grib_yyvsp[(10) - (10)].lval),1);  free((grib_yyvsp[(2) - (10)].str));free((grib_yyvsp[(6) - (10)].str));free((grib_yyvsp[(4) - (10)].str));free((grib_yyvsp[(8) - (10)].str)); }
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 559 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(4) - (14)].str),0,(grib_yyvsp[(8) - (14)].str),(grib_yyvsp[(2) - (14)].str),(grib_yyvsp[(6) - (14)].str),(grib_yyvsp[(10) - (14)].str),(grib_yyvsp[(12) - (14)].str),0,(grib_yyvsp[(14) - (14)].lval),1);  free((grib_yyvsp[(4) - (14)].str));free((grib_yyvsp[(8) - (14)].str));free((grib_yyvsp[(6) - (14)].str));free((grib_yyvsp[(10) - (14)].str));free((grib_yyvsp[(12) - (14)].str)); free((grib_yyvsp[(2) - (14)].str));}
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 560 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(4) - (12)].str),0,(grib_yyvsp[(8) - (12)].str),(grib_yyvsp[(2) - (12)].str),(grib_yyvsp[(6) - (12)].str),(grib_yyvsp[(10) - (12)].str),0,0,(grib_yyvsp[(12) - (12)].lval),1);  free((grib_yyvsp[(4) - (12)].str));free((grib_yyvsp[(8) - (12)].str));free((grib_yyvsp[(6) - (12)].str));free((grib_yyvsp[(10) - (12)].str)); free((grib_yyvsp[(2) - (12)].str));}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 561 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(4) - (8)].str),(grib_yyvsp[(6) - (8)].concept_value),0,(grib_yyvsp[(2) - (8)].str),0,0,0,0,(grib_yyvsp[(8) - (8)].lval),1);  free((grib_yyvsp[(2) - (8)].str));free((grib_yyvsp[(4) - (8)].str)); }
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 562 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(4) - (11)].str),(grib_yyvsp[(9) - (11)].concept_value),0,(grib_yyvsp[(2) - (11)].str),(grib_yyvsp[(6) - (11)].str),0,0,0,(grib_yyvsp[(11) - (11)].lval),1);  free((grib_yyvsp[(2) - (11)].str));free((grib_yyvsp[(4) - (11)].str));free((grib_yyvsp[(6) - (11)].str)); }
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 567 "griby.y"
    { (grib_yyval.concept_value) = (grib_yyvsp[(2) - (2)].concept_value); (grib_yyvsp[(2) - (2)].concept_value)->next = (grib_yyvsp[(1) - (2)].concept_value);   }
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 571 "griby.y"
    { (grib_yyval.case_value) = (grib_yyvsp[(2) - (2)].case_value); (grib_yyvsp[(2) - (2)].case_value)->next = (grib_yyvsp[(1) - (2)].case_value);   }
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 574 "griby.y"
    { (grib_yyval.case_value) = grib_case_new(grib_parser_context,(grib_yyvsp[(2) - (4)].explist),(grib_yyvsp[(4) - (4)].act));  }
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 578 "griby.y"
    { (grib_yyval.act) = grib_action_create_switch(grib_parser_context,(grib_yyvsp[(3) - (10)].explist),(grib_yyvsp[(6) - (10)].case_value),(grib_yyvsp[(9) - (10)].act)); }
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 579 "griby.y"
    { (grib_yyval.act) = grib_action_create_switch(grib_parser_context,(grib_yyvsp[(3) - (9)].explist),(grib_yyvsp[(6) - (9)].case_value),grib_action_create_noop(grib_parser_context,"continue")); }
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 580 "griby.y"
    { (grib_yyval.act) = grib_action_create_switch(grib_parser_context,(grib_yyvsp[(3) - (7)].explist),(grib_yyvsp[(6) - (7)].case_value),0); }
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 583 "griby.y"
    {
	  				(grib_yyval.concept_value) = grib_concept_value_new(grib_parser_context,(grib_yyvsp[(1) - (5)].str),(grib_yyvsp[(4) - (5)].concept_condition)); free((grib_yyvsp[(1) - (5)].str));}
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 585 "griby.y"
    {
	  				(grib_yyval.concept_value) = grib_concept_value_new(grib_parser_context,(grib_yyvsp[(1) - (5)].str),(grib_yyvsp[(4) - (5)].concept_condition)); free((grib_yyvsp[(1) - (5)].str));}
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 587 "griby.y"
    {
					char buf[80]; sprintf(buf,"%ld",(long)(grib_yyvsp[(1) - (5)].lval)); (grib_yyval.concept_value) = grib_concept_value_new(grib_parser_context,buf,(grib_yyvsp[(4) - (5)].concept_condition));}
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 589 "griby.y"
    {
					char buf[80]; sprintf(buf,"%g",(double)(grib_yyvsp[(1) - (5)].dval)); (grib_yyval.concept_value) = grib_concept_value_new(grib_parser_context,buf,(grib_yyvsp[(4) - (5)].concept_condition));}
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 594 "griby.y"
    { (grib_yyvsp[(1) - (2)].concept_condition)->next = (grib_yyvsp[(2) - (2)].concept_condition); (grib_yyval.concept_condition) = (grib_yyvsp[(1) - (2)].concept_condition); }
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 597 "griby.y"
    { (grib_yyval.concept_condition) = grib_concept_condition_new(grib_parser_context,(grib_yyvsp[(1) - (4)].str),(grib_yyvsp[(3) - (4)].exp)); free((grib_yyvsp[(1) - (4)].str)); }
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 600 "griby.y"
    { (grib_yyval.exp) = new_accessor_expression(grib_parser_context,(grib_yyvsp[(1) - (1)].str)); free((grib_yyvsp[(1) - (1)].str)); }
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 601 "griby.y"
    { (grib_yyval.exp) = new_string_expression(grib_parser_context,(grib_yyvsp[(1) - (1)].str));  free((grib_yyvsp[(1) - (1)].str)); }
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 605 "griby.y"
    { (grib_yyval.exp) = new_long_expression(grib_parser_context,(grib_yyvsp[(1) - (1)].lval));  }
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 606 "griby.y"
    { (grib_yyval.exp) = new_double_expression(grib_parser_context,(grib_yyvsp[(1) - (1)].dval));  /* TODO: change to new_float_expression*/}
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 608 "griby.y"
    { (grib_yyval.exp) = NULL; }
    break;

  case 178:

/* Line 1455 of yacc.c  */
#line 609 "griby.y"
    { (grib_yyval.exp) = new_true_expression(grib_parser_context); }
    break;

  case 179:

/* Line 1455 of yacc.c  */
#line 610 "griby.y"
    { (grib_yyval.exp) = (grib_yyvsp[(2) - (3)].exp); }
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 611 "griby.y"
    { (grib_yyval.exp) = new_unop_expression(grib_parser_context,&grib_op_neg,&grib_op_neg_d,(grib_yyvsp[(2) - (2)].exp)); }
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 612 "griby.y"
    { (grib_yyval.exp) = new_func_expression(grib_parser_context,(grib_yyvsp[(1) - (3)].str),NULL); free((grib_yyvsp[(1) - (3)].str));}
    break;

  case 182:

/* Line 1455 of yacc.c  */
#line 613 "griby.y"
    { (grib_yyval.exp) = new_func_expression(grib_parser_context,(grib_yyvsp[(1) - (4)].str),(grib_yyvsp[(3) - (4)].explist)); free((grib_yyvsp[(1) - (4)].str));}
    break;

  case 183:

/* Line 1455 of yacc.c  */
#line 617 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_pow,NULL,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 621 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_mul,&grib_op_mul_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 186:

/* Line 1455 of yacc.c  */
#line 622 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_div,&grib_op_div_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 187:

/* Line 1455 of yacc.c  */
#line 623 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_modulo,NULL,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 624 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_bit,NULL,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 189:

/* Line 1455 of yacc.c  */
#line 625 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_bitoff,NULL,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 629 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_add,&grib_op_add_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 630 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_sub,&grib_op_sub_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 634 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_gt,&grib_op_gt_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 636 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_eq,&grib_op_eq_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 637 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_lt,&grib_op_lt_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 638 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_ge,&grib_op_ge_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 639 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_le,&grib_op_le_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 640 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_ne,&grib_op_ne_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 200:

/* Line 1455 of yacc.c  */
#line 641 "griby.y"
    { (grib_yyval.exp) = new_string_compare_expression(grib_parser_context,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 201:

/* Line 1455 of yacc.c  */
#line 646 "griby.y"
    { (grib_yyval.exp) = new_unop_expression(grib_parser_context,&grib_op_not,NULL,(grib_yyvsp[(2) - (2)].exp)); }
    break;

  case 203:

/* Line 1455 of yacc.c  */
#line 650 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_and,NULL,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 205:

/* Line 1455 of yacc.c  */
#line 654 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_or,NULL,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp));}
    break;

  case 210:

/* Line 1455 of yacc.c  */
#line 668 "griby.y"
    { (grib_yyval.rule_entry) = grib_new_rule_entry(grib_parser_context,(grib_yyvsp[(1) - (4)].str),(grib_yyvsp[(3) - (4)].exp)); free((grib_yyvsp[(1) - (4)].str)); }
    break;

  case 211:

/* Line 1455 of yacc.c  */
#line 669 "griby.y"
    { (grib_yyval.rule_entry) = grib_new_rule_entry(grib_parser_context,"skip",0);}
    break;

  case 213:

/* Line 1455 of yacc.c  */
#line 673 "griby.y"
    { (grib_yyvsp[(1) - (2)].rule_entry)->next = (grib_yyvsp[(2) - (2)].rule_entry); (grib_yyval.rule_entry) = (grib_yyvsp[(1) - (2)].rule_entry); }
    break;

  case 214:

/* Line 1455 of yacc.c  */
#line 676 "griby.y"
    { (grib_yyval.rules) = grib_new_rule(grib_parser_context,NULL,(grib_yyvsp[(1) - (1)].rule_entry)); }
    break;

  case 215:

/* Line 1455 of yacc.c  */
#line 680 "griby.y"
    { (grib_yyval.rules) = grib_new_rule(grib_parser_context,(grib_yyvsp[(3) - (7)].exp),(grib_yyvsp[(6) - (7)].rule_entry)); }
    break;

  case 217:

/* Line 1455 of yacc.c  */
#line 684 "griby.y"
    { (grib_yyvsp[(1) - (2)].rules)->next = (grib_yyvsp[(2) - (2)].rules); (grib_yyval.rules) = (grib_yyvsp[(1) - (2)].rules); }
    break;



/* Line 1455 of yacc.c  */
#line 3571 "y.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", grib_yyr1[grib_yyn], &grib_yyval, &grib_yyloc);

  YYPOPSTACK (grib_yylen);
  grib_yylen = 0;
  YY_STACK_PRINT (grib_yyss, grib_yyssp);

  *++grib_yyvsp = grib_yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  grib_yyn = grib_yyr1[grib_yyn];

  grib_yystate = grib_yypgoto[grib_yyn - YYNTOKENS] + *grib_yyssp;
  if (0 <= grib_yystate && grib_yystate <= YYLAST && grib_yycheck[grib_yystate] == *grib_yyssp)
    grib_yystate = grib_yytable[grib_yystate];
  else
    grib_yystate = grib_yydefgoto[grib_yyn - YYNTOKENS];

  goto grib_yynewstate;


/*------------------------------------.
| grib_yyerrlab -- here on detecting error |
`------------------------------------*/
grib_yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!grib_yyerrstatus)
    {
      ++grib_yynerrs;
#if ! YYERROR_VERBOSE
      grib_yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T grib_yysize = grib_yysyntax_error (0, grib_yystate, grib_yychar);
	if (grib_yymsg_alloc < grib_yysize && grib_yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T grib_yyalloc = 2 * grib_yysize;
	    if (! (grib_yysize <= grib_yyalloc && grib_yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      grib_yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (grib_yymsg != grib_yymsgbuf)
	      YYSTACK_FREE (grib_yymsg);
	    grib_yymsg = (char *) YYSTACK_ALLOC (grib_yyalloc);
	    if (grib_yymsg)
	      grib_yymsg_alloc = grib_yyalloc;
	    else
	      {
		grib_yymsg = grib_yymsgbuf;
		grib_yymsg_alloc = sizeof grib_yymsgbuf;
	      }
	  }

	if (0 < grib_yysize && grib_yysize <= grib_yymsg_alloc)
	  {
	    (void) grib_yysyntax_error (grib_yymsg, grib_yystate, grib_yychar);
	    grib_yyerror (grib_yymsg);
	  }
	else
	  {
	    grib_yyerror (YY_("syntax error"));
	    if (grib_yysize != 0)
	      goto grib_yyexhaustedlab;
	  }
      }
#endif
    }



  if (grib_yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (grib_yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (grib_yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  grib_yydestruct ("Error: discarding",
		      grib_yytoken, &grib_yylval);
	  grib_yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto grib_yyerrlab1;


/*---------------------------------------------------.
| grib_yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
grib_yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label grib_yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto grib_yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (grib_yylen);
  grib_yylen = 0;
  YY_STACK_PRINT (grib_yyss, grib_yyssp);
  grib_yystate = *grib_yyssp;
  goto grib_yyerrlab1;


/*-------------------------------------------------------------.
| grib_yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
grib_yyerrlab1:
  grib_yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      grib_yyn = grib_yypact[grib_yystate];
      if (grib_yyn != YYPACT_NINF)
	{
	  grib_yyn += YYTERROR;
	  if (0 <= grib_yyn && grib_yyn <= YYLAST && grib_yycheck[grib_yyn] == YYTERROR)
	    {
	      grib_yyn = grib_yytable[grib_yyn];
	      if (0 < grib_yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (grib_yyssp == grib_yyss)
	YYABORT;


      grib_yydestruct ("Error: popping",
		  grib_yystos[grib_yystate], grib_yyvsp);
      YYPOPSTACK (1);
      grib_yystate = *grib_yyssp;
      YY_STACK_PRINT (grib_yyss, grib_yyssp);
    }

  *++grib_yyvsp = grib_yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", grib_yystos[grib_yyn], grib_yyvsp, grib_yylsp);

  grib_yystate = grib_yyn;
  goto grib_yynewstate;


/*-------------------------------------.
| grib_yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
grib_yyacceptlab:
  grib_yyresult = 0;
  goto grib_yyreturn;

/*-----------------------------------.
| grib_yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
grib_yyabortlab:
  grib_yyresult = 1;
  goto grib_yyreturn;

#if !defined(grib_yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| grib_yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
grib_yyexhaustedlab:
  grib_yyerror (YY_("memory exhausted"));
  grib_yyresult = 2;
  /* Fall through.  */
#endif

grib_yyreturn:
  if (grib_yychar != YYEMPTY)
     grib_yydestruct ("Cleanup: discarding lookahead",
		 grib_yytoken, &grib_yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (grib_yylen);
  YY_STACK_PRINT (grib_yyss, grib_yyssp);
  while (grib_yyssp != grib_yyss)
    {
      grib_yydestruct ("Cleanup: popping",
		  grib_yystos[*grib_yyssp], grib_yyvsp);
      YYPOPSTACK (1);
    }
#ifndef grib_yyoverflow
  if (grib_yyss != grib_yyssa)
    YYSTACK_FREE (grib_yyss);
#endif
#if YYERROR_VERBOSE
  if (grib_yymsg != grib_yymsgbuf)
    YYSTACK_FREE (grib_yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (grib_yyresult);
}



/* Line 1675 of yacc.c  */
#line 688 "griby.y"


static grib_concept_value *reverse_concept(grib_concept_value *r,grib_concept_value *s)
{
    grib_concept_value *v;

    if(r == NULL) return s;

    v         = r->next;
    r->next   = s;
    return reverse_concept(v,r);
}


static grib_concept_value* reverse(grib_concept_value* r)
{
    return reverse_concept(r,NULL);
}





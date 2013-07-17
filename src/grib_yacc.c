
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
#line 334 "y.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define grib_yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 346 "y.tab.c"

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
#define YYLAST   1400

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  110
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  45
/* YYNRULES -- Number of rules.  */
#define YYNRULES  216
/* YYNRULES -- Number of states.  */
#define YYNSTATES  688

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   347

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? grib_yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const grib_yytype_uint8 grib_yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,   108,     2,     2,
      97,    98,   106,   109,    93,   104,   100,   107,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   103,    94,
       2,    99,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    95,     2,    96,   105,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   101,     2,   102,     2,     2,     2,     2,
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
      85,    86,    87,    88,    89,    90,    91,    92
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
     674,   676,   678,   680,   682,   684,   686,   695,   703,   711,
     718,   728,   741,   756,   767,   782,   795,   804,   816,   823,
     833,   846,   857,   872,   885,   894,   906,   908,   911,   913,
     916,   921,   932,   942,   950,   956,   962,   968,   974,   976,
     979,   984,   986,   988,   990,   992,   994,   996,   998,  1002,
    1005,  1009,  1014,  1018,  1020,  1024,  1028,  1032,  1036,  1040,
    1042,  1046,  1050,  1052,  1056,  1060,  1064,  1068,  1072,  1076,
    1080,  1083,  1085,  1089,  1091,  1095,  1097,  1099,  1101,  1103,
    1108,  1111,  1113,  1116,  1118,  1126,  1128
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const grib_yytype_int16 grib_yyrhs[] =
{
     111,     0,    -1,   112,    -1,   133,    -1,   114,    -1,   154,
      -1,     1,    -1,    -1,    92,    -1,   113,    93,    92,    -1,
      91,    -1,   113,    93,    91,    -1,   115,    -1,   115,   114,
      -1,   115,    94,   114,    -1,   115,    94,    -1,   120,    94,
      -1,   121,    -1,   129,    -1,   130,    -1,   131,    -1,   132,
      -1,   122,    -1,   136,    -1,    94,    -1,   116,    94,    -1,
     112,    -1,   118,    -1,   119,    -1,   119,    93,   118,    -1,
     148,    -1,     8,    95,    91,    96,    89,   125,   126,    -1,
       8,    95,    91,    96,    89,    95,   117,    96,   125,   126,
      -1,     8,    97,    91,    98,    89,   125,   126,    -1,     8,
      97,    91,    98,    89,    95,   117,    96,   125,   126,    -1,
      12,    95,    91,    96,    89,   125,   126,    -1,    12,    95,
      91,    96,    90,   125,   126,    -1,    19,    95,    91,    96,
      89,   125,   126,    -1,    19,    95,    91,    96,    89,    95,
     117,    96,   125,   126,    -1,    13,    95,    91,    96,    89,
     125,   126,    -1,    18,    95,    91,    96,    89,   125,   126,
      -1,    18,    95,    91,    96,    89,    95,   117,    96,   125,
     126,    -1,    18,    97,    91,    98,    89,   125,   126,    -1,
      18,    97,    91,    98,    89,    95,   117,    96,   125,   126,
      -1,    20,    95,    91,    96,    89,   119,   125,   126,    -1,
      20,    95,    91,    96,    89,   119,   125,    50,    97,    89,
      98,   126,    -1,    20,    95,    91,    96,    89,    97,   117,
      98,   125,   126,    -1,    89,    99,    31,    97,   117,    98,
     125,   126,    -1,    21,    95,    91,    96,    89,   119,   125,
     126,    -1,    21,    95,    91,    96,    89,    97,   117,    98,
     125,   126,    -1,    68,    95,    91,    96,    89,   119,   125,
     126,    -1,    22,    95,    91,    96,    89,    97,   117,    98,
     126,    -1,    29,    89,    97,   117,    98,   125,   126,    -1,
      14,    89,    -1,    14,    90,    -1,    17,    89,   125,   126,
      -1,    17,    89,   100,    89,   125,   126,    -1,    17,    89,
      95,   119,    96,   125,   126,    -1,    26,    89,    -1,    27,
      89,    99,   119,   126,    -1,    28,    89,    99,   119,   126,
      -1,    92,    89,   125,   126,    -1,    92,    89,   100,    89,
     125,   126,    -1,    92,    89,    95,   119,    96,   125,   126,
      -1,    64,    89,    -1,    67,    95,    91,    96,    89,    -1,
      65,    95,    91,    96,    89,    97,   117,    98,    -1,    66,
      95,    91,    96,    89,    97,   117,    98,    -1,    72,    89,
     119,    -1,    58,    89,    97,   117,    98,    -1,    61,    89,
      97,   117,    98,    -1,    62,    89,    97,   117,    98,    -1,
      63,    89,    97,   117,    98,    -1,    60,    95,    91,    96,
      89,   126,    -1,    59,    89,   126,    -1,     9,    89,    90,
      -1,    10,    89,    90,    -1,    23,    89,    99,    89,   126,
      -1,    24,    89,    -1,    23,    89,   100,    89,    99,    89,
     126,    -1,    24,    89,   100,    89,    -1,    25,    89,    89,
      97,   117,    98,   125,   126,    -1,    25,    89,   100,    89,
      89,    97,   117,    98,   125,   126,    -1,    69,    89,    97,
     117,    98,    -1,    70,    89,    97,   117,    98,    -1,    71,
      89,    97,   117,    98,    -1,    55,    89,    97,   117,    98,
      -1,    56,   117,    -1,    73,    97,   148,    98,    -1,    35,
      89,   126,    -1,    50,    89,    99,    46,    -1,    50,    89,
      99,   148,    -1,    50,    89,    99,   101,   113,   102,    -1,
      51,    89,    99,   148,    -1,    52,    90,    -1,    52,    -1,
      52,    97,    91,    98,    90,    -1,    52,    97,    91,    98,
      -1,    53,    90,    -1,    53,    -1,    53,    97,    91,    98,
      90,    -1,    53,    97,    91,    98,    -1,    54,    90,    -1,
      54,    97,    90,    98,    90,    -1,    54,    -1,     4,    97,
     148,    98,   101,   114,   102,    -1,     4,    97,   148,    98,
     101,   114,   102,     6,   101,   114,   102,    -1,     5,    97,
     148,    98,   101,   114,   102,    -1,     5,    97,   148,    98,
     101,   114,   102,     6,   101,   114,   102,    -1,    49,    97,
     148,    98,   123,   116,    -1,    49,    97,   148,    98,   101,
     124,   102,    -1,    49,    97,   148,    98,   101,   124,   102,
       6,   101,   124,   102,    -1,    50,    89,    99,   148,    -1,
      51,    89,    99,   148,    -1,   123,   116,    -1,   124,   123,
     116,    -1,   112,    -1,    99,   117,    -1,   112,    -1,   103,
     127,    -1,   128,    -1,   127,    93,   128,    -1,    36,    -1,
       3,    -1,    40,    -1,    39,    -1,    41,    -1,    44,    -1,
      42,    -1,    45,    -1,    47,    -1,    43,    -1,    48,    -1,
      28,    -1,    37,    -1,    38,    -1,    89,    15,    97,   148,
      98,   101,   114,   102,    -1,    16,    97,   148,    98,   101,
     114,   102,    -1,    11,    97,   117,    98,   101,   114,   102,
      -1,    30,    89,   101,   133,   102,   126,    -1,    30,    89,
      97,    89,    98,   101,   133,   102,   126,    -1,    30,    89,
      97,    89,    93,    90,    93,    89,    93,    89,    98,   126,
      -1,    30,    89,    97,    89,    93,    90,    93,    89,    93,
      89,    93,    89,    98,   126,    -1,    30,    89,    97,    89,
      93,    90,    93,    89,    98,   126,    -1,    30,    89,   100,
      89,    97,    89,    93,    90,    93,    89,    93,    89,    98,
     126,    -1,    30,    89,   100,    89,    97,    89,    93,    90,
      93,    89,    98,   126,    -1,    30,    89,   100,    89,   101,
     133,   102,   126,    -1,    30,    89,   100,    89,    97,    89,
      98,   101,   133,   102,   126,    -1,    32,    89,   101,   133,
     102,   126,    -1,    32,    89,    97,    89,    98,   101,   133,
     102,   126,    -1,    32,    89,    97,    89,    93,    90,    93,
      89,    93,    89,    98,   126,    -1,    32,    89,    97,    89,
      93,    90,    93,    89,    98,   126,    -1,    32,    89,   100,
      89,    97,    89,    93,    90,    93,    89,    93,    89,    98,
     126,    -1,    32,    89,   100,    89,    97,    89,    93,    90,
      93,    89,    98,   126,    -1,    32,    89,   100,    89,   101,
     133,   102,   126,    -1,    32,    89,   100,    89,    97,    89,
      98,   101,   133,   102,   126,    -1,   137,    -1,   133,   137,
      -1,   135,    -1,   134,   135,    -1,    74,   118,   103,   114,
      -1,    75,    97,   117,    98,   101,   134,    76,   103,   114,
     102,    -1,    75,    97,   117,    98,   101,   134,    76,   103,
     102,    -1,    75,    97,   117,    98,   101,   134,   102,    -1,
      90,    99,   101,   138,   102,    -1,    89,    99,   101,   138,
     102,    -1,    91,    99,   101,   138,   102,    -1,    92,    99,
     101,   138,   102,    -1,   139,    -1,   139,   138,    -1,    89,
      99,   148,    94,    -1,    89,    -1,    90,    -1,   140,    -1,
      91,    -1,    92,    -1,    33,    -1,    34,    -1,    97,   148,
      98,    -1,   104,   141,    -1,    89,    97,    98,    -1,    89,
      97,   117,    98,    -1,   141,   105,   142,    -1,   141,    -1,
     143,   106,   142,    -1,   143,   107,   142,    -1,   143,   108,
     142,    -1,   143,    83,   142,    -1,   143,    84,   142,    -1,
     142,    -1,   144,   109,   143,    -1,   144,   104,   143,    -1,
     143,    -1,   145,    82,   144,    -1,   145,    77,   144,    -1,
     145,    81,   144,    -1,   145,    79,   144,    -1,   145,    80,
     144,    -1,   145,    78,   144,    -1,   140,    88,   140,    -1,
      87,   145,    -1,   144,    -1,   146,    85,   145,    -1,   145,
      -1,   147,    86,   146,    -1,   146,    -1,   147,    -1,   152,
      -1,   153,    -1,    89,    99,   148,    94,    -1,    57,    94,
      -1,   150,    -1,   150,   151,    -1,   150,    -1,     4,    97,
     148,    98,   101,   151,   102,    -1,   149,    -1,   149,   154,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const grib_yytype_uint16 grib_yyrline[] =
{
       0,   209,   209,   210,   211,   212,   214,   217,   220,   221,
     222,   223,   225,   226,   227,   228,   231,   232,   233,   234,
     235,   236,   237,   238,   242,   243,   246,   247,   250,   251,
     254,   258,   261,   264,   267,   270,   274,   277,   280,   283,
     286,   289,   292,   295,   298,   301,   305,   308,   311,   314,
     317,   320,   323,   326,   329,   332,   335,   338,   341,   344,
     347,   350,   353,   356,   359,   362,   365,   368,   371,   374,
     377,   380,   383,   386,   389,   391,   393,   396,   399,   402,
     406,   410,   413,   416,   428,   440,   452,   455,   458,   461,
     464,   465,   466,   468,   471,   472,   473,   474,   475,   476,
     477,   478,   480,   481,   482,   486,   487,   488,   489,   493,
     494,   495,   498,   499,   502,   503,   507,   508,   511,   512,
     515,   516,   519,   520,   521,   522,   523,   524,   525,   526,
     527,   528,   529,   530,   531,   532,   535,   538,   541,   544,
     545,   546,   547,   548,   549,   550,   551,   552,   553,   554,
     555,   556,   557,   558,   559,   560,   564,   565,   568,   569,
     572,   576,   577,   578,   581,   583,   585,   587,   591,   592,
     595,   598,   599,   602,   603,   604,   606,   607,   608,   609,
     610,   611,   615,   616,   619,   620,   621,   622,   623,   624,
     627,   628,   629,   632,   634,   635,   636,   637,   638,   639,
     644,   645,   648,   649,   652,   653,   656,   662,   663,   666,
     667,   670,   671,   674,   678,   681,   682
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
  "LONG_TYPE", "NO_COPY", "DUMP", "NO_FAIL", "EDITION_SPECIFIC",
  "OVERRIDE", "HIDDEN", "CAN_BE_MISSING", "MISSING", "CONSTRAINT",
  "COPY_OK", "WHEN", "SET", "SET_NOFAIL", "WRITE", "APPEND", "PRINT",
  "EXPORT", "REMOVE", "SKIP", "PAD", "SECTION_PADDING", "MESSAGE", "PADTO",
  "PADTOEVEN", "PADTOMULTIPLE", "G1_HALF_BYTE", "G1_MESSAGE_LENGTH",
  "G1_SECTION4_LENGTH", "SECTION_LENGTH", "FLAG", "ITERATOR", "NEAREST",
  "BOX", "KSEC", "ASSERT", "CASE", "SWITCH", "DEFAULT", "EQ", "NE", "GE",
  "LE", "LT", "GT", "BIT", "BITOFF", "AND", "OR", "NOT", "IS", "IDENT",
  "STRING", "INTEGER", "FLOAT", "','", "';'", "'['", "']'", "'('", "')'",
  "'='", "'.'", "'{'", "'}'", "':'", "'-'", "'^'", "'*'", "'/'", "'%'",
  "'+'", "$accept", "all", "empty", "dvalues", "instructions",
  "instruction", "semi", "argument_list", "arguments", "argument",
  "simple", "if_block", "when_block", "set", "set_list", "default",
  "flags", "flag_list", "flag", "list_block", "while_block",
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
     345,   346,   347,    44,    59,    91,    93,    40,    41,    61,
      46,   123,   125,    58,    45,    94,    42,    47,    37,    43
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const grib_yytype_uint8 grib_yyr1[] =
{
       0,   110,   111,   111,   111,   111,   111,   112,   113,   113,
     113,   113,   114,   114,   114,   114,   115,   115,   115,   115,
     115,   115,   115,   115,   116,   116,   117,   117,   118,   118,
     119,   120,   120,   120,   120,   120,   120,   120,   120,   120,
     120,   120,   120,   120,   120,   120,   120,   120,   120,   120,
     120,   120,   120,   120,   120,   120,   120,   120,   120,   120,
     120,   120,   120,   120,   120,   120,   120,   120,   120,   120,
     120,   120,   120,   120,   120,   120,   120,   120,   120,   120,
     120,   120,   120,   120,   120,   120,   120,   120,   120,   120,
     120,   120,   120,   120,   120,   120,   120,   120,   120,   120,
     120,   120,   120,   120,   120,   121,   121,   121,   121,   122,
     122,   122,   123,   123,   124,   124,   125,   125,   126,   126,
     127,   127,   128,   128,   128,   128,   128,   128,   128,   128,
     128,   128,   128,   128,   128,   128,   129,   130,   131,   132,
     132,   132,   132,   132,   132,   132,   132,   132,   132,   132,
     132,   132,   132,   132,   132,   132,   133,   133,   134,   134,
     135,   136,   136,   136,   137,   137,   137,   137,   138,   138,
     139,   140,   140,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   142,   142,   143,   143,   143,   143,   143,   143,
     144,   144,   144,   145,   145,   145,   145,   145,   145,   145,
     145,   145,   146,   146,   147,   147,   148,   149,   149,   150,
     150,   151,   151,   152,   153,   154,   154
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
       1,     1,     1,     1,     1,     1,     8,     7,     7,     6,
       9,    12,    14,    10,    14,    12,     8,    11,     6,     9,
      12,    10,    14,    12,     8,    11,     1,     2,     1,     2,
       4,    10,     9,     7,     5,     5,     5,     5,     1,     2,
       4,     1,     1,     1,     1,     1,     1,     1,     3,     2,
       3,     4,     3,     1,     3,     3,     3,     3,     3,     1,
       3,     3,     1,     3,     3,     3,     3,     3,     3,     3,
       2,     1,     3,     1,     3,     1,     1,     1,     1,     4,
       2,     1,     2,     1,     7,     1,     2
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
      23,   156,   215,   213,   207,   208,     5,     0,     0,     0,
       0,     0,     0,     7,     0,     0,    53,    54,     0,     7,
       0,     0,     0,     0,     0,     0,     0,    78,     0,    58,
       0,     0,     0,     0,     0,     7,     0,     0,     0,    94,
       0,    98,     0,   102,     0,     0,   176,   177,     0,   171,
     172,   174,   175,     0,     0,    26,    87,    27,    28,   173,
     183,   189,   192,   201,   203,   205,   206,    30,   210,     0,
       7,     0,     0,     0,     0,    64,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     7,     0,     0,     0,     0,
       7,     0,     1,     0,     0,     0,    15,    13,    16,     0,
       0,   157,     0,     0,   216,     0,     0,     0,     0,    75,
      76,     0,     0,     0,     0,     0,     7,     0,   116,     7,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     7,     0,     0,     0,     0,     0,     0,
       0,   118,    89,     0,     0,     0,     0,     0,     0,     7,
     200,     0,     0,   173,   179,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     7,    74,     0,     7,     7,     7,     0,
       0,     0,     0,     7,     7,     7,    68,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     7,     0,     0,
       0,    14,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   117,     7,    55,     0,     0,     0,
       0,     0,     0,     7,     0,    80,     7,     0,     7,     7,
       0,     0,     0,     0,     0,     0,     0,   123,   133,   122,
     134,   135,   125,   124,   126,   128,   131,   127,   129,   130,
     132,   119,   120,     0,    90,     0,    91,    93,    97,   101,
       0,     0,   180,     0,   178,    29,   171,   199,   182,   187,
     188,   184,   185,   186,   191,   190,   194,   198,   196,   197,
     195,   193,   202,   204,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    88,     0,     0,     7,
       0,     0,   168,   209,     0,     0,     0,     7,    61,     0,
       0,     0,     0,     0,     7,     7,     0,     7,     7,     7,
       0,     7,     7,     7,     7,     7,     0,     0,     0,    77,
       0,     0,     0,    59,    60,     7,     0,     0,     0,     0,
       7,     0,     0,     0,     0,     7,     0,     0,     0,     0,
       0,    10,     8,     0,    96,   100,   103,    86,   181,    69,
       7,    70,    71,    72,     0,     0,    65,     0,    83,    84,
      85,     0,     0,     0,     0,   165,   169,   164,   166,     7,
       7,   167,     0,     0,     0,     0,   211,     0,     0,     7,
       7,     7,     7,     0,     7,     7,     7,     0,     7,    56,
       7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
       7,     7,     7,     7,     7,     0,     0,     0,     0,   139,
       0,     0,     0,     0,   148,   121,     0,     0,     0,     0,
      24,   109,     0,    92,    73,     7,     7,     7,     0,     0,
     158,     0,     7,     0,     7,    62,     0,     0,     0,   105,
     212,   214,   107,     0,    31,     0,    33,   138,    35,    36,
      39,   137,    57,     0,    40,     0,    42,     0,    37,     0,
      30,     7,     0,     7,     0,    79,     7,     0,    52,     0,
       0,     0,     0,     7,     0,     0,     0,     0,     7,     0,
       0,   114,   110,     0,    25,    11,     9,     0,     0,     7,
       0,     0,   163,   159,     0,     7,   170,    63,     0,     0,
       7,     7,     7,     7,     7,     7,     0,    44,     7,    48,
       7,    81,     7,     0,     7,     0,     0,   146,     0,     7,
       0,     0,   154,   112,   113,     0,   115,    66,    67,    50,
       0,     0,   136,    47,     0,     0,     7,     7,     7,     7,
       7,     7,     0,     7,    51,     7,     0,     7,   140,     0,
       0,     0,     7,   149,     0,     0,     0,   160,   162,     0,
       0,     0,    32,    34,    41,    43,    38,    46,     0,    49,
      82,     0,   143,     0,     7,     0,   151,     0,     7,     0,
     161,   106,   108,     7,     0,     7,     0,     7,   147,     7,
       0,     7,   155,   111,    45,     0,   141,     0,   145,   150,
       0,   153,     7,     7,     7,   142,   144,   152
};

/* YYDEFGOTO[NTERM-NUM].  */
static const grib_yytype_int16 grib_yydefgoto[] =
{
      -1,    58,   211,   423,   455,    61,   501,   126,   127,   128,
      62,    63,    64,   498,   499,   189,   212,   321,   322,    65,
      66,    67,    68,    69,   509,   510,    70,    71,   371,   372,
     129,   130,   131,   132,   133,   134,   135,   136,   137,    72,
     456,   457,    74,    75,    76
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -491
static const grib_yytype_int16 grib_yypact[] =
{
    1042,  -491,   -48,   -45,   -73,   -29,   -27,    10,   -25,    26,
     126,    12,    71,   -57,    74,    76,    91,   102,    99,   116,
     177,   184,   191,   199,   206,   209,   217,   223,   142,   225,
     227,   -26,   -15,   -14,   234,   109,   189,   237,   241,   195,
     243,   247,   267,   270,   207,   278,   282,   290,   286,   298,
     299,   304,   300,   302,    -6,   311,   315,   -53,   403,  -491,
    -491,  1131,   321,  -491,  -491,  -491,  -491,  -491,  -491,   229,
    -491,  -491,     2,  -491,  -491,  -491,  -491,   109,   109,   325,
     329,   331,   340,   109,   341,   342,  -491,  -491,   109,    13,
     344,   345,   346,   354,   355,   357,   141,   349,   -46,  -491,
     332,   351,   358,   -13,    22,   348,   109,   359,   360,  -491,
     363,  -491,   366,  -491,   370,   364,  -491,  -491,   109,   365,
    -491,  -491,  -491,   109,   218,  -491,  -491,  -491,   372,   375,
     362,  -491,   -51,   -10,   262,   386,   387,  -491,  -491,   377,
     348,   381,   378,   379,   383,  -491,   391,   397,   401,   402,
     400,   404,   405,   109,   109,   109,   406,    14,   398,   399,
     119,   407,  -491,   408,    -4,   389,  1308,  -491,  -491,   410,
     411,  -491,   409,   416,  -491,   418,   419,   422,   428,  -491,
    -491,   429,   433,   434,   435,   109,   109,   415,  -491,   348,
     438,   437,   440,   441,   442,   443,   451,   452,   453,   446,
     455,   109,   109,   109,   456,   457,   229,   458,   459,   229,
     310,  -491,  -491,   454,    64,   109,   460,   461,   462,   109,
     262,    83,   463,  -491,  -491,   109,   167,   218,   218,   218,
     218,   218,   218,   218,   218,   218,   218,   218,   218,   218,
     218,   109,   109,   109,  -491,   466,   109,   109,   109,   468,
     469,   470,   471,   109,   109,   109,  -491,   472,   474,   109,
     476,   464,   413,   464,   464,   109,   465,   348,   464,   109,
     467,  -491,   449,   109,   109,   450,   473,   479,   480,   477,
     182,   486,   478,   481,  -491,   483,  -491,   491,   494,   495,
     496,   504,   505,   348,   497,  -491,   109,   508,   348,   348,
     500,   -54,    51,   -23,    27,   106,    55,  -491,  -491,  -491,
    -491,  -491,  -491,  -491,  -491,  -491,  -491,  -491,  -491,  -491,
    -491,   507,  -491,   -20,  -491,   139,  -491,  -491,   512,   514,
     515,   509,  -491,   510,  -491,  -491,  -491,  -491,  -491,  -491,
    -491,  -491,  -491,  -491,   -51,   -51,   -10,   -10,   -10,   -10,
     -10,   -10,   262,   386,   511,   517,   518,   519,   520,   523,
     524,   530,   531,   525,   526,   527,  -491,   521,   528,   109,
     529,   447,   464,  -491,   536,   538,   533,   483,  -491,   539,
     532,   534,  1222,  1308,   128,   186,  1308,   483,   483,   483,
    1308,   483,   348,   194,   201,   202,   178,   187,   546,  -491,
     556,   549,   551,  -491,  -491,   483,   537,   548,   561,   229,
     348,   563,   553,   567,   229,   348,   310,   568,   569,    35,
     565,  -491,  -491,   -28,  -491,  -491,  -491,  -491,  -491,  -491,
     348,  -491,  -491,  -491,   564,   566,  -491,   109,  -491,  -491,
    -491,   482,   559,   570,   109,  -491,  -491,  -491,  -491,   483,
     348,  -491,   571,   572,     3,   560,   -36,   562,   573,   109,
     348,   109,   348,   574,   348,   348,   348,   575,   348,  -491,
     109,   348,   109,   348,   109,   348,   109,   483,   109,   483,
     109,   348,   483,   109,   348,   577,   229,    65,    93,  -491,
     578,   229,    66,   100,  -491,  -491,   579,   580,   565,   -24,
    -491,   586,   213,  -491,  -491,   109,   109,   483,   109,   -39,
    -491,  1308,   483,   587,   348,  -491,  1308,   -36,   145,   659,
    -491,  -491,   660,   588,  -491,   590,  -491,  -491,  -491,  -491,
    -491,  -491,  -491,   591,  -491,   592,  -491,   593,  -491,   576,
     463,   -31,   584,   348,   585,  -491,   348,   594,  -491,   601,
     272,   603,   596,   348,   609,   276,   610,   598,   348,   109,
     109,   586,   698,   565,  -491,  -491,  -491,   607,   608,   348,
     604,   605,  -491,  -491,   611,   348,  -491,  -491,   614,   615,
     483,   483,   483,   483,   483,   483,   612,  -491,   483,  -491,
     348,  -491,   483,   131,   348,   617,   229,  -491,   140,   348,
     618,   229,  -491,  -491,  -491,   619,   586,  -491,  -491,  -491,
    1308,   947,  -491,  -491,  1308,  1308,   348,   348,   348,   348,
     348,   348,   629,   348,  -491,   348,   630,   348,  -491,   632,
     280,   633,   348,  -491,   634,   316,    35,  -491,  -491,   623,
     624,   625,  -491,  -491,  -491,  -491,  -491,  -491,   635,  -491,
    -491,   150,  -491,   165,   348,   636,  -491,   166,   348,   -22,
    -491,  -491,  -491,   348,   639,   348,   641,   348,  -491,   348,
     642,   348,  -491,  -491,  -491,   637,  -491,   638,  -491,  -491,
     640,  -491,   348,   348,   348,  -491,  -491,  -491
};

/* YYPGOTO[NTERM-NUM].  */
static const grib_yytype_int16 grib_yypgoto[] =
{
    -491,  -491,     7,  -491,     0,  -491,  -490,   448,  -221,  -151,
    -491,  -491,  -491,  -322,   -81,   -60,   171,  -491,   323,  -491,
    -491,  -491,  -491,  -192,  -491,    48,  -491,   -59,  -248,  -491,
     -99,   613,   196,   147,   204,  -115,   490,  -491,   -65,  -491,
       5,   285,  -491,  -491,   670
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -8
static const grib_yytype_int16 grib_yytable[] =
{
      60,   420,   256,   220,   335,    73,   172,    59,   561,   156,
     171,   156,   175,   176,   303,   374,   375,   306,   156,   586,
     379,    36,    79,   184,    80,   223,   417,   418,   417,   418,
     417,   418,   228,   229,   283,   508,   160,   571,    90,   406,
      91,   213,   125,   199,   407,   260,   161,   116,   117,    77,
     298,   299,    78,   173,   200,   230,   231,   232,   222,    36,
      81,   167,    82,   572,   109,   502,   169,    55,    56,   170,
      84,   110,   210,   606,   503,   111,   113,    73,   562,   410,
     673,   419,   112,   114,   204,   417,   418,   205,   206,   257,
     125,   173,   262,   157,   233,   270,   188,   116,   117,   234,
     267,   118,   518,   119,   120,   121,   122,    83,   185,    88,
     324,   123,   186,   187,   376,   261,   116,   117,   124,   207,
     411,    85,   208,   209,   446,   412,   352,   337,   223,   223,
     223,   223,   223,   223,   223,   223,   223,   223,   223,   223,
     223,   223,   116,   117,   169,    55,    56,   170,   408,   326,
     327,   118,   409,   119,   120,   121,   122,   415,   551,   556,
      89,   123,   125,   552,   557,   325,   271,   188,   124,    92,
     118,    93,   119,   120,   121,   122,   260,   563,   116,   117,
     123,   332,   169,    55,    56,   170,    94,   124,    96,   169,
      55,    56,   170,   125,   368,   553,   118,    95,   119,   120,
     121,   122,   558,   413,   380,    97,   123,   414,   381,   262,
     125,   116,   117,   124,   265,    86,    87,   488,   186,   266,
     116,   117,   493,   459,   626,   392,   125,   186,   125,   627,
     421,   422,   118,   631,   119,   120,   121,   122,   632,   106,
     196,   197,   123,   664,   171,   477,   479,   171,   665,   124,
     125,   116,   117,   125,   125,   125,   336,   120,   666,   670,
     125,   125,   125,   667,   671,   118,    98,   119,   120,   121,
     122,   387,   388,    99,   118,   476,   119,   120,   121,   122,
     100,   461,   124,   138,   478,   186,   507,   570,   101,   470,
     141,   124,   188,   186,   550,   102,   472,   474,   103,   555,
     186,   186,   146,   125,   565,   566,   104,   119,   120,   121,
     122,   244,   105,   307,   107,   123,   108,   450,   169,    55,
      56,   170,   124,   115,   460,   462,   139,   464,   465,   466,
     140,   468,   142,   471,   473,   475,   143,   563,   308,   235,
     236,   237,   238,   239,   240,   484,   309,   310,   311,   312,
     313,   314,   315,   316,   317,   318,   144,   319,   320,   145,
     286,   169,    55,    56,   170,   169,    55,    56,   170,   169,
      55,    56,   170,   147,   594,   150,   125,   148,   599,   513,
     344,   345,   654,   458,   188,   149,   463,   151,   152,   514,
     467,   188,   188,   153,   188,   188,   188,   154,   188,   155,
     188,   188,   188,   162,   630,   169,    55,    56,   170,   635,
     158,   540,   188,   540,   159,   168,   177,   541,   658,   543,
     178,   179,   546,   338,   339,   340,   341,   342,   343,   171,
     180,   201,   182,   183,   171,   190,   191,   192,   378,   346,
     347,   348,   349,   350,   351,   193,   194,   569,   195,   198,
     202,   210,   575,   262,   216,   203,   188,   217,   214,   215,
     218,   219,   221,   226,   399,   225,   125,   227,   125,   403,
     404,   241,   245,   242,   243,   246,   247,   125,   160,   125,
     248,   125,   249,   125,   188,   125,   188,   125,   250,   188,
     125,   171,   251,   252,   603,   604,   171,   253,   260,   263,
     264,   254,   255,   259,   285,   269,   273,   373,   268,   272,
     161,   574,   125,   125,   188,   274,   275,   276,   277,   188,
     616,   617,   618,   619,   620,   621,   278,   279,   623,   280,
     281,   181,   625,   282,   287,   288,   289,   290,   291,   292,
     293,   294,   295,   296,   297,   301,   302,   304,   305,   445,
     261,   382,   323,   370,   377,   659,   508,   573,   328,   329,
     330,   334,   355,   469,   359,   360,   361,   362,   384,   385,
     366,   171,   367,   369,   383,   389,   171,   391,   386,   390,
     393,   489,   186,   394,   395,   396,   494,   188,   188,   188,
     188,   188,   188,   397,   398,   188,   400,   402,   405,   188,
     416,   504,   424,   258,   425,   426,   430,   427,   428,   429,
     637,   639,   434,   435,   640,   641,   431,   432,   433,   436,
     437,   515,   441,   438,   439,   440,   442,   485,   444,   449,
     452,   524,   453,   526,   284,   528,   529,   530,   447,   532,
     448,   451,   534,   480,   536,   481,   538,   482,   483,   486,
     487,   300,   545,   490,   491,   548,   492,   496,   497,   500,
     511,   505,   519,   506,   521,   578,   579,   331,   512,   333,
     549,   554,   516,   517,   585,   522,   527,   531,   559,   560,
     564,   576,   588,   590,   580,   577,   581,   582,   583,   584,
     593,   354,   592,   595,   356,   357,   358,   596,   598,   601,
     600,   363,   364,   365,   605,   607,   608,   610,   611,   622,
     629,   634,   587,   612,   589,   614,   615,   591,   648,   651,
     636,   653,   655,   657,   597,   660,   661,   662,   675,   602,
     677,   680,   353,   663,   669,   682,   683,   224,   684,   495,
     609,   520,   174,     0,   401,     0,   613,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   624,     0,     0,     0,   628,     0,     0,     0,     0,
     633,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   642,   643,   644,
     645,   646,   647,     0,   649,     0,   650,     0,   652,     0,
       0,     0,     0,   656,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   443,     0,     0,
       0,     0,     0,     0,     0,   668,     0,     0,     0,   672,
       0,     0,     0,     0,   674,     0,   676,     0,   678,     0,
     679,     0,   681,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   685,   686,   687,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   523,     0,   525,
       0,     0,     0,     0,     0,     0,     0,     0,   533,     0,
     535,     0,   537,     0,   539,     0,   542,     0,   544,     0,
       0,   547,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   163,     3,   567,   568,     4,     5,     6,     7,     8,
       9,    10,     0,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,     0,    26,
       0,     0,    27,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    28,    29,    30,    31,
      32,    33,    34,    35,     0,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,     0,    53,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   164,     0,     0,   165,
       0,     0,    -7,     1,     0,     0,     2,     3,     0,   638,
       4,     5,     6,     7,     8,     9,    10,     0,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,     0,    26,     0,     0,    27,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,     0,    53,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    54,    55,    56,    57,   163,     3,     0,     0,     4,
       5,     6,     7,     8,     9,    10,     0,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,     0,    26,     0,     0,    27,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      28,    29,    30,    31,    32,    33,    34,    35,     0,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,     0,    53,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     164,     0,     0,   165,     0,   166,   163,     3,     0,     0,
       4,     5,     6,     7,     8,     9,    10,     0,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,     0,    26,     0,     0,    27,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,     0,    53,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   454,   163,     3,   165,     0,     4,     5,     6,     7,
       8,     9,    10,     0,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,     0,
      26,     0,     0,    27,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    28,    29,    30,
      31,    32,    33,    34,    35,     0,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,     0,    53,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   164,     0,     0,
     165
};

static const grib_yytype_int16 grib_yycheck[] =
{
       0,   323,   153,   118,   225,     0,     4,     0,   498,    15,
      69,    15,    77,    78,   206,   263,   264,   209,    15,    50,
     268,    57,    95,    88,    97,   124,    50,    51,    50,    51,
      50,    51,    83,    84,   185,    74,    89,    76,    95,    93,
      97,   106,    35,    89,    98,    31,    99,    33,    34,    97,
     201,   202,    97,    89,   100,   106,   107,   108,   123,    57,
      89,    61,    89,   102,    90,    93,    89,    90,    91,    92,
      95,    97,   103,   563,   102,    90,    90,    72,   102,   102,
     102,   101,    97,    97,    97,    50,    51,   100,   101,   154,
      83,    89,   157,    99,   104,    99,    89,    33,    34,   109,
     160,    87,    99,    89,    90,    91,    92,    97,    95,    97,
      46,    97,    99,   100,   265,   101,    33,    34,   104,    97,
      93,    95,   100,   101,   372,    98,   241,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,   238,
     239,   240,    33,    34,    89,    90,    91,    92,    97,   214,
     215,    87,   101,    89,    90,    91,    92,   102,    93,    93,
      89,    97,   155,    98,    98,   101,   166,   160,   104,    95,
      87,    95,    89,    90,    91,    92,    31,   499,    33,    34,
      97,    98,    89,    90,    91,    92,    95,   104,    89,    89,
      90,    91,    92,   186,   259,   102,    87,    95,    89,    90,
      91,    92,   102,    97,   269,    89,    97,   101,   273,   274,
     203,    33,    34,   104,    95,    89,    90,   409,    99,   100,
      33,    34,   414,    95,    93,   285,   219,    99,   221,    98,
      91,    92,    87,    93,    89,    90,    91,    92,    98,    97,
      99,   100,    97,    93,   303,   396,   397,   306,    98,   104,
     243,    33,    34,   246,   247,   248,    89,    90,    93,    93,
     253,   254,   255,    98,    98,    87,    89,    89,    90,    91,
      92,    89,    90,    89,    87,    97,    89,    90,    91,    92,
      89,    95,   104,    94,    97,    99,   437,   508,    89,    95,
      95,   104,   285,    99,   486,    89,    95,    95,    89,   491,
      99,    99,    95,   296,    91,    92,    89,    89,    90,    91,
      92,   140,    89,     3,    89,    97,    89,   377,    89,    90,
      91,    92,   104,    89,   384,   385,    89,   387,   388,   389,
      89,   391,    89,   393,   394,   395,    89,   659,    28,    77,
      78,    79,    80,    81,    82,   405,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    89,    47,    48,    89,
     189,    89,    90,    91,    92,    89,    90,    91,    92,    89,
      90,    91,    92,    95,   102,    89,   369,    95,   102,   444,
     233,   234,   102,   383,   377,    95,   386,    89,    89,   449,
     390,   384,   385,    89,   387,   388,   389,    97,   391,    97,
     393,   394,   395,     0,   596,    89,    90,    91,    92,   601,
      99,   476,   405,   478,    99,    94,    91,   477,   102,   479,
      91,    90,   482,   227,   228,   229,   230,   231,   232,   488,
      90,    99,    91,    91,   493,    91,    91,    91,   267,   235,
     236,   237,   238,   239,   240,    91,    91,   507,    91,   100,
      99,   103,   512,   518,    91,    97,   449,    91,    99,    99,
      90,    97,    97,    88,   293,    93,   459,   105,   461,   298,
     299,    85,    91,    86,    97,    97,    97,   470,    89,   472,
      97,   474,    91,   476,   477,   478,   479,   480,    91,   482,
     483,   550,    91,    91,   559,   560,   555,    97,    31,   101,
     101,    97,    97,    97,    89,    97,    97,    94,   101,    99,
      99,   511,   505,   506,   507,    99,    98,    98,    96,   512,
     580,   581,   582,   583,   584,   585,    98,    98,   588,    96,
      96,    83,   592,    98,    96,    98,    96,    96,    96,    96,
      89,    89,    89,    97,    89,    89,    89,    89,    89,   102,
     101,   101,    98,    89,    89,   636,    74,   509,    98,    98,
      98,    98,    96,   392,    96,    96,    96,    96,    89,    89,
      98,   630,    98,    97,   101,    89,   635,    96,   101,   101,
      89,   410,    99,    89,    89,    89,   415,   580,   581,   582,
     583,   584,   585,    89,    89,   588,    99,    89,    98,   592,
      93,   430,    90,   155,    90,    90,    89,    98,    98,    98,
     610,   611,    89,    89,   614,   615,    98,    98,    98,    89,
      89,   450,   101,    98,    98,    98,    98,    90,    99,    96,
      98,   460,    98,   462,   186,   464,   465,   466,   102,   468,
     102,   102,   471,    97,   473,    89,   475,    98,    97,   101,
      89,   203,   481,    90,   101,   484,    89,    89,    89,    94,
     101,    97,   102,    97,   102,     6,     6,   219,    98,   221,
      93,    93,   101,   101,    98,   102,   102,   102,    99,    99,
      94,    94,    98,    98,    96,   514,    96,    96,    96,    96,
      89,   243,    98,    90,   246,   247,   248,   101,    89,   101,
      90,   253,   254,   255,     6,    98,    98,   103,   103,    97,
      93,    93,   541,   102,   543,   101,   101,   546,    89,    89,
     101,    89,    89,    89,   553,   102,   102,   102,    89,   558,
      89,    89,   242,    98,    98,    98,    98,   124,    98,   416,
     569,   456,    72,    -1,   296,    -1,   575,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   590,    -1,    -1,    -1,   594,    -1,    -1,    -1,    -1,
     599,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   616,   617,   618,
     619,   620,   621,    -1,   623,    -1,   625,    -1,   627,    -1,
      -1,    -1,    -1,   632,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   369,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   654,    -1,    -1,    -1,   658,
      -1,    -1,    -1,    -1,   663,    -1,   665,    -1,   667,    -1,
     669,    -1,   671,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   682,   683,   684,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   459,    -1,   461,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   470,    -1,
     472,    -1,   474,    -1,   476,    -1,   478,    -1,   480,    -1,
      -1,   483,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     4,     5,   505,   506,     8,     9,    10,    11,    12,
      13,    14,    -1,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    -1,    32,
      -1,    -1,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    49,    50,    51,    52,
      53,    54,    55,    56,    -1,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    -1,    75,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    89,    -1,    -1,    92,
      -1,    -1,     0,     1,    -1,    -1,     4,     5,    -1,   102,
       8,     9,    10,    11,    12,    13,    14,    -1,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    -1,    32,    -1,    -1,    35,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    -1,    75,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    89,    90,    91,    92,     4,     5,    -1,    -1,     8,
       9,    10,    11,    12,    13,    14,    -1,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    -1,    32,    -1,    -1,    35,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      49,    50,    51,    52,    53,    54,    55,    56,    -1,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    -1,    75,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      89,    -1,    -1,    92,    -1,    94,     4,     5,    -1,    -1,
       8,     9,    10,    11,    12,    13,    14,    -1,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    -1,    32,    -1,    -1,    35,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    -1,    75,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    89,     4,     5,    92,    -1,     8,     9,    10,    11,
      12,    13,    14,    -1,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    -1,
      32,    -1,    -1,    35,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    49,    50,    51,
      52,    53,    54,    55,    56,    -1,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    -1,    75,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,    -1,    -1,
      92
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const grib_yytype_uint8 grib_yystos[] =
{
       0,     1,     4,     5,     8,     9,    10,    11,    12,    13,
      14,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    32,    35,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    75,    89,    90,    91,    92,   111,   112,
     114,   115,   120,   121,   122,   129,   130,   131,   132,   133,
     136,   137,   149,   150,   152,   153,   154,    97,    97,    95,
      97,    89,    89,    97,    95,    95,    89,    90,    97,    89,
      95,    97,    95,    95,    95,    95,    89,    89,    89,    89,
      89,    89,    89,    89,    89,    89,    97,    89,    89,    90,
      97,    90,    97,    90,    97,    89,    33,    34,    87,    89,
      90,    91,    92,    97,   104,   112,   117,   118,   119,   140,
     141,   142,   143,   144,   145,   146,   147,   148,    94,    89,
      89,    95,    89,    89,    89,    89,    95,    95,    95,    95,
      89,    89,    89,    89,    97,    97,    15,    99,    99,    99,
      89,    99,     0,     4,    89,    92,    94,   114,    94,    89,
      92,   137,     4,    89,   154,   148,   148,    91,    91,    90,
      90,   117,    91,    91,   148,    95,    99,   100,   112,   125,
      91,    91,    91,    91,    91,    91,    99,   100,   100,    89,
     100,    99,    99,    97,    97,   100,   101,    97,   100,   101,
     103,   112,   126,   148,    99,    99,    91,    91,    90,    97,
     145,    97,   148,   140,   141,    93,    88,   105,    83,    84,
     106,   107,   108,   104,   109,    77,    78,    79,    80,    81,
      82,    85,    86,    97,   126,    91,    97,    97,    97,    91,
      91,    91,    91,    97,    97,    97,   119,   148,   117,    97,
      31,   101,   148,   101,   101,    95,   100,   125,   101,    97,
      99,   114,    99,    97,    99,    98,    98,    96,    98,    98,
      96,    96,    98,   119,   117,    89,   126,    96,    98,    96,
      96,    96,    96,    89,    89,    89,    97,    89,   119,   119,
     117,    89,    89,   133,    89,    89,   133,     3,    28,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    47,
      48,   127,   128,    98,    46,   101,   148,   148,    98,    98,
      98,   117,    98,   117,    98,   118,    89,   140,   142,   142,
     142,   142,   142,   142,   143,   143,   144,   144,   144,   144,
     144,   144,   145,   146,   117,    96,   117,   117,   117,    96,
      96,    96,    96,   117,   117,   117,    98,    98,   148,    97,
      89,   138,   139,    94,   138,   138,   119,    89,   126,   138,
     148,   148,   101,   101,    89,    89,   101,    89,    90,    89,
     101,    96,   125,    89,    89,    89,    89,    89,    89,   126,
      99,   117,    89,   126,   126,    98,    93,    98,    97,   101,
     102,    93,    98,    97,   101,   102,    93,    50,    51,   101,
     123,    91,    92,   113,    90,    90,    90,    98,    98,    98,
      89,    98,    98,    98,    89,    89,    89,    89,    98,    98,
      98,   101,    98,   117,    99,   102,   138,   102,   102,    96,
     125,   102,    98,    98,    89,   114,   150,   151,   114,    95,
     125,    95,   125,   114,   125,   125,   125,   114,   125,   126,
      95,   125,    95,   125,    95,   125,    97,   119,    97,   119,
      97,    89,    98,    97,   125,    90,   101,    89,   133,   126,
      90,   101,    89,   133,   126,   128,    89,    89,   123,   124,
      94,   116,    93,   102,   126,    97,    97,   119,    74,   134,
     135,   101,    98,   148,   125,   126,   101,   101,    99,   102,
     151,   102,   102,   117,   126,   117,   126,   102,   126,   126,
     126,   102,   126,   117,   126,   117,   126,   117,   126,   117,
     148,   125,   117,   125,   117,   126,   125,   117,   126,    93,
     133,    93,    98,   102,    93,   133,    93,    98,   102,    99,
      99,   116,   102,   123,    94,    91,    92,   117,   117,   125,
     118,    76,   102,   135,   114,   125,    94,   126,     6,     6,
      96,    96,    96,    96,    96,    98,    50,   126,    98,   126,
      98,   126,    98,    89,   102,    90,   101,   126,    89,   102,
      90,   101,   126,   148,   148,     6,   116,    98,    98,   126,
     103,   103,   102,   126,   101,   101,   125,   125,   125,   125,
     125,   125,    97,   125,   126,   125,    93,    98,   126,    93,
     133,    93,    98,   126,    93,   133,   101,   114,   102,   114,
     114,   114,   126,   126,   126,   126,   126,   126,    89,   126,
     126,    89,   126,    89,   102,    89,   126,    89,   102,   124,
     102,   102,   102,    98,    93,    98,    93,    98,   126,    98,
      93,    98,   126,   102,   126,    89,   126,    89,   126,   126,
      89,   126,    98,    98,    98,   126,   126,   126
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
#line 209 "griby.y"
    { grib_parser_all_actions = 0;grib_parser_concept=0; grib_parser_rules=0; }
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 210 "griby.y"
    { grib_parser_concept     = reverse((grib_yyvsp[(1) - (1)].concept_value)); }
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 211 "griby.y"
    { grib_parser_all_actions = (grib_yyvsp[(1) - (1)].act); }
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 212 "griby.y"
    { grib_parser_rules       = (grib_yyvsp[(1) - (1)].rules); }
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 214 "griby.y"
    { grib_parser_all_actions = 0; grib_parser_concept=0; grib_parser_rules=0; }
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 220 "griby.y"
    { (grib_yyval.dvalue)=grib_darray_push(0,0,(grib_yyvsp[(1) - (1)].dval));}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 221 "griby.y"
    { (grib_yyval.dvalue)=grib_darray_push(0,(grib_yyvsp[(1) - (3)].dvalue),(grib_yyvsp[(3) - (3)].dval));}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 222 "griby.y"
    { (grib_yyval.dvalue)=grib_darray_push(0,0,(grib_yyvsp[(1) - (1)].lval));}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 223 "griby.y"
    { (grib_yyval.dvalue)=grib_darray_push(0,(grib_yyvsp[(1) - (3)].dvalue),(grib_yyvsp[(3) - (3)].lval));}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 226 "griby.y"
    { (grib_yyvsp[(1) - (2)].act)->next = (grib_yyvsp[(2) - (2)].act); (grib_yyval.act) = (grib_yyvsp[(1) - (2)].act); }
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 227 "griby.y"
    { (grib_yyvsp[(1) - (3)].act)->next = (grib_yyvsp[(3) - (3)].act); (grib_yyval.act) = (grib_yyvsp[(1) - (3)].act); }
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 228 "griby.y"
    {  (grib_yyval.act) = (grib_yyvsp[(1) - (2)].act);}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 246 "griby.y"
    { (grib_yyval.explist) = 0; }
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 251 "griby.y"
    { (grib_yyvsp[(1) - (3)].explist)->next = (grib_yyvsp[(3) - (3)].explist); (grib_yyval.explist) = (grib_yyvsp[(1) - (3)].explist); }
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 254 "griby.y"
    { (grib_yyval.explist) = grib_arguments_new(grib_parser_context,(grib_yyvsp[(1) - (1)].exp),NULL); }
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 259 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (7)].str),"unsigned",(grib_yyvsp[(3) - (7)].lval),NULL,(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL,NULL);        free((grib_yyvsp[(5) - (7)].str));  }
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 262 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (10)].str),"unsigned",(grib_yyvsp[(3) - (10)].lval),(grib_yyvsp[(7) - (10)].explist),(grib_yyvsp[(9) - (10)].explist),(grib_yyvsp[(10) - (10)].lval),NULL,NULL);        free((grib_yyvsp[(5) - (10)].str));  }
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 265 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (7)].str),"unsigned_bits",(grib_yyvsp[(3) - (7)].lval),NULL,(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL,NULL);        free((grib_yyvsp[(5) - (7)].str));  }
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 268 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (10)].str),"unsigned_bits",(grib_yyvsp[(3) - (10)].lval),(grib_yyvsp[(7) - (10)].explist),(grib_yyvsp[(9) - (10)].explist),(grib_yyvsp[(10) - (10)].lval),NULL,NULL);        free((grib_yyvsp[(5) - (10)].str));  }
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 271 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (7)].str),"ascii",(grib_yyvsp[(3) - (7)].lval),NULL,(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL,NULL);  free((grib_yyvsp[(5) - (7)].str));  }
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 275 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (7)].str),"ascii",(grib_yyvsp[(3) - (7)].lval),NULL,(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL,NULL);  free((grib_yyvsp[(5) - (7)].str));  }
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 278 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (7)].str),"bytes",(grib_yyvsp[(3) - (7)].lval),NULL,(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL,NULL);      free((grib_yyvsp[(5) - (7)].str));  }
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 281 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (10)].str),"bytes",(grib_yyvsp[(3) - (10)].lval),(grib_yyvsp[(7) - (10)].explist),(grib_yyvsp[(9) - (10)].explist),(grib_yyvsp[(10) - (10)].lval),NULL,NULL);      free((grib_yyvsp[(5) - (10)].str));  }
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 284 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (7)].str),"ksec1expver",(grib_yyvsp[(3) - (7)].lval),NULL,(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL,NULL);  free((grib_yyvsp[(5) - (7)].str));  }
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 287 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (7)].str),"signed",(grib_yyvsp[(3) - (7)].lval),NULL,(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL,NULL);      free((grib_yyvsp[(5) - (7)].str));  }
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 290 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (10)].str),"signed",(grib_yyvsp[(3) - (10)].lval),(grib_yyvsp[(7) - (10)].explist),(grib_yyvsp[(9) - (10)].explist),(grib_yyvsp[(10) - (10)].lval),NULL,NULL);      free((grib_yyvsp[(5) - (10)].str));  }
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 293 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (7)].str),"signed_bits",(grib_yyvsp[(3) - (7)].lval),NULL,(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL,NULL);      free((grib_yyvsp[(5) - (7)].str));  }
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 296 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (10)].str),"signed_bits",(grib_yyvsp[(3) - (10)].lval),(grib_yyvsp[(7) - (10)].explist),(grib_yyvsp[(9) - (10)].explist),(grib_yyvsp[(10) - (10)].lval),NULL,NULL);      free((grib_yyvsp[(5) - (10)].str));  }
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 299 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (8)].str),"codetable",(grib_yyvsp[(3) - (8)].lval), (grib_yyvsp[(6) - (8)].explist),(grib_yyvsp[(7) - (8)].explist),(grib_yyvsp[(8) - (8)].lval),NULL,NULL);    free((grib_yyvsp[(5) - (8)].str)); }
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 302 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (12)].str),"codetable",(grib_yyvsp[(3) - (12)].lval), (grib_yyvsp[(6) - (12)].explist),(grib_yyvsp[(7) - (12)].explist),(grib_yyvsp[(12) - (12)].lval),NULL,(grib_yyvsp[(10) - (12)].str));
					free((grib_yyvsp[(5) - (12)].str));free((grib_yyvsp[(10) - (12)].str)); }
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 306 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (10)].str),"codetable",(grib_yyvsp[(3) - (10)].lval), (grib_yyvsp[(7) - (10)].explist),(grib_yyvsp[(9) - (10)].explist),(grib_yyvsp[(10) - (10)].lval),NULL,NULL);    free((grib_yyvsp[(5) - (10)].str)); }
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 309 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(1) - (8)].str),"getenv",0,(grib_yyvsp[(5) - (8)].explist),(grib_yyvsp[(7) - (8)].explist),(grib_yyvsp[(8) - (8)].lval),NULL,NULL);    free((grib_yyvsp[(1) - (8)].str)); }
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 312 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (8)].str),"complex_codetable",(grib_yyvsp[(3) - (8)].lval), (grib_yyvsp[(6) - (8)].explist),(grib_yyvsp[(7) - (8)].explist),(grib_yyvsp[(8) - (8)].lval),NULL,NULL);    free((grib_yyvsp[(5) - (8)].str)); }
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 315 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (10)].str),"complex_codetable",(grib_yyvsp[(3) - (10)].lval), (grib_yyvsp[(7) - (10)].explist),(grib_yyvsp[(9) - (10)].explist),(grib_yyvsp[(10) - (10)].lval),NULL,NULL);    free((grib_yyvsp[(5) - (10)].str)); }
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 318 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (8)].str),"codeflag",(grib_yyvsp[(3) - (8)].lval), (grib_yyvsp[(6) - (8)].explist),(grib_yyvsp[(7) - (8)].explist),(grib_yyvsp[(8) - (8)].lval),NULL,NULL);  free((grib_yyvsp[(5) - (8)].str)); }
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 321 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (9)].str),"lookup",(grib_yyvsp[(3) - (9)].lval),(grib_yyvsp[(7) - (9)].explist),NULL,(grib_yyvsp[(9) - (9)].lval),NULL,NULL); free((grib_yyvsp[(5) - (9)].str)); }
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 324 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (7)].str),"bit",0,(grib_yyvsp[(4) - (7)].explist),(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL,NULL); free((grib_yyvsp[(2) - (7)].str)); }
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 327 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (2)].str),"label",0,NULL,NULL,0,NULL,NULL);   free((grib_yyvsp[(2) - (2)].str));  }
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 330 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (2)].str),"label",0,NULL,NULL,0,NULL,NULL);   free((grib_yyvsp[(2) - (2)].str));  }
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 333 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (4)].str),"ibmfloat",4,NULL,(grib_yyvsp[(3) - (4)].explist),(grib_yyvsp[(4) - (4)].lval),NULL,NULL);free((grib_yyvsp[(2) - (4)].str));  }
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 336 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(4) - (6)].str),"ibmfloat",4,NULL,(grib_yyvsp[(5) - (6)].explist),(grib_yyvsp[(6) - (6)].lval),(grib_yyvsp[(2) - (6)].str),NULL);free((grib_yyvsp[(4) - (6)].str)); free((grib_yyvsp[(2) - (6)].str)); }
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 339 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (7)].str),"ibmfloat",4,(grib_yyvsp[(4) - (7)].explist),(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL,NULL);free((grib_yyvsp[(2) - (7)].str));  }
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 342 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (2)].str),"position",0,NULL,NULL,0,NULL,NULL);     free((grib_yyvsp[(2) - (2)].str));  }
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 345 "griby.y"
    { (grib_yyval.act) = grib_action_create_variable(grib_parser_context,(grib_yyvsp[(2) - (5)].str),"constant",0,(grib_yyvsp[(4) - (5)].explist),NULL,(grib_yyvsp[(5) - (5)].lval),NULL);free((grib_yyvsp[(2) - (5)].str)); }
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 348 "griby.y"
    { (grib_yyval.act) = grib_action_create_variable(grib_parser_context,(grib_yyvsp[(2) - (5)].str),"transient",0,(grib_yyvsp[(4) - (5)].explist),(grib_yyvsp[(4) - (5)].explist),(grib_yyvsp[(5) - (5)].lval),NULL);   free((grib_yyvsp[(2) - (5)].str)); }
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 351 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (4)].str),"ieeefloat",4,NULL,(grib_yyvsp[(3) - (4)].explist),(grib_yyvsp[(4) - (4)].lval),NULL,NULL);   free((grib_yyvsp[(2) - (4)].str));  }
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 354 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(4) - (6)].str),"ieeefloat",4,NULL,(grib_yyvsp[(5) - (6)].explist),(grib_yyvsp[(6) - (6)].lval),(grib_yyvsp[(2) - (6)].str),NULL);  free((grib_yyvsp[(4) - (6)].str));free((grib_yyvsp[(2) - (6)].str));}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 357 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (7)].str),"ieeefloat",4,(grib_yyvsp[(4) - (7)].explist),(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL,NULL);free((grib_yyvsp[(2) - (7)].str));  }
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 360 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (2)].str),"g1_half_byte_codeflag",0,NULL,NULL,0,NULL,NULL);free((grib_yyvsp[(2) - (2)].str));  }
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 363 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (5)].str),"section_length",(grib_yyvsp[(3) - (5)].lval),NULL,NULL,0,NULL,NULL);free((grib_yyvsp[(5) - (5)].str));  }
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 366 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (8)].str),"g1_message_length",(grib_yyvsp[(3) - (8)].lval),(grib_yyvsp[(7) - (8)].explist),NULL,0,NULL,NULL);free((grib_yyvsp[(5) - (8)].str));  }
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 369 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (8)].str),"g1_section4_length",(grib_yyvsp[(3) - (8)].lval),(grib_yyvsp[(7) - (8)].explist),NULL,0,NULL,NULL);free((grib_yyvsp[(5) - (8)].str));  }
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 372 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (3)].str),"ksec",0,(grib_yyvsp[(3) - (3)].explist),NULL,0,NULL,NULL);free((grib_yyvsp[(2) - (3)].str)); }
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 375 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (5)].str),"pad",0,(grib_yyvsp[(4) - (5)].explist),0,0,NULL,NULL);   free((grib_yyvsp[(2) - (5)].str)); }
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 378 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (5)].str),"padto",0,(grib_yyvsp[(4) - (5)].explist),0,0,NULL,NULL);   free((grib_yyvsp[(2) - (5)].str)); }
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 381 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (5)].str),"padtoeven",0,(grib_yyvsp[(4) - (5)].explist),0,0,NULL,NULL);   free((grib_yyvsp[(2) - (5)].str)); }
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 384 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (5)].str),"padtomultiple",0,(grib_yyvsp[(4) - (5)].explist),0,0,NULL,NULL);   free((grib_yyvsp[(2) - (5)].str)); }
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 387 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (6)].str),"message",(grib_yyvsp[(3) - (6)].lval),0,0,(grib_yyvsp[(6) - (6)].lval),NULL,NULL);   free((grib_yyvsp[(5) - (6)].str));  }
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 390 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (3)].str),"section_padding",0,0,0,(grib_yyvsp[(3) - (3)].lval),NULL,NULL);   free((grib_yyvsp[(2) - (3)].str));  }
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 392 "griby.y"
    { (grib_yyval.act) = grib_action_create_template(grib_parser_context,0,(grib_yyvsp[(2) - (3)].str),(grib_yyvsp[(3) - (3)].str)); free((grib_yyvsp[(2) - (3)].str)); free((grib_yyvsp[(3) - (3)].str));}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 394 "griby.y"
    { (grib_yyval.act) = grib_action_create_template(grib_parser_context,1,(grib_yyvsp[(2) - (3)].str),(grib_yyvsp[(3) - (3)].str)); free((grib_yyvsp[(2) - (3)].str)); free((grib_yyvsp[(3) - (3)].str));}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 397 "griby.y"
    { (grib_yyval.act) = grib_action_create_alias(grib_parser_context,(grib_yyvsp[(2) - (5)].str),(grib_yyvsp[(4) - (5)].str),NULL,(grib_yyvsp[(5) - (5)].lval));  free((grib_yyvsp[(2) - (5)].str)); free((grib_yyvsp[(4) - (5)].str)); }
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 400 "griby.y"
    { (grib_yyval.act) = grib_action_create_alias(grib_parser_context,(grib_yyvsp[(2) - (2)].str),NULL,NULL,0);  free((grib_yyvsp[(2) - (2)].str)); }
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 403 "griby.y"
    {
         (grib_yyval.act) = grib_action_create_alias(grib_parser_context,(grib_yyvsp[(4) - (7)].str),(grib_yyvsp[(6) - (7)].str),(grib_yyvsp[(2) - (7)].str),(grib_yyvsp[(7) - (7)].lval));  free((grib_yyvsp[(2) - (7)].str)); free((grib_yyvsp[(4) - (7)].str)); free((grib_yyvsp[(6) - (7)].str));
    }
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 407 "griby.y"
    {
         (grib_yyval.act) = grib_action_create_alias(grib_parser_context,(grib_yyvsp[(4) - (4)].str),NULL,(grib_yyvsp[(2) - (4)].str),0);  free((grib_yyvsp[(2) - (4)].str)); free((grib_yyvsp[(4) - (4)].str)); 
    }
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 411 "griby.y"
    { (grib_yyval.act) = grib_action_create_meta(grib_parser_context,(grib_yyvsp[(2) - (8)].str),(grib_yyvsp[(3) - (8)].str),(grib_yyvsp[(5) - (8)].explist),(grib_yyvsp[(7) - (8)].explist),(grib_yyvsp[(8) - (8)].lval),NULL); free((grib_yyvsp[(2) - (8)].str));free((grib_yyvsp[(3) - (8)].str));}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 414 "griby.y"
    { (grib_yyval.act) = grib_action_create_meta(grib_parser_context,(grib_yyvsp[(4) - (10)].str),(grib_yyvsp[(5) - (10)].str),(grib_yyvsp[(7) - (10)].explist),(grib_yyvsp[(9) - (10)].explist),(grib_yyvsp[(10) - (10)].lval),(grib_yyvsp[(2) - (10)].str)); free((grib_yyvsp[(4) - (10)].str));free((grib_yyvsp[(5) - (10)].str));free((grib_yyvsp[(2) - (10)].str));}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 417 "griby.y"
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
#line 429 "griby.y"
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
#line 441 "griby.y"
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
#line 453 "griby.y"
    { (grib_yyval.act) = grib_action_create_put(grib_parser_context,(grib_yyvsp[(2) - (5)].str),(grib_yyvsp[(4) - (5)].explist));free((grib_yyvsp[(2) - (5)].str));}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 456 "griby.y"
    { (grib_yyval.act) = grib_action_create_remove(grib_parser_context,(grib_yyvsp[(2) - (2)].explist));}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 459 "griby.y"
    { (grib_yyval.act) = grib_action_create_assert(grib_parser_context,(grib_yyvsp[(3) - (4)].exp));}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 462 "griby.y"
    { (grib_yyval.act) = grib_action_create_modify(grib_parser_context,(grib_yyvsp[(2) - (3)].str),(grib_yyvsp[(3) - (3)].lval)); free((grib_yyvsp[(2) - (3)].str));}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 464 "griby.y"
    { (grib_yyval.act) = grib_action_create_set_missing(grib_parser_context,(grib_yyvsp[(2) - (4)].str)); free((grib_yyvsp[(2) - (4)].str)); }
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 465 "griby.y"
    { (grib_yyval.act) = grib_action_create_set(grib_parser_context,(grib_yyvsp[(2) - (4)].str),(grib_yyvsp[(4) - (4)].exp),0); free((grib_yyvsp[(2) - (4)].str)); }
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 466 "griby.y"
    { (grib_yyval.act) = grib_action_create_set_darray(grib_parser_context,(grib_yyvsp[(2) - (6)].str),(grib_yyvsp[(5) - (6)].dvalue)); free((grib_yyvsp[(2) - (6)].str)); }
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 468 "griby.y"
    { (grib_yyval.act) = grib_action_create_set(grib_parser_context,(grib_yyvsp[(2) - (4)].str),(grib_yyvsp[(4) - (4)].exp),1); free((grib_yyvsp[(2) - (4)].str)); }
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 471 "griby.y"
    { (grib_yyval.act) = grib_action_create_write(grib_parser_context,(grib_yyvsp[(2) - (2)].str),0,0); free((grib_yyvsp[(2) - (2)].str));}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 472 "griby.y"
    { (grib_yyval.act) = grib_action_create_write(grib_parser_context,"",0,0); }
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 473 "griby.y"
    { (grib_yyval.act) = grib_action_create_write(grib_parser_context,(grib_yyvsp[(5) - (5)].str),0,(grib_yyvsp[(3) - (5)].lval)); free((grib_yyvsp[(5) - (5)].str));}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 474 "griby.y"
    { (grib_yyval.act) = grib_action_create_write(grib_parser_context,"",0,(grib_yyvsp[(3) - (4)].lval)); }
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 475 "griby.y"
    { (grib_yyval.act) = grib_action_create_write(grib_parser_context,(grib_yyvsp[(2) - (2)].str),1,0); free((grib_yyvsp[(2) - (2)].str));}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 476 "griby.y"
    { (grib_yyval.act) = grib_action_create_write(grib_parser_context,"",1,0); }
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 477 "griby.y"
    { (grib_yyval.act) = grib_action_create_write(grib_parser_context,(grib_yyvsp[(5) - (5)].str),1,(grib_yyvsp[(3) - (5)].lval)); free((grib_yyvsp[(5) - (5)].str));}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 478 "griby.y"
    { (grib_yyval.act) = grib_action_create_write(grib_parser_context,"",1,(grib_yyvsp[(3) - (4)].lval)); }
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 480 "griby.y"
    { (grib_yyval.act) = grib_action_create_print(grib_parser_context,(grib_yyvsp[(2) - (2)].str),0); free((grib_yyvsp[(2) - (2)].str)); }
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 481 "griby.y"
    { (grib_yyval.act) = grib_action_create_print(grib_parser_context,(grib_yyvsp[(5) - (5)].str),(grib_yyvsp[(3) - (5)].str)); free((grib_yyvsp[(5) - (5)].str)); free((grib_yyvsp[(3) - (5)].str));}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 482 "griby.y"
    { (grib_yyval.act) = grib_action_create_print(grib_parser_context,"",0);  }
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 486 "griby.y"
    { (grib_yyval.act) = grib_action_create_if(grib_parser_context,(grib_yyvsp[(3) - (7)].exp),(grib_yyvsp[(6) - (7)].act),0,0); }
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 487 "griby.y"
    { (grib_yyval.act) = grib_action_create_if(grib_parser_context,(grib_yyvsp[(3) - (11)].exp),(grib_yyvsp[(6) - (11)].act),(grib_yyvsp[(10) - (11)].act),0); }
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 488 "griby.y"
    { (grib_yyval.act) = grib_action_create_if(grib_parser_context,(grib_yyvsp[(3) - (7)].exp),(grib_yyvsp[(6) - (7)].act),0,1); }
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 489 "griby.y"
    { (grib_yyval.act) = grib_action_create_if(grib_parser_context,(grib_yyvsp[(3) - (11)].exp),(grib_yyvsp[(6) - (11)].act),(grib_yyvsp[(10) - (11)].act),1); }
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 493 "griby.y"
    { (grib_yyval.act) = grib_action_create_when(grib_parser_context,(grib_yyvsp[(3) - (6)].exp),(grib_yyvsp[(5) - (6)].act),NULL); }
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 494 "griby.y"
    { (grib_yyval.act) = grib_action_create_when(grib_parser_context,(grib_yyvsp[(3) - (7)].exp),(grib_yyvsp[(6) - (7)].act),NULL); }
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 495 "griby.y"
    { (grib_yyval.act) = grib_action_create_when(grib_parser_context,(grib_yyvsp[(3) - (11)].exp),(grib_yyvsp[(6) - (11)].act),(grib_yyvsp[(10) - (11)].act)); }
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 498 "griby.y"
    { (grib_yyval.act) = grib_action_create_set(grib_parser_context,(grib_yyvsp[(2) - (4)].str),(grib_yyvsp[(4) - (4)].exp),0); free((grib_yyvsp[(2) - (4)].str)); }
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 499 "griby.y"
    { (grib_yyval.act) = grib_action_create_set(grib_parser_context,(grib_yyvsp[(2) - (4)].str),(grib_yyvsp[(4) - (4)].exp),1); free((grib_yyvsp[(2) - (4)].str)); }
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 503 "griby.y"
    { (grib_yyvsp[(1) - (3)].act)->next = (grib_yyvsp[(2) - (3)].act); (grib_yyval.act) = (grib_yyvsp[(1) - (3)].act); }
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 507 "griby.y"
    { (grib_yyval.explist) = NULL ;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 508 "griby.y"
    { (grib_yyval.explist) = (grib_yyvsp[(2) - (2)].explist) ;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 511 "griby.y"
    { (grib_yyval.lval) = 0 ; }
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 512 "griby.y"
    { (grib_yyval.lval) = (grib_yyvsp[(2) - (2)].lval); }
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 516 "griby.y"
    { (grib_yyval.lval) = (grib_yyvsp[(1) - (3)].lval) | (grib_yyvsp[(3) - (3)].lval); }
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 519 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_READ_ONLY; }
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 520 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_LOWERCASE; }
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 521 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_DUMP; }
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 522 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_NO_COPY; }
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 523 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_NO_FAIL; }
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 524 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_HIDDEN; }
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 525 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_EDITION_SPECIFIC; }
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 526 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_CAN_BE_MISSING; }
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 527 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_CONSTRAINT; }
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 528 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_OVERRIDE; }
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 529 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_COPY_OK; }
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 530 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_TRANSIENT; }
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 531 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_STRING_TYPE; }
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 532 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_LONG_TYPE; }
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 535 "griby.y"
    { (grib_yyval.act) = grib_action_create_list(grib_parser_context,(grib_yyvsp[(1) - (8)].str),(grib_yyvsp[(4) - (8)].exp),(grib_yyvsp[(7) - (8)].act)); free((grib_yyvsp[(1) - (8)].str)); }
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 538 "griby.y"
    { (grib_yyval.act) = grib_action_create_while(grib_parser_context,(grib_yyvsp[(3) - (7)].exp),(grib_yyvsp[(6) - (7)].act));  }
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 541 "griby.y"
    { (grib_yyval.act) = grib_action_create_trigger(grib_parser_context,(grib_yyvsp[(3) - (7)].explist),(grib_yyvsp[(6) - (7)].act));  }
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 544 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(2) - (6)].str),(grib_yyvsp[(4) - (6)].concept_value),0,0,0,0,0,0,(grib_yyvsp[(6) - (6)].lval),0);  free((grib_yyvsp[(2) - (6)].str)); }
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 545 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(2) - (9)].str),(grib_yyvsp[(7) - (9)].concept_value),0,0,(grib_yyvsp[(4) - (9)].str),0,0,0,(grib_yyvsp[(9) - (9)].lval),0);  free((grib_yyvsp[(2) - (9)].str));free((grib_yyvsp[(4) - (9)].str)); }
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 546 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(2) - (12)].str),0,(grib_yyvsp[(6) - (12)].str),0,(grib_yyvsp[(4) - (12)].str),(grib_yyvsp[(8) - (12)].str),(grib_yyvsp[(10) - (12)].str),0,(grib_yyvsp[(12) - (12)].lval),0);  free((grib_yyvsp[(2) - (12)].str));free((grib_yyvsp[(6) - (12)].str));free((grib_yyvsp[(4) - (12)].str));free((grib_yyvsp[(8) - (12)].str));free((grib_yyvsp[(10) - (12)].str)); }
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 547 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(2) - (14)].str),0,(grib_yyvsp[(6) - (14)].str),0,(grib_yyvsp[(4) - (14)].str),(grib_yyvsp[(8) - (14)].str),(grib_yyvsp[(10) - (14)].str),(grib_yyvsp[(12) - (14)].str),(grib_yyvsp[(14) - (14)].lval),0);  free((grib_yyvsp[(2) - (14)].str));free((grib_yyvsp[(6) - (14)].str));free((grib_yyvsp[(4) - (14)].str));free((grib_yyvsp[(8) - (14)].str));free((grib_yyvsp[(10) - (14)].str));free((grib_yyvsp[(12) - (14)].str)); }
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 548 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(2) - (10)].str),0,(grib_yyvsp[(6) - (10)].str),0,(grib_yyvsp[(4) - (10)].str),(grib_yyvsp[(8) - (10)].str),0,0,(grib_yyvsp[(10) - (10)].lval),0);  free((grib_yyvsp[(2) - (10)].str));free((grib_yyvsp[(6) - (10)].str));free((grib_yyvsp[(4) - (10)].str));free((grib_yyvsp[(8) - (10)].str)); }
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 549 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(4) - (14)].str),0,(grib_yyvsp[(8) - (14)].str),(grib_yyvsp[(2) - (14)].str),(grib_yyvsp[(6) - (14)].str),(grib_yyvsp[(10) - (14)].str),(grib_yyvsp[(12) - (14)].str),0,(grib_yyvsp[(14) - (14)].lval),0);  free((grib_yyvsp[(4) - (14)].str));free((grib_yyvsp[(8) - (14)].str));free((grib_yyvsp[(6) - (14)].str));free((grib_yyvsp[(10) - (14)].str)); free((grib_yyvsp[(2) - (14)].str));}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 550 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(4) - (12)].str),0,(grib_yyvsp[(8) - (12)].str),(grib_yyvsp[(2) - (12)].str),(grib_yyvsp[(6) - (12)].str),(grib_yyvsp[(10) - (12)].str),0,0,(grib_yyvsp[(12) - (12)].lval),0);  free((grib_yyvsp[(4) - (12)].str));free((grib_yyvsp[(8) - (12)].str));free((grib_yyvsp[(6) - (12)].str));free((grib_yyvsp[(10) - (12)].str)); free((grib_yyvsp[(2) - (12)].str));}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 551 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(4) - (8)].str),(grib_yyvsp[(6) - (8)].concept_value),0,(grib_yyvsp[(2) - (8)].str),0,0,0,0,(grib_yyvsp[(8) - (8)].lval),0);  free((grib_yyvsp[(2) - (8)].str));free((grib_yyvsp[(4) - (8)].str)); }
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 552 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(4) - (11)].str),(grib_yyvsp[(9) - (11)].concept_value),0,(grib_yyvsp[(2) - (11)].str),(grib_yyvsp[(6) - (11)].str),0,0,0,(grib_yyvsp[(11) - (11)].lval),0);  free((grib_yyvsp[(2) - (11)].str));free((grib_yyvsp[(4) - (11)].str));free((grib_yyvsp[(6) - (11)].str)); }
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 553 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(2) - (6)].str),(grib_yyvsp[(4) - (6)].concept_value),0,0,0,0,0,0,(grib_yyvsp[(6) - (6)].lval),1);  free((grib_yyvsp[(2) - (6)].str)); }
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 554 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(2) - (9)].str),(grib_yyvsp[(7) - (9)].concept_value),0,0,(grib_yyvsp[(4) - (9)].str),0,0,0,(grib_yyvsp[(9) - (9)].lval),1);  free((grib_yyvsp[(2) - (9)].str));free((grib_yyvsp[(4) - (9)].str)); }
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 555 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(2) - (12)].str),0,(grib_yyvsp[(6) - (12)].str),0,(grib_yyvsp[(4) - (12)].str),(grib_yyvsp[(8) - (12)].str),(grib_yyvsp[(10) - (12)].str),0,(grib_yyvsp[(12) - (12)].lval),1);  free((grib_yyvsp[(2) - (12)].str));free((grib_yyvsp[(6) - (12)].str));free((grib_yyvsp[(4) - (12)].str));free((grib_yyvsp[(8) - (12)].str));free((grib_yyvsp[(10) - (12)].str)); }
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 556 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(2) - (10)].str),0,(grib_yyvsp[(6) - (10)].str),0,(grib_yyvsp[(4) - (10)].str),(grib_yyvsp[(8) - (10)].str),0,0,(grib_yyvsp[(10) - (10)].lval),1);  free((grib_yyvsp[(2) - (10)].str));free((grib_yyvsp[(6) - (10)].str));free((grib_yyvsp[(4) - (10)].str));free((grib_yyvsp[(8) - (10)].str)); }
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 557 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(4) - (14)].str),0,(grib_yyvsp[(8) - (14)].str),(grib_yyvsp[(2) - (14)].str),(grib_yyvsp[(6) - (14)].str),(grib_yyvsp[(10) - (14)].str),(grib_yyvsp[(12) - (14)].str),0,(grib_yyvsp[(14) - (14)].lval),1);  free((grib_yyvsp[(4) - (14)].str));free((grib_yyvsp[(8) - (14)].str));free((grib_yyvsp[(6) - (14)].str));free((grib_yyvsp[(10) - (14)].str));free((grib_yyvsp[(12) - (14)].str)); free((grib_yyvsp[(2) - (14)].str));}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 558 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(4) - (12)].str),0,(grib_yyvsp[(8) - (12)].str),(grib_yyvsp[(2) - (12)].str),(grib_yyvsp[(6) - (12)].str),(grib_yyvsp[(10) - (12)].str),0,0,(grib_yyvsp[(12) - (12)].lval),1);  free((grib_yyvsp[(4) - (12)].str));free((grib_yyvsp[(8) - (12)].str));free((grib_yyvsp[(6) - (12)].str));free((grib_yyvsp[(10) - (12)].str)); free((grib_yyvsp[(2) - (12)].str));}
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 559 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(4) - (8)].str),(grib_yyvsp[(6) - (8)].concept_value),0,(grib_yyvsp[(2) - (8)].str),0,0,0,0,(grib_yyvsp[(8) - (8)].lval),1);  free((grib_yyvsp[(2) - (8)].str));free((grib_yyvsp[(4) - (8)].str)); }
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 560 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(4) - (11)].str),(grib_yyvsp[(9) - (11)].concept_value),0,(grib_yyvsp[(2) - (11)].str),(grib_yyvsp[(6) - (11)].str),0,0,0,(grib_yyvsp[(11) - (11)].lval),1);  free((grib_yyvsp[(2) - (11)].str));free((grib_yyvsp[(4) - (11)].str));free((grib_yyvsp[(6) - (11)].str)); }
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 565 "griby.y"
    { (grib_yyval.concept_value) = (grib_yyvsp[(2) - (2)].concept_value); (grib_yyvsp[(2) - (2)].concept_value)->next = (grib_yyvsp[(1) - (2)].concept_value);   }
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 569 "griby.y"
    { (grib_yyval.case_value) = (grib_yyvsp[(2) - (2)].case_value); (grib_yyvsp[(2) - (2)].case_value)->next = (grib_yyvsp[(1) - (2)].case_value);   }
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 572 "griby.y"
    { (grib_yyval.case_value) = grib_case_new(grib_parser_context,(grib_yyvsp[(2) - (4)].explist),(grib_yyvsp[(4) - (4)].act));  }
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 576 "griby.y"
    { (grib_yyval.act) = grib_action_create_switch(grib_parser_context,(grib_yyvsp[(3) - (10)].explist),(grib_yyvsp[(6) - (10)].case_value),(grib_yyvsp[(9) - (10)].act)); }
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 577 "griby.y"
    { (grib_yyval.act) = grib_action_create_switch(grib_parser_context,(grib_yyvsp[(3) - (9)].explist),(grib_yyvsp[(6) - (9)].case_value),grib_action_create_noop(grib_parser_context,"continue")); }
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 578 "griby.y"
    { (grib_yyval.act) = grib_action_create_switch(grib_parser_context,(grib_yyvsp[(3) - (7)].explist),(grib_yyvsp[(6) - (7)].case_value),0); }
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 581 "griby.y"
    {
	  				(grib_yyval.concept_value) = grib_concept_value_new(grib_parser_context,(grib_yyvsp[(1) - (5)].str),(grib_yyvsp[(4) - (5)].concept_condition)); free((grib_yyvsp[(1) - (5)].str));}
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
					char buf[80]; sprintf(buf,"%ld",(long)(grib_yyvsp[(1) - (5)].lval)); (grib_yyval.concept_value) = grib_concept_value_new(grib_parser_context,buf,(grib_yyvsp[(4) - (5)].concept_condition));}
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 587 "griby.y"
    {
					char buf[80]; sprintf(buf,"%g",(double)(grib_yyvsp[(1) - (5)].dval)); (grib_yyval.concept_value) = grib_concept_value_new(grib_parser_context,buf,(grib_yyvsp[(4) - (5)].concept_condition));}
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 592 "griby.y"
    { (grib_yyvsp[(1) - (2)].concept_condition)->next = (grib_yyvsp[(2) - (2)].concept_condition); (grib_yyval.concept_condition) = (grib_yyvsp[(1) - (2)].concept_condition); }
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 595 "griby.y"
    { (grib_yyval.concept_condition) = grib_concept_condition_new(grib_parser_context,(grib_yyvsp[(1) - (4)].str),(grib_yyvsp[(3) - (4)].exp)); free((grib_yyvsp[(1) - (4)].str)); }
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 598 "griby.y"
    { (grib_yyval.exp) = new_accessor_expression(grib_parser_context,(grib_yyvsp[(1) - (1)].str)); free((grib_yyvsp[(1) - (1)].str)); }
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 599 "griby.y"
    { (grib_yyval.exp) = new_string_expression(grib_parser_context,(grib_yyvsp[(1) - (1)].str));  free((grib_yyvsp[(1) - (1)].str)); }
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 603 "griby.y"
    { (grib_yyval.exp) = new_long_expression(grib_parser_context,(grib_yyvsp[(1) - (1)].lval));  }
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 604 "griby.y"
    { (grib_yyval.exp) = new_double_expression(grib_parser_context,(grib_yyvsp[(1) - (1)].dval));  /* TODO: change to new_float_expression*/}
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 606 "griby.y"
    { (grib_yyval.exp) = NULL; }
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 607 "griby.y"
    { (grib_yyval.exp) = new_true_expression(grib_parser_context); }
    break;

  case 178:

/* Line 1455 of yacc.c  */
#line 608 "griby.y"
    { (grib_yyval.exp) = (grib_yyvsp[(2) - (3)].exp); }
    break;

  case 179:

/* Line 1455 of yacc.c  */
#line 609 "griby.y"
    { (grib_yyval.exp) = new_unop_expression(grib_parser_context,&grib_op_neg,&grib_op_neg_d,(grib_yyvsp[(2) - (2)].exp)); }
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 610 "griby.y"
    { (grib_yyval.exp) = new_func_expression(grib_parser_context,(grib_yyvsp[(1) - (3)].str),NULL); free((grib_yyvsp[(1) - (3)].str));}
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 611 "griby.y"
    { (grib_yyval.exp) = new_func_expression(grib_parser_context,(grib_yyvsp[(1) - (4)].str),(grib_yyvsp[(3) - (4)].explist)); free((grib_yyvsp[(1) - (4)].str));}
    break;

  case 182:

/* Line 1455 of yacc.c  */
#line 615 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_pow,NULL,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 184:

/* Line 1455 of yacc.c  */
#line 619 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_mul,&grib_op_mul_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 620 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_div,&grib_op_div_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 186:

/* Line 1455 of yacc.c  */
#line 621 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_modulo,NULL,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 187:

/* Line 1455 of yacc.c  */
#line 622 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_bit,NULL,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 623 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_bitoff,NULL,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 190:

/* Line 1455 of yacc.c  */
#line 627 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_add,&grib_op_add_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 628 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_sub,&grib_op_sub_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 632 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_gt,&grib_op_gt_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 634 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_eq,&grib_op_eq_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 635 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_lt,&grib_op_lt_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 636 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_ge,&grib_op_ge_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 637 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_le,&grib_op_le_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 638 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_ne,&grib_op_ne_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 639 "griby.y"
    { (grib_yyval.exp) = new_string_compare_expression(grib_parser_context,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 200:

/* Line 1455 of yacc.c  */
#line 644 "griby.y"
    { (grib_yyval.exp) = new_unop_expression(grib_parser_context,&grib_op_not,NULL,(grib_yyvsp[(2) - (2)].exp)); }
    break;

  case 202:

/* Line 1455 of yacc.c  */
#line 648 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_and,NULL,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 204:

/* Line 1455 of yacc.c  */
#line 652 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_or,NULL,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp));}
    break;

  case 209:

/* Line 1455 of yacc.c  */
#line 666 "griby.y"
    { (grib_yyval.rule_entry) = grib_new_rule_entry(grib_parser_context,(grib_yyvsp[(1) - (4)].str),(grib_yyvsp[(3) - (4)].exp)); free((grib_yyvsp[(1) - (4)].str)); }
    break;

  case 210:

/* Line 1455 of yacc.c  */
#line 667 "griby.y"
    { (grib_yyval.rule_entry) = grib_new_rule_entry(grib_parser_context,"skip",0);}
    break;

  case 212:

/* Line 1455 of yacc.c  */
#line 671 "griby.y"
    { (grib_yyvsp[(1) - (2)].rule_entry)->next = (grib_yyvsp[(2) - (2)].rule_entry); (grib_yyval.rule_entry) = (grib_yyvsp[(1) - (2)].rule_entry); }
    break;

  case 213:

/* Line 1455 of yacc.c  */
#line 674 "griby.y"
    { (grib_yyval.rules) = grib_new_rule(grib_parser_context,NULL,(grib_yyvsp[(1) - (1)].rule_entry)); }
    break;

  case 214:

/* Line 1455 of yacc.c  */
#line 678 "griby.y"
    { (grib_yyval.rules) = grib_new_rule(grib_parser_context,(grib_yyvsp[(3) - (7)].exp),(grib_yyvsp[(6) - (7)].rule_entry)); }
    break;

  case 216:

/* Line 1455 of yacc.c  */
#line 682 "griby.y"
    { (grib_yyvsp[(1) - (2)].rules)->next = (grib_yyvsp[(2) - (2)].rules); (grib_yyval.rules) = (grib_yyvsp[(1) - (2)].rules); }
    break;



/* Line 1455 of yacc.c  */
#line 3581 "y.tab.c"
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
#line 686 "griby.y"


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





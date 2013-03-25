
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
#line 10 "griby.y"


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

/* Line 214 of yacc.c  */
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



/* Line 214 of yacc.c  */
#line 328 "y.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define grib_yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 340 "y.tab.c"

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
#define YYLAST   1355

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  109
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  45
/* YYNRULES -- Number of rules.  */
#define YYNRULES  215
/* YYNRULES -- Number of states.  */
#define YYNSTATES  680

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   344

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? grib_yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const grib_yytype_uint8 grib_yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,   105,     2,     2,
      94,    95,   103,   106,    90,   101,    96,   104,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   100,    91,
     108,    97,   107,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    92,     2,    93,   102,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    98,     2,    99,     2,     2,     2,     2,
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
      85,    86,    87,    88,    89
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
     153,   161,   172,   180,   191,   200,   213,   224,   233,   244,
     253,   263,   271,   274,   277,   282,   289,   297,   300,   306,
     312,   317,   324,   332,   335,   341,   350,   359,   363,   369,
     375,   381,   387,   394,   398,   402,   406,   412,   415,   423,
     428,   437,   448,   454,   460,   466,   472,   475,   480,   484,
     489,   494,   501,   506,   509,   511,   517,   522,   525,   527,
     533,   538,   541,   547,   549,   557,   569,   577,   589,   596,
     604,   616,   621,   626,   629,   633,   635,   638,   640,   643,
     645,   649,   651,   653,   655,   657,   659,   661,   663,   665,
     667,   669,   671,   673,   675,   677,   686,   694,   702,   709,
     719,   732,   747,   758,   773,   786,   795,   807,   814,   824,
     837,   848,   863,   876,   885,   897,   899,   902,   904,   907,
     912,   923,   933,   941,   947,   953,   959,   965,   967,   970,
     975,   977,   979,   981,   983,   985,   987,   989,   993,   996,
    1000,  1005,  1009,  1011,  1015,  1019,  1023,  1027,  1031,  1033,
    1037,  1041,  1043,  1047,  1051,  1055,  1059,  1063,  1067,  1071,
    1074,  1076,  1080,  1082,  1086,  1088,  1090,  1092,  1094,  1099,
    1102,  1104,  1107,  1109,  1117,  1119
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const grib_yytype_int16 grib_yyrhs[] =
{
     110,     0,    -1,   111,    -1,   132,    -1,   113,    -1,   153,
      -1,     1,    -1,    -1,    89,    -1,   112,    90,    89,    -1,
      88,    -1,   112,    90,    88,    -1,   114,    -1,   114,   113,
      -1,   114,    91,   113,    -1,   114,    91,    -1,   119,    91,
      -1,   120,    -1,   128,    -1,   129,    -1,   130,    -1,   131,
      -1,   121,    -1,   135,    -1,    91,    -1,   115,    91,    -1,
     111,    -1,   117,    -1,   118,    -1,   118,    90,   117,    -1,
     147,    -1,     8,    92,    88,    93,    86,   124,   125,    -1,
       8,    92,    88,    93,    86,    92,   116,    93,   124,   125,
      -1,     8,    94,    88,    95,    86,   124,   125,    -1,     8,
      94,    88,    95,    86,    92,   116,    93,   124,   125,    -1,
      12,    92,    88,    93,    86,   124,   125,    -1,    12,    92,
      88,    93,    87,   124,   125,    -1,    19,    92,    88,    93,
      86,   124,   125,    -1,    19,    92,    88,    93,    86,    92,
     116,    93,   124,   125,    -1,    13,    92,    88,    93,    86,
     124,   125,    -1,    18,    92,    88,    93,    86,   124,   125,
      -1,    18,    92,    88,    93,    86,    92,   116,    93,   124,
     125,    -1,    18,    94,    88,    95,    86,   124,   125,    -1,
      18,    94,    88,    95,    86,    92,   116,    93,   124,   125,
      -1,    20,    92,    88,    93,    86,   118,   124,   125,    -1,
      20,    92,    88,    93,    86,   118,   124,    49,    94,    86,
      95,   125,    -1,    20,    92,    88,    93,    86,    94,   116,
      95,   124,   125,    -1,    21,    92,    88,    93,    86,   118,
     124,   125,    -1,    21,    92,    88,    93,    86,    94,   116,
      95,   124,   125,    -1,    67,    92,    88,    93,    86,   118,
     124,   125,    -1,    22,    92,    88,    93,    86,    94,   116,
      95,   125,    -1,    29,    86,    94,   116,    95,   124,   125,
      -1,    14,    86,    -1,    14,    87,    -1,    17,    86,   124,
     125,    -1,    17,    86,    96,    86,   124,   125,    -1,    17,
      86,    92,   118,    93,   124,   125,    -1,    26,    86,    -1,
      27,    86,    97,   118,   125,    -1,    28,    86,    97,   118,
     125,    -1,    89,    86,   124,   125,    -1,    89,    86,    96,
      86,   124,   125,    -1,    89,    86,    92,   118,    93,   124,
     125,    -1,    63,    86,    -1,    66,    92,    88,    93,    86,
      -1,    64,    92,    88,    93,    86,    94,   116,    95,    -1,
      65,    92,    88,    93,    86,    94,   116,    95,    -1,    71,
      86,   118,    -1,    57,    86,    94,   116,    95,    -1,    60,
      86,    94,   116,    95,    -1,    61,    86,    94,   116,    95,
      -1,    62,    86,    94,   116,    95,    -1,    59,    92,    88,
      93,    86,   125,    -1,    58,    86,   125,    -1,     9,    86,
      87,    -1,    10,    86,    87,    -1,    23,    86,    97,    86,
     125,    -1,    24,    86,    -1,    23,    86,    96,    86,    97,
      86,   125,    -1,    24,    86,    96,    86,    -1,    25,    86,
      86,    94,   116,    95,   124,   125,    -1,    25,    86,    96,
      86,    86,    94,   116,    95,   124,   125,    -1,    68,    86,
      94,   116,    95,    -1,    69,    86,    94,   116,    95,    -1,
      70,    86,    94,   116,    95,    -1,    54,    86,    94,   116,
      95,    -1,    55,   116,    -1,    72,    94,   147,    95,    -1,
      34,    86,   125,    -1,    49,    86,    97,    45,    -1,    49,
      86,    97,   147,    -1,    49,    86,    97,    98,   112,    99,
      -1,    50,    86,    97,   147,    -1,    51,    87,    -1,    51,
      -1,    51,    94,    88,    95,    87,    -1,    51,    94,    88,
      95,    -1,    52,    87,    -1,    52,    -1,    52,    94,    88,
      95,    87,    -1,    52,    94,    88,    95,    -1,    53,    87,
      -1,    53,    94,    87,    95,    87,    -1,    53,    -1,     4,
      94,   147,    95,    98,   113,    99,    -1,     4,    94,   147,
      95,    98,   113,    99,     6,    98,   113,    99,    -1,     5,
      94,   147,    95,    98,   113,    99,    -1,     5,    94,   147,
      95,    98,   113,    99,     6,    98,   113,    99,    -1,    48,
      94,   147,    95,   122,   115,    -1,    48,    94,   147,    95,
      98,   123,    99,    -1,    48,    94,   147,    95,    98,   123,
      99,     6,    98,   123,    99,    -1,    49,    86,    97,   147,
      -1,    50,    86,    97,   147,    -1,   122,   115,    -1,   123,
     122,   115,    -1,   111,    -1,    97,   116,    -1,   111,    -1,
     100,   126,    -1,   127,    -1,   126,    90,   127,    -1,    35,
      -1,     3,    -1,    39,    -1,    38,    -1,    40,    -1,    43,
      -1,    41,    -1,    44,    -1,    46,    -1,    42,    -1,    47,
      -1,    28,    -1,    36,    -1,    37,    -1,    86,    15,    94,
     147,    95,    98,   113,    99,    -1,    16,    94,   147,    95,
      98,   113,    99,    -1,    11,    94,   116,    95,    98,   113,
      99,    -1,    30,    86,    98,   132,    99,   125,    -1,    30,
      86,    94,    86,    95,    98,   132,    99,   125,    -1,    30,
      86,    94,    86,    90,    87,    90,    86,    90,    86,    95,
     125,    -1,    30,    86,    94,    86,    90,    87,    90,    86,
      90,    86,    90,    86,    95,   125,    -1,    30,    86,    94,
      86,    90,    87,    90,    86,    95,   125,    -1,    30,    86,
      96,    86,    94,    86,    90,    87,    90,    86,    90,    86,
      95,   125,    -1,    30,    86,    96,    86,    94,    86,    90,
      87,    90,    86,    95,   125,    -1,    30,    86,    96,    86,
      98,   132,    99,   125,    -1,    30,    86,    96,    86,    94,
      86,    95,    98,   132,    99,   125,    -1,    31,    86,    98,
     132,    99,   125,    -1,    31,    86,    94,    86,    95,    98,
     132,    99,   125,    -1,    31,    86,    94,    86,    90,    87,
      90,    86,    90,    86,    95,   125,    -1,    31,    86,    94,
      86,    90,    87,    90,    86,    95,   125,    -1,    31,    86,
      96,    86,    94,    86,    90,    87,    90,    86,    90,    86,
      95,   125,    -1,    31,    86,    96,    86,    94,    86,    90,
      87,    90,    86,    95,   125,    -1,    31,    86,    96,    86,
      98,   132,    99,   125,    -1,    31,    86,    96,    86,    94,
      86,    95,    98,   132,    99,   125,    -1,   136,    -1,   132,
     136,    -1,   134,    -1,   133,   134,    -1,    73,   117,   100,
     113,    -1,    74,    94,   116,    95,    98,   133,    75,   100,
     113,    99,    -1,    74,    94,   116,    95,    98,   133,    75,
     100,    99,    -1,    74,    94,   116,    95,    98,   133,    99,
      -1,    87,    97,    98,   137,    99,    -1,    86,    97,    98,
     137,    99,    -1,    88,    97,    98,   137,    99,    -1,    89,
      97,    98,   137,    99,    -1,   138,    -1,   138,   137,    -1,
      86,    97,   147,    91,    -1,    86,    -1,    87,    -1,   139,
      -1,    88,    -1,    89,    -1,    32,    -1,    33,    -1,    94,
     147,    95,    -1,   101,   140,    -1,    86,    94,    95,    -1,
      86,    94,   116,    95,    -1,   140,   102,   141,    -1,   140,
      -1,   142,   103,   141,    -1,   142,   104,   141,    -1,   142,
     105,   141,    -1,   142,    80,   141,    -1,   142,    81,   141,
      -1,   141,    -1,   143,   106,   142,    -1,   143,   101,   142,
      -1,   142,    -1,   144,   107,   143,    -1,   144,    76,   143,
      -1,   144,   108,   143,    -1,   144,    78,   143,    -1,   144,
      79,   143,    -1,   144,    77,   143,    -1,   139,    85,   139,
      -1,    84,   144,    -1,   143,    -1,   145,    82,   144,    -1,
     144,    -1,   146,    83,   145,    -1,   145,    -1,   146,    -1,
     151,    -1,   152,    -1,    86,    97,   147,    91,    -1,    56,
      91,    -1,   149,    -1,   149,   150,    -1,   149,    -1,     4,
      94,   147,    95,    98,   150,    99,    -1,   148,    -1,   148,
     153,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const grib_yytype_uint16 grib_yyrline[] =
{
       0,   204,   204,   205,   206,   207,   209,   212,   215,   216,
     217,   218,   220,   221,   222,   223,   226,   227,   228,   229,
     230,   231,   232,   233,   237,   238,   241,   242,   245,   246,
     249,   253,   256,   259,   262,   265,   269,   272,   275,   278,
     281,   284,   287,   290,   293,   296,   300,   303,   306,   309,
     312,   315,   318,   321,   324,   327,   330,   333,   336,   339,
     342,   345,   348,   351,   354,   357,   360,   363,   366,   369,
     372,   375,   378,   381,   383,   385,   388,   391,   394,   398,
     402,   405,   408,   420,   432,   444,   447,   450,   453,   456,
     457,   458,   460,   463,   464,   465,   466,   467,   468,   469,
     470,   472,   473,   474,   478,   479,   480,   481,   485,   486,
     487,   490,   491,   494,   495,   499,   500,   503,   504,   507,
     508,   511,   512,   513,   514,   515,   516,   517,   518,   519,
     520,   521,   522,   523,   524,   527,   530,   533,   536,   537,
     538,   539,   540,   541,   542,   543,   544,   545,   546,   547,
     548,   549,   550,   551,   552,   556,   557,   560,   561,   564,
     568,   569,   570,   573,   575,   577,   579,   583,   584,   587,
     590,   591,   594,   595,   596,   598,   599,   600,   601,   602,
     603,   607,   608,   611,   612,   613,   614,   615,   616,   619,
     620,   621,   624,   626,   627,   628,   629,   630,   631,   636,
     637,   640,   641,   644,   645,   648,   654,   655,   658,   659,
     662,   663,   666,   670,   673,   674
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
  "META", "POS", "INTCONST", "TRANS", "FLAGBIT", "CONCEPT",
  "CONCEPT_NOFAIL", "NIL", "DUMMY", "MODIFY", "READ_ONLY", "STRING_TYPE",
  "LONG_TYPE", "NO_COPY", "DUMP", "NO_FAIL", "EDITION_SPECIFIC",
  "OVERRIDE", "HIDDEN", "CAN_BE_MISSING", "MISSING", "CONSTRAINT",
  "COPY_OK", "WHEN", "SET", "SET_NOFAIL", "WRITE", "APPEND", "PRINT",
  "EXPORT", "REMOVE", "SKIP", "PAD", "SECTION_PADDING", "MESSAGE", "PADTO",
  "PADTOEVEN", "PADTOMULTIPLE", "G1_HALF_BYTE", "G1_MESSAGE_LENGTH",
  "G1_SECTION4_LENGTH", "SECTION_LENGTH", "FLAG", "ITERATOR", "NEAREST",
  "BOX", "KSEC", "ASSERT", "CASE", "SWITCH", "DEFAULT", "EQ", "NE", "GE",
  "LE", "BIT", "BITOFF", "AND", "OR", "NOT", "IS", "IDENT", "STRING",
  "INTEGER", "FLOAT", "','", "';'", "'['", "']'", "'('", "')'", "'.'",
  "'='", "'{'", "'}'", "':'", "'-'", "'^'", "'*'", "'/'", "'%'", "'+'",
  "'>'", "'<'", "$accept", "all", "empty", "dvalues", "instructions",
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
      44,    59,    91,    93,    40,    41,    46,    61,   123,   125,
      58,    45,    94,    42,    47,    37,    43,    62,    60
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const grib_yytype_uint8 grib_yyr1[] =
{
       0,   109,   110,   110,   110,   110,   110,   111,   112,   112,
     112,   112,   113,   113,   113,   113,   114,   114,   114,   114,
     114,   114,   114,   114,   115,   115,   116,   116,   117,   117,
     118,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   120,   120,   120,   120,   121,   121,
     121,   122,   122,   123,   123,   124,   124,   125,   125,   126,
     126,   127,   127,   127,   127,   127,   127,   127,   127,   127,
     127,   127,   127,   127,   127,   128,   129,   130,   131,   131,
     131,   131,   131,   131,   131,   131,   131,   131,   131,   131,
     131,   131,   131,   131,   131,   132,   132,   133,   133,   134,
     135,   135,   135,   136,   136,   136,   136,   137,   137,   138,
     139,   139,   140,   140,   140,   140,   140,   140,   140,   140,
     140,   141,   141,   142,   142,   142,   142,   142,   142,   143,
     143,   143,   144,   144,   144,   144,   144,   144,   144,   144,
     144,   145,   145,   146,   146,   147,   148,   148,   149,   149,
     150,   150,   151,   152,   153,   153
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const grib_yytype_uint8 grib_yyr2[] =
{
       0,     2,     1,     1,     1,     1,     1,     0,     1,     3,
       1,     3,     1,     2,     3,     2,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     1,     1,     1,     3,
       1,     7,    10,     7,    10,     7,     7,     7,    10,     7,
       7,    10,     7,    10,     8,    12,    10,     8,    10,     8,
       9,     7,     2,     2,     4,     6,     7,     2,     5,     5,
       4,     6,     7,     2,     5,     8,     8,     3,     5,     5,
       5,     5,     6,     3,     3,     3,     5,     2,     7,     4,
       8,    10,     5,     5,     5,     5,     2,     4,     3,     4,
       4,     6,     4,     2,     1,     5,     4,     2,     1,     5,
       4,     2,     5,     1,     7,    11,     7,    11,     6,     7,
      11,     4,     4,     2,     3,     1,     2,     1,     2,     1,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     8,     7,     7,     6,     9,
      12,    14,    10,    14,    12,     8,    11,     6,     9,    12,
      10,    14,    12,     8,    11,     1,     2,     1,     2,     4,
      10,     9,     7,     5,     5,     5,     5,     1,     2,     4,
       1,     1,     1,     1,     1,     1,     1,     3,     2,     3,
       4,     3,     1,     3,     3,     3,     3,     3,     1,     3,
       3,     1,     3,     3,     3,     3,     3,     3,     3,     2,
       1,     3,     1,     3,     1,     1,     1,     1,     4,     2,
       1,     2,     1,     7,     1,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const grib_yytype_uint8 grib_yydefact[] =
{
       0,     6,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    94,    98,   103,     0,     7,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     2,
       4,    12,     0,    17,    22,    18,    19,    20,    21,     3,
      23,   155,   214,   212,   206,   207,     5,     0,     0,     0,
       0,     0,     0,     7,     0,     0,    52,    53,     0,     7,
       0,     0,     0,     0,     0,     0,     0,    77,     0,    57,
       0,     0,     0,     0,     0,     7,     0,     0,     0,    93,
       0,    97,     0,   101,     0,     0,   175,   176,     0,   170,
     171,   173,   174,     0,     0,    26,    86,    27,    28,   172,
     182,   188,   191,   200,   202,   204,   205,    30,   209,     0,
       7,     0,     0,     0,     0,    63,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     7,     0,     0,     0,     0,
       7,     0,     1,     0,     0,     0,    15,    13,    16,     0,
       0,   156,     0,     0,   215,     0,     0,     0,     0,    74,
      75,     0,     0,     0,     0,     0,     0,     7,   115,     7,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     7,     0,     0,     0,     0,     0,     0,
       0,   117,    88,     0,     0,     0,     0,     0,     0,     7,
     199,     0,     0,   172,   178,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     7,    73,     0,     7,     7,     7,     0,
       0,     0,     0,     7,     7,     7,    67,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     7,     0,     0,    14,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     7,   116,    54,     0,     0,     0,     0,     0,
       0,     0,     7,    79,     7,     0,     7,     7,     0,     0,
       0,     0,     0,     0,     0,   122,   132,   121,   133,   134,
     124,   123,   125,   127,   130,   126,   128,   129,   131,   118,
     119,     0,    89,     0,    90,    92,    96,   100,     0,     0,
     179,     0,   177,    29,   170,   198,   181,   186,   187,   183,
     184,   185,   190,   189,   193,   197,   195,   196,   192,   194,
     201,   203,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    87,     0,     0,     0,     0,   167,
     208,     0,     0,     0,     7,    60,     0,     0,     0,     0,
       0,     7,     7,     0,     7,     7,     7,     0,     7,     7,
       7,     7,     7,     0,     0,     0,     0,    76,     0,     0,
      58,    59,     7,     0,     0,     0,     0,     7,     0,     0,
       0,     0,     7,     0,     0,     0,     0,     0,    10,     8,
       0,    95,    99,   102,    85,   180,    68,     7,    69,    70,
      71,     0,     0,    64,     0,    82,    83,    84,     0,     0,
       0,   164,   168,   163,   165,     7,     7,   166,     0,     0,
       0,     0,   210,     0,     0,     7,     7,     7,     7,     0,
       7,     7,     7,     0,     7,    55,     7,     7,     7,     7,
       7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
       7,     0,     0,     0,     0,   138,     0,     0,     0,     0,
     147,   120,     0,     0,     0,     0,    24,   108,     0,    91,
      72,     7,     7,     7,     0,     0,   157,     0,     0,     7,
      61,     0,     0,   104,   211,   213,   106,     0,    31,     0,
      33,   137,    35,    36,    39,   136,    56,     0,    40,     0,
      42,     0,    37,     0,    30,     7,     0,     7,     0,    78,
       7,     0,    51,     0,     0,     0,     0,     7,     0,     0,
       0,     0,     7,     0,     0,   113,   109,     0,    25,    11,
       9,     0,     0,     7,     0,     0,   162,   158,     0,   169,
      62,     0,     0,     7,     7,     7,     7,     7,     7,     0,
      44,     7,    47,     7,    80,     7,     0,     7,     0,     0,
     145,     0,     7,     0,     0,   153,   111,   112,     0,   114,
      65,    66,    49,     0,     0,   135,     0,     0,     7,     7,
       7,     7,     7,     7,     0,     7,    50,     7,     0,     7,
     139,     0,     0,     0,     7,   148,     0,     0,     0,   159,
     161,     0,     0,     0,    32,    34,    41,    43,    38,    46,
       0,    48,    81,     0,   142,     0,     7,     0,   150,     0,
       7,     0,   160,   105,   107,     7,     0,     7,     0,     7,
     146,     7,     0,     7,   154,   110,    45,     0,   140,     0,
     144,   149,     0,   152,     7,     7,     7,   141,   143,   151
};

/* YYDEFGOTO[NTERM-NUM].  */
static const grib_yytype_int16 grib_yydefgoto[] =
{
      -1,    58,   211,   420,   451,    61,   497,   126,   127,   128,
      62,    63,    64,   494,   495,   189,   212,   319,   320,    65,
      66,    67,    68,    69,   505,   506,    70,    71,   368,   369,
     129,   130,   131,   132,   133,   134,   135,   136,   137,    72,
     452,   453,    74,    75,    76
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -476
static const grib_yytype_int16 grib_yypact[] =
{
    1009,  -476,   -61,   -44,   115,   -22,    -8,    -9,   -32,    12,
      92,    13,    36,   202,    58,    62,    73,    93,    75,    85,
     106,   109,   130,   137,   141,   144,   184,   196,   107,   205,
     211,   -63,   -21,   -19,   213,   148,   218,   238,   248,   147,
     253,   266,   269,   281,   278,   290,   294,   298,   293,   308,
     315,   316,   309,   310,   -11,   319,   320,   -45,   407,  -476,
    -476,  1095,   328,  -476,  -476,  -476,  -476,  -476,  -476,   289,
    -476,  -476,     6,  -476,  -476,  -476,  -476,   148,   148,   332,
     333,   335,   336,   148,   337,   338,  -476,  -476,   148,    -3,
     339,   340,   342,   343,   344,   345,   215,   341,   -37,  -476,
     346,   348,   347,   192,   206,   334,   148,   349,   350,  -476,
     351,  -476,   352,  -476,   355,   356,  -476,  -476,   148,   357,
    -476,  -476,  -476,   148,   110,  -476,  -476,  -476,   363,   353,
     354,  -476,    79,     2,    37,   372,   374,  -476,  -476,   361,
     334,   370,   365,   366,   367,  -476,   375,   377,   384,   388,
     396,   397,   398,   148,   148,   148,   399,    11,   400,   401,
      14,   402,  -476,   403,   420,   408,  1266,  -476,  -476,   404,
     405,  -476,   409,   414,  -476,   411,   417,   421,   418,  -476,
    -476,   423,   424,   426,   425,   148,   410,   148,  -476,   334,
     428,   427,   430,   431,   432,   433,   429,   441,   442,   435,
     444,   148,   148,   148,   445,   446,   289,   447,   448,   289,
     307,  -476,  -476,   440,   -17,   148,   443,   449,   450,   148,
      37,    69,   451,  -476,  -476,   148,   228,   110,   110,   110,
     110,   110,   110,   110,   110,   110,   110,   110,   110,   110,
     110,   148,   148,   148,  -476,   454,   148,   148,   148,   455,
     456,   457,   458,   148,   148,   148,  -476,   459,   460,   148,
     453,   413,   453,   453,   148,   466,   334,   453,   148,  -476,
     438,   148,   148,   439,   462,   467,   470,   463,   235,   471,
     465,   472,   419,  -476,  -476,   480,   482,   483,   484,   485,
     486,   461,   334,  -476,   148,   487,   334,   334,   479,    28,
     -62,   -41,    29,    27,   101,  -476,  -476,  -476,  -476,  -476,
    -476,  -476,  -476,  -476,  -476,  -476,  -476,  -476,  -476,   452,
    -476,   -15,  -476,   242,  -476,  -476,   488,   489,   491,   492,
    -476,   494,  -476,  -476,  -476,  -476,  -476,  -476,  -476,  -476,
    -476,  -476,    79,    79,     2,     2,     2,     2,     2,     2,
      37,   372,   495,   500,   496,   498,   499,   509,   510,   511,
     513,   505,   506,   507,  -476,   517,   516,   508,   518,   453,
    -476,   519,   521,   529,   419,  -476,   525,   531,   532,  1183,
    1266,    54,    55,  1266,   419,   419,   419,  1266,   419,   334,
      76,    77,    80,   219,   239,   534,   544,  -476,   536,   539,
    -476,  -476,   419,   547,   537,   550,   289,   334,   551,   541,
     554,   289,   334,   307,   556,   558,   287,   546,  -476,  -476,
     -34,  -476,  -476,  -476,  -476,  -476,  -476,   334,  -476,  -476,
    -476,   552,   553,  -476,   148,  -476,  -476,  -476,   572,   555,
     148,  -476,  -476,  -476,  -476,   419,   334,  -476,   557,   559,
     -10,   549,   -35,   560,   561,   148,   334,   148,   334,   563,
     334,   334,   334,   564,   334,  -476,   148,   334,   148,   334,
     148,   334,   148,   419,   148,   419,   148,   334,   419,   148,
     334,   562,   289,    96,   126,  -476,   566,   289,   113,   132,
    -476,  -476,   567,   569,   546,   -23,  -476,   576,   284,  -476,
    -476,   148,   148,   419,   148,   -36,  -476,  1266,   580,   334,
    -476,  1266,   -35,   643,  -476,  -476,   644,   565,  -476,   579,
    -476,  -476,  -476,  -476,  -476,  -476,  -476,   581,  -476,   585,
    -476,   586,  -476,   578,   451,   -26,   587,   334,   588,  -476,
     334,   589,  -476,   568,   170,   593,   583,   334,   599,   177,
     601,   592,   334,   148,   148,   576,   645,   546,  -476,  -476,
    -476,   591,   596,   334,   395,   594,  -476,  -476,   597,  -476,
    -476,   595,   600,   419,   419,   419,   419,   419,   419,   603,
    -476,   419,  -476,   334,  -476,   419,   134,   334,   605,   289,
    -476,   143,   334,   610,   289,  -476,  -476,  -476,   604,   576,
    -476,  -476,  -476,  1266,   917,  -476,  1266,  1266,   334,   334,
     334,   334,   334,   334,   615,   334,  -476,   334,   617,   334,
    -476,   619,   230,   620,   334,  -476,   621,   270,   287,  -476,
    -476,   609,   611,   612,  -476,  -476,  -476,  -476,  -476,  -476,
     614,  -476,  -476,   150,  -476,   153,   334,   618,  -476,   178,
     334,   -20,  -476,  -476,  -476,   334,   626,   334,   628,   334,
    -476,   334,   631,   334,  -476,  -476,  -476,   623,  -476,   624,
    -476,  -476,   625,  -476,   334,   334,   334,  -476,  -476,  -476
};

/* YYPGOTO[NTERM-NUM].  */
static const grib_yytype_int16 grib_yypgoto[] =
{
    -476,  -476,     7,  -476,     0,  -476,  -475,   422,  -224,  -147,
    -476,  -476,  -476,  -319,  -192,  -107,   152,  -476,   127,  -476,
    -476,  -476,  -476,  -189,  -476,    38,  -476,   -60,  -249,  -476,
     -99,   598,   133,   151,   175,  -115,   481,  -476,   -66,  -476,
       8,   272,  -476,  -476,   649
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -8
static const grib_yytype_int16 grib_yytable[] =
{
      60,   333,   417,   220,   156,   156,   256,    59,    73,   171,
     172,   175,   176,   371,   372,   116,   117,   301,   376,   555,
     304,    36,   184,   579,   109,   223,   414,   415,   322,   414,
     415,   110,   405,    77,   414,   415,   406,   504,   281,   565,
     213,   160,   125,   116,   117,   169,    55,    56,   170,   199,
      78,   173,   161,   266,   296,   297,   498,   222,   407,   200,
      84,   167,    36,   566,    81,   499,   111,   118,   113,   119,
     120,   121,   122,   112,   210,   114,   556,   123,    82,   665,
      73,   323,   599,   416,   124,    83,   157,   272,   257,   185,
     125,   261,   173,   186,   187,   118,   188,   119,   120,   121,
     122,   116,   117,   233,    85,   123,   264,    88,   234,   260,
     265,   187,   124,   235,   236,   237,   238,   373,   403,   408,
     442,   410,    89,   404,   409,   411,   350,   335,   223,   223,
     223,   223,   223,   223,   223,   223,   223,   223,   223,   223,
     223,   223,   116,   117,   239,   240,   455,   457,   324,   325,
      92,   187,   187,   118,    93,   119,   120,   121,   122,   228,
     229,    96,   125,   123,   330,    94,   269,   188,   466,   468,
     124,    97,   470,   187,   187,   389,   557,   187,    86,    87,
     116,   117,   230,   231,   232,    95,   545,   169,    55,    56,
     170,   546,    98,   366,   125,    99,   119,   120,   121,   122,
     412,   106,   377,   550,   123,   378,   261,    79,   551,    80,
     125,   124,   169,    55,    56,   170,   100,   484,   169,    55,
      56,   170,   489,   101,   618,   547,   125,   102,   125,   619,
     103,   552,   118,   623,   119,   120,   121,   122,   624,   141,
     656,   171,   123,   658,   171,   657,   473,   475,   659,   124,
     125,   116,   117,   125,   125,   125,   169,    55,    56,   170,
     125,   125,   125,   169,    55,    56,   170,   446,   662,   587,
     104,   116,   117,   663,   456,   458,   592,   460,   461,   462,
     564,   464,   105,   467,   469,   471,   204,   503,   205,   188,
     206,   107,   244,   544,    90,   480,    91,   108,   549,   115,
     207,   125,   208,   118,   209,   119,   120,   121,   122,   138,
     305,   196,   197,   472,   334,   120,   169,    55,    56,   170,
     124,   384,   385,   118,   139,   119,   120,   121,   122,   646,
     418,   419,   557,   474,   140,   306,   414,   415,   509,   142,
     124,   284,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   143,   317,   318,   144,   169,    55,    56,   170,
     336,   337,   338,   339,   340,   341,   535,   145,   537,   650,
     146,   540,   559,   560,   508,   169,    55,    56,   170,   150,
     454,   188,   147,   459,   342,   343,   148,   463,   188,   188,
     149,   188,   188,   188,   151,   188,   563,   188,   188,   188,
     622,   152,   153,   154,   155,   627,   534,   162,   534,   188,
     344,   345,   346,   347,   348,   349,   158,   159,   375,   168,
     177,   178,   179,   180,   171,   182,   183,   190,   191,   171,
     192,   193,   194,   195,   210,   156,   651,   198,   226,   216,
     217,   203,   218,   201,   397,   202,   214,   215,   400,   401,
     219,   221,   188,   225,   241,   243,   227,   242,   245,   246,
     247,   248,   125,   249,   125,   250,   608,   609,   610,   611,
     612,   613,   251,   125,   615,   125,   252,   125,   617,   125,
     188,   125,   188,   125,   171,   188,   125,   596,   597,   171,
     253,   254,   255,   259,   160,   603,   282,   268,   262,   263,
     267,   270,   161,   271,   370,   181,   273,   568,   125,   125,
     188,   272,   274,   276,   275,   291,   187,   278,   277,   279,
     280,   285,   286,   287,   288,   289,   290,   292,   293,   294,
     295,   299,   300,   302,   303,   321,   260,   379,   326,   367,
     491,   465,   413,   567,   327,   328,   332,   353,   357,   358,
     359,   360,   374,   381,   364,   365,   382,   386,   396,   485,
     380,   383,   171,   387,   490,   388,   390,   171,   391,   392,
     393,   394,   395,   399,   402,   421,   422,   258,   423,   500,
     188,   188,   188,   188,   188,   188,   427,   424,   188,   425,
     426,   428,   188,   429,   430,   431,   432,   433,   510,   434,
     435,   436,   437,   629,   631,   440,   632,   633,   518,   283,
     520,   439,   522,   523,   524,   438,   526,   441,   443,   528,
     444,   530,   445,   532,   447,   298,   448,   449,   476,   539,
     477,   478,   542,   479,   481,   482,   483,   496,   486,   487,
     488,   329,   492,   331,   493,   504,   501,   502,   513,   571,
     572,   598,   543,   507,   586,   511,   548,   512,   573,   515,
     516,   570,   521,   525,   553,   352,   554,   558,   354,   355,
     356,   569,   574,   578,   575,   361,   362,   363,   576,   577,
     588,   589,   581,   583,   585,   591,   600,   580,   593,   582,
     594,   601,   584,   606,   604,   621,   605,   614,   607,   590,
     626,   640,   628,   643,   595,   645,   647,   649,   652,   655,
     653,   654,   667,   661,   669,   602,   398,   672,   674,   675,
     676,   174,   224,   351,   514,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   616,     0,     0,     0,   620,
       0,     0,     0,     0,   625,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     634,   635,   636,   637,   638,   639,     0,   641,     0,   642,
       0,   644,     0,     0,     0,     0,   648,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   660,     0,
       0,     0,   664,     0,     0,     0,     0,   666,     0,   668,
       0,   670,     0,   671,     0,   673,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   677,   678,   679,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   517,     0,   519,
       0,     0,     0,     0,     0,     0,     0,     0,   527,     0,
     529,     0,   531,     0,   533,     0,   536,     0,   538,     0,
       0,   541,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   163,     3,   561,   562,     4,     5,     6,     7,     8,
       9,    10,     0,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,     0,
       0,    27,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    28,    29,    30,    31,    32,
      33,    34,    35,     0,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
       0,    53,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   164,     0,     0,   165,     0,     0,    -7,
       1,     0,     0,     2,     3,     0,   630,     4,     5,     6,
       7,     8,     9,    10,     0,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,     0,     0,    27,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,     0,    53,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    54,    55,    56,    57,   163,
       3,     0,     0,     4,     5,     6,     7,     8,     9,    10,
       0,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,     0,     0,    27,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    28,    29,    30,    31,    32,    33,    34,
      35,     0,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,     0,    53,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   164,     0,     0,   165,     0,   166,   163,     3,     0,
       0,     4,     5,     6,     7,     8,     9,    10,     0,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,     0,     0,    27,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,     0,    53,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   450,
     163,     3,   165,     0,     4,     5,     6,     7,     8,     9,
      10,     0,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,     0,     0,
      27,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    28,    29,    30,    31,    32,    33,
      34,    35,     0,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,     0,
      53,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   164,     0,     0,   165
};

static const grib_yytype_int16 grib_yycheck[] =
{
       0,   225,   321,   118,    15,    15,   153,     0,     0,    69,
       4,    77,    78,   262,   263,    32,    33,   206,   267,   494,
     209,    56,    88,    49,    87,   124,    49,    50,    45,    49,
      50,    94,    94,    94,    49,    50,    98,    73,   185,    75,
     106,    86,    35,    32,    33,    86,    87,    88,    89,    86,
      94,    86,    97,   160,   201,   202,    90,   123,    99,    96,
      92,    61,    56,    99,    86,    99,    87,    84,    87,    86,
      87,    88,    89,    94,   100,    94,    99,    94,    86,    99,
      72,    98,   557,    98,   101,    94,    97,    97,   154,    92,
      83,   157,    86,    96,    97,    84,    89,    86,    87,    88,
      89,    32,    33,   101,    92,    94,    92,    94,   106,    98,
      96,    97,   101,    76,    77,    78,    79,   264,    90,    90,
     369,    94,    86,    95,    95,    98,   241,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,   238,
     239,   240,    32,    33,   107,   108,    92,    92,   214,   215,
      92,    97,    97,    84,    92,    86,    87,    88,    89,    80,
      81,    86,   155,    94,    95,    92,   166,   160,    92,    92,
     101,    86,    92,    97,    97,   282,   495,    97,    86,    87,
      32,    33,   103,   104,   105,    92,    90,    86,    87,    88,
      89,    95,    86,   259,   187,    86,    86,    87,    88,    89,
      99,    94,   268,    90,    94,   271,   272,    92,    95,    94,
     203,   101,    86,    87,    88,    89,    86,   406,    86,    87,
      88,    89,   411,    86,    90,    99,   219,    86,   221,    95,
      86,    99,    84,    90,    86,    87,    88,    89,    95,    92,
      90,   301,    94,    90,   304,    95,   393,   394,    95,   101,
     243,    32,    33,   246,   247,   248,    86,    87,    88,    89,
     253,   254,   255,    86,    87,    88,    89,   374,    90,    99,
      86,    32,    33,    95,   381,   382,    99,   384,   385,   386,
     504,   388,    86,   390,   391,   392,    94,   434,    96,   282,
      98,    86,   140,   482,    92,   402,    94,    86,   487,    86,
      94,   294,    96,    84,    98,    86,    87,    88,    89,    91,
       3,    96,    97,    94,    86,    87,    86,    87,    88,    89,
     101,    86,    87,    84,    86,    86,    87,    88,    89,    99,
      88,    89,   651,    94,    86,    28,    49,    50,   445,    86,
     101,   189,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    86,    46,    47,    86,    86,    87,    88,    89,
     227,   228,   229,   230,   231,   232,   473,    86,   475,    99,
      92,   478,    88,    89,   440,    86,    87,    88,    89,    86,
     380,   374,    92,   383,   233,   234,    92,   387,   381,   382,
      92,   384,   385,   386,    86,   388,   503,   390,   391,   392,
     589,    86,    86,    94,    94,   594,   472,     0,   474,   402,
     235,   236,   237,   238,   239,   240,    97,    97,   266,    91,
      88,    88,    87,    87,   484,    88,    88,    88,    88,   489,
      88,    88,    88,    88,   100,    15,   628,    96,    85,    88,
      88,    94,    87,    97,   292,    97,    97,    97,   296,   297,
      94,    94,   445,    90,    82,    94,   102,    83,    88,    94,
      94,    94,   455,    88,   457,    88,   573,   574,   575,   576,
     577,   578,    88,   466,   581,   468,    88,   470,   585,   472,
     473,   474,   475,   476,   544,   478,   479,   553,   554,   549,
      94,    94,    94,    94,    86,   100,    86,    94,    98,    98,
      98,    97,    97,    94,    91,    83,    95,   507,   501,   502,
     503,    97,    95,    95,    93,    86,    97,    93,    95,    93,
      95,    93,    95,    93,    93,    93,    93,    86,    86,    94,
      86,    86,    86,    86,    86,    95,    98,    98,    95,    86,
     413,   389,    90,   505,    95,    95,    95,    93,    93,    93,
      93,    93,    86,    86,    95,    95,    86,    86,    97,   407,
      98,    98,   622,    98,   412,    93,    86,   627,    86,    86,
      86,    86,    86,    86,    95,    87,    87,   155,    87,   427,
     573,   574,   575,   576,   577,   578,    86,    95,   581,    95,
      95,    95,   585,    95,    95,    86,    86,    86,   446,    86,
      95,    95,    95,   603,   604,    97,   606,   607,   456,   187,
     458,    95,   460,   461,   462,    98,   464,    99,    99,   467,
      99,   469,    93,   471,    99,   203,    95,    95,    94,   477,
      86,    95,   480,    94,    87,    98,    86,    91,    87,    98,
      86,   219,    86,   221,    86,    73,    94,    94,    99,     6,
       6,     6,    90,    98,    86,    98,    90,    98,    93,    99,
      99,   509,    99,    99,    97,   243,    97,    91,   246,   247,
     248,    91,    93,    95,    93,   253,   254,   255,    93,    93,
      87,    98,    95,    95,    95,    86,    95,   535,    87,   537,
      98,    95,   540,    98,   100,    90,    99,    94,    98,   547,
      90,    86,    98,    86,   552,    86,    86,    86,    99,    95,
      99,    99,    86,    95,    86,   563,   294,    86,    95,    95,
      95,    72,   124,   242,   452,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   583,    -1,    -1,    -1,   587,
      -1,    -1,    -1,    -1,   592,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     608,   609,   610,   611,   612,   613,    -1,   615,    -1,   617,
      -1,   619,    -1,    -1,    -1,    -1,   624,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   646,    -1,
      -1,    -1,   650,    -1,    -1,    -1,    -1,   655,    -1,   657,
      -1,   659,    -1,   661,    -1,   663,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   674,   675,   676,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   455,    -1,   457,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   466,    -1,
     468,    -1,   470,    -1,   472,    -1,   474,    -1,   476,    -1,
      -1,   479,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     4,     5,   501,   502,     8,     9,    10,    11,    12,
      13,    14,    -1,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    -1,
      -1,    34,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    48,    49,    50,    51,    52,
      53,    54,    55,    -1,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      -1,    74,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    86,    -1,    -1,    89,    -1,    -1,     0,
       1,    -1,    -1,     4,     5,    -1,    99,     8,     9,    10,
      11,    12,    13,    14,    -1,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    -1,    -1,    34,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    -1,    74,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    87,    88,    89,     4,
       5,    -1,    -1,     8,     9,    10,    11,    12,    13,    14,
      -1,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    -1,    -1,    34,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    48,    49,    50,    51,    52,    53,    54,
      55,    -1,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    -1,    74,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    -1,    -1,    89,    -1,    91,     4,     5,    -1,
      -1,     8,     9,    10,    11,    12,    13,    14,    -1,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    -1,    -1,    34,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    -1,    74,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,
       4,     5,    89,    -1,     8,     9,    10,    11,    12,    13,
      14,    -1,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    -1,    -1,
      34,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    48,    49,    50,    51,    52,    53,
      54,    55,    -1,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    -1,
      74,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    86,    -1,    -1,    89
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const grib_yytype_uint8 grib_yystos[] =
{
       0,     1,     4,     5,     8,     9,    10,    11,    12,    13,
      14,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    34,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    74,    86,    87,    88,    89,   110,   111,
     113,   114,   119,   120,   121,   128,   129,   130,   131,   132,
     135,   136,   148,   149,   151,   152,   153,    94,    94,    92,
      94,    86,    86,    94,    92,    92,    86,    87,    94,    86,
      92,    94,    92,    92,    92,    92,    86,    86,    86,    86,
      86,    86,    86,    86,    86,    86,    94,    86,    86,    87,
      94,    87,    94,    87,    94,    86,    32,    33,    84,    86,
      87,    88,    89,    94,   101,   111,   116,   117,   118,   139,
     140,   141,   142,   143,   144,   145,   146,   147,    91,    86,
      86,    92,    86,    86,    86,    86,    92,    92,    92,    92,
      86,    86,    86,    86,    94,    94,    15,    97,    97,    97,
      86,    97,     0,     4,    86,    89,    91,   113,    91,    86,
      89,   136,     4,    86,   153,   147,   147,    88,    88,    87,
      87,   116,    88,    88,   147,    92,    96,    97,   111,   124,
      88,    88,    88,    88,    88,    88,    96,    97,    96,    86,
      96,    97,    97,    94,    94,    96,    98,    94,    96,    98,
     100,   111,   125,   147,    97,    97,    88,    88,    87,    94,
     144,    94,   147,   139,   140,    90,    85,   102,    80,    81,
     103,   104,   105,   101,   106,    76,    77,    78,    79,   107,
     108,    82,    83,    94,   125,    88,    94,    94,    94,    88,
      88,    88,    88,    94,    94,    94,   118,   147,   116,    94,
      98,   147,    98,    98,    92,    96,   124,    98,    94,   113,
      97,    94,    97,    95,    95,    93,    95,    95,    93,    93,
      95,   118,    86,   116,   125,    93,    95,    93,    93,    93,
      93,    86,    86,    86,    94,    86,   118,   118,   116,    86,
      86,   132,    86,    86,   132,     3,    28,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    46,    47,   126,
     127,    95,    45,    98,   147,   147,    95,    95,    95,   116,
      95,   116,    95,   117,    86,   139,   141,   141,   141,   141,
     141,   141,   142,   142,   143,   143,   143,   143,   143,   143,
     144,   145,   116,    93,   116,   116,   116,    93,    93,    93,
      93,   116,   116,   116,    95,    95,   147,    86,   137,   138,
      91,   137,   137,   118,    86,   125,   137,   147,   147,    98,
      98,    86,    86,    98,    86,    87,    86,    98,    93,   124,
      86,    86,    86,    86,    86,    86,    97,   125,   116,    86,
     125,   125,    95,    90,    95,    94,    98,    99,    90,    95,
      94,    98,    99,    90,    49,    50,    98,   122,    88,    89,
     112,    87,    87,    87,    95,    95,    95,    86,    95,    95,
      95,    86,    86,    86,    86,    95,    95,    95,    98,    95,
      97,    99,   137,    99,    99,    93,   124,    99,    95,    95,
      86,   113,   149,   150,   113,    92,   124,    92,   124,   113,
     124,   124,   124,   113,   124,   125,    92,   124,    92,   124,
      92,   124,    94,   118,    94,   118,    94,    86,    95,    94,
     124,    87,    98,    86,   132,   125,    87,    98,    86,   132,
     125,   127,    86,    86,   122,   123,    91,   115,    90,    99,
     125,    94,    94,   118,    73,   133,   134,    98,   147,   124,
     125,    98,    98,    99,   150,    99,    99,   116,   125,   116,
     125,    99,   125,   125,   125,    99,   125,   116,   125,   116,
     125,   116,   125,   116,   147,   124,   116,   124,   116,   125,
     124,   116,   125,    90,   132,    90,    95,    99,    90,   132,
      90,    95,    99,    97,    97,   115,    99,   122,    91,    88,
      89,   116,   116,   124,   117,    75,    99,   134,   113,    91,
     125,     6,     6,    93,    93,    93,    93,    93,    95,    49,
     125,    95,   125,    95,   125,    95,    86,    99,    87,    98,
     125,    86,    99,    87,    98,   125,   147,   147,     6,   115,
      95,    95,   125,   100,   100,    99,    98,    98,   124,   124,
     124,   124,   124,   124,    94,   124,   125,   124,    90,    95,
     125,    90,   132,    90,    95,   125,    90,   132,    98,   113,
      99,   113,   113,   113,   125,   125,   125,   125,   125,   125,
      86,   125,   125,    86,   125,    86,    99,    86,   125,    86,
      99,   123,    99,    99,    99,    95,    90,    95,    90,    95,
     125,    95,    90,    95,   125,    99,   125,    86,   125,    86,
     125,   125,    86,   125,    95,    95,    95,   125,   125,   125
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
#line 204 "griby.y"
    { grib_parser_all_actions = 0;grib_parser_concept=0; grib_parser_rules=0; }
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 205 "griby.y"
    { grib_parser_concept     = reverse((grib_yyvsp[(1) - (1)].concept_value)); }
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 206 "griby.y"
    { grib_parser_all_actions = (grib_yyvsp[(1) - (1)].act); }
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 207 "griby.y"
    { grib_parser_rules       = (grib_yyvsp[(1) - (1)].rules); }
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 209 "griby.y"
    { grib_parser_all_actions = 0; grib_parser_concept=0; grib_parser_rules=0; }
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 215 "griby.y"
    { (grib_yyval.dvalue)=grib_darray_push(0,0,(grib_yyvsp[(1) - (1)].dval));}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 216 "griby.y"
    { (grib_yyval.dvalue)=grib_darray_push(0,(grib_yyvsp[(1) - (3)].dvalue),(grib_yyvsp[(3) - (3)].dval));}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 217 "griby.y"
    { (grib_yyval.dvalue)=grib_darray_push(0,0,(grib_yyvsp[(1) - (1)].lval));}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 218 "griby.y"
    { (grib_yyval.dvalue)=grib_darray_push(0,(grib_yyvsp[(1) - (3)].dvalue),(grib_yyvsp[(3) - (3)].lval));}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 221 "griby.y"
    { (grib_yyvsp[(1) - (2)].act)->next = (grib_yyvsp[(2) - (2)].act); (grib_yyval.act) = (grib_yyvsp[(1) - (2)].act); }
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 222 "griby.y"
    { (grib_yyvsp[(1) - (3)].act)->next = (grib_yyvsp[(3) - (3)].act); (grib_yyval.act) = (grib_yyvsp[(1) - (3)].act); }
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 223 "griby.y"
    {  (grib_yyval.act) = (grib_yyvsp[(1) - (2)].act);}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 241 "griby.y"
    { (grib_yyval.explist) = 0; }
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 246 "griby.y"
    { (grib_yyvsp[(1) - (3)].explist)->next = (grib_yyvsp[(3) - (3)].explist); (grib_yyval.explist) = (grib_yyvsp[(1) - (3)].explist); }
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 249 "griby.y"
    { (grib_yyval.explist) = grib_arguments_new(grib_parser_context,(grib_yyvsp[(1) - (1)].exp),NULL); }
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 254 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (7)].str),"unsigned",(grib_yyvsp[(3) - (7)].lval),NULL,(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL,NULL);        free((grib_yyvsp[(5) - (7)].str));  }
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 257 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (10)].str),"unsigned",(grib_yyvsp[(3) - (10)].lval),(grib_yyvsp[(7) - (10)].explist),(grib_yyvsp[(9) - (10)].explist),(grib_yyvsp[(10) - (10)].lval),NULL,NULL);        free((grib_yyvsp[(5) - (10)].str));  }
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 260 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (7)].str),"unsigned_bits",(grib_yyvsp[(3) - (7)].lval),NULL,(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL,NULL);        free((grib_yyvsp[(5) - (7)].str));  }
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 263 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (10)].str),"unsigned_bits",(grib_yyvsp[(3) - (10)].lval),(grib_yyvsp[(7) - (10)].explist),(grib_yyvsp[(9) - (10)].explist),(grib_yyvsp[(10) - (10)].lval),NULL,NULL);        free((grib_yyvsp[(5) - (10)].str));  }
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 266 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (7)].str),"ascii",(grib_yyvsp[(3) - (7)].lval),NULL,(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL,NULL);  free((grib_yyvsp[(5) - (7)].str));  }
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 270 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (7)].str),"ascii",(grib_yyvsp[(3) - (7)].lval),NULL,(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL,NULL);  free((grib_yyvsp[(5) - (7)].str));  }
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 273 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (7)].str),"bytes",(grib_yyvsp[(3) - (7)].lval),NULL,(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL,NULL);      free((grib_yyvsp[(5) - (7)].str));  }
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 276 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (10)].str),"bytes",(grib_yyvsp[(3) - (10)].lval),(grib_yyvsp[(7) - (10)].explist),(grib_yyvsp[(9) - (10)].explist),(grib_yyvsp[(10) - (10)].lval),NULL,NULL);      free((grib_yyvsp[(5) - (10)].str));  }
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 279 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (7)].str),"ksec1expver",(grib_yyvsp[(3) - (7)].lval),NULL,(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL,NULL);  free((grib_yyvsp[(5) - (7)].str));  }
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 282 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (7)].str),"signed",(grib_yyvsp[(3) - (7)].lval),NULL,(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL,NULL);      free((grib_yyvsp[(5) - (7)].str));  }
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 285 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (10)].str),"signed",(grib_yyvsp[(3) - (10)].lval),(grib_yyvsp[(7) - (10)].explist),(grib_yyvsp[(9) - (10)].explist),(grib_yyvsp[(10) - (10)].lval),NULL,NULL);      free((grib_yyvsp[(5) - (10)].str));  }
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 288 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (7)].str),"signed_bits",(grib_yyvsp[(3) - (7)].lval),NULL,(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL,NULL);      free((grib_yyvsp[(5) - (7)].str));  }
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 291 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (10)].str),"signed_bits",(grib_yyvsp[(3) - (10)].lval),(grib_yyvsp[(7) - (10)].explist),(grib_yyvsp[(9) - (10)].explist),(grib_yyvsp[(10) - (10)].lval),NULL,NULL);      free((grib_yyvsp[(5) - (10)].str));  }
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 294 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (8)].str),"codetable",(grib_yyvsp[(3) - (8)].lval), (grib_yyvsp[(6) - (8)].explist),(grib_yyvsp[(7) - (8)].explist),(grib_yyvsp[(8) - (8)].lval),NULL,NULL);    free((grib_yyvsp[(5) - (8)].str)); }
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 297 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (12)].str),"codetable",(grib_yyvsp[(3) - (12)].lval), (grib_yyvsp[(6) - (12)].explist),(grib_yyvsp[(7) - (12)].explist),(grib_yyvsp[(12) - (12)].lval),NULL,(grib_yyvsp[(10) - (12)].str));
					free((grib_yyvsp[(5) - (12)].str));free((grib_yyvsp[(10) - (12)].str)); }
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 301 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (10)].str),"codetable",(grib_yyvsp[(3) - (10)].lval), (grib_yyvsp[(7) - (10)].explist),(grib_yyvsp[(9) - (10)].explist),(grib_yyvsp[(10) - (10)].lval),NULL,NULL);    free((grib_yyvsp[(5) - (10)].str)); }
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 304 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (8)].str),"complex_codetable",(grib_yyvsp[(3) - (8)].lval), (grib_yyvsp[(6) - (8)].explist),(grib_yyvsp[(7) - (8)].explist),(grib_yyvsp[(8) - (8)].lval),NULL,NULL);    free((grib_yyvsp[(5) - (8)].str)); }
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 307 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (10)].str),"complex_codetable",(grib_yyvsp[(3) - (10)].lval), (grib_yyvsp[(7) - (10)].explist),(grib_yyvsp[(9) - (10)].explist),(grib_yyvsp[(10) - (10)].lval),NULL,NULL);    free((grib_yyvsp[(5) - (10)].str)); }
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 310 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (8)].str),"codeflag",(grib_yyvsp[(3) - (8)].lval), (grib_yyvsp[(6) - (8)].explist),(grib_yyvsp[(7) - (8)].explist),(grib_yyvsp[(8) - (8)].lval),NULL,NULL);  free((grib_yyvsp[(5) - (8)].str)); }
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 313 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (9)].str),"lookup",(grib_yyvsp[(3) - (9)].lval),(grib_yyvsp[(7) - (9)].explist),NULL,(grib_yyvsp[(9) - (9)].lval),NULL,NULL); free((grib_yyvsp[(5) - (9)].str)); }
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 316 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (7)].str),"bit",0,(grib_yyvsp[(4) - (7)].explist),(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL,NULL); free((grib_yyvsp[(2) - (7)].str)); }
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 319 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (2)].str),"label",0,NULL,NULL,0,NULL,NULL);   free((grib_yyvsp[(2) - (2)].str));  }
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 322 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (2)].str),"label",0,NULL,NULL,0,NULL,NULL);   free((grib_yyvsp[(2) - (2)].str));  }
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 325 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (4)].str),"ibmfloat",4,NULL,(grib_yyvsp[(3) - (4)].explist),(grib_yyvsp[(4) - (4)].lval),NULL,NULL);free((grib_yyvsp[(2) - (4)].str));  }
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 328 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(4) - (6)].str),"ibmfloat",4,NULL,(grib_yyvsp[(5) - (6)].explist),(grib_yyvsp[(6) - (6)].lval),(grib_yyvsp[(2) - (6)].str),NULL);free((grib_yyvsp[(4) - (6)].str)); free((grib_yyvsp[(2) - (6)].str)); }
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 331 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (7)].str),"ibmfloat",4,(grib_yyvsp[(4) - (7)].explist),(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL,NULL);free((grib_yyvsp[(2) - (7)].str));  }
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 334 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (2)].str),"position",0,NULL,NULL,0,NULL,NULL);     free((grib_yyvsp[(2) - (2)].str));  }
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 337 "griby.y"
    { (grib_yyval.act) = grib_action_create_variable(grib_parser_context,(grib_yyvsp[(2) - (5)].str),"constant",0,(grib_yyvsp[(4) - (5)].explist),NULL,(grib_yyvsp[(5) - (5)].lval),NULL);free((grib_yyvsp[(2) - (5)].str)); }
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 340 "griby.y"
    { (grib_yyval.act) = grib_action_create_variable(grib_parser_context,(grib_yyvsp[(2) - (5)].str),"transient",0,(grib_yyvsp[(4) - (5)].explist),(grib_yyvsp[(4) - (5)].explist),(grib_yyvsp[(5) - (5)].lval),NULL);   free((grib_yyvsp[(2) - (5)].str)); }
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 343 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (4)].str),"ieeefloat",4,NULL,(grib_yyvsp[(3) - (4)].explist),(grib_yyvsp[(4) - (4)].lval),NULL,NULL);   free((grib_yyvsp[(2) - (4)].str));  }
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 346 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(4) - (6)].str),"ieeefloat",4,NULL,(grib_yyvsp[(5) - (6)].explist),(grib_yyvsp[(6) - (6)].lval),(grib_yyvsp[(2) - (6)].str),NULL);  free((grib_yyvsp[(4) - (6)].str));free((grib_yyvsp[(2) - (6)].str));}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 349 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (7)].str),"ieeefloat",4,(grib_yyvsp[(4) - (7)].explist),(grib_yyvsp[(6) - (7)].explist),(grib_yyvsp[(7) - (7)].lval),NULL,NULL);free((grib_yyvsp[(2) - (7)].str));  }
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 352 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (2)].str),"g1_half_byte_codeflag",0,NULL,NULL,0,NULL,NULL);free((grib_yyvsp[(2) - (2)].str));  }
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 355 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (5)].str),"section_length",(grib_yyvsp[(3) - (5)].lval),NULL,NULL,0,NULL,NULL);free((grib_yyvsp[(5) - (5)].str));  }
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 358 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (8)].str),"g1_message_length",(grib_yyvsp[(3) - (8)].lval),(grib_yyvsp[(7) - (8)].explist),NULL,0,NULL,NULL);free((grib_yyvsp[(5) - (8)].str));  }
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 361 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (8)].str),"g1_section4_length",(grib_yyvsp[(3) - (8)].lval),(grib_yyvsp[(7) - (8)].explist),NULL,0,NULL,NULL);free((grib_yyvsp[(5) - (8)].str));  }
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 364 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (3)].str),"ksec",0,(grib_yyvsp[(3) - (3)].explist),NULL,0,NULL,NULL);free((grib_yyvsp[(2) - (3)].str)); }
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 367 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (5)].str),"pad",0,(grib_yyvsp[(4) - (5)].explist),0,0,NULL,NULL);   free((grib_yyvsp[(2) - (5)].str)); }
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 370 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (5)].str),"padto",0,(grib_yyvsp[(4) - (5)].explist),0,0,NULL,NULL);   free((grib_yyvsp[(2) - (5)].str)); }
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 373 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (5)].str),"padtoeven",0,(grib_yyvsp[(4) - (5)].explist),0,0,NULL,NULL);   free((grib_yyvsp[(2) - (5)].str)); }
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 376 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (5)].str),"padtomultiple",0,(grib_yyvsp[(4) - (5)].explist),0,0,NULL,NULL);   free((grib_yyvsp[(2) - (5)].str)); }
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 379 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(5) - (6)].str),"message",(grib_yyvsp[(3) - (6)].lval),0,0,(grib_yyvsp[(6) - (6)].lval),NULL,NULL);   free((grib_yyvsp[(5) - (6)].str));  }
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 382 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[(2) - (3)].str),"section_padding",0,0,0,(grib_yyvsp[(3) - (3)].lval),NULL,NULL);   free((grib_yyvsp[(2) - (3)].str));  }
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 384 "griby.y"
    { (grib_yyval.act) = grib_action_create_template(grib_parser_context,0,(grib_yyvsp[(2) - (3)].str),(grib_yyvsp[(3) - (3)].str)); free((grib_yyvsp[(2) - (3)].str)); free((grib_yyvsp[(3) - (3)].str));}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 386 "griby.y"
    { (grib_yyval.act) = grib_action_create_template(grib_parser_context,1,(grib_yyvsp[(2) - (3)].str),(grib_yyvsp[(3) - (3)].str)); free((grib_yyvsp[(2) - (3)].str)); free((grib_yyvsp[(3) - (3)].str));}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 389 "griby.y"
    { (grib_yyval.act) = grib_action_create_alias(grib_parser_context,(grib_yyvsp[(2) - (5)].str),(grib_yyvsp[(4) - (5)].str),NULL,(grib_yyvsp[(5) - (5)].lval));  free((grib_yyvsp[(2) - (5)].str)); free((grib_yyvsp[(4) - (5)].str)); }
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 392 "griby.y"
    { (grib_yyval.act) = grib_action_create_alias(grib_parser_context,(grib_yyvsp[(2) - (2)].str),NULL,NULL,0);  free((grib_yyvsp[(2) - (2)].str)); }
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 395 "griby.y"
    {
         (grib_yyval.act) = grib_action_create_alias(grib_parser_context,(grib_yyvsp[(4) - (7)].str),(grib_yyvsp[(6) - (7)].str),(grib_yyvsp[(2) - (7)].str),(grib_yyvsp[(7) - (7)].lval));  free((grib_yyvsp[(2) - (7)].str)); free((grib_yyvsp[(4) - (7)].str)); free((grib_yyvsp[(6) - (7)].str));
    }
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 399 "griby.y"
    {
         (grib_yyval.act) = grib_action_create_alias(grib_parser_context,(grib_yyvsp[(4) - (4)].str),NULL,(grib_yyvsp[(2) - (4)].str),0);  free((grib_yyvsp[(2) - (4)].str)); free((grib_yyvsp[(4) - (4)].str)); 
    }
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 403 "griby.y"
    { (grib_yyval.act) = grib_action_create_meta(grib_parser_context,(grib_yyvsp[(2) - (8)].str),(grib_yyvsp[(3) - (8)].str),(grib_yyvsp[(5) - (8)].explist),(grib_yyvsp[(7) - (8)].explist),(grib_yyvsp[(8) - (8)].lval),NULL); free((grib_yyvsp[(2) - (8)].str));free((grib_yyvsp[(3) - (8)].str));}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 406 "griby.y"
    { (grib_yyval.act) = grib_action_create_meta(grib_parser_context,(grib_yyvsp[(4) - (10)].str),(grib_yyvsp[(5) - (10)].str),(grib_yyvsp[(7) - (10)].explist),(grib_yyvsp[(9) - (10)].explist),(grib_yyvsp[(10) - (10)].lval),(grib_yyvsp[(2) - (10)].str)); free((grib_yyvsp[(4) - (10)].str));free((grib_yyvsp[(5) - (10)].str));free((grib_yyvsp[(2) - (10)].str));}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 409 "griby.y"
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

  case 83:

/* Line 1455 of yacc.c  */
#line 421 "griby.y"
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

  case 84:

/* Line 1455 of yacc.c  */
#line 433 "griby.y"
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

  case 85:

/* Line 1455 of yacc.c  */
#line 445 "griby.y"
    { (grib_yyval.act) = grib_action_create_put(grib_parser_context,(grib_yyvsp[(2) - (5)].str),(grib_yyvsp[(4) - (5)].explist));free((grib_yyvsp[(2) - (5)].str));}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 448 "griby.y"
    { (grib_yyval.act) = grib_action_create_remove(grib_parser_context,(grib_yyvsp[(2) - (2)].explist));}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 451 "griby.y"
    { (grib_yyval.act) = grib_action_create_assert(grib_parser_context,(grib_yyvsp[(3) - (4)].exp));}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 454 "griby.y"
    { (grib_yyval.act) = grib_action_create_modify(grib_parser_context,(grib_yyvsp[(2) - (3)].str),(grib_yyvsp[(3) - (3)].lval)); free((grib_yyvsp[(2) - (3)].str));}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 456 "griby.y"
    { (grib_yyval.act) = grib_action_create_set_missing(grib_parser_context,(grib_yyvsp[(2) - (4)].str)); free((grib_yyvsp[(2) - (4)].str)); }
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 457 "griby.y"
    { (grib_yyval.act) = grib_action_create_set(grib_parser_context,(grib_yyvsp[(2) - (4)].str),(grib_yyvsp[(4) - (4)].exp),0); free((grib_yyvsp[(2) - (4)].str)); }
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 458 "griby.y"
    { (grib_yyval.act) = grib_action_create_set_darray(grib_parser_context,(grib_yyvsp[(2) - (6)].str),(grib_yyvsp[(5) - (6)].dvalue)); free((grib_yyvsp[(2) - (6)].str)); }
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 460 "griby.y"
    { (grib_yyval.act) = grib_action_create_set(grib_parser_context,(grib_yyvsp[(2) - (4)].str),(grib_yyvsp[(4) - (4)].exp),1); free((grib_yyvsp[(2) - (4)].str)); }
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 463 "griby.y"
    { (grib_yyval.act) = grib_action_create_write(grib_parser_context,(grib_yyvsp[(2) - (2)].str),0,0); free((grib_yyvsp[(2) - (2)].str));}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 464 "griby.y"
    { (grib_yyval.act) = grib_action_create_write(grib_parser_context,"",0,0); }
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 465 "griby.y"
    { (grib_yyval.act) = grib_action_create_write(grib_parser_context,(grib_yyvsp[(5) - (5)].str),0,(grib_yyvsp[(3) - (5)].lval)); free((grib_yyvsp[(5) - (5)].str));}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 466 "griby.y"
    { (grib_yyval.act) = grib_action_create_write(grib_parser_context,"",0,(grib_yyvsp[(3) - (4)].lval)); }
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 467 "griby.y"
    { (grib_yyval.act) = grib_action_create_write(grib_parser_context,(grib_yyvsp[(2) - (2)].str),1,0); free((grib_yyvsp[(2) - (2)].str));}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 468 "griby.y"
    { (grib_yyval.act) = grib_action_create_write(grib_parser_context,"",1,0); }
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 469 "griby.y"
    { (grib_yyval.act) = grib_action_create_write(grib_parser_context,(grib_yyvsp[(5) - (5)].str),1,(grib_yyvsp[(3) - (5)].lval)); free((grib_yyvsp[(5) - (5)].str));}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 470 "griby.y"
    { (grib_yyval.act) = grib_action_create_write(grib_parser_context,"",1,(grib_yyvsp[(3) - (4)].lval)); }
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 472 "griby.y"
    { (grib_yyval.act) = grib_action_create_print(grib_parser_context,(grib_yyvsp[(2) - (2)].str),0); free((grib_yyvsp[(2) - (2)].str)); }
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 473 "griby.y"
    { (grib_yyval.act) = grib_action_create_print(grib_parser_context,(grib_yyvsp[(5) - (5)].str),(grib_yyvsp[(3) - (5)].str)); free((grib_yyvsp[(5) - (5)].str)); free((grib_yyvsp[(3) - (5)].str));}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 474 "griby.y"
    { (grib_yyval.act) = grib_action_create_print(grib_parser_context,"",0);  }
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 478 "griby.y"
    { (grib_yyval.act) = grib_action_create_if(grib_parser_context,(grib_yyvsp[(3) - (7)].exp),(grib_yyvsp[(6) - (7)].act),0,0); }
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 479 "griby.y"
    { (grib_yyval.act) = grib_action_create_if(grib_parser_context,(grib_yyvsp[(3) - (11)].exp),(grib_yyvsp[(6) - (11)].act),(grib_yyvsp[(10) - (11)].act),0); }
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 480 "griby.y"
    { (grib_yyval.act) = grib_action_create_if(grib_parser_context,(grib_yyvsp[(3) - (7)].exp),(grib_yyvsp[(6) - (7)].act),0,1); }
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 481 "griby.y"
    { (grib_yyval.act) = grib_action_create_if(grib_parser_context,(grib_yyvsp[(3) - (11)].exp),(grib_yyvsp[(6) - (11)].act),(grib_yyvsp[(10) - (11)].act),1); }
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 485 "griby.y"
    { (grib_yyval.act) = grib_action_create_when(grib_parser_context,(grib_yyvsp[(3) - (6)].exp),(grib_yyvsp[(5) - (6)].act),NULL); }
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 486 "griby.y"
    { (grib_yyval.act) = grib_action_create_when(grib_parser_context,(grib_yyvsp[(3) - (7)].exp),(grib_yyvsp[(6) - (7)].act),NULL); }
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 487 "griby.y"
    { (grib_yyval.act) = grib_action_create_when(grib_parser_context,(grib_yyvsp[(3) - (11)].exp),(grib_yyvsp[(6) - (11)].act),(grib_yyvsp[(10) - (11)].act)); }
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 490 "griby.y"
    { (grib_yyval.act) = grib_action_create_set(grib_parser_context,(grib_yyvsp[(2) - (4)].str),(grib_yyvsp[(4) - (4)].exp),0); free((grib_yyvsp[(2) - (4)].str)); }
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 491 "griby.y"
    { (grib_yyval.act) = grib_action_create_set(grib_parser_context,(grib_yyvsp[(2) - (4)].str),(grib_yyvsp[(4) - (4)].exp),1); free((grib_yyvsp[(2) - (4)].str)); }
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 495 "griby.y"
    { (grib_yyvsp[(1) - (3)].act)->next = (grib_yyvsp[(2) - (3)].act); (grib_yyval.act) = (grib_yyvsp[(1) - (3)].act); }
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 499 "griby.y"
    { (grib_yyval.explist) = NULL ;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 500 "griby.y"
    { (grib_yyval.explist) = (grib_yyvsp[(2) - (2)].explist) ;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 503 "griby.y"
    { (grib_yyval.lval) = 0 ; }
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 504 "griby.y"
    { (grib_yyval.lval) = (grib_yyvsp[(2) - (2)].lval); }
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 508 "griby.y"
    { (grib_yyval.lval) = (grib_yyvsp[(1) - (3)].lval) | (grib_yyvsp[(3) - (3)].lval); }
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 511 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_READ_ONLY; }
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 512 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_LOWERCASE; }
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 513 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_DUMP; }
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 514 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_NO_COPY; }
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 515 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_NO_FAIL; }
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 516 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_HIDDEN; }
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 517 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_EDITION_SPECIFIC; }
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 518 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_CAN_BE_MISSING; }
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 519 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_CONSTRAINT; }
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 520 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_OVERRIDE; }
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 521 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_COPY_OK; }
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 522 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_TRANSIENT; }
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 523 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_STRING_TYPE; }
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 524 "griby.y"
    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_LONG_TYPE; }
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 527 "griby.y"
    { (grib_yyval.act) = grib_action_create_list(grib_parser_context,(grib_yyvsp[(1) - (8)].str),(grib_yyvsp[(4) - (8)].exp),(grib_yyvsp[(7) - (8)].act)); free((grib_yyvsp[(1) - (8)].str)); }
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 530 "griby.y"
    { (grib_yyval.act) = grib_action_create_while(grib_parser_context,(grib_yyvsp[(3) - (7)].exp),(grib_yyvsp[(6) - (7)].act));  }
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 533 "griby.y"
    { (grib_yyval.act) = grib_action_create_trigger(grib_parser_context,(grib_yyvsp[(3) - (7)].explist),(grib_yyvsp[(6) - (7)].act));  }
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 536 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(2) - (6)].str),(grib_yyvsp[(4) - (6)].concept_value),0,0,0,0,0,0,(grib_yyvsp[(6) - (6)].lval),0);  free((grib_yyvsp[(2) - (6)].str)); }
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 537 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(2) - (9)].str),(grib_yyvsp[(7) - (9)].concept_value),0,0,(grib_yyvsp[(4) - (9)].str),0,0,0,(grib_yyvsp[(9) - (9)].lval),0);  free((grib_yyvsp[(2) - (9)].str));free((grib_yyvsp[(4) - (9)].str)); }
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 538 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(2) - (12)].str),0,(grib_yyvsp[(6) - (12)].str),0,(grib_yyvsp[(4) - (12)].str),(grib_yyvsp[(8) - (12)].str),(grib_yyvsp[(10) - (12)].str),0,(grib_yyvsp[(12) - (12)].lval),0);  free((grib_yyvsp[(2) - (12)].str));free((grib_yyvsp[(6) - (12)].str));free((grib_yyvsp[(4) - (12)].str));free((grib_yyvsp[(8) - (12)].str));free((grib_yyvsp[(10) - (12)].str)); }
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 539 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(2) - (14)].str),0,(grib_yyvsp[(6) - (14)].str),0,(grib_yyvsp[(4) - (14)].str),(grib_yyvsp[(8) - (14)].str),(grib_yyvsp[(10) - (14)].str),(grib_yyvsp[(12) - (14)].str),(grib_yyvsp[(14) - (14)].lval),0);  free((grib_yyvsp[(2) - (14)].str));free((grib_yyvsp[(6) - (14)].str));free((grib_yyvsp[(4) - (14)].str));free((grib_yyvsp[(8) - (14)].str));free((grib_yyvsp[(10) - (14)].str));free((grib_yyvsp[(12) - (14)].str)); }
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 540 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(2) - (10)].str),0,(grib_yyvsp[(6) - (10)].str),0,(grib_yyvsp[(4) - (10)].str),(grib_yyvsp[(8) - (10)].str),0,0,(grib_yyvsp[(10) - (10)].lval),0);  free((grib_yyvsp[(2) - (10)].str));free((grib_yyvsp[(6) - (10)].str));free((grib_yyvsp[(4) - (10)].str));free((grib_yyvsp[(8) - (10)].str)); }
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 541 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(4) - (14)].str),0,(grib_yyvsp[(8) - (14)].str),(grib_yyvsp[(2) - (14)].str),(grib_yyvsp[(6) - (14)].str),(grib_yyvsp[(10) - (14)].str),(grib_yyvsp[(12) - (14)].str),0,(grib_yyvsp[(14) - (14)].lval),0);  free((grib_yyvsp[(4) - (14)].str));free((grib_yyvsp[(8) - (14)].str));free((grib_yyvsp[(6) - (14)].str));free((grib_yyvsp[(10) - (14)].str)); free((grib_yyvsp[(2) - (14)].str));}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 542 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(4) - (12)].str),0,(grib_yyvsp[(8) - (12)].str),(grib_yyvsp[(2) - (12)].str),(grib_yyvsp[(6) - (12)].str),(grib_yyvsp[(10) - (12)].str),0,0,(grib_yyvsp[(12) - (12)].lval),0);  free((grib_yyvsp[(4) - (12)].str));free((grib_yyvsp[(8) - (12)].str));free((grib_yyvsp[(6) - (12)].str));free((grib_yyvsp[(10) - (12)].str)); free((grib_yyvsp[(2) - (12)].str));}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 543 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(4) - (8)].str),(grib_yyvsp[(6) - (8)].concept_value),0,(grib_yyvsp[(2) - (8)].str),0,0,0,0,(grib_yyvsp[(8) - (8)].lval),0);  free((grib_yyvsp[(2) - (8)].str));free((grib_yyvsp[(4) - (8)].str)); }
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 544 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(4) - (11)].str),(grib_yyvsp[(9) - (11)].concept_value),0,(grib_yyvsp[(2) - (11)].str),(grib_yyvsp[(6) - (11)].str),0,0,0,(grib_yyvsp[(11) - (11)].lval),0);  free((grib_yyvsp[(2) - (11)].str));free((grib_yyvsp[(4) - (11)].str));free((grib_yyvsp[(6) - (11)].str)); }
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 545 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(2) - (6)].str),(grib_yyvsp[(4) - (6)].concept_value),0,0,0,0,0,0,(grib_yyvsp[(6) - (6)].lval),1);  free((grib_yyvsp[(2) - (6)].str)); }
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 546 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(2) - (9)].str),(grib_yyvsp[(7) - (9)].concept_value),0,0,(grib_yyvsp[(4) - (9)].str),0,0,0,(grib_yyvsp[(9) - (9)].lval),1);  free((grib_yyvsp[(2) - (9)].str));free((grib_yyvsp[(4) - (9)].str)); }
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 547 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(2) - (12)].str),0,(grib_yyvsp[(6) - (12)].str),0,(grib_yyvsp[(4) - (12)].str),(grib_yyvsp[(8) - (12)].str),(grib_yyvsp[(10) - (12)].str),0,(grib_yyvsp[(12) - (12)].lval),1);  free((grib_yyvsp[(2) - (12)].str));free((grib_yyvsp[(6) - (12)].str));free((grib_yyvsp[(4) - (12)].str));free((grib_yyvsp[(8) - (12)].str));free((grib_yyvsp[(10) - (12)].str)); }
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 548 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(2) - (10)].str),0,(grib_yyvsp[(6) - (10)].str),0,(grib_yyvsp[(4) - (10)].str),(grib_yyvsp[(8) - (10)].str),0,0,(grib_yyvsp[(10) - (10)].lval),1);  free((grib_yyvsp[(2) - (10)].str));free((grib_yyvsp[(6) - (10)].str));free((grib_yyvsp[(4) - (10)].str));free((grib_yyvsp[(8) - (10)].str)); }
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 549 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(4) - (14)].str),0,(grib_yyvsp[(8) - (14)].str),(grib_yyvsp[(2) - (14)].str),(grib_yyvsp[(6) - (14)].str),(grib_yyvsp[(10) - (14)].str),(grib_yyvsp[(12) - (14)].str),0,(grib_yyvsp[(14) - (14)].lval),1);  free((grib_yyvsp[(4) - (14)].str));free((grib_yyvsp[(8) - (14)].str));free((grib_yyvsp[(6) - (14)].str));free((grib_yyvsp[(10) - (14)].str));free((grib_yyvsp[(12) - (14)].str)); free((grib_yyvsp[(2) - (14)].str));}
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 550 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(4) - (12)].str),0,(grib_yyvsp[(8) - (12)].str),(grib_yyvsp[(2) - (12)].str),(grib_yyvsp[(6) - (12)].str),(grib_yyvsp[(10) - (12)].str),0,0,(grib_yyvsp[(12) - (12)].lval),1);  free((grib_yyvsp[(4) - (12)].str));free((grib_yyvsp[(8) - (12)].str));free((grib_yyvsp[(6) - (12)].str));free((grib_yyvsp[(10) - (12)].str)); free((grib_yyvsp[(2) - (12)].str));}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 551 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(4) - (8)].str),(grib_yyvsp[(6) - (8)].concept_value),0,(grib_yyvsp[(2) - (8)].str),0,0,0,0,(grib_yyvsp[(8) - (8)].lval),1);  free((grib_yyvsp[(2) - (8)].str));free((grib_yyvsp[(4) - (8)].str)); }
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 552 "griby.y"
    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[(4) - (11)].str),(grib_yyvsp[(9) - (11)].concept_value),0,(grib_yyvsp[(2) - (11)].str),(grib_yyvsp[(6) - (11)].str),0,0,0,(grib_yyvsp[(11) - (11)].lval),1);  free((grib_yyvsp[(2) - (11)].str));free((grib_yyvsp[(4) - (11)].str));free((grib_yyvsp[(6) - (11)].str)); }
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 557 "griby.y"
    { (grib_yyval.concept_value) = (grib_yyvsp[(2) - (2)].concept_value); (grib_yyvsp[(2) - (2)].concept_value)->next = (grib_yyvsp[(1) - (2)].concept_value);   }
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 561 "griby.y"
    { (grib_yyval.case_value) = (grib_yyvsp[(2) - (2)].case_value); (grib_yyvsp[(2) - (2)].case_value)->next = (grib_yyvsp[(1) - (2)].case_value);   }
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 564 "griby.y"
    { (grib_yyval.case_value) = grib_case_new(grib_parser_context,(grib_yyvsp[(2) - (4)].explist),(grib_yyvsp[(4) - (4)].act));  }
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 568 "griby.y"
    { (grib_yyval.act) = grib_action_create_switch(grib_parser_context,(grib_yyvsp[(3) - (10)].explist),(grib_yyvsp[(6) - (10)].case_value),(grib_yyvsp[(9) - (10)].act)); }
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 569 "griby.y"
    { (grib_yyval.act) = grib_action_create_switch(grib_parser_context,(grib_yyvsp[(3) - (9)].explist),(grib_yyvsp[(6) - (9)].case_value),grib_action_create_noop(grib_parser_context,"continue")); }
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 570 "griby.y"
    { (grib_yyval.act) = grib_action_create_switch(grib_parser_context,(grib_yyvsp[(3) - (7)].explist),(grib_yyvsp[(6) - (7)].case_value),0); }
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 573 "griby.y"
    {
	  				(grib_yyval.concept_value) = grib_concept_value_new(grib_parser_context,(grib_yyvsp[(1) - (5)].str),(grib_yyvsp[(4) - (5)].concept_condition)); free((grib_yyvsp[(1) - (5)].str));}
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 575 "griby.y"
    {
	  				(grib_yyval.concept_value) = grib_concept_value_new(grib_parser_context,(grib_yyvsp[(1) - (5)].str),(grib_yyvsp[(4) - (5)].concept_condition)); free((grib_yyvsp[(1) - (5)].str));}
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 577 "griby.y"
    {
					char buf[80]; sprintf(buf,"%ld",(long)(grib_yyvsp[(1) - (5)].lval)); (grib_yyval.concept_value) = grib_concept_value_new(grib_parser_context,buf,(grib_yyvsp[(4) - (5)].concept_condition));}
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 579 "griby.y"
    {
					char buf[80]; sprintf(buf,"%g",(double)(grib_yyvsp[(1) - (5)].dval)); (grib_yyval.concept_value) = grib_concept_value_new(grib_parser_context,buf,(grib_yyvsp[(4) - (5)].concept_condition));}
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 584 "griby.y"
    { (grib_yyvsp[(1) - (2)].concept_condition)->next = (grib_yyvsp[(2) - (2)].concept_condition); (grib_yyval.concept_condition) = (grib_yyvsp[(1) - (2)].concept_condition); }
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 587 "griby.y"
    { (grib_yyval.concept_condition) = grib_concept_condition_new(grib_parser_context,(grib_yyvsp[(1) - (4)].str),(grib_yyvsp[(3) - (4)].exp)); free((grib_yyvsp[(1) - (4)].str)); }
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 590 "griby.y"
    { (grib_yyval.exp) = new_accessor_expression(grib_parser_context,(grib_yyvsp[(1) - (1)].str)); free((grib_yyvsp[(1) - (1)].str)); }
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 591 "griby.y"
    { (grib_yyval.exp) = new_string_expression(grib_parser_context,(grib_yyvsp[(1) - (1)].str));  free((grib_yyvsp[(1) - (1)].str)); }
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 595 "griby.y"
    { (grib_yyval.exp) = new_long_expression(grib_parser_context,(grib_yyvsp[(1) - (1)].lval));  }
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 596 "griby.y"
    { (grib_yyval.exp) = new_double_expression(grib_parser_context,(grib_yyvsp[(1) - (1)].dval));  /* TODO: change to new_float_expression*/}
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 598 "griby.y"
    { (grib_yyval.exp) = NULL; }
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 599 "griby.y"
    { (grib_yyval.exp) = new_true_expression(grib_parser_context); }
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 600 "griby.y"
    { (grib_yyval.exp) = (grib_yyvsp[(2) - (3)].exp); }
    break;

  case 178:

/* Line 1455 of yacc.c  */
#line 601 "griby.y"
    { (grib_yyval.exp) = new_unop_expression(grib_parser_context,&grib_op_neg,&grib_op_neg_d,(grib_yyvsp[(2) - (2)].exp)); }
    break;

  case 179:

/* Line 1455 of yacc.c  */
#line 602 "griby.y"
    { (grib_yyval.exp) = new_func_expression(grib_parser_context,(grib_yyvsp[(1) - (3)].str),NULL); free((grib_yyvsp[(1) - (3)].str));}
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 603 "griby.y"
    { (grib_yyval.exp) = new_func_expression(grib_parser_context,(grib_yyvsp[(1) - (4)].str),(grib_yyvsp[(3) - (4)].explist)); free((grib_yyvsp[(1) - (4)].str));}
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 607 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_pow,NULL,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 183:

/* Line 1455 of yacc.c  */
#line 611 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_mul,&grib_op_mul_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 184:

/* Line 1455 of yacc.c  */
#line 612 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_div,&grib_op_div_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 613 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_modulo,NULL,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 186:

/* Line 1455 of yacc.c  */
#line 614 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_bit,NULL,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 187:

/* Line 1455 of yacc.c  */
#line 615 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_bitoff,NULL,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 189:

/* Line 1455 of yacc.c  */
#line 619 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_add,&grib_op_add_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 190:

/* Line 1455 of yacc.c  */
#line 620 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_sub,&grib_op_sub_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 624 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_gt,&grib_op_gt_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 626 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_eq,&grib_op_eq_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 627 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_lt,&grib_op_lt_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 628 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_ge,&grib_op_ge_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 629 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_le,&grib_op_le_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 630 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_ne,&grib_op_ne_d,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 631 "griby.y"
    { (grib_yyval.exp) = new_string_compare_expression(grib_parser_context,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 636 "griby.y"
    { (grib_yyval.exp) = new_unop_expression(grib_parser_context,&grib_op_not,NULL,(grib_yyvsp[(2) - (2)].exp)); }
    break;

  case 201:

/* Line 1455 of yacc.c  */
#line 640 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_and,NULL,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp)); }
    break;

  case 203:

/* Line 1455 of yacc.c  */
#line 644 "griby.y"
    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_or,NULL,(grib_yyvsp[(1) - (3)].exp),(grib_yyvsp[(3) - (3)].exp));}
    break;

  case 208:

/* Line 1455 of yacc.c  */
#line 658 "griby.y"
    { (grib_yyval.rule_entry) = grib_new_rule_entry(grib_parser_context,(grib_yyvsp[(1) - (4)].str),(grib_yyvsp[(3) - (4)].exp)); free((grib_yyvsp[(1) - (4)].str)); }
    break;

  case 209:

/* Line 1455 of yacc.c  */
#line 659 "griby.y"
    { (grib_yyval.rule_entry) = grib_new_rule_entry(grib_parser_context,"skip",0);}
    break;

  case 211:

/* Line 1455 of yacc.c  */
#line 663 "griby.y"
    { (grib_yyvsp[(1) - (2)].rule_entry)->next = (grib_yyvsp[(2) - (2)].rule_entry); (grib_yyval.rule_entry) = (grib_yyvsp[(1) - (2)].rule_entry); }
    break;

  case 212:

/* Line 1455 of yacc.c  */
#line 666 "griby.y"
    { (grib_yyval.rules) = grib_new_rule(grib_parser_context,NULL,(grib_yyvsp[(1) - (1)].rule_entry)); }
    break;

  case 213:

/* Line 1455 of yacc.c  */
#line 670 "griby.y"
    { (grib_yyval.rules) = grib_new_rule(grib_parser_context,(grib_yyvsp[(3) - (7)].exp),(grib_yyvsp[(6) - (7)].rule_entry)); }
    break;

  case 215:

/* Line 1455 of yacc.c  */
#line 674 "griby.y"
    { (grib_yyvsp[(1) - (2)].rules)->next = (grib_yyvsp[(2) - (2)].rules); (grib_yyval.rules) = (grib_yyvsp[(1) - (2)].rules); }
    break;



/* Line 1455 of yacc.c  */
#line 3554 "y.tab.c"
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
#line 678 "griby.y"


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





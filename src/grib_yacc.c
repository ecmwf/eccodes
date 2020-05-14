/* A Bison parser, made by GNU Bison 3.5.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.5.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 12 "griby.y"


#include "grib_api_internal.h"
/* #include "grib_parser.h" */

extern int grib_yylex(void);
extern int grib_yyerror(const char*);
extern int grib_yylineno;
extern char* file_being_parsed(void);

extern   grib_action*           grib_parser_all_actions;
extern   grib_concept_value*    grib_parser_concept;
extern   grib_hash_array_value*    grib_parser_hash_array;
extern   grib_context*          grib_parser_context;
extern   grib_rule*             grib_parser_rules;

static grib_concept_value* reverse_concept(grib_concept_value* r);
static grib_concept_value *_reverse_concept(grib_concept_value *r,grib_concept_value *s);
static grib_hash_array_value* reverse_hash_array(grib_hash_array_value* r);
static grib_hash_array_value *_reverse_hash_array(grib_hash_array_value *r,grib_hash_array_value *s);

/* typedef int (*testp_proc)(long,long); */
/* typedef long (*grib_op_proc)(long,long);   */



#line 97 "y.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int grib_yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum grib_yytokentype
  {
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
    UINT8 = 283,
    INT8 = 284,
    UINT16 = 285,
    INT16 = 286,
    UINT16_LITTLE_ENDIAN = 287,
    INT16_LITTLE_ENDIAN = 288,
    UINT32 = 289,
    INT32 = 290,
    UINT32_LITTLE_ENDIAN = 291,
    INT32_LITTLE_ENDIAN = 292,
    UINT64 = 293,
    INT64 = 294,
    UINT64_LITTLE_ENDIAN = 295,
    INT64_LITTLE_ENDIAN = 296,
    BLOB = 297,
    BYTE = 298,
    CODETABLE = 299,
    SMART_TABLE = 300,
    DICTIONARY = 301,
    COMPLEX_CODETABLE = 302,
    LOOKUP = 303,
    ALIAS = 304,
    UNALIAS = 305,
    META = 306,
    POS = 307,
    INTCONST = 308,
    TRANS = 309,
    FLAGBIT = 310,
    CONCEPT = 311,
    GETENV = 312,
    HASH_ARRAY = 313,
    CONCEPT_NOFAIL = 314,
    NIL = 315,
    DUMMY = 316,
    MODIFY = 317,
    READ_ONLY = 318,
    STRING_TYPE = 319,
    LONG_TYPE = 320,
    DOUBLE_TYPE = 321,
    NO_COPY = 322,
    DUMP = 323,
    JSON = 324,
    XML = 325,
    NO_FAIL = 326,
    EDITION_SPECIFIC = 327,
    OVERRIDE = 328,
    HIDDEN = 329,
    CAN_BE_MISSING = 330,
    MISSING = 331,
    CONSTRAINT = 332,
    COPY_OK = 333,
    WHEN = 334,
    SET = 335,
    SET_NOFAIL = 336,
    WRITE = 337,
    APPEND = 338,
    PRINT = 339,
    EXPORT = 340,
    REMOVE = 341,
    RENAME = 342,
    SKIP = 343,
    PAD = 344,
    SECTION_PADDING = 345,
    MESSAGE = 346,
    MESSAGE_COPY = 347,
    PADTO = 348,
    PADTOEVEN = 349,
    PADTOMULTIPLE = 350,
    G1_HALF_BYTE = 351,
    G1_MESSAGE_LENGTH = 352,
    G1_SECTION4_LENGTH = 353,
    SECTION_LENGTH = 354,
    LENGTH = 355,
    FLAG = 356,
    ITERATOR = 357,
    NEAREST = 358,
    BOX = 359,
    KSEC = 360,
    ASSERT = 361,
    SUBSTR = 362,
    CASE = 363,
    SWITCH = 364,
    DEFAULT = 365,
    EQ = 366,
    NE = 367,
    GE = 368,
    LE = 369,
    LT = 370,
    GT = 371,
    BIT = 372,
    BITOFF = 373,
    AND = 374,
    OR = 375,
    NOT = 376,
    IS = 377,
    IDENT = 378,
    STRING = 379,
    INTEGER = 380,
    FLOAT = 381
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
#define WHEN 334
#define SET 335
#define SET_NOFAIL 336
#define WRITE 337
#define APPEND 338
#define PRINT 339
#define EXPORT 340
#define REMOVE 341
#define RENAME 342
#define SKIP 343
#define PAD 344
#define SECTION_PADDING 345
#define MESSAGE 346
#define MESSAGE_COPY 347
#define PADTO 348
#define PADTOEVEN 349
#define PADTOMULTIPLE 350
#define G1_HALF_BYTE 351
#define G1_MESSAGE_LENGTH 352
#define G1_SECTION4_LENGTH 353
#define SECTION_LENGTH 354
#define LENGTH 355
#define FLAG 356
#define ITERATOR 357
#define NEAREST 358
#define BOX 359
#define KSEC 360
#define ASSERT 361
#define SUBSTR 362
#define CASE 363
#define SWITCH 364
#define DEFAULT 365
#define EQ 366
#define NE 367
#define GE 368
#define LE 369
#define LT 370
#define GT 371
#define BIT 372
#define BITOFF 373
#define AND 374
#define OR 375
#define NOT 376
#define IS 377
#define IDENT 378
#define STRING 379
#define INTEGER 380
#define FLOAT 381

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 39 "griby.y"

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

#line 419 "y.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE grib_yylval;

int grib_yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */



#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ grib_yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t grib_yytype_int8;
#else
typedef signed char grib_yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ grib_yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t grib_yytype_int16;
#else
typedef short grib_yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ grib_yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t grib_yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char grib_yytype_uint8;
#else
typedef short grib_yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ grib_yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t grib_yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short grib_yytype_uint16;
#else
typedef int grib_yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))

/* Stored state numbers (used for stacks). */
typedef grib_yytype_int16 grib_yy_state_t;

/* State numbers in computations.  */
typedef int grib_yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about grib_yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
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
  grib_yy_state_t grib_yyss_alloc;
  YYSTYPE grib_yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union grib_yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (grib_yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T grib_yynewbytes;                                         \
        YYCOPY (&grib_yyptr->Stack_alloc, Stack, grib_yysize);                    \
        Stack = &grib_yyptr->Stack_alloc;                                    \
        grib_yynewbytes = grib_yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        grib_yyptr += grib_yynewbytes / YYSIZEOF (*grib_yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T grib_yyi;                      \
          for (grib_yyi = 0; grib_yyi < (Count); grib_yyi++)   \
            (Dst)[grib_yyi] = (Src)[grib_yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  214
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1892

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  144
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  50
/* YYNRULES -- Number of rules.  */
#define YYNRULES  267
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  903

#define YYUNDEFTOK  2
#define YYMAXUTOK   381


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by grib_yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK ? grib_yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by grib_yylex.  */
static const grib_yytype_uint8 grib_yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,   142,     2,     2,
     131,   132,   140,   143,   127,   138,   134,   141,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   137,   128,
       2,   133,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   129,     2,   130,   139,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   135,     2,   136,     2,     2,     2,     2,
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
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const grib_yytype_int16 grib_yyrline[] =
{
       0,   262,   262,   264,   265,   266,   267,   269,   273,   276,
     277,   278,   279,   282,   283,   287,   288,   291,   292,   293,
     294,   297,   298,   299,   300,   301,   302,   303,   304,   305,
     309,   310,   313,   314,   317,   318,   321,   325,   328,   331,
     334,   337,   340,   343,   346,   349,   352,   355,   359,   362,
     365,   368,   371,   374,   377,   380,   383,   386,   397,   401,
     404,   407,   410,   413,   416,   419,   422,   425,   428,   431,
     434,   438,   441,   444,   447,   450,   453,   456,   459,   462,
     465,   468,   471,   474,   477,   480,   484,   487,   490,   493,
     496,   498,   501,   504,   507,   510,   513,   516,   519,   522,
     525,   528,   531,   534,   537,   540,   543,   545,   547,   550,
     553,   556,   560,   564,   567,   570,   582,   594,   606,   609,
     612,   614,   617,   620,   621,   622,   623,   625,   628,   629,
     630,   631,   632,   633,   634,   635,   637,   638,   639,   640,
     641,   645,   646,   647,   648,   652,   653,   654,   657,   658,
     661,   662,   666,   667,   670,   671,   674,   675,   678,   679,
     680,   681,   682,   683,   684,   685,   686,   687,   688,   689,
     690,   691,   694,   697,   700,   703,   704,   705,   706,   707,
     708,   709,   710,   711,   712,   713,   714,   715,   716,   717,
     718,   719,   723,   724,   727,   728,   731,   732,   735,   736,
     739,   743,   744,   745,   748,   750,   752,   754,   758,   759,
     762,   763,   767,   769,   773,   774,   775,   776,   779,   780,
     781,   783,   784,   785,   786,   787,   788,   792,   793,   796,
     797,   798,   799,   800,   801,   802,   803,   804,   805,   806,
     807,   810,   811,   812,   815,   817,   818,   819,   820,   821,
     822,   827,   828,   831,   832,   835,   836,   839,   845,   846,
     849,   850,   853,   854,   857,   861,   864,   865
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const grib_yytname[] =
{
  "$end", "error", "$undefined", "LOWERCASE", "IF", "IF_TRANSIENT",
  "ELSE", "END", "CLOSE", "UNSIGNED", "TEMPLATE", "TEMPLATE_NOFAIL",
  "TRIGGER", "ASCII", "GROUP", "NON_ALPHA", "KSEC1EXPVER", "LABEL", "LIST",
  "IS_IN_LIST", "IS_IN_DICT", "IS_INTEGER", "TO_INTEGER", "TO_STRING",
  "SEX2DEC", "WHILE", "IBMFLOAT", "SIGNED", "UINT8", "INT8", "UINT16",
  "INT16", "UINT16_LITTLE_ENDIAN", "INT16_LITTLE_ENDIAN", "UINT32",
  "INT32", "UINT32_LITTLE_ENDIAN", "INT32_LITTLE_ENDIAN", "UINT64",
  "INT64", "UINT64_LITTLE_ENDIAN", "INT64_LITTLE_ENDIAN", "BLOB", "BYTE",
  "CODETABLE", "SMART_TABLE", "DICTIONARY", "COMPLEX_CODETABLE", "LOOKUP",
  "ALIAS", "UNALIAS", "META", "POS", "INTCONST", "TRANS", "FLAGBIT",
  "CONCEPT", "GETENV", "HASH_ARRAY", "CONCEPT_NOFAIL", "NIL", "DUMMY",
  "MODIFY", "READ_ONLY", "STRING_TYPE", "LONG_TYPE", "DOUBLE_TYPE",
  "NO_COPY", "DUMP", "JSON", "XML", "NO_FAIL", "EDITION_SPECIFIC",
  "OVERRIDE", "HIDDEN", "CAN_BE_MISSING", "MISSING", "CONSTRAINT",
  "COPY_OK", "WHEN", "SET", "SET_NOFAIL", "WRITE", "APPEND", "PRINT",
  "EXPORT", "REMOVE", "RENAME", "SKIP", "PAD", "SECTION_PADDING",
  "MESSAGE", "MESSAGE_COPY", "PADTO", "PADTOEVEN", "PADTOMULTIPLE",
  "G1_HALF_BYTE", "G1_MESSAGE_LENGTH", "G1_SECTION4_LENGTH",
  "SECTION_LENGTH", "LENGTH", "FLAG", "ITERATOR", "NEAREST", "BOX", "KSEC",
  "ASSERT", "SUBSTR", "CASE", "SWITCH", "DEFAULT", "EQ", "NE", "GE", "LE",
  "LT", "GT", "BIT", "BITOFF", "AND", "OR", "NOT", "IS", "IDENT", "STRING",
  "INTEGER", "FLOAT", "','", "';'", "'['", "']'", "'('", "')'", "'='",
  "'.'", "'{'", "'}'", "':'", "'-'", "'^'", "'*'", "'/'", "'%'", "'+'",
  "$accept", "all", "empty", "dvalues", "svalues", "integer_array",
  "instructions", "instruction", "semi", "argument_list", "arguments",
  "argument", "simple", "if_block", "when_block", "set", "set_list",
  "default", "flags", "flag_list", "flag", "list_block", "while_block",
  "trigger_block", "concept_block", "concept_list", "hash_array_list",
  "hash_array_block", "case_list", "case_value", "switch_block",
  "concept_value", "concept_conditions", "concept_condition",
  "hash_array_value", "string_or_ident", "atom", "power", "factor", "term",
  "condition", "conjonction", "disjonction", "expression", "rule",
  "rule_entry", "rule_entries", "fact", "conditional_rule", "rules", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const grib_yytype_int16 grib_yytoknum[] =
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
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,    44,    59,    91,
      93,    40,    41,    61,    46,   123,   125,    58,    45,    94,
      42,    47,    37,    43
};
# endif

#define YYPACT_NINF (-654)

#define grib_yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-9)

#define grib_yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const grib_yytype_int16 grib_yypact[] =
{
    1435,  -654,   -24,   -10,     8,    65,   -67,   115,   162,   147,
     186,   202,   204,    64,   200,   213,    82,   245,   249,   253,
     264,   277,   281,   282,   284,   285,   287,   288,   289,   290,
     291,   292,   293,   295,   294,   300,   303,   296,   297,   298,
     308,   311,   312,   315,   316,   318,   319,   320,   258,   321,
     322,   -16,   -12,    61,   323,   847,   305,   324,   325,   327,
     326,   328,   333,   334,   335,   337,   332,   347,   348,   349,
     356,   357,   358,   361,   354,   355,     7,   283,   314,   -79,
     487,  -654,  -654,  1538,   360,  -654,  -654,  -654,  -654,  -654,
    -654,   256,   128,  -654,  -654,  -654,  -654,    18,  -654,  -654,
    -654,  -654,   847,   847,   366,   365,   367,   369,   370,   847,
     371,   157,   362,   372,  -654,  -654,   847,    38,   373,   374,
     362,   362,   362,   362,   362,   362,   362,   362,   362,   362,
     362,   362,   362,   362,   380,   375,   187,   379,   376,   386,
     221,   381,   -47,  -654,   383,   387,   382,   193,   -94,   195,
     377,   847,   390,   398,  -654,   394,  -654,   413,  -654,   251,
     411,   414,   418,   419,  -654,  -654,   420,   422,   847,   423,
    -654,  -654,  -654,   847,   109,  -654,  -654,  -654,   364,   433,
     405,  -654,   -75,   -20,   149,   437,   441,  -654,   434,  -654,
     431,   377,   438,   377,   435,   436,   442,  -654,   439,   443,
     444,   449,   447,   448,   450,   847,   847,   847,   454,   235,
     -76,   445,    66,   451,  -654,   459,     9,   468,  1766,  -654,
    -654,   460,   462,   463,  -654,   464,   469,  -654,   461,   470,
    -654,   473,   474,   481,   471,   485,  -654,  -654,   491,   500,
     847,   847,  -654,   377,   377,   501,   502,   847,   509,   377,
     503,   505,   377,   377,   377,   377,   377,   377,   377,   377,
     377,   377,   377,   377,   377,   377,   847,   510,   512,   527,
     847,   528,   529,   538,   543,   546,   504,   548,   847,   539,
     847,   549,   550,   256,   553,   128,   555,   556,   256,   544,
    -654,  -654,   535,   302,   847,   551,   552,   558,   563,   847,
     557,   559,   575,   578,   260,   149,   568,   570,  -654,  -654,
     847,   -84,   109,   109,   109,   109,   109,   109,    84,    84,
      84,    84,    84,    84,    84,    84,   847,   847,   554,   847,
    -654,   574,  -654,   847,   847,   847,   577,   579,   580,   595,
     847,   847,   847,  -654,   573,   594,   847,   596,   597,   598,
     599,   600,   607,   610,   606,   607,   610,   610,   847,   612,
     377,   610,   847,    11,  -654,   601,   602,   615,   616,   847,
     847,   611,   614,  -654,   624,   627,   617,   262,   619,  -654,
    -654,  -654,   630,   620,   626,   362,  -654,   634,   635,  -654,
    -654,  -654,  -654,  -654,  -654,  -654,  -654,  -654,  -654,  -654,
    -654,  -654,  -654,   629,   637,   638,   640,   633,   643,   644,
     377,   639,  -654,   847,   645,   377,   266,   377,   641,    41,
     -11,   -78,   647,   -54,    87,    55,   -74,  -654,  -654,  -654,
    -654,  -654,  -654,  -654,  -654,  -654,  -654,  -654,  -654,  -654,
    -654,   648,  -654,   -49,  -654,   -36,  -654,  -654,   646,   652,
     653,   654,   649,   655,   656,    94,   658,   657,   665,  -654,
     663,  -654,  -654,  -654,  -654,  -654,  -654,  -654,  -654,  -654,
    -654,   -75,   -75,   -20,   -20,   -20,   -20,   -20,   -20,   149,
     437,   673,   667,   674,   668,   669,   670,   681,   682,   683,
     687,   679,   680,   685,  -654,   636,   686,   847,   847,   847,
     847,   847,  -654,   -56,   690,   677,   610,  -654,    93,   678,
     688,   650,   362,  -654,   689,   694,   695,  1663,  1766,   108,
     215,  1766,   362,   362,   362,   362,  1766,   362,   377,   224,
     236,   362,   238,   847,   946,   362,  1036,   697,  -654,   696,
     699,   698,  -654,  -654,  -654,   -52,  -654,   362,   709,   703,
     717,   256,   377,   719,   377,   722,   706,   727,   256,   377,
     544,   729,   730,   313,   651,  -654,   -35,   -34,  -654,  -654,
    -654,  -654,  -654,   732,   733,   737,  -654,  -654,   739,   740,
    -654,   726,  -654,   377,  -654,  -654,  -654,   723,   738,   362,
     847,  -654,  -654,  -654,   762,   736,   742,   744,   745,   747,
     748,   757,  -654,   788,  -654,  -654,  -654,  -654,  -654,   362,
     377,  -654,   750,   752,    10,   754,   -62,   755,   756,   847,
     377,   847,   377,   758,   377,   377,   377,   377,   760,   377,
    -654,   847,   377,   847,   377,   377,   847,   377,   362,   847,
     362,   377,   847,   362,   847,   377,   362,   847,   271,   377,
     377,   766,   256,    97,   -45,  -654,   770,  -654,   774,   256,
     104,   -27,  -654,  -654,   769,   771,   651,   -63,  -654,   775,
    -654,   759,  -654,   773,   778,   117,   789,   791,  -654,  -654,
     847,   847,  -654,   362,   847,   -72,  -654,  1766,   377,   377,
     377,   362,   362,  -654,   607,   787,   377,  -654,  1766,   -62,
     182,   866,  -654,  -654,   914,   794,  -654,   795,  -654,  -654,
    -654,  -654,  -654,  -654,  -654,  -654,   797,  -654,   798,  -654,
    -654,   799,  -654,   377,   800,   570,   -65,  -654,   802,   377,
     803,  -654,   377,   805,  -654,  -654,  -654,  -654,   807,    39,
     814,   786,   377,   808,   817,    53,   819,   806,   377,   847,
     847,   775,   938,   651,  -654,  -654,  -654,  -654,   820,  -654,
     821,   823,   818,   824,   377,   812,   816,  -654,  -654,   815,
    -654,  -654,  -654,   377,   377,   100,  -654,  -654,   822,   825,
     362,   362,   362,   362,   362,  -654,   362,   828,  -654,   362,
    -654,   377,  -654,   362,   121,   377,   834,   256,  -654,   835,
     145,   377,   836,   256,  -654,  -654,  -654,   829,   775,   837,
     843,   845,  -654,  -654,  -654,  1766,  1306,  -654,  -654,  -654,
     827,  1766,  1766,   377,   377,   377,   377,   377,   377,   856,
     377,  -654,   377,   857,   377,  -654,   858,    57,   859,   863,
     377,  -654,   864,   144,   313,  -654,  -654,  -654,  -654,  -654,
     852,  -654,   853,   854,  -654,  -654,  -654,  -654,  -654,  -654,
     860,  -654,  -654,   151,  -654,   152,   377,   861,   862,  -654,
     159,   377,   -51,  -654,  -654,  -654,   377,   868,   377,   872,
     377,  -654,   377,   377,   873,   377,  -654,  -654,  -654,   865,
    -654,   867,  -654,  -654,  -654,   870,  -654,   377,   377,   377,
    -654,  -654,  -654
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const grib_yytype_int16 grib_yydefact[] =
{
       0,     7,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   129,   133,   140,     0,     8,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     2,     5,    17,     0,    22,    28,    23,    24,    25,
      26,     3,     4,    27,    29,   192,   194,   266,   264,   258,
     259,     6,     0,     0,     0,     0,     0,     0,     0,     8,
       0,     8,     8,     0,    68,    69,     0,     8,     0,     0,
       8,     8,     8,     8,     8,     8,     8,     8,     8,     8,
       8,     8,     8,     8,     0,     0,     0,     0,     0,     0,
       0,   110,     0,    88,     0,     0,     0,     0,     0,     0,
       8,     0,     0,     0,   128,     0,   132,     0,   137,     0,
       0,     0,     0,     0,   221,   222,     0,     0,     0,   215,
     217,   219,   220,     0,     0,    32,   119,    33,    34,   218,
     228,   234,   243,   252,   254,   256,   257,    36,     0,   261,
       0,     8,     0,     8,     0,     0,     0,    95,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     8,     0,     0,
       0,     0,     8,     0,     1,     0,     0,     0,    20,    18,
      21,     0,     0,     0,   193,     0,     0,   195,     0,     0,
     267,     0,     0,     0,     0,     0,   107,   108,     0,     0,
       8,     8,   152,     8,     8,     0,     0,     0,     0,     8,
       0,     0,     8,     8,     8,     8,     8,     8,     8,     8,
       8,     8,     8,     8,     8,     8,     8,     0,     0,     0,
       8,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       8,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     154,   122,     0,     0,     0,     0,     0,     0,     0,     8,
       0,     0,     0,     0,     0,   251,     0,     0,   218,   224,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     8,
     106,     0,   105,     8,     8,     8,     0,     0,     0,     0,
       8,     8,     8,    99,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       8,     0,     0,     0,    19,     0,     0,     0,     0,     0,
       0,     0,     0,   136,     0,     0,     0,     0,     0,   153,
      42,    47,     0,     0,     0,     8,    70,     0,     0,    72,
      71,    74,    73,    76,    75,    78,    77,    80,    79,    82,
      81,    84,    83,     0,     0,     0,     0,     0,     0,     0,
       8,     0,   112,     8,     0,     8,     0,     8,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   159,   168,   158,
     169,   170,   171,   161,   160,   162,   164,   163,   165,   166,
     167,   155,   156,     0,   123,     0,   124,   127,   131,   135,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   225,
       0,   223,    35,   215,   250,   227,   232,   233,   229,   230,
     231,   242,   241,   245,   249,   247,   248,   246,   244,   253,
     255,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   121,     0,     0,     8,     8,     8,
       8,     8,    15,     0,     0,     0,   208,   260,     0,     0,
       0,     0,     8,    92,     0,     0,     0,     0,     0,     8,
       8,     0,     8,     8,     8,     8,     0,     8,     8,     8,
       8,     8,     8,     0,     0,     8,     0,     0,   109,     0,
       0,     0,    89,    11,     9,     0,    90,     8,     0,     0,
       0,     0,     8,     0,     8,     0,     0,     0,     0,     8,
       0,     0,     0,     0,     0,    13,     0,     0,   130,   134,
     139,   138,   118,     0,     0,     0,   240,   235,     0,     0,
     226,     0,   100,     8,   101,   102,   103,     0,     0,     8,
       0,   115,   116,   117,     0,     0,     0,     0,     0,     0,
       0,     0,   213,     0,   205,   209,   212,   204,   206,     8,
       8,   207,     0,     0,     0,     0,   262,     0,     0,     8,
       8,     8,     8,     0,     8,     8,     8,     8,     0,     8,
      86,     8,     8,     8,     8,     8,     8,     8,     8,     8,
       8,     8,     8,     8,     8,     8,     8,     8,     0,     8,
       8,     0,     0,     0,     0,   175,     0,   196,     0,     0,
       0,     0,   184,   157,     0,     0,     0,     0,    30,   145,
     125,     0,   126,     0,     0,     0,     0,     0,   120,   104,
       8,     8,    96,     8,     0,     0,   198,     0,     8,     8,
       8,     8,     8,    16,     0,     0,     8,    93,     0,     0,
       0,   141,   263,   265,   143,     0,    37,     0,    39,   174,
      41,    48,    43,    51,   173,    87,     0,    52,     0,    54,
      85,     0,    49,     8,     0,    36,     8,    60,     0,     8,
       0,   111,     8,     0,    12,    10,    91,    67,     0,     0,
       0,     0,     8,     0,     0,     0,     0,     0,     8,     0,
       0,   150,   146,     0,    31,    14,   236,   237,     0,   238,
       0,     0,     0,     0,     8,     0,     0,   203,   199,     0,
      44,    46,    45,     8,     8,     0,   210,    94,     0,     0,
       8,     8,     8,     8,     8,    57,     8,     0,    56,     8,
      63,     8,   113,     8,     0,     8,     0,     0,   182,     0,
       0,     8,     0,     0,   190,   148,   149,     0,   151,     0,
       0,     0,    97,    98,    65,     0,     0,   172,    61,    62,
       0,     0,     0,     8,     8,     8,     8,     8,     8,     0,
       8,    66,     8,     0,     8,   176,     0,     0,     0,     0,
       8,   185,     0,     0,     0,   239,   214,   216,   200,   202,
       0,   211,     0,     0,    38,    40,    53,    55,    50,    59,
       0,    64,   114,     0,   179,     0,     8,     0,     0,   187,
       0,     8,     0,   201,   142,   144,     8,     0,     8,     0,
       8,   183,     8,     8,     0,     8,   191,   147,    58,     0,
     177,     0,   181,   197,   186,     0,   189,     8,     8,     8,
     178,   180,   188
};

  /* YYPGOTO[NTERM-NUM].  */
static const grib_yytype_int16 grib_yypgoto[] =
{
    -654,  -654,     5,   564,  -654,  -351,     0,  -654,  -653,   242,
    -307,  -184,  -654,  -654,  -654,  -442,   154,   591,   210,  -654,
     440,  -654,  -654,  -654,  -654,  -264,   718,  -654,  -654,   329,
    -654,   -89,  -345,  -654,   -82,  -165,   830,   -15,    80,    -6,
    -160,   691,  -654,   -96,  -654,    14,   392,  -654,  -654,   913
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const grib_yytype_int16 grib_yydefgoto[] =
{
      -1,    80,   290,   545,   567,   503,   615,    83,   669,   176,
     177,   178,    84,    85,    86,   666,   667,   243,   291,   441,
     442,    87,    88,    89,    90,    91,    92,    93,   685,   686,
      94,    95,   505,   506,    96,   179,   180,   181,   182,   183,
     184,   185,   186,   187,    97,   616,   617,    99,   100,   101
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const grib_yytype_int16 grib_yytable[] =
{
      82,   564,   224,   462,   508,    81,   231,   232,   305,   308,
     227,   509,   510,   751,    98,   787,   514,   561,   562,   421,
     246,   343,   228,   167,   426,   208,    57,   208,   208,   561,
     562,   561,   562,   347,   348,   349,   684,   284,   766,   463,
     170,   285,   313,   314,   212,   221,   222,    78,   223,   221,
     222,    78,   223,   355,   213,   292,   107,   350,   552,   356,
     175,   229,   559,   384,   767,   315,   316,   317,   351,   225,
     226,   601,   289,   752,   602,   648,   276,   307,   221,   222,
      78,   223,   554,   219,   649,   887,   563,   277,   565,   543,
     544,   742,   648,   671,   415,   417,   221,   222,    78,   223,
     808,   670,   672,   161,   162,   163,    57,   102,   154,   748,
     344,    98,   156,   354,   175,   155,   242,   242,   318,   157,
     550,   103,   242,   319,   551,   242,   242,   242,   242,   242,
     242,   242,   242,   242,   242,   242,   242,   242,   242,   104,
     209,   229,   363,   700,   164,   165,   464,   308,   308,   308,
     308,   308,   308,   308,   308,   308,   308,   308,   308,   308,
     308,   605,   221,   222,    78,   223,   479,   247,   548,   164,
     165,   241,   248,   549,   511,   795,   221,   222,    78,   223,
     221,   222,    78,   223,   166,   158,   557,   114,   115,   801,
     558,   167,   159,   866,   105,   358,   106,   446,   447,   241,
     359,   161,   162,   163,   347,   348,   349,   169,   170,   171,
     172,   118,   175,   119,   555,   173,   167,   242,   364,   556,
     601,   575,   174,   606,   740,   753,   576,   601,   350,   741,
     820,   746,   169,   170,   171,   172,   747,   619,   108,   351,
     173,   241,   164,   165,   758,   175,   175,   174,   833,   759,
     496,   225,   226,   834,   161,   162,   163,   347,   348,   349,
     320,   321,   322,   323,   324,   325,   515,   221,   222,    78,
     223,   175,   839,   516,   354,   175,   110,   840,   877,   879,
     871,   350,   166,   878,   880,   175,   884,   654,   240,   167,
     241,   885,   351,   109,   661,   164,   165,   465,   466,   467,
     468,   469,   470,   168,   175,   169,   170,   171,   172,   111,
     268,   175,   269,   173,   473,   474,   475,   476,   477,   478,
     174,   161,   162,   163,   281,   112,   286,   282,   283,   287,
     288,   116,   224,   113,   175,   166,   117,   224,   175,   175,
     175,   227,   167,   775,   621,   175,   175,   175,   241,   638,
     640,   238,   643,   631,   273,   274,   168,   241,   169,   170,
     171,   172,   164,   165,   352,   633,   173,   636,   120,   241,
     353,   241,   121,   174,   297,   298,   122,   765,   444,   221,
     222,    78,   223,   457,   458,   522,   523,   123,   739,   151,
     242,   543,   544,   561,   562,   745,   734,   735,   471,   472,
     124,   330,   166,   332,   125,   126,   683,   127,   128,   167,
     129,   130,   131,   132,   133,   134,   210,   137,   175,   140,
     141,   142,   135,   168,   136,   169,   170,   171,   172,   138,
     753,   143,   139,   173,   144,   145,   188,   445,   146,   147,
     174,   148,   149,   150,   152,   153,   160,   211,   190,   345,
     191,   193,   189,   380,   381,   192,   194,   195,   196,   386,
     197,   198,   389,   390,   391,   392,   393,   394,   395,   396,
     397,   398,   399,   400,   401,   402,   199,   200,   201,   202,
     203,   204,   378,   379,   205,   206,   207,   214,   220,   233,
     234,   310,   235,   236,   237,   241,   239,   245,   250,   251,
     267,   271,   175,   175,   175,   175,   175,   695,   403,   266,
     270,   272,   407,   280,   289,   275,   278,   242,   618,   295,
     279,   623,   418,   293,   242,   242,   628,   242,   242,   242,
     242,   294,   242,   837,   242,   242,   242,   242,   296,   843,
     242,   452,   299,   725,   312,   300,   725,   427,   460,   301,
     302,   303,   242,   304,   306,   311,   326,   328,   161,   162,
     163,   327,   329,   331,   336,   224,   333,   334,   337,   338,
     513,   482,   224,   335,   339,   484,   485,   486,   340,   341,
     357,   342,   491,   492,   493,   346,   361,   161,   162,   163,
     362,   212,   369,   365,   242,   366,   213,   367,   428,   164,
     165,   374,   368,   370,   354,   371,   372,   429,   430,   431,
     432,   433,   434,   373,   242,   435,   436,   375,   437,   438,
     538,   439,   440,   376,   175,   542,   175,   546,   164,   165,
     377,   382,   385,   387,   383,   413,   175,   388,   175,   166,
     404,   175,   405,   242,   175,   242,   167,   175,   242,   175,
     224,   242,   175,   805,   806,   540,   224,   406,   408,   409,
     168,   410,   169,   170,   171,   172,   411,   443,   166,   412,
     173,   414,   419,   420,   416,   167,   422,   174,   424,   425,
     453,   481,   454,   448,   449,   175,   175,   769,   242,   168,
     450,   169,   170,   171,   172,   451,   242,   242,   455,   173,
     459,   456,   461,   244,   483,   494,   174,   487,   249,   488,
     489,   252,   253,   254,   255,   256,   257,   258,   259,   260,
     261,   262,   263,   264,   265,   490,   495,   497,   498,   499,
     500,   501,   502,   504,   507,   512,   353,   356,   630,   596,
     597,   598,   599,   600,   352,   355,   517,   519,   224,   518,
     520,   524,   521,   525,   224,   526,   527,   529,   530,   531,
     532,   533,   655,   534,   657,   535,   536,   537,   541,   662,
     568,   594,   539,   547,   553,   560,   569,   570,   571,   668,
     609,   572,   573,   574,   578,   242,   242,   242,   242,   242,
     577,   242,   579,   679,   242,   580,   581,   583,   242,   582,
     584,   585,   586,   360,   587,   588,   589,   161,   162,   163,
     590,   591,   592,   604,   607,   848,   850,   593,   595,   645,
     697,   852,   853,   603,   608,   611,   612,   613,   644,   647,
     706,   646,   708,   651,   710,   711,   712,   713,   652,   715,
     653,   659,   717,   656,   719,   720,   658,   722,   164,   165,
     660,   727,   664,   665,   680,   731,   673,   674,   678,   736,
     737,   705,   675,   707,   676,   677,   161,   162,   163,   681,
     684,   687,   778,   716,   688,   718,   689,   690,   721,   691,
     692,   724,   693,   755,   728,   698,   730,   699,   166,   733,
     701,   703,   704,   738,   709,   167,   714,   743,   770,   771,
     772,   744,   749,   754,   750,   756,   777,   164,   165,   168,
     757,   169,   170,   171,   172,   776,   760,   694,   761,   173,
     779,   797,   762,   763,   780,   781,   174,   782,   783,   784,
     794,   799,   786,   785,   789,   791,   788,   793,   796,   790,
     800,   803,   792,   802,   807,   809,   810,   166,   811,   815,
     812,   817,   798,   816,   167,   851,   813,   821,   804,   829,
     822,   836,   838,   842,   844,   161,   162,   163,   168,   845,
     169,   170,   171,   172,   814,   846,   528,   847,   173,   860,
     863,   865,   867,   818,   819,   174,   868,   870,   873,   874,
     875,   889,   876,   882,   883,   891,   895,   897,   872,   898,
     663,   831,   899,   423,   309,   835,   164,   165,   702,   566,
     230,   841,     0,     0,   768,     0,     0,     0,   480,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   854,   855,   856,   857,   858,   859,     0,
     861,     0,   862,     0,   864,     0,   166,     0,     0,     0,
     869,     0,     0,   167,     0,   161,   162,   163,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   168,     0,   169,
     170,   171,   172,     0,     0,     0,   881,   639,     0,     0,
       0,   886,     0,     0,   174,     0,   888,     0,   890,     0,
     892,     0,   893,   894,     0,   896,   164,   165,     0,     0,
       0,     0,     0,   610,     0,     0,     0,   900,   901,   902,
     620,   622,     0,   624,   625,   626,   627,     0,   629,     0,
     632,   634,   635,   637,     0,     0,   641,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   166,     0,   650,     0,
       0,     0,     0,   167,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   168,     0,   169,
     170,   171,   172,     0,     0,     0,     0,   642,     0,     0,
       0,     0,     0,     0,   174,     0,     0,     0,     0,     0,
     682,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     696,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   723,
       0,   726,     0,     0,   729,     0,     0,   732,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   764,     0,     0,     0,     0,     0,
       0,     0,   773,   774,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     215,     3,     0,     0,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,     0,     0,     0,     0,     0,     0,
       0,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,     0,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,     0,    45,    46,     0,     0,    47,     0,
       0,   823,   824,   825,   826,   827,     0,   828,     0,     0,
     830,     0,     0,     0,   832,    48,    49,    50,    51,    52,
      53,    54,    55,    56,     0,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,     0,    69,    70,    71,
      72,    73,    74,     0,     0,    75,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   216,
       0,     0,   217,     0,     0,    -8,     1,     0,     0,     2,
       3,     0,   849,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,     0,     0,     0,     0,     0,     0,     0,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,     0,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,     0,    45,    46,     0,     0,    47,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,     0,    69,    70,    71,    72,
      73,    74,   215,     3,    75,     0,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,     0,     0,    76,    77,
      78,    79,     0,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,     0,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,     0,    45,    46,     0,     0,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    48,    49,    50,
      51,    52,    53,    54,    55,    56,     0,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,     0,    69,
      70,    71,    72,    73,    74,     0,     0,    75,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   216,     0,     0,   217,     0,   218,   215,     3,     0,
       0,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,     0,     0,     0,     0,     0,     0,     0,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,     0,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
       0,    45,    46,     0,     0,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,     0,    69,    70,    71,    72,    73,    74,
     215,     3,    75,     0,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,     0,     0,   614,     0,     0,   217,
       0,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,     0,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,     0,    45,    46,     0,     0,    47,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    48,    49,    50,    51,    52,
      53,    54,    55,    56,     0,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,     0,    69,    70,    71,
      72,    73,    74,     0,     0,    75,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   216,
       0,     0,   217
};

static const grib_yytype_int16 grib_yycheck[] =
{
       0,   443,    91,   310,   355,     0,   102,   103,   168,   174,
      92,   356,   357,   666,     0,    80,   361,    80,    81,   283,
     116,   205,     4,   107,   288,    18,    88,    18,    18,    80,
      81,    80,    81,    22,    23,    24,   108,   131,   110,   123,
     124,   135,   117,   118,   123,   123,   124,   125,   126,   123,
     124,   125,   126,   129,   133,   151,   123,    46,   136,   135,
      55,   123,   136,   247,   136,   140,   141,   142,    57,   123,
     124,   127,   137,   136,   130,   127,   123,   173,   123,   124,
     125,   126,   136,    83,   136,   136,   135,   134,   124,   125,
     126,   136,   127,   127,   278,   279,   123,   124,   125,   126,
     753,   136,   136,    19,    20,    21,    88,   131,   124,   136,
     206,    97,   124,   209,   109,   131,   111,   112,   138,   131,
     131,   131,   117,   143,   135,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   131,
     133,   123,   133,   133,    60,    61,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   506,   123,   124,   125,   126,   326,   129,   127,    60,
      61,   133,   134,   132,   358,   136,   123,   124,   125,   126,
     123,   124,   125,   126,   100,   124,   131,   123,   124,   136,
     135,   107,   131,   136,   129,   129,   131,   293,   294,   133,
     134,    19,    20,    21,    22,    23,    24,   123,   124,   125,
     126,   129,   207,   131,   127,   131,   107,   212,   218,   132,
     127,   127,   138,   130,   127,   667,   132,   127,    46,   132,
     130,   127,   123,   124,   125,   126,   132,   129,   123,    57,
     131,   133,    60,    61,   127,   240,   241,   138,   127,   132,
     346,   123,   124,   132,    19,    20,    21,    22,    23,    24,
     111,   112,   113,   114,   115,   116,   362,   123,   124,   125,
     126,   266,   127,   369,   370,   270,   129,   132,   127,   127,
     136,    46,   100,   132,   132,   280,   127,   551,   131,   107,
     133,   132,    57,   131,   558,    60,    61,   312,   313,   314,
     315,   316,   317,   121,   299,   123,   124,   125,   126,   123,
     123,   306,   125,   131,   320,   321,   322,   323,   324,   325,
     138,    19,    20,    21,   131,   123,   131,   134,   135,   134,
     135,   131,   421,   129,   329,   100,   123,   426,   333,   334,
     335,   423,   107,   694,   129,   340,   341,   342,   133,   533,
     534,   109,   536,   129,   133,   134,   121,   133,   123,   124,
     125,   126,    60,    61,   129,   129,   131,   129,   123,   133,
     135,   133,   123,   138,   123,   124,   123,   684,    76,   123,
     124,   125,   126,   123,   124,   123,   124,   123,   652,   131,
     385,   125,   126,    80,    81,   659,   125,   126,   318,   319,
     123,   191,   100,   193,   123,   123,   590,   123,   123,   107,
     123,   123,   123,   123,   123,   123,   133,   123,   413,   123,
     123,   123,   129,   121,   129,   123,   124,   125,   126,   129,
     872,   123,   129,   131,   123,   123,   131,   135,   123,   123,
     138,   123,   123,   123,   123,   123,   123,   133,   123,   207,
     123,   123,   128,   243,   244,   129,   123,   123,   123,   249,
     123,   129,   252,   253,   254,   255,   256,   257,   258,   259,
     260,   261,   262,   263,   264,   265,   129,   129,   129,   123,
     123,   123,   240,   241,   123,   131,   131,     0,   128,   123,
     125,   127,   125,   124,   124,   133,   125,   125,   125,   125,
     125,   125,   497,   498,   499,   500,   501,   603,   266,   129,
     131,   125,   270,   131,   137,   134,   133,   512,   518,   125,
     133,   521,   280,   133,   519,   520,   526,   522,   523,   524,
     525,   133,   527,   797,   529,   530,   531,   532,   125,   803,
     535,   299,   131,   639,   139,   131,   642,     3,   306,   131,
     131,   131,   547,   131,   131,   122,   119,   123,    19,    20,
      21,   120,   131,   125,   125,   654,   131,   131,   125,   125,
     360,   329,   661,   131,   125,   333,   334,   335,   131,   131,
     135,   131,   340,   341,   342,   131,   135,    19,    20,    21,
     131,   123,   131,   133,   589,   133,   133,   133,    54,    60,
      61,   130,   133,   133,   700,   132,   132,    63,    64,    65,
      66,    67,    68,   132,   609,    71,    72,   132,    74,    75,
     410,    77,    78,   132,   619,   415,   621,   417,    60,    61,
     130,   130,   123,   130,   132,   131,   631,   132,   633,   100,
     130,   636,   130,   638,   639,   640,   107,   642,   643,   644,
     739,   646,   647,   749,   750,   413,   745,   130,   130,   130,
     121,   123,   123,   124,   125,   126,   123,   132,   100,   123,
     131,   123,   123,   123,   135,   107,   123,   138,   123,   123,
     123,   127,   123,   132,   132,   680,   681,   687,   683,   121,
     132,   123,   124,   125,   126,   132,   691,   692,   123,   131,
     132,   123,   132,   112,   130,   132,   138,   130,   117,   130,
     130,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   130,   132,   131,   131,   131,
     131,   131,   125,   123,   128,   123,   135,   135,   528,   497,
     498,   499,   500,   501,   129,   129,   135,   123,   837,   135,
     123,   132,   135,   123,   843,   135,   130,   123,   123,   130,
     123,   123,   552,   123,   554,   132,   123,   123,   123,   559,
     124,   135,   133,   132,   127,   127,   124,   124,   124,   128,
     130,   132,   127,   127,   127,   780,   781,   782,   783,   784,
     132,   786,   127,   583,   789,   132,   123,   123,   793,   132,
     132,   132,   132,   212,   123,   123,   123,    19,    20,    21,
     123,   132,   132,   136,   136,   815,   816,   132,   132,   123,
     610,   821,   822,   133,   136,   136,   132,   132,   131,   131,
     620,   132,   622,   124,   624,   625,   626,   627,   135,   629,
     123,   135,   632,   124,   634,   635,   124,   637,    60,    61,
     123,   641,   123,   123,   131,   645,   124,   124,   132,   649,
     650,   619,   125,   621,   125,   125,    19,    20,    21,   131,
     108,   135,     6,   631,   132,   633,   132,   132,   636,   132,
     132,   639,   125,   124,   642,   135,   644,   135,   100,   647,
     136,   136,   136,   127,   136,   107,   136,   127,   688,   689,
     690,   127,   133,   128,   133,   132,   696,    60,    61,   121,
     132,   123,   124,   125,   126,   128,   127,   129,   127,   131,
       6,   135,   680,   681,   130,   130,   138,   130,   130,   130,
     123,   123,   132,   723,   132,   132,   726,   132,   124,   729,
     123,   135,   732,   124,     6,   125,   125,   100,   125,   137,
     132,   136,   742,   137,   107,   128,   132,   135,   748,   131,
     135,   127,   127,   127,   135,    19,    20,    21,   121,   132,
     123,   124,   125,   126,   764,   132,   385,   132,   131,   123,
     123,   123,   123,   773,   774,   138,   123,   123,   136,   136,
     136,   123,   132,   132,   132,   123,   123,   132,   844,   132,
     560,   791,   132,   285,   174,   795,    60,    61,   616,   445,
      97,   801,    -1,    -1,   685,    -1,    -1,    -1,   327,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   823,   824,   825,   826,   827,   828,    -1,
     830,    -1,   832,    -1,   834,    -1,   100,    -1,    -1,    -1,
     840,    -1,    -1,   107,    -1,    19,    20,    21,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   121,    -1,   123,
     124,   125,   126,    -1,    -1,    -1,   866,   131,    -1,    -1,
      -1,   871,    -1,    -1,   138,    -1,   876,    -1,   878,    -1,
     880,    -1,   882,   883,    -1,   885,    60,    61,    -1,    -1,
      -1,    -1,    -1,   512,    -1,    -1,    -1,   897,   898,   899,
     519,   520,    -1,   522,   523,   524,   525,    -1,   527,    -1,
     529,   530,   531,   532,    -1,    -1,   535,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   100,    -1,   547,    -1,
      -1,    -1,    -1,   107,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   121,    -1,   123,
     124,   125,   126,    -1,    -1,    -1,    -1,   131,    -1,    -1,
      -1,    -1,    -1,    -1,   138,    -1,    -1,    -1,    -1,    -1,
     589,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     609,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   638,
      -1,   640,    -1,    -1,   643,    -1,    -1,   646,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   683,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   691,   692,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       4,     5,    -1,    -1,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    -1,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    -1,    58,    59,    -1,    -1,    62,    -1,
      -1,   780,   781,   782,   783,   784,    -1,   786,    -1,    -1,
     789,    -1,    -1,    -1,   793,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    -1,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,    -1,   101,   102,   103,
     104,   105,   106,    -1,    -1,   109,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   123,
      -1,    -1,   126,    -1,    -1,     0,     1,    -1,    -1,     4,
       5,    -1,   136,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    -1,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    -1,    58,    59,    -1,    -1,    62,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,    -1,   101,   102,   103,   104,
     105,   106,     4,     5,   109,    -1,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    -1,    -1,   123,   124,
     125,   126,    -1,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    -1,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    -1,    58,    59,    -1,    -1,
      62,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    -1,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,    -1,   101,
     102,   103,   104,   105,   106,    -1,    -1,   109,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   123,    -1,    -1,   126,    -1,   128,     4,     5,    -1,
      -1,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    -1,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      -1,    58,    59,    -1,    -1,    62,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,    -1,   101,   102,   103,   104,   105,   106,
       4,     5,   109,    -1,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    -1,    -1,   123,    -1,    -1,   126,
      -1,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    -1,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    -1,    58,    59,    -1,    -1,    62,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    -1,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,    -1,   101,   102,   103,
     104,   105,   106,    -1,    -1,   109,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   123,
      -1,    -1,   126
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const grib_yytype_uint8 grib_yystos[] =
{
       0,     1,     4,     5,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    58,    59,    62,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   101,
     102,   103,   104,   105,   106,   109,   123,   124,   125,   126,
     145,   146,   150,   151,   156,   157,   158,   165,   166,   167,
     168,   169,   170,   171,   174,   175,   178,   188,   189,   191,
     192,   193,   131,   131,   131,   129,   131,   123,   123,   131,
     129,   123,   123,   129,   123,   124,   131,   123,   129,   131,
     123,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   123,   129,   129,   123,   129,   129,
     123,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   131,   123,   123,   124,   131,   124,   131,   124,   131,
     123,    19,    20,    21,    60,    61,   100,   107,   121,   123,
     124,   125,   126,   131,   138,   146,   153,   154,   155,   179,
     180,   181,   182,   183,   184,   185,   186,   187,   131,   128,
     123,   123,   129,   123,   123,   123,   123,   123,   129,   129,
     129,   129,   123,   123,   123,   123,   131,   131,    18,   133,
     133,   133,   123,   133,     0,     4,   123,   126,   128,   150,
     128,   123,   124,   126,   175,   123,   124,   178,     4,   123,
     193,   187,   187,   123,   125,   125,   124,   124,   153,   125,
     131,   133,   146,   161,   161,   125,   187,   129,   134,   161,
     125,   125,   161,   161,   161,   161,   161,   161,   161,   161,
     161,   161,   161,   161,   161,   161,   129,   125,   123,   125,
     131,   125,   125,   133,   134,   134,   123,   134,   133,   133,
     131,   131,   134,   135,   131,   135,   131,   134,   135,   137,
     146,   162,   187,   133,   133,   125,   125,   123,   124,   131,
     131,   131,   131,   131,   131,   184,   131,   187,   179,   180,
     127,   122,   139,   117,   118,   140,   141,   142,   138,   143,
     111,   112,   113,   114,   115,   116,   119,   120,   123,   131,
     162,   125,   162,   131,   131,   131,   125,   125,   125,   125,
     131,   131,   131,   155,   187,   153,   131,    22,    23,    24,
      46,    57,   129,   135,   187,   129,   135,   135,   129,   134,
     161,   135,   131,   133,   150,   133,   133,   133,   133,   131,
     133,   132,   132,   132,   130,   132,   132,   130,   153,   153,
     162,   162,   130,   132,   155,   123,   162,   130,   132,   162,
     162,   162,   162,   162,   162,   162,   162,   162,   162,   162,
     162,   162,   162,   153,   130,   130,   130,   153,   130,   130,
     123,   123,   123,   131,   123,   155,   135,   155,   153,   123,
     123,   169,   123,   170,   123,   123,   169,     3,    54,    63,
      64,    65,    66,    67,    68,    71,    72,    74,    75,    77,
      78,   163,   164,   132,    76,   135,   187,   187,   132,   132,
     132,   132,   153,   123,   123,   123,   123,   123,   124,   132,
     153,   132,   154,   123,   179,   181,   181,   181,   181,   181,
     181,   182,   182,   183,   183,   183,   183,   183,   183,   184,
     185,   127,   153,   130,   153,   153,   153,   130,   130,   130,
     130,   153,   153,   153,   132,   132,   187,   131,   131,   131,
     131,   131,   125,   149,   123,   176,   177,   128,   149,   176,
     176,   155,   123,   162,   176,   187,   187,   135,   135,   123,
     123,   135,   123,   124,   132,   123,   135,   130,   161,   123,
     123,   130,   123,   123,   123,   132,   123,   123,   162,   133,
     153,   123,   162,   125,   126,   147,   162,   132,   127,   132,
     131,   135,   136,   127,   136,   127,   132,   131,   135,   136,
     127,    80,    81,   135,   159,   124,   147,   148,   124,   124,
     124,   124,   132,   127,   127,   127,   132,   132,   127,   127,
     132,   123,   132,   123,   132,   132,   132,   123,   123,   123,
     123,   132,   132,   132,   135,   132,   153,   153,   153,   153,
     153,   127,   130,   133,   136,   176,   130,   136,   136,   130,
     161,   136,   132,   132,   123,   150,   189,   190,   150,   129,
     161,   129,   161,   150,   161,   161,   161,   161,   150,   161,
     162,   129,   161,   129,   161,   161,   129,   161,   155,   131,
     155,   161,   131,   155,   131,   123,   132,   131,   127,   136,
     161,   124,   135,   123,   169,   162,   124,   162,   124,   135,
     123,   169,   162,   164,   123,   123,   159,   160,   128,   152,
     136,   127,   136,   124,   124,   125,   125,   125,   132,   162,
     131,   131,   161,   155,   108,   172,   173,   135,   132,   132,
     132,   132,   132,   125,   129,   187,   161,   162,   135,   135,
     133,   136,   190,   136,   136,   153,   162,   153,   162,   136,
     162,   162,   162,   162,   136,   162,   153,   162,   153,   162,
     162,   153,   162,   161,   153,   187,   161,   162,   153,   161,
     153,   162,   161,   153,   125,   126,   162,   162,   127,   169,
     127,   132,   136,   127,   127,   169,   127,   132,   136,   133,
     133,   152,   136,   159,   128,   124,   132,   132,   127,   132,
     127,   127,   153,   153,   161,   154,   110,   136,   173,   150,
     162,   162,   162,   161,   161,   149,   128,   162,     6,     6,
     130,   130,   130,   130,   130,   162,   132,    80,   162,   132,
     162,   132,   162,   132,   123,   136,   124,   135,   162,   123,
     123,   136,   124,   135,   162,   187,   187,     6,   152,   125,
     125,   125,   132,   132,   162,   137,   137,   136,   162,   162,
     130,   135,   135,   161,   161,   161,   161,   161,   161,   131,
     161,   162,   161,   127,   132,   162,   127,   169,   127,   127,
     132,   162,   127,   169,   135,   132,   132,   132,   150,   136,
     150,   128,   150,   150,   162,   162,   162,   162,   162,   162,
     123,   162,   162,   123,   162,   123,   136,   123,   123,   162,
     123,   136,   160,   136,   136,   136,   132,   127,   132,   127,
     132,   162,   132,   132,   127,   132,   162,   136,   162,   123,
     162,   123,   162,   162,   162,   123,   162,   132,   132,   132,
     162,   162,   162
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const grib_yytype_uint8 grib_yyr1[] =
{
       0,   144,   145,   145,   145,   145,   145,   145,   146,   147,
     147,   147,   147,   148,   148,   149,   149,   150,   150,   150,
     150,   151,   151,   151,   151,   151,   151,   151,   151,   151,
     152,   152,   153,   153,   154,   154,   155,   156,   156,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   156,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   156,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   156,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   156,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   156,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   156,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   156,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   156,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   156,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   156,   156,
     156,   157,   157,   157,   157,   158,   158,   158,   159,   159,
     160,   160,   161,   161,   162,   162,   163,   163,   164,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     164,   164,   165,   166,   167,   168,   168,   168,   168,   168,
     168,   168,   168,   168,   168,   168,   168,   168,   168,   168,
     168,   168,   169,   169,   170,   170,   171,   171,   172,   172,
     173,   174,   174,   174,   175,   175,   175,   175,   176,   176,
     177,   177,   178,   178,   179,   179,   179,   179,   180,   180,
     180,   180,   180,   180,   180,   180,   180,   181,   181,   182,
     182,   182,   182,   182,   182,   182,   182,   182,   182,   182,
     182,   183,   183,   183,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   185,   185,   186,   186,   187,   188,   188,
     189,   189,   190,   190,   191,   192,   193,   193
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const grib_yytype_int8 grib_yyr2[] =
{
       0,     2,     1,     1,     1,     1,     1,     1,     0,     1,
       3,     1,     3,     1,     3,     1,     3,     1,     2,     3,
       2,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     1,     1,     1,     3,     1,     7,    10,     7,
      10,     7,     4,     7,     7,     7,     7,     4,     7,     7,
      10,     7,     7,    10,     7,    10,     8,     8,    12,    10,
       7,     8,     8,     8,    10,     8,     9,     7,     2,     2,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     7,     6,     7,     2,     5,
       5,     7,     4,     6,     7,     2,     6,     8,     8,     3,
       5,     5,     5,     5,     6,     3,     3,     3,     3,     5,
       2,     7,     4,     8,    10,     5,     5,     5,     5,     2,
       6,     4,     3,     4,     4,     6,     6,     4,     2,     1,
       5,     4,     2,     1,     5,     4,     4,     2,     5,     5,
       1,     7,    11,     7,    11,     6,     7,    11,     4,     4,
       2,     3,     1,     2,     1,     2,     1,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     8,     7,     7,     6,     9,    12,    14,    10,
      14,    12,     8,    11,     6,     9,    12,    10,    14,    12,
       8,    11,     1,     2,     1,     2,     6,    12,     1,     2,
       4,    10,     9,     7,     5,     5,     5,     5,     1,     2,
       4,     6,     5,     5,     8,     1,     8,     1,     1,     1,
       1,     1,     1,     3,     2,     3,     4,     3,     1,     3,
       3,     3,     3,     3,     1,     4,     6,     6,     6,     8,
       4,     3,     3,     1,     3,     3,     3,     3,     3,     3,
       3,     2,     1,     3,     1,     3,     1,     1,     1,     1,
       4,     2,     1,     2,     1,     7,     1,     2
};


#define grib_yyerrok         (grib_yyerrstatus = 0)
#define grib_yyclearin       (grib_yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto grib_yyacceptlab
#define YYABORT         goto grib_yyabortlab
#define YYERROR         goto grib_yyerrorlab


#define YYRECOVERING()  (!!grib_yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (grib_yychar == YYEMPTY)                                        \
      {                                                           \
        grib_yychar = (Token);                                         \
        grib_yylval = (Value);                                         \
        YYPOPSTACK (grib_yylen);                                       \
        grib_yystate = *grib_yyssp;                                         \
        goto grib_yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        grib_yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (grib_yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (grib_yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      grib_yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
grib_yy_symbol_value_print (FILE *grib_yyo, int grib_yytype, YYSTYPE const * const grib_yyvaluep)
{
  FILE *grib_yyoutput = grib_yyo;
  YYUSE (grib_yyoutput);
  if (!grib_yyvaluep)
    return;
# ifdef YYPRINT
  if (grib_yytype < YYNTOKENS)
    YYPRINT (grib_yyo, grib_yytoknum[grib_yytype], *grib_yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (grib_yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
grib_yy_symbol_print (FILE *grib_yyo, int grib_yytype, YYSTYPE const * const grib_yyvaluep)
{
  YYFPRINTF (grib_yyo, "%s %s (",
             grib_yytype < YYNTOKENS ? "token" : "nterm", grib_yytname[grib_yytype]);

  grib_yy_symbol_value_print (grib_yyo, grib_yytype, grib_yyvaluep);
  YYFPRINTF (grib_yyo, ")");
}

/*------------------------------------------------------------------.
| grib_yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
grib_yy_stack_print (grib_yy_state_t *grib_yybottom, grib_yy_state_t *grib_yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; grib_yybottom <= grib_yytop; grib_yybottom++)
    {
      int grib_yybot = *grib_yybottom;
      YYFPRINTF (stderr, " %d", grib_yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (grib_yydebug)                                                  \
    grib_yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
grib_yy_reduce_print (grib_yy_state_t *grib_yyssp, YYSTYPE *grib_yyvsp, int grib_yyrule)
{
  int grib_yylno = grib_yyrline[grib_yyrule];
  int grib_yynrhs = grib_yyr2[grib_yyrule];
  int grib_yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             grib_yyrule - 1, grib_yylno);
  /* The symbols being reduced.  */
  for (grib_yyi = 0; grib_yyi < grib_yynrhs; grib_yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", grib_yyi + 1);
      grib_yy_symbol_print (stderr,
                       grib_yystos[+grib_yyssp[grib_yyi + 1 - grib_yynrhs]],
                       &grib_yyvsp[(grib_yyi + 1) - (grib_yynrhs)]
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (grib_yydebug)                          \
    grib_yy_reduce_print (grib_yyssp, grib_yyvsp, Rule); \
} while (0)

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
#ifndef YYINITDEPTH
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
#   define grib_yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
#  else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
grib_yystrlen (const char *grib_yystr)
{
  YYPTRDIFF_T grib_yylen;
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
static char *
grib_yystpcpy (char *grib_yydest, const char *grib_yysrc)
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
static YYPTRDIFF_T
grib_yytnamerr (char *grib_yyres, const char *grib_yystr)
{
  if (*grib_yystr == '"')
    {
      YYPTRDIFF_T grib_yyn = 0;
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
            else
              goto append;

          append:
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

  if (grib_yyres)
    return grib_yystpcpy (grib_yyres, grib_yystr) - grib_yyres;
  else
    return grib_yystrlen (grib_yystr);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
grib_yysyntax_error (YYPTRDIFF_T *grib_yymsg_alloc, char **grib_yymsg,
                grib_yy_state_t *grib_yyssp, int grib_yytoken)
{
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *grib_yyformat = YY_NULLPTR;
  /* Arguments of grib_yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  char const *grib_yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Actual size of YYARG. */
  int grib_yycount = 0;
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T grib_yysize = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in grib_yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated grib_yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (grib_yytoken != YYEMPTY)
    {
      int grib_yyn = grib_yypact[+*grib_yyssp];
      YYPTRDIFF_T grib_yysize0 = grib_yytnamerr (YY_NULLPTR, grib_yytname[grib_yytoken]);
      grib_yysize = grib_yysize0;
      grib_yyarg[grib_yycount++] = grib_yytname[grib_yytoken];
      if (!grib_yypact_value_is_default (grib_yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int grib_yyxbegin = grib_yyn < 0 ? -grib_yyn : 0;
          /* Stay within bounds of both grib_yycheck and grib_yytname.  */
          int grib_yychecklim = YYLAST - grib_yyn + 1;
          int grib_yyxend = grib_yychecklim < YYNTOKENS ? grib_yychecklim : YYNTOKENS;
          int grib_yyx;

          for (grib_yyx = grib_yyxbegin; grib_yyx < grib_yyxend; ++grib_yyx)
            if (grib_yycheck[grib_yyx + grib_yyn] == grib_yyx && grib_yyx != YYTERROR
                && !grib_yytable_value_is_error (grib_yytable[grib_yyx + grib_yyn]))
              {
                if (grib_yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    grib_yycount = 1;
                    grib_yysize = grib_yysize0;
                    break;
                  }
                grib_yyarg[grib_yycount++] = grib_yytname[grib_yyx];
                {
                  YYPTRDIFF_T grib_yysize1
                    = grib_yysize + grib_yytnamerr (YY_NULLPTR, grib_yytname[grib_yyx]);
                  if (grib_yysize <= grib_yysize1 && grib_yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    grib_yysize = grib_yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (grib_yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        grib_yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    /* Don't count the "%s"s in the final size, but reserve room for
       the terminator.  */
    YYPTRDIFF_T grib_yysize1 = grib_yysize + (grib_yystrlen (grib_yyformat) - 2 * grib_yycount) + 1;
    if (grib_yysize <= grib_yysize1 && grib_yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      grib_yysize = grib_yysize1;
    else
      return 2;
  }

  if (*grib_yymsg_alloc < grib_yysize)
    {
      *grib_yymsg_alloc = 2 * grib_yysize;
      if (! (grib_yysize <= *grib_yymsg_alloc
             && *grib_yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *grib_yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *grib_yyp = *grib_yymsg;
    int grib_yyi = 0;
    while ((*grib_yyp = *grib_yyformat) != '\0')
      if (*grib_yyp == '%' && grib_yyformat[1] == 's' && grib_yyi < grib_yycount)
        {
          grib_yyp += grib_yytnamerr (grib_yyp, grib_yyarg[grib_yyi++]);
          grib_yyformat += 2;
        }
      else
        {
          ++grib_yyp;
          ++grib_yyformat;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
grib_yydestruct (const char *grib_yymsg, int grib_yytype, YYSTYPE *grib_yyvaluep)
{
  YYUSE (grib_yyvaluep);
  if (!grib_yymsg)
    grib_yymsg = "Deleting";
  YY_SYMBOL_PRINT (grib_yymsg, grib_yytype, grib_yyvaluep, grib_yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (grib_yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int grib_yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE grib_yylval;
/* Number of syntax errors so far.  */
int grib_yynerrs;


/*----------.
| grib_yyparse.  |
`----------*/

int
grib_yyparse (void)
{
    grib_yy_state_fast_t grib_yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int grib_yyerrstatus;

    /* The stacks and their tools:
       'grib_yyss': related to states.
       'grib_yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow grib_yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    grib_yy_state_t grib_yyssa[YYINITDEPTH];
    grib_yy_state_t *grib_yyss;
    grib_yy_state_t *grib_yyssp;

    /* The semantic value stack.  */
    YYSTYPE grib_yyvsa[YYINITDEPTH];
    YYSTYPE *grib_yyvs;
    YYSTYPE *grib_yyvsp;

    YYPTRDIFF_T grib_yystacksize;

  int grib_yyn;
  int grib_yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int grib_yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE grib_yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char grib_yymsgbuf[128];
  char *grib_yymsg = grib_yymsgbuf;
  YYPTRDIFF_T grib_yymsg_alloc = sizeof grib_yymsgbuf;
#endif

#define YYPOPSTACK(N)   (grib_yyvsp -= (N), grib_yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int grib_yylen = 0;

  grib_yyssp = grib_yyss = grib_yyssa;
  grib_yyvsp = grib_yyvs = grib_yyvsa;
  grib_yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  grib_yystate = 0;
  grib_yyerrstatus = 0;
  grib_yynerrs = 0;
  grib_yychar = YYEMPTY; /* Cause a token to be read.  */
  goto grib_yysetstate;


/*------------------------------------------------------------.
| grib_yynewstate -- push a new state, which is found in grib_yystate.  |
`------------------------------------------------------------*/
grib_yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  grib_yyssp++;


/*--------------------------------------------------------------------.
| grib_yysetstate -- set current state (the top of the stack) to grib_yystate.  |
`--------------------------------------------------------------------*/
grib_yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", grib_yystate));
  YY_ASSERT (0 <= grib_yystate && grib_yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *grib_yyssp = YY_CAST (grib_yy_state_t, grib_yystate);
  YY_IGNORE_USELESS_CAST_END

  if (grib_yyss + grib_yystacksize - 1 <= grib_yyssp)
#if !defined grib_yyoverflow && !defined YYSTACK_RELOCATE
    goto grib_yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T grib_yysize = grib_yyssp - grib_yyss + 1;

# if defined grib_yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        grib_yy_state_t *grib_yyss1 = grib_yyss;
        YYSTYPE *grib_yyvs1 = grib_yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if grib_yyoverflow is a macro.  */
        grib_yyoverflow (YY_("memory exhausted"),
                    &grib_yyss1, grib_yysize * YYSIZEOF (*grib_yyssp),
                    &grib_yyvs1, grib_yysize * YYSIZEOF (*grib_yyvsp),
                    &grib_yystacksize);
        grib_yyss = grib_yyss1;
        grib_yyvs = grib_yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= grib_yystacksize)
        goto grib_yyexhaustedlab;
      grib_yystacksize *= 2;
      if (YYMAXDEPTH < grib_yystacksize)
        grib_yystacksize = YYMAXDEPTH;

      {
        grib_yy_state_t *grib_yyss1 = grib_yyss;
        union grib_yyalloc *grib_yyptr =
          YY_CAST (union grib_yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (grib_yystacksize))));
        if (! grib_yyptr)
          goto grib_yyexhaustedlab;
        YYSTACK_RELOCATE (grib_yyss_alloc, grib_yyss);
        YYSTACK_RELOCATE (grib_yyvs_alloc, grib_yyvs);
# undef YYSTACK_RELOCATE
        if (grib_yyss1 != grib_yyssa)
          YYSTACK_FREE (grib_yyss1);
      }
# endif

      grib_yyssp = grib_yyss + grib_yysize - 1;
      grib_yyvsp = grib_yyvs + grib_yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, grib_yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (grib_yyss + grib_yystacksize - 1 <= grib_yyssp)
        YYABORT;
    }
#endif /* !defined grib_yyoverflow && !defined YYSTACK_RELOCATE */

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
  if (grib_yypact_value_is_default (grib_yyn))
    goto grib_yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (grib_yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      grib_yychar = grib_yylex ();
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
      if (grib_yytable_value_is_error (grib_yyn))
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
  grib_yystate = grib_yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++grib_yyvsp = grib_yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  grib_yychar = YYEMPTY;
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
| grib_yyreduce -- do a reduction.  |
`-----------------------------*/
grib_yyreduce:
  /* grib_yyn is the number of a rule to reduce with.  */
  grib_yylen = grib_yyr2[grib_yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

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
#line 262 "griby.y"
                          { grib_parser_all_actions = 0;grib_parser_concept=0; 
                            grib_parser_hash_array=0;grib_parser_rules=0; }
#line 2374 "y.tab.c"
    break;

  case 3:
#line 264 "griby.y"
                          { grib_parser_concept     = reverse_concept((grib_yyvsp[0].concept_value)); }
#line 2380 "y.tab.c"
    break;

  case 4:
#line 265 "griby.y"
                             { grib_parser_hash_array     = reverse_hash_array((grib_yyvsp[0].hash_array_value)); }
#line 2386 "y.tab.c"
    break;

  case 5:
#line 266 "griby.y"
                          { grib_parser_all_actions = (grib_yyvsp[0].act); }
#line 2392 "y.tab.c"
    break;

  case 6:
#line 267 "griby.y"
                          { grib_parser_rules       = (grib_yyvsp[0].rules); }
#line 2398 "y.tab.c"
    break;

  case 7:
#line 269 "griby.y"
                      { grib_parser_all_actions = 0; grib_parser_concept=0; 
	                    grib_parser_hash_array=0; grib_parser_rules=0; }
#line 2405 "y.tab.c"
    break;

  case 9:
#line 276 "griby.y"
                  { (grib_yyval.dvalue)=grib_darray_push(grib_parser_context,0,(grib_yyvsp[0].dval));}
#line 2411 "y.tab.c"
    break;

  case 10:
#line 277 "griby.y"
                         { (grib_yyval.dvalue)=grib_darray_push(grib_parser_context,(grib_yyvsp[-2].dvalue),(grib_yyvsp[0].dval));}
#line 2417 "y.tab.c"
    break;

  case 11:
#line 278 "griby.y"
               { (grib_yyval.dvalue)=grib_darray_push(grib_parser_context,0,(grib_yyvsp[0].lval));}
#line 2423 "y.tab.c"
    break;

  case 12:
#line 279 "griby.y"
                           { (grib_yyval.dvalue)=grib_darray_push(grib_parser_context,(grib_yyvsp[-2].dvalue),(grib_yyvsp[0].lval));}
#line 2429 "y.tab.c"
    break;

  case 13:
#line 282 "griby.y"
                 { (grib_yyval.svalue)=grib_sarray_push(grib_parser_context,0,(grib_yyvsp[0].str));}
#line 2435 "y.tab.c"
    break;

  case 14:
#line 283 "griby.y"
                          { (grib_yyval.svalue)=grib_sarray_push(grib_parser_context,(grib_yyvsp[-2].svalue),(grib_yyvsp[0].str));}
#line 2441 "y.tab.c"
    break;

  case 15:
#line 287 "griby.y"
                          { (grib_yyval.ivalue)=grib_iarray_push(0,(grib_yyvsp[0].lval));}
#line 2447 "y.tab.c"
    break;

  case 16:
#line 288 "griby.y"
                                 { (grib_yyval.ivalue)=grib_iarray_push((grib_yyvsp[-2].ivalue),(grib_yyvsp[0].lval));}
#line 2453 "y.tab.c"
    break;

  case 18:
#line 292 "griby.y"
                                    { (grib_yyvsp[-1].act)->next = (grib_yyvsp[0].act); (grib_yyval.act) = (grib_yyvsp[-1].act); }
#line 2459 "y.tab.c"
    break;

  case 19:
#line 293 "griby.y"
                                         { (grib_yyvsp[-2].act)->next = (grib_yyvsp[0].act); (grib_yyval.act) = (grib_yyvsp[-2].act); }
#line 2465 "y.tab.c"
    break;

  case 20:
#line 294 "griby.y"
                            {  (grib_yyval.act) = (grib_yyvsp[-1].act);}
#line 2471 "y.tab.c"
    break;

  case 32:
#line 313 "griby.y"
                           { (grib_yyval.explist) = 0; }
#line 2477 "y.tab.c"
    break;

  case 35:
#line 318 "griby.y"
                                       { (grib_yyvsp[-2].explist)->next = (grib_yyvsp[0].explist); (grib_yyval.explist) = (grib_yyvsp[-2].explist); }
#line 2483 "y.tab.c"
    break;

  case 36:
#line 321 "griby.y"
                          { (grib_yyval.explist) = grib_arguments_new(grib_parser_context,(grib_yyvsp[0].exp),NULL); }
#line 2489 "y.tab.c"
    break;

  case 37:
#line 326 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-2].str),"unsigned",(grib_yyvsp[-4].lval),NULL,(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);        free((grib_yyvsp[-2].str));  }
#line 2495 "y.tab.c"
    break;

  case 38:
#line 329 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-5].str),"unsigned",(grib_yyvsp[-7].lval),(grib_yyvsp[-3].explist),(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);        free((grib_yyvsp[-5].str));  }
#line 2501 "y.tab.c"
    break;

  case 39:
#line 332 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-2].str),"unsigned_bits",(grib_yyvsp[-4].lval),NULL,(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);        free((grib_yyvsp[-2].str));  }
#line 2507 "y.tab.c"
    break;

  case 40:
#line 335 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-5].str),"unsigned_bits",(grib_yyvsp[-7].lval),(grib_yyvsp[-3].explist),(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);        free((grib_yyvsp[-5].str));  }
#line 2513 "y.tab.c"
    break;

  case 41:
#line 338 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-2].str),"ascii",(grib_yyvsp[-4].lval),NULL,(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);  free((grib_yyvsp[-2].str));  }
#line 2519 "y.tab.c"
    break;

  case 42:
#line 341 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-2].str),"group",0,NULL,(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);  free((grib_yyvsp[-2].str));  }
#line 2525 "y.tab.c"
    break;

  case 43:
#line 344 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-5].str),"group",0,(grib_yyvsp[-3].explist),(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);  free((grib_yyvsp[-5].str));  }
#line 2531 "y.tab.c"
    break;

  case 44:
#line 347 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-6].str),"to_integer",0,(grib_yyvsp[-2].explist),0,(grib_yyvsp[0].lval),NULL,NULL);  free((grib_yyvsp[-6].str));  }
#line 2537 "y.tab.c"
    break;

  case 45:
#line 350 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-6].str),"sexagesimal2decimal",0,(grib_yyvsp[-2].explist),0,(grib_yyvsp[0].lval),NULL,NULL);  free((grib_yyvsp[-6].str));  }
#line 2543 "y.tab.c"
    break;

  case 46:
#line 353 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-6].str),"to_string",0,(grib_yyvsp[-2].explist),0,(grib_yyvsp[0].lval),NULL,NULL);  free((grib_yyvsp[-6].str));  }
#line 2549 "y.tab.c"
    break;

  case 47:
#line 356 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-2].str),"non_alpha",0,NULL,(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);  free((grib_yyvsp[-2].str));  }
#line 2555 "y.tab.c"
    break;

  case 48:
#line 360 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-2].str),"ascii",(grib_yyvsp[-4].lval),NULL,(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);  free((grib_yyvsp[-2].str));  }
#line 2561 "y.tab.c"
    break;

  case 49:
#line 363 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-2].str),"bytes",(grib_yyvsp[-4].lval),NULL,(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);      free((grib_yyvsp[-2].str));  }
#line 2567 "y.tab.c"
    break;

  case 50:
#line 366 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-5].str),"bytes",(grib_yyvsp[-7].lval),(grib_yyvsp[-3].explist),(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);      free((grib_yyvsp[-5].str));  }
#line 2573 "y.tab.c"
    break;

  case 51:
#line 369 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-2].str),"ksec1expver",(grib_yyvsp[-4].lval),NULL,(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);  free((grib_yyvsp[-2].str));  }
#line 2579 "y.tab.c"
    break;

  case 52:
#line 372 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-2].str),"signed",(grib_yyvsp[-4].lval),NULL,(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);      free((grib_yyvsp[-2].str));  }
#line 2585 "y.tab.c"
    break;

  case 53:
#line 375 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-5].str),"signed",(grib_yyvsp[-7].lval),(grib_yyvsp[-3].explist),(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);      free((grib_yyvsp[-5].str));  }
#line 2591 "y.tab.c"
    break;

  case 54:
#line 378 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-2].str),"signed_bits",(grib_yyvsp[-4].lval),NULL,(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);      free((grib_yyvsp[-2].str));  }
#line 2597 "y.tab.c"
    break;

  case 55:
#line 381 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-5].str),"signed_bits",(grib_yyvsp[-7].lval),(grib_yyvsp[-3].explist),(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);      free((grib_yyvsp[-5].str));  }
#line 2603 "y.tab.c"
    break;

  case 56:
#line 384 "griby.y"
    { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-3].str),"codetable",(grib_yyvsp[-5].lval), (grib_yyvsp[-2].explist),(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);    free((grib_yyvsp[-3].str)); }
#line 2609 "y.tab.c"
    break;

  case 57:
#line 387 "griby.y"
    {
      /* ECC-485: Set length to 0 and prepend the new argument */
      grib_arguments* a = grib_arguments_new(grib_parser_context, new_accessor_expression(grib_parser_context,(grib_yyvsp[-5].str),0,0),NULL);
      a->next = (grib_yyvsp[-2].explist);
      (grib_yyval.act) = grib_action_create_gen(grib_parser_context, (grib_yyvsp[-3].str), "codetable",
                                  0, a, /* length=0 and additional argument */
                                  (grib_yyvsp[-1].explist), (grib_yyvsp[0].lval), NULL, NULL);
      free((grib_yyvsp[-3].str));
    }
#line 2623 "y.tab.c"
    break;

  case 58:
#line 398 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-7].str),"codetable",(grib_yyvsp[-9].lval), (grib_yyvsp[-6].explist),(grib_yyvsp[-5].explist),(grib_yyvsp[0].lval),NULL,(grib_yyvsp[-2].str));
           free((grib_yyvsp[-7].str));free((grib_yyvsp[-2].str)); }
#line 2630 "y.tab.c"
    break;

  case 59:
#line 402 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-5].str),"codetable",(grib_yyvsp[-7].lval), (grib_yyvsp[-3].explist),(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);    free((grib_yyvsp[-5].str)); }
#line 2636 "y.tab.c"
    break;

  case 60:
#line 405 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-5].str),"smart_table",0,(grib_yyvsp[-3].explist),(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);    free((grib_yyvsp[-5].str)); }
#line 2642 "y.tab.c"
    break;

  case 61:
#line 408 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-7].str),"dictionary",0,(grib_yyvsp[-3].explist),(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);    free((grib_yyvsp[-7].str)); }
#line 2648 "y.tab.c"
    break;

  case 62:
#line 411 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-7].str),"getenv",0,(grib_yyvsp[-3].explist),(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);    free((grib_yyvsp[-7].str)); }
#line 2654 "y.tab.c"
    break;

  case 63:
#line 414 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-3].str),"complex_codetable",(grib_yyvsp[-5].lval), (grib_yyvsp[-2].explist),(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);    free((grib_yyvsp[-3].str)); }
#line 2660 "y.tab.c"
    break;

  case 64:
#line 417 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-5].str),"complex_codetable",(grib_yyvsp[-7].lval), (grib_yyvsp[-3].explist),(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);    free((grib_yyvsp[-5].str)); }
#line 2666 "y.tab.c"
    break;

  case 65:
#line 420 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-3].str),"codeflag",(grib_yyvsp[-5].lval), (grib_yyvsp[-2].explist),(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);  free((grib_yyvsp[-3].str)); }
#line 2672 "y.tab.c"
    break;

  case 66:
#line 423 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-4].str),"lookup",(grib_yyvsp[-6].lval),(grib_yyvsp[-2].explist),NULL,(grib_yyvsp[0].lval),NULL,NULL); free((grib_yyvsp[-4].str)); }
#line 2678 "y.tab.c"
    break;

  case 67:
#line 426 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-5].str),"bit",0,(grib_yyvsp[-3].explist),(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL); free((grib_yyvsp[-5].str)); }
#line 2684 "y.tab.c"
    break;

  case 68:
#line 429 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[0].str),"label",0,NULL,NULL,0,NULL,NULL);   free((grib_yyvsp[0].str));  }
#line 2690 "y.tab.c"
    break;

  case 69:
#line 432 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[0].str),"label",0,NULL,NULL,0,NULL,NULL);   free((grib_yyvsp[0].str));  }
#line 2696 "y.tab.c"
    break;

  case 70:
#line 435 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-2].str),"ibmfloat",4,NULL,(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);free((grib_yyvsp[-2].str));  }
#line 2702 "y.tab.c"
    break;

  case 71:
#line 439 "griby.y"
  { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-2].str),"int8",1,NULL,(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);free((grib_yyvsp[-2].str));  }
#line 2708 "y.tab.c"
    break;

  case 72:
#line 442 "griby.y"
  { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-2].str),"uint8",1,NULL,(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);free((grib_yyvsp[-2].str));  }
#line 2714 "y.tab.c"
    break;

  case 73:
#line 445 "griby.y"
  { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-2].str),"int16",2,NULL,(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);free((grib_yyvsp[-2].str));  }
#line 2720 "y.tab.c"
    break;

  case 74:
#line 448 "griby.y"
  { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-2].str),"uint16",2,NULL,(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);free((grib_yyvsp[-2].str));  }
#line 2726 "y.tab.c"
    break;

  case 75:
#line 451 "griby.y"
  { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-2].str),"int16_little_endian",2,NULL,(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);free((grib_yyvsp[-2].str));  }
#line 2732 "y.tab.c"
    break;

  case 76:
#line 454 "griby.y"
  { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-2].str),"uint16_little_endian",2,NULL,(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);free((grib_yyvsp[-2].str));  }
#line 2738 "y.tab.c"
    break;

  case 77:
#line 457 "griby.y"
  { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-2].str),"int32",4,NULL,(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);free((grib_yyvsp[-2].str));  }
#line 2744 "y.tab.c"
    break;

  case 78:
#line 460 "griby.y"
  { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-2].str),"uint32",4,NULL,(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);free((grib_yyvsp[-2].str));  }
#line 2750 "y.tab.c"
    break;

  case 79:
#line 463 "griby.y"
  { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-2].str),"int32_little_endian",4,NULL,(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);free((grib_yyvsp[-2].str));  }
#line 2756 "y.tab.c"
    break;

  case 80:
#line 466 "griby.y"
  { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-2].str),"uint32_little_endian",4,NULL,(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);free((grib_yyvsp[-2].str));  }
#line 2762 "y.tab.c"
    break;

  case 81:
#line 469 "griby.y"
  { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-2].str),"int64",8,NULL,(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);free((grib_yyvsp[-2].str));  }
#line 2768 "y.tab.c"
    break;

  case 82:
#line 472 "griby.y"
  { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-2].str),"uint64",8,NULL,(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);free((grib_yyvsp[-2].str));  }
#line 2774 "y.tab.c"
    break;

  case 83:
#line 475 "griby.y"
  { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-2].str),"int64_little_endian",8,NULL,(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);free((grib_yyvsp[-2].str));  }
#line 2780 "y.tab.c"
    break;

  case 84:
#line 478 "griby.y"
  { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-2].str),"uint64_little_endian",8,NULL,(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);free((grib_yyvsp[-2].str));  }
#line 2786 "y.tab.c"
    break;

  case 85:
#line 481 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-5].str),"blob",0,(grib_yyvsp[-3].explist),(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);        free((grib_yyvsp[-5].str));  }
#line 2792 "y.tab.c"
    break;

  case 86:
#line 485 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-2].str),"ibmfloat",4,NULL,(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),(grib_yyvsp[-4].str),NULL);free((grib_yyvsp[-2].str)); free((grib_yyvsp[-4].str)); }
#line 2798 "y.tab.c"
    break;

  case 87:
#line 488 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-5].str),"ibmfloat",4,(grib_yyvsp[-3].explist),(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);free((grib_yyvsp[-5].str));  }
#line 2804 "y.tab.c"
    break;

  case 88:
#line 491 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[0].str),"position",0,NULL,NULL,0,NULL,NULL);     free((grib_yyvsp[0].str));  }
#line 2810 "y.tab.c"
    break;

  case 89:
#line 494 "griby.y"
        { (grib_yyval.act) = grib_action_create_variable(grib_parser_context,(grib_yyvsp[-3].str),"constant",0,(grib_yyvsp[-1].explist),NULL,(grib_yyvsp[0].lval),NULL);free((grib_yyvsp[-3].str)); }
#line 2816 "y.tab.c"
    break;

  case 90:
#line 497 "griby.y"
        { (grib_yyval.act) = grib_action_create_variable(grib_parser_context,(grib_yyvsp[-3].str),"transient",0,(grib_yyvsp[-1].explist),(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL);   free((grib_yyvsp[-3].str)); }
#line 2822 "y.tab.c"
    break;

  case 91:
#line 499 "griby.y"
        { (grib_yyval.act) = grib_action_create_transient_darray(grib_parser_context,(grib_yyvsp[-5].str),(grib_yyvsp[-2].dvalue),(grib_yyvsp[0].lval)); free((grib_yyvsp[-5].str)); }
#line 2828 "y.tab.c"
    break;

  case 92:
#line 502 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-2].str),"ieeefloat",4,NULL,(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);   free((grib_yyvsp[-2].str));  }
#line 2834 "y.tab.c"
    break;

  case 93:
#line 505 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-2].str),"ieeefloat",4,NULL,(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),(grib_yyvsp[-4].str),NULL);  free((grib_yyvsp[-2].str));free((grib_yyvsp[-4].str));}
#line 2840 "y.tab.c"
    break;

  case 94:
#line 508 "griby.y"
   { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-5].str),"ieeefloat",4,(grib_yyvsp[-3].explist),(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL,NULL);free((grib_yyvsp[-5].str));  }
#line 2846 "y.tab.c"
    break;

  case 95:
#line 511 "griby.y"
   { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[0].str),"g1_half_byte_codeflag",0,NULL,NULL,0,NULL,NULL);free((grib_yyvsp[0].str));  }
#line 2852 "y.tab.c"
    break;

  case 96:
#line 514 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-1].str),"section_length",(grib_yyvsp[-3].lval),NULL,(grib_yyvsp[0].explist),0,NULL,NULL);free((grib_yyvsp[-1].str));  }
#line 2858 "y.tab.c"
    break;

  case 97:
#line 517 "griby.y"
   { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-3].str),"g1_message_length",(grib_yyvsp[-5].lval),(grib_yyvsp[-1].explist),NULL,0,NULL,NULL);free((grib_yyvsp[-3].str));  }
#line 2864 "y.tab.c"
    break;

  case 98:
#line 520 "griby.y"
  { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-3].str),"g1_section4_length",(grib_yyvsp[-5].lval),(grib_yyvsp[-1].explist),NULL,0,NULL,NULL);free((grib_yyvsp[-3].str));  }
#line 2870 "y.tab.c"
    break;

  case 99:
#line 523 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-1].str),"ksec",0,(grib_yyvsp[0].explist),NULL,0,NULL,NULL);free((grib_yyvsp[-1].str)); }
#line 2876 "y.tab.c"
    break;

  case 100:
#line 526 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-3].str),"pad",0,(grib_yyvsp[-1].explist),0,0,NULL,NULL);   free((grib_yyvsp[-3].str)); }
#line 2882 "y.tab.c"
    break;

  case 101:
#line 529 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-3].str),"padto",0,(grib_yyvsp[-1].explist),0,0,NULL,NULL);   free((grib_yyvsp[-3].str)); }
#line 2888 "y.tab.c"
    break;

  case 102:
#line 532 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-3].str),"padtoeven",0,(grib_yyvsp[-1].explist),0,0,NULL,NULL);   free((grib_yyvsp[-3].str)); }
#line 2894 "y.tab.c"
    break;

  case 103:
#line 535 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-3].str),"padtomultiple",0,(grib_yyvsp[-1].explist),0,0,NULL,NULL);   free((grib_yyvsp[-3].str)); }
#line 2900 "y.tab.c"
    break;

  case 104:
#line 538 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-1].str),"message",(grib_yyvsp[-3].lval),0,0,(grib_yyvsp[0].lval),NULL,NULL);   free((grib_yyvsp[-1].str));  }
#line 2906 "y.tab.c"
    break;

  case 105:
#line 541 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-1].str),"message_copy",0,0,0,(grib_yyvsp[0].lval),NULL,NULL);   free((grib_yyvsp[-1].str));  }
#line 2912 "y.tab.c"
    break;

  case 106:
#line 544 "griby.y"
        { (grib_yyval.act) = grib_action_create_gen(grib_parser_context,(grib_yyvsp[-1].str),"section_padding",0,0,0,(grib_yyvsp[0].lval),NULL,NULL);   free((grib_yyvsp[-1].str));  }
#line 2918 "y.tab.c"
    break;

  case 107:
#line 546 "griby.y"
        { (grib_yyval.act) = grib_action_create_template(grib_parser_context,0,(grib_yyvsp[-1].str),(grib_yyvsp[0].str)); free((grib_yyvsp[-1].str)); free((grib_yyvsp[0].str));}
#line 2924 "y.tab.c"
    break;

  case 108:
#line 548 "griby.y"
    { (grib_yyval.act) = grib_action_create_template(grib_parser_context,1,(grib_yyvsp[-1].str),(grib_yyvsp[0].str)); free((grib_yyvsp[-1].str)); free((grib_yyvsp[0].str));}
#line 2930 "y.tab.c"
    break;

  case 109:
#line 551 "griby.y"
        { (grib_yyval.act) = grib_action_create_alias(grib_parser_context,(grib_yyvsp[-3].str),(grib_yyvsp[-1].str),NULL,(grib_yyvsp[0].lval));  free((grib_yyvsp[-3].str)); free((grib_yyvsp[-1].str)); }
#line 2936 "y.tab.c"
    break;

  case 110:
#line 554 "griby.y"
        { (grib_yyval.act) = grib_action_create_alias(grib_parser_context,(grib_yyvsp[0].str),NULL,NULL,0);  free((grib_yyvsp[0].str)); }
#line 2942 "y.tab.c"
    break;

  case 111:
#line 557 "griby.y"
        {
         (grib_yyval.act) = grib_action_create_alias(grib_parser_context,(grib_yyvsp[-3].str),(grib_yyvsp[-1].str),(grib_yyvsp[-5].str),(grib_yyvsp[0].lval));  free((grib_yyvsp[-5].str)); free((grib_yyvsp[-3].str)); free((grib_yyvsp[-1].str));
    }
#line 2950 "y.tab.c"
    break;

  case 112:
#line 561 "griby.y"
        {
         (grib_yyval.act) = grib_action_create_alias(grib_parser_context,(grib_yyvsp[0].str),NULL,(grib_yyvsp[-2].str),0);  free((grib_yyvsp[-2].str)); free((grib_yyvsp[0].str)); 
    }
#line 2958 "y.tab.c"
    break;

  case 113:
#line 565 "griby.y"
        { (grib_yyval.act) = grib_action_create_meta(grib_parser_context,(grib_yyvsp[-6].str),(grib_yyvsp[-5].str),(grib_yyvsp[-3].explist),(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),NULL); free((grib_yyvsp[-6].str));free((grib_yyvsp[-5].str));}
#line 2964 "y.tab.c"
    break;

  case 114:
#line 568 "griby.y"
    { (grib_yyval.act) = grib_action_create_meta(grib_parser_context,(grib_yyvsp[-6].str),(grib_yyvsp[-5].str),(grib_yyvsp[-3].explist),(grib_yyvsp[-1].explist),(grib_yyvsp[0].lval),(grib_yyvsp[-8].str)); free((grib_yyvsp[-6].str));free((grib_yyvsp[-5].str));free((grib_yyvsp[-8].str));}
#line 2970 "y.tab.c"
    break;

  case 115:
#line 571 "griby.y"
        {
      grib_arguments* a = grib_arguments_new(
        grib_parser_context,
        new_accessor_expression(grib_parser_context,(grib_yyvsp[-3].str),0,0),
		NULL
        );
      a->next=(grib_yyvsp[-1].explist);
      (grib_yyval.act) = grib_action_create_meta(grib_parser_context,
      "ITERATOR","iterator",a,NULL,
      GRIB_ACCESSOR_FLAG_HIDDEN|GRIB_ACCESSOR_FLAG_READ_ONLY,NULL); free((grib_yyvsp[-3].str));
    }
#line 2986 "y.tab.c"
    break;

  case 116:
#line 583 "griby.y"
        {
      grib_arguments* a = grib_arguments_new(
        grib_parser_context,
        new_accessor_expression(grib_parser_context,(grib_yyvsp[-3].str),0,0),
		NULL
        );
      a->next=(grib_yyvsp[-1].explist);
      (grib_yyval.act) = grib_action_create_meta(grib_parser_context,
      "NEAREST","nearest",a,NULL,
      GRIB_ACCESSOR_FLAG_HIDDEN|GRIB_ACCESSOR_FLAG_READ_ONLY,NULL); free((grib_yyvsp[-3].str));
    }
#line 3002 "y.tab.c"
    break;

  case 117:
#line 595 "griby.y"
        {
      grib_arguments* a = grib_arguments_new(
        grib_parser_context,
        new_accessor_expression(grib_parser_context,(grib_yyvsp[-3].str),0,0),
		NULL
        );
      a->next=(grib_yyvsp[-1].explist);
      (grib_yyval.act) = grib_action_create_meta(grib_parser_context,
      "BOX","box",a,NULL,
      GRIB_ACCESSOR_FLAG_HIDDEN|GRIB_ACCESSOR_FLAG_READ_ONLY,NULL); free((grib_yyvsp[-3].str));
    }
#line 3018 "y.tab.c"
    break;

  case 118:
#line 607 "griby.y"
       { (grib_yyval.act) = grib_action_create_put(grib_parser_context,(grib_yyvsp[-3].str),(grib_yyvsp[-1].explist));free((grib_yyvsp[-3].str));}
#line 3024 "y.tab.c"
    break;

  case 119:
#line 610 "griby.y"
       { (grib_yyval.act) = grib_action_create_remove(grib_parser_context,(grib_yyvsp[0].explist));}
#line 3030 "y.tab.c"
    break;

  case 120:
#line 612 "griby.y"
                                     { (grib_yyval.act) = grib_action_create_rename(grib_parser_context,(grib_yyvsp[-3].str),(grib_yyvsp[-1].str));free((grib_yyvsp[-3].str));free((grib_yyvsp[-1].str));}
#line 3036 "y.tab.c"
    break;

  case 121:
#line 615 "griby.y"
       { (grib_yyval.act) = grib_action_create_assert(grib_parser_context,(grib_yyvsp[-1].exp));}
#line 3042 "y.tab.c"
    break;

  case 122:
#line 618 "griby.y"
       { (grib_yyval.act) = grib_action_create_modify(grib_parser_context,(grib_yyvsp[-1].str),(grib_yyvsp[0].lval)); free((grib_yyvsp[-1].str));}
#line 3048 "y.tab.c"
    break;

  case 123:
#line 620 "griby.y"
                          { (grib_yyval.act) = grib_action_create_set_missing(grib_parser_context,(grib_yyvsp[-2].str)); free((grib_yyvsp[-2].str)); }
#line 3054 "y.tab.c"
    break;

  case 124:
#line 621 "griby.y"
                             { (grib_yyval.act) = grib_action_create_set(grib_parser_context,(grib_yyvsp[-2].str),(grib_yyvsp[0].exp),0); free((grib_yyvsp[-2].str)); }
#line 3060 "y.tab.c"
    break;

  case 125:
#line 622 "griby.y"
                                  { (grib_yyval.act) = grib_action_create_set_darray(grib_parser_context,(grib_yyvsp[-4].str),(grib_yyvsp[-1].dvalue)); free((grib_yyvsp[-4].str)); }
#line 3066 "y.tab.c"
    break;

  case 126:
#line 623 "griby.y"
                                  { (grib_yyval.act) = grib_action_create_set_sarray(grib_parser_context,(grib_yyvsp[-4].str),(grib_yyvsp[-1].svalue)); free((grib_yyvsp[-4].str)); }
#line 3072 "y.tab.c"
    break;

  case 127:
#line 625 "griby.y"
                                    { (grib_yyval.act) = grib_action_create_set(grib_parser_context,(grib_yyvsp[-2].str),(grib_yyvsp[0].exp),1); free((grib_yyvsp[-2].str)); }
#line 3078 "y.tab.c"
    break;

  case 128:
#line 628 "griby.y"
                 { (grib_yyval.act) = grib_action_create_write(grib_parser_context,(grib_yyvsp[0].str),0,0); free((grib_yyvsp[0].str));}
#line 3084 "y.tab.c"
    break;

  case 129:
#line 629 "griby.y"
          { (grib_yyval.act) = grib_action_create_write(grib_parser_context,"",0,0); }
#line 3090 "y.tab.c"
    break;

  case 130:
#line 630 "griby.y"
                                 { (grib_yyval.act) = grib_action_create_write(grib_parser_context,(grib_yyvsp[0].str),0,(grib_yyvsp[-2].lval)); free((grib_yyvsp[0].str));}
#line 3096 "y.tab.c"
    break;

  case 131:
#line 631 "griby.y"
                          { (grib_yyval.act) = grib_action_create_write(grib_parser_context,"",0,(grib_yyvsp[-1].lval)); }
#line 3102 "y.tab.c"
    break;

  case 132:
#line 632 "griby.y"
                  { (grib_yyval.act) = grib_action_create_write(grib_parser_context,(grib_yyvsp[0].str),1,0); free((grib_yyvsp[0].str));}
#line 3108 "y.tab.c"
    break;

  case 133:
#line 633 "griby.y"
           { (grib_yyval.act) = grib_action_create_write(grib_parser_context,"",1,0); }
#line 3114 "y.tab.c"
    break;

  case 134:
#line 634 "griby.y"
                                  { (grib_yyval.act) = grib_action_create_write(grib_parser_context,(grib_yyvsp[0].str),1,(grib_yyvsp[-2].lval)); free((grib_yyvsp[0].str));}
#line 3120 "y.tab.c"
    break;

  case 135:
#line 635 "griby.y"
                           { (grib_yyval.act) = grib_action_create_write(grib_parser_context,"",1,(grib_yyvsp[-1].lval)); }
#line 3126 "y.tab.c"
    break;

  case 136:
#line 637 "griby.y"
                        { (grib_yyval.act) = grib_action_create_close(grib_parser_context,(grib_yyvsp[-1].str)); free((grib_yyvsp[-1].str));}
#line 3132 "y.tab.c"
    break;

  case 137:
#line 638 "griby.y"
                 { (grib_yyval.act) = grib_action_create_print(grib_parser_context,(grib_yyvsp[0].str),0); free((grib_yyvsp[0].str)); }
#line 3138 "y.tab.c"
    break;

  case 138:
#line 639 "griby.y"
                                { (grib_yyval.act) = grib_action_create_print(grib_parser_context,(grib_yyvsp[0].str),(grib_yyvsp[-2].str)); free((grib_yyvsp[0].str)); free((grib_yyvsp[-2].str));}
#line 3144 "y.tab.c"
    break;

  case 139:
#line 640 "griby.y"
                               { (grib_yyval.act) = grib_action_create_print(grib_parser_context,(grib_yyvsp[0].str),(grib_yyvsp[-2].str)); free((grib_yyvsp[0].str)); free((grib_yyvsp[-2].str));}
#line 3150 "y.tab.c"
    break;

  case 140:
#line 641 "griby.y"
          { (grib_yyval.act) = grib_action_create_print(grib_parser_context,"",0);  }
#line 3156 "y.tab.c"
    break;

  case 141:
#line 645 "griby.y"
                                             { (grib_yyval.act) = grib_action_create_if(grib_parser_context,(grib_yyvsp[-4].exp),(grib_yyvsp[-1].act),0,0,grib_yylineno,file_being_parsed()); }
#line 3162 "y.tab.c"
    break;

  case 142:
#line 646 "griby.y"
                                                                        { (grib_yyval.act) = grib_action_create_if(grib_parser_context,(grib_yyvsp[-8].exp),(grib_yyvsp[-5].act),(grib_yyvsp[-1].act),0,grib_yylineno,file_being_parsed()); }
#line 3168 "y.tab.c"
    break;

  case 143:
#line 647 "griby.y"
                                                       { (grib_yyval.act) = grib_action_create_if(grib_parser_context,(grib_yyvsp[-4].exp),(grib_yyvsp[-1].act),0,1,grib_yylineno,file_being_parsed()); }
#line 3174 "y.tab.c"
    break;

  case 144:
#line 648 "griby.y"
                                                                                  { (grib_yyval.act) = grib_action_create_if(grib_parser_context,(grib_yyvsp[-8].exp),(grib_yyvsp[-5].act),(grib_yyvsp[-1].act),1,grib_yylineno,file_being_parsed()); }
#line 3180 "y.tab.c"
    break;

  case 145:
#line 652 "griby.y"
                                     { (grib_yyval.act) = grib_action_create_when(grib_parser_context,(grib_yyvsp[-3].exp),(grib_yyvsp[-1].act),NULL); }
#line 3186 "y.tab.c"
    break;

  case 146:
#line 653 "griby.y"
                                               { (grib_yyval.act) = grib_action_create_when(grib_parser_context,(grib_yyvsp[-4].exp),(grib_yyvsp[-1].act),NULL); }
#line 3192 "y.tab.c"
    break;

  case 147:
#line 654 "griby.y"
                                                                   { (grib_yyval.act) = grib_action_create_when(grib_parser_context,(grib_yyvsp[-8].exp),(grib_yyvsp[-5].act),(grib_yyvsp[-1].act)); }
#line 3198 "y.tab.c"
    break;

  case 148:
#line 657 "griby.y"
                               { (grib_yyval.act) = grib_action_create_set(grib_parser_context,(grib_yyvsp[-2].str),(grib_yyvsp[0].exp),0); free((grib_yyvsp[-2].str)); }
#line 3204 "y.tab.c"
    break;

  case 149:
#line 658 "griby.y"
                                    { (grib_yyval.act) = grib_action_create_set(grib_parser_context,(grib_yyvsp[-2].str),(grib_yyvsp[0].exp),1); free((grib_yyvsp[-2].str)); }
#line 3210 "y.tab.c"
    break;

  case 151:
#line 662 "griby.y"
                             { (grib_yyvsp[-2].act)->next = (grib_yyvsp[-1].act); (grib_yyval.act) = (grib_yyvsp[-2].act); }
#line 3216 "y.tab.c"
    break;

  case 152:
#line 666 "griby.y"
                { (grib_yyval.explist) = NULL ;}
#line 3222 "y.tab.c"
    break;

  case 153:
#line 667 "griby.y"
                       { (grib_yyval.explist) = (grib_yyvsp[0].explist) ;}
#line 3228 "y.tab.c"
    break;

  case 154:
#line 670 "griby.y"
                      { (grib_yyval.lval) = 0 ; }
#line 3234 "y.tab.c"
    break;

  case 155:
#line 671 "griby.y"
                      { (grib_yyval.lval) = (grib_yyvsp[0].lval); }
#line 3240 "y.tab.c"
    break;

  case 157:
#line 675 "griby.y"
                        { (grib_yyval.lval) = (grib_yyvsp[-2].lval) | (grib_yyvsp[0].lval); }
#line 3246 "y.tab.c"
    break;

  case 158:
#line 678 "griby.y"
                        { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_READ_ONLY; }
#line 3252 "y.tab.c"
    break;

  case 159:
#line 679 "griby.y"
                           { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_LOWERCASE; }
#line 3258 "y.tab.c"
    break;

  case 160:
#line 680 "griby.y"
                      { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_DUMP; }
#line 3264 "y.tab.c"
    break;

  case 161:
#line 681 "griby.y"
                         { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_NO_COPY; }
#line 3270 "y.tab.c"
    break;

  case 162:
#line 682 "griby.y"
                               { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_NO_FAIL; }
#line 3276 "y.tab.c"
    break;

  case 163:
#line 683 "griby.y"
                        { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_HIDDEN; }
#line 3282 "y.tab.c"
    break;

  case 164:
#line 684 "griby.y"
                        { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_EDITION_SPECIFIC; }
#line 3288 "y.tab.c"
    break;

  case 165:
#line 685 "griby.y"
                        { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_CAN_BE_MISSING; }
#line 3294 "y.tab.c"
    break;

  case 166:
#line 686 "griby.y"
                        { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_CONSTRAINT; }
#line 3300 "y.tab.c"
    break;

  case 167:
#line 687 "griby.y"
                        { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_COPY_OK; }
#line 3306 "y.tab.c"
    break;

  case 168:
#line 688 "griby.y"
                    { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_TRANSIENT; }
#line 3312 "y.tab.c"
    break;

  case 169:
#line 689 "griby.y"
                          { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_STRING_TYPE; }
#line 3318 "y.tab.c"
    break;

  case 170:
#line 690 "griby.y"
                        { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_LONG_TYPE; }
#line 3324 "y.tab.c"
    break;

  case 171:
#line 691 "griby.y"
                        { (grib_yyval.lval) = GRIB_ACCESSOR_FLAG_DOUBLE_TYPE; }
#line 3330 "y.tab.c"
    break;

  case 172:
#line 694 "griby.y"
                                                                { (grib_yyval.act) = grib_action_create_list(grib_parser_context,(grib_yyvsp[-7].str),(grib_yyvsp[-4].exp),(grib_yyvsp[-1].act)); free((grib_yyvsp[-7].str)); }
#line 3336 "y.tab.c"
    break;

  case 173:
#line 697 "griby.y"
                                                            { (grib_yyval.act) = grib_action_create_while(grib_parser_context,(grib_yyvsp[-4].exp),(grib_yyvsp[-1].act));  }
#line 3342 "y.tab.c"
    break;

  case 174:
#line 700 "griby.y"
                                                                   { (grib_yyval.act) = grib_action_create_trigger(grib_parser_context,(grib_yyvsp[-4].explist),(grib_yyvsp[-1].act));  }
#line 3348 "y.tab.c"
    break;

  case 175:
#line 703 "griby.y"
                                                         { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[-4].str),(grib_yyvsp[-2].concept_value),0,0,0,0,0,0,(grib_yyvsp[0].lval),0);  free((grib_yyvsp[-4].str)); }
#line 3354 "y.tab.c"
    break;

  case 176:
#line 704 "griby.y"
                                                            { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[-7].str),(grib_yyvsp[-2].concept_value),0,0,(grib_yyvsp[-5].str),0,0,0,(grib_yyvsp[0].lval),0);  free((grib_yyvsp[-7].str));free((grib_yyvsp[-5].str)); }
#line 3360 "y.tab.c"
    break;

  case 177:
#line 705 "griby.y"
                                                                      { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[-10].str),0,(grib_yyvsp[-6].str),0,(grib_yyvsp[-8].str),(grib_yyvsp[-4].str),(grib_yyvsp[-2].str),0,(grib_yyvsp[0].lval),0);  free((grib_yyvsp[-10].str));free((grib_yyvsp[-6].str));free((grib_yyvsp[-8].str));free((grib_yyvsp[-4].str));free((grib_yyvsp[-2].str)); }
#line 3366 "y.tab.c"
    break;

  case 178:
#line 706 "griby.y"
                                                                                { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[-12].str),0,(grib_yyvsp[-8].str),0,(grib_yyvsp[-10].str),(grib_yyvsp[-6].str),(grib_yyvsp[-4].str),(grib_yyvsp[-2].str),(grib_yyvsp[0].lval),0);  free((grib_yyvsp[-12].str));free((grib_yyvsp[-8].str));free((grib_yyvsp[-10].str));free((grib_yyvsp[-6].str));free((grib_yyvsp[-4].str));free((grib_yyvsp[-2].str)); }
#line 3372 "y.tab.c"
    break;

  case 179:
#line 707 "griby.y"
                                                            { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[-8].str),0,(grib_yyvsp[-4].str),0,(grib_yyvsp[-6].str),(grib_yyvsp[-2].str),0,0,(grib_yyvsp[0].lval),0);  free((grib_yyvsp[-8].str));free((grib_yyvsp[-4].str));free((grib_yyvsp[-6].str));free((grib_yyvsp[-2].str)); }
#line 3378 "y.tab.c"
    break;

  case 180:
#line 708 "griby.y"
                                                                                { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[-10].str),0,(grib_yyvsp[-6].str),(grib_yyvsp[-12].str),(grib_yyvsp[-8].str),(grib_yyvsp[-4].str),(grib_yyvsp[-2].str),0,(grib_yyvsp[0].lval),0);  free((grib_yyvsp[-10].str));free((grib_yyvsp[-6].str));free((grib_yyvsp[-8].str));free((grib_yyvsp[-4].str)); free((grib_yyvsp[-2].str)); free((grib_yyvsp[-12].str));}
#line 3384 "y.tab.c"
    break;

  case 181:
#line 709 "griby.y"
                                                                      { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[-8].str),0,(grib_yyvsp[-4].str),(grib_yyvsp[-10].str),(grib_yyvsp[-6].str),(grib_yyvsp[-2].str),0,0,(grib_yyvsp[0].lval),0);  free((grib_yyvsp[-8].str));free((grib_yyvsp[-4].str));free((grib_yyvsp[-6].str));free((grib_yyvsp[-2].str)); free((grib_yyvsp[-10].str));}
#line 3390 "y.tab.c"
    break;

  case 182:
#line 710 "griby.y"
                                                        { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[-4].str),(grib_yyvsp[-2].concept_value),0,(grib_yyvsp[-6].str),0,0,0,0,(grib_yyvsp[0].lval),0);  free((grib_yyvsp[-6].str));free((grib_yyvsp[-4].str)); }
#line 3396 "y.tab.c"
    break;

  case 183:
#line 711 "griby.y"
                                                                      { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[-7].str),(grib_yyvsp[-2].concept_value),0,(grib_yyvsp[-9].str),(grib_yyvsp[-5].str),0,0,0,(grib_yyvsp[0].lval),0);  free((grib_yyvsp[-9].str));free((grib_yyvsp[-7].str));free((grib_yyvsp[-5].str)); }
#line 3402 "y.tab.c"
    break;

  case 184:
#line 712 "griby.y"
                                                    { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[-4].str),(grib_yyvsp[-2].concept_value),0,0,0,0,0,0,(grib_yyvsp[0].lval),1);  free((grib_yyvsp[-4].str)); }
#line 3408 "y.tab.c"
    break;

  case 185:
#line 713 "griby.y"
                                                                   { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[-7].str),(grib_yyvsp[-2].concept_value),0,0,(grib_yyvsp[-5].str),0,0,0,(grib_yyvsp[0].lval),1);  free((grib_yyvsp[-7].str));free((grib_yyvsp[-5].str)); }
#line 3414 "y.tab.c"
    break;

  case 186:
#line 714 "griby.y"
                                                                             { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[-10].str),0,(grib_yyvsp[-6].str),0,(grib_yyvsp[-8].str),(grib_yyvsp[-4].str),(grib_yyvsp[-2].str),0,(grib_yyvsp[0].lval),1);  free((grib_yyvsp[-10].str));free((grib_yyvsp[-6].str));free((grib_yyvsp[-8].str));free((grib_yyvsp[-4].str));free((grib_yyvsp[-2].str)); }
#line 3420 "y.tab.c"
    break;

  case 187:
#line 715 "griby.y"
                                                                   { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[-8].str),0,(grib_yyvsp[-4].str),0,(grib_yyvsp[-6].str),(grib_yyvsp[-2].str),0,0,(grib_yyvsp[0].lval),1);  free((grib_yyvsp[-8].str));free((grib_yyvsp[-4].str));free((grib_yyvsp[-6].str));free((grib_yyvsp[-2].str)); }
#line 3426 "y.tab.c"
    break;

  case 188:
#line 716 "griby.y"
                                                                                       { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[-10].str),0,(grib_yyvsp[-6].str),(grib_yyvsp[-12].str),(grib_yyvsp[-8].str),(grib_yyvsp[-4].str),(grib_yyvsp[-2].str),0,(grib_yyvsp[0].lval),1);  free((grib_yyvsp[-10].str));free((grib_yyvsp[-6].str));free((grib_yyvsp[-8].str));free((grib_yyvsp[-4].str));free((grib_yyvsp[-2].str)); free((grib_yyvsp[-12].str));}
#line 3432 "y.tab.c"
    break;

  case 189:
#line 717 "griby.y"
                                                                             { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[-8].str),0,(grib_yyvsp[-4].str),(grib_yyvsp[-10].str),(grib_yyvsp[-6].str),(grib_yyvsp[-2].str),0,0,(grib_yyvsp[0].lval),1);  free((grib_yyvsp[-8].str));free((grib_yyvsp[-4].str));free((grib_yyvsp[-6].str));free((grib_yyvsp[-2].str)); free((grib_yyvsp[-10].str));}
#line 3438 "y.tab.c"
    break;

  case 190:
#line 718 "griby.y"
                                                               { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[-4].str),(grib_yyvsp[-2].concept_value),0,(grib_yyvsp[-6].str),0,0,0,0,(grib_yyvsp[0].lval),1);  free((grib_yyvsp[-6].str));free((grib_yyvsp[-4].str)); }
#line 3444 "y.tab.c"
    break;

  case 191:
#line 719 "griby.y"
                                                                             { (grib_yyval.act) = grib_action_create_concept(grib_parser_context,(grib_yyvsp[-7].str),(grib_yyvsp[-2].concept_value),0,(grib_yyvsp[-9].str),(grib_yyvsp[-5].str),0,0,0,(grib_yyvsp[0].lval),1);  free((grib_yyvsp[-9].str));free((grib_yyvsp[-7].str));free((grib_yyvsp[-5].str)); }
#line 3450 "y.tab.c"
    break;

  case 193:
#line 724 "griby.y"
                                          { (grib_yyval.concept_value) = (grib_yyvsp[0].concept_value); (grib_yyvsp[0].concept_value)->next = (grib_yyvsp[-1].concept_value);   }
#line 3456 "y.tab.c"
    break;

  case 195:
#line 728 "griby.y"
                                                { (grib_yyval.hash_array_value) = (grib_yyvsp[0].hash_array_value); (grib_yyvsp[0].hash_array_value)->next = (grib_yyvsp[-1].hash_array_value);   }
#line 3462 "y.tab.c"
    break;

  case 196:
#line 731 "griby.y"
                                                                  { (grib_yyval.act) = grib_action_create_hash_array(grib_parser_context,(grib_yyvsp[-4].str),(grib_yyvsp[-2].hash_array_value),0,0,0,0,0,0,(grib_yyvsp[0].lval),0);  free((grib_yyvsp[-4].str)); }
#line 3468 "y.tab.c"
    break;

  case 197:
#line 732 "griby.y"
                                                                         { (grib_yyval.act) = grib_action_create_hash_array(grib_parser_context,(grib_yyvsp[-10].str),0,(grib_yyvsp[-6].str),0,(grib_yyvsp[-8].str),(grib_yyvsp[-4].str),(grib_yyvsp[-2].str),0,(grib_yyvsp[0].lval),0);  free((grib_yyvsp[-10].str));free((grib_yyvsp[-6].str));free((grib_yyvsp[-8].str));free((grib_yyvsp[-4].str));free((grib_yyvsp[-2].str)); }
#line 3474 "y.tab.c"
    break;

  case 199:
#line 736 "griby.y"
                                    { (grib_yyval.case_value) = (grib_yyvsp[0].case_value); (grib_yyvsp[0].case_value)->next = (grib_yyvsp[-1].case_value);   }
#line 3480 "y.tab.c"
    break;

  case 200:
#line 739 "griby.y"
                                               { (grib_yyval.case_value) = grib_case_new(grib_parser_context,(grib_yyvsp[-2].explist),(grib_yyvsp[0].act));  }
#line 3486 "y.tab.c"
    break;

  case 201:
#line 743 "griby.y"
                                                                           { (grib_yyval.act) = grib_action_create_switch(grib_parser_context,(grib_yyvsp[-7].explist),(grib_yyvsp[-4].case_value),(grib_yyvsp[-1].act)); }
#line 3492 "y.tab.c"
    break;

  case 202:
#line 744 "griby.y"
                                                               { (grib_yyval.act) = grib_action_create_switch(grib_parser_context,(grib_yyvsp[-6].explist),(grib_yyvsp[-3].case_value),grib_action_create_noop(grib_parser_context,"continue")); }
#line 3498 "y.tab.c"
    break;

  case 203:
#line 745 "griby.y"
                                                   { (grib_yyval.act) = grib_action_create_switch(grib_parser_context,(grib_yyvsp[-4].explist),(grib_yyvsp[-1].case_value),0); }
#line 3504 "y.tab.c"
    break;

  case 204:
#line 748 "griby.y"
                                                         {
	  				(grib_yyval.concept_value) = grib_concept_value_new(grib_parser_context,(grib_yyvsp[-4].str),(grib_yyvsp[-1].concept_condition)); free((grib_yyvsp[-4].str));}
#line 3511 "y.tab.c"
    break;

  case 205:
#line 750 "griby.y"
                                                                       {
	  				(grib_yyval.concept_value) = grib_concept_value_new(grib_parser_context,(grib_yyvsp[-4].str),(grib_yyvsp[-1].concept_condition)); free((grib_yyvsp[-4].str));}
#line 3518 "y.tab.c"
    break;

  case 206:
#line 752 "griby.y"
                                                                         {
					char buf[80]; sprintf(buf,"%ld",(long)(grib_yyvsp[-4].lval)); (grib_yyval.concept_value) = grib_concept_value_new(grib_parser_context,buf,(grib_yyvsp[-1].concept_condition));}
#line 3525 "y.tab.c"
    break;

  case 207:
#line 754 "griby.y"
                                                                       {
					char buf[80]; sprintf(buf,"%g",(double)(grib_yyvsp[-4].dval)); (grib_yyval.concept_value) = grib_concept_value_new(grib_parser_context,buf,(grib_yyvsp[-1].concept_condition));}
#line 3532 "y.tab.c"
    break;

  case 209:
#line 759 "griby.y"
                                                       { (grib_yyvsp[-1].concept_condition)->next = (grib_yyvsp[0].concept_condition); (grib_yyval.concept_condition) = (grib_yyvsp[-1].concept_condition); }
#line 3538 "y.tab.c"
    break;

  case 210:
#line 762 "griby.y"
                                               { (grib_yyval.concept_condition) = grib_concept_condition_new(grib_parser_context,(grib_yyvsp[-3].str),(grib_yyvsp[-1].exp),0); free((grib_yyvsp[-3].str)); }
#line 3544 "y.tab.c"
    break;

  case 211:
#line 763 "griby.y"
                                               { (grib_yyval.concept_condition) = grib_concept_condition_new(grib_parser_context,(grib_yyvsp[-5].str),0,(grib_yyvsp[-2].ivalue)); free((grib_yyvsp[-5].str)); }
#line 3550 "y.tab.c"
    break;

  case 212:
#line 767 "griby.y"
                                                     {
	  				(grib_yyval.hash_array_value) = grib_integer_hash_array_value_new(grib_parser_context,(grib_yyvsp[-4].str),(grib_yyvsp[-1].ivalue)); free((grib_yyvsp[-4].str));}
#line 3557 "y.tab.c"
    break;

  case 213:
#line 769 "griby.y"
                                                                  {
	  				(grib_yyval.hash_array_value) = grib_integer_hash_array_value_new(grib_parser_context,(grib_yyvsp[-4].str),(grib_yyvsp[-1].ivalue)); free((grib_yyvsp[-4].str));}
#line 3564 "y.tab.c"
    break;

  case 214:
#line 773 "griby.y"
                                                                { (grib_yyval.exp) = new_accessor_expression(grib_parser_context,(grib_yyvsp[-5].str),(grib_yyvsp[-3].lval),(grib_yyvsp[-1].lval));  free((grib_yyvsp[-5].str)); }
#line 3570 "y.tab.c"
    break;

  case 215:
#line 774 "griby.y"
                                                                          { (grib_yyval.exp) = new_accessor_expression(grib_parser_context,(grib_yyvsp[0].str),0,0); free((grib_yyvsp[0].str)); }
#line 3576 "y.tab.c"
    break;

  case 216:
#line 775 "griby.y"
                                                                 { (grib_yyval.exp) = new_sub_string_expression(grib_parser_context,(grib_yyvsp[-5].str),(grib_yyvsp[-3].lval),(grib_yyvsp[-1].lval));  free((grib_yyvsp[-5].str)); }
#line 3582 "y.tab.c"
    break;

  case 217:
#line 776 "griby.y"
                          { (grib_yyval.exp) = new_string_expression(grib_parser_context,(grib_yyvsp[0].str));  free((grib_yyvsp[0].str)); }
#line 3588 "y.tab.c"
    break;

  case 219:
#line 780 "griby.y"
                { (grib_yyval.exp) = new_long_expression(grib_parser_context,(grib_yyvsp[0].lval));  }
#line 3594 "y.tab.c"
    break;

  case 220:
#line 781 "griby.y"
              { (grib_yyval.exp) = new_double_expression(grib_parser_context,(grib_yyvsp[0].dval));  /* TODO: change to new_float_expression*/}
#line 3600 "y.tab.c"
    break;

  case 221:
#line 783 "griby.y"
              { (grib_yyval.exp) = NULL; }
#line 3606 "y.tab.c"
    break;

  case 222:
#line 784 "griby.y"
                    { (grib_yyval.exp) = new_true_expression(grib_parser_context); }
#line 3612 "y.tab.c"
    break;

  case 223:
#line 785 "griby.y"
                           { (grib_yyval.exp) = (grib_yyvsp[-1].exp); }
#line 3618 "y.tab.c"
    break;

  case 224:
#line 786 "griby.y"
                 { (grib_yyval.exp) = new_unop_expression(grib_parser_context,&grib_op_neg,&grib_op_neg_d,(grib_yyvsp[0].exp)); }
#line 3624 "y.tab.c"
    break;

  case 225:
#line 787 "griby.y"
                    { (grib_yyval.exp) = new_func_expression(grib_parser_context,(grib_yyvsp[-2].str),NULL); free((grib_yyvsp[-2].str));}
#line 3630 "y.tab.c"
    break;

  case 226:
#line 788 "griby.y"
                                  { (grib_yyval.exp) = new_func_expression(grib_parser_context,(grib_yyvsp[-3].str),(grib_yyvsp[-1].explist)); free((grib_yyvsp[-3].str));}
#line 3636 "y.tab.c"
    break;

  case 227:
#line 792 "griby.y"
                                    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_pow,NULL,(grib_yyvsp[-2].exp),(grib_yyvsp[0].exp)); }
#line 3642 "y.tab.c"
    break;

  case 229:
#line 796 "griby.y"
                                     { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_mul,&grib_op_mul_d,(grib_yyvsp[-2].exp),(grib_yyvsp[0].exp)); }
#line 3648 "y.tab.c"
    break;

  case 230:
#line 797 "griby.y"
                                     { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_div,&grib_op_div_d,(grib_yyvsp[-2].exp),(grib_yyvsp[0].exp)); }
#line 3654 "y.tab.c"
    break;

  case 231:
#line 798 "griby.y"
                                     { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_modulo,NULL,(grib_yyvsp[-2].exp),(grib_yyvsp[0].exp)); }
#line 3660 "y.tab.c"
    break;

  case 232:
#line 799 "griby.y"
                                  { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_bit,NULL,(grib_yyvsp[-2].exp),(grib_yyvsp[0].exp)); }
#line 3666 "y.tab.c"
    break;

  case 233:
#line 800 "griby.y"
                                  { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_bitoff,NULL,(grib_yyvsp[-2].exp),(grib_yyvsp[0].exp)); }
#line 3672 "y.tab.c"
    break;

  case 235:
#line 802 "griby.y"
                                   { (grib_yyval.exp) = new_length_expression(grib_parser_context,(grib_yyvsp[-1].str)); free((grib_yyvsp[-1].str));}
#line 3678 "y.tab.c"
    break;

  case 236:
#line 803 "griby.y"
                                                  { (grib_yyval.exp) = new_is_in_list_expression(grib_parser_context,(grib_yyvsp[-3].str),(grib_yyvsp[-1].str)); free((grib_yyvsp[-3].str));free((grib_yyvsp[-1].str));}
#line 3684 "y.tab.c"
    break;

  case 237:
#line 804 "griby.y"
                                                  { (grib_yyval.exp) = new_is_in_dict_expression(grib_parser_context,(grib_yyvsp[-3].str),(grib_yyvsp[-1].str)); free((grib_yyvsp[-3].str));free((grib_yyvsp[-1].str));}
#line 3690 "y.tab.c"
    break;

  case 238:
#line 805 "griby.y"
                                                   { (grib_yyval.exp) = new_is_integer_expression(grib_parser_context,(grib_yyvsp[-3].str),(grib_yyvsp[-1].lval),0); free((grib_yyvsp[-3].str));}
#line 3696 "y.tab.c"
    break;

  case 239:
#line 806 "griby.y"
                                                               { (grib_yyval.exp) = new_is_integer_expression(grib_parser_context,(grib_yyvsp[-5].str),(grib_yyvsp[-3].lval),(grib_yyvsp[-1].lval)); free((grib_yyvsp[-5].str));}
#line 3702 "y.tab.c"
    break;

  case 240:
#line 807 "griby.y"
                                       { (grib_yyval.exp) = new_is_integer_expression(grib_parser_context,(grib_yyvsp[-1].str),0,0); free((grib_yyvsp[-1].str));}
#line 3708 "y.tab.c"
    break;

  case 241:
#line 810 "griby.y"
                                    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_add,&grib_op_add_d,(grib_yyvsp[-2].exp),(grib_yyvsp[0].exp)); }
#line 3714 "y.tab.c"
    break;

  case 242:
#line 811 "griby.y"
                                    { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_sub,&grib_op_sub_d,(grib_yyvsp[-2].exp),(grib_yyvsp[0].exp)); }
#line 3720 "y.tab.c"
    break;

  case 244:
#line 815 "griby.y"
                                     { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_gt,&grib_op_gt_d,(grib_yyvsp[-2].exp),(grib_yyvsp[0].exp)); }
#line 3726 "y.tab.c"
    break;

  case 245:
#line 817 "griby.y"
                                     { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_eq,&grib_op_eq_d,(grib_yyvsp[-2].exp),(grib_yyvsp[0].exp)); }
#line 3732 "y.tab.c"
    break;

  case 246:
#line 818 "griby.y"
                                     { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_lt,&grib_op_lt_d,(grib_yyvsp[-2].exp),(grib_yyvsp[0].exp)); }
#line 3738 "y.tab.c"
    break;

  case 247:
#line 819 "griby.y"
                                     { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_ge,&grib_op_ge_d,(grib_yyvsp[-2].exp),(grib_yyvsp[0].exp)); }
#line 3744 "y.tab.c"
    break;

  case 248:
#line 820 "griby.y"
                                     { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_le,&grib_op_le_d,(grib_yyvsp[-2].exp),(grib_yyvsp[0].exp)); }
#line 3750 "y.tab.c"
    break;

  case 249:
#line 821 "griby.y"
                                     { (grib_yyval.exp) = new_binop_expression(grib_parser_context,&grib_op_ne,&grib_op_ne_d,(grib_yyvsp[-2].exp),(grib_yyvsp[0].exp)); }
#line 3756 "y.tab.c"
    break;

  case 250:
#line 822 "griby.y"
                                                  { (grib_yyval.exp) = new_string_compare_expression(grib_parser_context,(grib_yyvsp[-2].exp),(grib_yyvsp[0].exp)); }
#line 3762 "y.tab.c"
    break;

  case 251:
#line 827 "griby.y"
                                      { (grib_yyval.exp) = new_unop_expression(grib_parser_context,&grib_op_not,NULL,(grib_yyvsp[0].exp)); }
#line 3768 "y.tab.c"
    break;

  case 253:
#line 831 "griby.y"
                                        { (grib_yyval.exp) = new_logical_and_expression(grib_parser_context,(grib_yyvsp[-2].exp),(grib_yyvsp[0].exp)); }
#line 3774 "y.tab.c"
    break;

  case 255:
#line 835 "griby.y"
                                            { (grib_yyval.exp) = new_logical_or_expression(grib_parser_context,(grib_yyvsp[-2].exp),(grib_yyvsp[0].exp));}
#line 3780 "y.tab.c"
    break;

  case 260:
#line 849 "griby.y"
                                       { (grib_yyval.rule_entry) = grib_new_rule_entry(grib_parser_context,(grib_yyvsp[-3].str),(grib_yyvsp[-1].exp)); free((grib_yyvsp[-3].str)); }
#line 3786 "y.tab.c"
    break;

  case 261:
#line 850 "griby.y"
                       { (grib_yyval.rule_entry) = grib_new_rule_entry(grib_parser_context,"skip",0);}
#line 3792 "y.tab.c"
    break;

  case 263:
#line 854 "griby.y"
                                       { (grib_yyvsp[-1].rule_entry)->next = (grib_yyvsp[0].rule_entry); (grib_yyval.rule_entry) = (grib_yyvsp[-1].rule_entry); }
#line 3798 "y.tab.c"
    break;

  case 264:
#line 857 "griby.y"
                  { (grib_yyval.rules) = grib_new_rule(grib_parser_context,NULL,(grib_yyvsp[0].rule_entry)); }
#line 3804 "y.tab.c"
    break;

  case 265:
#line 861 "griby.y"
                                                             { (grib_yyval.rules) = grib_new_rule(grib_parser_context,(grib_yyvsp[-4].exp),(grib_yyvsp[-1].rule_entry)); }
#line 3810 "y.tab.c"
    break;

  case 267:
#line 865 "griby.y"
                   { (grib_yyvsp[-1].rules)->next = (grib_yyvsp[0].rules); (grib_yyval.rules) = (grib_yyvsp[-1].rules); }
#line 3816 "y.tab.c"
    break;


#line 3820 "y.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter grib_yychar, and that requires
     that grib_yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of grib_yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering grib_yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", grib_yyr1[grib_yyn], &grib_yyval, &grib_yyloc);

  YYPOPSTACK (grib_yylen);
  grib_yylen = 0;
  YY_STACK_PRINT (grib_yyss, grib_yyssp);

  *++grib_yyvsp = grib_yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int grib_yylhs = grib_yyr1[grib_yyn] - YYNTOKENS;
    const int grib_yyi = grib_yypgoto[grib_yylhs] + *grib_yyssp;
    grib_yystate = (0 <= grib_yyi && grib_yyi <= YYLAST && grib_yycheck[grib_yyi] == *grib_yyssp
               ? grib_yytable[grib_yyi]
               : grib_yydefgoto[grib_yylhs]);
  }

  goto grib_yynewstate;


/*--------------------------------------.
| grib_yyerrlab -- here on detecting error.  |
`--------------------------------------*/
grib_yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  grib_yytoken = grib_yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (grib_yychar);

  /* If not already recovering from an error, report this error.  */
  if (!grib_yyerrstatus)
    {
      ++grib_yynerrs;
#if ! YYERROR_VERBOSE
      grib_yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR grib_yysyntax_error (&grib_yymsg_alloc, &grib_yymsg, \
                                        grib_yyssp, grib_yytoken)
      {
        char const *grib_yymsgp = YY_("syntax error");
        int grib_yysyntax_error_status;
        grib_yysyntax_error_status = YYSYNTAX_ERROR;
        if (grib_yysyntax_error_status == 0)
          grib_yymsgp = grib_yymsg;
        else if (grib_yysyntax_error_status == 1)
          {
            if (grib_yymsg != grib_yymsgbuf)
              YYSTACK_FREE (grib_yymsg);
            grib_yymsg = YY_CAST (char *, YYSTACK_ALLOC (YY_CAST (YYSIZE_T, grib_yymsg_alloc)));
            if (!grib_yymsg)
              {
                grib_yymsg = grib_yymsgbuf;
                grib_yymsg_alloc = sizeof grib_yymsgbuf;
                grib_yysyntax_error_status = 2;
              }
            else
              {
                grib_yysyntax_error_status = YYSYNTAX_ERROR;
                grib_yymsgp = grib_yymsg;
              }
          }
        grib_yyerror (grib_yymsgp);
        if (grib_yysyntax_error_status == 2)
          goto grib_yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
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
  /* Pacify compilers when the user code never invokes YYERROR and the
     label grib_yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
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
  grib_yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      grib_yyn = grib_yypact[grib_yystate];
      if (!grib_yypact_value_is_default (grib_yyn))
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

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++grib_yyvsp = grib_yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


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


#if !defined grib_yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| grib_yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
grib_yyexhaustedlab:
  grib_yyerror (YY_("memory exhausted"));
  grib_yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| grib_yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
grib_yyreturn:
  if (grib_yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      grib_yytoken = YYTRANSLATE (grib_yychar);
      grib_yydestruct ("Cleanup: discarding lookahead",
                  grib_yytoken, &grib_yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (grib_yylen);
  YY_STACK_PRINT (grib_yyss, grib_yyssp);
  while (grib_yyssp != grib_yyss)
    {
      grib_yydestruct ("Cleanup: popping",
                  grib_yystos[+*grib_yyssp], grib_yyvsp);
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
  return grib_yyresult;
}
#line 869 "griby.y"


static grib_concept_value *_reverse_concept(grib_concept_value *r,grib_concept_value *s)
{
    grib_concept_value *v;

    if(r == NULL) return s;

    v         = r->next;
    r->next   = s;
    return _reverse_concept(v,r);
}

static grib_concept_value* reverse_concept(grib_concept_value* r)
{
    return _reverse_concept(r,NULL);
}

static grib_hash_array_value *_reverse_hash_array(grib_hash_array_value *r,grib_hash_array_value *s)
{
    grib_hash_array_value *v;

    if(r == NULL) return s;

    v         = r->next;
    r->next   = s;
    return _reverse_hash_array(v,r);
}


static grib_hash_array_value* reverse_hash_array(grib_hash_array_value* r)
{
    return _reverse_hash_array(r,NULL);
}




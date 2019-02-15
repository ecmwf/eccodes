/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"

/*

Rename all yy* functions so more that one yacc output can be liked
See http://www.delorie.com/gnu/docs/automake/automake_48.html

*/


#define	yymaxdepth grib_filter_maxdepth
#define	yyparse	grib_filter_parse
#define	yylex	grib_filter_lex
#define	yyerror	grib_filter_error
#define	yylval	grib_filter_lval
#define	yychar	grib_filter_char
#define	yydebug	grib_filter_debug
#define	yypact	grib_filter_pact
#define	yyr1	grib_filter_r1
#define	yyr2	grib_filter_r2
#define	yydef	grib_filter_def
#define	yychk	grib_filter_chk
#define	yypgo	grib_filter_pgo
#define	yyact	grib_filter_act
#define	yyexca	grib_filter_exca
#define yyerrflag grib_filter_errflag
#define yynerrs	grib_filter_nerrs
#define	yyps	grib_filter_ps
#define	yypv	grib_filter_pv
#define	yys	grib_filter_s
#define	yy_yys	grib_filter_yys
#define	yystate	grib_filter_state
#define	yytmp	grib_filter_tmp
#define	yyv	grib_filter_v
#define	yy_yyv	grib_filter_yyv
#define	yyval	grib_filter_val
#define	yylloc	grib_filter_lloc
#define yyreds	grib_filter_reds
#define yytoks	grib_filter_toks
#define yylhs	grib_filter_yylhs
#define yylen	grib_filter_yylen
#define yydefred grib_filter_yydefred
#define yydgoto	grib_filter_yydgoto
#define yysindex grib_filter_yysindex
#define yyrindex grib_filter_yyrindex
#define yygindex grib_filter_yygindex
#define yytable	 grib_filter_yytable
#define yycheck	 grib_filter_yycheck
#define yyname   grib_filter_yyname
#define yyrule   grib_filter_yyrule

#define MAX_KEYS 40
#define MAX_RULES 40
#define MAX_SET  40

typedef struct key_value key_value;
struct key_value {
	char* key; 
	grib_runtime_type value;
}

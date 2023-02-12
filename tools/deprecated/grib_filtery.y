%{
#include "grib_filter.h"
#include "grib_filtery.h"
#include <stdio.h>
extern int yylex();
extern int yylineno;
extern int grib_filter_irule;
extern key_value grib_filter_to_set[MAX_RULES][MAX_SET];
extern char* grib_filter_keys[MAX_RULES][MAX_KEYS];
extern long grib_filter_values[MAX_RULES][MAX_KEYS];
extern int grib_filter_nkey[MAX_RULES];
extern int grib_filter_nset[MAX_RULES];
extern int grib_filter_verbose;


int grib_filter_wrap(void) {return 1;}

void yyerror(const char* msg) {printf("%s at line %d\n",msg,yylineno);}

%}

%union {
    char     *str;
    long     lval;
	double     dval;
};

%start all

%token IF
%token ELSE
%token END
%token FLOAT
%token SET


%token EQ
%token NE
%token GE
%token LE

%token AND
%token OR
%token NOT

%token <str>IDENT
%token <str>STRING

%token <lval>INTEGER
%token <dval>FLOAT


%%

all : instructions
    | if_blocks
	| all semi
	;

semi : ';'
     | semi ';'
	 ;


set : IDENT '=' INTEGER { 	
							grib_filter_to_set[grib_filter_irule][grib_filter_nset[grib_filter_irule]].key=$1;
							grib_runtime_type* rtt=grib_runtime_type_new(NULL);
							grib_runtime_type_set_long(rtt,$3);
							grib_filter_to_set[grib_filter_irule][grib_filter_nset[grib_filter_irule]].value=rtt;
							grib_filter_nset[grib_filter_irule]++;
						} 
		| IDENT '=' FLOAT { 	
							grib_filter_to_set[grib_filter_irule][grib_filter_nset[grib_filter_irule]].key=$1;
							grib_runtime_type* rtt=grib_runtime_type_new(NULL);
							grib_runtime_type_set_double(rtt,$3);
							grib_filter_to_set[grib_filter_irule][grib_filter_nset[grib_filter_irule]].value=rtt;
							grib_filter_nset[grib_filter_irule]++;
						} 
		| IDENT '=' STRING { 	
							grib_filter_to_set[grib_filter_irule][grib_filter_nset[grib_filter_irule]].key=$1;
							grib_runtime_type* rtt=grib_runtime_type_new(NULL);
							grib_runtime_type_set_char(rtt,$3,strlen($3));
							grib_filter_to_set[grib_filter_irule][grib_filter_nset[grib_filter_irule]].value=rtt;
							grib_filter_nset[grib_filter_irule]++;
						}
   ; 
   
instruction : set semi
	 ;

instructions : instruction
             | instruction instructions
			 ;

if_blocks : if_block
			| if_block if_blocks

if_block : 
  IF '(' condition ')' '{' instructions '}' {grib_filter_irule++;}
   ; 
   

comparison  : IDENT EQ INTEGER 	{ 	
									grib_filter_keys[grib_filter_irule][grib_filter_nkey[grib_filter_irule]]=$1;
									grib_filter_values[grib_filter_irule][grib_filter_nkey[grib_filter_irule]]=$3;
									grib_filter_nkey[grib_filter_irule]++;
								}
            ;

condition   : comparison
            | condition AND condition {;}
            ;


%%

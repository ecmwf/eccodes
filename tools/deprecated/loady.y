%{

#include "grib_api.h"
#include <string.h>

extern int yylex();
extern void yyerror(const char*);

#include "load.h"




%}

%union {
    char                  *str;
    long                  lval;
    double                dval;
};

%start all

%token MISSING;

%token <str>IDENT
%token <str>STRING

%token <lval>INTEGER
%token <dval>FLOAT

%%

all        : empty        
           | message
           ;

empty:;

messages : message
         | messages message
         ;

message : IDENT '{' accessors '}' ';'  { load_finish(); }
        ; 

accessors : accessor
    | accessors accessor
    ;

accessor : IDENT '=' INTEGER ';'     { load_long($1,$3);   }
         | IDENT '=' '-' INTEGER ';' { load_long($1,-$4);  }
         | IDENT '=' STRING  ';'     { load_string($1,$3); }
         | IDENT '=' FLOAT  ';'      { load_double($1,$3); }
         | IDENT '=' MISSING  ';'    { load_missing($1);  }
         | IDENT '=' '-' FLOAT  ';'  { load_double($1,-$4); }
		 | IDENT '=' '{' {load_start_array();} values ',' '}'  ';' 
		 	{ load_end_array($1); free($1); }

         ;

values: value 
	  | values ',' value 
	  ;

value : INTEGER      { load_long_value($1);  }
      | '-' INTEGER  { load_long_value(-$2); }
	  | FLOAT        { load_double_value($1);  }
	  | '-' FLOAT    { load_double_value(-$2); }
	  ;

%%

%{
#include "lib_global.h"
int liberror();
int liblex();
int libdebug ;
extern int LIB_LINE;

%}


%union {
    char                                        *stringtype;
    struct chain                                *chainlist;
    struct { char *ident; struct chain *param;} head_struct;
};


%token <stringtype>        _NUMBER
%token <stringtype>        _STRING
%token <stringtype>        _IDENT
%token                     _LPAR
%token                     _RPAR
%token                     _LCURLY
%token                     _RCURLY
%token                     _SEMI
%token                     _COLON
%token                     _COMMA
%token <stringtype>        _OPE

%start test

%type  <stringtype>         primary operateur make_expression 
%type  <stringtype>         expression attr_val 
%type  <chainlist>          param_list
%type  <head_struct>        head

%%
 
/*file	: group */
/*		;       */


test : {yydebug=0;} group ;

group	: head _LCURLY
        {
            lib_treat_group_head($1.ident, $1.param); 
        }
        
        statements _RCURLY
        {
            lib_treat_group_end();
        }
        
        | head _LCURLY
        {
            lib_treat_group_head($1.ident, $1.param);      
        }
        
        _RCURLY
		{
            lib_treat_group_end();
        }
        ;


statements 	: statement
		 	| statements statement
			;


statement 	: simple_attr
			| complex_attr
			| group
			;

simple_attr	: _IDENT _COLON attr_val _SEMI 
            {
                lib_treat_s_attr($1, $3);
            }

            
			| _IDENT _COLON attr_val 
            {
                lib_treat_s_attr($1, $3);
            }
			;

complex_attr 	: head  _SEMI 
                {
                    lib_treat_c_attr($1.ident, $1.param);
                }
                | head  
                {
                    lib_treat_c_attr($1.ident, $1.param);
                }
				;

head	: _IDENT _LPAR param_list _RPAR
        { 
            $$.ident=$1;
            $$.param=reverse($3);
        }
        | _IDENT _LPAR _RPAR

        {
           $$.ident=$1;
           $$.param=NULL;
        }
        ;


param_list  : attr_val
            { 
                $$ = addchain(NULL,$1);
            } 
            | param_list _COMMA attr_val
            { 
                $$ = addchain($1,$3); 
            }
			;

attr_val : _STRING { $$ = lib_unquote($1);}
         | expression { $$ = $1;}
		 ;
        
expression : make_expression
            {
            $$ = $1;
            }
            ;
            
make_expression    : operateur primary 
                  { 
                  char concat[1024];
                  strcpy(concat, $1);
                  strcat(concat, $2);
                  $$ = mbkstrdup(concat);
                  mbkfree($2);
                  }
              | make_expression _OPE primary 
                  {
                  char concat[1024];
                  strcpy(concat, $1);
                  strcat(concat, $2);
                  strcat(concat, $3);
                  $$ = mbkstrdup(concat);
                  mbkfree($1); mbkfree($2); mbkfree($3);
                  }
              | operateur _LPAR make_expression _RPAR 
                  {
                  char concat[1024];
                  strcpy(concat, $1);
                  strcat(concat, "(");
                  strcat(concat, $3);
                  strcat(concat, ")");
                  $$ = mbkstrdup(concat);
                  mbkfree($1);  mbkfree($3);
                  }
              | make_expression _OPE _LPAR make_expression _RPAR 
                  {
                  char concat[1024];
                  strcpy(concat, $1);
                  strcat(concat, $2);
                  strcat(concat, "(");
                  strcat(concat, $4);
                  strcat(concat, ")");
                  $$ = mbkstrdup(concat);
                  mbkfree($1); mbkfree($2); mbkfree($4);
                  }
              ;
              
operateur    :          { $$ = ""; }
             | _OPE     { $$ = $1; }
             ;
             
primary   : _IDENT    {$$ = $1;}  
          | _NUMBER   {$$ = $1;} 

          ;

%%




int liberror ()
{
    fprintf (stderr, "LIBERTY ERROR: parse error line %d\n", LIB_LINE) ;
    return 0;
}


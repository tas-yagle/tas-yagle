/******************************************************************************/
/*                                                                            */
/*                      Chaine de CAO & VLSI   AVERTEC                        */
/*                                                                            */
/*    Produit : VCD Version 1.00                                              */
/*    Fichier : vcd.y                                                         */
/*                                                                            */
/*    (c) copyright 2000 AVERTEC                                              */
/*    Tous droits reserves                                                    */
/*                                                                            */
/*    Auteur(s) : Gilles Augustins                                            */
/*                                                                            */
/******************************************************************************/

%{
#include VCD_H
#include "vcd_parse.h"


/******************************************************************************/
/* function declarations                                                      */
/******************************************************************************/

int yyerror ();
int yylex ();


%}

%union {
  char  *txt;
  float  unit;
};

%token <txt> _STRING
%token <txt> _LINE
%token <txt> _RANGE          
%token <txt> _INDEX          
%token       _END           
%token       _TIMESCALE     
%token       _SCOPE           
%token       _VAR           
%token       _TIME          
%token       _CHAR
%token       _PS
%token       _NS
%token       _US
%token       _MS
%token       _S

%start vcd_file 

%type <txt>  ident

%%

vcd_file    : timescale scope_list changes 
            {
            }
            ;

timescale   : _TIMESCALE _STRING _STRING _END
            {
                vcd_set_ts (atol ($2), vcd_TimeUnit ($3));
                mbkfree ($2);
            }
            | _TIMESCALE _STRING _END
            {
                int value;
                char *tunit;
                value = strtod($2, &tunit);
                vcd_set_ts (value, vcd_TimeUnit (tunit));
                mbkfree ($2);
            }
            ;

scope_list  : /* empty */ 
            {
            }
            | scope_list scope var_list 
            {
            }
            ;

scope       : _SCOPE _LINE
            {
                vcd_getmodule ($2);
                mbkfree ($2);
            }
            ;
            
var_list    : /* empty */ 
            {
            }
            | var_list var 
            {
            }
            ;

var         : _VAR _STRING _STRING _STRING ident _END 
            {
                vcd_addvar ($5, $4);
                mbkfree ($2);
                mbkfree ($3);
                mbkfree ($4);
            }
            ;

ident       : _STRING 
            {
                $$ = namealloc ($1);
                mbkfree ($1);
            }
            | _STRING _RANGE 
            {
                char buf[1024];
                sprintf (buf, "%s%s", $1, $2);
                $$ = namealloc (buf);
                mbkfree ($1);
                mbkfree ($2);
            }
            | _STRING _INDEX 
            {
                char buf[1024];
                sprintf (buf, "%s%s", $1, $2);
                $$ = namealloc (buf);
                mbkfree ($1);
                mbkfree ($2);
            }
            ;
            
line_list   : /* empty */ 
            {
            }
            | line_list _LINE 
            {
                vcd_parseline ($2);
                mbkfree ($2);
            }
            ;
            
changes     : line_list
            {
            }
            ;

%%

extern char stbtext[] ;

int yyerror ()
{
	fprintf (stderr, "[WScan] Parse error line %d\n", vcd_line);
    exit (EXIT_FAILURE);
}


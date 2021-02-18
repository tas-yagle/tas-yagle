/************************************************************/
/*                         slib.y                           */
/************************************************************/

/************************************************************/
/*                       declaration                        */
/************************************************************/
%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "slib_error.h"
#include SLB_H

library     *SLIB_LIBRARY;
symbol_list *SLIB_CURRENT_SYMBOL;

draw_list   *FUNC_TION;
line        *U_LINE;
arc         *U_ARC;
circle      *U_CIRCLE;
pin         *U_PIN;

int yydebug;
int yyerror();
int yylex();
extern int slibParseLine  ;
extern eqt_ctx *slibEqtCtx;

int id;

%}

/************************************************************/
/*                    union pour yylval                     */
/************************************************************/
%union {
    char    *string_type;
    float   float_type;
    int     int_type;
    double  double_type;
    };



/************************************************************/
/*                         token                            */
/************************************************************/
%token <string_type> EGAL SEMI COLON COMMA 
%token <string_type> LPAR RPAR LCURLY RCURLY
%token LIB SYMBOL SYMBNAME LAYER SUB_SYMB AREA ANNOTATE
%token LINE ARC CIRCLE PIN T_GRID
%token <string_type> NUM
%token <string_type> STRING
%token <string_type> OPE

%type <string_type> expression_list make_expression operateur primary function choice
%type <double_type> expression 
%%

file    :  statements
          {
            slibEqtCtx->EQT_RES_CALC = EQT_COMPLETE;
            if(!eqt_getvar (slibEqtCtx,namealloc("AND_HEIGHT")))
                slib_error(ERR_NO_REF_HEIGHT, slibParseLine,NULL);

            SLIB_LIBRARY->REFHEIGHT = eqt_getvar(slibEqtCtx,namealloc("AND_HEIGHT"));
                            
            SLIB_LIBRARY->SYMB = (symbol_list *)reverse((chain_list *)SLIB_LIBRARY->SYMB);
          }
        ;

group   : head LCURLY statements RCURLY 
          {}
        | head LCURLY  RCURLY 
          {}
        ;
            
statements 	: statement 
              {}
		 	| statements statement  
              {}
			;

statement 	: group  {}
            | egality SEMI{}
            | function SEMI { mbkfree($1); }
            | canonicalname {}
            | define SEMI {}
            | draw SEMI {}
            | call SEMI {}
            | useless SEMI {}
            | T_GRID LPAR choice RPAR SEMI 
            { 
              slib_set_lib_grid( SLIB_LIBRARY, eqt_eval(slibEqtCtx,namealloc($3),EQTFAST) );
             
              if(eqt_resistrue(slibEqtCtx) != EQT_COMPLETE)
                  slib_error(ERR_NO_GRID, slibParseLine,$3);
            }
            ;

choice  : NUM     { $$=$1; }
        | STRING  { $$=$1; }
        ;

egality     : STRING EGAL expression  
            {
              if(eqt_resistrue(slibEqtCtx))
                  eqt_addvar(slibEqtCtx,namealloc($1),$3); 
              else
                  slib_error(ERR_NO_EVAL, slibParseLine,$1); 
            }
            ;

define      : STRING COLON expression {}
            ;

canonicalname : SYMBNAME COLON STRING SEMI 
               {}
              ;
               
ident   : LIB      
          { 
          id =1; 
          slib_set_function(slibEqtCtx);
          }
        | SYMBOL   
          { id =2; }
        | LAYER    
          { id =0; }
        | ANNOTATE 
          { id =0; }
        ;

head	: ident LPAR STRING RPAR  
            { 
            if (id ==1){
                  SLIB_LIBRARY = slib_create_lib($3) ;
                }
            if (id ==2){
                  SLIB_CURRENT_SYMBOL = slib_add_symbol(SLIB_LIBRARY,$3);
                }
            }
        | ident LPAR RPAR {}
		;

call        : SUB_SYMB LPAR STRING COMMA expression COMMA expression COMMA expression RPAR 
              {
                if(!slib_getsymbol(SLIB_LIBRARY, $3))
                    slib_error(ERR_NO_SYMBOL, slibParseLine,$3);
                    
                slib_add_subsymbol(SLIB_LIBRARY,SLIB_CURRENT_SYMBOL,$3,$5,$7,$9);
              }
            ;

useless : AREA LPAR expression_list RPAR 
        { mbkfree($3); }
        ;

draw    : LINE   LPAR expression COMMA expression COMMA expression COMMA expression RPAR 
          {
          slib_add_draw_line(SLIB_CURRENT_SYMBOL,$3,$5,$7,$9,SLIB_LIBRARY->GRID);
          }
        | ARC    LPAR expression COMMA expression COMMA expression COMMA expression COMMA expression COMMA expression RPAR 
          {
          slib_add_draw_arc(SLIB_CURRENT_SYMBOL,$3,$5,$7,$9,$11,$13,SLIB_LIBRARY->GRID);
          }
        | CIRCLE LPAR expression COMMA expression COMMA expression RPAR 
          {
          slib_add_draw_circle(SLIB_CURRENT_SYMBOL,$3,$5,$7,SLIB_LIBRARY->GRID);
          }
        | PIN    LPAR STRING COMMA expression COMMA expression COMMA STRING RPAR 
          {
          int pin_direction;
          
          if( !strcasecmp($9,"UP") )
            pin_direction = SLIB_PIN_UP ;
          else if ( !strcasecmp($9,"DOWN") )
            pin_direction = SLIB_PIN_DOWN ;
          else if ( !strcasecmp($9,"LEFT") )
            pin_direction = SLIB_PIN_LEFT ;
          else if ( !strcasecmp($9,"RIGHT") )
            pin_direction = SLIB_PIN_RIGHT ;
          else
            slib_error(ERR_PIN_DIR, slibParseLine,$9);
            
          slib_add_draw_pin(SLIB_CURRENT_SYMBOL,$3,$5,$7,pin_direction,SLIB_LIBRARY->GRID);
          }
        ;


function    : STRING LPAR expression_list RPAR 
            {
            char concat[1024];
            strcpy(concat, $1);
            strcat(concat, "(");
            strcat(concat, $3);
            strcat(concat, ")");
            $$ = mbkstrdup(concat);
            //printf("### fonction crée:\t%s\n",$$);
            mbkfree($3);
            }
            ;

expression_list : expression 
                {
                char concat[1024];
                sprintf(concat,"%f",$1);
                $$ = mbkstrdup(concat);
                }
                | expression_list COMMA expression
                {
                char concat[1024];
                char rescue[1024];
                sprintf(rescue,"%f",$3);
                strcpy(concat, $1);
                strcat(concat, ",");
                strcat(concat, rescue);
                $$ = mbkstrdup(concat);
                mbkfree($1);
                }
                ;

expression : make_expression
            {
            $$ = eqt_eval(slibEqtCtx,namealloc($1),EQTFAST);
            mbkfree($1);
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
              | make_expression OPE primary 
                  {
                  char concat[1024];
                  strcpy(concat, $1);
                  strcat(concat, $2);
                  strcat(concat, $3);
                  $$ = mbkstrdup(concat);
                  mbkfree($1); mbkfree($3);
                  }
              | operateur LPAR make_expression RPAR 
                  {
                  char concat[1024];
                  strcpy(concat, $1);
                  strcat(concat, "(");
                  strcat(concat, $3);
                  strcat(concat, ")");
                  $$ = mbkstrdup(concat);
                  mbkfree($3);
                  }
              | make_expression OPE LPAR make_expression RPAR 
                  {
                  char concat[1024];
                  strcpy(concat, $1);
                  strcat(concat, $2);
                  strcat(concat, "(");
                  strcat(concat, $4);
                  strcat(concat, ")");
                  $$ = mbkstrdup(concat);
                  mbkfree($1); mbkfree($4);
                  }
              ;
              
operateur    :     { $$ = ""; }
             | OPE { $$ = $1; }
             ;
             
primary   : choice   { $$ = mbkstrdup($1); }
          | function { $$ = mbkstrdup($1); mbkfree($1); }
          ;




%%

int yyerror()
{
	printf("SLIB Parse Error at line %d .\n",slibParseLine);
    EXIT (EXIT_FAILURE) ;
    return 0;
}

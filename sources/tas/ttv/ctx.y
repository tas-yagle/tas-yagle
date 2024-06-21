/******************************************************************************/
/*                                                                            */
/*                      Chaine de CAO & VLSI   AVERTEC                        */
/*                                                                            */
/*    Produit: TTV Version 1.00                                               */
/*    Fichier: ctx.y                                                          */
/*                                                                            */
/*    (c) copyright 2000 AVERTEC                                              */
/*    Tous droits reserves                                                    */
/*                                                                            */
/*    Auteur(s): Karim Dioury                                                 */
/*                                                                            */
/******************************************************************************/

%{
#define API_HIDE_TOKENS
#include STM_H
#include "ttv.h"
#include "ttv_ctx.h"
#include TTV_H

int yyerror ();
int yylex ();

ttvfig_list *headfig ;
ttvfig_list *insfig ;
ttvdelay_list *ptdelay ;
int linepos ;
int linesize ;
long linetype ;
char ctx_buf[64] ;

%}

%union {
  char           *t_pchar;
  float           t_float;
  long            t_long;
  char            t_char;
};

%token <t_pchar>  _IDENTIFIER 
%token <t_long>   _INTEGER 
%token <t_float>  _NUMBER 
%token <t_float>  _ENUMBER 
%token <t_pchar>  _VERSION_NUMBER 
%token _H
%token _G
%token _M
%token _I
%token _D
%token _E
%token _F

%start ctx_file 

%type <t_pchar> tool
%type <t_pchar> version
%type <t_pchar> figname
%type <t_pchar> tec
%type <t_pchar> tecversion
%type <t_long>  defslope
%type <t_float> capa 
%type <t_float> number 
%type <t_long>  level
%type <t_long>  day
%type <t_long>  month
%type <t_long>  year
%type <t_long>  hour
%type <t_long>  minute
%type <t_long>  second
%type <t_long>  val      
%type <t_long>  slope 
%type <t_long>  typeline 
%type <t_long>  typetiming 

%%

ctx_file    : header figurelist eof
            ;

header      : _H tool version figname tec tecversion defslope capa level date ';'
            {
              if( headfig->INFO->TECHNONAME    != $5 ||
                  headfig->INFO->TECHNOVERSION != $6 ||
                  headfig->INFO->SLOPE         != $7 ||
                  headfig->INFO->CAPAOUT       != $8
                )
              {
                fprintf( stderr, "Header of ctx file %s doesn't match timing view. Ignore.\n", $4 );
                YYABORT;
              }
            }
            | _H tool version figname tec tecversion defslope capa number number number number number level date ';'
            {
              if( headfig->INFO->TECHNONAME    != $5 ||
                  headfig->INFO->TECHNOVERSION != $6 ||
                  headfig->INFO->SLOPE         != $7 ||
                  headfig->INFO->CAPAOUT       != $8
                )
              {
                fprintf( stderr, "Header of ctx file %s doesn't match timing view. Ignore.\n", $4 );
                YYABORT;
              }
            }
            ;

eof         : _G ';'
            ;

tool        : _IDENTIFIER
            {
              $$ = $1 ;
            }
            ;

version     : _NUMBER
            {   
              sprintf(ctx_buf,"%.2f",$1) ;
              $$ = namealloc(ctx_buf) ;
            }
            | _VERSION_NUMBER
            {
              $$ = $1 ;
            }
            ;

figname     : _IDENTIFIER
            {
             headfig = ttv_getttvfig($1,0) ;
             if(headfig == NULL) 
              {
               yyerror () ;
               EXIT(1) ;
              }
              $$ = $1 ;
            }
            ;

tec         : _IDENTIFIER
            {
              $$ = $1 ;
            }
            ;

tecversion  : _NUMBER
            {
              sprintf(ctx_buf,"%.2f",$1) ;
              $$ = namealloc(ctx_buf) ;
            }
            ;

defslope    : _NUMBER
            {
              $$ = (long)($1 * TTV_UNIT) ;
            }
            | _INTEGER
            {
              $$ = (long)($1 * TTV_UNIT) ;
            }
            ;

capa        : _NUMBER
            {
              $$ = $1 ;
            }
            ;

number      : _NUMBER
            {
              $$ = $1 ;
            }
            | _INTEGER
            {
              $$ = $1 ;
            }
            ;

level       : _INTEGER
            {
              $$ = $1 ;
            }
            ;

date        : '(' day month year hour minute second ')'
            {
              if( headfig->INFO->TTVYEAR  != $4 ||
                  headfig->INFO->TTVMONTH != $3 ||
                  headfig->INFO->TTVDAY   != $2 ||
                  headfig->INFO->TTVHOUR  != $5 ||
                  headfig->INFO->TTVMIN   != $6 ||
                  headfig->INFO->TTVSEC   != $7
                )
              {
                fprintf( stderr, "Date of ctx file %s doesn't match date of timing view. Ignore.\n", headfig->INFO->FIGNAME );
                YYABORT;
              }
            }
            ;

day         : _INTEGER
            {
              $$ = $1 ;
            }
            ;

month       : _INTEGER
            {
              $$ = $1 ;
            }
            ;

year        : _INTEGER
            {
              $$ = $1 ;
            }
            ;

hour        : _INTEGER
            {
              $$ = $1 ;
            }
            ;

minute      : _INTEGER
            {
              $$ = $1 ;
            }
            ;

second      : _INTEGER
            {
              $$ = $1 ;
            }
            ;

figurelist  : empty
            | figurelist figure 
            ;

figure      : model
            | instance
            ;

model       : _M _IDENTIFIER 
            {
             if(headfig->INFO->FIGNAME != $2)
              {
               yyerror () ;
               EXIT(1) ;
              }
             insfig = headfig ;
            }
              '(' delaylist ')' ';'
            ;

instance    : _I _IDENTIFIER _IDENTIFIER 
            {
             insfig = ttv_getttvins(headfig,$2,$3) ;
             if(insfig == NULL)
              {
               yyerror () ;
               EXIT(1) ;
              }
             }
            '(' delaylist ')' ';'
            ;

delaylist   : empty
            | delaylist  delaydef 
            ;

delaydef    : typeline _INTEGER 
            {
             if((insfig->STATUS & $1) != $1)
              ttv_parsttvfig(insfig,$1,TTV_FILE_DTX) ;
             linepos = ttv_getdelaypos(insfig,linetype,&linesize) ;
             ptdelay = ttv_allocdelayline(insfig,linepos,(int)$2,linesize) ;
            }
             '(' timinglist ')' ';'
            ;

typeline    : _D
            {
             $$ = TTV_STS_CLS|TTV_STS_D ;
             linetype = TTV_LINE_D ;
            }
            | _E
            {
             $$ = TTV_STS_CLS|TTV_STS_E ;
             linetype = TTV_LINE_E ;
            }
            | _F
            {
             $$ = TTV_STS_CLS|TTV_STS_F ;
             linetype = TTV_LINE_F ;
            }
            ;

timinglist  : empty
            | timinglist  timings 
            ;

timings     : '(' timing ')'
            ;

timing      : '(' typetiming val slope ')'
            {
             if($2 == TTV_FIND_MAX)
              {
               ptdelay->VALMAX = $3 ;
               if($4 != 0)
                 ptdelay->FMAX = $4 ;
               else
                 ptdelay->FMAX = TTV_NOSLOPE ;
              }
             else if($2 == TTV_FIND_MIN)
              {
               ptdelay->VALMIN = $3 ;
               if($4 != 0)
                 ptdelay->FMIN = $4 ;
               else
                 ptdelay->FMIN = TTV_NOSLOPE ;
              }
            }
            ;

typetiming  : _IDENTIFIER
            {
             if(strcasecmp($1,"MAX") == 0)
             
               $$ = TTV_FIND_MAX ;
             else
               $$ = TTV_FIND_MIN ;
            }
            ;

val         : _NUMBER
            {
              $$ = mbk_long_round($1 * TTV_UNIT) ;
            }
            | _INTEGER
            {
              $$ = mbk_long_round($1 * TTV_UNIT) ;
            }
            ;

slope       : _NUMBER 
            {
              $$ = mbk_long_round($1 * TTV_UNIT) ;
            }
            | _INTEGER
            {
              $$ = mbk_long_round($1 * TTV_UNIT) ;
            }
            ;

empty       :
            ;
%%

int yyerror ()
{
  extern char *ctxtext;
  fprintf (stderr,"*** parse error file %s line %d near %s ***\n\n",CTX_FILE,CTX_LINE,ctxtext);
  return 0;
}

/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : SPEF Version 1.00                                           */
/*    Fichier : spef_dnet.y                                                 */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Olivier Bichaut                                           */
/*                                                                          */
/****************************************************************************/

%{
#include SPE_H
#include MUT_H
#include MLO_H
#include MLU_H
#include "spef_actions.h" 
#include "spef_annot.h" 
#include "spef_util.h" 
#include "spef_drive.h" 

/*****************************************************************************
* private                                                                    *
*****************************************************************************/

/*****************************************************************************
* function declarations                                                      *
*****************************************************************************/
/*ptsignode = (signode_ctc*)mbkalloc (sizeof (struct signode_ctc));*/

int yyerror();
int yylex();
extern spef_info *SPEF_INFO;
extern char *vss;
extern it *namemaptable;
extern ht *portmap_ht;
extern chain_list *ctclist;

%}

%union
{
  char         *text ;
  struct 
  {
    double real ;
    char text[1024];
  } UR;
  struct 
  {
    long longint;
    char text[1024];
  } UL;

  struct chain *ch_l ;
} ;

/* general */
%token _SPEF_I               
%token _SPEF_B               
%token _SPEF_O               
%token _E_C             
%token _E_L             
%token _E_S             
%token _E_D             
%token _E_V _E_N      
%token _E_D_NET         
%token _E_CONN          
%token _E_P             
%token _E_I _E_X            
%token _E_CAP           
%token _E_RES     
%token _E_INDUC
%token _E_END
%token <UR> _NUMBER
%token <text> _INDEX
%token <text> _PREFIX_BUS_DELIM
%token <text> _SUFFIX_BUS_DELIM
%token <UL> _POS_INTEGER
%token <UR> _POS_NUMBER
%token <text> _IDENTIFIER
%token <text> _HCHAR
%token <text> _QSTRING

%start net

%type <text> net_ref
%type <UR.real> par_value
%type <UR.real> total_cap
%type <text> node_name a_name reduced_node_name

%%

/* reduced_node_name pour eviter des reduce/reduce conflit pour la regle "cap_elm" 
   sinon il faut prevoir de remanier le lex
*/

direction            : _SPEF_I { }
                     | _SPEF_B { }
                     | _SPEF_O { }
                     ;
                     
conn_attr_list       : empty { }
                     | conn_attr_list conn_attr { } ;
                                          
conn_attr            : coordinates { }
                     | cap_load { }
                     | slews { }
                     | driving_cell { } ;

coordinates          : _E_C number number { } ;

number               : _NUMBER { }
                     | _POS_NUMBER { }
                     | _POS_INTEGER { } ;

cap_load             : _E_L par_value { } ;

par_value            : _POS_NUMBER { $$ = $1.real; }
                     | _POS_NUMBER ":" _POS_NUMBER ":" _POS_NUMBER { $$ = $3.real; }
                     | _POS_INTEGER { $$ = $1.longint; }
                     | _POS_INTEGER ":" _POS_INTEGER ":" _POS_INTEGER { $$ = $3.longint; } ;
                     
slews                : _E_S par_value par_value { } ;

driving_cell         : _E_D a_name
                     { 
                       mbkfree($2);
                     } ;

net                 : d_net {YYACCEPT; } ;

d_net                : _E_D_NET net_ref total_cap 
                     {
                       char *name;
                       losig_list *ls;
                       if($2[0] == '*'){
                         name = (char*)getititem(namemaptable, atoi($2 + 1));
                         ls = spef_NewNet(name, $3 * SPEF_INFO->SPEF_CAP_SCALE);
                       }else{
                         name = spef_spi_devect($2);
                         ls = spef_NewNet(name, $3 * SPEF_INFO->SPEF_CAP_SCALE);
                       }
                       spef_setcurnet($2, ls);
                       mbkfree($2);
                     }
                     .routing_conf. .conn_sec. .cap_sec. .res_sec. .induc_sec. _E_END 
                     {
                       spef_setcurnet("", NULL);
                     } ;

net_ref              : _INDEX { $$ = $1; }
                     | a_name { $$ = $1; } ;


total_cap            : par_value { $$ = $1; } ;

.routing_conf.       : empty { }
                     | _E_V _POS_INTEGER { } ;

.conn_sec.           : _E_CONN conn_def conn_def_list .internal_node_coord_list. { }
                     |
                     ;

conn_def_list        : empty { }
                     | conn_def_list conn_def { } ;

.internal_node_coord_list.: 
                          .internal_node_coord_list. internal_node_coord
                          |
                          ;

internal_node_coord : _E_N node_name coordinates {mbkfree($2);}
                      ;

 .extention_node.: _E_X _POS_INTEGER
{
}
|
{
}
;

conn_def             : _E_P a_name direction conn_attr_list .extention_node.
                     {
                       mbkfree($2);
                     }
                     | _E_P _INDEX direction conn_attr_list .extention_node.
                     {
                       mbkfree($2);
                     }
                     | _E_I a_name direction conn_attr_list .extention_node.
                     {
                       mbkfree($2);
                     } ;

.cap_sec.            : _E_CAP cap_elm cap_elm_list { } 
                     |
                     ;

cap_elm_list         : empty { }
                     | cap_elm_list cap_elm { } ;

cap_elm              : _POS_INTEGER reduced_node_name par_value 
                     { 
                       ctclist = spef_AddCapacitance(ctclist, $2,vss,$3 * SPEF_INFO->SPEF_CAP_SCALE);
                       mbkfree($2);
                     }
                     | _POS_INTEGER reduced_node_name reduced_node_name par_value 
                     { 
                       ctclist = spef_AddCapacitance(ctclist, $2,$3,$4 * SPEF_INFO->SPEF_CAP_SCALE);
                       mbkfree($2);
                       mbkfree($3);
                     } ;

a_name               : _IDENTIFIER
                     { 
                       $$ = $1; 
                     }
                     | _POS_INTEGER
                     {
                       $$ = mbkstrdup($1.text);
                     }
                     ;
node_name            : a_name
                     { 
                       $$ = $1; 
                     }
                     | _INDEX
                     { 
                       $$ = $1; 
                     }
                     ;
                     
reduced_node_name    : _IDENTIFIER
                     { 
                       $$ = $1; 
                     }
                     | _INDEX
                     { 
                       $$ = $1; 
                     }
                     ;

.res_sec.            : _E_RES res_elm res_elm_list { } 
                     |
                     ;

res_elm_list         : empty { }
                     | res_elm_list res_elm { } ;

res_elm              : _POS_INTEGER node_name node_name par_value
                     { 
                       spef_AddResistor($2,$3,$4 * SPEF_INFO->SPEF_RES_SCALE);
                       mbkfree($2);
                       mbkfree($3);
                     } ;

.induc_sec.          : _E_INDUC induc_elm induc_elm_list { }
                     | empty { } ;

induc_elm_list       : empty { }
                     | induc_elm_list induc_elm { } ;

induc_elm            : _POS_INTEGER node_name node_name par_value { mbkfree($2); mbkfree($3);} ;
                     
                     
empty                : { } ;
%%

//extern char stbtext[] ;

int yyerror()
{
  printf("\nSPEF Parser:%s:%d: Syntax Error.\n", spef_ParsedFile, Line) ;
  return 0;
}


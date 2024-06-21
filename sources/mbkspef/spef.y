/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : SPEF Version 1.00                                           */
/*    Fichier : spef.yac                                                    */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Olivier Bichaut                                           */
/*                                                                          */
/****************************************************************************/

%{
#include SPE_H
#include AVT_H
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

static int ignore=0;
int yyerror();
int yylex();
char *vss = NULL;
it *namemaptable = NULL;
ht *portmap_ht = NULL;
chain_list *ctclist = NULL;
spef_info *SPEF_INFO;
// static void clean_SPEF_INFO();
 static int SPEF_ERROR;
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
  char char_t;
} ;

/* general */
%token _E_SPEF          
%token _E_DESIGN        
%token _E_DATE          
%token _E_VENDOR        
%token _E_PROGRAM       
%token _E_VERSION       
%token _E_DESIGN_FLOW   
%token _E_DIVIDER       
%token _E_DELIMITER     
%token _E_BUS_DELIMITER 
%token _E_T_UNIT        
%token _NS              
%token _PS              
%token _E_C_UNIT        
%token _PF              
%token _FF              
%token _E_R_UNIT        
%token _OHM             
%token _KOHM            
%token _E_L_UNIT        
%token _HENRY           
%token _MH              
%token _UH              
%token _E_NAME_MAP      
%token _E_PORTS
%token _SPEF_I               
%token _SPEF_B               
%token _SPEF_O               
%token _SPEF_X               
%token _E_C _E_GROUND_NETS _E_POWER_NETS _E_PHYSICAL_PORTS 
%token _E_L _E_DEFINE _E_PDEFINE _E_RNET _E_RPNET _E_DPNET           
%token _E_S _E_INDUC _E_DRIVER _E_CELL _E_C2_R1_C1 _E_LOADS
%token _E_D _E_RC _E_Q _E_K _E_N
%token _E_V       
%token _E_D_NET         
%token _E_CONN          
%token _E_P             
%token _E_I _E_X            
%token _E_CAP           
%token _E_RES     
%token _E_END
%token <UR> _NUMBER
%token <text> _INDEX
%token <UL> _POS_INTEGER 
%token <UR> _POS_NUMBER
%token <text> _IDENTIFIER
%token <text> _QSTRING
%start spef_file

%type <text> net_ref
%type <UR.real> par_value
%type <UR.real> total_cap
%type <UL> .extention_node.
%type <text> node_name a_name reduced_node_name
%type <char_t> _PREFIX_BUS_DELIM _SUFFIX_BUS_DELIM _HCHAR

%%


spef_file            :{SPEF_INFO = (spef_info*)mbkalloc(sizeof (struct spef_info)); spef_initinfo(SPEF_INFO);} 
                      header_def name_map power_def external_def define_def internal_def 
                     {
                       if(SPEF_PRELOAD == 'N'){
                           spef_remove_htctc();
                           if(portmap_ht)
                               delht(portmap_ht);
                           if(namemaptable)
                               delit(namemaptable);
                           spef_treatafterctclist(&ctclist);
                       }
                       spef_setneg2posnode(Lofig);
                     };
                     
header_def           : spef_version { SPEF_ERROR=0; } design_name date vendor program_name
                       program_version design_flow hierarchy_div_def
                       pin_delim_def bus_delim_def unit_def 
                     { 
                       char *str ;
                       if(V_BOOL_TAB[__MBK_FLATTEN_FOR_PARA].VALUE){
                           char separavant = SEPAR;
                           char oldkeepstate;

                           SEPAR = SPEF_INFO->DIVIDER;
                           oldkeepstate=FLATTEN_KEEP_ALL_NAMES;
                           FLATTEN_KEEP_ALL_NAMES=1;

                           rflattenlofig(Lofig, YES, YES);

                           FLATTEN_KEEP_ALL_NAMES=oldkeepstate;
                           SEPAR = separavant;
                       }
                       if( MBKFOPEN_FILTER==NO )
                           rcn_enable_cache( Lofig, spef_cache_parse, spef_cache_free, pool );
                       else  {
                           if( RCN_CACHE_SIZE > 0 ) {
                               avt_errmsg(SPE_ERRMSG, "001", AVT_WARNING);
                           }
                       }
                       if( rcn_getlofigcache( Lofig ) ){
                           SPEF_PRELOAD = 'Y';
                           SPEF_LOFIG_CACHE = 1;
                       }
        
                       spef_destroyexistingRC(Lofig);
                       spef_create_losig_htable(Lofig);
                       spef_create_loins_htable(Lofig);
                     };

spef_version         : _E_SPEF _QSTRING   
                     {
                       SPEF_INFO->SPEF = namealloc($2);
                       mbkfree($2);
                     }; 
                                         
design_name          : _E_DESIGN _QSTRING   
                     { 
                       char *name = spef_deqstring($2);
                       if(Lofig==NULL || strcmp(name, Lofig->NAME)!=0){
                         if ((Lofig = getloadedlofig(name))==NULL)
                           {
                             avt_errmsg(SPE_ERRMSG, "002", AVT_ERROR, name, spef_ParsedFile);
                             SPEF_ERROR=1;
                             YYACCEPT;
                           }
                       }
                       if (!spef_quiet) fprintf(stdout, "LOADING FILE %s.spef\n", name);
                       mbkfree($2);
                     } ;
                                         
date                 : _E_DATE _QSTRING     { mbkfree($2); } ;
				                         
vendor               : _E_VENDOR _QSTRING   { SPEF_INFO->VENDOR = namealloc($2); mbkfree($2); } ;
                                         
program_name         : _E_PROGRAM _QSTRING  { SPEF_INFO->PROGRAM = namealloc($2); mbkfree($2); } ;
				                         
program_version      : _E_VERSION _QSTRING  { SPEF_INFO->VERSION = namealloc($2); mbkfree($2); } ;

design_flow          : _E_DESIGN_FLOW _QSTRING qstring_list  
                     { 
                       SPEF_INFO->DESIGN_FLOW = addchain(SPEF_INFO->DESIGN_FLOW, mbkstrdup($2)); 
                       mbkfree($2);
                     } ;

qstring_list         : empty { }
                     | qstring_list _QSTRING 
                     { 
                       SPEF_INFO->DESIGN_FLOW = addchain(SPEF_INFO->DESIGN_FLOW, mbkstrdup($2)); 
                       mbkfree($2);
                     } ;

_HCHAR: '.' { $$='.'; }
      | '/' { $$='/'; }
      | ':' { $$=':'; }
      | '|' { $$='|'; }
      ;
      
hierarchy_div_def    : _E_DIVIDER _HCHAR { SPEF_INFO->DIVIDER = $2; } ;
				     
pin_delim_def        : _E_DELIMITER _HCHAR { SPEF_INFO->DELIMITER = $2; } ;

_PREFIX_BUS_DELIM: '[' {$$='[';}
                 | '{' {$$='{';}
                 | '(' {$$='(';}
                 | '<' {$$='<';}
                 ;

_SUFFIX_BUS_DELIM:  ']' {$$=']';}
                 | '}' {$$='}';}
                 | ')' {$$=')';}
                 | '>' {$$='>';}
                 ;


bus_delim_def        : _E_BUS_DELIMITER _PREFIX_BUS_DELIM 
                     { SPEF_INFO->PREFIX_BUS_DELIMITER = $2; 
                       SPEF_INFO->SUFFIX_BUS_DELIMITER = spef_sufdelim($2);
                     }
                     | _E_BUS_DELIMITER _PREFIX_BUS_DELIM _SUFFIX_BUS_DELIM 
                     { SPEF_INFO->PREFIX_BUS_DELIMITER = $2;
                       SPEF_INFO->SUFFIX_BUS_DELIMITER = $3;
                     } ;

unit_def             : time_scale cap_scale res_scale induc_scale { } ;

time_scale           : _E_T_UNIT _POS_NUMBER time_unit { SPEF_INFO->SPEF_T_SCALE = $2.real; }
                     | _E_T_UNIT _POS_INTEGER time_unit { SPEF_INFO->SPEF_T_SCALE = $2.longint; } ;


time_unit            : _NS { SPEF_INFO->SPEF_T_UNIT = 'N'; }
                     | _PS { SPEF_INFO->SPEF_T_UNIT = 'P'; } ;

cap_scale            : _E_C_UNIT _POS_NUMBER cap_unit 
                     { SPEF_INFO->SPEF_CAP_SCALE = $2.real; 
                       if(SPEF_INFO->SPEF_CAP_UNIT == 'F')
                           SPEF_INFO->SPEF_CAP_SCALE /= 1000.0; 
                     }
                     | _E_C_UNIT _POS_INTEGER cap_unit 
                     { SPEF_INFO->SPEF_CAP_SCALE = $2.longint; 
                       if(SPEF_INFO->SPEF_CAP_UNIT == 'F')
                           SPEF_INFO->SPEF_CAP_SCALE /= 1000.0; 
                     } ;

cap_unit             : _PF { SPEF_INFO->SPEF_CAP_UNIT = 'P'; }
                     | _FF { SPEF_INFO->SPEF_CAP_UNIT = 'F'; } ;

res_scale            : _E_R_UNIT _POS_NUMBER res_unit 
                     { SPEF_INFO->SPEF_RES_SCALE = $2.real; 
                       if(SPEF_INFO->SPEF_RES_UNIT == 'K')
                           SPEF_INFO->SPEF_RES_SCALE *= 1000.0; 
                     }
                     | _E_R_UNIT _POS_INTEGER res_unit 
                     { SPEF_INFO->SPEF_RES_SCALE = $2.longint; 
                       if(SPEF_INFO->SPEF_RES_UNIT == 'K')
                           SPEF_INFO->SPEF_RES_SCALE *= 1000.0; 
                     } ;

res_unit             : _OHM { SPEF_INFO->SPEF_RES_UNIT = 'O'; }
                     | _KOHM { SPEF_INFO->SPEF_RES_UNIT = 'K';} ;

induc_scale          : _E_L_UNIT _POS_NUMBER induc_unit { SPEF_INFO->SPEF_L_SCALE = $2.real; }
                     | _E_L_UNIT _POS_INTEGER induc_unit { SPEF_INFO->SPEF_L_SCALE = $2.longint; } ;

induc_unit           : _HENRY { SPEF_INFO->SPEF_L_UNIT = 'H'; }
                     | _MH { SPEF_INFO->SPEF_L_UNIT = 'M'; }
                     | _UH { SPEF_INFO->SPEF_L_UNIT = 'U'; } ;

name_map             : empty             { }
                     | _E_NAME_MAP {namemaptable = addit(100); } name_map_entry name_map_entry_list { } ;

name_map_entry       : _INDEX a_name 
                     { 
                       int index = atoi($1 + 1);
                       char *name  = spef_rename($2);
                       setititem(namemaptable, index, (long)name);
                       mbkfree($1);
                       mbkfree($2);
                     } ;

name_map_entry_list  : empty { }
                     | name_map_entry_list name_map_entry { } ;

power_def            : empty             { }
                     | power_net_def
                     | power_net_def ground_net_def
                     | ground_net_def
                     ;

power_net_def : _E_POWER_NETS net_name_list ;
ground_net_def : _E_GROUND_NETS net_name_list ;

net_name_list: net_name 
             | net_name_list net_name 
             ;

net_name : net_ref {mbkfree($1);}
         ;


external_def         : empty 
                     {
                         locon_list *ptcon;
                         portmap_ht = addht(100);
                         for(ptcon=Lofig->LOCON; ptcon; ptcon=ptcon->NEXT)
                             addhtitem(portmap_ht, ptcon->NAME, (long)ptcon->NAME);
                         if(!vss){
                           vss = namealloc("VSS");
                           spef_SetGroundSignal(vss);
                         }
                     }
                     | port_def 
                     {
                         if(!vss){
                           vss = namealloc("VSS");
                           spef_SetGroundSignal(vss);
                         }
                     } ;

port_def             : _E_PORTS {portmap_ht = addht(100);} port_entry port_entry_list 
                     {
                     } ;

port_entry           : port_name direction conn_attr_list { } ;

port_entry_list      : empty { }
                     | port_entry_list port_entry { } ;

port_name            : _INDEX 
                     {
                       if(isvss($1)){
                         if(!vss){
                           vss = namealloc($1);
                           spef_SetGroundSignal($1);
                         }
                       }
                       else
                         addhtitem(portmap_ht, spef_spi_devect($1), (long)spef_spi_devect($1));
                       mbkfree($1);
                     }
                     | _IDENTIFIER
                     {
                       if(isvss($1)){
                         if(!vss){
                           vss = namealloc($1);
                           spef_SetGroundSignal($1);
                         }
                       }
                       else
                         addhtitem(portmap_ht, spef_spi_devect($1), (long)spef_spi_devect($1));
                       mbkfree($1);
                     } ;

direction            : _SPEF_I { }
                     | _SPEF_B { }
                     | _SPEF_O { }
                     | _SPEF_X { } ;

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
                     | _POS_NUMBER ':' _POS_NUMBER ':' _POS_NUMBER { $$ = $3.real; }
                     | _POS_INTEGER { $$ = $1.longint; }
                     | _POS_INTEGER ':' _POS_INTEGER ':' _POS_INTEGER { $$ = $3.longint; } ;
                     
slews                : _E_S par_value par_value .thresholds. { } ;

.thresholds. : threshold threshold
             |
             ;
             
threshold: _POS_NUMBER {}
         | _POS_NUMBER ':' _POS_NUMBER ':' _POS_NUMBER {}
         ;
         

driving_cell         : _E_D a_name
                     { 
                       mbkfree($2);
                     }
                     | _E_D _INDEX {mbkfree($2);}
                     ;

define_def           : empty { } ;

internal_def         : nets nets_list { } ;

nets_list            : empty { }
                     | nets_list nets { } ;

nets                 : d_net { }
                     | r_net
                     | d_pnet
                     | r_pnet
                     ;

d_net                : _E_D_NET net_ref total_cap 
                     {
                       char *name;
                       losig_list *ls;
                       if($2[0] == '*'){
                         name = (char*)getititem(namemaptable, atoi($2 + 1));
                         ls =spef_NewNet(name, $3 * SPEF_INFO->SPEF_CAP_SCALE);
                       }else{
                         name = spef_spi_devect($2);
                         ls=spef_NewNet(name, $3 * SPEF_INFO->SPEF_CAP_SCALE);                       
                       }
                       spef_setcurnet($2, ls);
                     }
                     .routing_conf. .conn_sec. .cap_sec. .res_sec. .induc_sec. _E_END 
                     {
                       losig_list *ptsig;
                       if ((ptsig = spef_getcurnet())!=NULL)
                         rcn_calccapa(ptsig);
                       spef_setcurnet("", NULL);
                       mbkfree($2);
                     } ;
d_pnet                : _E_DPNET {ignore=1;} net_ref total_cap 
                     .routing_conf. .conn_sec. .cap_sec. .res_sec. .induc_sec. {ignore=0; mbkfree($3); } _E_END 
                     ;

net_ref              : _INDEX { $$ = $1; }
                     | a_name { $$ = $1; } ;


total_cap            : par_value { $$ = $1; } ;

.routing_conf.         : empty { }
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
  $$.longint=$2.longint;
}
|
{
  $$.longint=-1;
}
;

conn_def             : _E_P a_name direction conn_attr_list .extention_node.
                     {
                       char *name;
                       name = namealloc($2);
                       if (!ignore) spef_AddConnector(name, $5.longint);
                       mbkfree($2);
                     }
                     | _E_P _INDEX direction conn_attr_list .extention_node.
                     {
                       char *name;
                       name = namealloc($2);
                       if (!ignore) spef_AddConnector(name, $5.longint);
                       mbkfree($2);
                     }
                     | _E_I a_name direction conn_attr_list .extention_node.
                     {
                       char *instpinname, *pinname, *name;
                       if (!ignore) {
                         instpinname = mbkstrdup($2);
                         pinname = strrchr(instpinname, SPEF_INFO->DELIMITER);
                         if (pinname!=NULL) 
                         {
                           *pinname = '\0';
                           pinname++;
                         }
                         else 
                         {
                           pinname="<delimiter not found>";
                           avt_errmsg(SPE_ERRMSG, "015", AVT_FATAL, spef_ParsedFile, Line, SPEF_INFO->DELIMITER, instpinname);
                         }
                           
                         if(instpinname[0] == '*')
                           name = (char*)getititem(namemaptable, atoi(instpinname + 1));
                         else
                           name = instpinname;

                         spef_AddInstanceConnector($2,name,pinname,$5.longint);
                         mbkfree(instpinname);
                       }
                       mbkfree($2);
                     } 
                     ;

.cap_sec.            : _E_CAP cap_elm cap_elm_list { } 
                     |
                     ;

cap_elm_list         : empty { }
                     | cap_elm_list cap_elm { } ;

cap_elm              : _POS_INTEGER reduced_node_name par_value 
                     { 
                       if (!ignore) ctclist = spef_AddCapacitance(ctclist, $2,vss,$3 * SPEF_INFO->SPEF_CAP_SCALE);
                       mbkfree($2);
                     }
                     | _POS_INTEGER reduced_node_name reduced_node_name par_value 
                     { 
                       if (!ignore) ctclist = spef_AddCapacitance(ctclist, $2,$3,$4 * SPEF_INFO->SPEF_CAP_SCALE);
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
                       if (!ignore) spef_AddResistor($2,$3,$4 * SPEF_INFO->SPEF_RES_SCALE);
                       mbkfree($2);
                       mbkfree($3);
                     } ;

.induc_sec.          : _E_INDUC induc_elm induc_elm_list { }
                     | empty { } ;

induc_elm_list       : empty { }
                     | induc_elm_list induc_elm { } ;

induc_elm          : _POS_INTEGER node_name node_name par_value { mbkfree($2); mbkfree($3);} ;
                     
                     
empty                : { } ;

r_net                : _E_RNET net_ref total_cap 
                     .routing_conf. .driver_reduc_list. _E_END {mbkfree($2);}
                     ;

r_pnet                : _E_RPNET net_ref total_cap 
                     .routing_conf. .driver_reduc_list. _E_END {mbkfree($2);}
                     ;
                     
.driver_reduc_list. : 
                    .driver_reduc_list. driver_reduc
                    |
                    ;

driver_reduc: _E_DRIVER a_name _E_CELL node_name _E_C2_R1_C1 par_value par_value par_value _E_LOADS rc_desc_list 
              {mbkfree($2); mbkfree($4);};

rc_desc_list : rc_desc
             | rc_desc_list rc_desc
             ;

rc_desc: _E_RC a_name par_value .pole_residue_desc. {mbkfree($2);};

.pole_residue_desc.: pole_desc residue_desc
                   |
                   ;
                   
pole_desc: _E_Q _POS_INTEGER pole_list ;
residue_desc: _E_K _POS_INTEGER residue_list ;

pole_list: pole
         | pole_list pole;

residue_list: residue
            | residue_list residue
            ;
            
pole: complex_par_value ;
residue: complex_par_value ;

complex_par_value: cnumber
                 | number
                 | cnumber ':' cnumber ':' cnumber
                 | number ':' number ':' number
                 ;

cnumber: '(' number number ')' ;


%%

extern char *speftext ;

int yyerror()
{
  avt_errmsg(SPE_ERRMSG, "004", AVT_FATAL, spef_ParsedFile, Line, speftext);
//  printf("\nSPEF Parser:%s:%d: Syntax Error.\n", spef_ParsedFile, Line) ;
  EXIT(1);
  return 0;
}

void spef_parser_error(char *message)
{
  avt_errmsg(SPE_ERRMSG, "003", AVT_FATAL, spef_ParsedFile, Line, message);
//fprintf(stderr,"\nSPEF Parser:%s:%d: %s\n", spef_ParsedFile, Line, message) ;
  EXIT(1);
}
/*
static void clean_SPEF_INFO()
{
  chain_list *cl;
  for (cl=SPEF_INFO->DESIGN_FLOW; cl!=NULL; cl=cl->NEXT)
    mbkfree(cl->DATA);
  freechain(SPEF_INFO->DESIGN_FLOW);
  mbkfree(SPEF_INFO);
}
*/
int spef_error()
{
  return SPEF_ERROR;
}

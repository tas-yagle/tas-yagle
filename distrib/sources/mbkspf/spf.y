/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : SPF Version 1.00                                            */
/*    Fichier : spf.yac                                                     */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

%{
#include AVT_H
#include SPF_H
#include "spf_actions.h" 
#include "spf_annot.h" 
#include "spf_util.h" 

static double spfgetval(char *txt, int line, char *err, int capa);

/*****************************************************************************
* private                                                                    *
*****************************************************************************/

/*****************************************************************************
* function declarations                                                      *
*****************************************************************************/

  int yyerror();
  int yylex();
  static char *circuitname;

  union 
  {
    long int dval;
    double ival;
  } toto;
%}

%union
  {
  char          text[1024] ;
  double        real ;
  struct
  {
   int  integer ;
   char text[512];
  } I;
  struct chain *ch_l ;
  } ;

/* general */
%token           _LEFTPAR _ENDOFSPICELINE
%token           _RIGHTPAR
%token <text>    _TEXT
%token <text>    _QSTRING
%token <I> _INTEGER
%token           _NVER

/* DSPF statements */
%token        _DESIGN
%token        _DATE
%token        _VENDOR
%token        _PROGRAM
%token        _VERSION
%token        _DSPF
%token <text> _DIVIDER
%token <text> _DELIMITER
%token <text> _BUSBIT
%token        _GROUND_NET
%token        _NET
%token        _PIN        
%token        _SUB        
%token        _INST        
/* native SPICE statements */
%token        _SUBCKT
%token        _END
%token        _ENDS
%token <text> _RESISTANCE        
%token <text> _CAPACITANCE        
%token <text> _SUBCKTCALL        

%start dspf_file

%type <text> qstring 
%type <text> chipname
%type <text> nodename
%type <text> pinname
%type <text> instname
%type <text> instpinname
%type <text> subnodename
%type <real> netcap 
%type <real> pincap 
%type <real> cvalue
%type <real> rvalue
%type <ch_l> extnodes_list

%%


dspf_file            : spf_version header_elements body end { } ;

.endofspiceline.     : _ENDOFSPICELINE
                     ;

header_elements      : design_name header_elements
                     | date vendor header_elements
                     | program_name header_elements
                     | program_version header_elements
                     | hierarchy_divider header_elements
                     | name_delimiter header_elements
                     | busbit header_elements
                     |
                     ;
                     
spf_version          : _DSPF _TEXT .endofspiceline.
                     | error _ENDOFSPICELINE
                     ; 
                                         
design_name          : _DESIGN qstring .endofspiceline.
                     { 
                       circuitname=spf_deqstring($2);
                     }
                     | error _ENDOFSPICELINE
                     ;
                                         
date                 : _DATE qstring .endofspiceline.
                     | error _ENDOFSPICELINE
                     ;
                                         
vendor               : _VENDOR qstring .endofspiceline.
                     | error _ENDOFSPICELINE
                     ;
                                         
program_name         : _PROGRAM qstring .endofspiceline.
                     | error _ENDOFSPICELINE
                     ;
                                         
program_version      : _VERSION qstring .endofspiceline.
                     | error _ENDOFSPICELINE
                     ;
                     
hierarchy_divider    : _DIVIDER .endofspiceline.
                       {
                         char *s;
                         s=strchr($1, ' ');
                         if (s!=NULL) while (*s==' ' || *s=='\t') s++;
                         if (s==NULL)
                          {
                            avt_errmsg(SPF_ERRMSG, "005", AVT_WARNING, spf_Line-1);
                            spf_setDivider('/');
                          }
                         else
                          spf_setDivider(*s);
                       } 
                     | error _ENDOFSPICELINE
                       ;
                     
name_delimiter       : _DELIMITER .endofspiceline.
                       {
                         char *s;
                         s=strchr($1, ' ');
                         if (s!=NULL) while (*s==' ' || *s=='\t') s++;
                         if (s==NULL)
                          {
                            avt_errmsg(SPF_ERRMSG, "006", AVT_WARNING, spf_Line-1);
                            spf_setDelimiter(':');
                          }
                         else
                          spf_setDelimiter(*s);
                       } 
                     | error _ENDOFSPICELINE
                       ;

busbit               : _BUSBIT .endofspiceline.       
                       {
                         char temp[1024], temp1[1024];
                         sscanf($1,"%s%s", temp, temp1);
                         if (strlen(temp1)==2)
                           spf_setBUSDelimiters(temp1[0], temp1[1]);
                         else 
                         if ((strlen(temp1)==4) && (temp1[0] == '"') && (temp1[3] == '"'))
                           spf_setBUSDelimiters(temp1[1], temp1[2]);
                         else
                           avt_errmsg(SPF_ERRMSG, "007", AVT_WARNING, spf_Line-1);
                       } 
                     | error _ENDOFSPICELINE
                     ;

qstring              : empty             { strcpy($$, "") ; } 
                     | _QSTRING          { strcpy($$ ,$1) ; }
                     | _TEXT             { strcpy($$ ,$1) ; } ;

body                 : subCircuit
                     | gnet_def_list
                       {
                         char *name = circuitname;
                         if (name==NULL)
                         {
                           avt_errmsg(SPF_ERRMSG, "008", AVT_WARNING);
                           YYACCEPT;
                         }
                         if (spf_Lofig!=NULL) fprintf(stdout, "LOADING FILE %s.%s\n", name, IN_PARASITICS);
                         if (spf_Lofig==NULL || strcmp(name, spf_Lofig->NAME))
                           {
                             if ((spf_Lofig = getloadedlofig(name))==NULL)
                               {
                                 avt_errmsg(SPF_ERRMSG, "009", AVT_WARNING, name);
                                 YYACCEPT;
                               }
                           }
                         spf_prepare_lofig(spf_Lofig);
                       }
                       net_block_list
                     ;

subCircuit           : subckt_def gnet_def_list net_block_list inst_block_list end_subckt    
                     ;
                     
end                  : empty
                     | _END .endofspiceline.
                     ;
                     
subckt_def           : _SUBCKT _TEXT extnodes_list .endofspiceline.
                       {
                         char *name = namealloc($2);

                         if (spf_Lofig!=NULL) fprintf(stdout, "LOADING FILE %s.%s\n", name, IN_PARASITICS);
                         if (spf_Lofig==NULL || strcmp(name, spf_Lofig->NAME))
                           {
                             if ((spf_Lofig = getloadedlofig(name))==NULL)
                               {
                                 avt_errmsg(SPF_ERRMSG, "009", AVT_WARNING, name);
                                 YYACCEPT;
                               }
                           }
                         spf_prepare_lofig(spf_Lofig);
#ifdef SPF_DEBUG
                         printf("Circuit : %s\n",$2) ;
#endif
                       }
                     | error _ENDOFSPICELINE
                     ;

extnodes_list        : empty             { $$ = NULL ; } 
                     | extnodes_list nodename 
                       {
                         /*$$ = addchain($1, $2)*/
                       } ;
                     
gnet_def_list        : empty 
                     | gnet_def_list gnet_def  
                     ;

gnet_def             : _GROUND_NET nodename .endofspiceline.
                       {
                         spf_SetGroundSignal($2);
                       }
                     | error _ENDOFSPICELINE
                     ;
                     
net_block_list       : empty
                     | net_block_list net_block 
                     ;

net_block            : net_def node_def_list res_capa_def_list 
                     ;
                                               
node_def_list        : empty
                     | node_def_list node_def 
                     ;

res_capa_def_list    : empty
                     | res_capa_def_list capacitor_def 
                     | res_capa_def_list resistor_def 
                     ;

inst_block_list      : empty   
                     | inst_block_list inst_block 
                     ;

inst_block           : _SUBCKTCALL extnodes_list .endofspiceline.     /* native SPICE */
                     {}
                     ;
                     
end_subckt           : _ENDS chipname .endofspiceline.   
                       {
                         spf_ResetLocalHTable();
                       }
                     | _ENDS .endofspiceline.       
                       {
                         spf_ResetLocalHTable();
                       }
                     ;
                     
net_def              : _NET nodename netcap .endofspiceline.
                       {
                         spf_NewNet($2,$3);
                       }
                     | error _ENDOFSPICELINE
                     ;
                     
node_def             : pin_def 
                     | instpin_def
                     | subnode_def
                     ;
                     
pin_def              : _PIN pin_element .endofspiceline.
                     | error _ENDOFSPICELINE
                     ;
                     
pin_element          : _LEFTPAR pinname pintype pincap coord_list _RIGHTPAR 
                       {
                         spf_AddConnector($2);
                       }
                     ;
                     
coord_list           : empty
                     | coord_list coord
                     ;
                     
instpin_def          : _INST instpin_element .endofspiceline.
                     | error _ENDOFSPICELINE
                     ;
                  
instpin_element      : _LEFTPAR instpinname instname pinname pintype pincap coord_list _RIGHTPAR 
                       {
                         spf_AddInstanceConnector($2,$3,$4);
                       }
                     ;

subnode_def          : _SUB subnode_element .endofspiceline.
                     ;


subnode_element      : _LEFTPAR subnodename coord_list _RIGHTPAR 
                       {
                         spf_AddSubNode($2);
                       }
                     ;

capacitor_def        : _CAPACITANCE nodename nodename cvalue .endofspiceline.   
                       {
                         spf_AddCapacitance($2,$3,$4);
                       }
                     | error _ENDOFSPICELINE
                     ;   
                     
resistor_def         : _RESISTANCE nodename nodename rvalue .endofspiceline.  
                       {
                         spf_AddResistor($2,$3,$4);
                       }
                     | error _ENDOFSPICELINE
                     ;   

netcap               : cvalue            { $$ = $1 ; } ;

pintype              : _TEXT                
                       { 
                         if ($1[1]!='\0' || strchr("IiOoSsBbJjXx", $1[0])==NULL)
                           avt_errmsg(SPF_ERRMSG, "001", AVT_WARNING, $1, spf_Line);
                       } 
                     ;

pincap               : cvalue            { $$ = $1 ; } 
                     | cvalue _LEFTPAR rcpair_list _RIGHTPAR 
                       {
                         $$ = $1 ; /* Elmore ? */
                       }
                     ;

rcpair_list          : rcpair
                     | rcpair_list rcpair 
                     ;

rcpair               : rvalue cvalue
                     {};
                                         
coord                : xcoord ycoord
                     ;

xcoord               : _TEXT
                     {};
                                         
ycoord               : _TEXT
                     {};
                                         
cvalue               : _TEXT
                      { 
                        $$=spfgetval($1, spf_Line, "010", 1)*1e12;
                      }
                     ;
                                         
rvalue               : _TEXT
                      { 
                        $$=spfgetval($1, spf_Line, "011", 0);
                      }
                     ;

nodename             : _TEXT          
                       {
                         strcpy($$, $1);
                       }
                     ;
                     
chipname             : _TEXT       { strcpy($$, $1) ; }
                     ;
                     
pinname              : _TEXT       { strcpy($$, $1) ; }
                     ;

instname             : _TEXT       { strcpy($$, $1) ; }
                     ;

instpinname          : _TEXT       { strcpy($$, $1) ; }
                     ;

subnodename          : _TEXT       { strcpy($$, $1) ; }
                     ;

empty                : {}
                     ;
%%

extern char *spftext;

int yyerror()
{
  char *tok=spftext;
  if (*tok=='\n') tok="<CR>";
  avt_errmsg(SPF_ERRMSG, "002", AVT_WARNING, spf_Line, tok);
  return 0;
}

static double spfgetval(char *txt, int line, char *err, int capa)
{
  char *endptr;
  double dval, factor;
  int l;
  dval=strtod(txt, &endptr);
  if (*endptr!='\0')
  {
    if ((l=strlen(endptr))==1 || (capa && l==2 && tolower(endptr[1])=='f'))
     {
       switch(tolower(endptr[0]))
       {
         case 'f': factor = spf_femto(); break;
         case 'p': factor = spf_pico(); break;
         case 'n': factor = spf_nano(); break;
         case 'u': factor = spf_micro(); break;
         case 'm': factor = spf_milli(); break;
         case 'k': factor = spf_kilo(); break;
         default:
           avt_errmsg(SPF_ERRMSG, err, AVT_WARNING, txt, line);
           factor = spf_pico();
       }
     }
     else
     {
       avt_errmsg(SPF_ERRMSG, err, AVT_WARNING, txt, line);
       factor = spf_pico();
     }
  }
  else
  {
    factor=1;
  }
  return dval*factor;
}


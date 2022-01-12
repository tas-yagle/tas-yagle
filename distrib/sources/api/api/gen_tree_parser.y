/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_tree_parser.yac                                         */
/*                                                                          */
/*    Auteur(s) : Francois DONNET                        le : 08/02/1999    */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/


%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include MUT_H
//#include MLO_H
#include "api_lib.h"
#include "gen_tree_utils.h"
#include "gen_tree_errors.h"
#include "gen_execute_C.h"

#define yytext gen_tree_parsertext
#define yyin gen_tree_parserin

  static char *unit_M, *unit_UM, *unit_PM, *unit_NM, *unit_MM;
  static ExecutionContext *GLOBAL_EC;

/*functions to build the TREE*/
static inline tree_list *to_interface_connectors(int,chain_list*,int,tree_list*);
static inline tree_list *to_internal_connectors(int,chain_list*,int,tree_list*);
void processfunctionheader(char *str, char **type, int *pointer, char **name);

/* syntax error schedule */
 extern int yylex();                /* parse ahead (by step of one char)*/
 extern int lineno;               /* line number to give user's information */
 extern FILE *yyin;                 /* flow from LEX */
 extern char *genius_yyin;
 extern char *yytext;               /* current parsed string */
 static inline int yyerror();              /* display error message */
 static char* file;                  /*name of file currently parsed*/
 static char *this_entity;
 static tree_list *main_tree;
 static chain_list *GEN_TOKEN_SYM=NULL, *GEN_TOKEN_COUPL=NULL, *GEN_TOKEN_UNUSED=NULL, *GEN_TOKEN_FORCEMATCH=NULL;
 static ptype_list *GEN_TOKEN_WITHOUTS=NULL, *GEN_TOKEN_EXCLUDES=NULL, *GEN_TOKEN_EXCLUDES_AT_END=NULL, *GEN_TOKEN_STOP_POWER=NULL;

 static template_corresp *API_INST_ADTS=NULL;
 static chain_list *API_MODEL_DEFINES=NULL;
 extern char *model_corresp(char *name);
 static char *instance_corresp(char *name);
 static template_corresp *getadts(char *name);
 static char *instance_reverse_corresp(char *name);
 static int MODELMODE=0, INSTMODE=0;
 static ht *API_TEMPLATE_HT=NULL;
%}

 /* one shift/reduce due to classical 
  "if then else." / "if then. else"   conflict in C language */
%expect 2

%union {
  int integer;
  double *real;
  double number;
  char *string;
  tree_list *tree;
  struct {int VALU;tree_list *TREE;} ptype;
  chain_list *chain;
  generic_map_info *gminfo;
  ptype_list *pt;
}


%token <integer> GEN_TOKEN_DIGIT GEN_TOKEN_DIGIT_CHAR        /*integer*/
%token <string>  GEN_TOKEN_IDENT        /*one word  --> NAMEALLOC !!!! from alliance mbk */
%token <string>  GEN_TOKEN_STRING GEN_TOKEN_FONCTION_HEADER    /*all characters in ""  ---> MBKALLOC !!!!! */
%token <real>    GEN_TOKEN_DIGIT_DOUBLE
%token GEN_TOKEN_STOP                /*EOF*/
%token GEN_TOKEN_ENTITY                /*VHDL*/
%token GEN_TOKEN_ARCHITECTURE
%token GEN_TOKEN_OF
%token GEN_TOKEN_IS                
%token GEN_TOKEN_COMPONENT
%token GEN_TOKEN_BEGIN
%token GEN_TOKEN_END                
%token GEN_TOKEN_SIGNAL
%token GEN_TOKEN_VARIABLE
%token GEN_TOKEN_INTEGER
%token GEN_TOKEN_IN                /*interface*/
%token GEN_TOKEN_OUT
%token GEN_TOKEN_INOUT                
%token GEN_TOKEN_DOWNTO                
%token GEN_TOKEN_TO                
%token GEN_TOKEN_BIT                
%token GEN_TOKEN_BIT_VECTOR                
%token GEN_TOKEN_WOR_BIT                
%token GEN_TOKEN_BUS                
%token GEN_TOKEN_MUX_BIT                
%token GEN_TOKEN_WOR_VECTOR                
%token GEN_TOKEN_MUX_VECTOR                
%token GEN_TOKEN_EQUI                                
%token GEN_TOKEN_PORT                /*port map for instance as in a vst*/
%token GEN_TOKEN_MAP
%token GEN_TOKEN_GENERIC
%token GEN_TOKEN_GENERATE
%token GEN_TOKEN_IN
%token GEN_TOKEN_LINKAGE

%token GEN_TOKEN_NAND                
%token GEN_TOKEN_NOR                
%token GEN_TOKEN_XOR                
%token GEN_TOKEN_NOT        
%token GEN_TOKEN_POW       /* i.e  **    6**3:=6*6*6   */
%token GEN_TOKEN_MOD
%token GEN_TOKEN_REM       /* modulo could be negative */
%token GEN_TOKEN_ABSOL       /* understood but without any used cause of integer var.*/

/*user's C*/
%token GEN_TOKEN_ACTION                
%token GEN_TOKEN_MBKFOPEN            /* open a file for command print */
%token GEN_TOKEN_FOPEN            /* open a file for command print */
%token GEN_TOKEN_FCLOSE           /* open a file for command print */
%token GEN_TOKEN_PRINTF        /* display a message*/
%token GEN_TOKEN_EXCLUDE         /* do not include an instance ion amodel */
%token GEN_TOKEN_STATIC
%token GEN_TOKEN_CHAR
%token GEN_TOKEN_LONG
%token GEN_TOKEN_FILE
%token GEN_TOKEN_FOR
%token GEN_TOKEN_DO
%token GEN_TOKEN_WHILE
%token GEN_TOKEN_IF
%token GEN_TOKEN_ELSE
%token GEN_TOKEN_FPRINTF
%token GEN_TOKEN_SPRINTF
%token GEN_TOKEN_BSPRINTF  /* first of multipart sprintf */
%token GEN_TOKEN_EXIT
%token GEN_TOKEN_RETURN
%token GEN_TOKEN_BREAK
%token GEN_TOKEN_STDOUT
%token GEN_TOKEN_STDERR
%token GNS_TOKEN_AND               /* i.e &&  for C or "and" for VHDL */
%token GEN_TOKEN_OR                /* i.e ||  for C or "or" for VHDL */
%token GEN_TOKEN_SUPEG            /* i.e >=  */
%token GEN_TOKEN_INFEG           /* i.e <=  */
%token GEN_TOKEN_NOTEG           /* i.e != for C or /= for VHDL */
%token GEN_TOKEN_EG                /* i.e ==  */
%token GEN_TOKEN_SHR                /* i.e >>  */
%token GEN_TOKEN_SHL                /* i.e <<  */
%token GEN_TOKEN_INC                /* i.e ++  */
%token GEN_TOKEN_DEC                /* i.e --  */
%token GEN_TOKEN_ARRAY
%token GEN_TOKEN_NULL

/*now they aren't tokens, they have been only created to avoid conflict token
 number in the syntaxic tree*/
%token GEN_TOKEN_OPPOSITE            /* i.e -x  */
%token GEN_TOKEN_INC_AFTER            /* i.e x++ */
%token GEN_TOKEN_INC_BEFORE        /* i.e ++x */
%token GEN_TOKEN_DEC_AFTER            /* i.e x-- */
%token GEN_TOKEN_DEC_BEFORE        /* i.e --x */
%token GEN_TOKEN_IN                   /* for the lofig */
%token GEN_TOKEN_OUT                
%token GEN_TOKEN_INOUT                
%token GEN_TOKEN_TRISTATE            
%token GEN_TOKEN_TRANSCV                
%token GEN_TOKEN_NODE            /*ABL format*/
%token GEN_TOKEN_NOP             /*for an empty block*/
%token GEN_TOKEN_LOINS           /*terminal wich points to a loins in a lofig*/
%token GEN_TOKEN_REF             /* to signal a pointer reference in C */
%token GEN_TOKEN_BLOCK           /* signal a block contained in brackets in C */
%token GEN_TOKEN_REF_CHAR        /*for type in C */
%token GEN_TOKEN_REF_FILE
%token GEN_TOKEN_REF_INT
%token GEN_TOKEN_REF_DOUBLE
%token GEN_TOKEN_REF_VOID
%token GEN_TOKEN_VOIDTOKEN  /* void func(...) */
%token GEN_TOKEN_VOIDTOKEN_CL  GEN_TOKEN_VOIDTOKEN_CL_TA
%token GEN_TOKEN_VOIDTOKEN_D /* void *var */
%token GEN_TOKEN_FUNCTION
%token GEN_TOKEN_DOUBLE
%token GEN_TOKEN_ADR
%token GEN_TOKEN_CAST
%token GEN_TOKEN_OTHER_TYPES
%token <chain> GEN_TOKEN_PRAGMA_SYM GEN_TOKEN_PRAGMA_COUPLED 

 /*type for tree*/
%type <tree> rules_list rule entity_rule entity_header generic generic_list declarations_gen port port_list declarations_port architecture_rule architecture_header elem_list elements_list signals_list instances_list signal component component_header instance_grp instance generate_if generate_for port_map equi_list equi map_list map map2 action_rule pseudo_C affect_list affect block C_list inst_optional expr_optional declarations_list declaration_type declaration decl_list regular_expr exp terminal var format string_args string vector array exp_vhdl boolean_vhdl terminal_vhdl module_name block_name model_name signal_name ident digit C_or_block regular_expr_2 instruction can_be_terminals cast_and_stuff

%type <tree> global_declaration instances_list_sub

%type <ptype> mode_gen mode_port type_in type_out type_inout  /* 2 args to return */
%type <chain> ident_list                        /* to break the order of flow */
%type <string> string_list c_known_types
%type <chain> map_or_equi_list generic_equi_list generic_map_list generic_map
%type <gminfo> generic_equi generic_map_implicit
%type <chain> c_function_param_list c_function_params c_variable
%type <tree> c_function_header args_list .args_list. affect_or_exp
%type <integer> pointer_depth
%type <number> digit_number unit

 /*define priority and list free tokens*/
%left ';' ','
%left '{' '}' '(' ')' '[' ']' '?' ':'
%right '=' GEN_TOKEN_EG_OR GEN_TOKEN_EG_XOR GEN_TOKEN_EG_AND GEN_TOKEN_EG_ADD GEN_TOKEN_EG_SUB GEN_TOKEN_EG_MUL GEN_TOKEN_EG_DIV GEN_TOKEN_EG_MOD
%left GNS_TOKEN_AND GEN_TOKEN_OR GEN_TOKEN_XOR GEN_TOKEN_NAND GEN_TOKEN_NOR '!'
%right GEN_TOKEN_NOT
%left GEN_TOKEN_SUPEG GEN_TOKEN_INFEG GEN_TOKEN_EG GEN_TOKEN_NOTEG '>' '<'
%left GEN_TOKEN_SHR GEN_TOKEN_SHL 
%left '-' '+'
%left '*' '/' '%' GEN_TOKEN_MOD GEN_TOKEN_REM
%left '^' '&' '|' GEN_TOKEN_POW
%left GEN_TOKEN_INC GEN_TOKEN_DEC GEN_TOKEN_ABSOL '~'
%start file




%%


 /****************************************************************************/
 /*       A library file can contain several model descriptions              */
 /****************************************************************************/

file:
  GEN_TOKEN_STOP                 {main_tree=NULL; return 0;}
| rules_list  GEN_TOKEN_STOP     {main_tree=$1; return 0;}
;


rules_list:
  rule    
{
  $$=$1;
  GLOBAL_EC->BLOCKS=addchain(GLOBAL_EC->BLOCKS,$1);
}
| rules_list rule
{ 
  if ($1!=NULL) $$=PUT_BIN(';',$1,$2); else $$=$2; 
  GLOBAL_EC->BLOCKS=addchain(GLOBAL_EC->BLOCKS,$2);
}
| rules_list GEN_TOKEN_PRAGMA_SYM { $$=$1; }
| error
{do  
    yychar = yylex();
 /*synchronisation with a special token*/
 while ((yychar != GEN_TOKEN_ENTITY) && (yychar != GEN_TOKEN_ARCHITECTURE) 
        && (yychar != GEN_TOKEN_ACTION) && (yychar != GEN_TOKEN_STOP));
 if (yychar==GEN_TOKEN_STOP) return 0;
 yyerrok;
 $$=NULL;
}
;


rule:
entity_rule         {$$=$1;}
| 
architecture_rule   {$$=$1;}
|
action_rule         {$$=$1;}
;


 /****************************************************************************/
 /*                         model description                                */
 /****************************************************************************/

entity_rule:
  entity_header  
  generic
  port
  entity_sym_info
  entity_end                             
    {
      chain_list *cl=NULL;
      cl=addchain(cl, GEN_TOKEN_UNUSED);
      cl=addchain(cl, GEN_TOKEN_COUPL);
      cl=addchain(cl, GEN_TOKEN_SYM);
      GEN_TOKEN_SYM=NULL; GEN_TOKEN_COUPL=NULL; GEN_TOKEN_UNUSED=NULL;
      this_entity=(char *)$1->DATA; // entity name
      $$=PUT_QUA(GEN_TOKEN_ENTITY,$1,$2,$3,PUT_ATO(GEN_TOKEN_PRAGMA_SYM, cl));
/*  entity_coupled_info */
    }
;

entity_sym_info:
    GEN_TOKEN_PRAGMA_SYM entity_sym_info
    {
      
      chain_list *cl=$1, *ch;
//      cl=PRAGMA_SPLIT($1);
      if (cl!=NULL)
        {
          if (strcasecmp(cl->DATA,"symmetric")==0)
        {
          SymInfoItem *t=NULL;
          for (ch=cl->NEXT; ch!=NULL; ch=ch->NEXT)
            {
              t=AddSymInfoItem(t);
              t->ConnectorName=(char *)ch->DATA;
              t->ConnectorRadical=vectorradical((char *)ch->DATA);
              t->ConnectorIndex=vectorindex((char *)ch->DATA);
              t->FLAGS=0;
            }

          GEN_TOKEN_SYM=addchain(GEN_TOKEN_SYM, t);
//          cl->NEXT=NULL;
        }
          else
        if (strcasecmp(cl->DATA,"coupled")==0)
          {
            SymInfoItem *t=NULL;
            for (ch=cl->NEXT; ch!=NULL; ch=ch->NEXT)
              {
            t=AddSymInfoItem(t);
            t->ConnectorName=(char *)ch->DATA;
            t->ConnectorRadical=vectorradical((char *)ch->DATA);
            t->ConnectorIndex=vectorindex((char *)ch->DATA);
            t->FLAGS=0;
              }
            GEN_TOKEN_COUPL=addchain(GEN_TOKEN_COUPL, t);
//            cl->NEXT=NULL;
          }
                else
                  if (strcasecmp(cl->DATA,"unused")==0)
                    {
                      SymInfoItem *t=NULL;
                      for (ch=cl->NEXT; ch!=NULL; ch=ch->NEXT)
                        {
                          t=AddSymInfoItem(t);
                          t->ConnectorName=(char *)ch->DATA;
                          t->ConnectorRadical=vectorradical((char *)ch->DATA);
                          t->ConnectorIndex=vectorindex((char *)ch->DATA);
                          t->FLAGS=0;
                        }
                      GEN_TOKEN_UNUSED=addchain(GEN_TOKEN_UNUSED, t);
                      //            cl->NEXT=NULL;
                    }
        else
          fprintf(stderr,"%s:%d: pragma '%s' with no effect\n",file,lineno,(char *)cl->DATA); 
          freechain(cl);
        }
//      free($1);
    }
    | {}
    ;

entity_header:
  GEN_TOKEN_ENTITY  model_name GEN_TOKEN_IS
     {
       char *name;
       MODELMODE=1;
       name=(char *)GET_ATO($2);
//       API_MODEL_DEFINESADTS=getadts(name);       
       CHANGE_ATO($2, model_corresp(name));
       $$=$2;
     }
| GEN_TOKEN_ENTITY error            
{do  
    yychar = yylex ();
 /*synchronisation with a special token*/
 while ((yychar != GEN_TOKEN_PORT) && (yychar != GEN_TOKEN_GENERIC) && (yychar != GEN_TOKEN_STOP));
 if (yychar==GEN_TOKEN_STOP) return 0;
 yyerrok;
 $$=NULL;
}
;


entity_end:
  GEN_TOKEN_END ';'                                              {}
| GEN_TOKEN_END model_name ';'    {MODELMODE=0;}
| GEN_TOKEN_END error               
{do  {yychar = yylex ();}
 while ((yychar != GEN_TOKEN_ARCHITECTURE) && (yychar != GEN_TOKEN_ENTITY) 
        && (yychar != GEN_TOKEN_ACTION) && (yychar != GEN_TOKEN_STOP));
 if (yychar==GEN_TOKEN_STOP) return 0;
 yyerrok;
}
;



 /****************************************************************************/
 /*                         generic interface                                */
 /****************************************************************************/
   
generic:
  /*empty*/                               {$$=PUT_ATO(GEN_TOKEN_NOP,0);}
| GEN_TOKEN_GENERIC '(' generic_list ')' ';'           {$$=PUT_UNI(GEN_TOKEN_GENERIC,$3);}
| error 
{do  
    yychar = yylex ();
 /*synchronisation with a special token*/
 while ((yychar != GEN_TOKEN_PORT) && (yychar != GEN_TOKEN_END) && (yychar != GEN_TOKEN_STOP));
 if (yychar==GEN_TOKEN_STOP) return 0;
 yyerrok;
 $$=NULL;
}
;


generic_list:
  declarations_gen                        {$$=$1;}
| generic_list ';' declarations_gen      
  {tree_list *p;
   if ($3==NULL) $$=NULL;
   else if (TOKEN($3)!=',') $$=PUT_BIN(',',$1,$3);
   else {/* $$ must be developped in only 1 format: left slim side tree */
      /* search the last ',' */
      for (p=$3;  TOKEN(p->NEXT->DATA)==','; p=p->NEXT->DATA)   {}
      /* put at the last the other branch */
      p->NEXT->DATA=PUT_BIN(',',$1,p->NEXT->DATA);
      $$=$3;
   }   
  }
| error
{do  yychar = yylex ();
 /*synchronisation with a special token*/
 while ((yychar != ';') && (yychar != ')') && (yychar != GEN_TOKEN_STOP));
 if (yychar==GEN_TOKEN_STOP) return 0;
 yyerrok;
 $$=NULL;
}
;


declarations_gen:
  GEN_TOKEN_VARIABLE ident_list ':' mode_gen   
  {$$=to_interface_connectors(GEN_TOKEN_VARIABLE,$2,$4.VALU,$4.TREE); 
   freechain($2);
  }
| ident_list ':' mode_gen   
  {$$=to_interface_connectors(GEN_TOKEN_VARIABLE,$1,$3.VALU,$3.TREE); 
   freechain($1);
  }
;

  
mode_gen:
  GEN_TOKEN_INTEGER                         {$$.VALU = GEN_TOKEN_INTEGER;     $$.TREE = NULL; }
;



 /****************************************************************************/
 /*                            port interface                                */
 /****************************************************************************/

port:
  /*empty*/                               {$$=PUT_ATO(GEN_TOKEN_NOP,0);}
| GEN_TOKEN_PORT '(' port_list ')' ';'                 {$$=PUT_UNI(GEN_TOKEN_PORT,$3);}
| GEN_TOKEN_PORT error 
{do  {yychar = yylex ();}
 /*synchronisation with a special token*/
 while ((yychar != GEN_TOKEN_END) && (yychar != GEN_TOKEN_STOP));
 if (yychar==GEN_TOKEN_STOP) return 0;
 yyerrok;
 $$=NULL;
}
;


port_list:
  declarations_port                            {$$=$1;}
| port_list ';' declarations_port           
  {tree_list *p;
   if ($3==NULL) $$=NULL;
   else if (TOKEN($3)!=',') $$=PUT_BIN(',',$1,$3);
   else {/* $$ must be developped in only 1 format: left slim side tree */
      /* search the last ',' */
      for (p=$3; TOKEN(p->NEXT->DATA)==','; p=p->NEXT->DATA) {}
      /* put at the last the other branch */
      p->NEXT->DATA=PUT_BIN(',',$1,p->NEXT->DATA);
      $$=$3;
   }   
  }
| error
{do  {yychar = yylex ();}
 /*synchronisation with a special token*/
 while ((yychar != ';') && (yychar != ')') && (yychar != GEN_TOKEN_STOP));
 if (yychar==GEN_TOKEN_STOP) return 0;
 yyerrok;
 $$=NULL;
}
;


declarations_port:
  GEN_TOKEN_SIGNAL ident_list ':' mode_port   
  {$$=to_interface_connectors(GEN_TOKEN_SIGNAL,$2,$4.VALU,$4.TREE); 
   freechain($2);
  }
| ident_list ':' mode_port   
  {$$=to_interface_connectors(GEN_TOKEN_SIGNAL,$1,$3.VALU,$3.TREE); 
   freechain($1);
  }
;


mode_port:
  type_in                         {$$=$1;}
| GEN_TOKEN_IN type_in                      {$$=$2;}
| GEN_TOKEN_OUT type_out                   {$$=$2;}
| GEN_TOKEN_INOUT type_inout               {$$=$2;}
;


type_in:
  GEN_TOKEN_BIT                             {$$.VALU = GEN_TOKEN_IN;     $$.TREE = NULL; }
| GEN_TOKEN_BIT_VECTOR array              {$$.VALU = GEN_TOKEN_IN;     $$.TREE = $2;   }
;


type_out:
  GEN_TOKEN_BIT                             {$$.VALU = GEN_TOKEN_OUT;      $$.TREE = NULL; }
| GEN_TOKEN_WOR_BIT GEN_TOKEN_BUS                   {$$.VALU = GEN_TOKEN_TRISTATE; $$.TREE = NULL; }
| GEN_TOKEN_MUX_BIT GEN_TOKEN_BUS                   {$$.VALU = GEN_TOKEN_TRISTATE; $$.TREE = NULL; }
| GEN_TOKEN_BIT_VECTOR array              {$$.VALU = GEN_TOKEN_OUT;      $$.TREE = $2;   }
| GEN_TOKEN_WOR_VECTOR array    GEN_TOKEN_BUS       {$$.VALU = GEN_TOKEN_TRISTATE; $$.TREE = $2;   }
| GEN_TOKEN_MUX_VECTOR array    GEN_TOKEN_BUS       {$$.VALU = GEN_TOKEN_TRISTATE; $$.TREE = $2;   }
;


type_inout:
  GEN_TOKEN_BIT                             {$$.VALU = GEN_TOKEN_INOUT;   $$.TREE = NULL; }
| GEN_TOKEN_WOR_BIT GEN_TOKEN_BUS                   {$$.VALU = GEN_TOKEN_TRANSCV; $$.TREE = NULL; }
| GEN_TOKEN_MUX_BIT GEN_TOKEN_BUS                   {$$.VALU = GEN_TOKEN_TRANSCV; $$.TREE = NULL; }
| GEN_TOKEN_BIT_VECTOR array              {$$.VALU = GEN_TOKEN_INOUT;   $$.TREE = $2;   }
| GEN_TOKEN_WOR_VECTOR array    GEN_TOKEN_BUS       {$$.VALU = GEN_TOKEN_TRANSCV; $$.TREE = $2;   }
| GEN_TOKEN_MUX_VECTOR array    GEN_TOKEN_BUS       {$$.VALU = GEN_TOKEN_TRANSCV; $$.TREE = $2;   }
;



 /****************************************************************************/
 /*                            describe this model                           */
 /****************************************************************************/

architecture_rule:
  architecture_header 
  elem_list
  GEN_TOKEN_BEGIN
  instances_list_sub
  architecture_end                    
  {
    misc_stuffs *ms=NULL;
    if (GEN_TOKEN_WITHOUTS!=NULL || GEN_TOKEN_EXCLUDES!=NULL || GEN_TOKEN_EXCLUDES_AT_END!=NULL || GEN_TOKEN_STOP_POWER!=NULL)
      {
    ms=(misc_stuffs *)malloc(sizeof(misc_stuffs));
    ms->WITHOUTS=GEN_TOKEN_WITHOUTS;
    ms->EXCLUDES=GEN_TOKEN_EXCLUDES;
    ms->EXCLUDES_AT_END=GEN_TOKEN_EXCLUDES_AT_END;
    ms->STOP_POWER=GEN_TOKEN_STOP_POWER;
    ms->FORCEMATCH=GEN_TOKEN_FORCEMATCH;
      }
    $1->NEXT->NEXT->NEXT->NEXT->DATA->DATA=(tree_list *)ms;
    $$=PUT_TRI(GEN_TOKEN_ARCHITECTURE,$1,$2,$4);
   GEN_TOKEN_WITHOUTS=NULL, GEN_TOKEN_EXCLUDES=NULL, GEN_TOKEN_EXCLUDES_AT_END=NULL;
   GEN_TOKEN_STOP_POWER=NULL; GEN_TOKEN_FORCEMATCH=NULL;
  }
;

instances_list_sub:
  instances_list { $$ = $1; }
| { $$=NULL; }
;
  
architecture_header:
  GEN_TOKEN_ARCHITECTURE module_name GEN_TOKEN_OF model_name GEN_TOKEN_IS      
{
  char *name;
  MODELMODE=1;
  name=(char *)GET_ATO($4);
//  API_MODEL_ADTS=getadts(name);       
  CHANGE_ATO($4, model_corresp(name));

  $$=PUT_QUA(GEN_TOKEN_OF,$2,$4, PUT_ATO(GEN_TOKEN_VOIDTOKEN,NULL), PUT_ATO(GEN_TOKEN_NULL, NULL));
}
| GEN_TOKEN_ARCHITECTURE error            
{do  
    yychar = yylex ();
 /*synchronisation with next token*/
 while ((yychar != GEN_TOKEN_IDENT) && (yychar != GEN_TOKEN_STOP));
 if (yychar==GEN_TOKEN_STOP) return 0;
 yyerrok;
 $$=NULL;
}
;


architecture_end:
  GEN_TOKEN_END ';'                            {}
| GEN_TOKEN_END module_name    ';' { MODELMODE=0; }
| GEN_TOKEN_END error               
{do  
    yychar = yylex ();
 /*synchronisation with next token*/
 while ((yychar != GEN_TOKEN_ARCHITECTURE) && (yychar != GEN_TOKEN_ENTITY) 
        && (yychar != GEN_TOKEN_ACTION) && (yychar != GEN_TOKEN_STOP));
 if (yychar==GEN_TOKEN_STOP) return 0;
 yyerrok;
}
;




 /****************************************************************************/
 /*                                  base elements                           */
 /****************************************************************************/


elem_list:
  signals_list elements_list        {$$=PUT_BIN(';',$1,$2);}
| elements_list                     {$$=$1;}
| { $$=NULL; }
;


elements_list:
  component                         {$$=$1;}
| elements_list component           {$$=PUT_BIN(';',$1,$2);}
| elements_list signal              {$<tree>$=PUT_BIN(';',$1,$2);}
  {tree_list *p;
   if ($2==NULL) $$=NULL;
   else if (TOKEN($2)!=',') $$=PUT_BIN(';',$1,$2);
   else {/* $$ must be developped in only 1 format: left slim side tree */
      /* search the last ',' */
      for (p=$2; ; p=p->NEXT->DATA) {
         p->TOKEN=';';                         /* change sign ',' in ';' */
         if (TOKEN(p->NEXT->DATA)!=',') break;
      }
      /* put at the last the other branch */
      p->NEXT->DATA=PUT_BIN(';',$1,p->NEXT->DATA);
      $$=$2;
   }   
  }
;


signals_list:
  signal                             {$$=$1;}
| signals_list signal               
  {tree_list *p;
   if ($2==NULL) $$=NULL;
   else if (TOKEN($2)!=',') $$=PUT_BIN(';',$1,$2);
   else {/* $$ must be developped in only 1 format: left slim side tree */
      /* search the last ',' */
      for (p=$2; ; p=p->NEXT->DATA) {
         p->TOKEN=';';                 /* change sign ',' in ';' */
         if (TOKEN(p->NEXT->DATA)!=',') break;
      }
      /* put at the last the other branch */
      p->NEXT->DATA=PUT_BIN(';',$1,p->NEXT->DATA);
      $$=$2;
   }   
  }
;


signal:
  GEN_TOKEN_SIGNAL ident_list ':' type_inout ';'
  {$$=to_internal_connectors(GEN_TOKEN_SIGNAL,$2,$4.VALU,$4.TREE); 
   freechain($2);
  }
| GEN_TOKEN_SIGNAL error
  {do  {yychar = yylex ();}
   /*synchronisation with next token*/
   while ((yychar != GEN_TOKEN_COMPONENT) && (yychar != GEN_TOKEN_SIGNAL) 
        && (yychar != GEN_TOKEN_BEGIN) && (yychar != GEN_TOKEN_STOP));
   if (yychar==GEN_TOKEN_STOP) return 0;
   yyerrok;
   $$=NULL;
  }
;


component:
  component_header
  generic
  port
  component_end
  {$$=PUT_TRI(GEN_TOKEN_COMPONENT,$1,$2,$3);}
;


component_header:
  GEN_TOKEN_COMPONENT model_name                  
  {
    char *name;
    name=(char *)GET_ATO($2);
    API_INST_ADTS=getadts(name);       
    INSTMODE=1;
    $$=$2;
  }
| GEN_TOKEN_COMPONENT error
{do  {yychar = yylex ();}
 /*synchronisation with next token*/
 while ((yychar != GEN_TOKEN_PORT) && (yychar != GEN_TOKEN_STOP));
 if (yychar==GEN_TOKEN_STOP) return 0;
 yyerrok;
 $$=NULL;
}
;


component_end:
  GEN_TOKEN_END GEN_TOKEN_COMPONENT ';'         {INSTMODE=0; API_INST_ADTS=NULL;}
| GEN_TOKEN_END error
{do  
    yychar = yylex ();
 /*synchronisation with next token*/
 while ((yychar != GEN_TOKEN_COMPONENT) && (yychar != GEN_TOKEN_SIGNAL) 
        && (yychar != GEN_TOKEN_BEGIN) && (yychar != GEN_TOKEN_STOP));
 if (yychar==GEN_TOKEN_STOP) return 0;
 yyerrok;
}
;



 
 /****************************************************************************/
 /*                            describe this model                           */
 /****************************************************************************/

instances_list:
  instance_grp                            {$$=$1;}
| instances_list instance_grp            
{
  if ($1!=NULL && $2!=NULL) $$=PUT_BIN(';',$1,$2);
  else if ($1!=NULL) $$=$1;
  else $$=$2;
}
;


instance_grp:
  instance                                 {$$=$1;}
| GEN_TOKEN_PRAGMA_SYM
{
  chain_list *cl=$1, *ch;
  ptype_list *pt;
//  cl=PRAGMA_SPLIT($1);
  if (cl!=NULL)
    {
      if (strcasecmp(cl->DATA,"without")==0)
    {
      for (pt=NULL, ch=cl->NEXT; ch!=NULL; ch=ch->NEXT)
        pt=addptype(pt, lineno, ch->DATA);
      GEN_TOKEN_WITHOUTS=(ptype_list *)append((chain_list *)GEN_TOKEN_WITHOUTS, (chain_list *)pt);
    }
      else
    if (strcasecmp(cl->DATA,"exclude")==0)
      {
        for (pt=NULL, ch=cl->NEXT; ch!=NULL; ch=ch->NEXT)
          pt=addptype(pt, lineno, ch->DATA);
        GEN_TOKEN_EXCLUDES=(ptype_list *)append((chain_list *)GEN_TOKEN_EXCLUDES, (chain_list *)pt);
      }
      else
          if (strcasecmp(cl->DATA,"exclude_at_end")==0)
            {
              for (pt=NULL, ch=cl->NEXT; ch!=NULL; ch=ch->NEXT)
                 pt=addptype(pt, lineno, ch->DATA);
              GEN_TOKEN_EXCLUDES_AT_END=(ptype_list *)append((chain_list *)GEN_TOKEN_EXCLUDES_AT_END, (chain_list *)pt);
             }
      else
          if (strcasecmp(cl->DATA,"stop_at_power_supplies")==0)
            {
              for (pt=NULL, ch=cl->NEXT; ch!=NULL; ch=ch->NEXT)
                 pt=addptype(pt, lineno, ch->DATA);
              GEN_TOKEN_STOP_POWER=(ptype_list *)append((chain_list *)GEN_TOKEN_STOP_POWER, (chain_list *)pt);
            }
      else
          if (strcasecmp(cl->DATA,"forcematch")==0)
            {
              for (pt=NULL, ch=cl->NEXT; ch!=NULL; ch=ch->NEXT)
                 pt=addptype(pt, lineno, ch->DATA);
              GEN_TOKEN_FORCEMATCH=(ptype_list *)append((chain_list *)GEN_TOKEN_FORCEMATCH, (chain_list *)pt);
            }

      else
      fprintf(stderr,"%s:%d: pragma '%s' with no effect\n",file,lineno,(char *)cl->DATA); 
      freechain(cl);
    }
//  free($1);
  $$=NULL;
}
| generate_if                               {$$=$1;}
| generate_for                              {$$=$1;}
;


generate_if:
  block_name ':' GEN_TOKEN_IF boolean_vhdl GEN_TOKEN_GENERATE
  instances_list
  GEN_TOKEN_END GEN_TOKEN_GENERATE ';'
  {$$=PUT_BIN(GEN_TOKEN_IF,$4,$6);}  
;


generate_for:
  block_name ':' GEN_TOKEN_FOR ident GEN_TOKEN_IN exp_vhdl GEN_TOKEN_TO exp_vhdl GEN_TOKEN_GENERATE
  instances_list
  GEN_TOKEN_END GEN_TOKEN_GENERATE ';'
  {$$=PUT_QUA(GEN_TOKEN_FOR,$4,$6,$8,$10);} 
| block_name ':' GEN_TOKEN_FOR ident GEN_TOKEN_IN exp_vhdl GEN_TOKEN_DOWNTO exp_vhdl GEN_TOKEN_GENERATE
  instances_list
  GEN_TOKEN_END GEN_TOKEN_GENERATE ';'
  {$$=PUT_QUA(GEN_TOKEN_FOR,$4,$8,$6,$10);} 
;


instance:
  block_name ':' model_name
  {
    char *name;
    name=(char *)GET_ATO($3);
    API_INST_ADTS=getadts(name);
    INSTMODE=1;
  }
  generic_map port_map ';'     
  {$$=PUT_TRI(GEN_TOKEN_MAP,PUT_QUA(GEN_TOKEN_OF,$1,$3,PUT_ATO(GEN_TOKEN_VOIDTOKEN,NULL), PUT_ATO(GEN_TOKEN_VOIDTOKEN,NULL)),$6,PUT_ATO(GEN_TOKEN_GENERIC,$5)); API_INST_ADTS=NULL;INSTMODE=0; }
| block_name error                    
{do  
    yychar = yylex ();
 while ((yychar != ';') && (yychar != GEN_TOKEN_STOP));
 if (yychar==GEN_TOKEN_STOP) return 0;
 if (yychar==';') yychar = yylex ();
 yyerrok;
 $$=NULL;
}
;




 /****************************************************************************/
 /*                                PORT MAP                                  */
 /****************************************************************************/

generic_map:
  GEN_TOKEN_GENERIC GEN_TOKEN_MAP  '(' map_or_equi_list ')'            {$$=$4;}
| { $$ =NULL; }
| GEN_TOKEN_GENERIC error                
{do  
    yychar = yylex ();
 while ((yychar != ';') && (yychar != GEN_TOKEN_STOP));
 if (yychar==GEN_TOKEN_STOP) return 0;
 yyerrok;
 $$=NULL;
}
;


map_or_equi_list:
  generic_equi_list { $$ = $1; }
| generic_map_list { $$ = $1; }
;

generic_equi_list:
  generic_equi ',' generic_equi_list { $$=append(addchain(NULL,$1),$3); }
| generic_equi  { $$=addchain(NULL, $1); }
;

generic_map_list:
  generic_map_implicit ',' generic_map_list { $$=append(addchain(NULL,$1),$3); }
| generic_map_implicit  { $$=addchain(NULL,$1); }
;

generic_equi:
GEN_TOKEN_IDENT GEN_TOKEN_EQUI GEN_TOKEN_IDENT
{
  generic_map_info *f=(generic_map_info *)mbkalloc(sizeof(generic_map_info));
  f->left=instance_corresp($1);
  f->type='n';
  f->right.name=model_corresp($3);
  f->FILE=file;
  f->LINE=lineno;
  $$=f;
  /* $$=addchain(addchain(NULL,$1),$3); */
}
|
GEN_TOKEN_IDENT GEN_TOKEN_EQUI digit_number unit
{
  generic_map_info *f=(generic_map_info *)mbkalloc(sizeof(generic_map_info));
  f->left=$1;
  f->type='v';
  f->right.value=$3*$4;
  f->FILE=file;
  f->LINE=lineno;
  $$=f;
  /* $$=addchain(addchain(NULL,$1),$3); */
}
;

unit:
GEN_TOKEN_IDENT
{
  if ($1==unit_M) $$=1.0;
  else if ($1==unit_MM) $$=1e-3;
  else if ($1==unit_UM) $$=1e-6;
  else if ($1==unit_NM) $$=1e-9;
  else if ($1==unit_PM) $$=1e-12;
  else
    {
      fprintf(stderr,"%s:%d: unknown unit '%s'\n",file,lineno,$1); 
      Inc_Error();
      $$=1.0;
    }
}
|
{
  $$=1.0;
}
;

generic_map_implicit: 
GEN_TOKEN_IDENT
{
  generic_map_info *f=(generic_map_info *)mbkalloc(sizeof(generic_map_info));
  f->left=NULL;
  f->type='n';
  f->right.name=model_corresp($1);
  f->FILE=file;
  f->LINE=lineno;
  $$=f;
  //$$=addchain(addchain(NULL,NULL),$1); 
}
|
digit_number unit
{
  generic_map_info *f=(generic_map_info *)mbkalloc(sizeof(generic_map_info));
  f->left=NULL;
  f->type='v';
  f->right.value=$1*$2;
  f->FILE=file;
  f->LINE=lineno;
  $$=f;
}
;

port_map:
  GEN_TOKEN_PORT GEN_TOKEN_MAP  '(' map_list ')'            {$$=$4;}
| GEN_TOKEN_PORT GEN_TOKEN_MAP  '(' equi_list ')'            {$$=$4;}
| GEN_TOKEN_PORT error                
{do  
    yychar = yylex ();
 while ((yychar != ';') && (yychar != GEN_TOKEN_STOP));
 if (yychar==GEN_TOKEN_STOP) return 0;
 yyerrok;
 $$=NULL;
}
;


equi_list:
  equi                                 {$$=$1;}
| equi_list ',' equi                   {$$=PUT_BIN(',',$1,$3);}
;


equi:
  map2 GEN_TOKEN_EQUI {INSTMODE=0;} map2                        {$$=PUT_BIN(GEN_TOKEN_EQUI,$1,$4);INSTMODE=1;}
| error
  {do  {yychar = yylex ();}
   /*synchronisation with a special token*/
   while ((yychar != ',') && (yychar != ')') && (yychar != GEN_TOKEN_STOP));
   if (yychar==GEN_TOKEN_STOP) return 0;
   yyerrok;
   $$=NULL;
  }
;


map2:
  signal_name vector
  {$$=PUT_BIN('(',$1,$2);}
| signal_name    
  {$$=$1;}
| signal_name error
  {do  {yychar = yylex ();}
   /*synchronisation with a special token*/
   while ((yychar != ',') && (yychar != ')') && (yychar != GEN_TOKEN_STOP));
   if (yychar==GEN_TOKEN_STOP) return 0;
   yyerrok;
   $$=NULL;
  }
;


map_list:
  map                                 {$$=$1;}
| map_list ',' map                {$$=PUT_BIN(',',$1,$3);}
;


map:
  signal_name /*{INSTMODE=0;}*/ vector    
  {
    char *name;
    name=(char *)GET_ATO($1);
    CHANGE_ATO($1, model_corresp(instance_reverse_corresp(name)));
    $$=PUT_UNI(GEN_TOKEN_SIGNAL,PUT_BIN('(',$1,$2));
    INSTMODE=1;
  }
| signal_name    
  {
    char *name;
    name=(char *)GET_ATO($1);
    CHANGE_ATO($1, model_corresp(instance_reverse_corresp(name)));
    $$=PUT_UNI(GEN_TOKEN_SIGNAL,$1);
  }
| signal_name error
  {do  {yychar = yylex ();}
   /*synchronisation with a special token*/
   while ((yychar != ',') && (yychar != ')') && (yychar != GEN_TOKEN_STOP));
   if (yychar==GEN_TOKEN_STOP) return 0;
   yyerrok;
   $$=NULL;
  }
;




 /****************************************************************************/
 /*               how to reduce this model in behaviour                      */
 /****************************************************************************/

action_rule:
  global_declaration
  c_function_header
  block 
  { // was C_list
    Add_Global_Decl($1, GLOBAL_EC);
    $$=PUT_BIN(GEN_TOKEN_ACTION,$2,$3);
  }
|
  c_function_header
  block 
  { // was C_list
    $$=PUT_BIN(GEN_TOKEN_ACTION,$1,$2);
  }
;


block:
'{' declarations_list C_list '}'     {
                                        if ($2) {
                                         if ($3) 
                                          $$=PUT_UNI(GEN_TOKEN_BLOCK,PUT_BIN(';',$2,$3));
                                         else $$=PUT_UNI(GEN_TOKEN_BLOCK,$2);
                                        } 
                                        else if ($3) $$=PUT_UNI(GEN_TOKEN_BLOCK,$3);
                                        else $$=PUT_ATO(GEN_TOKEN_NOP,0);
                                     }
|
'{' C_list '}'     {
                       if ($2) $$=PUT_UNI(GEN_TOKEN_BLOCK,$2);
                       else $$=PUT_ATO(GEN_TOKEN_NOP,0);
                   }
;

C_list:
pseudo_C                             { if ($1) $$=$1; else $$=PUT_ATO(GEN_TOKEN_NOP,0); } 
|
block                                { $$=$1;/*$$=NULL;must be caught*/}
| 
C_list block
{
  if ($1) 
    {
      if ($2) $$=PUT_BIN(';',$1,$2); 
      else $$=$1;
    }
  else $$=$2;/*could be NULL*/
}
|
C_list pseudo_C
{
  if ($1) 
    {
      if ($2) $$=PUT_BIN(';',$1,$2); 
      else $$=$1;
    }
  else $$=$2;/*could be NULL*/
}
;

C_or_block:
pseudo_C                             { if ($1) $$=$1; else $$=PUT_ATO(GEN_TOKEN_NOP,0); } 
|
block                                { $$=$1;/*$$=NULL;must be caught*/}
;

pseudo_C:
 regular_expr_2 ';'                               {$$=$1;}
| GEN_TOKEN_FCLOSE '(' string ')' ';'                 {$$=PUT_UNI(GEN_TOKEN_FCLOSE,$3);}
| GEN_TOKEN_FCLOSE '(' ident ')' ';'                  {$$=PUT_UNI(GEN_TOKEN_FCLOSE,$3);}
| GEN_TOKEN_EXIT '(' regular_expr ')' ';'                 {$$=PUT_UNI(GEN_TOKEN_EXIT,$3);}
| GEN_TOKEN_RETURN regular_expr ';'                                   {$$=PUT_ATO(GEN_TOKEN_RETURN,$2);}
| GEN_TOKEN_RETURN ';'                                   {$$=PUT_ATO(GEN_TOKEN_RETURN,NULL);}
| GEN_TOKEN_BREAK ';'                                   {$$=PUT_ATO(GEN_TOKEN_BREAK,0);}
| GEN_TOKEN_PRINTF '(' format ')' ';'                    {$$=PUT_BIN(GEN_TOKEN_FPRINTF, PUT_ATO(GEN_TOKEN_STDOUT,0),$3);}
| GEN_TOKEN_FPRINTF '(' ident ',' format ')' ';'        {$$=PUT_BIN(GEN_TOKEN_FPRINTF,$3,$5);}
| GEN_TOKEN_SPRINTF '(' ident ',' format ')' ';'        {$$=PUT_BIN(GEN_TOKEN_SPRINTF,$3,$5);}
| GEN_TOKEN_IF '(' regular_expr ')' C_or_block               {$$=PUT_BIN(GEN_TOKEN_IF,$3,$5);}
| GEN_TOKEN_WHILE '(' regular_expr ')' C_or_block          {$$=PUT_BIN(GEN_TOKEN_WHILE,$3,$5);}
| GEN_TOKEN_DO block GEN_TOKEN_WHILE '(' regular_expr ')' ';'  {$$=PUT_BIN(GEN_TOKEN_DO,$2,$5);}
| GEN_TOKEN_IF '(' regular_expr ')' C_or_block GEN_TOKEN_ELSE C_or_block {$$=PUT_TRI(GEN_TOKEN_ELSE,$3,$5,$7);}
| GEN_TOKEN_FOR '(' inst_optional ';' expr_optional ';' inst_optional ')'C_or_block
                                             {$$=PUT_QUA(GEN_TOKEN_FOR,$3,$5,$7,$9);}
| ident '(' .args_list. ')' ';'     
{
  chain_list *cl;
  used_func *uf;
  for (cl=GLOBAL_EC->ALL_USED_FUNCTIONS; cl!=NULL; cl=cl->NEXT) 
    {
      uf=(used_func *)cl->DATA;
      if (strcmp(uf->name,getname($1))==0) break;
    }
  $$=PUT_BIN(GEN_TOKEN_FUNCTION,$1,$3); 
  if (cl!=NULL)
    uf->where=addchain(uf->where, Duplicate_Tree($$));
  else
    {
      uf=(used_func *)mbkalloc(sizeof(used_func));
      uf->name=getname($1);
      uf->where=addchain(NULL, Duplicate_Tree($$));
      GLOBAL_EC->ALL_USED_FUNCTIONS=addchain(GLOBAL_EC->ALL_USED_FUNCTIONS, uf);
    }
}
| error
{do  
    yychar = yylex ();
 while ((yychar != ';') && (yychar != GEN_TOKEN_STOP));
 if (yychar==GEN_TOKEN_STOP) return 0;
 yyerrok;
 $$=NULL;
}
;



 /****************************************************************************/
 /*                               Operators C                                */
 /****************************************************************************/

inst_optional:
  /*empty*/                         {$$=PUT_ATO(GEN_TOKEN_NOP,0); /*nothing to do*/}
| regular_expr                      {$$=$1;}
;


expr_optional:
  /*empty*/                         {$$=PUT_ATO(GEN_TOKEN_DIGIT,1); /*always true"for"*/}
| regular_expr                      {$$=$1;}
;


regular_expr:
  exp                               {$$=$1;}
| affect_list                       {$$=$1;}
| regular_expr ',' regular_expr     {$$=PUT_BIN(',',$1,$3);}
;

instruction:
can_be_terminals                 { $$ = $1; }
|
affect_list                       {$$=$1;}
;

regular_expr_2:
instruction                       {$$=$1;}
|
regular_expr_2 ',' instruction    {$$=PUT_BIN(',',$1,$3);}
;


declarations_list:
declaration_type      { $$=$1; }
| declarations_list declaration_type      {if ($1) $$=PUT_BIN(';',$1,$2);
                                           else $$=$2;
                                          }
;

global_declaration: 
declaration { $$=$1; }
|
global_declaration declaration { if ($1) $$=PUT_BIN(';',$1,$2); else $$=$2; }
;

declaration_type:
GEN_TOKEN_STATIC declaration                   {$$=PUT_UNI(GEN_TOKEN_STATIC,$2);}
|
declaration                           {$$=$1;}
;


declaration:
  GEN_TOKEN_INTEGER decl_list ';'                {$$=PUT_UNI(GEN_TOKEN_INTEGER,$2);}
| GEN_TOKEN_LONG decl_list ';'                   {$$=PUT_UNI(GEN_TOKEN_LONG,$2);}
| GEN_TOKEN_CHAR decl_list ';'                   {$$=PUT_UNI(GEN_TOKEN_CHAR,$2);}
| GEN_TOKEN_DOUBLE decl_list ';'                 {$$=PUT_UNI(GEN_TOKEN_DOUBLE,$2);}
| GEN_TOKEN_FILE decl_list ';'                   {$$=PUT_UNI(GEN_TOKEN_FILE,$2);}
| GEN_TOKEN_VOIDTOKEN decl_list ';'              {$$=PUT_UNI(GEN_TOKEN_VOIDTOKEN_D,$2);}
| ident decl_list ';'                   {$$=PUT_BIN(GEN_TOKEN_OTHER_TYPES,$1,$2);}
;


decl_list:
  var                               {$$=$1;}
| affect                            {$$=$1;}
| decl_list ',' var                 {$$=PUT_BIN(',',$1,$3);}
| decl_list ',' affect              {$$=PUT_BIN(',',$1,$3);}
;

affect_list:
  affect                            {$$=$1;}
| var '=' affect_list             {$$=PUT_BIN('=',$1,$3);}
;


affect:
  var '=' exp                        {$$=PUT_BIN('=',$1,$3);}
| var GEN_TOKEN_EG_ADD exp             
  {tree_list *ope=PUT_BIN('+',Duplicate_Tree($1),$3);
   $$=PUT_BIN('=',$1,ope); 
  }
| var GEN_TOKEN_EG_SUB exp        
  {tree_list *ope=PUT_BIN('-',Duplicate_Tree($1),$3);
   $$=PUT_BIN('=',$1,ope); 
  }
| var GEN_TOKEN_EG_MUL exp    
  {tree_list *ope=PUT_BIN('*',Duplicate_Tree($1),$3);
   $$=PUT_BIN('=',$1,ope); 
  }
| var GEN_TOKEN_EG_DIV exp        
  {tree_list *ope=PUT_BIN('/',Duplicate_Tree($1),$3);
   $$=PUT_BIN('=',$1,ope); 
  }
| var GEN_TOKEN_EG_MOD exp        
  {tree_list *ope=PUT_BIN('%',Duplicate_Tree($1),$3);
   $$=PUT_BIN('=',$1,ope); 
  }
| var GEN_TOKEN_EG_AND exp    
  {tree_list *ope=PUT_BIN('&',Duplicate_Tree($1),$3);
   $$=PUT_BIN('=',$1,ope); 
  }
| var GEN_TOKEN_EG_OR exp    
  {tree_list *ope=PUT_BIN('|',Duplicate_Tree($1),$3);
   $$=PUT_BIN('=',$1,ope); 
  }
| var GEN_TOKEN_EG_XOR exp            
  {tree_list *ope=PUT_BIN('^',Duplicate_Tree($1),$3);
   $$=PUT_BIN('=',$1,ope); 
  }
;    

c_known_types:
  GEN_TOKEN_INTEGER        { $$="int"; }
| GEN_TOKEN_LONG           { $$="long"; }
| GEN_TOKEN_CHAR           { $$="char"; }
| GEN_TOKEN_DOUBLE         { $$="double"; }
| GEN_TOKEN_FILE           { $$="FILE"; }
| GEN_TOKEN_VOIDTOKEN      { $$="void"; }
;

cast_and_stuff:
'(' c_known_types pointer_depth ')' exp { $$=PUT_TRI(GEN_TOKEN_CAST, (tree_list *)$2, (tree_list *)(long)$3, $5); }
|
'(' c_known_types ')' exp               { $$=PUT_TRI(GEN_TOKEN_CAST, (tree_list *)$2, (tree_list *)(long)0, $4); }
|
'(' regular_expr ')'                    { $$=$2; }
|
'(' ident pointer_depth ')' exp         { $$=PUT_TRI(GEN_TOKEN_CAST, (tree_list *)$2->DATA, (tree_list *)(long)$3, $5); }
|
'(' ident '*' exp ')' {$$=PUT_BIN('*',$2,$4);}
;


exp:
  terminal            {$$=$1;}
| can_be_terminals              {$$=$1;}
| cast_and_stuff                {$$=$1;}
| exp '+' exp            {$$=PUT_BIN('+',$1,$3);}
| exp '-' exp            {$$=PUT_BIN('-',$1,$3);}    
| exp '*' exp            {$$=PUT_BIN('*',$1,$3);}
| exp '/' exp               {$$=PUT_BIN('/',$1,$3);}
| exp '%' exp              {$$=PUT_BIN('%',$1,$3);}
| exp '>' exp            {$$=PUT_BIN('>',$1,$3);}
| exp '<' exp               {$$=PUT_BIN('<',$1,$3);}
| exp '&' exp               {$$=PUT_BIN('&',$1,$3);}
| exp '|' exp              {$$=PUT_BIN('|',$1,$3);}
| exp '^' exp               {$$=PUT_BIN('^',$1,$3);}
| exp GNS_TOKEN_AND exp                {$$=PUT_BIN(GNS_TOKEN_AND,$1,$3);}
| exp GEN_TOKEN_OR exp                {$$=PUT_BIN(GEN_TOKEN_OR,$1,$3);}
| exp GEN_TOKEN_EG exp               {$$=PUT_BIN(GEN_TOKEN_EG,$1,$3);}
| exp GEN_TOKEN_SUPEG exp        {$$=PUT_BIN(GEN_TOKEN_SUPEG,$1,$3);}
| exp GEN_TOKEN_INFEG exp           {$$=PUT_BIN(GEN_TOKEN_INFEG,$1,$3);}
| exp GEN_TOKEN_NOTEG exp          {$$=PUT_BIN(GEN_TOKEN_NOTEG,$1,$3);}
| exp GEN_TOKEN_SHR exp               {$$=PUT_BIN(GEN_TOKEN_SHR,$1,$3);}
| exp GEN_TOKEN_SHL exp               {$$=PUT_BIN(GEN_TOKEN_SHL,$1,$3);}
| '-' exp                  {$$=PUT_UNI(GEN_TOKEN_OPPOSITE,$2);}
| '+' exp               {$$=$2;}
| '~' exp                  {$$=PUT_UNI('~',$2);}
| '!' exp                   {$$=PUT_UNI(GEN_TOKEN_NOT,$2);}
| exp '?' exp ':' exp           {$$=PUT_TRI('?',$1,$3,$5);}
;

can_be_terminals:
GEN_TOKEN_INC ident                   { $$=PUT_UNI(GEN_TOKEN_INC_BEFORE,$2); }
| GEN_TOKEN_INC ident '[' exp ']'     { $$=PUT_UNI(GEN_TOKEN_INC_BEFORE,PUT_BIN(GEN_TOKEN_ARRAY,$2,$4)); }
| GEN_TOKEN_DEC ident                 { $$=PUT_UNI(GEN_TOKEN_DEC_BEFORE,$2); }
| GEN_TOKEN_DEC ident '[' exp ']'     { $$=PUT_UNI(GEN_TOKEN_DEC_BEFORE,PUT_BIN(GEN_TOKEN_ARRAY,$2,$4)); }
| ident GEN_TOKEN_INC                 { $$=PUT_UNI(GEN_TOKEN_INC_AFTER,$1); }
| ident '[' exp ']' GEN_TOKEN_INC     { $$=PUT_UNI(GEN_TOKEN_INC_AFTER,PUT_BIN(GEN_TOKEN_ARRAY,$1,$3)); }
| ident GEN_TOKEN_DEC             { $$=PUT_UNI(GEN_TOKEN_DEC_AFTER,$1); }
| ident '[' exp ']' GEN_TOKEN_DEC     { $$=PUT_UNI(GEN_TOKEN_DEC_AFTER,PUT_BIN(GEN_TOKEN_ARRAY,$1,$3)); }
| ident '(' .args_list. ')'  
{ 
  chain_list *cl;
  used_func *uf;

  for (cl=GLOBAL_EC->ALL_USED_FUNCTIONS; cl!=NULL; cl=cl->NEXT) 
    {
      uf=(used_func *)cl->DATA;
      if (strcmp(uf->name,getname($1))==0) break;
    }
  $$=PUT_BIN(GEN_TOKEN_FUNCTION,$1,$3); 
  if (cl!=NULL)
    uf->where=addchain(uf->where, Duplicate_Tree($$));
  else
    {
      uf=(used_func *)mbkalloc(sizeof(used_func));
      uf->name=getname($1);
      uf->where=addchain(NULL, Duplicate_Tree($$));
      GLOBAL_EC->ALL_USED_FUNCTIONS=addchain(GLOBAL_EC->ALL_USED_FUNCTIONS, uf);
    }
}
;

terminal:
  digit                 {$$=$1;}
| var                   {$$=$1;}
| string                {$$=$1;}
| GEN_TOKEN_NULL                 { $$=PUT_ATO(GEN_TOKEN_NULL,0); }
| '&' ident             { $$=PUT_UNI(GEN_TOKEN_ADR,$2); }
| '&' ident '[' exp ']' { $$=PUT_UNI(GEN_TOKEN_ADR,PUT_BIN(GEN_TOKEN_ARRAY,$2,$4)); }
;

var:
  ident                 {$$=$1;}
| ident '[' exp ']'     {$$=PUT_BIN(GEN_TOKEN_ARRAY,$1,$3);}
| pointer_depth ident   {$$=PUT_BIN(GEN_TOKEN_REF,$2,(tree_list *)(long)$1);}
| pointer_depth ident '[' exp ']'   {$$=PUT_BIN(GEN_TOKEN_REF,PUT_BIN(GEN_TOKEN_ARRAY,$2,$4),(tree_list *)(long)$1);}
;

pointer_depth:
'*' { $$=1; }
|
pointer_depth '*' { $$ = $1+1; }
;

 /****************************************************************************/
 /*              format for arguments of functions C                         */
 /****************************************************************************/
 

.args_list.:
args_list { $$= $1; }
| { $$ = NULL;}
;

args_list:
  args_list ',' affect_or_exp { $$=PUT_BIN(',',$1,$3); }
| affect_or_exp { $$ = $1; }
;


affect_or_exp:
exp { $$ = $1; }
|
affect { $$ = $1; }
;

format:
  ident                             {$$=$1;}
| string                            {$$=$1;}
| string ',' regular_expr           {$$=PUT_BIN(',',$1,$3);}
;


string_args:
  string                            {$$=$1;}
| string_args ',' string            {$$=PUT_BIN(',',$1,$3);}
;


string:
  string_list           {$$=PUT_ATO(GEN_TOKEN_STRING,$1);}
;


string_list:
  GEN_TOKEN_STRING               {$$=$1;}
| string_list GEN_TOKEN_STRING   {char *a;                       /*mbkalloc*/
                         a=(char *)mbkalloc(strlen($1)+strlen($2)+1);
                         strcpy(a,$1);
                         strcat(a,$2);
                         mbkfree($1); mbkfree($2);
                         $$=a;   /* a = "$1" "$2"   concatenated  */
                        }
;


 
 
 /****************************************************************************/
 /*                                Operators VHDL                            */
 /****************************************************************************/

vector:
  '(' exp_vhdl GEN_TOKEN_TO exp_vhdl ')'                {$$=PUT_BIN(GEN_TOKEN_TO,$2,$4);}
| '(' exp_vhdl GEN_TOKEN_DOWNTO exp_vhdl ')'             {$$=PUT_BIN(GEN_TOKEN_DOWNTO,$2,$4);}
| '(' exp_vhdl ')'                              {$$=$2;}
;


array:
  '(' exp_vhdl GEN_TOKEN_TO exp_vhdl ')'                {$$=PUT_BIN(GEN_TOKEN_TO,$2,$4);}
| '(' exp_vhdl GEN_TOKEN_DOWNTO exp_vhdl ')'             {$$=PUT_BIN(GEN_TOKEN_DOWNTO,$2,$4);}
;


exp_vhdl:
  terminal_vhdl                    {$$=$1;}
| '(' exp_vhdl ')'                 {$$=$2;}
| exp_vhdl '+' exp_vhdl            {$$=PUT_BIN('+',$1,$3);}
| exp_vhdl '-' exp_vhdl            {$$=PUT_BIN('-',$1,$3);}   
| exp_vhdl '*' exp_vhdl            {$$=PUT_BIN('*',$1,$3);}
| exp_vhdl '/' exp_vhdl            {$$=PUT_BIN('/',$1,$3);}
| exp_vhdl GEN_TOKEN_MOD exp_vhdl           {$$=PUT_BIN(GEN_TOKEN_MOD,$1,$3);}
| exp_vhdl GEN_TOKEN_REM exp_vhdl           {$$=PUT_BIN('%',$1,$3);}
| exp_vhdl GEN_TOKEN_POW exp_vhdl           {$$=PUT_BIN(GEN_TOKEN_POW,$1,$3);}
| GEN_TOKEN_ABSOL exp_vhdl                    {$$=$2;/*absolute value of an integer*/} 
| '-' exp_vhdl                     {$$=PUT_UNI(GEN_TOKEN_OPPOSITE,$2);}
| '+' exp_vhdl                     {$$=$2;}
;


boolean_vhdl:
  '(' boolean_vhdl ')'             {$$=$2;}
| exp_vhdl '>' exp_vhdl            {$$=PUT_BIN('>',$1,$3);}
| exp_vhdl '<' exp_vhdl            {$$=PUT_BIN('<',$1,$3);}
| exp_vhdl GEN_TOKEN_EG exp_vhdl            {$$=PUT_BIN(GEN_TOKEN_EG,$1,$3);}
| exp_vhdl GEN_TOKEN_SUPEG exp_vhdl         {$$=PUT_BIN(GEN_TOKEN_SUPEG,$1,$3);}
| exp_vhdl GEN_TOKEN_INFEG exp_vhdl         {$$=PUT_BIN(GEN_TOKEN_INFEG,$1,$3);}
| exp_vhdl GEN_TOKEN_NOTEG exp_vhdl         {$$=PUT_BIN(GEN_TOKEN_NOTEG,$1,$3);}
| boolean_vhdl GEN_TOKEN_XOR boolean_vhdl   {$$=PUT_BIN(GEN_TOKEN_XOR,$1,$3);}
| boolean_vhdl GEN_TOKEN_NAND boolean_vhdl  {$$=PUT_UNI(GEN_TOKEN_NOT,PUT_BIN(GNS_TOKEN_AND,$1,$3));}
| boolean_vhdl GEN_TOKEN_NOR boolean_vhdl   {$$=PUT_UNI(GEN_TOKEN_NOT,PUT_BIN(GEN_TOKEN_OR,$1,$3));}
| boolean_vhdl GNS_TOKEN_AND boolean_vhdl   {$$=PUT_BIN(GNS_TOKEN_AND,$1,$3);}
| boolean_vhdl GEN_TOKEN_OR boolean_vhdl    {$$=PUT_BIN(GEN_TOKEN_OR,$1,$3);}
| GEN_TOKEN_NOT boolean_vhdl                {$$=PUT_UNI(GEN_TOKEN_NOT,$2);}
;
  

terminal_vhdl:
  digit                      {$$=$1;}
| ident                      {$$=$1;}
;


module_name:
  ident                      {$$=$1;}
;


block_name:
  ident                      {$$=$1;}
;


model_name:
  ident                      {$$=$1;}
;


signal_name:
  ident                      {$$=$1;}
;


ident_list:
  ident                           {$$=addchain(NULL,$1);}
| ident_list ',' ident          {$$=addchain($1,$3);/*warning reverse chain*/}
;




 /****************************************************************************/
 /*                               VHDL and C                                 */
 /****************************************************************************/

digit:
GEN_TOKEN_DIGIT            {$$=PUT_ATO(GEN_TOKEN_DIGIT,(long)$1);}
| 
GEN_TOKEN_DIGIT_DOUBLE           {$$=PUT_ATO(GEN_TOKEN_DIGIT_DOUBLE,$1); }
| 
GEN_TOKEN_DIGIT_CHAR             {$$=PUT_ATO(GEN_TOKEN_DIGIT_CHAR,(long)$1); }
;

digit_number:
GEN_TOKEN_DIGIT            { $$=(double)$1;}
| 
GEN_TOKEN_DIGIT_DOUBLE           { $$=*$1; mbkfree($1); }
;

ident:
  GEN_TOKEN_IDENT               
  {
    if (INSTMODE)
      $$=PUT_ATO(GEN_TOKEN_IDENT,instance_corresp($1));
    else if (MODELMODE) 
      $$=PUT_ATO(GEN_TOKEN_IDENT,model_corresp($1)); 
    else
      $$=PUT_ATO(GEN_TOKEN_IDENT,$1);
  }
;


/* zinaps: genius language extension, C syntax */

c_function_header:
  GEN_TOKEN_FONCTION_HEADER c_function_params ')' 
{
  char *type, *name;
  t_arg *ta;
  int pointer, err=0;
  char buf[2048];
  strcpy(buf, $1);
  processfunctionheader(buf, &type, &pointer, &name);
  if (
      strcmp(type,"void")!=0
      && strcmp(type,"int")!=0
      && strcmp(type,"double")!=0
      && strcmp(type,"char")!=0
      && strcmp(type,"long")!=0
      && pointer==0
      )
    {
      fprintf(stderr,"%s:%d: function return type must be a pointer for type '%s'\n",file,lineno,type); 
      Inc_Error();
      err=1;
    }
  if (err==0)
    {
      ta=APINewTARG(type, pointer+1, name);
      ta->POINTER--; // pour pouvoir allouer un 'void'
    }
  else ta=NULL;
  $$=PUT_QUA(GEN_TOKEN_OF,PUT_ATO(GEN_TOKEN_IDENT,name),
             PUT_ATO(GEN_TOKEN_IDENT,name),
             PUT_ATO(GEN_TOKEN_VOIDTOKEN_CL_TA,$2),
             PUT_ATO(GEN_TOKEN_VOIDTOKEN_CL_TA,addchain(NULL,ta))
             );
}
;

c_function_params:
  c_function_param_list { $$ = $1; }
|
{ $$ = NULL; }
;

c_function_param_list:
c_variable            { $$=$1;}
| 
c_variable ',' c_function_param_list { $1->NEXT=$3; $$ = $1; }
;

c_variable:
c_known_types pointer_depth GEN_TOKEN_IDENT { $$=APIAddTARG(NULL, $1, $2, $3);}
|
c_known_types GEN_TOKEN_IDENT { $$=APIAddTARG(NULL, $1, 0, $2);}
|
GEN_TOKEN_IDENT pointer_depth GEN_TOKEN_IDENT { $$=APIAddTARG(NULL, $1, $2, $3); }
;

%%


/*
| GEN_TOKEN_EXCLUDE '(' string_args ')' ';'           {$$=PUT_UNI(GEN_TOKEN_EXCLUDE,$3);}
*/

 /****************************************************************************/
 /*              user's message for syntaxical error                         */
 /****************************************************************************/
static inline int yyerror()
{
  switch ((int)yychar) {
     case GEN_TOKEN_STOP: 
    fprintf(stderr,"%s:%d: unexpected end of file\n",file,lineno); 
     break;
     case ';': case ',': default: 
    fprintf(stderr,"%s:%d: syntax error happened at '%s'\n",file,lineno,yytext);
     break;
  }
  Inc_Error();
  return lineno;
}



 /****************************************************************************/
 /*                          to get started                                  */
 /* files_list is a list of file names associated to a priority              */
 /****************************************************************************/
extern void init_lexer();
static int API_IN_PARSER=0;

int APIInParser()
{
    return API_IN_PARSER;
}

void APIParseFile_SetTemplateInfo(ht *head)
{
  API_TEMPLATE_HT=head;
}

char *model_corresp(char *name)
{
  char res[1024];
  if (API_MODEL_DEFINES==NULL) return namealloc(name);
  if (gen_find_template_corresp(NULL, API_MODEL_DEFINES, name, res)==0)
    return namealloc(res);
  return namealloc(name);
}

static char *instance_corresp(char *name)
{
  char res[1024];
  if (API_INST_ADTS==NULL) return namealloc(name);
  if (gen_find_template_corresp(API_TEMPLATE_HT, API_INST_ADTS->defines, name, res)==0)
    return namealloc(res);
  return namealloc(name);
}

static template_corresp *getadts(char *name)
{
  if (API_TEMPLATE_HT==NULL) return NULL;
  return gen_get_template_corresp(API_TEMPLATE_HT, name);
}

static char *instance_reverse_corresp(char *name)
{
  char res[1024];
  if (API_INST_ADTS==NULL) return namealloc(name);
  if (gen_find_reverse_template_corresp(API_INST_ADTS->defines, name, res)==0)
    return namealloc(res);
  return namealloc(name);
}

tree_list *APIParseFile(FILE *f, char *filename, ExecutionContext *ec, chain_list *defines)
{
//  char *src, *dest;
  unit_M=namealloc("m");
  unit_MM=namealloc("mm");
  unit_UM=namealloc("um");
  unit_NM=namealloc("nm");
  unit_PM=namealloc("pm");

  GLOBAL_EC=ec;
  API_MODEL_DEFINES=defines, API_INST_ADTS=NULL;
  MODELMODE=0, INSTMODE=0;

  yyin = f;
#if 0
  if (defines==NULL)
    {
      yyin = f;
      genius_yyin=NULL;
    }
  else
    {
      int size;
      src=mbkalloc(MAX_VHD_RULE_SIZE);
      dest=mbkalloc(MAX_VHD_RULE_SIZE);
      size=fread(src, sizeof(char), MAX_VHD_RULE_SIZE, f);
      src[size]='\0';
      APIPreprocess(src, dest, defines);
      mbkfree(src);
      yyin=NULL;
      genius_yyin=dest;
    }
#endif
  file=sensitive_namealloc(filename);
  lineno=1;
  init_lexer();
  API_IN_PARSER=1;
  yyparse();
  API_IN_PARSER=0;
#if 0
  if (defines!=NULL) mbkfree(dest);
#endif
  return main_tree;
}


 /****************************************************************************/
 /*              take a list of typed 'type' connectors name                 */
 /*               and build a REVERSE interface                              */
 /****************************************************************************/
static inline tree_list *to_interface_connectors(kind,connectors_list,type,dimension)
  tree_list *dimension;
  chain_list *connectors_list;
  int kind,type;
{
  tree_list *var,*ret;
  if (!connectors_list) {
     fprintf(stderr,"to_interface_connectors: NULL pointer\n");
     EXIT(1);
  }
  /*build current*/
  if (dimension) {
     var=PUT_BIN('(',connectors_list->DATA,dimension);
      ret=PUT_UNI(type,var);
      ret=PUT_UNI(kind,ret);
  }
  else {
     ret=PUT_UNI(type,connectors_list->DATA);
      ret=PUT_UNI(kind,ret);
  }
  /*build the next*/
  if (connectors_list->NEXT) {
     var=to_interface_connectors( kind, connectors_list->NEXT,type,
                                  dimension?Duplicate_Tree(dimension):NULL);
     ret=PUT_BIN(',',var,ret);
  }
  return ret;
}


 /****************************************************************************/
 /*              take a list of typed 'type' connectors name                 */
 /*               and build a REVERSE interface                              */
 /****************************************************************************/
static inline tree_list *to_internal_connectors(kind,connectors_list,type,dimension)
  tree_list *dimension;
  chain_list *connectors_list;
  int kind,type;
{
  tree_list *var,*ret;
  if (!connectors_list) {
     fprintf(stderr,"to_internal_connectors: NULL pointer\n");
     EXIT(1);
  }
  /*build current*/
  if (dimension) {
     var=PUT_BIN('(',connectors_list->DATA,dimension);
      ret=PUT_UNI(type,var);
      ret=PUT_UNI(kind,ret);
  }
  else {
     ret=PUT_UNI(type,connectors_list->DATA);
      ret=PUT_UNI(kind,ret);
  }
  /*build the next*/
  if (connectors_list->NEXT) {
     var=to_internal_connectors( kind, connectors_list->NEXT,type,
                                  dimension?Duplicate_Tree(dimension):NULL);
     ret=PUT_BIN(';',var,ret);
  }
  return ret;
}


void processfunctionheader(char *str, char **type, int *pointer, char **name)
{
  int i=0, last;
  char c;
  while (str[i]!=' ' && str[i]!='\n' && str[i]!='\t' && str[i]!='*') i++;
  c=str[i]; str[i]='\0';
  *type=sensitive_namealloc(str);
  str[i]=c;
  while (str[i]==' ' || str[i]=='\n' || str[i]=='\t') i++;
  last=i;
  *pointer=0;
  if (str[i]=='*') 
    {
      do {
        (*pointer)++;
        i++;
      } while (str[i]=='*');
      while (str[i]==' ' || str[i]=='\n' || str[i]=='\t') i++;
      last=i;
    }
  while (str[i]!=' ' && str[i]!='\n' && str[i]!='\t' && str[i]!='(') i++;
  str[i]='\0';
  *name=sensitive_namealloc(&str[last]);
//  printf(">> %s %d %s\n",*type, *pointer, *name);
}
 /****************************************************************************/
 /*************************** END of LEX&YACC ********************************/
 /****************************************************************************/

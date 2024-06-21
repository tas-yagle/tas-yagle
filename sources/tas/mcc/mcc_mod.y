/******************************************************************************/
/*                                                                            */
/*                      Chaine de CAO & VLSI   AVERTEC                        */
/*                                                                            */
/*    Fichier : mcc_mod.y                                                     */
/*                                                                            */
/*                                                                            */
/*    (c) copyright 2001 AVERTEC                                              */
/*    Tous droits reserves                                                    */
/*                                                                            */
/*    Auteur(s) : Marc KUOCH                                                  */
/*----------------------------------------------------------------------------*/
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/

%{
#include MCC_H
#include MSL_H
#include "mcc_mod_util.h"

/******************************************************************************/
/* defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* functions                                                                  */
/******************************************************************************/


/******************************************************************************/
/* private                                                                    */
/******************************************************************************/

char *LIBTYPE = NULL ;
char *CURLIBTYPE = NULL ; 
char *FILENAME = NULL ; 

static mcc_modellist *MCC_MODEL = NULL ;
//static mcc_explist * MCC_EXP = NULL ; 
static char *SUBCKTNAME = NULL ;
static char *EXPRESSION = NULL ;
static lofig_list *LOFIG = NULL ;
static eqt_ctx *LOFIG_CTX = NULL ;
static lotrs_list *LOTRS = NULL ;
static ht *HTABSIG = NULL ;
static int SIGINDEX = 0 ;
static chain_list *SIGLIST ; 


/******************************************************************************/
/* function declarations                                                      */
/******************************************************************************/

extern int MCC_LINE;
extern void pushcontext() ;
extern void yyinit() ;
chain_list *mcc_name2sig(chain_list *) ;
void mcc_expformat(char *) ;
int yyerror();
int yylex();

%}

%union {
  char                                *text ;
  int                                  integer ;
  double                               real ;
} ;

%token T_GIGA  T_KILO T_MILI T_MICRO T_NANO T_PICO T_FEMTO
%token T_LIB T_ENDL T_SUBCKT T_ENDS
%token T_MODEL T_PMOS T_NMOS T_VARD T_VAR T_INCLUDE
%token T_RESISTOR T_CAPACITOR T_DIODE T_ELEMENT T_VALUE 
%token <real> T_NUMBER 
%token <real> T_VALUE 
%token <text> T_EQUATION 
%token <text> T_STRING
%token <text> T_TRANSISTOR
%token <text> T_INSTANCE

%type <text> equation
%type <integer> model_type
%type <real> number elemval
%type <real> value numb_ext

%start spiceobj_list

%%

spiceobj_list      : // {mcc_moddebug = 1;}
                   | spiceobj_list spiceobj
                   {
                    MCC_HEADTECHNO->EXPR = (mcc_explist *)reverse((chain_list *)MCC_HEADTECHNO->EXPR) ;
                   }
                   ;

spiceobj           : variable
                   | model 
                   | libfile
                   | include
                   | library
                   | subcircuit
                   ; 

subcircuit         : T_SUBCKT T_STRING 
                   {
                    SUBCKTNAME = namealloc ($2) ;
                    if((CURLIBTYPE == LIBTYPE) || (CURLIBTYPE == NULL))
                     {
                      if(getloadedlofig(SUBCKTNAME) == NULL)
                       {
                        LOFIG = addlofig(SUBCKTNAME) ;
                        LOFIG_CTX = eqt_init(13);
                        locklofig(LOFIG) ;
                        HTABSIG = addht(10) ;
                       }
                      else
                       {
                        LOFIG = NULL ;
                        LOFIG_CTX = NULL ;
                       }
                     }
                    else
                     {
                      LOFIG = NULL ;
                      LOFIG_CTX = NULL ;
                     }
                    mbkfree($2) ;
                   } 
                   name_list defvariable
                   {
                    ptype_list *ptype ;
                    chain_list *chain ;
                    if(LOFIG != NULL)
                     {
                      SIGLIST = reverse(SIGLIST) ;
                      spi_addpreloadedlib(LOFIG->NAME,dupchainlst(SIGLIST)) ;
                      mcc_name2sig(SIGLIST) ;
                      for(chain = SIGLIST ; chain != NULL ; chain = chain->NEXT)
                       {
                        addlocon(LOFIG,getsigname((losig_list *)chain->DATA),
                                (losig_list *)chain->DATA,UNKNOWN) ;
                        ((losig_list *)chain->DATA)->TYPE = EXTERNAL ;
                       }
                      freechain(SIGLIST) ;
                      SIGLIST = NULL ;
                      ptype = getptype(LOFIG->USER,MCC_MODEL_EXPR) ;
                      if(ptype != NULL)
                      ptype->TYPE = MCC_DEFMODEL_EXPR ;
                      LOFIG->USER = addptype (LOFIG->USER, PARAM_CONTEXT, eqt_export_vars (LOFIG_CTX));
                      LOFIG_CTX = NULL;
                     }
                   }
                   element_list ends
                   {
                    lotrs_list *trs ;
                    mcc_modellist *model ;
                    ptype_list *ptype ;
                    if(LOFIG != NULL)
                    for(trs = LOFIG->LOTRS ; trs != NULL ; trs = trs->NEXT)
                      {
                       for(model =  MCC_HEADTECHNO->MODEL ; 
                           model != NULL ; model = model->NEXT)
                         {
                          if(model->NAME == getlotrsmodel(trs)) 
                           {
                            if(model->TYPE == MCC_PMOS)
                              trs->TYPE = TRANSP ;
                            else
                              trs->TYPE = TRANSN ;
                            break ;
                           }
                         }
                      }
                    if(LOFIG != NULL)
                      ptype = getptype(LOFIG->USER,MCC_MODEL_EXPR) ;
                    else
                      ptype = NULL ;
                    if(ptype != NULL)
                      ptype->DATA = reverse((chain_list *)ptype->DATA) ;
                    SIGINDEX = 0 ;
                    LOFIG = NULL ;
                    LOFIG_CTX = NULL ;
                    freechain(SIGLIST) ;
                    SIGLIST = NULL ;
                    if(HTABSIG != NULL)
                     {
                      delht(HTABSIG) ;
                      HTABSIG = NULL ;
                     }
                   }
                   ;

ends               :
                   T_ENDS T_STRING
                   {
                    SUBCKTNAME = NULL ; 
                    mbkfree($2) ;
                   }
                   | T_ENDS
                   {
                    SUBCKTNAME = NULL ; 
                   }
                   ; 

name_list          : 
                   | name_list name
                   ;

name              : T_STRING 
                   {
                    if(LOFIG != NULL)
                      SIGLIST = addchain(SIGLIST,namealloc($1)) ;
                    mbkfree($1) ;
                   }
                   | T_NUMBER
                   {
                    char buf[1024] ;

                    if(LOFIG != NULL)
                     {
                      sprintf(buf,"mbk_sig%ld",(long)$1) ;
                      SIGLIST = addchain(SIGLIST,namealloc(buf)) ;
                     }
                   }
                   ;

element_list       : 
                   | element_list element
                   ;

element            : model
                   | variable
                   | transistor
                   | diode
                   | resistor
                   | capacitor
                   | instance
                   | other
                   ;

transistor         : T_TRANSISTOR name name name name T_STRING 
                   {
                    if(LOFIG != NULL)
                     {
                      mcc_name2sig(SIGLIST) ;
                      LOTRS = addlotrs(LOFIG,TRANSN,(long)0,(long)0,
                              0,0,0,0,0,0,
                              (losig_list *)SIGLIST->NEXT->NEXT->DATA,
                              (losig_list *)SIGLIST->NEXT->DATA,
                              (losig_list *)SIGLIST->NEXT->NEXT->NEXT->DATA,
                              (losig_list *)SIGLIST->DATA,
                              namealloc($1+1)) ;
                               
                      addlotrsmodel(LOTRS,$6) ;
                      freechain(SIGLIST) ;
                      SIGLIST = NULL ;
                     }
                    mbkfree($1) ;
                    mbkfree($6) ;
                   }
                   expression_list
                   {
                    ptype_list *ptype ;

                    if(LOTRS != NULL)
                      ptype = getptype(LOTRS->USER,MCC_MODEL_EXPR) ;
                    else
                      ptype = NULL ;
                    if(ptype != NULL)
                      ptype->DATA = reverse((chain_list *)ptype->DATA) ;
                    LOTRS = NULL ;
                   }
                   ;

instance           : T_INSTANCE name_list expression_list
                   {
                    loins_list * loins ;

                    if(LOFIG != NULL)
                     {
                      SIGLIST->NEXT = reverse(SIGLIST->NEXT) ; 
                      loins = addloins(LOFIG,namealloc($1+1),
                                       getlofig((char *)SIGLIST->DATA,'P'),
                                       mcc_name2sig(SIGLIST->NEXT)) ;
                      freechain(SIGLIST) ;
                      SIGLIST = NULL ;
                     }
                    mbkfree($1) ;
                   }
                   ;

diode              : T_DIODE name name T_STRING expression_list
                    {
                     mbkfree($4) ;
                    }
                   ;

resistor           : T_RESISTOR name name elemval expression_list
                   ;

capacitor          : T_CAPACITOR name name elemval expression_list
                   ;

other              : T_ELEMENT  name_list elemval
                   ;

elemval            : T_VALUE '=' value
                   {
                    $$ = $3 ;
                   }
                   | value
                   {
                    $$ = $1 ;
                   }
                   ;

library            : T_LIB T_STRING
                   {
                    CURLIBTYPE = namealloc($2) ;
                    mbkfree($2) ;
                   } 
                   spiceobj_list endl
                   ; 

endl               :
                   T_ENDL T_STRING
                   {
                    CURLIBTYPE = NULL ;
                    mbkfree($2) ;
                   }
                   | T_ENDL
                   {
                    CURLIBTYPE = NULL ;
                   }
                   ; 

libfile            : T_LIB T_STRING T_STRING
                   {
                    if((CURLIBTYPE == LIBTYPE) || (CURLIBTYPE == NULL))
                     {
                      pushcontext($2) ;
                      LIBTYPE = namealloc($3) ;
                      mbkfree($2) ;
                      mbkfree($3) ;
                     }
                   }
                   ;

include            : T_INCLUDE T_STRING
                   {
                    if((CURLIBTYPE == LIBTYPE) || (CURLIBTYPE == NULL))
                     {
                      pushcontext($2) ;
                      mbkfree($2) ;
                     }
                   }
                   ;

defvariable        : 
                   | T_VARD expression_list
                   ;

variable           : T_VAR expression_list
                   ;

expression_list    : 
                   | expression_list expression
                   ;

expression         : T_STRING '=' value
                   { 
                    ptype_list **ptuser ;
                    ptype_list *ptype ;
                    char *pt ;

                    if((CURLIBTYPE == LIBTYPE) || (CURLIBTYPE == NULL))
                     {
                      downstr($1,$1) ;
                      if(LOFIG == NULL)
                       {
                        MCC_HEADTECHNO->EXPR = mcc_addexp(MCC_HEADTECHNO->EXPR,
                                                          $1,EXPRESSION,$3) ;
                        if(EXPRESSION == NULL)
                         {
                          pt = $1 ;
                          downstr(pt,pt) ;
                          eqt_addvar(mccEqtCtx,namealloc(pt),$3) ;
                         }
                       }
                      else
                       {
                        if(LOTRS != NULL)
                          ptuser = &LOTRS->USER ;
                        else
                          ptuser = &LOFIG->USER ;
                        ptype = getptype(*ptuser,MCC_MODEL_EXPR) ;
                        if(ptype == NULL)
                         {
                          *ptuser = addptype(*ptuser,MCC_MODEL_EXPR,NULL) ;
                          ptype =  *ptuser ;
                         }
                        ptype->DATA = mcc_addexp((mcc_explist *)ptype->DATA,
                                                 $1,EXPRESSION,$3) ;
                        if(LOTRS != NULL)
                         {
                          addlotrsparam(LOTRS, $1, $3, EXPRESSION);
                         }

                        if(LOFIG_CTX != NULL)
                         {
                          pt = $1 ;
                          downstr(pt,pt) ;
                          eqt_addvar(LOFIG_CTX, namealloc(pt), $3);
                         }
                       }
                       EXPRESSION = NULL ;
                     }
                    mbkfree($1) ;
                   }
                   ;

model              : T_MODEL T_STRING model_type
                   {
                     char *name;
                     if((CURLIBTYPE == LIBTYPE) || (CURLIBTYPE == NULL))
                      {
                       MCC_MODEL = mcc_addmodel(MCC_HEADTECHNO->NAME, $2, 
                                                SUBCKTNAME, $3, MCC_TYPICAL) ;
                       name = namealloc ($2);
                       switch ( $3 ) {
                         case MCC_NMOS : if (!mbk_istransn(name))
                                            TNMOS = addchain(TNMOS, name);
                                            break;
                         case MCC_PMOS : if (!mbk_istransp(name))
                                            TPMOS = addchain(TPMOS, name);
                                            break;
                       }
                      }
                     mbkfree($2) ;
                   }
                   param_list
                   ;

model_type         : T_NMOS
                   {
                    $$ = MCC_NMOS ;
                   }
                   | T_PMOS
                   {
                    $$ = MCC_PMOS ;
                   }
                   | T_STRING
                   {
                    $$ = MCC_UNKNOWN ;
                    mbkfree($1) ;
                   }
                   ;

param_list         : 
                   | param_list param
                   ;

param              : T_STRING '=' value
                   {
                     if((CURLIBTYPE == LIBTYPE) || (CURLIBTYPE == NULL))
                      {
                       downstr($1,$1) ;
                       mcc_setparam (MCC_MODEL, $1, $3) ;
                       if(EXPRESSION != NULL)
                         {
                          mcc_paramlist *pt = mcc_chrparam(MCC_MODEL,$1) ;

                          pt->EXPR = mcc_addexp(pt->EXPR,$1,EXPRESSION,$3) ;
                          mbkfree(EXPRESSION) ;
                          EXPRESSION = NULL ;
                         }
                      }
                     mbkfree($1) ;
                   }
                   ;

value              : number
                   | numb_ext 
                   ;

numb_ext           : number T_FEMTO
                   {
                     $$ = $1 * 1e-15 ;
                     mcc_expformat("1e-15") ;
                   }
                   | number T_PICO
                   {
                     $$ = $1 * 1e-12 ;
                     mcc_expformat("1e-12") ;
                   }
                   | number T_NANO
                   {
                     $$ = $1 * 1e-9 ;
                     mcc_expformat("1e-9") ;
                   }
                   | number T_MICRO
                   {
                     $$ = $1 * 1e-6 ;
                     mcc_expformat("1e-6") ;
                   }
                   | number T_MILI
                   {
                     $$ = $1 * 1e-3 ;
                     mcc_expformat("1e-3") ;
                   }
                   | number T_KILO
                   {
                     $$ = $1 * 1e+3 ;
                     mcc_expformat("1e+3") ;
                   }
                   | number T_GIGA
                   {
                     $$ = $1 * 1e+9 ;
                     mcc_expformat("1e+9") ;
                   }
                   ;
                   
number             : T_NUMBER
                   {
                     $$ = $1 ;
                     EXPRESSION = NULL ;
                   }
                   | equation
                   {
                    if((CURLIBTYPE == LIBTYPE) || (CURLIBTYPE == NULL))
                      {
                       if(EXPRESSION != NULL)
                         mbkfree(EXPRESSION) ;
                       EXPRESSION = $1 ;
                       downstr(EXPRESSION,EXPRESSION) ;
                       if(LOFIG == NULL)
                        {
                         $$ = eqt_eval(mccEqtCtx,EXPRESSION,EQTFAST) ; 
                         if(eqt_resistrue(mccEqtCtx) == 1)
                          {
                           mbkfree(EXPRESSION) ;
                           EXPRESSION = NULL ;
                          }
                        }
                      }
                   }
                   ;

equation           : T_EQUATION
                   | T_STRING
                   {
                    $$ = $1 ;
                   }
                   ;

%%

int yyerror ()
{
    fprintf (stderr, "\nMCC ERROR: parse error in %s line %d\n", FILENAME, MCC_LINE) ;
    EXIT (0) ;
    return 0;
}

void mcc_expformat(format)
char *format ;
{
 char buf[1024] ;

 if(((CURLIBTYPE == LIBTYPE) || (CURLIBTYPE == NULL)) && (EXPRESSION != NULL))
   {
    sprintf(buf,"%s * %s",EXPRESSION,format) ;
    mbkfree(EXPRESSION) ;
    EXPRESSION = mbkstrdup(buf) ;
   }
}

chain_list *mcc_name2sig(sigchain)
chain_list *sigchain ;
{
 chain_list *chain ;
 losig_list *losig ;
 char *signame ;

 for(chain = sigchain ; chain != NULL ; chain = chain->NEXT)
  {
   if((losig = (losig_list *)gethtitem(HTABSIG,(char *)chain->DATA)) == (losig_list *)EMPTYHT)
    {
     signame = chain->DATA ;
     chain->DATA = addlosig(LOFIG,++SIGINDEX,addchain(NULL,signame),INTERNAL) ;
     addhtitem(HTABSIG,signame,(long)chain->DATA) ;
    }
   else
    chain->DATA = losig ;
  }
 return(chain) ;
}


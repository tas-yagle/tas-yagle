/******************************************************************************/
/*                                                                            */
/*                      Chaine de CAO & VLSI   AVERTEC                        */
/*                                                                            */
/*    Produit : TLF Version 1.00                                              */
/*    Fichier : tlf.y                                                         */
/*                                                                            */
/*    (c) copyright 2000 AVERTEC                                              */
/*    Tous droits reserves                                                    */
/*                                                                            */
/*    Auteur(s) : Gilles Augustins                                            */
/*                                                                            */
/******************************************************************************/

%{
#include "tlf_parse.h"
#include "tlf_env.h"
#include "tlf_util.h"

/******************************************************************************/
/* private                                                                    */
/******************************************************************************/
static cbhcomb  *ptrcbhcomb;
static cbhseq   *ptrcbhseq;
static short     mout;

typedef struct minmax {
    float MIN;
    float MAX;
} minmax;

typedef struct loconbus {
    char *NAME;
    losig_list *LOSIG;
    char DIREC;
} loconbus;

static chain_list        *output;

static long               losig_index;
static lofig_list        *ptrlofig;
static losig_list        *ptrlosig;
static locon_list        *ptrlocon;

static char *cname;

static char               namebuf[1024];
static chain_list        *ch, *chp, *xchain, *ychain, *xydatamin, *xydatamax, *ydatamin, *ydatamax;
static char               xtype, ytype;
static char               dimension;
static int                nx, ny;
static float             *ptfl;
static timing_model      *modelmin;
static timing_model      *modelmax;
static timing_ttable     *ttemplate;
static char              *cname, *pt;
static char              *version;
static ttvfig_list       *fig;
static int                nbconsig, nbbreaksig, nblatchsig;
static chain_list        *consigchain, *breaksigchain, *latchsigchain;
static ttvevent_list     *root, *node;
static ttvline_list      *line;
static ht                *modshrinkht;
static ht                *pinht;
static ht                *pincapht;
static ht                *doublemodels;
static chain_list        *clocks;
static int                i, l, m, lsb, msb, flagbus = 0;
static ttvsig_list       *sigroot, *signode, *sig;
static char              *pinr, *pinn, *pname, *bname, *vectname;
static char              *mindlymname, *maxdlymname, *minslwmname, *maxslwmname;
static long               linetype;
static long               capa;
static float              slwmin, slwmax, dlymin, dlymax, maxcstr;
static float             *caparray;
static float              pincapa = TLF_NO_CAPA, buscapa = TLF_NO_CAPA;
static int                v;
static timing_model      *delaymodelmin, *slewmodelmin;
static timing_model      *delaymodelmax, *slewmodelmax;
static long               dir;
static chain_list        *flipflops;
static chain_list        *latchs;
static char               pindir;
static int                clockpin;
static float              defbidircapa = 0;
static float              defincapa    = 0;
static float              defoutcapa   = 0;
static minmax            *ptmm;
static segment           *ptseg;
static double             fallthmin = 0.1, fallthmax = 0.9;
static double             risethmin = 0.1, risethmax = 0.9;
static double             inputfalldth = 0.5, outputfalldth = 0.5;
static double             inputrisedth = 0.5, outputrisedth = 0.5;
static double             voltage, temperature;
static int                itype, otype;
static timing_props      *properties;

typedef struct statevar {
    char *PINNAME;
    char *VAR;
} statevar;

statevar          *cbhname1=NULL; 
statevar          *cbhname2=NULL;



/******************************************************************************/
/* function declarations                                                      */
/******************************************************************************/

int yyerror();
int yylex();

%}

%union {
  char                                         *text;
  float                                         real;
  struct chain                                 *chainlist;
  struct { struct chain *CHAIN; char TYPE; } axedef;
  struct { struct chain *CHAIN; char DIM;  } datadef;
  struct { float MIN; float MAX; }           valdef;
  struct { char *MIN; char *MAX; }           bitext;
  char                                         cchar;
  long                                         llong;
  struct segment                               *ssegment;
};

%token            TLF4_TOKEN_01
%token            TLF4_TOKEN_0Z
%token            TLF4_TOKEN_10
%token            TLF4_TOKEN_1Z
%token            TLF4_TOKEN_ARROW "=>"
%token            TLF4_TOKEN_BUS_ARROW "*>"
%token            TLF4_TOKEN_ASYNCH
%token <text>     TLF4_TOKEN_ATTRIBUTE
%token            TLF4_TOKEN_BIDIR
%token            TLF4_TOKEN_BUS
%token            TLF4_TOKEN_CAP_UNIT
%token            TLF4_TOKEN_CAPACITANCE
%token            TLF4_TOKEN_CELL
%token            TLF4_TOKEN_CELLTYPE
%token            TLF4_TOKEN_CLEAR
%token            TLF4_TOKEN_CLEAR_PRESET_VAR1
%token            TLF4_TOKEN_CLEAR_PRESET_VAR2
%token            TLF4_TOKEN_CLOCK_PIN
%token            TLF4_TOKEN_CLOCK_REG
%token            TLF4_TOKEN_CLOCK_SLEW_AXIS
%token            TLF4_TOKEN_COMB
%token            TLF4_TOKEN_CONST
%token            TLF4_TOKEN_CONTROL
%token            TLF4_TOKEN_DATA
%token            TLF4_TOKEN_DATE
%token            TLF4_TOKEN_DEFINE_ATTRIBUTE
%token            TLF4_TOKEN_DELAY
%token            TLF4_TOKEN_ENABLE
%token            TLF4_TOKEN_ENERGY
%token <real>     TLF4_TOKEN_ENUMBER
%token            TLF4_TOKEN_ENVIRONMENT
%token <text>     TLF4_TOKEN_EXPR
%token            TLF4_TOKEN_FALL
%token            TLF4_TOKEN_FAST
%token            TLF4_TOKEN_FOR_BITS
%token            TLF4_TOKEN_FOR_PIN
%token            TLF4_TOKEN_FUNCTION
%token            TLF4_TOKEN_GROUND
%token            TLF4_TOKEN_SUPPLY
%token            TLF4_TOKEN_GENMODEL
%token            TLF4_TOKEN_HEADER
%token            TLF4_TOKEN_HIGH
%token            TLF4_TOKEN_HOLD
%token <text>     TLF4_TOKEN_IDENT
%token            TLF4_TOKEN_IGNORE 
%token            TLF4_TOKEN_INCELL
%token            TLF4_TOKEN_INPUT
%token            TLF4_TOKEN_INPUT_REG
%token            TLF4_TOKEN_INPUT_SLEW_AXIS
%token            TLF4_TOKEN_INTERNAL
%token            TLF4_TOKEN_INVERTED_OUTPUT
%token            TLF4_TOKEN_LATCH
%token            TLF4_TOKEN_LIBRARY
%token            TLF4_TOKEN_LOAD_AXIS
%token            TLF4_TOKEN_LOAD2_AXIS
%token            TLF4_TOKEN_LOW
%token            TLF4_TOKEN_LOWER_THRESHOLD
%token            TLF4_TOKEN_INPUT_THRESHOLD
%token            TLF4_TOKEN_OUTPUT_THRESHOLD
%token            TLF4_TOKEN_MODEL
%token            TLF4_TOKEN_NEGEDGE
%token <real>     TLF4_TOKEN_NUMBER
%token            TLF4_TOKEN_OUTCELL
%token            TLF4_TOKEN_OUTPUT
%token            TLF4_TOKEN_PATH
%token            TLF4_TOKEN_PIN
%token            TLF4_TOKEN_PINDIR
%token            TLF4_TOKEN_PINTYPE
%token            TLF4_TOKEN_POSEDGE
%token            TLF4_TOKEN_PROPERTIES
%token <text>     TLF4_TOKEN_QSTRING
%token            TLF4_TOKEN_REGISTER
%token            TLF4_TOKEN_RISE
%token            TLF4_TOKEN_SEQ
%token            TLF4_TOKEN_SET
%token            TLF4_TOKEN_SETUP
%token            TLF4_TOKEN_SLAVE_CLOCK
%token            TLF4_TOKEN_SLEW
%token            TLF4_TOKEN_SLOW
%token            TLF4_TOKEN_SLEW_AXIS
%token            TLF4_TOKEN_SPLINE
%token            TLF4_TOKEN_TECHNOLOGY
%token            TLF4_TOKEN_TIME_UNIT
%token            TLF4_TOKEN_RES_UNIT
%token            TLF4_TOKEN_TIMING_PROPS
%token            TLF4_TOKEN_TLF_VERSION
%token            TLF4_TOKEN_TRISTATE
%token            TLF4_TOKEN_UNIT
%token            TLF4_TOKEN_UPPER_THRESHOLD
%token <text>     TLF4_TOKEN_VECT
%token <text>     TLF4_TOKEN_VECT_IDENT
%token            TLF4_TOKEN_VENDOR
%token            TLF4_TOKEN_VERSION
%token            TLF4_TOKEN_Z0
%token            TLF4_TOKEN_Z1
%token            TLF4_TOKEN_VDROP_LIMIT
%token            TLF4_TOKEN_TRANSITION_START
%token            TLF4_TOKEN_TRANSITION_END
%token            TLF4_TOKEN_LINEAR
%token            TLF4_TOKEN_VALUE
%token            TLF4_TOKEN_NET_CAP_MODEL
%token            TLF4_TOKEN_NET_RES_MODEL
%token            TLF4_TOKEN_NET_CAP
%token            TLF4_TOKEN_NET_RES
%token            TLF4_TOKEN_TEMPERATURE
%token            TLF4_TOKEN_VOLTAGE

%start tlf_file 

%type <text>      arg
%type <chainlist> array1D 
%type <chainlist> array2D
%type <chainlist> array1D_list
%type <axedef>    axe 
%type <cchar>     axetype 
%type <cchar>     bustype
%type <text>      cellname
%type <cchar>     celltype 
%type <cchar>     clocktrans
%type <valdef>    constant
%type <valdef>    constdelay
%type <valdef>    constslew
%type <bitext>    constraint
%type <valdef>    cval
%type <chainlist> cval_list
%type <chainlist> segment_list
%type <datadef>   data 
%type <bitext>    delay 
%type <text>      ident 
%type <chainlist> ident_list 
%type <datadef>   modeldata 
%type <text>      modelname
%type <chainlist> outputs
%type <real>      pincap
%type <text>      pinname
%type <cchar>     pintype
%type <bitext>    slew 
%type <cchar>     target
%type <text>      templatename
%type <cchar>     transition 
%type <chainlist> val_list
%type <real>      val
%type <real>      capacitance
%type <ssegment>  segment

%%

tlf_file     : header def_list cell_list 
             {
                if(TLF_TRACE_MODE == 'Y')
                    fprintf (stdout, "   <--- done\n");
/*                else
                    fprintf(stdout,"DONE!\n");*/
                cbh_delcct();
             }
             ;

def_list     : empty
             | def def_list
             ;

def          : defattr
             | genmod
             | netcapmodel
             | netresmodel
             | gentprop
             | properties
             | attribute
             ;
             

defattr      : TLF4_TOKEN_DEFINE_ATTRIBUTE '(' ident target type ')'
             {
               set_attribute ($3, $4);
             }
             ;

target       : '(' TLF4_TOKEN_CELL ')'
             {
               $$ = 'C';
             }
             | '(' TLF4_TOKEN_LIBRARY ')'
             {
               $$ = 'L';
             }
             | '(' TLF4_TOKEN_PIN ')'
             {
               $$ = 'P';
             }
             ;

type         : '(' ident ')'
             ;

genmod       : TLF4_TOKEN_GENMODEL '(' templatename '(' modelbody ')' ')' 
             { 
                for (nx = 0, ch = xchain; ch; ch = ch->NEXT) nx++;
                for (ny = 0, ch = ychain; ch; ch = ch->NEXT) ny++;
                ttemplate = stm_modtbl_addtemplate ($3, nx, ny, xtype, ytype);
                if (xtype == STM_INPUT_SLEW || xtype == STM_CLOCK_SLEW) {
                    if (TLF_TIME_UNIT == TLF_NS)
                        stm_modtbl_settemplateXrange (ttemplate, xchain, 1e3);
                    if (TLF_TIME_UNIT == TLF_PS)
                        stm_modtbl_settemplateXrange (ttemplate, xchain, 1);
                }
                if (xtype == STM_LOAD) {
                    if (TLF_CAPA_UNIT == TLF_PF)
                        stm_modtbl_settemplateXrange (ttemplate, xchain, 1e3);
                    if (TLF_CAPA_UNIT == TLF_FF)
                        stm_modtbl_settemplateXrange (ttemplate, xchain, 1);
                }
                if (ytype == STM_INPUT_SLEW || ytype == STM_CLOCK_SLEW) {
                    if (TLF_TIME_UNIT == TLF_NS)
                        stm_modtbl_settemplateYrange (ttemplate, ychain, 1e3);
                    if (TLF_TIME_UNIT == TLF_PS)
                        stm_modtbl_settemplateYrange (ttemplate, ychain, 1);
                }
                if (ytype == STM_LOAD) {
                    if (TLF_CAPA_UNIT == TLF_PF)
                        stm_modtbl_settemplateYrange (ttemplate, ychain, 1e3);
                    if (TLF_CAPA_UNIT == TLF_FF)
                        stm_modtbl_settemplateYrange (ttemplate, ychain, 1);
                }
                for (ch = xchain; ch; ch = ch->NEXT)  
                    mbkfree(ch->DATA);
                freechain (xchain); 
                for (ch = ychain; ch; ch = ch->NEXT)  
                    mbkfree(ch->DATA);
                freechain (ychain); 
             } 
             ;

gentprop     : TLF4_TOKEN_TIMING_PROPS '(' ')'
             ;

properties   : TLF4_TOKEN_PROPERTIES '(' prop_list ')'
             {
             float scale;
               if(properties){
                 if(properties->RESMODEL){
                     if(TLF_RES_UNIT == TLF_KOHM)
                         scale = 1000.0;
                     else if(TLF_RES_UNIT == TLF_OHM)
                         scale = 1.0;
                     stm_scale_loadmodel(properties->RESMODEL,scale);
                 }
                 if(properties->CAPMODEL){
                     if(TLF_CAPA_UNIT == TLF_PF)
                         scale = 1000.0;
                     else if(TLF_CAPA_UNIT == TLF_FF)
                         scale = 1.0;
                     stm_scale_loadmodel(properties->CAPMODEL,scale);
                 }
               }
             }
             ;

prop_list    : empty
             | prop_list prop
             ;

prop         : TLF4_TOKEN_UNIT '(' unit_list ')' 
             | TLF4_TOKEN_FOR_PIN '(' TLF4_TOKEN_INPUT capacitance ')'
             {
                defincapa = $4;
             }
             | TLF4_TOKEN_FOR_PIN '(' TLF4_TOKEN_OUTPUT capacitance ')'
             {
                defoutcapa = $4;
             }
             | TLF4_TOKEN_FOR_PIN '(' TLF4_TOKEN_BIDIR capacitance ')'
             {
                defbidircapa = $4;
             }
             | TLF4_TOKEN_TEMPERATURE '(' val ')'
             {
                temperature = $3;
             }
             | TLF4_TOKEN_VOLTAGE '(' val ')'
             {
                voltage = $3;
             }
             | delay_th 
             {
                if((inputrisedth != inputfalldth) || (inputrisedth != inputfalldth) || (inputrisedth != inputfalldth))
                   fprintf (stderr, "***tlf warning: input and output thresholds are not the same***\n"); 
             }
             | threshold 
             {
                if((risethmin != fallthmin) || (risethmax != fallthmax))
                   fprintf (stderr, "***tlf warning: slew thresholds are not the same for rising and falling***\n");
             }
             | TLF4_TOKEN_NET_CAP '(' ident ')'
             {
                if(!STM_PROPERTIES)
                    properties = stm_prop_create (NULL, NULL);
                else
                    properties = STM_PROPERTIES;
                if(!properties->CAPMODEL)
                    stm_addcapmodel(properties, stm_getmodel(cname, $3));
             }
             | TLF4_TOKEN_NET_RES '(' ident ')'
             {
                if(!STM_PROPERTIES)
                    properties = stm_prop_create (NULL, NULL);
                else
                    properties = STM_PROPERTIES;
                if(!properties->RESMODEL)
                    stm_addresmodel(properties, stm_getmodel(cname, $3));
             }
             ;

delay_th     : TLF4_TOKEN_INPUT_THRESHOLD '(' TLF4_TOKEN_RISE '(' val ')' TLF4_TOKEN_FALL '(' val ')' ')'
             {
               inputrisedth = $5 / 100;
               inputfalldth = $9 / 100;
             }
             | TLF4_TOKEN_OUTPUT_THRESHOLD '(' TLF4_TOKEN_RISE '(' val ')' TLF4_TOKEN_FALL '(' val ')' ')'
             {
               outputrisedth = $5 / 100;
               outputfalldth = $9 / 100;
             }
             ;
             
threshold    : TLF4_TOKEN_LOWER_THRESHOLD '(' TLF4_TOKEN_RISE '(' val ')' TLF4_TOKEN_FALL '(' val ')' ')'
             {
               risethmin = $5 / 100;
               fallthmin = $9 / 100;
             }
             | TLF4_TOKEN_UPPER_THRESHOLD '(' TLF4_TOKEN_RISE '(' val ')' TLF4_TOKEN_FALL '(' val ')' ')'
             {
               risethmax = $5 / 100;
               fallthmax = $9 / 100;
             }
             | TLF4_TOKEN_TRANSITION_START '(' val ')'
             {
               risethmin = $3;
               fallthmin = $3;
             }
             | TLF4_TOKEN_TRANSITION_END '(' val ')'
             {
               risethmax = $3;
               fallthmax = $3;
             }
             ;


capacitance  : TLF4_TOKEN_CAPACITANCE '(' val ')'
             {
                $$ = $3;
             }
             ;

unit_list    : empty
             | unit_list unit 
             ;

unit         : TLF4_TOKEN_CAP_UNIT '(' val TLF4_TOKEN_IDENT ')'
             {
                if (!strcasecmp ($4, "pf"))
                    TLF_CAPA_UNIT = TLF_PF;
                if (!strcasecmp ($4, "ff"))
                    TLF_CAPA_UNIT = TLF_FF;
             }
             | TLF4_TOKEN_TIME_UNIT '(' val TLF4_TOKEN_IDENT ')'
             {
                if (!strcasecmp ($4, "ps"))
                    TLF_TIME_UNIT = TLF_PS;
                if (!strcasecmp ($4, "ns"))
                    TLF_TIME_UNIT = TLF_NS;
             }
             | TLF4_TOKEN_RES_UNIT '(' val TLF4_TOKEN_IDENT ')'
             {
                if (!strcasecmp ($4, "kohm"))
                    TLF_RES_UNIT = TLF_KOHM;
                if (!strcasecmp ($4, "ohm"))
                    TLF_RES_UNIT = TLF_OHM;
             }
             ;

attribute    : TLF4_TOKEN_ATTRIBUTE '(' arg ')'
             {
               char *str;

               if((!strcasecmp($1,"state_variable_map")) && (ptrcbhcomb != NULL)){
                   if(cbhname1 == NULL){
                        cbhname1 = (statevar*)mbkalloc(sizeof(struct statevar));
                        cbhname1->PINNAME = mbkstrdup(pname);
                        cbhname1->VAR = mbkstrdup($3);
                        if(ptrcbhcomb->FUNCTION == NULL) {
                            str = eqt_ConvertStr($3);
                            ptrcbhcomb->FUNCTION = eqt_StrToAbl(tlfEqtCtx,str);
                            mbkfree (str);
                        }
                   }
                   else {
                        if(strcasecmp(cbhname1->VAR,$3)) {
                            cbhname2 = (statevar*)mbkalloc(sizeof(struct statevar));
                            cbhname2->PINNAME =  mbkstrdup(pname);
                            cbhname2->VAR = mbkstrdup($3);
                            if(ptrcbhcomb->FUNCTION == NULL) {
                                str = eqt_ConvertStr($3);
                                ptrcbhcomb->FUNCTION = eqt_StrToAbl(tlfEqtCtx,str);
                                mbkfree (str);
                            }
                        }
                        else {
                            mout = tlf_multout(&ptrlofig,&ptrcbhcomb,&ptrcbhseq,cname);
                            cbhname1 = NULL;
                        }
                   }
               }

               callback_attribute ($1, $3);
             }
             ;

arg          : TLF4_TOKEN_QSTRING
             {
               $$ = stm_unquote ($1);
               mbkfree($1);
             }
             | ident
             {
               $$ = $1;
             }
             | val
             {
               char buf[TLF_BUFSIZE];
               sprintf(buf,"%g",$1);
               $$ = namealloc(buf);
             }

             ;

header       : TLF4_TOKEN_HEADER '(' helem_list ')'
             {
/*               if(TLF_TRACE_MODE != 'Y') {
                   fprintf(stdout,"\nLOADING LIBRARY %s TLF4... ", LIBRARY_TLF_NAME);
                   fflush( stdout );
               }*/
               output = NULL;
               losig_index = 0;
               ptrlofig = NULL;
               ptrlosig = NULL;
               ptrlocon = NULL;
               ptrcbhcomb = NULL; 
               ptrcbhseq = NULL;
               
               pincapa = TLF_NO_CAPA;
               buscapa = TLF_NO_CAPA;
               defbidircapa = 0;
               defincapa    = 0;
               defoutcapa   = 0;
               fallthmin = 0.1;
               fallthmax = 0.9;
               risethmin = 0.1; 
               risethmax = 0.9;
               //flagbus = 0;

               tlf4reset();
               
               cbh_newclassifier();
               cbh_createcct();
             }
             ;

helem_list   : empty
             | helem_list helem
             ;

helem        : library
             | technology
             | date
             | vendor
             | environment
             | version
             | tlf_version
             ;

library      : TLF4_TOKEN_LIBRARY '(' TLF4_TOKEN_QSTRING ')'
             {
                nbconsig    = 0;
                nbbreaksig  = 0;
                nblatchsig  = 0;
                consigchain    = NULL;
                breaksigchain  = NULL;
                latchsigchain  = NULL;
                clocks = NULL;
                
                tlf_setenv();
                LIBRARY_TLF_NAME = tlf_treatname($3);
                stm_addcell (LIBRARY_TLF_NAME);
                cname = LIBRARY_TLF_NAME;
                mbkfree($3);
             }
             ;

technology   : TLF4_TOKEN_TECHNOLOGY '(' TLF4_TOKEN_QSTRING ')'
             {
                mbkfree($3);
             }
             ;

date         : TLF4_TOKEN_DATE '(' TLF4_TOKEN_QSTRING ')'
             {
                mbkfree($3);
             }
             ;

vendor       : TLF4_TOKEN_VENDOR '(' TLF4_TOKEN_QSTRING ')'
             {
                mbkfree($3);
             }
             ;

environment  : TLF4_TOKEN_ENVIRONMENT '(' TLF4_TOKEN_QSTRING ')'
             {
                mbkfree($3);
             }
             ;

version      : TLF4_TOKEN_VERSION '(' TLF4_TOKEN_QSTRING ')'
             {
                mbkfree($3);
             }
             ;

tlf_version  : TLF4_TOKEN_TLF_VERSION '(' TLF4_TOKEN_QSTRING ')'
             {
                mbkfree($3);
             }
             ;

cell_list    : empty
             | cell_list cell 
             ;
             
cell         : TLF4_TOKEN_CELL '(' celldef cellelms arcs ')' 
             {
                char         buf[1024];
                locon_list  *p;
                cbhcomb     *c;
                char        *str;

                
                if((ptrcbhseq != NULL) && (!mout)){
                    if(cbhname1 != NULL){
                        if(!strcasecmp(cbhname1->PINNAME,ptrcbhseq->PIN))
                            ptrcbhseq->NAME = namealloc(cbhname1->VAR);
                        else if(!strcasecmp(cbhname1->PINNAME,ptrcbhseq->NEGPIN))
                            ptrcbhseq->NEGNAME = namealloc(cbhname1->VAR);
                    }
                    if(cbhname2 != NULL){
                        if(!strcasecmp(cbhname2->PINNAME,ptrcbhseq->PIN))
                            ptrcbhseq->NAME = namealloc(cbhname2->VAR);
                        else if(!strcasecmp(cbhname2->PINNAME,ptrcbhseq->NEGPIN))
                            ptrcbhseq->NEGNAME = namealloc(cbhname2->VAR);
                    }

                    if((ptrcbhseq->NAME == NULL) && (ptrcbhseq->PIN !=NULL)) {
                        sprintf(buf,"I%s",ptrcbhseq->PIN);
                        ptrcbhseq->NAME = namealloc(buf);
                    }
                    
                    if((ptrcbhseq->NEGNAME == NULL) && (ptrcbhseq->NEGPIN != NULL)){
                        sprintf(buf,"I%s",ptrcbhseq->NEGPIN);
                        ptrcbhseq->NEGNAME = namealloc(buf);
                    }                    

                    if(ptrcbhseq->PIN != NULL) {
                        p = getlocon(ptrlofig, ptrcbhseq->PIN);        
                        c = cbh_getcombfromlocon(p);
                        if(!c->FUNCTION) {
                            str = eqt_ConvertStr(ptrcbhseq->NAME);
                            c->FUNCTION = eqt_StrToAbl(tlfEqtCtx,str);
                            mbkfree (str);
                        }
                        if((ptrcbhseq->DATA != NULL) && (tlf_lookingForExpr(ptrcbhseq->DATA, ptrcbhseq->PIN) == 1)) 
                            ptrcbhseq->DATA = tlf_replaceInAbl(ptrcbhseq->DATA, ptrcbhseq->PIN, ptrcbhseq->NAME);    
                    }
                    
                    if(ptrcbhseq->NEGPIN != NULL) {
                        p = getlocon(ptrlofig, ptrcbhseq->NEGPIN);        
                        c = cbh_getcombfromlocon(p);
                        if(!c->FUNCTION) {
                            str = eqt_ConvertStr(ptrcbhseq->NEGNAME);
                            c->FUNCTION = eqt_StrToAbl(tlfEqtCtx,str);
                            mbkfree (str);
                        }
                        if((ptrcbhseq->DATA != NULL) && (tlf_lookingForExpr(ptrcbhseq->DATA, ptrcbhseq->NEGPIN) == 1))
                            ptrcbhseq->DATA = tlf_replaceInAbl(ptrcbhseq->DATA, ptrcbhseq->NEGPIN, ptrcbhseq->NEGNAME);
                    }

                    cbh_addseqtolofig(ptrlofig, ptrcbhseq);
                }
                 
                if(mout == 0)
                    cbh_classlofig(ptrlofig);
                 
                
                for (i = 0; i < fig->NBCONSIG; i++) {
                    sig = fig->CONSIG[i];
                    if ((sig->TYPE & TTV_SIG_B) == TTV_SIG_B) {
                        breaksigchain = addchain(breaksigchain,sig);
                        nbbreaksig++;
                    }
                    if ((sig->TYPE & TTV_SIG_L) == TTV_SIG_L) {
                        latchsigchain = addchain(latchsigchain,sig);
                        nblatchsig++;
                    }
                }

                fig->NBEBREAKSIG = nbbreaksig;
                fig->EBREAKSIG   = ttv_allocreflist (breaksigchain, nbbreaksig);
                fig->NBELATCHSIG = nblatchsig;
                fig->ELATCHSIG   = ttv_allocreflist (latchsigchain, nblatchsig);
                
                for (i = 0; i < fig->NBCONSIG; i++) {
                    capa = gethtitem (pincapht, namealloc (fig->CONSIG[i]->NAME));
                    if (capa != EMPTYHT)
                        mbkfree((float*)capa);
                }
                        
                ptrcbhseq = NULL;
                cbhname1 = NULL;
                cbhname2 = NULL;
                delht (modshrinkht);
                delht (pinht);
                delht (pincapht);
                delht (doublemodels);
                freechain (clocks);
                freechain (flipflops);
                freechain (latchs);
                clocks        = NULL;
                flipflops     = NULL;
                latchs        = NULL;
                nbbreaksig    = 0;
                latchsigchain = NULL;
                nblatchsig    = 0;
                breaksigchain = NULL;
                fig->STATUS |= TTV_STS_DTX; 
                fig->STATUS |= TTV_STS_TTX; 
             }
             ;

cellelms     : cellelm_list
             {
                fig->CONSIG = ttv_allocreflist (consigchain, nbconsig);
                fig->NBCONSIG = nbconsig;
                for (i = 0; i < fig->NBCONSIG; i++) 
                    addhtitem (pinht, namealloc (fig->CONSIG[i]->NAME), (long)fig->CONSIG[i]);
                consigchain = NULL;
                nbconsig = 0;
             }
             ;

cellelm_list : empty
             | cellelm_list cellelm
             ;

cellelm      : model
             | directive
             | pin
             | registers
             | celltprops
             | attribute
             ;

registers    : TLF4_TOKEN_REGISTER '(' 
             {
                if(ptrcbhseq != NULL) {
                    mout = tlf_multout(&ptrlofig,&ptrcbhcomb,&ptrcbhseq,cname);
                }
                else {
                    ptrcbhseq = cbh_newseq();
                    ptrcbhseq->SEQTYPE = CBH_FLIPFLOP;
                }
             }
              reg_list ')'
             {
                if(ptrcbhseq != NULL)
                    flipflops = append (flipflops, output);
                output = NULL;
             }
             | TLF4_TOKEN_LATCH    '('
             {
                if(ptrcbhseq != NULL) {
                    mout = tlf_multout(&ptrlofig,&ptrcbhcomb,&ptrcbhseq,cname);
                }

                else {
                    ptrcbhseq = cbh_newseq();
                    ptrcbhseq->SEQTYPE = CBH_LATCH;             
                }
             }
              reg_list ')'
             {
                if(ptrcbhseq != NULL)
                    latchs = append (latchs, output);
                output = NULL;
             }
             ;
             
reg_list : empty{}
         | reg_list outputs 
         | reg_list reg_elem {}
         ;

reg_elem : TLF4_TOKEN_CLEAR TLF4_TOKEN_EXPR
         { 
            char *str;
            if(ptrcbhseq != NULL) {
                str = eqt_ConvertStr($2);
                ptrcbhseq->RESET = eqt_StrToAbl(tlfEqtCtx,str);
                mbkfree (str);
            }
         }
         | TLF4_TOKEN_CLOCK_REG TLF4_TOKEN_EXPR
         {
            char *str;
            if(ptrcbhseq != NULL) {
                str = eqt_ConvertStr($2);
                ptrcbhseq->CLOCK = eqt_StrToAbl(tlfEqtCtx,str);
                mbkfree (str);
            }
         }
         | TLF4_TOKEN_SLAVE_CLOCK TLF4_TOKEN_EXPR
         {
            char *str;
            if(ptrcbhseq != NULL) {
                str = eqt_ConvertStr($2);
                ptrcbhseq->SLAVECLOCK = eqt_StrToAbl(tlfEqtCtx,str);
                mbkfree (str);
            }
         }
         | TLF4_TOKEN_SET TLF4_TOKEN_EXPR
         {
            char *str;
            if(ptrcbhseq != NULL) {
                str = eqt_ConvertStr($2);
                ptrcbhseq->SET = eqt_StrToAbl(tlfEqtCtx,str);
                mbkfree (str);
            }
         }
         | TLF4_TOKEN_INPUT_REG TLF4_TOKEN_EXPR
         {
            char *str;
            if(ptrcbhseq != NULL) { 
                str = eqt_ConvertStr($2);
                ptrcbhseq->DATA = eqt_StrToAbl(tlfEqtCtx,str);
                mbkfree (str);
            }
         }
         | TLF4_TOKEN_CLEAR_PRESET_VAR1 TLF4_TOKEN_EXPR
         {
            char *str;
            if(ptrcbhseq != NULL) {
                str = eqt_ConvertStr($2);
                ptrcbhseq->RSCONF = eqt_StrToAbl(tlfEqtCtx,str);
                mbkfree (str);
            }
         }
         | TLF4_TOKEN_CLEAR_PRESET_VAR2 TLF4_TOKEN_EXPR
         {
            char *str;
            if(ptrcbhseq != NULL) {
                str = eqt_ConvertStr($2);
                ptrcbhseq->RSCONFNEG = eqt_StrToAbl(tlfEqtCtx,str);
                mbkfree (str);
            }
         }
         ;

outputs      : TLF4_TOKEN_OUTPUT '(' ident_list ')' 
             {
                if($3->NEXT != NULL)
                     mout = tlf_multout(&ptrlofig,&ptrcbhcomb,&ptrcbhseq,cname);
                else {
                    if(ptrcbhseq != NULL)
                       ptrcbhseq->PIN = namealloc(tlf_chainlistToStr($3)); 
                }
                if (output == NULL)
                    output = $3;
                else
                    output = append(output,$3);
             }
             | TLF4_TOKEN_INVERTED_OUTPUT '(' ident_list ')'
             {
                if($3->NEXT != NULL)
                     mout = tlf_multout(&ptrlofig,&ptrcbhcomb,&ptrcbhseq,cname);
                else {
                    if(ptrcbhseq != NULL)
                        ptrcbhseq->NEGPIN = namealloc(tlf_chainlistToStr($3));
                }
                if (output == NULL)
                    output = $3;
                else
                    output = append(output,$3);
             }
             ;

ident_list   : ident 
             {
                $$ = addchain (NULL, $1);
             }
             | ident ident_list 
             {
                $$ = addchain ($2, $1);
             }
             ;

celldef      : cellname celltype 
             {
                lofig_list *p;

                cname = $1;
                mout = 0;
                if(TLF_TRACE_MODE == 'Y')
                    fprintf (stdout, "   ---> processing cell %s\n", cname);

                /* nouvelle cellule --> lofig a creer */
                losig_index = 1;
                if((p=getloadedlofig(cname)) == NULL) {
                    ptrlofig = addlofig(cname);
                    ptrlofig->MODE = 'P';
                    addcatalog(ptrlofig->NAME);
                }

                else
                    ptrlofig = p;

                version = (char*) mbkalloc (TLF_BUFSIZE * sizeof (char));
                sprintf (version, "%.2f", elpTechnoVersion);
                stm_addcell ($1);
                fig = ttv_givehead ($1, $1, NULL); 
                ttv_lockttvfig(fig);
                ttv_setttvlevel (fig);
                TLF_TTVFIG_LIST = addchain (TLF_TTVFIG_LIST, fig);
                fig->INFO->TOOLNAME      = TLF_TOOLNAME;
                fig->INFO->TOOLVERSION   = TLF_TOOLVERSION;
                fig->INFO->TECHNONAME    = elpTechnoName;
                fig->INFO->TECHNOVERSION = version;
                fig->INFO->SLOPE         = STM_DEF_SLEW;
                fig->INFO->CAPAOUT       = STM_DEF_LOAD;
                fig->INFO->STHHIGH       = risethmax;
                fig->INFO->STHLOW        = risethmin;
                fig->INFO->DTH           = outputrisedth;
   	            fig->INFO->TEMP          = temperature;
   	            fig->INFO->VDD           = voltage;
                ttv_setttvdate (fig, TTV_DATE_LOCAL);
                modshrinkht = addht (100);
                pinht = addht (100);
                pincapht = addht (100);
                doublemodels = addht (100);
             }
             ;

directive    : ident
             {
             }
             ;

celltype     : TLF4_TOKEN_CELLTYPE '(' TLF4_TOKEN_IGNORE ')' 
             {
                $$ = STM_IGNORE;
             }
             | TLF4_TOKEN_CELLTYPE '(' TLF4_TOKEN_OUTCELL ')' 
             {
                $$ = STM_IGNORE;
             }
             | TLF4_TOKEN_CELLTYPE '(' TLF4_TOKEN_INCELL ')' 
             {
                $$ = STM_IGNORE;
             }
             | TLF4_TOKEN_CELLTYPE '(' TLF4_TOKEN_INCELL TLF4_TOKEN_OUTCELL ')' 
             {
                $$ = STM_IGNORE;
             }
             | TLF4_TOKEN_CELLTYPE '(' TLF4_TOKEN_SEQ ')' 
             {
                $$ = STM_SEQ;
             }
             | TLF4_TOKEN_CELLTYPE '(' TLF4_TOKEN_COMB ')' 
             {
                $$ = STM_COMB;
             }
             | empty
             {
                $$ = STM_IGNORE;
             }
             ;

celltprops   : TLF4_TOKEN_TIMING_PROPS '(' ')'
             ;

arcs         : arc_list
             {
             
                for (i = 0; i < fig->NBCONSIG; i++) {
                    sig = fig->CONSIG[i];

                    /* typer B, LL (latch), LF (flip-flop) */
                    
                    for (line = sig->NODE[0].INLINE; line; line = line->NEXT) {
                        /* access */
                        if ((line->TYPE & TTV_LINE_A) == TTV_LINE_A) { 
                            switch (getoutputtype (sig->NAME, flipflops, latchs)) {
                                case 'B' :
                                    sig->TYPE |= TTV_SIG_B;
                                    break;
                                case 'L' :
                                    sig->TYPE |= TTV_SIG_LL;
                                    break;
                                case 'F' :
                                    sig->TYPE |= TTV_SIG_LF;
                                    break;
                            }
                            line->NODE->ROOT->TYPE |= TTV_SIG_B;
                            break;
                        }
                        /* hold, setup */
                        if (((line->TYPE & TTV_LINE_O) == TTV_LINE_O) || ((line->TYPE & TTV_LINE_U) == TTV_LINE_U)) { 
                            sig->TYPE |= TTV_SIG_B;
                            line->NODE->ROOT->TYPE |= TTV_SIG_B;
                        }
                    }
                    
                    for (line = sig->NODE[1].INLINE; line; line = line->NEXT) {
                        /* access */
                        if ((line->TYPE & TTV_LINE_A) == TTV_LINE_A) { 
                            switch (getoutputtype (sig->NAME, flipflops, latchs)) {
                                case 'B' :
                                    sig->TYPE |= TTV_SIG_B;
                                    break;
                                case 'L' :
                                    sig->TYPE |= TTV_SIG_LL;
                                    break;
                                case 'F' :
                                    sig->TYPE |= TTV_SIG_LF;
                                    break;
                            }
                            line->NODE->ROOT->TYPE |= TTV_SIG_B;
                            break;
                        }
                        /* hold, setup */
                        if (((line->TYPE & TTV_LINE_O) == TTV_LINE_O) || ((line->TYPE & TTV_LINE_U) == TTV_LINE_U)) { 
                            sig->TYPE |= TTV_SIG_B;
                            line->NODE->ROOT->TYPE |= TTV_SIG_B;
                        }
                    }
                }
             }
             ;


netcapmodel  : TLF4_TOKEN_NET_CAP_MODEL '(' modelname '(' modelbody ')' ')'
             {
                modelmax = NULL;
                for (nx = 0, ch = xchain; ch; ch = ch->NEXT) nx++;
                if (xydatamax && !xydatamin) {
                    sprintf (namebuf, "%s", $3);
                    modelmax = stm_addtblmodel (cname, namebuf, nx, 0, xtype, STM_NOTYPE);
                    stm_storemodel (cname, namebuf, modelmax, 0);
                }
                if (xtype == STM_LOAD) {
                    stm_modtbl_setXrange (modelmax->UMODEL.TABLE, xchain, 1);
                }
                switch (dimension) { 
                    case TLF_1D :
                        stm_modtbl_set1Dset (modelmax->UMODEL.TABLE, xydatamax, 1);
                        for (ch = xydatamax; ch; ch = ch->NEXT)
                            mbkfree(ch->DATA);
                        if (xydatamax)
                            freechain (xydatamax); 
                    break;
                }
                if(xchain){
                    for (ch = xchain; ch; ch = ch->NEXT){
                        if(ch->DATA)
                            mbkfree(ch->DATA);
                    }
                    freechain (xchain); 
                }
             } 
             |  TLF4_TOKEN_NET_CAP_MODEL '(' modelname '(' TLF4_TOKEN_LINEAR TLF4_TOKEN_VALUE '(' segment_list ')' ')' ')'
             {
                modelmax = stm_addmodel(cname, $3);
                modelmax->UTYPE = STM_MOD_MODTBL;
                modelmax->UMODEL.TABLE = stm_prop_seg2tbl ($8, STM_LOAD);
                stm_storemodel (cname, $3, modelmax, 0);
             }
             ;

netresmodel  : TLF4_TOKEN_NET_RES_MODEL '(' modelname '(' modelbody ')' ')'
             {
                modelmax = NULL;
                for (nx = 0, ch = xchain; ch; ch = ch->NEXT) nx++;
                if (xydatamax && !xydatamin) {
                    sprintf (namebuf, "%s", $3);
                    modelmax = stm_addtblmodel (cname, namebuf, nx, 0, xtype, STM_NOTYPE);
                    stm_storemodel (cname, namebuf, modelmax, 0);
                }
                if (xtype == STM_LOAD) {
                    stm_modtbl_setXrange (modelmax->UMODEL.TABLE, xchain, 1);
                }
                switch (dimension) { 
                    case TLF_1D :
                        stm_modtbl_set1Dset (modelmax->UMODEL.TABLE, xydatamax, 1);
                        for (ch = xydatamax; ch; ch = ch->NEXT)
                            mbkfree(ch->DATA);
                        if (xydatamax)
                            freechain (xydatamax); 
                    break;
                }
                if(xchain){
                    for (ch = xchain; ch; ch = ch->NEXT){
                        if(ch->DATA)
                            mbkfree(ch->DATA);
                    }
                    freechain (xchain); 
                }
             }
             | TLF4_TOKEN_NET_RES_MODEL '(' modelname '(' TLF4_TOKEN_LINEAR TLF4_TOKEN_VALUE '(' segment_list ')' ')' ')'
             {
                modelmax = stm_addmodel(cname, $3);
                modelmax->UTYPE = STM_MOD_MODTBL;
                modelmax->UMODEL.TABLE = stm_prop_seg2tbl ($8, STM_LOAD);
                stm_storemodel (cname, $3, modelmax, 0);
             
             }
             ;

model        : TLF4_TOKEN_MODEL '(' modelname templatename '(' modelbody ')' ')' 
             { 
                if (!xydatamin && !xydatamax) {
                    for (nx = 0, ch = xchain; ch; ch = ch->NEXT) nx++;
                    for (ny = 0, ch = ychain; ch; ch = ch->NEXT) ny++;
                    ttemplate = stm_modtbl_addtemplate ($3, nx, ny, xtype, ytype);
                    if (xtype == STM_INPUT_SLEW || xtype == STM_CLOCK_SLEW) {
                        if (TLF_TIME_UNIT == TLF_NS)
                            stm_modtbl_settemplateXrange (ttemplate, xchain, 1e3);
                        if (TLF_TIME_UNIT == TLF_PS)
                            stm_modtbl_settemplateXrange (ttemplate, xchain, 1);
                    }
                    if (xtype == STM_LOAD) {
                        if (TLF_CAPA_UNIT == TLF_PF)
                            stm_modtbl_settemplateXrange (ttemplate, xchain, 1e3);
                        if (TLF_CAPA_UNIT == TLF_FF)
                            stm_modtbl_settemplateXrange (ttemplate, xchain, 1);
                    }
                    if (ytype == STM_INPUT_SLEW || ytype == STM_CLOCK_SLEW) {
                        if (TLF_TIME_UNIT == TLF_NS)
                            stm_modtbl_settemplateYrange (ttemplate, ychain, 1e3);
                        if (TLF_TIME_UNIT == TLF_PS)
                            stm_modtbl_settemplateYrange (ttemplate, ychain, 1);
                    }
                    if (ytype == STM_LOAD) {
                        if (TLF_CAPA_UNIT == TLF_PF)
                            stm_modtbl_settemplateYrange (ttemplate, ychain, 1e3);
                        if (TLF_CAPA_UNIT == TLF_FF)
                            stm_modtbl_settemplateYrange (ttemplate, ychain, 1);
                    }
                } else {
                    modelmin = NULL;
                    modelmax = NULL;
                    if ($4) {
                        ttemplate = stm_modtbl_gettemplate ($4);
                        if (xydatamin) {
                            sprintf (namebuf, "%s_min", $3);
                            modelmin = stm_addmodel (cname, namebuf);
                            modelmin->UMODEL.TABLE = stm_modtbl_create_ftemplate (ttemplate);
                            modelmin->UTYPE = STM_MOD_MODTBL;
                            stm_storemodel (cname, namebuf, modelmin, 0);
                            addhtitem (doublemodels, namealloc ($3), 1);
                        }
                        if (xydatamax && xydatamin) {
                            sprintf (namebuf, "%s_max", $3);
                            modelmax = stm_addmodel (cname, namebuf);
                            modelmax->UMODEL.TABLE = stm_modtbl_create_ftemplate (ttemplate);
                            modelmax->UTYPE = STM_MOD_MODTBL;
                            stm_storemodel (cname, namebuf, modelmax, 0);
                        }
                        if (xydatamax && !xydatamin) {
                            sprintf (namebuf, "%s", $3);
                            modelmax = stm_addmodel (cname, namebuf);
                            modelmax->UMODEL.TABLE = stm_modtbl_create_ftemplate (ttemplate);
                            modelmax->UTYPE = STM_MOD_MODTBL;
                            stm_storemodel (cname, namebuf, modelmax, 0);
                        }
                    } else {
                        for (nx = 0, ch = xchain; ch; ch = ch->NEXT) nx++;
                        for (ny = 0, ch = ychain; ch; ch = ch->NEXT) ny++;
                        if (xydatamin) {
                            addhtitem (doublemodels, namealloc ($3), 1);
                            sprintf (namebuf, "%s_min", $3);
                            modelmin = stm_addtblmodel (cname, namebuf, nx, ny, xtype, ytype);
                        }
                        if (xydatamax && xydatamin) {
                            sprintf (namebuf, "%s_max", $3);
                            modelmax = stm_addtblmodel (cname, namebuf, nx, ny, xtype, ytype);
                        }
                        if (xydatamax && !xydatamin) {
                            sprintf (namebuf, "%s", $3);
                            modelmax = stm_addtblmodel (cname, namebuf, nx, ny, xtype, ytype);
                        }
                    }
                    if (xtype == STM_INPUT_SLEW || xtype == STM_CLOCK_SLEW) {
                        if (TLF_TIME_UNIT == TLF_NS) {
                            if (modelmin)
                                stm_modtbl_setXrange (modelmin->UMODEL.TABLE, xchain, 1e3);
                            stm_modtbl_setXrange (modelmax->UMODEL.TABLE, xchain, 1e3);
                        }
                        if (TLF_TIME_UNIT == TLF_PS) {
                            if (modelmin)
                                stm_modtbl_setXrange (modelmin->UMODEL.TABLE, xchain, 1);
                            stm_modtbl_setXrange (modelmax->UMODEL.TABLE, xchain, 1);
                        }
                    }
                    if (xtype == STM_LOAD) {
                        if (TLF_CAPA_UNIT == TLF_PF) {
                            if (modelmin)
                                stm_modtbl_setXrange (modelmin->UMODEL.TABLE, xchain, 1e3);
                            stm_modtbl_setXrange (modelmax->UMODEL.TABLE, xchain, 1e3);
                        }
                        if (TLF_CAPA_UNIT == TLF_FF) {
                            if (modelmin)
                                stm_modtbl_setXrange (modelmin->UMODEL.TABLE, xchain, 1);
                            stm_modtbl_setXrange (modelmax->UMODEL.TABLE, xchain, 1);
                        }
                    }
                    if (ytype == STM_INPUT_SLEW || ytype == STM_CLOCK_SLEW) {
                        if (TLF_TIME_UNIT == TLF_NS) {
                            if (modelmin)
                                stm_modtbl_setYrange (modelmin->UMODEL.TABLE, ychain, 1e3);
                            stm_modtbl_setYrange (modelmax->UMODEL.TABLE, ychain, 1e3);
                        }
                        if (TLF_TIME_UNIT == TLF_PS) {
                            if (modelmin)
                                stm_modtbl_setYrange (modelmin->UMODEL.TABLE, ychain, 1);
                            stm_modtbl_setYrange (modelmax->UMODEL.TABLE, ychain, 1);
                        }
                    }
                    if (ytype == STM_LOAD) {
                        if (TLF_CAPA_UNIT == TLF_PF) {
                            if (modelmin)
                                stm_modtbl_setYrange (modelmin->UMODEL.TABLE, ychain, 1e3);
                            stm_modtbl_setYrange (modelmax->UMODEL.TABLE, ychain, 1e3);
                        }
                        if (TLF_CAPA_UNIT == TLF_FF) {
                            if (modelmin)
                                stm_modtbl_setYrange (modelmin->UMODEL.TABLE, ychain, 1);
                            stm_modtbl_setYrange (modelmax->UMODEL.TABLE, ychain, 1);
                        }
                    }
                    switch (dimension) { 
                        case TLF_CONST :
                            if (nx > 0 || ny > 0)
                                avt_errmsg (STM_ERRMSG, "000", AVT_ERROR, $3);
                            else {
                                if (TLF_TIME_UNIT == TLF_NS) {
                                    if (xydatamin) 
                                        stm_modtbl_setconst (modelmin->UMODEL.TABLE, *(float*)xydatamin->DATA * 1e3);
                                    if (xydatamax) 
                                        stm_modtbl_setconst (modelmax->UMODEL.TABLE, *(float*)xydatamax->DATA * 1e3);
                                }
                                if (TLF_TIME_UNIT == TLF_PS) {
                                    if (xydatamin) 
                                        stm_modtbl_setconst (modelmin->UMODEL.TABLE, *(float*)xydatamin->DATA);
                                    if (xydatamax) 
                                        stm_modtbl_setconst (modelmax->UMODEL.TABLE, *(float*)xydatamax->DATA);
                                }
                                if(xydatamin){
                                    if (xydatamin->DATA) {
                                        mbkfree(xydatamin->DATA);
                                    }
                                    freechain (xydatamin);
                                }
                                if(xydatamax){
                                    if (xydatamax->DATA) {
                                        mbkfree(xydatamax->DATA);
                                    }
                                    freechain (xydatamax);
                                }
                            }
                        break;
                        case TLF_1D :
                            if (nx > 0 && ny > 0)
                                avt_errmsg (STM_ERRMSG, "000", AVT_ERROR, $3);
                            else {
                                if (TLF_TIME_UNIT == TLF_NS) {
                                    if (modelmin)
                                        stm_modtbl_set1Dset (modelmin->UMODEL.TABLE, xydatamin, 1e3);
                                    stm_modtbl_set1Dset (modelmax->UMODEL.TABLE, xydatamax, 1e3);
                                }
                                if (TLF_TIME_UNIT == TLF_PS) {
                                    if (modelmin)
                                        stm_modtbl_set1Dset (modelmin->UMODEL.TABLE, xydatamin, 1);
                                    stm_modtbl_set1Dset (modelmax->UMODEL.TABLE, xydatamax, 1);
                                }
                                
                                for (ch = xydatamin; ch; ch = ch->NEXT)
                                    mbkfree(ch->DATA);
                                if (xydatamin)
                                    freechain (xydatamin); 

                                for (ch = xydatamax; ch; ch = ch->NEXT)
                                    mbkfree(ch->DATA);
                                if (xydatamax)
                                    freechain (xydatamax); 
                            }
                        break;
                        case TLF_2D :
                            if (nx == 0 || ny == 0)
                                avt_errmsg (STM_ERRMSG, "000", AVT_ERROR, $3);
                            else {
                                if (TLF_TIME_UNIT == TLF_NS) {
                                    if (modelmin)
                                        stm_modtbl_set2Dset (modelmin->UMODEL.TABLE, xydatamin, 1e3);
                                    stm_modtbl_set2Dset (modelmax->UMODEL.TABLE, xydatamax, 1e3);
                                }
                                if (TLF_TIME_UNIT == TLF_PS) {
                                    if (modelmin)
                                        stm_modtbl_set2Dset (modelmin->UMODEL.TABLE, xydatamin, 1);
                                    stm_modtbl_set2Dset (modelmax->UMODEL.TABLE, xydatamax, 1);
                                }
                                for (ch = xydatamin; ch; ch = ch->NEXT) {
                                    for (chp = (chain_list*)ch->DATA; chp; chp = chp->NEXT)
                                        mbkfree(chp->DATA);
                                    freechain (ch->DATA);
                                }
                                if (xydatamin)
                                    freechain (xydatamin); 
                                for (ch = xydatamax; ch; ch = ch->NEXT) {
                                    for (chp = (chain_list*)ch->DATA; chp; chp = chp->NEXT)
                                        mbkfree(chp->DATA);
                                    freechain (ch->DATA);
                                }
                                if (xydatamax)
                                    freechain (xydatamax); 
                            }
                        break;
                    }
                }
                if(xchain){
                    for (ch = xchain; ch; ch = ch->NEXT){
                        if(ch->DATA)
                            mbkfree(ch->DATA);
                    }
                    freechain (xchain); 
                }
                if(ychain){
                    for (ch = ychain; ch; ch = ch->NEXT){
                        if(ch->DATA)
                            mbkfree(ch->DATA);
                    }
                    freechain (ychain); 
                }
             } 
             ;
             
modelbody    : modeltype axe axe axe data 
             { 
                xchain    = $2.CHAIN;
                xtype     = $2.TYPE;
                ychain    = $3.CHAIN;
                ytype     = $3.TYPE;
                xydatamax = NULL;
                for (ch = $5.CHAIN; ch; ch = ch->NEXT) {
                    ydatamax = NULL;
                    for (chp = (chain_list*)ch->DATA; chp; chp = chp->NEXT) {
                        ptfl = (float*)mbkalloc (sizeof (float));
                        *ptfl = ((minmax*)chp->DATA)->MAX;
                        ydatamax = addchain (ydatamax, ptfl);
                    }
                    ydatamax = reverse (ydatamax);
                    xydatamax = addchain (xydatamax, ydatamax);
                }
                if (xydatamax)
                    xydatamax = reverse (xydatamax);
                
                xydatamin = NULL;
                if ($5.CHAIN) {
                    if (((minmax*)(((chain_list*)($5.CHAIN->DATA))->DATA))->MIN != STM_NOVALUE) {
                        for (ch = $5.CHAIN; ch; ch = ch->NEXT) {
                            ydatamin = NULL;
                            for (chp = (chain_list*)ch->DATA; chp; chp = chp->NEXT) {
                                ptfl = (float*)mbkalloc (sizeof (float));
                                *ptfl = ((minmax*)chp->DATA)->MIN;
                                ydatamin = addchain (ydatamin, ptfl);
                            }
                            ydatamin = reverse (ydatamin);
                            xydatamin = addchain (xydatamin, ydatamin);
                        }
                        xydatamin = reverse (xydatamin);
                    }
                    for (ch = $5.CHAIN; ch; ch = ch->NEXT) {
                        for (chp = (chain_list*)ch->DATA; chp; chp = chp->NEXT)
                            mbkfree ((minmax*)chp->DATA);
                        freechain ((chain_list*)ch->DATA);
                    }
                    //freechain ($5.CHAIN);
                }
                dimension = $5.DIM;
             } 
             | modeltype axe axe data 
             { 
                xchain    = $2.CHAIN;
                xtype     = $2.TYPE;
                ychain    = $3.CHAIN;
                ytype     = $3.TYPE;
                xydatamax = NULL;
                for (ch = $4.CHAIN; ch; ch = ch->NEXT) {
                    ydatamax = NULL;
                    for (chp = (chain_list*)ch->DATA; chp; chp = chp->NEXT) {
                        ptfl = (float*)mbkalloc (sizeof (float));
                        *ptfl = ((minmax*)chp->DATA)->MAX;
                        ydatamax = addchain (ydatamax, ptfl);
                    }
                    ydatamax = reverse (ydatamax);
                    xydatamax = addchain (xydatamax, ydatamax);
                }
                if (xydatamax)
                    xydatamax = reverse (xydatamax);
                
                xydatamin = NULL;
                if ($4.CHAIN) {
                    if (((minmax*)(((chain_list*)($4.CHAIN->DATA))->DATA))->MIN != STM_NOVALUE) {
                        for (ch = $4.CHAIN; ch; ch = ch->NEXT) {
                            ydatamin = NULL;
                            for (chp = (chain_list*)ch->DATA; chp; chp = chp->NEXT) {
                                ptfl = (float*)mbkalloc (sizeof (float));
                                *ptfl = ((minmax*)chp->DATA)->MIN;
                                ydatamin = addchain (ydatamin, ptfl);
                            }
                            ydatamin = reverse (ydatamin);
                            xydatamin = addchain (xydatamin, ydatamin);
                        }
                        xydatamin = reverse (xydatamin);
                    }
                    for (ch = $4.CHAIN; ch; ch = ch->NEXT) {
                        for (chp = (chain_list*)ch->DATA; chp; chp = chp->NEXT)
                            mbkfree ((minmax*)chp->DATA);
                        freechain ((chain_list*)ch->DATA);
                    }
                    //freechain ($4.CHAIN);
                }
                dimension = $4.DIM;
             } 
             | modeltype axe data 
             { 
                xchain    = $2.CHAIN;
                xtype     = $2.TYPE;
                ychain    = NULL;
                ytype     = STM_NOTYPE;
                xydatamax = NULL;
                for (ch = $3.CHAIN; ch; ch = ch->NEXT) {
                    ptfl = (float*)mbkalloc (sizeof (float));
                    *ptfl = ((minmax*)ch->DATA)->MAX;
                    xydatamax = addchain (xydatamax, ptfl);
                }
                if (xydatamax)
                    xydatamax = reverse (xydatamax);
                xydatamin = NULL;
                if ($3.CHAIN) {
                    if (((minmax*)$3.CHAIN->DATA)->MIN != STM_NOVALUE) {
                        for (ch = $3.CHAIN; ch; ch = ch->NEXT) {
                            ptfl = (float*)mbkalloc (sizeof (float));
                            *ptfl = ((minmax*)ch->DATA)->MIN;
                            xydatamin = addchain (xydatamin, ptfl);
                        }
                        xydatamin = reverse (xydatamin);
                    }
                    for (ch = $3.CHAIN; ch; ch = ch->NEXT) 
                        mbkfree ((minmax*)ch->DATA);
                }
                dimension = $3.DIM;
             }
             | modeltype data 
             { 
                xchain    = NULL;
                xtype     = STM_NOTYPE;
                ychain    = NULL;
                ytype     = STM_NOTYPE;
                xydatamax = NULL;
                xydatamin = NULL;
                dimension = $2.DIM;
                switch(dimension){
                    case TLF_1D:
                    case TLF_CONST:
                        xydatamax = NULL;
                        for (ch = $2.CHAIN; ch; ch = ch->NEXT) {
                            ptfl = (float*)mbkalloc (sizeof (float));
                            *ptfl = ((minmax*)ch->DATA)->MAX;
                            xydatamax = addchain (xydatamax, ptfl);
                        }
                        if (xydatamax)
                            xydatamax = reverse (xydatamax);
                        xydatamin = NULL;
                        if ($2.CHAIN) {
                            if (((minmax*)$2.CHAIN->DATA)->MIN != STM_NOVALUE) {
                                for (ch = $2.CHAIN; ch; ch = ch->NEXT) {
                                    ptfl = (float*)mbkalloc (sizeof (float));
                                    *ptfl = ((minmax*)ch->DATA)->MIN;
                                    xydatamin = addchain (xydatamin, ptfl);
                                }
                                xydatamin = reverse (xydatamin);
                            }
                            for (ch = $2.CHAIN; ch; ch = ch->NEXT) 
                                mbkfree ((minmax*)ch->DATA);
                        }
                        break;
                    case TLF_2D:
                        xydatamax = NULL;
                        for (ch = $2.CHAIN; ch; ch = ch->NEXT) {
                            ydatamax = NULL;
                            for (chp = (chain_list*)ch->DATA; chp; chp = chp->NEXT) {
                                ptfl = (float*)mbkalloc (sizeof (float));
                                *ptfl = ((minmax*)chp->DATA)->MAX;
                                ydatamax = addchain (ydatamax, ptfl);
                            }
                            ydatamax = reverse (ydatamax);
                            xydatamax = addchain (xydatamax, ydatamax);
                        }
                        if (xydatamax)
                            xydatamax = reverse (xydatamax);
                        
                        xydatamin = NULL;
                        if ($2.CHAIN) {
                            if (((minmax*)(((chain_list*)($2.CHAIN->DATA))->DATA))->MIN != STM_NOVALUE) {
                                for (ch = $2.CHAIN; ch; ch = ch->NEXT) {
                                    ydatamin = NULL;
                                    for (chp = (chain_list*)ch->DATA; chp; chp = chp->NEXT) {
                                        ptfl = (float*)mbkalloc (sizeof (float));
                                        *ptfl = ((minmax*)chp->DATA)->MIN;
                                        ydatamin = addchain (ydatamin, ptfl);
                                    }
                                    ydatamin = reverse (ydatamin);
                                    xydatamin = addchain (xydatamin, ydatamin);
                                }
                                xydatamin = reverse (xydatamin);
                            }
                            for (ch = $2.CHAIN; ch; ch = ch->NEXT) {
                                for (chp = (chain_list*)ch->DATA; chp; chp = chp->NEXT)
                                    mbkfree ((minmax*)chp->DATA);
                                freechain ((chain_list*)ch->DATA);
                            }
                            //freechain ($2.CHAIN);
                        }
                        break;
                }
             } 
             ;

segment_list : empty        
             { 
                $$ = NULL; 
             }
             | segment segment_list  
             { 
                ptseg = $1;
                $$ = addchain ($2, ptseg);
             } 
             ;

segment      : val ':' val ':' val ':' val
             {
                ptseg = (segment*)mbkalloc (sizeof (struct segment));
                ptseg->X0 = $1;
                ptseg->X1 = $3;
                ptseg->INTER = $5;
                ptseg->SLO = $7;
                $$ = ptseg;
             }
             ;
             
modeltype    : TLF4_TOKEN_SPLINE 
             | TLF4_TOKEN_CONST  
             ;

axe          : '(' axetype val val_list ')' 
             { 
                ptfl = (float*)mbkalloc (sizeof (float));
                *ptfl = $3;
                $$.CHAIN = addchain ($4, ptfl);
                $$.TYPE = $2;
             } 
             ;


axetype      : TLF4_TOKEN_INPUT_SLEW_AXIS 
             { 
                $$ = STM_INPUT_SLEW;
             }
             | TLF4_TOKEN_CLOCK_SLEW_AXIS       
             { 
                $$ = STM_CLOCK_SLEW;
             }
             | TLF4_TOKEN_SLEW_AXIS       
             { 
                $$ = STM_INPUT_SLEW;
             }
             | TLF4_TOKEN_LOAD_AXIS       
             { 
                $$ = STM_LOAD;
             } 
             | TLF4_TOKEN_LOAD2_AXIS       
             { 
                $$ = STM_LOAD;
             } 
             ;

cval_list    : empty        
             { 
                $$ = NULL; 
             }
             | cval cval_list  
             { 
                ptmm = (minmax*)mbkalloc (sizeof (struct minmax));
                ptmm->MIN = $1.MIN;
                ptmm->MAX = $1.MAX;
                $$ = addchain ($2, ptmm);
             } 
             ;

val_list     : empty        
             { 
                $$ = NULL; 
             }
             | val val_list  
             { 
                ptfl = (float*)mbkalloc (sizeof (float));
                *ptfl = $1;
                $$ = addchain ($2, ptfl);
             } 
             ;

data         : TLF4_TOKEN_DATA '(' modeldata ')' 
             { 
                $$ = $3;
             } 
             | '(' modeldata ')'       
             { 
                $$ = $2;
             } 
             | TLF4_TOKEN_DATA modeldata 
             { 
                $$ = $2;
             } 
             | modeldata       
             { 
                $$ = $1;
             } 
             ;
   
modeldata    : cval     
             { 
                ptmm = (minmax*)mbkalloc (sizeof (struct minmax));
                ptmm->MIN = $1.MIN;
                ptmm->MAX = $1.MAX;
                $$.CHAIN = addchain (NULL, ptmm); 
                $$.DIM = TLF_CONST;
             } 
             | array1D 
             { 
                $$.CHAIN = $1;
                $$.DIM = TLF_1D;
             }
             | array2D 
             { 
                $$.CHAIN = $1;
                $$.DIM = TLF_2D;
             } 
             | empty
             {
                $$.CHAIN = NULL;
                $$.DIM   = TLF_NODIM;
             }
             
             ;

array2D      : array1D array1D array1D_list 
             { 
                $$ = addchain ($3, $2); 
                $$ = addchain ($$, $1); 
             } 
             ;

array1D      : '(' cval cval cval_list ')' 
             { 
                ptmm = (minmax*)mbkalloc (sizeof (struct minmax));
                ptmm->MIN = $3.MIN;
                ptmm->MAX = $3.MAX;
                $$ = addchain ($4, ptmm);
                ptmm = (minmax*)mbkalloc (sizeof (struct minmax));
                ptmm->MIN = $2.MIN;
                ptmm->MAX = $2.MAX;
                $$ = addchain ($$, ptmm);
             } 
             ;

array1D_list : empty 
             { 
                $$ = NULL;
             }
             | array1D array1D_list 
             { 
                $$ = addchain ($2, $1);
             } 
             ;

pin          : TLF4_TOKEN_PIN '(' pinname 
             {
                pname = $3;
             }
               pinprop_list ')'
             {
                int i=0;
                locon_list *c;
                losig_list *s;

                ptfl = (float*)mbkalloc (sizeof (float));
                if (pincapa == TLF_NO_CAPA) {
                    if (buscapa != TLF_NO_CAPA)
                        *ptfl = buscapa;
                    else{
                        switch (pindir) {
                            case 'I' :
                                *ptfl = defincapa; 
                                break;
                            case 'O' :
                                *ptfl = defoutcapa;
                                break;
                            case 'B' :
                                *ptfl = defbidircapa;
                                break;
                            default :
                                *ptfl = 0.0;
                        }
                    }
                    addhtitem (pincapht, pname, (long)ptfl);
                } else {
                    *ptfl = pincapa;
                    addhtitem (pincapht, pname, (long)ptfl);
                }
                switch (pindir) {
                    case 'I' :
                        dir = TTV_SIG_CI;
                        break;
                    case 'O' :
                        if(ptrcbhcomb){
                            if((ptrcbhcomb->HZFUNC) != NULL) {
                                pindir = 'Z';
                                dir = TTV_SIG_CZ ;
                            }
                            else
                                dir = TTV_SIG_CO;
                        }else
                            dir = TTV_SIG_CO;
                        break;
                    case 'B' :
                        if(ptrcbhcomb){
                            if((ptrcbhcomb->HZFUNC) != NULL) {
                                pindir = 'T';
                                dir = TTV_SIG_CT ;
                            }
                            else
                                dir = TTV_SIG_CB;
                        }else
                            dir = TTV_SIG_CB;
                        break;
                    default :
                        dir = TTV_SIG_CX;
                }
                if((pindir != 'A') && !isglobalvdd(pname) && !isglobalvss(pname)){
                    consigchain = ttv_addrefsig (fig, pname, pname, *ptfl, dir, consigchain);
                    nbconsig++;
                    if (clockpin)
                        clocks = addchain (clocks, pname);
                }
                if((pindir != 'X') && (!flagbus)){
                   /* nouveau connecteur --> LOCON et LOSIG a creer */
                   for(c=ptrlofig->LOCON; ((c != NULL) && (c->NAME != ttv_devect(pname))); c=c->NEXT);
                   if(c == NULL) { /* connecteur inexistant, on ajoute signal et connecteur */
                        for(s=ptrlofig->LOSIG; (s != NULL); s=s->NEXT) {/* recherche du 1er index de libre */
                            if (i < s->INDEX)
                                i = s->INDEX;
                        }
                        losig_index = i + 1;
                        ptrlosig = addlosig(ptrlofig, losig_index, addchain(NULL, ttv_devect(pname)), 'E');
//                        if(!flagbus)
                            ptrlocon = addlocon(ptrlofig, ttv_devect(pname), ptrlosig, pindir);
//                        else{
//                            ptloconbus = (loconbus*)mbkalloc (sizeof (struct loconbus));
//                            ptloconbus->NAME = ttv_devect(pname);
//                            ptloconbus->LOSIG = ptrlosig;
//                            ptloconbus->DIREC = pindir;
//                            buschain = addchain(buschain, ptloconbus);
//                        }
                    }
                    else { /* connecteur et signal existants, on redirige le connecteur */
                        ptrlocon = c;
                        ptrlocon->DIRECTION = pindir;
                    }
                
                    if(/*!flagbus && */ptrcbhcomb)
                        cbh_addcombtolocon(ptrlocon, ptrcbhcomb);
                }
//                if(!flagbus){
                    ptrcbhcomb = NULL;
                    ptrlosig = NULL;
                    ptrlocon = NULL;
//                }
                pincapa = TLF_NO_CAPA;
                pindir = 'X';
                clockpin = 0;
             }
             | TLF4_TOKEN_BUS '(' busname bustype busprops {/* buschain = NULL;*/ flagbus = 1;} pin_list ')'
             {
                locon_list *c;
                losig_list *s;
                int n = 0;
                char buf[1024];
                
                if(l > m)
                  for(i = l; i >= m; i--){
                   sprintf(buf, "%s %d", bname, i);
                   for(c=ptrlofig->LOCON; ((c != NULL) && (c->NAME != namealloc(buf))); c=c->NEXT);
                   if(c == NULL) { /* connecteur inexistant, on ajoute signal et connecteur */
                        for(s=ptrlofig->LOSIG; (s != NULL); s=s->NEXT) {/* recherche du 1er index de libre */
                            if (n < s->INDEX)
                                n = s->INDEX;
                        }
                        losig_index = n + 1;
                        ptrlosig = addlosig(ptrlofig, losig_index, addchain(NULL, namealloc(buf)), 'E');
                        ptrlocon = addlocon(ptrlofig, namealloc(buf), ptrlosig, pindir);
                   }
                  }
                else if(l < m)
                  for(i = l; i <= m; i++){
                   sprintf(buf, "%s %d", bname, i);
                   for(c=ptrlofig->LOCON; ((c != NULL) && (c->NAME != namealloc(buf))); c=c->NEXT);
                   if(c == NULL) { /* connecteur inexistant, on ajoute signal et connecteur */
                        for(s=ptrlofig->LOSIG; (s != NULL); s=s->NEXT) {/* recherche du 1er index de libre */
                            if (n < s->INDEX)
                                n = s->INDEX;
                        }
                        losig_index = n + 1;
                        ptrlosig = addlosig(ptrlofig, losig_index, addchain(NULL, namealloc(buf)), 'E');
                        ptrlocon = addlocon(ptrlofig, namealloc(buf), ptrlosig, pindir);
                   }
                  }
                 flagbus = 0;
//                if(buschain)
//                    if(l != vectorindex(((loconbus*)buschain->DATA)->NAME))
//                        reverse(buschain);
//                for(chain=buschain; chain; chain=chain->NEXT){
//                    ptloconbus = (loconbus*)chain->DATA;
//                    ptrlocon = addlocon(ptrlofig, ptloconbus->NAME, ptloconbus->LOSIG, ptloconbus->DIREC);
//                    cbh_addcombtolocon(ptrlocon, ptrcbhcomb);
//                    mbkfree(chain->DATA) ;
//                }
//                if(buschain)
//                    freechain(buschain);
                for (i = lsb; i <= msb; i++) {
                    pt = stm_vect (bname, i);
                    vectname = namealloc (pt);
                    mbkfree(pt);
                    if(i == msb)
                        mbkfree(bname);
                    for(ch = consigchain; ch; ch = ch->NEXT)
                        if(!strcmp(vectname, ((ttvsig_list*)ch->DATA)->NAME)){
                            break;
                        }
                    if(ch)
                        continue;
                    ptfl = (float*)mbkalloc (sizeof (float));
                    if (buscapa != TLF_NO_CAPA)
                        *ptfl = buscapa;
                    else
                    if (caparray[i - lsb] != TLF_NO_CAPA)
                        *ptfl = caparray[i - lsb];
                    else
                        switch ($4) {
                            case 'I' :
                                *ptfl = defincapa; 
                                break;
                            case 'O' :
                                *ptfl = defoutcapa;
                                break;
                            case 'B' :
                                *ptfl = defbidircapa;
                                break;
                            default :
                                *ptfl = 0.0;
                        }
                    addhtitem (pincapht, vectname, (long)ptfl);
                    switch ($4) {
                        case 'I' :
                            dir = TTV_SIG_CI;
                            break;
                        case 'O' :
                            dir = TTV_SIG_CO;
                            break;
                        case 'B' :
                            dir = TTV_SIG_CB;
                            break;
                        default :
                            dir = TTV_SIG_CX;
                    }
                    consigchain = ttv_addrefsig (fig, vectname, vectname, *ptfl, dir, consigchain);
                    nbconsig++;
                    if (clockpin)
                        clocks = addchain (clocks, vectname);
                }
                mbkfree(caparray);
                buscapa = TLF_NO_CAPA;
                pindir = 'X';
                clockpin = 0;
             }
             | TLF4_TOKEN_PIN '(' busname pintype busprops ')'
             {
                for (i = lsb; i <= msb; i++) {
                    pt = stm_vect (bname, i);
                    vectname = namealloc (pt);
                    mbkfree(pt);
                    if(i == msb)
                        mbkfree(bname);
                    ptfl = (float*)mbkalloc (sizeof (float));
                    
                    if (pincapa == TLF_NO_CAPA) {
                        if (buscapa != TLF_NO_CAPA)
                            *ptfl = buscapa;
                        else{
                            switch (pindir) {
                                case 'I' :
                                    *ptfl = defincapa; 
                                    break;
                                case 'O' :
                                    *ptfl = defoutcapa;
                                    break;
                                case 'B' :
                                    *ptfl = defbidircapa;
                                    break;
                                default :
                                    *ptfl = 0.0;
                            }
                        }
                        addhtitem (pincapht, vectname, (long)ptfl);
                    } else {
                        *ptfl = caparray[i - lsb];
                        addhtitem (pincapht, vectname, (long)ptfl);
                    }
                    switch (pindir) {
                        case 'I' :
                            dir = TTV_SIG_CI;
                            break;
                        case 'O' :
                            dir = TTV_SIG_CO;
                            break;
                        case 'B' :
                            dir = TTV_SIG_CB;
                            break;
                        default :
                            dir = TTV_SIG_CX;
                    }
                    consigchain = ttv_addrefsig (fig, vectname, vectname, *ptfl, dir, consigchain);
                    nbconsig++;
                    if (clockpin)
                        clocks = addchain (clocks, vectname);
                }
                mbkfree(caparray);
                pindir = 'X';
                clockpin = 0;
             }
             ;

bustype      : pintype
             {
                $$ = $1;
             }
             ;

pintype      : TLF4_TOKEN_PINTYPE '(' TLF4_TOKEN_BIDIR ')'
             {
                $$ = 'B';
             }
             | TLF4_TOKEN_PINTYPE '(' TLF4_TOKEN_INPUT ')'
             {
                $$ = 'I';
             }
             | TLF4_TOKEN_PINTYPE '(' TLF4_TOKEN_SUPPLY ')'
             {
                $$ = 'A';
             }
             | TLF4_TOKEN_PINTYPE '(' TLF4_TOKEN_GROUND ')'
             {
                $$ = 'A';
             }
             | TLF4_TOKEN_PINTYPE '(' TLF4_TOKEN_INTERNAL ')'
             {
                $$ = 'X';
             }
             | TLF4_TOKEN_PINTYPE '(' TLF4_TOKEN_OUTPUT ')'
             {
                $$ = 'O';
             }
             ;

pinprop_list : empty
             | pinprop_list pinprop
             ;

pinprop      : pincap
             {
                pincapa = $1;
             }
             | energy
             {
             }
             | vdrop_limit
             {
             }
             | pintype
             { 
                pindir = $1;
                if((pindir == 'O') || (pindir == 'B')){
                    ptrcbhcomb = cbh_newcomb();
                    ptrcbhcomb->NAME = namealloc(pname);
                }
             }
             | TLF4_TOKEN_CLOCK_PIN
             {
                clockpin = 1;
             }
             | pinfunc
             {
             }
             | pinenable
             {
             }
             | attribute
             {
             }
             | directive
             {
             }
             ;

pinenable    : TLF4_TOKEN_ENABLE TLF4_TOKEN_EXPR
             {
                char    *str;
                if(ptrcbhcomb != NULL) {
                    str = eqt_ConvertStr($2);
                    ptrcbhcomb->HZFUNC = notExpr(eqt_StrToAbl(tlfEqtCtx,str));
                    mbkfree (str);
                }
             }
             ;

pinfunc      : TLF4_TOKEN_FUNCTION  TLF4_TOKEN_EXPR 
             {
                char    *str;
                if(ptrcbhcomb != NULL) {
                    str = eqt_ConvertStr($2);
                    ptrcbhcomb->FUNCTION = eqt_StrToAbl(tlfEqtCtx,str);
                    mbkfree (str);
                }
             } 
             ;


rise         : TLF4_TOKEN_RISE '(' modelname ')'
             | TLF4_TOKEN_RISE '(' modelbody ')'
             |
             ;

fall         : TLF4_TOKEN_FALL '(' modelname ')'
             | TLF4_TOKEN_FALL '(' modelbody ')'
             | empty
             ;

busprops     : empty
             | forbits_list 
             {
             }
             | pincap
             {
                buscapa = $1;
             }
             | TLF4_TOKEN_CLOCK_PIN
             {
                clockpin = 1;
             }
             ;

pin_list     : empty
             | pin_list pin
             ;

forbits_list : forbits
             | forbits_list forbits
             ;

forbits      : TLF4_TOKEN_FOR_BITS '(' TLF4_TOKEN_VECT pincap ')'
             {
                v = atoi (stm_unquote ($3));
                mbkfree($3);
                if (v <= msb && v >= lsb) 
                    caparray[v - lsb] = $4;
             }
             ;

pincap       : TLF4_TOKEN_CAPACITANCE '(' val ')'
             {
                if (TLF_CAPA_UNIT == TLF_PF)
                    $$ = $3 * 1e3;
                if (TLF_CAPA_UNIT == TLF_FF)
                    $$ = $3;
             }
             ;

arc_list     : empty
             | arc_list arc
             ;

arc          : path 
             | setup
             | hold
             ;

speed        : TLF4_TOKEN_FAST
             | TLF4_TOKEN_SLOW
             | empty
             ;

path         : TLF4_TOKEN_PATH '(' pinname "=>" pinname speed transition transition delay slew energy_list ')'
             {
                if ($7 != 'X' && $8 != 'X') {
                    pinn = $3;
                    pinr = $5;
                    mindlymname = $9.MIN ? $9.MIN : $9.MAX;
                    minslwmname = $10.MIN ? $10.MIN : $10.MAX;
                    maxdlymname = $9.MAX;
                    maxslwmname = $10.MAX;
                    signode = (ttvsig_list*)gethtitem (pinht, pinn);
                    sigroot = (ttvsig_list*)gethtitem (pinht, pinr);
                    if ((long)sigroot != EMPTYHT && (long)signode != EMPTYHT) {
                        if (isclockpin (clocks, pinn))
                            linetype = TTV_LINE_A;
                        else
                            linetype = 0;
                        if ($7 == 'U') {
                            node = &signode->NODE[1];
                            itype = elpRISE;
                        }
                        else if ($7 == 'D') {
                            node = &signode->NODE[0];
                            itype = elpFALL;
                        }
                        if ($8 == 'U') {
                            root = &sigroot->NODE[1];
                            otype = elpRISE;
                        }
                        else if ($8 == 'D') {
                            root = &sigroot->NODE[0];
                            otype = elpFALL;
                        }
                        else if ($8 == 'u') {
                            root = &sigroot->NODE[1];
                            otype = elpRISE;
                            linetype |= TTV_LINE_HZ;
                        }
                        else if ($8 == 'd') {
                            root = &sigroot->NODE[0];
                            otype = elpFALL;
                            linetype |= TTV_LINE_HZ;
                        }

                        delaymodelmax = stm_getmodel (cname, maxdlymname);
                        if(gethtitem(modshrinkht, maxdlymname) == EMPTYHT){
                            if (itype == elpRISE)
                               stm_mod_shrinkslewaxis (delaymodelmax, risethmin, risethmax, itype);
                            else
                               stm_mod_shrinkslewaxis (delaymodelmax, fallthmin, fallthmax, itype);
                            addhtitem (modshrinkht, maxdlymname, (long)delaymodelmax);
                        }


                        slewmodelmax  = stm_getmodel (cname, maxslwmname);
                        if(gethtitem(modshrinkht, maxslwmname) == EMPTYHT){
                            if (itype == elpRISE)
                               stm_mod_shrinkslewaxis (slewmodelmax, risethmin, risethmax, itype);
                            else
                               stm_mod_shrinkslewaxis (slewmodelmax, fallthmin, fallthmax, itype);
                            if (otype == elpRISE)
                               stm_mod_shrinkslewdata (slewmodelmax, risethmin, risethmax, otype);
                            else   
                               stm_mod_shrinkslewdata (slewmodelmax, fallthmin, fallthmax, otype);
                            addhtitem (modshrinkht, maxslwmname, (long)slewmodelmax);
                        }

                        delaymodelmin = stm_getmodel (cname, mindlymname);
                        if(gethtitem(modshrinkht, mindlymname) == EMPTYHT){
                            if (itype == elpRISE)
                               stm_mod_shrinkslewaxis (delaymodelmin, risethmin, risethmax, itype);
                            else
                               stm_mod_shrinkslewaxis (delaymodelmin, fallthmin, fallthmax, itype);
                            addhtitem (modshrinkht, mindlymname, (long)delaymodelmin);
                        }
                        
                        slewmodelmin  = stm_getmodel (cname, minslwmname);
                        if(gethtitem(modshrinkht, minslwmname) == EMPTYHT){
                            if (itype == elpRISE)
                               stm_mod_shrinkslewaxis (slewmodelmin, risethmin, risethmax, itype);
                            else
                               stm_mod_shrinkslewaxis (slewmodelmin, fallthmin, fallthmax, itype);
                            if (otype == elpRISE)
                               stm_mod_shrinkslewdata (slewmodelmin, risethmin, risethmax, otype);
                            else
                               stm_mod_shrinkslewdata (slewmodelmin, fallthmin, fallthmax, otype);
                            addhtitem (modshrinkht, minslwmname, (long)slewmodelmin);
                        }

                        if ($8 == 'D') {
                            stm_mod_update(delaymodelmax, voltage/2.0, voltage, voltage/5.0, 0.0);
                            stm_mod_update(slewmodelmax, voltage/2.0, voltage, voltage/5.0, 0.0);
                            stm_mod_update(delaymodelmin, voltage/2.0, voltage, voltage/5.0, 0.0);
                            stm_mod_update(slewmodelmin, voltage/2.0, voltage, voltage/5.0, 0.0);
                        }else{
                            stm_mod_update(delaymodelmax, voltage/2.0, voltage, voltage/5.0, voltage);
                            stm_mod_update(slewmodelmax, voltage/2.0, voltage, voltage/5.0, voltage);
                            stm_mod_update(delaymodelmin, voltage/2.0, voltage, voltage/5.0, voltage);
                            stm_mod_update(slewmodelmin, voltage/2.0, voltage, voltage/5.0, voltage);
                        }

                        capa = gethtitem (pincapht, pinr);
                        if (capa != EMPTYHT) {
                            slwmin = stm_mod_slew (slewmodelmin, *(float*)capa + STM_DEF_LOAD, STM_DEF_SLEW, NULL, NULL, "unknown");
                            dlymin = stm_mod_delay (delaymodelmin, *(float*)capa + STM_DEF_LOAD, STM_DEF_SLEW, NULL, "unknown");
                            slwmax = stm_mod_slew (slewmodelmax, *(float*)capa + STM_DEF_LOAD, STM_DEF_SLEW, NULL, NULL, "unknown");
                            dlymax = stm_mod_delay (delaymodelmax, *(float*)capa + STM_DEF_LOAD, STM_DEF_SLEW, NULL, "unknown");
                        } else {
                            slwmin = stm_mod_slew (slewmodelmin, STM_DEF_LOAD, STM_DEF_SLEW, NULL, NULL, "unknown");
                            dlymin = stm_mod_delay (delaymodelmin, STM_DEF_LOAD, STM_DEF_SLEW, NULL, "unknown");
                            slwmax = stm_mod_slew (slewmodelmax, STM_DEF_LOAD, STM_DEF_SLEW, NULL, NULL, "unknown");
                            dlymax = stm_mod_delay (delaymodelmax, STM_DEF_LOAD, STM_DEF_SLEW, NULL, "unknown");
                        }
                        line = ttv_addline (fig, root, node, dlymax*TTV_UNIT, slwmax*TTV_UNIT, dlymin*TTV_UNIT, slwmin*TTV_UNIT, TTV_LINE_T | linetype);
                        ttv_addcaracline (line, maxdlymname, 
                                                                                              mindlymname, 
                                                                                              maxslwmname, 
                                                                                              minslwmname);
                        line = ttv_addline (fig, root, node, dlymax*TTV_UNIT, slwmax*TTV_UNIT, dlymin*TTV_UNIT, slwmin*TTV_UNIT, TTV_LINE_D | linetype);
                        ttv_addcaracline (line, maxdlymname, 
                                                                                              mindlymname, 
                                                                                              maxslwmname, 
                                                                                              minslwmname);
                    }   
                }
             }
             | TLF4_TOKEN_PATH '(' pinname "*>" busname speed transition transition delay slew energy_list ')'
             {
                if ($7 != 'X' && $8 != 'X') {
                    for(i = lsb; i <= msb; i++){
                        pinn = $3;
                        pt = stm_vect (bname, i);
                        pinr = namealloc (pt);
                        mbkfree(pt);
                        if(i == msb)
                            mbkfree(bname);
                        mindlymname = $9.MIN ? $9.MIN : $9.MAX;
                        minslwmname = $10.MIN ? $10.MIN : $10.MAX;
                        maxdlymname = $9.MAX;
                        maxslwmname = $10.MAX;
                        signode = (ttvsig_list*)gethtitem (pinht, pinn);
                        sigroot = (ttvsig_list*)gethtitem (pinht, pinr);
                        if ((long)sigroot != EMPTYHT && (long)signode != EMPTYHT) {
                            if (isclockpin (clocks, pinn))
                                linetype = TTV_LINE_A;
                            else
                                linetype = 0;
                            if ($7 == 'U') {
                                node = &signode->NODE[1];
                                itype = elpRISE;
                            }
                            else if ($7 == 'D') {
                                node = &signode->NODE[0];
                                itype = elpFALL;
                            }
                            if ($8 == 'U') {
                                root = &sigroot->NODE[1];
                                otype = elpRISE;
                            }
                            else if ($8 == 'D') {
                                root = &sigroot->NODE[0];
                                otype = elpFALL;
                            }
                            else if ($8 == 'u') {
                                root = &sigroot->NODE[1];
                                otype = elpRISE;
                                linetype |= TTV_LINE_HZ;
                            }
                            else if ($8 == 'd') {
                                root = &sigroot->NODE[0];
                                otype = elpFALL;
                                linetype |= TTV_LINE_HZ;
                            }

                            delaymodelmax = stm_getmodel (cname, maxdlymname);
                            if(gethtitem(modshrinkht, maxdlymname) == EMPTYHT){
                                if (itype == elpRISE)
                                   stm_mod_shrinkslewaxis (delaymodelmax, risethmin, risethmax, itype);
                                else
                                   stm_mod_shrinkslewaxis (delaymodelmax, fallthmin, fallthmax, itype);
                                addhtitem (modshrinkht, maxdlymname, (long)delaymodelmax);
                            }
    
    
                            slewmodelmax  = stm_getmodel (cname, maxslwmname);
                            if(gethtitem(modshrinkht, maxslwmname) == EMPTYHT){
                                if (itype == elpRISE)
                                   stm_mod_shrinkslewaxis (slewmodelmax, risethmin, risethmax, itype);
                                else
                                   stm_mod_shrinkslewaxis (slewmodelmax, fallthmin, fallthmax, itype);
                                if (otype == elpRISE)
                                   stm_mod_shrinkslewdata (slewmodelmax, risethmin, risethmax, otype);
                                else   
                                   stm_mod_shrinkslewdata (slewmodelmax, fallthmin, fallthmax, otype);
                                addhtitem (modshrinkht, maxslwmname, (long)slewmodelmax);
                            }
    
                            delaymodelmin = stm_getmodel (cname, mindlymname);
                            if(gethtitem(modshrinkht, mindlymname) == EMPTYHT){
                                if (itype == elpRISE)
                                   stm_mod_shrinkslewaxis (delaymodelmin, risethmin, risethmax, itype);
                                else
                                   stm_mod_shrinkslewaxis (delaymodelmin, fallthmin, fallthmax, itype);
                                addhtitem (modshrinkht, mindlymname, (long)delaymodelmin);
                            }
                            
                            slewmodelmin  = stm_getmodel (cname, minslwmname);
                            if(gethtitem(modshrinkht, minslwmname) == EMPTYHT){
                                if (itype == elpRISE)
                                   stm_mod_shrinkslewaxis (slewmodelmin, risethmin, risethmax, itype);
                                else
                                   stm_mod_shrinkslewaxis (slewmodelmin, fallthmin, fallthmax, itype);
                                if (otype == elpRISE)
                                   stm_mod_shrinkslewdata (slewmodelmin, risethmin, risethmax, otype);
                                else
                                   stm_mod_shrinkslewdata (slewmodelmin, fallthmin, fallthmax, otype);
                                addhtitem (modshrinkht, minslwmname, (long)slewmodelmin);
                            }
                    
                            if ($8 == 'D') {
                                stm_mod_update(delaymodelmax, voltage/2.0, voltage, voltage/5.0, 0.0);
                                stm_mod_update(slewmodelmax, voltage/2.0, voltage, voltage/5.0, 0.0);
                                stm_mod_update(delaymodelmin, voltage/2.0, voltage, voltage/5.0, 0.0);
                                stm_mod_update(slewmodelmin, voltage/2.0, voltage, voltage/5.0, 0.0);
                            }else{
                                stm_mod_update(delaymodelmax, voltage/2.0, voltage, voltage/5.0, voltage);
                                stm_mod_update(slewmodelmax, voltage/2.0, voltage, voltage/5.0, voltage);
                                stm_mod_update(delaymodelmin, voltage/2.0, voltage, voltage/5.0, voltage);
                                stm_mod_update(slewmodelmin, voltage/2.0, voltage, voltage/5.0, voltage);
                            }

                            capa = gethtitem (pincapht, pinr);
                            if (capa != EMPTYHT) {
                                slwmin = stm_mod_slew (slewmodelmin, *(float*)capa + STM_DEF_LOAD, STM_DEF_SLEW, NULL, NULL, "unknown");
                                dlymin = stm_mod_delay (delaymodelmin, *(float*)capa + STM_DEF_LOAD, STM_DEF_SLEW, NULL, "unknown");
                                slwmax = stm_mod_slew (slewmodelmax, *(float*)capa + STM_DEF_LOAD, STM_DEF_SLEW, NULL, NULL, "unknown");
                                dlymax = stm_mod_delay (delaymodelmax, *(float*)capa + STM_DEF_LOAD, STM_DEF_SLEW, NULL, "unknown");
                            } else {
                                slwmin = stm_mod_slew (slewmodelmin, STM_DEF_LOAD, STM_DEF_SLEW, NULL, NULL, "unknown");
                                dlymin = stm_mod_delay (delaymodelmin, STM_DEF_LOAD, STM_DEF_SLEW, NULL, "unknown");
                                slwmax = stm_mod_slew (slewmodelmax, STM_DEF_LOAD, STM_DEF_SLEW, NULL, NULL, "unknown");
                                dlymax = stm_mod_delay (delaymodelmax, STM_DEF_LOAD, STM_DEF_SLEW, NULL, "unknown");
                            }
                            line = ttv_addline (fig, root, node, dlymax*TTV_UNIT, slwmax*TTV_UNIT, dlymin*TTV_UNIT, slwmin*TTV_UNIT, TTV_LINE_T | linetype);
                            ttv_addcaracline (line, maxdlymname, 
                                                                                                  mindlymname, 
                                                                                                  maxslwmname, 
                                                                                                  minslwmname);
                            line = ttv_addline (fig, root, node, dlymax*TTV_UNIT, slwmax*TTV_UNIT, dlymin*TTV_UNIT, slwmin*TTV_UNIT, TTV_LINE_D | linetype);
                            ttv_addcaracline (line, maxdlymname, 
                                                                                                  mindlymname, 
                                                                                                  maxslwmname, 
                                                                                                  minslwmname);
                        }
                    }
                }
                mbkfree(caparray);
             }
             | TLF4_TOKEN_PATH '(' pinname "=>" pinname speed transition transition constdelay constslew energy_list ')'
             {
                if ($7 != 'X' && $8 != 'X') {
                    pinn = $3;
                    pinr = $5;
                    dlymin = $9.MIN != STM_NOVALUE ? $9.MIN : $9.MAX;
                    slwmin = $10.MIN != STM_NOVALUE ? $10.MIN : $10.MAX;
                    dlymax = $9.MAX;
                    slwmax = $10.MAX;
                    signode = (ttvsig_list*)gethtitem (pinht, pinn);
                    sigroot = (ttvsig_list*)gethtitem (pinht, pinr);
                    if ((long)sigroot != EMPTYHT && (long)signode != EMPTYHT && (long)sigroot != DELETEHT && (long)signode != DELETEHT) {
                        if (isclockpin (clocks, pinn))
                            linetype = TTV_LINE_A;
                        else
                            linetype = 0;
                        if ($8 == 'U'){
                            root = &sigroot->NODE[1];
                        }else if ($8 == 'D'){
                            root = &sigroot->NODE[0];
                        }else if ($8 == 'u'){
                            root = &sigroot->NODE[1];
                            linetype |= TTV_LINE_HZ;
                        }else if ($8 == 'd'){
                            root = &sigroot->NODE[0];
                            linetype |= TTV_LINE_HZ;
                        }
                        if ($7 == 'U')
                            node = &signode->NODE[1];
                        else if ($7 == 'D')
                            node = &signode->NODE[0];
                        line = ttv_addline (fig, root, node, dlymax*TTV_UNIT, slwmax*TTV_UNIT, dlymin*TTV_UNIT, slwmin*TTV_UNIT, TTV_LINE_T | linetype);
                        line = ttv_addline (fig, root, node, dlymax*TTV_UNIT, slwmax*TTV_UNIT, dlymin*TTV_UNIT, slwmin*TTV_UNIT, TTV_LINE_D | linetype);
                    }
                }
             }
             | TLF4_TOKEN_PATH '(' pinname "*>" busname speed transition transition constdelay constslew energy_list ')'
             {
                if ($7 != 'X' && $8 != 'X') {
                    for(i = lsb; i <= msb; i++){
                        pinn = $3;
                        pt = stm_vect (bname, i);
                        pinr = namealloc (pt);
                        mbkfree(pt);
                        if(i == msb)
                            mbkfree(bname);
                        dlymin = $9.MIN != STM_NOVALUE ? $9.MIN : $9.MAX;
                        slwmin = $10.MIN != STM_NOVALUE ? $10.MIN : $10.MAX;
                        dlymax = $9.MAX;
                        slwmax = $10.MAX;
                        signode = (ttvsig_list*)gethtitem (pinht, pinn);
                        sigroot = (ttvsig_list*)gethtitem (pinht, pinr);
                        if ((long)sigroot != EMPTYHT && (long)signode != EMPTYHT && (long)sigroot != DELETEHT && (long)signode != DELETEHT) {
                            if (isclockpin (clocks, pinn))
                                linetype = TTV_LINE_A;
                            else
                                linetype = 0;
                            if ($8 == 'U'){
                                root = &sigroot->NODE[1];
                            }else if ($8 == 'D'){
                                root = &sigroot->NODE[0];
                            }else if ($8 == 'u'){
                                root = &sigroot->NODE[1];
                                linetype |= TTV_LINE_HZ;
                            }else if ($8 == 'd'){
                                root = &sigroot->NODE[0];
                                linetype |= TTV_LINE_HZ;
                            }
                            if ($7 == 'U')
                                node = &signode->NODE[1];
                            else if ($7 == 'D')
                                node = &signode->NODE[0];
                            line = ttv_addline (fig, root, node, dlymax*TTV_UNIT, slwmax*TTV_UNIT, dlymin*TTV_UNIT, slwmin*TTV_UNIT, TTV_LINE_T | linetype);
                            line = ttv_addline (fig, root, node, dlymax*TTV_UNIT, slwmax*TTV_UNIT, dlymin*TTV_UNIT, slwmin*TTV_UNIT, TTV_LINE_D | linetype);
                        }
                    }
                }
                mbkfree(caparray);
             }
             | TLF4_TOKEN_PATH '(' pinname "=>" pinname speed transition transition energy ')'
             {
             }
             | TLF4_TOKEN_PATH '(' pinname "*>" busname speed transition transition energy ')'
             {
                mbkfree(caparray);
             }
             ;

setup        : TLF4_TOKEN_SETUP '(' pinname "=>" pinname transition transition constraint ')'
             {
                if ($6 != 'X' && $7 != 'X') {
                    pinn = $3;
                    pinr = $5;
                    maxdlymname = $8.MAX;
                    sigroot = (ttvsig_list*)gethtitem (pinht, pinr);
                    signode = (ttvsig_list*)gethtitem (pinht, pinn);
                    if ((long)sigroot != EMPTYHT && (long)signode != EMPTYHT && (long)sigroot != DELETEHT && (long)signode != DELETEHT) {
                        if ($7 == 'U')
                            root = &sigroot->NODE[1];
                        if ($7 == 'D')
                            root = &sigroot->NODE[0];
                        if ($6 == 'U') {
                            node = &signode->NODE[1];
                            itype = elpRISE;
                        }
                        if ($6 == 'D') {
                            node = &signode->NODE[0];
                            itype = elpFALL;
                        }
                        delaymodelmax = stm_getmodel (cname, maxdlymname);
                        if(gethtitem(modshrinkht, maxdlymname) == EMPTYHT){
                            if (itype == elpRISE)
                               stm_mod_shrinkslewaxis (delaymodelmax, risethmin, risethmax, itype);
                            else
                               stm_mod_shrinkslewaxis (delaymodelmax, fallthmin, fallthmax, itype);
                            addhtitem (modshrinkht, maxdlymname, (long)delaymodelmax);
                        }
                        if (!delaymodelmax)
                            printf ("no model %s\n", maxdlymname);
                        maxcstr = stm_mod_constraint (delaymodelmax, STM_DEF_SLEW, STM_DEF_SLEW);
                        if (itype == elpFALL) {
                            stm_mod_update(delaymodelmax, voltage/2.0, voltage, voltage/5.0, 0.0);
                        }else{
                            stm_mod_update(delaymodelmax, voltage/2.0, voltage, voltage/5.0, voltage);
                        }
                        line = ttv_addline (fig, root, node, maxcstr*TTV_UNIT, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_T | TTV_LINE_U);
                        ttv_addcaracline (line, maxdlymname, NULL, NULL, NULL);
                        line = ttv_addline (fig, root, node, maxcstr*TTV_UNIT, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_D | TTV_LINE_U);
                        ttv_addcaracline (line, maxdlymname, NULL, NULL, NULL);
                    }   
                }
             }
             | TLF4_TOKEN_SETUP '(' busname "*>" pinname transition transition constraint ')'
             {
                if ($6 != 'X' && $7 != 'X') {
                    for(i = lsb; i <= msb; i++){
                        pt = stm_vect (bname, i);
                        pinn = namealloc (pt);
                        mbkfree(pt);
                        if(i == msb)
                            mbkfree(bname);
                        pinr = $5;
                        maxdlymname = $8.MAX;
                        sigroot = (ttvsig_list*)gethtitem (pinht, pinr);
                        signode = (ttvsig_list*)gethtitem (pinht, pinn);
                        if ((long)sigroot != EMPTYHT && (long)signode != EMPTYHT && (long)sigroot != DELETEHT && (long)signode != DELETEHT) {
                            if ($7 == 'U')
                                root = &sigroot->NODE[1];
                            if ($7 == 'D')
                                root = &sigroot->NODE[0];
                            if ($6 == 'U') {
                                node = &signode->NODE[1];
                                itype = elpRISE;
                            }
                            if ($6 == 'D') {
                                node = &signode->NODE[0];
                                itype = elpFALL;
                            }
                            delaymodelmax = stm_getmodel (cname, maxdlymname);
                            if(gethtitem(modshrinkht, maxdlymname) == EMPTYHT){
                                if (itype == elpRISE)
                                   stm_mod_shrinkslewaxis (delaymodelmax, risethmin, risethmax, itype);
                                else
                                   stm_mod_shrinkslewaxis (delaymodelmax, fallthmin, fallthmax, itype);
                                addhtitem (modshrinkht, maxdlymname, (long)delaymodelmax);
                            }
                            if (!delaymodelmax)
                                printf ("no model %s\n", maxdlymname);
                            maxcstr = stm_mod_constraint (delaymodelmax, STM_DEF_SLEW, STM_DEF_SLEW);
                            if (itype == elpFALL) {
                                stm_mod_update(delaymodelmax, voltage/2.0, voltage, voltage/5.0, 0.0);
                            }else{
                                stm_mod_update(delaymodelmax, voltage/2.0, voltage, voltage/5.0, voltage);
                            }
                            line = ttv_addline (fig, root, node, maxcstr*TTV_UNIT, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_T | TTV_LINE_U);
                            ttv_addcaracline (line, maxdlymname, NULL, NULL, NULL);
                            line = ttv_addline (fig, root, node, maxcstr*TTV_UNIT, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_D | TTV_LINE_U);
                            ttv_addcaracline (line, maxdlymname, NULL, NULL, NULL);
                        }   
                    }
                }
                mbkfree(caparray);
             }
             | TLF4_TOKEN_SETUP '(' pinname "=>" pinname transition transition constant ')'
             {
                if ($6 != 'X' && $7 != 'X') {
                    pinn = $3;
                    pinr = $5;
                    maxcstr = $8.MAX;
                    sigroot = (ttvsig_list*)gethtitem (pinht, pinr);
                    signode = (ttvsig_list*)gethtitem (pinht, pinn);
                    if ((long)sigroot != EMPTYHT && (long)signode != EMPTYHT && (long)sigroot != DELETEHT && (long)signode != DELETEHT) {
                        if ($7 == 'U')
                            root = &sigroot->NODE[1];
                        if ($7 == 'D')
                            root = &sigroot->NODE[0];
                        if ($6 == 'U')
                            node = &signode->NODE[1];
                        if ($6 == 'D')
                            node = &signode->NODE[0];
                        line = ttv_addline (fig, root, node, maxcstr*TTV_UNIT, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_T | TTV_LINE_U);
                        line = ttv_addline (fig, root, node, maxcstr*TTV_UNIT, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_D | TTV_LINE_U);
                    }   
                }
             }
             | TLF4_TOKEN_SETUP '(' busname "*>" pinname transition transition constant ')'
             {
                if ($6 != 'X' && $7 != 'X') {
                    for(i = lsb; i <= msb; i++){
                        pt = stm_vect (bname, i);
                        pinn = namealloc (pt);
                        mbkfree(pt);
                        if(i == msb)
                            mbkfree(bname);
                        pinr = $5;
                        maxcstr = $8.MAX;
                        sigroot = (ttvsig_list*)gethtitem (pinht, pinr);
                        signode = (ttvsig_list*)gethtitem (pinht, pinn);
                        if ((long)sigroot != EMPTYHT && (long)signode != EMPTYHT && (long)sigroot != DELETEHT && (long)signode != DELETEHT) {
                            if ($7 == 'U')
                                root = &sigroot->NODE[1];
                            if ($7 == 'D')
                                root = &sigroot->NODE[0];
                            if ($6 == 'U')
                                node = &signode->NODE[1];
                            if ($6 == 'D')
                                node = &signode->NODE[0];
                            line = ttv_addline (fig, root, node, maxcstr*TTV_UNIT, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_T | TTV_LINE_U);
                            line = ttv_addline (fig, root, node, maxcstr*TTV_UNIT, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_D | TTV_LINE_U);
                        }
                    }
                }
                mbkfree(caparray);
             }
             ;
             
hold         : TLF4_TOKEN_HOLD '(' pinname "=>" pinname transition transition constraint ')'
             {
                if ($6 != 'X' && $7 != 'X') {
                    pinr = $5;
                    pinn = $3;
                    maxdlymname = $8.MAX;
                    sigroot = (ttvsig_list*)gethtitem (pinht, pinr);
                    signode = (ttvsig_list*)gethtitem (pinht, pinn);
                    if ((long)sigroot != EMPTYHT && (long)signode != EMPTYHT && (long)sigroot != DELETEHT && (long)signode != DELETEHT) {
                        if ($7 == 'U')
                            root = &sigroot->NODE[1];
                        if ($7 == 'D')
                            root = &sigroot->NODE[0];
                        if ($6 == 'U') {
                            node = &signode->NODE[1];
                            itype = elpRISE;
                        }
                        if ($6 == 'D') {
                            node = &signode->NODE[0];
                            itype = elpFALL;
                        }
                        delaymodelmax = stm_getmodel (cname, maxdlymname);
                        if(gethtitem(modshrinkht, maxdlymname) == EMPTYHT){
                            if (itype == elpRISE)
                               stm_mod_shrinkslewaxis (delaymodelmax, risethmin, risethmax, itype);
                            else
                               stm_mod_shrinkslewaxis (delaymodelmax, fallthmin, fallthmax, itype);
                            addhtitem (modshrinkht, maxdlymname, (long)delaymodelmax);
                        }
                        if (!delaymodelmax)
                            printf ("no model %s\n", maxdlymname);
                        maxcstr = stm_mod_constraint (delaymodelmax, STM_DEF_SLEW, STM_DEF_SLEW);
                        if (itype == elpFALL) {
                            stm_mod_update(delaymodelmax, voltage/2.0, voltage, voltage/5.0, 0.0);
                        }else{
                            stm_mod_update(delaymodelmax, voltage/2.0, voltage, voltage/5.0, voltage);
                        }
                        line = ttv_addline (fig, root, node, maxcstr*TTV_UNIT, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_T | TTV_LINE_O);
                        ttv_addcaracline (line, maxdlymname, NULL, NULL, NULL);
                        line = ttv_addline (fig, root, node, maxcstr*TTV_UNIT, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_D | TTV_LINE_O);
                        ttv_addcaracline (line, maxdlymname, NULL, NULL, NULL);
                    }   
                }
             }
             | TLF4_TOKEN_HOLD '(' busname "*>" pinname transition transition constraint ')'
             {
                if ($6 != 'X' && $7 != 'X') {
                    for(i = lsb; i <= msb; i++){
                        pinr = $5;
                        pt = stm_vect (bname, i);
                        pinn = namealloc(pt);
                        mbkfree(pt);
                        if(i == msb)
                            mbkfree(bname);
                        maxdlymname = $8.MAX;
                        sigroot = (ttvsig_list*)gethtitem (pinht, pinr);
                        signode = (ttvsig_list*)gethtitem (pinht, pinn);
                        if ((long)sigroot != EMPTYHT && (long)signode != EMPTYHT && (long)sigroot != DELETEHT && (long)signode != DELETEHT) {
                            if ($7 == 'U')
                                root = &sigroot->NODE[1];
                            if ($7 == 'D')
                                root = &sigroot->NODE[0];
                            if ($6 == 'U') {
                                node = &signode->NODE[1];
                                itype = elpRISE;
                            }
                            if ($6 == 'D') {
                                node = &signode->NODE[0];
                                itype = elpFALL;
                            }
                            delaymodelmax = stm_getmodel (cname, maxdlymname);
                            if(gethtitem(modshrinkht, maxdlymname) == EMPTYHT){
                                if (itype == elpRISE)
                                   stm_mod_shrinkslewaxis (delaymodelmax, risethmin, risethmax, itype);
                                else
                                   stm_mod_shrinkslewaxis (delaymodelmax, fallthmin, fallthmax, itype);
                                addhtitem (modshrinkht, maxdlymname, (long)delaymodelmax);
                            }
                            if (!delaymodelmax)
                                printf ("no model %s\n", maxdlymname);
                            maxcstr = stm_mod_constraint (delaymodelmax, STM_DEF_SLEW, STM_DEF_SLEW);
                            if (itype == elpFALL) {
                                stm_mod_update(delaymodelmax, voltage/2.0, voltage, voltage/5.0, 0.0);
                            }else{
                                stm_mod_update(delaymodelmax, voltage/2.0, voltage, voltage/5.0, voltage);
                            }
                            line = ttv_addline (fig, root, node, maxcstr*TTV_UNIT, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_T | TTV_LINE_O);
                            ttv_addcaracline (line, maxdlymname, NULL, NULL, NULL);
                            line = ttv_addline (fig, root, node, maxcstr*TTV_UNIT, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_D | TTV_LINE_O);
                            ttv_addcaracline (line, maxdlymname, NULL, NULL, NULL);
                        }   
                    }
                }
                mbkfree(caparray);
             }
             | TLF4_TOKEN_HOLD '(' pinname "=>" pinname transition transition constant ')'
             {
                if ($6 != 'X' && $7 != 'X') {
                    pinr = $5;
                    pinn = $3;
                    maxcstr = $8.MAX;
                    sigroot = (ttvsig_list*)gethtitem (pinht, pinr);
                    signode = (ttvsig_list*)gethtitem (pinht, pinn);
                    if ((long)sigroot != EMPTYHT && (long)signode != EMPTYHT && (long)sigroot != DELETEHT && (long)signode != DELETEHT) {
                        if ($7 == 'U')
                            root = &sigroot->NODE[1];
                        if ($7 == 'D')
                            root = &sigroot->NODE[0];
                        if ($6 == 'U')
                            node = &signode->NODE[1];
                        if ($6 == 'D')
                            node = &signode->NODE[0];
                        line = ttv_addline (fig, root, node, maxcstr*TTV_UNIT, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_T | TTV_LINE_O);
                        line = ttv_addline (fig, root, node, maxcstr*TTV_UNIT, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_D | TTV_LINE_O);
                    } 
                }
             }
             | TLF4_TOKEN_HOLD '(' busname "*>" pinname transition transition constant ')'
             {
                if ($6 != 'X' && $7 != 'X') {
                    for(i = lsb; i <= msb; i++){
                        pinr = $5;
                        pt = stm_vect (bname, i);
                        pinn = namealloc (pt);
                        mbkfree(pt);
                        if(i == msb)
                            mbkfree(bname);
                        maxcstr = $8.MAX;
                        sigroot = (ttvsig_list*)gethtitem (pinht, pinr);
                        signode = (ttvsig_list*)gethtitem (pinht, pinn);
                        if ((long)sigroot != EMPTYHT && (long)signode != EMPTYHT && (long)sigroot != DELETEHT && (long)signode != DELETEHT) {
                            if ($7 == 'U')
                                root = &sigroot->NODE[1];
                            if ($7 == 'D')
                                root = &sigroot->NODE[0];
                            if ($6 == 'U')
                                node = &signode->NODE[1];
                            if ($6 == 'D')
                                node = &signode->NODE[0];
                            line = ttv_addline (fig, root, node, maxcstr*TTV_UNIT, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_T | TTV_LINE_O);
                            line = ttv_addline (fig, root, node, maxcstr*TTV_UNIT, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_D | TTV_LINE_O);
                        }
                    }
                }
                mbkfree(caparray);
             }
             ;

transition   : TLF4_TOKEN_10
             {
                $$ = 'D';
             }
             | TLF4_TOKEN_01
             {
                $$ = 'U';
             }
             | TLF4_TOKEN_0Z
             {
                $$ = 'u';
             }
             | TLF4_TOKEN_1Z
             {
                $$ = 'd';
             }
             | TLF4_TOKEN_Z0
             {
                $$ = 'D';
             }
             | TLF4_TOKEN_Z1
             {
                $$ = 'U';
             }
             | clocktrans
             {
                $$ = $1;
             }
             ;

clocktrans   : TLF4_TOKEN_NEGEDGE
             {
                $$ = 'D'; 
             }
             | TLF4_TOKEN_POSEDGE
             {
                $$ = 'U';
             }
             | TLF4_TOKEN_HIGH
             {
                $$ = 'H';
             }
             | TLF4_TOKEN_LOW
             {
                $$ = 'L';
             }
             ;

energy_list  : energy
             {
             }
             | empty
             ;

energy       : TLF4_TOKEN_ENERGY '(' modelname ')'
             | TLF4_TOKEN_ENERGY '(' rise fall ')'
             | TLF4_TOKEN_ENERGY constant
             ;

vdrop_limit  : TLF4_TOKEN_VDROP_LIMIT '(' val ')'
             ;
             
constant     : '(' TLF4_TOKEN_CONST '(' cval ')' ')'
             {
                if (TLF_TIME_UNIT == TLF_NS) {
                    if($4.MIN != STM_NOVALUE)
                        $$.MIN = $4.MIN * 1e3;
                    else
                        $$.MIN = $4.MIN;
                    if($4.MAX != STM_NOVALUE)
                        $$.MAX = $4.MAX * 1e3;
                    else
                        $$.MAX = $4.MAX;
                }
                if (TLF_TIME_UNIT == TLF_PS)
                    $$ = $4;
             }
             ;
constdelay   : TLF4_TOKEN_DELAY '(' TLF4_TOKEN_CONST '(' cval ')' ')'
             {
                $$ = $5;
             }
             ;

constslew    : TLF4_TOKEN_SLEW '(' TLF4_TOKEN_CONST '(' cval ')' ')'
             {
                $$ = $5;
             }
             ;

constraint   : modelname
             {
                if (gethtitem (doublemodels, namealloc ($1)) != EMPTYHT) {
                    sprintf (namebuf, "%s_min", $1);
                    $$.MIN = namealloc (namebuf);
                    sprintf (namebuf, "%s_max", $1);
                    $$.MAX = namealloc (namebuf);
                } else {
                    $$.MIN = NULL;
                    $$.MAX = namealloc ($1);
                }
             }
             ;

delay        : TLF4_TOKEN_DELAY '(' modelname ')'
             {
                if (gethtitem (doublemodels, namealloc ($3)) != EMPTYHT) {
                    sprintf (namebuf, "%s_min", $3);
                    $$.MIN = namealloc (namebuf);
                    sprintf (namebuf, "%s_max", $3);
                    $$.MAX = namealloc (namebuf);
                } else {
                    $$.MIN = NULL;
                    $$.MAX = namealloc ($3);
                }
             }
             ;


slew         : TLF4_TOKEN_SLEW  '(' modelname ')'
             {
                if (gethtitem (doublemodels, namealloc ($3)) != EMPTYHT) {
                    sprintf (namebuf, "%s_min", $3);
                    $$.MIN = namealloc (namebuf);
                    sprintf (namebuf, "%s_max", $3);
                    $$.MAX = namealloc (namebuf);
                } else {
                    $$.MIN = NULL;
                    $$.MAX = namealloc ($3);
                }
             }
             ;

templatename : empty       
             { 
                $$ = NULL; 
             }
             | ident 
             { 
                $$ = $1;   
             }
             ;

busname      : TLF4_TOKEN_VECT_IDENT 
             { 
                bname = stm_basename ($1);
                l = stm_lsb ($1);
                m = stm_msb ($1);
                mbkfree($1);
                lsb = l < m ? l : m;
                msb = m > l ? m : l;
                caparray = (float*)mbkalloc ((msb - lsb + 1) * sizeof (float));
                for (i = 0; i < msb - lsb; i++)
                    caparray[i] = TLF_NO_CAPA;
             } 
             ;

pinname      : ident 
             { 
                $$ = $1;
             }
             ;

modelname    : ident 
             { 
                $$ = $1;
             } 
             ;

cellname     : ident  
             { 
                $$ = $1;
             }
             ;

cval         : val ':' val ':' val
             {
                $$.MIN = $1;
                $$.MAX = $5;
             }
             | val ':' ':' val
             {
                $$.MIN = $1;
                $$.MAX = $4;
             }
             | val
             {
                $$.MIN = STM_NOVALUE;
                $$.MAX = $1;
             }
             ;

val          : TLF4_TOKEN_NUMBER 
             { 
                $$ = $1;
             }
             | TLF4_TOKEN_ENUMBER 
             { 
                $$ = $1;
             } 
             | TLF4_TOKEN_10
             {
                $$ = 10 ;
             }
             | '~'
             {
                $$ = 1000000 ;
             }
             ;

ident        : TLF4_TOKEN_IDENT
             {
                $$ = $1;
             }
             ;

empty        : /* empty */ 
             ;

%%

extern char stbtext[];

int yyerror ()
{
    avt_errmsg (STM_ERRMSG, "051", AVT_ERROR, TLF_LINE);
    return 0;
}


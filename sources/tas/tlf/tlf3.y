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
#include "tlf_util.h"
#include "tlf_parse.h"
#include "tlf_env.h"

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

static chain_list       *output;

static long              losig_index;
static lofig_list       *ptrlofig;
static losig_list       *ptrlosig;
static locon_list       *ptrlocon;

static char             *cname;

static char              namebuf[1024];
static chain_list       *ch, *chp, *xchain, *ychain, *xydatamin, *xydatamax, *ydatamin, *ydatamax;
static char              xtype, ytype;
static char              dimension;
static int               nx, ny;
static float            *ptfl;
static timing_model     *modelmin;
static timing_model     *modelmax;
static timing_ttable    *ttemplate;
static char             *cname;
static char             *version;
static ttvfig_list      *fig;
static int               nbconsig, nbbreaksig, nblatchsig;
static chain_list       *consigchain, *breaksigchain, *latchsigchain;
static ttvevent_list    *root, *node;
static ttvline_list     *line;
static ht               *pinht;
static ht               *pincapht;
static ht               *doublemodels;
static chain_list       *clocks;
static int               i, l, m, lsb, msb;
static ttvsig_list      *sigroot, *signode, *sig;
static char             *pinr, *pinn, *pname, *bname, *vectname;
static char             *mindlymname, *maxdlymname, *minslwmname, *maxslwmname;
static long              linetype;
static long              capa;
static float             slwmin, slwmax, dlymin, dlymax, mincstr, maxcstr;
static float            *caparray;
static float             pincapa;
static int               v;
static timing_model     *delaymodelmin, *slewmodelmin;
static timing_model     *delaymodelmax, *slewmodelmax;
static long              dir;
static chain_list       *flipflops;
static chain_list       *latchs;
static minmax           *ptmm;
static segment          *ptseg;
static double            thmin = 0.1, thmax = 0.9;
static double            inputdth = 0.5, outputdth = 0.5;
static double            voltage, temperature;
static int               itype, otype;
static timing_props     *properties;


/******************************************************************************/
/* function declarations                                                      */
/******************************************************************************/

int yyerror();
int yylex();

%}

%union {
  char                                        *text;
  float                                        real;
  struct chain                                *chainlist;
  struct { struct chain *CHAIN; char TYPE; } axedef;
  struct { struct chain *CHAIN; char DIM;  } datadef;
  struct { float MIN; float MAX; }           valdef;
  struct { char *MIN; char *MAX; }           bitext;
  char                                         cchar;
  long                                         llong;
  struct segment                               *ssegment;
};

%token            TLF3_TOKEN_01
%token            TLF3_TOKEN_0Z
%token            TLF3_TOKEN_10
%token            TLF3_TOKEN_1Z
%token            TLF3_TOKEN_ARROW "=>"
%token            TLF3_TOKEN_ASYNCH
%token            TLF3_TOKEN_BIDIR
%token            TLF3_TOKEN_CELL
%token            TLF3_TOKEN_CELLTYPE
%token            TLF3_TOKEN_CLEAR
%token            TLF3_TOKEN_CLOCK
%token            TLF3_TOKEN_CLOCK_REG
%token            TLF3_TOKEN_CLOCK_SLEW_AXIS
%token            TLF3_TOKEN_COMB
%token            TLF3_TOKEN_CONST
%token            TLF3_TOKEN_CONTROL
%token            TLF3_TOKEN_DATA
%token            TLF3_TOKEN_DATE
%token            TLF3_TOKEN_DELAY
%token <real>     TLF3_TOKEN_ENABLE
%token <real>     TLF3_TOKEN_ENUMBER
%token            TLF3_TOKEN_ENVIRONMENT
%token <text>     TLF3_TOKEN_EXPR
%token            TLF3_TOKEN_FAST
%token            TLF3_TOKEN_FOR_BITS
%token            TLF3_TOKEN_FUNCTION
%token            TLF3_TOKEN_GROUND
%token            TLF3_TOKEN_HEADER
%token            TLF3_TOKEN_HIGH
%token            TLF3_TOKEN_HOLD
%token <text>     TLF3_TOKEN_IDENT
%token            TLF3_TOKEN_IGNORE 
%token            TLF3_TOKEN_INCELL
%token            TLF3_TOKEN_INPUT
%token            TLF3_TOKEN_INPUT_REG
%token            TLF3_TOKEN_INPUT_SLEW_AXIS
%token            TLF3_TOKEN_INVERTED_OUTPUT
%token            TLF3_TOKEN_LATCH
%token            TLF3_TOKEN_LIBRARY
%token            TLF3_TOKEN_LOAD_AXIS
%token            TLF3_TOKEN_AXIS
%token            TLF3_TOKEN_LOW
%token            TLF3_TOKEN_MODEL
%token            TLF3_TOKEN_NEGEDGE
%token <real>     TLF3_TOKEN_NUMBER
%token            TLF3_TOKEN_OPE
%token            TLF3_TOKEN_OUTCELL
%token            TLF3_TOKEN_OUTPUT
%token            TLF3_TOKEN_PATH
%token            TLF3_TOKEN_PIN
%token            TLF3_TOKEN_PINDIR
%token            TLF3_TOKEN_PINTYPE
%token            TLF3_TOKEN_PIN_CAP
%token            TLF3_TOKEN_POSEDGE
%token <text>     TLF3_TOKEN_QSTRING
%token            TLF3_TOKEN_REGISTER
%token            TLF3_TOKEN_SEQ
%token            TLF3_TOKEN_SET
%token            TLF3_TOKEN_SETUP
%token            TLF3_TOKEN_SLAVE_CLOCK
%token            TLF3_TOKEN_SLEW
%token            TLF3_TOKEN_SLOW
%token            TLF3_TOKEN_SLEW_AXIS
%token            TLF3_TOKEN_SPLINE
%token            TLF3_TOKEN_TECHNOLOGY
%token            TLF3_TOKEN_TIMING_PROPS
%token            TLF3_TOKEN_TLF_VERSION
%token            TLF3_TOKEN_TRISTATE
%token            TLF3_TOKEN_TRANSITION_START
%token            TLF3_TOKEN_TRANSITION_END
%token            TLF3_TOKEN_INPUT_THRESHOLD
%token            TLF3_TOKEN_OUTPUT_THRESHOLD
%token <text>     TLF3_TOKEN_VECT
%token <text>     TLF3_TOKEN_VECT_IDENT
%token            TLF3_TOKEN_VENDOR
%token            TLF3_TOKEN_VERSION
%token            TLF3_TOKEN_Z0
%token            TLF3_TOKEN_Z1
%token            TLF3_TOKEN_LINEAR
%token            TLF3_TOKEN_VALUE
%token            TLF3_TOKEN_NET_CAP
%token            TLF3_TOKEN_NET_RES
%token            TLF3_TOKEN_TEMPERATURE
%token            TLF3_TOKEN_VOLTAGE

%start tlf_file 

%type <chainlist> array1D 
%type <chainlist> array2D
%type <chainlist> array1D_list
%type <axedef>    axe 
%type <cchar>     axetype 
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
%type <chainlist> reg_list
%type <chainlist> outputs
%type <real>      pincap
%type <llong>     pindir
%type <text>      pinname
%type <cchar>     pintype
%type <bitext>    slew 
%type <text>      templatename
%type <cchar>     transition 
%type <chainlist> val_list
%type <real>      val
%type <ssegment>  segment

%%

tlf_file     : header genmod_list gentprops cell_list 
             {
                if(TLF_TRACE_MODE == 'Y')
                    fprintf (stdout, "   <--- done\n");
/*                else
                    fprintf(stdout,"DONE!\n");*/
                cbh_delcct();
             }
             ;

genmod_list  : empty
             | genmod_list genmod
             ;
             
/*genmod       : TLF3_TOKEN_MODEL '(' ')'
             ;*/
             
genmod       : netmodel
             ;            
             
netmodel     : TLF3_TOKEN_MODEL '(' modelname '(' modelbody ')' ')'
             {
                modelmax = NULL;
                for (nx = 0, ch = xchain; ch; ch = ch->NEXT) nx++;
                if (xydatamax && !xydatamin) {
                    sprintf (namebuf, "%s", $3);
                    modelmax = stm_addtblmodel (cname, namebuf, nx, 0, xtype, STM_NOTYPE);
                    stm_storemodel (cname, namebuf, modelmax, 0);
                }
                if ((xtype == STM_LOAD) || (xtype == STM_NOTYPE)) {
                    stm_modtbl_setXrange (modelmax->UMODEL.TABLE, xchain, 1);
                }
                switch (dimension) { 
                    case TLF_1D :
                        if (nx > 0 && ny > 0)
                            avt_errmsg (STM_ERRMSG, "000", AVT_ERROR, $3);
                        else {
                            stm_modtbl_set1Dset (modelmax->UMODEL.TABLE, xydatamax, 1);
                            for (ch = xydatamax; ch; ch = ch->NEXT)
                                mbkfree(ch->DATA);
                            if (xydatamax)
                                freechain (xydatamax); 
                        }
                    break;
                    case TLF_CONST :
                        if (nx > 0 && ny > 0)
                            avt_errmsg (STM_ERRMSG, "000", AVT_ERROR, $3);
                        else {
                            stm_modtbl_setconst (modelmax->UMODEL.TABLE, *(float*)xydatamax->DATA);
                            for (ch = xydatamax; ch; ch = ch->NEXT)
                                mbkfree(ch->DATA);
                            if (xydatamax)
                                freechain (xydatamax); 
                        }
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
             |  TLF3_TOKEN_MODEL '(' modelname '(' TLF3_TOKEN_LINEAR TLF3_TOKEN_VALUE '(' segment_list ')' ')' ')'
             {
                modelmax = stm_addmodel(cname, $3);
                modelmax->UTYPE = STM_MOD_MODTBL;
                modelmax->UMODEL.TABLE = stm_prop_seg2tbl ($8, STM_LOAD);
                stm_storemodel (cname, $3, modelmax, 0);
             }
             ;

gentprops    : TLF3_TOKEN_TIMING_PROPS '(' prop_list ')'
             ;

prop_list    : empty
             | prop_list prop
             ;
             
prop         : threshold
             {
             }
             | delay_th
             {
                if(inputdth != outputdth)
                   fprintf (stderr, "***tlf warning: input and output thresholds are not the same***\n"); 
             }
             | TLF3_TOKEN_TEMPERATURE '(' val ')'
             {
                temperature = $3;
             }
             | TLF3_TOKEN_VOLTAGE '(' val ')'
             {
                voltage = $3;
             }
             | TLF3_TOKEN_NET_CAP '(' ident ')'
             {
                modelmax = stm_getmodel(cname, $3);
                if (TLF_CAPA_UNIT == TLF_PF)
                    for(i = 0; i < modelmax->UMODEL.TABLE->NX; i++)
                        modelmax->UMODEL.TABLE->SET1D[i] = modelmax->UMODEL.TABLE->SET1D[i] * 1000.0;
                stm_addcapmodel(properties, modelmax);
             }
             | TLF3_TOKEN_NET_RES '(' ident ')'
             {
                stm_addresmodel(properties, stm_getmodel(cname, $3));
             }
             ;

delay_th     : TLF3_TOKEN_INPUT_THRESHOLD '(' val ')' 
             {
               inputdth = $3;
             }
             | TLF3_TOKEN_OUTPUT_THRESHOLD '(' val ')' 
             {
               outputdth = $3;
             }
             ;
             
threshold    : TLF3_TOKEN_TRANSITION_START '(' val ')'
             {
               thmin = $3;
             }
             | TLF3_TOKEN_TRANSITION_END '(' val ')'
             {
               thmax = $3;
             }
             ;

header       : TLF3_TOKEN_HEADER '(' helem_list ')'
             {
/*               if(TLF_TRACE_MODE != 'Y') {
                   fprintf(stdout,"\nLOADING LIBRARY %s TLF3... ", LIBRARY_TLF_NAME);
                   fflush( stdout );
               }*/
               output = NULL;
               losig_index = 0;
               ptrlofig = NULL;
               ptrlosig = NULL;
               ptrlocon = NULL;
               ptrcbhcomb = NULL; 
               ptrcbhseq = NULL;
                
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

library      : TLF3_TOKEN_LIBRARY '(' TLF3_TOKEN_QSTRING ')'
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
                if(!STM_PROPERTIES)
                    properties = stm_prop_create (NULL, NULL);
                else
                    properties = STM_PROPERTIES;
                mbkfree($3);
             }
             ;

technology   : TLF3_TOKEN_TECHNOLOGY '(' TLF3_TOKEN_QSTRING ')'
             {
                mbkfree($3);
             }
             ;

date         : TLF3_TOKEN_DATE '(' TLF3_TOKEN_QSTRING ')'
             {
                mbkfree($3);
             }
             ;

vendor       : TLF3_TOKEN_VENDOR '(' TLF3_TOKEN_QSTRING ')'
             {
                mbkfree($3);
             }
             ; 

environment  : TLF3_TOKEN_ENVIRONMENT '(' TLF3_TOKEN_QSTRING ')'
             {
                mbkfree($3);
             }
             ;

version      : TLF3_TOKEN_VERSION '(' TLF3_TOKEN_QSTRING ')'
             {
                mbkfree($3);
             }
             ;

tlf_version  : TLF3_TOKEN_TLF_VERSION '(' TLF3_TOKEN_QSTRING ')'
             {
                mbkfree($3);
             }
             ;

cell_list    : empty
             | cell_list cell 
             ;
             
cell         : TLF3_TOKEN_CELL '(' celldef celltprops definitions arcs ')' 
             {
                locon_list  *p;
                cbhcomb     *c;
                char        *str;
                
                if((ptrcbhseq != NULL) && (!mout)) {
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

definitions  : def_list
             {
                fig->CONSIG = ttv_allocreflist (consigchain, nbconsig);
                fig->NBCONSIG = nbconsig;
                for (i = 0; i < fig->NBCONSIG; i++) 
                    addhtitem (pinht, namealloc (fig->CONSIG[i]->NAME), (long)fig->CONSIG[i]);
                consigchain = NULL;
                nbconsig = 0;
             }
             ;

def_list     : empty
             | def_list def
             ;

def          : model
             | pin
             | reg
             ;

reg          : TLF3_TOKEN_REGISTER '('
             {
                if(ptrcbhseq != NULL) {
                    mout = tlf_multout(&ptrlofig,&ptrcbhcomb,&ptrcbhseq,cname);
                }
                else {
                    ptrcbhseq = cbh_newseq();
                    ptrcbhseq->SEQTYPE = CBH_FLIPFLOP;
                    ptrcbhseq->RSCONF = NULL;
                    ptrcbhseq->RSCONFNEG = NULL;
               }
             }
              reg_list ')'
             {
                if(ptrcbhseq != NULL)
                    flipflops = append (flipflops, output);
                output = NULL;
             }
             | TLF3_TOKEN_LATCH    '('
             {
                if(ptrcbhseq != NULL) {
                    mout = tlf_multout(&ptrlofig,&ptrcbhcomb,&ptrcbhseq,cname);
                }

                else {
                    ptrcbhseq = cbh_newseq();
                    ptrcbhseq->SEQTYPE = CBH_LATCH;             
                    ptrcbhseq->RSCONF = NULL;
                    ptrcbhseq->RSCONFNEG = NULL;
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

reg_elem : TLF3_TOKEN_CLEAR TLF3_TOKEN_EXPR
         { 
            char    *str;
            str = eqt_ConvertStr($2);
            ptrcbhseq->RESET = eqt_StrToAbl(tlfEqtCtx,str);
            mbkfree (str);
         }
         | TLF3_TOKEN_CLOCK_REG TLF3_TOKEN_EXPR
         {
            char    *str;
            str = eqt_ConvertStr($2);
            ptrcbhseq->CLOCK = eqt_StrToAbl(tlfEqtCtx,str);
            mbkfree (str);
         }
         | TLF3_TOKEN_SLAVE_CLOCK TLF3_TOKEN_EXPR
         {
            char    *str;
            str = eqt_ConvertStr($2);
            ptrcbhseq->SLAVECLOCK = eqt_StrToAbl(tlfEqtCtx,str);
            mbkfree (str);
         }
         | TLF3_TOKEN_SET TLF3_TOKEN_EXPR
         {
            char    *str;
            str = eqt_ConvertStr($2);
            ptrcbhseq->SET = eqt_StrToAbl(tlfEqtCtx,str);
            mbkfree (str);
         }
         | TLF3_TOKEN_INPUT_REG TLF3_TOKEN_EXPR
         {
            char    *str;
            str = eqt_ConvertStr($2);
            ptrcbhseq->DATA = eqt_StrToAbl(tlfEqtCtx,str);
            mbkfree (str);
         }
         ;

outputs      : TLF3_TOKEN_OUTPUT '(' ident_list ')' 
             {
                char pin[1024], name[1024];
                
                if($3->NEXT != NULL)
                    mout = tlf_multout(&ptrlofig,&ptrcbhcomb,&ptrcbhseq,cname);
                else {
                    if(ptrcbhseq != NULL){
                        strcpy(pin,tlf_chainlistToStr($3));
                        sprintf(name,"I%s",pin);
                        ptrcbhseq->PIN = namealloc(pin); 
                        ptrcbhseq->NAME = namealloc(name); 
                    }
                }
                if (output == NULL)
                    output = $3;
                else
                    output = append(output,$3);
             }
             | TLF3_TOKEN_INVERTED_OUTPUT '(' ident_list ')'
             {
                char negpin[1024], negname[1024];
                
                if($3->NEXT != NULL)
                    mout = tlf_multout(&ptrlofig,&ptrcbhcomb,&ptrcbhseq,cname);
                else {
                    if(ptrcbhseq != NULL) {
                        strcpy(negpin,tlf_chainlistToStr($3));
                        sprintf(negname,"I%s",negpin);
                        ptrcbhseq->NEGPIN = namealloc(negpin); 
                        ptrcbhseq->NEGNAME = namealloc(negname);                     
                    }
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
                fig->INFO->STHHIGH       = thmax;
                fig->INFO->STHLOW        = thmin;
                fig->INFO->DTH           = outputdth;
   	            fig->INFO->TEMP          = temperature;
   	            fig->INFO->VDD           = voltage;
                ttv_setttvdate (fig, TTV_DATE_LOCAL);
                pinht = addht (100);
                pincapht = addht (100);
                doublemodels = addht (100);
             }
             ;

celltype     : TLF3_TOKEN_CELLTYPE '(' TLF3_TOKEN_IGNORE ')' 
             {
                $$ = STM_IGNORE;
             }
             | TLF3_TOKEN_CELLTYPE '(' TLF3_TOKEN_OUTCELL ')' 
             {
                $$ = STM_IGNORE;
             }
             | TLF3_TOKEN_CELLTYPE '(' TLF3_TOKEN_INCELL ')' 
             {
                $$ = STM_IGNORE;
             }
             | TLF3_TOKEN_CELLTYPE '(' TLF3_TOKEN_INCELL TLF3_TOKEN_OUTCELL ')' 
             {
                $$ = STM_IGNORE;
             }
             | TLF3_TOKEN_CELLTYPE '(' TLF3_TOKEN_SEQ ')' 
             {
                $$ = STM_SEQ;
             }
             | TLF3_TOKEN_CELLTYPE '(' TLF3_TOKEN_COMB ')' 
             {
                $$ = STM_COMB;
             }
             ;

celltprops   : empty
             | TLF3_TOKEN_TIMING_PROPS '(' ')'
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
             
model        : TLF3_TOKEN_MODEL '(' modelname templatename '(' modelbody ')' ')' 
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
                                    if (modelmin) 
                                        stm_modtbl_setconst (modelmin->UMODEL.TABLE, *(float*)xydatamin->DATA * 1e3);
                                    if (modelmax) 
                                        stm_modtbl_setconst (modelmax->UMODEL.TABLE, *(float*)xydatamax->DATA * 1e3);
                                }
                                if (TLF_TIME_UNIT == TLF_PS) {
                                    if (modelmin) 
                                        stm_modtbl_setconst (modelmin->UMODEL.TABLE, *(float*)xydatamin->DATA);
                                    if (modelmax) 
                                        stm_modtbl_setconst (modelmax->UMODEL.TABLE, *(float*)xydatamax->DATA);
                                }
                                if (xydatamin) {
                                    mbkfree(xydatamin->DATA);
                                    freechain (xydatamin);
                                }
                                if (xydatamax) {
                                    mbkfree(xydatamax->DATA);
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
                for (ch = xchain; ch; ch = ch->NEXT)  
                    mbkfree(ch->DATA);
                freechain (xchain); 
                for (ch = ychain; ch; ch = ch->NEXT)  
                    mbkfree(ch->DATA);
                freechain (ychain); 
             } 
             ;

modelbody    : modeltype axe axe data 
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
                xydatamax = reverse (xydatamax);

                xydatamin = NULL;
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
                freechain (ch);
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
                xydatamax = reverse (xydatamax);
                xydatamin = NULL;
                if (((minmax*)$3.CHAIN->DATA)->MIN != STM_NOVALUE) {
                    for (ch = $3.CHAIN; ch; ch = ch->NEXT) {
                        ptfl = (float*)mbkalloc (sizeof (float));
                        *ptfl = ((minmax*)ch->DATA)->MIN;
                        xydatamin = addchain (xydatamin, ptfl);
                    }
                    xydatamin = reverse (xydatamin);
                }
                for (ch = $3.CHAIN; ch; ch = ch->NEXT) {
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
                for (ch = $2.CHAIN; ch; ch = ch->NEXT) {
                    ptfl = (float*)mbkalloc (sizeof (float));
                    *ptfl = ((minmax*)ch->DATA)->MAX;
                    xydatamax = addchain (xydatamax, ptfl);
                }
                xydatamax = reverse (xydatamax);
                xydatamin = NULL;
                if (((minmax*)$2.CHAIN->DATA)->MIN != STM_NOVALUE) {
                    for (ch = $2.CHAIN; ch; ch = ch->NEXT) {
                        ptfl = (float*)mbkalloc (sizeof (float));
                        *ptfl = ((minmax*)ch->DATA)->MIN;
                        xydatamin = addchain (xydatamin, ptfl);
                    }
                    xydatamin = reverse (xydatamin);
                }
                for (ch = $2.CHAIN; ch; ch = ch->NEXT) {
                    mbkfree ((minmax*)ch->DATA);
                }
                dimension = $2.DIM;
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
             
modeltype    : TLF3_TOKEN_SPLINE 
             | TLF3_TOKEN_CONST  
             ;

axe          : '(' axetype val val val_list ')' 
             { 
                ptfl = (float*)mbkalloc (sizeof (float));
                *ptfl = $4;
                $$.CHAIN = addchain ($5, ptfl);
                ptfl = (float*)mbkalloc (sizeof (float));
                *ptfl = $3;
                $$.CHAIN = addchain ($$.CHAIN, ptfl);
                $$.TYPE = $2;
             } 
             ;

axetype      : TLF3_TOKEN_INPUT_SLEW_AXIS 
             { 
                $$ = STM_INPUT_SLEW;
             }
             | TLF3_TOKEN_CLOCK_SLEW_AXIS       
             { 
                $$ = STM_CLOCK_SLEW;
             }
             | TLF3_TOKEN_SLEW_AXIS       
             { 
                $$ = STM_INPUT_SLEW;
             }
             | TLF3_TOKEN_LOAD_AXIS       
             { 
                $$ = STM_LOAD;
             } 
             | TLF3_TOKEN_AXIS       
             { 
                $$ = STM_NOTYPE;
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

data         : TLF3_TOKEN_DATA '(' modeldata ')' 
             { 
                $$ = $3;
             } 
             | '(' modeldata ')'       
             { 
                $$ = $2;
             } 
             | TLF3_TOKEN_DATA modeldata 
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

pin          : TLF3_TOKEN_PIN '(' pinname
             {
                pname = $3;
             }
               pintype pindir pinfunc pinenable pntprop_list ')'
             {
                int i=0;
                locon_list *c;
                losig_list *s;
                char locondir;

                locondir = $6;
                

                ptfl = (float*)mbkalloc (sizeof (float));
                *ptfl = pincapa;
                addhtitem (pincapht, pname, (long)ptfl);
                if ($6 == 'I') 
                    dir = TTV_SIG_CI;
                  
                if ($6 == 'O') {
                    if ($5 == 'T') {
                        locondir = 'Z';
                        dir = TTV_SIG_CZ;
                    }
                    else 
                        dir = TTV_SIG_CO;
                }
                if ($6 == 'B') {
                    if ($5 == 'T') {
                        locondir = 'T';
                        dir = TTV_SIG_CT;
                    }
                    else 
                        dir = TTV_SIG_CB;
                }
                consigchain = ttv_addrefsig (fig, pname, pname, *ptfl , dir, consigchain);
                nbconsig++;
                if ($5 == 'C')
                    clocks = addchain (clocks, pname);
                pincapa = TLF_NO_CAPA;


                /* nouveau connecteur --> LOCON et LOSIG a creer */
                if($5 != 'G') {
                    for(c=ptrlofig->LOCON; ((c != NULL) && (strcasecmp(c->NAME,pname) != 0)); c=c->NEXT);
                    if(c == NULL) { /* connecteur inexistant, on ajoute signal et connecteur */
                        for(s=ptrlofig->LOSIG; (s != NULL); s=s->NEXT) {/* recherche du 1er index de libre */
                            if (i < s->INDEX)
                                i = s->INDEX;
                        }
                        losig_index = i + 1;
                        ptrlosig = addlosig(ptrlofig, losig_index, addchain(NULL, pname), 'E');
                        ptrlocon = addlocon(ptrlofig, ttv_devect(pname), ptrlosig, locondir);
                    }
                    else { /* connecteur et signal existants, on redirige le connecteur */
                        ptrlocon = c;
                        ptrlocon->DIRECTION = locondir;
                    }

                    if(ptrcbhcomb !=NULL){ 
                        cbh_addcombtolocon(ptrlocon, ptrcbhcomb);
                    }
                }

                ptrcbhcomb = NULL;
                ptrlosig = NULL;
                ptrlocon = NULL;

             }
             | TLF3_TOKEN_PIN '(' vpinname pintype pindir pinfunc vpintprops ')'
             {
             
                for (i = lsb; i <= msb; i++) {
                    vectname = namealloc (stm_vect (bname, i));
                    ptfl = (float*)mbkalloc (sizeof (float));
                    *ptfl = caparray[i - lsb];
                    addhtitem (pincapht, vectname, (long)ptfl);
                    addhtitem (pincapht, pname, (long)ptfl);
                    if ($5 == 'I') 
                        dir = TTV_SIG_CI;
                    if ($5 == 'O') {
                        if ($4 == 'T')
                            dir = TTV_SIG_CZ;
                        else 
                            dir = TTV_SIG_CO;
                    }
                    if ($5 == 'B') {
                        if ($4 == 'T')
                            dir = TTV_SIG_CT;
                        else 
                            dir = TTV_SIG_CB;
                    }   
                    consigchain = ttv_addrefsig (fig, vectname, vectname , *ptfl, dir, consigchain);
                    nbconsig++;
                    if ($4 == 'C')
                        clocks = addchain (clocks, vectname);
                }
                mbkfree(caparray);
             }
             ;

pintype      : TLF3_TOKEN_PINTYPE '(' TLF3_TOKEN_DATA ')'
             {
                $$ = 'D';
             }
             | TLF3_TOKEN_PINTYPE '(' TLF3_TOKEN_CLOCK ')'
             {
                $$ = 'C';
             }
             | TLF3_TOKEN_PINTYPE '(' TLF3_TOKEN_CLOCK clocktrans ')'
             {
                $$ = 'C';
             }
             | TLF3_TOKEN_PINTYPE '(' TLF3_TOKEN_CONTROL TLF3_TOKEN_ASYNCH clocktrans ')'
             {
                $$ = 'H';
             }
             | TLF3_TOKEN_PINTYPE '(' TLF3_TOKEN_GROUND ')'
             {
                $$ = 'G';
             }
             | TLF3_TOKEN_PINTYPE '(' TLF3_TOKEN_DATA TLF3_TOKEN_TRISTATE ')'
             {
                $$ = 'T';
             }
             ;

pindir       : TLF3_TOKEN_PINDIR '(' TLF3_TOKEN_INPUT ')'
             { 
                $$ = 'I';
             } 
             | TLF3_TOKEN_PINDIR '(' TLF3_TOKEN_OUTPUT ')'
             { 
                $$ = 'O';
                ptrcbhcomb = cbh_newcomb();
                ptrcbhcomb->NAME = namealloc(pname);
             } 
             | TLF3_TOKEN_PINDIR '(' TLF3_TOKEN_BIDIR ')'
             { 
                $$ = 'B';
                ptrcbhcomb = cbh_newcomb();
                ptrcbhcomb->NAME = pname;
             } 
             ;

pinfunc      : empty
             {
             }
             |TLF3_TOKEN_FUNCTION  TLF3_TOKEN_EXPR 
             {
                char    *str;
                str = eqt_ConvertStr($2);
                ptrcbhcomb->FUNCTION = eqt_StrToAbl(tlfEqtCtx,str);
                mbkfree (str);
             } 
             ;

pinenable    : empty
             {
             }
             |TLF3_TOKEN_ENABLE TLF3_TOKEN_EXPR
             {
                char    *str;
                if(ptrcbhcomb != NULL) {
                    str = eqt_ConvertStr($2);
                    ptrcbhcomb->HZFUNC = notExpr(eqt_StrToAbl(tlfEqtCtx,str));
                    mbkfree (str);
                }
             }
             ;

pntprop_list : empty
             | pntprop_list pntprop
             ;

pntprop      : TLF3_TOKEN_TIMING_PROPS '(' pincap ')'
             {
                pincapa = $3;
             }
             | TLF3_TOKEN_TIMING_PROPS '(' ')' 
             ;

vpintprops   : TLF3_TOKEN_TIMING_PROPS '(' forbits_list ')'
             {
             }
             ;

forbits_list : forbits
             | forbits_list forbits
             ;

forbits      : TLF3_TOKEN_FOR_BITS '(' TLF3_TOKEN_VECT pincap ')'
             {
                v = atoi (stm_unquote ($3));
                mbkfree($3);
                if (v <= msb && v >= lsb) 
                    caparray[v - lsb] = $4;
             }
             ;

pincap       : TLF3_TOKEN_PIN_CAP '(' val ')'
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

speed        : TLF3_TOKEN_FAST
             | TLF3_TOKEN_SLOW
             | empty
             ;

path         : TLF3_TOKEN_PATH '(' pinname "=>" pinname speed transition transition delay slew ')'
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
                        if ($8 == 'U') {
                            root = &sigroot->NODE[1];
                            otype = elpRISE;
                        }
                        if ($8 == 'D') {
                            root = &sigroot->NODE[0];
                            otype = elpFALL;
                        }
                        if ($7 == 'U') {
                            node = &signode->NODE[1];
                            itype = elpRISE;
                        }
                        if ($7 == 'D') {
                            node = &signode->NODE[0];
                            itype = elpFALL;
                        }
                        if (isclockpin (clocks, pinn))
                            linetype = TTV_LINE_A;
                        else
                            linetype = 0;
                        delaymodelmax = stm_getmodel (cname, maxdlymname);
                        stm_mod_shrinkslewaxis (delaymodelmax, thmin, thmax, itype);

                        slewmodelmax  = stm_getmodel (cname, maxslwmname);
                        stm_mod_shrinkslewaxis (slewmodelmax, thmin, thmax, itype);
                        stm_mod_shrinkslewdata (slewmodelmax, thmin, thmax, otype);
                        
                        delaymodelmin = stm_getmodel (cname, mindlymname);
                        stm_mod_shrinkslewaxis (delaymodelmin, thmin, thmax, itype);

                        slewmodelmin  = stm_getmodel (cname, minslwmname);
                        stm_mod_shrinkslewaxis (slewmodelmin, thmin, thmax, itype);
                        stm_mod_shrinkslewdata (slewmodelmin, thmin, thmax, otype);
                        
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
             | TLF3_TOKEN_PATH '(' pinname "=>" pinname speed transition transition constdelay constslew ')'
             {
                if ($7 != 'X' && $8 != 'X') {
                    pinn = $3;
                    pinr = $5;
                    dlymin = $9.MIN != STM_NOVALUE ? $9.MIN * 1000 : $9.MAX * 1000;
                    slwmin = $10.MIN != STM_NOVALUE ? $10.MIN * 1000 : $10.MAX * 1000;
                    dlymax = $9.MAX * 1000;
                    slwmax = $10.MAX * 1000;
                    signode = (ttvsig_list*)gethtitem (pinht, pinn);
                    sigroot = (ttvsig_list*)gethtitem (pinht, pinr);
                    if ((long)sigroot != EMPTYHT && (long)signode != EMPTYHT) {
                        if ($8 == 'U')
                            root = &sigroot->NODE[1];
                        if ($8 == 'D')
                            root = &sigroot->NODE[0];
                        if ($7 == 'U')
                            node = &signode->NODE[1];
                        if ($7 == 'D')
                            node = &signode->NODE[0];
                        if (isclockpin (clocks, pinn))
                            linetype = TTV_LINE_A;
                        else
                            linetype = 0;
                        line = ttv_addline (fig, root, node, dlymax*TTV_UNIT, slwmax*TTV_UNIT, dlymin*TTV_UNIT, slwmin*TTV_UNIT, TTV_LINE_T | linetype);
                        line = ttv_addline (fig, root, node, dlymax*TTV_UNIT, slwmax*TTV_UNIT, dlymin*TTV_UNIT, slwmin*TTV_UNIT, TTV_LINE_D | linetype);
                    }   
                }
             }
             ;

setup        : TLF3_TOKEN_SETUP '(' pinname "=>" pinname transition transition constraint ')'
             {
                if ($6 != 'X' && $7 != 'X') {
                    pinn = $3;
                    pinr = $5;
                    maxdlymname = $8.MAX;
                    sigroot = (ttvsig_list*)gethtitem (pinht, pinr);
                    signode = (ttvsig_list*)gethtitem (pinht, pinn);
                    if ((long)sigroot != EMPTYHT && (long)signode != EMPTYHT) {
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
                        stm_mod_shrinkslewaxis (delaymodelmax, thmin, thmax, itype);

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
             | TLF3_TOKEN_SETUP '(' pinname "=>" pinname transition transition constant ')'
             {
                if ($6 != 'X' && $7 != 'X') {
                    pinn = $3;
                    pinr = $5;
                    maxcstr = $8.MAX * 1000;
                    sigroot = (ttvsig_list*)gethtitem (pinht, pinr);
                    signode = (ttvsig_list*)gethtitem (pinht, pinn);
                    if ((long)sigroot != EMPTYHT && (long)signode != EMPTYHT) {
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
             ;
             
hold         : TLF3_TOKEN_HOLD '(' pinname "=>" pinname transition transition constraint ')'
             {
                if ($6 != 'X' && $7 != 'X') {
                    pinr = $5;
                    pinn = $3;
                    maxdlymname = $8.MAX;
                    sigroot = (ttvsig_list*)gethtitem (pinht, pinr);
                    signode = (ttvsig_list*)gethtitem (pinht, pinn);
                    if ((long)sigroot != EMPTYHT && (long)signode != EMPTYHT) {
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
                        stm_mod_shrinkslewaxis (delaymodelmax, thmin, thmax, itype);

                        if (!delaymodelmax)
                            printf ("no model %s\n", maxdlymname);
                        maxcstr = stm_mod_constraint (delaymodelmax, STM_DEF_SLEW, STM_DEF_SLEW);
                        if (itype == elpFALL) {
                            stm_mod_update(delaymodelmax, voltage/2.0, voltage, voltage/5.0, 0.0);
                        }else{
                            stm_mod_update(delaymodelmax, voltage/2.0, voltage, voltage/5.0, voltage);
                        }
                        line = ttv_addline (fig, root, node, TTV_NOTIME, TTV_NOSLOPE, mincstr*TTV_UNIT, 0, TTV_LINE_T | TTV_LINE_O);
                        ttv_addcaracline (line, maxdlymname, NULL, NULL, NULL);
                        line = ttv_addline (fig, root, node, TTV_NOTIME, TTV_NOSLOPE, mincstr*TTV_UNIT, 0, TTV_LINE_D | TTV_LINE_O);
                        ttv_addcaracline (line, maxdlymname, NULL, NULL, NULL);
                    }   
                }
             }
             | TLF3_TOKEN_HOLD '(' pinname "=>" pinname transition transition constant ')'
             {
                if ($6 != 'X' && $7 != 'X') {
                    pinr = $5;
                    pinn = $3;
                    maxcstr = $8.MAX * 1000;
                    sigroot = (ttvsig_list*)gethtitem (pinht, pinr);
                    signode = (ttvsig_list*)gethtitem (pinht, pinn);
                    if ((long)sigroot != EMPTYHT && (long)signode != EMPTYHT) {
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
             ;

transition   : TLF3_TOKEN_10
             {
                $$ = 'D';
             }
             | TLF3_TOKEN_01
             {
                $$ = 'U';
             }
             | TLF3_TOKEN_0Z
             {
                $$ = 'X';
             }
             | TLF3_TOKEN_1Z
             {
                $$ = 'X';
             }
             | TLF3_TOKEN_Z0
             {
                $$ = 'D';
             }
             | TLF3_TOKEN_Z1
             {
                $$ = 'U';
             }
             | clocktrans
             {
                $$ = $1;
             }
             ;

clocktrans   : TLF3_TOKEN_NEGEDGE
             {
                $$ = 'D'; 
             }
             | TLF3_TOKEN_POSEDGE
             {
                $$ = 'U';
             }
             | TLF3_TOKEN_HIGH
             {
                $$ = 'H';
             }
             | TLF3_TOKEN_LOW
             {
                $$ = 'L';
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

delay        : TLF3_TOKEN_DELAY '(' modelname ')'
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

slew         : TLF3_TOKEN_SLEW  '(' modelname ')'
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

vpinname     : TLF3_TOKEN_VECT_IDENT 
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

constant     : TLF3_TOKEN_CONST '(' cval ')'
             {
                $$ = $3;
             }
             | TLF3_TOKEN_CONST cval
             {
                $$ = $2;
             }
             ;

constdelay   : TLF3_TOKEN_DELAY '(' TLF3_TOKEN_CONST '(' cval ')' ')'
             {
                $$ = $5;
             }
             ;

constslew    : TLF3_TOKEN_SLEW '(' TLF3_TOKEN_CONST '(' cval ')' ')'
             {
                $$ = $5;
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

val          : TLF3_TOKEN_NUMBER 
             { 
                $$ = $1;
             }
             | TLF3_TOKEN_ENUMBER 
             { 
                $$ = $1;
             } 
             | TLF3_TOKEN_10
             {
                $$ = 10 ;
             } 
             | '~'
             {
                $$ = 1000000 ;
             }
             ;

ident        : TLF3_TOKEN_IDENT
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


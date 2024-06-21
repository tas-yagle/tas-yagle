%{
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <float.h>

#include AVT_H
#include MUT_H
#include INF_H

#define yytext inftext

  extern int infParseLine, inf_ignorename ;
  extern char *infFile, *infname, *yytext;

  /*---------------------------+
    | DECLARATION DES PROTOTYPES |
    +--------------------------*/
  static void checkJokName();
  static char *getwhere();
  static double todoublesec(void *val);

  int yyerror();
  int yylex();

  static inf_carac * carac = NULL ;
  static double *valbuf = NULL ;
  static int i, nbval ;
  static int TBL_BUFSIZE = 100 ;  
  int INF_ERROR = 0;

  char  INF_LANG ;

static void addstab(char *name, misc_type *mscdata, misc_type *mscck, int hzopt, inf_stb_p_s_stab *stab, int stabtype, char *inftype);
static void INFCHECKFIG();
char *infTasVectName(char *name);
static ptype_list *finish_falsepath(ptype_list *pt);

  %}

%union
{
  long ival ;
  double val ;
  ptype_list *falseevent ;
  chain_list *chain;
  char * str ;
  char tab[256];
  misc_type misc;
  inf_stb_p_s_stab *stab;
} 


%token  T_END   T_MUTEX T_CROSSTALK T_PARISITICS
%token  T_RENAME T_VIRTUAL T_IDEAL T_LATENCY
%token  T_SUPPRESS
%token  T_DIROUT
%token  T_DLATCH T_FLIPFLOP T_MODELLOOP
%token  T_MARKSIG T_MARKTRANS T_MARKRS
%token  T_DEFAULT
%token <val> T_VAL
%token <ival> T_IN T_INOUT T_OUT  
%token <ival> T_JOIN
%token T_CONSTRAINT T_STOP T_PRECHARGE T_BYPASS T_SUPPLY T_SENSITIVE
%token T_INTER T_BREAK T_PATHIN T_PATHOUT T_PATHSIGS
%token T_RC T_NORC // T_ELM T_RCN
%token T_NORISING T_NOFALLING
%token T_FALSEPATH T_DELAY T_FALSESLACK
%token T_UP T_DOWN T_VOID
%token T_L T_W
%token T_CAPAOUT T_SLOPEIN
%token T_PINSLEW T_RISE T_FALL
%token T_CLOCK
%token T_ASYNCHRON
%token T_INPUTS
%token T_NOTLATCH T_KEEP_TRISTATE_BEHAVIOUR
%token T_CKLATCH
%token T_CKPRECH
%token T_TEMP T_STRICT_SETUP
%token T_PERIOD T_SWITCHPROBA T_MEMSYM
%token T_OPERATING T_CONDITION T_CAPACITANCE T_DIRECTIVES T_CLOCK_LATENCIES T_CLOCKUNCERTAINTY
%token <ival> T_CMPU T_CMPD T_MUXU T_MUXD
%token <tab> T_NAME
%type  <ival> valeur
%type  <ival> digit
//%type  <ival> time
%type  <ival> updown pathsigtype .pathtag. .disable_date_delay_dir. .updown. falsehztag .updownfalsehztag.
//%type  <ival> clockdatatype
%type  <falseevent> falsename falsename_list ck falseslack
%type  <falseevent> clockdetail_list 
%type  <str> pin
%type  <str> state .multicyclefrom. 
%type <tab> identifier nameordefault path_tag_item
%type  <str> disable_from disable_to
%type  <val> time_value period setup hold .period. .units. .capaunits. capa_value .vclockfactor. .vclockdelta. time_or_nothing
%type <chain> integer_list ckname_list cmux_name_list mux_name_list slope time_values time_val_list capa_values capa_val_list capa crosstalkmux_name_list
%type <misc> edge_option source_phase dest_phase relative_phase group egroup clock_spec .multicycledatadir. latency
%type <misc> range clock_ident edge_up edge_dn pinslew .pinsupply. .clock_spec_gene_info.
%type <ival> relative_type stability_type hz_option .falseslack_type.
%type <stab> stability stability_list
%type <ival> .multicyclestartend. .multicyclesetuphold. .multicycledatarisefall. .virtual. .ideal. clock_type .clockuncertaintyrisefall. 
%type <ival> directive_state_ud .directive_state_rf. directive_state_rf directive_action  .clockuncertaintysetuphold.
%type <val> directive_margin 
%type <chain> namelist
%type <str> connector_direction_spec
%type <misc> directive_name_or_time directive_source_name_or_time .directive_from.

%token        T_BEGIN
%token        T_DEFAULT
%token        T_PERIOD
%token        T_VERSION
%token        T_STABILITY
%token        TOK_NAME
%token        T_SETUPTIME T_SETUPHOLD
%token        T_HOLDTIME
%token        T_CONNECTORS
%token        T_CLOCK
%token        T_GROUPS
%token        T_ASYNCHRONOUS
%token        T_EQUIVALENT
%token        T_DISABLE
%token        T_RISING
%token        T_FALLING
%token        T_FROM
%token        T_FOR
%token        T_TO
%token        T_STABLE
%token        T_UNSTABLE
%token        T_INPUT
%token        T_STABILITY
%token        T_OUTPUT
%token        T_VERIFY
%token        T_SPECIFY
%token        T_INTERNAL
%token        T_NODES
%token        T_AFTER
%token        T_BEFORE
%token        T_CONDITIONED
%token        T_COMMAND
%token        T_STATES
%token        T_NOVERIF T_NOCHECK
%token        T_WITHOUT
%token        T_PRECHARGE
%token        T_EVALUATE
%token        T_MULTIPLE
%token        T_PRIORITY
%token        T_MEMORY T_GATE T_CHARACT
%token        T_UNITNS T_UNITPS T_UNITUS T_UNITPF T_UNITFF
%token        T_PATH T_DELAY T_MARGIN
%token        T_MULTICYCLE T_CYCLE T_START T_WITH
%token        T_IGNORE T_CONNECTOR T_DIRECTIONS T_DONTCROSS T_TRANSPARENT T_STABFLAGS

%start expr 

%%

expr    : info_list  
        ; 

info_list   : /* rien */  
| info_list info
;

info    : mux_info
| crosstalkmux_info
| renom_info
| memsym_info
| suppress_info
| stop_info
| sensitive_info
| dirout_info
| dlatch_info
| flipflop_info
| collage_info
| prech_info
| modloop_info
| bypass_info
| inter_info
| break_info
| pathin_info
| pathout_info
| pathsigs_info
| rc_info
| norc_info
| norising_info
| nofalling_info
| falsepath_info
| delay_info
| capaout_info
| slopein_info
| clock_info
| asynchron_info
| dontcross_info
| transparent_info
| inputs_info
| notlatch_info
| keeptristate_info
| cklatch_info
| ckprech_info
| pinslew_info
| opcond_info
| ignore_info
| connector_direction_info
| multicycle_info
| path_delay_margin_info
| stb_general_info
| stb_ck_declar_info
| stb_disable_info
| stb_wenable_info
| stb_specin_info
| stb_specout_info
| stb_inputs_info
| stb_outputs_info
| stb_memories_info
| stb_internals_info
| nocheck_info
| outcapa_info
| setuphold_info
| stabflags_info
| marksig_info
| marktrans_info
| markRS_info
| disable_gate_delay_info
| characteristic_info
| directives_info
| conditions_info
| falseslack_info
| proba_info
| clocklatencies_info
| strictsetup_info
| clockuncertainty_info
;
  

.sc. : ';'
     |
     ;

.begin. : T_BEGIN
        |
        ;

mux_info        : T_MUTEX .begin. mux_list T_END .sc.
;

mux_list        : /* rien */ 
| mux_list mux
;

mux     : T_MUXU '{' mux_name_list '}' ';'
{
  INFCHECKFIG();
  inf_AddList(INF_FIG, INF_LOADED_LOCATION, INF_MUXU, "", $3, getwhere());
}

| T_MUXD '{' mux_name_list '}' ';'
{
  INFCHECKFIG();
  inf_AddList(INF_FIG, INF_LOADED_LOCATION, INF_MUXD, "", $3, getwhere());
}

| T_CMPU '{' mux_name_list '}' ';'
{
  INFCHECKFIG();
  inf_AddList(INF_FIG, INF_LOADED_LOCATION, INF_CMPU, "", $3, getwhere());
}

| T_CMPD '{' mux_name_list '}' ';'
{
  INFCHECKFIG();
  inf_AddList(INF_FIG, INF_LOADED_LOCATION, INF_CMPD, "", $3, getwhere());
}
;

crosstalkmux_info        : T_CROSSTALK T_MUTEX .begin. crosstalkmux_list T_END .sc.
;

crosstalkmux_list        : /* rien */ 
| crosstalkmux crosstalkmux_list 
;
crosstalkmux     : T_MUXU '{' crosstalkmux_name_list '}' ';'
{
  INFCHECKFIG();
  inf_AddList(INF_FIG, INF_LOADED_LOCATION, INF_CROSSTALKMUXU, "", $3, getwhere());
}
| T_MUXD '{' crosstalkmux_name_list '}' ';'
{
  INFCHECKFIG();
  inf_AddList(INF_FIG, INF_LOADED_LOCATION, INF_CROSSTALKMUXD, "", $3, getwhere());
}
;

crosstalkmux_name_list   : T_NAME
                  {
                    $$=addchain(NULL, infTasVectName($1));
                  }
                  | T_NAME ',' cmux_name_list 
                  { 
                    $$=append(addchain(NULL, infTasVectName($1)), $3);
                  }
                  ;

cmux_name_list  : T_NAME
                  {
                    $$=addchain(NULL,infTasVectName($1) );
                  }
                  | T_NAME ',' cmux_name_list 
                  { 
                    $$=append(addchain(NULL,infTasVectName($1)) , $3);
                  }
                  ;

mux_name_list   : T_NAME
                  {
                    $$=addchain(NULL, mbk_decodeanyvector($1));
                  }
                  | T_NAME ',' mux_name_list 
                  { 
                    $$=append(addchain(NULL, mbk_decodeanyvector($1)), $3);
                  }
                  ;

renom_info      : T_RENAME .begin. renom_list T_END .sc.
;

renom_list      : /* rien */
| renom_list renom
;

renom           : T_NAME ':' T_NAME  ';'
{
  INFCHECKFIG();
  checkJokName($3,$1);
  inf_AddAssociation(INF_FIG, INF_LOADED_LOCATION, INF_RENAME, "", mbk_decodeanyvector($1), mbk_decodeanyvector($3), 0, 0, getwhere());
}
;

memsym_info      : T_MEMSYM .begin. memsym_list T_END .sc.
;

memsym_list      : /* rien */
| memsym_list memsym
;

memsym           : T_NAME ':' T_NAME  ';'
{
  INFCHECKFIG();
  inf_AddAssociation(INF_FIG, INF_LOADED_LOCATION, INF_MEMSYM, "", mbk_decodeanyvector($1), mbk_decodeanyvector($3), 0, 0, getwhere());
}
;

suppress_info   : T_SUPPRESS .begin. suppress_list T_END .sc.
;

suppress_list   : /* rien */
| suppress suppress_list 
;

suppress        : T_NAME ';'
{
  INFCHECKFIG();
  inf_AddString(INF_FIG, INF_LOADED_LOCATION, mbk_decodeanyvector($1), INF_SUPPRESS, NULL, getwhere());
}
;

inputs_info     : T_INPUTS .begin. inputs_list T_END .sc.
;

inputs_list     : /* rien */
| inputs_list inputs
;

inputs          : T_NAME ';'
{
  INFCHECKFIG();
  inf_AddString(INF_FIG, INF_LOADED_LOCATION, mbk_decodeanyvector($1), INF_INPUTS, NULL, getwhere());
}
;

notlatch_info     : T_NOTLATCH .begin. notlatch_list T_END .sc.
;

notlatch_list     : /* rien */
| notlatch_list notlatch
;

notlatch          : T_NAME ';'
{
  INFCHECKFIG();
  inf_AddString(INF_FIG, INF_LOADED_LOCATION, mbk_decodeanyvector($1), INF_NOTLATCH, NULL, getwhere());
}
;

keeptristate_info     : T_KEEP_TRISTATE_BEHAVIOUR .begin. keeptristate_list T_END .sc.
;

keeptristate_list     : /* rien */
| keeptristate_list keeptristate
;

keeptristate          : T_NAME ';'
{
  INFCHECKFIG();
  inf_AddString(INF_FIG, INF_LOADED_LOCATION, mbk_decodeanyvector($1), INF_KEEP_TRISTATE_BEHAVIOUR, NULL, getwhere());
}
;

cklatch_info    : T_CKLATCH .begin. cklatch_list T_END .sc.
;

cklatch_list    : /* rien */
| cklatch_list cklatch
;

cklatch         : T_NAME ';'
{
  INFCHECKFIG();
  inf_AddString(INF_FIG, INF_LOADED_LOCATION, mbk_decodeanyvector($1), INF_CKLATCH, INF_YES, getwhere());
}
| '~' T_NAME ';'
{
  INFCHECKFIG();
  inf_AddString(INF_FIG, INF_LOADED_LOCATION, mbk_decodeanyvector($2), INF_CKLATCH, INF_NO, getwhere());
}
;

ckprech_info    : T_CKPRECH .begin. ckprech_list T_END .sc.
;

ckprech_list    : /* rien */
| ckprech_list ckprech
;

ckprech         : T_NAME ';'
{
  INFCHECKFIG();
  inf_AddString(INF_FIG, INF_LOADED_LOCATION, mbk_decodeanyvector($1), INF_CKPRECH, NULL, getwhere());
}
;

stop_info   : T_STOP .begin. stop_list T_END .sc.
;

stop_list   : /* rien */
|  stop_list stop
;

stop        : T_NAME ';'
{
  INFCHECKFIG();
  inf_AddString(INF_FIG, INF_LOADED_LOCATION, mbk_decodeanyvector($1), INF_STOP, NULL, getwhere());
}
;

sensitive_info   : T_SENSITIVE .begin. sensitive_list T_END .sc.
;

sensitive_list   : /* rien */
| sensitive sensitive_list
;

sensitive        : T_NAME ';'
{
  INFCHECKFIG();
  inf_AddString(INF_FIG, INF_LOADED_LOCATION, mbk_decodeanyvector($1), INF_SENSITIVE, NULL, getwhere());
}
;

dirout_info      : T_DIROUT .begin. dirout_list T_END .sc.
;

dirout_list      : /* rien */
| dirout_list dirout
;

dirout          : T_NAME ';'
{
  INFCHECKFIG();
  inf_AddInt(INF_FIG, INF_LOADED_LOCATION, mbk_decodeanyvector($1), INF_DIROUT, 0, getwhere());
}
| '~' T_NAME ';'
{
  INFCHECKFIG();
  inf_AddInt(INF_FIG, INF_LOADED_LOCATION, mbk_decodeanyvector($2), INF_DIROUT, -1, getwhere());
}
| T_NAME ':' digit ';'
{
  INFCHECKFIG();
  inf_AddInt(INF_FIG, INF_LOADED_LOCATION, mbk_decodeanyvector($1), INF_DIROUT, $3, getwhere());
}
;

marksig_info      : T_MARKSIG .begin. marksig_list T_END .sc.
;

marksig_list      : /* rien */
| marksig_list marksig
;

marksig          : T_NAME ':' T_NAME';'
{
  INFCHECKFIG();
  inf_AddInt(INF_FIG, INF_LOADED_LOCATION, mbk_decodeanyvector($1), INF_MARKSIG, inf_code_marksig($3), getwhere());
}
;

markRS_info      : T_MARKRS .begin. markRS_list T_END .sc.
;

markRS_list      : /* rien */
| markRS_list markRS
;

markRS          : T_NAME ':' T_NAME';'
{
  int mark;
  INFCHECKFIG();
  mark=inf_code_markRS($3);
  if (mark!=0) inf_AddInt(INF_FIG, INF_LOADED_LOCATION, mbk_decodeanyvector($1), INF_MARKRS, mark, getwhere());
  else avt_errmsg(INF_ERRMSG, "025", AVT_WARNING, getwhere(), $3);
}
;


marktrans_info      : T_MARKTRANS .begin. marktrans_list T_END .sc.
;

marktrans_list      : /* rien */
| marktrans_list marktrans
;

marktrans          : T_NAME ':' T_NAME';'
{
  INFCHECKFIG();
  inf_AddInt(INF_FIG, INF_LOADED_LOCATION, mbk_decodeanyvector($1), INF_MARKTRANS, inf_code_marktrans($3), getwhere());
}
;

dlatch_info      : T_DLATCH .begin. dlatch_list T_END .sc.
;

dlatch_list      : /* rien */
| dlatch_list dlatch
;

dlatch          : T_NAME ';'
{
  INFCHECKFIG();
  inf_AddString(INF_FIG, INF_LOADED_LOCATION, mbk_decodeanyvector($1), INF_DLATCH, INF_YES, getwhere());
}
| '~' T_NAME ';'
{
  INFCHECKFIG();
  inf_AddString(INF_FIG, INF_LOADED_LOCATION, mbk_decodeanyvector($2), INF_DLATCH, INF_NO, getwhere());
}
;

flipflop_info      : T_FLIPFLOP .begin. flipflop_list T_END .sc.
;

flipflop_list      : /* rien */
| flipflop_list flipflop
;

flipflop          : T_NAME ';'
{
  INFCHECKFIG();
  inf_AddString(INF_FIG, INF_LOADED_LOCATION, infTasVectName($1), INF_FLIPFLOP, NULL, getwhere());
}
;

digit           : T_VAL
{
  $$=mbk_long_round($1);
}
;

collage_info    : T_CONSTRAINT .begin. collage_list T_END .sc.
;

collage_list    : /* rien */
| collage_list collage
;

collage         : T_NAME ':' valeur ';'
{
  INFCHECKFIG();
  inf_AddInt(INF_FIG, INF_LOADED_LOCATION, mbk_decodeanyvector($1), INF_STUCK, $3==INF_LL_STUCKVSS?0:1, getwhere());
}
;

valeur          : T_VAL
{
  if ($1 == 0)
    $$ = INF_LL_STUCKVSS ;
  else
    if ($1 == 1)
      $$ = INF_LL_STUCKVDD;
    else 
      yyerror() ;
}
;

prech_info      : T_PRECHARGE .begin. prech_list T_END .sc.
;

prech_list      : /* rien */
| precharge prech_list
;

precharge       : T_NAME ';'
{
  INFCHECKFIG();
  inf_AddString(INF_FIG, INF_LOADED_LOCATION, infTasVectName($1), INF_PRECHARGE, INF_YES, getwhere());
}
| '~' T_NAME ';'
{
  INFCHECKFIG();
  inf_AddString(INF_FIG, INF_LOADED_LOCATION, infTasVectName($2), INF_PRECHARGE, INF_NO, getwhere());
}
;

modloop_info      : T_MODELLOOP .begin. modloop_list T_END .sc.
;

modloop_list      : /* rien */
| modloop modloop_list
;

modloop       : T_NAME ';'
{
  INFCHECKFIG();
  inf_AddString(INF_FIG, INF_LOADED_LOCATION, infTasVectName($1), INF_MODELLOOP, INF_YES, getwhere());
}
| '~' T_NAME ';'
{
  INFCHECKFIG();
  inf_AddString(INF_FIG, INF_LOADED_LOCATION, infTasVectName($2), INF_MODELLOOP, INF_NO, getwhere());
}
;

asynchron_info  : T_ASYNCHRON .begin. asynchron_list T_END .sc.
;

asynchron_list  : /* rien */
| asynchron_list asynchron
;

asynchron       : T_NAME ';'
{
  INFCHECKFIG();
  inf_AddString(INF_FIG, INF_LOADED_LOCATION, infTasVectName($1), INF_ASYNCHRON, NULL, getwhere());
}
;

dontcross_info : T_DONTCROSS .begin. dontcross_list T_END .sc.
;

dontcross_list : 
| dontcross_list dontcross
;

dontcross : T_NAME ';'
{
  INFCHECKFIG();
  inf_AddString(INF_FIG, INF_LOADED_LOCATION, infTasVectName($1), INF_DONTCROSS, NULL, getwhere());
}
;

transparent_info : T_TRANSPARENT .begin. transparent_list T_END .sc.
;

transparent_list : 
| transparent_list transparent
;

transparent : T_NAME ';'
{
  INFCHECKFIG();
  inf_AddString(INF_FIG, INF_LOADED_LOCATION, infTasVectName($1), INF_TRANSPARENT, NULL, getwhere());
}
;

clock_info      : T_CLOCK .begin. clock_list T_END .sc.
;

clock_list      : /* rien */
| clock_list clock
;

.virtual. : T_VIRTUAL
{
  $$=INF_CLOCK_VIRTUAL;
}
;

.ideal. : T_IDEAL
{
  $$=INF_CLOCK_IDEAL;
}
;

clock           : T_NAME clockdetail_list ';'
{
  ptype_list *pt;
  char *aname;

  INFCHECKFIG();

  aname=infTasVectName($1);

  inf_AddInt(INF_FIG, INF_LOADED_LOCATION, aname, INF_CLOCK_TYPE, 0, getwhere());
  if ($2!=NULL)
    {
      if ((pt=getptype($2, INF_UP))!=NULL)
        {
          inf_AddDouble(INF_FIG, INF_LOADED_LOCATION, aname, INF_MIN_RISE_TIME, todoublesec(pt->DATA), getwhere());
          inf_AddDouble(INF_FIG, INF_LOADED_LOCATION, aname, INF_MAX_RISE_TIME, todoublesec(pt->DATA), getwhere());
        }
      if ((pt=getptype($2, INF_DOWN))!=NULL)
        {
          inf_AddDouble(INF_FIG, INF_LOADED_LOCATION, aname, INF_MIN_FALL_TIME, todoublesec(pt->DATA), getwhere());
          inf_AddDouble(INF_FIG, INF_LOADED_LOCATION, aname, INF_MAX_FALL_TIME, todoublesec(pt->DATA), getwhere());
        }
      if ((pt=getptype($2, INF_PERIOD))!=NULL)
        {
          inf_AddDouble(INF_FIG, INF_LOADED_LOCATION, aname, INF_CLOCK_PERIOD, todoublesec(pt->DATA), getwhere());
        }
      freeptype($2);
    }
}
;
                 
clockdetail_list: /* empty */
{
  $$ = NULL;
}
| ':' T_RISE '=' T_VAL      T_FALL '=' T_VAL    T_PERIOD '=' T_VAL
{
  ptype_list *ptype;
  ptype = addptype (NULL, INF_UP, (void*)((long)$4));
  ptype = addptype (ptype, INF_DOWN, (void*)((long)$7));
  ptype = addptype (ptype, INF_PERIOD, (void*)((long)$10));
  $$ = ptype;
}
| ':' T_RISE '=' T_VAL      T_PERIOD '=' T_VAL  T_FALL '=' T_VAL 
{
  ptype_list *ptype;
  ptype = addptype (NULL, INF_UP, (void*)((long)$4));
  ptype = addptype (ptype, INF_DOWN, (void*)((long)$10));
  ptype = addptype (ptype, INF_PERIOD, (void*)((long)$7));
  $$ = ptype;
}
| ':' T_FALL '=' T_VAL      T_RISE '=' T_VAL    T_PERIOD '=' T_VAL
{
  ptype_list *ptype;
  ptype = addptype (NULL, INF_UP, (void*)((long)$7));
  ptype = addptype (ptype, INF_DOWN, (void*)((long)$4));
  ptype = addptype (ptype, INF_PERIOD, (void*)((long)$10));
  $$ = ptype;
}
| ':' T_FALL '=' T_VAL      T_PERIOD '=' T_VAL  T_RISE '=' T_VAL
{
  ptype_list *ptype;
  ptype = addptype (NULL, INF_UP, (void*)((long)$10));
  ptype = addptype (ptype, INF_DOWN, (void*)((long)$4));
  ptype = addptype (ptype, INF_PERIOD, (void*)((long)$7));
  $$ = ptype;
}
| ':' T_PERIOD '=' T_VAL    T_RISE '=' T_VAL    T_FALL '=' T_VAL 
{
  ptype_list *ptype;
  ptype = addptype (NULL, INF_UP, (void*)((long)$7));
  ptype = addptype (ptype, INF_DOWN, (void*)((long)$10));
  ptype = addptype (ptype, INF_PERIOD, (void*)((long)$4));
  $$ = ptype;
}
| ':' T_PERIOD '=' T_VAL    T_FALL '=' T_VAL    T_RISE '=' T_VAL
{
  ptype_list *ptype;
  ptype = addptype (NULL, INF_UP, (void*)((long)$10));
  ptype = addptype (ptype, INF_DOWN, (void*)((long)$7));
  ptype = addptype (ptype, INF_PERIOD, (void*)((long)$4));
  $$ = ptype;
}
;

opcond_info     : T_OPERATING T_CONDITION .begin. opcond_list T_END .sc.
;

opcond_list     : /* empty */
| opcond_list opcond
;

opcond          : T_TEMP '=' T_VAL ';'
{
  INFCHECKFIG();
  inf_AddDouble(INF_FIG, INF_LOADED_LOCATION, INF_OPERATING_CONDITION, INF_TEMPERATURE,$3,getwhere());
}
| T_SUPPLY ':' T_NAME '=' T_VAL ';'
{
  INFCHECKFIG();
  inf_AddDouble(INF_FIG, INF_LOADED_LOCATION, mbk_decodeanyvector($3), INF_POWER, $5,getwhere());
}
;

bypass_info     : T_BYPASS .begin. bypass_list T_END .sc.
;

bypass_list     : /* rien */
| bypass_list bypass
;

bypass          : identifier ';'
{
  INFCHECKFIG();
  inf_AddString(INF_FIG, INF_LOADED_LOCATION, infTasVectName($1), INF_BYPASS, INF_ALL, getwhere());
}
| identifier '<' ';'
{
  INFCHECKFIG();
  /* les chemins arrivant sur ce noeud sont ignores */
  inf_AddString(INF_FIG, INF_LOADED_LOCATION, infTasVectName($1), INF_BYPASS, INF_IN, getwhere());
}
| identifier '>' ';'
{
  INFCHECKFIG();
  /* les chemins partant de ce noeud sont ignores */
  inf_AddString(INF_FIG, INF_LOADED_LOCATION, infTasVectName($1), INF_BYPASS, INF_OUT, getwhere());
}
| identifier '!' ';'
{
  INFCHECKFIG();
  /* les chemins partant de ce connecteurs sont ignores */
  inf_AddString(INF_FIG, INF_LOADED_LOCATION, infTasVectName($1), INF_BYPASS, INF_ONLYEND, getwhere());
}
;

identifier      : T_NAME
{
  strcpy($$, $1);
}
| T_VAL
{
  sprintf ($$, "%g", $1);
}
;

inter_info  : T_INTER .begin. inter_list T_END .sc.
;

inter_list  : /* rien */
| inter_list inter
;

inter       : T_NAME ';'
{
  INFCHECKFIG();
  inf_AddString(INF_FIG, INF_LOADED_LOCATION, infTasVectName($1), INF_INTER, NULL, getwhere());
}
;
strictsetup_info  : T_STRICT_SETUP T_BEGIN strictsetup_list T_END ';'
;

strictsetup_list  : /* rien */
| strictsetup_list strictsetup
;

strictsetup       : T_NAME ';'
{
  INFCHECKFIG();
  inf_AddString(INF_FIG, INF_LOADED_LOCATION, infTasVectName($1), INF_STRICT_SETUP, NULL, getwhere());
}
;

break_info  : T_BREAK .begin. break_list T_END .sc.
;

break_list  : /* rien */
| break_list break
;

break       : T_NAME ';'
{
  INFCHECKFIG();
  inf_AddString(INF_FIG, INF_LOADED_LOCATION, infTasVectName($1), INF_BREAK, NULL, getwhere());
}
;

pathin_info : T_PATHIN .begin. pathin_list T_END .sc.
;

pathin_list : /* rien */
| pathin_list pathin
;

pathin      : T_NAME ';'
{
  INFCHECKFIG();
  inf_AddString(INF_FIG, INF_LOADED_LOCATION, mbk_decodeanyvector($1), INF_PATHIN, NULL,getwhere());
}
;

pathout_info    : T_PATHOUT .begin. pathout_list T_END .sc.
;

pathout_list:   /* rien */
| pathout_list pathout
;

pathout     : T_NAME ';'
{
  INFCHECKFIG();
  inf_AddString(INF_FIG, INF_LOADED_LOCATION, mbk_decodeanyvector($1), INF_PATHOUT, NULL,getwhere());
}
;

pathsigs_info   : T_PATHSIGS .begin. pathsigs_list T_END .sc.
;

pathsigs_list:  /* rien */
| pathsigs_list pathsig
;

pathsig     : T_NAME ';'
{
  INFCHECKFIG();
  infAddList(INF_FIG, INF_LOADED_LOCATION,infTasVectName($1),INF_LL_PATHSIGS,'d',NULL,getwhere()) ;
}
| T_NAME updown ';'
{
  ptype_list *ptype;
  INFCHECKFIG();
  ptype = addptype(NULL,$2,infTasVectName($1)) ;
  infAddList(INF_FIG, INF_LOADED_LOCATION,infTasVectName($1),INF_LL_PATHSIGS,'d',ptype,getwhere()) ;
}
;

rc_info     : T_RC .begin. rc_list T_END .sc.
;

rc_list:    /* rien */
| rc_list rc
;

rc      : T_NAME ';'
{
  INFCHECKFIG();
  inf_AddString(INF_FIG, INF_LOADED_LOCATION, infTasVectName($1), INF_RC, INF_YES, getwhere());
}
;

norc_info   : T_NORC .begin. norc_list T_END .sc.
;

norc_list:  /* rien */
| norc_list norc
;

norc        : T_NAME ';'
{
  INFCHECKFIG();
  inf_AddString(INF_FIG, INF_LOADED_LOCATION, infTasVectName($1), INF_RC, INF_NO, getwhere());
}
;

norising_info    : T_NORISING .begin. norising_list T_END .sc.
;

norising_list:   /* rien */
| norising_list norising
;

norising     : T_NAME ';'
{
  INFCHECKFIG();
  inf_AddString(INF_FIG, INF_LOADED_LOCATION, infTasVectName($1), INF_NORISING, NULL, getwhere());
}
;

nofalling_info    : T_NOFALLING .begin. nofalling_list T_END .sc.
;

nofalling_list:   /* rien */
| nofalling_list nofalling
;

nofalling     : T_NAME ';'
{
  INFCHECKFIG();
  inf_AddString(INF_FIG, INF_LOADED_LOCATION, infTasVectName($1), INF_NOFALLING, NULL, getwhere());
}
;

falsepath_info  : T_FALSEPATH .begin. falsepath_list T_END .sc.
;

falsepath_list  : /* rien */
| falsepath_list falsepath
;

falsepath       : falsename_list ';'
{
  ptype_list *pt=$1;  
  while (pt!=NULL && pt->TYPE==INF_NOTHING) pt=delptype(pt, INF_NOTHING);
  pt=(ptype_list *)reverse((chain_list *)pt);
  while (pt!=NULL && pt->TYPE==INF_NOTHING) pt=delptype(pt, INF_NOTHING);
  pt=(ptype_list *)reverse((chain_list *)pt);
  $1=pt;

  if($1 == NULL)
    {
      yyerror() ;
    }
  if($1->NEXT == NULL)
    {
      freeptype($1) ;
      yyerror() ;
    }
  $1=finish_falsepath($1);
  INFCHECKFIG();
  INF_FIG->LOADED.INF_FALSEPATH=addchain(INF_FIG->LOADED.INF_FALSEPATH,reverse((chain_list*)$1));
}
| falsename_list ':' ck ';'
{

  if($1 == NULL)
    {
      yyerror() ;
    }
  if($1->NEXT == NULL)
    {
      freeptype($1) ;
      yyerror() ;
    }

  $1=finish_falsepath($1);
  INFCHECKFIG();

  $3->NEXT = $1;
  INF_FIG->LOADED.INF_FALSEPATH=addchain(INF_FIG->LOADED.INF_FALSEPATH,(void*)reverse((chain_list*)$3));
}
;

ck             : T_NAME
{
  $$ = addptype(NULL,INF_CK,infTasVectName($1)) ;
}
| '<' T_NAME '>'
{
  if (strcasecmp($2,"HZ")==0)
    $$ = addptype(NULL,INF_CK,infTasVectName(INF_HZ_STR)) ;
  else if (strcasecmp($2,"NOTHZ")==0)
    $$ = addptype(NULL,INF_CK,infTasVectName(INF_NOTHZ_STR)) ;
  else
    $$ = addptype(NULL,INF_CK,infTasVectName($2)) ; // error?
}
;

falsename_list  : /* rien */
{
  $$ = NULL ;
}
| falsename_list falsename
{
  ptype_list *ptype ;
  ptype = $2 ;
  ptype->NEXT = $1 ;
  $$ = ptype;
}
;

falsename       : T_NAME
{
  $$ = addptype(NULL,INF_UPDOWN,infTasVectName($1)) ;
}
| updown
{
  $$ = addptype(NULL,$1,NULL) ;
}
| '<' T_VOID '>'
{
  $$ = addptype(NULL,INF_NOTHING,NULL) ;
}
;

updown          : '<' T_UP '>'
{
  $$ = INF_UP ;
}
| '<' T_DOWN '>'
{
  $$ = INF_DOWN ;
}
| '<' T_RISE '>'
{
  $$ = INF_UP ;
}
| '<' T_FALL '>'
{
  $$ = INF_DOWN ;
}
;

.updownfalsehztag. :
updown falsehztag
{
 $$=$1|$2;
}
|
falsehztag
{
 $$=$1;
}
|
updown
{
 $$=$1;
}
|
{
 $$=INF_FALSESLACK_HZ|INF_FALSESLACK_NOTHZ|INF_FALSESLACK_UP|INF_FALSESLACK_DOWN;
}
;
 
falseslack_info  : T_FALSESLACK T_BEGIN falseslack_list T_END ';'
;

falseslack_list  : /* rien */
| falseslack_list falseslack
;

.falseslack_type.: ':' T_NAME 
{
  char buf[1024], *tok, *c;
  char val=0;
  strcpy(buf, $2);
  tok=strtok_r(buf, "-", &c);
  while (tok!=NULL)
    {
      if (strcasecmp(tok, "setup")==0) val|=INF_FALSESLACK_SETUP;
      else if (strcasecmp(tok, "hold")==0) val|=INF_FALSESLACK_HOLD;
      else if (strcasecmp(tok, "latch")==0) val|=INF_FALSESLACK_LATCH;
      else if (strcasecmp(tok, "prech")==0) val|=INF_FALSESLACK_PRECH;
      else 
        {
          avt_errmsg(INF_ERRMSG, "002", AVT_ERROR, getwhere(), "'setup', 'hold' or 'latch'");
        }
      tok=strtok_r(NULL, " ", &c);
    }
  if ((val & (INF_FALSESLACK_SETUP|INF_FALSESLACK_HOLD))==0)
    val|=INF_FALSESLACK_SETUP|INF_FALSESLACK_HOLD;
  $$=val;
}
|
{
 $$=INF_FALSESLACK_SETUP|INF_FALSESLACK_HOLD;
}
;

falseslack       : T_NAME .updown. T_NAME .updown. T_NAME .updownfalsehztag. T_NAME .updown. .falseslack_type. ';'
{
  ptype_list *pt;
  INFCHECKFIG();
  pt=addptype(NULL, $8, infTasVectName($7));
  pt=addptype(pt, $6, infTasVectName($5));
  pt=addptype(pt, $4, infTasVectName($3));
  pt=addptype(pt, $2|$9, infTasVectName($1));
  INF_FIG->LOADED.INF_FALSESLACK=addchain(INF_FIG->LOADED.INF_FALSESLACK,pt);
}
;

falsehztag : '<' T_NAME '>'
{
  if (strcasecmp($2,"HZ")==0)
    $$ = INF_FALSESLACK_HZ ;
  else if (strcasecmp($2,"NOTHZ")==0)
    $$ = INF_FALSESLACK_NOTHZ;
  else
    $$ = 0 ; // error?
}
;

.updown.: updown
{
  $$=$1;
}
|
{
  $$=INF_FALSESLACK_UP|INF_FALSESLACK_DOWN;
}
;

delay_info      : T_DELAY .begin. delay_list T_END .sc.
;

delay_list      : 
| delay_list delay
;

delay           : T_NAME ':' T_NAME ':' time_value ';'
{
  INFCHECKFIG();
  inf_AddAssociation(INF_FIG, INF_LOADED_LOCATION,  INF_DELAY, "", mbk_decodeanyvector($1), mbk_decodeanyvector($3), 0, $5, getwhere());
}
;

pinslew_info    : T_PINSLEW .begin. pinslew_list T_END .sc.
;

pinslew_list    : /* rien */
| pinslew_list pinslew
{
  INFCHECKFIG();
  if (strcasecmp($2.nom,"default")==0) $2.nom=namealloc("*");
  if ($2.val1>=0 && $2.val1<1e-12)
  {
    $2.val1=1e-12;
    avt_errmsg(INF_ERRMSG, "019", AVT_WARNING, getwhere(), $2.nom, "rising", "1ps");
  }
  if ($2.val2>=0 && $2.val2<1e-12)
  {
    $2.val2=1e-12;
    avt_errmsg(INF_ERRMSG, "019", AVT_WARNING, getwhere(), $2.nom, "falling", "1ps");
  }
  inf_AddDouble(INF_FIG, INF_LOADED_LOCATION, $2.nom, INF_PIN_RISING_SLEW, $2.val1, getwhere());
  inf_AddDouble(INF_FIG, INF_LOADED_LOCATION, $2.nom, INF_PIN_FALLING_SLEW, $2.val2, getwhere());
  INF_FIG->changed|=INF_CHANGED_RECOMP;
  if ($2.val3!=-DBL_MAX)
    {
      if ($2.val3!=0) 
        inf_AddDouble(INF_FIG, INF_LOADED_LOCATION, $2.nom, INF_PIN_LOW_VOLTAGE, $2.val3, getwhere());
      inf_AddDouble(INF_FIG, INF_LOADED_LOCATION, $2.nom, INF_PIN_HIGH_VOLTAGE, $2.val4, getwhere());
    }
  mbkfree(carac);
}
;

pinslew         : pin ':' .pinsupply. T_RISE '=' time_value T_FALL '=' time_value ';'
{
  $$.nom=$1;
  $$.val1=$6;
  $$.val2=$9;
  $$.val3=$3.val1;
  $$.val4=$3.val2;
}
| pin ':'  .pinsupply. T_FALL '=' time_value T_RISE '=' time_value ';'
{
  $$.nom=$1;
  $$.val1=$9;
  $$.val2=$6;
  $$.val3=$3.val1;
  $$.val4=$3.val2;
}
| pin ':' .pinsupply. time_value ';'
{
  $$.nom=$1;
  $$.val1=$4;
  $$.val2=$4;
  $$.val3=$3.val1;
  $$.val4=$3.val2;
}
;

.pinsupply. : T_SUPPLY '=' T_VAL
{
  $$.val1=0;
  $$.val2=$3;
}
| T_SUPPLY '=' '(' T_VAL ',' T_VAL ')'
{
  $$.val1=$4;
  $$.val2=$6;
}
|
{
  $$.val1=-DBL_MAX;
  $$.val2=-DBL_MAX;
}
;

slopein_info    : T_SLOPEIN .begin. slope_list T_END .sc.
{
}
;

slope_list      : /* rien */
| slope_list slope
;

slope           : T_NAME ':' time_values ';'
{
  char *name;
  chain_list *cl;

  INFCHECKFIG();

  name=infTasVectName ($1);
  inf_RemoveKey(INF_FIG, INF_LOADED_LOCATION, name, INF_SLOPEIN);  

  for (cl=$3; cl!=NULL; cl=cl->NEXT)
    {
      inf_AddAssociation(INF_FIG, INF_LOADED_LOCATION, name, INF_SLOPEIN, NULL, NULL, 0, *(double *)cl->DATA, getwhere());
      mbkfree(cl->DATA);
    }
  freechain($3);
}
;

capaout_info    : T_CAPAOUT .begin. capa_list T_END .sc.
{

}
;

capa_list       : /* rien */
| capa_list capa
{

}
;
				
capa            : T_NAME ':' capa_values ';'
{
  char *name;
  chain_list *cl;

  INFCHECKFIG();
  name=infTasVectName ($1);
  inf_RemoveKey(INF_FIG, INF_LOADED_LOCATION, name, INF_CAPAOUT);  

  for (cl=$3; cl!=NULL; cl=cl->NEXT)
    {
      inf_AddAssociation(INF_FIG, INF_LOADED_LOCATION, name, INF_CAPAOUT, NULL, NULL, 0, *(double *)cl->DATA, getwhere());
      mbkfree(cl->DATA);
    }
  freechain($3);
  INF_FIG->changed|=INF_CHANGED_RECOMP;
}
| T_NAME ':' capa_values T_NAME '=' T_VAL T_NAME '=' T_VAL ';'
{
  char *name;
  chain_list *cl;
  double l=-1, w=-1;

  INFCHECKFIG();

  name=infTasVectName ($1);
  
  for (cl=$3; cl!=NULL; cl=cl->NEXT)
    {
      inf_AddAssociation(INF_FIG, INF_LOADED_LOCATION, name, INF_CAPAOUT, NULL, NULL, 0, *(double *)cl->DATA, getwhere());
      mbkfree(cl->DATA);
    }
  freechain($3);
  
  if (strcasecmp($4,"l")==0) l=$6; else if (strcasecmp($4,"w")==0) w=$6;
  if (strcasecmp($7,"l")==0) l=$9; else if (strcasecmp($7,"w")==0) w=$9;

  if (l==-1 || w==-1)
    {
      avt_errmsg(INF_ERRMSG, "001", AVT_ERROR, getwhere());
    }
  else
    {
      inf_AddDouble(INF_FIG, INF_LOADED_LOCATION, name, INF_CAPAOUT_LW, l / w, getwhere());
      inf_AddDouble(INF_FIG, INF_LOADED_LOCATION, name, INF_CAPAOUT_L, l , getwhere());
    }
}
;
				
pin             : T_NAME
{
  carac = (inf_carac*)mbkalloc (sizeof (struct inf_carac)) ;
  nbval = 0 ;
  if (!valbuf)
    valbuf = (double*)mbkalloc (TBL_BUFSIZE * sizeof (double)) ;
  for (i = 0 ; i < TBL_BUFSIZE ; i++)
    valbuf[i] = 0 ;
  $$ = infTasVectName ($1) ;
}
| T_DEFAULT
{
  carac = (inf_carac*)mbkalloc (sizeof (struct inf_carac)) ;
  nbval = 0 ;
  if (!valbuf)
    valbuf = (double*)mbkalloc (TBL_BUFSIZE * sizeof (double)) ;
  for (i = 0 ; i < TBL_BUFSIZE ; i++)
    valbuf[i] = 0 ;
  $$ = namealloc ("default") ;
}
;

time_values          : '(' time_val_list ')'
{
  $$ = $2;
}
| '(' time_value ':' time_value ':' time_value ')'
{
  double *d;
  double step;

  $$=NULL;
  step = $6 ;
  
  for (i = 0 ; i <= ($4 - $2+1e-18) / step ; i++) 
    {
      d=(double *)mbkalloc(sizeof(double));
      *d=$2 + step * i;
      $$=addchain($$, d);
    }
  $$=reverse($$);
}
;

time_val_list        : time_value
{
  double *d;
  d=(double *)mbkalloc(sizeof(double));
  *d=$1;
  $$=addchain(NULL, d);
}
| time_val_list ',' time_value
{
  double *d;
  d=(double *)mbkalloc(sizeof(double));
  *d=$3;
  $$=append($1, addchain(NULL, d));
}
;

capa_values          : '(' capa_val_list ')'
{
  $$ = $2;
}
| '(' capa_value ':' capa_value ':' capa_value ')'
{
  double *d;
  double step;

  $$=NULL;
  step = $6 ;
  
  for (i = 0 ; i <= ($4 - $2+1e-18) / step ; i++) 
    {
      d=(double *)mbkalloc(sizeof(double));
      *d=$2 + step * i;
      $$=addchain($$, d);
    }
  $$=reverse($$);
}
;

capa_val_list        : capa_value
{
  double *d;
  d=(double *)mbkalloc(sizeof(double));
  *d=$1;
  $$=addchain(NULL, d);
}
| capa_val_list ',' capa_value
{
  double *d;
  d=(double *)mbkalloc(sizeof(double));
  *d=$3;
  $$=append($1, addchain(NULL, d));
}
;


path_delay_margin_info : T_PATH T_DELAY T_MARGIN .begin. .path_delay_margin_list. T_END .sc. 
;

.path_delay_margin_list.: path_delay_margin .path_delay_margin_list.
|
;

path_delay_margin : pathsigtype T_NAME ':' T_NAME '=' T_VAL T_NAME '=' time_value .pathtag. ';'
{
  int deft, i, j, k;
  struct
  {
    int t;
    char *s;
  } run[]=
    {
      {INF_MARGIN_ON_MINDELAY, INF_PATHDELAYMARGINMIN},
      {INF_MARGIN_ON_MAXDELAY, INF_PATHDELAYMARGINMAX},
      {INF_MARGIN_ON_CLOCKPATH, INF_PATHDELAYMARGINCLOCK},
      {INF_MARGIN_ON_DATAPATH, INF_PATHDELAYMARGINDATA},
      {INF_MARGIN_ON_RISEDELAY, INF_PATHDELAYMARGINRISE},
      {INF_MARGIN_ON_FALLDELAY, INF_PATHDELAYMARGINFALL}
    };
  char section[128];

  if (($10 & 0x800000)==0) //erreur?
  {
    if (strcasecmp($4,"factor")!=0 && strcasecmp($4,"f")!=0)
        avt_errmsg(INF_ERRMSG, "002", AVT_ERROR, getwhere(), "'factor' or 'f'");
    if (strcasecmp($7,"delta")!=0 && strcasecmp($7,"d")!=0)
        avt_errmsg(INF_ERRMSG, "002", AVT_ERROR, getwhere(), "'delta' or 'd'");
    deft=$10;

    if ((deft & (INF_MARGIN_ON_MINDELAY|INF_MARGIN_ON_MAXDELAY))==0)
      deft|=INF_MARGIN_ON_MINDELAY|INF_MARGIN_ON_MAXDELAY;
    if ((deft & (INF_MARGIN_ON_CLOCKPATH|INF_MARGIN_ON_DATAPATH))==0)
      deft|=INF_MARGIN_ON_CLOCKPATH|INF_MARGIN_ON_DATAPATH;
    if ((deft & (INF_MARGIN_ON_RISEDELAY|INF_MARGIN_ON_FALLDELAY))==0)
      deft|=INF_MARGIN_ON_RISEDELAY|INF_MARGIN_ON_FALLDELAY;

    INFCHECKFIG();
    
    for (i=0; i<2; i++)
      for (j=2; j<4; j++)
        for (k=4; k<6; k++)
          {
            if ((deft & (run[i].t|run[j].t|run[k].t))==(run[i].t|run[j].t|run[k].t))
              {
                sprintf(section,INF_PATHDELAYMARGINPREFIX"|%s,%s,%s",run[i].s, run[j].s, run[k].s);
                inf_AddMiscData(INF_FIG, INF_LOADED_LOCATION, infTasVectName($2), section, NULL, NULL, $1|$10, $6, $9, getwhere());
              }

          }
  }
}
;

pathsigtype: T_NAME
{
  char *type=$1;
  if (strcasecmp(type,"any")==0) $$=INF_MARGIN_ON_LATCH|INF_MARGIN_ON_BREAK|INF_MARGIN_ON_CONNECTOR|INF_MARGIN_ON_PRECHARGE|INF_MARGIN_ON_CMD;
  else if (strcasecmp(type,"latch")==0) $$=INF_MARGIN_ON_LATCH;
  else if (strcasecmp(type,"flipflop")==0) $$=INF_MARGIN_ON_FLIPFLOP;
  else if (strcasecmp(type,"prech")==0) $$=INF_MARGIN_ON_PRECHARGE;
  else if (strcasecmp(type,"break")==0) $$=INF_MARGIN_ON_BREAK;
  else if (strcasecmp(type,"cmd")==0) $$=INF_MARGIN_ON_CMD;
  else if (strcasecmp(type,"con")==0) $$=INF_MARGIN_ON_CONNECTOR;
  else 
    {
      avt_errmsg(INF_ERRMSG, "003", AVT_ERROR, getwhere(), type);
      $$=0;
    }
}
;


path_tag_item:
T_NAME
{
  strcpy($$, $1);
}
|
T_RISE
{
  strcpy($$, "rise");
}
|
T_FALL
{
  strcpy($$, "fall");
}
;

.pathtag.: path_tag_item .pathtag.
{
  if (strcasecmp($1,"min")==0) $$=INF_MARGIN_ON_MINDELAY|$2;
  else if (strcasecmp($1,"max")==0) $$=INF_MARGIN_ON_MAXDELAY|$2;
  else if (strcasecmp($1,"rise")==0) $$=INF_MARGIN_ON_RISEDELAY|$2;
  else if (strcasecmp($1,"fall")==0) $$=INF_MARGIN_ON_FALLDELAY|$2;
  else if (strcasecmp($1,"clockpath")==0) $$=INF_MARGIN_ON_CLOCKPATH|$2;
  else if (strcasecmp($1,"datapath")==0) $$=INF_MARGIN_ON_DATAPATH|$2;
  else
    {
      avt_errmsg(INF_ERRMSG, "003", AVT_ERROR, getwhere(), $1);
      $$=0x800000|$2;
    }
}
|
{
  $$=0;
}
;


multicycle_info: T_MULTICYCLE T_PATH .begin. multicycle_info_list  T_END .sc. ;


multicycle_info_list: multicycle_info_list multicycle
|
;

multicycle: .multicyclesetuphold. .multicycledatadir. T_AFTER T_VAL .multicyclestartend. .multicyclecycle.  .multicyclefrom. ';' 
{
  INFCHECKFIG();
  inf_AddAssociation(INF_FIG, INF_LOADED_LOCATION, INF_MULTICYCLE_PATH, "", $7, $2.nom, $2.edgetype|$1|$5, $4, getwhere());
}
;

.multicyclesetuphold. : T_FOR T_NAME
{
  if (strcasecmp($2,"setup")==0) $$=INF_MULTICYCLE_SETUP;
  else if (strcasecmp($2,"hold")==0) $$=INF_MULTICYCLE_HOLD;
  else 
    {
      $$=INF_MULTICYCLE_SETUP|INF_MULTICYCLE_HOLD;
      avt_errmsg(INF_ERRMSG, "004", AVT_ERROR, getwhere(), "'setup' or 'hold'");
    }
}
|
{
  $$=INF_MULTICYCLE_SETUP;
}
;

.multicyclecycle. : T_CYCLE
|
;

.multicyclestartend. : T_START
{
  $$=INF_MULTICYCLE_START;
}
|
T_END
{
  $$=INF_MULTICYCLE_END;
}
|
{
  $$=INF_MULTICYCLE_END;
}
;
.multicyclefrom. : T_FROM T_NAME
{
  $$=infTasVectName($2);
}
| 
{
  $$=infTasVectName("*");
}
;

.multicycledatarisefall. : T_RISING
{
  $$=INF_MULTICYCLE_RISE;
}
|
T_FALLING
{
  $$=INF_MULTICYCLE_FALL;
}
|
{
  $$=INF_MULTICYCLE_RISE|INF_MULTICYCLE_FALL;
}
;

.multicycledatadir. : T_NAME .multicycledatarisefall.
{
  $$.nom=infTasVectName($1);
  $$.edgetype=$2;
}
|
{
  $$.nom=infTasVectName("*");
  $$.edgetype=INF_MULTICYCLE_RISE|INF_MULTICYCLE_FALL;
}
;

clockuncertainty_info: T_CLOCKUNCERTAINTY .begin. clockuncertainty_info_list  T_END .sc. ;


clockuncertainty_info_list: clockuncertainty_info_list clockuncertainty
|
;

clockuncertainty: .clockuncertaintysetuphold. T_FROM T_NAME .clockuncertaintyrisefall. T_TO T_NAME .clockuncertaintyrisefall. ':' time_value ';' 
{
  long sd,ed,sh;
  INFCHECKFIG();
  sh=$1;
  if ($4==1) sd=INF_CLOCK_UNCERTAINTY_START_RISE;
  else if ($4==0) sd=INF_CLOCK_UNCERTAINTY_START_FALL;
  else sd=INF_CLOCK_UNCERTAINTY_START_RISE|INF_CLOCK_UNCERTAINTY_START_FALL;
  if ($7==1) ed=INF_CLOCK_UNCERTAINTY_END_RISE;
  else if ($7==0) ed=INF_CLOCK_UNCERTAINTY_END_FALL;
  else ed=INF_CLOCK_UNCERTAINTY_END_RISE|INF_CLOCK_UNCERTAINTY_END_FALL;
  inf_AddAssociation(INF_FIG, INF_LOADED_LOCATION, INF_CLOCK_UNCERTAINTY, "", infTasVectName($3), infTasVectName($6), sd|ed|sh, $9, getwhere());
}
;

.clockuncertaintysetuphold. : T_FOR T_NAME
{
  if (strcasecmp($2,"setup")==0) $$=INF_CLOCK_UNCERTAINTY_SETUP;
  else if (strcasecmp($2,"hold")==0) $$=INF_MULTICYCLE_HOLD;
  else 
    {
      $$=INF_CLOCK_UNCERTAINTY_SETUP|INF_CLOCK_UNCERTAINTY_HOLD;
      avt_errmsg(INF_ERRMSG, "004", AVT_ERROR, getwhere(), "'setup' or 'hold'");
    }
}
|
{
  $$=INF_CLOCK_UNCERTAINTY_SETUP|INF_CLOCK_UNCERTAINTY_HOLD;
}
;

.clockuncertaintyrisefall. : T_RISING
{
  $$=1;
}
|
T_FALLING
{
  $$=0;
}
|
{
  $$=-1;
}
;

ignore_info: T_IGNORE .begin. ignore_info_list T_END .sc. ;

ignore_info_list: ignore_info_list ignore
|
;

ignore: T_NAME ':' namelist ';'
{
  chain_list *cl;
  char *section_type;

  INFCHECKFIG();

  if (strcasecmp($1,"instance")==0 || strcasecmp($1,"instances")==0)
    section_type=INF_IGNORE_INSTANCE;
  else if (strcasecmp($1,"transistor")==0 || strcasecmp($1,"transistors")==0)
    section_type=INF_IGNORE_TRANSISTOR;
  else if (strcasecmp($1,"resistance")==0 || strcasecmp($1,"resistances")==0)
    section_type=INF_IGNORE_RESISTANCE;
  else if (strcasecmp($1,"diode")==0 || strcasecmp($1,"diodes")==0)
    section_type=INF_IGNORE_DIODE;
  else if (strcasecmp($1,"capacitance")==0 || strcasecmp($1,"capacitances")==0)
    section_type=INF_IGNORE_CAPACITANCE;
  else if (strcasecmp($1,"parasitics")==0)
    section_type=INF_IGNORE_PARASITICS;
  else if (strcasecmp($1,"signalname")==0 || strcasecmp($1,"signalnames")==0)
    section_type=INF_IGNORE_NAMES;
   else
   {
     avt_errmsg(INF_ERRMSG, "005", AVT_ERROR, getwhere(), $1);
     section_type=NULL;
   }
  
  for (cl=$3; cl!=NULL; cl=cl->NEXT)
    {
      if (section_type!=NULL)
        inf_AddAssociation(INF_FIG, INF_LOADED_LOCATION, section_type, "", namealloc((char *)cl->DATA), NULL, 0, 0, getwhere());
      mbkfree(cl->DATA);
    }
  freechain($3);
}
| namelist
{
  chain_list *cl;

  INFCHECKFIG();

  for (cl=$1; cl!=NULL; cl=cl->NEXT)
    {
      inf_AddAssociation(INF_FIG, INF_LOADED_LOCATION, INF_IGNORE_INSTANCE, "", namealloc((char *)cl->DATA), NULL, 0, 0, getwhere());
      mbkfree(cl->DATA);
    }
  freechain($1);
}
;

namelist       : T_NAME
{
  $$=addchain(NULL, mbkstrdup($1));
}
| namelist ',' T_NAME
{
  $$=append($1, addchain(NULL, mbkstrdup($3)));
}
;

connector_direction_info: T_CONNECTOR T_DIRECTIONS .begin. connector_direction_info_list T_END .sc. ;

connector_direction_info_list: connector_direction_info_list connector_direction
|
;

connector_direction_spec: 
T_INPUT { $$="I"; }
|
T_OUTPUT { $$="O"; }
|
T_INOUT { $$="B"; }
|
T_NAME
{
  if (strcasecmp($1,"tristate")==0) $$="T";
  else if (strcasecmp($1,"unknown")==0) $$="X";
  else if (strcasecmp($1,"hz")==0) $$="Z";
  else
   {
     avt_errmsg(INF_ERRMSG, "006", AVT_ERROR, getwhere(), $1);
     $$="";
   }
}
;

connector_direction: connector_direction_spec ':' namelist ';'
{
  chain_list *cl;
  char *dir=$1;

  INFCHECKFIG();

  for (cl=$3; cl!=NULL; cl=cl->NEXT)
    {
      if (dir!="")
        inf_AddString(INF_FIG, INF_LOADED_LOCATION, namealloc((char *)cl->DATA), INF_CONNECTOR_DIRECTION, dir, getwhere());
      mbkfree(cl->DATA);
    }
  freechain($3);
}
;

.disable_date_delay_dir.:
 updown
{
  $$ = $1;
}
| '<' '?' '>'
{
  $$ = INF_UPDOWN ;
}
|
{
  $$ = INF_UPDOWN ;
}
;

disable_gate_delay   : T_NAME .disable_date_delay_dir. T_NAME .disable_date_delay_dir. ';'
{
   int val;
   if ($4==INF_UPDOWN) val=0x03;
   else if ($4==INF_UP) val=0x02;
   else val=0x01;
   if ($2==INF_UPDOWN) val|=0x30;
   else if ($2==INF_UP) val|=0x20;
   else val|=0x10;

   inf_AddAssociation(INF_FIG, INF_LOADED_LOCATION, infTasVectName($3), INF_DISABLE_GATE_DELAY, infTasVectName($1), NULL, val, 0, getwhere());
}
;

disable_gate_delay_info: T_DISABLE T_GATE T_DELAY .begin. disable_gate_delay_info_list T_END .sc. ;

disable_gate_delay_info_list: disable_gate_delay_info_list disable_gate_delay
|
;


characteristic_info: T_CHARACT .begin. characteristic_info_list T_END .sc. ;

characteristic_info_list: characteristic_info_list characteristic
|
;

characteristic: T_NAME ':' namelist ';'
{
  chain_list *cl;
  char *dir=$1, *section=NULL;

  if (strcasecmp(dir,"data")==0) section=INF_CHARAC_DATA;
  else avt_errmsg(INF_ERRMSG, "022", AVT_ERROR, getwhere(), $1);

  if (section!=NULL)
  {
    INFCHECKFIG();

    for (cl=$3; cl!=NULL; cl=cl->NEXT)
      {
        inf_AddString(INF_FIG, INF_LOADED_LOCATION, namealloc((char *)cl->DATA), section, dir, getwhere());
        mbkfree(cl->DATA);
      }
    freechain($3);
  }
}
;

/*

------------------------------------------------ stb -------------------------------------------------


*/

name            : TOK_NAME T_NAME ';'
                {
                  inffig_list *oldinffig;
                  if (!inf_ignorename)
                    {
                      oldinffig=INF_FIG;
                      if ((INF_FIG=getloadedinffig(namealloc($2)))==NULL)
                        INF_FIG=addinffig($2);
                      if (getptype(INF_FIG->USER, INF_IS_DIRTY)!=NULL) 
                        {
                          infClean(INF_FIG, INF_LOADED_LOCATION);
                          INF_FIG->USER=delptype(INF_FIG->USER, INF_IS_DIRTY);
                        }
                      if (getptype(INF_FIG->USER,INF_LOADED_FROM_DISK)==NULL)
                        INF_FIG->USER=addptype(INF_FIG->USER,INF_LOADED_FROM_DISK,NULL);
                    }
                }
                ;

version         : T_VERSION T_VAL ';'
                ;


stb_general_info  : period
                    {
                      INFCHECKFIG();
                      inf_AddDouble(INF_FIG, INF_LOADED_LOCATION, INF_STB_HEADER, INF_DEFAULT_PERIOD, $1, getwhere());
                    }
                    | setup
                    {
                      INFCHECKFIG();
                      inf_AddDouble(INF_FIG, INF_LOADED_LOCATION, INF_STB_HEADER, INF_SETUPMARGIN, $1, getwhere());
                    }
                    | hold
                    {
                      INFCHECKFIG();
                      inf_AddDouble(INF_FIG, INF_LOADED_LOCATION, INF_STB_HEADER, INF_HOLDMARGIN, $1, getwhere());
                    }
                    | name
                    | version
                    ;

.units. : T_UNITPS
        {
          $$=1e-12;
        }
      | T_UNITNS
        {
          $$=1e-9;
        }
      | T_UNITUS
        {
          $$=1e-6;
        }
      |
        {
          $$=1e-12;
        }
      ;

.capaunits. : T_UNITPF
        {
          $$=1e-12;
        }
      | T_UNITFF
        {
          $$=1e-15;
        }
      |
        {
          $$=1e-15;
        }
      ;

time_value: T_VAL .units.
            {
              $$=$1*$2;
            }
            ;

capa_value: T_VAL .capaunits.
            {
              $$=$1*$2;
            }
            ;

period          :
                T_PERIOD time_value ';'
                {
                  $$=$2;
                }
                ;

.period.       : period
               { 
                 $$=$1;
               }
               |
               { 
                 $$=-1;
               }
               ;

setup           : T_SETUPTIME time_value ';'
                {
                  $$=$2;
                }
                ;

hold            : T_HOLDTIME time_value ';'
                {
                  $$=$2;
                }
                ;

range           : '(' time_value ':' time_value ')'
                {
                  $$.val1 = $2;
                  $$.val2 = $4;
                }
                | time_value
                {
                  $$.val1 = $1;
                  $$.val2 = $1;
                }
                ;

clocklatencies_info: T_CLOCK_LATENCIES T_BEGIN clocklatencies_list T_END ';' 
                   ;

clocklatencies_list: clocklatencies_list clocklatencies
                   |
                   ;

clocklatencies: T_NAME ':' time_value time_value time_value time_value ';'
               {
                  char *name=infTasVectName($1);
                  if ($6+$3+$4+$5!=0) // les 4 egaux a 0? (la latence est positive)
                  {
                    inf_AddDouble (INF_FIG, INF_LOADED_LOCATION, name, INF_LATENCY_FALL_MIN, $5, getwhere());
                    inf_AddDouble (INF_FIG, INF_LOADED_LOCATION, name, INF_LATENCY_FALL_MAX, $6, getwhere());
                    inf_AddDouble (INF_FIG, INF_LOADED_LOCATION, name, INF_LATENCY_RISE_MIN, $3, getwhere());
                    inf_AddDouble (INF_FIG, INF_LOADED_LOCATION, name, INF_LATENCY_RISE_MAX, $4, getwhere());
                  }
               }
               ;

.clock_spec_gene_info.: T_CLOCK clock_ident state state ';'
                {
                   $$.nom=$2.nom;
                   if ($3==INF_VERIFUP || $3==INF_VERIFRISE) $$.edgetype=1; else $$.edgetype=0;
                   if ($4==INF_VERIFUP || $4==INF_VERIFRISE) $$.reltype=1; else $$.reltype=0;
                }
                |
                {
                   $$.nom=NULL;
                }
                ;


clock_spec      : edge_up edge_dn latency .period. .clock_spec_gene_info.
                {
                  $$.val1 = $1.val1 + $3.val1;
                  $$.val2 = $1.val2 + $3.val2;
                  $$.val3 = $2.val1 + $3.val3;
                  $$.val4 = $2.val2 + $3.val4;
                  $$.val = $4;
                  $$.nom=$5.nom;
                  $$.edgetype=$5.edgetype;
                  $$.reltype=$5.reltype;
                }
                | edge_dn edge_up latency .period. .clock_spec_gene_info.
                {
                  $$.val1 = $2.val1 + $3.val1;
                  $$.val2 = $2.val2 + $3.val2;
                  $$.val3 = $1.val1 + $3.val3;
                  $$.val4 = $1.val2 + $3.val4;
                  $$.val = $4;
                  $$.nom=$5.nom;
                  $$.edgetype=$5.edgetype;
                  $$.reltype=$5.reltype;
                }
                | T_CLOCK clock_ident .vclockfactor. .vclockdelta. ';'
                {
                  int val;
                  INFCHECKFIG();
                  $$.nom=NULL;
                  if (inf_GetInt(INF_FIG, $2.nom, INF_CLOCK_TYPE, &val)==0)
                    avt_errmsg(INF_ERRMSG, "007", AVT_ERROR, getwhere(), $2.nom);
                  else
                    { 
                      double per, defper, min;
                      double minr, maxr, minf, maxf;

                      if (!inf_GetDouble(INF_FIG, INF_STB_HEADER, INF_DEFAULT_PERIOD, &defper)) defper=-1;
                      if (!inf_GetDouble(INF_FIG, $2.nom, INF_CLOCK_PERIOD, &per)) per=-1;
                      
                      if (defper==-1 && per==-1 && $3!=1)
                        avt_errmsg(INF_ERRMSG, "008", AVT_ERROR, getwhere(), $2.nom);
                      else
                        {
                          if (per!=-1)
                            $$.val=per*$3;
                          else if (defper!=-1)
                            $$.val=defper*$3;
                          else
                            $$.val=0;

                          inf_GetDouble(INF_FIG, $2.nom, INF_MIN_RISE_TIME, &minr);
                          inf_GetDouble(INF_FIG, $2.nom, INF_MAX_RISE_TIME, &maxr);
                          inf_GetDouble(INF_FIG, $2.nom, INF_MIN_FALL_TIME, &minf);
                          inf_GetDouble(INF_FIG, $2.nom, INF_MAX_FALL_TIME, &maxf);

                          
                          if (minr<minf) min=minr; else min=minf;

                          $$.val1=min+(minr-min)*$3+$4;
                          $$.val2=min+(maxr-min)*$3+$4;
                          $$.val3=min+(minf-min)*$3+$4;
                          $$.val4=min+(maxf-min)*$3+$4;
                          
                          $$.edgetype=1; if ($3<1) $$.reltype=1; else $$.reltype=0;
                          
                          if ((val & INF_CLOCK_INVERTED)!=0)
                            {
                              min=$$.val1; $$.val1=$$.val3; $$.val3=min;
                              min=$$.val2; $$.val2=$$.val4; $$.val4=min;

                              $$.edgetype=($$.edgetype+1) & 1;
                              $$.reltype=($$.reltype+1) & 1;
                            }

                          $$.nom=$2.nom;
                        }
                    }
                }
                ;

latency         : T_LATENCY edge_up T_LATENCY edge_dn
                {
                  $$.val1 = $2.val1;
                  $$.val2 = $2.val2;
                  $$.val3 = $4.val1;
                  $$.val4 = $4.val2;
                }
                | T_LATENCY edge_dn T_LATENCY edge_up
                {
                  $$.val1 = $4.val1;
                  $$.val2 = $4.val2;
                  $$.val3 = $2.val1;
                  $$.val4 = $2.val2;
                }
                |
                {
                  $$.val1 = 0;
                  $$.val2 = 0;
                  $$.val3 = 0;
                  $$.val4 = 0;
                }
                ;
                
.vclockfactor.  : '*' T_VAL
                {
                  $$=$2;
                }
                | T_NAME T_VAL
                {
                  if (strcmp($1,"*")!=0) yyerror();
                  $$=$2;
                }
                | T_NAME
                {
                  if ($1[0]!='*') yyerror();
                  $$=atof(&$1[1]);
                }
                |
                {
                  $$=1;
                }
                ;

.vclockdelta.  : '+' time_value
                {
                  $$=$2;
                }
                | '-' time_value
                {
                  $$=-$2;
                }
                |
                {
                  $$=0;
                }
                ;

edge_up         : T_UP range ';'
                {
                  $$ = $2;
                }
                ;

edge_dn         : T_DOWN range ';'
                {
                  $$ = $2;
                }
                ;

stb_ck_declar_info : clock
                | ckprio
                | clock_groups
                | eqvt_clocks
                ;

clock           : T_CLOCK T_CONNECTORS T_BEGIN cklocon_list T_END ';'
                ;

cklocon_list    : 
                | cklocon_list cklocon
                ;

cklocon         : clock_ident ':' clock_spec
                {
                  INFCHECKFIG();
                  inf_AddInt(INF_FIG, INF_LOADED_LOCATION, $1.nom, INF_CLOCK_TYPE, $1.edgetype, getwhere());
                  
                  inf_AddDouble(INF_FIG, INF_LOADED_LOCATION, $1.nom, INF_MIN_RISE_TIME, $3.val1, getwhere());
                  inf_AddDouble(INF_FIG, INF_LOADED_LOCATION, $1.nom, INF_MAX_RISE_TIME, $3.val2, getwhere());

                  inf_AddDouble(INF_FIG, INF_LOADED_LOCATION, $1.nom, INF_MIN_FALL_TIME, $3.val3, getwhere());
                  inf_AddDouble(INF_FIG, INF_LOADED_LOCATION, $1.nom, INF_MAX_FALL_TIME, $3.val4, getwhere());

                  if ($3.val>=0)
                    inf_AddDouble(INF_FIG, INF_LOADED_LOCATION, $1.nom, INF_CLOCK_PERIOD, $3.val, getwhere());

                  if ($3.nom!=NULL)
                   {
                     inf_AddString(INF_FIG, INF_LOADED_LOCATION, $1.nom, INF_MASTER_CLOCK, $3.nom, getwhere());
                     inf_AddInt(INF_FIG, INF_LOADED_LOCATION, $1.nom, INF_MASTER_CLOCK_EDGES, ($3.edgetype<<1)|$3.reltype, getwhere());
                   }
                }
                |
                 clock_ident ';'
                {
                  INFCHECKFIG();
                  inf_AddInt(INF_FIG, INF_LOADED_LOCATION, $1.nom, INF_CLOCK_TYPE, $1.edgetype, getwhere());                  
                }
                ;

clock_ident     : clock_type T_NAME
                {
                  $$.nom = infTasVectName($2);
                  $$.edgetype=$1; // not inverted
                }
                | clock_type '!' T_NAME
                {
                  $$.nom = infTasVectName($3);
                  $$.edgetype=$1|INF_CLOCK_INVERTED; //inverted
                }
                ;

clock_type      : .virtual.
                {
                  $$ = $1;
                }
                | .ideal.
                {
                  $$ = $1;
                }
                |
                {
                  $$=0;
                }
                ;

ckname_list     : 
                {
                  $$ = NULL;
                }
                | T_NAME
                {
                  $$=addchain(NULL, infTasVectName($1));
                }
                | ckname_list ',' T_NAME
                {
                  $$=addchain($1, infTasVectName($3));
                }
                ;

domain_groups   : 
                | group domain_groups
                {
                  chain_list *cl;
                  INFCHECKFIG();
                  if ($1.val>=0) inf_AddDouble(INF_FIG, INF_LOADED_LOCATION, INF_ASYNC_CLOCK_GROUP_PERIOD, $1.nom, $1.val, getwhere());
                  for (cl=$1.chain; cl!=NULL; cl=cl->NEXT)
                    inf_AddString(INF_FIG, INF_LOADED_LOCATION, (char *)cl->DATA, INF_ASYNC_CLOCK_GROUP, $1.nom, getwhere());
                  freechain($1.chain);
                }
                ;

eqvt_groups     : 
                | egroup eqvt_groups
                {
                  chain_list *cl;
                  INFCHECKFIG();
                  for (cl=$1.chain; cl!=NULL; cl=cl->NEXT)
                    inf_AddString(INF_FIG, INF_LOADED_LOCATION, (char *)cl->DATA, INF_EQUIV_CLOCK_GROUP, $1.nom, getwhere());
                  freechain($1.chain);
                }
                ;

group           : T_NAME ':' ckname_list ';' .period.
                {
                  $$.chain=$3;
                  $$.nom=infTasVectName($1);
                  $$.val=$5;
                }
                ;

egroup          : T_NAME ':' ckname_list ';'
                {
                  $$.chain=$3;
                  $$.nom=infTasVectName($1);
                }
                ;

clock_groups : T_ASYNCHRONOUS T_CLOCK T_GROUPS T_BEGIN domain_groups T_END ';'
                ;

eqvt_clocks : T_EQUIVALENT T_CLOCK T_GROUPS T_BEGIN eqvt_groups T_END ';'
                ;

disable_from    :
                {
                  $$ = NULL;
                }
                | T_FROM T_NAME
                {
                  $$ = infTasVectName($2);
                }
                ;

disable_to      :
                {
                  $$ = NULL;
                }
                | T_TO T_NAME
                {
                  $$ = infTasVectName($2);
                }
                ;
                
disable_pair    : disable_from disable_to ';'
                {
                  INFCHECKFIG();
                  if ($1!=NULL || $2!=NULL)
                    {
                      inf_AddAssociation(INF_FIG, INF_LOADED_LOCATION, INF_DISABLE_PATH, "", $1, $2, 0, 0, getwhere());
                    }
                }
                ;

disable_list    : 
                | disable_list disable_pair
                ;

ckprio          : T_MULTIPLE T_CLOCK T_PRIORITY T_BEGIN ckprio_list T_END ';'
                ;

ckprio_list     : 
                | ckprio_list ckprio_pair
                ;

ckprio_pair     : T_NAME ':' T_NAME ';'
                {
                  INFCHECKFIG();
                  inf_AddString(INF_FIG, INF_LOADED_LOCATION, infTasVectName($1), INF_PREFERED_CLOCK, infTasVectName($3), getwhere());
                }
                ;

stb_disable_info : T_DISABLE T_BEGIN disable_list T_END ';'
                ;

stb_wenable_info : T_CONDITIONED T_COMMAND T_STATES T_BEGIN command_list T_END ';'
                {
                  
                }
                ;

state           : T_UP
                {
                  $$ = INF_VERIFUP;
                }
                | T_DOWN
                {
                  $$ = INF_VERIFDOWN;
                }
                | T_RISING
                {
                  $$ = INF_VERIFRISE;
                }
                | T_FALLING
                {
                  $$ = INF_VERIFFALL;
                }
                | T_NOVERIF
                {
                  $$ = INF_NOVERIF;
                }
                ;

command_list    : 
                | command_list command
                ;

command         : nameordefault ':' state ';'
                {
                  INFCHECKFIG();
                  inf_AddString(INF_FIG, INF_LOADED_LOCATION, infTasVectName($1), INF_VERIF_STATE, $3, getwhere());
                }
                ;

stb_specin_info :
                T_SPECIFY T_INPUT T_CONNECTORS T_BEGIN inlocon_list/* indefault_list*/ T_END ';'
                ;

stability_list  : 
                {
                  $$=NULL;
                }
                | stability stability_list
                {
                  $1->NEXT=$2;
                  $$=$1;
                }
                ;

stability       : stability_type integer_list ';'
                {
                  inf_stb_p_s_stab *ispss;
                  ispss=inf_stb_parse_spec_stab_alloc(NULL);
                  ispss->TIME_LIST=$2;
                  ispss->STABILITY=$1;
                  ispss->CKNAME=NULL;
                  ispss->CKEDGE=INF_STB_SLOPEALL;
                  $$=ispss;
                }
                | stability_type time_value relative_phase ';'
                {
                  inf_stb_p_s_stab *ispss;
                  ispss=inf_stb_parse_spec_stab_alloc(NULL);
                  ispss->DELAY=$2;
                  ispss->RELATIVITY=$3.reltype;
                  ispss->STABILITY=$1;
                  ispss->CKNAME=$3.nom;
                  ispss->CKEDGE=$3.edgetype;
                  $$=ispss;
                }
                ;

stability_type  : T_UNSTABLE
                {
                  $$ = INF_STB_UNSTABLE;
                }
                | T_STABLE
                {
                  $$ = INF_STB_STABLE;
                }
                ;

relative_phase  : relative_type T_NAME edge_option
                {
                  $$.nom=infTasVectName($2);
                  $$.edgetype=$3.edgetype;
                  $$.reltype=$1;
                }
                ;

relative_type   : T_AFTER
                {
                  $$ = INF_STB_AFTER;
                }
                | T_BEFORE
                {
                  $$ = INF_STB_BEFORE;
                }
                ;

edge_option     : 
                {
                  $$.edgetype = INF_STB_SLOPEALL;
                }
                | T_RISING
                {
                  $$.edgetype = INF_STB_RISING;
                }
                | T_FALLING
                {
                  $$.edgetype = INF_STB_FALLING;
                }
                ;

source_phase    : 
                {
                  $$.nom=NULL;
                  $$.edgetype=INF_STB_SLOPEALL;
                }
                | T_FROM T_NAME edge_option
                {
                  $$.nom=infTasVectName($2);
                  $$.edgetype=$3.edgetype;
                }
                ;

dest_phase      : 
                {
                  $$.nom=NULL;
                  $$.edgetype=INF_STB_SLOPEALL;
                }
                | T_FOR T_NAME edge_option
                {
                  $$.nom=infTasVectName($2);
                  $$.edgetype=$3.edgetype;
                }
                ;

integer_list    : 
                {
                  $$ = NULL;
                }
                | time_value integer_list
                {
                  double *dp;
                  dp=(double *)mbkalloc(sizeof(double));
                  *dp=$1;
                  $$=addchain($2, dp);
                }
                ;

inlocon_list    : 
                | inlocon_list inlocon
                ;

nameordefault: T_NAME
               {
                 strcpy($$,$1);
               } 
               |
               T_DEFAULT
               {
                 strcpy($$,"default");
               }
               ;

inlocon         : nameordefault edge_option source_phase ':' stability_list
                {
                  addstab($1, &$2, &$3, INF_STB_NOTHING, $5, INF_STB_SPECIN, INF_SPECIN);
                }
                ;

stb_specout_info : 
                T_VERIFY T_OUTPUT T_CONNECTORS T_BEGIN outlocon_list T_END ';'
                ;

outlocon_list   : 
                | outlocon_list outlocon
                ;

outlocon        : nameordefault edge_option dest_phase ':' stability_list
                {
                  addstab($1, &$2, &$3, INF_STB_NOTHING, $5, INF_STB_SPECOUT, INF_SPECOUT);
                }
                ;


hz_option       : 
                {
                  $$ = INF_STB_NOTHING;
                }
                | T_WITHOUT T_PRECHARGE
                {
                  $$ = INF_STB_HZ_NO_PRECH;
                }
                | T_WITHOUT T_EVALUATE
                {
                  $$ = INF_STB_HZ_NO_EVAL;
                }
                ;

stb_inputs_info : T_INPUT T_CONNECTORS T_STABILITY T_BEGIN innode_list T_END ';'
                ;

innode_list     : 
                | innode_list innode
                ;

innode          : T_NAME edge_option source_phase hz_option ':' stability_list
                {
                  addstab($1, &$2, &$3, $4, $6, INF_STB_STBOUT_SPECIN, INF_STBOUT_SPECIN);
                }
                ;

stb_outputs_info : T_OUTPUT T_CONNECTORS T_STABILITY T_BEGIN outnode_list T_END ';'
                ;

outnode_list    : 
                | outnode_list outnode
                ;

outnode         : T_NAME edge_option source_phase hz_option ':' stability_list
                {
                  addstab($1, &$2, &$3, $4, $6, INF_STB_STBOUT_SPECOUT, INF_STBOUT_SPECOUT);
                }
                ;

stb_memories_info : T_MEMORY T_NODES T_STABILITY T_BEGIN mem_list T_END ';'
                ;

mem_list        : 
                | mem_list mem
                ;

mem             : T_NAME edge_option source_phase hz_option ':' stability_list
                {
                  addstab($1, &$2, &$3, $4, $6, INF_STB_STBOUT_SPECMEM, INF_STBOUT_SPECMEM);
                }
                ;

stb_internals_info : T_INTERNAL T_NODES T_STABILITY T_BEGIN node_list T_END ';'
                ;

node_list       : 
                | node_list node
                ;

node            : T_NAME edge_option source_phase hz_option ':' stability_list
                {
                  addstab($1, &$2, &$3, $4, $6, INF_STB_STBOUT_SPECINODE, INF_STBOUT_SPECINODE);
                }
                ;

nocheck_info      : T_NOCHECK .begin. nocheck_list T_END .sc.
;

nocheck_list      : /* rien */
| nocheck_list nocheck
;


/*
.nocheckclockspeck.: T_NAME edge_option T_NAME edge_option
{
  $$.nom=infTasVectName($1);
  $$.nom1=infTasVectName($3);
  $$.edgetype=$2;
  $$.edgetype=$4;
}
| { $$.nom=NULL;}
;
*/
nocheck           : T_NAME  T_NAME  ';'
{
  int val=0, old=0;
  char *name;
  
  INFCHECKFIG();

  if (strcasecmp($2,"setup")==0 || strcasecmp($2,"all")==0) val|=INF_NOCHECK_SETUP;
  if (strcasecmp($2,"hold")==0 || strcasecmp($2,"all")==0) val|=INF_NOCHECK_HOLD;
  if (val!=0)
  {
    name=infTasVectName($1);
    inf_GetInt(INF_FIG, name, INF_NOCHECK, &old);
    old|=val;
    inf_AddInt(INF_FIG, INF_LOADED_LOCATION, name, INF_NOCHECK, val, getwhere());
  }
  else 
    avt_errmsg(INF_ERRMSG, "020", AVT_ERROR, getwhere(), $2);
}
;

outcapa_info    : T_OUTPUT T_CAPACITANCE T_BEGIN outcapa_list T_END ';'
;

outcapa_list       : /* rien */
| outcapa_list outcapa
;

outcapa         : T_NAME ':' capa_value ';'
{
  char *name;

  INFCHECKFIG();

  name=infTasVectName ($1);
  inf_AddDouble(INF_FIG, INF_LOADED_LOCATION, name, INF_OUTPUT_CAPACITANCE, $3, getwhere());
}
;

setuphold_info    : T_SETUPHOLD T_BEGIN setuphold_list T_END ';'
;

setuphold_list       : /* rien */
| setuphold_list setuphold
;

time_or_nothing:
 time_value { $$ = $1; }
| '-' { $$ = -1; }
;

setuphold         : T_NAME ':' '(' time_or_nothing time_or_nothing ')' '(' time_or_nothing time_or_nothing ')' ';'
{
  char *name;

  INFCHECKFIG();

  name=infTasVectName ($1);
  if ($4!=-1) inf_AddDouble(INF_FIG, INF_LOADED_LOCATION, name, INF_FALL_SETUP, $4, getwhere());
  if ($5!=-1) inf_AddDouble(INF_FIG, INF_LOADED_LOCATION, name, INF_FALL_HOLD, $5, getwhere());
  if ($8!=-1) inf_AddDouble(INF_FIG, INF_LOADED_LOCATION, name, INF_RISE_SETUP, $8, getwhere());
  if ($9!=-1) inf_AddDouble(INF_FIG, INF_LOADED_LOCATION, name, INF_RISE_HOLD, $9, getwhere());
}
;

stabflags_info    : T_STABFLAGS T_BEGIN stabflags_list T_END ';'
;

stabflags_list       : /* rien */
| stabflags_list stabflags
;

stabflags         : T_NAME ':' T_NAME T_NAME ';'
{
  char *name, *fl[2]={$3, $4};
  int flag=0;
  int i, j;
  INFCHECKFIG();

  name=infTasVectName ($1);
  for (i=0; i<2; i++)
  {
    flag=flag<<16;
    for (j=0; fl[i][j]!='\0'; j++)
     {
       if (fl[i][j]=='S') flag|=INF_NODE_FLAG_STABCORRECT;
     }
  }
  if (flag!=0) inf_AddInt(INF_FIG, INF_LOADED_LOCATION, name, INF_STB_NODE_FLAGS, flag, getwhere());
}
;

directive_state_ud : T_UP
                {
                  $$ = INF_DIRECTIVE_UP;
                }
                | T_DOWN
                {
                  $$ = INF_DIRECTIVE_DOWN;
                }
                ;
                
directive_state_rf:
                 T_RISING
                {
                  $$ = INF_DIRECTIVE_RISING;
                }
                | T_FALLING
                {
                  $$ = INF_DIRECTIVE_FALLING;
                }
                ;

.directive_state_rf.:
                directive_state_rf
                {
                  $$=$1;
                }
                | 
                {
                  $$ = 0;
                }
                ;

directive_action: T_BEFORE
                {
                  $$=INF_DIRECTIVE_BEFORE;
                }
                | T_AFTER
                {
                  $$=INF_DIRECTIVE_AFTER;
                }
                ;

directive_margin: T_MARGIN  time_value
                {
                  $$=$2;
                }
                |
                {
                  $$=0;
                }
                ;
                
directive_source_name_or_time:T_CLOCK T_NAME directive_state_rf
                              {
                                $$.nom=infTasVectName($2);
                                $$.edgetype=$3|INF_DIRECTIVE_CLOCK;
                              }
                              | T_NAME .directive_state_rf.
                              {
                                $$.nom=infTasVectName($1);
                                $$.edgetype=$2;
                              }
                              ;

directive_name_or_time: T_WITH T_CLOCK T_NAME directive_state_ud
                        {
                          $$.nom=infTasVectName($3);
                          $$.edgetype=$4|INF_DIRECTIVE_CLOCK;
                          $$.reltype=0;
                        }
                        | directive_action T_CLOCK T_NAME directive_state_rf
                        {
                          $$.nom=infTasVectName($3);
                          $$.edgetype=$4|INF_DIRECTIVE_CLOCK;
                          $$.reltype=$1;
                        }
                        | directive_action T_NAME .directive_state_rf.
                        {
                          $$.nom=infTasVectName($2);
                          $$.edgetype=$3;
                          $$.reltype=$1;
                        }
                        | directive_action T_DELAY time_value .directive_from.
                        {
                          // not supported yet
                          $$.nom=$4.nom;
                          $$.reltype=$1;
                          $$.val=$3;
                          $$.edgetype=INF_DIRECTIVE_DELAY|$4.edgetype;
                        }
                        ;

.directive_from. : T_FROM T_NAME .directive_state_rf.
                   {
                      $$.nom=infTasVectName($2);
                      $$.edgetype=$3;
                   }
                   |
                   {
                      $$.nom=infTasVectName("*");
                      $$.edgetype=0;
                   }
                   ;

                        
directives: T_NAME ':' directive_source_name_or_time directive_name_or_time directive_margin ';'
            {
              int val, err=0;
              splitint *si=(splitint *)&val;

              INFCHECKFIG();
              if (strcasecmp($1, "filter")==0) si->cval.a=INF_DIRECTIVE_FILTER;
              else if (strcasecmp($1, "check")==0) si->cval.a=0;
              else { err=1; avt_errmsg(INF_ERRMSG, "005", AVT_ERROR, getwhere(), $1); }
              if (err==0)
              {
                if ((si->cval.a & INF_DIRECTIVE_FILTER)!=0 && (($3.edgetype & INF_DIRECTIVE_CLOCK)!=0 || ($4.edgetype & INF_DIRECTIVE_CLOCK)==0 || $4.reltype!=0))
                  { err=1; avt_errmsg(INF_ERRMSG, "024", AVT_ERROR, getwhere()); }
              }
              if (err==0)
              {                
                double delay=$5;
                si->cval.b=$3.edgetype;
                si->cval.c=$4.reltype;
                si->cval.d=$4.edgetype;
                if ($4.edgetype & INF_DIRECTIVE_DELAY)
                  {
                    delay=$4.val;
                  }
                inf_AddAssociation(INF_FIG, INF_LOADED_LOCATION, INF_DIRECTIVES, "", infTasVectName($3.nom), infTasVectName($4.nom), val, delay, getwhere());
              }
            }
            ;
            
directives_list       : /* rien */
| directives_list directives
;

directives_info : T_DIRECTIVES T_BEGIN directives_list T_END ';'

conditions_info      : T_CONDITION T_BEGIN conditions_list T_END ';'
;

conditions_list      : /* rien */
| conditions_list conditions
;

conditions           : T_NAME '=' T_NAME  ';'
{
  INFCHECKFIG();
  inf_AddAssociation(INF_FIG, INF_LOADED_LOCATION, INF_CONDITIONS, "", namealloc($1), namealloc($3), 0, 0, getwhere());
}
;

proba_info    : T_SWITCHPROBA .begin. proba_list T_END .sc.
{

}
;

proba_list       : /* rien */
| proba_list proba
{

}
;
				
proba           : T_NAME ':' T_VAL ';'
{
  char *name;
  double val=$3;

  INFCHECKFIG();
  if (val>=0 && val<=1)
  {
    name=infTasVectName ($1);  
    inf_AddDouble(INF_FIG, INF_LOADED_LOCATION, name, INF_SWITCHING_PROBABILITY, val, getwhere());
  }
  else
    avt_errmsg(INF_ERRMSG, "026", AVT_ERROR, getwhere(), val);
}
;
	
%%
int yyerror ()
{
   
  avt_errmsg(INF_ERRMSG, "009", AVT_ERROR, infFile, infParseLine, yytext);
  INF_ERROR = 1;
  return 0 ;
}

/*--------------------------------------------------------------------+
  | Remplace un nom vectorise par son equivalent valide TAS             |
  | ex 'toto<0>' => 'toto[0]'                                            |
  +--------------------------------------------------------------------*/
char * infTasVectName(name)
     char *name;
{
  return mbk_vect(mbk_decodeanyvector(name),'[',']');
}

/***********************************************************************
 *                         fonction checkJokName();                    *
 **********************************************************************/
static void checkJokName(motif,name)
     char * motif ;
     char * name ;
{
  short i=0 ;
  char * pt = motif ;
  while(*pt!='\0')
    {
      if(*pt==INF_JOK) i++;
      pt++;
    }
  pt=name ;
  while(*pt!='\0')
    {
      if(*pt==INF_JOK) i--;
      pt++;
    }
  if(i!=0)  
    {
      avt_errmsg(INF_ERRMSG, "010", AVT_ERROR, infFile, infParseLine);
      EXIT(-1);
    }
}

static char *getwhere()
{
  static char buf[512];
  sprintf(buf,"%s:%d: ", infFile, infParseLine);
  return buf;
}

static double todoublesec(void *val)
{
  return ((double)(long)val)*1e-12;
}

static void addstab(char *name, misc_type *mscdata, misc_type *mscck, int hzopt, inf_stb_p_s_stab *stab, int stabtype, char *inftype)
{
  inf_stb_p_s *ispst, *old, *exist;
  char *sig;
  
  INFCHECKFIG();

  sig=infTasVectName(name);
  ispst=inf_stb_parse_spec_alloc(NULL);
  ispst->STABILITY=stab;
  ispst->DATAEDGE=mscdata->edgetype;
  ispst->CKNAME=mscck->nom;
  ispst->CKEDGE=mscck->edgetype;
  ispst->SPECTYPE=stabtype;
  ispst->HZOPTION=hzopt;
  if (!inf_GetPointer(INF_FIG, sig, inftype, (void **)&old))
    {
      ispst->NEXT=NULL;
      inf_AddPointer(INF_FIG, INF_LOADED_LOCATION, sig, inftype, INF_SPECIN, ispst, getwhere());
    }
  else
    {
      if ((exist=inf_stb_spec_exists(ispst, old))==NULL)
        append((chain_list *)old, (chain_list *)ispst);
      else
        {
          // on remplace la stabilite existante
          old=exist->NEXT;
          inf_stb_parse_spec_stab_free(exist->STABILITY);
          memcpy(exist, ispst, sizeof(*ispst));
          exist->NEXT=old;
          ispst->STABILITY=NULL;
          inf_stb_parse_spec_del(ispst);
        }
    }
}

static void INFCHECKFIG()
{
  inffig_list *oldinffig;
  if (INF_FIG!=NULL) return;

  oldinffig=INF_FIG;
  if ((INF_FIG=getloadedinffig(infname))==NULL)
    INF_FIG=addinffig(infname);

  if (getptype(INF_FIG->USER, INF_IS_DIRTY)!=NULL) 
  { 
    infClean(INF_FIG, INF_LOADED_LOCATION);
    INF_FIG->USER=delptype(INF_FIG->USER, INF_IS_DIRTY);
  }
  if (getptype(INF_FIG->USER,INF_LOADED_FROM_DISK)==NULL)
    INF_FIG->USER=addptype(INF_FIG->USER,INF_LOADED_FROM_DISK,NULL);

  if (!inf_ignorename)
    avt_errmsg(INF_ERRMSG, "011", AVT_WARNING, infname);

  inf_ignorename=0;
}

static ptype_list *finish_falsepath(ptype_list *pt)
{
  ptype_list *head, *npt;
  head=pt=(ptype_list *)reverse((chain_list *)pt);
  while (pt!=NULL && pt->NEXT!=NULL)
  {
     if (pt->DATA!=NULL && pt->NEXT->DATA==NULL && pt->NEXT->TYPE!=INF_NOTHING)
      {
        pt->TYPE=pt->NEXT->TYPE;
        npt=pt->NEXT;
        pt->NEXT=pt->NEXT->NEXT;
        npt->NEXT=NULL;
        freeptype(npt);
      }
     else pt=pt->NEXT;
  }
  return (ptype_list *)reverse((chain_list *)head);
}

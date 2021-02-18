/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STB Version 1.00                                            */
/*    Fichier : stb_util.c                                                  */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                Anthony LESTER                                            */
/*                                                                          */
/****************************************************************************/

#include <stdlib.h>

#include MUT_H
#include STB_H

#include "stb_util.h"
#include "stb_error.h"
#include "stb_init.h"
#include "stb_clock_tree.h"
#include "stb_falseslack.h"
#include "stb_relaxation_correction.h"

/*****************************************************************************
* global variables                                                           *
*****************************************************************************/

stbfig_list  *HEAD_STBFIG = NULL;

char STB_LANG = 'E';
char STB_TRACE_MODE = 'N';
char STB_REPORT = 'Y';
char STB_OUT = 'Y';
char STB_SILENT = 'N';
char STB_MULTIPLE_COMMAND = 'Y';
char STB_OPEN_LATCH_PHASE = 'Y';
char *STB_TOOLNAME = "stb" ;
char STB_ANALYSIS_VAR = STB_GLOB_ANALYSIS ;
char STB_GRAPH_VAR = STB_RED_GRAPH ;
char STB_MODE_VAR = STB_STABILITY_LT|STB_STABILITY_BEST|STB_STABILITY_SETUP|STB_STABILITY_HOLD ;
char STB_CTK_VAR = STB_CTK_NOT|STB_CTK_OBSERVABLE  ;
double STB_LIMITS = 100.0;
double STB_LIMITL = 900.0;

char STB_FILE_FORMAT = STB_STB_FORMAT;
char *STB_FOREIGN_TIME_UNIT = "1ns"; /* default time unit in Synopsys Liberty format */
char *STB_FOREIGN_CONSTRAINT_FILE = NULL;

/*****************************************************************************
* private variables                                                          *
*****************************************************************************/

static stbpair_list *HEAD_STBPAIR;

/*****************************************************************************
* static function declarations                                               *
*****************************************************************************/

static int phasecompare __P((phase *p1, phase *p2));
static int longcompare __P((long *l1, long *l2));

/*****************************************************************************
*                           fonction stb_env()                               *
*****************************************************************************/
void
stb_env()
{
    char *str;

    stb_init_var();

    /* variable mode trace */
        STB_TRACE_MODE = V_BOOL_TAB[__STB_TRACE_MODE].VALUE ? 'Y' : 'N' ;
  
    /* variable langage */
    STB_LANG = 'E' ;
        STB_REPORT = V_BOOL_TAB[__STB_REPORT_FILE].VALUE ? 'Y' : 'N';
        STB_OUT = V_BOOL_TAB[__STB_OUT_FILE].VALUE ? 'Y' : 'N';
    
        STB_GRAPH_VAR = V_BOOL_TAB[__STB_DET_GRAPH].VALUE? STB_DET_GRAPH : STB_RED_GRAPH;
        STB_ANALYSIS_VAR = V_BOOL_TAB[__STB_DET_ANALYSIS].VALUE? STB_DET_ANALYSIS : STB_GLOB_ANALYSIS;
        if(V_BOOL_TAB[__STB_STABILITY_WORST].VALUE ){
            STB_MODE_VAR |= STB_STABILITY_WORST;
            STB_MODE_VAR &= ~(STB_STABILITY_BEST);
        }else{
            STB_MODE_VAR &= ~(STB_STABILITY_WORST);
            STB_MODE_VAR |= STB_STABILITY_BEST;
        }
    str = V_STR_TAB[__STB_MONOPHASE].VALUE ;
    if(str != NULL) {
        if(strcmp(str,"flip_flop") == 0 ){
            STB_MODE_VAR |= STB_STABILITY_FF;
            STB_MODE_VAR &= ~(STB_STABILITY_LT|STB_STABILITY_ER);
        }
        else if(strcmp(str,"transparent") == 0 ){
            STB_MODE_VAR |= STB_STABILITY_LT;
            STB_MODE_VAR &= ~(STB_STABILITY_FF|STB_STABILITY_ER);
        }
        else if(strcmp(str,"error") == 0 ){
            STB_MODE_VAR |= STB_STABILITY_ER;
            STB_MODE_VAR &= ~(STB_STABILITY_FF|STB_STABILITY_LT);
        }
    }
        if(V_BOOL_TAB[__STB_STABILITY_SETUP].VALUE){
            STB_MODE_VAR |= STB_STABILITY_SETUP;
            STB_MODE_VAR &= ~(STB_STABILITY_HOLD);
        }
        if(V_BOOL_TAB[__STB_STABILITY_HOLD].VALUE ){
            STB_MODE_VAR |= STB_STABILITY_HOLD;
            STB_MODE_VAR &= ~(STB_STABILITY_SETUP);
        }
        if(V_BOOL_TAB[__STB_STABILITY_LAST].VALUE){
            STB_MODE_VAR |= STB_STABILITY_LAST;
        }
        if(V_BOOL_TAB[__STB_SILENT].VALUE){
            STB_SILENT = 'Y' ;
        }
        if(V_BOOL_TAB[__STB_CTK].VALUE){
            STB_CTK_VAR |= STB_CTK;
        }
        if(V_BOOL_TAB[__STB_CTK_LINE].VALUE){
            STB_CTK_VAR |= STB_CTK|STB_CTK_LINE;
        }
        if(V_BOOL_TAB[__STB_CTK_WORST].VALUE){
            STB_CTK_VAR |= STB_CTK|STB_CTK_WORST;
        }

    if (V_BOOL_TAB[__STB_CTK_OBSERVABLE].VALUE)
      STB_CTK_VAR |= STB_CTK_OBSERVABLE;
    else
      STB_CTK_VAR &= ~STB_CTK_OBSERVABLE;
    
        if(!V_BOOL_TAB[__STB_CTK_REPORT].VALUE ){
            STB_CTK_VAR &= ~STB_CTK_REPORT;
        } else {
            STB_CTK_VAR |= STB_CTK_REPORT;
        }

    str = V_STR_TAB[__STB_FILE_FORMAT].VALUE ;
    if(str != NULL) {
        if (strcmp(str,"sdc") == 0 ) {
            STB_FILE_FORMAT = STB_SDC_FORMAT;
        }
        else if (strcmp(str,"stb") == 0 ) {
            STB_FILE_FORMAT = STB_STB_FORMAT;
        }
        else
            stb_error (ERR_ENV, "Bad value for the variable StbFileFormat\n", 0, STB_NONFATAL );
    }
    
            STB_MULTIPLE_COMMAND = V_BOOL_TAB[__STB_MULTIPLE_COMMAND].VALUE?'Y':'N' ;

    str = V_STR_TAB[__STB_FOREIGN_TIME_UNIT].VALUE ;
    if(str != NULL) {
        STB_FOREIGN_TIME_UNIT = namealloc (str);
    }
    str = V_STR_TAB[__STB_FOREIGN_CONSTRAINT_FILE].VALUE ;
    if(str != NULL) {
        STB_FOREIGN_CONSTRAINT_FILE = namealloc (str);
    }
    str = getenv("STB_OPEN_LATCH_PHASE") ;
    if(str != NULL) {
        if(strcmp(str,"no") == 0 ){
            STB_OPEN_LATCH_PHASE = 'N' ;
        }
        else {
            STB_OPEN_LATCH_PHASE = 'Y' ;
        }
    }
        STB_DEF_SLEW = V_FLOAT_TAB[__SIM_INPUT_SLOPE].VALUE * 1e12;
}

/*****************************************************************************
 *                          function stb_exit()                              *
 *****************************************************************************/

void
stb_exit(code)
    int code;
{
    EXIT(code);
}

/*****************************************************************************
*                           fonction stb_addstbnode()                        *
*****************************************************************************/
void
stb_addstbnode(ptnode)
    ttvevent_list *ptnode;
{
    stbnode       *ptstbnode;

    if (getptype(ptnode->USER, STB_NODE) == NULL) {
        ptstbnode = (stbnode *)mbkalloc(sizeof(stbnode));
        ptstbnode->SPECIN = NULL;
        ptstbnode->SPECOUT = NULL;
        ptstbnode->CK = NULL;
        ptstbnode->STBTAB = NULL;
        ptstbnode->STBHZ = NULL;
        ptstbnode->SETUP = STB_NO_TIME;
        ptstbnode->HOLD = STB_NO_TIME;
        ptstbnode->NBINDEX = STB_NO_INDEX;
        ptstbnode->FLAGRC = STB_NO_RC_DELAY ;
        ptstbnode->FLAG = (char)0 ;

        ptstbnode->EVENT = ptnode;
        ptnode->USER = addptype(ptnode->USER, STB_NODE, ptstbnode);
    }
}

/*****************************************************************************
*                           fonction stb_delstbnode()                        *
*****************************************************************************/
void
stb_delstbnode(ptnode, nbindex)
    ttvevent_list *ptnode;
    char           nbindex;
{
    stbnode       *ptstbnode;
    ptype_list    *ptuser;
    int            i;

    if ((ptuser = getptype(ptnode->USER, STB_NODE)) != NULL) {
        ptstbnode = (stbnode *)ptuser->DATA;
        stb_delstbck(ptstbnode);
        if (ptstbnode->SPECIN != NULL) {
            for (i = 0; i < nbindex; i++) {
                stb_freestbpair(ptstbnode->SPECIN[i]);
            }
            mbkfree(ptstbnode->SPECIN);
        }
        if (ptstbnode->SPECOUT != NULL) {
            for (i = 0; i < nbindex; i++) {
                stb_freestbpair(ptstbnode->SPECOUT[i]);
            }
            mbkfree(ptstbnode->SPECOUT);
        }
        if (ptstbnode->STBTAB != NULL) {
            for (i = 0; i < nbindex; i++) {
                stb_freestbpair(ptstbnode->STBTAB[i]);
            }
            mbkfree(ptstbnode->STBTAB);
        }
        if (ptstbnode->STBHZ != NULL) {
            for (i = 0; i < nbindex; i++) {
                stb_freestbpair(ptstbnode->STBHZ[i]);
            }
            mbkfree(ptstbnode->STBHZ);
        }
        mbkfree(ptstbnode);
        ptnode->USER = delptype(ptnode->USER, STB_NODE);
    }
}

/*****************************************************************************
*                           fonction stb_alloctab()                          *
*****************************************************************************/
stbpair_list **
stb_alloctab(size)
    int     size;
{
    stbpair_list    **pttable;
    int               i;

    pttable = (stbpair_list **)mbkalloc(size * sizeof(stbpair_list *));
    for (i = 0; i < size; i++) {
        pttable[i] = NULL;
    }
    return pttable;
}

/*****************************************************************************
*                           fonction stb_addstbpair()                        *
*****************************************************************************/
stbpair_list *stb_addstbpair_and_phase(stbpair_list *head, long d, long u, unsigned char pD, unsigned char pU)
{
    stbpair_list *ptstbpair;
    int           i;

    if (HEAD_STBPAIR == NULL) {

        #ifdef NOHEAPALLOC
        HEAD_STBPAIR = (stbpair_list *)mbkalloc(sizeof(stbpair_list));
        HEAD_STBPAIR->NEXT = NULL;
        #else
        HEAD_STBPAIR = (stbpair_list *)mbkalloc(BUFSIZE*sizeof(stbpair_list));
        ptstbpair = HEAD_STBPAIR;
        for (i = 1; i < BUFSIZE; i++) {
            ptstbpair->NEXT = ptstbpair + 1;
            ptstbpair++;
        }
        ptstbpair->NEXT = NULL;
        #endif
    }

    if(d > u)
      d = u ;

    ptstbpair = HEAD_STBPAIR;
    HEAD_STBPAIR = HEAD_STBPAIR->NEXT;
    ptstbpair->NEXT = head;
    ptstbpair->D = d;
    ptstbpair->U = u;
    ptstbpair->phase_U = pU;
    ptstbpair->phase_D = pD;
    return ptstbpair;
}

stbpair_list *stb_addstbpair(stbpair_list *head, long d, long u)
{
  return stb_addstbpair_and_phase(head, d, u, STB_NO_INDEX, STB_NO_INDEX);
}
/*****************************************************************************
*                           fonction stb_delstbpair()                        *
*****************************************************************************/
stbpair_list *
stb_delstbpair(head, del)
    stbpair_list *head;
    stbpair_list *del;
{
    stbpair_list *pt;
    stbpair_list *ptprevious = NULL;

    if (head == NULL || del == NULL) {
        fflush(stdout);
        fprintf(stderr, "*** stb error ***");
        fprintf(stderr, "  stb_delstbpair() impossible : NULL pointer !\n");
        EXIT(1);
    }

    if (del == head) {
        pt = head->NEXT;
        head->NEXT = NULL;
        stb_freestbpair(head);
        return pt;
    }
    else {
        for (pt = head; pt; pt = pt->NEXT) {
            if (pt == del) break;
            ptprevious = pt;
        }
        if (pt != NULL) {
            ptprevious->NEXT = pt->NEXT;
            del->NEXT = NULL;
            stb_freestbpair(del);
            return head;
        }
        else return NULL;
    }
}

/*****************************************************************************
*                           fonction stb_freestbpair()                       *
*****************************************************************************/
void
stb_freestbpair(head)
    stbpair_list *head;
{
    #ifdef NOHEAPALLOC
    stbpair_list *scan, *next;
    for( scan = head ; scan ; scan = next ) {
        next = scan->NEXT;
        mbkfree( scan );
    }
    #else
    HEAD_STBPAIR = (stbpair_list *)append((chain_list *)head, (chain_list *)HEAD_STBPAIR);
    #endif
}

/*****************************************************************************
*                           fonction stb_freestbtabpair()                    *
*****************************************************************************/
void
stb_freestbtabpair(tab,size)
    stbpair_list **tab;
    char size ;
{
 int i ;

 for (i = 0; i < (int) size; i++) {
   stb_freestbpair(tab[i]);
 }
}

/*****************************************************************************
*                           fonction stb_addstbck()                          *
*****************************************************************************/
void
stb_addstbck(ptstbnode, supmin, supmax, sdnmin, sdnmax, period, setup, hold, index, active, verif, type, origclock)
    stbnode *ptstbnode;
    long supmin;
    long supmax;
    long sdnmin;
    long sdnmax;
    long period;
    long setup;
    long hold;
    char index;
    char active;
    char verif;
    char type;
    ttvevent_list *origclock;
{
    stbck   *ptstbck;

    ptstbck = ptstbnode->CK;
    if (ptstbck == NULL) {
        ptstbck = (stbck *)mbkalloc(sizeof(stbck));
        ptstbnode->CK = ptstbck;
        ptstbck->VERIF = 0;
    }
    ptstbck->NEXT = NULL;
    ptstbck->SUPMAX = supmax;
    ptstbck->SDNMAX = sdnmax;
    ptstbck->SUPMIN = supmin;
    ptstbck->SDNMIN = sdnmin;
    ptstbck->PERIOD = period;
    ptstbck->SETUP = setup;
    ptstbck->HOLD = hold;
    ptstbck->ACCESSMAX = (long)0 ;
    ptstbck->ACCESSMIN = (long)0 ;
    ptstbck->CKINDEX = index;
    ptstbck->ACTIVE = active;
    ptstbck->VERIF |= verif;
    ptstbck->TYPE = type;
    ptstbck->CMD = NULL ;
    ptstbck->ORIGINAL_CLOCK = origclock ;
    ptstbck->CTKCKINDEX = index;
    ptstbck->FLAGS=0;
}

/*****************************************************************************
*                           fonction stb_chainstbck()                        *
*****************************************************************************/
void
stb_chainstbck(stbnode *ptstbnode, long supmin, long supmax, long sdnmin, long sdnmax, long period, long setup, long hold, char index, char active, char verif, char type, ttvevent_list *cmd, ttvevent_list *original_clock)
{
    stbck   *ptstbck;

    for( ptstbck = ptstbnode->CK ; ptstbck ; ptstbck = ptstbck->NEXT ) {
      if( ptstbck->CMD == cmd ) {
        ptstbck->SUPMAX = supmax;
        ptstbck->SDNMAX = sdnmax;
        ptstbck->SUPMIN = supmin;
        ptstbck->SDNMIN = sdnmin;
        return ;
      }
    }
    
    ptstbck = (stbck *)mbkalloc(sizeof(stbck));
 
    ptstbck->NEXT = ptstbnode->CK;
    ptstbnode->CK = ptstbck;
    ptstbck->SUPMAX = supmax;
    ptstbck->SDNMAX = sdnmax;
    ptstbck->SUPMIN = supmin;
    ptstbck->SDNMIN = sdnmin;
    ptstbck->PERIOD = period;
    ptstbck->SETUP = setup;
    ptstbck->HOLD = hold;
    ptstbck->ACCESSMAX = (long)0 ;
    ptstbck->ACCESSMIN = (long)0 ;
    ptstbck->CKINDEX = index;
    ptstbck->ACTIVE = active;
    ptstbck->VERIF = verif;
    ptstbck->TYPE = type;
    ptstbck->CMD = cmd ;
    ptstbck->ORIGINAL_CLOCK = original_clock ;
    ptstbck->CTKCKINDEX = index;
    ptstbck->FLAGS=0;
}

/*****************************************************************************
*                           fonction stb_delstbck()                          *
*****************************************************************************/
void
stb_delstbck(ptstbnode)
    stbnode *ptstbnode;
{
    mbkfree(ptstbnode->CK);
    ptstbnode->CK = NULL;
}

/*****************************************************************************
*                           fonction stb_getstbck()                          *
*****************************************************************************/
void
stb_getstbck(ptstbnode,cmd)
    stbnode *ptstbnode;
    ttvevent_list *cmd ;
{
    stbck   *ptstbck;
    stbck   *ptstbsav ;

  for(ptstbck = ptstbnode->CK ; ptstbck != NULL ; ptstbck = ptstbck->NEXT)
   {
    if(ptstbck->CMD == cmd)
      break ;
    ptstbsav = ptstbck ;
   }

  if((ptstbck != NULL) && (ptstbck != ptstbnode->CK))
    {
     ptstbsav->NEXT = ptstbck->NEXT ;
     ptstbck->NEXT = ptstbnode->CK ;
     ptstbnode->CK = ptstbck ;
    }
}

stbck *stb_findstbck(stbnode *ptstbnode,ttvevent_list *cmd)
{
  stbck   *ptstbck;

  for(ptstbck = ptstbnode->CK ; ptstbck != NULL ; ptstbck = ptstbck->NEXT)
   {
    if(ptstbck->CMD == cmd) break;
   }

  return ptstbck;
}

/*****************************************************************************
*                           fonction stb_addstbfig()                         *
*****************************************************************************/
stbfig_list *
stb_addstbfig(ptttvfig)
    ttvfig_list *ptttvfig;
{
    stbfig_list *ptfig;
    stbfig_list *ptnewfig;
    stbfig_list *ptdelfig = NULL;
    ttvfig_list *pttestttvfig;
    ptype_list  *ptuser;
    chain_list  *ptchain;
    chain_list  *headchain;

    headchain = ttv_getttvfiglist(ptttvfig);
    for (ptchain = headchain; ptchain; ptchain = ptchain->NEXT) {
        pttestttvfig = (ttvfig_list *)ptchain->DATA;
        if ((ptuser = getptype(pttestttvfig->USER, STB_FIGURE)) != NULL) {
            ptdelfig = (stbfig_list *)ptuser->DATA;
            break;
        }
    }
    freechain( headchain);
    if (ptdelfig != NULL) {
        for (ptfig = HEAD_STBFIG; ptfig; ptfig = ptfig->NEXT) {
            for (ptchain = ptfig->INSTANCE; ptchain; ptchain = ptchain->NEXT) {
                if (ptttvfig == (ttvfig_list *)ptchain->DATA) {
                    ptdelfig = ptfig;
                    break;
                }
            }
            if (ptdelfig != NULL) break;
        }
    }

    if (ptdelfig != NULL) stb_delstbfig(ptdelfig);

    ptnewfig = (stbfig_list *)mbkalloc(sizeof(stbfig_list));
    ptnewfig->NEXT = HEAD_STBFIG;
    ptnewfig->FIG = ptttvfig;
    ptnewfig->INSTANCE = NULL;
    ptnewfig->CONNECTOR = NULL;
    ptnewfig->MEMORY = NULL;
    ptnewfig->COMMAND = NULL;
    ptnewfig->PRECHARGE = NULL;
    ptnewfig->BREAK = NULL;
    ptnewfig->NODE = NULL;
    ptnewfig->CLOCK = NULL;
    ptnewfig->PRIOCLOCK = NULL;
    ptnewfig->CKDOMAIN = NULL;
    ptnewfig->SETUP = 0;
    ptnewfig->HOLD = 0;
    ptnewfig->DISABLE = NULL ;
    ptnewfig->CLOCKPERIOD = 0;
    ptnewfig->PHASENUMBER = 1;
    ptnewfig->CHANGEFLAG = 0;
    ptnewfig->STABILITYFLAG = 0;
    ptnewfig->STABILITYMODE = STB_STABILITY_BEST ;
    ptnewfig->CTKMODE = STB_CTK_NOT ;
    ptnewfig->ANALYSIS = 0;
    ptnewfig->GRAPH = 0;
    ptnewfig->USER = NULL;
    ptnewfig->FLAGS = 0;
    CreateHeap(sizeof(stb_clock_tree), 4096, &ptnewfig->CLOCK_TREE_HEAP);
//    CreateHeap(sizeof(stb_directive), 4096, &ptnewfig->DIRECTIVE_HEAP);

    HEAD_STBFIG = ptnewfig;
    ptttvfig->USER = addptype(ptttvfig->USER, STB_FIGURE, ptnewfig);

    return ptnewfig;
}

/*****************************************************************************
*                           fonction stb_getstbfig()                         *
*****************************************************************************/
stbfig_list *
stb_getstbfig(ptttvfig)
    ttvfig_list *ptttvfig;
{
    ptype_list  *ptuser;
    
    if ((ptuser = getptype(ptttvfig->USER, STB_FIGURE)) != NULL) {
        return (stbfig_list *)ptuser->DATA;
    }
    return NULL;
}

/*****************************************************************************
*                           fonction stb_delstbfig()                         *
*****************************************************************************/
int
stb_delstbfig(ptstbfig)
    stbfig_list *ptstbfig;
{
    chain_list     *ptchain;
    stbfig_list    *ptfig;
    ttvsig_list    *ptttvsig;
    ttvevent_list  *tve;
	stbdomain_list *ptdomain, *pttodel ;
    ptype_list     *ptuser;
    int             i;

    if (ptstbfig == HEAD_STBFIG) {
        HEAD_STBFIG = HEAD_STBFIG->NEXT;
    }
    else {
        for (ptfig = HEAD_STBFIG; ptfig; ptfig = ptfig->NEXT) {
            if (ptfig->NEXT == ptstbfig) break;
        }
        if (ptfig == NULL) return 1;
        else ptfig->NEXT = ptstbfig->NEXT;
    }

    stb_ctk_clean_stat( ptstbfig );

    for(ptchain = ptstbfig->NODE; ptchain ; ptchain = ptchain->NEXT)
    {
      tve = (ttvevent_list *)ptchain->DATA;
      stb_free_clock_tree_info(tve);
      stb_delaymargins_freeval(tve);
      tve->USER=testanddelptype(tve->USER, STB_BREAK_TEST_EVENT);
//      tve->ROOT->USER=testanddelptype(tve->ROOT->USER, STB_DIRECTIVES);
      tve->ROOT->USER=testanddelptype(tve->ROOT->USER, STB_TRANSITION_PROBABILITY);
      tve->USER=testanddelptype (tve->USER, STB_NODE_CLOCK);
      if ((ptuser=getptype(tve->USER, STB_ONE_OR_NO_CLOCK_EVENT))!=NULL)
      {
        freechain((chain_list *)ptuser->DATA);
        tve->USER=delptype (tve->USER, STB_ONE_OR_NO_CLOCK_EVENT);
      }
    }

    stb_clean_relax_correction_info(ptstbfig);

    for (ptchain = ptstbfig->CONNECTOR; ptchain; ptchain = ptchain->NEXT) {
        ptttvsig = (ttvsig_list *)ptchain->DATA;
        stb_delstbnode(ptttvsig->NODE, ptstbfig->PHASENUMBER);
        stb_delstbnode(ptttvsig->NODE + 1, ptstbfig->PHASENUMBER);
    }
    freechain(ptstbfig->CONNECTOR);
    for (ptchain = ptstbfig->MEMORY; ptchain; ptchain = ptchain->NEXT) {
        ptttvsig = (ttvsig_list *)ptchain->DATA;
        stb_delstbnode(ptttvsig->NODE, ptstbfig->PHASENUMBER);
        stb_delstbnode(ptttvsig->NODE + 1, ptstbfig->PHASENUMBER);
    }
    freechain(ptstbfig->MEMORY);
    for (ptchain = ptstbfig->COMMAND; ptchain; ptchain = ptchain->NEXT) {
        ptttvsig = (ttvsig_list *)ptchain->DATA;
        stb_delstbnode(ptttvsig->NODE, ptstbfig->PHASENUMBER);
        stb_delstbnode(ptttvsig->NODE + 1, ptstbfig->PHASENUMBER);
    }
    freechain(ptstbfig->COMMAND);
    for (ptchain = ptstbfig->PRECHARGE; ptchain; ptchain = ptchain->NEXT) {
        ptttvsig = (ttvsig_list *)ptchain->DATA;
        stb_delstbnode(ptttvsig->NODE, ptstbfig->PHASENUMBER);
        stb_delstbnode(ptttvsig->NODE + 1, ptstbfig->PHASENUMBER);
    }
    freechain(ptstbfig->PRECHARGE);
    for (ptchain = ptstbfig->BREAK; ptchain; ptchain = ptchain->NEXT) {
        ptttvsig = (ttvsig_list *)ptchain->DATA;
        stb_delstbnode(ptttvsig->NODE, ptstbfig->PHASENUMBER);
        stb_delstbnode(ptttvsig->NODE + 1, ptstbfig->PHASENUMBER);
    }
    freechain(ptstbfig->BREAK);
    for (ptchain = ptstbfig->PRIOCLOCK; ptchain; ptchain = ptchain->NEXT) {
        mbkfree(ptchain->DATA) ;
    }
    freechain(ptstbfig->PRIOCLOCK);
    for (ptchain = ptstbfig->NODE; ptchain; ptchain = ptchain->NEXT) {
        stb_delstbnode((ttvevent_list *)ptchain->DATA, ptstbfig->PHASENUMBER);
    }
    freechain(ptstbfig->NODE);
    for (ptchain = ptstbfig->CLOCK; ptchain; ptchain = ptchain->NEXT) {
        ptttvsig = (ttvsig_list *)ptchain->DATA;
        if (getptype(ptttvsig->USER, STB_DOMAIN) != NULL) {
            ptttvsig->USER = delptype(ptttvsig->USER, STB_DOMAIN);
        }
        if (getptype(ptttvsig->USER, STB_EQUIVALENT) != NULL) {
            ptttvsig->USER = delptype(ptttvsig->USER, STB_EQUIVALENT);
        }
        if ((ptuser=getptype(ptttvsig->USER, STB_IS_CLOCK)) != NULL) {
           mbkfree(ptuser->DATA);
           ptttvsig->USER = delptype(ptttvsig->USER, STB_IS_CLOCK);
        }
        if ((ptuser=getptype(ptttvsig->USER, STB_CLOCK_LOCAL_LATENCY)) != NULL) {
           mbkfree(ptuser->DATA);
           ptttvsig->USER = delptype(ptttvsig->USER, STB_CLOCK_LOCAL_LATENCY);
        }
        if (getptype(ptttvsig->USER, STB_IDEAL_CLOCK) != NULL) {
            ptttvsig->USER = delptype(ptttvsig->USER, STB_IDEAL_CLOCK);
        }
        stb_delstbnode(ptttvsig->NODE, ptstbfig->PHASENUMBER);
        stb_delstbnode(ptttvsig->NODE + 1, ptstbfig->PHASENUMBER);
        if (getptype(ptttvsig->USER, STB_VIRTUAL_CLOCK) != NULL) {
            ptttvsig->USER = delptype(ptttvsig->USER, STB_VIRTUAL_CLOCK);
            mbkfree(ptttvsig); // rajouter artificiellement
        }
    }
    freechain(ptstbfig->CLOCK);

	ptdomain = ptstbfig->CKDOMAIN ;
	while (ptdomain) {
		pttodel = ptdomain ;
		ptdomain = ptdomain->NEXT ;
		mbkfree(pttodel) ;
	}
	
    if ((ptuser = getptype(ptstbfig->USER, STB_DOMAIN)) != NULL) {
        for (ptchain = (chain_list *)ptuser->DATA; ptchain; ptchain = ptchain->NEXT) {
            freechain((chain_list *)ptchain->DATA);
        }
        freechain((chain_list *)ptuser->DATA);
        ptstbfig->USER = delptype(ptstbfig->USER, STB_DOMAIN);
    }
    if ((ptuser = getptype(ptstbfig->USER, STB_EQUIVALENT)) != NULL) {
        for (ptchain = (chain_list *)ptuser->DATA; ptchain; ptchain = ptchain->NEXT) {
            freechain((chain_list *)ptchain->DATA);
        }
        freechain((chain_list *)ptuser->DATA);
        ptstbfig->USER = delptype(ptstbfig->USER, STB_EQUIVALENT);
    }

    if (ptstbfig->DISABLE != NULL) {
       for (i = 0; i < ptstbfig->PHASENUMBER; i++) {
           mbkfree(ptstbfig->DISABLE[i]);
       }
       mbkfree(ptstbfig->DISABLE);
    }
    if (getptype (ptstbfig->FIG->USER, STB_FIGURE))
        ptstbfig->FIG->USER = delptype (ptstbfig->FIG->USER, STB_FIGURE);

    DeleteHeap(&ptstbfig->CLOCK_TREE_HEAP);
//    DeleteHeap(&ptstbfig->DIRECTIVE_HEAP);

    stb_freefalseslack(ptstbfig);

    mbkfree(ptstbfig);

    
    return 0;
}


/*****************************************************************************
*                           fonction stb_addstbdomain()                      *
*****************************************************************************/

stbdomain_list *
stb_addstbdomain (stbdomain_list *head, char min, char max)
{
	stbdomain_list *ptdomain = (stbdomain_list *)mbkalloc (sizeof (struct stbdomain)) ;
	ptdomain->CKMIN = min ;
	ptdomain->CKMAX = max ;
	ptdomain->NEXT = head ;

	return (ptdomain) ;
}

/*****************************************************************************
*                           fonction stb_getstbnode()                        *
*****************************************************************************/
stbnode *
stb_getstbnode(ptnode)
    ttvevent_list *ptnode;
{
    ptype_list    *ptuser;

    if ((ptuser = getptype(ptnode->USER, STB_NODE)) != NULL) {
        return (stbnode *)ptuser->DATA;
    }
    else return NULL;
}

/*****************************************************************************
*                           fonction stb_dupstbpairlist()                    *
*****************************************************************************/
stbpair_list *
stb_dupstbpairlist(ptheadlist)
    stbpair_list  *ptheadlist;
{
    stbpair_list  *ptstbpair;
    stbpair_list  *ptnewlist = NULL;

    for (ptstbpair = ptheadlist; ptstbpair; ptstbpair = ptstbpair->NEXT) {
        ptnewlist = stb_addstbpair_and_phase(ptnewlist, ptstbpair->D, ptstbpair->U, ptstbpair->phase_D, ptstbpair->phase_U);
    }
    ptnewlist = (stbpair_list *)reverse((chain_list *)ptnewlist);

    return ptnewlist;
}

void stb_assign_phase_to_stbpair(stbpair_list  *ptheadlist, unsigned char phase)
{
  stbpair_list  *ptstbpair;
  for (ptstbpair = ptheadlist; ptstbpair; ptstbpair = ptstbpair->NEXT) 
  {
    ptstbpair->phase_U=ptstbpair->phase_D=phase;
  }
}
/*****************************************************************************
*                             fonction stb_revstbpairlist()                  *
*****************************************************************************/

stbpair_list *stb_revstbpairlist (stbpair_list *head)
{
    stbpair_list *p;
    stbpair_list *q = NULL;
    
    if (!head) 
        return NULL; 
    
    while ((p = head->NEXT)) { 
        head->NEXT = q; 
        q = head; 
        head = p; 
    } 
    head->NEXT = q; 
    return head;
}


/*****************************************************************************
*                           fonction stb_getsigfromlist()                    *
*****************************************************************************/
chain_list *
stb_getsigfromlist(ptfig, ptlist, name)
    ttvfig_list      *ptfig;
    chain_list       *ptlist;
    char             *name;
{
    ttvsig_list      *ptttvsig;
    chain_list       *ptchain, *cl=NULL;
    char              namebuf[1024];

    for (ptchain = ptlist; ptchain; ptchain = ptchain->NEXT) {
        ptttvsig = (ttvsig_list *)ptchain->DATA;
        if (mbk_TestREGEX(ttv_getnetname(ptfig, namebuf, ptttvsig), name)
            || mbk_TestREGEX(ttv_getsigname(ptfig, namebuf, ptttvsig), name))
            cl=addchain(cl, ptttvsig);
           
//        if (namealloc(ttv_getsigname(ptfig, namebuf, ptttvsig)) == name) break;
    }
    return cl;
/*    if (ptchain != NULL) return ptttvsig;
    else return NULL;*/
}

/*****************************************************************************
*                           fonction stb_geteqvtck()                         *
*****************************************************************************/
chain_list *
stb_geteqvtck(ptttvsig, ptlist)
    ttvsig_list      *ptttvsig;
    chain_list       *ptlist;
{
    ttvsig_list      *pteqvtsig;
    chain_list       *ptchain;
    chain_list       *reschain = NULL;
    ptype_list       *ptuser;
    char              eqvtindex;

    if ((ptuser = getptype(ptttvsig->USER, STB_EQUIVALENT)) == NULL) return NULL;

    eqvtindex = (char)((long)ptuser->DATA);

    for (ptchain = ptlist; ptchain; ptchain = ptchain->NEXT) {
        pteqvtsig = (ttvsig_list *)ptchain->DATA;
        if (pteqvtsig == ptttvsig) continue;
//        if ((ptuser = getptype(ptttvsig->USER, STB_EQUIVALENT)) == NULL) continue;
        if ((ptuser = getptype(pteqvtsig->USER, STB_EQUIVALENT)) == NULL) continue;
        if ((char)((long)ptuser->DATA) == eqvtindex) {
            reschain = addchain(reschain, pteqvtsig);
        }
    }
    return reschain;
}

/*****************************************************************************
*                           fonction stb_suppresseqvtck()                    *
*****************************************************************************/
static int
stb_morephases(ptttvsignew, ptttvsigold)
    ttvsig_list      *ptttvsignew;
    ttvsig_list      *ptttvsigold;
{
    stbck            *ptstbcknew;
    stbck            *ptstbckold;
    int               newcount = 0;
    int               oldcount = 0;

    if (ptttvsigold == NULL) return TRUE;
    ptstbcknew = stb_getstbnode(ptttvsignew->NODE)->CK;
    ptstbckold = stb_getstbnode(ptttvsigold->NODE)->CK;
    if ((ptstbcknew->ACTIVE & STB_UP) != 0) newcount++;
    if ((ptstbcknew->ACTIVE & STB_DN) != 0) newcount++;
    if ((ptstbckold->ACTIVE & STB_UP) != 0) oldcount++;
    if ((ptstbckold->ACTIVE & STB_DN) != 0) oldcount++;
    
    if (newcount > oldcount) return TRUE;
    else return FALSE;
}

chain_list *
stb_suppresseqvtck(clocklist)
    chain_list       *clocklist;
{
    ttvsig_list      *ptttvsig;
    chain_list       *ptchain;
    chain_list       *reschain = NULL;
    ttvsig_list      *sigtable[128];
    ptype_list       *ptuser;
    long              index;
    int               i;

    for (i=0; i<128; i++) sigtable[i] = NULL;
    for (ptchain = clocklist; ptchain; ptchain = ptchain->NEXT) {
        ptttvsig = (ttvsig_list *)ptchain->DATA;
        if ((ptuser = getptype(ptttvsig->USER, STB_EQUIVALENT)) == NULL) {
            reschain = addchain(reschain, ptttvsig);
        }
        else {
            index = (long)ptuser->DATA;
            if (stb_morephases(ptttvsig, sigtable[(int)index])) {
                sigtable[(int)index] = ptttvsig;
            }
        }
    }
    for (i=0; i<128; i++) {
       if(sigtable[i] != NULL)
        reschain = addchain(reschain, sigtable[i]);
    }
    return reschain;
}

/*****************************************************************************
*                           fonction stb_sortphase()                         *
*****************************************************************************/
static int stb_check_inverted(stbck *ref, stbck *cur)
{
  if (cur->SUPMAX==ref->SDNMAX && cur->SUPMIN==ref->SDNMIN
      && cur->SDNMAX==ref->SUPMAX && cur->SDNMIN==ref->SUPMIN) return 1;
  return 0;
}
char
stb_sortphase(ptstbfig, clocklist)
    stbfig_list      *ptstbfig;
    chain_list       *clocklist;
{
    chain_list       *ptchain, *ptchain1;
    chain_list       *clockchain;
    chain_list       *eqvtchain;
    ttvsig_list      *ptttvsig;
    ttvsig_list      *pteqvtsig;
    ttvevent_list    *ptevent;
    stbdomain_list   *ptdomain;
    chain_list       *domainchain;
    chain_list       *domainclocks;
    stbnode          *ptstbnode, *ptstbnode2;
    stbck            *ptstbck, *refck, *curck;
    phase            *phasetable;
    stb_propagated_clock_to_clock *spctc;
    ptype_list *pt;
    char              index_up, index_dn, swap;
    int               numphase = 0;
    int               totalphase = 0;
    int               i = 0, assigninvert;

    phasetable = (phase *)mbkalloc(2 * stb_countchain(clocklist) * sizeof(phase));

    domainchain = (chain_list *)getptype(ptstbfig->USER, STB_DOMAIN)->DATA;
    for (ptdomain = ptstbfig->CKDOMAIN; ptdomain && domainchain; ptdomain = ptdomain->NEXT) {
        domainclocks = (chain_list *)domainchain->DATA;
        clockchain = stb_suppresseqvtck(domainclocks);
        domainchain = domainchain->NEXT;
        numphase = 0;

        for (ptchain = clockchain; ptchain; ptchain = ptchain->NEXT) {
            ptttvsig = (ttvsig_list *)ptchain->DATA;
            ptstbnode = stb_getstbnode(ptttvsig->NODE);
            ptstbck = ptstbnode->CK;
            if ((pt=getptype(ptttvsig->USER, STB_IS_CLOCK))!=NULL)
            {
              spctc=(stb_propagated_clock_to_clock *)pt->DATA;
              ptstbck = &spctc->original_waveform;
            }

            ptevent = ptttvsig->NODE;
#ifndef PHASEFORALL
            if ((ptstbck->ACTIVE & STB_DN) != 0)
#endif
            {
                phasetable[numphase].EVENT = ptevent;
                phasetable[numphase].TIME = ptstbck->SDNMAX;
                numphase++;
            }

            ptevent = ptttvsig->NODE+1;
#ifndef PHASEFORALL
            if ((ptstbck->ACTIVE & STB_UP) != 0)
#endif
            {
                phasetable[numphase].EVENT = ptevent;
                phasetable[numphase].TIME = ptstbck->SUPMAX;
                numphase++;
            }
        }
       
        ptstbfig->PHASE_DATE[0]=0;
        if( numphase > 0 ) {
            qsort(phasetable, numphase, sizeof(phase), (void *)phasecompare);

            for (i = 0; i < numphase; i++) {
                ptevent = phasetable[i].EVENT;
                ptstbnode = stb_getstbnode(ptevent);
                ptstbnode->CK->CKINDEX = i + totalphase;
                ptstbfig->PHASE_DATE[i + totalphase]=phasetable[i].TIME;
            }
            ptdomain->CKMIN = totalphase;
            ptdomain->CKMAX = totalphase + numphase - 1;

            /* update equivalent clocks */
            for (ptchain = clockchain; ptchain; ptchain = ptchain->NEXT) {
                ptttvsig = (ttvsig_list *)ptchain->DATA;
                if ((eqvtchain = stb_geteqvtck(ptttvsig, domainclocks)) != NULL) {
                    ptstbnode = stb_getstbnode(ptttvsig->NODE);
                    index_dn = ptstbnode->CK->CKINDEX;
                    ptstbnode = stb_getstbnode(ptttvsig->NODE + 1);
                    index_up = ptstbnode->CK->CKINDEX;
                    refck=ptstbnode->CK;
                    if ((pt=getptype(ptttvsig->USER, STB_IS_CLOCK))!=NULL)
                    {
                      spctc=(stb_propagated_clock_to_clock *)pt->DATA;
                      refck = &spctc->original_waveform;
                    }
                    swap=-2;
                    if (getptype(ptttvsig->USER, STB_INVERTED_CLOCK))
                       swap=index_up, index_up=index_dn, index_dn=swap;
                    for (ptchain1 = eqvtchain; ptchain1; ptchain1 = ptchain1->NEXT) {
                        pteqvtsig = (ttvsig_list *)ptchain1->DATA;
                        ptstbnode = stb_getstbnode(pteqvtsig->NODE);
                        ptstbnode2 = stb_getstbnode(pteqvtsig->NODE + 1);                    
                        curck=stb_getstbnode(pteqvtsig->NODE + 1)->CK;
                        assigninvert=0;
                        if ((pt=getptype(pteqvtsig->USER, STB_IS_CLOCK))!=NULL)
                        {
                          spctc=(stb_propagated_clock_to_clock *)pt->DATA;
                          curck = &spctc->original_waveform;
                        }
                        if (stb_check_inverted(refck, curck))
                        {
                          if (swap==-2) assigninvert=1;
                        }
                        else if (getptype(pteqvtsig->USER, STB_INVERTED_CLOCK)!=NULL) assigninvert=1;

                        if (!assigninvert)
                        {
                          ptstbnode->CK->CKINDEX = index_dn;
                          ptstbnode2->CK->CKINDEX = index_up;
                        }
                        else
                        {
                          ptstbnode->CK->CKINDEX = index_up;
                          ptstbnode2->CK->CKINDEX = index_dn;
                        }
                    }
                    freechain(eqvtchain);
                }
            }
        }
        freechain(clockchain);

        totalphase += numphase;
    }

    mbkfree(phasetable);

    if (totalphase == 0) totalphase = 1;

    return (char)totalphase;
}

static int
phasecompare(p1, p2)
    phase *p1;
    phase *p2;
{
    if (p1->TIME > p2->TIME) return 1;
    if (p2->TIME > p1->TIME) return -1;
    return 0;
}

/*****************************************************************************
*                           fonction stb_getphase()                          *
*****************************************************************************/
ttvevent_list *
stb_getphase(ptstbfig, name, edge)
    stbfig_list    *ptstbfig;
    char           *name;
    char            edge;
{
    ttvsig_list    *ptttvsig;
    stbnode        *ptstbnode;
    chain_list     *ptchain;
    ttvevent_list  *ptttvevent = NULL;
    char            namebuf[1024];

    for (ptchain = ptstbfig->CLOCK; ptchain; ptchain = ptchain->NEXT) {
        ptttvsig = (ttvsig_list *)ptchain->DATA;
        if (namealloc(ttv_getsigname(ptstbfig->FIG, namebuf, ptttvsig)) == name) break;
    }
    if (ptchain != NULL) {
        if (edge == STB_SLOPE_DN) {
            ptstbnode = stb_getstbnode(ptttvsig->NODE);
            if (ptstbnode->CK->CKINDEX != STB_NO_INDEX) ptttvevent = ptttvsig->NODE;
        }
        else if (edge == STB_SLOPE_UP) {
            ptstbnode = stb_getstbnode(ptttvsig->NODE+1);
            if (ptstbnode->CK->CKINDEX != STB_NO_INDEX) ptttvevent = ptttvsig->NODE+1;
        }
        else if (edge == STB_SLOPE_ALL) {
            ptstbnode = stb_getstbnode(ptttvsig->NODE);
            if ((ptstbnode->CK->ACTIVE & STB_SLOPE_DN) == STB_SLOPE_DN) {
                ptttvevent = ptttvsig->NODE;
            }
            else if ((ptstbnode->CK->ACTIVE & STB_SLOPE_UP) == STB_SLOPE_UP) {
                ptttvevent = ptttvsig->NODE+1;
            }
        }
    }
    return ptttvevent;
}

ttvevent_list *
stb_getclockevent(ptstbfig, name, edge)
    stbfig_list    *ptstbfig;
    char           *name;
    char            edge;
{
    ttvsig_list    *ptttvsig;
    chain_list     *ptchain;
    ttvevent_list  *ptttvevent = NULL;
    char            namebuf[1024];

    for (ptchain = ptstbfig->CLOCK; ptchain; ptchain = ptchain->NEXT) {
        ptttvsig = (ttvsig_list *)ptchain->DATA;
        if (namealloc(ttv_getsigname(ptstbfig->FIG, namebuf, ptttvsig)) == name) break;
    }
    if (ptchain != NULL) {
        if (edge == STB_SLOPE_DN) {
            ptttvevent = ptttvsig->NODE;
        }
        else if (edge == STB_SLOPE_UP) {
            ptttvevent = ptttvsig->NODE+1;
        }
    }
    return ptttvevent;
}
/*****************************************************************************
*                           fonction stb_getphaseindex()                     *
*****************************************************************************/
char
stb_getphaseindex(ptstbfig, name, edge)
    stbfig_list    *ptstbfig;
    char           *name;
    char            edge;
{
    ttvevent_list  *ptttvevent;

    ptttvevent = stb_getphase(ptstbfig, name, edge);
    if (ptttvevent == NULL) return STB_NO_INDEX;
    else return stb_getstbnode(ptttvevent)->CK->CKINDEX;
}

/*****************************************************************************
*                           fonction stb_getclock()                          *
*****************************************************************************/
stbck *
stb_getclock(stbfig_list  *ptstbfig, char phaseindex, char *namebuf, char *ptedge, stbck *ck)
{
    ttvsig_list  *ptttvsig, *knownsig;
    stbck        *ptstbck;
    chain_list   *ptchain;

    if (ck!=NULL && ck->ORIGINAL_CLOCK!=NULL)
       knownsig=ck->ORIGINAL_CLOCK->ROOT;
    else
       knownsig=NULL;

    if( namebuf ) 
      namebuf[0]='\0';
    
    for (ptchain = ptstbfig->CLOCK; ptchain; ptchain = ptchain->NEXT) {
        ptttvsig = (ttvsig_list *)ptchain->DATA;
        ptstbck = stb_GetClockCK(ptttvsig->NODE, stb_getstbnode(ptttvsig->NODE)->CK);
        if (ptstbck->CKINDEX == phaseindex && (!knownsig || ptttvsig==knownsig)) {
            *ptedge = STB_SLOPE_DN;
            break;
        }
        ptstbck = stb_GetClockCK(ptttvsig->NODE+1, stb_getstbnode(ptttvsig->NODE+1)->CK);
        if (ptstbck->CKINDEX == phaseindex && (!knownsig || ptttvsig==knownsig)) {
            *ptedge = STB_SLOPE_UP;
            break;
        }
    }
    if (ptchain != NULL) {
        if (namebuf != NULL) {
            ttv_getsigname(ptstbfig->FIG, namebuf, ptttvsig);
        }
        return ptstbck;
    }
    else if (knownsig==NULL) return NULL;

    // en cas de bug sur le original clock

    for (ptchain = ptstbfig->CLOCK; ptchain; ptchain = ptchain->NEXT) {
        ptttvsig = (ttvsig_list *)ptchain->DATA;
        ptstbck = stb_getstbnode(ptttvsig->NODE)->CK;
        if (ptstbck->CKINDEX == phaseindex) {
            *ptedge = STB_SLOPE_DN;
            break;
        }
        ptstbck = stb_getstbnode(ptttvsig->NODE+1)->CK;
        if (ptstbck->CKINDEX == phaseindex) {
            *ptedge = STB_SLOPE_UP;
            break;
        }
    }
    if (ptchain != NULL) {
        if (namebuf != NULL) {
            ttv_getsigname(ptstbfig->FIG, namebuf, ptttvsig);
        }
        return ptstbck;
    }
    else return NULL;
}

/*****************************************************************************
*                           fonction stb_getclock()                          *
*****************************************************************************/
stbck *
stb_getclocknode(stbfig_list  *ptstbfig, char phaseindex, char *namebuf, ttvevent_list **ptevent, stbck *ck)
{
    ttvsig_list  *ptttvsig, *knownsig;
    stbck        *ptstbck;
    chain_list   *ptchain;

    if (ck!=NULL && ck->ORIGINAL_CLOCK!=NULL)
       knownsig=ck->ORIGINAL_CLOCK->ROOT;
    else
       knownsig=NULL;

    for (ptchain = ptstbfig->CLOCK; ptchain; ptchain = ptchain->NEXT) {
        ptttvsig = (ttvsig_list *)ptchain->DATA;
        ptstbck = stb_getstbnode(ptttvsig->NODE)->CK;
        if (ptstbck->CKINDEX == phaseindex && (!knownsig || ptttvsig==knownsig)) {
            *ptevent = ptttvsig->NODE;
            break;
        }
        ptstbck = stb_getstbnode(ptttvsig->NODE+1)->CK;
        if (ptstbck->CKINDEX == phaseindex && (!knownsig || ptttvsig==knownsig)) {
            *ptevent = ptttvsig->NODE+1;
            break;
        }
    }
    if (ptchain != NULL) {
        if (namebuf != NULL) {
            ttv_getsigname(ptstbfig->FIG, namebuf, ptttvsig);
        }
        return ptstbck;
    }
    else return NULL;
}

/*****************************************************************************
*                           fonction stb_getperiod()                         *
*****************************************************************************/
long
stb_getperiod(ptstbfig, phaseindex)
    stbfig_list  *ptstbfig;
    char          phaseindex;
{
    stbck        *ptstbck;
    char          ckedge;

    ptstbck = stb_getclock(ptstbfig, phaseindex, NULL, &ckedge, NULL);
    if (ptstbck == NULL) return STB_NO_TIME;
    else return ptstbck->PERIOD;
}

/*****************************************************************************
*                           fonction stb_buildintervals()                    *
*****************************************************************************/
stbpair_list *
stb_buildintervals(ptstablelist, ptunstablelist, clockperiod, errcode)
    chain_list   *ptstablelist;
    chain_list   *ptunstablelist;
    long          clockperiod;
    int          *errcode;
{
    chain_list   *ptchain;
    long         *stabletable;
    long         *unstabletable;
    int           numstable, numunstable;
    long          stable, unstable;
    stbpair_list *ptstbpair = NULL;
    long          savelast;
    int           i;

    numstable = stb_countchain(ptstablelist);
    numunstable = stb_countchain(ptunstablelist);

    if (numstable != numunstable) {
        *errcode = ERR_UNMATCHED_INTERVALS;
        return NULL;
    }

    stabletable = (long *)mbkalloc(numstable * sizeof(long));
    i = 0;
    for (ptchain = ptstablelist; ptchain; ptchain = ptchain->NEXT) {
        stabletable[i++] = (long)ptchain->DATA;
    }
    qsort(stabletable, numstable, sizeof(long), (void *)longcompare);

    unstabletable = (long *)mbkalloc(numunstable * sizeof(long));
    i = 0;
    for (ptchain = ptunstablelist; ptchain; ptchain = ptchain->NEXT) {
        unstabletable[i++] = (long)ptchain->DATA;
    }
    qsort(unstabletable, numstable, sizeof(long), (void *)longcompare);

    if (stabletable[0] < unstabletable[0]) {
		if (!clockperiod) {
			fprintf (stderr, "Please provide ordered unstability intervals when performing combinational analysis\n") ;
			EXIT (0) ;
		} else {
        	unstabletable[numunstable-1] -= clockperiod;
        	if (numunstable > 1) {
            	savelast = unstabletable[numunstable-1];
            	for (i = numunstable-2; i >= 0; i--) {
                	unstabletable[i+1] = unstabletable[i];
            	}
            	stabletable[0] = savelast;
        	}
		}
    }

    for (i = numstable-1; i>=0; i--) {
        unstable = unstabletable[i];
        stable = stabletable[i];
        if (unstable > stable) {
            *errcode = ERR_UNMATCHED_INTERVALS;
            stb_freestbpair(ptstbpair);
            return NULL;
        }
        ptstbpair = stb_addstbpair(ptstbpair, unstable, stable);
    }

    mbkfree(stabletable);
    mbkfree(unstabletable);
    return ptstbpair;
}

static int
longcompare(l1, l2)
    long *l1;
    long *l2;
{
    if (*l1 > *l2) return 1;
    if (*l2 > *l1) return -1;
    return 0;
}

/*****************************************************************************
*                           fonction stb_addintervals()                      *
*****************************************************************************/
int
stb_addintervals(ptstbfig, ptttvsig, ptstablelist, ptunstablelist, table, slope, phaseindex, cktype,nodemode)
    stbfig_list   *ptstbfig;
    ttvsig_list   *ptttvsig;
    chain_list    *ptstablelist;
    chain_list    *ptunstablelist;
    int            table;
    char           slope;
    int            phaseindex;
    char           cktype;
    char           nodemode;
{
    stbnode       *ptstbnode_up;
    stbnode       *ptstbnode_dn;
    stbck         *ptstbck;
    stbpair_list  *ptstbpair;
    int            errcode = 0, used=0;
    char           edge;

    if ((ptstbck = stb_getclock(ptstbfig, (char)phaseindex, NULL, &edge, NULL)))
    	ptstbpair = stb_buildintervals(ptstablelist, ptunstablelist, ptstbck->PERIOD, &errcode);
	else
    	ptstbpair = stb_buildintervals(ptstablelist, ptunstablelist, 0, &errcode);

    if (errcode != 0) {
        return errcode;
    }

    ptstbnode_dn = stb_getstbnode(ptttvsig->NODE);
    ptstbnode_up = stb_getstbnode(ptttvsig->NODE+1);

    switch (table) {
    case STB_TABLE:
        if (slope == STB_SLOPE_UP) {
            if (ptstbnode_up->STBTAB==NULL) ptstbnode_up->STBTAB = stb_alloctab(ptstbfig->PHASENUMBER);
            stb_freestbpair(ptstbnode_up->STBTAB[phaseindex]);
            ptstbnode_up->STBTAB[phaseindex] = ptstbpair;
        }
        else if (slope == STB_SLOPE_DN) {
            if (ptstbnode_dn->STBTAB==NULL) ptstbnode_dn->STBTAB = stb_alloctab(ptstbfig->PHASENUMBER);
            stb_freestbpair(ptstbnode_dn->STBTAB[phaseindex]);
            ptstbnode_dn->STBTAB[phaseindex] = ptstbpair;
        }
        else {
            if (ptstbnode_up->STBTAB==NULL) ptstbnode_dn->STBTAB = stb_alloctab(ptstbfig->PHASENUMBER);
            if (ptstbnode_dn->STBTAB==NULL) ptstbnode_up->STBTAB = stb_alloctab(ptstbfig->PHASENUMBER);
            stb_freestbpair(ptstbnode_up->STBTAB[phaseindex]);
            stb_freestbpair(ptstbnode_dn->STBTAB[phaseindex]);
            ptstbnode_dn->STBTAB[phaseindex] = ptstbpair;
            ptstbnode_up->STBTAB[phaseindex] = stb_dupstbpairlist(ptstbpair);
        }
        used=1;
        break;

    case STB_THZ:
        if (slope == STB_SLOPE_UP) {
            ptstbnode_up->STBHZ = stb_alloctab(ptstbfig->PHASENUMBER);
            ptstbnode_up->STBHZ[phaseindex] = ptstbpair;
            if (cktype != 0) ptstbnode_up->CK->TYPE = cktype;
        }
        else if (slope == STB_SLOPE_DN) {
            ptstbnode_dn->STBHZ = stb_alloctab(ptstbfig->PHASENUMBER);
            ptstbnode_dn->STBHZ[phaseindex] = ptstbpair;
            if (cktype != 0) ptstbnode_dn->CK->TYPE = cktype;
        }
        else {
            ptstbnode_dn->STBHZ = stb_alloctab(ptstbfig->PHASENUMBER);
            ptstbnode_up->STBHZ = stb_alloctab(ptstbfig->PHASENUMBER);
            ptstbnode_dn->STBHZ[phaseindex] = ptstbpair;
            ptstbnode_up->STBHZ[phaseindex] = stb_dupstbpairlist(ptstbpair);
            if (cktype != 0) {
                ptstbnode_dn->CK->TYPE = cktype;
                ptstbnode_up->CK->TYPE = cktype;
            }
        }
        used=1;
        break;

    case STB_SPECIN:
        if (slope == STB_SLOPE_UP) {
            if (ptstbnode_up->SPECIN==NULL)
            {
             ptstbnode_up->SPECIN = stb_alloctab(ptstbfig->PHASENUMBER);
             ptstbnode_up->SPECIN[phaseindex] = ptstbpair;
            used=1;
            }
        }
        else if (slope == STB_SLOPE_DN) {
            if (ptstbnode_dn->SPECIN==NULL)
            {
             ptstbnode_dn->SPECIN = stb_alloctab(ptstbfig->PHASENUMBER);
             ptstbnode_dn->SPECIN[phaseindex] = ptstbpair;
            used=1;
            }
        }
        else {
            if (ptstbnode_up->SPECIN==NULL)
            {
            ptstbnode_up->SPECIN = stb_alloctab(ptstbfig->PHASENUMBER);
            ptstbnode_up->SPECIN[phaseindex] = stb_dupstbpairlist(ptstbpair);
            used=1;
            }
            if (ptstbnode_dn->SPECIN==NULL)
            {
            ptstbnode_dn->SPECIN = stb_alloctab(ptstbfig->PHASENUMBER);
            ptstbnode_dn->SPECIN[phaseindex] = ptstbpair;
            used=1;
            }
        }
        break;

    case STB_SPECOUT:
        if (slope == STB_SLOPE_UP) {
            if (nodemode==0 || ptstbnode_up->SPECOUT==NULL)
              ptstbnode_up->SPECOUT = stb_alloctab(ptstbfig->PHASENUMBER);
            stb_freestbpair(ptstbnode_up->SPECOUT[phaseindex]);
            ptstbnode_up->SPECOUT[phaseindex] = ptstbpair;
        }
        else if (slope == STB_SLOPE_DN) {
            if (nodemode==0 || ptstbnode_dn->SPECOUT==NULL)
              ptstbnode_dn->SPECOUT = stb_alloctab(ptstbfig->PHASENUMBER);
            stb_freestbpair(ptstbnode_dn->SPECOUT[phaseindex]);
            ptstbnode_dn->SPECOUT[phaseindex] = ptstbpair;
        }
        else {
            if (nodemode==0 || ptstbnode_dn->SPECOUT==NULL)
               ptstbnode_dn->SPECOUT = stb_alloctab(ptstbfig->PHASENUMBER);
            if (nodemode==0 || ptstbnode_up->SPECOUT==NULL)
               ptstbnode_up->SPECOUT = stb_alloctab(ptstbfig->PHASENUMBER);
            stb_freestbpair(ptstbnode_dn->SPECOUT[phaseindex]);
            stb_freestbpair(ptstbnode_up->SPECOUT[phaseindex]);
            ptstbnode_dn->SPECOUT[phaseindex] = ptstbpair;
            ptstbnode_up->SPECOUT[phaseindex] = stb_dupstbpairlist(ptstbpair);
        }
        used=1;
        break;
    }
    if (!used) stb_freestbpair(ptstbpair);
    return 0;
}

/*****************************************************************************
*                           fonction stb_countchain()                        *
*****************************************************************************/
int
stb_countchain(ptchain)
    chain_list   *ptchain;
{
    int           i = 0;

    for (; ptchain; ptchain = ptchain->NEXT) i++;

    return i;
}

/*****************************************************************************
*                           fonction stb_viewstbnode()                       *
*****************************************************************************/
void
stb_viewstbnode(ptstbfig, ptevent)
    stbfig_list   *ptstbfig;
    ttvevent_list *ptevent;
{
    stbnode       *ptstbnode;
    char           namebuf[1024];

    ttv_getsigname(ptstbfig->FIG,namebuf,ptevent->ROOT);
    if ((ptevent->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN) {
        printf("    %s FALLING\n",namebuf);
    }
    else {
        printf("    %s RISING\n",namebuf);
    }
    ptstbnode = stb_getstbnode(ptevent);
    stb_viewstbtab(ptstbnode->STBTAB, (int)ptstbnode->NBINDEX);
    stb_viewstbtab(ptstbnode->STBHZ, (int)ptstbnode->NBINDEX);
    fflush(stdout);
}

/*****************************************************************************
*                           fonction stb_viewstbtab()                        *
*****************************************************************************/
void
stb_viewstbtab(ptstbtab, size)
    stbpair_list **ptstbtab;
    int            size;
{
    int            i;

    if (ptstbtab == NULL) return;
    for (i = 0; i < size; i++) {
        if (ptstbtab[i] != NULL) {
            printf("        From phase %d\n", i);
            stb_viewstbpairlist(ptstbtab[i]);
        }
    }
    fflush(stdout);
}

/*****************************************************************************
*                           fonction stb_viewstbpairlist()                   *
*****************************************************************************/
void
stb_viewstbpairlist(ptstbpairlist)
    stbpair_list *ptstbpairlist;
{
    stbpair_list  *ptstbpair;
    long           prev_u;
    int            numintervals = 0;


    if (ptstbpairlist) prev_u = ptstbpairlist->D - 1;
    for (ptstbpair = ptstbpairlist; ptstbpair; ptstbpair = ptstbpair->NEXT) {
        printf("            D = %ld, U = %ld\n", ptstbpair->D, ptstbpair->U);
        if (ptstbpair->D <= prev_u || ptstbpair->U <= ptstbpair->D) {
            printf("****** BAD INTERVAL LIST ******\n");
        }
        prev_u = ptstbpair->U;
        numintervals++;
    }
    printf("            Number of intervals = %d\n", numintervals);
}

/*****************************************************************************
*                           fonction stb_getstbdomain ()                     *
*****************************************************************************/
/* les stbdomain sont supposes tries dans l'ordre croissant                 */

stbdomain_list *
stb_getstbdomain (stbdomain_list *domain, char index)
{
	stbdomain_list *ptdomain = domain ;
	
	while (ptdomain) {
		if (ptdomain->CKMAX >= index && ptdomain->CKMAX != STB_NO_INDEX )
			return (ptdomain) ;
		ptdomain = ptdomain->NEXT ;
	}

	return (NULL) ;
}

/*****************************************************************************
*                           fonction stb_adddisable ()                       *
*****************************************************************************/
/* suppression de certains chemins                                          */

void
stb_adddisable(stbfig, from, to)
    stbfig_list *stbfig;
    char         from;
    char         to;
{
    int i ;
    int j ;

    if ((stbfig->PHASENUMBER == 0) 
    || (from >= stbfig->PHASENUMBER) || (to >= stbfig->PHASENUMBER)) {
        return;
    }

    /* Initialise the disable table if necessary */
    if (stbfig->DISABLE == NULL) {
        stbfig->DISABLE = (char **)mbkalloc(stbfig->PHASENUMBER * sizeof(char *));
        for(i = 0 ; i < stbfig->PHASENUMBER ; i++) {
            stbfig->DISABLE[i] = (char *)mbkalloc(stbfig->PHASENUMBER * sizeof(char));
            for(j = 0 ; j < stbfig->PHASENUMBER ; j++) {
                stbfig->DISABLE[i][j] = (char)FALSE;
            }
        }
    }
 
    if (from != STB_NO_INDEX && to != STB_NO_INDEX) { /* one phase to one phase */
        stbfig->DISABLE[(int)from][(int)to] = (char)TRUE;
    }
    else if (from != STB_NO_INDEX) { /* one phase to every phase */
        for (j = 0; j < stbfig->PHASENUMBER; j++) {
            stbfig->DISABLE[(int)from][j] = (char)FALSE;
        }
    }
    else if (to != STB_NO_INDEX) { /* every phase to one phase */
        for (i = 0; i < stbfig->PHASENUMBER; i++) {
            stbfig->DISABLE[i][(int)to] = (char)FALSE;
        }
    }
}

/*****************************************************************************
*                           fonction stb_isdisable ()                        *
*****************************************************************************/
/* le chemin est il supprimer                                               */
    
int stb_isdisable(stbfig,from,to)
   stbfig_list *stbfig ;
   char from ;
   char to ;
{
 
 if (stbfig->DISABLE == NULL) {
  return (FALSE) ;
 }
 else {
  return ((int)stbfig->DISABLE[(int)from][(int)to]) ;
 }
}

/*****************************************************************************
*                           fonction stb_ckdisable ()                        *
*****************************************************************************/
/* add disable between all active phases of given clocks */
void
stb_ckdisable(stbfig, ckfrom, ckto)
    stbfig_list *stbfig;
    char        *ckfrom;
    char        *ckto;
{
    char         phasefrom;
    char         phaseto;

    if (ckfrom != NULL && ckto != NULL) {
        if ((phasefrom = stb_getphaseindex(stbfig, ckfrom, STB_SLOPE_UP)) != STB_NO_INDEX
        && (phaseto = stb_getphaseindex(stbfig, ckto, STB_SLOPE_UP)) != STB_NO_INDEX) {
            stb_adddisable(stbfig, phasefrom, phaseto);
        }    
        if ((phasefrom = stb_getphaseindex(stbfig, ckfrom, STB_SLOPE_UP)) != STB_NO_INDEX
        && (phaseto = stb_getphaseindex(stbfig, ckto, STB_SLOPE_DN)) != STB_NO_INDEX) {
            stb_adddisable(stbfig, phasefrom, phaseto);
        }
        if ((phasefrom = stb_getphaseindex(stbfig, ckfrom, STB_SLOPE_DN)) != STB_NO_INDEX
        && (phaseto = stb_getphaseindex(stbfig, ckto, STB_SLOPE_UP)) != STB_NO_INDEX) {
            stb_adddisable(stbfig, phasefrom, phaseto);
        }
        if ((phasefrom = stb_getphaseindex(stbfig, ckfrom, STB_SLOPE_DN)) != STB_NO_INDEX
        && (phaseto = stb_getphaseindex(stbfig, ckto, STB_SLOPE_DN)) != STB_NO_INDEX) {
            stb_adddisable(stbfig, phasefrom, phaseto);
        }
    }
    else if (ckfrom != NULL) {
        if ((phasefrom = stb_getphaseindex(stbfig, ckfrom, STB_SLOPE_UP)) != STB_NO_INDEX) {
            stb_adddisable(stbfig, phasefrom, STB_NO_INDEX);
        }    
        if ((phasefrom = stb_getphaseindex(stbfig, ckfrom, STB_SLOPE_DN)) != STB_NO_INDEX) {
            stb_adddisable(stbfig, phasefrom, STB_NO_INDEX);
        }
    }
    else if (ckto != NULL) {
        if ((phaseto = stb_getphaseindex(stbfig, ckto, STB_SLOPE_UP)) != STB_NO_INDEX) {
            stb_adddisable(stbfig, STB_NO_INDEX, phaseto);
        }    
        if ((phaseto = stb_getphaseindex(stbfig, ckto, STB_SLOPE_DN)) != STB_NO_INDEX) {
            stb_adddisable(stbfig, STB_NO_INDEX, phaseto);
        }
    }
}

/*****************************************************************************
*                           fonction stb_init_var ()                         *
*****************************************************************************/
void stb_init_var()
{
    STB_MODE_VAR = STB_STABILITY_LT|STB_STABILITY_BEST|STB_STABILITY_SETUP|STB_STABILITY_HOLD ;
    STB_CTK_VAR = STB_CTK_NOT|STB_CTK_OBSERVABLE ;
    STB_ANALYSIS_VAR = STB_GLOB_ANALYSIS;
    STB_GRAPH_VAR    = STB_RED_GRAPH;
}


/*****************************************************************************
*                           fonction stb_calperiode()                        *
*****************************************************************************/
/* fonction qui test L<O<=R en modulo */
static inline int greater_lowerequal(char L, char O, char R)
{
  if (R>L) return L<O && O<=R;
  return O>L || O<=R;
}

long stb_calperiode(stbfig_list *stbfig, char u, stbnode *node, ptype_list *phaselatch, char phaseinit, long periode)
{
  ptype_list *pt;
  char last_phase=u, phase;
  long period=0;
  stbnode *te;

  for (pt=phaselatch; pt!=NULL; pt=pt->NEXT)
    {
      te=stb_getstbnode((ttvevent_list *)pt->DATA);
      phase=(char)pt->TYPE;
      if (phase==last_phase && (stbfig->STABILITYMODE & STB_STABILITY_LT)!=0) continue;
      if (greater_lowerequal(last_phase, 0, phase)) period+=te->CK->PERIOD;
      last_phase=phase;
    }
  if (node->CK!=NULL)
//    if(!(last_phase==phaseinit && node->CK!=NULL && node->CK->TYPE == STB_TYPE_LATCH && (stbfig->STABILITYMODE & STB_STABILITY_LT) != 0))
    if (!(last_phase==phaseinit) 
    || !(node->CK->TYPE == STB_TYPE_LATCH || node->CK->TYPE == STB_TYPE_EVAL || node->CK->TYPE == STB_TYPE_PRECHARGE)
    || (stbfig->STABILITYMODE & STB_STABILITY_LT) == 0)
      {
        if (greater_lowerequal(last_phase, 0, phaseinit))
          period+=te->CK->PERIOD;
      }

  return period-periode;
}

void stb_getmulticycleperiod(ttvevent_list *inpute, ttvevent_list *outpute, long inputperiod, long outputperiod, long *setupP, long *holdP, int *nb_setup_cycle, int *nb_hold_cycle)
{
  ttv_MultiCycleInfo *tmi;
  chain_list *cl, *outlist, *inlist, *ch;
  long outdirmask, sp, hp;
  ptype_list *pt;

  sp=LONG_MIN;
  hp=LONG_MIN;
  *nb_setup_cycle=*nb_hold_cycle=0;
          
  if ((pt=getptype(outpute->ROOT->USER, TTV_SIG_MULTICYCLE_OUT))!=NULL) outlist=(chain_list *)pt->DATA;
  else outlist=NULL;

  if (inpute!=NULL && (pt=getptype(inpute->ROOT->USER, TTV_SIG_MULTICYCLE_IN))!=NULL) inlist=(chain_list *)pt->DATA;
  else inlist=NULL;

  if (outpute->TYPE & TTV_NODE_UP) outdirmask=INF_MULTICYCLE_RISE;
  else outdirmask=INF_MULTICYCLE_FALL;

  for (cl=outlist; (hp==LONG_MIN || sp!=LONG_MIN) && cl!=NULL; cl=cl->NEXT)
    {
      tmi=(ttv_MultiCycleInfo *)cl->DATA;
      if ((tmi->Flags & TTV_MULTICYCLE_ANYINPUT)==0 && inpute==NULL) continue;
      if ((tmi->Flags & TTV_MULTICYCLE_ANYINPUT)!=0)
        ch=(chain_list *)1;
      else
        for (ch=inlist; ch!=NULL && (int)(long)ch->DATA!=tmi->RuleNumber; ch=ch->NEXT) ;
      if (ch!=NULL)
        {
          if (sp==LONG_MIN
              && (tmi->Flags & INF_MULTICYCLE_SETUP)!=0
              && (tmi->Flags & outdirmask)!=0
              ) 
            {
              if ((tmi->Flags & INF_MULTICYCLE_START)!=0 && inputperiod!=0)
                sp=inputperiod*(tmi->Multiplier-1), *nb_setup_cycle=tmi->Multiplier;
              else if ((tmi->Flags & INF_MULTICYCLE_END)!=0 && outputperiod!=0)
                sp=outputperiod*(tmi->Multiplier-1), *nb_setup_cycle=tmi->Multiplier;
            }
          if (hp==LONG_MIN
              && (tmi->Flags & INF_MULTICYCLE_HOLD)!=0
              && (tmi->Flags & outdirmask)!=0
              )
            {
              if ((tmi->Flags & INF_MULTICYCLE_START)!=0 && inputperiod!=0)
                hp=inputperiod*tmi->Multiplier, *nb_hold_cycle=tmi->Multiplier;
              else if ((tmi->Flags & INF_MULTICYCLE_END)!=0 && outputperiod!=0)
                hp=outputperiod*tmi->Multiplier, *nb_hold_cycle=tmi->Multiplier;
            }
        }
    }

  if (sp!=LONG_MIN) *setupP=sp;
  else *setupP=0;
  if (hp!=LONG_MIN) *holdP=hp;
  else *holdP=0;
}

static long stb_gettruncatedaccess_delta(stbck *clock, int max)
{
  long slopeopen, slopeopenmax, slopeclose, delta;
  delta=0;
  if (clock!=NULL)
  {
    if ((clock->ACTIVE & STB_STATE_UP) == STB_STATE_UP)
     {
      if(STB_OPEN_LATCH_PHASE == 'N')
        {
         slopeopen =
          ((clock->SUPMIN <
           clock->SDNMIN) ? clock->SUPMIN : clock->SUPMIN - clock->PERIOD) + 
           clock->ACCESSMIN ;
          slopeopenmax = ((clock->SUPMAX <
             clock->SDNMAX) ? clock->SUPMAX : clock->SUPMAX - clock->PERIOD) + 
             clock->ACCESSMAX ;
          slopeclose = clock->SDNMAX;
         }
      else
         {
          if (!max) slopeopen = clock->SUPMIN + clock->ACCESSMIN ;
          else slopeopen = clock->SUPMAX + clock->ACCESSMAX ;
          slopeclose = ((clock->SUPMAX < clock->SDNMAX) ? 
                  clock->SDNMAX : clock->SDNMAX + clock->PERIOD) ;
         }
      if(slopeopen > slopeclose)
          delta=slopeclose-slopeopen ;
     }
    else
     {
      if(STB_OPEN_LATCH_PHASE == 'N')
        {
         slopeopen =
          ((clock->SDNMIN <
           clock->SUPMIN) ? clock->SDNMIN : clock->SDNMIN - clock->PERIOD) + 
          clock->ACCESSMIN ;
          slopeopenmax = ((clock->SDNMAX <
              clock->SUPMAX) ? clock->SDNMAX : clock->SDNMAX - clock->PERIOD) + 
            clock->ACCESSMAX ;
          slopeclose = clock->SUPMAX;
         }
      else
         {
          if (!max) slopeopen = clock->SDNMIN + clock->ACCESSMIN ;
          else slopeopen = clock->SDNMAX + clock->ACCESSMAX ;
          slopeclose = ((clock->SDNMAX < clock->SUPMAX) ? 
                  clock->SUPMAX : clock->SUPMAX + clock->PERIOD) ;
         }
      if(slopeopen > slopeclose)
          delta=slopeclose-slopeopen ;
     }
  }

  return delta;
}

long stb_gettruncatedaccess(ttvevent_list *latch, ttvevent_list *cmd, int max)        
{
  stbck *ck;
  stbnode *sn;

  sn=stb_getstbnode(latch);
  if (sn==NULL) return TTV_NOTIME;
  for (ck=sn->CK; ck!=NULL && ck->CMD!=cmd; ck=ck->NEXT) ;
  if (ck==NULL) ck=sn->CK;
  if (ck==NULL) return TTV_NOTIME;
  if (max) return ck->ACCESSMAX+stb_gettruncatedaccess_delta(ck, max);
  else return ck->ACCESSMIN+stb_gettruncatedaccess_delta(ck, max);
}


int stb_sigisonclockpath(ttvevent_list *pevent, ttvevent_list *from)
{
  stbck *ck;
  stbnode *sn;

  if (V_INT_TAB[__STB__PRECHARGE_DATA_HEURISTIC].VALUE==2)
  {
/*    sn=stb_getstbnode(pevent);
    if ((sn->FLAG & STB_NODE_DATA_ON_PRECHARGE)!=0)  return 1;
*/                            
    if (from!=NULL)
    {
      sn=stb_getstbnode(from);
      if ((sn->FLAG & STB_NODE_DATA_ON_PRECHARGE)!=0)  return 1;
    }
  }

  if (V_INT_TAB[__STB__PRECHARGE_DATA_HEURISTIC].VALUE==1)
  {
    sn=stb_getstbnode(pevent);
    if (sn!=NULL && sn->CK!=NULL && (sn->CK->FLAGS & STBCK_ONCLOCK_PATH)!=0)
      return 1;
  }
  return 0;
}

void stb_mark_data_on_clock_path(ttvpath_list *pth)
{ 
  ttvsig_list *tvs;
  if (V_INT_TAB[__STB__PRECHARGE_DATA_HEURISTIC].VALUE==2)
  {
    while (pth!=NULL)
    {
      tvs=pth->NODE->ROOT;
      if ((pth->ROOT->ROOT->TYPE & TTV_SIG_R) != 0 && (pth->ROOT->TYPE & TTV_NODE_UP)!=0)
      {
        if (getptype(tvs->USER, STB_DATA_ON_PRECHARGE_NODE)==NULL)
         tvs->USER=addptype(tvs->USER, STB_DATA_ON_PRECHARGE_NODE, NULL);
      }
      pth=pth->NEXT;
    }
  }
}

void stb_free_data_on_clock_path(ttvpath_list *pth)
{ 
  ttvsig_list *tvs;
  if (V_INT_TAB[__STB__PRECHARGE_DATA_HEURISTIC].VALUE==2)
  {
    while (pth!=NULL)
    {
      tvs=pth->NODE->ROOT;
      if ((pth->ROOT->ROOT->TYPE & TTV_SIG_R) != 0)
         tvs->USER=testanddelptype(tvs->USER, STB_DATA_ON_PRECHARGE_NODE);
      pth=pth->NEXT;
    }
  }
}

int stb_checkvalidcommand(ttvevent_list *tve, stbck *inputck)
{
  stbnode *cmdnode;
  if ((tve->ROOT->TYPE & TTV_SIG_L)!=0 && inputck!=NULL && inputck->CMD!=NULL)
  {
   cmdnode=stb_getstbnode(inputck->CMD);
   if (cmdnode->CK!=NULL)
   {
    if ((inputck->CMD->TYPE & TTV_NODE_UP)!=0 && (cmdnode->CK->FLAGS & STBCK_FAKE_UP)!=0) return 0;
    if ((inputck->CMD->TYPE & TTV_NODE_UP)==0 && (cmdnode->CK->FLAGS & STBCK_FAKE_DOWN)!=0) return 0;
   }
  }
  return 1;
}

checkstruct sto_cfg[]=
  {
//    { 'b', __STB_CTK},
//    { 'b', __STB_DET_GRAPH},
    { 'b', __STB_STABILITY_HOLD},
    { 'b', __STB_STABILITY_SETUP},
    { 'b', __STB_ENBALE_COMMAND_CHECK},
    { 's', __STB_MONOPHASE},
    { 'i', __STB__PRECHARGE_DATA_HEURISTIC},
    { 'i', __STB_COREL_SKEW_ANA_DEPTH},
    { 'b', __STB_MULTIPLE_COMMAND}
  };
int sto_cfg_size=sizeof(sto_cfg)/sizeof(*sto_cfg);


int stb_getfalsepathkey(inffig_list *ifl)
{
  chain_list *ch;
  ptype_list *p;
  int key=0;
  chain_list *cl, *list0;
  char *name, *val, *code;

  if (ifl!=NULL)
    {
      if (ifl->LOADED.INF_FALSEPATH!=NULL)
        {
          for (ch = ifl->LOADED.INF_FALSEPATH; ch; ch = ch->NEXT) {
            for (p = (ptype_list *) ch->DATA; p; p = p->NEXT) {
              key+=mbk_sumchar((char *)p->DATA);      
              switch (p->TYPE) {
              case INF_UPDOWN: key+=3001; break;
              case INF_UP: key+=3101; break;
              case INF_DOWN: key+=3011; break;
              case INF_NOTHING: key+=5012; break;
              case INF_CK:
                if (((char *)p->DATA)[0]=='^') key+=6027;
                else key+=7087;
                break;
              }
            }
          }
        }

      list0 = inf_GetEntriesByType (ifl, INF_BYPASS, INF_ANY_VALUES);
      for (cl = list0; cl; cl = cl->NEXT) 
        {
          name = (char *)cl->DATA;
          inf_GetString (ifl, name, INF_BYPASS, &val);
          key+=mbk_sumchar(name)*1056;
          key+=mbk_sumchar(val)<<8;
        }
      freechain (list0);
    }

  return key;
}
int stb_getfalseslackkey (inffig_list *ifl)
{
  chain_list *ch;
  ptype_list *p;
  int key=0, skey;

  if (ifl!=NULL)
  {
    for (ch = ifl->LOADED.INF_FALSESLACK; ch; ch = ch->NEXT) {
      skey=0;
      for (p = (ptype_list *) ch->DATA; p; p = p->NEXT) {
         skey=mbk_sumchar((char *)p->DATA);
        if ((p->TYPE & (INF_FALSESLACK_UP|INF_FALSESLACK_DOWN))!=(INF_FALSESLACK_UP|INF_FALSESLACK_DOWN))
        {
          if (p->TYPE & INF_FALSESLACK_UP) skey+=3101;
          else skey+=3001;
        }
        if ((p->TYPE & (INF_FALSESLACK_HZ|INF_FALSESLACK_NOTHZ))!=(INF_FALSESLACK_HZ|INF_FALSESLACK_NOTHZ))
        {
          if (p->TYPE & INF_FALSESLACK_UP) skey+=7101;
          else skey+=8101;
        }
        skey=skey << 1;
      }
      key+=skey;
    }
  }
  return key;
}

int stb_getmulticyclekey(inffig_list * myfig)
{
  chain_list *chainx, *list;
  inf_assoc *assoc;
  int key=0;

  if (myfig!=NULL && inf_GetPointer (myfig, INF_MULTICYCLE_PATH, "", (void **)&list))
    {
      for (chainx = list; chainx; chainx = chainx->NEXT) 
        {
          assoc = (inf_assoc *) chainx->DATA;
          if ((assoc->lval & (INF_MULTICYCLE_SETUP | INF_MULTICYCLE_HOLD)) !=
              (INF_MULTICYCLE_SETUP | INF_MULTICYCLE_HOLD)) 
            {
              if (assoc->lval & INF_MULTICYCLE_SETUP)
                key+=1946;
              else
                key+=6491;
            }

          if (!
              (strcmp (assoc->dest, "*") == 0
               && (assoc->lval & (INF_MULTICYCLE_RISE | INF_MULTICYCLE_FALL)) ==
               (INF_MULTICYCLE_RISE | INF_MULTICYCLE_FALL))) 
            {
              key+=mbk_sumchar(assoc->dest)*5;
              if ((assoc->lval & (INF_MULTICYCLE_RISE | INF_MULTICYCLE_FALL)) !=
                  (INF_MULTICYCLE_RISE | INF_MULTICYCLE_FALL)) 
                {
                  if (assoc->lval & INF_MULTICYCLE_RISE)
                    key+=0x40006;
                  else
                    key+=0x40050;
                }
            }

          key+=assoc->dval*13;

          if (assoc->lval & INF_MULTICYCLE_END)
            key-=0x30005;
          else
            key+=0x30006;

          key+=mbk_sumchar(assoc->orig)*17;
        }
    }
  return key;
}
int stb_getclockuncertaintykey(inffig_list * myfig)
{
  chain_list *chainx, *list;
  inf_assoc *assoc;
  int key=0;

  if (myfig!=NULL && inf_GetPointer (myfig, INF_CLOCK_UNCERTAINTY, "", (void **)&list))
    {
      for (chainx = list; chainx; chainx = chainx->NEXT) 
        {
          assoc = (inf_assoc *) chainx->DATA;
          key+=mbk_sumchar(assoc->dest)*5;
          key+=assoc->dval*13;
          key+=assoc->lval*14;
          key+=mbk_sumchar(assoc->orig)*17;
        }
    }
  return key;
}

int stb_getdirectivekey(inffig_list * myfig)
{
  chain_list *chainx, *list;
  inf_assoc *assoc;
  int key=0;

  if (myfig!=NULL && inf_GetPointer (myfig, INF_DIRECTIVES, "", (void **)&list)) 
    {
      for (chainx = list; chainx; chainx = chainx->NEXT) 
        {
          assoc = (inf_assoc *) chainx->DATA;
          key+=mbk_sumchar(assoc->orig)*42;
          key+=assoc->lval*27;
          key+=mbk_sumchar(assoc->dest);
          key+=assoc->dval*13;
        }
    }
  return key;
}

int stb_getdelaymarginkey (inffig_list * myfig)
{
  ht *tempht;
  chain_list *list, *cl;
  int key=0;
  char *run[] = {
    INF_PATHDELAYMARGINMIN, INF_PATHDELAYMARGINMAX,
    INF_PATHDELAYMARGINCLOCK, INF_PATHDELAYMARGINDATA,
    INF_PATHDELAYMARGINRISE, INF_PATHDELAYMARGINFALL
  };
  int i, j, k;
  char section[128];
  inf_miscdata *imd0;

  for (i = 0; i < 2; i++)
    for (j = 2; j < 4; j++)
      for (k = 4; k < 6; k++) 
        {
          sprintf (section, INF_PATHDELAYMARGINPREFIX "|%s,%s,%s", run[i], run[j], run[k]);
          list = inf_GetEntriesByType (myfig, section, INF_ANY_VALUES);
          for (cl = list; cl; cl = cl->NEXT)
            {
              if (inf_GetPointer (myfig, (char *)cl->DATA, section, (void **)&imd0)) 
                {
                  key+=mbk_sumchar((char *)cl->DATA)*15;
                  key+=imd0->lval<<4;
                  key+=(long)(imd0->dval*10000)+(long)(imd0->dval1 * 1e13);
                  key+=mbk_sumchar(section)*mbk_sumchar((char *)cl->DATA);
                }
            }
          freechain (list);
        }
  return key;
}

int stb_getnocheckkey(inffig_list * ifl)
{
  chain_list *ch, *list0;
  int val, key=0;

  list0 = inf_GetEntriesByType (ifl, INF_NOCHECK, INF_ANY_VALUES);
  for (ch = list0; ch; ch = ch->NEXT) {
    inf_GetInt (ifl, (char *)ch->DATA, INF_NOCHECK, &val);
    key+=val*mbk_sumchar((char *)ch->DATA);
  }
  freechain(list0);
  return key;
}

int stb_getlistsectionkey(inffig_list * ifl, char *section)
{
  chain_list *ch, *list0;
  int key=0;

  list0 = inf_GetEntriesByType (ifl, section, INF_ANY_VALUES);
  for (ch = list0; ch; ch = ch->NEXT) {
    key+=mbk_sumchar((char *)ch->DATA);
  }
  freechain(list0);
  return key;
}

void create_clock_stability(stbnode *clocknode, stbpair_list **RES_STBTAB)
{
  long min, max;
   if (clocknode->EVENT->TYPE & TTV_NODE_UP) min=clocknode->CK->SUPMIN, max=clocknode->CK->SUPMAX;
   else min=clocknode->CK->SDNMIN, max=clocknode->CK->SDNMAX;
 
   RES_STBTAB[(int)clocknode->CK->CKINDEX]=stb_addstbpair(NULL, min, max);
}

ht *stb_buildquickaccessht(ttvfig_list *tvf, NameAllocator *NA)
{
  chain_list *chainsig;
  ht *h;
  char buf[1024], *nname;
  ttvsig_list *tvs;

  CreateNameAllocator(10000, NA, CASE_SENSITIVE);

  chainsig = ttv_getsigbytype(tvf,NULL,TTV_SIG_TYPEALL,NULL) ;

  h=addht(100000);
  while (chainsig!=NULL)
    {
      tvs=(ttvsig_list *)chainsig->DATA;

      if (ttv_isgateoutput(tvf,tvs,TTV_FILE_DTX))
      {
        ttv_getnetname(tvf, buf, tvs);
        nname=NameAlloc(NA, buf);
        addhtitem(h, nname, (long)tvs);
      }
      chainsig=delchain(chainsig, chainsig);
    }
  return h;
}

static chain_list *stb_getsiglist(ttvfig_list *tvf, ht *h, char *name, NameAllocator *NA)
{
  char *nname;
  long l;
  chain_list chains, *chainsig;
  
  nname=NameAlloc(NA, name);
  if ((l=gethtitem(h, nname))!=EMPTYHT) return addchain(NULL, (ttvsig_list *)l);

  chains.DATA = name ;
  chains.NEXT = NULL ;
     
  // a faire: filtrer les noeuds rc
  chainsig = ttv_getsigbytype_and_netname(tvf,NULL,TTV_SIG_TYPEALL,&chains) ;
  if (chainsig==NULL) avt_errmsg(STB_ERRMSG, "045", AVT_WARNING, name);
  return chainsig;
}

void stb_setprobability (stbfig_list *sbf, inffig_list *ifl)
{
  chain_list *chainx, *list, *origlist;
  double val;
  NameAllocator NA;
  ht *h;
  ptype_list *pt;
  ttvsig_list *tvs;

  list=inf_GetEntriesByType(ifl, INF_SWITCHING_PROBABILITY, INF_ANY_VALUES);
    
  if (list) 
  {
    h=stb_buildquickaccessht(sbf->FIG, &NA);
    
    for (chainx = list; chainx; chainx = chainx->NEXT) 
    {
      inf_GetDouble(ifl, (char*)chainx->DATA, INF_SWITCHING_PROBABILITY, &val);

      origlist=stb_getsiglist(sbf->FIG, h, (char*)chainx->DATA, &NA);
      while (origlist!=NULL)
      {
        tvs=(ttvsig_list *)origlist->DATA;
        if ((pt=getptype(tvs->USER, STB_TRANSITION_PROBABILITY))==NULL)
        {
          tvs->USER=addptype(tvs->USER, STB_TRANSITION_PROBABILITY, NULL);
          *(float *)&tvs->USER->DATA=val;
        }
        origlist=delchain(origlist, origlist);
      }
    }
    freechain(list);
    delht(h);
    DeleteNameAllocator(&NA);
  }
}

int stb_getswitchingprobakey(inffig_list * ifl)
{
  chain_list *ch, *list0;
  int key=0;
  double val;

  list0 = inf_GetEntriesByType (ifl, INF_SWITCHING_PROBABILITY, INF_ANY_VALUES);
  for (ch = list0; ch; ch = ch->NEXT) {
    inf_GetDouble (ifl, (char *)ch->DATA, INF_SWITCHING_PROBABILITY, &val);
    key+=(int)(val*10000*mbk_sumchar((char *)ch->DATA));
  }
  freechain(list0);
  return key;
}

int stb_IsClockCK(ttvevent_list *tve, stbck *ck)
{
  if (tve==ck->ORIGINAL_CLOCK) return 1;
  return 0;
}

stbck *stb_GetClockCK(ttvevent_list *tve, stbck *ck)
{
  while (ck!=NULL && tve!=ck->ORIGINAL_CLOCK) ck=ck->NEXT;
  return ck;
}
int stb_cmpphase(stbfig_list *sf, char a, char b)
{
  int ret=0;
  if (a==STB_NO_INDEX || b==STB_NO_INDEX)
  {
    if (a<b) ret=-1;
    else if (a>b) ret=1;
  }
  else
  {
    if (sf->PHASE_DATE[(int)a]<sf->PHASE_DATE[(int)b]) ret=-1;
    else if (sf->PHASE_DATE[(int)a]>sf->PHASE_DATE[(int)b]) ret=1;
  }
//  printf("ret=%d a=%d b=%d\n",ret,a,b);
  return ret;
}
long stb_synchronize_slopes(stbfig_list *ptstbfig, char startphase, char endphase, long endper, int mode)
{
  long startslope, endslope, per=0;
  char edge;
  stbck *ck;

  if (startphase!=STB_NO_INDEX && endphase!=STB_NO_INDEX)
  {
    
    if (stb_cmpphase(ptstbfig, endphase,startphase)<0)
    {
      if (V_BOOL_TAB[__STB_SYNC_SLOPES].VALUE)
      {
        ck=stb_getclock(ptstbfig, startphase, NULL, &edge, NULL);
        if (edge==STB_SLOPE_DN) startslope=ck->SDNMIN; else startslope=ck->SUPMIN;
        ck=stb_getclock(ptstbfig, endphase, NULL, &edge, NULL);
        if (edge==STB_SLOPE_DN) endslope=ck->SDNMIN; else endslope=ck->SUPMIN;
  //      endper=ck->PERIOD;
        if (mode & STB_SPECOUTMODE) endslope+=endper;
        per=((startslope-endslope+endper-1)/endper)*endper;
      }
      else
      {
        per=endper;
      }
    }
    if ((mode & STB_DECIFEQUALPHASE)!=0 && (stb_cmpphase(ptstbfig,endphase,startphase)==0 || (V_BOOL_TAB[__STB_SYNC_SLOPES].VALUE && stb_cmpphase(ptstbfig, endphase,startphase)<0 && startslope+per==endslope))) per+=endper;
  }
  return per;
}

stbpair_list *stb_clippair(stbpair_list *ptstbpair, long min, long max)
{
  stbpair_list *pts, *uppair, *downpair;
  if(min != TTV_NOTIME && ptstbpair != NULL)
    {
      while((ptstbpair->D) < min)
        {
          if((ptstbpair->U) < min)
            {
              pts = ptstbpair ;
              ptstbpair = ptstbpair->NEXT;
              pts->NEXT = NULL;
              stb_freestbpair (pts);
              if(ptstbpair == NULL)
                break ;
            }
          else
            {
              ptstbpair->D =  min;
              break ;
            }
        }
    }
  if(max!=TTV_NOTIME && ptstbpair != NULL)
    { 
      uppair=ptstbpair; downpair=NULL;
      while(uppair!=NULL && (uppair->D) <= max) downpair=uppair, uppair=uppair->NEXT;
      if (downpair!=NULL)
        {
          stb_freestbpair (downpair->NEXT); downpair->NEXT=NULL;
          if (downpair->U > max) downpair->U=max;
        } else ptstbpair=NULL;
    }

  return ptstbpair;
}

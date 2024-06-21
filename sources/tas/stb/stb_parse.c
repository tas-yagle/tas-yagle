/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STB Version 1.00                                            */
/*    Fichier : stb_parse.c                                                 */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                Anthony LESTER                                            */
/*    03082004  : Antony PINTO                                              */
/*                several functions moved from stb.yac                      */
/*                                                                          */
/****************************************************************************/

#include STB_H

#include "stb_init.h"
#include "stb_util.h"
#include "stb_error.h"
#include "stb_parse.h"
#include "stb_fromx.h"
#include <sys/wait.h>

#define STB_P_MEM 5     // node memory
#define STB_P_NOD 6     // node
#define STB_P_ONOD 7    // output node
#define STB_P_INOD 8    // input node
#define STB_P_LOC 9     // locon
#define STB_P_ILOCD 10  // input default locon
#define STB_P_OLOCD 11  // output define locon

#define STB_P_DUPPAIR 1
#define STB_P_ADDPAIR 2

#define BEGIN //printf("%s\n",__func__);

extern int      stbparse();
extern void     stbrestart();
extern void     stb_parse_init_stab(stb_parse *stab);

extern FILE    *stbin;
int             yylineno;
static stb_parse    *STAB;

/****************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void
stb_yyclean()
{
    if (STAB->PTINSCHAIN != NULL) {
        freechain(STAB->PTINSCHAIN);
        STAB->PTINSCHAIN = NULL;
    }
    if (STAB->PTSTABLELIST != NULL) {
        freechain(STAB->PTSTABLELIST);
        STAB->PTSTABLELIST = NULL;
    }
    if (STAB->PTUNSTABLELIST != NULL) {
        freechain(STAB->PTUNSTABLELIST);
        STAB->PTUNSTABLELIST = NULL;
    }
    if (STAB->PARSEDFIG != NULL) {
        stb_delstbfig(STAB->PARSEDFIG);
        STAB->PARSEDFIG = NULL;
    }
}

void
stb_parse_init_stab(stb_parse *stab)
{
    STAB    = stab;
    stb_parse_init(STAB,stb_yyclean);
}

stb_parse *
stb_convert_inf_to_stb_1(stbfig_list *ptstbfig)
{
  stb_fromX_t       fxt;
  static stb_parse  stab = { 
    NULL, // parsedfig
    NULL, // ptinschain
    NULL, // ptstablelist
    NULL, // ptunstablelist
    NULL, // ckdomains
    NULL, // ckeqvt
    0,    // defaultphase
    0,    // domainindex
    0,    // eqvtindex
    0,    // inverted
    0,    // ideal
    0,    // virtual
    NULL  // cleanfunc
  };
  fxt.STBFIG      = ptstbfig;
  fxt.UNIT        = 1e-12;


  stb_parse_init_stab(&stab);
  stb_fromX_STEP1_traditional(&fxt, &stab);

  return &stab; 
    
}

int
stb_convert_inf_to_stb_2(stbfig_list *ptstbfig, stb_parse *stab)
{
  stb_fromX_t       fxt;
  fxt.STBFIG      = ptstbfig;
  fxt.UNIT        = 1e-12;


  stb_fromX_STEP2_stability(&fxt, stab);
  stb_fromX_STEP3_finish(&fxt, stab);

  return 0; 
    
}

int
stb_loadstbfig(stbfig_list *ptstbfig)
{
  pid_t             pid;
  int               status;
  char              str[1024];
  int               j;
  stb_fromX_t       fxt;
  static stb_parse  stab = { 
    NULL, // parsedfig
    NULL, // ptinschain
    NULL, // ptstablelist
    NULL, // ptunstablelist
    NULL, // ckdomains
    NULL, // ckeqvt
    0,    // defaultphase
    0,    // domainindex
    0,    // eqvtindex
    0,    // inverted
    0,    // ideal
    0,    // virtual
    NULL  // cleanfunc
  };


  if (STB_FILE_FORMAT == STB_SDC_FORMAT) {
    if (STB_FOREIGN_CONSTRAINT_FILE == NULL)  {
      strcpy (str, ptstbfig->FIG->INFO->FIGNAME);
      strcat (str, ".sdc");
      STB_FOREIGN_CONSTRAINT_FILE = namealloc (str);
    }

    pid = vfork ();

    if (pid == (pid_t) (-1)) 
      stb_error (ERR_CANNOT_RUN, "sdc2stb", 0, STB_FATAL);

    else if (pid == 0) {
      if (execlp ("sdc2stb", "sdc2stb", STB_FOREIGN_CONSTRAINT_FILE, "-tu", STB_FOREIGN_TIME_UNIT, "-out", ptstbfig->FIG->INFO->FIGNAME, NULL)==-1)
      {
        perror("system says ");
        EXIT(1);
      }   
    }
    else {
      j = mbkwaitpid (pid, 1, &status);
      if ((j == 0) || (WIFEXITED (status) == 0)) 
        fprintf (stderr, "cannot execute sdc2stb\n");
      else if (WEXITSTATUS (status) != 0) 
        fprintf (stderr, "sdc2stb exit with a non zero value\n");
      else
      { 
        inf_DisplayLoad(1);
        inf_Dirty(ptstbfig->FIG->INFO->FIGNAME);
        getinffig(ptstbfig->FIG->INFO->FIGNAME);
        inf_DisplayLoad(0);
      }
    }
  }

/*
  if ((stbin = mbkfopen(ptstbfig->FIG->INFO->FIGNAME, "stb", READ_TEXT)) == NULL) {
    stb_error(ERR_CANNOT_OPEN, NULL, 0, STB_FATAL);
  }
*/
 /* 
  if(vierge == 0)
    stbrestart(stbin) ;
  vierge = 0 ;

  yylineno = 1;
*/
  fxt.STBFIG      = ptstbfig;
  fxt.UNIT        = 1e-12;


  stb_parse_init_stab(&stab);
  stb_fromX(&fxt,&stab);
 // stbparse();

  /*
  if (fclose(stbin)) {
    stb_error(ERR_CANNOT_CLOSE, NULL, 0, STB_FATAL);
  }
  */

  return 0; 
}

/*}}}************************************************************************/
/****************************************************************************/
/*{{{                    Parse struct                                       */
/****************************************************************************/
/*{{{                    stb_parse_init();                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
stb_parse_init(stb_parse *stab, void (*cleanfunc)(void))
{BEGIN
  stab->PARSEDFIG       = NULL;
  stab->PTINSCHAIN      = NULL;
  stab->PTSTABLELIST    = NULL;
  stab->PTUNSTABLELIST  = NULL;
  stab->CKDOMAINS       = NULL;
  stab->CKEQVT          = NULL;
  stab->DEFAULTPHASE    = STB_NO_INDEX;
  stab->DOMAININDEX     = 0;
  stab->EQVTINDEX       = 0;
  stab->INVERTED        = 0;
  stab->IDEAL           = 0;
  stab->VIRTUAL         = 0;
  stab->CLEANFUNC       = cleanfunc;
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Parse Clock                                        */
/****************************************************************************/
/*{{{                    addstbck()                                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline void
addstbck(stbnode *ptstbnode, stb_parse_pulse *pulse, stbfig_list *parsedfig)
{
  stb_addstbck(ptstbnode,
               pulse->supmin, pulse->supmax, 
               pulse->sdnmin, pulse->sdnmax, 
               pulse->period,
               parsedfig->SETUP, parsedfig->HOLD,
               STB_NO_INDEX, 0, STB_NO_VERIF, STB_TYPE_CLOCK, ptstbnode->EVENT);
  
}

/*}}}************************************************************************/
/*{{{                    stb_parse_cklocon()                                */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

static chain_list *clocksigpriority(chain_list *cl)
{
  chain_list *ch=NULL, *runcl;
  ttvsig_list *tvs;
  long go[4]={TTV_SIG_B, TTV_SIG_L, TTV_SIG_C, 0};
  int i;

  for (i=0; i<4; i++)
  {
    for (runcl=cl; runcl!=NULL; runcl=runcl->NEXT)
    {
      tvs=(ttvsig_list *)runcl->DATA;
      if (tvs!=NULL && (go[i]==0 || (tvs->TYPE & go[i])!=0)) 
      {
        ch=addchain(ch, tvs);
        runcl->DATA=NULL;
      }
    }
  }
  freechain(cl);
  return ch;
}

static long stb_findlatency(inffig_list *ifl, stb_parse *stab, ttvsig_list *ptttvsig, char *type, int *found)
{
  chain_list *list, *chainx, *cl;
  double val=0;
  int res=0;
  // clock
  list          = inf_GetEntriesByType(ifl,type,INF_ANY_VALUES);
  for (chainx = list; chainx && res==0; chainx = delchain(chainx,chainx))
  {
    cl=addchain(NULL, (char *)chainx->DATA);
    res=ttv_testnetnamemask(stab->PARSEDFIG->FIG, ptttvsig, cl);
    if (res)
    {
      inf_GetDouble(ifl, (char *)chainx->DATA, type, &val);
      *found=1;
    }
    freechain(cl);
  }
  return val*1e12*TTV_UNIT;
}

void
stb_parse_cklocon(stb_parse *stab, char *ident, stb_parse_pulse *pulse, inffig_list *ifl)
{BEGIN
  ttvsig_list    *ptttvsig;
  chain_list     *ptchain;
  chain_list     *headlist, *cl;
  char           *name;
  long            period, swap;
  int             found = FALSE;
  stb_propagated_clock_to_clock *spctc;
          
  name          = namealloc(ident);
  if (pulse->supmin==STB_NO_TIME || pulse->sdnmin==STB_NO_TIME)
     avt_errmsg(STB_ERRMSG, "044", AVT_FATAL, name);

  headlist      = ttv_getsigbytype(stab->PARSEDFIG->FIG, NULL,
                                   TTV_SIG_C|TTV_SIG_B|TTV_SIG_L/*|TTV_SIG_Q*/, NULL);

  headlist=clocksigpriority(headlist);

  cl=addchain(NULL, name);
  for (ptchain = headlist; ptchain; ptchain = ptchain->NEXT)
  {
    ptttvsig    = (ttvsig_list *)ptchain->DATA;
    if (ttv_testnetnamemask(stab->PARSEDFIG->FIG, ptttvsig, cl))
    {
      ttvevent_list *ptevent;
      stbnode       *ptstbnode;

      if (stab->INVERTED)
      {             
        ptttvsig->USER  = addptype (ptttvsig->USER, STB_INVERTED_CLOCK, 0);
        swap=pulse->supmin, pulse->supmin=pulse->sdnmin, pulse->sdnmin=swap;
        swap=pulse->supmax, pulse->supmax=pulse->sdnmax, pulse->sdnmax=swap;
      }
      if (stab->IDEAL)
        ptttvsig->USER  = addptype (ptttvsig->USER, STB_IDEAL_CLOCK, 0);

      period            = pulse->period;
      stab->PARSEDFIG->CLOCK = addchain(stab->PARSEDFIG->CLOCK, ptttvsig);
      
      ptevent           = ptttvsig->NODE;
      ptstbnode         = stb_getstbnode(ptevent);
      addstbck(ptstbnode,pulse,stab->PARSEDFIG);
      
      ptevent           = ptttvsig->NODE+1;
      ptstbnode         = stb_getstbnode(ptevent);
      addstbck(ptstbnode,pulse,stab->PARSEDFIG);

      spctc=(stb_propagated_clock_to_clock *)mbkalloc(sizeof(stb_propagated_clock_to_clock));
      spctc->edges=pulse->master_edges;
      spctc->master=pulse->master_clock;
      memcpy(&spctc->original_waveform, ptstbnode->CK, sizeof(stbck));
      spctc->haslatency=0;
      spctc->latencies.SUPMIN=stb_findlatency(ifl,stab, ptttvsig, INF_LATENCY_RISE_MIN,&spctc->haslatency);
      spctc->latencies.SUPMAX=stb_findlatency(ifl,stab, ptttvsig, INF_LATENCY_RISE_MAX,&spctc->haslatency);
      spctc->latencies.SDNMIN=stb_findlatency(ifl,stab, ptttvsig, INF_LATENCY_FALL_MIN,&spctc->haslatency);
      spctc->latencies.SDNMAX=stb_findlatency(ifl,stab, ptttvsig, INF_LATENCY_FALL_MAX,&spctc->haslatency);
      if (stab->INVERTED)
      {
        swap=spctc->latencies.SUPMIN; spctc->latencies.SUPMIN=spctc->latencies.SDNMIN; spctc->latencies.SDNMIN=swap;
        swap=spctc->latencies.SUPMAX; spctc->latencies.SUPMAX=spctc->latencies.SDNMAX; spctc->latencies.SDNMAX=swap;
      }
      ptstbnode->CK->SUPMIN+=spctc->latencies.SUPMIN; ptstbnode->CK->SUPMAX+=spctc->latencies.SUPMAX;
      ptstbnode->CK->SDNMIN+=spctc->latencies.SDNMIN; ptstbnode->CK->SDNMAX+=spctc->latencies.SDNMAX;
      ptttvsig->USER  = addptype (ptttvsig->USER, STB_IS_CLOCK, spctc);
              
      found             = TRUE;
      
      if (!mbk_isregex_name(name))
        break;
    }
  }
  freechain(cl);
  freechain(headlist);
  
  if (found == FALSE)
    {
      if (stab->VIRTUAL)
        {
          ttvevent_list *ptevent;
          stbnode       *ptstbnode;
          ptttvsig    = (ttvsig_list *)mbkalloc(sizeof(ttvsig_list));
          ttv_init_refsig(stab->PARSEDFIG->FIG, ptttvsig, ident, ident, 0, TTV_SIG_C);
          stb_addstbnode (ptttvsig->NODE);
          stb_addstbnode (ptttvsig->NODE+1);

          ptttvsig->USER  = addptype (ptttvsig->USER, STB_VIRTUAL_CLOCK, 0);

          if (stab->INVERTED)
            {             
              ptttvsig->USER  = addptype (ptttvsig->USER, STB_INVERTED_CLOCK, 0);
              swap=pulse->supmin, pulse->supmin=pulse->sdnmin, pulse->sdnmin=swap;
              swap=pulse->supmax, pulse->supmax=pulse->sdnmax, pulse->sdnmax=swap;
            }
          if (stab->IDEAL)
            ptttvsig->USER  = addptype (ptttvsig->USER, STB_IDEAL_CLOCK, 0);

          period            = pulse->period;
          stab->PARSEDFIG->CLOCK = addchain(stab->PARSEDFIG->CLOCK, ptttvsig);
      
          ptevent           = ptttvsig->NODE;
          ptstbnode         = stb_getstbnode(ptevent);
          addstbck(ptstbnode,pulse,stab->PARSEDFIG);
      
          ptevent           = ptttvsig->NODE+1;
          ptstbnode         = stb_getstbnode(ptevent);
          addstbck(ptstbnode,pulse,stab->PARSEDFIG);
        
          spctc=(stb_propagated_clock_to_clock *)mbkalloc(sizeof(stb_propagated_clock_to_clock));
          spctc->edges=pulse->master_edges;
          spctc->master=pulse->master_clock;
          memcpy(&spctc->original_waveform, ptstbnode->CK, sizeof(stbck));
          spctc->haslatency=0;
          spctc->latencies.SUPMIN=stb_findlatency(ifl,stab, ptttvsig, INF_LATENCY_RISE_MIN,&spctc->haslatency);
          spctc->latencies.SUPMAX=stb_findlatency(ifl,stab, ptttvsig, INF_LATENCY_RISE_MAX,&spctc->haslatency);
          spctc->latencies.SDNMIN=stb_findlatency(ifl,stab, ptttvsig, INF_LATENCY_FALL_MIN,&spctc->haslatency);
          spctc->latencies.SDNMAX=stb_findlatency(ifl,stab, ptttvsig, INF_LATENCY_FALL_MAX,&spctc->haslatency);
          if (stab->INVERTED)
          {
            swap=spctc->latencies.SUPMIN; spctc->latencies.SUPMIN=spctc->latencies.SDNMIN; spctc->latencies.SDNMIN=swap;
            swap=spctc->latencies.SUPMAX; spctc->latencies.SUPMAX=spctc->latencies.SDNMAX; spctc->latencies.SDNMAX=swap;
          }
          ptstbnode->CK->SUPMIN+=spctc->latencies.SUPMIN; ptstbnode->CK->SUPMAX+=spctc->latencies.SUPMAX;
          ptstbnode->CK->SDNMIN+=spctc->latencies.SDNMIN; ptstbnode->CK->SDNMAX+=spctc->latencies.SDNMAX;
          ptttvsig->USER  = addptype (ptttvsig->USER, STB_IS_CLOCK, spctc);
        
          found             = TRUE;
        }

      if (found == FALSE)
        {       
          stab->CLEANFUNC();
          stb_error(ERR_NO_CLOCK_CONNECTOR, name, yylineno, STB_FATAL);
        }
    }

  stab->INVERTED        = 0;
  stab->IDEAL           = 0;
}

/*}}}************************************************************************/
/*{{{                    stb_parse_ckdeclar()                               */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline char edge(char org)
{
  switch (org)
  {
    case INF_STB_RISING : return STB_SLOPE_UP;
    case INF_STB_FALLING : return STB_SLOPE_DN;
    case INF_STB_SLOPEALL : return STB_SLOPE_ALL;
    default : printf("0) Ca va pas non!!\n"); return 0;
  }
}

static void
treatpin_mark(stb_fromX_t *fxt, char *name, char *type)
{
  inf_stb_p_s       *ipss;
  inffig_list       *inf;
  ttvevent_list *tve;

  inf               = fxt->INFFIG;
  if (inf_GetPointer(inf,name,type,(void **)&ipss))
    {
      while (ipss!=NULL)
        {
          if (ipss->CKNAME!=NULL)
            {
              if ((tve=stb_getclockevent(fxt->STBFIG, ipss->CKNAME, edge(ipss->CKEDGE)))!=NULL)
                {
                  long flag=STB_SLOPE_DN;
                  if ((tve->TYPE & TTV_NODE_UP)==TTV_NODE_UP) flag=STB_SLOPE_UP;
                  stb_getstbnode (tve->ROOT->NODE)->CK->ACTIVE |= flag;
                  stb_getstbnode (tve->ROOT->NODE + 1)->CK->ACTIVE |= flag;
                }
            }
          ipss=ipss->NEXT;
        }
    }
}


static int stb_ismultiple(long val0, long val1)
{
  if (val0<val1) return (val1 % val0)==0;
  return (val0 % val1)==0;
}

void
stb_parse_ckdeclar(void *fxt0, stb_parse *stab)
{BEGIN
  ttvsig_list   *ptttvsig;
  chain_list    *ptchain, *ptchain1, *list;
  chain_list    *foundchain = NULL;
  stbck         *ptstbck;
  long           period;
  char           namebuf[1024];
  stb_fromX_t *fxt=(stb_fromX_t *)fxt0;
  stab->DOMAININDEX ++;
  for (ptchain = stab->PARSEDFIG->CLOCK; ptchain; ptchain = ptchain->NEXT)
  {
    ptttvsig            = (ttvsig_list *)ptchain->DATA;
    if (getptype(ptttvsig->USER, STB_DOMAIN) == NULL)
    {
      foundchain        = addchain(foundchain, ptttvsig);
      ptttvsig->USER    = addptype(ptttvsig->USER, STB_DOMAIN,
                                   (void *)((long)stab->DOMAININDEX));
    }
    ptstbck             = stb_getstbnode(ptttvsig->NODE)->CK;
    if (ptstbck->PERIOD == STB_NO_TIME)
    {
      if (stab->PARSEDFIG->CLOCKPERIOD == STB_NO_TIME)
      {
        stab->CLEANFUNC();
        stb_error(ERR_UNDEFINED_PERIOD,
                  ttv_getsigname(stab->PARSEDFIG->FIG, namebuf, ptttvsig),
                  0, STB_FATAL);
      }
      ptstbck->PERIOD   = stab->PARSEDFIG->CLOCKPERIOD;
      ptstbck           = stb_getstbnode(ptttvsig->NODE+1)->CK;
      ptstbck->PERIOD   = stab->PARSEDFIG->CLOCKPERIOD;
    }
  }
  if (foundchain != NULL)
  {
    stab->CKDOMAINS     = addchain(stab->CKDOMAINS, foundchain);
    stab->PARSEDFIG->CKDOMAIN = stb_addstbdomain(stab->PARSEDFIG->CKDOMAIN,
                                                 STB_NO_INDEX, STB_NO_INDEX);
  }
  stab->PARSEDFIG->USER = addptype(stab->PARSEDFIG->USER, STB_DOMAIN,
                                   stab->CKDOMAINS);

  /* verify that domains have equivalent periods */
  for (ptchain = stab->CKDOMAINS; ptchain; ptchain = ptchain->NEXT)
  {
    period              = STB_NO_TIME;
    for (ptchain1 = (chain_list *)ptchain->DATA;
         ptchain1;
         ptchain1 = ptchain1->NEXT)
    {
      ptttvsig          = (ttvsig_list *)ptchain1->DATA;
      ptstbck           = stb_getstbnode(ptttvsig->NODE)->CK;
      if (period == STB_NO_TIME)
        period          = ptstbck->PERIOD;
      else if (!stb_ismultiple(ptstbck->PERIOD,period))
      {
        stb_error( ERR_INCOHERENT_PERIOD,
                   stab->PARSEDFIG ? 
                       ttv_getsigname(stab->PARSEDFIG->FIG, namebuf, ptttvsig) :
                       "-null-", 
                   0,
                   STB_FATAL);
        stab->CLEANFUNC();
      }
    }
  }
  
  // marquage pour les phases des inputs
  list              = inf_GetEntriesByType(fxt->INFFIG,INF_SPECIN,INF_ANY_VALUES);
  for (ptchain = list; ptchain; ptchain = delchain(ptchain,ptchain))
    treatpin_mark(fxt,ptchain->DATA,INF_SPECIN);

  // marquage pour les phases des outputs
 list              = inf_GetEntriesByType(fxt->INFFIG,INF_SPECOUT,INF_ANY_VALUES);
  for (ptchain = list; ptchain; ptchain = delchain(ptchain,ptchain))
    treatpin_mark(fxt,ptchain->DATA,INF_SPECOUT);

  /* initialise the clocks */
  stb_initclock(stab->PARSEDFIG,0);
  stb_checkclock(stab->PARSEDFIG);
}

/*}}}************************************************************************/
/*{{{                    stb_parse_domain_groups()                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
stb_parse_domain_groups(stb_parse *stab, chain_list *group)
{BEGIN
  ttvsig_list   *ptttvsig;
  chain_list    *ptchain;
  char           name[1024];

  stab->DOMAININDEX ++;
  stab->CKDOMAINS       = addchain(stab->CKDOMAINS, group);
  for (ptchain = group; ptchain; ptchain = ptchain->NEXT)
  {
    ptttvsig            = (ttvsig_list *)ptchain->DATA;
    if (getptype(ptttvsig->USER, STB_DOMAIN) != NULL)
    {
      ttv_getsigname(stab->PARSEDFIG->FIG, name, ptttvsig);
      stab->CLEANFUNC();
      stb_error(ERR_MULTIPLE_DOMAIN, name, yylineno, STB_FATAL);
    }
    ptttvsig->USER      = addptype(ptttvsig->USER, STB_DOMAIN,
                                 (void *)((long)stab->DOMAININDEX));
  }
  stab->PARSEDFIG->CKDOMAIN = stb_addstbdomain(stab->PARSEDFIG->CKDOMAIN,
                                               STB_NO_INDEX, STB_NO_INDEX);
}

/*}}}************************************************************************/
/*{{{                    stb_parse_ckprio_pair()                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
stbpriority *
stb_parse_ckprio_pair(stb_parse *stab, char *id1, char *id3)
{BEGIN
  char           cknamebuf[1024];
  char           cknetnamebuf[1024];
  chain_list    *ptchain;
  stbpriority   *res = NULL;
  ttvsig_list   *ptttvsig;

  for (ptchain = stab->PARSEDFIG->CLOCK ; ptchain ; ptchain = ptchain->NEXT)
  {
    ptttvsig        = (ttvsig_list*)ptchain->DATA ;
    ttv_getsigname(stab->PARSEDFIG->FIG, cknamebuf, ptttvsig);
    ttv_getnetname(stab->PARSEDFIG->FIG, cknetnamebuf, ptttvsig);
    if (!mbk_casestrcmp(cknamebuf, id3) || !mbk_casestrcmp(cknetnamebuf, id3))
    {
      res           = (stbpriority*)mbkalloc (sizeof (struct stbpriority)) ;
      res->CLOCK    = ptttvsig ;
      res->MASK     = strdup(id1) ;
      break ;
    }
  }
  if (!ptchain) 
    stb_error(ERR_NO_CLOCK_CONNECTOR, id3, yylineno, STB_FATAL);

  return res;
}

/*}}}************************************************************************/
/*{{{                    stb_parse_ckname_list()                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
chain_list *
stb_parse_ckname_list(stb_parse *stab, chain_list *cklist, char *ident)
{BEGIN
  chain_list    *res;
  char          *ck;
  chain_list *cl;

  ck        = namealloc(ident);
  cl       = stb_getsigfromlist(stab->PARSEDFIG->FIG,
                                 stab->PARSEDFIG->CLOCK,
                                 ck);

  if (!cl)
  {
    stab->CLEANFUNC();
    stb_error(ERR_NO_CLOCK_CONNECTOR, ck, yylineno, STB_FATAL);
  }
  //res       = addchain(cklist,sig);
  while (cl!=NULL)
  {
    if (getchain(cklist, cl->DATA)==NULL)
      cklist=addchain(cklist, cl->DATA);
//    res       = append(cklist,cl);
    cl=delchain(cl,cl);
  }

  return cklist;
}

/*}}}************************************************************************/
/*{{{                    stb_parse_group()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
chain_list *
stb_parse_group(chain_list *cklist, long period)
{BEGIN
  ttvsig_list   *ptttvsig;
  chain_list    *ptchain;
  stbck         *ptstbck;

  if (period != STB_NO_TIME)
  {
    for (ptchain = cklist; ptchain; ptchain = ptchain->NEXT)
    {
      ptttvsig          = (ttvsig_list *)ptchain->DATA;
      
      ptstbck           = stb_getstbnode(ptttvsig->NODE)->CK;
      if (ptstbck->PERIOD == STB_NO_TIME)
        ptstbck->PERIOD = period;
      
      ptstbck           = stb_getstbnode(ptttvsig->NODE+1)->CK;
      if (ptstbck->PERIOD == STB_NO_TIME)
        ptstbck->PERIOD = period;
    }
  }

  return cklist;
}

/*}}}************************************************************************/
/*{{{                    stb_parse_eqvt_groups()                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
stb_parse_eqvt_groups(stb_parse *stab, chain_list *group)
{BEGIN
  ttvsig_list    *ptttvsig;
  chain_list     *ptchain;
  ptype_list     *ptuser;
  char            curdomain = STB_NO_INDEX;
  char            sigdomain;
  char            name[1024];

  stab->CKEQVT      = addchain(stab->CKEQVT, group);
  stab->EQVTINDEX ++;
  for (ptchain = group; ptchain; ptchain = ptchain->NEXT)
  {
    ptttvsig        = (ttvsig_list *)ptchain->DATA;
    if (getptype(ptttvsig->USER, STB_EQUIVALENT) != NULL)
    {
      ttv_getsigname(stab->PARSEDFIG->FIG, name, ptttvsig);
      stab->CLEANFUNC();
      stb_error(ERR_MULTIPLE_EQUIVALENCE, name, yylineno, STB_FATAL);
    }
    
    if ((ptuser = getptype(ptttvsig->USER, STB_DOMAIN)) == NULL)
      sigdomain = 0;
    else
      sigdomain = (char)((long)ptuser->DATA);
    
    if (curdomain == STB_NO_INDEX)
      curdomain = sigdomain;
    else if (sigdomain != curdomain)
    {
      ttv_getsigname(stab->PARSEDFIG->FIG, name, ptttvsig);
      stab->CLEANFUNC();
      stb_error(ERR_DOMAIN_NOT_EQUIVALENT, name, yylineno, STB_FATAL);
    }
    
    ptttvsig->USER = addptype(ptttvsig->USER, STB_EQUIVALENT,
                              (void *)((long)stab->EQVTINDEX));
  }
}

chain_list *stb_auto_create_equivalent_groups(stb_parse *stab)
{
  stb_propagated_clock_to_clock *spctc;
  chain_list *cl, *ch, *clocks;
  char *master;
  stbck *ref, *cur;
  ttvsig_list *tvs, *tvso;
  char clockname[1024];
  char *thisgroupmaster, ok;
  chain_list *grp, *res=NULL;
  ptype_list *pt;
  void *refdomain, *curdomain;

  clocks=dupchainlst(stab->PARSEDFIG->CLOCK);

  for (cl = clocks; cl; cl = cl->NEXT)
    {
      tvso=(ttvsig_list *)cl->DATA;
      if (tvso!=NULL)
        {
/*          if ((pt=getptype(tvso->USER, STB_IS_CLOCK))!=NULL)
            {
              spctc=(stb_propagated_clock_to_clock *)pt->DATA;
              ref=&spctc->original_waveform;
            }
          else*/
            ref=stb_getstbnode(&tvso->NODE[0])->CK;
          if ((pt=getptype(tvso->USER, STB_DOMAIN))!=NULL) refdomain=pt->DATA;
          else refdomain=NULL;

          grp=addchain(NULL, tvso);
          cl->DATA=NULL;
          for (ch = cl->NEXT; ch; ch = ch->NEXT)
            {
              tvs=(ttvsig_list *)ch->DATA;
              if (tvs!=NULL)
                {
/*                  if ((pt=getptype(tvs->USER, STB_IS_CLOCK))!=NULL)
                    {
                      spctc=(stb_propagated_clock_to_clock *)pt->DATA;
                      cur=&spctc->original_waveform;
                    }
                  else*/
                    cur=stb_getstbnode(&tvs->NODE[0])->CK;

                  if ((pt=getptype(tvso->USER, STB_DOMAIN))!=NULL) curdomain=pt->DATA;
                  else curdomain=NULL;
                  if (curdomain==refdomain && cur->PERIOD==ref->PERIOD
                      && ((cur->SUPMAX==ref->SUPMAX && cur->SUPMIN==ref->SUPMIN
                          && cur->SDNMAX==ref->SDNMAX && cur->SDNMIN==ref->SDNMIN)
                         || (cur->SUPMAX==ref->SDNMAX && cur->SUPMIN==ref->SDNMIN
                          && cur->SDNMAX==ref->SUPMAX && cur->SDNMIN==ref->SUPMIN)))
                    {
                      grp=addchain(grp, tvs);
                      ch->DATA=NULL;
                    }
                }
            }
          if (grp->NEXT!=NULL)
            {
              avt_log(LOGSTABILITY,1, "New equivalent clock group:");
              for (ch=grp; ch!=NULL; ch=ch->NEXT)
                avt_log(LOGSTABILITY,1, " %s", ttv_getsigname(stab->PARSEDFIG->FIG,clockname,(ttvsig_list *)ch->DATA));
              avt_log(LOGSTABILITY,1, "\n");
           
              res=addchain(res, grp);
            }
          else
            freechain(grp);
        }
    }
  freechain(clocks);
  return res;
}

chain_list *stb_merge_equiv_if_needed(chain_list *equiv, chain_list *equiv_auto)
{
  chain_list *cl, *ch, *ecl, *ech, *toapp;
  while (equiv_auto!=NULL)
    {
      toapp=NULL;
      for (ch=(chain_list *)equiv_auto->DATA; ch!=NULL; ch=ch->NEXT)
        {
          for (ecl=equiv; ecl!=NULL; ecl=ecl->NEXT)
            {
              for (ech=(chain_list *)ecl->DATA; ech!=NULL && ech->DATA!=ch->DATA; ech=ech->NEXT) ;
              if (ech!=NULL)
                {
                  toapp=append(ecl->DATA, toapp);
                  ecl->DATA=NULL;
                }
            }
        }
      while (toapp!=NULL)
        {
          for (ch=(chain_list *)equiv_auto->DATA; ch!=NULL && ch->DATA!=toapp->DATA; ch=ch->NEXT) ;
          if (ch==NULL)
            equiv_auto->DATA=addchain(equiv_auto->DATA, toapp->DATA);
          toapp=delchain(toapp, toapp);
        }
      equiv=addchain(equiv, append((chain_list *)equiv_auto->DATA, toapp));
      equiv_auto=delchain(equiv_auto, equiv_auto);
    }
  return equiv;
}

/*}}}************************************************************************/
/*{{{                    stb_parse_eqvt_clocks()                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
stb_parse_eqvt_clocks(stb_parse *stab)
{BEGIN
  stab->PARSEDFIG->USER   = addptype(stab->PARSEDFIG->USER, STB_EQUIVALENT,
                                     stab->CKEQVT);
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Parse Pin                                          */
/****************************************************************************/
/*{{{                    command_state                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline void
command_state(ttvsig_list *ptttvsig, char state, int isdefault)
{
  stbnode       *ptstbnode_up;
  stbnode       *ptstbnode_dn;
  
  ptstbnode_dn = stb_getstbnode(ptttvsig->NODE);
  ptstbnode_up = stb_getstbnode(ptttvsig->NODE+1);
  if (state == STB_NO_VERIF)
  {
    ptstbnode_dn->CK->VERIF = state;
    ptstbnode_up->CK->VERIF = state;
  }
  else
  {
    if (!isdefault)
    {
      ptstbnode_dn->CK->VERIF &= ~(STB_UP|STB_DN);
      ptstbnode_up->CK->VERIF &= ~(STB_UP|STB_DN);
    }
    ptstbnode_dn->CK->VERIF = state;
    ptstbnode_up->CK->VERIF = state;
  }
}

/*}}}************************************************************************/
/*{{{                    stb_parse_command()                                */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
stb_parse_command(stb_parse *stab, char *ident, char state)
{BEGIN
  ttvsig_list   *ptttvsig;
  chain_list    *ptchain;
  char          *name = namealloc(ident);
  chain_list *cl, *start;
  int tag=1;
  
  
  if ((start=getStartForNode_HT(stab->PARSEDFIG, name, 0))==NULL)
     start=stab->PARSEDFIG->COMMAND, tag=0;
  cl=addchain(NULL, name);
  for (ptchain = start; ptchain; ptchain = ptchain->NEXT)
  {
    ptttvsig         = (ttvsig_list *)ptchain->DATA;
    if (getptype(ptttvsig->USER, STB_WENABLE) != NULL)
      continue ;
    if (tag || ttv_testnetnamemask(stab->PARSEDFIG->FIG, ptttvsig, cl))
    {
      ptttvsig->USER = addptype(ptttvsig->USER, STB_WENABLE, (void *)NULL);
      command_state(ptttvsig,state,0);
      if (!mbk_isregex_name(name))
        break;
    }
  }
  if (tag) freechain(start);
  freechain(cl);
  if (ptchain == NULL && !mbk_isregex_name(name))
  {
    stab->CLEANFUNC();
    stb_error(ERR_NO_COMMAND, name, yylineno, STB_FATAL);
  }
}

/*}}}************************************************************************/
/*{{{                    stb_parse_comdefault()                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
stb_parse_comdefault(stb_parse *stab, char state)
{BEGIN
  ttvsig_list   *ptttvsig;
  chain_list    *ptchain;  

  for (ptchain = stab->PARSEDFIG->COMMAND; ptchain; ptchain = ptchain->NEXT)
  {
    ptttvsig = (ttvsig_list *)ptchain->DATA;
    if (getptype(ptttvsig->USER, STB_WENABLE) != NULL)
      continue ;
    command_state(ptttvsig,state,1);
  }
}

/*}}}************************************************************************/
/*{{{                    Node abstraction                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline ttvsig_list *
getSigForNode(chain_list *ptchain, int type)
{
  ttvsig_list   *res = NULL;
  
  switch (type)
  {
    case STB_P_ONOD :
    case STB_P_INOD :
    case STB_P_LOC :
    case STB_P_MEM :
         res = (ttvsig_list *)ptchain->DATA;
         break;
    case STB_P_NOD :
         res = ((ttvevent_list *)ptchain->DATA)->ROOT;
         break;
  }
  return res;
}

static NameAllocator stb_na;
static ht *stb_sight=NULL;

chain_list *
getStartForNode_HT(stbfig_list *parsedfig, char *ident, int type)
{
  chain_list *cl;
  char buf[1024];
  long l;
  
  if (stb_sight==NULL)
  {
    stb_sight=addht(10000);
    CreateNameAllocator(20000, &stb_na, CASE_SENSITIVE);
    for (cl=parsedfig->MEMORY; cl!=NULL; cl=cl->NEXT)
    {
      ttv_getsigname(parsedfig->FIG, buf, (ttvsig_list *)cl->DATA);
      addhtitem(stb_sight, NameAlloc(&stb_na, buf), (long)cl->DATA);
    }
    for (cl=parsedfig->CONNECTOR; cl!=NULL; cl=cl->NEXT)
    {
      ttv_getsigname(parsedfig->FIG, buf, (ttvsig_list *)cl->DATA);
      addhtitem(stb_sight, NameAlloc(&stb_na, buf), (long)cl->DATA);
    }
    for (cl=parsedfig->NODE; cl!=NULL; cl=cl->NEXT)
    {
      ttv_getsigname(parsedfig->FIG, buf, ((ttvevent_list *)cl->DATA)->ROOT);
      addhtitem(stb_sight, NameAlloc(&stb_na, buf), (long)((ttvevent_list *)cl->DATA)->ROOT);
    }    
  }

  if ((l=gethtitem(stb_sight, NameAlloc(&stb_na, ident)))==EMPTYHT)
    return NULL;
  
  return addchain(NULL, (void *)l);
}

void cleanStartForNode_HT()
{
  if (stb_sight!=NULL)
  {
    delht(stb_sight);
    DeleteNameAllocator(&stb_na);
    stb_sight=NULL;
  }
}

static inline chain_list *
getStartForNode(stbfig_list *parsedfig, int type)
{
  chain_list    *res = NULL;
  switch (type)
  {
    case STB_P_MEM :
         res = parsedfig->MEMORY;
         break;
    case STB_P_NOD :
         res = parsedfig->NODE;
         break;
    case STB_P_INOD :
    case STB_P_ONOD :
         res = parsedfig->CONNECTOR;
         break;
  }
  
  return res;
}

static inline int
contForNode(ttvsig_list *ptttvsig, int type)
{
  int            res = 0;
  
  switch (type)
  {
    case STB_P_MEM :
         res = ((ptttvsig->TYPE & TTV_SIG_C) == TTV_SIG_C ||
                (ptttvsig->TYPE & TTV_SIG_L) != TTV_SIG_L);
         break;
    case STB_P_NOD :
         res = ((ptttvsig->TYPE & TTV_SIG_C) == TTV_SIG_C ||
                (ptttvsig->TYPE & TTV_SIG_L) == TTV_SIG_L);
         break;
    case STB_P_OLOCD :
    case STB_P_ONOD :
         res = ((ptttvsig->TYPE & TTV_SIG_CO) != TTV_SIG_CO);
         break;
    case STB_P_ILOCD :
    case STB_P_INOD :
         res = ((ptttvsig->TYPE & TTV_SIG_CI) != TTV_SIG_CI ||
                (ptttvsig->TYPE & TTV_SIG_CB) == TTV_SIG_CB);
         break;
  }
  return res;
}

static inline int
chkCkForDefaultNode(stbnode *ptstbnode, int type)
{
  int            res = 0;
  
  switch (type)
  {
    case STB_P_ILOCD :
         res  = (ptstbnode->CK && ptstbnode->CK->TYPE == STB_TYPE_CLOCK);
         break;
  }
  return res;
}                       

static inline stbpair_list ***
getPairForDefaultNode(stbnode *ptstbnode, int type)
{
  stbpair_list ***pair = NULL;

  switch (type)
  {
    case STB_P_ILOCD :
         pair = &(ptstbnode->SPECIN);
         break;
    case STB_P_OLOCD :
         pair = &(ptstbnode->SPECOUT);
         break;
  }

  return pair;
}

/*}}}************************************************************************/
/*{{{                    copyIntervallsForDefaultNode()                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline void
copyIntervallsForDefaultNode(stbnode *ptstbnode,
                             stbpair_list *ptstbpair,
                             int edge_option,
                             int phase,
                             stbfig_list *parsedfig,
                             int type, int slope,
                             long d, long u,
                             int mode)
{
  stbpair_list    ***pair;
  
  if (ptstbnode)
  {
    pair                = getPairForDefaultNode(ptstbnode,type); 
    if (!(*pair) && 
        (edge_option == slope || edge_option == STB_SLOPE_ALL))
    {
      *pair             = stb_alloctab(parsedfig->PHASENUMBER);
      if (mode == STB_P_DUPPAIR)
        (*pair)[phase]  = stb_dupstbpairlist(ptstbpair);
      else if (mode == STB_P_ADDPAIR)
        (*pair)[phase]  = stb_addstbpair(NULL, d, u);
    }
  }
}

/*}}}************************************************************************/
/*{{{                    parse_checkPhase()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline char
parse_checkPhase(stb_parse *stab, char *ident, int source_phase)
{
  char       phase;
  
  if (source_phase == STB_NO_INDEX)
    if (stab->DEFAULTPHASE != STB_NO_INDEX)
      phase     = stab->DEFAULTPHASE;
    else if (!stab->PARSEDFIG->CKDOMAIN ||
             !stab->PARSEDFIG->CKDOMAIN->NEXT)
      phase     = (int)stab->PARSEDFIG->PHASENUMBER - 1;
    else
      stb_error(ERR_UNKNOWN_PHASE, ident, yylineno, STB_FATAL);
  else
    phase       = source_phase;

  return phase;
}

/*}}}************************************************************************/
/*{{{                    node()                                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline void
node(stb_parse *stab, char *ident, char edge_option, int source_phase,
     char hz_option, int type)
{
  ttvsig_list   *ptttvsig;
  chain_list    *ptchain;
  int            errcode = 0;
  int            table, done=0;
  char           phase;

#if 1
  // + rapide mais consomme localement +
  ptchain       = getStartForNode_HT(stab->PARSEDFIG, ident, type);

  for ( ; ptchain; ptchain = delchain(ptchain, ptchain))
  {
    ptttvsig    = (ttvsig_list *)ptchain->DATA;
    if (contForNode(ptttvsig,type))
      continue;

    phase         = parse_checkPhase(stab,ident,source_phase);
    if (hz_option == 0)
      table       = STB_TABLE;
    else
      table       = STB_THZ;

    errcode       = stb_addintervals(stab->PARSEDFIG, ptttvsig,
                                     stab->PTSTABLELIST,
                                     stab->PTUNSTABLELIST, table,
                                     edge_option, phase, hz_option,type==STB_P_NOD?1:0);
    if (errcode)
    {
      stab->CLEANFUNC();
      stb_error(errcode, ident, yylineno, STB_FATAL);
    }
    done=1;
  }
  stb_parse_freeStab(stab);
  if (!done)
  {
    stb_error(ERR_NOT_FOUND_NODE, ident, yylineno, STB_NONFATAL);
  }
#else
  char           namebuf[1024];
  // consomme moins (?) et N2/2 pour les noeuds internes
  ptchain       = getStartForNode(stab->PARSEDFIG,type);
  for ( ; ptchain; ptchain = ptchain->NEXT)
  {
    ptttvsig    = getSigForNode(ptchain,type);
    if (contForNode(ptttvsig,type))
      continue;
    ttv_getsigname(stab->PARSEDFIG->FIG, namebuf, ptttvsig);
//    if (namealloc(namebuf) == name)
    if (mbk_casestrcmp(namebuf,ident)==0)
    {
      phase         = parse_checkPhase(stab,ident,source_phase);
      if (hz_option == 0)
        table       = STB_TABLE;
      else
        table       = STB_THZ;

      errcode       = stb_addintervals(stab->PARSEDFIG, ptttvsig,
                                       stab->PTSTABLELIST,
                                       stab->PTUNSTABLELIST, table,
                                       edge_option, phase, hz_option,type==STB_P_NOD?1:0);
      if (errcode)
      {
        stab->CLEANFUNC();
        stb_error(errcode, ident, yylineno, STB_FATAL);
      }
      if (type==STB_P_NOD) break; // pas de regex et ttvsig en double dans la liste
    }
  }
  stb_parse_freeStab(stab);
#endif
}

/*}}}************************************************************************/
/*{{{                    stb_parse_mem()                                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
stb_parse_mem(stb_parse *stab, char *ident, char edge_option,
              int source_phase, char hz_option)
{BEGIN
  node(stab, ident, edge_option, source_phase, hz_option, STB_P_MEM);
}

/*}}}************************************************************************/
/*{{{                    stb_parse_node()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
stb_parse_node(stb_parse *stab, char *ident, char edge_option,
               int source_phase, char hz_option)
{BEGIN
  node(stab, ident, edge_option, source_phase, hz_option, STB_P_NOD);
}

/*}}}************************************************************************/
/*{{{                    stb_parse_outnode()                                */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
stb_parse_outnode(stb_parse *stab, char *ident, char edge_option,
                  int source_phase, char hz_option)
{BEGIN
  node(stab, ident, edge_option, source_phase, hz_option, STB_P_ONOD);
}

/*}}}************************************************************************/
/*{{{                    stb_parse_innode()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
stb_parse_innode(stb_parse *stab, char *ident, char edge_option,
                 int source_phase, char hz_option)
{BEGIN
  node(stab, ident, edge_option, source_phase, hz_option, STB_P_INOD);
}

/*}}}************************************************************************/
/*{{{                    stb_parse_freeStab()                               */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
stb_parse_freeStab(stb_parse *stab)
{BEGIN
  freechain(stab->PTSTABLELIST);
  stab->PTSTABLELIST    = NULL;
  freechain(stab->PTUNSTABLELIST);
  stab->PTUNSTABLELIST  = NULL;
  stab->DEFAULTPHASE    = STB_NO_INDEX;
}

/*}}}************************************************************************/
/*{{{                    locon()                                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline void
locon(stb_parse *stab, char *ident, char edge_option, int source_phase,
      int table)
{
  ttvsig_list   *ptttvsig;
  chain_list    *ptchain;
  char          *name = namealloc(ident);
  char           namebuf[1024];
  int            errcode = 0, cx=0;
  char           phase;

  for (ptchain = stab->PARSEDFIG->CONNECTOR; ptchain; ptchain = ptchain->NEXT)
  {
    ptttvsig        = (ttvsig_list *)ptchain->DATA;

    if (table==STB_SPECIN && contForNode(ptttvsig,STB_P_ILOCD) && (ptttvsig->TYPE & TTV_SIG_CX) != TTV_SIG_CX)
      continue;
    if (table==STB_SPECIN && (ptttvsig->TYPE & TTV_SIG_CB) == TTV_SIG_CB)
      continue;
    if (table==STB_SPECOUT && contForNode(ptttvsig,STB_P_OLOCD) && (ptttvsig->TYPE & TTV_SIG_CX) != TTV_SIG_CX)
      continue;

    if ((ptttvsig->TYPE & TTV_SIG_CX) == TTV_SIG_CX) cx=1;
    
    ttv_getsigname(stab->PARSEDFIG->FIG, namebuf, ptttvsig);

    if (mbk_TestREGEX(namebuf, name))
    {
      // on evite les clocks
      if (getptype(ptttvsig->USER, STB_IS_CLOCK)==NULL)
      {
        phase         = parse_checkPhase(stab,ident,source_phase);
        errcode       = stb_addintervals(stab->PARSEDFIG, ptttvsig,
                                         stab->PTSTABLELIST,
                                         stab->PTUNSTABLELIST,
                                         table, edge_option, phase, 0, table==STB_SPECOUT?1:0);
        if (errcode != 0)
        {
          stab->CLEANFUNC();
          stb_error(errcode, name, yylineno, STB_FATAL);
        }
      }
      if (!mbk_isregex_name(name))
        break;
    }
  }
  stb_parse_freeStab(stab);
  if (ptchain == NULL && !mbk_isregex_name(name))
  {
    stab->CLEANFUNC();
    stb_error(ERR_NO_CONNECTOR, name, yylineno, STB_FATAL);
  }
  else
  {
    if (cx && !mbk_isregex_name(name)) avt_errmsg(STB_ERRMSG, "043", AVT_WARNING, name);
  }
}

/*}}}************************************************************************/
/*{{{                    stb_parse_inlocon()                                */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
stb_parse_inlocon(stb_parse *stab, char *ident, char edge_option,
                  int source_phase)
{BEGIN
  locon(stab,ident,edge_option,source_phase,STB_SPECIN);
}

/*}}}************************************************************************/
/*{{{                    stb_parse_outlocon()                               */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
stb_parse_outlocon(stb_parse *stab, char *ident, char edge_option,
                   int source_phase)
{BEGIN
  locon(stab,ident,edge_option,source_phase,STB_SPECOUT);
}

/*}}}************************************************************************/
/*{{{                    default_locon()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline void
default_locon(stb_parse *stab, char edge_option, int source_phase, int type)
{
  ttvsig_list   *pttarget;
  stbnode       *ptstbnode_up;
  stbnode       *ptstbnode_dn;
  stbpair_list  *ptstbpair;
  chain_list    *ptchain;  
  long           period;
  int            errcode = 0;
  char           phase;

  phase         = parse_checkPhase(stab,"default",source_phase);
  period        = stb_getperiod(stab->PARSEDFIG, phase);
  ptstbpair     = stb_buildintervals(stab->PTSTABLELIST,
                                     stab->PTUNSTABLELIST,
                                     period,&errcode);
  if (errcode != 0)
  {
    stab->CLEANFUNC();
    stb_error(errcode, "default", yylineno, STB_FATAL);
  }
  stb_parse_freeStab(stab);

  for (ptchain = stab->PARSEDFIG->CONNECTOR; ptchain; ptchain = ptchain->NEXT)
  {
    pttarget        = (ttvsig_list *)ptchain->DATA;
    if (contForNode(pttarget,type))
      continue;
    ptstbnode_dn    = stb_getstbnode(pttarget->NODE);
    ptstbnode_up    = stb_getstbnode(pttarget->NODE+1);
    
    if (chkCkForDefaultNode(ptstbnode_dn,type) ||
        chkCkForDefaultNode(ptstbnode_up,type))
      continue;

    copyIntervallsForDefaultNode(ptstbnode_dn,ptstbpair,edge_option,phase,
                                 stab->PARSEDFIG,type,STB_SLOPE_DN,0,0,
                                 STB_P_DUPPAIR);
    copyIntervallsForDefaultNode(ptstbnode_up,ptstbpair,edge_option,phase,
                                 stab->PARSEDFIG,type,STB_SLOPE_UP,0,0,
                                 STB_P_DUPPAIR);
  }
  stb_freestbpair(ptstbpair);
}

/*}}}************************************************************************/
/*{{{                    stb_parse_indefault()                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
stb_parse_indefault(stb_parse *stab, char edge_option, int source_phase)
{BEGIN
  default_locon(stab,edge_option,source_phase,STB_P_ILOCD);
}

/*}}}************************************************************************/
/*{{{                    stb_parse_outdefault()                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
stb_parse_outdefault(stb_parse *stab, char edge_option, int source_phase)
{BEGIN
  default_locon(stab,edge_option,source_phase,STB_P_OLOCD);
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Main                                               */
/****************************************************************************/
/*{{{                    stb_parse_stb_file()                               */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
stb_parse_stb_file(stb_parse *stab)
{BEGIN
  ttvsig_list   *ptttvsig;
  stbfig_list   *parsedfig = stab->PARSEDFIG;
  chain_list    *ptchain;
  stbnode       *ptstbnode;
  stbck         *ptstbck;
  char           edge, phasenumber;
  long           d = 0, u = 0;
  int            i;

  phasenumber   = parsedfig->PHASENUMBER;
  ptstbck       = stb_getclock(parsedfig,phasenumber - 1,NULL,&edge,NULL);
  
  if (ptstbck != NULL)
  {
    if (edge == STB_SLOPE_DN)
    {
      d = ptstbck->SDNMIN;
      u = ptstbck->SDNMAX;
    }
    else
    {
      d = ptstbck->SUPMIN;
      u = ptstbck->SUPMAX;
    }
  }

  for (ptchain = parsedfig->CONNECTOR; ptchain; ptchain = ptchain->NEXT)
  {
    ptttvsig    = (ttvsig_list *)ptchain->DATA;
    if (contForNode(ptttvsig,STB_P_ILOCD))
      continue;
    for (i = 0; i < 2; i++)
    {
      ptstbnode = stb_getstbnode(ptttvsig->NODE+i);
      if (chkCkForDefaultNode(ptstbnode,STB_P_ILOCD))
        continue;

      copyIntervallsForDefaultNode(ptstbnode,NULL,0,phasenumber-1,parsedfig,
                                   STB_P_ILOCD,0,d,u,STB_P_ADDPAIR);
    }
  }
  stab->PARSEDFIG   = NULL; /* finished with it */
}

/*}}}************************************************************************/
/*{{{                    stb_parse_name()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
stb_parse_name(stb_parse *stab, char *ident, int doit)
{BEGIN
  stbfig_list   *ptfig;
  char          *name;
  int            found = FALSE;

  doit=0;
//  if (doit || !done)
//  {
    name                = namealloc(ident);

    /* check that the STB figure exists */   
    for (ptfig = HEAD_STBFIG; ptfig && !found; ptfig = ptfig->NEXT) 
    {
      if (ptfig->FIG->INFO->FIGNAME == name)
      {
        found           = TRUE;
        stab->PARSEDFIG = ptfig;
      }
      if (!found)
        stb_error(ERR_NO_FIGURE, NULL, 0, STB_FATAL);
    }
//  }
}

/*}}}************************************************************************/
/*{{{                    stb_parse_relative_phase()                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
stb_parse_relative_phase(stb_parse *stab, stb_parse_doublet *relative_phase,
                         char relative_type, char *ident, int edge_option)
{BEGIN
  ttvevent_list *ptttvevent;
  ttvevent_list *ptttvck;
  stbck         *ptstbck;
  char          *name;

  name                      = namealloc(ident);
  ptttvevent                = stb_getphase(stab->PARSEDFIG,name,edge_option);
  if (!ptttvevent)
  {
    ptttvevent              = stb_getphase(stab->PARSEDFIG,name,STB_SLOPE_ALL);
    if (ptttvevent)
      if ((ptttvevent->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
        ptttvck             = ptttvevent->ROOT->NODE ;
      else
        ptttvck             = ptttvevent->ROOT->NODE + 1 ;
    else
    {
      stab->CLEANFUNC();
      stb_error(ERR_UNKNOWN_PHASE,name,yylineno,STB_FATAL);
    }
  }
  else
    ptttvck                 = ptttvevent ;
  
  ptstbck                   = stb_getstbnode(ptttvck)->CK;
  if ((ptttvck->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
  {
    relative_phase->start   = ptstbck->SUPMIN;
    relative_phase->end     = ptstbck->SUPMAX;
  }
  else
  {
    relative_phase->start   = ptstbck->SDNMIN;
    relative_phase->end     = ptstbck->SDNMAX;
  }
  
  if (relative_type == STBYY_BEFORE)
  {
    relative_phase->start  += ptstbck->PERIOD;
    relative_phase->end    += ptstbck->PERIOD;
    relative_phase->signe   = (long)-1;
  }
  else
    relative_phase->signe   = (long)1;
  
  ptstbck                   = stb_getstbnode(ptttvevent)->CK;
  stab->DEFAULTPHASE        = ptstbck->CKINDEX;

}

/*}}}************************************************************************/
/*{{{                    stb_parse_phase()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int
stb_parse_phase(stb_parse *stab, char *ident, int edge_option)
{BEGIN
  int            res;
  char          *name;

  if (!ident)
    res     = STB_NO_INDEX;
  else
  {
    name    = namealloc(ident);
    res     = (int)stb_getphaseindex(stab->PARSEDFIG,name,edge_option);
    if (res == STB_NO_INDEX)
    {
      stab->CLEANFUNC();
      stb_error(ERR_UNKNOWN_PHASE,name,yylineno,STB_FATAL);
    }
  }
  
  return res;
}

/*}}}************************************************************************/
/*{{{                    stb_parse_stability()                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
stb_parse_stability(stb_parse *stab, char stability_type, long integer,
                    stb_parse_doublet *relative_phase, chain_list *inschain)
{BEGIN
  chain_list    *res;
  long           stabtime;

  if (relative_phase)
  {
    stabtime        = integer * relative_phase->signe + relative_phase->end;
    res             = addchain(NULL,(void *)stabtime);
  }
  else
    res             = inschain;
      
  if (stability_type == STB_STABLE)
    stab->PTSTABLELIST      = append(stab->PTSTABLELIST,res);
  else
    stab->PTUNSTABLELIST    = append(stab->PTUNSTABLELIST,res);
}

/*}}}************************************************************************/
/*{{{                    stb_parse_wenable()                                */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
stb_parse_wenable(stb_parse *stab)
{BEGIN
  ttvsig_list   *ptttvsig;
  chain_list    *ptchain;

  for (ptchain = stab->PARSEDFIG->COMMAND; ptchain; ptchain = ptchain->NEXT)
  {
    ptttvsig            = (ttvsig_list *)ptchain->DATA;
    if (getptype(ptttvsig->USER, STB_WENABLE) != NULL)
      ptttvsig->USER    = delptype(ptttvsig->USER, STB_WENABLE);
  }
}

/*}}}************************************************************************/
/*}}}************************************************************************/

void
stb_parse_memory(stb_parse *stab, char *ident, int state)
{BEGIN
  ttvsig_list   *ptttvsig;
  chain_list    *ptchain;
  char          *name = namealloc(ident);
  stbnode *n;
  chain_list *cl, *start;
  int tag=1;
  
    if ((start=getStartForNode_HT(stab->PARSEDFIG, name, 0))==NULL)
     start=stab->PARSEDFIG->MEMORY, tag=0;

  cl=addchain(NULL, ident);
  for (ptchain = start; ptchain; ptchain = ptchain->NEXT)
  {
    ptttvsig         = (ttvsig_list *)ptchain->DATA;
    if (tag || ttv_testnetnamemask(stab->PARSEDFIG->FIG, ptttvsig, cl))
    {
      n=stb_getstbnode(ptttvsig->NODE);
      if (state & INF_NOCHECK_SETUP) n->FLAG|=STB_NODE_NOSETUP;
      if (state & INF_NOCHECK_HOLD) n->FLAG|=STB_NODE_NOHOLD;
      n=stb_getstbnode(ptttvsig->NODE+1);
      if (state & INF_NOCHECK_SETUP) n->FLAG|=STB_NODE_NOSETUP;
      if (state & INF_NOCHECK_HOLD) n->FLAG|=STB_NODE_NOHOLD;
      if (!mbk_isregex_name(name))
        break;
    }
  }
  if (tag) freechain(start);
  freechain(cl);
  if (ptchain == NULL && !mbk_isregex_name(name))
  {
    stab->CLEANFUNC();
    stb_error(ERR_NOT_FOUND_MEMORY, name, yylineno, STB_FATAL);
  }
}


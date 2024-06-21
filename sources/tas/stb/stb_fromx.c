/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : AVERTEC global tools                                        */
/*    Fichier : stb_fromx.c                                                 */
/*                                                                          */
/*    © copyright 2000 AVERTEC                                              */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Antony PINTO                                              */
/*                                                                          */
/****************************************************************************/

#include STB_H

#include "stb_parse.h"
#include "stb_fromx.h"
#include "stb_util.h"
#include "stb_directives.h"
#include "stb_falseslack.h"

/****************************************************************************/
/*{{{                                                                       */
/****************************************************************************/
/*{{{                    roundtime()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline void
getroundtime(inffig_list *inf, char *key, char *field, double unit, long *res)
{
  double         tmp;
  
  if (inf_GetDouble(inf,key,field,&tmp))
  {
    if (tmp/ unit * TTV_UNIT<(double)LONG_MIN || tmp/ unit * TTV_UNIT>(double)LONG_MAX)
       avt_errmsg(STB_ERRMSG, "038", AVT_WARNING, tmp, field, key);
    *res    = mbk_long_round(tmp / unit * TTV_UNIT );
  }
}

/*}}}************************************************************************/
/*{{{                    fillckpulse()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void
fillckpulse(stb_fromX_t *fxt, char *ckname, stb_parse_pulse *pulse)
{
  inffig_list   *inf;
  char *master;
  int master_edges=0;
  
  if ((inf = fxt->INFFIG))
  {
    pulse->master_clock=NULL;
    pulse->master_edges=0;
    pulse->supmin=pulse->supmax=pulse->sdnmin=pulse->sdnmax=STB_NO_TIME;
    getroundtime(inf,ckname,INF_MIN_RISE_TIME,fxt->UNIT,&pulse->supmin);
    getroundtime(inf,ckname,INF_MAX_RISE_TIME,fxt->UNIT,&pulse->supmax);
    getroundtime(inf,ckname,INF_MIN_FALL_TIME,fxt->UNIT,&pulse->sdnmin);
    getroundtime(inf,ckname,INF_MAX_FALL_TIME,fxt->UNIT,&pulse->sdnmax);
    pulse->period=STB_NO_TIME;
    getroundtime(inf,ckname,INF_CLOCK_PERIOD ,fxt->UNIT,&pulse->period);
    if (inf_GetString(inf,ckname,INF_MASTER_CLOCK,&master))
    {
      if (inf_GetInt(inf,ckname,INF_MASTER_CLOCK_EDGES,&master_edges))
      {
        pulse->master_clock=master;
        pulse->master_edges=master_edges;
      }
    }
    pulse->supmin%=pulse->period;
    pulse->supmax%=pulse->period;
    pulse->sdnmin%=pulse->period;
    pulse->sdnmax%=pulse->period;
  }
}

/*}}}************************************************************************/
/*{{{                    conversion                                         */
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

static inline char relat(char org)
{
  switch (org)
  {
    case INF_STB_AFTER : return STBYY_AFTER;
    case INF_STB_BEFORE : return STBYY_BEFORE;
    default : printf("1) Ca va pas non!!\n"); return 0;
  }
}

static inline char stabi(char org)
{
  switch (org)
  {
    case INF_STB_STABLE : return STB_STABLE;
    case INF_STB_UNSTABLE : return STB_UNSTABLE;
    default : printf("2) Ca va pas non!!\n"); return 0;
  }
}
static inline char hzstate(char org)
{
  switch (org)
  {
  case INF_STB_HZ_NO_PRECH : return STB_TYPE_PRECHARGE;
  case INF_STB_HZ_NO_EVAL : return STB_TYPE_EVAL;
  case INF_STB_NOTHING : return 0;
  default : printf("3) Ca va pas non!!\n"); return 0;
  }
}


static chain_list *__buildlonglist(chain_list *cl)
{
  chain_list *res;
  res=NULL;
  while (cl!=NULL)
    {
      res=addchain(res, (void *)(long)(mbk_long_round(*(double *)cl->DATA * 1e12*TTV_UNIT)));
      cl=cl->NEXT;
    }
  return reverse(res);
}

/*}}}************************************************************************/
/*{{{                    treatpinin()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void
treatpin(stb_fromX_t *fxt, stb_parse *stab, char *name, char *type, int def)
{
  stb_parse_doublet  relative_phase;
  inf_stb_p_s_stab  *ipsss;
  inf_stb_p_s       *ipss;
  inffig_list       *inf;
  char               relativity, stability;
  int                phase;

  inf               = fxt->INFFIG;
  if (inf_GetPointer(inf,name,type,(void**)&ipss))
    {
      while (ipss!=NULL)
        {

          phase           = stb_parse_phase(stab,ipss->CKNAME,ipss->CKNAME?edge(ipss->CKEDGE):0);
          
          for (ipsss = ipss->STABILITY; ipsss; ipsss = ipsss->NEXT)
            { // foreach stability
              stability     = stabi(ipsss->STABILITY);
              
              if (ipsss->TIME_LIST) // if not an integer list
                stb_parse_stability(stab,stability,0,NULL,__buildlonglist(ipsss->TIME_LIST));
              
              else if (ipsss->CKNAME)
                {
                  relativity  = relat(ipsss->RELATIVITY);
                  stb_parse_relative_phase(stab,&relative_phase,relativity,
                                           ipsss->CKNAME,edge(ipsss->CKEDGE));
                  stb_parse_stability(stab,stability,mbk_long_round(ipsss->DELAY*1e12*TTV_UNIT),
                                      &relative_phase,NULL);
                }
            }
          
          if (type == INF_SPECIN)
            {
              if (def)
                stb_parse_indefault(stab,edge(ipss->DATAEDGE),phase);
              else
                stb_parse_inlocon(stab,name,edge(ipss->DATAEDGE),phase);
            }
          else if (type == INF_SPECOUT)
            {
              if (def)
                stb_parse_outdefault(stab,edge(ipss->DATAEDGE),phase);
              else
                stb_parse_outlocon(stab,name,edge(ipss->DATAEDGE),phase);
            }
          ipss=ipss->NEXT;
        }
    }
}

static void
treatstboutnode(stb_fromX_t *fxt, stb_parse *stab, char *name, char *type)
{
  stb_parse_doublet  relative_phase;
  inf_stb_p_s_stab  *ipsss;
  inf_stb_p_s       *ipss;
  inffig_list       *inf;
  char               relativity, stability;
  int                phase;

  inf               = fxt->INFFIG;
  if (inf_GetPointer(inf,name,type,(void**)&ipss))
  {
    while (ipss!=NULL)
    {
      phase           = stb_parse_phase(stab,ipss->CKNAME,edge(ipss->CKEDGE));
      
      for (ipsss = ipss->STABILITY; ipsss; ipsss = ipsss->NEXT)
      { // foreach stability
        stability     = stabi(ipsss->STABILITY);
  
        if (ipsss->TIME_LIST) // if not an integer list
          stb_parse_stability(stab,stability,0,NULL,__buildlonglist(ipsss->TIME_LIST));
  
        else if (ipsss->CKNAME)
        {
          relativity  = relat(ipsss->RELATIVITY);
          stb_parse_relative_phase(stab,&relative_phase,relativity,
                                   ipsss->CKNAME,edge(ipsss->CKEDGE));
          stb_parse_stability(stab,stability,mbk_long_round(ipsss->DELAY*1e12*TTV_UNIT),
                              &relative_phase,NULL);
        }
      }
  
      if (type == INF_STBOUT_SPECIN)
      {
        stb_parse_innode(stab,name,edge(ipss->DATAEDGE),phase,hzstate(ipss->HZOPTION));
      }
      else if (type == INF_STBOUT_SPECOUT)
      {
        stb_parse_outnode(stab,name,edge(ipss->DATAEDGE),phase,hzstate(ipss->HZOPTION));
      }
      else if (type == INF_STBOUT_SPECMEM)
      {
        stb_parse_mem(stab,name,edge(ipss->DATAEDGE),phase,hzstate(ipss->HZOPTION));
      } 
      else if (type == INF_STBOUT_SPECINODE)
      {
        stb_parse_node(stab,name,edge(ipss->DATAEDGE),phase,hzstate(ipss->HZOPTION));
      }

      ipss=ipss->NEXT;
    }
  }
}
/*}}}************************************************************************/
/*{{{                    stb_fromX()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

int stb_fromX_STEP1_traditional(stb_fromX_t *fxt, stb_parse *stab)
{
  stb_parse_pulse    pulse;
  chain_list        *list, *chainx, *clocklist, *namelist, *equiv_auto, *equiv;
  long               period;
  int                i, cltype;
  char        *type[] = {INF_SPECIN,INF_SPECOUT, NULL};
  char              *def, *domainname, *name, state;
  double val;
  inf_assoc *assoc;
  char clockname[1024];

  fxt->INFFIG   = getloadedinffig(fxt->STBFIG->FIG->INFO->FIGNAME);

  stb_parse_name(stab,fxt->STBFIG->FIG->INFO->FIGNAME,1);
 
  // header
  getroundtime(fxt->INFFIG,INF_STB_HEADER,INF_DEFAULT_PERIOD,
               fxt->UNIT,&stab->PARSEDFIG->CLOCKPERIOD);
  getroundtime(fxt->INFFIG,INF_STB_HEADER,INF_SETUPMARGIN,
               fxt->UNIT,&stab->PARSEDFIG->SETUP);
  getroundtime(fxt->INFFIG,INF_STB_HEADER,INF_HOLDMARGIN,
               fxt->UNIT,&stab->PARSEDFIG->HOLD);

  // clock
  list          = inf_GetEntriesByType(fxt->INFFIG,INF_CLOCK_TYPE,INF_ANY_VALUES);
  for (chainx = list; chainx; chainx = delchain(chainx,chainx))
  {

    if (inf_GetInt(fxt->INFFIG, (char *)chainx->DATA, INF_CLOCK_TYPE, &cltype))
      {
        if ((cltype & INF_CLOCK_VIRTUAL)!=0) stab->VIRTUAL=1; //continue;
        if ((cltype & INF_CLOCK_INVERTED)!=0) stab->INVERTED = 1;
        if ((cltype & INF_CLOCK_IDEAL)!=0) stab->IDEAL = 1;
      }
    // if (inf_GetString(fxt->INFFIG, (char *)chainx->DATA, INF_PREFERED_CLOCK, &def) && def!=NULL) stab->INVERTED = 1;
    fillckpulse(fxt,chainx->DATA,&pulse);
    stb_parse_cklocon(stab,chainx->DATA,&pulse,fxt->INFFIG);
  }

  // equiv clock domain

  if (V_BOOL_TAB[__STB_CREATE_EQUIV_GROUPS].VALUE)
    equiv_auto=stb_auto_create_equivalent_groups(stab);
  else
    equiv_auto=NULL;

  equiv=NULL;

  list          = inf_GetValuesByType(fxt->INFFIG,INF_EQUIV_CLOCK_GROUP);  
  for (chainx = list; chainx; chainx = delchain(chainx,chainx))
    { 
      domainname=(char *)chainx->DATA;
      clocklist=NULL;
      for (namelist=inf_GetEntriesByType(fxt->INFFIG,INF_EQUIV_CLOCK_GROUP,domainname);
           namelist!=NULL;
           namelist=delchain(namelist,namelist))
        {          
          clocklist=stb_parse_ckname_list(stab, clocklist, (char *)namelist->DATA);
        }
      equiv=addchain(equiv, clocklist);
/*      stb_parse_group(clocklist,STB_NO_TIME);    
      stb_parse_eqvt_groups(stab,clocklist);*/
    }

  equiv=stb_merge_equiv_if_needed(equiv, equiv_auto);

  while (equiv!=NULL)
    {
      if (equiv->DATA!=NULL)
        {
           avt_log(LOGSTABILITY,1, "Final equivalent clock group:");
           for (equiv_auto=(chain_list *)equiv->DATA; equiv_auto!=NULL; equiv_auto=equiv_auto->NEXT)
             avt_log(LOGSTABILITY,1, " %s", ttv_getsigname(stab->PARSEDFIG->FIG,clockname,(ttvsig_list *)equiv_auto->DATA));
           avt_log(LOGSTABILITY,1, "\n");
          stb_parse_group((chain_list *)equiv->DATA,STB_NO_TIME);    
          stb_parse_eqvt_groups(stab,(chain_list *)equiv->DATA);
        }
      equiv=delchain(equiv, equiv);
    }

  stb_parse_eqvt_clocks(stab);

  // async clock groups
  list          = inf_GetValuesByType(fxt->INFFIG,INF_ASYNC_CLOCK_GROUP);  
  for (chainx = list; chainx; chainx = delchain(chainx,chainx))
    { 
      domainname=(char *)chainx->DATA;
      clocklist=NULL;
      for (namelist=inf_GetEntriesByType(fxt->INFFIG,INF_ASYNC_CLOCK_GROUP,domainname);
           namelist!=NULL;
           namelist=delchain(namelist,namelist))
        {          
          clocklist=stb_parse_ckname_list(stab, clocklist, (char *)namelist->DATA);
        }
      
      if (!inf_GetDouble(fxt->INFFIG, domainname, INF_ASYNC_CLOCK_GROUP_PERIOD, &val))
        period=STB_NO_TIME;
      else
        period=mbk_long_round(val*1e12);

      stb_parse_group(clocklist,period);
      stb_parse_domain_groups(stab,clocklist);
    }

  // priority clock
  list=inf_GetEntriesByType(fxt->INFFIG,INF_PREFERED_CLOCK,INF_ANY_VALUES);
  for (chainx = list, list=NULL; chainx; chainx = delchain(chainx,chainx))
    { 
      inf_GetString(fxt->INFFIG, (char *)chainx->DATA, INF_PREFERED_CLOCK, &def);
      list=addchain (list,stb_parse_ckprio_pair(stab,(char *)chainx->DATA,def));
    }
  stab->PARSEDFIG->PRIOCLOCK = list;

  //---->
  stb_parse_ckdeclar(fxt, stab);
  //<----

  // disable
  if (inf_GetPointer(fxt->INFFIG, INF_DISABLE_PATH, INF_ASSOCLIST, (void **)&list))
    {
      for (chainx = list; chainx; chainx = chainx->NEXT)
        { 
          assoc=(inf_assoc *)chainx->DATA;
          stb_ckdisable(stab->PARSEDFIG, assoc->orig, assoc->dest);
        }
    }

  // verif
  stb_parse_wenable( stab );

  list          = inf_GetEntriesByType(fxt->INFFIG,INF_VERIF_STATE,INF_ANY_VALUES);
  for (chainx = list; chainx; chainx = delchain(chainx,chainx))
    { 
      name=(char *)chainx->DATA;
      inf_GetString(fxt->INFFIG, name, INF_VERIF_STATE, &def);
      if (strcmp(def, INF_VERIFUP)==0) state=STB_UP|STB_VERIF_STATE;
      else if (strcmp(def, INF_VERIFDOWN)==0) state=STB_DN|STB_VERIF_STATE;
      else if (strcmp(def, INF_VERIFRISE)==0) state=STB_UP|STB_VERIF_EDGE;
      else if (strcmp(def, INF_VERIFFALL)==0) state=STB_DN|STB_VERIF_EDGE;
      else if (strcmp(def, INF_NOVERIF)==0) state=STB_NO_VERIF;
      else { printf("Ca va pas non!!\n"); continue;}
      stb_parse_command(stab,name,state);
    }
  
  // nosetup/hold
  list          = inf_GetEntriesByType(fxt->INFFIG,INF_NOCHECK,INF_ANY_VALUES);
  for (chainx = list; chainx; chainx = delchain(chainx,chainx))
    { 
      name=(char *)chainx->DATA;
      inf_GetInt(fxt->INFFIG, name, INF_NOCHECK, &i);
      stb_parse_memory(stab,name,i);
    }

  i                 = 0;
  def               = namealloc("default");
  while (type[i])
  {
    list              = inf_GetEntriesByType(fxt->INFFIG,type[i],INF_ANY_VALUES);
    for (chainx = list; chainx; chainx = delchain(chainx,chainx))
      if (chainx->DATA != def)
        treatpin(fxt,stab,chainx->DATA,type[i],0);
    treatpin(fxt,stab,def,type[i],1);
    i ++;
  }

//  ttv_setdirectives(fxt->STBFIG ,fxt->INFFIG);
  stb_setfalseslack(fxt->STBFIG ,fxt->INFFIG);
  stb_setprobability(fxt->STBFIG ,fxt->INFFIG);

  return 0;
}

int stb_fromX_STEP2_stability(stb_fromX_t *fxt, stb_parse *stab)
{
  chain_list        *list, *chainx;
  int                i;
  char        *typeout[] = {INF_STBOUT_SPECIN,INF_STBOUT_SPECOUT, INF_STBOUT_SPECMEM, INF_STBOUT_SPECINODE, NULL};

  fxt->INFFIG   = getloadedinffig(fxt->STBFIG->FIG->INFO->FIGNAME);

  i                 = 0;
  while (typeout[i])
  {
    list = inf_GetEntriesByType(fxt->INFFIG,typeout[i],INF_ANY_VALUES);
    for (chainx = list; chainx; chainx = delchain(chainx,chainx))
      treatstboutnode(fxt,stab,chainx->DATA,typeout[i]);
    i ++;
  }

  return 0;
}

int stb_fromX_STEP3_finish(stb_fromX_t *fxt, stb_parse *stab)
{
  stb_parse_stb_file(stab);
  // to avoid warning:
  fxt=NULL;
  return 0;
}


int stb_fromX(stb_fromX_t *fxt, stb_parse *stab)
{
  stb_fromX_STEP1_traditional(fxt, stab);
  stb_fromX_STEP2_stability(fxt, stab);
  stb_fromX_STEP3_finish(fxt, stab);
  return 0;
}


/*}}}************************************************************************/
/*}}}************************************************************************/

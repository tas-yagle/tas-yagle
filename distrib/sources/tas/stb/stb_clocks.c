#include <stdlib.h>

#include MUT_H
#include STB_H

#include "stb_util.h"
#include "stb_error.h"
#include "stb_init.h"
#include "stb_transfer.h"
#include "stb_overlap.h"
#include "stb_clock_tree.h"

typedef struct
{
  long min, max;
} d_minmax;

typedef struct
{
  d_minmax delays[2];
  ttvsig_list *master;
} d_minmax_rf;


int stb_find_path_from_clock_to_clock(ttvfig_list *tvf, ttvsig_list *source, ttvsig_list *dest, long type, int start_event[2], d_minmax_rf *result)
{
  chain_list *clocks;
  ptype_list *pt, *cmdlist;
  ttvpath_list *path, *rpath;
  ttvevent_list *cmdevent;
  int i, minmax;
  long thisdelay, find;
  char buf0[1024], buf1[1024];
  stb_propagated_clock_to_clock *spctc;

  result->master=source;
  for (i=0; i<2; i++)
    result->delays[i].min=result->delays[i].max=TTV_NOTIME;

  if ((pt=getptype(dest->USER, STB_IS_CLOCK))!=NULL)
    {
      spctc=(stb_propagated_clock_to_clock *)pt->DATA;
      if (spctc->haslatency)
      {
        result->delays[0].min=spctc->latencies.SDNMIN;
        result->delays[0].max=spctc->latencies.SDNMAX;
        result->delays[1].min=spctc->latencies.SUPMIN;
        result->delays[1].max=spctc->latencies.SUPMAX;
        return 1;
      }
    }

  if (getptype (source->USER, STB_IDEAL_CLOCK)!=NULL || getptype (source->USER, STB_VIRTUAL_CLOCK)!=NULL) return 0;

  clocks=addchain(NULL, source);

  if ((dest->TYPE & TTV_SIG_L)!=0)
    {
      for (i=0; i<2; i++)
        {
          for (minmax=0; minmax<2; minmax++)
            {
              if (minmax==0) find=TTV_FIND_MIN; else find=TTV_FIND_MAX;

              cmdlist = ttv_getlatchaccess(tvf,&dest->NODE[i],find) ;
              
              for(pt=cmdlist ; pt!=NULL ; pt=pt->NEXT)
                {
                  cmdevent = (ttvevent_list *)pt->DATA ;
                  
                  ttv_search_mode(1, TTV_MORE_OPTIONS_MUST_BE_CLOCK);
                  path = ttv_getpathnocross (tvf, NULL, cmdevent->ROOT, clocks, TTV_DELAY_MAX, TTV_DELAY_MIN, TTV_FIND_SIG |find | TTV_FIND_CMD | type);
                  ttv_search_mode(0, TTV_MORE_OPTIONS_MUST_BE_CLOCK);
              
                  for (rpath=path; rpath!=NULL; rpath=rpath->NEXT)
                    {
                      if (rpath->ROOT==cmdevent && (rpath->NODE->TYPE & start_event[i])!=0) break;
                    }
                  
                  if (rpath!=NULL)
                    {
                      thisdelay=rpath->DELAY;
                      if (minmax==0)
                        {
                          if (pt->TYPE!=0) thisdelay+=((ttvline_list *)pt->TYPE)->VALMIN;
                          if (result->delays[i].min==TTV_NOTIME || thisdelay<result->delays[i].min) result->delays[i].min=thisdelay;
                        }
                      else
                        {
                          if (pt->TYPE!=0) thisdelay+=((ttvline_list *)pt->TYPE)->VALMAX;
                          if (result->delays[i].max==TTV_NOTIME || thisdelay<result->delays[i].max) result->delays[i].max=thisdelay;
                        }
                    }
                  ttv_freepathlist(path);
                }
              freeptype(cmdlist);
            }
        }
    }
  else
    {
      int maxperiod ;
      maxperiod = V_INT_TAB[__TTV_MAX_PATH_PERIOD].VALUE ;
      V_INT_TAB[__TTV_MAX_PATH_PERIOD].VALUE = 0 ;

      for (minmax=0; minmax<2; minmax++)
        {
          if (minmax==0) find=TTV_FIND_MIN; else find=TTV_FIND_MAX;
          
          ttv_search_mode(1, TTV_MORE_OPTIONS_MUST_BE_CLOCK);
          TTV_QUIET_MODE=1;
          path = ttv_getaccess (tvf, NULL, dest, NULL, clocks, TTV_DELAY_MAX, TTV_DELAY_MIN, TTV_FIND_SIG | find | TTV_FIND_CMD | type);
          TTV_QUIET_MODE=0;
          ttv_search_mode(0, TTV_MORE_OPTIONS_MUST_BE_CLOCK);
          
          for (i=0; i<2; i++)
            {
              for (rpath=path; rpath!=NULL; rpath=rpath->NEXT)
                {
                  if (rpath->ROOT==&dest->NODE[i] && rpath->NODE->ROOT==source && (rpath->NODE->TYPE & start_event[i])!=0) break;
                }
              
              if (rpath!=NULL)
                {
                  thisdelay=rpath->DELAY;
                  if (minmax==0)
                    {
                      if (result->delays[i].min==TTV_NOTIME || thisdelay<result->delays[i].min) result->delays[i].min=thisdelay;
                    }
                  else
                    {
                      if (result->delays[i].max==TTV_NOTIME || thisdelay<result->delays[i].max) result->delays[i].max=thisdelay;
                    }
                }
            }
          ttv_freepathlist(path);
        }
      V_INT_TAB[__TTV_MAX_PATH_PERIOD].VALUE = maxperiod ;
    }

  avt_log(LOGSTABILITY,2, "Paths from '%s' to '%s'\n",ttv_getsigname(tvf,buf0,source), ttv_getsigname(tvf,buf1,dest));
  for (i=0; i<2; i++)
    {
      avt_log(LOGSTABILITY,2, "\t%s-%s : min=",(start_event[i] & TTV_NODE_UP)!=0?"rise":"fall", i?"rise":"fall");
      if (result->delays[i].min==TTV_NOTIME) avt_log(LOGSTABILITY,2,"NOT FOUND");
      else avt_log(LOGSTABILITY,2,"%g",result->delays[i].min*1e-12/TTV_UNIT);
      avt_log(LOGSTABILITY,2, "  max=");
      if (result->delays[i].max==TTV_NOTIME) avt_log(LOGSTABILITY,2,"NOT FOUND");
      else avt_log(LOGSTABILITY,2,"%g",result->delays[i].max*1e-12/TTV_UNIT);
      avt_log(LOGSTABILITY,2, "\n");
    }

  freechain(clocks);
  return 1;
}

void stb_get_clock_local_latency(ttvsig_list *tvs, long *rmin, long *rmax, long *fmin, long *fmax)
{
  d_minmax_rf *cur;
  ptype_list *pt0;
  *rmin=*rmax=*fmin=*fmax=0;

  if ((pt0=getptype(tvs->USER, STB_CLOCK_LOCAL_LATENCY))!=NULL)
    {
      cur=(d_minmax_rf *)pt0->DATA;      
      if (cur->delays[0].min!=TTV_NOTIME) *fmin=cur->delays[0].min;
      if (cur->delays[0].max!=TTV_NOTIME) *fmax=cur->delays[0].max;
      if (cur->delays[1].min!=TTV_NOTIME) *rmin=cur->delays[1].min;
      if (cur->delays[1].max!=TTV_NOTIME) *rmax=cur->delays[1].max;
    }
}

static void recur_compute_clock_latency(ttvsig_list *tvs, d_minmax_rf *sum)
{
  int i;
  ptype_list *pt0;
  d_minmax_rf *cur;

  if ((pt0=getptype(tvs->USER, STB_CLOCK_LOCAL_LATENCY))!=NULL)
    {
      cur=(d_minmax_rf *)pt0->DATA;
      for (i=0; i<2; i++)
        {
          if (cur->delays[i].min!=TTV_NOTIME) sum->delays[i].min+=cur->delays[i].min;
          if (cur->delays[i].max!=TTV_NOTIME) sum->delays[i].max+=cur->delays[i].max;
        }
      if (cur->master!=NULL) recur_compute_clock_latency(cur->master, sum);
    }
}

int stb_compute_clock_latency(stbfig_list *sb, long type)
{
  chain_list *cl, *chain, *chainsig;
  int start_event[2]={TTV_NODE_UP, TTV_NODE_UP};
  ptype_list *pt, *pt0;
  ttvsig_list *tvs, *clock;
  d_minmax_rf dmrf, *dmrf_tmp;
  stb_propagated_clock_to_clock *spctc;
  int i;
  stbnode *n;
  char buf0[1024], buf1[1024];

  for (cl=sb->CLOCK; cl!=NULL; cl=cl->NEXT)
    {
      tvs=(ttvsig_list *)cl->DATA;
      if ((pt=getptype(tvs->USER, STB_IS_CLOCK))!=NULL)
        {
          spctc=(stb_propagated_clock_to_clock *)pt->DATA;
          if (spctc->master!=NULL)
            {
              clock=NULL;

              chain=addchain(NULL, spctc->master);
              for (chainsig=sb->CLOCK; chainsig!=NULL; chainsig=chainsig->NEXT)
                {
                  clock=(ttvsig_list *)chainsig->DATA;
                  if (getptype(clock->USER, STB_VIRTUAL_CLOCK)==NULL)
                    {
                      if (ttv_testnetnamemask(sb->FIG, clock, chain)) break;
                    }
                  else
                    {
                      if (mbk_TestREGEX(clock->NETNAME, spctc->master)) break;
                    }
                }
              freechain(chain);
              if (chain==NULL) clock=NULL;
              
              if ((spctc->edges & 2)!=0) start_event[1]=TTV_NODE_UP; else start_event[1]=TTV_NODE_DOWN;
              if ((spctc->edges & 1)!=0) start_event[0]=TTV_NODE_UP; else start_event[0]=TTV_NODE_DOWN;

              if (getptype(tvs->USER, STB_INVERTED_CLOCK)!=NULL)
                {
                  long l;
                  l=start_event[0];
                  start_event[0]=start_event[1];
                  start_event[1]=l;
                }

              if (clock!=NULL)
                {
                  stb_find_path_from_clock_to_clock(sb->FIG, clock, tvs, type, start_event, &dmrf);
                  if ((pt0=getptype(tvs->USER, STB_CLOCK_LOCAL_LATENCY))!=NULL) dmrf_tmp=(d_minmax_rf *)pt0->DATA;
                  else
                    {
                      dmrf_tmp=(d_minmax_rf *)mbkalloc(sizeof(d_minmax_rf));
                      tvs->USER=addptype(tvs->USER, STB_CLOCK_LOCAL_LATENCY, dmrf_tmp);
                    }
                  memcpy(dmrf_tmp, &dmrf, sizeof(d_minmax_rf));
                }
              else
                {
                  spctc->master=NULL;
                }
            }
        }
    }

  for (cl=sb->CLOCK; cl!=NULL; cl=cl->NEXT)
    {
      tvs=(ttvsig_list *)cl->DATA;
      for (i=0; i<2; i++)
        {
          dmrf.delays[i].min=dmrf.delays[i].max=0;
        }
      recur_compute_clock_latency(tvs, &dmrf);

      if ((pt=getptype(tvs->USER, STB_IS_CLOCK))!=NULL)
        {
          spctc=(stb_propagated_clock_to_clock *)pt->DATA;
          if (spctc->master!=NULL)
            {
              avt_log(LOGSTABILITY,2, "Latency computed from '%s' to '%s'", spctc->master, ttv_getsigname(sb->FIG,buf1,tvs));
              avt_log(LOGSTABILITY,2, ": RISE(%g,%g) FALL(%g,%g)\n",dmrf.delays[1].min*1e-12/TTV_UNIT,dmrf.delays[1].max*1e-12/TTV_UNIT,dmrf.delays[0].min*1e-12/TTV_UNIT,dmrf.delays[0].max*1e-12/TTV_UNIT);
              
              if ((pt=getptype(tvs->USER, STB_IS_CLOCK))!=NULL)
                {
                  spctc=(stb_propagated_clock_to_clock *)pt->DATA;
                  for (i=0; i<2; i++)
                    {
                      n= stb_getstbnode(tvs->NODE+i);
                      n->CK->SUPMAX=spctc->original_waveform.SUPMAX+dmrf.delays[1].max;
                      n->CK->SUPMIN=spctc->original_waveform.SUPMIN+dmrf.delays[1].min;
                      n->CK->SDNMAX=spctc->original_waveform.SDNMAX+dmrf.delays[0].max;
                      n->CK->SDNMIN=spctc->original_waveform.SDNMIN+dmrf.delays[0].min;
                    }
                  avt_log(LOGSTABILITY,2, "\tNew waveform: RISE(%g,%g) FALL(%g,%g)\n",n->CK->SUPMIN*1e-12/TTV_UNIT, n->CK->SUPMAX*1e-12/TTV_UNIT, n->CK->SDNMIN*1e-12/TTV_UNIT, n->CK->SDNMAX*1e-12/TTV_UNIT);

                }
            }
        }
    }


  return 0;
}

chain_list *get_clock_latency_tree(ttvevent_list *tve, long type, chain_list *cl)
{
  int i, mask;
  ptype_list *pt;
  d_minmax_rf *cur;
  long delay, date;
  stb_propagated_clock_to_clock *spctc;
  stbnode *n;
  stb_clock_latency_information *scli;

  if ((pt=getptype(tve->ROOT->USER, STB_IS_CLOCK))!=NULL)
    {
      spctc=(stb_propagated_clock_to_clock *)pt->DATA;
      if (tve->TYPE & TTV_NODE_UP) { i=1; mask=2; } else { i=0; mask=1; }
      if (getptype(tve->ROOT->USER, STB_INVERTED_CLOCK)!=NULL)
        {
          if (mask==1) mask=2; else mask=1;
        }
      if ((pt=getptype(tve->ROOT->USER, STB_CLOCK_LOCAL_LATENCY))!=NULL)
        {
          cur=(d_minmax_rf *)pt->DATA;
                           
          if (cur->master!=NULL)
            {
              if (type & TTV_FIND_MAX) delay=cur->delays[i].max; else delay=cur->delays[i].min;
              if (delay==TTV_NOTIME) delay=0;
              
              if ((spctc->edges & mask)!=0) mask=1; else mask=0;
              if (cl==NULL)
                {
                  n=stb_getstbnode(tve);
                  if (i==0)
                    {
                      if (type & TTV_FIND_MAX) date=n->CK->SDNMAX; else date=n->CK->SDNMIN;
                    }
                  else
                    {
                      if (type & TTV_FIND_MAX) date=n->CK->SUPMAX; else date=n->CK->SUPMIN;
                    }                  
                  scli=(stb_clock_latency_information *)mbkalloc(sizeof(stb_clock_latency_information));
                  scli->date=date;
                  scli->delay=TTV_NOTIME;
                  scli->period=n->CK->PERIOD;
                  scli->clock_event=tve;
                  cl=addchain(cl, scli);
                }

              n=stb_getstbnode(&cur->master->NODE[mask]);
              if (mask==0)
                {
                  if (type & TTV_FIND_MAX) date=n->CK->SDNMAX; else date=n->CK->SDNMIN;
                }
              else
                {
                  if (type & TTV_FIND_MAX) date=n->CK->SUPMAX; else date=n->CK->SUPMIN;
                }
              scli=(stb_clock_latency_information *)mbkalloc(sizeof(stb_clock_latency_information));
              scli->date=date;
              scli->delay=delay;
              scli->period=n->CK->PERIOD;
              scli->clock_event=&cur->master->NODE[mask];
              cl=addchain(cl, scli);
              cl=get_clock_latency_tree(&cur->master->NODE[mask], type, cl);
            }
        }
      else
        {
          n=stb_getstbnode(tve);
          if (mask==0)
            {
              if (type & TTV_FIND_MAX) date=n->CK->SDNMAX, delay=spctc->latencies.SDNMAX;
              else date=n->CK->SDNMIN, delay=spctc->latencies.SDNMIN; 
            }
          else
            {
              if (type & TTV_FIND_MAX) date=n->CK->SUPMAX, delay=spctc->latencies.SUPMAX;
              else date=n->CK->SUPMIN, delay=spctc->latencies.SUPMIN;
            }

          if (delay!=0)
            {
              scli=(stb_clock_latency_information *)mbkalloc(sizeof(stb_clock_latency_information));
              scli->date=date-delay;
              scli->delay=delay;
              scli->period=n->CK->PERIOD;
              scli->clock_event=tve;
              cl=addchain(cl, scli);
            }
        }
    }
  return cl;
}


ptype_list *stb_build_generated_clock_info(chain_list *clocklist)
{
  int i;
  ptype_list *pt=NULL;
  ttvsig_list *tvs;
  chain_list *min, *max;
  while (clocklist!=NULL)
  {
    tvs=(ttvsig_list *)clocklist->DATA;
    for (i=0; i<2; i++)
    {
      min=get_clock_latency_tree(&tvs->NODE[i], TTV_FIND_MIN, NULL);
      max=get_clock_latency_tree(&tvs->NODE[i], TTV_FIND_MAX, NULL);
      pt=addptype(pt, (long)&tvs->NODE[i], addchain(addchain(NULL, max), min)); //min puis max
    }
    clocklist=clocklist->NEXT;
  }
  return pt;
}

void stb_free_built_generated_clock_info(ptype_list *geneclockinfo)
{
  int i;
  ptype_list *pt;
  ttvsig_list *tvs;
  chain_list *cl, *ch;
  for (pt=geneclockinfo; pt!=NULL; pt=pt->NEXT)
  {
    cl=(chain_list *)pt->DATA;
    while (cl!=NULL)
    {
      for (ch=(chain_list *)cl->DATA; ch!=NULL; ch=ch->NEXT)
        mbkfree(ch->DATA);
      freechain((chain_list *)cl->DATA);
      cl=delchain(cl, cl);
    }
  }
  freeptype(geneclockinfo);
}

long stb_get_generated_clock_skew(ptype_list *geneclockinfo, ttvevent_list *tve0, ttvevent_list *tve1, ttvevent_list **common_node)
{
  ptype_list *pt;
  chain_list *c0, *c1;
  long minmax=TTV_NOTIME;
  stb_clock_latency_information *scli0, *scli1;
  *common_node=NULL;
  for (pt=geneclockinfo; pt!=NULL && pt->TYPE!=(long)tve0; pt=pt->NEXT) ;
  if (pt==NULL) return TTV_NOTIME;
  c0=(chain_list *)((chain_list *)pt->DATA)->DATA; // min
  for (pt=geneclockinfo; pt!=NULL && pt->TYPE!=(long)tve1; pt=pt->NEXT) ;
  if (pt==NULL) return TTV_NOTIME;
  c1=(chain_list *)((chain_list *)pt->DATA)->NEXT->DATA; // max
  while (c0!=NULL && c1!=NULL)
  {
    scli0=(stb_clock_latency_information *)c0->DATA;
    scli1=(stb_clock_latency_information *)c1->DATA;
    if (scli0->clock_event==scli1->clock_event) minmax=scli1->date-scli0->date, *common_node=scli1->clock_event;
    else break;
    c0=c0->NEXT;
    c1=c1->NEXT;
  }
  return minmax;
}



#include MUT_H
#include STB_H

#include "stb_util.h"
#include "stb_transfer.h"
#include "stb_error.h"
#include "stb_directives.h"
#include "stb_falseslack.h"
#include "stb_relaxation_correction.h"
#include "stb_debug.h"

typedef struct node_path_info_per_phase
{
  struct node_path_info_per_phase *next;  
  long delaymax, delaymin;
  long movemax, movemin;
  long startmin, startmax;
  char phase;
} node_path_info_per_phase;

typedef struct node_path_info
{
  struct node_path_info *next;
  node_path_info_per_phase *PP;
  char output_phase;
  ttvevent_list *start, *cmd;
} node_path_info;


static node_path_info *create_node_path_info()
{
  node_path_info *npi;
  npi=(node_path_info *)mbkalloc(sizeof(node_path_info));
  npi->PP=NULL;
  npi->output_phase=STB_NO_INDEX;
  npi->start=NULL;
  npi->next=NULL;
  return npi;
}

static node_path_info_per_phase *create_node_path_info_PP()
{
  node_path_info_per_phase *npipp;
  npipp=(node_path_info_per_phase *)mbkalloc(sizeof(node_path_info_per_phase));
  npipp->movemax=npipp->movemin=0;
  npipp->startmin=npipp->startmax=0;
  npipp->delaymin=npipp->delaymax=TTV_NOTIME;
  npipp->phase=STB_NO_INDEX;
  return npipp;
}

static valid_range_info *give_valid_range_info(ttvline_list *tl, int nbindex)
{
  ptype_list *pt;
  valid_range_info *vri;
  int i;

  if ((pt=getptype(tl->USER, STB_TRANSFERT_CORRECTION_PTYPE))!=NULL)
    return (valid_range_info *)pt->DATA;

  vri=(valid_range_info *)mbkalloc(sizeof(valid_range_info)*nbindex);
  for (i=0; i<nbindex; i++)
  {
//    vri[i].validmin=vri[i].validmax=TTV_NOTIME;
    vri[i].realpair=NULL;
  }

  tl->USER=addptype(tl->USER, STB_TRANSFERT_CORRECTION_PTYPE, vri);
  return vri;
}

valid_range_info *get_valid_range_info(ttvline_list *tl)
{
  ptype_list *pt;

  if ((pt=getptype(tl->USER, STB_TRANSFERT_CORRECTION_PTYPE))!=NULL)
    return (valid_range_info *)pt->DATA;

  return NULL;
}


void stb_clean_relax_correction_info (stbfig_list *stbfig)
{
  ttvevent_list *event;
  ttvline_list *line;
  char level;
  chain_list *cl;
  ptype_list *pt;
  valid_range_info *vri;
  int i;

  for (cl=stbfig->NODE; cl!=NULL; cl=cl->NEXT)
    {
      event=(ttvevent_list *)cl->DATA;
      if ((stbfig->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST)
        level = stbfig->FIG->INFO->LEVEL;
      else
        level = event->ROOT->ROOT->INFO->LEVEL;

      if (stbfig->GRAPH == STB_RED_GRAPH)
        {
          ttv_expfigsig (stbfig->FIG, event->ROOT, level, stbfig->FIG->INFO->LEVEL,
                         TTV_STS_CL_PJT, TTV_FILE_TTX);
          line = event->INPATH;
        }
      else
        {
          ttv_expfigsig (stbfig->FIG, event->ROOT, level, stbfig->FIG->INFO->LEVEL,
                         TTV_STS_CLS_FED, TTV_FILE_DTX);
          line = event->INLINE;
        }

      for (; line; line = line->NEXT)
        {          
          if ((pt=getptype(line->USER, STB_TRANSFERT_CORRECTION_PTYPE))!=NULL)
            {
              vri=(valid_range_info *)pt->DATA;
              for (i=0; i<(int)stbfig->PHASENUMBER; i++)
              {
                stb_freestbpair(vri[i].realpair);
              }
              mbkfree(vri);
              line->USER=delptype(line->USER, STB_TRANSFERT_CORRECTION_PTYPE);
            }
        }
    }
}

void stb_clean_relax_correction_path_info (stbfig_list *stbfig)
{
  ttvevent_list *event;
  chain_list *cl;
  node_path_info *npi, *nnpi;
  node_path_info_per_phase *npipp, *nnpipp;
  
  ptype_list *pt;

  for (cl=stbfig->NODE; cl!=NULL; cl=cl->NEXT)
    {
      event=(ttvevent_list *)cl->DATA;
      if ((pt=getptype(event->USER, STB_PATH_LIST))!=NULL)
        {
          for (npi=(node_path_info *)pt->DATA; npi!=NULL; npi=nnpi)
            {
              nnpi=npi->next;
              for (npipp=npi->PP; npipp!=NULL; npipp=nnpipp)
                {
                  nnpipp=npipp->next;
                  mbkfree(npipp);
                }
              mbkfree(npi);
            }
          event->USER=delptype(event->USER, STB_PATH_LIST);
        }
    }
}

node_path_info *stb_assign_paths(stbfig_list *stbfig, ttvevent_list *tve)
{
  long find, l;
  int oldprecisionlevel;
  ttvpath_list *path, *scanpath;
  ptype_list *ptype;
  ht *startht;
  node_path_info *pathl=NULL, *npi;
  node_path_info_per_phase *npipp;
  chain_list *cl;
  ptype_list *pt;
  ttvevent_list *start;
  ttvpath_stb_stuff *tps;
  char ophase;

  if ((pt=getptype(tve->USER, STB_PATH_LIST))!=NULL) return (node_path_info *)pt->DATA;


  ttv_search_mode(1, TTV_MORE_OPTIONS_KEEP_PHASE_INFO);

  oldprecisionlevel=ttv_SetPrecisionLevel(0);

  startht=addht(100);

  if(stbfig->GRAPH == STB_RED_GRAPH)
    find = TTV_FIND_PATH ;
  else
    find = TTV_FIND_LINE ;

  if (tve->TYPE & TTV_NODE_UP)
    find |= TTV_FIND_NOT_UPDW | TTV_FIND_NOT_DWDW;
  else
    find |= TTV_FIND_NOT_UPUP | TTV_FIND_NOT_DWUP;
        
  if (stbfig->CLOCK && (stbfig->FLAGS & STB_HAS_FALSE_ACCESS)!=0 && V_BOOL_TAB[__STB_HANDLE_FALSE_ACCESS].VALUE)
  {
    if(ttv_canbeinfalsepath(tve, 'o'))
    {
      if (ttv_hasaccessfalsepath(stbfig->FIG, tve, stbfig->CLOCK))
      {
        ttv_set_ttv_getpath_clock_list(stbfig->CLOCK);
        ttv_activate_path_and_access_mode(1);
        find|=TTV_FIND_ACCESS;
      }
    }
  }
     
  path = ttv_getpathnocross_v2(stbfig->FIG,NULL,tve->ROOT,NULL, TTV_DELAY_MAX,TTV_DELAY_MIN,TTV_FIND_MAX|find,0) ;
  
  for(scanpath = path ; scanpath != NULL ; scanpath = scanpath->NEXT)
    {
      if ((scanpath->TYPE & TTV_FIND_HZ) == TTV_FIND_HZ) continue ;
      if (scanpath->LATCH==NULL && getptype(scanpath->NODE->ROOT->USER, STB_IS_CLOCK)!=NULL
          && (scanpath->TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_FLAG_THRU_FILTER_FOUND)==0)
        continue;
      
      if ((scanpath->ROOT->ROOT->TYPE & TTV_SIG_R) != 0)
        {
          ptype=getptype(scanpath->USER, TTV_PATH_PREVIOUS_NODE);
          scanpath->CMD=(ttvevent_list *)ptype->DATA;
        }

      start=scanpath->LATCH?scanpath->LATCH:scanpath->NODE;
      if ((l=gethtitem(startht, start))!=EMPTYHT)
        {
          for (cl=(chain_list *)l; cl!=NULL && (npi=(node_path_info *)cl->DATA)->cmd!=scanpath->CMD; cl=cl->NEXT) ;
          if (cl!=NULL) continue;
          cl=(chain_list *)l;
        }
      else cl=NULL;
        
      npi=create_node_path_info();
      npi->start=start;
      npi->cmd=scanpath->CMD;
      npi->next=pathl;
      pathl=npi;
      addhtitem(startht, start, (long)addchain(cl, npi));
      if ((pt=getptype(scanpath->USER, TTV_PATH_OUTPUT_PHASE))!=NULL)
        {
          if ((ophase=(char)(long)pt->DATA)==TTV_NO_PHASE) ophase=STB_NO_INDEX;
          npi->output_phase=ophase;
        }
      else npi->output_phase=STB_NO_INDEX;
      
      pt=getptype(scanpath->USER,TTV_PATH_PHASE_INFO);
      if (pt!=NULL)
        {
          for (tps=(ttvpath_stb_stuff *)pt->DATA; tps!=NULL; tps=tps->NEXT)
            {
              npipp=create_node_path_info_PP();
              npipp->next=npi->PP;
              npi->PP=npipp;
              npipp->phase=tps->STARTPHASE;
              npipp->startmax=tps->STARTTIME;
              npipp->delaymax=scanpath->DELAY-scanpath->CLOCKPATHDELAY;
              npipp->movemax=tps->PERIOD_CHANGE;
            }
          ttv_freepathstblist((ttvpath_stb_stuff *)pt->DATA);
        }
      else
        {
          npipp=create_node_path_info_PP();
          npipp->next=npi->PP;
          npi->PP=npipp;
          npipp->phase=STB_NO_INDEX;
          npipp->startmax=TTV_NOTIME;
          npipp->delaymax=scanpath->DELAY-scanpath->CLOCKPATHDELAY;
          npipp->movemax=0;
        }
    }
  ttv_freepathlist(path) ;
  
  path = ttv_getpathnocross_v2(stbfig->FIG,NULL,tve->ROOT,NULL, TTV_DELAY_MAX,TTV_DELAY_MIN,TTV_FIND_MIN|find,0) ;

  for(scanpath = path ; scanpath != NULL ; scanpath = scanpath->NEXT)
    {
      if((scanpath->TYPE & TTV_FIND_HZ) == TTV_FIND_HZ) continue ;
      if (scanpath->LATCH==NULL && getptype(scanpath->NODE->ROOT->USER, STB_IS_CLOCK)!=NULL
          && (scanpath->TTV_MORE_SEARCH_OPTIONS & TTV_MORE_OPTIONS_FLAG_THRU_FILTER_FOUND)==0)
        continue;

      if ((scanpath->ROOT->ROOT->TYPE & TTV_SIG_R) != 0)
        {
          ptype=getptype(scanpath->USER, TTV_PATH_PREVIOUS_NODE);
          scanpath->CMD=(ttvevent_list *)ptype->DATA;
        }

      start=scanpath->LATCH?scanpath->LATCH:scanpath->NODE;
      if ((l=gethtitem(startht, start))!=EMPTYHT)
        {
          for (cl=(chain_list *)l; cl!=NULL && (npi=(node_path_info *)cl->DATA)->cmd!=scanpath->CMD; cl=cl->NEXT) ;
//          if (cl==NULL) exit(60);
        }
      else cl=NULL;
      
      if (cl==NULL)
        {
          npi=create_node_path_info();
          npi->next=pathl;
          pathl=npi;          
          if (l!=EMPTYHT) cl=(chain_list *)l;
          addhtitem(startht, start, (long)addchain(cl, npi));
          if ((pt=getptype(scanpath->USER, TTV_PATH_OUTPUT_PHASE))!=NULL)
            {
              if ((ophase=(char)(long)pt->DATA)==TTV_NO_PHASE) ophase=STB_NO_INDEX;
              npi->output_phase=ophase;
            }
          else npi->output_phase=STB_NO_INDEX;
          npi->start=start;
          npi->cmd=scanpath->CMD;
        }

      pt=getptype(scanpath->USER,TTV_PATH_PHASE_INFO);

      if (pt!=NULL)
        {
          for (tps=(ttvpath_stb_stuff *)pt->DATA; tps!=NULL; tps=tps->NEXT)
            {
              for (npipp=npi->PP; npipp!=NULL && npipp->phase!=tps->STARTPHASE; npipp=npipp->next) ;
              if (npipp==NULL)
                {
                  npipp=create_node_path_info_PP();
                  npipp->next=npi->PP;
                  npi->PP=npipp;
                  npipp->phase=tps->STARTPHASE;
                }
              npipp->startmin=tps->STARTTIME;
              npipp->delaymin=scanpath->DELAY-scanpath->CLOCKPATHDELAY;
              npipp->movemin=tps->PERIOD_CHANGE;
            }
          ttv_freepathstblist((ttvpath_stb_stuff *)pt->DATA);
        }
      else
        {
          for (npipp=npi->PP; npipp!=NULL && npipp->phase!=STB_NO_INDEX; npipp=npipp->next) ;
          if (npipp==NULL)
            {
              npipp=create_node_path_info_PP();
              npipp->next=npi->PP;
              npi->PP=npipp;
              npipp->phase=STB_NO_INDEX;
            }
          npipp->startmin=TTV_NOTIME;
          npipp->delaymin=scanpath->DELAY-scanpath->CLOCKPATHDELAY;
          npipp->movemin=0;
        }

    }
  ttv_freepathlist(path) ;

  ttv_search_mode(0, TTV_MORE_OPTIONS_KEEP_PHASE_INFO);

  cl=GetAllHTElems(startht);
  while (cl!=NULL)
    {
      freechain((chain_list *)cl->DATA);
      cl=delchain(cl,cl);
    }
  delht(startht);

  ttv_SetPrecisionLevel(oldprecisionlevel);
  tve->USER=addptype(tve->USER, STB_PATH_LIST, pathl);
  ttv_set_ttv_getpath_clock_list(NULL);
  ttv_activate_path_and_access_mode(0);

  return pathl;
}

static int stb_inrelax=0;

void stb_set_relax(int val)
{
  stb_inrelax=val;
}

#ifdef RELAX_CORRECT_DEBUG
#define DISPDIR(x) ((x->TYPE & TTV_NODE_UP)!=0?'u':'d')
#endif

void stb_compute_falsepath_and_falseslack_effect(stbfig_list *sf, stbnode *node, ttvline_list *line, ttvevent_list *linecmd, stbck *nodeck, int flags)
{
  node_path_info *npi;
  valid_range_info *vri;
  stbck *ck, *ick;
  stbnode *startnode;
  int i, res, startindex;
//  long validmin, validmax;
  stbpair_list *STBTAB[256], *STBHZ[256], *ptstbpair, *pair;
  int ckindex;
  long periodeSetup, periodeHold, output_periode, input_periode;
  int nb0, nb1, first=0;
  char ckedge;
  node_path_info_per_phase *npipp, *backupnpipp;
#ifdef RELAX_CORRECT_DEBUG                  
  int deb=0;
  if (strcmp(node->EVENT->ROOT->NAME,"F2171467")==0 && (node->EVENT->TYPE & TTV_NODE_UP)==0) deb=1;
#endif

  if (node->EVENT->ROOT->TYPE & TTV_SIG_R)
    linecmd=line->NODE;

  if (!stb_inrelax && get_valid_range_info(line)!=NULL) return;

  vri=give_valid_range_info(line, node->NBINDEX);
  
  npi=stb_assign_paths(sf, node->EVENT);
  while (npi!=NULL)
    {
      if (npi->cmd==linecmd)
        {
          startnode=stb_getstbnode(npi->start);
          ck=startnode->CK;
          do
            {
              first++;

#ifdef RELAX_CORRECT_DEBUG
              if (strcmp(npi->start->ROOT->NAME,"F1867251")==0)
                  printf("");
#endif
              for (i=0; i<node->NBINDEX; i++) STBHZ[i]=NULL, STBTAB[i]=NULL;
              
              if (getptype(npi->start->ROOT->USER, STB_IS_CLOCK)!=NULL)
                create_clock_stability(startnode, STBTAB);
              else if (!stb_inrelax/*(startnode->FLAG & STB_NODE_DONE)==0*/)
               {
                 for (i=0; i<node->NBINDEX; i++) STBTAB[i]=stb_dupstbpairlist(startnode->STBTAB[i]);
               }
              else
                stb_transferstbline (sf, npi->start, startnode, STBTAB, STBHZ, 0, 1, NULL, ck, 1, 0) ;

              if (ck==NULL)
                for (ckindex=0; ckindex<node->NBINDEX && STBTAB[ckindex]==NULL; ckindex++) ;
              else
                ckindex=(int)ck->CKINDEX;

              if (ckindex<node->NBINDEX)
                {                
                  res=0;
                  startindex=ckindex;
                  if (STBTAB[startindex]!=NULL)
                    {
                      for (npipp=npi->PP, backupnpipp=NULL; npipp!=NULL && npipp->phase!=startindex; npipp=npipp->next)
                        if (npipp->phase==STB_NO_INDEX) backupnpipp=npipp;

                      if (npipp==NULL) npipp=backupnpipp;

                      if (npipp!=NULL)
                        {
                          if (stb_hasfalseslack(sf, node->EVENT))
                            {
                              ttvevent_list *startclock, *endclocksetup, *endclockhold;
                              int fs=0, fh=0;
                             
                               stb_getslacktopclocks(NULL, node, 0, ck, nodeck, &startclock, &endclocksetup, &endclockhold);
                               if (startclock!=NULL && endclocksetup!=NULL)
                                 fs=stb_isfalseslack(sf, startclock, npi->start, node->EVENT, endclocksetup, INF_FALSESLACK_SETUP);
                               
                               if (startclock!=NULL && endclockhold!=NULL)
                                 fh=stb_isfalseslack(sf, startclock, npi->start, node->EVENT, endclockhold, INF_FALSESLACK_HOLD);
                               if ((fs & INF_FALSESLACK_NOTHZ)!=0 && (fh & INF_FALSESLACK_NOTHZ)!=0) res=1;
                              /*                               if (res!=0)
                                 printf("found %s -> %s\n", npi->start->ROOT->NETNAME, node->EVENT->ROOT->NETNAME);*/
#ifdef RELAX_CORRECT_DEBUG                  
                          if (deb && stb_hasfalseslack(sf, node->EVENT))
                            printf("  [FS %d] %s(%c) %s(%c) %s(%c) %s(%c)", res, startclock?startclock->ROOT->NAME:"?", DISPDIR(startclock),
                                            npi->start->ROOT->NAME, DISPDIR(npi->start),
                                            node->EVENT->ROOT->NAME, DISPDIR(node->EVENT),
                                            endclocksetup?endclocksetup->ROOT->NAME:"?", endclocksetup?DISPDIR(endclocksetup):'x');
#endif
                            }
                          if (npipp->delaymin==TTV_NOTIME) npipp->delaymin=npipp->delaymax;
                          if (npipp->delaymax==TTV_NOTIME) npipp->delaymax=npipp->delaymin;
                          if (npipp->delaymin>npipp->delaymax) npipp->delaymin=npipp->delaymax;
                          
                          if (npi->output_phase!=STB_NO_INDEX) // filtre
                            ckindex=npi->output_phase;
                          

                          if (res==0)
                            {
                              input_periode=0;
                              ick=stb_getclock(sf, startindex, NULL, &ckedge, NULL);
                              if (ick!=NULL) input_periode=ick->PERIOD;
     
                              output_periode=input_periode;
                              if (nodeck!=NULL)
                                {
                                  ick=stb_getclock(sf, nodeck->CKINDEX, NULL, &ckedge, NULL);
                                  if (ick!=NULL) output_periode=ick->PERIOD;
                                }

                              if ((flags & STB_TRANSFERT_NOFILTERING)==0)
                                stb_getmulticycleperiod(npi->start, node->EVENT, input_periode, output_periode, &periodeSetup, &periodeHold, &nb0, &nb1);
                              else
                                periodeSetup=periodeHold=0;

                              //                        if (periodeSetup!=0)
                              {
                                long dmin, dmax;
                                dmax=npipp->delaymax-periodeSetup-npipp->movemax;
                                dmin=npipp->delaymin-periodeSetup-npipp->movemin;
                                if (dmax<dmin)
                                  {
                                    long temp;
                                    temp=dmax;
                                    dmax=dmin;
                                    dmin=temp;
                                  }
                                pair = stb_dupstbpairlist (STBTAB[startindex]);
                                pair = stb_clippair( pair, npipp->startmin, npipp->startmax);
//                                stb_assign_phase_to_stbpair(pair, startindex);
#ifdef RELAX_CORRECT_DEBUG 
                                if (deb && pair)
                                {
                                  printf(" this[%d %d]", pair->D+dmin, pair->U+dmax);
                                }
#endif
                                if (first==1)
                                {
                                  stb_freestbpair(vri[ckindex].realpair);
                                  vri[ckindex].realpair=NULL;
                                }
                                vri[ckindex].realpair = stb_transferstbpair (pair, vri[ckindex].realpair, dmax, dmin, 0, 0);
                                if ((sf->ANALYSIS == STB_GLOB_ANALYSIS) && vri[ckindex].realpair!=NULL && vri[ckindex].realpair->NEXT!=NULL)
                                  {
                                    stbpair_list *sbp;
                                    vri[ckindex].realpair = stb_globalstbpair(sbp=vri[ckindex].realpair);
                                    stb_freestbpair(sbp);
                                  }

#ifdef RELAX_CORRECT_DEBUG 
                                if (deb)
                                {
                                ptstbpair = stb_globalstbpair(STBTAB[startindex]);
                                printf(" MC:  startstab from %s (%c): [%ld %ld] (mcs=%ld) move:(%ld/%ld) totstab: \n", npi->start->ROOT->NAME, (npi->start->TYPE & TTV_NODE_UP)!=0?'u':'d', ptstbpair->D, ptstbpair->U, periodeSetup, npipp->movemin, npipp->movemax);
                          
                                for (pair=vri[ckindex].realpair; pair!=NULL; pair=pair->NEXT)
                                  printf("[%ld %ld]", pair->D, pair->U);
                                printf("\n");
                                stb_freestbpair(ptstbpair);
                                }
#endif
                              }
                            }
#ifdef RELAX_CORRECT_DEBUG
                           else if (deb)
                             printf("\n");
#endif
                                      
                        }
                    }
                }
              stb_freestbtabpair(STBTAB, node->NBINDEX);
              stb_freestbtabpair(STBHZ, node->NBINDEX);
              if (ck==NULL) break;
              ck=ck->NEXT;
            } while (ck!=NULL);

        }
      npi=npi->next;
    }
}

int path_false_slack_check(ttvpath_list *tp, ttvevent_list *opencmd, ttvevent_list *topopenclock)
{
  ttvevent_list *startclock, *endclocksetup, *endclockhold;
  int fs=0, fh=0;
  stbnode *node;
  stbck *nodeck;
  stbfig_list *sf;

  
  if (tp->LATCH==NULL) return 0;

  node=stb_getstbnode(tp->ROOT);
  if (node==NULL) return 0;
  
  for (nodeck=node->CK; nodeck!=NULL && nodeck->CMD!=opencmd; nodeck=nodeck->NEXT) ;

  if (nodeck==NULL) return 0;

  sf=stb_getstbfig(tp->NODE->ROOT->ROOT);
  stb_getslacktopclocks(NULL, node, 0, NULL, nodeck, &startclock, &endclocksetup, &endclockhold);
  if (endclocksetup!=NULL && (topopenclock==NULL || endclocksetup->ROOT==topopenclock->ROOT))
    fs=stb_isfalseslack(sf, tp->NODE, tp->LATCH, node->EVENT, endclocksetup, INF_FALSESLACK_SETUP);
  
  if (endclockhold!=NULL && (topopenclock==NULL || endclockhold->ROOT==topopenclock->ROOT))
    fh=stb_isfalseslack(sf, tp->NODE, tp->LATCH, node->EVENT, endclockhold, INF_FALSESLACK_HOLD);
  if ((fs & INF_FALSESLACK_NOTHZ)!=0 && (fh & INF_FALSESLACK_NOTHZ)!=0) return 1;
  return 0;
}

#include MUT_H
#include MLO_H
#include MLU_H
#include RCN_H
#include MSL_H
#include AVT_H
#include INF_H
#include ELP_H
#include CNS_H
#include YAG_H
#include TLC_H
#include TRC_H
#include STM_H
#include TTV_H
#include MCC_H
#include SIM_H
#include TAS_H
#include FCL_H
#include BEH_H
#include CBH_H
#include LIB_H
#include TLF_H
#include STB_H

#define  API_USE_REAL_TYPES
#include "../ttv/ttv_API_LOCAL.h"
#include "ttv_API.h"
#include "../ttv/ttv_API_display.h"
#include "../ttv/ttv_API_util.h"
#include "stb_LOCAL.h"
#include "stb_API.h"
#include "../../tas/stb/stb_transfer.h"
#include "../../tas/stb/stb_util.h"
#define SETUP_SLOPE_INFO   0xfab70124
#define SETUP_SLOPE_SOURCE 0xfab70125
typedef struct
{  
  long setup_slope;
  long setup_slope_hz;
  long period;
  stbck *ck;
} setup_slope_info_phase;

typedef struct
{
  long worst_slack;
  setup_slope_info_phase *ssip;
} setup_slope_info;

static setup_slope_info *stb_get_setup_slope_info(ttvevent_list *tve)
{
  ptype_list *pt;
  if ((pt=getptype(tve->USER, SETUP_SLOPE_INFO))!=NULL) return (setup_slope_info *)pt->DATA;
  return NULL;
}

static setup_slope_info *stb_give_setup_slope_info(ttvevent_list *tve, int nbphase)
{
  setup_slope_info *ssi;
  int i;
  if ((ssi=stb_get_setup_slope_info(tve))!=NULL) return ssi;
  ssi=(setup_slope_info *)mbkalloc(sizeof(setup_slope_info));
  ssi->ssip=(setup_slope_info_phase *)mbkalloc(sizeof(setup_slope_info_phase)*nbphase);

  for (i=0; i<nbphase; i++) 
    {
      ssi->ssip[i].setup_slope=ssi->ssip[i].setup_slope_hz=TTV_NOTIME;
      ssi->ssip[i].ck=NULL;
    }
  ssi->worst_slack=TTV_NOTIME;
  tve->USER=addptype(tve->USER, SETUP_SLOPE_INFO, ssi);
  return ssi;
}

static void stb_delete_setup_slope_info(ttvevent_list *tve)
{
  setup_slope_info *ssi;
  if ((ssi=stb_get_setup_slope_info(tve))!=NULL)
    {
      mbkfree(ssi->ssip);
      mbkfree(ssi);
      tve->USER=delptype(tve->USER, SETUP_SLOPE_INFO);
    }
}


void stb_propagate_setup_slope (stbfig_list *stbfig, ttvevent_list *event)
{
  stbnode *node;
  ttvevent_list *inevent, *cmdmax;
  ttvline_list *line;
  stbck *ck;
  long delaymax;
  long delaymin;
  long slope, period;
  char phase;
  char level;
  int hz, newstartslope, i, slopesource, sourcechanged;
  setup_slope_info *ssi, *outssi;
  long *pslope;

  if((node = stb_getstbnode (event)) == NULL)
    return ;

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

  if (stb_get_setup_slope_info(event)!=NULL) sourcechanged=1; else sourcechanged=0;
  outssi=stb_give_setup_slope_info(event, node->NBINDEX);

  for (; line; line = line->NEXT)
    {
      if (((line->TYPE & TTV_LINE_CONT) == TTV_LINE_CONT) ||
          (((line->TYPE & (TTV_LINE_D | TTV_LINE_T)) != 0) &&
           (line->FIG != stbfig->FIG)) ||
          (((line->TYPE & (TTV_LINE_P | TTV_LINE_F)) != 0) &&
           (line->FIG != stbfig->FIG) &&
           ((stbfig->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST)))
        continue;

      if ((line->TYPE & TTV_LINE_HZ)!=0) hz=1; else hz=0;
      
      inevent = line->NODE;
      delaymin = ttv_getdelaymin (line);
      delaymax = ttv_getdelaymax (line);
      if (delaymin == TTV_NOTIME) delaymin = delaymax;
      if (delaymax == TTV_NOTIME) delaymax = delaymin;
      
      newstartslope=0;
      ssi=stb_give_setup_slope_info(inevent, node->NBINDEX);

      if (getptype(inevent->USER, SETUP_SLOPE_SOURCE)!=NULL) slopesource=1; else slopesource=0;

      for (ck=node->CK; node->CK==NULL || ck!=NULL ; ck=ck->NEXT)
        {
          if(ck != NULL && ck->CMD!=NULL)
            {
              cmdmax = ttv_getlinecmd(stbfig->FIG, line, TTV_LINE_CMDMAX) ;
              if(ck->CMD != cmdmax) continue ;

//              if (!stb_checkvalidcommand(event, ck)) continue;
            }

          slope=stb_getsetupslope(stbfig, node, ck, hz, &phase);
          if (slope!=TTV_NOTIME && phase!=STB_NO_INDEX)
            {
              newstartslope=1;
              if (sourcechanged)
                {
                  for (i=0; i<node->NBINDEX; i++) outssi->ssip[i].setup_slope=ssi->ssip[i].setup_slope_hz=TTV_NOTIME;
                  outssi->worst_slack=TTV_NOTIME;
                  sourcechanged=0;
                }
              if (getptype(event->USER, SETUP_SLOPE_SOURCE)==NULL)
                event->USER=addptype(event->USER, SETUP_SLOPE_SOURCE, NULL);

              if (!hz) pslope=&outssi->ssip[(int)phase].setup_slope; else pslope=&outssi->ssip[(int)phase].setup_slope_hz;
              if (*pslope==TTV_NOTIME || *pslope>slope)
                {
                  *pslope=slope;
                  outssi->ssip[(int)phase].period=ck!=NULL?ck->PERIOD:0;
                  outssi->ssip[(int)phase].ck=ck;
                }
              outssi->worst_slack=node->SETUP!=STB_NO_TIME?node->SETUP:TTV_NOTIME;

              slope-=delaymax;
              if (slopesource==0)
                {
                  if (ssi->ssip[(int)phase].setup_slope==TTV_NOTIME || ssi->ssip[(int)phase].setup_slope>slope)
                    {
                      ssi->ssip[(int)phase].setup_slope=slope;
                      ssi->ssip[(int)phase].period=ck!=NULL?ck->PERIOD:0;
                      ssi->ssip[(int)phase].ck=NULL;
                    }
                  if (ssi->worst_slack==TTV_NOTIME || ssi->worst_slack>outssi->worst_slack)
                    ssi->worst_slack=outssi->worst_slack;
                }              
            }
          if (ck==NULL) break;
        }

      if ((slope=stb_get_directive_setup_slope(stbfig, node, &phase, &period, 0))!=TTV_NOTIME)
        {
          newstartslope=1;
          if (sourcechanged)
            {
              for (i=0; i<node->NBINDEX; i++) outssi->ssip[i].setup_slope=ssi->ssip[i].setup_slope_hz=TTV_NOTIME;
              sourcechanged=0;
            }
          if (getptype(event->USER, SETUP_SLOPE_SOURCE)==NULL)
            event->USER=addptype(event->USER, SETUP_SLOPE_SOURCE, NULL);
          if (outssi->ssip[(int)phase].setup_slope==TTV_NOTIME || outssi->ssip[(int)phase].setup_slope>slope)
            {
              outssi->ssip[(int)phase].setup_slope=slope;
              outssi->ssip[(int)phase].period=period;
              outssi->ssip[(int)phase].ck=NULL;
            }

          slope-=delaymax;
          
          if (slopesource==0 && (ssi->ssip[(int)phase].setup_slope==TTV_NOTIME || ssi->ssip[(int)phase].setup_slope>slope))
            {
              ssi->ssip[(int)phase].setup_slope=slope;
              ssi->ssip[(int)phase].period=period;
              ssi->ssip[(int)phase].ck=NULL;
            }
        }

      if (!newstartslope && !slopesource)
        {
          for (i=0; i<node->NBINDEX; i++)
            {
              if (outssi->ssip[i].setup_slope!=TTV_NOTIME)
                {
                  slope=outssi->ssip[i].setup_slope-delaymax;
                  if (ssi->ssip[i].setup_slope==TTV_NOTIME || ssi->ssip[i].setup_slope>slope)
                    {
                      ssi->ssip[i].setup_slope=slope;
                      ssi->ssip[i].period=outssi->ssip[i].period;
                      ssi->ssip[i].ck=NULL;
                    }
                  if (ssi->worst_slack==TTV_NOTIME || ssi->worst_slack>outssi->worst_slack)
                    ssi->worst_slack=outssi->worst_slack;
                }
            }
        }
    }
}

static void stb_getstab(stbfig_list *stbfig, stbnode *node, stbpair_list **STBTAB, stbpair_list **STBHZ, stbck *ck)
{
  int i;

  for (i=0; i<node->NBINDEX; i++) STBHZ[i]=NULL, STBTAB[i]=NULL;

  stb_transferstbline (stbfig, node->EVENT, node , STBTAB, STBHZ, 0, 1, NULL, ck, 1, STB_TRANSFERT_NOFILTERING|STB_TRANSFERT_NOPHASECHANGE) ;

}


long stb_computelocalmargin(stbfig_list *stbfig, ttvevent_list *tve, FILE *f)
{
  stbnode *node;
  setup_slope_info *ssi;
  int i, j;
  long period, setup_margin=TTV_NOTIME, val;
  stbpair_list *STBTAB[256], *STBHZ[256], *pair, *pairhz;
  stbdomain_list *ptdomain;

  if((node = stb_getstbnode (tve)) == NULL) return TTV_NOTIME;

// qq heursistique reportee ici pour coller
  if ((tve->ROOT->TYPE & TTV_SIG_R)!=0)
    {
      if (V_INT_TAB[__STB__PRECHARGE_DATA_HEURISTIC].VALUE==2 && (tve->TYPE & TTV_NODE_UP)!=0)
        return TTV_NOTIME;
    }

  if ((ssi=stb_get_setup_slope_info(tve))!=NULL)
    {
      for (i=0; i<node->NBINDEX; i++)
        {
          for (j=0; j<node->NBINDEX; j++)
            {
              ptdomain = stb_getstbdomain(stbfig->CKDOMAIN, j) ;
              
              if (i>=ptdomain->CKMIN && i<=ptdomain->CKMAX)
                {
                  stb_getstab(stbfig, node, STBTAB, STBHZ, ssi->ssip[j].ck);
                  for (pair=STBTAB[i]; pair!=NULL && pair->NEXT!=NULL; pair=pair->NEXT) ;
                  for (pairhz=STBHZ[i]; pairhz!=NULL && pairhz->NEXT!=NULL; pairhz=pairhz->NEXT) ;
                  
                  if (stb_cmpphase(stbfig, i, j)>0/*i>j*/)
                    period=ssi->ssip[j].period;
                  else if (i==j)
                    {
                      if((stbfig->STABILITYMODE & STB_STABILITY_FF) != 0)
                        period=ssi->ssip[j].period;
                      else if((stbfig->STABILITYMODE & STB_STABILITY_LT) != 0)
                        period=0;
                      else
                        period=ssi->ssip[j].period;
                    }
                  else period=0;
                  
                  if (pair!=NULL && ssi->ssip[j].setup_slope!=TTV_NOTIME)
                    {
                      val=ssi->ssip[j].setup_slope+period-pair->U;
                      if (setup_margin==TTV_NOTIME || val<setup_margin) setup_margin=val;
                      if (f!=NULL) fprintf(f,"\t\t\t   (%d->%d %ld+%ld-%ld=%ld) worst:%ld\n", i, j, ssi->ssip[j].setup_slope, period, pair->U, val, ssi->worst_slack);
                    }
                  if (pairhz!=NULL && ssi->ssip[j].setup_slope_hz!=TTV_NOTIME)
                    {
                      val=ssi->ssip[j].setup_slope_hz+period-pairhz->U;
                      if (setup_margin==TTV_NOTIME || val<setup_margin) setup_margin=val;
                      if (f!=NULL) fprintf(f,"\t\t\t   (hz:%d->%d %ld+%ld-%ld=%ld) worst:%ld\n", i, j, ssi->ssip[j].setup_slope_hz, period, pairhz->U, val, ssi->worst_slack);
                    }
                  if (setup_margin!=TTV_NOTIME && setup_margin<ssi->worst_slack)
                    {
                      setup_margin=ssi->worst_slack;
                      if (f!=NULL) fprintf(f,"\t\t *CAPPED %ld*\n", setup_margin);
                    }

                  stb_freestbtabpair(STBHZ, node->NBINDEX);
                  stb_freestbtabpair(STBTAB, node->NBINDEX);
                }
            }
        }      
    }

  return setup_margin;
}



void stb_compute_local_margins(stbfig_list *stbfig)
{
  chain_list *cl;
  FILE *f;
  setup_slope_info *ssi;
  ttvevent_list *tve, *maxtve=NULL;
  stbnode *node;
  int i;
  long val, min=TTV_NOTIME, max=TTV_NOTIME, val1;
 
  if (stbfig==NULL) {printf("stb_compute_local_margins: NULL stbfig\n");return;}
  stbfig->NODE = reverse(stbfig->NODE);
  for (cl=stbfig->NODE; cl!=NULL; cl=cl->NEXT) stb_delete_setup_slope_info((ttvevent_list *)cl->DATA);
  for (cl=stbfig->NODE; cl!=NULL; cl=cl->NEXT) stb_propagate_setup_slope(stbfig, (ttvevent_list *)cl->DATA);

  for (cl=stbfig->NODE; cl!=NULL; cl=cl->NEXT)
    {
      tve=(ttvevent_list *)cl->DATA;
      tve->USER=testanddelptype(tve->USER, SETUP_SLOPE_SOURCE);
    }

  f=fopen("test.log","wt");
  for (cl=stbfig->NODE; cl!=NULL; cl=cl->NEXT)
    {
      tve=(ttvevent_list *)cl->DATA;
      node=stb_getstbnode(tve);
      ssi=stb_get_setup_slope_info(tve);
      if (ssi!=NULL)
        {
          if ((tve->ROOT->TYPE & TTV_SIG_L)!=0)
            fprintf(f, "L");
          else if ((tve->ROOT->TYPE & TTV_SIG_R)!=0)
            fprintf(f, "P");
          else
            fprintf(f, " ");
          fprintf(f, "%20s %20s (%c) ", tve->ROOT->NETNAME, tve->ROOT->NAME, (tve->TYPE & TTV_NODE_UP)?'R':'F');
          if (ssi->worst_slack!=TTV_NOTIME) fprintf(f, " worst=%ld", ssi->worst_slack);
            
          for (i=0; i<node->NBINDEX; i++)
            {
              if (ssi->ssip[i].setup_slope!=TTV_NOTIME)
                fprintf(f," (%d: %ld %ld)", i, ssi->ssip[i].setup_slope, ssi->ssip[i].period);
            }
          fprintf(f, "\n");
          val=stb_computelocalmargin(stbfig, tve, f);
          if (node->SETUP!=STB_NO_TIME && val!=node->SETUP)
            fprintf(f, " *** FAILED orig: %ld != %ld ***\n", node->SETUP,val);
          if ((tve->TYPE & TTV_NODE_UP)!=0 && ttv_isgateoutput(stbfig->FIG, tve->ROOT, TTV_FILE_DTX))
            {
              val1=stb_computelocalmargin(stbfig, tve->ROOT->NODE, NULL);
              if (val1!=TTV_NOTIME && val1<val) val=val1;          
              if (val!=TTV_NOTIME)
                {
                  if (min==TTV_NOTIME || val<min) min=val;
                  if (max==TTV_NOTIME || val>max) 
                    { 
                      max=val;
                      if ((tve->ROOT->TYPE & TTV_SIG_L)==0)
                        {
                          if (ttv_testsigflag(tve->ROOT, TTV_SIG_FLAGS_ISSLAVE)==0
                              && ttv_testsigflag(tve->ROOT, TTV_SIG_FLAGS_ISMASTER)==0)
                            maxtve=tve; 
                        }
                    }
                }
            }
        }
    }

  fclose(f);

#define MAX 10

#define RANGE_MIN 1
#define RANGE_MAX 2
#define SIG_NUMBER 3
#define SIG_POURC 4
#define NB_MASTER 5
#define NB_SLAVE 6
#define NB_CTRL_POINT 7

  f=fopen("histo.log","wt");

  {
    struct 
    {
      int nbs, nbm, nbctrl;
      int nb;
    } nb[MAX];
    int idx, j;
    int tot=0;
    Board *b;
    char buf[1024];
    float step;

    fprintf(f, "* min=%.1f   max=%.1f (%s)\n", min/TTV_UNIT, max/TTV_UNIT, maxtve?maxtve->ROOT->NAME:"?");

    b=Board_CreateBoard();
    Board_SetSize(b, RANGE_MIN, 7, DISPLAY_number_justify);
    Board_SetSize(b, RANGE_MAX, 7, DISPLAY_number_justify);
    Board_SetSize(b, SIG_NUMBER, 7, DISPLAY_number_justify);
    Board_SetSize(b, SIG_POURC, 7, DISPLAY_number_justify);
    Board_SetSize(b, NB_MASTER, 7, DISPLAY_number_justify);
    Board_SetSize(b, NB_SLAVE, 7, DISPLAY_number_justify);
    Board_SetSize(b, NB_CTRL_POINT, 7, DISPLAY_number_justify);
    Board_NewLine(b);

    Board_SetValue(b, RANGE_MIN, TITLE_START "Range");
    Board_SetValue(b, RANGE_MAX, TITLE_END "");

    Board_NewLine(b);

    Board_SetValue(b, RANGE_MIN, "Min");
    Board_SetValue(b, RANGE_MAX, "Max");
    Board_SetValue(b, SIG_NUMBER, "# Gates");
    Board_SetValue(b, SIG_POURC, "% Gates");
    Board_SetValue(b, NB_MASTER, "# Masters");
    Board_SetValue(b, NB_SLAVE, "# Slaves");
    Board_SetValue(b, NB_CTRL_POINT, "# CTRLPoints");

    Board_NewSeparation(b);

/*    if (abs(min)>abs(max)) min=-abs(min), max=abs(min);
    else min=-abs(max), max=abs(max);*/
    step=(MAX)/(float)(max-min);
    for (i=0; i<MAX; i++) nb[i].nb=0, nb[i].nbs=0, nb[i].nbm=0, nb[i].nbctrl=0;
    for (cl=stbfig->NODE; cl!=NULL; cl=cl->NEXT)
      {
        tve=(ttvevent_list *)cl->DATA;
        if ((tve->TYPE & TTV_NODE_UP)!=0 && ttv_isgateoutput(stbfig->FIG, tve->ROOT, TTV_FILE_DTX))
          {
            val=stb_computelocalmargin(stbfig, tve, NULL);
            val1=stb_computelocalmargin(stbfig, tve->ROOT->NODE, NULL);
            if (val1!=TTV_NOTIME && val1<val) val=val1;
            if (val!=TTV_NOTIME)
              {
                idx=(int)(step*(float)(val-min));
//                if (val<0) idx--;
                if (idx>=MAX) idx=MAX-1;
                if (idx<0) idx=0;
                nb[idx].nb++;
                tot++;
                if (stb_getstbnode(tve)->SETUP!=STB_NO_TIME || stb_getstbnode(tve->ROOT->NODE)->SETUP!=STB_NO_TIME) nb[idx].nbctrl++;
                  
                if (ttv_testsigflag(tve->ROOT, TTV_SIG_FLAGS_ISSLAVE)!=0) nb[idx].nbs++;
                if (ttv_testsigflag(tve->ROOT, TTV_SIG_FLAGS_ISMASTER)!=0) nb[idx].nbm++;
              }
          }
      }
    for (i=0; i<MAX && nb[i].nb==0; i++) ;
    for (j=MAX-1; j>=0 && nb[i].nb==0; j--) ;
    while (i<=j)
      {
        Board_NewLine(b);
        
        Board_SetValue(b, RANGE_MIN, FormaT(_LONG_TO_DOUBLE(min+i/step)*DISPLAY_time_unit, buf, DISPLAY_time_format));
        Board_SetValue(b, RANGE_MAX, FormaT(_LONG_TO_DOUBLE(min+(i+1)/step)*DISPLAY_time_unit, buf, DISPLAY_time_format));
        sprintf(buf, "%d", nb[i].nb);
        Board_SetValue(b, SIG_NUMBER, buf);
        sprintf(buf, "%.1f%%", nb[i].nb*100.0/tot);
        Board_SetValue(b, SIG_POURC, buf);
        sprintf(buf, "%d", nb[i].nbm);
        Board_SetValue(b, NB_MASTER, buf);
        sprintf(buf, "%d", nb[i].nbs);
        Board_SetValue(b, NB_SLAVE, buf);
        sprintf(buf, "%d", nb[i].nbctrl);
        Board_SetValue(b, NB_CTRL_POINT, buf);

//        fprintf(f, "%.1f - %.1f : %d (%.1f%%)    / s=%d m=%d\n", (min+i*(max-min)/MAX)/TTV_UNIT, (min+(i+1)*(max-min)/MAX)/TTV_UNIT, nb[i].nb, nb[i].nb*100.0/tot, nb[i].nbs, nb[i].nbm);
        i++;
      }
    Board_Display(f, b, "");
    Board_FreeBoard(b);
  }
  fclose(f);

  
  stbfig->NODE = reverse(stbfig->NODE);
}


#define TEMPMARK_SOURCE 0xfab70411

static void stb_find_stb_path (stbfig_list *stbfig, ttvevent_list *event, ttvevent_list *prev, long time )
{
  stbnode *node;
  ttvevent_list *inevent;
  ttvline_list *line;
  stbpair_list *sp;
  long delaymax;
  long delaymin;
  char phase=STB_NO_INDEX;
  char level, f;
  int i;

  if((node = stb_getstbnode (event)) == NULL)
    return ;

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

  for (i=0; i<node->NBINDEX; i++)
    {
      sp=stb_globalstbpair (node->STBTAB[i]);
      if (sp!=NULL)
        {
          if (time==sp->U) { phase=i; break; }
        }
      stb_freestbpair(sp);
    }

  if (phase==STB_NO_INDEX) return;

  if (event->ROOT->TYPE & TTV_SIG_L) f='L';
  else if (event->ROOT->TYPE & TTV_SIG_R) f='P';
  else f='-';
  fprintf(stdout,"found '%s %s(%c)' (%c) at %ld phase %d\n", event->ROOT->NAME, event->ROOT->NETNAME, event->TYPE & TTV_NODE_UP?'R':'F', f, time, phase);

  if (prev!=NULL && (event->ROOT->TYPE & (TTV_SIG_L|TTV_SIG_R))!=0) return;
  if (getptype(event->USER, TEMPMARK_SOURCE)!=NULL) return;
  event->USER=addptype(event->USER, TEMPMARK_SOURCE, NULL);
  for (; line; line = line->NEXT)
    {
      if (((line->TYPE & TTV_LINE_CONT) == TTV_LINE_CONT) ||
          (((line->TYPE & (TTV_LINE_D | TTV_LINE_T)) != 0) &&
           (line->FIG != stbfig->FIG)) ||
          (((line->TYPE & (TTV_LINE_P | TTV_LINE_F)) != 0) &&
           (line->FIG != stbfig->FIG) &&
           ((stbfig->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST)))
        continue;

      if ((line->TYPE & TTV_LINE_HZ)!=0) continue; //hz=1; else hz=0;
      
      inevent = line->NODE;
      delaymin = ttv_getdelaymin (line);
      delaymax = ttv_getdelaymax (line);
      if (delaymin == TTV_NOTIME) delaymin = delaymax;
      if (delaymax == TTV_NOTIME) delaymax = delaymin;

      
      stb_find_stb_path (stbfig, inevent, event, time-delaymax );
    }
  event->USER=delptype(event->USER, TEMPMARK_SOURCE);
}

void stb_trackstbpath (stbfig_list *stbfig, char *nodename, char dir, double timeo)
{
  ttvsig_list *tvs;
  int num;
  long time;
  chain_list *cl, *ch;


  cl=addchain(NULL, namealloc(nodename));

  ch = ttv_getsigbytype_and_netname(stbfig->FIG,NULL,TTV_SIG_TYPEALL,cl) ;
  freechain(cl);

  time=mbk_long_round(timeo*1e12*TTV_UNIT);
  if (tolower(dir)=='r' || tolower(dir)=='u') num=1;
  else if (tolower(dir)=='f' || tolower(dir)=='d') num=0;
  else {printf("direction %c invalide\n",dir);return;}

  while (ch!=NULL)
    {
      tvs=(ttvsig_list *)ch->DATA;
      stb_find_stb_path (stbfig, &tvs->NODE[num], NULL, time );
      fprintf(stdout,"------------\n");
      ch=delchain(ch, ch);
    }
}

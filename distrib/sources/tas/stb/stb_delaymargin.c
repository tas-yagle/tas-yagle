
#include MUT_H
#include STB_H
#include RCN_H
#include TRC_H
#include TTV_H
#include "stb_util.h"

typedef struct
{
  ttvpath_list *datamin, *datamax;
  ttvevent_list *clockeventmax, *clockeventmin;
  ttvpath_list *clockpathsmax, *clockpathsmin;
  int clean;
} searchinfo;

static char *getsygtype(ttvsig_list *tvs)
{
  if ((tvs->TYPE & TTV_SIG_L)==TTV_SIG_L) return "Latch";
  if ((tvs->TYPE & TTV_SIG_B)==TTV_SIG_B) return "Breakpoint";
  if ((tvs->TYPE & TTV_SIG_C)==TTV_SIG_C) return "Connector";
  if ((tvs->TYPE & TTV_SIG_R)==TTV_SIG_R) return "Precharge";
  if ((tvs->TYPE & TTV_SIG_Q)==TTV_SIG_Q) return "Command";
  return "?";
}

stbck *stb_getgoodclock_and_status(stbfig_list *sb, stbck *clock, ttvevent_list *tve, ttvevent_list *latch, int *inverted)
{
  stbck *origclock=clock;
  ttvevent_list *clockevent=NULL;

  *inverted=0;
  if (clock==NULL) return NULL;
  
  while (clock!=NULL)
  {
    if (clock->CMD==tve) break;
    clock=clock->NEXT;
  }
  
  if (clock==NULL) clock=origclock;
  stb_getclocknode(sb, clock->CKINDEX, NULL, &clockevent, clock) ;
  if((STB_OPEN_LATCH_PHASE == 'N') && clockevent &&
     (((latch->ROOT->TYPE & TTV_SIG_LL) == TTV_SIG_LL) || 
      ((latch->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R)))
    {
     if((clockevent->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
        clockevent = clockevent->ROOT->NODE ;
     else
        clockevent = clockevent->ROOT->NODE + 1 ;
    }

  if(clockevent!=NULL && (clockevent->TYPE & TTV_NODE_UP) != (tve->TYPE & TTV_NODE_UP)) *inverted=1;
  return clock;
}

stbck *stb_gooddir(stbck *clock)
{
  stbck *origclock=clock;
  long active = clock->ACTIVE;

  if ((clock->TYPE == STB_TYPE_COMMAND)
    && ((clock->VERIF & STB_NO_VERIF) != STB_NO_VERIF))
  {
    if ((clock->ACTIVE & STB_STATE_UP) == STB_STATE_UP)
      {
        if ((((clock->VERIF & STB_DN) == STB_DN) && ((clock->VERIF & STB_VERIF_STATE) == STB_VERIF_STATE))
            || (((clock->VERIF & STB_UP) == STB_UP) && ((clock->VERIF & STB_VERIF_EDGE) == STB_VERIF_EDGE)))
          active = STB_STATE_DN;
      }
    else if ((clock->ACTIVE & STB_STATE_DN) == STB_STATE_DN)
      {
        if ((((clock->VERIF & STB_UP) == STB_UP) && ((clock->VERIF & STB_VERIF_STATE) == STB_VERIF_STATE))
            || (((clock->VERIF & STB_DN) == STB_DN) && ((clock->VERIF & STB_VERIF_EDGE) == STB_VERIF_EDGE)))
          active = STB_STATE_UP;
      }
  }

  do {
    if (clock->CMD!=NULL 
        && ((((active & STB_SLOPE_UP)==STB_SLOPE_UP || (active & STB_STATE_UP) == STB_STATE_UP) && clock->CMD==&clock->CMD->ROOT->NODE[1])
            || (((active & STB_SLOPE_DN)==STB_SLOPE_DN || (active & STB_STATE_DN) == STB_STATE_DN) && clock->CMD==&clock->CMD->ROOT->NODE[0])))
        return clock;
    clock=clock->NEXT;
  } while (clock!=NULL);
 return origclock;        
}

static long dirof(ttvevent_list *tve, int rev)
{
  long dir;
  if ((tve->TYPE & TTV_NODE_UP)!=0) dir=__FINDUP__;
  else dir=__FINDDOWN__;
  if (rev)
   {
     if (dir==__FINDDOWN__) dir=__FINDUP__;
     else dir=__FINDDOWN__;
   }
  return dir;
}
void stb_getclockandtestslopes(stbfig_list *sf, ttvevent_list *tve, stbck *clock, ttvsig_list **tvs_setup, long *dir_setup, ttvsig_list **tvs_hold, long *dir_hold)
{
  long active;
  char edge;
  long checkdir=-1;
  stbck *old=clock;
  
  while (clock!=NULL && !stb_cmd_can_generate_event(sf, tve, clock->CMD)) clock=clock->NEXT;
  if (clock==NULL) clock=old;

  active = clock->ACTIVE;

  if ((tve->ROOT->TYPE & (TTV_SIG_Q|TTV_SIG_R))!=0)
    *tvs_setup=*tvs_hold=tve->ROOT;
  else if (clock->CMD!=NULL)
    {
      *tvs_setup=clock->CMD->ROOT;
      *tvs_hold=clock->CMD->ROOT;
    }
  else
    *tvs_setup=*tvs_hold=NULL;

  if (clock->CMD!=NULL)
  {
   if ((active & STB_SLOPE) == STB_SLOPE)
     {
       *dir_setup=dirof(clock->CMD, 1);
       *dir_hold=dirof(clock->CMD, 1);
     }
   else if ((active & STB_STATE) == STB_STATE)
     {
       if ((clock->VERIF & STB_VERIF_EDGE) == STB_VERIF_EDGE)
         *dir_setup=dirof(clock->CMD, 1);
       else
         *dir_setup=dirof(clock->CMD, 0);    
       *dir_hold=dirof(clock->CMD, 1);
     }
  }
}

static int isclockpath(ttvpath_list *pth, chain_list *clocks)
{
  chain_list *cl;
  for (cl=clocks; cl!=NULL; cl=cl->NEXT)
    if (pth->NODE->ROOT==cl->DATA) return 1;
  return 0;        
}

static void stb_delaymargins_setval(ttvevent_list *tve, long data, long clock, int setup)
{
  ptype_list *pt;
  stb_delaymargins *sdm;

  if ((pt=getptype(tve->USER, STB_DELAYMARGINS))==NULL)
  {
     sdm=(stb_delaymargins *)mbkalloc(sizeof(stb_delaymargins));
     sdm->setup_data=sdm->setup_clock=sdm->hold_data=sdm->hold_clock=STB_NO_TIME;
     tve->USER=addptype(tve->USER, STB_DELAYMARGINS, sdm);
  } else sdm=(stb_delaymargins *)pt->DATA;

  if (setup)
     sdm->setup_data=data, sdm->setup_clock=clock;
  else
     sdm->hold_data=data, sdm->hold_clock=clock;
}
void stb_delaymargins_getval(ttvevent_list *tve, long *data, long *clock, int setup)
{
  ptype_list *pt;
  stb_delaymargins *sdm;

  *data=*clock=STB_NO_TIME;
  if ((pt=getptype(tve->USER, STB_DELAYMARGINS))==NULL) *data=*clock=STB_NO_TIME;
  else
  {
   sdm=(stb_delaymargins *)pt->DATA;
   if (setup)
      *data=sdm->setup_data, *clock=sdm->setup_clock;
   else
      *data=sdm->hold_data, *clock=sdm->hold_clock;
  }
}
void stb_delaymargins_freeval(ttvevent_list *tve)
{
  ptype_list *pt;

  if ((pt=getptype(tve->USER, STB_DELAYMARGINS))!=NULL)
  {
    mbkfree(pt->DATA);
    tve->USER=delptype(tve->USER, STB_DELAYMARGINS);
  }
}

static int stb_find_latch_margin_fastmode(FILE *f, ttvfig_list *tvf, ttvsig_list *tvs, int trans, inffig_list *ifl, char *infregex, chain_list *clocklist, NameAllocator *na, long findmode, searchinfo *si)
{
  ttvpath_list *datapath_max, *clockpath_max, *pth, *pthck;
  ttvpath_list *datapath_min, *clockpath_min;
  float factor_data, factor_clock;
  long delta_data, delta_clock;
  float newdatapathdelay, oldDELAY, oldSTART, oldNEWDELAY;
  long biggestU=TTV_NOTIME, lowestD=TTV_NOTIME;
  long margeclock, margedata;
  float margeclockf, margedataf, overlapc, overlapd;
  long localoverlapc, localoverlapd;
  stbck *clock, *ckl;
  ttvsig_list *clocksig_setup, *clocksig_hold;
  ttvevent_list *tve;
  long finddir_setup, finddir_hold/*, finddir_data*/;
  float CPD, DPD;
  stbnode     *node;
  char section[1024];
  int ret=0;
  ht *cht;
  stbfig_list *sf;

  sf=stb_getstbfig(tvf);

  node=stb_getstbnode(&tvs->NODE[trans]);
  clock=node->CK;
  if (node->SETUP==STB_NO_TIME && node->HOLD==STB_NO_TIME) return ret;
  if (clock == NULL || (clock->VERIF & STB_NO_VERIF) == STB_NO_VERIF) return ret;
  
  if (f!=NULL && trans==0) fprintf(f,"Node '%s' : %s (regex:'%s')\n", ttv_getnetname(tvf, section, tvs), getsygtype(tvs), infregex);
/*
  if (trans==0) finddir_data=__FINDDOWN__;
  else finddir_data=__FINDUP__;
*/
  stb_getclockandtestslopes(sf, &tvs->NODE[trans], clock, &clocksig_setup, &finddir_setup, &clocksig_hold, &finddir_hold);
            
  sprintf(section,INF_PATHDELAYMARGINPREFIX"|"INF_PATHDELAYMARGINMIN","INF_PATHDELAYMARGINCLOCK",""%s", finddir_setup==__FINDDOWN__?INF_PATHDELAYMARGINFALL:INF_PATHDELAYMARGINRISE);

  ttv_getmargin(ifl, tvs, infregex, section, &factor_clock, &delta_clock);

  sprintf(section,INF_PATHDELAYMARGINPREFIX"|"INF_PATHDELAYMARGINMAX","INF_PATHDELAYMARGINDATA",%s", trans==0?INF_PATHDELAYMARGINFALL:INF_PATHDELAYMARGINRISE);

  ttv_getmargin(ifl, tvs, infregex, section, &factor_data, &delta_data);

  if (!(factor_clock==1 && factor_data==1 && delta_clock==0 && delta_data==0))
    {
      // --------------------- SETUP -----------------------------

      pthck=NULL;
      CPD=DPD=0;
      if (clocksig_setup!=NULL)
        {
          if (finddir_setup==__FINDDOWN__) tve=&clocksig_setup->NODE[0]; else tve=&clocksig_setup->NODE[1];
          if (si->clockeventmin!=NULL && si->clockeventmin==tve)
            clockpath_min=si->clockpathsmin;
          else
            clockpath_min= ttv_getpathnocross(tvf, NULL, clocksig_setup, clocklist, TTV_DELAY_MAX, TTV_DELAY_MIN, findmode|TTV_FIND_SIG|TTV_FIND_MIN|finddir_setup|TTV_FIND_PATHDET);
          if (si->clockeventmin==NULL) si->clockeventmin=tve, si->clockpathsmin=clockpath_min;
        }
      else
        clockpath_min=NULL;
      
      if (clockpath_min!=NULL)
        {
          for (pth=clockpath_min; pth!=NULL; pth=pth->NEXT)
            {
//              if ((tvs->TYPE & (TTV_SIG_Q|TTV_SIG_R))!=0 && !isclockpath(pth, clocklist)) continue;

              newdatapathdelay=pth->DELAY*factor_clock+delta_clock;
              if (lowestD==TTV_NOTIME || pth->DELAYSTART+newdatapathdelay<oldSTART+oldNEWDELAY)
                {
                  lowestD=pth->DELAYSTART+newdatapathdelay;
                  CPD=oldDELAY=pth->DELAY;
                  oldSTART=pth->DELAYSTART;
                  oldNEWDELAY=newdatapathdelay;
                  pthck=pth;
                }
            }
          if (lowestD!=TTV_NOTIME)
            {
              margeclockf=oldNEWDELAY-oldDELAY;
            }
          else margeclockf=delta_clock;
        }
      else margeclockf=delta_clock;

      if (pthck!=NULL) cht=ttv_buildclockdetailht(na, tvf, pthck, findmode);
      else cht=NULL;

      overlapc=overlapd=0;
      if (si->datamax==NULL)
        si->datamax=ttv_getaccess(tvf, NULL, tvs, NULL, NULL, TTV_DELAY_MAX, TTV_DELAY_MIN, TTV_FIND_SIG|findmode|TTV_FIND_MAX);

      datapath_max=si->datamax;

      if (datapath_max!=NULL)
        {
          for (pth=datapath_max; pth!=NULL; pth=pth->NEXT)
            {
              if (pth->ROOT != &pth->ROOT->ROOT->NODE[trans]) continue;
              if ((tvs->TYPE & (TTV_SIG_Q|TTV_SIG_R))!=0  && pth->LATCH==NULL && isclockpath(pth, clocklist)) continue;
              newdatapathdelay=pth->DELAY*factor_data+delta_data;
              if (biggestU==TTV_NOTIME || pth->DELAYSTART+newdatapathdelay>=oldSTART+oldNEWDELAY)
                {
                  // calcul d'intersection
                  ttv_checkpathoverlap(na, tvf, pth, cht, findmode, &localoverlapc, &localoverlapd);
                  if (biggestU==TTV_NOTIME || pth->DELAYSTART+newdatapathdelay>oldSTART+oldNEWDELAY || (pth->DELAYSTART+newdatapathdelay==oldSTART+oldNEWDELAY && localoverlapd<overlapd))
                  {
                    // ---------------------
                    newdatapathdelay-=localoverlapd*factor_data;
                    if (biggestU==TTV_NOTIME || pth->DELAYSTART+newdatapathdelay>oldSTART+oldNEWDELAY)
                      {
                        biggestU=pth->DELAYSTART+newdatapathdelay;
                        DPD=pth->DELAY;
                        oldDELAY=pth->DELAY-localoverlapd;
                        oldSTART=pth->DELAYSTART;
                        oldNEWDELAY=newdatapathdelay;
                        overlapd=localoverlapd;
                        overlapc=localoverlapc;
                      }
                  }
                }
            }

          if (biggestU!=TTV_NOTIME)
            {
              margedataf=oldNEWDELAY-oldDELAY;
            }
          else margedataf=delta_data;
        }
      else margedataf=delta_data;
 
      if (clocksig_setup!=NULL)
        {
          if (si->clockeventmin!=tve) ttv_freepathlist(clockpath_min);
        }
      if (si->clean)
        {
          ttv_freepathlist(si->clockpathsmin);
          ttv_freepathlist(si->datamax);
        }

      if (cht!=NULL) delht(cht);

      // update du setup
      ret=1;
      margeclockf=margeclockf-overlapc*factor_clock+overlapc;
      
      margeclock=mbk_long_round(margeclockf);
      margedata=mbk_long_round(margedataf);

      if (f!=NULL) fprintf(f," Setup [data %s]: margin=%.1f%+.1f (c:%.1f%+.1f d:%.1f%+.1f oc:%+.1f od:%+.1f)\n", trans==0?"falling":"rising", clock->SETUP/TTV_UNIT, (margedata-margeclock)/TTV_UNIT, CPD/TTV_UNIT, margeclock/TTV_UNIT, DPD/TTV_UNIT, margedata/TTV_UNIT, overlapc/TTV_UNIT, overlapd/TTV_UNIT);

      stb_delaymargins_setval(node->EVENT, margedata, -margeclock, 1);
      for (ckl=node->CK; ckl!=NULL; ckl=ckl->NEXT)
        ckl->SETUP+=margedata-margeclock;
//      clock->SETUP+=margedata-margeclock;
      node->SETUP-=margedata-margeclock;
    }


  // --------------------- HOLD -----------------------------
  biggestU=TTV_NOTIME;
  lowestD=TTV_NOTIME;
  CPD=DPD=0;
  sprintf(section,INF_PATHDELAYMARGINPREFIX"|"INF_PATHDELAYMARGINMAX","INF_PATHDELAYMARGINCLOCK",""%s", finddir_hold==__FINDDOWN__?INF_PATHDELAYMARGINFALL:INF_PATHDELAYMARGINRISE);

  ttv_getmargin(ifl, tvs, infregex, section, &factor_clock, &delta_clock);

  sprintf(section,INF_PATHDELAYMARGINPREFIX"|"INF_PATHDELAYMARGINMIN","INF_PATHDELAYMARGINDATA",%s", trans==0?INF_PATHDELAYMARGINFALL:INF_PATHDELAYMARGINRISE);

  ttv_getmargin(ifl, tvs, infregex, section, &factor_data, &delta_data);

  if (!(factor_clock==1 && factor_data==1 && delta_clock==0 && delta_data==0))
    {
      pthck=NULL;

      if (clocksig_hold!=NULL)
        {
          if (finddir_hold==__FINDDOWN__) tve=&clocksig_hold->NODE[0]; else tve=&clocksig_hold->NODE[1];
          if (si->clockeventmax!=NULL && si->clockeventmax==tve)
            clockpath_max=si->clockpathsmax;
          else
            clockpath_max=ttv_getpathnocross(tvf, NULL, clocksig_hold, clocklist, TTV_DELAY_MAX, TTV_DELAY_MIN, findmode|TTV_FIND_SIG|TTV_FIND_MAX|finddir_hold|TTV_FIND_PATHDET);
          if (si->clockeventmax==NULL) si->clockeventmax=tve, si->clockpathsmax=clockpath_max;
        }
      else
        clockpath_max=NULL;
      
      if (clockpath_max!=NULL)
        {
          
          for (pth=clockpath_max; pth!=NULL; pth=pth->NEXT)
            {
//              if ((tvs->TYPE & (TTV_SIG_Q|TTV_SIG_R))!=0 && !isclockpath(pth, clocklist)) continue;

              newdatapathdelay=pth->DELAY*factor_clock+delta_clock;
              if (lowestD==TTV_NOTIME || pth->DELAYSTART+newdatapathdelay>oldSTART+oldNEWDELAY)
                {
                  lowestD=pth->DELAYSTART+newdatapathdelay;
                  CPD=oldDELAY=pth->DELAY;
                  oldSTART=pth->DELAYSTART;
                  oldNEWDELAY=newdatapathdelay;
                  pthck=pth;
                }
            }
          if (lowestD!=TTV_NOTIME)
            {
              margeclockf=oldNEWDELAY-oldDELAY;
            }
          else margeclockf=delta_clock;
        }
      else margeclockf=delta_clock;

      if (pthck!=NULL) cht=ttv_buildclockdetailht(na, tvf, pthck, findmode);
      else cht=NULL;

      overlapc=overlapd=0;
      if (si->datamin==NULL)
        si->datamin=ttv_getaccess(tvf, NULL, tvs, NULL, NULL, TTV_DELAY_MAX, TTV_DELAY_MIN, TTV_FIND_SIG|findmode|TTV_FIND_MIN);
      
      datapath_min=si->datamin;

      if (datapath_min!=NULL)
        {
          for (pth=datapath_min; pth!=NULL; pth=pth->NEXT)
            {
              if (pth->ROOT != &pth->ROOT->ROOT->NODE[trans]) continue;
              if ((tvs->TYPE & (TTV_SIG_Q|TTV_SIG_R))!=0 && pth->LATCH==NULL && isclockpath(pth, clocklist)) continue;
              newdatapathdelay=pth->DELAY*factor_data+delta_data;
              if (biggestU==TTV_NOTIME || pth->DELAYSTART+newdatapathdelay<=oldSTART+oldNEWDELAY)
                {
                   // calcul d'intersection
                  ttv_checkpathoverlap(na, tvf, pth, cht, findmode, &localoverlapc, &localoverlapd);
                  if (biggestU==TTV_NOTIME || pth->DELAYSTART+newdatapathdelay<oldSTART+oldNEWDELAY || (pth->DELAYSTART+newdatapathdelay==oldSTART+oldNEWDELAY && localoverlapd>overlapd))
                  {
                    // ---------------------
                    newdatapathdelay-=localoverlapd*factor_data;
                    if (biggestU==TTV_NOTIME || pth->DELAYSTART+newdatapathdelay<oldSTART+oldNEWDELAY)
                      {
                        biggestU=pth->DELAYSTART+newdatapathdelay;
                        DPD=pth->DELAY;
                        oldDELAY=pth->DELAY-localoverlapd;
                        oldSTART=pth->DELAYSTART;
                        oldNEWDELAY=newdatapathdelay;
                        overlapd=localoverlapd;
                        overlapc=localoverlapc;
                      }
                  }
                }
            }

          if (biggestU!=TTV_NOTIME)
            {
              margedataf=oldNEWDELAY-oldDELAY;
            }
          else margedataf=delta_data;
        }
      else margedataf=delta_data;
 
      
       if (clocksig_hold!=NULL)
        {
          if (si->clockeventmax!=tve) ttv_freepathlist(clockpath_max);
        }
       if (si->clean)
        {
          ttv_freepathlist(si->clockpathsmax);
          ttv_freepathlist(si->datamin);
        }

      if (cht!=NULL) delht(cht);

      // update du hold
      ret=1;
      margeclockf=margeclockf-overlapc*factor_clock+overlapc;

      margeclock=mbk_long_round(margeclockf);
      margedata=mbk_long_round(margedataf);
      
      if (f!=NULL) fprintf(f," Hold  [data %s]: margin=%.1f%+.1f (c:%.1f%+.1f d:%.1f%+.1f oc:%+.1f od:%+.1f)\n", trans==0?"falling":"rising", clock->HOLD/TTV_UNIT, (-margedata+margeclock)/TTV_UNIT, CPD/TTV_UNIT, margeclock/TTV_UNIT, DPD/TTV_UNIT, margedata/TTV_UNIT, overlapc/TTV_UNIT, overlapd/TTV_UNIT);
      
      stb_delaymargins_setval(node->EVENT, -margedata, margeclock, 0);
      for (ckl=node->CK; ckl!=NULL; ckl=ckl->NEXT)
        ckl->HOLD+=-margedata+margeclock;
//      clock->HOLD+=-margedata+margeclock;
      node->HOLD-=-margedata+margeclock;
    }
  return ret;
}

/*
// ----------------------------------- SLOW MODE ----------------------------------------------

static int stb_find_latch_margin_slowmode(FILE *f, ttvfig_list *tvf, ttvsig_list *tvs, int trans, inffig_list *ifl, char *infregex, chain_list *clocklist, NameAllocator *na, long findmode)
{
  ttvpath_list *datapath_max, *clockpath_max, *pth, *pthck;
  ttvpath_list *datapath_min, *clockpath_min;
  chain_list *clklist, *datalist;
  float factor_data, factor_clock;
  long delta_data, delta_clock;
  long newdatapathdelay, oldDELAY, oldSTART, oldNEWDELAY, biggestU=TTV_NOTIME, lowestD=TTV_NOTIME, newclockpathdelay;
  long margeclock, margedata, overlap, marginadd;
  stbck *clock;
  ttvsig_list *clocksig_setup, *clocksig_hold;
  long finddir_setup, finddir_hold, finddir_data, clockadd, dataadd, oversub;
  stbnode     *node;
  char section[1024];
  int ret=0;
  ht *cht;

  node=stb_getstbnode(&tvs->NODE[trans]);
  clock=node->CK;
  if (node->SETUP==STB_NO_TIME && node->HOLD==STB_NO_TIME) return ret;
  if (clock == NULL || (clock->VERIF & STB_NO_VERIF) == STB_NO_VERIF) return ret;
  
  if (f!=NULL && trans==0) fprintf(f,"Node '%s' : %s (regex:'%s')\n", ttv_getnetname(tvf, section, tvs), getsygtype(tvs), infregex);

  if (trans==0) finddir_data=__FINDDOWN__;
  else finddir_data=__FINDUP__;

  getclockandtestslopes(tvs, clock, &clocksig_setup, &finddir_setup, &clocksig_hold, &finddir_hold);
            
  sprintf(section,INF_PATHDELAYMARGINPREFIX"|"INF_PATHDELAYMARGINMIN","INF_PATHDELAYMARGINCLOCK",""%s", finddir_setup==__FINDDOWN__?INF_PATHDELAYMARGINFALL:INF_PATHDELAYMARGINRISE);

  getmargin(ifl, tvs, infregex, section, &factor_clock, &delta_clock);

  sprintf(section,INF_PATHDELAYMARGINPREFIX"|"INF_PATHDELAYMARGINMAX","INF_PATHDELAYMARGINDATA",%s", trans==0?INF_PATHDELAYMARGINFALL:INF_PATHDELAYMARGINRISE);

  getmargin(ifl, tvs, infregex, section, &factor_data, &delta_data);

  if (!(factor_clock==1 && factor_data==1 && delta_clock==0 && delta_data==0))
    {
      // --------------------- SETUP -----------------------------

      pthck=NULL;

      if (clocksig_setup!=NULL)
        clockpath_min= ttv_getpathnocross(tvf, NULL, clocksig_setup, NULL, TTV_DELAY_MAX, TTV_DELAY_MIN, findmode|TTV_FIND_SIG|TTV_FIND_MIN|finddir_setup);
      else
        clockpath_min=NULL;

      if (clockpath_min!=NULL)
        {
          datapath_max=ttv_getsigaccess(tvf, NULL, tvs, NULL, NULL, TTV_DELAY_MAX, TTV_DELAY_MIN, TTV_FIND_SIG|findmode|TTV_FIND_MAX|finddir_data);
      
          for (pthck=clockpath_min; pthck!=NULL; pthck=pthck->NEXT)
            {
              if ((tvs->TYPE & (TTV_SIG_Q|TTV_SIG_R))!=0  && !isclockpath(pthck, clocklist)) continue;

              cht=buildclockdetailht(na, tvf, pthck, findmode);

              newclockpathdelay=pthck->DELAY*factor_clock+delta_clock;

              for (pth=datapath_max; pth!=NULL; pth=pth->NEXT)
                {
                  if ((tvs->TYPE & (TTV_SIG_Q|TTV_SIG_R))!=0  && pth->LATCH==NULL && isclockpath(pth, clocklist)) continue;

                  newdatapathdelay=pth->DELAY*factor_data+delta_data;

                  // calcul d'intersection
                  overlap=checkpathoverlap(na, tvf, pth, cht, findmode);
                  // ---------------------
                  newdatapathdelay-=overlap*factor_data;
                  newclockpathdelay-=overlap*factor_clock;

                  margeclock=newclockpathdelay-pthck->DELAY;
                  margedata=newdatapathdelay-pth->DELAY;

                  marginadd=margedata-margeclock;

                  if (biggestU==TTV_NOTIME || marginadd>biggestU)
                    {
                      biggestU=marginadd;
                      clockadd=margeclock;
                      dataadd=margedata;
                      oversub=overlap;
                    }
                }
              delht(cht);
            }
          ttv_freepathlist(datapath_max);
        } 
      ttv_freepathlist(clockpath_min);

      if (biggestU!=TTV_NOTIME)
        {
          // update du setup
          ret=1;
          
          if (f!=NULL) fprintf(f," Setup [data %s]: margin=%.1f%+.1f (c:%+.1f d:%+.1f o:%+.1f)\n", trans==0?"falling":"rising", clock->SETUP/TTV_UNIT, biggestU/TTV_UNIT, clockadd/TTV_UNIT, dataadd/TTV_UNIT, oversub/TTV_UNIT);
          
          clock->SETUP+=biggestU;
          node->SETUP-=biggestU;
        }
    }

#if 0
  // --------------------- HOLD -----------------------------

  sprintf(section,INF_PATHDELAYMARGINPREFIX"|"INF_PATHDELAYMARGINMAX","INF_PATHDELAYMARGINCLOCK",""%s", finddir_hold==__FINDDOWN__?INF_PATHDELAYMARGINFALL:INF_PATHDELAYMARGINRISE);

  getmargin(ifl, tvs, infregex, section, &factor_clock, &delta_clock);

  sprintf(section,INF_PATHDELAYMARGINPREFIX"|"INF_PATHDELAYMARGINMIN","INF_PATHDELAYMARGINDATA",%s", trans==0?INF_PATHDELAYMARGINFALL:INF_PATHDELAYMARGINRISE);

  getmargin(ifl, tvs, infregex, section, &factor_data, &delta_data);

  if (!(factor_clock==1 && factor_data==1 && delta_clock==0 && delta_data==0))
    {
      pthck=NULL;

      if (clocksig_hold!=NULL)
        clockpath_max=ttv_getpathnocross(tvf, NULL, clocksig_hold, NULL, TTV_DELAY_MAX, TTV_DELAY_MIN, findmode|TTV_FIND_SIG|TTV_FIND_MAX|finddir_hold);
      else
        clockpath_max=NULL;
      
      if (clockpath_max!=NULL)
        {
          
          for (pth=clockpath_max; pth!=NULL; pth=pth->NEXT)
            {
              if ((tvs->TYPE & (TTV_SIG_Q|TTV_SIG_R))!=0  && !isclockpath(pth, clocklist)) continue;
              newdatapathdelay=pth->DELAY*factor_clock+delta_clock;
              if (lowestD==TTV_NOTIME || pth->DELAYSTART+newdatapathdelay>oldSTART+oldNEWDELAY)
                {
                  lowestD=pth->DELAYSTART+newdatapathdelay;
                  oldDELAY=pth->DELAY;
                  oldSTART=pth->DELAYSTART;
                  oldNEWDELAY=newdatapathdelay;
                  pthck=pth;
                }
            }
          if (lowestD!=TTV_NOTIME)
            {
              // calcul nouvelle marge et mise a jour du setup
              // marge=diff_data+diffclock
              margeclock=oldNEWDELAY-oldDELAY;
              
            }
          else margeclock=0;
        }
      else margeclock=0;

      if (pthck!=NULL) cht=buildclockdetailht(na, tvf, pthck);
      else cht=NULL;

      overlap=0;
      datapath_min=ttv_getaccess(tvf, NULL, tvs, NULL, NULL, TTV_DELAY_MAX, TTV_DELAY_MIN, TTV_FIND_SIG|findmode|TTV_FIND_MIN|finddir_data);

      if (datapath_min!=NULL)
        {
          for (pth=datapath_min; pth!=NULL; pth=pth->NEXT)
            {
              if ((tvs->TYPE & (TTV_SIG_Q|TTV_SIG_R))!=0 && pth->LATCH==NULL && isclockpath(pth, clocklist)) continue;
              newdatapathdelay=pth->DELAY*factor_data+delta_data;
              if (biggestU==TTV_NOTIME || pth->DELAYSTART+newdatapathdelay<oldSTART+oldNEWDELAY)
                {
                   // calcul d'intersection
                  overlap=checkpathoverlap(na, tvf, pth, cht);
                  // ---------------------
                  newdatapathdelay-=overlap*factor_data;
                  if (biggestU==TTV_NOTIME || pth->DELAYSTART+newdatapathdelay<oldSTART+oldNEWDELAY)
                    {
                      biggestU=pth->DELAYSTART+newdatapathdelay;
                      oldDELAY=pth->DELAY;
                      oldSTART=pth->DELAYSTART;
                      oldNEWDELAY=newdatapathdelay;
                    }
                }
            }

          if (biggestU!=TTV_NOTIME)
            {
              // calcul nouvelle marge et mise a jour du setup
              // marge=diff_data+diffclock
              margedata=oldNEWDELAY-oldDELAY;
            }
          else margedata=0;
        }
      else margedata=0;
 
      
      ttv_freepathlist(clockpath_max);
      ttv_freepathlist(datapath_min);

      if (cht!=NULL) delht(cht);

      if (biggestU!=TTV_NOTIME)
        {
          // update du hold
          ret=1;
          margeclock-=overlap*factor_clock;
          
          if (f!=NULL) fprintf(f," Hold  [data %s]: margin=%.1f%+.1f (c:%+.1f d:%+.1f o:%+.1f)\n", trans==0?"falling":"rising", clock->HOLD/TTV_UNIT, (-margedata+margeclock)/TTV_UNIT, margeclock/TTV_UNIT, margedata/TTV_UNIT, overlap/TTV_UNIT);
          
          clock->HOLD+=-margedata+margeclock;
          node->HOLD-=-margedata+margeclock;
        }
    }
#endif
  return ret;
}
*/


void stb_computedelaymargin(stbfig_list *sbf)
{
  chain_list *chain, *chainsig, *cl, *list, *ch;
  chain_list *clocks;
  ttvsig_list *tvs;
  inffig_list *ifl;
  int multimatch=0, ret1, ret2, cnt;
  FILE *f;
  NameAllocator na;
  long find;
  searchinfo si;
  char buf[1024];
  ptype_list *pt;

  if ((ifl=getinffig(sbf->FIG->INFO->FIGNAME))==NULL) return;

  if (sbf->GRAPH == STB_RED_GRAPH)
    find = TTV_FIND_PATH;
  else
    find = TTV_FIND_LINE;


  list=ttv_getallmarginregex(ifl);
  if (list!=NULL)
    {
      if ((f=mbkfopen(sbf->FIG->INFO->FIGNAME, "PathMargins", WRITE_TEXT))==NULL)
        avt_error("stb", 157, AVT_WAR, "could not open path margins report file\n");
      else
        {
          avt_printExecInfo(f, "#", "Path margin report file", "");
          fprintf(f, "#------ Legend -----\n");
          fprintf(f, "# Node '<nodename>' : <node type> (regex:<rule used in inf section>)\n");
          fprintf(f, "#  <Setup|Hold> [data <direction>]: margin=<orig margin>+-<margin added>\n");
          fprintf(f, "#     ( c:<original clockpath delay>+-<clockpath delta>\n");
          fprintf(f, "#       d:<original datapath delay>+-<datapath delta>\n");
          fprintf(f, "#       oc:<overlapping clock path delay>\n");
          fprintf(f, "#       od:<overlapping data path delay> )\n");
          fprintf(f, "#--------------------\n\n");
        }
#if 0
      clocks=ttv_getclocksiglist(sbf->FIG);

      CreateNameAllocator(257, &na, 'y');
      cnt=0;

      ch=NULL;
      for (cl=list; cl!=NULL; cl=cl->NEXT)
        {
          chain=addchain(NULL, cl->DATA);
          
          chainsig = ttv_getsigbytype_and_netname(sbf->FIG,NULL,TTV_SIG_C|TTV_SIG_L|TTV_SIG_B|TTV_SIG_R|TTV_SIG_Q,chain);
          freechain(chain);
          
          for (chain=chainsig; chain!=NULL; chain=chain->NEXT)
            {
              tvs=(ttvsig_list *)chain->DATA;
              if (getptype(tvs->USER, TTV_SIG_PATH_DELAY_MARGINS)==NULL)
                {
                  ttv_activate_path_and_access_mode(1);
#if 1
                  si.datamax=si.datamin=NULL, si.clean=0;
                  si.clockeventmax=si.clockeventmin=NULL;
                  si.clockpathsmax=si.clockpathsmin=NULL;
                  ret1=stb_find_latch_margin_fastmode(f, sbf->FIG, tvs, 0, ifl, (char *)cl->DATA, clocks, &na, find, &si);
                  si.clean=1;
                  ret2=stb_find_latch_margin_fastmode(f, sbf->FIG, tvs, 1, ifl, (char *)cl->DATA, clocks, &na, find, &si);
#else
                  ret1=stb_find_latch_margin_slowmode(f, sbf->FIG, tvs, 0, ifl, (char *)cl->DATA, clocks, &na, find);
                  ret2=stb_find_latch_margin_slowmode(f, sbf->FIG, tvs, 1, ifl, (char *)cl->DATA, clocks, &na, find);
#endif
                  cnt++;
                  if (ret1 || ret2)
                    {
                      tvs->USER=addptype(tvs->USER, TTV_SIG_PATH_DELAY_MARGINS, (char *)cl->DATA);
                      ch=addchain(ch, tvs);
                    }
                  ttv_activate_path_and_access_mode(0);
                }
              else
                multimatch++;
              if (cnt>2000) 
                {
                  cnt=0;
                  DeleteNameAllocator(&na);
                  CreateNameAllocator(257, &na, 'y');
                }
            }
          freechain(chainsig);
        }

      DeleteNameAllocator(&na);

      for (cl=ch; cl!=NULL; cl=cl->NEXT)
        {
          tvs=(ttvsig_list *)cl->DATA;
          tvs->USER=delptype(tvs->USER, TTV_SIG_PATH_DELAY_MARGINS);
          if (getptype(tvs->USER, STB_SETUP)!=NULL)
            tvs->USER=delptype(tvs->USER, STB_SETUP);
          if (getptype(tvs->USER, STB_HOLD)!=NULL)
            tvs->USER=delptype(tvs->USER, STB_HOLD);
        }

      freechain(clocks);
      freechain(ch);

      if (multimatch>0)
        {
          avt_error("stb", 156, AVT_WAR, "%d nodes have been matched by several entries in inf section 'PathDelayMargin'\n", multimatch);
          if (f!=NULL) fprintf(f,"%d nodes have been matched by several entries in inf section 'PathDelayMargin'\n", multimatch);
        }
#else
      if (f!=NULL) fprintf(f, "# WARNING: per path margin implemented, this file does not contain the worst path margins anymore\n\n");
      chain=addchain(NULL, "*");      
      chainsig = ttv_getsigbytype_and_netname(sbf->FIG,NULL,TTV_SIG_C|TTV_SIG_L|TTV_SIG_B|TTV_SIG_R|TTV_SIG_Q,chain);
      freechain(chain);
      
      for (chain=chainsig; chain!=NULL; chain=chain->NEXT)
        {
          tvs=(ttvsig_list *)chain->DATA;
          if ((pt=getptype(tvs->USER, TTV_SIG_PATH_DELAY_MARGINS_REGEX))!=NULL)
             if (f!=NULL) fprintf(f,"Node '%s' : %s (regex:'%s')\n", ttv_getnetname(sbf->FIG, buf, tvs), getsygtype(tvs), (char *)pt->DATA);
        }
      freechain(chainsig);
#endif
      if (f!=NULL) fclose(f);
    }
  freechain(list);
}


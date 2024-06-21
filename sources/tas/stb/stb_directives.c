#include <stdlib.h>

#include MUT_H
#include STB_H
#include "stb_debug.h"
#include "stb_transfer.h"
#include "stb_util.h"
#include "stb_directives.h"
#include "stb_falseslack.h"

static void stb_sd_getstab(stbfig_list *stbfig, stbnode *node, long dir, stbpair_list **STBTAB)
{
  stbpair_list *pair, *tmp;
  stbpair_list *STBHZ[256];
  int i, flag;

  for (i=0; i<node->NBINDEX; i++) STBHZ[i]=NULL, STBTAB[i]=NULL;
  if (dir & TTV_NODE_DOWN) 
    {

      node=stb_getstbnode(&node->EVENT->ROOT->NODE[0]);
      flag=(node->FLAG & STB_NODE_STABCORRECT)!=0?STB_TRANSFERT_CORRECTION:0;      
      stb_transferstbline (stbfig, node->EVENT, node , STBTAB, STBHZ, 0, 1, NULL, NULL, 1, flag|STB_TRANSFERT_NOFILTERING|STB_TRANSFERT_NOPHASECHANGE) ;

      for (i=0; i<node->NBINDEX; i++)
        {
          pair=stb_globalstbpair(STBTAB[i]);
          stb_freestbpair(STBTAB[i]);
          STBTAB[i]=pair;
        }
      stb_freestbtabpair(STBHZ, node->NBINDEX);
      
      /* if (pair!=NULL) 
         {
         *min=pair->D, *max=pair->U;
         *phasemin=pair->phase_D, *phasemax=pair->phase_U;
         *flagmin=*flagmax=STB_DEBUG_DIRECTIVE_RISING;
         }
         stb_freestbpair (pair);*/
    }
  for (i=0; i<node->NBINDEX; i++) STBHZ[i]=NULL, STBTAB[256+i]=NULL;
  if (dir & TTV_NODE_UP) 
    {
      node=stb_getstbnode(&node->EVENT->ROOT->NODE[1]);
      flag=(node->FLAG & STB_NODE_STABCORRECT)!=0?STB_TRANSFERT_CORRECTION:0;      
      stb_transferstbline (stbfig, node->EVENT, node , &STBTAB[256], STBHZ, 0, 1, NULL, NULL, 1, flag|STB_TRANSFERT_NOFILTERING|STB_TRANSFERT_NOPHASECHANGE) ;
      
      for (i=0; i<node->NBINDEX; i++)
        {
          pair=stb_globalstbpair(STBTAB[256+i]);
          stb_freestbpair(STBTAB[256+i]);
          STBTAB[256+i]=pair;
        }
      stb_freestbtabpair(STBHZ, node->NBINDEX);

      /*      if (pair!=NULL )
              {
              if (*min!=STB_NO_TIME)
              {
              if (pair->D<*min) *min=pair->D, *phasemin=pair->phase_D, *flagmin=STB_DEBUG_DIRECTIVE_FALLING;
              if (pair->U>*max) *max=pair->U, *phasemax=pair->phase_U, *flagmax=STB_DEBUG_DIRECTIVE_FALLING;
              }
              else 
              {
              *min=pair->D, *max=pair->U;
              *phasemin=pair->phase_D, *phasemax=pair->phase_U;
              *flagmin=*flagmax=STB_DEBUG_DIRECTIVE_FALLING;
              }
            
              }
              stb_freestbpair (pair);*/
    }
}

static char stb_directive_getphase(stbnode *node)
{
  if (node->CK->CTKCKINDEX!=STB_NO_INDEX) return node->CK->CTKCKINDEX;
  if (node->CK->CKINDEX!=STB_NO_INDEX) return node->CK->CKINDEX;
  return STB_NO_INDEX;
}
static int stb_isafter(stbfig_list *sf, stbnode *node, char phase)
{
  ttvevent_list *tve;
  stbck *ck;
  stbnode *cknode;
  long orig, dest;
  char p;

  // pas de ckindex sur les noeuds intermediaires, j'utilise ctkckindex
  // on fait + confiance au ctkckindex: les 2 events des commandes ont la meme phase sinon
  if ((p=stb_directive_getphase(node))!=STB_NO_INDEX)
    {
      if (stb_cmpphase(sf,p,phase)>=0/*p>=phase*/) return 1;
      else return 0;
    }
#ifdef PHASEFORALL  
  else printf("internal error\n");
#endif
  exit (15);
  ck=stb_getclocknode(sf, phase, NULL, &tve, NULL);
  if (!ck) return -1;
  if ((tve->TYPE & TTV_NODE_UP)!=0) orig=ck->SUPMAX; else orig=ck->SDNMAX;
  if (node->CK->ORIGINAL_CLOCK==NULL) return -1;
  cknode=stb_getstbnode(node->CK->ORIGINAL_CLOCK);
  if ((node->CK->ORIGINAL_CLOCK->TYPE & TTV_NODE_UP)!=0) dest=cknode->CK->SUPMIN; else dest=cknode->CK->SDNMIN;
  if (dest==STB_NO_TIME || orig==STB_NO_TIME) return -1;
  if (dest>=orig) return 1;
  return 0;
}

static void stb_sd_getedge(stbnode *node, stbck *res)
{
  int i;

  res->PERIOD=STB_NO_TIME;
  res->CKINDEX=res->CTKCKINDEX=stb_directive_getphase(node);
  for (i=0; i<2; i++)
    {
      node=stb_getstbnode(&node->EVENT->ROOT->NODE[i]);
      if (node->CK!=NULL) 
        {
          if (node->CK->PERIOD!=STB_NO_TIME) res->PERIOD=node->CK->PERIOD;
          if (node->CK->SUPMAX!=STB_NO_TIME) res->SUPMAX=node->CK->SUPMAX;
          if (node->CK->SUPMIN!=STB_NO_TIME) res->SUPMIN=node->CK->SUPMIN;
          if (node->CK->SDNMAX!=STB_NO_TIME) res->SDNMAX=node->CK->SDNMAX;
          if (node->CK->SDNMIN!=STB_NO_TIME) res->SDNMIN=node->CK->SDNMIN;
        }
    }
}


static void initlongstab(stbpair_list **STBTAB, int nbindex)
{
  int i;
  for (i=0; i<nbindex; i++)
    STBTAB[i]=NULL, STBTAB[256+i]=NULL;
}
static void freelongstab(stbpair_list **STBTAB, int nbindex)
{
  stb_freestbtabpair(STBTAB, nbindex);
  stb_freestbtabpair(&STBTAB[256], nbindex);
}


static int stb_getfromminmax(ttvfig_list *ttvfig, stbck *input, stbnode *node, char *startmask, long *min, long *max, long indir)
{
  char buf[1024];
  int ok=0;
  stbnode *clocknode;

  *min=*max=STB_NO_TIME;
  if (input!=NULL && input->ORIGINAL_CLOCK!=NULL && (input->ORIGINAL_CLOCK->TYPE & indir)!=0)
    {
      ttv_getnetname(ttvfig,buf,input->ORIGINAL_CLOCK->ROOT);
      if (mbk_TestREGEX(buf, startmask)) ok=1;
      else 
        {
          ttv_getsigname(ttvfig,buf,input->ORIGINAL_CLOCK->ROOT);
          if (mbk_TestREGEX(buf, startmask)) ok=1;
        }
      if (ok)
        {
          clocknode=stb_getstbnode(input->ORIGINAL_CLOCK);
          if (clocknode->EVENT->TYPE & TTV_NODE_UP) *min=clocknode->CK->SUPMIN, *max=clocknode->CK->SUPMAX;
          else *min=clocknode->CK->SDNMIN, *max=clocknode->CK->SDNMAX;
          return 1;
        }
    }

  if ((node->EVENT->ROOT->TYPE & TTV_SIG_C)!=0 && (node->EVENT->TYPE & indir)!=0)
    {
      ttv_getnetname(ttvfig,buf,node->EVENT->ROOT);
      if (mbk_TestREGEX(buf, startmask)) ok=1;
      else 
        {
          ttv_getsigname(ttvfig,buf,node->EVENT->ROOT);
          if (mbk_TestREGEX(buf, startmask)) ok=1;
        }

      if (ok) return 1;
    }
  return 0;
}

stbdebug_list *stb_calc_directive_setuphold(stbfig_list *stbfig, stbdebug_list *debug, stbnode *node, long margin, long computeflags, ptype_list *geneclockinfo)
{
  ttvevent_list *event, *sig1event ;
  ttvevent_list *ptevent ;
  ttvsig_list *ptsig ;
  stbdelay_list *delay, *useddelay[2];
  chain_list *chainsig = NULL ;
  chain_list *chain ;
  stbnode *nodex ;
  stbdomain_list *ptdomain = NULL;
  stbpair_list *tabpair[256] ;
  stbpair_list *tabhz[256] ;
  stbpair_list *ptstbpair, *uppair, *downpair ;
  stbpair_list *pts ;
  stbck *ck, *inputck, *run_inputck ;
  long delaymax, startmin, startmax ;
  long delaymin ;
  long delaym ;
  long periode ;
  long periodemax[256] ;
  long periodemin[256] ;
  long input_periode, output_periode ;
  int k ;
  int j ;
  char u, specin ;
  char v ;
  char ckedge ;
  char phaseused[256];
  int transparence, has_multicycle;
  long periodeSetup, periodeHold, marginv, mode_period, subbed_period;
  datamoved_type datamoved;
  int datamoved_start;
  EventPeriodeHold_type EventPeriodeHold[2];
  int diff, doonce;
  int involved_input_transistion[2][4];
  Extended_Setup_Hold_Computation_Data eshcd[2];
  stbpair_list *STBTAB[2][256], **pSTBTAB;
  stbpair_list *STBHZ[2][256], **pSTBHZ;
  inffig_list *ifl;
  ttvsig_list *sig;
  // ------------

  ttv_directive *sd;
  long setup, hold;
  long datamin, datamax;
  long clockmin, clockmax, dir, worstsetup, worsthold;
  stbck edge, oedge;
  stbnode *other_node;
  int stab;
  Setup_Hold_Computation_Detail detail[2], *thisdetail, *otherdetail;
  short flags, flagmin, flagmax;
  int phasemin;
  stbpair_list *STBTAB_DEST[512];
  int i, id, rfd, outdir, falseaccessmode=0, falseslack_setup[2], falseslack_hold[2];
  long period, mperiod, cmperiod;
  ttvevent_list *directiveclock;
  
  if ((stbfig->FLAGS & STB_HAS_FALSE_ACCESS)!=0 && V_BOOL_TAB[__STB_HANDLE_FALSE_ACCESS].VALUE)
   { 
     if(ttv_canbeinfalsepath(node->EVENT->ROOT->NODE, 'o') || ttv_canbeinfalsepath(node->EVENT->ROOT->NODE+1, 'o'))
     {
        if (ttv_hasaccessfalsepath(stbfig->FIG, node->EVENT->ROOT->NODE, stbfig->CLOCK) || ttv_hasaccessfalsepath(stbfig->FIG, node->EVENT->ROOT->NODE+1, stbfig->CLOCK))
          falseaccessmode=1;
     }
   }
 
  if ((sd=ttv_get_directive(node->EVENT->ROOT))!=NULL)
    {
      sig=node->EVENT->ROOT;
      while (sd!=NULL)
        {
          if (sd->filter==0)
            {              
              ifl=getloadedinffig(stbfig->FIG->INFO->FIGNAME);
              if (node->EVENT->TYPE & TTV_NODE_UP) outdir=1; else outdir=0;
              thisdetail=&detail[outdir], otherdetail=&detail[(outdir+1)&1];
              otherdetail->setup.VALUE=STB_NO_TIME;
              otherdetail->hold.VALUE=STB_NO_TIME;
              flags=STB_DEBUG_DIRECTIVE;

              edge.PERIOD=oedge.PERIOD=STB_NO_TIME;
              edge.SUPMIN=edge.SUPMAX=edge.SDNMIN=edge.SDNMAX=STB_NO_TIME;
              oedge.SUPMIN=oedge.SUPMAX=oedge.SDNMIN=oedge.SDNMAX=STB_NO_TIME;
              cmperiod=0;
              stab=0;
              if ((sd->target1_dir & INF_DIRECTIVE_CLOCK)!=0)
                {
                  if (((sd->target1_dir & INF_DIRECTIVE_RISING)!=0 && (node->EVENT->TYPE & TTV_NODE_UP)!=0)
                      || ((sd->target1_dir & INF_DIRECTIVE_FALLING)!=0 && (node->EVENT->TYPE & TTV_NODE_DOWN)!=0))
                    {
                      flags|=STB_DEBUG_DIRECTIVE_DATA_IS_CLOCK;
                    }
                  else { sd=sd->next; continue; }
                }
              else
                {
                  if (!(sd->target1_dir==0
                        || ((sd->target1_dir & INF_DIRECTIVE_RISING)!=0 && (node->EVENT->TYPE & TTV_NODE_UP)!=0)
                        || ((sd->target1_dir & INF_DIRECTIVE_FALLING)!=0 && (node->EVENT->TYPE & TTV_NODE_DOWN)!=0)))
                    { sd=sd->next; continue; }
                }

              if (sd->target2!=NULL)
                {
                  other_node=stb_getstbnode(&sd->target2->NODE[0]);

                  if ((sd->target2_dir & INF_DIRECTIVE_CLOCK)!=0)
                    {
                      initlongstab(STBTAB_DEST, node->NBINDEX);
                      stb_sd_getedge(other_node, &oedge);
                      if (oedge.PERIOD==STB_NO_TIME) { sd=sd->next; continue; }
                      if ((sd->target2_dir & INF_DIRECTIVE_RISING)!=0)
                        {
                          clockmin=oedge.SUPMIN; flagmin=flagmax=STB_DEBUG_DIRECTIVE_RISING;
                          clockmax=oedge.SUPMAX;
                          phasemin=stb_directive_getphase(stb_getstbnode(directiveclock=&sd->target2->NODE[1]));
                          STBTAB_DEST[256+phasemin]=stb_addstbpair(STBTAB_DEST[256+phasemin], clockmin, clockmax);
                        }
                      else if ((sd->target2_dir & INF_DIRECTIVE_FALLING)!=0)
                        {
                          clockmin=oedge.SDNMIN; flagmin=flagmax=STB_DEBUG_DIRECTIVE_FALLING;
                          clockmax=oedge.SDNMAX;
                          phasemin=stb_directive_getphase(stb_getstbnode(directiveclock=&sd->target2->NODE[0]));
                          STBTAB_DEST[phasemin]=stb_addstbpair(STBTAB_DEST[phasemin], clockmin, clockmax);
                        }
                      else
                        {
                          if ((sd->target2_dir & INF_DIRECTIVE_UP)!=0)
                            {
                              clockmin=oedge.SUPMIN; flagmin=STB_DEBUG_DIRECTIVE_RISING;
                              clockmax=oedge.SDNMAX; flagmax=STB_DEBUG_DIRECTIVE_FALLING;
                              if (clockmax<clockmin) clockmax+=oedge.PERIOD, cmperiod=oedge.PERIOD;
                              phasemin=stb_directive_getphase(stb_getstbnode(directiveclock=&sd->target2->NODE[1]));
                              STBTAB_DEST[256+phasemin]=stb_addstbpair(STBTAB_DEST[256+phasemin], clockmin, clockmax);
                            }
                          else
                            {
                              clockmin=oedge.SDNMIN; flagmin=STB_DEBUG_DIRECTIVE_FALLING;
                              clockmax=oedge.SUPMAX; flagmax=STB_DEBUG_DIRECTIVE_RISING;
                              if (clockmax<clockmin) clockmax+=oedge.PERIOD, cmperiod=oedge.PERIOD;
                              phasemin=stb_directive_getphase(stb_getstbnode(directiveclock=&sd->target2->NODE[0]));
                              STBTAB_DEST[phasemin]=stb_addstbpair(STBTAB_DEST[phasemin], clockmin, clockmax);
                            }
                        }
                    }
                  else
                    {
                      oedge.PERIOD=0;
                      dir=0;
                      if (sd->target2_dir==0 || (sd->target2_dir & INF_DIRECTIVE_RISING)!=0) dir|=TTV_NODE_UP;
                      if (sd->target2_dir==0 || (sd->target2_dir & INF_DIRECTIVE_FALLING)!=0) dir|=TTV_NODE_DOWN;
                      stb_sd_getstab(stbfig, other_node,  dir, STBTAB_DEST);
                      flags|=STB_DEBUG_DIRECTIVE_CLOCK_IS_DATA;
                      phasemin=STB_NO_INDEX;
                      directiveclock=NULL;
                    }
                }
              else
                {
                  other_node=node;
                  stb_sd_getedge(other_node, &oedge);
                  oedge.PERIOD=0;
                  phasemin=STB_NO_INDEX;
                  directiveclock=NULL;
                  flags|=STB_DEBUG_DIRECTIVE_DELAYMODE;
                  dir=0;
                  if ((sd->target2_dir & (INF_DIRECTIVE_RISING|INF_DIRECTIVE_FALLING))==0 || (sd->target2_dir & INF_DIRECTIVE_RISING)!=0) dir|=TTV_NODE_UP;
                  if ((sd->target2_dir & (INF_DIRECTIVE_RISING|INF_DIRECTIVE_FALLING))==0 || (sd->target2_dir & INF_DIRECTIVE_FALLING)!=0) dir|=TTV_NODE_DOWN;
                }

              ttv_search_mode(1, TTV_MORE_OPTIONS_DONT_FILTER_ENDING_NODE);
              chainsig=stb_debug_retreive_delays(stbfig, sig, outdir, (flags & STB_DEBUG_DIRECTIVE_DATA_IS_CLOCK)!=0?1:2, falseaccessmode, computeflags);
              ttv_search_mode(0, TTV_MORE_OPTIONS_DONT_FILTER_ENDING_NODE);
              
              specin='N';
              if((node->EVENT->ROOT->TYPE & TTV_SIG_CT) != TTV_SIG_CT)
                {
                  if(node->SPECIN != NULL)
                    chainsig = addchain(chainsig,sig), specin='Y' ;
                }

              for(chain = chainsig ; chain != NULL ; chain = chain->NEXT)
                {         
                  long crossmin, crossmax;
                  ptsig = (ttvsig_list *)chain->DATA ;
                  ptsig->TYPE &= ~TTV_SIG_MARQUE;

                  if ((sd->target1_dir & INF_DIRECTIVE_CLOCK)!=0 && getptype(ptsig->USER, TTV_SIG_CLOCK)==NULL) continue;

                  for (diff=0; diff<2; diff++)
                    {
                      node = stb_getstbnode(&ptsig->NODE[diff]);
                      sig1event=&ptsig->NODE[diff];

                      if (node->CK==NULL) doonce=1; else doonce=0;

                      for (run_inputck=node->CK; doonce || run_inputck!=NULL; run_inputck=run_inputck->NEXT)
                        {
                          inputck=run_inputck;
                          if (stb_checkvalidcommand(sig1event, inputck))
                            {
                              if (!((flags & STB_DEBUG_DIRECTIVE_DELAYMODE)!=0 && stb_getfromminmax(stbfig->FIG, inputck, node, sd->from, &startmin, &startmax, dir)==0))
                                  
                                {
                                  ttvevent_list *accesscmd, *accessclock;
                 
                                  if(inputck == NULL)
                                    {
                                      for (i=0; i<node->NBINDEX; i++)
                                        if (node->STBTAB[i]!=NULL)
                                          {
                                            inputck=stb_getclock(stbfig, i, NULL, &ckedge, NULL);
                                            break;
                                          }
                                    }
                                  // IIII
                                  for( j=0; j<2; j++)
                                    for (i=0; i<node->NBINDEX; i++)
                                      {
                                        STBHZ[j][i]=NULL, STBTAB[j][i]=NULL;
                                      }

                                  nodex = stb_getstbnode(&ptsig->NODE[diff]);
                                  if (getptype(ptsig->USER, STB_IS_CLOCK)!=NULL)
                                    create_clock_stability(nodex, STBTAB[diff]);
                                  else
                                    stb_transferstbline (stbfig, nodex->EVENT, nodex , STBTAB[diff], STBHZ[diff], 0, 1, NULL, inputck, 1, (nodex->FLAG & STB_NODE_STABCORRECT)!=0?STB_TRANSFERT_CORRECTION:0) ;

                                  // OOOO
                                  for(i = 0 ; i < (int)stbfig->PHASENUMBER ; i++)
                                    {
                                      phaseused[i]=0;
                                      tabpair[i] = NULL ;
                                      tabhz[i] = NULL ;
                                    }

                                  i = 2 ;
                 
                                  transparence=0; has_multicycle=0;
                 
                                  for(j = 0 ; j < 2 ; j++)
                                    {
                                      EventPeriodeHold[j].total=EventPeriodeHold[j].period=0;
                                      for (k=0; k<4; k++) involved_input_transistion[j][k]=0;
                                      useddelay[j]=NULL;
                                      falseslack_setup[j]=falseslack_hold[j]=0;
                                    }
                                  for (k=0; k<4; k++)
                                    for (j=0; j<node->NBINDEX; j++)
                                      datamoved[k][j]=0;
                 
                                  i=diff+i;
                     
                                  ptevent = ptsig->NODE + (i % 2) ;
                                  pSTBTAB=STBTAB[i %2];
                                  pSTBHZ=STBHZ[i %2];
                                  j=outdir;

                                  event = sig->NODE + j ;
                                  node = stb_getstbnode(event) ;
                                  if (inputck!=NULL && inputck->CMD!=NULL && falseaccessmode)
                                    accesscmd=inputck->CMD, accessclock=inputck->ORIGINAL_CLOCK;
                                  delay = stb_getstbdelay(event,ptevent,NULL,(long)0,accesscmd,accessclock) ;
                                  datamoved_start=0;
                                  if(delay != NULL)
                                    {
                                      if(delay->DELAYMAX == TTV_NOTIME)
                                        delaymax = delay->DELAYMIN ;
                                      else
                                        delaymax = delay->DELAYMAX ;
                             
                                      if(delay->DELAYMIN == TTV_NOTIME)
                                        delaymin = delay->DELAYMAX ;
                                      else
                                        delaymin = delay->DELAYMIN ;
                             
                             
                                      if(delaymax < delaymin)
                                        {
                                          delaym = delaymin ;
                                          delaymin = delaymax ;
                                          delaymax = delaym ;
                                        }
                                    }
                                  else
                                    {
                                      delaymin = delaymax = 0 ;
                                    }

                                  // falseslack check
                                  if (delay!=NULL && ifl!=NULL && ifl->LOADED.INF_FALSESLACK!=NULL && (flags & STB_DEBUG_DIRECTIVE_DELAYMODE)==0)
                                    {
                                      ttvevent_list *startclock, *endclocksetup, *endclockhold;
                                      int fs=0, fh=0;
                             
                                      stb_getslacktopclocks(directiveclock, node, 0, inputck, NULL, &startclock, &endclocksetup, &endclockhold);
                                      if (startclock!=NULL && endclocksetup!=NULL)
                                        fs=stb_isfalseslack(stbfig, startclock, ptevent, event, endclocksetup, INF_FALSESLACK_SETUP);
                                      
                                      if (startclock!=NULL && endclockhold!=NULL)
                                        fh=stb_isfalseslack(stbfig, startclock, ptevent, event, endclocksetup, INF_FALSESLACK_HOLD);
                                      if ((i<2 && (fs & INF_FALSESLACK_HZ)!=0) || (i>=2 && (fs & INF_FALSESLACK_NOTHZ)!=0)) falseslack_setup[outdir]=1;
                                      if ((i<2 && (fh & INF_FALSESLACK_HZ)!=0) || (i>=2 && (fh & INF_FALSESLACK_NOTHZ)!=0)) falseslack_hold[outdir]=1;
                                      if (falseslack_setup[outdir] && falseslack_hold[outdir])
                                        delay=NULL;
/*ttvevent_list *startclock, *endclock;
                                      int res;
                                      stbnode *directiveclocknode;
                                      if (!(inputck!=NULL && stb_getclocknode(stbfig, inputck->CKINDEX, NULL, &startclock, inputck)!=NULL)) startclock=NULL;
                               
                                      if (directiveclock) directiveclocknode=stb_getstbnode(directiveclock); else directiveclocknode=NULL;
                                      if (!(directiveclocknode!=NULL && directiveclocknode->CK!=NULL && stb_getclocknode(stbfig, stb_directive_getphase(directiveclocknode), NULL, &endclock, node->CK)!=NULL)) endclock=NULL;
                                      res=stb_isfalseslack(stbfig, startclock, ptevent, event, endclock);
                                      if ((i<2 && (res & INF_FALSESLACK_HZ)!=0) || (i>=2 && (res & INF_FALSESLACK_NOTHZ)!=0))
                                        delay=NULL;*/
                                    }
                                  if(delay != NULL || (specin=='Y' && ptevent==node->EVENT))
                                    {
                                      nodex = stb_getstbnode(ptevent) ;
                                      input_periode=0;
                                      if (inputck!=NULL)
                                        {
                                          ck=stb_getclock(stbfig, inputck->CKINDEX, NULL, &ckedge, NULL);
                                          if (ck!=NULL) input_periode=ck->PERIOD;
                                        }
                                      else if (inputck!=NULL) input_periode=inputck->PERIOD;
                             
                                      periode=0;
                                      for(u = 0 ; u < node->NBINDEX ; u++)
                                        {
                                          v=u;

                                          output_periode=0;
                                          if (directiveclock!=NULL && stb_getstbnode(directiveclock)->CK!=NULL)
                                            {
                                              ck=stb_getclock(stbfig, stb_directive_getphase(stb_getstbnode(directiveclock)), NULL, &ckedge, NULL);
                                              if (ck!=NULL) output_periode=ck->PERIOD;
                                            }
            
                                          stb_getmulticycleperiod(nodex->EVENT, node->EVENT, input_periode, output_periode, &periodeSetup, &periodeHold, &EventPeriodeHold[j].nb_setup_cycle, &EventPeriodeHold[j].nb_hold_cycle);
                                          EventPeriodeHold[j].mc_setup_period=periodeSetup;
                                          EventPeriodeHold[j].mc_hold_period=periodeHold;

                                          periodemax[(int)u] = (long)0 ;
                                          periodemin[(int)u] = (long)0 ;
                                          if(delay != NULL)
                                            {
                                              if(delay->PHASELISTMAX != NULL)
                                                periodemax[(int)u] = stb_calperiode(stbfig,u,node, delay->PHASELISTMAX,v,periode) ;
                                              if(delay->PHASELISTMIN != NULL)
                                                periodemin[(int)u] = stb_calperiode(stbfig,u,node, delay->PHASELISTMIN,v,periode) ;
                                              if (periodemin[(int)u]!=periodemax[(int)u])
                                                periodemin[(int)u]=periodemax[(int)u];
                                            }

                                          periodemax[(int)u]+=periodeSetup;
                                          periodemin[(int)u]+=periodeSetup;

                                          ptstbpair = stb_dupstbpairlist(*(pSTBTAB + u)) ;

                                          stb_assign_phase_to_stbpair(ptstbpair, u);

                                          if (delay!=NULL)
                                            {
                                              crossmin=stb_getcross_PP(delay->STARTMIN_PP, u);
                                              crossmax=stb_getcross_PP(delay->STARTMAX_PP, u);
                                              ptstbpair=stb_clippair(ptstbpair, crossmin, crossmax);
                                              if(( crossmin != TTV_NOTIME || crossmax!=TTV_NOTIME) && (ptstbpair != NULL))
                                                useddelay[j]=delay;
                                            }
                                          if((periodemax[(int)u] + periode) > 0 && ptstbpair!=NULL)
                                            datamoved[datamoved_start+j][(int)u]=-(periodemax[(int)u] + periode) ;
                                          tabpair[(int)v] = stb_transferstbpair(ptstbpair,tabpair[(int)v],
                                                                                delaymax-periodemax[(int)u],
                                                                                delaymin-periodemin[(int)u],
                                                                                periode, periode) ;
                                          if (periodeSetup>0) has_multicycle=1;
                                          if (periodeHold>=EventPeriodeHold[j].total) 
                                            EventPeriodeHold[j].total=periodeHold, EventPeriodeHold[j].period=output_periode;
                                          if (ptstbpair!=NULL)
                                            {
                                              phaseused[(int)u]=1;
                                              involved_input_transistion[j][i]=1;
                                            }
                                        }
                                    }

                                  stb_compute_common_clock_gap(ifl, stbfig, ptsig, inputck, node, involved_input_transistion[outdir], STBTAB, tabpair, tabhz, datamoved, &eshcd[outdir], directiveclock, flags, geneclockinfo);
                                  worstsetup=worsthold=STB_NO_TIME;
                              
                                  for (i=0; i<other_node->NBINDEX; i++)
                                    {
                                      for (rfd=0; rfd<=256; rfd+=256)
                                        {
                                          if ((flags & STB_DEBUG_DIRECTIVE_DELAYMODE)!=0 && (rfd!=0 || tabpair[i]==NULL)) continue;
                                          for (id=0; id<other_node->NBINDEX; id++)
                                            {                              
                                              if ((flags & STB_DEBUG_DIRECTIVE_DELAYMODE)!=0 && id!=i) continue;
                                              if (tabpair[i]!=NULL && (STBTAB_DEST[rfd+id]!=NULL || (flags & STB_DEBUG_DIRECTIVE_DELAYMODE)!=0))
                                                {
                                                  ptdomain = stb_getstbdomain(stbfig->CKDOMAIN, id) ;
                                                  if ((flags & STB_DEBUG_DIRECTIVE_DELAYMODE)!=0 || ptdomain==NULL || (i>=ptdomain->CKMIN && i<=ptdomain->CKMAX))
                                                    {
                                                      period=stb_getperiod(stbfig, id);
                                                      if ((flags & STB_DEBUG_DIRECTIVE_DELAYMODE)==0 && stb_cmpphase(stbfig, i, id)>0/*i>id*/)
                                                        {
                                                          //                                                mperiod=period;
                                                          mperiod= stb_synchronize_slopes(stbfig, i, id, period, 0);
                                                        }
                                                      else 
                                                        mperiod=0;
                                      
                                                      if ((flags & STB_DEBUG_DIRECTIVE_DELAYMODE)==0)
                                                        {
                                                          clockmin=STBTAB_DEST[rfd+id]->D+mperiod;
                                                          clockmax=STBTAB_DEST[rfd+id]->U+mperiod;
                                                          marginv=sd->margin;
                                                          mode_period=period;
                                                        }
                                                      else
                                                        {
                                                          if (startmin!=STB_NO_TIME)
                                                            {
                                                              clockmin=startmax+sd->margin;
                                                              clockmax=startmin+sd->margin;
                                                            }
                                                          else
                                                            {
                                                              clockmin=STBTAB[diff][i]->U+sd->margin;
                                                              clockmax=STBTAB[diff][i]->D+sd->margin;
                                                            }
                                                          marginv=0;
                                                          mode_period=0;
                                                        }
                                                      datamin=tabpair[i]->D;
                                                      datamax=tabpair[i]->U;
                                      
                                                      if (sd->operation==0)
                                                        {
                                                          setup=(clockmin+eshcd[outdir].setup_clock_gap[1].clock_margin-marginv-eshcd[outdir].setup_clock_gap[1].clock_uncertainty)-(datamax+eshcd[outdir].setup_clock_gap[1].data_margin)+eshcd[outdir].setup_clock_gap[1].value;
                                                          hold=(datamin+eshcd[outdir].hold_clock_gap[1].data_margin)+EventPeriodeHold[outdir].total-(clockmax+eshcd[outdir].hold_clock_gap[1].clock_margin+marginv-mode_period+eshcd[outdir].hold_clock_gap[1].clock_uncertainty)+eshcd[outdir].hold_clock_gap[1].value;
                                                        }
                                                      else if (sd->operation==INF_DIRECTIVE_AFTER)
                                                        {
                                                          setup=STB_NO_TIME;
                                                          hold=(datamin+eshcd[outdir].hold_clock_gap[1].data_margin)+EventPeriodeHold[outdir].total-(clockmax+eshcd[outdir].hold_clock_gap[1].clock_margin+marginv-mode_period+eshcd[outdir].hold_clock_gap[1].clock_uncertainty)+eshcd[outdir].hold_clock_gap[1].value;
                                                        }
                                                      else if (sd->operation==INF_DIRECTIVE_BEFORE)
                                                        {
                                                          setup=(clockmin+eshcd[outdir].setup_clock_gap[1].clock_margin-marginv-eshcd[outdir].setup_clock_gap[1].clock_uncertainty)-(datamax+eshcd[outdir].setup_clock_gap[1].data_margin)+eshcd[outdir].setup_clock_gap[1].value;
                                                          hold=STB_NO_TIME;
                                                        }
                                      
                                                      if (setup!=STB_NO_TIME && (worstsetup==STB_NO_TIME || setup<worstsetup))
                                                        {
                                                          if (flags & STB_DEBUG_DIRECTIVE_CLOCK_IS_DATA)
                                                            {
                                                              if (rfd<256) flagmin=STB_DEBUG_DIRECTIVE_FALLING;
                                                              else flagmin=STB_DEBUG_DIRECTIVE_RISING;
                                                            }
                                                          stb_setdetail(&thisdetail->setup, setup, datamax-mperiod, clockmin-mperiod, marginv, 0, 0, &eshcd[outdir].setup_clock_gap[1], flags|flagmin, i, 0);
                                                          stb_assign_operations_low(&thisdetail->setup, -mperiod, &EventPeriodeHold[outdir]);
                                                          worstsetup=setup;
                                                        }
                                                      if (hold!=STB_NO_TIME && (worsthold==STB_NO_TIME || hold<worsthold))
                                                        {
                                                          if (flags & STB_DEBUG_DIRECTIVE_CLOCK_IS_DATA)
                                                            {
                                                              if (rfd<256) flagmax=STB_DEBUG_DIRECTIVE_FALLING;
                                                              else flagmax=STB_DEBUG_DIRECTIVE_RISING;
                                                            }
                                                          stb_setdetail(&thisdetail->hold, hold, datamin-mperiod, clockmax-cmperiod-mperiod, marginv, mode_period, EventPeriodeHold[outdir].total, &eshcd[outdir].hold_clock_gap[1], flags|flagmax, i, cmperiod);
                                                          stb_assign_operations_low(&thisdetail->hold, -mperiod, &EventPeriodeHold[outdir]);
                                                          worsthold=hold;
                                                        }
                                                    }
                                                }
                                            }
                                          if ((flags & STB_DEBUG_DIRECTIVE_DELAYMODE)!=0) break;
        
                                        }  
                                    }

                                  stb_freestbtabpair(tabpair,other_node->NBINDEX) ;
                         
                                  setup=worstsetup;
                                  hold=worsthold;
                                  thisdetail->setup.VALUE=setup;
                                  thisdetail->hold.VALUE=hold;
                                  //stb_assign_operations(outdir, detail, datamoved, EventPeriodeHold);
                                  stb_add_infos(stbfig, event, outdir, detail, inputck, sig1event, useddelay);

                                  if (falseslack_setup[outdir]) setup=thisdetail->setup.VALUE=STB_NO_TIME;
                                  if (falseslack_hold[outdir]) hold=thisdetail->hold.VALUE=STB_NO_TIME;

                                  if (computeflags & STB_UPDATE_SETUP_HOLD)
                                    {
                                      if ((node->FLAG & STB_NODE_NOSETUP)==0 && setup!=STB_NO_TIME && (node->SETUP==STB_NO_TIME || node->SETUP>setup)) node->SETUP=setup;
                                      if ((node->FLAG & STB_NODE_NOHOLD)==0 && hold!=STB_NO_TIME && (node->HOLD==STB_NO_TIME || node->HOLD>hold)) node->HOLD=hold;
                                    }
                                  if ((node->FLAG & STB_NODE_NOSETUP)!=0 || (computeflags & STB_COMPUTE_HOLD_ONLY)!=0) setup=STB_NO_TIME;
                                  if ((node->FLAG & STB_NODE_NOHOLD)!=0 || (computeflags & STB_COMPUTE_SETUP_ONLY)!=0) hold=STB_NO_TIME;              

                                  if((setup!=STB_NO_TIME && setup <= margin) || (hold!=STB_NO_TIME && hold <= margin))
                                    {
                                      debug = stb_addstbdebug(debug,ptsig,sig1event,sig,oedge.PERIOD,NULL,NULL,sd->target2?&sd->target2->NODE[0]:NULL,
                                                              STB_NO_TIME,
                                                              STB_NO_TIME,
                                                              STB_NO_TIME,
                                                              STB_NO_TIME,

                                                              oedge.SUPMIN,
                                                              oedge.SUPMAX,
                                                              oedge.SDNMIN,
                                                              oedge.SDNMAX,

                                                              edge.SUPMIN,
                                                              edge.SUPMAX,
                                                              edge.SDNMIN,
                                                              edge.SDNMAX,

                                                              edge.PERIOD,
                                                              sd->margin,sd->margin,setup,hold,has_multicycle?EventPeriodeHold[outdir].total+EventPeriodeHold[outdir].period:0,
                                                              0,0,
                                                              detail,
                                                              inputck==NULL || doonce?NULL:inputck->CMD,
                                                              inputck==NULL?NULL:inputck->ORIGINAL_CLOCK
                                                              ) ;
                                    }
                                  for (j=0; j<2; j++)
                                    {
                                      stb_freestbtabpair(STBTAB[j], other_node->NBINDEX);
                                    }
                                }
                            }

                          if (doonce) break;
                        }
                    }
                }
              freechain(chainsig);
              stb_delstbdelay(&sig->NODE[outdir]) ;

              if ((flags & STB_DEBUG_DIRECTIVE_DELAYMODE)==0)
                freelongstab(STBTAB_DEST, other_node->NBINDEX);
            }
          sd=sd->next;
        }
    }
  return debug;
}

int stb_get_filter_directive_info(stbfig_list *sf, ttvevent_list *tve, char phase, long *start, long *startmax, long *end, long *endmin, char *state, long *move, ttvevent_list **clockedsig, stbck *origedge)
{
  ttv_directive *sd;
  stbck edge, oedge;
  stbnode *other_node, *other_node0;
  long clockmax=TTV_DELAY_MAX, clockmin=TTV_DELAY_MAX, clockmin_max=TTV_DELAY_MAX, clockmax_min=TTV_DELAY_MAX;
  int idx;
  *move=0;

  if ((sd=ttv_get_directive(tve->ROOT))!=NULL)
    {
      while (sd!=NULL)
        {
          if (sd->filter)
            {              
              if (sd->target1_dir==0
                  || ((sd->target1_dir & INF_DIRECTIVE_RISING)!=0 && (tve->TYPE & TTV_NODE_UP)!=0)
                  || ((sd->target1_dir & INF_DIRECTIVE_FALLING)!=0 && (tve->TYPE & TTV_NODE_DOWN)!=0))
                {
                  if ((sd->target2_dir & INF_DIRECTIVE_UP)!=0) idx=1; else idx=0;
                  other_node=stb_getstbnode(&sd->target2->NODE[idx]);
                  other_node0=stb_getstbnode(ttv_opposite_event(&sd->target2->NODE[idx]));
                  if (other_node->CK!=NULL && other_node0->CK!=NULL) 
                  {
                    if (clockedsig!=NULL) *clockedsig=&sd->target2->NODE[idx];
                    
                    stb_sd_getedge(other_node, &oedge);
                    if (origedge!=NULL) memcpy(origedge, &oedge, sizeof(oedge));
                    if (oedge.PERIOD!=STB_NO_TIME)
                      {
                        if ((sd->target2_dir & INF_DIRECTIVE_UP)!=0)
                          {
                            if (oedge.SDNMAX<oedge.SUPMAX) oedge.SDNMAX+=oedge.PERIOD, oedge.SDNMIN+=oedge.PERIOD;
                            clockmin=oedge.SUPMIN;
                            clockmin_max=oedge.SUPMAX;
                            clockmax=oedge.SDNMAX;
                            clockmax_min=oedge.SDNMIN;
                            *state='u';
                          }
                        else
                          {
                            if (oedge.SUPMAX<oedge.SDNMAX) oedge.SUPMAX+=oedge.PERIOD, oedge.SUPMIN+=oedge.PERIOD;
                            clockmin=oedge.SDNMIN;
                            clockmin_max=oedge.SDNMAX;
                            clockmax=oedge.SUPMAX;
                            clockmax_min=oedge.SUPMIN;
                            *state='d';
                          }
                        if (phase!=STB_NO_INDEX && stb_isafter(sf, other_node, phase)==0) 
                          {
                            *move=stb_synchronize_slopes(sf, phase, stb_directive_getphase(other_node), oedge.PERIOD, 0);
                            clockmin+=*move, clockmin_max+=*move;
                            clockmax+=*move, clockmax_min+=*move;

                          }
                        break;
                      }
                  }
                }
            }
          sd=sd->next;
        }
    }
  *start=clockmin;
  *startmax=clockmin_max;
  *end=clockmax;
  *endmin=clockmax_min;
  if (clockmax!=TTV_DELAY_MAX) return 1;
  return 0;
}
/*
  long stb_getfilterlag(long abs_time, ttvevent_list *tve, long type)
  {
  long startmin, startmax, endmin, endmax;
  if (stb_get_filter_directive_info(tve,  &startmin, &startmax, &endmax, &endmin))
  {
  if (type & TTV_FIND_MIN)
  {
  return startmin-abs_time;
  }
  else
  {
  if (abs_time<startmax) return startmax-abs_time;
  if (abs_time>endmax) return startmax-abs_time;
  return endmax-abs_time;
  }
  }
  else
  return 0;
  }
*/
int stb_has_filter_directive(ttvevent_list *tve)
{
  ttv_directive *sd;
  stbnode *other_node, *other_node0;

  if ((sd=ttv_get_directive(tve->ROOT))!=NULL)
    {
      while (sd!=NULL)
        {
          if (sd->filter)
          {
            other_node=stb_getstbnode(&sd->target2->NODE[0]);
            other_node0=stb_getstbnode(&sd->target2->NODE[1]);
            if (other_node->CK!=NULL && other_node0->CK!=NULL) 
               return 1;
          }
          sd=sd->next;
        }
    }
  return 0;
}

long stb_get_directive_setup_slope(stbfig_list *stbfig, stbnode *node, char *phase, long *period, int filter)
{
  ttv_directive *sd;
  long clockmin;
  stbck oedge;
  stbnode *other_node;
  int stab;

  if ((sd=ttv_get_directive(node->EVENT->ROOT))!=NULL)
    {
      while (sd!=NULL)
        {
          if (sd->filter==filter)
            {              
              oedge.SUPMIN=oedge.SUPMAX=oedge.SDNMIN=oedge.SDNMAX=STB_NO_TIME;
              stab=0;
              if ((sd->target1_dir & INF_DIRECTIVE_CLOCK)==0 && (sd->target2_dir & INF_DIRECTIVE_CLOCK)!=0)
                {

                  other_node=stb_getstbnode(&sd->target2->NODE[0]);

                  stb_sd_getedge(other_node, &oedge);
                  if (oedge.PERIOD==STB_NO_TIME || (((sd->target1_dir & INF_DIRECTIVE_RISING)!=0 && (node->EVENT->TYPE & TTV_NODE_UP)!=0)
                                                    || ((sd->target1_dir & INF_DIRECTIVE_FALLING)!=0 && (node->EVENT->TYPE & TTV_NODE_DOWN)!=0)))
                    { sd=sd->next; continue; }
                  *period=oedge.PERIOD;
                  if ((sd->target2_dir & INF_DIRECTIVE_RISING)!=0)
                    {
                      clockmin=oedge.SUPMIN;
                      *phase=stb_directive_getphase(stb_getstbnode(&sd->target2->NODE[1]));
                    }
                  else if ((sd->target2_dir & INF_DIRECTIVE_FALLING)!=0)
                    {
                      clockmin=oedge.SDNMIN;
                      *phase=stb_directive_getphase(stb_getstbnode(&sd->target2->NODE[0]));
                    }
                  else
                    {
                      if ((sd->target2_dir & INF_DIRECTIVE_UP)!=0)
                        {
                          clockmin=oedge.SUPMIN;
                          *phase=stb_directive_getphase(stb_getstbnode(&sd->target2->NODE[1]));
                        }
                      else
                        {
                          clockmin=oedge.SDNMIN;
                          *phase=stb_directive_getphase(stb_getstbnode(&sd->target2->NODE[0]));
                        }
                    }
                  if ((node->FLAG & STB_NODE_NOSETUP)==0) return clockmin-sd->margin;
                }
            }
          sd=sd->next;
        }
    }
  return TTV_NOTIME;
}


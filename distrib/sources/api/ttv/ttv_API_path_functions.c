#include STM_H
#include TTV_H
#include MLU_H
#include MUT_H
#include INF_H
#include EFG_H
#include TAS_H
#include TRC_H
#include YAG_H
#include MCC_H
#include STB_H

#include AVT_H
#define API_USE_REAL_TYPES
#include "ttv_API_LOCAL.h"
#include "ttv_API.h"
#include "ttv_API_display.h"
#include "ttv_API_util.h"


static void recur_run(ttvfig_list *tvf, long slopein, ttvevent_list *a, chain_list *nextnodelist, chain_list **lst, chain_list **config, char dirend, long minmax, chain_list **allsearch)
{
  ttvpath_list *path_list, *pth;
  ttvsig_list *tvsend, *tvs2;
  long slopeout, findcmd=0, savelastslope;
  char dir[3]=" ?";
  int doaccess;
  chain_list *cl, *savenodelist, *ch;

  dir[0]=ttv_GetTimingEventDirection(a);

  if (nextnodelist!=NULL)
    {
      savelastslope=TTV_PROPAGATE_FORCED_SLOPE;
      if (slopein!=0) TTV_PROPAGATE_FORCED_SLOPE=slopein;
     
      cl=(chain_list *)nextnodelist->DATA;
      nextnodelist=nextnodelist->NEXT;
      if (cl!=NULL && cl->DATA==NULL) doaccess=1, cl=cl->NEXT;
      else doaccess=0;

      while (cl!=NULL)
        {
          tvsend=(ttvsig_list *)cl->DATA;

          // un access ?
          if (doaccess && nextnodelist!=NULL && (tvsend->TYPE & (TTV_SIG_L|TTV_SIG_B))!=0)
            {
              savenodelist=nextnodelist;
              ch=(chain_list *)nextnodelist->DATA;
              nextnodelist=nextnodelist->NEXT;
              if (ch!=NULL && ch->DATA==NULL) ch=ch->NEXT;

              while (ch!=NULL)
                {
                  tvs2=(ttvsig_list *)ch->DATA;
                  
                  if((a->ROOT->TYPE & (TTV_SIG_C | TTV_SIG_L | TTV_SIG_R | TTV_SIG_B)) == 0)
                    {
                      a->ROOT->NODE->TYPE |= TTV_NODE_STOP ;
                      (a->ROOT->NODE+1)->TYPE |= TTV_NODE_STOP ;
                      if ((a->ROOT->TYPE & TTV_SIG_N) != 0)
                        TTV_MORE_SEARCH_OPTIONS|=TTV_MORE_OPTIONS_ENABLE_STOP_ON_TTV_SIG_N;
                    }
                  if ((a->ROOT->TYPE & TTV_SIG_Q) != 0)
                    findcmd=TTV_FIND_CMD;          
                  
                  // pour que les falsepaths s'appliquent
                  if ((tvs2->TYPE & (TTV_SIG_C | TTV_SIG_L | TTV_SIG_R | TTV_SIG_B)) == 0)
                    {        
                      tvs2->NODE->TYPE|=TTV_NODE_STOP ;
                      (tvs2->NODE+1)->TYPE|=TTV_NODE_STOP ;
                    }
                  
                  TTV_QUIET_MODE=1;
                  path_list=ttv_getsigaccesslist (tvf, tvsend, tvs2, a->ROOT, dir, 0, minmax|findcmd, 0, 0);
                  TTV_QUIET_MODE=0;
                  
                  if((a->ROOT->TYPE & (TTV_SIG_C | TTV_SIG_L | TTV_SIG_R | TTV_SIG_B)) == 0)
                    {
                      a->ROOT->NODE->TYPE &= ~TTV_NODE_STOP ;
                      (a->ROOT->NODE+1)->TYPE &= ~TTV_NODE_STOP ;
                      if ((a->ROOT->TYPE & TTV_SIG_N) != 0)
                        TTV_MORE_SEARCH_OPTIONS&=~TTV_MORE_OPTIONS_ENABLE_STOP_ON_TTV_SIG_N;
                    }
                  
                  // pour que les falsepaths s'appliquent
                  if ((tvs2->TYPE & (TTV_SIG_C | TTV_SIG_L | TTV_SIG_R | TTV_SIG_B)) == 0)
                    {  
                      tvs2->NODE->TYPE&=~TTV_NODE_STOP ;
                      (tvs2->NODE+1)->TYPE&=~TTV_NODE_STOP ;
                    }
                  
                  
                  if (path_list!=NULL)
                    {
                      *allsearch=addchain(*allsearch, path_list);
                      
                      for (pth=path_list; pth!=NULL; pth=pth->NEXT)
                        {
                          if (!(nextnodelist!=NULL && ttv_PathIsHZ(pth)))
                            {
                              *lst=addchain(*lst, pth);
                              
                              slopeout=pth->SLOPE;
                              recur_run(tvf, slopeout, pth->ROOT, nextnodelist, lst, config, dirend, minmax, allsearch);
                              
                              *lst=delchain(*lst, *lst);
                            }
                        }
                    }
                  ch=ch->NEXT;
                }
              nextnodelist=savenodelist;
            }

          if (!doaccess)
            {
              // un path?
              if((a->ROOT->TYPE & (TTV_SIG_C | TTV_SIG_L | TTV_SIG_R | TTV_SIG_B)) == 0)
                {
                  a->ROOT->NODE->TYPE |= TTV_NODE_STOP ;
                  (a->ROOT->NODE+1)->TYPE |= TTV_NODE_STOP ;
                  if ((a->ROOT->TYPE & TTV_SIG_N) != 0)
                    TTV_MORE_SEARCH_OPTIONS|=TTV_MORE_OPTIONS_ENABLE_STOP_ON_TTV_SIG_N;
                }
              if ((a->ROOT->TYPE & TTV_SIG_Q) != 0)
                findcmd=TTV_FIND_CMD;          
              
              // pour que les falsepaths s'appliquent
              if ((tvsend->TYPE & (TTV_SIG_C | TTV_SIG_L | TTV_SIG_R | TTV_SIG_B)) == 0)
                {        
                  tvsend->NODE->TYPE|=TTV_NODE_STOP ;
                  (tvsend->NODE+1)->TYPE|=TTV_NODE_STOP ;
                }
              
              path_list=ttv_getcritic_pathlist (tvf, a->ROOT, tvsend, dir, 0, minmax|findcmd, 0, 0, 0);
              
              if((a->ROOT->TYPE & (TTV_SIG_C | TTV_SIG_L | TTV_SIG_R | TTV_SIG_B)) == 0)
                {
                  a->ROOT->NODE->TYPE &= ~TTV_NODE_STOP ;
                  (a->ROOT->NODE+1)->TYPE &= ~TTV_NODE_STOP ;
                  if ((a->ROOT->TYPE & TTV_SIG_N) != 0)
                    TTV_MORE_SEARCH_OPTIONS&=~TTV_MORE_OPTIONS_ENABLE_STOP_ON_TTV_SIG_N;
                }
              
              // pour que les falsepaths s'appliquent
              if ((tvsend->TYPE & (TTV_SIG_C | TTV_SIG_L | TTV_SIG_R | TTV_SIG_B)) == 0)
                {  
                  tvsend->NODE->TYPE&=~TTV_NODE_STOP ;
                  (tvsend->NODE+1)->TYPE&=~TTV_NODE_STOP ;
                }
          
              if (path_list!=NULL)
                {
                  *allsearch=addchain(*allsearch, path_list);
                  
                  for (pth=path_list; pth!=NULL; pth=pth->NEXT)
                    {
                      if (!(nextnodelist!=NULL && ttv_PathIsHZ(pth)))
                        {
                          *lst=addchain(*lst, pth);
                          
                          slopeout=pth->SLOPE;
                          recur_run(tvf, slopeout, pth->ROOT, nextnodelist, lst, config, dirend, minmax, allsearch);
                          
                          *lst=delchain(*lst, *lst);
                        }
                    }
                }
            }
          cl=cl->NEXT;
        }
      TTV_PROPAGATE_FORCED_SLOPE=savelastslope;
    }  
  else
    {
      if (dirend=='?' || dirend==dir[0])
        *config=addchain(*config, reverse(dupchainlst(*lst)));
    }
}

// renvoie le front d'ouverture le plus tard ou tot selon le type de recherche
static ttvevent_list *getgoodclockevent(stbfig_list *stbfig, ttvevent_list *latch, long type)
{
  long lastopen, open, move;
  ttvevent_list *tve=NULL, *cmd;
  stbck *ck;
  stbnode *node;
  chain_list *cmds=NULL, *cl;

  node=stb_getstbnode(latch);
  if (node!=NULL)
    {
      // stb_getlatchslope modifie la structure donc on garde une liste a part
      for (ck=node->CK; ck!=NULL; ck=ck->NEXT) cmds=addchain(cmds, ck->CMD);

      for (cl=cmds; cl!=NULL; cl=cl->NEXT)
        {
          cmd=(ttvevent_list *)cl->DATA;
          if (cmd!=NULL)
            {
              if ((type & TTV_FIND_MAX)==TTV_FIND_MAX)
                {
                  stb_getlatchslope(stbfig,latch,cmd,&open,NULL,NULL,NULL,NULL,1,STB_NO_INDEX,&move);
                  if (tve==NULL || lastopen<open) lastopen=open, tve=cmd;
                }
              else
                {
                  stb_getlatchslope(stbfig,latch,cmd,NULL,&open,NULL,NULL,NULL,1,STB_NO_INDEX,&move);
                  if (tve==NULL || open>lastopen) lastopen=open, tve=cmd;
                }
            }          
        }
      freechain(cmds);
    }
  return tve;
}

#define CLIPCTK 0x0fab1
#define CLIPREF 0x0fab2

static void performstbclipping(ttvfig_list *tvf, chain_list *head, long type)
{
  ttvpath_list *tph;
  long lastopen=TTV_NOTIME, lastclose;
  long openmax, openmin, closemax, closemin, period, deltaref, deltactk, open, close, move;
  long timeref=0, timectk=0, err;
  chain_list *cl;
  ttvevent_list *tve, *newtve;
  stbfig_list *stbfig=NULL;
  ptype_list *pt;

  if ((stbfig=stb_getstbfig(tvf))==NULL) return;

  for (cl=head; cl!=NULL && cl->NEXT!=NULL; cl=cl->NEXT)
    {
      tph=(ttvpath_list *)cl->DATA;
      timectk+=tph->DELAY;
      timeref+=tph->REFDELAY;
      

      if ((tve=ttv_GetPathCommand(tph))!=NULL)
        {
          if ((newtve=getgoodclockevent(stbfig, tph->ROOT, type))!=NULL)
            tve=newtve;
          stb_getlatchslope(stbfig,tph->ROOT,tve,&openmax,&openmin,&closemax,&closemin,&period,1,STB_NO_INDEX,&move);
          if ((type & TTV_FIND_MAX)==0) openmax=openmin, closemax=closemin;

          if (lastopen==TTV_NOTIME)
            {
              timeref=timectk=0;
            }
          else
            {
              if (openmax<lastopen)
                {
                  open=openmax+period-lastopen, close=closemax+period-lastopen;
                }
              else
                {
                  open=openmax-lastopen, close=closemax-lastopen;
                }
              if (timeref<open) deltaref=open-timeref; else deltaref=0;
              if (timectk<open) deltactk=open-timectk; else deltactk=0;
              if (timeref>close || timectk>close)
                {
                  err=timeref-close;
                  if (timectk-close>err) err=timectk-close;
                  avt_errmsg(TTV_API_ERRMSG, "033", AVT_WARNING, ttv_GetFullSignalName_COND(tvf, tph->ROOT->ROOT), err/TTV_UNIT);
//                  avt_error("ttvapi", -1, AVT_WAR, "stability error found at node %s: data too late by %.1fps\n", ttv_GetFullSignalName_COND(tvf, tph->ROOT->ROOT), err/TTV_UNIT);
                  if (timeref>close) timeref=close;
                  if (timectk>close) timectk=close;
                }
              if ((pt=getptype(tph->USER, CLIPCTK))==NULL) pt=tph->USER=addptype(tph->USER, CLIPCTK, 0);
              pt->DATA=(void *)deltactk;
              if ((pt=getptype(tph->USER, CLIPREF))==NULL) pt=tph->USER=addptype(tph->USER, CLIPREF, 0);
              pt->DATA=(void *)deltaref;
              
              timeref=timeref+deltaref-open;
              timectk=timectk+deltactk-open;
            }

          lastopen=openmax;
          lastclose=closemax;
        }
    }
}

static long getdatadelta(ttvpath_list *pth, long type)
{
  ptype_list *pt;
  if ((pt=getptype(pth->USER, type))!=NULL)
    return (long)pt->DATA;
  else 
    return 0;
}
// retourne le delay total ref ou normal
static double getconfigdelay(chain_list *head, char type, double *slope)
{
  ttvpath_list *tph;
  double delay=0;
  chain_list *cl;
  for (cl=head; cl!=NULL; cl=cl->NEXT)
    {
      tph=(ttvpath_list *)cl->DATA;
      switch(type)
        {
        case 'n': delay+=ttv_GetPathDelay(tph)+getdatadelta(tph, CLIPCTK)*1e-12/TTV_UNIT; break;
        case 'r': delay+=ttv_GetPathRefDelay(tph)+getdatadelta(tph, CLIPREF)*1e-12/TTV_UNIT; break;
        }      
    }
  switch(type)
    {
    case 'n': *slope=ttv_GetPathSlope(tph); break;
    case 'r': *slope=ttv_GetPathRefSlope(tph); break;
    }
  return delay;
}

// recupere le min ou max des paths pour une config de l'e/s
static void removebadconfig(chain_list *headconfig, char dirstart, char dirend, long search)
{
  chain_list *lastconfig=NULL, *cl, *ch;
  double lastdelay, thisdelay, slope;
  ttvpath_list *tph, *lastfirstpath=NULL, *fpt;
  int max;

  if ((search & TTV_FIND_MAX)!=0) max=1; else max=0;

  for (cl=headconfig; cl!=NULL; cl=cl->NEXT)
    {
      if (cl->DATA!=NULL)
        {
          ch=(chain_list *)cl->DATA;
          fpt=tph=(ttvpath_list *)ch->DATA;
          if (ttv_GetPathStartDirection(tph)==dirstart)
            {
              while (ch->NEXT!=NULL) ch=ch->NEXT;
              tph=(ttvpath_list *)ch->DATA;
              if (ttv_GetPathEndDirection(tph)==dirend)
                {
                  thisdelay=getconfigdelay((chain_list *)cl->DATA, 'n', &slope);
                  if (lastconfig==NULL || (max &&  thisdelay>lastdelay) || (!max && thisdelay<lastdelay))
                    {
                      if (lastconfig!=NULL) { freechain((chain_list *)lastconfig->DATA); lastconfig->DATA=NULL; }
                      lastconfig=cl;
                      lastfirstpath=fpt;
                    }
                }
            }
        }
    }
}

// cree un path global pour une config
static ttvpath_list *buildconfigpath(chain_list *config, ttvpath_list *last)
{
  chain_list *ch;
  double delay, delayref, slope, sloperef;
  ttvpath_list *pt, *newpth, *ptend;

  ch=config;
  pt=(ttvpath_list *)ch->DATA;
  while (ch->NEXT!=NULL) ch=ch->NEXT;
  ptend=(ttvpath_list *)ch->DATA;

  delay=getconfigdelay(config, 'n', &slope);
  delayref=getconfigdelay(config, 'n', &sloperef);

  newpth=ttv_allocpath(last,
                       pt->FIG,
                       ptend->ROOT,
                       pt->NODE,
                       ptend->CMD,
                       pt->LATCH,
                       pt->CMDLATCH,
                       NULL, // latch list
                       pt->DATADELAY,
                       pt->ACCESS,
                       pt->REFACCESS,
                       pt->TYPE,
                       mbk_long_round(delayref*1e12*TTV_UNIT),
                       mbk_long_round(sloperef*1e12*TTV_UNIT),
                       mbk_long_round(delay*1e12*TTV_UNIT),
                       mbk_long_round(slope*1e12*TTV_UNIT),
                       pt->DELAYSTART,
                       pt->SLOPESTART,
                       NULL,
                       NULL,
                       pt->CROSSMINDELAY,
                       pt->PHASE,
                       pt->CLOCKPATHDELAY
                       ) ;

  return newpth;
}

// cree un detail global pour une config
static ttvcritic_list *buildconfigdetail(chain_list *config)
{
  chain_list *ch;
  ttvpath_list *pt;
  ttvcritic_list *tc, *newtc=NULL, *emptytc;

  for (ch=config; ch!=NULL; ch=ch->NEXT)
    {
      pt=(ttvpath_list *)ch->DATA;
      for (tc=pt->CRITIC; tc!=NULL; tc=tc->NEXT)
        {
          if (!(ch!=config && tc==pt->CRITIC))
            {
              emptytc=ttv_alloccritic(
                                      NULL,
                                      tc->FIG,
                                      NULL,
                                      pt->ROOT,
                                      0,
                                      0,
                                      0,
                                      0,
                                      0,
                                      0,
                                      0,
                                      NULL,
                                      NULL
                                      ) ;
              if ((emptytc->NODE_FLAG & TTV_NODE_FLAG_NOALLOC)==0)
              {
                mbkfree(emptytc->NAME);
                mbkfree(emptytc->NETNAME);
              }
              memcpy(emptytc, tc, sizeof(*tc));
              emptytc->USER=NULL;
              if (tc->NEXT==NULL)
                {
                  emptytc->DELAY+=getdatadelta(pt, CLIPCTK);
                  emptytc->REFDELAY+=getdatadelta(pt, CLIPREF);
                }
              if ((tc->NODE_FLAG & TTV_NODE_FLAG_NOALLOC)==0)
              {
                emptytc->NAME=mbkstrdup(tc->NAME);
                emptytc->NETNAME=mbkstrdup(tc->NETNAME);
              }
              emptytc->NEXT=newtc;
              newtc=emptytc;
            }
        }
    }
  return (ttvcritic_list *)reverse((chain_list *)newtc);
}

static chain_list *filterpath(chain_list *lst, int max)
{
  chain_list *cl;
  int i;

  if (max!=0)
    {
      for(cl=lst, i=1; i<max && cl!=NULL ; cl=cl->NEXT , i++) ;
      if(cl!=NULL)
        {
          ttv_FreePathList(cl->NEXT);
          freechain(cl->NEXT);
          cl->NEXT=NULL;
        }
    }
  return lst;
}

static void freedoublechain(chain_list *cl)
{
  while (cl!=NULL)
    {
      freechain((chain_list *)cl->DATA);
      cl=delchain(cl, cl);
    }
}

chain_list *ttv_ProbeDelay_sub(ttvfig_list *tvf, double slopein, chain_list *nodenamelist, char *dir, int nbpath, char *path_or_access, char *minmax, int nosync, int noprop)
{
  chain_list *cl, *mask, *nodelist, *ch, *allsearch=NULL, *ch0;
  ttvpath_list *path_list, *pth;
  ttvsig_list *tvs;
  long minmax_i=1;
  chain_list *config=NULL, *nlist;
  char *oldmode, d0, d1;
  int oldprop, path, cnt;
  long lastslope=mbk_long_round(slopein*1e12*TTV_UNIT);
  long oldexstart, oldexend, mode;

  API_TEST_TOKEN_SUB(TMA_API,"tma")

  if (nbpath<0) nbpath=0;

  if (!strcasecmp (minmax, "max")) minmax_i = TTV_FIND_MAX;
  else if (!strcasecmp (minmax, "min")) minmax_i = TTV_FIND_MIN;
  else
    {
      avt_errmsg(TTV_API_ERRMSG, "022", AVT_ERROR);
//    fprintf (stderr, "ttv_ProbeDelay: error: available values for 'minmax' are 'min' or 'max'\n");
      return NULL;
    }

  if (!strcasecmp (path_or_access, "path")) path=1;
  else if (!strcasecmp (path_or_access, "access")) path=0;
  else if (strcasecmp (path_or_access, "-")!=0)
    {
      avt_errmsg(TTV_API_ERRMSG, "024", AVT_ERROR);
//      fprintf (stderr, "ttv_ProbeDelay: error: available values for 'path_or_access' are 'path' or 'access'\n");
      return NULL;
    }
  else path=-1;

  if (path==1 && countchain(nodenamelist)<2)
    {
      avt_errmsg(TTV_API_ERRMSG, "035", AVT_ERROR, 2);
      //fprintf (stderr, "ttv_ProbeDelay: at least 2 node names are required for the function to run\n");
      return NULL;
    }

  if (path==0 && countchain(nodenamelist)<3)
    {
      avt_errmsg(TTV_API_ERRMSG, "035", AVT_ERROR, 3);
//      fprintf (stderr, "ttv_ProbeDelay: at least 3 node names are required for the function to run\n");
      return NULL;
    }

  if (tvf==NULL) return NULL;

//  ttv_checkinfchange(tvf);
  if ((ttv_getloadedfigtypes(tvf) & TTV_FILE_DTX)==TTV_FILE_DTX) mode=TTV_FILE_DTX;
  else mode=TTV_FILE_TTX;

  for (cl=nodenamelist, nodelist=NULL; cl!=NULL; cl=cl->NEXT)
    {
      char *var=(char *)cl->DATA;
      unsigned int i, access=0;
      const char *as="-access";
      char *pref[]=
        {
          "-signal=",
          "-latch=",
          "-prech=",
          "-con=",
          "-any=",
          "="
        };
      long stype=0;
      
      if (strncasecmp(var,as,strlen(as))==0) { var=&var[strlen(as)]; access=1; }
      if (path==0 && cl==nodenamelist->NEXT) access=1;
      if (cl==nodenamelist || cl->NEXT==NULL) access=0;
      
      for (i=0; i<sizeof(pref)/sizeof(*pref); i++)
        {
          if (strncasecmp(var,pref[i],strlen(pref[i]))==0) { var=&var[strlen(pref[i])]; break; }
        }
      if (i>=sizeof(pref)/sizeof(*pref) && access) var=(char *)cl->DATA;
      if (i>=sizeof(pref)/sizeof(*pref) || i==0 || i==4) stype=(access==0)?TTV_SIG_TYPEALL:TTV_SIG_L|TTV_SIG_R|TTV_SIG_B;
      if (i==1) stype=TTV_SIG_L;
      if (i==2) stype=TTV_SIG_R;
      if (i==3) stype=TTV_SIG_C;
      if ((i==5 || i>=sizeof(pref)/sizeof(*pref)) && cl!=nodenamelist && cl->NEXT!=NULL) stype=TTV_SIG_L;
            
      tvs=ttv_GetTimingSignal(tvf, var);
      if (tvs!=NULL)
        {
          if (i>=sizeof(pref)/sizeof(*pref) || (tvs->TYPE & stype)!=0) ch=addchain(NULL, tvs);
          else ch=NULL;
        }
      else
        {
          if (i>=sizeof(pref)/sizeof(*pref) && !mbk_isregex_name(var))
            stype=(access==0)?TTV_SIG_TYPEALL:TTV_SIG_L|TTV_SIG_R|TTV_SIG_B;

          ch=ttv_getsigbytype_and_netname(tvf,NULL,stype,mask=addchain(NULL, namealloc(var)));
          freechain(mask);
          nlist=NULL;
          
          for (ch0=ch; ch0!=NULL; ch0=ch0->NEXT)
            {
              tvs=(ttvsig_list *)ch0->DATA;
              if ((tvs->TYPE & (TTV_SIG_C|TTV_SIG_L|TTV_SIG_R|TTV_SIG_B))!=0 || ttv_isgateoutput(tvf, tvs, mode))
                nlist=addchain(nlist, tvs);
            }
          freechain(ch);
          ch=nlist;
        }
      if (access && ch!=NULL) ch=addchain(ch,NULL);
      if (ch==NULL) 
        {
          avt_errmsg(TTV_API_ERRMSG, "034", AVT_ERROR, (char *)cl->DATA);
          freechain(ch);
          freedoublechain(nodelist);
          return NULL;
        }
      if ((cnt=countchain(ch))>10000)
        avt_errmsg(TTV_API_ERRMSG, "047", AVT_ERROR, (char *)cl->DATA, cnt);
      nodelist=addchain(nodelist, ch);
    }
  nodelist=reverse(nodelist);

  if (lastslope<=0 || noprop)
    lastslope=0;

  ttv_setsearchexclude(0, 0, &oldexstart, &oldexend);
  if (!noprop)
    oldprop=ttv_SetPrecisionLevel(1);
  oldmode=ttv_AutomaticDetailBuild("on");
  mask=NULL;

  d0=ttv_getUorD(dir[0]);
  d1=ttv_getUorD(dir[1]);

  for (cl=(chain_list *)nodelist->DATA; cl!=NULL; cl=cl->NEXT)
    {
      tvs=(ttvsig_list *)cl->DATA;
      if (d0=='d' || d0=='?')
        recur_run(tvf, lastslope, tvs->NODE, nodelist->NEXT, &mask, &config, d1, minmax_i, &allsearch);
      if (d0=='u' || d0=='?')
        recur_run(tvf, lastslope, tvs->NODE+1, nodelist->NEXT, &mask, &config, d1, minmax_i, &allsearch);
    }

  freedoublechain(nodelist);  
  ttv_AutomaticDetailBuild(oldmode);
  if (!noprop)
    ttv_SetPrecisionLevel(oldprop);
  ttv_setsearchexclude(oldexstart, oldexend, &oldexstart, &oldexend);

  // creation des details
  if (countchain(nodenamelist)==2)
    {
      config=reverse(config);      
      path_list=NULL;
      for (cl=config, ch=NULL; cl!=NULL; cl=cl->NEXT)
        {
          path_list=((chain_list *)(cl->DATA))->DATA;
          path_list->USER=addptype(path_list->USER, CLIPCTK, NULL);

          if (ch==NULL) path_list->NEXT=NULL;
          else path_list->NEXT=(ttvpath_list *)ch->DATA;
          ch=addchain(ch, path_list);
          freechain((chain_list *)cl->DATA);
        }
      freechain(ch);

      while (allsearch)
      {
        pth=(ttvpath_list *)allsearch->DATA;
        if (getptype(pth->USER, CLIPCTK)==NULL)
        {
           pth->NEXT=NULL;
           ttv_freepathlist(pth);
        }
        else
          pth->USER=delptype(pth->USER, CLIPCTK);
        allsearch=delchain(allsearch, allsearch);
      }

      path_list=ttv_classpath(path_list, minmax_i);

      ch=NULL;
      while (path_list!=NULL)
        {
          ch=addchain(ch, path_list);
          path_list=path_list->NEXT;
        }
      ch=reverse(ch);

      freechain(config);
    }
  else
    {
/*      removebadconfig(config, 'u', 'u', minmax_i);
      removebadconfig(config, 'u', 'd', minmax_i);
      removebadconfig(config, 'd', 'u', minmax_i);
      removebadconfig(config, 'd', 'd', minmax_i);*/
      ch=NULL;
      path_list=NULL;
      for (cl=config; cl!=NULL; cl=cl->NEXT)
        {
          if (cl->DATA!=NULL)
            {
              if (!nosync) performstbclipping(tvf, (chain_list *)cl->DATA, minmax_i);
              path_list=buildconfigpath((chain_list *)cl->DATA, path_list);
              path_list->CRITIC=buildconfigdetail((chain_list *)cl->DATA);
            }
        }
      path_list=ttv_classpath(path_list, minmax_i);
      while (path_list!=NULL)
        {
          ch=addchain(ch, path_list);
          path_list=path_list->NEXT;
        }
      ch=reverse(ch);

      // cleaning
      for (cl=config; cl!=NULL; cl=cl->NEXT)
        {
          if (cl->DATA!=NULL)
            freechain((chain_list *)cl->DATA);
        }
      freechain(config);
      for (cl=allsearch; cl!=NULL; cl=cl->NEXT)
        {
          for (path_list=(ttvpath_list *)cl->DATA; path_list!=NULL; path_list=path_list->NEXT)
            {
              path_list->USER=testanddelptype(path_list->USER, CLIPCTK);
              path_list->USER=testanddelptype(path_list->USER, CLIPREF);
            }
          ttv_freepathlist((ttvpath_list *)cl->DATA);
        }
      freechain(allsearch);
    }

  ch=filterpath(ch, nbpath);
  return ch;
}

// ------------------------------------------ V2 -----------------------------------------------------------------
static int allow_signal(chain_list *lst, ttvsig_list *tvs, int depth)
{
  chain_list *cl;
  ttvpath_list *pth;
  for (cl=lst; cl!=NULL; cl=cl->NEXT)
    {
      pth=(ttvpath_list *)cl->DATA;
      if (pth->NODE->ROOT==tvs) return 0;
//      if (pth->ROOT->ROOT==tvs) return 0;
    }
  return 1;
}

static void recur_run_v2(ttvfig_list *tvf, long slopein, ttvevent_list *a, ttvsig_list *end, chain_list **lst, chain_list **config, char dirend, long minmax, chain_list **allsearch, int depth)
{
  ttvpath_list *path_list, *pth;
  long slopeout, findcmd=0, savelastslope;
  char dir[3]=" ?";

  if (depth>0 && a->ROOT==end)
    {
      if (dirend=='?' || dirend==ttv_GetTimingEventDirection(pth->ROOT))
        {
          *config=addchain(*config, reverse(dupchainlst(*lst)));
          return;
        }
    }

  if (depth>10) return;

  if (!allow_signal(*lst, a->ROOT, depth)) return;


  dir[0]=ttv_GetTimingEventDirection(a);

  savelastslope=TTV_PROPAGATE_FORCED_SLOPE;
  if (slopein!=0) TTV_PROPAGATE_FORCED_SLOPE=slopein;

  // access
  if((a->ROOT->TYPE & (TTV_SIG_C | TTV_SIG_L | TTV_SIG_R | TTV_SIG_B)) == 0)
    {
      a->ROOT->NODE->TYPE |= TTV_NODE_STOP ;
      (a->ROOT->NODE+1)->TYPE |= TTV_NODE_STOP ;
      if ((a->ROOT->TYPE & TTV_SIG_N) != 0)
        TTV_MORE_SEARCH_OPTIONS|=TTV_MORE_OPTIONS_ENABLE_STOP_ON_TTV_SIG_N;
    }
  if ((a->ROOT->TYPE & TTV_SIG_Q) != 0)
    findcmd=TTV_FIND_CMD;          
  
  // pour que les falsepaths s'appliquent
  if ((end->TYPE & (TTV_SIG_C | TTV_SIG_L | TTV_SIG_R | TTV_SIG_B)) == 0)
    {        
      end->NODE->TYPE|=TTV_NODE_STOP ;
      (end->NODE+1)->TYPE|=TTV_NODE_STOP ;
    }
  
  TTV_QUIET_MODE=1;
  path_list=ttv_getsigaccesslist (tvf, a->ROOT, NULL, a->ROOT, dir, 0, minmax|findcmd, 0, 0);
  TTV_QUIET_MODE=0;
  
  if((a->ROOT->TYPE & (TTV_SIG_C | TTV_SIG_L | TTV_SIG_R | TTV_SIG_B)) == 0)
    {
      a->ROOT->NODE->TYPE &= ~TTV_NODE_STOP ;
      (a->ROOT->NODE+1)->TYPE &= ~TTV_NODE_STOP ;
      if ((a->ROOT->TYPE & TTV_SIG_N) != 0)
        TTV_MORE_SEARCH_OPTIONS&=~TTV_MORE_OPTIONS_ENABLE_STOP_ON_TTV_SIG_N;
    }
  
  // pour que les falsepaths s'appliquent
  if ((end->TYPE & (TTV_SIG_C | TTV_SIG_L | TTV_SIG_R | TTV_SIG_B)) == 0)
    {  
      end->NODE->TYPE&=~TTV_NODE_STOP ;
      (end->NODE+1)->TYPE&=~TTV_NODE_STOP ;
    }
  
                  
  if (path_list!=NULL)
    {
      *allsearch=addchain(*allsearch, path_list);
      
      for (pth=path_list; pth!=NULL; pth=pth->NEXT)
        {
          if (pth->ROOT->ROOT==end || !ttv_PathIsHZ(pth))
            {
              *lst=addchain(*lst, pth);
              
              slopeout=pth->SLOPE;
              recur_run_v2(tvf, slopeout, pth->ROOT, end, lst, config, dirend, minmax, allsearch, depth+1);
              
              *lst=delchain(*lst, *lst);
            }
        }
    }

  // un path?
  if((a->ROOT->TYPE & (TTV_SIG_C | TTV_SIG_L | TTV_SIG_R | TTV_SIG_B)) == 0)
    {
      a->ROOT->NODE->TYPE |= TTV_NODE_STOP ;
      (a->ROOT->NODE+1)->TYPE |= TTV_NODE_STOP ;
      if ((a->ROOT->TYPE & TTV_SIG_N) != 0)
        TTV_MORE_SEARCH_OPTIONS|=TTV_MORE_OPTIONS_ENABLE_STOP_ON_TTV_SIG_N;
    }
  if ((a->ROOT->TYPE & TTV_SIG_Q) != 0)
    findcmd=TTV_FIND_CMD;          
  
          // pour que les falsepaths s'appliquent
  if ((end->TYPE & (TTV_SIG_C | TTV_SIG_L | TTV_SIG_R | TTV_SIG_B)) == 0)
    {        
      end->NODE->TYPE|=TTV_NODE_STOP ;
      (end->NODE+1)->TYPE|=TTV_NODE_STOP ;
    }
  
  path_list=ttv_getcritic_pathlist (tvf, a->ROOT, NULL, dir, 0, minmax|findcmd, 0, 0, 0);
  
  if((a->ROOT->TYPE & (TTV_SIG_C | TTV_SIG_L | TTV_SIG_R | TTV_SIG_B)) == 0)
    {
      a->ROOT->NODE->TYPE &= ~TTV_NODE_STOP ;
      (a->ROOT->NODE+1)->TYPE &= ~TTV_NODE_STOP ;
      if ((a->ROOT->TYPE & TTV_SIG_N) != 0)
        TTV_MORE_SEARCH_OPTIONS&=~TTV_MORE_OPTIONS_ENABLE_STOP_ON_TTV_SIG_N;
    }
  
  // pour que les falsepaths s'appliquent
  if ((end->TYPE & (TTV_SIG_C | TTV_SIG_L | TTV_SIG_R | TTV_SIG_B)) == 0)
    {  
      end->NODE->TYPE&=~TTV_NODE_STOP ;
      (end->NODE+1)->TYPE&=~TTV_NODE_STOP ;
    }
          
  if (path_list!=NULL)
    {
      *allsearch=addchain(*allsearch, path_list);
      
      for (pth=path_list; pth!=NULL; pth=pth->NEXT)
        {
          if (pth->ROOT->ROOT==end || ((pth->ROOT->ROOT->TYPE & (TTV_SIG_R|TTV_SIG_Q))==0 && !ttv_PathIsHZ(pth)))
            {
              *lst=addchain(*lst, pth);
              
              slopeout=pth->SLOPE;
              recur_run_v2(tvf, slopeout, pth->ROOT, end, lst, config, dirend, minmax, allsearch, depth+1);
              
              *lst=delchain(*lst, *lst);
            }
        }
    }
  
  TTV_PROPAGATE_FORCED_SLOPE=savelastslope;

}

chain_list *ttv_ProbeDelay_v2(ttvfig_list *tvf, double slopein, char *start, char *end, char *dir, int nbpath, char *minmax)
{
  chain_list *cl, *mask, *ch, *allsearch=NULL;
  ttvpath_list *path_list;
  ttvsig_list *tvs_start, *tvs_end;
  long minmax_i=1;
  chain_list *config=NULL;
  char *oldmode, d0, d1;
  int oldprop;
  long lastslope=mbk_long_round(slopein*1e12*TTV_UNIT);
  long oldexstart, oldexend;

  if (nbpath<0) nbpath=0;

  if (!strcasecmp (minmax, "max")) minmax_i = TTV_FIND_MAX;
  else if (!strcasecmp (minmax, "min")) minmax_i = TTV_FIND_MIN;
  else
    {
      avt_errmsg(TTV_API_ERRMSG, "022", AVT_ERROR);
//      fprintf (stderr, "ttv_ProbeDelay: error: available values for 'minmax' are 'min' or 'max'\n");
      return NULL;
    }

  tvs_start=ttv_GetTimingSignal(tvf, start);
  tvs_end=ttv_GetTimingSignal(tvf, end);

  if (tvs_start==NULL)
    {
      avt_errmsg(TTV_API_ERRMSG, "034", AVT_ERROR, start);
//      avt_error("ttvapi", -1, AVT_ERR, "ttv_ProbeDelay: could not find node '%s'\n", start);
      return NULL;
    }
  if (tvs_end==NULL)
    {
      avt_errmsg(TTV_API_ERRMSG, "034", AVT_ERROR, end);
      //avt_error("ttvapi", -1, AVT_ERR, "ttv_ProbeDelay: could not find node '%s'\n", end);
      return NULL;
    }

  if (lastslope<=0)
    lastslope=0;

//  ttv_checkinfchange(tvf);

  ttv_setsearchexclude(0, 0, &oldexstart, &oldexend);
  oldprop=ttv_SetPrecisionLevel(1);
  oldmode=ttv_AutomaticDetailBuild("on");
  mask=NULL;

  d0=ttv_getUorD(dir[0]);
  d1=ttv_getUorD(dir[1]);

  if (d0=='d' || d0=='?')
    recur_run_v2(tvf, lastslope, tvs_start->NODE, tvs_end, &mask, &config, d1, minmax_i, &allsearch, 0);
  if (d0=='u' || d0=='?')
    recur_run_v2(tvf, lastslope, tvs_start->NODE+1, tvs_end, &mask, &config, d1, minmax_i, &allsearch, 0);

  ttv_AutomaticDetailBuild(oldmode);
  ttv_SetPrecisionLevel(oldprop);
  ttv_setsearchexclude(oldexstart, oldexend, &oldexstart, &oldexend);


  ch=NULL;
  path_list=NULL;
  for (cl=config; cl!=NULL; cl=cl->NEXT)
    {
      if (cl->DATA!=NULL)
        {
          performstbclipping(tvf, (chain_list *)cl->DATA, minmax_i);
          path_list=buildconfigpath((chain_list *)cl->DATA, path_list);
          path_list->CRITIC=buildconfigdetail((chain_list *)cl->DATA);
        }
    }
  path_list=ttv_classpath(path_list, minmax_i);
  while (path_list!=NULL)
    {
      ch=addchain(ch, path_list);
      path_list=path_list->NEXT;
    }
  ch=reverse(ch);
  
  // cleaning
  for (cl=config; cl!=NULL; cl=cl->NEXT)
    {
      if (cl->DATA!=NULL)
        freechain((chain_list *)cl->DATA);
    }
  freechain(config);
  for (cl=allsearch; cl!=NULL; cl=cl->NEXT)
    {
      for (path_list=(ttvpath_list *)cl->DATA; path_list!=NULL; path_list=path_list->NEXT)
        {
          path_list->USER=testanddelptype(path_list->USER, CLIPCTK);
          path_list->USER=testanddelptype(path_list->USER, CLIPREF);
        }
      ttv_freepathlist((ttvpath_list *)cl->DATA);
    }
  freechain(allsearch);

  ch=filterpath(ch, nbpath);
  return ch;
}

chain_list *ttv_CharacPaths(ttvfig_list *tf, double slopein, char *start, char *end, char *dir, long number, char *all, char *path, char *minmax, double capaout, int propagate)
{
  int oldprop=ttv_SetPrecisionLevel(propagate!=0?1:0);
  long lastslope=mbk_long_round(slopein*1e12*TTV_UNIT);
  long savelastslope;
  double savelastcapa;
  chain_list *res;
  char *oldmode;
  
  API_TEST_TOKEN_SUB(TMA_API,"tma")
//  ttv_checkinfchange(tf);

  if (propagate==2)
    ttv_experim_setstopaftergate1(1);
  
  if (lastslope<=0)
    lastslope=0;

  oldmode=ttv_AutomaticDetailBuild("on");
  savelastslope=TTV_PROPAGATE_FORCED_SLOPE;
  savelastcapa=TTV_PROPAGATE_FORCED_CAPA;
  if (lastslope!=0) TTV_PROPAGATE_FORCED_SLOPE=lastslope;
  TTV_PROPAGATE_FORCED_CAPA=capaout*1e15;
  res=ttv_GetPaths_sub(tf, start, start, end, dir, number, all, path, minmax);
  TTV_PROPAGATE_FORCED_CAPA=savelastcapa;
  TTV_PROPAGATE_FORCED_SLOPE=savelastslope;
  ttv_SetPrecisionLevel(oldprop);

  ttv_AutomaticDetailBuild(oldmode);

  ttv_experim_setstopaftergate1(0);

  return res;
}

static chain_list *converttodoublelist(chain_list *val, int nbx)
{
  chain_list *cl, *ch=NULL;
  int i;

  while (val!=NULL)
    {
      cl=NULL;
      for (i=0;i<nbx && val!=NULL;i++)
        {
          cl=addchain(cl, val->DATA);
          val=delchain(val,val);
        }
      ch=addchain(ch, reverse(cl));
    }
  return reverse(ch);
}
chain_list *ttv_CharacPathTables(ttvpath_list *pth, chain_list *slopes, chain_list *capas, int propagate)
{
//  int oldprop=ttv_SetPrecisionLevel(propagate!=0?1:0);
  float val;
  chain_list *res, *cl;
  chain_list *slopes_in=NULL, *capas_in=NULL, *delays_out, *slopes_out, *energy;
  long type;
  
  API_TEST_TOKEN_SUB(TMA_API,"tma")
//  ttv_checkinfchange(tf);

  type=(pth->TYPE & (~TTV_FIND_PATH)) | TTV_FIND_LINE;
  
/*  if (propagate==2)
    ttv_experim_setstopaftergate1(1);
*/
  if (slopes==NULL) slopes_in=addchain(NULL, (void *)TTV_NOTIME);
  else
  {
    for (cl=slopes; cl!=NULL; cl=cl->NEXT)
    {
      val=*(float *)&cl->DATA;
      if (val>0)
        slopes_in=addchain(slopes_in, (void *)mbk_long_round(val*1e12*TTV_UNIT));
      else
        slopes_in=addchain(slopes_in, (void *)TTV_NOTIME);
    }
  }

  if (capas==NULL) capas_in=addchain(NULL, 0), *(float *)&capas_in->DATA=-1.0;
  else
  {
    for (cl=capas; cl!=NULL; cl=cl->NEXT)
    {
      val=*(float *)&cl->DATA;
      capas_in=addchain(capas_in, NULL);
      *(float *)&capas_in->DATA=val*1e15;
    }
  }

  slopes_in=reverse(slopes_in);
  capas_in=reverse(capas_in);

  ttv_activate_search_charac_mode(slopes_in, capas_in);

  if (pth->ROOT==pth->NODE && pth->DELAY==0)
  {
     int nbx, nby, i;
     nbx=countchain(slopes_in);
     nby=countchain(capas_in);
     delays_out=NULL;
     slopes_out=NULL;
     energy=NULL;
     for (cl=slopes_in; cl!=NULL; cl=cl->NEXT)
       for (i=0; i<nby; i++)
       {
         delays_out=addchain(delays_out, (void *)pth->DELAY);
         slopes_out=addchain(slopes_out, cl->DATA);
         energy=addchain(energy, NULL);
         *(float *)&energy->DATA=0;
       }
  }
  else
  {
    long sTTV_MORE_SEARCH_OPTIONS=TTV_MORE_SEARCH_OPTIONS;
    TTV_MORE_SEARCH_OPTIONS=pth->TTV_MORE_SEARCH_OPTIONS;
    if ((pth->TYPE & TTV_FIND_ALL)==0)
      ttv_getcritic(ttv_GetTopTimingFigure(pth->FIG),pth->FIG, pth->ROOT, pth->NODE, pth->LATCH, pth->CMDLATCH, type);
    else
      ttv_getcriticpara(ttv_GetTopTimingFigure(pth->FIG), pth->FIG, pth->ROOT, pth->NODE, pth->DELAY, type, NULL, pth->TTV_MORE_SEARCH_OPTIONS);
    ttv_retreive_search_charac_results(&delays_out, &slopes_out, &energy);
    TTV_MORE_SEARCH_OPTIONS=sTTV_MORE_SEARCH_OPTIONS;
  }

  for (cl=delays_out; cl!=NULL; cl=cl->NEXT)
    val=((long)cl->DATA)*1e-12/TTV_UNIT, *(float *)&cl->DATA=val;

  for (cl=slopes_out; cl!=NULL; cl=cl->NEXT)
    val=((long)cl->DATA)*1e-12/TTV_UNIT, *(float *)&cl->DATA=val;

  if(energy)
    cl=addchain(NULL, converttodoublelist(energy, countchain(capas_in)));
  else
    cl=addchain(NULL, NULL);
  cl=addchain(cl, converttodoublelist(slopes_out, countchain(capas_in)));
  cl=addchain(cl, converttodoublelist(delays_out, countchain(capas_in)));

  ttv_activate_search_charac_mode(NULL, NULL);
  freechain(capas_in);
  freechain(slopes_in);
  return cl;
}

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

typedef struct
{
  ttvevent_list *input;
  ttvpath_list *path;
} input_group;



static int ttv_run_detail(ttvpath_list *path1, ttvpath_list *path2, char dir, int *depth, char *name)
{
  chain_list *det1, *det2, *c1, *c2;
  char *last=NULL;
  int i;

  det1=ttv_GetPathDetail(path1);
  det2=ttv_GetPathDetail(path2);

  if (dir=='b') det1=reverse(det1), det2=reverse(det2);

  c1=det1; c2=det2;
  i=0;
  while (c1!=NULL && c2!=NULL)
    {
      if (strcmp(ttv_GetDetailSignalName((ttvcritic_list *)c1->DATA), ttv_GetDetailSignalName((ttvcritic_list *)c2->DATA))!=0) break;

      last=ttv_GetDetailSignalName((ttvcritic_list *)c1->DATA);
      c1=c1->NEXT, c2=c2->NEXT, i++;
    }

  *depth=i;
  strcpy(name, last);
  
  if (c1==NULL || c2==NULL)
    i=0;
  else
    i=1;

  if (dir=='b') det1=reverse(det1), det2=reverse(det2);

  ttv_FreePathDetail(det1);
  ttv_FreePathDetail(det2);
  freechain(det1);
  freechain(det2);
  return i;
}




static chain_list *getendpoints(ttvfig_list *tvf, chain_list *startpoints)
{
  chain_list *commands, *cl;
  ht *myht;

#if 0

  char *oldmode;
  chain_list *allpaths;
  ttvpath_list *pth;


  oldmode=ttv_AutomaticDetailBuild("off");
//  ttv_SetSearchMode("dual");

  commands=append(ttv_GetTimingSignalList(tvf, "command", "all"), NULL/*ttv_GetTimingSignalList(tvf, "latch", "all")*/);
  commands=append(commands, ttv_GetTimingSignalList(tvf, "precharge", "all"));

  allpaths=ttv_internal_GetPaths_EXPLICIT(tvf, NULL, startpoints, commands, "??", -1, "critic", "path", "max");

  freechain(commands);

//  ttv_SetSearchMode("!dual");
  ttv_AutomaticDetailBuild(oldmode);

  myht=addht(10000);
  for (cl=allpaths; cl!=NULL; cl=cl->NEXT)
    {
      pth=(ttvpath_list *)cl->DATA;
      if ((pth->ROOT->ROOT->TYPE & (TTV_SIG_Q|TTV_SIG_L|TTV_SIG_R))!=0)
        addhtitem(myht, pth->ROOT->ROOT, 1);
    }
  commands=GetAllHTKeys(myht);
  delht(myht);

  ttv_FreePathList(allpaths);
  freechain(allpaths);
#else
  long type=TTV_FIND_MAX;
  chain_list *ch;

  if ((ttv_getloadedfigtypes(tvf) & TTV_FILE_DTX)==TTV_FILE_DTX)
    type = TTV_FIND_LINE;
  else
    type = TTV_FIND_PATH;

  ch=ttv_getendpoints(tvf, startpoints, type|TTV_FIND_DUAL);

  myht=addht(10000);
  for (cl=ch; cl!=NULL; cl=cl->NEXT)
    {
      if ((((ttvsig_list *)cl->DATA)->TYPE & (TTV_SIG_Q|TTV_SIG_L|TTV_SIG_R))!=0)
        addhtitem(myht, cl->DATA, 1);
    }
  freechain(ch);
  commands=GetAllHTKeys(myht);
  delht(myht);
  
#endif
  return commands;
}

static int countgatesmax2(chain_list *det)
{
  int cnt=0;
  while (det!=NULL && cnt<2)
  {
    if ((((ttvcritic_list *)det->DATA)->TYPE & TTV_FIND_RC)==0) cnt++;
    det=det->NEXT;
  }
  return cnt;
}
static chain_list *groupparallelpaths(chain_list *allpaths, NameAllocator *na)
{
  ht *same;
  char buf[2048], *name;
  long l;
  chain_list *lst, *elems, *cl, *ch;
  ttvpath_list *pth;
  int requ;
  chain_list *det;

  same=addht(10000);
  for (ch=allpaths; ch!=NULL; ch=ch->NEXT)
    {
      pth=(ttvpath_list *)ch->DATA;
//      sprintf(buf, "%p,%p", pth->NODE, pth->ROOT);
      if ((pth->ROOT->ROOT->TYPE & TTV_SIG_R)==0) requ=1; else requ=2;

      det=ttv_GetPathDetail(pth);
      if (det==NULL || countgatesmax2(det->NEXT)<requ) continue;
      if ((pth->ROOT->ROOT->TYPE & TTV_SIG_R)==0)
      {
        sprintf(buf, "%p,%p,%d", pth->NODE,pth->ROOT->ROOT,ttv_PathIsHZ(pth));
      }
      else
        {
          ttvcritic_list *tc;
          det=reverse(det);
          tc=(ttvcritic_list *)det->NEXT->DATA;
          sprintf(buf, "%p,%p,%s", pth->NODE,pth->ROOT->ROOT,ttv_GetDetailSignalName(tc));
        }
      freechain(det);
      name=NameAlloc(na, buf);
      if ((l=gethtitem(same, name))==EMPTYHT) lst=NULL;
      else lst=(chain_list *)l;
      lst=addchain(lst, pth);
      addhtitem(same, name, (long)lst);
    }
  
  elems=GetAllHTElems(same);
  ch=NULL;
  for (cl=elems; cl!=NULL; cl=cl->NEXT)
    {
      lst=(chain_list *)cl->DATA;
  //    if (countchain(lst)>1)
        ch=addchain(ch, lst);
/*      else
        freechain(lst);*/
    }
  freechain(elems);
  delht(same);
  return ch;
}

static int ttv_check_false_path_simple(ttvfig_list *tvf, chain_list *grps, inffig_list *ifl, cnsfig_list *cf, NameAllocator *na, int singlepath, int tillend)
{
  chain_list *cl, *ch, *ch0, *savech;
  chain_list *det;
  char convname[1024], divname[1024], buf[2048];
  ttvpath_list *pth;
  ttvcritic_list *tc, *tc0, *tc2;
  int cnt=0, tag, besttag, beststarttag, newfalsepath;
  ptype_list *ptype, *p0, *p1;
  char *where, *prev;
  ht *prevht, *nextht;
  long l, l0;

  besttag=beststarttag=-1;
  if (!singlepath && !tillend)
    {
      for (cl=grps; cl!=NULL && cl->NEXT!=NULL; cl=cl->NEXT)
        {
          if (ttv_run_detail((ttvpath_list *)cl->DATA, (ttvpath_list *)cl->NEXT->DATA, 'b', &tag, buf))
            {
              if (besttag==-1 || tag<besttag)
                {
                  strcpy(convname, buf);
                  besttag=tag;
                }
            }
          /*     if (ttv_run_detail((ttvpath_list *)cl->DATA, (ttvpath_list *)cl->NEXT->DATA, 'f', &tag, buf))
                 {
                 if (beststarttag==-1 || tag<beststarttag)
                 {
                 strcpy(divname, buf);
                 beststarttag=tag;
                 }
                 }*/
        }
    }
  
  nextht=addht(100);
  prevht=addht(100);

  for (cl=grps, ch=NULL, savech=NULL; cl!=NULL; cl=cl->NEXT)
    {
      pth=(ttvpath_list *)cl->DATA;
      det=ttv_GetPathDetail(pth);
      if (beststarttag==-1)
         strcpy(divname, ttv_GetFullSignalName(tvf, ttv_GetPathStartSignal(pth)));
      if (besttag==-1) 
         strcpy(convname, ttv_GetFullSignalName(tvf, ttv_GetPathEndSignal(pth)));
      prev=NULL;
      for (ch0=det; ch0!=NULL; ch0=ch0->NEXT)
        {
          tc=(ttvcritic_list *)ch0->DATA;
          if (ch0->NEXT!=NULL && (tc->TYPE & TTV_FIND_RC) == TTV_FIND_RC) continue;

          where=NameAlloc(na, ttv_GetDetailSignalName(ch0->DATA));
          if (prev!=NULL)
            {
              if ((l=gethtitem(prevht, where))==EMPTYHT)
                addhtitem(prevht, where, (long)prev);
              else if (l!=1 && l!=(long)prev)
                addhtitem(prevht, where, 1); // convergence
              
              if ((l=gethtitem(nextht, prev))==EMPTYHT)
                addhtitem(nextht, prev, (long)where);
              else if (l!=1 && l!=(long)where)
                addhtitem(nextht, prev, 1); // divergence

              if (singlepath && strcmp(ttv_GetDetailType(tc),"inv")==0)
                {
                  addhtitem(nextht, prev, 1);
                  addhtitem(prevht, where, 1);
                }
            }
          prev=where;
        }
      if (ttv_PathIsHZ(pth)) ((ttvcritic_list *)det->DATA)->CAPA=-1;
      else ((ttvcritic_list *)det->DATA)->CAPA=1;
      ch=addchain(ch, pth);
      savech=addchain(savech, det);
    }

  newfalsepath=1;

  while (newfalsepath)
  {
    newfalsepath=0;
    for (cl=ch; cl!=NULL; cl=cl->NEXT)
      {
        pth=(ttvpath_list *)cl->DATA;
        tc=pth->CRITIC;
        if (tc==NULL) continue;
        if ((tag=ttv_path_is_activable(tvf, cf, pth, ch, divname, convname))==0)
          {
            ptype=NULL;
            tag=0;
            for (tc0=tc; tc0!=NULL; tc0=tc0->NEXT)
              {
                if (/*tc0->NEXT!=NULL && */(tc0->TYPE & TTV_FIND_RC) == TTV_FIND_RC) continue;
                where=NameAlloc(na, ttv_GetDetailSignalName(tc0));
                l0=gethtitem(nextht, where);
                l=gethtitem(prevht, where);
                if (tc0==tc || tc0->NEXT==NULL || l==1 || l0==1)
                  {
                    if (tag) ptype=addptype(ptype, INF_NOTHING, NULL); // <void>
                    else if (l==1)
                      ptype=addptype(ptype, ttv_GetDetailDirection(tc2)=='u'?INF_UP:INF_DOWN, namealloc(ttv_GetDetailSignalName(tc2)));
                    ptype=addptype(ptype, ttv_GetDetailDirection(tc0)=='u'?INF_UP:INF_DOWN, namealloc(ttv_GetDetailSignalName(tc0)));
                    tag=1;
                  }
                else tag=0;
                tc2=tc0;
              }
            if (tc->CAPA<0) ptype=addptype(ptype, INF_CK, namealloc("^")); // HZ
            else if ((tc2->SIGTYPE & TTV_SIG_R)!=0 || (tc2->SIGTYPE & TTV_SIG_CT)==TTV_SIG_CT)
                   ptype=addptype(ptype, INF_CK, namealloc("°")); // NOT HZ
              
            ptype=(ptype_list *)reverse((chain_list *)ptype);


            if (ifl==NULL) ifl=addinffig(tvf->INFO->FIGNAME);
#if 0
            for (ch0=ifl->LOADED.INF_FALSEPATH; ch0!=NULL; ch0=ch0->NEXT)
              {
                for (p0=ptype, p1=(ptype_list *)ch0->DATA; p0!=NULL && p1!=NULL; p0=p0->NEXT, p1=p1->NEXT)
                  if (p0->TYPE!=p1->TYPE || p0->DATA!=p1->DATA) break;

                if (p0==NULL && p1==NULL) break;
              }
#else
            ch0=NULL;
#endif
            if (ch0==NULL)
              {
//                printf("new falsepath detected %s %c\n", divname, ttv_GetDetailDirection(tc0));
                avt_log(LOGFALSEPATH,1,"// FalsePath detected: ");
                for (p0=ptype; p0!=NULL; p0=p0->NEXT)
                  {
                    switch(p0->TYPE)
                      {
                      case INF_UPDOWN: where="rise/fall"; break;
                      case INF_DOWN: where="fall"; break;
                      case INF_UP: where="rise"; break;
                      case INF_CK: where="HZ"; break;
                      default: where="?";
                      }
                    if (p0->DATA==NULL)
                      avt_log(LOGFALSEPATH,1,"<nothing>");
                    else
                      avt_log(LOGFALSEPATH,1,"%s %s", (char *)p0->DATA, where);
                    if (p0->NEXT!=NULL) avt_log(LOGFALSEPATH,1," -> ");
                  }
                avt_log(LOGFALSEPATH,1,"\n");
                ifl->LOADED.INF_FALSEPATH=addchain(ifl->LOADED.INF_FALSEPATH,ptype);
                cnt++;
              }
            else
              freeptype(ptype);
            //newfalsepath=1;
//            cl->DATA=NULL; // ne plus prendre en compte
          }
        else
          if (tag==2) {newfalsepath=0; break;} // une erreur (signal non trouve): aucun check ne fonctionnera
      }
  }
  for (cl=savech; cl!=NULL; cl=cl->NEXT)
    {      
      ttv_FreePathDetail(cl->DATA);
      freechain((chain_list *)cl->DATA);
    } 
  freechain(ch);
  freechain(savech);
  delht(nextht);
  delht(prevht);
  return cnt;
}

//void ttv_DetectFalseParallelClockPath(ttvfig_list *tvf)
void ttv_DetectFalsePath_sub(ttvfig_list *tvf, char *start, char *end, int addsinglepath, int access)
{
  chain_list *commands, *cl, *grps, *allpaths, *runcmd, *allclocksig;
  char *oldmode;
  ttvsig_list *tvs;
  ttvpath_list *pth;
  chain_list *allgroups, *maskin;
  int i, cnt, nbpaths;
  inffig_list *ifl;
  cnsfig_list *cf;
  NameAllocator na, na_rc;
  char *falsedebug, *noprech;
  char *type;
  char buf[128];

 
#ifdef DELAY_DEBUG_STAT
  long TM=0;
  int cur, tot;
  
#endif

  type=access?"access":(end==NULL && start==NULL)?"clock":"combinatorial";

  sprintf(buf,"%s falsepaths:",type);


  if ((ifl=getloadedinffig(tvf->INFO->FIGNAME))==NULL)
    ifl=addinffig(tvf->INFO->FIGNAME);

  cf=ttvutil_cnsload(tvf->INFO->FIGNAME, ifl);

  if (cf==NULL) { avt_errmsg(TTV_API_ERRMSG, "044", AVT_WARNING,tvf->INFO->FIGNAME); return;}

  if (start==NULL || (strcmp(start,"*")==0 && access))
    {
      allclocksig=ttv_getclocksiglist(tvf);
      allclocksig=append(ttv_getasyncsiglist(tvf), allclocksig);

      if (allclocksig==NULL)
        {
          avt_errmsg(TTV_API_ERRMSG, "002", AVT_WARNING, type);
          return;
        }
    }
  else
    {
      allclocksig=ttv_getsigbytype_and_netname(tvf,NULL,TTV_SIG_C|TTV_SIG_L|TTV_SIG_R|TTV_SIG_B|TTV_SIG_Q,maskin=addchain(NULL, start)) ;
      freechain(maskin);
      if (allclocksig==NULL && access) return;
    }

  falsedebug=getenv("FALSEDEBUG");
  noprech=getenv("FALSENOPRECH");

  if (end==NULL)
  {
    commands=getendpoints(tvf, allclocksig/*clocks*/);
    maskin=addchain(NULL, "*");
    commands=append(ttv_getmatchingdirectivenodes(tvf, maskin, 1), commands);
    freechain(maskin);
  }
  else
    {
      commands=ttv_getsigbytype_and_netname(tvf,NULL,TTV_SIG_C|TTV_SIG_L|TTV_SIG_R|TTV_SIG_B|TTV_SIG_Q,maskin=addchain(NULL, end)) ;
      commands=append(ttv_getmatchingdirectivenodes(tvf, maskin, 1), commands);
      freechain(maskin);
    }
  
  oldmode=ttv_AutomaticDetailBuild("on");

#ifdef DELAY_DEBUG_STAT
  tot=countchain(commands);
  cur=0;
#endif

  cnt=0;      
  nbpaths=0;

  CreateNameAllocator( 1000, &na_rc, 'y' );

  for (runcmd=commands; runcmd!=NULL; runcmd=runcmd->NEXT)
    {
      tvs=(ttvsig_list *)runcmd->DATA;

      if ((tvs->TYPE & (TTV_SIG_C|TTV_SIG_Q|TTV_SIG_L|TTV_SIG_B|TTV_SIG_R))!=0)
      {
      if (!access && (start==NULL || end==NULL))
        {
          if (!((tvs->TYPE & TTV_SIG_LR)==TTV_SIG_LR
                || (tvs->TYPE & TTV_SIG_LS)==TTV_SIG_LS
                || (tvs->TYPE & TTV_SIG_Q)==TTV_SIG_Q
                || (!noprech && (tvs->TYPE & TTV_SIG_R)==TTV_SIG_R))
                || (noprech && (tvs->TYPE & TTV_SIG_R)==TTV_SIG_R
                      )
              )
            {
#ifdef DELAY_DEBUG_STAT
              cur++;
#endif
              continue;
            }
        }
      }
      if (NameAllocFind(&na_rc, ttv_GetFullSignalNetName(tvf, tvs))) continue;
      NameAllocFind(&na_rc, ttv_GetFullSignalNetName(tvf, tvs));

      if (falsedebug!=NULL && strcasecmp(ttv_GetFullSignalNetName(tvf, tvs), falsedebug)!=0) continue;

      cl=addchain(NULL, tvs);
      allpaths=ttv_internal_GetPaths_EXPLICIT(tvf, access?allclocksig:NULL, !access?allclocksig:NULL, cl, "??", -1, "all", !access?"path":"access", "max");
      freechain(cl);

      CreateNameAllocator( 1000, &na, 'y' );

      allgroups=groupparallelpaths(allpaths, &na);
      
      for (cl=allgroups; cl!=NULL; cl=cl->NEXT)
        {
          grps=(chain_list *)cl->DATA;
          pth=(ttvpath_list *)grps->DATA;

          i=countchain(grps);
          if (i>1)
            {
              avt_log(LOGFALSEPATH,6,"solving // false path from %s to %s %s (tr=%c-* %d paths)\n", ttv_GetFullSignalNetName(tvf, pth->NODE->ROOT), ttv_GetFullSignalNetName(tvf, tvs), ttv_PathIsHZ(pth)?"hz":"", ttv_GetPathStartDirection(pth), countchain(grps));
              
              cnt+=ttv_check_false_path_simple(tvf, grps, ifl, cf, &na, 0, addsinglepath?1:0);
              
            }
          else
            {
              avt_log(LOGFALSEPATH,6,"solving single // path from %s to %s %s (tr=%c-%c)\n", ttv_GetFullSignalNetName(tvf, pth->NODE->ROOT), ttv_GetFullSignalNetName(tvf, tvs), ttv_PathIsHZ(pth)?"hz":"", ttv_GetPathStartDirection(pth), ttv_GetPathEndDirection(pth));
              cnt+=ttv_check_false_path_simple(tvf, grps, ifl, cf, &na, 1, 1);
            }
          nbpaths+=i;
          freechain(grps);
        }
      
#ifdef DELAY_DEBUG_STAT
      
      mbk_DisplayProgress(&TM,buf,cur,tot,'%');
      cur++;
#endif
      freechain(allgroups);

      ttv_FreePathList(allpaths);
      freechain(allpaths);

      DeleteNameAllocator(&na);
    }

  DeleteNameAllocator(&na_rc);
#ifdef DELAY_DEBUG_STAT
  mbk_DisplayProgress(&TM,buf,cur,tot,'e');  
#endif
  avt_log(LOGFALSEPATH, 0, "%d paths analyzed, %d false %s paths found\n", nbpaths, cnt, type);
      
  if (cnt>0) ttv_getinffile(tvf);

  ttv_AutomaticDetailBuild(oldmode);
  freechain(commands);
  freechain(allclocksig);
}

void ttv_DetectFalseClockPath(ttvfig_list *tvf)
{
  ttv_search_mode(1, TTV_MORE_OPTIONS_MUST_BE_CLOCK);
  ttv_DetectFalsePath_sub(tvf, NULL, NULL, 0, 0);
  ttv_search_mode(0, TTV_MORE_OPTIONS_MUST_BE_CLOCK);
}

void ttv_DetectFalsePath(ttvfig_list *tvf, char *start, char *end)
{
  ttv_DetectFalsePath_sub(tvf, start, end, 1, 0);
  ttv_DetectFalsePath_sub(tvf, start, end, 1, 1);
}

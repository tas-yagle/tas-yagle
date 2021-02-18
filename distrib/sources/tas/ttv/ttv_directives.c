
#include "ttv.h"
#include "ttv_directives.h"

static ttv_directive *ttv_add_directive(HeapAlloc *ha, ttvsig_list *tvs)
{
  ttv_directive *sd;
  ptype_list *pt;

  if ((pt=getptype(tvs->USER, TTV_SIG_DIRECTIVES))==NULL)
    pt=tvs->USER=addptype(tvs->USER, TTV_SIG_DIRECTIVES, NULL);

  sd=(ttv_directive *)AddHeapItem(ha);
  sd->next=pt->DATA;
  pt->DATA=sd;
  return sd;
}

ttv_directive *ttv_get_directive(ttvsig_list *tvs)
{
  ptype_list *pt;

  if ((pt=getptype(tvs->USER, TTV_SIG_DIRECTIVES))==NULL) return NULL;
  return (ttv_directive *)pt->DATA;
}


static chain_list *ttv_getsiglist(ttvfig_list *tvf, ht *h, char *name, NameAllocator *NA)
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
  if (chainsig==NULL) avt_errmsg(TTV_ERRMSG, "060", AVT_WARNING, name);
  return chainsig;
}

void ttv_setdirectives (ttvfig_list *tvf, inffig_list *ifl, HeapAlloc *ha)
{
  chain_list *chainx, *list, *origlist, *destlist;
  inf_assoc *assoc;
  splitint *si;
  int val;
  NameAllocator NA;
  ht *h;
  ttv_directive *sd;

  if (inf_GetPointer (ifl, INF_DIRECTIVES, "", (void **)&list)) 
    {
      ttv_disablecache(tvf);
/*      chainx=ttv_levelise(tvf,tvf->INFO->LEVEL,ttv_getloadedfigtypes(tvf));
      freechain(chainx);*/

      h=ttv_buildquickaccessht(tvf, &NA);
    
      for (chainx = list; chainx; chainx = chainx->NEXT) 
        {
          assoc = (inf_assoc *) chainx->DATA;
          val=assoc->lval;
          si=(splitint *)&val;

          origlist=ttv_getsiglist(tvf, h, assoc->orig, &NA);
          while (origlist!=NULL)
            {
              if ((si->cval.d & INF_DIRECTIVE_DELAY)==0)
                destlist=ttv_getsiglist(tvf, h, assoc->dest, &NA);
              else
                destlist=addchain(NULL, NULL);
              while (destlist!=NULL)
                {
                  sd=ttv_add_directive(ha, (ttvsig_list *)origlist->DATA);
                  if (si->cval.a & INF_DIRECTIVE_FILTER)
                    sd->filter=1;
                  else
                    sd->filter=0;

                  sd->target1_dir=si->cval.b;
                  sd->target2_dir=si->cval.d;
                  sd->operation=si->cval.c;
                  sd->target2=(ttvsig_list *)destlist->DATA;
                  sd->from=assoc->dest;
                  sd->margin=mbk_long_round(assoc->dval*1e12*TTV_UNIT);
                  destlist=delchain(destlist, destlist);
                }
              origlist=delchain(origlist, origlist);
            }
        }
      delht(h);
      DeleteNameAllocator(&NA);
    }
}

ptype_list *ttv_get_directive_slopes(ttvevent_list *tve, int setup, int clockisdata, int dataisclock)
{
  ttv_directive *sd;
  ptype_list *res=NULL;
  int dir;

  if ((sd=ttv_get_directive(tve->ROOT))!=NULL)
    {
      while (sd!=NULL)
        {
          if (!sd->filter)
            {
              if (dataisclock>=0 &&
                  ((dataisclock==1 && (sd->target1_dir & INF_DIRECTIVE_CLOCK)==0)
                  || (dataisclock==0 && (sd->target1_dir & INF_DIRECTIVE_CLOCK)!=0)))
                { sd=sd->next; continue;}
              if (clockisdata<0 || ((!clockisdata && (sd->target2_dir & INF_DIRECTIVE_CLOCK)!=0)
                                    || (clockisdata && (sd->target2_dir & INF_DIRECTIVE_CLOCK)==0)))
/*                  (sd->target1_dir & INF_DIRECTIVE_CLOCK)==0 && sd->target2_dir & INF_DIRECTIVE_CLOCK)!=0)*/
                {
                  if (setup>=0 && ((sd->operation==INF_DIRECTIVE_AFTER && setup)
                                   || (sd->operation==INF_DIRECTIVE_BEFORE && !setup))
                    ) { sd=sd->next; continue;}

                  if ((((sd->target1_dir & INF_DIRECTIVE_RISING)!=0 && (tve->TYPE & TTV_NODE_UP)==0)
                       || ((sd->target1_dir & INF_DIRECTIVE_FALLING)!=0 && (tve->TYPE & TTV_NODE_DOWN)==0)))
                    { sd=sd->next; continue; }

                  if ((sd->target2_dir & INF_DIRECTIVE_RISING)!=0)
                    {
                      res=addptype(res, sd->margin, &sd->target2->NODE[1]);
                    }
                  else if ((sd->target2_dir & INF_DIRECTIVE_FALLING)!=0)
                    {
                      res=addptype(res, sd->margin, &sd->target2->NODE[0]);
                    }
                  else
                    {
                      if ((sd->target2_dir & INF_DIRECTIVE_UP)!=0) dir=1;
                      else dir=0;

                      if (!setup) dir=(dir+1) & 1;
                      res=addptype(res, sd->margin, &sd->target2->NODE[dir]);
                    }
                }              
            }
          sd=sd->next;
        }
    }
  return res;
}

chain_list *ttv_get_signal_with_directives(ttvfig_list *tvf, int clocks, int data)
{
  chain_list *allnodes, *cl=NULL, *ch;
  ttvsig_list *tvs, *tvs0;
  ptype_list *pt, *pt0;
  int i, j;
  
  allnodes=ttv_getsigbytype(tvf,NULL,TTV_SIG_TYPEALL,NULL);
  for (ch=allnodes; ch!=NULL; ch=ch->NEXT)
  {
    tvs=(ttvsig_list *)ch->DATA;
    for (j=0; j<1; j++)
    {
      for (i=0; i<1; i++)
      {
        pt=ttv_get_directive_slopes(&tvs->NODE[j], i, -1, -1);
//        pt=(ptype_list *)append((chain_list *)ttv_get_directive_slopes(&tvs->NODE[j], i, 1), (chain_list *)pt);
        for (pt0=pt; pt0!=NULL; pt0=pt0->NEXT)
        {
          tvs0=((ttvevent_list *)pt0->DATA)->ROOT;
          if (clocks && pt!=NULL && (tvs0->TYPE & TTV_SIG_MARQUE)==0) 
          {
            cl=addchain(cl, tvs0);
            tvs0->TYPE|=TTV_SIG_MARQUE;
          }
        }
        freeptype(pt);
      }
    }
    if (data && pt!=NULL && (tvs->TYPE & TTV_SIG_MARQUE)==0) 
      {
        cl=addchain(cl, tvs);
        tvs->TYPE|=TTV_SIG_MARQUE;
      }
  }
  freechain(allnodes);
  for (ch=cl; ch!=NULL; ch=ch->NEXT)
  {
    tvs=(ttvsig_list *)ch->DATA;
    tvs->TYPE&=~TTV_SIG_MARQUE;
  }
 return cl;
}

chain_list *ttv_getmatchingdirectivenodes(ttvfig_list *tvf, chain_list *mask, int nonstop)
{
  chain_list *cl, *chainsig=NULL;
  ttvsig_list *tvs;
  char buf[1024];

  cl=ttv_get_signal_with_directives(tvf, 1, 1);
  while (cl!=NULL)
  {
    tvs=(ttvsig_list *)cl->DATA;
    if ((!nonstop || (tvs->TYPE & (TTV_SIG_C|TTV_SIG_Q|TTV_SIG_L|TTV_SIG_B|TTV_SIG_R))==0)
        && ttv_testnetnamemask(tvf, tvs, mask))
       chainsig=addchain(chainsig, tvs);
    cl=delchain(cl, cl);
  }
  return chainsig;
}


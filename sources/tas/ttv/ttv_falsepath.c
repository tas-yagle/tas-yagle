/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TTV Verison 1                                               */
/*    Fichier : ttv_falsepath.c                                             */
/*                                                                          */
/*    (c) copyright 1995-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/
/* fontion d'utilite general                                                */
/****************************************************************************/

#include "ttv.h"
#include "ttv_directives.h"

/*
  FALSE PATH HZ apres meditation
  si chemin     F - F - R   (precharge par eg.)
  alors chemin  R - R - Rz  (hz correspondant)
*/

typedef struct
{
  ttvfalsepath_list *WILDCARDS, *NOWILDCARDS;
} falsepath_info;

typedef struct
{
  int nb;
  chain_list *FALSEPATH_WILDCARD, *FALSEPATH_NOWILDCARD;
} nowildcard_info;

static int ttv_samefalsedir(ptype_list *pt, ttvevent_list *ev, long type)
{
  if ((type & TTV_FIND_HZ)!=TTV_FIND_HZ) return (ev->TYPE & pt->TYPE)!=0;
  if ((pt->TYPE & TTV_NODE_UP)!=0) return (ev->TYPE & TTV_NODE_DOWN)!=0;
  return (ev->TYPE & TTV_NODE_UP)!=0;
}

int ttv_canbeinfalsepath(ttvevent_list *ev, char where)
{
  switch (where)
  {
    case 'i':
              if((ev->TYPE & TTV_NODE_FALSEIN) == TTV_NODE_FALSEIN) return 1;
              if ((ev->TYPE & TTV_NODE_STOP) == TTV_NODE_STOP && (ev->ROOT->TYPE & (TTV_SIG_C | TTV_SIG_L | TTV_SIG_R |TTV_SIG_N|TTV_SIG_B|TTV_SIG_Q)) == 0) return 1;
              return 0;
    case 'o': if((ev->TYPE & TTV_NODE_FALSEOUT) == TTV_NODE_FALSEOUT) return 1;
              if ((ev->TYPE & TTV_NODE_STOP) == TTV_NODE_STOP && (ev->ROOT->TYPE & (TTV_SIG_C | TTV_SIG_L | TTV_SIG_R |TTV_SIG_N|TTV_SIG_B|TTV_SIG_Q)) == 0) return 1;
              return 0;
    case 'f': if ((ev->TYPE & TTV_NODE_FALSEPATHNODE)!=0) return 1;
              return 0;
    default:
              // error
              return 0;
  }
}

static int ttv_falsepath_match_name(ttvfig_list *tvf, ttvsig_list *tvs, char *regex)
{
  char buf[1024];
  if (mbk_TestREGEX(ttv_getsigname(tvf, buf, tvs),regex)) return 1;
  if (mbk_TestREGEX(ttv_getnetname(tvf, buf, tvs),regex)) return 1;
  return 0;
}

ht *ttv_buildquickaccessht_forfalsepath(ttvfig_list *ttvfig, NameAllocator *NA)
{
  chain_list *chainsig;
  ht *h;
  char buf[1024], *nname;
  ttvsig_list *tvs;
  long l;
  chain_list *cl;
  

  CreateNameAllocator(10000, NA, CASE_SENSITIVE);

  chainsig = ttv_getsigbytype(ttvfig,NULL,TTV_SIG_C|
                              TTV_SIG_Q|
                              TTV_SIG_L|
                              TTV_SIG_B|
                              TTV_SIG_R,NULL) ;
  
  cl=ttv_get_signal_with_directives(ttvfig, 1, 1);
  while (cl!=NULL)
  {
    tvs=(ttvsig_list *)cl->DATA;
    if ((tvs->TYPE & (TTV_SIG_C|TTV_SIG_Q|TTV_SIG_L|TTV_SIG_B|TTV_SIG_R))==0)
       chainsig=addchain(chainsig, tvs);
    cl=delchain(cl, cl);
  }

  h=addht(100000);
  while (chainsig!=NULL)
    {
      tvs=(ttvsig_list *)chainsig->DATA;

      ttv_getnetname(ttvfig, buf, tvs);
      nname=NameAlloc(NA, buf);
      if ((l=gethtitem(h, nname))==EMPTYHT) cl=NULL;
      else cl=(chain_list *)l;

      addhtitem(h, nname, (long)addchain(cl, tvs));

      ttv_getsigname(ttvfig, buf, tvs);
      if (mbk_casestrcmp(nname, buf)!=0 && (nname=namefind(buf))!=NULL)
        addhtitem(h, nname, (long)tvs);

      chainsig=delchain(chainsig, chainsig);
    }
  return h;
}

void ttv_freeequickaccessht_forfalsepath(ht *h, NameAllocator *NA)
{
  chain_list *cl;
  long  nextitem ;
  void *nextkey ;

  scanhtkey( h, 1, &nextkey, &nextitem ) ;
  while( nextitem != EMPTYHT )
  {
    if (NameAllocFind(NA, nextkey)!=NULL)
      freechain( (chain_list *)nextitem ) ;
    scanhtkey( h, 0, &nextkey, &nextitem ) ;
  }
  
  delht(h) ;
}

static int ttv_addfalsepath_entry_in_hash_table(ttvfig_list *ttvfig, long destination, char *sig, long slope, long tag, ht *h, ttvfalsepath_list *pt, int wild, NameAllocator *NA)
{
  nowildcard_info *nwi;
  long l, l1;
  ttvsig_list *ptsig;
  char *signame, buf[1024];
  chain_list *cl;
  ptype_list *ptype;
  
  if ((l=gethtitem(h, sig))!=EMPTYHT || ((signame=NameAllocFind(NA, sig))!=NULL && (l1=gethtitem(h, signame))!=EMPTYHT))
    {
      if (l!=EMPTYHT)
      {
        ttv_getnetname(ttvfig, buf, (ttvsig_list *)l);
        signame=NameAllocFind(NA, buf);
        if (signame!=NULL) l1=gethtitem(h, signame);
        else l1=EMPTYHT;
      }
      if (l1!=EMPTYHT)
      {
        for (cl=(chain_list *)l1; cl!=NULL; cl=cl->NEXT)
        {
          ptsig=(ttvsig_list *)cl->DATA;
          if((slope & TTV_NODE_UP) == TTV_NODE_UP)
            ptsig->NODE[1].TYPE |= tag ;
          if((slope & TTV_NODE_DOWN) == TTV_NODE_DOWN)
            ptsig->NODE[0].TYPE |= tag ;
          
          if ((ptype=getptype(ptsig->USER, destination))==NULL)
            {
              nwi=(nowildcard_info *)mbkalloc(sizeof(nowildcard_info));
              nwi->nb=0;
              nwi->FALSEPATH_NOWILDCARD=nwi->FALSEPATH_WILDCARD=NULL;
              ptsig->USER=addptype(ptsig->USER, destination, nwi);
            }
          else
            {
              nwi=(nowildcard_info *)ptype->DATA;
            }
          nwi->nb++;
          if (wild) nwi->FALSEPATH_WILDCARD=addchain(nwi->FALSEPATH_WILDCARD, pt);
          else nwi->FALSEPATH_NOWILDCARD=addchain(nwi->FALSEPATH_NOWILDCARD, pt);
        }      
        return 1;
      }
    }
  return 0;
}

/*****************************************************************************/
/*                        function ttv_addfalsepath                          */
/* parametres :                                                              */
/* ttvfig : figure                                                           */
/*                                                                           */
/* ajoute un faux chemin dans la liste des faux chemins                      */
/*****************************************************************************/

void ttv_addfalsepath(ttvfig_list *ttvfig, char *sigin, long slopein, char *sigout, long slopeout, char *clock, ptype_list *siglist, ht *nametosig,NameAllocator *NA)
{
 ttvfalsepath_list *pt, **whereptype;
 ttvsig_list *ptsig  ;
 ptype_list *ptype;
 chain_list *chainsig ;
 chain_list *chain ;
 chain_list chains ;
 falsepath_info *fpi;
 int inw, outw;

 pt = (ttvfalsepath_list *)mbkalloc(sizeof(ttvfalsepath_list));

 pt->FIG = ttvfig ;
 pt->SIGIN = sigin ;
 pt->SIGOUT = sigout ;
 pt->SLOPEIN = slopein ;
 pt->SLOPEOUT = slopeout ;
 pt->CLOCK = clock ;
 pt->SIGLIST = siglist ;

 if ((ptype = getptype(ttvfig->USER,TTV_FIG_FALSEPATH))==NULL)
   {
     fpi=(falsepath_info *)mbkalloc(sizeof(falsepath_info));
     fpi->WILDCARDS=fpi->NOWILDCARDS=NULL;
     ttvfig->USER = addptype(ttvfig->USER, TTV_FIG_FALSEPATH, fpi);
   }
 else
   fpi=(falsepath_info *)ptype->DATA;

 if (!mbk_isregex_name(sigin)) inw=0; else inw=1;
 if (!mbk_isregex_name(sigout)) outw=0; else outw=1;
 if (!inw || !outw)
   whereptype=&fpi->NOWILDCARDS;
 else
   whereptype=&fpi->WILDCARDS;

 pt->NEXT = *whereptype;
 *whereptype= pt;

 if (!ttv_addfalsepath_entry_in_hash_table(ttvfig,TTV_SIG_FALSEPATH_START, sigin, slopein, TTV_NODE_FALSEIN, nametosig, pt, outw, NA))
  {
    chains.DATA = sigin ;
    chains.NEXT = NULL ;

    chainsig = ttv_getsigbytype_and_netname(ttvfig,NULL,TTV_SIG_C|
                                TTV_SIG_Q|
                                TTV_SIG_L|
                                TTV_SIG_B|
                                TTV_SIG_R,&chains) ;
    
    chainsig=append(ttv_getmatchingdirectivenodes(ttvfig, &chains, 1), chainsig);

    for(chain = chainsig ; chain != NULL ; chain = chain->NEXT)
      {
        ptsig = (ttvsig_list *)chain->DATA  ;
        
        if((slopein & TTV_NODE_UP) == TTV_NODE_UP)
          ptsig->NODE[1].TYPE |= TTV_NODE_FALSEIN ;
        if((slopein & TTV_NODE_DOWN) == TTV_NODE_DOWN)
          ptsig->NODE[0].TYPE |= TTV_NODE_FALSEIN ;
      }
    
    freechain(chainsig) ;
    if (chainsig==NULL) avt_errmsg(TTV_ERRMSG, "058", AVT_WARNING, "starting", mbk_get_reverse_index_regex(sigin));
  }   

 if (!ttv_addfalsepath_entry_in_hash_table(ttvfig,TTV_SIG_FALSEPATH_END, sigout, slopeout, TTV_NODE_FALSEOUT, nametosig, pt, inw, NA))
  {
     chains.DATA = sigout ;
     chains.NEXT = NULL ;
     
     chainsig = ttv_getsigbytype_and_netname(ttvfig,NULL,TTV_SIG_C|
                                 TTV_SIG_Q|
                                 TTV_SIG_L|
                                 TTV_SIG_B|
                                 TTV_SIG_R,&chains) ;

     chainsig=append(ttv_getmatchingdirectivenodes(ttvfig, &chains, 1), chainsig);

     for(chain = chainsig ; chain != NULL ; chain = chain->NEXT)
       {
         ptsig = (ttvsig_list *)chain->DATA  ;
         
         if((slopeout & TTV_NODE_UP) == TTV_NODE_UP)
           ptsig->NODE[1].TYPE |= TTV_NODE_FALSEOUT ;
         if((slopeout & TTV_NODE_DOWN) == TTV_NODE_DOWN)
           ptsig->NODE[0].TYPE |= TTV_NODE_FALSEOUT ;
       }
     
     freechain(chainsig) ;
     if (chainsig==NULL) avt_errmsg(TTV_ERRMSG, "058", AVT_WARNING, "ending", mbk_get_reverse_index_regex(sigout));
   }
 if (pt->SIGLIST)
 {
  for (ptype=pt->SIGLIST, chain=NULL; ptype!=NULL; ptype=ptype->NEXT)
    if (ptype->DATA!=NULL) chain=addchain(chain, ptype->DATA);
          
  chainsig = ttv_getsigbytype_and_netname(ttvfig,NULL,TTV_SIG_TYPEALL,chain) ;
  freechain(chain);
  while (chainsig!=NULL)
  {
    ptsig = (ttvsig_list *)chainsig->DATA  ;
    ptsig->NODE->TYPE|=TTV_NODE_FALSEPATHNODE;
    (ptsig->NODE+1)->TYPE|=TTV_NODE_FALSEPATHNODE;
    chainsig=delchain(chainsig, chainsig);
  }
 }
}

/*****************************************************************************/
/*                        function ttv_freefalsepath                         */
/* parametres :                                                              */
/* ttvfig : figure                                                           */
/*                                                                           */
/* modelise un lien en fonction de sa resistance et son parametre de front   */
/*****************************************************************************/

static void cleanallnodes(ttvfig_list *ttvfig)
{
  chain_list *chainsig, *chain;
  ttvsig_list *ptsig;

  chainsig = ttv_getsigbytype(ttvfig,NULL,TTV_SIG_C|
                              TTV_SIG_Q|
                              TTV_SIG_L|
                              TTV_SIG_B|
                              TTV_SIG_R,NULL) ;
  
  for(chain = chainsig ; chain != NULL ; chain = chain->NEXT)
    {
      ptsig = (ttvsig_list *)chain->DATA  ;
      ptsig->NODE[1].TYPE &= ~(TTV_NODE_FALSEOUT|TTV_NODE_FALSEIN) ;
      ptsig->NODE[0].TYPE &= ~(TTV_NODE_FALSEOUT|TTV_NODE_FALSEIN) ;
    }
  
  freechain(chainsig) ;
}

void ttv_freefalsepath(ttvfig)
ttvfig_list *ttvfig ;
{
 ptype_list *ptype ;
 falsepath_info *fpi;
 nowildcard_info *nwi;
 chain_list *cl;
 ttvfalsepath_list *fpl, *nfpl;
 ttvsig_list *tvs;
 const long types[2]={TTV_SIG_FALSEPATH_START, TTV_SIG_FALSEPATH_END};
 int i;

 ptype = getptype(ttvfig->USER,TTV_FIG_FALSEPATH) ;

 if(ptype == NULL)
  return ;

 fpi=(falsepath_info *)ptype->DATA;

 
 cl = ttv_getsigbytype_and_netname(ttvfig,NULL,TTV_SIG_TYPEALL,NULL) ;
 while (cl!=NULL)
 {
   tvs=(ttvsig_list *)cl->DATA;
   for (i=0; i<2; i++)
   {
     if ((ptype=getptype(tvs->USER, types[i]))!=NULL)
     {
       nwi=(nowildcard_info *)ptype->DATA;
       freechain(nwi->FALSEPATH_WILDCARD);
       freechain(nwi->FALSEPATH_NOWILDCARD);
       mbkfree(nwi);
       tvs->USER=delptype(tvs->USER, types[i]);
     }
   }
   cl=delchain(cl,cl);
 }

 for (fpl=fpi->WILDCARDS; fpl!=NULL; fpl=nfpl)
  {
    nfpl=fpl->NEXT;
    freeptype(fpl->SIGLIST) ;
    mbkfree(fpl) ;
  }

 for (fpl=fpi->NOWILDCARDS; fpl!=NULL; fpl=nfpl)
  {
    nfpl=fpl->NEXT;
    freeptype(fpl->SIGLIST) ;
    mbkfree(fpl) ;
  }

 mbkfree(fpi);
 cleanallnodes(ttvfig);
 ttvfig->USER = delptype(ttvfig->USER,TTV_FIG_FALSEPATH) ;
}

/*****************************************************************************/
/*                        function ttv_isfalsedetailpath                     */
/* parametres :                                                              */
/* ttvfig : figure                                                           */
/*                                                                           */
/*****************************************************************************/
static inline int ttv_falsepathdetailretcode(ttvfig_list *tvf, ttvevent_list *latch, ttvline_list *line, int onenodemode, int anystartdir)
{
  stbck *ck;
  stbnode *node;
  ttvsig_list *oclock;
  long type;
  ptype_list *cmdlist, *pt, *pt0;
  chain_list *cl;
  ttvevent_list *clockev;
  if (onenodemode)
  {
    if (latch!=NULL && (((line->TYPE & TTV_LINE_RC)!=0 && line->NODE==latch)
        || ((line->TYPE & TTV_LINE_RC)==0 && line->ROOT==latch)))
    {
      cmdlist = ttv_getlatchaccess(tvf,latch,TTV_FIND_MAX) ;
      oclock=NULL;
      type =0;
      // une seule clock?
      for (pt=cmdlist; pt!=NULL; pt=pt->NEXT)
      {
        if ((pt0=getptype(((ttvevent_list *)pt->DATA)->USER, STB_ONE_OR_NO_CLOCK_EVENT))!=NULL)
           cl=(chain_list *)pt0->DATA;
        else
           cl=NULL;
        
        while (cl!=NULL)
        {
          clockev=(ttvevent_list *)cl->DATA;
          if (oclock==NULL || clockev->ROOT==oclock)
           {
             type|=clockev->TYPE;
             oclock=clockev->ROOT;
           }
          else
            break;
          cl=cl->NEXT;
        }
        if (cl!=NULL) break;
      }
      
      if (pt==NULL && ((type & (TTV_NODE_UP|TTV_NODE_DOWN))!=(TTV_NODE_UP|TTV_NODE_DOWN)
                       || anystartdir))
      {
        // tout les chemins seront des false path, inutile de lancer la recherche en parallele
        return 2;
      }
    }
  }
  return 1;
}

static int ttv_isfalsedetailpath(ttvfig_list *ttvfig,ttvevent_list *eventin,ttvevent_list *eventout,long type,ttvfalsepath_list *pt, ttvevent_list *latch,int anystartdir)
{
  ttvevent_list *node, *rcnode ;
  ptype_list *ptype ;
  char buf[1024] ;
  int nothingtriggered, found=0, nexttrigger, onenodemode;
  ttvline_list *line;

  if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
    {
      node = eventout ;
      pt->SIGLIST=(ptype_list *)reverse((chain_list *)pt->SIGLIST);
    }
  else
    {
      node = eventin ;
    }

  ptype = pt->SIGLIST ;
  if (V_BOOL_TAB[__AVT_FALSEPATH_OPTIM].VALUE && latch!=NULL && (latch->ROOT->TYPE & TTV_SIG_L)!=0 && ptype->NEXT==NULL) onenodemode=1;
  else onenodemode=0;
  
  nothingtriggered=0; nexttrigger=0;
  while (ptype!=NULL && ptype->DATA==NULL) {nothingtriggered=1; ptype=ptype->NEXT;}

  if((type & TTV_FIND_DUAL) != TTV_FIND_DUAL && node->FIND->OUTLINE!=NULL && (node->FIND->OUTLINE->TYPE & TTV_LINE_RC)!=0)
    node=node->FIND->OUTLINE->ROOT; // special case RC at input of path

  while(node->FIND->OUTLINE != NULL)
    {
      rcnode=NULL;
      if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
        {
          line=node->FIND->OUTLINE;
          node = node->FIND->OUTLINE->NODE ;
          if (node->FIND->OUTLINE!=NULL && (line->TYPE & TTV_LINE_RC)!=0)
          {
            rcnode=node;
            line=node->FIND->OUTLINE;
            node = node->FIND->OUTLINE->NODE ;
          }
            
          if(node == eventin || rcnode==eventin)
            {
              pt->SIGLIST=(ptype_list *)reverse((chain_list *)pt->SIGLIST);
              if (ptype==NULL) return ttv_falsepathdetailretcode(ttvfig,latch, line, onenodemode,anystartdir);
              return(0) ;
            }
        }
      else
        {
          line=node->FIND->OUTLINE;
          node = node->FIND->OUTLINE->ROOT ;
          if (node->FIND->OUTLINE!=NULL && (node->FIND->OUTLINE->TYPE & TTV_LINE_RC)!=0)
          {
            rcnode=node;
            line=node->FIND->OUTLINE;
            node = node->FIND->OUTLINE->ROOT ;
          }
          if(node == eventout || rcnode==eventout)
            {
              if (ptype==NULL) return ttv_falsepathdetailretcode(ttvfig,latch, line, onenodemode,anystartdir);
              return(0) ;
            }
        }
   
      if (ptype==NULL)
        {
          if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
          {
            pt->SIGLIST=(ptype_list *)reverse((chain_list *)pt->SIGLIST);
            if ((node->FIND->OUTLINE->TYPE & TTV_LINE_RC)!=0 && node->FIND->OUTLINE->NODE==eventin)
              return ttv_falsepathdetailretcode(ttvfig,latch, line, onenodemode,anystartdir); // special case RC at input of path
          }
          return 0;
        }

      found=0;
      if(ttv_canbeinfalsepath(node, 'f') &&
         ((node->TYPE & ptype->TYPE) != (long)0) &&
         (pt->FIG->INFO->LEVEL >= node->ROOT->ROOT->INFO->LEVEL))
        {
          if(ttv_falsepath_match_name(pt->FIG, node->ROOT, ptype->DATA)
             || (rcnode!=NULL && ttv_falsepath_match_name(pt->FIG, rcnode->ROOT, ptype->DATA))
                          )
// mbk_TestREGEX(ttv_getsigname(pt->FIG,buf,node->ROOT), ptype->DATA))
            {
              ptype = ptype->NEXT ;
              if(ptype == NULL)
                {
                  if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
                    pt->SIGLIST=(ptype_list *)reverse((chain_list *)pt->SIGLIST);
                  return ttv_falsepathdetailretcode(ttvfig,latch, line, onenodemode,anystartdir);
                }
              found=1;

              nexttrigger=0;
              while (ptype!=NULL && ptype->DATA==NULL) {nexttrigger=1; ptype=ptype->NEXT;}
            }
        }
      if (!found && nothingtriggered/* && (line->TYPE & TTV_LINE_RC)!=TTV_LINE_RC*/)
        {
          if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
            pt->SIGLIST=(ptype_list *)reverse((chain_list *)pt->SIGLIST);
          return(0) ;
        }
      if (found) nothingtriggered=nexttrigger, nexttrigger=0;
    }
  if((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)
    pt->SIGLIST=(ptype_list *)reverse((chain_list *)pt->SIGLIST);
  return(0) ;
#ifndef __ALL__WARNING_
  ttvfig = NULL;
#endif
}

/*****************************************************************************/
/*                        function ttv_isfalsepath                           */
/* parametres :                                                              */
/* ttvfig : figure                                                           */
/*                                                                           */
/* modelise un lien en fonction de sa resistance et son parametre de front   */
/*****************************************************************************/
static int falsepath_matchhz(long type, ttvfalsepath_list *pt)
{
  if ((type & TTV_FIND_HZ)!=0 && (pt->CLOCK==NULL ||
       strncmp(pt->CLOCK, INF_HZ_STR, 1))) return 1;
  if ((type & TTV_FIND_HZ)==0 && (pt->CLOCK==NULL ||
       strncmp(pt->CLOCK, INF_HZ_STR, 1))) return 1;
  return 0;
}


int ttv_isfalsepath(ttvfig_list *ttvfig,ttvevent_list *eventin,ttvevent_list *eventout,long type, int clocktoclock, ttvevent_list *latch)
{
 ttvfalsepath_list *pt ;
 ttvfig_list *ptfig ;
 ptype_list *ptype ;
 char buf[1024] ;
 falsepath_info *fpi;
 long l,ret;
 int usew;
 nowildcard_info *nwi_start, *nwi_end, *orig_nwi_start, *orig_nwi_end;
 chain_list *cl;

 if((TTV_MARK_MODE & TTV_MARK_MODE_DO)!=0
    || !ttv_canbeinfalsepath(eventin, 'i') ||
    !ttv_canbeinfalsepath(eventout, 'o'))
   return(0) ;

 if((ptype = getptype(ttvfig->USER,TTV_FIG_FALSEPATH)) == NULL)
   return(0) ;

 fpi=(falsepath_info *)ptype->DATA;

 // GESTION DES NON WILDCARDS
 if ((ptype=getptype(eventin->ROOT->USER, TTV_SIG_FALSEPATH_START))!=NULL) nwi_start=(nowildcard_info *)ptype->DATA;
 else nwi_start=NULL;
 if ((ptype=getptype(eventout->ROOT->USER, TTV_SIG_FALSEPATH_END))!=NULL) nwi_end=(nowildcard_info *)ptype->DATA;
 else nwi_end=NULL;

 orig_nwi_start=nwi_start;
 orig_nwi_end=nwi_end;
 l=1; // start
 if (nwi_end!=NULL && nwi_start!=NULL)
   {
     // on grade la liste la plus courte
     usew=0;
     if (nwi_end->nb<nwi_start->nb) nwi_start=nwi_end, l=2;
   }
 else
   {
     usew=1;
     if (nwi_start==NULL) nwi_start=nwi_end, l=2;
   }

 if (nwi_start!=NULL)
   {
     chain_list *tab[2];
     if (usew==0) cl=nwi_start->FALSEPATH_NOWILDCARD;
     else cl=nwi_start->FALSEPATH_WILDCARD;

     while (cl!=NULL)
      {
        pt=(ttvfalsepath_list *)cl->DATA;
        if (!clocktoclock || pt->SIGLIST == NULL)
        {
          if(falsepath_matchhz(type, pt) && (eventin->TYPE & pt->SLOPEIN) != 0 && (eventout->TYPE & pt->SLOPEOUT) !=0)
           {
  /*          if((l==2 && mbk_TestREGEX(ttv_getsigname(pt->FIG,buf, eventin->ROOT),pt->SIGIN)) 
               || (l==1 && mbk_TestREGEX(ttv_getsigname(pt->FIG,buf, eventout->ROOT),pt->SIGOUT)))*/
            if((l==2 && ttv_falsepath_match_name(pt->FIG, eventin->ROOT,pt->SIGIN)) 
               || (l==1 && ttv_falsepath_match_name(pt->FIG,eventout->ROOT,pt->SIGOUT)))
              {
                if(pt->SIGLIST == NULL)
                  return(2) ;
                else if((ret=ttv_isfalsedetailpath(ttvfig,eventin,eventout,type,pt,latch, pt->SLOPEIN==(TTV_NODE_DOWN|TTV_NODE_UP)?1:0))!=0)
                  return(ret) ;
              }
           }
        }
        cl=cl->NEXT;
      }

     if (usew==0)
     {
        if (orig_nwi_start!=NULL) tab[0]=orig_nwi_start->FALSEPATH_WILDCARD; else tab[0]=NULL;
        if (orig_nwi_end!=NULL) tab[1]=orig_nwi_end->FALSEPATH_WILDCARD; else tab[1]=NULL;
        
        for (l=1; l<3; l++)
        {
          cl=tab[l-1];
          while (cl!=NULL)
            {
              pt=(ttvfalsepath_list *)cl->DATA;
              if (!clocktoclock || pt->SIGLIST == NULL)
              {
                 if(falsepath_matchhz(type, pt) && (eventin->TYPE & pt->SLOPEIN) != 0 && (eventout->TYPE & pt->SLOPEOUT) !=0)
                  {
                   if((l==2 && ttv_falsepath_match_name(pt->FIG, eventin->ROOT,pt->SIGIN)) 
                      || (l==1 && ttv_falsepath_match_name(pt->FIG, eventout->ROOT, pt->SIGOUT)))
                     {
                       if(pt->SIGLIST == NULL)
                         return(2) ;
                       else if((ret=ttv_isfalsedetailpath(ttvfig,eventin,eventout,type,pt,latch,pt->SLOPEIN==(TTV_NODE_DOWN|TTV_NODE_UP)?1:0))!=0)
                         return(ret) ;
                     }
                  }
              }
              cl=cl->NEXT;
            }
        }
     }
   }

 // GESTION DES WILDCARDS
 for(pt = fpi->WILDCARDS ; pt != NULL ; pt = pt->NEXT)
   {
    if(((eventin->TYPE & pt->SLOPEIN) == (long)0) ||
       ((eventout->TYPE & pt->SLOPEOUT) == (long)0))
      continue ;

    if (!clocktoclock || pt->SIGLIST == NULL)
    {
      if(falsepath_matchhz(type, pt) && (ttv_falsepath_match_name(pt->FIG, eventin->ROOT, pt->SIGIN)) &&
         (ttv_falsepath_match_name(pt->FIG, eventout->ROOT, pt->SIGOUT)))
        {
         if(pt->SIGLIST == NULL)
           return(2) ;
         else if((ret=ttv_isfalsedetailpath(ttvfig,eventin,eventout,type,pt,latch,pt->SLOPEIN==(TTV_NODE_DOWN|TTV_NODE_UP)?1:0))!=0)
           return ret ;
           
        }
    }
   }

 if (!clocktoclock)
 {
   if((eventin->ROOT->ROOT != ttvfig) && (eventout->ROOT->ROOT != ttvfig))
    {
     for(ptfig = eventin->ROOT->ROOT ; ptfig != ttvfig ; ptfig = ptfig->NEXT)
       {
        ptfig->STATUS |= TTV_STS_MARQUE ;
       }

     for(ptfig = eventout->ROOT->ROOT ; ptfig != ttvfig ; ptfig = ptfig->NEXT)
       {
        if((ptfig->STATUS & TTV_STS_MARQUE) == TTV_STS_MARQUE)
         {
          if((ret=ttv_isfalsepath(ptfig,eventin,eventout,type,clocktoclock,latch))!=0)
            return(ret) ;
         }
       }

     for(ptfig = eventin->ROOT->ROOT ; ptfig != ttvfig ; ptfig = ptfig->NEXT)
       {
        ptfig->STATUS &= ~(TTV_STS_MARQUE) ;
       }
    }
 }

 return(0) ;
}

int ttv_hasaccessfalsepath(ttvfig_list *ttvfig,ttvevent_list *eventout, chain_list *clocks)
{
 ttvfalsepath_list *pt ;
 ptype_list *ptype ;
 falsepath_info *fpi;
 long l;
 nowildcard_info *nwi_end;
 chain_list *cl, *ch;

 if(!ttv_canbeinfalsepath(eventout, 'o'))
   return(0) ;

 if((ptype = getptype(ttvfig->USER,TTV_FIG_FALSEPATH)) == NULL)
   return(0) ;

 fpi=(falsepath_info *)ptype->DATA;

 // GESTION DES NON WILDCARDS
 if ((ptype=getptype(eventout->ROOT->USER, TTV_SIG_FALSEPATH_END))!=NULL) nwi_end=(nowildcard_info *)ptype->DATA;
 else nwi_end=NULL;

 if (nwi_end!=NULL)
   {
     cl=nwi_end->FALSEPATH_NOWILDCARD;
     while (cl!=NULL)
      {
        pt=(ttvfalsepath_list *)cl->DATA;
          if((eventout->TYPE & pt->SLOPEOUT) !=0)
           {
            for (ch=clocks; ch!=NULL; ch=ch->NEXT)
             if(ttv_falsepath_match_name(pt->FIG, (ttvsig_list *)ch->DATA,pt->SIGIN)) return 1;
           }
        cl=cl->NEXT;
      }

     cl=nwi_end->FALSEPATH_WILDCARD;
     while (cl!=NULL)
       {
         pt=(ttvfalsepath_list *)cl->DATA;
         if((eventout->TYPE & pt->SLOPEOUT) !=0)
          {
            for (ch=clocks; ch!=NULL; ch=ch->NEXT)
             if(ttv_falsepath_match_name(pt->FIG, (ttvsig_list *)ch->DATA,pt->SIGIN)) return 1;
          }
         cl=cl->NEXT;
       }
   }

 // GESTION DES WILDCARDS
 for(pt = fpi->WILDCARDS ; pt != NULL ; pt = pt->NEXT)
   {
     if((eventout->TYPE & pt->SLOPEOUT) !=0 && ttv_falsepath_match_name(pt->FIG, eventout->ROOT, pt->SIGOUT))
      {
        for (ch=clocks; ch!=NULL; ch=ch->NEXT)
         if(ttv_falsepath_match_name(pt->FIG, (ttvsig_list *)ch->DATA,pt->SIGIN)) return 1;
      }
   }

 return(0) ;
}

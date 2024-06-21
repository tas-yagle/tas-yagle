#include <stdlib.h>

#include MUT_H
#include STB_H
#include "stb_falseslack.h"


typedef struct stbfalsepath               
    {
     struct stbfalsepath *NEXT ;
     char *startclock;
     char *endclock;
     char *startsig;
     char *endsig;
     char startclock_dir;
     char endclock_dir;
     char startsig_dir;
     char endsig_dir;
     char setuphold;
    }
stbfalseslack_list ;

typedef struct
{
  stbfalseslack_list *WILDCARDS, *NOWILDCARDS;
  ht *START_HASH, *END_HASH;
} falseslack_info;

typedef struct
{
  int nb;
  chain_list *FALSESLACK_WILDCARD, *FALSESLACK_NOWILDCARD;
} nowildcard_info;

static int stb_falseslack_match_name(ttvfig_list *tvf, ttvsig_list *tvs, char *regex)
{
  char buf[1024];
  if (mbk_TestREGEX(ttv_getsigname(tvf, buf, tvs),regex)) return 1;
  if (mbk_TestREGEX(ttv_getnetname(tvf, buf, tvs),regex)) return 1;
  return 0;
}

static inline void *getkey_forsignal(ttvsig_list *tvs)
{
  return tvs;
}

ht *stb_buildquickaccessht_forfalsepath(ttvfig_list *ttvfig, NameAllocator *NA)
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

void stb_freeequickaccessht_forfalsepath(ht *h, NameAllocator *NA)
{
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

static int stb_addfalsepath_entry_in_hash_table(ttvfig_list *ttvfig, ht *destination, char *sig, ht *h, stbfalseslack_list *pt, int wild, NameAllocator *NA)
{
  nowildcard_info *nwi;
  long l, l1;
  ttvsig_list *ptsig;
  char *signame, buf[1024];
  chain_list *cl;
  
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
          
          if ((l=gethtitem(destination, getkey_forsignal(ptsig)))==EMPTYHT)
            {
              nwi=(nowildcard_info *)mbkalloc(sizeof(nowildcard_info));
              nwi->nb=0;
              nwi->FALSESLACK_NOWILDCARD=nwi->FALSESLACK_WILDCARD=NULL;
              addhtitem(destination, getkey_forsignal(ptsig), (long)nwi);
            }
          else
            {
              nwi=(nowildcard_info *)l;
            }
          nwi->nb++;
          if (wild) nwi->FALSESLACK_WILDCARD=addchain(nwi->FALSESLACK_WILDCARD, pt);
          else nwi->FALSESLACK_NOWILDCARD=addchain(nwi->FALSESLACK_NOWILDCARD, pt);
        }      
        return 1;
      }
      else avt_errmsg(STB_ERRMSG, "042", AVT_WARNING, mbk_get_reverse_index_regex(sig));
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
void stb_addfalsepath(stbfig_list *stbfig, ptype_list *siglist, ht *nametosig,NameAllocator *NA)
{
 stbfalseslack_list *pt, **whereptype;
 ptype_list *ptype;
 falseslack_info *fpi;
 int inw, outw;

 pt = (stbfalseslack_list *)mbkalloc(sizeof(stbfalseslack_list));

 pt->setuphold=((char)siglist->TYPE) & (INF_FALSESLACK_SETUP|INF_FALSESLACK_HOLD|INF_FALSESLACK_LATCH);
 pt->startclock=mbk_index_regex((char *)siglist->DATA);
 pt->startclock_dir=(char)siglist->TYPE;
 siglist=siglist->NEXT;
 pt->startsig=mbk_index_regex((char *)siglist->DATA);
 pt->startsig_dir=(char)siglist->TYPE;
 siglist=siglist->NEXT;
 pt->endsig=mbk_index_regex((char *)siglist->DATA);
 pt->endsig_dir=(char)siglist->TYPE;
 siglist=siglist->NEXT;
 pt->endclock=mbk_index_regex((char *)siglist->DATA);
 pt->endclock_dir=(char)siglist->TYPE;

 if ((ptype = getptype(stbfig->USER,STB_FIG_FALSESLACK))==NULL)
   {
     fpi=(falseslack_info *)mbkalloc(sizeof(falseslack_info));
     fpi->WILDCARDS=fpi->NOWILDCARDS=NULL;
     fpi->START_HASH=addht(100);
     fpi->END_HASH=addht(100);
     stbfig->USER = addptype(stbfig->USER, STB_FIG_FALSESLACK, fpi);
   }
 else
   fpi=(falseslack_info *)ptype->DATA;

 if (!mbk_isregex_name(pt->startsig)) inw=0; else inw=1;
 if (!mbk_isregex_name(pt->endsig)) outw=0; else outw=1;
 if (!inw || !outw)
   whereptype=&fpi->NOWILDCARDS;
 else
   whereptype=&fpi->WILDCARDS;

 pt->NEXT = *whereptype;
 *whereptype= pt;

 if (!stb_addfalsepath_entry_in_hash_table(stbfig->FIG,fpi->START_HASH, pt->startsig, nametosig, pt, outw, NA))
  {
  }   

 if (!stb_addfalsepath_entry_in_hash_table(stbfig->FIG,fpi->END_HASH, pt->endsig, nametosig, pt, inw, NA))
  {
  }
}

/*****************************************************************************/
/*                        function ttv_freefalsepath                         */
/* parametres :                                                              */
/* ttvfig : figure                                                           */
/*                                                                           */
/* modelise un lien en fonction de sa resistance et son parametre de front   */
/*****************************************************************************/

void stb_freefalseslack(stbfig_list *stbfig)
{
 ptype_list *ptype ;
 falseslack_info *fpi;
 nowildcard_info *nwi;
 chain_list *cl;
 stbfalseslack_list *fpl, *nfpl;

 ptype = getptype(stbfig->USER,STB_FIG_FALSESLACK) ;

 if(ptype == NULL)
  return ;

 fpi=(falseslack_info *)ptype->DATA;
 cl=GetAllHTElems(fpi->START_HASH);
 while (cl!=NULL)
   {
     nwi=(nowildcard_info *)cl->DATA;
     freechain(nwi->FALSESLACK_WILDCARD);
     freechain(nwi->FALSESLACK_NOWILDCARD);
     mbkfree(nwi);
     cl=delchain(cl, cl);
   }
 delht(fpi->START_HASH);

 cl=GetAllHTElems(fpi->END_HASH);
 while (cl!=NULL)
   {
     nwi=(nowildcard_info *)cl->DATA;
     freechain(nwi->FALSESLACK_WILDCARD);
     freechain(nwi->FALSESLACK_NOWILDCARD);
     mbkfree(nwi);
     cl=delchain(cl, cl);
   }
 delht(fpi->END_HASH);

 for (fpl=fpi->WILDCARDS; fpl!=NULL; fpl=nfpl)
  {
    nfpl=fpl->NEXT;
    mbkfree(fpl) ;
  }

 for (fpl=fpi->NOWILDCARDS; fpl!=NULL; fpl=nfpl)
  {
    nfpl=fpl->NEXT;
    mbkfree(fpl) ;
  }

 mbkfree(fpi);
 stbfig->USER = delptype(stbfig->USER,STB_FIG_FALSESLACK) ;
}

static int stb_sameevent(char ev, long type)
{
   if ((ev & INF_FALSESLACK_UP)!=0 && (type & TTV_NODE_UP)!=0) return 1;
   if ((ev & INF_FALSESLACK_DOWN)!=0 && (type & TTV_NODE_UP)==0) return 1;
   return 0;
}

static int check_goodtype(int type, ttvevent_list *eventout)
{
 if ((type & (INF_FALSESLACK_LATCH|INF_FALSESLACK_PRECH))==0) return 1;
 if ((type & INF_FALSESLACK_LATCH)!=0 && (eventout->ROOT->TYPE & TTV_SIG_L)!=0) return 1;
 if ((type & INF_FALSESLACK_PRECH)!=0 && (eventout->ROOT->TYPE & TTV_SIG_R)!=0) return 1;
 return 0;
}

int stb_isfalseslack(stbfig_list *stbfig, ttvevent_list *startck, ttvevent_list *eventin, ttvevent_list *eventout, ttvevent_list *endck, int type)
{
 stbfalseslack_list *pt ;
 ptype_list *ptype ;
 falseslack_info *fpi;
 long l;
 int usew;
 nowildcard_info *nwi_start, *nwi_end, *orig_nwi_start, *orig_nwi_end;
 chain_list *cl;
 int res=0, maxres;

 if((ptype = getptype(stbfig->USER,STB_FIG_FALSESLACK)) == NULL)
   return(0) ;

 fpi=(falseslack_info *)ptype->DATA;

 // GESTION DES NON WILDCARDS
 if ((l=gethtitem(fpi->START_HASH, getkey_forsignal(eventin->ROOT)))!=EMPTYHT) nwi_start=(nowildcard_info *)l;
 else nwi_start=NULL;
 if ((l=gethtitem(fpi->END_HASH, getkey_forsignal(eventout->ROOT)))!=EMPTYHT) nwi_end=(nowildcard_info *)l;
 else nwi_end=NULL;

 if ((eventout->ROOT->TYPE & TTV_SIG_R)!=0 || (eventout->ROOT->TYPE & TTV_SIG_CT)==TTV_SIG_CT) maxres=INF_FALSESLACK_NOTHZ|INF_FALSESLACK_HZ;
 else maxres=INF_FALSESLACK_NOTHZ;
 
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
     if (usew==0) cl=nwi_start->FALSESLACK_NOWILDCARD;
     else cl=nwi_start->FALSESLACK_WILDCARD;

     while (cl!=NULL)
      {
        pt=(stbfalseslack_list *)cl->DATA;
        if ((pt->setuphold & type)!=0 && check_goodtype(pt->setuphold,eventout))
        {
          if(stb_sameevent(pt->startsig_dir, eventin->TYPE) && stb_sameevent(pt->endsig_dir, eventout->TYPE) &&
             (startck==NULL || stb_sameevent(pt->startclock_dir, startck->TYPE)) &&
             (endck==NULL || stb_sameevent(pt->endclock_dir, endck->TYPE)))
           {
            if(((l==2 && stb_falseslack_match_name(stbfig->FIG, eventin->ROOT, pt->startsig)) 
               || (l==1 && stb_falseslack_match_name(stbfig->FIG, eventout->ROOT,pt->endsig)))
               && (startck==NULL || stb_falseslack_match_name(stbfig->FIG, startck->ROOT,pt->startclock))
               && (endck==NULL || stb_falseslack_match_name(stbfig->FIG, endck->ROOT,pt->endclock)))
              {
                 res|=(pt->endsig_dir & (INF_FALSESLACK_HZ|INF_FALSESLACK_NOTHZ));
                 if ((res & maxres)==maxres) return res ;
              }
           }
        }
        cl=cl->NEXT;
      }

     if (usew==0)
     {
        if (orig_nwi_start!=NULL) tab[0]=orig_nwi_start->FALSESLACK_WILDCARD; else tab[0]=NULL;
        if (orig_nwi_end!=NULL) tab[1]=orig_nwi_end->FALSESLACK_WILDCARD; else tab[1]=NULL;
        
        for (l=1; l<3; l++)
        {
          cl=tab[l-1];
          while (cl!=NULL)
            {
              pt=(stbfalseslack_list *)cl->DATA;
              if ((pt->setuphold & type)!=0 && check_goodtype(pt->setuphold,eventout))
              {
                if(stb_sameevent(pt->startsig_dir, eventin->TYPE) && stb_sameevent(pt->endsig_dir, eventout->TYPE) &&
                   (startck==NULL || stb_sameevent(pt->startclock_dir, startck->TYPE)) &&
                   (endck==NULL || stb_sameevent(pt->endclock_dir, endck->TYPE)))
                 {
                  if(((l==2 && stb_falseslack_match_name(stbfig->FIG, eventin->ROOT, pt->startsig)) 
                     || (l==1 && stb_falseslack_match_name(stbfig->FIG, eventout->ROOT,pt->endsig)))
                     && (startck==NULL || stb_falseslack_match_name(stbfig->FIG, startck->ROOT,pt->startclock))
                     && (endck==NULL || stb_falseslack_match_name(stbfig->FIG, endck->ROOT,pt->endclock)))
                    {
                       res|=(pt->endsig_dir & (INF_FALSESLACK_HZ|INF_FALSESLACK_NOTHZ));
                       if ((res & maxres)==maxres) return res ;
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
     if ((pt->setuphold & type)!=0 && check_goodtype(pt->setuphold,eventout))
     {
       if(stb_sameevent(pt->startsig_dir, eventin->TYPE) && stb_sameevent(pt->endsig_dir, eventout->TYPE) &&
           (startck==NULL || stb_sameevent(pt->startclock_dir, startck->TYPE)) &&
           (endck==NULL || stb_sameevent(pt->endclock_dir, endck->TYPE)))
         {
          if(stb_falseslack_match_name(stbfig->FIG, eventin->ROOT, pt->startsig) 
             && stb_falseslack_match_name(stbfig->FIG, eventout->ROOT,pt->endsig)
             && (startck==NULL || stb_falseslack_match_name(stbfig->FIG, startck->ROOT,pt->startclock))
             && (endck==NULL || stb_falseslack_match_name(stbfig->FIG, endck->ROOT,pt->endclock)))
           {
              res|=(pt->endsig_dir & (INF_FALSESLACK_HZ|INF_FALSESLACK_NOTHZ));
              if ((res & maxres)==maxres) return res ;
           }
         }
     }
   }

 return res ;
}

void stb_setfalseslack(stbfig_list *sf, inffig_list *ifl)
{
  chain_list *ch;
  ptype_list *p;
  ht *nametosig;
  NameAllocator NA;

  if (ifl!=NULL && ifl->LOADED.INF_FALSESLACK!=NULL)
  {
    nametosig=stb_buildquickaccessht_forfalsepath(sf->FIG, &NA);
    
    for (ch = ifl->LOADED.INF_FALSESLACK; ch; ch = ch->NEXT)
    {
      p = (ptype_list *) ch->DATA;
      stb_addfalsepath(sf, p, nametosig, &NA);
    }
    stb_freeequickaccessht_forfalsepath(nametosig, &NA);
    DeleteNameAllocator(&NA);
  }
}

int stb_hasfalseslack(stbfig_list *stbfig, ttvevent_list *eventout)
{
 stbfalseslack_list *pt ;
 ptype_list *ptype ;
 falseslack_info *fpi;
 long l;
 int res=0, maxres;

 if((ptype = getptype(stbfig->USER,STB_FIG_FALSESLACK)) == NULL) return(0) ;

 fpi=(falseslack_info *)ptype->DATA;

 if ((l=gethtitem(fpi->END_HASH, getkey_forsignal(eventout->ROOT)))!=EMPTYHT) return 1;

 for(pt = fpi->WILDCARDS ; pt != NULL ; pt = pt->NEXT)
   {
     if(stb_sameevent(pt->endsig_dir, eventout->TYPE))
       {
        if(stb_falseslack_match_name(stbfig->FIG, eventout->ROOT,pt->endsig)) return 1;
       }
   }

 return 0;
}


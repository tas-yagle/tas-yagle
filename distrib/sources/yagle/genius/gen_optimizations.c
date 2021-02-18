
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include MUT_H
#include MLO_H
#include RCN_H
#include FCL_H
#include CNS_H
#include MSL_H
#include API_H

#include "gen_env.h"
#include "gen_execute_VHDL.h"
#include "gen_model_utils.h"
#include "gen_model_transistor.h"
#include "gen_corresp.h"
#include "gen_search_utils.h"
#include "gen_symmetric.h"
#include "gen_optimizations.h"


typedef struct LofigChainUpgrade
{
  chain_list *Mine;
  void **Previous;
} LofigChainUpgrade;

HeapAlloc lcu_heap;

void LofigChainUpgradeHeap_Manage(int mode)
{
  if (mode==0)
    CreateHeap(sizeof(LofigChainUpgrade), 0, &lcu_heap);
  else
    DeleteHeap(&lcu_heap);
}



int SameRadical(char *a, char *b)
{
  int i=0;
  while (a[i]!='\0' && b[i]!='\0' && a[i]!=' ' && b[i]!=' ' && a[i]==b[i]) i++;
  if (a[i]==b[i] && (a[i]=='\0' || a[i]==' ')) return 1;
  if ((a[i]=='\0' && b[i]==' ') || (a[i]==' ' && b[i]=='\0')) return 1;
  return 0;
}


// acceleration des operation sur lofig chain

char *ccname(locon_list *lc)
{
  char n[1000];
  if (lc->TYPE=='T')
    {
      sprintf(n,"%s.%s",((lotrs_list *)lc->ROOT)->TRNAME!=NULL?((lotrs_list *)lc->ROOT)->TRNAME:"??",lc->NAME);
    }
  else
    if (lc->TYPE=='I')
      sprintf(n,"%s.%s",((loins_list *)lc->ROOT)->INSNAME,lc->NAME);
  else
    sprintf(n,"%s",lc->NAME);
  return namealloc(n);
}
void add_pending_con(locon_list *lc, chain_list *cl)
{
  ptype_list *p;

  if (lc->FLAGS==0) return;
  if ((p=getptype(lc->USER, GENIUS_CON_LOFIG_PTYPE))==NULL) 
    {
      p=lc->USER=addptype(lc->USER, GENIUS_CON_LOFIG_PTYPE, NULL);
    }
  p->DATA=addchain((chain_list *)p->DATA, cl);
}

void remove_pending_con(locon_list *lc)
{
  chain_list *cl;
  ptype_list *p;
  if (lc->FLAGS==0) return;
  if ((p=getptype(lc->USER, GENIUS_CON_LOFIG_PTYPE))==NULL) EXIT(57);
  
  cl=(chain_list *)p->DATA;
  p->DATA=cl->NEXT;
  cl->NEXT=NULL;
  freechain(cl);

}

void exchange_pending_con(locon_list *lc0, locon_list *lc1)
{
  chain_list *cl0, *cl1, *cl;
  ptype_list *p0, *p1;

  if ((p0=getptype(lc0->USER, GENIUS_CON_LOFIG_PTYPE))!=NULL)
    cl0=(chain_list *)p0->DATA;
  else
    cl0=NULL;
  if ((p1=getptype(lc1->USER, GENIUS_CON_LOFIG_PTYPE))!=NULL)
    cl1=(chain_list *)p1->DATA;
  else
    cl1=NULL;

  for (cl=cl0; cl!=NULL; cl=cl->NEXT)
    ((chain_list *)cl->DATA)->DATA=lc1;
  
  for (cl=cl1; cl!=NULL; cl=cl->NEXT)
    ((chain_list *)cl->DATA)->DATA=lc0;

}

chain_list *ALL_FAST_LOFIGCHAIN=NULL;

#define ISHEAD(val) (((val) & 1)!=0)
#define UNHEAD(val) ((val) & ~(long)1)
#define MAKEHEAD(val) ((val) | (long)1)

void checklofig(losig_list *ls)
{
  ptype_list *p, *p1;
  chain_list *cl, *pred, *ch;
  locon_list *lc;

  if ((p=getptype(ls->USER,FAST_LOFIGCHAIN_PTYPE))!=NULL)
    {
      p=getptype(ls->USER,LOFIGCHAIN);

      pred=(chain_list *)MAKEHEAD((long)&p->DATA); // tres dangereux : alignement sur 1 octet
      for (cl=(chain_list *)p->DATA; cl!=NULL; pred=cl, cl=cl->NEXT)
       {
         lc=(locon_list *)cl->DATA;
         p1=getptype(lc->USER, FAST_LOFIGCHAIN_PTYPE);
         if (p1->DATA!=pred) 
           {
             avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 32);
          /*printf("(1)lofigchain test failed, pred(%s)!=%p (%p)\n",ccname(lc), pred, p1->DATA);
             printf("%s", (char *)0x1);*/
             EXIT(44);
           }
         if (cl==p->DATA)
           ch=*(chain_list **)UNHEAD((long)pred);
         else
           ch=((chain_list *)pred)->NEXT;
         if (ch->DATA!=lc)
           {
             avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 33);
/*             printf("(2)lofigchain test failed, pred(%s)!=%p (%p), ch=%p\n",ccname(lc), pred, p1->DATA,ch);
             printf("%s", (char *)0x1);*/
             EXIT(43);
           }
       }
    }
}



void createlofigchainhash(losig_list *ls)
{
  ptype_list *p;
  chain_list *cl, *pred;
  locon_list *lc;

  if ((p=getptype(ls->USER,FAST_LOFIGCHAIN_PTYPE))==NULL)
    {
      p=getptype(ls->USER,LOFIGCHAIN);

      pred=(chain_list *)MAKEHEAD((long)&p->DATA); // tres dangereux : alignement sur 1 octet

      for (cl=(chain_list *)p->DATA; cl!=NULL; pred=cl, cl=cl->NEXT)
       {
         lc=(locon_list *)cl->DATA;
         lc->USER=addptype(lc->USER, FAST_LOFIGCHAIN_PTYPE, (void *)pred);
       }

      ls->USER=addptype(ls->USER, FAST_LOFIGCHAIN_PTYPE, 0);
      ALL_FAST_LOFIGCHAIN=addchain(ALL_FAST_LOFIGCHAIN, ls);
    }
}

void cleanalllofigchainhash()
{
  chain_list *cl, *ch;
  losig_list *ls;
  ptype_list *p;
  locon_list *lc;
  for (cl=ALL_FAST_LOFIGCHAIN; cl!=NULL; cl=cl->NEXT)
    {
      ls=(losig_list *)cl->DATA;
      ls->USER=delptype(ls->USER, FAST_LOFIGCHAIN_PTYPE);
      if (ls->USER==NULL) EXIT(72);
      p=getptype(ls->USER,LOFIGCHAIN);
      for (ch=(chain_list *)p->DATA; ch!=NULL; ch=ch->NEXT)
       {
         lc=(locon_list *)ch->DATA;
         if (getptype(lc->USER, FAST_LOFIGCHAIN_PTYPE)==NULL)
           EXIT(71);
         lc->USER=delptype(lc->USER, FAST_LOFIGCHAIN_PTYPE);
       }
    }
  freechain(ALL_FAST_LOFIGCHAIN);
  ALL_FAST_LOFIGCHAIN=NULL;
}

int fastremovelofigchainconnector(losig_list *ls, locon_list *lc)
{
  ptype_list *p, *p1=NULL;
  chain_list **cl, *next, *prev;
  long item;

  if ((p=getptype(ls->USER,FAST_LOFIGCHAIN_PTYPE))!=NULL)
    {
      item=(long)getptype(lc->USER, FAST_LOFIGCHAIN_PTYPE)->DATA;
      if (ISHEAD(item)) 
       {
         cl=(chain_list **)UNHEAD(item);
         next=(chain_list *)(*cl);
         *(chain_list **)cl=next->NEXT;
       }
      else
       {
         prev=(chain_list *)item;
         next=prev->NEXT; // it's <lc>
         prev->NEXT=prev->NEXT->NEXT;
       }
      
      if (next->NEXT!=NULL) p1=getptype(((locon_list *)next->NEXT->DATA)->USER, FAST_LOFIGCHAIN_PTYPE);
      next->NEXT=NULL;
      freechain(next);
      if (p1!=NULL) p1->DATA=(void *)item;
      return 1;
    }
  return 0;
}

int losigisspedup(losig_list *ls)
{
  return getptype(ls->USER,FAST_LOFIGCHAIN_PTYPE)!=NULL;
}

void **getpred(locon_list *lc)
{
  ptype_list *p;
  p=getptype(lc->USER, FAST_LOFIGCHAIN_PTYPE);
  return (void **)&p->DATA;
}

void fastremovestaycoherent(chain_list *me, void *pred, void **head)
{
  ptype_list *p;

  if (me->NEXT==NULL) return;
  p=getptype(((locon_list *)me->NEXT->DATA)->USER, FAST_LOFIGCHAIN_PTYPE);
  if (pred==NULL) p->DATA=(void *)MAKEHEAD((long)head);
  else p->DATA=pred;

}

void fastswaplofigchain(losig_list *ls, locon_list *lc, locon_list *newcl, int nooptim)
{
  ptype_list *p, *p1;
  chain_list *cl;
  long item;

  int count;

  if ((p=getptype(ls->USER,FAST_LOFIGCHAIN_PTYPE))!=NULL)
    {
      p=getptype(ls->USER,LOFIGCHAIN);
      if (p->DATA!=NULL) // on ne sait jamais
       {
         // newcl and cl USER have been swapped
         p1=getptype(newcl->USER, FAST_LOFIGCHAIN_PTYPE);

         item=(long)p1->DATA;
         if (ISHEAD(item)) 
           {
             cl=*(chain_list **)UNHEAD(item);
             if (cl->DATA!=lc) 
              {
          avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 34);
//                printf("%s found instead of %s\n",ccname(cl->DATA), ccname(lc));
                EXIT(68);
              }
             cl->DATA=newcl;
           }
         else 
           {
             cl=(chain_list *)item;
             if (cl->NEXT->DATA!=lc) 
              {
          avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 35);
//                printf("%s found instead of %s\n",ccname(cl->NEXT->DATA), ccname(lc));
                EXIT(69);
              }
             cl->NEXT->DATA=newcl;
           }

//         checklofig(ls);
         return;
       }
    }

  p=getptype(ls->USER,LOFIGCHAIN);
  for (cl=(chain_list *)p->DATA, count=0;cl!=NULL && cl->DATA!=lc; cl=cl->NEXT, count++) ;
  if (cl==NULL) //EXIT(56);
          avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 36);
  cl->DATA=newcl;

  if (nooptim==0 && count>50)
    {
      createlofigchainhash(ls);
    }

}

int fastaddlofigchainconnector(losig_list *ls, locon_list *lc)
{
  ptype_list *p, *p1;
  locon_list *prems;

  if (ls==NULL) return 0;
  if ((p=getptype(ls->USER,FAST_LOFIGCHAIN_PTYPE))!=NULL)
    {
      p=getptype(ls->USER,LOFIGCHAIN);
      if (p->DATA!=NULL) // on ne sait jamais
       {
         prems=(locon_list *)((chain_list *)p->DATA)->DATA;
         p1=getptype(prems->USER, FAST_LOFIGCHAIN_PTYPE);

         // a enlever
         if (!ISHEAD((long)p1->DATA)) //EXIT(65);
          avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 37);

         p->DATA=addchain((chain_list *)p->DATA, lc);
         lc->USER=addptype(lc->USER, FAST_LOFIGCHAIN_PTYPE, p1->DATA);
         p1->DATA=p->DATA;
//         checklofig(ls);

         return 1;
       }
    }
  p=getptype(ls->USER,LOFIGCHAIN);
  p->DATA=addchain((chain_list *)p->DATA, lc);
  return 0;
}

// optimization des match failed en reordonnant les lofigchain

static ht *OFTEN_FAILING_SIGNALS=NULL;
static chain_list *ALL_FAILING_SIGNALS=NULL;

typedef struct
{
  locon_list *lastunmatched;
  int count;
  int pass;
  int bestcountfailed, countfailed;
} statistics;

HeapAlloc stat_heap;

void StatHeap_Manage(int mode)
{
  if (mode==0)
    CreateHeap(sizeof(statistics), 0, &stat_heap);
  else
    DeleteHeap(&stat_heap);
}

void SignalStatisticsRemove(losig_list *ls, locon_list *lc)
{
  long item;
  statistics *stat;
  if (OFTEN_FAILING_SIGNALS==NULL) return;

  if ((item=gethtitem(OFTEN_FAILING_SIGNALS, ls))==EMPTYHT) return;

  stat=(statistics *)item;
  if (stat->lastunmatched==lc)
    stat->lastunmatched=NULL;
}

void UpdateSignalStatistics(losig_list *ls, locon_list *lc, int mode)
{
  long item;
  statistics *stat;

  if (lc==NULL) return;

  if (OFTEN_FAILING_SIGNALS==NULL)
    OFTEN_FAILING_SIGNALS=addht(4093);

  if ((item=gethtitem(OFTEN_FAILING_SIGNALS, ls))!=EMPTYHT)
    {
      stat=(statistics *)item;
    }
  else
    {
      stat=(statistics *)AddHeapItem(&stat_heap);
      stat->lastunmatched=lc;
      stat->count=0;
      stat->countfailed=0;
      stat->bestcountfailed=0;
      ALL_FAILING_SIGNALS=addchain(ALL_FAILING_SIGNALS, ls);
      addhtitem(OFTEN_FAILING_SIGNALS, ls, (long)stat);
      stat->pass=0;
    }
    
  if (mode==STAT_COUNT_SUCESS || mode==STAT_FORCE_CHANGE)
    {
      if (mode==STAT_FORCE_CHANGE)
       {
         stat->lastunmatched=lc;
         stat->count=1000;
         stat->pass=0;
       }
      else
       {
         if (stat->lastunmatched==NULL || lc==stat->lastunmatched)
           {
             stat->count++;
             stat->lastunmatched=lc;
             stat->pass=0;
           }
         if (stat->countfailed>=stat->bestcountfailed)
           {
             stat->count++;
             stat->bestcountfailed=stat->countfailed;
           }
       }
      stat->countfailed=0;
      stat->pass=0;
    }
  else
    {
      stat->countfailed++;
    }
}

void ProcessSignalStatistics()
{
  chain_list *cl, *pred, *next;
  chain_list *ch, *chcon, *tmp;
  long item;
  losig_list *ls;
  statistics *stat;
  ptype_list *p;
  int wasfast, count;
  void **preds;

  if (OFTEN_FAILING_SIGNALS==NULL) return;
  for (cl=ALL_FAILING_SIGNALS, pred=NULL; cl!=NULL; cl=next)
    {
      next=cl->NEXT;
      ls=(losig_list *)cl->DATA;
      item=gethtitem(OFTEN_FAILING_SIGNALS, ls);
      stat=(statistics *)item;

      stat->pass++;
      
      if (stat->count>10)
       {
         chcon=NULL;
         p=getptype(ls->USER, LOFIGCHAIN);
         ch=(chain_list *)p->DATA;
         for (count=0, ch=(chain_list *)p->DATA; ch!=NULL && ch->NEXT!=NULL; ch=ch->NEXT, count++)
           {
             if (ch->NEXT->DATA==stat->lastunmatched) chcon=ch;
           }
         
         if (ch!=NULL && ch->DATA!=stat->lastunmatched) // si ce n'est pas le dernier
           {
             if (p->DATA!=NULL && ((chain_list *)p->DATA)->DATA==stat->lastunmatched) 
              // si c'est le premier
              {
                // first in the list
                wasfast=losigisspedup(ls);
                if (wasfast) { preds=getpred((locon_list *)((chain_list *)p->DATA)->DATA); *preds=ch; } 
                ch->NEXT= p->DATA;
                tmp=(chain_list *)p->DATA;
                if (wasfast) { preds=getpred((locon_list *)tmp->NEXT->DATA); *preds=(void *)MAKEHEAD((long)&p->DATA); } 
                p->DATA=tmp->NEXT;
                ch->NEXT->NEXT=NULL;
              }
             else
              if (chcon!=NULL)
                {
                  wasfast=losigisspedup(ls);
                  //wasfast=updatefastlofigchain(ls);
                  if (wasfast) { preds=getpred((locon_list *)((chain_list *)p->DATA)->DATA); *preds=ch; } 
                  ch->NEXT=(chain_list *)p->DATA;
                  tmp=chcon->NEXT->NEXT;
                  if (wasfast) { preds=getpred((locon_list *)tmp->DATA); *preds=(void *)MAKEHEAD((long)&p->DATA); } 
                  p->DATA=tmp;
                  chcon->NEXT->NEXT=NULL;
                }
           }
         stat->pass=1;
         stat->count=0;
         stat->lastunmatched=NULL;
         stat->bestcountfailed=0;
         pred=cl;
       }
      else
       if (stat->pass>15)
         {
           // too old, remove
           DelHeapItem(&stat_heap, stat); 
           delhtitem(OFTEN_FAILING_SIGNALS, ls);
           if (gethtitem(OFTEN_FAILING_SIGNALS, ls)!=EMPTYHT)
             avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 38); //EXIT(59);
           if (pred==NULL) ALL_FAILING_SIGNALS=cl->NEXT;
           else pred->NEXT=cl->NEXT;
           cl->NEXT=NULL;
           freechain(cl);
         }
       else 
         {
           if (stat->bestcountfailed>stat->countfailed)
             stat->bestcountfailed-=(stat->bestcountfailed-stat->countfailed)/5;
           pred=cl;
         }

    }
}

void EraseSignalStatistics()
{
  chain_list *cl;
  long item;
  losig_list *ls;
  statistics *stat;

  if (OFTEN_FAILING_SIGNALS==NULL || ALL_FAILING_SIGNALS==NULL) return;

  for (cl=ALL_FAILING_SIGNALS; cl!=NULL; cl=cl->NEXT)
    {
      ls=(losig_list *)cl->DATA;
      item=gethtitem(OFTEN_FAILING_SIGNALS, ls);
      stat=(statistics *)item;
      delhtitem(OFTEN_FAILING_SIGNALS, ls);
      DelHeapItem(&stat_heap, stat); 
    }
  freechain(ALL_FAILING_SIGNALS);
  ALL_FAILING_SIGNALS=NULL;
}

chain_list *RemoveLofigChainLocon(chain_list *cl, locon_list *lc)
{
  chain_list *pred, *ch;
  if (cl==NULL) return NULL;
  for (ch=cl, pred=NULL; ch!=NULL && ch->DATA!=lc; pred=ch, ch=ch->NEXT) ; //printf("%s ",ccname(ch->DATA));
  if (ch==NULL)
    {
      avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 39);
      //fprintf(stderr,"RemoveLofigChainLocon: locon '%s' not found (%s)\n",ccname(lc),(char *)lc->SIG->NAMECHAIN->DATA);
      EXIT(1);
    }
  if (pred!=NULL)
    {
      pred->NEXT=pred->NEXT->NEXT;
      ch->NEXT=NULL;
      freechain(ch);
      return cl;
    }
  ch=cl->NEXT;
  cl->NEXT=NULL;
  freechain(cl);
  return ch;
}

long TransistorKey(lotrs_list *lt)
{
  long res;
  res=KeyContribValueFor(lt->GRID->SIG) + KeyContribValueFor(lt->DRAIN->SIG) + KeyContribValueFor(lt->SOURCE->SIG);
  if (SPI_IGNORE_BULK=='N' && lt->BULK->SIG!=NULL) res+=KeyContribValueFor(lt->BULK->SIG);
  return res;
}

long long KeyContrib2ValueFor(losig_list *ls)
{
  if (ls->TYPE==CNS_SIGVDD) return 0x0ffff;
  if (ls->TYPE==CNS_SIGVSS) return 0x0fffe;
  return ((long)ls >> 4) & 0x0ffff;
}


void TransistorKeyV2(lotrs_list *lt, long long *key0, long long *key1)
{
  long long res;
  long long d, s;
  
  res  = KeyContrib2ValueFor(lt->GRID->SIG)<<(32+16);
  res |= (d=KeyContrib2ValueFor(lt->DRAIN->SIG)) << 32;
  res |= (s=KeyContrib2ValueFor(lt->SOURCE->SIG)) << 16;
  if (SPI_IGNORE_BULK=='N' && lt->BULK->SIG!=NULL)
    res |= KeyContrib2ValueFor(lt->BULK->SIG);
  // else case does not matter
  *key0=res;
  res &= ~(long long)0x0ffffffff0000LL; // removes drain and source
  res |= s << 32;
  res |= d << 16;
  *key1=res;
}



long InstanceKey(loins_list *li)
{
  long res;
  locon_list *lc;
  res=0; 
  for (lc=li->LOCON; lc!=NULL; lc=lc->NEXT)
    res+=KeyContribValueFor(lc->SIG);
  return res;
}

chain_list *LOSIG_FOR_WITHOUT_SPEEDUP=NULL;
#define FAKEFLAG 0x8000000


ht *GetQuickMatchChanceHT(losig_list *ls)
{
  ptype_list *p;
  chain_list *cl;
  locon_list *lc;
  ht *h;
  long key, val;

  p=getptype(ls->USER, GENIUS_LOFIGCHAIN_KEYS_PTYPE);
  if (p==NULL)
    {
      h=addht(2048);
      p=getptype(ls->USER, LOFIGCHAIN);
      for (cl=(chain_list *)p->DATA; cl!=NULL; cl=cl->NEXT)
       {
         lc=(locon_list *)cl->DATA;
         
         if (lc->TYPE=='T')
           key=TransistorKey((lotrs_list *)lc->ROOT);
         else
           key=InstanceKey((loins_list *)lc->ROOT);

         val=gethtitem(h, (void *)key);
         if (val==EMPTYHT) val=0;
         addhtitem(h, (void *)key, val+1);
       }
      ls->USER=addptype(ls->USER, GENIUS_LOFIGCHAIN_KEYS_PTYPE, h);
      LOSIG_FOR_WITHOUT_SPEEDUP=addchain(LOSIG_FOR_WITHOUT_SPEEDUP, ls);
    }
  else 
    h=(ht *)p->DATA;
  return h;
}
int QuickMatchChance(loins_list *model_ins, losig_list *ls, ptype_list *env)
{
  ht *h;
  long key, val;

  if ((h=GetQuickMatchChanceHT(ls))==NULL) {printf("htable on alim\n");EXIT(70);}
  
  key=ComputeModelInstanceKey(model_ins->LOCON, env);
  val=gethtitem(h, (void *)key);
  if (val==EMPTYHT) return QUICK_NONE_MATCH;
  if ((val & ~FAKEFLAG)>1 || (val & FAKEFLAG)==0) return QUICK_ONE_OR_MORE_MATCH;
  return QUICK_FAKE_MATCH;
}

void AddQuickMatchChance(losig_list *ls, long key) // toujours pour les fakes
{
  long val;
  ht *h;

  if ((h=GetQuickMatchChanceHT(ls))==NULL) return;

  val=gethtitem(h, (void *)key);
  if (val==EMPTYHT) val=0;
  val|=FAKEFLAG;
  addhtitem(h, (void *)key, val+1);
}

void ClearQuickMatchChanceHashTables()
{
  chain_list *cl;
  ptype_list *p;
  losig_list *ls;
  for (cl=LOSIG_FOR_WITHOUT_SPEEDUP; cl!=NULL; cl=cl->NEXT)
    {
      ls=(losig_list *)cl->DATA;
      p=getptype(ls->USER, GENIUS_LOFIGCHAIN_KEYS_PTYPE);
      if (p==NULL) EXIT(11);
      delht((ht *)p->DATA);      
      ls->USER=delptype(ls->USER, GENIUS_LOFIGCHAIN_KEYS_PTYPE);
    }
  freechain(LOSIG_FOR_WITHOUT_SPEEDUP);
  LOSIG_FOR_WITHOUT_SPEEDUP=NULL; 
}

void SetPNODEtoNULL(loins_list *li)
{
  locon_list *lc;
  for (lc=li->LOCON; lc!=NULL; lc=lc->NEXT)
    lc->PNODE=NULL;
}

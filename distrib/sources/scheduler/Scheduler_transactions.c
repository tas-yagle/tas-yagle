
#include MUT_H
#include SDLR_H
#include "Scheduler_transactions.h"

//#define DEBUG

static void Scheduler_get_date_location(long date, int *high_offset, int *low_offset)
{
  *high_offset=(date >> SCHEDULER_HIGH_OFFSET_DEC) & SCHEDULER_HIGH_OFFSET_MASK;
  *low_offset=(date >> SCHEDULER_LOW_OFFSET_DEC) & SCHEDULER_LOW_OFFSET_MASK;
}

void Scheduler_RemoveTransaction(Scheduler *S, Scheduler_Transaction *T)
{
  int high, low;
  Scheduler_Registred_Object *objlist;
  chain_list *prev, *cl;

  Scheduler_get_date_location(T->Date, &high, &low);
  if (S->DS.HIGH_OFFSET_Table[high]==NULL) { exit(6); return;}

  // removing from global date ordering
  if (T->Next!=NULL) T->Next->Previous=T->Previous;
  if (T->Previous!=NULL) T->Previous->Next=T->Next;
  else 
    {
      prev=NULL;
      for (cl=S->DS.HIGH_OFFSET_Table[high][low]; cl!=NULL; cl=cl->NEXT)
        {
          if (((Scheduler_Transaction *)cl->DATA)->Date==T->Date) break;
          prev=cl;
        }
      if (cl==NULL) exit(7);
      if (T->Next==NULL) 
        {
          if (prev==NULL) S->DS.HIGH_OFFSET_Table[high][low]=cl->NEXT;
          else prev->NEXT=cl->NEXT;
          cl->NEXT=NULL;
          freechain(cl);
        }
      else cl->DATA=T->Next;
    }

  // removing from object list
  if (T->ObjectNext!=NULL) T->ObjectNext->ObjectPrevious=T->ObjectPrevious;
  if (T->ObjectPrevious!=NULL)
    T->ObjectPrevious->ObjectNext=T->ObjectNext;
  else
    {
      objlist=(Scheduler_Registred_Object *)GetAdvancedTableElem(S->DS.ObjectTable, T->ObjectIndex);
      objlist->HEAD=T->ObjectNext;
    }

  DelHeapItem(&S->Transaction_heap, T);
  S->DS.TransactionCount--;
}

void Scheduler_FreeTransaction(Scheduler *S, Scheduler_Transaction *T)
{  
  DelHeapItem(&S->Transaction_heap, T);
}

void Scheduler_RemoveOlderTransactions(Scheduler *S, int objindex, long Date)
{
  Scheduler_Transaction *next, *lst;
  lst=((Scheduler_Registred_Object *)GetAdvancedTableElem(S->DS.ObjectTable, objindex))->HEAD;
  while (lst!=NULL)
    {
      next=lst->ObjectNext;
      if (lst->Date>=Date)
        {
#ifdef DEBUG
          printf(" removing older for %d at %ld>=%ld\n",objindex,lst->Date,Date);
#endif
          Scheduler_RemoveTransaction(S, lst);
        }
      lst=next;
    }
}

int Scheduler_HasYoungerTransactionAlreadyPosted(Scheduler *S, int objindex, long Date)
{
  Scheduler_Transaction *lst;
  lst=((Scheduler_Registred_Object *)GetAdvancedTableElem(S->DS.ObjectTable, objindex))->HEAD;
  if (lst!=NULL && lst->PostedDate==S->Date && lst->Date<Date) return 1;
  return 0;
}

void debugtrans(Scheduler *S, int objindex)
{
  Scheduler_Transaction *lst;
  lst=((Scheduler_Registred_Object *)GetAdvancedTableElem(S->DS.ObjectTable, objindex))->HEAD;
  printf("for %d =>",objindex);
  while (lst!=NULL)
    {
      printf(" (%ld)", lst->Date);
      lst=lst->ObjectNext;
    }
  printf("\n");
}

void debugtransL(Scheduler *S, Scheduler_Transaction *lst)
{
  if (lst!=NULL)
    {
      printf("date %ld =>",lst->Date);
      while (lst!=NULL)
        {
          printf(" (%d)", lst->ObjectIndex);
          lst=lst->Next;
        }
      printf("\n");
    }
}

void Scheduler_RegisterObject(Scheduler *S, int objectindex, void *object)
{
  Scheduler_Registred_Object *sro;
  sro=(Scheduler_Registred_Object *)GetAdvancedTableElem(S->DS.ObjectTable, objectindex);
  sro->HEAD=NULL;
  sro->Object=object;
}

void *Scheduler_GetRegisterObject(Scheduler *S, int objectindex)
{
  return ((Scheduler_Registred_Object *)GetAdvancedTableElem(S->DS.ObjectTable, objectindex))->Object;
}

Scheduler_Transaction *Scheduler_GetTransactions(Scheduler *S, long Date)
{
  int high, low;
  Scheduler_Transaction *et, *cnt;
  Scheduler_Registred_Object *objlist;
  chain_list *prev, *cl;

  Scheduler_get_date_location(Date, &high, &low);
  if (S->DS.HIGH_OFFSET_Table[high]==NULL) return NULL;

  prev=NULL;
  for (cl=S->DS.HIGH_OFFSET_Table[high][low]; cl!=NULL; cl=cl->NEXT)
    {
      if (((Scheduler_Transaction *)cl->DATA)->Date==Date) break;
      prev=cl;
    }
  if (cl==NULL) return NULL;

  // removing from global date ordering
  et=(Scheduler_Transaction *)cl->DATA;
  if (prev==NULL) S->DS.HIGH_OFFSET_Table[high][low]=cl->NEXT;
  else prev->NEXT=cl->NEXT;
  cl->NEXT=NULL;
  freechain(cl);

  // removing from object list
  for (cnt=et; cnt!=NULL; cnt=cnt->Next)
    {
      if (cnt->ObjectNext!=NULL) cnt->ObjectNext->ObjectPrevious=cnt->ObjectPrevious;
      if (cnt->ObjectPrevious!=NULL)
        cnt->ObjectPrevious->ObjectNext=cnt->ObjectNext;
      else
        {
          objlist=(Scheduler_Registred_Object *)GetAdvancedTableElem(S->DS.ObjectTable, cnt->ObjectIndex);
          objlist->HEAD=cnt->ObjectNext;
        }

      S->DS.TransactionCount--;
    }
  return et;
}

void Scheduler_AddTransaction(Scheduler *S, int ObjectIndex, long Date, void *data)
{
  Scheduler_Transaction *et, *temp;
  Scheduler_Registred_Object *oet;
  int high, low, i;
  chain_list *cl;

  if (Date<0) Date=0;

  Date+=S->Date;

  if (Scheduler_HasYoungerTransactionAlreadyPosted(S, ObjectIndex, Date))
    {
#ifdef DEBUG
     printf("transaction for %d at %ld ignored\n", ObjectIndex, Date);
#endif          
     return;
    }

  Scheduler_RemoveOlderTransactions(S, ObjectIndex, Date);

  et=AddHeapItem(&S->Transaction_heap);
  et->ObjectIndex=ObjectIndex;
  et->Date=Date;
  et->PostedDate=S->Date;
  et->TAG=0;
  et->Next=NULL;
  et->Previous=NULL;
  et->ObjectNext=NULL;
  et->ObjectPrevious=NULL;
  memcpy(((char *)et)+sizeof(Scheduler_Transaction), data, S->datasize);

  Scheduler_get_date_location(Date, &high, &low);
  if (S->DS.HIGH_OFFSET_Table[high]==NULL)
    {
      S->DS.HIGH_OFFSET_Table[high]=AddHeapItem(&S->DS.LOW_OFFSET_Table_heap);
      for (i=0;i<SCHEDULER_LOW_OFFSET_MASK+1; i++) S->DS.HIGH_OFFSET_Table[high][i]=NULL;
    }
  
  for (cl=S->DS.HIGH_OFFSET_Table[high][low]; cl!=NULL; cl=cl->NEXT)
    {
      if (((Scheduler_Transaction *)cl->DATA)->Date==Date) break;
    }

  if (cl==NULL) S->DS.HIGH_OFFSET_Table[high][low]=addchain(S->DS.HIGH_OFFSET_Table[high][low], et);
  else { temp=(Scheduler_Transaction *)cl->DATA; et->Next=temp; temp->Previous=et; cl->DATA=et; }

  oet=(Scheduler_Registred_Object *)GetAdvancedTableElem(S->DS.ObjectTable, ObjectIndex);
  et->ObjectNext=oet->HEAD;
  if (oet->HEAD!=NULL) oet->HEAD->ObjectPrevious=et;
  oet->HEAD=et;

  S->DS.TransactionCount++;
#ifdef DEBUG
  printf("adding transaction for %d at %ld\n", ObjectIndex, Date);
#endif
}

long Scheduler_GetNextDate(Scheduler *S)
{
  int high, low;
  long  lastdate=SCHEDULER_NODATE;
  Scheduler_Transaction *et;
  chain_list *min, *cl;

  if (S->DS.TransactionCount==0) return SCHEDULER_NODATE;
  Scheduler_get_date_location(S->Date, &high, &low);

  while (high<SCHEDULER_HIGH_OFFSET_MASK+1)
    {
      if (S->DS.HIGH_OFFSET_Table[high]!=NULL)
        {
          while (low<SCHEDULER_LOW_OFFSET_MASK+1 && S->DS.HIGH_OFFSET_Table[high][low]==NULL) low++;
          if (low<SCHEDULER_LOW_OFFSET_MASK+1)
            {
              for (cl=S->DS.HIGH_OFFSET_Table[high][low]; cl!=NULL; cl=cl->NEXT)
                {
                  et=(Scheduler_Transaction *)cl->DATA;
                  if (et->Date<lastdate) lastdate=et->Date, min=cl;
                }
            }
          if (lastdate!=SCHEDULER_NODATE) return lastdate;
        }
      high++;
      low=0;
    }

  return lastdate;
}

void Scheduler_GotoDate(Scheduler *S, long Date, int check)
{
  int high, low, nhigh, nlow;
  int i, missed=0;
  chain_list *cl, *ncl;
  Scheduler_Transaction *et, *net;

  Scheduler_get_date_location(S->Date, &high, &low);
  Scheduler_get_date_location(Date, &nhigh, &nlow);

  while (high<nhigh)
    {
      if (S->DS.HIGH_OFFSET_Table[high]!=NULL)
        {
          if (check)
            {
              for (i=0; i<SCHEDULER_LOW_OFFSET_MASK+1; i++)
                {
                  for (cl=S->DS.HIGH_OFFSET_Table[high][i]; cl!=NULL; cl=ncl)
                    {
                      ncl=cl->NEXT;
                      for (et=(Scheduler_Transaction *)cl->DATA; et!=NULL; et=net)
                        {
                          net=et->Next;
                          Scheduler_RemoveTransaction(S, et);
                          missed++;
                        }
                    }
                  freechain(S->DS.HIGH_OFFSET_Table[high][i]);
                  S->DS.HIGH_OFFSET_Table[high][i]=NULL;
                }
              if (missed>0) printf("[!] %d forgotten transactions in scheduler between %ld and %ld\n",missed,S->Date,Date);
            }
          DelHeapItem(&S->DS.LOW_OFFSET_Table_heap, S->DS.HIGH_OFFSET_Table[high]);
          S->DS.HIGH_OFFSET_Table[high]=NULL;
        }
      high++;
    }
  S->Date=Date;
}

void Scheduler_CleanChain(Scheduler *S)
{
  int high, i;

  for (high=0; high<SCHEDULER_HIGH_OFFSET_MASK+1;high++)
    {
      if (S->DS.HIGH_OFFSET_Table[high]!=NULL)
        {
          for (i=0; i<SCHEDULER_LOW_OFFSET_MASK+1; i++)
            {
              freechain(S->DS.HIGH_OFFSET_Table[high][i]);
            }
        }
    }
}

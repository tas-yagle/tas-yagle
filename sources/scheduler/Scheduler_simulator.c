
#include MUT_H
#include SDLR_H
#include "Scheduler_transactions.h"

static void *Scheduler_get_data_from_transaction(Scheduler_Transaction *T)
{
  return ((char *)T)+sizeof(Scheduler_Transaction);
}

Scheduler *Scheduler_CreateSimulator(int datasize, UpdateFunc UF, ExecuteFunc EF, PatternInputFunc PIF)
{
  Scheduler *S;
  int i;

  S=(Scheduler *)mbkalloc(sizeof(Scheduler));
  S->datasize=datasize;
  S->ExecuteFunction=EF;
  S->UpdateFunction=UF;
  S->PatternInputFunction=PIF;
  S->Date=0;

  CreateHeap(S->datasize+sizeof(Scheduler_Transaction), 10000, &S->Transaction_heap);

  for (i=0;i<SCHEDULER_HIGH_OFFSET_MASK+1;i++)
    S->DS.HIGH_OFFSET_Table[i]=NULL;

  CreateHeap((sizeof(chain_list *)*(SCHEDULER_LOW_OFFSET_MASK+1)), 100, &S->DS.LOW_OFFSET_Table_heap);

  S->DS.ObjectTable=CreateAdvancedTableAllocator(10000, sizeof(Scheduler_Registred_Object));
  S->DS.TransactionCount=0;

  return S;
}

void Scheduler_DeleteSimulator(Scheduler *S)
{
  Scheduler_CleanChain(S);
  DeleteHeap(&S->DS.LOW_OFFSET_Table_heap);
  FreeAdvancedTableAllocator(S->DS.ObjectTable);
  DeleteHeap(&S->Transaction_heap);
  mbkfree(S);
}

void Scheduler_CollectPatterns(Scheduler *S, long CurrentDate, long NextDate)
{
  S->PatternInputFunction(S, CurrentDate, NextDate);
}

void Scheduler_RunSimulator(Scheduler *S, long Date)
{
  long nextdate;
  Scheduler_Transaction *events, *ev, *nev;

  while (S->Date<Date)
    {
      // check if news patterns are applyed before next simulation step
      nextdate=Scheduler_GetNextDate(S);
      Scheduler_CollectPatterns(S, S->Date, nextdate);

      // retreive the 'perhaps' new event date
      nextdate=Scheduler_GetNextDate(S);
      if (nextdate==SCHEDULER_NODATE) break;

      Scheduler_GotoDate(S, nextdate, 1);

      events=Scheduler_GetTransactions(S, S->Date);
      // signal value update and event detection
      for (ev=events; ev!=NULL; ev=ev->Next)
        {
          if (S->UpdateFunction(S, ev->ObjectIndex, Scheduler_get_data_from_transaction(ev)))
            ev->TAG|=SCHEDULER_EVENT_DETECTED;
        }
      // new events propagation
      for (ev=events; ev!=NULL; ev=nev)
        {
          nev=ev->Next;
          if (ev->TAG & SCHEDULER_EVENT_DETECTED)
            S->ExecuteFunction(S, ev->ObjectIndex);

          Scheduler_FreeTransaction(S, ev);
        }
      //
    }
  S->Date=Date;
}

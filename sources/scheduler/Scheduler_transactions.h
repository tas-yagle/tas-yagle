
#include <limits.h>

#define SCHEDULER_NODATE LONG_MAX
#define SCHEDULER_EVENT_DETECTED 0x1

typedef struct Scheduler_Transaction
{
  struct Scheduler_Transaction *Previous, *Next, *ObjectPrevious, *ObjectNext;
  long PostedDate, Date;
  int ObjectIndex;
  int TAG;
} Scheduler_Transaction;

typedef struct
{
  Scheduler_Transaction *HEAD;
  void *Object;
} Scheduler_Registred_Object;

void Scheduler_DeleteTransactions(Scheduler *S, Scheduler_Transaction *T);
void Scheduler_RemoveOlderTransactions(Scheduler *S, int objindex, long Date);
Scheduler_Transaction *Scheduler_GetTransactions(Scheduler *S, long Date);
long Scheduler_GetNextDate(Scheduler *S);
void Scheduler_FreeTransaction(Scheduler *S, Scheduler_Transaction *T);
void Scheduler_GotoDate(Scheduler *S, long Date, int check);
void Scheduler_CleanChain(Scheduler *S);

void debugtransL(Scheduler *S, Scheduler_Transaction *lst);

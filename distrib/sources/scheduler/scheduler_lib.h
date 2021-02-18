
#ifndef Scheduler_DEFINE
#define Scheduler_DEFINE


#define SCHEDULER_HIGH_OFFSET_BITS 20
#define SCHEDULER_LOW_OFFSET_BITS 8
#define SCHEDULER_LAST_OFFSET_BITS 4

#define SCHEDULER_HIGH_OFFSET_DEC (SCHEDULER_LOW_OFFSET_BITS+SCHEDULER_LAST_OFFSET_BITS)
#define SCHEDULER_LOW_OFFSET_DEC (SCHEDULER_LAST_OFFSET_BITS)

#define SCHEDULER_HIGH_OFFSET_MASK ((1<<SCHEDULER_HIGH_OFFSET_BITS)-1)
#define SCHEDULER_LOW_OFFSET_MASK ((1<<SCHEDULER_LOW_OFFSET_BITS)-1)

struct Scheduler;

typedef int (*UpdateFunc)(struct Scheduler *S, int ObjectIndex, void *ProjectedValue);
typedef void (*ExecuteFunc)(struct Scheduler *S, int ObjectIndex);
typedef void (*PatternInputFunc)(struct Scheduler *S, long curdate, long nextdate);

typedef struct
{
  chain_list **HIGH_OFFSET_Table[SCHEDULER_HIGH_OFFSET_MASK+1];
  HeapAlloc LOW_OFFSET_Table_heap;  
  int TransactionCount;
  AdvancedTableAllocator *ObjectTable;
} Scheduler_DateSpace;

typedef struct Scheduler
{
  int datasize;
  HeapAlloc Transaction_heap;
  UpdateFunc UpdateFunction;
  ExecuteFunc ExecuteFunction;
  PatternInputFunc PatternInputFunction;
  long Date;
  Scheduler_DateSpace DS;
} Scheduler;

Scheduler *Scheduler_CreateSimulator(int datasize, UpdateFunc UF, ExecuteFunc EF, PatternInputFunc PIF);
void Scheduler_DeleteSimulator(Scheduler *S);
void Scheduler_RunSimulator(Scheduler *S, long Date);
void Scheduler_RegisterObject(Scheduler *S, int objectindex, void *object);
void *Scheduler_GetRegisterObject(Scheduler *S, int objectindex);
void Scheduler_AddTransaction(Scheduler *S, int ObjectIndex, long Date, void *data);

// debug
void debugtrans(Scheduler *S, int objindex);

#endif

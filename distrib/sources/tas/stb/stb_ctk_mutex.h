#ifndef STB_CTK_MUTEX
#define STB_CTK_MUTEX

#define MUTEX_ONLY_ONE ((int)1)

typedef struct
{
  int mutextype;
  int mutexnum;
  chain_list *allttvevents;  
} one_mutex;

typedef struct sortedmutexlist
{
  struct sortedmutexlist *NEXT ;       
  one_mutex              *MUTEX ;
  chain_list             *LIST ;
} sortedmutexlist ;

typedef struct 
{
  chain_list      *NOMUTEX ;
  one_mutex       *MUTEX ;   // eventualy the mutex of victim event.
  sortedmutexlist *LIST  ;
} sortedmutex ;

#define CTK_MUTEX_PTYPE 0xfabbaf01

one_mutex *GetCtkMutex(ttvfig_list *tvf, ttvevent_list *tev);
int GetCtkMutexType(one_mutex *om);
int GetCtkMutexIdent(one_mutex *om);
chain_list* GetCtkMutexEventList(one_mutex *om);
void CtkMutexFree(ttvfig_list *tvf);
extern void CtkMutexInit(ttvfig_list *tvf);
int GetCtkMutexNumber(ttvfig_list *tvf);

sortedmutex* stb_ctk_sort_by_mutex( ttvfig_list *tvf, ttvevent_list *victim, chain_list *eventlist );
void stb_ctk_free_sortedmutex( ttvfig_list *tvf, sortedmutex *mutex );
#endif

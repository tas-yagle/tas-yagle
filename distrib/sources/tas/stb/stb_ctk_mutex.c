

#include MUT_H
#include INF_H
#include STM_H
#include TTV_H
#include TRC_H

#include "stb_ctk_mutex.h"

static chain_list *ALL_MUTEX=NULL;
static unsigned int NB_MUTEX=0;

typedef struct {
  chain_list *HEAD ;
  one_mutex  *MTX ;
} tabmtx ;

tabmtx* STB_CTK_TAB_MTX = NULL ;

static void __CtkMutexInit(ttvfig_list *tvf, int num, long level)
{
  inffig_list *ifl;
  lofig_list *lf;
  chain_list *chain, *head, *chainsig, *cl, *ch;
  ttvsig_list *tvs;
  ttvevent_list *tvn;
  losig_list *ls;
  unsigned int i;
  ptype_list *pt;
  one_mutex *om;
  const struct
  {
    char *section;
    char edge;
    int mutextype;    
  } annot[]=
    {
      {INF_CROSSTALKMUXU, 'u', MUTEX_ONLY_ONE},
      {INF_CROSSTALKMUXD, 'd', MUTEX_ONLY_ONE}
    };

  ifl=getinffig(tvf->INFO->FIGNAME);
  lf=rcx_getlofig(tvf->INFO->FIGNAME, NULL);

  for (i=0; i<sizeof(annot)/sizeof(*annot); i++)
    {
      if (inf_GetPointer(ifl, annot[i].section, "", (void **)&head))
        {
          for (chain=head ; chain ; chain=chain->NEXT) 
            {
              om=NULL;
              for (ch=(chain_list *)chain->DATA; ch!=NULL; ch=ch->NEXT)
                {
                  cl = addchain(NULL,ch->DATA) ;
                  chainsig = ttv_getsigbytype_and_netname(tvf,NULL,TTV_SIG_TYPEALL,cl) ;
                  freechain(cl);
                  for (cl=chainsig; cl!=NULL; cl=cl->NEXT)
                    {
                      tvs=(ttvsig_list *)cl->DATA;
                      ls=rcx_gethtrcxsig( NULL, lf, tvs->NETNAME);
                      if (ls!=NULL)
                        tvs=ttv_getttvsig_sub(tvf, ls, level, TTV_FIND_LINE | TTV_FIND_MIN | TTV_FIND_MAX);

                      if (annot[i].edge=='u') tvn=&tvs->NODE[1];
                      else if (annot[i].edge=='d') tvn=&tvs->NODE[0];
                      else { tvn=&tvs->NODE[0]; exit(1);}
                      if ((pt=getptype(tvn->USER, CTK_MUTEX_PTYPE))!=NULL)
                        {
                          if (((one_mutex *)pt->DATA)->mutexnum!=num)
                            {
/*                              avt_error("stbctk", 1, AVT_ERR, "signal '%s' %s in multiple crosstalk mutex\n", tvs->NETNAME, annot[i].edge=='u'?"rising":"falling"),
                              exit(2);*/
                              continue;
                            }
                        }
                      else
                        {
                          if (om==NULL) 
                            {
                              om=(one_mutex *)mbkalloc(sizeof(one_mutex));
                              om->mutexnum=num;
                              om->mutextype=annot[i].mutextype;
                              om->allttvevents=NULL;
                              ALL_MUTEX=addchain(ALL_MUTEX, om);
                            }
                          tvn->USER=addptype(tvn->USER, CTK_MUTEX_PTYPE, om);
                          om->allttvevents=addchain(om->allttvevents, tvn);
                        }
                    }
                  freechain(chainsig);
                }
              if (om!=NULL) num++;
            }          
        }
    }
  NB_MUTEX=num;
  
  if( STB_CTK_TAB_MTX )
    mbkfree( STB_CTK_TAB_MTX );

  STB_CTK_TAB_MTX = mbkalloc( sizeof( tabmtx ) * NB_MUTEX );
  for( i=0 ; i<NB_MUTEX ; i++ ) {
    STB_CTK_TAB_MTX[i].HEAD = NULL ;
    STB_CTK_TAB_MTX[i].MTX  = NULL ;
  }

  // recursive call for the sons
  for(chain = tvf->INS ; chain != NULL ; chain = chain->NEXT)
    {
      __CtkMutexInit((ttvfig_list *)chain->DATA, num, level) ;
    }
}

void CtkMutexInit(ttvfig_list *tvf)
{
  __CtkMutexInit(tvf, 0, tvf->INFO->LEVEL);
}

void CtkMutexFree(ttvfig_list *tvf)
{
  chain_list *cl, *ch;
  ttvevent_list *tev;
  one_mutex *om;
  for (cl=ALL_MUTEX; cl!=NULL; cl=cl->NEXT)
    {
      om=(one_mutex *)cl->DATA;
      for (ch=om->allttvevents; ch!=NULL; ch=ch->NEXT)
        {
          tev=(ttvevent_list *)ch->DATA;
          if (getptype(tev->USER, CTK_MUTEX_PTYPE)==NULL) exit(8); // temporaire
          tev->USER=delptype(tev->USER, CTK_MUTEX_PTYPE);
        }
      freechain(om->allttvevents);
      mbkfree(om);
    }
  freechain(ALL_MUTEX);
  ALL_MUTEX=NULL;
  NB_MUTEX=0;
  
  if( STB_CTK_TAB_MTX )
    mbkfree( STB_CTK_TAB_MTX );
  STB_CTK_TAB_MTX = NULL ;
  tvf = NULL;
}

one_mutex *GetCtkMutex(ttvfig_list *tvf, ttvevent_list *tev)
{
  ptype_list *pt;
  if ((pt=getptype(tev->USER, CTK_MUTEX_PTYPE))!=NULL)
    return (one_mutex *)pt->DATA;
  return NULL;
  tvf=NULL;
}

int GetCtkMutexType(one_mutex *om)
{
  return om->mutextype;
}

int GetCtkMutexIdent(one_mutex *om)
{
  return om->mutexnum;
}

chain_list* GetCtkMutexEventList(one_mutex *om)
{
  return om->allttvevents;
}

int GetCtkMutexNumber(ttvfig_list *tvf)
{
  return NB_MUTEX;
  tvf = NULL;
}

sortedmutex* stb_ctk_sort_by_mutex( ttvfig_list *tvf, ttvevent_list *victim, chain_list *eventlist )
{
  one_mutex       *m ;
  sortedmutex     *mutex ;
  sortedmutexlist *ml ;
  chain_list      *chain ;
  num_list        *todo ;
  num_list        *num ;
  int              n ;
  ttvevent_list   *event ;
  
  todo = NULL;

  if (ALL_MUTEX!=NULL) {
    for( chain = eventlist ; chain ; chain = chain->NEXT ) {
    
      event = (ttvevent_list*)chain->DATA ;

      m = GetCtkMutex( tvf, event ) ;

      if( m ) {
        n = GetCtkMutexIdent( m ) ;
        if( STB_CTK_TAB_MTX[n].MTX == NULL )
          todo = addnum( todo, n ) ;
        STB_CTK_TAB_MTX[n].HEAD = addchain( STB_CTK_TAB_MTX[n].HEAD, event ) ;
        STB_CTK_TAB_MTX[n].MTX  = m ;
      }
    }
  }
  if( !todo ) {
    return NULL ;
  }

  mutex = (sortedmutex*)addptype( NULL, 0l, NULL );
  
  mutex->MUTEX = GetCtkMutex( tvf, victim );
  mutex->NOMUTEX = NULL ;
  
  for( chain = eventlist ; chain ; chain = chain->NEXT ) {
    event = (ttvevent_list*)chain->DATA ;
    m = GetCtkMutex( tvf, event ) ;
    if( !m ) 
      mutex->NOMUTEX = addchain( mutex->NOMUTEX, event );
  }

  mutex->LIST = NULL ;
  for( num = todo ; num ; num = num->NEXT ) {
    ml = ( sortedmutexlist* ) addptype( NULL, 0l, NULL );
    ml->NEXT = mutex->LIST ;
    mutex->LIST = ml ;
    ml->MUTEX = STB_CTK_TAB_MTX[num->DATA].MTX ;
    ml->LIST  = STB_CTK_TAB_MTX[num->DATA].HEAD ;
    STB_CTK_TAB_MTX[num->DATA].MTX  = NULL ;
    STB_CTK_TAB_MTX[num->DATA].HEAD = NULL ;
  }
  freenum(todo);

  return mutex ;
}


void stb_ctk_free_sortedmutex( ttvfig_list *tvf, sortedmutex *mutex )
{
  sortedmutexlist *ml ;

  for( ml = mutex->LIST ; ml ; ml = ml->NEXT ) 
    freechain( ml->LIST );
  freeptype( (ptype_list*)mutex->LIST );
  freechain( mutex->NOMUTEX );

  ((ptype_list*)mutex)->NEXT = NULL ;
  freeptype( (ptype_list*)mutex );

  tvf=NULL;
}

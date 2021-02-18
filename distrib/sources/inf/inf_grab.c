#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include AVT_H
#include MUT_H
#include INF_H

#include "inf_grab.h"

static void addname(ht **hash, void *name)
{
  if (*hash==NULL) *hash=addht(10);
  addhtitem(*hash, name, 0);
}

static chain_list *return_names(ht *hash)
{
  chain_list *cl;
  if (hash==NULL) return NULL;
  cl=GetAllHTKeys(hash);
  delht(hash);
  return cl;
}

chain_list *grab_inf_assoc (inffig_list *ifl, char *sectionname)
{
  chain_list  *list0, *rename, *cl;
  inf_assoc *assoc; 
  ht *hash=NULL;
  char *name;

  rename=inf_GetEntriesByType (ifl, sectionname, INF_ANY_VALUES);

  while (rename!=NULL)
    {    
     name=(char *)rename->DATA;
     if (name[0]!='+')
       addname(&hash, name);
     inf_GetPointer (ifl, name, sectionname, (void **)&cl);
     while (cl!=NULL)
      {
        assoc = (inf_assoc *) cl->DATA;
        if (assoc->orig!=NULL) addname(&hash, assoc->orig);
        if (assoc->dest!=NULL) addname(&hash, assoc->dest);
        cl=cl->NEXT;
      }
     rename=delchain(rename, rename);
    }
  return return_names(hash);
}

static void   INF_driveOneMutex (ht **hash, chain_list *cl)
{
  while (cl!=NULL)
    {
      addname(hash, (char *)cl->DATA);
      cl=cl->NEXT;
    }
}


chain_list *grab_inf_Mutex (inffig_list *myfig)
{
  chain_list *mutex;
  chain_list *head0=NULL, *head1=NULL, *head2=NULL, *head3=NULL;
  ht *hash=NULL;

  inf_GetPointer(myfig,INF_MUXU, "", (void **)&head0);
  inf_GetPointer(myfig,INF_MUXD, "", (void **)&head1);
  inf_GetPointer(myfig,INF_CMPU, "", (void **)&head2);
  inf_GetPointer(myfig,INF_CMPD, "", (void **)&head3);
    
  if (head0 || head1 || head2 || head3)
    {
      
      for (mutex=head0 ; mutex ; mutex=mutex->NEXT) 
        {
          INF_driveOneMutex(&hash, (chain_list *)mutex->DATA);
        }
      for (mutex=head1 ; mutex ; mutex=mutex->NEXT) 
        {
          INF_driveOneMutex(&hash, (chain_list *)mutex->DATA);
        }
      for (mutex=head2 ; mutex ; mutex=mutex->NEXT) 
        {
          INF_driveOneMutex(&hash, (chain_list *)mutex->DATA);
        }
      for (mutex=head3 ; mutex ; mutex=mutex->NEXT) 
        {
          INF_driveOneMutex(&hash, (chain_list *)mutex->DATA);
        }

    }
  return return_names(hash);
}

chain_list *grab_inf_CrosstalkMutex (inffig_list *myfig)
{
  chain_list *mutex;
  chain_list *head0=NULL, *head1=NULL;
  ht *hash=NULL;

  inf_GetPointer(myfig,INF_CROSSTALKMUXU, "", (void **)&head0);
  inf_GetPointer(myfig,INF_CROSSTALKMUXD, "", (void **)&head1);
    
  if (head0 || head1)
    {
      for (mutex=head0 ; mutex ; mutex=mutex->NEXT) 
        {
          INF_driveOneMutex(&hash, (chain_list *)mutex->DATA);
        }
      for (mutex=head1 ; mutex ; mutex=mutex->NEXT) 
        {
          INF_driveOneMutex(&hash, (chain_list *)mutex->DATA);
        }
    }
  return return_names(hash);
}

chain_list *grab_inf_Ignore (inffig_list *myfig)
{
  chain_list *mutex;
  chain_list *head0=NULL, *head1=NULL, *head2=NULL, *head3=NULL, *head4=NULL, *head5=NULL;
  ht *hash=NULL;

  inf_GetPointer(myfig,INF_IGNORE_INSTANCE, "", (void **)&head0);
  inf_GetPointer(myfig,INF_IGNORE_TRANSISTOR, "", (void **)&head1);
  inf_GetPointer(myfig,INF_IGNORE_RESISTANCE, "", (void **)&head2);
  inf_GetPointer(myfig,INF_IGNORE_CAPACITANCE, "", (void **)&head3);
  inf_GetPointer(myfig,INF_IGNORE_PARASITICS, "", (void **)&head4);
  inf_GetPointer(myfig,INF_IGNORE_NAMES, "", (void **)&head5);
    
  if (head0 || head1 || head2 || head3 || head5)
    {
      for (mutex=head0 ; mutex ; mutex=mutex->NEXT) 
        {
          addname(&hash, ((inf_assoc *)mutex->DATA)->orig);
        }

      for (mutex=head1 ; mutex ; mutex=mutex->NEXT) 
        {
          addname(&hash, ((inf_assoc *)mutex->DATA)->orig);
        }

      for (mutex=head2 ; mutex ; mutex=mutex->NEXT) 
        {
          addname(&hash, ((inf_assoc *)mutex->DATA)->orig);
        }

      for (mutex=head3 ; mutex ; mutex=mutex->NEXT) 
        {
          addname(&hash, ((inf_assoc *)mutex->DATA)->orig);
        }

      for (mutex=head4 ; mutex ; mutex=mutex->NEXT) 
        {
          addname(&hash, ((inf_assoc *)mutex->DATA)->orig);
        }

      for (mutex=head5 ; mutex ; mutex=mutex->NEXT) 
        {
          addname(&hash, ((inf_assoc *)mutex->DATA)->orig);
        }
    }
  return return_names(hash);
}


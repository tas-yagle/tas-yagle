
#include <string.h>
#include MUT_H
#include "gen_new_kind.h"

HeapAlloc ea_heap;

void EAHeap_Manage(int mode)
{
  if (mode==0)
    CreateHeap(sizeof(ExpandableArray), 0, &ea_heap);
  else
    DeleteHeap(&ea_heap);
}

ExpandableArray *NewExpandableArray()
{
  ExpandableArray *ea;
  ea=AddHeapItem(&ea_heap);
  ea->Array=NULL;
  return ea;
}

void DeleteExpandableArray(ExpandableArray *ea)
{
  mbkfree(ea->Array);
  DelHeapItem(&ea_heap, ea);
}

void **GetExpandableArrayItem(ExpandableArray *ea, int index, ExpandableArrayUpdateFunc eaupfunc)
{
  if (ea->Array==NULL)
    {
      int l=index-100, r=index+100, i=0;
      if (l<0) l=0;
      ea->Array=mbkalloc((r-l+1)*sizeof(void *));
      ea->StartIndex=l;
      ea->EndIndex=r;
      while (i<=r-l) ea->Array[i++]=NULL;
      return &ea->Array[index - ea->StartIndex];
    }

  if (index>=ea->StartIndex && index<=ea->EndIndex)
    return &ea->Array[index - ea->StartIndex];

  if (index<ea->StartIndex)
    {
      int l=index - (ea->StartIndex-index)*5, i;
      void **newarray;
      if (l<0) l=0;
      newarray=mbkalloc((ea->EndIndex-l+1)*sizeof(void *));
      memcpy(&newarray[ea->StartIndex-l], ea->Array, sizeof(void *)*(ea->EndIndex-ea->StartIndex+1));
      for (i=0;i<ea->StartIndex-l;i++)
	newarray[i]=NULL;
      if (eaupfunc!=NULL)
	{
	  for (i=ea->StartIndex-l; i<=ea->EndIndex-l; i++)
	    if (newarray[i]!=NULL) eaupfunc(&newarray[i]);
	}
      ea->StartIndex=l;
      mbkfree(ea->Array);
      ea->Array=newarray;
      return &ea->Array[index - ea->StartIndex];
    }

//  if (index>ea->EndIndex)
  {
    int r=index + (index-ea->EndIndex)*5, i;
    void **newarray;
    newarray=mbkalloc((r-ea->StartIndex+1)*sizeof(void *));
    memcpy(newarray, ea->Array, sizeof(void *)*(ea->EndIndex-ea->StartIndex+1));
    for (i=ea->EndIndex-ea->StartIndex+1;i<=r-ea->StartIndex;i++)
      newarray[i]=NULL;
    if (eaupfunc!=NULL)
      {
	for (i=0; i<=ea->EndIndex; i++)
	  if (newarray[i]!=NULL) eaupfunc(&newarray[i]);
      }
    ea->EndIndex=r;
    mbkfree(ea->Array);
    ea->Array=newarray;
    return &ea->Array[index - ea->StartIndex];
  }
}

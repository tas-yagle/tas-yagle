#include <math.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
//#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "mut_lib.h"
#include "mbk_int.h"

// ----------- ADVANCED NAME ALLOCATOR --------------

#define MAXNAMELENGTH 4096
#define HASHMODELIMIT 64

AdvancedNameAllocator *CreateAdvancedNameAllocator(int _case)
{
  int i;
  AdvancedNameAllocator *ana;
  ana=(AdvancedNameAllocator *)mbkalloc(sizeof(AdvancedNameAllocator));
  ana->curindex=0;
  ana->aba=CreateAdvancedBlockAllocator(8192, 'y');
  ana->ata=CreateAdvancedTableAllocator(4096, sizeof(_advanced_namealloc_piece *));
  CreateNameAllocator(0, &ana->na, _case);
  ana->allhtables=NULL;
  ana->numbers=NULL;
  for (i=0;i<256;i++) ana->tab[i].start=NULL;

  if (tolower(_case)=='y') ana->sensitive=1;
  else if (tolower(_case)=='p') ana->sensitive=2; // case preserve
  else ana->sensitive=0;
  return ana;
}

static inline void __split_name(char *name0, char *tempbuf, char **tab)
{
//  chain_list *cl=NULL;
  int i=0, bi=0, j=0;
  char *start;
  unsigned char *name=(unsigned char *)name0;

  if (name[0]=='\0') { tab[0]=NULL; return; }

  do 
    {
      start=&tempbuf[bi];
      while (name[i]!='\0' && !(isalpha(name[i]) || isdigit(name[i])))
        tempbuf[bi++]=name[i++];
      while (name[i]!='\0' && (isalpha(name[i]) || isdigit(name[i])))
        tempbuf[bi++]=name[i++];
      tempbuf[bi++]='\0';
      tab[j++]=start;
//      cl=addchain(cl, start);
    } while (name[i]!='\0');
  tab[j]=NULL;
//  return reverse(cl);
}

static int __AdvancedNameAllocNumber(AdvancedNameAllocator *ana, char *name, int test, int *exists)
{
  char *fin;
  int cnt, val;
  long l;
  _advanced_namealloc_piece **prev;

  val = strtol(name, &fin, 10);
  
  if (*fin!='\0') return -2;

#ifdef UNUN
  if (ana->numbers==NULL) ana->numbers=creatmbk_tableint();

  cnt=tstmbk_tableint( ana->numbers, val );
#else
  if (ana->numbers==NULL) ana->numbers=addht(1000);

  cnt=gethtitem(ana->numbers, (void *)(long)val);
#endif
//  printf("try #%s -> %d\n",name,cnt);

  if (test) { return cnt; }

  if (cnt!=-1) { *exists=1; return cnt; }

  prev=(_advanced_namealloc_piece **)GetAdvancedTableElem(ana->ata, ana->curindex);
  l=(val << 1) | 1;
  *prev=(_advanced_namealloc_piece *)l;
#ifdef UNUN
  cnt=setmbk_tableint(ana->numbers, val, ana->curindex++);
#else
  addhtitem(ana->numbers, (void *)(long)val, ana->curindex++);
#endif
//  printf("#%s -> %d\n",name,ana->curindex-1);
  *exists=0;
  return ana->curindex-1;
}
/*
static int __AdvancedNameAllocatedNumber(AdvancedNameAllocator *ana, int index, char *buf)
{
  int cnt;

  cnt=mbk_scanint(ana->numbers, index);
  if (cnt==-1) 
    {
      sprintf(buf,"<notfound#%d>",index);
      return 0;
    }

  sprintf(buf,"%d",cnt);
  return 1;
}
*/
static int __AdvancedNameAlloc(AdvancedNameAllocator *ana, char *name, int test, int *exists)
{
  char temp[MAXNAMELENGTH];
  char *c=NULL;
  int starti, cnt, hashmode;
  ht *h;
  long l;
  char *tab[10000];
  int j=0;

  _advanced_namealloc_piece *anp, **prev, *a, *lastanp;

  if (name[0]>='0' && name[0]<='9')
    {
      cnt=__AdvancedNameAllocNumber(ana, name, test, exists);
      if (cnt!=-2) return cnt;
    }

  __split_name(name, temp, tab);
  if (tab[0]==NULL) return -1;

  if (ana->sensitive==1)
    starti=(int)(unsigned char)tab[j][0]; // case sensitive
  else
    starti=(int)(unsigned char)tolower(tab[j][0]); // non sens ou preserve

  lastanp=NULL;
  anp=ana->tab[starti].start;
  prev=&ana->tab[starti].start;
  do
    {
      c=NameAlloc(&ana->na, tab[j]);
      if (((long)anp & 1)==0)
        {
          cnt=0;
          for (a=anp; a!=NULL && a->piece!=c; a=a->brother) cnt++;
          if (cnt>=HASHMODELIMIT)
            {
//              printf("hmod: %s/%d sur '%s'\n",name,cnt,c);
              h=addht(HASHMODELIMIT*2);
              for (a=anp; a!=NULL; a=a->brother) addhtitem(h, a->piece, (long)a);
              *prev= (_advanced_namealloc_piece *)((long)h | 1);
              ana->allhtables=addchain(ana->allhtables, h);
              hashmode=1;
            }
          else 
            hashmode=0;
        }
      else
        {
          h=(ht *)((long)anp & ~0x1);
          l=gethtitem(h, c);
          if (l==EMPTYHT) a=NULL;
          else a=(_advanced_namealloc_piece *)l;
          hashmode=1;
        }
      if (a==NULL)
        {
          if (test) { return -1; }
          anp=(_advanced_namealloc_piece *)AdvancedBlockAlloc(ana->aba, sizeof(_advanced_namealloc_piece));
          anp->index=-1;
          anp->next=NULL;
          anp->prev=lastanp;
          anp->piece=c;
          if (hashmode)
            addhtitem(h, c, (long)anp);
          else
            {
              anp->brother=*prev;
              *prev=anp;
            }
        }
      else
        {
          anp=a;
        }
      lastanp=anp;
      prev=&anp->next;
      anp=anp->next;
      j++;
    } while (tab[j]!=NULL);
  if (!test && lastanp->index==-1)
    {
      lastanp->index=ana->curindex++;
      prev=(_advanced_namealloc_piece **)GetAdvancedTableElem(ana->ata, lastanp->index);
      *prev=lastanp;
      *exists=0;
    }
  else
    *exists=1;
//  freechain(ch);

//  printf("'%s' -> %d\n",name,lastanp->index);

  return lastanp->index;
}

int AdvancedNameAlloc(AdvancedNameAllocator *ana, char *name)
{
  int exists;
  return __AdvancedNameAlloc(ana, name, 0, &exists);
}
int AdvancedNameAllocated(AdvancedNameAllocator *ana, char *name)
{
  int exists;
  return __AdvancedNameAlloc(ana, name, 1, &exists);
}

int AdvancedNameAllocEx(AdvancedNameAllocator *ana, char *name, int *exists)
{
  return __AdvancedNameAlloc(ana, name, 0, exists);
}


int AdvancedNameAllocName(AdvancedNameAllocator *ana, int index, char *buf)
{
  char temp[MAXNAMELENGTH];
  int idx=MAXNAMELENGTH-1, a;
  long l;
  _advanced_namealloc_piece **prev, *anp;
  prev=(_advanced_namealloc_piece **)GetAdvancedTableElem(ana->ata, index);
  
  l=(long)*prev;
  if ((l & 1)==0)
    {
      temp[idx]='\0';
      for (anp=*prev; anp!=NULL; anp=anp->prev)
        {
          a=strlen(anp->piece);
          idx-=a;
          memcpy(&temp[idx],anp->piece,a);
        }
      strcpy(buf, &temp[idx]);
      return 0;
    }
  
  sprintf(buf,"%ld",l>>1);
  return 1;
}


void AdvancedNameAllocStat(AdvancedNameAllocator *ana)
{
#ifdef ENABLE_STATS
  //char temp[1024];
  long /*mem_aba = 0, mem_ata = 0, mem_ana = 0,*/ mem_tot=0;
  long mem_ana_na = 0, mem_ana_na_names = 0, mem_ana_aba = 0, mem_ana_ata = 0;

  mem_ana_na = ana->na.__SIZE__ / (1024 * 1024);
  mem_ana_na_names = ana->na.__NBNAMES__;
  mem_ana_aba = ana->aba->__SIZE__ / (1024 * 1024);
  mem_ana_ata = ana->ata->__SIZE__ / (1024 * 1024);
  mem_tot = mem_ana_na + mem_ana_aba + mem_ana_ata;

  printf ("[na:%ld|%ld aba:%ld ata:%ld] ~%ldmo\n",
           mem_ana_na, mem_ana_na_names, mem_ana_aba, mem_ana_ata, mem_tot);
#else
  ana   = NULL;
#endif
}

void FreeAdvancedNameAllocator(AdvancedNameAllocator *ana)
{
  chain_list *cl;

  if (ana->numbers!=NULL)
#ifdef UNUN
    freembk_tableint(ana->numbers);
#else
    delht(ana->numbers);
#endif
  DeleteNameAllocator(&ana->na);
  FreeAdvancedTableAllocator(ana->ata);
  FreeAdvancedBlockAllocator(ana->aba);
  for (cl=ana->allhtables; cl!=NULL; cl=cl->NEXT) delht((ht *)cl->DATA);
  freechain(ana->allhtables);
  mbkfree(ana);
}

// ----------- ADVANCED BLOCK ALLOCATOR --------------

AdvancedBlockAllocator *CreateAdvancedBlockAllocator(int minsize, int align)
{
  AdvancedBlockAllocator *aba;
  aba=(AdvancedBlockAllocator *)mbkalloc(sizeof(AdvancedBlockAllocator));
  if (align) minsize=((minsize+sizeof(void *)-1)/sizeof(void *))*sizeof(void *);
  aba->minsize=minsize;
  aba->search=aba->head=NULL;
  if (tolower(align)=='y') aba->align=1; else aba->align=0;
#ifdef ENABLE_STATS
  aba->__SIZE__=sizeof(AdvancedBlockAllocator);
#endif
  return aba;
}

static _advanced_block_allocator_elem *__findbloc(AdvancedBlockAllocator *aba, int size)
{
  _advanced_block_allocator_elem *abae, *prev;
  int cnt;
  for (abae=aba->head, prev=NULL, cnt=0; abae!=aba->search && (abae->size-abae->free)<size; prev=abae, abae=abae->next, cnt++) ;
  if (abae==aba->search)
    {
      int sz=size;
      if (sz<aba->minsize) sz=aba->minsize;
      abae=(_advanced_block_allocator_elem *)mbkalloc(sizeof(_advanced_block_allocator_elem));
      abae->free=0;
      abae->size=sz;
      abae->tab=(char *)mbkalloc(sz);
      abae->next=aba->head;
      aba->head=abae;
      if (cnt>=32) aba->search=prev;
#ifdef ENABLE_STATS
      aba->__SIZE__+=sz+sizeof(_advanced_block_allocator_elem);
#endif
    }
  return abae;
}

void *AdvancedBlockAlloc(AdvancedBlockAllocator *aba, int size)
{
  _advanced_block_allocator_elem *abae;
  void *pt;
  if (aba->align) size=((size+sizeof(void *)-1)/sizeof(void *))*sizeof(void *);
  abae=__findbloc(aba, size);
  pt=&abae->tab[abae->free];
  abae->free+=size;
  return pt;
}

void FreeAdvancedBlockAllocator(AdvancedBlockAllocator *aba)
{
   _advanced_block_allocator_elem *abae, *next;
   for (abae=aba->head; abae!=NULL; abae=next)
     {
       next=abae->next;
       mbkfree(abae->tab);
       mbkfree(abae);
     }
   mbkfree(aba);
}

// ----------- ADVANCED TABLE ALLOCATOR --------------

AdvancedTableAllocator *CreateAdvancedTableAllocator(int basesize, int elemsize)
{
  AdvancedTableAllocator *ata;
  ata=(AdvancedTableAllocator *)mbkalloc(sizeof(AdvancedTableAllocator));
#ifdef ENABLE_STATS
  ata->__SIZE__=0;
#endif
  ata->blocs=NULL;
  ata->maxblocs=0;
  ata->init=0;
  ata->nbblockelempow=(int)ceil(log(basesize)/log(2));
  if (ata->nbblockelempow>16) ata->nbblockelempow=16;
  ata->nbblockelemmask=(1<<ata->nbblockelempow)-1;
  ata->elemsize=elemsize;
//  printf("c: %d %x %d\n",ata->nbblockelempow, ata->nbblockelemmask, ata->elemsize);
  return ata;
}


void *GetAdvancedTableElem(AdvancedTableAllocator *ata, int index)
{  
  int row, idx, i, nbblocs;
  char **newtab;
  row=index>>ata->nbblockelempow;
  idx=index & ata->nbblockelemmask;
  if (row>=ata->maxblocs)
    {
      nbblocs=ata->maxblocs;
      ata->maxblocs*=2;
      if (row>=ata->maxblocs) ata->maxblocs=row+1;
      newtab=mbkalloc(sizeof(char *)*ata->maxblocs);
      for (i=0;i<nbblocs;i++) newtab[i]=ata->blocs[i];
      for (i=nbblocs;i<ata->maxblocs;i++) newtab[i]=NULL;
      if (ata->blocs!=NULL) mbkfree(ata->blocs);
      ata->blocs=newtab;
#ifdef ENABLE_STATS
      ata->__SIZE__+=(ata->maxblocs-nbblocs)*sizeof(char *);
#endif
    }
  if( ata->blocs[row]==NULL)
    {
      ata->blocs[row]=(char *)mbkalloc(ata->elemsize<<ata->nbblockelempow);
      if (ata->init) memset(ata->blocs[row], 0, ata->elemsize<<ata->nbblockelempow);
#ifdef ENABLE_STATS
      ata->__SIZE__+=ata->elemsize<<ata->nbblockelempow;
#endif
    }

//  printf("%d: %d %d %p\n",index, row, idx,&ata->blocs[row][idx*ata->elemsize]);
  return &ata->blocs[row][idx*ata->elemsize];
}

void FreeAdvancedTableAllocator(AdvancedTableAllocator *ata)
{
  int i;
  for (i=0;i<ata->maxblocs;i++)
    if (ata->blocs[i]!=NULL) mbkfree(ata->blocs[i]);
  if (ata->blocs!=NULL) mbkfree(ata->blocs);
  mbkfree(ata);
}



char *mbk_GetUserChrono(mbk_chrono *mc, char *temp)
{
    unsigned long user;
    unsigned long userM, userS, userD;

    user = (100 * mc->end_t.ru_utime.tv_sec + (mc->end_t.ru_utime.tv_usec / 10000))
        - (100 * mc->start_t.ru_utime.tv_sec + (mc->end_t.ru_utime.tv_usec / 10000));

    userM = user / 6000;
    userS = (user % 6000) / 100;
    userD = (user % 100) / 10;

    sprintf(temp, "%02ldm%02ld.%ld", userM, userS, userD);
 
    return temp;
}

char *mbk_GetRealChrono(mbk_chrono *mc, char *temp)
{
  unsigned long temps;
  temps=mc->end_rt-mc->start_rt;
  sprintf(temp, "%02ldm%02lds", (long) (temps / 60), (long) (temps % 60));
  return temp;
}

void mbk_StartChrono(mbk_chrono *mc)
{
  getrusage(RUSAGE_SELF, &mc->start_t);
  time(&mc->start_rt);
}

void mbk_StopChrono(mbk_chrono *mc)
{
  getrusage(RUSAGE_SELF, &mc->end_t);
  time(&mc->end_rt);
}

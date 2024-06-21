#include <stdlib.h>

#include AVT_H
#include MUT_H
#include SDLR_H
#include "Scheduler_transactions.h"

//#define TEST1
#define TEST2

int DUMPVCD=0;
int DEBUG=0;

#define ONE 1
#define ZERO 0
#define UNK 2 

typedef struct
{
  char *nom;
  char *func;
  char *input[4];
  int val;
  int timing;
  chain_list *output;
} sig_type;

ht *indexfromname=NULL;
sig_type allsigs[200];
int nbsig=0;

FILE *f;
int lastprintdate=0;
int randmode=0;

int _IFN(char *name)
{
  long l;
  l=gethtitem(indexfromname, namealloc(name));
  if (l==EMPTYHT) exit(9);
  return l;
}

void addsig(Scheduler *S, char *name, char *func, char *a, char *b, char *c, int val, int timing)
{
  if (indexfromname==NULL) indexfromname=addht(100);
  name=namealloc(name);
  addhtitem(indexfromname, name, nbsig);
  allsigs[nbsig].nom=name;
  allsigs[nbsig].func=namealloc(func);
  allsigs[nbsig].output=NULL;
  allsigs[nbsig].input[0]=namealloc(a);
  allsigs[nbsig].input[1]=namealloc(b);
  allsigs[nbsig].input[2]=namealloc(c);
  allsigs[nbsig].val=val;
  allsigs[nbsig].timing=timing;

  if (strcmp(a,"")!=0) allsigs[_IFN(a)].output=addchain(allsigs[_IFN(a)].output, name);
  if (strcmp(b,"")!=0) allsigs[_IFN(b)].output=addchain(allsigs[_IFN(b)].output, name);
  if (strcmp(c,"")!=0) allsigs[_IFN(c)].output=addchain(allsigs[_IFN(c)].output, name);

  Scheduler_RegisterObject(S, nbsig, &allsigs[nbsig]);

  if (DUMPVCD)
    {
      fprintf(f,"$var wire 1 %c %s $end\n",'!'+nbsig, name);
    }

  nbsig++;  
}

int updatefunc(Scheduler *S, int index, void *data)
{
  int newval=*(int *)data;
  if (DEBUG) avt_fprintf(stdout,"date " AVT_BOLD "%ld" AVT_RESET " : update '%s' %d -> %d\n",S->Date,allsigs[index].nom, allsigs[index].val, newval);
  if (allsigs[index].val!=newval)
    {
      if (DUMPVCD)
        {
          if (lastprintdate!=S->Date) { fprintf(f,"#%ld\n",S->Date); lastprintdate=S->Date;}
          fprintf(f,"%c%c\n", newval==UNK?'x':'0'+newval, '!'+index);
        }
      allsigs[index].val=newval;
      return 1; // event detected
    }
  // value is the same, no event detected
  return 0;
}

void execfunc(Scheduler *S, int index)
{
  int a, b, c, newval, output, av, bv, timing;
  chain_list *cl;

  for (cl=allsigs[index].output; cl!=NULL; cl=cl->NEXT)
    {
      output=_IFN((char *)cl->DATA);
      if (randmode) timing=rand() % 500;
      else timing=allsigs[output].timing;
        

      if (strcmp(allsigs[output].func, "not")==0)
        {
          a=_IFN(allsigs[output].input[0]);
          av=allsigs[a].val;

          if (av==UNK) newval=UNK;
          else newval=(av+1) & 1;

          if (DEBUG) printf("exec '%s'=not '%s' after %d => %d\n",allsigs[output].nom, allsigs[a].nom, timing, newval);
          Scheduler_AddTransaction(S, output, timing, &newval);
        }
      else if (strcmp(allsigs[output].func, "and")==0)
        {
          a=_IFN(allsigs[output].input[0]);
          av=allsigs[a].val;
          b=_IFN(allsigs[output].input[1]);
          bv=allsigs[b].val;
          if (av==UNK || bv==UNK) newval=UNK;
          else newval=av & bv;

          if (DEBUG)  printf("exec '%s'='%s' and '%s' after %d => %d\n",allsigs[output].nom, allsigs[a].nom, allsigs[b].nom, timing, newval);
          Scheduler_AddTransaction(S, output, timing, &newval);
        }
      else if (strcmp(allsigs[output].func, "nor")==0)
        {
          a=_IFN(allsigs[output].input[0]);
          av=allsigs[a].val;
          b=_IFN(allsigs[output].input[1]);
          bv=allsigs[b].val;
          if (av==UNK || bv==UNK) newval=UNK;
          else newval=((av | bv)+1) & 1;

          if (DEBUG) printf("exec '%s'='%s' nor '%s' after %d => %d\n",allsigs[output].nom, allsigs[a].nom, allsigs[b].nom, timing, newval);
          Scheduler_AddTransaction(S, output, timing, &newval);
        }
      else if (strcmp(allsigs[output].func, "nand")==0)
        {
          a=_IFN(allsigs[output].input[0]);
          av=allsigs[a].val;
          b=_IFN(allsigs[output].input[1]);
          bv=allsigs[b].val;
          if (av==UNK || bv==UNK) newval=UNK;
          else newval=((av & bv)+1) & 1;

          if (DEBUG) printf("exec '%s'='%s' nand '%s' after %d => %d\n",allsigs[output].nom, allsigs[a].nom, allsigs[b].nom, timing, newval);
          Scheduler_AddTransaction(S, output, timing, &newval);
        }
      else { printf("unknown function '%s'\n", allsigs[output].func); exit(10);}
    }
}

long nextpatterndate=0;
int endofpattern=0;

void pattern(Scheduler *S, long min, long next)
{
  if (endofpattern==0 && nextpatterndate>=min && nextpatterndate<=next)
    {
      long delta=nextpatterndate-min;
      int newval, var;
      char name[10];
      if (DEBUG) avt_fprintf(stdout, "" AVT_BLUE "reading patterns at %ld" AVT_RESET "/%ld\n",nextpatterndate,min);
      else avt_fprintf(stdout, "\r" AVT_BLUE "reading patterns at %ld" AVT_RESET "/%ld",nextpatterndate,min);
      switch (nextpatterndate)
        {
        case 0:
          newval=1; Scheduler_AddTransaction(S, _IFN("a"), delta, &newval);
          nextpatterndate=100;
          break;
        case 100:
          newval=0; Scheduler_AddTransaction(S, _IFN("b"), delta, &newval);
          nextpatterndate=150;
          break;
        case 150:
          newval=1; Scheduler_AddTransaction(S, _IFN("d"), delta, &newval);
          nextpatterndate=300;
          break;
        case 300:
          newval=0; Scheduler_AddTransaction(S, _IFN("c"), delta, &newval);
          nextpatterndate=320;
          break;
        case 320:
          newval=1; Scheduler_AddTransaction(S, _IFN("a"), delta, &newval);
          newval=1; Scheduler_AddTransaction(S, _IFN("b"), delta, &newval);
          newval=1; Scheduler_AddTransaction(S, _IFN("c"), delta, &newval);
          newval=0; Scheduler_AddTransaction(S, _IFN("d"), delta, &newval);
          nextpatterndate=500;
          break;
        case 500:
          newval=0; Scheduler_AddTransaction(S, _IFN("a"), delta, &newval);
          newval=0; Scheduler_AddTransaction(S, _IFN("b"), delta, &newval);
          newval=1; Scheduler_AddTransaction(S, _IFN("c"), delta, &newval);
          newval=1; Scheduler_AddTransaction(S, _IFN("d"), delta, &newval);
          nextpatterndate=600;
        default:
          // random
          randmode=1;
          var=rand() % 4;
          sprintf(name,"%c",'a'+var);
          if (rand() % 3==0) newval=rand() % 3;
          else newval=rand() % 2;
          Scheduler_AddTransaction(S, _IFN(name), delta, &newval);
          nextpatterndate+=1+(rand() % 10);

//          endofpattern=1;
          break;
        }
    }
}


int main(int ac, char *av[])
{
  Scheduler *S;
  double data=1.8;
  int i;
  Scheduler_Transaction *tr, *ntr;

  avtenv();
  mbkenv();

#ifdef TEST1
  S=Scheduler_CreateSimulator(sizeof(double), updatefunc, execfunc, pattern);

  // ajout d'object a simuler
  Scheduler_RegisterObject(S, 1);
  Scheduler_RegisterObject(S, 3);
  Scheduler_RegisterObject(S, 5);
  Scheduler_RegisterObject(S, 2);

  // transactions
  printf("next date = %ld\n",Scheduler_GetNextDate(S));
  Scheduler_AddTransaction(S, 1, 100, &data);
  Scheduler_AddTransaction(S, 1, 100, &data);
  Scheduler_AddTransaction(S, 1, 200, &data);
  Scheduler_AddTransaction(S, 1, 400, &data);
  Scheduler_AddTransaction(S, 1, 300, &data);
  debugtrans(S, 1);
  printf("%d transactions remaining\n",S->DS.TransactionCount);
  printf("next date = %ld\n",Scheduler_GetNextDate(S));
  Scheduler_AddTransaction(S, 5, 70, &data);
  Scheduler_AddTransaction(S, 5, 50, &data);
  debugtrans(S, 5);
  printf("%d transactions remaining\n",S->DS.TransactionCount);
  printf("next date = %ld\n",Scheduler_GetNextDate(S));

  Scheduler_AddTransaction(S, 2, 75, &data);
  printf("next date = %ld\n",Scheduler_GetNextDate(S));

  printf("%d transactions remaining\n",S->DS.TransactionCount);

  S->Date=Scheduler_GetNextDate(S);
  tr=Scheduler_GetTransactions(S, S->Date);
  debugtransL(S, tr);
  while (tr!=NULL) { ntr=tr->Next; Scheduler_FreeTransaction(S, tr); tr=ntr;}

  printf("%d transactions remaining\n",S->DS.TransactionCount);
  printf("next date = %ld\n",Scheduler_GetNextDate(S));

  Scheduler_GotoDate(S, 5000, 1);
  Scheduler_DeleteSimulator(S);
#endif
#ifdef TEST2
  srand(time(NULL));

  if (DUMPVCD)
    {
      f=fopen("test.vcd","wt");
      fprintf(f,"$date\n--\n$end\n$version\nAvertec PAVO v1.0\n$end\n$timescale\n	1fs\n$end\n$scope module test $end\n");
    }

  S=Scheduler_CreateSimulator(sizeof(int), updatefunc, execfunc, pattern);
  
  addsig(S, "a", "input", "", "", "", UNK, 0);
  addsig(S, "b", "input", "", "", "", UNK, 0);
  addsig(S, "c", "input", "", "", "", UNK, 0);
  addsig(S, "d", "input", "", "", "", UNK, 0);
  addsig(S, "e", "and", "a", "b", "", UNK, 10);
  addsig(S, "f", "and", "c", "d", "", UNK, 11);
  addsig(S, "g", "not", "d", "", "", UNK, 5);
  addsig(S, "h", "nor", "e", "g", "", UNK, 13);
  addsig(S, "i", "nand", "h", "f", "", UNK, 8);
  addsig(S, "o", "not", "i", "", "", UNK, 4);

  if (DUMPVCD)
    {
      fprintf(f,"$upscope $end\n$enddefinitions $end\n");
      fprintf(f,"#0\n$dumpvars\n");
      for (i=0;i<nbsig;i++) 
        fprintf(f,"%c%c\n", allsigs[i].val==UNK?'x':'0'+allsigs[i].val, '!'+i);
    }


  Scheduler_RunSimulator(S, 20000000);

  if (DUMPVCD)
    {
      fprintf(f,"#%ld\n",S->Date);
      for (i=0;i<nbsig;i++) 
        fprintf(f,"%c%c\n", allsigs[i].val==UNK?'x':'0'+allsigs[i].val, '!'+i);
    }

  if (!DEBUG) printf("\n");

  printf("%d transactions remaining\n",S->DS.TransactionCount);
  printf("next date = %ld\n",Scheduler_GetNextDate(S));


  Scheduler_DeleteSimulator(S);

  if (DUMPVCD)
    {
      fclose(f);
    }
#endif
  return 0;
}

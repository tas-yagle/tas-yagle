#include <stdlib.h>
#include <math.h>

#include AVT_H
#include MUT_H
#include CNS_H
/*#include STM_H
#include TTV_H
#include TAS_H*/
#include SDLR_H
#include "pavo_lib.h"

#define PAVO_CONE_INDEX 0xf0270705

static int DEBUG=1;

int randmode=0;

typedef struct
{
  long ModificationDate;
  float CurrentVoltage;
  cone_list *CONE;
} ConeInformations;

typedef struct pavo_cone_simulator_info
{
  ht *ExternalCones;
  cnsfig_list *cf;
} pavo_ConeSimulatorInfo;

pavo_ConeSimulatorInfo CSI;

int pavo_get_cone_index(cone_list *cl)
{
  ptype_list *pt;
  if ((pt=getptype(cl->USER, PAVO_CONE_INDEX))!=NULL) return (int)(long)pt->DATA;
  exit(6); // err msg
}

int pavo_get_input_cone(char *name)
{
  long l;
  if ((l=gethtitem(CSI.ExternalCones, namealloc(name)))==EMPTYHT) {exit(10); return -1;} // msg
  return l;
}

int pavo_UpdateFunction(Scheduler *S, int index, void *data)
{
  float newval=*(float *)data;
  ConeInformations *ci;

  ci=(ConeInformations *)Scheduler_GetRegisterObject(S, index);
  
  if (DEBUG) avt_fprintf(stdout,"date " AVT_BOLD "%ld" AVT_RESET " : update '%s' %g -> %g\n", S->Date, ci->CONE->NAME, ci->CurrentVoltage, newval);

  if (fabs(ci->CurrentVoltage-newval)>1e-3)
    {
      ci->CurrentVoltage=newval;
      ci->ModificationDate=S->Date;
      return 1; // event detected
    }
  // value is the same, no event detected
  return 0;
}

void pavo_GetConeDelay_and_Voltage(Scheduler *S, cone_list *cl, long *delay, float *value)
{
  ConeInformations *ci;
  edge_list *el;

  // positionning voltage on the cone inputs
  for (el=cl->INCONE; el!=NULL; el=el->NEXT)
    {
     if ((el->TYPE & CNS_EXT)!=CNS_EXT)
        {
          ci=(ConeInformations *)Scheduler_GetRegisterObject(S, pavo_get_cone_index(el->UEDGE.CONE));
          pavo_SetSignalVoltage(pavo_GetLosigByCone(CSI.cf, el->UEDGE.CONE), ci->CurrentVoltage);
        }
    }

  // calling appropriate function
    *value=pavo_CalcInternalNodeVoltage(cl);
    *delay=1+(rand() % 10);

#if 0
  // --- fake value and timing-- a remplacer par la fonction adequate
  {
    // evaluation de la sortie
    int ra=rand() % 6;


    if (ra==0 || ra==4) *value=0;
    else if (ra==1 || ra==5) *value=1.6;
    else if (ra==2) *value=PAVO_HZ_VOLTAGE;
    else *value=PAVO_UNKNOWN_VOLTAGE;

    
    // recuperation du temps de propagation
  }
  // ---------------------------
#endif
  // cleaning
  for (el=cl->INCONE; el!=NULL; el=el->NEXT)
    {
      if ((el->TYPE & CNS_EXT)!=CNS_EXT)
        {
          ci=(ConeInformations *)Scheduler_GetRegisterObject(S, pavo_get_cone_index(el->UEDGE.CONE));
          pavo_RemoveSignalVoltage(pavo_GetLosigByCone(CSI.cf, el->UEDGE.CONE));
        }
    }
}


void pavo_ExecuteFunction(Scheduler *S, int index)
{
  int output;
  long timing;
  float newval;
  ConeInformations *ci;
  edge_list *el;

  ci=(ConeInformations *)Scheduler_GetRegisterObject(S, index);

  for (el=ci->CONE->OUTCONE; el!=NULL; el=el->NEXT)
    {
      if ((el->TYPE & CNS_EXT)!=CNS_EXT)
        {
          output=pavo_get_cone_index(el->UEDGE.CONE);

          pavo_GetConeDelay_and_Voltage(S, el->UEDGE.CONE, &timing, &newval);

          if (DEBUG) printf("exec '%s'=f('%s') after %ld => %g\n",el->UEDGE.CONE->NAME, ci->CONE->NAME, timing, newval);
          if (newval!=PAVO_HZ_VOLTAGE) Scheduler_AddTransaction(S, output, timing, &newval);
        }
    }
}

long nextpatterndate=0;
int endofpattern=0;

void pavo_RandomPatterns(Scheduler *S, long lastdate, long nextdate)
{
  if (endofpattern==0 && nextpatterndate>=lastdate && nextpatterndate<=nextdate)
    {
      long delta=nextpatterndate-lastdate;
      float newval;
      int var, inewval;
      char name[10];

      if (DEBUG) avt_fprintf(stdout, "" AVT_BLUE "reading patterns at %ld" AVT_RESET "\n",nextpatterndate);
      else avt_fprintf(stdout, "\r" AVT_BLUE "reading patterns at %ld" AVT_RESET "",nextpatterndate);
      switch (nextpatterndate)
        {
        case 0:
          newval=1.6; Scheduler_AddTransaction(S, pavo_get_input_cone("a"), delta, &newval);
          nextpatterndate=1000;
          break;
        case 1000:
          newval=0; Scheduler_AddTransaction(S, pavo_get_input_cone("b"), delta, &newval);
          nextpatterndate=1500;
          break;
        case 1500:
          newval=1.6; Scheduler_AddTransaction(S, pavo_get_input_cone("d"), delta, &newval);
          nextpatterndate=3000;
          break;
        case 3000:
          newval=0; Scheduler_AddTransaction(S, pavo_get_input_cone("c"), delta, &newval);
          nextpatterndate=3200;
          break;
        case 3200:
          newval=1.6; Scheduler_AddTransaction(S, pavo_get_input_cone("a"), delta, &newval);
          newval=1.6; Scheduler_AddTransaction(S, pavo_get_input_cone("b"), delta, &newval);
          newval=1.6; Scheduler_AddTransaction(S, pavo_get_input_cone("c"), delta, &newval);
          newval=0; Scheduler_AddTransaction(S, pavo_get_input_cone("d"), delta, &newval);
          nextpatterndate=5000;
          break;
        case 5000:
          newval=0; Scheduler_AddTransaction(S, pavo_get_input_cone("a"), delta, &newval);
          newval=0; Scheduler_AddTransaction(S, pavo_get_input_cone("b"), delta, &newval);
          newval=1.6; Scheduler_AddTransaction(S, pavo_get_input_cone("c"), delta, &newval);
          newval=1.6; Scheduler_AddTransaction(S, pavo_get_input_cone("d"), delta, &newval);
          nextpatterndate=6000;
        default:
          // random
          randmode=1;
          var=rand() % 4;
          sprintf(name,"%c",'a'+var);
          if (rand() % 3==0) inewval=rand() % 4;
          else inewval=rand() % 2;
          if (inewval==1) newval=1.6;
          else if (inewval==2) newval=PAVO_UNKNOWN_VOLTAGE;
          else newval=0;
          Scheduler_AddTransaction(S, pavo_get_input_cone(name), delta, &newval);
          nextpatterndate+=1+(rand() % 10);

//          endofpattern=1;
          break;
        }
    }
}

void pavo_register_cones(cnsfig_list *cf, Scheduler *S, pavo_ConeSimulatorInfo *CSI)
{
  cone_list *cl;
  int number=0;
  ConeInformations *ci;

  CSI->ExternalCones=addht(100);

  for (cl=cf->CONE; cl!=NULL; cl=cl->NEXT)
    {
      cl->USER=addptype(cl->USER, PAVO_CONE_INDEX, (void *)(long)number);
      ci=(ConeInformations *)mbkalloc(sizeof(ConeInformations));
      ci->CONE=cl;
      Scheduler_RegisterObject(S, number, ci);
      if (cl->TYPE & CNS_EXT)
        {
          addhtitem(CSI->ExternalCones, cl->NAME, number);
        }
      number++;
    }
}

void pavo_initialize_cone(cnsfig_list *cf, Scheduler *S)
{
  cone_list *cl;
  int index;
  ConeInformations *ci;

  for (cl=cf->CONE; cl!=NULL; cl=cl->NEXT)
    {
      index=pavo_get_cone_index(cl);
      ci=(ConeInformations *)Scheduler_GetRegisterObject(S, index);
      ci->CurrentVoltage=PAVO_UNKNOWN_VOLTAGE;
      ci->ModificationDate=0;
    }
}

void pavo_uninitialize_cone(cnsfig_list *cf, Scheduler *S)
{
  cone_list *cl;
  int index;
  ConeInformations *ci;

  for (cl=cf->CONE; cl!=NULL; cl=cl->NEXT)
    {
      index=pavo_get_cone_index(cl);
      ci=(ConeInformations *)Scheduler_GetRegisterObject(S, index);
      mbkfree(ci);
    }
}

void pavo_SimulateNetlist(cnsfig_list *cf, long UntilDate)
{
  Scheduler *S;
  
  S=Scheduler_CreateSimulator(sizeof(float), pavo_UpdateFunction, pavo_ExecuteFunction, pavo_RandomPatterns);
  
  CSI.cf=cf;

  pavo_register_cones(cf, S, &CSI);
  pavo_initialize_cone(cf, S);

  Scheduler_RunSimulator(S, UntilDate);

  printf("%d transactions remaining\n",S->DS.TransactionCount);
  printf("next date = %ld\n",Scheduler_GetNextDate(S));

  pavo_uninitialize_cone(cf, S);
  delht(CSI.ExternalCones);

  Scheduler_DeleteSimulator(S);
}

void pavo_SimulateNetlist_test(char *name, long UntilDate)
{
  cnsfig_list *cf;
  if ((cf=getloadedcnsfig(name))!=NULL)
    {
      pavo_SimulateNetlist(cf, UntilDate);
    }
  else
    {
      printf("could not find cone netlist '%s', no simulation done.\n", name);
    }
}

#if 0
int ymain(int ac, char *av[])
{
  cnsfig_list *cf;
  avtenv();
  mbkenv();

  srand(time(NULL));

  // desassembler netlist

  // lancer simulation
  pavo_SimulateNetlist(cf, 4000);


  return 0;
}
#endif

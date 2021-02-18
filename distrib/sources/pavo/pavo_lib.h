
#ifndef PAVO
#define PAVO

/*****************************************************************************/
/*     includes                                                              */
/*****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include MUT_H
#include MLO_H
#include MLU_H
#include RCN_H
#include CNS_H
#include ELP_H


/*****************************************************************************/
/*     defines                                                               */
/*****************************************************************************/

#define PAVO_SIGNAL_SIMULATION_INFORMATIONS 0xf0250705
#define PAVO_SIGNAL_VOLTAGE 0xf1250705

#define PAVO_VOLTAGE_RESOLUTION 10000.0
#define PAVO_UNKNOWN_VOLTAGE 100.0
#define PAVO_HZ_VOLTAGE      200.0

#define PAVO_CONE2LOSIG ((long)0x10000000)


/*****************************************************************************/
/*     Global                                                                */
/*****************************************************************************/

/*****************************************************************************/
/*     Structures                                                            */
/*****************************************************************************/
typedef struct SignalEvent
{
  struct SignalEvent *Next;
  float Time;
  float Voltage;
} SignalEvent;

typedef struct
{
  int Flags;
  ptype_list *USER;
  SignalEvent *Events;
  float CurrentVoltage;
} SignalSimulationInformations;

/*****************************************************************************/
/*     functions                                                             */
/*****************************************************************************/
SignalSimulationInformations *pavo_GetSignalSimulationInformations(losig_list *ls);
void pavo_RemoveSignalSimulationInformations(losig_list *ls);
SignalSimulationInformations *pavo_CreateSignalSimulationInformations(losig_list *ls);
void pavo_SetSignalVoltage(losig_list *ls, float voltage);
float pavo_GetSignalVoltage(losig_list *ls);
void pavo_RemoveSignalVoltage(losig_list *ls);

losig_list *pavo_SetSigVoltByName (cnsfig_list *cnsfig, char *name, float voltage);
losig_list *pavo_GetLosigByCone (cnsfig_list *cnsfig,cone_list *cone);
float pavo_CalcInternalNodeVoltage (cone_list *cone);



#endif

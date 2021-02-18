/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : AVERTEC global tools                                        */
/*    Fichier : sim_meas.h                                                  */
/*                                                                          */
/*    © copyright 2000 AVERTEC                                              */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Antony PINTO                                              */
/*                                                                          */
/****************************************************************************/

#ifndef SIM_MEAS_H
#define SIM_MEAS_H

#include "sim_message.h"
#include <stdarg.h>
#include MUT_H
#include SIM_H

#define SIM_MEAS_LABEL  (int)0
#define SIM_MEAS_SIG    (int)1
#define SIM_MEAS_VALUE  (int)2
#define SIM_MEAS_TRANS  (int)3
#define SIM_MEAS_NTRANS (int)4
#define SIM_MEAS_UP     (int)0
#define SIM_MEAS_DN     (int)1

#define SIM_MEAS_A4S    (int)4
#define SIM_MEAS_NBSIG  (int)2
#define SIM_MEAS_NBARG  (int)(SIM_MEAS_A4S * SIM_MEAS_NBSIG + 1)

typedef struct sim_meas_cmd
{
  /* order of each argument                   */
  short                  ARGORDER[SIM_MEAS_NBARG];
  /* last representing string                 */
  char                  *LAST[SIM_MEAS_NBSIG];
  /* transition U and D representing string   */
  char                  *TRANS[SIM_MEAS_NBSIG][2];
  /* command string                           */
  char                  *CMDSTR;
} sim_meas_cmd;

typedef struct sim_meas_arg
{
  struct sim_meas_arg   *NEXT;
  char                  *ARG[SIM_MEAS_NBARG];/*list of argument to be driven*/
  sim_meas_cmd          *REFCMD;        /* reference command rule for drive */
} sim_meas_arg;

typedef struct sim_meas_print
{
  struct sim_meas_print *NEXT;
  char                  *LABEL;
  char                  *SIGNAME[2];
  char                   SIGTYPE[2];
  char                  *TRANSITION;
  int                   DELAY_TYPE;
  SIM_FLOAT TH_IN, TH_OUT;
} sim_meas_print;

void sim_decodeMeasCmd(char *env);
void sim_decodeMeasCmdFree();       
extern void sim_affectMeasCmdArg(sim_model *model, char *label, char *sig1, char trans1, int ntrans1, double val1, char *sig2, char trans2, int ntrans2, double val2);
extern void sim_printMeasCmd(sim_model *model, FILE *fd);
extern void sim_freeMeasArg(sim_model *model, char *label);
extern void sim_freeMeasAllArg(sim_model *model);
extern void sim_freePrn(sim_model *model);

extern int  sim_getMeas(sim_model *model, char *delay, char *slope, double *dval, double *sval);
extern int  sim_getMeasSlope(sim_model *model, char *label, double *value);
extern int  sim_getMeasDelay(sim_model *model, char *label, double *value);
extern int  sim_getMcMeas( sim_model *model, char *label, double *value, int idmc );
extern void sim_addMeas(sim_model *model, char *delay, char *slope, char *sig1, char *sig2, char *transition, int delay_type);
extern void sim_addMeasDelay(sim_model *model, char *delay, char *sig1, char *sig2, char *transition, int delay_type);
extern void sim_addMeasSlope(sim_model *model, char *slope, char *sig, char *transition, int delay_type);

extern void sim_addMeasWithoutNameCheck(sim_model *model, char *delay, char *slope, char *sig1, char *sig2, char *transition, int delay_type);
extern void sim_addMeasDelayWithoutNameCheck(sim_model *model, char *delay, char *sig1, char *sig2, char *transition, int delay_type);
extern void sim_addMeasSlopeWithoutNameCheck(sim_model *model, char *slope, char *sig, char *transition, int delay_type);
int sim_getTransition(sim_model *model, char *trans, char t[2], long n[2]);

#endif

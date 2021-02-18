/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : AVERTEC global tools                                        */
/*    Fichier : sim_API_extract.c                                           */
/*                                                                          */
/*    © copyright 2000 AVERTEC                                              */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Antony PINTO                                              */
/*                                                                          */
/****************************************************************************/
#include AVT_H
#include API_H
#include STM_H
#include TTV_H

#include "sim_API_extract.h"

#define API_USE_REAL_TYPES

#include "sim_API.h"

static ptype_list   *SLOPE_RES_LIST = NULL;
static ptype_list   *DELAY_RES_LIST = NULL;

/****************************************************************************/
/*{{{                                                                       */
/****************************************************************************/
/*{{{                    sim_AddSpiceMeasure()                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

static void checkdelaytype(char *delay_type)
{
  if (tolower(*delay_type)!='m')
  {
    avt_errmsg(SIM_API_ERRMSG, "004", AVT_ERROR);
//    fprintf(stderr,"[SIM] sim_AddSpiceMeasure: authorized type for delay type are SIM_MAX and SIM_MIN\n");
    *delay_type='M';
  }
}

void sim_AddSpiceMeasure(sim_model *model, char *delay, char *slope,
                         char *sig1, char *sig2, char *transition, char delay_type)
{
  char           separ = SEPAR, *sig[2] = { sig1, sig2};
  int            i;

  checkdelaytype(&delay_type);

  SEPAR = SIM_SEPAR;
  for (i = 0; i < 2; i ++)
    if (sig[i])
      sig[i]  = gen_sim_devect(sig[i]);
  sim_addMeas(model,delay,slope,sig[0],sig[1],transition,delay_type);
  SEPAR       = separ;
}

/*}}}************************************************************************/
///*{{{                    sim_GetSpiceMeasure()                              */
///*                                                                          */
///*                                                                          */
///****************************************************************************/
//int sim_GetSpiceMeasure(sim_model *model, char *label, double *value)
//{
//  return sim_getMeas(model,label,value);
//}
//
///*}}}************************************************************************/
/*{{{                    sim_GetSpiceMeasureSlope()                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
double sim_GetSpiceMeasureSlope(sim_model *model, char *label)
{
  double value;
  int ret = sim_getMeasSlope(model,label,&value);
  if (ret) return value;
  return -1.0;
}

/*}}}************************************************************************/
/*{{{                    sim_GetSpiceMeasureDelay()                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
double sim_GetSpiceMeasureDelay(sim_model *model, char *label)
{
  double value;
  int ret = sim_getMeasDelay(model,label,&value);
  if (ret) return value;
  return -1.0;
}

/*}}}************************************************************************/
/*{{{                    sim_AddSpiceMeasureSlope()                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void sim_AddSpiceMeasureSlope(sim_model *model, char *slope, char *sig,
                             char *transition, char delay_type)
{
  checkdelaytype(&delay_type);
  sim_AddSpiceMeasure(model,NULL,slope,sig,sig,transition,delay_type);
}

/*}}}************************************************************************/
/*{{{                    sim_AddSpiceMeasureDelay()                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void sim_AddSpiceMeasureDelay(sim_model *model, char *delay,
                             char *sig1, char *sig2, char *transition, char delay_type)
{
  checkdelaytype(&delay_type);
  sim_AddSpiceMeasure(model,delay,NULL,sig1,sig2,transition,delay_type);
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                                                                       */
/****************************************************************************/
/*{{{                    sim_GetAllMeasure()                                */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void sim_GetAllMeasure(sim_model *model)
{
  ptype_list    *ptx;
  
  for (ptx = DELAY_RES_LIST; ptx; ptx = ptx->NEXT)
  {
    sim_getMeasDelay(model,(char*)ptx->TYPE,ptx->DATA);
    mbkfree((void*)ptx->TYPE);
  }
  
  for (ptx = SLOPE_RES_LIST; ptx; ptx = ptx->NEXT)
  {
    sim_getMeasSlope(model,(char*)ptx->TYPE,ptx->DATA);
    mbkfree((void*)ptx->TYPE);
  }

  freeptype(DELAY_RES_LIST);
  freeptype(SLOPE_RES_LIST);
  DELAY_RES_LIST    = NULL;
  SLOPE_RES_LIST    = NULL;
}

/*}}}************************************************************************/
/*{{{                    sim_SpiceMeasure()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
// should be named stripunwantedcharacters
static char *suc(char *name, char *buf)
{
  int i;
  for (i=0; name[i]!='\0'; i++)
    if (strchr("()./",name[i])!=NULL) buf[i]='_';
    else buf[i]=name[i];
  buf[i]=name[i];
  return buf;
}

void sim_SpiceMeasure(sim_model *model, char *delay, double *valued, 
                      char *slope, double *values,
                      char *sig1, char *sig2, char *transition, char delay_type)
{
  char           separ = SEPAR, *sig[2] = { sig1, sig2};
  char           delayb[1024], slopeb[1024];
  char           buf1[1024], buf0[1024];
  int            i;

  checkdelaytype(&delay_type);

  SEPAR         = SIM_SEPAR;
  
  if (!delay && valued)
  {
    sprintf(delayb,"gen_label_delay_%s_%s_%s_%s",suc(sig1, buf0),suc(sig2, buf1),transition,delay_type==SIM_MAX?"max":"min");
    delay       = delayb;
  }
  if (!slope && values)
  {
    sprintf(slopeb,"gen_label_slope_%s_%s_%s_%s",suc(sig1, buf0),suc(sig2, buf1),transition,delay_type==SIM_MAX?"max":"min");
    slope       = slopeb;
  }
  
  for (i = 0; i < 2; i ++)
    if (sig[i])
      sig[i]    = gen_sim_devect(sig[i]);
  sim_addMeas(model,delay,slope,sig[0],sig[1],transition,delay_type);
  
  if (delay)
    DELAY_RES_LIST = addptype(DELAY_RES_LIST,(long)mbkstrdup(delay),valued);
  if (slope)
    SLOPE_RES_LIST = addptype(SLOPE_RES_LIST,(long)mbkstrdup(slope),values);
  
  SEPAR         = separ;
}

/*}}}************************************************************************/
/*{{{                    sim_SpiceMeasureSlope()                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void sim_SpiceMeasureSlope(sim_model *model, char *slope, double *value,
                           char *sig, char *transition, char delay_type)
{
  checkdelaytype(&delay_type);
  sim_SpiceMeasure(model,NULL,NULL,slope,value,sig,sig,transition,delay_type);
}

/*}}}************************************************************************/
/*{{{                    sim_SpiceMeasureDelay()                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void sim_SpiceMeasureDelay(sim_model *model, char *delay, double *value,
                     char *sig1, char *sig2, char *transition, char delay_type)
{
  checkdelaytype(&delay_type);
  sim_SpiceMeasure(model,delay,value,NULL,NULL,sig1,sig2,transition,delay_type);
}

/*}}}************************************************************************/
/*}}}************************************************************************/

double sim_ReadMeasure(char *filename, char *label)
{
  double value;
  simenv();
  if (mfe_readmeasure(filename,SIM_EXTRACT_RULE/*NULL*/,NULL/*V_STR_TAB[__SIM_EXTRACT_RULE].VALUE*/,label,&value)) value=-1;
  return value;
}

void sim_ResetMeasures(sim_model *model)
{
   sim_resetmeasures(model);
}


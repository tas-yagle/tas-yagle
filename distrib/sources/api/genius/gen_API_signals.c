/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : AVERTEC global tools                                        */
/*    Fichier : gen_API_signals.c                                               */
/*                                                                          */
/*    © copyright 2000 AVERTEC                                              */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Antony PINTO                                              */
/*                                                                          */
/****************************************************************************/

#define API_USE_REAL_TYPES
#include "gen_API_signals.h"
#include STM_H

/****************************************************************************/
/*{{{                    gen_AddCapa()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void gns_AddCapa(lofig_list *lofig, char *con_name, double capa)
{
  locon_list    *locon;
  losig_list    *gnd;

  if (!(gnd = gen_get_ground(lofig)))
  {
    avt_errmsg(GNS_API_ERRMSG, "010", AVT_ERROR, __func__);
//    fprintf (stderr, "[WAR_%s]: ground not found\n",__func__);
    return;
  }
  else if (!(locon = gen_get_connector(lofig,con_name)))
  {
    avt_errmsg(GNS_API_ERRMSG, "011", AVT_ERROR, con_name, lofig->NAME);
    //fprintf (stderr, "[WAR_%s]: connector %s not found\n",__func__,con_name);
    return;
  }
  else
  {
    capa               *= 1e+12;
    if (!gnd->PRCN)
      gnd->PRCN         = addlorcnet(gnd);

    if (!locon->SIG->PRCN)
      locon->SIG->PRCN  = addlorcnet(locon->SIG);

    if (!locon->PNODE)
      setloconnode(locon,locon->SIG->PRCN->NBNODE);

    addloctc(locon->SIG,locon->PNODE->DATA,gnd,1,capa);
  }
}

/*}}}************************************************************************/
/*{{{                    gns_AddResi()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void gns_AddResi(lofig_list *lofig, char *con1_name, char *con2_name,
                 double resi)
{
  gns_AddLineRC(lofig,con1_name,con2_name,resi,0.0,0.0);
}

/*}}}************************************************************************/
/*{{{                    gns_AddLineRc()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void gns_AddLineRC(lofig_list *lofig, char *con1_name, char *con2_name,
                   double resi, double capa1, double capa2)
{
  locon_list    *locon1, *locon2;
  losig_list    *gnd;

  if (!(gnd = gen_get_ground(lofig)))
  {
    avt_errmsg(GNS_API_ERRMSG, "010", AVT_ERROR, __func__);
//    fprintf (stderr, "[WAR_%s]: ground not found\n",__func__);
    return;
  }
  else if (!(locon1 = gen_get_connector(lofig,con1_name)))
  {
    avt_errmsg(GNS_API_ERRMSG, "011", AVT_ERROR, con1_name, lofig->NAME);
    //fprintf(stderr,"[WAR_%s]: connector %s not found\n",__func__,con1_name);
    return;
  }
  else if (!(locon2 = gen_get_connector(lofig,con2_name)))
  {
    avt_errmsg(GNS_API_ERRMSG, "011", AVT_ERROR, con2_name, lofig->NAME);
    //fprintf(stderr,"[WAR_%s]: connector %s not found\n",__func__,con2_name);
    return;
  }
  else if (locon1->SIG != locon2->SIG)
  {
    avt_errmsg(GNS_API_ERRMSG, "016", AVT_ERROR, con1_name,con2_name);
/*    fprintf(stderr,"[WAR_%s]: connectors %s and %s ",__func__,con1_name,con2_name);
    fprintf(stderr,"don't come from the same signal\n");*/
    return;
  }
  else
  {
    losig_list      *sig = locon2->SIG;
    
    capa1              *= 1e+12;
    capa2              *= 1e+12;
    if (!sig->PRCN)
      sig->PRCN         = addlorcnet(sig);
    
    if (!gnd->PRCN)
      gnd->PRCN         = addlorcnet(gnd);

    if (!locon1->PNODE)
      setloconnode(locon1,sig->PRCN->NBNODE);

    if (!locon2->PNODE)
      setloconnode(locon2,sig->PRCN->NBNODE);


    addlowire(sig,0,resi,0,locon1->PNODE->DATA,locon2->PNODE->DATA);
    addloctc(sig,locon1->PNODE->DATA,gnd,1,capa1);
    addloctc(sig,locon2->PNODE->DATA,gnd,1,capa2);
  }
}

/*}}}************************************************************************/

double gns_GetSignalVoltage(char *name)
{
  losig_list *ls;
  float val;
  if (strncasecmp(name,"ext:",4)==0)
    ls=mbk_quickly_getlosigbyname(GENIUS_GLOBAL_LOFIG, namealloc(&name[4]));
  else
    ls=gns_GetCorrespondingSignal(name);
  if (ls!=NULL && getlosigalim (ls, &val)) { return val; }
  return -10000;
}

int gns_GetSignalVoltageSwing(char *name, double *low, double *high)
{
  losig_list *ls;
  cnsfig_list *cf;
  inffig_list *ifl;
  float val;
  char *where;
  alim_list *al;

  *high=V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE; *low=0;

  ls=gns_GetCorrespondingSignal(name);
  if (ls==NULL) return 1;

  if ((cf=getloadedcnsfig(GENIUS_GLOBAL_LOFIG->NAME))==NULL)
    {
      cnsenv();
      where=filepath (GENIUS_GLOBAL_LOFIG->NAME,"cns");
      if (where!=NULL)
        {
          printf("Loading '%s'\n",where);
          cf=getcnsfig(GENIUS_GLOBAL_LOFIG->NAME, NULL);
          ifl=getloadedinffig(GENIUS_GLOBAL_LOFIG->NAME);
          cns_addmultivoltage(ifl, cf);
        }
    }

  if (cf==NULL) return 1;

  if ((al=cns_get_signal_multivoltage(ls))!=NULL) { *high=al->VDDMAX; *low=al->VSSMIN; return 1;}

  return 0;
}
/*
int gns_OrientTransistor(char *transname, char *signalname, char *dir)
{
  losig_list *ls;
  lotrs_list *tr;
  int dir;

  if (strcasecmp(dir,"input")==0)
  ls=gns_GetCorrespondingSignal(signalname);
  if (ls==NULL) return 1;
  tr=gns_GetCorrespondingTransistor(transname);
  if (tr==NULL) return 1;
  return 0;
}
*/

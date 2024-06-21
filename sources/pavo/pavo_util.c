/*****************************************************************************\
 * pavo_util.c
\*****************************************************************************/

#include PAVO_H

SignalSimulationInformations *pavo_GetSignalSimulationInformations(losig_list *ls)
{
  ptype_list *pt;
  if ((pt=getptype(ls->USER, PAVO_SIGNAL_SIMULATION_INFORMATIONS))==NULL) return NULL;
  return (SignalSimulationInformations *)pt->DATA;
}

void pavo_RemoveSignalSimulationInformations(losig_list *ls)
{
  ptype_list *pt;
  SignalSimulationInformations *ssi;
  if ((pt=getptype(ls->USER, PAVO_SIGNAL_SIMULATION_INFORMATIONS))==NULL)
    exit(1);

  ssi=(SignalSimulationInformations *)pt->DATA;
  // a faire : remove events
  // free ptypes
  mbkfree(ssi);
  
  ls->USER=delptype(ls->USER, PAVO_SIGNAL_SIMULATION_INFORMATIONS);
}

SignalSimulationInformations *pavo_CreateSignalSimulationInformations(losig_list *ls)
{
  ptype_list *pt;
  SignalSimulationInformations *ssi;
  if ((pt=getptype(ls->USER, PAVO_SIGNAL_SIMULATION_INFORMATIONS))==NULL)
  ssi=(SignalSimulationInformations *)mbkalloc(sizeof(SignalSimulationInformations));
  ssi->Flags=0;
  ssi->USER=NULL;
  ssi->CurrentVoltage=PAVO_UNKNOWN_VOLTAGE;
  ssi->Events=NULL;
  ls->USER=addptype(ls->USER, PAVO_SIGNAL_SIMULATION_INFORMATIONS, ssi);
  return ssi;
}



void pavo_SetSignalVoltage(losig_list *ls, float voltage)
{
  ptype_list *pt;
  if ((pt=getptype(ls->USER, PAVO_SIGNAL_VOLTAGE))==NULL)
    pt=ls->USER=addptype(ls->USER, PAVO_SIGNAL_VOLTAGE, NULL);
  pt->DATA=(void *)(long)(voltage*PAVO_VOLTAGE_RESOLUTION);
}

float pavo_GetSignalVoltage(losig_list *ls)
{
  ptype_list *pt;
  if ((pt=getptype(ls->USER, PAVO_SIGNAL_VOLTAGE))==NULL)
     exit(1);
  return (float)(((long)pt->DATA)/PAVO_VOLTAGE_RESOLUTION);

}

void pavo_RemoveSignalVoltage(losig_list *ls)
{
  ptype_list *pt;

  if ((pt=getptype(ls->USER, PAVO_SIGNAL_VOLTAGE))==NULL)
    exit(1);
  ls->USER=delptype(ls->USER, PAVO_SIGNAL_VOLTAGE);
}

/*****************************************************************************\
 * Func pavo_GetLosigByCone
\*****************************************************************************/
losig_list *pavo_GetLosigByCone (cnsfig_list *cnsfig,cone_list *cone)
{
  lofig_list *lofig;
  losig_list *losig;
  ptype_list *ptype;

  if ( !(ptype = getptype (cone->USER,PAVO_CONE2LOSIG))) {
    lofig = getloadedlofig (cnsfig->NAME);
    losig = mbk_quickly_getlosigbyname ( lofig, cone->NAME);
    cone->USER = addptype (cone->USER, PAVO_CONE2LOSIG, losig);
  }
  else
    losig = (losig_list*)ptype->DATA;

  return losig;
}

/*****************************************************************************\
 * Func pavo_SetSigVoltByName
 *
\*****************************************************************************/
losig_list *pavo_SetSigVoltByName (cnsfig_list *cnsfig, char *name, float voltage)
{
  char *sig2find;
  cone_list *cone;
  locon_list *locon;
  losig_list *losig=NULL;
  edge_list *ptinedge;
  int done = 0;

  sig2find = namealloc (name);
  for (cone = cnsfig->CONE ; cone && !done ; cone=cone->NEXT ) {
    for ( ptinedge = cone->INCONE ; ptinedge ; ptinedge=ptinedge->NEXT ) {
      if ((ptinedge->TYPE & CNS_BLEEDER) != 0) continue;
      if ((ptinedge->TYPE & CNS_FEEDBACK) != 0) continue;
      if ((ptinedge->TYPE & CNS_EXT) != 0) {
        locon = ptinedge->UEDGE.LOCON;
        if (getsigname (locon->SIG) == sig2find) {
          losig = locon->SIG;
          pavo_SetSignalVoltage(losig, voltage);
          done = 1;
          break;
        }
      }
      else {
        if ( (losig = pavo_GetLosigByCone (cnsfig,cone)) ) {
          if (getsigname (losig) == sig2find) {
            pavo_SetSignalVoltage(losig, voltage);
            done = 1;
            break;
          }
        }
      }
    }
  }
  if (!losig)
    printf ("[PAVO ERR] Can't set %s to %fV!\n", name, voltage );
  return losig;
}
/*****************************************************************************\
 * Func pavo_CalcInternalNodeVoltage
 *
 * Return the voltage of the output cone
\*****************************************************************************/
float pavo_CalcInternalNodeVoltage (cone_list *cone)
{
  lotrs_list *lotrs;
  locon_list *locon;
  branch_list *branch[3] ;
  losig_list *drain,*gate,*source,*bulk;
  link_list *link ;
  int i;
  float vs=0.0;
  float vgate=0.0;
  //double vti
  double vt,vdd;
  elpmodel_list *elpmodel;
  int pmos_unknown;
  int pmos_actif_unknown=0;
  int nmos_actif_unknown=0;
  int nmos_unknown;
  int nmos_actif=0;
  int pmos_actif=0;
  int transcase = elpTYPICAL;
  double vgs,vbs,vds;
  int br_vdd =0;
  int br_vdd_unknown =0;
  int br_vss =0;
  int br_vss_unknown =0;

  branch[0] = cone->BRVDD ;
  branch[1] = cone->BRVSS ;
  branch[2] = cone->BREXT ;

  for(i = 0 ; i < 3 ; i++) {
    for(; branch[i] != NULL ; branch[i] = branch[i]->NEXT) {
      if((branch[i]->TYPE & (CNS_BLEEDER|CNS_FEEDBACK|CNS_EXT)) == 0) {
        pmos_unknown = 0;
        nmos_unknown = 0;
        pmos_actif = 1;
        nmos_actif = 1;
        for(link = branch[i]->LINK ; link != NULL ; link = link->NEXT) {
          if((link->TYPE & (CNS_IN | CNS_INOUT)) == 0) {
            lotrs = link->ULINK.LOTRS ;

            vdd = elpGetVddFromCorner ( lotrs, transcase );
            if ( (elpmodel = elpGetModel (lotrs,vdd,transcase))) {
              vt = elpmodel->elpModel[elpVT];
              //vti = elpmodel->elpModel[elpVTI];
            }
            drain = lotrs->DRAIN->SIG;
            gate = lotrs->GRID->SIG;
            source = lotrs->SOURCE->SIG;
            bulk = lotrs->BULK->SIG;
            vgate = pavo_GetSignalVoltage(gate);
            
            /*
            printf ("[PAVO] Xtor %s, drain : %s\n",lotrs->TRNAME,getsigname(drain));
            printf ("[PAVO]          gate  : %s voltage = %f\n",getsigname(gate),vgate);
            printf ("[PAVO]          source: %s \n",getsigname(source));
            printf ("[PAVO]          bulk: %s \n",getsigname(bulk));
            */

            if ( i == 0 ) { //BRVDD
              if (vgate == PAVO_UNKNOWN_VOLTAGE) {
                pmos_unknown = 1;
              }
              else {
                if ( (vdd-vgate) <= vt ) { // branche non passante -> passe a la suivante
                  pmos_actif = 0;
                  break;
                }
              }
            }
            if ( i == 1 ) { //BRVSS
              if ( vgate == PAVO_UNKNOWN_VOLTAGE ) {
                nmos_unknown = 1;
              }
              else {
                if (vgate < vt ) { // branche non passante -> passe a la suivante
                  nmos_actif = 0;
                  break;
                }
              }
            }
          }
          else {
            locon = link->ULINK.LOCON ;
            printf ("[PAVO] Locon : %s\n",getsigname(locon->SIG));
          }
        }
        if ( i == 0 && pmos_actif ) { // BRVDD
          if ( !pmos_unknown ) {
            br_vdd = 1;
            break;
          }
          else
            br_vdd_unknown = 1;
        }
        else if ( i == 1 && nmos_actif ) { // BRVSS
          if ( !nmos_unknown ) {
            br_vss = 1;
            break;
          }
          else 
            br_vss_unknown = 1;
        }
      }
    }
  }

  if ( br_vss && br_vdd )  // conflit
    vs = vdd/2.0;
  else if ( br_vss && !br_vdd &&!br_vdd_unknown ) 
    vs = 0.0;
  else if ( br_vdd && !br_vss &&!br_vss_unknown ) 
    vs = vdd;
  else if ( !br_vdd && !br_vdd_unknown && !br_vss &&!br_vss_unknown ) 
    vs = PAVO_HZ_VOLTAGE;
  else if ( br_vdd && br_vdd_unknown && !br_vss &&!br_vss_unknown ) 
    vs = vdd;
  else if ( br_vss && br_vss_unknown && !br_vdd &&!br_vdd_unknown ) 
    vs = 0.0;
  else
    vs = PAVO_UNKNOWN_VOLTAGE;

  return vs;
}

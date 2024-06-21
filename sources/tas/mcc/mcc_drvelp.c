/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : MCC Version 1                                               */
/*    Fichier : mcc_drvelp.c                                                */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

#include "mcc_drvelp.h"
#include "mcc_util.h"
#include "mcc_debug.h"

int cmpmccelptechno( int techmcc, int techelp )
{
  if( ( techmcc == MCC_MOS2    && techelp == elpMOS2    ) ||
      ( techmcc == MCC_BSIM3V3 && techelp == elpBSIM3V3 ) ||
      ( techmcc == MCC_MM9     && techelp == elpMM9     ) ||
      ( techmcc == MCC_BSIM4   && techelp == elpBSIM4   ) ||
      ( techmcc == MCC_EXTMOD  && techelp == elpEXTMOD     ) ||
      ( techmcc == MCC_MPSP    && techelp == elpPSP     ) ||
      ( techmcc == MCC_MPSPB   && techelp == elpPSP     )    )
    return 1 ;
  return 0 ;
}

int mcctechnotoelptechno( int techmcc )
{
  int r ;
  switch( techmcc ) {
  case MCC_MOS2 :       r = elpMOS2 ; break ;
  case MCC_BSIM3V3 :    r = elpBSIM3V3 ; break ;
  case MCC_MM9 :        r = elpMM9 ; break ;
  case MCC_BSIM4 :      r = elpBSIM4 ; break ;
  case MCC_MPSP :
  case MCC_MPSPB :      r = elpPSP ; break ;
  case MCC_EXTMOD :     r = elpEXTMOD ; break ;
  default :             r = elpNoTechno ;
  }

  return r ;
}

void mcc_addmodele(ftype,typet, lotrsparam_n, lotrsparam_p,
                   elpmodeln, elpmodelp)
int ftype ;
int typet ;
elp_lotrs_param *lotrsparam_n;
elp_lotrs_param *lotrsparam_p;
elpmodel_list  **elpmodeln;
elpmodel_list  **elpmodelp;
{
 elpmodel_list *model ;
 long lmax ;
 long lmin ;
 long wmax ;
 long wmin ;

 if(ftype == MCC_PARAM)
   {
    if ( elpGeneral[elpTEMP] < ELPMINTEMP )
      elpGeneral[elpTEMP] = MCC_TEMP ;
    elpGeneral[elpSLOPE] =  MCC_SLOPE ;
    if(elpGeneral[elpGVDDMAX] < 0.0)
      elpGeneral[elpGVDDMAX] = MCC_VDDmax ;
    elpGeneral[elpGDTHR] = MCC_VTH ;
    elpGeneral[elpGSHTHR] = MCC_VTH_HIGH ;
    elpGeneral[elpGSLTHR] = MCC_VTH_LOW ;
   }

 if((typet == MCC_TRANS_N) || (typet == MCC_TRANS_B))
  {
   lmin = mcc_ftol(MCC_LNMIN * (double)SCALE_X) ;
   lmax = mcc_ftol(MCC_LNMAX * (double)SCALE_X) ;
   wmin = mcc_ftol(MCC_WNMIN * (double)SCALE_X) ;
   wmax = mcc_ftol(MCC_WNMAX * (double)SCALE_X) ;

   if(ftype == MCC_PARAM)
     {
      model = elpAddModel(MCC_TNMODEL,NULL,elpNMOS,MCC_NINDEX,
                          lmin,lmax,wmin,wmax,
                          MCC_DLN,MCC_DWN,MCC_XLN,MCC_XWN,MCC_VDDmax,MCC_NCASE,mcctechnotoelptechno(MCC_SPICEMODELTYPE),
                          lotrsparam_n->PARAM[elpMULU0],
                          lotrsparam_n->PARAM[elpDELVT0],
                          lotrsparam_n->PARAM[elpSA],
                          lotrsparam_n->PARAM[elpSB],
                          lotrsparam_n->PARAM[elpSD],
                          lotrsparam_n->PARAM[elpNF],
                          lotrsparam_n->PARAM[elpM],
                          lotrsparam_n->PARAM[elpNRS],
                          lotrsparam_n->PARAM[elpNRD],
                          lotrsparam_n->VBULK, 
                          lotrsparam_n->PARAM[elpSC],
                          lotrsparam_n->PARAM[elpSCA],
                          lotrsparam_n->PARAM[elpSCB],
                          lotrsparam_n->PARAM[elpSCC],
                          lotrsparam_n->longkey) ;
     }
    else
     {
      for(model = ELP_MODEL_LIST ; model != NULL ; model = model->NEXT)
        {
         if((strcasecmp(model->elpModelName,MCC_TNMODEL) == 0) &&
            (model->elpTransIndex == MCC_NINDEX) &&
            (model->elpTransType == elpNMOS) &&
            (model->elpRange[elpLMIN] == lmin) &&
            (model->elpRange[elpLMAX] == lmax) &&
            (model->elpRange[elpWMIN] == wmin) &&
            (model->elpRange[elpWMAX] == wmax) &&
            (model->elpTransCase == MCC_NCASE) &&
            (model->elpTransTechno == mcctechnotoelptechno(MCC_SPICEMODELTYPE) ) )
           break ;
        }
     }
  
   if(ftype == MCC_PARAM)
    {
     model->elpTemp =  MCC_TEMP ;
     model->elpModel[elpVT] = MCC_VTN ;
     model->elpModel[elpVT0] = MCC_VT0N ;
     model->elpModel[elpKT] = MCC_KTN ;
     model->elpModel[elpKS] = MCC_KSN ;
     model->elpModel[elpKR] = MCC_KRN ;
     model->elpModel[elpA] = MCC_AN ;
     model->elpModel[elpB] = MCC_BN ;
     model->elpModel[elpRT] = MCC_RNT ;
     model->elpModel[elpRS] = MCC_RNS ;
     model->elpModel[elpKRT] = MCC_KRNT ;
     model->elpModel[elpKRS] = MCC_KRNS ;
     model->elpShrink[elpDLC] = MCC_DLCN ;
     model->elpShrink[elpDWC] = MCC_DWCN ;
     model->elpShrink[elpDWCJ] = MCC_DWCJN ;
     model->elpVoltage[elpVDEG] = MCC_VDDdeg ;
     model->elpVoltage[elpVTI] = MCC_VTIN;
     model->elpVoltage[elpVDDMAX] = MCC_VDDmax ;
     model->elpCapa[elpCGS] = MCC_CGSN ;
     model->elpCapa[elpCGS0] = MCC_CGS0N ;
     model->elpCapa[elpCGSU] = MCC_CGSUN ;
     model->elpCapa[elpCGSUF] = MCC_CGSUFN ;
     model->elpCapa[elpCGSU0] = MCC_CGSU0N ;
     model->elpCapa[elpCGSUMIN] = MCC_CGSU_N_MIN ;
     model->elpCapa[elpCGSUMAX] = MCC_CGSU_N_MAX ;
     model->elpCapa[elpCGSD] = MCC_CGSDN ;
     model->elpCapa[elpCGSDF] = MCC_CGSDFN ;
     model->elpCapa[elpCGSD0] = MCC_CGSD0N ;
     model->elpCapa[elpCGSDMIN] = MCC_CGSD_N_MIN ;
     model->elpCapa[elpCGSDMAX] = MCC_CGSD_N_MAX ;
     model->elpCapa[elpCGD] = MCC_CGDN ;
     model->elpCapa[elpCGDC] = MCC_CGDCN ;
     model->elpCapa[elpCGD0] = MCC_CGD0N ;
     model->elpCapa[elpCGD1] = MCC_CGD1N ;
     model->elpCapa[elpCGD2] = MCC_CGD2N ;
     model->elpCapa[elpCGDC0] = MCC_CGDC0N ;
     model->elpCapa[elpCGDC1] = MCC_CGDC1N ;
     model->elpCapa[elpCGDC2] = MCC_CGDC2N ;
     model->elpCapa[elpCGSI] = MCC_CGSIN ;
     model->elpCapa[elpCGSIC] = MCC_CGSICN ;
     model->elpCapa[elpCGP] = MCC_CGPN ;
     model->elpCapa[elpCGPUMIN] = MCC_CGPU_N_MIN ;
     model->elpCapa[elpCGPUMAX] = MCC_CGPU_N_MAX ;
     model->elpCapa[elpCGPDMIN] = MCC_CGPD_N_MIN ;
     model->elpCapa[elpCGPDMAX] = MCC_CGPD_N_MAX ;
     model->elpCapa[elpCGPO] = MCC_CGPO_N ;
     model->elpCapa[elpCGPOC] = MCC_CGPOC_N ;
     model->elpCapa[elpCDS] = MCC_CDSN ;
     model->elpCapa[elpCDSU] = MCC_CDS_U_N ;
     model->elpCapa[elpCDSD] = MCC_CDS_D_N ;
     model->elpCapa[elpCDP] = MCC_CDPN ;
     model->elpCapa[elpCDPU] = MCC_CDP_U_N ;
     model->elpCapa[elpCDPD] = MCC_CDP_D_N ;
     model->elpCapa[elpCDW] = MCC_CDWN ;
     model->elpCapa[elpCDWU] = MCC_CDW_U_N ;
     model->elpCapa[elpCDWD] = MCC_CDW_D_N ;
     model->elpCapa[elpCSS] = MCC_CSSN ;
     model->elpCapa[elpCSSU] = MCC_CSS_U_N ;
     model->elpCapa[elpCSSD] = MCC_CSS_D_N ;
     model->elpCapa[elpCSP] = MCC_CSPN ;
     model->elpCapa[elpCSPU] = MCC_CSP_U_N ;
     model->elpCapa[elpCSPD] = MCC_CSP_D_N ;
     model->elpCapa[elpCSW] = MCC_CSWN ;
     model->elpCapa[elpCSWU] = MCC_CSW_U_N ;
     model->elpCapa[elpCSWD] = MCC_CSW_D_N ;
     model->elpSWJUNCAP = MCC_SWJUNCAPN ;
     model->elpRacc[elpRACCS] = MCC_RACCNS ;
     model->elpRacc[elpRACCD] = MCC_RACCND ;
     if ( lotrsparam_n ) {
       model->elpModel[elpMULU0]   = lotrsparam_n->PARAM[elpMULU0];
       model->elpModel[elpDELVT0]  = lotrsparam_n->PARAM[elpDELVT0];
       model->elpModel[elpSA]      = lotrsparam_n->PARAM[elpSA];
       model->elpModel[elpSB]      = lotrsparam_n->PARAM[elpSB];
       model->elpModel[elpSD]      = lotrsparam_n->PARAM[elpSD];
       model->elpModel[elpNF]      = lotrsparam_n->PARAM[elpNF];
       model->elpVoltage[elpVBULK] = lotrsparam_n->VBULK;
     }
     if ( elpmodeln ) *elpmodeln = model;
    }
   else if(ftype == MCC_FIT_A)
    {
     model->elpModel[elpA] = MCC_AN ;
    }
   else if(ftype == MCC_FIT_CG)
    {
     model->elpCapa[elpCGS] = MCC_CGSN ;
     model->elpCapa[elpCGS0] = MCC_CGS0N ;
     model->elpCapa[elpCGSU] = MCC_CGSUN ;
     model->elpCapa[elpCGSUF] = MCC_CGSUFN ;
     model->elpCapa[elpCGSU0] = MCC_CGSU0N ;
     model->elpCapa[elpCGSUMIN] = MCC_CGSU_N_MIN ;
     model->elpCapa[elpCGSUMAX] = MCC_CGSU_N_MAX ;
     model->elpCapa[elpCGSD] = MCC_CGSDN ;
     model->elpCapa[elpCGSDF] = MCC_CGSDFN ;
     model->elpCapa[elpCGSD0] = MCC_CGSD0N ;
     model->elpCapa[elpCGSDMIN] = MCC_CGSD_N_MIN ;
     model->elpCapa[elpCGSDMAX] = MCC_CGSD_N_MAX ;
     model->elpCapa[elpCGD] = MCC_CGDN ;
     model->elpCapa[elpCGDC] = MCC_CGDCN ;
     model->elpCapa[elpCGD0] = MCC_CGD0N ;
     model->elpCapa[elpCGD1] = MCC_CGD1N ;
     model->elpCapa[elpCGD2] = MCC_CGD2N ;
     model->elpCapa[elpCGDC0] = MCC_CGDC0N ;
     model->elpCapa[elpCGDC1] = MCC_CGDC1N ;
     model->elpCapa[elpCGDC2] = MCC_CGDC2N ;
     model->elpCapa[elpCGSI] = MCC_CGSIN ;
     model->elpCapa[elpCGSIC] = MCC_CGSICN ;
     model->elpCapa[elpCGP] = MCC_CGPN ;
     model->elpCapa[elpCGPUMIN] = MCC_CGPU_N_MIN ;
     model->elpCapa[elpCGPUMAX] = MCC_CGPU_N_MAX ;
     model->elpCapa[elpCGPDMIN] = MCC_CGPD_N_MIN ;
     model->elpCapa[elpCGPDMAX] = MCC_CGPD_N_MAX ;
     model->elpCapa[elpCGPO] = MCC_CGPO_N ;
     model->elpCapa[elpCGPOC] = MCC_CGPOC_N ;
    }
   else if(ftype == MCC_FIT_CDN)
    {
     model->elpCapa[elpCDS] = MCC_CDSN ;
     model->elpCapa[elpCDSU] = MCC_CDS_U_N ;
     model->elpCapa[elpCDSD] = MCC_CDS_D_N ;
     model->elpCapa[elpCDP] = MCC_CDPN ;
     model->elpCapa[elpCDPU] = MCC_CDP_U_N ;
     model->elpCapa[elpCDPD] = MCC_CDP_D_N ;
     model->elpCapa[elpCDW] = MCC_CDWN ;
     model->elpCapa[elpCDWU] = MCC_CDW_U_N ;
     model->elpCapa[elpCDWD] = MCC_CDW_D_N ;
     model->elpCapa[elpCSS] = MCC_CSSN ;
     model->elpCapa[elpCSSU] = MCC_CSS_U_N ;
     model->elpCapa[elpCSSD] = MCC_CSS_D_N ;
     model->elpCapa[elpCSP] = MCC_CSPN ;
     model->elpCapa[elpCSPU] = MCC_CSP_U_N ;
     model->elpCapa[elpCSPD] = MCC_CSP_D_N ;
     model->elpCapa[elpCSW] = MCC_CSWN ;
     model->elpCapa[elpCSWU] = MCC_CSW_U_N ;
     model->elpCapa[elpCSWD] = MCC_CSW_D_N ;
    }
  }

 if((typet == MCC_TRANS_P) || (typet == MCC_TRANS_B))
  {
   lmin = mcc_ftol(MCC_LPMIN * (double)SCALE_X) ;
   lmax = mcc_ftol(MCC_LPMAX * (double)SCALE_X) ;
   wmin = mcc_ftol(MCC_WPMIN * (double)SCALE_X) ;
   wmax = mcc_ftol(MCC_WPMAX * (double)SCALE_X) ;
  
   if(ftype == MCC_PARAM)
     {
      model = elpAddModel(MCC_TPMODEL,NULL,elpPMOS,MCC_PINDEX,
                          lmin,lmax,wmin,wmax,
                          MCC_DLP,MCC_DWP,MCC_XLP,MCC_XWP,MCC_VDDmax,MCC_PCASE,mcctechnotoelptechno(MCC_SPICEMODELTYPE),
                          lotrsparam_p->PARAM[elpMULU0],
                          lotrsparam_p->PARAM[elpDELVT0],
                          lotrsparam_p->PARAM[elpSA],
                          lotrsparam_p->PARAM[elpSB],
                          lotrsparam_p->PARAM[elpSD],
                          lotrsparam_p->PARAM[elpNF],
                          lotrsparam_p->PARAM[elpM],
                          lotrsparam_p->PARAM[elpNRS],
                          lotrsparam_p->PARAM[elpNRD],
                          lotrsparam_p->VBULK, 
                          lotrsparam_p->PARAM[elpSC],
                          lotrsparam_p->PARAM[elpSCA],
                          lotrsparam_p->PARAM[elpSCB],
                          lotrsparam_p->PARAM[elpSCC],
                          lotrsparam_p->longkey) ;
     }
    else
     {
      for(model = ELP_MODEL_LIST ; model != NULL ; model = model->NEXT)
        {
         if((strcasecmp(model->elpModelName,MCC_TPMODEL) == 0) &&
            (model->elpTransIndex == MCC_PINDEX) &&
            (model->elpTransType == elpPMOS) &&
            (model->elpRange[elpLMIN] == lmin) &&
            (model->elpRange[elpLMAX] == lmax) &&
            (model->elpRange[elpWMIN] == wmin) &&
            (model->elpRange[elpWMAX] == wmax) &&
            (model->elpTransCase == MCC_PCASE) &&
            (model->elpTransTechno == mcctechnotoelptechno( MCC_SPICEMODELTYPE ) ) )
           break ;
        }
     }
  
   if(ftype == MCC_PARAM)
    {
     model->elpTemp =  MCC_TEMP ;
     model->elpModel[elpVT] = MCC_VTP ;
     model->elpModel[elpVT0] = MCC_VT0P ;
     model->elpModel[elpKT] = MCC_KTP ;
     model->elpModel[elpKS] = MCC_KSP ;
     model->elpModel[elpKR] = MCC_KRP ;
     model->elpModel[elpA] = MCC_AP ;
     model->elpModel[elpB] = MCC_BP ;
     model->elpModel[elpRT] = MCC_RPT ;
     model->elpModel[elpRS] = MCC_RPS ;
     model->elpModel[elpKRT] = MCC_KRPT ;
     model->elpModel[elpKRS] = MCC_KRPS ;
     model->elpShrink[elpDLC] = MCC_DLCP ;
     model->elpShrink[elpDWC] = MCC_DWCP ;
     model->elpShrink[elpDWCJ] = MCC_DWCJP ;
     model->elpVoltage[elpVDEG] = MCC_VSSdeg ;
     model->elpVoltage[elpVTI] = MCC_VTIP;
     model->elpVoltage[elpVDDMAX] = MCC_VDDmax ;
     model->elpCapa[elpCGS] = MCC_CGSP ;
     model->elpCapa[elpCGS0] = MCC_CGS0P ;
     model->elpCapa[elpCGSU] = MCC_CGSUP ;
     model->elpCapa[elpCGSUF] = MCC_CGSUFP ;
     model->elpCapa[elpCGSU0] = MCC_CGSU0P ;
     model->elpCapa[elpCGSUMIN] = MCC_CGSU_P_MIN ;
     model->elpCapa[elpCGSUMAX] = MCC_CGSU_P_MAX ;
     model->elpCapa[elpCGSD] = MCC_CGSDP ;
     model->elpCapa[elpCGSDF] = MCC_CGSDFP ;
     model->elpCapa[elpCGSD0] = MCC_CGSD0P ;
     model->elpCapa[elpCGSDMIN] = MCC_CGSD_P_MIN ;
     model->elpCapa[elpCGSDMAX] = MCC_CGSD_P_MAX ;
     model->elpCapa[elpCGD] = MCC_CGDP ;
     model->elpCapa[elpCGDC] = MCC_CGDCP ;
     model->elpCapa[elpCGD0] = MCC_CGD0P ;
     model->elpCapa[elpCGD1] = MCC_CGD1P ;
     model->elpCapa[elpCGD2] = MCC_CGD2P ;
     model->elpCapa[elpCGDC0] = MCC_CGDC0P ;
     model->elpCapa[elpCGDC1] = MCC_CGDC1P ;
     model->elpCapa[elpCGDC2] = MCC_CGDC2P ;
     model->elpCapa[elpCGSI] = MCC_CGSIP ;
     model->elpCapa[elpCGSIC] = MCC_CGSICP ;
     model->elpCapa[elpCGP] = MCC_CGPP ;
     model->elpCapa[elpCGPUMIN] = MCC_CGPU_P_MIN ;
     model->elpCapa[elpCGPUMAX] = MCC_CGPU_P_MAX ;
     model->elpCapa[elpCGPDMIN] = MCC_CGPD_P_MIN ;
     model->elpCapa[elpCGPDMAX] = MCC_CGPD_P_MAX ;
     model->elpCapa[elpCGPO] = MCC_CGPO_P ;
     model->elpCapa[elpCGPOC] = MCC_CGPOC_P ;

     model->elpCapa[elpCDS] = MCC_CDSP ;
     model->elpCapa[elpCDSU] = MCC_CDS_U_P ;
     model->elpCapa[elpCDSD] = MCC_CDS_D_P ;
     model->elpCapa[elpCDP] = MCC_CDPP ;
     model->elpCapa[elpCDPU] = MCC_CDP_U_P ;
     model->elpCapa[elpCDPD] = MCC_CDP_D_P ;
     model->elpCapa[elpCDW] = MCC_CDWP ;
     model->elpCapa[elpCDWU] = MCC_CDW_U_P ;
     model->elpCapa[elpCDWD] = MCC_CDW_D_P ;
     model->elpCapa[elpCSS] = MCC_CSSP ;
     model->elpCapa[elpCSSU] = MCC_CSS_U_P ;
     model->elpCapa[elpCSSD] = MCC_CSS_D_P ;
     model->elpCapa[elpCSP] = MCC_CSPP ;
     model->elpCapa[elpCSPU] = MCC_CSP_U_P ;
     model->elpCapa[elpCSPD] = MCC_CSP_D_P ;
     model->elpCapa[elpCSW] = MCC_CSWP ;
     model->elpCapa[elpCSWU] = MCC_CSW_U_P ;
     model->elpCapa[elpCSWD] = MCC_CSW_D_P ;
     model->elpSWJUNCAP = MCC_SWJUNCAPP ;

     model->elpRacc[elpRACCS] = MCC_RACCPS ;
     model->elpRacc[elpRACCD] = MCC_RACCPD ;

     if ( lotrsparam_p ) {
       model->elpModel[elpMULU0]   = lotrsparam_p->PARAM[elpMULU0];
       model->elpModel[elpDELVT0]  = lotrsparam_p->PARAM[elpDELVT0];
       model->elpModel[elpSA]      = lotrsparam_p->PARAM[elpSA];
       model->elpModel[elpSB]      = lotrsparam_p->PARAM[elpSB];
       model->elpModel[elpSD]      = lotrsparam_p->PARAM[elpSD];
       model->elpModel[elpNF]      = lotrsparam_p->PARAM[elpNF];
       model->elpVoltage[elpVBULK] = lotrsparam_p->VBULK;
     }
     if ( elpmodelp ) *elpmodelp = model;
    }
   else if(ftype == MCC_FIT_A)
    {
     model->elpModel[elpA] = MCC_AP ;
    }
   else if(ftype == MCC_FIT_CG)
    {
     model->elpCapa[elpCGS] = MCC_CGSP ;
     model->elpCapa[elpCGS0] = MCC_CGS0P ;
     model->elpCapa[elpCGSU] = MCC_CGSUP ;
     model->elpCapa[elpCGSUF] = MCC_CGSUFP ;
     model->elpCapa[elpCGSU0] = MCC_CGSU0P ;
     model->elpCapa[elpCGSUMIN] = MCC_CGSU_P_MIN ;
     model->elpCapa[elpCGSUMAX] = MCC_CGSU_P_MAX ;
     model->elpCapa[elpCGSDMIN] = MCC_CGSD_P_MIN ;
     model->elpCapa[elpCGSDMAX] = MCC_CGSD_P_MAX ;
     model->elpCapa[elpCGSD] = MCC_CGSDP ;
     model->elpCapa[elpCGSDF] = MCC_CGSDFP ;
     model->elpCapa[elpCGSD0] = MCC_CGSD0P ;
     model->elpCapa[elpCGD] = MCC_CGDP ;
     model->elpCapa[elpCGDC] = MCC_CGDCP ;
     model->elpCapa[elpCGD0] = MCC_CGD0P ;
     model->elpCapa[elpCGD1] = MCC_CGD1P ;
     model->elpCapa[elpCGD2] = MCC_CGD2P ;
     model->elpCapa[elpCGDC0] = MCC_CGDC0P ;
     model->elpCapa[elpCGDC1] = MCC_CGDC1P ;
     model->elpCapa[elpCGDC2] = MCC_CGDC2P ;
     model->elpCapa[elpCGSI] = MCC_CGSIP ;
     model->elpCapa[elpCGSIC] = MCC_CGSICP ;
     model->elpCapa[elpCGP] = MCC_CGPP ;
     model->elpCapa[elpCGPUMIN] = MCC_CGPU_P_MIN ;
     model->elpCapa[elpCGPUMAX] = MCC_CGPU_P_MAX ;
     model->elpCapa[elpCGPDMIN] = MCC_CGPD_P_MIN ;
     model->elpCapa[elpCGPDMAX] = MCC_CGPD_P_MAX ;
     model->elpCapa[elpCGPO] = MCC_CGPO_P ;
     model->elpCapa[elpCGPOC] = MCC_CGPOC_P ;
    }
   else if(ftype == MCC_FIT_CDP)
    {
     model->elpCapa[elpCDS] = MCC_CDSP ;
     model->elpCapa[elpCDSU] = MCC_CDS_U_P ;
     model->elpCapa[elpCDSD] = MCC_CDS_D_P ;
     model->elpCapa[elpCDP] = MCC_CDPP ;
     model->elpCapa[elpCDPU] = MCC_CDP_U_P ;
     model->elpCapa[elpCDPD] = MCC_CDP_D_P ;
     model->elpCapa[elpCDW] = MCC_CDWP ;
     model->elpCapa[elpCDWU] = MCC_CDW_U_P ;
     model->elpCapa[elpCDWD] = MCC_CDW_D_P ;
     model->elpCapa[elpCSS] = MCC_CSSP ;
     model->elpCapa[elpCSSU] = MCC_CSS_U_P ;
     model->elpCapa[elpCSSD] = MCC_CSS_D_P ;
     model->elpCapa[elpCSP] = MCC_CSPP ;
     model->elpCapa[elpCSPU] = MCC_CSP_U_P ;
     model->elpCapa[elpCSPD] = MCC_CSP_D_P ;
     model->elpCapa[elpCSW] = MCC_CSWP ;
     model->elpCapa[elpCSWU] = MCC_CSW_U_P ;
     model->elpCapa[elpCSWD] = MCC_CSW_D_P ;
    }
  }
}

void mcc_drvelp(ftype,typet,lotrsparam_n,lotrsparam_p,elpdriveall)
int ftype ;
int typet ;
elp_lotrs_param *lotrsparam_n;
elp_lotrs_param *lotrsparam_p;
int elpdriveall;
{
 char *pt ;
 char techname[1024] ;
 elpmodel_list *elpmodeln,*elpmodelp;

 mcc_addmodele(ftype,typet,lotrsparam_n,lotrsparam_p,&elpmodeln,&elpmodelp) ;

 strcpy(techname,MCC_ELPFILE) ; 

 if((pt = strrchr(techname,(int)('.'))) != NULL)
     *pt = '\0' ;

 strcpy(elpTechnoName,techname) ;
 elpTechnoVersion = atof(MCC_ELPVERSION) ;
 strcpy(elpEsimName,MCC_SPICENAME) ;

 switch ( elpdriveall ) {
   case MCC_DRV_ALL_MODEL : elpDriveElp(techname) ;
                            break;
   case MCC_DRV_ONE_MODEL : elpDriveModel (techname, (typet == MCC_TRANS_N) ? elpmodeln : elpmodelp );
                            break;
   case MCC_DONOT_DRV_MODEL : elpVerifModel ( (typet == MCC_TRANS_N) ? elpmodeln : elpmodelp );
                             break;
 }
}

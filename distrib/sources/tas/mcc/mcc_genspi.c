/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : MCC Version 1                                               */
/*    Fichier : mcc_genspi.c                                                */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

#include <unistd.h>
#include "mcc_optim.h"
#include "mcc_util.h"
#include "mcc_genspi.h"
#include "mcc_drvelp.h"
#include "mcc_curv.h"
#include "mcc_fit.h"
#include "mcc_mod_util.h"
#include "mcc_mod_spice.h"
#include "mcc_debug.h"

char *MCC_TN_SUBCKT=NULL, *MCC_TP_SUBCKT=NULL;
lotrs_list *MCC_CURRENT_LOTRS=NULL; 
char *MCC_CURRENT_SUBCKT=NULL;
int  MCC_FLAG_FIT ;
long MCC_SPICEUD_FITA ;
long MCC_SPICEDU_FITA ;
long MCC_SPICEUD_FITCGNOCAPA ;
long MCC_SPICEDU_FITCGNOCAPA ;
long MCC_SPICEUD_FITCG ;
long MCC_SPICEDU_FITCG ;
long MCC_SPICEUD_FITDN ;
long MCC_SPICEDU_FITDN ;
long MCC_SPICEDU_FITDP ;
long MCC_SPICEUD_FITDP ;
long MCC_SPICEFUD_FITA ;
long MCC_SPICEFDU_FITA ;
long MCC_SPICEFUD_FITCG ;
long MCC_SPICEFDU_FITCG ;
long MCC_SPICEFUD_FITCGNOCAPA ;
long MCC_SPICEFDU_FITCGNOCAPA ;
long MCC_SPICEFUD_FITDN ;
long MCC_SPICEFDU_FITDN ;
long MCC_SPICEFDU_FITDP ;
long MCC_SPICEFUD_FITDP ;

double *MCC_IDNSAT ;
double *MCC_IDNRES ;
double *MCC_IDNVGS ;
double *MCC_IDPSAT ;
double *MCC_IDPRES ;
double *MCC_IDPVGS ;
double *MCC_VDDDEG ;
double *MCC_VSSDEG ;
double **MCC_FITS ;

char surf_string_n[1024];
char surf_string_p[1024];
elp_lotrs_param mcc_user_lotrsparam_n, mcc_user_lotrsparam_p;

void mcc_printf_instance_specific(FILE *file, elp_lotrs_param *lotrsparam_n)
{
   int b3=0,b4=0;
   b3 = 1;
   b4 = ( MCC_SPICEMODELTYPE == MCC_BSIM4 ? 1 : 0 ) ;

   if (b3 || b4) {
     fprintf(file,"+");
     if (lotrsparam_n->PARAM[elpDELVT0]!=0)
       fprintf(file," delvto=%g", lotrsparam_n->PARAM[elpDELVT0]);     
     if (lotrsparam_n->PARAM[elpMULU0]!=1)
       fprintf(file," mulu0=%g", lotrsparam_n->PARAM[elpMULU0]);     
     if ( lotrsparam_n->PARAM[elpSA] > ELPMINVALUE)
       fprintf(file," sa=%g", lotrsparam_n->PARAM[elpSA]);
     if ( lotrsparam_n->PARAM[elpSB] > ELPMINVALUE )
       fprintf(file," sb=%g", lotrsparam_n->PARAM[elpSB]);
     if ( lotrsparam_n->PARAM[elpSD] > ELPMINVALUE )
       fprintf(file," sd=%g", lotrsparam_n->PARAM[elpSD]);
     if ( lotrsparam_n->PARAM[elpNF] > ELPMINVALUE && lotrsparam_n->PARAM[elpNF]!=1)
       fprintf(file," nf=%g", lotrsparam_n->PARAM[elpNF]);
     if ( lotrsparam_n->PARAM[elpM] > ELPMINVALUE && lotrsparam_n->PARAM[elpM]!=1)
       fprintf(file," m=%g", lotrsparam_n->PARAM[elpM]);
     if ( lotrsparam_n->PARAM[elpNRS] > ELPMINVALUE)
       fprintf(file," nrs=%g", lotrsparam_n->PARAM[elpNRS]);
     if ( lotrsparam_n->PARAM[elpNRD] > ELPMINVALUE)
       fprintf(file," nrd=%g", lotrsparam_n->PARAM[elpNRD]);
     if ( lotrsparam_n->PARAM[elpSC] > ELPMINVALUE)
       fprintf(file," sc=%g", lotrsparam_n->PARAM[elpSC]);
     if ( lotrsparam_n->PARAM[elpSCA] > ELPMINVALUE)
       fprintf(file," sca=%g", lotrsparam_n->PARAM[elpSCA]);
     if ( lotrsparam_n->PARAM[elpSCB] > ELPMINVALUE)
       fprintf(file," scb=%g", lotrsparam_n->PARAM[elpSCB]);
     if ( lotrsparam_n->PARAM[elpSCC] > ELPMINVALUE)
       fprintf(file," scc=%g", lotrsparam_n->PARAM[elpSCC]);
     fprintf(file,"\n");
   }
}

double mcc_calcul_isat(vgs,vbs,type_t)
double vgs;
double vbs;
int type_t;
{
 double is ;
 double vtn = MCC_VTN + MCC_KTN * vbs ;
 double vtp = MCC_VTP + MCC_KTP * vbs ;

 if(type_t == MCC_TRANS_N)
 {
  if(vgs <= vtn) return(0.0);
  is = (MCC_AN*(vgs-vtn)*(vgs-vtn))/(1.0+MCC_BN*(vgs-vtn));
 }
 else
 {
  if(vgs <= vtp) return(0.0);
  is = (MCC_AP*(vgs-vtp)*(vgs-vtp))/(1.0+MCC_BP*(vgs-vtp));
 }

 return(is);
}

double mcc_calcul_ures(vgs,vbs,type_t)
double vgs;
double vbs;
int type_t;
{
 double i0;
 double kres;
 double ures;
 double vtn = MCC_VTN + MCC_KTN * vbs ;
 double vtp = MCC_VTP + MCC_KTP * vbs ;

 if(type_t == MCC_TRANS_N)
 {
  if(vgs <= vtn) return(0.0);
  i0 = mcc_calcul_isat(MCC_VDDmax,vbs,type_t) ;
  kres = ((MCC_RNT*i0-(MCC_RNT*MCC_VDDmax)/MCC_RNS)/(1.0 - MCC_RNT/MCC_RNS))/(MCC_VDDmax-vtn);
  ures = MCC_KRN*kres*(vgs-vtn) + kres*(1.0-MCC_KRN)*(MCC_VDDmax - vtn) ;
 }
 else
 {
  if(vgs <= vtp) return(0.0);
  i0 = mcc_calcul_isat(MCC_VDDmax,vbs,type_t) ;
  kres = ((MCC_RPT*i0-(MCC_RPT*MCC_VDDmax)/MCC_RPS)/(1.0 - MCC_RPT/MCC_RPS))/(MCC_VDDmax-vtp);
  ures = MCC_KRP*kres*(vgs-vtp) + kres*(1.0-MCC_KRP)*(MCC_VDDmax - vtp) ;
 }

 return(ures);
}

double mcc_calcul_ires(vgs,vbs,type_t)
double vgs;
double vbs;
int type_t;
{
 double ires ;
 double vtn = MCC_VTN + MCC_KTN * vbs ;
 double vtp = MCC_VTP + MCC_KTP * vbs ;

 if(type_t == MCC_TRANS_N)
    ires = (MCC_AN*(vgs-vtn)*(vgs-vtn))/(1.0+MCC_BN*(vgs-vtn)) - ((MCC_VDDmax - mcc_calcul_ures(vgs,vbs,type_t)) * (vgs - vtn)) / ((MCC_KSN * MCC_RNS * (MCC_VDDmax - vtn)) + (MCC_RNS * (1.0 - MCC_KSN) * (vgs-vtn))) ;
 else
    ires = (MCC_AP*(vgs-vtp)*(vgs-vtp))/(1.0+MCC_BP*(vgs-vtp)) - ((MCC_VDDmax - mcc_calcul_ures(vgs,vbs,type_t)) * (vgs - vtp)) / ((MCC_KSP * MCC_RPS * (MCC_VDDmax - vtp)) + (MCC_RPS * (1.0 - MCC_KSP) * (vgs-vtp))) ;

 return(ires) ;
}

void mcc_trans_mcc_from_global( int type_t, mcc_trans_mcc *trs )
{
  trs->VDD = MCC_VDDmax ;
  trs->RD  = 0.0 ;
  trs->RS  = 0.0 ;
  
  if( type_t == MCC_TRANS_N ) {
    trs->VG = MCC_VDDmax ;
    trs->VB = 0.0 ;
    trs->TRWIDTH  = (long)(0.5+MCC_WNeff * ((double)SCALE_X)) ;
    trs->TRLENGTH = (long)(0.5+MCC_LNeff * ((double)SCALE_X)) ;
    trs->A = MCC_AN ;
    trs->B = MCC_BN ;
    trs->RNT = MCC_RNT ;
    trs->RNS = MCC_RNS ;
    trs->VT  = MCC_VTN ;
    trs->KVT = MCC_KTN ;
    trs->VLIN = MCC_VDDmax * MCC_KRNT ;
    trs->VSAT = MCC_VDDmax * MCC_KRNS ;
    trs->KR = MCC_KRN ;
    trs->KS = MCC_KSN ;
    trs->TYPE = TRANSN ;
  }
  else {
    trs->VG = 0.0 ;
    trs->VB = MCC_VDDmax ;
    trs->TRWIDTH  = (long)(0.5+MCC_WPeff * ((double)SCALE_X)) ;
    trs->TRLENGTH = (long)(0.5+MCC_LPeff * ((double)SCALE_X)) ;
    trs->A = MCC_AP ;
    trs->B = MCC_BP ;
    trs->RNT = MCC_RPT ;
    trs->RNS = MCC_RPS ;
    trs->VT  = MCC_VTP ;
    trs->KVT = MCC_KTP ;
    trs->VLIN = MCC_VDDmax * MCC_KRPT ;
    trs->VSAT = MCC_VDDmax * MCC_KRPS ;
    trs->KR = MCC_KRP ;
    trs->KS = MCC_KSP ;
    trs->TYPE = TRANSP ;
  }
}

#define NEWMCCCURRENT

#ifdef NEWMCCCURRENT
double mcc_current(vgs,vds,vbs,type_t)
double vds ; /* valeur absolue */
double vgs ; /* valeur absolue */
double vbs ; /* valeur absolue */
int type_t ;
{
  mcc_trans_mcc  trs ;
  float          ids ;
  float          vs ;
  float          vd ;
  float          wl ;
  float          coef ;

  mcc_trans_mcc_from_global( type_t, &trs );
  trs.TRWIDTH  = 1.0 ;
  trs.TRLENGTH = 1.0 ;
  
  if( type_t == MCC_TRANS_N ) {
    wl = MCC_WNeff/MCC_LNeff ;
    vbs = -vbs ;
    coef = 1.0 ;
  }
  else {
    wl = MCC_WPeff/MCC_LPeff ;
    vgs = -vgs ;
    vds = -vds ;
    coef = -1.0 ;
  }

  vs = trs.VB - vbs ;
  vd = vds + vs ;
  trs.VG = vgs + vs ;

  if( ! mcc_mcc_ids( &trs, vs, vd, &ids ) )
    return 0.0 ;
  return ids*wl*coef ;
  
}
#else
double mcc_current(vgs,vds,vbs,type_t)
double vds;
double vgs;
double vbs;
int type_t;
{
 double aux;
 double isat;
 double ires;
 double is;
 double r;
 double rsat;
 double ures;
 double vtn = MCC_VTN + MCC_KTN * vbs ;
 double vtp = MCC_VTP + MCC_KTP * vbs ;
 double factor = 1.0 ;
 double vsat ;
 double vlin ;
 double rsatmin;

 if(vds < 0.0)
  {
   vgs = vgs - vds ;
   vbs = vbs - vds ;
   vds = -vds ;
   factor = -1.0 ;
  }

 if(type_t == MCC_TRANS_N)
 {
  if(vgs <= vtn) return(0.0);
  isat = mcc_calcul_isat(vgs,vbs,type_t) ;

  ures = mcc_calcul_ures(vgs,vbs,type_t) ;
  ires = mcc_calcul_ires(vgs,vbs,type_t) ;
  r = ures / ires ;

  rsat = (MCC_VDDmax - ures)/(isat - ires) ;
  rsatmin = MCC_VDDmax/isat ;

  if( rsat > rsatmin && ures > 0.0 ) {
    is = (MCC_VDDmax - vds)/rsat ;
    is = isat - is ;

    vsat = ((vgs-vtn)/(MCC_VDDmax-vtn))*(MCC_VDDmax * MCC_KRNS)  ;
    vlin = ((vgs-vtn)/(MCC_VDDmax-vtn))*(MCC_VDDmax * MCC_KRNT) ;

    {
      double ilin, isx, di, dv, a, b, iit, im, a1, b1, a2, b2, a3, b3, a4, b4 ;
      double ures2, ires2, iit2, im2, csat, psat, plin ;
    
      ilin = vlin/r ;
      plin = 1.0/r ;

      isx = isat-(MCC_VDDmax - vsat)/rsat ;
      psat = 1.0/rsat ;
      csat = isx-vsat*psat ;

      di=isx-ilin ;
      dv=(vsat-vlin) ;
      a=di/dv ;
      b=ilin-a*vlin ;

      iit=a*ures+b ;
      im=(ires-iit)/2.0+iit ;

      a1=(im-ilin)/(ures-vlin) ;
      b1=ilin-a1*vlin ;

      a2=(isx-im)/(vsat-ures) ;
      b2=isx-a2*vsat ;

      ures2=(csat-b1)/(a1-psat) ;
      ires2=psat*ures2+csat ;
      iit2=a2*ures2+b2 ;
      im2=(ires2-iit2)/2.0+iit2 ;

      a3=(im2-im)/(ures2-ures) ;
      b3=im2-a3*ures2 ;

      a4=(isx-im2)/(vsat-ures2) ;
      b4=isx-a4*vsat ;

      if( vds < vlin ) 
        aux = ( plin * vds ) * MCC_WNeff/MCC_LNeff ;
      if( vds >= vlin && vds < ures ) 
        aux = ( a1*vds + b1 ) * MCC_WNeff/MCC_LNeff ;
      if( vds >= ures && vds < ures2 ) 
        aux = ( a3*vds + b3 ) * MCC_WNeff/MCC_LNeff ;
      if( vds >= ures2 && vds < vsat ) 
        aux = ( a4*vds + b4 ) * MCC_WNeff/MCC_LNeff ;
      if( vds >= vsat ) 
        aux = ( psat*vds + csat ) * MCC_WNeff/MCC_LNeff ;

    }
  }
  else {
    aux = vds / rsatmin * MCC_WNeff/MCC_LNeff ;
  }
 }
 else
 {
  if(vgs <= vtp) return(0.0);
  isat = mcc_calcul_isat(vgs,vbs,type_t) ;

  ures = mcc_calcul_ures(vgs,vbs,type_t) ;
  ires = mcc_calcul_ires(vgs,vbs,type_t) ;
  r = ures / ires ;

  rsat = (MCC_VDDmax - ures)/(isat - ires) ;
  rsatmin = MCC_VDDmax/isat ;

  if( rsat > rsatmin && ures > 0.0 ) {
    is = (MCC_VDDmax - vds)/rsat ;
    is = isat - is ;

    vsat = ((vgs-vtp)/(MCC_VDDmax-vtp))*(MCC_VDDmax * MCC_KRPS )  ;
    vlin = ((vgs-vtp)/(MCC_VDDmax-vtp))*(MCC_VDDmax * MCC_KRPT ) ;

    {
      double ilin, isx, di, dv, a, b, iit, im, a1, b1, a2, b2, a3, b3, a4, b4 ;
      double ures2, ires2, iit2, im2, csat, psat, plin ;
    
      ilin = vlin/r ;
      plin = 1.0/r ;

      isx = isat-(MCC_VDDmax - vsat)/rsat ;
      psat = 1.0/rsat ;
      csat = isx-vsat*psat ;

      di=isx-ilin ;
      dv=(vsat-vlin) ;
      a=di/dv ;
      b=ilin-a*vlin ;

      iit=a*ures+b ;
      im=(ires-iit)/2.0+iit ;

      a1=(im-ilin)/(ures-vlin) ;
      b1=ilin-a1*vlin ;

      a2=(isx-im)/(vsat-ures) ;
      b2=isx-a2*vsat ;

      ures2=(csat-b1)/(a1-psat) ;
      ires2=psat*ures2+csat ;
      iit2=a2*ures2+b2 ;
      im2=(ires2-iit2)/2.0+iit2 ;

      a3=(im2-im)/(ures2-ures) ;
      b3=im2-a3*ures2 ;

      a4=(isx-im2)/(vsat-ures2) ;
      b4=isx-a4*vsat ;

      if( vds < vlin ) 
        aux = ( plin * vds ) * MCC_WPeff/MCC_LPeff ;
      if( vds >= vlin && vds < ures ) 
        aux = ( a1*vds + b1 ) * MCC_WPeff/MCC_LPeff ;
      if( vds >= ures && vds < ures2 ) 
        aux = ( a3*vds + b3 ) * MCC_WPeff/MCC_LPeff ;
      if( vds >= ures2 && vds < vsat ) 
        aux = ( a4*vds + b4 ) * MCC_WPeff/MCC_LPeff ;
      if( vds >= vsat ) 
        aux = ( psat*vds + csat ) * MCC_WPeff/MCC_LPeff ;

    }
  }
  else {
    aux = vds / rsatmin * MCC_WPeff/MCC_LPeff ;
  }
 }

 return(aux*factor);
}
#endif

double mcc_spicecurrent(vgs,vds,vbs,type,lotrsparam)
double vgs ;
double vds ;
double vbs ;
int type ;
elp_lotrs_param *lotrsparam;
{
 if(MCC_CALC_CUR == MCC_SIM_MODE)
  return(mcc_spicesimcurrent(vgs,vds,vbs,type)) ;
 else
  return(mcc_spicecalccurrent(vgs,vds,vbs,type,lotrsparam)) ;
}

double mcc_spicecalccurrent(vgs,vds,vbs,type,lotrsparam)
double vgs ;
double vds ;
double vbs ;
int type ;
elp_lotrs_param *lotrsparam;
{
 double gs ;
 double ds ;
 double bs ;
 double w ;
 double l ;
 char *transname ;
 int typet, corner ;
 double factor = 1.0 ;

 if(vds < 0.0)
  {
   vgs = vgs - vds ;
   vbs = vbs - vds ;
   vds = -vds ;
   factor = -1.0 ;
  }

 if(type == MCC_TRANS_N)
  {
   typet = MCC_NMOS ;
   corner = MCC_NCASE ;
   transname = MCC_TNMODEL ;
   ds = vds ;
   gs = vgs ;
   bs = vbs ;
   w = MCC_WN ;
   l = MCC_LN ;
  }
 else
  {
   typet = MCC_PMOS ;
   corner = MCC_PCASE ;
   transname = MCC_TPMODEL ;
   ds = -vds ;
   gs = -vgs ;
   bs = vbs ;
   w = MCC_WP ;
   l = MCC_LP ;
  }

 return(factor * fabs(mcc_calcIDS(MCC_MODELFILE, transname,
                   typet, corner, bs, gs, 
                   ds, l*1.0e-6, w*1.0e-6,MCC_TEMP,lotrsparam))) ;
}

static int mcc_optiom_resetsearch=1;
void mcc_optim_resetsearch_for_new_pwl()
{
   mcc_optiom_resetsearch=1;
}
double mcc_spicesimcurrent(vgs,vds,vbs,type)
double vgs ;
double vds ;
double vbs ;
int type ;
{
 double v, res ;
 double *tab ;
 int step ;
 int stepmax, *curidx ;
 mbk_pwl *mypwl;
 static int MCC_OPTIM_IDNVGS_idx, MCC_OPTIM_IDNRES_idx, MCC_OPTIM_IDNSAT_idx;
 static int MCC_OPTIM_IDPVGS_idx, MCC_OPTIM_IDPRES_idx, MCC_OPTIM_IDPSAT_idx;

#ifdef OPTIM5
 if (MCC_OPTIM_MODE)
 {
   if (mcc_optiom_resetsearch)
   {
     MCC_OPTIM_IDNVGS_idx=MCC_OPTIM_IDNRES_idx=MCC_OPTIM_IDNSAT_idx=-1;
     MCC_OPTIM_IDPVGS_idx=MCC_OPTIM_IDPRES_idx=MCC_OPTIM_IDPSAT_idx=-1;
     mcc_optiom_resetsearch=0;
   }
   if((mcc_ftol(MCC_VGS * 1000.0) == mcc_ftol(vgs * 1000.0)) &&
      (mcc_ftol(MCC_VDDmax * 1000.0) != mcc_ftol(vds * 1000.0)) &&
      (mcc_ftol(MCC_VGS * 1000.0) != mcc_ftol(MCC_VDDmax * 1000.0)))
    {
     v = vds ;
     if(type == MCC_TRANS_N)
       mypwl = MCC_OPTIM_IDNVGS, curidx=&MCC_OPTIM_IDNVGS_idx ;
     else
       mypwl = MCC_OPTIM_IDPVGS, curidx=&MCC_OPTIM_IDPVGS_idx ;
    }
   else if(vgs > vds)
    {
     v = vds ;
     if(type == MCC_TRANS_N)
       mypwl = MCC_OPTIM_IDNRES, curidx=&MCC_OPTIM_IDNRES_idx ;
     else
       mypwl = MCC_OPTIM_IDPRES, curidx=&MCC_OPTIM_IDPRES_idx ;
    }
   else
    {
     v = vgs ;
     if(type == MCC_TRANS_N)
       mypwl = MCC_OPTIM_IDNSAT, curidx=&MCC_OPTIM_IDNSAT_idx ;
     else
       mypwl = MCC_OPTIM_IDPSAT, curidx=&MCC_OPTIM_IDPSAT_idx ;
    }
   if (*curidx!=-1 && v<mypwl->DATA[ *curidx ].X0) *curidx=-1;
   mbk_pwl_get_value_bytabindex (mypwl,v,&res,curidx);
   if(type == MCC_TRANS_P) res=fabs(res);
   return res;
 }
 else
#endif
 {
   if((mcc_ftol(MCC_VGS * 1000.0) == mcc_ftol(vgs * 1000.0)) &&
      (mcc_ftol(MCC_VDDmax * 1000.0) != mcc_ftol(vds * 1000.0)) &&
      (mcc_ftol(MCC_VGS * 1000.0) != mcc_ftol(MCC_VDDmax * 1000.0)))
    {
     v = vds ;
     if(type == MCC_TRANS_N)
       tab = MCC_IDNVGS ;
     else
       tab = MCC_IDPVGS ;
    }
   else if(vgs > vds)
    {
     v = vds ;
     if(type == MCC_TRANS_N)
       tab = MCC_IDNRES ;
     else
       tab = MCC_IDPRES ;
    }
   else
    {
     v = vgs ;
     if(type == MCC_TRANS_N)
       tab = MCC_IDNSAT ;
     else
       tab = MCC_IDPSAT ;
    }

   step = mcc_ftoi((v) / (MCC_DC_STEP)) ;
   stepmax =  mcc_ftoi((MCC_VDDmax) / (MCC_DC_STEP)) ;
   
   if(step > stepmax)
     step = stepmax ;

   return(tab[step]) ;
 }
#ifndef __ALL__WARNING_
 vbs = 0.0;
#endif
}

double mcc_spicevdeg(type,lotrsparam)
int type ;
elp_lotrs_param *lotrsparam;
{
 double v ;

 if(MCC_CALC_CUR == MCC_SIM_MODE)
  {
   if(type == MCC_TRANS_N)
    {
     v = MCC_VDDDEG[1] ;
     if(v > (MCC_VDDmax - MCC_VTN)) 
       v = MCC_VDDmax - MCC_VTN ;
     if(v < (0.55*MCC_VDDmax)) 
       v = 0.55*MCC_VDDmax;
    }
   else
    {
     v = MCC_VSSDEG[1] ;
     if(v < MCC_VTP)
       v = MCC_VTP ;
    }
  }
 else
  {
   if(type == MCC_TRANS_N)
    {
     v = mcc_calcVDDDEG(MCC_MODELFILE, MCC_TNMODEL,
                        MCC_NMOS, MCC_NCASE, MCC_LN*1.0e-6, MCC_WN*1.0e-6,
                        MCC_VDDmax, MCC_TEMP, MCC_DC_STEP,lotrsparam) ;
     if(v > (MCC_VDDmax - MCC_VTN)) 
       v = (MCC_VDDmax - MCC_VTN)/ (1+MCC_KTN) ;
     if(v < (0.55*MCC_VDDmax)) 
       v = 0.55*MCC_VDDmax;
    }
   else
    {
     v = mcc_calcVSSDEG(MCC_MODELFILE, MCC_TPMODEL,
                        MCC_PMOS, MCC_PCASE, MCC_LP*1.0e-6, MCC_WP*1.0e-6,
                        MCC_VDDmax, MCC_TEMP, MCC_DC_STEP,lotrsparam) ;
     if(v < MCC_VTP)
       v = MCC_VTP/(1-MCC_KTP) ;
    }

  }
 return(v) ;
}

double mcc_calcdiffmodelsat(double vmin,double vmax,int type,elp_lotrs_param *lotrsparam, double exitvalue, mcc_corner_info *info)
{
 double var,vbs ;
 double res = 0.0 ;
 double imod ;
 double ispice ;
 double val ;
 int cont = 0 ;

 if ( MCC_OPTIM_MODE ) 
  {
   if( type == MCC_TRANS_N && MCC_OPTIM_IDNSAT )
    return mcc_optim_calcdiffmodelsat (type,MCC_OPTIM_IDNSAT,exitvalue,info);
   else if ( type == MCC_TRANS_P && MCC_OPTIM_IDPSAT )
    return mcc_optim_calcdiffmodelsat (type,MCC_OPTIM_IDPSAT,exitvalue,info);
  }
 var = vmin + MCC_DC_STEP ;

 if( lotrsparam->ISVBSSET )
   vbs = lotrsparam->VBS ;
 else
   vbs = (type == MCC_TRANS_N) ? lotrsparam->VBULK : lotrsparam->VBULK-MCC_VDDmax;
 while (var <= vmax )
  {
   imod = mcc_current(var,MCC_VDDmax,0.0,type) ;
   ispice = mcc_spicecurrent(var,MCC_VDDmax,vbs,type,lotrsparam) ;
   if(ispice == 0.0)
    {
     var += MCC_DC_STEP;
     continue ;
    }
   val = (imod - ispice) * (imod - ispice) ;
   if(val > 1.0)
    {
     var += MCC_DC_STEP;
     continue ;
    }
   res += val ;
   if (exitvalue>=0 && res>exitvalue) return res;
   cont ++ ;
   var += MCC_DC_STEP;
  }

 return(res
#ifndef OPTIM0 
                 /(double)cont
#endif
                 ) ;
}

double mcc_calcul_vt_error( int type, elp_lotrs_param *lotrsparam_n, elp_lotrs_param *lotrsparam_p, double testvt, mcc_corner_info *info )
{
  double v1 ;
  double emin ;
  double savevt ;
  elp_lotrs_param *trs_param;
  
  if( type == MCC_TRANS_N ) {
    savevt = MCC_VTN ;
    MCC_VTN = testvt ;
    trs_param = lotrsparam_n ;
  }
  else {
    savevt = MCC_VTP ;
    MCC_VTP = testvt ;
    trs_param = lotrsparam_p ;
  }
    
  if( MCC_NEW_CALC_ABR && MCC_OPTIM_MODE ) {
    emin = mcc_get_best_abr_from_vt ( type, lotrsparam_n, lotrsparam_p, info ) ;
  }
  else {
    v1 = (MCC_VDDmax-testvt)/2.0 + testvt  ;
    mcc_calcul_abr( type, v1, MCC_VDDmax, lotrsparam_n, lotrsparam_p ) ;
    emin = mcc_calcdiffmodelsat( testvt, MCC_VDDmax, type, trs_param, -1,NULL ) ;
  }

  if( type == MCC_TRANS_N ) 
    MCC_VTN = savevt ;
  else
    MCC_VTP = savevt ;

  return emin ;
}

void mcc_calcul_vt_old( int type, elp_lotrs_param *lotrsparam_n, elp_lotrs_param *lotrsparam_p, mcc_corner_info *info )
{
 double deltavt ;
 double delta ;
 double vmin ;
 double vmax ;
 double vhalf ;
 double vhalf1 ;
 double vhalf2 ;
 double vstep ;
 int    cmin ;
 int    cmax ;
 int    chalf ;
 double emin ;
 double emax ;
 double ehalf ;
 double ehalf1 ;
 double ehalf2 ;
 char   *env;
 static FILE *vtfile=NULL;

 if ( !MCC_CALC_VT )
   return;
#if 0
 if (!vtfile)
 {
   char *where=getenv("VTFILE");
   if (where!=NULL)
   {
     vtfile=fopen(where, "a");
     fprintf(vtfile,"***\n");
   }
 }
#endif
 env = getenv( "MCC_DELTA_VT" ) ;
 if ( env ) 
   deltavt = atof ( env );
 else
   deltavt = 0.5;

 if(type == MCC_TRANS_N) {
   MCC_KSN = 1.0;
   MCC_KRN = 1.0;
   vmin = MCC_VTN * (1.0-deltavt) ;
   vmax = MCC_VTN * (1.0+deltavt) ;
 }
 else {
   if(type == MCC_TRANS_P) {
     MCC_KSP = 1.0;
     MCC_KRP = 1.0;
     vmin = MCC_VTP * (1.0-deltavt) ;
     vmax = MCC_VTP * (1.0+deltavt) ;
   }
   else {
     MCC_KSN = 1.0;
     MCC_KRN = 1.0;
     MCC_KSP = 1.0;
     MCC_KRP = 1.0;
   }
 }

 if ( !mcc_check_param ( type) )
   return;

 if( V_BOOL_TAB[ __MCC_PRECISE_VT ].VALUE ) {
   emin=1e10 ;
   if(vmax > 0.9*MCC_VDDmax)
     vmax = 0.9*MCC_VDDmax ;
   if(vmin > 0.8*MCC_VDDmax)
     vmin = 0.8*MCC_VDDmax ;
   vstep = (vmax-vmin)/100.0 ;
   for( vhalf = vmin ; vhalf <= vmax ; vhalf = vhalf+vstep ) {
     ehalf = mcc_calcul_vt_error( type, lotrsparam_n,lotrsparam_p, vhalf, info );
     if( ehalf < emin ) {
       emin   = ehalf ;
       deltavt = vhalf ;
     }
   }
   if( type == MCC_TRANS_P )
     MCC_VTP = deltavt ;
   else
     MCC_VTN = deltavt ;
   return ;
 }

 if( type == MCC_TRANS_N || type == MCC_TRANS_P ) {
 
   if(vmax > MCC_VDDmax)
     vmax = MCC_VDDmax ;

   delta = (vmax-vmin)/1000.0 ;

   vhalf = (vmin+vmax)/2.0 ;
   cmin  = 0 ;
   cmax  = 0 ;
   chalf = 0 ;

   do {
    
     if( !cmin ) {
       emin = mcc_calcul_vt_error( type, lotrsparam_n,lotrsparam_p, vmin, NULL );
       cmin = 1 ;
     }
       
     if( !chalf ) {
       ehalf = mcc_calcul_vt_error( type, lotrsparam_n,lotrsparam_p, vhalf, NULL );
       chalf = 1 ;
     }
    
     if( !cmax ) {
       emax = mcc_calcul_vt_error( type, lotrsparam_n,lotrsparam_p, vmax, NULL );
       cmax = 1 ;
     }

     if( emin < ehalf && ehalf < emax ) {
       vmax = vhalf ;
       emax = ehalf ;
       vhalf = (vmin+vmax)/2.0 ;
       chalf = 0 ;
     }
     else {
       if( emin > ehalf && ehalf > emax ) {
         vmin = vhalf ;
         emin = ehalf ;
         vhalf = (vmin+vmax)/2.0 ;
         chalf = 0 ;
       }
       else {
       
         if( emin > ehalf && emax > ehalf ) {
         
           vhalf1 = (vmin+vhalf)/2.0 ;
           vhalf2 = (vmax+vhalf)/2.0 ;
           ehalf1 = mcc_calcul_vt_error( type, lotrsparam_n,lotrsparam_p, vhalf1, NULL );
           ehalf2 = mcc_calcul_vt_error( type, lotrsparam_n,lotrsparam_p, vhalf2, NULL );
           
           if( ehalf1 < ehalf2 ) {
             vmax = vhalf ;
             emax = ehalf ;
             vhalf = vhalf1 ;
             ehalf = ehalf1 ;
           }
           else {
             vmin = vhalf ;
             emin = ehalf ;
             vhalf = vhalf2 ;
             ehalf = ehalf2 ;
           }
         }
         else {
           if( emin < emax ) {
             vmax = vmin ;
             emax = emin ;
             vhalf = vmin ;
             ehalf = emin ;
           }
           else {
             vmin = vmax ;
             emin = emax ;
             vhalf = vmax ;
             ehalf = emax ;
           }
         }
       }
     }
   }
   while( (vmax-vmin) > delta );
   
   if( type == MCC_TRANS_N )
      MCC_VTN = vhalf ;
   else
      MCC_VTP = vhalf ;

   if (vtfile!=NULL)
   {
      double vt0=(type != MCC_TRANS_N)?MCC_VTP:MCC_VTN;
      fprintf(vtfile, "%g %g\n",(vhalf-vt0)/vt0, ehalf);
   }
 }
}

void mcc_calcul_vt_new( int type, elp_lotrs_param *lotrsparam_n, elp_lotrs_param *lotrsparam_p )
{
 double deltavt ;
 double delta ;
 double vmin ;
 double vmax ;
 double vhalf ;
 int    cmin ;
 int    cmax ;
 int    chalf ;
 double emin ;
 double emax ;
 double ehalf ;
 double ehalfp ;
 double pente ;
 double lmax ;
 char   *env;

 if ( !MCC_CALC_VT )
   return;

 env = getenv("MCC_DELTA_VT") ;
 if ( env ) 
   deltavt = atof ( env );
 else
   deltavt = 0.5;

 if(type == MCC_TRANS_N) {
   MCC_KSN = 1.0;
   MCC_KRN = 1.0;
   vmin = MCC_VTN * (1.0-deltavt) ;
   vmax = MCC_VTN * (1.0+deltavt) ;
   lmax = MCC_VDDmax - (MCC_VDDmax-MCC_VTN)/10.0 ;
   if( vmax > lmax )
     vmax = lmax ;
 }
 else {
   if(type == MCC_TRANS_P) {
     MCC_KSP = 1.0;
     MCC_KRP = 1.0;
     vmin = MCC_VTP * (1.0-deltavt) ;
     vmax = MCC_VTP * (1.0+deltavt) ;
     lmax = MCC_VDDmax - (MCC_VDDmax-MCC_VTP)/10.0 ;
     if( vmax > lmax )
       vmax = lmax ;
   }
   else {
     MCC_KSN = 1.0;
     MCC_KRN = 1.0;
     MCC_KSP = 1.0;
     MCC_KRP = 1.0;
   }
 }

 if ( !mcc_check_param ( type) )
   return;

 if( type == MCC_TRANS_N || type == MCC_TRANS_P ) {
 

   cmin  = 0 ;
   cmax  = 0 ;
   chalf = 0 ;

   /* check domain validy of mcc_calcul_vt_error() since allowing negative
      capacitance may leads to forbiden values for vt */
   delta = (vmax-vmin)/20.0 ;
   do {
     emax = mcc_calcul_vt_error( type, lotrsparam_n,lotrsparam_p, vmax, NULL );
     if( emax > FLT_MAX/2.0 ) {
       cmax = 0 ;
       vmax = vmax - delta ;
       if( vmax <= vmin )
         break ;
     }
     else 
       cmax = 1 ;
     
   }
   while( !cmax );
   
   if( !cmax ) {
     printf( "can't find a valid solution for VT\n");
     exit(1);
   }
   
   vhalf = (vmin+vmax)/2.0 ;

   delta = (vmax-vmin)/1000.0 ;
   
   do {
    
     if( !cmin ) {
       emin = mcc_calcul_vt_error( type, lotrsparam_n,lotrsparam_p, vmin, NULL );
       cmin = 1 ;
     }
       
     if( !chalf ) {
       ehalf = mcc_calcul_vt_error( type, lotrsparam_n,lotrsparam_p, vhalf, NULL );
       chalf = 1 ;
     }

     if( !cmax ) {
       emax = mcc_calcul_vt_error( type, lotrsparam_n,lotrsparam_p, vmax, NULL );
       cmax = 1 ;
     }

     if( emin > FLT_MAX/2.0 || ehalf > FLT_MAX/2.0 || emax > FLT_MAX/2.0 ) {
       printf( "validity domain for vt unconsistant\n" );
       exit(1);
     }

     if( emin < ehalf && ehalf < emax ) {
       vmax = vhalf ;
       emax = ehalf ;
       vhalf = (vmin+vmax)/2.0 ;
       chalf = 0 ;
     }
     else {
       if( emin > ehalf && ehalf > emax ) {
         vmin = vhalf ;
         emin = ehalf ;
         vhalf = (vmin+vmax)/2.0 ;
         chalf = 0 ;
       }
       else {
       
         if( emin > ehalf && emax > ehalf ) {
         
           ehalfp = mcc_calcul_vt_error( type, lotrsparam_n,lotrsparam_p, vhalf+delta/2.0, NULL );
           pente = (ehalfp-ehalf)/(delta/2.0);

           if( pente > 0.0 ) {
             vmax = vhalf ;
             emax = ehalf ;
           }
           else {
             vmin = vhalf ;
             emin = ehalf ;
           }

           vhalf = (vmin+vmax)/2.0 ;
           chalf = 0 ;
           
         }
         else {
           if( emin < emax ) {
             vmax = vmin ;
             emax = emin ;
             vhalf = vmin ;
             ehalf = emin ;
           }
           else {
             vmin = vmax ;
             emin = emax ;
             vhalf = vmax ;
             ehalf = emax ;
           }
         }
       }
     }
   }
   while( (vmax-vmin) > delta );
   
   if( type == MCC_TRANS_N )
      MCC_VTN = vhalf ;
   else
      MCC_VTP = vhalf ;
 }
}

int mcc_get_sat_parameter( double v1, double i1,
                           double v2, double i2,
                           double v3, double i3,
                           double *a,
                           double *b,
                           double *vt
                         )
{
  double r1 ;
  double r2 ;
  double s1 ;
  double s2 ;
  double t1 ;
  double t2 ;
  double ta ;
  double tb ;
  double tc ;
  double delta ;
  double a1 ;
  double b1 ;
  double vt1 ;
  double a2 ;
  double b2 ;
  double vt2 ;
  double bmin ;
  int    valid1 ;
  int    valid2 ;
  int    sol ;
  int    found ;

  r1 = i1*(v1-v3) ;
  r2 = i2*(v2-v3) ;
  s1 = 1.0-i1/i3 ;
  s2 = 1.0-i2/i3 ;
  t1 = i1*v3/i3 - v1 ;
  t2 = i2*v3/i3 - v2 ;

  ta = s2*r1 - s1*r2 ;
  tb = v1*s1*r2 - t1*r2 - v2*s2*r1 + t2*r1 ;
  tc = v1*t1*r2 - v2*t2*r1 ;

  delta = tb*tb - 4.0*ta*tc ;

  if( delta >= 0.0 ) {
  
    vt1 = ( -tb - sqrt( delta ) ) / ( 2.0*ta ) ;
    a1  = i1*(v1-v3)/( (v3-vt1)*(v1-vt1)*(i1*(v3-vt1)/i3-(v1-vt1))) ;
    b1  = a1*(v3-vt1)/i3-1.0/(v3-vt1) ;

    vt2 = ( -tb + sqrt( delta ) ) / ( 2.0*ta ) ;
    a2  = i1*(v1-v3)/( (v3-vt2)*(v1-vt2)*(i1*(v3-vt2)/i3-(v1-vt2))) ;
    b2  = a2*(v3-vt2)/i3-1.0/(v3-vt1) ;

    if( mcc_check_sat_param( a1, b1, vt1, MCC_VDDmax, 'T' ) && vt1 < v1 )
      valid1 = 1 ;
    else
      valid1 = 0 ;

    if( mcc_check_sat_param( a2, b2, vt2, MCC_VDDmax, 'T' ) && vt2 < v1 )
      valid2 = 1 ;
    else
      valid2 = 0 ;
      
    sol = 0 ;
    if( valid1 && valid2 ) {
      if( vt1 < vt2 )
        sol = 1 ;
      else 
        sol = 2 ;
    }
    else {
      if( valid1 )
        sol = 1 ;
      if( valid2 )
        sol = 2 ;
    }

    if( sol>0 ) {
      found = 1 ;
      if( sol==1 ) {
        *a  = a1 ;
        *b  = b1 ;
        *vt = vt1 ;
      }
      else {
        *a  = a2 ;
        *b  = b2 ;
        *vt = vt2 ;
      }
    }
    else 
      found = 0 ;
  }
  else
    found = 0 ;

  return found ;
}

int mcc_get_iflex_isat( mbk_pwl *pwl, double *vgs, double *ids )
{
  double amin ;
  int    i ;
  int    inflex ;
  int    ret ;

  amin   = pwl->DATA[0].A ;
  inflex = 0 ;
  
  for( i=1; i<pwl->N ; i++ ) {
    if( pwl->DATA[i].A < amin ) {
      inflex = 1 ;
      break ;
    }
    amin = pwl->DATA[i].A ;
  }
  
  if( inflex ) {
    if( vgs )
      *vgs = pwl->DATA[i].X0 ;
    if( ids )
      *ids = pwl->DATA[i].A * pwl->DATA[i].X0 + pwl->DATA[i].B ;
    ret = 1 ;
  }
  else
    ret = 0 ;

  return ret ;
}

int mcc_check_sat_param( double a,
                         double b,
                         double vt,
                         double vdd,
                         char   type /* 'B'=branch, 'T'=transistor */
                       )
{
  int allownegativeb ;
  double bmin ;

  if( type == 'B' )
    allownegativeb = V_BOOL_TAB[ __MCC_ALLOW_NEGATIVE_B_BRANCH ].VALUE ;
  else /* 'T' */
    allownegativeb = V_BOOL_TAB[ __MCC_ALLOW_NEGATIVE_B ].VALUE ;

  if( allownegativeb )
    bmin = -1.0/(vdd-vt);
  else 
    bmin = 0.0 ;

  if( vt > vdd ) return 0 ;
  if( vt < 0.0 ) return 0 ;
  if( b < bmin ) return 0 ;
  if( a < 0.0  ) return 0 ;

  return 1 ;
}
void mcc_calcul_sat_parameter( int type, elp_lotrs_param *lotrsparam, mcc_corner_info *info )
{
  double   vs ;
  double   vbs ;
  double   *pt_a ; 
  double   *pt_b ;
  double   *pt_vt ;
  double   *tabvgs ;
  double   ids ;
  double   vgs ;
  double   imax ;
  double   vgsmax ;
  double   *tabids ;
  double   best_a ;
  double   best_b ;
  double   best_vt ;
  double   error ;
  double   minerror ;
  double   pmin ;
  double   pmax ;
  double   wl ;
  double   sgn ;
  double   ids0 ;
  double   ids1 ;
  double   vgs0 ;
  double   vgs1 ;
  int      n ;
  int      i ;
  int      j ;
  int      r ;
  int      found ;
  int      onemoretime ;
  mbk_pwl *spipwl ;
  static int fordebug ;

  fordebug++ ;

  if( type == MCC_TRANS_N ) {
    vs     = 0.0 ;
    pt_a   = &MCC_AN ;
    pt_b   = &MCC_BN ;
    pt_vt  = &MCC_VTN ;
    wl     = (MCC_WN+MCC_DWN)/(MCC_LN+MCC_DLN) ;
    spipwl = MCC_OPTIM_IDNSAT ;
    sgn    = 1.0 ;
  }
  else {
    vs     = MCC_VDDmax ;
    pt_a   = &MCC_AP ;
    pt_b   = &MCC_BP ;
    pt_vt  = &MCC_VTP ;
    wl     = (MCC_WP+MCC_DWP)/(MCC_LP+MCC_DLP) ;
    spipwl = MCC_OPTIM_IDPSAT ;
    sgn    = -1.0 ;
  }
  
  if( lotrsparam->ISVBSSET )
    vbs = lotrsparam->VBS ;
  else
    vbs = lotrsparam->VBULK - vs ;

  n = V_INT_TAB[ __MCC_SAT_N ].VALUE ;

  tabvgs = (double*)alloca( sizeof(double)*(n-1) );
  tabids = (double*)alloca( sizeof(double)*(n-1) );

  vgsmax = MCC_VDDmax ;
  mbk_pwl_get_value( spipwl, MCC_VDDmax, &imax );
  imax = sgn*imax ;

  ids0 = imax*1.0/((double)n);
  ids1 = imax*2.0/((double)n);
  
  do {
    mbk_pwl_get_inv_value( spipwl, sgn*ids0, &vgs );
    tabvgs[0] = vgs ;
    tabids[0] = mcc_spicecurrent( vgs, MCC_VDDmax, vbs, type, lotrsparam ) ;
    mbk_pwl_get_inv_value( spipwl, sgn*ids1, &vgs );
    tabvgs[1] = vgs ;
    tabids[1] = mcc_spicecurrent( vgs, MCC_VDDmax, vbs, type, lotrsparam ) ;

    pmin = ( tabids[1] - tabids[0] ) / ( tabvgs[1] - tabvgs[0] ) ;
    pmax = ( imax - tabids[1] ) / ( vgsmax - tabvgs[1] ) ;

    if( pmin > 0.95 * pmax ) {
      ids0 = ids0/2.0 ;
      ids1 = ids1/2.0 ;
      onemoretime = 1 ;
    }
    else
      onemoretime = 0 ;
  }
  while( onemoretime );
  
  
  for( i=3 ; i<n  ; i++ ) {
      
    //ids = (imax-ids1)*((double)(i-2))/((double)(n-2))+ids1;
    ids = (imax-tabids[1])*((double)(i-2))/((double)(n-2))+tabids[1];

    mbk_pwl_get_inv_value( spipwl, sgn*ids, &vgs );
    
    tabvgs[i-1] = vgs ;
    tabids[i-1] = mcc_spicecurrent( tabvgs[i-1], MCC_VDDmax, vbs, type, lotrsparam ) ;
  }

  minerror = MAXFLOAT ;

  found = 0 ;
  for( i=0 ; i<(n-2) ; i++ ) {
  
    for( j=(i+1) ; j<(n-1) ; j++ ) {
    
      r = mcc_get_sat_parameter( tabvgs[i], tabids[i]/wl,
                                 tabvgs[j], tabids[j]/wl,
                                 vgsmax,    imax/wl,
                                 pt_a,
                                 pt_b,
                                 pt_vt
                               ) ;
      if( !r )
        continue ;

      error = mcc_calcdiffmodelsat( *pt_vt, MCC_VDDmax, type, lotrsparam, minerror, info ) ;
      if( error < minerror ) {
        best_a   = *pt_a ;
        best_b   = *pt_b ;
        best_vt  = *pt_vt ;
        minerror = error ;
        found    = 1 ;
      }
    }
  }
  
  if( !found ) {
    printf( "can't find a valid set of parameter A, B, VT (fordebug=%d)\n", fordebug );
    exit(0);
  }
  
  *pt_a  = best_a ;
  *pt_b  = best_b ;
  *pt_vt = best_vt ;
}

void mcc_calcul_vt( int type, elp_lotrs_param *lotrsparam_n, elp_lotrs_param *lotrsparam_p, mcc_corner_info *info )
{
  if( V_BOOL_TAB[ __MCC_CALCUL_VT_NEW].VALUE ) {
    if( type == MCC_TRANS_B ) {
      mcc_calcul_vt_new( MCC_TRANS_N, lotrsparam_n, lotrsparam_p );
      mcc_calcul_vt_new( MCC_TRANS_P, lotrsparam_n, lotrsparam_p );
    }
    else
      mcc_calcul_vt_new( type, lotrsparam_n, lotrsparam_p );
  }
  else {
    if( type == MCC_TRANS_B ) {
      mcc_calcul_vt_old( MCC_TRANS_N, lotrsparam_n, lotrsparam_p, info );
      mcc_calcul_vt_old( MCC_TRANS_P, lotrsparam_n, lotrsparam_p, info );
    }
    else
      mcc_calcul_vt_old( type, lotrsparam_n, lotrsparam_p, info );
  }
}

double mcc_calcul_a(v1,v2,i1,i2,vt,l,w)
double v1 ;
double v2 ;
double i1 ;
double i2 ;
double vt ;
double l ;
double w ;
{
 double g1 ;
 double g2 ;
 double ct ;
 double ai1 ;
 double ai2 ;
 double res ;
 
 g1 = fabs(v1 - vt);
 g2 = fabs(v2 - vt);

 ai1 = fabs(i1) ;
 ai2 = fabs(i2) ;

 ct= w/l ;

 res = (ai1*ai2*(g2-g1))/(g1*g2*ct*((ai2*g1)-(ai1*g2))) ;

 return(res);
}

double mcc_calcul_b(v1,v2,i1,i2,vt,l,w)
double v1 ;
double v2 ;
double i1 ;
double i2 ;
double vt ;
double l ;
double w ;
{
 double g1 ;
 double g2 ;
 double ct ;
 double ai1 ;
 double ai2 ;
 double res ;
 
 g1 = fabs(v1 - vt);
 g2 = fabs(v2 - vt);

 ai1 = fabs(i1) ;
 ai2 = fabs(i2) ;

 ct= w/l ;

 res = ((ai2*g1*g1)-(ai1*g2*g2))/(g1*g2*((ai1*g2)-(ai2*g1))) ;

 return(res);
}

void mcc_calcul_ab(v1,v2,i1,i2,vt,l,w,a,b)
double v1 ;
double v2 ;
double i1 ;
double i2 ;
double vt ;
double l ;
double w ;
double *a ;
double *b ;
{
 double g1 ;
 double g2 ;
 double ct ;
 double ai1 ;
 double ai2 ;
 
 g1 = v1 - vt;
 g2 = v2 - vt;

 ai1 = i1 ;
 ai2 = i2 ;

 ct= w/l ;

 *a = (ai1*ai2*(g2-g1))/(g1*g2*ct*((ai2*g1)-(ai1*g2))) ;
 *b = ((ai2*g1*g1)-(ai1*g2*g2))/(g1*g2*((ai1*g2)-(ai2*g1))) ;
}

double mcc_calcul_r(v,i,l,w)
double v ;
double i ;
double l ;
double w ;
{
 double r ;
 double ct ;

 ct = (w/l);

 r = (v/i) * ct;

 return(r);
}

double mcc_calcul_rs(v1,i1,v2,i2,l,w)
double v1 ;
double v2 ;
double i1 ;
double i2 ;
double l ;
double w ;
{
 double ct ;

 ct = (w/l);

 return(fabs((v2-v1)/(i2-i1))*ct);
}

double mcc_calcul_k_error( int     type, 
                           double  vbs,
                           int     n, 
                           double *tabvgs, 
                           double *tabvsat, 
                           double *tabvmed, 
                           double *tabispisat, 
                           double *tabispimed 
                         ) 
{
  int i ;
  double sumerror ;
  double error ;
  double imccsat ;
  double imccmed ;

  sumerror = 0.0 ;
  
  for( i=0 ; i<n ; i++ ) {
  
    imccsat = mcc_current( tabvgs[i], tabvsat[i], vbs, type ) ;
    imccmed = mcc_current( tabvgs[i], tabvmed[i], vbs, type ) ;
    
    error =   (imccsat - tabispisat[i]) * (imccsat - tabispisat[i]) 
            + (imccmed - tabispimed[i]) * (imccmed - tabispimed[i]) ;
            
    sumerror = sumerror + error ;
  }

  return sumerror ;
}

void mcc_calcul_k_new( int type, elp_lotrs_param *lotrsparam)
{
  double  vt ;
  double *ptkr ;
  double *ptks ;
  double  vlin0 ;
  double  vsat0 ;
  double  rlin0 ;
  double  rsat0 ;
  double  vbs ;
  double  vgs ;
  double  ispimaxvdd ;
  double  tabvgs[3] ;
  double  ispimax[3] ;
  double  ispimed[3] ;
  double  ispisat[3] ;
  double  ispilin[3] ;
  double  vmed[3] ;
  double  vsat[3] ;
  double  vlin[3] ;
  double  l, w ;
  double  rsat ;
  double  rlin ;
  double  usat ;
  double  kres ;
  double  error ;
  double  minerror ;
  double  bestkr ;
  double  bestks ;
  double  vs ;
  int     calc ;
  int     n ;
 
  if( type == MCC_TRANS_N ) {
    vt    = MCC_VTN ;
    ptkr  = &MCC_KRN ;
    ptks  = &MCC_KSN ;
    vlin0 = MCC_VDDmax * MCC_KRNT ;
    vsat0 = MCC_VDDmax * MCC_KRNS ;
    l     = MCC_LN+MCC_DLN ;
    w     = MCC_WN+MCC_DWN ;
    rlin0 = MCC_RNT ;
    rsat0 = MCC_RNS ;
    vs    = 0.0 ;
  }
  else {
    vt    = MCC_VTP ;
    ptkr  = &MCC_KRP ;
    ptks  = &MCC_KSP ;
    vlin0 = MCC_VDDmax * MCC_KRPT ;
    vsat0 = MCC_VDDmax * MCC_KRPS ;
    l     = MCC_LP+MCC_DLP ;
    w     = MCC_WP+MCC_DWP ;
    rlin0 = MCC_RPT ;
    rsat0 = MCC_RPS ;
    vs    = MCC_VDDmax ;
  }
  
  calc = MCC_CALC_CUR ;
  if( MCC_VGS == MCC_VDDmax ) {
    *ptkr = 1.0 ;
    *ptks = 1.0 ;
  }

  if( ( MCC_VGS <= 1.1*vt ) || ( MCC_CALC_ORG == MCC_CALC_MODE ) ) {
    if( MCC_CALC_ORG == MCC_SIM_MODE ) {
      *ptkr = 1.0 ;
      *ptks = 1.0 ;
      return ;
    }
  }

  if( MCC_CALC_CUR == MCC_SIM_MODE )
    MCC_CALC_CUR = MCC_CALC_MODE ;

  if( lotrsparam->ISVBSSET )
    vbs = lotrsparam->VBS ;
  else
    vbs = lotrsparam->VBULK - vs ;

  tabvgs[0] = 0.25 * ( MCC_VDDmax-vt ) + vt ;
  tabvgs[1] = 0.50 * ( MCC_VDDmax-vt ) + vt ;
  tabvgs[2] = 0.75 * ( MCC_VDDmax-vt ) + vt ; 
  
  ispimaxvdd = mcc_calcul_isat( MCC_VDDmax, 0.0, type ) ;

  for( n=0 ; n<=2 ; n++ ) {

    vgs     = tabvgs[n] ;
    vlin[n] = ( vgs - vt ) / ( MCC_VDDmax - vt ) * vlin0 ;
    vsat[n] = ( vgs - vt ) / ( MCC_VDDmax - vt ) * vsat0 ;
    vmed[n] = ( MCC_VDDmax - vsat[n] ) / 2.0 + vsat[n] ;

    ispimax[n]  = mcc_spicecurrent( vgs, MCC_VDDmax, vbs, type, lotrsparam ) ;
    ispisat[n]  = mcc_spicecurrent( vgs, vsat[n],    vbs, type, lotrsparam ) ;
    ispilin[n]  = mcc_spicecurrent( vgs, vlin[n],    vbs, type, lotrsparam ) ;
    ispimed[n]  = mcc_spicecurrent( vgs, vmed[n],    vbs, type, lotrsparam ) ;
  }

  minerror = FLT_MAX ;
  
  for( n=0 ; n<=2 ; n++ ) {
  
    vgs  = tabvgs[n] ;

    rsat = mcc_calcul_rs( MCC_VDDmax, ispimax[n], vsat[n], ispisat[n], l, w ) ;
    rlin = mcc_calcul_r( vlin[n], ispilin[n], l, w ) ;
    
    usat = ( rlin * (ispimax[n]*l/w) - rlin * MCC_VDDmax / rsat ) / (1.0 - rlin/rsat ) ;
    kres = ( ( rlin0*ispimaxvdd - ( rlin0 * MCC_VDDmax ) / rsat0 ) / ( 1.0 - rlin0/rsat0 )) / ( MCC_VDDmax - vt ) ;
    
    *ptkr = ( kres * (MCC_VDDmax - vt) - usat) / (kres * ((MCC_VDDmax - vt) - (vgs - vt))) ;
    *ptks = ((vgs - vt) * (rsat - rsat0)) / (rsat0 * ((MCC_VDDmax - vt) - (vgs - vt))) ;
    
    error = mcc_calcul_k_error( type, 0.0, 3, tabvgs, vsat, vmed, ispisat, ispimed ) ;
    if( error < minerror ) {
      minerror = error ;
      bestkr = *ptkr ;
      bestks = *ptks ;
    }
  }

  *ptkr = bestkr ;
  *ptks = bestks ;

  MCC_CALC_CUR = calc ;
}
  
void mcc_calcul_k(type,lotrsparam)
int type ;
elp_lotrs_param *lotrsparam;
{
 double il ;
 double rl ;
 double is ;
 double rs ;
 double vlin ;
 double vsat ;
 double usat ;
 double imax ;
 double kres ;
 double i0 ;
 double vbs ;
 double vgs ;
 int step ;
 int calc ;

 if( V_BOOL_TAB[ __MCC_PRECISE_K ].VALUE ) {
   mcc_calcul_k_new( type, lotrsparam );
   return ;
 }

 calc = MCC_CALC_CUR ;

 if(type == MCC_TRANS_N)
 {
  if(MCC_VGS == MCC_VDDmax)
    {
     MCC_KRN = 1.0 ;
     MCC_KSN = 1.0 ;
     return ;
    }
  vgs = MCC_VGS ;
  if((MCC_VGS <= (MCC_VTN * 1.1)) || (MCC_CALC_ORG == MCC_CALC_MODE))
    {
     if(MCC_CALC_ORG == MCC_SIM_MODE)
       {
        MCC_KRN = 1.0 ;
        MCC_KSN = 1.0 ;
        return ;
       }
     MCC_VGS = MCC_VTN + (MCC_VDDmax-MCC_VTN) / 2.0 ;
     if(MCC_CALC_CUR == MCC_SIM_MODE)
         MCC_CALC_CUR = MCC_CALC_MODE ;
    }
  step = mcc_ftoi(((MCC_VGS-MCC_VTN)/(MCC_VDDmax-MCC_VTN))*(MCC_VDDmax * MCC_KRNT / MCC_DC_STEP)) ;
  vlin = step * MCC_DC_STEP ;
  step =  mcc_ftoi(((MCC_VGS-MCC_VTN)/(MCC_VDDmax-MCC_VTN))*(MCC_VDDmax * MCC_KRNS ) / MCC_DC_STEP) ;
  vsat = step * MCC_DC_STEP ;
  
  if( lotrsparam->ISVBSSET )
    vbs = lotrsparam->VBS ;
  else
    vbs = lotrsparam->VBULK ;
    
  imax = mcc_spicecurrent(MCC_VGS,MCC_VDDmax,vbs,MCC_TRANS_N,lotrsparam) ;
  is = mcc_spicecurrent(MCC_VGS,vsat,vbs,MCC_TRANS_N,lotrsparam) ;
  rs = mcc_calcul_rs(MCC_VDDmax,imax,vsat,is,MCC_LN+MCC_DLN,MCC_WN+MCC_DWN) ;
  il = mcc_spicecurrent(MCC_VGS,vlin,vbs,MCC_TRANS_N,lotrsparam) ;
  rl = mcc_calcul_r(vlin,il,MCC_LN+MCC_DLN,MCC_WN+MCC_DWN) ;
  usat = (rl * (imax*(MCC_LN+MCC_DLN)/(MCC_WN+MCC_DWN)) - rl * MCC_VDDmax/rs) / (1.0 - rl/rs) ;
         
  i0 = mcc_calcul_isat(MCC_VDDmax,0.0,type) ;
  kres = ((MCC_RNT*i0-(MCC_RNT*MCC_VDDmax)/MCC_RNS)/(1 - MCC_RNT/MCC_RNS))/(MCC_VDDmax-MCC_VTN);
  MCC_KRN = (kres * (MCC_VDDmax - MCC_VTN) - usat) / (kres * ((MCC_VDDmax - MCC_VTN) - (MCC_VGS - MCC_VTN))) ;
  MCC_KSN = ((MCC_VGS - MCC_VTN) * (rs - MCC_RNS)) / (MCC_RNS * ((MCC_VDDmax - MCC_VTN) - (MCC_VGS - MCC_VTN))) ;
  MCC_VGS = vgs ;
  MCC_CALC_CUR = calc ;
  if((MCC_KRN <= 0.0) || (MCC_KSN <= 0.0))
    {
     MCC_KRN = 1.0 ;
     MCC_KSN = 1.0 ;
    }
 }
 else
 {
  if(MCC_VGS == MCC_VDDmax)
    {
     MCC_KRP = 1.0 ;
     MCC_KSP = 1.0 ;
     return ;
    }
  vgs = MCC_VGS ;
  if((MCC_VGS <= (MCC_VTP * 1.1)) || (MCC_CALC_ORG == MCC_CALC_MODE))
    {
     if(MCC_CALC_ORG == MCC_SIM_MODE)
       {
        MCC_KRP = 1.0 ;
        MCC_KSP = 1.0 ;
        return ;
       }
     MCC_VGS = MCC_VTP + (MCC_VDDmax-MCC_VTP) / 2.0 ;
     if(MCC_CALC_CUR == MCC_SIM_MODE)
         MCC_CALC_CUR = MCC_CALC_MODE ;
    }
  step = mcc_ftoi(((MCC_VGS-MCC_VTP)/(MCC_VDDmax-MCC_VTP))*(MCC_VDDmax * MCC_KRPT / MCC_DC_STEP)) ;
  vlin = step * MCC_DC_STEP ;
  step =  mcc_ftoi(((MCC_VGS-MCC_VTP)/(MCC_VDDmax-MCC_VTP))*(MCC_VDDmax * MCC_KRPS) / MCC_DC_STEP) ;
  vsat = step * MCC_DC_STEP ;
  if( lotrsparam->ISVBSSET )
    vbs = lotrsparam->VBS ;
  else
    vbs = lotrsparam->VBULK-MCC_VDDmax ;
  imax = mcc_spicecurrent(MCC_VGS,MCC_VDDmax,vbs,MCC_TRANS_P,lotrsparam) ;
  is = mcc_spicecurrent(MCC_VGS,vsat,vbs,MCC_TRANS_P,lotrsparam) ;
  rs = mcc_calcul_rs(MCC_VDDmax,imax,vsat,is,MCC_LP+MCC_DLP,MCC_WP+MCC_DWP) ;
  il = mcc_spicecurrent(MCC_VGS,vlin,vbs,MCC_TRANS_P,lotrsparam) ;
  rl = mcc_calcul_r(vlin,il,MCC_LP+MCC_DLP,MCC_WP+MCC_DWP) ;
  usat = (rl * (imax*(MCC_LP+MCC_DLP)/(MCC_WP+MCC_DWP)) - rl * MCC_VDDmax/rs) / (1.0 - rl/rs) ;

  i0 = mcc_calcul_isat(MCC_VDDmax,0.0,type) ;
  kres = ((MCC_RPT*i0-(MCC_RPT*MCC_VDDmax)/MCC_RPS)/(1 - MCC_RPT/MCC_RPS))/(MCC_VDDmax-MCC_VTP);

  MCC_KRP = (kres * (MCC_VDDmax - MCC_VTP) - usat) / (kres * ((MCC_VDDmax - MCC_VTP) - (MCC_VGS - MCC_VTP))) ;
  MCC_KSP = ((MCC_VGS - MCC_VTP) * (rs - MCC_RPS)) / (MCC_RPS * ((MCC_VDDmax - MCC_VTP) - (MCC_VGS - MCC_VTP))) ;
  MCC_VGS = vgs ;
  MCC_CALC_CUR = calc ;
  if((MCC_KRP <= 0.0) || (MCC_KSP <= 0.0))
    {
     MCC_KRP = 1.0 ;
     MCC_KSP = 1.0 ;
    }
 }
}

float mcc_error_rst( mbk_pwl *spipwl, mcc_model *mccpwl, int nbmccpwl, int debug )
{
  int                    nmcc ;
  int                    nspi ;
  float                  lastv ;
  float                  lastdeltai ;
  float                  maxspi ;
  float                  currentv ;
  float                  ispi ;
  float                  imcc ;
  float                  deltai ;
  float                  interror ;
  float                  error ;
  int                    move ;
 
  nspi   = 0 ;
  nmcc   = 0 ; 
  
  lastv      = 0.0 ;
  lastdeltai = 0.0 ;
  error      = 0.0 ;
  
  while( nspi < spipwl->N && nmcc < nbmccpwl ) {
 
    if( nspi < spipwl->N -1 )
      maxspi = spipwl->DATA[nspi].X0 ;
    else
      maxspi = spipwl->X1 ;
  
    if( maxspi < mccpwl[nmcc].VMAX ) {
      currentv = maxspi ;
      move = 1 ;
    }
    else {
      currentv = mccpwl[nmcc].VMAX ;
      move = 2 ;
    }

    ispi = fabs( spipwl->DATA[nspi].A * currentv + spipwl->DATA[nspi].B ) ;
    imcc = fabs( mccpwl[nmcc].A * currentv + mccpwl[nmcc].B ) ;
    deltai = ispi - imcc ;
    
    interror = ( currentv - lastv ) * ( deltai + lastdeltai ) / 2.0 ;
    error = error + fabs(interror) ;
  
    if( debug ) {
      printf( "%g %g %g %g %g\n", currentv, ispi, imcc, interror, error );
    }
    
    if( V_BOOL_TAB[ __AVT_BUG_RSAT ].VALUE )
      lastdeltai = ispi ;
    else
      lastdeltai = deltai ;

    lastv      = currentv ;
    
    if( move==1 ) 
      nspi++ ;
    else
      nmcc++ ;
  }

  return error ;
}

void mcc_trs_corner( int type, elp_lotrs_param *lotrsparam_n, elp_lotrs_param *lotrsparam_p, mcc_corner_info *info )
{
  stm_solver_maillon_list *brhead ;
  stm_solver_maillon_list *scan ;
  stm_solver_maillon      *stmm[5] ;
  mcc_trans_spice          trsspi[5] ;
  float                    ibranch ;
  float                    out ;
  int                      i ;
  int                      trs ;
  chain_list              *chain ;
  lotrs_list               lotrs[5] ;
  int                      n ;
  mccglobal               *global ;

  brhead = NULL ;
  n = V_INT_TAB[ __AVT_RST_N ].VALUE ;

  if( type == MCC_TRANS_N )
    trs = 0 ;
  else
    trs = 1 ;
    
  for( i=0 ; i<n ; i++ ) {

    trsspi[i].MODELFILE = MCC_MODELFILE ;
    trsspi[i].MODELTYPE = mcc_getmodeltype (MCC_MODELFILE) ;
    trsspi[i].TRANSNAME = ( trs==0 ? MCC_TNMODEL         : MCC_TPMODEL         );
    trsspi[i].TRANSTYPE = ( trs==0 ? MCC_NMOS            : MCC_PMOS            );
    trsspi[i].TRANSCASE = ( trs==0 ? MCC_NCASE           : MCC_PCASE           );
    trsspi[i].TRLENGTH  = ( trs==0 ? MCC_LN              : MCC_LP              ) * 1e-6 ;
    trsspi[i].TRWIDTH   = ( trs==0 ? MCC_WN              : MCC_WP              ) * 1e-6 ;
    trsspi[i].AD        = ( trs==0 ? MCC_ADN             : MCC_ADP             );
    trsspi[i].AS        = ( trs==0 ? MCC_ASN             : MCC_ASP             );
    trsspi[i].PD        = ( trs==0 ? MCC_PDN             : MCC_PDP             );
    trsspi[i].PS        = ( trs==0 ? MCC_PSN             : MCC_PSP             );
    trsspi[i].VB        = ( trs==0 ? lotrsparam_n->VBULK : lotrsparam_p->VBULK );
    trsspi[i].VG        = ( trs==0 ? MCC_VDDmax          : 0.0                 );
    trsspi[i].PARAM     = ( trs==0 ? lotrsparam_n        : lotrsparam_p        );
    trsspi[i].TEMP      = MCC_TEMP ;
    trsspi[i].VDD       = MCC_VDDmax ;
    trsspi[i].LOTRS     = &(lotrs[i]) ;
    trsspi[i].RS        = 0.0 ;
    trsspi[i].RD        = 0.0 ;
    
    lotrs[i].TRNAME = NULL;
    lotrs[i].LENGTH = (trs==0?MCC_LN:MCC_LP)*SCALE_X ;
    lotrs[i].WIDTH  = (trs==0?MCC_WN:MCC_WP)*SCALE_X ;
    lotrs[i].TYPE = ( trs==0 ? TRANSN : TRANSP ) ;
    lotrs[i].USER = NULL;
    if (trsspi[i].PARAM->SUBCKTNAME!=NULL)
       lotrs[i].USER=addptype(lotrs[i].USER, TRANS_FIGURE, trsspi[i].PARAM->SUBCKTNAME);
    lotrs[i].BULK = NULL;
    lotrs[i].GRID = NULL;
    lotrs[i].SOURCE = NULL;
    lotrs[i].DRAIN  = NULL;
    lotrs[i].NEXT   = NULL;
    lotrs[i].PS   = 0 ;
    lotrs[i].PD   = 0 ;
    lotrs[i].XS   = 0 ;
    lotrs[i].XD   = 0 ;
    addlotrsmodel( &(lotrs[i]), trsspi[i].TRANSNAME ) ;

    chain = addchain( NULL, &trsspi[i] );

    stmm[i] = stm_solver_new_maillon();

    stm_solver_add_model( stmm[i],
                          (char(*)(void*,float,float,float*))mcc_spice_ids_list,
                          (char(*)(void*,float,float,float*))mcc_spice_vds_list,
                          chain,
                          chain
                        );
    freechain( chain );

    brhead = stm_solver_maillon_addchain( brhead, stmm[i] );
  }
  
  brhead = stm_solver_maillon_reverse( brhead );
  
  for( scan = brhead ; scan->NEXT ; scan = scan->NEXT );
  
  if( trs == 0 ) {
    scan->MAILLON->VS = 0.0 ;
    out = MCC_VDDmax ;
  }
  else {
    scan->MAILLON->VS = MCC_VDDmax ;
    out = 0.0 ;
  }

  global = mcc_getglobal() ;
  stm_solver_i( brhead, out, &ibranch );
  mcc_setglobal( global );
  
  info->vdsmin = fabs( stmm[n-1]->VD - stmm[n-1]->VS ) ;
  info->vgsmin = fabs( trsspi[0].VG - stmm[0]->VS );

  for (i=0; i<n; i++) freeptype(trsspi[i].LOTRS->USER);
  stm_solver_maillon_freechain( brhead );
}

void mcc_calcul_rst_better( int type, elp_lotrs_param *lotrsparam_n, elp_lotrs_param *lotrsparam_p, mcc_corner_info *infos )
{
  double                  *kl, *ks, *rl, *rs ;
  mcc_trans_mcc            trselp ;
  double                   vsat, vlin ;
  double                   isat, ilin ;
  double                   imax ;
  mbk_pwl                 *spipwl ;
  double                   l, w ;
  mcc_model                mccpwl[10] ;
  int                      nbmccpwl ;
  int                      nsat ;
  int                      npwl ;
  float                    error ;
  float                    besterror ;
  int                      status ;

  if( !MCC_OPTIM_MODE ) 
    return ;
    
  if( type != MCC_TRANS_N && type != MCC_TRANS_P ) 
    return ;

  if( type == MCC_TRANS_N ) {
  
    kl     = &MCC_KRNT ;
    ks     = &MCC_KRNS ;
    rl     = &MCC_RNT ;
    rs     = &MCC_RNS ;
    spipwl = MCC_OPTIM_IDNRES ;
    l      = MCC_LNeff ;
    w      = MCC_WNeff ;
    
    trselp.TYPE     = TRANSN ;
    trselp.TRLENGTH = (long)(0.5+MCC_LNeff * ((double)SCALE_X)) ;
    trselp.TRWIDTH  = (long)(0.5+MCC_WNeff * ((double)SCALE_X)) ;
    trselp.A        = MCC_AN ;
    trselp.B        = MCC_BN ;
    trselp.VT       = MCC_VTN ;
    trselp.KVT      = MCC_KTN ;
    trselp.VG       = MCC_VDDmax ;
    trselp.VB       = 0.0 ;
  }
  else {
    kl     = &MCC_KRPT ;
    ks     = &MCC_KRPS ;
    rl     = &MCC_RPT ;
    rs     = &MCC_RPS ;
    spipwl = MCC_OPTIM_IDPRES ;
    l      = MCC_LPeff ;
    w      = MCC_WPeff ;

    trselp.TYPE     = TRANSP ;
    trselp.TRLENGTH = (long)(0.5+MCC_LPeff * ((double)SCALE_X)) ;
    trselp.TRWIDTH  = (long)(0.5+MCC_WPeff * ((double)SCALE_X)) ;
    trselp.A        = MCC_AP ;
    trselp.B        = MCC_BP ;
    trselp.VT       = MCC_VTP ;
    trselp.KVT      = MCC_KTP ;
    trselp.VG       = 0 ;
    trselp.VB       = MCC_VDDmax ;
  }

  trselp.VDD      = MCC_VDDmax ;
  trselp.KS       = 1.0 ;
  trselp.KR       = 1.0 ;
  trselp.RS       = 0.0 ;
  trselp.RD       = 0.0 ;

  /*
  rlin and vlin 
  */

  vlin = infos->vdsmin ;

  mbk_pwl_get_value( spipwl, vlin, &ilin ) ;
  if( ilin < 0.0 ) 
    ilin = -ilin ;
  *kl = vlin / MCC_VDDmax ;
  *rl = (w/l)*( vlin / ilin ) ; 

  trselp.VLIN = (*kl)*MCC_VDDmax ;
  trselp.RNT  = (*rl) ;
 
  /*
  rsat and vsat 
  */

  for( npwl = 0 ; npwl < spipwl->N && spipwl->DATA[npwl].X0 <= trselp.VLIN ; npwl++ ) ;
  imax = fabs( spipwl->DATA[spipwl->N-1].A * spipwl->X1 + spipwl->DATA[spipwl->N-1].B ) ;
  besterror = MAXFLOAT ;

  for( nsat = npwl+1 ; nsat < spipwl->N ; nsat++ ) {
        
    vsat = spipwl->DATA[nsat].X0 ;
    isat = fabs(spipwl->DATA[nsat].A * vsat + spipwl->DATA[nsat].B) ;

    trselp.VSAT = vsat ;
    trselp.RNS  = (w/l)*(MCC_VDDmax-vsat)/(imax-isat) ;

    status =  mcc_mcc_characteristic( &trselp, MCC_VDDmax, 0.0, mccpwl, &nbmccpwl, 10 ) ;
            
    if( status ) {

      error = mcc_error_rst( spipwl, mccpwl, nbmccpwl, 0 );

      if( error < besterror ) {
        *rs = trselp.RNS ;
        *ks = trselp.VSAT / MCC_VDDmax;
        besterror = error ;
      }
    }
  }
}

/*
compute MCC_RNS, MCC_RNT, MCC_KRNS, MCC_KRNT
MCC_OPTIM_MODE must be enabled.
if MCC_OPTIM_MODE is not enabled :
  - no operation is performed on MCC_RNS and MCC_RNT : they are supposed to have been computed with mcc_calcul_abr().
  - MCC_KRNS and MCC_KRNT are set to 3/4 and 1/6.
*/

void mcc_calcul_rst( int type, elp_lotrs_param *lotrsparam_n, elp_lotrs_param *lotrsparam_p )
{
  float          vlinmax ;
  float          defaultvlin ;
  float          defaultvsat ;
  float          vlin ;
  float          vsat ;
  float          vbs ;
  float          imax ;
  float          error ;
  float          besterror ;
  float          ilin ;
  float          isat ;
  float          bestrns ;
  float          bestrnt ;
  float          bestvlin ;
  float          bestvsat ;
  float          k ;
  int            npwl ;
  int            nsat ;
  int            nbmccpwl ;
  mbk_pwl       *spipwl ;
  mcc_trans_mcc  trs ;
  mcc_model      mccpwl[10];
  int            status ;
 
  if( type == MCC_TRANS_N || type == MCC_TRANS_B ) {
    MCC_KRNS = 3.0/4.0 ;
    MCC_KRNT = 1.0/6.0 ;
  }
  else {
    MCC_KRPS = 3.0/4.0 ;
    MCC_KRPT = 1.0/6.0 ;
  }

  if( !MCC_OPTIM_MODE ) 
    return ;

  if( type == MCC_TRANS_N || type == MCC_TRANS_B ) {
    vlinmax      = MCC_VDDmax - MCC_VTN ;
    defaultvlin  = MCC_VDDmax * MCC_KRNT ;
    defaultvsat  = MCC_VDDmax * MCC_KRNS ;
    spipwl       = MCC_OPTIM_IDNRES ;
    trs.TYPE     = TRANSN ;
    trs.TRLENGTH = (long)(0.5+MCC_LNeff * ((double)SCALE_X)) ;
    trs.TRWIDTH  = (long)(0.5+MCC_WNeff * ((double)SCALE_X)) ;
    trs.A        = MCC_AN ;
    trs.B        = MCC_BN ;
    trs.VT       = MCC_VTN ;
    trs.KVT      = MCC_KTN ;
    if( lotrsparam_n->ISVBSSET )
      vbs = lotrsparam_n->VBS ;
    else
      vbs = lotrsparam_n->VBULK;
    trs.VG       = MCC_VDDmax ;
    trs.VB       = 0.0 ;
  }
  else {
    vlinmax      = MCC_VDDmax - MCC_VTP ;
    defaultvlin  = MCC_VDDmax * MCC_KRPT ;
    defaultvsat  = MCC_VDDmax * MCC_KRPS ;
    spipwl       = MCC_OPTIM_IDPRES ;
    trs.TYPE     = TRANSP ;
    trs.TRLENGTH = (long)(0.5+MCC_LPeff * ((double)SCALE_X)) ;
    trs.TRWIDTH  = (long)(0.5+MCC_WPeff * ((double)SCALE_X)) ;
    trs.A        = MCC_AP ;
    trs.B        = MCC_BP ;
    trs.VT       = MCC_VTP ;
    trs.KVT      = MCC_KTP ;
    if( lotrsparam_p->ISVBSSET )
      vbs = lotrsparam_p->VBS ;
    else
      vbs = lotrsparam_p->VBULK - MCC_VDDmax;
    if( V_BOOL_TAB[ __AVT_BUG_RSAT ].VALUE ) {
      trs.VG       = MCC_VDDmax ;
      trs.VB       = 0.0 ;
    }
    else {
      trs.VG       = 0 ;
      trs.VB       = MCC_VDDmax ;
    }
  }

  k            = ((float)trs.TRWIDTH) / ((float)trs.TRLENGTH ) ;
  trs.VDD      = MCC_VDDmax ;
  trs.KS       = 1.0 ;
  trs.KR       = 1.0 ;
  trs.RS       = 0.0 ;
  trs.RD       = 0.0 ;

  /* Pour l'instant, on ne calcule ces valeurs que si le vt est supérieur à vdd/2.
     C'est parce lorsqu'on les calcules proprement, ça fait sortir certains bench
     d'infineon de [-4%:4%]. 
  */

  if( ( V_BOOL_TAB[ __MCC_ACCURATE_RLIN_THRESHOLD ].VALUE && trs.VT > trs.VDD/2.0 ) ||
      ! V_BOOL_TAB[ __MCC_ACCURATE_RLIN_THRESHOLD ].VALUE ) {
    if( spipwl && spipwl->N > 1 ) {
   
      imax = fabs( spipwl->DATA[spipwl->N-1].A * spipwl->X1 + spipwl->DATA[spipwl->N-1].B ) ;

      besterror = MAXFLOAT ;

      npwl = 1 ;
      while( npwl < spipwl->N && spipwl->DATA[npwl].X0 <= vlinmax ) {
      
        vlin = spipwl->DATA[npwl].X0 ;
        ilin = fabs( spipwl->DATA[npwl].A*vlin + spipwl->DATA[npwl].B ) ;
        
        trs.VLIN = vlin ;
        trs.RNT  = k*vlin/ilin ;

        for( nsat = npwl+1 ; nsat < spipwl->N ; nsat++ ) {
        
          vsat = spipwl->DATA[nsat].X0 ;
          isat = fabs(spipwl->DATA[nsat].A * vsat + spipwl->DATA[nsat].B) ;

          trs.VSAT = vsat ;
          trs.RNS  = k*(MCC_VDDmax-vsat)/(imax-isat) ;

          if( V_BOOL_TAB[ __AVT_BUG_RSAT ].VALUE )
            status =  mcc_mcc_characteristic( &trs, MCC_VDDmax, vbs, mccpwl, &nbmccpwl, 10 ) ;
          else
            status =  mcc_mcc_characteristic( &trs, MCC_VDDmax, 0.0, mccpwl, &nbmccpwl, 10 ) ;
            
          if( status ) {

            error = mcc_error_rst( spipwl, mccpwl, nbmccpwl, 0 );

            if( error < besterror ) {
              bestrns   = trs.RNS ;
              bestrnt   = trs.RNT ;
              bestvlin  = trs.VLIN ;
              bestvsat  = trs.VSAT ;
              besterror = error ;
            }
          }
        }

        npwl++ ;
      }
      
      if( type == MCC_TRANS_N || type == MCC_TRANS_B ) {
        MCC_RNS = bestrns ;
        MCC_RNT = bestrnt ;
        MCC_KRNT = bestvlin / MCC_VDDmax ;
        MCC_KRNS = bestvsat / MCC_VDDmax ;
      }
      else {
        MCC_RPS = bestrns ;
        MCC_RPT = bestrnt ;
        MCC_KRPT = bestvlin / MCC_VDDmax ;
        MCC_KRPS = bestvsat / MCC_VDDmax ;
      }
    }
  }
}

void mcc_calcul_abr(type,v1,v2,lotrsparam_n,lotrsparam_p)
int type ;
double v1;
double v2;
elp_lotrs_param *lotrsparam_n;
elp_lotrs_param *lotrsparam_p;
{
 double i1 ;
 double i2 ;
 double ir ;
 double irs ;
 double vr ;
 double vrs ;
 double vbs;
 int stepr ;
 int steprs ;
 int valid_param_n = 0,
     valid_param_p = 0;
 static int fordebug ;

 fordebug++ ;

 if((type == MCC_TRANS_N) || (type == MCC_TRANS_B)) {
   stepr = mcc_ftoi((MCC_VDDmax * MCC_KRNT) / ( MCC_DC_STEP)) ;
   steprs = mcc_ftoi((MCC_VDDmax * MCC_KRNS) / ( MCC_DC_STEP)) ;
 }
 else {
   stepr = mcc_ftoi((MCC_VDDmax * MCC_KRPT) / ( MCC_DC_STEP)) ;
   steprs = mcc_ftoi((MCC_VDDmax * MCC_KRPS) / ( MCC_DC_STEP)) ;
 }

 vr = MCC_DC_STEP * stepr ;
 vrs = MCC_DC_STEP * steprs ;

 if((type == MCC_TRANS_N) || (type == MCC_TRANS_B))
   {
    if ( v1 < 0.0 )
      v1 = (MCC_VDDmax-MCC_VTN)/2.0 + MCC_VTN;
    if ( v2 < 0.0 )
      v2 = MCC_VDDmax;
    if( lotrsparam_n->ISVBSSET )
      vbs = lotrsparam_n->VBS ;
    else
      vbs = lotrsparam_n->VBULK;
    i2 = mcc_spicecurrent(v2,MCC_VDDmax,vbs,MCC_TRANS_N,lotrsparam_n) ;
    ir = mcc_spicecurrent(MCC_VDDmax,vr,vbs,MCC_TRANS_N,lotrsparam_n) ;
    irs = mcc_spicecurrent(MCC_VDDmax,vrs,vbs,MCC_TRANS_N,lotrsparam_n) ;
    if( V_BOOL_TAB[ __MCC_ALLOW_NEGATIVE_B ].VALUE ) {
      i1 = mcc_spicecurrent(v1,MCC_VDDmax,vbs,MCC_TRANS_N,lotrsparam_n) ;
      MCC_AN = mcc_calcul_a(v1,v2,i1,i2,MCC_VTN,MCC_LN+MCC_DLN,MCC_WN+MCC_DWN) ;
      MCC_BN = mcc_calcul_b(v1,v2,i1,i2,MCC_VTN,MCC_LN+MCC_DLN,MCC_WN+MCC_DWN) ;
    }
    else {
    // loop to satisfy B > 0.0
      while ( !valid_param_n && v1 < MCC_VDDmax ) 
        {
         i1 = mcc_spicecurrent(v1,MCC_VDDmax,vbs,MCC_TRANS_N,lotrsparam_n) ;
         MCC_AN = fabs(mcc_calcul_a(v1,v2,i1,i2,MCC_VTN,MCC_LN+MCC_DLN,MCC_WN+MCC_DWN)) ;
         if ( MCC_CALC_VT && !V_BOOL_TAB[ __MCC_TEST_B_NEG ].VALUE )
           valid_param_n = 1;
         else if ( (MCC_WNeff/MCC_LNeff)*MCC_AN*(v1-MCC_VTN)*(v1-MCC_VTN) > i1 )
           valid_param_n = 1;
         else 
           v1 += MCC_VTN*0.05;
        }
      MCC_BN = fabs(mcc_calcul_b(v1,v2,i1,i2,MCC_VTN,MCC_LN+MCC_DLN,MCC_WN+MCC_DWN)) ;
    }
    MCC_RNT = mcc_calcul_r(vr,ir,MCC_LN+MCC_DLN,MCC_WN+MCC_DWN) ;
    MCC_RNS = mcc_calcul_rs(v2,i2,vrs,irs,MCC_LN+MCC_DLN,MCC_WN+MCC_DWN) ;
   }
 

 if((type == MCC_TRANS_P) || (type == MCC_TRANS_B))
   {
    if ( v1 < 0.0 )
      v1 = (MCC_VDDmax-MCC_VTP)/2.0 + MCC_VTP;
    if ( v2 < 0.0 )
      v2 = MCC_VDDmax;
    if( lotrsparam_p->ISVBSSET )
      vbs = lotrsparam_p->VBS ;
    else
      vbs = lotrsparam_p->VBULK - MCC_VDDmax;
    i2 = mcc_spicecurrent(v2,MCC_VDDmax,vbs,MCC_TRANS_P,lotrsparam_p) ;
    ir = mcc_spicecurrent(MCC_VDDmax,vr,vbs,MCC_TRANS_P,lotrsparam_p) ;
    irs = mcc_spicecurrent(MCC_VDDmax,vrs,vbs,MCC_TRANS_P,lotrsparam_p) ;
    if( V_BOOL_TAB[ __MCC_ALLOW_NEGATIVE_B ].VALUE ) {
      i1 = mcc_spicecurrent(v1,MCC_VDDmax,vbs,MCC_TRANS_P,lotrsparam_p) ;
      MCC_AP = mcc_calcul_a(v1,v2,i1,i2,MCC_VTP,MCC_LP+MCC_DLP,MCC_WP+MCC_DWP) ;
      MCC_BP = mcc_calcul_b(v1,v2,i1,i2,MCC_VTP,MCC_LP+MCC_DLP,MCC_WP+MCC_DWP) ;
    }
    else {
    // loop to satisfy B > 0.0
      while ( !valid_param_p && v1 < MCC_VDDmax ) 
        {
         i1 = mcc_spicecurrent(v1,MCC_VDDmax,vbs,MCC_TRANS_P,lotrsparam_p) ;
         MCC_AP = fabs(mcc_calcul_a(v1,v2,i1,i2,MCC_VTP,MCC_LP+MCC_DLP,MCC_WP+MCC_DWP)) ;
         if ( MCC_CALC_VT && !V_BOOL_TAB[ __MCC_TEST_B_NEG ].VALUE )
           valid_param_p = 1;
         else if ( (MCC_WPeff/MCC_LPeff)*MCC_AP*(v1-MCC_VTP)*(v1-MCC_VTP) > i1 )
           valid_param_p = 1;
         else 
           v1 += MCC_VTP*0.05;
        }
      MCC_BP = fabs(mcc_calcul_b(v1,v2,i1,i2,MCC_VTP,MCC_LP+MCC_DLP,MCC_WP+MCC_DWP)) ;
    }
    MCC_RPT = mcc_calcul_r(vr,ir,MCC_LP+MCC_DLP,MCC_WP+MCC_DWP) ;
    MCC_RPS = mcc_calcul_rs(v2,i2,vrs,irs,MCC_LP+MCC_DLP,MCC_WP+MCC_DWP) ;
   }
}

void mcc_calcul_vti(type,lotrsparam_n,lotrsparam_p)
int type ;
elp_lotrs_param *lotrsparam_n;
elp_lotrs_param *lotrsparam_p;
{
 if((type == MCC_TRANS_N) || (type == MCC_TRANS_B))
   MCC_VTIN = mcc_calcVTI(MCC_MODELFILE, MCC_TNMODEL,
                          MCC_NMOS, MCC_NCASE, MCC_LN*1.0e-6, MCC_WN*1.0e-6,
                          MCC_VDDmax, MCC_TEMP, MCC_DC_STEP,lotrsparam_n) ;
 if((type == MCC_TRANS_P) || (type == MCC_TRANS_B))
   MCC_VTIP = mcc_calcVTI(MCC_MODELFILE, MCC_TPMODEL,
                          MCC_PMOS, MCC_PCASE, MCC_LP*1.0e-6, MCC_WP*1.0e-6,
                          MCC_VDDmax, MCC_TEMP, MCC_DC_STEP,lotrsparam_p) ;

}

void mcc_calcul_vdeg(type,lotrsparam_n,lotrsparam_p)
int type ;
elp_lotrs_param *lotrsparam_n;
elp_lotrs_param *lotrsparam_p;
{
 if((type == MCC_TRANS_N) || (type == MCC_TRANS_B))
   MCC_VDDdeg = mcc_spicevdeg(MCC_TRANS_N,lotrsparam_n) ;
 if((type == MCC_TRANS_P) || (type == MCC_TRANS_B))
   MCC_VSSdeg = mcc_spicevdeg(MCC_TRANS_P,lotrsparam_p) ;
}

void mcc_calcul_raccess(type,lotrsparam_n,lotrsparam_p)
int type ;
elp_lotrs_param *lotrsparam_n;
elp_lotrs_param *lotrsparam_p;
{
 if((type == MCC_TRANS_N) || (type == MCC_TRANS_B))
   mcc_calcRACCESS( MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE,  MCC_LN*1.0e-6, MCC_WN*1.0e-6, lotrsparam_n, &MCC_RACCNS, &MCC_RACCND );
 if((type == MCC_TRANS_P) || (type == MCC_TRANS_B))
   mcc_calcRACCESS( MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS, MCC_PCASE,  MCC_LP*1.0e-6, MCC_WP*1.0e-6, lotrsparam_p, &MCC_RACCPS, &MCC_RACCPD );
}

void mcc_get_np_vgs(float *vgsn, float *vgsp)
{
  *vgsn=  MCC_VDDmax/2;
  *vgsp=  MCC_VDDmax/2;
/*  *vgsn=(MCC_VDDmax-MCC_VTN)/2+MCC_VTN;
  *vgsp=(MCC_VDDmax-MCC_VTP)/2;
  */
}
void mcc_drvspitab(spifile,tab,nbx,nby,lotrsparam_n,lotrsparam_p)
int spifile ;
double **tab ;
int nbx ;
int nby ;
elp_lotrs_param *lotrsparam_n;
elp_lotrs_param *lotrsparam_p;
{
 FILE *file ;
 char argv[1024][8] ;
 double step, val[8] ;
 double vbsn,vbsp;
 char *nomout ;
 char *pt ;
 int i, k ;
 int j ;
 float vgsn, vgsp, rn, rp;

 i = 0 ;
 if( (spifile == MCC_PARAM) || (spifile == MCC_OPT_PARAM) || (spifile == MCC_CALC_PARAM))
  {
   strcpy(argv[i++],"vdnsat") ;
   strcpy(argv[i++],"vdnres") ;

   if(mcc_ftol(MCC_VGS * 1000.0) != mcc_ftol(MCC_VDDmax * 1000.0))
   strcpy(argv[i++],"vdnvgs") ;
      
   strcpy(argv[i++],"vdpsat") ;
   strcpy(argv[i++],"vdpres") ;

   if(mcc_ftol(MCC_VGS * 1000.0) != mcc_ftol(MCC_VDDmax * 1000.0))
   strcpy(argv[i++],"vdpvgs") ;
      
   strcpy(argv[i++],"vdddeg") ;
   strcpy(argv[i++],"vssdeg") ;
  }
 else
  {
   for(i = 0 ; i < nbx ; i++)
    sprintf(argv[i],"s%d",i) ; 
  }

 if ( spifile == MCC_OPT_PARAM )
   nomout = mbkstrdup (mcc_debug_prefix("param_opt.dat")) ;
 else if ( spifile == MCC_CALC_PARAM )
 {
   nomout = mbkstrdup (mcc_debug_prefix("param_calc.dat")) ;
   for (j=0; j<8; j++)
    {
      TRS_CURVS.I_BSIM[j]=mbkalloc(sizeof(double)*TRS_CURVS.nb);
    }
 }
 else {
   nomout = mbkalloc(strlen(mcc_debug_prefix(MCC_SPICEFILE[spifile])) + 1) ;
   strcpy(nomout,mcc_debug_prefix(MCC_SPICEFILE[spifile])) ;
   if((pt = strchr(nomout,(int)('.'))) != NULL)
     *pt = '\0' ;
 }

 file = mbkfopen(nomout,"dat",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");

 if(file == NULL)
  {
   fprintf(stderr,"\nmcc error: can't open file %s.dat\n",nomout) ;
   EXIT(1);
  }

 if( (spifile == MCC_PARAM) || (spifile == MCC_OPT_PARAM) || (spifile == MCC_CALC_PARAM))
   fprintf(file,"#volt ") ;
 else
   fprintf(file,"#time ") ;

 for(i = 0 ; i < nbx ; i ++)
  {
   fprintf(file,"%s ",*(argv + i)) ;
  }
 fprintf(file,"\n") ;

 step = 0.0 ;

 if( lotrsparam_n->ISVBSSET )
   vbsn = lotrsparam_n->VBS ;
 else
   vbsn = lotrsparam_n->VBULK;
 if( lotrsparam_p->ISVBSSET )
   vbsp = lotrsparam_p->VBS ;
 else
   vbsp = lotrsparam_p->VBULK - MCC_VDDmax;

 mcc_get_np_vgs(&vgsn, &vgsp);
 for(j = 0 ; j < nby ; j ++)
  {
   fprintf(file,"%.3e ",step) ;
   if( (spifile == MCC_PARAM) || (spifile == MCC_OPT_PARAM) || (spifile == MCC_CALC_PARAM))
    {
     MCC_VGS=vgsn;
     rn=mcc_spicecurrent(vgsn,step,vbsn,MCC_TRANS_N,lotrsparam_n);
     MCC_VGS=vgsp;
     rp=mcc_spicecurrent(vgsp,step,vbsp,MCC_TRANS_P,lotrsparam_p);
     fprintf(file,"%.3e %.3e %.3e %.3e %.3e %.3e %.3e %.3e ",
                  val[0]=mcc_spicecurrent(step,MCC_VDDmax,vbsn,MCC_TRANS_N,lotrsparam_n),
                  val[1]=mcc_spicecurrent(MCC_VDDmax,step,vbsn,MCC_TRANS_N,lotrsparam_n),
                  val[2]=rn/*mcc_spicecurrent(vgsn,step,vbsn,MCC_TRANS_N,lotrsparam_n)*/,
                  val[3]=mcc_spicecurrent(step,MCC_VDDmax,vbsp,MCC_TRANS_P,lotrsparam_p),
                  val[4]=mcc_spicecurrent(MCC_VDDmax,step,vbsp,MCC_TRANS_P,lotrsparam_p),
                  val[5]=rp/*mcc_spicecurrent(vgsp,step,vbsp,MCC_TRANS_P,lotrsparam_p)*/,
                  val[6]=mcc_spicevdeg(MCC_TRANS_N,lotrsparam_n),
                  val[7]=mcc_spicevdeg(MCC_TRANS_P,lotrsparam_p)) ;
     if ( spifile == MCC_CALC_PARAM && j<TRS_CURVS.nb)
      {
         for (k=0; k<8; k++)
           TRS_CURVS.I_BSIM[k][j]=val[k];
      }
    }
   else
    {
     for(i = 0 ; i < nbx ; i ++)
      {
       fprintf(file,"%.3e ",tab[i][j]) ;
      }
    }
    fprintf(file,"\n") ;

   if( (spifile == MCC_PARAM) || (spifile == MCC_OPT_PARAM) || (spifile == MCC_CALC_PARAM))
     step += MCC_DC_STEP ;
   else
     step += (V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE) ;
  }

 if(fclose(file) != 0)
  {
   fprintf(stderr,"\nmcc error: can't close file %s.dat\n",nomout) ;
   EXIT(1);
  }

 mbkfree(nomout) ;
}

void mcc_prsspifile(spifile,nbx,nby,tab)
int spifile ;
int nbx ;
int nby ;
double **tab ;
{
 char *fileout ;
 char *argv[1024] ;
 char buf[64] ;
 double mccstep ;
 double limit ;
 int x, y ;
 int i ;

 fileout = sim_getjoker(MCC_SPICEOUT,mcc_debug_prefix(MCC_SPICEFILE[spifile])) ;

 if(spifile == MCC_PARAM)
  {
   i = 0 ;
   argv[i++] = mcc_initstr("vdnsat") ;
   argv[i++] = mcc_initstr("vdnres") ;
   if(nbx > 6)
     argv[i++] = mcc_initstr("vdnvgs") ;
   argv[i++] = mcc_initstr("vdpsat") ;
   argv[i++] = mcc_initstr("vdpres") ;
   if(nbx > 6)
     argv[i++] = mcc_initstr("vdpvgs") ;
   argv[i++] = mcc_initstr("vdddeg") ;
   argv[i++] = mcc_initstr("vssdeg") ;
   mccstep = MCC_DC_STEP ;
   limit = MCC_VDDmax ;
  }
 else
  {
   mccstep = (V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE) ;
   limit = (V_FLOAT_TAB[ __SIM_TIME ].VALUE) ;
   for(x = 0 ; x < nbx ; x++)
    {
     sprintf(buf,"s%d",x) ; 
     argv[x] = mcc_initstr(buf) ;
    }
  }

 sim_readspifiletab (fileout,argv,nbx,nby,tab,limit,mccstep) ;
 mbkfree(fileout) ;
 for(x = 0 ; x < nbx ; x++) 
  {
   for(y = 0 ; y < nby ; y++) 
    {
     tab[x][y] = fabs(tab[x][y]);
    }
  }
 for(x = 0 ; x < nbx ; x++)
  {
   mbkfree(argv[x]) ;
  }
}

void mcc_addspidata(type,lotrsparam_n,lotrsparam_p)
int type ;
elp_lotrs_param *lotrsparam_n;
elp_lotrs_param *lotrsparam_p;
{
 double step,vbs ;
 double vdddeg ;
 double vssdeg ;
 int i ;
 int tabsize ;

 tabsize = mcc_ftoi(MCC_VDDmax / MCC_DC_STEP) ;
 tabsize++ ;
 
 if((type == MCC_TRANS_N) || (type == MCC_TRANS_B)) 
  {   
   vdddeg =  mcc_spicevdeg(MCC_TRANS_N,lotrsparam_n) ;
   step = 0.0 ;
   if( lotrsparam_n->ISVBSSET )
     vbs = lotrsparam_n->VBS ;
   else
     vbs = lotrsparam_n->VBULK;
   for(i = 0 ; i < tabsize ; i ++)
    {
     MCC_IDNSAT[i] = mcc_spicecurrent(step,MCC_VDDmax,vbs,MCC_TRANS_N,lotrsparam_n) ;
     MCC_IDNRES[i] = mcc_spicecurrent(MCC_VDDmax,step,vbs,MCC_TRANS_N,lotrsparam_n) ;
     if(mcc_ftol(MCC_VGS * 1000.0) != mcc_ftol(MCC_VDDmax * 1000.0))
      {
       MCC_IDNVGS[i] = mcc_spicecurrent(MCC_VGS,step,vbs,MCC_TRANS_N,lotrsparam_n) ;
      }
     MCC_VDDDEG[i] = vdddeg ;
     step += MCC_DC_STEP ;
    }
  }
 
 if((type == MCC_TRANS_P) || (type == MCC_TRANS_B)) 
  {   
   vssdeg =  mcc_spicevdeg(MCC_TRANS_P,lotrsparam_p) ;
   step = 0.0 ;
   if( lotrsparam_p->ISVBSSET )
     vbs = lotrsparam_p->VBS ;
   else
     vbs = lotrsparam_p->VBULK - MCC_VDDmax;
   for(i = 0 ; i < tabsize ; i ++)
    {
     MCC_IDPSAT[i] = mcc_spicecurrent(step,MCC_VDDmax,vbs,MCC_TRANS_P,lotrsparam_p) ;
     MCC_IDPRES[i] = mcc_spicecurrent(MCC_VDDmax,step,vbs,MCC_TRANS_P,lotrsparam_p) ;
     if(mcc_ftol(MCC_VGS * 1000.0) != mcc_ftol(MCC_VDDmax * 1000.0))
      {
       MCC_IDPVGS[i] = mcc_spicecurrent(MCC_VGS,step,vbs,MCC_TRANS_P,lotrsparam_p) ;
      }
     MCC_VSSDEG[i] = vssdeg ;
     step += MCC_DC_STEP ;
    }
  }
 MCC_CALC_CUR = MCC_SIM_MODE ;
}

void mcc_readspidata(fit,lotrsparam_n,lotrsparam_p)
int fit ;
elp_lotrs_param *lotrsparam_n;
elp_lotrs_param *lotrsparam_p;
{
 double totimeud ;
 double totimedu ;
 double beforeu ;
 double befored ;
 double seuil = MCC_VDDmax/2.0 ;
 double sfl = MCC_VDDmax*0.1 ;
 double sfh = MCC_VDDmax*0.9 ;
 double delay ;
 double slope ;
 double sloped ;
 double slopeu ;
 double last ;
 double *ptparam[8] ;
 int nbtimeud ;
 int nbtimedu ;
 int tabsize ;
 int i, k ;
 int f ;
 int type ;
 char oldMCC_CALC_ORG;

 sim_set_result_file_extension('p', 1, &MCC_SPICEOUT);
 
 MCC_FLAG_FIT = MCC_FIT_OK ;

 tabsize = mcc_ftoi(MCC_VDDmax / MCC_DC_STEP) ;
 tabsize++ ;

 mcc_allocspidata(MCC_TRANS_B) ;

 i = 0 ;
 ptparam[i++] = MCC_IDNSAT ;
 ptparam[i++] = MCC_IDNRES ;
// if(mcc_ftol(MCC_VGS * 1000.0) != mcc_ftol(MCC_VDDmax * 1000.0))
   ptparam[i++] = MCC_IDNVGS ;
 ptparam[i++] = MCC_IDPSAT ;
 ptparam[i++] = MCC_IDPRES ;
// if(mcc_ftol(MCC_VGS * 1000.0) != mcc_ftol(MCC_VDDmax * 1000.0))
   ptparam[i++] = MCC_IDPVGS ;
 ptparam[i++] = MCC_VDDDEG ;
 ptparam[i++] = MCC_VSSDEG ;

 if(MCC_CALC_CUR == MCC_SIM_MODE)
   {
    oldMCC_CALC_ORG=MCC_CALC_ORG;
    MCC_CALC_ORG = MCC_SIM_MODE ;
    mcc_prsspifile(MCC_PARAM,i,tabsize,ptparam) ;
    TRS_CURVS.nb=tabsize;
    for (k=0; k<i; k++)
    {
      TRS_CURVS.I_SIMU[k]=mbkalloc(sizeof(double)*tabsize);
      memcpy(TRS_CURVS.I_SIMU[k], ptparam[k], sizeof(double)*tabsize);
    }
    mcc_drvspitab(MCC_PARAM,ptparam,i,tabsize,lotrsparam_n,lotrsparam_p) ;
    MCC_CALC_CUR = MCC_CALC_MODE ;
    mcc_drvspitab(MCC_CALC_PARAM,ptparam,i,tabsize,lotrsparam_n,lotrsparam_p) ;
    MCC_CALC_CUR = MCC_SIM_MODE ;
    MCC_CALC_ORG=oldMCC_CALC_ORG;
   }
 else 
  {
   if ( MCC_PLOT && !MCC_OPTIM_MODE ) 
    {
     mcc_addspidata(MCC_TRANS_B,lotrsparam_n,lotrsparam_p) ;
     mcc_drvspitab(MCC_PARAM,ptparam,i,tabsize,lotrsparam_n,lotrsparam_p) ;
    }
   if ( MCC_OPTIM_MODE ) 
    {
     mcc_optim_addspidata (MCC_TRANS_B,lotrsparam_n,lotrsparam_p) ;
     for (k=0; k<i; k++)
     {
       TRS_CURVS.I_OPTIM[k]=mbkalloc(sizeof(double)*tabsize);
       memcpy(TRS_CURVS.I_OPTIM[k], ptparam[k], sizeof(double)*tabsize);
     }
     if ( !MCC_PLOT )
       mcc_drvspitab(MCC_PARAM,ptparam,i,tabsize,lotrsparam_n,lotrsparam_p) ;
     else
       mcc_drvspitab(MCC_OPT_PARAM,ptparam,i,tabsize,lotrsparam_n,lotrsparam_p) ;
    }
  }

 if(fit == 0)
  return ;

 sim_set_result_file_extension('p', 0, &MCC_SPICEOUT);
 
 tabsize = mcc_ftoi(V_FLOAT_TAB[ __SIM_TIME ].VALUE / V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE) ;
 tabsize++ ;

 MCC_FITS = (double **)mbkalloc((MCC_INSNUMB + 1) * sizeof(double *)) ;

 for(i = 0 ; i <= MCC_INSNUMB ; i++)
   MCC_FITS[i] = (double *)mbkalloc(tabsize *sizeof(double)) ;

 for(f = 0 ; f < MCC_SPICENB - 1 ; f++)
  {
   mcc_prsspifile(f,MCC_INSNUMB+1,tabsize,MCC_FITS) ;
   mcc_drvspitab(f,MCC_FITS,MCC_INSNUMB+1,tabsize,lotrsparam_n,lotrsparam_p) ;
   beforeu = -1.0 ;
   befored = -1.0 ;
   totimeud = 0.0 ;
   totimedu = 0.0 ;
   sloped = 0.0 ;
   slopeu = 0.0 ;
   nbtimeud = 0 ;
   nbtimedu = 0 ;
   type = 0 ;
   for(i = 3 ; i <= MCC_INSNUMB - 2 ; i++)
     {
      if(type == 0)
        last = befored ;
      else
        last = beforeu ;

      delay = -1.0 ;
      slope = -1.0 ;

      if(type == 1)
       {
        sfl = (MCC_VDDmax - MCC_VTP)/4.0 ;
        sfh = (MCC_VDDmax - MCC_VTP) ;
       }
      else
       {
        sfl = MCC_VTN ;
        sfh =  (3.0*MCC_VDDmax + MCC_VTN)/4.0 ;
       }

      last = sim_calcdelayslope(MCC_FITS[i],tabsize,2,seuil,sfl,sfh,last,
                                &delay,&slope,&type,V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE*1e9,NULL,NULL) ;

      if(type == 1)
        {
         if((delay > 0) && (slope > 0))
          {
           totimedu += delay ;
           nbtimedu++ ;
           slopeu += slope * 1000.0 ;
          }
         beforeu = last ; 
        }
       else
        {
         if((delay > 0) && (slope > 0))
          {
           totimeud += delay ;
           nbtimeud++ ;
           sloped += slope * 1000.0 ;
          }
         befored = last ; 
        }
     }
   if((nbtimeud != 0) && (nbtimedu != 0))
    {
     totimeud = totimeud * 1000.0 ;
     totimedu = totimedu * 1000.0 ;
     totimeud = (double)(totimeud/(double)nbtimeud) ;
     totimedu = (double)(totimedu/(double)nbtimedu) ;
     slopeu = (double)(slopeu/(double)nbtimedu) ;
     sloped = (double)(sloped/(double)nbtimeud) ;
    }
   else
    {
     beforeu = -1.0 ;
     befored = -1.0 ;
     totimeud = 0.0 ;
     totimedu = 0.0 ;
     sloped = 0.0 ;
     slopeu = 0.0 ;
     nbtimeud = 0 ;
     nbtimedu = 0 ;
     for(i = 1 ; i <= MCC_INSNUMB - 3 ; i++)
       {
        if(type == 0)
          last = befored ;
        else
          last = beforeu ;

        delay = -1.0 ;
        slope = -1.0 ;

        if(type == 1)
         {
          sfl = (MCC_VDDmax - MCC_VTP)/4.0 ;
          sfh = (MCC_VDDmax - MCC_VTP) ;
         }
        else
         {
          sfl = MCC_VTN ;
          sfh =  (3.0*MCC_VDDmax + MCC_VTN)/4.0 ;
         }

        last = sim_calcdelayslope(MCC_FITS[i],tabsize,2,seuil,sfl,sfh,last,
                                  &delay,&slope,&type,V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE*1e9,NULL,NULL) ;
        if(type == 1)
          {
           if((delay > 0) && (slope > 0))
            {
             totimedu += delay ;
             nbtimedu++ ;
             slopeu += slope * 1000.0 ;
            }
           beforeu = last ; 
          }
         else
          {
           if((delay > 0) && (slope > 0))
            {
             totimeud += delay ;
             nbtimeud++ ;
             sloped += slope * 1000.0 ;
            }
           befored = last ; 
          }
       }
      if((nbtimeud != 0) && (nbtimedu != 0))
       {
        totimeud = totimeud * 1000.0 ;
        totimedu = totimedu * 1000.0 ;
        totimeud = (double)(totimeud/(double)nbtimeud) ;
        totimedu = (double)(totimedu/(double)nbtimedu) ;
        slopeu = (double)(slopeu/(double)nbtimedu) ;
        sloped = (double)(sloped/(double)nbtimeud) ;
       }
     }
   switch(f)
     {
      case MCC_FIT_A : MCC_SPICEUD_FITA = (long)totimeud ;
                       MCC_SPICEDU_FITA = (long)totimedu ;
                       MCC_SPICEFUD_FITA = (long)sloped ;
                       MCC_SPICEFDU_FITA = (long)slopeu ;
                       if((MCC_SPICEUD_FITA == 0) ||
                          (MCC_SPICEDU_FITA == 0))
                           MCC_FLAG_FIT = MCC_FIT_KO ;
                       break ;
      case MCC_FIT_CG_NOCAPA:                 
                        MCC_SPICEUD_FITCGNOCAPA = (long)totimeud ;
                        MCC_SPICEDU_FITCGNOCAPA = (long)totimedu ;
                        MCC_SPICEFUD_FITCGNOCAPA = (long)sloped ;
                        MCC_SPICEFDU_FITCGNOCAPA = (long)slopeu ;
                        if((MCC_SPICEUD_FITCGNOCAPA == 0) ||
                           (MCC_SPICEDU_FITCGNOCAPA == 0))
                            MCC_FLAG_FIT = MCC_FIT_KO ;
                        break ;
      case MCC_FIT_CG : MCC_SPICEUD_FITCG = (long)totimeud ;
                        MCC_SPICEDU_FITCG = (long)totimedu ;
                        MCC_SPICEFUD_FITCG = (long)sloped ;
                        MCC_SPICEFDU_FITCG = (long)slopeu ;
                        if((MCC_SPICEUD_FITCG == 0) ||
                           (MCC_SPICEDU_FITCG == 0))
                            MCC_FLAG_FIT = MCC_FIT_KO ;
                        break ;
      case MCC_FIT_CDN : MCC_SPICEUD_FITDN = (long)totimeud ;
                         MCC_SPICEDU_FITDN = (long)totimedu ;
                         MCC_SPICEFUD_FITDN = (long)sloped ;
                         MCC_SPICEFDU_FITDN = (long)slopeu ;
                         if((MCC_SPICEUD_FITDN == 0) ||
                            (MCC_SPICEDU_FITDN == 0))
                             MCC_FLAG_FIT = MCC_FIT_KO ;
                         break ;
      case MCC_FIT_CDP : MCC_SPICEUD_FITDP = (long)totimeud ;
                         MCC_SPICEDU_FITDP = (long)totimedu ;
                         MCC_SPICEFUD_FITDP = (long)sloped ;
                         MCC_SPICEFDU_FITDP = (long)slopeu ;
                         if((MCC_SPICEUD_FITDP == 0) ||
                            (MCC_SPICEDU_FITDP == 0))
                             MCC_FLAG_FIT = MCC_FIT_KO ;
                         break ;
     }
  }
 sim_set_result_file_extension('p', 1, &MCC_SPICEOUT);
 
}

void mcc_allocspidata(type)
int type ;
{
 int tabsize ;

 tabsize = mcc_ftoi(MCC_VDDmax / MCC_DC_STEP) ;
 tabsize++ ;

 if((type == MCC_TRANS_N) || (type == MCC_TRANS_B)) 
  {   
     MCC_IDNSAT = (double *)mbkalloc(tabsize * sizeof(double)) ;
     MCC_IDNRES = (double *)mbkalloc(tabsize * sizeof(double)) ;
     MCC_VDDDEG = (double *)mbkalloc(tabsize * sizeof(double)) ;
     if(mcc_ftol(MCC_VGS * 1000.0) != mcc_ftol(MCC_VDDmax * 1000.0))
        MCC_IDNVGS = (double *)mbkalloc(tabsize * sizeof(double)) ;
     MCC_OPTIM_IDNSAT = NULL;
     MCC_OPTIM_IDNRES = NULL;
     MCC_OPTIM_IDNVGS = NULL;
  }
 if((type == MCC_TRANS_P) || (type == MCC_TRANS_B)) 
  {   
     MCC_IDPSAT = (double *)mbkalloc(tabsize * sizeof(double)) ;
     MCC_IDPRES = (double *)mbkalloc(tabsize * sizeof(double)) ;
     MCC_VSSDEG = (double *)mbkalloc(tabsize * sizeof(double)) ;
     if(mcc_ftol(MCC_VGS * 1000.0) != mcc_ftol(MCC_VDDmax * 1000.0))
       MCC_IDPVGS = (double *)mbkalloc(tabsize * sizeof(double)) ;
     MCC_OPTIM_IDPSAT = NULL;
     MCC_OPTIM_IDPRES = NULL;
     MCC_OPTIM_IDPVGS = NULL;
  }
}

void mcc_freespidata(fit,type)
int fit ;
int type ;
{
 int i ;

 if(MCC_CALC_CUR == MCC_SIM_MODE)
  {
   if((type == MCC_TRANS_N) || (type == MCC_TRANS_B)) 
    {   
     mbkfree(MCC_IDNSAT) ; MCC_IDNSAT=NULL;
     mbkfree(MCC_IDNRES) ;
     mbkfree(MCC_VDDDEG) ;
     if(mcc_ftol(MCC_VGS * 1000.0) != mcc_ftol(MCC_VDDmax * 1000.0))
      mbkfree(MCC_IDNVGS) ;
     if ( MCC_OPTIM_MODE ) 
      {
       mbk_pwl_free_pwl ( MCC_OPTIM_IDNSAT );
       MCC_OPTIM_IDNSAT = NULL;
       mbk_pwl_free_pwl ( MCC_OPTIM_IDNRES );
       MCC_OPTIM_IDNRES = NULL;
       if(mcc_ftol(MCC_VGS * 1000.0) != mcc_ftol(MCC_VDDmax * 1000.0)) 
        {
         mbk_pwl_free_pwl ( MCC_OPTIM_IDNVGS );
         MCC_OPTIM_IDNVGS = NULL;
        }
      }
    }
   if((type == MCC_TRANS_P) || (type == MCC_TRANS_B)) 
    {   
     mbkfree(MCC_IDPSAT) ; MCC_IDPSAT=NULL;
     mbkfree(MCC_IDPRES) ;
     mbkfree(MCC_VSSDEG) ;
     if(mcc_ftol(MCC_VGS * 1000.0) != mcc_ftol(MCC_VDDmax * 1000.0))
      mbkfree(MCC_IDPVGS) ;
     if ( MCC_OPTIM_MODE ) 
      {
       mbk_pwl_free_pwl ( MCC_OPTIM_IDPSAT );
       MCC_OPTIM_IDPSAT = NULL;
       mbk_pwl_free_pwl ( MCC_OPTIM_IDPRES );
       MCC_OPTIM_IDPRES = NULL;
       if(mcc_ftol(MCC_VGS * 1000.0) != mcc_ftol(MCC_VDDmax * 1000.0)) 
        {
         mbk_pwl_free_pwl ( MCC_OPTIM_IDPVGS );
         MCC_OPTIM_IDPVGS = NULL;
        }
      }
    }
  }

 if(fit == 0)
  return ;

 if ( MCC_FITS )
  {
   for(i = 0 ; i <= MCC_INSNUMB ; i++)
     mbkfree(MCC_FITS[i]) ;
  
   mbkfree(MCC_FITS) ;
  }
}

void mcc_runspice (int fit)
{
    int i;

    for (i = fit ? 0 : MCC_PARAM; i < MCC_SPICENB; i++)
        if((MCC_CALC_CUR == MCC_CALC_MODE) && (i == MCC_PARAM)) break;
        else sim_execspice(mcc_debug_prefix(MCC_SPICEFILE[i]), 0, MCC_SPICENAME, MCC_SPICESTRING, MCC_SPICESTDOUT);
}

void mcc_genspicom(file,filename,lotrsparam_n,lotrsparam_p)
FILE *file ;
char *filename ;
elp_lotrs_param *lotrsparam_n;
elp_lotrs_param *lotrsparam_p;
{
 char model[1024] ;
 char basename[1024] ;
 char *pt ;
 int i ;
 int issim ;
 char *subcktmodeln, *subcktmodelp ;
 char *modeln, *modelp ;
 char *mn, *mp ;
 char bufn[1024] ;
 char bufp[1024] ;
 int bs3 = 0 ;
 int bs4 = 0 ;
 float t;

 // unused
 lotrsparam_n = NULL ;
 lotrsparam_p = NULL ;

 bs3 = ( MCC_SPICEMODELTYPE == MCC_BSIM3V3 ? 1 : 0 ) ;
 bs4 = ( MCC_SPICEMODELTYPE == MCC_BSIM4   ? 1 : 0 ) ;

 modeln = MCC_TNMODEL ;
 modelp = MCC_TPMODEL ;
 sprintf(bufn,"m") ;
 sprintf(bufp,"m") ;

 strcpy(model,filename) ;
 pt = strchr(model,(int)('.')) ;
 if(pt != NULL)
   *pt = '\0' ;
 strcpy(basename,model) ;
 pt = strstr(model,"_sim") ;
 if(pt != NULL)
  {
   *pt = '\0' ;
   issim = 1 ;
  }
 else
  {
   issim = 0 ;
  }

 fprintf(file,"****spice description of %s\n",filename);
 fprintf(file,"\n");
 
 if(issim != 0)
  {
   if(MCC_SPICEOPTIONS != NULL) fprintf(file,".option %s\n",MCC_SPICEOPTIONS);
   if ( V_INT_TAB[__SIM_TOOL].VALUE == SIM_TOOL_NGSPICE )
     fprintf(file,".include %s\n",MCC_TECHFILE);
   else
     fprintf(file,".include \"%s\"\n",MCC_TECHFILE);
   fprintf(file,"\n");
  }

 subcktmodeln = mcc_getsubckt( MCC_MODELFILE,
                               MCC_TNMODEL,
                               MCC_NMOS,
                               MCC_NCASE,
                               MCC_LN*1.0e-6,
                               MCC_WN*1.0e-6
                             ) ;
 subcktmodelp = mcc_getsubckt( MCC_MODELFILE,
                               MCC_TPMODEL,
                               MCC_PMOS,
                               MCC_PCASE,
                               MCC_LP*1.0e-6,
                               MCC_WP*1.0e-6
                             ) ;

 if ((subcktmodeln != NULL) && (issim != 0))
     /*- le model est dans un subckt -*/    
  {
   sprintf(bufn,"xm") ;
   modeln = subcktmodeln ;
  }

 if ((subcktmodelp != NULL) && (issim != 0))
     /*- le model est dans un subckt -*/    
  {
   sprintf(bufp,"xm") ;
   modelp = subcktmodelp ;
  }

 mn = mbkstrdup(bufn) ;
 mp = mbkstrdup(bufp) ;

 fprintf(file,".subckt inv in out vbn vbp vdd vss\n");
 fprintf(file,"%s1 out in vss vbn %s l=%gu w=%gu %s\n"
         ,mn,modeln,MCC_LN,MCC_WN,surf_string_n);
 mcc_printf_instance_specific(file, &mcc_user_lotrsparam_n);
 fprintf(file,"%s2 out in vdd vbp %s l=%gu w=%gu %s\n"
         ,mp,modelp,MCC_LP,MCC_WP,surf_string_p);
 mcc_printf_instance_specific(file, &mcc_user_lotrsparam_p);
 if(strstr(model,"fit_a") != NULL)
  {
   if(MCC_CAPA > 0.0)
     fprintf(file,"c1 out vss %gf\n",MCC_CAPA) ;
  }
 else if(strstr(model,"fit_cg") !=NULL || strstr(model,"fit_cg_nocapa") !=NULL)
  {
   for(i = 0 ; i < MCC_INVNUMB ; i++)
    {
     fprintf(file,"%sn%d out%d out ",mn,i,i) ;
     fprintf(file,"vss vbn %s l=%gu w=%gu %s\n"
             ,modeln,MCC_LN,MCC_WN,surf_string_n);
     mcc_printf_instance_specific(file, &mcc_user_lotrsparam_n);
     fprintf(file,"%sp%d out%d out ",mp,i,i) ;
     fprintf(file,"vdd vbp %s l=%gu w=%gu %s\n"
         ,modelp,MCC_LP,MCC_WP,surf_string_p);
     mcc_printf_instance_specific(file, &mcc_user_lotrsparam_p);
     if(strstr(model,"fit_cg_nocapa")==NULL && MCC_CAPA > 0.0)
       fprintf(file,"c%d out%d vss %gf\n",i,i,MCC_CAPA) ;
    }
  }
 else if(strstr(model,"fit_cdn") != NULL)
  {
   for(i = 0 ; i < MCC_TRANSNUMB ; i++)
    {
     fprintf(file,"%sn%d ",mn,i) ;
     fprintf(file,"out vss vss vbn %s l=%gu w=%gu %s\n"
             ,modeln,MCC_LN,MCC_WN,surf_string_n);
     mcc_printf_instance_specific(file, &mcc_user_lotrsparam_n);
    }
  }
 else if(strstr(model,"fit_cdp") !=NULL)
  {
   for(i = 0 ; i < MCC_TRANSNUMB ; i++)
    {
     fprintf(file,"%sp%d ",mp,i) ;
     fprintf(file,"out vdd vdd vbp %s l=%gu w=%gu %s\n"
             ,modelp,MCC_LP,MCC_WP,surf_string_p);
     mcc_printf_instance_specific(file, &mcc_user_lotrsparam_p);
    }
  }
 mbkfree(mn) ;
 mbkfree(mp) ;

 fprintf(file,".ends inv\n");

 fprintf(file,"\n") ;

 fprintf(file,".subckt %s",model) ;

 for(i = 0 ; i < MCC_INSNUMB + 1 ; i++)
  {
   fprintf(file," s%d",i) ;
  }

 fprintf(file," vbn vbp vdd vss\n") ;

 for(i = 0 ; i < MCC_INSNUMB ; i++)
  {
   fprintf(file,"xinv%d s%d s%d vbn vbp vdd vss inv\n",i,i,i+1) ;
  }

 if ( !issim )
  {
   fprintf(file,"vbulkn vbn vss %g\n",(MCC_VBULKN > ELPMINVBULK) ? MCC_VBULKN : 0.0);
   fprintf(file,"vbulkp vbp vss %g\n",(MCC_VBULKP > ELPMINVBULK) ? MCC_VBULKP : MCC_VDDmax);
  }

 fprintf(file,".ends %s\n",model) ;

 if(issim != 0)
  {
   fprintf(file,"\n") ;
   fprintf(file,"x%s",model) ;

   for(i = 0 ; i < MCC_INSNUMB + 1 ; i++)
    {
     fprintf(file," s%d",i) ;
    }

   fprintf(file," vbn vbp vdd 0 %s\n",model);
   fprintf(file,"\n");
   fprintf(file,"valim vdd 0 %gV\n",MCC_VDDmax);
   fprintf(file,"vbulkn vbn 0 %gV\n",(MCC_VBULKN > ELPMINVBULK) ? MCC_VBULKN : 0.0);
   fprintf(file,"vbulkp vbp 0 %gV\n",(MCC_VBULKP > ELPMINVBULK) ? MCC_VBULKP : MCC_VDDmax);
   fprintf(file,"vin s0 0 pwl(\n");
   for(t = 0.0 ; t < (5*(float)MCC_SLOPE) ; t += ((float)MCC_SLOPE/10.0))
     fprintf(file,"+%.5fNS %.5fV\n",t/1000.0,(float)stm_get_v(t,0.2,0,MCC_VDDmax,MCC_SLOPE)) ;
   fprintf(file,"+)\n") ;

   fprintf(file,".option tnom=%g\n", V_FLOAT_TAB[__SIM_TNOM].VALUE);
   fprintf(file,".temp %g\n",MCC_TEMP) ;
   fprintf(file,".tran %gn %gn\n",V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE*1e9,V_FLOAT_TAB[ __SIM_TIME ].VALUE*1e9) ;
   if ( V_BOOL_TAB[__SIM_USE_PRINT].VALUE ) 
    {
     for(i = 0 ; i < MCC_INSNUMB + 1 ; i++)
       fprintf(file,".print tran v(s%d)\n",i) ;
    }
   if ( V_BOOL_TAB[__SIM_USE_MEAS].VALUE ) 
     mcc_add_print_meas (file,filename);
   if ( V_INT_TAB[__SIM_TOOL].VALUE == SIM_TOOL_TITAN || V_INT_TAB[__SIM_TOOL].VALUE == SIM_TOOL_TITAN7) 
     fprintf(file,".save %s\n",basename);
   fprintf(file,".end\n");
  }
}

void mcc_genspi(fit,lotrsparam_n,lotrsparam_p)
int fit ;
elp_lotrs_param *lotrsparam_n;
elp_lotrs_param *lotrsparam_p;
{
 FILE *file ;
 int i ;
 char *subcktmodeln, *subcktmodelp ;
 char *modeln, *modelp, *name ;
 char *mn, *mp ;
 char bufn[1024] ;
 char bufp[1024], buf[1024] ;
 int bs3 = 0,
     bs4 = 0;
 float vgsn, vgsp;

 bs3 = ( MCC_SPICEMODELTYPE == MCC_BSIM3V3 ? 1 : 0 ) ;
 bs4 = ( MCC_SPICEMODELTYPE == MCC_BSIM4   ? 1 : 0 ) ;
 modeln = MCC_TNMODEL ;
 modelp = MCC_TPMODEL ;
 sprintf(bufn,"m") ;
 sprintf(bufp,"m") ;

 subcktmodeln = mcc_getsubckt( MCC_MODELFILE,
                               MCC_TNMODEL,
                               MCC_NMOS,
                               MCC_NCASE,
                               MCC_LN*1.0e-6,
                               MCC_WN*1.0e-6
                             ) ;
 subcktmodelp = mcc_getsubckt( MCC_MODELFILE,
                               MCC_TPMODEL,
                               MCC_PMOS,
                               MCC_PCASE,
                               MCC_LP*1.0e-6,
                               MCC_WP*1.0e-6
                             ) ;

 if (subcktmodeln != NULL) /*- le model est dans un subckt -*/    
  {
   sprintf(bufn,"xm") ;
   modeln = subcktmodeln ;
  }

 if (subcktmodelp != NULL) /*- le model est dans un subckt -*/    
  {
   sprintf(bufp,"xm") ;
   modelp = subcktmodelp ;
  }
 mn = mbkstrdup(bufn) ;
 mp = mbkstrdup(bufp) ;

 if(MCC_CALC_CUR == MCC_SIM_MODE)
  {
   name=mcc_debug_prefix(MCC_SPICEFILE[MCC_PARAM]);
   file = mcc_fopen(name,"w") ;
   avt_printExecInfo(file, "*", "", "");

   mcc_get_np_vgs(&vgsn, &vgsp);
   
   fprintf(file,"* TAS PARAMETRISATION\n");
   fprintf(file,"\n");
   if(MCC_SPICEOPTIONS != NULL)
     fprintf(file,".option %s\n",MCC_SPICEOPTIONS);
   if (V_INT_TAB[__SIM_TOOL].VALUE==SIM_TOOL_HSPICE)
     fprintf(file,".option INGOLD=1\n");
   if (V_INT_TAB[__SIM_TOOL].VALUE==SIM_TOOL_NGSPICE)
     fprintf(file,".include %s\n",MCC_TECHFILE);
   else
     fprintf(file,".include \"%s\"\n",MCC_TECHFILE);
   fprintf(file,"\n");

   fprintf(file,"%sn1 11 2 0 vbn %s l=%gu w=%gu %s\n",
                 mn,modeln,MCC_LN,MCC_WN,surf_string_n);
   mcc_printf_instance_specific(file, &mcc_user_lotrsparam_n);
   fprintf(file,"%sn2 22 vdd 0 vbn %s l=%gu w=%gu %s\n",
                 mn,modeln,MCC_LN,MCC_WN,surf_string_n);
   mcc_printf_instance_specific(file, &mcc_user_lotrsparam_n);
//   if(mcc_ftol(MCC_VGS * 1000.0) != mcc_ftol(MCC_VDDmax * 1000.0)) 
    {
     fprintf(file,"%sn3 222 vgsn 0 vbn %s l=%gu w=%gu %s\n",
                   mn,modeln,MCC_LN,MCC_WN,surf_string_n);
     mcc_printf_instance_specific(file, &mcc_user_lotrsparam_n);
    }
   fprintf(file,"%sp1 10 3 vdd vbp %s l=%gu w=%gu %s\n",
                 mp,modelp,MCC_LP,MCC_WP,surf_string_p);
   mcc_printf_instance_specific(file, &mcc_user_lotrsparam_p);
   fprintf(file,"%sp2 33 0 vdd vbp %s l=%gu w=%gu %s\n",
                 mp,modelp,MCC_LP,MCC_WP,surf_string_p);
   mcc_printf_instance_specific(file, &mcc_user_lotrsparam_p);
//   if(mcc_ftol(MCC_VGS * 1000.0) != mcc_ftol(MCC_VDDmax * 1000.0))
    {
     fprintf(file,"%sp3 333 vgsp vdd vbp %s l=%gu w=%gu %s\n",
                   mp,modelp,MCC_LP,MCC_WP,surf_string_p);
     mcc_printf_instance_specific(file, &mcc_user_lotrsparam_p);
    }
   fprintf(file,"\n");
   fprintf(file,"%spassn vdd vdd vdddeg vbn %s l=%gu w=%gu %s\n",
                 mn,modeln,MCC_LN,MCC_WN,surf_string_n);
   mcc_printf_instance_specific(file, &mcc_user_lotrsparam_n);
   fprintf(file,"%spassp 0 0 vssdeg vbp %s l=%gu w=%gu %s\n",
                 mp,modelp,MCC_LP,MCC_WP,surf_string_p);
   mcc_printf_instance_specific(file, &mcc_user_lotrsparam_p);

   mbkfree(mn) ;
   mbkfree(mp) ;
   fprintf(file,"rvdddeg vdddeg 0 1e9\n");
   fprintf(file,"rvssdeg vssdeg vdd 1e9\n");
   fprintf(file,"\n");
   fprintf(file,"vddmax vdd 0 dc %gv\n",MCC_VDDmax);
   fprintf(file,"vbulkn vbn 0 %gv\n",(MCC_VBULKN > ELPMINVBULK) ? MCC_VBULKN : 0.0);
   fprintf(file,"vbulkp vbp 0 %gv\n",(MCC_VBULKP > ELPMINVBULK) ? MCC_VBULKP : MCC_VDDmax);
//   if(mcc_ftol(MCC_VGS * 1000.0) != mcc_ftol(MCC_VDDmax * 1000.0))
     {
      fprintf(file,"vgsnmax vgsn 0 dc %gv\n",vgsn);
      fprintf(file,"vgspmax vdd vgsp dc %gv\n",vgsp);
     }
   fprintf(file,"var 2 0 dc 0\n");
   fprintf(file,"ep vdd 3 2 0 1\n");
   fprintf(file,"vdnsat vdd 11 dc 0\n");
   fprintf(file,"vdnres 2 22 dc 0\n");
   fprintf(file,"vdpsat 0 10 dc 0\n");
   fprintf(file,"vdpres 3 33 dc 0\n");
//   if(mcc_ftol(MCC_VGS * 1000.0) != mcc_ftol(MCC_VDDmax * 1000.0))
    {
     fprintf(file,"vdnvgs 2 222 dc 0\n");
     fprintf(file,"vdpvgs 3 333 dc 0\n");
    }
   fprintf(file,"\n");
   fprintf(file,".option tnom=%g\n", V_FLOAT_TAB[__SIM_TNOM].VALUE);
   fprintf(file,".temp %g\n",MCC_TEMP);
   fprintf(file,".dc var 0 %g %g\n",MCC_VDDmax,MCC_DC_STEP);
   fprintf(file,".print dc i(vdnsat)\n" );
   fprintf(file,".print dc i(vdnres)\n" );
   fprintf(file,".print dc i(vdnvgs)\n" );
   fprintf(file,".print dc i(vdpsat)\n" );
   fprintf(file,".print dc i(vdpres)\n" );
   fprintf(file,".print dc i(vdpvgs)\n" );
   fprintf(file,".print dc v(vdddeg)\n" );
   fprintf(file,".print dc v(vssdeg)\n" );
   fprintf(file,"\n");
   if ( V_INT_TAB[__SIM_TOOL].VALUE == SIM_TOOL_TITAN || V_INT_TAB[__SIM_TOOL].VALUE == SIM_TOOL_TITAN7) 
   {
     char *c;
     strcpy(buf,name);
     if ((c=strrchr(buf,'.'))!=NULL) *c='\0';
     fprintf(file,".save %s\n", buf);
   }
   fprintf(file,".end\n");

   mcc_fclose(file,name);
  }

 if(fit == 0)
  return ;
/* 
 for(i = 0 ; i < MCC_TASNB ; i++)
  {
   name=mcc_debug_prefix(MCC_TASFILE[i]);
   file = mcc_fopen(name,"w");
   mcc_genspicom(file,name,lotrsparam_n,lotrsparam_p) ;
   mcc_fclose(file,name);
  }
 */
 for(i = 0 ; i < MCC_SPICENB - 1 ; i++)
  {
   name=mcc_debug_prefix(MCC_SPICEFILE[i]);
   file = mcc_fopen(name,"w");
   avt_printExecInfo(file, "*", "", "");
   mcc_genspicom(file,name,lotrsparam_n,lotrsparam_p) ;
   mcc_fclose(file,name);
  }
}

void mcc_calcspicedelay(filename,namelist,start,vthr,vsthrl,vsthrh,tabres)
char *filename ;
chain_list *namelist ;
double start ;
double vthr ;
double vsthrl ;
double vsthrh ;
double **tabres ;
{
 chain_list *chain ;
 char *fileout ;
 int nbx ;
 int nby ;
 int i ;
 int j ;
 int type ;
 double **tab ;
 char *argv[1024] ;
 char *resfilename ;
 char *pt ;
 FILE *resfile;
 double total = 0.0;

 nbx = 0 ;
 for(chain = namelist ; chain != NULL ; chain = chain->NEXT,nbx++)
   {
    argv[nbx] = (char*)chain->DATA ;
   }

 nby = mcc_ftoi(V_FLOAT_TAB[ __SIM_TIME ].VALUE / V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE) ;
 nby++ ;

 tab = (double **)mbkalloc(nbx * sizeof(double *)) ;

 for(i = 0 ; i < nbx ; i++)
   tab[i] = (double *)mbkalloc(nby *sizeof(double)) ;

 sim_execspice(filename,0, MCC_SPICENAME, MCC_SPICESTRING, MCC_SPICESTDOUT) ;
 fileout = sim_getjoker(MCC_SPICEOUT,filename) ;
 sim_readspifiletab (fileout,argv,nbx,nby,tab,V_FLOAT_TAB[ __SIM_TIME ].VALUE,V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE) ;

 resfilename = strdup(filename);
 pt = strchr(resfilename,'.');
 if (pt) *pt = '\0';
 if ((resfile = mbkfopen(resfilename,"dat",WRITE_TEXT)) != NULL ) 
  {
   avt_printExecInfo(resfile, "#", "", "");
   for(i = 0 , j = 0 ; i < nbx ; i++ , j+=2)
    {
     *tabres[j] = 0.0;   //initialisation du delay
     *tabres[j+1] = 0.0; //initialisation du front
     start = sim_calcdelayslope(tab[i],nby,2,vthr,vsthrl,vsthrh,start,
                                tabres[j],tabres[j+1],&type,V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE*1e9,NULL,NULL) ;
     //printf("==> tp( %s ) = %g pS\n",argv[i],start*1000.0);
    }

   fprintf(resfile,"# Data extracted from %s\n\n",fileout);
   
   for(i = 0 , j = 0 ; i < nbx ; i++ , j+=2)
    {
     total += *tabres[j];
     fprintf(resfile,"Signal %s :\n",argv[i]);
     fprintf(resfile,"------\n");
     fprintf(resfile,"* slope       = %8g pS\n",*tabres[j+1]*1000.0);
     fprintf(resfile,"* delay       = %8g pS\n",*tabres[j]*1000.0);
     fprintf(resfile,"* total delay = %8g pS\n",total*1000.0);
     fprintf(resfile,"\n");
    }
  }
 else 
  {
   fprintf(stderr, "\nmcc error: can't open file %s.dat\n",resfilename);
   EXIT(1);
  }

 fclose(resfile);
 mbkfree(resfilename) ;
 mbkfree(fileout) ;
 for(i = 0 ; i < nbx ; i++)
  mbkfree(tab[i]) ;
 mbkfree(tab) ;
}

int mcc_modeltype_known ( int modeltype )
{
  int res = 0;

  switch ( modeltype ) {
    case MCC_BSIM3V3 :
    case MCC_BSIM4 :
    case MCC_MPSP :
    case MCC_MPSPB :
    case MCC_EXTMOD :
                     res = 1;
                     break;
    default        : res = 0;
                     break;
  } 
  return res;
}

void mcc_genparam(modelname,type,l,w,vdd,lotrs,lotrscase,lotrsparam)
char *modelname ;
int type ;
double l ;
double w ;
double vdd ;
int lotrscase;
lotrs_list *lotrs;
elp_lotrs_param *lotrsparam;
{
 double vbs=0.0;
 double lscale = 0.0, wscale = 0.0 ;
 elp_lotrs_param *lotrsparam_n = NULL, *lotrsparam_p = NULL;
 //int lotrs_unused = 0;
 mcc_modellist *model = NULL;
 static int driveall = 1;
 alim_list *power;
 double Weffcj;
 long pd_s,xd_s,ps_s,xs_s;
 double v1,v2;
 float vsource;
 double vbulkn, vbulkp;
 char vbulkfixed=0;
 mcc_corner_info info;
 ptype_list *pt;


 mcc_update_technoparams (modelname,type,l,w,lotrs,lotrscase);

 switch ( lotrscase ) {
   case MCC_BEST  : MCC_VDDmax = MCC_VDD_BEST;
                    MCC_TEMP = MCC_TEMP_BEST;
                    break;
   case MCC_WORST : MCC_VDDmax = MCC_VDD_WORST;
                    MCC_TEMP = MCC_TEMP_WORST;
                    break;
   default        : MCC_VDDmax = vdd;
                    break;
 }
 MCC_VGS = MCC_VDDmax/2.0;
 
 if(type == MCC_TRANS_N)
  {   
   lscale = MCC_LN*1.0e-6 ;
   wscale = MCC_WN*1.0e-6 ;
  }
 else
  {
   lscale = MCC_LP*1.0e-6 ;
   wscale = MCC_WP*1.0e-6 ;
  }

 MCC_CALC_CUR = MCC_CALC_MODE ;

 if((model = mcc_getmodel(MCC_MODELFILE,modelname,type,lotrscase,
                 lscale,wscale,1))) {
      model->USER = addptype(model->USER, MCC_MODEL_LOTRS, lotrs);
      if (mbk_istranscrypt(model->NAME)) avt_set_encrypted_mode(1);
      if ( avt_islog (1,LOGMCC )) {
         avt_log(LOGMCC,1,"Associated Model in technofile : ");
         avt_log(LOGMCC,1,"%s",model->NAME);
         avt_log(LOGMCC,1," %s\n",(type == MCC_TRANS_N) ? "NMOS":"PMOS");
        if ( model->SUBCKTNAME )
           avt_log(LOGMCC,1,"SubcktName : %s\n",model->SUBCKTNAME);

      }
      if ( lotrsparam ) {
        if(type == MCC_TRANS_N) {
          lotrsparam_n = elp_lotrs_param_dup ( lotrsparam );
          if ( lotrsparam_n->VBULK < ELPMINVBULK )
            lotrsparam_n->VBULK = 0.0;
          lotrsparam_p = mcc_init_lotrsparam ();
          lotrsparam_p->VBULK = MCC_VDDmax;
        }
        else {
          lotrsparam_p = elp_lotrs_param_dup ( lotrsparam );
          if ( lotrsparam_p->VBULK < ELPMINVBULK || lotrscase != MCC_TYPICAL )
            lotrsparam_p->VBULK = MCC_VDDmax; 
          lotrsparam_n = mcc_init_lotrsparam ();
          lotrsparam_n->VBULK = 0.0; 
        }
      }
      else {
        lotrsparam_n = mcc_init_lotrsparam ();
        lotrsparam_p = mcc_init_lotrsparam ();
        lotrsparam_n->VBULK = 0.0;
        lotrsparam_p->VBULK = MCC_VDDmax; 
      }

      lotrsparam_p->SUBCKTNAME=NULL;
      lotrsparam_n->SUBCKTNAME=NULL;

      if ( lotrs ) {
        if(type == MCC_TRANS_N) {
          if ( lotrs->SOURCE && getlosigalim (lotrs->SOURCE->SIG, &vsource))
            vbs = lotrsparam_n->VBULK - vsource;
          else if ( (power = cns_get_lotrs_multivoltage(lotrs))!=NULL) {
            vsource = power->VSSMIN;
            vbs = lotrsparam_n->VBULK - vsource;
          }
          else
            vbs = lotrsparam_n->VBULK;
          lotrsparam_n->VBS=vbs;
          lotrsparam_n->ISVBSSET=1;
          if ((pt=getptype(lotrs->USER, TRANS_FIGURE))!=NULL) lotrsparam_n->SUBCKTNAME=(char *)pt->DATA;
        }
        else {
          if ( lotrs->SOURCE && getlosigalim (lotrs->SOURCE->SIG, &vsource))
            vbs = lotrsparam_p->VBULK - vsource;
          else if ( cns_getlotrsalim (lotrs, 'M', &vsource))
            vbs = lotrsparam_p->VBULK - vsource;
          else
            vbs = lotrsparam_p->VBULK - MCC_VDDmax;
          lotrsparam_n->VBS=vbs;
          lotrsparam_n->ISVBSSET=1;
          if ((pt=getptype(lotrs->USER, TRANS_FIGURE))!=NULL) lotrsparam_p->SUBCKTNAME=(char *)pt->DATA;
        }
      }
      /*
      if ( (getptype (lotrs->USER,CNS_UNUSED)) )  {
        vbs = 0.0; // to avoid error while cheking vbs for an unused lotrs
        lotrs_unused = 1;
      }
      */
      avt_log(LOGMCC,1,"Automatic Characterization for temp=%g vdd=%g vbs=%g\n",MCC_TEMP,MCC_VDDmax,vbs);
      if ( mcc_check_vbs (model,l,w,vbs) == 0 ) {

        vbulkfixed = 1 ;
        vbulkn = lotrsparam_n->VBULK ;
        vbulkp = lotrsparam_p->VBULK ;

        lotrsparam_n->VBULK = 0.0 ;
        lotrsparam_p->VBULK = MCC_VDDmax ;
        lotrsparam_n->ISVBSSET = 0.0 ;
        lotrsparam_p->ISVBSSET = 0.0 ;
        avt_errmsg(MCC_ERRMSG, "001", AVT_WARNING);
      }
        
        mcc_calcspiparam(type,lotrsparam_n,lotrsparam_p) ;
        
        //if ( !lotrs_unused ) {
          if ( mcc_check_param (type) ) {
            if ((mcc_modeltype_known ( model->MODELTYPE ))) {
                mcc_allocspidata(type) ;
                if ( !MCC_OPTIM_MODE )
                  mcc_addspidata(type,lotrsparam_n,lotrsparam_p) ;
                else
                  mcc_optim_addspidata(type,lotrsparam_n,lotrsparam_p) ;
                mcc_trs_corner( type, lotrsparam_n, lotrsparam_p, &info );
                if( V_BOOL_TAB[ __MCC_NEW_IDS_SAT ].VALUE ) {
                  switch( type ) {
                  case MCC_TRANS_N :
                    mcc_calcul_sat_parameter( MCC_TRANS_N, lotrsparam_n, &info );
                    break ;
                  case MCC_TRANS_P :
                    mcc_calcul_sat_parameter( MCC_TRANS_P, lotrsparam_p, &info );
                    break ;
                  default :
                    mcc_calcul_sat_parameter( MCC_TRANS_N, lotrsparam_n, &info );
                    mcc_calcul_sat_parameter( MCC_TRANS_P, lotrsparam_p, &info );
                  }
                }
                else {
                  mcc_calcul_vt(type,lotrsparam_n,lotrsparam_p, &info) ;
                  if ( type == MCC_TRANS_N )
                    v1 = (MCC_VDDmax-MCC_VTN)/2.0 + MCC_VTN;
                  else
                    v1 = (MCC_VDDmax-MCC_VTP)/2.0 + MCC_VTP;
                  v2 = MCC_VDDmax;
                  if ( MCC_NEW_CALC_ABR && MCC_OPTIM_MODE )
                    mcc_get_best_abr_from_vt ( type, lotrsparam_n,lotrsparam_p, &info );
                  else
                    mcc_calcul_abr(type,v1,v2,lotrsparam_n,lotrsparam_p) ;
                }

                if( V_BOOL_TAB[__AVT_RST_BETTER].VALUE ) {
                  mcc_calcul_rst_better(type,lotrsparam_n,lotrsparam_p, &info );
                }
                else {
                  mcc_calcul_rst(type,lotrsparam_n,lotrsparam_p);
                }

                mcc_calcul_k(type,(type == MCC_TRANS_N) ? lotrsparam_n:lotrsparam_p) ;
                mcc_calcul_vdeg(type,lotrsparam_n,lotrsparam_p) ;
                mcc_calcul_vti(type,lotrsparam_n,lotrsparam_p) ;
                mcc_calcul_raccess(type,lotrsparam_n,lotrsparam_p);
                mcc_freespidata(0,type) ;
            }
          }
        //}
        
        mcc_cleanmodel( model );

        if ( V_BOOL_TAB[__ELP_DRV_FILE].VALUE )
          driveall = (ELP_MODEL_LIST == NULL) ? MCC_DRV_ALL_MODEL : MCC_DRV_ONE_MODEL;
        else
          driveall = MCC_DONOT_DRV_MODEL;
     
      if( vbulkfixed ) {
        lotrsparam_n->VBULK = vbulkn ;
        lotrsparam_p->VBULK = vbulkp ;
      }
      mcc_drvelp(MCC_PARAM, type,lotrsparam_n,lotrsparam_p,driveall) ;
      
      if ( avt_islog (2,LOGMCC )) {
        // update area and perimeters 
        elpLotrsGetShrinkDim(MCC_CURRENT_LOTRS,NULL,NULL,&xs_s,
                             &xd_s,&ps_s,&pd_s,NULL,NULL, lotrscase);
        if ( type == MCC_TRANS_N ) {
          Weffcj = MCC_WN*MCC_XWN+MCC_DWCJN;
          MCC_ADN = ((double)xd_s*Weffcj)/((double)(100.0*SCALE_X*SCALE_X));
          MCC_ASN = ((double)xs_s*Weffcj)/((double)(100.0*SCALE_X*SCALE_X));
          MCC_PDN = (double)pd_s/(double)SCALE_X;
          MCC_PSN = (double)ps_s/(double)SCALE_X;
          mcc_DisplayInfos (MCC_MODELFILE, MCC_TNMODEL,
                            MCC_NMOS, MCC_NCASE, MCC_LN*1.0e-6, MCC_WN*1.0e-6, 
                            MCC_TEMP, MCC_VDDmax/2.0, 0.0, MCC_VDDmax/2.0,
                            MCC_VDDmax,lotrsparam_n);
        }
        else {
          Weffcj = MCC_WP*MCC_XWP+MCC_DWCJP;
          MCC_ADP = ((double)xd_s*Weffcj)/((double)(100.0*SCALE_X*SCALE_X));
          MCC_ASP = ((double)xs_s*Weffcj)/((double)(100.0*SCALE_X*SCALE_X));
          MCC_PDP = (double)pd_s/(double)SCALE_X;
          MCC_PSP = (double)ps_s/(double)SCALE_X;
          mcc_DisplayInfos (MCC_MODELFILE, MCC_TPMODEL,
                            MCC_PMOS, MCC_PCASE, MCC_LP*1.0e-6, MCC_WP*1.0e-6, 
                            MCC_TEMP, MCC_VDDmax/2.0, 0.0, MCC_VDDmax/2.0,
                            MCC_VDDmax,lotrsparam_p);
        }
      }
      elp_lotrs_param_free ( lotrsparam_n );
      elp_lotrs_param_free ( lotrsparam_p );
 }
 MCC_NCASE = MCC_TYPICAL;
 MCC_PCASE = MCC_TYPICAL;
 MCC_CURRENT_LOTRS = NULL;
 model->USER = delptype(model->USER, MCC_MODEL_LOTRS);
 avt_set_encrypted_mode(0);
}

void mcc_setelpparam ( void )
{
  elpGeneral[elpTEMP] = MCC_TEMP ;
  elpGeneral[elpSLOPE] =  MCC_SLOPE ;
  elpGeneral[elpGVDDMAX] = MCC_VDDmax ;
  elpGeneral[elpGVDDBEST] = MCC_VDD_BEST ;
  elpGeneral[elpGVDDWORST] = MCC_VDD_WORST ;
  elpGeneral[elpGDTHR] = MCC_VTH ;
  elpGeneral[elpGSHTHR] = MCC_VTH_HIGH ;
  elpGeneral[elpGSLTHR] = MCC_VTH_LOW ;
}

/****************************************************************************\
 * Function : mcc_get_best_abr_from_vt
\****************************************************************************/
double mcc_get_new_best_abr_from_vt ( int type, 
                                      elp_lotrs_param *lotrsparam_n, 
                                      elp_lotrs_param *lotrsparam_p,
                                      mcc_corner_info *info
                                    )
{
  double vt ;
  double vstep ;
  double *pt_a ;
  double *pt_b ;
  double *pt_rs ;
  double *pt_rt ;
  double error ;
  double minerror ;
  double v ;
  double best_a ;
  double best_b ;
  double best_rs ;
  double best_rt ;
  mbk_pwl *pwl ;

  if( type == MCC_TRANS_N ) {
    vt    = MCC_VTN ;
    pwl   = MCC_OPTIM_IDNSAT ;
    pt_a  = & MCC_AN ;
    pt_b  = & MCC_BN ;
    pt_rs = & MCC_RNS ;
    pt_rt = & MCC_RNT ;
  }
  else {
    vt    = MCC_VTP ;
    pwl   = MCC_OPTIM_IDPSAT ;
    pt_a  = & MCC_AP ;
    pt_b  = & MCC_BP ;
    pt_rs = & MCC_RPS ;
    pt_rt = & MCC_RPT ;
  }

  vstep = (MCC_VDDmax - vt )/100.0 ;
  minerror = FLT_MAX ;

  for( v = vt+vstep ; v < MCC_VDDmax ; v = v + vstep ) {
  
    mcc_calcul_abr( type, v, MCC_VDDmax, lotrsparam_n, lotrsparam_p );

    if( V_BOOL_TAB[ __MCC_ALLOW_NEGATIVE_B ].VALUE ) {
      if( *pt_b <= -1.0/(MCC_VDDmax-vt) )
        error = FLT_MAX ;
      else
        error = mcc_calcdiffmodelsat( vt, MCC_VDDmax, type, lotrsparam_n, -1.0, info );
    }
    else
      error = mcc_calcdiffmodelsat( vt, MCC_VDDmax, type, lotrsparam_n, -1.0, info );
    if( error <= minerror ) {
      best_a   = *pt_a ;
      best_b   = *pt_b ;
      best_rs  = *pt_rs ;
      best_rt  = *pt_rt ;
      minerror = error;
    }
  }

  *pt_a  = best_a;
  *pt_b  = best_b;
  *pt_rs = best_rs;
  *pt_rt = best_rt;

  return minerror ;
}

double mcc_get_best_abr_from_vt ( int type, 
                                  elp_lotrs_param *lotrsparam_n, 
                                  elp_lotrs_param *lotrsparam_p,
                                  mcc_corner_info *info
                                )
{
  double olderror = FLT_MAX , error = FLT_MAX ;
  double best_v1,nbpoints;
  double v1, v2 = MCC_VDDmax;
  double best_a,best_b,best_rs,best_rt;
  int i;
  int best_i;
  double exitvalue=-1, step=FLT_MAX, cnt=0;

  if( V_BOOL_TAB[ __MCC_NEW_BEST_ABR ].VALUE )
    return mcc_get_new_best_abr_from_vt( type, lotrsparam_n, lotrsparam_p, info );

  if (type == MCC_TRANS_N) {
    nbpoints = MCC_OPTIM_IDNSAT->N;
    for ( i = 0 ; i < nbpoints ; i++ ) {
      v1 = MCC_OPTIM_IDNSAT->DATA[i].X0;
      if( V_BOOL_TAB[ __MCC_ALLOW_NEGATIVE_B ].VALUE ) {
        if( v1 < MCC_VTN )
          continue ;
      }
#ifdef OPTIM2
      if( v1 <= MCC_VTN )
          continue ;
#ifdef OPTIM3
      if (step==FLT_MAX)
      {
        if (nbpoints-i-1<OPTIM3) step=1;
        else step=OPTIM3/(nbpoints-i-1);
      }
      cnt+=step;
      if (cnt<1) continue;
      cnt--;
#endif
#endif
      mcc_calcul_abr(type,v1,v2,lotrsparam_n,lotrsparam_p) ;
      if( V_BOOL_TAB[ __MCC_ALLOW_NEGATIVE_B ].VALUE ) {
        if( MCC_BN <= -1.0/(MCC_VDDmax-MCC_VTN) )
          error = FLT_MAX ;
        else
          error = mcc_calcdiffmodelsat(MCC_VTN,MCC_VDDmax,type,lotrsparam_n,exitvalue, info) ;
      }
      else
        error = mcc_calcdiffmodelsat(MCC_VTN,MCC_VDDmax,type,lotrsparam_n,exitvalue, info) ;
      if ( error <= olderror ) {
        best_a = MCC_AN;
        best_b = MCC_BN;
        best_rs = MCC_RNS;
        best_rt = MCC_RNT;
        best_v1 = v1;
        olderror = error;
        best_i = i ;
#ifdef OPTIM1
        exitvalue=error;
#endif       
      }
    }
    MCC_AN = best_a;
    MCC_BN = best_b;
    MCC_RNS = best_rs;
    MCC_RNT = best_rt;
  }
  else {
    nbpoints = MCC_OPTIM_IDPSAT->N;
    for ( i = 0 ; i < nbpoints ; i++ ) {
      v1 = MCC_OPTIM_IDPSAT->DATA[i].X0;
      if( V_BOOL_TAB[ __MCC_ALLOW_NEGATIVE_B ].VALUE ) {
        if( v1 < MCC_VTP )
          continue ;
      }
#ifdef OPTIM2
      if( v1 <= MCC_VTP )
          continue ;
#ifdef OPTIM3
      if (step==FLT_MAX)
      {
        if (nbpoints-i-1<OPTIM3) step=1;
        else step=OPTIM3/(nbpoints-i-1);
      }
      cnt+=step;
      if (cnt<1) continue;
      cnt--;
#endif
#endif
      mcc_calcul_abr(type,v1,v2,lotrsparam_n,lotrsparam_p) ;
      if( V_BOOL_TAB[ __MCC_ALLOW_NEGATIVE_B ].VALUE ) {
        if( MCC_BP <= -1.0/(MCC_VDDmax-MCC_VTP) )
          error = FLT_MAX ;
        else
          error = mcc_calcdiffmodelsat(MCC_VTP,MCC_VDDmax,type,lotrsparam_p,exitvalue, info) ;
      }
      else
        error = mcc_calcdiffmodelsat(MCC_VTP,MCC_VDDmax,type,lotrsparam_p,exitvalue, info) ;
      if ( error <= olderror ) {
        best_a = MCC_AP;
        best_b = MCC_BP;
        best_rs = MCC_RPS;
        best_rt = MCC_RPT;
        best_v1 = v1;
        olderror = error;
#ifdef OPTIM1
        exitvalue=error;
#endif
      }
    }
    MCC_AP = best_a;
    MCC_BP = best_b;
    MCC_RPS = best_rs;
    MCC_RPT = best_rt;
  }
#ifndef BUGERROR
  return olderror;
#else
  return error;
#endif
}


void mcc_update_technoparams (char *modelname,int type, double l, double w,lotrs_list *lotrs,int lotrscase)
{
  mcc_modellist *model=NULL;
  ptype_list *ptype;

 // Si le fichier techno (spice) est le nom par defaut cad que l utilisateur
 // ne veut pas utiliser de fichier techno, et qu'il n existe pas de modele
 // mcc associe a un eventuel fichier techno inclus par une netlist spice
 // alors on ne genere aucun parametre.
 if ( !mcc_gettechno(MCC_MODELFILE)  &&
      (avt_is_default_technoname(MCC_MODELFILE)) )
   return;
 if((l <= 0.0) || (w <= 0.0))
  {
   avt_errmsg(MCC_ERRMSG, "002", AVT_ERROR, modelname,mcc_ftol(l*1e03),mcc_ftol(w*1e03));
   return;
  }

 mcc_init_globals ();

 MCC_CURRENT_LOTRS = lotrs;

 if ( avt_islog (1,LOGMCC )) 
  {
    avt_log(LOGMCC,1,"-----------------------\n");
    if ( MCC_CURRENT_LOTRS->TRNAME )
      avt_log(LOGMCC,1,"TRNAME : %s ",MCC_CURRENT_LOTRS->TRNAME);
    if ( (ptype = getptype ( MCC_CURRENT_LOTRS->USER, TRANS_FIGURE )) )
      avt_log(LOGMCC,1," TRANS FIGURE : %s  ", (char*)ptype->DATA);
    avt_log(LOGMCC,1,"(L=%gu, ",(double)MCC_CURRENT_LOTRS->LENGTH/(double)SCALE_X);
    avt_log(LOGMCC,1,"W=%gu)\n",(double)MCC_CURRENT_LOTRS->WIDTH /(double)SCALE_X);
  }

 if(elpTechnoFile)
  {
   mbkfree(MCC_ELPFILE);   
   MCC_ELPFILE = mbkstrdup(elpTechnoFile) ;
  }

 if(type == MCC_TRANS_N)
  {   
   if((l > 0.0) && (w > 0.0))
    {
     MCC_NCASE = lotrscase;
     MCC_LN = l ;
     MCC_WN = w ;
     if(MCC_TNMODEL != NULL)
       {
        if(strcmp(MCC_TNMODEL,modelname) != 0)
         {
          mbkfree(MCC_TNMODEL) ;
          MCC_TNMODEL = mcc_initstr(modelname) ;
         }
       }
     else
       {
        MCC_TNMODEL = mcc_initstr(modelname) ;
       }
    }
   else
    {
     MCC_LN = MCC_TECSIZE ;
     MCC_WN = MCC_TECSIZE*6.0 ;
     if ((model = mcc_getmodel(MCC_MODELFILE,NULL,type,MCC_NCASE,MCC_LN*1.0e-6,MCC_WN*1.0e-6,0)))
       MCC_TNMODEL = mbkstrdup(mcc_cutname (model->NAME)); 
    }
  }
 else
  {
   if((l > 0.0) && (w > 0.0))
    {
     MCC_PCASE = lotrscase;
     MCC_LP = l ;
     MCC_WP = w ;
     if(MCC_TPMODEL != NULL)
       {
        if(strcmp(MCC_TPMODEL,modelname) != 0)
         {
          mbkfree(MCC_TPMODEL) ;
          MCC_TPMODEL = mcc_initstr(modelname) ;
         }
       }
     else
       {
        MCC_TPMODEL = mcc_initstr(modelname) ;
       }
    }
   else
    {
     MCC_LP = MCC_TECSIZE ;
     MCC_WP = MCC_TECSIZE*12.0 ;
     if ((model = mcc_getmodel(MCC_MODELFILE,NULL,type,MCC_PCASE,MCC_LP*1.0e-6,MCC_WP*1.0e-6,0)))
       MCC_SPICEMODELTYPE = model->MODELTYPE;
       MCC_TPMODEL = mbkstrdup(mcc_cutname (model->NAME)); 
    }
  }
}

/*****************************************************************************************\
 * FUNC mcc_calcIleakage_from_lotrs
\*****************************************************************************************/
double mcc_calcIleakage_from_lotrs (char *modelname,int type, double l, double w,
                                    lotrs_list *lotrs, int lotrscase,
                                    double vgs,double vds,double vbs, 
                                    double AD,double PD,double AS, double PS, 
                                    double *BLeak,double *DLeak,double *SLeak,
                                    elp_lotrs_param *lotrsparam)
{
  double lscale,wscale;
  double Ileak = 0.0;

  if ( lotrs ) {
    lscale = l*1.0e-6;
    wscale = w*1.0e-6;

    Ileak = mcc_calcILeakage (MCC_MODELFILE, modelname,
                              type, lotrscase, 
                              vbs, vds, vgs,
                              lscale, wscale, MCC_TEMP,
                              AD, PD, AS, PS,
                              BLeak,DLeak,SLeak,
                              lotrsparam);
  }
  if ( avt_islog (2,LOGMCC )) 
   {
     avt_log(LOGMCC,2," ITotal leak = %g (vgs=%g,vds=%g,vbs=%g)\n",
                             Ileak,vgs,vds,vbs);
    if ( BLeak )
       avt_log(LOGMCC,2," B leak = %g (vgs=%g,vds=%g,vbs=%g)\n",
                             *BLeak,vgs,vds,vbs);
    if ( DLeak )
       avt_log(LOGMCC,2," D leak = %g (vgs=%g,vds=%g,vbs=%g)\n",
                             *DLeak,vgs,vds,vbs);
    if ( SLeak )
       avt_log(LOGMCC,2," S leak = %g (vgs=%g,vds=%g,vbs=%g)\n",
                             *SLeak,vgs,vds,vbs);
   }
  return Ileak;
}




void mcc_genspi_for_inverter_simulator(elp_lotrs_param *lotrsparam_n,elp_lotrs_param *lotrsparam_p)
{
  FILE *file ;
  int i, j, k ;
  char *subcktmodeln, *subcktmodelp, *fileout ;
  char *modeln, *modelp, *name ;
  char *mn, *mp, *c, *dir ;
  char bufn[1024] ;
  char bufp[1024], buf[1024], tbuf[1024] ;
  int bs3 = 0, bs4 = 0, nbdata=2;
  char *argv[]={"i", "sig"};
  float vgsn, vgsp, slope, t, t_vdd_sur_2, val;
  double **tab;
  mcc_modellist *ptmodel ;
  int maxl[2];

  bs3 = ( MCC_SPICEMODELTYPE == MCC_BSIM3V3 ? 1 : 0 ) ;
  bs4 = ( MCC_SPICEMODELTYPE == MCC_BSIM4   ? 1 : 0 ) ;
  modeln = MCC_TNMODEL ;
  modelp = MCC_TPMODEL ;
  sprintf(bufn,"m") ;
  sprintf(bufp,"m") ;

  ptmodel = mcc_getmodel ( MCC_MODELFILE,
                           MCC_TPMODEL,
                           MCC_PMOS,
                           MCC_PCASE,
                           MCC_LP*1.0e-6,
                           MCC_WP*1.0e-6, 0);
  subcktmodeln = mcc_getsubckt( MCC_MODELFILE,
                                MCC_TNMODEL,
                                MCC_NMOS,
                                MCC_NCASE,
                                MCC_LN*1.0e-6,
                                MCC_WN*1.0e-6
                                ) ;
  subcktmodelp = mcc_getsubckt( MCC_MODELFILE,
                                MCC_TPMODEL,
                                MCC_PMOS,
                                MCC_PCASE,
                                MCC_LP*1.0e-6,
                                MCC_WP*1.0e-6
                                ) ;

  if (subcktmodeln != NULL) /*- le model est dans un subckt -*/    
    {
      sprintf(bufn,"xm") ;
      modeln = subcktmodeln ;
    }

  if (subcktmodelp != NULL) /*- le model est dans un subckt -*/    
    {
      sprintf(bufp,"xm") ;
      modelp = subcktmodelp ;
    }
  mn = mbkstrdup(bufn) ;
  mp = mbkstrdup(bufp) ;

  sim_set_result_file_extension('p', 0, &MCC_SPICEOUT);

  strcpy(bufn, "");

  for (i=0; i<2; i++)
    {
      name=mbkstrdup(mcc_debug_prefix(i==0?"inverter_rf.spi":"inverter_fr.spi"));
      strcpy(buf,name);
      if ((c=strrchr(buf,'.'))!=NULL) *c='\0';     

      file = mcc_fopen(name,"w") ;
      avt_printExecInfo(file, "*", "", "");
      mcc_get_np_vgs(&vgsn, &vgsp);
      
      fprintf(file,"* TAS PARAMETRISATION\n");
      fprintf(file,"\n");
      if(MCC_SPICEOPTIONS != NULL)
        fprintf(file,".option %s\n",MCC_SPICEOPTIONS);
      if (V_INT_TAB[__SIM_TOOL].VALUE==SIM_TOOL_HSPICE)
        fprintf(file,".option INGOLD=1\n");
      if (V_INT_TAB[__SIM_TOOL].VALUE==SIM_TOOL_NGSPICE)
        fprintf(file,".include %s\n",MCC_TECHFILE);
      else
        fprintf(file,".include \"%s\"\n",MCC_TECHFILE);
      fprintf(file,"\n");

      fprintf(file,".subckt %s i sig o\n", buf);
      fprintf(file,"%sin1 sig i 0 vbn %s l=%gu w=%gu %s\n",
              mn,modeln,MCC_LN,MCC_WN,surf_string_n);
      mcc_printf_instance_specific(file, &mcc_user_lotrsparam_n);
      
      fprintf(file,"%sip1 sig i vdd vbp %s l=%gu w=%gu %s\n",
              mp,modelp,MCC_LP,MCC_WP,surf_string_p);
      mcc_printf_instance_specific(file, &mcc_user_lotrsparam_p);
      
      fprintf(file,"%sin2 o sig 0 vbn %s l=%gu w=%gu %s\n",
              mn,modeln,MCC_LN,MCC_WN,surf_string_n);
      mcc_printf_instance_specific(file, &mcc_user_lotrsparam_n);
      
      fprintf(file,"%sip2 o sig vdd vbp %s l=%gu w=%gu %s\n",
              mp,modelp,MCC_LP,MCC_WP,surf_string_p);
      mcc_printf_instance_specific(file, &mcc_user_lotrsparam_p);
      fprintf(file,"c1 o 0 %gf\n",MCC_CAPA) ;

      fprintf(file,"vddmax vdd 0 dc %gv\n",MCC_VDDmax);
      fprintf(file,"vbulkn vbn 0 %gv\n",(lotrsparam_n->VBULK > ELPMINVBULK) ? lotrsparam_n->VBULK : 0.0);
      fprintf(file,"vbulkp vbp 0 %gv\n",(lotrsparam_p->VBULK > ELPMINVBULK) ? lotrsparam_p->VBULK : MCC_VDDmax);
      fprintf(file,".ends\n");
      
      fprintf(file,"\nxinst i sig o %s\n", buf);
      fprintf(file,"\n");
      fprintf(file,".option tnom=%g\n", V_FLOAT_TAB[__SIM_TNOM].VALUE);
      fprintf(file,".temp %g\n",MCC_TEMP);     

#define RANGELIMIT 0.005

      slope=MCC_SLOPE;
      fprintf(file,"vinput i 0 PWL(\n") ;
      for(t = 0.0 ; t < V_FLOAT_TAB[ __SIM_TIME ].VALUE*1e9*1000; t += ((float)slope/10.0))
        {
          fprintf(file,"+%.5fNS %.5fV\n",t/1000.0,val=(float)mcc_tanh_slope_tas(t,i==0?MCC_VTN:MCC_VTP,i==0?0:MCC_VDDmax,i==0?MCC_VDDmax:0,(float)slope)) ;
          if ((i==0 && val>MCC_VDDmax*(1-RANGELIMIT)) || (i==1 && val<MCC_VDDmax*RANGELIMIT)) break;
        }
      fprintf(file,"+)\n") ;
            
      fprintf(file,".tran %gn %gn\n",V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE*1e9,V_FLOAT_TAB[ __SIM_TIME ].VALUE*1e9) ;
      
      fprintf(file,".print tran v(i)\n");
      fprintf(file,".print tran v(sig)\n");
      fprintf(file,"\n");
      if ( V_INT_TAB[__SIM_TOOL].VALUE == SIM_TOOL_TITAN || V_INT_TAB[__SIM_TOOL].VALUE == SIM_TOOL_TITAN7) 
        fprintf(file,".save %s\n", buf);

      fprintf(file,".end\n");
      
      mcc_fclose(file,name);

      sim_execspice(name,0,MCC_SPICENAME,MCC_SPICESTRING,MCC_SPICESTDOUT);

      fileout = sim_getjoker(MCC_SPICEOUT,buf) ;
      
      tab = (double **)mbkalloc(nbdata * sizeof(double *)) ;
      for(j = 0 ; j < nbdata ; j++)
        tab[j] = (double *)mbkalloc((mcc_ftoi(V_FLOAT_TAB[ __SIM_TIME ].VALUE/V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE)+1) *sizeof(double)) ;
      
      sim_readspifiletab (fileout,argv,nbdata,mcc_ftoi(V_FLOAT_TAB[ __SIM_TIME ].VALUE/V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE)+1,tab,V_FLOAT_TAB[ __SIM_TIME ].VALUE,V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE) ;
      
      if (i==0)
        {
          for (j=0; j<mcc_ftoi(V_FLOAT_TAB[ __SIM_TIME ].VALUE/V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE)+1 && tab[0][j]<MCC_VDDmax/2; j++) ;
          for (k=0; k<mcc_ftoi(V_FLOAT_TAB[ __SIM_TIME ].VALUE/V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE)+1 && tab[1][k]>MCC_VDDmax*RANGELIMIT; k++) ;
        }
      else
        {
          for (j=0; j<mcc_ftoi(V_FLOAT_TAB[ __SIM_TIME ].VALUE/V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE)+1 && tab[0][j]>MCC_VDDmax/2; j++) ;
          for (k=0; k<mcc_ftoi(V_FLOAT_TAB[ __SIM_TIME ].VALUE/V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE)+1 && tab[1][k]<MCC_VDDmax*(1-RANGELIMIT); k++) ;
        }
      
      t_vdd_sur_2=(j-1)*V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE*1e9;
      t_vdd_sur_2+=((MCC_VDDmax/2-tab[0][j-1])/(tab[0][j]-tab[0][j-1]))*V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE*1e9;

      if (i==0) sprintf(bufp," t0r=%g", t_vdd_sur_2*1e-9);
      else sprintf(bufp," t0f=%g", t_vdd_sur_2*1e-9);
      strcat(bufn, bufp);
//      printf("vdd/2: %g\n", t_vdd_sur_2);

      sprintf(bufp,"%s.spice.dat", buf);
      file = mcc_fopen(bufp,"w") ;
      avt_printExecInfo(file, "#", "", "");
      fprintf(file,"#time\t\tv(i)\t\tv(sig)\n");
      TRS_CURVS.nbdyna = mcc_ftoi(V_FLOAT_TAB[ __SIM_TIME ].VALUE/V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE)+1 ;
      for (j=0; j<2; j++)
        TRS_CURVS.SIMUINV[i][j]=(double *)mbkalloc(sizeof(double)*TRS_CURVS.nbdyna);

      for (j=0; j<mcc_ftoi(V_FLOAT_TAB[ __SIM_TIME ].VALUE/V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE)+1; j++)
      {
        fprintf(file,"%10g\t%10g\t%10g\n", j*V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE, tab[0][j], tab[1][j]);
        TRS_CURVS.SIMUINV[i][0][j]=tab[0][j];
        TRS_CURVS.SIMUINV[i][1][j]=tab[1][j];
      }
      mcc_fclose(file,bufp);
      for(j = 0 ; j < nbdata ; j++)
        mbkfree(tab[j]) ;      
      mbkfree(tab) ;


      if (i==0) dir="ud"; else dir="du";
      sprintf(bufp,"%s.plt", buf);
      file = mcc_fopen(bufp,"w") ;
      avt_printExecInfo(file, "#", "", "");
      fprintf(file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
      if( i==0 ) 
        fprintf(file,"set key bottom left\n");
      else
        fprintf(file,"set key top left\n");
      fprintf(file,"set xlabel \"time\"\n") ;
      fprintf(file,"set ylabel \"V\"\n") ;
      fprintf(file,"set grid\n") ;
      fprintf(file,"set xrange [:%g]\n",k*V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE) ;
      maxl[i]=k;

      fprintf(file,"plot \"%s%s.dat\" using 1:2 title \"HiTas simulator with elp current : input\" with lines,\\\n", mcc_debug_prefix("inverter.tas."), dir);
      fprintf(file,"     \"%s%s.dat\" using 1:3 title \"HiTas simulator with elp current : output\" with lines,\\\n", mcc_debug_prefix("inverter.tas."), dir);
      fprintf(file,"     \"%s%s.dat\" using 1:2 title \"HiTas simulator with %s current : input\" with lines,\\\n", mcc_debug_prefix("inverter.mcc."), dir, mcc_getmccname( ptmodel ));
      fprintf(file,"     \"%s%s.dat\" using 1:3 title \"HiTas simulator with %s current : output\" with lines,\\\n", mcc_debug_prefix("inverter.mcc."), dir, mcc_getmccname( ptmodel ));
      fprintf(file,"     \"%s.%s.stm_ve.dat\" using 1:2 title \"HiTas analytical model\" with lines,\\\n", mcc_debug_prefix("inverter"), dir);
      fprintf(file,"     \"%s.%s.stm_vs.dat\" using 1:2 title \"HiTas analytical model\" with lines,\\\n", mcc_debug_prefix("inverter"), dir);
      fprintf(file,"     \"%s.spice.dat\" using 1:2 title \"External electrical simulator\" with lines,\\\n", buf);
      fprintf(file,"     \"%s.spice.dat\" using 1:3 title \"External electrical simulator\" with lines\n", buf);
      fprintf(file,"pause -1 'Hit CR to finish'\n") ;

      mcc_fclose(file,bufp);

      if (i!=1) mbkfree(name);
    }
  mbkfree(mn) ;
  mbkfree(mp) ;
  
  mcc_bilan_capa_simulated_inverter(lotrsparam_n,lotrsparam_p, maxl);
          
  sprintf(bufp,"S sig %s", mcc_debug_prefix("inverter"));
  avt_sethashvar("tasSimulateInverter", bufp);
  sprintf(bufp," tmax=%g", -1.0/*V_FLOAT_TAB[ __SIM_TIME ].VALUE*/);
  strcat(bufn, bufp);
  avt_sethashvar("tasSimulateInverterConfig", bufn);
  avt_sethashvar("tasDebugPwl", "sig");
  sprintf( tbuf, "top_%s", buf );
  c=strchr( tbuf,'.');
  if( c )
    *c='\0';
  if( getloadedlofig( tbuf ) )
    dellofig( tbuf );
  mcc_runtas_tcl(name,MCC_SLOPE,0,NULL,buf);
  sprintf(bufp,"T sig %s", mcc_debug_prefix("inverter"));
  avt_sethashvar("tasDebugPwl", NULL);
  avt_sethashvar("tasSimulateInverter", bufp);
  if( getloadedlofig( tbuf ) )
    dellofig( tbuf );
  mcc_runtas_tcl(name,MCC_SLOPE,0,NULL,buf);
  avt_sethashvar("tasSimulateInverter", NULL);
  avt_sethashvar("tasSimulateInverterConfig", NULL);


//  mcc_runtas_tcl(gate,slope,capa,buf1,NULL) ;
  mbkfree(name);
}

void mcc_calcul_ibranch(elp_lotrs_param *lotrsparam_n,elp_lotrs_param *lotrsparam_p)
{
  int i ;
  
  mcc_calcul_ibranch_spice( lotrsparam_n, lotrsparam_p, TRS_CURVS.tabspin, TRS_CURVS.tabspip );
  for( i=1 ; i<=4 ; i++ ) {
    mcc_calcul_ibranch_mcc( lotrsparam_n, lotrsparam_p, &(TRS_CURVS.tabmccn[i-1]), &(TRS_CURVS.tabmccp[i-1]), i, 'm' );
    mcc_calcul_ibranch_mcc( lotrsparam_n, lotrsparam_p, &(TRS_CURVS.tabelpn[i-1]), &(TRS_CURVS.tabelpp[i-1]), i, 'e' );
  }

}

void mcc_calcul_ibranch_mcc( elp_lotrs_param *lotrsparam_n, elp_lotrs_param *lotrsparam_p, double *tabin, double *tabip, int n, char mode )
{
  stm_solver_maillon_list *brhead ;
  stm_solver_maillon_list *scan ;
  stm_solver_maillon     **stmm ;
  mcc_trans_mcc           *trselp ;
  mcc_trans_spice         *trsmcc ;
  float                    imax ;
  float                    out ;
  int                      i ;
  int                      trs ;
  chain_list              *chain, *tofree=NULL ;
  lotrs_list              *lotrs ;
  
  stmm = alloca( sizeof( stm_solver_maillon* )*n ) ;
  if( mode == 'e' )
    trselp = alloca( sizeof( mcc_trans_mcc ) * n );
  else {
    trsmcc = alloca( sizeof( mcc_trans_spice ) * n );
    lotrs  = alloca( sizeof( lotrs_list ) * n );
  }

  for( trs=0 ; trs<2 ; trs++ ) {
    brhead = NULL ;
    for( i=1 ; i<=n ; i++ ) {
    
      stmm[i-1] = stm_solver_new_maillon();
      
      if( mode == 'e' ) {
      
        if( trs==0 )
          mcc_trans_mcc_from_global( MCC_TRANS_N, &(trselp[i-1]) );
        else
          mcc_trans_mcc_from_global( MCC_TRANS_P, &(trselp[i-1]) );
          
        chain = addchain( NULL, &trselp[i-1] );
        stm_solver_add_model( stmm[i-1], 
                              (char(*)(void*,float,float,float*))mcc_mcc_ids_list,
                              (char(*)(void*,float,float,float*))mcc_mcc_vds_list,
                              chain,
                              chain
                            );
      }
      else {
        trsmcc[i-1].MODELFILE = MCC_MODELFILE ;
        trsmcc[i-1].MODELTYPE = mcc_getmodeltype (MCC_MODELFILE) ;
        trsmcc[i-1].TRANSNAME = ( trs==0 ? MCC_TNMODEL         : MCC_TPMODEL         );
        trsmcc[i-1].TRANSTYPE = ( trs==0 ? MCC_NMOS            : MCC_PMOS            );
        trsmcc[i-1].TRANSCASE = ( trs==0 ? MCC_NCASE           : MCC_PCASE           );
        trsmcc[i-1].TRLENGTH  = ( trs==0 ? MCC_LN              : MCC_LP              ) * 1e-6 ;
        trsmcc[i-1].TRWIDTH   = ( trs==0 ? MCC_WN              : MCC_WP              ) * 1e-6 ;
        trsmcc[i-1].AD        = ( trs==0 ? MCC_ADN             : MCC_ADP             );
        trsmcc[i-1].AS        = ( trs==0 ? MCC_ASN             : MCC_ASP             );
        trsmcc[i-1].PD        = ( trs==0 ? MCC_PDN             : MCC_PDP             );
        trsmcc[i-1].PS        = ( trs==0 ? MCC_PSN             : MCC_PSP             );
        trsmcc[i-1].VB        = ( trs==0 ? lotrsparam_n->VBULK : lotrsparam_p->VBULK );
        trsmcc[i-1].VG        = ( trs==0 ? MCC_VDDmax          : 0.0                 );
        trsmcc[i-1].PARAM     = ( trs==0 ? lotrsparam_n        : lotrsparam_p        );
        trsmcc[i-1].TEMP      = MCC_TEMP ;
        trsmcc[i-1].VDD       = MCC_VDDmax ;
        trsmcc[i-1].LOTRS     = &(lotrs[i-1]) ;
        trsmcc[i-1].RS        = 0.0 ;
        trsmcc[i-1].RD        = 0.0 ;
        
        lotrs[i-1].TRNAME = NULL;
        lotrs[i-1].LENGTH = (trs==0?MCC_LN:MCC_LP)*SCALE_X ;
        lotrs[i-1].WIDTH  = (trs==0?MCC_WN:MCC_WP)*SCALE_X ;
        lotrs[i-1].TYPE = ( trs==0 ? TRANSN : TRANSP ) ;
        lotrs[i-1].USER = NULL;
        if (trsmcc[i-1].PARAM->SUBCKTNAME!=NULL)
        {
           lotrs[i-1].USER=addptype(lotrs[i-1].USER, TRANS_FIGURE, trsmcc[i-1].PARAM->SUBCKTNAME);
           tofree=addchain(tofree, lotrs[i-1].USER);
        }
        lotrs[i-1].BULK = NULL;
        lotrs[i-1].GRID = NULL;
        lotrs[i-1].SOURCE = NULL;
        lotrs[i-1].DRAIN= NULL;
        addlotrsmodel( &(lotrs[i-1]), trsmcc[i-1].TRANSNAME ) ;

        chain = addchain( NULL, &trsmcc[i-1] );
        stm_solver_add_model( stmm[i-1],
                              (char(*)(void*,float,float,float*))mcc_spice_ids_list,
                              (char(*)(void*,float,float,float*))mcc_spice_vds_list,
                              chain,
                              chain
                            );
        freechain( chain );
      }

      brhead = stm_solver_maillon_addchain( brhead, stmm[i-1] );
    }
    
    brhead = stm_solver_maillon_reverse( brhead );
    
    for( scan = brhead ; scan->NEXT ; scan = scan->NEXT );
    
    if( trs == 0 ) {
      scan->MAILLON->VS = 0.0 ;
      out = MCC_VDDmax ;
    }
    else {
      scan->MAILLON->VS = MCC_VDDmax ;
      out = 0.0 ;
    }

    stm_solver_i( brhead, out, &imax );
    while (tofree!=NULL)
    {
      freeptype((ptype_list *)tofree->DATA);
      tofree=delchain(tofree, tofree);
    }
    stm_solver_maillon_freechain( brhead );

    if( trs == 0 ) 
      *tabin = -imax*1e6 ;
    else
      *tabip = imax*1e6 ;
  }

  MCC_CURRENT_LOTRS = NULL ;
}

void mcc_calcul_ibranch_spice( elp_lotrs_param *lotrsparam_n, elp_lotrs_param *lotrsparam_p, double *tabin, double *tabip )
{
  FILE *file ;
  int j ;
  char *subcktmodeln, *subcktmodelp, *fileout ;
  char *modeln, *modelp, *name ;
  char *mn, *mp, *c ;
  char bufn[1024] ;
  char bufp[1024], buf[1024] ;
  int bs3 = 0, bs4 = 0, nbdata=8;
  char *argv[]={"VN1","VN2","VN3","VN4", "VP1","VP2","VP3", "VP4"};
  float vgsn, vgsp ;
  double **tab;
  mcc_modellist *ptmodel ;

  bs3 = ( MCC_SPICEMODELTYPE == MCC_BSIM3V3 ? 1 : 0 ) ;
  bs4 = ( MCC_SPICEMODELTYPE == MCC_BSIM4   ? 1 : 0 ) ;
  modeln = MCC_TNMODEL ;
  modelp = MCC_TPMODEL ;
  sprintf(bufn,"m") ;
  sprintf(bufp,"m") ;

  ptmodel = mcc_getmodel ( MCC_MODELFILE,
                           MCC_TPMODEL,
                           MCC_PMOS,
                           MCC_PCASE,
                           MCC_LP*1.0e-6,
                           MCC_WP*1.0e-6, 0);
  subcktmodeln = mcc_getsubckt( MCC_MODELFILE,
                                MCC_TNMODEL,
                                MCC_NMOS,
                                MCC_NCASE,
                                MCC_LN*1.0e-6,
                                MCC_WN*1.0e-6
                                ) ;
  subcktmodelp = mcc_getsubckt( MCC_MODELFILE,
                                MCC_TPMODEL,
                                MCC_PMOS,
                                MCC_PCASE,
                                MCC_LP*1.0e-6,
                                MCC_WP*1.0e-6
                                ) ;

  if (subcktmodeln != NULL) /*- le model est dans un subckt -*/    
    {
      sprintf(bufn,"xm") ;
      modeln = subcktmodeln ;
    }

  if (subcktmodelp != NULL) /*- le model est dans un subckt -*/    
    {
      sprintf(bufp,"xm") ;
      modelp = subcktmodelp ;
    }
  mn = mbkstrdup(bufn) ;
  mp = mbkstrdup(bufp) ;

  sim_set_result_file_extension('p', 1, &MCC_SPICEOUT);

  strcpy(bufn, "");

  name=mbkstrdup(mcc_debug_prefix("ibranch.spi"));
  strcpy(buf,name);
  if ((c=strrchr(buf,'.'))!=NULL) *c='\0';     

  file = mcc_fopen(name,"w") ;
  avt_printExecInfo(file, "*", "", "");
  mcc_get_np_vgs(&vgsn, &vgsp);
  
  fprintf(file,"* branch current measure\n" );
  fprintf(file,"\n");
  if(MCC_SPICEOPTIONS != NULL)
    fprintf(file,".option %s\n",MCC_SPICEOPTIONS);
  if (V_INT_TAB[__SIM_TOOL].VALUE==SIM_TOOL_HSPICE)
    fprintf(file,".option INGOLD=1\n");
  if(V_INT_TAB[__SIM_TOOL].VALUE==SIM_TOOL_NGSPICE)
    fprintf(file,".include %s\n",MCC_TECHFILE);
  else
    fprintf(file,".include \"%s\"\n",MCC_TECHFILE);
  fprintf(file,"\n");

  fprintf(file,".subckt %s in out1 out2 out3 out4\n", buf);
  
  fprintf(file,"%sn11 out1 in 0 vbn %s l=%gu w=%gu %s\n", mn,modeln,MCC_LN,MCC_WN,surf_string_n);
  mcc_printf_instance_specific(file, &mcc_user_lotrsparam_n);
  fprintf(file,"%sp11 out1 in vdd vbp %s l=%gu w=%gu %s\n", mp,modelp,MCC_LP,MCC_WP,surf_string_p);
  mcc_printf_instance_specific(file, &mcc_user_lotrsparam_p);
      
  fprintf(file,"%sn21 out2 in n21 vbn %s l=%gu w=%gu %s\n", mn,modeln,MCC_LN,MCC_WN,surf_string_n);
  mcc_printf_instance_specific(file, &mcc_user_lotrsparam_n);
  fprintf(file,"%sn22 n21 in 0 vbn %s l=%gu w=%gu %s\n", mn,modeln,MCC_LN,MCC_WN,surf_string_n);
  mcc_printf_instance_specific(file, &mcc_user_lotrsparam_n);
  fprintf(file,"%sp21 out2 in p21 vbp %s l=%gu w=%gu %s\n", mp,modelp,MCC_LP,MCC_WP,surf_string_p);
  mcc_printf_instance_specific(file, &mcc_user_lotrsparam_p);
  fprintf(file,"%sp22 p21 in vdd vbp %s l=%gu w=%gu %s\n", mp,modelp,MCC_LP,MCC_WP,surf_string_p);
  mcc_printf_instance_specific(file, &mcc_user_lotrsparam_p);
  fprintf(file,"rn21 n21 0   10Meg\n");
  fprintf(file,"rp21 p21 vdd 10Meg\n");

  fprintf(file,"%sn31 out3 in n31 vbn %s l=%gu w=%gu %s\n", mn,modeln,MCC_LN,MCC_WN,surf_string_n);
  mcc_printf_instance_specific(file, &mcc_user_lotrsparam_n);
  fprintf(file,"%sn32 n31 in n32 vbn %s l=%gu w=%gu %s\n", mn,modeln,MCC_LN,MCC_WN,surf_string_n);
  mcc_printf_instance_specific(file, &mcc_user_lotrsparam_n);
  fprintf(file,"%sn33 n32 in 0 vbn %s l=%gu w=%gu %s\n", mn,modeln,MCC_LN,MCC_WN,surf_string_n);
  mcc_printf_instance_specific(file, &mcc_user_lotrsparam_n);
  fprintf(file,"%sp31 out3 in p31 vbp %s l=%gu w=%gu %s\n", mp,modelp,MCC_LP,MCC_WP,surf_string_p);
  mcc_printf_instance_specific(file, &mcc_user_lotrsparam_p);
  fprintf(file,"%sp32 p31 in p32 vbp %s l=%gu w=%gu %s\n", mp,modelp,MCC_LP,MCC_WP,surf_string_p);
  mcc_printf_instance_specific(file, &mcc_user_lotrsparam_p);
  fprintf(file,"%sp33 p32 in vdd vbp %s l=%gu w=%gu %s\n", mp,modelp,MCC_LP,MCC_WP,surf_string_p);
  mcc_printf_instance_specific(file, &mcc_user_lotrsparam_p);
  fprintf(file,"rn31 n31 0   10Meg\n");
  fprintf(file,"rn32 n32 0   10Meg\n");
  fprintf(file,"rp31 p31 vdd 10Meg\n");
  fprintf(file,"rp32 p32 vdd 10Meg\n");

  fprintf(file,"%sn41 out4 in n41 vbn %s l=%gu w=%gu %s\n", mn,modeln,MCC_LN,MCC_WN,surf_string_n);
  mcc_printf_instance_specific(file, &mcc_user_lotrsparam_n);
  fprintf(file,"%sn42 n41 in n42 vbn %s l=%gu w=%gu %s\n", mn,modeln,MCC_LN,MCC_WN,surf_string_n);
  mcc_printf_instance_specific(file, &mcc_user_lotrsparam_n);
  fprintf(file,"%sn43 n42 in n43 vbn %s l=%gu w=%gu %s\n", mn,modeln,MCC_LN,MCC_WN,surf_string_n);
  mcc_printf_instance_specific(file, &mcc_user_lotrsparam_n);
  fprintf(file,"%sn44 n43 in 0 vbn %s l=%gu w=%gu %s\n", mn,modeln,MCC_LN,MCC_WN,surf_string_n);
  mcc_printf_instance_specific(file, &mcc_user_lotrsparam_n);
  fprintf(file,"%sp41 out4 in p41 vbp %s l=%gu w=%gu %s\n", mp,modelp,MCC_LP,MCC_WP,surf_string_p);
  mcc_printf_instance_specific(file, &mcc_user_lotrsparam_p);
  fprintf(file,"%sp42 p41 in p42 vbp %s l=%gu w=%gu %s\n", mp,modelp,MCC_LP,MCC_WP,surf_string_p);
  mcc_printf_instance_specific(file, &mcc_user_lotrsparam_p);
  fprintf(file,"%sp43 p42 in p43 vbp %s l=%gu w=%gu %s\n", mp,modelp,MCC_LP,MCC_WP,surf_string_p);
  mcc_printf_instance_specific(file, &mcc_user_lotrsparam_p);
  fprintf(file,"%sp44 p43 in vdd vbp %s l=%gu w=%gu %s\n", mp,modelp,MCC_LP,MCC_WP,surf_string_p);
  mcc_printf_instance_specific(file, &mcc_user_lotrsparam_p);
  fprintf(file,"rn41 n41 0   10Meg\n");
  fprintf(file,"rn42 n42 0   10Meg\n");
  fprintf(file,"rn43 n43 0   10Meg\n");
  fprintf(file,"rp41 p41 vdd 10Meg\n");
  fprintf(file,"rp42 p42 vdd 10Meg\n");
  fprintf(file,"rp43 p43 vdd 10Meg\n");

  fprintf(file,"vddmax vdd 0 dc %gv\n",MCC_VDDmax);
  fprintf(file,"vbulkn vbn 0 %gv\n",(lotrsparam_n->VBULK > ELPMINVBULK) ? lotrsparam_n->VBULK : 0.0);
  fprintf(file,"vbulkp vbp 0 %gv\n",(lotrsparam_p->VBULK > ELPMINVBULK) ? lotrsparam_p->VBULK : MCC_VDDmax);
  fprintf(file,".ends\n\n");
      
  fprintf(file,"xinstn in outn1 outn2 outn3 outn4 %s\n", buf);
  fprintf(file,"xinstp in outp1 outp2 outp3 outp4 %s\n\n", buf);
  fprintf(file,".option tnom=%g\n", V_FLOAT_TAB[__SIM_TNOM].VALUE);
  fprintf(file,".temp %g\n\n",MCC_TEMP);     

  fprintf(file,"var in    0 dc %gv\n",MCC_VDDmax/2.0);
  fprintf(file,"vn1 outn1 0 dc %gv\n",MCC_VDDmax);
  fprintf(file,"vn2 outn2 0 dc %gv\n",MCC_VDDmax);
  fprintf(file,"vn3 outn3 0 dc %gv\n",MCC_VDDmax);
  fprintf(file,"vn4 outn4 0 dc %gv\n",MCC_VDDmax);
  fprintf(file,"vp1 outp1 0 dc %gv\n",0.0);
  fprintf(file,"vp2 outp2 0 dc %gv\n",0.0);
  fprintf(file,"vp3 outp3 0 dc %gv\n",0.0);
  fprintf(file,"vp4 outp4 0 dc %gv\n\n",0.0);

  fprintf(file,".dc var %g %g %g\n\n", 0.0, MCC_VDDmax, MCC_VDDmax/10.0 );
      
  fprintf(file,".print dc i(vn1)\n" );
  fprintf(file,".print dc i(vn2)\n" );
  fprintf(file,".print dc i(vn3)\n" );
  fprintf(file,".print dc i(vn4)\n" );
  fprintf(file,".print dc i(vp1)\n" );
  fprintf(file,".print dc i(vp2)\n" );
  fprintf(file,".print dc i(vp3)\n" );
  fprintf(file,".print dc i(vp4)\n" );
  
  if ( V_INT_TAB[__SIM_TOOL].VALUE == SIM_TOOL_TITAN ) 
    fprintf(file,".save %s\n", buf);

  fprintf(file,".end\n");
      
  mcc_fclose(file,name);

  sim_execspice(name,0,MCC_SPICENAME,MCC_SPICESTRING,MCC_SPICESTDOUT);

  fileout = sim_getjoker(MCC_SPICEOUT,buf) ;
      
  tab = (double **)mbkalloc(nbdata * sizeof(double *)) ;
  for(j = 0 ; j < nbdata ; j++)
    tab[j] = (double *)mbkalloc(11*sizeof(double)) ;
      
  sim_readspifiletab( fileout, argv, nbdata, 11, tab, MCC_VDDmax, MCC_VDDmax/10.0) ;

  tabin[0] = -tab[0][10]*1e6;
  tabin[1] = -tab[1][10]*1e6;
  tabin[2] = -tab[2][10]*1e6;
  tabin[3] = -tab[3][10]*1e6;
  tabip[0] =  tab[4][0]*1e6;
  tabip[1] =  tab[5][0]*1e6;
  tabip[2] =  tab[6][0]*1e6;
  tabip[3] =  tab[7][0]*1e6;
}

char *mcc_check_subckt(char *nmos, char *pmos)
{
 char *subcktmodeln, *subcktmodelp;
 mcc_modellist *ptmodeln,*ptmodelp;
 FILE *f;
 char *buf;
 
 MCC_CURRENT_LOTRS=NULL;
 subcktmodeln = mcc_getsubckt( MCC_MODELFILE,
                               MCC_TNMODEL,
                               MCC_NMOS,
                               MCC_NCASE,
                               MCC_LN*1.0e-6,
                               MCC_WN*1.0e-6
                             ) ;
 subcktmodelp = mcc_getsubckt( MCC_MODELFILE,
                               MCC_TPMODEL,
                               MCC_PMOS,
                               MCC_PCASE,
                               MCC_LP*1.0e-6,
                               MCC_WP*1.0e-6
                             ) ;
 if (subcktmodeln || subcktmodelp)
 {
    buf=sensitive_namealloc(mcc_debug_prefix("no_param.techno"));
    f=mcc_fopen(buf, "w");
    avt_printExecInfo(f, "*", "", "");
    fprintf(f, "* generated technofile with all evaluated parameters\n\n");
    
    ptmodeln = mcc_getmodel ( MCC_MODELFILE,
                              MCC_TNMODEL,
                              MCC_NMOS,
                              MCC_NCASE,
                              MCC_LN*1.0e-6,
                              MCC_WN*1.0e-6, 0);
    fprintf(f, "*\n*\n* NMOS **********************\n*\n*\n\n");
    mcc_drive_dot_model(f, ptmodeln);
    ptmodelp = mcc_getmodel ( MCC_MODELFILE,
                              MCC_TPMODEL,
                              MCC_PMOS,
                              MCC_PCASE,
                              MCC_LP*1.0e-6,
                              MCC_WP*1.0e-6, 0);

    fprintf(f, "*\n*\n* PMOS **********************\n*\n*\n\n");
    mcc_drive_dot_model(f, ptmodelp);
    strcpy(nmos, ptmodeln->NAME);
    strcpy(pmos, ptmodelp->NAME);
    fclose(f);
    return buf;
 }
 return NULL;
}

static void mcc_check_trs(lotrs_list *lt, elp_lotrs_param *lotrsparam_n, char *subname)
{
  ptype_list *pt;
  optparam_list *ptopt;
  int disp;
  losig_list *sigs[4]={lt->SOURCE->SIG, lt->GRID->SIG, lt->DRAIN->SIG, lt->BULK->SIG};
  char *names[4]={"source", "gate", "drain", "bulk"};
  char *paramname;
  double value;
  if ((pt=getptype(lt->USER, OPT_PARAMS))!=NULL)
  { 
    for (ptopt = (optparam_list *)pt->DATA; ptopt; ptopt = ptopt->NEXT)
    {
      disp=1;
      if (isknowntrsparam (ptopt->UNAME.STANDARD)) 
      {
        if (**(ptopt->UNAME.STANDARD) != '$')  paramname=*(ptopt->UNAME.STANDARD);
        else continue;
      }
      else if (ptopt->TAG != '$') 
      {
        paramname=ptopt->UNAME.SPECIAL;
      }
      else continue;

      value=ptopt->UDATA.VALUE;

      if (strcasecmp(paramname, "delvto")==0) lotrsparam_n->PARAM[elpDELVT0]=value;
      else if (strcasecmp(paramname, "mulu0")==0) lotrsparam_n->PARAM[elpMULU0]=value;
      else if (strcasecmp(paramname, "sa")==0) lotrsparam_n->PARAM[elpSA]=value;
      else if (strcasecmp(paramname, "sb")==0) lotrsparam_n->PARAM[elpSB]=value;
      else if (strcasecmp(paramname, "sd")==0) lotrsparam_n->PARAM[elpSD]=value;
      else if (strcasecmp(paramname, "nf")==0) lotrsparam_n->PARAM[elpNF]=value;
      else if (strcasecmp(paramname, "nrs")==0) lotrsparam_n->PARAM[elpNRS]=value;
      else if (strcasecmp(paramname, "nrd")==0) lotrsparam_n->PARAM[elpNRD]=value;
      else if (strcasecmp(paramname, "sc")==0) lotrsparam_n->PARAM[elpSC]=value;
      else if (strcasecmp(paramname, "sca")==0) lotrsparam_n->PARAM[elpSCA]=value;
      else if (strcasecmp(paramname, "scb")==0) lotrsparam_n->PARAM[elpSCB]=value;
      else if (strcasecmp(paramname, "scc")==0) lotrsparam_n->PARAM[elpSCC]=value;
      else if (strcasecmp(paramname, "m")==0)
      {
        if (mcc_ftoi(value)!=1)
         avt_errmsg(MCC_ERRMSG, "040", AVT_ERROR, mcc_ftoi(value));
        disp=0;
      }
      else disp=0;
      if (disp)
      {
         avt_errmsg(MCC_ERRMSG, "039", AVT_WARNING, paramname, value, subname);
      }
    }
  }
  for (disp=0; disp<4; disp++)
  {
    if (sigs[disp]->PRCN!=NULL && sigs[disp]->PRCN->PCTC!=NULL)
      avt_errmsg(MCC_ERRMSG, "041", AVT_WARNING, "capacitance", names[disp], subname);
    if (sigs[disp]->PRCN!=NULL && sigs[disp]->PRCN->PWIRE!=NULL)
      avt_errmsg(MCC_ERRMSG, "041", AVT_WARNING, "resistor", names[disp], subname);
  }

}

void mcc_check_techno_with_subckt(elp_lotrs_param *lotrsparam_n,elp_lotrs_param *lotrsparam_p, char *subcktmodeln, char *subcktmodelp)
{
 char *modeln, *modelp ;
 char bufn[1024] ;
 char bufp[1024] ;
 FILE *f;
 lofig_list *lf;
 lotrs_list *lt;

 modeln = MCC_TNMODEL ;
 modelp = MCC_TPMODEL ;
 sprintf(bufn,"m") ;
 sprintf(bufp,"m") ;

 if (subcktmodeln != NULL || subcktmodelp != NULL) 
  {
   if (subcktmodeln != NULL)
   {
    sprintf(bufn,"xm") ;
    modeln = subcktmodeln ;
   }
   if (subcktmodelp != NULL)
   {
    sprintf(bufp,"xm") ;
    modelp = subcktmodelp ;
   }

   f=mcc_fopen("test.spi", "w");
   avt_printExecInfo(f, "*", "", "");

   fprintf(f,"\n.subckt test in1 out1 vss in2 out2 vdd\n");
   fprintf(f,"%s1 out1 in1 vss vss %s l=%gu w=%gu %s\n"
           ,bufn,modeln,MCC_LN,MCC_WN,surf_string_n);
   mcc_printf_instance_specific(f, &mcc_user_lotrsparam_n);
   fprintf(f,"%s2 out2 in2 vdd vdd %s l=%gu w=%gu %s\n"
           ,bufp,modelp,MCC_LP,MCC_WP,surf_string_p);
   mcc_printf_instance_specific(f, &mcc_user_lotrsparam_p);
   fprintf(f,".ends\n");
   fclose(f);
 
   parsespice("test.spi");
   if ((lf=getloadedlofig("test"))!=NULL)
   {
      rflattenlofig(lf, YES, NO);
      for (lt=lf->LOTRS; lt!=NULL; lt=lt->NEXT)
      {
        if (MLO_IS_TRANSN(lt->TYPE) && subcktmodeln!=NULL)
           mcc_check_trs(lt, lotrsparam_n, subcktmodeln);
        if (MLO_IS_TRANSP(lt->TYPE) && subcktmodelp!=NULL)
           mcc_check_trs(lt, lotrsparam_p, subcktmodelp);
      }
      dellofig("test");
   }
   unlink("test.spi");
  }

}


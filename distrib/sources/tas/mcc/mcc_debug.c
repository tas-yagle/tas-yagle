/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : MCC Version 1                                               */
/*    Fichier : mcc_debug.c                                                 */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

#include "mcc_util.h"
#include "mcc_debug.h"
#include "mcc_genspi.h"
#include "mcc_drvelp.h"
#include "mcc_curv.h"
#include "mcc_fit.h"
#include "mcc_mod_util.h"
#include "mcc_mod_spice.h"
#include "mcc_mod_bsim4.h"
#include "mcc_optim.h"

#define MCC_NB_GATES 63

int MCC_DEBUG_MODE = 0 ;
char *MCC_DEBUG_GATE = NULL ;
char MCC_DEBUG_INPUT = -1 ;
char MCC_DEBUG_TRANS = -1 ;
int  MCC_DEBUG_SLOPE = 0 ;
int  MCC_DEBUG_CAPA = 0 ;
char MCC_DEBUG_RSAT = 'Y' ;
char MCC_DEBUG_LEAK = 'Y' ;
char MCC_DEBUG_RLIN = 'Y' ;
char MCC_DEBUG_VTH = 'Y' ;
char MCC_DEBUG_QINT = 'Y' ;
char MCC_DEBUG_JCT_CAPA = 'Y' ; // Junction capacitance => Cjbd, Cjbs...
char MCC_DEBUG_INPUTCAPA = 'Y' ;
char MCC_DEBUG_CARAC = 'Y' ;
char MCC_DEBUG_CARACNEG = 'Y' ;
char MCC_DEBUG_DEG = 'Y' ;
char MCC_DEBUG_PASS_TRANS  = 'X' ; // value can be 'N' or 'P'
char MCC_DEBUG_SWITCH = 'X' ;      // value can be 'N' or 'P'
char MCC_DEBUG_BLEEDER = 'N' ;   
char *MCC_DEBUG_PREFIX="";
char MCC_DEBUG_INTERNAL = 'N' ;
Icurvs TRS_CURVS; 
mcc_debug_internal MCC_DEBUG_INTERNAL_DATA ;

void (*hitas_tcl_pt)(char *name)=NULL;

char *mcc_debug_prefix(char *name)
{
  static char temp[128];
  if (strlen(MCC_DEBUG_PREFIX)==0) return name;
  sprintf(temp,"%s_%s", MCC_DEBUG_PREFIX, name);
  return temp;
}

float mcc_tanh_slope_tas(float t,float vt,float vinit,float vfin, float slope)
{
  SIM_FLOAT tab[7]={0, slope, vinit, vfin, vt, vfin>vinit?1:-1, 0};
  return sim_tanh_slope_tas(t, tab);
}

//extern ttvfig_list *hitas_tcl (char *figname);
void mcc_runtas_tcl(char *gate,float slope,float capa, char *output, char *figname)
{
  float save_slope, save_capa;
  char *save_out=V_STR_TAB[__TAS_FIG_NAME].VALUE;

  if (hitas_tcl_pt==NULL) return;
  parsespice(gate);
  
  save_slope=V_FLOAT_TAB[__FRONT_CON].VALUE;
  save_capa=V_FLOAT_TAB[__TAS_CAPAOUT].VALUE;
  V_FLOAT_TAB[__FRONT_CON].VALUE=slope; V_FLOAT_TAB[__FRONT_CON].SET=1; // ps -> s
  V_FLOAT_TAB[__TAS_CAPAOUT].VALUE=capa/1000; V_FLOAT_TAB[__TAS_CAPAOUT].SET=1; // ff -> pf
  V_STR_TAB[__TAS_FIG_NAME].VALUE=output; V_STR_TAB[__TAS_FIG_NAME].SET=1;
  hitas_tcl_pt(figname!=NULL?figname:gate);
  V_FLOAT_TAB[__FRONT_CON].VALUE=save_slope;
  V_FLOAT_TAB[__TAS_CAPAOUT].VALUE=save_capa;
  V_STR_TAB[__TAS_FIG_NAME].VALUE=save_out;
//  FRONT_CON
}

static void mcc_spice_hiername(char *inst0, char *inst1, char *res)
{
   switch(V_INT_TAB[__SIM_TOOL].VALUE)
   {
     case SIM_TOOL_ELDO: 
     case SIM_TOOL_HSPICE: 
     case SIM_TOOL_MSPICE: 
     case SIM_TOOL_NGSPICE:
     case SIM_TOOL_LTSPICE:
     case SIM_TOOL_SPICE:
     case SIM_TOOL_TITAN7: 
       sprintf( res, "%s%c%s", inst0, SPI_INS_SEPAR, inst1);
       break;
     case SIM_TOOL_TITAN: 
       sprintf( res, "%s%c%s", inst1, SPI_INS_SEPAR, inst0);
       break;
   }
}
void mcc_gettrsname(char type, char **pref, char **trans, char *trsinsname)
{
 char *subcktmodel;
 lofig_list *lf;
 char buf[128];
 if (type=='n')
   subcktmodel = mcc_getsubckt( MCC_MODELFILE,
                                 MCC_TNMODEL,
                                 MCC_NMOS,
                                 MCC_NCASE,
                                 MCC_LN*1.0e-6,
                                 MCC_WN*1.0e-6
                               ) ;
 else
   subcktmodel = mcc_getsubckt( MCC_MODELFILE,
                                 MCC_TPMODEL,
                                 MCC_PMOS,
                                 MCC_PCASE,
                                 MCC_LP*1.0e-6,
                                 MCC_WP*1.0e-6
                               ) ;
 if (subcktmodel != NULL)
  {
   sprintf(buf,"x%s", trsinsname);
   *pref=sensitive_namealloc(buf);
   *trans="?";
   if (trans!=NULL && (lf=getloadedlofig(subcktmodel))!=NULL)
     if (lf->LOTRS!=NULL)
     {
       mcc_spice_hiername(*pref, lf->LOTRS->TRNAME, buf);
       *trans=sensitive_namealloc(buf);
     }
  }
 else
 {
   *trans=*pref=sensitive_namealloc(trsinsname);
 }
}

/****************************************************************************\
 * Function : mcc_q_coupling_capa
 *
\****************************************************************************/
void mcc_q_coupling_capa(elp_lotrs_param *lotrsparam_n, elp_lotrs_param *lotrsparam_p,
                             double vgsn, double vdsn, double vgsp, double vdsp,
                             double *qgpn, double *qgpp, double *qgdn, double *qgdp)
{
  double vbsn = lotrsparam_n->VBULK;
  double vbsp = lotrsparam_p->VBULK - MCC_VDDmax;
    
  if( lotrsparam_n->ISVBSSET )
    vbsn = lotrsparam_n->VBS ;
  if( lotrsparam_p->ISVBSSET )
    vbsp = lotrsparam_p->VBS ;

  mcc_calcCGP (MCC_MODELFILE, MCC_TNMODEL,
               MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, 
               MCC_WN * 1.0e-6, vgsn, qgpn, lotrsparam_n, MCC_TEMP);
  mcc_calcCGP (MCC_MODELFILE, MCC_TPMODEL,
               MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, 
               MCC_WP * 1.0e-6, vgsp, qgpp, lotrsparam_p, MCC_TEMP);
  mcc_calcQint (MCC_MODELFILE, MCC_TNMODEL,MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6,
                MCC_TEMP, vgsn, vbsn, vdsn, NULL, NULL, qgdn, NULL,lotrsparam_n);
  mcc_calcQint (MCC_MODELFILE, MCC_TPMODEL,MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6,
                MCC_TEMP, vgsp, vbsp, vdsp, NULL, NULL, qgdp, NULL,lotrsparam_p);

  if(qgpn)
      *qgpn = *qgpn * MCC_WNeff * 1.0e-6;
  if(qgpp)
      *qgpp = *qgpp * MCC_WPeff * 1.0e-6;
  if(qgdn)
      *qgdn = *qgdn * (MCC_WN*MCC_XWN+MCC_DWCN) * (MCC_LN*MCC_XLN+MCC_DLCN) * 1.0e-12;
  if(qgdp)
      *qgdp = *qgdp * (MCC_WP*MCC_XWP+MCC_DWCP) * (MCC_LP*MCC_XLP+MCC_DLCP) * 1.0e-12;

}

/****************************************************************************\
 * Function : mcc_calc_coupling_capa
 *
\****************************************************************************/
void mcc_calc_coupling_capa( elp_lotrs_param *lotrsparam_n, elp_lotrs_param *lotrsparam_p )
{
  double vbsn = lotrsparam_n->VBULK;
  double vbsp = lotrsparam_p->VBULK - MCC_VDDmax;
  double qgpn,  qgpp,  qdn,  qdp  ;
  double qgpn0, qgpp0, qdn0, qdp0 ;
  double cgpn,  cgpp,  cdn,  cdp, ctot  ;
  double vdsn,  vdsp,  vgsn, vgsp ;
  double vgsn0,  vgsp0 ;
  double vgd0, vgd ;
  double vini, vinf, vstep ;
  double in, ip ;
  double vin, vout, vin0, vout0 ;
  int    i, j, k ;
  FILE  *file ;
  char   filedat[1024] ;
  int    mode, maxmode ;
  char  *modetitle[]={"vout static","infinite load","realistic vout","original"};
  double vinud2[]  = { 0,          MCC_VTN,    0.50*MCC_VDDmax, MCC_VDDmax-MCC_VTP, MCC_VDDmax-MCC_VTP/2.0, MCC_VDDmax };
  double voutud2[] = { MCC_VDDmax, MCC_VDDmax, 0.75*MCC_VDDmax, 0.50*MCC_VDDmax,    0.25*MCC_VDDmax,        0.0        };
  double vindu2[]  = { MCC_VDDmax, MCC_VDDmax-MCC_VTP, 0.50*MCC_VDDmax, MCC_VTN,         MCC_VTN/2.0,     0.0        };
  double voutdu2[] = { 0.0,        0.0,                0.25*MCC_VDDmax, 0.50*MCC_VDDmax, 0.75*MCC_VDDmax, MCC_VDDmax }; 
  double vinud3[]  = { 0,          MCC_VTN,    0.50*MCC_VDDmax, MCC_VDDmax-MCC_VTP, MCC_VDDmax, MCC_VDDmax };
  double voutud3[] = { MCC_VDDmax, MCC_VDDmax, 0.75*MCC_VDDmax, 0.50*MCC_VDDmax,    0.25*MCC_VDDmax,        0.0        };
  double vindu3[]  = { MCC_VDDmax, MCC_VDDmax-MCC_VTP, 0.50*MCC_VDDmax, MCC_VTN,         0.0,     0.0        };
  double voutdu3[] = { 0.0,        0.0,                0.25*MCC_VDDmax, 0.50*MCC_VDDmax, 0.75*MCC_VDDmax, MCC_VDDmax }; 
  double *tabvin, *tabvout ;
  int    n=20 ;

  if( lotrsparam_n->ISVBSSET )
    vbsn = lotrsparam_n->VBS ;
    
  if( lotrsparam_p->ISVBSSET )
    vbsp = lotrsparam_p->VBS ;

  if( MCC_VTN < MCC_VDDmax/2.0 && MCC_VDDmax-MCC_VTP > MCC_VDDmax/2.0 )
    maxmode = 2 ;
  else
    maxmode = 1 ;

  for( mode=0 ; mode<=maxmode ; mode ++ ) {

    for( i=0 ; i<=1 ; i++ ) {

      if( i==0 ) {
        sprintf( filedat, "%s_%d", mcc_debug_prefix( "coupling_capa_ud" ), mode );
        vini = 0.0 ;
        vinf = MCC_VDDmax ;
      }
      else {
        sprintf( filedat, "%s_%d", mcc_debug_prefix( "coupling_capa_du" ), mode );
        vini = MCC_VDDmax ;
        vinf = 0.0 ;
      }

      file = mbkfopen( filedat, "plt", WRITE_TEXT );
      fprintf( file, "set title '%s'\n", modetitle[mode] );
      fprintf( file, "plot \\\n" );
      fprintf( file, "'%s.dat' using 1:3  title 'covn(vgd)', \\\n", filedat );
      fprintf( file, "'%s.dat' using 1:4  title 'covp(vgd)', \\\n", filedat );
      fprintf( file, "'%s.dat' using 1:5  title 'cintn(vgd)', \\\n", filedat );
      fprintf( file, "'%s.dat' using 1:6  title 'cintp(vgd)', \\\n", filedat );
      fprintf( file, "'%s.dat' using 1:7  title 'ctot(vgd)'\n", filedat );
      fprintf( file, "pause -1\n" );
      fprintf( file, "set title '%s'\n", modetitle[mode] );
      fprintf( file, "plot \\\n" );
      fprintf( file, "'%s.dat' using 1:3  title 'covn(vgd)', \\\n", filedat );
      fprintf( file, "'%s.dat' using 1:4  title 'covp(vgd)', \\\n", filedat );
      fprintf( file, "'%s.dat' using 1:5  title 'cintn(vgd)', \\\n", filedat );
      fprintf( file, "'%s.dat' using 1:6  title 'cintp(vgd)', \\\n", filedat );
      fprintf( file, "'%s.dat' using 1:7  title 'ctot(vgd)', \\\n", filedat );
      fprintf( file, "'%s.dat' using 1:8  title 'QDn', \\\n", filedat );
      fprintf( file, "'%s.dat' using 1:9  title 'QDp', \\\n", filedat );
      fprintf( file, "'%s.dat' using 1:10 title 'QDtot'\n", filedat );
      fprintf( file, "pause -1\n" );
      fprintf( file, "plot \\\n" );
      fprintf( file, "'%s.dat' using 11:3  title 'covn(vgd)', \\\n", filedat );
      fprintf( file, "'%s.dat' using 11:4  title 'covp(vgd)', \\\n", filedat );
      fprintf( file, "'%s.dat' using 11:5  title 'cintn(vgd)', \\\n", filedat );
      fprintf( file, "'%s.dat' using 11:6  title 'cintp(vgd)', \\\n", filedat );
      fprintf( file, "'%s.dat' using 11:7  title 'ctot(vgd)'\n", filedat );
      fprintf( file, "pause -1\n" );
      fclose( file );
      
      file = mbkfopen( filedat, "dat", WRITE_TEXT ) ;
      fprintf( file ,"index vin vout cgpn cgpp cdn cdp ctot\n" );

      vstep = (vinf-vini)/n ;
      for( j=0 ; j<=n ; j++ ) {
    
        vin = vini+vstep*((double)j) ;

        switch( mode ) {
          case 0 :

            vgsn = vin ;
            vgsp = MCC_VDDmax-vin ;

            in = mcc_spicecurrent( vgsn, MCC_VDDmax, vbsn, MCC_TRANS_N, lotrsparam_n );
            ip = mcc_spicecurrent( vgsp, MCC_VDDmax, vbsp, MCC_TRANS_P, lotrsparam_p );
            vout = MCC_VDDmax * ip/(in+ip) ;
            break ;
          case 1 : 
            if( i==0 )
              vout = MCC_VDDmax ;
            else
              vout = 0.0 ;
            break ;
          case 2 :
            if( i==0 ) {
              tabvin = vinud2 ;
              tabvout = voutud2 ;
              for( k=0 ; k<5 ; k++ ) 
                if( vin >= tabvin[k] && vin<=tabvin[k+1] )
                  break ;
            }
            else {
              tabvin = vindu2 ;
              tabvout = voutdu2 ;
              for( k=0 ; k<5 ; k++ ) 
                if( vin <= tabvin[k] && vin >= tabvin[k+1] )
                  break ;
            }
            vout = tabvout[k]+(vin-tabvin[k])*(tabvout[k+1]-tabvout[k])/(tabvin[k+1]-tabvin[k]);
            break ;
          case 3:
            k=(int)(((float)j)/((float)(n+1))*6.0) ;
            if( i==0 ) {
              tabvin = vinud3 ;
              tabvout = voutud3 ;
            }
            else {
              tabvin = vindu3 ;
              tabvout = voutdu3 ;
            }
            vin = tabvin[k];
            vout = tabvout[k];
            break ;
        }

        vgsn = vin ;
        vgsp = vin-MCC_VDDmax ;

        vdsn = vout ;
        vdsp = vout-MCC_VDDmax ;

        vgd  = vin-vout;

        mcc_calcCGP (MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6, vgd, &qgpn, lotrsparam_n, MCC_TEMP);
        mcc_calcCGP (MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6, vgd, &qgpp, lotrsparam_p, MCC_TEMP);
        mcc_calcQint (MCC_MODELFILE, MCC_TNMODEL,MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6, MCC_TEMP, vgsn, vbsn, vdsn, NULL, NULL, &qdn, NULL,lotrsparam_n);
        mcc_calcQint (MCC_MODELFILE, MCC_TPMODEL,MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6, MCC_TEMP, vgsp, vbsp, vdsp, NULL, NULL, &qdp, NULL,lotrsparam_p);

        qgpn = qgpn * MCC_WNeff * 1.0e-6;
        qgpp = qgpp * MCC_WPeff * 1.0e-6;
        qdn = qdn * (MCC_WN*MCC_XWN+MCC_DWCN) * (MCC_LN*MCC_XLN+MCC_DLCN) * 1.0e-12;
        qdp = qdp * (MCC_WP*MCC_XWP+MCC_DWCP) * (MCC_LP*MCC_XLP+MCC_DLCP) * 1.0e-12;

        if( j>0 && fabs(vgd-vgd0)>1e-6 ) {
       
          cgpn = (qgpn-qgpn0)/(vgd-vgd0)*1e15 ;
          cgpp = (qgpp-qgpp0)/(vgd-vgd0)*1e15 ;
          cdn  = -(qdn-qdn0)/(vgd-vgd0)*1e15 ;
          cdp  = -(qdp-qdp0)/(vgd-vgd0)*1e15 ;

          ctot = fabs(cgpn)+fabs(cgpp)+fabs(cdn)+fabs(cdp) ;
          
          fprintf( file, "%g %g %g %g %g %g %g %g %g %g %g\n", vin0, vout0, cgpn, cgpp, cdn, cdp, ctot, qdn0*1e15, qdp0*1e15, (qdn0+qdp0)*1e15, vgd0 );
          fprintf( file, "%g %g %g %g %g %g %g %g %g %g %g\n", vin,  vout,  cgpn, cgpp, cdn, cdp, ctot, qdn*1e15,  qdp*1e15, (qdn+qdp)*1e15, vgd );
        }

        qgpn0 = qgpn  ;
        qgpp0 = qgpp  ;
        qdn0  = qdn   ;
        qdp0  = qdp   ;
        vgd0  = vgd   ;
        vin0  = vin   ;
        vgsn0 = vgsn  ;
        vgsp0 = vgsp  ;
        vout0 = vout  ;
        
      }

      fclose( file );
    }
  }

  for( i=0 ; i<=1 ; i++ ) {
    if( i==0 )
      sprintf( filedat, "%s", mcc_debug_prefix( "coupling_capa_ud" ) );
    else
      sprintf( filedat, "%s", mcc_debug_prefix( "coupling_capa_du" ) );
    file = mbkfopen( filedat, "plt", WRITE_TEXT );

    fprintf( file, "set title 'voltage for capacitance evaluation'\n" );
    fprintf( file, "plot \\\n" );
    for( mode = 0 ; mode <= maxmode ; mode++ ) {
      if( mode==maxmode )
        fprintf( file, "'%s_%d.dat' using 1:2 title '%s'\n", filedat, mode, modetitle[mode] );
      else
        fprintf( file, "'%s_%d.dat' using 1:2 title '%s', \\\n", filedat, mode, modetitle[mode] );
    } 
    fprintf( file, "pause -1\n" );

    fprintf( file, "set title 'N charge value'\n" );
    fprintf( file, "plot [:]\\\n" );
    for( mode = 0 ; mode <= maxmode ; mode++ ) {
      if( mode==maxmode )
        fprintf( file, "'%s_%d.dat' using 1:8 title '%s'\n", filedat, mode, modetitle[mode] );
      else
        fprintf( file, "'%s_%d.dat' using 1:8 title '%s', \\\n", filedat, mode, modetitle[mode] );
    } 
    fprintf( file, "pause -1\n" );
    fprintf( file, "set title 'P charge value'\n" );
    fprintf( file, "plot [:]\\\n" );
    for( mode = 0 ; mode <= maxmode ; mode++ ) {
      if( mode==maxmode )
        fprintf( file, "'%s_%d.dat' using 1:9 title '%s'\n", filedat, mode, modetitle[mode] );
      else
        fprintf( file, "'%s_%d.dat' using 1:9 title '%s', \\\n", filedat, mode, modetitle[mode] );
    } 
    fprintf( file, "pause -1\n" );
    fprintf( file, "set title 'Total charge value'\n" );
    fprintf( file, "plot [:]\\\n" );
    for( mode = 0 ; mode <= maxmode ; mode++ ) {
      if( mode==maxmode )
        fprintf( file, "'%s_%d.dat' using 1:10 title '%s'\n", filedat, mode, modetitle[mode] );
      else
        fprintf( file, "'%s_%d.dat' using 1:10 title '%s', \\\n", filedat, mode, modetitle[mode] );
    } 
    fprintf( file, "pause -1\n" );

    fprintf( file, "set title 'capacitance value'\n" );
    fprintf( file, "plot [:][0:] \\\n" );
    for( mode = 0 ; mode <= maxmode ; mode++ ) {
      if( mode==maxmode )
        fprintf( file, "'%s_%d.dat' using 1:7 title '%s'\n", filedat, mode, modetitle[mode] );
      else
        fprintf( file, "'%s_%d.dat' using 1:7 title '%s', \\\n", filedat, mode, modetitle[mode] );
    } 
    fprintf( file, "pause -1\n" );
    fclose( file );
  }

}

void mcc_calc_coupling_capa_old (elp_lotrs_param *lotrsparam_n, elp_lotrs_param *lotrsparam_p)
{
  FILE *file1;
  double covn, covp, cintn, cintp, sumcapa, qgpn0, qgpp0, qgdn0, qgdp0, qgpn1, qgpp1, qgdn1, qgdp1;
  double qgpn_init, qgpp_init, qgdn_init, qgdp_init, qgpn_fin, qgpp_fin, qgdn_fin, qgdp_fin, capai, kfup, kfdown;
  double max;
  double vbsn = lotrsparam_n->VBULK;
  double vbsp = lotrsparam_p->VBULK - MCC_VDDmax;
  double VN[3], VP[3];
    
  if( lotrsparam_n->ISVBSSET )
    vbsn = lotrsparam_n->VBS ;
  if( lotrsparam_p->ISVBSSET )
    vbsp = lotrsparam_p->VBS ;

  /* 0 < VN[0] < VN[1] < VN[2] < VDD */
  /* VDD > VP[2] > VP[1] > VP[0] > 0 */
  VN[0] = MCC_VTN;
  VN[1] = MCC_VDDmax/2.0;
  VN[2] = MCC_VDDmax-MCC_VTP;
  VP[0] = MCC_VTP;
  VP[1] = MCC_VDDmax/2.0;
  VP[2] = MCC_VDDmax-MCC_VTN;
  qsort(VN, 3, sizeof(double), (void *)mbk_qsort_dbl_cmp);
  qsort(VP, 3, sizeof(double), (void *)mbk_qsort_dbl_cmp);
  
  kfup = 1.0 - (MCC_VDDmax - MCC_VTP - MCC_VTN) / (MCC_VDDmax - MCC_VTP);
  if(kfup > 1.0) kfup = 1.0;
  kfdown = 1.0 - (MCC_VDDmax - MCC_VTN - MCC_VTP) / (MCC_VDDmax - MCC_VTN);
  if(kfdown > 1.0) kfdown = 1.0;
    

  // UP
  file1 = mbkfopen(mcc_debug_prefix("coupling_capa_up"),"dat",WRITE_TEXT) ;
  avt_printExecInfo(file1, "#", "", "");
  fprintf(file1,"# zone covn covp cintn cintp sumcapa  \n");
  
  /*etat initial*/
  mcc_q_coupling_capa(lotrsparam_n, lotrsparam_p, 0.0, MCC_VDDmax, MCC_VDDmax, 0.0, &qgpn0, &qgpp0, &qgdn0, &qgdp0);
  qgpn_init = qgpn0;
  qgpp_init = qgpp0;
  qgdn_init = qgdn0;
  qgdp_init = qgdp0;
  mcc_q_coupling_capa(lotrsparam_n, lotrsparam_p, MCC_VDDmax, MCC_VDDmax, 0.0, 0.0, &qgpn1, &qgpp1, &qgdn1, &qgdp1);
  qgpn_fin = qgpn1;
  qgpp_fin = qgpp1;
  qgdn_fin = qgdn1;
  qgdp_fin = qgdp1;
  capai = ((fabs((qgpn_fin-qgpn_init)) + fabs((qgpp_fin-qgpp_init))) * (1.0 + MCC_VTN*(kfup-1.0)/MCC_VDDmax)
          + fabs((qgdn_fin-qgdn_init)) 
          + fabs((qgdp_fin-qgdp_init)) * (1.0 + MCC_VTN*(kfup-1.0)/(MCC_VDDmax-MCC_VTP)))
          /(MCC_VDDmax)*1e15;
      
  /*etat final*/
  mcc_q_coupling_capa(lotrsparam_n, lotrsparam_p, VN[0], MCC_VDDmax, VP[2], 0.0, &qgpn1, &qgpp1, &qgdn1, &qgdp1);

  covn = fabs((qgpn1-qgpn0)/(VN[0]))*1e15;
  covp = fabs((qgpp1-qgpp0)/(MCC_VDDmax - VP[2]))*1e15;
  cintn = fabs((qgdn1-qgdn0)/(VN[0]))*1e15;
  cintp = fabs((qgdp1-qgdp0)/(MCC_VDDmax - VP[2]))*1e15;
  sumcapa = covn + covp + cintn + cintp;
  max = sumcapa;
  fprintf(file1," 0 %g %g %g %g %g %g\n", covn, covp, cintn, cintp, sumcapa, capai);
  fprintf(file1," 1 %g %g %g %g %g %g\n", covn, covp, cintn, cintp, sumcapa, capai);

  qgpn0 = qgpn1;
  qgpp0 = qgpp1;
  qgdn0 = qgdn1;
  qgdp0 = qgdp1;
  
  mcc_q_coupling_capa(lotrsparam_n, lotrsparam_p, VN[1], MCC_VDDmax*3.0/4.0, VP[1], MCC_VDDmax/4.0, &qgpn1, &qgpp1, &qgdn1, &qgdp1);

  covn = fabs((qgpn1-qgpn0)/(VN[1] - VN[0]))*1e15;
  covp = fabs((qgpp1-qgpp0)/(VP[2] - VP[1]))*1e15;
  cintn = fabs((qgdn1-qgdn0)/(VN[1] - VN[0] + MCC_VDDmax/4.0))*1e15;
  cintp = fabs((qgdp1-qgdp0)/(VP[2] - VP[1] + MCC_VDDmax/4.0))*1e15;
  sumcapa = covn + covp + cintn + cintp;
  if (sumcapa > max) max = sumcapa;
  fprintf(file1," 1 %g %g %g %g %g %g\n", covn, covp, cintn, cintp, sumcapa, capai);
  fprintf(file1," 2 %g %g %g %g %g %g\n", covn, covp, cintn, cintp, sumcapa, capai);

  qgpn0 = qgpn1;
  qgpp0 = qgpp1;
  qgdn0 = qgdn1;
  qgdp0 = qgdp1;
  
  mcc_q_coupling_capa(lotrsparam_n, lotrsparam_p, VN[2], MCC_VDDmax/2.0, VP[0], MCC_VDDmax/2.0, &qgpn1, &qgpp1, &qgdn1, &qgdp1);

  covn = fabs((qgpn1-qgpn0)/(VN[2] - VN[1]))*1e15;
  covp = fabs((qgpp1-qgpp0)/(VP[1] - VP[0]))*1e15;
  cintn = fabs((qgdn1-qgdn0)/(VN[2] - VN[1] + MCC_VDDmax/4.0))*1e15;
  cintp = fabs((qgdp1-qgdp0)/(VP[1] - VP[0] + MCC_VDDmax/4.0))*1e15;
  sumcapa = covn + covp + cintn + cintp;
  if (sumcapa > max) max = sumcapa;
  fprintf(file1," 2 %g %g %g %g %g %g\n", covn, covp, cintn, cintp, sumcapa, capai);
  fprintf(file1," 3 %g %g %g %g %g %g\n", covn, covp, cintn, cintp, sumcapa, capai);

  qgpn0 = qgpn1;
  qgpp0 = qgpp1;
  qgdn0 = qgdn1;
  qgdp0 = qgdp1;

  mcc_q_coupling_capa(lotrsparam_n, lotrsparam_p, MCC_VDDmax, MCC_VDDmax/4.0, 0.0, MCC_VDDmax*3.0/4.0, &qgpn1, &qgpp1, &qgdn1, &qgdp1);

  covn = fabs((qgpn1-qgpn0)/(MCC_VDDmax - VN[2]))*1e15;
  covp = fabs((qgpp1-qgpp0)/(VP[0]))*1e15;
  cintn = fabs((qgdn1-qgdn0)/(MCC_VDDmax - VN[2] + MCC_VDDmax/4.0))*1e15;
  cintp = fabs((qgdp1-qgdp0)/(VP[0] + MCC_VDDmax/4.0))*1e15;
  sumcapa = covn + covp + cintn + cintp;
  if (sumcapa > max) max = sumcapa;
  fprintf(file1," 3 %g %g %g %g %g %g\n", covn, covp, cintn, cintp, sumcapa, capai);
  fprintf(file1," 4 %g %g %g %g %g %g\n", covn, covp, cintn, cintp, sumcapa, capai);

  qgpn0 = qgpn1;
  qgpp0 = qgpp1;
  qgdn0 = qgdn1;
  qgdp0 = qgdp1;
    
  mcc_q_coupling_capa(lotrsparam_n, lotrsparam_p, MCC_VDDmax, 0.0, 0.0, MCC_VDDmax, &qgpn1, &qgpp1, &qgdn1, &qgdp1);

  cintn = fabs((qgdn1-qgdn0)/(MCC_VDDmax/4.0))*1e15;
  cintp = fabs((qgdp1-qgdp0)/(MCC_VDDmax/4.0))*1e15;
  sumcapa = covn + covp + cintn + cintp;
  if (sumcapa > max) max = sumcapa;
  fprintf(file1," 4 %g %g %g %g %g %g\n", covn, covp, cintn, cintp, sumcapa, capai);
  fprintf(file1," 5 %g %g %g %g %g %g\n", covn, covp, cintn, cintp, sumcapa, capai);
  
  fclose(file1) ;

  file1 = mbkfopen(mcc_debug_prefix("coupling_capa_up"),"plt",WRITE_TEXT) ;
  avt_printExecInfo(file1, "#", "", "");
  fprintf(file1,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE);
  fprintf(file1,"set title 'UP vdd=%g vtn=%g vtp=%g'\n", MCC_VDDmax, MCC_VTN, MCC_VTP);
  fprintf(file1,"set grid\n");
  fprintf(file1,"set ytics %g\n", max/10.0);
  fprintf(file1,"set ylabel \"C(fF)\"\n ");
  fprintf(file1,"set xrange [0:5]\n ");
  fprintf(file1,"plot '%s' using 1:2 title 'covn' with lines\n", mcc_debug_prefix("coupling_capa_up.dat"));
  fprintf(file1,"pause -1 'Hit CR to finish'\n");
  fprintf(file1,"plot '%s' using 1:3 title 'covp' with lines\n", mcc_debug_prefix("coupling_capa_up.dat"));
  fprintf(file1,"pause -1 'Hit CR to finish'\n");
  fprintf(file1,"plot '%s' using 1:4 title 'cintn' with lines\n", mcc_debug_prefix("coupling_capa_up.dat"));
  fprintf(file1,"pause -1 'Hit CR to finish'\n");
  fprintf(file1,"plot '%s' using 1:5 title 'cintp' with lines\n", mcc_debug_prefix("coupling_capa_up.dat"));
  fprintf(file1,"pause -1 'Hit CR to finish'\n");
  fprintf(file1,"plot '%s' using 1:6 title 'sumcapa' with lines,\\\n", mcc_debug_prefix("coupling_capa_up.dat"));
  fprintf(file1,"'%s' using 1:7 title 'capai' with lines\n", mcc_debug_prefix("coupling_capa_down.dat"));
  fprintf(file1,"pause -1 'Hit CR to finish'\n");
  fclose(file1) ;

  // DOWN
  file1 = mbkfopen(mcc_debug_prefix("coupling_capa_down"),"dat",WRITE_TEXT) ;
  avt_printExecInfo(file1, "#", "", "");
  fprintf(file1,"# zone covn covp cintn cintp sumcapa  \n");
  
  /*etat initial*/
  mcc_q_coupling_capa(lotrsparam_n, lotrsparam_p, MCC_VDDmax, 0.0, 0.0, MCC_VDDmax, &qgpn0, &qgpp0, &qgdn0, &qgdp0);
  qgpn_init = qgpn0;
  qgpp_init = qgpp0;
  qgdn_init = qgdn0;
  qgdp_init = qgdp0;
  mcc_q_coupling_capa(lotrsparam_n, lotrsparam_p, 0.0, 0.0, MCC_VDDmax, MCC_VDDmax, &qgpn1, &qgpp1, &qgdn1, &qgdp1);
  qgpn_fin = qgpn1;
  qgpp_fin = qgpp1;
  qgdn_fin = qgdn1;
  qgdp_fin = qgdp1;
  capai = ((fabs((qgpn_fin-qgpn_init)) + fabs((qgpp_fin-qgpp_init))) * (1.0 + MCC_VTP*(kfdown-1.0)/MCC_VDDmax)
          + fabs((qgdn_fin-qgdn_init)) * (1.0 + MCC_VTP*(kfdown-1.0)/(MCC_VDDmax-MCC_VTN))
          + fabs((qgdp_fin-qgdp_init)))
          /(MCC_VDDmax)*1e15;

  /*etat final*/
  mcc_q_coupling_capa(lotrsparam_n, lotrsparam_p, VN[2], 0.0, VP[0], MCC_VDDmax, &qgpn1, &qgpp1, &qgdn1, &qgdp1);

  covn = fabs((qgpn1-qgpn0)/(MCC_VDDmax - VN[2]))*1e15;
  covp = fabs((qgpp1-qgpp0)/(VP[0]))*1e15;
  cintn = fabs((qgdn1-qgdn0)/(MCC_VDDmax - VN[2]))*1e15;
  cintp = fabs((qgdp1-qgdp0)/(VP[0]))*1e15;
  sumcapa = covn + covp + cintn + cintp;
  max = sumcapa;
  fprintf(file1," 0 %g %g %g %g %g %g\n", covn, covp, cintn, cintp, sumcapa, capai);
  fprintf(file1," 1 %g %g %g %g %g %g\n", covn, covp, cintn, cintp, sumcapa, capai);

  qgpn0 = qgpn1;
  qgpp0 = qgpp1;
  qgdn0 = qgdn1;
  qgdp0 = qgdp1;
 
  mcc_q_coupling_capa(lotrsparam_n, lotrsparam_p, VN[1], MCC_VDDmax/4.0, VP[1], MCC_VDDmax*3.0/4.0, &qgpn1, &qgpp1, &qgdn1, &qgdp1);

  covn = fabs((qgpn1-qgpn0)/(VN[2] - VN[1]))*1e15;
  covp = fabs((qgpp1-qgpp0)/(VP[1] - VP[0]))*1e15;
  cintn = fabs((qgdn1-qgdn0)/(VN[2] - VN[1] + MCC_VDDmax/4.0))*1e15;
  cintp = fabs((qgdp1-qgdp0)/(VP[1] - VP[0] + MCC_VDDmax/4.0))*1e15;
  sumcapa = covn + covp + cintn + cintp;
  if (sumcapa > max) max = sumcapa;
  fprintf(file1," 1 %g %g %g %g %g %g\n", covn, covp, cintn, cintp, sumcapa, capai);
  fprintf(file1," 2 %g %g %g %g %g %g\n", covn, covp, cintn, cintp, sumcapa, capai);

  qgpn0 = qgpn1;
  qgpp0 = qgpp1;
  qgdn0 = qgdn1;
  qgdp0 = qgdp1;
 
  mcc_q_coupling_capa(lotrsparam_n, lotrsparam_p, VN[0], MCC_VDDmax/2.0, VP[2], MCC_VDDmax/2.0, &qgpn1, &qgpp1, &qgdn1, &qgdp1);

  covn = fabs((qgpn1-qgpn0)/(VN[1] - VN[0]))*1e15;
  covp = fabs((qgpp1-qgpp0)/(VP[2] - VP[1]))*1e15;
  cintn = fabs((qgdn1-qgdn0)/(VN[1] - VN[0] + MCC_VDDmax/4.0))*1e15;
  cintp = fabs((qgdp1-qgdp0)/(VP[2] - VP[1] + MCC_VDDmax/4.0))*1e15;
  sumcapa = covn + covp + cintn + cintp;
  if (sumcapa > max) max = sumcapa;
  fprintf(file1," 2 %g %g %g %g %g %g\n", covn, covp, cintn, cintp, sumcapa, capai);
  fprintf(file1," 3 %g %g %g %g %g %g\n", covn, covp, cintn, cintp, sumcapa, capai);

  qgpn0 = qgpn1;
  qgpp0 = qgpp1;
  qgdn0 = qgdn1;
  qgdp0 = qgdp1;
 
  mcc_q_coupling_capa(lotrsparam_n, lotrsparam_p, 0.0, MCC_VDDmax*3.0/4.0, MCC_VDDmax, MCC_VDDmax/4.0, &qgpn1, &qgpp1, &qgdn1, &qgdp1);

  covn = fabs((qgpn1-qgpn0)/(VN[0]))*1e15;
  covp = fabs((qgpp1-qgpp0)/(MCC_VDDmax - VP[2]))*1e15;
  cintn = fabs((qgdn1-qgdn0)/(VN[0] + MCC_VDDmax/4.0))*1e15;
  cintp = fabs((qgdp1-qgdp0)/(MCC_VDDmax - VP[2] + MCC_VDDmax/4.0))*1e15;
  sumcapa = covn + covp + cintn + cintp;
  if (sumcapa > max) max = sumcapa;
  fprintf(file1," 3 %g %g %g %g %g %g\n", covn, covp, cintn, cintp, sumcapa, capai);
  fprintf(file1," 4 %g %g %g %g %g %g\n", covn, covp, cintn, cintp, sumcapa, capai);

  qgpn0 = qgpn1;
  qgpp0 = qgpp1;
  qgdn0 = qgdn1;
  qgdp0 = qgdp1;
 
  mcc_q_coupling_capa(lotrsparam_n, lotrsparam_p, 0.0, MCC_VDDmax, MCC_VDDmax, 0.0, &qgpn1, &qgpp1, &qgdn1, &qgdp1);

  cintn = fabs((qgdn1-qgdn0)/(MCC_VDDmax/4.0))*1e15;
  cintp = fabs((qgdp1-qgdp0)/(MCC_VDDmax/4.0))*1e15;
  sumcapa = covn + covp + cintn + cintp;
  if (sumcapa > max) max = sumcapa;
  fprintf(file1," 4 %g %g %g %g %g %g\n", covn, covp, cintn, cintp, sumcapa, capai);
  fprintf(file1," 5 %g %g %g %g %g %g\n", covn, covp, cintn, cintp, sumcapa, capai);

  fclose(file1) ;
  
  file1 = mbkfopen(mcc_debug_prefix("coupling_capa_down"),"plt",WRITE_TEXT) ;
  avt_printExecInfo(file1, "#", "", "");
  fprintf(file1,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE);
  fprintf(file1,"set title 'DOWN vdd=%g vtn=%g vtp=%g'\n", MCC_VDDmax, MCC_VTN, MCC_VTP);
  fprintf(file1,"set grid\n");
  fprintf(file1,"set ytics %g\n", max/10.0);
  fprintf(file1,"set ylabel \"C(fF)\"\n ");
  fprintf(file1,"set xrange [0:5]\n ");
  fprintf(file1,"plot '%s' using 1:2 title 'covn' with lines\n", mcc_debug_prefix("coupling_capa_down.dat"));
  fprintf(file1,"pause -1 'Hit CR to finish'\n");
  fprintf(file1,"plot '%s' using 1:3 title 'covp' with lines\n", mcc_debug_prefix("coupling_capa_down.dat"));
  fprintf(file1,"pause -1 'Hit CR to finish'\n");
  fprintf(file1,"plot '%s' using 1:4 title 'cintn' with lines\n", mcc_debug_prefix("coupling_capa_down.dat"));
  fprintf(file1,"pause -1 'Hit CR to finish'\n");
  fprintf(file1,"plot '%s' using 1:5 title 'cintp' with lines\n", mcc_debug_prefix("coupling_capa_down.dat"));
  fprintf(file1,"pause -1 'Hit CR to finish'\n");
  fprintf(file1,"plot '%s' using 1:6 title 'sumcapa' with lines,\\\n", mcc_debug_prefix("coupling_capa_down.dat"));
  fprintf(file1,"'%s' using 1:7 title 'capai' with lines\n", mcc_debug_prefix("coupling_capa_down.dat"));
  fprintf(file1,"pause -1 'Hit CR to finish'\n");
  fclose(file1) ;

}


/****************************************************************************\
 * Function : mcc_calc_jct_capa
 *
 * Drain  : cbd = cds (surfacique) + cdp (perimetrique)
 * Source : cbs = css (surfacique) + csp (perimetrique)
 *
 * Les transistors sont bloques, seul Vdrain varie de +vdd a -vdd
\****************************************************************************/
void mcc_calc_jct_capa (elp_lotrs_param *lotrsparam_n, elp_lotrs_param *lotrsparam_p)
{
  // TODO prevoir les tensions de bulk des lotrs
  FILE *file1,*file2,*file3,*file4;
  int i,stepmin,stepmax;
  double vbx;
  double cbdn,cbsn;
  double cbdp,cbsp;
  double cgp,cdw,cds,cdp;
  double Wcjp,Wcjn;
  double xwn,dwcjn,xwp,dwcjp,dwcn,dwcp;
  double Wan,Wap,ptQov;
  double vgx = MCC_VDDmax/2.0;

  mcc_get_area_perim();

  xwn = mcc_getXW (MCC_MODELFILE, MCC_TNMODEL,
                   MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6,
                   MCC_WN * 1.0e-6);
  dwcjn = mcc_calcDWCJ (MCC_MODELFILE, MCC_TNMODEL,
                        MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6,
                        MCC_WN * 1.0e-6,lotrsparam_n, MCC_TEMP) ;
  dwcn = mcc_calcDWC (MCC_MODELFILE, MCC_TNMODEL,
                      MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6,
                      MCC_WN * 1.0e-6,lotrsparam_n) ;

  xwp = mcc_getXW (MCC_MODELFILE, MCC_TPMODEL,
                   MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6,
                   MCC_WP * 1.0e-6);

  dwcjp = mcc_calcDWCJ(MCC_MODELFILE, MCC_TPMODEL,
                       MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6,
                       MCC_WP * 1.0e-6,lotrsparam_p, MCC_TEMP);

  dwcp = mcc_calcDWC (MCC_MODELFILE, MCC_TPMODEL,
                      MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6,
                      MCC_WP * 1.0e-6,lotrsparam_p);

  Wcjn = MCC_WN*1.0e-6*xwn+dwcjn;
  Wan = MCC_WN*1.0e-6*xwn+dwcn;

  Wcjp = MCC_WP*1.0e-6*xwp+dwcjp;
  Wap = MCC_WP*1.0e-6*xwp+dwcp;

  stepmax = mcc_ftoi((MCC_VDDmax) / (MCC_DC_STEP)) ;
  stepmin = -stepmax ;

  // NMOS
  file1 = mbkfopen(mcc_debug_prefix("cbdn"),"dat",WRITE_TEXT) ;
  file2 = mbkfopen(mcc_debug_prefix("cbsn"),"dat",WRITE_TEXT) ;
  file3 = mbkfopen(mcc_debug_prefix("cgpn"),"dat",WRITE_TEXT) ;
  file4 = mbkfopen(mcc_debug_prefix("q_nmos"),"dat",WRITE_TEXT) ;

  avt_printExecInfo(file1, "#", "", "");
  avt_printExecInfo(file2, "#", "", "");
  avt_printExecInfo(file3, "#", "", "");
  avt_printExecInfo(file4, "#", "", "");
  fprintf(file1,"# vbx cbdn \n");
  fprintf(file2,"# vbx cbsn \n");
  fprintf(file3,"# vbx cgpn Qov\n");
  fprintf(file4,"# vbx q1=cbdn*vbx q2=cgpn*vbx q3=(cbdn+cgpn)*vbx q4=fabs(q3)\n");

  for(i = stepmin ; i <= stepmax ; i++)
   {
    vbx = i*MCC_DC_STEP;
    vgx = vbx;
    fprintf(file1,"%.4g",vbx) ;
    fprintf(file2,"%.4g",vbx) ;
    fprintf(file3,"%.4g",vbx) ;
    fprintf(file4,"%.4g",vbx) ;

    //---------- Drain ------------------------------------
    
    cdw = mcc_calcCDW(MCC_MODELFILE,MCC_TNMODEL,MCC_NMOS,MCC_NCASE,
                       MCC_LN*1.0e-6, MCC_WN*1.0e-6,MCC_TEMP,
                       0.0,vbx,vgx,lotrsparam_n);
    cgp = mcc_calcCGP (MCC_MODELFILE, MCC_TNMODEL,
                        MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, 
                        MCC_WN * 1.0e-6, vgx, &ptQov,lotrsparam_n, MCC_TEMP);

    cds = mcc_calcCDS(MCC_MODELFILE, MCC_TNMODEL, 
                       MCC_NMOS, MCC_NCASE, MCC_LN*1.0e-6, MCC_WN*1.0e-6,
                       MCC_TEMP, 0.0,vbx,lotrsparam_n);
    cds *=  MCC_ADN*1.0e-6;
    cdp = mcc_calcCDP(MCC_MODELFILE, MCC_TNMODEL, 
                       MCC_NMOS, MCC_NCASE, MCC_LN*1.0e-6, MCC_WN*1.0e-6,
                       MCC_TEMP, 0.0,vbx, lotrsparam_n) ;
    cdp *= MCC_PDN*1.0e-6;

    cbdn = cds+cdp+(cdw-cgp)*Wcjn;

    //---------- Source ------------------------------------
    cdw = mcc_calcCDW(MCC_MODELFILE,MCC_TNMODEL,MCC_NMOS,MCC_NCASE,
                       MCC_LN*1.0e-6, MCC_WN*1.0e-6,MCC_TEMP,
                       0.0,vbx,vgx,lotrsparam_n);
    cgp = mcc_calcCGP (MCC_MODELFILE, MCC_TNMODEL,
                        MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, 
                        MCC_WN * 1.0e-6, vgx, NULL,lotrsparam_n, MCC_TEMP);

    cds = mcc_calcCDS(MCC_MODELFILE, MCC_TNMODEL, 
                       MCC_NMOS, MCC_NCASE, MCC_LN*1.0e-6, MCC_WN*1.0e-6,
                       MCC_TEMP, 0.0,vbx,lotrsparam_n);
    cds *= MCC_ASN*1.0e-6;
    cdp = mcc_calcCDP(MCC_MODELFILE, MCC_TNMODEL, 
                       MCC_NMOS, MCC_NCASE, MCC_LN*1.0e-6, MCC_WN*1.0e-6,
                       MCC_TEMP, 0.0,vbx, lotrsparam_n);
    cdp *= MCC_PSN*1.0e-6;

    cbsn = cds+cdp+(cdw-cgp)*Wcjn;

    fprintf(file1," %g\n",cbdn) ;
    fprintf(file2," %g\n",cbsn) ;
    fprintf(file3," %g %g\n",cgp*Wcjn,ptQov*vbx*Wcjn) ;
    fprintf(file4," %g %g %g %g\n",cbdn*vbx,cgp*Wcjn*vbx,(cbdn+cgp*Wcjn)*vbx,fabs((cbdn+cgp*Wcjn)*vbx)) ;
   }
  fclose(file1) ;
  fclose(file2) ;
  fclose(file3) ;
  fclose(file4) ;

  // PMOS
  file1 = mbkfopen(mcc_debug_prefix("cbdp"),"dat",WRITE_TEXT) ;
  file2 = mbkfopen(mcc_debug_prefix("cbsp"),"dat",WRITE_TEXT) ;
  file3 = mbkfopen(mcc_debug_prefix("cgpp"),"dat",WRITE_TEXT) ;
  file4 = mbkfopen(mcc_debug_prefix("q_pmos"),"dat",WRITE_TEXT) ;

  avt_printExecInfo(file1, "#", "", "");
  avt_printExecInfo(file2, "#", "", "");
  avt_printExecInfo(file3, "#", "", "");
  avt_printExecInfo(file4, "#", "", "");
  fprintf(file1,"# vbx cbdp \n");
  fprintf(file2,"# vbx cbsp \n");
  fprintf(file3,"# vbx cgpp Qov\n");
  fprintf(file4,"# vbx q1=cbdp*vbx q2=cgpp*vbx q3=(cbdp+cgpp)*vbx q4=fabs(q3)\n");

  for(i = stepmin ; i <= stepmax ; i++)
   {
    vbx = i*MCC_DC_STEP;
    vgx=vbx;
    fprintf(file1,"%.4g",vbx) ;
    fprintf(file2,"%.4g",vbx) ;
    fprintf(file3,"%.4g",vbx) ;
    fprintf(file4,"%.4g",vbx) ;

    //---------- Drain -----------------------------------
    
    cdw = mcc_calcCDW(MCC_MODELFILE,MCC_TPMODEL,MCC_PMOS,MCC_PCASE,
                       MCC_LP*1.0e-6, MCC_WP*1.0e-6,MCC_TEMP,
                       0.0,vbx,vgx,lotrsparam_p);
    cgp = mcc_calcCGP (MCC_MODELFILE, MCC_TPMODEL,
                        MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, 
                        MCC_WP * 1.0e-6,vgx, &ptQov,lotrsparam_p, MCC_TEMP);

    cds = mcc_calcCDS(MCC_MODELFILE, MCC_TPMODEL, 
                       MCC_PMOS, MCC_PCASE, MCC_LP*1.0e-6, MCC_WP*1.0e-6,
                       MCC_TEMP, 0.0,vbx,lotrsparam_p);
    cds *= MCC_ADP*1.0e-6;

    cdp =  mcc_calcCDP(MCC_MODELFILE, MCC_TPMODEL, 
                       MCC_PMOS, MCC_PCASE, MCC_LP*1.0e-6, MCC_WP*1.0e-6,
                       MCC_TEMP, 0.0,vbx, lotrsparam_p);
    cdp *= MCC_PDP*1.0e-6;

    cbdp = cds+cdp+(cdw-cgp)*Wcjp;

    //---------- Source ------------------------------------
    cdw = mcc_calcCDW(MCC_MODELFILE,MCC_TPMODEL,MCC_PMOS,MCC_PCASE,
                       MCC_LP*1.0e-6, MCC_WP*1.0e-6,MCC_TEMP,
                       0.0,vbx,vgx,lotrsparam_p);
    cgp = mcc_calcCGP (MCC_MODELFILE, MCC_TPMODEL,
                        MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, 
                        MCC_WP * 1.0e-6, vgx, NULL,lotrsparam_p, MCC_TEMP);
    cds = mcc_calcCDS(MCC_MODELFILE, MCC_TPMODEL, 
                       MCC_PMOS, MCC_PCASE, MCC_LP*1.0e-6, MCC_WP*1.0e-6,
                       MCC_TEMP, 0.0,vbx,lotrsparam_p);
    cds *= MCC_ASP*1.0e-6;

    cdp = mcc_calcCDP(MCC_MODELFILE, MCC_TPMODEL, 
                       MCC_PMOS, MCC_PCASE, MCC_LP*1.0e-6, MCC_WP*1.0e-6,
                       MCC_TEMP, 0.0,vbx, lotrsparam_p);
    cdp *= MCC_PSP*1.0e-6;

    cbsp = cds+cdp+ (cdw-cgp)*Wcjp;

    fprintf(file1," %g\n",cbdp) ;
    fprintf(file2," %g\n",cbsp) ;
    fprintf(file3," %g %g\n",cgp*Wcjp,ptQov*Wcjp*vbx) ;
    fprintf(file4," %g %g %g %g\n",cbdp*vbx,cgp*Wcjp*vbx,(cbdp+cgp*Wcjp)*vbx,fabs((cbdp+cgp*Wcjp)*vbx)) ;
   }
  fclose(file1) ;
  fclose(file2) ;
  fclose(file3) ;
  fclose(file4) ;

  // mcc up & down charges
  file1 = mbkfopen(mcc_debug_prefix("q_mcc_nmos"),"dat",WRITE_TEXT) ;
  file2 = mbkfopen(mcc_debug_prefix("q_mcc_pmos"),"dat",WRITE_TEXT) ;

  avt_printExecInfo(file1, "#", "", "");
  avt_printExecInfo(file2, "#", "", "");
  fprintf(file1,"# vbd qdsn_u qdpn_u qdwn_u qdnTOT_u qdsn+d qdpn_d qdwn_d cdnTOT_d\n");
  fprintf(file2,"# vbd qdsp_u qdpp_u qdwp_u qdpTOT_u qdsp+d qdpp_d qdwp_d cdpTOT_d\n");
  for(i = stepmin ; i <= stepmax; i++)
   {
    vbx = i*MCC_DC_STEP;
    fprintf(file1,"%g %g %g %g %g %g %g %g %g\n",vbx,MCC_ADN*1.0e-6*MCC_CDS_U_N*vbx,
                                                     MCC_PDN*1.0e-12*MCC_CDP_U_N*vbx,
                                                     Wcjn*1.0e-6*MCC_CDW_U_N*vbx,
                                                     (MCC_ADN*1.0e-6*MCC_CDS_U_N+
                                                      MCC_PDN*1.0e-12*MCC_CDP_U_N+
                                                      Wcjn*1.0e-6*MCC_CDW_U_N)*vbx,
                                                     MCC_ADN*1.0e-6*MCC_CDS_D_N*vbx,
                                                     MCC_PDN*1.0e-12*MCC_CDP_D_N*vbx,
                                                     Wcjn*1.0e-6*MCC_CDW_D_N*vbx,
                                                     (MCC_ADN*1.0e-6*MCC_CDS_D_N+
                                                      MCC_PDN*1.0e-12*MCC_CDP_D_N+
                                                      Wcjn*1.0e-6*MCC_CDW_D_N)*vbx) ;

    fprintf(file2,"%g %g %g %g %g %g %g %g %g\n",vbx,MCC_ADP*1.0e-6*MCC_CDS_U_P*vbx,
                                                     MCC_PDP*1.0e-12*MCC_CDP_U_P*vbx,
                                                     Wcjp*1.0e-6*MCC_CDW_U_P*vbx,
                                                     (MCC_ADP*1.0e-6*MCC_CDS_U_P+
                                                      MCC_PDP*1.0e-12*MCC_CDP_U_P+
                                                      Wcjp*1.0e-6*MCC_CDW_U_P)*vbx,
                                                     MCC_ADP*1.0e-6*MCC_CDS_D_P*vbx,
                                                     MCC_PDP*1.0e-12*MCC_CDP_D_P*vbx,
                                                     Wcjp*1.0e-6*MCC_CDW_D_P*vbx,
                                                     (MCC_ADP*1.0e-6*MCC_CDS_D_P+
                                                      MCC_PDP*1.0e-12*MCC_CDP_D_P+
                                                      Wcjp*1.0e-6*MCC_CDW_D_P)*vbx) ;
   }
  fclose(file1) ;
  fclose(file2) ;

}

/****************************************************************************\
 * Function : mcc_calcul_rapisat_fqmulu0
\****************************************************************************/
void mcc_calcul_rapisat_fqmulu0 (lotrsparam_n,lotrsparam_p)
elp_lotrs_param *lotrsparam_n;
elp_lotrs_param *lotrsparam_p;
{
 mcc_modellist *ptmodeln,*ptmodelp;
 double orig_muluon,orig_muluop;
 double ic1,ic2 ;
 char mode ;
 FILE *file ;
 int stepmax ;
 int stepmin ;
 int i,j,k;
 double vgs[2];
 double mulu0_1,mulu0_2;
 double max = 1.1,
        min = 0.9;
 elp_lotrs_param *trsparam;

 vgs[0] = MCC_VDDmax/2.0;
 vgs[1] = MCC_VDDmax;
 mode = MCC_CALC_CUR ;
 MCC_CALC_CUR = MCC_CALC_MODE ;

 ptmodeln = mcc_getmodel ( MCC_MODELFILE,
                           MCC_TNMODEL,
                           MCC_NMOS,
                           MCC_NCASE,
                           MCC_LN*1.0e-6,
                           MCC_WN*1.0e-6, 0);
 ptmodelp = mcc_getmodel ( MCC_MODELFILE,
                           MCC_TPMODEL,
                           MCC_PMOS,
                           MCC_PCASE,
                           MCC_LP*1.0e-6,
                           MCC_WP*1.0e-6, 0);

 if ( !ptmodeln || !ptmodelp ) return;

 orig_muluon = mcc_getparam_quick(ptmodeln,__MCC_QUICK_MULU0);
 orig_muluop = mcc_getparam_quick(ptmodelp,__MCC_QUICK_MULU0);

 stepmax = mcc_ftoi((MCC_VDDmax) / (MCC_DC_STEP)) ;
 stepmin = 0 ;

 file = mbkfopen(mcc_debug_prefix("rapisat_fqmulu0"),"dat",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");

 fprintf(file,"# mulu0_1/mulu_0_2 rap_isatN rap_isatP\n");

 mulu0_1 = 1.0;
 for(mulu0_2 = max ; mulu0_2 >= min ; mulu0_2 -= 0.01)
  {
   fprintf(file,"%g",mulu0_2/mulu0_1) ;

   for(j = 0 ; j < 2 ; j++)
    {
     for(k = 0 ; k < 2 ; k++)
      {
       if ( j == MCC_TRANS_N ) {
         trsparam = lotrsparam_n;
         mcc_addparam (ptmodeln,"MULU0",mulu0_1,MCC_SETVALUE);
         ic1 = mcc_spicecurrent(vgs[k],MCC_VDDmax,0.0,j,trsparam) ;
         mcc_addparam (ptmodeln,"MULU0",mulu0_2,MCC_SETVALUE);
         ic2 = mcc_spicecurrent(vgs[k],MCC_VDDmax,0.0,j,trsparam) ;
       }
       else {
         trsparam = lotrsparam_p;
         mcc_addparam (ptmodelp,"MULU0",mulu0_1,MCC_SETVALUE);
         ic1 = mcc_spicecurrent(vgs[k],MCC_VDDmax,0.0,j,trsparam) ;
         mcc_addparam (ptmodelp,"MULU0",mulu0_2,MCC_SETVALUE);
         ic2 = mcc_spicecurrent(vgs[k],MCC_VDDmax,0.0,j,trsparam) ;
       }
       fprintf(file," %g",ic2/ic1) ;
      }
    }
    fprintf(file,"\n") ;
  }

 fclose(file) ;

 file = mbkfopen(mcc_debug_prefix("rapisat_fqmulu0"),"plt",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");
 for(i = 0 ; i < 2 ; i++)
  {
   fprintf(file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
   fprintf(file,"set xlabel \"QMULU\"\n") ;
   fprintf(file,"set ylabel \"QIDS (A)\"\n") ;
   fprintf(file,"set xrange [%g:%g]\n ",min,max);
   fprintf(file,"plot ") ;
   for(k = 1 ; k < 3 ; k++)
    {
     fprintf(file,"'%s' using 1:%d title 'rap Isat %c vgs=%g' with lines %c",
                  mcc_debug_prefix("rapisat_fqmulu0.dat"), i*2+k+1,
                  (i==0? 'N':'P'),
                  vgs[k-1],
                  (k == 2)?'\n':',') ;
    }
   fprintf(file,"pause -1 'Hit CR to finish'\n") ;
  }
 mcc_addparam (ptmodeln,"MULU0",orig_muluon,MCC_SETVALUE);
 mcc_addparam (ptmodelp,"MULU0",orig_muluop,MCC_SETVALUE);

 MCC_CALC_CUR = mode ;
 fclose(file) ;
}

/****************************************************************************\
 * Function : mcc_calcul_rapisat_fdmulu0
\****************************************************************************/
void mcc_calcul_rapisat_fdmulu0 (lotrsparam_n,lotrsparam_p)
elp_lotrs_param *lotrsparam_n;
elp_lotrs_param *lotrsparam_p;
{
 mcc_modellist *ptmodeln,*ptmodelp;
 double orig_muluon,orig_muluop;
 double ic1,ic2 ;
 double mulu_factor[3] ;
 double factor[3] ;
 char mode ;
 FILE *file ;
 int stepmax ;
 int stepmin ;
 int i,j,k ;
 double mulu0_1,mulu0_2;
 elp_lotrs_param *trsparam;

 mulu_factor[0] = 0.95 ;
 mulu_factor[1] = 1.0 ;
 mulu_factor[2] = 1.05;
 factor[0] = 0.9 ;
 factor[1] = 1.0 ;
 factor[2] = 1.11;

 mode = MCC_CALC_CUR ;
 MCC_CALC_CUR = MCC_CALC_MODE ;

 ptmodeln = mcc_getmodel ( MCC_MODELFILE,
                           MCC_TNMODEL,
                           MCC_NMOS,
                           MCC_NCASE,
                           MCC_LN*1.0e-6,
                           MCC_WN*1.0e-6, 0);
 ptmodelp = mcc_getmodel ( MCC_MODELFILE,
                           MCC_TPMODEL,
                           MCC_PMOS,
                           MCC_PCASE,
                           MCC_LP*1.0e-6,
                           MCC_WP*1.0e-6, 0);

 if ( !ptmodeln || !ptmodelp ) return;

 orig_muluon = mcc_getparam_quick(ptmodeln,__MCC_QUICK_MULU0);
 orig_muluop = mcc_getparam_quick(ptmodelp,__MCC_QUICK_MULU0);

 stepmax = mcc_ftoi((MCC_VDDmax) / (MCC_DC_STEP)) ;
 stepmin = 0 ;

 file = mbkfopen(mcc_debug_prefix("rapisat_fdmulu0"),"dat",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");

 fprintf(file,"# vgs dmulu1n dmulu2n dmulu3n dmulu1p dmulu2p dmulu3p\n");

 for(i = stepmin ; i <= stepmax ; i++)
  {
   fprintf(file,"%g",i*MCC_DC_STEP) ;

   for(j = 0 ; j < 2 ; j++)
     {
      for(k = 0 ; k < 2 ; k++)
       {
        mulu0_1 = mulu_factor[k];
        mulu0_2 = mulu_factor[k+1];
        if ( j == MCC_TRANS_N ) {
          trsparam = lotrsparam_n;
          mcc_addparam (ptmodeln,"MULU0",mulu0_1,MCC_SETVALUE);
          ic1 = mcc_spicecurrent(i*MCC_DC_STEP,MCC_VDDmax,0.0,j,trsparam) ;
          mcc_addparam (ptmodeln,"MULU0",mulu0_2,MCC_SETVALUE);
          ic2 = mcc_spicecurrent(i*MCC_DC_STEP,MCC_VDDmax,0.0,j,trsparam) ;
        }
        else {
          trsparam = lotrsparam_p;
          mcc_addparam (ptmodelp,"MULU0",mulu0_1,MCC_SETVALUE);
          ic1 = mcc_spicecurrent(i*MCC_DC_STEP,MCC_VDDmax,0.0,j,trsparam) ;
          mcc_addparam (ptmodelp,"MULU0",mulu0_2,MCC_SETVALUE);
          ic2 = mcc_spicecurrent(i*MCC_DC_STEP,MCC_VDDmax,0.0,j,trsparam) ;
        }
        fprintf(file," %g",ic2/ic1) ;
       }
      for(k = 0 ; k < 2 ; k++)
       {
        mulu0_1 = factor[k];
        mulu0_2 = factor[k+1];
        if ( j == MCC_TRANS_N ) {
          trsparam = lotrsparam_n;
          mcc_addparam (ptmodeln,"MULU0",mulu0_1,MCC_SETVALUE);
          ic1 = mcc_spicecurrent(i*MCC_DC_STEP,MCC_VDDmax,0.0,j,trsparam) ;
          mcc_addparam (ptmodeln,"MULU0",mulu0_2,MCC_SETVALUE);
          ic2 = mcc_spicecurrent(i*MCC_DC_STEP,MCC_VDDmax,0.0,j,trsparam) ;
        }
        else {
          trsparam = lotrsparam_p;
          mcc_addparam (ptmodelp,"MULU0",mulu0_1,MCC_SETVALUE);
          ic1 = mcc_spicecurrent(i*MCC_DC_STEP,MCC_VDDmax,0.0,j,trsparam) ;
          mcc_addparam (ptmodelp,"MULU0",mulu0_2,MCC_SETVALUE);
          ic2 = mcc_spicecurrent(i*MCC_DC_STEP,MCC_VDDmax,0.0,j,trsparam) ;
        }
        fprintf(file," %g",ic2/ic1) ;
       }
     }
   fprintf(file,"\n") ;
  }

 fclose(file) ;

 file = mbkfopen(mcc_debug_prefix("rapisat_fdmulu0"),"plt",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");

 for(i = 0 ; i < 2 ; i++)
  {
   fprintf(file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
   fprintf(file,"set xlabel \"VGS (V)\"\n") ;
   fprintf(file,"set ylabel \"IDS (A)\"\n") ;
   fprintf(file,"set key top left\n") ;
   fprintf(file,"set xrange [0.0:%g]\n", MCC_VDDmax*1.05);
   fprintf(file,"plot ") ;
   for(k = 1 ; k < 3 ; k++)
    {
     fprintf(file,"'%s' using 1:%d title 'spice mulu0_1=%g mulu0_2=%g' with lines%c",
                  mcc_debug_prefix("rapisat_fdmulu0.dat"), k+1+i*4,
                  mulu_factor[k-1],
                  mulu_factor[k],
                  ',') ;
    }
   for(k = 1 ; k < 3 ; k++)
    {
     fprintf(file,"'%s' using 1:%d title 'spice mulu0_1=%g mulu0_2=%g' with lines%c",
                  mcc_debug_prefix("rapisat_fdmulu0.dat"), k+3+i*4,
                  factor[k-1],
                  factor[k],
                  (k == 2)?'\n':',') ;
    }
   fprintf(file,"pause -1 'Hit CR to finish'\n") ;
  }

 file = mbkfopen(mcc_debug_prefix("rapires_fdmulu0"),"dat",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");

 fprintf(file,"# vds dmulu1n dmulu2n dmulu3n dmulu1p dmulu2p dmulu3p\n");

 stepmin = 0;
 for(i = stepmin ; i <= stepmax ; i++)
  {
   fprintf(file,"%g",i*MCC_DC_STEP) ;

   for(j = 0 ; j < 2 ; j++)
     {
      for(k = 0 ; k < 2 ; k++)
       {
        mulu0_1 = mulu_factor[k];
        mulu0_2 = mulu_factor[k+1];
        if ( j == MCC_TRANS_N ) {
          trsparam = lotrsparam_n;
          mcc_addparam (ptmodeln,"MULU0",mulu0_1,MCC_SETVALUE);
          ic1 = mcc_spicecurrent(MCC_VDDmax,i*MCC_DC_STEP,0.0,j,trsparam) ;
          mcc_addparam (ptmodeln,"MULU0",mulu0_2,MCC_SETVALUE);
          ic2 = mcc_spicecurrent(MCC_VDDmax,i*MCC_DC_STEP,0.0,j,trsparam) ;
        }
        else {
          trsparam = lotrsparam_p;
          mcc_addparam (ptmodelp,"MULU0",mulu0_1,MCC_SETVALUE);
          ic1 = mcc_spicecurrent(MCC_VDDmax,i*MCC_DC_STEP,0.0,j,trsparam) ;
          mcc_addparam (ptmodelp,"MULU0",mulu0_2,MCC_SETVALUE);
          ic2 = mcc_spicecurrent(MCC_VDDmax,i*MCC_DC_STEP,0.0,j,trsparam) ;
        }
        fprintf(file," %g",ic2/ic1) ;
       }
      for(k = 0 ; k < 2 ; k++)
       {
        mulu0_1 = factor[k];
        mulu0_2 = factor[k+1];
        if ( j == MCC_TRANS_N ) {
          trsparam = lotrsparam_n;
          mcc_addparam (ptmodeln,"MULU0",mulu0_1,MCC_SETVALUE);
          ic1 = mcc_spicecurrent(MCC_VDDmax,i*MCC_DC_STEP,0.0,j,trsparam) ;
          mcc_addparam (ptmodeln,"MULU0",mulu0_2,MCC_SETVALUE);
          ic2 = mcc_spicecurrent(MCC_VDDmax,i*MCC_DC_STEP,0.0,j,trsparam) ;
        }
        else {
          trsparam = lotrsparam_p;
          mcc_addparam (ptmodelp,"MULU0",mulu0_1,MCC_SETVALUE);
          ic1 = mcc_spicecurrent(MCC_VDDmax,i*MCC_DC_STEP,0.0,j,trsparam) ;
          mcc_addparam (ptmodelp,"MULU0",mulu0_2,MCC_SETVALUE);
          ic2 = mcc_spicecurrent(MCC_VDDmax,i*MCC_DC_STEP,0.0,j,trsparam) ;
        }
        fprintf(file," %g",ic2/ic1) ;
       }
     }
   fprintf(file,"\n") ;
  }

 fclose(file) ;

 file = mbkfopen(mcc_debug_prefix("rapires_fdmulu0"),"plt",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");

 for(i = 0 ; i < 2 ; i++)
  {
   fprintf(file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
   fprintf(file,"set xlabel \"VDS (V)\"\n") ;
   fprintf(file,"set ylabel \"IDS (A)\"\n") ;
   fprintf(file,"set key top left\n") ;
   fprintf(file,"set xrange [0.0:%g]\n", MCC_VDDmax*1.05);
   fprintf(file,"plot ") ;
   for(k = 1 ; k < 3 ; k++)
    {
     fprintf(file,"'%s' using 1:%d title 'spice mulu0_1=%g mulu0_2=%g' with lines%c",
                  mcc_debug_prefix("rapires_fdmulu0.dat"), k+1+i*4,
                  mulu_factor[k-1],
                  mulu_factor[k],
                  ',') ;
    }
   for(k = 1 ; k < 3 ; k++)
    {
     fprintf(file,"'%s' using 1:%d title 'spice mulu0_1=%g mulu0_2=%g' with lines%c",
                  mcc_debug_prefix("rapires_fdmulu0.dat"), k+3+i*4,
                  factor[k-1],
                  factor[k],
                  (k == 2)?'\n':',') ;
    }
   fprintf(file,"pause -1 'Hit CR to finish'\n") ;
  }

 mcc_addparam (ptmodeln,"MULU0",orig_muluon,MCC_SETVALUE);
 mcc_addparam (ptmodelp,"MULU0",orig_muluop,MCC_SETVALUE);

 MCC_CALC_CUR = mode ;
 fclose(file) ;
}

/****************************************************************************\
 * Function : mcc_calcul_isat_fmulu0
\****************************************************************************/
void mcc_calcul_isat_fmulu0 (lotrsparam_n,lotrsparam_p)
elp_lotrs_param *lotrsparam_n;
elp_lotrs_param *lotrsparam_p;
{
 mcc_modellist *ptmodeln,*ptmodelp;
 double orig_muluon,orig_muluop;
 double ic ;
 double mulu_factor[5] ;
 char mode ;
 FILE *file ;
 int stepmax ;
 int stepmin ;
 int i,j,k ;
 double mulu0,orig_a;
 elp_lotrs_param *trsparam;

 /*
 mulu_factor[0] = 0.8 ;
 mulu_factor[1] = 0.9 ;
 mulu_factor[2] = 1.0 ;
 mulu_factor[3] = 1.1 ;
 mulu_factor[4] = 1.2 ;
 */

 mulu_factor[0] = 0.9 ;
 mulu_factor[1] = 0.95;
 mulu_factor[2] = 1.0 ;
 mulu_factor[3] = 1.05;
 mulu_factor[4] = 1.1 ;

 mode = MCC_CALC_CUR ;
 MCC_CALC_CUR = MCC_CALC_MODE ;

 ptmodeln = mcc_getmodel ( MCC_MODELFILE,
                           MCC_TNMODEL,
                           MCC_NMOS,
                           MCC_NCASE,
                           MCC_LN*1.0e-6,
                           MCC_WN*1.0e-6, 0);
 ptmodelp = mcc_getmodel ( MCC_MODELFILE,
                           MCC_TPMODEL,
                           MCC_PMOS,
                           MCC_PCASE,
                           MCC_LP*1.0e-6,
                           MCC_WP*1.0e-6, 0);

 if ( !ptmodeln || !ptmodelp ) return;

 orig_muluon = mcc_getparam_quick(ptmodeln,__MCC_QUICK_MULU0);
 orig_muluop = mcc_getparam_quick(ptmodelp,__MCC_QUICK_MULU0);

 stepmax = mcc_ftoi((MCC_VDDmax) / (MCC_DC_STEP)) ;
 stepmin = 0 ;

 file = mbkfopen(mcc_debug_prefix("isat_fmulu0"),"dat",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");

 fprintf(file,"# vgs mulu1n mulu2n mulu3n mulu4n mulu5n mulu1p mulu2p mulu3p mulu4p mulu5p \n") ;

 for(i = stepmin ; i <= stepmax ; i++)
  {
   fprintf(file,"%g",i*MCC_DC_STEP) ;

   for(j = 0 ; j < 2 ; j++)
     {
      for(k = 0 ; k < 5 ; k++)
       {
        mulu0 = mulu_factor[k];
        if ( j == MCC_TRANS_N ) {
          trsparam = lotrsparam_n;
          mcc_addparam (ptmodeln,"MULU0",mulu0,MCC_SETVALUE);
        }
        else {
          trsparam = lotrsparam_p;
          mcc_addparam (ptmodelp,"MULU0",mulu0,MCC_SETVALUE);
        }
        ic = mcc_spicecurrent(i*MCC_DC_STEP,MCC_VDDmax,0.0,j,trsparam) ;
        fprintf(file," %g",ic) ;

        if ( j == MCC_TRANS_N ) {
          orig_a = MCC_AN;
          MCC_AN *= mulu0/orig_muluon;
        }
        else {
          orig_a = MCC_AP;
          MCC_AP *= mulu0/orig_muluop;
        }
        ic = mcc_current(i*MCC_DC_STEP,MCC_VDDmax,0.0,j) ;
        fprintf(file," %g",ic) ;
        if ( j == MCC_TRANS_N )
          MCC_AN = orig_a;
        else 
          MCC_AP = orig_a;
       }
     }
   fprintf(file,"\n") ;
  }

 fclose(file) ;

 file = mbkfopen(mcc_debug_prefix("isat_fmulu0"),"plt",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");

 for(i = 0 ; i < 2 ; i++)
  {
   fprintf(file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
   fprintf(file,"set xlabel \"VGS (V)\"\n") ;
   fprintf(file,"set ylabel \"IDS (A)\"\n") ;
   fprintf(file,"set key top left\n") ;
   fprintf(file,"set xrange [0.0:%g]\n", MCC_VDDmax*1.05);
   fprintf(file,"plot ") ;
   for(k = 1 ; k < 6 ; k++)
    {
      /*
     fprintf(file,"'isat_fmulu0.dat' using 1:%d title 'spice mulu0=%g' with lines%c",
                  2*k+i*10,
                  mulu_factor[k-1],
                  (k == 5)?'\n':',') ;
                  */
     fprintf(file,"'%s' using 1:%d title 'spice mulu0=%g' with lines, 'isat_fmulu0.dat' using 1:%d title 'mcc mulu0=%g ' with lines%c",
                  mcc_debug_prefix("isat_fmulu0.dat"), 2*k+i*10,
                  mulu_factor[k-1],
                  2*k+1+i*10,
                  mulu_factor[k-1],
                  (k == 5)?'\n':',') ;
    }
   fprintf(file,"pause -1 'Hit CR to finish'\n") ;
  }

 stepmin = 0 ;

 file = mbkfopen(mcc_debug_prefix("ires_fmulu0"),"dat",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");

 fprintf(file,"# vds mulu1n mulu2n mulu3n mulu4n mulu5n mulu1p mulu2p mulu3p mulu4p mulu5p \n") ;

 for(i = stepmin ; i <= stepmax ; i++)
  {
   fprintf(file,"%g",i*MCC_DC_STEP) ;

   for(j = 0 ; j < 2 ; j++)
     {
      for(k = 0 ; k < 5 ; k++)
       {
        mulu0 = mulu_factor[k];
        if ( j == MCC_TRANS_N ) {
          trsparam = lotrsparam_n;
          mcc_addparam (ptmodeln,"MULU0",mulu0,MCC_SETVALUE);
        }
        else {
          trsparam = lotrsparam_p;
          mcc_addparam (ptmodelp,"MULU0",mulu0,MCC_SETVALUE);
        }
        ic = mcc_spicecurrent(MCC_VDDmax,i*MCC_DC_STEP,0.0,j,trsparam) ;
        fprintf(file," %g",ic) ;

        if ( j == MCC_TRANS_N ) {
          orig_a = MCC_AN;
          MCC_AN *= mulu0/orig_muluon;
        }
        else {
          orig_a = MCC_AP;
          MCC_AP *= mulu0/orig_muluop;
        }
        ic = mcc_current(MCC_VDDmax,i*MCC_DC_STEP,0.0,j) ;
        fprintf(file," %g",ic) ;
        if ( j == MCC_TRANS_N )
          MCC_AN = orig_a;
        else 
          MCC_AP = orig_a;
       }
     }
   fprintf(file,"\n") ;
  }

 fclose(file) ;

 file = mbkfopen(mcc_debug_prefix("ires_fmulu0"),"plt",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");

 for(i = 0 ; i < 2 ; i++)
  {
   fprintf(file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
   fprintf(file,"set xlabel \"VDS (V)\"\n") ;
   fprintf(file,"set ylabel \"IDS (A)\"\n") ;
   fprintf(file,"set key top left\n") ;
   fprintf(file,"set xrange [0.0:%g]\n", MCC_VDDmax*1.05);
   fprintf(file,"plot ") ;
   for(k = 1 ; k < 6 ; k++)
    {
      /*
     fprintf(file,"'ires_fmulu0.dat' using 1:%d title 'spice mulu0=%g' with lines%c",
                  2*k+i*10,
                  mulu_factor[k-1],
                  (k == 5)?'\n':',') ;
                  */
     fprintf(file,"'%s' using 1:%d title 'spice mulu0=%g' with lines, 'ires_fmulu0.dat' using 1:%d title 'mcc mulu0=%g ' with lines%c",
                  mcc_debug_prefix("ires_fmulu0.dat"), 2*k+i*10,
                  mulu_factor[k-1],
                  2*k+1+i*10,
                  mulu_factor[k-1],
                  (k == 5)?'\n':',') ;
    }
   fprintf(file,"pause -1 'Hit CR to finish'\n") ;
  }

 mcc_addparam (ptmodeln,"MULU0",orig_muluon,MCC_SETVALUE);
 mcc_addparam (ptmodelp,"MULU0",orig_muluop,MCC_SETVALUE);

 MCC_CALC_CUR = mode ;
 fclose(file) ;
}

/****************************************************************************\
 * Function : mcc_calcul_isat_fdelvt0
\****************************************************************************/
void mcc_calcul_isat_fdelvt0 (lotrsparam_n,lotrsparam_p)
elp_lotrs_param *lotrsparam_n;
elp_lotrs_param *lotrsparam_p;
{
 mcc_modellist *ptmodeln,*ptmodelp;
 double orig_delvton,orig_delvtop;
 double ic ;
 double delvt_factor[5] ;
 char mode ;
 FILE *file ;
 int stepmax ;
 int stepmin ;
 int i,j,k ;
 double delvt0,orig_vt;
 elp_lotrs_param *trsparam;

 delvt_factor[0] =-0.5 ;
 delvt_factor[1] =-0.25 ;
 delvt_factor[2] = 0.0 ;
 delvt_factor[3] = 0.25 ;
 delvt_factor[4] = 0.5 ;

 mode = MCC_CALC_CUR ;
 MCC_CALC_CUR = MCC_CALC_MODE ;

 ptmodeln = mcc_getmodel ( MCC_MODELFILE,
                           MCC_TNMODEL,
                           MCC_NMOS,
                           MCC_NCASE,
                           MCC_LN*1.0e-6,
                           MCC_WN*1.0e-6, 0);
 ptmodelp = mcc_getmodel ( MCC_MODELFILE,
                           MCC_TPMODEL,
                           MCC_PMOS,
                           MCC_PCASE,
                           MCC_LP*1.0e-6,
                           MCC_WP*1.0e-6, 0);
 if ( !ptmodeln || !ptmodelp ) return;

 orig_delvton = mcc_getparam_quick(ptmodeln,__MCC_QUICK_DELVT0);
 orig_delvtop = mcc_getparam_quick(ptmodelp,__MCC_QUICK_DELVT0);

 stepmax = mcc_ftoi((MCC_VDDmax) / (MCC_DC_STEP)) ;
 stepmin = 0 ;

 file = mbkfopen("isat_fdelvt0","dat",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");

 fprintf(file,"# vgs delvt1n delvt2n delvt3n delvt4n delvt5n delvt1p delvt2p delvt3p delvt4p delvt5p \n") ;

 for(i = stepmin ; i <= stepmax ; i++)
  {
   fprintf(file,"%g",i*MCC_DC_STEP) ;

   for(j = 0 ; j < 2 ; j++)
     {
      for(k = 0 ; k < 5 ; k++)
       {
        if ( j == MCC_TRANS_N ) {
          trsparam = lotrsparam_n;
          delvt0 = MCC_VTN*delvt_factor[k];
          mcc_addparam (ptmodeln,"DELVT0",delvt0,MCC_SETVALUE);
        }
        else {
          trsparam = lotrsparam_p;
          delvt0 = MCC_VTP*delvt_factor[k];
          mcc_addparam (ptmodelp,"DELVT0",delvt0,MCC_SETVALUE);
        }
        ic = mcc_spicecurrent(i*MCC_DC_STEP,MCC_VDDmax,0.0,j,trsparam) ;
        fprintf(file," %g",ic) ;

        if ( j == MCC_TRANS_N ) {
          orig_vt = MCC_VTN;
          MCC_VTN += delvt0;
        }
        else {
          orig_vt = MCC_VTP;
          MCC_VTP -= delvt0;
        }
        ic = mcc_current(i*MCC_DC_STEP,MCC_VDDmax,0.0,j) ;
        fprintf(file," %g",ic) ;
        if ( j == MCC_TRANS_N )
          MCC_VTN = orig_vt;
        else 
          MCC_VTP = orig_vt;
       }
     }
   fprintf(file,"\n") ;
  }

 fclose(file) ;

 file = mbkfopen("isat_fdelvt0","plt",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");

 for(i = 0 ; i < 2 ; i++)
  {
   fprintf(file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
   fprintf(file,"set title \"%s\"\n", i==0?"NMOS":"PMOS");
   fprintf(file,"set xlabel \"VGS (V)\"\n") ;
   fprintf(file,"set ylabel \"IDS (A)\"\n") ;
   fprintf(file,"set key top left\n") ;
   fprintf(file,"set xrange [0.0:%g]\n", MCC_VDDmax*1.05);
   fprintf(file,"plot ") ;
   for(k = 1 ; k < 6 ; k++)
    {
     fprintf(file,"'isat_fdelvt0.dat' using 1:%d title 'spice vt*delvt0=%g' with lines, 'isat_fdelvt0.dat' using 1:%d title 'mcc vt*delvt0=%g ' with lines%c",
                  2*k+i*10,
                  delvt_factor[k-1],
                  2*k+1+i*10,
                  delvt_factor[k-1],
                  (k == 5)?'\n':',') ;
    }
   fprintf(file,"pause -1 'Hit CR to finish'\n") ;
  }

 mcc_addparam (ptmodeln,"DELVT0",orig_delvton,MCC_SETVALUE);
 mcc_addparam (ptmodelp,"DELVT0",orig_delvtop,MCC_SETVALUE);

 MCC_CALC_CUR = mode ;
 fclose(file) ;
}

/****************************************************************************\
 * Function : mcc_calcul_abr_fdelvt0
\****************************************************************************/
void mcc_calcul_abr_fdelvt0 (lotrsparam_n,lotrsparam_p)
elp_lotrs_param *lotrsparam_n;
elp_lotrs_param *lotrsparam_p;
{
 double delvt_factor[5] ;
 int i,j;
 char mode ;
 mcc_modellist *ptmodeln,*ptmodelp;
 double orig_delvton,orig_delvtop;
 FILE *file;

 delvt_factor[0] =-0.5 ;
 delvt_factor[1] =-0.25 ;
 delvt_factor[2] = 0.0 ;
 delvt_factor[3] = 0.25 ;
 delvt_factor[4] = 0.5 ;

 mode = MCC_CALC_CUR ;
 MCC_CALC_CUR = MCC_CALC_MODE ;

 ptmodeln = mcc_getmodel ( MCC_MODELFILE,
                           MCC_TNMODEL,
                           MCC_NMOS,
                           MCC_NCASE,
                           MCC_LN*1.0e-6,
                           MCC_WN*1.0e-6, 0);
 ptmodelp = mcc_getmodel ( MCC_MODELFILE,
                           MCC_TPMODEL,
                           MCC_PMOS,
                           MCC_PCASE,
                           MCC_LP*1.0e-6,
                           MCC_WP*1.0e-6, 0);

 if ( !ptmodeln || !ptmodelp ) return;

 orig_delvton = mcc_getparam_quick(ptmodeln,__MCC_QUICK_DELVT0);
 orig_delvtop = mcc_getparam_quick(ptmodelp,__MCC_QUICK_DELVT0);

 file = mbkfopen("abr_fdelvt0","dat",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");

 fprintf(file,"# delvt0 an bn rn ap bp rp\n") ;
 for ( j = 0 ; j < 5 ; j++ ) {
   fprintf (file,"%g ",delvt_factor[j]);
   mcc_addparam ( ptmodeln, "DELVT0", delvt_factor[j]*MCC_VTN, MCC_SETVALUE);
   mcc_addparam ( ptmodelp, "DELVT0", delvt_factor[j]*MCC_VTP, MCC_SETVALUE);
   for ( i = 0 ; i < 2 ; i++ ) {
     mcc_calcul_abr (i,-1.0,-1.0,lotrsparam_n,lotrsparam_p);
     if ( i == MCC_NMOS )
       fprintf ( file, "%g %g %g ",MCC_AN, MCC_BN, MCC_RNT);
     else
       fprintf ( file, "%g %g %g",MCC_AP, MCC_BP, MCC_RPT);
   }
   fprintf (file,"\n");
 }
 fclose(file) ;

 file = mbkfopen("abr_fdelvt0","plt",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");

 fprintf(file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
 fprintf(file,"set xlabel \"VT*DELVT0 (V)\"\n") ;
 fprintf(file,"set ylabel \"elp A\"\n") ;
 fprintf(file,"plot [-0.5:0.5] ") ;
 fprintf(file,"'abr_fdelvt0.dat' using 1:2 title 'an' with lines, 'abr_fdelvt0.dat' using 1:5 title 'ap' with lines\n");
 fprintf(file,"pause -1 'Hit CR to finish'\n") ;

 fprintf(file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
 fprintf(file,"set xlabel \"VT*DELVT0 (V)\"\n") ;
 fprintf(file,"set ylabel \"elp B\"\n") ;
 fprintf(file,"plot [-0.5:0.5] ") ;
 fprintf(file,"'abr_fdelvt0.dat' using 1:3 title 'bn' with lines, 'abr_fdelvt0.dat' using 1:6 title 'bp' with lines\n");
 fprintf(file,"pause -1 'Hit CR to finish'\n") ;

 fprintf(file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
 fprintf(file,"set xlabel \"VT*DELVT0 (V)\"\n") ;
 fprintf(file,"set ylabel \"elp param \"\n") ;
 fprintf(file,"plot [-0.5:0.5] ") ;
 fprintf(file,"'abr_fdelvt0.dat' using 1:4 title 'rn' with lines, 'abr_fdelvt0.dat' using 1:7 title 'rp' with lines\n");
 fprintf(file,"pause -1 'Hit CR to finish'\n") ;

 mcc_addparam (ptmodeln,"DELVT0",orig_delvton,MCC_SETVALUE);
 mcc_addparam (ptmodelp,"DELVT0",orig_delvtop,MCC_SETVALUE);

 MCC_CALC_CUR = mode ;
 fclose(file) ;
}

/****************************************************************************\
 * Function : mcc_calcul_abr_fmulu0
\****************************************************************************/
void mcc_calcul_abr_fmulu0 (lotrsparam_n,lotrsparam_p)
elp_lotrs_param *lotrsparam_n;
elp_lotrs_param *lotrsparam_p;
{
 double mulu_factor[5] ;
 double orig_muluon,orig_muluop;
 int i,j;
 char mode ;
 mcc_modellist *ptmodeln,*ptmodelp;
 FILE *file;

 mulu_factor[0] = 0.6 ;
 mulu_factor[1] = 0.8 ;
 mulu_factor[2] = 1.0 ;
 mulu_factor[3] = 1.2 ;
 mulu_factor[4] = 1.4 ;

 mode = MCC_CALC_CUR ;
 MCC_CALC_CUR = MCC_CALC_MODE ;

 ptmodeln = mcc_getmodel ( MCC_MODELFILE,
                           MCC_TNMODEL,
                           MCC_NMOS,
                           MCC_NCASE,
                           MCC_LN*1.0e-6,
                           MCC_WN*1.0e-6, 0);
 ptmodelp = mcc_getmodel ( MCC_MODELFILE,
                           MCC_TPMODEL,
                           MCC_PMOS,
                           MCC_PCASE,
                           MCC_LP*1.0e-6,
                           MCC_WP*1.0e-6, 0);

 if ( !ptmodeln || !ptmodelp ) return;

 orig_muluon = mcc_getparam_quick(ptmodeln,__MCC_QUICK_MULU0);
 orig_muluop = mcc_getparam_quick(ptmodelp,__MCC_QUICK_MULU0);

 file = mbkfopen("abr_fmulu0","dat",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");

 fprintf(file,"# mulu0 an bn rn ap bp rp\n") ;
 for ( j = 0 ; j < 5 ; j++ ) {
   fprintf (file,"%g ",mulu_factor[j]);
   mcc_addparam ( ptmodeln, "MULU0", mulu_factor[j], MCC_SETVALUE);
   mcc_addparam ( ptmodelp, "MULU0", mulu_factor[j], MCC_SETVALUE);
   for ( i = 0 ; i < 2 ; i++ ) {
     mcc_calcul_abr (i,-1.0,-1.0,lotrsparam_n,lotrsparam_p);
     if ( i == MCC_NMOS )
       fprintf ( file, "%g %g %g ",MCC_AN, MCC_BN, MCC_RNT);
     else
       fprintf ( file, "%g %g %g",MCC_AP, MCC_BP, MCC_RPT);
   }
   fprintf (file,"\n");
 }
 fclose(file) ;

 file = mbkfopen("abr_fmulu0","plt",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");

 fprintf(file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
 fprintf(file,"set xlabel \"MULU0 (V)\"\n") ;
 fprintf(file,"set ylabel \"elp A\"\n") ;
 fprintf(file,"plot [0.5:1.5] ") ;
 fprintf(file,"'abr_fmulu0.dat' using 1:2 title 'an' with lines, 'abr_fmulu0.dat' using 1:5 title 'ap' with lines\n");
 fprintf(file,"pause -1 'Hit CR to finish'\n") ;

 fprintf(file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
 fprintf(file,"set xlabel \"MULU0 (V)\"\n") ;
 fprintf(file,"set ylabel \"elp B\"\n") ;
 fprintf(file,"plot [0.5:1.5] ") ;
 fprintf(file,"'abr_fmulu0.dat' using 1:3 title 'bn' with lines, 'abr_fmulu0.dat' using 1:6 title 'bp' with lines\n");
 fprintf(file,"pause -1 'Hit CR to finish'\n") ;

 fprintf(file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
 fprintf(file,"set xlabel \"MULU0 (V)\"\n") ;
 fprintf(file,"set ylabel \"elp param \"\n") ;
 fprintf(file,"plot [0.5:1.5] ") ;
 fprintf(file,"'abr_fmulu0.dat' using 1:4 title 'rn' with lines, 'abr_fmulu0.dat' using 1:7 title 'rp' with lines\n");
 fprintf(file,"pause -1 'Hit CR to finish'\n") ;

 mcc_addparam (ptmodeln,"MULU0",orig_muluon,MCC_SETVALUE);
 mcc_addparam (ptmodelp,"MULU0",orig_muluop,MCC_SETVALUE);

 MCC_CALC_CUR = mode ;
 fclose(file) ;
}


/****************************************************************************\
 * Function : mcc_calcul_rsat
\****************************************************************************/
void mcc_calcul_rsat( elp_lotrs_param *lotrsparam_n, elp_lotrs_param *lotrsparam_p) 
{
 double imax ;
 double irs ;
 double rs ;
 double vrs ;
 char mode ;
 FILE *file ;
 int stepmax ;
 int step ;
 int i ;
 mcc_modellist *ptmodeln,*ptmodelp;
 char *name=mcc_debug_prefix("rsat");
 mode = MCC_CALC_CUR ;
 MCC_CALC_CUR = MCC_CALC_MODE ;

 ptmodeln = mcc_getmodel ( MCC_MODELFILE,
                           MCC_TNMODEL,
                           MCC_NMOS,
                           MCC_NCASE,
                           MCC_LN*1.0e-6,
                           MCC_WN*1.0e-6, 0);
 ptmodelp = mcc_getmodel ( MCC_MODELFILE,
                           MCC_TPMODEL,
                           MCC_PMOS,
                           MCC_PCASE,
                           MCC_LP*1.0e-6,
                           MCC_WP*1.0e-6, 0);
 stepmax = mcc_ftoi((MCC_VDDmax) / (MCC_DC_STEP)) ;

 file = mbkfopen(name,"dat",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");

 fprintf(file,"# vgs spicenrsat mccnrsat spiceprsat mccprsat\n") ;

 for(i = 0 ; i <= stepmax ; i++)
  {
   step = mcc_ftoi(((i*MCC_DC_STEP-MCC_VTN)/(MCC_VDDmax-MCC_VTN))*(MCC_VDDmax * MCC_KRNS) / MCC_DC_STEP) ;
   vrs =  MCC_DC_STEP * step ;
   imax = mcc_spicecurrent(i*MCC_DC_STEP,MCC_VDDmax,MCC_VBULKN,MCC_TRANS_N,lotrsparam_n) ;
   irs = mcc_spicecurrent(i*MCC_DC_STEP,vrs,MCC_VBULKN,MCC_TRANS_N,lotrsparam_n) ;
   rs = mcc_calcul_rs(MCC_VDDmax,imax,vrs,irs,1.0,1.0) ;
   fprintf(file,"%g %g",i*MCC_DC_STEP,rs) ;

   imax = mcc_current(i*MCC_DC_STEP,MCC_VDDmax,0.0,MCC_TRANS_N) ;
   irs = mcc_current(i*MCC_DC_STEP,vrs,0.0,MCC_TRANS_N) ;
   rs = mcc_calcul_rs(MCC_VDDmax,imax,vrs,irs,1.0,1.0) ;
   fprintf(file," %g",rs) ;

   vrs =  ((i*MCC_DC_STEP-MCC_VTP)/(MCC_VDDmax-MCC_VTP))*(MCC_VDDmax * MCC_KRPS) ;
   imax = mcc_spicecurrent(i*MCC_DC_STEP,MCC_VDDmax,MCC_VBULKP-MCC_VDDmax,MCC_TRANS_P,lotrsparam_p) ;
   irs = mcc_spicecurrent(i*MCC_DC_STEP,vrs,MCC_VBULKP-MCC_VDDmax,MCC_TRANS_P,lotrsparam_p) ;
   rs = mcc_calcul_rs(MCC_VDDmax,imax,vrs,irs,1.0,1.0) ;
   fprintf(file," %g",rs) ;

   imax = mcc_current(i*MCC_DC_STEP,MCC_VDDmax,0.0,MCC_TRANS_P) ;
   irs = mcc_current(i*MCC_DC_STEP,vrs,0.0,MCC_TRANS_P) ;
   rs = mcc_calcul_rs(MCC_VDDmax,imax,vrs,irs,1.0,1.0) ;
   fprintf(file," %g",rs) ;
   fprintf(file,"\n") ;
  }

 fclose(file) ;
 file = mbkfopen(name,"plt",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");

 for(i = 0 ; i < 2 ; i++)
  {
   double vt;

   if ( i == 0 )
     vt = MCC_VTN+(MCC_VDDmax-MCC_VTN)/3.0;
   else
     vt = MCC_VTP+(MCC_VDDmax-MCC_VTP)/3.0;
   fprintf(file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
   fprintf(file,"set title \"%s\"\n", i==0?"NMOS":"PMOS");
   fprintf(file,"set xlabel \"VGS (V)\"\n") ;
   fprintf(file,"set ylabel \"RSAT (O)\"\n") ;
   fprintf(file,"set xrange [%g:]\n", vt);
   fprintf(file,"plot '%s.dat' using 1:%d title '%s'  with lines, '%s.dat' using 1:%d title 'elp' with lines\n", name, 2+2*i, mcc_getmccname( i==0?ptmodeln:ptmodelp), name,3+2*i) ;
   fprintf(file,"pause -1 'Hit CR to finish'\n") ;
  }

 MCC_CALC_CUR = mode ;
 fclose(file) ;
}

/****************************************************************************\
 * Function : mcc_calcul_rlin
\****************************************************************************/
void mcc_calcul_rlin(lotrsparam_n,lotrsparam_p)
elp_lotrs_param *lotrsparam_n;
elp_lotrs_param *lotrsparam_p;
{
 double ir ;
 double r ;
 double vr ;
 char mode ;
 FILE *file ;
 int stepmax ;
 int step ;
 int i ;
 mcc_modellist *ptmodeln,*ptmodelp;
 char *name=mcc_debug_prefix("rlin");
 ptmodeln = mcc_getmodel ( MCC_MODELFILE,
                           MCC_TNMODEL,
                           MCC_NMOS,
                           MCC_NCASE,
                           MCC_LN*1.0e-6,
                           MCC_WN*1.0e-6, 0);
 ptmodelp = mcc_getmodel ( MCC_MODELFILE,
                           MCC_TPMODEL,
                           MCC_PMOS,
                           MCC_PCASE,
                           MCC_LP*1.0e-6,
                           MCC_WP*1.0e-6, 0);

 mode = MCC_CALC_CUR ;
 MCC_CALC_CUR = MCC_CALC_MODE ;

 stepmax = mcc_ftoi((MCC_VDDmax) / (MCC_DC_STEP)) ;

 file = mbkfopen(name,"dat",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");

 fprintf(file,"# vgs spicenrlin mccnrlin spiceprlin mccprlin\n") ;

 for(i = 0 ; i <= stepmax ; i++)
  {
   step = mcc_ftoi(((i*MCC_DC_STEP-MCC_VTN)/(MCC_VDDmax-MCC_VTN))*(MCC_VDDmax * MCC_KRNT / MCC_DC_STEP)) ;
   vr = MCC_DC_STEP * step ;

   ir = mcc_spicecurrent(i*MCC_DC_STEP,vr,MCC_VBULKN,MCC_TRANS_N,lotrsparam_n) ;
   r = mcc_calcul_r(vr,ir,1.0,1.0) ;
   fprintf(file,"%g %g",i*MCC_DC_STEP,r) ;

   ir = mcc_current(i*MCC_DC_STEP,vr,0.0,MCC_TRANS_N) ;
   if(ir != 0.0)
     r = mcc_calcul_r(vr,ir,1.0,1.0) ;
   else
     r = 0.0 ;
   fprintf(file," %g",r) ;

   vr = ((i*MCC_DC_STEP-MCC_VTP)/(MCC_VDDmax-MCC_VTP))*(MCC_VDDmax * MCC_KRPT) ;
   ir = mcc_spicecurrent(i*MCC_DC_STEP,vr,MCC_VBULKP-MCC_VDDmax,MCC_TRANS_P,lotrsparam_p) ;
   r = mcc_calcul_r(vr,ir,1.0,1.0) ;
   fprintf(file," %g",r) ;

   ir = mcc_current(i*MCC_DC_STEP,vr,0.0,MCC_TRANS_P) ;
   if(ir != 0.0)
     r = mcc_calcul_r(vr,ir,1.0,1.0) ;
   else
     r = 0.0 ;
   fprintf(file," %g",r) ;
   fprintf(file," \n") ;
  }

 fclose(file) ;
 file = mbkfopen(name,"plt",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");

 for(i = 0 ; i < 2 ; i++)
  {
   double vt;

   if ( i == 0 )
     vt = MCC_VTN+(MCC_VDDmax-MCC_VTN)/3.0;
   else
     vt = MCC_VTP+(MCC_VDDmax-MCC_VTP)/3.0;
   fprintf(file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
   fprintf(file,"set title \"%s\"\n", i==0?"NMOS":"PMOS");
   fprintf(file,"set xlabel \"VGS (V)\"\n") ;
   fprintf(file,"set ylabel \"RLIN (O)\"\n") ;
   fprintf(file,"set xrange [%g:]\n", vt);
   fprintf(file,"plot '%s.dat' using 1:%d title '%s'  with lines, '%s.dat' using 1:%d title 'elp' with lines\n", name, 2+2*i, mcc_getmccname(i==0?ptmodeln:ptmodelp), name, 3+2*i ) ;
   fprintf(file,"pause -1 'Hit CR to finish'\n") ;
  }

 MCC_CALC_CUR = mode ;
 fclose(file) ;
}

/****************************************************************************\
 * Function : mcc_calcul_vsat
\****************************************************************************/
void mcc_calcul_vsat(lotrsparam_n,lotrsparam_p)
elp_lotrs_param *lotrsparam_n;
elp_lotrs_param *lotrsparam_p;
{
 double il ;
 double rl ;
 double is ;
 double rs ;
 double vlin ;
 double vsat ;
 double usat ;
 double isat ;
 double imax ;
 char mode ;
 FILE *file ;
 int stepmax ;
 int step ;
 int i ;
 char *name=mcc_debug_prefix("vsat");
 mcc_modellist *ptmodeln,*ptmodelp;
 mode = MCC_CALC_CUR ;
 MCC_CALC_CUR = MCC_CALC_MODE ;

 ptmodeln = mcc_getmodel ( MCC_MODELFILE,
                           MCC_TNMODEL,
                           MCC_NMOS,
                           MCC_NCASE,
                           MCC_LN*1.0e-6,
                           MCC_WN*1.0e-6, 0);
 ptmodelp = mcc_getmodel ( MCC_MODELFILE,
                           MCC_TPMODEL,
                           MCC_PMOS,
                           MCC_PCASE,
                           MCC_LP*1.0e-6,
                           MCC_WP*1.0e-6, 0);
 stepmax = mcc_ftoi((MCC_VDDmax) / (MCC_DC_STEP)) ;

 file = mbkfopen(name,"dat",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");

 fprintf(file,"# vgs spicenusat mccnusat spicpusat mccpusat\n") ;

 for(i = 0 ; i <= stepmax ; i++)
  {
   step = mcc_ftoi(((i*MCC_DC_STEP-MCC_VTN)/(MCC_VDDmax-MCC_VTN))*(MCC_VDDmax * MCC_KRNT / MCC_DC_STEP)) ;
   vlin = step * MCC_DC_STEP ;
   step =  mcc_ftoi(((i*MCC_DC_STEP-MCC_VTN)/(MCC_VDDmax-MCC_VTN))*(MCC_VDDmax * MCC_KRNS / MCC_DC_STEP)) ;
   vsat = step * MCC_DC_STEP ;
   imax = mcc_spicecurrent(i*MCC_DC_STEP,MCC_VDDmax,MCC_VBULKN,MCC_TRANS_N,lotrsparam_n) ;
   is = mcc_spicecurrent(i*MCC_DC_STEP,vsat,MCC_VBULKN,MCC_TRANS_N,lotrsparam_n) ;
   rs = mcc_calcul_rs(MCC_VDDmax,imax,vsat,is,MCC_LN+MCC_DLN,MCC_WN+MCC_DWN) ;
   il = mcc_spicecurrent(i*MCC_DC_STEP,vlin,MCC_VBULKN,MCC_TRANS_N,lotrsparam_n) ;
   if(il != 0.0)
     rl = mcc_calcul_r(vlin,il,MCC_LN+MCC_DLN,MCC_WN+MCC_DWN) ;
   else
     rl = 0.0 ;
   usat = (rl * (imax*(MCC_LN+MCC_DLN)/(MCC_WN+MCC_DWN)) - rl * MCC_VDDmax/rs) / (1.0 - rl/rs) ;

   fprintf(file,"%g %g",i*MCC_DC_STEP,((i * MCC_DC_STEP) <= MCC_VTN) ? 0.0 : usat) ;

   isat = imax - ((MCC_VDDmax - usat) / rs) * (MCC_WN+MCC_DWN)/(MCC_LN+MCC_DLN) ;
   fprintf(file," %g",((i * MCC_DC_STEP) <= MCC_VTN) ? 0.0 : isat) ;

   imax = mcc_current(i*MCC_DC_STEP,MCC_VDDmax,0.0,MCC_TRANS_N) ;
   is = mcc_current(i*MCC_DC_STEP,vsat,0.0,MCC_TRANS_N) ;
   rs = mcc_calcul_rs(MCC_VDDmax,imax,vsat,is,MCC_LN+MCC_DLN,MCC_WN+MCC_DWN) ;
   il = mcc_current(i*MCC_DC_STEP,vlin,0.0,MCC_TRANS_N) ;
   if(il != 0.0)
     rl = mcc_calcul_r(vlin,il,MCC_LN+MCC_DLN,MCC_WN+MCC_DWN) ;
   else
     rl = 0.0 ;

   if( rs <= 0.0 || rl <= 0.0 || fabs(rl/rs-1) < 0.001 )
     usat = 0.0 ;
   else
     usat = (rl * (imax*(MCC_LN+MCC_DLN)/(MCC_WN+MCC_DWN)) - rl * MCC_VDDmax/rs) / (1.0 - rl/rs) ;

   fprintf(file," %g",((i * MCC_DC_STEP) <= MCC_VTN) ? 0.0 : usat) ;

   isat = imax - ((MCC_VDDmax - usat) / rs) * (MCC_WN+MCC_DWN)/(MCC_LN+MCC_DLN) ;
   fprintf(file," %g",((i * MCC_DC_STEP) <= MCC_VTN) ? 0.0 : isat) ;

   usat =  mcc_calcul_ures(i * MCC_DC_STEP,0.0,MCC_TRANS_N) ;
   fprintf(file," %g",((i * MCC_DC_STEP) <= MCC_VTN) ? 0.0 : usat) ;

   isat =  mcc_calcul_ires(i * MCC_DC_STEP,0.0,MCC_TRANS_N) ;
   isat *= (MCC_WN+MCC_DWN)/(MCC_LN+MCC_DLN) ;
   fprintf(file," %g",((i * MCC_DC_STEP) <= MCC_VTN) ? 0.0 : isat) ;

   step = mcc_ftoi(((i*MCC_DC_STEP-MCC_VTP)/(MCC_VDDmax-MCC_VTP))*(MCC_VDDmax * MCC_KRPT / MCC_DC_STEP)) ;
   vlin = step * MCC_DC_STEP ;
   step =  mcc_ftoi(((i*MCC_DC_STEP-MCC_VTP)/(MCC_VDDmax-MCC_VTP))*(MCC_VDDmax * MCC_KRPS / MCC_DC_STEP )) ;
   vsat = step * MCC_DC_STEP + 10.0 * MCC_DC_STEP ;
   imax = mcc_spicecurrent(i*MCC_DC_STEP,MCC_VDDmax,MCC_VBULKP-MCC_VDDmax,MCC_TRANS_P,lotrsparam_p) ;
   is = mcc_spicecurrent(i*MCC_DC_STEP,vsat,MCC_VBULKP-MCC_VDDmax,MCC_TRANS_P,lotrsparam_p) ;
   rs = mcc_calcul_rs(MCC_VDDmax,imax,vsat,is,MCC_LP+MCC_DLP,MCC_WP+MCC_DWP) ;
   il = mcc_spicecurrent(i*MCC_DC_STEP,vlin,MCC_VBULKP-MCC_VDDmax,MCC_TRANS_P,lotrsparam_p) ;
   if(il != 0.0)
     rl = mcc_calcul_r(vlin,il,MCC_LP+MCC_DLP,MCC_WP+MCC_DWP) ;
   else
     rl = 0.0 ;
   usat = (rl * (imax*(MCC_LP+MCC_DLP)/(MCC_WP+MCC_DWP)) - rl * MCC_VDDmax/rs) / (1.0 - rl/rs) ;

   fprintf(file," %g",((i * MCC_DC_STEP) <= MCC_VTP) ? 0.0 : usat) ;

   isat = imax - ((MCC_VDDmax - usat) / rs) * (MCC_WP+MCC_DWP)/(MCC_LP+MCC_DLP) ;
   fprintf(file," %g",((i * MCC_DC_STEP) <= MCC_VTP) ? 0 : isat) ;

   imax = mcc_current(i*MCC_DC_STEP,MCC_VDDmax,0.0,MCC_TRANS_P) ;
   is = mcc_current(i*MCC_DC_STEP,vsat,0.0,MCC_TRANS_P) ;
   rs = mcc_calcul_rs(MCC_VDDmax,imax,vsat,is,MCC_LP+MCC_DLP,MCC_WP+MCC_DWP) ;
   il = mcc_current(i*MCC_DC_STEP,vlin,0.0,MCC_TRANS_P) ;
   if(il != 0.0)
     rl = mcc_calcul_r(vlin,il,MCC_LP+MCC_DLP,MCC_WP+MCC_DWP) ;
   else
     rl = 0.0 ;
   if( rs <= 0.0 || rl <= 0.0 || fabs(rl/rs-1) < 0.001 )
     usat = 0.0 ;
   else
     usat = (rl * (imax*(MCC_LP+MCC_DLP)/(MCC_WP+MCC_DWP)) - rl * MCC_VDDmax/rs) / (1.0 - rl/rs) ;

   fprintf(file," %g",((i * MCC_DC_STEP) <= MCC_VTP) ? 0.0 : usat) ;

   isat = imax - ((MCC_VDDmax - usat) / rs) * (MCC_WP+MCC_DWP)/(MCC_LP+MCC_DLP) ;
   fprintf(file," %g",((i * MCC_DC_STEP) <= MCC_VTP) ? 0.0 : isat) ;

   usat =  mcc_calcul_ures(i * MCC_DC_STEP,0.0,MCC_TRANS_P) ;
   fprintf(file," %g",((i * MCC_DC_STEP) <= MCC_VTP) ? 0.0 : usat) ;

   isat =  mcc_calcul_ires(i * MCC_DC_STEP,0.0,MCC_TRANS_P) ;
   isat *= (MCC_WP+MCC_DWP)/(MCC_LP+MCC_DLP) ;
   fprintf(file," %g",((i * MCC_DC_STEP) <= MCC_VTP) ? 0.0 : isat) ;

   fprintf(file," \n") ;
  }

 fclose(file) ;
 file = mbkfopen(name,"plt",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");

 for(i = 0 ; i < 2 ; i++)
  {
   fprintf(file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
   fprintf(file,"set title \"%s\"\n", i==0?"NMOS":"PMOS");
   fprintf(file,"set xlabel \"VGS (V)\"\n") ;
   fprintf(file,"set ylabel \"VSAT (V)\"\n") ;
   fprintf(file,"set xrange [%g:%g]\n", (i == 0)?MCC_VTN:MCC_VTP,MCC_VDDmax*1.05);
   fprintf(file,"plot '%s.dat' using 1:%d title '%s'  with lines, '%s.dat' using 1:%d title 'elp' with lines, '%s.dat' using 1:%d title 'elp (charac only)' with lines \n", name, 2+6*i, mcc_getmccname( i==0?ptmodeln:ptmodelp), name,4+6*i, name, 6+6*i ) ;
   fprintf(file,"pause -1 'Hit CR to finish'\n") ;
   fprintf(file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
   fprintf(file,"set xlabel \"VGS (V)\"\n") ;
   fprintf(file,"set ylabel \"ISAT (A)\"\n") ;
   fprintf(file,"set xrange [%g:%g]\n", (i == 0)?MCC_VTN:MCC_VTP,MCC_VDDmax*1.05);
   fprintf(file,"plot '%s.dat' using 1:%d title '%s'  with lines, '%s.dat' using 1:%d title 'elp isat' with lines, '%s.dat' using 1:%d title 'elp (charac only)' with lines\n", name, 3+6*i, mcc_getmccname( i==0?ptmodeln:ptmodelp), name, 5+6*i, name, 7+6*i ) ;
   fprintf(file,"pause -1 'Hit CR to finish'\n") ;
  }

 MCC_CALC_CUR = mode ;
 fclose(file) ;
}

/****************************************************************************\
 * Function : mcc_calcul_vth
\****************************************************************************/
void mcc_calcul_vth( elp_lotrs_param *lotrsparam_n, elp_lotrs_param *lotrsparam_p )
{
 double vt ;
 char mode ;
 double factor[7] ;
 FILE *file ;
 int stepmax ;
 int i,k ;
 char *name=mcc_debug_prefix("vth");
 mcc_modellist *ptmodeln,*ptmodelp;

 ptmodeln = mcc_getmodel ( MCC_MODELFILE,
                           MCC_TNMODEL,
                           MCC_NMOS,
                           MCC_NCASE,
                           MCC_LN*1.0e-6,
                           MCC_WN*1.0e-6, 0);
 ptmodelp = mcc_getmodel ( MCC_MODELFILE,
                           MCC_TPMODEL,
                           MCC_PMOS,
                           MCC_PCASE,
                           MCC_LP*1.0e-6,
                           MCC_WP*1.0e-6, 0);
 factor[0] = 1.0 ;
 factor[1] = 3.0/4.0 ;
 factor[2] = 2/3.0 ;
 factor[3] = 1.0/2.0 ;
 factor[4] = 1.0/3.0 ;
 factor[5] = 1.0/4.0 ;
 factor[6] = 0.0 ;

 mode = MCC_CALC_CUR ;
 MCC_CALC_CUR = MCC_CALC_MODE ;

 stepmax = mcc_ftoi((MCC_VDDmax) / (MCC_DC_STEP)) ;

 file = mbkfopen(name,"dat",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");

 fprintf(file,"# vgs ispicevthn ispicevthn mccvthp mccvthp\n") ;

 for(i = 0 ; i <= stepmax ; i++)
  {
   fprintf(file,"%g",i*MCC_DC_STEP) ;

   for(k = 1 ; k < 8 ; k++)
     {
      vt = fabs(mcc_calcVTH (MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE, 
                             MCC_LN * 1.0e-6, MCC_WN * 1.0e-6, MCC_TEMP, -MCC_VDDmax*factor[k-1],
                             i*MCC_DC_STEP, lotrsparam_n,MCC_NO_LOG)) ;
      fprintf(file," %g",vt) ;

      vt = MCC_VTN + MCC_KTN * (MCC_VDDmax*factor[k-1]) ;
      fprintf(file," %g",vt) ;
     }

   for(k = 1 ; k < 8 ; k++)
     {
      vt = fabs(mcc_calcVTH (MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS, MCC_PCASE,
                             MCC_LP * 1.0e-6, MCC_WP * 1.0e-6, MCC_TEMP, 
                             MCC_VDDmax*factor[k-1], i*MCC_DC_STEP,lotrsparam_p,MCC_NO_LOG));
      fprintf(file," %g",vt) ;

      vt = MCC_VTP + MCC_KTP * (MCC_VDDmax*factor[k-1]) ;
      fprintf(file," %g",vt) ;
     }

   for(k = 1 ; k < 8 ; k++)
     {
      vt = fabs(mcc_calcVTH (MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, 
                             MCC_WN * 1.0e-6, MCC_TEMP, -i*MCC_DC_STEP, MCC_VDDmax*factor[k-1],
                             lotrsparam_n,MCC_NO_LOG)) ;
      fprintf(file," %g",vt) ;

      vt = MCC_VTN + MCC_KTN * i*MCC_DC_STEP ;
      fprintf(file," %g",vt) ;
     }

   for(k = 1 ; k < 8 ; k++)
     {
      vt = fabs(mcc_calcVTH (MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS, MCC_PCASE,
                             MCC_LP * 1.0e-6, MCC_WP * 1.0e-6, MCC_TEMP, i*MCC_DC_STEP,
                             MCC_VDDmax*factor[k-1],lotrsparam_p,MCC_NO_LOG));
      fprintf(file," %g",vt) ;

      vt = MCC_VTP + MCC_KTP * i*MCC_DC_STEP ;
      fprintf(file," %g",vt) ;
     }

   fprintf(file," \n") ;
  }

 fclose(file) ;
 file = mbkfopen(name,"plt",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");

 for(i = 0 ; i < 2 ; i++)
  {
   fprintf(file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
   fprintf(file,"set title \"%s\"\n", i==0?"NMOS":"PMOS");
   fprintf(file,"set xlabel \"VDS (V)\"\n") ;
   fprintf(file,"set ylabel \"VTH (A)\"\n") ;
   fprintf(file,"set xrange [0.0:%g]\n", MCC_VDDmax+1);
   fprintf(file,"plot ") ;
   for(k = 1 ; k < 8 ; k++)
    {
     fprintf(file,"'%s.dat' using 1:%d title '%s vbs=%g' with lines,'%s.dat' using 1:%d title 'elp vbs=%g' with lines%c",name,2*k+i*14,mcc_getmccname( i==0?ptmodeln:ptmodelp),MCC_VDDmax*factor[k-1],name,2*k+1+i*14,MCC_VDDmax*factor[k-1],(k == 7)?'\n':',') ;
    }
   fprintf(file,"pause -1 'Hit CR to finish'\n") ;
  }

 for(i = 0 ; i < 2 ; i++)
  {
   fprintf(file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
   fprintf(file,"set title \"%s\"\n", i==0?"NMOS":"PMOS");
   fprintf(file,"set xlabel \"VBS (V)\"\n") ;
   fprintf(file,"set ylabel \"VTH (A)\"\n") ;
   fprintf(file,"set xrange [0.0:%g]\n", MCC_VDDmax+1);
   fprintf(file,"plot ") ;
   for(k = 1 ; k < 8 ; k++)
    {
     fprintf(file,"'%s.dat' using 1:%d title '%s vds=%g' with lines,'%s.dat' using 1:%d title 'elp vds=%g' with lines%c",name,2*k+i*14+28,mcc_getmccname( i==0?ptmodeln:ptmodelp),MCC_VDDmax*factor[k-1],name,2*k+1+i*14+28,MCC_VDDmax*factor[k-1],(k == 7)?'\n':',') ;
    }
   fprintf(file,"pause -1 'Hit CR to finish'\n") ;
  }

 MCC_CALC_CUR = mode ;
 fclose(file) ;
}

/****************************************************************************\
 * Function : mcc_calcul_qint
\****************************************************************************/
static void readqintresults(char *name, char *n[8], int j, char location)
{
  int i, tabsize, k, m;
  char *fileout, *c;
  double *tab[8];
  double s ;
  char *argv[8];
  char buffer[1024];
  FILE *f;

  tabsize = mcc_ftoi(MCC_VDDmax / MCC_DC_STEP) +1;
  fileout = sim_getjoker(MCC_SPICEOUT,name) ;
  
  for (i=0; i<8; i++)
  {
   if ((c=strchr(n[i],'='))!=NULL) *c='\0';
   argv[i]=n[i];
  }

  if( location =='G' ) 
    m=0 ;
  else
    m=1 ;

  for (i=0; i<4; i++)
    for (k=0; k<2; k++)
      {
        TRS_CURVS.QINT_SPICE[j][k][m][i]=mbkalloc(sizeof(double)*tabsize);
        tab[k*4+i]=TRS_CURVS.QINT_SPICE[j][k][m][i];
      }
  sim_readspifiletab (fileout,argv,8,tabsize,tab,MCC_VDDmax,MCC_DC_STEP) ;

  sprintf(buffer, "%s.spice.dat", sim_getjoker("$",name) );
  
  f = mbkfopen( buffer, NULL, "w" );
  fprintf( f, "#vsweep" ) ;
  for( k=0 ; k<8 ; k++ ) {
    if( k==4) 
      fprintf( f, " %s", "QTOT" );
    fprintf( f, " %s", argv[k] );
  }
  fprintf( f, " %s", "QTOT" );
  fprintf( f, "\n" );

  for( i=0 ; i<tabsize; i++ ) {
    fprintf( f, "%g", ((float)i)*MCC_DC_STEP ) ;
    s = 0 ;
    for( k=0 ; k<8 ; k++ ) {
      fprintf( f, " %g",tab[k][i] );
      s = s + tab[k][i];
      if( k==3 ) {
        fprintf( f, " %g", s );
        s = 0 ;
      }
    }
    fprintf( f, " %g", s );
    fprintf( f, "\n" );
  }
  fclose(f);
}

void mcc_calcul_qint (elp_lotrs_param *lotrsparam_n,elp_lotrs_param *lotrsparam_p, char location)
{
 FILE *file;
 int b3=0,b4=0;
 char buf[1024], name[1024];
 char qs0[256], qs1[256], qd0[256], qd1[256], qg0[256], qg1[256], qb0[256], qb1[256];
 char *trname0, *probename0;
 char *trname1, *probename1, *n[8];
 char *drain0, *drain1, *grid0, *grid1 ;
 
 n[0]=qg0; n[1]=qb0; n[2]=qs0; n[3]=qd0;
 n[4]=qg1; n[5]=qb1; n[6]=qs1; n[7]=qd1;

 b3 =  MCC_SPICEMODELTYPE == MCC_BSIM3V3 ? 1 : 0 ;
 b4 =  MCC_SPICEMODELTYPE == MCC_BSIM4   ? 1 : 0 ;

 mcc_PrintQint (MCC_MODELFILE, MCC_TNMODEL,
                MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6,
                MCC_WN * 1.0e-6, MCC_TEMP, MCC_VDDmax,lotrsparam_n, "nmos_qint_mcc_mn0", "nmos_qint_mcc_mn1",0,location);
 mcc_PrintQint (MCC_MODELFILE, MCC_TPMODEL,
                MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6,
                MCC_WP * 1.0e-6, MCC_TEMP, MCC_VDDmax,lotrsparam_p, "pmos_qint_mcc_mp0", "pmos_qint_mcc_mp1",0,location);

 // drive simulation file

 mcc_gettrsname('n', &trname0, &probename0, "mn0");
 mcc_gettrsname('n', &trname1, &probename1, "mn1");

 // NMOS case
 if( location=='G' ) {
   grid0="var";
   grid1="var";
   drain0="vdd";
   drain1="vss";
 }
 else {
   grid0="vdd";
   grid1="vss";
   drain0="var";
   drain1="var";
 }
 sprintf( name, "%s_%c", mcc_debug_prefix("nmos_qint"), location );
 file = mbkfopen( name, "spi", WRITE_TEXT ) ;
 avt_printExecInfo(file, "*", "", "");

 fprintf(file,"\n");
 if(MCC_SPICEOPTIONS != NULL)
   fprintf(file,".option %s\n",MCC_SPICEOPTIONS);
 if (V_INT_TAB[__SIM_TOOL].VALUE==SIM_TOOL_HSPICE)
   fprintf(file,".option DCCAP=1 INGOLD=1 CO=256\n");
   
 if (V_INT_TAB[__SIM_TOOL].VALUE==SIM_TOOL_NGSPICE)
   fprintf(file,"\n.include %s\n\n",MCC_TECHFILE) ;
 else
   fprintf(file,"\n.include \"%s\"\n\n",MCC_TECHFILE) ;
 fprintf(file,".option tnom=%g\n", V_FLOAT_TAB[__SIM_TNOM].VALUE);
 fprintf(file,".temp %g\n\n",MCC_TEMP) ;

   
 fprintf(file,"%s %s %s vss nbulk %s l=%gu w=%gu %s\n", trname0, drain0, grid0, MCC_TNMODEL, MCC_LN,MCC_WN,surf_string_n);
 mcc_printf_instance_specific(file, &mcc_user_lotrsparam_n);

 fprintf(file,"%s %s %s vss nbulk %s l=%gu w=%gu %s\n",trname1, drain1, grid1, MCC_TNMODEL, MCC_LN,MCC_WN,surf_string_n);
 mcc_printf_instance_specific(file, &mcc_user_lotrsparam_n);
   
 fprintf(file,"\nvbulkn nbulk 0 dc %gv\n",MCC_VBULKN) ;
 fprintf(file,"Vvss vss 0 dc 0v\n") ;
 fprintf(file,"Vvdd vdd 0 dc %gv\n",MCC_VDDmax) ;
 fprintf(file,"var  var 0 dc 0\n") ;
 fprintf(file,".dc var 0 %g %g\n",MCC_VDDmax,MCC_DC_STEP) ;

 switch( V_INT_TAB[__SIM_TOOL].VALUE ) {
 case SIM_TOOL_HSPICE :
 case SIM_TOOL_MSPICE :
   if( b3 ) {
     sprintf(qd0,"qdi(%s)", probename0);
     sprintf(qd1,"qdi(%s)", probename1);
     sprintf(qg0,"qgi(%s)", probename0);
     sprintf(qg1,"qgi(%s)", probename1);
     sprintf(qs0,"qsi(%s)", probename0);
     sprintf(qs1,"qsi(%s)", probename1);
     sprintf(qb0,"qbi(%s)", probename0);
     sprintf(qb1,"qbi(%s)", probename1);
   }
   else {
     sprintf(qd0,"qdint_0=PAR(`qd(%s)-(vds(%s)-vgs(%s))*COVLGD(%s)')", probename0, probename0, probename0, probename0);
     sprintf(qd1,"qdint_1=PAR(`qd(%s)-(vds(%s)-vgs(%s))*COVLGD(%s)')", probename1, probename1, probename1, probename1);
     sprintf(qg0,"qgint_0=PAR(`qg(%s)-vgs(%s)*COVLGS(%s)-(vgs(%s)-vds(%s))*COVLGD(%s)-(vgs(%s)-vbs(%s))*COVLGB(%s)')", probename0, probename0, probename0, probename0, probename0, probename0, probename0, probename0, probename0);
     sprintf(qg1,"qgint_1=PAR(`qg(%s)-vgs(%s)*COVLGS(%s)-(vgs(%s)-vds(%s))*COVLGD(%s)-(vgs(%s)-vbs(%s))*COVLGB(%s)')", probename1, probename1, probename1, probename1, probename1, probename1, probename1, probename1, probename1);
     sprintf(qs0,"qsint_0=PAR(`-QB(%s)-QG(%s)-QD(%s)+VGS(%s)*COVLGS(%s)')", probename0, probename0, probename0, probename0, probename0);
     sprintf(qs1,"qsint_1=PAR(`-QB(%s)-QG(%s)-QD(%s)+VGS(%s)*COVLGS(%s)')", probename1, probename1, probename1, probename1, probename1);
     sprintf(qb0,"qbint_0=PAR('qb(%s)+(vgs(%s)-vbs(%s))*COVLGB(%s)')", probename0, probename0, probename0, probename0);
     sprintf(qb1,"qbint_1=PAR('qb(%s)+(vgs(%s)-vbs(%s))*COVLGB(%s)')", probename1, probename1, probename1, probename1);
   }
   break ;
 case SIM_TOOL_NGSPICE :
   sprintf(qd0,"@%s[qd]", probename0);
   sprintf(qd1,"@%s[qd]", probename1);
   sprintf(qg0,"@%s[qg]", probename0);
   sprintf(qg1,"@%s[qg]", probename1);
   sprintf(qs0,"@%s[qs]", probename0);
   sprintf(qs1,"@%s[qs]", probename1);
   sprintf(qb0,"@%s[qb]", probename0);
   sprintf(qb1,"@%s[qb]", probename1);
   break ;
 case SIM_TOOL_ELDO :
   if( b3 ) {
     /*
     ne fonctionne pas : les parametres overlap lv36-38 renvoient 0
     */
     printf( "warning : eldo can't display intrinsic charge for bsim3 model\n" );
     fprintf( file, ".defwave qdint0=lx16(%s)-(lx3(%s)-lx2(%s))*lv37(%s)\n", probename0, probename0, probename0, probename0);
     sprintf( qd0, "w(qdint0)" ),
     fprintf( file, ".defwave qdint1=lx16(%s)-(lx3(%s)-lx2(%s))*lv37(%s)\n", probename1, probename1, probename1, probename1);
     sprintf( qd1, "w(qdint1)" );
     fprintf( file, ".defwave qgint0=lx14(%s)-lx2(%s)*lv36(%s)-(lx2(%s)-lx3(%s))*lv37(%s)-(lx2(%s)-lx1(%s))*lv38(%s)\n", probename0, probename0, probename0, probename0, probename0, probename0, probename0, probename0, probename0);
     sprintf( qg0, "w(qgint0)" );
     fprintf( file, ".defwave qgint1=lx14(%s)-lx2(%s)*lv36(%s)-(lx2(%s)-lx3(%s))*lv37(%s)-(lx2(%s)-lx1(%s))*lv38(%s)\n", probename1, probename1, probename1, probename1, probename1, probename1, probename1, probename1, probename1);
     sprintf( qg1, "w(qgint1)" );
     fprintf( file, ".defwave qsint0=-lx12(%s)-lx14(%s)-lx16(%s)+lx2(%s)*lv36(%s)\n", probename0, probename0, probename0, probename0, probename0 );
     sprintf( qs0, "w(qsint0)" );
     fprintf( file, ".defwave qsint1=-lx12(%s)-lx14(%s)-lx16(%s)+lx2(%s)*lv36(%s)\n", probename1, probename1, probename1, probename1, probename1);
     sprintf( qs1, "w(qsint1)" );
     fprintf( file,".defwave qbint0=lx12(%s)+(lx2(%s)-lx1(%s))*lv38(%s)\n", probename0, probename0, probename0, probename0);
     sprintf( qb0, "w(qbint0)" );
     fprintf( file,".defwave qbint1=lx12(%s)+(lx2(%s)-lx1(%s))*lv38(%s)\n", probename1, probename1, probename1, probename1);
     sprintf( qb1, "w(qbint1)" );
   }
   else {
     /*
     ces variables ne sont disponibles que pour du bsim4 
     */
     sprintf( qd0, "S(%s->qdrn)", probename0 );
     sprintf( qd1, "S(%s->qdrn)", probename1 );
     sprintf( qs0, "S(%s->qsrc)", probename0 );
     sprintf( qs1, "S(%s->qsrc)", probename1 );
     sprintf( qb0, "S(%s->qbulk)", probename0 );
     sprintf( qb1, "S(%s->qbulk)", probename1 );
     sprintf( qg0, "S(%s->qgate)", probename0 );
     sprintf( qg1, "S(%s->qgate)", probename1 );
   }
   break ;
   
 default :
   sprintf(qd0,"qd(%s)", probename0);
   sprintf(qd1,"qd(%s)", probename1);
   sprintf(qg0,"qg(%s)", probename0);
   sprintf(qg1,"qg(%s)", probename1);
   sprintf(qs0,"qs(%s)", probename0);
   sprintf(qs1,"qs(%s)", probename1);
   sprintf(qb0,"qb(%s)", probename0);
   sprintf(qb1,"qb(%s)", probename1);
 }
 fprintf(file, ".print dc %s\n", qg0 );
 fprintf(file, ".print dc %s\n", qb0 );
 fprintf(file, ".print dc %s\n", qs0 );
 fprintf(file, ".print dc %s\n", qd0 );
 fprintf(file, ".print dc %s\n", qg1 );
 fprintf(file, ".print dc %s\n", qb1 );
 fprintf(file, ".print dc %s\n", qs1 );
 fprintf(file, ".print dc %s\n", qd1 );
 if ( V_INT_TAB[__SIM_TOOL].VALUE == SIM_TOOL_TITAN || V_INT_TAB[__SIM_TOOL].VALUE == SIM_TOOL_TITAN7)
   fprintf(file,".save %s\n", name) ;

 fprintf(file,"\n.end\n") ;
 fclose(file) ;

 sprintf(buf,"%s.spi",name);
 sim_execspice(buf,0,MCC_SPICENAME,MCC_SPICESTRING,MCC_SPICESTDOUT);
 readqintresults(buf, n, 0, location);
 
 // --------------------------
 
 mcc_gettrsname('p', &trname0, &probename0, "mp0");
 mcc_gettrsname('p', &trname1, &probename1, "mp1");
 // PMOS case
 if( location=='G' ) {
   grid0="var";
   grid1="var";
   drain0="vdd";
   drain1="vss";
 }
 else {
   grid0="vdd";
   grid1="vss";
   drain0="var";
   drain1="var";
 }
 sprintf( name, "%s_%c", mcc_debug_prefix("pmos_qint"), location );
 file = mbkfopen(name,"spi",WRITE_TEXT) ;
 avt_printExecInfo(file, "*", "", "");

 fprintf(file,"\n");
 if(MCC_SPICEOPTIONS != NULL)
   fprintf(file,".option %s\n",MCC_SPICEOPTIONS);

 if (V_INT_TAB[__SIM_TOOL].VALUE==SIM_TOOL_HSPICE)
   fprintf(file,".option DCCAP=1 INGOLD=1 CO=256\n");

 if (V_INT_TAB[__SIM_TOOL].VALUE==SIM_TOOL_NGSPICE)
   fprintf(file,"\n.include %s\n\n",MCC_TECHFILE) ;
 else
   fprintf(file,"\n.include \"%s\"\n\n",MCC_TECHFILE) ;

 fprintf(file,".option tnom=%g\n", V_FLOAT_TAB[__SIM_TNOM].VALUE);
 fprintf(file,".temp %g\n\n",MCC_TEMP) ;

 fprintf(file,"%s %s %s vdd pbulk %s l=%gu w=%gu %s\n", trname0, drain0, grid0, MCC_TPMODEL, MCC_LP,MCC_WP,surf_string_p);
 mcc_printf_instance_specific(file, &mcc_user_lotrsparam_p);

 fprintf(file,"%s %s %s vdd pbulk %s l=%gu w=%gu %s\n", trname1, drain1, grid1, MCC_TPMODEL, MCC_LP,MCC_WP,surf_string_p);
 mcc_printf_instance_specific(file, &mcc_user_lotrsparam_p);
   
 fprintf(file,"vbulkp pbulk 0 dc %gv\n",MCC_VBULKP) ;
 fprintf(file,"Vvss vss 0 dc 0v\n") ;
 fprintf(file,"Vvdd vdd 0 dc %gv\n",MCC_VDDmax) ;
 fprintf(file,"var  var 0 dc 0\n") ;
 fprintf(file,".dc var 0 %g %g\n",MCC_VDDmax,MCC_DC_STEP) ;

 switch( V_INT_TAB[__SIM_TOOL].VALUE ) {
 case SIM_TOOL_HSPICE :
 case SIM_TOOL_MSPICE :
   if( b3 ) {
     sprintf(qd0,"qdi(%s)", probename0);
     sprintf(qd1,"qdi(%s)", probename1);
     sprintf(qg0,"qgi(%s)", probename0);
     sprintf(qg1,"qgi(%s)", probename1);
     sprintf(qs0,"qsi(%s)", probename0);
     sprintf(qs1,"qsi(%s)", probename1);
     sprintf(qb0,"qbi(%s)", probename0);
     sprintf(qb1,"qbi(%s)", probename1);
   }
   else {
     sprintf(qd0,"qdint_0=PAR(`(qd(%s)-(vds(%s)-vgs(%s))*COVLGD(%s))')",probename0, probename0, probename0, probename0);
     sprintf(qd1,"qdint_1=PAR(`(qd(%s)-(vds(%s)-vgs(%s))*COVLGD(%s))')",probename1, probename1, probename1, probename1);
     sprintf(qg0,"qgint_0=PAR(`qg(%s)-vgs(%s)*COVLGS(%s)-(vgs(%s)-vds(%s))*COVLGS(%s)-(vgs(%s)-vbs(%s))*COVLGB(%s)')", probename0, probename0, probename0, probename0, probename0, probename0, probename0, probename0, probename0);
     sprintf(qg1,"qgint_1=PAR(`qg(%s)-vgs(%s)*COVLGS(%s)-(vgs(%s)-vds(%s))*COVLGS(%s)-(vgs(%s)-vbs(%s))*COVLGB(%s)')", probename1, probename1, probename1, probename1, probename1, probename1, probename1, probename1, probename1);
     sprintf(qs0,"qsint_0=PAR(`(-QB(%s)-QG(%s)-QD(%s)+VGS(%s)*COVLGS(%s))')",probename0,probename0,probename0,probename0,probename0);
     sprintf(qs1,"qsint_1=PAR(`(-QB(%s)-QG(%s)-QD(%s)+VGS(%s)*COVLGS(%s))')",probename1,probename1,probename1,probename1,probename1);
     sprintf(qb0,"qbint_0=PAR('qb(%s)+(vgs(%s)-vbs(%s))*COVLGB(%s)')", probename0, probename0, probename0, probename0);
     sprintf(qb1,"qbint_1=PAR('qb(%s)+(vgs(%s)-vbs(%s))*COVLGB(%s)')", probename1, probename1, probename1, probename1);
   }
   break ;
 case SIM_TOOL_NGSPICE :
   sprintf(qd0,"@%s[qd]", probename0);
   sprintf(qd1,"@%s[qd]", probename1);
   sprintf(qg0,"@%s[qg]", probename0);
   sprintf(qg1,"@%s[qg]", probename1);
   sprintf(qs0,"@%s[qs]", probename0);
   sprintf(qs1,"@%s[qs]", probename1);
   sprintf(qb0,"@%s[qb]", probename0);
   sprintf(qb1,"@%s[qb]", probename1);
   break ;
 case SIM_TOOL_ELDO :
   if( b3 ) {
     printf( "warning : eldo can't display intrinsic charge for bsim3 model\n" );
     fprintf( file, ".defwave qdint0=lx16(%s)-(lx3(%s)-lx2(%s))*lv37(%s)\n", probename0, probename0, probename0, probename0);
     sprintf( qd0, "w(qdint0)" ),
     fprintf( file, ".defwave qdint1=lx16(%s)-(lx3(%s)-lx2(%s))*lv37(%s)\n", probename1, probename1, probename1, probename1);
     sprintf( qd1, "w(qdint1)" );
     fprintf( file, ".defwave qgint0=lx14(%s)-lx2(%s)*lv36(%s)-(lx2(%s)-lx3(%s))*lv37(%s)-(lx2(%s)-lx1(%s))*lv38(%s)\n", probename0, probename0, probename0, probename0, probename0, probename0, probename0, probename0, probename0);
     sprintf( qg0, "w(qgint0)" );
     fprintf( file, ".defwave qgint1=lx14(%s)-lx2(%s)*lv36(%s)-(lx2(%s)-lx3(%s))*lv37(%s)-(lx2(%s)-lx1(%s))*lv38(%s)\n", probename1, probename1, probename1, probename1, probename1, probename1, probename1, probename1, probename1);
     sprintf( qg1, "w(qgint1)" );
     fprintf( file, ".defwave qsint0=-lx12(%s)-lx14(%s)-lx16(%s)+lx2(%s)*lv36(%s)\n", probename0, probename0, probename0, probename0, probename0 );
     sprintf( qs0, "w(qsint0)" );
     fprintf( file, ".defwave qsint1=-lx12(%s)-lx14(%s)-lx16(%s)+lx2(%s)*lv36(%s)\n", probename1, probename1, probename1, probename1, probename1);
     sprintf( qs1, "w(qsint1)" );
     fprintf( file,".defwave qbint0=lx12(%s)+(lx2(%s)-lx1(%s))*lv38(%s)\n", probename0, probename0, probename0, probename0);
     sprintf( qb0, "w(qbint0)" );
     fprintf( file,".defwave qbint1=lx12(%s)+(lx2(%s)-lx1(%s))*lv38(%s)\n", probename1, probename1, probename1, probename1);
     sprintf( qb1, "w(qbint1)" );
   }
   else {
     fprintf( file, ".defwave qd0=-S(%s->qdrn)\n",  probename0 );
     fprintf( file, ".defwave qd1=-S(%s->qdrn)\n",  probename1 );
     fprintf( file, ".defwave qs0=-S(%s->qsrc)\n",  probename0 );
     fprintf( file, ".defwave qs1=-S(%s->qsrc)\n",  probename1 );
     fprintf( file, ".defwave qg0=-S(%s->qgate)\n", probename0 );
     fprintf( file, ".defwave qg1=-S(%s->qgate)\n", probename1 );
     fprintf( file, ".defwave qb0=-S(%s->qbulk)\n", probename0 );
     fprintf( file, ".defwave qb1=-S(%s->qbulk)\n", probename1 );

     sprintf( qd0, "w(qd0)" );
     sprintf( qd1, "w(qd1)" );
     sprintf( qs0, "w(qs0)" );
     sprintf( qs1, "w(qs1)" );
     sprintf( qb0, "w(qb0)" );
     sprintf( qb1, "w(qb1)" );
     sprintf( qg0, "w(qg0)" );
     sprintf( qg1, "w(qg1)" );
   }
   break ;
 default :
   sprintf(qd0,"qd(%s)", probename0);
   sprintf(qd1,"qd(%s)", probename1);
   sprintf(qg0,"qg(%s)", probename0);
   sprintf(qg1,"qg(%s)", probename1);
   sprintf(qs0,"qs(%s)", probename0);
   sprintf(qs1,"qs(%s)", probename1);
   sprintf(qb0,"qb(%s)", probename0);
   sprintf(qb1,"qb(%s)", probename1);
 }
 fprintf(file, ".print dc %s\n", qg0 );
 fprintf(file, ".print dc %s\n", qb0 );
 fprintf(file, ".print dc %s\n", qs0 );
 fprintf(file, ".print dc %s\n", qd0 );
 fprintf(file, ".print dc %s\n", qg1 );
 fprintf(file, ".print dc %s\n", qb1 );
 fprintf(file, ".print dc %s\n", qs1 );
 fprintf(file, ".print dc %s\n", qd1 );
 if ( V_INT_TAB[__SIM_TOOL].VALUE == SIM_TOOL_TITAN || V_INT_TAB[__SIM_TOOL].VALUE == SIM_TOOL_TITAN7)
   fprintf(file,".save %s\n", name) ;

 fprintf(file,"\n.end\n") ;
 fclose(file) ;
 sprintf(buf,"%s.spi",name);
 sim_execspice(buf,0,MCC_SPICENAME,MCC_SPICESTRING,MCC_SPICESTDOUT);
 readqintresults(buf, n, 1, location);

}

/****************************************************************************\
 * Function : mcc_sim_cg  
\****************************************************************************/
void mcc_sim_cg(elp_lotrs_param *lotrsparam_n,elp_lotrs_param *lotrsparam_p, char trs, char con)
{
 FILE *file ;
 char *subcktmodeln, *subcktmodelp ;
 char *modeln, *modelp ;
 char *mn, *mp ;
 char bufn[1024] ;
 char bufp[1024], name[128] ;
 double t,vt,vinit,vfin,slope ;
 char *cg, *cd, *cs;
 lotrsparam_p=NULL ; // avoid warning at compilation
 lotrsparam_n=NULL ;

 t = 0.0 ;
 vt = 0.2 ;
 vinit = 0.0 ;
 vfin = MCC_VDDmax ;
 slope = MCC_SLOPE ;

 sprintf(bufn,"m") ;
 sprintf(bufp,"m") ;

 modeln = MCC_TNMODEL ;
 modelp = MCC_TPMODEL ;

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
 mn = mbkstrdup(bufn) ;
 mp = mbkstrdup(bufp) ;

 if (subcktmodeln != NULL)
  {
   sprintf(bufn,"xm") ;
   modeln = subcktmodeln ;
   mbkfree(mn) ;
   mn = mbkstrdup(bufn) ;
  }

 if (subcktmodelp != NULL)
  {
   sprintf(bufp,"xm") ;
   modelp = subcktmodelp ;
   mbkfree(mp) ;
   mp = mbkstrdup(bufp) ;
  }

 sprintf(name,"inputcapa_%c_%c", trs, con);
 file = mbkfopen(mcc_debug_prefix(name),"spi",WRITE_TEXT) ;
 avt_printExecInfo(file, "*", "", "");

 fprintf(file,"****spice description of inputcapa sim\n");
 fprintf(file,"\n");

 if(MCC_SPICEOPTIONS != NULL)
   fprintf(file,".option %s\n",MCC_SPICEOPTIONS);
 if (V_INT_TAB[__SIM_TOOL].VALUE==SIM_TOOL_HSPICE)
   fprintf(file,".option INGOLD=1\n");

 fprintf(file,".include \"%s\"\n",MCC_TECHFILE);

 fprintf(file,"%s1 f i vdd vbp %s l=%gu w=%gu %s\n"
         ,mp,modelp,MCC_LP,MCC_WP,surf_string_p);
 mcc_printf_instance_specific(file, &mcc_user_lotrsparam_p);

 fprintf(file,"%s2 f i vss vbn %s l=%gu w=%gu %s\n"
         ,mn,modeln,MCC_LN,MCC_WN,surf_string_n);
 mcc_printf_instance_specific(file, &mcc_user_lotrsparam_n);
         
 if (trs=='p')
 {
   if (con=='s') cs="fo", cg="vdd", cd="vss";
   else if (con=='d') cd="fo", cg="vdd", cs="vdd";
   else if (con=='g') cg="fo", cd="vss", cs="vdd";

   fprintf(file,"%scheck %s %s %s vbp %s l=%gu w=%gu %s\n"
           ,mp,cd,cg,cs,modelp,MCC_LP,MCC_WP,surf_string_p);
   mcc_printf_instance_specific(file, &mcc_user_lotrsparam_p);
 }
 else if (trs=='n')
 {
   if (con=='s') cs="fo", cg="vss", cd="vdd";
   else if (con=='d') cd="fo", cg="vss", cs="vss";
   else if (con=='g') cg="fo", cd="vdd", cs="vss";
   fprintf(file,"%scheck %s %s %s vbn %s l=%gu w=%gu %s\n"
           ,mn,cd,cg,cs,modeln,MCC_LN,MCC_WN,surf_string_n);
   mcc_printf_instance_specific(file, &mcc_user_lotrsparam_n);
 }
 fprintf(file,"vf f fo 0V\n");
 fprintf(file,"ci fi 0 %gff\n",MCC_VDDmax);
 fprintf(file,"ri fi 0 1e18\n");
 fprintf(file,"fi vdd fi vf 1\n");

 fprintf(file,"vdd vdd 0 %gV\n",MCC_VDDmax);
 fprintf(file,"vss vss 0 %gV\n",0.0);
 fprintf(file,"vbn vbn vss %g\n",(MCC_VBULKN > ELPMINVBULK) ? MCC_VBULKN : 0.0);
 fprintf(file,"vbp vbp vss %g\n",(MCC_VBULKP > ELPMINVBULK) ? MCC_VBULKP : MCC_VDDmax);
fprintf(file,"vi i 0 PWL(\n") ;

for(t = 0.0 ; t < (5*(float)slope) ; t += ((float)slope/10.0))
  fprintf(file,"+%.5fNS %.5fV\n",t/1000.0,(float)mcc_tanh_slope_tas(t,vt,vinit,vfin,(float)slope)) ;
fprintf(file,"+)\n") ;

 fprintf(file,".option tnom=%g\n", V_FLOAT_TAB[__SIM_TNOM].VALUE);
 fprintf(file,".temp %g\n",MCC_TEMP) ;
 fprintf(file,".print tran v(i) v(f) v(fi)\n") ;
 fprintf(file,".tran %gn %gn\n",V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE*1e9,V_FLOAT_TAB[ __SIM_TIME ].VALUE*1e9) ;
 fprintf(file,".ic v(fi) = 0V\n") ;

 if(V_INT_TAB[__SIM_TOOL].VALUE == SIM_TOOL_TITAN|| V_INT_TAB[__SIM_TOOL].VALUE == SIM_TOOL_TITAN7)
   {
    fprintf(file,".save %s\n", mcc_debug_prefix(name));
   }

 fprintf(file,".end\n");
 fclose(file) ;

 mbkfree(mn) ;
 mbkfree(mp) ;

 sprintf(bufn,"%s.spi", mcc_debug_prefix(name));
 sim_execspice(bufn,0,MCC_SPICENAME,MCC_SPICESTRING,MCC_SPICESTDOUT);

}

void mcc_cal_con_capa(char trs, char con)
{
 char name[128] ;
 double elpcapa ;
 int saveshort, elpgen;
 lofig_list *lf;
 lotrs_list *lt;
 chain_list *cl;
 FILE *file;
 eqt_ctx *mc_ctx=NULL;

 sprintf(name,"inputcapa_%c_%c", trs, con);
 elpgen=ELP_GENPARAM;
 ELP_GENPARAM=1;
 saveshort=V_BOOL_TAB[__MBK_SPI_ZEROVOLT_RESI].VALUE;
 V_BOOL_TAB[__MBK_SPI_ZEROVOLT_RESI].VALUE=1; V_BOOL_TAB[__MBK_SPI_ZEROVOLT_RESI].SET=1;
 parsespice(mcc_debug_prefix(name));
 V_BOOL_TAB[__MBK_SPI_ZEROVOLT_RESI].VALUE=saveshort;
 lf=getloadedlofig(mcc_debug_prefix(name));
 flatten_parameters (lf, NULL, NULL, 0, 0, 0, 0, 0, 0, &mc_ctx);
 if (mc_ctx!=NULL) eqt_term(mc_ctx);
 for (lt=lf->LOTRS; lt!=NULL && lt->TRNAME!=NULL && strstr(lt->TRNAME,"check")==NULL; lt=lt->NEXT) ;
 if (lt!=NULL)
 {
   if (con=='s')  elpcapa=elpLotrsCapaSource(lt,ELP_CAPA_TYPICAL,elpTYPICAL)*1000;
   else if (con=='d')  elpcapa=elpLotrsCapaDrain(lt,ELP_CAPA_TYPICAL,elpTYPICAL)*1000;
   else if (con=='g') elpcapa=elpLotrsInCapa(NULL,lt,ELP_CAPA_TYPICAL,elpTYPICAL,0)*1000;
 }
 else elpcapa=0;
 dellofig(mcc_debug_prefix(name));

 file = mbkfopen(mcc_debug_prefix(name),"dat",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");
 fprintf(file," capa = %g ff\n", elpcapa);
 fprintf(file," Q @ vdd/2 = %g fc\n", elpcapa*(MCC_VDDmax/2));
 fclose(file);
 ELP_GENPARAM=elpgen;
}

/****************************************************************************\
 * Function : mcc_calcul_trans
\****************************************************************************/
void mcc_calcul_trans(lotrsparam_n,lotrsparam_p)
elp_lotrs_param *lotrsparam_n;
elp_lotrs_param *lotrsparam_p;
{
 double ic ;
 double factor[6] ;
 double fvbs[4] ;
 double vbs ;
 double signe ;
 char mode ;
 FILE *file ;
 int stepmax ;
 int stepmin ;
 int i,j,k,l ;
 elp_lotrs_param *trsparam;
 char *name=mcc_debug_prefix("trans");
 mcc_modellist *ptmodeln, *ptmodelp ;

  ptmodeln = mcc_getmodel ( MCC_MODELFILE,
                            MCC_TNMODEL,
                            MCC_NMOS,
                            MCC_NCASE,
                            MCC_LN*1.0e-6,
                            MCC_WN*1.0e-6, 0);
  ptmodelp = mcc_getmodel ( MCC_MODELFILE,
                            MCC_TPMODEL,
                            MCC_PMOS,
                            MCC_PCASE,
                            MCC_LP*1.0e-6,
                            MCC_WP*1.0e-6, 0);
 factor[0] = 1.0 ;
 factor[1] = 3.0/4.0 ;
 factor[2] = 2/3.0 ;
 factor[3] = 1.0/2.0 ;
 factor[4] = 1.0/3.0 ;
 factor[5] = 1.0/4.0 ;

 fvbs[0] = 0 ;
 fvbs[1] = 1.0/8.0 ;
 fvbs[2] = 1.0/6.0 ;
 fvbs[3] = 1.0/4.0 ;

 mode = MCC_CALC_CUR ;
 MCC_CALC_CUR = MCC_CALC_MODE ;

 stepmax = mcc_ftoi((MCC_VDDmax) / (MCC_DC_STEP)) ;
 stepmin = 0 ;

 file = mbkfopen(name,"dat",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");

 fprintf(file,"# vds vgs1n vgs2n vgs3n vgs4n vgs5n vgs6n vgs1p vgs2p vgs3p vgs4p vgs5p vgs6p\n") ;

 for(i = stepmin ; i <= stepmax ; i++)
  {
   fprintf(file,"%g",i*MCC_DC_STEP) ;

   for(l = 0 ; l < 4 ; l++)
   for(j = 0 ; j < 2 ; j++)
     {
      for(k = 1 ; k < 7 ; k++)
       {
        if ( j == MCC_TRANS_N )
         {
          trsparam = lotrsparam_n;
          vbs = MCC_VBULKN ; ;
          signe = -1.0 ;
         }
        else
         {
          trsparam = lotrsparam_p;
          vbs = MCC_VBULKP - MCC_VDDmax ; ;
          signe = 1.0 ;
         }
        ic = mcc_spicecurrent(MCC_VDDmax*factor[k-1],i*MCC_DC_STEP,signe*MCC_VDDmax*fvbs[l]+vbs,j,trsparam) ;
        fprintf(file," %g",ic) ;

        ic = mcc_current(MCC_VDDmax*factor[k-1],i*MCC_DC_STEP,MCC_VDDmax*fvbs[l],j) ;
        fprintf(file," %g",ic) ;
       }
     }
   fprintf(file,"\n") ;
  }

 fclose(file) ;
 file = mbkfopen(name,"plt",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");

 for(l = 0 ; l < 4 ; l++)
 for(i = 0 ; i < 2 ; i++)
  {
   fprintf(file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
   fprintf(file,"set title \"%s\"\n", i==0?"NMOS":"PMOS");
   fprintf(file,"set xlabel \"VDS (V)\"\n") ;
   fprintf(file,"set ylabel \"IDS (A)\"\n") ;
   fprintf(file,"set key top left\n") ;
   fprintf(file,"set xrange [0.0:%g]\n", MCC_VDDmax*1.05);
   fprintf(file,"plot ") ;
   for(k = 1 ; k < 7 ; k++)
    {
     fprintf(file,"'%s.dat' using 1:%d title '%s vgs=%g vbs=%g' with lines,\\\n'%s.dat' using 1:%d title 'elp vgs=%g vbs=%g' with lines%s",name,2*k+i*12+l*24,mcc_getmccname(i==0?ptmodeln:ptmodelp),MCC_VDDmax*factor[k-1],MCC_VDDmax*fvbs[l],name,2*k+1+i*12+l*24,MCC_VDDmax*factor[k-1],MCC_VDDmax*fvbs[l],(k == 6)?"\n":",\\\n") ;
    }
   fprintf(file,"pause -1 'Hit CR to finish'\n") ;
  }

 MCC_CALC_CUR = mode ;
 fclose(file) ;
}

/****************************************************************************\
 * Function : mcc_calcul_transneg
\****************************************************************************/
void mcc_calcul_transneg(lotrsparam_n,lotrsparam_p)
elp_lotrs_param *lotrsparam_n;
elp_lotrs_param *lotrsparam_p;
{
 double ic ;
 double factor[6] ;
 double fvbs[4] ;
 double vbs ;
 double signe ;
 char mode ;
 FILE *file ;
 int stepmax ;
 int stepmin ;
 int i,j,k,l ;
 elp_lotrs_param *trsparam;
 char *name=mcc_debug_prefix("transneg");
 mcc_modellist *ptmodeln, *ptmodelp ;

  ptmodeln = mcc_getmodel ( MCC_MODELFILE,
                            MCC_TNMODEL,
                            MCC_NMOS,
                            MCC_NCASE,
                            MCC_LN*1.0e-6,
                            MCC_WN*1.0e-6, 0);
  ptmodelp = mcc_getmodel ( MCC_MODELFILE,
                            MCC_TPMODEL,
                            MCC_PMOS,
                            MCC_PCASE,
                            MCC_LP*1.0e-6,
                            MCC_WP*1.0e-6, 0);

 factor[0] = 1.0 ;
 factor[1] = 3.0/4.0 ;
 factor[2] = 1.0/2.0 ;

 fvbs[0] = 0 ;
 fvbs[1] = 1.0/8.0 ;
 fvbs[2] = 1.0/6.0 ;
 fvbs[3] = 1.0/4.0 ;

 mode = MCC_CALC_CUR ;
 MCC_CALC_CUR = MCC_CALC_MODE ;

 stepmax = 0 ;
 stepmin = -(mcc_ftoi((MCC_VDDmax/2) / (MCC_DC_STEP))) ;

 file = mbkfopen(name,"dat",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");

 fprintf(file,"# vds vgs1n vgs2n vgs3n vgs4n vgs5n vgs6n vgs1p vgs2p vgs3p vgs4p vgs5p vgs6p\n") ;

 for(i = stepmin ; i <= stepmax ; i++)
  {
   fprintf(file,"%g",i*MCC_DC_STEP) ;

   for(l = 0 ; l < 4 ; l++)
   for(j = 0 ; j < 2 ; j++)
     {
      for(k = 1 ; k < 4 ; k++)
       {
        if ( j == MCC_TRANS_N )
         {
          trsparam = lotrsparam_n;
          vbs = MCC_VBULKN ; ;
          signe = -1.0 ;
         }
        else
         {
          trsparam = lotrsparam_p;
          vbs = MCC_VBULKP - MCC_VDDmax ; ;
          signe = 1.0 ;
         }
        ic = mcc_spicecurrent(MCC_VDDmax*factor[k-1],i*MCC_DC_STEP,signe*MCC_VDDmax*fvbs[l]+vbs,j,trsparam) ;
        fprintf(file," %g",ic) ;

        ic = mcc_current(MCC_VDDmax*factor[k-1],i*MCC_DC_STEP,MCC_VDDmax*fvbs[l],j) ;
        fprintf(file," %g",ic) ;
       }
     }
   fprintf(file,"\n") ;
  }

 fclose(file) ;
 file = mbkfopen(name,"plt",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");

 for(l = 0 ; l < 4 ; l++)
 for(i = 0 ; i < 2 ; i++)
  {
   fprintf(file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
   fprintf(file,"set title \"%s\"\n", i==0?"NMOS":"PMOS");
   fprintf(file,"set xlabel \"VDS (V)\"\n") ;
   fprintf(file,"set ylabel \"IDS (A)\"\n") ;
   fprintf(file,"set key top left\n") ;
   fprintf(file,"set xrange [%d:%d]\n", (int)(stepmin * MCC_DC_STEP) - 1,0);
   fprintf(file,"plot ") ;
   for(k = 1 ; k < 4 ; k++)
    {
     fprintf(file,"'%s.dat' using 1:%d title '%s vgs=%g vbs=%g' with lines,\\\n'%s.dat' using 1:%d title 'elp vgs=%g vbs=%g' with lines%s", name, 2*k+i*6+l*12, mcc_getmccname(i==0?ptmodeln:ptmodelp), MCC_VDDmax*factor[k-1], MCC_VDDmax*fvbs[l], name, 2*k+1+i*6+l*12, MCC_VDDmax*factor[k-1], MCC_VDDmax*fvbs[l], (k == 3)?"\n":",\\\n" ) ;
    }
   fprintf(file,"pause -1 'Hit CR to finish'\n") ;
  }

 MCC_CALC_CUR = mode ;
 fclose(file) ;
}

/****************************************************************************\
 * Function : mcc_calcul_deg
\****************************************************************************/
void mcc_calcul_deg(lotrsparam_n,lotrsparam_p)
elp_lotrs_param *lotrsparam_n;
elp_lotrs_param *lotrsparam_p;
{
 double ic ;
 double ir ;
 double rs ;
 double rc ;
 double rsx ;
 double rcx ;
 double factor[6] ;
 char mode ;
 FILE *file ;
 int stepmax ;
 int stepmin ;
 int i,j,k ;
 elp_lotrs_param *trsparam;
 char *name=mcc_debug_prefix("deg");
 mcc_modellist *ptmodeln,*ptmodelp;
 double vbs ;
 double vbsr ;
 double vbsc ;

 ptmodeln = mcc_getmodel ( MCC_MODELFILE,
                           MCC_TNMODEL,
                           MCC_NMOS,
                           MCC_NCASE,
                           MCC_LN*1.0e-6,
                           MCC_WN*1.0e-6, 0);
 ptmodelp = mcc_getmodel ( MCC_MODELFILE,
                           MCC_TPMODEL,
                           MCC_PMOS,
                           MCC_PCASE,
                           MCC_LP*1.0e-6,
                           MCC_WP*1.0e-6, 0);

 factor[0] = 1.0 ;
 factor[1] = 1.0/3.0 ;
 factor[2] = 1.0/4.0 ;
 factor[3] = 1.0/6.0 ;
 factor[4] = 1.0/8.0 ;
 factor[5] = 0.0 ;

 mode = MCC_CALC_CUR ;
 MCC_CALC_CUR = MCC_CALC_MODE ;

 stepmax = mcc_ftoi((MCC_VDDmax) / (MCC_DC_STEP)) ;
 stepmin = 0 ;

 file = mbkfopen(name,"dat",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");

 fprintf(file,"# vds vgs1n vgs2n vgs3n vgs4n vgs5n vgs6n vgs1p vgs2p vgs3p vgs4p vgs5p vgs6p\n") ;

 for(i = stepmin ; i <= stepmax ; i++)
  {
   fprintf(file,"%g",i*MCC_DC_STEP) ;

   for(j = 0 ; j < 2 ; j++)
     {
      if ( j == MCC_TRANS_N ) {
        trsparam = lotrsparam_n;
        vbs = trsparam->VBULK - i*MCC_DC_STEP ;
      }
      else {
        trsparam = lotrsparam_p;
        vbs = trsparam->VBULK - (MCC_VDDmax - i*MCC_DC_STEP );
      }
      ic = mcc_spicecurrent(MCC_VDDmax-i*MCC_DC_STEP,MCC_VDDmax-i*MCC_DC_STEP,vbs,j,trsparam) ;
      fprintf(file," %g",ic) ;

      ic = mcc_current(MCC_VDDmax-i*MCC_DC_STEP,MCC_VDDmax-i*MCC_DC_STEP,i*MCC_DC_STEP,j) ;
      fprintf(file," %g",ic) ;
     }

   for(k = 0 ; k < 5 ; k++)
   for(j = 0 ; j < 2 ; j++)
     {
      if ( j == MCC_TRANS_N ) {
        trsparam = lotrsparam_n;
        vbs = trsparam->VBULK - i*MCC_DC_STEP ;
      }
      else {
        trsparam = lotrsparam_p;
        vbs = trsparam->VBULK - (MCC_VDDmax - i*MCC_DC_STEP );
      }
      ic = mcc_spicecurrent(MCC_VDDmax-i*MCC_DC_STEP,MCC_VDDmax*factor[k],vbs,j,trsparam) ;
      fprintf(file," %g",ic) ;

      ic = mcc_current(MCC_VDDmax-i*MCC_DC_STEP,MCC_VDDmax*factor[k],i*MCC_DC_STEP,j) ;
      fprintf(file," %g",ic) ;
     }

   for(k = 1 ; k < 6 ; k++)
   for(j = 0 ; j < 2 ; j++)
     {
      if ( j == MCC_TRANS_N ) {
        trsparam = lotrsparam_n;
        vbs = trsparam->VBULK - MCC_VDDmax*factor[k] ;
      }
      else {
        trsparam = lotrsparam_p;
        vbs = trsparam->VBULK - ( MCC_VDDmax - MCC_VDDmax*factor[k] );
      }
      ic = mcc_spicecurrent(MCC_VDDmax-MCC_VDDmax*factor[k],i*MCC_DC_STEP,vbs,j,trsparam) ;
      fprintf(file," %g",ic) ;

      ic = mcc_current(MCC_VDDmax-MCC_VDDmax*factor[k],i*MCC_DC_STEP,MCC_VDDmax*factor[k],j) ;
      fprintf(file," %g",ic) ;
     }

   for(j = 0 ; j < 2 ; j++)
     {
      if ( j == MCC_TRANS_N ) {
        trsparam = lotrsparam_n;
        vbsc = trsparam->VBULK ;
        vbsr = trsparam->VBULK - MCC_VDDmax/2.0 ;
      }
      else {
        trsparam = lotrsparam_p;
        vbsc = trsparam->VBULK - MCC_VDDmax ;
        vbsr = trsparam->VBULK - MCC_VDDmax/2.0 ;
      }

      if(i*MCC_DC_STEP > (MCC_VDDmax/10.0))
        {
         ic = mcc_spicecurrent(MCC_VDDmax,i*MCC_DC_STEP,vbsc,j,trsparam) ;
         ir = mcc_spicecurrent(MCC_VDDmax/2.0,i*MCC_DC_STEP,vbsr,j,trsparam) ;
         rs = fabs((MCC_VDDmax/2.0)/(ic -ir)) ;

         ic = mcc_current(MCC_VDDmax,i*MCC_DC_STEP,0.0,j) ;
         ir = mcc_current(MCC_VDDmax/2.0,i*MCC_DC_STEP,MCC_VDDmax/2.0,j) ;

         rc = fabs((MCC_VDDmax/2.0)/(ic -ir)) ;
        }
       else
        {
         rs = 0.0 ;
         rc = 0.0 ;
        }

       fprintf(file," %g",rs) ;
       fprintf(file," %g",rc) ;
     }

   for(j = 0 ; j < 2 ; j++)
     {
      if ( j == MCC_TRANS_N ) {
        trsparam = lotrsparam_n;
        vbs = trsparam->VBULK - i*MCC_DC_STEP ;
      }
      else {
        trsparam = lotrsparam_p;
        vbs = trsparam->VBULK - ( MCC_VDDmax - i*MCC_DC_STEP );
      }

      if(i*MCC_DC_STEP < (MCC_VDDmax/2.0))
        {
         ic = mcc_spicecurrent(MCC_VDDmax-i*MCC_DC_STEP,MCC_VDDmax-i*MCC_DC_STEP,vbs,j,trsparam) ;
         ir = mcc_spicecurrent(MCC_VDDmax-i*MCC_DC_STEP,(MCC_VDDmax-i*MCC_DC_STEP)/2.0,vbs,j,trsparam) ;
         rsx = fabs(((MCC_VDDmax-i*MCC_DC_STEP)/2.0)/(ic -ir)) ;
         ic = mcc_current(MCC_VDDmax-i*MCC_DC_STEP,MCC_VDDmax-i*MCC_DC_STEP,i*MCC_DC_STEP,j) ;
         ir = mcc_current(MCC_VDDmax-i*MCC_DC_STEP,(MCC_VDDmax-i*MCC_DC_STEP)/2.0,i*MCC_DC_STEP,j) ;
         rcx = fabs(((MCC_VDDmax-i*MCC_DC_STEP)/2.0)/(ic -ir)) ;
        }
      else {
        rsx = 0.0 ;
        rcx = 0.0 ;
      }
       fprintf(file," %g",rsx) ;
       fprintf(file," %g",rcx) ;
     }

   fprintf(file,"\n") ;
  }

 fclose(file) ;
 file = mbkfopen(name,"plt",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");

 for(j = 0 ; j < 2 ; j++)
  {
   fprintf(file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
   fprintf(file,"set title \"%s G, D & B fixed, S vary from 0 to Vdd\"\n", j==0?"NMOS":"PMOS");
   fprintf(file,"set xlabel \"VS (V)\"\n") ;
   fprintf(file,"set ylabel \"IDS (A)\"\n") ;
   fprintf(file,"set key top left\n") ;
   fprintf(file,"set xrange [0.0:%g]\n", MCC_VDDmax*1.05);
   fprintf(file,"plot ") ;
   fprintf(file,"'%s.dat' using 1:%d title '%s' with lines,\\\n'%s.dat' using 1:%d title 'elp' with lines\n",name,2*j+2,mcc_getmccname(j==0?ptmodeln:ptmodelp),name,2*j+3) ;
   fprintf(file,"pause -1 'Hit CR to finish'\n") ;
  }

 for(j = 0 ; j < 2 ; j++)
  {
   fprintf(file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
   fprintf(file,"set title \"%s G & B fixed, S vary from 0 to VDD, VDS fixed\"\n", j==0?"NMOS":"PMOS");
   fprintf(file,"set xlabel \"VS (V)\"\n") ;
   fprintf(file,"set ylabel \"IDS (A)\"\n") ;
   fprintf(file,"set key top right\n") ;
   fprintf(file,"set xrange [0.0:%g]\n", MCC_VDDmax*1.05);
   fprintf(file,"plot ") ;
   for(k = 0 ; k < 5 ; k++)
    {
     fprintf(file,"'%s.dat' using 1:%d title '%s vds=%g' with lines,\\\n'%s.dat' using 1:%d title 'elp vds=%g' with lines", name, 6+j*2+k*4, mcc_getmccname(j==0?ptmodeln:ptmodelp), MCC_VDDmax*factor[k], name,6+j*2+k*4+1, MCC_VDDmax*factor[k] );
     if( k<4)
       fprintf( file, ",\\\n" );
     else
       fprintf( file, "\n" );
    }
   fprintf(file,"pause -1 'Hit CR to finish'\n") ;
  }

 for(j = 0 ; j < 2 ; j++)
  {
   fprintf(file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
   fprintf(file,"set title \"%s G & B fixed, VDS vary from 0 to VDD, VS fixed\"\n", j==0?"NMOS":"PMOS");
   fprintf(file,"set xlabel \"VDS (V)\"\n") ;
   fprintf(file,"set ylabel \"IDS (A)\"\n") ;
   fprintf(file,"set key top right\n") ;
   fprintf(file,"set xrange [0.0:%g]\n", MCC_VDDmax*1.05);
   fprintf(file,"plot ") ;
   for(k = 1 ; k < 6 ; k++)
    {
     fprintf(file,"'%s.dat' using 1:%d title '%s vs=%g' with lines,\\\n'%s.dat' using 1:%d title 'elp vs=%g' with lines", name, 26+j*2+(k-1)*4, mcc_getmccname(j==0?ptmodeln:ptmodelp), MCC_VDDmax*factor[k], name, 26+j*2+(k-1)*4+1, MCC_VDDmax*factor[k] ) ;
     if( k<5)
       fprintf( file, ",\\\n" );
     else
       fprintf( file, "\n" );
    }
   fprintf(file,"pause -1 'Hit CR to finish'\n") ;
  }

 for(j = 0 ; j < 2 ; j++)
  {
   fprintf(file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
   fprintf(file,"set title \"%s VG and VB fixed, VDS vary from 0 to VDD, R computed between VS=0 and VS=VDD/2\"\n", j==0?"NMOS":"PMOS");
   fprintf(file,"set xlabel \"VDS (V)\"\n") ;
   fprintf(file,"set ylabel \"RS (O)\"\n") ;
   fprintf(file,"set key top left\n") ;
   fprintf(file,"set xrange [0.0:%g]\n", MCC_VDDmax*1.05);
   fprintf(file,"plot ") ;
   fprintf(file,"'%s.dat' using 1:%d title '%s' with lines,\\\n'%s.dat' using 1:%d title 'elp' with lines\n", name, 2*j+46, mcc_getmccname(j==0?ptmodeln:ptmodelp), name, 2*j+47 ) ;
   fprintf(file,"pause -1 'Hit CR to finish'\n") ;
  }

 for(j = 0 ; j < 2 ; j++)
  {
   fprintf(file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
   fprintf(file,"set title \"%s VG and VB fixed, VS vary from 0 to VDD, R computed between VD=0 and VD=VDD/2\"\n", j==0?"NMOS":"PMOS");
   fprintf(file,"set xlabel \"VS (V)\"\n") ;
   fprintf(file,"set ylabel \"RT (O)\"\n") ;
   fprintf(file,"set key top left\n") ;
   fprintf(file,"set xrange [0.0:%g]\n", MCC_VDDmax*1.05);
   fprintf(file,"plot ") ;
   fprintf(file,"'%s.dat' using 1:%d title '%s' with lines,\\\n'%s.dat' using 1:%d title 'elp' with lines\n", name, 2*j+50, mcc_getmccname(j==0?ptmodeln:ptmodelp), name,2*j+51 ) ;
   fprintf(file,"pause -1 'Hit CR to finish'\n") ;
  }

 MCC_CALC_CUR = mode ;
 fclose(file) ;
}

/****************************************************************************\
 * Function : mcc_generatetrans
\****************************************************************************/
void mcc_generatetrans(file,gate,input,model,m,w,l,ptrans)
FILE *file ;
char *gate ;
int input ;
char *model ; 
char *m ; 
double w ;
double l ;
int ptrans;
{
 int i ;
 char drain[16] ;
 char source[16] ;
 char flagfile = 'N' ;

 // TODO add specific instances paramaters
 if(file == NULL)
  {
   file = mbkfopen(gate,"spi",WRITE_TEXT) ;
   avt_printExecInfo(file, "*", "", "");
   flagfile = 'Y' ;
  }

 fprintf(file,"****spice description of %s\n",gate);
 fprintf(file,"\n");

 fprintf(file,".subckt %s ",gate);

 for(i = 0 ; i < input ; i ++)
   {
    fprintf(file,"%c ",'a'+ (char)i);
   }

 fprintf(file,"s alim\n");
 fprintf(file,"vds s s_0 0\n");

 for(i = 0 ; i < input ; i++)
  {
   if(i == 1)
     sprintf(drain,"s_%d",i) ;
   else 
     strcpy(drain,source) ;

   if(i == input)
     strcpy(source,"alim") ;
   else 
     sprintf(source,"s_%d",i) ;

   fprintf(file,"%s%d %s %c %s vss %s l=%gu w=%gu %s\n"
             ,m,i,drain,'a'+ i,source,
             model,l,w,ptrans?surf_string_p:surf_string_n);
  }

 fprintf(file,".ends %s\n",gate);
 if(flagfile == 'Y')
   fclose(file) ;
}

/**********************************************************************************\
FUNC : mcc_drive_inv
\**********************************************************************************/
void mcc_drive_inv (file,in,out,bulkn,bulkp,trs_index,
                    modeln,modelp,mn,mp,wn,wp,ln,lp)
FILE *file ;
char *in;
char *out;
char *bulkn;
char *bulkp;
int trs_index;
char *modeln ; 
char *modelp ;
char *mn ; 
char *mp ;
double wn ;
double wp ;
double ln ;
double lp ;
{
 fprintf(file,"%s%d %s %s vdd %s %s l=%gu w=%gu %s\n"
         ,mp,trs_index, out,in,bulkp,modelp,lp,wp,surf_string_p);
 fprintf(file,"%s%d %s %s vss %s %s l=%gu w=%gu %s\n"
         ,mn,trs_index+1, out,in,bulkn,modeln,ln,wn,surf_string_n);
}

/**********************************************************************************\
FUNC : mcc_drive_passtrans_switch 
\**********************************************************************************/
void mcc_drive_passtrans_switch (file,input,passn,switchn,passp,switchp,trs_index,flagfile,
                                 modeln,modelp,mn,mp,wn,wp,ln,lp)
FILE *file ;
int input;
int passn;
int switchn;
int passp;
int switchp;
int *trs_index;
int flagfile;
char *modeln ; 
char *modelp ;
char *mn ; 
char *mp ;
double wn ;
double wp ;
double ln ;
double lp ;
{
 int i,nbitem;
 char drain[16], in[2] ;
 char source[16] ;
 char cmd[16] ;

 if ( passn || passp )
   nbitem = ( passn > passp ) ? passn : passp;
 else
   nbitem = ( switchn > switchp ) ? switchn : switchp;
 // => drive the pass trans
 for(i = 1 ; i <= nbitem ; i++)
  {
   if ( flagfile == 'Y' ) // for tas
    {
     if ( i == 1 ) 
       {
        strcpy(drain,"si") ;
       }
     else
       strcpy(drain,source) ;
     if( i == nbitem )
       strcpy(source,"s") ;
     else
       sprintf(source,"spass_%d",i) ;
    }
   else // for spice
    {
     if(i == 1 ) 
       {
        fprintf(file,"vpass si sii 0\n");
        sprintf(drain,"sii") ;
       }
     else 
       strcpy(drain,source) ;
     
     if(i == nbitem)
       strcpy(source,"s") ;
     else 
       sprintf(source,"spass_%d",i) ;
    }
 
   if ( passp || switchp )
    {
     fprintf(file,"%s%d %s %c %s vbp %s l=%gu w=%gu %s\n"
             ,mp,*trs_index+i-1,drain,'a'+input+i-2,source,
             modelp,lp,wp,surf_string_p);
     *trs_index += 1 ;
     if ( switchp )
      {
       in[0] = (char)('a'+input+i-2);
       in[1] = '\0';
       sprintf (cmd,"cmd_%d",i);
       fprintf(file,"%s%d %s %s %s vbn %s l=%gu w=%gu %s\n"
               ,mn,*trs_index+i-1,drain,cmd,source,
               modeln,ln,wn,surf_string_n);
       *trs_index += 1 ;
       mcc_drive_inv (file,in,cmd,"vbn","vbp",*trs_index,
                      modeln,modelp,mn,mp,wn,wp,ln,lp);
       *trs_index += 2 ;
      }
    }
   else
    {
     fprintf(file,"%s%d %s %c %s vbn %s l=%gu w=%gu %s\n"
             ,mn,*trs_index+i-1,drain,'a'+input+i-2,source,
             modeln,ln,wn,surf_string_n);
     *trs_index += 1 ;
     if ( switchn )
      {
       in[0] = (char)('a'+input+i-2);
       in[1] = '\0';
       sprintf (cmd,"cmd_%d",i);
       fprintf(file,"%s%d %s %s %s vbp %s l=%gu w=%gu %s\n"
               ,mp,*trs_index+i-1,drain,cmd,source,
               modelp,lp,wp,surf_string_p);
       *trs_index += 1 ;
       mcc_drive_inv (file,in,cmd,"vbn","vbp",*trs_index,
                      modeln,modelp,mn,mp,wn,wp,ln,lp);
       *trs_index += 2 ;
      }
    }
  }
}

/**********************************************************************************\
FUNC : mcc_generategate
\**********************************************************************************/
void mcc_generategate (file,gate,passn,switchn,passp,switchp,bleeder,
                       input,up,down,modeln,modelp,mn,mp,wn,wp,ln,lp)
FILE *file ;
char *gate ;
int passn ;
int switchn ;
int passp ;
int switchp ;
int bleeder ;
int input ;
int up ;
int down ;
char *modeln ; 
char *modelp ;
char *mn ; 
char *mp ;
double wn ;
double wp ;
double ln ;
double lp ;
{
 int i,j,k ;
 int nbtrans ;
 int nb_switch_or_pass=0;
 char drain[16] ;
 char source[16] ;
 char flagfile = 'N' ; // if 'N' : file to be simulated by spice

 if(file == NULL)
  {
   file = mbkfopen(gate,"spi",WRITE_TEXT) ;
   avt_printExecInfo(file, "*", "", "");
   flagfile = 'Y' ;
  }

 /*
 if ( pass || switchs )
   {
     if ( pass >= switchs )
       nb_switch_or_pass = pass;
     else
       nb_switch_or_pass = switchs;
   }*/

 if ( passn || passp )
   nb_switch_or_pass = ( passn > passp ) ? passn : passp;
 else if ( switchn || switchp )
   nb_switch_or_pass = ( switchn > switchp ) ? switchn : switchp;

 fprintf(file,"****spice description of %s\n",gate);
 fprintf(file,"\n");

 fprintf(file,".subckt %s ",gate);

 for(i = 0 ; i < input ; i ++)
   {
    fprintf(file,"%c ",'a'+ (char)i);
   }

 fprintf(file,"s vbn vbp vdd vss\n");

 if ( !nb_switch_or_pass )
   {
    nbtrans = input / down ;
   }
 else
   {
    nbtrans = (input - nb_switch_or_pass) / down ;
   }
 k = 0 ;
 for(i = 0 ; i < down ; i++)
  {
   if ( flagfile == 'N' )
     {
      if ( !nb_switch_or_pass )
        fprintf(file,"vd%d s sd_%d 0\n",i,i);
      else
        fprintf(file,"vd%d si sd_%d 0\n",i,i);
     }
   for(j = 1 ; j <= nbtrans ; j++)
    {
     if ( flagfile == 'Y' )
      {
       if ( j == 1 ) {
         if ( !nb_switch_or_pass )
           strcpy(drain,"s") ;
         else
           strcpy(drain,"si") ;
       }
       else
         strcpy(drain,source) ;
       if(j == nbtrans)
         strcpy(source,"vss") ;
       else
         sprintf(source,"sd_%d_%d",i,j) ;
      }
     else
      {
       if(j == 1)
         sprintf(drain,"sd_%d",i) ;
       else 
         strcpy(drain,source) ;
       
       if(j == nbtrans)
         strcpy(source,"vss") ;
       else 
         sprintf(source,"sd_%d_%d",i,j) ;
      }

     fprintf(file,"%s%d %s %c %s vbn %s l=%gu w=%gu %s\n"
             ,mn,k,drain,'a'+ k,source,
             modeln,ln,wn,surf_string_n);
     k++ ;
    }
  }

 if ( !nb_switch_or_pass )
   {
    nbtrans = input / up ;
   }
 else
   {
    nbtrans = (input - nb_switch_or_pass) / up ;
   }
 k = 0 ;
 for(i = 0 ; i < up ; i++)
  {
   if ( flagfile == 'N' )
     {
      if ( !nb_switch_or_pass ) 
        fprintf(file,"vu%d su_%d s 0\n",i,i);
      else
        fprintf(file,"vu%d su_%d si 0\n",i,i);
     }
   for(j = 1 ; j <= nbtrans ; j++)
    {
     if ( flagfile == 'Y' )
      {
       if ( j == 1 ) {
         if ( !nb_switch_or_pass )
           strcpy(drain,"s") ;
         else
           strcpy(drain,"si") ;
       }
       else
         strcpy(drain,source) ;
       if(j == nbtrans)
         strcpy(source,"vdd") ;
       else
         sprintf(source,"su_%d_%d",i,j) ;
      }
     else
      {
       if(j == 1)
         sprintf(drain,"su_%d",i) ;
       else 
         strcpy(drain,source) ;
       
       if(j == nbtrans)
         strcpy(source,"vdd") ;
       else 
         sprintf(source,"su_%d_%d",i,j) ;
      }

     fprintf(file,"%s%d %s %c %s vbp %s l=%gu w=%gu %s\n"
             ,mp,k+input-nb_switch_or_pass,drain,'a'+ k,source,
             modelp,lp,wp,surf_string_p);
     k++ ;
    }
 }

 if ( nb_switch_or_pass )
   {
    k += input-nb_switch_or_pass;
    // => drive the pass trans / switch
    mcc_drive_passtrans_switch (file,input,passn,switchn,passp,switchp, &k, flagfile,
                                modeln,modelp,mn,mp,wn,wp,ln,lp);
    // => drive the inv ( last gate )
    mcc_drive_inv (file,"s","so","vbn","vbp",k,
                   modeln,modelp,mn,mp,wn,wp,ln,lp);
    // => drive the bleeder
    if ( bleeder )
      mcc_drive_inv (file,"so","s","vbn","vbp",k+2,
                     modeln,modelp,mn,mp,wn/3.0,wp/3.0,2.0*ln,2.0*lp);
   }

 fprintf(file,"vbulkn vbn vss %g\n",(MCC_VBULKN > ELPMINVBULK) ? MCC_VBULKN : 0.0);
 fprintf(file,"vbulkp vbp vss %g\n",(MCC_VBULKP > ELPMINVBULK) ? MCC_VBULKP : MCC_VDDmax);
 if (flagfile=='Y')
 {
   fprintf(file,"vvdd vdd 0 %gv\n",MCC_VDDmax);
   fprintf(file,"vvss vss 0 0v\n");
 }
 fprintf(file,".ends %s\n",gate);

 if(flagfile == 'Y')
   fclose(file) ;
}

/**********************************************************************************\
FUNC : mcc_generatesimgate
\**********************************************************************************/
        
        
double mcc_generatesimgate (gate,passn,switchn,passp,switchp,bleeder,
                            up,down,input,inputs,in,slope,capa,other,
                            modeln,modelp,mn,mp,wn,wp,ln,lp)
char *gate ;
int passn;
int switchn ;
int passp;
int switchp ;
int bleeder ;
int up ;
int down ;
int input ;
int inputs ;
int in ;
int slope ;
int capa ;
int other ;
char *modeln ; 
char *modelp ;
char *mn ; 
char *mp ;
double wn ;
double wp ;
double ln ;
double lp ;
{
 FILE *file ;
 FILE *resfile ;
 int i ;
 int j ;
 int out ;
 char buf[128], buf1[128] ;
 char name[128] ;
 float t ;
 float vt ;
 float vinit ;
 float vfin ;
 char *argv[1024] ;
 char arg[1024] ;
 char *fileout ;
 double **tab ;
 double step ;
 double delay ;
 double start ;
 double tend ;
 double sslope ;
 double error ;
 double serror ;
 double capaconf ;
 double imoy ;
 double capaconfold ;
 double imoyold ;
 double sfl ;
 double sfh ;
 double cmd_value; // value to enable the pass trans or switch
 double tdelay ;
 double tslope ;
 int type ;
 int level ;
 char *str ;
 int tstart ;
 int mes ;
 int mesold ;
 int nbdata=4;
 int intrans;

   level = V_INT_TAB[__TAS_SIMULATION_LEVEL].VALUE ;

 switch ( in )
  {
   case MCC_UD : out = 0 ;
                 vt = (float)MCC_VTN ;
                 vinit = 0.0 ;
                 vfin = (float)MCC_VDDmax ;
                 intrans = 1;
                 break;
   case MCC_DU : out = 1 ;
                 vt = (float)MCC_VTP ;
                 vinit = (float)MCC_VDDmax ;
                 vfin = 0.0 ;
                 intrans = 0;
                 break;
   case MCC_DD : out = 0 ;
                 vt = (float)MCC_VTP ;
                 vinit = (float)MCC_VDDmax ;
                 vfin = 0.0 ;
                 intrans = 0;
                 break;
   case MCC_UU : out = 1 ;
                 vt = (float)MCC_VTN ;
                 vinit = 0.0;
                 vfin = (float)MCC_VDDmax ;
                 intrans = 1;
                 break;
   default     : avt_errmsg(MCC_ERRMSG, "000", AVT_WARNING);
                 return 0.0;
  }

 sprintf(buf,"f%d_%c%ds%d_%s_c%d",slope,'a'+input,intrans,out,gate,capa) ;
 sprintf(buf1,"%s_%s",gate,buf) ;

 file = mbkfopen(buf,"spi",WRITE_TEXT) ;
 avt_printExecInfo(file, "*", "", "");

 resfile = fopen(mcc_debug_prefix("benchresult"),"a") ;

 fprintf(file,"****spice simulation of %s\n",gate);
 fprintf(file,"\n");

 if(MCC_SPICEOPTIONS != NULL)
   fprintf(file,".option %s\n",MCC_SPICEOPTIONS);
 fprintf(file,".include \"%s\"\n",MCC_TECHFILE);
 fprintf(file,"\n");

 mcc_generategate (file,gate,passn,switchn,passp,switchp,bleeder,
                   inputs,up,down,modeln,modelp,mn,mp,wn,wp,ln,lp) ;

 fprintf(file,"\n");

 fprintf(file,"x%s ",gate) ;

 for(i = 0 ; i < inputs ; i++)
   fprintf(file,"%c ",'a'+i) ;

 fprintf(file,"s vbn vbp vdd 0 %s\n",gate);
 fprintf(file,"\n");

 fprintf(file,"valim vdd 0 %gV\n",MCC_VDDmax);

 for(i = 0 ; i < inputs ; i++)
  if(i != input)
   {
    if ( ( passn || passp || switchn || switchp ) && ( i == inputs -1 ) )
     {
      if ( passn || switchn )
        cmd_value = MCC_VDDmax;
      else
        cmd_value = 0.0;
      fprintf(file,"v%c %c 0 %gV\n",'a'+i,'a'+i,cmd_value) ;
     }
    else
     {
      fprintf(file,"v%c %c 0 %gV\n",'a'+i,'a'+i,(other == 0)? 0.0:MCC_VDDmax) ;
     }
   }


fprintf(file,"v%c %c 0 PWL(\n",'a'+input,'a'+input) ;
for(t = 0.0 ; t < (5*(float)slope) ; t += ((float)slope/10.0))
  fprintf(file,"+%.5fNS %.5fV\n",t/1000.0,mcc_tanh_slope_tas(t,vt,vinit,vfin,(float)slope)) ;
//  fprintf(file,"+%.5fNS %.5fV\n",t/1000.0,(float)stm_get_v(t,vt,vinit,vfin,(float)slope)) ;
fprintf(file,"+)\n") ;

 fprintf(file,"cs s 0 %dfF\n",capa);

 fprintf(file,".option tnom=%g\n", V_FLOAT_TAB[__SIM_TNOM].VALUE);
 fprintf(file,".temp %g\n",MCC_TEMP) ;
 fprintf(file,".tran %gn %gn\n",V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE*1e9,V_FLOAT_TAB[ __SIM_TIME ].VALUE*1e9) ;
 fprintf(file,".ic v(s) = %gV\n",(out==0)?MCC_VDDmax:0.0) ;
 fprintf(file,".print tran ") ;

 for(i = 0 ; i < inputs ; i++)
  if(i == input)
   {
    fprintf(file,"v(%c) ",'a'+i) ;
    sprintf(arg,"%c",'a'+i) ;
    argv[0] = mcc_initstr(arg) ;
   }

 fprintf(file,"v(s)\n");
 argv[1] = mcc_initstr("s") ;

 fprintf(file,".print tran ") ;

 if ( !passn && !passp && !switchn && !switchp )
 for(i = 0 ; i < up ; i++)
  {
   if((i == input) || (up == 1))
    {
     if ( V_INT_TAB[__SIM_TOOL].VALUE == SIM_TOOL_TITAN || V_INT_TAB[__SIM_TOOL].VALUE == SIM_TOOL_TITAN7)
      {
       fprintf(file,"i(vu%d.x%s) ",i,gate) ;
       sprintf(arg,"vu%d.x%s",i,gate) ;
      }
     else
      {
       fprintf(file,"i(x%s.vu%d) ",gate,i) ;
       sprintf(arg,"x%s.vu%d",gate,i) ;
      }
     argv[2] = mcc_initstr(arg) ;
    }
  }

 if ( !passn && !passp && !switchn && !switchp )
 for(i = 0 ; i < down ; i++)
  {
   if((i == input) || (down == 1))
    {
     if ( V_INT_TAB[__SIM_TOOL].VALUE == SIM_TOOL_TITAN || V_INT_TAB[__SIM_TOOL].VALUE == SIM_TOOL_TITAN7)
      {
       fprintf(file,"i(vd%d.x%s) ",i,gate) ;
       sprintf(arg,"vd%d.x%s",i,gate) ;
      }
     else
      {
       fprintf(file,"i(x%s.vd%d) ",gate,i) ;
       sprintf(arg,"x%s.vd%d",gate,i) ;
      }
     argv[3] = mcc_initstr(arg) ;
    }
  }

 if ( passn || passp || switchn || switchp )
  {
   if ( V_INT_TAB[__SIM_TOOL].VALUE == SIM_TOOL_TITAN || V_INT_TAB[__SIM_TOOL].VALUE == SIM_TOOL_TITAN7)
    {
     fprintf(file,"v(si.x%s) ",gate) ;
     sprintf(arg,"si.x%s",gate) ;
     argv[2] = mcc_initstr(arg) ;
     fprintf(file,"i(vpass.x%s) ",gate) ;
     sprintf(arg,"vpass.x%s",gate) ;
     argv[3] = mcc_initstr(arg) ;
    }
   else
    {
     fprintf(file,"v(x%s.si) ",gate) ;
     sprintf(arg,"x%s.si",gate) ;
     argv[2] = mcc_initstr(arg) ;
     fprintf(file,"i(x%s.vpass) ",gate) ;
     sprintf(arg,"x%s.vpass",gate) ;
     argv[3] = mcc_initstr(arg) ;
    }
  }

 fprintf(file,"\n\n");

 if ( V_INT_TAB[__SIM_TOOL].VALUE == SIM_TOOL_TITAN || V_INT_TAB[__SIM_TOOL].VALUE == SIM_TOOL_TITAN7)
  {
   fprintf(file,".save %s\n",buf);
  }
 fprintf(file,".end\n");
 fclose(file) ;

 sim_set_result_file_extension('p', 0, &MCC_SPICEOUT);
 
 sprintf(name,"%s.spi",buf) ;
 sim_execspice(name,0,MCC_SPICENAME,MCC_SPICESTRING,MCC_SPICESTDOUT);
// mcc_runtas(gate,slope,capa) ;
 mcc_runtas_tcl(gate,slope,capa,buf1,NULL) ;
 fileout = sim_getjoker(MCC_SPICEOUT,buf) ;

 tab = (double **)mbkalloc(nbdata * sizeof(double *)) ;
 for(i = 0 ; i < nbdata ; i++)
   tab[i] = (double *)mbkalloc((mcc_ftoi(V_FLOAT_TAB[ __SIM_TIME ].VALUE*1e9/V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE*1e9)+1) *sizeof(double)) ;

 sim_readspifiletab (fileout,argv,nbdata,mcc_ftoi(V_FLOAT_TAB[ __SIM_TIME ].VALUE*1e9/V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE*1e9)+1,tab,V_FLOAT_TAB[ __SIM_TIME ].VALUE,V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE) ;

 sprintf(name,"%c",'a'+input) ;
 mcc_getdelay(buf1,name,"s",1,(intrans==0)?'D':'U',(out==0)?'D':'U',NULL) ;

 switch ( in )
  {
   case MCC_UD : tdelay = (double)MCC_TASUD;
                 tslope = (double)MCC_TASSUD;
                 break;
   case MCC_DU : tdelay = (double)MCC_TASDU;
                 tslope = (double)MCC_TASSDU;
                 break;
   case MCC_DD : tdelay = (double)MCC_TASDD;
                 tslope = (double)MCC_TASSDD;
                 break;
   case MCC_UU : tdelay = (double)MCC_TASUU;
                 tslope = (double)MCC_TASSUU;
                 break;
  }

 if(out == 0)
  {
   sfl = (MCC_VDDmax - MCC_VTN)/4.0 ;
   sfh = (MCC_VDDmax - MCC_VTN) ;
  }
 else
  {
   sfl = MCC_VTP ;
   sfh =  (3.0*MCC_VDDmax + MCC_VTP)/4.0 ;
  }

 delay = 0 ;
 start = sim_calcdelayslope(tab[0],mcc_ftoi(V_FLOAT_TAB[ __SIM_TIME ].VALUE*1e9/V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE*1e9)+1,
                    2,MCC_VDDmax/2.0,sfl,sfh,-1.0,
                    &delay,&sslope,&type,V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE*1e9,NULL,NULL) ;
 sim_calcdelayslope(tab[1],mcc_ftoi(V_FLOAT_TAB[ __SIM_TIME ].VALUE*1e9/V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE*1e9)+1,
                    2,MCC_VDDmax/2.0,sfl,sfh,start,
                    &delay,&sslope,&type,V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE*1e9,NULL,NULL) ;

 delay = delay * 1000.0 ;
 sslope = sslope * 1000.0 ;
 start = start * 1000.0 ;

 if(tdelay > delay)
   error=(double)(((double)tdelay - delay) / (double)(tdelay)) ;
 else
   error=(double)(((double)tdelay - delay) / (double)(delay)) ;

 if(tslope > sslope)
   serror=(double)(((double)tslope - sslope) / (double)(tslope)) ;
 else
   serror=(double)(((double)tslope - sslope) / (double)(sslope)) ;

 error = error*100.0 ;
 serror = serror*100.0 ;

 if(fabs(error) < 0.01)
   error = 0.0 ;

 file = mbkfopen(buf,"dat",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");

 fprintf(file,"#time ") ;

 for(i = 0 ; i < nbdata ; i ++)
  {
   fprintf(file,"%s ",*(argv + i)) ;
  }
 fprintf(file,"\n") ;

 tstart = 0 ;
 mes = 0 ;
 capaconf = 0.0 ;
 mesold = 0 ;
 capaconfold = 0.0 ;

 for(j = 0 ; j < mcc_ftoi(V_FLOAT_TAB[ __SIM_TIME ].VALUE*1e9/V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE*1e9) + 1 ; j ++)
  {
   if(intrans == 1)
    {
     if(((double)j * V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE * 1e12) < (start+delay))
       {
        capaconf += fabs(tab[2][j]) ;
        mes++ ;
       }
     if((tab[0][j] > MCC_VTN) && 
        (((double)j * V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE * 1e12) < (start+delay)))
       {
        capaconfold += fabs(tab[2][j]) ;
        mesold++ ;
        if(tstart == 0)
         tstart = j ;
       }
    }
   else
    {
     if(((double)j * V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE * 1e12) < (start+delay))
      {
       capaconf += fabs(tab[3][j]) ;
       mes++ ;
      }
     if((tab[0][j] < (MCC_VDDmax - MCC_VTP)) && 
        (((double)j * V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE * 1e12) < (start+delay)))
       {
        capaconfold += fabs(tab[3][j]) ;
        mesold++ ;
        if(tstart == 0)
         tstart = j ;
       }
    }
  }

 imoyold = capaconfold / (double)mesold ;
 capaconfold = imoyold * ((start+delay) - ((double)tstart * V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE * 1e12)) ;
 capaconfold = capaconfold / (MCC_VDDmax/2.0) ;

 imoy = capaconf / (double)mes ;
 capaconf = imoy * ((start+delay) /*- ((double)tstart * V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE * 1e12)*/) ;
 capaconf = capaconf / (MCC_VDDmax/2.0) ;

 if(intrans == 1)
   capaconf -= (MCC_WPeff * MCC_DIF * MCC_CDSP +  MCC_WPeff * MCC_CDWP +  2.0*(MCC_DIF + MCC_WPeff) * MCC_CDPP) ;
 else
   capaconf -= (MCC_WNeff * MCC_DIF * MCC_CDSN +  MCC_WNeff * MCC_CDWN +  2.0*(MCC_DIF + MCC_WNeff) * MCC_CDPN) ;

 imoy = (capaconf * (MCC_VDDmax/2.0)) / ((start+delay) - ((double)tstart * V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE * 1e12)) ;

 fprintf(stdout,"%s : spice=%g capaconf=%g tas=%g error = %g\n",buf,delay,capaconf,tdelay,error) ;
 fprintf(stdout,"%s : slopespice=%g slopetas=%g sfl = %g sfh = %g error = %g\n",buf,sslope,tslope,sfl,sfh,serror) ;
 fprintf(resfile,"%s : spice=%g capaconf=%g tas=%g error = %.3g\n",buf,delay,capaconf,tdelay,error) ;
 fprintf(resfile,"%s : slopespice=%g slopetas=%g sfl = %g sfh = %g error = %g\n",buf,sslope,tslope,sfl,sfh,serror) ;
 fclose(resfile) ;


 step = 0.0 ;
 tend = 0.0 ;

 for(j = 0 ; j < mcc_ftoi(V_FLOAT_TAB[ __SIM_TIME ].VALUE*1e9/V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE*1e9) + 1 ; j ++)
  {
   fprintf(file,"%.3e ",step) ;
   for(i = 0 ; i < nbdata ; i ++)
    {
       fprintf(file,"%.3e ",tab[i][j]) ;
    }
   if((j > tstart) && (((double)j * V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE * 1e12) < (start+delay)))
      fprintf(file,"%.3e ",imoy) ;
   else
      fprintf(file,"%.3e ",0.0) ;
   fprintf(file,"\n") ;

   if((((double)j * V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE * 1e12) > (start+delay)) && (tend == 0.0))
     {
      if(intrans == 1)
       {
        if((tab[0][j] > MCC_VDDmax * 0.95) && (tab[1][j] < MCC_VDDmax * 0.05))
         tend = (double)j * V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE ;
       }
      else
       {
        if((tab[1][j] > MCC_VDDmax * 0.95) && (tab[0][j] < MCC_VDDmax * 0.05))
         tend = (double)j * V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE ;
       }
     }

   step += (V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE) ;
  }

 if(tend == 0.0)
   tend = V_FLOAT_TAB[ __SIM_TIME ].VALUE ;

 fclose(file) ;

 for(i = 0 ; i < nbdata ; i++)
  {
   mbkfree(argv[i]) ;
  }

 for(i = 0 ; i < nbdata ; i++)
   mbkfree(tab[i]) ;

 mbkfree(tab) ;
 mbkfree(fileout) ;

 file = mbkfopen(buf,"plt",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");

 fprintf(file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
 fprintf(file,"set xlabel \"time (ps)\"\n") ;
 fprintf(file,"set ylabel \"voltagle (V)\"\n") ;
 fprintf(file,"set key top left\n") ;
 fprintf(file,"set xrange [0.0:%g]\n", tend);
 fprintf(file,"plot '%s.dat' using 1:2 title 'v(%c)'  with lines, '%s.dat' using 1:3 title 'v(s) spice' with lines, '%s_l%d.dat' using 1:3 title 'v(s) mcc' with lines\n",buf,'a'+input,buf,buf,level) ;
 fprintf(file,"pause -1 'Hit CR to finish'\n") ;
 fprintf(file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
 fprintf(file,"set xlabel \"time (ps)\"\n") ;
 fprintf(file,"set ylabel \"current (A)\"\n") ;
 fprintf(file,"set xrange [0.0:%g]\n", tend);
 fprintf(file,"plot '%s.dat' using 1:4 title 'Iup spice'  with lines, '%s.dat' using 1:5 title 'Idown spice' with lines, '%s.dat' using 1:6 title 'Imoyconf spice' with lines, '%s_l%d.dat' using 1:4 title 'Iup mcc'  with lines, '%s_l%d.dat' using 1:5 title 'Idown mcc' with lines\n",buf,buf,buf,buf,level,buf,level) ;
 fprintf(file,"pause -1 'Hit CR to finish'\n") ;

 /*
 if ( pass || switchs ) 
  {
   print v(si) and i(vpass)
  }
  */

 fclose(file) ;

 return(error) ;
}

/**********************************************************************************\
FUNC : mcc_init_gate
\**********************************************************************************/
void mcc_init_gate ( char **gate , int *input, int *up, int *down, int *other, 
                     int *passn, int *passp, int *switchn, int *switchp, int *bleeder )
{
  gate[0] = "inv" ; input[0] = 1 ; up[0] = 1 ; down[0] = 1 ;  other[0] = 1 ; passn[0] = 0 ; switchn[0] = 0 ; passp[0] = 0 ; switchp[0] = 0 ; bleeder[0] = 0 ;
  gate[1] = "na2" ; input[1] = 2 ; up[1] = 2 ; down[1] = 1 ;  other[1] = 1 ; passn[1] = 0 ; switchn[1] = 0 ; passp[1] = 0 ; switchp[1] = 0 ; bleeder[1] = 0 ;
  gate[2] = "na3" ; input[2] = 3 ; up[2] = 3 ; down[2] = 1 ;  other[2] = 1 ; passn[2] = 0 ; switchn[2] = 0 ; passp[2] = 0 ; switchp[2] = 0 ; bleeder[2] = 0 ;
  gate[3] = "na4" ; input[3] = 4 ; up[3] = 4 ; down[3] = 1 ;  other[3] = 1 ; passn[3] = 0 ; switchn[3] = 0 ; passp[3] = 0 ; switchp[3] = 0 ; bleeder[3] = 0 ;
  gate[4] = "no2" ; input[4] = 2 ; up[4] = 1 ; down[4] = 2 ;  other[4] = 0 ; passn[4] = 0 ; switchn[4] = 0 ; passp[4] = 0 ; switchp[4] = 0 ; bleeder[4] = 0 ;
  gate[5] = "no3" ; input[5] = 3 ; up[5] = 1 ; down[5] = 3 ;  other[5] = 0 ; passn[5] = 0 ; switchn[5] = 0 ; passp[5] = 0 ; switchp[5] = 0 ; bleeder[5] = 0 ;
  gate[6] = "no4" ; input[6] = 4 ; up[6] = 1 ; down[6] = 4 ;  other[6] = 0 ; passn[6] = 0 ; switchn[6] = 0 ; passp[6] = 0 ; switchp[6] = 0 ; bleeder[6] = 0 ;
 
    //------------------------
   // inverseur + passn trans
  //-------------------------
  gate[7]  = "inv_pn"       ; input[7] = 2 ; up[7] = 1 ; down[7] = 1 ;  other[7] = 1 ; passn[7] = 1 ; switchn[7] = 0 ; passp[7] = 0 ; switchp[7] = 0 ; bleeder[7] = 0 ;
  gate[8]  = "inv_pp"       ; input[8] = 2 ; up[8] = 1 ; down[8] = 1 ;  other[8] = 0 ; passn[8] = 0 ; switchn[8] = 0 ; passp[8] = 1 ; switchp[8] = 0 ; bleeder[8] = 0 ;
  gate[9]  = "inv_pn_bleed" ; input[9] = 2 ; up[9] = 1 ; down[9] = 1 ;  other[9] = 1 ; passn[9] = 1 ; switchn[9] = 0 ; passp[9] = 0 ; switchp[9] = 0 ; bleeder[9] = 1 ;
  gate[10] = "inv_pp_bleed" ; input[10] = 2 ; up[10] = 1 ; down[10] = 1 ;  other[10] = 0 ; passn[10] = 0 ; switchn[10] = 0 ;  passp[10] = 1 ; switchp[10] = 0 ;bleeder[10] = 1 ;
  
    //------------------------
   // inverseur + switch
  //-------------------------
  gate[11] = "inv_sn"       ; input[11] = 2 ; up[11] = 1 ; down[11] = 1 ;  other[11] = 1 ; passn[11] = 0 ; switchn[11] = 1 ; passp[11] = 0 ; switchp[11] = 0 ; bleeder[11] = 0 ;
  gate[12] = "inv_sp"       ; input[12] = 2 ; up[12] = 1 ; down[12] = 1 ;  other[12] = 0 ; passn[12] = 0 ; switchn[12] = 0 ; passp[12] = 0 ; switchp[12] = 1 ; bleeder[12] = 0 ;
  gate[13] = "inv_sn_bleed" ; input[13] = 2 ; up[13] = 1 ; down[13] = 1 ;  other[13] = 1 ; passn[13] = 0 ; switchn[13] = 1 ; passp[13] = 0 ; switchp[13] = 0 ; bleeder[13] = 1 ;
  gate[14] = "inv_sp_bleed" ; input[14] = 2 ; up[14] = 1 ; down[14] = 1 ;  other[14] = 1 ; passn[14] = 0 ; switchn[14] = 0 ; passp[14] = 0 ; switchp[14] = 1 ; bleeder[14] = 1 ;

    //------------------------
   // na2 + passn trans
  //-------------------------
  gate[15]  = "na2_pn"       ; input[15] = 3 ; up[15] = 2 ; down[15] = 1 ;  other[15] = 1 ; passn[15] = 1 ; switchn[15] = 0 ; passp[15] = 0 ; switchp[15] = 0 ; bleeder[15] = 0 ;
  gate[16]  = "na2_pp"       ; input[16] = 3 ; up[16] = 2 ; down[16] = 1 ;  other[16] = 1 ; passn[16] = 0 ; switchn[16] = 0 ; passp[16] = 1 ; switchp[16] = 0 ; bleeder[16] = 0 ;
  gate[17]  = "na2_pn_bleed" ; input[17] = 3 ; up[17] = 2 ; down[17] = 1 ;  other[17] = 1 ; passn[17] = 1 ; switchn[17] = 0 ; passp[17] = 0 ; switchp[17] = 0 ; bleeder[17] = 1 ;
  gate[18]  = "na2_pp_bleed" ; input[18] = 3 ; up[18] = 2 ; down[18] = 1 ;  other[18] = 1 ; passn[18] = 0 ; switchn[18] = 0 ; passp[18] = 1 ; switchp[18] = 0 ; bleeder[18] = 1 ;

    //------------------------
   // na2 + switch
  //-------------------------
  gate[19]  = "na2_sn"       ; input[19] = 3 ; up[19] = 2 ; down[19] = 1 ;  other[19] = 1 ; passn[19] = 0 ; switchn[19] = 1 ; passp[19] = 0 ; switchp[19] = 0 ; bleeder[19] = 0 ;
  gate[20]  = "na2_sp"       ; input[20] = 3 ; up[20] = 2 ; down[20] = 1 ;  other[20] = 1 ; passn[20] = 0 ; switchn[20] = 0 ; passp[20] = 0 ; switchp[20] = 1 ; bleeder[20] = 0 ;
  gate[21]  = "na2_sn_bleed" ; input[21] = 3 ; up[21] = 2 ; down[21] = 1 ;  other[21] = 1 ; passn[21] = 0 ; switchn[21] = 1 ; passp[21] = 0 ; switchp[21] = 0 ; bleeder[21] = 1 ;
  gate[22]  = "na2_sp_bleed" ; input[22] = 3 ; up[22] = 2 ; down[22] = 1 ;  other[22] = 1 ; passn[22] = 0 ; switchn[22] = 0 ; passp[22] = 0 ; switchp[22] = 1 ; bleeder[22] = 1 ;

    //------------------------
   // na3 + passn trans
  //-------------------------
  gate[23]  = "na3_pn"       ; input[23] = 4 ; up[23] = 3 ; down[23] = 1 ;  other[23] = 1 ; passn[23] = 1 ; switchn[23] = 0 ; passp[23] = 0 ; switchp[23] = 0 ; bleeder[23] = 0 ;
  gate[24]  = "na3_pp"       ; input[24] = 4 ; up[24] = 3 ; down[24] = 1 ;  other[24] = 1 ; passn[24] = 0 ; switchn[24] = 0 ; passp[24] = 1 ; switchp[24] = 0 ; bleeder[24] = 0 ;
  gate[25]  = "na3_pn_bleed" ; input[25] = 4 ; up[25] = 3 ; down[25] = 1 ;  other[25] = 1 ; passn[25] = 1 ; switchn[25] = 0 ; passp[25] = 0 ; switchp[25] = 0 ; bleeder[25] = 1 ;
  gate[26]  = "na3_pp_bleed" ; input[26] = 4 ; up[26] = 3 ; down[26] = 1 ;  other[26] = 1 ; passn[26] = 0 ; switchn[26] = 0 ; passp[26] = 1 ; switchp[26] = 0 ; bleeder[26] = 1 ;

    //------------------------
   // na3 + switch
  //-------------------------
  gate[27]  = "na3_sn"       ; input[27] = 4 ; up[27] = 3 ; down[27] = 1 ;  other[27] = 1 ; passn[27] = 0 ; switchn[27] = 1 ; passp[27] = 0 ; switchp[27] = 0 ; bleeder[27] = 0 ;
  gate[28]  = "na3_sp"       ; input[28] = 4 ; up[28] = 3 ; down[28] = 1 ;  other[28] = 1 ; passn[28] = 0 ; switchn[28] = 0 ; passp[28] = 0 ; switchp[28] = 1 ; bleeder[28] = 0 ;
  gate[29]  = "na3_sn_bleed" ; input[29] = 4 ; up[29] = 3 ; down[29] = 1 ;  other[29] = 1 ; passn[29] = 0 ; switchn[29] = 1 ; passp[29] = 0 ; switchp[29] = 0 ; bleeder[29] = 1 ;
  gate[30]  = "na3_sp_bleed" ; input[30] = 4 ; up[30] = 3 ; down[30] = 1 ;  other[30] = 1 ; passn[30] = 0 ; switchn[30] = 0 ; passp[30] = 0 ; switchp[30] = 1 ; bleeder[30] = 1 ;

    //------------------------
   // na4 + passn trans
  //-------------------------
  gate[31]  = "na4_pn"       ; input[31] = 5 ; up[31] = 4 ; down[31] = 1 ;  other[31] = 1 ; passn[31] = 1 ; switchn[31] = 0 ; passp[31] = 0 ; switchp[31] = 0 ; bleeder[31] = 0 ;
  gate[32]  = "na4_pp"       ; input[32] = 5 ; up[32] = 4 ; down[32] = 1 ;  other[32] = 1 ; passn[32] = 0 ; switchn[32] = 0 ; passp[32] = 1 ; switchp[32] = 0 ; bleeder[32] = 0 ;
  gate[33]  = "na4_pn_bleed" ; input[33] = 5 ; up[33] = 4 ; down[33] = 1 ;  other[33] = 1 ; passn[33] = 1 ; switchn[33] = 0 ; passp[33] = 0 ; switchp[33] = 0 ; bleeder[33] = 1 ;
  gate[34]  = "na4_pp_bleed" ; input[34] = 5 ; up[34] = 4 ; down[34] = 1 ;  other[34] = 1 ; passn[34] = 0 ; switchn[34] = 0 ; passp[34] = 1 ; switchp[34] = 0 ; bleeder[34] = 1 ;

    //------------------------
   // na4 + switch
  //-------------------------
  gate[35]  = "na4_sn"       ; input[35] = 5 ; up[35] = 4 ; down[35] = 1 ;  other[35] = 1 ; passn[35] = 0 ; switchn[35] = 1 ; passp[35] = 0 ; switchp[35] = 0 ; bleeder[35] = 0 ;
  gate[36]  = "na4_sp"       ; input[36] = 5 ; up[36] = 4 ; down[36] = 1 ;  other[36] = 1 ; passn[36] = 0 ; switchn[36] = 0 ; passp[36] = 0 ; switchp[36] = 1 ; bleeder[36] = 0 ;
  gate[37]  = "na4_sn_bleed" ; input[37] = 5 ; up[37] = 4 ; down[37] = 1 ;  other[37] = 1 ; passn[37] = 0 ; switchn[37] = 1 ; passp[37] = 0 ; switchp[37] = 0 ; bleeder[37] = 1 ;
  gate[38]  = "na4_sp_bleed" ; input[38] = 5 ; up[38] = 4 ; down[38] = 1 ;  other[38] = 1 ; passn[38] = 0 ; switchn[38] = 0 ; passp[38] = 0 ; switchp[38] = 1 ; bleeder[38] = 1 ;

    //------------------------
   // no2 + pass trans
  //-------------------------
  gate[39]  = "no2_pn"       ; input[39] = 3 ; up[39] = 1 ; down[39] = 2 ;  other[39] = 0 ; passn[39] = 1 ; switchn[39] = 0 ; passp[39] = 0 ; switchp[39] = 0 ; bleeder[39] = 0 ;
  gate[40]  = "no2_pp"       ; input[40] = 3 ; up[40] = 1 ; down[40] = 2 ;  other[40] = 0 ; passn[40] = 0 ; switchn[40] = 0 ; passp[40] = 1 ; switchp[40] = 0 ; bleeder[40] = 0 ;
  gate[41]  = "no2_pn_bleed" ; input[41] = 3 ; up[41] = 1 ; down[41] = 2 ;  other[41] = 0 ; passn[41] = 1 ; switchn[41] = 0 ; passp[41] = 0 ; switchp[41] = 0 ; bleeder[41] = 1 ;
  gate[42]  = "no2_pp_bleed" ; input[42] = 3 ; up[42] = 1 ; down[42] = 2 ;  other[42] = 0 ; passn[42] = 0 ; switchn[42] = 0 ; passp[42] = 1 ; switchp[42] = 0 ; bleeder[42] = 1 ;

    //------------------------
   // no2 + switch
  //-------------------------
  gate[43]  = "no2_sn"       ; input[43] = 3 ; up[43] = 1 ; down[43] = 2 ;  other[43] = 0 ; passn[43] = 0 ; switchn[43] = 1 ; passp[43] = 0 ; switchp[43] = 0 ; bleeder[43] = 0 ;
  gate[44]  = "no2_sp"       ; input[44] = 3 ; up[44] = 1 ; down[44] = 2 ;  other[44] = 0 ; passn[44] = 0 ; switchn[44] = 0 ; passp[44] = 0 ; switchp[44] = 1 ; bleeder[44] = 0 ;
  gate[45]  = "no2_sn_bleed" ; input[45] = 3 ; up[45] = 1 ; down[45] = 2 ;  other[45] = 0 ; passn[45] = 0 ; switchn[45] = 1 ; passp[45] = 0 ; switchp[45] = 0 ; bleeder[45] = 1 ;
  gate[46]  = "no2_sp_bleed" ; input[46] = 3 ; up[46] = 1 ; down[46] = 2 ;  other[46] = 0 ; passn[46] = 0 ; switchn[46] = 0 ; passp[46] = 0 ; switchp[46] = 1 ; bleeder[46] = 1 ;

    //------------------------
   // no3 + passn trans
  //-------------------------
  gate[47]  = "no3_pn"       ; input[47] = 4 ; up[47] = 1 ; down[47] = 3 ;  other[47] = 0 ; passn[47] = 1 ; switchn[47] = 0 ; passp[47] = 0 ; switchp[47] = 0 ; bleeder[47] = 0 ;
  gate[48]  = "no3_pp"       ; input[48] = 4 ; up[48] = 1 ; down[48] = 3 ;  other[48] = 0 ; passn[48] = 0 ; switchn[48] = 0 ; passp[48] = 1 ; switchp[48] = 0 ; bleeder[48] = 0 ;
  gate[49]  = "no3_pn_bleed" ; input[49] = 4 ; up[49] = 1 ; down[49] = 3 ;  other[49] = 0 ; passn[49] = 1 ; switchn[49] = 0 ; passp[49] = 0 ; switchp[49] = 0 ; bleeder[49] = 1 ;
  gate[50]  = "no3_pp_bleed" ; input[50] = 4 ; up[50] = 1 ; down[50] = 3 ;  other[50] = 0 ; passn[50] = 0 ; switchn[50] = 0 ; passp[50] = 1 ; switchp[50] = 0 ; bleeder[50] = 1 ;

    //------------------------
   // no3 + switch
  //-------------------------
  gate[51]  = "no3_sn"       ; input[51] = 4 ; up[51] = 1 ; down[51] = 3 ;  other[51] = 0 ; passn[51] = 0 ; switchn[51] = 1 ; passp[51] = 0 ; switchp[51] = 0 ; bleeder[51] = 0 ;
  gate[52]  = "no3_sp"       ; input[52] = 4 ; up[52] = 1 ; down[52] = 3 ;  other[52] = 0 ; passn[52] = 0 ; switchn[52] = 0 ; passp[52] = 0 ; switchp[52] = 1 ; bleeder[52] = 0 ;
  gate[53]  = "no3_sn_bleed" ; input[53] = 4 ; up[53] = 1 ; down[53] = 3 ;  other[53] = 0 ; passn[53] = 0 ; switchn[53] = 1 ; passp[53] = 0 ; switchp[53] = 0 ; bleeder[53] = 1 ;
  gate[54]  = "no3_sp_bleed" ; input[54] = 4 ; up[54] = 1 ; down[54] = 3 ;  other[54] = 0 ; passn[54] = 0 ; switchn[54] = 0 ; passp[54] = 0 ; switchp[54] = 1 ; bleeder[54] = 1 ;

    //------------------------
   // no4 + passn trans
  //-------------------------
  gate[55]  = "no4_pn"       ; input[55] = 5 ; up[55] = 1 ; down[55] = 4 ;  other[55] = 0 ; passn[55] = 1 ; switchn[55] = 0 ; passp[55] = 0 ; switchp[55] = 0 ; bleeder[55] = 0 ;
  gate[56]  = "no4_pp"       ; input[56] = 5 ; up[56] = 1 ; down[56] = 4 ;  other[56] = 0 ; passn[56] = 0 ; switchn[56] = 0 ; passp[56] = 1 ; switchp[56] = 0 ; bleeder[56] = 0 ;
  gate[57]  = "no4_pn_bleed" ; input[57] = 5 ; up[57] = 1 ; down[57] = 4 ;  other[57] = 0 ; passn[57] = 1 ; switchn[57] = 0 ; passp[57] = 0 ; switchp[57] = 0 ; bleeder[57] = 1 ;
  gate[58]  = "no4_pp_bleed" ; input[58] = 5 ; up[58] = 1 ; down[58] = 4 ;  other[58] = 0 ; passn[58] = 0 ; switchn[58] = 0 ; passp[58] = 1 ; switchp[58] = 0 ; bleeder[58] = 1 ;

    //------------------------
   // no4 + switch
  //-------------------------
  gate[59]  = "no4_sn"       ; input[59] = 5 ; up[59] = 1 ; down[59] = 4 ;  other[59] = 0 ; passn[59] = 0 ; switchn[59] = 1 ; passp[59] = 0 ; switchp[59] = 0 ; bleeder[59] = 0 ;
gate[60]  = "no4_sp"       ; input[60] = 5 ; up[60] = 1 ; down[60] = 4 ;  other[60] = 0 ; passn[60] = 0 ; switchn[60] = 0 ; passp[60] = 0 ; switchp[60] = 1 ; bleeder[60] = 0 ;
  gate[61]  = "no4_sn_bleed" ; input[61] = 5 ; up[61] = 1 ; down[61] = 4 ;  other[61] = 0 ; passn[61] = 0 ; switchn[61] = 1 ; passp[61] = 0 ; switchp[61] = 0 ; bleeder[61] = 1 ;
  gate[62]  = "no4_sp_bleed" ; input[62] = 5 ; up[62] = 1 ; down[62] = 4 ;  other[62] = 0 ; passn[62] = 0 ; switchn[62] = 0 ; passp[62] = 0 ; switchp[62] = 1 ; bleeder[62] = 1 ;
}

/**********************************************************************************\
FUNC : mcc_genbenchtrans
\**********************************************************************************/
void mcc_genbenchtrans()
{
 int i ;
 int j ;
 int k ;
 int s ;
 int c ;
 char *subcktmodeln, *subcktmodelp ;
 char *modeln, *modelp ;
 char *mn, *mp ;
 char bufn[1024] ;
 char bufp[1024] ;
 char *gate[MCC_NB_GATES] ; 
 int input[MCC_NB_GATES] ;  // contenu = determine le nb d entrees de la gate
 int up[MCC_NB_GATES] ;     // contenu = determine le nb de transistor pmos de la gate
 int down[MCC_NB_GATES] ;   // contenu = determine le nb de transistor nmos de la gate
 int other[MCC_NB_GATES] ;  // contenu = valeur 0 ou vdd a driver pour le reste des signaux
 int passn[MCC_NB_GATES] ;   // contenu = nb de pass trans n a driver
 int switchn[MCC_NB_GATES] ;// contenu = nb de switch n a driver 
 int passp[MCC_NB_GATES] ;   // contenu = nb de pass trans p  a driver
 int switchp[MCC_NB_GATES] ;// contenu = nb de switch p a driver
 int bleeder[MCC_NB_GATES] ;// contenu = nb de bleeder a driver
 int capa[4] ;
 int slope[3] ;
 int nbgate = MCC_NB_GATES ;
 FILE *file ;
 FILE *resfile ;
 double errorneg = 0.0 ;
 double errorpos = 0.0 ;
 double error = 0.0 ;
 double err ;
 int nb = 0 ;
 int nbneg = 0 ;
 int nbpos = 0 ;
 int taberr[9] ;
 int transition[4] ;

 transition[0] = MCC_DU;
 transition[1] = MCC_UD;
 transition[2] = MCC_DD;
 transition[3] = MCC_UU;
 modeln = MCC_TNMODEL ;
 modelp = MCC_TPMODEL ;

 file = fopen(mcc_debug_prefix("benchresult"),"w") ;
 avt_printExecInfo(file, "#", "", "");
 fclose(file) ;

 sprintf(bufn,"m") ;
 sprintf(bufp,"m") ;

 mcc_init_gate ( gate , input, up, down, other, passn, switchn, passp, switchp, bleeder );

 capa[0] = 10 ;
 capa[1] = 50 ;
 capa[2] = 200 ;
 capa[3] = 500 ;
 
 slope[0] = 50 ;
 slope[1] = 200 ;
 slope[2] = 500 ;

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

 mn = mbkstrdup(bufn) ;
 mp = mbkstrdup(bufp) ;

 for(i = 0 ; i < nbgate ; i++)
   {
    if(MCC_DEBUG_GATE != NULL) 
     if(strcmp(MCC_DEBUG_GATE,gate[i]) != 0)
       continue ;
    // only duals
    if( !MCC_DEBUG_GATE ) 
      {
       // zap the pass trans
       if (( passn[i] || passp [i] ) && ( MCC_DEBUG_PASS_TRANS == 'X' )) 
         continue;
       // zap the switchs
       if (( switchn[i] || switchp[i] ) && ( MCC_DEBUG_SWITCH == 'X' )) 
         continue;
      }
    mcc_generategate(NULL,gate[i],passn[i],switchn[i],passp[i],switchp[i],bleeder[i],
                     input[i],up[i],down[i],
                     modeln,modelp,mn,mp,
                     MCC_WN,MCC_WP,MCC_LN,MCC_LP) ;
   }
 
 if (subcktmodeln != NULL)
  {
   sprintf(bufn,"xm") ;
   modeln = subcktmodeln ;
   mbkfree(mn) ;
   mn = mbkstrdup(bufn) ;
  }

 if (subcktmodelp != NULL)
  {
   sprintf(bufp,"xm") ;
   modelp = subcktmodelp ;
   mbkfree(mp) ;
   mp = mbkstrdup(bufp) ;
  }

 for(i = 0 ; i < 9 ; i++)
    taberr[i] = 0 ;

 for(i = 0 ; i < nbgate ; i++)       // gate variation
 for(j = 0 ; j < input[i] ; j++)     // input variation 
 for(k = 0 ; k < 4 ; k++)            // transition variation
 for(c = 0 ; c < 4 ; c++)            // capa variation
 for(s = 0 ; s < 3 ; s++)            // slowpe variation
   {
    if( !passn[i] && !passp[i] && !switchn[i] && !switchp[i] && k > 1 )
      continue ;
    if(( passn[i] || passp[i] || switchn[i] || switchp[i] ) && ( k > 1 ) && (j != input[i]-1))
      // if input is not the cmd of the pass or switch
      continue ;
     

    if((MCC_DEBUG_INPUT != -1) && (MCC_DEBUG_INPUT != j))
      continue ;

    if((MCC_DEBUG_TRANS != -1) && (MCC_DEBUG_TRANS != transition[k]))
      continue ;

    if(MCC_DEBUG_SLOPE != 0)
     {
      if(s == 0)
        slope[0] = MCC_DEBUG_SLOPE ;
      else
        continue ;
     }

    if(MCC_DEBUG_CAPA != 0)
     {
      if(c == 0)
        capa[0] = MCC_DEBUG_CAPA ;
      else
        continue ;
     }

    if(MCC_DEBUG_GATE != NULL) 
     if(strcmp(MCC_DEBUG_GATE,gate[i]) != 0)
       continue ;
    if( !MCC_DEBUG_GATE ) 
      {
       // zap the pass trans
       if (( passn[i] || passp [i] ) && ( MCC_DEBUG_PASS_TRANS == 'X' )) 
         continue;
       // zap the switchs
       if (( switchn[i] || switchp[i] ) && ( MCC_DEBUG_SWITCH == 'X' )) 
         continue;
      }

    if(( passn[i] || passp[i] || switchn[i] || switchp[i] ) && (j == input[i]-1))  // if input is the cmd of the pass or switch
     {
      if ( passn[i] || switchn[i] )  // pass trans or switch on nmos 
       {
        if ( transition[k] == MCC_DU || transition[k] == MCC_DD )   // transition doesn t exist
          continue;
        other[i] =  ( transition[k] == MCC_UD ) ? 1 : 0;
       }
      else                                                         // pass trans or switch on pmos 
       {
        if ( transition[k] == MCC_UD || transition[k] == MCC_UU ) // transition doesn t exist
          continue;
        other[i] =  ( transition[k] == MCC_DU ) ? 0 : 1;
       }
     }


    err = mcc_generatesimgate (gate[i],passn[i],switchn[i],passp[i],switchp[i],bleeder[i],
                               up[i],down[i],j,input[i],transition[k],slope[s],capa[c],other[i],
                               modeln,modelp,mn,mp,MCC_WN,MCC_WP,MCC_LN,MCC_LP) ;
    nb++ ;

    if(err >= 0)
      {
       errorpos += err ;
       nbpos++ ;
       error += err ;
      }
    else
      {
       errorneg += -err ;
       nbneg++ ;
       error += -err ;
      }

    if(err < -30.0)
      taberr[0]++ ;
    else if(err < -20.0)
      taberr[1]++ ;
    else if(err < -10.0)
      taberr[2]++ ;
    else if(err < -5.0)
      taberr[3]++ ;
    else if(err < 5.0)
      taberr[4]++ ;
    else if(err < 10.0)
      taberr[5]++ ;
    else if(err < 20.0)
      taberr[6]++ ;
    else if(err < 30.0)
      taberr[7]++ ;
    else
      taberr[8]++ ;
   }

 resfile = fopen(mcc_debug_prefix("benchresult"),"a") ;
 fprintf(stdout,"\n\naverage = %g negaverage = -%g posaverage = %g\n\n",
                 (nb == 0) ? 0.0 : error/(double)nb,(nbneg == 0) ? 0.0 : errorneg/(double)nbneg,(nbpos == 0) ? 0.0 : errorpos/(double)nbpos) ;
 fprintf(resfile,"\n\naverage = %g negaverage = -%g posaverage = %g\n\n",
                 (nb == 0) ? 0.0 : error/(double)nb,(nbneg == 0) ? 0.0 : errorneg/(double)nbneg,(nbpos == 0) ? 0.0 : errorpos/(double)nbpos) ;

 fprintf(stdout,"      error < -30 : %d \n",taberr[0]) ;
 fprintf(stdout,"-30 < error < -20 : %d \n",taberr[1]) ;
 fprintf(stdout,"-20 < error < -10 : %d \n",taberr[2]) ;
 fprintf(stdout,"-10 < error < -5  : %d \n",taberr[3]) ;
 fprintf(stdout," -5 < error < 5   : %d \n",taberr[4]) ;
 fprintf(stdout,"  5 < error < 10  : %d \n",taberr[5]) ;
 fprintf(stdout," 10 < error < 20  : %d \n",taberr[6]) ;
 fprintf(stdout," 20 < error < 30  : %d \n",taberr[7]) ;
 fprintf(stdout," 30 < error       : %d \n",taberr[8]) ;

 fprintf(resfile,"      error < -30 : %d \n",taberr[0]) ;
 fprintf(resfile,"-30 < error < -20 : %d \n",taberr[1]) ;
 fprintf(resfile,"-20 < error < -10 : %d \n",taberr[2]) ;
 fprintf(resfile,"-10 < error < -5  : %d \n",taberr[3]) ;
 fprintf(resfile," -5 < error < 5   : %d \n",taberr[4]) ;
 fprintf(resfile,"  5 < error < 10  : %d \n",taberr[5]) ;
 fprintf(resfile," 10 < error < 20  : %d \n",taberr[6]) ;
 fprintf(resfile," 20 < error < 30  : %d \n",taberr[7]) ;
 fprintf(resfile," 30 < error       : %d \n",taberr[8]) ;
 fclose(resfile) ;

 mbkfree(mn) ;
 mbkfree(mp) ;
}

/**********************************************************************************\
FUNC : mcc_genbench
\**********************************************************************************/
void mcc_genbench()
{
// sim_set_result_file_extension('p', 0, &MCC_SPICEOUT);
 mcc_genbenchtrans() ;
}

/****************************************************************************\
 * Function : mcc_calcul_leak
 *
\****************************************************************************/
void mcc_calcul_leak (elp_lotrs_param *lotrsparam_n, elp_lotrs_param *lotrsparam_p)
{
  double L,W,temp;
  double var,igb,igidl,igisl,idb,isb;
  double igd,igs,igcs,igcd;
  FILE *file;
  mcc_modellist *ptmodeln,*ptmodelp;
  int debug_igb = 1;
  int debug_igixl = 1;
  int debug_ixb = 1;
  int debug_igx = 1;
  int debug_igcx = 1;
  int debug_nmos = 1;
  int debug_pmos = 1;

  ptmodeln = mcc_getmodel ( MCC_MODELFILE,
                            MCC_TNMODEL,
                            MCC_NMOS,
                            MCC_NCASE,
                            MCC_LN*1.0e-6,
                            MCC_WN*1.0e-6, 0);
  ptmodelp = mcc_getmodel ( MCC_MODELFILE,
                            MCC_TPMODEL,
                            MCC_PMOS,
                            MCC_PCASE,
                            MCC_LP*1.0e-6,
                            MCC_WP*1.0e-6, 0);

  temp = MCC_TEMP;
 
  if ( !ptmodeln || !ptmodelp ) return;
  if ( ptmodeln->MODELTYPE != MCC_BSIM4 || ptmodelp->MODELTYPE != MCC_BSIM4 ) return;

  if ( debug_pmos ) {
    // PMOS 
    
    L = MCC_LP*1.0e-6;
    W =  MCC_WP*1.0e-6;

    if ( debug_igb ) {
      // Igb 
      file = fopen ("Igb_pmos_vgs.dat","w");
      avt_printExecInfo(file, "#", "", "");
      fprintf (file,"# vgs Igb\n");
      
      for ( var = -MCC_VDDmax ; var <= 0.0 ; var+=MCC_DC_STEP ) {
       igb = mcc_calcIgb_bsim4 (ptmodelp, L, W, temp, 
                                var, -MCC_VDDmax, 0.0, 
                                lotrsparam_p);
       fprintf(file,"%g %g\n",var,igb);
      }
      fclose (file);
      file = fopen ("Igb_pmos_vbs.dat","w");
      avt_printExecInfo(file, "#", "", "");
      fprintf (file,"# vbs Igb\n");
      for ( var = -MCC_VDDmax/2.0 ; var <= MCC_VDDmax/2.0 ; var+=MCC_DC_STEP ) {
       igb = mcc_calcIgb_bsim4 (ptmodelp, L, W, temp, 
                                0.0, -MCC_VDDmax, var, 
                                lotrsparam_p);
       fprintf(file,"%g %g\n",var,igb);
      }
      fclose (file);
      
      file = fopen ("Igb_pmos_vds.dat","w");
      avt_printExecInfo(file, "#", "", "");
      fprintf (file,"# vds Igb\n");
      for ( var = -MCC_VDDmax ; var <= 0.0 ; var+=MCC_DC_STEP ) {
       igb = mcc_calcIgb_bsim4 (ptmodelp, L, W, temp, 
                                0.0, var , 0.0, 
                                lotrsparam_p);
       fprintf(file,"%g %g\n",var,igb);
      }
      fclose (file);
    }
    
    if ( debug_igixl ) {
      // GIDL GISL debug
      file = fopen ("Igixl_pmos_vgs.dat","w");
      avt_printExecInfo(file, "#", "", "");
      fprintf (file,"# vgs Igidl Igisl\n");
      for ( var = -MCC_VDDmax ; var <= 0.0 ; var+=MCC_DC_STEP ) {
       mcc_calcIgixl_bsim4 (ptmodelp, L, W,  
                            &igidl, &igisl, temp,
                            var, -MCC_VDDmax, 0.0, 
                            lotrsparam_p);
       fprintf(file,"%g %g %g\n",var,igidl,igisl);
      }
      fclose (file);
      
      file = fopen ("Igixl_pmos_vbs.dat","w");
      avt_printExecInfo(file, "#", "", "");
      fprintf (file,"# vbs Igidl Igisl\n");
      for ( var = -MCC_VDDmax/2.0 ; var <= MCC_VDDmax/2.0 ; var+=MCC_DC_STEP ) {
       mcc_calcIgixl_bsim4 (ptmodelp, L, W,  
                            &igidl, &igisl, temp,
                            0.0, -MCC_VDDmax, var, 
                            lotrsparam_p);
       fprintf(file,"%g %g %g\n",var,igidl,igisl);
      }
      fclose (file);
      
      file = fopen ("Igixl_pmos_vds.dat","w");
      avt_printExecInfo(file, "#", "", "");
      fprintf (file,"# vds Igidl Igisl\n");
      for ( var = -MCC_VDDmax ; var <= 0.0 ; var+=MCC_DC_STEP ) {
       mcc_calcIgixl_bsim4 (ptmodelp, L, W,  
                            &igidl, &igisl, temp,
                            0.0, var, 0.0, 
                            lotrsparam_p);
       fprintf(file,"%g %g %g\n",var,igidl,igisl);
      }
      fclose (file);
    }
    
    if ( debug_ixb ) {
      // Jonction IV debug
      file = fopen ("Ixb_pmos_vbs.dat","w");
      avt_printExecInfo(file, "#", "", "");
      fprintf (file,"# var Idb Isb\n");
      for ( var = -MCC_VDDmax/2.0 ; var <= MCC_VDDmax/2.0 ; var+=MCC_DC_STEP ) {
        mcc_calcIxb_bsim4 (ptmodelp, L, W, 
                           &idb,&isb,
                           temp, -MCC_VDDmax, var, 
                           -1.0, -1.0, -1.0, -1.0,
                           lotrsparam_p);
        fprintf(file,"%g %g %g\n",var,idb,isb);
      }
      fclose (file);
    
      file = fopen ("Ixb_pmos_vds.dat","w");
      avt_printExecInfo(file, "#", "", "");
      fprintf (file,"# var Idb Isb\n");
      for ( var = -MCC_VDDmax ; var <= 0.0 ; var+=MCC_DC_STEP ) {
        mcc_calcIxb_bsim4 (ptmodelp, L, W, 
                           &idb,&isb,
                           temp, var, 0.0, 
                           -1.0, -1.0, -1.0, -1.0,
                           lotrsparam_p);
        fprintf(file,"%g %g %g\n",var,idb,isb);
      }
      fclose (file);
    }
 
    if ( debug_igx ) {
      // Gate tunneling : Igs, Igd
      file = fopen ("Igx_pmos_vgs.dat","w");
      avt_printExecInfo(file, "#", "", "");
      fprintf (file,"# vgs Igd Igs\n");
      for ( var = 0.0 ; var <= MCC_VDDmax; var+=MCC_DC_STEP ) {
        mcc_calcIgx_bsim4 (ptmodelp, L, W, 
                           &igd,&igs,
                           temp, MCC_VDDmax, var-MCC_VDDmax, 
                           lotrsparam_p);
        fprintf(file,"%g %g %g\n",var,igb,igs);
      }
      fclose (file);
    }
 
    if ( debug_igcx ) {
      // Gate tunneling : Igcs, Igcd
      file = fopen ("Igcx_pmos_vgs.dat","w");
      avt_printExecInfo(file, "#", "", "");
      fprintf (file,"# vgs Igcd Igcs\n");
      for ( var = 0.0 ; var <= MCC_VDDmax ; var+=MCC_DC_STEP ) {
        mcc_calcIgcx_bsim4 (ptmodelp, L, W, 
                           &igcd,&igcs,
                           temp, MCC_VDDmax, var-MCC_VDDmax, lotrsparam_p->VBULK-MCC_VDDmax,
                           lotrsparam_p);
        fprintf(file,"%g %g %g\n",var,igcd,igcs);
      }
      fclose (file);
    }
  }
  
 /*-----------------------------------------------------------------------------*/ 

  if ( debug_nmos ) {
    //- NMOS
    L = MCC_LN*1.0e-6;
    W =  MCC_WN*1.0e-6;


    if ( debug_igb ) {
      // Igb 
      file = fopen ("Igb_nmos_vgs.dat","w");
      avt_printExecInfo(file, "#", "", "");
      fprintf (file,"# vgs Igb\n");
      
      for ( var = 0.0 ; var <= MCC_VDDmax ; var+=MCC_DC_STEP ) {
       igb = mcc_calcIgb_bsim4 (ptmodeln, L, W, temp, 
                                var, MCC_VDDmax, 0.0, 
                                lotrsparam_n);
       fprintf(file,"%g %g\n",var,igb);
      }
      fclose (file);
      file = fopen ("Igb_nmos_vbs.dat","w");
      avt_printExecInfo(file, "#", "", "");
      fprintf (file,"# vbs Igb\n");
      for ( var = -MCC_VDDmax/2.0 ; var <= MCC_VDDmax/2.0 ; var+=MCC_DC_STEP ) {
       igb = mcc_calcIgb_bsim4 (ptmodeln, L, W, temp, 
                                0.0, MCC_VDDmax, var, 
                                lotrsparam_n);
       fprintf(file,"%g %g\n",var,igb);
      }
      fclose (file);
      
      file = fopen ("Igb_nmos_vds.dat","w");
      avt_printExecInfo(file, "#", "", "");
      fprintf (file,"# vds Igb\n");
      for ( var = 0.0 ; var <= MCC_VDDmax ; var+=MCC_DC_STEP ) {
       igb = mcc_calcIgb_bsim4 (ptmodeln, L, W, temp, 
                                0.0, var , 0.0, 
                                lotrsparam_n);
       fprintf(file,"%g %g\n",var,igb);
      }
      fclose (file);
    }
    
    if ( debug_igixl ) {
      // GIDL GISL debug
      file = fopen ("Igixl_nmos_vgs.dat","w");
      avt_printExecInfo(file, "#", "", "");
      fprintf (file,"# vgs Igidl Igisl\n");
      for ( var = 0.0; var <= MCC_VDDmax ; var+=MCC_DC_STEP ) {
       mcc_calcIgixl_bsim4 (ptmodeln, L, W,  
                            &igidl, &igisl, temp,
                            var, MCC_VDDmax, 0.0, 
                            lotrsparam_n);
       fprintf(file,"%g %g %g\n",var,igidl,igisl);
      }
      fclose (file);
      
      file = fopen ("Igixl_nmos_vbs.dat","w");
      avt_printExecInfo(file, "#", "", "");
      fprintf (file,"# vbs Igidl Igisl\n");
      for ( var = -MCC_VDDmax/2.0 ; var <= MCC_VDDmax/2.0 ; var+=MCC_DC_STEP ) {
       mcc_calcIgixl_bsim4 (ptmodeln, L, W,  
                            &igidl, &igisl, temp,
                            0.0, MCC_VDDmax, var, 
                            lotrsparam_n);
       fprintf(file,"%g %g %g\n",var,igidl,igisl);
      }
      fclose (file);
      
      file = fopen ("Igixl_nmos_vds.dat","w");
      avt_printExecInfo(file, "#", "", "");
      fprintf (file,"# vds Igidl Igisl\n");
      for ( var = 0.0; var <= MCC_VDDmax ; var+=MCC_DC_STEP ) {
       mcc_calcIgixl_bsim4 (ptmodeln, L, W,  
                            &igidl, &igisl, temp,
                            0.0, var, 0.0, 
                            lotrsparam_n);
       fprintf(file,"%g %g %g\n",var,igidl,igisl);
      }
      fclose (file);
    }
    
    if ( debug_ixb ) {
      // Jonction IV debug
      file = fopen ("Ixb_nmos_vbs.dat","w");
      avt_printExecInfo(file, "#", "", "");
      fprintf (file,"# var Idb Isb\n");
      for ( var = -MCC_VDDmax/2.0 ; var <= MCC_VDDmax/2.0 ; var+=MCC_DC_STEP ) {
        mcc_calcIxb_bsim4 (ptmodeln, L, W, 
                           &idb,&isb,
                           temp, MCC_VDDmax, var, 
                           -1.0, -1.0, -1.0, -1.0,
                           lotrsparam_n);
        fprintf(file,"%g %g %g\n",var,idb,isb);
      }
      fclose (file);
      
      file = fopen ("Ixb_nmos_vds.dat","w");
      avt_printExecInfo(file, "#", "", "");
      fprintf (file,"# var Idb Isb\n");
      for ( var = 0.0; var <= MCC_VDDmax ; var+=MCC_DC_STEP ) {
        mcc_calcIxb_bsim4 (ptmodeln, L, W, 
                           &idb,&isb,
                           temp, var, 0.0, 
                           -1.0, -1.0, -1.0, -1.0,
                           lotrsparam_n);
        fprintf(file,"%g %g %g\n",var,idb,isb);
      }
      fclose (file);
    }
      
    if ( debug_igx ) {
      // Gate tunneling : Igs, Igd
      file = fopen ("Igx_nmos_vgs.dat","w");
      avt_printExecInfo(file, "#", "", "");
      fprintf (file,"# vgs Igd Igs\n");
      for ( var = 0.0 ; var <= MCC_VDDmax ; var+=MCC_DC_STEP ) {
        mcc_calcIgx_bsim4 (ptmodeln, L, W, 
                           &igd,&igs,
                           temp, MCC_VDDmax, var, 
                           lotrsparam_n);
        fprintf(file,"%g %g %g\n",var,igd,igs);
      }
      fclose (file);
    }
    
    if ( debug_igcx ) {
      // Gate tunneling : Igcs, Igcd
      file = fopen ("Igcx_nmos_vgs.dat","w");
      avt_printExecInfo(file, "#", "", "");
      fprintf (file,"# vgs Igcd Igcs\n");
      for ( var = 0.0 ; var <= MCC_VDDmax ; var+=MCC_DC_STEP ) {
        mcc_calcIgcx_bsim4 (ptmodeln, L, W, 
                            &igcd,&igcs,
                            temp, MCC_VDDmax, var, lotrsparam_n->VBULK,
                            lotrsparam_n);
        fprintf(file,"%g %g %g\n",var,igcd,igcs);
      }
      fclose (file);
    }
  }
}

void mcc_plot_qint( char *filename, char *technoname, char *transname, int transtype, int transcase, double L, double W, double vds, double vgsmax, double vbs, double temp, elp_lotrs_param *lotrsparam )
{
  FILE   *ptf ;
  double step ;
  double vgs ;
  double qg, qd, qs, qb ;
  double xl, xw, la, wa ;
  double rap ;
  
  ptf = mbkfopen( filename, "dat", WRITE_TEXT );
  avt_printExecInfo(ptf, "#", "", "");
  if( !ptf ) {
    printf( "can't open file for writing.\n" );
    return ;
  }

  xl = mcc_getXL (technoname, transname, transtype, transcase, L,W);
  xw = mcc_getXW (technoname, transname, transtype, transcase, L,W);
  la   = L*xl + mcc_calcDLC (technoname, transname, transtype, transcase, L, W, lotrsparam);
  wa   = W*xw + mcc_calcDWC (technoname, transname, transtype, transcase, L, W, lotrsparam);
  rap = la*wa ;

  fprintf( ptf, "# vgs qg qs qd qb\n" );
  
  step = vgsmax/1000.0 ;
  for( vgs=0.0 ; vgs<vgsmax ; vgs = vgs + step ) {

     mcc_calcQint( technoname, transname, transtype, transcase, L, W, temp, vgs, vbs, vds, &qg, &qs, &qd, &qb, lotrsparam );
     fprintf( ptf, "%g %g %g %g %g\n", vgs, qg*rap, qs*rap, qd*rap, qb*rap );
  }

  vgs=vgsmax;
  mcc_calcQint( technoname, transname, transtype, transcase, L, W, temp, vgs, vbs, vds, &qg, &qs, &qd, &qb, lotrsparam );
  fprintf( ptf, "%g %g %g %g %g\n", vgs, qg*rap, qs*rap, qd*rap, qb*rap );

  fclose( ptf );
  
}

typedef struct
{
  mbk_pwl *tabs[3];
} SAVEOPTIM;

/*
static void save_optim(SAVEOPTIM *so, int get, int type)
{
  int i;
  mbk_pwl *tabs[3];
  mbk_pwl **tabss[3];

  if (type==MCC_TRANS_N)
    {
      tabs[0]=MCC_OPTIM_IDNRES, tabs[1]=MCC_OPTIM_IDNVGS, tabs[2]=MCC_OPTIM_IDNSAT;
      tabss[0]=&MCC_OPTIM_IDNRES, tabss[1]=&MCC_OPTIM_IDNVGS, tabss[2]=&MCC_OPTIM_IDNSAT;
    }
  else
    {
      tabs[0]=MCC_OPTIM_IDPRES, tabs[1]=MCC_OPTIM_IDPVGS, tabs[2]=MCC_OPTIM_IDPSAT;
      tabss[0]=&MCC_OPTIM_IDPRES, tabss[1]=&MCC_OPTIM_IDPVGS, tabss[2]=&MCC_OPTIM_IDPSAT;
    }

  if (get)
    {
      for (i=0; i<3; i++)
        {
          so->tabs[i]=tabs[i];
        }
    }
  else
    {
      for (i=0; i<3; i++)
        {
          *tabss[i]=so->tabs[i];
        }
    }
}
static void free_optim(SAVEOPTIM *so)
{
  int i;
  for (i=0; i<3; i++)
    {
      if (so->tabs[i]!=NULL) mbk_pwl_free_pwl(so->tabs[i]);
    }
}
*/


void mcc_fill_current_for_rsat(elp_lotrs_param *lotrsparam_n,elp_lotrs_param *lotrsparam_p)
{
  int i, j, k, l, m;
  int mode, stepmin, stepmax;
  FILE *f;
  elp_lotrs_param *trsparam;
  char *name=mcc_debug_prefix("i_for_rlin_rsat.dat");
  float ic;
  float n_vbs_values[2]={0, -MCC_VDDmax/4};
  float p_vbs_values[2]={0, MCC_VDDmax/4};
  float n_vgs_values[2]={MCC_VDDmax, (MCC_VDDmax-MCC_VTN)/2+MCC_VTN};
  float p_vgs_values[2]={MCC_VDDmax, (MCC_VDDmax-MCC_VTP)/2+MCC_VTP};
  float *pvbs, *pvgs;
  char buf[128];
  mcc_modellist *ptmodeln,*ptmodelp;
  double vbs ;

 ptmodeln = mcc_getmodel ( MCC_MODELFILE,
                           MCC_TNMODEL,
                           MCC_NMOS,
                           MCC_NCASE,
                           MCC_LN*1.0e-6,
                           MCC_WN*1.0e-6, 0);
 ptmodelp = mcc_getmodel ( MCC_MODELFILE,
                           MCC_TPMODEL,
                           MCC_PMOS,
                           MCC_PCASE,
                           MCC_LP*1.0e-6,
                           MCC_WP*1.0e-6, 0);

  mode = MCC_CALC_CUR ;
  MCC_CALC_CUR = MCC_CALC_MODE ;

  stepmax = mcc_ftoi((MCC_VDDmax) / (MCC_DC_STEP)) ;
  stepmin = 0 ;

  name=mbkstrdup(name);
  f = mcc_fopen(name,"w") ;
  avt_printExecInfo(f, "#", "", "");

  for(j = 0 ; j < 2 ; j++)
    for (i=0; i<3; i++)
      for (l=0; l<2; l++)
        for (k=0; k<2; k++)
          TRS_CURVS.I_VDS_RSATLIN[j][i][l][k]=(double *)mbkalloc(sizeof(double)*(stepmax+1));
          
  for(j = 0 ; j < 2 ; j++)
    {
      if ( j == MCC_TRANS_N )
        trsparam = lotrsparam_n, pvgs=n_vgs_values, pvbs=n_vbs_values;
      else
        trsparam = lotrsparam_p, pvgs=p_vgs_values, pvbs=p_vbs_values;
      for (m=0; m<3; m++)
        for (k=0; k<2; k++)
          for (l=0; l<2; l++)
            {
              for(i = stepmin ; i <= stepmax ; i++)
                {
                  switch (m)
                    {
                    case 0: // BSIM
                      MCC_CALC_CUR = MCC_CALC_MODE;
                      if( j==MCC_TRANS_N ) 
                        vbs = pvbs[k]+lotrsparam_n->VBULK;
                      else
                        vbs = pvbs[k]+lotrsparam_p->VBULK-MCC_VDDmax;
                      ic = mcc_spicecurrent(pvgs[l],i*MCC_DC_STEP,vbs,j,trsparam) ;
                      break;
                    case 1: // MCC
/*                      MCC_CALC_CUR = MCC_SIM_MODE;
                      save_optim(&so[j][k][l], 0, j);
                      MCC_VGS=pvgs[l];
                      ic = mcc_spicecurrent(pvgs[l],i*MCC_DC_STEP,pvbs[k],j,trsparam) ;*/
                      ic=0;
                      break;
                    case 2: // ELP
                      ic = mcc_current(pvgs[l],i*MCC_DC_STEP,fabs(pvbs[k]),j) ;
                      break;
                    }
                  TRS_CURVS.I_VDS_RSATLIN[j][m][k][l][i]=ic;
                }
            }
    }

  for (i=0; i<2; i++)
    {
      fprintf(f,"%-10s", i==0?"# VDS":"#");
      
      for(j = 0 ; j < 2 ; j++)
        {
          if ( j == MCC_TRANS_N )
            pvgs=n_vgs_values, pvbs=n_vbs_values;
          else
            pvgs=p_vgs_values, pvbs=p_vbs_values;
          for (m=0; m<3; m++)
            for (k=0; k<2; k++)
              for (l=0; l<2; l++)
            {
              if (i==0)
                sprintf(buf,"%s/%s",j==0?"N":"P",m==0?"BSIM":m==1?"MCC":"ELP");
              else if (i==1)
                sprintf(buf,"Vbs=%.2f/Vgs=%.2f", pvbs[k], pvgs[l]);
              fprintf(f,"%-20s", buf);
            }
        }
      fprintf(f,"\n");
    }
  for(i = stepmin ; i <= stepmax ; i++)
    {
      fprintf(f,"%10g",i*MCC_DC_STEP);
      for(j = 0 ; j < 2 ; j++)
        for (m=0; m<3; m++)
          for (k=0; k<2; k++)
            for (l=0; l<2; l++)
              {
                fprintf(f,"%20g",TRS_CURVS.I_VDS_RSATLIN[j][m][k][l][i]);
              }
      fprintf(f,"\n");
    }

  fclose(f);

  f = mcc_fopen(mcc_debug_prefix("i_for_rlin_rsat.plt"),"w") ;
  avt_printExecInfo(f, "#", "", "");

  for(j = 0 ; j < 2 ; j++)
    {
      if ( j == MCC_TRANS_N )
        pvgs=n_vgs_values, pvbs=n_vbs_values;
      else
        pvgs=p_vgs_values, pvbs=p_vbs_values;
      //    for (m=0; m<3; m++)
      for (k=0; k<2; k++)
        for (l=0; l<2; l++)
          {
            fprintf(f,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
            fprintf(f,"set title \"%s\"\n", j==0?"NMOS":"PMOS");
            fprintf(f,"set xlabel \"VDS (V)\"\n") ;
            fprintf(f,"set ylabel \"IDS (A)\"\n") ;
            fprintf(f,"set grid\n") ;
            fprintf(f,"set key top left\n");
//            fprintf(f,"set xrange [0.0:%g]\n", MCC_VDDmax);
            fprintf(f,"plot '%s' using 1:%d title '%s vbs=%.2f/vgs=%.2f' with lines,\\\n", name, 2+ j*12+0*4+k*2+l, mcc_getmccname( j==0?ptmodeln:ptmodelp ),pvbs[k], pvgs[l]);
//            fprintf(f,"     '%s' using 1:%d title 'MCC vbs=%.2f/vgs=%.2f' with lines,\\\n", name, 2+ j*12+1*4+k*2+l, pvbs[k], pvgs[l]);
            fprintf(f,"     '%s' using 1:%d title 'elp vbs=%.2f/vgs=%.2f' with lines\n", name, 2+ j*12+2*4+k*2+l, pvbs[k], pvgs[l]);
            fprintf(f,"pause -1 'Hit CR to finish'\n") ;
          }
    }
  
  fclose(f);
/*
  for(j = 0 ; j < 2 ; j++)
    {
      for (l=0; l<2; l++)
        for (k=0; k<2; k++)
          {
            free_optim(&so[j][k][l]);
          }
    }

  save_optim(&origson, 0, MCC_TRANS_N);
  save_optim(&origsop, 0, MCC_TRANS_P);
*/
  mbkfree(name);
  MCC_CALC_CUR=mode;

}

static int cmpfloat(const void *vv1, const void *vv2)
{
  float *v1=(float *)vv1, *v2=(float *)vv2;
  if (*v1<*v2) return -1;
  else if (*v1>*v2) return 1;
  return 0;
}

typedef struct
{
  double cgdp, cgdpi, cgdn, cgdni, cgsp, cgsn, cgpn, cgpp;
  double cgatep, cgaten, ndeltal, pdeltal, ndeltaw, pdeltaw, startv, endv;
  double cdrainp, cdrainn, cdsn, cdpn, cdwn, cdsp, cdpp, cdwp;
  double ndw, pdw, nairshrink, pairshrink, covn, covp;
  double capai;
} allforbilan;

//#define DEBUGBC
static void get_bilan_capa(elp_lotrs_param *lotrsparam_n, elp_lotrs_param *lotrsparam_p, int trans, allforbilan *afb, float input_voltage, float output_voltage)
{
  float vgs, vbs, vds;
  float startv, endv;
  // NMOS ***********
  vds=output_voltage;
  vgs=input_voltage;
  vbs=lotrsparam_n->VBULK;

  if (trans==0)
    mcc_GetInputCapa(MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6, MCC_TEMP, 0, input_voltage, MCC_VDDmax, output_voltage, 0, 0, lotrsparam_n, &afb->cgsn, &afb->cgdni, &afb->cgpn);
  else
    mcc_GetInputCapa(MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6, MCC_TEMP, MCC_VDDmax, input_voltage, 0, output_voltage, 0, 0, lotrsparam_n, &afb->cgsn, &afb->cgdni, &afb->cgpn);
  afb->covn=2*(MCC_WNeff+afb->ndeltaw)*1e-6*afb->cgpn;
  afb->cgaten=(MCC_WNeff+afb->ndeltaw)*(MCC_LNeff+afb->ndeltal)*1e-12*afb->cgsn+afb->covn;
#ifdef DEBUGBC
  printf("--gate: %g %g %g = %g\n", cgdni, cgsn, cgpn, cgaten);
#endif
  afb->cgdn=afb->cgdni*(MCC_WNeff+afb->ndeltaw)*(MCC_LNeff+afb->ndeltal)*1e-12;

      
  //CD 
  if (trans==0)
    {
      startv=lotrsparam_n->VBULK-MCC_VDDmax;
      endv=lotrsparam_n->VBULK-output_voltage;
    }
  else
    {
      startv=lotrsparam_n->VBULK-0;
      endv=lotrsparam_n->VBULK-output_voltage;
    }
  afb->cdsn = mcc_calcCDS (MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6, MCC_TEMP, startv, endv, lotrsparam_n);
  afb->cdpn = mcc_calcCDP (MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6, MCC_TEMP, startv, endv,lotrsparam_n);
  afb->cdwn = mcc_calcCDW (MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6, MCC_TEMP, startv, endv, vgs, lotrsparam_n); 

  afb->cdrainn=afb->cdsn*(afb->nairshrink*(MCC_ADN/MCC_WN)*MCC_WNeff)*1e-12 + afb->cdpn*MCC_PDN*1e-6 + afb->cdwn*MCC_WNeff*1e-6;
#ifdef DEBUGBC
  printf("drain: %g %g %g= %g\n", afb->cdsn, afb->cdpn, afb->cdwn, afb->cdrainn);
  printf("  CG=%g CD=%g Cov=%g CGD=%g\n", afb->cgaten, afb->cdrainn, afb->covn, afb->cgdn);
#endif
      
  // PMOS ***********
  vds=MCC_VDDmax-output_voltage;
  vgs=MCC_VDDmax-input_voltage;
  vbs=lotrsparam_p->VBULK;

  if (trans==0)
    mcc_GetInputCapa(MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6, MCC_TEMP, 0, input_voltage, MCC_VDDmax, output_voltage, 0, 0, lotrsparam_p, &afb->cgsp, &afb->cgdpi, &afb->cgpp);
  else
    mcc_GetInputCapa(MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6, MCC_TEMP, MCC_VDDmax, input_voltage, 0, output_voltage, 0, 0, lotrsparam_p, &afb->cgsp, &afb->cgdpi, &afb->cgpp);
  afb->covp=2*(MCC_WPeff+afb->pdeltaw)*1e-6*afb->cgpp;
  afb->cgatep=(MCC_WPeff+afb->pdeltaw)*(MCC_LPeff+afb->pdeltal)*1e-12*afb->cgsp+afb->covp;
#ifdef DEBUGBC
  printf("--gate: %g %g %g = %g\n", afb->cgdpi, afb->cgsp, afb->cgpp, afb->cgatep);
#endif

  afb->cgdp=afb->cgdpi*(MCC_WPeff+afb->pdeltaw)*(MCC_LPeff+afb->pdeltal)*1e-12;

  //CD 
  if (trans==0)
    {
      startv=lotrsparam_p->VBULK-MCC_VDDmax;
      endv=lotrsparam_p->VBULK-output_voltage;
    }
  else
    {
      startv=lotrsparam_p->VBULK-0;
      endv=lotrsparam_p->VBULK-output_voltage;
    }
  afb->cdsp = mcc_calcCDS (MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6, MCC_TEMP, startv, endv, lotrsparam_p);
  afb->cdpp = mcc_calcCDP (MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6, MCC_TEMP, startv, endv,lotrsparam_p);
  afb->cdwp = mcc_calcCDW (MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6, MCC_TEMP, startv, endv, vgs, lotrsparam_p); 

  afb->cdrainp=afb->cdsp*(afb->pairshrink*(MCC_ADP/MCC_WP)*MCC_WPeff)*1e-12 + afb->cdpp*MCC_PDP*1e-6 + afb->cdwp*MCC_WPeff*1e-6;
#ifdef DEBUGBC
  printf("drain: %g %g %g= %g\n", afb->cdsp, afb->cdpp, afb->cdwp, afb->cdrainp);
  printf("  CG=%g CD=%g Cov=%g CGD=%g\n", afb->cgatep, afb->cdrainp, afb->covp, afb->cgdp);
#endif
  afb->capai=afb->cgdp+afb->cgdn + (afb->covp+afb->covn)/2;
}

void mcc_bilan_capa(elp_lotrs_param *lotrsparam_n,elp_lotrs_param *lotrsparam_p)
{
#if 1
  float input_voltage[]={MCC_VTN, MCC_VDDmax-MCC_VTP, MCC_VDDmax*0.01, MCC_VDDmax*0.25, MCC_VDDmax*0.5, MCC_VDDmax*0.75, MCC_VDDmax*0.99};
  float output_voltage[]={MCC_VDDmax*0.01, MCC_VDDmax*0.25, MCC_VDDmax*0.5, MCC_VDDmax*0.75, MCC_VDDmax*0.99};
#else
 
  float input_voltage[]={MCC_VDDmax*0.5};
  float output_voltage[]={MCC_VDDmax*0.99};
#endif
  unsigned int i, j;
  int trans;
  FILE *f;
  char *name, buf[128];
  allforbilan afb;
  float totcgate, totcdrain, totcov, totcgd, ratio;
		  
  qsort(input_voltage, sizeof(input_voltage)/sizeof(*input_voltage), sizeof(*input_voltage), cmpfloat);

  afb.ndeltal = -mcc_calcDL (MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6, lotrsparam_n)*1e6;
  afb.ndeltal+= mcc_calcDLC (MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6, lotrsparam_n)*1e6;
  afb.ndw = mcc_calcDW (MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6, lotrsparam_n)*1e6;
  afb.ndeltaw= -afb.ndw + mcc_calcDWC (MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6, lotrsparam_n)*1e6;
  afb.nairshrink = mcc_getXW (MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6)-afb.ndw/MCC_WNeff;

  afb.pdeltal = -mcc_calcDL (MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS , MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6, lotrsparam_p)*1e6;
  afb.pdeltal+= mcc_calcDLC (MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6, lotrsparam_p)*1e6;
  afb.pdw = mcc_calcDW (MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS , MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6, lotrsparam_p)*1e6;
  afb.pdeltaw= -afb.pdw + mcc_calcDWC (MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6, lotrsparam_p)*1e6;
  afb.pairshrink = mcc_getXW (MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6)-afb.pdw/MCC_WPeff;

  for (trans=0; trans<2; trans++)
    {
      sprintf(buf,"charge_balance_sheet_%s.dat",trans==0?"rf":"fr");
      f = mcc_fopen(name=mcc_debug_prefix(buf),"w") ;
      avt_printExecInfo(f, "#", trans==0?"charge balance sheet R-F":"charge balance sheet F-R", "");

      fprintf(f,"%-10s%-10s", trans==0?"RISE":"FALL", trans==0?"FALL":"RISE");
      fprintf(f,"%-15s%-15s%-15s%-15s", "NMOS", "NMOS", "NMOS", "NMOS");
      fprintf(f,"%-15s%-15s%-15s%-15s", "PMOS", "PMOS", "PMOS", "PMOS");
      fprintf(f,"%-15s%-15s%-15s%-15s", "NMOS+PMOS", "NMOS+PMOS", "NMOS+PMOS", "NMOS+PMOS");
      fprintf(f,"%-15s%-15s\n", "NMOS+PMOS", "NMOS+PMOS");
      fprintf(f,"%-10s%-10s", "VG", "VD");
      fprintf(f,"%-15s%-15s%-15s%-15s", "CGATE", "CDRAIN", "COVERLAP", "CGD");
      fprintf(f,"%-15s%-15s%-15s%-15s", "CGATE", "CDRAIN", "COVERLAP", "CGD");
      fprintf(f,"%-15s%-15s%-15s%-15s", "CGATE", "CDRAIN", "COVERLAPD", "CGD");
      fprintf(f,"%-15s%-15s\n", "CAPAI", "CAPAI/CDRAIN");
      for (j=0; j<sizeof(input_voltage)/sizeof(*input_voltage); j++)
        {
          for (i=0; i< sizeof(output_voltage)/sizeof(*output_voltage); i++)
            {
              get_bilan_capa(lotrsparam_n, lotrsparam_p, trans, &afb, input_voltage[j], output_voltage[i]);

              fprintf(f,"%10g%10g", input_voltage[j], output_voltage[i]);
              fprintf(f,"%15g%15g%15g%15g", afb.cgaten, afb.cdrainn, afb.covn, afb.cgdn);
              fprintf(f,"%15g%15g%15g%15g", afb.cgatep, afb.cdrainp, afb.covp, afb.cgdp);
              totcgate=afb.cgatep+afb.cgaten;
              totcdrain=afb.cdrainp+afb.cdrainn;
              totcov=(afb.covp+afb.covn)/2;
              totcgd=afb.cgdp+afb.cgdn;
              ratio=afb.capai/totcdrain;
              fprintf(f,"%15g%15g%15g%15g", totcgate, totcdrain, totcov, totcgd);
              fprintf(f,"%15g%12.2g", afb.capai, ratio);
              fprintf(f,"\n");
            }
          fprintf(f,"\n");
        }
      fclose(f);
    }

}

void mcc_bilan_capa_simulated_inverter(elp_lotrs_param *lotrsparam_n,elp_lotrs_param *lotrsparam_p, int *maxl)
{
  unsigned int j;
  int trans;
  FILE *f;
  char *name, buf[128], *n;
  allforbilan afb;
		  
  afb.ndeltal = -mcc_calcDL (MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6, lotrsparam_n)*1e6;
  afb.ndeltal+= mcc_calcDLC (MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6, lotrsparam_n)*1e6;
  afb.ndw = mcc_calcDW (MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6, lotrsparam_n)*1e6;
  afb.ndeltaw= -afb.ndw + mcc_calcDWC (MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6, lotrsparam_n)*1e6;
  afb.nairshrink = mcc_getXW (MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6)-afb.ndw/MCC_WNeff;

  afb.pdeltal = -mcc_calcDL (MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS , MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6, lotrsparam_p)*1e6;
  afb.pdeltal+= mcc_calcDLC (MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6, lotrsparam_p)*1e6;
  afb.pdw = mcc_calcDW (MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS , MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6, lotrsparam_p)*1e6;
  afb.pdeltaw= -afb.pdw + mcc_calcDWC (MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6, lotrsparam_p)*1e6;
  afb.pairshrink = mcc_getXW (MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6)-afb.pdw/MCC_WPeff;

  sprintf(buf,"charge_balance_sheet_for_inverter.dat");
  f = mcc_fopen(name=mcc_debug_prefix(buf),"w") ;
  name=mbkstrdup(name);
  avt_printExecInfo(f, "#", "charge balance sheet for simulated inverter", "");
  fprintf(f,"%-15s", "TIME");
  fprintf(f,"%-15s%-15s%-15s%-15s%-15s", "CDRAIN", "COVERLAP", "CGD", "CAPAI", "CAPAI/CDRAIN");
  fprintf(f,"%-15s%-15s%-15s%-15s%-15s", "CDRAIN", "COVERLAP", "CGD", "CAPAI", "CAPAI/CDRAIN");

  for (j=0; j<(unsigned)TRS_CURVS.nbdyna; j++)
    {
      fprintf(f,"%15g", j*V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE);
      for (trans=0; trans<2; trans++)
        {
          get_bilan_capa(lotrsparam_n, lotrsparam_p, trans, &afb, TRS_CURVS.SIMUINV[trans][0][j], TRS_CURVS.SIMUINV[trans][1][j]);
          fprintf(f,"%15g%15g%15g", afb.cdrainp+afb.cdrainn, afb.covn+afb.covp, afb.cgdn+afb.cgdp);
          fprintf(f,"%15g%12.2g", afb.capai, afb.capai/(afb.cdrainp+afb.cdrainn));
        }
      fprintf(f,"\n");
    }
  fclose(f);

  sprintf(buf,"charge_balance_sheet_for_inverter.plt");
  f = mcc_fopen(mcc_debug_prefix(buf),"w") ;
  avt_printExecInfo(f, "#", "charge balance sheet for simulated inverter", "");
  
  fprintf(f,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
  fprintf(f,"set xlabel \"time\"\n") ;
  fprintf(f,"set ylabel \"V or RATIO\"\n") ;
  fprintf(f,"set grid\n") ;
  fprintf(f,"set yrange [:%g]\n",MCC_VDDmax*1.1) ;
  
  for (trans=0; trans<2; trans++)
    {
      fprintf(f,"set xrange [:%g]\n",maxl[trans]*V_FLOAT_TAB[ __SIM_TRAN_STEP ].VALUE) ;
      if (trans==0) n="inverter_rf.spice.dat"; else n="inverter_fr.spice.dat";
      fprintf(f,"plot \"%s\" using 1:2 title \"INPUT\" with lines,\\\n", mcc_debug_prefix(n));
      fprintf(f,"     \"%s\" using 1:3 title \"OUTPUT\" with lines,\\\n", mcc_debug_prefix(n));
      fprintf(f,"     \"%s\" using 1:%d title \"RATIO CAPAI/CDRAIN\" with lines\n", name, 1+5*(trans+1));
      fprintf(f,"pause -1 'Hit CR to finish'\n") ;
    }
  fclose(f);
  mbkfree(name);
}

void mcc_drive_summary(char *bsim)
{
  int stepmax;
  int i, j, k, l,m;
  double *x, res;
  float n_vbs_values[2]={0, -MCC_VDDmax/4};
  float p_vbs_values[2]={0, MCC_VDDmax/4};
  float n_vgs_values[2]={MCC_VDDmax, (MCC_VDDmax-MCC_VTN)/2+MCC_VTN};
  float p_vgs_values[2]={MCC_VDDmax, (MCC_VDDmax-MCC_VTP)/2+MCC_VTP};
  FILE *f;
  float *pvbs, *pvgs;
  char *label;
  char buf[128];
  float rmcc, relp ;
  mcc_debug_internal_trans *trans ;

  stepmax = mcc_ftoi((MCC_VDDmax) / (MCC_DC_STEP)) ;
  f = mcc_fopen(mcc_debug_prefix("techno_summary.txt"),"w") ;
  avt_printExecInfo(f, "*", "", "");

  x=mbkalloc(sizeof(double)*(stepmax+1));
  for (i=0; i<=stepmax; i++) x[i]=i*MCC_DC_STEP;


  if (TRS_CURVS.I_SIMU[0]!=NULL)
  {
    fprintf(f,"\n\n***** Current deviation depending on Vgs *****\n"
            "***** file : %s\n", mcc_debug_prefix("techno_check.plt"));
    for(j = 0 ; j < 6 ; j++)
      {
        if (j==0) fprintf(f,"  %s\n\n", "NMOS");
        else if (j==3) fprintf(f,"  %s\n\n", "PMOS");

        switch (j %3)
          {
          case 0: 
            label="ids=f(vgs), vds=vdd"; break;
          case 1:
            label="ids=f(vds), vgs=vdd"; break;
          case 2:
            label="ids=f(vds), vgs=vdd/2.0"; break;
          }
        fprintf(f,"      %s\n",label);
        res=mbk_compute_curve_error(stepmax, x, TRS_CURVS.I_SIMU[j], stepmax, x,TRS_CURVS.I_BSIM[j]);
        fprintf(f,"        Deviation %s/SPICE  = %.3f\n", bsim, res);
        res=mbk_compute_curve_error(stepmax, x, TRS_CURVS.I_SIMU[j], stepmax, x,TRS_CURVS.I_OPTIM[j]);
        fprintf(f,"        Deviation %s pwl/SPICE  = %.3f\n", bsim, res);
        res=mbk_compute_curve_error(stepmax, x, TRS_CURVS.I_SIMU[j], stepmax, x,TRS_CURVS.I_ELP[j]);
        fprintf(f,"        Deviation ELP/SPICE  = %.3f\n", res);
      }
  }

  if (TRS_CURVS.QINT_SPICE!=NULL)
  {
    fprintf(f,"\n\n***** Charge deviation %s/SPICE *****\n"/*
            "***** file : %s\n", mcc_debug_prefix("i_for_rlin_rsat.plt")*/, bsim);

    for(j = 0 ; j < 2 ; j++)
      {
        fprintf(f,"  %s\n\n", j==0?"NMOS":"PMOS");
        for( m=0 ; m<=1 ; m++ ) {
        fprintf( f,  "  %s voltage variation\n", m==0?"Gate":"Drain" );
        for (k=0; k<2; k++)
          {
            sprintf(buf, "%cmos_qint_mcc_m%c%d_%c.plt", j==0?'n':'p', j==0?'n':'p', 1-k, m==0?'G':'D');
            strcpy(buf, mcc_debug_prefix(buf));
            fprintf(f,"    %s     ***** file : %s\n", k==0?"VDS=VDD":"VDS=0",buf);
            for (l=0; l<4; l++)
              {
                res=mbk_compute_curve_error(stepmax, x, TRS_CURVS.QINT_SPICE[j][k][m][l], stepmax, x,TRS_CURVS.QINT_BSIM[j][k][m][l]);
                fprintf(f,"        Deviation Q%c = %.3f\n",l==0?'G':l==1?'B':l==2?'S':'D', res);
              }
          }
        }
      }
  }

  if (TRS_CURVS.I_VDS_RSATLIN!=NULL)
  {
    fprintf(f,"\n\n***** Current deviation depending on Vbs and Vgs *****\n"
            "***** file : %s\n", mcc_debug_prefix("i_for_rlin_rsat.plt"));
    for(j = 0 ; j < 2 ; j++)
      {
        fprintf(f,"  %s\n\n", j==0?"NMOS":"PMOS");
        if ( j == MCC_TRANS_N )
          pvgs=n_vgs_values, pvbs=n_vbs_values;
        else
          pvgs=p_vgs_values, pvbs=p_vbs_values;
        for (k=0; k<2; k++)
          for (l=0; l<2; l++)
            {
              res=mbk_compute_curve_error(stepmax, x, TRS_CURVS.I_VDS_RSATLIN[j][0][k][l], stepmax, x,TRS_CURVS.I_VDS_RSATLIN[j][2][k][l]);
              fprintf(f,"      Deviation ELP/%s vbs=%.2f vgs=%.2f = %.3f\n",bsim,pvbs[k], pvgs[l], res);
            }
      }
  }
  fprintf(f, "\n\n***** Current measure for an homogen branch *****\n" );
  fprintf(f, "      ibranch (uA) : %-7s %-15s %-15s\n", "spice", "mcc", "elp" );
  for( i=0 ; i<4 ; i++ ) {
    rmcc = 100.0*(TRS_CURVS.tabmccn[i]-TRS_CURVS.tabspin[i])/TRS_CURVS.tabspin[i] ;
    relp = 100.0*(TRS_CURVS.tabelpn[i]-TRS_CURVS.tabspin[i])/TRS_CURVS.tabspin[i] ;
    fprintf(f, "               %d n : %7.3f %7.3f (%+4.1f%%) %7.3f (%+4.1f%%)\n", i+1, TRS_CURVS.tabspin[i], TRS_CURVS.tabmccn[i], rmcc, TRS_CURVS.tabelpn[i], relp );
  }
  for( i=0 ; i<4 ; i++ ) {
    rmcc = 100.0*(TRS_CURVS.tabmccp[i]-TRS_CURVS.tabspip[i])/TRS_CURVS.tabspip[i] ;
    relp = 100.0*(TRS_CURVS.tabelpp[i]-TRS_CURVS.tabspip[i])/TRS_CURVS.tabspip[i] ;
    fprintf(f, "               %d p : %7.3f %7.3f (%+4.1f%%) %7.3f (%+4.1f%%)\n", i+1, TRS_CURVS.tabspip[i], TRS_CURVS.tabmccp[i], rmcc, TRS_CURVS.tabelpp[i], relp );
  }

  fprintf(f, "\n\n***** Internal model parameter\n" );
  for( i=0 ; i<2 ; i++ ) {
    if( i==0 ) {
      fprintf( f, "   TN\n" );
      trans = & MCC_DEBUG_INTERNAL_DATA.TN ;
    }
    else {
      fprintf( f, "   TP\n" );
      trans = & MCC_DEBUG_INTERNAL_DATA.TP ;
    }
    fprintf(f, "          Ids = %g*%g*%g*%g*%g*%g = %g\n", trans->I1,
                                                           trans->I2,
                                                           trans->I3,
                                                           trans->I4,
                                                           trans->I5,
                                                           trans->I6,
                                                           trans->IDS );
  }
  
  mbkfree(x);
  fclose(f);

}

void mcc_check_capa( elp_lotrs_param *lotrsparam_n, elp_lotrs_param *lotrsparam_p )
{
  mcc_check_capa_print( 1, MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6, MCC_TEMP, MCC_VDDmax, lotrsparam_n );
  mcc_check_capa_print( 1, MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6, MCC_TEMP, MCC_VDDmax, lotrsparam_p );
  mcc_check_capa_print( 0, MCC_MODELFILE, MCC_TNMODEL, MCC_NMOS, MCC_NCASE, MCC_LN * 1.0e-6, MCC_WN * 1.0e-6, MCC_TEMP, MCC_VDDmax, lotrsparam_n );
  mcc_check_capa_print( 0, MCC_MODELFILE, MCC_TPMODEL, MCC_PMOS, MCC_PCASE, MCC_LP * 1.0e-6, MCC_WP * 1.0e-6, MCC_TEMP, MCC_VDDmax, lotrsparam_p );
}


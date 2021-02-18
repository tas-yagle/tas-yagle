/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : MCC Version 1                                               */
/*    Fichier : mcc_curv.c                                                  */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

#include "mcc_curv.h"
#include "mcc_util.h"
#include "mcc_genspi.h"
#include "mcc_debug.h"

char* mcc_getmccname( mcc_modellist *model )
{
  char *pt ;
  static char buf[128] ;
  
  switch( model->MODELTYPE ) {
    case MCC_NOMODEL :
    case MCC_MOS2     : pt = "mos2" ;     break ;
    case MCC_BSIM3V3  : pt = "bs32" ;     break ;
    case MCC_MM9 :      pt = "mm9" ;      break ;
    case MCC_MPSP:      pt = "psp" ;      break ;
    case MCC_MPSPB:     pt = "pspb" ;     break ;
    case MCC_EXTMOD:    pt = "ext" ;      break ;
    case MCC_BSIM4 :    
      sprintf( buf, "bs%d", mcc_ftoi(10.0*mcc_getparam_quick(model,__MCC_QUICK_VERSION)) );
      pt = buf;
      break ;
    default :           pt = "unknown" ;
  }

  return pt ;
}
void mcc_printplot(int type, int forcecalc, float nc_vt, float pc_vt, mcc_modellist *modeln, mcc_modellist *modelp)
{
 FILE *file ;
 char *techno ;
 char *name ;
 char *pt ;
 int i ;
 int j ;
 int e ;
 double current = 0.0 ;
 double ncurrent = 0.0 ;
 double pcurrent = 0.0 ;
 double ncurrent_vt = 0.0 ;
 double pcurrent_vt = 0.0 ;
 float vgsn, vgsp;

 if (MCC_CALC_CUR == MCC_SIM_MODE && type == MCC_PARAM) forcecalc=1;

 ncurrent = mcc_current(MCC_VDDmax,MCC_VDDmax,0.0,MCC_TRANS_N) * 1.1 ;
 pcurrent = mcc_current(MCC_VDDmax,MCC_VDDmax,0.0,MCC_TRANS_P) * 1.1 ;

 if (nc_vt>-100) ncurrent_vt=nc_vt;
 else ncurrent_vt = mcc_spicesimcurrent(MCC_VTN,MCC_VDDmax,0.0,MCC_TRANS_N) ; // ABR
 
 if (pc_vt>-100) pcurrent_vt=pc_vt;
 else pcurrent_vt = mcc_spicesimcurrent(MCC_VTP,MCC_VDDmax,0.0,MCC_TRANS_P) ;

 name = mbkalloc(strlen(MCC_SPICEFILE[MCC_PARAM]) + 1) ;
 strcpy(name,MCC_SPICEFILE[MCC_PARAM]) ;
 if((pt = strchr(name,(int)('.'))) != NULL)
   *pt = '\0' ;

 techno = mbkalloc(strlen(MCC_ELPFILE) + 1) ;
 strcpy(techno,MCC_ELPFILE) ;
 if((pt = strchr(techno,(int)('.'))) != NULL)
   *pt = '\0' ;

 file = mbkfopen(techno,"plt",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");
 mcc_get_np_vgs(&vgsn, &vgsp);

 e = 0 ;
 for(i = 2 ; i < 6 ; i++)
  {
   if(i < 4)
   {
    current = ncurrent ;
   }
   else
    current = pcurrent ;
   fprintf(file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
   fprintf(file,"set title \"%s\"\n", current==ncurrent?"NMOS":"PMOS");
   fprintf(file,"set grid\n");
   if(i%2 == 0)
     fprintf(file,"set xlabel \"VGS (V)\"\n") ;
   else
     fprintf(file,"set xlabel \"VDS (V)\"\n") ;
   fprintf(file,"set ylabel \"IDS (A)\"\n") ;
   fprintf(file,"set key top left\n") ;
   
   if ( i == 2 || i == 4 ) 
    {
     fprintf(file,"set xrange [:%g]\n",(i<4)?MCC_VTN:MCC_VTP);
     /*fprintf(file,"set yrange [:]\n");*/
     fprintf(file,"plot '%s.dat' using 1:%d title 'spice Isub vds=vdd'  with lines",
                  mcc_debug_prefix(name),i+e) ;
     if ( forcecalc )
       fprintf(file,",\\\n'%s_calc.dat' using 1:%d title '%s Isub vds=vdd'  with lines",mcc_debug_prefix(name),i+e,mcc_getmccname(i<4?modeln:modelp)) ;
     fprintf(file,"\n") ;
     fprintf(file,"pause -1 'Hit CR to finish'\n") ;
     fprintf(file,"set term postscript eps monochrome \"Times-Roman\" 20\n") ;
     fprintf(file,"set output \"%s_%d.eps\"\n",techno,i) ;
     fprintf(file,"replot\n") ;
    }
   fprintf(file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
   fprintf(file,"set xrange [0:%g]\n",MCC_VDDmax*1.05 );
   /*fprintf(file,"set yrange [0:%e]\n",current);*/
   fprintf(file,"plot '%s.dat' using 1:%d title 'spice'  with lines,\\\n '%s.dat' using 1:%d title 'elp' with lines",
                mcc_debug_prefix(name),i+e,techno,i+e) ;
   if ( MCC_OPTIM_MODE && type == MCC_PARAM )
     fprintf(file,",\\\n'%s_opt.dat' using 1:%d title '%s pwl'  with lines",mcc_debug_prefix(name),i+e,mcc_getmccname(i<4?modeln:modelp)) ;
   if ( forcecalc )
     fprintf(file,",\\\n'%s_calc.dat' using 1:%d title '%s'  with lines",mcc_debug_prefix(name),i+e,mcc_getmccname(i<4?modeln:modelp)) ;
   if((i%2 != 0)/* && 
      (mcc_ftol(MCC_VGS * 1000.0) != mcc_ftol(MCC_VDDmax * 1000.0))*/)
    {
     e++ ;
     fprintf(file,",\\\n '%s.dat' using 1:%d title 'spice vgs=%g ' with lines ,\\\n '%s.dat' using 1:%d title 'elp vgs=%g ' with lines",
                  mcc_debug_prefix(name),i+e,i<4?vgsn:vgsp,techno,i+e,i<4?vgsn:vgsp) ;
     if ( MCC_OPTIM_MODE && type == MCC_PARAM )
       fprintf(file,",\\\n'%s_opt.dat' using 1:%d title '%s pwl vgs=%g'  with lines",mcc_debug_prefix(name),i+e,mcc_getmccname(i<4?modeln:modelp),i<4?vgsn:vgsp) ;
     if (forcecalc )
       fprintf(file,",\\\n'%s_calc.dat' using 1:%d title '%s vgs=%g'  with lines",mcc_debug_prefix(name),i+e,mcc_getmccname(i<4?modeln:modelp),i<4?vgsn:vgsp) ;
    }

   if(type != MCC_PARAM)
     fprintf(file,",\\\n '%s_fit.dat' using 1:%d title 'after fit' with lines",techno,i+e) ;
   fprintf(file,"\n") ;
   fprintf(file,"pause -1 'Hit CR to finish'\n") ;
   fprintf(file,"set term postscript eps monochrome \"Times-Roman\" 20\n") ;
   fprintf(file,"set output \"%s_%d.eps\"\n",techno,i) ;
   fprintf(file,"replot\n") ;
  }

 if(fclose(file) != 0)
  {
   fprintf(stderr,"\nmcc error : can't close file %s.plt\n",techno) ;
   EXIT(1);
  }

 if(type == MCC_PARAM)
   return ;

 mbkfree(techno) ;
 mbkfree(name) ;

 techno = mbkalloc(strlen(MCC_ELPFILE) + 10) ;
 name = mbkalloc(strlen(MCC_ELPFILE) + 10) ;
 strcpy(name,MCC_ELPFILE) ; 
 if((pt = strchr(name,(int)('.'))) != NULL)
   *pt = '\0' ;
 sprintf(techno,"%s_fit",name) ;

 file = mbkfopen(techno,"plt",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");

 mbkfree(name) ;

 for(i = 0 ; i < MCC_SPICENB - 1 ; i++)
  {
   name = mbkalloc(strlen(MCC_SPICEFILE[i]) + 1) ;
   strcpy(name,MCC_SPICEFILE[i]) ; 
   if((pt = strchr(name,(int)('.'))) != NULL)
     *pt = '\0' ;

   fprintf(file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
   fprintf(file,"set xlabel \"TIME (PS)\"\n") ;
   fprintf(file,"set ylabel \"VOUT (V)\"\n") ;
   fprintf(file,"set xtics (") ;
   j = 0 ;
   while(j < (int)(V_FLOAT_TAB[__SIM_TIME].VALUE*1e9))
    {
     j+=5 ;
     fprintf(file,"%d.0e-9,",j) ;
    }
   j+=5 ;
   fprintf(file,"%d.0e-9)\n",j) ;
   fprintf(file,"set xrange [0:%e]\n",(V_FLOAT_TAB[__SIM_TIME].VALUE*1e9 + 1)*1.0e-9) ;
   fprintf(file,"set yrange [0:%g]\n",MCC_VDDmax*1.05) ;
   fprintf(file,"plot ") ;
   for(j = 0 ; j < MCC_INSNUMB ; j++)
     fprintf(file,"'%s.dat' using 1:%d title 's%d' with lines, ",mcc_debug_prefix(name),j+2,j) ;
   fprintf(file,"'%s.dat' using 1:%d title 's%d' with lines\n",mcc_debug_prefix(name),j+2,j) ;
   fprintf(file,"pause -1 'Hit CR to finish'\n") ;
   fprintf(file,"set term postscript eps monochrome \"Times-Roman\" 20\n") ;
   fprintf(file,"set output \"%s.eps\"\n",mcc_debug_prefix(name)) ;
   fprintf(file,"replot\n") ;

   mbkfree(name) ;
  }

if(fclose(file) != 0)
  {
   fprintf(stderr,"\nmcc error : can't close file %s.plt\n",techno) ;
   EXIT(1);
  }

 mbkfree(techno) ;
}

void mcc_technoParameters(lotrsparam_n,lotrsparam_p)
elp_lotrs_param *lotrsparam_n;
elp_lotrs_param *lotrsparam_p;
{
 elpmodel_list *modeln ;
 elpmodel_list *modelp ;

 if(MCC_ELPFILE != NULL) strcpy(elpTechnoFile,MCC_ELPFILE) ;

 elpLoadOnceElp();
 /*
 if(elpLoadElp())
 {
  fprintf(stderr,"\nmcc error : bad technology file %s\n",MCC_ELPFILE) ;
  EXIT(1) ;
 }
*/
 modeln = elpGetParamModel(MCC_TNMODEL,MCC_LN,MCC_WN,TRANSN,elpGeneral[elpGVDDMAX],MCC_NCASE,lotrsparam_n) ;
 modelp = elpGetParamModel(MCC_TPMODEL,MCC_LP,MCC_WP,TRANSP,elpGeneral[elpGVDDMAX],MCC_PCASE,lotrsparam_p) ;

 MCC_VTN = modeln->elpModel[elpVT] ;
 MCC_KTN = modeln->elpModel[elpKT] ;
 MCC_KSN = modeln->elpModel[elpKS] ;
 MCC_KRN = modeln->elpModel[elpKR] ;
 MCC_AN  = modeln->elpModel[elpA] ;
 MCC_BN  = modeln->elpModel[elpB] ;
 MCC_RNT = modeln->elpModel[elpRT] ;
 MCC_RNS = modeln->elpModel[elpRS] ;
 MCC_DWN = modeln->elpShrink[elpDW] ;
 MCC_DLN = modeln->elpShrink[elpDL] ;
 MCC_XLN = modeln->elpShrink[elpLMLT] ;
 MCC_XWN = modeln->elpShrink[elpWMLT] ;
 MCC_VTP = modelp->elpModel[elpVT] ;
 MCC_KTP = modelp->elpModel[elpKT] ;
 MCC_KSP = modelp->elpModel[elpKS] ;
 MCC_KRP = modelp->elpModel[elpKR] ;
 MCC_AP  = modelp->elpModel[elpA] ;
 MCC_BP  = modelp->elpModel[elpB] ;
 MCC_RPT = modelp->elpModel[elpRT] ;
 MCC_RPS = modelp->elpModel[elpRS] ;
 MCC_DWP = modelp->elpShrink[elpDW] ;
 MCC_DLP = modelp->elpShrink[elpDL] ;
 MCC_XLP = modelp->elpShrink[elpLMLT] ;
 MCC_XWP = modelp->elpShrink[elpWMLT] ;
 MCC_WNeff = MCC_XWN*MCC_WN + MCC_DWN ;
 MCC_LNeff = MCC_XLN*MCC_LN + MCC_DLN ;
 MCC_WPeff = MCC_XWP*MCC_WP + MCC_DWP ;
 MCC_LPeff = MCC_XLP*MCC_LP + MCC_DLP ;
 MCC_VDDmax = elpGeneral[elpGVDDMAX] ;
}

void mcc_visu()
{
 fprintf(stdout,"MCC CURRENT PARAMETERS :\n") ;
 fprintf(stdout,"DWN      = %g\n",MCC_DWN) ;
 fprintf(stdout,"DLN      = %g\n",MCC_DLN) ;
 fprintf(stdout,"VTN      = %g\n",MCC_VTN) ;
 fprintf(stdout,"KTN      = %g\n",MCC_KTN) ;
 fprintf(stdout,"KSN      = %g\n",MCC_KSN) ;
 fprintf(stdout,"KRN      = %g\n",MCC_KRN) ;
 fprintf(stdout,"AN       = %g\n",MCC_AN) ;
 fprintf(stdout,"BN       = %g\n",MCC_BN) ;
 fprintf(stdout,"RNT      = %g\n",MCC_RNT) ;
 fprintf(stdout,"RNS      = %g\n",MCC_RNS) ;
 fprintf(stdout,"DWP      = %g\n",MCC_DWP) ;
 fprintf(stdout,"DLP      = %g\n",MCC_DLP) ;
 fprintf(stdout,"VTP      = %g\n",MCC_VTP) ;
 fprintf(stdout,"KTP      = %g\n",MCC_KTP) ;
 fprintf(stdout,"KSP      = %g\n",MCC_KSP) ;
 fprintf(stdout,"KRP      = %g\n",MCC_KRP) ;
 fprintf(stdout,"AP       = %g\n",MCC_AP) ;
 fprintf(stdout,"BP       = %g\n",MCC_BP) ;
 fprintf(stdout,"RPT      = %g\n",MCC_RPT) ;
 fprintf(stdout,"RPS      = %g\n",MCC_RPS) ;
 fprintf(stdout,"XWN      = %g\n",MCC_XWN) ;
 fprintf(stdout,"XLN      = %g\n",MCC_XLN) ;
 fprintf(stdout,"XWP      = %g\n",MCC_XWP) ;
 fprintf(stdout,"XLP      = %g\n",MCC_XLP) ;
 fprintf(stdout,"VDDmax   = %g\n",MCC_VDDmax) ;
 fprintf(stdout,"TRANSTEP = %g\n",V_FLOAT_TAB[__SIM_TRAN_STEP].VALUE*1e9) ;
 fprintf(stdout,"DCSTEP   = %g\n",V_FLOAT_TAB[__SIM_TIME].VALUE*1e9) ;
 fprintf(stdout,"WNeff    = %g\n",MCC_WNeff) ;
 fprintf(stdout,"LNeff    = %g\n",MCC_LNeff) ;
 fprintf(stdout,"WPeff    = %g\n",MCC_WPeff) ;
 fprintf(stdout,"LPeff    = %g\n",MCC_LPeff) ;
}

void mcc_drvmtfile(file)
FILE *file;
{
 double var = 0.0 ;
 double i[6];
 int idx=0, k;

 fprintf(file,"#volt ");
 fprintf(file,"idnsat ");
 fprintf(file,"idnres ");
 if(mcc_ftol(MCC_VGS * 1000.0) != mcc_ftol(MCC_VDDmax * 1000.0))
 fprintf(file,"idnvgs ");
 fprintf(file,"idpsat ");
 fprintf(file,"idpres ");
 if(mcc_ftol(MCC_VGS * 1000.0) != mcc_ftol(MCC_VDDmax * 1000.0))
 fprintf(file,"idpvgs");
 fprintf(file,"\n");

 for (k=0; k<6; k++)
    {
      TRS_CURVS.I_ELP[k]=mbkalloc(sizeof(double)*TRS_CURVS.nb);
    }

 while(var <= MCC_VDDmax)
 {
//  if(mcc_ftol(MCC_VGS * 1000.0) != mcc_ftol(MCC_VDDmax * 1000.0))
  {
   fprintf(file,"%.3e %.3e %.3e %.3e %.3e %.3e %.3e\n",var, 
     i[0]=mcc_current(var,MCC_VDDmax,0.0,MCC_TRANS_N),
     i[1]=mcc_current(MCC_VDDmax,var,0.0,MCC_TRANS_N),
     i[2]=mcc_current(MCC_VGS,var,0.0,MCC_TRANS_N),
     i[3]=mcc_current(var,MCC_VDDmax,0.0,MCC_TRANS_P),
     i[4]=mcc_current(MCC_VDDmax,var,0.0,MCC_TRANS_P),
     i[5]=mcc_current(MCC_VGS,var,0.0,MCC_TRANS_P));
     for (k=0; k<6; k++) TRS_CURVS.I_ELP[k][idx]=i[k];
  }
/*  else
  {
   fprintf(file,"%.3e %.3e %.3e %.3e %.3e\n",var, 
     mcc_current(var,MCC_VDDmax,0.0,MCC_TRANS_N),
     mcc_current(MCC_VGS,var,0.0,MCC_TRANS_N),
     mcc_current(var,MCC_VDDmax,0.0,MCC_TRANS_P),
     mcc_current(MCC_VGS,var,0.0,MCC_TRANS_P));
  }*/
  var += MCC_DC_STEP;
  idx++;
 }
}

void mcc_gencurv(type,lotrsparam_n,lotrsparam_p)
int type ;
elp_lotrs_param *lotrsparam_n;
elp_lotrs_param *lotrsparam_p;
{
 FILE *file ;
 char *nomout ;
 char *pt ;

 mcc_technoParameters(lotrsparam_n,lotrsparam_p);

/* MCC_CALC_CUR=MCC_CALC_MODE;
 printf("%g\n",mcc_spicecurrent(MCC_VDDmax, MCC_VDDmax, 0, MCC_TRANS_P, lotrsparam_p));
 exit(1);*/
 nomout = mbkalloc(strlen(MCC_ELPFILE) + 10) ;
 strcpy(nomout,MCC_ELPFILE) ;
 if((pt = strchr(nomout,(int)('.'))) != NULL)
   *pt = '\0' ;

 if(type != MCC_PARAM)
   sprintf(nomout+strlen(nomout),"_fit") ;

 file = mbkfopen(nomout,"dat",WRITE_TEXT) ;
 avt_printExecInfo(file, "#", "", "");

 if(file == NULL)
  {
   fprintf(stderr,"\nmcc error : can't open file %s.dat\n",nomout) ;
   EXIT(1);
  }

 mcc_drvmtfile(file);

 if(fclose(file) != 0)
  {
   fprintf(stderr,"\nmcc error : can't close file %s.dat\n",nomout) ;
   EXIT(1);
  }

 mbkfree(nomout) ;
}

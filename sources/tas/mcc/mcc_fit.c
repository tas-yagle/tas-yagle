/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : MCC Version 1                                               */
/*    Fichier : mcc_fit.c                                                   */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

#include "mcc_fit.h"
#include "mcc_debug.h"

long MCC_TASUD ;
long MCC_TASDU ;
long MCC_TASDD ;
long MCC_TASUU ;
long MCC_TASSUD ;
long MCC_TASSDU ;
long MCC_TASSDD ;
long MCC_TASSUU ;

int mcc_getdelay(char *filename,char *in,char *out,int mode,char sin,char sout,FILE *file)
{
 ttvfig_list *ttvfig ;
 chain_list *chain1 ;
 chain_list *chain2 ;
 ttvpath_list *path ;
 ttvpath_list *headpath ;
 ttvcritic_list *headcritic ;
 ttvcritic_list *critic ;
 ttvcritic_list *criticx ;
 long nbud ;
 long nbdu ;
 long nb ;
 char pin ;
 char pout ;
 char trans[2];

 ttvfig = ttv_getttvfig(filename,(long)0) ;

 if(ttvfig == NULL)
  return(0) ;

 chain1 = addchain(NULL,(void *)in) ;
 chain2 = addchain(NULL,(void *)out) ;

 headpath = ttv_getcriticpath(ttvfig,NULL,chain1,chain2,TTV_DELAY_MAX,TTV_DELAY_MIN,100,TTV_FIND_MAX|TTV_FIND_LINE) ;
 path = headpath ;

 if(path != NULL)
  {
   while(path != NULL)
    {
     if((path->NODE->TYPE & TTV_NODE_UP) ==  TTV_NODE_UP)
       pin = 'U' ;
     else
       pin = 'D' ;

     if((path->ROOT->TYPE & TTV_NODE_UP) ==  TTV_NODE_UP)
       pout = 'U' ;
     else
       pout = 'D' ;

     if(((sin == pin) || (sin == 'X')) && ((sout == pout) || (sout == 'X')))
      {
       headcritic = ttv_getcritic(ttvfig,NULL,path->ROOT,path->NODE,path->LATCH,path->CMDLATCH,(path->TYPE & ~(TTV_FIND_PATH)) | TTV_FIND_LINE) ;
       break ;
      }
     path = path->NEXT ;
     if(path == NULL)
         return(0) ;
    }
  }
 else
  {
   return(0) ;
  }

 ttv_freepathlist(headpath) ;

 if(headcritic == NULL)
  return(0) ;

 MCC_TASUD = (long)0 ;
 MCC_TASDU = (long)0 ;
 MCC_TASDD = (long)0 ;
 MCC_TASUU = (long)0 ;
 MCC_TASSUD = (long)0 ;
 MCC_TASSDU = (long)0 ;
 MCC_TASSDD = (long)0 ;
 MCC_TASSUU = (long)0 ;
 nbud = (long)0 ;
 nbdu = (long)0 ;
 nb = (long)0 ;

 if(mode == 0)
 for(critic = headcritic->NEXT ; critic->NEXT != NULL ; critic = critic->NEXT)
   {
    criticx = critic->NEXT ;
    if(nb == (long)0)
     {
      nb++ ;
      continue ;
     }
    if(criticx->NEXT->NEXT == NULL)
      break ;
    if(critic->SNODE == TTV_UP)
     {
      MCC_TASUD += criticx->DELAY ;
      MCC_TASSUD = criticx->SLOPE ;
      nbud ++ ;
     }
    else
     {
      MCC_TASDU += criticx->DELAY ;
      MCC_TASSDU = criticx->SLOPE ;
      nbdu ++ ;
     }
   }
 else // that case if for mcc debug 
   {
    sprintf(trans,"%c%c",sin,sout);
    for(critic = headcritic->NEXT ; critic->NEXT != NULL ; critic = critic->NEXT) ;
    if(critic->SNODE == TTV_DOWN)
     {
      MCC_TASUD += critic->DELAY ;
      MCC_TASSUD = critic->SLOPE ;
      nbud ++ ;
     }
    else
     {
      MCC_TASDU += critic->DELAY ;
      MCC_TASSDU = critic->SLOPE ;
      nbdu ++ ;
     }
   }

 ttv_freecriticlist(headcritic) ;

 if(nbud != 0)
   MCC_TASUD = (MCC_TASUD / nbud) ;
 else
   MCC_TASUD = 0 ;
 if(nbdu != 0)
   MCC_TASDU = (MCC_TASDU / nbdu) ;
 else
   MCC_TASDU = 0 ;

 if ( mode ) 
  {
   if ( !strcmp(trans,"UU") )
    {
     if ( MCC_TASUD > MCC_TASDU )
      {
       MCC_TASUU = MCC_TASUD;
       MCC_TASSUU = MCC_TASSUD;
       MCC_TASUD = 0;
       MCC_TASSUD = 0;
      }
     else
      {
       MCC_TASUU  = MCC_TASDU;
       MCC_TASSUU = MCC_TASSDU;
       MCC_TASDU  = 0;
       MCC_TASSDU = 0;
      }
    }
   else if ( !strcmp(trans,"DD") )
    {
     if ( MCC_TASUD > MCC_TASDU )
      {
       MCC_TASDD  = MCC_TASUD;
       MCC_TASSDD = MCC_TASSUD;
       MCC_TASUD  = 0;
       MCC_TASSUD = 0;
      }
    }
   fprintf(file==NULL?stdout:file,"\n%s tas delay : UD=%ld DU=%ld DD=%ld UU=%ld slope : UD=%ld DU=%ld DD=%ld UU=%ld\n",
                  filename,(long)(MCC_TASUD/TTV_UNIT),(long)(MCC_TASDU/TTV_UNIT),(long)(MCC_TASDD/TTV_UNIT),(long)(MCC_TASUU/TTV_UNIT),(long)(MCC_TASSUD/TTV_UNIT),(long)(MCC_TASSDU/TTV_UNIT),(long)(MCC_TASSDD/TTV_UNIT),(long)(MCC_TASSUU/TTV_UNIT)) ;
  }
 else
  {
   fprintf(file==NULL?stdout:file,"\n%s tas delay : UD=%ld DU=%ld slope : UD=%ld DU=%ld\n",
                  filename,(long)(MCC_TASUD/TTV_UNIT),(long)(MCC_TASDU/TTV_UNIT),(long)(MCC_TASSUD/TTV_UNIT),(long)(MCC_TASSDU/TTV_UNIT)) ;
  }
 ttv_freeallttvfig(ttvfig) ;

 MCC_TASUD /= TTV_UNIT ;
 MCC_TASDU /= TTV_UNIT ;
 MCC_TASDD /= TTV_UNIT ;
 MCC_TASUU /= TTV_UNIT ;
 MCC_TASSUD /= TTV_UNIT ;
 MCC_TASSDU /= TTV_UNIT ;
 MCC_TASSDD /= TTV_UNIT ;
 MCC_TASSUU /= TTV_UNIT ;

 return(1) ;
}

void mcc_runtas(filename,slope,capa)
char *filename ;
int slope ;
int capa ;
{
 pid_t pid ;
 int status ;
 char *argv[6] ;
 char env[1024] ;
 char slopes[48] ;
 char capas[48] ;
 char *name ;
 int n = 0 ;

 if(slope > 0)
   sprintf(slopes,"-slope=%d",slope) ;

 if(capa > 0)
   sprintf(capas,"-cout=%.3f",(float)capa/1000.0) ;

 pid = vfork() ;

 if(pid == (pid_t)(-1))
  {
   fprintf(stderr,"\nmcc error: can't run %s\n",MCC_SPICENAME) ;
   EXIT(1) ;
  }
 else if(pid == 0)
  {
   sprintf(env,"MBK_SPI_TN=%s",MCC_TNMODEL) ;
   name = (char *)mbkalloc(strlen(env) + 1) ;
   strcpy(name,env) ;
   putenv(name) ;
   sprintf(env,"MBK_SPI_TP=%s",MCC_TPMODEL) ;
   name = (char *)mbkalloc(strlen(env) + 1) ;
   strcpy(name,env) ;
   putenv(name) ;
   sprintf(env,"ELP_TECHNO_NAME=%s",MCC_ELPFILE) ;
   name = (char *)mbkalloc(strlen(env) + 1) ;
   strcpy(name,env) ;
   putenv(name) ;
   strcpy(env,"SIM_SLOPE_INTERNAL_THRESHOLDS=yes") ;
   name = (char *)mbkalloc(strlen(env) + 1) ;
   strcpy(name,env) ;
   putenv(name) ;
   argv[n++] = MCC_TASNAME ;
   argv[n++] = "-t$" ;
   argv[n++] = "-in=spi" ;
   argv[n++] = "-s" ;
   if(slope > 0)
     argv[n++] = slopes ;
   if(capa > 0)
     argv[n++] = capas ;
   name = filename ;
   argv[n++] = name ;
   argv[n++] = NULL ;
   n = 0 ;
   fprintf(stdout,"\nRUN: ") ;
    while(argv[n] != NULL)
    {
     fprintf(stdout,"%s ",argv[n]) ;
     n++ ;
     fflush(stdout) ;
    }
   fprintf(stdout,"\n") ;
   fflush(stdout) ;
   status = execvp(argv[0],argv) ;

   if(status != -1) {
     EXIT(0) ;
   }
   else {
     EXIT(1) ;
   }
  }
 else
  {
   n = mbkwaitpid(pid,1,&status) ;
   if((n == 0) || (WIFEXITED(status) == 0))
    {
     fprintf(stderr,"\nmcc error: can not execute tas\n") ;
    }
   else if(WEXITSTATUS(status) != 0)
    {
     fprintf(stderr,"\nmcc warning: tas exit with non zero value\n") ;
    }
   else
    {
     fprintf(stdout,"Execution Completed\n") ;
    }
  }
}

void mcc_fit(elp_lotrs_param *lotrsparam_n,elp_lotrs_param *lotrsparam_p,int componly)
{
 double error ;
 double errorx ;
 double tref ;
 long tasdelay ;
 long spicedelay ;
 int i ;
 int maxloop ;
 char *filename ;
 char s1[8] ;
 char s2[8] ;
 char buf[1024], buf0[1024];
 FILE *f;

 strcpy(buf0,MCC_ELPFILE); // modifier a l'appel de tas

 if (!componly) mcc_drvelp(MCC_PARAM,MCC_TRANS_B,lotrsparam_n,lotrsparam_p,MCC_DRV_ALL_MODEL) ;
// mcc_gencurv(MCC_PARAM,lotrsparam_n,lotrsparam_p) ;

 f=fopen(mcc_debug_prefix("fit_output.log"),"w");
 for(i = 0 ; i < MCC_TASNB ; i++)
  {
   if ( i == MCC_FIT_A && ( avt_gethashvar ("MCC_DONT_FITA" )) )
     continue;
   if ( (i == MCC_FIT_CG || i==MCC_FIT_CG_NOCAPA) && ( avt_gethashvar ("MCC_DONT_FITCG" )) )
     continue;
   if ( i == MCC_FIT_CDN && ( avt_gethashvar ("MCC_DONT_FITCDN" )) )
     continue;
   if ( i == MCC_FIT_CDP && ( avt_gethashvar ("MCC_DONT_FITCDP" )) )
     continue;
   maxloop = MCC_NBMAXLOOP ;
   filename = mcc_getfilename(MCC_TASFILE[i]) ;
   error = 100.0 ;
   while(error > MCC_ERROR) 
    {
     if (!componly) mcc_drvelp(i,MCC_TRANS_B,lotrsparam_n,lotrsparam_p,MCC_DRV_ALL_MODEL) ;
//     mcc_runtas_tcl(mcc_debug_prefix(mcc_getfilename(MCC_TASFILE[i])),V_FLOAT_TAB[__FRONT_CON].VALUE,V_FLOAT_TAB[__TAS_CAPAOUT].VALUE,NULL) ;
     strcpy(buf,mcc_debug_prefix(MCC_SPICEFILE[i]));
     mcc_runtas_tcl(buf,V_FLOAT_TAB[__FRONT_CON].VALUE,0,NULL,mcc_debug_prefix(mcc_getfilename(MCC_TASFILE[i]))) ;
     mbkfree(MCC_ELPFILE);
     MCC_ELPFILE=mbkstrdup(buf0);
     sprintf(s1,"s0") ;
     sprintf(s2,"s%d",MCC_INSNUMB) ;
     if(mcc_getdelay(mcc_debug_prefix(filename),s1,s2,0,'X','X',f) == 0)
      {
       fprintf(stderr,"\nmcc error : can't extract data from file %s\n",
        MCC_TASFILE[i]) ;
       EXIT(1) ;
      }
     switch(i)
       {
        case MCC_FIT_A : tasdelay = MCC_TASUD + MCC_TASDU ;
                         spicedelay = MCC_SPICEUD_FITA + MCC_SPICEDU_FITA ;
                         if (f!=NULL) fprintf(f,"%s spice delay : UD=%ld DU=%ld slope : UD=%ld DU=%ld\n",filename,MCC_SPICEUD_FITA,MCC_SPICEDU_FITA,MCC_SPICEFUD_FITA,MCC_SPICEFDU_FITA) ;
                         if(MCC_TASUD > MCC_SPICEUD_FITA)
                          error = (double)((MCC_TASUD - MCC_SPICEUD_FITA) / 
                                           (double)(MCC_TASUD)) ;
                         else
                          error = (double)((MCC_SPICEUD_FITA - MCC_TASUD) / 
                                           (double)(MCC_SPICEUD_FITA)) ;
                         if(MCC_TASDU > MCC_SPICEDU_FITA)
                          errorx = (double)((MCC_TASDU - MCC_SPICEDU_FITA) / 
                                            (double)(MCC_TASDU)) ;
                         else
                          errorx = (double)((MCC_SPICEDU_FITA - MCC_TASDU) / 
                                            (double)(MCC_SPICEDU_FITA)) ;
                         if(errorx > error)
                           error = errorx ;
 
                         if (componly) break;

                         tref = (double)((double)(MCC_TASUD) /
                                         (double)(MCC_SPICEUD_FITA)) ;
                         MCC_AN = MCC_AN * tref ;
                         tref = (double)((double)(MCC_TASDU) / 
                                         (double)(MCC_SPICEDU_FITA)) ;
                         MCC_AP = MCC_AP * tref ;
                        break ;
        case MCC_FIT_CG_NOCAPA:
                          tasdelay = MCC_TASUD + MCC_TASDU ;
                          spicedelay = MCC_SPICEUD_FITCGNOCAPA + MCC_SPICEDU_FITCGNOCAPA;
                          if (f!=NULL) fprintf(f,"%s spice delay : UD=%ld DU=%ld slope : UD=%ld DU=%ld\n",filename,MCC_SPICEUD_FITCGNOCAPA,MCC_SPICEDU_FITCGNOCAPA,MCC_SPICEFUD_FITCGNOCAPA,MCC_SPICEFDU_FITCGNOCAPA) ;
                          tref = (double)((double)(spicedelay) / 
                                          (double)(tasdelay)) ;
                          if(tasdelay > spicedelay)
                           error = (double)((tasdelay - spicedelay) / 
                                            (double)(tasdelay)) ;
                          else
                           error = (double)((spicedelay - tasdelay) / 
                                            (double)(spicedelay)) ;
                          if (componly) break;
                          MCC_CGSN = MCC_CGSN * tref ;
                          MCC_CGSP = MCC_CGSP * tref ;
                          MCC_CGDN = MCC_CGDN * tref ;
                          MCC_CGDCN = MCC_CGDCN * tref ;
                          MCC_CGDP = MCC_CGDP * tref ;
                          MCC_CGDCP = MCC_CGDCP * tref ;
                          MCC_CGSIN = MCC_CGSIN * tref ;
                          MCC_CGSICN = MCC_CGSICN * tref ;
                          MCC_CGSIP = MCC_CGSIP * tref ;
                          MCC_CGSICP = MCC_CGSICP * tref ;
                          MCC_CGPN = MCC_CGPN * tref ;
                          MCC_CGPP = MCC_CGPP * tref ;
                          break ;
        case MCC_FIT_CG : tasdelay = MCC_TASUD + MCC_TASDU ;
                          spicedelay = MCC_SPICEUD_FITCG + MCC_SPICEDU_FITCG;
                          if (f!=NULL) fprintf(f,"%s spice delay : UD=%ld DU=%ld slope : UD=%ld DU=%ld\n",filename,MCC_SPICEUD_FITCG,MCC_SPICEDU_FITCG,MCC_SPICEFUD_FITCG,MCC_SPICEFDU_FITCG) ;
                          if(tasdelay > spicedelay)
                           error = (double)((tasdelay - spicedelay) / 
                                            (double)(tasdelay)) ;
                          else
                           error = (double)((spicedelay - tasdelay) / 
                                            (double)(spicedelay)) ;
                          if (componly) break;
                          tref = (double)((double)(spicedelay) / 
                                          (double)(tasdelay)) ;
                          MCC_CGSN = MCC_CGSN * tref ;
                          MCC_CGSP = MCC_CGSP * tref ;
                          MCC_CGDN = MCC_CGDN * tref ;
                          MCC_CGDCN = MCC_CGDCN * tref ;
                          MCC_CGDP = MCC_CGDP * tref ;
                          MCC_CGDCP = MCC_CGDCP * tref ;
                          MCC_CGSIN = MCC_CGSIN * tref ;
                          MCC_CGSICN = MCC_CGSICN * tref ;
                          MCC_CGSIP = MCC_CGSIP * tref ;
                          MCC_CGSICP = MCC_CGSICP * tref ;
                          MCC_CGPN = MCC_CGPN * tref ;
                          MCC_CGPP = MCC_CGPP * tref ;
                          break ;
        case MCC_FIT_CDN : tasdelay = MCC_TASUD + MCC_TASDU ;
                           spicedelay = MCC_SPICEUD_FITDN + MCC_SPICEDU_FITDN;
                           if (f!=NULL) fprintf(f,"%s spice delay : UD=%ld DU=%ld slope : UD=%ld DU=%ld\n",filename,MCC_SPICEUD_FITDN,MCC_SPICEDU_FITDN,MCC_SPICEFUD_FITDN,MCC_SPICEFDU_FITDN) ;
                           if(tasdelay > spicedelay)
                            error = (double)((tasdelay - spicedelay) /
                                             (double)(tasdelay)) ;
                           else
                            error = (double)((spicedelay - tasdelay) /
                                             (double)(spicedelay)) ;
                           if (componly) break;
                           tref = (double)((double)(spicedelay) /        
                                           (double)(tasdelay)) ;
                           MCC_CDSN = MCC_CDSN * tref ;
                           MCC_CDS_U_N = MCC_CDS_U_N * tref ;
                           MCC_CDS_D_N = MCC_CDS_D_N * tref ;
                           MCC_CDPN = MCC_CDPN * tref ;
                           MCC_CDP_U_N = MCC_CDP_U_N * tref ;
                           MCC_CDP_D_N = MCC_CDP_D_N * tref ;
                           MCC_CDWN = MCC_CDWN * tref ;
                           MCC_CDW_U_N = MCC_CDW_U_N * tref ;
                           MCC_CDW_D_N = MCC_CDW_D_N * tref ;
                           MCC_CSSN = MCC_CDSN ;
                           MCC_CSS_U_N = MCC_CDS_U_N ;
                           MCC_CSS_D_N = MCC_CDS_D_N ;
                           MCC_CSPN = MCC_CDPN ;
                           MCC_CSP_U_N = MCC_CDP_U_N ;
                           MCC_CSP_D_N = MCC_CDP_D_N ;
                           MCC_CSWN = MCC_CDWN ;
                           MCC_CSW_U_N = MCC_CDW_U_N ;
                           MCC_CSW_U_N = MCC_CDW_D_N ;
                           break ;
        case MCC_FIT_CDP : tasdelay = MCC_TASUD + MCC_TASDU ;
                           spicedelay = MCC_SPICEUD_FITDP + MCC_SPICEDU_FITDP;
                           if (f!=NULL) fprintf(f,"%s spice delay : UD=%ld DU=%ld slope : UD=%ld DU=%ld\n",filename,MCC_SPICEUD_FITDP,MCC_SPICEDU_FITDP,MCC_SPICEFUD_FITDP,MCC_SPICEFDU_FITDP) ;
                           if(tasdelay > spicedelay)
                            error = (double)((tasdelay - spicedelay) /
                                             (double)(tasdelay)) ;
                           else
                            error = (double)((spicedelay - tasdelay) /
                                             (double)(spicedelay)) ;
                           if (componly) break;
                           tref = (double)((double)(spicedelay) /
                                           (double)(tasdelay)) ;
                           MCC_CDSP = MCC_CDSP * tref ;
                           MCC_CDS_U_P = MCC_CDS_U_P * tref ;
                           MCC_CDS_D_P = MCC_CDS_D_P * tref ;
                           MCC_CDPP = MCC_CDPP * tref ;
                           MCC_CDP_U_P = MCC_CDP_U_P * tref ;
                           MCC_CDP_D_P = MCC_CDP_D_P * tref ;
                           MCC_CDWP = MCC_CDWP * tref ;
                           MCC_CDW_U_P = MCC_CDW_U_P * tref ;
                           MCC_CDW_D_P = MCC_CDW_D_P * tref ;
                           MCC_CSSP = MCC_CDSP ;
                           MCC_CSS_U_P = MCC_CDS_U_P ;
                           MCC_CSS_D_P = MCC_CDS_D_P ;
                           MCC_CSPP = MCC_CDPP ;
                           MCC_CSP_U_P = MCC_CDP_U_P ;
                           MCC_CSP_D_P = MCC_CDP_D_P ;
                           MCC_CSWP = MCC_CDWP ;
                           MCC_CSW_U_P = MCC_CDW_U_P ;
                           MCC_CSW_D_P = MCC_CDW_D_P ;
                           break ;
       }
     maxloop-- ;
     error = error * 100.0 ;
     if (f!=NULL) fprintf(f,"tasdelay = %ld spicedelay = %ld ratio = %g\n",
                     tasdelay,spicedelay,error) ;
     if(maxloop == 0 || componly) 
       break ;
    }
   mbkfree(filename) ;
   if (!componly) mcc_drvelp(i,MCC_TRANS_B,lotrsparam_n,lotrsparam_p,MCC_DRV_ALL_MODEL) ;
  }
 if (f!=NULL) fclose(f);

 mcc_gencurv(MCC_FIT,lotrsparam_n,lotrsparam_p) ;
}

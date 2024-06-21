/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TTVREN Version 1                                            */
/*    Fichier : ttvren.c                                                    */
/*                                                                          */
/*    (c) copyright 2002 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Antony PINTO                                              */
/*                                                                          */
/****************************************************************************/

#include "ttvren.h"



char            *NAMEFIG = NULL ;
char            *NAMEEXT = NULL ;
char             NEWNAME[1024];
ttvfig_list     *TTVFIG;
timing_cell     *MODELSTM;


/****************************************************************************/
/*{{{                    ren_printrun()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void  ren_printrun(file,tTstart,tTend,Tstart,Tend)
FILE *file ;
struct rusage  Tstart  ,
               Tend    ;
long           tTstart ,
               tTend   ;
{
  double cpu ;
  double tm ;
  double ss ;
  double sl ;
  double cs ;
  double cl ;
  long trun ;
  long tuse ;
  long user ;
  long userms ;
  long userM ;
  long userS ;
  long userD ;
  long syst ;
  long systms ;
  long systM ;
  long systS ;
  long systD ;
 
  cl = ttv_getlinecachesize() ;
  cs = ttv_getsigcachesize() ;
 
  sl = ttv_getlinememorysize() ;
  ss = ttv_getsigmemorysize() ;
 
  tm = (double)(Tend.ru_maxrss * getpagesize()) / (1024.0 * 1024.0) ;
  trun = 10L * (tTend-tTstart);
  if(trun == 0L) trun=10L;
  tuse = (long)(((10L*Tend.ru_utime.tv_sec)+(Tend.ru_utime.tv_usec/100000L))
          -((10L*Tstart.ru_utime.tv_sec)+(Tstart.ru_utime.tv_usec/100000L))
          +((10L*Tend.ru_stime.tv_sec)+(Tend.ru_stime.tv_usec/100000L))
          -((10L*Tstart.ru_stime.tv_sec)+(Tstart.ru_stime.tv_usec/100000L)));
  cpu = 100.0 * ((double)tuse/trun);
  if(cpu > 100.0) cpu = 100.0 ;
 
  user  = (long)(((10L*Tend.ru_utime.tv_sec)+(Tend.ru_utime.tv_usec/100000L))
        - ((10L*Tstart.ru_utime.tv_sec)+(Tstart.ru_utime.tv_usec/100000L))) ;
  syst  = (long)(((10L*Tend.ru_stime.tv_sec)+(Tend.ru_stime.tv_usec/100000L))
        - ((10L*Tstart.ru_stime.tv_sec)+(Tstart.ru_stime.tv_usec/100000L))) ;
  userms = (long)(((1000000L*Tend.ru_utime.tv_sec)+(Tend.ru_utime.tv_usec))
         - ((1000000L*Tstart.ru_utime.tv_sec)+(Tstart.ru_utime.tv_usec))) ;
  systms = (long)(((1000000L*Tend.ru_stime.tv_sec)+(Tend.ru_stime.tv_usec))
         - ((1000000L*Tstart.ru_stime.tv_sec)+(Tstart.ru_stime.tv_usec))) ;
  userM = (long)((user/10L)/60L) ;
  userS = (long)((user/10L)%60L) ;
  userD = (long)(user%10L) ;
  systM = (long)((syst/10L)/60L) ;
  systS = (long)((syst/10L)%60L) ;
  systD = (long)(syst%10L) ;
  fprintf(file,"\n---------------------------------------------------------\n") ;
  fprintf(file,"signal cache size in megabyte : %.2f MB\n",cs) ;
  fprintf(file,"path cache size in megabyte : %.2f MB\n",cl) ;
  fprintf(file,"signal size in megabyte : %.2f MB\n",ss) ;
  fprintf(file,"path size in megabyte : %.2f MB\n",sl) ;
  fprintf(file,"total run time : %02ldmin %02lds\n",
               (long)((tTend-tTstart)/60L),(long)((tTend-tTstart)%60L)) ;
  fprintf(file,"time user   : %02ld'%02ld.%ld''\n",userM,userS,userD) ;
  fprintf(file,"time system : %02ld'%02ld.%ld''\n",systM,systS,systD) ;
  fprintf(file,"time user in ms   : %.2f\n",(double)userms/(double)1000.0) ;
  fprintf(file,"time system in ms : %.2f\n",(double)systms/(double)1000.0) ;
  fprintf(file,"total memory : %.2f MB\n",tm) ;
  fprintf(file,"cpu ratio : %.2f%% CPU\n",cpu) ;
  fprintf(file,"\n---------------------------------------------------------\n") ;
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void printarg(FILE *file, char *opt, char *def, int mode)
{
  switch (mode)
  {
    case 0 :
         fprintf(file," %s",opt);
         break;
    case 1 :
         fprintf(file,"%20s : %s\n",opt,def);
         break;
    case 2 :
         fprintf(file,"%20s : %s\n"," ",def);
         break;
  }
}

/*}}}************************************************************************/
/*{{{                    ren_printhelp()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void ren_printhelp(int mode)
{
  if (!mode)
    fprintf(stdout,"\nUsage : ttvren");
  else
    fprintf(stdout,"\n");
  
  printarg(stdout,"-ttx"            ,"rename ttx file"                ,mode);
  printarg(stdout,"-dtx"            ,"rename dtx file"                ,mode);
  printarg(stdout,"-ext=<extension>","extension added to the filename",mode);
  printarg(stdout,"-l"              ,"do a top level renaming"        ,mode);
  printarg(stdout,"-h"              ,"print this page"                ,mode);
  printarg(stdout,"filename"        ,"without extension"              ,mode);
  fprintf(stdout,"\n");
}

/*}}}************************************************************************/
/*{{{                    retkeyegal()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
short retkeyegal(chaine)
char *chaine ;
{
  size_t     i ;
 
  for(i = 0 ; (i<strlen(chaine)) && (chaine[i] != '=') ; i ++)
    ;
 
  return((i == strlen(chaine)) ? -1 : (short)i) ;
}

/*}}}************************************************************************/
/*{{{                    retkeypercent()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
short retkeypercent(chaine)
char *chaine ;
{
  size_t     i ;
 
  for(i = 0 ; (i<strlen(chaine)) && (chaine[i] != '%') ; i ++)
    ;
 
  return((i == strlen(chaine)) ? -1 : (short)i) ;
}

/*}}}************************************************************************/
/*{{{                    ren_error()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void ren_error()
{
  ren_printhelp(0);
  EXIT(1) ;
}

/*}}}************************************************************************/
/*{{{                    ren_getopt()                                       */
/* parametres :                                                             */
/*                                                                          */
/* renvoie le masque correspondant aux options                              */
/****************************************************************************/
long ren_getopt(argc,argv)
int argc;
char *argv[];
{
  int           i ;
  short         eq ;
  long          maskopt = 0;
  
  for(i = 1 ; i != argc ; i++)
  {
    if(argv[i][0] != '-')
      NAMEFIG = namealloc(argv[i]) ;
    /* test si l'on a un = */
    else if((eq=retkeyegal(argv[i])) != -1)
    {
      argv[i][eq]='\0';
      if (strcmp(argv[i],"-ext") == 0)
      {
        NAMEEXT = namealloc((char *)(argv[i]+eq+1));
        maskopt |= REN_OPT_EXT ;
      }
      else
        ren_error() ;
    }
    else
    {
      if(strcmp(argv[i],"-dtx") == 0)
        maskopt |= REN_OPT_DTX ;
      else if(strcmp(argv[i],"-silent") == 0)
        maskopt |= REN_OPT_SILENT ;
      else if(strcmp(argv[i],"-ttx") == 0)
        maskopt |= REN_OPT_TTX ;
      else if(strcmp(argv[i],"-l") == 0)
        maskopt |= REN_OPT_HIER ;
      else if(strcmp(argv[i],"-h") == 0)
        maskopt |= REN_OPT_HELP ;
      else if(strcmp(argv[i],"-stm") == 0)
        maskopt |= REN_OPT_STM ;
      else
        ren_error() ;
    }
  }
  if (! (maskopt&REN_OPT_EXT))
    ren_error();
  return maskopt;
}

/*}}}************************************************************************/
/*{{{                    main()                                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int main(argc,argv)
int argc;
char *argv[];
{
  struct rusage  tstart, tend, Tstart, Tend;
  long           ttstart, ttend, tTstart, tTend;
  FILE           *file;
  long           maskopt, filetype;
 
#ifdef AVERTEC
  avtenv() ;
#endif

  mbkenv() ;

  
#ifdef AVERTEC_LICENSE
  if (avt_givetoken("HITAS_LICENSE_SERVER","ttvren") != AVT_VALID_TOKEN)
    EXIT(1) ;
#endif
 

  if (((maskopt = ren_getopt(argc,argv))&REN_OPT_HELP)==REN_OPT_HELP)
  {
    ren_printhelp(1);
    EXIT(0);
  }

  if ((maskopt & REN_OPT_SILENT)==0)
   {
     avt_banner("TtvRen","timing file rename","2002");
   }

  if ((maskopt&REN_OPT_DTXTTX)==0)
    maskopt|= REN_OPT_DTXTTX;
  if (NAMEFIG == NULL)
    ren_error() ;

  filetype  = 0;
  
  if ((maskopt&REN_OPT_DTXTTX) == REN_OPT_DTX)
    filetype    = TTV_FILE_DTX;
  if ((maskopt&REN_OPT_DTXTTX) == REN_OPT_TTX)
    filetype    = TTV_FILE_TTX;
  if ((TTVFIG = ttv_getttvfig(NAMEFIG,filetype)) == NULL)
    ren_error() ;
  
//  if ((maskopt&REN_OPT_STM)==REN_OPT_STM)
//    if (!(MODELSTM = stm_getcell(NAMEFIG)))
//      ren_error();
  
  strcat(NEWNAME,"_");
  strcat(NEWNAME,NAMEEXT);
  
  file  = stdout;
  
  getrusage(RUSAGE_SELF,&Tstart) ;
  time(&tTstart) ;

  getrusage(RUSAGE_SELF,&tstart) ;
  time(&ttstart) ;

  #ifdef AVERTEC_LICENSE
    if (avt_givetoken("HITAS_LICENSE_SERVER","ttvren") != AVT_VALID_TOKEN)
      EXIT(1) ;
  #endif
  ren_ttvfigren(maskopt) ;
//  if ((maskopt&REN_OPT_STM)==REN_OPT_STM) ren_stmren(maskopt);

  getrusage(RUSAGE_SELF,&Tend) ;
  time(&tTend) ;

  getrusage(RUSAGE_SELF,&tend) ;
  time(&ttend) ;

  if ((maskopt & REN_OPT_SILENT)==0)
  {
    ren_printrun(file,ttstart,ttend,tstart,tend) ;
    ren_printrun(file,tTstart,tTend,Tstart,Tend) ;
  }
  fprintf(file,"\nend\n") ;

  ttv_freeallttvfig(TTVFIG) ;
  if ((maskopt&REN_OPT_STM)==REN_OPT_STM)
    stm_freecell(NAMEFIG);

  EXIT(0) ;
  return 0;
}

/*}}}************************************************************************/

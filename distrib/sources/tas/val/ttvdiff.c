/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TTVDIFF Version 1                                           */
/*    Fichier : ttvdiff.c                                                   */
/*                                                                          */
/*    (c) copyright 2002 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                Antony PINTO                                              */
/*                                                                          */
/****************************************************************************/

#include "ttvdiff.h"

long            DELTA;
long            DELTAS;
long            DELTAD;
long            DELTAC;
float           MODF[DIFF_MAX_MODNB]; 
float           MODC[DIFF_MAX_MODNB]; 
float           MODK[DIFF_MAX_MODNB];
int             NBMODF = 0;
int             NBMODC = 0;
int             NBMODK = 0;
char            *NAMEFIG1 = NULL ;
char            *NAMEFIG2 = NULL ;
char            *FILEOUT = NULL ;

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void  diff_printrun(file,tTstart,tTend,Tstart,Tend)
FILE            *file ;
struct rusage    Tstart  , Tend    ;
long             tTstart , tTend   ;
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
  static int     cpt = 0;
  int            lenght;
    
  if (mode)
    if (opt)
      fprintf(file,"%-20s : %s\n",opt,def);
    else
      fprintf(file,"%20s   %s\n","",def);
  else
    if (opt)
    {
      lenght    = strlen(opt);
      if (cpt + lenght > 60)
      {
        cpt     = 0;
        fprintf(file,"\n%7s","");
      }
      cpt      += lenght; 
      fprintf(file," %s",opt);
    }
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void diff_printhelp(int mode)
{
  if (mode)
    fprintf(stdout,"\n");
  else
    fprintf(stdout,"\nUsage :");
  
  printarg(stdout,"-nd"                 ,"don't print delays diff"                     ,mode);
  printarg(stdout,"-ns"                 ,"don't print slope diff"                      ,mode);
  printarg(stdout,"-nc"                 ,"don't print capacitance diff"                ,mode);
  printarg(stdout,"-s"                  ,"compare netname"                             ,mode);
  printarg(stdout,"-d"                  ,"print dtx diff"                              ,mode);
  printarg(stdout,"-nt"                 ,"don't print ttx diff"                        ,mode);
  printarg(stdout,"-m"                  ,"print model diff"                            ,mode);
  printarg(stdout,"-v"                  ,"print report on screen mode"                 ,mode);
  printarg(stdout,"-i"                  ,"print info diff"                             ,mode);
  printarg(stdout,"-l"                  ,"do a hierarchical diff"                      ,mode);
  printarg(stdout,"-x=<min>:<max>"      ,"interval of path delay search"               ,mode);
  printarg(stdout,"-xin=<\"name\">"     ,"mask for signal name begin"                  ,mode);
  printarg(stdout,"-xout=<\"name\">"    ,"mask for signal name end"                    ,mode);
  printarg(stdout,"-delta=<delta>"      ,"print only if delay,slope or capacitance is" ,mode);
  printarg(stdout,0                     ,"upper than delta."                           ,mode);
  printarg(stdout,0                     ,"delta can be in ps or in %%"                 ,mode);
  printarg(stdout,"-deltad=<delta>"     ,"same as -delta but only for delay"           ,mode);
  printarg(stdout,"-deltas=<delta>"     ,"same as -delta but only for slope"           ,mode);
  printarg(stdout,"-deltac=<delta>"     ,"same as -delta but only for capacitance"     ,mode);
  printarg(stdout,"-h"                  ,"print this page"                             ,mode);
  printarg(stdout,"-im"                 ,"ignore missing"                              ,mode);
  printarg(stdout,"-ie"                 ,"ignore extra"                                ,mode);
  printarg(stdout,"-ic"                 ,"ignore change"                               ,mode);
  printarg(stdout,"-f=<s1>,..,<s32>"    ,"diff modele slope in this range"             ,mode);
  printarg(stdout,"-c=<c1>,..,<c32>"    ,"diff modele capacitance in this range"       ,mode);
  printarg(stdout,"-k=<k1>,..,<k32>"    ,"diff modele clock slope int his range"       ,mode);
  printarg(stdout,"-out=<\"FILEOUT\">"  ,"output file, default is <filename1>.ttvdiff" ,mode);
  printarg(stdout,"filename1"           ,"file to be compared .ttx or .dtx without"    ,mode);
  printarg(stdout,0                     ,"extention"                                   ,mode);
  printarg(stdout,"filename2"           ,"file compared .ttx or .dtx without extention",mode);
  printarg(stdout,0                     ,"extention"                                   ,mode);
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
short retkeyegal(chaine)
char *chaine ;
{
  size_t     i ;

  for(i = 0 ; (i<strlen(chaine)) && (chaine[i] != '=') ; i ++)
    ;

  return ((i == strlen(chaine)) ? -1 : (int)i) ;
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
short retkeycomma(chaine)
char *chaine ;
{
  size_t     i ;

  for(i = 0 ; (i<strlen(chaine)) && (chaine[i] != ',') ; i ++)
    ;

  return ((i == strlen(chaine)) ? -1 : (int)i) ;
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
short retkeypercent(chaine)
char *chaine ;
{
  size_t     i ;

  for(i = 0 ; (i<strlen(chaine)) && (chaine[i] != '%') ; i ++) ;

  return((i == strlen(chaine)) ? -1 : (int)i) ;
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void diff_error()
{
  diff_printhelp(0);
  fprintf(stdout,"\n");
  EXIT(1) ;
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void diff_initmodarray()
{
  int i;

  for (i=0;i<DIFF_MAX_MODNB;i++)
  {
    MODF[i]=-1;
    MODC[i]=-1;
    MODK[i]=-1;
  }
}

/****************************************************************************/
/*                        function diff_getopt()                            */
/* parametres :                                                             */
/*                                                                          */
/* renvoie le masque correspondant aux options                              */
/****************************************************************************/
long diff_getopt(argc,argv)
int argc;
char *argv[];
{
  int           i ;
  short         eq ;
  short         pc;
  short         co;
  long          maskopt = 0;

  for(i = 1 ; i != argc ; i++)
  {
    if(argv[i][0] != '-')
    {
      if(NAMEFIG1 == NULL)
        NAMEFIG1 = namealloc(argv[i]) ;
      else 
        NAMEFIG2 = namealloc(argv[i]) ;
    }
    /* test si l'on a un = */
    else if((eq=retkeyegal(argv[i])) != -1)
    {
      /* test si l'on a un % */
      if ((pc=retkeypercent(argv[i])) != -1)
      {
        argv[i][eq]='\0';
        if(strcmp(argv[i],"-deltad") == 0)
        {
          argv[i][pc]='\0';
          if((DELTAD = atof(argv[i]+eq+1)) < 0)
            diff_error() ;
          maskopt |= DIFF_OPT_DELTADP ;
        }
        else if(strcmp(argv[i],"-deltas") == 0)
        {
          argv[i][pc]='\0';
          if((DELTAS = atof(argv[i]+eq+1)) < 0)
            diff_error() ;
          maskopt |= DIFF_OPT_DELTASP ;
        }
        else if(strcmp(argv[i],"-deltac") == 0)
        {
          argv[i][pc]='\0';
          if((DELTAC = atof(argv[i]+eq+1)) < 0)
            diff_error() ;
          maskopt |= DIFF_OPT_DELTACP ;
        }
        else if(strcmp(argv[i],"-delta") == 0)
        {
          argv[i][pc]='\0';
          if((DELTA = atof(argv[i]+eq+1)) < 0)
            diff_error() ;
          maskopt |= DIFF_OPT_DELTAP  ;
        }
        else diff_error() ;
      } /* test si on a une virgule */ 
      else if ((co=retkeycomma(argv[i])) != -1)
      {
        int     j;
        char    tmpargv[2048];

        argv[i][eq]='\0';
        co -= eq+1;
        if(strcmp(argv[i],"-f") == 0)
        {
          strcpy(tmpargv,argv[i]+eq+1);
          for (j=0;j<=DIFF_MAX_MODNB;j++,NBMODF++)
          {
            tmpargv[co]='\0';
            if (tmpargv[co+1]=='\0')
            {
              fprintf(stderr,"error null value inserted in -f\n");
              diff_error() ;
            }
            if ((MODF[j] = atof(tmpargv)) < 0)
              diff_error() ;
            strcpy(tmpargv,tmpargv+co+1);

            if ((co=retkeycomma(tmpargv)) == -1) 
            {
              if((MODF[j+1] = atof(tmpargv)) < 0)
                diff_error() ;
              if (tmpargv[co+1]=='\0')
              {
                fprintf(stderr,"error null value inserted in -f\n");
                diff_error() ;
              }
              break;
            }
            if (j==DIFF_MAX_MODNB)
            {
              fprintf(stderr,"error too much arguments in -f");
              diff_error();
            }
          }
          maskopt |= DIFF_OPT_MODF ;
        }
        else if(strcmp(argv[i],"-c") == 0)
        {
          strcpy(tmpargv,argv[i]+eq+1);
          for (j=0;j<=DIFF_MAX_MODNB;j++,NBMODC++)
          {
            tmpargv[co]='\0';
            if (tmpargv[co+1]=='\0')
            {
              fprintf(stderr,"error null value inserted in -c\n");
              diff_error() ;
            }
            if ((MODC[j] = atof(tmpargv)) < 0)
              diff_error() ;

            //printf("%s\n",tmpargv);

            strcpy(tmpargv,tmpargv+co+1);
            if ((co=retkeycomma(tmpargv)) == -1) 
            {
              //printf("%s\n",tmpargv);
              if((MODC[j+1] = atof(tmpargv)) < 0)
                diff_error() ;
              if (tmpargv[co+1]=='\0')
              {
                fprintf(stderr,"error null value inserted in -c\n");
                diff_error() ;
              }
              break;
            }
            if (j==DIFF_MAX_MODNB)
            {
              fprintf(stderr,"error too much arguments in -c");
              diff_error();
            }
          }
          maskopt |= DIFF_OPT_MODC ;
        }
        else if(strcmp(argv[i],"-k") == 0)
        {
          strcpy(tmpargv,argv[i]+eq+1);
          for (j=0;j<=DIFF_MAX_MODNB;j++,NBMODK++)
          {
            tmpargv[co]='\0';
            if (tmpargv[co+1]=='\0')
            {
              fprintf(stderr,"error null value inserted in -k\n");
              diff_error() ;
            }
            if ((MODK[j] = atof(tmpargv)) < 0)
              diff_error() ;
            strcpy(tmpargv,tmpargv+co+1);

            if ((co=retkeycomma(tmpargv)) == -1) 
            {
              if((MODK[j+1] = atof(tmpargv)) < 0)
                diff_error() ;
              if (tmpargv[co+1]=='\0')
              {
                fprintf(stderr,"error null value inserted in -k\n");
                diff_error() ;
              }
              break;
            }
            if (j==DIFF_MAX_MODNB)
            {
              fprintf(stderr,"error too much arguments in -k");
              diff_error();
            }
          }
          maskopt |= DIFF_OPT_MODK ;
        }
      }
      else
      {
        argv[i][eq]='\0';
        if(strcmp(argv[i],"-deltad") == 0)
        {
          if((DELTAD = atof(argv[i]+eq+1)) < 0)
            diff_error() ;
          maskopt |= DIFF_OPT_DELTAD ;
        }
        else if(strcmp(argv[i],"-deltas") == 0)
        {
          if((DELTAS = atof(argv[i]+eq+1)) < 0)
            diff_error() ;
          maskopt |= DIFF_OPT_DELTAS ;
        }
        else if(strcmp(argv[i],"-deltac") == 0)
        {
          if((DELTAC = atof(argv[i]+eq+1)) < 0)
            diff_error() ;
          maskopt |= DIFF_OPT_DELTAC ;
        }
        else if(strcmp(argv[i],"-delta") == 0)
        {
          if((DELTA = atof(argv[i]+eq+1)) < 0)
            diff_error() ;
          maskopt |= DIFF_OPT_DELTA  ;
        }
        else if(strcmp(argv[i],"-c") == 0)
        {
          if((MODC[0] = atof(argv[i]+eq+1)) < 0)
            diff_error() ;
          maskopt |= DIFF_OPT_MODC  ;
          NBMODC = 1;
        }
        else if(strcmp(argv[i],"-k") == 0)
        {
          if((MODK[0] = atof(argv[i]+eq+1)) < 0)
            diff_error() ;
          maskopt |= DIFF_OPT_MODK  ;
          NBMODK = 1;
        }
        else if(strcmp(argv[i],"-f") == 0)
        {
          if((MODF[0] = atof(argv[i]+eq+1)) < 0)
            diff_error() ;
          maskopt |= DIFF_OPT_MODF  ;
          NBMODF = 1;
        }
        else if(strcmp(argv[i],"-xin") == 0)
        {
          TTV_NAME_IN = addchain(TTV_NAME_IN,
                                 (void *)namealloc((char *)(argv[i]+eq+1)));
        }
        else if(strcmp(argv[i],"-out") == 0)
        {
          FILEOUT = namealloc((char *)(argv[i]+eq+1));
        }
        else if(strcmp(argv[i],"-xout") == 0)
        {
          TTV_NAME_OUT=addchain(TTV_NAME_OUT,
                                (void *)namealloc((char *)(argv[i]+eq+1)));
        }
        else if(strcmp(argv[i],"-x") == 0)
        {
          char           pathopt[48] ;
          unsigned int   j ;

          maskopt |= DIFF_OPT_X  ;
          strcpy(pathopt,argv[i] + 3) ;
          for(j = 0 ; (j != strlen(pathopt)) && (pathopt[j] != ':') ; j++) ;
          if(j == strlen(pathopt))
          {
            TTV_SEUIL_MIN = atol(pathopt) ;
            TTV_SEUIL_MAX = TTV_DELAY_MAX ;
            if((TTV_SEUIL_MIN == 0) && (strcmp(pathopt,"0") != 0) &&
               (strcmp(pathopt,"00") != 0))
              diff_error() ;
          }
          else
          {
            char *aux = pathopt ;

            aux[j] = '\0' ;

            if(j != 0)
            {
              TTV_SEUIL_MIN = atol(aux) ;

              if((TTV_SEUIL_MIN == 0) && (strcmp(aux,"0") != 0) &&
                 (strcmp(aux,"00") != 0))
                diff_error() ;
            }
            else TTV_SEUIL_MIN = TTV_DELAY_MIN ;

            aux += j+1 ;

            if(aux[0] != '\0')
            {
              TTV_SEUIL_MAX = atol(aux) ;

              if((TTV_SEUIL_MAX == 0) && (strcmp(aux,"0") != 0) &&
                 (strcmp(aux,"00") != 0))
                diff_error() ;
            }
            else TTV_SEUIL_MAX = TTV_DELAY_MAX ;
          }
        }
        else diff_error() ;
      }
    }
    else
    {
      if(strcmp(argv[i],"-v") == 0)
      {
        maskopt |= DIFF_OPT_SCREEN ;
      }
      else if(strcmp(argv[i],"-nd") == 0)
      {
        maskopt |= DIFF_OPT_NDELAY ;
      }
      else if(strcmp(argv[i],"-nsig") == 0)
      {
        maskopt |= DIFF_OPT_NSIG ;
      }
      else if(strcmp(argv[i],"-test") == 0)
      {
        maskopt |= DIFF_OPT_TEST ;
      }
      else if(strcmp(argv[i],"-ns") == 0)
      {
        maskopt |= DIFF_OPT_NSLOPE ;
      }
      else if(strcmp(argv[i],"-nc") == 0)
      {
        maskopt |= DIFF_OPT_NCAPA  ;
      }
      else if(strcmp(argv[i],"-s") == 0)
      {
        maskopt |= DIFF_OPT_NETNAME;
      }
      else if(strcmp(argv[i],"-d") == 0)
      {
        maskopt |= DIFF_OPT_DTX ;
      }
      else if(strcmp(argv[i],"-nt") == 0)
      {
        maskopt |= DIFF_OPT_NTTX ;
      }
      else if(strcmp(argv[i],"-m") == 0)
      {
        maskopt |= DIFF_OPT_MODEL  ;
      }
      else if(strcmp(argv[i],"-i") == 0)
      {
        maskopt |= DIFF_OPT_INFO   ;
      }
      else if(strcmp(argv[i],"-l") == 0)
      {
        maskopt |= DIFF_OPT_HIER   ;
      }
      else if(strcmp(argv[i],"-h") == 0)
      {
        maskopt |= DIFF_OPT_HELP   ;
      }
      else if(strcmp(argv[i],"-im") == 0)
      {
        maskopt |= DIFF_OPT_IMISS  ;
      }
      else if(strcmp(argv[i],"-ie") == 0)
      {
        maskopt |= DIFF_OPT_IEXTRA ;
      }
      else if(strcmp(argv[i],"-ic") == 0)
      {
        maskopt |= DIFF_OPT_ICHANGE;
      }
      else diff_error() ;
    }
  }
  return maskopt;
}

/*}}}************************************************************************/
/*{{{                                                                       */
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
  ttvfig_list    *ttvfig1, *ttvfig2;
  long           maskopt;
  int            nbdiff;
  char           buff[4096];

#ifdef AVERTEC
  avtenv() ;
#endif
  mbkenv() ;

  diff_initmodarray();

  if (((maskopt = diff_getopt(argc,argv))&DIFF_OPT_HELP)==DIFF_OPT_HELP)
  {
    diff_printhelp(1);
    EXIT(0);
  }
  if (!(maskopt&DIFF_OPT_X))
  {
    TTV_SEUIL_MAX = TTV_DELAY_MAX ;
    TTV_SEUIL_MIN = TTV_DELAY_MIN ;
  }

#ifdef AVERTEC_LICENSE
  if(avt_givetoken("HITAS_LICENSE_SERVER","ttvdiff") != AVT_VALID_TOKEN)
    EXIT(1) ;
#endif

  if (!(maskopt&DIFF_OPT_TEST))
  {
    avt_banner("TtvDiff","timing file compare","2002");
  }

  if ((NAMEFIG1 == NULL) || (NAMEFIG2 == NULL)) 
    diff_error() ;

  if ((maskopt&DIFF_OPT_NTTX))
  {
    if ((ttvfig1 = ttv_getttvfig(NAMEFIG1,TTV_FILE_DTX)) == NULL)
      diff_error() ;
    if ((ttvfig2 = ttv_getttvfig(NAMEFIG2,TTV_FILE_DTX)) == NULL)
      diff_error() ;
  }
  else
  {
    if ((ttvfig1 = ttv_getttvfig(NAMEFIG1,TTV_FILE_TTX)) == NULL)
      diff_error() ;
    if ((ttvfig2 = ttv_getttvfig(NAMEFIG2,TTV_FILE_TTX)) == NULL)
      diff_error() ;
  }

  if(!(maskopt&(DIFF_OPT_SCREEN)))
  {
    if(FILEOUT == NULL)
      FILEOUT = ttvfig1->INFO->FIGNAME ;

    file = mbkfopen(FILEOUT,"ttvdiff",WRITE_TEXT) ;

    if (file == NULL)
    {
      (void)fprintf(stderr, "*** ttvdiff error ***\n");
      (void)fprintf(stderr, "can not open the file %s.ttvdiff",FILEOUT);
      EXIT(1) ;
    }
    sprintf(buff,"ttvdiff validation report\n"
                 "figure name : %s %s\n",
            ttvfig1->INFO->FIGNAME,ttvfig2->INFO->FIGNAME) ;
    avt_printExecInfo(file,"***",buff,"***");
  }
  else 
  {
    file  = stdout;
    fprintf(file,"figure name : %s %s\n",ttvfig1->INFO->FIGNAME,ttvfig2->INFO->FIGNAME) ;
  }
  fprintf(file,"\n");
  
  fflush(file) ;

  if ((maskopt&(DIFF_OPT_TEST))==0)
  {
    getrusage(RUSAGE_SELF,&Tstart) ;
    time(&tTstart) ;

    getrusage(RUSAGE_SELF,&tstart) ;
    time(&ttstart) ;
  }

#ifdef AVERTEC_LICENSE
  if(avt_givetoken("HITAS_LICENSE_SERVER","ttvdiff") != AVT_VALID_TOKEN)
    EXIT(1) ;
#endif
  nbdiff = diff_runttvdiff(file,ttvfig1,ttvfig2,maskopt);

  if ((maskopt&(DIFF_OPT_TEST))==0)
  {
    getrusage(RUSAGE_SELF,&Tend) ;
    time(&tTend) ;
    getrusage(RUSAGE_SELF,&tend) ;
    time(&ttend) ;
    
    diff_printrun(file,ttstart,ttend,tstart,tend) ;
    diff_printrun(file,tTstart,tTend,Tstart,Tend) ;
  }

  if ((maskopt&(DIFF_OPT_TEST))==0)
    fprintf(file,"\nend\n") ;

  nbdiff = (nbdiff > 0xFF) ? 0xFF : nbdiff;

  if(fclose(file) != 0)
  {
    (void)fprintf(stderr, "*** ttvdiff error ***\n");
    (void)fprintf(stderr, "can not close the file %s.ttvdiff",FILEOUT);
    ttv_freeallttvfig(ttvfig1) ;
    if (ttvfig1 != ttvfig2)
      ttv_freeallttvfig(ttvfig2) ;
    EXIT(nbdiff);
  }

  
  ttv_freeallttvfig(ttvfig1) ;
  if (ttvfig1 != ttvfig2)
    ttv_freeallttvfig(ttvfig2) ;

  EXIT(nbdiff);
  return 0;
}

/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : VAL Version 1                                               */
/*    Fichier : val_main.c                                                  */
/*                                                                          */
/*    (c) copyright 1997-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

#include "val_main.h"

void  val_printrun(file,tTstart,tTend,Tstart,Tend)
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



void  val_printpath(ttvfig,file,path)
ttvfig_list *ttvfig ;
FILE *file ;
ttvpath_list *path ;
{
 char name[1024] ;

 if(path != NULL)
   {
    fprintf(file,"\n%s ",
    ttv_getsigname(ttvfig,name,path->NODE->ROOT)) ;
    fprintf(file,"%s %c%c %s TP = %ld ps RC = %ld ps\n\n",
    ttv_getsigname(ttvfig,name,path->ROOT->ROOT),
    (((path->NODE->TYPE & TTV_NODE_UP) == TTV_NODE_UP) ? 'U' : 'D'),
    (((path->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP) ? 'U' : 'D'),
    (((path->TYPE & TTV_FIND_MAX) == TTV_FIND_MAX)? "MAX" : "MIN"),
    path->DELAY,path->SLOPE) ;
   }
}

void  val_printcritic(file,critic)
FILE *file ;
ttvcritic_list *critic ;
{
 ttvcritic_list *auxcritic ;
 long delay = (long)0 ;


 if(critic != NULL)
   {
    for(auxcritic = critic ; auxcritic->NEXT != NULL ; 
    auxcritic = auxcritic->NEXT)
      delay += auxcritic->DELAY ;
    delay += auxcritic->DELAY ;
    fprintf(file,"\n%s %s %c%c %s TP = %ld ps RC = %ld ps\n",
            critic->NAME,auxcritic->NAME,critic->SNODE,auxcritic->SNODE,
            (((critic->TYPE & TTV_FIND_MAX) == TTV_FIND_MAX)? "MAX" : "MIN"),
            delay,auxcritic->SLOPE) ;
    for(; critic->NEXT != NULL ; critic = critic->NEXT)
      {
       auxcritic = critic->NEXT ;
       fprintf(file,"\n") ;
       if(critic->SNODE == TTV_UP)
         fprintf(file,"UP(") ;
       else fprintf(file,"DOWN(") ;
       fprintf(file,"%s",critic->NAME) ;
       fprintf(file,") => ") ;
       if(((critic->TYPE & TTV_FIND_HZ) == TTV_FIND_HZ) &&
          (auxcritic->NEXT == NULL))
         fprintf(file,"HighZ(") ;
       else if(auxcritic->SNODE == TTV_UP)
         fprintf(file,"UP(") ;
       else fprintf(file,"DOWN(") ;
       fprintf(file,"%s",auxcritic->NAME) ;
       fprintf(file,"), TP=%ld ps RC = %ld ps",auxcritic->DELAY,
                                               auxcritic->SLOPE) ;
      }
   }
 fprintf(file,"\n\n") ;
}

short retkey(chaine)
char *chaine ;
{
  size_t    i ;

  for (i = 0 ; (i<strlen(chaine)) && (chaine[i] != '=') ; i ++)
    ;

  return ((i == strlen(chaine)) ? -1 : (short)i) ;
}

void val_error()
{
 fprintf(stderr,"\nUsage : val -t -d -h -l -m -c -p -r -x=<min>:<max> -lm=<size> -lms=<size> -lml=<size>\n") ;
 fprintf(stderr,"        -xin=<\"name\"> -xout=<\"name\"> -out=<\"fileout\"> -dtx -ttx filename \n") ;

 EXIT(1) ;
}

int main(argc,argv)
int argc;
char *argv[];
{
 struct rusage  tstart  ,
                tend    ;
 long           ttstart ,
                ttend   ;
 struct rusage  Tstart  ,
                Tend    ;
 long           tTstart ,
                tTend   ;
 FILE *file ;
 ttvfig_list *ttvfig ;
 ttvfig_list *ttvins = NULL ;
 ttvfig_list *ttvfigx ;
 ttvsig_list *ptsig ;
 chain_list *chainsig ;
 chain_list *chainsigx ;
 ttvpath_list *path ;
 ttvpath_list *pathx ;
 ttvpath_list *pathp ;
 ttvpath_list *pathpx ;
 ttvcritic_list *critic ;
 ttvcritic_list *criticx ;
 chain_list *chaincp ;
 chain_list *chaincpx ;
 char *namefig = NULL ;
 char *fileout = NULL ;
 long type = (long)0 ;
 long filetype ;
 long delay ;
 long delaymax = TTV_NOTIME ;
 long delaymin = TTV_NOTIME ;
 long nbpath = (long)0 ;
 long nbpathmax = (long)0 ;
 long nbpathmin = (long)0 ;
 long nbpara = (long)0 ;
 long nbparaeq = (long)0 ;
 long nbparaeqx = (long)0 ;
 long nbcritic = (long)0 ;
 long nbcriticpara = (long)0 ;
 double limit = (double)0.0 ;
 double limitl = (double)0.0 ;
 double limits = (double)0.0 ;
 long dualmode = (double)0.0 ;
 long nbc = (long)0 ;
 long nbn = (long)0 ;
 long nbq = (long)0 ;
 long nbl = (long)0 ;
 long nbr = (long)0 ;
 long nbi = (long)0 ;
 long nbs = (long)0 ;
 long nbt = (long)0 ;
 long nbj = (long)0 ;
 long nbp = (long)0 ;
 long nbd = (long)0 ;
 long nbe = (long)0 ;
 long nbf = (long)0 ;
 long nbfig = (long)0 ;
 long j ;
 int eq ;
 int i ;
 int w1 = 0 ;
 int w2 = 0 ;
 int w3 = 0 ;
 int w4 = 0 ;
 int w5 = 0 ;
 int e1 = 0 ;
 int e2 = 0 ;
 int e3 = 0 ;
 int e4 = 0 ;
 int e5 = 0 ;
 int e6 = 0 ;
 int e7 = 0 ;
 int e8 = 0 ;
 int e9 = 0 ;
 int e10 = 0 ;
 char flagerr = 'N' ;
 char newflagerr = 'N' ;
 char tracemode = 'N' ;
 char check_path = 'N' ;
 char check_long = 'N' ;
 char check_high = 'N' ;
 char check_critic = 'N' ;
 char check_para = 'N' ;
 char check_detail = 'N' ;
 char name[1024] ;

#ifdef AVERTEC
 avtenv() ;
#endif

 mbkenv() ;

 for(i = 1 ; i != argc ; i++)
  {
   if(argv[i][0] != '-')
      {
       namefig = namealloc(argv[i]) ;
      }
   else if((eq=retkey(argv[i])) != -1)
      {
       argv[i][eq]='\0';
       if(strcmp(argv[i],"-xin") == 0)
         {
          TTV_NAME_IN=addchain(TTV_NAME_IN,
                              (void *)namealloc((char *)(argv[i]+eq+1)));
         }
       else if(strcmp(argv[i],"-out") == 0)
         {
          fileout = namealloc((char *)(argv[i]+eq+1));
         }
       else if(strcmp(argv[i],"-xout") == 0)
         {
          TTV_NAME_OUT=addchain(TTV_NAME_OUT,
                              (void *)namealloc((char *)(argv[i]+eq+1)));
         }
       else if(strcmp(argv[i],"-lm") == 0)
         {
          if((limit = atof(argv[i]+eq+1)) <= (double)0.0)
            val_error() ;
         }
       else if(strcmp(argv[i],"-lms") == 0)
         {
          if((limits = atof(argv[i]+eq+1)) <= (double)0.0)
            val_error() ;
         }
       else if(strcmp(argv[i],"-lml") == 0)
         {
          if((limitl = atof(argv[i]+eq+1)) <= (double)0.0)
            val_error() ;
         }
       else if(strcmp(argv[i],"-x") == 0)
         {
          char pathopt[48] ;
          unsigned int j ;

          strcpy(pathopt,argv[i] + 3) ;

          for(j = 0 ; (j != strlen(pathopt)) && (pathopt[j] != ':') ; j++) ;
          if(j == strlen(pathopt))
           {
            TTV_SEUIL_MIN = atol(pathopt) ;
            TTV_SEUIL_MAX = TTV_DELAY_MAX ;

            if((TTV_SEUIL_MIN == 0) && (strcmp(pathopt,"0") != 0) &&
               (strcmp(pathopt,"00") != 0))
              val_error() ;
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
                  val_error() ;
               }
            else TTV_SEUIL_MIN = TTV_DELAY_MIN ;

            aux += j+1 ;

            if(aux[0] != '\0')
              {
               TTV_SEUIL_MAX = atol(aux) ;

               if((TTV_SEUIL_MAX == 0) && (strcmp(aux,"0") != 0) &&
                  (strcmp(aux,"00") != 0))
                 val_error() ;
              }
            else TTV_SEUIL_MAX = TTV_DELAY_MAX ;
           }
         }
       else
         {
          argv[i][eq]='\0';
          val_error() ;
         }
       argv[i][eq]='\0';
      }
   else
      {
       if(strcmp(argv[i],"-t") == 0) tracemode = 'Y' ;
       else if(strcmp(argv[i],"-d") == 0) dualmode = TTV_FIND_DUAL ;
       else if(strcmp(argv[i],"-m") == 0) check_path = 'Y' ;
       else if(strcmp(argv[i],"-l") == 0) check_long = 'Y' ;
       else if(strcmp(argv[i],"-h") == 0) check_high = 'Y' ;
       else if(strcmp(argv[i],"-c") == 0) check_critic = 'Y' ;
       else if(strcmp(argv[i],"-p") == 0) check_para = 'Y' ;
       else if(strcmp(argv[i],"-r") == 0) check_detail = 'Y' ;
       else if(strcmp(argv[i],"-dtx") == 0)
        {
         type = TTV_FIND_LINE ;
        }
       else if(strcmp(argv[i],"-ttx") == 0)
        {
         type = TTV_FIND_PATH ;
        }
       else val_error() ;
      }
  }

#ifdef AVERTEC_LICENSE
if(avt_givetoken("HITAS_LICENSE_SERVER","val") != AVT_VALID_TOKEN)
 EXIT(1) ;
#endif
 
 avt_banner("vAl","timing file check","1998");

 if(namefig == NULL) 
     val_error() ;

 if((limitl <= (double)0.0) && (limits <= (double)0.0))
  {
   if(limit <= (double)0.0)
     limit = (double)100.0 ;
   ttv_setcachesize((double)(limit * (double)0.1),
                    (double)(limit * (double)0.9)) ;
  }
 else
  {
   if(limitl <= (double)0.0)
     limitl = (double)90.0 ;
   if(limits <= (double)0.0)
     limits = (double)10.0 ;
   ttv_setcachesize(limits,limitl) ;
  }

 if((check_path == 'N') && (check_critic == 'N') 
    && (check_para == 'N') && (check_detail == 'N'))
   {
    check_critic = 'Y' ;
    check_para = 'Y' ;
    check_detail = 'Y' ;
   }
  else if(check_detail == 'Y')
    check_para = 'Y' ;

 if((ttvfig = ttv_getttvfig(namefig,(long)0)) == NULL)
   val_error() ;

 filetype = ttv_existefile(ttvfig,TTV_FILE_DTX|TTV_FILE_TTX) ;

 if(TTV_SEUIL_MAX == TTV_DELAY_MIN)
   TTV_SEUIL_MAX = TTV_DELAY_MAX ;
 if(TTV_SEUIL_MIN == TTV_DELAY_MAX)
   TTV_SEUIL_MIN = TTV_DELAY_MIN ;

 if(fileout == NULL)
   file = mbkfopen(ttvfig->INFO->FIGNAME,"val",WRITE_TEXT) ;
 else
   file = mbkfopen(fileout,"val",WRITE_TEXT) ;

 if(file == NULL)
  {
   (void)fprintf(stderr, "*** val error ***\n");
   (void)fprintf(stderr, "can not open the file %s.val",ttvfig->INFO->FIGNAME);
   EXIT(1) ;
  }

 fprintf(file,"ttv view validation report : \n") ;
 fprintf(file,"figure name : %s \n\n",ttvfig->INFO->FIGNAME) ;

 fflush(file) ;

 if(check_high == 'Y')
   ttvins = ttvfig ;

 getrusage(RUSAGE_SELF,&Tstart) ;
 time(&tTstart) ;

 getrusage(RUSAGE_SELF,&tstart) ;
 time(&ttstart) ;

 path = ttv_getcriticpath(ttvfig,ttvins,TTV_NAME_IN,TTV_NAME_OUT,TTV_SEUIL_MAX,
                          TTV_SEUIL_MIN,(long)1,type|dualmode|TTV_FIND_MAX) ;

 if(path != NULL)
  {
   getrusage(RUSAGE_SELF,&tend) ;
   time(&ttend) ;
   fprintf(file,"MAX PATH : \n") ;
   val_printpath(ttvfig,file,path) ;
   val_printrun(file,ttstart,ttend,tstart,tend) ;
   ttv_freepathlist(path) ;
  }


 getrusage(RUSAGE_SELF,&tstart) ;
 time(&ttstart) ;

 path = ttv_getcriticpath(ttvfig,ttvins,TTV_NAME_IN,TTV_NAME_OUT,TTV_SEUIL_MAX,
                          TTV_SEUIL_MIN,(long)1,type|dualmode|TTV_FIND_MIN) ;

 if(path != NULL)
  {
   getrusage(RUSAGE_SELF,&tend) ;
   time(&ttend) ;
   fprintf(file,"MAX PATH : \n") ;
   val_printpath(ttvfig,file,path) ;
   val_printrun(file,ttstart,ttend,tstart,tend) ;
   ttv_freepathlist(path) ;
  }
 
 if(check_long == 'Y')
  {
   ttv_freeallttvfig(ttvfig) ;
   EXIT(0) ;
  }

 if((dualmode & TTV_FIND_DUAL) != TTV_FIND_DUAL)
  chainsig = ttv_getsigbytype(ttvfig,NULL,TTV_SIG_C|TTV_SIG_Q|TTV_SIG_L|
                              TTV_SIG_R|TTV_SIG_B,TTV_NAME_OUT) ;
 else
  chainsig = ttv_getsigbytype(ttvfig,NULL,TTV_SIG_C|TTV_SIG_L|TTV_SIG_R|
                                 TTV_SIG_B,
                                 TTV_NAME_IN) ;

 for(chainsigx = chainsig ; chainsigx != NULL ; chainsigx = chainsigx->NEXT)
    {
     ptsig = (ttvsig_list*)chainsigx->DATA ;

     if(tracemode == 'Y')
      {
       fprintf(stdout,"validation signal : %s\n\n",
               ttv_getsigname(ttvfig,name,ptsig)) ;
       fflush(stdout) ;
      }

     if((ptsig->TYPE & TTV_SIG_C) == TTV_SIG_C)
       {
        if((((ptsig->TYPE & TTV_SIG_CO) != TTV_SIG_CO) && 
           ((dualmode & TTV_FIND_DUAL) != TTV_FIND_DUAL)) ||
           (((ptsig->TYPE & TTV_SIG_CI) != TTV_SIG_CI) &&
           ((dualmode & TTV_FIND_DUAL) == TTV_FIND_DUAL)))
          continue ;
       }
     if((dualmode & TTV_FIND_DUAL) != TTV_FIND_DUAL)
      {
       path = ttv_getpath(ttvfig,NULL,ptsig,TTV_NAME_IN,TTV_SEUIL_MAX,
                          TTV_SEUIL_MIN,type|TTV_FIND_MAX|dualmode) ;
       pathx = ttv_getpath(ttvfig,NULL,ptsig,TTV_NAME_IN,TTV_SEUIL_MAX,
                           TTV_SEUIL_MIN,type|TTV_FIND_MIN|dualmode) ;
      }
     else
      {
       path = ttv_getpath(ttvfig,NULL,ptsig,TTV_NAME_OUT,TTV_SEUIL_MAX,
                          TTV_SEUIL_MIN,type|TTV_FIND_MAX|dualmode) ;
       pathx = ttv_getpath(ttvfig,NULL,ptsig,TTV_NAME_OUT,TTV_SEUIL_MAX,
                           TTV_SEUIL_MIN,type|TTV_FIND_MIN|dualmode) ;
      }
     path = (ttvpath_list *)append((chain_list *)path,(chain_list *)pathx) ;
    
     if(path == NULL)
      {
       w1++ ;
       fprintf(file,"WARNING 1 no path found for signal : \n") ;
       fprintf(file,"\n%s\n\n",ttv_getsigname(ttvfig,name,ptsig)) ;
       fflush(file) ;
      }

     for(pathx = path ; pathx != NULL ; pathx = pathx->NEXT)
      {
       nbpath++ ;
       
       if((pathx->TYPE & TTV_FIND_MAX) == TTV_FIND_MAX)
        {
         if((delaymax == TTV_NOTIME) || (delaymax < pathx->DELAY))
           delaymax = pathx->DELAY ;
         nbpathmax++ ;
        }
       else if((pathx->TYPE & TTV_FIND_MIN) == TTV_FIND_MIN)
        {
         if((delaymin == TTV_NOTIME) || (delaymin > pathx->DELAY))
           delaymin = pathx->DELAY ;
         nbpathmin++ ;
        }


       if(tracemode == 'Y')
        {
         fprintf(stdout,"validation path number : %ld\n",nbpath) ;
         fflush(stdout) ;
        }

       if(check_critic == 'Y')
        {
         critic = ttv_getcritic(ttvfig,pathx->FIG,pathx->ROOT,pathx->NODE,
                                pathx->LATCH, pathx->CMDLATCH,
                              (pathx->TYPE|TTV_FIND_LINE) & ~(TTV_FIND_PATH)) ;
         if(critic == NULL)
          {
           e1++ ;
           fprintf(file,"ERROR 1 no detail for path : \n") ;
           val_printpath(ttvfig,file,pathx) ;
           flagerr = 'Y' ;
           newflagerr = 'Y' ;
           fflush(file) ;
          }
         else
          {
           delay = (long)0 ;
           nbcritic++ ;
           for(criticx = critic ; criticx != NULL ; criticx = criticx->NEXT)
             delay += criticx->DELAY ;
           if(delay > pathx->DELAY)
            {
             if((pathx->TYPE & TTV_FIND_MAX) == TTV_FIND_MAX)
              {
               e2++ ;
               fprintf(file,"ERROR 2 detail delay is longer than path delay : %ld ps\n",delay - pathx->DELAY) ;
              }
             else
              {
               w2++ ;
               fprintf(file,"WARNING 2 detail delay is longer than path delay : %ld ps\n",delay - pathx->DELAY) ;
              }
             val_printpath(ttvfig,file,pathx) ;
             val_printcritic(file,critic) ;
             flagerr = 'Y' ;
             newflagerr = 'Y' ;
             fflush(file) ;
            }
           if(delay < pathx->DELAY)
            {
             if((pathx->TYPE & TTV_FIND_MAX) == TTV_FIND_MAX)
              {
               w3++ ;
               fprintf(file,"WARNING 3 detail delay is shorter than path delay : %ldps\n",pathx->DELAY - delay) ;
              }
             else
              {
               e3++ ;
               fprintf(file,"ERROR 3 detail delay is shorter than path delay : %ldps\n",pathx->DELAY - delay) ;
              }
             val_printpath(ttvfig,file,pathx) ;
             val_printcritic(file,critic) ;
             flagerr = 'Y' ;
             newflagerr = 'Y' ;
             fflush(file) ;
            }
          }
         ttv_freecriticlist(critic) ;
         if(tracemode == 'Y')
          {
           fprintf(stdout,"detailed path validation :\n") ;
           fflush(stdout) ;
          }
        }
       if(check_para == 'Y')
        {
         pathp = ttv_getpara(ttvfig,NULL,pathx->ROOT,pathx->NODE,TTV_SEUIL_MAX,
                             TTV_SEUIL_MIN,pathx->TYPE,NULL,1000000) ;
         if(pathp == NULL)
          {
           e6++ ;
           fprintf(file,"ERROR 6 no parallel path for path : \n") ;
           val_printpath(ttvfig,file,pathx) ;
           flagerr = 'Y' ;
           newflagerr = 'Y' ;
           fflush(file) ;
          }
         else if(pathp->DELAY > pathx->DELAY)
          {
           if((pathx->TYPE & TTV_FIND_MAX) == TTV_FIND_MAX)
            {
             e4++ ;
             fprintf(file,"ERROR 4 parallel delay is longer than path delay : %ldps\n",pathp->DELAY - pathx->DELAY) ;
            }
           else
            {
             w4++ ;
             fprintf(file,"WARNING 4 parallel delay is longer than path delay : %ldps\n",pathp->DELAY - pathx->DELAY) ;
            }
           val_printpath(ttvfig,file,pathx) ;
           val_printpath(ttvfig,file,pathp) ;
           chaincp = ttv_getcriticpara(ttvfig,pathp->FIG,pathp->ROOT,
                                       pathp->NODE,pathp->DELAY,
                              (pathp->TYPE|TTV_FIND_LINE) & ~(TTV_FIND_PATH),
                              NULL) ; 
           for(chaincpx = chaincp ; chaincpx != NULL ; 
                 chaincpx = chaincpx->NEXT)
             {
              critic = (ttvcritic_list *)chaincpx->DATA ;
              val_printcritic(file,critic) ;
             }
           ttv_freecriticpara(chaincp) ;
           flagerr = 'Y' ;
           newflagerr = 'Y' ;
           fflush(file) ;
          }
         else if(pathp->DELAY < pathx->DELAY)
          {
           if((path->TYPE & TTV_FIND_MAX) == TTV_FIND_MAX)
            {
             w5++ ;
             fprintf(file,"WARNING 5 parallel delay is shorter than path delay : %ldps\n",pathx->DELAY - pathp->DELAY) ;
            }
           else
            {
             e5++ ;
             fprintf(file,"ERROR 5 parallel delay is shorter than path delay : %ldps\n",pathx->DELAY - pathp->DELAY) ;
            }
           val_printpath(ttvfig,file,pathx) ;
           val_printpath(ttvfig,file,pathp) ;
           chaincp = ttv_getcriticpara(ttvfig,pathp->FIG,pathp->ROOT,
                                       pathp->NODE,pathp->DELAY,
                              (pathp->TYPE|TTV_FIND_LINE) & ~(TTV_FIND_PATH),
                              NULL) ; 
           for(chaincpx = chaincp ; chaincpx != NULL ; 
                 chaincpx = chaincpx->NEXT)
             {
              critic = (ttvcritic_list *)chaincpx->DATA ;
              val_printcritic(file,critic) ;
             }
           ttv_freecriticpara(chaincp) ;
           flagerr = 'Y' ;
           newflagerr = 'Y' ;
           fflush(file) ;
          }
         nbparaeqx = (long)0 ;
         if(tracemode == 'Y')
          {
           fprintf(stdout,"parallel path validation :\n") ;
           fflush(stdout) ;
          }
         for(pathpx = pathp ; pathpx != NULL ; pathpx = pathpx->NEXT)
          {
           nbpara++ ;
           if(check_detail == 'N')
            continue ;
           if(pathpx->NEXT != NULL)
            {
             if(pathpx->DELAY == pathpx->NEXT->DELAY)
              {
               nbparaeq++ ;
               nbparaeqx++ ;
               continue ;
              }
            }
           chaincp = ttv_getcriticpara(ttvfig,pathpx->FIG,pathpx->ROOT,
                                       pathpx->NODE,pathpx->DELAY,
                              (pathpx->TYPE|TTV_FIND_LINE) & ~(TTV_FIND_PATH),
                              NULL) ;  
           if(chaincp == NULL)
            {
             e7++ ;
             fprintf(file,"ERROR 7 no parallel detail for parallel path :\n") ;
             val_printpath(ttvfig,file,pathx) ;
             flagerr = 'Y' ;
             newflagerr = 'Y' ;
             fflush(file) ;
            }
           else
            {
             for(chaincpx = chaincp ; chaincpx != NULL ; 
                 chaincpx = chaincpx->NEXT)
              {
               nbcriticpara++ ;
               if(chaincpx != chaincp)
                 nbparaeqx-- ;
               delay = (long)0 ;
               critic = (ttvcritic_list *)chaincpx->DATA ;
               for(criticx = critic ; criticx != NULL ; criticx = criticx->NEXT)
                 delay += criticx->DELAY ;
               if(delay > pathpx->DELAY)
                {
                 e8++ ;
                 fprintf(file,"ERROR 8 parallel detail is longer than parallel path : %ldps\n",delay - pathpx->DELAY) ;
                 val_printpath(ttvfig,file,pathpx) ;
                 val_printcritic(file,critic) ;
                 flagerr = 'Y' ;
                 newflagerr = 'Y' ;
                 fflush(file) ;
                }
               if(delay < pathpx->DELAY)
                {
                 e9++ ;
                 fprintf(file,"ERROR 9 parallel detail is shorter than parallel path : %ldps\n",pathpx->DELAY - delay) ;
                 val_printpath(ttvfig,file,pathpx) ;
                 val_printcritic(file,critic) ;
                 flagerr = 'Y' ;
                 newflagerr = 'Y' ;
                 fflush(file) ;
                }
              }
            }
           if(nbparaeqx != (long)0)
            {
             e10++ ;
             fprintf(file,"ERROR 10 number of  detail is different than number of parallel path :\n") ;
                 val_printpath(ttvfig,file,pathpx) ;
             flagerr = 'Y' ;
             newflagerr = 'Y' ;
             fflush(file) ;
             nbparaeqx = (long)0 ;
            }
           ttv_freecriticpara(chaincp) ;
          }
         ttv_freepathlist(pathp) ;
         if((tracemode == 'Y') && (check_detail == 'Y'))
          {
           fprintf(stdout,"detailed parallel path validation :\n") ;
           fflush(stdout) ;
          }
        }
       if(tracemode == 'Y')
        {
         if(flagerr == 'N')
           fprintf(stdout,"no error yet\n") ;
         else if(newflagerr == 'Y')
           fprintf(stdout,"errors in this path\n") ;
         else if(flagerr == 'Y')
           fprintf(stdout,"errors in previous path\n") ;
         fprintf(stdout,"\n") ;
        }
       newflagerr = 'N' ;
      }
     ttv_freepathlist(path) ;
    }

 freechain(chainsig) ;


 getrusage(RUSAGE_SELF,&Tend) ;


 time(&tTend) ;

 chainsig = ttv_getsigbytype(ttvfig,NULL,TTV_SIG_C,NULL) ;
 for(chainsigx = chainsig ; chainsigx != NULL ; chainsigx = chainsigx->NEXT)
  nbc++ ;
 freechain(chainsig) ;

 chainsig = ttv_getsigbytype(ttvfig,NULL,TTV_SIG_N,NULL) ;
 for(chainsigx = chainsig ; chainsigx != NULL ; chainsigx = chainsigx->NEXT)
  nbn++ ;
 freechain(chainsig) ;

 chainsig = ttv_getsigbytype(ttvfig,NULL,TTV_SIG_Q,NULL) ;
 for(chainsigx = chainsig ; chainsigx != NULL ; chainsigx = chainsigx->NEXT)
  nbq++ ;
 freechain(chainsig) ;

 chainsig = ttv_getsigbytype(ttvfig,NULL,TTV_SIG_L,NULL) ;
 for(chainsigx = chainsig ; chainsigx != NULL ; chainsigx = chainsigx->NEXT)
  nbl++ ;
 freechain(chainsig) ;

 chainsig = ttv_getsigbytype(ttvfig,NULL,TTV_SIG_R,NULL) ;
 for(chainsigx = chainsig ; chainsigx != NULL ; chainsigx = chainsigx->NEXT)
  nbr++ ;
 freechain(chainsig) ;

 chainsig = ttv_getsigbytype(ttvfig,NULL,TTV_SIG_B,NULL) ;
 for(chainsigx = chainsig ; chainsigx != NULL ; chainsigx = chainsigx->NEXT)
  nbr++ ;
 freechain(chainsig) ;

 chainsig = ttv_getsigbytype(ttvfig,NULL,TTV_SIG_I,NULL) ;
 for(chainsigx = chainsig ; chainsigx != NULL ; chainsigx = chainsigx->NEXT)
  nbi++ ;
 freechain(chainsig) ;

 chainsig = ttv_getttvfiglist(ttvfig) ;

 for(chainsigx = chainsig ; chainsigx != NULL ; chainsigx = chainsigx->NEXT)
  {
   nbfig++ ;
   ttvfigx = chainsigx->DATA ;

   if(ttvfigx == ttvfig)
    {
     if((filetype & TTV_FILE_TTX) == TTV_FILE_TTX)
       ttv_parsttvfig(ttvfigx,TTV_STS_CL_PJT,TTV_FILE_TTX) ;
     nbt += ttvfigx->NBTBLOC ;
     nbj += ttvfigx->NBJBLOC ;
     nbp += ttvfigx->NBPBLOC ;
    }
   else
    {
     if((filetype & TTV_FILE_TTX) == TTV_FILE_TTX)
       ttv_parsttvfig(ttvfigx,TTV_STS_L_PJ,TTV_FILE_TTX) ;
     nbj += ttvfigx->NBJBLOC ;
     nbp += ttvfigx->NBPBLOC ;
    }

   if(ttvfigx == ttvfig)
    {
     if((filetype & TTV_FILE_DTX) == TTV_FILE_DTX)
       ttv_parsttvfig(ttvfigx,TTV_STS_CLS_FED|TTV_STS_DENOTINPT,TTV_FILE_DTX) ;
     nbd += ttvfigx->NBDBLOC ;
     nbe += ttvfigx->NBEBLOC ;
     nbf += ttvfigx->NBFBLOC ;
    }
   else
    {
     if((filetype & TTV_FILE_DTX) == TTV_FILE_DTX)
       ttv_parsttvfig(ttvfigx,TTV_STS_LS_FE|TTV_STS_DENOTINPT,TTV_FILE_DTX) ;
     nbe += ttvfigx->NBEBLOC ;
     nbf += ttvfigx->NBFBLOC ;
    }

   nbs += ttvfigx->NBISIG ;
   for(j = (long)0 ; j < ttvfigx->NBESIG ; j++)
    if((*(ttvfigx->ESIG + j))->ROOT == ttvfigx)
       nbs++ ;
  }
 freechain(chainsig) ;

 if(w1 != 0)
   fprintf(file,"number of warning 1 : %d\n",w1) ;
 if(w2 != 0)
   fprintf(file,"number of warning 2 : %d\n",w2) ;
 if(w3 != 0)
   fprintf(file,"number of warning 3 : %d\n",w3) ;
 if(w4 != 0)
   fprintf(file,"number of warning 4 : %d\n",w3) ;
 if(w5 != 0)
   fprintf(file,"number of warning 5 : %d\n",w3) ;

 if((w1 == 0) && (w2 == 0) && (w3 == 0) && (w4 == 0) && (w5 == 0))
   fprintf(file,"no warning found\n") ;

 if(flagerr == 'N')
   fprintf(file,"no error found\n") ;
 else
  {
   if(e1 != 0)
     fprintf(file,"number of error 1 : %d\n",e1) ;
   if(e2 != 0)
     fprintf(file,"number of error 2 : %d\n",e2) ;
   if(e4 != 0)
     fprintf(file,"number of error 4 : %d\n",e4) ;
   if(e5 != 0)
     fprintf(file,"number of error 5 : %d\n",e5) ;
   if(e6 != 0)
     fprintf(file,"number of error 6 : %d\n",e6) ;
   if(e7 != 0)
     fprintf(file,"number of error 7 : %d\n",e7) ;
   if(e8 != 0)
     fprintf(file,"number of error 8 : %d\n",e8) ;
   if(e9 != 0)
     fprintf(file,"number of error 9 : %d\n",e9) ;
   if(e10 != 0)
     fprintf(file,"number of error 10 : %d\n",e10) ;
  }
 
 fprintf(file,"\nreport : \n") ;
 fprintf(file,"number of fig : %ld \n",nbfig) ;
 fprintf(file,"number of connector : %ld \n",nbc) ;
 fprintf(file,"number of internal connector : %ld \n",nbn) ;
 fprintf(file,"number of command : %ld \n",nbq) ;
 fprintf(file,"number of latch : %ld \n",nbl) ;
 fprintf(file,"number of precharge : %ld \n",nbr) ;
 fprintf(file,"number of factorisation signals : %ld \n",nbi) ;
 fprintf(file,"number of internal signal : %ld \n",nbs) ;
 fprintf(file,"number of d delai : %ld \n",nbd) ;
 fprintf(file,"number of e delai : %ld \n",nbe) ;
 fprintf(file,"number of f delai : %ld \n",nbf) ;
 fprintf(file,"number of t path : %ld \n",nbt) ;
 fprintf(file,"number of j path : %ld \n",nbj) ;
 fprintf(file,"number of p path : %ld \n",nbp) ;
 fprintf(file,"number of delai : %ld \n",nbd+nbe+nbf) ;
 fprintf(file,"number of ends of path : %ld \n",nbc+nbq+nbl+nbr) ;
 fprintf(file,"number of signal : %ld \n",nbc+nbn+nbq+nbl+nbr+nbi+nbs) ;
 fprintf(file,"number of factorized path : %ld \n",nbt+nbj+nbp) ;
 fprintf(file,"number of path : %ld \n",nbpath) ;
 if(nbpathmax < nbpathmin)
  nbpath = nbpathmin ;
 else
  nbpath = nbpathmax ;
 fprintf(file,"reduced causality graphe size : %ld \n",
         (nbc+nbn+nbq+nbl+nbr+nbi)*2 + (nbt+nbj+nbp)) ;
 fprintf(file,"causality graphe size : %ld \n",
         (nbc+nbn+nbq+nbl+nbr+nbi+nbs)*2 + (nbd+nbe+nbf)) ;
 fprintf(file,"path graphe size : %ld \n",
         (nbc+nbq+nbl+nbr)*2 + nbpath) ;
 fprintf(file,"ratio of CG/RCG : %.2f\n",
         ((double)((nbc+nbn+nbq+nbl+nbr+nbi+nbs)*2 + (nbd+nbe+nbf))/
         (double)((nbc+nbn+nbq+nbl+nbr+nbi)*2 + (nbt+nbj+nbp))));
 fprintf(file,"ratio of PG/RCG : %.2f\n",
         ((double)((nbc+nbq+nbl+nbr)*2 + nbpath)/
         (double)((nbc+nbn+nbq+nbl+nbr+nbi)*2 + (nbt+nbj+nbp))));
 fprintf(file,"ratio of PG/CG : %.2f\n",
         ((double)((nbc+nbq+nbl+nbr)*2 + nbpath)/
         (double)((nbc+nbn+nbq+nbl+nbr+nbi+nbs)*2 + (nbd+nbe+nbf))));
 fprintf(file,"ratio of fact point/stab point : %.2f%%\n",
         ((double)(nbi+nbn)/(double)(nbc+nbq+nbl+nbr))*100.0) ;
 fprintf(file,"ratio of fact point/all point : %.2f%%\n",
         ((double)(nbi+nbn)/(double)(nbc+nbn+nbq+nbl+nbr+nbi+nbs))*100.0) ;
 fprintf(file,"ratio of path/fact path : %.2f \n",
         (double)nbpath/(double)(nbt+nbj+nbp)) ;
 fprintf(file,"ratio of path/stab point : %.2f \n",
         (double)(nbpath)/(double)(2*(nbc+nbq+nbl+nbr))) ;
 fprintf(file,"ratio of fact path/stab + fact  point : %.2f \n",
         (double)(nbt+nbj+nbp)/(double)(2*(nbc+nbn+nbq+nbl+nbr+nbi))) ;
 fprintf(file,"ratio of delay/point : %.2f \n",
         (double)(nbd+nbe+nbf)/(double)(2*(nbc+nbn+nbq+nbl+nbr+nbi+nbs))) ;
 fprintf(file,"number of detail path : %ld \n",nbcritic) ;
 fprintf(file,"number of parallel path : %ld \n",nbpara) ;
 fprintf(file,"number of parallel path with same delay : %ld \n",nbparaeq) ;
 fprintf(file,"number of detail parallel path : %ld \n",nbcriticpara) ;
 if(delaymax != TTV_NOTIME)
 fprintf(file,"max path delay : %ld \n",delaymax) ;
 if(delaymin != TTV_NOTIME)
 fprintf(file,"min path delay : %ld \n",delaymin) ;
 val_printrun(file,tTstart,tTend,Tstart,Tend) ;
 
 fprintf(file,"\nend\n") ;

 if(fclose(file) != 0)
    {
     (void)fprintf(stderr, "*** val error ***\n");
     (void)fprintf(stderr, "can not close the file %s.val",ttvfig->INFO->FIGNAME);
     ttv_freeallttvfig(ttvfig) ;
     EXIT(1) ;
    }

 ttv_freeallttvfig(ttvfig) ;

 EXIT(0) ;
 
 return 0;
}

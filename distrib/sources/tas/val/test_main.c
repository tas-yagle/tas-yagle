/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TEST Version 1                                              */
/*    Fichier : test_main.c                                                 */
/*                                                                          */
/*    (c) copyright 1997-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

#include "test_main.h"

void  test_printrun(file,tTstart,tTend,Tstart,Tend)
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
 char *namefig ;
 chain_list *chainfig ;
 chain_list *chain ;
 char buf[1024] ;

#ifndef __ALL__WARNING__
 argc = 0 ; // added to prevent warning, unused ..anto..
#endif
 file = stdout ;

#ifdef AVERTEC
 avtenv() ;
#endif

 mbkenv() ;

 getrusage(RUSAGE_SELF,&Tstart) ;
 time(&tTstart) ;

 getrusage(RUSAGE_SELF,&tstart) ;
 time(&ttstart) ;

 namefig = argv[1] ;

 if((ttvfig = ttv_getttvfig(namefig,(long)0)) == NULL)
  EXIT(0) ;

 chainfig = ttv_getttvfiglist(ttvfig) ;

 for(chain = chainfig ; chain != NULL ; chain = chain->NEXT)
 ttv_parsttvfig((ttvfig_list *)chain->DATA,TTV_STS_DTX,TTV_FILE_DTX) ;


 for(chain = chainfig ; chain != NULL ; chain = chain->NEXT)
  {
   if(argv[2] != NULL)
     namefig = argv[2] ;
   else
     {
      sprintf(buf,"%s_ren",((ttvfig_list *)chain->DATA)->INFO->FIGNAME) ;
      namefig = buf ;
     }
   ((ttvfig_list *)chain->DATA)->INFO->FIGNAME = namealloc(namefig) ;
   ttv_drittv((ttvfig_list *)chain->DATA,TTV_FILE_DTX,(long)0, NULL) ;
  }

 freechain(chainfig) ;

 getrusage(RUSAGE_SELF,&tend) ;
 time(&ttend) ;

 getrusage(RUSAGE_SELF,&Tend) ;
 time(&tTend) ;

 test_printrun(file,tTstart,tTend,Tstart,Tend) ;
 
 EXIT(0) ;
 return 0;
}

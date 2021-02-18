/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TTV Verison 1                                               */
/*    Fichier : ttv_ctx.c                                                   */
/*                                                                          */
/*    (c) copyright 1995-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/
/* driver des fichier resultats                                             */
/****************************************************************************/

#include "ttv.h"

char *CTX_FILE ;
int CTX_LINE ;

extern int ctxparse() ;
extern int ctxrestart() ;
extern FILE *ctxin ;

int ttv_ctxparse(ttvfig)
ttvfig_list *ttvfig ;
{
 char buf[1024] ;
 static char vierge = 1 ;
 int r;

 ctxin = ttv_openfile(ttvfig,TTV_FILE_CTX,READ_TEXT) ;
 CTX_LINE = 1 ;

 sprintf(buf,"%s.ctx",ttvfig->INFO->FIGNAME) ;
 CTX_FILE = buf ;

 if(ctxin == NULL)
   {
    ttv_error(11,buf,TTV_WARNING) ;
    return 0;
   }

 if(vierge == 0)
   ctxrestart(ctxin) ;
 vierge = 0 ;

 r=(ctxparse()==0?1:0) ;
 

 if(fclose(ctxin) != 0)
  ttv_error(21,buf,TTV_WARNING) ;

 return r;
}

void ttv_printctxline(ttvfig,file,type) 
ttvfig_list *ttvfig ;
FILE *file ;
long type ;
{
 char ctype ;
 ttvdelay_list **delay ;
 ttvdelay_list *ptdelay ;
 long i ;
 
 if(ttvfig->DELAY == NULL)
   return ;

 if((type & TTV_STS_D) == TTV_STS_D)
   {
    if((delay = ttvfig->DELAY[TTV_DELAY_D]) == NULL)
       return ;
    ctype = 'D' ;
   }
 if((type & TTV_STS_E) == TTV_STS_E)
   {
    if((delay = ttvfig->DELAY[TTV_DELAY_E]) == NULL)
       return ;
    ctype = 'E' ;
   }
 if((type & TTV_STS_F) == TTV_STS_F)
   {
    if((delay = ttvfig->DELAY[TTV_DELAY_F]) == NULL)
       return ;
    ctype = 'F' ;
   }

 i = 0 ;
 for(ptdelay = delay[i] ; ptdelay != TTV_DELAY_END ; ptdelay = delay[++i]) 
  {
   if(ptdelay != NULL)
    {
     if((ptdelay->VALMAX == TTV_NOTIME) && (ptdelay->VALMIN == TTV_NOTIME))
       continue ;

     fprintf(file,"%c %ld (",ctype,i) ;

     if(ptdelay->VALMAX != TTV_NOTIME)
       {
        fprintf(file,"\n\t((MAX %g",(double)ptdelay->VALMAX/TTV_UNIT) ;
        if(ptdelay->FMAX != TTV_NOSLOPE)
           fprintf(file," %g))",(double)ptdelay->FMAX/TTV_UNIT) ;
        else
           fprintf(file," 0))\n") ;
       }
     if(ptdelay->VALMIN != TTV_NOTIME)
       {
        fprintf(file,"\n\t((MIN %g",(double)ptdelay->VALMIN/TTV_UNIT) ;
        if(ptdelay->FMIN != TTV_NOSLOPE)
           fprintf(file," %g))",(double)ptdelay->FMIN/TTV_UNIT) ;
        else
           fprintf(file," 0))\n") ;
       }
     fprintf(file,");\n\n") ;
    }
  }
}

void ttv_ctxdrive(ttvfig) 
ttvfig_list *ttvfig ;
{
 FILE *file ;
 chain_list *chainfig ;
 chain_list *chain ;
 ttvfig_list *ttvins ;
 char buf[1024] ;

 chainfig = ttv_getttvfiglist(ttvfig) ;
 file = ttv_openfile(ttvfig,TTV_FILE_CTX,WRITE_TEXT) ;

 sprintf(buf,"%s.ctx",ttvfig->INFO->FIGNAME) ;

 if(file == NULL)
  {
   ttv_error(20,buf,TTV_WARNING) ;
   return ;
  }

 fprintf(file,"H %s %s %s %s %s %g %.1f %.2f %.2f %.2f %.2f %.2f %ld (%d %d %d %d %d %d);\n\n",
         ttvfig->INFO->TOOLNAME,ttvfig->INFO->TOOLVERSION,ttvfig->INFO->FIGNAME,
         ttvfig->INFO->TECHNONAME,ttvfig->INFO->TECHNOVERSION,
         (double)ttvfig->INFO->SLOPE/TTV_UNIT,ttvfig->INFO->CAPAOUT,ttvfig->INFO->STHHIGH,
         ttvfig->INFO->STHLOW,ttvfig->INFO->DTH,ttvfig->INFO->TEMP,
         ttvfig->INFO->VDD,ttvfig->INFO->LEVEL,
         ttvfig->INFO->TTVDAY,ttvfig->INFO->TTVMONTH,ttvfig->INFO->TTVYEAR,
         ttvfig->INFO->TTVHOUR,ttvfig->INFO->TTVMIN,ttvfig->INFO->TTVSEC) ;

 for(chain = chainfig ; chain != NULL ; chain = chain->NEXT)
    {
     ttvins = (ttvfig_list *)chain->DATA ;
     if(ttvins == ttvfig)
       {
        fprintf(file,"M %s(\n",ttvfig->INFO->FIGNAME) ;
        ttv_printctxline(ttvfig,file,TTV_STS_D) ;
       }
     else
       {
        fprintf(file,"I %s %s(\n",ttvins->INFO->FIGNAME,
                                  ttv_getinsname(ttvfig,buf,ttvins)) ;
       }
     ttv_printctxline(ttvins,file,TTV_STS_E) ;
     ttv_printctxline(ttvins,file,TTV_STS_F) ;
     fprintf(file,");\n") ;
    }
    
 freechain( chainfig );
 fprintf(file,"\nG;") ;

 if(fclose(file) != 0) {
  ttv_error(21,buf,TTV_WARNING) ;
 }
}

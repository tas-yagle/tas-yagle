/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : ETAS Version 1                                              */
/*    Fichier : etas.c                                                      */
/*                                                                          */
/*    (c) copyright 1997-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

#include "etas.h"

sigjmp_buf GET_ENV ;
ttvfig_list *GET_TTVFIG = NULL ;
ttvpath_list *GET_MAX_LIST = NULL ;
ttvpath_list *GET_PATH_LIST = NULL ;
chain_list *GET_PATH_LIST_SIG = NULL ;
ttvpath_list *GET_PARA_LIST = NULL ;
ttvcritic_list *GET_CRITICMAX_LIST = NULL ;
ttvcritic_list *GET_CRITICPATH_LIST = NULL ;
chain_list *GET_CRITICPARA_LIST = NULL ;
chain_list *GET_LATCH_LIST = NULL ;
chain_list *GET_CMD_LIST = NULL ;
chain_list *GET_PRECHARGE_LIST = NULL ;
chain_list *GET_BREAK_LIST = NULL ;
chain_list *GET_CON_LIST = NULL ;
chain_list *GET_SIG_LIST = NULL ;
chain_list *GET_FIG_LIST = NULL ;
chain_list *GET_NAMEMASK = NULL ;
chain_list *GET_DELAY_LIST = NULL ;
long GET_TTVINS = 0 ;
long GET_PATH_NUM = 0 ;
long GET_TYPE = TTV_FIND_MAX|TTV_FIND_GATE ;
char GET_REQUEST = '\0' ;
char GET_REQUEST_PREVPATH = '\0' ;
char GET_REQUEST_LASTLIST = '\0' ;
char GET_REQUEST_LASTPATH = '\0' ;
char GET_REQUEST_LASTDETAIL = '\0' ;
char ETAS_DEBUG_MODE = 'N' ;
long GET_MAXWINDOW = 10 ;
int GET_CPN = 1 ;
int GET_BATCH = 0 ;
FILE *GET_FILE = NULL ;

void init(mode) 
int mode ;
{
 if(mode == 0)
   ttv_freepathlist(GET_MAX_LIST) ;
 if(mode == 0)
   ttv_freepathlist(GET_PATH_LIST) ;
 if(mode == 0)
   freechain(GET_PATH_LIST_SIG) ;
 if(mode == 0)
   ttv_freepathlist(GET_PARA_LIST) ;

 ttv_freecriticlist(GET_CRITICMAX_LIST) ;
 ttv_freecriticlist(GET_CRITICPATH_LIST) ;
 ttv_freecriticpara(GET_CRITICPARA_LIST) ;
 ttv_freedelaylist(GET_DELAY_LIST) ;
 freechain(GET_LATCH_LIST) ;
 freechain(GET_CMD_LIST) ;
 freechain(GET_PRECHARGE_LIST) ;
 freechain(GET_BREAK_LIST) ;
 freechain(GET_CON_LIST) ;
 freechain(GET_SIG_LIST) ;
 freechain(GET_FIG_LIST) ;
 freechain(GET_NAMEMASK) ;
 freechain(TTV_NAME_IN) ;
 freechain(TTV_NAME_OUT) ;

 if(mode == 0)
   GET_MAX_LIST = NULL ;
 if(mode == 0)
   GET_PATH_LIST = NULL ;
 if(mode == 0)
   GET_PATH_LIST_SIG = NULL ;
 if(mode == 0)
   GET_PARA_LIST = NULL ;

 GET_CRITICMAX_LIST = NULL ;
 GET_CRITICPATH_LIST = NULL ;
 GET_CRITICPARA_LIST = NULL ;
 GET_DELAY_LIST = NULL ;
 GET_LATCH_LIST = NULL ;
 GET_CMD_LIST = NULL ;
 GET_PRECHARGE_LIST = NULL ;
 GET_BREAK_LIST = NULL ;
 GET_CON_LIST = NULL ;
 GET_SIG_LIST = NULL ;
 GET_FIG_LIST = NULL ;
 GET_NAMEMASK = NULL ;
 TTV_NAME_IN = NULL ;
 TTV_NAME_OUT = NULL ;
 TTV_SEUIL_MAX = TTV_DELAY_MAX ;
 TTV_SEUIL_MIN = TTV_DELAY_MIN ;
 GET_TTVINS = 0 ;
 GET_PATH_NUM = 0 ;
 GET_TYPE = TTV_FIND_MAX|TTV_FIND_GATE ;
 GET_MAXWINDOW = 10 ;
 GET_CPN = 1 ;
}

int get_sizelong(l)
long l ;
{
 char buf[1024] ;

 sprintf(buf,"%ld",l) ;
 return(strlen(buf)) ;
}
int get_sizelongTTV_UNIT(l)
long l ;
{
 char buf[1024] ;

 sprintf(buf,"%.1f",l/TTV_UNIT) ;
 return(strlen(buf)) ;
}

void get_printtype(number,type,buf,maxsize)
long number ;
char *type ;
char *buf ;
int maxsize ;
{
 int len ;
 char *pt ;
 
 pt = buf ;

 len = get_sizelong(number) ;
 len = maxsize - len - strlen(type) - 3 ;

 while(len--)
  {
   *pt = ' ' ;
   pt++ ;
  }

 sprintf(pt,"%ld : %s",number,type) ;
}

void get_calcsize(sizetype,sizedelay,sizecumul,sizeslope,sizename1,sizename2,type,chain,delay,w)
int *sizetype ;
int *sizedelay ;
int *sizecumul ;
int *sizeslope ;
int *sizename1 ;
int *sizename2 ;
char type ;
chain_list *chain ;
long *delay ;
long w ;
{
 char buf[1024] ;
 ttvcritic_list *critic ;
 ttvpath_list *path ;
 int auxtype ;
 int auxdelay ;
 int auxslope ;
 int auxname1 ;
 int auxname2 ;
 int first ;
 long i ;

 *sizetype = 4 ;
 *sizedelay = 5 ;
 *sizeslope = 5 ;
 *sizecumul = 5 ;
 *sizename1 = 5 ;
 *sizename2 = 5 ;

 if((type == 'd') || (type == 'm') || (type == 'p'))
  {
   first = 0 ;
   *delay = (long)0 ;
   if(type == 'p')
    critic = (ttvcritic_list *)chain->DATA ;
   else
    critic = (ttvcritic_list *)chain ;
   for(; critic != NULL ; critic = critic->NEXT)
     {
      *delay += critic->DELAY ;
      auxdelay = get_sizelongTTV_UNIT(critic->DELAY) ;
      if(auxdelay > *sizedelay)
       *sizedelay = auxdelay ;
      auxslope = get_sizelongTTV_UNIT(critic->SLOPE) ;
      if(auxslope > *sizeslope)
       *sizeslope = auxslope ;
      if(critic->NEXT != NULL)
       {
        auxname1 = strlen(critic->NAME) ;
        if(auxname1 > *sizename1)
         *sizename1 = auxname1 ;
       }
      if(first != 0)
       {
        auxname2 = strlen(critic->NAME) ;
        if(auxname2 > *sizename2)
         *sizename2 = auxname2 ;
       }
      first = 1 ;
     }
   *sizecumul = get_sizelongTTV_UNIT(*delay) ;
   if(*sizecumul < 5) *sizecumul = 5 ;
   *sizetype = 4 ;
  }
 else if(type == 'T')
  {
   i = w ;
   w = w + GET_MAXWINDOW - 1 ; 
   for(; chain != NULL ; chain = chain->NEXT)
    {
     critic = (ttvcritic_list *)chain->DATA ;
     auxdelay = get_sizelongTTV_UNIT(critic->NEXT->DELAY) ;
     if(auxdelay > *sizedelay)
      *sizedelay = auxdelay ;
     auxslope = get_sizelongTTV_UNIT(critic->NEXT->SLOPE) ;
     if(auxslope > *sizeslope)
      *sizeslope = auxslope ;
     auxname1 = strlen(critic->NAME) ;
     if(auxname1 > *sizename1)
      *sizename1 = auxname1 ;
     auxname2 = strlen(critic->NEXT->NAME) ;
     if(auxname2 > *sizename2)
      *sizename2 = auxname2 ;
     auxtype = get_sizelong(i) + 5 ;
     if(auxtype > *sizetype)
      *sizetype = auxtype ;
     if(i > w ) 
      break ;
     i++ ;
    }
   *sizecumul = 0 ;
  }
 else
  {
   i = w ;
   w = w + GET_MAXWINDOW - 1 ; 
   for(; chain != NULL ; chain = chain->NEXT)
    {
     path = (ttvpath_list *)chain ;

     auxdelay = get_sizelongTTV_UNIT(path->DELAY) ;
     if(auxdelay > *sizedelay)
      *sizedelay = auxdelay ;
     auxslope = get_sizelongTTV_UNIT(path->SLOPE) ;
     if(auxslope > *sizeslope)
      *sizeslope = auxslope ;
     auxname1 = strlen(ttv_getsigname(GET_TTVFIG,buf,path->NODE->ROOT)) ;
     if(auxname1 > *sizename1)
      *sizename1 = auxname1 ;
     auxname2 = strlen(ttv_getsigname(GET_TTVFIG,buf,path->ROOT->ROOT)) ;
     if(auxname2 > *sizename2)
      *sizename2 = auxname2 ;
     auxtype = get_sizelong(i) + 8 ;
     if(auxtype > *sizetype)
      *sizetype = auxtype ;
     if(i > w ) 
      break ;
     i++ ;
    }
   *sizecumul = 0 ;
  }
}

void get_printfirstend(sizetype,sizedelay,sizecumul,sizeslope,sizename1,sizename2)
int sizetype ;
int sizedelay ;
int sizecumul ;
int sizeslope ;
int sizename1 ;
int sizename2 ;
{
 char buf[2048] ;
 char *pt ;
 int len ;

 pt = buf ;

 if(sizetype > 0)
  {
   len = sizetype ;
   while(len--)
    {
     *pt = ' ' ;
     pt++ ;
    }
   *pt = ' ' ;
   pt++ ;
  }

 if(sizedelay > 0)
  {
   len = sizedelay - 2 ;
   while(len--)
    {
     *pt = ' ' ;
     pt++ ;
    }
   strcpy(pt,"Tp") ;
   pt += 2 ;
   *pt = ' ' ;
   pt++ ;
  }
 
 if(sizecumul > 0)
  {
   len = sizecumul - 5 ;
   while(len--)
    {
     *pt = ' ' ;
     pt++ ;
    }
   strcpy(pt,"Total") ;
   pt += 5 ;
   *pt = ' ' ;
   pt++ ;
  }
 
 if(sizeslope > 0)
  {
   len = sizeslope - 5 ;
   while(len--)
    {
     *pt = ' ' ;
     pt++ ;
    }
   strcpy(pt,"Slope") ;
   pt += 5 ;
   *pt = ' ' ;
   pt++ ;
  }
 
 if(sizename1 > 0)
  {
   len = sizename1 - 4 ;
   while(len--)
    {
     *pt = ' ' ;
     pt++ ;
    }
   strcpy(pt,"From    ") ;
   pt += 8 ;
  }

 if(sizename2 > 0)
  {
   strcpy(pt,"To") ;
   pt += 2 ;
   len = sizename2 - 2 ;
   while(len--)
    {
     *pt = ' ' ;
     pt++ ;
    }
  }

 *pt = '\0' ;

 fprintf(stdout,"%s\n",buf) ;
}

void get_printlinesepar(sizetype,sizedelay,sizecumul,sizeslope,sizename1,sizename2)
int sizetype ;
int sizedelay ;
int sizecumul ;
int sizeslope ;
int sizename1 ;
int sizename2 ;
{
 char buf[2048] ;
 char *pt ;
 int len ;

 fprintf(stdout,"#") ;
 pt = buf ;

 if(sizetype > 0)
  {
   len = sizetype - 1 ;
   while(len--)
    {
     *pt = '-' ;
     pt++ ;
    }
   *pt = ' ' ;
   pt++ ;
  }

 if(sizedelay > 0)
  {
   len = sizedelay ;
   while(len--)
    {
     *pt = '-' ;
     pt++ ;
    }
   *pt = ' ' ;
   pt++ ;
  }

 if(sizecumul > 0)
  {
   len = sizecumul ;
   while(len--)
    {
     *pt = '-' ;
     pt++ ;
    }
   *pt = ' ' ;
   pt++ ;
  }

 if(sizeslope > 0)
  {
   len = sizeslope ;
   while(len--)
    {
     *pt = '-' ;
     pt++ ;
    }
   *pt = ' ' ;
   pt++ ;
  }

 if(sizename1 > 0)
  {
   len = sizename1 ;
   while(len--)
    {
     *pt = '-' ;
     pt++ ;
    }
   *pt = ' ' ;
   pt++ ;
   strcpy(pt,"-- ") ;
   pt+= 3 ;
  }

 if(sizename2 > 0)
  {
   len = sizename2 ;
   while(len--)
    {
     *pt = '-' ;
     pt++ ;
    }
  }

 *pt = '\0' ;

 fprintf(stdout,"%s\n",buf) ;
}

void get_printline(typedelay,sizetype,delay,sizedelay,cumul,sizecumul,
                   slope,sizeslope,name1,sizename1,slope1,slope2,
                   name2,sizename2)
char *typedelay ;
int sizetype ;
long delay ;
int sizedelay ;
long cumul ;
int sizecumul ;
long slope ;
int sizeslope ;
char *name1 ;
int sizename1 ;
char slope1 ;
char slope2 ;
char *name2 ;
int sizename2 ;
{
 char buf[2048] ;
 char *pt ;
 int len ;

 if(sizetype > 0)
  {
   fprintf(stdout,"%s ",typedelay) ;
  }

 pt = buf ;

 if(sizedelay > 0)
  {
   len = get_sizelongTTV_UNIT(delay) ;
   len = sizedelay - len ;
   while(len--)
    {
     *pt = ' ' ;
     pt++ ;
    }
   sprintf(pt,"%.1f ",delay/TTV_UNIT) ;
   pt += (get_sizelongTTV_UNIT(delay) + 1) ;
  }
 
 if(sizecumul > 0)
  {
   len = get_sizelongTTV_UNIT(cumul) ;
   len = sizecumul - len ;
   while(len--)
    {
    *pt = ' ' ;
     pt++ ;
    }
   sprintf(pt,"%.1f ",cumul/TTV_UNIT) ;
   pt += (get_sizelongTTV_UNIT(cumul) + 1) ;
  }

 if(sizeslope > 0)
  {
   len = get_sizelongTTV_UNIT(slope) ;
   len = sizeslope - len ;
   while(len--)
    {
     *pt = ' ' ;
     pt++ ;
    }
   sprintf(pt,"%.1f ",slope/TTV_UNIT) ;
   pt += (get_sizelongTTV_UNIT(slope) + 1) ;
  }

 if(sizename1 > 0)
  {
   len = strlen(name1) ;
   len = sizename1 - len ;
   while(len--)
    {
     *pt = ' ' ;
     pt++ ;
    }
   sprintf(pt,"%s %c%c ",name1,slope1,slope2) ;
   pt = pt + strlen(name1) + 4 ;
  }

 if(sizename2 > 0)
  {
   sprintf(pt,"%s",name2) ;
   len = strlen(name2) ;
   pt += len ;
   len = sizename2 - len ;
   while(len--)
    {
     *pt = ' ' ;
     pt++ ;
    }
  }

 *pt = '\0' ;

 fprintf(stdout,"%s\n",buf) ;
}

short retkey(chaine)
char *chaine ;
{
 short i ;
 
 for(i = 0 ; (i<(signed)strlen(chaine)) && (chaine[i] != '=') ; i ++) ;
 
 return((i == (signed)strlen(chaine)) ? -1 : i) ;
}

void *get_chainnumber(chain,n)
chain_list *chain ;
long n ;
{
if(n == 0)
 return(chain) ;

for(;(chain != NULL) && (n != (long)1) ; n--,chain = chain->NEXT) ;

if((chain != NULL) && (n == (long)1)) return((void *)chain) ;
else return(NULL) ;
}

char *get_getarg(buf,name,pos)
char *buf ;
char *name ;
int pos ;
{
 int posx = 0 ;

 if((pos == 0) || (strcmp(buf,"") == 0)) 
   {
    *name = '\0' ;
    return(NULL) ;
   }

 for(;;)
  {
   if((isspace((int)*buf) == 0) && (*buf != '\0'))
    {
     posx++ ;
     while((isspace((int)*buf) == 0) && (*buf != '\0'))
      {
       if(pos == posx)
        {
         *name = *buf ;
          name++ ;
        }
       buf++ ;
      }
     if(pos == posx)
      {
       *name = '\0' ;
       break ;
      }
    }
   if((*buf == '\0') || (*buf == '\n'))
     break  ;
   buf++ ;
  }
 if(pos == posx)
  {
   name++ ;
   return(name) ;
  }
 else 
  {
   *name = '\0' ;
   return(NULL) ;
  }
}

int get_builtlist()
{
 ttvfig_list *ttvfig ;
 ttvfig_list *ttvins ;
 ttvpath_list *path ;
 chain_list *chain ;

 if((ttvfig = GET_TTVFIG) == NULL)
  return(0) ;

 if(GET_TTVINS == 0)
   ttvins = NULL ;
 else if(GET_TTVINS == 1)
   ttvins = GET_TTVFIG ;
 else 
  {
   if(GET_FIG_LIST == NULL)
     return(0) ;
   chain = get_chainnumber(GET_FIG_LIST,GET_TTVINS) ;
   if(chain == NULL) 
     return(0) ;
   ttvins = (ttvfig_list *)chain->DATA ;
  }

 switch(GET_REQUEST)
  {
   case GET_LATCHLIST     : if(GET_LATCH_LIST != NULL)
                              freechain(GET_LATCH_LIST) ;
                            if(ttvins != NULL)
                              GET_LATCH_LIST = ttv_getsigbytype(ttvfig,ttvins,
                                               TTV_SIG_L,GET_NAMEMASK) ;
                            else
                              GET_LATCH_LIST = ttv_getsigbytype(ttvfig,NULL,
                                               TTV_SIG_L,GET_NAMEMASK) ;
                            break ;
   case GET_CMDLIST       : if(GET_CMD_LIST != NULL)
                              freechain(GET_CMD_LIST) ;
                            if(ttvins != NULL)
                              GET_CMD_LIST = ttv_getsigbytype(ttvfig,ttvins,
                                               TTV_SIG_Q,GET_NAMEMASK) ;
                            else
                              GET_CMD_LIST = ttv_getsigbytype(ttvfig,NULL,
                                               TTV_SIG_Q,GET_NAMEMASK) ;
                            break ;
   case GET_PRECHARGELIST : if(GET_PRECHARGE_LIST != NULL)
                              freechain(GET_PRECHARGE_LIST) ;
                            if(ttvins != NULL)
                              GET_PRECHARGE_LIST = ttv_getsigbytype(ttvfig,
                                           ttvins,TTV_SIG_R,GET_NAMEMASK) ;
                            else
                              GET_PRECHARGE_LIST = ttv_getsigbytype(ttvfig,NULL,
                                               TTV_SIG_R,GET_NAMEMASK) ;
                            break ;
   case GET_BREAKLIST     : if(GET_BREAK_LIST != NULL)
                              freechain(GET_BREAK_LIST) ;
                            if(ttvins != NULL)
                              GET_BREAK_LIST = ttv_getsigbytype(ttvfig,
                                           ttvins,TTV_SIG_B,GET_NAMEMASK) ;
                            else
                              GET_BREAK_LIST = ttv_getsigbytype(ttvfig,NULL,
                                               TTV_SIG_B,GET_NAMEMASK) ;
                            break ;
   case GET_CONLIST       : if(GET_CON_LIST != NULL)
                              freechain(GET_CON_LIST) ;
                            GET_CON_LIST = ttv_getsigbytype(ttvfig,ttvfig,
                                           TTV_SIG_C,GET_NAMEMASK) ;
                            break ;
   case GET_SIGLIST       : if(GET_SIG_LIST != NULL)
                              ttv_freenamelist(GET_SIG_LIST) ;
                            GET_SIG_LIST = ttv_getsignamelist(ttvfig,
                                               ttvins,GET_NAMEMASK) ;
                            break ;
   case GET_FIGLIST       : if(GET_FIG_LIST != NULL)
                              freechain(GET_CON_LIST) ;
                            GET_FIG_LIST = ttv_getttvfiglist(ttvfig) ;
                            break ;
   case GET_DELAYLIST     : if(GET_DELAY_LIST != NULL)
                              ttv_freedelaylist(GET_DELAY_LIST) ;
                            GET_DELAY_LIST = ttv_getdelay(ttvfig,ttvins,
                                                          TTV_NAME_IN,
                                                          TTV_NAME_OUT,
                                                          TTV_SEUIL_MAX,
                                                          TTV_SEUIL_MIN,
                                                          GET_TYPE) ;
                            break ;
   case GET_MAX           : if(GET_MAX_LIST != NULL)
                              ttv_freepathlist(GET_MAX_LIST) ;
                            GET_MAX_LIST = ttv_getcriticpath(ttvfig,ttvins,
                                           TTV_NAME_IN,
                                           TTV_NAME_OUT,TTV_SEUIL_MAX,
                                           TTV_SEUIL_MIN,GET_CPN,GET_TYPE) ;
                            break ;
   case GET_PARA          : if(GET_REQUEST_PREVPATH == GET_MAX)
                              path = GET_MAX_LIST ;
                            else
                              path = GET_PATH_LIST ;
                            if((path = (ttvpath_list *)get_chainnumber(
                            (chain_list *)path,GET_PATH_NUM)) != NULL)
                             {
                              if(GET_PARA_LIST != NULL)
                              ttv_freepathlist(GET_PARA_LIST) ;
                              GET_PARA_LIST = ttv_getpara(ttvfig,ttvins,
                              path->ROOT,path->NODE,TTV_SEUIL_MAX,
                              TTV_SEUIL_MIN,GET_TYPE,GET_NAMEMASK,1000) ;
                             }
                            else 
                            return(0) ;
                            break ;
   case GET_PATH          : if(GET_PATH_LIST_SIG != NULL)
                              freechain(GET_PATH_LIST_SIG) ;
                            if((GET_TYPE & TTV_FIND_DUAL) == TTV_FIND_DUAL)
                               GET_PATH_LIST_SIG = ttv_getsigbytype(ttvfig,NULL,
                               TTV_SIG_C|TTV_SIG_L|TTV_SIG_R,
                               TTV_NAME_IN) ;
                            else
                               GET_PATH_LIST_SIG = ttv_getsigbytype(ttvfig,NULL,
                               TTV_SIG_C|TTV_SIG_L|TTV_SIG_Q|TTV_SIG_R,
                               TTV_NAME_OUT) ;
                            if(GET_PATH_LIST != NULL)
                             ttv_freepathlist(GET_PATH_LIST) ;
                            if(GET_PATH_LIST_SIG == NULL)
                              return(0) ;
                            if((GET_TYPE & TTV_FIND_DUAL) == TTV_FIND_DUAL)
                            GET_PATH_LIST = ttv_getpath(ttvfig,ttvins,
                            (ttvsig_list *)GET_PATH_LIST_SIG->DATA,
                            TTV_NAME_OUT,TTV_SEUIL_MAX,TTV_SEUIL_MIN,GET_TYPE) ;
                            else
                            GET_PATH_LIST = ttv_getpath(ttvfig,ttvins,
                            (ttvsig_list *)GET_PATH_LIST_SIG->DATA,
                            TTV_NAME_IN,TTV_SEUIL_MAX,TTV_SEUIL_MIN,GET_TYPE) ;
                            break ;
   case GET_CRITICMAX     : if((path = (ttvpath_list *)get_chainnumber(
                            (chain_list *)GET_MAX_LIST,GET_PATH_NUM)) != NULL)
                             {
                              if(GET_CRITICMAX_LIST != NULL)
                              ttv_freecriticlist(GET_CRITICMAX_LIST) ;
                              GET_CRITICMAX_LIST = ttv_getcritic(ttvfig,
                              path->FIG,path->ROOT,path->NODE,path->LATCH,path->CMDLATCH,(path->TYPE|
                              TTV_FIND_LINE) & ~(TTV_FIND_PATH)) ;
                             }
                            else 
                            return(0) ;
                            break ;
   case GET_CRITICPATH    : if((path = (ttvpath_list *)get_chainnumber(
                            (chain_list *)GET_PATH_LIST,GET_PATH_NUM)) != NULL)
                             {
                              if(GET_CRITICPATH_LIST != NULL)
                              ttv_freecriticlist(GET_CRITICPATH_LIST) ;
                              GET_CRITICPATH_LIST = ttv_getcritic(ttvfig,
                              path->FIG,path->ROOT,path->NODE,path->LATCH,path->CMDLATCH,(path->TYPE|
                              TTV_FIND_LINE) & ~(TTV_FIND_PATH)) ;
                             }
                            else 
                            return(0) ;
                            break ;
   case GET_CRITICPARA    : if((path = (ttvpath_list *)get_chainnumber(
                            (chain_list *)GET_PARA_LIST,GET_PATH_NUM)) != NULL)
                             {
                              if(GET_CRITICPARA_LIST != NULL)
                              ttv_freecriticpara(GET_CRITICPARA_LIST) ;
                              GET_CRITICPARA_LIST = ttv_getcriticpara(ttvfig,
                              path->FIG,path->ROOT,path->NODE,path->DELAY,
                              (path->TYPE|TTV_FIND_LINE) & ~(TTV_FIND_PATH),
                              GET_NAMEMASK) ;
                             }
                            else 
                            return(0) ;
                            break ;
   default                : return(0) ;
  }
 return(1) ;
}

int get_printlist()
{
 char name[1024] ;
 char namx[1024] ;
 char c[48] ;
 chain_list *chain ;
 ttvfig_list *ttvins ;
 ttvpath_list *path ;
 ttvcritic_list *critic ;
 ttvcritic_list *auxcritic ;
 long i ;
 long j ;
 long n ;
 long m ;
 char ppath = 'N' ;
 char type  ;
 char slope1 ;
 char slope2 ;
 char *name1 ;
 char *name2 ;
 char *typedelay ;
 long delay ;
 long slope ;
 long cumul ;
 int sizetype ;
 int sizedelay ;
 int sizeslope ;
 int sizecumul ;
 int sizename1 ;
 int sizename2 ;

 if(GET_TTVFIG == NULL)
    return(0) ;

 if(GET_TTVINS == 0)
   ttvins = NULL ;
 else if(GET_TTVINS == 1)
   ttvins = GET_TTVFIG ;
 else
  {
   if(GET_FIG_LIST == NULL)
     return(0) ;
   chain = get_chainnumber(GET_FIG_LIST,GET_TTVINS) ;
   if(chain == NULL) 
     return(0) ;
   ttvins = (ttvfig_list *)chain->DATA ;
  }

 switch(GET_REQUEST)
  {
   case GET_LATCHLIST     : chain = GET_LATCH_LIST ;
                            type = 'L' ;
                            break ;
   case GET_CMDLIST       : chain = GET_CMD_LIST ;
                            type = 'Q' ;
                            break ;
   case GET_PRECHARGELIST : chain = GET_PRECHARGE_LIST ;
                            type = 'R' ;
                            break ;
   case GET_BREAKLIST     : chain = GET_BREAK_LIST ;
                            type = 'B' ;
                            break ;
   case GET_CONLIST       : chain = GET_CON_LIST ;
                            type = 'C' ;
                            break ;
   case GET_SIGLIST       : chain = GET_SIG_LIST ;
                            type = 'S' ;
                            break ;
   case GET_FIGLIST       : chain = GET_FIG_LIST ;
                            type = 'I' ;
                            break ;
   case GET_DELAYLIST     : chain = GET_DELAY_LIST ;
                            type = 'T' ;
                            break ;
   case GET_MAX           : chain = (chain_list *)GET_MAX_LIST ;
                            type = 'M' ;
                            break ;
   case GET_CRITICMAX     : chain = (chain_list *)GET_CRITICMAX_LIST ;
                            type = 'm' ;
                            break ;
   case GET_PATH          : chain = (chain_list *)GET_PATH_LIST_SIG ;
                            type = 'D' ;
                            break ;
   case GET_CRITICPATH    : chain = (chain_list *)GET_CRITICPATH_LIST ;
                            type = 'd' ;
                            break ;
   case GET_PARA          : chain = (chain_list *)GET_PARA_LIST ;
                            type = 'P' ;
                            break ;
   case GET_CRITICPARA    : chain = (chain_list *)GET_CRITICPARA_LIST ;
                            type = 'p' ;
                            break ;
   default                : return(0) ;
        
  }

 i = (long)1 ;
 n = (long)1 ;
 j = GET_MAXWINDOW ;
 m = GET_MAXWINDOW ;

 if((type == 'I') && (chain != NULL))
   {
    fprintf(stdout,"1 : F modele name : %s\n",GET_TTVFIG->INFO->FIGNAME) ;
     fprintf(stdout,"%lx : t = %ld : j = %ld : p = %ld : d = %ld : e = %ld : f = %ld : se = %ld : si = %ld \n",
             GET_TTVFIG->STATUS,
             GET_TTVFIG->NBTBLOC,
             GET_TTVFIG->NBJBLOC,
             GET_TTVFIG->NBPBLOC,
             GET_TTVFIG->NBDBLOC,
             GET_TTVFIG->NBEBLOC,
             GET_TTVFIG->NBFBLOC, 
             GET_TTVFIG->NBESIG, 
             GET_TTVFIG->NBISIG) ; 
    i++ ;
   }

 if(chain != NULL)
 for(; chain != NULL ; chain = chain->NEXT) 
  {
   fflush(stdout) ;
   if(type == 'S')
     fprintf(stdout,"%ld : %c %s\n",i,type,(char *)chain->DATA) ; 
   else if(type == 'C')
     fprintf(stdout,"%ld : %c %c %s\n",i,type,
             ttv_getdir((ttvsig_list *)chain->DATA),
             ttv_getsigname(GET_TTVFIG,name,(ttvsig_list *)chain->DATA)) ; 
   else if((type == 'd') || (type == 'm') || (type == 'p') || (type == 'T'))
    {
     if((type != 'T') || (i % GET_MAXWINDOW == 1))
      {
       get_calcsize(&sizetype,&sizedelay,&sizecumul,&sizeslope,
                    &sizename1,&sizename2,type,chain,&delay,i) ;
      }
     if(type != 'T')
       fprintf(stdout,"%ld : %c total delay = %.1f ps\n\n",n,type,delay/TTV_UNIT);
     if((type != 'T') || (i % GET_MAXWINDOW == 1))
      {
       get_printfirstend(sizetype,sizedelay,sizecumul,sizeslope,
                         sizename1,sizename2) ;
       get_printlinesepar(sizetype,sizedelay,sizecumul,sizeslope,
                          sizename1,sizename2) ;
      }
     if((type == 'p') || (type == 'T'))
      critic = (ttvcritic_list *)chain->DATA ;
     else
      critic = (ttvcritic_list *)chain ;
     cumul = critic->DELAY ;
     for(; critic->NEXT != NULL ; critic = critic->NEXT)
      {
       auxcritic = critic->NEXT ;
       if(type != 'T')
        {
         if((auxcritic->TYPE & TTV_FIND_RC) == TTV_FIND_RC)
           typedelay = "# RC" ;
         else
           typedelay = "# GT" ;
        }
       else
        {
         if((auxcritic->TYPE & TTV_FIND_RC) == TTV_FIND_RC)
           get_printtype(i,"RC",c,sizetype) ;
         else
           get_printtype(i,"GT",c,sizetype) ;
         typedelay = c ;
        }
       if(critic->SNODE == TTV_UP)
         slope1 = 'U' ;
       else 
         slope1 = 'D' ;
       name1 = critic->NAME ;
       if(((auxcritic->TYPE & TTV_FIND_HZ) == TTV_FIND_HZ) &&
          (auxcritic->NEXT == NULL))
         slope2 = 'Z' ;
       else if(auxcritic->SNODE == TTV_UP)
         slope2 = 'U' ;
       else
         slope2 = 'D' ;
       name2 = auxcritic->NAME ;
       slope = auxcritic->SLOPE ;
       delay = auxcritic->DELAY ;
       cumul += auxcritic->DELAY ;
       get_printline(typedelay,sizetype,delay,sizedelay,cumul,sizecumul,
                     slope,sizeslope,name1,sizename1,slope1,slope2,
                     name2,sizename2) ;
      }
     if((type != 'T') || (i % GET_MAXWINDOW == 0) || (chain->NEXT == NULL))
      {
       get_printlinesepar(sizetype,sizedelay,sizecumul,sizeslope,
                          sizename1,sizename2) ;
       get_printfirstend(sizetype,sizedelay,sizecumul,sizeslope,
                         sizename1,sizename2) ;
       fprintf(stdout,"\n") ;
      }
     if((type != 'p') && (type != 'T'))
      {
       fflush(stdout) ;
       return(1) ;
      }
     if(type != 'T')
      i = j ;
    }
   else if((type == 'M') || (type == 'P'))
    {
     if(i % GET_MAXWINDOW == 1)
      {
       get_calcsize(&sizetype,&sizedelay,&sizecumul,&sizeslope,
                    &sizename1,&sizename2,type,chain,&delay,i) ;
      }
     if(i % GET_MAXWINDOW == 1)
      {
       get_printfirstend(sizetype,sizedelay,sizecumul,sizeslope,
                         sizename1,sizename2) ;
       get_printlinesepar(sizetype,sizedelay,sizecumul,sizeslope,
                          sizename1,sizename2) ;
      }
     path = (ttvpath_list *)chain ;
     
     if((path->TYPE & TTV_FIND_MAX) == TTV_FIND_MAX)
       {
        sprintf(name,"%c %s",type,"MAX") ;
       }
     else
       {
        sprintf(name,"%c %s",type,"MIN") ;
       }
     get_printtype(i,name,c,sizetype) ;
     typedelay = c ;
     get_printline(typedelay,sizetype,path->DELAY,sizedelay,(long)0,sizecumul,
                   path->SLOPE,sizeslope,
                   ttv_getsigname(GET_TTVFIG,name,path->NODE->ROOT),sizename1,
                   GET_NODELEVEL(path->NODE),
                   ((path->TYPE & TTV_FIND_HZ) == TTV_FIND_HZ) ? 'Z' : 
                                             GET_NODELEVEL(path->ROOT),
                   ttv_getsigname(GET_TTVFIG,namx,path->ROOT->ROOT),sizename2) ;

     if((i % GET_MAXWINDOW == 0) || (chain->NEXT == NULL))
      {
       get_printlinesepar(sizetype,sizedelay,sizecumul,sizeslope,
                          sizename1,sizename2) ;
       get_printfirstend(sizetype,sizedelay,sizecumul,sizeslope,
                         sizename1,sizename2) ;
       fprintf(stdout,"\n") ;
      }
    }
   else if(type == 'D')
    {
     path = GET_PATH_LIST ;
     if((path == NULL) && (chain == GET_PATH_LIST_SIG))
          continue ;
     if(chain != GET_PATH_LIST_SIG)
      {
       ttvpath_list *pathx ;
       long type ;

       path = GET_PATH_LIST ;
       if(path != NULL)
        {
         type = path->TYPE ;
         if(((GET_TYPE & TTV_FIND_LINE) == TTV_FIND_LINE) &&
            ((GET_TYPE & TTV_FIND_PATH) != TTV_FIND_PATH))
          {
           type |= TTV_FIND_LINE ;
           type &= ~(TTV_FIND_PATH) ;
          }
        }
       else
        type = GET_TYPE ;
       pathx = ttv_getpath(GET_TTVFIG,ttvins,(ttvsig_list *)chain->DATA,
       (((type & TTV_FIND_DUAL) == TTV_FIND_DUAL)? 
       TTV_NAME_OUT : TTV_NAME_IN), TTV_SEUIL_MAX,TTV_SEUIL_MIN,
                       type & (TTV_FIND_DUAL|TTV_FIND_MAX|TTV_FIND_MIN|
                               TTV_FIND_LINE|TTV_FIND_PATH)) ;
       if(pathx != NULL)
        {
         GET_PATH_LIST = pathx ;
         ttv_freepathlist(path) ;
         path = GET_PATH_LIST ;
        }
       else continue ;
      }
     for(; path != NULL ; path = path->NEXT)
      {
       ppath = 'Y' ;
       if(n % GET_MAXWINDOW == 1)
        {
         get_calcsize(&sizetype,&sizedelay,&sizecumul,&sizeslope,
                      &sizename1,&sizename2,type,(chain_list *)path,&delay,n) ;
        }
       if(n % GET_MAXWINDOW == 1)
        {
         get_printfirstend(sizetype,sizedelay,sizecumul,sizeslope,
                           sizename1,sizename2) ;
         get_printlinesepar(sizetype,sizedelay,sizecumul,sizeslope,
                            sizename1,sizename2) ;
        }
       
       if((path->TYPE & TTV_FIND_MAX) == TTV_FIND_MAX)
         {
          sprintf(name,"%c %s",type,"MAX") ;
         }
       else
         {
          sprintf(name,"%c %s",type,"MIN") ;
         }
       get_printtype(n,name,c,sizetype) ;
       typedelay = c ;
       get_printline(typedelay,sizetype,path->DELAY,sizedelay,(long)0,sizecumul,
                     path->SLOPE,sizeslope,
                     ttv_getsigname(GET_TTVFIG,name,path->NODE->ROOT),sizename1,
                     GET_NODELEVEL(path->NODE),
                     ((path->TYPE & TTV_FIND_HZ) == TTV_FIND_HZ) ? 'Z' : 
                                               GET_NODELEVEL(path->ROOT),
                     ttv_getsigname(GET_TTVFIG,namx,path->ROOT->ROOT),
                     sizename2) ;
  
       if((n % GET_MAXWINDOW == 0) || (path->NEXT == NULL))
        {
         get_printlinesepar(sizetype,sizedelay,sizecumul,sizeslope,
                            sizename1,sizename2) ;
         get_printfirstend(sizetype,sizedelay,sizecumul,sizeslope,
                           sizename1,sizename2) ;
         fprintf(stdout,"\n") ;
        }
       if((n >= m) && (path->NEXT != NULL))
        {
         fprintf(stdout,"---return for more, s to stop or n for next reference---\n") ;
         fgets(c, sizeof(c), stdin) ;
         if((*c == 's') && (*(c+1) == '\0'))
          {
           fflush(stdout) ;
           return(1) ;
          }
         else if((*c == 'n') && (*(c+1) == '\0'))
          {
           break ;
          }
         m += GET_MAXWINDOW ;
        }
       n++ ;
      }
     n = (long)1 ;
     m = GET_MAXWINDOW ;
     if(path != NULL)
      continue ;
     if(chain->NEXT != NULL)
      {
       fprintf(stdout,"---return for more or s with return to stop---\n") ;
       fgets(c, sizeof(c), stdin) ;
       if((*c == 's') && (*(c+1) == '\0'))
        {
         fflush(stdout) ;
         if(ppath == 'Y')
           return(1) ;
         else
           return(0) ;
        }
      }
     continue ;
    }
   else if(type == 'I')
    {
     if(GET_TTVFIG != (ttvfig_list *)chain->DATA)
     fprintf(stdout,"%ld : %c modele name : %s instance name : %s\n",i,type,
             ((ttvfig_list *)chain->DATA)->INFO->FIGNAME,
             ((ttvfig_list *)chain->DATA)->INSNAME) ; 
     else
      continue ;
     fprintf(stdout,"%lx : t = %ld : j = %ld : p = %ld : d = %ld : e = %ld : f = %ld : se = %ld : si = %ld\n",
             ((ttvfig_list *)chain->DATA)->STATUS,
             ((ttvfig_list *)chain->DATA)->NBTBLOC,
             ((ttvfig_list *)chain->DATA)->NBJBLOC,
             ((ttvfig_list *)chain->DATA)->NBPBLOC,
             ((ttvfig_list *)chain->DATA)->NBDBLOC,
             ((ttvfig_list *)chain->DATA)->NBEBLOC,
             ((ttvfig_list *)chain->DATA)->NBFBLOC, 
             ((ttvfig_list *)chain->DATA)->NBESIG, 
             ((ttvfig_list *)chain->DATA)->NBISIG) ; 
    }
   else if(type == 'L')
    {
     if((((ttvsig_list *)chain->DATA)->TYPE & TTV_SIG_LL) == TTV_SIG_LL)
     fprintf(stdout,"%ld : %c %s\n",i,type,
             ttv_getsigname(GET_TTVFIG,name,(ttvsig_list *)chain->DATA)) ; 
     else
     fprintf(stdout,"%ld : F %s\n",i,
             ttv_getsigname(GET_TTVFIG,name,(ttvsig_list *)chain->DATA)) ; 
    }
   else
    {
     fprintf(stdout,"%ld : %c %s\n",i,type,
             ttv_getsigname(GET_TTVFIG,name,(ttvsig_list *)chain->DATA)) ; 
    }
   if((i >= j) && (chain->NEXT != NULL))
    {
     fprintf(stdout,"---return for more or s with return to stop---\n") ;
     fgets(c, sizeof(c), stdin) ;
     if((*c == 's') && (*(c+1) == '\0'))
      {
       fflush(stdout) ;
       if((ppath == 'Y') || (type != 'D'))
         return(1) ;
       else 
         return(0) ;
      }
     j += GET_MAXWINDOW ;
    }
   i++ ;
  }
 else 
  {
   fflush(stdout) ;
   return(0) ;
  }

 fflush(stdout) ;
 if((ppath == 'Y') || (type != 'D'))
   return(1) ;
 else 
   return(0) ;
}

int get_paramlist(buf,n)
char *buf ;
int n ; 
{
 chain_list *chain ;
 char name[2048] ;
 int eq ;
 int numb ;

 while(get_getarg(buf,name,n++) != NULL)
   {
    if((eq=retkey(name)) != -1)
      {
       name[eq] = '\0' ;
       if(strcmp(name,"in") == 0)
         {
          if((numb = atol(name + eq +1)) != (long)0)
           {
            switch(GET_REQUEST_LASTLIST)
             {
              case GET_LATCHLIST     : chain = GET_LATCH_LIST ;
                                       break ;
              case GET_CMDLIST       : chain = GET_CMD_LIST ;
                                       break ;
              case GET_PRECHARGELIST : chain = GET_PRECHARGE_LIST ;
                                       break ;
              case GET_BREAKLIST     : chain = GET_BREAK_LIST ;
                                       break ;
              case GET_CONLIST       : chain = GET_CON_LIST ;
                                       break ;
              case GET_SIGLIST       : chain = GET_SIG_LIST ;
                                       break ;
              default                : return(0) ; 
             }
            chain = (chain_list *)get_chainnumber(chain,numb) ;
            if(chain == NULL)
             return(0) ;
            ttv_getsigname(GET_TTVFIG,name,(ttvsig_list *)chain->DATA) ;
            if(TTV_NAME_IN != NULL)
               {
                freechain(TTV_NAME_IN) ;
                TTV_NAME_IN = NULL ;
               }
            TTV_NAME_IN=addchain(TTV_NAME_IN,(void *)namealloc(name));
           }
          else
           {
            if(TTV_NAME_IN != NULL)
               {
                freechain(TTV_NAME_IN) ;
                TTV_NAME_IN = NULL ;
               }
            TTV_NAME_IN=addchain(TTV_NAME_IN,
                                (void *)namealloc((char *)(name+eq+1)));
           }
         }
       else if(strcmp(name,"out") == 0)
         {
          if((numb = atol(name + eq +1)) != (long)0)
           {
            switch(GET_REQUEST_LASTLIST)
             {
              case GET_LATCHLIST     : chain = GET_LATCH_LIST ;
                                       break ;
              case GET_CMDLIST       : chain = GET_CMD_LIST ;
                                       break ;
              case GET_PRECHARGELIST : chain = GET_PRECHARGE_LIST ;
                                       break ;
              case GET_BREAKLIST     : chain = GET_BREAK_LIST ;
                                       break ;
              case GET_CONLIST       : chain = GET_CON_LIST ;
                                       break ;
              case GET_SIGLIST       : chain = GET_SIG_LIST ;
                                       break ;
              default                : return(0) ;
             }
            chain = (chain_list *)get_chainnumber(chain,numb) ;
            if(chain == NULL)
             return(0) ;
            ttv_getsigname(GET_TTVFIG,name,(ttvsig_list *)chain->DATA) ;
            if(TTV_NAME_OUT != NULL)
               {
                freechain(TTV_NAME_OUT) ;
                TTV_NAME_OUT = NULL ;
               }
            TTV_NAME_OUT=addchain(TTV_NAME_OUT,(void *)namealloc(name));
           }
          else
           {
            if(TTV_NAME_OUT != NULL)
               {
                freechain(TTV_NAME_OUT) ;
                TTV_NAME_OUT = NULL ;
               }
            TTV_NAME_OUT=addchain(TTV_NAME_OUT,
                                (void *)namealloc((char *)(name+eq+1)));
           }
         }
       else if(strcmp(name,"name") == 0)
         {
          char *ptx ;
          char *ptxx ;

          if(GET_NAMEMASK != NULL)
           {
            freechain(GET_NAMEMASK) ;
            GET_NAMEMASK = NULL ;
           }
          ptx = name+eq+1 ;
          while((ptxx = strchr(ptx,',')) != NULL)
           {
            *ptxx = '\0' ;
            GET_NAMEMASK=addchain(GET_NAMEMASK,(void *)namealloc(ptx));
            *ptxx = ',' ;
            ptx = ptxx + 1 ;
           }
          if(*ptx != '\0')
            GET_NAMEMASK=addchain(GET_NAMEMASK,(void *)namealloc(ptx));
         }
       else if(strcmp(name,"delaymax") == 0)
         {
          if((numb = atol(name + eq +1)) != (long)0)
            TTV_SEUIL_MAX = numb ;
          else 
            return(0) ;
         }
       else if(strcmp(name,"delaymin") == 0)
         {
          if((numb = atol(name + eq +1)) != (long)0)
            TTV_SEUIL_MIN = numb ;
          else 
            return(0) ;
         }
       else if(strcmp(name,"pathindex") == 0)
         {
          if((numb = atol(name + eq +1)) != (long)0)
            GET_PATH_NUM = numb  ;
          else 
            return(0) ;
         }
       else if(strcmp(name,"instance") == 0)
         {
          if((numb = atol(name + eq +1)) != (long)0)
            GET_TTVINS = numb  ;
          else
           {
            numb = 1 ;
            if(GET_FIG_LIST == NULL)
             GET_FIG_LIST = ttv_getttvfiglist(GET_TTVFIG) ;
            for(chain = GET_FIG_LIST ; chain != NULL ; chain = chain->NEXT)
              {
               if((ttvfig_list *)chain->DATA != GET_TTVFIG)
                 numb ++ ;
               if(strcmp(name+eq+1,((ttvfig_list *)chain->DATA)->INSNAME) == 0)
                 break ;
              }
            if(chain != NULL)
              GET_TTVINS = numb  ;
            else 
              return(0) ;
           }
         }
       else return(0) ;
      }
     else
      {
       if(strcmp(name,"noin") == 0)
        {
         if(TTV_NAME_IN != NULL)
           {
            freechain(TTV_NAME_IN) ;
            TTV_NAME_IN = NULL ;
           }
        }
       else if(strcmp(name,"noout") == 0)
        {
         if(TTV_NAME_OUT != NULL)
           {
            freechain(TTV_NAME_OUT) ;
            TTV_NAME_OUT = NULL ;
           }
        }
       else if(strcmp(name,"noname") == 0)
        {
         if(GET_NAMEMASK != NULL)
           {
            freechain(GET_NAMEMASK) ;
            GET_NAMEMASK = NULL ;
           }
         GET_TYPE &= ~(TTV_FIND_MASKOR|TTV_FIND_MASKAND|TTV_FIND_MASKNOT) ;
        }
       else if(strcmp(name,"nodelay") == 0)
        {
         TTV_SEUIL_MAX = TTV_DELAY_MAX ;
         TTV_SEUIL_MIN = TTV_DELAY_MIN ;
        }
       else if(strcmp(name,"noins") == 0)
        {
         GET_TTVINS = (long)0 ;
        }
       else if(strcmp(name,"frin") == 0)
        {
         GET_TYPE |= TTV_FIND_DUAL;
        }
       else if(strcmp(name,"ttx") == 0)
        {
         GET_TYPE |= TTV_FIND_PATH ;
         GET_TYPE &= ~(TTV_FIND_LINE) ;
        }
       else if(strcmp(name,"dtx") == 0)
        {
         GET_TYPE |= TTV_FIND_LINE ;
         GET_TYPE &= ~(TTV_FIND_PATH) ;
        }
       else if(strcmp(name,"nupup") == 0)
        {
         GET_TYPE |= TTV_FIND_NOT_UPUP ;
        }
       else if(strcmp(name,"ndwup") == 0)
        {
         GET_TYPE |= TTV_FIND_NOT_DWUP ;
        }
       else if(strcmp(name,"nupdw") == 0)
        {
         GET_TYPE |= TTV_FIND_NOT_UPDW ;
        }
       else if(strcmp(name,"ndwdw") == 0)
        {
         GET_TYPE |= TTV_FIND_NOT_DWDW ;
        }
       else if(strcmp(name,"upup") == 0)
        {
         GET_TYPE &= ~(TTV_FIND_NOT_UPUP) ;
        }
       else if(strcmp(name,"dwup") == 0)
        {
         GET_TYPE &= ~(TTV_FIND_NOT_DWUP) ;
        }
       else if(strcmp(name,"updw") == 0)
        {
         GET_TYPE &= ~(TTV_FIND_NOT_UPDW) ;
        }
       else if(strcmp(name,"dwdw") == 0)
        {
         GET_TYPE &= ~(TTV_FIND_NOT_DWDW) ;
        }
       else if(strcmp(name,"frout") == 0)
        {
         GET_TYPE &= ~(TTV_FIND_DUAL) ;
        }
       else if(strcmp(name,"min") == 0)
        {
         GET_TYPE |= TTV_FIND_MIN;
         GET_TYPE &= ~(TTV_FIND_MAX) ;
        }
     else if(strcmp(name,"and") == 0)
        {
         GET_TYPE |= TTV_FIND_MASKAND ;
         GET_TYPE &= ~(TTV_FIND_MASKOR|TTV_FIND_MASKNOT) ;
        }
      else if(strcmp(name,"or") == 0)
        {
         GET_TYPE |= TTV_FIND_MASKOR ;
         GET_TYPE &= ~(TTV_FIND_MASKAND|TTV_FIND_MASKNOT) ;
        }
      else if(strcmp(name,"not") == 0)
        {
         GET_TYPE |= TTV_FIND_MASKNOT ;
         GET_TYPE &= ~(TTV_FIND_MASKAND|TTV_FIND_MASKOR) ;
        }
      else if(strcmp(name,"max") == 0)
        {
         GET_TYPE |= TTV_FIND_MAX ;
         GET_TYPE &= ~(TTV_FIND_MIN) ;
        }
      else if(strcmp(name,"gate") == 0)
        {
         GET_TYPE |= TTV_FIND_GATE ;
         GET_TYPE &= ~(TTV_FIND_RC) ;
        }
      else if(strcmp(name,"rc") == 0)
        {
         GET_TYPE |= TTV_FIND_RC ;
         GET_TYPE &= ~(TTV_FIND_GATE) ;
        }
      else if((numb = atol(name)) != 0)
        { 
         if(numb <= 0) return(0) ;
         if(GET_REQUEST == GET_MAX) 
           GET_CPN = numb ;
         else
           GET_PATH_NUM = numb ;
        }
      else return(0) ;
      }
   }

 return(1) ;
}

void get_error(n)
int n ;
{
 fflush(stdout) ;
 switch(n)
   {
    case 0 :
    fprintf(stderr,"\nUsage : etas -x=<min>:<max> -w=<size> -lm=<size> -name=<\"name\">\n") ;
    fprintf(stderr,"        -in=<\"name\"> -out=<\"name\"> [filename]\n") ;
    EXIT(1) ;
    case 1 : fprintf(stderr,"syntax error\n") ;
             break ;
    case 2 : fprintf(stderr,"file not found\n") ;
             break ;
    case 3 : fprintf(stderr,"no figure loaded\n") ;
             break ;
    case 4 : fprintf(stderr,"nothing found\n") ;
             break ;
    case 5 : fprintf(stderr,"nothing to remove\n") ;
             break ;

   }
 fflush(stderr) ;
}

void get_handler()
{
 ttvfig_list *ttvfig ;

 for(ttvfig = TTV_LIST_TTVFIG ; ttvfig != NULL ; ttvfig = ttvfig->NEXT)
   if(ttvfig == GET_TTVFIG)
     break ;
 if(ttvfig == NULL)
   GET_TTVFIG = NULL ;
 ttv_fifoclean() ;
 ttv_cleantagttvfig(TTV_STS_FREE_MASK) ;
 siglongjmp(GET_ENV,1) ;
}

void get_handler_core()
{
 fprintf(stderr,"severity error check your files\n") ;
 EXIT(1) ;
}

void get_ttvinf(namefig)
char *namefig ;
{
 char *namex ;
 char *buf = NULL ;
 char name[2048] ;
 long numb ;
 int nextarg ;
 int valset = 0 ;
#ifdef AVERTEC_LICENSE
 int checkkey = 0 ;
#endif
 
 if(TTV_SEUIL_MAX == TTV_DELAY_MIN)
   TTV_SEUIL_MAX = TTV_DELAY_MAX ;
 if(TTV_SEUIL_MIN == TTV_DELAY_MAX)
   TTV_SEUIL_MIN = TTV_DELAY_MIN ;

 if(namefig != NULL)
   namex = namealloc(namefig) ;
 else
   namex = NULL ;

 signal(SIGTERM,get_handler) ;

 if((valset = sigsetjmp(GET_ENV,1)) != 0)
   signal(SIGTERM,get_handler) ;

if(ETAS_DEBUG_MODE == 'N')
 {
  signal(SIGBUS,get_handler_core) ;
  signal(SIGSEGV,get_handler_core) ;
 }


 if((namex != NULL) && (valset == 0))
  {
   if(GET_TTVFIG == NULL)
    {
     if((GET_TTVFIG = ttv_getttvfig(namefig,(long)TTV_FILE_INF)) == NULL)
       get_error(2) ;
    }
   else
    {
     if(namex != GET_TTVFIG->INFO->FIGNAME)
      {
       if((GET_TTVFIG = ttv_getttvfig(namefig,(long)TTV_FILE_INF)) == NULL)
         get_error(2) ;
      }
    }
  }

 if(valset == 0)
   {
    fprintf(stdout,"\n") ;
   }

 for(;;)
  {
#ifdef AVERTEC_LICENSE
   checkkey++ ;
   if(checkkey == 10)
    {
     if(avt_givetoken("HITAS_LICENSE_SERVER","etas") != AVT_VALID_TOKEN )
      EXIT(1) ;
     checkkey = 0 ;
    }
#endif
   nextarg = 1 ;
   if(buf != NULL)
     mbkfree(buf) ;
   if(GET_BATCH)
     {
      init(1) ;
      buf = mbkalloc(1024) ;
      while(1)
       {
        if(fgets(buf,1024,GET_FILE) == NULL)
         {
          strcpy(buf,"quit") ;
          break ;
         }
        namex = buf ;

        while((*namex != '\n') && (*namex != '\0'))
         { 
          namex++ ;
         }

        if(*namex == '\n')
           *namex = '\0' ;

        if(*buf != '#')
          break ;
       }
     } 
   else
      buf = readline("etas> ") ;
   if((buf != NULL) && (strcmp(buf,"") != 0)) 
    {
     if(GET_BATCH == 0)
       add_history (strdup(buf)) ;
    }
   else
    continue ;
   get_getarg(buf,name,1) ;
   if((strcmp(name,"quit") == 0) || (strcmp(name,"q") == 0))
    {
     if(get_getarg(buf,name,2) == NULL)
      {
       break ;
      }
     else
       get_error(1) ;
    }
   else
    {
     if((strcmp(name,"list") == 0) || (strcmp(name,"l") == 0))
      {
       if(GET_TTVFIG == NULL)
          {
           get_error(3) ;
           continue ;
          }
       if(get_getarg(buf,name,2) != NULL)
        {
         if((strcmp(name,"latch") == 0) || (strcmp(name,"l") == 0))
          GET_REQUEST = GET_LATCHLIST ;
         else if((strcmp(name,"precharge") == 0) || (strcmp(name,"p") == 0))
          GET_REQUEST = GET_PRECHARGELIST ;
         else if((strcmp(name,"precharge") == 0) || (strcmp(name,"b") == 0))
          GET_REQUEST = GET_BREAKLIST ;
         else if((strcmp(name,"command") == 0) || (strcmp(name,"q") == 0))
          GET_REQUEST = GET_CMDLIST ;
         else if((strcmp(name,"connector") == 0) || (strcmp(name,"c") == 0))
          GET_REQUEST = GET_CONLIST ;
         else if((strcmp(name,"signal") == 0) || (strcmp(name,"s") == 0))
          GET_REQUEST = GET_SIGLIST ;
         else if((strcmp(name,"instance") == 0) || (strcmp(name,"i") == 0))
          GET_REQUEST = GET_FIGLIST ;
         else if((strcmp(name,"delay") == 0) || (strcmp(name,"d") == 0))
          GET_REQUEST = GET_DELAYLIST ;
         else
          {
           get_error(1) ;
           continue ;
          }
         nextarg = 3 ;
         GET_REQUEST_LASTLIST = GET_REQUEST ;
        }
       else 
        {
         GET_REQUEST = GET_REQUEST_LASTLIST ;
         if(get_printlist() == 0)
           get_error(4) ;
         continue ;
        }
      }
     else if((strcmp(name,"path") == 0) || (strcmp(name,"p") == 0))
      {
       if(GET_TTVFIG == NULL)
          {
           get_error(3) ;
           continue ;
          }
       if(get_getarg(buf,name,2) != NULL)
        {
         if(GET_REQUEST_LASTPATH != GET_PARA)
           GET_REQUEST_PREVPATH = GET_REQUEST_LASTPATH ;
         if((strcmp(name,"critic") == 0) || (strcmp(name,"c") == 0))
          {
           GET_CPN = 1 ;
           GET_REQUEST = GET_MAX ;
          }
         else if((strcmp(name,"parallel") == 0) || (strcmp(name,"p") == 0))
          GET_REQUEST = GET_PARA ;
         else if((strcmp(name,"delay") == 0) || (strcmp(name,"d") == 0))
          GET_REQUEST = GET_PATH ;
         else
          {
           get_error(1) ;
           continue ;
          }
         nextarg = 3 ;
         GET_REQUEST_LASTPATH = GET_REQUEST ;
        }
       else 
        {
         GET_REQUEST = GET_REQUEST_LASTPATH ;
         if(get_printlist() == 0)
           get_error(4) ;
         continue ;
        }
      }
     else if((strcmp(name,"detail") == 0) || (strcmp(name,"d") == 0))
      {
       if(GET_TTVFIG == NULL)
          {
           get_error(3) ;
           continue ;
          }
       if(get_getarg(buf,name,2) != NULL)
        {
         if((strcmp(name,"critic") == 0) || (strcmp(name,"c") == 0))
           GET_REQUEST = GET_CRITICMAX ;
         else if((strcmp(name,"parallel") == 0) || (strcmp(name,"p") == 0))
          GET_REQUEST = GET_CRITICPARA ;
         else if((strcmp(name,"delay") == 0) || (strcmp(name,"d") == 0))
          GET_REQUEST = GET_CRITICPATH ;
         else
          {
           get_error(1) ;
           continue ;
          }
         nextarg = 3 ;
         GET_REQUEST_LASTDETAIL = GET_REQUEST ;
        }
       else 
        {
         GET_REQUEST = GET_REQUEST_LASTDETAIL ;
         if(get_printlist() == 0)
           get_error(4) ;
         continue ;
        }
      }
     else if((strcmp(name,"open") == 0) || (strcmp(name,"o") == 0))
      {
       if(get_getarg(buf,name,3) != NULL) 
           get_error(1) ;
       if(get_getarg(buf,name,2) != NULL) 
        {
         ttvfig_list *ttvfigx = GET_TTVFIG ;

         namefig = namealloc(name) ;
         if(GET_TTVFIG != NULL)
          {
           if(namefig != GET_TTVFIG->INFO->FIGNAME)
            {
             if((GET_TTVFIG = ttv_getttvfig(namefig,(long)TTV_FILE_INF)) == NULL)
              {
               GET_TTVFIG = ttvfigx ;
               get_error(2) ;
              }
             else
              {
               init(0) ;
              }
            }
          }
         else
          {
           if((GET_TTVFIG = ttv_getttvfig(namefig,(long)TTV_FILE_INF)) == NULL)
            get_error(2) ;
          }
        }
       else
         get_error(1) ;
       continue ;
      }
     else if((strcmp(name,"remove") == 0) || (strcmp(name,"r") == 0))
      {
       if(get_getarg(buf,name,2) != NULL)
         {
           get_error(1) ;
         }
       else
          {
           if(GET_TTVFIG != NULL)
            {
             ttv_freeallttvfig(GET_TTVFIG) ;
             GET_TTVFIG = NULL ;
             init(0) ;
            }
           else
             get_error(5) ;
          }
       continue ;
      }
     else if((strcmp(name,"cache") == 0) || (strcmp(name,"c") == 0))
      {
       double c1 = (double)0.0 ;
       double c2 = (double)0.0 ;

       if(get_getarg(buf,name,4) != NULL) 
           get_error(1) ;
       if(get_getarg(buf,name,3) != NULL) 
          {
           c1 = atof(name) ;
           if(get_getarg(buf,name,2) != NULL) 
             c2 = atof(name) ;
           else 
             get_error(1) ;
           if((c1 <= (double)0.0) || (c2 <= (long)0.0))
             get_error(1) ;
           else
           ttv_setcachesize(c2,c1) ;
          }
       else if(get_getarg(buf,name,2) != NULL) 
        {
         c1 = atof(name) ;
         
         if(c1 <= (double)0.0)
           get_error(1) ;
         else
          {
           ttv_setcachesize((double)(c1 * (double)0.1),
                            (double)(c1 * (double)0.9)) ;
          }
        }
       else
        {
         double cl = ttv_getlinecachesize() ;
         double cs = ttv_getsigcachesize() ;
         fprintf(stdout,"signal cache size in megabyte : %.2f MB\n",cs) ;
         fprintf(stdout,"path cache size in megabyte : %.2f MB\n",cl) ;
        }
       continue ;
      }
     else if((strcmp(name,"window") == 0) || (strcmp(name,"w") == 0))
      {
       if(get_getarg(buf,name,3) != NULL) 
           get_error(1) ;
       if(get_getarg(buf,name,2) != NULL) 
        {
         long w = atol(name) ;
         
         if(w != 0)
           GET_MAXWINDOW = w ;
         else
           get_error(1) ;
        }
       else 
         fprintf(stdout,"window size : %ld\n",GET_MAXWINDOW) ;
       continue ;
      }
     else if((strcmp(name,"memory") == 0) || (strcmp(name,"m") == 0))
      {
       if(get_getarg(buf,name,2) != NULL)
           get_error(1) ;
       else
        {
         double sl = ttv_getlinememorysize() ;
         double ss = ttv_getsigmemorysize() ;
         fprintf(stdout,"path size in megabyte : %.2f MB\n",sl) ;
         fprintf(stdout,"signal size in megabyte : %.2f MB\n",ss) ;
        }
       continue ;
      }
     else if((strcmp(name,"status") == 0) || (strcmp(name,"s") == 0))
      {
       ttvfig_list *ttvfigx ;
       ttvpath_list *path ; 
       chain_list *chainx ;
 
       if(GET_TTVINS != (long)0)
         {
          if(GET_TTVINS != (long)1)
           {
            chainx = (chain_list *)get_chainnumber((chain_list *)GET_FIG_LIST,
                                                  GET_TTVINS) ;
            if(chainx != NULL)
             ttvfigx = (ttvfig_list *)chainx->DATA ;
            else 
              ttvfigx = NULL ;
           }
          else
           ttvfigx = GET_TTVFIG ;
         }
       else
         ttvfigx = NULL ;

       if((GET_PATH_NUM != (long)0) && (GET_PATH_LIST != NULL))
         {
          path = (ttvpath_list *)get_chainnumber((chain_list *)GET_PATH_LIST,
                                                  GET_PATH_NUM) ;
         }
       else
         path = NULL ;

       if(GET_TTVFIG != NULL)
         fprintf(stdout,"current figure : %s\n",GET_TTVFIG->INFO->FIGNAME) ;
       else
         fprintf(stdout,"no figure loaded\n") ;

       if((ttvfigx != NULL) && (ttvfigx != GET_TTVFIG))
         fprintf(stdout,"instance for path search : %s %s\n",
                 ttvfigx->INFO->FIGNAME,ttvfigx->INSNAME) ;
       else if((ttvfigx == GET_TTVFIG) && (ttvfigx != NULL))
         fprintf(stdout,"figure for path search : %s\n",
                 ttvfigx->INFO->FIGNAME) ;
       else 
         fprintf(stdout,"no instance specified\n") ;

       if(GET_TYPE != (long)0)
         fprintf(stdout,"type of search : ") ;
       if((GET_TYPE & TTV_FIND_MAX) == TTV_FIND_MAX)
         fprintf(stdout,"max ") ;
       else if((GET_TYPE & TTV_FIND_MIN) == TTV_FIND_MIN)
         fprintf(stdout,"min ") ;
       if((GET_TYPE & TTV_FIND_DUAL) == TTV_FIND_DUAL)
         fprintf(stdout,"from in\n") ;
       else
         fprintf(stdout,"from out\n") ;
       if(GET_NAMEMASK != NULL) 
        {
         fprintf(stdout,"mask for the signals list : ") ;
         for(chainx= GET_NAMEMASK ; chainx != NULL ; chainx = chainx->NEXT)
          {
           fprintf(stdout,"%s",(char *)chainx->DATA) ;
           if(chainx->NEXT == NULL) break ;
           fprintf(stdout,",") ;
          }
           fprintf(stdout,"\n") ;
        }
       if(TTV_NAME_IN != NULL) 
         fprintf(stdout,"mask for the input signals : %s\n",
                 (char *)TTV_NAME_IN->DATA) ;
       if(TTV_NAME_OUT != NULL) 
         fprintf(stdout,"mask for the output signals : %s\n",
                 (char *)TTV_NAME_OUT->DATA) ;
       if(TTV_SEUIL_MAX != TTV_DELAY_MAX)
         fprintf(stdout,"max delay for path search : %ld\n",TTV_SEUIL_MAX) ;
       if(TTV_SEUIL_MIN != TTV_DELAY_MIN)
         fprintf(stdout,"min delay for path search : %ld\n",TTV_SEUIL_MIN) ;
       if(path != NULL)
        {
         fprintf(stdout,"current path : %s ",
         ttv_getsigname(GET_TTVFIG,name,path->NODE->ROOT)) ;
         if((path->TYPE & TTV_FIND_HZ) == TTV_FIND_HZ)
           fprintf(stdout,"%s %cZ %s TP = %.1f ps SLOPE = %.1f ps\n",
           ttv_getsigname(GET_TTVFIG,name,path->ROOT->ROOT),
           GET_NODELEVEL(path->NODE),
           (((path->TYPE & TTV_FIND_MAX) == TTV_FIND_MAX)? "MAX" : "MIN"),
           path->DELAY/TTV_UNIT,path->SLOPE/TTV_UNIT) ;
         else
           fprintf(stdout,"%s %c%c %s TP = %.1f ps SLOPE = %.1f ps\n",
           ttv_getsigname(GET_TTVFIG,name,path->ROOT->ROOT),
           GET_NODELEVEL(path->NODE),GET_NODELEVEL(path->ROOT),
           (((path->TYPE & TTV_FIND_MAX) == TTV_FIND_MAX)? "MAX" : "MIN"),
           path->DELAY/TTV_UNIT,path->SLOPE/TTV_UNIT) ;
        }
 
       if(GET_NAMEMASK != NULL)
        {
         if((GET_TYPE & TTV_FIND_MASKOR) == TTV_FIND_MASKOR) 
           fprintf(stdout,"condition for parallele paths : or\n") ;
         else if((GET_TYPE & TTV_FIND_MASKAND) == TTV_FIND_MASKAND) 
           fprintf(stdout,"condition for parallele paths : and\n") ;
         else if((GET_TYPE & TTV_FIND_MASKNOT) == TTV_FIND_MASKNOT) 
           fprintf(stdout,"condition for parallele paths : not\n") ;
        }

       if((GET_TYPE & TTV_FIND_LINE) == TTV_FIND_LINE) 
         fprintf(stdout,"search path from dtx file\n") ;
       else if((GET_TYPE & TTV_FIND_PATH) == TTV_FIND_PATH) 
         fprintf(stdout,"search path from ttx file\n") ;

       fflush(stdout) ;
       continue ;
      }
     else if((strcmp(name,"help") == 0) || (strcmp(name,"h") == 0))
      {
       if(get_getarg(buf,name,2) == NULL)
        {
         fprintf(stdout,"option   -> : etas options\n") ;
         fprintf(stdout,"command  -> : etas commands\n") ;
         fprintf(stdout,"argument -> : etas arguments\n") ;
        }
       else
        {
         if(strcmp(name,"option") == 0) 
          {
           if(get_getarg(buf,name,3) == NULL)
            {
             fprintf(stdout,"-x    : delay max and delay min\n") ;
             fprintf(stdout,"-w    : number element to print\n") ;
             fprintf(stdout,"-lm   : cache size on MB\n") ;
             fprintf(stdout,"-in   : mask for the inputs\n") ;
             fprintf(stdout,"-out  : mask for the ouputs\n") ;
             fprintf(stdout,"-name : mask for signal name\n") ;
            }
           else
            {
             get_error(4) ;
            }
          }
         else if(strcmp(name,"command") == 0)
          {
           if(get_getarg(buf,name,3) == NULL)
            {
             fprintf(stdout,"list(l)   -> : list of signals in the circuit\n") ;
             fprintf(stdout,"path(p)   -> : path search \n") ;
             fprintf(stdout,"detail(d) -> : detailed delay search\n") ;
             fprintf(stdout,"open(o)      : open a circuit\n") ;
             fprintf(stdout,"remove(r)    : remove a circuit\n") ;
             fprintf(stdout,"window(w)    : number of element to print\n") ;
             fprintf(stdout,"cache(c)     : path and signal cache size\n") ;
             fprintf(stdout,"memory(m)    : memory used for path and signal\n") ;
             fprintf(stdout,"status(s)    : state of the arguments\n") ;
             fprintf(stdout,"help(h)      : help\n") ;
             fprintf(stdout,"quit(q)      : quit\n") ;
             fprintf(stdout,"<return>     : last command\n") ;
             fprintf(stdout,"<integer>   : path index\n") ;
            }
           else if(strcmp(name,"list") == 0)
            {
             if(get_getarg(buf,name,4) == NULL)
              {
               fprintf(stdout,"instance(i)  : instance list\n") ;
               fprintf(stdout,"command(q)   : command list\n") ;
               fprintf(stdout,"connector(c) : connector list\n") ;
               fprintf(stdout,"latch(l)     : latch list\n") ;
               fprintf(stdout,"precharge(p) : precharge list\n") ;
               fprintf(stdout,"break(b)     : break list\n") ;
               fprintf(stdout,"signal(s)    : other signal list\n") ;
               fprintf(stdout,"delay(d)     : delay list\n") ;
              }
             else
              {
               get_error(4) ;
              }
            }
           else if(strcmp(name,"path") == 0)
            {
             if(get_getarg(buf,name,4) == NULL)
              {
               fprintf(stdout,"delay(d)    : all the critical paths\n") ;
               fprintf(stdout,"critic(c)   : the max or min path\n") ;
               fprintf(stdout,"parallel(p) : parallel paths of a path\n") ;
              }
             else
              {
               get_error(4) ;
              }
            }
           else if(strcmp(name,"detail") == 0)
            {
             if(get_getarg(buf,name,4) == NULL)
              {
               fprintf(stdout,"delay(d)    : detailed delay of a path\n") ;
               fprintf(stdout,"critic(c)   : detailed delay of the max or min path\n") ;
               fprintf(stdout,"parallel(p) : detailed of a parallel path\n") ;
              }
             else
              {
               get_error(4) ;
              }
            }
           else
            {
             get_error(4) ;
            }
          }
         else if(strcmp(name,"argument") == 0)
          {
           if(get_getarg(buf,name,3) == NULL)
            {
             fprintf(stdout,"in        : input name or index on the list\n") ;
             fprintf(stdout,"noin      : all the inputs\n") ;
             fprintf(stdout,"out       : output name or index on the list\n") ;
             fprintf(stdout,"noout     : all the outputs\n") ;
             fprintf(stdout,"name      : list of mask for signal list and parallel path\n") ;
             fprintf(stdout,"noname    : no mask for signal list and parallel path\n") ;
             fprintf(stdout,"instance  : instance name or index on the list\n") ;
             fprintf(stdout,"noins     : no instance for path or list\n") ;
             fprintf(stdout,"delaymax  : max delay for path search\n") ;
             fprintf(stdout,"delaymin  : min delay for path search\n") ;
             fprintf(stdout,"nodelay   : no max or min delay\n") ;
             fprintf(stdout,"frin      : search from the input\n") ;
             fprintf(stdout,"frout     : search from the output\n") ;
             fprintf(stdout,"max       : max delay search\n") ;
             fprintf(stdout,"min       : min delay search\n") ;
             fprintf(stdout,"dtx       : search from the dtx file\n") ;
             fprintf(stdout,"ttx       : search from the ttx file\n") ;
             fprintf(stdout,"or        : or between mask list for parallel path\n") ;
             fprintf(stdout,"and       : and between mask list for parallel path\n") ;
             fprintf(stdout,"not       : not cross mask list for parallel path\n") ;
             fprintf(stdout,"upup      : UP UP paths for path search\n") ;
             fprintf(stdout,"dwup      : DOWN UP paths for path search\n") ;
             fprintf(stdout,"updw      : UP DOWN paths for path search\n") ;
             fprintf(stdout,"dwdw      : DOWN DOWN paths for path search\n") ;
             fprintf(stdout,"nupup     : not UP UP paths for path search\n") ;
             fprintf(stdout,"ndwup     : not DOWN UP paths for path search\n") ;
             fprintf(stdout,"nupdw     : not UP DOWN paths for path search\n") ;
             fprintf(stdout,"ndwdw     : not DOWN DOWN paths for path search\n") ;
             fprintf(stdout,"rc        : rc delay list\n") ;
             fprintf(stdout,"gate      : gate delay list\n") ;
             fprintf(stdout,"pathindex : path index in the list for path search\n") ;
            }
           else
            {
             get_error(4) ;
            }
          }
         else
          {
           get_error(4) ;
          }
        }
       fflush(stdout) ;
       continue ;
      }
     else if(strcmp(name,"") == 0)
      {
       if(get_printlist() == 0)
         get_error(4) ;
       continue ;
      }
     else if((numb = atol(name)) != 0)
      {
       GET_PATH_NUM = numb ;
       continue ;
      }
      
     if(get_paramlist(buf,nextarg) == 0)
       get_error(1) ;
     else if(get_builtlist() == 0)
       get_error(4) ;
     else if(get_printlist() == 0)
       get_error(4) ;
    }
  }
}

int main(argc,argv)
int argc;
char *argv[];
{
 char *namefig = NULL ;
 char *str ;
 double limit = (double)100.0 ;
 long win ;
 int eq ;
 int i ;

#ifdef AVERTEC
 avtenv() ;
#endif

 mbkenv() ;

str = getenv("ETAS_DEBUG_MODE") ;
if(str != NULL)
 {
  ETAS_DEBUG_MODE = (strcmp(str,"yes") == 0 ) ? 'Y' : 'N' ;
 }


 ttv_setcachesize((double)limit / (double)10.0, 
                  ((double)limit * (double)9.0)/(double)10.0) ;

 for(i = 1 ; i != argc ; i++)
  {
   if(argv[i][0] != '-')
      {
       namefig = namealloc(argv[i]) ;
      }
   else if((eq=retkey(argv[i])) != -1)
      {
       argv[i][eq]='\0';
       if(strcmp(argv[i],"-w") == 0)
         {
          if((win = atol(argv[i]+eq+1)) != 0)
            GET_MAXWINDOW = win ;
         }
       else if(strcmp(argv[i],"-name") == 0)
         {
          char *ptx ;
          char *ptxx ;

          ptx = argv[i]+eq+1 ;
          while((ptxx = strchr(ptx,',')) != NULL)
           {
            *ptxx = '\0' ;
            GET_NAMEMASK=addchain(GET_NAMEMASK,(void *)namealloc(ptx));
            *ptxx = ',' ;
            ptx = ptxx + 1 ;
           }
          if(*ptx != '\0')
            GET_NAMEMASK=addchain(GET_NAMEMASK,(void *)namealloc(ptx));
         }
       else if(strcmp(argv[i],"-in") == 0)
         {
          TTV_NAME_IN=addchain(TTV_NAME_IN,
                              (void *)namealloc((char *)(argv[i]+eq+1)));
         }
       else if(strcmp(argv[i],"-out") == 0)
         {
          TTV_NAME_OUT=addchain(TTV_NAME_OUT,
                              (void *)namealloc((char *)(argv[i]+eq+1)));
         }
       else if(strcmp(argv[i],"-lm") == 0)
         {
          if((limit = atof(argv[i]+eq+1)) <= (double)0.0)
            get_error(0) ;
          ttv_setcachesize((double)(limit * (double)0.1),
                           (double)(limit * (double)0.9)) ;
         }
       else if(strcmp(argv[i],"-x") == 0)
         {
          char pathopt[48] ;
          int j ;
 
          strcpy(pathopt,argv[i] + 3) ;

          for(j = 0 ; (j != (signed)strlen(pathopt)) && (pathopt[j] != ':') ; j++) ;
          if(j == (signed)strlen(pathopt))
           {
            TTV_SEUIL_MIN = atol(pathopt) ;
            TTV_SEUIL_MAX = TTV_DELAY_MAX ;
        
            if((TTV_SEUIL_MIN == 0) && (strcmp(pathopt,"0") != 0) &&
               (strcmp(pathopt,"00") != 0))
              get_error(0) ;
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
                  get_error(0) ;
               }
            else TTV_SEUIL_MIN = TTV_DELAY_MIN ;
        
            aux += j+1 ;
        
            if(aux[0] != '\0')
              {
               TTV_SEUIL_MAX = atol(aux) ;
        
               if((TTV_SEUIL_MAX == 0) && (strcmp(aux,"0") != 0) &&
                  (strcmp(aux,"00") != 0))
                 get_error(0) ;
              }
            else TTV_SEUIL_MAX = TTV_DELAY_MAX ;
           }
         }
       else 
         {
          argv[i][eq]='=';
          get_error(0) ;
         }
       argv[i][eq]='=';
      }
   else
      {
       if(strcmp(argv[i],"-x") == 0)
         {
          TTV_SEUIL_MAX = TTV_DELAY_MAX ;
          TTV_SEUIL_MIN = TTV_DELAY_MIN ;
         }
       if(strcmp(argv[i],"-b") == 0)
         {
          GET_BATCH = 1 ;
          GET_MAXWINDOW = 10000000 ;
         }
       else get_error(0) ;
      }
  }

#ifdef AVERTEC_LICENSE
 if(avt_givetoken("HITAS_LICENSE_SERVER","etas") != AVT_VALID_TOKEN)
  EXIT(1) ;
#endif

 if(GET_BATCH)
  {
   GET_BATCH = 1;
   GET_FILE = fopen(namefig,"r") ;
   if(GET_FILE == NULL)
    {
     fprintf(stderr,"can not open file %s\n",namefig) ;
     fflush(stderr) ;
     EXIT(1) ;
    }
   namefig = NULL ;
  }

 if( !GET_BATCH ) {
 avt_banner("etAs","Timing files browser","1998");
 }

 get_ttvinf(namefig) ;

 EXIT(0) ;
}

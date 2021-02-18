/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TTV Verison 1                                               */
/*    Fichier : ttv_drittv.c                                                */
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

long TTV_SEUIL_MAX = TTV_DELAY_MIN ;
long TTV_SEUIL_MIN = TTV_DELAY_MAX ;
long TTV_INFO_CAPARM = (long)0 ;
long TTV_INFO_CAPASW = (long)0 ;
char *TTV_INFO_TOOLDATE = NULL ;
char **TTV_INFO_ARGV = NULL ;
short TTV_INFO_ARGC = 0 ;
char TTV_PATH_SAVE = TTV_NOTSAVE_PATH ;


/*****************************************************************************/
/*                        function ttv_printcaracsig()                       */
/* parametres :                                                              */
/* file : fichier                                                            */
/* ptsig : signal                                                            */
/*                                                                           */
/* ecrit dans le fichier les caracteristiques d'un signal                    */
/*****************************************************************************/
void ttv_printcaracsig(ttvfig,file,ptsig)
ttvfig_list *ttvfig ;
FILE *file ;
ttvsig_list *ptsig ;
{
 char buf[1024] ;
 
 fprintf(file,"%s %.2f",ttv_getnetname(ttvfig,buf,ptsig),ptsig->CAPA) ;
}

void ttv_printsigsettings(ttvfig_list *ttvfig, FILE *file, ttvsig_list *ptsig, long type)
{
  long sup=0, sdn=0 ;
  float low, high;
  if ((ptsig->TYPE & TTV_SIG_CI)==TTV_SIG_CI || (ptsig->TYPE & TTV_SIG_CT)==TTV_SIG_CT
      || ((ptsig->TYPE & TTV_SIG_B)==TTV_SIG_B && ptsig->NODE[0].INLINE==NULL && ptsig->NODE[1].INLINE==NULL))
  {
    if ((sdn = ttv_getnodeslew(ptsig->NODE,TTV_FIND_MAX))==TTV_NOSLOPE) sdn=0;
    if ((sup = ttv_getnodeslew(ptsig->NODE+1,TTV_FIND_MAX))==TTV_NOSLOPE) sup=0;
  }
  fprintf(file,"%.1f %.1f %.2f", sdn/TTV_UNIT, sup/TTV_UNIT, ttv_get_signal_output_capacitance(ttvfig, ptsig)) ;
  if (ttv_get_signal_swing(ttvfig, ptsig, &low, &high))
    fprintf(file, " . .");
  else
    fprintf(file, " %g %g", low, high);
}

void ttv_printsigproperty(ttvfig_list *ttvfig, FILE *file, ttvsig_list *tvs)
{
  long val;

  if (ttv_testsigflag(tvs, TTV_SIG_FLAGS_ISMASTER))
    fprintf(file," M");
  else if (ttv_testsigflag(tvs, TTV_SIG_FLAGS_ISSLAVE))
    fprintf(file," S");
  else 
    fprintf(file," .");

  if ((val=ttv_testsigflag(tvs, 0xff000000))!=0)
    fprintf(file," %ld", (((unsigned long)val)>>24));
  else
    fprintf(file," .");

  if (tvs->PNODE[0]!=-1 && tvs->PNODE[1]!=-1)
  {
    fprintf(file," '%d '%d",tvs->PNODE[0],tvs->PNODE[1]);
  }
}

/*****************************************************************************/
/*                        function ttv_printname()                           */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* file : fichier                                                            */
/* ptsig : signal                                                            */
/*                                                                           */
/* ecrit dans le fichier le nom hierarchique d'un signal                     */
/*****************************************************************************/
void ttv_printname(ttvfig,file,ptsig)
ttvfig_list *ttvfig ;
FILE *file ;
ttvsig_list *ptsig ;
{
 char bufname[1024] ;

 ttv_getsigname(ttvfig,bufname,ptsig) ;
 fprintf(file,"%s",bufname) ;
}

/*****************************************************************************/
/*                        function ttv_getdir()                              */
/* parametres :                                                              */
/* ptsig : signal                                                            */
/*                                                                           */
/* donne le carractere correspondant a la direction d'un connecteur          */
/*****************************************************************************/
char ttv_getdir(ptsig)
ttvsig_list *ptsig ;

{
 long type ;
 
 type = ptsig->TYPE & TTV_SIG_TYPECON ;
 switch(type)
    {
     case TTV_SIG_CI :  return('I') ;
     case TTV_SIG_CO :  return('O') ;
     case TTV_SIG_CZ :  return('Z') ;
     case TTV_SIG_CB :  return('B') ;
     case TTV_SIG_CT :  return('T') ;
     case TTV_SIG_CX :  return('X') ;
    }

  return('X');
}

/*****************************************************************************/
/*                        function ttv_getsigtype()                          */
/* parametres :                                                              */
/* ptsig : signal                                                            */
/* ptline : lien                                                             */
/*                                                                           */
/* donne le carractere correspondant a une extremite d'un lien               */
/*****************************************************************************/
char ttv_getsigtype(ptsig,ptline,sig)
ttvsig_list *ptsig ;
ttvline_list *ptline ;
int sig ;
{
 if((ptsig->TYPE & TTV_SIG_R) == TTV_SIG_R)
  {
   if((ptline->TYPE & TTV_LINE_PR) == TTV_LINE_PR) return('P') ;
   else if((ptline->TYPE & TTV_LINE_EV) == TTV_LINE_EV) return('E') ;
   else return('R') ;
  }
 if((((ptsig->TYPE & TTV_SIG_LR) == TTV_SIG_LR) ||
    ((ptsig->TYPE & TTV_SIG_LS) == TTV_SIG_LS)) && (sig == 2))
  {
   if((ptline->TYPE & TTV_LINE_R) == TTV_LINE_R)
     return('Z') ;
   else if((ptline->TYPE & TTV_LINE_S) == TTV_LINE_S)
    return('O') ;
  }
 if((ptsig->TYPE & TTV_SIG_L) == TTV_SIG_L) return('L') ;
 if((ptsig->TYPE & TTV_SIG_B) == TTV_SIG_B) return('B') ;
 if((ptsig->TYPE & TTV_SIG_C) == TTV_SIG_C) return('C') ;
 if((ptsig->TYPE & TTV_SIG_N) == TTV_SIG_N) return('N') ;
 if((ptsig->TYPE & TTV_SIG_Q) == TTV_SIG_Q) return('Q') ;
 if((ptsig->TYPE & TTV_SIG_I) == TTV_SIG_I) return('I') ;
 if((ptsig->TYPE & TTV_SIG_S) == TTV_SIG_S) return('S') ;

 return(0);
}

/*****************************************************************************/
/*                        function ttv_printcritic()                         */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* file : fichier                                                            */
/* ptline : lien                                                             */
/* type : type                                                               */
/*                                                                           */
/* ecrit dans le fichier le detail du chemin critic de type max ou min       */
/*****************************************************************************/
void ttv_printcritic(ttvfig,file,root,node,type)
ttvfig_list *ttvfig ;
FILE *file ;
ttvevent_list *root ;
ttvevent_list *node ;
long type ;
{
 ttvcritic_list *headcritic ;
 ttvcritic_list *critic ;
 ttvcritic_list *auxcritic ;

 if((type & TTV_FIND_NOTSAVE) == TTV_FIND_NOTSAVE) return ;

 type |= TTV_FIND_LINE ;
 type &= ~(TTV_FIND_PATH) ;

 headcritic = critic = ttv_getcritic(ttvfig,NULL,root,node,NULL,NULL,type) ;
 if(critic == NULL)
  return ;
 
 for(; critic->NEXT != NULL ; critic = critic->NEXT)
   {
    auxcritic = critic->NEXT ;
    fprintf(file,"\n#\t ") ;
    if(critic->SNODE == TTV_UP)
      fprintf(file,"UP(") ;
    else fprintf(file,"DOWN(") ;
    fprintf(file,"%s",critic->NAME) ;
    fprintf(file,") => ") ; 
    if(((type & TTV_FIND_HZ) == TTV_FIND_HZ) &&
       (auxcritic->NEXT == NULL))
      fprintf(file,"HighZ(") ;
    else if(auxcritic->SNODE == TTV_UP)
      fprintf(file,"UP(") ;
    else fprintf(file,"DOWN(") ;
    fprintf(file,"%s",auxcritic->NAME) ;
    fprintf(file,"), Tp=%g pS",(double)auxcritic->DELAY/TTV_UNIT) ;
   }
 ttv_freecriticlist(headcritic) ;
}

/*****************************************************************************/
/*                        function ttv_drvline()                             */
/* parametres :                                                              */
/* cmdindex : index de la commande                                           */
/* fcmd : front de commande                                                  */
/* fin : front de l'entree                                                   */
/* fout : front de la sortie                                                 */ 
/* typel : type de transistion                                               */
/* delay : delay                                                             */ 
/* slope : front                                                             */ 
/* mdelay : model de delay                                                   */
/* mslope : model de front                                                   */
/*                                                                           */
/* drive une transistion                                                     */
/*****************************************************************************/
void ttv_drvline(file,cmdindex,fcmd,fin,fout,typel,delay,slope,mdelay,mslope)
FILE *file ;
long cmdindex ;
char fcmd ;
char fin ;
char fout ;
char *typel ;
long delay ;
long slope ;
char *mdelay ;
char *mslope ;
{
 fprintf(file,"\n\t(");
 if(cmdindex != 0)
   fprintf(file,"%ld %c",cmdindex,fcmd);
 fprintf(file,"(%c%c %s %g %g",fin,fout,typel,(double)delay/TTV_UNIT,(double)slope/TTV_UNIT);
 if(mdelay != NULL)
   fprintf(file," MOD %s",mdelay);
 if(mslope != NULL)
   fprintf(file," %s",mslope);
 fprintf(file,"))") ;
}

/*****************************************************************************/
/*                        function ttv_printline()                           */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* ttvfigr : ttvfig de reference                                             */
/* file : fichier                                                            */
/* typeline : type de lien                                                   */
/* type : type de fichier ttv dtv ttx ttx                                    */
/* find : type de recherche hierarchique ou pas                              */
/*                                                                           */
/* ecrit dans le fichier l'ensemble d'un type de lien typeline               */
/* TTV_STS_DENOTINPT n'est pas pris en compte typelmax et typelmin sont      */
/* toujours en majuscule                                                     */
/*****************************************************************************/
void ttv_printline(ttvfig,ttvfigr,file,typeline,type,find)
ttvfig_list *ttvfig ;
ttvfig_list *ttvfigr ;
FILE *file ;
long typeline ;
long type ;
long find ;
{
 ttvline_list *ptline ;
 ttvline_list *ptlinex ;
 ttvsig_list *ptsig1 ;
 ttvsig_list *ptsig2 ;
 ttvlbloc_list *ptlbloc ;
 chain_list *chainout = NULL ;
 chain_list *chainline = NULL ;
 chain_list *chain ;
 ptype_list *ptype, *pt ;
 long nbline ;
 long i ;
 long j ;
 long nbend ;
 long nbfree ;
 char typelmax[5] ;
 char typelmin[5] ;
 char path ;
 char typepath = 'X' ;
 
 switch(typeline)
     {
      case TTV_LINE_T : path = 'T' ;
                        ptlbloc = ttvfig->TBLOC ;
                        nbline = ttvfig->NBTBLOC ;
                        break ;

      case TTV_LINE_J : path = 'J' ;
                        ptlbloc = ttvfig->JBLOC ;
                        nbline = ttvfig->NBJBLOC ;
                        break ;

      case TTV_LINE_P : path = 'P' ;
                        ptlbloc  = ttvfig->PBLOC ;
                        nbline = ttvfig->NBPBLOC ;
                        break ;

      case TTV_LINE_D : path = 'D' ;
                        ptlbloc  = ttvfig->DBLOC ;
                        nbline = ttvfig->NBDBLOC ;
                        break ;

      case TTV_LINE_E : path = 'E' ;
                        ptlbloc  = ttvfig->EBLOC ;
                        nbline = ttvfig->NBEBLOC ;
                        break ;

      case TTV_LINE_F : path = 'F' ;
                        ptlbloc  = ttvfig->FBLOC ;
                        nbline = ttvfig->NBFBLOC ;
                        break ;
     }

 nbfree = (TTV_MAX_LBLOC - (nbline % TTV_MAX_LBLOC))  % TTV_MAX_LBLOC ;
 nbend = nbline + nbfree ;
 for(i = nbfree ; i < nbend ; i++)
   {
    j = (i % TTV_MAX_LBLOC) ;
    if((j == 0) && (i != 0)) ptlbloc = ptlbloc->NEXT ;
    ptline = ptlbloc->LINE + j ;
    if((((type & TTV_FILE_TTV)  == TTV_FILE_TTV) &&
      (((ptline->ROOT->ROOT->TYPE & TTV_SIG_MASKOUT) != TTV_SIG_MASKOUT) || 
       ((ptline->NODE->ROOT->TYPE & TTV_SIG_MASKIN) != TTV_SIG_MASKIN))) ||
      ((ptline->TYPE & TTV_LINE_FR) == TTV_LINE_FR)) 
      continue ;
    ptsig1 =  ptline->ROOT->ROOT ;
    if((ptsig1->TYPE & TTV_SIG_MOUTSIG) != TTV_SIG_MOUTSIG)
     {
      ptsig1->TYPE |= TTV_SIG_MOUTSIG ;
      ptsig1->USER = addptype(ptsig1->USER,TTV_SIG_OUTSIG,
                              (void *)addchain(NULL,(void *)ptline)) ;
      chainout = addchain(chainout,(void *)ptsig1) ;
     }
    else 
     {
      ptype = getptype(ptsig1->USER,TTV_SIG_OUTSIG) ;
      ptype->DATA = (void *)addchain(ptype->DATA,(void *)ptline) ;
     }
   }

 chainout = reverse(chainout) ;

 for(chain = chainout ; chain != NULL ;  chain = chain->NEXT)
  {
   chain_list *chainx ;
   chain_list *chainin = NULL ;

   ptsig1 = (ttvsig_list *)chain->DATA ;
   for(chainx = (chain_list *)getptype(ptsig1->USER,TTV_SIG_OUTSIG)->DATA ;
       chainx != NULL ; chainx = chainx->NEXT)
     {
      ptline = (ttvline_list *)chainx->DATA ;
      ptsig2 =  ptline->NODE->ROOT ;
      if((ptsig2->TYPE & TTV_SIG_MINSIG) != TTV_SIG_MINSIG)
       {
        ptsig2->TYPE |= TTV_SIG_MINSIG ;
        ptsig2->USER = addptype(ptsig2->USER,TTV_SIG_INSIG,
                                (void *)addchain(NULL,(void *)ptline)) ;
        chainin = addchain(chainin,(void *)ptsig2) ;
       }
      else
       {
        ptype = getptype(ptsig2->USER,TTV_SIG_INSIG) ;
        ptype->DATA = (void *)addchain(ptype->DATA,(void *)ptline) ;
       }
     }

   for(chainx = chainin ; chainx != NULL ; chainx = chainx->NEXT)
    {
     ptsig2 = (ttvsig_list *)chainx->DATA ;
     if((ptsig1->TYPE & TTV_SIG_R) == TTV_SIG_R)
      {
       chain_list *chainpr = NULL ;    
       chain_list *chainav = NULL ;    
       ptype_list *ptypep = getptype(ptsig2->USER,TTV_SIG_INSIG) ; 
       chain_list *chainev = (chain_list *)ptypep->DATA ;

       while(chainev != NULL)
         {
          if((((ttvline_list*)chainev->DATA)->TYPE & TTV_LINE_PR) 
                                                  == TTV_LINE_PR)
           {
            if(chainav == NULL)
             {
              chainev = chainev->NEXT ;
              ((chain_list*)ptypep->DATA)->NEXT = chainpr ;
              chainpr = (chain_list*)ptypep->DATA ;
              ptypep->DATA = (void*)chainev ;
             }
            else
             {
              chainav->NEXT = chainev->NEXT ;
              chainev->NEXT = chainpr ;
              chainpr = chainev ;
              chainev = chainav->NEXT ;
             }
            continue ;
           }
          chainav = chainev ;
          chainev = chainev->NEXT ;
         }
       ptypep->DATA = append(chainpr,(chain_list*)ptypep->DATA) ;
      }
     else if(((ptsig1->TYPE & TTV_SIG_LR) == TTV_SIG_LR) ||
             ((ptsig1->TYPE & TTV_SIG_LS) == TTV_SIG_LS))
      {
       chain_list *chainres = NULL ;    
       chain_list *chainav = NULL ;    
       ptype_list *ptypep = getptype(ptsig2->USER,TTV_SIG_INSIG) ; 
       chain_list *chainset = (chain_list *)ptypep->DATA ;

       while(chainset != NULL)
         {
          if((((ttvline_list*)chainset->DATA)->TYPE & TTV_LINE_R) 
                                                  == TTV_LINE_R)
           {
            if(chainav == NULL)
             {
              chainset = chainset->NEXT ;
              ((chain_list*)ptypep->DATA)->NEXT = chainres ;
              chainres = (chain_list*)ptypep->DATA ;
              ptypep->DATA = (void*)chainset ;
             }
            else
             {
              chainav->NEXT = chainset->NEXT ;
              chainset->NEXT = chainres ;
              chainres = chainset ;
              chainset = chainav->NEXT ;
             }
            continue ;
           }
          chainav = chainset ;
          chainset = chainset->NEXT ;
         }
       ptypep->DATA = append(chainres,(chain_list*)ptypep->DATA) ;
      }
     else
      {
       chain_list *chaina = NULL ;    
       chain_list *chainu = NULL ;    
       chain_list *chaino = NULL ;    
       chain_list *chainav = NULL ;    
       ptype_list *ptypep = getptype(ptsig2->USER,TTV_SIG_INSIG) ; 
       chain_list *chain = (chain_list *)ptypep->DATA ;

       while(chain != NULL)
         {
          if((((ttvline_list*)chain->DATA)->TYPE & TTV_LINE_A) 
                                                  == TTV_LINE_A)
           {
            if(chainav == NULL)
             {
              chain = chain->NEXT ;
              ((chain_list*)ptypep->DATA)->NEXT = chaina ;
              chaina = (chain_list*)ptypep->DATA ;
              ptypep->DATA = (void*)chain ;
             }
            else
             {
              chainav->NEXT = chain->NEXT ;
              chain->NEXT = chaina ;
              chaina = chain ;
              chain = chainav->NEXT ;
             }
            continue ;
           }
          if((((ttvline_list*)chain->DATA)->TYPE & TTV_LINE_U) 
                                                  == TTV_LINE_U)
           {
            if(chainav == NULL)
             {
              chain = chain->NEXT ;
              ((chain_list*)ptypep->DATA)->NEXT = chainu ;
              chainu = (chain_list*)ptypep->DATA ;
              ptypep->DATA = (void*)chain ;
             }
            else
             {
              chainav->NEXT = chain->NEXT ;
              chain->NEXT = chainu ;
              chainu = chain ;
              chain = chainav->NEXT ;
             }
            continue ;
           }
          if((((ttvline_list*)chain->DATA)->TYPE & TTV_LINE_O) 
                                                  == TTV_LINE_O)
           {
            if(chainav == NULL)
             {
              chain = chain->NEXT ;
              ((chain_list*)ptypep->DATA)->NEXT = chaino ;
              chaino = (chain_list*)ptypep->DATA ;
              ptypep->DATA = (void*)chain ;
             }
            else
             {
              chainav->NEXT = chain->NEXT ;
              chain->NEXT = chaino ;
              chaino = chain ;
              chain = chainav->NEXT ;
             }
            continue ;
           }
          chainav = chain ;
          chain = chain->NEXT ;
         }
       ptypep->DATA = append(chaina,(chain_list*)ptypep->DATA) ;
       ptypep->DATA = append(chaino,(chain_list*)ptypep->DATA) ;
       ptypep->DATA = append(chainu,(chain_list*)ptypep->DATA) ;
      }
     chainline = append(reverse((chain_list*)getptype(ptsig2->USER,TTV_SIG_INSIG)->DATA),
                        chainline) ;
     ptsig2->TYPE &= ~(TTV_SIG_MINSIG) ;
     ptsig2->USER = delptype(ptsig2->USER,TTV_SIG_INSIG) ;
    }
   freechain(chainin) ;
   freechain((chain_list *)getptype(ptsig1->USER,TTV_SIG_OUTSIG)->DATA) ;
   ptsig1->TYPE &= ~(TTV_SIG_MOUTSIG) ;
   ptsig1->USER = delptype(ptsig1->USER,TTV_SIG_OUTSIG) ;
  }

 freechain(chainout) ;
 chain = chainline ;
 while(chain != NULL) 
  {
   ptline = (ttvline_list *)chain->DATA ;
   ptsig1 = ptline->NODE->ROOT ;
   ptsig2 = ptline->ROOT->ROOT ;

   if((type & (TTV_FILE_TTX | TTV_FILE_DTX)) != 0)
    {
     int x1, x2;
     fprintf(file,"%c %c %ld %c %ld (",path,
             ttv_getsigtype(ptsig1,ptline,1),
             (long)(getptype(ptsig1->USER,TTV_SIG_INDEX)->DATA),
             ttv_getsigtype(ptsig2,ptline,2),
             (long)(getptype(ptsig2->USER,TTV_SIG_INDEX)->DATA)) ;

     x1 = (long)(getptype(ptsig1->USER,TTV_SIG_INDEX)->DATA) ;
     x2 = (long)(getptype(ptsig2->USER,TTV_SIG_INDEX)->DATA) ;
     if((ptsig2->TYPE & TTV_SIG_R) != TTV_SIG_R)
      {
       if((ptline->TYPE & TTV_LINE_S) == TTV_LINE_S)
         typepath = 'O' ; 
       else if((ptline->TYPE & TTV_LINE_A) == TTV_LINE_A)
         typepath = 'A' ; 
       else if((ptline->TYPE & TTV_LINE_U) == TTV_LINE_U)
         typepath = 'S' ; 
       else if((ptline->TYPE & TTV_LINE_O) == TTV_LINE_O)
         typepath = 'H' ; 
       else if((ptline->TYPE & TTV_LINE_R) == TTV_LINE_R)
         typepath = 'Z' ; 
       else
         typepath = 'X' ; 
      }
     else 
      {
       if((ptline->TYPE & TTV_LINE_EV) == TTV_LINE_EV)
         typepath = 'E' ;
       else if((ptline->TYPE & TTV_LINE_PR) == TTV_LINE_PR)
         typepath = 'P' ;
      }
    }
   else
    {
     if((ptline->TYPE & TTV_LINE_CONT) == TTV_LINE_CONT)
      {
       chain = chain->NEXT ;
       continue ;
      }
     if((ptsig2->TYPE & TTV_SIG_R) != TTV_SIG_R)
      {
       fprintf(file,"T X X ") ;
       typepath = 'X' ;
      }
     else 
      {
       if((ptline->TYPE & TTV_LINE_PR) == TTV_LINE_PR)
        {
         fprintf(file,"T P P ") ;
         typepath = 'P' ;
        }
       else
        {
         fprintf(file,"T E E ") ;
         typepath = 'E' ;
        }
      }
     ttv_printname(ttvfigr,file,ptsig1) ;
     fprintf(file," ") ;
     ttv_printname(ttvfigr,file,ptsig2) ;
     fprintf(file," (") ;
    }

  for(; chain != NULL ; chain = chain->NEXT)
    {
     ptlinex = (ttvline_list *)chain->DATA ;

     if(((ptlinex->TYPE & TTV_LINE_CONT) == TTV_LINE_CONT) &&
        ((type & (TTV_FILE_TTX | TTV_FILE_DTX)) == 0))
       continue ;

     if((ptlinex->ROOT->ROOT != ptsig2) || (ptlinex->NODE->ROOT != ptsig1)) 
         break ;
     if(typepath != 'X')
      {
       if((typepath == 'O') &&
         ((ptlinex->TYPE & TTV_LINE_S) != TTV_LINE_S))
         break ;
       if((typepath == 'Z') &&
         ((ptlinex->TYPE & TTV_LINE_R) != TTV_LINE_R))
         break ;
       if((typepath == 'S') &&
         ((ptlinex->TYPE & TTV_LINE_U) != TTV_LINE_U))
         break ;
       if((typepath == 'H') &&
         ((ptlinex->TYPE & TTV_LINE_O) != TTV_LINE_O))
         break ;
       if((typepath == 'A') &&
         ((ptlinex->TYPE & TTV_LINE_A) != TTV_LINE_A))
         break ;
       if((typepath == 'E') &&
         ((ptlinex->TYPE & TTV_LINE_EV) != TTV_LINE_EV))
         break ; ;
       if((typepath == 'P') &&
         ((ptlinex->TYPE & TTV_LINE_PR) != TTV_LINE_PR))
         break ;
      }
     else 
      {
       if((ptlinex->TYPE & (TTV_LINE_CONT|
                            TTV_LINE_S|
                            TTV_LINE_R|
                            TTV_LINE_EV|
                            TTV_LINE_PR)) != 0)
           break ;
      }

     if((ptlinex->NODE->ROOT) == ptsig1)
      {
       ttvevent_list *cmdmax ;
       long cmdmaxindex ;
       ttvevent_list *cmdmin ;
       long cmdminindex ;
       char fin ;
       char fout ;
       char fcmdmax  = 'X';
       char fcmdmin  = 'X';

       if((ptsig2->TYPE & TTV_SIG_L) == TTV_SIG_L)
         {
          if(ptlinex->VALMAX != TTV_NOTIME)
           {
            cmdmax = ttv_getlinecmd(ttvfigr,ptlinex,TTV_LINE_CMDMAX) ;
            if(cmdmax != NULL)
             {
              if((cmdmax->TYPE & TTV_NODE_UP) == TTV_NODE_UP) fcmdmax = 'U' ;
              else fcmdmax = 'D' ;
             }
           }
          if(ptlinex->VALMIN != TTV_NOTIME)
           {
            cmdmin = ttv_getlinecmd(ttvfigr,ptlinex,TTV_LINE_CMDMIN) ;
            if(cmdmin != NULL)
             {
              if((cmdmin->TYPE & TTV_NODE_UP) == TTV_NODE_UP) fcmdmin = 'U' ;
              else fcmdmin = 'D' ;
             }
           }
         }
       else 
         {
          cmdmax = cmdmin = NULL ;
         }

       if((ptlinex->NODE->TYPE & TTV_NODE_UP) == TTV_NODE_UP) fin = 'U' ;
       else fin = 'D' ;

       if((ptlinex->TYPE & TTV_LINE_HZ) == TTV_LINE_HZ) 
         {
          if(((ptlinex->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP) &&
             ((type & (TTV_FILE_TTX | TTV_FILE_DTX)) != 0))
            fout = 'Y' ;
          else
            fout = 'Z' ;
         }
       else if((ptlinex->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP) 
               fout = 'U' ;
       else fout = 'D' ;
             
       if((type & (TTV_FILE_TTX | TTV_FILE_DTX)) != 0)
         {
          if((ptlinex->TYPE & TTV_LINE_DENPTMAX) == TTV_LINE_DENPTMAX)
            {
             if((ptlinex->TYPE & TTV_LINE_RC) == TTV_LINE_RC)
              strcpy(typelmax,"IMAX") ;
             else if((ptlinex->TYPE & TTV_LINE_A) == TTV_LINE_A)
              strcpy(typelmax,"AMAX") ;
             else if((ptlinex->TYPE & TTV_LINE_O) == TTV_LINE_O)
              strcpy(typelmax,"HMAX") ;
             else if((ptlinex->TYPE & TTV_LINE_U) == TTV_LINE_U)
              strcpy(typelmax,"SMAX") ;
             else
              strcpy(typelmax,"MAX") ;
            }
          else
            {
             if((ptlinex->TYPE & TTV_LINE_RC) == TTV_LINE_RC)
              strcpy(typelmax,"IMAX") ;
             else if((ptlinex->TYPE & TTV_LINE_A) == TTV_LINE_A)
              strcpy(typelmax,"AMAX") ;
             else if((ptlinex->TYPE & TTV_LINE_O) == TTV_LINE_O)
              strcpy(typelmax,"HMAX") ;
             else if((ptlinex->TYPE & TTV_LINE_U) == TTV_LINE_U)
              strcpy(typelmax,"SMAX") ;
             else
              strcpy(typelmax,"MAX") ;
            }
          if((ptlinex->TYPE & TTV_LINE_DENPTMIN) == TTV_LINE_DENPTMIN)
            {
             if((ptlinex->TYPE & TTV_LINE_RC) == TTV_LINE_RC)
              strcpy(typelmin,"IMIN") ;
             else if((ptlinex->TYPE & TTV_LINE_A) == TTV_LINE_A)
              strcpy(typelmin,"AMIN") ;
             else if((ptlinex->TYPE & TTV_LINE_O) == TTV_LINE_O)
              strcpy(typelmin,"HMIN") ;
             else if((ptlinex->TYPE & TTV_LINE_U) == TTV_LINE_U)
              strcpy(typelmin,"SMIN") ;
             else
              strcpy(typelmin,"MIN") ;
            }
          else  
            {
             if((ptlinex->TYPE & TTV_LINE_RC) == TTV_LINE_RC)
              strcpy(typelmin,"IMIN") ;
             else if((ptlinex->TYPE & TTV_LINE_A) == TTV_LINE_A)
              strcpy(typelmin,"AMIN") ;
             else if((ptlinex->TYPE & TTV_LINE_O) == TTV_LINE_O)
              strcpy(typelmin,"HMIN") ;
             else if((ptlinex->TYPE & TTV_LINE_U) == TTV_LINE_U)
              strcpy(typelmin,"SMIN") ;
             else
              strcpy(typelmin,"MIN") ;
            }

          if(ptlinex->VALMAX != TTV_NOTIME)
           {
            if(cmdmax != NULL)
            {
              if ((pt=getptype(cmdmax->ROOT->USER, TTV_CMD_INDEX))==NULL)
              {
                char buf[512], buf1[512];
                ttv_getsigname(ttvfig,buf,cmdmax->ROOT) ;
                ttv_getsigname(ttvfig,buf1,ptsig2) ;
                avt_errmsg( TTV_ERRMSG, "057", AVT_ERROR, buf, buf1);
                cmdmaxindex = (long)0 ;
              }
              else
                cmdmaxindex = (long)pt->DATA ;
            }
            else
              cmdmaxindex = (long)0 ;
            ttv_drvline(file,cmdmaxindex,fcmdmax,fin,fout,typelmax,
                        ptlinex->VALMAX,ptlinex->FMAX,
                        ptlinex->MDMAX,ptlinex->MFMAX) ;
           }

          if(ptlinex->VALMIN != TTV_NOTIME)
           {
            if(cmdmin != NULL)
            {
              if ((pt=getptype(cmdmin->ROOT->USER, TTV_CMD_INDEX))==NULL)
              {
                char buf[512], buf1[512];
                ttv_getsigname(ttvfig,buf,cmdmin->ROOT) ;
                ttv_getsigname(ttvfig,buf1,ptsig2) ;
                avt_errmsg( TTV_ERRMSG, "057", AVT_ERROR, buf, buf1);
                cmdminindex = (long)0 ;
              }
              else
                cmdminindex = (long)pt->DATA ;
            }
            else
              cmdminindex = (long)0 ;
            ttv_drvline(file,cmdminindex,fcmdmin,fin,fout,typelmin,
                        ptlinex->VALMIN,ptlinex->FMIN,
                        ptlinex->MDMIN,ptlinex->MFMIN) ;
           }
         }
       else
         {
          if(ptlinex->VALMAX != TTV_NOTIME)
           {
           if(cmdmax != NULL)
             {
              fprintf(file,"\n\t(");
              ttv_printname(ttvfigr,file,cmdmax->ROOT) ;
              if(((ptsig2->TYPE & TTV_SIG_C) == TTV_SIG_C) &&
                ((ptlinex->TYPE & TTV_LINE_RC) != TTV_LINE_RC))
               {
                float rs ;
                long frs ;
                if((ptlinex->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP) 
                   frs = 0 ;
                else
                   frs = 0 ;
                if(fout == 'U')
                 rs = 0.0 ;
                else
                 rs = 0.0 ;
                fprintf(file," (%c%c MAX %ld %ld %.2f 0 %ld))",
                      fin,fout,(long)(ptlinex->VALMAX/TTV_UNIT),(long)(ptlinex->FMAX/TTV_UNIT),rs,frs) ;
               }
              else
               {
                fprintf(file," (%c%c MAX %ld %ld))",
                        fin,fout,(long)(ptlinex->VALMAX/TTV_UNIT),(long)(ptlinex->FMAX/TTV_UNIT)) ;
               }
              if(((ptlinex->VALMAX <= TTV_SEUIL_MAX) && 
                  (ptlinex->VALMAX >= TTV_SEUIL_MIN)) && 
                  ((type & TTV_FILE_TTV) == TTV_FILE_TTV))
                  ttv_printcritic(ttvfigr,file,ptlinex->ROOT,ptlinex->NODE,
                  ((fout == 'Z') || (fout == 'Y')) ? 
                                  find | TTV_FIND_MAX | TTV_FIND_HZ : 
                                  find | TTV_FIND_MAX) ;
             }
           else
             {
              if(((ptsig2->TYPE & TTV_SIG_C) == TTV_SIG_C) &&
                ((ptlinex->TYPE & TTV_LINE_RC) != TTV_LINE_RC))
               {
                float rs ;
                long frs ;
                if(fout == 'U')
                 rs = 0.0 ;
                else
                 rs = 0.0 ;
                if((ptlinex->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP) 
                   frs = 0 ;
                else
                   frs = 0 ;
                fprintf(file,"\n\t(* (%c%c MAX %ld %ld %.2f 0 %ld))",
                      fin,fout,(long)(ptlinex->VALMAX/TTV_UNIT),(long)(ptlinex->FMAX/TTV_UNIT),rs,frs) ;
               }
              else
               {
                fprintf(file,"\n\t(* (%c%c MAX %ld %ld))",
                        fin,fout,(long)(ptlinex->VALMAX/TTV_UNIT),(long)(ptlinex->FMAX/TTV_UNIT)) ;
               }

              if(((ptlinex->VALMAX <= TTV_SEUIL_MAX) && 
                  (ptlinex->VALMAX >= TTV_SEUIL_MIN)) && 
                  ((type & TTV_FILE_TTV) == TTV_FILE_TTV))
                  ttv_printcritic(ttvfigr,file,ptlinex->ROOT,ptlinex->NODE,
                  ((fout == 'Z') || (fout == 'Y'))
                                  ? find | TTV_FIND_MAX | TTV_FIND_HZ : 
                                  find | TTV_FIND_MAX) ;
             }
           }
          if(ptlinex->VALMIN != TTV_NOTIME)
          {
           if(cmdmin != NULL)
             {
              fprintf(file,"\n\t(");
              ttv_printname(ttvfigr,file,cmdmin->ROOT) ;
              if(((ptsig2->TYPE & TTV_SIG_C) == TTV_SIG_C) &&
                ((ptlinex->TYPE & TTV_LINE_RC) != TTV_LINE_RC))
               {
                float rs ;
                long frs ;
                if(fout == 'U')
                 rs = 0.0 ;
                else
                 rs = 0.0 ;
                if((ptlinex->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP) 
                   frs = 0 ;
                else
                   frs = 0 ;
                fprintf(file," (%c%c MIN %ld %ld %.2f 0 %ld))",
                      fin,fout,(long)(ptlinex->VALMIN/TTV_UNIT),(long)(ptlinex->FMIN/TTV_UNIT),rs,frs) ;
               }
              else
               {
                fprintf(file," (%c%c MIN %ld %ld))",
                        fin,fout,(long)(ptlinex->VALMIN/TTV_UNIT),(long)(ptlinex->FMIN/TTV_UNIT)) ;
               }
              if(((ptlinex->VALMIN <= TTV_SEUIL_MAX) && 
                  (ptlinex->VALMIN >= TTV_SEUIL_MIN)) && 
                  ((type & TTV_FILE_TTV) == TTV_FILE_TTV))
                  ttv_printcritic(ttvfigr,file,ptlinex->ROOT,ptlinex->NODE,
                  ((fout == 'Z') || (fout == 'Y')) ? 
                                  find | TTV_FIND_MIN | TTV_FIND_HZ : 
                                  find | TTV_FIND_MIN) ;
             }
           else
             {
              if(((ptsig2->TYPE & TTV_SIG_C) == TTV_SIG_C) &&
                ((ptlinex->TYPE & TTV_LINE_RC) != TTV_LINE_RC))
               {
                float rs ;
                long frs ;
                if(fout == 'U')
                 rs = 0.0 ;
                else
                 rs = 0.0 ;
                if((ptlinex->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP) 
                   frs = 0 ;
                else
                   frs = 0 ;
                fprintf(file,"\n\t(* (%c%c MIN %ld %ld %.2f 0 %ld))",
                      fin,fout,(long)(ptlinex->VALMIN/TTV_UNIT),(long)(ptlinex->FMIN/TTV_UNIT),rs,frs) ;
               }
            else
               {
                fprintf(file,"\n\t(* (%c%c MIN %ld %ld))",
                        fin,fout,(long)(ptlinex->VALMIN/TTV_UNIT),(long)(ptlinex->FMIN/TTV_UNIT)) ;
               }
              if(((ptlinex->VALMIN <= TTV_SEUIL_MAX) && 
                  (ptlinex->VALMIN >= TTV_SEUIL_MIN)) && 
                  ((type & TTV_FILE_TTV) == TTV_FILE_TTV))
                  ttv_printcritic(ttvfigr,file,ptlinex->ROOT,ptlinex->NODE,
                  ((fout == 'Z') || (fout == 'Y')) ? 
                                  find | TTV_FIND_MIN | TTV_FIND_HZ : 
                                  find | TTV_FIND_MIN) ;
             }
            }
         }
      }
    }

  if((type & (TTV_FILE_TTX | TTV_FILE_DTX)) != 0)
    fprintf(file,");\n\n") ;
  else
    fprintf(file,"\n);\n") ;
 }

 freechain(chainline) ;
}

/*****************************************************************************/
/*                        function ttv_printhead()                           */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* file : fichier                                                            */
/* type : type de fichier ttv dtv                                            */
/* ptpathmax : plus long chemin                                              */
/* ptpathmin : plus court chemin                                             */
/*                                                                           */
/* ecrit dans les fichiers ttv et dtv des commentaires avec les details des  */
/* chemins critiques                                                         */
/*****************************************************************************/
void ttv_printhead(ttvfig,file,type,ptpathmax,ptpathmin)
ttvfig_list *ttvfig ;
FILE *file ;
long type ;
ttvpath_list *ptpathmax ;
ttvpath_list *ptpathmin ;
{
 int i ;

 if((type & TTV_FILE_TTV)== TTV_FILE_TTV) 
    {
     fprintf(file,"#Perfmodule %s\n",ttvfig->INFO->FIGNAME) ;
    }
 else
    {
     if(TTV_LANG == TTV_LANG_E)
     fprintf(file,"#intermediate Perfmodule %s\n",ttvfig->INFO->FIGNAME) ;
     else if(TTV_LANG == TTV_LANG_F)
     fprintf(file,"#Perfmodule intermediaire %s\n",ttvfig->INFO->FIGNAME) ;
    }
 
 fprintf(file,"#%s version %s ",ttvfig->INFO->TOOLNAME,
                                ttvfig->INFO->TOOLVERSION) ;

 if(TTV_LANG == TTV_LANG_E)
    {
     if(TTV_INFO_TOOLDATE != NULL)
      fprintf(file,"of %s\n",TTV_INFO_TOOLDATE) ;
     else
      fprintf(file,"\n") ;
     if(TTV_INFO_CAPARM >= (long)0)
      fprintf(file,"#out of path capacitance factor   (opc)  = %.2f%%\n",
              (double)TTV_INFO_CAPARM) ;
     if(TTV_INFO_CAPASW >= (long)0)
      fprintf(file,"#input switch capacitance factor  (swc)  = %.2f%%\n",
              (double)TTV_INFO_CAPASW) ;
     fprintf(file,"#output capacitance               (cout) = %.3fpF\n",
             ((double)ttvfig->INFO->CAPAOUT / 1000.0)) ;
     fprintf(file,"#command line:") ;
     for(i = 0 ; i != TTV_INFO_ARGC ; i++)
     fprintf(file," %s",TTV_INFO_ARGV[i]) ;
     fprintf(file,"\n#technology: %s -- V%s\n",ttvfig->INFO->TECHNONAME,
             ttvfig->INFO->TECHNOVERSION) ;
    }
 else if(TTV_LANG == TTV_LANG_F)
    {
     if(TTV_INFO_TOOLDATE != NULL)
      fprintf(file,"du %s\n",TTV_INFO_TOOLDATE) ;
     else
      fprintf(file,"\n") ;
     if(TTV_INFO_CAPARM >= (long)0)
     fprintf(file,"#facteur de capacite hors-chemin  (opc)  = %.2f%%\n",
             (double)TTV_INFO_CAPARM) ;
     if(TTV_INFO_CAPASW >= (long)0)
      fprintf(file,"#facteur de switch d'antree       (swc)  = %.2f%%\n",
              (double)TTV_INFO_CAPARM) ;
     fprintf(file,"#capacite de charge               (cout) = %.3fpF\n",
             ((double)ttvfig->INFO->CAPAOUT / 1000.0)) ;
     fprintf(file,"#ligne de commande:") ;
     for(i = 0 ; i != TTV_INFO_ARGC ; i++)
     fprintf(file," %s",TTV_INFO_ARGV[i]) ;
     fprintf(file,"\n#technologie: %s -- V%s\n",ttvfig->INFO->TECHNONAME,
             ttvfig->INFO->TECHNOVERSION) ;
    }

 if((type & TTV_FILE_TTV)== TTV_FILE_TTV) 
  {
   if(ptpathmax != NULL)
    {
     if(TTV_LANG == TTV_LANG_E)
         fprintf(file,"#worst case delay = %ld\n",
                 ptpathmax->DELAY) ;
     else if(TTV_LANG == TTV_LANG_F)
         fprintf(file,"#plus long delai = %ld\n",
                 ptpathmax->DELAY) ;

     if((ptpathmax->NODE->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
     fprintf(file,"#UP(") ;
     else fprintf(file,"#DOWN(") ;

     ttv_printname(ttvfig,file,ptpathmax->NODE->ROOT) ;
     fprintf(file,") ---> ") ;
    
     if((ptpathmax->TYPE & TTV_FIND_HZ) == TTV_FIND_HZ)
     fprintf(file,"HighZ(") ;
     else if((ptpathmax->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
     fprintf(file,"UP(") ;
     else fprintf(file,"DOWN(") ;
        
     ttv_printname(ttvfig,file,ptpathmax->ROOT->ROOT) ;
     fprintf(file,") --- Tp=%ld pS",
             ptpathmax->DELAY) ;
     ttv_printcritic(ttvfig,file,ptpathmax->ROOT,
                                 ptpathmax->NODE,ptpathmax->TYPE) ;
     fprintf(file,"\n") ;
    }

   if(ptpathmin != NULL)
    {
     if(TTV_LANG == TTV_LANG_E)
         fprintf(file,"#best case delay = %ld\n",
                 ptpathmin->DELAY) ;
     else if(TTV_LANG == TTV_LANG_F)
         fprintf(file,"#plus court delai = %ld\n",
                 ptpathmin->DELAY) ;

     if((ptpathmin->NODE->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
     fprintf(file,"#UP(") ;
     else fprintf(file,"#DOWN(") ;

     ttv_printname(ttvfig,file,ptpathmin->NODE->ROOT) ;
     fprintf(file,") ---> ") ;
    
     if((ptpathmin->TYPE & TTV_FIND_HZ) == TTV_FIND_HZ)
     fprintf(file,"HighZ(") ;
     else if((ptpathmin->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
     fprintf(file,"UP(") ;
     else fprintf(file,"DOWN(") ;
        
     ttv_printname(ttvfig,file,ptpathmin->ROOT->ROOT) ;
     fprintf(file,") --- Tp=%ld pS",
             ptpathmin->DELAY) ;
     ttv_printcritic(ttvfig,file,ptpathmin->ROOT,
                                 ptpathmin->NODE,ptpathmin->TYPE) ;
     fprintf(file,"\n") ;
    }
 }
}

/*****************************************************************************/
/*                        function ttv_drittv()                              */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* type : type de fichier ttv dtv ttx ttx                                    */
/* find : type de recherche hierarchique ou pas                              */
/*                                                                           */
/* ecrit sur disque un type de fichier particulier                           */
/* il est possible d'ecrire des fichiers dtv ttv dtx et ttx                  */
/*****************************************************************************/
void ttv_drittv(ttvfig_list *ttvfig, long type, long find, char *filename)
{
 ttvsig_list *ptsig ;
 ttvsbloc_list *ptsbloc ;
 ttvpath_list *ptpathmax = NULL ;
 ttvpath_list *ptpathmin = NULL ;
 chain_list *chain ;
 FILE *file ;
 long i ;
 long j ;
 long nbfree ;
 long nbend ;
 char typesig, *savename ;
 char filetype[4] ;
 char buf[64];

 if((type & (TTV_FILE_TTX | TTV_FILE_DTX)) != 0)
  {
   if((type & TTV_FILE_TTX) == TTV_FILE_TTX)
    {
     strcpy(filetype,"ttx") ;
     if((ttvfig->STATUS & TTV_STS_TTX) != (TTV_STS_TTX))
       {
        char straux[128] ;

        sprintf(straux,"%s.%s",ttvfig->INFO->FIGNAME,filetype) ;
        ttv_error(22,straux,TTV_WARNING) ;
        return ;
       }
     if (filename!=NULL) savename=ttvfig->INFO->FILENAME, ttvfig->INFO->FILENAME=filename;
     file = ttv_openfile(ttvfig,TTV_FILE_TTX,WRITE_TEXT) ;
     if (filename!=NULL) ttvfig->INFO->FILENAME=savename;
    }
   else 
    {
     strcpy(filetype,"dtx") ;
     if((ttvfig->STATUS & TTV_STS_DTX) != TTV_STS_DTX)
       {
        char straux[128] ;

        sprintf(straux,"%s.%s",ttvfig->INFO->FIGNAME,filetype) ;
        ttv_error(22,straux,TTV_WARNING) ;
        return ;
       }
     if (filename!=NULL) savename=ttvfig->INFO->FILENAME, ttvfig->INFO->FILENAME=filename;
     file = ttv_openfile(ttvfig,TTV_FILE_DTX,WRITE_TEXT) ;
     if (filename!=NULL) ttvfig->INFO->FILENAME=savename;
    }

   if(file == NULL)
      ttv_error(11,ttvfig->INFO->FIGNAME,TTV_ERROR) ;
   
   avt_printExecInfo(file, "#", ttvfig->INFO->FIGNAME, "");
   
   if (ttvfig->INFO->TNOM>-500) sprintf(buf," @%.2f",ttvfig->INFO->TNOM);
   else strcpy(buf,"");

   fprintf(file,"H3 %s %s %s %s %s %g %.1f %.2f %.2f %.2f %.2f%s %.2f %ld (%d %d %d %d %d %d);\n\n",           
           ttvfig->INFO->TOOLNAME,ttvfig->INFO->TOOLVERSION,ttvfig->INFO->FIGNAME,
           ttvfig->INFO->TECHNONAME,ttvfig->INFO->TECHNOVERSION,
           (double)ttvfig->INFO->SLOPE/TTV_UNIT,ttvfig->INFO->CAPAOUT,ttvfig->INFO->STHHIGH,
           ttvfig->INFO->STHLOW,ttvfig->INFO->DTH,ttvfig->INFO->TEMP, buf,
           ttvfig->INFO->VDD,ttvfig->INFO->LEVEL,
           ttvfig->INFO->TTVDAY,ttvfig->INFO->TTVMONTH,ttvfig->INFO->TTVYEAR,
           ttvfig->INFO->TTVHOUR,ttvfig->INFO->TTVMIN,ttvfig->INFO->TTVSEC) ;
  }
 else
  {
   if((type & TTV_FILE_TTV) == TTV_FILE_TTV)
    {
     strcpy(filetype,"ttv") ;
     if((ttvfig->STATUS & TTV_STS_TTX) != TTV_STS_TTX)
       {
        char straux[128] ;

        sprintf(straux,"%s.%s",ttvfig->INFO->FIGNAME,filetype) ;
        ttv_error(22,straux,TTV_WARNING) ;
        return ;
       }
     if (filename!=NULL) savename=ttvfig->INFO->FILENAME, ttvfig->INFO->FILENAME=filename;
     file = ttv_openfile(ttvfig,TTV_FILE_TTV,WRITE_TEXT) ;
     if (filename!=NULL) ttvfig->INFO->FILENAME=savename;
     ptpathmax = ttv_getcriticpath(ttvfig,NULL,TTV_NAME_IN,TTV_NAME_OUT,
                                   TTV_DELAY_MAX,TTV_DELAY_MIN,1,TTV_FIND_MAX) ;
     ptpathmin = ttv_getcriticpath(ttvfig,NULL,TTV_NAME_IN,TTV_NAME_OUT,
                                   TTV_DELAY_MAX,TTV_DELAY_MIN,1,TTV_FIND_MIN) ;
     if((find & TTV_FIND_NOTSAVE) == TTV_FIND_NOTSAVE)
       {
        if(ptpathmax != NULL)
          ptpathmax->TYPE |= TTV_FIND_NOTSAVE ;
        if(ptpathmin != NULL)
          ptpathmin->TYPE |= TTV_FIND_NOTSAVE ;
       }
    }
   else 
    {
     strcpy(filetype,"dtv") ;
     if((ttvfig->STATUS & TTV_STS_DTX) != TTV_STS_DTX)
       {
        char straux[128] ;

        sprintf(straux,"%s.%s",ttvfig->INFO->FIGNAME,filetype) ;
        ttv_error(22,straux,TTV_WARNING) ;
        return ;
       }
     if (filename!=NULL) savename=ttvfig->INFO->FILENAME, ttvfig->INFO->FILENAME=filename;
     file = ttv_openfile(ttvfig,TTV_FILE_DTV,WRITE_TEXT) ;
     if (filename!=NULL) ttvfig->INFO->FILENAME=savename;
    }
   ttv_printhead(ttvfig,file,type,ptpathmax,ptpathmin) ;
   ttv_freepathlist(ptpathmax) ;
   ttv_freepathlist(ptpathmin) ;
   fprintf(file,"\n") ;
   fprintf(file,"\n") ;
   fprintf(file,"G %s %s %ld %s %s (%d %d %d %d %d %d) %ld *;\n\n",
           ttvfig->INFO->FIGNAME,ttvfig->INFO->TECHNONAME,(long)(ttvfig->INFO->SLOPE/TTV_UNIT),
           ttvfig->INFO->TOOLNAME,ttvfig->INFO->TOOLVERSION,ttvfig->INFO->TTVDAY,
           ttvfig->INFO->TTVMONTH,ttvfig->INFO->TTVYEAR,ttvfig->INFO->TTVHOUR,
           ttvfig->INFO->TTVMIN,ttvfig->INFO->TTVSEC,ttvfig->INFO->LEVEL) ;
  }

 if(file == NULL)
   {
    char straux[128] ;

    sprintf(straux,"%s.%s",ttvfig->INFO->FIGNAME,filetype) ;
    ttv_error(20,straux,TTV_ERROR) ;
   }

if(ttvfig->INS != NULL)
 {
  for(chain = ttvfig->INS ; chain != NULL ; chain = chain->NEXT)
   {
    ttvfig_list *ttvins = chain->DATA ;
    if((type & (TTV_FILE_TTX | TTV_FILE_DTX)) != 0)
      {
       fprintf(file,"X %s %s;\n",ttvins->INFO->FIGNAME,ttvins->INSNAME) ;
      }
    else
      {
       fprintf(file,"H %s %s;\n",ttvins->INFO->FIGNAME,ttvins->INSNAME) ;
      }
   }
  fprintf(file,"\n") ;
 }

 if(ttvfig->NBCONSIG != 0)
  {
   for(i = 0 ; i < ttvfig->NBCONSIG ; i++)
    {
     ptsig = *(ttvfig->CONSIG + i) ;
     if((type & (TTV_FILE_TTX | TTV_FILE_DTX)) != 0)
       {
        if((ptsig->TYPE & TTV_SIG_R) != TTV_SIG_R)
          ptsig->USER = addptype(ptsig->USER,TTV_SIG_INDEX,(void *)(i+1)) ;
        if((ptsig->TYPE & TTV_SIG_Q) == TTV_SIG_Q)
          fprintf(file,"C %cQ %ld ",ttv_getdir(ptsig),i+1) ;
        else if((ptsig->TYPE & TTV_SIG_L) == TTV_SIG_L)
          fprintf(file,"C %cL %ld ",ttv_getdir(ptsig),i+1) ;
        else if((ptsig->TYPE & TTV_SIG_R) == TTV_SIG_R)
          fprintf(file,"C %cR %ld ",ttv_getdir(ptsig),i+1) ;
        else if((ptsig->TYPE & TTV_SIG_B) == TTV_SIG_B)
          fprintf(file,"C %cB %ld ",ttv_getdir(ptsig),i+1) ;
        else
          fprintf(file,"C %c %ld ",ttv_getdir(ptsig),i+1) ;
        ttv_printname(ttvfig,file,ptsig) ;
        fprintf(file," ") ;
        ttv_printcaracsig(ttvfig,file,ptsig) ;
        fprintf(file," ") ;
        ttv_printsigsettings(ttvfig,file,ptsig,type) ;
        ttv_printsigproperty(ttvfig,file,ptsig) ;
       }
     else
       {
        char dir ;
        if((type & TTV_FILE_TTV) == TTV_FILE_TTV)
         {
          if((ttv_testmask(ttvfig,ptsig,TTV_NAME_IN) != 0) ||
             (TTV_PATH_SAVE == TTV_SAVE_ALLPATH))
            ptsig->TYPE |= TTV_SIG_MASKIN ;
          if((ttv_testmask(ttvfig,ptsig,TTV_NAME_OUT) != 0) ||
             (TTV_PATH_SAVE == TTV_SAVE_ALLPATH))
            ptsig->TYPE |= TTV_SIG_MASKOUT ;
         }
        fprintf(file,"X %c ",(((dir = ttv_getdir(ptsig)) == 'X') ? 'U' : dir)) ;
        ttv_printname(ttvfig,file,ptsig) ;
        fprintf(file," %ld",(long)(ptsig->CAPA)) ;
       }
     fprintf(file,";\n") ;
    }
   fprintf(file,"\n") ;
  }

 if(ttvfig->NBNCSIG != 0)
  {
   for(i = 0 ; i < ttvfig->NBNCSIG ; i++)
    {
     ptsig = *(ttvfig->NCSIG + i) ;
     if((type & TTV_FILE_TTV) == TTV_FILE_TTV)
      {
       if((ttv_testmask(ttvfig,ptsig,TTV_NAME_IN) != 0) ||
          (TTV_PATH_SAVE == TTV_SAVE_ALLPATH))
         ptsig->TYPE |= TTV_SIG_MASKIN ;
       if((ttv_testmask(ttvfig,ptsig,TTV_NAME_OUT) != 0) ||
          (TTV_PATH_SAVE == TTV_SAVE_ALLPATH))
         ptsig->TYPE |= TTV_SIG_MASKOUT ;
      }
     if((type & (TTV_FILE_TTX | TTV_FILE_DTX)) != 0)
       {
        ptsig->USER = addptype(ptsig->USER,TTV_SIG_INDEX,(void *)(i+1)) ;
        if((ptsig->TYPE & TTV_SIG_Q) == TTV_SIG_Q) typesig = 'Q' ;
        else if((ptsig->TYPE & TTV_SIG_L) == TTV_SIG_L) typesig = 'L' ;
        else if((ptsig->TYPE & TTV_SIG_R) == TTV_SIG_R) typesig = 'R' ;
        else if((ptsig->TYPE & TTV_SIG_B) == TTV_SIG_B) typesig = 'B' ;
        else typesig = 'I' ;
        fprintf(file,"N %c %ld ",typesig,i+1) ;
        ttv_printname(ttvfig,file,ptsig) ;
        fprintf(file," ") ;
        ttv_printcaracsig(ttvfig,file,ptsig) ;
        ttv_printsigproperty(ttvfig,file,ptsig) ;
       }
     else
       {
        fprintf(file,"N ") ;
        ttv_printname(ttvfig,file,ptsig) ;
       }
     fprintf(file,";\n") ;
    }
   fprintf(file,"\n") ;
  }

 if((ttvfig->NBELCMDSIG + ttvfig->NBILCMDSIG) != 0) 
  {
   for(i = 0 ; i < ttvfig->NBELCMDSIG + ttvfig->NBILCMDSIG ; i++)
    {
     ptype_list *ptype ;
     chain_list *chain ;

     ptsig = (i < ttvfig->NBELCMDSIG) ? *(ttvfig->ELCMDSIG + i)
             : *(ttvfig->ILCMDSIG + i - ttvfig->NBELCMDSIG) ;
     if((type & TTV_FILE_TTV) == TTV_FILE_TTV)
       {
        if((ttv_testmask(ttvfig,ptsig,TTV_NAME_IN) != 0) ||
           (TTV_PATH_SAVE == TTV_SAVE_ALLPATH))
          ptsig->TYPE |= TTV_SIG_MASKIN ;
        if((ttv_testmask(ttvfig,ptsig,TTV_NAME_OUT) != 0) ||
           (TTV_PATH_SAVE == TTV_SAVE_ALLPATH))
          ptsig->TYPE |= TTV_SIG_MASKOUT ;
       }
     ptsig->USER = addptype(ptsig->USER,TTV_CMD_INDEX,(void *)(i+1)) ;
     if((type & (TTV_FILE_TTX | TTV_FILE_DTX)) == 0) 
     continue ;
     if(getptype(ptsig->USER,TTV_SIG_INDEX) == NULL)
       {
        ptsig->USER = addptype(ptsig->USER,TTV_SIG_INDEX,(void *)(i+1)) ;
       }
     if((ptsig->TYPE & TTV_SIG_C) == TTV_SIG_C) typesig = 'C' ;
     else if((ptsig->TYPE & TTV_SIG_N) == TTV_SIG_N) typesig = 'N' ;
     else if(i < ttvfig->NBELCMDSIG) typesig = 'E' ;
     else typesig = 'I' ;
     fprintf(file,"Q %c %ld ",typesig ,i+1) ;
     ttv_printname(ttvfig,file,ptsig) ;
     fprintf(file," ") ;
     ttv_printcaracsig(ttvfig,file,ptsig) ;
     ptype = getptype(ptsig->USER,TTV_SIG_CMDOLD) ;
     if((ptype != NULL) && (ptsig->ROOT == ttvfig))
      {
       fprintf(file," (") ;
       for(chain = (chain_list *)ptype->DATA ; chain != NULL ; 
           chain = chain->NEXT) 
         {
          ttv_printname(ttvfig,file,(ttvsig_list *)chain->DATA) ;
          if(chain->NEXT != NULL)
             fprintf(file," ") ;
         }
       fprintf(file,")") ;
      }
     ttv_printsigproperty(ttvfig,file,ptsig) ;
     fprintf(file,";\n") ;
    }
   if((type & (TTV_FILE_TTX | TTV_FILE_DTX)) != 0) 
     fprintf(file,"\n") ;
  }

 if(((ttvfig->NBEPRESIG + ttvfig->NBIPRESIG) != 0) 
     && ((type & (TTV_FILE_TTV|TTV_FILE_DTV)) != 0))
  {
   for(i = 0 ; i < ttvfig->NBEPRESIG + ttvfig->NBIPRESIG ; i++)
    {
     ptsig = (i < ttvfig->NBEPRESIG) ? *(ttvfig->EPRESIG + i)
             : *(ttvfig->IPRESIG + i - ttvfig->NBEPRESIG) ;
     if((type & TTV_FILE_TTV) == TTV_FILE_TTV)
      {
       if((ttv_testmask(ttvfig,ptsig,TTV_NAME_IN) != 0) ||
          (TTV_PATH_SAVE == TTV_SAVE_ALLPATH))
         ptsig->TYPE |= TTV_SIG_MASKIN ;
       if((ttv_testmask(ttvfig,ptsig,TTV_NAME_OUT) != 0) ||
          (TTV_PATH_SAVE == TTV_SAVE_ALLPATH))
         ptsig->TYPE |= TTV_SIG_MASKOUT ;
      }
     fprintf(file,"P ") ;
     ttv_printname(ttvfig,file,ptsig) ;
     fprintf(file," ()") ;
     fprintf(file,";\n") ;
    }
   fprintf(file,"\n") ;
  }

 if((ttvfig->NBELATCHSIG + ttvfig->NBILATCHSIG) != 0)
  {
   for(i = 0 ; i < ttvfig->NBELATCHSIG + ttvfig->NBILATCHSIG ; i++)
    {
     ttvevent_list *ptnodecmd ;
     ttvsig_list *ptsigcmd ;
     ptype_list *ptypecmd ;
     chain_list *chaincmd ;
     char chaincmd1 ;

     ptsig = (i < ttvfig->NBELATCHSIG) ? *(ttvfig->ELATCHSIG + i)
             : *(ttvfig->ILATCHSIG + i - ttvfig->NBELATCHSIG) ;
     if((type & TTV_FILE_TTV) == TTV_FILE_TTV)
      {
       if((ttv_testmask(ttvfig,ptsig,TTV_NAME_IN) != 0) ||
          (TTV_PATH_SAVE == TTV_SAVE_ALLPATH))
         ptsig->TYPE |= TTV_SIG_MASKIN ;
       if((ttv_testmask(ttvfig,ptsig,TTV_NAME_OUT) != 0) ||
          (TTV_PATH_SAVE == TTV_SAVE_ALLPATH))
         ptsig->TYPE |= TTV_SIG_MASKOUT ;
      }
     if((type & (TTV_FILE_TTX | TTV_FILE_DTX)) != 0)
       {
        if(getptype(ptsig->USER,TTV_SIG_INDEX) == NULL)
          ptsig->USER = addptype(ptsig->USER,TTV_SIG_INDEX,(void *)(i+1)) ;
        else
          getptype(ptsig->USER,TTV_SIG_INDEX)->DATA = (void *)(i+1) ;
        if((ptsig->TYPE & TTV_SIG_LS) == TTV_SIG_LS)
          typesig = 'S' ;
        else if((ptsig->TYPE & TTV_SIG_LR) == TTV_SIG_LR) 
          typesig = 'R' ;
        else
          typesig = ((ptsig->TYPE & TTV_SIG_LF) == TTV_SIG_LF) ? 'F' : 'L' ;
        fprintf(file,"L %c",typesig) ;
        if((ptsig->TYPE & TTV_SIG_NL) == TTV_SIG_NL)
          fprintf(file,"N") ;
        else if((ptsig->TYPE & TTV_SIG_CL) == TTV_SIG_CL)
          fprintf(file,"C") ;
        else if(i < ttvfig->NBELATCHSIG)
          fprintf(file,"E") ;
        else
          fprintf(file,"I") ;
        fprintf(file," %ld ",i+1) ;
        ttv_printname(ttvfig,file,ptsig) ;
        fprintf(file," ") ;
        ttv_printcaracsig(ttvfig,file,ptsig) ;
        ttv_printsigproperty(ttvfig,file,ptsig) ;
       }
     else
       {
        if((ptsig->TYPE & TTV_SIG_LL) == TTV_SIG_LL)
          fprintf(file,"L S ") ;
        else
          fprintf(file,"L F ") ;
        ttv_printname(ttvfig,file,ptsig) ;
       }
     ptypecmd = getptype(ptsig->USER,TTV_SIG_CMD) ;
     if(((type & (TTV_FILE_TTV|TTV_FILE_DTV)) != 0) || (ptypecmd != NULL))
     fprintf(file," (") ;
     chaincmd1 = 'N' ;
     if(ptypecmd != NULL) 
       for(chaincmd = ptypecmd->DATA;chaincmd != NULL;chaincmd = chaincmd->NEXT)
         {
          char levelcmd ;
          ptype_list *ptypecmdindex ;

          ptnodecmd = (ttvevent_list *)chaincmd->DATA ;
          ptsigcmd = ptnodecmd->ROOT ;
          levelcmd = ((ptnodecmd->TYPE&TTV_NODE_UP) == TTV_NODE_UP) ? 'U':'D' ;

          if((ptypecmdindex = getptype(ptsigcmd->USER,TTV_CMD_INDEX)) == NULL)
             {
              continue ;
             }
             
          if((type & (TTV_FILE_TTX | TTV_FILE_DTX)) != 0)
            {
             if(chaincmd1 == 'N')
               {
                fprintf(file,"%ld %c",(long)ptypecmdindex->DATA,levelcmd) ;
                chaincmd1 = 'O' ;
               }
             else
                fprintf(file," %ld %c",(long)ptypecmdindex->DATA,levelcmd) ;
            }
          else
            {
             if(chaincmd1 == 'N')
               {
                fprintf(file," ") ;
                chaincmd1 = 'O' ;
               }
             if((ptnodecmd->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN)
                 fprintf(file,"`") ;
              ttv_printname(ttvfig,file,ptsigcmd) ;
              fprintf(file," ") ;
            }
         }
     if(((type & (TTV_FILE_TTV|TTV_FILE_DTV)) != 0) || (ptypecmd != NULL))
       fprintf(file,");\n") ;
     else
       fprintf(file,";\n") ;
    }
   fprintf(file,"\n") ;
  }

 if(((ttvfig->NBEPRESIG + ttvfig->NBIPRESIG) != 0) 
    && ((type & (TTV_FILE_TTX | TTV_FILE_DTX)) != 0))
  {
   for(i = 0 ; i < ttvfig->NBEPRESIG + ttvfig->NBIPRESIG ; i++)
    {
     ptsig = (i < ttvfig->NBEPRESIG) ? *(ttvfig->EPRESIG + i)
             : *(ttvfig->IPRESIG + i - ttvfig->NBEPRESIG) ;
     if(getptype(ptsig->USER,TTV_SIG_INDEX) == NULL)
        ptsig->USER = addptype(ptsig->USER,TTV_SIG_INDEX,(void *)(i+1)) ;
     else
        getptype(ptsig->USER,TTV_SIG_INDEX)->DATA = (void *)(i+1) ;
     if((ptsig->TYPE & TTV_SIG_NR) == TTV_SIG_NR)
       typesig = 'N' ;
     else if((ptsig->TYPE & TTV_SIG_CR) == TTV_SIG_CR)
       typesig = 'C' ;
     else if(i < ttvfig->NBEPRESIG)
       typesig = 'E' ;
     else
       typesig = 'I' ;
     fprintf(file,"R %c %ld ",typesig,i+1) ;
     ttv_printname(ttvfig,file,ptsig) ;
     fprintf(file," ") ;
     ttv_printcaracsig(ttvfig,file,ptsig) ;
     ttv_printsigproperty(ttvfig,file,ptsig) ;
     fprintf(file,";\n") ;
    }
   fprintf(file,"\n") ;
  }

 if(((ttvfig->NBEBREAKSIG + ttvfig->NBIBREAKSIG) != 0) 
    && ((type & (TTV_FILE_TTX | TTV_FILE_DTX)) != 0))
  {
   for(i = 0 ; i < ttvfig->NBEBREAKSIG + ttvfig->NBIBREAKSIG ; i++)
    {
     ptsig = (i < ttvfig->NBEBREAKSIG) ? *(ttvfig->EBREAKSIG + i)
             : *(ttvfig->IBREAKSIG + i - ttvfig->NBEBREAKSIG) ;
     if(getptype(ptsig->USER,TTV_SIG_INDEX) == NULL)
        ptsig->USER = addptype(ptsig->USER,TTV_SIG_INDEX,(void *)(i+1)) ;
     else
        getptype(ptsig->USER,TTV_SIG_INDEX)->DATA = (void *)(i+1) ;
     if((ptsig->TYPE & TTV_SIG_N) == TTV_SIG_N)
       typesig = 'N' ;
     else if((ptsig->TYPE & TTV_SIG_C) == TTV_SIG_C)
       typesig = 'C' ;
     else if(i < ttvfig->NBEBREAKSIG)
       typesig = 'E' ;
     else
       typesig = 'I' ;
     fprintf(file,"B %c %ld ",typesig,i+1) ;
     ttv_printname(ttvfig,file,ptsig) ;
     fprintf(file," ") ;
     ttv_printcaracsig(ttvfig,file,ptsig) ;
     fprintf(file," ") ;
     ttv_printsigsettings(ttvfig,file,ptsig,type) ;
     ttv_printsigproperty(ttvfig,file,ptsig) ;
     fprintf(file,";\n") ;
    }
   fprintf(file,"\n") ;
  }
 else if(((ttvfig->NBEBREAKSIG + ttvfig->NBIBREAKSIG) != 0)
        && ((type & (TTV_FILE_TTV|TTV_FILE_DTV)) != 0))
  {
   for(i = 0 ; i < ttvfig->NBEBREAKSIG + ttvfig->NBIBREAKSIG ; i++)
    {
     ptsig = (i < ttvfig->NBEBREAKSIG) ? *(ttvfig->EBREAKSIG + i)
             : *(ttvfig->IBREAKSIG + i - ttvfig->NBEBREAKSIG) ;
     if((type & TTV_FILE_TTV) == TTV_FILE_TTV)
      {
       if((ttv_testmask(ttvfig,ptsig,TTV_NAME_IN) != 0) ||
          (TTV_PATH_SAVE == TTV_SAVE_ALLPATH))
         ptsig->TYPE |= TTV_SIG_MASKIN ;
       if((ttv_testmask(ttvfig,ptsig,TTV_NAME_OUT) != 0) ||
          (TTV_PATH_SAVE == TTV_SAVE_ALLPATH))
         ptsig->TYPE |= TTV_SIG_MASKOUT ;
      }
     fprintf(file,"B ") ;
     ttv_printname(ttvfig,file,ptsig) ;
     fprintf(file," ") ;
     ttv_printsigsettings(ttvfig,file,ptsig,type) ;
     fprintf(file,";\n") ;
     }
   fprintf(file,"\n") ;
  }

 if((ttvfig->NBEXTSIG + ttvfig->NBINTSIG) != 0)
  {
   for(i = 0 ; i < ttvfig->NBEXTSIG + ttvfig->NBINTSIG ; i++)
    {
     ptsig = (i < ttvfig->NBEXTSIG) ? *(ttvfig->EXTSIG + i)
             : *(ttvfig->INTSIG + i - ttvfig->NBEXTSIG) ;
     if((type & TTV_FILE_TTV) == TTV_FILE_TTV)
      {
       if((ttv_testmask(ttvfig,ptsig,TTV_NAME_IN) != 0) ||
          (TTV_PATH_SAVE == TTV_SAVE_ALLPATH))
         ptsig->TYPE |= TTV_SIG_MASKIN ;
       if((ttv_testmask(ttvfig,ptsig,TTV_NAME_OUT) != 0) ||
          (TTV_PATH_SAVE == TTV_SAVE_ALLPATH))
         ptsig->TYPE |= TTV_SIG_MASKOUT ;
      }
     if((type & (TTV_FILE_TTX | TTV_FILE_DTX)) != 0)
       {
        ptsig->USER = addptype(ptsig->USER,TTV_SIG_INDEX,(void *)(i+1)) ;
        if(i < ttvfig->NBEXTSIG)
          typesig = 'E' ;
        else
          typesig = 'I' ;
        fprintf(file,"I %c %ld ",typesig,i+1) ;
        ttv_printname(ttvfig,file,ptsig) ;
        fprintf(file," ") ;
        ttv_printcaracsig(ttvfig,file,ptsig) ;
        ttv_printsigproperty(ttvfig,file,ptsig) ;
       }
     else
       {
        fprintf(file,"I ") ;
        ttv_printname(ttvfig,file,ptsig) ;
       }
     fprintf(file,";\n") ;
    }
   fprintf(file,"\n") ;
  }

 if((type & TTV_FILE_DTX) == TTV_FILE_DTX) 
  {
   if(ttvfig->NBESIG != 0)
    {
     for(i = 0 ; i < ttvfig->NBESIG ; i++)
      {
       ptsig = *(ttvfig->ESIG + i) ;
       ptsig->USER = addptype(ptsig->USER,TTV_SIG_INDEX,(void *)(i+1)) ;
       fprintf(file,"S E %ld ",i+1) ;
       ttv_printname(ttvfig,file,ptsig) ;
       fprintf(file," ") ;
       ttv_printcaracsig(ttvfig,file,ptsig) ;
       ttv_printsigproperty(ttvfig,file,ptsig) ;
       fprintf(file,";\n") ;
      }
    }

   if(ttvfig->NBISIG != 0)
    {
     long index = ttvfig->NBESIG + 1 ;
     ttvfig->ISIG = (ttvsbloc_list *)reverse((chain_list *)ttvfig->ISIG) ;
     nbend = ttvfig->NBISIG ;
     ptsbloc = ttvfig->ISIG ;

     for(i = 0 ; i < nbend ; i++)
      {
       j = (TTV_MAX_SBLOC -1) - (i % TTV_MAX_SBLOC) ;
       if((j == (TTV_MAX_SBLOC - 1)) && (i != 0)) ptsbloc = ptsbloc->NEXT ;
       ptsig = ptsbloc->SIG + j ;
       ptsig->USER = addptype(ptsig->USER,TTV_SIG_INDEX,
                                        (void *)(index)) ;
       fprintf(file,"S I %ld ",index) ;
       ttv_printname(ttvfig,file,ptsig) ;
       fprintf(file," ") ;
       ttv_printcaracsig(ttvfig,file,ptsig) ;
       ttv_printsigproperty(ttvfig,file,ptsig) ;
       fprintf(file,";\n") ;
       index++ ;
      }
     ttvfig->ISIG = (ttvsbloc_list *)reverse((chain_list *)ttvfig->ISIG) ;
    }

   fprintf(file,"\n") ;
  }

 if((type & (TTV_FILE_TTX | TTV_FILE_TTV)) != 0)
  {
   if((((type & TTV_FILE_TTV) == TTV_FILE_TTV) && 
      (TTV_PATH_SAVE != TTV_NOTSAVE_PATH)) ||
      ((type & TTV_FILE_TTX) == TTV_FILE_TTX))
    {
     ttv_printline(ttvfig,ttvfig,file,TTV_LINE_J,type,find) ;
     ttv_printline(ttvfig,ttvfig,file,TTV_LINE_P,type,find) ;
     ttv_printline(ttvfig,ttvfig,file,TTV_LINE_T,type,find) ;
    }
  }
 if((type & (TTV_FILE_DTX | TTV_FILE_DTV)) != 0)
  {
   ttv_printline(ttvfig,ttvfig,file,TTV_LINE_E,type,find) ;
   ttv_printline(ttvfig,ttvfig,file,TTV_LINE_F,type,find) ;
   ttv_printline(ttvfig,ttvfig,file,TTV_LINE_D,type,find) ;
  }

 if((type & (TTV_FILE_DTX | TTV_FILE_TTX)) != 0)
    fprintf(file,"\nG;\n") ;
 else
    fprintf(file,"\nE;\n") ;

 if(fclose(file) != 0)
    {
     char straux[128] ;

     sprintf(straux,"%s.%s",ttvfig->INFO->FIGNAME,filetype) ;
     ttv_error(21,straux,TTV_WARNING) ;
    }

 if((type & (TTV_FILE_TTX | TTV_FILE_DTX)) != 0) 
   {
    if(ttvfig->NBCONSIG != 0)
     {
      for(i = 0 ; i < ttvfig->NBCONSIG ; i++)
       {
        ptsig = *(ttvfig->CONSIG + i) ;
        ptsig->USER = delptype(ptsig->USER,TTV_SIG_INDEX) ;
       }
     }

    if(ttvfig->NBNCSIG != 0)
     {
      for(i = 0 ; i < ttvfig->NBNCSIG ; i++)
       {
        ptsig = *(ttvfig->NCSIG + i) ;
        ptsig->USER = delptype(ptsig->USER,TTV_SIG_INDEX) ;
       }
     }

    if((ttvfig->NBELCMDSIG + ttvfig->NBILCMDSIG) != 0)
     {
      for(i = 0 ; i < ttvfig->NBELCMDSIG + ttvfig->NBILCMDSIG ; i++)
       {
        ptsig = (i < ttvfig->NBELCMDSIG) ? *(ttvfig->ELCMDSIG + i)
                : *(ttvfig->ILCMDSIG + i - ttvfig->NBELCMDSIG) ;
        if(getptype(ptsig->USER,TTV_SIG_INDEX) != NULL)
           ptsig->USER = delptype(ptsig->USER,TTV_SIG_INDEX) ;
        ptsig->USER = delptype(ptsig->USER,TTV_CMD_INDEX) ;
       }
     }

    if((ttvfig->NBELATCHSIG + ttvfig->NBILATCHSIG) != 0)
     {
      for(i = 0 ; i < ttvfig->NBELATCHSIG + ttvfig->NBILATCHSIG ; i++)
       {
        ptsig = (i < ttvfig->NBELATCHSIG) ? *(ttvfig->ELATCHSIG + i)
                : *(ttvfig->ILATCHSIG + i - ttvfig->NBELATCHSIG) ;
        if(getptype(ptsig->USER,TTV_SIG_INDEX) != NULL)
          ptsig->USER = delptype(ptsig->USER,TTV_SIG_INDEX) ;
       }
     }


    if((ttvfig->NBEPRESIG + ttvfig->NBIPRESIG) != 0)
     {
      for(i = 0 ; i < ttvfig->NBEPRESIG + ttvfig->NBIPRESIG ; i++)
       {
        ptsig = (i < ttvfig->NBEPRESIG) ? *(ttvfig->EPRESIG + i)
                : *(ttvfig->IPRESIG + i - ttvfig->NBEPRESIG) ;
        if(getptype(ptsig->USER,TTV_SIG_INDEX) != NULL)
          ptsig->USER = delptype(ptsig->USER,TTV_SIG_INDEX) ;
       }
     }

    if((ttvfig->NBEBREAKSIG + ttvfig->NBIBREAKSIG) != 0)
     {
      for(i = 0 ; i < ttvfig->NBEBREAKSIG + ttvfig->NBIBREAKSIG ; i++)
       {
        ptsig = (i < ttvfig->NBEBREAKSIG) ? *(ttvfig->EBREAKSIG + i)
                : *(ttvfig->IBREAKSIG + i - ttvfig->NBEBREAKSIG) ;
        if(getptype(ptsig->USER,TTV_SIG_INDEX) != NULL)
          ptsig->USER = delptype(ptsig->USER,TTV_SIG_INDEX) ;
       }
     }

    if((ttvfig->NBEXTSIG + ttvfig->NBINTSIG) != 0)
     {
      for(i = 0 ; i < ttvfig->NBEXTSIG + ttvfig->NBINTSIG ; i++)
       {
        ptsig = (i < ttvfig->NBEXTSIG) ? *(ttvfig->EXTSIG + i)
                : *(ttvfig->INTSIG + i - ttvfig->NBEXTSIG) ;
        ptsig->USER = delptype(ptsig->USER,TTV_SIG_INDEX) ;
       }
     }

    if((type & TTV_FILE_DTX) == TTV_FILE_DTX) 
     {
      if(ttvfig->NBESIG != 0)
       {
        for(i = 0 ; i < ttvfig->NBESIG ; i++)
         {
          ptsig = *(ttvfig->ESIG + i) ;
          ptsig->USER = delptype(ptsig->USER,TTV_SIG_INDEX) ;
         }
       }

      if(ttvfig->NBISIG != 0)
       {
        ptsbloc = ttvfig->ISIG ;
        nbfree = (TTV_MAX_SBLOC - (ttvfig->NBISIG % TTV_MAX_SBLOC)) 
                  % TTV_MAX_SBLOC ;
        nbend = ttvfig->NBISIG + nbfree ;

        for(i = nbfree ; i < nbend ; i++)
         {
          j = i % TTV_MAX_SBLOC ;
          if((j == 0) && (i != 0)) ptsbloc = ptsbloc->NEXT ;
          ptsig = ptsbloc->SIG + j ;
          ptsig->USER = delptype(ptsig->USER,TTV_SIG_INDEX) ;
         }
       }
     }
   }

 if(((ttvfig->NBELCMDSIG + ttvfig->NBILCMDSIG)!= 0) 
    && ((type & (TTV_FILE_TTV|TTV_FILE_DTV)) != 0)) 
  {
   for(i = 0 ; i < ttvfig->NBELCMDSIG + ttvfig->NBILCMDSIG ; i++)
    {
     ptsig = (i < ttvfig->NBELCMDSIG) ? *(ttvfig->ELCMDSIG + i)
             : *(ttvfig->ILCMDSIG + i - ttvfig->NBELCMDSIG) ;
     ptsig->USER = delptype(ptsig->USER,TTV_CMD_INDEX) ;
    }
  }

 if((type & TTV_FILE_TTV) == TTV_FILE_TTV)
   {
    if(ttvfig->NBCONSIG != 0)
     {
      for(i = 0 ; i < ttvfig->NBCONSIG ; i++)
       {
        (*(ttvfig->CONSIG + i))->TYPE &= ~(TTV_SIG_MASKIN|TTV_SIG_MASKOUT) ;
       }
     }

    if(ttvfig->NBNCSIG != 0)
     {
      for(i = 0 ; i < ttvfig->NBNCSIG ; i++)
       {
        (*(ttvfig->NCSIG + i))->TYPE &= ~(TTV_SIG_MASKIN|TTV_SIG_MASKOUT) ;
       }
     }

    if((ttvfig->NBELCMDSIG + ttvfig->NBILCMDSIG) != 0)
     {
      for(i = 0 ; i < ttvfig->NBELCMDSIG + ttvfig->NBILCMDSIG ; i++)
       {
        ptsig = (i < ttvfig->NBELCMDSIG) ? *(ttvfig->ELCMDSIG + i)
                : *(ttvfig->ILCMDSIG + i - ttvfig->NBELCMDSIG) ;
        ptsig->TYPE &= ~(TTV_SIG_MASKIN|TTV_SIG_MASKOUT) ;
       }
     }


    if((ttvfig->NBELATCHSIG + ttvfig->NBILATCHSIG) != 0)
     {
      for(i = 0 ; i < ttvfig->NBELATCHSIG + ttvfig->NBILATCHSIG ; i++)
       {
        ptsig = (i < ttvfig->NBELATCHSIG) ? *(ttvfig->ELATCHSIG + i)
                : *(ttvfig->ILATCHSIG + i - ttvfig->NBELATCHSIG) ;
        ptsig->TYPE &= ~(TTV_SIG_MASKIN|TTV_SIG_MASKOUT) ;
       }
     }

    if((ttvfig->NBEPRESIG + ttvfig->NBIPRESIG) != 0)
     {
      for(i = 0 ; i < ttvfig->NBEPRESIG + ttvfig->NBIPRESIG ; i++)
       {
        ptsig = (i < ttvfig->NBEPRESIG) ? *(ttvfig->EPRESIG + i)
                : *(ttvfig->IPRESIG + i - ttvfig->NBEPRESIG) ;
        ptsig->TYPE &= ~(TTV_SIG_MASKIN|TTV_SIG_MASKOUT) ;
       }
     }

    if((ttvfig->NBEBREAKSIG + ttvfig->NBIBREAKSIG) != 0)
     {
      for(i = 0 ; i < ttvfig->NBEBREAKSIG + ttvfig->NBIBREAKSIG ; i++)
       {
        ptsig = (i < ttvfig->NBEBREAKSIG) ? *(ttvfig->EBREAKSIG + i)
                : *(ttvfig->IBREAKSIG + i - ttvfig->NBEBREAKSIG) ;
        ptsig->TYPE &= ~(TTV_SIG_MASKIN|TTV_SIG_MASKOUT) ;
       }
     }

    if((ttvfig->NBEXTSIG + ttvfig->NBINTSIG) != 0)
     {
      for(i = 0 ; i < ttvfig->NBEXTSIG + ttvfig->NBINTSIG ; i++)
       {
        ptsig = (i < ttvfig->NBEXTSIG) ? *(ttvfig->EXTSIG + i)
                : *(ttvfig->INTSIG + i - ttvfig->NBEXTSIG) ;
        ptsig->TYPE &= ~(TTV_SIG_MASKIN|TTV_SIG_MASKOUT) ;
       }
     }
   }
 return ;
}

/*****************************************************************************/
/*                        function ttv_drittvold()                           */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* type : type de fichier ttv dtv ttx ttx                                    */
/* find : type de recherche hierarchique ou pas                              */
/*                                                                           */
/* ecrit sur disque les anciens fichier ttv et dtv                           */
/*****************************************************************************/
void ttv_drittvold(ttvfig,type,find)
ttvfig_list *ttvfig ;
long type ;
long find ;
{
 ttvsig_list *ptsig ;
 ttvsig_list *ptsigx ;
 ttvpath_list *ptpathmax = NULL ;
 ttvpath_list *ptpathmin = NULL ;
 ttvpath_list *path ;
 ttvpath_list *pathx ;
 chain_list *chain ;
 chain_list *chainx ;
 chain_list *chainnode ;
 chain_list *chainnodex ;
 ptype_list *ptype ;
 FILE *file ;
 long frs ;
 float rs ;
 char filetype[4] ;
 char typedelay[4] ;
 char fin ;
 char fout ;
 char typepath ;

 if((type & (TTV_FILE_TTV|TTV_FILE_DTV)) == 0)
  return ;

 if((type & TTV_FILE_TTV) == TTV_FILE_TTV)
  {
   strcpy(filetype,"ttv") ;
   if((((ttvfig->STATUS & TTV_STS_TTX) != TTV_STS_TTX) &&
       ((find & TTV_FIND_LINE) != TTV_FIND_LINE)) ||
      (((ttvfig->STATUS & TTV_STS_DTX) != TTV_STS_DTX) &&
       ((find & TTV_FIND_LINE) == TTV_FIND_LINE)))
       {
        char straux[128] ;
 
        sprintf(straux,"%s.%s",ttvfig->INFO->FIGNAME,filetype) ;
        ttv_error(22,straux,TTV_WARNING) ;
        return ;
       }
   file = ttv_openfile(ttvfig,TTV_FILE_TTV,WRITE_TEXT) ;
   ptpathmax = ttv_getcriticpath(ttvfig,NULL,TTV_NAME_IN,TTV_NAME_OUT,
                                 TTV_DELAY_MAX,TTV_DELAY_MIN,1,
                                 TTV_FIND_MAX|(find & ~(TTV_FIND_NOTSAVE))) ;
   ptpathmin = ttv_getcriticpath(ttvfig,NULL,TTV_NAME_IN,TTV_NAME_OUT,
                                 TTV_DELAY_MAX,TTV_DELAY_MIN,1,
                                 TTV_FIND_MIN|(find & ~(TTV_FIND_NOTSAVE))) ;
   if((find & TTV_FIND_NOTSAVE) == TTV_FIND_NOTSAVE)
     {
      if(ptpathmax != NULL)
        ptpathmax->TYPE |= TTV_FIND_NOTSAVE ;
      if(ptpathmin != NULL)
        ptpathmin->TYPE |= TTV_FIND_NOTSAVE ;
     }
  }
 else 
  {
   strcpy(filetype,"dtv") ;
   if((ttvfig->STATUS & TTV_STS_DTX) != TTV_STS_DTX)
       {
        char straux[128] ;
 
        sprintf(straux,"%s.%s",ttvfig->INFO->FIGNAME,filetype) ;
        ttv_error(22,straux,TTV_WARNING) ;
        return ;
       }
   file = ttv_openfile(ttvfig,TTV_FILE_DTV,WRITE_TEXT) ;
  }

 ttv_printhead(ttvfig,file,type,ptpathmax,ptpathmin) ;
 ttv_freepathlist(ptpathmax) ;
 ttv_freepathlist(ptpathmin) ;
 fprintf(file,"\n") ;
 fprintf(file,"\n") ;
 fprintf(file,"G %s %s %ld %s %s (%d %d %d %d %d %d) %ld *;\n\n",
         ttvfig->INFO->FIGNAME,ttvfig->INFO->TECHNONAME,(long)(ttvfig->INFO->SLOPE/TTV_UNIT),
         ttvfig->INFO->TOOLNAME,ttvfig->INFO->TOOLVERSION,ttvfig->INFO->TTVDAY,
         ttvfig->INFO->TTVMONTH,ttvfig->INFO->TTVYEAR,ttvfig->INFO->TTVHOUR,
         ttvfig->INFO->TTVMIN,ttvfig->INFO->TTVSEC,ttvfig->INFO->LEVEL) ;

 if(file == NULL)
   {
    char straux[128] ;

    sprintf(straux,"%s.%s",ttvfig->INFO->FIGNAME,filetype) ;
    ttv_error(20,straux,TTV_ERROR) ;
   }

 if((chain = ttv_getsigbytype(ttvfig,ttvfig,TTV_SIG_C,NULL)) != NULL)
  {
   for(chainx = chain ; chainx != NULL ; chainx = chainx->NEXT)
    {
     char dir ;

     ptsig = (ttvsig_list*)chainx->DATA ;

     fprintf(file,"X %c ",(((dir = ttv_getdir(ptsig)) == 'X') ? 'U' : dir)) ;
     ttv_printname(ttvfig,file,ptsig) ;
     fprintf(file," %ld",(long)(ptsig->CAPA)) ;
     fprintf(file,";\n") ;
    }
   fprintf(file,"\n") ;
   freechain(chain) ;
  }

 if((chain = ttv_getsigbytype(ttvfig,NULL,TTV_SIG_R,NULL)) != NULL)
  {
   for(chainx = chain ; chainx != NULL ; chainx = chainx->NEXT)
    {
     ptsig = (ttvsig_list*)chainx->DATA ;

     fprintf(file,"P ") ;
     ttv_printname(ttvfig,file,ptsig) ;
     fprintf(file," ()") ;
     fprintf(file,";\n") ;
    }
   fprintf(file,"\n") ;
   freechain(chain) ;
  }

 if((chain = ttv_getsigbytype(ttvfig,NULL,TTV_SIG_L,NULL)) != NULL)
  {
   for(chainx = chain ; chainx != NULL ; chainx = chainx->NEXT)
    {
     ttvevent_list *ptnodecmd ;
     ttvsig_list *ptsigcmd ;
     chain_list *chaincmd ;
     chain_list *chaincmdx ;
     char chaincmd1 ;

     ptsig = (ttvsig_list*)chainx->DATA ;

     if((ptsig->TYPE & TTV_SIG_LL) == TTV_SIG_LL)
       fprintf(file,"L S ") ;
     else
       fprintf(file,"L F ") ;
     ttv_printname(ttvfig,file,ptsig) ;
     fprintf(file," (") ;
     chaincmd1 = 'N' ;
     if((chaincmd = ttv_getlrcmd(ttvfig,ptsig)) != NULL)
      {
       for(chaincmdx = chaincmd ;chaincmdx != NULL;chaincmdx = chaincmdx->NEXT)
         {
          ptnodecmd = (ttvevent_list *)chaincmdx->DATA ;
          ptsigcmd = ptnodecmd->ROOT ;
  
          if(chaincmd1 == 'N')
            {
             fprintf(file," ") ;
             chaincmd1 = 'O' ;
            }
          if((ptnodecmd->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN)
              fprintf(file,"`") ;
           ttv_printname(ttvfig,file,ptsigcmd) ;
           fprintf(file," ") ;
         }
       freechain(chaincmd) ;
      }
     fprintf(file,");\n") ;
    }
   fprintf(file,"\n") ;
   freechain(chain) ;
  }

/*
 if((chain = ttv_getsigbytype(ttvfig,NULL,TTV_SIG_B,NULL)) != NULL)
  {
   for(chainx = chain ; chainx != NULL ; chainx = chainx->NEXT)
    {
     ptsig = (ttvsig_list*)chainx->DATA ;

     fprintf(file,"B ") ;
     ttv_printname(ttvfig,file,ptsig) ;
     fprintf(file,";\n") ;
    }
   fprintf(file,"\n") ;
   freechain(chain) ;
  }
*/

 if((type & TTV_FILE_TTV) == TTV_FILE_TTV)
  {
   chain = ttv_getsigbytype(ttvfig,NULL,TTV_SIG_C|TTV_SIG_Q|TTV_SIG_L|
                               TTV_SIG_R,TTV_NAME_OUT) ;
   if(TTV_PATH_SAVE != TTV_NOTSAVE_PATH)
   for(chainx = chain ; chainx != NULL ; chainx = chainx->NEXT)
    {
     ptsig = (ttvsig_list*)chainx->DATA ;

     if(TTV_PATH_SAVE != TTV_SAVE_ALLPATH)
      {
       if((TTV_PATH_SAVE == TTV_SAVE_PATH) && 
          (ttv_testmask(ttvfig,ptsig,TTV_NAME_OUT) == 0))
       continue ;
      }

     if((ptsig->TYPE & TTV_SIG_C) == TTV_SIG_C)
       {
        if((ptsig->TYPE & TTV_SIG_CO) != TTV_SIG_CO)
          continue ;
       }
     path = ttv_getpath(ttvfig,NULL,ptsig,TTV_NAME_IN,TTV_DELAY_MAX,
                        TTV_DELAY_MIN,TTV_FIND_MAX|TTV_FIND_NOTCLASS|
                                      (find & ~(TTV_FIND_NOTSAVE))) ;
     pathx = ttv_getpath(ttvfig,NULL,ptsig,TTV_NAME_IN,TTV_DELAY_MAX,
                         TTV_DELAY_MIN,TTV_FIND_MIN|TTV_FIND_NOTCLASS|
                                       (find & ~(TTV_FIND_NOTSAVE))) ;
     path = (ttvpath_list *)append((chain_list *)path,(chain_list *)pathx) ;
     chainnode = NULL ;
     for(pathx = path ; pathx != NULL ; pathx = pathx->NEXT )
      {
       pathx->TYPE &= ~(TTV_FIND_MARQUE) ;
       ptsigx =  pathx->NODE->ROOT ;
       if(TTV_PATH_SAVE != TTV_SAVE_ALLPATH)
        {
         if((TTV_PATH_SAVE == TTV_SAVE_PATH) &&
            (ttv_testmask(ttvfig,ptsigx,TTV_NAME_IN) == 0))
         continue ;
        }
       if((ptsigx->TYPE & TTV_SIG_MINSIG) != TTV_SIG_MINSIG)
        {
         ptsigx->TYPE |= TTV_SIG_MINSIG ;
         ptsigx->USER = addptype(ptsigx->USER,TTV_SIG_INSIG,
                                 (void *)addchain(NULL,(void *)pathx)) ;
         chainnode = addchain(chainnode,(void *)ptsigx) ;
        }
       else
        {
         ptype = getptype(ptsigx->USER,TTV_SIG_INSIG) ;
         ptype->DATA = (void *)addchain(ptype->DATA,(void *)pathx) ;
        }
      }
     path = NULL ;
     for(chainnodex = chainnode ; chainnodex != NULL ; 
         chainnodex = chainnodex->NEXT)
      {
       chain_list *chainpath ;
       chain_list *chainpathx ;

       ptsigx = (ttvsig_list *)chainnodex->DATA ;
       ptype = getptype(ptsigx->USER,TTV_SIG_INSIG) ;
       for(chainpath = (chain_list *)ptype->DATA ; chainpath != NULL ; 
           chainpath = chainpath->NEXT)
        {
         pathx = (ttvpath_list *)chainpath->DATA ;
         if((pathx->TYPE & TTV_FIND_MARQUE) == TTV_FIND_MARQUE)
           continue ;
         pathx->NEXT = path ;  
         path = pathx ;
         path->TYPE |= TTV_FIND_MARQUE ;
         for(chainpathx = chainpath ;  chainpathx != NULL ; 
             chainpathx = chainpathx->NEXT)
          {
           pathx = (ttvpath_list *)chainpath->DATA ;
           if((pathx->TYPE & TTV_FIND_MARQUE) == TTV_FIND_MARQUE)
            continue ;
           if((pathx->NODE == path->NODE) && (pathx->ROOT == path->ROOT) &&
             ((pathx->TYPE & (TTV_FIND_PR|TTV_FIND_EV|TTV_FIND_HZ)) == 
              (path->TYPE & (TTV_FIND_PR|TTV_FIND_EV|TTV_FIND_HZ))))
            {
             pathx->NEXT = path ;  
             path = pathx ;
             path->TYPE |= TTV_FIND_MARQUE ;
             break ;
            }
          }
        }
      }
     for(chainnodex = chainnode ; chainnodex != NULL ; 
         chainnodex = chainnodex->NEXT)
      {
       ptsigx = (ttvsig_list *)chainnodex->DATA ;
       ptsigx->TYPE &= ~(TTV_SIG_MINSIG) ;
       ptype = getptype(ptsigx->USER,TTV_SIG_INSIG) ;
       freechain((chain_list *)ptype->DATA) ;
       ptsigx->USER = delptype(ptsigx->USER,TTV_SIG_INSIG) ;
      }
     freechain(chainnode) ;
     for(pathx = path ; pathx != NULL ; pathx = pathx->NEXT )
      {
       pathx->TYPE &= ~(TTV_FIND_MARQUE) ;
      }
     if((ptsig->TYPE & TTV_SIG_R) == TTV_SIG_R)
      {
       chainnode = NULL ;
       chainnodex = NULL ;
       for(pathx = path ; pathx != NULL ; pathx = pathx->NEXT )
        {
         if((ptsig->TYPE & TTV_SIG_R) == TTV_SIG_R)
          {
           if((pathx->TYPE & TTV_FIND_PR) == TTV_FIND_PR)
            chainnode = addchain(chainnode,pathx) ;
           else
            chainnodex = addchain(chainnodex,pathx) ;
          }
        }
       chainnode = append(chainnode,chainnodex) ;
       path = NULL ;
       for(chainnodex = chainnode ; chainnodex != NULL ; 
           chainnodex = chainnodex->NEXT)
        {
         pathx = (ttvpath_list *)chainnodex->DATA ;
         pathx->NEXT = path ;
         path = pathx ;
        }
       freechain(chainnode) ;
      }
     pathx = path ;
     while(pathx != NULL)
      {
       ptsig = pathx->NODE->ROOT ;
       ptsigx = pathx->ROOT->ROOT ;
       if((ptsigx->TYPE & TTV_SIG_R) != TTV_SIG_R)
        {
         fprintf(file,"T X X ") ;
         typepath = 'X' ;
        }
       else
        {
         if((pathx->TYPE & TTV_FIND_PR) == TTV_FIND_PR)
          {
           fprintf(file,"T P P ") ;
           typepath = 'P' ;
          }
         else
          {
           fprintf(file,"T E E ") ;
           typepath = 'E' ;
          }
        }
       ttv_printname(ttvfig,file,ptsig) ;
       fprintf(file," ") ;
       ttv_printname(ttvfig,file,ptsigx) ;
       fprintf(file," (") ;
       for(; pathx != NULL ; pathx = pathx->NEXT)
        {
         if(pathx->NODE->ROOT != ptsig) break ;
         if(typepath != 'X')
          {
           if((typepath == 'E') &&
             ((pathx->TYPE & TTV_FIND_EV) != TTV_FIND_EV))
             break ; ;
           if((typepath == 'P') &&
             ((pathx->TYPE & TTV_FIND_PR) != TTV_FIND_PR))
             break ; ;
          }
         if((pathx->NODE->TYPE & TTV_NODE_UP) == TTV_NODE_UP) fin = 'U' ;
         else fin = 'D' ;
 
         if((pathx->TYPE & TTV_FIND_HZ) == TTV_FIND_HZ) fout = 'Z' ;
         else if((pathx->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
                 fout = 'U' ;
         else fout = 'D' ;
         if((pathx->TYPE & TTV_FIND_MAX) == TTV_FIND_MAX)
           strcpy(typedelay,"MAX") ;
         else
           strcpy(typedelay,"MIN") ;
         if(pathx->CMD != NULL)
          {
           fprintf(file,"\n\t(") ;
           ttv_printname(ttvfig,file,pathx->CMD->ROOT) ;
          }
         else
          {
           fprintf(file,"\n\t(*") ;
          }
         if((ptsigx->TYPE & TTV_SIG_C) == TTV_SIG_C)
          {
           if(fout == 'U')
             {
              if((pathx->TYPE & TTV_FIND_MAX) == TTV_FIND_MAX)
               {
                rs = 0.0 ;
                frs = 0 ;
               }
              else
              {
               rs = 0.0 ;
               frs = 0 ;
              }
             }
           else if(fout == 'D')
             {
              if((pathx->TYPE & TTV_FIND_MAX) == TTV_FIND_MAX)
               {
                rs = 0.0 ;
                frs = 0 ;
               }
              else
               {
                rs = 0.0 ;
                frs = 0 ;
               }
             }
           else
             {
              rs = 0.0 ;
              frs = (long)0 ;
             }
           fprintf(file," (%c%c %s %ld %ld %.2f 0 %ld))",
                   fin,fout,typedelay,(long)(pathx->DELAY/TTV_UNIT),(long)(pathx->SLOPE/TTV_UNIT),rs,frs) ;
          }
         else
          {
           fprintf(file," (%c%c %s %ld %ld))",
                   fin,fout,typedelay,(long)(pathx->DELAY/TTV_UNIT),(long)(pathx->SLOPE/TTV_UNIT)) ;
          }
         if((pathx->DELAY <= TTV_SEUIL_MAX) &&
            (pathx->DELAY >= TTV_SEUIL_MIN))
             ttv_printcritic(ttvfig,file,pathx->ROOT,
                             pathx->NODE,pathx->TYPE|find) ;
        }
       fprintf(file,"\n);\n") ;
      }
     ttv_freepathlist(path) ;
    }
   freechain(chain) ;
  }
 else if((type & TTV_FILE_DTV) == TTV_FILE_DTV)
  {
   chain = ttv_getttvfiglist(ttvfig) ;
   for(chainx = chain ; chainx != NULL ; chainx = chainx->NEXT)
    {
     ttv_parsttvfig((ttvfig_list *)chainx->DATA,TTV_STS_LS_FE|TTV_STS_DENOTINPT,
                                                TTV_FILE_DTX) ;
     if((ttvfig_list *)chainx->DATA == ttvfig)
      {
       ttv_printline((ttvfig_list *)chainx->DATA,ttvfig,file,
                     TTV_LINE_D,type,find) ;
      }
     ttv_printline((ttvfig_list *)chainx->DATA,ttvfig,file,
                   TTV_LINE_E,type,find) ;
     ttv_printline((ttvfig_list *)chainx->DATA,ttvfig,file,
                   TTV_LINE_F,type,find) ;
    }
   freechain(chain) ;
  }

 fprintf(file,"\nE;\n") ;

 if(fclose(file) != 0)
    {
     char straux[128] ;

     sprintf(straux,"%s.%s",ttvfig->INFO->FIGNAME,filetype) ;
     ttv_error(21,straux,TTV_WARNING) ;
    }

 return ;
}

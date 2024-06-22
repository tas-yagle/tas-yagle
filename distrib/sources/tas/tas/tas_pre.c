/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TAS Version 5                                               */
/*    Fichier : tas_pre.c                                                   */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Amjad HAJJAR et Payam KIANI                               */
/*                Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/
/* les pre-traitements avant le calcul des temps                            */
/* (switch-cmos ou chemins paralleles, etc...)                              */
/****************************************************************************/

#include "tas.h"

//extern int   infAddList          __P(( char*, long, char, void * ));
//extern int   ttv_jokersubst      __P(( char*, char*, char ));
//extern char* yagGetName          __P(( losig_list* ));

/*****************************************************************************/
/*                        function tas_setsigname()                          */
/* met les noms de signals dans les losig pour rcx                           */
/*****************************************************************************/
void tas_setsigname(lofig)
     lofig_list *lofig ;
{
  chain_list *chain ;
  ptype_list *ptype ;
  locon_list *locon ;
  losig_list *losig ;
  char *name ;
  inffig_list *ifl;

  ifl=getloadedinffig(lofig->NAME);
 
  for(losig = lofig->LOSIG ; losig != NULL ; losig = losig->NEXT)
    {
      if(losig->TYPE == EXTERNAL)
        {
          ptype = getptype(losig->USER,LOFIGCHAIN) ;
          if(ptype != NULL)
            {
              for(chain = (chain_list *)ptype->DATA ; chain != NULL ; 
                  chain = chain->NEXT)
                {
                  locon = (locon_list *)chain->DATA ;
                  if(locon->TYPE == EXTERNAL)
                    {
                      name = locon->NAME ;
                      break ;
                    }
                }
              if(chain == NULL)
                name = yagGetName(ifl, losig) ;
            }
          else
            name = yagGetName(ifl, losig) ;
        }
      else
        {
          name = yagGetName(ifl, losig) ;
        }
      if(losig->NAMECHAIN == NULL)
        losig->NAMECHAIN = addchain(losig->NAMECHAIN,name) ;
      else if((losig->NAMECHAIN->NEXT != NULL) || 
              (losig->NAMECHAIN->DATA != name))
        {
          freechain(losig->NAMECHAIN) ;
          losig->NAMECHAIN = addchain(NULL,name) ;
        }
    }
}

/*****************************************************************************/
/*                        function tas_compelmdelay()                        */
/* compre les delais d'elmore des signaux                                    */
/*****************************************************************************/
int tas_compelmdelay(losig1,losig2)
     losig_list **losig1 ;
     losig_list **losig2 ;
{
  ptype_list *ptype1 ;
  ptype_list *ptype2 ;

  ptype1 = getptype((*losig1)->USER,TAS_SIG_MAXELM) ;
  ptype2 = getptype((*losig2)->USER,TAS_SIG_MAXELM) ;

  return((int)((long)ptype2->DATA - (long)ptype1->DATA)) ;
}

/*****************************************************************************/
/*                        function tas_testelmore()                          */
/* verifie la validité du delai d'elmore                                     */
/*****************************************************************************/
void tas_testelmore(lofig)
     lofig_list *lofig ;
{
  losig_list *losig ;
  locon_list *locon ;
  ptype_list *ptype ;
  chain_list *chain ;
  chain_list *chainsig = NULL ;
  losig_list **tablosig ;
  char message[1024] ;
  long nbemet ;
  long nbrec ;
  long max ;
  long min ;
  int nb ;
  inffig_list *ifl;
 
  if(lofig == NULL)
    return ;

  ifl=getloadedinffig(lofig->NAME);

  for(losig = lofig->LOSIG ; losig != NULL ; losig = losig->NEXT)
    {
      if(losig->TYPE == TAS_ALIM)
        continue ;
      nbemet = (long)0 ;
      nbrec = (long)0 ;
      ptype = getptype(losig->USER,TAS_SIG_NORCDELAY) ;
      if(ptype != NULL)
        continue ;
      if(getrcx(losig) == NULL)
        continue ;
      ptype = getptype(losig->USER,LOFIGCHAIN) ;
      if(ptype == NULL)
        continue ;
      for(chain = (chain_list *)ptype->DATA ; chain != NULL ; chain = chain->NEXT)
        {
          locon = (locon_list *)chain->DATA ;
          if(locon->TYPE == 'T')
            {
              if(((lotrs_list *)locon->ROOT)->GRID == locon)
                {
                  nbrec++ ;
                  locon->DIRECTION = IN ;
                }
              else
                {
                  nbemet++ ;
                  locon->DIRECTION = OUT ;
                }
            } 
          else if(locon->TYPE == EXTERNAL)
            {
              if((locon->DIRECTION == CNS_O) || (locon->DIRECTION == CNS_B) ||
                 (locon->DIRECTION == CNS_Z) || (locon->DIRECTION == CNS_T))
                nbrec++ ;
              else
                nbemet++ ;
            }
          else
            {
              if((locon->DIRECTION == CNS_O) || (locon->DIRECTION == CNS_B) ||
                 (locon->DIRECTION == CNS_Z) || (locon->DIRECTION == CNS_T))
                nbemet++ ;
              else
                nbrec++ ;
            }
        }
      if((nbemet > TAS_NBEMETMAX) || (nbrec > TAS_NBRECMAX) || 
         ((nbemet + nbrec) > TAS_NBCONMAX))
        {
          if(getptype(losig->USER,TAS_SIG_MAXELM) != NULL)
            chainsig = addchain(chainsig,losig) ;
        }
    }

  nb = 0 ;
  for(chain = chainsig ; chain != NULL ; chain = chain->NEXT)
    nb++ ;

  if(nb == 0)
    {
      tablosig = (losig_list **)mbkalloc(nb * sizeof(losig_list *)) ;

      nb = 0 ;
      for(chain = chainsig ; chain != NULL ; chain = chain->NEXT)
        {
          *(tablosig + nb) = (losig_list *)chain->DATA ;
          nb++ ;
        }

      qsort(tablosig,nb,sizeof(losig_list *),(int (*)(const void*,const void*))tas_compelmdelay)  ;

 
      nb = 0 ;

      for(chain = chainsig ; chain != NULL ; chain = chain->NEXT)
        {
          chain->DATA = (void *)(*(tablosig + nb)) ;
          nb++ ;
        }

      mbkfree(tablosig) ;
    }

  for(chain = chainsig ; chain != NULL ; chain = chain->NEXT)
    {
      losig = (losig_list *)chain->DATA ;
      max = (long)getptype(losig->USER,TAS_SIG_MAXELM)->DATA ;
      if((ptype = getptype(losig->USER,TAS_SIG_MINELM)) != NULL)
        min = (long)ptype->DATA ;
      else
        min = (long) 0 ;
      if(((max - min) > TAS_DELTADELAY) && 
         (max > TAS_MAXDELAY) &&
         ((double)((double)(max - min) / (double)max) > (double)(TAS_DIFFDELAY/100)))
        {
          sprintf(message,"%s\nmax delay = %ld delta delay = %ld",
                  yagGetName(ifl,losig),max,max-min) ;
          tas_error(66,message,TAS_WARNING) ;
        }
    }

  if(chainsig != NULL)
    freechain(chainsig) ;

  for(losig = lofig->LOSIG ; losig != NULL ; losig = losig->NEXT)
    {
      if(getptype(losig->USER,TAS_SIG_MINELM) != NULL)
        losig->USER = delptype(losig->USER,TAS_SIG_MINELM) ;
      if(getptype(losig->USER,TAS_SIG_MAXELM) != NULL)
        losig->USER = delptype(losig->USER,TAS_SIG_MAXELM) ;
    }
}

/*****************************************************************************/
/*                        function tas_parafactor()                          */
/* multiplie les capas par un facteur correctif                              */
/*****************************************************************************/
/*
  void tas_parafactor(lofig,cnsfig,fcapa,fresi)
  lofig_list *lofig ;
  cnsfig_list *cnsfig ;
  float fcapa ;
  float fresi ;
  {
  losig_list *losig ;
  locon_list *locon ;
  lowire_list *ptwire ;
  lorcnet_list *ptrcnet ;
  loctc_list *ptctc ;
  ptype_list *ptype ;
  chain_list *chain ;
  tlc_loconparam  *ptparam;
  branch_list *branch[3];
  cone_list *cone ;
  link_list *link ;
  int i ;

  if((fcapa > 0.99999) && (fcapa < 1.000001) && 
  (fresi > 0.99999) && (fresi < 1.000001))
  return ;

  if(lofig != NULL)
  for(losig = lofig->LOSIG ; losig != NULL ; losig = losig->NEXT)
  {
  ptrcnet = losig->PRCN ;

  ptrcnet->CAPA *= fcapa ;

  for(ptwire = ptrcnet->PWIRE ; ptwire != NULL ; ptwire = ptwire->NEXT)
  ptwire->CAPA *= fresi ;
  for(chain = ptrcnet->PCTC ; chain != NULL ; chain = chain->NEXT)
  {
  ptctc = (loctc_list *)chain->DATA ;
  ptctc->CAPA *= fcapa ;
  }
  if((ptype = getptype(losig->USER,LOFIGCHAIN)) != NULL)
  {
  for(chain = (chain_list *)ptype->DATA ; chain !=NULL ;
  chain = chain->NEXT)
  {
  locon = (locon_list *)chain->DATA ;
  ptparam = tlc_getloconparam(locon) ;
  ptparam->CAPA *= fcapa ;
  }
  }
  }

  if(cnsfig != NULL)
  {
  for(cone = cnsfig->CONE ; cone != NULL ; cone = cone->NEXT)
  {
  branch[0]=cone->BREXT ;
  branch[1]=cone->BRVDD ;
  branch[2]=cone->BRVSS ;
  for(i = 0 ; i < 3 ; i++)
  for(; branch[i] != NULL ; branch[i] = branch[i]->NEXT)
  {
  for(link = branch[i]->LINK ; link != NULL ; link=link->NEXT)
  {
  link->CAPA *= fcapa ;
  }
  }
  }
  }
  }
*/

/*****************************************************************************/
/*                        function tas_capasig()                              */
/* calcul la capacite d'un signal                                            */
/*****************************************************************************/

void tas_capasig(lofig)
     lofig_list *lofig ;
{
  /*
    losig_list *losig ;
    locon_list *locon ;
    losig_list *losigvss = NULL ;
    lotrs_list *lotrs ;
    lotrs_list *lotrssav = NULL ;
    lorcnet_list *ptrcnet ;
    lowire_list *ptwire ;
    loctc_list *ptctc ;
    chain_list *chain ;
    chain_list *chainlotrs=NULL ;
    char flag = 'N' ;

    lotrs = lofig->LOTRS ;

    while(lotrs != NULL)
    {
    if((lotrs->WIDTH != 0) && (lotrs->LENGTH != 0))
    {
    lotrssav = lotrs ;
    lotrs = lotrs->NEXT ;
    continue ;
    }

    flag = 'Y' ;

    chainlotrs = addchain(chainlotrs,lotrs) ;

    if(lotrssav == NULL)  
    {
    lofig->LOTRS = lotrs->NEXT ;
    lotrs = lofig->LOTRS ;
    }
    else
    {
    lotrssav->NEXT = lotrs->NEXT ;
    lotrs = lotrssav->NEXT ;
    }
    }

    if(flag == 'Y')
    {
    for(chain = chainlotrs ; chain != NULL ; chain = chain->NEXT)
    {
    lotrs = (lotrs_list *)chain->DATA ;
    lotrs->NEXT = lofig->LOTRS ;
    lofig->LOTRS = lotrs ;

    if((lotrs->PS != 0) || (lotrs->XS != 0))
    {
    if(((lotrs->SOURCE->SIG->PRCN->PWIRE == NULL) && 
    (lotrs->SOURCE->SIG->PRCN->PCTC == NULL)) ||
    (lotrs->SOURCE->PNODE == NULL) || (losigvss == NULL))
    addcapa(lotrs->SOURCE->SIG, elpLotrsCapaSource(lotrs)) ;
    else
    addcapa(lotrs->SOURCE->SIG, elpLotrsCapaSource(lotrs)) ;
    }

    if((lotrs->PD != 0) || (lotrs->XD != 0))
    {
    if(((lotrs->DRAIN->SIG->PRCN->PWIRE == NULL) && 
    (lotrs->DRAIN->SIG->PRCN->PCTC == NULL)) ||
    (lotrs->DRAIN->PNODE == NULL) || (losigvss == NULL))
    addcapa( lotrs->DRAIN->SIG, elpLotrsCapaDrain(lotrs) ) ;
    else
    addcapa( lotrs->DRAIN->SIG, elpLotrsCapaDrain(lotrs) ) ;
    }
    dellotrs(lofig,lotrs) ;
    }

    freechain(chainlotrs) ;
    }
  */
#ifndef __ALL__WARNING__
  lofig = NULL;
#endif
}

/*****************************************************************************/
/*                        function tas_devect()                              */
/* parametres :                                                              */
/* name : nom a devectoriser                                                 */
/*                                                                           */
/* enleve la vectorisation d'un nom de signaux                               */
/*****************************************************************************/
char *tas_devect(str)
     char *str ;
{
  char buf[1024] ;
  int  i,j = 0;
  int  close_bracket;
  char *end;
 
  if (str == NULL)
    return NULL;
 
  close_bracket = FALSE;
  for (i = 0; str[i] != '\0' ; i++);
  while (str[i-1] == ' ') i--;
  end = str + i;
  for (i = 0; str + i < end; i++)
    {
      if (str[i] == '(' || str[i] == '[' || str[i] == ' ')
        {
          if (close_bracket)
            {
              buf[j++] = '_';
              buf[j++] = 'v';
              close_bracket = FALSE;
            }
          else
            {
              buf[j++] = '_';
              close_bracket = (str[i] == ' ');
            }
        }
      else if (str[i] == ')' || str[i] == ']')
        {
          buf[j++] = '_';
          buf[j++] = 'v';
        }
      else if (str[i] == SEPAR)
        {
          if (close_bracket)
            {
              buf[j++] = '_';
              buf[j++] = 'v';
              close_bracket = FALSE;
            }
          buf[j++] = SEPAR;
        }
      else
        {
          buf[j++] = str[i];
        }
    }
  if (str[i] == ' ')
    {
      while (str[i] != '\0')
        buf[j++] = str[i++];
    }
 
  buf[j] = '\0' ;
  while (buf[--j] == ' ') buf[j] = 0;
  return namealloc(buf) ;
}

/****************************************************************************/
/*                           fonction tas_testbleeder()                     */
/* recherche s'il y a des branches bleeder vers le haut ou vers le bas      */
/****************************************************************************/
int tas_testbleeder(cone,typebr,typebl)
     cone_list *cone ;
     long typebr;
     char typebl;
{
  branch_list *branch ;
  ptype_list *ptype ;
  chain_list *chain ;

  if((cone->TYPE & (CNS_LATCH|CNS_MEMSYM|CNS_FLIP_FLOP|CNS_MASTER|CNS_SLAVE)) != 0)
    {
      if((((typebr & CNS_VDD) == CNS_VDD) && 
          (typebl == 'D')) ||
         (((typebr & CNS_VSS) == CNS_VSS) && 
          (typebl == 'U')) ||
         ((typebr & CNS_EXT) == CNS_EXT))
        return 1 ;
      else return 0 ;
    }

  if((ptype = getptype(cone->USER,CNS_BLEEDER))==NULL)
    return 0 ;

  for(chain = (chain_list *)ptype->DATA ; chain != NULL ; chain = chain->NEXT)
    {
      branch = (branch_list *)chain->DATA ;
      if((((typebr & CNS_VDD) == CNS_VDD) && 
          ((branch->TYPE & CNS_VSS) == CNS_VSS)) ||
         (((typebr & CNS_VSS) == CNS_VSS) && 
          ((branch->TYPE & CNS_VDD) == CNS_VDD)) ||
         ((typebr & CNS_EXT) == CNS_EXT))
        return 1 ;
    }
  return(0) ;
}

/****************************************************************************/
/*                           fonction tas_GetInLink()                       */
/* Renvoie l'entree correspondant au signal attaquant le maillon passe en   */
/* parametre.                                                               */
/****************************************************************************/
edge_list *tas_GetInLink(cone,link)
     cone_list *cone ;
     link_list *link ;

{
  edge_list *incone ;

  for(incone = cone->INCONE ; incone != NULL ; incone = incone->NEXT)
    if((link->TYPE & (CNS_IN | CNS_INOUT)) == 0)
      {
        if((cone_list *)getptype(link->ULINK.LOTRS->USER,CNS_DRIVINGCONE)->DATA ==
           incone->UEDGE.CONE)
          break ;
      }
    else
      {
        if(link->ULINK.LOCON == incone->UEDGE.LOCON)
          break ;
      }

  return(incone) ;
}

/****************************************************************************/
/*                           fonction tas_test_p()                          */
/* recherche la chaine '_p' dans le nom passe en parametre. Elle sert a     */
/* detecter la precharge. Renvoie 1 si la chaine existe, 0 sinon.           */
/****************************************************************************/
short tas_test_p(name)
     char *name ;
{
  long i = strlen(name) - 1 ;
  long j ;


  while((i != 0) && (isdigit((int)name[i])))
    {
      i-- ;
      if((i != 0) && (name[i] == '_'))
        {
          i-- ;
          break ;
        }
    }

  if(i == 0)
    return(0) ;

  j = i ;

  while((j != 0) && (name[j] != ']')) 
    j-- ;

  if(j != 0)
    i = j ;

  if(name[i] == ']')
    {
      if((name[i+1] != '\0') && (name[i+1] != '_'))
        return(0) ;
      while((i != 0) && (name[i--] == ']'))
        while((i != 0) && (name[i--] != '[')) ;

      i++ ;
    }

  if((i > 1) && (name[i] == 'p') && (name[i-1] == '_') && 
     ((name[i+1] == '\0') || (name[i+1] == '_') || (name[i+1] == '[')))
    i = 0 ;

  return(!i) ;
}


/****************************************************************************/
/*                           fonction tas_CorrectHZType()                   */
/* pour un signal precharge, cette fonction enleve l'attribut HZ du cone et */
/* du connecteur eventuellement associe.                                    */
/****************************************************************************/
int tas_CorrectHZType(cone)
     cone_list *cone ;

{
  locon_list *locon;
  locon= cns_get_one_cone_external_connector(cone) ;

  cone->TYPE &= ~CNS_TRI;
  if(locon != NULL)
    {
      locon = (locon_list *)((ptype_list *)locon)->DATA ;

      if(locon->DIRECTION == 'T')
        locon->DIRECTION = 'B' ;
      if(locon->DIRECTION == 'Z')
        locon->DIRECTION = 'O' ;
    }

  return(0) ;
}


/****************************************************************************/
/*                           fonction tas_TreatInfSig()                     */
/* type le cone s'il appartient a la liste des collages.                    */
/****************************************************************************/
int tas_TreatInfSig(ifl, cone)
     inffig_list *ifl;
     cone_list *cone ;
{
  list_list   *list        ;
  ptype_list *ptype;
  losig_list *losigcone;
  edge_list *incone;
  cone_list *cn;

  if (cone->INCONE==NULL)
  {
    cone->TYPE |= TAS_BREAK ;
    if ((ptype=getptype(cone->USER, CNS_SIGNAL))!=NULL)
     {
       losigcone = (losig_list *)ptype->DATA;
       if (getptype(losigcone->USER,TAS_SIG_NORCDELAY) == NULL)
         losigcone->USER = addptype(losigcone->USER,TAS_SIG_NORCDELAY,NULL) ;
     }
  }
  if ((cone->TYPE & CNS_EXT)!=0 && ((cone->TYPE & CNS_LATCH)!=0 || (cone->TYPE & CNS_PRECHARGE)!=0))
    {
      if ((ptype=getptype(cone->USER, CNS_SIGNAL))!=NULL)
        {
          losigcone = (losig_list *)ptype->DATA;
          if (getptype(losigcone->USER,TAS_SIG_FORCERCDELAY) == NULL)
            losigcone->USER = addptype(losigcone->USER,TAS_SIG_FORCERCDELAY,NULL) ;
        }
    }

  
  for (incone = cone->INCONE; incone != NULL; incone = incone->NEXT) 
    {
      if ((incone->TYPE & CNS_COMMAND)!=0 && (incone->TYPE & CNS_EXT)==0)
      {
        cn=(cone_list *) incone->UEDGE.CONE;
        if ((cn->TYPE & CNS_LATCH)!=0 || (cn->TYPE & CNS_PRECHARGE)!=0)
        {
          if ((ptype=getptype(cn->USER, CNS_SIGNAL))!=NULL)
            {
              losigcone = (losig_list *)ptype->DATA;
              if (getptype(losigcone->USER,TAS_SIG_FORCERCDELAY) == NULL)
                losigcone->USER = addptype(losigcone->USER,TAS_SIG_FORCERCDELAY,NULL) ;
            }
        }
      }
    }

  for(list = TAS_CONTEXT->INF_SIGLIST ; list != NULL ; list = list->NEXT) 
    {
    if((list->TYPE != INF_LL_STUCKVSS) && (list->TYPE != INF_LL_STUCKVDD)
       /*&& (list->TYPE != INF_LL_SLOPEIN) && (list->TYPE != INF_LL_CAPAOUT)*/
       /*&& (list->TYPE != INF_PINSLEW)*/)
      {
        if(list->DATA == cone->NAME)
          {
            /* 0 et 1 collage => traite plus loin => ici ignores */
            if(list->TYPE == INF_LL_PRECHARGE) 
              {
                cone->TYPE |= TAS_PRECHARGE ; list->DATA = NULL ;
                tas_CorrectHZType(cone) ;
              }
            if(list->TYPE == INF_LL_NOTPRECHARGE) /* signal non-precharge */
              {
                cone->TYPE |= TAS_NOPRECH ;
                cone->TYPE &= ~TAS_PRECHARGE ;
              }
            if(list->TYPE == INF_LL_BYPASSIN) 
              cone->TYPE |= TAS_BYPASSIN ;
            if(list->TYPE == INF_LL_BYPASSOUT)
              cone->TYPE |= TAS_BYPASSOUT ;
            if(list->TYPE == INF_LL_BYPASS) 
              cone->TYPE |= TAS_BYPASSIN | TAS_BYPASSOUT ;
            if(list->TYPE == INF_LL_INTER)
              cone->TYPE |= TAS_INTER ;
            if(list->TYPE == INF_LL_BREAK)
              cone->TYPE |= TAS_BREAK ;
            if(list->TYPE == INF_LL_NORISING)
              cone->TYPE |= TAS_NORISING ;
            if(list->TYPE == INF_LL_NOFALLING)
              cone->TYPE |= TAS_NOFALLING ;

            list->DATA = NULL ;
          }
        else if((cone->BREXT != NULL) && ((cone->TYPE & CNS_EXT) != CNS_EXT))
          {
            branch_list *branch ;
            link_list *link ;
            locon_list *locon ;
            for(branch = cone->BREXT ; branch != NULL ; branch = branch->NEXT)
              {
                for(link = branch->LINK ; link != NULL ; link = link->NEXT)
                  if((link->TYPE & CNS_EXT) == CNS_EXT) break ;
                if(link != NULL) locon = link->ULINK.LOCON ;
                if(list->DATA == locon->NAME)
                  {
                    if(list->TYPE == INF_LL_BYPASSIN)
                      if(getptype(locon->USER,TAS_CON_BYPASSIN) == NULL)
                        locon->USER = addptype(locon->USER,TAS_CON_BYPASSIN,NULL) ;
                    if(list->TYPE == INF_LL_BYPASSOUT)
                      if(getptype(locon->USER,TAS_CON_BYPASSOUT) == NULL)
                        locon->USER = addptype(locon->USER,TAS_CON_BYPASSOUT,NULL) ;
                    if(list->TYPE == INF_LL_BYPASS)
                      {
                        if(getptype(locon->USER,TAS_CON_BYPASSIN) == NULL)
                          locon->USER = addptype(locon->USER,TAS_CON_BYPASSIN,NULL) ;
                        if(getptype(locon->USER,TAS_CON_BYPASSOUT) == NULL)
                          locon->USER = addptype(locon->USER,TAS_CON_BYPASSOUT,NULL) ;
                      }
                    list->DATA = NULL ;
                  }
              }
          }
      }
  }
  return(0);
}


/****************************************************************************/
/*                           fonction init_timing()                         */
/* initialisiation pour la phase de calcul des temps elementaires           */
/****************************************************************************/
int init_timing(ptfig)
     cnsfig_list *ptfig   ;
{
  cone_list *cone ;
  list_list *list, *ll ;
  inffig_list *ifl;

  ifl=getloadedinffig(ptfig->NAME);

  /*-------------------------------------------------------------------------*/
  /* Si l'option precharge n'est pas active, tous les cones types precharges */
  /* seront detypes pour etre consideres comme des cones combinatoires.      */
  /*-------------------------------------------------------------------------*/
  for(cone = ptfig->CONE ; cone != NULL ; cone = cone->NEXT)
    {
      short i;
      branch_list  *path         ;
      branch_list *pathc[3]      ;
      locon_list *locon = NULL ;

      /* revectorisation du nom de cone */
      cone->NAME = ttv_revect(cone->NAME) ;

      /* si option desactive et cone precharge => detypage */
      if(TAS_CONTEXT->TAS_TREATPRECH == 'N')
        cone->TYPE &= ~TAS_PRECHARGE ;

      /*-------------------------------------------------------------------*/
      /* traitement du cone suivant le fichier '.inf': si le cone est dans */
      /* la liste des signaux du fichier, on fait le traitement adequat:   */
      /*         type = 0 => collage a 0                                   */
      /*         type = 1 => collage a 1                                   */
      /*         type = 2 => signal precharge                              */
      /*         type = 3 => signal non precharge                          */
      /*         type = 4 => signal insignifiant                           */
      /*-------------------------------------------------------------------*/
      tas_TreatInfSig(ifl, cone) ;

      /* sinon on cherche le '_p' */
      if(TAS_CONTEXT->TAS_TREATPRECH != 'N')
        if(((tas_test_p(cone->NAME) == 1) ||
            ((cone->TYPE & CNS_PRECHARGE) == CNS_PRECHARGE)) &&
           ((cone->TYPE & TAS_NOPRECH) != TAS_NOPRECH))
          {
            cone->TYPE |= TAS_PRECHARGE ;
            tas_CorrectHZType(cone) ;
          }

      if((cone->TYPE & TAS_PRECHARGE) == TAS_PRECHARGE)
        {
          branch_list *auxpath = NULL ;
 
          for(path = cone->BRVDD ; path != NULL ; path = path->NEXT)
            if((path->TYPE & CNS_NOT_FUNCTIONAL) != CNS_NOT_FUNCTIONAL)
              {
                link_list *link = path->LINK ;

                if((link->ULINK.LOTRS->TYPE & CNS_TN) == CNS_TN)
                  {
                    link_list *linkx ;
   
                    for(linkx = (link_list *)path->LINK ; (linkx != NULL) && 
                          ((linkx->ULINK.LOTRS->TYPE & CNS_TP) != CNS_TP) ;
                        linkx = linkx->NEXT) ;
  
                    if(linkx != NULL)
                      path->TYPE |= CNS_NOT_FUNCTIONAL ;
                  }
                else if((link->ULINK.LOTRS->TYPE & CNS_TP) == CNS_TP)
                  auxpath = path ;
              }
          if(auxpath != NULL)
            for(path = cone->BRVDD ; path != NULL ; path = path->NEXT)
              if((path->TYPE & CNS_NOT_FUNCTIONAL) != CNS_NOT_FUNCTIONAL)
                if((path->LINK->ULINK.LOTRS->TYPE & CNS_TN) == CNS_TN)
                  path->TYPE |= CNS_NOT_FUNCTIONAL ;
        }


      /* determination du connecteur de sortie eventuel */
      if((cone->TYPE & (CNS_EXT | TAS_CONE_CONNECT)) == CNS_EXT)
        {
          chain_list *cl;
          cl=cns_get_cone_external_connectors(cone);
          
          /*ptype_list *ptype = getptype(cone->USER,CNS_EXT) ;

          if(ptype != NULL)*/
           while (cl!=NULL)
            {
              edge_list  *incone ;

              locon = (locon_list *)cl->DATA; //ptype->DATA ;

              /*-------------------------------------------------------------*/
              /* si un cone est monte sur un connecteur INOUT, ce connecteur */
              /* apparait en entree et en sortie de ce cone. On le type donc */
              /* en tant qu'entree pour ne pas le prendre en compte.         */
              /*-------------------------------------------------------------*/
              for(incone = cone->INCONE ; incone != NULL ; incone = incone->NEXT)
                if(incone->UEDGE.LOCON == locon)
                  {
                    incone->TYPE |= TAS_PATH_INOUT ;
                    break ;
                  }
              cl=delchain(cl,cl);
            } 
        }
      pathc[0] = cone->BREXT;
      pathc[1] = cone->BRVDD;
      pathc[2] = cone->BRVSS;
      for(i = 0 ; i < 3 ; i++)
        for(; pathc[i] != NULL ; pathc[i] = pathc[i]->NEXT)
          {
            link_list *link = (link_list *)pathc[i]->LINK ;

            /*--------------------------------------------------------------------*/
            /* si un connecteur INOUT est en entree et en sortie de cone, il doit */
            /* etre ignore (meme chose que pour INCONE un peu plus haut).         */
            /*--------------------------------------------------------------------*/
            if(link->ULINK.LOCON == locon)
              if(locon->DIRECTION != 'I')
                pathc[i]->TYPE |= TAS_PATH_INOUT ;
          }
    }

  /* recherche si toute la liste sauf les collages a ete traitee */
  for(list = TAS_CONTEXT->INF_SIGLIST ; list != NULL ; list = list->NEXT)
    {
      if((list->DATA != NULL) 
         && (list->TYPE != INF_LL_STUCKVSS) 
         && (list->TYPE != INF_LL_STUCKVDD) 
         /*&& (list->TYPE != INF_LL_SLOPEIN) 
         && (list->TYPE != INF_LL_CAPAOUT)*/
         /*&& (list->TYPE != INF_LL_PINSLEW)*/)
        {
          /*tas_error(9,list->DATA,TAS_WARNING) ;*/
          list->DATA = NULL ;
        }
    }
  return(0) ;
}

/****************************************************************************/
/*                           fonction tas_detectinout()                     */
/* detect les connecteurs inout qui passe par des passtrans                 */
/****************************************************************************/
void tas_detectinout(ptfig)
     cnsfig_list *ptfig ;
{
  locon_list *locon ;
  cone_list  *cone  ;
  cone_list *conein ;
  cone_list *conex ;
  edge_list *incone ;
  edge_list *outcone ;
  edge_list *in ;
  branch_list *path[2] ;
  link_list *link ;
  ptype_list *ptype ;
  chain_list *chainpath ;
  chain_list *chaincone ;
  chain_list *chain ;
  link_list *linkx ;
  long type ;

  short i;
 
  for(cone = ptfig->CONE ; cone != NULL ; cone = cone->NEXT)
    {
      cone->TYPE &= TAS_CONE_MASK ;
      if((cone->TYPE & CNS_MEMSYM) == CNS_MEMSYM)
        {
          for(incone = cone->INCONE ; incone != NULL ; incone = incone->NEXT)
            if((incone->TYPE & (CNS_EXT|CNS_LOOP)) == CNS_LOOP)
              if((incone->UEDGE.CONE->TYPE & CNS_MEMSYM) == CNS_MEMSYM)
                {
                  incone->TYPE |= TAS_IN_NOTMEMSYM ;
                  break ;
                }
     
          if(incone != NULL)
            {
              conein = incone->UEDGE.CONE ;

              if((((cone->TYPE & (CNS_MEMSYM | CNS_LATCH)) == (CNS_MEMSYM | CNS_LATCH))
                  && ((conein->TYPE & (CNS_MEMSYM | CNS_LATCH)) == CNS_MEMSYM)) ||
                 (((cone->TYPE & (CNS_MEMSYM | CNS_FLIP_FLOP)) == 
                   (CNS_MEMSYM | CNS_FLIP_FLOP))
                  && ((conein->TYPE & (CNS_MEMSYM | CNS_FLIP_FLOP)) == CNS_MEMSYM)))
                {
                  for(incone = conein->INCONE ; incone != NULL ; incone = incone->NEXT)
                    {
                      if((incone->TYPE & (CNS_LOOP|CNS_BLEEDER|CNS_FEEDBACK)) != 0) 
                        continue ;
                      for(outcone = cone->INCONE ; outcone != NULL ;
                          outcone = outcone->NEXT)
                        {
                          if(((outcone->TYPE & CNS_EXT) == CNS_EXT) &&
                             (incone->UEDGE.LOCON == outcone->UEDGE.LOCON))
                            break ;
                          else if(((outcone->TYPE & CNS_EXT) != CNS_EXT) &&
                                  (incone->UEDGE.CONE == outcone->UEDGE.CONE))
                            break ;
                        }
                      if(outcone == NULL)
                        {
                          cone->INCONE->USER = addptype(cone->INCONE->USER,TAS_IN_CONESYM,
                                                        (void*)conein) ;
                        }
                    }
                }
            }
        }
    }

  for(locon = ptfig->LOCON ; locon != NULL ; locon = locon->NEXT)
    {
      if(/*(locon->DIRECTION != CNS_B) &&*/ (locon->DIRECTION != CNS_T))
        continue ;
    
      chaincone = NULL ;
      if(getptype(locon->USER,CNS_EXT) == NULL)
        continue ;

      if (!(ptype = getptype(locon->USER,CNS_EXT))) continue;
      cone = (cone_list *)ptype->DATA ;
 
      path[0] = cone->BRVDD;
      path[1] = cone->BRVSS;
      for(i = 0 ; i < 2 ; i++)
        for(; path[i] != NULL ; path[i] = path[i]->NEXT)
          if((path[i]->TYPE & (CNS_BLEEDER|CNS_FEEDBACK)) == 0)
            {
              link = path[i]->LINK ;
              if((ptype = getptype(link->ULINK.LOTRS->USER,TAS_TRANS_INOUT)) == NULL)
                link->ULINK.LOTRS->USER = addptype(link->ULINK.LOTRS->USER,
                                                   TAS_TRANS_INOUT,(void*)NULL) ;
            }
      for(incone = cone->INCONE ; incone != NULL ; incone = incone->NEXT)
        {
          if((incone->TYPE & CNS_EXT) == CNS_EXT) continue ;
          for(outcone = incone->UEDGE.CONE->OUTCONE;outcone != NULL;
              outcone = outcone->NEXT)
            {
              cone_list *conex ;
              char inadd = 'N' ;

              if((outcone->TYPE & CNS_EXT) == CNS_EXT) continue ;
              conex = outcone->UEDGE.CONE ;

              chainpath = NULL ;

              if((conex->TYPE & TAS_MARQUE) == TAS_MARQUE) continue ;
              conex->TYPE |= TAS_MARQUE ;
              chaincone = addchain(chaincone,(void*)conex);

              if(cone == conex) continue ;
              path[0] = conex->BRVDD;
              path[1] = conex->BRVSS;
              for(i = 0 ; i < 2 ; i++)
                for(; path[i] != NULL ; path[i] = path[i]->NEXT)
                  if((path[i]->TYPE & (CNS_BLEEDER|CNS_FEEDBACK)) == 0)
                    {
                      char flag = 'N' ;

                      for(link = path[i]->LINK ; link != NULL ; link = link->NEXT)
                        {
                          if((link->TYPE & (CNS_IN|CNS_INOUT)) != 0) continue ;
                          conein = (cone_list *)getptype(link->ULINK.LOTRS->USER,
                                                         CNS_DRIVINGCONE)->DATA ;
                          for(in = conex->INCONE ; in != NULL ; in = in->NEXT)
                            {
                              if(((in->TYPE & CNS_EXT) != CNS_EXT) && (in->UEDGE.CONE == conein))
                                break ;
                            }
                          if(getptype(link->ULINK.LOTRS->USER,TAS_TRANS_INOUT) != NULL)
                            {
                              flag = 'Y' ;
                              inadd = 'Y' ;
                              chainpath = addchain(chainpath,(void*)path[i]) ;
                              link->TYPE |= TAS_LINK_INOUT ;
                            }
                          if(flag == 'Y')
                            {
                              if((in != NULL) && 
                                 ((in->TYPE & TAS_INCONE_NINOUT) != TAS_INCONE_NINOUT))
                                in->TYPE |= TAS_PATH_INOUT ;
                            }
                          else
                            {
                              if(in != NULL)
                                {
                                  in->TYPE |= TAS_INCONE_NINOUT ;
                                  in->TYPE &= ~(TAS_PATH_INOUT) ;
                                }
                              link->TYPE &= ~(TAS_LINK_INOUT) ;
                            }
                        }
                    }
              for(in = conex->INCONE ; in != NULL ; in = in->NEXT)
                in->TYPE &= ~(TAS_INCONE_NINOUT) ;
              if(inadd == 'Y')
                for(in = conex->INCONE ; in != NULL ; in = in->NEXT)
                  {
                    if(((in->TYPE & CNS_EXT) == CNS_EXT) && (in->UEDGE.LOCON == locon))
                      break ;
                  }
              if((inadd == 'Y') && (in == NULL))
                {
                  conex->INCONE = addedge(conex->INCONE, CNS_EXT, (void*)locon) ;
                  for(chain = chainpath ; chain != NULL ; chain = chain->NEXT)
                    {
                      linkx = NULL ;
                      link = ((branch_list *)chain->DATA)->LINK ;
                      type = ((branch_list *)chain->DATA)->TYPE & ~(CNS_VDD | CNS_VSS) ;
                      type |= (CNS_EXT | CNS_DEGRADED) ;
                      type &= ~(CNS_PARALLEL_INS) ;
                      ((branch_list *)chain->DATA)->TYPE |= (CNS_NOT_FUNCTIONAL|
                                                             TAS_PATH_INOUT) ;
                      while((link->TYPE & TAS_LINK_INOUT) != TAS_LINK_INOUT)
                        {
                          linkx = addlink(linkx,link->TYPE,link->ULINK.PTR,link->SIG) ; 
                          link = link->NEXT ;
                        }
                      if(link != NULL) link->TYPE &= ~(TAS_LINK_INOUT) ;
                      linkx = addlink(linkx,CNS_INOUT,(void*)locon,link->SIG) ;
                      conex->BREXT = addbranch(conex->BREXT,type,
                                               (link_list*)reverse((chain_list *)linkx));
                    }
                  for(path[0] = conex->BREXT;path[0] != NULL;path[0] = path[0]->NEXT)
                    if((path[0]->TYPE & (CNS_BLEEDER|CNS_FEEDBACK)) == 0)
                      for(path[1] = conex->BREXT;path[1] != NULL;path[1] = path[1]->NEXT)
                        {
                          if((path[0] == path[1]) || 
                             ((path[1]->TYPE & (CNS_BLEEDER|CNS_FEEDBACK)) != 0))
                            continue ;
                          for(link = path[0]->LINK ,linkx = path[1]->LINK ;
                              (link != NULL) && (linkx != NULL) ;
                              link = link->NEXT , linkx = linkx->NEXT) 
                            if(link->ULINK.PTR != linkx->ULINK.PTR) break ;
                          if((link == NULL) && (linkx == NULL))
                            path[1]->TYPE |= CNS_NOT_FUNCTIONAL ;
                        }
                  locon2cone(locon,conex) ;
                  cone2locon(conex,locon) ;
                }
              freechain(chainpath) ;
            }
        }
      path[0] = cone->BRVDD;
      path[1] = cone->BRVSS;
      for(i = 0 ; i < 2 ; i++)
        for(; path[i] != NULL ; path[i] = path[i]->NEXT)
          if((path[i]->TYPE & (CNS_BLEEDER|CNS_FEEDBACK)) == 0)
            {
              link = path[i]->LINK ;
              if((getptype(link->ULINK.LOTRS->USER,TAS_TRANS_INOUT)) != NULL)
                link->ULINK.LOTRS->USER = delptype(link->ULINK.LOTRS->USER,
                                                   TAS_TRANS_INOUT) ;
            }
      for(chain = chaincone ; chain != NULL ; chain = chain->NEXT)
        ((cone_list*)chain->DATA)->TYPE &= ~(TAS_MARQUE) ;
      freechain(chaincone) ;
    }
}

/****************************************************************************/
/*                           fonction tas_capara()                          */
/* calcule la capacite ramenee des maillons qui en voient                   */
/****************************************************************************/

static long
tas_get_sum_or_max(chain_list *numlist, int threshold)
{
    chain_list *ptchain;
    int calcsum;
    int result = 0;
    
    if (threshold <= 0 || countchain(numlist) <= threshold) calcsum = 1;
    else calcsum = 0;
    for (ptchain = numlist; ptchain; ptchain = ptchain->NEXT) {
        if (calcsum) result += (long)ptchain->DATA;
        else result = ((long)ptchain->DATA > result)?(long)ptchain->DATA:result;
    }
    return result;
}

int 
tas_capara(cnsfig_list *ptfig)
{
    cone_list      *cone;
    cone_list      *coneout;
    locon_list     *locon;
    losig_list     *checksig;
    lotrs_list     *lotrs, *prevtrans, *opswtrs;
    link_list      *link, *auxlink;
    ptype_list     *ptype;
    branch_list    *path[3];
    edge_list      *incone;
    edge_list      *outcone;
    chain_list     *chainout = NULL;
    chain_list     *chaintrs = NULL;
    chain_list     *auxlinkchain;
    chain_list     *chain, *chainx;
    cone_list      *conex;
    caraclink_list *caraclink;
    double          LinkCapa;
    long            capatr;
    int             i, count;

    /* mark all cones which share transistors or connectors */
    for (cone = ptfig->CONE; cone != NULL; cone = cone->NEXT) {
        path[0] = cone->BREXT;
        path[1] = cone->BRVDD;
        path[2] = cone->BRVSS;
        for (i = 0; i < 3; i++) {
            for (; path[i] != NULL; path[i] = path[i]->NEXT) {
                if ((path[i]->TYPE & CNS_BLEEDER) == CNS_BLEEDER) continue;
                if ((path[i]->TYPE & CNS_BLEEDER) == CNS_FEEDBACK) continue;
                for (link = path[i]->LINK; link != NULL; link = link->NEXT) {
                    if ((link->TYPE & CNS_EXT) != 0) {
                        if ((ptype = getptype((link->ULINK.LOCON)->USER, TAS_CON_TAGCONE)) == NULL) {
                            (link->ULINK.LOCON)->USER = addptype((link->ULINK.LOCON)->USER, TAS_CON_TAGCONE, (void *) cone);
                        }
                        else {
                            if (ptype->DATA != (void *) cone) {
                                cone->TYPE |= TAS_MARQUE;
                                ((cone_list *) ptype->DATA)->TYPE |= TAS_MARQUE;
                            }
                        }
                    }
                    else {
                        if ((ptype = getptype((link->ULINK.LOTRS)->USER, TAS_TRANS_TAGCONE)) == NULL) {
                            (link->ULINK.LOTRS)->USER = addptype((link->ULINK.LOTRS)->USER, TAS_TRANS_TAGCONE, (void *) cone);
                        }
                        else {
                            if (ptype->DATA != (void *) cone) {
                                cone->TYPE |= TAS_MARQUE;
                                ((cone_list *) ptype->DATA)->TYPE |= TAS_MARQUE;
                            }
                        }
                    }
                }
            }
        }
    }

    /* traverse all marked cones */
    for (cone = ptfig->CONE; cone != NULL; cone = cone->NEXT) {
        if ((cone->TYPE & TAS_MARQUE) != TAS_MARQUE) continue;
        /* skip latch feedback cones */
        if ((cone->TYPE & CNS_LATCH) == 0 && (cone->OUTCONE && !cone->OUTCONE->NEXT && (cone->OUTCONE->TYPE & CNS_FEEDBACK) == CNS_FEEDBACK)) continue;
        chainout = NULL ; 
        chaintrs = NULL ; 

        path[0] = cone->BREXT;
        path[1] = cone->BRVDD;
        path[2] = cone->BRVSS;
        for (i = 0; i < 3; i++) {
            for (; path[i] != NULL; path[i] = path[i]->NEXT) {
                if ((path[i]->TYPE & CNS_BLEEDER) == CNS_BLEEDER) continue;
                if ((path[i]->TYPE & CNS_BLEEDER) == CNS_FEEDBACK) continue;

                /* on tag les transistors et les connecteurs du cone */
                for (link = path[i]->LINK; link != NULL; link = link->NEXT) {
                    if ((link->TYPE & CNS_EXT) != 0) {
                        if ((ptype = getptype((link->ULINK.LOCON)->USER, TAS_CON_TAGCONE)) == NULL)
                            (link->ULINK.LOCON)->USER = addptype((link->ULINK.LOCON)->USER, TAS_CON_TAGCONE, (void *) cone);
                        else ptype->DATA = (void *) cone;
                    }
                    else {
                        if ((ptype = getptype((link->ULINK.LOTRS)->USER, TAS_TRANS_TAGCONE)) == NULL)
                            (link->ULINK.LOTRS)->USER = addptype((link->ULINK.LOTRS)->USER, TAS_TRANS_TAGCONE, (void *) cone);
                        else ptype->DATA = (void *) cone;
                        if ((link->TYPE & CNS_SWITCH) == CNS_SWITCH) {
                            opswtrs = (lotrs_list *)getptype(link->ULINK.LOTRS->USER, CNS_SWITCH)->DATA;
                            if ((ptype = getptype(opswtrs->USER, TAS_TRANS_TAGCONE)) == NULL)
                                opswtrs->USER = addptype(opswtrs->USER, TAS_TRANS_TAGCONE, (void *) cone);
                            else ptype->DATA = (void *) cone;
                        }
                    }
                }
            }
        }

        /*------------------------------------------------------------------------*/
        /* l'idee c'est, une fois les transistors tages, parcourir la liste des   */
        /* sorties des entrees du cone courant. On obtient ainsi la liste des     */
        /* cones susceptibles d'avoir des transistors en commun avec le cone      */
        /* courant, ce qui est source de capacite ramenee. On parcourt alors pour */
        /* chacune de ces sorties, sa liste de transistors. Si a un moment donne  */
        /* on arrive sur des transistors tages, c'est qu'on en a en commun. Alors */
        /* il suffit de ramener les capacites des maillons non-tages sur celui    */
        /* qui l'est.                                                             */
        /*------------------------------------------------------------------------*/

        /* get list of susceptible cones */
        for (incone = cone->INCONE; incone != NULL; incone = incone->NEXT) {
            if ((incone->TYPE & (CNS_FEEDBACK | TAS_IN_MEMSYM | CNS_BLEEDER | CNS_EXT)) != 0) continue;
            for (outcone = ((cone_list *) incone->UEDGE.CONE)->OUTCONE; outcone != NULL; outcone = outcone->NEXT) {
                if ((outcone->TYPE & (CNS_FEEDBACK|TAS_IN_MEMSYM|CNS_BLEEDER|CNS_EXT)) != 0) continue;
                coneout = outcone->UEDGE.CONE;
                if (coneout != cone && (coneout->TYPE & (TAS_DEJAEMPILE | TAS_MARQUE)) == TAS_MARQUE) {
                    chainout = addchain(chainout, coneout);
                    coneout->TYPE |= TAS_DEJAEMPILE;
                }
            }
        }
        /* parcours de cones susceptibles */
        for (chain = chainout; chain; chain = chain->NEXT) {
            coneout = (cone_list *)chain->DATA;
            /* recherche des transistors en commun */
            path[0] = coneout->BREXT;
            path[1] = coneout->BRVDD;
            path[2] = coneout->BRVSS;
            for (i = 0; i < 3; i++) {
                for (; path[i] != NULL; path[i] = path[i]->NEXT) {
                    if ((path[i]->TYPE & CNS_BLEEDER) == CNS_BLEEDER) continue;
                    if ((path[i]->TYPE & CNS_BLEEDER) == CNS_FEEDBACK) continue;
                    auxlinkchain = NULL;
                    capatr = 0;
                    for (link = (link_list *) path[i]->LINK; link != NULL; link = link->NEXT) {
                        /* si on tombe sur un connecteur tage */
                        if ((link->TYPE & CNS_EXT) != 0) {
                            ptype = getptype((link->ULINK.LOCON)->USER, TAS_CON_TAGCONE);
                            if (ptype && (cone_list *)ptype->DATA == cone) {
                                if (auxlinkchain != NULL) {
                                    locon = (locon_list *)link->ULINK.LOCON;
                                    /* ignore out path caps inside a cone */
                                    if (V_INT_TAB[__TAS_CAPARA_STRICT].VALUE == 1 || (V_INT_TAB[__TAS_CAPARA_STRICT].VALUE == 2 && ((cone->TYPE & CNS_LATCH) != 0))) {
                                        checksig = locon->SIG;
                                        ptype = getptype(cone->USER, CNS_SIGNAL);
                                        if (ptype && ptype->DATA != checksig) break;
                                    }
                                    count = 0;
                                    for (chainx = auxlinkchain; chainx; chainx = chainx->NEXT) {
                                        count++;
                                        if (count > V_INT_TAB[__TAS_CAPARA_DEPTH].VALUE && V_INT_TAB[__TAS_CAPARA_DEPTH].VALUE >= 0) break;
                                        auxlink = (link_list *)chainx->DATA;
                                        if ((ptype = getptype(auxlink->ULINK.LOTRS->USER, TAS_TRANS_TAGOUT)) == NULL) {
                                            capatr += (long)tas_getcapalink(NULL, auxlink, TAS_UNKNOWN_EVENT);
                                            chaintrs = addchain(chaintrs, auxlink->ULINK.LOTRS);
                                            auxlink->ULINK.LOTRS->USER = addptype(auxlink->ULINK.LOTRS->USER, TAS_TRANS_TAGOUT, coneout);
                                        }
                                    }
                                    if ((ptype = getptype(locon->USER, TAS_CON_TAGOUT)) == NULL) {
                                        locon->USER = addptype(locon->USER, TAS_CON_TAGOUT, cone);
                                    }
                                    else ptype->DATA = cone;
                                    if ((ptype = getptype(locon->USER, TAS_CON_TAGCAPA)) == NULL) {
                                        /* ajout de la capa ramenee */
                                        locon->USER = addptype(locon->USER, TAS_CON_TAGCAPA, addchain(NULL, (void *)capatr));
                                    }
                                    else {
                                        if ((getptype(locon->USER, TAS_CON_TAGUSED)->DATA != coneout)) {
                                            /* add to existing capa if existing capa is not due to same coneout */
                                            ptype->DATA = addchain((chain_list *)ptype->DATA, (void *)capatr);
                                        }
                                    }
                                    /* add coneout to bleeder up/down lists for connector if necessary */
                                    if (count <= V_INT_TAB[__TAS_CAPARA_DEPTH].VALUE || V_INT_TAB[__TAS_CAPARA_DEPTH].VALUE < 0) {
                                        if (tas_testbleeder(coneout, path[i]->TYPE, 'U') == 1) {
                                            if ((ptype = getptype(locon->USER, TAS_CON_BLDUP)) == NULL) {
                                                locon->USER = addptype(locon->USER, TAS_CON_BLDUP, addchain(NULL, coneout));
                                            }
                                            else {
                                                if ((getptype(locon->USER, TAS_CON_TAGUSED)->DATA != coneout)) {
                                                    ptype->DATA = addchain((chain_list *)ptype->DATA, coneout);
                                                }
                                            }
                                        }
                                        if (tas_testbleeder(coneout, path[i]->TYPE, 'D') == 1) {
                                            if ((ptype = getptype(locon->USER, TAS_CON_BLDDOWN)) == NULL) {
                                                locon->USER = addptype(locon->USER, TAS_CON_BLDDOWN, addchain(NULL, coneout));
                                            }
                                            else {
                                                if ((getptype(locon->USER, TAS_CON_TAGUSED)->DATA != coneout)) {
                                                    ptype->DATA = addchain((chain_list *)ptype->DATA, coneout);
                                                }
                                            }
                                        }
                                    }
                                    /* tag the connector with the coneout */
                                    if ((ptype = getptype(locon->USER, TAS_CON_TAGUSED)) == NULL) {
                                        locon->USER = addptype(locon->USER, TAS_CON_TAGUSED, (void *)coneout);
                                    }
                                    else ptype->DATA = coneout;
                                }
                            }
                        }
                        /* si transistor tage */
                        else {
                            ptype = getptype((link->ULINK.LOTRS)->USER, TAS_TRANS_TAGCONE);
                            if (ptype && (cone_list *)ptype->DATA == cone) {
                                if (auxlinkchain != NULL) {
                                    lotrs = link->ULINK.LOTRS;
                                    /* ignore out path caps inside a cone */
                                    if (V_INT_TAB[__TAS_CAPARA_STRICT].VALUE == 1 || (V_INT_TAB[__TAS_CAPARA_STRICT].VALUE == 2 && ((cone->TYPE & CNS_LATCH) != 0))) {
                                        prevtrans = ((link_list *)auxlinkchain->DATA)->ULINK.LOTRS;
                                        if (lotrs->DRAIN->SIG == prevtrans->SOURCE->SIG || lotrs->DRAIN->SIG == prevtrans->DRAIN->SIG) {
                                            checksig = lotrs->DRAIN->SIG;
                                        }
                                        else checksig = lotrs->SOURCE->SIG;
                                        ptype = getptype(cone->USER, CNS_SIGNAL);
                                        if (ptype && ptype->DATA != checksig) break;
                                    }
                                    count = 0;
                                    for (chainx = auxlinkchain; chainx; chainx = chainx->NEXT) {
                                        count++;
                                        if (count > V_INT_TAB[__TAS_CAPARA_DEPTH].VALUE && V_INT_TAB[__TAS_CAPARA_DEPTH].VALUE >= 0) break;
                                        auxlink = (link_list *)chainx->DATA;
                                        if ((ptype = getptype(auxlink->ULINK.LOTRS->USER, TAS_TRANS_TAGOUT)) == NULL) {
                                            capatr += (long)tas_getcapalink(NULL, auxlink, TAS_UNKNOWN_EVENT);
                                            chaintrs = addchain(chaintrs, auxlink->ULINK.LOTRS);
                                            auxlink->ULINK.LOTRS->USER = addptype(auxlink->ULINK.LOTRS->USER, TAS_TRANS_TAGOUT, coneout);
                                        }
                                    }
                                    if ((ptype = getptype(lotrs->USER, TAS_TRANS_TAGOUT)) == NULL) {
                                        lotrs->USER = addptype(lotrs->USER, TAS_TRANS_TAGOUT, cone);
                                    }
                                    else ptype->DATA = cone;
                                    if ((ptype = getptype(lotrs->USER, TAS_TRANS_TAGCAPA)) == NULL) {
                                        /* ajout de la capa ramenee */
                                        lotrs->USER = addptype(lotrs->USER, TAS_TRANS_TAGCAPA, addchain(NULL, (void *)capatr));
                                    }
                                    else {
                                        if ((getptype(lotrs->USER, TAS_TRANS_TAGUSED)->DATA != coneout)) {
                                            /* add to existing capa if existing capa is not due to same coneout */
                                            ptype->DATA = addchain((chain_list *)ptype->DATA, (void *)capatr);
                                        }
                                    }
                                    /* add coneout to bleeder up/down lists for transistor if necessary */
                                    if (count <= V_INT_TAB[__TAS_CAPARA_DEPTH].VALUE || V_INT_TAB[__TAS_CAPARA_DEPTH].VALUE < 0) {
                                        if (tas_testbleeder(coneout, path[i]->TYPE, 'U') == 1) {
                                            if ((ptype = getptype(lotrs->USER, TAS_TRANS_BLDUP)) == NULL) {
                                                lotrs->USER = addptype(lotrs->USER, TAS_TRANS_BLDUP, addchain(NULL, coneout));
                                            }
                                            else {
                                                if ((getptype(lotrs->USER, TAS_TRANS_TAGUSED)->DATA != coneout)) {
                                                    ptype->DATA = addchain((chain_list *) ptype->DATA, coneout);
                                                }
                                            }
                                        }
                                        if (tas_testbleeder(coneout, path[i]->TYPE, 'D') == 1) {
                                            if ((ptype = getptype(lotrs->USER, TAS_TRANS_BLDDOWN)) == NULL) {
                                                lotrs->USER = addptype(lotrs->USER, TAS_TRANS_BLDDOWN, addchain(NULL, coneout));
                                            }
                                            else {
                                                if ((getptype(lotrs->USER, TAS_TRANS_TAGUSED)->DATA != coneout)) {
                                                    ptype->DATA = addchain((chain_list *) ptype->DATA, coneout);
                                                }
                                            }
                                        }
                                    }
                                    /* tag the transistor with the coneout */
                                    if ((ptype = getptype(lotrs->USER, TAS_TRANS_TAGUSED)) == NULL) {
                                        lotrs->USER = addptype(lotrs->USER, TAS_TRANS_TAGUSED, coneout);
                                    }
                                    else ptype->DATA = coneout;
                                }
                                break;
                            }
                        }
                        auxlinkchain = addchain(auxlinkchain, link);
                    }
                    freechain(auxlinkchain);
                }
            }
        } /* fin parcours de cones susceptibles */

        for (chain = chaintrs; chain != NULL; chain = chain->NEXT) {
            lotrs = (lotrs_list *) chain->DATA;
            lotrs->USER = delptype(lotrs->USER, TAS_TRANS_TAGOUT);
        }

        for (chain = chainout; chain != NULL; chain = chain->NEXT) {
            ((cone_list *) chain->DATA)->TYPE &= ~(TAS_DEJAEMPILE);
        }

        freechain(chaintrs);
        freechain(chainout);

        /* on ramene la capa ramenee sur le maillon du cone */
        path[0] = cone->BREXT;
        path[1] = cone->BRVDD;
        path[2] = cone->BRVSS;
        for (i = 0; i < 3; i++) {
            for (; path[i] != NULL; path[i] = path[i]->NEXT) {
                if ((path[i]->TYPE & CNS_BLEEDER) == CNS_BLEEDER) continue;
                if ((path[i]->TYPE & CNS_BLEEDER) == CNS_FEEDBACK) continue;
                for (link = (link_list *) path[i]->LINK; link != NULL; link = link->NEXT) {
                    if ((link->TYPE & CNS_EXT) == 0) {
                        ptype = getptype(link->ULINK.LOTRS->USER, TAS_TRANS_TAGOUT);
                        if (ptype && ptype->DATA == cone) {
                            ptype = getptype(link->ULINK.LOTRS->USER, TAS_TRANS_TAGCAPA);
                            if (ptype == NULL) LinkCapa = 0.0;
                            else {
                                LinkCapa = (TAS_CONTEXT->TAS_CAPARAPREC * (double)tas_get_sum_or_max((chain_list *)ptype->DATA, V_INT_TAB[__TAS_CAPARA_FANOUT].VALUE));
                            }
                            caraclink = TAS_GETCLINK(link);
                            caraclink->CRAM = (float) LinkCapa;
                            if (link->ULINK.LOTRS->TRNAME) {
                                avt_log(LOGTAS, 1, "Cone %s: Capacitance value of %.2f fF (%.1f%%) added on transistor %s\n", cone->NAME, caraclink->CRAM, TAS_CONTEXT->TAS_CAPARAPREC * 100.0, link->ULINK.LOTRS->TRNAME);
                            }
                            else {
                                avt_log(LOGTAS, 1, "Cone %s: Capacitance value of %.2f fF (%.1f%%) added on transistor ???\n", cone->NAME, caraclink->CRAM, TAS_CONTEXT->TAS_CAPARAPREC * 100.0);
                            }
                            ptype = getptype(link->ULINK.LOTRS->USER, TAS_TRANS_BLDUP);
                            if (ptype != NULL) {
                                chain = NULL;
                                chainx = (chain_list *)ptype->DATA;
                                if (countchain(chainx) > V_INT_TAB[__TAS_CAPARA_FANOUT].VALUE && V_INT_TAB[__TAS_CAPARA_FANOUT].VALUE > 0) {
                                    if (chainx) chain = addchain(NULL, chainx->DATA);
                                }
                                else {
                                    for (; chainx != NULL; chainx = chainx->NEXT) {
                                        chain = addchain(chain, chainx->DATA);
                                    }
                                }
                                link->USER = addptype(link->USER, TAS_LINK_BLDUP, chain);
                            }
                            ptype = getptype(link->ULINK.LOTRS->USER, TAS_TRANS_BLDDOWN);
                            if (ptype != NULL) {
                                chain = NULL;
                                chainx = (chain_list *)ptype->DATA;
                                if (countchain(chainx) > V_INT_TAB[__TAS_CAPARA_FANOUT].VALUE && V_INT_TAB[__TAS_CAPARA_FANOUT].VALUE > 0) {
                                    if (chainx) chain = addchain(NULL, chainx->DATA);
                                }
                                else {
                                    for (; chainx != NULL; chainx = chainx->NEXT) {
                                        chain = addchain(chain, chainx->DATA);
                                    }
                                }
                                link->USER = addptype(link->USER, TAS_LINK_BLDDOWN, chain);
                            }
                        }
                    }
                    else {
                        ptype = getptype(link->ULINK.LOCON->USER, TAS_CON_TAGOUT);
                        if (ptype == NULL) conex = NULL;
                        else conex = ptype->DATA;
                        if (cone == conex) {
                            ptype = getptype(link->ULINK.LOCON->USER, TAS_CON_TAGCAPA);
                            if (ptype == NULL) LinkCapa = 0.0;
                            else {
                                LinkCapa = (TAS_CONTEXT->TAS_CAPARAPREC * (double)tas_get_sum_or_max((chain_list *)ptype->DATA, V_INT_TAB[__TAS_CAPARA_FANOUT].VALUE));
                            }
                            caraclink = TAS_GETCLINK(link);
                            caraclink->CRAM = (float) LinkCapa;
                            avt_log(LOGTAS, 1, "Cone %s: Capacitance value of %.2f fF (%.1f%%) added on connector %s\n", cone->NAME, caraclink->CRAM, TAS_CONTEXT->TAS_CAPARAPREC * 100.0, link->ULINK.LOCON->NAME);
                            ptype = getptype(link->ULINK.LOCON->USER, TAS_CON_BLDUP);
                            if (ptype != NULL) {
                                chain = NULL;
                                chainx = (chain_list *)ptype->DATA;
                                if (countchain(chainx) > V_INT_TAB[__TAS_CAPARA_FANOUT].VALUE && V_INT_TAB[__TAS_CAPARA_FANOUT].VALUE > 0) {
                                    if (chainx) chain = addchain(NULL, chainx->DATA);
                                }
                                else {
                                    for (; chainx != NULL; chainx = chainx->NEXT) {
                                        chain = addchain(chain, chainx->DATA);
                                    }
                                }
                                link->USER = addptype(link->USER, TAS_LINK_BLDUP, chain);
                            }
                            ptype = getptype(link->ULINK.LOCON->USER, TAS_CON_BLDDOWN);
                            if (ptype != NULL) {
                                chain = NULL;
                                chainx = (chain_list *)ptype->DATA;
                                if (countchain(chainx) > V_INT_TAB[__TAS_CAPARA_FANOUT].VALUE && V_INT_TAB[__TAS_CAPARA_FANOUT].VALUE > 0) {
                                    if (chainx) chain = addchain(NULL, chainx->DATA);
                                }
                                else {
                                    for (; chainx != NULL; chainx = chainx->NEXT) {
                                        chain = addchain(chain, chainx->DATA);
                                    }
                                }
                                link->USER = addptype(link->USER, TAS_LINK_BLDDOWN, chain);
                            }
                        }
                    }
                }
            }
        }
        
        /* nettoyage */

        path[0] = cone->BREXT;
        path[1] = cone->BRVDD;
        path[2] = cone->BRVSS;
        for (i = 0; i < 3; i++) {
            for (; path[i] != NULL; path[i] = path[i]->NEXT) {
                if ((path[i]->TYPE & CNS_BLEEDER) == CNS_BLEEDER) continue;
                if ((path[i]->TYPE & CNS_BLEEDER) == CNS_FEEDBACK) continue;
                for (link = path[i]->LINK; link != NULL; link = link->NEXT) {
                    if ((link->TYPE & (CNS_IN | CNS_INOUT)) != 0) {
                        if ((ptype = getptype((link->ULINK.LOCON)->USER, TAS_CON_TAGCAPA)) != NULL) {
                            freechain((chain_list *) ptype->DATA);
                            (link->ULINK.LOCON)->USER = delptype((link->ULINK.LOCON)->USER, TAS_CON_TAGCAPA);
                        }
                        if ((ptype = getptype((link->ULINK.LOCON)->USER, TAS_CON_TAGCONE)) != NULL)
                            (link->ULINK.LOCON)->USER = delptype((link->ULINK.LOCON)->USER, TAS_CON_TAGCONE);
                        if ((ptype = getptype((link->ULINK.LOCON)->USER, TAS_CON_TAGOUT)) != NULL)
                            (link->ULINK.LOCON)->USER = delptype((link->ULINK.LOCON)->USER, TAS_CON_TAGOUT);
                        if ((ptype = getptype((link->ULINK.LOCON)->USER, TAS_CON_TAGUSED)) != NULL)
                            (link->ULINK.LOCON)->USER = delptype((link->ULINK.LOCON)->USER, TAS_CON_TAGUSED);
                        if ((ptype = getptype((link->ULINK.LOCON)->USER, TAS_CON_BLDUP)) != NULL) {
                            freechain((chain_list *) ptype->DATA);
                            (link->ULINK.LOCON)->USER = delptype((link->ULINK.LOCON)->USER, TAS_CON_BLDUP);
                        }
                        if ((ptype = getptype((link->ULINK.LOCON)->USER, TAS_CON_BLDDOWN)) != NULL) {
                            freechain((chain_list *) ptype->DATA);
                            (link->ULINK.LOCON)->USER = delptype((link->ULINK.LOCON)->USER, TAS_CON_BLDDOWN);
                        }
                    }
                    else {
                        if ((ptype = getptype((link->ULINK.LOTRS)->USER, TAS_TRANS_TAGCAPA)) != NULL) {
                            freechain((chain_list *) ptype->DATA);
                            (link->ULINK.LOTRS)->USER = delptype((link->ULINK.LOTRS)->USER, TAS_TRANS_TAGCAPA);
                        }
                        if ((ptype = getptype((link->ULINK.LOTRS)->USER, TAS_TRANS_TAGCONE)) != NULL)
                            (link->ULINK.LOTRS)->USER = delptype((link->ULINK.LOTRS)->USER, TAS_TRANS_TAGCONE);
                        if ((ptype = getptype((link->ULINK.LOTRS)->USER, TAS_TRANS_TAGOUT)) != NULL)
                            (link->ULINK.LOTRS)->USER = delptype((link->ULINK.LOTRS)->USER, TAS_TRANS_TAGOUT);
                        if ((ptype = getptype((link->ULINK.LOTRS)->USER, TAS_TRANS_TAGUSED)) != NULL)
                            (link->ULINK.LOTRS)->USER = delptype((link->ULINK.LOTRS)->USER, TAS_TRANS_TAGUSED);
                        if ((ptype = getptype((link->ULINK.LOTRS)->USER, TAS_TRANS_BLDUP)) != NULL) {
                            freechain((chain_list *) ptype->DATA);
                            (link->ULINK.LOTRS)->USER = delptype((link->ULINK.LOTRS)->USER, TAS_TRANS_BLDUP);
                        }
                        if ((ptype = getptype((link->ULINK.LOTRS)->USER, TAS_TRANS_BLDDOWN)) != NULL) {
                            freechain((chain_list *) ptype->DATA);
                            (link->ULINK.LOTRS)->USER = delptype((link->ULINK.LOTRS)->USER, TAS_TRANS_BLDDOWN);
                        }
                    }
                }
            }
        }
    } /* fin de parcours des cones */

    for (lotrs = ptfig->LOTRS; lotrs != NULL; lotrs = lotrs->NEXT) {
        TAS_INFO.nb_trans++;
        if (getptype(lotrs->USER, TAS_TRANS_TAGCONE) != NULL)
            lotrs->USER = delptype(lotrs->USER, TAS_TRANS_TAGCONE);
    }

    for (locon = ptfig->LOCON; locon != NULL; locon = locon->NEXT) {
        if (getptype(locon->USER, TAS_CON_TAGCONE) != NULL)
            locon->USER = delptype(locon->USER, TAS_CON_TAGCONE);
    }

    for (cone = ptfig->CONE; cone != NULL; cone = cone->NEXT) {
        cone->TYPE &= ~(TAS_MARQUE);
    }
    return (0);
}

/****************************************************************************/
/*                           fonction tas_DetectConeConnect()               */
/* type TAS_CONE_CONNECT les cones connecteurs                              */
/****************************************************************************/
int tas_DetectConeConnect(cnsfig)
     cnsfig_list *cnsfig ;

{
  locon_list *locon ;

  /*--------------------------------------------------------------------------*/
  /* un cone connecteur est un cone qui a en entree un connecteur de direct.  */
  /* 'I' mais qui a dans son champ USER un pointeur qui pointe dessus avec le */
  /* mot clef EXT.                                                            */
  /*--------------------------------------------------------------------------*/
  for(locon = cnsfig->LOCON ; locon != NULL ; locon = locon->NEXT)
    {
      locon->NAME = ttv_revect(locon->NAME) ;

      if(locon->DIRECTION == 'I')
        {
          cone_list *cone;
          ptype_list *ptype = getptype(locon->USER,CNS_EXT);
          if(ptype!=NULL) 
            {
              cone = (cone_list *)ptype->DATA ;
              if(cone != NULL) cone->TYPE |= TAS_CONE_CONNECT ;
            }
        }
    }

  return(0) ;
}


/****************************************************************************/
/*                           fonction tas_prelink()                         */
/* range les dimensions du transistor du maillon dans le champ USER du      */
/* maillon. La structure utilisee est la structure bilong qui est la meme   */
/* pour FUP et FDOWN. Retourne le nombre de maillons traites.               */
/****************************************************************************/
long tas_prelink(ptfig)
     cnsfig_list *ptfig ;

{
  cone_list *cone ;
  ptype_list *ptype ;
  chain_list *chain ;
  lotrs_list *lotrs ;

  init_timing(ptfig) ;

  /* pour tous les cones de la figure */
  for(cone = ptfig->CONE ; cone != NULL ; cone = cone->NEXT)
    {
      short i;
      branch_list *path[3] ;

      TAS_INFO.nb_cone ++ ;
      /* pour toutes les banches du cone */
      path[0] = cone->BREXT;
      path[1] = cone->BRVDD;
      path[2] = cone->BRVSS;
      for(i = 0 ; i < 3 ; i++)
        for(; path[i] != NULL ; path[i] = path[i]->NEXT)
          {
            link_list *link ;

            /* pour tous les maillons de la branche */
            for(link = path[i]->LINK ; link != NULL ; link = link->NEXT)
              {
                long lshrink;
                long wshrink;
                caraclink_list *caraclink ;
                if((link->TYPE & (CNS_IN | CNS_INOUT)) == 0) 
                  {
                    caraclink = TAS_GETCLINK(link) ;
                    elpLotrsGetShrinkDim(link->ULINK.LOTRS,
                                         &lshrink,
                                         &wshrink,
                                         NULL,
                                         NULL,
                                         NULL,
                                         NULL,
                                         NULL,
                                         NULL,
                                         TAS_CASE
                                         );
                    caraclink->WIDTH = wshrink;
                    caraclink->LENGTH = lshrink;

                    ptype = getptype( link->ULINK.LOTRS->USER, MBK_TRANS_PARALLEL );
                    if( ptype ) {
                      chain = (chain_list*)ptype->DATA ;
                      if( chain ) {
                        for( chain = chain->NEXT ; chain ; chain = chain->NEXT ) {
                          lotrs = (lotrs_list*)chain->DATA ;
                          elpLotrsGetShrinkDim(lotrs,
                                               &lshrink,
                                               &wshrink,
                                               NULL,
                                               NULL,
                                               NULL,
                                               NULL,
                                               NULL,
                                               NULL,
                                               TAS_CASE
                                               );
                          caraclink->WIDTH = tas_get_width_equiv_lotrs( link->ULINK.LOTRS,
                                                                        lotrs,
                                                                        TAS_GETWIDTH( link ),
                                                                        TAS_GETLENGTH( link ),
                                                                        wshrink,
                                                                        lshrink
                                                                      );
                        }
                      }
                    }
                  }
              } /* fin des maillons */
          } /* fin des branches */
    } /* fin des cones */

  return(0) ;
}


/*****************************************************************************
 *                              fonction  delpath()                           *
 *                           destruction d'un chemin                          *
 *****************************************************************************/
int delpath(cone,path)
     cone_list *cone ; /* cone ou il y a la branche */
     branch_list *path ; /* branche a detruire        */
{
  link_list *nextlink = NULL ;
  int       resultat = 0 ;
  ptype_list *ptype;

  /* on detruit la branche */
  if(path == cone->BREXT) /* si c'est la premiere */
    {
      cone->BREXT = path->NEXT ;
      resultat = 1 ;
    }
  if(path == cone->BRVDD) /* si c'est la premiere */
    {
      cone->BRVDD = path->NEXT ;
      resultat = 1 ;
    }
  if(path == cone->BRVSS) /* si c'est la premiere */
    {
      cone->BRVSS = path->NEXT ;
      resultat = 1 ;
    }
  else
    {
      branch_list *pt_path[3] ;
      short i;

      pt_path[0] = cone->BREXT;
      pt_path[1] = cone->BRVDD;
      pt_path[2] = cone->BRVSS;

      for(i = 0 ; i < 3 ; i++)
        for(; pt_path[i] != NULL ; pt_path[i] = pt_path[i]->NEXT)
          { /* pour chaque chemin */
            if(pt_path[i]->NEXT == path)
              {
                pt_path[i]->NEXT = path->NEXT ;
                resultat = 1 ;
                break ;
              }
          }
    }

  path->NEXT = NULL ;
  for(nextlink = path->LINK ; nextlink != NULL ; nextlink = nextlink->NEXT)
    {
      if ((ptype=getptype(nextlink->USER,TAS_LINK_BLDDOWN))!=NULL) freechain((chain_list *)ptype->DATA);
      if ((ptype=getptype(nextlink->USER,TAS_LINK_BLDUP))!=NULL) freechain((chain_list *)ptype->DATA);
      freeptype(nextlink->USER) ;
    }
  freeptype(path->USER) ;
  freebranch(path) ;

  return(resultat) ;
}

long tas_get_width_equiv_lotrs( lotrs_list *lotrs1, lotrs_list *lotrs2, long w1, long l1, long w2, long l2 )
{
        float i1, i2, i ;
        float vdd ;
        int r1, r2 ;
        double largeur ;
        double vbulk ;

        vdd = tas_getparam( lotrs1, TAS_CASE, TP_VDDmax );
        elp_lotrs_param_get( lotrs1, NULL,NULL, NULL,  NULL,NULL, NULL, NULL, NULL,NULL,&vbulk, NULL, NULL, NULL, NULL );
        if( ( lotrs1->TYPE & CNS_TN ) == CNS_TN )
          r1 = tas_get_lotrs_current( lotrs1, w1, l1, vdd, vdd, vbulk, &i1 );
        else
          r1 = tas_get_lotrs_current( lotrs1, w1, l1, -vdd, -vdd, vbulk-vdd, &i1 );

        vdd = tas_getparam( lotrs2, TAS_CASE, TP_VDDmax );
        elp_lotrs_param_get( lotrs2, NULL,NULL, NULL,  NULL,NULL, NULL, NULL, NULL,NULL,&vbulk, NULL, NULL, NULL, NULL );
        if( ( lotrs2->TYPE & CNS_TN ) == CNS_TN )
          r2 = tas_get_lotrs_current( lotrs2, w2, l2, vdd, vdd, vbulk, &i2 );
        else
          r2 = tas_get_lotrs_current( lotrs2, w2, l2, -vdd, -vdd, vbulk-vdd, &i2 );
      
        if( !r1 || !r2 ) {
            largeur = (double)w1 ;
            largeur += (double)w2*((double)l1/(double)l2);
        }
        else {
          i = i1+i2 ;
          largeur = (double)w1 * i/i1 ;
        }

        return (long)largeur ;
}

/******************************************************************************
 *                   fonction del_para()                                       *
 * elimine les branches paralleles dans un cone                                *
 ******************************************************************************/
int del_para(cone,chaine1,chaine2)
     cone_list *cone ;
     chain_list *chaine1 ;
     chain_list *chaine2 ;

{
  branch_list *chemin2;
  link_list *maillon1,*maillon2;
  lotrs_list *trans1,*trans2;
  char transfor='n';

  chemin2=(branch_list *)chaine2->DATA;
  maillon1=(link_list *)((branch_list *)chaine1->DATA)->LINK;
  maillon2=(link_list *)chemin2->LINK;

  if((maillon1->TYPE & (CNS_IN | CNS_INOUT)) == 0)
    /* on ne calcule pas une largeur equivalente entre deux memes transistors */
    if (((trans1=maillon1->ULINK.LOTRS)!=(trans2=maillon2->ULINK.LOTRS)) &&
        (getptype(trans2->USER,TAS_TRANS_USED) == NULL))
      {
        long largeur ;
        largeur = tas_get_width_equiv_lotrs( maillon1->ULINK.LOTRS, 
                                             maillon2->ULINK.LOTRS,
                                             TAS_GETWIDTH( maillon1 ),
                                             TAS_GETLENGTH( maillon1 ),
                                             TAS_GETWIDTH( maillon2 ),
                                             TAS_GETLENGTH( maillon2 )
                                           );
        TAS_GETCLINK(maillon1)->WIDTH = largeur ;
        trans2->USER = addptype(trans2->USER,TAS_TRANS_USED,(void*)NULL) ;
        transfor='o';
      }

  maillon1=maillon1->NEXT;
  maillon2=maillon2->NEXT;

  while((maillon1 != NULL) && ((maillon1->TYPE & (CNS_IN | CNS_INOUT))== 0))
    {
      if(((trans1=maillon1->ULINK.LOTRS)!=(trans2=maillon2->ULINK.LOTRS)) &&
         (getptype(trans2->USER,TAS_TRANS_USED) == NULL))
        {
          long largeur ;
          largeur = tas_get_width_equiv_lotrs( maillon1->ULINK.LOTRS, 
                                               maillon2->ULINK.LOTRS,
                                               TAS_GETWIDTH( maillon1 ),
                                               TAS_GETLENGTH( maillon1 ),
                                               TAS_GETWIDTH( maillon2 ),
                                               TAS_GETLENGTH( maillon2 )
                                             );
          TAS_GETCLINK(maillon1)->WIDTH = largeur ;

          trans2->USER = addptype(trans2->USER,TAS_TRANS_USED,(void*)NULL) ;
          if (transfor=='o' )
            {
              if(getptype(trans2->USER,TAS_CAPA_USED) == NULL)
                {
                  chain_list *chain = NULL ;
                  ptype_list *ptype ;

                  if((ptype = getptype(trans1->USER,TAS_TRANS_PARA)) != NULL)
                    for(chain = (chain_list *)ptype->DATA ; chain != NULL ; 
                        chain = chain->NEXT)
                      if(chain->DATA == (void*)trans2) break ;
                  if(chain == NULL)
                    {
                      tas_addcapalink(maillon1, tas_getcapalink(NULL, maillon2,TAS_UNKNOWN_EVENT));
                      if(getptype(trans2->USER,TAS_CAPA_USED) == NULL)
                        trans2->USER = addptype(trans2->USER,TAS_CAPA_USED,
                                                (void*)NULL) ; 
                      if((ptype = getptype(trans2->USER,TAS_TRANS_PARA)) != NULL)
                        for(chain = (chain_list *)ptype->DATA ; chain != NULL ; 
                            chain = chain->NEXT)
                          {
                            lotrs_list *trans = ((lotrs_list*)chain->DATA) ;
                            if(getptype(trans->USER,TAS_CAPA_USED) == NULL)
                              trans->USER = addptype(trans->USER,TAS_CAPA_USED,
                                                     (void*)NULL) ; 
                          }
                    }
                }
              transfor='o';
            }
        }
      else transfor ='n';

      maillon1=maillon1->NEXT;
      maillon2=maillon2->NEXT;
    }

  delpath(cone,chemin2);
  return(0);
}

/******************************************************************************
 *                              fonction number_maillon()                      *
 ******************************************************************************/
long number_maillon(chemin0) 
     branch_list *chemin0;

{
  link_list *maillon0;
  long num=0L;

  for(maillon0=(link_list *)chemin0->LINK;maillon0;maillon0=maillon0->NEXT) num++;

  avt_log (LOGTAS, 3, "number_maill      :%ld\n",num);   

  return(num);
}


/******************************************************************************
 *                               fonction tas_para()                           *
 ******************************************************************************/
int tas_para(cone0)
     cone_list *cone0;

{
  chain_list *chaine0,*chaine1,*chaine2;  
  chain_list *chbl = (chain_list *)getptype(cone0->USER,CNS_BLEEDER) ;
  branch_list  *chemin1;

  avt_log (LOGTAS, 3, "para      : cone(%ld)\n",cone0->INDEX);

  /*****************************************************************************/
  /* dans la liste des branches paralleles, il y a des branches non fonction-  */
  /* nelles: en fait soit elles sont toutes fonctionnelles, soit aucune. On    */
  /* traite alors seulement la premiere branche. Si elle est fonctionnelle on  */
  /* fait le traitement sinon on sort de la premiere boucle. D'autre part ce   */
  /* n'est pas la peine de tester si getptype retourne NULL ou non: cette      */
  /* fonction n'est appelee que si le cone a des branches paralleles.          */
  /*****************************************************************************/

  for(chaine0 = (chain_list *)getptype(cone0->USER,CNS_PARALLEL)->DATA ;
      chaine0 != NULL ; chaine0 = chaine0->NEXT)
    {
      for(chaine1 = (chain_list *)chaine0->DATA ; chaine1 != NULL ;
          chaine1 = chaine1->NEXT)
        {
          branch_list *branch1 = (branch_list *)chaine1->DATA ;

          for(chaine2 = (chain_list *)chaine1 ; chaine2 != NULL ;
              chaine2 = chaine2->NEXT)
            {
              branch_list *branch2 = (branch_list *)chaine2->DATA ;
              link_list *link1 = branch1->LINK ;
              link_list *link2 = branch2->LINK ;
              char prevtrans = 'n' ;

              if(chaine2 == chaine1) continue ;
              for(;(link1 != NULL) || (link2 != NULL) ; 
                  link1 = link1->NEXT , link2 = link2->NEXT)
                {
                  lotrs_list *trans1 ;
                  lotrs_list *trans2 ;

                  if(((link1->TYPE & (CNS_IN | CNS_INOUT)) != 0) ||
                     ((link2->TYPE & (CNS_IN | CNS_INOUT)) != 0))
                    break ;
                  trans1 = link1->ULINK.LOTRS ;
                  trans2 = link2->ULINK.LOTRS ;

                  if((prevtrans == 'o') && (trans1 != trans2))
                    {
                      ptype_list *ptype ;
                      if((ptype = getptype(trans1->USER,TAS_TRANS_PARA)) == NULL)
                        {
                          trans1->USER = addptype(trans1->USER,TAS_TRANS_PARA,
                                                  addchain(NULL,(void*)trans2)) ;
                        }
                      else
                        {
                          chain_list *chain = (chain_list *)ptype->DATA ;

                          for(;chain != NULL ; chain = chain->NEXT)
                            if((lotrs_list*)chain->DATA == trans2) break ;
                          if(chain == NULL) 
                            ptype->DATA = (void*)addchain(ptype->DATA,trans2) ;
                        }
                      if((ptype = getptype(trans2->USER,TAS_TRANS_PARA)) == NULL)
                        {
                          trans2->USER = addptype(trans2->USER,TAS_TRANS_PARA,
                                                  addchain(NULL,(void*)trans1)) ;
                        }
                      else
                        {
                          chain_list *chain = (chain_list *)ptype->DATA ;

                          for(;chain != NULL ; chain = chain->NEXT)
                            if((lotrs_list*)chain->DATA == trans1) break ;
                          if(chain == NULL) 
                            ptype->DATA = (void*)addchain(ptype->DATA,trans1) ;
                        }
                    }
                  if(trans1 == trans2) prevtrans = 'o' ;
                  else prevtrans = 'n' ;
                }
            }
        }
    }

  for(chaine0 = (chain_list *)getptype(cone0->USER,CNS_PARALLEL)->DATA ;
      chaine0 != NULL ; chaine0 = chaine0->NEXT)
    {
      chain_list *auxbl = chbl ;

      chaine1=(chain_list *)chaine0->DATA;
      chaine2=chaine1;

      /* initialisation */
      chemin1=(branch_list *) chaine2->DATA;
    
      /* recherche de la fonctionnalite de la branche */ /* modif. du 26/06/91 */
      for( /* initialise */ ; (auxbl != NULL) &&
                              ((branch_list *)auxbl->DATA != chemin1) ; auxbl = auxbl->NEXT) ;

      if(((chemin1->TYPE & CNS_NOT_FUNCTIONAL) == CNS_NOT_FUNCTIONAL) && (auxbl == NULL)) 
        continue ;

      for(chaine2 = chaine1 ; chaine2 != NULL ; chaine2 = chaine2->NEXT)
        {
          link_list *link ;
          ptype_list *ptype ;
          chemin1=(branch_list *) chaine2->DATA;
          for(link = chemin1->LINK ; link != NULL ; link = link->NEXT)
            {
              if((link->TYPE & (CNS_IN | CNS_INOUT)) != 0) break ;
              if(getptype(link->ULINK.LOTRS->USER,TAS_TRANS_USED) != NULL)
                link->ULINK.LOTRS->USER = delptype(link->ULINK.LOTRS->USER,
                                                   TAS_TRANS_USED) ;
              if(getptype(link->ULINK.LOTRS->USER,TAS_CAPA_USED) != NULL)
                link->ULINK.LOTRS->USER = delptype(link->ULINK.LOTRS->USER,
                                                   TAS_CAPA_USED) ;
              if((ptype = getptype(link->ULINK.LOTRS->USER,TAS_TRANS_PARA)) != NULL)
                {
                  freechain((chain_list*)ptype->DATA);
                  link->ULINK.LOTRS->USER = delptype(link->ULINK.LOTRS->USER,
                                                     TAS_TRANS_PARA) ;
                }
            }
        }
      chaine2 = chaine1->NEXT ;
      while(chaine2 != NULL)
        {
          del_para(cone0,chaine1,chaine2);
          chaine2=chaine2->NEXT;
        }
    }
  return(0);
}
/******************************************************************************
 *                              fonction tas_addptype_switch_cmos()            *
 ******************************************************************************/
void tas_addptype_switch_cmos(cone_list *cone)
{
  chain_list  *chainswitch;
  chain_list  *chainlotrs;
  chain_list  *chainlink;
  lotrs_list  *lotrs1, *lotrs2;
  ptype_list  *ptype;
  branch_list *branch[3];
  link_list   *link;
  int          i;

  if((ptype = getptype(cone->USER,CNS_SWITCH)) == NULL) return;
  if(!ptype->DATA) return;
    
  for(chainswitch = ptype->DATA; chainswitch; chainswitch = chainswitch->NEXT){
    for(chainlotrs = chainswitch->DATA; chainlotrs; chainlotrs = chainlotrs->NEXT){
      if(chainlotrs->NEXT){
        lotrs1 = chainlotrs->DATA;
        lotrs2 = chainlotrs->NEXT->DATA;
        lotrs1->USER = addptype(lotrs1->USER, TAS_TRANS_SWITCH, lotrs2);
        lotrs2->USER = addptype(lotrs2->USER, TAS_TRANS_SWITCH, lotrs1);
      }
    }
  }

  branch[0] = cone->BREXT;
  branch[1] = cone->BRVDD;
  branch[2] = cone->BRVSS;
  for (i = 0; i < 3; i++){
    for (; branch[i]; branch[i] = branch[i]->NEXT){
      for (link = (link_list *) branch[i]->LINK; link; link = link->NEXT){
        if((link->TYPE & CNS_SWITCH) == CNS_SWITCH){
          lotrs1 = link->ULINK.LOTRS;
          if((ptype = getptype(lotrs1->USER, TAS_TRANS_LINK)) != NULL){
            ptype->DATA = addchain((chain_list *)ptype->DATA, link);
          }else{
            chainlink = addchain(NULL, link);
            lotrs1->USER = addptype(lotrs1->USER, TAS_TRANS_LINK, chainlink);
          }
        }
      }
    }
  }
}

                  
/******************************************************************************
 *                              fonction tas_switch_cmos()                     *
 ******************************************************************************/
int tas_switch_cmos(cone0)
     cone_list *cone0;

{
  short i;
  branch_list *chemin0[3],*chemin1;
  link_list *maillon0;
  char change='n'; 

  avt_log (LOGTAS, 3, "switch_cmos  : cone(%ld)\n",cone0->INDEX);

  chemin0[0]=cone0->BREXT;
  chemin0[1]=cone0->BRVDD;
  chemin0[2]=cone0->BRVSS;

  for(i = 0 ; i < 3 ; i++)
    while (chemin0[i] != NULL)
      {
        maillon0=(link_list *) chemin0[i]->LINK;

        while(maillon0 != NULL)
          {
            if((maillon0->TYPE & CNS_SWITCH)==CNS_SWITCH) 
              {
                /* AJOUT DU 19/05/1994: traitement des switches en entree */
                if((chemin0[i]->TYPE & CNS_EXT) == CNS_EXT)
                  chemin0[i]->TYPE |= TAS_SWITCH ;

                else if(((maillon0->ULINK.LOTRS->TYPE & CNS_TP)==CNS_TP) 
                        && ((chemin0[i]->TYPE & CNS_VSS)== CNS_VSS))
                  {
                    chemin1=chemin0[i];
                    chemin0[i]=chemin0[i]->NEXT;        
                    chemin1->TYPE |= TAS_SWITCH_DEG | CNS_DEGRADED ;
                    if(TAS_CONTEXT->TAS_DELAY_SWITCH == 'N')
                      delpath(cone0,chemin1);
                    change='o';
                    break;
                  }

                else if(((maillon0->ULINK.LOTRS->TYPE & CNS_TN)== CNS_TN) 
                        &&((chemin0[i]->TYPE & CNS_VDD)== CNS_VDD))
                  {
                    chemin1=chemin0[i];
                    chemin0[i]=chemin0[i]->NEXT;        
                    chemin1->TYPE |= TAS_SWITCH_DEG | CNS_DEGRADED;
                    if(TAS_CONTEXT->TAS_DELAY_SWITCH == 'N')
                      delpath(cone0,chemin1);
                    change='o';
                    break;
                  }
              }
            maillon0=maillon0->NEXT;
          }

        if(change=='o') change='n';
        else chemin0[i]=chemin0[i]->NEXT;
      }
  return(0);
}


/*****************************************************************************/
/*                            fonction tas_AddCapaDiff()                     */
/* calcul la capacite de diffusion des transistors                           */
/*****************************************************************************/
void tas_AddCapaDiff(lofig)
     lofig_list *lofig ;
{
  elpLofigCapaDiff(lofig,TAS_CASE) ;
}


/*****************************************************************************/
/*                          fonction tas_AddCapaOut()                        */
/* rajoute la capacite de sortie passee par cout sur les connecteurs         */
/*****************************************************************************/
void tas_AddCapaOut(cnsfig)
     cnsfig_list *cnsfig ;
{
  locon_list *locon ;

  for(locon = cnsfig->LOCON ; locon != NULL ; locon = locon->NEXT)
    {
      if((locon->DIRECTION == CNS_O) || (locon->DIRECTION == CNS_B) ||
         (locon->DIRECTION == CNS_Z) || (locon->DIRECTION == CNS_T)) 
        {
          ptype_list *ptype = (ptype_list *)getptype(locon->USER,CNS_EXT) ;
          cone_list *cone ;
          branch_list *path[3] ;
          link_list *link ;
          chain_list *chain ;
          int i ;

          if(ptype == NULL){
            tas_error(4,(char *)locon->NAME,TAS_ERROR) ;
            continue;
          }
          cone = (cone_list *)ptype->DATA ;

          path[0] = cone->BRVDD ;
          path[1] = cone->BRVSS ;
          path[2] = cone->BREXT ;
          for(i= 0 ; i < 3 ; i++){
            for(; path[i] != NULL ; path[i] = path[i]->NEXT)
              {
                link_list *link = path[i]->LINK ;
                tas_addcapalink(link, tas_get_cone_output_capacitance(cone)/*TAS_CONTEXT->TAS_CAPAOUT*/ *1000.0);
              }
          }
          ptype = (ptype_list *)getptype(locon->USER,CNS_CONE) ;
          if(ptype != NULL)
            for(chain = (chain_list*)ptype->DATA ; chain != NULL ; 
                chain = chain->NEXT)
              {
                cone_list *cone2 = (cone_list *)chain->DATA ;
                if(cone2 != cone){
                  for(path[0] = cone2->BREXT ; path[0] != NULL ; path[0] = path[0]->NEXT){
                    for(link = path[0]->LINK ; link->NEXT != NULL ; link = link->NEXT) ;
                    if(link->ULINK.LOCON == locon){
                      tas_addcapalink(link, tas_get_cone_output_capacitance(cone)/*TAS_CONTEXT->TAS_CAPAOUT*/ *1000.0);
                    }
                  }
                }
              }
        }
    }
}


/*****************************************************************************/
/*                               fonction tas_NewConeAlim()                  */
/* pour un cone alim donne, il type les entrees de ses sorties en VDD ou VSS.*/
/*****************************************************************************/
int tas_NewConeAlim(cone,level)
     cone_list *cone ;
     long      level ;

{
  edge_list *outcone;

  long      type     = (level == INF_LL_STUCKVSS) ? CNS_VSS : CNS_VDD ;

  avt_log (LOGTAS, 2, "Stuck %s : %ld\n",cone->NAME,level);   

  cone->TYPE |= type ;

  for(outcone = cone->OUTCONE ; outcone != NULL ;
      outcone = outcone->NEXT)
    if((outcone->TYPE & (CNS_EXT|CNS_BLEEDER|TAS_IN_MEMSYM|CNS_FEEDBACK)) == 0)
      {
        edge_list *incone = ((cone_list * )outcone->UEDGE.CONE)->INCONE ;

        for( /* initialise */ ; (incone != NULL) && (incone->UEDGE.CONE != cone) ;
                              incone = incone->NEXT) ;

        /* si entree pas trouvee => erreur */
        if(incone == NULL)
          tas_error(12,(char *)outcone->UEDGE.CONE,TAS_ERROR) ;
        else
          {
            incone->TYPE &= ~CNS_CONE ;
            incone->TYPE |= type | CNS_FEEDBACK ;
          }
      } /* fin de parcours des sorties */

  return(0) ;
}

/*****************************************************************************/
/*                               fonction tas_StuckAt()                      */
/* create the stuck at markings for tas (warning: this may be incompatible   */
/* with differential latch markings)                                         */
/*****************************************************************************/
int tas_StuckAt(cnsfig)
     cnsfig_list *cnsfig ;
{
  cone_list *cone ;
  locon_list *locon ;
  ptype_list *ptuser ; 

  for(cone = cnsfig->CONE ; cone != NULL ; cone = cone->NEXT)
    {
      if((cone->TECTYPE & CNS_ONE) == CNS_ONE) tas_NewConeAlim(cone, INF_LL_STUCKVDD);
      if((cone->TECTYPE & CNS_ZERO) == CNS_ZERO) tas_NewConeAlim(cone, INF_LL_STUCKVSS);
      if((cone->TECTYPE & (CNS_ONE|CNS_ZERO)) != 0 && (cone->TYPE & CNS_EXT) == CNS_EXT)
        {
          chain_list *cl;
          cl=cns_get_cone_external_connectors(cone);
//            ptuser = getptype(cone->USER, CNS_EXT);
          while (cl!=NULL)
          {
            if (/*ptuser != NULL &&*/ TAS_CONTEXT->TAS_PRES_CON_DIR == 'N')
              {
                locon = (locon_list *)cl->DATA;
                locon->DIRECTION = UNKNOWN;
              }
            cl=delchain(cl,cl);
          }
        }
    }

  if (TAS_CONTEXT->TAS_PRES_CON_DIR == 'N')
    {
      for(locon = cnsfig->LOCON ; locon ; locon = locon->NEXT)
        {
          if (!mbk_LosigIsVDD(locon->SIG) && !mbk_LosigIsVSS(locon->SIG))
          {
            ptuser = getptype(locon->USER, CNS_TYPELOCON);
            if (ptuser != NULL)
              {
                if (((long)ptuser->DATA & (CNS_ZERO|CNS_ONE)) != 0)
                  {
                    locon->DIRECTION = UNKNOWN;
                  }
              }
          }
        }
    }
  return(0) ;
}

/****************************************************************************/
/*                            fonction tas_AddList()                        */
/* rajoute un list_list a la liste pointee par head.                        */
/****************************************************************************/
list_list *tas_AddList(head,type,data,user)
     list_list  *head ;
     long       type  ;
     char       *data ;
     void       *user ;

{
  list_list *aux = (list_list *)mbkalloc(sizeof(list_list)) ;

  aux->NEXT = head ;
  aux->DATA = data ;
  aux->TYPE = type ;
  aux->USER = user ;

  return(aux) ;
}

/****************************************************************************/
/*                          fonction tas_traiteinout()                      */
/* traitement des pathin et pathout du fichier .inf                         */
/****************************************************************************/
//list_list *tas_traiteinout(list)
//     list_list  *list  ;
void tas_traiteinout(inffig_list *ifl)
{
  chain_list *lst0;
/*  list_list  *auxlist = list ;
  list_list  *aux ;*/
  char flag = 'N' ;

  lst0=inf_GetEntriesByType(ifl, INF_PATHIN, INF_ANY_VALUES);
  if (lst0!=NULL) { TTV_NAME_IN=append(lst0, TTV_NAME_IN); flag = 'Y' ; }

  lst0=inf_GetEntriesByType(ifl, INF_PATHOUT, INF_ANY_VALUES);
  if (lst0!=NULL) { TTV_NAME_OUT=append(lst0, TTV_NAME_OUT); flag = 'Y' ; }
/*
  while(auxlist != NULL)
    {
      if(auxlist->TYPE == INF_PATHIN)
        {
          TTV_NAME_IN=addchain(TTV_NAME_IN,
                               (void *)namealloc((char *)auxlist->DATA));
          flag = 'Y' ;
          if(auxlist == list)
            {
              list = list->NEXT ;
              mbkfree(auxlist) ;
              auxlist = list ;
            }
          else
            {
              aux->NEXT = auxlist->NEXT ;
              mbkfree(auxlist) ;
              auxlist = aux->NEXT ;
            }
          continue ;
        }
      if(auxlist->TYPE == INF_PATHOUT)
        {
          TTV_NAME_OUT=addchain(TTV_NAME_OUT,
                                (void *)namealloc((char *)auxlist->DATA));
          flag = 'Y' ;
          if(auxlist == list)
            {
              list = list->NEXT ;
              mbkfree(auxlist) ; 
              auxlist = list ;
            }
          else
            {
              aux->NEXT = auxlist->NEXT ;
              mbkfree(auxlist) ;
              auxlist = aux->NEXT ;
            }
          continue ;
        }
      aux = auxlist ;
      auxlist = auxlist->NEXT ;
    }
*/
  if((TTV_PATH_SAVE == TTV_SAVE_ALLPATH) && (flag == 'Y'))
    {
      TTV_PATH_SAVE = TTV_SAVE_PATH ;
    }

  if((TTV_NAME_IN == NULL) && (TTV_NAME_OUT != NULL))
    TTV_NAME_IN = addchain(NULL,(void *)namealloc("*")) ;
  if((TTV_NAME_IN != NULL) && (TTV_NAME_OUT == NULL))
    TTV_NAME_OUT = addchain(NULL,(void *)namealloc("*")) ;

//  return(list) ;
}

/****************************************************************************/
/*                          fonction tas_GetAliasSig()                      */
/* remplace les noms du fichier '.inf' par l'alias le plus significatif.    */
/****************************************************************************/
list_list *tas_GetAliasSig(lofig,list)
     lofig_list *lofig ;
     list_list  *list  ;
{
  losig_list *losig          ;
  list_list  *auxlist = NULL ;
  inffig_list *ifl;

  if(lofig != NULL)
    {
      ifl=getloadedinffig(lofig->NAME);
      while(list != NULL)
        {
          char *name ;
          char *ptsubd = ttv_devect(list->DATA) ;
          char      flag1 = 'N' ;
          char      flag  = 'N' ;
          list_list *aux  = list->NEXT ;

          for(losig = lofig->LOSIG ; losig != NULL ; losig = losig->NEXT)
            {
              chain_list *namechain ;
              if(losig->TYPE == EXTERNAL)
                {
                  locon_list *locon ;

                  for(locon = lofig->LOCON ;(locon != NULL) && (locon->SIG != losig) ;
                      locon = locon->NEXT) ;
                  if(locon == NULL)
                    tas_error(15,list->DATA,TAS_ERROR) ;
                  for(namechain = losig->NAMECHAIN ; namechain != NULL ;
                      namechain = namechain->NEXT)
                    if(namechain->DATA == locon->NAME)
                      break ;
                  if(namechain == NULL)
                    {
                      losig->NAMECHAIN = addchain(losig->NAMECHAIN,locon->NAME) ;
                    }
                }

              /* recherche du signal ayant le meme nom */
              for(namechain = losig->NAMECHAIN ; namechain != NULL ;
                  namechain = namechain->NEXT)
                //if((namechain->DATA == ptsubd) ||
                //   (ttv_jokersubst(ttv_revect(namechain->DATA),list->DATA,'*') == 1))
                if((namechain->DATA == ptsubd) ||
                   (mbk_TestREGEX(ttv_revect(namechain->DATA),list->DATA)))
                  {
                    /* si signal externe, on prend le nom du connecteur */
                    if(losig->TYPE == 'E')
                      {
                        locon_list *locon ;

                        for(locon = lofig->LOCON ;
                            (locon != NULL) && (locon->SIG != losig) ;
                            locon = locon->NEXT) ;

                        if(locon == NULL)
                          tas_error(15,list->DATA,TAS_ERROR) ;

                        name = locon->NAME ;
                      }
                    else name = (char *)yagGetName(ifl,losig) ;

                    switch(list->TYPE)
                      {
                      case INF_LL_STUCKVDD  : if(TAS_CONTEXT->TAS_HIER == 'Y')
                        {
                          if(getptype(losig->USER,TAS_SIG_VDD)
                             == NULL)
                            losig->USER = addptype(losig->USER,
                                                   TAS_SIG_VDD,NULL) ;
                        }
                        break ;
                      case INF_LL_STUCKVSS  : if(TAS_CONTEXT->TAS_HIER == 'Y')
                        {
                          if(getptype(losig->USER,TAS_SIG_VSS)
                             == NULL)
                            losig->USER = addptype(losig->USER,
                                                   TAS_SIG_VSS,NULL) ;
                        }
                        break ;
                      case INF_LL_BYPASSIN  : if(TAS_CONTEXT->TAS_HIER == 'Y')
                        {
                          if(getptype(losig->USER,TAS_SIG_BYPASSIN) 
                             == NULL)
                            losig->USER = addptype(losig->USER,
                                                   TAS_SIG_BYPASSIN,NULL) ;
                        }
                        break ;
                      case INF_LL_BYPASSOUT : if(TAS_CONTEXT->TAS_HIER == 'Y')
                        {
                          if(getptype(losig->USER,TAS_SIG_BYPASSOUT) 
                             == NULL)
                            losig->USER = addptype(losig->USER,
                                                   TAS_SIG_BYPASSOUT,NULL) ;
                        }
                        break ;
                      case INF_LL_ONLYEND   : if(TAS_CONTEXT->TAS_HIER == 'Y')
                        {
                          if(getptype(losig->USER,TAS_SIG_ONLYEND) 
                             == NULL)
                            losig->USER = addptype(losig->USER,
                                                   TAS_SIG_ONLYEND,NULL) ;
                        }
                        break ;
                      case INF_LL_BYPASS    : if(TAS_CONTEXT->TAS_HIER == 'Y')
                        {
                          if(getptype(losig->USER,TAS_SIG_BYPASSIN) 
                             == NULL)
                            losig->USER = addptype(losig->USER,
                                                   TAS_SIG_BYPASSIN,NULL) ;
                          if(getptype(losig->USER,TAS_SIG_BYPASSOUT) 
                             == NULL)
                            losig->USER = addptype(losig->USER,
                                                   TAS_SIG_BYPASSOUT,NULL) ;
                        }
                        break ;
                      case INF_LL_INTER     : if(TAS_CONTEXT->TAS_HIER == 'Y')
                        {
                          if(getptype(losig->USER,TAS_SIG_INTER) 
                             == NULL)
                            losig->USER = addptype(losig->USER,
                                                   TAS_SIG_INTER,NULL) ;
                        }
                        break ;
                      case INF_LL_BREAK     : if(TAS_CONTEXT->TAS_HIER == 'Y')
                        {
                          if(getptype(losig->USER,TAS_SIG_BREAK) 
                             == NULL)
                            losig->USER = addptype(losig->USER,
                                                   TAS_SIG_BREAK,NULL) ;
                        }
                        break ;
                      case INF_LL_RC        : if(getptype(losig->USER,TAS_SIG_RC) == NULL)
                        losig->USER = addptype(losig->USER,
                                               TAS_SIG_RC,NULL) ;
                        break ;
                      case INF_LL_NORC      : if(getptype(losig->USER,TAS_SIG_NORC) 
                                              == NULL)
                        losig->USER = addptype(losig->USER,
                                               TAS_SIG_NORC,NULL) ;
                        break ;
                      }

                    if((name != (char*)namechain->DATA) || (name != (char *)list->DATA))
                      auxlist = tas_AddList(auxlist,list->TYPE,ttv_revect(name),list->USER) ;
                    else if(flag1 == 'N')
                      {
                        auxlist = tas_AddList(auxlist,list->TYPE,list->DATA,list->USER) ;
                        flag1 = 'Y' ;
                      }

                    flag = 'Y' ;
                    /* ce signal ne nous interesse plus => break */
                    break ;
                  }

              /*-----------------------------------------------------------*/
              /* s'il n'y a pas de joker dans le nom, le signal est trouve */
              /* et il n'y en a pas d'autre: ce n'est pas la peine de con- */
              /* tinuer a en chercher dans les autres signaux.             */
              /*-----------------------------------------------------------*/
              if((namechain != NULL) && (namechain->DATA == list->DATA))
                break ;
            } /* fin de la boucle sur les signaux */

          if(flag == 'N')
            {
              if(TAS_CONTEXT->TAS_HIER == 'N')
                auxlist = tas_AddList(auxlist,list->TYPE,list->DATA,list->USER) ;
              /*else
                tas_error(9,list->DATA,TAS_WARNING) ;*/
            }

          mbkfree(list) ;
          list = aux ;
        } /* fin du parcours de la liste des noms du '.inf'*/
    }
  return(auxlist) ;
}


/****************************************************************************/
/*                         fonction tas_DifLatch()                          */
/* traitement des latches BULLIT: les commandes ne sont pas sur le premier  */
/* maillon des branches, mais sur le dernier maillon afin de limiter les    */
/* bruits. Cette fonction traite les entrees commandes.                     */
/****************************************************************************/
int tas_DifLatch(cnsfig)
     cnsfig_list *cnsfig ;

{
  cone_list *cone ;

  for(cone = cnsfig->CONE ; cone != NULL ; cone = cone->NEXT)
    if(((cone->TYPE & (CNS_MEMSYM | CNS_LATCH)) == (CNS_MEMSYM | CNS_LATCH)) ||
       ((cone->TYPE & (CNS_MEMSYM | CNS_FLIP_FLOP)) == (CNS_MEMSYM | CNS_FLIP_FLOP)))
      {
        branch_list *path[3];
        edge_list  *incone ;
        short i;

        /* deselection des commandes existantes */
        for(incone = cone->INCONE ; incone != NULL ; incone = incone->NEXT)
          incone->TYPE &= ~CNS_COMMAND ;

        path[0] =  ((cone_list *)cone)->BREXT;
        path[1] =  ((cone_list *)cone)->BRVDD;
        path[2] =  ((cone_list *)cone)->BRVSS;

        for(i = 0 ; i < 3 ; i++)
          for(; path[i] != NULL ; path[i] = path[i]->NEXT)
            if((path[i]->TYPE & CNS_NOT_FUNCTIONAL) != CNS_NOT_FUNCTIONAL)
              {
                link_list *link ;

                for(link = (link_list *)path[i]->LINK ; link->NEXT != NULL ;
                    link = link->NEXT) ;

                /* si maillon connecteur => erreur: impossible */
                if((link->TYPE & (CNS_IN | CNS_INOUT)) != 0)
                  tas_error(13,cone->NAME,TAS_ERROR) ;

                if((incone = tas_GetInLink(cone,link)) != NULL)
                  incone->TYPE |= CNS_COMMAND ;
              } /* fin de parcours des branches */
      } /* fin de parcours de la liste des registres */

  return(0) ;
}
/*------------------------------------------------------------------------*/
/*                    tas_AffectConnectorsInputSlew()                     */
/*------------------------------------------------------------------------*/

static chain_list *mergeclist(chain_list *l1, chain_list *l2)
{
  chain_list *cl, *ch;
  for (cl=l1; cl!=NULL; cl=cl->NEXT)
    {
      for (ch=l2; ch!=NULL; ch=ch->NEXT)
        if (cl->DATA==ch->DATA) break;
      if (ch==NULL) l2=addchain(l2, cl->DATA);
    }
  freechain(l1);
  return l2;
}


void tas_AffectConnectorsInputSlew (inffig_list *ifl, cnsfig_list *cnsfig)
{
  locon_list *locon ;
  locon_list *tabcon[2] ;
  int i ;
  double rise, fall;
  chain_list *lst, *cl;
  
  tabcon[0] = cnsfig->LOCON ;
  tabcon[1] = cnsfig->INTCON ;

  for (i = 0 ; i < 2 ; i++ )
    for (locon = tabcon[i]  ; locon ; locon = locon->NEXT) {
      locon->FLAGS=0;
      if ((locon->DIRECTION == CNS_I) || (locon->DIRECTION == CNS_T))
        {
          tas_SetPinSlew (locon, -1, -1) ;
        }
    }

  lst=inf_GetEntriesByType(ifl, INF_PIN_RISING_SLEW, INF_ANY_VALUES);
  cl=inf_GetEntriesByType(ifl, INF_PIN_FALLING_SLEW, INF_ANY_VALUES);

  lst=inf_SortEntries(mergeclist(lst, cl));

  for (cl=lst; cl!=NULL; cl=cl->NEXT)
  {
    if (!inf_GetDouble(ifl, (char *)cl->DATA, INF_PIN_RISING_SLEW, &rise)) rise=-1;
    else rise=mbk_long_round(rise*1e12);
    if (!inf_GetDouble(ifl, (char *)cl->DATA, INF_PIN_FALLING_SLEW, &fall)) fall=-1;
    else fall=mbk_long_round(fall*1e12);

    for (i = 0 ; i < 2 ; i++ )
      for (locon = tabcon[i]  ; locon ; locon = locon->NEXT) {
        if ((locon->DIRECTION == CNS_I) || (locon->DIRECTION == CNS_T))
          {
            if (locon->FLAGS!=3 && mbk_TestREGEX(locon->NAME, (char *)cl->DATA))
            {
              if (locon->FLAGS & 1) rise=-1;
              if (locon->FLAGS & 2) fall=-1;
              tas_SetPinSlew (locon, rise, fall) ;
              if (rise>0) locon->FLAGS|=1;
              if (fall>0) locon->FLAGS|=2;
            }
          }
      }
  }

  freechain(lst);
}

void tas_AffectConnectorsOutputCapacitance (inffig_list *ifl, cnsfig_list *cnsfig)
{
  double c;
  chain_list *list0, *cl;
  cone_list *cone;
  char *name, *revconename;
  
  list0=inf_GetEntriesByType(ifl, INF_OUTPUT_CAPACITANCE, INF_ANY_VALUES);

  for (cone=cnsfig->CONE; list0!=NULL && cone!=NULL; cone=cone->NEXT)
  {
    if ((cone->TYPE & CNS_EXT)!=0)
    {
      revconename=ttv_revect(cone->NAME);
      for (cl=list0; cl!=NULL; cl=cl->NEXT) 
      {
        name=(char *)cl->DATA;

        if (mbk_TestREGEX(revconename, name))
        {
          inf_GetDouble(ifl, name, INF_OUTPUT_CAPACITANCE, &c);
          cone->USER=addptype(cone->USER, TAG_CONE_OUTPUT_CAPACITANCE, NULL);
          *(float *)&cone->USER->DATA=(float)c;
          break;
        }
      }
    }
  }
  freechain(list0);
}

float tas_get_cone_output_capacitance(cone_list *cone)
{
  ptype_list *pt;
  if ((pt=getptype(cone->USER, TAG_CONE_OUTPUT_CAPACITANCE))!=NULL)
  {
    return (*(float *)&pt->DATA)*1e12;
  }
  return TAS_CONTEXT->TAS_CAPAOUT;
}


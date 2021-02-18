/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TAS Version 5                                               */
/*    Fichier : tas_parscns.c                                               */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/
/* transformation cns caracterise en ttvfig                                 */
/****************************************************************************/

#include "tas.h"


ht *TAS_HTAB_NAME = NULL ;
chain_list *DETAILED_MODELS = NULL ;

/*****************************************************************************/
/*                        function tas_getconename()                         */
/* donne le noms d'une entree                                                */
/*****************************************************************************/
char *tas_getconename(cone)
cone_list *cone ;
{
 ptype_list *ptype ;
 chain_list *chain ;
 chain_list *chainx ;
 locon_list *locon ;
 locon_list *loconsav = NULL ;
 char *name ;
 char *noname ;
 char *namesav = NULL ;
 char *ptchar ;
 int nbsepar ;
 int nbseparmin = 1000 ;
 char buf[1024] ;
 long index ;
 char flag ;

 if((ptype = getptype(cone->USER,TAS_CONE_NAME)) != NULL)
   return((char *)ptype->DATA) ;

 if((cone->TYPE & CNS_EXT) == CNS_EXT)
  {
   locon=cns_get_one_cone_external_connector(cone);
   if(locon!=NULL/*(ptype = getptype(cone->USER,CNS_EXT)) != NULL*/)
    {
     if((locon->DIRECTION == CNS_O) || (locon->DIRECTION == CNS_B) ||
        (locon->DIRECTION == CNS_Z) || (locon->DIRECTION == CNS_T))
       noname = locon->NAME ; 
     else
      {
       cone->USER = addptype(cone->USER,TAS_CONE_NAME,(void *)cone->NAME) ;
       return(cone->NAME) ;
      }
    }
   else
    noname = NULL ;
  }
 else
  noname = NULL ;

 ptype = getptype(cone->USER,TAS_RC_CONE) ;

 if((ptype == NULL) || (TAS_CONTEXT->TAS_MERGERCN == 'Y'))
  {
   cone->USER = addptype(cone->USER,TAS_CONE_NAME,(void *)cone->NAME) ;
   return(cone->NAME) ;
  }

 flag = 'N' ;

 for(chain = (chain_list *)ptype->DATA ; chain != NULL ; chain = chain->NEXT)
  {
   locon = (locon_list *)chain->DATA ;
   if(loconsav == NULL)
     loconsav = locon ;
   if((ptype = getptype(locon->USER,PNODENAME)) != NULL)
    {
     for(chainx = (chain_list *)ptype->DATA ; chainx != NULL ; 
         chainx = chainx->NEXT)
      {
       name = namealloc((char *)chainx->DATA);
       if(name == noname)
         continue ;
       if(name == cone->NAME)
         flag = 'Y' ;
       nbsepar = 0 ;
       for(ptchar = name ; *ptchar != '\0' ; ptchar++)
          if (*ptchar == SEPAR)
              nbsepar++;
       if(nbsepar < nbseparmin)
        {
         nbseparmin = nbsepar ;
         namesav = name ;
         loconsav = locon ;
        }
       else if(nbsepar == nbseparmin)
        {
         if(strlen(name) < strlen(namesav) || mbk_casestrcmp(name,namesav)<0)
          {
           namesav = name ;
           loconsav = locon ;
          }
        }
      }
    }
  }

 if(namesav == NULL)
  {
   if((cone->NAME != noname) || (flag == 'Y'))
     namesav = cone->NAME ;
   else
    {
     if((ptype = getptype(cone->USER,TAS_RC_INDEX)) == NULL)
       ptype = cone->USER = addptype(cone->USER,TAS_RC_INDEX,(void *)1) ;
     index = (long)ptype->DATA ;
     ptype->DATA = (void *)(index + 1) ;
     sprintf(buf,"%s_tas_%ld",cone->NAME,index) ;
     if(getptype(cone->USER,TAS_CONE_BASENAME) == NULL)
      cone->USER = addptype(cone->USER,TAS_CONE_BASENAME,(void *)cone->NAME) ;
     namesav = namealloc(buf) ;
    }
  }

 if(loconsav != NULL)
   addloconrcxname(loconsav,namesav) ;

 cone->USER = addptype(cone->USER,TAS_CONE_NAME,(void *)namesav) ;

 return(namesav) ;
}

/*****************************************************************************/
/*                        function tas_getinconename()                       */
/* donne le noms d'une entree                                                */
/*****************************************************************************/
char *tas_getinconename(incone,chainname)
edge_list *incone ;
chain_list *chainname ;
{
 ptype_list *ptype ;
 chain_list *chain ;
 chain_list *chainx ;
 chain_list *chainxx ;
 locon_list *locon ;
 locon_list *loconsav = NULL ;
 cone_list *cone ;
 char *name ;
 char *noname ;
 char *namesav = NULL ;
 char *ptchar ;
 int nbsepar ;
 int nbseparmin = 1000 ;
 char buf[1024] ;
 long index ;

 if((ptype = getptype(incone->USER,TAS_INCONE_NAME)) != NULL)
   return((char *)ptype->DATA) ;

 if((incone->TYPE & CNS_EXT) == CNS_EXT)
    {
     if((ptype = getptype(incone->UEDGE.LOCON->USER,CNS_EXT)) != NULL)
      {
       cone = (cone_list *)ptype->DATA ;
       noname = tas_getconename(cone) ;
      }
     else
      {
       cone = NULL ;
       noname = incone->UEDGE.LOCON->NAME ;
      }
    }
   else
    {
     cone = incone->UEDGE.CONE ;
     noname = tas_getconename(cone) ;
    }

 ptype = getptype(incone->USER,TAS_RC_LOCON) ;

 if(ptype == NULL)
  return(NULL) ;

 for(chain = (chain_list *)ptype->DATA ; chain != NULL ; chain = chain->NEXT)
  {
   locon = (locon_list *)chain->DATA ;
   if(loconsav == NULL)
     loconsav = locon ;
   if((ptype = getptype(locon->USER,PNODENAME)) != NULL)
    {
     for(chainx = (chain_list *)ptype->DATA ; chainx != NULL ; 
         chainx = chainx->NEXT)
      {
       name = namealloc((char *)chainx->DATA);
       if(cone != NULL)
       if(name == noname)
         continue ;
       for(chainxx = chainname ; chainxx != NULL ; chainxx = chainxx->NEXT)
          if(chainxx->DATA == name)
            break ;
       if(chainxx != NULL)
        continue ;
       nbsepar = 0 ;
       for(ptchar = name ; *ptchar != '\0' ; ptchar++)
          if (*ptchar == SEPAR)
              nbsepar++;
       if(nbsepar < nbseparmin)
        {
         nbseparmin = nbsepar ;
         namesav = name ;
         loconsav = locon ;
        }
       else if(nbsepar == nbseparmin)
        {
         if(strlen(name) < strlen(namesav))
          {
           namesav = name ;
           loconsav = locon ;
          }
        }
      }
    }
  }

 if(namesav == NULL)
  {
   if(cone != NULL)
    {
     if((ptype = getptype(cone->USER,TAS_RC_INDEX)) == NULL)
       ptype = cone->USER = 
               addptype(cone->USER,TAS_RC_INDEX,(void *)1) ;
     index = (long)ptype->DATA ;
     ptype->DATA = (void *)(index + 1) ;
     if((ptype = getptype(cone->USER,TAS_CONE_BASENAME)) != NULL)
       sprintf(buf,"%s_tas_%ld",(char *)ptype->DATA,index) ;
     else if((ptype = getptype(cone->USER,TAS_CONE_NAME)) != NULL)
       sprintf(buf,"%s_tas_%ld",(char *)ptype->DATA,index) ;
     else
       sprintf(buf,"%s_tas_%ld",cone->NAME,index) ;
    }
   else
    {
     if((ptype = getptype(incone->UEDGE.LOCON->USER,TAS_RC_INDEX)) == NULL)
       ptype = incone->UEDGE.LOCON->USER =
               addptype(incone->UEDGE.LOCON->USER,TAS_RC_INDEX,(void *)1) ;
     index = (long)ptype->DATA ;
     ptype->DATA = (void *)(index + 1) ;
     sprintf(buf,"%s_tas_%ld",incone->UEDGE.LOCON->NAME,index) ;
    }
   namesav = namealloc(buf) ;
  }

 if(loconsav != NULL)
   addloconrcxname(loconsav,namesav) ;

 incone->USER = addptype(incone->USER,TAS_INCONE_NAME,(void *)namesav) ;
 return(namesav) ;
}

/*****************************************************************************/
/*                        function tas_getsigname()                          */
/* donne le noms d'un signal                                                 */
/* C : signal de cone                                                        */
/* X : signal de connecteur                                                  */
/* I : signal de incone                                                      */
/* type : type de noeud X connecteur C cone I incone                         */
/*****************************************************************************/
char *tas_getsigname(cone,locon,incone)
cone_list *cone ;
locon_list *locon ;
edge_list *incone ;
{
 char *name ;
 chain_list *chain ;
 ptype_list *ptype ;

 if(TAS_HTAB_NAME == NULL)
   TAS_HTAB_NAME = addht(100000) ;

 if(incone != NULL)
  {
   if((ptype = getptype(incone->USER,TAS_INCONE_NAME)) != NULL)
      return((char *)ptype->DATA) ;
   chain = NULL ;
   name = tas_getinconename(incone,chain) ;
   while(sethtitem(TAS_HTAB_NAME, name, (long)0) != 0)
     {
      if(getptype(incone->USER,TAS_INCONE_NAME) != NULL)
          incone->USER = delptype(incone->USER,TAS_INCONE_NAME) ;
      chain = addchain(chain,(void *)name) ;
      name = tas_getinconename(incone,chain) ;
     }
   if(chain != NULL)
     freechain(chain) ;
   return(name) ;
  }
 else if(locon != NULL)
  {
   name = locon->NAME ;
   if(sethtitem(TAS_HTAB_NAME, name, (long)0) == 0)
    return(name) ;
   else
    {
     tas_error(65,name,TAS_WARNING) ;
     return(name) ;
    }
  }
 else if(cone != NULL)
  {
   if((ptype = getptype(cone->USER,TAS_CONE_NAME)) != NULL)
      return((char *)ptype->DATA) ;
   name = tas_getconename(cone) ;
   if(sethtitem(TAS_HTAB_NAME, name, (long)0) == 0)
    return(name) ;
   else
    {
     tas_error(65,name,TAS_WARNING) ;
     return(name) ;
    }
  }

 return(NULL) ;
}

char *tas_locon_netname(locon_list *locon)
{
  char *name;
  ptype_list *ptype;
  cone_list *cone;
  name = locon->NAME ;
  if (locon->TYPE==EXTERNAL && (ptype=getptype(locon->USER, CNS_EXT))!=NULL)
  {
    cone=(cone_list *)ptype->DATA;
    if (cone->NAME!=name) name=cone->NAME;
  }
  return name;
}
/*****************************************************************************/
/*                        function tas_getcmd()                              */
/* renvoie la command d'un latch                                             */
/*****************************************************************************/
ttvevent_list *tas_getcmd(cone,conecmd,link)
cone_list *cone ;
cone_list *conecmd ;
link_list *link ;
{
 edge_list *incone ;
 ttvevent_list *cmd ;
 ptype_list *ptype ;
 char invert = 'N' ;

 if(((cone->TYPE & CNS_FLIP_FLOP) == CNS_FLIP_FLOP) &&
    ((cone->TYPE & CNS_SLAVE) != CNS_SLAVE))
      invert = 'Y' ;

 if((TAS_CONTEXT->TAS_MERGERCN == 'N') && (TAS_CONTEXT->TAS_CALCRCX == 'Y'))
  for(incone = cone->INCONE ; incone != NULL ; incone = incone->NEXT)
   {
    if((incone->TYPE & CNS_COMMAND) == CNS_COMMAND)
      {
       if(incone->UEDGE.CONE == conecmd)
        {
         if((ptype = getptype(incone->USER,TAS_SIG_INCONE)) != NULL)
           {
            if((link->ULINK.LOTRS->TYPE & CNS_TN) == CNS_TN)
             {
              if(invert == 'N')
                cmd = ((ttvsig_list *)ptype->DATA)->NODE+1 ;
              else
                cmd = ((ttvsig_list *)ptype->DATA)->NODE ;
             }
            else
             {
              if(invert == 'N')
                cmd = ((ttvsig_list *)ptype->DATA)->NODE ;
              else
                cmd = ((ttvsig_list *)ptype->DATA)->NODE+1 ;
             }
            return(cmd) ;
           }
        }
      }
   }

 if((link->ULINK.LOTRS->TYPE & CNS_TN) == CNS_TN)
  {
   if(invert == 'N')
   cmd = ((ttvsig_list *)(getptype(conecmd->USER,TAS_SIG_CONE))->DATA)->NODE+1 ;
   else
   cmd = ((ttvsig_list *)(getptype(conecmd->USER,TAS_SIG_CONE))->DATA)->NODE ;
  }
 else
  {
   if(invert == 'N')
   cmd = ((ttvsig_list *)(getptype(conecmd->USER,TAS_SIG_CONE))->DATA)->NODE ;
   else
   cmd = ((ttvsig_list *)(getptype(conecmd->USER,TAS_SIG_CONE))->DATA)->NODE+1 ;
  }
 return(cmd) ;
}

/*****************************************************************************/
/*                        function tas_detectsig()                           */
/* recherche les signaux d'interconnexions                                   */
/*****************************************************************************/
void tas_detectsig(cnsfig)
cnsfig_list *cnsfig ;
{
 cone_list *cone ;
 edge_list *incone ;
 locon_list *locon ;
 ptype_list *ptype ;
 ptype_list *ptypesig ;
 chain_list *chain ;

 for(cone = cnsfig->CONE ; cone != NULL ; cone = cone->NEXT)
  {
   if((cone->TYPE & (CNS_VDD | CNS_VSS)) != 0) continue ;
   for(incone = cone->INCONE ; incone != NULL ; incone = incone->NEXT)
    {
     if((incone->TYPE & (CNS_FEEDBACK | CNS_VDD | CNS_VSS | CNS_BLEEDER | 
                         TAS_PATH_INOUT)) != 0)
      {
       if((incone->TYPE & (CNS_COMMAND | CNS_VDD | CNS_VSS | CNS_BLEEDER | 
                           TAS_PATH_INOUT)) != CNS_COMMAND)
       continue ;
      }
     if((ptype = getptype(incone->USER,TAS_RC_LOCON)) != NULL)
      {
       ptypesig = addptype(NULL,(long)0,NULL) ;
       for(chain = (chain_list *)ptype->DATA ; chain != NULL ; 
           chain = chain->NEXT)
        {
         locon = (locon_list *)chain->DATA ;
         if(getptype(locon->USER,TAS_CON_INCONE) != NULL)
           {
            break ;
           }
         else
          {
           locon->USER = addptype(locon->USER,TAS_CON_INCONE,(void *)ptypesig) ;
          }
        }
       if(chain != NULL) 
        {
         freeptype(ptypesig) ;
         ptypesig = getptype(((locon_list *)chain->DATA)->USER,TAS_CON_INCONE) ;
         ptypesig = (ptype_list *)ptypesig->DATA ;
         ptypesig->DATA = NULL ;
         for(chain = (chain_list *)ptype->DATA ; chain != NULL ;
           chain = chain->NEXT)
          {
           locon = (locon_list *)chain->DATA ;
           if((ptype = getptype(locon->USER,TAS_CON_INCONE)) != NULL)
             {
              ptype->DATA = (void *)ptypesig ;
             }
           else
            {
             locon->USER = addptype(locon->USER,TAS_CON_INCONE,
                                    (void *)ptypesig) ;
            }
          }
        }
     
       if((incone->TYPE & CNS_COMMAND) == CNS_COMMAND)
        {
         ptypesig->TYPE |= TTV_SIG_Q ;
         incone->UEDGE.CONE->TYPE &= ~(TAS_LCOMMAND) ;
         if((incone->TYPE & CNS_EXT) == CNS_EXT)
          {
           if(incone->UEDGE.LOCON->TYPE == EXTERNAL)
             ptypesig->TYPE |= TTV_SIG_N ;
          }
         else if((incone->UEDGE.CONE->TYPE & CNS_EXT) == CNS_EXT)
          {
           //if(((locon_list *)getptype(incone->UEDGE.CONE->USER,CNS_EXT)->DATA)->TYPE == EXTERNAL)
           if(cns_get_one_cone_external_connector(incone->UEDGE.CONE)->TYPE == EXTERNAL)
             ptypesig->TYPE |= TTV_SIG_N ;
          }
        }
       else
        {
         if((incone->TYPE & CNS_EXT) == CNS_EXT) 
          {
           if(incone->UEDGE.LOCON->TYPE == EXTERNAL)
             ptypesig->TYPE |= TTV_SIG_N ;
          }
         else if((incone->UEDGE.CONE->TYPE & CNS_EXT) == CNS_EXT)
          {
           //if(((locon_list *)getptype(incone->UEDGE.CONE->USER, CNS_EXT)->DATA)->TYPE == EXTERNAL)
           if(cns_get_one_cone_external_connector(incone->UEDGE.CONE)->TYPE == EXTERNAL)
             ptypesig->TYPE |= TTV_SIG_N ;
          }
         else if((cone->TYPE & CNS_EXT) == CNS_EXT)
          {
           //if(((locon_list *)getptype(cone->USER,CNS_EXT)->DATA)->TYPE == EXTERNAL)
           if(cns_get_one_cone_external_connector(cone)->TYPE == EXTERNAL)
             ptypesig->TYPE |= TTV_SIG_EXT ;
          }
        }
      }
    } 
  }
}

/*****************************************************************************/
/*                        function tas_printloop()                           */
/* generation des fichiers .loop.                                            */
/*****************************************************************************/
void tas_printloop(nb_loop,list_loop,ttvfig)
int nb_loop ;
chain_list *list_loop ;
ttvfig_list *ttvfig ;
{
 if(nb_loop != 0)
    {
     FILE *file ;
     char f[64] ;
     char buffer[1024] ;
     char bufsigname[1024] ;
     char bufnetname[1024] ;
     int i = 1 ;

     strcpy(f,ttvfig->INFO->FIGNAME) ; 
     strcat(f,".loop") ;

     if((file = fopen(f,"w")) == NULL) /*  erreur en ouverture du fichier */
     tas_error(8,f,TAS_WARNING) ;

     sprintf(buffer,"List of circuit loops : %s.loop\n",ttvfig->INFO->FIGNAME);
     avt_printExecInfo(file, "#", buffer, "");
     if(nb_loop == 1) fprintf(file,"one loop was detected :\n") ;
     else fprintf(file,"%d loops were detected :\n",nb_loop) ;
     fprintf(file,"\n") ;

     while(list_loop != NULL)
        {
         chain_list  *aux_l ;
         list_loop->DATA = reverse(list_loop->DATA);

         fprintf(file,"loop no %d :\n",i) ;
         i++ ;

        for(aux_l=(chain_list *)list_loop->DATA;aux_l;aux_l=aux_l->NEXT)
            {
             ttv_getsigname(ttvfig,bufsigname,((ttvevent_list *)aux_l->DATA)->ROOT) ;
             ttv_getnetname(ttvfig,bufnetname,((ttvevent_list *)aux_l->DATA)->ROOT) ;
             if((((ttvevent_list *)aux_l->DATA)->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
                 fprintf(file,"%s (%s) Rise\n",bufsigname,bufnetname) ;
             else
                 fprintf(file,"%s (%s) Fall\n",bufsigname,bufnetname) ;
            }

         fprintf(file,"\n") ;
         list_loop = list_loop->NEXT ;
        }

     if(fclose(file) != 0) tas_error(10,f,TAS_WARNING) ;
    }
}

/*****************************************************************************/
/*                        function tas_loconpathmarque()                     */
/* marque tout les chemins qui vont a un connecteur                          */
/*****************************************************************************/
void tas_loconpathmarque(cone,type)
cone_list *cone ;
long type ;
{
 edge_list *edge ;

 if((cone->TYPE & (CNS_VSS|CNS_VDD)) != 0)
    return ;

 if((cone->TYPE & (CNS_LATCH|CNS_FLIP_FLOP|TAS_PRECHARGE)) != 0)
   {
    ((ttvsig_list *)(getptype(cone->USER,
                             TAS_SIG_CONE))->DATA)->TYPE |= TTV_SIG_EXT ;
    if((cone->TYPE & CNS_EXT) != CNS_EXT)
      return ;
   }
 else if((cone->TYPE & (TAS_INTER | TAS_LCOMMAND | TAS_BREAK)) != 0)
   {
    ((ttvsig_list *)(getptype(cone->USER,
                             TAS_SIG_CONE))->DATA)->TYPE |= TTV_SIG_EXT ;
   }
 
 if(type == TAS_EXT_IN)
   edge = cone->INCONE ;
 else
   edge = cone->OUTCONE ;
  
 while(edge != NULL)
  {
   if(((edge->TYPE & type) == type) ||
      ((edge->TYPE & (CNS_EXT | CNS_FEEDBACK | CNS_VDD | CNS_VSS | 
                      CNS_COMMAND | TAS_PATH_INOUT | CNS_BLEEDER)) != 0))
     {
      edge = edge->NEXT ;
      continue ;
     }

   edge->TYPE |= type ;

   if(type == TAS_EXT_OUT)
    {
     edge_list *edgex ;
   
     for(edgex = edge->UEDGE.CONE->INCONE ; edgex != NULL ; edgex = edgex->NEXT)
      if((edgex->TYPE & CNS_EXT) != CNS_EXT)
       if(cone == edgex->UEDGE.CONE)
        {
         edgex->TYPE |= TAS_EXT_OUT ;
         break ;
        }

     if(edgex != NULL)
     {
     if(((edge->UEDGE.CONE->TYPE & (CNS_LATCH|CNS_FLIP_FLOP)) != 0) && 
        ((cone->TYPE & TAS_LCOMMAND) == TAS_LCOMMAND) &&
        ((edgex->TYPE & CNS_COMMAND) == CNS_COMMAND))
       {
        edge = edge->NEXT ;
        continue ;
       }
     else if(((edge->UEDGE.CONE->TYPE & (CNS_LATCH|CNS_FLIP_FLOP)) != 0) &&
        ((cone->TYPE & TAS_LCOMMAND) != TAS_LCOMMAND) &&
        ((edgex->TYPE & CNS_COMMAND) == CNS_COMMAND))
       {
        if(getptype(edgex->USER,TAS_SIG_INCONE) != NULL)
                ((ttvsig_list *)(getptype(edgex->USER,
                                 TAS_SIG_INCONE))->DATA)->TYPE |= TTV_SIG_EXT ;
        edge = edge->NEXT ;
        continue ;
       }
     }
    }
   tas_loconpathmarque(edge->UEDGE.CONE,type) ;
   edge = edge->NEXT ;
  }
}

/*****************************************************************************/
/*                        function tas_latchdelayloop()                      */
/* ajoute les contraintes a un latch                                         */
/*****************************************************************************/
void tas_latchdelayloop(cone,tupmax,tdnmax,tupmin,tdnmin)
cone_list *cone ;
long *tupmax ;
long *tdnmax ;
long *tupmin ;
long *tdnmin ;
{
 edge_list *incone ;
 edge_list *inconeloop ;
 cone_list *coneloop ;
 delay_list *delaymax = NULL ;
 delay_list *delaymin = NULL ;
 ptype_list *ptypemax ;
 ptype_list *ptypemin ;
 long rc ;
 long gate ;

 *tupmax = (long)0 ;
 *tdnmax = (long)0 ;
 *tupmin = (long)0 ;
 *tdnmin = (long)0 ;

 for(incone = cone->INCONE ; incone != NULL ; incone = incone->NEXT){
     if(((incone->TYPE & CNS_LOOP) == CNS_LOOP) && 
       (((incone->TYPE & CNS_FEEDBACK) == CNS_FEEDBACK) || ((incone->TYPE & CNS_MEMSYM) == CNS_MEMSYM))){
        coneloop = incone->UEDGE.CONE;
        for(inconeloop = coneloop->INCONE ; inconeloop != NULL ; inconeloop = inconeloop->NEXT){
           if(inconeloop->UEDGE.CONE == cone)
              break ;
        }
      if(inconeloop != NULL)
       {
        
        ptypemax = (ptype_list *)getptype(inconeloop->USER,TAS_DELAY_MAX) ;
        ptypemin = (ptype_list *)getptype(inconeloop->USER,TAS_DELAY_MIN) ;
        if(ptypemax != NULL)
          {
           delaymax = (delay_list *)(ptypemax->DATA) ;

           if(delaymax->RCHH != TAS_NOTIME)
              rc = delaymax->RCHH ;
           else
              rc = (long)0 ;

           if(delaymax->TPHL != TAS_NOTIME)
             {
              gate =  delaymax->TPHL ;
              if((*tupmax == 0) || ((rc + gate) > *tupmax))
                  *tupmax = rc + gate ;
             }

           if(delaymax->TPHH != TAS_NOTIME)
             {
              gate =  delaymax->TPHH ;
              if((*tupmax == 0) || ((rc + gate) > *tupmax))
                  *tupmax = rc + gate ;
             }

           if(delaymax->RCLL != TAS_NOTIME)
              rc = delaymax->RCLL ;
           else
              rc = (long)0 ;

           if(delaymax->TPLH != TAS_NOTIME)
             {
              gate =  delaymax->TPLH ;
              if((*tdnmax == 0) || ((rc + gate) > *tdnmax))
                  *tdnmax = rc + gate ;
             }

           if(delaymax->TPLL != TAS_NOTIME)
             {
              gate =  delaymax->TPLL ;
              if((*tdnmax == 0) || ((rc + gate) > *tdnmax))
                  *tdnmax = rc + gate ;
             }
          }
        if(ptypemin != NULL)
          {
           delaymin = (delay_list *)(ptypemin->DATA) ;

           if(delaymin->RCHH != TAS_NOTIME)
              rc = delaymin->RCHH ;
           else
              rc = (long)0 ;

           if(delaymin->TPHL != TAS_NOTIME)
             {
              gate =  delaymin->TPHL ;
              if((*tupmin == 0) || ((rc + gate) < *tupmin))
                  *tupmin = rc + gate ;
             }

           if(delaymin->TPHH != TAS_NOTIME)
             {
              gate =  delaymin->TPHH ;
              if((*tupmin == 0) || ((rc + gate) < *tupmin))
                  *tupmin = rc + gate ;
             }

           if(delaymin->RCLL != TAS_NOTIME)
              rc = delaymin->RCLL ;
           else
              rc = (long)0 ;

           if(delaymin->TPLH != TAS_NOTIME)
             {
              gate =  delaymin->TPLH ;
              if((*tdnmin == 0) || ((rc + gate) < *tdnmin))
                  *tdnmin = rc + gate ;
             }

           if(delaymin->TPLL != TAS_NOTIME)
             {
              gate =  delaymin->TPLL ;
              if((*tdnmin == 0) || ((rc + gate) < *tdnmin))
                  *tdnmin = rc + gate ;
             }
          }
       }
     }
   }
}

/*****************************************************************************/
/*                        function tas_getlatchcmdlist()                     */
/* ajoute les contraintes a un latch                                         */
/*****************************************************************************/
void tas_addcaraclatch(cone,conecmd,cmd,tupmax,tdnmax,tupmin,tdnmin)
cone_list *cone ;
cone_list *conecmd ;
ttvevent_list *cmd ;
long tupmax ;
long tdnmax ;
long tupmin ;
long tdnmin ;
{
 ttvevent_list *cmdcont ;
 branch_list *path[3]  ;
 ptype_list *ptype = getptype(cone->USER,TAS_SIG_CONE), *ptype2 ;
 ttvsig_list *ptsig ;
 locon_list *locon;
 long type ;
 int i, j ;
 long cmdtype ;
 long slopemax ;
 long slopemin ;
 float fshrink, slopemaxshrink, slopeminshrink;
 front_list *front ;
 char savepathtype ;
 double seuil ;
 double vt ;
 double kt ;
 double vdd ;
 double vslope ;
 double tsupmax, tsupmax2 ;
 double tsdnmax, tsdnmax2 ;
 double tsupmin, tsupmin2 ;
 double tsdnmin, tsdnmin2 ;
 timing_model *tmodeld1 = NULL;
 timing_model *tmodeld2 = NULL;
 timing_model *tmodeld3 = NULL;
 timing_model *tmodeld4 = NULL;
 ttvline_list *ptline;
 int nfcarac;
 double *fcarac;
 char sens;
 char *latching_voltage = V_STR_TAB[__TAS_SETUP_LATCHING_VOLTAGE].VALUE;

 if(ptype == NULL)
    return ;

 path[0] = cone->BREXT;
 path[1] = cone->BRVDD;
 path[2] = cone->BRVSS;

 cmdtype = (long)0 ;

 if((cone->TYPE & CNS_LATCH) == CNS_LATCH)
    cmdcont = (cmd->TYPE & TTV_NODE_UP)? cmd->ROOT->NODE : cmd->ROOT->NODE +1 ;
 else
    cmdcont = cmd ;

 savepathtype = TAS_PATH_TYPE ; 

 TAS_PATH_TYPE = 'M' ;
 front = tas_getslope(cone,conecmd) ;
 slopemax = ((cmdcont->TYPE & TTV_NODE_UP) == TTV_NODE_UP)? front->FUP : front->FDOWN ;
 if(slopemax == TAS_NOFRONT){
     TAS_PATH_TYPE = savepathtype; 
     return;
 }

 TAS_PATH_TYPE = 'm' ;
 front = tas_getslope(cone,conecmd) ;
 slopemin = ((cmdcont->TYPE & TTV_NODE_UP) == TTV_NODE_UP)? front->FUP : front->FDOWN ;
 if (slopemin == TAS_NOFRONT) slopemin = slopemax;

 TAS_PATH_TYPE = savepathtype  ; 

 sens  = ((cmdcont->TYPE & TTV_NODE_UP) == TTV_NODE_UP)? 'U' : 'D' ;
 for(i = 0 ; i < 3 ; i++)
 for(; path[i] != NULL ; path[i] = path[i]->NEXT)
 if((path[i]->TYPE & CNS_NOT_FUNCTIONAL) != CNS_NOT_FUNCTIONAL)
    {
     link_list *link ;

     if ((i == 1) && tas_is_degraded_memsym(path[i], cone, 'U')) continue;
     if ((i == 2) && tas_is_degraded_memsym(path[i], cone, 'D')) continue;
     for(link = (link_list *)path[i]->LINK ; link != NULL ; link = link->NEXT)
        if((link->TYPE & CNS_COMMAND) == CNS_COMMAND)
           {
            cone_list *conecmdx = (cone_list *)getptype(link->ULINK.LOTRS->USER,
                                                       CNS_DRIVINGCONE)->DATA ;
            if(conecmdx == conecmd)
             {
              vt = tas_getparam(link->ULINK.LOTRS,TAS_CASE,TP_VT) ;
              kt = tas_getparam(link->ULINK.LOTRS,TAS_CASE,TP_KT) ;
              vdd = tas_getparam(link->ULINK.LOTRS,TAS_CASE,TP_VDDmax) ;
              seuil = tas_getparam(link->ULINK.LOTRS,TAS_CASE,TP_SEUIL) ;
              vslope = seuil + vt + kt * seuil ;
              if(vslope > 0.98*vdd) vslope = 0.98*vdd ;
              else
               {
                vslope = (vslope+vdd)/2.0;
               }
              if(!V_BOOL_TAB[ __AVT_OLD_FEATURE_27 ].VALUE){
                  slopemaxshrink = stm_thr2scm(slopemax/TTV_UNIT,STM_DEFAULT_SMINF,STM_DEFAULT_SMAXF,vt,0.0,vdd,STM_DN);
                  slopeminshrink = stm_thr2scm(slopemin/TTV_UNIT,STM_DEFAULT_SMINF,STM_DEFAULT_SMAXF,vt,0.0,vdd,STM_DN);
              }else{
                  slopemaxshrink = slopemax/TTV_UNIT;
                  slopeminshrink = slopemin/TTV_UNIT;
              }

              if(MLO_IS_TRANSN(link->ULINK.LOTRS->TYPE))
               {
                tsupmax = (stm_get_t(seuil,vt,vdd,0.0,slopemaxshrink) - stm_get_t(vslope,vt,vdd,0.0,slopemaxshrink)) * TTV_UNIT ;
                tsdnmax = (stm_get_t(seuil,vt,vdd,0.0,slopemaxshrink) - stm_get_t(vt,vt,vdd,0.0,slopemaxshrink)) * TTV_UNIT ;
                tsupmin = (stm_get_t(seuil,vt,vdd,0.0,slopeminshrink) - stm_get_t(vslope,vt,vdd,0.0,slopeminshrink)) * TTV_UNIT ;
                if(!strcmp(latching_voltage, "vdd"))
                    tsdnmin = stm_get_t(seuil,vt,vdd,0.0,slopeminshrink);
                else if(!strcmp(latching_voltage, "vdd-div-2"))
                    tsdnmin = 0.0;
                else if(!strcmp(latching_voltage, "max-vt-vdd-div-2")){
                    if(vt > seuil)
                        tsdnmin = stm_get_t(seuil,vt,vdd,0.0,slopeminshrink);
                    else
                        tsdnmin = (stm_get_t(seuil,vt,vdd,0.0,slopeminshrink) - stm_get_t(vt,vt,vdd,0.0,slopeminshrink)) * TTV_UNIT ;
                }else
                    tsdnmin = (stm_get_t(seuil,vt,vdd,0.0,slopeminshrink) - stm_get_t(vt,vt,vdd,0.0,slopeminshrink)) * TTV_UNIT ;
               }
              else
               {
                tsdnmax = (stm_get_t(seuil,vt,vdd,0.0,slopemaxshrink) - stm_get_t(vslope,vt,vdd,0.0,slopemaxshrink)) * TTV_UNIT ;
                tsupmax = (stm_get_t(seuil,vt,vdd,0.0,slopemaxshrink) - stm_get_t(vt,vt,vdd,0.0,slopemaxshrink)) * TTV_UNIT ;
                tsdnmin = (stm_get_t(seuil,vt,vdd,0.0,slopeminshrink) - stm_get_t(vslope,vt,vdd,0.0,slopeminshrink)) * TTV_UNIT ;
                if(!strcmp(latching_voltage, "vdd"))
                    tsupmin = stm_get_t(seuil,vt,vdd,0.0,slopeminshrink);
                else if(!strcmp(latching_voltage, "vdd-div-2"))
                    tsupmin = 0.0;
                else if(!strcmp(latching_voltage, "max-vt-vdd-div-2")){
                    if(vt > seuil)
                        tsupmin = stm_get_t(seuil,vt,vdd,0.0,slopeminshrink);
                    else
                        tsupmin = (stm_get_t(seuil,vt,vdd,0.0,slopeminshrink) - stm_get_t(vt,vt,vdd,0.0,slopeminshrink)) * TTV_UNIT ;
                }else
                    tsupmin = (stm_get_t(seuil,vt,vdd,0.0,slopeminshrink) - stm_get_t(vt,vt,vdd,0.0,slopeminshrink)) * TTV_UNIT ;
               }
              if(i == 0)
                cmdtype |= (CNS_VDD|CNS_VSS) ;
              else if(i == 1)
                cmdtype |= CNS_VDD ;
              else if(i == 2)
                cmdtype |= CNS_VSS ;
/*              if((link->TYPE & CNS_SWITCH) == CNS_SWITCH)
                cmdtype |= (CNS_VDD|CNS_VSS) ;*/

              if((conecmd->TYPE & CNS_EXT)==CNS_EXT && tmodeld1==NULL) {
                chain_list *cl;
                cl=cns_get_cone_external_connectors(conecmd);
                while (cl!=NULL)
                {
                  //ptype2 = getptype (conecmd->USER, CNS_EXT) ;
                  locon = (locon_list*)cl->DATA; //ptype2->DATA ;
                  ptype2 = getptype (locon->USER, TAS_CON_CARAC) ;
                  if (locon->DIRECTION == CNS_I) {
                    fcarac = ((caraccon_list*)ptype2->DATA)->SLOPEIN ;
                    nfcarac = ((caraccon_list*)ptype2->DATA)->NSLOPE ;
                    if(nfcarac){
/*                        // ----- evite des memory leaks MAIS code valide?
                        if (tmodeld1!=NULL) stm_mod_destroy(tmodeld1);
                        if (tmodeld2!=NULL) stm_mod_destroy(tmodeld2);
                        if (tmodeld3!=NULL) stm_mod_destroy(tmodeld3);
                        if (tmodeld4!=NULL) stm_mod_destroy(tmodeld4);*/
                        // -----
                        tmodeld1 = stm_mod_create_table (stm_mod_name(cone->NAME, sens, conecmd->NAME, 'U', STM_DELAY|STM_SETUP, 'M', 0), nfcarac, 0, STM_CLOCK_SLEW, STM_NOTYPE);
                        tmodeld2 = stm_mod_create_table (stm_mod_name(cone->NAME, sens, conecmd->NAME, 'D', STM_DELAY|STM_SETUP, 'M', 0), nfcarac, 0, STM_CLOCK_SLEW, STM_NOTYPE);
                        tmodeld3 = stm_mod_create_table (stm_mod_name(cone->NAME, sens, conecmd->NAME, 'U', STM_DELAY|STM_HOLD, 'M', 0), nfcarac, 0, STM_CLOCK_SLEW, STM_NOTYPE);
                        tmodeld4 = stm_mod_create_table (stm_mod_name(cone->NAME, sens, conecmd->NAME, 'D', STM_DELAY|STM_HOLD, 'M', 0), nfcarac, 0, STM_CLOCK_SLEW, STM_NOTYPE);
                    }
                    for(j = 0; j < nfcarac; j++){
                        if(!V_BOOL_TAB[ __AVT_OLD_FEATURE_27 ].VALUE){
                            fshrink = stm_thr2scm(fcarac[j],STM_DEFAULT_SMINF,STM_DEFAULT_SMAXF,vt,0.0,vdd,STM_DN);
                        }else{
                            fshrink = fcarac[j];
                        }
                        if(MLO_IS_TRANSN(link->ULINK.LOTRS->TYPE))
                         {
                          tsupmax2 = (stm_get_t(seuil,vt,vdd,0.0,fshrink) - stm_get_t(vslope,vt,vdd,0.0,fshrink)) ;
                          tsdnmax2 = (stm_get_t(seuil,vt,vdd,0.0,fshrink) - stm_get_t(vt,vt,vdd,0.0,fshrink)) ;
                          tsupmin2 = tsupmax2 ;
                          if(!strcmp(latching_voltage, "vdd"))
                              tsdnmin2 = stm_get_t(seuil,vt,vdd,0.0,fshrink);
                          else if(!strcmp(latching_voltage, "vdd-div-2"))
                              tsdnmin2 = 0.0;
                          else if(!strcmp(latching_voltage, "max-vt-vdd-div-2")){
                              if(vt > seuil)
                                  tsdnmin2 = stm_get_t(seuil,vt,vdd,0.0,fshrink);
                              else
                                  tsdnmin2 = tsdnmax2 ;
                          }else
                              tsdnmin2 = tsdnmax2 ;
                         }
                        else
                         {
                          tsdnmax2 = (stm_get_t(seuil,vt,vdd,0.0,fshrink) - stm_get_t(vslope,vt,vdd,0.0,fshrink)) ;
                          tsupmax2 = (stm_get_t(seuil,vt,vdd,0.0,fshrink) - stm_get_t(vt,vt,vdd,0.0,fshrink)) ;
                          tsdnmin2 = tsdnmax2 ;
                          if(!strcmp(latching_voltage, "vdd"))
                              tsupmin2 = stm_get_t(seuil,vt,vdd,0.0,fshrink);
                          else if(!strcmp(latching_voltage, "vdd-div-2"))
                              tsupmin2 = 0.0;
                          else if(!strcmp(latching_voltage, "max-vt-vdd-div-2")){
                              if(vt > seuil)
                                  tsupmin2 = stm_get_t(seuil,vt,vdd,0.0,fshrink);
                              else
                                  tsupmin2 = tsupmax2 ;
                          }else
                              tsupmin2 = tsupmax2 ;
                         }
                        stm_modtbl_setXrangeval (tmodeld1->UMODEL.TABLE, j, fcarac[j]);
                        stm_modtbl_set1Dval (tmodeld1->UMODEL.TABLE, j, tupmax/TTV_UNIT+tsupmin2);
                        stm_modtbl_setXrangeval (tmodeld2->UMODEL.TABLE, j, fcarac[j]);
                        stm_modtbl_set1Dval (tmodeld2->UMODEL.TABLE, j, tdnmax/TTV_UNIT+tsdnmin2);
                        stm_modtbl_setXrangeval (tmodeld3->UMODEL.TABLE, j, fcarac[j]);
                        stm_modtbl_set1Dval (tmodeld3->UMODEL.TABLE, j, -tsupmax2);
                        stm_modtbl_setXrangeval (tmodeld4->UMODEL.TABLE, j, fcarac[j]);
                        stm_modtbl_set1Dval (tmodeld4->UMODEL.TABLE, j, -tsdnmax2);

                    }
                  }
                  cl=delchain(cl,cl);
                }
              }
             }
           }
    }


 ptsig = (ttvsig_list *)ptype->DATA ;

 if(((ptsig->TYPE & (TTV_SIG_C|TTV_SIG_N)) == 0) &&
    ((cmdcont->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_N)) == 0))
   type = TTV_LINE_E ;
 else
  {
   type = TTV_LINE_D ;
   if((cmdcont->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0)
      ptsig->TYPE |= TTV_SIG_EXT ;
  }

 if((cmdtype & CNS_VDD) == CNS_VDD){
 ptline = ttv_addline(ptsig->ROOT,cmdcont,ptsig->NODE+1,tupmax+tsupmin,(long)0,TTV_NOTIME,TTV_NOSLOPE,
             type|TTV_LINE_U) ;
 if(tmodeld1){
     stm_storemodel(CELL, NULL, tmodeld1, 0);
     ttv_addcaracline(ptline,tmodeld1->NAME,NULL,NULL,NULL) ;
     tmodeld1=NULL; //used
 }
 }

 if((cmdtype & CNS_VSS) == CNS_VSS){
 ptline = ttv_addline(ptsig->ROOT,cmdcont,ptsig->NODE,tdnmax+tsdnmin,(long)0,TTV_NOTIME,TTV_NOSLOPE,
             type|TTV_LINE_U) ;
 if(tmodeld2){
     stm_storemodel(CELL, NULL, tmodeld2, 0);
     ttv_addcaracline(ptline,tmodeld2->NAME,NULL,NULL,NULL) ;
     tmodeld2=NULL; //used
 }
 }

 if((cmdtype & CNS_VDD) == CNS_VDD){
 ptline = ttv_addline(ptsig->ROOT,cmdcont,ptsig->NODE+1,-tsupmax,(long)0,TTV_NOTIME,TTV_NOSLOPE,
             type|TTV_LINE_O) ;
 if(tmodeld3){
     stm_storemodel(CELL, NULL, tmodeld3, 0);
     ttv_addcaracline(ptline,tmodeld3->NAME,NULL,NULL,NULL) ;
     tmodeld3=NULL; //used
 }
 }

 if((cmdtype & CNS_VSS) == CNS_VSS){
 ptline = ttv_addline(ptsig->ROOT,cmdcont,ptsig->NODE,-tsdnmax,(long)0,TTV_NOTIME,TTV_NOSLOPE,
             type|TTV_LINE_O) ;
 if(tmodeld4){
     stm_storemodel(CELL, NULL, tmodeld4, 0);
     ttv_addcaracline(ptline,tmodeld4->NAME,NULL,NULL,NULL) ;
     tmodeld4=NULL; //used
 }
 }

 if (tmodeld1!=NULL) stm_mod_destroy(tmodeld1);
 if (tmodeld2!=NULL) stm_mod_destroy(tmodeld2);
 if (tmodeld3!=NULL) stm_mod_destroy(tmodeld3);
 if (tmodeld4!=NULL) stm_mod_destroy(tmodeld4);
}

/*****************************************************************************/
/*                        function tas_getlatchcmdlist()                     */
/* renvoie la liste des commandes d'un latch                                 */
/*****************************************************************************/
chain_list *tas_getlatchcmdlist(cone)
cone_list *cone ;
{
 branch_list *path[3]  ;
 ttvevent_list *cmd ;
 chain_list *chain = NULL ;
 chain_list *chainx ;
 short i;
 long tupmax ;
 long tdnmax ;
 long tupmin ;
 long tdnmin ;

 path[0] = cone->BREXT;
 path[1] = cone->BRVDD;
 path[2] = cone->BRVSS;

 if(TAS_CONTEXT->TAS_CARAC_MEMORY == 'Y') 
   tas_latchdelayloop(cone,&tupmax,&tdnmax,&tupmin,&tdnmin) ;

 for(i = 0 ; i < 3 ; i++)
 for(; path[i] != NULL ; path[i] = path[i]->NEXT)
 if((path[i]->TYPE & CNS_NOT_FUNCTIONAL) != CNS_NOT_FUNCTIONAL)
    {
     link_list *link ;

     for(link = (link_list *)path[i]->LINK ; link != NULL ; link = link->NEXT)
        if((link->TYPE & CNS_COMMAND) == CNS_COMMAND)
           {
            cone_list *conecmd = (cone_list *)getptype(link->ULINK.LOTRS->USER,
                                                       CNS_DRIVINGCONE)->DATA ;
            cmd = tas_getcmd(cone,conecmd,link) ;
            if((cmd->TYPE & TTV_NODE_CMDMARQUE) != TTV_NODE_CMDMARQUE)
              {
               chain = addchain(chain,(void*)cmd) ;
               cmd->TYPE |= TTV_NODE_CMDMARQUE ;
               if(TAS_CONTEXT->TAS_CARAC_MEMORY == 'Y') 
                tas_addcaraclatch(cone,conecmd,cmd,tupmax,tdnmax,tupmin,tdnmin);
              }
           }
    }

 if(((cone->TYPE & (CNS_LATCH|CNS_MEMSYM)) == (CNS_LATCH|CNS_MEMSYM)) ||
    ((cone->TYPE & (CNS_FLIP_FLOP|CNS_MEMSYM)) == (CNS_FLIP_FLOP|CNS_MEMSYM)))
  {
   edge_list *incone ;
   for(incone = cone->INCONE ; incone != NULL ; incone = incone->NEXT)
    if((incone->TYPE & TAS_IN_MEMSYM) == TAS_IN_MEMSYM)
     {
      cone = (cone_list *)getptype(incone->USER,TAS_IN_CONESYM)->DATA ;
       path[0] = cone->BREXT;
       path[1] = cone->BRVDD;
       path[2] = cone->BRVSS;
 
       for(i = 0 ; i < 3 ; i++)
       for(; path[i] != NULL ; path[i] = path[i]->NEXT)
       if((path[i]->TYPE & CNS_NOT_FUNCTIONAL) != CNS_NOT_FUNCTIONAL)
          {
           link_list *link ;
 
           for(link = (link_list *)path[i]->LINK ; link != NULL ; 
               link = link->NEXT)
              if((link->TYPE & CNS_COMMAND) == CNS_COMMAND)
                 {
                  cone_list *conecmd = 
                  (cone_list *)getptype(link->ULINK.LOTRS->USER,
                                        CNS_DRIVINGCONE)->DATA ;
                  cmd = tas_getcmd(cone,conecmd,link) ;
                  if((cmd->TYPE & TTV_NODE_CMDMARQUE) != TTV_NODE_CMDMARQUE)
                    {
                     chain = addchain(chain,(void*)cmd) ;
                     cmd->TYPE |= TTV_NODE_CMDMARQUE ;
                    }
                 }
 
          }
     }
  }

 for(chainx = chain ; chainx != NULL ; chainx = chainx->NEXT)
  ((ttvevent_list *)chain->DATA)->TYPE &= ~(TTV_NODE_CMDMARQUE) ;

return(chain) ;
}

/*****************************************************************************/
/*                        function tas_getlatchcmd()                         */
/* fonction qui donne la commande du latch cone qui commande l'entree incone */
/* au niveau level. Elle retourne un noeud de la ttvfig qui correspond a un  */
/* niveau de la command. Haut si la commande ouvre le latch au niveau haut   */
/* bas sinon.                                                                */
/*****************************************************************************/
chain_list *tas_getlatchcmd(cone,incone,level)
cone_list *cone ;
edge_list *incone ;
long level ;
{
 branch_list *path[3]  ;
 ttvevent_list *cmd ;
 chain_list *chaincmd = NULL ;
 chain_list *chain ;
 short i;

 if((incone->TYPE & TAS_IN_MEMSYM) == TAS_IN_MEMSYM)
  {
   cone = (cone_list *)getptype(incone->USER,TAS_IN_CONESYM)->DATA ;
   if((level & CNS_VSS) == CNS_VSS)
    {
     level &= ~(CNS_VSS) ;
     level |= CNS_VDD ;
    }
   else if((level & CNS_VDD) == CNS_VDD)
    {
     level &= ~(CNS_VDD) ;
     level |= CNS_VSS ;
    }
  }

 path[0] = cone->BREXT;
 path[1] = cone->BRVDD;
 path[2] = cone->BRVSS;

 for(i = 0 ; i < 3 ; i++)
 for(; path[i] != NULL ; path[i] = path[i]->NEXT)
 if((path[i]->TYPE & CNS_NOT_FUNCTIONAL) != CNS_NOT_FUNCTIONAL)
 if((((level & CNS_VSS) == CNS_VSS) && ((path[i]->TYPE & CNS_NOT_DOWN) != CNS_NOT_DOWN)) ||
    (((level & CNS_VDD) == CNS_VDD) && ((path[i]->TYPE & CNS_NOT_UP) != CNS_NOT_UP)))
 if(((path[i]->TYPE & CNS_EXT) == CNS_EXT) || 
    ((path[i]->TYPE & level) == level))
    {
     link_list *link ;

     for(link = (link_list *)path[i]->LINK ; link != NULL ; link = link->NEXT)
        {
         if(((incone->TYPE & CNS_EXT) != CNS_EXT) &&
           ((link->TYPE & (CNS_IN | CNS_INOUT)) == 0))
            {
             if((cone_list *)(getptype(link->ULINK.LOTRS->USER,CNS_DRIVINGCONE)
                                     ->DATA) == incone->UEDGE.CONE)
             break ;
            }
         else if(((link->TYPE & (CNS_IN | CNS_INOUT)) != 0) &&
                 ((incone->TYPE & CNS_EXT) == CNS_EXT))
            {
             if(link->ULINK.LOCON == incone->UEDGE.LOCON)
             break ;
            }
        }

     if(link != NULL)
        {
         if((link->TYPE & CNS_COMMAND) != CNS_COMMAND){
           for(link = (link_list *)path[i]->LINK ; link != NULL ; link = link->NEXT)
             if((link->TYPE & CNS_COMMAND) == CNS_COMMAND)
               break ;
         }

         if(link != NULL)
           {
            cone_list *conecmd = (cone_list *)getptype(link->ULINK.LOTRS->USER,
                                                       CNS_DRIVINGCONE)->DATA ;
            cmd = tas_getcmd(cone,conecmd,link) ;
            if(!getptype(link->USER, TAS_LINK_UNUSED_SWITCH_COMMAND))
              if((cmd->TYPE & TTV_NODE_CMDMARQUE) != TTV_NODE_CMDMARQUE){
                  chaincmd = addchain(chaincmd,cmd) ;
                  cmd->TYPE |= TTV_NODE_CMDMARQUE ;
              }
            
           }
        }

    }
 for(chain = chaincmd ; chain != NULL ; chain = chain->NEXT)
  ((ttvevent_list *)chain->DATA)->TYPE &= ~(TTV_NODE_CMDMARQUE) ;
 return(chaincmd) ;
}

/*****************************************************************************/
/*                        function tas_linklocontcone()                      */
/* fonction qui relie un connecteur de sortie et un cone                     */ 
/*****************************************************************************/
void tas_linklocontcone(ttvfig,locon,incone)
ttvfig_list *ttvfig ;
locon_list *locon ;
edge_list *incone ;
{
 ptype_list *ptype ;
 ttvsig_list *loconsig ;
 ttvsig_list *inconesig ;
 delay_list *delaymax = NULL ;
 delay_list *delaymin = NULL ;
 ptype_list *ptypemax = (ptype_list *)getptype(incone->USER,TAS_DELAY_MAXT) ;
 ptype_list *ptypemin = (ptype_list *)getptype(incone->USER,TAS_DELAY_MINT) ;
 long hhmax = TTV_NOTIME ;
 long llmax = TTV_NOTIME ;
 long fhhmax = TTV_NOSLOPE ;
 long fllmax = TTV_NOSLOPE ;
 long hhmin = TTV_NOTIME ;
 long llmin = TTV_NOTIME ;
 long fhhmin = TTV_NOSLOPE ;
 long fllmin = TTV_NOSLOPE ;

 if((incone->TYPE & TAS_NORCT_INCONE) == TAS_NORCT_INCONE)
   return ;
 else
   incone->TYPE |= TAS_NORCT_INCONE ;

 if(ptypemax != NULL)
  {
   delaymax = (delay_list *)(ptypemax->DATA) ;
  }
 else
  return ;
 

 if(ptypemin != NULL)
  {
   delaymin = (delay_list *)(ptypemin->DATA) ;
  }
 else
  return ;

 if((ptype = getptype(locon->USER,TAS_SIG_CONE)) == NULL) return ;

 loconsig = (ttvsig_list *)ptype->DATA ;

 if((ptype = getptype(incone->USER,TAS_SIG_INCONE)) == NULL) return ;

 inconesig = (ttvsig_list *)ptype->DATA ;

 if(inconesig == loconsig) return ;

 if(delaymax != NULL)
  {
   if(delaymax->RCHH  != TAS_NOTIME) 
     hhmax = delaymax->RCHH ;
   else
     hhmax = (long)0 ;
   if(delaymax->FRCHH  != TAS_NOFRONT) 
     fhhmax = delaymax->FRCHH ;
   else
     fhhmax = (long)0 ;

   if(delaymax->RCLL  != TAS_NOTIME) 
     llmax = delaymax->RCLL ;
   else
     llmax = (long)0 ;
   if(delaymax->FRCLL  != TAS_NOFRONT) 
     fllmax = delaymax->FRCLL ;
   else
     fllmax = (long)0 ;
  }
 else
  {
   hhmax = TTV_NOTIME ;
   llmax = TTV_NOTIME ;
   fhhmax = TTV_NOSLOPE ;
   fllmax = TTV_NOSLOPE ;
  }
 if((delaymin != NULL) && (TAS_CONTEXT->TAS_FIND_MIN == 'Y'))
  {
   if(delaymin->RCHH  != TAS_NOTIME)
     hhmin = delaymin->RCHH ;
   else
     hhmin = (long)0 ;
   if(delaymin->FRCHH  != TAS_NOFRONT)
     fhhmin = delaymin->FRCHH ;
   else
     fhhmin = (long)0 ;

   if(delaymin->RCLL  != TAS_NOTIME)
     llmin = delaymin->RCLL ;
   else
     llmin = (long)0 ;
   if(delaymin->FRCLL  != TAS_NOFRONT)
     fllmin = delaymin->FRCLL ;
   else
     fllmin = (long)0 ;
  }
 else
  {
   hhmin = TTV_NOTIME ;
   llmin = TTV_NOTIME ;
   fhhmin = TTV_NOSLOPE ;
   fllmin = TTV_NOSLOPE ;
  }

  if((llmax != TTV_NOTIME) || (llmin != TTV_NOTIME))
   {
    ttv_addline(ttvfig,inconesig->NODE,loconsig->NODE,llmax,fllmax,llmin,fllmin,
                TTV_LINE_D|TTV_LINE_RC) ;
   }
  if((hhmax != TTV_NOTIME) || (hhmin != TTV_NOTIME))
   {
    ttv_addline(ttvfig,inconesig->NODE+1,loconsig->NODE+1,hhmax,fhhmax,hhmin,
                fhhmin,TTV_LINE_D|TTV_LINE_RC) ;
   }
}

/*****************************************************************************/
/*                        function tas_linkconelocon()                       */
/* fonction qui relie un connecteur de sortie et un cone                     */ 
/*****************************************************************************/
void tas_linkconelocon(ttvfig,locon)
ttvfig_list *ttvfig ;
locon_list *locon ;
{
 cone_list *cone ;
 ptype_list *ptype ;
 ttvsig_list *loconsig ;
 ttvsig_list *conesig ;
 delay_list *delaymax = NULL ;
 delay_list *delaymin = NULL ;
 ptype_list *ptypemax = (ptype_list *)getptype(locon->USER,TAS_DELAY_MAX) ;
 ptype_list *ptypemin = (ptype_list *)getptype(locon->USER,TAS_DELAY_MIN) ;
 long hhmax = TTV_NOTIME ;
 long llmax = TTV_NOTIME ;
 long fhhmax = TTV_NOSLOPE ;
 long fllmax = TTV_NOSLOPE ;
 long hhmin = TTV_NOTIME ;
 long llmin = TTV_NOTIME ;
 long fhhmin = TTV_NOSLOPE ;
 long fllmin = TTV_NOSLOPE ;

 if((ptype = getptype(locon->USER,CNS_EXT)) == NULL) return ;

 cone = (cone_list *)ptype->DATA ;

 if(getptype(cone->USER,TAS_RC_CONE) == NULL) return ;

 if(ptypemax != NULL)
  {
   delaymax = (delay_list *)(ptypemax->DATA) ;
  }
 
 if(ptypemin != NULL)
  {
   delaymin = (delay_list *)(ptypemin->DATA) ;
  }

 if((ptype = getptype(locon->USER,TAS_SIG_CONE)) == NULL) return ;

 loconsig = (ttvsig_list *)ptype->DATA ;

 if((ptype = getptype(cone->USER,TAS_SIG_CONE)) == NULL) return ;

 conesig = (ttvsig_list *)ptype->DATA ;

 if(conesig == loconsig) return ;

 if(delaymax != NULL)
  {
   if(delaymax->RCHH  != TAS_NOTIME) 
     hhmax = delaymax->RCHH ;
   else
     hhmax = (long)0 ;
   if(delaymax->FRCHH  != TAS_NOFRONT) 
     fhhmax = delaymax->FRCHH ;
   else
     fhhmax = (long)0 ;

   if(delaymax->RCLL  != TAS_NOTIME) 
     llmax = delaymax->RCLL ;
   else
     llmax = (long)0 ;
   if(delaymax->FRCLL  != TAS_NOFRONT) 
     fllmax = delaymax->FRCLL ;
   else
     fllmax = (long)0 ;
  }
 else
  {
   hhmax = TTV_NOTIME ;
   llmax = TTV_NOTIME ;
   fhhmax = TTV_NOSLOPE ;
   fllmax = TTV_NOSLOPE ;
  }
 if((delaymin != NULL) && (TAS_CONTEXT->TAS_FIND_MIN == 'Y'))
  {
   if(delaymin->RCHH  != TAS_NOTIME)
     hhmin = delaymin->RCHH ;
   else
     hhmin = (long)0 ;
   if(delaymin->FRCHH  != TAS_NOFRONT)
     fhhmin = delaymin->FRCHH ;
   else
     fhhmin = (long)0 ;

   if(delaymin->RCLL  != TAS_NOTIME)
     llmin = delaymin->RCLL ;
   else
     llmin = (long)0 ;
   if(delaymin->FRCLL  != TAS_NOFRONT)
     fllmin = delaymin->FRCLL ;
   else
     fllmin = (long)0 ;
  }
 else
  {
   hhmin = TTV_NOTIME ;
   llmin = TTV_NOTIME ;
   fhhmin = TTV_NOSLOPE ;
   fllmin = TTV_NOSLOPE ;
  }

  if((llmax != TTV_NOTIME) || (llmin != TTV_NOTIME))
   {
    ttv_addline(ttvfig,loconsig->NODE,conesig->NODE,llmax,fllmax,llmin,fllmin,
                TTV_LINE_D|TTV_LINE_RC) ;
   }
  if((hhmax != TTV_NOTIME) || (hhmin != TTV_NOTIME))
   {
    ttv_addline(ttvfig,loconsig->NODE+1,conesig->NODE+1,hhmax,fhhmax,hhmin,
                fhhmin,TTV_LINE_D|TTV_LINE_RC) ;
   }
}

/*****************************************************************************/
/*                        function tas_is_false_access()                     */
/*****************************************************************************/
int tas_is_false_access(ttvevent_list *nodesig, ttvevent_list *nodecmd, long type)
{
  int false = 0;
  
  if((type & TTV_LINE_A) == TTV_LINE_A){
      if((nodesig->ROOT == nodecmd->ROOT) && (nodesig != nodecmd)){
          false = 1;
      }else if(nodesig->ROOT != nodecmd->ROOT){
          false = 1;
      }
  }
  return false;
}

/*****************************************************************************/
/*                        function tas_linkconenode()                        */
/* fonction qui relie deux signaux de la ttvfig par des liens elementaires   */ 
/* qui ont pour valeur les temps des strutures delay dans incone.            */
/* in et out sont les signaux a relier.                                      */
/* la variable type indique la nature du lien D ou E ou F                    */
/* si out = NULL pas de delai de porte si sig = NULL pas de delai RC         */
/*****************************************************************************/
void tas_linkconenode(ttvfig_list *ttvfig,ttvsig_list *in,ttvsig_list *out,chain_list *siglist,cone_list *cone,edge_list *incone,long type)
{
 delay_list *delaymax = NULL ; 
 delay_list *delaymin = NULL ; 
 ptype_list *ptypemax = (ptype_list *)getptype(incone->USER,TAS_DELAY_MAX) ; 
 ptype_list *ptypemin = (ptype_list *)getptype(incone->USER,TAS_DELAY_MIN) ; 
 ptype_list *ptype ;
 chain_list *chain, *savechain ;
 chain_list *chaincmd = NULL ;
 ttvsig_list *sig;
 long hhmax = TTV_NOTIME ;
 long lhmax = TTV_NOTIME ;
 long hlmax = TTV_NOTIME ;
 long llmax = TTV_NOTIME ;
 long fhhmax = TTV_NOSLOPE ;
 long flhmax = TTV_NOSLOPE ;
 long fhlmax = TTV_NOSLOPE ;
 long fllmax = TTV_NOSLOPE ;
 long rhhmax = TTV_NORES ;
 long rlhmax = TTV_NORES ;
 long rhlmax = TTV_NORES ;
 long rllmax = TTV_NORES ;
 long shhmax = TTV_NOS ;
 long slhmax = TTV_NOS ;
 long shlmax = TTV_NOS ;
 long sllmax = TTV_NOS ;
 long hhmin = TTV_NOTIME ;
 long lhmin = TTV_NOTIME ;
 long hlmin = TTV_NOTIME ;
 long llmin = TTV_NOTIME ;
 long fhhmin = TTV_NOSLOPE ;
 long flhmin = TTV_NOSLOPE ;
 long fhlmin = TTV_NOSLOPE ;
 long fllmin = TTV_NOSLOPE ;
 long rhhmin = TTV_NORES  ;
 long rlhmin = TTV_NORES ;
 long rhlmin = TTV_NORES ;
 long rllmin = TTV_NORES ;
 long shhmin = TTV_NOS ;
 long slhmin = TTV_NOS ;
 long shlmin = TTV_NOS ;
 long sllmin = TTV_NOS ;
 long typehh ;
 long typelh ; 
 long typehl ;
 long typell ;
 long typers ;
 char* tmhhmax = NULL ;
 char* tmhlmax = NULL ;
 char* tmlhmax = NULL ;
 char* tmllmax = NULL ;
 char* fmhhmax = NULL ;
 char* fmhlmax = NULL ;
 char* fmlhmax = NULL ;
 char* fmllmax = NULL ;
 char* tmhhmin = NULL ;
 char* tmhlmin = NULL ;
 char* tmlhmin = NULL ;
 char* tmllmin = NULL ;
 char* fmhhmin = NULL ;
 char* fmhlmin = NULL ;
 char* fmlhmin = NULL ;
 char* fmllmin = NULL ;
 timing_model *tmodel;
 char         *modname;
 ttvsig_list  *outhz = NULL ;
 ttvsig_list  *outx = out ;
 locon_list   *locon ;
 char         *input_name;
 char         flagcmd ;
 char         flagline ;
 
 ttvline_list *ptline ;
 
 if((ptypemax == NULL) && (ptypemin == NULL)) return ;
 
 typehh = type ;
 typelh = type ;
 typehl = type ;
 typell = type ;

 if((incone->TYPE&CNS_CONE)==CNS_CONE){
     if(incone->UEDGE.CONE->NAME)
         input_name = incone->UEDGE.CONE->NAME;
 }else if(incone->UEDGE.LOCON->NAME){ 
         input_name = incone->UEDGE.LOCON->NAME;
 }


/* if((TAS_CONTEXT->TAS_CARAC_MEMORY == 'N') &&
    ((incone->TYPE & CNS_COMMAND) == CNS_COMMAND))
   flagcmd = 'Y' ;
 else*/ if((incone->TYPE & CNS_COMMAND) == CNS_COMMAND)
      {
       if((cone->TYPE & (CNS_LATCH|CNS_FLIP_FLOP)) != 0)
         {
          typehh |= TTV_LINE_A ;
          typelh |= TTV_LINE_A ;
          typehl |= TTV_LINE_A ;
          typell |= TTV_LINE_A ;
          flagcmd = 'N' ;
         }
        else
         flagcmd = 'Y' ;
      }
 else
   flagcmd = 'N' ;
 
 if(ptypemax != NULL)
 {
  delaymax = (delay_list *)(ptypemax->DATA) ; 
  if(delaymax->TPHH  != TAS_NOTIME) hhmax = delaymax->TPHH ;
  if(delaymax->TPLH  != TAS_NOTIME) lhmax = delaymax->TPLH ;
  if(delaymax->TPHL  != TAS_NOTIME) hlmax = delaymax->TPHL ;
  if(delaymax->TPLL  != TAS_NOTIME) llmax = delaymax->TPLL ;
  if(delaymax->FHH   != TAS_NOFRONT) fhhmax = delaymax->FHH ;
  if(delaymax->FLH   != TAS_NOFRONT) flhmax = delaymax->FLH ;
  if(delaymax->FHL   != TAS_NOFRONT) fhlmax = delaymax->FHL ;
  if(delaymax->FLL   != TAS_NOFRONT) fllmax = delaymax->FLL ;
 
          
  if(out != NULL)
  if(((in->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0) || ((out->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0)) {
     rhhmax = TTV_NORES ;
     rlhmax = TTV_NORES ;
     rhlmax = TTV_NORES ;
     rllmax = TTV_NORES ;
     sllmax = TTV_NORES ;
     shhmax = TTV_NOS ;
     slhmax = TTV_NOS ;
     shlmax = TTV_NOS ;
     sllmax = TTV_NOS ;
#ifdef USEOLDTEMP     
     if((out->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0)
      {
       if(delaymax->RHH != TAS_NORES) rhhmax = delaymax->RHH ;
       if(delaymax->RLH != TAS_NORES) rlhmax = delaymax->RLH ;
       if(delaymax->RHL != TAS_NORES) rhlmax = delaymax->RHL ;
       if(delaymax->RLL != TAS_NORES) rllmax = delaymax->RLL ;
      }
     if((in->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0)
      {
       if(delaymax->SHH != TAS_NOS) shhmax = delaymax->SHH ;
       if(delaymax->SLH != TAS_NOS) slhmax = delaymax->SLH ;
       if(delaymax->SHL != TAS_NOS) shlmax = delaymax->SHL ;
       if(delaymax->SLL != TAS_NOS) sllmax = delaymax->SLL ;
      }
#endif
    }
    if (delaymax->TMHH) {
        tmhhmax = delaymax->TMHH->NAME ;
    }else if((hhmax != TTV_NOTIME) && ((rhhmax != TTV_NORES) || (shhmax != TTV_NOS))){
        modname = stm_mod_name(input_name, 'U', cone->NAME, 'U', STM_DELAY, 'M', 0);
        tmodel = stm_mod_create(modname);
        stm_mod_update(tmodel, tmodel->VTH, tmodel->VDD, tmodel->VT, tmodel->VDD);
        stm_mod_update_transition(tmodel, STM_HH);
        tmodel->UTYPE = STM_MOD_MODPLN;
        if(rhhmax == TTV_NORES) rhhmax = -1;
        if(shhmax == TTV_NOS) shhmax = -1;
        tmodel->UMODEL.POLYNOM = stm_modpln_create_fequa (rhhmax, shhmax, tas_getcapai(cone), fhhmax, hhmax);
        tmhhmax = stm_storemodel(CELL, NULL, tmodel, 0);
    }
    if (delaymax->TMHL) {
    	tmhlmax = delaymax->TMHL->NAME ;
    }else if((hlmax != TTV_NOTIME) && ((rhlmax != TTV_NORES) || (shlmax != TTV_NOS))){
        modname = stm_mod_name(input_name, 'U', cone->NAME, 'D', STM_DELAY, 'M', 0);
        tmodel = stm_mod_create(modname);
        stm_mod_update(tmodel, tmodel->VTH, tmodel->VDD, tmodel->VT, 0.0);
        stm_mod_update_transition(tmodel, STM_HL);
        tmodel->UTYPE = STM_MOD_MODPLN;
        if(rhlmax == TTV_NORES) rhlmax = -1;
        if(shlmax == TTV_NOS) shlmax = -1;
        tmodel->UMODEL.POLYNOM = stm_modpln_create_fequa (rhlmax, shlmax, tas_getcapai(cone), fhlmax, hlmax);
        tmhlmax = stm_storemodel(CELL, NULL, tmodel, 0);
    }
    if (delaymax->TMLH) {
        tmlhmax = delaymax->TMLH->NAME ;
    }else if((lhmax != TTV_NOTIME) && ((rlhmax != TTV_NORES) || (slhmax != TTV_NOS))){
        modname = stm_mod_name(input_name, 'D', cone->NAME, 'U', STM_DELAY, 'M', 0);
        tmodel = stm_mod_create(modname);
        stm_mod_update(tmodel, tmodel->VTH, tmodel->VDD, tmodel->VT, tmodel->VDD);
        stm_mod_update_transition(tmodel, STM_LH);
        tmodel->UTYPE = STM_MOD_MODPLN;
        if(rlhmax == TTV_NORES) rlhmax = -1;
        if(slhmax == TTV_NOS) slhmax = -1;
        tmodel->UMODEL.POLYNOM = stm_modpln_create_fequa (rlhmax, slhmax, tas_getcapai(cone), flhmax, lhmax);
        tmlhmax = stm_storemodel(CELL, NULL, tmodel, 0);
    }
    if (delaymax->TMLL) {
        tmllmax = delaymax->TMLL->NAME ;
    }else if((llmax != TTV_NOTIME) && ((rllmax != TTV_NORES) || (sllmax != TTV_NOS))){
        modname = stm_mod_name(input_name, 'D', cone->NAME, 'D', STM_DELAY, 'M', 0);
        tmodel = stm_mod_create(modname);
        stm_mod_update(tmodel, tmodel->VTH, tmodel->VDD, tmodel->VT, 0.0);
        stm_mod_update_transition(tmodel, STM_LL);
        tmodel->UTYPE = STM_MOD_MODPLN;
        if(rllmax == TTV_NORES) rllmax = -1;
        if(sllmax == TTV_NOS) sllmax = -1;
        tmodel->UMODEL.POLYNOM = stm_modpln_create_fequa (rllmax, sllmax, tas_getcapai(cone), fllmax, llmax);
        tmllmax = stm_storemodel(CELL, NULL, tmodel, 0);
    }
    if (delaymax->FMHH) {
        fmhhmax = delaymax->FMHH->NAME;
    }else if(tmhhmax && (fhhmax != TAS_NOTIME)){
        modname = stm_mod_name(input_name, 'U', cone->NAME, 'U', STM_SLEW, 'M', 0);
        tmodel = stm_mod_create(modname);
        stm_mod_update(tmodel, tmodel->VTH, tmodel->VDD, tmodel->VT, tmodel->VDD);
        stm_mod_update_transition(tmodel, STM_HH);
        tmodel->UTYPE = STM_MOD_MODPLN;
        tmodel->UMODEL.POLYNOM = stm_modpln_create_fequaforslew (fhhmax, tas_getcapai(cone));
        fmhhmax = stm_storemodel(CELL, NULL, tmodel, 0);
    }
    if (delaymax->FMHL) {
        fmhlmax = delaymax->FMHL->NAME;
    }else if(tmhlmax && (fhlmax != TAS_NOTIME)){
        modname = stm_mod_name(input_name, 'U', cone->NAME, 'D', STM_SLEW, 'M', 0);
        tmodel = stm_mod_create(modname);
        stm_mod_update(tmodel, tmodel->VTH, tmodel->VDD, tmodel->VT, 0.0);
        stm_mod_update_transition(tmodel, STM_HL);
        tmodel->UTYPE = STM_MOD_MODPLN;
        tmodel->UMODEL.POLYNOM = stm_modpln_create_fequaforslew (fhlmax, tas_getcapai(cone));
        fmhlmax = stm_storemodel(CELL, NULL, tmodel, 0);
    }
    if (delaymax->FMLH) {
        fmlhmax = delaymax->FMLH->NAME;
    }else if(tmlhmax && (flhmax != TAS_NOTIME)){
        modname = stm_mod_name(input_name, 'D', cone->NAME, 'U', STM_SLEW, 'M', 0);
        tmodel = stm_mod_create(modname);
        stm_mod_update(tmodel, tmodel->VTH, tmodel->VDD, tmodel->VT, tmodel->VDD);
        stm_mod_update_transition(tmodel, STM_LH);
        tmodel->UTYPE = STM_MOD_MODPLN;
        tmodel->UMODEL.POLYNOM = stm_modpln_create_fequaforslew (flhmax, tas_getcapai(cone));
        fmlhmax = stm_storemodel(CELL, NULL, tmodel, 0);
    }
    if (delaymax->FMLL) {
        fmllmax = delaymax->FMLL->NAME;
    }else if(tmllmax && (fllmax != TAS_NOTIME)){
        modname = stm_mod_name(input_name, 'D', cone->NAME, 'D', STM_SLEW, 'M', 0);
        tmodel = stm_mod_create(modname);
        stm_mod_update(tmodel, tmodel->VTH, tmodel->VDD, tmodel->VT, 0.0);
        stm_mod_update_transition(tmodel, STM_LL);
        tmodel->UTYPE = STM_MOD_MODPLN;
        tmodel->UMODEL.POLYNOM = stm_modpln_create_fequaforslew (fllmax, tas_getcapai(cone));
        fmllmax = stm_storemodel(CELL, NULL, tmodel, 0);
    }
}
 
 if(ptypemin != NULL)
 {
  delaymin = (delay_list *)(ptypemin->DATA) ;
  if(delaymin->TPHH  != TAS_NOTIME) hhmin = delaymin->TPHH ;
  if(delaymin->TPLH  != TAS_NOTIME) lhmin = delaymin->TPLH ;
  if(delaymin->TPHL  != TAS_NOTIME) hlmin = delaymin->TPHL ;
  if(delaymin->TPLL  != TAS_NOTIME) llmin = delaymin->TPLL ;
  if(delaymin->FHH   != TAS_NOFRONT) fhhmin = delaymin->FHH ;
  if(delaymin->FLH   != TAS_NOFRONT) flhmin = delaymin->FLH ;
  if(delaymin->FHL   != TAS_NOFRONT) fhlmin = delaymin->FHL ;
  if(delaymin->FLL   != TAS_NOFRONT) fllmin = delaymin->FLL ;
  
  
  if(out != NULL)
  if(((in->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0) || ((out->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0)) {
     rhhmin = TTV_NORES ;
     rlhmin = TTV_NORES ;
     rhlmin = TTV_NORES ;
     rllmin = TTV_NORES ;
     sllmin = TTV_NORES ;
     shhmin = TTV_NOS ;
     slhmin = TTV_NOS ;
     shlmin = TTV_NOS ;
     sllmin = TTV_NOS ;
#ifdef USEOLDTEMP     
     if((out->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0)
      {
       if(delaymin->RHH != TAS_NORES) rhhmin = delaymin->RHH ;
       if(delaymin->RLH != TAS_NORES) rlhmin = delaymin->RLH ;
       if(delaymin->RHL != TAS_NORES) rhlmin = delaymin->RHL ;
       if(delaymin->RLL != TAS_NORES) rllmin = delaymin->RLL ;
      }
     if((in->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0)
      {
       if(delaymin->SHH != TAS_NOS) shhmin = delaymin->SHH ;
       if(delaymin->SLH != TAS_NOS) slhmin = delaymin->SLH ;
       if(delaymin->SHL != TAS_NOS) shlmin = delaymin->SHL ;
       if(delaymin->SLL != TAS_NOS) sllmin = delaymin->SLL ;
      }
#endif
    }
    if (delaymin->TMHH) {
        tmhhmin = delaymin->TMHH->NAME ;
    }else if((hhmin != TTV_NOTIME) && ((rhhmin != TTV_NORES) || (shhmin != TTV_NOS))){
        modname = stm_mod_name(input_name, 'U', cone->NAME, 'U', STM_DELAY, 'm', 0);
        tmodel = stm_mod_create(modname);
        stm_mod_update(tmodel, tmodel->VTH, tmodel->VDD, tmodel->VT, tmodel->VDD);
        stm_mod_update_transition(tmodel, STM_HH);
        tmodel->UTYPE = STM_MOD_MODPLN;
        if(rhhmin == TTV_NORES) rhhmin = -1;
        if(shhmin == TTV_NOS) shhmin = -1;
        tmodel->UMODEL.POLYNOM = stm_modpln_create_fequa (rhhmin, shhmin, tas_getcapai(cone), fhhmin, hhmin);
        tmhhmin = stm_storemodel(CELL, NULL, tmodel, 0);
    }
    if (delaymin->TMHL) {
        tmhlmin = delaymin->TMHL->NAME ;
    }else if((hlmin != TTV_NOTIME) && ((rhlmin != TTV_NORES) || (shlmin != TTV_NOS))){
        modname = stm_mod_name(input_name, 'U', cone->NAME, 'D', STM_DELAY, 'm', 0);
        tmodel = stm_mod_create(modname);
        stm_mod_update(tmodel, tmodel->VTH, tmodel->VDD, tmodel->VT, 0.0);
        stm_mod_update_transition(tmodel, STM_HL);
        tmodel->UTYPE = STM_MOD_MODPLN;
        if(rhlmin == TTV_NORES) rhlmin = -1;
        if(shlmin == TTV_NOS) shlmin = -1;
        tmodel->UMODEL.POLYNOM = stm_modpln_create_fequa (rhlmin, shlmin, tas_getcapai(cone), fhlmin, hlmin);
        tmhlmin = stm_storemodel(CELL, NULL, tmodel, 0);
    }
    if (delaymin->TMLH) {
        tmlhmin = delaymin->TMLH->NAME ;
    }else if((lhmin != TTV_NOTIME) && ((rlhmin != TTV_NORES) || (slhmin != TTV_NOS))){
        modname = stm_mod_name(input_name, 'D', cone->NAME, 'U', STM_DELAY, 'm', 0);
        tmodel = stm_mod_create(modname);
        stm_mod_update(tmodel, tmodel->VTH, tmodel->VDD, tmodel->VT, tmodel->VDD);
        stm_mod_update_transition(tmodel, STM_LH);
        tmodel->UTYPE = STM_MOD_MODPLN;
        if(rlhmin == TTV_NORES) rlhmin = -1;
        if(slhmin == TTV_NOS) slhmin = -1;
        tmodel->UMODEL.POLYNOM = stm_modpln_create_fequa (rlhmin, slhmin, tas_getcapai(cone), flhmin, lhmin);
        tmlhmin = stm_storemodel(CELL, NULL, tmodel, 0);
    }
    if (delaymin->TMLL) {
        tmllmin = delaymin->TMLL->NAME ;
    }else if((llmin != TTV_NOTIME) && ((rllmin != TTV_NORES) || (sllmin != TTV_NOS))){
        modname = stm_mod_name(input_name, 'D', cone->NAME, 'D', STM_DELAY, 'm', 0);
        tmodel = stm_mod_create(modname);
        stm_mod_update(tmodel, tmodel->VTH, tmodel->VDD, tmodel->VT, 0.0);
        stm_mod_update_transition(tmodel, STM_LL);
        tmodel->UTYPE = STM_MOD_MODPLN;
        if(rllmin == TTV_NORES) rllmin = -1;
        if(sllmin == TTV_NOS) sllmin = -1;
        tmodel->UMODEL.POLYNOM = stm_modpln_create_fequa (rllmin, sllmin, tas_getcapai(cone), fllmin, llmin);
        tmllmin = stm_storemodel(CELL, NULL, tmodel, 0);
    }
    if (delaymin->FMHH) {
        fmhhmin = delaymin->FMHH->NAME;
    }else if(tmhhmin && (fhhmin != TAS_NOTIME)){
        modname = stm_mod_name(input_name, 'U', cone->NAME, 'U', STM_SLEW, 'm', 0);
        tmodel = stm_mod_create(modname);
        stm_mod_update(tmodel, tmodel->VTH, tmodel->VDD, tmodel->VT, tmodel->VDD);
        stm_mod_update_transition(tmodel, STM_HH);
        tmodel->UTYPE = STM_MOD_MODPLN;
        tmodel->UMODEL.POLYNOM = stm_modpln_create_fequaforslew (fhhmin, tas_getcapai(cone));
        fmhhmin = stm_storemodel(CELL, NULL, tmodel, 0);
    }
    if (delaymin->FMHL) {
        fmhlmin = delaymin->FMHL->NAME;
    }else if(tmhlmin && (fhlmin != TAS_NOTIME)){
        modname = stm_mod_name(input_name, 'U', cone->NAME, 'D', STM_SLEW, 'm', 0);
        tmodel = stm_mod_create(modname);
        stm_mod_update(tmodel, tmodel->VTH, tmodel->VDD, tmodel->VT, 0.0);
        stm_mod_update_transition(tmodel, STM_HL);
        tmodel->UTYPE = STM_MOD_MODPLN;
        tmodel->UMODEL.POLYNOM = stm_modpln_create_fequaforslew (fhlmin, tas_getcapai(cone));
        fmhlmin = stm_storemodel(CELL, NULL, tmodel, 0);
    }
    if (delaymin->FMLH) {
        fmlhmin = delaymin->FMLH->NAME;
    }else if(tmlhmin && (flhmin != TAS_NOTIME)){
        modname = stm_mod_name(input_name, 'D', cone->NAME, 'U', STM_SLEW, 'm', 0);
        tmodel = stm_mod_create(modname);
        stm_mod_update(tmodel, tmodel->VTH, tmodel->VDD, tmodel->VT, tmodel->VDD);
        stm_mod_update_transition(tmodel, STM_LH);
        tmodel->UTYPE = STM_MOD_MODPLN;
        tmodel->UMODEL.POLYNOM = stm_modpln_create_fequaforslew (flhmin, tas_getcapai(cone));
        fmlhmin = stm_storemodel(CELL, NULL, tmodel, 0);
    }
    if (delaymin->FMLL) {
        fmllmin = delaymin->FMLL->NAME;
    }else if(tmllmin && (fllmin != TAS_NOTIME)){
        modname = stm_mod_name(input_name, 'D', cone->NAME, 'D', STM_SLEW, 'm', 0);
        tmodel = stm_mod_create(modname);
        stm_mod_update(tmodel, tmodel->VTH, tmodel->VDD, tmodel->VT, 0.0);
        stm_mod_update_transition(tmodel, STM_LL);
        tmodel->UTYPE = STM_MOD_MODPLN;
        tmodel->UMODEL.POLYNOM = stm_modpln_create_fequaforslew (fllmin, tas_getcapai(cone));
        fmllmin = stm_storemodel(CELL, NULL, tmodel, 0);
    }
 }
 
 if(((cone->TYPE & TAS_PRECHARGE) == TAS_PRECHARGE) ||
    ((incone->TYPE & CNS_HZCOM) == CNS_HZCOM))
   {
    if((cone->TYPE & TAS_PRECHARGE) == TAS_PRECHARGE)
      {
       typehh |= TTV_LINE_PR ;
       typelh |= TTV_LINE_PR ;
       typehl |= TTV_LINE_EV ;
       typell |= TTV_LINE_EV ;
      }
    else
      {
       locon=cns_get_one_cone_external_connector(cone);
       if(locon!=NULL/*(ptype = getptype(cone->USER,CNS_EXT)) != NULL*/)
         {
//          locon = (locon_list *)ptype->DATA ;
          if((ptype = getptype(locon->USER,TAS_SIG_CONE)) != NULL)
            {
             outhz = (ttvsig_list *)ptype->DATA ;
            }
         }
      }

    if((fhhmax == 0L) || (fhhmin == 0L)) 
      typehh |= TTV_LINE_HZ ;
    if((flhmax == 0L) || (flhmin == 0L)) 
      typelh |= TTV_LINE_HZ ;
    if((fhlmax == 0L) || (fhlmin == 0L)) 
      typehl |= TTV_LINE_HZ ;
    if((fllmax == 0L) || (fllmin == 0L)) 
      typell |= TTV_LINE_HZ ;
   }

 if((cone->TYPE & (CNS_RS|CNS_LATCH)) == (CNS_RS|CNS_LATCH))
   {
    if((cone->TECTYPE & CNS_NAND) == CNS_NAND)
     {
      if((incone->TYPE & (CNS_LOOP|CNS_EXT)) == CNS_LOOP)
       {
        if((incone->UEDGE.CONE->TYPE & CNS_RS) == CNS_RS)
         typers = TTV_LINE_S ;
        else
         typers = TTV_LINE_R ;
       }
      else
       typers = TTV_LINE_R ;
     }
    else if((cone->TECTYPE & CNS_NOR) == CNS_NOR)
     {
      if((incone->TYPE & (CNS_LOOP|CNS_EXT)) == CNS_LOOP)
       {
        if((incone->UEDGE.CONE->TYPE & CNS_RS) == CNS_RS)
         typers = TTV_LINE_R ;
        else
         typers = TTV_LINE_S ;
       }
      else
       typers = TTV_LINE_S ;
     }
    else
     typers = (long)0 ;
    typehh |= typers ;
    typelh |= typers ;
    typehl |= typers ;
    typell |= typers ;
   }

 if(((hhmax != TTV_NOTIME) || (hhmin != TTV_NOTIME)) && (out != NULL) && (flagcmd == 'N'))
  {
   savechain=chaincmd = tas_getlatchcmd(cone,incone,CNS_VDD);
   flagline = 0;
   if(((typehh & TTV_LINE_HZ) == TTV_LINE_HZ) && (outhz != NULL))
     {
      out = outhz ;
     }
   while(chaincmd || (!chaincmd && !flagline)){
       if((chaincmd && !tas_is_false_access(in->NODE+1, (ttvevent_list*)chaincmd->DATA, typehh)) || (!chaincmd && !flagline)){
           ptline = ttv_addline(ttvfig,out->NODE+1,in->NODE+1,hhmax,fhhmax,hhmin,fhhmin,typehh) ;
           
           if(((in->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0) || 
             ((out->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0) || tmhhmax)
             
             {
                 if(TAS_CONTEXT->TAS_FIND_MIN == 'Y')
                     ttv_addcaracline(ptline,tmhhmax,tmhhmin,fmhhmax,fmhhmin) ;
                 else
                     ttv_addcaracline(ptline,tmhhmax,NULL,fmhhmax,NULL) ;
        
                     
             }
           if(chaincmd && ((cone->TYPE & (CNS_LATCH|CNS_FLIP_FLOP)) != 0))
             {
              TAS_PATH_TYPE = 'M' ;
              if(hhmax != TTV_NOTIME) 
               ttv_addcmd(ptline,TTV_LINE_CMDMAX,(ttvevent_list*)chaincmd->DATA) ;
              TAS_PATH_TYPE = 'm' ;
              if(hhmin != TTV_NOTIME) 
               ttv_addcmd(ptline,TTV_LINE_CMDMIN,(ttvevent_list*)chaincmd->DATA) ;
             }
           if(!chaincmd && (cone->TYPE & (CNS_MEMSYM)))
             {
              TAS_PATH_TYPE = 'M' ;
              if(hhmax != TTV_NOTIME) 
               ttv_addcmd(ptline,TTV_LINE_CMDMAX,in->NODE+1) ;
              TAS_PATH_TYPE = 'm' ;
              if(hhmin != TTV_NOTIME) 
               ttv_addcmd(ptline,TTV_LINE_CMDMIN,in->NODE+1) ;
             }
       }
       if(!chaincmd && !flagline) break;
       flagline = 1;
       chaincmd = chaincmd->NEXT;
   }
   out = outx ;
   freechain(savechain);
  }
 if(((lhmax != TTV_NOTIME) || (lhmin != TTV_NOTIME)) && (out != NULL) && (flagcmd == 'N'))
  {
   savechain=chaincmd = tas_getlatchcmd(cone,incone,CNS_VDD);
   flagline = 0;
   if(((typelh & TTV_LINE_HZ) == TTV_LINE_HZ) && (outhz != NULL))
     {
      out = outhz ;
     }
   while(chaincmd || (!chaincmd && !flagline)){
       if((chaincmd && !tas_is_false_access(in->NODE, (ttvevent_list*)chaincmd->DATA, typelh)) || (!chaincmd && !flagline)){
           ptline = ttv_addline(ttvfig,out->NODE+1,in->NODE,lhmax,flhmax,lhmin,flhmin,typelh) ;
           if(((in->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0) || 
             ((out->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0) || tmlhmax)
            {
                 if(TAS_CONTEXT->TAS_FIND_MIN == 'Y')
                     ttv_addcaracline(ptline,tmlhmax,tmlhmin,fmlhmax,fmlhmin) ;
                 else
                     ttv_addcaracline(ptline,tmlhmax,NULL,fmlhmax,NULL) ;
                     
            }
           if(chaincmd && ((cone->TYPE & (CNS_LATCH|CNS_FLIP_FLOP)) != 0))
             {
              TAS_PATH_TYPE = 'M' ;
              if(lhmax != TTV_NOTIME) 
               ttv_addcmd(ptline,TTV_LINE_CMDMAX,(ttvevent_list*)chaincmd->DATA) ;
              TAS_PATH_TYPE = 'm' ;
              if(lhmin != TTV_NOTIME) 
               ttv_addcmd(ptline,TTV_LINE_CMDMIN,(ttvevent_list*)chaincmd->DATA) ;
             }
       }
       if(!chaincmd && !flagline) break;
       flagline = 1;
       chaincmd = chaincmd->NEXT;
   }
   out = outx ;
   freechain(savechain);
  }
 if(((hlmax != TTV_NOTIME) || (hlmin != TTV_NOTIME)) && (out != NULL) && (flagcmd == 'N'))
  {
   savechain=chaincmd = tas_getlatchcmd(cone,incone,CNS_VSS);
   flagline = 0;
   if(((typehl & TTV_LINE_HZ) == TTV_LINE_HZ) && (outhz != NULL))
     {
      out = outhz ;
     }
   while(chaincmd || (!chaincmd && !flagline)){
       if((chaincmd && !tas_is_false_access(in->NODE+1, (ttvevent_list*)chaincmd->DATA, typehl)) || (!chaincmd && !flagline)){
           ptline = ttv_addline(ttvfig,out->NODE,in->NODE+1,hlmax,fhlmax,hlmin,fhlmin,typehl) ;
           if(((in->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0) || 
             ((out->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0) || tmhlmax)
            {
                 if(TAS_CONTEXT->TAS_FIND_MIN == 'Y')
                     ttv_addcaracline(ptline,tmhlmax,tmhlmin,fmhlmax,fmhlmin) ;
                 else
                     ttv_addcaracline(ptline,tmhlmax,NULL,fmhlmax,NULL) ;
                     
            }
           if(chaincmd && ((cone->TYPE & (CNS_LATCH|CNS_FLIP_FLOP)) != 0))
             {
              TAS_PATH_TYPE = 'M' ;
              if(hlmax != TTV_NOTIME) 
               ttv_addcmd(ptline,TTV_LINE_CMDMAX,(ttvevent_list*)chaincmd->DATA) ;
              TAS_PATH_TYPE = 'm' ;
              if(hlmin != TTV_NOTIME) 
               ttv_addcmd(ptline,TTV_LINE_CMDMIN,(ttvevent_list*)chaincmd->DATA) ;
             }
       }
       if(!chaincmd && !flagline) break;
       flagline = 1;
       chaincmd = chaincmd->NEXT;
   }
   out = outx ;
   freechain(savechain);
  }
 if(((llmax != TTV_NOTIME) || (llmin != TTV_NOTIME)) && (out != NULL) && (flagcmd == 'N'))
  {
   savechain=chaincmd = tas_getlatchcmd(cone,incone,CNS_VSS);
   flagline = 0;
   if(((typell & TTV_LINE_HZ) == TTV_LINE_HZ) && (outhz != NULL))
     {
      out = outhz ;
     }
   while(chaincmd || (!chaincmd && !flagline)){
       if((chaincmd && !tas_is_false_access(in->NODE, (ttvevent_list*)chaincmd->DATA, typell)) || (!chaincmd && !flagline)){
           ptline = ttv_addline(ttvfig,out->NODE,in->NODE,llmax,fllmax,llmin,fllmin,typell) ;
           if(((in->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0) || 
             ((out->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0) || tmllmax)
            {
                 if(TAS_CONTEXT->TAS_FIND_MIN == 'Y')
                     ttv_addcaracline(ptline,tmllmax,tmllmin,fmllmax,fmllmin) ;
                 else
                     ttv_addcaracline(ptline,tmllmax,NULL,fmllmax,NULL) ;
                     
            }
           if(chaincmd && ((cone->TYPE & (CNS_LATCH|CNS_FLIP_FLOP)) != 0))
             {
              TAS_PATH_TYPE = 'M' ;
              if(llmax != TTV_NOTIME) 
               ttv_addcmd(ptline,TTV_LINE_CMDMAX,(ttvevent_list*)chaincmd->DATA) ;
              TAS_PATH_TYPE = 'm' ;
              if(llmin != TTV_NOTIME) 
               ttv_addcmd(ptline,TTV_LINE_CMDMIN,(ttvevent_list*)chaincmd->DATA) ;
             }
           if(!chaincmd && (cone->TYPE & (CNS_MEMSYM)))
             {
              TAS_PATH_TYPE = 'M' ;
              if(llmax != TTV_NOTIME) 
               ttv_addcmd(ptline,TTV_LINE_CMDMAX,in->NODE) ;
              TAS_PATH_TYPE = 'm' ;
              if(llmin != TTV_NOTIME) 
               ttv_addcmd(ptline,TTV_LINE_CMDMIN,in->NODE) ;
             }
       }
       if(!chaincmd && !flagline) break;
       flagline = 1;
       chaincmd = chaincmd->NEXT;
   }
   out = outx ;
   freechain(savechain);
  }

 if((siglist != NULL) && ((incone->TYPE & TAS_NORC_INCONE) != TAS_NORC_INCONE))
 { 
   incone->TYPE |= TAS_NORC_INCONE ;
   while (siglist!=NULL)
   {
    sig=(ttvsig_list *)siglist->DATA;
    if(delaymax != NULL)
     {
      if((delaymax->TPHH  != TAS_NOTIME) || (delaymax->TPHL  != TAS_NOTIME) ||
         ((incone->TYPE & CNS_COMMAND) == CNS_COMMAND) ||
         ((incone->TYPE & TAS_RCUU_INCONE) == TAS_RCUU_INCONE))
       {
        if(delaymax->RCHH  != TAS_NOTIME) 
          hhmax = delaymax->RCHH ;
        else
          hhmax = (long)0 ;
        if(delaymax->FRCHH  != TAS_NOFRONT) 
          fhhmax = delaymax->FRCHH ;
        else
          fhhmax = (long)0 ;
       }
      else
       {
        hhmax = TTV_NOTIME ;
        fhhmax = TTV_NOSLOPE ;
       }

      if((delaymax->TPLL  != TAS_NOTIME) || (delaymax->TPLH  != TAS_NOTIME) ||
         ((incone->TYPE & CNS_COMMAND) == CNS_COMMAND) ||
         ((incone->TYPE & TAS_RCDD_INCONE) == TAS_RCDD_INCONE))
       {
        if(delaymax->RCLL  != TAS_NOTIME) 
          llmax = delaymax->RCLL ;
        else
          llmax = (long)0 ;
        if(delaymax->FRCLL  != TAS_NOFRONT) 
          fllmax = delaymax->FRCLL ;
        else
          fllmax = (long)0 ;
       }
      else
       {
        llmax = TTV_NOTIME ;
        fllmax = TTV_NOSLOPE ;
       }
     }
    else
     {
      hhmax = TTV_NOTIME ;
      llmax = TTV_NOTIME ;
      fhhmax = TTV_NOSLOPE ;
      fllmax = TTV_NOSLOPE ;
     }
    if((delaymin != NULL) && (TAS_CONTEXT->TAS_FIND_MIN == 'Y'))
     {
      if((delaymin->TPHH  != TAS_NOTIME) || (delaymin->TPHL  != TAS_NOTIME) ||
         ((incone->TYPE & CNS_COMMAND) == CNS_COMMAND) ||
         ((incone->TYPE & TAS_RCUU_INCONE) == TAS_RCUU_INCONE))
       {
        if(delaymin->RCHH  != TAS_NOTIME)
          hhmin = delaymin->RCHH ;
        else
          hhmin = (long)0 ;
        if(delaymin->FRCHH  != TAS_NOFRONT)
          fhhmin = delaymin->FRCHH ;
        else
          fhhmin = (long)0 ;
       }
      else
       {
        hhmin = TTV_NOTIME ;
        fhhmin = TTV_NOSLOPE ;
       }

      if((delaymin->TPLL  != TAS_NOTIME) || (delaymin->TPLH  != TAS_NOTIME) ||
         ((incone->TYPE & CNS_COMMAND) == CNS_COMMAND) ||
         ((incone->TYPE & TAS_RCDD_INCONE) == TAS_RCDD_INCONE))
       {
        if(delaymin->RCLL  != TAS_NOTIME)
          llmin = delaymin->RCLL ;
        else
          llmin = (long)0 ;
        if(delaymin->FRCLL  != TAS_NOFRONT)
          fllmin = delaymin->FRCLL ;
        else
          fllmin = (long)0 ;
       }
      else
       {
        llmin = TTV_NOTIME ;
        fllmin = TTV_NOSLOPE ;
       }
     }
    else
     {
      hhmin = TTV_NOTIME ;
      llmin = TTV_NOTIME ;
      fhhmin = TTV_NOSLOPE ;
      fllmin = TTV_NOSLOPE ;
     }
     if((type & TTV_LINE_F) == TTV_LINE_F)
      {
       if((ptype = getptype(incone->USER,TAS_IN_RCIN)) != NULL) 
        {
         for(chain = (chain_list *)ptype->DATA ; chain != NULL ; 
             chain = chain->NEXT)
          {
           if((((edge_list *)chain->DATA)->TYPE & (TAS_EXT_IN|TAS_EXT_OUT)) != 0)
             {
              type &= ~(TTV_LINE_F) ;
              type |= TTV_LINE_E ;
              break ;
             }
          }
        }
      }
     if((llmax != TTV_NOTIME) || (llmin != TTV_NOTIME))
      {
       ttv_addline(ttvfig,in->NODE,sig->NODE,llmax,fllmax,llmin,fllmin,
                   type|TTV_LINE_RC) ;
      }
     if((hhmax != TTV_NOTIME) || (hhmin != TTV_NOTIME))
      {
       ttv_addline(ttvfig,in->NODE+1,sig->NODE+1,hhmax,fhhmax,hhmin,fhhmin,
                   type|TTV_LINE_RC) ;
      }
    
    siglist=delchain(siglist, siglist);
   }
 }
 else
 {
  freechain(siglist);
 }
}

static void assigngatetype(cone_list *cone, ttvsig_list *tvs)
{
  ptype_list *pt;
  int num=0;
  alim_list *power;
  
  if ((pt=getptype(cone->USER,CNS_SIGNATURE))!=NULL)
   {
     num=ttv_findgateinfo((char *)pt->DATA);
     if (num!=0) ttv_setsigflaggate(tvs, num);
   }
  if (num==0)
  {
    if ((cone->TYPE & CNS_MEMSYM)==CNS_MEMSYM) ttv_setsigflaggate(tvs,TTV_SIG_GATE_MEMSYM);
    else if ((cone->TYPE & CNS_RS)==CNS_RS) ttv_setsigflaggate(tvs,TTV_SIG_GATE_RS);
    else if ((cone->TYPE & (CNS_CONFLICT|CNS_TRI))==(CNS_CONFLICT|CNS_TRI)) ttv_setsigflaggate(tvs,TTV_SIG_GATE_TRISTATE_CONFLICT);
    else if ((cone->TYPE & CNS_TRI)==CNS_TRI) ttv_setsigflaggate(tvs,TTV_SIG_GATE_TRISTATE);
    else if ((cone->TYPE & CNS_CONFLICT)==CNS_CONFLICT) ttv_setsigflaggate(tvs,TTV_SIG_GATE_CONFLICT);
    else if ((cone->TECTYPE & CNS_DUAL_CMOS)==CNS_DUAL_CMOS) ttv_setsigflaggate(tvs,TTV_SIG_GATE_DUAL_CMOS);
  }
  else
  {
    char *buf;
    int nbi;
    if (ttv_getgateinfo(num, &buf, &nbi))
    {
      if ((cone->TYPE & CNS_RS)==CNS_RS)
      {
        if (strcasecmp(buf,"nd2")==0) ttv_setsigflaggate(tvs,TTV_SIG_GATE_NAND_RS);
        else if (strcasecmp(buf,"nr2")==0) ttv_setsigflaggate(tvs,TTV_SIG_GATE_NOR_RS);
      }
    }
  }
  if (cone->TYPE & CNS_MASTER) ttv_setsigflag(tvs, TTV_SIG_FLAGS_ISMASTER);
  if (cone->TYPE & CNS_SLAVE) ttv_setsigflag(tvs, TTV_SIG_FLAGS_ISSLAVE);
  
  if ((cone->TYPE & CNS_EXT)!=0 || cone->INCONE==NULL)
  {
    power = cns_get_multivoltage(cone);
    if (power!=NULL && (power->VDDMAX!=V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE || power->VSSMIN!=0))
      ttv_set_signal_swing(tvs, power->VSSMIN, power->VDDMAX);
  }
}

/*****************************************************************************/
/*                        function tas_cns2ttv()                             */
/* fonction qui verifie que l'on n'a pas de boucle dans la cnsfig. Si il y   */
/* en a on casse la boucle, on envoie un warning et on sauve dans un fichier */
/* .loop les signaux par ou passe la boucle                                  */
/* Quand les boucles sont supprimees, on construit a partir de la cnsfig la  */
/* ttvfig.                                                                   */
/*****************************************************************************/
ttvfig_list *tas_cns2ttv(ptfig,ttvfig,lofig)
cnsfig_list    *ptfig ;
ttvfig_list *ttvfig ;
lofig_list *lofig ;
{
cone_list *cone ;
locon_list *connect ;
locon_list *tabcon[2] ;
losig_list *losig ;
ttvsig_list *ptsig ;
long type ;
long typefile ;
ptype_list *ptype ;
ptype_list *ptypesig ;
chain_list *chain ;
chain_list *hchain = NULL ;
chain_list *list_loop = NULL ;
chain_list *loop = NULL ;
int nb_loop = 0 ;
int delete ;
chain_list *consig = NULL;
chain_list *ncsig = NULL;
long nbconsig = 0 ;
long nbncsig = 0 ;
chain_list *lcmdsig = NULL;
chain_list *latchsig = NULL;
chain_list *presig = NULL;
chain_list *intsig = NULL;
chain_list *breaksig = NULL;
chain_list *esig = NULL;
edge_list *incone ;
char condloop = 'N' ;
int j, oldmode ;

/*****************************************************************************/
/* initialisation de la cnsfig pour le parcours en profondeur de detection   */
/* de boucle . on cherche les entrees de graphe (latch + les connecteur on   */
/* les a deja.                                                               */
/*****************************************************************************/

 mbk_comcheck( 0, cns_signcns(ptfig), 0 );

 oldmode=ttv_addsig_addrcxpnode(1);

 if(TAS_CONTEXT->TAS_SHORT_MODELNAME == 'N')
   tas_calcaracline(NULL,NULL,-1,0) ;
 else
   tas_calcaracline(NULL,NULL,1,(long)ttvfig) ;

 tabcon[0] = ptfig->LOCON ;
 tabcon[1] = ptfig->INTCON ;

 for(j = 0 ; j < 2 ; j++)
 for(connect = tabcon[j] ; connect != NULL ; connect = connect->NEXT)
    {
     edge_list *in ;
     edge_list *out ;

      if((connect->DIRECTION == CNS_VDDC) || (connect->DIRECTION == CNS_VSSC) ||
         (connect->DIRECTION == TAS_ALIM))
          continue ;

      if((connect->DIRECTION == 'O') || (connect->DIRECTION == 'B') ||
         (connect->DIRECTION == 'Z') || (connect->DIRECTION == 'T')   )
        {
         if((ptype = getptype(connect->USER,CNS_EXT)) == NULL)
             continue ;
         
         cone = (cone_list *)ptype->DATA ;
         //hchain = addchain(hchain,cone) ;

         if(connect->TYPE != EXTERNAL)
             continue ;

         ptype = getptype(connect->USER,CNS_CONE) ;
         if(ptype != NULL)
          {
           for(chain = ptype->DATA ; chain != NULL ; chain = chain->NEXT)
             if((cone_list *)chain->DATA != cone) 
               ((cone_list *)chain->DATA)->TYPE |= TAS_ECONE ;
           }

         for(in = cone->INCONE ; in != NULL ; in = in->NEXT)
           {
            if((getptype(in->USER,TAS_RC_LOCON) == NULL) ||
               (TAS_CONTEXT->TAS_MERGERCN == 'Y'))
             {
              if((in->TYPE & CNS_EXT) != CNS_EXT)
                  in->UEDGE.CONE->TYPE |= TAS_ECONE ;
             }
           }

         for(out = cone->OUTCONE ; out != NULL ; out = out->NEXT)
            if((out->TYPE & CNS_EXT) != CNS_EXT)
                out->UEDGE.CONE->TYPE |= TAS_ECONE ;
        }
      else
       {
         if(connect->TYPE != EXTERNAL)
             continue ;

         ptype = getptype(connect->USER,CNS_EXT) ;
         
         if(ptype != NULL)
           {
            cone = (cone_list *)ptype->DATA ;

            for(out = cone->OUTCONE ; out != NULL ; out = out->NEXT)
               if((out->TYPE & CNS_EXT) != CNS_EXT)
                   out->UEDGE.CONE->TYPE |= TAS_ECONE ;

            ptype = getptype(connect->USER,CNS_CONE) ;

            if(ptype != NULL)
              {
               for(chain = ptype->DATA ; chain != NULL ; chain = chain->NEXT)
                if((cone_list *)chain->DATA != cone)
                  ((cone_list *)chain->DATA)->TYPE |= TAS_ECONE ;
              }
           }
         else
           {
            ptype = getptype(connect->USER,CNS_CONE) ;
            if(ptype != NULL)
            for(chain = ptype->DATA ; chain != NULL ; chain = chain->NEXT)
               {
                cone = (cone_list *)chain->DATA ;
                cone->TYPE |= TAS_ECONE ;
               }
            }
       }
    }

 for(cone = ptfig->CONE ; cone != NULL ; cone = cone->NEXT)
    {
     cone->USER = addptype(cone->USER,TAS_INCONE,(void *)cone->INCONE ) ;
     cone->USER = addptype(cone->USER,TAS_IN_BACK,(void*)NULL) ;

/*     if(cone->OUTCONE == NULL) 
        hchain = addchain(hchain,cone) ;*/
        
     if((cone->TYPE & (CNS_LATCH|CNS_FLIP_FLOP|CNS_MEMSYM|CNS_PRECHARGE)) != 0)
      {
        edge_list *in ;

        //hchain = addchain(hchain,cone) ;
        for(in = cone->INCONE ; in != NULL ; in = in->NEXT)
          {
           cone_list *conex = in->UEDGE.CONE ;
           if((in->TYPE & CNS_COMMAND) == CNS_COMMAND)
            {
             if((conex->TYPE & (CNS_VDD|CNS_VSS)) != 0)
               {
                int i ;
                branch_list *path[3] ;

                in->TYPE &= ~(CNS_COMMAND) ;
                path[0] = cone->BREXT;
                path[1] = cone->BRVDD;
                path[2] = cone->BRVSS;

                for(i = 0 ; i < 3 ; i++)
                for(; path[i] != NULL ; path[i] = path[i]->NEXT)
                if((path[i]->TYPE & CNS_NOT_FUNCTIONAL) != CNS_NOT_FUNCTIONAL)
                   {
                    link_list *link ;
               
                    for(link = (link_list *)path[i]->LINK ; link != NULL ; 
                        link = link->NEXT)
                       {
                        if((link->TYPE & (CNS_IN|CNS_INOUT)) == 0)
                          {
                           if(((cone_list *)getptype(link->ULINK.LOTRS->USER,
                               CNS_DRIVINGCONE)->DATA) == conex) 
                              link->TYPE &= ~(CNS_COMMAND) ;
                          }
                       }
                    }
              }
             else if((conex->TYPE & TAS_DEJAEMPILE) != TAS_DEJAEMPILE)
                {
                 conex->TYPE |= (TAS_LCOMMAND | TAS_DEJAEMPILE) ;
                 //hchain = addchain(hchain,conex) ;
                }
            }
          }
      }
    }

/*****************************************************************************/
/* parcours en profondeur on type les conne par ou on passe si l'on repasse  */
/* un cone tout en avancant dans le graphe c'est que l'on est dans une       */
/* boucle. dans ce cas on sauve le chemin par ou passe la boucle et on       */
/* l'ajoute a la liste des boucles.                                          */
/*****************************************************************************/
/*
 while(1)
  {
   for(chain = hchain ; chain != NULL ; chain = chain->NEXT)
    {
     cone_list *sommet ;

     sommet = (cone_list *)chain->DATA ;

     while(((getptype(sommet->USER,TAS_IN_BACK)->DATA) != NULL) 
           || (sommet->INCONE != NULL))
        {
         cone_list   *aux_sommet ;

         if((sommet->INCONE != NULL)  &&
           ((sommet->INCONE->TYPE & (CNS_FEEDBACK | CNS_VDD | CNS_VSS 
           | CNS_COMMAND | TAS_PATH_INOUT | CNS_BLEEDER)) == 0))
            {
             if((sommet->INCONE->TYPE & CNS_EXT) == CNS_EXT)
             {
              connect = sommet->INCONE->UEDGE.LOCON ;
              if((connect->DIRECTION == CNS_B) || (connect->DIRECTION == CNS_T))
               {
                ptype = getptype(connect->USER,CNS_EXT) ;
                if(ptype == NULL) aux_sommet = NULL ;
                else aux_sommet = ptype->DATA ;
               }
              else aux_sommet = NULL ;
             }
             else 
              {
               if(((sommet->INCONE->UEDGE.CONE)->TYPE & (CNS_LATCH|CNS_FLIP_FLOP|CNS_PRECHARGE)) == 0)
                 aux_sommet = sommet->INCONE->UEDGE.CONE ;
               else
                 aux_sommet = NULL ;
              }
            }
           else aux_sommet = NULL ;

         if(aux_sommet != NULL)
            {
             if(((aux_sommet->TYPE & TAS_MARQUE) == TAS_MARQUE) && 
                 (aux_sommet->INCONE != NULL))
                {
                 nb_loop ++ ;

                 if(condloop == 'N')
                  {
                   tas_error(31,ptfig->NAME,TAS_WARNING) ;
                   condloop = 'Y' ;
                  }

                 for(cone=sommet;cone!=aux_sommet; cone=(cone_list *)
                        (getptype(cone->USER,TAS_IN_BACK)->DATA))
                    {   
                     loop = addchain(loop,(void *)cone->NAME) ;
                    }
    
                 loop = addchain(loop,(void *)aux_sommet->NAME) ;
                }
        
             if((nb_loop != 0) && (loop != NULL))
                {
                 list_loop = addchain(list_loop,(void *)loop) ;
                 loop = NULL ;
                 sommet->INCONE->TYPE |= (CNS_FEEDBACK) ;
                 sommet->INCONE = sommet->INCONE->NEXT ;
                 aux_sommet = sommet ;
                }
             else (getptype(aux_sommet->USER,TAS_IN_BACK)->DATA) = sommet;
    
             aux_sommet->TYPE |= TAS_MARQUE ;
             sommet = aux_sommet ;
            }
         else
            {
             if(sommet->INCONE == NULL)
                {
                 cone_list   *aux_cone = sommet ;

                 sommet = (getptype(sommet->USER,TAS_IN_BACK)->DATA) ;
                 (getptype(aux_cone->USER,TAS_IN_BACK)->DATA) = NULL ;
                }

             if(sommet->INCONE != NULL) sommet->INCONE =  sommet->INCONE->NEXT ;
            }
        }
    }

   for(chain = hchain ; chain != NULL ; chain = chain->NEXT)
    {
     cone = (cone_list *)chain->DATA ;
     cone->TYPE |= TAS_MARQUE ;
    }

  freechain(hchain) ;
  hchain = NULL ;
  for(cone = ptfig->CONE ; cone != NULL ; cone = cone->NEXT)
   {
    if((cone->TYPE & TAS_MARQUE) != TAS_MARQUE)
      hchain = addchain(hchain,cone) ;
   }
  if(hchain == NULL)
   break ;
 }
*/
 for(cone = ptfig->CONE ; cone != NULL ; cone = cone->NEXT)
  {
   cone->USER = delptype(cone->USER,TAS_IN_BACK) ;
   cone->INCONE = (edge_list *)(getptype(cone->USER,TAS_INCONE)->DATA) ;
   cone->USER = delptype(cone->USER,TAS_INCONE) ;
   cone->TYPE &= ~(TAS_MARQUE) ;
  }

/*****************************************************************************/
/* creation des noeuds connecteurs externes                                  */
/*****************************************************************************/

 if((TAS_CONTEXT->TAS_MERGERCN == 'N') && (TAS_CONTEXT->TAS_CALCRCX == 'Y'))
   tas_detectsig(ptfig) ;

 for(j = 0 ; j < 2 ; j++)
 for(connect = tabcon[j] ; connect != NULL ; connect = connect->NEXT)
    {
      if((connect->DIRECTION == CNS_VDDC) || (connect->DIRECTION == CNS_VSSC) ||
         (connect->DIRECTION == TAS_ALIM))
          continue ;
         losig = connect->SIG ;

      if((getptype(losig->USER,TAS_SIG_NORCDELAY) == NULL) &&
         (TAS_CONTEXT->TAS_MERGERCN == 'N') && (TAS_CONTEXT->TAS_CALCRCX == 'Y'))
        continue ;

      if((connect->DIRECTION == 'O') || (connect->DIRECTION == 'B') ||
         (connect->DIRECTION == 'Z') || (connect->DIRECTION == 'T')   )
        {
         if((ptype = getptype(connect->USER,CNS_EXT)) == NULL)
             continue ;
         cone = (cone_list *)ptype->DATA ;

         if((cone->TYPE & TAS_PRECHARGE) == TAS_PRECHARGE)
          {
           losig->USER = addptype(losig->USER,TAS_SIG_TYPE,(void *)TTV_SIG_R) ;
          }
         else if((cone->TYPE & (CNS_LATCH|CNS_FLIP_FLOP)) != 0)
          {
           losig->USER = addptype(losig->USER,TAS_SIG_TYPE,(void *)TTV_SIG_L) ;
          }
         else if((cone->TYPE & TAS_LCOMMAND) == TAS_LCOMMAND)
          {
           losig->USER = addptype(losig->USER,TAS_SIG_TYPE,(void *)TTV_SIG_Q) ;
          }
         else if((cone->TYPE & TAS_ECONE) == TAS_ECONE)
          {
           losig->USER = addptype(losig->USER,TAS_SIG_TYPE,(void *)TTV_SIG_EXT) ;
          }
        }
      else
       {
         ptype = getptype(connect->USER,CNS_EXT) ;
         
         if(ptype != NULL)
           {
            cone = (cone_list *)ptype->DATA ;
            if((cone->TYPE & TAS_LCOMMAND) == TAS_LCOMMAND)
             losig->USER = addptype(losig->USER,TAS_SIG_TYPE,(void *)TTV_SIG_Q);
            else if((cone->TYPE & TAS_ECONE) == TAS_ECONE)
             losig->USER = addptype(losig->USER,TAS_SIG_TYPE,(void *)TTV_SIG_EXT);
           }
       }
    }

 if(lofig->LOINS != NULL)
   tas_detecloconsig(lofig,ttvfig) ;

 consig = (chain_list *)ttvfig->CONSIG ;

 for(connect = ptfig->LOCON ; connect != NULL ; connect = connect->NEXT)
    {
     ttvsig_list *ptnsig = NULL ;
     ttvsig_list *ptconsig = NULL ;

      if((connect->DIRECTION == CNS_VDDC) || (connect->DIRECTION == CNS_VSSC) ||
         (connect->DIRECTION == TAS_ALIM))
          continue ;

      if((connect->DIRECTION == 'O') || (connect->DIRECTION == 'B') ||
         (connect->DIRECTION == 'Z') || (connect->DIRECTION == 'T')   )
        {
         if((ptype = getptype(connect->USER,CNS_EXT)) == NULL)
             continue ;
         cone = (cone_list *)ptype->DATA ;
         ptype = getptype(connect->USER,CNS_CONE) ;
         if(ptype != NULL)
          {
           for(chain = ptype->DATA ; chain != NULL ; chain = chain->NEXT)
             if((cone_list *)chain->DATA != cone) 
                break;
           }
         else chain = NULL ;
         if(connect->DIRECTION == 'O') type = TTV_SIG_CO ;
         else if(connect->DIRECTION == 'B') type = TTV_SIG_CB ;
         else if(connect->DIRECTION == 'Z') type = TTV_SIG_CZ ;
         else if(connect->DIRECTION == 'T') type = TTV_SIG_CT ;
         else type = TTV_SIG_CX ;
         if((ptype = getptype(connect->USER,TAS_LOCON_SIG)) != NULL)
           {
            ptconsig = (ttvsig_list *)ptype->DATA ;
           }
         else
           {
            consig = ttv_addrefsig(ttvfig,tas_getsigname(NULL,connect,NULL),
                                   tas_locon_netname(connect) /*->NAME*/,
                                   ((caraccon_list *)getptype(connect->USER,TAS_CON_CARAC)->DATA)->C,
                                   type,consig) ;    
            ptconsig = (ttvsig_list *)consig->DATA ;
            ttv_setallsigcapas ( lofig, connect->SIG, ptconsig);
            nbconsig ++ ;
           }
         assigngatetype(cone, ptconsig);
         if (getptype(cone->USER,TAS_SIG_CONE)==NULL)
         {
           if((getptype(cone->USER,TAS_RC_CONE) != NULL) &&
              (TAS_CONTEXT->TAS_MERGERCN == 'N'))
            {
             ncsig = ttv_addrefsig(ttvfig,tas_getsigname(cone,NULL,NULL),
                                   cone->NAME,((caraccon_list *)getptype(connect->USER,TAS_CON_CARAC)->DATA)->C,
                                   TTV_SIG_N,ncsig) ;    
             ptnsig = (ttvsig_list *)ncsig->DATA ;
             nbncsig++ ;
            }
             
           if((cone->TYPE & TAS_PRECHARGE) == TAS_PRECHARGE)
            {
             if(ptnsig == NULL)
               presig = addchain(presig,ptconsig) ;
             else
               presig = addchain(presig,ptnsig) ;
             ((ttvsig_list *)presig->DATA)->TYPE |= (TTV_SIG_EXT|TTV_SIG_R) ;
             assigngatetype(cone, (ttvsig_list *)presig->DATA);
             tas_error(36,connect->NAME,TAS_WARNING) ;
            }
           else if((cone->TYPE & (CNS_LATCH|CNS_FLIP_FLOP)) != 0)
            {
             if(ptnsig == NULL)
               latchsig = addchain(latchsig,ptconsig) ;
             else
               latchsig = addchain(latchsig,ptnsig) ;
             if((cone->TYPE & CNS_LATCH) == CNS_LATCH)
              {
               if((cone->TYPE & CNS_RS) == CNS_RS)
                {
                 if((cone->TECTYPE & CNS_NAND) == CNS_NAND)
                   ((ttvsig_list *)latchsig->DATA)->TYPE |= (TTV_SIG_EXT|
                                                             TTV_SIG_LR) ;
                 else if((cone->TECTYPE & CNS_NOR) == CNS_NOR)
                   ((ttvsig_list *)latchsig->DATA)->TYPE |= (TTV_SIG_EXT|
                                                             TTV_SIG_LS) ;
                 else
                   ((ttvsig_list *)latchsig->DATA)->TYPE |= (TTV_SIG_EXT|
                                                             TTV_SIG_LL) ;
                }
               else
               ((ttvsig_list *)latchsig->DATA)->TYPE |= (TTV_SIG_EXT|TTV_SIG_LL) ;
              }
             else
               ((ttvsig_list *)latchsig->DATA)->TYPE |= (TTV_SIG_EXT|TTV_SIG_LF) ;
             assigngatetype(cone, (ttvsig_list *)latchsig->DATA);
             tas_error(35,connect->NAME,TAS_WARNING) ;
            }
           else if((cone->TYPE & TAS_LCOMMAND) == TAS_LCOMMAND)
            {
             if(ptnsig == NULL)
               lcmdsig = addchain(lcmdsig,ptconsig) ;
             else
               lcmdsig = addchain(lcmdsig,ptnsig) ;

             ((ttvsig_list *)lcmdsig->DATA)->TYPE |= (TTV_SIG_EXT|TTV_SIG_Q) ;
             assigngatetype(cone, (ttvsig_list *)lcmdsig->DATA);
            }

           if(ptnsig == NULL)
             cone->USER = addptype(cone->USER,TAS_SIG_CONE,ptconsig) ;
           else
             {
              cone->USER = addptype(cone->USER,TAS_SIG_CONE,ptnsig) ;
             }
         }
         else
         {
           if((getptype(cone->USER,TAS_RC_CONE) != NULL) &&
              (TAS_CONTEXT->TAS_MERGERCN == 'N'))
             ptnsig=(ttvsig_list *)getptype(cone->USER,TAS_SIG_CONE)->DATA;
         }
         if((chain != NULL) || (ptnsig != NULL))
          connect->USER = addptype(connect->USER,TAS_SIG_CONE,ptconsig) ;
         if(ptnsig != NULL)
          connect->USER = addptype(connect->USER,TAS_LOCON_OUTCONE,ptnsig) ;
        }
      else
       {
         ptype = getptype(connect->USER,CNS_EXT) ;
         
         if(ptype != NULL)
           {
            cone = (cone_list *)ptype->DATA ;
            if((cone->TYPE & TAS_PRECHARGE) == TAS_PRECHARGE)
             {
              tas_error(47,connect->NAME,TAS_WARNING) ;
             }
            if((cone->TYPE & (CNS_LATCH|CNS_FLIP_FLOP)) != 0)
             {
              tas_error(48,connect->NAME,TAS_WARNING) ;
             }

            ptype = getptype(connect->USER,CNS_CONE) ;
            if(ptype != NULL)
              {
               for(chain = ptype->DATA ; chain != NULL ; chain = chain->NEXT)
                if((cone_list *)chain->DATA != cone) 
                  break;
              }
            else chain = NULL ;
            if(connect->DIRECTION == 'I') type = TTV_SIG_CI ;
            else type = TTV_SIG_CX ;
            if((ptype = getptype(connect->USER,TAS_LOCON_SIG)) != NULL)
              {
               ptconsig = (ttvsig_list *)ptype->DATA ;
              }
            else
              {
               consig = ttv_addrefsig(ttvfig,tas_getsigname(NULL,connect,NULL),
                                      tas_locon_netname(connect)/*->NAME*/,((caraccon_list *)getptype(connect->USER,TAS_CON_CARAC)->DATA)->C,
                                      type,consig) ;    
               ptconsig = (ttvsig_list *)consig->DATA ;
               ttv_setallsigcapas ( lofig, connect->SIG, ptconsig);
               nbconsig ++ ;
               if((cone->TYPE & TAS_LCOMMAND) == TAS_LCOMMAND)
                {
                 if(ptnsig == NULL)
                   lcmdsig = addchain(lcmdsig,ptconsig) ;
                 else
                   lcmdsig = addchain(lcmdsig,ptnsig) ;

                 ((ttvsig_list *)lcmdsig->DATA)->TYPE |= (TTV_SIG_EXT|TTV_SIG_Q) ;
                }
              }
            cone->USER = addptype(cone->USER,TAS_SIG_CONE,ptconsig) ;
            if(chain != NULL)
             connect->USER = addptype(connect->USER,TAS_SIG_CONE,ptconsig) ;
            if(chain==NULL && getptype(connect->USER,TAS_LOCON_SIG)==NULL)
             connect->USER = addptype(connect->USER,TAS_LOCON_SIG, ptconsig);
           }
         else
           {
            ptype = getptype(connect->USER,CNS_CONE) ;
            if(ptype == NULL)
             cone = NULL ;

            if(connect->DIRECTION == 'I') 
              {
               type = TTV_SIG_CI ;
              }
            else 
              {
               type = TTV_SIG_CX ;
              }

            if((ptype = getptype(connect->USER,TAS_LOCON_SIG)) != NULL)
              {
               ptconsig = (ttvsig_list *)ptype->DATA ;
              }
            else
              {
               consig = ttv_addrefsig(ttvfig,tas_getsigname(NULL,connect,NULL),
                                      connect->NAME,((caraccon_list *)getptype(connect->USER,TAS_CON_CARAC)->DATA)->C,
                                      type,consig) ;    
               ptconsig = (ttvsig_list *)consig->DATA ;
               ttv_setallsigcapas ( lofig, connect->SIG, ptconsig);
               nbconsig ++ ;
              }
            connect->USER = addptype(connect->USER,TAS_SIG_CONE,ptconsig) ;
           }
       }
     if((cone != NULL) && ((cone->TYPE & CNS_EXT) == CNS_EXT))
       {
        if((cone->TYPE & TAS_BYPASSOUT) == TAS_BYPASSOUT)
           {
            ptconsig->TYPE |= TTV_SIG_BYPASSOUT ;
           }
        if((cone->TYPE & TAS_BYPASSIN) == TAS_BYPASSIN)
           {
            ptconsig->TYPE |= TTV_SIG_BYPASSIN ;
           }
       }
     else
       {
        if(getptype(connect->USER,TAS_CON_BYPASSOUT) != NULL)
           {
            ptconsig->TYPE |= TTV_SIG_BYPASSOUT ;
            connect->USER = delptype(connect->USER,TAS_CON_BYPASSOUT) ;
           }
        if(getptype(connect->USER,TAS_CON_BYPASSIN) != NULL)
           {
            ptconsig->TYPE |= TTV_SIG_BYPASSIN ;
            connect->USER = delptype(connect->USER,TAS_CON_BYPASSIN) ;
           }
       }
    }

 for(connect = ptfig->INTCON ; connect != NULL ; connect = connect->NEXT)
    {
     ttvsig_list *ptsig = NULL ;

      if((connect->DIRECTION == CNS_VDDC) || (connect->DIRECTION == CNS_VSSC) ||
         (connect->DIRECTION == TAS_ALIM))
          continue ;

      if((connect->DIRECTION == 'O') || (connect->DIRECTION == 'B') ||
         (connect->DIRECTION == 'Z') || (connect->DIRECTION == 'T')   )
        {
         cone = (cone_list *)getptype(connect->USER,CNS_EXT)->DATA ;

         ptype = getptype(connect->USER,CNS_CONE) ;
         if(ptype != NULL)
          {
           for(chain = ptype->DATA ; chain != NULL ; chain = chain->NEXT)
             if((cone_list *)chain->DATA != cone) 
                break;
           }
         else chain = NULL ;

         if((ptype = getptype(connect->USER,TAS_LOCON_SIG)) == NULL)
           {
             if((cone->TYPE & TAS_PRECHARGE) == TAS_PRECHARGE)
              {
               presig = ttv_addrefsig(ttvfig,tas_getsigname(cone,NULL,NULL),
                                   cone->NAME,
                                   ((caraccon_list *)getptype(connect->USER,
                                   TAS_CON_CARAC)->DATA)->C,
                                   TTV_SIG_R,presig) ;    
               ptsig = presig->DATA ;
              }
             else if((cone->TYPE & (CNS_LATCH|CNS_FLIP_FLOP)) != 0)
              {
               latchsig = ttv_addrefsig(ttvfig,tas_getsigname(cone,NULL,NULL),
                                   cone->NAME,
                                   ((caraccon_list *)getptype(connect->USER,
                                   TAS_CON_CARAC)->DATA)->C,
                                   TTV_SIG_L,latchsig) ;    
               if((cone->TYPE & CNS_LATCH) == CNS_LATCH)
                 {
                  if((cone->TYPE & CNS_RS) == CNS_RS)
                   {
                    if((cone->TECTYPE & CNS_NAND) == CNS_NAND)
                      ((ttvsig_list *)latchsig->DATA)->TYPE |= TTV_SIG_LR ;
                    else if((cone->TECTYPE & CNS_NOR) == CNS_NOR)
                      ((ttvsig_list *)latchsig->DATA)->TYPE |= TTV_SIG_LS ;
                    else
                      ((ttvsig_list *)latchsig->DATA)->TYPE |= TTV_SIG_LL ;
                   }
                  else
                  ((ttvsig_list *)latchsig->DATA)->TYPE |= TTV_SIG_LL ;
                 }
                else
                  ((ttvsig_list *)latchsig->DATA)->TYPE |= TTV_SIG_LF ;
                ptsig = latchsig->DATA ;
               }
             else if((cone->TYPE & TAS_LCOMMAND) == TAS_LCOMMAND)
              {
               lcmdsig = ttv_addrefsig(ttvfig,tas_getsigname(cone,NULL,NULL),
                                   cone->NAME,
                                   ((caraccon_list *)getptype(connect->USER,
                                   TAS_CON_CARAC)->DATA)->C,
                                   TTV_SIG_Q,lcmdsig) ;    
               ptsig = lcmdsig->DATA ;
              }
             else if((cone->TYPE & TAS_INTER) == TAS_INTER)
              {
               intsig = ttv_addrefsig(ttvfig,tas_getsigname(cone,NULL,NULL),
                                      cone->NAME,tas_getcapai(cone),
                                      TTV_SIG_I,intsig) ;
               ptsig = intsig->DATA ;
              }
             else if((cone->TYPE & TAS_BREAK) == TAS_BREAK)
              {
               breaksig = ttv_addrefsig(ttvfig,tas_getsigname(cone,NULL,NULL),
                                      cone->NAME,tas_getcapai(cone),
                                      TTV_SIG_B,breaksig) ;
               ptsig = breaksig->DATA ;
              }
             else if((cone->TYPE & TAS_ECONE) == TAS_ECONE)
              {
               esig = ttv_addrefsig(ttvfig,tas_getsigname(cone,NULL,NULL),
                                    cone->NAME,tas_getcapai(cone),
                                    TTV_SIG_S,esig) ;
               ptsig = esig->DATA ;
              }
             else
              {
               ptsig = ttv_addsig(ttvfig,tas_getsigname(cone,NULL,NULL),
                          cone->NAME,tas_getcapai(cone),
                          TTV_SIG_S) ;
              }
            connect->USER = addptype(connect->USER,TAS_LOCON_SIG,ptsig) ;
            connect->USER = addptype(connect->USER,TAS_LOCON_OUTCONE,ptsig) ;
           }
         else 
           ptsig = (ttvsig_list *)ptype->DATA ;
           

         cone->USER = addptype(cone->USER,TAS_SIG_CONE,ptsig) ;

         if(chain != NULL)
          connect->USER = addptype(connect->USER,TAS_SIG_CONE,ptsig) ;
        }
      else
       {
         ptype = getptype(connect->USER,CNS_EXT) ;
         
         if(ptype != NULL)
           {
            cone = (cone_list *)ptype->DATA ;
            ptype = getptype(connect->USER,CNS_CONE) ;
            if(ptype != NULL)
              {
               for(chain = ptype->DATA ; chain != NULL ; chain = chain->NEXT)
                if((cone_list *)chain->DATA != cone) 
                  break;
              }
            else chain = NULL ;

            if((ptype = getptype(connect->USER,TAS_LOCON_SIG)) != NULL)
              {
               ptsig = (ttvsig_list *)ptype->DATA ;
               cone->USER = addptype(cone->USER,TAS_SIG_CONE,ptsig) ;
               if(chain != NULL)
                connect->USER = addptype(connect->USER,TAS_SIG_CONE,ptsig) ;
              }
           }
         else
           {
            ptype = getptype(connect->USER,CNS_CONE) ;
            if(ptype == NULL)
             cone = NULL ;

            if(connect->DIRECTION == 'I') 
              {
               type = TTV_SIG_CI ;
              }
            else 
              {
               type = TTV_SIG_CX ;
              }

            if((ptype = getptype(connect->USER,TAS_LOCON_SIG)) != NULL)
              {
               ptsig = (ttvsig_list *)ptype->DATA ;
               connect->USER = addptype(connect->USER,TAS_SIG_CONE,ptsig) ;
              }
           }
       }

     if((cone != NULL) && (ptsig != NULL) && 
        ((cone->TYPE & CNS_EXT) == CNS_EXT))
       {
        if((cone->TYPE & TAS_BYPASSOUT) == TAS_BYPASSOUT)
           {
            ptsig->TYPE |= TTV_SIG_BYPASSOUT ;
           }
        if((cone->TYPE & TAS_BYPASSIN) == TAS_BYPASSIN)
           {
            ptsig->TYPE |= TTV_SIG_BYPASSIN ;
           }
       }
     else if(ptsig != NULL)
       {
        if(getptype(connect->USER,TAS_CON_BYPASSOUT) != NULL)
           {
            ptsig->TYPE |= TTV_SIG_BYPASSOUT ;
            connect->USER = delptype(connect->USER,TAS_CON_BYPASSOUT) ;
           }
        if(getptype(connect->USER,TAS_CON_BYPASSIN) != NULL)
           {
            ptsig->TYPE |= TTV_SIG_BYPASSIN ;
            connect->USER = delptype(connect->USER,TAS_CON_BYPASSIN) ;
           }
       }
    }

/*****************************************************************************/
/* creation des noeuds qui correspondent a des reseaux RC                    */
/*****************************************************************************/

 if((TAS_CONTEXT->TAS_MERGERCN == 'N') && (TAS_CONTEXT->TAS_CALCRCX == 'Y'))
  {
   hchain = NULL ;

   for(cone = ptfig->CONE ; cone != NULL ; cone = cone->NEXT)
    {
     if((cone->TYPE & (CNS_VDD | CNS_VSS)) != 0) continue ;
     for(incone = cone->INCONE ; incone != NULL ; incone = incone->NEXT)
       {
        float netcapa ;
        char *netname ;
        if((incone->TYPE & (CNS_FEEDBACK | CNS_VDD | CNS_VSS | CNS_BLEEDER | 
                           TAS_PATH_INOUT)) != 0)
          {
           if((incone->TYPE & (CNS_COMMAND | CNS_VDD | CNS_VSS | CNS_BLEEDER | 
                           TAS_PATH_INOUT)) != CNS_COMMAND)
             continue ;
          }
        if((ptype = getptype(incone->USER,TAS_RC_LOCON)) != NULL)
          {
           for(chain = (chain_list *)ptype->DATA ; chain != NULL ;
               chain = chain->NEXT)
            {
             connect = (locon_list *)chain->DATA ;
             ptype = getptype(connect->USER,TAS_CON_INCONE) ;
             ptype = (ptype_list *)ptype->DATA ;
             if(ptype->DATA == NULL)
               {
                ptype->NEXT = (ptype_list *)incone ;

                if((incone->TYPE & CNS_EXT) == CNS_EXT)
                 {
                  connect = incone->UEDGE.LOCON ;
                  netname = connect->NAME ;
                  netcapa = ((caraccon_list *)getptype(connect->USER,TAS_CON_CARAC)->DATA)->C ;
                  ptypesig = getptype(connect->USER,TAS_SIG_CONE) ;
                  if(ptypesig != NULL)
                   {
                    ptsig = (ttvsig_list *)ptypesig->DATA ;
                   }
                  else
                   {
                    ptsig = NULL ;
                   }
                 }
                else if((incone->UEDGE.CONE->TYPE & CNS_EXT) == CNS_EXT)
                 {
                  netname = incone->UEDGE.CONE->NAME ;
/*                  ptypesig = getptype(incone->UEDGE.CONE->USER,CNS_EXT) ;
                  connect = (locon_list *)ptypesig->DATA ;*/
                  connect=cns_get_one_cone_external_connector(incone->UEDGE.CONE);
                  netcapa = ((caraccon_list *)getptype(connect->USER,TAS_CON_CARAC)->DATA)->C ;
                  ptypesig = getptype(connect->USER,TAS_SIG_CONE) ;
                  if(ptypesig != NULL)
                   {
                    ptsig = (ttvsig_list *)ptypesig->DATA ;
                   }
                  else
                   {
                    ptypesig = getptype(incone->UEDGE.CONE->USER,TAS_SIG_CONE) ;
                    if(ptypesig != NULL)
                     {
                      ptsig = (ttvsig_list *)ptypesig->DATA ;
                     }
                    else
                     {
                      ptsig = NULL ;
                     }
                   }
                 }
                else
                 {
                  netname = incone->UEDGE.CONE->NAME ;
                  netcapa = tas_getcapai(incone->UEDGE.CONE) ;
                  connect = NULL ;
                  ptsig = NULL ;
                 }
                if((ptype->TYPE & TTV_SIG_NQ) == TTV_SIG_NQ)
                 {
                  lcmdsig = ttv_addrefsig(ttvfig,
                                          tas_getsigname(NULL,NULL,incone),
                                          netname,netcapa,
                                          TTV_SIG_NQ|TTV_SIG_EXT,lcmdsig) ;
                  ncsig = addchain(ncsig,lcmdsig->DATA) ;
                  nbncsig++ ;
                  ptype->DATA = ncsig->DATA ;
                  ptsig = ncsig->DATA ;
                 }
                else if((ptype->TYPE & TTV_SIG_Q) == TTV_SIG_Q)
                 {
                  lcmdsig = ttv_addrefsig(ttvfig,
                                          tas_getsigname(NULL,NULL,incone),
                                          netname,netcapa,
                                       TTV_SIG_Q,lcmdsig) ;
                  ptype->DATA = lcmdsig->DATA ;
                  ptsig = lcmdsig->DATA ;
                 }
                else if((ptype->TYPE & TTV_SIG_N) == TTV_SIG_N)
                 {
                  ncsig = ttv_addrefsig(ttvfig,
                                        tas_getsigname(NULL,NULL,incone),
                                        netname,netcapa,
                                       TTV_SIG_N,ncsig) ;
                  nbncsig++ ;
                  ptype->DATA = ncsig->DATA ;
                  ptsig = ncsig->DATA ;
                 }
                else if((ptype->TYPE & TTV_SIG_EXT) == TTV_SIG_EXT)
                 {
                  esig = ttv_addrefsig(ttvfig,
                                       tas_getsigname(NULL,NULL,incone),
                                       netname,netcapa,
                                       TTV_SIG_S,esig) ;
                  ptype->DATA = esig->DATA ;
                  ptsig = esig->DATA ;
                 }
                else
                 {
                  ptsig = ttv_addsig(ttvfig,
                                     tas_getsigname(NULL,NULL,incone),
                                     netname,netcapa,
                                     TTV_SIG_S) ;
                  ptype->DATA = (void *)ptsig ;
                 }
                if((connect != NULL) && (connect->TYPE != EXTERNAL))
                  {
                   if((ptypesig = getptype(connect->USER,TAS_LOCON_INLIST)) == NULL)
                     {
                      connect->USER = addptype(connect->USER,TAS_LOCON_INLIST,
                                               addchain(NULL,ptsig)) ;
                     }
                   else
                     {
                      ptypesig->DATA = addchain(ptypesig->DATA,ptsig) ;
                     }
                  }

                break ;
               }
              else
               {
                delay_list *delay ;
                delay_list *delayx ;

                incone->TYPE |= TAS_NORC_INCONE ;
                if((incone->TYPE & CNS_EXT) == CNS_EXT)
                 if(incone->UEDGE.LOCON->DIRECTION == CNS_T)
                   incone->TYPE |= TAS_NORCT_INCONE ;
                if((incone->TYPE & CNS_COMMAND) == CNS_COMMAND)
                 {
                  ((edge_list *)ptype->NEXT)->TYPE |= (TAS_RCUU_INCONE|
                                                       TAS_RCDD_INCONE) ; 
                 }
                else
                 {
                  ptypesig = getptype(incone->USER,TAS_DELAY_MAX) ;
                  if(ptypesig == NULL)
                    ptypesig = getptype(incone->USER,TAS_DELAY_MIN) ;
                  if(ptypesig != NULL)
                   {
                    delay = (delay_list *)ptypesig->DATA ;
                    if((delay->TPHH != TAS_NOTIME) || 
                       (delay->TPHL != TAS_NOTIME))
                    ((edge_list *)ptype->NEXT)->TYPE |= TAS_RCUU_INCONE ;
                    if((delay->TPLL != TAS_NOTIME) || 
                       (delay->TPLH != TAS_NOTIME))
                    ((edge_list *)ptype->NEXT)->TYPE |= TAS_RCDD_INCONE ;
                   }
                 }
                if((ptypesig = getptype(((edge_list *)ptype->NEXT)->USER,
                                       TAS_IN_RCIN)) == NULL) 
                 {
                  ((edge_list *)ptype->NEXT)->USER = addptype(
                  ((edge_list *)ptype->NEXT)->USER,TAS_IN_RCIN,
                  (void *)addchain(NULL,(void *)incone)) ;
                 }
                else
                 {
                  ptypesig->DATA = (void *)addchain(
                  (chain_list *)ptypesig->DATA,(void *)incone) ;
                 }
                 ptypesig = getptype(((edge_list *)ptype->NEXT)->USER,
                                      TAS_DELAY_MAX) ;
                 if(ptypesig != NULL)
                 {
                  delay = (delay_list *)ptypesig->DATA ;
                  ptypesig = getptype(incone->USER,TAS_DELAY_MAX) ;
                  if(ptypesig != NULL)
                    {
                     delayx = (delay_list *)ptypesig->DATA ;
                     if((delay->RCHH == TAS_NOTIME) ||
                       ((delayx->RCHH != TAS_NOTIME) &&
                             (delayx->RCHH > delay->RCHH)))
                      {
                        delay->RCHH = delayx->RCHH ;
                        delay->FRCHH = delayx->FRCHH ;
                      }
                     if((delay->RCLL == TAS_NOTIME) ||
                       ((delayx->RCLL != TAS_NOTIME) &&
                             (delayx->RCLL > delay->RCLL)))
                      {
                        delay->RCLL = delayx->RCLL ;
                        delay->FRCLL = delayx->FRCLL ;
                      }
                    }
                 }
                 ptypesig = getptype(((edge_list *)ptype->NEXT)->USER,
                                      TAS_DELAY_MIN) ;
                 if(ptypesig != NULL)
                 {
                  delay = (delay_list *)ptypesig->DATA ;
                  ptypesig = getptype(incone->USER,TAS_DELAY_MIN) ;
                  if(ptypesig != NULL)
                    {
                     delayx = (delay_list *)ptypesig->DATA ;
                     if((delay->RCHH == TAS_NOTIME) ||
                       ((delayx->RCHH != TAS_NOTIME) &&
                             (delayx->RCHH < delay->RCHH)))
                      {
                        delay->RCHH = delayx->RCHH ;
                        delay->FRCHH = delayx->FRCHH ;
                      }
                     if((delay->RCLL == TAS_NOTIME) ||
                       ((delayx->RCLL != TAS_NOTIME) &&
                             (delayx->RCLL < delay->RCLL)))
                      {
                        delay->RCLL = delayx->RCLL ;
                        delay->FRCLL = delayx->FRCLL ;
                      }
                    }
                 }
               }
            }
           incone->USER = addptype(incone->USER,TAS_SIG_INCONE,
                                   ptype->DATA) ;
          }
       }
    }

   for(cone = ptfig->CONE ; cone != NULL ; cone = cone->NEXT)
    {
     if((cone->TYPE & (CNS_VDD | CNS_VSS)) != 0) continue ;

     for(incone = cone->INCONE ; incone != NULL ; incone = incone->NEXT)
       {
        if((incone->TYPE & (CNS_FEEDBACK | CNS_VDD | CNS_VSS | CNS_BLEEDER |
                           TAS_PATH_INOUT)) != 0)
          {
           if((incone->TYPE & (CNS_COMMAND | CNS_VDD | CNS_VSS | CNS_BLEEDER | 
                           TAS_PATH_INOUT)) != CNS_COMMAND)
           continue ;
          }
        if((ptype = getptype(incone->USER,TAS_RC_LOCON)) != NULL)
          {
           for(chain = (chain_list *)ptype->DATA ; chain != NULL ;
               chain = chain->NEXT)
            {
             connect = (locon_list *)chain->DATA ;
             if((ptype = getptype(connect->USER,TAS_CON_INCONE)) != NULL)
              {
               ptype = (ptype_list *)ptype->DATA ;
               if(ptype->DATA != NULL)
                {
                 hchain = addchain(hchain,(void *)ptype) ;
                 ptype->DATA = NULL ;
                 ptype->NEXT = NULL ;
                }
               connect->USER = delptype(connect->USER,TAS_CON_INCONE) ;
              }
            }
          }
        }
     }
   for(chain = hchain ; chain != NULL ; chain = chain->NEXT)
     freeptype((ptype_list *)chain->DATA) ;
   freechain(hchain) ;
  }

/*****************************************************************************/
/* creation des noeuds qui ne sont pas des connecteurs.                      */
/*****************************************************************************/

   for(cone = ptfig->CONE ; cone != NULL ; cone = cone->NEXT)
    {
     if((cone->TYPE & (CNS_VDD | CNS_VSS | CNS_EXT)) != 0) continue ;

     if((cone->TYPE & TAS_LCOMMAND) == TAS_LCOMMAND)
      {
       lcmdsig = ttv_addrefsig(ttvfig,tas_getsigname(cone,NULL,NULL),
                               cone->NAME,tas_getcapai(cone),
                               TTV_SIG_Q,lcmdsig) ;    
       ptype = cone->USER = addptype(cone->USER,TAS_SIG_CONE,lcmdsig->DATA) ;
      }

     else if((cone->TYPE & (CNS_LATCH|CNS_FLIP_FLOP)) != 0)
       {
        if((cone->TYPE & CNS_LATCH) == CNS_LATCH)
         {
          if((cone->TYPE & CNS_RS) == CNS_RS)
           {
            if((cone->TECTYPE & CNS_NAND) == CNS_NAND)
              latchsig = ttv_addrefsig(ttvfig,tas_getsigname(cone,NULL,NULL),
                                       cone->NAME,tas_getcapai(cone),
                                       TTV_SIG_LR,latchsig) ;
            else if((cone->TECTYPE & CNS_NOR) == CNS_NOR)
              latchsig = ttv_addrefsig(ttvfig,tas_getsigname(cone,NULL,NULL),
                                       cone->NAME,tas_getcapai(cone),
                                       TTV_SIG_LS,latchsig) ;
            else
              latchsig = ttv_addrefsig(ttvfig,tas_getsigname(cone,NULL,NULL),
                                       cone->NAME,tas_getcapai(cone),
                                       TTV_SIG_LL,latchsig) ;
           }
          else
            latchsig = ttv_addrefsig(ttvfig,tas_getsigname(cone,NULL,NULL),
                                       cone->NAME,tas_getcapai(cone),
                                     TTV_SIG_LL,latchsig) ;
         }
        else
          latchsig = ttv_addrefsig(ttvfig,tas_getsigname(cone,NULL,NULL),
                                   cone->NAME,tas_getcapai(cone),
                                   TTV_SIG_LF,latchsig) ;
        ptype = cone->USER = addptype(cone->USER,TAS_SIG_CONE,latchsig->DATA) ;
       }

     else if((cone->TYPE & TAS_PRECHARGE) == TAS_PRECHARGE)
       {
        presig = ttv_addrefsig(ttvfig,tas_getsigname(cone,NULL,NULL),
                               cone->NAME,tas_getcapai(cone),
                               TTV_SIG_R,presig) ;
        ptype = cone->USER = addptype(cone->USER,TAS_SIG_CONE,presig->DATA) ;
       }

     else if((cone->TYPE & TAS_INTER) == TAS_INTER)
       {
        intsig = ttv_addrefsig(ttvfig,tas_getsigname(cone,NULL,NULL),
                               cone->NAME,tas_getcapai(cone),
                               TTV_SIG_I,intsig) ;
        ptype = cone->USER = addptype(cone->USER,TAS_SIG_CONE,intsig->DATA) ;
       }

     else if((cone->TYPE & TAS_BREAK) == TAS_BREAK)
       {
        breaksig = ttv_addrefsig(ttvfig,tas_getsigname(cone,NULL,NULL),
                               cone->NAME,tas_getcapai(cone),
                               TTV_SIG_B,breaksig) ;
        ptype = cone->USER = addptype(cone->USER,TAS_SIG_CONE,breaksig->DATA) ;
       }

     else if((cone->TYPE & TAS_ECONE) == TAS_ECONE)
       {
        esig = ttv_addrefsig(ttvfig,tas_getsigname(cone,NULL,NULL),
                             cone->NAME,tas_getcapai(cone),
                             TTV_SIG_S,esig) ;
        ptype = cone->USER = addptype(cone->USER,TAS_SIG_CONE,esig->DATA) ;
       }

     else
       {
        ptsig = ttv_addsig(ttvfig,tas_getsigname(cone,NULL,NULL),
                           cone->NAME,tas_getcapai(cone),
                           TTV_SIG_S) ;
        ptype = cone->USER = addptype(cone->USER,TAS_SIG_CONE,ptsig) ;
       }

     if((cone->TYPE & TAS_BYPASSOUT) == TAS_BYPASSOUT)
        {
         ptsig = ptype->DATA ;
         ptsig->TYPE |= TTV_SIG_BYPASSOUT ;
        }

     if((cone->TYPE & TAS_BYPASSIN) == TAS_BYPASSIN)
        {
         ptsig = ptype->DATA ;
         ptsig->TYPE |= TTV_SIG_BYPASSIN ;
        }
    }

/*****************************************************************************/
/* recherche des chemins qui aboutissent a un connecteur                     */
/*****************************************************************************/

 for(connect = ptfig->LOCON ; connect != NULL ; connect = connect->NEXT)
    {
     if((connect->DIRECTION == 'O') || (connect->DIRECTION == 'B') ||
        (connect->DIRECTION == 'Z') || (connect->DIRECTION == 'T'))
        {
         if((ptype = getptype(connect->USER,CNS_EXT)) == NULL)
             continue ;

         cone = (cone_list *)ptype->DATA ;
         tas_loconpathmarque(cone,TAS_EXT_IN) ;
         if((connect->DIRECTION == 'B') || (connect->DIRECTION == 'T'))
          {
           tas_loconpathmarque(cone,TAS_EXT_OUT) ;
            ptype = getptype(connect->USER,CNS_CONE) ;
            if(ptype != NULL)
             {
              for(chain = ptype->DATA ; chain != NULL ; chain = chain->NEXT)
                if((cone_list *)chain->DATA != cone)
                  tas_loconpathmarque((cone_list *)chain->DATA,TAS_EXT_OUT) ;
              }
          }
        }
      else
        {
         ptype = getptype(connect->USER,CNS_EXT) ;
         if(ptype != NULL)
           {
            cone = (cone_list *)ptype->DATA ;
            tas_loconpathmarque(cone,TAS_EXT_OUT) ;
           }
           ptype = getptype(connect->USER,CNS_CONE) ;
           if(ptype != NULL)
             {
              for(chain = ptype->DATA ; chain != NULL ; chain = chain->NEXT)
               if((cone_list *)chain->DATA != cone)
                tas_loconpathmarque((cone_list *)chain->DATA,TAS_EXT_OUT) ;
             }
        }

    }

/*****************************************************************************/
/* ajoue des liens entre les noeuds.                                         */
/*****************************************************************************/
 
 for(cone = ptfig->CONE ; cone != NULL ; cone = cone->NEXT)
    {
     edge_list * in ;
     ptype_list *ptypeout = getptype(cone->USER,TAS_SIG_CONE) ;
     ttvsig_list *outsig ;

     if(ptypeout == NULL) continue ;
 
     outsig = ptypeout->DATA ;
     assigngatetype(cone, outsig);
       
     for(in = cone->INCONE ; in != NULL ; in = in->NEXT)
      {
       if(((in->TYPE & (CNS_FEEDBACK | CNS_VDD | CNS_VSS | CNS_BLEEDER | TAS_PATH_INOUT)) == 0) ||
           ((in->TYPE & (CNS_COMMAND | CNS_VDD | CNS_VSS | CNS_BLEEDER | TAS_PATH_INOUT)) == CNS_COMMAND))
          {
           ttvsig_list *insig ;
           ttvsig_list *conesig ;
           ptype_list *ptypein ;

           ptypein = getptype(in->USER,TAS_SIG_INCONE) ;
           if(ptypein == NULL)
            {
             if((in->TYPE & CNS_EXT) != CNS_EXT)
               ptypein = getptype(in->UEDGE.CONE->USER,TAS_SIG_CONE) ;
             else
               ptypein = getptype(in->UEDGE.LOCON->USER,TAS_SIG_CONE) ;
             conesig = NULL ;
            }
           else
            {
             if((in->TYPE & CNS_EXT) != CNS_EXT)
               ptype = getptype(in->UEDGE.CONE->USER,TAS_SIG_CONE) ;
             else
              {
               ptype = getptype(in->UEDGE.LOCON->USER,TAS_SIG_CONE) ;
               if((in->UEDGE.LOCON->DIRECTION == CNS_O) || 
                  (in->UEDGE.LOCON->DIRECTION == CNS_B) ||
                  (in->UEDGE.LOCON->DIRECTION == CNS_Z) || 
                  (in->UEDGE.LOCON->DIRECTION == CNS_T))
                {
                 ptype = getptype(in->UEDGE.LOCON->USER,CNS_EXT) ;
                 ptype = getptype(((cone_list*)ptype->DATA)->USER,
                                   TAS_SIG_CONE) ;
                }
              }
             if(ptype != NULL)
              conesig = (ttvsig_list *)ptype->DATA ;
             else
              {
               /*ptypein = NULL ;*/
               conesig = NULL ;
              }
            }
           if(ptypein == NULL) continue ;
           insig = ptypein->DATA ;
           if(conesig != NULL)
            {
             if(((insig->TYPE & (TTV_SIG_C|TTV_SIG_N)) == 0) ||
              ((conesig->TYPE & (TTV_SIG_C|TTV_SIG_N)) == 0))
                conesig = NULL ;
            }
           if(((insig->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0) || 
              ((outsig->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0))
           {
             chain_list *siglist=NULL;
             if (conesig!=NULL)
             {
               siglist=addchain(siglist, conesig);
               if((in->TYPE & CNS_EXT) != CNS_EXT)
                 if((in->UEDGE.CONE->TYPE & CNS_EXT)  == CNS_EXT && (conesig->TYPE & TTV_SIG_C)!=0)
                 {
                   chain_list *cl;
                   locon_list *lc;
                   cl=cns_get_cone_external_connectors(in->UEDGE.CONE);
                   while (cl!=NULL)
                   {
                     lc=(locon_list *)cl->DATA;
                     ptypein = getptype(lc->USER,TAS_LOCON_SIG) ;
                     if (ptypein!=NULL && ptypein->DATA!=conesig)
                       siglist=addchain(siglist, ptypein->DATA);
                     cl=delchain(cl,cl);
                   }
                 }
             }
              tas_linkconenode(ttvfig,insig,outsig,siglist,cone,in,TTV_LINE_D) ;
           }
           if((TAS_CONTEXT->TAS_MERGERCN == 'N') && (TAS_CONTEXT->TAS_CALCRCX == 'Y'))
            {
             if((in->TYPE & CNS_EXT) != CNS_EXT)
              {
               if((in->UEDGE.CONE->TYPE & CNS_EXT)  == CNS_EXT)
                {
                 locon_list *lc;
                 lc=cns_get_one_cone_external_connector(in->UEDGE.CONE);
                 if(lc!=NULL/*(ptype = getptype(in->UEDGE.CONE->USER,CNS_EXT)) != NULL*/)
                   {
                    if(lc->DIRECTION == CNS_T)
                      tas_linklocontcone(ttvfig,lc,in) ;
                   }
                }
              }
             else
              {
               if(in->UEDGE.LOCON->DIRECTION == CNS_T)
                 tas_linklocontcone(ttvfig,in->UEDGE.LOCON,in) ;
              }
            }
          }
      }
    }

  for(cone = ptfig->CONE ; cone != NULL ; cone = cone->NEXT)
    {
     edge_list * in ;
     ptype_list *ptypeout = getptype(cone->USER,TAS_SIG_CONE) ;
     ttvsig_list *outsig ;

     if(ptypeout == NULL) continue ;

     outsig = ptypeout->DATA ;

     for(in = cone->INCONE ; in != NULL ; in = in->NEXT)
      {
       if(((in->TYPE & (CNS_FEEDBACK | CNS_VDD | CNS_VSS | CNS_BLEEDER | TAS_PATH_INOUT | TAS_EXT_IN | TAS_EXT_OUT)) == 0) ||
           ((in->TYPE & (CNS_COMMAND | CNS_VDD | CNS_VSS | CNS_BLEEDER | TAS_PATH_INOUT | TAS_EXT_IN | TAS_EXT_OUT)) == CNS_COMMAND))
          {
           ttvsig_list *insig ;
           ptype_list *ptypein ;
           ttvsig_list *conesig ;

           ptypein = getptype(in->USER,TAS_SIG_INCONE) ;
           if(ptypein == NULL)
            {
             if((in->TYPE & CNS_EXT) != CNS_EXT)
               ptypein = getptype(in->UEDGE.CONE->USER,TAS_SIG_CONE) ;
             else
               ptypein = getptype(in->UEDGE.LOCON->USER,TAS_SIG_CONE) ;
             conesig = NULL ;
            }
           else
            {
             if((in->TYPE & CNS_EXT) != CNS_EXT)
               ptype = getptype(in->UEDGE.CONE->USER,TAS_SIG_CONE) ;
             else
              {
               ptype = getptype(in->UEDGE.LOCON->USER,TAS_SIG_CONE) ;
               if((in->UEDGE.LOCON->DIRECTION == CNS_O) || 
                  (in->UEDGE.LOCON->DIRECTION == CNS_B) ||
                  (in->UEDGE.LOCON->DIRECTION == CNS_Z) || 
                  (in->UEDGE.LOCON->DIRECTION == CNS_T))
                {
                 ptype = getptype(in->UEDGE.LOCON->USER,CNS_EXT) ;
                 ptype = getptype(((cone_list*)ptype->DATA)->USER,
                                   TAS_SIG_CONE) ;
                }
              }
             if(ptype != NULL)
              conesig = (ttvsig_list *)ptype->DATA ;
             else
              {
               /*ptypein = NULL ;*/
               conesig = NULL ;
              }
            }
           if(ptypein == NULL) continue ;
           insig = ptypein->DATA ;
           if(((insig->TYPE & (TTV_SIG_C|TTV_SIG_N)) == 0) &&
              ((outsig->TYPE & (TTV_SIG_C|TTV_SIG_N)) == 0))
           tas_linkconenode(ttvfig,insig,outsig,conesig==NULL?NULL:addchain(NULL,conesig),cone,in,TTV_LINE_F) ;
           else if((conesig != NULL) && 
                   ((in->TYPE & TAS_NORC_INCONE) != TAS_NORC_INCONE))
            {
             if(((insig->TYPE & (TTV_SIG_C|TTV_SIG_N)) == 0) &&
                ((conesig->TYPE & (TTV_SIG_C|TTV_SIG_N)) == 0))
             tas_linkconenode(ttvfig,insig,NULL,conesig==NULL?NULL:addchain(NULL,conesig),cone,in,TTV_LINE_F) ;
            }
          }
      }
    }

 for(cone = ptfig->CONE ; cone != NULL ; cone = cone->NEXT)
    {
     edge_list * in ;
     ptype_list *ptypeout = getptype(cone->USER,TAS_SIG_CONE) ;
     ttvsig_list *outsig ;
     ptype_list *ptypein ;
     ptype_list *ptypeleakage;

     if(ptypeout == NULL) continue ;

     outsig = ptypeout->DATA ;
     if((ptypeleakage = getptype(cone->USER, TAS_LEAKAGE)))
       outsig->USER = addptype(outsig->USER, TAS_LEAKAGE, ptypeleakage->DATA);

     for(in = cone->INCONE ; in != NULL ; in = in->NEXT)
      {
       if((ptypein = getptype(in->USER,TAS_IN_RCIN)) != NULL)
        {
         freechain((chain_list *)ptypein->DATA) ;
         in->USER = delptype(in->USER,TAS_IN_RCIN) ;
        }
       if((((in->TYPE & (CNS_FEEDBACK | CNS_VDD | CNS_VSS | CNS_BLEEDER | TAS_PATH_INOUT)) == 0) || 
           ((in->TYPE & (CNS_COMMAND | CNS_VDD | CNS_VSS | CNS_BLEEDER | TAS_PATH_INOUT)) == CNS_COMMAND)) &&
          ((in->TYPE & (TAS_EXT_IN | TAS_EXT_OUT)) != 0))
          {
           ttvsig_list *insig ;
           ttvsig_list *conesig ;

           ptypein = getptype(in->USER,TAS_SIG_INCONE) ;
           if(ptypein == NULL)
            {
             if((in->TYPE & CNS_EXT) != CNS_EXT)
               ptypein = getptype(in->UEDGE.CONE->USER,TAS_SIG_CONE) ;
             else
               ptypein = getptype(in->UEDGE.LOCON->USER,TAS_SIG_CONE) ;
             conesig = NULL ;
            }
           else
            {
             if((in->TYPE & CNS_EXT) != CNS_EXT)
               ptype = getptype(in->UEDGE.CONE->USER,TAS_SIG_CONE) ;
             else
              {
               ptype = getptype(in->UEDGE.LOCON->USER,TAS_SIG_CONE) ;
               if((in->UEDGE.LOCON->DIRECTION == CNS_O) ||
                  (in->UEDGE.LOCON->DIRECTION == CNS_B) ||
                  (in->UEDGE.LOCON->DIRECTION == CNS_Z) ||
                  (in->UEDGE.LOCON->DIRECTION == CNS_T))
                {
                 ptype = getptype(in->UEDGE.LOCON->USER,CNS_EXT) ;
                 ptype = getptype(((cone_list*)ptype->DATA)->USER,
                                   TAS_SIG_CONE) ;
                }
              }
             if(ptype != NULL)
              conesig = (ttvsig_list *)ptype->DATA ;
             else
              {
               /*ptypein = NULL ;*/
               conesig = NULL ;
              }
            }
           if(ptypein == NULL) continue ;
           insig = ptypein->DATA ;
           if(((insig->TYPE & (TTV_SIG_C|TTV_SIG_N)) == 0) &&
              ((outsig->TYPE & (TTV_SIG_C|TTV_SIG_N)) == 0))
           tas_linkconenode(ttvfig,insig,outsig,conesig==NULL?NULL:addchain(NULL,conesig),cone,in,TTV_LINE_E) ;
           else if((conesig != NULL) &&
                   ((in->TYPE & TAS_NORC_INCONE) != TAS_NORC_INCONE))
            {
             if(((insig->TYPE & (TTV_SIG_C|TTV_SIG_N)) == 0) &&
                ((conesig->TYPE & (TTV_SIG_C|TTV_SIG_N)) == 0))
             tas_linkconenode(ttvfig,insig,NULL,conesig==NULL?NULL:addchain(NULL,conesig),cone,in,TTV_LINE_E) ;
            }
          }
      }
    }

 if((TAS_CONTEXT->TAS_MERGERCN == 'N') && (TAS_CONTEXT->TAS_CALCRCX == 'Y'))
   for(connect = ptfig->LOCON ; connect != NULL ; connect = connect->NEXT)
    {
     if((connect->DIRECTION == 'O') || (connect->DIRECTION == 'B') ||
        (connect->DIRECTION == 'Z') || (connect->DIRECTION == 'T'))
        {
         tas_linkconelocon(ttvfig,connect) ;
        }
    }

 ttvfig->STATUS |= TTV_STS_DTX ;

/*****************************************************************************/
/* creation de la liste des commandes de chaque latch.                       */
/*****************************************************************************/
 
 /*
 if(latchsig != NULL) */
  {
   for(cone = ptfig->CONE ; cone != NULL ; cone = cone->NEXT)
    {
     ttvline_list *ptlinecmd ;
     ttvevent_list *ptnodecmd ;
     ptype_list *ptypecmd ;
     ptype_list *ptypesigcmd ;
     chain_list *chaincmd ;

     if(((cone->TYPE & (CNS_LATCH|CNS_FLIP_FLOP)) == 0) ||
        ((cone->TYPE & (CNS_VDD|CNS_VSS)) != 0))
      continue ;
     ptsig = (ttvsig_list *)getptype(cone->USER,TAS_SIG_CONE)->DATA ;
     chaincmd = tas_getlatchcmdlist(cone) ;
     if(chaincmd != NULL)
      {
       if ((ptypecmd=getptype(ptsig->USER,TTV_SIG_CMD))!=NULL)
       {
         ptypesigcmd = ptypecmd;
         ptypecmd->DATA=append((chain_list *)ptypecmd->DATA, chaincmd);
       }
       else
         ptypesigcmd = ptsig->USER = 
           addptype(ptsig->USER,TTV_SIG_CMD,(void*)chaincmd) ; 
      }
     else ptypesigcmd = NULL ;
     for(chain = chaincmd ; chain != NULL ; chain = chain->NEXT)
      ((ttvevent_list *)chain->DATA)->TYPE |= TTV_NODE_CMDMARQUE ;
     for(j = 0 ; j < 2 ; j ++)
     for(ptlinecmd = ptsig->NODE[j].INLINE ; ptlinecmd != NULL ; 
         ptlinecmd = ptlinecmd->NEXT)
       {
        ptypecmd = getptype(ptlinecmd->USER,TTV_LINE_CMDMAX) ;
        if(ptypecmd != NULL)
         {
          if(ptypesigcmd == NULL)
           ptypesigcmd = ptsig->USER = addptype(ptsig->USER,TTV_SIG_CMD,NULL) ;
          ptnodecmd = ptypecmd->DATA ;
          if((ptnodecmd->TYPE & TTV_NODE_CMDMARQUE) != TTV_NODE_CMDMARQUE)
            {
             ptnodecmd->TYPE |= TTV_NODE_CMDMARQUE ;
             ptypesigcmd->DATA = addchain(ptypesigcmd->DATA,ptnodecmd) ;
            }
         }
        ptypecmd = getptype(ptlinecmd->USER,TTV_LINE_CMDMIN) ;
        if(ptypecmd != NULL)
         {
          if(ptypesigcmd == NULL)
           ptypesigcmd = ptsig->USER = addptype(ptsig->USER,TTV_SIG_CMD,NULL) ;
          ptnodecmd = ptypecmd->DATA ;
          if((ptnodecmd->TYPE & TTV_NODE_CMDMARQUE) != TTV_NODE_CMDMARQUE)
            {
             ptnodecmd->TYPE |= TTV_NODE_CMDMARQUE ;
             ptypesigcmd->DATA = addchain(ptypesigcmd->DATA,ptnodecmd) ;
            }
         }
       }

     if(((ptsig->TYPE & TTV_SIG_EXT) == TTV_SIG_EXT) && (ptypesigcmd != NULL))
       for(chain = (chain_list *)ptypesigcmd->DATA ; chain != NULL ; 
           chain = chain->NEXT)
        {
         ptnodecmd = ((ttvevent_list *)chain->DATA) ;
         ptnodecmd->ROOT->TYPE |= TTV_SIG_EXT ;
        }

     for(j = 0 ; j < 2 ; j ++)
     for(ptlinecmd = ptsig->NODE[j].INLINE ; ptlinecmd != NULL ; 
         ptlinecmd = ptlinecmd->NEXT)
       {
        ptypecmd = getptype(ptlinecmd->USER,TTV_LINE_CMDMAX) ;
        if(ptypecmd != NULL)
         {
          ptnodecmd = ((ttvevent_list *)ptypecmd->DATA) ;
          ptnodecmd->TYPE &= ~(TTV_NODE_CMDMARQUE) ;
         }
        ptypecmd = getptype(ptlinecmd->USER,TTV_LINE_CMDMIN) ;
        if(ptypecmd != NULL)
         {
          ptnodecmd = ((ttvevent_list *)ptypecmd->DATA) ;
          ptnodecmd->TYPE &= ~(TTV_NODE_CMDMARQUE) ;
         }
       }
     for(chain = chaincmd ; chain != NULL ;
         chain = chain->NEXT)
      ((ttvevent_list *)chain->DATA)->TYPE &= ~(TTV_NODE_CMDMARQUE) ;
    }
  }

 ttvfig->CONSIG = (ttvsig_list **)consig ;
 ttvfig->NCSIG = (ttvsig_list **)append((chain_list *)ttvfig->NCSIG,ncsig) ;
 ttvfig->ESIG  = (ttvsig_list **)append((chain_list *)ttvfig->ESIG,esig) ;
 ttvfig->ELCMDSIG = (ttvsig_list **)append((chain_list *)ttvfig->ELCMDSIG,
                                           lcmdsig) ;
 ttvfig->ELATCHSIG = (ttvsig_list **)append((chain_list *)ttvfig->ELATCHSIG,
                                           latchsig) ;
 ttvfig->EPRESIG = (ttvsig_list **)append((chain_list *)ttvfig->EPRESIG,
                                           presig) ;
 ttvfig->EBREAKSIG = (ttvsig_list **)append((chain_list *)ttvfig->EBREAKSIG,
                                           breaksig) ;
 ttvfig->EXTSIG = (ttvsig_list **)append((chain_list *)ttvfig->EXTSIG,intsig) ;

/* if(nb_loop != 0)
  {
   tas_printloop(nb_loop,list_loop,ptfig->NAME) ;
   for(chain=(chain_list *)list_loop;chain;chain=chain->NEXT)
    freechain((chain_list*)chain->DATA) ;
   freechain(list_loop) ;
  }*/

 if(TAS_HTAB_NAME != NULL)
  {
   delht(TAS_HTAB_NAME) ;
   TAS_HTAB_NAME = NULL ;
  }

 if(lofig->LOINS != NULL)
  {
   tas_calcrcxdelay(lofig,ttvfig,TTV_FILE_DTX) ;
   tas_builtline(ttvfig,TTV_LINE_D) ;
  }

 tas_cleanfig(lofig,ttvfig) ;

 typefile = TTV_FILE_DTX ;
 ttvfig->STATUS |= TTV_STS_DTX ;
 tas_detectloop(ttvfig,TTV_FILE_DTX) ;
 if(lofig->LOINS != NULL)
  {
   delete = tas_deleteflatemptyfig(ttvfig,typefile,'Y') ;
   if(TAS_CONTEXT->TAS_CARACMODE == TAS_LUT_INT || TAS_CONTEXT->TAS_CARACMODE == TAS_LUT_CPL)
    tas_transformFCTmodels(ttvfig);
//   tas_detectloop(ttvfig,TTV_FILE_DTX) ;
  }
 else 
  delete = 0 ;

 ttv_builtrefsig(ttvfig) ;

 if(delete != 0)
   {
    ttv_setttvlevel(ttvfig) ;
    ttv_setsigttvfiglevel(ttvfig) ;
   }

 ttv_getinffile(ttvfig);

 if(lofig->LOINS != NULL || V_BOOL_TAB[__TAS_RECOMPUTE_DELAYS].VALUE)
 {
   type=0;
   if (V_BOOL_TAB[__TAS_RECOMPUTE_DELAYS].VALUE && V_BOOL_TAB[__TAS_DELAY_PROP].VALUE) type|=TTV_FIND_DELAY;
   type|=TTV_FIND_MAX;
   type|=(TAS_CONTEXT->TAS_FIND_MIN == 'Y') ? TTV_FIND_MIN:0;
   tas_calcfigdelay(ttvfig,lofig,type,typefile) ;
 }

 for(cone = ptfig->CONE ; cone != NULL ; cone = cone->NEXT)
  {
    if(getptype(cone->USER,TAS_SIG_CONE) != NULL)
       cone->USER = delptype(cone->USER,TAS_SIG_CONE) ;
    if((TAS_CONTEXT->TAS_MERGERCN == 'N') && (TAS_CONTEXT->TAS_CALCRCX == 'Y'))
     {
      for(incone = cone->INCONE ; incone != NULL ; incone = incone->NEXT)
       if(getptype(incone->USER,TAS_SIG_INCONE) != NULL)
          incone->USER = delptype(incone->USER,TAS_SIG_INCONE) ;
     }
  }

 for(j = 0 ; j < 2 ; j++)
 for(connect = tabcon[j] ; connect != NULL ; connect = connect->NEXT)
   {
    if(getptype(connect->USER,TAS_SIG_CONE) != NULL)
       connect->USER = delptype(connect->USER,TAS_SIG_CONE) ;
    if((ptype=getptype(connect->USER,TAS_LOCON_INLIST)) != NULL)
    {
       freechain((chain_list *)ptype->DATA);
       connect->USER = delptype(connect->USER,TAS_LOCON_INLIST) ;
    }
    
    if(getptype(connect->USER,TAS_LOCON_OUTCONE) != NULL)
       connect->USER = delptype(connect->USER,TAS_LOCON_OUTCONE) ;
   }
 ttv_addsig_addrcxpnode(oldmode);

 mbk_comcheck( 0, 0, ttv_signtimingfigure(ttvfig) );
 return(ttvfig) ;
}

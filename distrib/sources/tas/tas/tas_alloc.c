/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TAS Version 5                                               */
/*    Fichier : tas_alloc.c                                                 */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/
/* allocation des structures de tas                                         */
/****************************************************************************/

#include "tas.h"

chain_list *HEAD_DELAY = NULL ;
chain_list *HEAD_FRONT = NULL ;
chain_list *HEAD_CARACCON = NULL ;
chain_list *HEAD_CARACLINK = NULL ;

/*****************************************************************************/
/*                        function tas_initcnsfigalloc()                     */
/* fonction d'allocation des structures de donner que l'on ajoute a la cns   */
/* pour enregistrer les information de timing.                               */
/* Elles sont supprimees pour reccupperer la memoire des que l'on passe a la */
/* figure ttv.                                                               */
/* Pour chaque structure on compte le nombre necessaire et alloue la memoire */
/* en une seul fois. Puis on initialise tout les champs avec la valeur null  */
/* de chaque type de champs                                                  */
/*****************************************************************************/
void tas_initcnsfigalloc(cnsfig)
cnsfig_list *cnsfig ;
{
 locon_list *locon ;
 locon_list *tabcon[2] ;
 cone_list *cone ;
 edge_list *incone ;
 branch_list *branch[3];
 link_list *link ;
 caraclink_list *caraclink ;
 delay_list *delay ;
 front_list *front ;
 caraccon_list *caraccon ;
 long nbcaraccon = 0 ;
 long nbfront = 0 ;
 long nbdelay = 0 ;
 long nbcaraclink = 0 ;
 int i ;
 int j ;
 
 tabcon[0] = cnsfig->LOCON ;
 tabcon[1] = cnsfig->INTCON ;

 for(j = 0 ; j < 2 ; j++)
 for(locon = tabcon[j] ; locon != NULL ; locon = locon->NEXT)
   {
    if(nbcaraccon == (long)0)
     {
      caraccon = (caraccon_list*)mbkalloc(TAS_BLOC_SIZE*sizeof(caraccon_list)) ;
      HEAD_CARACCON = addchain(HEAD_CARACCON,(void*)caraccon) ;
      for(nbcaraccon = 0 ; nbcaraccon < TAS_BLOC_SIZE ; nbcaraccon++)
        {
         (caraccon + nbcaraccon)->C = TAS_NOCAPA ;
         (caraccon + nbcaraccon)->RUPMAX = TAS_NORES ;
         (caraccon + nbcaraccon)->RDOWNMAX = TAS_NORES ;
         (caraccon + nbcaraccon)->RUPMIN = TAS_NORES ;
         (caraccon + nbcaraccon)->RDOWNMIN = TAS_NORES ;
         (caraccon + nbcaraccon)->SLOPEIN = NULL ;
         (caraccon + nbcaraccon)->NSLOPE = 0 ;
         (caraccon + nbcaraccon)->CAPAOUT = NULL ;
         (caraccon + nbcaraccon)->NCAPA = 0 ;
        }
     }
    nbcaraccon -- ;
    locon->USER = 
    addptype(locon->USER,TAS_CON_CARAC,caraccon + nbcaraccon);
    if((locon->DIRECTION == CNS_I) || (locon->DIRECTION == CNS_T) || getptype(locon->USER, CNS_EXT) == NULL)
     {
      if(nbfront == (long)0)
       {
        front = (front_list*)mbkalloc(TAS_BLOC_SIZE*sizeof(front_list)) ;
        HEAD_FRONT = addchain(HEAD_FRONT,(void*)front) ;
        for(nbfront = 0 ; nbfront < TAS_BLOC_SIZE ; nbfront++)
          {
           (front + nbfront)->FUP = TAS_NOFRONT ;
           (front + nbfront)->FDOWN = TAS_NOFRONT ;
           (front + nbfront)->PWLUP = NULL ;
           (front + nbfront)->PWLDN = NULL ;
           (front + nbfront)->DRIVERUP.R = -1.0 ;
           (front + nbfront)->DRIVERUP.V = -1.0 ;
           (front + nbfront)->DRIVERDN.R = -1.0 ;
           (front + nbfront)->DRIVERDN.V = -1.0 ;
          }
       }
      nbfront -=2 ;
      locon->USER = 
      addptype(locon->USER,TAS_SLOPE_MIN,front + nbfront);
      locon->USER = 
      addptype(locon->USER,TAS_SLOPE_MAX,front + nbfront + 1);
     }
    if((locon->DIRECTION == CNS_O) || (locon->DIRECTION == CNS_B) ||
        (locon->DIRECTION == CNS_T) || (locon->DIRECTION == CNS_Z))
       {
        if(nbdelay == (long)0)
         {
          delay = (delay_list*)mbkalloc(TAS_BLOC_SIZE*sizeof(delay_list)) ;
          HEAD_DELAY = addchain(HEAD_DELAY,(void*)delay) ;
          for(nbdelay = 0 ; nbdelay < TAS_BLOC_SIZE ; nbdelay++)
            {
             (delay + nbdelay)->TPLH = TAS_NOTIME ;
             (delay + nbdelay)->TPHL = TAS_NOTIME ;
             (delay + nbdelay)->TPHH = TAS_NOTIME ;
             (delay + nbdelay)->TPLL = TAS_NOTIME ;
             (delay + nbdelay)->FLH = TAS_NOFRONT ;
             (delay + nbdelay)->FHL = TAS_NOFRONT ;
             (delay + nbdelay)->FHH = TAS_NOFRONT ;
             (delay + nbdelay)->FLL = TAS_NOFRONT ;
#ifdef USEOLDTEMP             
             (delay + nbdelay)->RLH = TAS_NORES ;
             (delay + nbdelay)->RHL = TAS_NORES ;
             (delay + nbdelay)->RHH = TAS_NORES ;
             (delay + nbdelay)->RLL = TAS_NORES ;
             (delay + nbdelay)->SLH = TAS_NOS ;
             (delay + nbdelay)->SHL = TAS_NOS ;
             (delay + nbdelay)->SHH = TAS_NOS ;
             (delay + nbdelay)->SLL = TAS_NOS ;
#endif
             (delay + nbdelay)->RCHH = TAS_NOTIME ;
             (delay + nbdelay)->RCLL = TAS_NOTIME ;
             (delay + nbdelay)->FRCHH = TAS_NOFRONT ;
             (delay + nbdelay)->FRCLL = TAS_NOFRONT ;
             (delay + nbdelay)->TMLH = NULL ;
             (delay + nbdelay)->TMHL = NULL ;
             (delay + nbdelay)->TMHH = NULL ;
             (delay + nbdelay)->TMLL = NULL ;
             (delay + nbdelay)->FMLH = NULL ;
             (delay + nbdelay)->FMHL = NULL ;
             (delay + nbdelay)->FMHH = NULL ;
             (delay + nbdelay)->FMLL = NULL ;
#ifdef USEOLDTEMP             
             (delay + nbdelay)->PWLTPLH = NULL ;
             (delay + nbdelay)->PWLTPHL = NULL ;
             (delay + nbdelay)->PWLRCLL = NULL ;
             (delay + nbdelay)->PWLRCHH = NULL ;
#endif
             (delay + nbdelay)->CARAC = NULL ;
            }
         }
        nbdelay -=2 ;
        locon->USER =
        addptype(locon->USER,TAS_DELAY_MIN,delay + nbdelay);
        locon->USER =
        addptype(locon->USER,TAS_DELAY_MAX,delay + nbdelay + 1);
       }
   }

 for(cone = cnsfig->CONE ; cone != NULL ; cone = cone->NEXT)
   {
    cone->TYPE &= TAS_CONE_MASK ;
    if(nbfront == (long)0)
     {
      front = (front_list*)mbkalloc(TAS_BLOC_SIZE*sizeof(front_list)) ;
      HEAD_FRONT = addchain(HEAD_FRONT,(void*)front) ;
      for(nbfront = 0 ; nbfront < TAS_BLOC_SIZE ; nbfront++)
        {
         (front + nbfront)->FUP = TAS_NOFRONT ;
         (front + nbfront)->FDOWN = TAS_NOFRONT ;
         (front + nbfront)->PWLUP = NULL ;
         (front + nbfront)->PWLDN = NULL ;
         (front + nbfront)->DRIVERUP.R = -1.0 ;
         (front + nbfront)->DRIVERUP.V = -1.0 ;
         (front + nbfront)->DRIVERDN.R = -1.0 ;
         (front + nbfront)->DRIVERDN.V = -1.0 ;
        }
     }
    nbfront -=2 ;
    cone->USER = 
    addptype(cone->USER,TAS_SLOPE_MIN,front + nbfront);
    cone->USER = 
    addptype(cone->USER,TAS_SLOPE_MAX,front + nbfront + 1);
    for(incone = cone->INCONE ; incone != NULL ; incone = incone->NEXT)
       {
        incone->TYPE &= (TAS_INCONE_MASK | TAS_IN_MEMSYM | TAS_IN_NOTMEMSYM) ;
        if(nbdelay == (long)0)
         {
          delay = (delay_list*)mbkalloc(TAS_BLOC_SIZE*sizeof(delay_list)) ;
          HEAD_DELAY = addchain(HEAD_DELAY,(void*)delay) ;
          for(nbdelay = 0 ; nbdelay < TAS_BLOC_SIZE ; nbdelay++)
            {
             (delay + nbdelay)->TPLH = TAS_NOTIME ;
             (delay + nbdelay)->TPHL = TAS_NOTIME ;
             (delay + nbdelay)->TPHH = TAS_NOTIME ;
             (delay + nbdelay)->TPLL = TAS_NOTIME ;
             (delay + nbdelay)->FLH = TAS_NOFRONT ;
             (delay + nbdelay)->FHL = TAS_NOFRONT ;
             (delay + nbdelay)->FHH = TAS_NOFRONT ;
             (delay + nbdelay)->FLL = TAS_NOFRONT ;
#ifdef USEOLDTEMP
             (delay + nbdelay)->RLH = TAS_NORES ;
             (delay + nbdelay)->RHL = TAS_NORES ;
             (delay + nbdelay)->RHH = TAS_NORES ;
             (delay + nbdelay)->RLL = TAS_NORES ;
             (delay + nbdelay)->SLH = TAS_NOS ;
             (delay + nbdelay)->SHL = TAS_NOS ;
             (delay + nbdelay)->SHH = TAS_NOS ;
             (delay + nbdelay)->SLL = TAS_NOS ;
#endif
             (delay + nbdelay)->RCHH = TAS_NOTIME ;
             (delay + nbdelay)->RCLL = TAS_NOTIME ;
             (delay + nbdelay)->FRCHH = TAS_NOFRONT ;
             (delay + nbdelay)->FRCLL = TAS_NOFRONT ;
             (delay + nbdelay)->TMLH = NULL ;
             (delay + nbdelay)->TMHL = NULL ;
             (delay + nbdelay)->TMHH = NULL ;
             (delay + nbdelay)->TMLL = NULL ;
             (delay + nbdelay)->FMLH = NULL ;
             (delay + nbdelay)->FMHL = NULL ;
             (delay + nbdelay)->FMHH = NULL ;
             (delay + nbdelay)->FMLL = NULL ;
#ifdef USEOLDTEMP             
             (delay + nbdelay)->PWLTPLH = NULL ;
             (delay + nbdelay)->PWLTPHL = NULL ;
             (delay + nbdelay)->PWLRCHH = NULL ;
             (delay + nbdelay)->PWLRCLL = NULL ;
#endif
             (delay + nbdelay)->CARAC = NULL ;
            }
         }
        nbdelay -=2 ;
        incone->USER = 
        addptype(incone->USER,TAS_DELAY_MIN,delay + nbdelay);
        incone->USER = 
        addptype(incone->USER,TAS_DELAY_MAX,delay + nbdelay + 1);
       }
     branch[0]=cone->BREXT ;
     branch[1]=cone->BRVDD ;
     branch[2]=cone->BRVSS ;
     for(i = 0 ; i < 3 ; i++)
     for(; branch[i] != NULL ; branch[i] = branch[i]->NEXT)
       {
        branch[i]->USER = addptype(branch[i]->USER,TAS_RESIST,
                                   (void*)TAS_NORES) ;
        for(link = branch[i]->LINK ; link != NULL ; link=link->NEXT)
          {
           if(nbcaraclink == (long)0)
            {
             caraclink = (caraclink_list*)
                          mbkalloc(TAS_BLOC_SIZE*sizeof(caraclink_list)) ;
             HEAD_CARACLINK = addchain(HEAD_CARACLINK,(void*)caraclink) ;
             for(nbcaraclink = 0 ; nbcaraclink < TAS_BLOC_SIZE ; nbcaraclink++)
               {
                (caraclink + nbcaraclink)->WIDTH = TAS_NOWIDTH ;
                (caraclink + nbcaraclink)->LENGTH = TAS_NOLENGTH ;
                (caraclink + nbcaraclink)->CRAM = TAS_NOCAPA ;
                (caraclink + nbcaraclink)->CEQUI = TAS_NOCAPA ;
                (caraclink + nbcaraclink)->CLINK = TAS_NOCAPA ;
               }
            }
           nbcaraclink -- ;
           link->USER = addptype(link->USER,TAS_LINK_CARAC,
                                 caraclink + nbcaraclink);
          }
       }
    }
}

/*****************************************************************************/
/*                        function tas_freecnsfigalloc()                     */
/* fonction de liberation de la memoire des strutures de timing attachees a  */
/* la cnsfig et suppresion des ptype associes au champs USER.                */
/*****************************************************************************/
void tas_freecnsfigalloc(cnsfig)
cnsfig_list *cnsfig ;
{
 locon_list *locon ;
 locon_list *tabcon[2] ;
 cone_list *cone ;
 edge_list *incone ;
 branch_list *branch[3];
 link_list *link ;
 chain_list *chain ;
 ptype_list *ptype ;
 long i ;
 caraccon_list *caraccon ;
 long nbcaraccon = 0 ;
 int j ;
 front_list *front;
 delay_list *delay;

 tabcon[0] = cnsfig->LOCON ;
 tabcon[1] = cnsfig->INTCON ;

 for(j = 0 ; j < 2 ; j++)
 for(locon = tabcon[j] ; locon != NULL ; locon = locon->NEXT)
  {
   locon->USER = delptype(locon->USER,TAS_CON_CARAC) ;
   if(getptype(locon->USER,TAS_SLOPE_MAX) != NULL)
     locon->USER = delptype(locon->USER,TAS_SLOPE_MAX) ;
   if(getptype(locon->USER,TAS_SLOPE_MAX) != NULL)
     locon->USER = delptype(locon->USER,TAS_SLOPE_MIN) ;
   if(getptype(locon->USER,TAS_DELAY_MAX) != NULL)
     locon->USER = delptype(locon->USER,TAS_DELAY_MAX) ;
   if(getptype(locon->USER,TAS_DELAY_MIN) != NULL)
     locon->USER = delptype(locon->USER,TAS_DELAY_MIN) ;
   if ((ptype=getptype(locon->SIG->USER, ELP_CAPASWITCH))!=NULL)
   {
     mbkfree(ptype->DATA);
     locon->SIG->USER=delptype(locon->SIG->USER, ELP_CAPASWITCH);
   }
  }
 
 for(cone = cnsfig->CONE ; cone != NULL ; cone = cone->NEXT)
    {
     cone->USER = delptype(cone->USER,TAS_SLOPE_MAX) ;
     cone->USER = delptype(cone->USER,TAS_SLOPE_MIN) ;
     for(incone = cone->INCONE ; incone != NULL ; incone = incone->NEXT)
       {
        incone->USER = delptype(incone->USER,TAS_DELAY_MAX) ;
        incone->USER = delptype(incone->USER,TAS_DELAY_MIN) ;
       }
     branch[0]=cone->BREXT ;
     branch[1]=cone->BRVDD ;
     branch[2]=cone->BRVSS ;
     for(i = 0 ; i < 3 ; i++)
     for(; branch[i] != NULL ; branch[i] = branch[i]->NEXT)
       {
        branch[i]->USER = delptype(branch[i]->USER,TAS_RESIST) ;
        for(link = branch[i]->LINK ; link != NULL ; link=link->NEXT)
          {
           link->USER = delptype(link->USER,TAS_LINK_CARAC) ;
           ptype = getptype(link->USER, TAS_LINK_CAPA);
           if(ptype){
               mbkfree((void*)ptype->DATA);
               link->USER =  delptype(link->USER, TAS_LINK_CAPA);
           }
           ptype = getptype(link->USER, TAS_LINK_UNUSED_SWITCH_COMMAND);
           if(ptype){
               link->USER =  delptype(link->USER, TAS_LINK_UNUSED_SWITCH_COMMAND);
           }

          }
       }
    }
 for(chain = HEAD_DELAY ; chain != NULL ; chain = chain->NEXT) {
     delay = (delay_list*)chain->DATA ;
#ifdef USEOLDTEMP 
     for( i=0 ; i < TAS_BLOC_SIZE ; i++ ) {
       stm_pwl_destroy( delay[i].PWLTPLH );
       stm_pwl_destroy( delay[i].PWLTPHL );
       stm_pwl_destroy( delay[i].PWLRCLL );
       stm_pwl_destroy( delay[i].PWLRCHH );
     }
#endif
     mbkfree(delay) ;

 }

 for(chain = HEAD_FRONT ; chain != NULL ; chain = chain->NEXT) {
     front = (front_list *)chain->DATA;
     mbkfree(front) ;
 }

 stm_pwl_finish();

 for(chain = HEAD_CARACCON ; chain != NULL ; chain = chain->NEXT) {
     caraccon = (caraccon_list*)chain->DATA ;
     for(nbcaraccon = 0 ; nbcaraccon < TAS_BLOC_SIZE ; nbcaraccon++) {
         if ((caraccon + nbcaraccon)->SLOPEIN) {
             mbkfree ((caraccon + nbcaraccon)->SLOPEIN) ; 
			 (caraccon + nbcaraccon)->SLOPEIN = NULL ;
		 }
         if ((caraccon + nbcaraccon)->CAPAOUT) {
             mbkfree ((caraccon + nbcaraccon)->CAPAOUT) ; 
			 (caraccon + nbcaraccon)->CAPAOUT = NULL ;
		 }
	 }
     mbkfree((void *)chain->DATA) ;
 }

 for(chain = HEAD_CARACLINK ; chain != NULL ; chain = chain->NEXT)
     mbkfree((void *)chain->DATA) ;

 freechain(HEAD_DELAY) ; 
 HEAD_DELAY = NULL ;

 freechain(HEAD_FRONT) ; 
 HEAD_FRONT = NULL ;

 freechain(HEAD_CARACCON) ; 
 HEAD_CARACCON = NULL ;

 freechain(HEAD_CARACLINK) ; 
 HEAD_CARACLINK = NULL ;

}

/*****************************************************************************/
/*                        function tas_freecnsfig()                          */
/* fonction de liberartion de la cnsfig                                      */
/*****************************************************************************/
void tas_freelofig(lofig)
lofig_list *lofig ;
{
 loins_list *loins ;
 locon_list *locon ;
 lotrs_list *lotrs ;
 losig_list *losig ;
 ptype_list *ptype, *pt ;
 lofig_list *rlofig ;
 lofig_list *dlofig ;

 if(lofig == NULL) return ;

 rcx_delalllosigrcx( lofig );
 /*
 mbk_freeparallel(lofig);


 for(locon = lofig->LOCON ; locon != NULL ; locon = locon->NEXT)
   {
    delloconuser( locon );
   }

 for(loins = lofig->LOINS ; loins != NULL ; loins = loins->NEXT)
   {
    delloinsuser(loins);
    for(locon = loins->LOCON ; locon != NULL ; locon = locon->NEXT)
      {
       delloconuser( locon );
      }
   }
*/
 for(lotrs = lofig->LOTRS ; lotrs != NULL ; lotrs = lotrs->NEXT)
   {
    if ((pt=getptype(lotrs->USER, TAS_TRANS_LINK))!=NULL)
      freechain((chain_list *)pt->DATA);
/*    freelotrsparams(lotrs);
    dellotrsuser(lotrs); //freeptype(lotrs->USER) ;
    //lotrs->USER = NULL ;
    delloconuser( lotrs->DRAIN );
    delloconuser( lotrs->SOURCE );
    delloconuser( lotrs->GRID );
    if (lotrs->BULK!=NULL)
     {
      delloconuser( lotrs->BULK );
     } */
    }

/*
 for(losig = lofig->LOSIG ; losig != NULL ; losig = losig->NEXT)
   {
    dellosiguser(losig);
    losig->USER = NULL ;
   }

 rlofig = HEAD_LOFIG ;
 for( rlofig = HEAD_LOFIG; rlofig ; rlofig = dlofig ) {
       dlofig = rlofig->NEXT;
       if( rlofig == TAS_CONTEXT->TAS_HIERLOFIG ) continue;
       if( islofiglocked( rlofig ) ) continue;
       lofigchain( rlofig );

       rcx_delalllosigrcx( rlofig );
       elpFreeCapaLofig( rlofig );
       dellofig(rlofig->NAME) ;
 }
 */
 lofigchain( lofig );
 rcx_delalllosigrcx( lofig );
 elpFreeCapaLofig( lofig );
 dellofig(lofig->NAME);
}

/*****************************************************************************/
/*                        function tas_freecnsfig()                          */
/* fonction de liberartion de la cnsfig                                      */
/*****************************************************************************/
void tas_freecnsfig(cnsfig)
cnsfig_list *cnsfig ;
{
 locon_list *locon ;
 locon_list *tabcon[2] ;
 lotrs_list *lotrs ;
 cone_list *cone ;
 edge_list *incone ;
 edge_list *outcone ;
 branch_list *branch[3];
 link_list *link ;
 ptype_list *ptype ;
 chain_list *chain ;
 int i ;
 int j ;

 if(cnsfig == NULL) return ;

 tas_remove_disable_gate_delay(cnsfig);

 tabcon[0] = cnsfig->LOCON ;
 tabcon[1] = cnsfig->INTCON ;

 for(j = 0 ; j < 2 ; j++)
 for(locon = tabcon[j] ; locon != NULL ; locon = locon->NEXT)
  {
    ptype = getptype(locon->USER,CNS_CONE) ;
    if(ptype != NULL)
      freechain((chain_list *)ptype->DATA) ;
  }

 for(lotrs = cnsfig->LOTRS ; lotrs != NULL ; lotrs = lotrs->NEXT)
  {
    ptype = getptype(lotrs->USER,CNS_CONE) ;
    if(ptype != NULL)
      freechain((chain_list *)ptype->DATA) ;
  }

 for(cone = cnsfig->CONE ; cone != NULL ; cone = cone->NEXT)
  {
   ptype = getptype(cone->USER,TAS_RC_CONE) ;
   if(ptype != NULL)
    freechain((chain_list *)ptype->DATA) ;
   ptype = getptype(cone->USER,CNS_BLEEDER) ;
   if(ptype != NULL)
    freechain((chain_list *)ptype->DATA) ;
   ptype = getptype(cone->USER,CNS_SWITCH) ;
   if(ptype != NULL)
   {
    for(chain = (chain_list *)ptype->DATA ; chain != NULL ; chain = chain->NEXT)
    freechain((chain_list *)chain->DATA) ;
    freechain((chain_list *)ptype->DATA) ;
   }
   ptype = getptype(cone->USER,CNS_PARALLEL) ;
   if(ptype != NULL)
   {
    for(chain = (chain_list *)ptype->DATA ; chain != NULL ; chain = chain->NEXT)
    freechain((chain_list *)chain->DATA) ;
    freechain((chain_list *)ptype->DATA) ;
   }
   ptype = getptype(cone->USER,CNS_LOCON) ;
   if(ptype != NULL)
    freechain((chain_list *)ptype->DATA) ;
   ptype = getptype(cone->USER,CNS_LOTRSGRID) ;
   if(ptype != NULL)
    freechain((chain_list *)ptype->DATA) ;
   ptype = getptype(cone->USER,CNS_LOTRS) ;
   if(ptype != NULL)
    freechain((chain_list *)ptype->DATA) ;

   if ((ptype=getptype(cone->USER, CNS_UPEXPR))!=NULL) freeExpr((chain_list *)ptype->DATA);
   if ((ptype=getptype(cone->USER, CNS_DNEXPR))!=NULL) freeExpr((chain_list *)ptype->DATA);
   if ((ptype=getptype(cone->USER, CNS_PARATRANS))!=NULL) freechain((chain_list *)ptype->DATA);
   if ((ptype=getptype(cone->USER, CNS_EXT))!=NULL) freechain((chain_list *)ptype->DATA);

   freeptype(cone->USER) ;
   cone->USER = NULL ;
   for(outcone = cone->OUTCONE ; outcone != NULL ; outcone = outcone->NEXT)
    {
     freeptype(outcone->USER) ;
     outcone->USER = NULL ;
    }
   for(incone = cone->INCONE ; incone != NULL ; incone = incone->NEXT)
    {
     ptype = getptype(incone->USER,TAS_RC_LOCON) ;
     if(ptype != NULL)
      freechain((chain_list *)ptype->DATA) ;
     freeptype(incone->USER) ;
     incone->USER = NULL ;
    }
   branch[0]=cone->BREXT ;
   branch[1]=cone->BRVDD ;
   branch[2]=cone->BRVSS ;
   for(i = 0 ; i < 3 ; i++)
    for(; branch[i] != NULL ; branch[i] = branch[i]->NEXT)
     {
      freeptype(branch[i]->USER) ;
      branch[i]->USER = NULL ;
      for(link = branch[i]->LINK ; link != NULL ; link=link->NEXT)
       {
        freeptype(link->USER) ;
        link->USER = NULL ;
       }
     }
  }

CNS_HEADCNSFIG =  delcnsfig(CNS_HEADCNSFIG,cnsfig) ;
}

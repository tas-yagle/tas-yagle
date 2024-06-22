/***************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TAS Verison 5                                               */
/*    Fichier : tas_builthfig.c                                             */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/
/* contruction d'une ttvfig pere grace a la lofig et ses fils               */
/****************************************************************************/

#include "tas.h"

#include YAG_H
#include GEN_H

/*****************************************************************************/
/*                        function tas_getcapa()                             */
/* parametres :                                                              */
/* lofig : figure logique                                                    */
/* losig : signal logique                                                    */
/*                                                                           */
/* renvoie la capacite d'un signal                                           */
/*****************************************************************************/
float tas_getcapa(lofig,losig)
lofig_list *lofig ;
losig_list *losig ;
{
 ptype_list *ptype ;

 if((ptype = getptype(losig->USER,TAS_SIG_CAPA)) == NULL)
   return(rcx_getcapa(lofig,losig) * 1000.0) ;
 else
  {
   return ((*(float*)ptype->DATA +  rcx_getcapa(lofig,losig)) * 1000.0) ;
  }
}

/*****************************************************************************/
/*                        function tas_routelosig()                          */
/* parametres :                                                              */
/* lofig : figure logique                                                    */
/* losig : signal logique                                                    */
/* capa : capacite                                                           */
/* resi : resistance                                                         */
/*                                                                           */
/* ajoute de la capacite a un signal                                         */
/*****************************************************************************/
void tas_routelosig(lofig,losig,capa,resi)
lofig_list *lofig ;
losig_list *losig ;
float capa ;
float resi ;
{
  locon_list *locon = NULL ;
  ptype_list *ptype ;
  chain_list *chain ;
  long nbcon ;
  long i ;

  ptype = getptype(losig->USER,LOFIGCHAIN) ;

  if(losig->PRCN != NULL)
     freelorcnet(losig) ;
  addlorcnet(losig) ;

  for(chain = (chain_list *)ptype->DATA , nbcon = (long)0 ; chain != NULL ;
      chain = chain->NEXT, nbcon++)
   {
    locon = (locon_list *)chain->DATA ;
    setloconnode(locon,nbcon+(long)1) ;
   }

  for(chain = (chain_list *)ptype->DATA , i = 0 ; chain != NULL ; 
      chain = chain->NEXT , i++)
   {
    addlowire(losig,0,resi/(float)nbcon,capa/(float)nbcon,i+1,nbcon+1) ;
   }
#ifndef __ALL__WARNING__
 lofig = NULL;
#endif
}

/*****************************************************************************/
/*                        function tas_addcapalosig()                        */
/* parametres :                                                              */
/* lofig : figure logique                                                    */
/* losig : signal logique                                                    */
/* capa : capacite                                                           */
/* resi : resistance                                                         */
/*                                                                           */
/* ajoute de la capacite a un signal                                         */
/*****************************************************************************/
void tas_addcapalosig(lofig,losig,capa,resi)
lofig_list *lofig ;
losig_list *losig ;
float capa ;
float resi ;
{
 float *pt ;
 ptype_list *ptype ;

 if(resi > 1.0)
  {
   tas_routelosig(lofig,losig,capa,resi) ;
  }

 if((ptype = getptype(losig->USER,TAS_SIG_CAPA)) == NULL)
  {
   pt = (float*)mbkalloc (sizeof (float));
   *pt =  capa ;
   losig->USER = addptype(losig->USER,TAS_SIG_CAPA,pt) ;
  }
 else
  {
   *(float*)ptype->DATA +=  capa;
  }
#ifndef __ALL__WARNING__
 lofig = NULL;
#endif
}

/*****************************************************************************/
/*                        function tas_deleteflatemptyfig()                  */
/* parametres :                                                              */
/* ttvfig : figure                                                           */
/* type : type de fichier                                                    */
/*                                                                           */
/* efface les figure vide et les figure dans le catalog                      */
/*****************************************************************************/
int tas_deleteflatemptyfig(ttvfig,type,flat) 
ttvfig_list *ttvfig ;
long type ;
char flat ;
{
 ttvfig_list *ttvins ;
 chain_list *chain ;
 chain_list *chainnext ;
 chain_list *chainsav ;
 chain_list *chainfig ;
 chain_list *chainflat ;
 chain_list *chainsig ;
 ht *htabcell ;
 char flagempty ;
 char flagflat ;
 int nbins = 0 ;
 int res = 0 ;

 for(chain = ttvfig->INS ; chain != NULL ; chain = chain->NEXT)
   nbins++ ;

 chain = ttvfig->INS;

 if(nbins != 0)
   htabcell = addht(nbins) ;
 else return 0 ;

 chainfig = NULL ;
 chainflat = NULL ;

 while(chain != NULL)
  {
   chainnext = chain->NEXT ;
   ttvins = (ttvfig_list *)chain->DATA ;

   if(gethtitem(htabcell,ttvins->INFO->FIGNAME) == EMPTYHT)
    {
/*    if((ttvins->STATUS & TTV_STS_LOCK) != TTV_STS_LOCK)
     if(stm_getcell(ttvins->INFO->FIGNAME) != NULL)
       stm_freecell(ttvins->INFO->FIGNAME) ;*/
     addhtitem(htabcell,ttvins->INFO->FIGNAME,(long)NULL) ;
    }

   flagempty = 'N' ;
   flagflat = 'N' ;

   if(ttv_isemptyttvins(ttvins,type) == 1)
    {
     flagempty = 'Y' ;
    }
   else if((flat == 'Y') || ((inlibcatalog(ttvins->INFO->FIGNAME) == 1) && 
            (TAS_CONTEXT->TAS_FLATCELLS == 'Y')))
    {
     flagflat = 'Y' ;
    }

   if((flagempty == 'Y') || (flagflat == 'Y'))
    {
     if(chain == ttvfig->INS)
       {
        ttvfig->INS = ttvfig->INS->NEXT ;
        chain->NEXT = NULL ;
        freechain(chain) ;
       }
     else
       {
        chainsav->NEXT = chain->NEXT ;
        chain->NEXT = NULL ;
        freechain(chain) ;
        chain = chainsav ;
       }
     if(flagempty == 'Y')
       chainfig = addchain(chainfig,ttvins) ; 
     else if(flagflat == 'Y')
       chainflat = addchain(chainflat,ttvins) ; 
    }
   else
     ttv_freettvfigmemory(ttvins,TTV_STS_C|TTV_STS_D|TTV_STS_T) ;

   chainsav = chain ;
   chain = chainnext ;
  }

 if(chainflat != NULL)
  {
   ttv_flatttvfigfromlist(ttvfig,chainflat,type) ;
   chainfig = append(chainfig,chainflat) ;
  }

 for(chain = chainfig ; chain != NULL ; chain = chain->NEXT)
   {
    ttvins = (ttvfig_list *)chain->DATA ;
    ttvins->ROOT = NULL ;
    ttvins->INSNAME =  ttvins->INFO->FIGNAME ;
    ttvins->STATUS |= TTV_STS_HEAD ;  
    TTV_HEAD_TTVFIG = addchain(TTV_HEAD_TTVFIG,ttvins) ;
    chainnext = ttv_getsigbytype((ttvfig_list *)chain->DATA,NULL,TTV_SIG_Q,NULL) ;
    for(chainsig = chainnext ; chainsig != NULL ; chainsig = chainsig->NEXT)
      {
       ttv_delcmd((ttvfig_list *)chain->DATA,(ttvsig_list *)chainsig->DATA) ;
      }
    freechain(chainnext) ;
   }

 if(chainfig != NULL)
  {
   ttv_freettvfiglist(chainfig) ;
   freechain(chainfig) ;
   res = 1 ;
  }

 if(nbins != 0)
   delht(htabcell) ;

 return(res) ;
}

/*****************************************************************************/
/*                        function tas_detectloop()                          */
/* parametres :                                                              */
/* ttvfig : figure                                                           */
/*                                                                           */
/* recherche les boucles et detect les signaux externes                      */
/*****************************************************************************/
void tas_detectloop(ttvfig,type)
ttvfig_list *ttvfig ;
long type ;
{
 chain_list *chain ;
 chain_list *chainloop ;
 int nbloop ;

 if((type & TTV_FILE_DTX) == TTV_FILE_DTX)
  {
   chainloop = ttv_detectloop(ttvfig,TTV_FIND_LINE) ;
   if(((type & TTV_FILE_TTX) == TTV_FILE_TTX) && (chainloop != NULL))
    {
     chain_list *chainx ;

     ttv_cleantagttvfig(TTV_STS_FREE_MASK) ;
     chain = ttv_detectloop(ttvfig,TTV_FIND_PATH) ;
     for(chainx = chain ; chainx != NULL ; chainx = chainx->NEXT)
       freechain((chain_list *)chainx->DATA) ;
     freechain(chain) ;
    }
  }
 else if((type & TTV_FILE_TTX) == TTV_FILE_TTX)
  {
   chainloop = ttv_detectloop(ttvfig,TTV_FIND_PATH) ;
  }

 if(chainloop != NULL)
  {
   nbloop = 0 ;
   chainloop = ttv_addlooplist(ttvfig,chainloop) ; 
   for(chain = chainloop ; chain != NULL ; chain = chain->NEXT)
      nbloop++ ;
   tas_printloop(nbloop,chainloop,ttvfig) ;
   for(chain = chainloop ; chain != NULL ; chain = chain->NEXT)
    {
     freechain((chain_list *)chain->DATA) ;
    }
   freechain(chainloop) ;
  }

 ttv_cleantagttvfig(TTV_STS_FREE_MASK) ;
}

/*****************************************************************************/
/*                        function tas_calcfigdelay()                        */
/* parametres :                                                              */
/* ttvfig : figure                                                           */
/* lofig  : lofig                                                            */
/* type : type de d'arc                                                      */
/*                                                                           */
/* calcul les delai                                                          */
/*****************************************************************************/
void tas_calcfigdelay(ttvfig,lofig,type,typefile)
ttvfig_list *ttvfig ;
lofig_list *lofig ;
long type ;
long typefile ;
{
 chain_list *chain = NULL ;
 long maxsig ;
 
 maxsig =  TTV_MAX_SIG ;
 TTV_MAX_SIG = TTV_ALLOC_MAX ;

 if (getptype(ttvfig->INFO->USER,TTV_FIG_LOFIG)==NULL)
    ttvfig->INFO->USER = addptype(ttvfig->INFO->USER,TTV_FIG_LOFIG,lofig) ;
 locklofig(lofig) ;

 if((typefile & TTV_FILE_DTX) == TTV_FILE_DTX)
   {
#ifdef DELAY_DEBUG_STAT
    fprintf(stderr,"dtx:");
#endif
    chain = ttv_calcfigdelay(ttvfig,chain,ttvfig->INFO->LEVEL,
                           TTV_FIND_LINE|TTV_FIND_LINETOP|type,TTV_MODE_LINE,NULL,NULL,NULL,0) ;
   }
 if((typefile & TTV_FILE_TTX) == TTV_FILE_TTX)
   {
#ifdef DELAY_DEBUG_STAT
    fprintf(stderr,"ttx:");
#endif
    chain = ttv_calcfigdelay(ttvfig,chain,ttvfig->INFO->LEVEL,
                           TTV_FIND_PATH|TTV_FIND_LINETOP|type,TTV_MODE_LINE,NULL,NULL,NULL,0) ;
   }

 ttv_update_slope_and_output_capa(ttvfig);
 ttv_freefigdelay(chain) ;

 freechain(chain) ;

 TTV_MAX_SIG = maxsig ;
#ifdef DELAY_DEBUG_STAT
 mbk_debugstat("free figdelay:", 0);
#endif
}

/*****************************************************************************/
/*                        function tas_lofigchain()                          */
/* parametres :                                                              */
/* lofig : figure logique                                                    */
/*                                                                           */
/* cree un lofigchain propre tas hierarchique                                */
/*****************************************************************************/
void tas_lofigchain(lofig)
lofig_list *lofig ;
{
 losig_list *losig ;
 ptype_list *ptype, *insuser ;
 ptype_list *ptypex ;
 chain_list *chain ;
 locon_list *lc;
 
 for(losig = lofig->LOSIG ; losig != NULL ; losig = losig->NEXT)
   {
    if((ptype = getptype(losig->USER,LOFIGCHAIN)) != NULL)
     {
      losig->USER = addptype(losig->USER,TAS_LOFIGCHAIN,NULL) ;
      ptypex = losig->USER ;
      for(chain = (chain_list *)ptype->DATA ; chain !=NULL ;
         chain = chain->NEXT)
        {
         lc=(locon_list *)chain->DATA;
         if (lc->TYPE=='I') insuser=getptype(((loins_list *)lc->ROOT)->USER, FCL_TRANSFER_PTYPE);
         else insuser=NULL;

         if (insuser==NULL || ((long)insuser->DATA & FCL_NEVER) == 0)
           ptypex->DATA = (void *)addchain((chain_list *)ptypex->DATA,
                                            chain->DATA) ;
        }
     }
   }
}

/*****************************************************************************/
/*                        function tas_gettypesig()                          */
/* parametres :                                                              */
/* losig : signal a ajouter                                                  */
/*                                                                           */
/* calcul le type de signal a ajouter dans la ttvfig                         */
/*****************************************************************************/
long tas_gettypesig(losig)
losig_list *losig ;
{
 ptype_list *ptype ;
 long type = (long)0 ;
 
 for(ptype = losig->USER ; ptype != NULL ; ptype = ptype->NEXT)
    switch(ptype->TYPE)
      {
       case TAS_SIG_BYPASSIN  : type |= TTV_SIG_BYPASSIN ;
                                break ;
       case TAS_SIG_BYPASSOUT : type |= TTV_SIG_BYPASSOUT ;
                                break ;
       case TAS_SIG_BREAK     : type |= TTV_SIG_B ;
                                break ;
       case TAS_SIG_INTER     : type |= TTV_SIG_I ;
                                break ;
       case TAS_SIG_TYPE      : type |= (long)ptype->DATA ;
                                break ;
      }

 if((type & (TTV_SIG_Q|TTV_SIG_L|TTV_SIG_Q)) != 0)
   type &= ~(TTV_SIG_I|TTV_SIG_B) ;

 if(losig->TYPE == EXTERNAL)
     type &= ~(TTV_SIG_EXT) ;

 return(type) ;
}

/*****************************************************************************/
/*                        function tas_flattenhfig()                         */
/* parametres :                                                              */
/* lofig : lofig a construire                                                */
/*                                                                           */
/* met a plat une figure hierarchique                                        */
/*****************************************************************************/
void tas_flattenhfig(lofig)
lofig_list *lofig ;
{
 FILE *file ;
 loins_list *loins ;
 chain_list *chain ;
 chain_list *chainsav ;
 chain_list *chainx ;
 char filename[256] ;

 chain = NULL ;
 chainsav = NULL ;

 for(loins = lofig->LOINS ; loins != NULL ; loins = loins->NEXT)
  {
   if(ttv_gethtmodel(loins->FIGNAME) != NULL)
    {
     chainsav = addchain(chainsav,(void *)loins) ;
    }
   else if((file = mbkfopen(loins->FIGNAME,"ttx",READ_TEXT)) != NULL)
    {
     if(fclose(file) != 0)
      {
       sprintf(filename,"%s.ttx",loins->FIGNAME) ;
       tas_error(10,filename,TAS_WARNING) ;
      }
     chainsav = addchain(chainsav,(void *)loins) ;
    }
   else if((file = mbkfopen(loins->FIGNAME,"dtx",READ_TEXT)) != NULL)
    {
     if(fclose(file) != 0)
      {
       sprintf(filename,"%s.dtx",loins->FIGNAME) ;
       tas_error(10,filename,TAS_WARNING) ;
      }
     chainsav = addchain(chainsav,(void *)loins) ;
    }
   else chain = addchain(chain,(void *)loins) ;
  }

 lofig->LOINS = NULL ;

 for(chainx = chainsav ; chainx != NULL ; chainx = chainx->NEXT)
  { 
   loins = (loins_list*)chainx->DATA ;
   loins->NEXT = lofig->LOINS ;
   lofig->LOINS = loins ;
  }

 freechain(chainsav) ;

 if(chain == NULL)
    return ;

 for(chainx = chain ; chainx != NULL ; chainx = chainx->NEXT)
  { 
   loins = (loins_list*)chainx->DATA ;
   loins->NEXT = lofig->LOINS ;
   lofig->LOINS = loins ;
  }

 chain = reverse(chain) ;

 flattenloinslist(lofig, chain, 'Y');

 /*
 for(chainx = chain ; chainx != NULL ; chainx = chainx->NEXT)
  {
   flattenlofig(lofig,((loins_list*)chainx->DATA)->INSNAME,'Y') ;
  }
 */

 freechain(chain) ;
 tas_flattenhfig(lofig);
}

/*****************************************************************************/
/*                        function tas_builtfig()                            */
/* parametres :                                                              */
/* lofig : lofig a construire                                                */
/*                                                                           */
/* construit une ttvfig pere avec ses connecteurs et ses fils                */
/* rajoute des ptype des signaux con des ttvfig vers les locon de la lofig   */
/*****************************************************************************/
ttvfig_list *tas_builtfig(lofig)
lofig_list *lofig ;
{
 ttvfig_list *ttvfig ;
 ttvfig_list *ttvins ;
 loins_list *loins ;
 losig_list *losig ;
 chain_list *chain ;
 ptype_list *insuser ;
 int cont ;
 char flag = 'N' ;

 mbk_comcheck( mbk_signlofig(lofig), 0, 0 );
 
 for(losig = lofig->LOSIG ; losig != NULL ; losig = losig->NEXT)
  {
   if(losig->PRCN == NULL)
     addlorcnet(losig) ;
   if(getptype(losig->USER,TAS_SIG_VDD) != NULL)
    {
     cbh_setConstraint(losig,CBH_CONSTRAINT_1) ;
     flag = 'Y' ;
    }
   if(getptype(losig->USER,TAS_SIG_VSS) != NULL)
    {
     cbh_setConstraint(losig,CBH_CONSTRAINT_0) ;
     flag = 'Y' ;
    }
  }

 if(flag == 'Y')
  {
//   cbh_sim(lofig) ;

   for(losig = lofig->LOSIG ; losig != NULL ; losig = losig->NEXT)
    {
     cont = cbh_getConstraint(losig) ;
     if((cont == CBH_CONSTRAINT_0) || (cont == CBH_CONSTRAINT_1) ||
        (cont == CBH_CONSTRAINT_Z))
       {
        losig->TYPE = TAS_ALIM ;
        tlc_setlosigalim(losig) ;
       }
    }
  }

 ttvfig = ttv_givehead(lofig->NAME,lofig->NAME,NULL)  ;

 if(ttvfig->STATUS != (TTV_STS_MODEL|TTV_STS_HEAD))
    tas_error(62,ttvfig->INFO->FIGNAME,TAS_ERROR) ;

 ttv_lockttvfig(ttvfig) ;

 for(loins = lofig->LOINS ; loins != NULL ; loins = loins->NEXT)
    {
     insuser=getptype(loins->USER, FCL_TRANSFER_PTYPE);
     if (insuser==NULL || ((long)insuser->DATA & FCL_NEVER) == 0)
        ttvins = ttv_givehead(loins->FIGNAME,loins->INSNAME,ttvfig) ;
    }

 ttvfig->INS = reverse(ttvfig->INS) ;

 for(chain = ttvfig->INS ; chain != NULL ; chain = chain->NEXT)
  {
   ttvins = (ttvfig_list *)chain->DATA ;
   if(TAS_CONTEXT->TAS_PERFINT == 'Y')
     ttv_parsttvfig(ttvins,TTV_STS_CL|TTV_STS_SE,TTV_FILE_DTX) ;
   if(TAS_CONTEXT->TAS_INT_END == 'N')
     ttv_parsttvfig(ttvins,TTV_STS_CL,TTV_FILE_TTX) ;
  }

 ttvfig->INFO->CAPAOUT = (TAS_CONTEXT->TAS_CAPAOUT * 1000.0);
 ttvfig->INFO->SLOPE = (long)TAS_CONTEXT->FRONT_CON;

 ttv_setttvlevel(ttvfig) ;

 stm_addcell(ttvfig->INFO->FIGNAME) ;

 mbk_comcheck( 0, 0, ttv_signtimingfigure( ttvfig ) );
 return(ttvfig) ;
}

/*****************************************************************************/
/*                        function tas_cleanfig()                            */
/* parametres :                                                              */
/* lofig : lofig a nettoyer                                                  */
/* ttvfig : ttvfig a nettoyer                                                */
/*                                                                           */
/* nettoye une figure des ptype et des chain                                 */
/*****************************************************************************/
void tas_cleanfig(lofig,ttvfig)
lofig_list *lofig ;
ttvfig_list *ttvfig ;
{
 ttvfig_list *ttvins ;
 ttvsig_list *ptsig ;
 locon_list *locon ;
 loins_list *loins ;
 losig_list *losig ;
 ptype_list *ptype, *insuser ;
 chain_list *chain ;
 long i ;

 for(locon = lofig->LOCON ; locon != NULL ; locon = locon->NEXT)
   {
    if(locon->DIRECTION == TAS_ALIM) continue ;
    if((ptype = getptype(locon->USER,TAS_LOCON_SIG)) != NULL) 
      {
       ptsig = (ttvsig_list *)ptype->DATA ;
       ptsig->USER = testanddelptype(ptsig->USER,TAS_SIG_LOCON) ;
       locon->USER = testanddelptype(locon->USER,TAS_LOCON_SIG) ;
      }
   }

 for(loins = lofig->LOINS ; loins != NULL ; loins = loins->NEXT)
   {
     insuser=getptype(loins->USER, FCL_TRANSFER_PTYPE);
     if (insuser!=NULL && ((long)insuser->DATA & FCL_NEVER) != 0) continue;

    for(locon = loins->LOCON ; locon != NULL ; locon = locon->NEXT)
      {
       if(locon->DIRECTION == TAS_ALIM) continue ;
       if((ptype = getptype(locon->USER,TAS_LOCON_SIG)) != NULL) 
         {
          ptsig = (ttvsig_list *)ptype->DATA ;
          ptsig->USER = delptype(ptsig->USER,TAS_SIG_LOCON) ;
          locon->USER = delptype(locon->USER,TAS_LOCON_SIG) ;
         }
       if((ptype = getptype(locon->USER,TAS_LOCON_NSIG)) != NULL)
         {
          freechain((chain_list *)ptype->DATA) ;
          locon->USER = delptype(locon->USER,TAS_LOCON_NSIG) ;
         }
      }
   }

 for(losig = lofig->LOSIG ; losig != NULL ; losig = losig->NEXT)
  {  
   if((ptype = getptype(losig->USER,TAS_LOSIG_BUF)) != NULL)
      {
       freechain((chain_list *)ptype->DATA) ;
       losig->USER = delptype(losig->USER,TAS_LOSIG_BUF) ;
      }
   if((getptype(losig->USER,TAS_SIG_NORCDELAY)) != NULL)
      {
       losig->USER = delptype(losig->USER,TAS_SIG_NORCDELAY) ;
      }
   if((ptype = getptype(losig->USER,TAS_LOFIGCHAIN)) != NULL)
      {
       freechain((chain_list *)ptype->DATA) ;
       losig->USER = delptype(losig->USER,TAS_LOFIGCHAIN) ;
      }
   if((ptype=getptype(losig->USER,TAS_SIG_CAPA)) != NULL)
      {
       mbkfree(ptype->DATA);
       losig->USER = delptype(losig->USER,TAS_SIG_CAPA) ;
      }
   if((getptype(losig->USER,TAS_SIG_TYPE)) != NULL)
      {
       losig->USER = delptype(losig->USER,TAS_SIG_TYPE) ;
      }
  }

 for(chain = ttvfig->INS ; chain != NULL ; chain = chain->NEXT)
  {
   ttvins = (ttvfig_list *)chain->DATA ;
   for(i = 0 ; i < ttvins->NBCONSIG ; i++)
    {
     ptsig = *(ttvins->CONSIG + i) ;
     ptsig->TYPE &= ~(TTV_SIG_MARQUE) ;
     if(getptype(ptsig->USER,TAS_SIG_NEWSIG) != NULL)
       ptsig->USER = delptype(ptsig->USER,TAS_SIG_NEWSIG) ;
     if(getptype(ptsig->USER,TAS_SIG_LOSIG) != NULL)
       ptsig->USER = delptype(ptsig->USER,TAS_SIG_LOSIG) ;
    }
   for(i = 0 ; i < ttvins->NBNCSIG ; i++)
    {
     ptsig = *(ttvins->NCSIG + i) ;
     ptsig->TYPE &= ~(TTV_SIG_MARQUE) ;
     if(getptype(ptsig->USER,TAS_SIG_NEWSIG) != NULL)
       ptsig->USER = delptype(ptsig->USER,TAS_SIG_NEWSIG) ;
     if(getptype(ptsig->USER,TAS_SIG_LOSIG) != NULL)
       ptsig->USER = delptype(ptsig->USER,TAS_SIG_LOSIG) ;
     if(getptype(ptsig->USER,TAS_SIG_LOCONEXT) != NULL)
       ptsig->USER = delptype(ptsig->USER,TAS_SIG_LOCONEXT) ;
    }
  }
}

/*****************************************************************************/
/*                        function tas_loconorient()                         */
/* parametres :                                                              */
/* lofig : lofig de depart                                                   */
/* ttvfig : ttvfig correspondantes a la lofig                                */
/*                                                                           */
/* rajoute des ptype des signaux con des ttvfig ins vers les locon des loins */
/* oriente les connecteurs de la lofig et des loins                          */
/*****************************************************************************/

/* sous fonction de creation d'une table de hash pour les connecteurs de dtx */
#define TOO_MUCH_LOCONS 100
#define LOFIG_LOCON_HT 0xfab60216

static ht *tas_create_con_ht(ttvfig_list *ttvins)
{
  ht *h;
  int i;
  h=addht(ttvins->NBCONSIG+3000);
  for(i = 0 ; i < ttvins->NBCONSIG ; i++)
  {
    addhtitem(h, (*(ttvins->CONSIG + i))->NAME, i);
  }
  return h;
}

static ht *tas_get_lofig_con_ht(lofig_list *lf)
{
  ht *h;
  locon_list *lc;

  h=addht(1000);
  for (lc=lf->LOCON; lc!=NULL; lc=lc->NEXT)
    addhtitem(h, lc->NAME, (long)lc);

  return h;
}


void tas_loconorient(lofig,ttvfig)
lofig_list *lofig ;
ttvfig_list *ttvfig ;
{
 ttvfig_list *ttvins ;
 ttvsig_list *ptsig ;
 locon_list *locon, *lc ;
 loins_list *loins ;
 losig_list *losig ;
 ptype_list *ptype, *insuser ;
 chain_list *chain ;
 chain_list *chainins ;
 ht *speedloconht, *lofigloconht;
 long i, l, isalim ;
 long type ;
 int nbi, errors=0 ;
 int nbo ;
 int nbz ;
 int nbb ;
 int nbt ;
 int nbx ;
 int fanout ;
 int index = 0 ;
 char message[256] ;
 inffig_list *ifl;
 lofig_list *lf;
 
 chain = NULL ;
 if(ttvfig != NULL) chainins = ttvfig->INS;
 else chainins = NULL;
 
 ifl=getloadedinffig(lofig->NAME);
 
 for(losig = lofig->LOSIG ; losig != NULL ; losig = losig->NEXT)
  {
   if(losig->INDEX > index) index = losig->INDEX ;
   if(losig->TYPE == EXTERNAL)
    continue ;
   if(((mbk_LosigIsVDD(losig)) != 0) ||
      ((mbk_LosigIsVSS(losig)) != 0))
     {
      losig->TYPE = TAS_ALIM ;
      tlc_setlosigalim(losig) ;
     }
  }

 locon = lofig->LOCON ;
 while(locon != NULL)
  {
   if(((mbk_LosigIsVDD(locon->SIG)) != 0) || 
      ((mbk_LosigIsVSS(locon->SIG)) != 0))
    {
     locon->SIG->TYPE = TAS_ALIM ;
     tlc_setlosigalim(locon->SIG) ;
     locon->DIRECTION = TAS_ALIM ;
     locon = locon->NEXT ;
     continue ;
    }
   locon->NAME = ttv_revect(locon->NAME) ;
   locon = locon->NEXT ;
  }
 if (chainins != NULL)
  {
   for(loins = lofig->LOINS ; loins != NULL ; loins = loins->NEXT)
    {
     insuser=getptype(loins->USER, FCL_TRANSFER_PTYPE);
     if (!(insuser==NULL || ((long)insuser->DATA & FCL_NEVER) == 0)) continue;

     ttvins = (ttvfig_list *)chainins->DATA ;

     lf=getloadedlofig(loins->FIGNAME);
     if (lf!=NULL) lofigloconht=tas_get_lofig_con_ht(lf);
     else lofigloconht=NULL;

     if (ttvins->NBCONSIG>TOO_MUCH_LOCONS) speedloconht=tas_create_con_ht(ttvins);
     else speedloconht=NULL; // pour etre sur de ne pas faire de betises
     
     locon = loins->LOCON ;
     while(locon != NULL)
        {
          if (lofigloconht!=NULL)
            {
              if ((l=gethtitem(lofigloconht, locon->NAME))==EMPTYHT)
                avt_errmsg( TAS_ERRMSG, "073", AVT_FATAL, lc->NAME, loins->FIGNAME);
              
              lc=(locon_list *)l;
              if(mbk_LosigIsVDD(lc->SIG) || mbk_LosigIsVSS(lc->SIG)) isalim=1;
              else isalim=0;
            }
          else
            {
              if(isvdd(locon->NAME) || isvss(locon->NAME)) isalim=1;
              else isalim=0;
            }

          locon->NAME = ttv_revect(locon->NAME) ;
          if (ttvins->NBCONSIG>TOO_MUCH_LOCONS)
          {
            // pour l'optimisation, on fait en sort de respecter le comportement de
            // la boucle ci-dessous
            if ((i=gethtitem(speedloconht, locon->NAME))==EMPTYHT) i=ttvins->NBCONSIG;
            if (i==ttvins->NBCONSIG && locon->SIG->TYPE==TAS_ALIM) isalim=1;
          }
          else
          {
            for(i = 0 ; i < ttvins->NBCONSIG ; i++)
            {
             if((*(ttvins->CONSIG + i))->NAME == locon->NAME) break ;
            }
            if (i==ttvins->NBCONSIG && locon->SIG->TYPE==TAS_ALIM) isalim=1;
          }

          if (isalim)
            {
              locon->DIRECTION = TAS_ALIM ;
              if (locon->SIG->TYPE != TAS_ALIM)
                avt_errmsg( TAS_ERRMSG, "072", AVT_ERROR, getsigname(locon->SIG), lc->NAME, loins->INSNAME);
              if(i != ttvins->NBCONSIG)
               {
                ptsig = *(ttvins->CONSIG + i) ;
                ptsig->USER = addptype(ptsig->USER,TAS_SIG_LOCON,(void *)locon) ;
                locon->USER = addptype(locon->USER,TAS_LOCON_SIG,(void *)ptsig) ;
               }
              locon = locon->NEXT ;
              continue ;
            }

         if(i != ttvins->NBCONSIG)
          {
           ptsig = *(ttvins->CONSIG + i) ;
           ptsig->USER = addptype(ptsig->USER,TAS_SIG_LOCON,(void *)locon) ;
           locon->USER = addptype(locon->USER,TAS_LOCON_SIG,(void *)ptsig) ;
          }
         else
          {
           if(TAS_CONTEXT->TAS_LANG == 'E')
             sprintf(message,"%s the connector %s",loins->FIGNAME,locon->NAME) ;
           else if(TAS_CONTEXT->TAS_LANG == 'F')
             sprintf(message,"%s le connecteur %s",loins->FIGNAME,locon->NAME) ;
           avt_errmsg( TAS_ERRMSG, "051", AVT_ERROR, message ); //tas_error(51,message,TAS_ERROR) ;
           errors++;
           ptsig=NULL;
          }
         
         if (ptsig!=NULL)
         {
          type = ptsig->TYPE & TTV_SIG_TYPECON ;
          switch(type)
            {
             case TTV_SIG_CI :  locon->DIRECTION = IN ;
                                break ;
             case TTV_SIG_CO :  locon->DIRECTION = OUT ;
                                break ;
             case TTV_SIG_CZ :  locon->DIRECTION = TRISTATE ;
                                break ;
             case TTV_SIG_CB :  locon->DIRECTION = INOUT ;
                                break ;
             case TTV_SIG_CT :  locon->DIRECTION = TRANSCV ;
                                break ;
             case TTV_SIG_CX :  locon->DIRECTION = UNKNOWN ;
                                break ;
             default         :  locon->DIRECTION = UNKNOWN ;
                                break ;
            }
         }
         locon = locon->NEXT ;
        }

     for(i = 0 ; i < ttvins->NBCONSIG ; i++)
      if((ptype=getptype((*(ttvins->CONSIG + i))->USER,TAS_SIG_LOCON)) == NULL)
       {
         if(TAS_CONTEXT->TAS_LANG == 'E')
          sprintf(message,"%s the connector %s",
                  ttvins->INFO->FIGNAME,(*(ttvins->CONSIG + i))->NAME) ;
         else if(TAS_CONTEXT->TAS_LANG == 'F')
          sprintf(message,"%s le connecteur %s",
                  ttvins->INFO->FIGNAME,(*(ttvins->CONSIG + i))->NAME) ;
         avt_errmsg( TAS_ERRMSG, "051", AVT_ERROR, message ); //tas_error(51,message,TAS_ERROR) ;
         errors++;
       }

     if (ttvins->NBCONSIG>TOO_MUCH_LOCONS) delht(speedloconht);
     delht(lofigloconht);

     chainins = chainins->NEXT ;
    }
  }

 if (errors) tas_GetKill(51);

 tas_lofigchain(lofig) ;

 for(locon = lofig->LOCON ; locon != NULL ; locon = locon->NEXT)
  {
   losig = locon->SIG ;
   ptype = getptype(losig->USER,TAS_LOFIGCHAIN) ;
   nbi = nbo = nbt = nbb = nbz = nbx = 0 ;
   if(ptype != NULL)
    for(chain = (chain_list *)ptype->DATA ; chain != NULL ; chain = chain->NEXT)
     {
      locon_list *loconx ;
      
      if((loconx = (locon_list *)chain->DATA) == locon) continue ;
      if(loconx->TYPE != INTERNAL) continue ;
      switch(loconx->DIRECTION)
        {
         case IN       : nbi++ ;
                         break ;
         case OUT      : nbo++ ;
                         break ;
         case TRISTATE : nbz++ ;
                         break ;
         case TRANSCV  : nbt++ ;
                         break ;
         case INOUT    : nbb++ ;
                         break ;
         case UNKNOWN  : nbx++ ;
                         break ;
        }
     }
   else continue ;


   if((nbi != 0) && (nbo == 0) && (nbb == 0) && (nbt == 0) && (nbz == 0))
    {
     if((locon->DIRECTION == UNKNOWN) || (locon->DIRECTION == IN))
       locon->DIRECTION = IN ;
     else if((locon->DIRECTION == OUT) || (locon->DIRECTION == INOUT))
       locon->DIRECTION = INOUT ;
     else if((locon->DIRECTION == TRISTATE) || (locon->DIRECTION == TRANSCV))
       locon->DIRECTION = TRANSCV ;
    }
   else if((nbi == 0) && (nbo >= 1) && (nbb == 0) && (nbt == 0) && (nbz == 0))
    {
     if((locon->DIRECTION == UNKNOWN) || (locon->DIRECTION == OUT))
       locon->DIRECTION = OUT ;
     else if((locon->DIRECTION == IN) || (locon->DIRECTION == INOUT))
       locon->DIRECTION = INOUT ;
     else if(locon->DIRECTION == TRISTATE)
       locon->DIRECTION = TRISTATE ;
     else if(locon->DIRECTION == TRANSCV)
       locon->DIRECTION = TRANSCV ;

     if(nbo > 1)
      {
       if(getptype(losig->USER,TAS_SIG_NORCDELAY) == NULL)
          losig->USER = addptype(losig->USER,TAS_SIG_NORCDELAY,NULL) ;

       losig->USER = addptype(losig->USER,TAS_LOSIG_BUF,NULL) ;
       for(chain = (chain_list *)ptype->DATA ; chain != NULL ;
            chain = chain->NEXT)
        {
         locon_list *loconx ;
 
         loconx = (locon_list *)chain->DATA ;
         if((loconx->DIRECTION == OUT) && (loconx->TYPE == INTERNAL))
            losig->USER->DATA = (void*)addchain((chain_list *)losig->USER->DATA,
                                                (void*)loconx) ;
        }
      }
    }
   else if(((nbi != 0) && (nbo >= 1) && (nbb == 0) && (nbt == 0) && (nbz == 0)) 
        || ((nbb >= 1) && (nbt == 0) && (nbz == 0)))
    {
     if((locon->DIRECTION == UNKNOWN) || (locon->DIRECTION == OUT) || 
        (locon->DIRECTION == INOUT) || (locon->DIRECTION == IN))
       locon->DIRECTION = INOUT ;
     else if((locon->DIRECTION == TRISTATE) || (locon->DIRECTION == TRANSCV))
       locon->DIRECTION = TRANSCV ;

     if((nbb > 1) || (nbo > 1))
      {
       if(getptype(losig->USER,TAS_SIG_NORCDELAY) == NULL)
          losig->USER = addptype(losig->USER,TAS_SIG_NORCDELAY,NULL) ;
 
       losig->USER = addptype(losig->USER,TAS_LOSIG_BUF,NULL) ;
       for(chain = (chain_list *)ptype->DATA ; chain != NULL ;
            chain = chain->NEXT)
        {
         locon_list *loconx ;
 
         loconx = (locon_list *)chain->DATA ;
         if(((loconx->DIRECTION == OUT) || (loconx->DIRECTION == INOUT))
             && (loconx->TYPE == INTERNAL))
            losig->USER->DATA = (void*)addchain((chain_list *)losig->USER->DATA,
                                                (void*)loconx) ;
        }
      }
    }
   else if(((nbo == 0) && (nbb == 0) && (nbt != 0))
        || ((nbi != 0) && (nbo == 0) && (nbb == 0) && (nbz != 0)))
    {
     locon->DIRECTION = TRANSCV ;
    }
   else if((nbi == 0) && (nbo == 0) && (nbb == 0) && (nbt == 0)  && (nbz != 0))
    {
     if(locon->DIRECTION == IN)
       locon->DIRECTION = TRANSCV ;
     else
       locon->DIRECTION = TRISTATE ;
    }
   else if((nbi == 0) && (nbo == 0) && (nbb == 0) && (nbt == 0) && (nbz == 0))
    {
     /* unknown */
    }
   else tas_error(52,ttv_revect(yagGetName(ifl,locon->SIG)),TAS_ERROR);
   
   fanout = nbo + nbt + nbb + nbz ;
   fanout += (nbi + nbx) ; 
   if(locon->DIRECTION != IN) fanout++;

   if(rcn_existcapa(lofig,losig) == 0)
    {
     tas_addcapalosig(lofig,losig,stm_getfancap(STM_PROPERTIES,(float)fanout)/1000.0,stm_getfanres(STM_PROPERTIES,(float)fanout));
    }
  }

 for(losig = lofig->LOSIG ; losig != NULL ; losig = losig->NEXT)
  {
   if(losig->TYPE == TAS_ALIM)
    {
     continue ;
    }

   if(getptype(losig->USER,TAS_SIG_NORC) != NULL)
    { 
     if(TAS_CONTEXT->TAS_CALCRCN == 'Y')
      {
       if(getptype(losig->USER,TAS_SIG_NORCDELAY) == NULL)
          losig->USER = addptype(losig->USER,TAS_SIG_NORCDELAY,NULL) ;
      }
     losig->USER = delptype(losig->USER,TAS_SIG_NORC) ;
    }
   
   if(TAS_CONTEXT->TAS_CALCRCN == 'N')
    {
     if(getptype(losig->USER,TAS_SIG_RC) != NULL)
      {
       losig->USER = delptype(losig->USER,TAS_SIG_RC) ;
      }
     else
      {
       if(getptype(losig->USER,TAS_SIG_NORCDELAY) == NULL)
          losig->USER = addptype(losig->USER,TAS_SIG_NORCDELAY,NULL) ;
      }
    }
   else if(getptype(losig->USER,TAS_SIG_RC) != NULL)
          losig->USER = delptype(losig->USER,TAS_SIG_RC) ;

   if(losig->TYPE == EXTERNAL)
     continue ;

   ptype = getptype(losig->USER,TAS_LOFIGCHAIN) ;
   nbi = nbo = nbt = nbb = nbz = nbx = 0 ;
   if(ptype != NULL)
    for(chain = (chain_list *)ptype->DATA ; chain != NULL ; chain = chain->NEXT)
     {
      locon = (locon_list *)chain->DATA ;
      if((locon->TYPE != INTERNAL) && (locon->TYPE != 'C')) continue ;
      switch(locon->DIRECTION)
        {
         case IN       : nbi++ ;
                         break ;
         case OUT      : nbo++ ;
                         break ;
         case TRISTATE : nbz++ ;
                         break ;
         case TRANSCV  : nbt++ ;
                         break ;
         case INOUT    : nbb++ ;
                         break ;
         case UNKNOWN  : nbx++ ;
                         break ;
        }
     }
   else continue ;

   fanout = nbo + nbt + nbb + nbz ;
   fanout += (nbi + nbx) ; 

   if(rcn_existcapa(lofig,losig) == 0)
    {
     tas_addcapalosig(lofig,losig,stm_getfancap(STM_PROPERTIES,(float)fanout)/1000.0,stm_getfanres(STM_PROPERTIES,(float)fanout));
    }

   if((nbi + nbo + nbb + nbt + nbz) == 1)
    {
     tas_error(54,ttv_revect(yagGetName(ifl,losig)),TAS_WARNING) ;
    }
   if((nbi != 0) && (nbo == 0) && (nbb == 0) && (nbt == 0) && (nbz == 0))
    {
     tas_error(55,ttv_revect(yagGetName(ifl,losig)),TAS_WARNING) ;
    }
   /*else if(((nbo + nbb) > 1) && (nbi == 0))
    {
     tas_error(56,ttv_revect(yagGetName(ifl,losig)),TAS_WARNING) ;
    }
   else if((nbz != 0) && (nbt == 0) && (nbi == 0))
    {
     tas_error(56,ttv_revect(yagGetName(ifl,losig)),TAS_WARNING) ;
    }
   else if(((nbo + nbb) >= 1) && ((nbt != 0) || (nbz != 0)))
    {
     tas_error(57,ttv_revect(yagGetName(ifl,losig)),TAS_WARNING) ;
    }*/
   else if((nbo + nbb) > 1)
    {
     if(getptype(losig->USER,TAS_SIG_NORCDELAY) == NULL)
          losig->USER = addptype(losig->USER,TAS_SIG_NORCDELAY,NULL) ;

     losig->USER = addptype(losig->USER,TAS_LOSIG_BUF,NULL) ;
     for(chain = (chain_list *)ptype->DATA ; chain != NULL ;
         chain = chain->NEXT)
      {
       locon_list *loconx ;

       loconx = (locon_list *)chain->DATA ;
       if((loconx->DIRECTION == OUT) || (loconx->DIRECTION == INOUT))
          losig->USER->DATA = (void*)addchain((chain_list *)losig->USER->DATA,
                                              (void*)loconx) ;
      }
    }
  }
}

/*****************************************************************************/
/*                        function tas_detecloconsig()                       */
/* parametres :                                                              */
/* ttvfig : ttvfig pere                                                      */
/*                                                                           */
/* cree les signaux de la nouvelle figure                                    */
/* s'il y a des reseaux sur un signal il cree des ncsig sinon un sig         */
/* si le reseau est termine il remplace les nsig par des signaux             */
/*****************************************************************************/

static ttvsig_list *tas_getcorrespnewsig(ttvsig_list *tvs)
{
  ptype_list *pt;
  if ((pt = getptype(tvs->USER,TAS_SIG_NEWSIG))!=NULL) return (ttvsig_list *)pt->DATA;
  return NULL;
}

static chain_list *tas_getsignalcmdlist(ttvsig_list *tvs)
{
  ptype_list *pt;
  if ((pt = getptype(tvs->USER,TTV_SIG_CMD))!=NULL) return (chain_list *)pt->DATA;
  return NULL;
}

static chain_list *tas_addsignalcmd(ttvsig_list *tvs, ttvevent_list *cmd)
{
  ptype_list *pt;
  if ((pt = getptype(tvs->USER,TTV_SIG_CMD))==NULL)
    pt=tvs->USER=addptype(tvs->USER,TTV_SIG_CMD, NULL);
  pt->DATA=addchain((chain_list *)pt->DATA, cmd);
  return (chain_list *)pt->DATA;
}

void tas_detecloconsig(lofig,ttvfig)
lofig_list *lofig ;
ttvfig_list *ttvfig ;
{
 ttvfig_list *ttvins ;
 ttvsig_list *ptsig ;
 ttvsig_list *ptsigx, *correspsig ;
 ttvline_list *ptline ;
 ttvlbloc_list *ptlbloc ;
 locon_list *locon ;
 losig_list *losig ;
 ptype_list *ptype ;
 chain_list *chain ;
 chain_list *chainx, *cl ;
 chain_list *ncsig = NULL ;
 chain_list *qsig = NULL ;
 chain_list *bsig = NULL ;
 chain_list *lsig = NULL ;
 chain_list *rsig = NULL ;
 chain_list *isig = NULL ;
 chain_list *esig = NULL ;
 chain_list *oldlatch = NULL, *ptsigcmd, *ptsigxcmd ;
 ttvevent_list *tev;
 long type ;
 long i ;
 char signame[1024] ;
 inffig_list *ifl;

 ifl=getloadedinffig(lofig->NAME);

 chain = NULL ;
 for(locon = lofig->LOCON ; locon != NULL ; locon = locon->NEXT)
  {
   if(locon->SIG->TYPE == TAS_ALIM)
     if(((mbk_LosigIsVDD(locon->SIG)) != 0) || 
        ((mbk_LosigIsVSS(locon->SIG)) != 0))
     continue ;

   type = tas_gettypesig(locon->SIG) ;
   
   switch(locon->DIRECTION)
     {
      case IN :
         type |= TTV_SIG_CI ;
               break ;
      case OUT :
         type |= TTV_SIG_CO ;
               break ;
      case INOUT :
         type |= TTV_SIG_CB ;
               break ;
      case TRANSCV :
         type |= TTV_SIG_CT ;
               break ;
      case TRISTATE :
         type |= TTV_SIG_CZ ;
               break ;
      case UNKNOWN :
         type |= TTV_SIG_CX ;
               break ;
      case TAS_ALIM :
         type |= TTV_SIG_CX ;
               break ;
     }

   type &= ~(TTV_SIG_L|TTV_SIG_Q|TTV_SIG_R|TTV_SIG_I|TTV_SIG_B) ;

   chain = ttv_addrefsig(ttvfig,locon->NAME,locon->NAME,
                         tas_getcapa(lofig,locon->SIG),TTV_SIG_C|type,chain) ;
   ptsig = (ttvsig_list *)chain->DATA ;
   ttv_setallsigcapas ( lofig, locon->SIG, ptsig);
   if(getptype(locon->SIG->USER,TAS_SIG_ONLYEND) != NULL)
     {
      ptsig->NODE->TYPE |= TTV_NODE_ONLYEND ;
      (ptsig->NODE+1)->TYPE |= TTV_NODE_ONLYEND ;
     }
   ptsig->USER = addptype(ptsig->USER,TAS_SIG_LOCON,(void *)locon) ;
   locon->USER = addptype(locon->USER,TAS_LOCON_SIG,(void *)ptsig) ;
  }

 ttvfig->CONSIG = (ttvsig_list **)chain ;

 for(chain = ttvfig->INS ; chain != NULL ; chain = chain->NEXT)
  {
   char typesig1 ;
   char typesig2 ;

   ttvins = (ttvfig_list *)chain->DATA ;

   if(TAS_CONTEXT->TAS_PERFINT == 'Y')
    {
     if((ttvins->STATUS & TTV_STS_D) != TTV_STS_D)
        ttv_parsttvfig(ttvins,TTV_STS_D,TTV_FILE_DTX) ;
     ptlbloc = ttvins->DBLOC ;
    }
    else
     ptlbloc = NULL ;

   for(; ptlbloc != NULL ; ptlbloc = ptlbloc->NEXT)
    for(i = 0 ; i < TTV_MAX_LBLOC ; i++)
      {
       ptline = ptlbloc->LINE + i ;
       if((ptline->TYPE & TTV_LINE_FR) == TTV_LINE_FR)
         continue ;
       ptsig = ptline->NODE->ROOT ;
       ptsigx = ptline->ROOT->ROOT ;

       if((ptsig->TYPE & (TTV_SIG_N | TTV_SIG_C)) == 0)
        {
         typesig1 = INTERNAL ;
        }
       else if((ptsig->TYPE & TTV_SIG_N) == TTV_SIG_N)
        {
         if((ptype = getptype(ptsig->USER,TAS_SIG_LOCONEXT)) != NULL)
          {
           locon = (locon_list *)ptype->DATA ;
           if(locon->SIG->TYPE == EXTERNAL)
             typesig1 = EXTERNAL ;
           else typesig1 = INTERNAL ;
          }
        }
       else
        {
         locon = (locon_list *)getptype(ptsig->USER,
                                        TAS_SIG_LOCON)->DATA ;
         if(locon->SIG->TYPE == EXTERNAL)
             typesig1 = EXTERNAL ;
         else typesig1 = INTERNAL ;
        }
        
       if((ptsigx->TYPE & (TTV_SIG_N | TTV_SIG_C)) == 0)
        {
         typesig2 = INTERNAL ;
        }
       else if((ptsigx->TYPE & TTV_SIG_N) == TTV_SIG_N)
        {
         if((ptype = getptype(ptsigx->USER,TAS_SIG_LOCONEXT)) != NULL)
          {
           locon = (locon_list *)ptype->DATA ;
           if(locon->SIG->TYPE == EXTERNAL)
             typesig2 = EXTERNAL ;
           else typesig2 = INTERNAL ;
          }
        }
       else
        {
         locon = (locon_list *)getptype(ptsigx->USER,
                                        TAS_SIG_LOCON)->DATA ;
         if(locon->SIG->TYPE == EXTERNAL)
             typesig2 = EXTERNAL ;
         else typesig2 = INTERNAL ;
        }

       if(((typesig1 == INTERNAL) && (typesig2 == EXTERNAL)) &&
          ((ptsig->TYPE & (TTV_SIG_N | TTV_SIG_C)) != 0))
        ptsig->TYPE |= TTV_SIG_MARQUE ;

       if(((typesig2 == INTERNAL) && (typesig1 == EXTERNAL)) &&
          ((ptsigx->TYPE & (TTV_SIG_N | TTV_SIG_C)) != 0))
        ptsigx->TYPE |= TTV_SIG_MARQUE ;

      }
    ttv_freememoryiffull(ttvins,TTV_STS_D) ;
  }

 ttvfig->NBNCSIG = 0 ;
 ttvfig->NBESIG = 0 ;

 for(chain = ttvfig->INS ; chain != NULL ; chain = chain->NEXT)
  {
   ttvins = (ttvfig_list *)chain->DATA ;
   for(i = 0 ; i < ttvins->NBNCSIG ; i++)
    {
     ptsig = *(ttvins->NCSIG + i) ;
     if((ptype = getptype(ptsig->USER,TAS_SIG_LOCONEXT)) != NULL)
       {
        locon = (locon_list *)ptype->DATA ;
        if(locon->SIG->TYPE == TAS_ALIM)
          continue ;
        if(getptype(locon->SIG->USER,TAS_SIG_NORCDELAY) == NULL)  
        {
        if(locon->SIG->TYPE == EXTERNAL)
         {
          type = tas_gettypesig(locon->SIG) ;
          type &= ~(TTV_SIG_I|TTV_SIG_B) ;
          ncsig = ttv_addrefsig(ttvfig,ttv_getsigname(ttvfig,signame,ptsig),
                                ttv_revect(yagGetName(ifl,locon->SIG)),tas_getcapa(lofig,locon->SIG),
                                TTV_SIG_N|type,ncsig) ;
          if((ptsig->TYPE & TTV_SIG_Q) == TTV_SIG_Q)
            {
             ptsigx = (ttvsig_list *)ncsig->DATA ;
             qsig = addchain(qsig,(void*)ptsigx) ;
             ptsigx->TYPE |= TTV_SIG_Q ;
             ptsig->USER =  addptype(ptsig->USER,TTV_SIG_CMDNEW,ptsigx) ; 
             ptsigx->USER =  addptype(ptsigx->USER,TTV_SIG_CMDOLD,
                                      addchain(NULL,ptsig)) ; 
            }
          else if((ptsig->TYPE & TTV_SIG_B) == TTV_SIG_B)
            {
             ptsigx = (ttvsig_list *)ncsig->DATA ;
             bsig = addchain(bsig,(void*)ptsigx) ;
             ptsigx->TYPE |= TTV_SIG_B ;
            }
          else if((ptsig->TYPE & TTV_SIG_L) == TTV_SIG_L)
            {
             ptsigx = (ttvsig_list *)ncsig->DATA ;
             lsig = addchain(lsig,(void*)ptsigx) ;
             oldlatch = addchain(oldlatch,ptsig) ;
             if((ptsig->TYPE & TTV_SIG_LL) == TTV_SIG_LL)
               ptsigx->TYPE |= TTV_SIG_LL ;
             else
               ptsigx->TYPE |= TTV_SIG_LF ;
            }
          else if((ptsig->TYPE & TTV_SIG_R) == TTV_SIG_R)
            {
             ptsigx = (ttvsig_list *)ncsig->DATA ;
             rsig = addchain(rsig,(void*)ptsigx) ;
             ptsigx->TYPE |= TTV_SIG_R ;
            }
          ttvfig->NBNCSIG ++ ;
          ptsig->USER = addptype(ptsig->USER,TAS_SIG_NEWSIG,ncsig->DATA) ;
          if(getptype(locon->SIG->USER,TAS_SIG_ONLYEND) != NULL)
            {
             ((ttvsig_list *)ncsig->DATA)->NODE->TYPE |= TTV_NODE_ONLYEND ;
             (((ttvsig_list *)ncsig->DATA)->NODE+1)->TYPE |= TTV_NODE_ONLYEND ;
            }
         }
        else if((ptsig->TYPE & TTV_SIG_Q) == TTV_SIG_Q)
         {
          type = tas_gettypesig(locon->SIG) ;
          type &= ~(TTV_SIG_I|TTV_SIG_B) ;
          qsig = ttv_addrefsig(ttvfig,ttv_getsigname(ttvfig,signame,ptsig),
                              ttv_revect(yagGetName(ifl,locon->SIG)),tas_getcapa(lofig,locon->SIG),
                               TTV_SIG_Q|type,qsig) ;
          ptsigx = (ttvsig_list *)qsig->DATA ;
          ptsig->USER =  addptype(ptsig->USER,TTV_SIG_CMDNEW,ptsigx) ; 
          ptsigx->USER =  addptype(ptsigx->USER,TTV_SIG_CMDOLD,
                                   addchain(NULL,ptsig)) ; 
          ptsig->USER = addptype(ptsig->USER,TAS_SIG_NEWSIG,(void*)ptsigx) ;
         }
        else if((ptsig->TYPE & TTV_SIG_B) == TTV_SIG_B)
         {
          type = tas_gettypesig(locon->SIG) ;
          type &= ~(TTV_SIG_I) ;
          bsig = ttv_addrefsig(ttvfig,ttv_getsigname(ttvfig,signame,ptsig),
                              ttv_revect(yagGetName(ifl,locon->SIG)),tas_getcapa(lofig,locon->SIG),
                               TTV_SIG_B|type,bsig) ;
          ptsigx = (ttvsig_list *)bsig->DATA ;
          ptsig->USER = addptype(ptsig->USER,TAS_SIG_NEWSIG,(void*)ptsigx) ;
         }
        else if((ptsig->TYPE & TTV_SIG_L) == TTV_SIG_L)
         {
          type = tas_gettypesig(locon->SIG) ;
          type &= ~(TTV_SIG_I|TTV_SIG_B) ;
          lsig = ttv_addrefsig(ttvfig,ttv_getsigname(ttvfig,signame,ptsig),
                              ttv_revect(yagGetName(ifl,locon->SIG)),tas_getcapa(lofig,locon->SIG),
                               TTV_SIG_L|type,lsig) ;
          ptsigx = (ttvsig_list *)lsig->DATA ;
          ptsig->USER = addptype(ptsig->USER,TAS_SIG_NEWSIG,(void*)ptsigx) ;
          oldlatch = addchain(oldlatch,ptsig) ;
          if((ptsig->TYPE & TTV_SIG_LL) == TTV_SIG_LL)
            ptsigx->TYPE |= TTV_SIG_LL ;
          else
            ptsigx->TYPE |= TTV_SIG_LF ;
         }
        else if((ptsig->TYPE & TTV_SIG_R) == TTV_SIG_R)
         {
          type = tas_gettypesig(locon->SIG) ;
          type &= ~(TTV_SIG_I|TTV_SIG_B) ;
          rsig = ttv_addrefsig(ttvfig,ttv_getsigname(ttvfig,signame,ptsig),
                              ttv_revect(yagGetName(ifl,locon->SIG)),tas_getcapa(lofig,locon->SIG),
                               TTV_SIG_R|type,rsig) ;
          ptsigx = (ttvsig_list *)rsig->DATA ;
          ptsig->USER = addptype(ptsig->USER,TAS_SIG_NEWSIG,(void*)ptsigx) ;
         }
        else
         {
          if((ptsig->TYPE & TTV_SIG_MARQUE) == TTV_SIG_MARQUE)
           {
            type = tas_gettypesig(locon->SIG) ;
            if((type & (TTV_SIG_I|TTV_SIG_B)) == 0)
             {
              esig = ttv_addrefsig(ttvfig,ttv_getsigname(ttvfig,signame,ptsig),
                                  ttv_revect(yagGetName(ifl,locon->SIG)),tas_getcapa(lofig,locon->SIG),
                                   TTV_SIG_S|type,esig) ;
              ttvfig->NBESIG ++ ;
              ptsig->USER = addptype(ptsig->USER,TAS_SIG_NEWSIG,esig->DATA) ;
              ptsigx = (ttvsig_list *)esig->DATA ;
             }
            else if((type & TTV_SIG_I) == TTV_SIG_I)
             {
              isig = ttv_addrefsig(ttvfig,ttv_getsigname(ttvfig,signame,ptsig),
                                  ttv_revect(yagGetName(ifl,locon->SIG)),tas_getcapa(lofig,locon->SIG),
                                   type,isig) ;
              ptsig->USER = addptype(ptsig->USER,TAS_SIG_NEWSIG,isig->DATA) ;
              ptsigx = (ttvsig_list *)isig->DATA ;
             }
            else
             {
              bsig = ttv_addrefsig(ttvfig,ttv_getsigname(ttvfig,signame,ptsig),
                                  ttv_revect(yagGetName(ifl,locon->SIG)),tas_getcapa(lofig,locon->SIG),
                                   type,bsig) ;
              ptsig->USER = addptype(ptsig->USER,TAS_SIG_NEWSIG,bsig->DATA) ;
              ptsigx = (ttvsig_list *)bsig->DATA ;
             }
           }
          else 
           {
            type = tas_gettypesig(locon->SIG) ;
            if((type & (TTV_SIG_I|TTV_SIG_B)) == 0)
             {
              ptsigx = ttv_addsig(ttvfig,ttv_getsigname(ttvfig,signame,ptsig),
                                 ttv_revect(yagGetName(ifl,locon->SIG)),tas_getcapa(lofig,locon->SIG),
                                  TTV_SIG_S|type) ;
             }
            else if((type & TTV_SIG_I) == TTV_SIG_I)
             {
              isig = ttv_addrefsig(ttvfig,ttv_getsigname(ttvfig,signame,ptsig),
                                  ttv_revect(yagGetName(ifl,locon->SIG)),tas_getcapa(lofig,locon->SIG),
                                   type,isig) ;
              ptsigx = (ttvsig_list *)isig->DATA ;
             }
            else 
             {
              bsig = ttv_addrefsig(ttvfig,ttv_getsigname(ttvfig,signame,ptsig),
                                  ttv_revect(yagGetName(ifl,locon->SIG)),tas_getcapa(lofig,locon->SIG),
                                   type,bsig) ;
              ptsigx = (ttvsig_list *)bsig->DATA ;
             }
            ptsig->USER = addptype(ptsig->USER,TAS_SIG_NEWSIG,(void*)ptsigx) ;
           }
         }
        ptsig->TYPE &= ~(TTV_SIG_MARQUE) ;
        }
       }
    }

   for(i = 0 ; i < ttvins->NBELCMDSIG ; i++)
    {
     ptsig = *(ttvins->ELCMDSIG + i) ;
     if((ptsig->TYPE & (TTV_SIG_C | TTV_SIG_N)) != 0)
       continue ;
     qsig = addchain(qsig,ptsig) ;
     ttv_setsiglevel(ptsig,ttvfig->INFO->LEVEL) ;
    }

   for(i = 0 ; i < ttvins->NBELATCHSIG ; i++)
    {
     ptsig = *(ttvins->ELATCHSIG + i) ;
     if((ptsig->TYPE & (TTV_SIG_C | TTV_SIG_N)) != 0)
       continue ;
     lsig = addchain(lsig,ptsig) ;
     ttv_setsiglevel(ptsig,ttvfig->INFO->LEVEL) ;
    }

   for(i = 0 ; i < ttvins->NBEPRESIG ; i++)
    {
     ptsig = *(ttvins->EPRESIG + i) ;
     if((ptsig->TYPE & (TTV_SIG_C | TTV_SIG_N)) != 0)
       continue ;
     rsig = addchain(rsig,ptsig) ;
     ttv_setsiglevel(ptsig,ttvfig->INFO->LEVEL) ;
    }

   for(i = 0 ; i < ttvins->NBEBREAKSIG ; i++)
    {
     ptsig = *(ttvins->EBREAKSIG + i) ;
     if((ptsig->TYPE & (TTV_SIG_C | TTV_SIG_N)) != 0)
       continue ;
     bsig = addchain(bsig,ptsig) ;
     ttv_setsiglevel(ptsig,ttvfig->INFO->LEVEL) ;
    }

   for(i = 0 ; i < ttvins->NBEXTSIG ; i++)
    {
     ptsig = *(ttvins->EXTSIG + i) ;
     isig = addchain(isig,ptsig) ;
     ttv_setsiglevel(ptsig,ttvfig->INFO->LEVEL) ;
    }

   for(i = 0 ; i < ttvins->NBESIG ; i++)
    {
     ptsig = *(ttvins->ESIG + i) ;
     esig = addchain(esig,ptsig) ;
     ttvfig->NBESIG++;
     ttv_setsiglevel(ptsig,ttvfig->INFO->LEVEL) ;
    }
  }

 for(losig = lofig->LOSIG ; losig != NULL ; losig = losig->NEXT)
  {
   if(losig->TYPE == TAS_ALIM) continue ;

   if( !rcx_isvisiblesignal( losig ) ) continue ;

   ptype = getptype(losig->USER,TAS_LOFIGCHAIN) ;
   if(ptype->DATA == NULL)
    continue ;

   if(getptype(losig->USER,TAS_SIG_NORCDELAY) == NULL)
    {
     if(losig->TYPE == INTERNAL)
      {
       for(chain = ptype->DATA ; chain != NULL ; chain = chain->NEXT)
         {
          locon = (locon_list *)chain->DATA ;
          if(locon->DIRECTION == UNKNOWN)
            continue ;
          if(locon->TYPE != INTERNAL)
            continue ;
          if(getptype(locon->USER,TAS_LOCON_NSIG) == NULL)
           {
            ptsig = (ttvsig_list *)getptype(locon->USER,TAS_LOCON_SIG)->DATA ;
            if((ptsig->TYPE & TTV_SIG_Q) == TTV_SIG_Q)
             {
              type = tas_gettypesig(losig) ;
              type &= ~(TTV_SIG_I|TTV_SIG_B) ;
              qsig = ttv_addrefsig(ttvfig,ttv_getsigname(ttvfig,signame,ptsig),
                                  ttv_revect(yagGetName(ifl,losig)),tas_getcapa(lofig,losig),
                                 TTV_SIG_Q|type,qsig) ;
              ptsigx = (ttvsig_list *)qsig->DATA ;
              ptsig->USER =  addptype(ptsig->USER,TTV_SIG_CMDNEW,ptsigx) ; 
              ptsigx->USER =  addptype(ptsigx->USER,TTV_SIG_CMDOLD,
                                       addchain(NULL,ptsig)) ; 
              ptsig->USER = addptype(ptsig->USER,TAS_SIG_NEWSIG,(void*)ptsigx) ;
             }
            else if((ptsig->TYPE & TTV_SIG_B) == TTV_SIG_B)
             {
              type = tas_gettypesig(losig) ;
              type &= ~(TTV_SIG_I) ;
              bsig = ttv_addrefsig(ttvfig,ttv_getsigname(ttvfig,signame,ptsig),
                                  ttv_revect(yagGetName(ifl,losig)),tas_getcapa(lofig,losig),
                                 TTV_SIG_B|type,bsig) ;
              ptsigx = (ttvsig_list *)bsig->DATA ;
              ptsig->USER = addptype(ptsig->USER,TAS_SIG_NEWSIG,(void*)ptsigx) ;
             }
            else if((ptsig->TYPE & TTV_SIG_L) == TTV_SIG_L)
             {
              type = tas_gettypesig(losig) ;
              type &= ~(TTV_SIG_I|TTV_SIG_B) ;
              lsig = ttv_addrefsig(ttvfig,ttv_getsigname(ttvfig,signame,ptsig),
                                  ttv_revect(yagGetName(ifl,losig)),tas_getcapa(lofig,losig),
                                 TTV_SIG_L|type,lsig) ;
              ptsigx = (ttvsig_list *)lsig->DATA ;
              if((ptsig->TYPE & TTV_SIG_LL) == TTV_SIG_LL)
               ptsigx->TYPE |= TTV_SIG_LL ;
              else
               ptsigx->TYPE |= TTV_SIG_LF ;
              oldlatch = addchain(oldlatch,ptsig) ;
              ptsig->USER = addptype(ptsig->USER,TAS_SIG_NEWSIG,(void*)ptsigx) ;
             }
            else if((ptsig->TYPE & TTV_SIG_R) == TTV_SIG_R)
             {
              type = tas_gettypesig(locon->SIG) ;
              type &= ~(TTV_SIG_I|TTV_SIG_B) ;
              rsig = ttv_addrefsig(ttvfig,ttv_getsigname(ttvfig,signame,ptsig),
                                  ttv_revect(yagGetName(ifl,losig)),tas_getcapa(lofig,losig),
                                   TTV_SIG_R|type,rsig) ;
              ptsigx = (ttvsig_list *)rsig->DATA ;
              ptsig->USER = addptype(ptsig->USER,TAS_SIG_NEWSIG,(void*)ptsigx) ;
             }
            else if((ptsig->TYPE & TTV_SIG_MARQUE) == TTV_SIG_MARQUE)
             {
              type = tas_gettypesig(losig) ;
              if((type & (TTV_SIG_I|TTV_SIG_B)) == 0)
               {
                esig = ttv_addrefsig(ttvfig,ttv_getsigname(ttvfig,
                                                           signame,ptsig),
                                   ttv_revect(yagGetName(ifl,losig)),tas_getcapa(lofig,losig),
                                   type|TTV_SIG_S,esig) ;
                ttvfig->NBESIG ++ ;
                ptsigx = (ttvsig_list *)esig->DATA ;
                ptsig->USER = addptype(ptsig->USER,TAS_SIG_NEWSIG,
                                      (void*)ptsigx) ;
               }
              else if((type & TTV_SIG_I) == TTV_SIG_I)
               {
                isig = ttv_addrefsig(ttvfig,ttv_getsigname(ttvfig,
                                                           signame,ptsig),
                                  ttv_revect(yagGetName(ifl,losig)),tas_getcapa(lofig,losig),
                                  type,isig) ;
                ptsigx = (ttvsig_list *)isig->DATA ;
                ptsig->USER = addptype(ptsig->USER,TAS_SIG_NEWSIG,
                                       (void*)ptsigx) ;
               }
              else
               {
                bsig = ttv_addrefsig(ttvfig,ttv_getsigname(ttvfig,
                                                           signame,ptsig),
                                   ttv_revect(yagGetName(ifl,losig)),tas_getcapa(lofig,losig),
                                   type,bsig) ;
                ptsigx = (ttvsig_list *)bsig->DATA ;
                ptsig->USER = addptype(ptsig->USER,TAS_SIG_NEWSIG,
                                       (void*)ptsigx) ;
               }
             }
            else
             {
              type = tas_gettypesig(losig) ;
              if((type & (TTV_SIG_I|TTV_SIG_B)) == 0)
               {
                ptsigx = ttv_addsig(ttvfig,ttv_getsigname(ttvfig,signame,ptsig),
                                   ttv_revect(yagGetName(ifl,losig)),tas_getcapa(lofig,losig),
                                    TTV_SIG_S|type) ;
               }
              else if((type & TTV_SIG_I) == TTV_SIG_I)
               {
                isig = ttv_addrefsig(ttvfig,ttv_getsigname(ttvfig,signame,ptsig),
                                   ttv_revect(yagGetName(ifl,losig)),tas_getcapa(lofig,losig),
                                   type,isig) ;
                ptsigx = (ttvsig_list *)isig->DATA ;
               }
              else
               {
                bsig = ttv_addrefsig(ttvfig,ttv_getsigname(ttvfig,signame,ptsig),
                                   ttv_revect(yagGetName(ifl,losig)),tas_getcapa(lofig,losig),
                                   type,bsig) ;
                ptsigx = (ttvsig_list *)bsig->DATA ;
               }
              ptsig->USER = addptype(ptsig->USER,TAS_SIG_NEWSIG,(void*)ptsigx) ;
             }
            ptsig->TYPE &= ~(TTV_SIG_MARQUE) ;
           }
         }
      }
     else
      {
       for(chain = ptype->DATA ; chain != NULL ; chain = chain->NEXT)
         {
          locon = (locon_list *)chain->DATA ;
          if((locon->TYPE == INTERNAL) && 
             (getptype(locon->USER,TAS_LOCON_NSIG) == NULL))
           {
            if(locon->DIRECTION == UNKNOWN)
              continue ;
            ptsig = (ttvsig_list *)getptype(locon->USER,TAS_LOCON_SIG)->DATA ;
            type = tas_gettypesig(losig) ;
            type &= ~(TTV_SIG_I|TTV_SIG_B) ;
            ncsig = ttv_addrefsig(ttvfig,ttv_getsigname(ttvfig,signame,ptsig),
                                 ttv_revect(yagGetName(ifl,losig)),tas_getcapa(lofig,losig),
                                  TTV_SIG_N|type,ncsig) ;
            ptsigx = (ttvsig_list *)ncsig->DATA ;
            if(getptype(losig->USER,TAS_SIG_ONLYEND) != NULL)
              {
               ptsigx->NODE->TYPE |= TTV_NODE_ONLYEND ;
               (ptsigx->NODE+1)->TYPE |= TTV_NODE_ONLYEND ;
              }
            ttvfig->NBNCSIG ++ ;
            ptsig->USER = addptype(ptsig->USER,TAS_SIG_NEWSIG,(void*)ptsigx) ;
            if((ptsig->TYPE & TTV_SIG_Q) == TTV_SIG_Q)
              {
               qsig = addchain(qsig,(void*)ptsigx) ;
               ptsigx->TYPE |= TTV_SIG_Q ;
               ptsig->USER =  addptype(ptsig->USER,TTV_SIG_CMDNEW,ptsigx) ; 
               ptsigx->USER =  addptype(ptsigx->USER,TTV_SIG_CMDOLD,
                                        addchain(NULL,ptsig)) ; 
              }
            else if((ptsig->TYPE & TTV_SIG_B) == TTV_SIG_B)
              {
               bsig = addchain(bsig,(void*)ptsigx) ;
               ptsigx->TYPE |= TTV_SIG_B ;
              }
            else if((ptsig->TYPE & TTV_SIG_L) == TTV_SIG_L)
             {
              lsig = addchain(lsig,(void*)ptsigx) ;
              oldlatch = addchain(oldlatch,ptsig) ;
              if((ptsig->TYPE & TTV_SIG_LL) == TTV_SIG_LL)
                ptsigx->TYPE |= TTV_SIG_LL ;
              else
                ptsigx->TYPE |= TTV_SIG_LF ;
             }
           else if((ptsig->TYPE & TTV_SIG_R) == TTV_SIG_R)
             {
              rsig = addchain(rsig,(void*)ptsigx) ;
              ptsigx->TYPE |= TTV_SIG_R ;
             }
            ptsig->TYPE &= ~(TTV_SIG_MARQUE) ;
           }
         }
      }
    }
   else 
    {
     if(losig->TYPE == INTERNAL)
      {
       ptype_list *ptypex ;
       chain_list *chainx ;
       ttvsig_list *latch = NULL;
       char flag = 'N' ;
       char flagq = 'N' ;
       char flagl = 'N' ;
       char flagr = 'N' ;
       char flagb = 'N' ;
       char flagc = 'N' ;

       type = tas_gettypesig(losig) ;

       if((type & TTV_SIG_L) == TTV_SIG_L)
         flagl = 'Y' ;
       else if((type & TTV_SIG_R) == TTV_SIG_R)
         flagr = 'Y' ;
       else if((type & TTV_SIG_Q) == TTV_SIG_Q)
         flagq = 'Y' ;
       else if((type & TTV_SIG_EXT) == TTV_SIG_EXT)
         flag = 'Y' ;

       for(chain = ptype->DATA ; chain != NULL ; chain = chain->NEXT)
         {
          locon = (locon_list *)chain->DATA ;
          if(locon->TYPE != INTERNAL)
            continue ;
          flagc = 'Y' ;
          ptsigx = (ttvsig_list *)getptype(locon->USER,TAS_LOCON_SIG)->DATA ;
          if((ptsigx->TYPE & TTV_SIG_MARQUE) == TTV_SIG_MARQUE)
           {
            ptsigx->TYPE &= ~(TTV_SIG_MARQUE) ;
            flag = 'Y' ;
           }
          if((ptsigx->TYPE & TTV_SIG_Q) == TTV_SIG_Q)
            flagq = 'Y' ;
          if((ptsigx->TYPE & TTV_SIG_B) == TTV_SIG_B)
           {
            if(flagl == 'N')
              flagb = 'Y' ;
           }
          if((ptsigx->TYPE & TTV_SIG_L) == TTV_SIG_L)
           {
            if(flagl == 'Y') tas_error(59,ttv_revect(yagGetName(ifl,losig)),
                                       TAS_WARNING) ;
            flagl = 'Y' ;
            flagb = 'N' ;
            latch = ptsigx ;
           }
          if((ptsigx->TYPE & TTV_SIG_R) == TTV_SIG_R)
           {
            if(flagr == 'Y') tas_error(60,ttv_revect(yagGetName(ifl,losig)),
                                       TAS_WARNING) ;
            flagr = 'Y' ;
           }
          if((ptypex = getptype(locon->USER,TAS_LOCON_NSIG)) != NULL)
            {
             for(chainx = ptypex->DATA ; chainx != NULL ; chainx = chainx->NEXT)
              {
               ptsig = (ttvsig_list *)chainx->DATA ;
               if((ptsig->TYPE & TTV_SIG_MARQUE) == TTV_SIG_MARQUE)
                {
                 ptsig->TYPE &= ~(TTV_SIG_MARQUE) ;
                 flag = 'Y' ;
                }
               if((ptsig->TYPE & TTV_SIG_Q) == TTV_SIG_Q)
                 flagq = 'Y' ;
               if((ptsig->TYPE & TTV_SIG_B) == TTV_SIG_B)
                {
                 if(flagl == 'N')
                   flagb = 'Y' ;
                }
               if((ptsig->TYPE & TTV_SIG_L) == TTV_SIG_L)
                {
                 if(flagl == 'Y') tas_error(59,ttv_revect(yagGetName(ifl,losig)),
                                            TAS_WARNING) ;
                 flagl = 'Y' ;
                 flagb = 'N' ;
                 latch = ptsig ;
                }
               if((ptsig->TYPE & TTV_SIG_R) == TTV_SIG_R)
                {
                 if(flagr == 'Y') tas_error(60,ttv_revect(yagGetName(ifl,losig)),
                                            TAS_WARNING) ;
                 flagr = 'Y' ;
                }
              }
            }
         }
       if(flagq == 'Y')
         {
          type = tas_gettypesig(losig) ;
          type &= ~(TTV_SIG_I|TTV_SIG_B) ;
          qsig = ttv_addrefsig(ttvfig,ttv_revect(yagGetName(ifl,losig)),
                              ttv_revect(yagGetName(ifl,losig)),tas_getcapa(lofig,losig),
                               TTV_SIG_Q|type,qsig) ;
          ptsig = (ttvsig_list *)qsig->DATA ;
          ptsig->USER =  addptype(ptsig->USER,TTV_SIG_CMDOLD,NULL) ;
         }
       else if(flagb == 'Y')
         {
          type = tas_gettypesig(losig) ;
          type &= ~(TTV_SIG_I) ;
          bsig = ttv_addrefsig(ttvfig,ttv_revect(yagGetName(ifl,losig)),
                              ttv_revect(yagGetName(ifl,losig)),tas_getcapa(lofig,losig),
                               TTV_SIG_B|type,bsig) ;
          ptsig = (ttvsig_list *)bsig->DATA ;
         }
       else if(flagl == 'Y')
         {
          type = tas_gettypesig(losig) ;
          type &= ~(TTV_SIG_I|TTV_SIG_B) ;
          lsig = ttv_addrefsig(ttvfig,ttv_revect(yagGetName(ifl,losig)),
                              ttv_revect(yagGetName(ifl,losig)),tas_getcapa(lofig,losig),
                               TTV_SIG_L|type,lsig) ;
          ptsig = (ttvsig_list *)lsig->DATA ;
          oldlatch = addchain(oldlatch,ptsigx) ;
          if (latch != NULL){
             if((latch->TYPE & TTV_SIG_LL) == TTV_SIG_LL){
               ptsig->TYPE |= TTV_SIG_LL ;
             }else{
               ptsig->TYPE |= TTV_SIG_LF ;
             }
          }
         }
       else if(flagr == 'Y')
         {
          type = tas_gettypesig(losig) ;
          type &= ~(TTV_SIG_I|TTV_SIG_B) ;
          rsig = ttv_addrefsig(ttvfig,ttv_revect(yagGetName(ifl,losig)),
                              ttv_revect(yagGetName(ifl,losig)),tas_getcapa(lofig,losig),
                               TTV_SIG_R|type,rsig) ;
          ptsig = (ttvsig_list *)rsig->DATA ;
         }
       else if(flag == 'Y')
        {
         type = tas_gettypesig(losig) ;
         if((type & (TTV_SIG_I|TTV_SIG_B)) == 0)
          {
           esig = ttv_addrefsig(ttvfig,ttv_revect(yagGetName(ifl,losig)),
                              ttv_revect(yagGetName(ifl,losig)),tas_getcapa(lofig,losig),
                                type|TTV_SIG_S,esig) ;
           ptsig = (ttvsig_list *)esig->DATA ;
           ttvfig->NBESIG ++ ;
          }
         else if((type & TTV_SIG_I) == TTV_SIG_I)
          {
           isig = ttv_addrefsig(ttvfig,ttv_revect(yagGetName(ifl,losig)),
                              ttv_revect(yagGetName(ifl,losig)),tas_getcapa(lofig,losig),
                                type,isig) ;
           ptsig = (ttvsig_list *)isig->DATA ;
          }
         else
          {
           bsig = ttv_addrefsig(ttvfig,ttv_revect(yagGetName(ifl,losig)),
                              ttv_revect(yagGetName(ifl,losig)),tas_getcapa(lofig,losig),
                                type,bsig) ;
           ptsig = (ttvsig_list *)bsig->DATA ;
          }
        }
       else if(flagc == 'Y') 
       {
        type = tas_gettypesig(losig) ;
        if((type & (TTV_SIG_I|TTV_SIG_B)) == 0)
         {
          ptsig = ttv_addsig(ttvfig,ttv_revect(yagGetName(ifl,losig)),
                             ttv_revect(yagGetName(ifl,losig)),tas_getcapa(lofig,losig),
                             TTV_SIG_S|type) ;
         }
        else if((type & TTV_SIG_I) == TTV_SIG_I)
         {
          isig = ttv_addrefsig(ttvfig,ttv_revect(yagGetName(ifl,losig)),
                              ttv_revect(yagGetName(ifl,losig)),tas_getcapa(lofig,losig),
                               type,isig) ;
          ptsig = (ttvsig_list *)isig->DATA ;
         }
        else
         {
          bsig = ttv_addrefsig(ttvfig,ttv_revect(yagGetName(ifl,losig)),
                              ttv_revect(yagGetName(ifl,losig)),tas_getcapa(lofig,losig),
                               type,bsig) ;
          ptsig = (ttvsig_list *)bsig->DATA ;
         }
       }


       for(chain = ptype->DATA ; chain != NULL ; chain = chain->NEXT)
         {
          locon = (locon_list *)chain->DATA ;
          if(locon->TYPE != INTERNAL)
           {
            locon->USER = addptype(locon->USER,TAS_LOCON_SIG,ptsig) ;
            continue ;
           }
          ptsigx = (ttvsig_list *)getptype(locon->USER,TAS_LOCON_SIG)->DATA ;
          ptsigx->USER = addptype(ptsigx->USER,TAS_SIG_NEWSIG,(void*)ptsig) ;
          if(((ptsig->TYPE & TTV_SIG_Q) == TTV_SIG_Q) &&
             ((ptsigx->TYPE & TTV_SIG_Q) == TTV_SIG_Q))
           {
            ptsigx->USER =  addptype(ptsigx->USER,TTV_SIG_CMDNEW,(void*)ptsig) ;
            ptsig->USER->DATA = (chain_list *)addchain(
                                (chain_list *)ptsig->USER->DATA,(void*)ptsigx) ;
           }
          if((ptypex = getptype(locon->USER,TAS_LOCON_NSIG)) != NULL)
            {
             ptsigx = (ttvsig_list *)getptype(locon->USER,TAS_LOCON_SIG)->DATA ;
             ptsigx->TYPE |= TTV_SIG_MARQUE ;
             for(chainx = ptypex->DATA ; chainx != NULL ; chainx = chainx->NEXT)
              {
               ptsigx = (ttvsig_list *)chainx->DATA ;
               ptsigx->TYPE |= TTV_SIG_MARQUE ;
               ptsigx->USER = addptype(ptsigx->USER,TAS_SIG_NEWSIG,
                                       (void*)ptsig) ;
               if(((ptsig->TYPE & TTV_SIG_Q) == TTV_SIG_Q) &&
                  ((ptsigx->TYPE & TTV_SIG_Q) == TTV_SIG_Q))
                {
                 ptsigx->USER =  addptype(ptsigx->USER,TTV_SIG_CMDNEW,
                                          (void*)ptsig) ;
                 ptsig->USER->DATA = (chain_list *)addchain(
                                (chain_list *)ptsig->USER->DATA,(void*)ptsigx) ;
                }
              }
            }
         }
        if((ptypex = getptype(ptsig->USER,TTV_SIG_CMDOLD)) != NULL && ptypex->DATA==NULL)
          ptsig->USER=delptype(ptsig->USER,TTV_SIG_CMDOLD);
      }  
     else
      {
       ptype_list *ptypex ;
       chain_list *chainx ;
       ttvsig_list *latch = NULL;
       char flagq = 'N' ;
       char flagb = 'N' ;
       char flagl = 'N' ;
       char flagr = 'N' ;

       type = tas_gettypesig(losig) ;

       if((type & TTV_SIG_L) == TTV_SIG_L)
         flagl = 'Y' ;
       else if((type & TTV_SIG_R) == TTV_SIG_R)
         flagr = 'Y' ;
       else if((type & TTV_SIG_Q) == TTV_SIG_Q)
         flagq = 'Y' ;

       for(chain = ptype->DATA ; chain != NULL ; chain = chain->NEXT)
         {
          locon = (locon_list *)chain->DATA ;
          if(locon->TYPE == EXTERNAL)
            ptsig = (ttvsig_list *)getptype(locon->USER,TAS_LOCON_SIG)->DATA ;
          else if(locon->TYPE != INTERNAL)
            continue ;
          ptsigx = (ttvsig_list *)getptype(locon->USER,TAS_LOCON_SIG)->DATA ;
          ptsigx->TYPE &= ~(TTV_SIG_MARQUE) ;
          if((ptsigx->TYPE & TTV_SIG_Q) == TTV_SIG_Q)
            flagq = 'Y' ;
          else if((ptsigx->TYPE & TTV_SIG_B) == TTV_SIG_B)
            {
             if(flagl == 'N')
              flagb = 'Y' ;
            }
          else if((ptsigx->TYPE & TTV_SIG_L) == TTV_SIG_L)
           {
            if(flagl == 'Y') tas_error(59,ttv_revect(yagGetName(ifl,losig)),
                                       TAS_WARNING) ;
            flagl = 'Y' ;
            flagb = 'N' ;
            latch = ptsigx ;
           }
          else if((ptsigx->TYPE & TTV_SIG_R) == TTV_SIG_R)
           {
            if(flagr == 'Y') tas_error(60,ttv_revect(yagGetName(ifl,losig)),
                                       TAS_WARNING) ;
            flagr = 'Y' ;
           }
          if((ptypex = getptype(locon->USER,TAS_LOCON_NSIG)) != NULL)
            {
             for(chainx = ptypex->DATA ; chainx != NULL ; chainx = chainx->NEXT)
              {
               ptsigx = (ttvsig_list *)chainx->DATA ;
               if((ptsigx->TYPE & TTV_SIG_Q) == TTV_SIG_Q)
                 flagq = 'Y' ;
               if((ptsigx->TYPE & TTV_SIG_B) == TTV_SIG_B)
                 {
                  if(flagl == 'N')
                    flagb = 'Y' ;
                 }
               if((ptsigx->TYPE & TTV_SIG_L) == TTV_SIG_L)
                {
                 if(flagl == 'Y') tas_error(59,ttv_revect(yagGetName(ifl,losig)),
                                            TAS_WARNING) ;
                 flagl = 'Y' ;
                 flagb = 'N' ;
                 latch = ptsigx ;
                }
               if((ptsigx->TYPE & TTV_SIG_R) == TTV_SIG_R)
                {
                 if(flagr == 'Y') tas_error(60,ttv_revect(yagGetName(ifl,losig)),
                                            TAS_WARNING) ;
                 flagr = 'Y' ;
                }
              }
            }
         }

       if(flagq == 'Y')
         {
          qsig = addchain(qsig,(void*)ptsig) ;
          ptsig->TYPE |= TTV_SIG_Q ;
          ptsig->USER =  addptype(ptsig->USER,TTV_SIG_CMDOLD,NULL) ;
         }
       if(flagb == 'Y')
         {
          bsig = addchain(bsig,(void*)ptsig) ;
          ptsig->TYPE |= TTV_SIG_B ;
         }
       else if(flagl == 'Y')
         {
          lsig = addchain(lsig,(void*)ptsig) ;
          oldlatch = addchain(oldlatch,latch) ;
          if (latch != NULL){
             if((latch->TYPE & TTV_SIG_LL) == TTV_SIG_LL){
               ptsig->TYPE |= TTV_SIG_LL ;
             }else{
               ptsig->TYPE |= TTV_SIG_LF ;
             }
          }
          ptsig->TYPE |= TTV_SIG_L ;
         }
       else if(flagr == 'Y')
         {
          rsig = addchain(rsig,(void*)ptsig) ;
          ptsig->TYPE |= TTV_SIG_R ;
         }

       for(chain = ptype->DATA ; chain != NULL ; chain = chain->NEXT)
         {
          locon = (locon_list *)chain->DATA ;
          if(locon->TYPE == INTERNAL)
           {
            ptsigx = (ttvsig_list *)getptype(locon->USER,TAS_LOCON_SIG)->DATA ;
            ptsigx->USER = addptype(ptsigx->USER,TAS_SIG_NEWSIG,(void*)ptsig) ;
            if(((ptsig->TYPE & TTV_SIG_Q) == TTV_SIG_Q) &&
               ((ptsigx->TYPE & TTV_SIG_Q) == TTV_SIG_Q))
             {
              ptsigx->USER = addptype(ptsigx->USER,TTV_SIG_CMDNEW,
                                      (void*)ptsig) ;
              ptsig->USER->DATA = (chain_list *)addchain(
                                (chain_list *)ptsig->USER->DATA,(void*)ptsigx) ;
             }
            if((ptypex = getptype(locon->USER,TAS_LOCON_NSIG)) != NULL)
              {
               ptsigx = (ttvsig_list *)getptype(locon->USER,
                                                TAS_LOCON_SIG)->DATA ;
               ptsigx->TYPE |= TTV_SIG_MARQUE ;
               for(chainx = ptypex->DATA ; chainx != NULL ; 
                                           chainx = chainx->NEXT)
                {
                 ptsigx = (ttvsig_list *)chainx->DATA ;
                 ptsigx->TYPE |= TTV_SIG_MARQUE ;
                 ptsigx->USER = addptype(ptsigx->USER,TAS_SIG_NEWSIG,
                                         (void*)ptsig) ;
                 if(((ptsig->TYPE & TTV_SIG_Q) == TTV_SIG_Q) &&
                    ((ptsigx->TYPE & TTV_SIG_Q) == TTV_SIG_Q))
                  {
                   ptsigx->USER =  addptype(ptsigx->USER,TTV_SIG_CMDNEW,
                                            (void*)ptsig) ;
                   ptsig->USER->DATA = (chain_list *)addchain(
                                (chain_list *)ptsig->USER->DATA,(void*)ptsigx) ;
                  }
                }
              }
           }
         }
        if((ptypex = getptype(ptsig->USER,TTV_SIG_CMDOLD)) != NULL && ptypex->DATA==NULL)
          ptsig->USER=delptype(ptsig->USER,TTV_SIG_CMDOLD);

      }  
    }
  }

 ttvfig->NCSIG = (ttvsig_list **)ncsig ;
 ttvfig->ELCMDSIG = (ttvsig_list **)qsig ;
 ttvfig->EBREAKSIG = (ttvsig_list **)bsig ;
 ttvfig->ELATCHSIG = (ttvsig_list **)lsig ;
 ttvfig->EPRESIG = (ttvsig_list **)rsig ;
 ttvfig->EXTSIG = (ttvsig_list **)isig ;
 ttvfig->ESIG = (ttvsig_list **)esig ;
 ttvfig->STATUS |= (TTV_STS_CLS|TTV_STS_HEAD) ;

 for(chain = oldlatch ; chain != NULL ; chain = chain->NEXT)
    {
      ptsig = (ttvsig_list *)chain->DATA ;
      if((ptsigx=tas_getcorrespnewsig(ptsig))!= NULL)
       {
         if ((ptsigcmd=tas_getsignalcmdlist(ptsig))!=NULL)
         {
           ptsigxcmd=tas_getsignalcmdlist(ptsigx) ;
           for(chainx = ptsigcmd ; chainx != NULL ; chainx = chainx->NEXT)
             {
               tev=(ttvevent_list *)chainx->DATA;
               if ((correspsig=tas_getcorrespnewsig(tev->ROOT))!=NULL)
                 {
                    if ((tev->TYPE & TTV_NODE_UP)==TTV_NODE_UP)
                       tev=correspsig->NODE+1;
                    else
                       tev=correspsig->NODE;
                    for (cl=ptsigxcmd; cl!=NULL && cl->DATA!=tev; cl=cl->NEXT) ;
                    if (cl==NULL)
                       ptsigxcmd=tas_addsignalcmd(ptsigx, tev);
                 }
              }
          }
       }
    }
 freechain(oldlatch) ;
}

/*****************************************************************************/
/*                        function tas_builtrcxview()                        */
/* parametres :                                                              */
/* lofig : lofig de depart                                                   */
/* ttvfig : ttvfig correspondantes a la lofig                                */
/*                                                                           */
/* construit le signaux rcx pour le calcul des delay d'interconnexion        */
/*****************************************************************************/
void tas_builtrcxview(lofig,ttvfig)
lofig_list *lofig ;
ttvfig_list *ttvfig ;
{
 ttvfig_list *ttvins ;
 ttvsig_list *ptsig ;
 loins_list *loins ;
 locon_list *locon ;
 losig_list *losig ;
 lotrs_list *lotrs ;
 rcx_list *ptrcx ;
 ptype_list *ptype, *insuser ;
 chain_list *chain ;
 chain_list *chainx ;
 long i ;
 char signame[1024] ;
 char *name ;
 ht *htab ;
 int nb ;
 int typelosig ;

 nb = 0 ;

 if(ttvfig != NULL)
 for(chain = ttvfig->INS ; chain != NULL ; chain = chain->NEXT)
  {
   ttvins = (ttvfig_list *)chain->DATA ;
   nb += ttvins->NBNCSIG ;
  }

 if((nb != 0) && (ttvfig != NULL))
  {
   htab = addht(nb) ;

   for(chain = ttvfig->INS ; chain != NULL ; chain = chain->NEXT)
    {
     ttvins = (ttvfig_list *)chain->DATA ;
     for(i = 0 ; i < ttvins->NBNCSIG ; i++)
      {
       ptsig = *(ttvins->NCSIG + i) ;
       name = namealloc(ttv_getsigname(ttvfig,signame,ptsig)) ;
       addhtitem(htab,name,(long)ptsig) ;
      }
    }
  }

 if((nb != 0) && (ttvfig != NULL))
 for(loins = lofig->LOINS ; loins != NULL ; loins = loins->NEXT)
 {
  insuser=getptype(loins->USER, FCL_TRANSFER_PTYPE);
  if (insuser!=NULL && ((long)insuser->DATA & FCL_NEVER) != 0) continue;

 for(locon = loins->LOCON ; locon != NULL ; locon = locon->NEXT)
  {
   ptype = getptype(locon->USER,PTYPERCX) ;
   if(ptype != NULL)
    {
     losig = (losig_list*)ptype->DATA;
     ptrcx = getrcx( losig );
     for(chainx = ptrcx->RCXINTERNAL ; chainx != NULL ; chainx = chainx->NEXT)
       if((ptsig = (ttvsig_list *)gethtitem(htab,
           ttv_revect((getloconrcxname((locon_list*)chainx->DATA))))) != 
           (ttvsig_list *)EMPTYHT)
        {
         ptsig->USER = addptype(ptsig->USER,TAS_SIG_LOCON,chainx->DATA) ;
         ptsig->USER = addptype(ptsig->USER,TAS_SIG_LOCONEXT,locon) ;

         if((ptype = getptype(locon->USER,TAS_LOCON_NSIG)) == NULL)
          {
           locon->USER = addptype(locon->USER,TAS_LOCON_NSIG,
                               (void*)addchain(NULL,ptsig)) ;
          }
         else
          {
           ptype->DATA = (void*)addchain((chain_list *)ptype->DATA,ptsig) ;
          }
        }
    }
  }
  }
 if((nb != 0) && (ttvfig != NULL))
  {
   delht(htab) ;
   for(chain = ttvfig->INS ; chain != NULL ; chain = chain->NEXT)
    {
     ttvins = (ttvfig_list *)chain->DATA ;
     for(i = 0 ; i < ttvins->NBNCSIG ; i++)
      {
       ptsig = *(ttvins->NCSIG + i) ;
       if(getptype(ptsig->USER,TAS_SIG_LOCON) == NULL)
         tas_error(53,ptsig->NAME,TAS_ERROR) ;
      }
    }
  }

 if(ttvfig != NULL)
 for(loins = lofig->LOINS ; loins != NULL ; loins = loins->NEXT)
 {
  insuser=getptype(loins->USER, FCL_TRANSFER_PTYPE);
  if (insuser!=NULL && ((long)insuser->DATA & FCL_NEVER) != 0) continue;

 if(is_genius_instance(lofig,loins->INSNAME) == 0)
 for(locon = loins->LOCON ; locon != NULL ; locon = locon->NEXT)
  {
   if(locon->DIRECTION == TAS_ALIM) continue ;

   if(getptype(locon->USER,PTYPERCX) == NULL)
     {
      ptsig = (ttvsig_list *)getptype(locon->USER,TAS_LOCON_SIG)->DATA ;
      switch( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE ) {
      case ELP_CAPA_LEVEL0 :
        rcx_setloconcapa_l0( lofig, NULL, locon, TRC_FULL, (float)ptsig->CAPA/1000.0,
                                                           (float)ptsig->CAPA/1000.0 );
        break;
      case ELP_CAPA_LEVEL1 :
        rcx_setloconcapa_l1( lofig, NULL, locon, TRC_FULL, (float)ptsig->CAPA/1000.0,
                                                           (float)ptsig->CAPA/1000.0,
                                                           (float)ptsig->CAPA/1000.0,
                                                           (float)ptsig->CAPA/1000.0 );
        break;
      case ELP_CAPA_LEVEL2 :
        rcx_setloconcapa_l2( lofig, NULL, locon, TRC_FULL, (float)ptsig->CAPA/1000.0,
                                                           (float)ptsig->CAPA/1000.0, 
                                                           (float)ptsig->CAPA/1000.0, 
                                                           (float)ptsig->CAPA/1000.0, 
                                                           (float)ptsig->CAPA/1000.0, 
                                                           (float)ptsig->CAPA/1000.0, 
                                                           (float)ptsig->CAPA/1000.0, 
                                                           (float)ptsig->CAPA/1000.0  );
        break;
      }
      elpAddCapaSig( lofig, locon->SIG, (float)ptsig->CAPA/1000.0, ELP_CAPA_TYPICAL ) ;
     }
  }
 }
 for(losig = lofig->LOSIG ; losig != NULL ; losig = losig->NEXT)
  {
   if(losig->TYPE == TAS_ALIM)
     continue ;

   if(!rcx_isvisiblesignal(losig) )
     continue ;

   ptrcx = getrcx( losig );

   if(ptrcx == NULL)
    {
     if(getptype(losig->USER,TAS_SIG_NORCDELAY) == NULL)
     losig->USER = addptype(losig->USER,TAS_SIG_NORCDELAY,NULL) ;
     continue;
    }

   if (getptype(losig->USER, TAS_SIG_FORCERCDELAY)!=NULL)
    {
      rcx_forcercdelay(losig) ;
      losig->USER=delptype(losig->USER, TAS_SIG_FORCERCDELAY);
    }
   if( (ptype=getptype(losig->USER,TAS_SIG_NORCDELAY)) != NULL)
    {
     if( ptype->DATA != (void*)0x1 ) 
       setrcxmodel(lofig,losig,RCX_NOR) ;
    }

   ptype = getptype(losig->USER,LOFIGCHAIN) ;
   if(ptype == NULL)
    continue ;

   for(chain = (chain_list *)ptype->DATA ; chain != NULL ; 
       chain = chain->NEXT)
    {
     locon = (locon_list *)chain->DATA ;
     if(locon->TYPE != 'T')
      continue ;
     lotrs = (lotrs_list *)locon->ROOT ;
     if(locon->NAME == MBK_DRAIN_NAME)
      {
       if((lotrs->PD != 0) || (lotrs->XD != 0) || (lotrs->WIDTH != 0))
        {
         //capadrain = elpLotrsCapaDrain(lotrs,TAS_CASE);
         switch( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE ) { 
         case ELP_CAPA_LEVEL0 :
           if ( !mcc_use_multicorner() )
           rcx_setloconcapa_l0(lofig,NULL,locon, TRC_FULL,
                               elpLotrsCapaDrain(lotrs,ELP_CAPA_TYPICAL,elpTYPICAL),
                               elpLotrsCapaDrain(lotrs,ELP_CAPA_TYPICAL,elpTYPICAL)
                               ) ;
           else
           rcx_setloconcapa_l0(lofig,NULL,locon, TRC_FULL,
                               elpLotrsCapaDrain(lotrs,ELP_CAPA_TYPICAL,elpBEST),
                               elpLotrsCapaDrain(lotrs,ELP_CAPA_TYPICAL,elpWORST)
                               ) ;
           break;
         case ELP_CAPA_LEVEL1 :
            if ( !mcc_use_multicorner() )
           rcx_setloconcapa_l1(lofig,NULL,locon,TRC_FULL,
                                                elpLotrsCapaDrain(lotrs,ELP_CAPA_UP,elpTYPICAL),
                                                elpLotrsCapaDrain(lotrs,ELP_CAPA_UP,elpTYPICAL),
                                                elpLotrsCapaDrain(lotrs,ELP_CAPA_DN,elpTYPICAL),
                                                elpLotrsCapaDrain(lotrs,ELP_CAPA_DN,elpTYPICAL)  ) ;
           else
           rcx_setloconcapa_l1(lofig,NULL,locon,TRC_FULL,
                                                elpLotrsCapaDrain(lotrs,ELP_CAPA_UP,elpBEST),
                                                elpLotrsCapaDrain(lotrs,ELP_CAPA_UP,elpWORST),
                                                elpLotrsCapaDrain(lotrs,ELP_CAPA_DN,elpBEST),
                                                elpLotrsCapaDrain(lotrs,ELP_CAPA_DN,elpWORST) );
           break;
         case ELP_CAPA_LEVEL2 :
            if ( !mcc_use_multicorner() )
           rcx_setloconcapa_l2(lofig,NULL,locon,TRC_FULL,
                                                elpLotrsCapaDrain(lotrs,ELP_CAPA_UP_MIN,elpTYPICAL),
                                                elpLotrsCapaDrain(lotrs,ELP_CAPA_UP,elpTYPICAL),  
                                                elpLotrsCapaDrain(lotrs,ELP_CAPA_UP,elpTYPICAL),  
                                                elpLotrsCapaDrain(lotrs,ELP_CAPA_UP_MAX,elpTYPICAL),
                                                elpLotrsCapaDrain(lotrs,ELP_CAPA_DN_MIN,elpTYPICAL),
                                                elpLotrsCapaDrain(lotrs,ELP_CAPA_DN,elpTYPICAL),
                                                elpLotrsCapaDrain(lotrs,ELP_CAPA_DN,elpTYPICAL),
                                                elpLotrsCapaDrain(lotrs,ELP_CAPA_DN_MAX,elpTYPICAL) ) ;
           else
           rcx_setloconcapa_l2(lofig,NULL,locon,TRC_FULL,
                                                elpLotrsCapaDrain(lotrs,ELP_CAPA_UP_MIN,elpBEST),
                                                elpLotrsCapaDrain(lotrs,ELP_CAPA_UP,elpBEST),  
                                                elpLotrsCapaDrain(lotrs,ELP_CAPA_UP,elpWORST),  
                                                elpLotrsCapaDrain(lotrs,ELP_CAPA_UP_MAX,elpWORST),
                                                elpLotrsCapaDrain(lotrs,ELP_CAPA_DN_MIN,elpBEST),
                                                elpLotrsCapaDrain(lotrs,ELP_CAPA_DN,elpBEST),
                                                elpLotrsCapaDrain(lotrs,ELP_CAPA_DN,elpWORST),
                                                elpLotrsCapaDrain(lotrs,ELP_CAPA_DN_MAX,elpWORST) ) ;
           break;
         }
        }
       locon->DIRECTION = OUT ;
      }
     if(locon->NAME == MBK_SOURCE_NAME)
      {
       if((lotrs->PS >= 0) || (lotrs->XS >= 0) || (lotrs->WIDTH != 0))
        {
         //capasource = elpLotrsCapaSource(lotrs,TAS_CASE);
         switch( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE ) { 
         case ELP_CAPA_LEVEL0 :
           if ( !mcc_use_multicorner() )
           rcx_setloconcapa_l0(lofig,NULL,locon,TRC_FULL,
                               elpLotrsCapaSource(lotrs,ELP_CAPA_TYPICAL,elpTYPICAL),
                               elpLotrsCapaSource(lotrs,ELP_CAPA_TYPICAL,elpTYPICAL)
                               ) ;
           else
           rcx_setloconcapa_l0(lofig,NULL,locon,TRC_FULL,
                               elpLotrsCapaSource(lotrs,ELP_CAPA_TYPICAL,elpBEST),
                               elpLotrsCapaSource(lotrs,ELP_CAPA_TYPICAL,elpWORST)
                               ) ;
           break;
         case ELP_CAPA_LEVEL1 :
            if ( !mcc_use_multicorner() )
           rcx_setloconcapa_l1(lofig,NULL,locon,TRC_FULL,
                                                elpLotrsCapaSource(lotrs,ELP_CAPA_UP,elpTYPICAL),
                                                elpLotrsCapaSource(lotrs,ELP_CAPA_UP,elpTYPICAL),
                                                elpLotrsCapaSource(lotrs,ELP_CAPA_DN,elpTYPICAL),
                                                elpLotrsCapaSource(lotrs,ELP_CAPA_DN,elpTYPICAL)  ) ;
           else
           rcx_setloconcapa_l1(lofig,NULL,locon,TRC_FULL,
                                                elpLotrsCapaSource(lotrs,ELP_CAPA_UP,elpBEST),
                                                elpLotrsCapaSource(lotrs,ELP_CAPA_UP,elpWORST),
                                                elpLotrsCapaSource(lotrs,ELP_CAPA_DN,elpBEST),
                                                elpLotrsCapaSource(lotrs,ELP_CAPA_DN,elpWORST) );
           break;
         case ELP_CAPA_LEVEL2 :
            if ( !mcc_use_multicorner() )
           rcx_setloconcapa_l2(lofig,NULL,locon,TRC_FULL,
                                                elpLotrsCapaSource(lotrs,ELP_CAPA_UP_MIN,elpTYPICAL),
                                                elpLotrsCapaSource(lotrs,ELP_CAPA_UP,elpTYPICAL),  
                                                elpLotrsCapaSource(lotrs,ELP_CAPA_UP,elpTYPICAL),  
                                                elpLotrsCapaSource(lotrs,ELP_CAPA_UP_MAX,elpTYPICAL),
                                                elpLotrsCapaSource(lotrs,ELP_CAPA_DN_MIN,elpTYPICAL),
                                                elpLotrsCapaSource(lotrs,ELP_CAPA_DN,elpTYPICAL),
                                                elpLotrsCapaSource(lotrs,ELP_CAPA_DN,elpTYPICAL),
                                                elpLotrsCapaSource(lotrs,ELP_CAPA_DN_MAX,elpTYPICAL) ) ;
           else
           rcx_setloconcapa_l2(lofig,NULL,locon,TRC_FULL,
                                                elpLotrsCapaSource(lotrs,ELP_CAPA_UP_MIN,elpBEST),
                                                elpLotrsCapaSource(lotrs,ELP_CAPA_UP,elpBEST),  
                                                elpLotrsCapaSource(lotrs,ELP_CAPA_UP,elpWORST),  
                                                elpLotrsCapaSource(lotrs,ELP_CAPA_UP_MAX,elpWORST),
                                                elpLotrsCapaSource(lotrs,ELP_CAPA_DN_MIN,elpBEST),
                                                elpLotrsCapaSource(lotrs,ELP_CAPA_DN,elpBEST),
                                                elpLotrsCapaSource(lotrs,ELP_CAPA_DN,elpWORST),
                                                elpLotrsCapaSource(lotrs,ELP_CAPA_DN_MAX,elpWORST) ) ;
           break;
         }
        }
       locon->DIRECTION = OUT ;
      }
     if(locon->NAME == MBK_GRID_NAME)
      {
       if((lotrs->WIDTH != 0) && (lotrs->LENGTH != 0))
        {
         switch( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE ) { 
         case ELP_CAPA_LEVEL0 :
           if ( !mcc_use_multicorner() )
           rcx_setloconcapa_l0(lofig,NULL,locon,TRC_FULL,
                               elpLotrsInCapa(lofig,lotrs,ELP_CAPA_TYPICAL,elpTYPICAL,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),
                               elpLotrsInCapa(lofig,lotrs,ELP_CAPA_TYPICAL,elpTYPICAL,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE)
                               ) ;
           else
           rcx_setloconcapa_l0(lofig,NULL,locon,TRC_FULL,
                               elpLotrsInCapa(lofig,lotrs,ELP_CAPA_TYPICAL,elpBEST,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),
                               elpLotrsInCapa(lofig,lotrs,ELP_CAPA_TYPICAL,elpWORST,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE)
                               ) ;
           break;
         case ELP_CAPA_LEVEL1 :
           if ( !mcc_use_multicorner() ) {
             rcx_setloconcapa_l1(lofig,NULL,locon,TRC_HALF,
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_UP,elpTYPICAL,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_UP,elpTYPICAL,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_DN,elpTYPICAL,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_DN,elpTYPICAL,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE)  ) ;
             rcx_setloconcapa_l1(lofig,NULL,locon,TRC_END,
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_UPF,elpTYPICAL,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_UPF,elpTYPICAL,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_DNF,elpTYPICAL,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_DNF,elpTYPICAL,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE)  ) ;
           }
           else {
             rcx_setloconcapa_l1(lofig,NULL,locon,TRC_HALF,
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_UP,elpBEST,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_UP,elpWORST,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_DN,elpBEST,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_DN,elpWORST,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE) );
             rcx_setloconcapa_l1(lofig,NULL,locon,TRC_END,
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_UPF,elpBEST,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_UPF,elpWORST,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_DNF,elpBEST,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_DNF,elpWORST,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE) );
           }
           break;
         case ELP_CAPA_LEVEL2 :
           if ( !mcc_use_multicorner() ) {
             rcx_setloconcapa_l2(lofig,NULL,locon,TRC_HALF,
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_UP_MIN,elpTYPICAL,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_UP,elpTYPICAL,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),  
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_UP,elpTYPICAL,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),  
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_UP_MAX,elpTYPICAL,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_DN_MIN,elpTYPICAL,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_DN,elpTYPICAL,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_DN,elpTYPICAL,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_DN_MAX,elpTYPICAL,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE) ) ;
             rcx_setloconcapa_l2(lofig,NULL,locon,TRC_END,
                                                  -1.0,
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_UPF,elpTYPICAL,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),  
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_UPF,elpTYPICAL,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),  
                                                  -1.0,
                                                  -1.0,
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_DNF,elpTYPICAL,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_DNF,elpTYPICAL,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),
                                                  -1.0 ) ;
           }
           else {
             rcx_setloconcapa_l2(lofig,NULL,locon,TRC_HALF,
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_UP_MIN,elpBEST,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_UP,elpBEST,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),  
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_UP,elpWORST,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),  
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_UP_MAX,elpWORST,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_DN_MIN,elpBEST,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_DN,elpBEST,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_DN,elpWORST,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_DN_MAX,elpWORST,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE) ) ;
             rcx_setloconcapa_l2(lofig,NULL,locon,TRC_END,
                                                  -1.0,
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_UPF,elpBEST,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),  
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_UPF,elpWORST,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),  
                                                  -1.0,
                                                  -1.0,
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_DNF,elpBEST,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),
                                                  elpLotrsInCapa(lofig,lotrs,ELP_CAPA_DNF,elpWORST,V_BOOL_TAB[__AVT_CORRECT_CGP].VALUE),
                                                  -1.0 ) ;
           }
           break;
         }
        }
       locon->DIRECTION = IN ;
      }
    }

   if(getptype(losig->USER,TAS_SIG_NORCDELAY) != NULL)
     typelosig = setrcxmodel(lofig,losig,RCX_NOR) ;
   else
     typelosig = setrcxmodel(lofig,losig,RCX_BEST) ;

   if((typelosig == RCX_NOR) || (typelosig == RCX_NORCTC))
    {
     if(getptype(losig->USER,TAS_SIG_NORCDELAY) == NULL)
      losig->USER = addptype(losig->USER,TAS_SIG_NORCDELAY,NULL) ;
     continue ;
    }

   if( TAS_CONTEXT->TAS_BREAKLOOP == 'Y' )
     rcx_setlosigbreakloop( losig );
  }
}

/*****************************************************************************/
/*                        function tas_calcullinetime()                      */
/* parametres :                                                              */
/* ptline : lien                                                             */
/* type : type max ou min                                                    */
/*                                                                           */
/* calcul les nouveaux delais d'un lien T ou D                               */
/*****************************************************************************/
long tas_calcullinetime(ptline,type)
ttvline_list *ptline ;
long type ;
{
  if(((type & TTV_FIND_MAX) == TTV_FIND_MAX) && (ptline->VALMAX != TTV_NOTIME))
      return ptline->VALMAX;
  else if(((type & TTV_FIND_MIN) == TTV_FIND_MIN) && (ptline->VALMIN != TTV_NOTIME))
      return ptline->VALMIN;
  else
      return TTV_NOTIME;
    
 return(TTV_NOTIME) ;
}

/*****************************************************************************/
/*                        function tas_calcullineslope()                     */
/* parametres :                                                              */
/* ptline : lien                                                             */
/* type : type max ou min                                                    */
/*                                                                           */
/* calcul les nouveaux fronts d'un lien T ou D                               */
/*****************************************************************************/
long tas_calcullineslope(ptline,type)
ttvline_list *ptline ;
long type ;
{
  if(((type & TTV_FIND_MAX) == TTV_FIND_MAX) && (ptline->FMAX != TTV_NOSLOPE))
      return ptline->FMAX;
  else if(((type & TTV_FIND_MIN) == TTV_FIND_MIN) && (ptline->FMIN != TTV_NOSLOPE))
      return ptline->FMIN;
  else
      return TTV_NOSLOPE;
    
 return(TTV_NOSLOPE) ;
}

/*****************************************************************************/
/*                        function tas_gettypeline()                         */
/* parametres :                                                              */
/* ptline : lien                                                             */
/*                                                                           */
/* determine le type du nouveau lien T ou D                                  */
/*****************************************************************************/
long tas_gettypeline(ptline)
ttvline_list *ptline ;
{
 ttvsig_list *ptsigin = NULL ;
 ttvsig_list *ptsigout = NULL ;

 if(((ptline->NODE->ROOT->TYPE & TTV_SIG_C) == TTV_SIG_C) ||
    ((ptline->NODE->ROOT->TYPE & TTV_SIG_N) == TTV_SIG_N))
   ptsigin = (ttvsig_list*)getptype(ptline->NODE->ROOT->USER,
                                    TAS_SIG_NEWSIG)->DATA ;
 else 
   ptsigin = ptline->NODE->ROOT ;
   

 if(((ptline->ROOT->ROOT->TYPE & TTV_SIG_C) == TTV_SIG_C) ||
    ((ptline->ROOT->ROOT->TYPE & TTV_SIG_N) == TTV_SIG_N))
   ptsigout = (ttvsig_list*)getptype(ptline->ROOT->ROOT->USER,
                                    TAS_SIG_NEWSIG)->DATA ;
 else 
   ptsigout = ptline->ROOT->ROOT ;

 return(ttv_getnewlinetype(ptline,ptsigin,ptsigout)) ;
}

/*****************************************************************************/
/*                        function tas_getnodesig()                          */
/* parametres :                                                              */
/* ttvfig : ttvfig du noeud                                                  */
/* ptevent : evenement                                                       */
/*                                                                           */
/* renvoie le nouveau noeud                                                  */
/*****************************************************************************/
ttvevent_list *tas_getnodesig(ttvfig,ptevent)
ttvfig_list *ttvfig ;
ttvevent_list *ptevent ;
{
 ttvsig_list *ptsig ;
 ptype_list *ptype ;

 if(((ptevent->ROOT->TYPE & TTV_SIG_C) != TTV_SIG_C) &&
    ((ptevent->ROOT->TYPE & TTV_SIG_N) != TTV_SIG_N))
   return(ptevent) ;

 ptype = getptype(ptevent->ROOT->USER,TAS_SIG_NEWSIG) ;

 if(ptype == NULL) 
   {
    if(((ptevent->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0) &&
       (ptevent->ROOT->ROOT != ttvfig))
      return(NULL) ;
    else return(ptevent) ;
   }

 ptsig = (ttvsig_list *)ptype->DATA ;

 if((ptevent->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
    return(ptsig->NODE + 1) ;
 else
    return(ptsig->NODE) ;
}

/*****************************************************************************/
/*                        function tas_getnodesighz()                        */
/* parametres :                                                              */
/* ttvfig : ttvfig du noeud                                                  */
/* ptevent : evenement                                                       */
/*                                                                           */
/* renvoie le nouveau noeud                                                  */
/*****************************************************************************/
ttvevent_list *tas_getnodesighz(ttvfig,ptevent)
ttvfig_list *ttvfig ;
ttvevent_list *ptevent ;
{
 ttvevent_list *newevent ;
 ttvsig_list *ptsig ;
 locon_list *locon ;
 ptype_list *ptype ;
 chain_list *chain ;

 if(((ptevent->ROOT->TYPE & TTV_SIG_C) != TTV_SIG_C) &&
    ((ptevent->ROOT->TYPE & TTV_SIG_R) != TTV_SIG_R))
   return(NULL) ;

 newevent = tas_getnodesig(ttvfig,ptevent) ;

 if(newevent == NULL)
    return(NULL) ;

 if((ptevent->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R)
   return(newevent) ;

 if(((newevent->ROOT->TYPE & TTV_SIG_C) != TTV_SIG_C) &&
    ((newevent->ROOT->TYPE & TTV_SIG_N) != TTV_SIG_N))
   return(NULL) ;

 if((newevent->ROOT->TYPE & TTV_SIG_C) == TTV_SIG_C)
   return(newevent) ;

 ptype = getptype(ptevent->ROOT->USER,TAS_SIG_LOCON) ;

 if(ptype != NULL) 
   {
    locon = (locon_list *)ptype->DATA ;
    if(locon->SIG->TYPE == INTERNAL)
      return(NULL) ;
    ptype = getptype(locon->SIG->USER,TAS_LOFIGCHAIN) ;
    chain = NULL ;
    if(ptype != NULL)
    for(chain = (chain_list *)ptype->DATA ; chain != NULL ; chain = chain->NEXT)
      {
       locon = (locon_list *)chain->DATA ;
       if(locon->TYPE == EXTERNAL)
          break ;
      }
    if(chain != NULL)
     {
      ptype = getptype(locon->USER,TAS_LOCON_SIG) ;
      if(ptype != NULL)
       {
        ptsig = (ttvsig_list *)ptype->DATA ; 
        if((ptevent->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
          return(ptsig->NODE + 1) ;
        else
          return(ptsig->NODE) ;
       }
     }
   }

 return(NULL) ;
}

/*****************************************************************************/
/*                        function tas_calcaracline()                        */
/* parametres :                                                              */
/* ptline : evenement                                                        */
/* ptlinenew : evenement                                                     */
/*                                                                           */
/* calcul les caracteristique electriques des nouvelles line                 */
/*****************************************************************************/
void tas_calcaracline(ptline,ptlinenew,reset,type)
ttvline_list *ptline ;
ttvline_list *ptlinenew ;
int reset ;
long type ;
{
 static int index = 0 ;

 if(reset == 1) 
  {
   if(type)
       index = stm_get_last_index(((ttvfig_list*)type)->INFO->FIGNAME) ;
   else
       index = 0 ;
   return ;
  }
 else if(reset == -1) 
  {
   index = -1 ;
   return ;
  }

 index = ttv_calcaracline(ptline,ptlinenew,type,index) ;
}

/*****************************************************************************/
/*                        function tas_freercxfig()                          */
/* parametres :                                                              */
/*                                                                           */
/* ajoute les signaux à charger dans les fils                                */
/*****************************************************************************/
void tas_freercxfig(ttvfig)
ttvfig_list *ttvfig ;
{
 ttvfig_list *ttvins ;
 chain_list *chain ;

 for(chain = ttvfig->INS ; chain != NULL ; chain = chain->NEXT)
  {
   ttvins = (ttvfig_list *)chain->DATA ;
   ttv_delrcxlofig(ttvins) ;
   tas_freercxfig(ttvins) ;
  }
}

/*****************************************************************************/
/*                        function tas_loadrcxfig()                          */
/* parametres :                                                              */
/*                                                                           */
/* ajoute les signaux à charger dans les fils                                */
/*****************************************************************************/
void tas_loadrcxfig(ttvfig)
ttvfig_list *ttvfig ;
{
 ttvfig_list *ttvins ;
 chain_list *chain ;
 ptype_list *ptype ;
 lofig_list *lofig ;
 ht *htab ;

 for(chain = ttvfig->INS ; chain != NULL ; chain = chain->NEXT)
  {
   ttvins = (ttvfig_list *)chain->DATA ;
   if((ptype = getptype(ttvins->USER,TAS_FIG_LOSIGRCX)) != NULL)
      {
       htab = (ht *)ptype->DATA ;
       ptype = getptype(ttvins->INFO->USER,TTV_FIG_LOFIG) ;
       if(ptype != NULL) continue ;
       lofig = rcx_getlofig(ttvins->INFO->FIGNAME,htab) ;
       if(lofig)
        {
         locklofig(lofig) ;
         ttvins->INFO->USER = addptype(ttvins->INFO->USER,TTV_FIG_LOFIG,lofig) ;
        }
       ttvins->USER = delptype(ttvins->USER,TAS_FIG_LOSIGRCX) ;
       delht(htab) ;
      }
   tas_loadrcxfig(ttvins) ;
  }
}

/*****************************************************************************/
/*                        function tas_addsigrcxload()                       */
/* parametres :                                                              */
/*                                                                           */
/* ajoute les signaux à charger dans les fils                                */
/*****************************************************************************/
void tas_addsigrcxload(ttvsig)
ttvsig_list *ttvsig ;
{
 ptype_list *ptype ;
 ht *htab ;

 if((ptype = getptype(ttvsig->ROOT->USER,TAS_FIG_LOSIGRCX)) != NULL)
    {
     htab = (ht *)ptype->DATA ;
    }
 else
    {
     htab = addht(100) ;
     ttvsig->ROOT->USER = addptype(ttvsig->ROOT->USER,TAS_FIG_LOSIGRCX,htab) ;
    }

 sethtitem(htab,ttvsig->NETNAME,(long)1) ;
}

/*****************************************************************************/
/*                        function tas_builtline()                           */
/* parametres :                                                              */
/* ttvfig : ttvfig pere                                                      */
/* type : type de liens                                                      */
/*                                                                           */
/* traitement des liens T et D des fils pour construire le pere              */
/*****************************************************************************/
void tas_builtline(ttvfig,type)
ttvfig_list *ttvfig ;
long type ;
{
 ttvfig_list *ttvins ;
 ttvevent_list *rootx ;
 ttvevent_list *nodex ;
 ttvevent_list *root ;
 ttvevent_list *node, *cmdmin, *cmdmax ;
 ttvlbloc_list *ptlbloc ;
 ttvline_list *ptline ;
 ttvline_list *ptlinex ;
 ptype_list *ptype ;
 chain_list *chain ;
 long slopemax ;
 long slopemin ;
 long delaymax ;
 long delaymin ;
 long typel ;
 long i ;
 long maxline ;
 char lock ;

 maxline = TTV_MAX_LINE ;
 TTV_MAX_LINE = TTV_ALLOC_MAX ;

 for(chain = ttvfig->INS ; chain != NULL ; chain = chain->NEXT)
  {
   ttvins = (ttvfig_list *)chain->DATA ;

   if((ttvins->STATUS & TTV_STS_LOCK) != TTV_STS_LOCK)
    {
     ttv_lockttvfig(ttvins) ;
     lock = 'Y' ;
    }
   else
     lock = 'N' ;

   if((type & TTV_LINE_D) == TTV_LINE_D)
    {
     if((ttvins->STATUS & TTV_STS_D) != TTV_STS_D)
        ttv_parsttvfig(ttvins,TTV_STS_D,TTV_FILE_DTX) ;
     ptlbloc = ttvins->DBLOC ;
    }
   else if((type & TTV_LINE_T) == TTV_LINE_T)
    {
     if((ttvins->STATUS & TTV_STS_T) != TTV_STS_T)
        ttv_parsttvfig(ttvins,TTV_STS_T,TTV_FILE_TTX) ;

     ptlbloc = ttvins->TBLOC ;
    }

   for(; ptlbloc != NULL ; ptlbloc = ptlbloc->NEXT)
    for(i = 0 ; i < TTV_MAX_LBLOC ; i++)
      {
       ptline = ptlbloc->LINE + i ;
       if((ptline->TYPE & TTV_LINE_FR) == TTV_LINE_FR)
         continue ;

       if((ptline->TYPE & TTV_LINE_RC) == TTV_LINE_RC)
          continue ;

       node = ptline->NODE ;
       root = ptline->ROOT ;
       nodex = tas_getnodesig(ttvfig,node) ;
       if((ptline->TYPE & TTV_LINE_HZ) == TTV_LINE_HZ)
         rootx = tas_getnodesighz(ttvfig,root) ;
       else
         rootx = tas_getnodesig(ttvfig,root) ;

       if((nodex == NULL) || (rootx == NULL))
         continue ;

       tlc_setmaxfactor() ;
       delaymax = tas_calcullinetime(ptline,TTV_FIND_MAX) ;
       slopemax = tas_calcullineslope(ptline,TTV_FIND_MAX) ;
       tlc_setminfactor() ;
       delaymin = tas_calcullinetime(ptline,TTV_FIND_MIN) ;
       slopemin = tas_calcullineslope(ptline,TTV_FIND_MIN) ;
       typel = tas_gettypeline(ptline) ;

       if((type & TTV_LINE_D) == TTV_LINE_D)
        {

         if ((ptype = getptype(ptline->USER,TTV_LINE_CMDMAX))!=NULL) cmdmax=(ttvevent_list *)ptype->DATA;
         else cmdmax=NULL;
         if ((ptype = getptype(ptline->USER,TTV_LINE_CMDMIN))!=NULL) cmdmin=(ttvevent_list *)ptype->DATA;
         else cmdmin=NULL;

         for(ptlinex = rootx->INLINE ; ptlinex != NULL ; 
             ptlinex = ptlinex->NEXT)
          {
            ttvevent_list *thiscmdmax, *thiscmdmin;
            if ((ptype = getptype(ptlinex->USER,TTV_LINE_CMDMAX))!=NULL) thiscmdmax=(ttvevent_list *)ptype->DATA;
            else thiscmdmax=NULL;
            if ((ptype = getptype(ptlinex->USER,TTV_LINE_CMDMIN))!=NULL) thiscmdmin=(ttvevent_list *)ptype->DATA;
            else thiscmdmin=NULL;

           if((ptlinex->NODE == nodex) && (ptlinex->FIG == ttvfig))
            {
             if((ptlinex->TYPE & (TTV_LINE_A|TTV_LINE_U|TTV_LINE_O|
                                  TTV_LINE_HZ|TTV_LINE_R|TTV_LINE_S)) == 
                (typel &  (TTV_LINE_A|TTV_LINE_U|TTV_LINE_O|TTV_LINE_HZ|
                          TTV_LINE_R|TTV_LINE_S))
                && cmdmax==thiscmdmax && cmdmin==thiscmdmin
                )
             break ;     
            }
          }
         if(ptlinex != NULL)
           {
            if(ptlinex->VALMAX < delaymax)
             {
              ptlinex->VALMAX = delaymax ;
              ptlinex->FMAX = slopemax ;
              ptlinex->TYPE |= typel ;
              if((root->ROOT->TYPE & TTV_SIG_L) == TTV_SIG_L)
               {
                ttvevent_list *cmd ;
      
                ptype = getptype(ptline->USER,TTV_LINE_CMDMAX) ;
                if(ptype != NULL)
                 {
                  cmd = (ttvevent_list *)ptype->DATA ;
                  ptype = getptype(ptlinex->USER,TTV_LINE_CMDMAX) ;
                  if(ptype != NULL)
                    ptlinex->USER = delptype(ptlinex->USER,TTV_LINE_CMDMAX) ;
                  ttv_addcmd(ptlinex,TTV_LINE_CMDMAX,cmd) ;
                 }
               }
              tas_calcaracline(ptline,ptlinex,0,TTV_FIND_MAX) ;
             }
            if((ptlinex->VALMIN > delaymin) && (delaymin != TTV_NOTIME))
             {
              ptlinex->VALMIN = delaymin ;
              ptlinex->FMIN = slopemin ;
              ptlinex->TYPE |= typel ;
              if((root->ROOT->TYPE & TTV_SIG_L) == TTV_SIG_L)
               {
                ttvevent_list *cmd ;
              
                ptype = getptype(ptline->USER,TTV_LINE_CMDMIN) ;
                if(ptype != NULL)
                 {
                  cmd = (ttvevent_list *)ptype->DATA ;
                  ptype = getptype(ptlinex->USER,TTV_LINE_CMDMIN) ;
                  if(ptype != NULL)
                    ptlinex->USER = delptype(ptlinex->USER,TTV_LINE_CMDMIN) ;
                  ttv_addcmd(ptlinex,TTV_LINE_CMDMIN,cmd) ;
                 }
               }
              tas_calcaracline(ptline,ptlinex,0,TTV_FIND_MIN) ;
             }
            continue ;
           }
        }

       ptlinex = ttv_addline(ttvfig, rootx, nodex, delaymax, slopemax,
                             delaymin,slopemin,typel) ;

       if(ptlinex->ROOT->ROOT->ROOT != ttvfig) 
          tas_addsigrcxload(ptlinex->ROOT->ROOT) ;
         
       if(TAS_CONTEXT->TAS_FIND_MIN == 'Y')
          tas_calcaracline(ptline,ptlinex,0,TTV_FIND_MAX|TTV_FIND_MIN) ;
       else
          tas_calcaracline(ptline,ptlinex,0,TTV_FIND_MAX) ;

       if((root->ROOT->TYPE & TTV_SIG_L) == TTV_SIG_L)
         {
          ttvevent_list *cmd ;

          ptype = getptype(ptline->USER,TTV_LINE_CMDMAX) ;
          if(ptype != NULL)
           {
            cmd = (ttvevent_list *)ptype->DATA ;
            ttv_addcmd(ptlinex,TTV_LINE_CMDMAX,cmd) ;
           }
          ptype = getptype(ptline->USER,TTV_LINE_CMDMIN) ;
          if(ptype != NULL)
           {
            cmd = (ttvevent_list *)ptype->DATA ;
            ttv_addcmd(ptlinex,TTV_LINE_CMDMIN,cmd) ;
           }
         }
      }

   if(lock == 'Y')
     ttv_unlockttvfig(ttvins) ;

   if((type & TTV_LINE_D) == TTV_LINE_D)
    ttv_freememoryifmax(ttvins,TTV_STS_D) ;
   else if((type & TTV_LINE_T) == TTV_LINE_T)
    ttv_freememoryifmax(ttvins,TTV_LINE_T) ;
  }

 TTV_MAX_LINE = maxline ;

 if((type & TTV_LINE_D) == TTV_LINE_D)
  {
   ttvfig->STATUS |= (TTV_STS_CLS_FED) ;
  }
 else
  {
   ttvfig->STATUS |= (TTV_STS_CL_PJT);
  }
}

/*****************************************************************************/
/*                        function tas_calcrcnline()                         */
/* parametres :                                                              */
/* ttvfig : ttvfig pere                                                      */
/* losig : signal logique                                                    */
/* root : noeud d'extremite                                                  */
/* node : noeud d'origine                                                    */
/*                                                                           */
/* calcul un delai rc entre deux noeuds et ajoute un lien                    */
/*****************************************************************************/
int tas_calcrcnline(ttvfig,root,node,typefile)
ttvfig_list *ttvfig ;
ttvevent_list *root ;
ttvevent_list *node ;
long typefile ;
{
 ttvevent_list *rootx = tas_getnodesig(ttvfig,root) ;
 ttvevent_list *nodex = tas_getnodesig(ttvfig,node) ;
 ttvline_list *ptline ;
 long type ;
 int res = 1 ;

 if((typefile & TTV_FILE_DTX) == TTV_FILE_DTX)
  {
   if(((rootx->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0) ||
     ((nodex->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0))
    type = (TTV_LINE_D | TTV_LINE_RC) ;
   else
    type = (TTV_LINE_F | TTV_LINE_RC) ;
   ptline = ttv_addline(ttvfig,rootx,nodex,TTV_NOTIME,TTV_NOSLOPE,TTV_NOTIME,
                        TTV_NOSLOPE,type) ;
  }

 if((typefile & TTV_FILE_TTX) == TTV_FILE_TTX)
  {
   if(((rootx->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0) ||
     ((nodex->ROOT->TYPE & (TTV_SIG_C|TTV_SIG_N)) != 0))
    type = (TTV_LINE_T | TTV_LINE_RC) ;
   else
    type = (TTV_LINE_P | TTV_LINE_RC) ;
   ptline = ttv_addline(ttvfig,rootx,nodex,TTV_NOTIME,TTV_NOSLOPE,TTV_NOTIME,
                        TTV_NOSLOPE,type) ;
  }

 return(res) ;
}

/*****************************************************************************/
/*                        function tas_findrcnline()                         */
/* parametres :                                                              */
/* ttvfig : ttvfig pere                                                      */
/* losig : signal courant                                                    */
/* ptnode : noeud de depart                                                  */
/* ptnodeav : noeud precedent                                                */
/*                                                                           */
/* retrouve tous les liens rc d'un noeud                                     */
/*****************************************************************************/
int tas_findrcnline(ttvfig,losig,ptnode,ptnodecur,ptnodeav,type)
ttvfig_list *ttvfig ;
losig_list *losig ;
ttvevent_list *ptnode ;
ttvevent_list *ptnodecur ;
ttvevent_list *ptnodeav ;
long type ;
{
 ttvevent_list *ptnodeap ;
 ptype_list *ptype ;
 chain_list *chain ;
 int res = 1 ;

 if((ptype  = getptype(ptnodecur->USER,TAS_DELAY_NNRC)) != NULL)
 for(chain = (chain_list *)ptype->DATA ; chain != NULL ; chain = chain->NEXT)
  {
   ptnodeap = (ttvevent_list *)chain->DATA ;
   if((ptnodeap->ROOT->ROOT == ttvfig) || 
      (getptype(ptnodeap->ROOT->USER,TAS_SIG_NEWSIG) != NULL))
     {
      if(ptnode == ptnodeap)
        continue ;
      if(ptnodeav != NULL)
      if(((ptnodeav->ROOT->TYPE & TTV_SIG_N) == TTV_SIG_N) && 
         ((ptnodecur->ROOT->TYPE & TTV_SIG_C) == TTV_SIG_C) && 
         ((ptnodeap->ROOT->TYPE & TTV_SIG_N) == TTV_SIG_N))
        continue ;
      if(tas_calcrcnline(ttvfig,ptnode,ptnodeap,type) == 0)
         res = 0 ; 
     }
   else
    {
     if((ptnodeav == NULL) && 
        /*((ptnodecur->ROOT->TYPE & (TTV_SIG_N | TTV_SIG_C)) != 0) &&  */
        ((ptnodeap->ROOT->TYPE & TTV_SIG_C) == TTV_SIG_C))
      {
       if(tas_findrcnline(ttvfig,losig,ptnode,ptnodeap,ptnodecur,type) == 0)
           res = 0  ;
      }
     else if(ptnodeav != NULL)
      {
       if(((ptnodeav->ROOT->TYPE & TTV_SIG_N) == TTV_SIG_N) && 
          ((ptnodecur->ROOT->TYPE & TTV_SIG_C) == TTV_SIG_C) && 
          ((ptnodeap->ROOT->TYPE & TTV_SIG_C) == TTV_SIG_C))
        if(tas_findrcnline(ttvfig,losig,ptnode,ptnodeap,ptnodecur,type) == 0)
            res = 0 ;
      }
    }
  }
 return(res) ;
}

/*****************************************************************************/
/*                        function tas_calcrcxdelay()                        */
/* parametres :                                                              */
/* lofig : lofig pere                                                        */
/* ttvfig : ttvfig pere                                                      */
/*                                                                           */
/* calcul les delais d'interconnexion d'une figure s'ils existent            */
/*****************************************************************************/
void tas_calcrcxdelay(lofig,ttvfig,type)
lofig_list *lofig ;
ttvfig_list *ttvfig ;
long type ;
{
 ttvfig_list *ttvins ;
 ttvsig_list *ptsig ;
 ttvlbloc_list *ptlbloc[2] ;
 ttvevent_list *root ;
 ttvevent_list *node ;
 ttvline_list *ptline ;
 locon_list *locon ;
 losig_list *losig ;
 ttvsig_list *ptsigext ;
 ttvsig_list *ptsigemt ;
 ttvsig_list *ptsigcone ;
 chain_list *ptsigtra ;
 chain_list *ptsigtri ;
 chain_list *ptsigin ;
 ptype_list *ptype ;
 chain_list *chain ;
 chain_list *chainx ;
 long i ;
 long j ;
 long maxline ;
 int res ;
 char lock ;
 inffig_list *ifl;

 ifl=getloadedinffig(lofig->NAME);
 
 for(chain = ttvfig->INS ; chain != NULL ; chain = chain->NEXT)
  {
   ttvins = (ttvfig_list *)chain->DATA ;

   if((ttvins->STATUS & TTV_STS_LOCK) != TTV_STS_LOCK)
    {
     ttv_lockttvfig(ttvins) ;
     lock = 'Y' ;
    }
   else
     lock = 'N' ;

   if((type & TTV_FILE_DTX) == TTV_FILE_DTX)
    {
     if((ttvins->STATUS & TTV_STS_D) != TTV_STS_D)
        ttv_parsttvfig(ttvins,TTV_STS_D,TTV_FILE_DTX) ;
     ptlbloc[0] = ttvins->DBLOC ;
    }
   else 
    ptlbloc[0] = NULL ;

   if((type & TTV_FILE_TTX) == TTV_FILE_TTX)
    {
     if((ttvins->STATUS & TTV_STS_T) != TTV_STS_T)
        ttv_parsttvfig(ttvins,TTV_STS_T,TTV_FILE_TTX) ;
     ptlbloc[1] = ttvins->TBLOC ;
    }
   else 
    ptlbloc[1] = NULL ;


   for(j = 0 ; j < 2 ; j++)
   for(; ptlbloc[j] != NULL ; ptlbloc[j] = ptlbloc[j]->NEXT)
    for(i = 0 ; i < TTV_MAX_LBLOC ; i++)
      {
       ptline = ptlbloc[j]->LINE + i ;
       if((ptline->TYPE & TTV_LINE_FR) == TTV_LINE_FR)
         continue ;
       node = ptline->NODE ;
       root = ptline->ROOT ;
       if((ptline->TYPE & TTV_LINE_RC) == TTV_LINE_RC)
         {
          if(((root->ROOT->TYPE & TTV_SIG_MARQUE) == TTV_SIG_MARQUE) &&
             ((node->ROOT->TYPE & TTV_SIG_MARQUE) == TTV_SIG_MARQUE))
              continue ;
          if((ptype = getptype(root->USER,TAS_DELAY_NNRC)) == NULL)
             root->USER = addptype(root->USER,TAS_DELAY_NNRC,
                                   (void*)addchain(NULL,(void*)node)) ;
          else
            {
             for(chainx = (chain_list *)ptype->DATA ; chainx != NULL ;
                 chainx = chainx->NEXT)
             if(((ttvevent_list *)chainx->DATA) == node) break ;
             if(chainx == NULL)
               ptype->DATA = addchain((chain_list *)ptype->DATA,(void*)node) ;
            }
         }
      }

    if(lock == 'Y')
      ttv_unlockttvfig(ttvins) ;

    if((ttvins->STATUS & TTV_STS_MODEL) == TTV_STS_MODEL)
      ttv_freememoryiffull(ttvins,TTV_STS_D|TTV_STS_T) ;
    else
      ttv_freememoryifmax(ttvins,TTV_STS_D|TTV_STS_T) ;
   }

 maxline = TTV_MAX_LINE ;
 TTV_MAX_LINE = TTV_ALLOC_MAX ;
 
 for(losig = lofig->LOSIG ; losig != NULL ; losig = losig->NEXT)
  {
   if((losig->TYPE == TAS_ALIM) || 
      (getptype(losig->USER,TAS_SIG_NORCDELAY) != NULL) ||
      (!rcx_isvisiblesignal( losig )))
    continue ;

   res = 1 ;
   ptsigemt = NULL ;
   ptsigcone = NULL ;
   ptsigtri = NULL ;
   ptsigtra = NULL ;
   ptsigin = NULL ;
   ptsigext = NULL ;
   for(chain = (chain_list*)getptype(losig->USER,TAS_LOFIGCHAIN)->DATA ;
       chain != NULL ; chain = chain->NEXT)
    {
     locon = (locon_list *)chain->DATA ;

     if((ptype = getptype(locon->USER,TAS_LOCON_INLIST)) != NULL)
       {
        for(chainx = (chain_list *)ptype->DATA ; chainx != NULL ; 
            chainx = chainx->NEXT)
          {
           ptsigin = addchain(ptsigin,chainx->DATA) ;
          }
       }

     if((ptype = getptype(locon->USER,TAS_LOCON_OUTCONE)) != NULL)
       {
        if((locon->DIRECTION == TRISTATE) || (locon->DIRECTION == TRANSCV))
         {
          ptsigtri = addchain(ptsigtri,ptype->DATA) ;
          ptsigcone = (ttvsig_list *)ptype->DATA  ;
         }
        else
         {
          ptsigemt = (ttvsig_list *)ptype->DATA ;
          ptsigcone = ptsigemt ;
         }
       }

     if((ptype = getptype(locon->USER,TAS_LOCON_SIG)) == NULL)
         continue ;

     if(locon->TYPE == EXTERNAL)
       ptsigext = (ttvsig_list *)ptype->DATA ;
     else if(locon->DIRECTION == IN)
       ptsigin = addchain(ptsigin,ptype->DATA) ;
     else if((locon->DIRECTION == OUT) || (locon->DIRECTION == INOUT))
       ptsigemt = (ttvsig_list *)ptype->DATA ;
     else if(locon->DIRECTION == TRISTATE) 
       ptsigtri = addchain(ptsigtri,ptype->DATA) ;
     else if(locon->DIRECTION == TRANSCV) 
       ptsigtra = addchain(ptsigtra,ptype->DATA) ;
    }
   if(ptsigemt != NULL)
    {
     for(chain = ptsigin ; chain != NULL ; chain = chain->NEXT)
      {
       ptsig = (ttvsig_list *)chain->DATA ;
       if((ptype = getptype(ptsig->NODE[0].USER,TAS_DELAY_NNRC)) == NULL)
         ptsig->NODE[0].USER = addptype(ptsig->NODE[0].USER,TAS_DELAY_NNRC,
                                addchain(NULL,ptsigemt->NODE)) ;
       else
        ptype->DATA = (void *)addchain((chain_list *)ptype->DATA,
                                       ptsigemt->NODE) ;

       if((ptype = getptype(ptsig->NODE[1].USER,TAS_DELAY_NNRC)) == NULL)
         ptsig->NODE[1].USER = addptype(ptsig->NODE[1].USER,TAS_DELAY_NNRC,
                                addchain(NULL,ptsigemt->NODE+1)) ;
       else
        ptype->DATA = (void *)addchain((chain_list *)ptype->DATA,
                                       ptsigemt->NODE+1) ;
      }
     if((ptsigext != NULL) && (ptsigemt != ptsigcone))
      {
       if((ptype = getptype(ptsigext->NODE[0].USER,TAS_DELAY_NNRC)) == NULL)
         ptsigext->NODE[0].USER = addptype(ptsigext->NODE[0].USER,
                       TAS_DELAY_NNRC,addchain(NULL,ptsigemt->NODE)) ;
       else
         ptype->DATA = (void *)addchain((chain_list *)ptype->DATA,
                                        ptsigemt->NODE) ;

       if((ptype = getptype(ptsigext->NODE[1].USER,TAS_DELAY_NNRC)) == NULL)
         ptsigext->NODE[1].USER = addptype(ptsigext->NODE[1].USER,
                       TAS_DELAY_NNRC,addchain(NULL,ptsigemt->NODE+1)) ;
       else
         ptype->DATA = (void *)addchain((chain_list *)ptype->DATA,
                                        ptsigemt->NODE+1) ;
      }
    }
   else
    {
     for(chain = ptsigin ; chain != NULL ; chain = chain->NEXT)
      {
       ptsig = (ttvsig_list *)chain->DATA ;
       if(ptsigext != NULL)
        {
         if((ptype = getptype(ptsig->NODE[0].USER,TAS_DELAY_NNRC)) == NULL)
           ptsig->NODE[0].USER = addptype(ptsig->NODE[0].USER,TAS_DELAY_NNRC,
                                  addchain(NULL,ptsigext->NODE)) ;
         else
           ptype->DATA = (void *)addchain((chain_list *)ptype->DATA,
                                          ptsigext->NODE) ;

         if((ptype = getptype(ptsig->NODE[1].USER,TAS_DELAY_NNRC)) == NULL)
           ptsig->NODE[1].USER = addptype(ptsig->NODE[1].USER,TAS_DELAY_NNRC,
                                  addchain(NULL,ptsigext->NODE+1)) ;
         else
           ptype->DATA = (void *)addchain((chain_list *)ptype->DATA,
                                          ptsigext->NODE+1) ;
        }
       for(chainx = ptsigtri ; chainx != NULL ; chainx = chainx->NEXT)
        {
         if((ptype = getptype(ptsig->NODE[0].USER,TAS_DELAY_NNRC)) == NULL)
           ptsig->NODE[0].USER = addptype(ptsig->NODE[0].USER,TAS_DELAY_NNRC,
                      addchain(NULL,((ttvsig_list*)chainx->DATA)->NODE)) ;
         else
           ptype->DATA = (void *)addchain((chain_list *)ptype->DATA,
                                         ((ttvsig_list*)chainx->DATA)->NODE) ;

         if((ptype = getptype(ptsig->NODE[1].USER,TAS_DELAY_NNRC)) == NULL)
           ptsig->NODE[1].USER = addptype(ptsig->NODE[1].USER,TAS_DELAY_NNRC,
                      addchain(NULL,((ttvsig_list*)chainx->DATA)->NODE+1)) ;
         else
           ptype->DATA = (void *)addchain((chain_list *)ptype->DATA,
                                         ((ttvsig_list*)chainx->DATA)->NODE+1) ;
        }
       for(chainx = ptsigtra ; chainx != NULL ; chainx = chainx->NEXT)
        {
         if((ptype = getptype(ptsig->NODE[0].USER,TAS_DELAY_NNRC)) == NULL)
           ptsig->NODE[0].USER = addptype(ptsig->NODE[0].USER,TAS_DELAY_NNRC,
                      addchain(NULL,((ttvsig_list*)chainx->DATA)->NODE)) ;
         else
           ptype->DATA = (void *)addchain((chain_list *)ptype->DATA,
                                         ((ttvsig_list*)chainx->DATA)->NODE) ;

         if((ptype = getptype(ptsig->NODE[1].USER,TAS_DELAY_NNRC)) == NULL)
           ptsig->NODE[1].USER = addptype(ptsig->NODE[1].USER,TAS_DELAY_NNRC,
                      addchain(NULL,((ttvsig_list*)chainx->DATA)->NODE+1)) ;
         else
           ptype->DATA = (void *)addchain((chain_list *)ptype->DATA,
                                         ((ttvsig_list*)chainx->DATA)->NODE+1) ;
        }
      }
     for(chain = ptsigtra ; chain != NULL ; chain = chain->NEXT)
      {
       ptsig = (ttvsig_list *)chain->DATA ;
       for(chainx = ptsigtra ; chainx != NULL ; chainx = chainx->NEXT)
        {
         if(ptsig == (ttvsig_list *)chainx->DATA) continue ;
         if((ptype = getptype(ptsig->NODE[0].USER,TAS_DELAY_NNRC)) == NULL)
           ptsig->NODE[0].USER = addptype(ptsig->NODE[0].USER,TAS_DELAY_NNRC,
                      addchain(NULL,((ttvsig_list*)chainx->DATA)->NODE)) ;
         else
           ptype->DATA = (void *)addchain((chain_list *)ptype->DATA,
                                         ((ttvsig_list*)chainx->DATA)->NODE) ;
 
         if((ptype = getptype(ptsig->NODE[1].USER,TAS_DELAY_NNRC)) == NULL)
           ptsig->NODE[1].USER = addptype(ptsig->NODE[1].USER,TAS_DELAY_NNRC,
                      addchain(NULL,((ttvsig_list*)chainx->DATA)->NODE+1)) ;
         else
           ptype->DATA = (void *)addchain((chain_list *)ptype->DATA,
                                         ((ttvsig_list*)chainx->DATA)->NODE+1) ;
        }
       for(chainx = ptsigtri ; chainx != NULL ; chainx = chainx->NEXT)
        {
         if((ptype = getptype(ptsig->NODE[0].USER,TAS_DELAY_NNRC)) == NULL)
           ptsig->NODE[0].USER = addptype(ptsig->NODE[0].USER,TAS_DELAY_NNRC,
                      addchain(NULL,((ttvsig_list*)chainx->DATA)->NODE)) ;
         else
           ptype->DATA = (void *)addchain((chain_list *)ptype->DATA,
                                         ((ttvsig_list*)chainx->DATA)->NODE) ;

         if((ptype = getptype(ptsig->NODE[1].USER,TAS_DELAY_NNRC)) == NULL)
           ptsig->NODE[1].USER = addptype(ptsig->NODE[1].USER,TAS_DELAY_NNRC,
                      addchain(NULL,((ttvsig_list*)chainx->DATA)->NODE+1)) ;
         else
           ptype->DATA = (void *)addchain((chain_list *)ptype->DATA,
                                         ((ttvsig_list*)chainx->DATA)->NODE+1) ;
        }
       if(ptsigext != NULL)
        {
         if((ptype = getptype(ptsig->NODE[0].USER,TAS_DELAY_NNRC)) == NULL)
           ptsig->NODE[0].USER = addptype(ptsig->NODE[0].USER,TAS_DELAY_NNRC,
                                 addchain(NULL,ptsigext->NODE)) ;
         else
           ptype->DATA = (void *)addchain((chain_list *)ptype->DATA,
                                         ptsigext->NODE) ;

         if((ptype = getptype(ptsig->NODE[1].USER,TAS_DELAY_NNRC)) == NULL)
           ptsig->NODE[1].USER = addptype(ptsig->NODE[1].USER,TAS_DELAY_NNRC,
                                 addchain(NULL,ptsigext->NODE+1)) ;
         else
           ptype->DATA = (void *)addchain((chain_list *)ptype->DATA,
                                         ptsigext->NODE+1) ;
        }
      }
     if(ptsigext != NULL)
      {
       for(chainx = ptsigtri ; chainx != NULL ; chainx = chainx->NEXT)
        {
         if((ttvsig_list*)chainx->DATA == ptsigcone) continue ;
         if((ptype = getptype(ptsigext->NODE[0].USER,TAS_DELAY_NNRC)) == NULL)
           ptsigext->NODE[0].USER = addptype(ptsigext->NODE[0].USER,
           TAS_DELAY_NNRC,addchain(NULL,((ttvsig_list*)chainx->DATA)->NODE)) ;
         else
           ptype->DATA = (void *)addchain((chain_list *)ptype->DATA,
                                         ((ttvsig_list*)chainx->DATA)->NODE) ;

         if((ptype = getptype(ptsigext->NODE[1].USER,TAS_DELAY_NNRC)) == NULL)
           ptsigext->NODE[1].USER = addptype(ptsigext->NODE[1].USER,
           TAS_DELAY_NNRC,addchain(NULL,((ttvsig_list*)chainx->DATA)->NODE+1)) ;
         else
           ptype->DATA = (void *)addchain((chain_list *)ptype->DATA,
                                         ((ttvsig_list*)chainx->DATA)->NODE+1) ;
        }
       for(chainx = ptsigtra ; chainx != NULL ; chainx = chainx->NEXT)
        {
         if((ptype = getptype(ptsigext->NODE[0].USER,TAS_DELAY_NNRC)) == NULL)
           ptsigext->NODE[0].USER = addptype(ptsigext->NODE[0].USER,
           TAS_DELAY_NNRC,addchain(NULL,((ttvsig_list*)chainx->DATA)->NODE)) ;
         else
           ptype->DATA = (void *)addchain((chain_list *)ptype->DATA,
                                         ((ttvsig_list*)chainx->DATA)->NODE) ;

         if((ptype = getptype(ptsigext->NODE[1].USER,TAS_DELAY_NNRC)) == NULL)
           ptsigext->NODE[1].USER = addptype(ptsigext->NODE[1].USER,
           TAS_DELAY_NNRC,addchain(NULL,((ttvsig_list*)chainx->DATA)->NODE+1)) ;
         else
           ptype->DATA = (void *)addchain((chain_list *)ptype->DATA,
                                         ((ttvsig_list*)chainx->DATA)->NODE+1) ;
        }
      }
    }

   freechain(ptsigin) ;
   freechain(ptsigtri) ;
   freechain(ptsigtra) ;

   ptsigin = NULL ;
   ptsigtri = NULL ;
   ptsigtra = NULL ;

   for(chain = (chain_list*)getptype(losig->USER,TAS_LOFIGCHAIN)->DATA ;
       chain != NULL ; chain = chain->NEXT)
    {
     locon = (locon_list *)chain->DATA ;
     if((ptype = getptype(locon->USER,TAS_LOCON_NSIG)) != NULL)
      {
       for(chainx = (chain_list *)ptype->DATA ; chainx != NULL ;
           chainx = chainx->NEXT)
        {
         ptsig = (ttvsig_list *)chainx->DATA ;
         if(getptype(ptsig->NODE[0].USER,TAS_DELAY_NNRC) != NULL)
          ptsigin = addchain(ptsigin,ptsig->NODE) ; 
         if(getptype(ptsig->NODE[1].USER,TAS_DELAY_NNRC) != NULL)
          ptsigin = addchain(ptsigin,ptsig->NODE+1) ; 
        }
       ptsig = (ttvsig_list *)getptype(locon->USER,TAS_LOCON_SIG)->DATA ;
       if(getptype(ptsig->NODE[0].USER,TAS_DELAY_NNRC) != NULL)
         ptsigtri = addchain(ptsigtri,ptsig->NODE) ;
       if(getptype(ptsig->NODE[1].USER,TAS_DELAY_NNRC) != NULL)
         ptsigtri = addchain(ptsigtri,ptsig->NODE+1) ;
      }
     else if(locon->TYPE == EXTERNAL)
      {
       ptsig = (ttvsig_list *)getptype(locon->USER,TAS_LOCON_SIG)->DATA ;
       if(getptype(ptsig->NODE[0].USER,TAS_DELAY_NNRC) != NULL)
        ptsigtra = addchain(ptsigtra,ptsig->NODE) ; 
       if(getptype(ptsig->NODE[1].USER,TAS_DELAY_NNRC) != NULL)
        ptsigtra = addchain(ptsigtra,ptsig->NODE+1) ; 
      }
     else if(locon->TYPE == INTERNAL)
      {
       ptsig = (ttvsig_list *)getptype(locon->USER,TAS_LOCON_SIG)->DATA ;
       if(getptype(ptsig->NODE[0].USER,TAS_DELAY_NNRC) != NULL)
        ptsigin = addchain(ptsigin,ptsig->NODE) ; 
       if(getptype(ptsig->NODE[1].USER,TAS_DELAY_NNRC) != NULL)
        ptsigin = addchain(ptsigin,ptsig->NODE+1) ; 
      }
     else if(locon->TYPE == 'C')
      {
       if((ptype = getptype(locon->USER,TAS_LOCON_INLIST)) != NULL)
         {
          for(chainx = (chain_list *)ptype->DATA ; chainx != NULL ; 
            chainx = chainx->NEXT)
            {
             ptsig = (ttvsig_list *)chainx->DATA ;
             if(getptype(ptsig->NODE[0].USER,TAS_DELAY_NNRC) != NULL)
              ptsigin = addchain(ptsigin,ptsig->NODE) ; 
             if(getptype(ptsig->NODE[1].USER,TAS_DELAY_NNRC) != NULL)
              ptsigin = addchain(ptsigin,ptsig->NODE+1) ; 
            }
         }
       if((ptype = getptype(locon->USER,TAS_LOCON_OUTCONE)) != NULL)
         {
          ptsig = (ttvsig_list *)ptype->DATA ;
          if(getptype(ptsig->NODE[0].USER,TAS_DELAY_NNRC) != NULL)
           ptsigin = addchain(ptsigin,ptsig->NODE) ; 
          if(getptype(ptsig->NODE[1].USER,TAS_DELAY_NNRC) != NULL)
           ptsigin = addchain(ptsigin,ptsig->NODE+1) ; 
         }
      }
    }

   ptsigin = append(ptsigtra,ptsigin) ;

   for(chain = ptsigin ; chain != NULL ; chain = chain->NEXT)
     if(tas_findrcnline(ttvfig,losig,(ttvevent_list *)chain->DATA,
                     (ttvevent_list *)chain->DATA,NULL,type) == 0)
        res = 0 ;

   if(res == 0)
     tas_error(58,ttv_revect(yagGetName(ifl,losig)),TAS_WARNING) ;

   ptsigin = append(ptsigtri,ptsigin) ;

   for(chain = ptsigin ; chain != NULL ; chain = chain->NEXT)
     {
      freechain((chain_list*)getptype(((ttvevent_list *)chain->DATA)->USER,
                                      TAS_DELAY_NNRC)->DATA) ;
      ((ttvevent_list *)chain->DATA)->USER = 
              delptype(((ttvevent_list *)chain->DATA)->USER,TAS_DELAY_NNRC) ;
     }

   freechain(ptsigin) ;
  }

 TTV_MAX_LINE = maxline ;
}

/*****************************************************************************/
/*                        function tas_builtttvfig()                         */
/* parametres :                                                              */
/* lofig : lofig correspondant au nouveau pere                               */
/*                                                                           */
/* construit une ttvfig pere a partir de la lofig                            */
/*****************************************************************************/
ttvfig_list *tas_builtttvfig(lofig)
lofig_list *lofig ;
{
 ttvfig_list *ttvfig ;
 int delete ;
 long typefile, type ;

#ifdef DELAY_DEBUG_STAT
 mbk_debugstat(NULL, 1);
#endif
 if(TAS_CONTEXT->TAS_PERFINT == 'Y')
  {
   if(TAS_CONTEXT->TAS_INT_END == 'N')
    {
     typefile = TTV_FILE_DTX|TTV_FILE_TTX ;
    }
   else
    {
     typefile = TTV_FILE_DTX ;
    }
  }
 else
  {
   typefile = TTV_FILE_TTX ;
  }

#ifdef DELAY_DEBUG_STAT
 mbk_debugstat("start:",0);
#endif
 
 ttvfig = tas_builtfig(lofig) ;

 if(TAS_CONTEXT->TAS_SHORT_MODELNAME == 'N')
    tas_calcaracline(NULL,NULL,-1,0) ;
 else
    tas_calcaracline(NULL,NULL,1,(long)ttvfig) ;

#ifdef DELAY_DEBUG_STAT
 mbk_debugstat("calcaracline:",0);
 mbk_debugstat(NULL,0);
#endif
 
 tas_loconorient(lofig,ttvfig) ;

 tas_setsigname(lofig) ;
 
#ifdef DELAY_DEBUG_STAT
 mbk_debugstat("loconori+setname:",0);
#endif
 
 rcx_create( lofig ) ;

 tas_builtrcxview(lofig,ttvfig) ;

#ifdef DELAY_DEBUG_STAT
 mbk_debugstat("builtrcxview:",0);
#endif
 
 buildrcx(lofig) ;
 
#ifdef DELAY_DEBUG_STAT
 mbk_debugstat("buildrcx:",0);
#endif
 
 tas_detecloconsig(lofig,ttvfig) ;

 tas_calcrcxdelay(lofig,ttvfig,typefile) ;

#ifdef DELAY_DEBUG_STAT
 mbk_debugstat("calcrcxdelay:",0);
#endif
 
 if(TAS_CONTEXT->TAS_PERFINT == 'Y')
  {
   tas_builtline(ttvfig,TTV_LINE_D) ;
  }
 if(TAS_CONTEXT->TAS_INT_END == 'N')
  {
   tas_builtline(ttvfig,TTV_LINE_T) ;
  }

#ifdef DELAY_DEBUG_STAT
 mbk_debugstat("builtline:",0);
#endif
 
 tas_cleanfig(lofig,ttvfig) ;

 if(TAS_CONTEXT->TAS_PERFINT == 'Y')
  {
   if(TAS_CONTEXT->TAS_INT_END == 'N')
    {
     ttvfig->STATUS |= TTV_STS_TTX|TTV_STS_DTX ;
    }
   else
    {
     ttvfig->STATUS |= TTV_STS_DTX ;
    }
  }
 else
  {
   ttvfig->STATUS |= TTV_STS_TTX ;
  }

 delete = tas_deleteflatemptyfig(ttvfig,typefile,'N') ;

#ifdef DELAY_DEBUG_STAT
 mbk_debugstat("clean+deleteflatemptyfig:",0);
#endif
 
 tas_detectloop(ttvfig,typefile) ;

 ttv_builtrefsig(ttvfig);

 if(delete != 0)
  {
   ttv_setttvlevel(ttvfig) ;
   ttv_setsigttvfiglevel(ttvfig) ;
  }

#ifdef DELAY_DEBUG_STAT
 mbk_debugstat("detectloop+builtrefsig:",0);
#endif
 
 tas_loadrcxfig(ttvfig) ;
 
#ifdef DELAY_DEBUG_STAT
 mbk_debugstat("loadrcx:",0);
#endif
 
 ttv_getinffile(ttvfig);

 type=TTV_FIND_MAX;
 type|=(TAS_CONTEXT->TAS_FIND_MIN == 'Y') ? TTV_FIND_MIN:0;

 if(V_BOOL_TAB[__TAS_RECOMPUTE_DELAYS].VALUE) // conditionnement pour ne rien changer du comportement precedent
 {
   if (V_BOOL_TAB[__TAS_DELAY_PROP].VALUE) type|=TTV_FIND_DELAY;
 }
 
 tas_calcfigdelay(ttvfig,lofig,type,typefile) ;
 tas_freercxfig(ttvfig) ;

 return(ttvfig) ;
}

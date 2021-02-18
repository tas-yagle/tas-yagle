/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TRC Version 1.01                                            */
/*    Fichier : trc_util.c                                                  */
/*                                                                          */
/*    (c) copyright 1997-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Gregoire AVOT                                             */
/*                                                                          */
/****************************************************************************/

/* CVS informations :

Revision : $Revision: 1.107 $
Author   : $Author: fabrice $
Date     : $Date: 2007/11/15 10:26:34 $

*/

#include "trc.h"

char RCXLOFIGEXTENSION[]="_rcx";

int rcx_isvisiblesignal( losig_list *losig )
{
  if( getptype( losig->USER, RCX_VSSNI ) )
    return 0;
  if( rcx_isbellow( losig ) )
    return 0;
  return 1;
}

losig_list* rcx_getvssni( lofig_list *lofig )
{
  ptype_list *ptl;
  losig_list *losig;
  
  ptl = getptype( lofig->USER, RCX_VSSNI );
  if( !ptl ) {
    losig = addlosig( lofig, 
                      getnumberoflosig( lofig ) + 1, 
                      addchain( NULL, namealloc( "vss_ni" ) ),
                      INTERNAL
                    );
    losig->USER = addptype( losig->USER, RCX_VSSNI, NULL );
    losig->USER = addptype( losig->USER, LOFIGCHAIN, NULL );
    addlorcnet( losig );
    lofig->USER = addptype( lofig->USER, RCX_VSSNI, (void*)losig );
    
  }
  else
    losig = (losig_list*)(ptl->DATA);
  return losig;
}

rcx_list* heaprcx            __P(( void ));

rcx_list	*HEAP_RCX	= NULL;
int		MAX_HEAP_RCX	= 128;

rcx_list* rcx_alloc( losig_list *ptsig )
{
  rcx_list *newrcx;
  newrcx = heaprcx();
  ptsig->USER = addptype( ptsig->USER, PTYPERCX, newrcx );
  return( newrcx );
}

rcx_list* heaprcx()
{
  rcx_list	*newrcx;
  int		 i;

  if(!HEAP_RCX)
  {
    HEAP_RCX=(rcx_list*)mbkalloc( sizeof(rcx_list)*MAX_HEAP_RCX );
    newrcx=HEAP_RCX;

    for(i=1;i<MAX_HEAP_RCX;i++)
    {
      newrcx->NEXT=newrcx+1;
      newrcx++;
    }
    newrcx->NEXT=NULL;
  }

  newrcx=HEAP_RCX;
  HEAP_RCX=HEAP_RCX->NEXT;
  newrcx->NEXT=NULL;
  newrcx->FLAG=0;
  newrcx->VIEW=NULL;
  newrcx->RCXEXTERNAL=NULL;
  newrcx->RCXINTERNAL=NULL;
  newrcx->DRIVERSUP=NULL;
  newrcx->DRIVERSDW=NULL;

  return(newrcx);
}

int delrcx( ptsig )
losig_list      *ptsig;
{
  chain_list    *chain ;
  chain_list    *newlofigchain ;
  ptype_list    *ptl ;
  rcx_list      *rcx ;
  chain_list    *scanctc;
  loctc_list    *loctc;
  losig_list    *sigagr;
  ptype_list    *pt;
  locon_list    *locon;

  rcx = getrcx( ptsig );
  if( !rcx )
    return 0;

  if(ptsig->PRCN != NULL)
  {
    {
      rcx_freewire( ptsig );

      if( !tlc_islosigalim( ptsig ) ) {
        for( scanctc = ptsig->PRCN->PCTC ; scanctc ; scanctc = scanctc->NEXT ) {
          loctc = (loctc_list*)(scanctc->DATA);
          sigagr = rcn_ctcothersig( loctc, ptsig );
          if( !tlc_islosigalim( sigagr ) )
            rcx_setnotinfluentagressor( sigagr, ptsig );
        }
      }
      rcx->VIEW = NULL;
    }
  }

  ptl = getptype( ptsig->USER, LOFIGCHAIN ) ;
  if( ptl ) {
    newlofigchain = NULL ;
    for( chain = (chain_list*)ptl->DATA ; chain ; chain=chain->NEXT ) {
      locon = (locon_list*)chain->DATA ;
      if( !getptype( locon->USER, RCX_FAKE ) )
        newlofigchain = addchain( newlofigchain, locon );
    }
    freechain( (chain_list*)ptl->DATA );
    if( newlofigchain ) 
      ptl->DATA = newlofigchain ;
    else
      ptsig->USER = delptype( ptsig->USER, LOFIGCHAIN );

  }
  for(chain = rcx->RCXEXTERNAL ; chain != NULL ; chain = chain->NEXT)
  {
    locon = (locon_list*)chain->DATA ;
    tlc_delloconparam( locon );
    rcx_cleannodebytransition( locon );      
    if( getptype( locon->USER, RCX_FAKE ) )
      freercxlocon( locon );
  }
  freechain(rcx->RCXEXTERNAL) ;
  rcx->RCXEXTERNAL = NULL;

  if(rcx->RCXINTERNAL != NULL)
  {
    for(chain = rcx->RCXINTERNAL ; chain != NULL ; chain = chain->NEXT)
    {
      locon = (locon_list*)chain->DATA ;
      tlc_delloconparam( locon );
      rcx_cleannodebytransition( locon );      
      if( getptype( locon->USER, RCX_FAKE ) )
        freercxlocon( locon );
    }
    freechain(rcx->RCXINTERNAL) ;
    rcx->RCXINTERNAL = NULL;
  }

  rcx_cleardriver( ptsig );

  freercx(rcx) ;

  if ((pt=getptype(ptsig->USER, RCX_ORIGIN))!=NULL)
  {
    freechain((chain_list *)pt->DATA);
    ptsig->USER = delptype( ptsig->USER, RCX_ORIGIN );
  }
  ptsig->USER = delptype( ptsig->USER, PTYPERCX );
  rcx_freeflagcoupled( ptsig );
  return 1 ;
}

void freercx(pt)
rcx_list	*pt;
{
  if( pt->VIEW )
  {
    avt_log(LOGTRC,2, "freercx() : VIEW not empty !\n" );
  }
  if( pt->RCXINTERNAL )
  {
    avt_log(LOGTRC,2, "freercx() : RCXINTERNAL not empty !\n" );
  }
  pt->NEXT=HEAP_RCX;
  HEAP_RCX=pt;
}

void addloconrcxname(ptlocon,name)
locon_list	*ptlocon;
char	*name;
{
  ptype_list	*ptl;

  if( ( ptl = getptype(ptlocon->USER,RCXNAME) ) )
    ptl->DATA = namealloc(name);
  else
  ptlocon->USER=addptype(ptlocon->USER,RCXNAME,namealloc(name));
}

void givetransloconrcxname(ptlocon)
locon_list *ptlocon ;
{
 chain_list *chain;
 lotrs_list *lotrs ;
 static ht_v2 *tablenomtrs = NULL ;
 static int trsidx = 1 ;
 char  tmp[1024];
 ptype_list *ptype ;
 char *pt ;

 if(tablenomtrs == NULL)
   tablenomtrs = addht_v2( 1024 );

 if(ptlocon == NULL)
  {
   delht_v2(tablenomtrs) ;
   tablenomtrs = NULL ;
   trsidx = 1;
   return ;
  }

 lotrs = (lotrs_list *)ptlocon->ROOT ;

 chain = NULL ;
 if(!tlc_islosigalim(ptlocon->SIG))
  if((ptype = getptype(ptlocon->USER,PNODENAME)) != NULL)
   {
    for(chain = (chain_list *)ptype->DATA ; chain != NULL ;
       chain = chain->NEXT)
     {
      if(chain->DATA != NULL)
       {
        pt = namealloc((char *)chain->DATA) ;
        if(testrcxloconname(ptlocon,pt) == 0)
          continue ;
        if(gethtitem_v2( tablenomtrs, pt) == EMPTYHT) 
         {
          addhtitem_v2( tablenomtrs, pt, 1 );
          addloconrcxname( ptlocon, pt ); 
          break ;
         }
       }
     }
   }

  if(!tlc_islosigalim(ptlocon->SIG))
  if(chain == NULL)
   {
    if( lotrs->TRNAME )
     {
      if(isdigit((int)*lotrs->TRNAME))
        sprintf( tmp, "m%s%c%s", lotrs->TRNAME , SEPAR, getloconrcxname(ptlocon));
      else
        sprintf( tmp, "%s%c%s", lotrs->TRNAME , SEPAR, getloconrcxname(ptlocon));
        pt = namealloc(tmp) ;
    
      if( gethtitem_v2( tablenomtrs, pt ) != EMPTYHT )
       {
         while(1)
         {
           if(isdigit((int)*lotrs->TRNAME))
             sprintf( tmp, "m%s_%d%c%s", lotrs->TRNAME, trsidx , SEPAR,
                      getloconrcxname(ptlocon) );
           else
             sprintf( tmp, "%s_%d%c%s", lotrs->TRNAME, trsidx , SEPAR,
                      getloconrcxname(ptlocon) );
           pt = namealloc(tmp) ;
           if( gethtitem_v2( tablenomtrs, pt) == EMPTYHT )
             break;
           trsidx++;
         }
       }
     }
     else
     {
      while(1)
       {
        sprintf( tmp, "TRS_%d.%s", trsidx , getloconrcxname(ptlocon)) ;
        pt = namealloc(tmp) ;
        if(gethtitem_v2( tablenomtrs, pt) == EMPTYHT)
          break ;
          trsidx++ ;
       }
     }
    addhtitem_v2( tablenomtrs, pt, 1 );
    addloconrcxname( ptlocon, pt ); 
   }
}

char* getloconrcxname(ptlocon)
locon_list	*ptlocon;
{
  ptype_list	*pt;
  static char    nullstring[]="(no locon)";
  if( !ptlocon )
    return nullstring;
  if( ( pt = getptype(ptlocon->USER,RCXNAME) ) )
    return((char*)(pt->DATA));
  return(ptlocon->NAME);  
}

void rcx_setnodebytransition( locon_list *locon, char trans, num_list *head )
{
  ptype_list *ptl ;

  if( trans == TRC_SLOPE_UP ) {
    ptl = getptype( locon->USER, RCX_LOCON_UP );
    if( ptl ) {
      freenum( (num_list*)ptl->DATA );
      ptl->DATA = dupnumlst( head );
    }
    else
      locon->USER = addptype( locon->USER, RCX_LOCON_UP, dupnumlst( head ) ) ;
  }
  else {
    ptl = getptype( locon->USER, RCX_LOCON_DN );
    if( ptl ) {
      freenum( (num_list*)ptl->DATA );
      ptl->DATA = dupnumlst( head );
    }
    else
      locon->USER = addptype( locon->USER, RCX_LOCON_DN, dupnumlst( head ) ) ;
  }
}

num_list* rcx_getnodebytransition( locon_list *locon, char trans )
{
  ptype_list *ptl ;
  num_list   *head ;
  
  head = NULL ;
  
  switch( trans ) {
  case TRC_SLOPE_UP :
    ptl = getptype( locon->USER, RCX_LOCON_UP );
    if( ptl )
      head = (num_list*)(ptl->DATA) ;
    break ;
  case TRC_SLOPE_DOWN :
    ptl = getptype( locon->USER, RCX_LOCON_DN );
    if( ptl )
      head = (num_list*)(ptl->DATA) ;
    break ;
  }

  if( !head )
    head = locon->PNODE ;
  
  return head ;
}

void rcx_cleannodebytransition( locon_list *locon )
{
  long trans[2] = { RCX_LOCON_UP, RCX_LOCON_DN } ;
  int i ;
  ptype_list *ptl ;

  for( i=0 ; i<2 ; i++ ) {
    ptl = getptype( locon->USER, trans[i] );
    if( ptl ) {
      freenum( (num_list*)ptl->DATA );
      locon->USER = delptype( locon->USER, trans[i] );
    }
  }
}

losig_list* getrcxpseudovss( pthead )
ptype_list      *pthead;
{
  ptype_list    *pt;

  pt = getptype( pthead, RCXPSEUDOVSS );
  if( !pt )
    return( NULL );
  return( (losig_list*)(pt->DATA) );
}

ptype_list* addrcxpseudovss( pthead, index, vss )
ptype_list      *pthead;
int              index;
losig_list      **vss;
{
  losig_list    *ptsig;
  ptype_list    *ptl;

  ptl = getptype( pthead, RCXPSEUDOVSS );
  if( ptl )
  {
    ptsig = (losig_list*)( ptl->DATA );
  }
  else
  {
    ptsig = rcx_loinsaddlosig();
    ptsig->INDEX     = index ;
    ptsig->TYPE      = 'I'   ;
    pthead = addptype( pthead, RCXPSEUDOVSS, ptsig );
  }

  if( vss )
    *vss = ptsig;
  return( pthead );
}

losig_list* rcx_getvss( lofig_list *lofig )
{
  ptype_list *ptl;
  losig_list *scanlosig ;

  ptl = getptype( lofig->USER, RCX_SIGVSS );
  if( ptl )
    return (losig_list*)(ptl->DATA) ;

  for( scanlosig = lofig->LOSIG ; scanlosig ; scanlosig = scanlosig->NEXT ) {
    if( !rcx_isvisiblesignal( scanlosig ) )
      continue ;

    if (mbk_LosigIsVSS(scanlosig))
      break;
  }

  if (scanlosig)
    lofig->USER = addptype( lofig->USER, RCX_SIGVSS, scanlosig );
  return( scanlosig );      
}

rcx_list* getrcx(signal)
losig_list	*signal;
{
  ptype_list	*pt;

  pt=getptype(signal->USER,PTYPERCX);

  if(pt)
    return((rcx_list*)(pt->DATA));

  return(NULL);
}


void rcx_addhtrcxcon(loins,lofig,locon)
loins_list *loins ;
lofig_list *lofig ;
locon_list *locon ;
{
 ptype_list *ptype ;
 char *ptname ;

 ptname = getloconrcxname(locon) ;

 if(lofig)
 {
   ptype = getptype(lofig->USER,RCXTABCON) ;
   if( !ptype ) {
     lofig->USER = addptype( lofig->USER, RCXTABCON, addht_v2(10) );
     ptype = lofig->USER ;
   }
   addhtitem_v2((ht_v2 *)ptype->DATA,ptname,(long)locon) ;
 }
 else
 {
   ptype = getptype(loins->USER,RCXTABCON) ;
   if( !ptype ) {
     loins->USER = addptype( loins->USER, RCXTABCON, addht(10) );
     ptype = loins->USER ;
   }
   addhtitem((ht*)ptype->DATA,ptname,(long)locon) ;
 }

}

locon_list *rcx_gethtrcxcon(loins,lofig,name)
loins_list *loins ;
lofig_list *lofig ;
char *name ;
{
 locon_list *locon ;
 ptype_list *ptype ;
 
 if(loins == NULL)
 {
   ptype = getptype(lofig->USER,RCXTABCON) ;
   if((locon = (locon_list *)gethtitem_v2((ht_v2*)ptype->DATA,name)) != (locon_list *)EMPTYHT)
     return(locon) ;
 }
 else
  {
   ptype = getptype(loins->USER,RCXTABCON) ;
   if((locon = (locon_list *)gethtitem((ht*)ptype->DATA,name)) != (locon_list *)EMPTYHT)
     return(locon) ;
  }

 if( lofig ) {
   for(locon = lofig->LOCON ; locon != NULL ; locon = locon->NEXT)
    {
     if( getloconrcxname(locon) == name)
      break ;
    }
 }
 
  if( !locon )
    avt_log(LOGTRC,2, "can't find locon %s\n", name );

  return(locon) ;
}

void rcx_delhtrcxcon( loins_list *loins, lofig_list *lofig )
{
  ptype_list    *ptype;

  if( loins ) {
    ptype = getptype( loins->USER, RCXTABCON );
    if( ptype ) {
      delht( (ht*)(ptype->DATA) );
      loins->USER = delptype( loins->USER, RCXTABCON );
    }
  }
  else {
    ptype = getptype( lofig->USER, RCXTABCON );
    if( ptype ) {
      delht_v2( (ht_v2*)(ptype->DATA) );
      lofig->USER = delptype( lofig->USER, RCXTABCON );
    }
  }
}

void rcx_addhtrcxsigbyname(loins,lofig,losig, ptname)
loins_list *loins ;
lofig_list *lofig ;
losig_list *losig ;
char       *ptname ;
{
 ptype_list *ptype ;

 if(lofig)
 {
   ptype = getptype(lofig->USER,RCXTABSIG) ;
   if( !ptype ) {
     lofig->USER = addptype( lofig->USER, RCXTABSIG, addht_v2(10) );
     ptype = lofig->USER ;
   }
   addhtitem_v2((ht_v2 *)ptype->DATA,ptname,(long)losig) ;
 }
 else
 {
   ptype = getptype(loins->USER,RCXTABSIG) ;
   if( !ptype ) {
     loins->USER = addptype( loins->USER, RCXTABSIG, addht(10) );
     ptype = loins->USER ;
   }
   addhtitem((ht*)ptype->DATA,ptname,(long)losig) ;
 }

}

losig_list *rcx_gethtrcxsig(loins,lofig,name)
loins_list *loins ;
lofig_list *lofig ;
char *name ;
{
  losig_list *losig = NULL ;
  ptype_list *ptype = NULL ;
  long        elem ;

  if( lofig ) {
    ptype = getptype(lofig->USER,RCXTABSIG) ;
    if( ptype ) {
      elem = gethtitem_v2( (ht_v2*)( ptype->DATA ), name );
      if( elem != EMPTYHT )
        return (losig_list*)elem ;
    }
  }

  if( loins ) {
    ptype = getptype(loins->USER,RCXTABSIG) ;
    if( ptype ) {
      elem = gethtitem( (ht*)( ptype->DATA ), name );
      if( elem != EMPTYHT )
        return (losig_list*)elem ;
    }
  }

  return(losig) ;
}

void rcx_delhtrcxsig( loins_list *loins, lofig_list *lofig )
{
  ptype_list    *ptype;

  if( loins ) {
    ptype = getptype( loins->USER, RCXTABSIG );
    if( ptype ) {
      delht( (ht*)(ptype->DATA) );
      loins->USER = delptype( loins->USER, RCXTABSIG );
    }
  }
  else {
    ptype = getptype( lofig->USER, RCXTABSIG );
    if( ptype ) {
      delht_v2( (ht_v2*)(ptype->DATA) );
      lofig->USER = delptype( lofig->USER, RCXTABSIG );
    }
  }
}

char* rcx_getlofigname( lofig )
lofig_list *lofig;
{
  char buffer[BUFSIZE] ;
  int  l, i, j ;

  // Vérifie si la chaine se termine par RCXFILEEXTENTION
  for( i=strlen( lofig->NAME )-1, j=strlen( RCXLOFIGEXTENSION )-1 ;
       i>=0 && j>=0 && lofig->NAME[i] == RCXLOFIGEXTENSION[j] ;
       i--, j-- 
     );
  
  if( j<0 )  {// On a retrouvé le RCXLOFIGEXTENTION en fin de nom
    l = strlen( lofig->NAME ) - strlen( RCXLOFIGEXTENSION ) ;
    strncpy( buffer, lofig->NAME, l );
    buffer[l]='\0';
  }
  else {
    strcpy( buffer, lofig->NAME );
  }
  return( namealloc( buffer ) );
}

lofig_list *rcx_getlofig(name, loadspecified)
char *name;
ht *loadspecified;
{
 lofig_list *lofig ;
 losig_list *losig ;
 rcx_list *ptrcx ;
 ptype_list *ptype ;
 chain_list *chain ;
 char buf[1024], *figname ;

 sprintf( buf, "%s%s", name, RCXLOFIGEXTENSION ) ;
 figname = namealloc( buf );

 lofig = getloadedlofig( figname );
 if( lofig ) 
   return( lofig );

 lofig = addlofig(figname) ;
 if(rcxparse(NULL,lofig,name,loadspecified) == 0)
   {
    rcx_dellofig( lofig ) ;
    return(NULL) ;
   }

 lofigchain(lofig) ;

 for(losig = lofig->LOSIG ; losig != NULL ; losig = losig->NEXT)
   {
    if((ptrcx = getrcx( losig )) != NULL)
     {
      ptype = getptype(losig->USER,LOFIGCHAIN) ;
      for(chain = ptrcx->RCXINTERNAL ; chain != NULL ; chain = chain->NEXT)
       {
        ptype->DATA = addchain((chain_list *)ptype->DATA,chain->DATA) ;
       }
     }
   }

 return(lofig) ;
}

void rcx_dellofig(lofig)
lofig_list *lofig ;
{
 losig_list *losig ;
 ptype_list *ptype ;

 trcflushdelaycache();
 
 rcx_delhtrcxcon( NULL, lofig );
 rcx_delhtrcxsig( NULL, lofig );

 if((ptype = getptype(lofig->USER,RCX_MODINS)) != NULL)
  {
   delht((ht *)ptype->DATA) ;
   lofig->USER = delptype(lofig->USER,RCX_MODINS) ;
  }

 if((ptype = getptype(lofig->USER,RCX_LISTINSNAME)) != NULL)
  {
   freechain((chain_list *)ptype->DATA) ;
   lofig->USER = delptype(lofig->USER,RCX_LISTINSNAME) ;
  }

 for(losig = lofig->LOSIG ; losig != NULL ; losig = losig->NEXT)
   delrcx(losig) ;

 rcx_loconcapa_freeheap( lofig, NULL );

 {
   lofig_list *lf;
   for (lf=HEAD_LOFIG; lf!=NULL && lf!=lofig; lf=lf->NEXT) ;
   if (lf)
     dellofig(lofig->NAME) ;
   else
     freelofig(lofig);
 }

}

void rcx_addhtrcxmod(lofig, insname, figname)
lofig_list *lofig ;
char       *insname ;
char       *figname ;
{
  ptype_list *ptype ;

  ptype = getptype(lofig->USER,RCX_MODINS) ;

  if(ptype == NULL) {
    lofig->USER = addptype( lofig->USER ,RCX_MODINS, addht(10) ) ;
    ptype = lofig->USER ;
  }

 sethtitem((ht *)ptype->DATA,insname,(long)figname) ;
}

char* rcx_gethtrcxmod(lofig, insname)
lofig_list *lofig ;
char *insname ;
{
  ptype_list *ptype ;
  char       *figname ;

  ptype = getptype( lofig->USER, RCX_MODINS ) ;
  figname = (char*) gethtitem( (ht *)ptype->DATA, insname );
  if( figname == (char*)EMPTYHT ) 
    return( NULL );
  return( figname );
}

void rcx_addorigin( lofig, dest, name )
lofig_list      *lofig ;
losig_list      *dest ;
char            *name ;
{
  ptype_list *ptl;

  ptl = getptype( dest->USER, RCX_ORIGIN );
  if( !ptl ) {
    dest->USER = addptype( dest->USER, RCX_ORIGIN, NULL );
    ptl = dest->USER ;
  }
  ptl->DATA = addchain( (chain_list*)ptl->DATA, name );

  rcx_addhtrcxsigbyname( NULL, lofig, dest, name );
}

chain_list* rcx_getoriginlist( losig_list *losig )
{
  ptype_list *ptl;

  ptl = getptype( losig->USER, RCX_ORIGIN );
  if( !ptl )
    return NULL;
  return (chain_list*)ptl->DATA;
}

/* Renvoie 1 si le connecteur externe du signal connecté au locon passé en 
 * paramètre porte le nom 'name' */
int testrcxloconname(locon,name)
locon_list *locon ;
char *name ;
{
 locon_list *con ;
 losig_list *losig ;
 chain_list *chain ;
 ptype_list *ptype ;

 losig = locon->SIG ;

 if(losig->TYPE == INTERNAL)
   return(1) ;

 ptype = getptype(losig->USER,LOFIGCHAIN) ;

 if(ptype != NULL)
   for(chain = (chain_list *)ptype->DATA ; chain != NULL ; chain = chain->NEXT)
    {
     con = (locon_list *)chain->DATA ;
     if( !rcx_isvalidlocon( con ) )
       continue;
     if(rcx_isloconexternal(con))
       break ;
    }

 if((ptype == NULL) || (chain == NULL))
  {
   return(0) ;
  }

 if(getloconrcxname(con) == name)
  {
   return(0) ;
  }
 else
   return(1) ;
}

int rcx_islosigexternal( losig )
losig_list *losig;
{
  if( losig->TYPE == EXTERNAL || losig->TYPE == 'T' /* yag_supply.c:199 */ )
    return 1;
  if( losig->TYPE == INTERNAL || losig->TYPE == 'A' ||
      losig->TYPE == 'S'      || losig->TYPE == 'D'    )
    return 0;
  
  rcx_error( 60, losig, AVT_ERROR );

  return 0; // never reach.
}

int rcx_isloconexternal( locon )
locon_list *locon;
{
  if( locon->TYPE == EXTERNAL )
    return 1;
  
  return 0;
}

/* Effacement de toutes les structures RCX et RCN d'une lofig */
void rcx_delalllosigrcx( lofig_list *lofig )
{
  losig_list *scanlosig;
  ptype_list *ptype ;
  
  trcflushdelaycache();

  for( scanlosig = lofig->LOSIG ; scanlosig ; scanlosig = scanlosig->NEXT )
    delrcx( scanlosig );

  if((ptype = getptype(lofig->USER,RCX_MODINS)) != NULL)
  {
    delht((ht *)ptype->DATA) ;
    lofig->USER = delptype(lofig->USER,RCX_MODINS) ;
  }

 if((ptype = getptype(lofig->USER,RCX_LISTINSNAME)) != NULL)
  {
   freechain((chain_list *)ptype->DATA) ;
   lofig->USER = delptype(lofig->USER,RCX_LISTINSNAME) ;
  }

  rcx_delhtrcxcon( NULL, lofig );
  rcx_delhtrcxsig( NULL, lofig );
  rcx_loconcapa_freeheap( lofig, NULL );
}

/* Construit un tableau des agresseurs du signal victime. Un agresseur est
un losig qui possède une vue RCX typée. La taille de la structure d'un élément
du tableau doit être mise dans selem. La structure DOIT avoir les trois premiers
champs definis de la façon suivante :
{
  losig_list*   : Le signal agresseur,
  RCXFLOAT         : Les capacités de couplage GLOBALES de cet agresseur,
  RCXFLOAT         : Les capacités de couplage LOCALES de cet agresseur,
  ...           : Le reste de la structure.
}
A partir d'un signal, il est possible de connaitre son emplacement dans le
tableau avec la fonction rcx_gettabagrindex(). */

void* rcx_buildtabagr( lofig_list *lofig,
                       losig_list *victime, 
                       size_t selem, 
                       int *nbelem, 
                       RCXFLOAT *cm 
                     )
{
  chain_list *scanctc;
  loctc_list *loctc;
  losig_list *sigagr;
  int         maxagr;
  char       *tableau;
  char       *base;
  chain_list *headagr;
  chain_list *scanagr;
  ptype_list *ptl;
  int         i;
  rcxmodagr  *commonelem;
  RCXFLOAT    capa;
  locon_list *locon;
  chain_list *scancon;
  rcxparam   *param;

  maxagr  = 0;
  headagr = NULL;
  *cm     = 0.0;
 
  rcn_lock_signal( lofig, victime );
  for( scanctc = victime->PRCN->PCTC ; scanctc ; scanctc = scanctc->NEXT ) {
  
    loctc  = (loctc_list*)(scanctc->DATA);
    sigagr = rcn_ctcothersig( loctc, victime );
    ptl    = getptype( sigagr->USER, RCX_REALAGRESSOR );
    
    if( !ptl ) {
      // Capacité à la masse
      if( rcn_ctcnode( loctc, victime ) ) *cm = *cm + loctc->CAPA;
      continue;
    }

    param = (rcxparam*)ptl->DATA;
    ptl = getptype( param->SIGNAL->USER, RCX_AGRINDEX_LST );
    if( !ptl ) {
      param->SIGNAL->USER = addptype( param->SIGNAL->USER, 
                                      RCX_AGRINDEX_LST, 
                                      (void*)(long)maxagr 
                                    );
      maxagr++;
      headagr = addchain( headagr, param );
    }
  }

  ptl = getptype( victime->USER, LOFIGCHAIN );
  if( ptl ) {
    for( scancon = (chain_list*)ptl->DATA ; 
         scancon ; 
         scancon = scancon->NEXT ) {
      locon = (locon_list*)scancon->DATA;
      if( !rcx_isvalidlocon( locon ) )
        continue;
      capa = rcx_getloconcapa( locon, TRC_SLOPE_UNK, TRC_CAPA_NOM, TRC_HALF, NULL );
      *cm = *cm + capa ;
    }
  }
  
  * nbelem = maxagr;

  if( maxagr == 0 ) {
    rcn_unlock_signal( lofig, victime );
    return NULL;
  }

  // Création et remplissage du tableau

  tableau = (char*)mbkalloc( selem * maxagr );
  
  for( scanagr=headagr ; scanagr ; scanagr=scanagr->NEXT ) {
    param = (rcxparam*)scanagr->DATA;
    sigagr = param->SIGNAL;
    i = (int)(long)(getptype(sigagr->USER, RCX_AGRINDEX_LST)->DATA);
    base = tableau + i*selem ;
    commonelem = (rcxmodagr*)base;
    commonelem->PARAM    = param;
    commonelem->CLOCALE  = 0.0;
    commonelem->CGLOBALE = 0.0;
  }

  for( scanctc = victime->PRCN->PCTC ; scanctc ; scanctc = scanctc->NEXT ) {
  
    loctc = (loctc_list*)(scanctc->DATA);
    sigagr = rcn_ctcothersig( loctc, victime );
    ptl    = getptype( sigagr->USER, RCX_REALAGRESSOR );
    if( !ptl )
      continue;
    param = (rcxparam*)ptl->DATA;
    ptl = getptype( param->SIGNAL->USER, RCX_AGRINDEX_LST );
    
    if( ptl ) {
      base = tableau + ((size_t)ptl->DATA) * selem;
      commonelem = (rcxmodagr*)base;
      
      if( rcn_ctcnode( loctc, victime ) )
        commonelem->CLOCALE = commonelem->CLOCALE + loctc->CAPA;
      else
        commonelem->CGLOBALE = commonelem->CGLOBALE + loctc->CAPA;
    }
  }

  freechain( headagr );

  rcn_unlock_signal( lofig, victime );
  return (void*)tableau ;
}

void rcx_freetabagr( void *tableau, size_t selem, int nbelem )
{
  losig_list *losig;
  int         i;
  rcxparam   *param;
  
  for( i=0 ; i<nbelem ; i++ ) {
    param = *((rcxparam**)(tableau+i*selem));
    losig = param->SIGNAL;
    losig->USER = delptype( losig->USER, RCX_AGRINDEX_LST );
  }

  mbkfree( tableau );
}

/* Le signal correspond au champs SIGNAL de la structure rcxparam */
int rcx_gettabagrindex( losig_list *losig )
{
  ptype_list *ptl;
  ptl = getptype( losig->USER, RCX_AGRINDEX_LST );
  if( ptl )
    return (int)(long)ptl->DATA;
  return -1;
}

char *rcx_getsigname( losig_list *losig )
{ 
  rcx_list *rcx;
  char     *name;
  ptype_list *ptl;

  ptl = getptype( losig->USER, RCXNAME );
  if( ptl ) {
    name = (char*)ptl->DATA ;
  }
  else {
    rcx = getrcx( losig );
    if( rcx && rcx->RCXEXTERNAL )
      name = getloconrcxname((locon_list *)rcx->RCXEXTERNAL->DATA);
    else
      name = getsigname( losig );
  }

  return name;
}

void rcx_setsigname( losig_list *losig, char *name )
{
  ptype_list *ptl;

  ptl = getptype( losig->USER, RCXNAME );
  if( !ptl ) {
    losig->USER = addptype( losig->USER, RCXNAME, NULL ) ;
    ptl = losig->USER ;
  }
  
  ptl->DATA = name;
}

/******************************************************************************\
Fonctions utilisées lorsque les modèles classiques de crosstalk ne fonctionnent
pas. Dans ce cas il faut utiliser un modèle basique, le 0C/1C/2C.
\******************************************************************************/

char backup_ctk_model=RCX_MILLER_NONE;
char backup_ctk_noise=RCX_NOISE_NONE;

int rcx_isctkbasicmodel( rcx_list *ptrcx )
{
  if( GETFLAG( ptrcx->FLAG, RCXCTKBASIC ) )
    return 1;
  return 0;
}

void rcx_setctkbasicmodel( losig_list *losig, rcx_list *ptrcx )
{
  /*
  fflush( stdout );
  fprintf( stderr, 
           "\nwarning : simple delay model used on net %s.\n",
           rcx_getsigname( losig ) 
         );
  */
  avt_log(LOGTRC,2, "simple delay model used on net %s.\n", 
           rcx_getsigname( losig ) 
         );
  SETFLAG( ptrcx->FLAG, RCXCTKBASIC );
}

void rcx_setenvbasicmodel( void )
{
  if( backup_ctk_model != RCX_MILLER_NONE ||
      backup_ctk_noise != RCX_NOISE_NONE     )
    rcx_error( 37, NULL, AVT_ERROR );

  backup_ctk_model = RCX_CTK_MODEL ;
  backup_ctk_noise = RCX_CTK_NOISE ;
  RCX_CTK_MODEL = RCX_MILLER_0C2C ;
  RCX_CTK_NOISE = RCX_NOISE_NEVER ;
}

void rcx_unsetenvbasicmodel( void ) 
{
  if( backup_ctk_model == RCX_MILLER_NONE ||
      backup_ctk_noise == RCX_NOISE_NONE     )
    rcx_error( 38, NULL, AVT_ERROR );

  RCX_CTK_MODEL = backup_ctk_model ;
  RCX_CTK_NOISE = backup_ctk_noise ;
  backup_ctk_model = RCX_MILLER_NONE ;
  backup_ctk_noise = RCX_NOISE_NONE ;
}

char *rcx_getinsname( loins_list *loins )
{
  char *pt ;
  char buf[1024] ;

  if( loins == NULL )
    return NULL ;

  pt = strchr( loins->INSNAME, (int)SEPAR );
 
  if( !pt )
    return loins->INSNAME ;

  strcpy( buf, loins->INSNAME );
  pt = buf ;

  while( ( pt = strchr( pt, (int)SEPAR ) ) != NULL )
  {
    *pt = '_' ;
    pt++;
  }

  return namealloc( buf ) ;
}

/******************************************************************************\
rcx_levelize_node()
Ajoute dans les nodes d'un réseau RC un entier représentant le plus petit
nombre de résistances depuis l'émetteur.
C'est un parcours en largeur d'abord.
\******************************************************************************/

void rcx_levelize_node( losig_list *losig, lonode_list *start )
{
  chain_list  *todo;
  chain_list  *doing;
  chain_list  *futur;
  int          index;
  lowire_list *wire;
  chain_list  *chwire;
  lonode_list *ptnode;
  lonode_list *next;

  index = 0;
  todo  = addchain( NULL, start );
  rcx_testandsetlevel_node( start, index );

  while( todo ) {
    index++;
    futur = NULL;
    
    for( doing = todo ; doing ; doing = doing->NEXT ) {
    
      ptnode = (lonode_list*)doing->DATA;
      
      for( chwire = ptnode->WIRELIST ; chwire ; chwire = chwire->NEXT ) {
      
        wire = (lowire_list*)chwire->DATA;
        next = getlonode( losig, wire->NODE1 == ptnode->INDEX ? wire->NODE2 :
                                                                wire->NODE1   );
                                                                
        if( rcx_testandsetlevel_node( next, index )==1 )
          futur = addchain( futur, next );
      }
    }
    freechain( todo ) ;
    todo = futur ;
  }
}

/******************************************************************************\
rcx_getlevel_node()
Renvoie le level d'un node ou -1. Le noeud d'origine vaut 0.
\******************************************************************************/
int rcx_getlevel_node( lonode_list *ptnode )
{
  ptype_list *ptl ;

  ptl = getptype( ptnode->USER, RCXNODELEVEL );
  if( ptl )
    return (int)(long)ptl->DATA;
  return -1;
}

/******************************************************************************\
rcx_testandsetlevel_node()
Positionne le level d'un node uniquement si il n'en a pas déjà un.
Renvoie 1 si on l'a positionné, 0 sinon.
\******************************************************************************/
int rcx_testandsetlevel_node( lonode_list *ptnode, int index )
{
  ptype_list *ptl ;

  ptl = getptype( ptnode->USER, RCXNODELEVEL );
  if( ptl ) 
    return 0;

  ptnode->USER = addptype( ptnode->USER, RCXNODELEVEL, (void*)(long)index );
  return 1;
}

/******************************************************************************\
rcx_cleanlevel_node()
Positionne le level d'un node uniquement si il n'en a pas déjà un.
Renvoie 1 si on l'a positionné, 0 sinon.
\******************************************************************************/
void rcx_cleanlevel_node( losig_list *losig )
{
  lowire_list *wire;
  lonode_list *ptnode;

  for( wire = losig->PRCN->PWIRE ; wire ; wire = wire->NEXT ) {
    ptnode = getlonode( losig, wire->NODE1 );
    if( getptype( ptnode->USER, RCXNODELEVEL ) )
      ptnode->USER = delptype( ptnode->USER, RCXNODELEVEL );
    ptnode = getlonode( losig, wire->NODE2 );
    if( getptype( ptnode->USER, RCXNODELEVEL ) )
      ptnode->USER = delptype( ptnode->USER, RCXNODELEVEL );
  }
}

/******************************************************************************\
rcx_triangle_search()
Renvoie les deux wires w2 et wt qui permettent de partir de lonode par wire
et d'y revenir sans passer par from. Ne donne pas de solution si il y a 
plusieurs possibilités.
Renvoie 1 si la solution a été trouvée, 0 sinon.
\******************************************************************************/

int rcx_triangle_search( losig_list *losig,
                         lonode_list *lonode,
                         lowire_list *wire,
                         lowire_list *from,
                         lowire_list **w2,
                         lowire_list **wt
                       )
{
  char found ;
  int instart ;
  int inode2 ;
  int inodet ;
  lonode_list *pnode2 ;
  lonode_list *pnodet ;
  lowire_list *wire2 ;
  lowire_list *wiret ;
  chain_list  *chwire2 ;
  chain_list  *chwiret ;
  int inend ;
  
  found = 0;
 
  instart = lonode->INDEX ;
  inode2 = wire->NODE1 == instart ? wire->NODE2 : wire->NODE1 ;
  pnode2 = getlonode( losig, inode2 );

  if( RCN_GETFLAG( pnode2->FLAG, RCN_FLAG_PASS ) )
    return 0 ;

  for( chwire2 = pnode2->WIRELIST ; chwire2 ; chwire2 = chwire2->NEXT ) {
    wire2 = (lowire_list*)chwire2->DATA ;
    if( RCN_GETFLAG( wire2->FLAG, RCN_FLAG_IGNORE ) ) continue ;
    if( RCN_GETFLAG( wire2->FLAG, RCXNODEINTR ) ) continue ;
    if( wire2 == from || wire2 == wire )
      continue ;
    inodet = wire2->NODE1 == inode2 ? wire2->NODE2 : wire2->NODE1 ;
    pnodet = getlonode( losig, inodet );

    if( RCN_GETFLAG( pnodet->FLAG, RCN_FLAG_PASS ) )
      continue ;
      
    for( chwiret = pnodet->WIRELIST ; chwiret ; chwiret = chwiret->NEXT ) {
      wiret = (lowire_list*)chwiret->DATA;
      if( RCN_GETFLAG( wiret->FLAG, RCN_FLAG_IGNORE ) ) continue ;
      if( RCN_GETFLAG( wire2->FLAG, RCXNODEINTR ) ) continue ;
      inend = wiret->NODE1 == inodet ? wiret->NODE2 : wiret->NODE1 ;
      if( inend == instart ) {
        if( found == 1 ) {
          *w2 = NULL;
          *wt = NULL;
          return 0;
        }
        *w2 = wire2 ;
        *wt = wiret ;
        found = 1;
      }
    }
  }

  return (int)found;
}

/******************************************************************************\
Fonctions d'allocation pour la reconnaissance des triangles.
\******************************************************************************/
rcx_build_tr* rcx_alloc_build_tr(void)
{
  rcx_build_tr *pt;

  pt = (rcx_build_tr*)mbkalloc( sizeof( rcx_build_tr ) );
  pt->LIST  = NULL ;
  pt->FINAL = NULL ;
  pt->NEXT  = NULL ;

  return pt ;
}

rcx_build_tr_bip* rcx_alloc_build_tr_bip( void )
{
  rcx_build_tr_bip *pt;

  pt = (rcx_build_tr_bip*) mbkalloc( sizeof( rcx_build_tr_bip ) );
  pt->NEXT  = NULL ;
  pt->RWIRE = NULL ;
  pt->EWIRE = NULL ;

  return pt ;
}

void rcx_free_build_tr( rcx_build_tr *pt )
{
  rcx_build_tr_bip *bip, *next ;

  for( bip = pt->LIST ; bip ; bip = next ) {
    next       = bip->NEXT ;
    RCN_CLEARFLAG( bip->EWIRE->FLAG, RCXNODEINTR );
    RCN_CLEARFLAG( bip->RWIRE->FLAG, RCXNODEINTR );
    bip->NEXT  = NULL ;
    bip->RWIRE = NULL ;
    bip->EWIRE = NULL ;
    mbkfree( bip );
  }

  if( pt->FINAL )
    RCN_CLEARFLAG( pt->FINAL->FLAG, RCXNODEINTR );
  mbkfree( pt );
}


/******************************************************************************\
rcx_build_node()
Renvoie la list des rcx_build_tr* qu'il est possible de construire sur lonode.
\******************************************************************************/

rcx_build_tr* rcx_build_node( losig_list  *losig, 
                              lonode_list *lonode,
                              lowire_list *from 
                            )
{  
  chain_list       *chwire ;
  lowire_list      *wire ;
  lowire_list      *wc ;
  lowire_list      *wp ;
  lowire_list      *w2 ;
  lowire_list      *wt ;
  rcx_build_tr     *head_build ;
  rcx_build_tr     *pt_build ;
  rcx_build_tr_bip *pt_bip ;

  head_build = NULL;

  for( chwire = lonode->WIRELIST ; chwire ; chwire = chwire->NEXT ) {
 
    wire = (lowire_list*)chwire->DATA;
    if( RCN_GETFLAG( wire->FLAG, RCN_FLAG_IGNORE ) ) continue ;
    if( wire == from )
      continue;
    if( RCN_GETFLAG( wire->FLAG, RCXNODEBUILD ) )
      continue ;
    if( rcn_getpack( losig, wire) )
      continue ;
    RCN_SETFLAG( wire->FLAG, RCXNODEBUILD );

    wc = wire ;
    wp = NULL ;
    
    pt_build = NULL;
    
    while( rcx_triangle_search( losig, lonode, wc, wp, &w2, &wt ) ) {
      if( !pt_build ) {
        pt_build = rcx_alloc_build_tr();
        pt_build->NEXT = head_build ;
        head_build = pt_build;
      }
      pt_bip = rcx_alloc_build_tr_bip();
      pt_bip->RWIRE   = wc ;
      pt_bip->EWIRE   = w2 ;
      pt_build->FINAL = wt ;
      pt_bip->NEXT = pt_build->LIST ;
      pt_build->LIST = pt_bip ;

      RCN_SETFLAG( wc->FLAG, RCXNODEINTR );
      RCN_SETFLAG( w2->FLAG, RCXNODEINTR );
      RCN_SETFLAG( wt->FLAG, RCXNODEINTR );

      wc = wt;
      wp = w2;

      RCN_SETFLAG( wt->FLAG, RCXNODEBUILD );
    }

    if( pt_build )
      pt_build->LIST = (rcx_build_tr_bip*)
                                         reverse( (chain_list*)pt_build->LIST );
  }

  for( chwire = lonode->WIRELIST ; chwire ; chwire = chwire->NEXT ) {
    wire = (lowire_list*)chwire->DATA;
    if( RCN_GETFLAG( wire->FLAG, RCN_FLAG_IGNORE ) ) continue ;
    RCN_CLEARFLAG( wire->FLAG, RCXNODEBUILD );
  }

  return head_build;
}

/******************************************************************************\
rcx_find_real_one()
Complete une structure rcx_build_tr en continuant sur les autres côtés si 
possible. La structure test contient un seul élément.
\******************************************************************************/
rcx_build_tr* rcx_find_real_one( losig_list *losig, 
                                 rcx_build_tr *test, 
                                 lonode_list *ptnode 
                               )
{
  int               i ;
  lonode_list      *tstnode[2] ;
  int               n[2] ;
  rcx_build_tr_bip *pt_bip ;
  rcx_build_tr_bip *last_bip ;
  rcx_build_tr     *pt_build[2] ;
  lowire_list      *wc ;
  lowire_list      *wp ;
  lowire_list      *w2 ;
  lowire_list      *wt ;

  tstnode[0] = getlonode( losig, test->LIST->EWIRE->NODE1 );
  tstnode[1] = getlonode( losig, test->LIST->EWIRE->NODE2 );
  
  for( i = 0 ; i <= 1 ; i++ ) {

    wc = test->LIST->EWIRE ;
    if( test->LIST->RWIRE->NODE1 == tstnode[i]->INDEX ||
        test->LIST->RWIRE->NODE2 == tstnode[i]->INDEX    )
      wp = test->FINAL ;
    else
      wp = test->LIST->RWIRE ;
  
    pt_build[i]=NULL;
    n[i]=0;
   
    pt_build[i] = rcx_alloc_build_tr();
    while( rcx_triangle_search( losig, tstnode[i], wc, wp, &w2, &wt ) ) {
      n[i] = n[i]+1 ;
      pt_bip = rcx_alloc_build_tr_bip();
      pt_bip->RWIRE   = wt ;
      pt_bip->EWIRE   = w2 ;
      pt_bip->NEXT = pt_build[i]->LIST ;
      pt_build[i]->LIST = pt_bip ;
      RCN_SETFLAG( w2->FLAG, RCXNODEINTR );
      RCN_SETFLAG( wt->FLAG, RCXNODEINTR );
      wc = wt ;
      wp = w2 ;
    }
    /* Les triangles crées ici sont temporaires : on a le droit de
       repasser par dessus */
    for( pt_bip = pt_build[i]->LIST ; pt_bip ; pt_bip = pt_bip->NEXT ) {
      RCN_CLEARFLAG( pt_bip->RWIRE->FLAG, RCXNODEINTR );
      RCN_CLEARFLAG( pt_bip->EWIRE->FLAG, RCXNODEINTR );
    }
  }

  if( ( n[0] == 0 && n[1] == 0 ) || 
      ( n[0] >  1 && n[1] >  1 )    ) {
    rcx_free_build_tr( pt_build[0] );
    rcx_free_build_tr( pt_build[1] );
    return NULL;
  }

  pt_bip=rcx_alloc_build_tr_bip() ;
  pt_bip->RWIRE = test->LIST->EWIRE ;
  if( n[0] > n[1] ) {
    if( test->LIST->RWIRE->NODE1 == tstnode[0]->INDEX ||
        test->LIST->RWIRE->NODE2 == tstnode[0]->INDEX    )    {
      pt_bip->EWIRE = test->FINAL ;
      pt_build[0]->FINAL = test->LIST->RWIRE ;
    }
    else {
      pt_bip->EWIRE = test->LIST->RWIRE ;
      pt_build[0]->FINAL = test->FINAL ;
    }
    rcx_free_build_tr( pt_build[1] );
    rcx_free_build_tr( test );
    for( last_bip = pt_build[0]->LIST ; 
         last_bip->NEXT ; 
         last_bip = last_bip->NEXT 
       ) ;
    last_bip->NEXT = pt_bip ;
    /* marque tous les triangles */
    for( pt_bip = pt_build[0]->LIST ; pt_bip ; pt_bip = pt_bip->NEXT ) {
      RCN_SETFLAG( pt_bip->RWIRE->FLAG, RCXNODEINTR );
      RCN_SETFLAG( pt_bip->EWIRE->FLAG, RCXNODEINTR );
    }
    RCN_SETFLAG( pt_build[0]->FINAL->FLAG, RCXNODEINTR );
    return pt_build[0];
  }
  else {
    if( test->LIST->RWIRE->NODE1 == tstnode[1]->INDEX ||
        test->LIST->RWIRE->NODE2 == tstnode[1]->INDEX    )    {
      pt_bip->EWIRE = test->FINAL ;
      pt_build[1]->FINAL = test->LIST->RWIRE ;
    }
    else {
      pt_bip->EWIRE = test->LIST->RWIRE ;
      pt_build[1]->FINAL = test->FINAL ;
    }
    rcx_free_build_tr( pt_build[0] );
    rcx_free_build_tr( test );
    for( last_bip = pt_build[1]->LIST ; 
         last_bip->NEXT ; 
         last_bip = last_bip->NEXT 
       ) ;
    last_bip->NEXT = pt_bip ;
    /* marque tous les triangles */
    for( pt_bip = pt_build[1]->LIST ; pt_bip ; pt_bip = pt_bip->NEXT ) {
      RCN_SETFLAG( pt_bip->RWIRE->FLAG, RCXNODEINTR );
      RCN_SETFLAG( pt_bip->EWIRE->FLAG, RCXNODEINTR );
    }
    RCN_SETFLAG( pt_build[1]->FINAL->FLAG, RCXNODEINTR );
    return pt_build[1];
  }
  ptnode = NULL ;
  return NULL;
}

/******************************************************************************\
rcx_find_real_two()
Complete une structure rcx_build_tr en continuant sur les autres côtés si 
possible. La structure test contient deux éléments.
\******************************************************************************/
rcx_build_tr* rcx_find_real_two( losig_list *losig, 
                                 rcx_build_tr *test, 
                                 lonode_list *ptnode 
                               )
{
  rcx_build_tr_bip *head ;
  rcx_build_tr_bip *pt_bip ;
  rcx_build_tr_bip *pt_bip1 ;
  rcx_build_tr_bip *pt_bip2 ;
  rcx_build_tr_bip *end_bip ;
  rcx_build_tr_bip *head_bip ;
  rcx_build_tr     *pt_build ;
  int               icenter ;
  lonode_list      *pcenter ;
  char              found ;
  lowire_list      *wc ;
  lowire_list      *wp ;
  lowire_list      *w2 ;
  lowire_list      *wt ;


  head            = NULL;
  pt_build        = rcx_alloc_build_tr();
  pt_bip1         = rcx_alloc_build_tr_bip();
  pt_bip1->RWIRE  = test->LIST->EWIRE ;
  pt_bip1->EWIRE  = test->LIST->RWIRE ;
  pt_bip2         = rcx_alloc_build_tr_bip();
  pt_bip2->RWIRE  = test->LIST->NEXT->RWIRE ;
  pt_bip2->EWIRE  = test->FINAL;
  pt_build->FINAL = test->LIST->NEXT->EWIRE ;
  pt_build->LIST  = pt_bip1 ;
  pt_bip1->NEXT   = pt_bip2 ;
  pt_bip2->NEXT   = NULL ;

  icenter = test->LIST->NEXT->RWIRE->NODE1 == ptnode->INDEX ? 
               test->LIST->NEXT->RWIRE->NODE2 : test->LIST->NEXT->RWIRE->NODE1 ;
  pcenter = getlonode( losig, icenter );
  
  wc      = pt_build->LIST->RWIRE ;
  wp      = pt_build->LIST->EWIRE ;
  end_bip = pt_build->LIST->NEXT ;

  found = 0;

  while( rcx_triangle_search( losig, pcenter, wc, wp, &w2, &wt ) ) {
    pt_bip = rcx_alloc_build_tr_bip();
    pt_bip->RWIRE   = wt ;
    pt_bip->EWIRE   = w2 ;
    pt_bip->NEXT = pt_build->LIST ;
    pt_build->LIST = pt_bip ;
    RCN_SETFLAG( wt->FLAG, RCXNODEINTR );
    RCN_SETFLAG( w2->FLAG, RCXNODEINTR );
    wc = wt ;
    wp = w2 ;
    found = 1 ;
  }

  wc = pt_build->FINAL ;
  wp = end_bip->EWIRE ;
  head_bip = NULL ;
  
  while( rcx_triangle_search( losig, pcenter, wc, wp, &w2, &wt ) ) {
    pt_bip = rcx_alloc_build_tr_bip() ;
    pt_bip->RWIRE = wc ;
    pt_bip->EWIRE = w2 ;
    pt_bip->NEXT = head_bip ;
    head_bip = pt_bip ;
    pt_build->FINAL = wt ;
    RCN_SETFLAG( wc->FLAG, RCXNODEINTR );
    RCN_SETFLAG( w2->FLAG, RCXNODEINTR );
    wc = wt;
    wp = w2;
    found = 1 ;
  }
  
  if( head_bip ) {
    head_bip = (rcx_build_tr_bip*) reverse( (chain_list*)head_bip ) ;
    for( pt_bip = pt_build->LIST ; pt_bip->NEXT ; pt_bip = pt_bip->NEXT );
    pt_bip->NEXT = head_bip ;
  }

  if( !found ) {
    rcx_free_build_tr( pt_build );
    pt_build = NULL ;
  }
  else {
    rcx_free_build_tr( test );
  }

  return pt_build ;
}

/******************************************************************************\
rcx_find_real()
Complete une structure rcx_build_tr en continuant sur les autres côtés si 
possible. La structure test contient un ou deux éléments.
\******************************************************************************/
rcx_build_tr* rcx_find_real( losig_list *losig, 
                             rcx_build_tr *test, 
                             lonode_list *ptnode 
                           )
{
  rcx_build_tr *real;
  
  if( test->LIST->NEXT ) 
    real = rcx_find_real_two( losig, test, ptnode );
  else
    real = rcx_find_real_one( losig, test, ptnode );
  return real;
}

/******************************************************************************\
rcx_build_to_triangle_exact()
convertie un rcx_build_tr contenant un seul triangle en une structure triangle.
la convertion est exacte.
\******************************************************************************/
rcx_triangle* rcx_build_to_triangle_exact( losig_list   *losig,
                                           lonode_list  *lonode,
                                           rcx_build_tr *pt_build 
                                         )
{
  rcx_triangle *nouv ;
  RCXFLOAT      rab ;
  RCXFLOAT      rac ;
  RCXFLOAT      rbc ;
  
  nouv = (rcx_triangle*)mbkalloc( sizeof( rcx_triangle ) ) ;
  rcx_settriangle_build( losig, pt_build, nouv );
  
  nouv->n0 = lonode ;
  
  if( pt_build->FINAL->NODE1 == lonode->INDEX )
    nouv->n1 = getlonode( losig, pt_build->FINAL->NODE2 );
  else
    nouv->n1 = getlonode( losig, pt_build->FINAL->NODE1 );

  if( pt_build->LIST->RWIRE->NODE1 == lonode->INDEX )
    nouv->n2 = getlonode( losig, pt_build->LIST->RWIRE->NODE2 );
  else
    nouv->n2 = getlonode( losig, pt_build->LIST->RWIRE->NODE1 );

  nouv->REPORTED_IN  = NULL ;
  nouv->REPORTED_N1 = NULL ;
  nouv->REPORTED_N2 = NULL ;
  
  rab = rcn_get_resi_para( pt_build->LIST->RWIRE ) ;
  rac = rcn_get_resi_para( pt_build->FINAL ) ;
  rbc = rcn_get_resi_para( pt_build->LIST->EWIRE ) ;
  nouv->Z0 = rab*rac / ( rab+rbc+rac );
  nouv->Z1 = rac*rbc / ( rab+rbc+rac );
  nouv->Z2 = rab*rbc / ( rab+rbc+rac );

  nouv->BUILD = pt_build ;
  return nouv ;
}

/******************************************************************************\
rcx_build_to_triangle_approx()
convertie un rcx_build_tr contenant plusieurs triangles en une structure
triangle. la convertion est approximative.
\******************************************************************************/
rcx_triangle* rcx_build_to_triangle_approx( losig_list   *losig, 
                                            lonode_list  *lonode, 
                                            rcx_build_tr *pt_build,
                                            RCXFLOAT      sre,
                                            RCXFLOAT      srr
                                          )
{
  rcx_triangle     *nouv ;
  rcx_build_tr_bip *bip ;
  int               inode ;
  int               cnode ;
  RCXFLOAT          r ;
  
  nouv = (rcx_triangle*)mbkalloc( sizeof( rcx_triangle ) );
  rcx_settriangle_build( losig, pt_build, nouv );
  nouv->REPORTED_IN  = NULL ;
  nouv->REPORTED_N1 = NULL ;
  nouv->REPORTED_N2 = NULL ;


  /* recherche du centre du triangle */
  if( pt_build->LIST->RWIRE->NODE1 == pt_build->FINAL->NODE1 ||
      pt_build->LIST->RWIRE->NODE2 == pt_build->FINAL->NODE1    )
    cnode = pt_build->FINAL->NODE1 ;
  else
    cnode = pt_build->FINAL->NODE2 ;

  if( lonode->INDEX == cnode ) {
    
    /* lonode est au centre */
    nouv->n0 = lonode ;
    
    if( pt_build->FINAL->NODE1 == lonode->INDEX )
      nouv->n1 = getlonode( losig, pt_build->FINAL->NODE2 );
    else
      nouv->n1 = getlonode( losig, pt_build->FINAL->NODE1 );

    if( pt_build->LIST->RWIRE->NODE1 == lonode->INDEX )
      nouv->n2 = getlonode( losig, pt_build->LIST->RWIRE->NODE2 );
    else
      nouv->n2 = getlonode( losig, pt_build->LIST->RWIRE->NODE1 );
    
    nouv->Z0 = srr ;
    nouv->Z1 = sre/2.0 ;
    nouv->Z2 = sre/2.0 ;

    r = rcn_get_resi_para( pt_build->LIST->EWIRE ) ;
    for( bip = pt_build->LIST->NEXT ; bip ; bip = bip->NEXT ) {
  
      if( bip->RWIRE->NODE1 == cnode )
        inode = bip->RWIRE->NODE2 ;
      else
        inode = bip->RWIRE->NODE1 ;

      if( r > sre / 2.0 )
        nouv->REPORTED_N1 = addchain( nouv->REPORTED_N1, getlonode( losig, inode ) );
      else
        nouv->REPORTED_N2 = addchain( nouv->REPORTED_N2, getlonode( losig, inode ) );

      r = r + rcn_get_resi_para( bip->EWIRE ) ;
    }
  }
  else {
    /* lonode est sur le coté opposé. On doit choisir l'une des deux extrémités du
    triangle pour n0 : c'est celle qui rapproche plus de lonode, c'est à dire qui a
    la résistance la plus petite. */
    
    r = 0.0 ;
    for( bip = pt_build->LIST ; bip ; bip = bip->NEXT ) {
      if( bip->RWIRE->NODE1 == lonode->INDEX ||
          bip->RWIRE->NODE2 == lonode->INDEX    )
        break ;
      r = r + rcn_get_resi_para( bip->EWIRE ) ;
    }

    
    /* calcule n0, n1 et n2 */
    if( r < sre/2.0 ) {
      nouv->n0 = getlonode( losig,
                            pt_build->LIST->RWIRE->NODE1 == cnode ?
                              pt_build->LIST->RWIRE->NODE2 : 
                              pt_build->LIST->RWIRE->NODE1
                          );
      nouv->n1 = getlonode( losig,
                            pt_build->FINAL->NODE1 == cnode ?
                              pt_build->FINAL->NODE2 : 
                              pt_build->FINAL->NODE1
                          );
    }
    else {
      nouv->n1 = getlonode( losig,
                            pt_build->LIST->RWIRE->NODE1 == cnode ?
                              pt_build->LIST->RWIRE->NODE2 : 
                              pt_build->LIST->RWIRE->NODE1
                          );
      nouv->n0 = getlonode( losig,
                            pt_build->FINAL->NODE1 == cnode ?
                              pt_build->FINAL->NODE2 : 
                              pt_build->FINAL->NODE1
                          );
    }
   
    nouv->n2 = getlonode( losig, cnode );

    nouv->Z0 = sre/2.0 ;
    nouv->Z1 = sre/2.0 ;
    nouv->Z2 = srr ;

    for( bip = pt_build->LIST->NEXT ; bip ; bip = bip->NEXT ) {
  
      if( bip->RWIRE->NODE1 == cnode )
        inode = bip->RWIRE->NODE2 ;
      else
        inode = bip->RWIRE->NODE1 ;

      if( inode != lonode->INDEX ) {
        nouv->REPORTED_IN = addchain( nouv->REPORTED_IN, getlonode( losig, inode ) );
      }
    }
    if( nouv->n0 != lonode )
        nouv->REPORTED_IN = addchain( nouv->REPORTED_IN, nouv->n0 );
  }
    

  nouv->BUILD = pt_build ;
  return nouv ;
}

void rcx_display_triangle( rcx_triangle *triangle ) 
{
  lonode_list      *node ;
  chain_list       *chain ;
  rcx_build_tr_bip *bip ;

  printf( "\ntriangle :\n" );
  printf( "  n0 = %2ld w0 = %e\n", triangle->n0->INDEX, triangle->Z0 );
  printf( "  n1 = %2ld w1 = %e\n", triangle->n1->INDEX, triangle->Z1 );
  printf( "  n2 = %2ld w2 = %e\n", triangle->n2->INDEX, triangle->Z2 );
  printf( "  reported nodes on input :" );
  for( chain = triangle->REPORTED_IN ; chain ; chain = chain->NEXT ) {
    node = (lonode_list*)chain->DATA ;
    if( chain->NEXT )
      printf ( " %ld", node->INDEX );
    else
      printf ( " %ld\n", node->INDEX );
  }
  printf( "  reported nodes on n1 :" );
  for( chain = triangle->REPORTED_N1 ; chain ; chain = chain->NEXT ) {
    node = (lonode_list*)chain->DATA ;
    if( chain->NEXT )
      printf ( " %ld", node->INDEX );
    else
      printf ( " %ld\n", node->INDEX );
  }
  printf( "  reported nodes on n2 :" );
  for( chain = triangle->REPORTED_N2 ; chain ; chain = chain->NEXT ) {
    node = (lonode_list*)chain->DATA ;
    if( chain->NEXT )
      printf ( " %ld", node->INDEX );
    else
      printf ( " %ld\n", node->INDEX );
  }
  printf( "  built from :\n");
  for( bip = triangle->BUILD->LIST ; bip ; bip = bip->NEXT )
    printf( "    %2ld-%2ld  %2ld-%2ld\n", bip->RWIRE->NODE1, bip->RWIRE->NODE2, 
                                        bip->EWIRE->NODE1, bip->EWIRE->NODE2  );
  printf( "    %2ld-%2ld\n", triangle->BUILD->FINAL->NODE1, 
                             triangle->BUILD->FINAL->NODE2 
        );
}

/******************************************************************************\
rcx_build_to_triangle()
Construit la structure triangle finale qui sera utilisée dans rcx_rcnload() et
pour AWE.
\******************************************************************************/
rcx_triangle* rcx_build_to_triangle( losig_list   *losig, 
                                     lonode_list  *lonode, 
                                     rcx_build_tr *pt_build
                                   )
{
  rcx_build_tr_bip *bip ;
  RCXFLOAT          sre ;
  RCXFLOAT          srr ;
  rcx_triangle     *triangle ;
  lowire_list      *tmp ;
  int               n ;
  RCXFLOAT          r1, r2, r3, r ;
  
  sre = 0.0 ;
  srr = 0.0 ;

  triangle = NULL ;
  if( pt_build->LIST->NEXT == NULL ) {
    triangle = rcx_build_to_triangle_exact( losig, lonode, pt_build );
  }
  else {
    n = 0;
    for( bip =  pt_build->LIST ; bip ; bip = bip->NEXT ) {
      n++;
      sre = sre + rcn_get_resi_para( bip->EWIRE ) ; 
      if( bip == pt_build->LIST )
        srr = rcn_get_resi_para( bip->RWIRE ) ;
      else {
        r = rcn_get_resi_para( bip->RWIRE );
        srr = ( srr * r ) / ( srr + r );
      }
    }
    r = rcn_get_resi_para( pt_build->FINAL );
    srr = ( srr * r ) / ( srr + r );

    if( sre < 5.0 && srr > 10.0*sre ) {
      triangle = rcx_build_to_triangle_approx( losig, 
                                               lonode, 
                                               pt_build, 
                                               sre, 
                                               srr 
                                             );
    }
    else {
      if( n==2 ) { /* on essaye de voir ce que ça donne si on arrive de l'autre côté */
      
        tmp                   = pt_build->LIST->EWIRE ;
        pt_build->LIST->EWIRE = pt_build->LIST->RWIRE ;
        pt_build->LIST->RWIRE = tmp ;
        
        tmp                         = pt_build->FINAL ;
        pt_build->FINAL             = pt_build->LIST->NEXT->EWIRE ;
        pt_build->LIST->NEXT->EWIRE = tmp ;
      
        sre =   rcn_get_resi_para( pt_build->LIST->EWIRE ) 
              + rcn_get_resi_para( pt_build->LIST->NEXT->EWIRE ) ;
        r1 = rcn_get_resi_para( pt_build->LIST->RWIRE ) ;
        r2 = rcn_get_resi_para( pt_build->LIST->NEXT->RWIRE ) ;
        r3 = rcn_get_resi_para( pt_build->FINAL ) ;
        srr =  (r1*r2*r3)/(r1*r2+r1*r3+r2*r3) ;
        
        if( sre < 5.0 && srr > 10.0*sre ) {
          triangle = rcx_build_to_triangle_approx( losig, 
                                                   lonode, 
                                                   pt_build, 
                                                   sre, 
                                                   srr 
                                                 );
        }
      }
      else {

        triangle = NULL;
      }
    }
  }

  /*
  if( triangle )
    rcx_display_triangle( triangle );
  */
  if( !triangle )
    rcx_free_build_tr( pt_build );
    
  losig    = NULL ;
  return triangle ;
}

/******************************************************************************\
rcx_triangle_node()
Renvoie tous les triangles sur lonode.
Les wires qui partent du lonode et qui sont sur des triangles renvoient la
structure triangle correspondante avec la fonction rcx_gettriangle_wire().
L'algo peut être coûteux si le réseau RC est localement fortement connecté.
\******************************************************************************/

rcx_triangle* rcx_triangle_node( losig_list *losig, 
                                 lonode_list *lonode,
                                 lowire_list *from 
                               )
{
  rcx_build_tr     *head_build ;
  rcx_build_tr     *next ;
  rcx_build_tr     *pt_build ;
  rcx_build_tr     *pt_real ;
  rcx_build_tr_bip *pt_bip ;
  int               n ;
  rcx_triangle     *trlist = NULL ;
  rcx_triangle     *tr ;
  
  head_build = rcx_build_node( losig, lonode, from );

  for( pt_build = head_build ; pt_build ; pt_build = next ) {
    next = pt_build->NEXT ; 
    n=0;
    for( pt_bip = pt_build->LIST ; pt_bip ; pt_bip = pt_bip->NEXT ) 
      n++;

    if( n > 2 ) {
      tr = rcx_build_to_triangle( losig, lonode, pt_build );
    }
    else {
      pt_real = rcx_find_real( losig, pt_build, lonode );
      if( pt_real )
        tr = rcx_build_to_triangle( losig, lonode, pt_real );
      else
        tr = rcx_build_to_triangle( losig, lonode, pt_build );
    }
    if( tr ) {
      tr->NEXT = trlist ;
      trlist = tr ;
    }
  }

  return trlist ;
}

void rcx_settriangle_build( losig_list *losig, 
                            rcx_build_tr *pt_build, 
                            rcx_triangle *triangle 
                          )
{
  rcx_build_tr_bip *bip ;
  rcn_setpack( losig, pt_build->FINAL, (void*) triangle );
  for( bip = pt_build->LIST ; bip ; bip = bip->NEXT ) {
    rcn_setpack( losig, bip->RWIRE, (void*) triangle );
    rcn_setpack( losig, bip->EWIRE, (void*) triangle );
  }

}

/*
void rcx_settriangle_wire( lowire_list *wire, rcx_triangle *triangle )
{
  wire->USER = addptype( wire->USER, RCXTRIANGLE, triangle );
}

rcx_triangle* rcx_gettriangle_wire( lowire_list *wire )
{
  ptype_list *ptl;
  ptl = getptype( wire->USER, RCXTRIANGLE );
  if( ptl )
    return (rcx_triangle*)ptl->DATA;
  return NULL;
}
*/

void rcx_cleantriangle_node( losig_list *losig,
                             lonode_list *lonode, 
                             rcx_triangle *triangle 
                           )
{
  rcx_build_tr_bip *bip ;

  if( !triangle )
    return ;

  for( bip = triangle->BUILD->LIST ; bip ; bip = bip->NEXT ) {
    RCN_CLEARFLAG( bip->RWIRE->FLAG, RCXNODEINTR );
    RCN_CLEARFLAG( bip->EWIRE->FLAG, RCXNODEINTR );
    rcn_clearpack( losig, bip->RWIRE );
    rcn_clearpack( losig, bip->EWIRE );
  }
  RCN_CLEARFLAG( triangle->BUILD->FINAL->FLAG, RCXNODEINTR );
  rcn_clearpack( losig, triangle->BUILD->FINAL );
  freechain( triangle->REPORTED_IN );
  freechain( triangle->REPORTED_N1 );
  freechain( triangle->REPORTED_N2 );
  rcx_free_build_tr( triangle->BUILD );
  mbkfree( triangle );
  lonode=NULL;
}

/******************************************************************************\
rcx_isonlytriangle()
renvoie 1 si le réseau ne comporte comme boucle que des triangles qu'on
sait convertir en étoiles.
\******************************************************************************/

int rcx_isreconvergence_node( losig_list *losig, lonode_list *lonode )
{
  chain_list  *list ;
  int          nblt;
  int          nbe;
  lonode_list *neighbour;
  int          l;
  int          level;
  lowire_list *wire;

  nblt    = 0;
  nbe     = 0;
  level   = rcx_getlevel_node( lonode );
  
  for( list = lonode->WIRELIST ; list ; list = list->NEXT ) {
  
    wire = (lowire_list*)list->DATA;
    if( RCN_GETFLAG( wire->FLAG, RCN_FLAG_IGNORE ) ) continue ;

    neighbour = getlonode( losig,
                           wire->NODE1 == lonode->INDEX ? wire->NODE2 : 
                                                          wire->NODE1 
                         );
    l = rcx_getlevel_node( neighbour );

    if( l < level ) {
      nblt++;
      if( nblt >=2 )
        return 0;
    }

    if( l == level ) {
      nbe++;
      if( nbe >=2 )
        return 0;
    }
  }

  return 1;
}

int rcx_isonlytriangle( losig_list *losig )
{
  lowire_list   *pwire;
  lonode_list   *node1;
  lonode_list   *node2;
  int            l1;
  int            l2;
  int            triangle;
  chain_list    *list1;
  chain_list    *list2;
  int            nodet1;
  int            nodet2;
  lowire_list   *wire1;
  lowire_list   *wire2;

  if( !losig || !losig->PRCN || !losig->PRCN->PWIRE ) {
    return 0;
  }

  rcx_levelize_node( losig, getlonode( losig, losig->PRCN->PWIRE->NODE1 ) );

  for( pwire = losig->PRCN->PWIRE ; pwire ; pwire = pwire->NEXT ) {
  
    node1 = getlonode( losig,pwire->NODE1 ) ;
    node2 = getlonode( losig,pwire->NODE2 ) ;
    
    if( !rcx_isreconvergence_node( losig, node1 ) ||
        !rcx_isreconvergence_node( losig, node2 )    ) {
      rcx_cleanlevel_node( losig );
      return 0;
    }

    l1 = rcx_getlevel_node( node1 );
    l2 = rcx_getlevel_node( node2 );

    triangle = 0;
    if( l1 == l2 ) {
      for( list1 = node1->WIRELIST ; list1 ; list1 = list1->NEXT ) {
        wire1 = (lowire_list*)list1->DATA;
        if( wire1 == pwire ) continue;
        nodet1 = (wire1->NODE1==node1->INDEX ? wire1->NODE2 : wire1->NODE1 );
        for( list2 = node2->WIRELIST ; list2 ; list2 = list2->NEXT ) {
          wire2 = (lowire_list*)list2->DATA;
          if( wire2 == pwire ) continue;
          nodet2 = (wire2->NODE1==node2->INDEX ? wire2->NODE2 : wire2->NODE1 );
          if( nodet1 == nodet2 ) {
            triangle = 1;
            break;
          }
        }
        if( triangle )
          break;
      }
      if( !triangle ) {
        rcx_cleanlevel_node( losig );
        return 0;
      }
    }
  }
  
  rcx_cleanlevel_node( losig );
  return 1;
}

/******************************************************************************\
rcx_get_wire_connexe()
Gives the list of connexe composant from locon.
Each pnode of locon have a ptype WIRECONNEXE wich point to the corresponding
wirefromlocon_list* structure.
\******************************************************************************/
wirefromlocon_list* rcx_get_wire_connexe( losig_list *losig, num_list *driver )
{
  num_list           *num ;
  lonode_list        *ptnode ;
  wirefromlocon_list *head ;
  wirefromlocon_list *ptnew ;
  wirefromlocon_list *ptprevious ;
  ptype_list         *ptl ;

  avt_logenterfunction(LOGTRC,2,"rcx_get_wire_connexe()" );

  for( num = driver ; num ; num = num->NEXT ) {
    ptnode = getlonode( losig, num->DATA );
    ptnode->USER = addptype( ptnode->USER, WIRECONNEXE, NULL );
  }

  head = NULL ;
  for( num = driver ; num ; num = num->NEXT ) {
  
    ptnode = getlonode( losig, num->DATA );
    
    ptl = getptype( ptnode->USER, WIRECONNEXE );
    ptprevious = (wirefromlocon_list*)ptl->DATA ;

    if( ! ptprevious ) {

      /* allocation using ptype... */
      ptnew = (wirefromlocon_list*)addptype( NULL, 0l, NULL );

      ptnew->NEXT = head ;
      ptnew->NODELIST = addchain( NULL, ptnode );
      ptnew->WIRELIST = NULL ;
      head = ptnew ;

      ptl->DATA = ptnew ;

      rcn_tripconnexe( losig,
                       ptnode->INDEX, 
                       (int (*)(losig_list*,lowire_list*,void*))
                                                        rcx_trip_connexe, 
                       ptnew 
                     );
    }
  }

  avt_logexitfunction(LOGTRC,2);
  return head ;
}

int rcx_trip_connexe( losig_list *losig,
                      lowire_list *lowire, 
                      wirefromlocon_list *connexe 
                    )
{
  lonode_list *ptnode ;
  ptype_list  *ptl ;

  connexe->WIRELIST = addchain( connexe->WIRELIST, lowire );
  ptnode = getlonode( losig, lowire->NODE1 );
  ptl = getptype( ptnode->USER, WIRECONNEXE ) ;
  if( ptl ) {
    if( ptl->DATA && ptl->DATA != connexe )
      rcx_error(41,NULL,AVT_FATAL);
    if( !ptl->DATA ) {
      ptl->DATA = connexe ;
      connexe->NODELIST = addchain( connexe->NODELIST, ptnode );
    }
  }
  ptnode = getlonode( losig, lowire->NODE2 );
  ptl = getptype( ptnode->USER, WIRECONNEXE ) ;
  if( ptl ) {
    if( ptl->DATA && ptl->DATA != connexe )
      rcx_error(42,NULL,AVT_FATAL);
    if( !ptl->DATA ) {
      ptl->DATA = connexe ;
      connexe->NODELIST = addchain( connexe->NODELIST, ptnode );
    }
  }

  return 1 ;
}

/******************************************************************************\
rcx_free_wire_connexe()
Clean all information build by rcx_get_wire_connexe().
\******************************************************************************/
void rcx_free_wire_connexe( losig_list         *losig,
                            num_list           *driver, 
                            wirefromlocon_list *connexe 
                          )
{
  num_list           *num ;
  wirefromlocon_list *next ;
  wirefromlocon_list *scan ;
  lonode_list        *ptnode ;

  for( scan = connexe ; scan ; scan = next ) {
    next = scan->NEXT ;
    freechain( scan->NODELIST );
    freechain( scan->WIRELIST );
    scan->NEXT = NULL ;
    freeptype( (ptype_list*)scan );
  }

  for( num = driver ; num ; num = num->NEXT ) {
    ptnode = getlonode( losig, num->DATA );
    ptnode->USER = delptype( ptnode->USER, WIRECONNEXE );
  }
}

void rcx_error( int code, losig_list *losig, int type )
{
  if( losig )
    avt_errmsg( TRC_ERRMSG, "005", type, code, rcx_getsigname(losig) );
  else
    avt_errmsg( TRC_ERRMSG, "006", type, code );
}

void rcx_has_nodeintr( losig_list *losig )
{
  lowire_list *lowire ;
  int n=0;
  for( lowire = losig->PRCN->PWIRE ; lowire ; lowire = lowire->NEXT ) {
    if( RCN_GETFLAG( lowire->FLAG, RCXNODEINTR ) )
      n++;
  }
  if( n ) {
    printf( "RCXNODEINTR : %s = %d\n", rcx_getsigname( losig ), n );
  }
}

void rcx_displaymemoryusage( lofig_list *lofig )
{
  losig_list *losig ;
  locon_list *locon ;
  chain_list *chain ;
  num_list   *num ;
  ptype_list *ptl ;
  it         *table ;
  HeapAlloc  *heap ;
  lowire_list *lowire ;
  loctc_list *loctc ;
  char        validptype ;
  rcx_list   *ptrcx ;
  mbkcache   *ptcache ;
  ht          *hash ;
  ht_v2       *hashv2 ;

  int nbsig           = 0 ;
  int sizesig         = 0 ;
  int nbsigname       = 0 ;
  int sizesigname     = 0 ;
  int nbcon           = 0 ;
  int sizecon         = 0 ;
  int nbconname       = 0 ;
  int sizeconname     = 0 ;
  int nbpnode         = 0 ;
  int sizercn         = 0 ;
  int nbwire          = 0 ;
  int sizewire        = 0 ;
  int nbctc           = 0 ;
  int sizectc         = 0 ;
  int sum             = 0 ;
  int sizeheapofcache = 0 ;
  int sizeofsetcache  = 0 ;
  int sizercx         = 0 ;
  int sizeloconrcx    = 0 ;
  int sizeheapcapa    = 0 ;
  int sizercncache    = 0 ;
  int sizehtcon       = 0 ;
  int sizehtsig       = 0 ;
  int sizeorigin      = 0 ;
  int sizeinslist     = 0 ;
  int sizehtmodins    = 0 ;
  int nblocked        = 0 ;
  int nbunknownptype  = 0 ;
  
  printf( "memory usage report\n\n" );
  
  for( losig = lofig->LOSIG ; losig ; losig = losig->NEXT ) {
  
    nbsig++;
    sizesig = sizesig + sizeof( losig_list );

    if( rcn_islock_signal( lofig, losig ) == YES )
      nblocked++ ;

    if( losig->PRCN ) {
      sizercn = sizercn + sizeof( lorcnet_list );
      for( lowire = losig->PRCN->PWIRE ; lowire ; lowire = lowire->NEXT ) {
        sizewire = sizewire + sizeof( lowire_list );
        nbwire++ ;
      }
      for( chain = losig->PRCN->PCTC ; chain ; chain = chain->NEXT ) {
        loctc = (loctc_list*)chain->DATA;
        if( loctc->SIG1 == losig ) {
          sizectc = sizectc + sizeof( loctc_list );
          nbctc++ ;
        }
      }
    }
    
    for( chain = losig->NAMECHAIN ; chain ; chain = chain->NEXT ) {
      nbsigname++ ;
      sizesigname = sizesigname + strlen((char*)(chain->DATA))+1 ;
    }
   
    for( ptl = losig->USER ; ptl ; ptl = ptl->NEXT ) {

      sizesig = sizesig + sizeof( ptype_list );
      
      validptype = 0 ;
      if( ptl->TYPE == LOFIGCHAIN ) {
        validptype = 1 ;
        for( chain = (chain_list*)ptl->DATA ; chain ; chain = chain->NEXT ) {
          locon = (locon_list*)chain->DATA ;
          nbcon++ ;
          sizecon = sizecon + sizeof( locon_list ) + sizeof( chain_list );
          nbconname++ ;
          sizeconname = sizeconname + strlen( locon->NAME )+1 ;
          for( num = locon->PNODE ; num ; num = num->NEXT ) 
            nbpnode++;
        }
      }
      
      if( ptl->TYPE == PTYPERCX ) {
        validptype = 1 ;
        ptrcx = ptl->DATA ;
        sizercx = sizercx + sizeof( rcx_list );
        if( ptrcx->DRIVERSUP ) {
          sizeloconrcx = sizeloconrcx + sizeof( locon_list ) ;
          for( num = ptrcx->DRIVERSUP->PNODE ; num ; num = num->NEXT ) 
            sizeloconrcx = sizeloconrcx + sizeof( num_list );
        }
        if( ptrcx->DRIVERSDW ) {
          sizeloconrcx = sizeloconrcx + sizeof( locon_list ) ;
          for( num = ptrcx->DRIVERSDW->PNODE ; num ; num = num->NEXT ) 
            sizeloconrcx = sizeloconrcx + sizeof( num_list );
        }
      }

      if( ptl->TYPE == RCX_ORIGIN ) {
        validptype = 1 ;
        for( chain = (chain_list*)(ptl->DATA) ; chain ; chain = chain->NEXT )
          sizeorigin = sizeorigin + strlen( (char*)chain->DATA ) + 1 ;
      }
      
      if( ptl->TYPE == RCX_VSSNI )
        validptype = 1 ;

      if( ptl->TYPE == TLC_ALIM )
        validptype = 1 ;

      if( !validptype ) 
        nbunknownptype++;
    }
  }

  for( ptl = lofig->USER ; ptl ; ptl = ptl->NEXT ) {

    validptype = 0 ;

    if( ptl->TYPE == RCX_FLAGHEAP ) {
      validptype = 1 ;
      heap = (HeapAlloc*)ptl->DATA ;
      sizeheapofcache = sizeheapofcache + getsizeofHeapAlloc( heap ) + sizeof( HeapAlloc );
    }

    if( ptl->TYPE == RCX_FLAGTAB ) {
      validptype = 1 ;
      table = (it*)ptl->DATA ;
      sizeofsetcache = sizeofsetcache + sizeof(long)*table->length + sizeof(it);
    }

    if( ptl->TYPE == RCX_LOCONCAPA_HEAP ) {
      validptype = 1 ;
      heap = (HeapAlloc*)ptl->DATA ;
      sizeheapcapa = sizeheapcapa + getsizeofHeapAlloc( heap ) + sizeof( HeapAlloc );
    }

    if( ptl->TYPE == RCN_LOFIGCACHE ) {
      ptcache = (mbkcache*)ptl->DATA ;
      validptype = 1 ;
      sizercncache = sizercncache + sizeof( mbkcache ) + getsizeofHeapAlloc( & ptcache->HEAPCACHELIST );
      if( ptcache->INFOS ) 
        sizercncache = sizercncache + sizeof( ht ) + ptcache->INFOS->length * sizeof( htitem );
    }

    if( ptl->TYPE == RCXTABSIG ) {
      validptype = 1 ;
      hashv2 = (ht_v2*)ptl->DATA ;
      sizehtsig = sizehtsig + getsizeofht_v2( hashv2 );
    }

    if( ptl->TYPE == RCXTABCON ) {
      validptype = 1 ;
      hashv2 = (ht_v2*)ptl->DATA ;
      sizehtcon = sizehtcon + getsizeofht_v2( hashv2 );
    }

    if( ptl->TYPE == RCX_LISTINSNAME ) {
      validptype = 1 ;
      for( chain = (chain_list*)ptl->DATA ; chain ; chain = chain->NEXT ) {
        sizeinslist = sizeinslist + strlen( (char*)chain->DATA ) + 1 ;
      }
    }

    if( ptl->TYPE == RCX_MODINS ) {
      validptype = 1 ;
      hash = (ht*)ptl->DATA ;
      sizehtmodins = sizehtmodins + sizeof( ht ) + hash->length * sizeof( htitem );
    }

    if( ptl->TYPE == RCX_VSSNI   ||
        ptl->TYPE == RCX_FILEPOS ||
        ptl->TYPE == RCN_LOFIGCACHE_FREE 
        )
      validptype = 1 ;
        
    if( !validptype ) 
      nbunknownptype++;
  }
  
  
  sum =   sizesig    + sizesigname
        + sizecon    + sizeconname
        + nbpnode*sizeof(num_list)
        + sizercn
        + sizeheapofcache + sizeofsetcache 
        + sizewire
        + sizectc 
        + sizercx + sizeloconrcx +
        + sizeheapcapa 
        + sizercncache 
        + sizehtsig 
        + sizehtcon 
        + sizeorigin
        + sizeinslist 
        + sizehtmodins ;

  printf( "%7d losig             %9u\n", nbsig, sizesig );
  printf( "%7d name for losig    %9u\n", nbsigname, sizesigname );
  printf( "        rcn               %9u\n", sizercn );
  printf( "%7d wire              %9u\n", nbwire, sizewire );
  printf( "%7d ctc               %9u\n", nbctc, sizectc );
  printf( "        rcx               %9u\n", sizercx );
  printf( "        loconrcx          %9u\n", sizeloconrcx );
  printf( "        origin            %9u\n", sizeorigin );
  printf( "%7d locon             %9u\n", nbcon, sizecon );
  printf( "%7d name for locon    %9u\n", nbconname, sizeconname );
  printf( "%7d pnode             %9u\n", nbpnode, nbpnode*sizeof(num_list) );
  printf( "        offset heap cache %9u\n", sizeheapofcache );
  printf( "        ofset cache       %9u\n", sizeofsetcache );
  printf( "        heap capa         %9u\n", sizeheapcapa );
  printf( "        rcn cache         %9u\n", sizercncache );
  printf( "        losig hash table  %9u\n", sizehtsig );
  printf( "        locon hash table  %9u\n", sizehtcon );
  printf( "        instance list     %9u\n", sizeinslist );
  printf( "        ht modins         %9u\n", sizehtmodins );
  printf( "                          ---------\n" );
  printf( "                          %9u\n\n\n", sum );
  printf( "%d signal locked\n", nblocked );
  printf( "%d unknown ptype\n", nbunknownptype );
}

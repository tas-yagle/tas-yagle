#include "rcn.h"
#include MSL_H

rcncacheline  *RCNCACHETABLE_LINE  = NULL;
int            RCNCACHETABLE_SIZE  = 10000;
int            RCNCACHETABLE_FIRST = -1;
int            RCNCACHETABLE_LAST  = -1;
int            RCNCACHETABLE_FREE  = -1;

/*******************************************************************************
* function addlorcnet()                                                        *
*******************************************************************************/
lorcnet_list *addlorcnet(ptsig)
losig_list *ptsig;
{

  rcn_checkwritesignal(ptsig );

  if( ptsig->PRCN ) {
    rcn_error( 1, AVT_WARNING ) ;
    return ptsig->PRCN ;
  }

  ptsig->PRCN = heaprcnet();
  return( ptsig->PRCN );
}

/*******************************************************************************
* function givelorcnet()                                                       *
*******************************************************************************/
lorcnet_list *givelorcnet(ptsig)
losig_list *ptsig;
{
  rcn_checkwritesignal(ptsig );
  if( ! ptsig->PRCN )
    ptsig->PRCN = heaprcnet();
  return( ptsig->PRCN );
}

/*******************************************************************************function rcn_existcapa()
Renvoie 1 si une capacité peut potentiellement exister sur ce signal, et 
0 sinon.
*******************************************************************************/
char rcn_existcapa( lofig, losig )
lofig_list *lofig;
losig_list *losig;
{
  // Pas de rc, donc pas de capa
  if( !losig->PRCN )
    return 0;
  
  // Est-ce qu'il y a une capacité ? On ne fait surtout pas de refresh pour 
  // la calculer, on veut juste savoir si il y en a une.
  if( rcn_getcapa( NULL, losig ) > 0.0 )
    return 1;
    
  // Est-ce qu'il y a des éléments RC dans le rc ?
  if( losig->PRCN->PWIRE || losig->PRCN->PCTC )
    return 1;
    
  // On a un cache, donc on trouvera dedans des rc. Un test plus précis serait
  // de savoir si le signal est cacheable, mais seul le parser le sait.
  if( rcn_getlofigcache( lofig ) )
    return 1;
  
  // On a rien trouvé, il n'y a donc pas de capacité sur ce signal.
  return 0;
}

/*******************************************************************************
function rcn_synccapa()
Prend en compte la capacité due aux interconnect. Cette fonction s'utilise lors
d'un rcn_refresh_signal().
*******************************************************************************/
void rcn_synccapa( lofig, ptsig )
lofig_list *lofig;
losig_list *ptsig;
{
  if( !rcn_isnetcapaok( ptsig ) && rcn_getlofigcache( lofig ) ) {
    rcn_setnetcapaok( ptsig );
    rcn_addcapa( ptsig, rcn_getnetcapa( ptsig ) );
  }
}

/*******************************************************************************
function rcn_getcapa()
Renvoie la capacité sur un signal. Si la capacité due aux éléments RC n'a pas
été calculée, l'argument lofig permet de faire un refresh pour la calculer.
*******************************************************************************/
float rcn_getcapa(lofig,ptsig)
lofig_list *lofig;
losig_list *ptsig;
{
  if( !ptsig->PRCN )
    return(0.0) ;

  if( lofig ) {
    if( !rcn_isnetcapaok( ptsig ) && rcn_getlofigcache( lofig ) ) 
      rcn_refresh_signal( lofig, ptsig );
  }
  return rcn_addcapa( ptsig, 0.0 );
}

/*******************************************************************************
function rcn_isnetcapaok()
function rcn_setnetcapaok()
Fonctions qui servent à mémoriser si on a pris en compte la capacité due aux RC.
*******************************************************************************/
char rcn_isnetcapaok(ptsig)
losig_list *ptsig;
{
  if( RCN_GETFLAG( ptsig->RCNCACHE, RCNCACHEFLAG_NETCAPAOK ) )
    return 1;
  return 0;
}

void rcn_setnetcapaok(ptsig)
losig_list *ptsig;
{
  RCN_SETFLAG( ptsig->RCNCACHE, RCNCACHEFLAG_NETCAPAOK );
}

/*******************************************************************************
* function rcn_setcapa()                                                       *
*******************************************************************************/
float rcn_setcapa(ptsig,value)
losig_list *ptsig;
float       value;
{
  if( !ptsig->PRCN ) {
    rcn_error( 2, AVT_WARNING ) ;
    return 0.0 ;
  }

  ptsig->PRCN->RCCAPA = value ;
  return( ptsig->PRCN->RCCAPA );
}

/*******************************************************************************
* function rcn_addcapa()                                                       *
*******************************************************************************/
float rcn_addcapa(ptsig,value)
losig_list *ptsig;
float       value;
{
  if( !ptsig->PRCN ) {
    rcn_error( 3, AVT_WARNING );
    return 0.0 ;
  }
    

  if( value > 0.0 )
    ptsig->PRCN->RCCAPA = ptsig->PRCN->RCCAPA + value ;

  return( ptsig->PRCN->RCCAPA );
}

/*******************************************************************************
* function rcn_removecapa()                                                    *
*******************************************************************************/
float rcn_removecapa(ptsig,value)
losig_list *ptsig;
float       value;
{
  if( !ptsig->PRCN ) {
    rcn_error( 3, AVT_WARNING );
    return 0.0 ;
  }
    

  if( value > 0.0 ) {
    ptsig->PRCN->RCCAPA = ptsig->PRCN->RCCAPA - value ;
    if( ptsig->PRCN->RCCAPA < 0.0 )
      ptsig->PRCN->RCCAPA = 0.0 ;
  }

  return( ptsig->PRCN->RCCAPA );
}

/******************************************************************************\
Calcule la capacité due aux éléments RC.
\******************************************************************************/
float rcn_getnetcapa( losig_list *ptsig )
{
  lowire_list *scanwire;
  chain_list  *scanctc;
  float        capa=0.0;
  
  for( scanwire = ptsig->PRCN->PWIRE ; scanwire ; scanwire = scanwire->NEXT )
    capa = capa + scanwire->CAPA ;
  for( scanctc = ptsig->PRCN->PCTC ; scanctc ; scanctc = scanctc->NEXT )
    capa = capa + ((loctc_list*)scanctc->DATA)->CAPA;
  return capa;
}

/******************************************************************************\
Remplace la capacité du signal par la capacité due aux éléments RC
\******************************************************************************/
float rcn_calccapa( losig_list *ptsig )
{
  rcn_checkwritesignal( ptsig );

  if( ! ptsig->PRCN ) {
    rcn_error( 4, AVT_WARNING );
    return 0.0;
  }

  rcn_setcapa( ptsig, rcn_getnetcapa( ptsig ) );

  return( rcn_getcapa( NULL, ptsig ) );
}

/*******************************************************************************
* function addlowire()                                                         *
*******************************************************************************/
lowire_list *addlowire(ptsig, flag, resi, capa, n1, n2)
losig_list *ptsig;
unsigned char flag;
float resi;
float capa;
long n1;
long n2;
{
  lowire_list	*ptwire;
  lonode_list	*ptnode1;
  lonode_list	*ptnode2;

  rcn_checkwritesignal(ptsig );

  if( ! ptsig->PRCN ) {
    rcn_error( 5, AVT_WARNING );
    return NULL ;
  }

  if( n1 == n2 )
    return NULL ;

  ptwire             = heaplowire();
  ptwire->FLAG       = flag;
  ptwire->RESI       = resi;
  ptwire->CAPA       = capa;
  ptwire->NODE1      = n1;
  ptwire->NODE2      = n2;
  ptwire->USER       = NULL;
  ptwire->NEXT       = ptsig->PRCN->PWIRE; 
                                         
  ptsig->PRCN->PWIRE = ptwire;  

  if( gettabnode( ptsig->PRCN ) )
  {
    ptnode1 = givelonode(ptsig, n1);
    ptnode2 = givelonode(ptsig, n2);
    ptnode1->WIRELIST = addchain(ptnode1->WIRELIST,ptwire);
    ptnode2->WIRELIST = addchain(ptnode2->WIRELIST,ptwire);

    switch( RCN_GETFLAG(ptnode1->FLAG,RCN_FLAG_ONE|RCN_FLAG_CROSS|RCN_FLAG_TWO))
    {
      case RCN_FLAG_ONE:
        RCN_CLEARFLAG(ptnode1->FLAG,RCN_FLAG_ONE);
        RCN_SETFLAG(ptnode1->FLAG,RCN_FLAG_TWO);
        break;
      case RCN_FLAG_TWO:
        RCN_CLEARFLAG(ptnode1->FLAG,RCN_FLAG_TWO);
        RCN_SETFLAG(ptnode1->FLAG,RCN_FLAG_CROSS);
      case RCN_FLAG_CROSS:
        break;
      default:
        RCN_SETFLAG(ptnode1->FLAG,RCN_FLAG_ONE);
    }

    switch( RCN_GETFLAG(ptnode2->FLAG,RCN_FLAG_ONE|RCN_FLAG_CROSS|RCN_FLAG_TWO))
    {
      case RCN_FLAG_ONE:
        RCN_CLEARFLAG(ptnode2->FLAG,RCN_FLAG_ONE);
        RCN_SETFLAG(ptnode2->FLAG,RCN_FLAG_TWO);
        break;
      case RCN_FLAG_TWO:
        RCN_CLEARFLAG(ptnode2->FLAG,RCN_FLAG_TWO);
        RCN_SETFLAG(ptnode2->FLAG,RCN_FLAG_CROSS);
        break;
      case RCN_FLAG_CROSS:
        break;
      default:
        RCN_SETFLAG(ptnode2->FLAG,RCN_FLAG_ONE);
    }
  }

  if( n1 >= ptsig->PRCN->NBNODE ) 
    ptsig->PRCN->NBNODE = n1+1;
  if( n2 >= ptsig->PRCN->NBNODE ) 
    ptsig->PRCN->NBNODE = n2+1;

  if (TRACE_MODE == 'Y')
    (void)fprintf(stdout, "--- rcn --- addlowire : between node %ld and node %ld on signal %ld\n",
	         ptwire->NODE1, ptwire->NODE2, ptsig->INDEX);

  return ptwire;
}

/*******************************************************************************
 * function addlowireparam()                                                     *
 *******************************************************************************/
void
addlowireparam(lowire_list *ptlowire, char *param, float value, char *expr)
{
    optparam_list *newparam;
    ptype_list    *ptuser;
    
    ptuser = getptype(ptlowire->USER, OPT_PARAMS);
    if (ptuser == NULL) {
        newparam = addoptparam(NULL, param, value, expr);
        ptlowire->USER = addptype(ptlowire->USER, OPT_PARAMS, newparam);
    }
    else {
        newparam = addoptparam((optparam_list *)ptuser->DATA, param, value, expr);
        ptuser->DATA = newparam;
    }
}

/*******************************************************************************
 * function getlowireparam()                                                     *
 *******************************************************************************/
float
getlowireparam(lowire_list *ptlowire, void *param, char **expr, int *status)
{
    ptype_list    *ptuser;

    ptuser = getptype(ptlowire->USER, OPT_PARAMS);
    if (ptuser != NULL) {
        return getoptparam((optparam_list *)ptuser->DATA, param, expr, status);
    }

    if (expr) *expr = NULL;
    if (status) *status = 0;
    return 0.0;
}

/*******************************************************************************
 * function freelowireparams()                                                  *
 *******************************************************************************/
void
freelowireparams(lowire_list *ptlowire)
{
    ptype_list    *ptuser;

    ptuser = getptype(ptlowire->USER, OPT_PARAMS);
    if (ptuser != NULL) {
        freeoptparams((optparam_list *)ptuser->DATA);
        ptlowire->USER = delptype(ptlowire->USER, OPT_PARAMS);
    }
}

/*******************************************************************************
* function addloctc()                                                          *
*******************************************************************************/

loctc_list* addloctc( ptsig1, node1, ptsig2, node2, value )
losig_list	*ptsig1;
long int	 node1;
losig_list	*ptsig2;
long int	 node2;
float		 value;
{
  loctc_list	*newctc;
  lonode_list	*ptnode1;
  lonode_list	*ptnode2;
  loctc_list    *nextctc;

  rcn_checkwritesignal(ptsig1);
  rcn_checkwritesignal(ptsig2);

  if( ptsig1 == ptsig2 ) 
    return NULL;
  
  if( !ptsig1->PRCN ) {
    rcn_error( 6, AVT_WARNING );
    return NULL ;
  }

  if( !ptsig2->PRCN ) {
    rcn_error( 7, AVT_WARNING );
    return NULL ;
  }

  newctc             = heaploctc();

  newctc->CAPA       = value;
  newctc->SIG1       = ptsig1;
  newctc->NODE1      = node1;
  newctc->SIG2       = ptsig2;
  newctc->NODE2      = node2;
  newctc->PREV1      = NULL;
  newctc->PREV2      = NULL;

  ptsig1->PRCN->PCTC = addchain( ptsig1->PRCN->PCTC, newctc );
  if( ptsig1->PRCN->PCTC->NEXT ) {
    nextctc = (loctc_list*) ptsig1->PRCN->PCTC->NEXT->DATA;
    if( nextctc->SIG1 == ptsig1 )
      nextctc->PREV1 = ptsig1->PRCN->PCTC;
    else
      nextctc->PREV2 = ptsig1->PRCN->PCTC;
  }
  
  ptsig2->PRCN->PCTC = addchain( ptsig2->PRCN->PCTC, newctc );
  if( ptsig2->PRCN->PCTC->NEXT ) {
    nextctc = (loctc_list*) ptsig2->PRCN->PCTC->NEXT->DATA;
    if( nextctc->SIG2 == ptsig2 )
      nextctc->PREV2 = ptsig2->PRCN->PCTC;
    else
      nextctc->PREV1 = ptsig2->PRCN->PCTC;
  }


  if( gettabnode(ptsig1->PRCN) && node1 )
  {
    ptnode1          = givelonode( ptsig1, node1 );
    ptnode1->CTCLIST = addchain( ptnode1->CTCLIST, newctc );
  }

  if( ptsig1->PRCN->NBNODE <= node1 )
    ptsig1->PRCN->NBNODE = node1+1;

  if( gettabnode(ptsig2->PRCN) && node2 )
  {
    ptnode2          = givelonode( ptsig2, node2 );
    ptnode2->CTCLIST = addchain( ptnode2->CTCLIST, newctc );
  }

  if( ptsig2->PRCN->NBNODE <= node2 )
    ptsig2->PRCN->NBNODE = node2+1;

  return( newctc );
}

/*******************************************************************************
 * function addloctcparam()                                                     *
 *******************************************************************************/
void
addloctcparam(loctc_list *ptloctc, char *param, float value, char *expr)
{
    optparam_list *newparam;
    ptype_list    *ptuser;
    
    ptuser = getptype(ptloctc->USER, OPT_PARAMS);
    if (ptuser == NULL) {
        newparam = addoptparam(NULL, param, value, expr);
        ptloctc->USER = addptype(ptloctc->USER, OPT_PARAMS, newparam);
    }
    else {
        newparam = addoptparam((optparam_list *)ptuser->DATA, param, value, expr);
        ptuser->DATA = newparam;
    }
}

/*******************************************************************************
 * function getloctcparam()                                                     *
 *******************************************************************************/
float
getloctcparam(loctc_list *ptloctc, void *param, char **expr, int *status)
{
    ptype_list    *ptuser;

    ptuser = getptype(ptloctc->USER, OPT_PARAMS);
    if (ptuser != NULL) {
        return getoptparam((optparam_list *)ptuser->DATA, param, expr, status);
    }

    if (expr) *expr = NULL;
    if (status) *status = 0;
    return 0.0;
}

/*******************************************************************************
 * function freeloctcparams()                                                  *
 *******************************************************************************/
void
freeloctcparams(loctc_list *ptloctc)
{
    ptype_list    *ptuser;

    ptuser = getptype(ptloctc->USER, OPT_PARAMS);
    if (ptuser != NULL) {
        freeoptparams((optparam_list *)ptuser->DATA);
        ptloctc->USER = delptype(ptloctc->USER, OPT_PARAMS);
    }
    ptloctc->USER = testanddelptype(ptloctc->USER, MSL_CAPANAME);
}
 
/******************************************************************************\
Fonctions de renumérotation des noeuds.
Ces fonctions effacent la table des noeuds. Ca peut se changer facilement.
\******************************************************************************/
void rcn_changewirenodes( losig_list *losig, lowire_list *wire, int n1, int n2 )
{
  freetable( losig );
  if( n1 >= losig->PRCN->NBNODE )
    losig->PRCN->NBNODE = n1+1;
  if( n2 >= losig->PRCN->NBNODE )
    losig->PRCN->NBNODE = n2+1;
    
  wire->NODE1 = n1;
  wire->NODE2 = n2;
}

void rcn_changectcnodes( loctc_list *ctc, int n1, int n2 )
{
  freetable( ctc->SIG1 );
  freetable( ctc->SIG2 );
  
  if( n1 >= ctc->SIG1->PRCN->NBNODE )
    ctc->SIG1->PRCN->NBNODE = n1+1;
  if( n2 >= ctc->SIG2->PRCN->NBNODE )
    ctc->SIG2->PRCN->NBNODE = n2+1;
    
  ctc->NODE1 = n1;
  ctc->NODE2 = n2;
}

/*******************************************************************************
* function freectclist()                                                       *
* Efface toute les ctc sur une liste de signaux. Si la chaine est NULL, alors  *
* tous les signaux de la lofig sont effacés.                                   *
*******************************************************************************/
void freectclist( ptfig, headsig )
lofig_list      *ptfig;
chain_list      *headsig;
{
  chain_list    *chainsig;
  losig_list    *losig;

  if( headsig == NULL ) {
    for( losig = ptfig->LOSIG ; losig ; losig = losig->NEXT ) {
      if( losig->PRCN ) {
        while( losig->PRCN->PCTC )
          delloctc( (loctc_list*)losig->PRCN->PCTC->DATA );
      }
    }
  }
  else {
    for( chainsig = headsig ; chainsig ; chainsig = chainsig->NEXT ) {
      losig = (losig_list*)chainsig->DATA;
      if( losig->PRCN ) {
        while( losig->PRCN->PCTC )
          delloctc( (loctc_list*)losig->PRCN->PCTC->DATA );
      }
    }
  }
}

/*******************************************************************************
* function addlonode()                                                         *
*******************************************************************************/
long addlonode(ptsig, ptlocon)
losig_list *ptsig;
locon_list *ptlocon;
{
  int   dispo;
  
  rcn_checkwritesignal(ptsig);

  if( ! ptsig->PRCN ) {
    rcn_error( 8, AVT_WARNING );
    return -1;
  }

  dispo = ptsig->PRCN->NBNODE;
  ptsig->PRCN->NBNODE++ ;

  if(ptlocon != NULL)
    setloconnode(ptlocon, dispo );

  if (TRACE_MODE == 'Y')
  (void)fprintf(stdout, "--- rcn --- addlonode	  : on signal %ld\n",
                ptsig->INDEX);
                
  return( dispo );
}

/*******************************************************************************
* function freelorcnet()                                                       *
*******************************************************************************/
int freelorcnet(signal)
losig_list	*signal;
{
  lowire_list		*scanwire;
  lowire_list		*prevwire;
  ptype_list		*pt;
  chain_list		*scancon;
  locon_list		*ptcon;
  chain_list            *delsig;
  
  rcn_checkwritesignal(signal);

  if( ! signal->PRCN ) {
    rcn_error( 9, AVT_WARNING );
    return 0 ;
  }

  /* Libère la table des noeuds */
  if( gettabnode(signal->PRCN) )
    freetable(signal);

  pt = getptype( signal->USER, LOFIGCHAIN );
  if( pt ) {
    /* Efface les PNODEs des locons sur le signal */
    for( scancon = (chain_list*)(pt->DATA) ; scancon ; scancon = scancon->NEXT )
    {
      ptcon = (locon_list*)(scancon->DATA);
  
      if( ptcon->PNODE )
      {
        freenum(ptcon->PNODE);
        ptcon->PNODE = NULL;
      }
    }
  }

  /* efface les wire */
  
  if( signal->PRCN->PWIRE )
  {
    prevwire = NULL;
  
    for( scanwire = signal->PRCN->PWIRE ;
         scanwire != NULL ;
         scanwire = scanwire->NEXT)
    {
      if(prevwire)
        freelowire(prevwire);
      prevwire=scanwire;
    }

    if(prevwire)
      freelowire(prevwire);
  }

  /* efface les ctc */

  delsig = addchain( NULL, signal );
  freectclist( NULL, delsig );
  freechain( delsig );
  
  /* Finallement, on efface le reseau lui meme */
  freercnet(signal->PRCN);
  signal->PRCN = NULL;

  return(1);
}

/*******************************************************************************
* function dellowire()                                                         *
*******************************************************************************/
int dellowire(ptsig, node1, node2)
losig_list *ptsig;
long node1;
long node2;
{
lowire_list	*ptwire;
lowire_list	*ptsav=NULL;
lonode_list	*ptnode1;
lonode_list	*ptnode2;
chain_list	*ptchain;

  rcn_checkwritesignal(ptsig);

  if( ! ptsig->PRCN ) {
    rcn_error( 11, AVT_WARNING );
    return 0 ;
  }

  for (ptwire = ptsig->PRCN->PWIRE; ptwire != NULL ; ptwire = ptwire->NEXT)
  {
    if(   (ptwire->NODE1 == node1 && ptwire->NODE2 == node2)
       || (ptwire->NODE1 == node2 && ptwire->NODE2 == node1) )
      break;
    ptsav = (void *)ptwire;
  }

  if (ptwire == NULL)   
    return 0;
  else 
  {
    if (ptwire == ptsig->PRCN->PWIRE) /* premier de la liste */ 
      ptsig->PRCN->PWIRE = ptwire->NEXT;
    else
      ptsav->NEXT        = ptwire->NEXT;
  }

  freelowire(ptwire);

  if( gettabnode(ptsig->PRCN) != NULL)
  {
    ptnode1 = getlonode(ptsig, node1);
    ptnode2 = getlonode(ptsig, node2);

    /* optimiser */
    for(ptchain = ptnode1->WIRELIST;ptchain != NULL;ptchain = ptchain->NEXT)
      if(ptchain->DATA == (void *)ptwire) break;
    ptnode1->WIRELIST = delchain(ptnode1->WIRELIST,ptchain);

    for(ptchain = ptnode2->WIRELIST;ptchain != NULL;ptchain = ptchain->NEXT)
      if(ptchain->DATA == (void *)ptwire) break;
    ptnode2->WIRELIST = delchain(ptnode2->WIRELIST,ptchain);

    switch(RCN_GETFLAG(ptnode1->FLAG,RCN_FLAG_CROSS|RCN_FLAG_ONE|RCN_FLAG_TWO))
    {
      case RCN_FLAG_CROSS:
        if(ptnode1->WIRELIST->NEXT->NEXT==NULL)
        {
          RCN_CLEARFLAG(ptnode1->FLAG,RCN_FLAG_CROSS);
          RCN_SETFLAG(ptnode1->FLAG,RCN_FLAG_TWO);
        }
        break;
      case RCN_FLAG_TWO:
        RCN_CLEARFLAG(ptnode1->FLAG,RCN_FLAG_TWO);
        RCN_SETFLAG(ptnode1->FLAG,RCN_FLAG_ONE);
        break;
      case RCN_FLAG_ONE:
        RCN_CLEARFLAG(ptnode1->FLAG,RCN_FLAG_ONE);
        break;
      default:
        rcn_error( 12, AVT_WARNING );
    }

    switch(RCN_GETFLAG(ptnode2->FLAG,RCN_FLAG_CROSS|RCN_FLAG_ONE|RCN_FLAG_TWO))
    {
      case RCN_FLAG_CROSS:
        if(ptnode2->WIRELIST->NEXT->NEXT==NULL)
        {
          RCN_CLEARFLAG(ptnode2->FLAG,RCN_FLAG_CROSS);
          RCN_SETFLAG(ptnode2->FLAG,RCN_FLAG_TWO);
        }
        break;
      case RCN_FLAG_TWO:
        RCN_CLEARFLAG(ptnode2->FLAG,RCN_FLAG_TWO);
        RCN_SETFLAG(ptnode2->FLAG,RCN_FLAG_ONE);
        break;
      case RCN_FLAG_ONE:
        RCN_CLEARFLAG(ptnode2->FLAG,RCN_FLAG_ONE);
        break;
      default:
        rcn_error( 13, AVT_WARNING );
    }
  }
  
  if (TRACE_MODE == 'Y')
    (void)fprintf(stdout, "--- rcn --- dellowire  : between node %ld and node %ld on signal %ld\n",
                           node1, node2, ptsig->INDEX);

  return 1;
}

/*******************************************************************************
* function mergedellowire()                                                    *
*******************************************************************************/
void mergedellowire(ptsig)
losig_list *ptsig;
{
  lowire_list *ptwire;
  lowire_list *ptprev;
  lowire_list *suiv;

  rcn_checkwritesignal(ptsig);

  if( !ptsig->PRCN ) {
    rcn_error( 14, AVT_WARNING );
    return ;
  }

  ptprev = NULL;
  for( ptwire = ptsig->PRCN->PWIRE ; ptwire ; ptwire = suiv )
  {
    suiv = ptwire->NEXT;
    if( RCN_GETFLAG( ptwire->FLAG, RCN_FLAG_DEL ) )
    {
      if( ptprev == NULL )
        ptsig->PRCN->PWIRE = ptwire->NEXT;
      else
        ptprev->NEXT = ptwire->NEXT;

      freelowire( ptwire );
    }
    else
      ptprev = ptwire;
  }
}

/*******************************************************************************
* function setdellowire()                                                      *
*******************************************************************************/
void setdellowire(ptsig, ptwire)
losig_list  *ptsig;
lowire_list *ptwire;
{
lonode_list	*ptnode1;
lonode_list	*ptnode2;
chain_list	*ptchain;

  rcn_checkwritesignal(ptsig);

  RCN_SETFLAG( ptwire->FLAG, RCN_FLAG_DEL );

  if( gettabnode(ptsig->PRCN) != NULL)
  {
    ptnode1 = getlonode(ptsig, ptwire->NODE1);
    ptnode2 = getlonode(ptsig, ptwire->NODE2);

    /* optimiser */
    for(ptchain = ptnode1->WIRELIST;ptchain != NULL;ptchain = ptchain->NEXT)
      if(ptchain->DATA == (void *)ptwire) break;
    ptnode1->WIRELIST = delchain(ptnode1->WIRELIST,ptchain);

    for(ptchain = ptnode2->WIRELIST;ptchain != NULL;ptchain = ptchain->NEXT)
      if(ptchain->DATA == (void *)ptwire) break;
    ptnode2->WIRELIST = delchain(ptnode2->WIRELIST,ptchain);

    switch(RCN_GETFLAG(ptnode1->FLAG,RCN_FLAG_CROSS|RCN_FLAG_ONE|RCN_FLAG_TWO))
    {
      case RCN_FLAG_CROSS:
        if(ptnode1->WIRELIST->NEXT->NEXT==NULL)
        {
          RCN_CLEARFLAG(ptnode1->FLAG,RCN_FLAG_CROSS);
          RCN_SETFLAG(ptnode1->FLAG,RCN_FLAG_TWO);
        }
        break;
      case RCN_FLAG_TWO:
        RCN_CLEARFLAG(ptnode1->FLAG,RCN_FLAG_TWO);
        RCN_SETFLAG(ptnode1->FLAG,RCN_FLAG_ONE);
        break;
      case RCN_FLAG_ONE:
        RCN_CLEARFLAG(ptnode1->FLAG,RCN_FLAG_ONE);
        break;
      default:
        rcn_error( 15, AVT_WARNING );
    }

    switch(RCN_GETFLAG(ptnode2->FLAG,RCN_FLAG_CROSS|RCN_FLAG_ONE|RCN_FLAG_TWO))
    {
      case RCN_FLAG_CROSS:
        if(ptnode2->WIRELIST->NEXT->NEXT==NULL)
        {
          RCN_CLEARFLAG(ptnode2->FLAG,RCN_FLAG_CROSS);
          RCN_SETFLAG(ptnode2->FLAG,RCN_FLAG_TWO);
        }
        break;
      case RCN_FLAG_TWO:
        RCN_CLEARFLAG(ptnode2->FLAG,RCN_FLAG_TWO);
        RCN_SETFLAG(ptnode2->FLAG,RCN_FLAG_ONE);
        break;
      case RCN_FLAG_ONE:
        RCN_CLEARFLAG(ptnode2->FLAG,RCN_FLAG_ONE);
        break;
      default:
        rcn_error( 16, AVT_WARNING );
    }
  }
}

/*******************************************************************************
* function getlowire()                                                         *
*******************************************************************************/
lowire_list *getlowire(ptsig, node1, node2)
losig_list  *ptsig;
long  node1;
long  node2;
{
  lowire_list  *ptwire;
  lonode_list  *ptnode;
  chain_list   *ptchain;

  if( ! ptsig->PRCN ) {
    rcn_error( 17, AVT_WARNING );
    return NULL ;
  }

  if( ! gettabnode(ptsig->PRCN) )
  {
    for ( ptwire = ptsig->PRCN->PWIRE ; ptwire != NULL ; ptwire = ptwire->NEXT )
      if ( (   (ptwire->NODE1 == node1 && ptwire->NODE2 == node2)
             ||(ptwire->NODE1 == node2 && ptwire->NODE2 == node1) ) &&
           ! RCN_GETFLAG( ptwire->FLAG, RCN_FLAG_DEL )                 )
        return ptwire;
    return(NULL);
  }

  ptnode = givelonode(ptsig,node1);
  for(ptchain=ptnode->WIRELIST;ptchain!=NULL;ptchain=ptchain->NEXT)
  {
    ptwire = (lowire_list*)ptchain->DATA;
    if( ( ptwire->NODE1 == node2 || ptwire->NODE2 == node2  ) &&
        ! RCN_GETFLAG( ptwire->FLAG, RCN_FLAG_DEL )
      )
      return ptwire;
  }
  return(NULL);
}

/*******************************************************************************
* function getlonode()                                                         *
*******************************************************************************/
lonode_list *getlonode(ptsig, index)
losig_list *ptsig;
long  index;
{
  static int  taille;
  static chain_list *head;
  chain_list *scan;
  int         max;
  int         pos;

  if(ptsig != NULL) {

    head   = gettabnode( ptsig->PRCN );
    if( head == NULL) {
      buildtable( ptsig );
      head   = gettabnode( ptsig->PRCN );
    }

    taille = (int)((long)(getptype( ptsig->PRCN->USER, RCN_SIZETAB )->DATA)); 

  }

  if(index < taille)
    return( &(((lonode_list*)(head->DATA))[(int)index]) );

  scan = head;
  pos=0;
  max = index / taille;
  for( pos = 0 ; pos <= max ; pos++ )
  {
    if( !scan ) {
      rcn_error( 18, AVT_WARNING );
      return NULL ;
    }
    if(pos != max)
     scan = scan->NEXT ;
  } 
  
  pos = index - max * taille ;
  
  return( &(((lonode_list*)(scan->DATA))[pos]) );
}
 
/*******************************************************************************
* function givelonode()                                                        *
*******************************************************************************/
lonode_list *givelonode(ptsig, index)
losig_list  *ptsig;
long  index;
{
  int taille;
  ptype_list *head;
  chain_list *scan;
  chain_list *prev;
  int max;
  int pos;
  int i;
  lonode_list *ptnode;

  if( !ptsig->PRCN ) {
    rcn_error( 19, AVT_WARNING );
    return NULL ;
  }

  if( index >= ptsig->PRCN->NBNODE )
    ptsig->PRCN->NBNODE = index+1;

  head   = getptype( ptsig->PRCN->USER, RCN_LONODE );
  if( head == NULL) {
    rcn_error( 20, AVT_WARNING );
    return NULL ;
  }

  taille = (int)((long)(getptype( ptsig->PRCN->USER, RCN_SIZETAB )->DATA)); 
  scan   = head->DATA;
  prev   = head->DATA;

  pos=0;
  max = index / taille;
  for( pos = 0 ; pos <= max ; pos++ )
  {
    if( !scan )
    {
      if( prev )
      {
        prev->NEXT = addchain( NULL, 
                               mbkalloc( sizeof( lonode_list ) * taille )
                             );
        scan = prev->NEXT;
      }
      else
      {
        head->DATA = addchain( NULL,
                               mbkalloc( sizeof( lonode_list ) * taille )
                             );
        scan = head->DATA;
      }

      for( i = 0 ; i < taille ; i++ )
      {
        ptnode = &(((lonode_list*)(scan->DATA))[i]);
        ptnode->INDEX = i + pos * taille ;
        ptnode->WIRELIST = NULL;
        ptnode->CTCLIST  = NULL;
        ptnode->FLAG     = 0;
        ptnode->USER     = NULL;
      }
        
    }
    prev = scan;
    if(pos != max)
     scan = scan->NEXT ;
  } 
  
  pos = index - max * taille ;
  
  return( &(((lonode_list*)(scan->DATA))[pos]) );
}
 
/*******************************************************************************
* function getloconnode()                                                      *
*******************************************************************************/

chain_list* getloconnode( node )
lonode_list *node;
{
  ptype_list		*ptptype;

  ptptype = getptype( node->USER, RCN_LOCON );
  if( !ptptype )
    return( NULL );

  return( (chain_list*)ptptype->DATA );
}
 
/*******************************************************************************
* function setloconnode()                                                      *
*******************************************************************************/

void setloconnode(ptlocon, node)
locon_list *ptlocon;
long node;
{ 
  losig_list		*ptsig;
  ptype_list		*ptptype;
  chain_list		*chain;
  lonode_list		*ptnode;

  ptsig = ptlocon->SIG;

  rcn_checkwritesignal(ptsig);

  if( ! ptsig->PRCN ) {
    rcn_error( 21, AVT_WARNING );
    return ;
  }

  if( ptsig->PRCN->NBNODE <= node )
    ptsig->PRCN->NBNODE = node+1;

  if( gettabnode( ptsig->PRCN ) != NULL)
  {
    ptnode = givelonode( ptsig, node );

    if(!RCN_GETFLAG(ptnode->FLAG,RCN_FLAG_LOCON))
    {
      RCN_SETFLAG(ptnode->FLAG,RCN_FLAG_LOCON);
      ptnode->USER = addptype(ptnode->USER,RCN_LOCON,addchain(NULL,ptlocon));
    }
    else
    {
      ptptype = getptype(ptnode->USER,RCN_LOCON);
      for(chain = (chain_list *)ptptype->DATA; chain; chain = chain->NEXT)
        if(ptlocon == (locon_list *)chain->DATA)
          break;

      if(chain == NULL) 
        ptptype->DATA = addchain(ptptype->DATA,ptlocon);
    }
  }

  ptlocon->PNODE = addnum( ptlocon->PNODE, node );

}

/*******************************************************************************
* function delloconnode()                                                      *
*******************************************************************************/

void delloconnode(ptcon, index )
locon_list	*ptcon;
long		 index;
{
  losig_list	*ptsig;
  int		 dernier;
  num_list	*scannum;
  num_list	*prev;
  ptype_list    *ptptype;
  chain_list    *scanchain;
  chain_list    *prevchain;
  lonode_list   *ptnode;


  ptsig = ptcon->SIG;
  rcn_checkwritesignal(ptsig);

  if( ! ptsig->PRCN ) {
    rcn_error( 22, AVT_WARNING );
    return ;
  }

  prev = NULL;
  for( scannum = ptcon->PNODE ; scannum ; scannum = scannum->NEXT )
  {
    if( scannum->DATA == index )
      break;
    prev = scannum;
  }
  
  dernier = 0;

  if( prev )
    prev->NEXT = scannum->NEXT;
  else
  {
    ptcon->PNODE = scannum->NEXT;
    if( ptcon->PNODE == NULL )
      dernier = 1;
  }

  scannum->NEXT = NULL;
  freenum( scannum );

  ptnode = NULL;

  if( gettabnode( ptsig->PRCN ) )
  {
    ptnode = getlonode( ptsig, index );
    ptptype = getptype( ptnode->USER, RCN_LOCON );

    prevchain = NULL;

    for( scanchain = (chain_list*)ptptype->DATA ; 
         scanchain ;
         scanchain = scanchain->NEXT
       )
    {
      if( scanchain->DATA == ptcon )
        break;
      prevchain = scanchain;
    }

    if( prevchain )
      prevchain->NEXT = scanchain->NEXT;
    else
      ptptype->DATA = scanchain->NEXT;
      
    scanchain->NEXT = NULL;
    freechain( scanchain );
    
    if( ptptype->DATA == NULL )
    {
      ptnode->USER = delptype( ptnode->USER, RCN_LOCON );
      RCN_CLEARFLAG( ptnode->FLAG, RCN_FLAG_LOCON );
    }
  }
}

/*******************************************************************************
* function addcapawire()                                                       *
*******************************************************************************/

void addcapawire(ptwire, capa)
lowire_list *ptwire;
float capa;
{
	ptwire->CAPA += capa;
}

/*******************************************************************************
* function setcapawire()                                                       *
*******************************************************************************/

void setcapawire(ptwire, capa)
lowire_list *ptwire;
float capa;
{
	ptwire->CAPA = capa;
}

/*******************************************************************************
* function addresiwire()                                                       *
*******************************************************************************/
void addresiwire(ptwire, resi)
lowire_list *ptwire;
float resi;
{
	ptwire->RESI += resi;
}

/*******************************************************************************
* function setresiwire()                                                       *
*******************************************************************************/
void setresiwire(ptwire, resi)
lowire_list *ptwire;
float resi;
{
	ptwire->RESI = resi;
}

/*******************************************************************************
* function addpararesiwire()                                                   *
*******************************************************************************/
void addpararesiwire(ptwire, resi)
lowire_list *ptwire;
float resi;
{
        if(ptwire->RESI==0.0 || resi==0.0)
	  ptwire->RESI = 0.0;
        else
          ptwire->RESI = 1 / ( 1/ptwire->RESI + 1/resi );
}

/*******************************************************************************
* lowire_list* heaplowire(void)						       *
*******************************************************************************/
static void **HEADLOWIRE=NULL;
static int COUNTLOWIRE=0;
#define COUNTLOWIREMAX 1000000

lowire_list* heaplowire(void)
{
  lowire_list *ptwire;

  if (HEADLOWIRE!=NULL)
    {
      ptwire=(lowire_list*)HEADLOWIRE;
      HEADLOWIRE=*HEADLOWIRE;
      COUNTLOWIRE--;
    }
  else
    ptwire = (lowire_list*)mbkalloc(sizeof(lowire_list));
  
  /* Clean the new wire */
  ptwire->NEXT		= NULL;
  ptwire->NODE1		= 0L;
  ptwire->NODE2		= 0L;
  ptwire->FLAG 		= 0;
  ptwire->RESI		= 0.0;
  ptwire->CAPA		= 0.0;
  ptwire->USER		= NULL;

  return(ptwire);
}

/*******************************************************************************
* loctc_list* heaploctc(void)   					       *
*******************************************************************************/
static void **HEADLOCTC=NULL;
static int COUNTLOCTC=0;
#define COUNTLOCTCMAX 1000000

loctc_list* heaploctc(void)
{
  loctc_list *ptctc;

  if (HEADLOCTC!=NULL)
    {
      ptctc=(loctc_list*)HEADLOCTC;
      HEADLOCTC=*HEADLOCTC;
      COUNTLOCTC--;
    }
  else
    ptctc=(loctc_list*)mbkalloc(sizeof(loctc_list));

  ptctc->SIG1	 = NULL;
  ptctc->NODE1	 = 0;
  ptctc->SIG2	 = NULL;
  ptctc->NODE2	 = 0;
  ptctc->CAPA    = 0.0;
  ptctc->USER    = NULL;

  return(ptctc);
}
/*******************************************************************************
* lorcnet_list* heaprcnet(void)						       *
*******************************************************************************/

lorcnet_list* heaprcnet(void)
{
  lorcnet_list *ptrcnet;

  ptrcnet=(lorcnet_list*)mbkalloc(sizeof(lorcnet_list));

  /* Clean the new rcnet figure */

  ptrcnet->RCCAPA	= 0.0;
  ptrcnet->PWIRE	= NULL;
  ptrcnet->NBNODE	= 1l;
  ptrcnet->PCTC		= NULL;
  ptrcnet->USER		= NULL;

  return(ptrcnet);
}

/*******************************************************************************
* void freercnet(lorcnet_list*)						       *
*******************************************************************************/

void freercnet(pt)
lorcnet_list	*pt;
{
  mbkfree(pt);
}

/*******************************************************************************
* void freelowire(lowire_list*)						       *
*******************************************************************************/

void freelowire(pt)
lowire_list	*pt;
{
 // static int msg=0;
  freelowireparams(pt);
/*  if( pt->USER && msg==0 )
  {
     fflush( stdout );
     fprintf( stderr, 
              "\n\n*** Warning : freelowire. USER field not empty (0x%08X).\n",
              (unsigned int)pt->USER->TYPE
            );
     msg=1;
  }
*/
  freeptype( pt->USER );

  *(void **)pt=HEADLOWIRE; HEADLOWIRE=(void **)pt;
  COUNTLOWIRE++;
  if (COUNTLOWIRE>=COUNTLOWIREMAX)
    {
      void *run;
      while (HEADLOWIRE!=NULL)
        {
          run=*(void **)HEADLOWIRE;
          mbkfree(HEADLOWIRE);
          HEADLOWIRE=run;
        }
      COUNTLOWIRE=0;
    }
}

/*******************************************************************************
* void freeloctc(loctc_list*)						       *
*******************************************************************************/

void freeloctc(pt)
loctc_list	*pt;
{
  static int msg=0;
  
  freeloctcparams ( pt );
  rcn_unsetCapaDiode( pt );
  if( pt->USER && msg==0 )
  {
     fflush( stdout );
     fprintf( stderr, "\n\n*** Warning : freeloctc. USER field not empty.\n" );
     msg=1;
  }

  freeptype( pt->USER );

  *(void **)pt=HEADLOCTC; HEADLOCTC=(void **)pt;
  COUNTLOCTC++;
  if (COUNTLOCTC>=COUNTLOCTCMAX)
    {
      void *run;
      while (HEADLOCTC!=NULL)
        {
          run=*(void **)HEADLOCTC;
          mbkfree(HEADLOCTC);
          HEADLOCTC=run;
        }
      COUNTLOCTC=0;
    }
}

/*******************************************************************************
* void freetable(rcnet_list*)						       *
*******************************************************************************/

void freetable(ptsig)
losig_list	*ptsig;
{
  chain_list	*head;
  chain_list	*scan;
  int		 taille;
  ptype_list	*ptl;
  int            i;
  lonode_list   *ptnode;

  if( !ptsig->PRCN ) {
    rcn_error( 23, AVT_WARNING );
    return ;
  }

  ptl = getptype( ptsig->PRCN->USER, RCN_LONODE );
  if( ptl == NULL)
    return;
  head = (chain_list*)(ptl->DATA);

  taille = (int)((long)(getptype( ptsig->PRCN->USER, RCN_SIZETAB )->DATA ));
  for( scan = head; scan ; scan = scan->NEXT )
  {
    for( i=0 ; i<taille ; i++ )
    {
      ptnode = &(((lonode_list*)(scan->DATA))[i]);
      if( RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_LOCON ) )
      {
        ptl = getptype( ptnode->USER, RCN_LOCON );
        if( ptl )
          freechain( ptl->DATA );
        ptnode->USER = delptype( ptnode->USER, RCN_LOCON );
      }
      freechain( ptnode->WIRELIST );
      freechain( ptnode->CTCLIST  );
      if( ptnode->USER )
      {
        fflush( stdout );
        fprintf( stderr, 
                 "*** rcn warning in freetable : non empty USER (0x%X).\n",
                 (unsigned)ptnode->USER->TYPE);
      }
    }

    mbkfree( scan->DATA );
  }

  freechain(head);
  ptsig->PRCN->USER = delptype( ptsig->PRCN->USER, RCN_LONODE  );
  ptsig->PRCN->USER = delptype( ptsig->PRCN->USER, RCN_SIZETAB );

  rcn_removelosigfromcachetable( ptsig );
}

/*******************************************************************************
* void buildtable(ptsig)						       *
*******************************************************************************/

void buildtable(ptsig)
losig_list	*ptsig;
{
  lowire_list		*ptwire;
  lonode_list		*ptnode1;
  lonode_list		*ptnode2;
  chain_list		*scancon;
  locon_list		*ptcon;
  num_list		*scannum;
  lonode_list		*ptnode;
  ptype_list		*ptlocon;
  ptype_list		*pt;
  chain_list		*scanctc;
  loctc_list		*ptloctc;

  if( ! ptsig->PRCN ) return;

  pt = getptype( ptsig->USER, LOFIGCHAIN );
  if( !pt ) {
    rcn_error( 24, AVT_WARNING );
    return ;
  }

  if( gettabnode( ptsig->PRCN ) ) return;

  rcn_addlosigincachetable( ptsig );

  ptsig->PRCN->USER = addptype( ptsig->PRCN->USER,
                                RCN_SIZETAB,
                                (void*)(ptsig->PRCN->NBNODE + 1)
                              );
  ptsig->PRCN->USER = addptype( ptsig->PRCN->USER,
                                RCN_LONODE,
                                NULL
                              );
  /* Cree les tables million pour tous les noeuds jusqu'a nbnode */
  givelonode(ptsig,ptsig->PRCN->NBNODE-1);

  if( ptsig->PRCN->PWIRE )
  {
    for( ptwire = ptsig->PRCN->PWIRE ; ptwire != NULL ; ptwire = ptwire->NEXT )
    {
      if( RCN_GETFLAG( ptwire->FLAG, RCN_FLAG_DEL ) )
        continue;
      ptnode1           = givelonode(ptsig,ptwire->NODE1);
      ptnode2           = givelonode(ptsig,ptwire->NODE2);
      ptnode1->WIRELIST = addchain(ptnode1->WIRELIST,ptwire);
      ptnode2->WIRELIST = addchain(ptnode2->WIRELIST,ptwire);
  
      switch( 
       RCN_GETFLAG( ptnode1->FLAG, RCN_FLAG_ONE | RCN_FLAG_CROSS | RCN_FLAG_TWO)
	    )
      {
        case RCN_FLAG_ONE:
          RCN_CLEARFLAG(ptnode1->FLAG,RCN_FLAG_ONE);
          RCN_SETFLAG(ptnode1->FLAG,RCN_FLAG_TWO);
          break;
        case RCN_FLAG_TWO:
          RCN_CLEARFLAG(ptnode1->FLAG,RCN_FLAG_TWO);
          RCN_SETFLAG(ptnode1->FLAG,RCN_FLAG_CROSS);
          break;
        case RCN_FLAG_CROSS:
          break;
        default:
          RCN_SETFLAG(ptnode1->FLAG,RCN_FLAG_ONE);
      }
  
      switch(
       RCN_GETFLAG( ptnode2->FLAG, RCN_FLAG_ONE | RCN_FLAG_CROSS | RCN_FLAG_TWO)
	    )
      {
        case RCN_FLAG_ONE:
          RCN_CLEARFLAG(ptnode2->FLAG,RCN_FLAG_ONE);
          RCN_SETFLAG(ptnode2->FLAG,RCN_FLAG_TWO);
          break;
        case RCN_FLAG_TWO:
          RCN_CLEARFLAG(ptnode2->FLAG,RCN_FLAG_TWO);
          RCN_SETFLAG(ptnode2->FLAG,RCN_FLAG_CROSS);
          break;
        case RCN_FLAG_CROSS:
          break;
        default:
          RCN_SETFLAG(ptnode2->FLAG,RCN_FLAG_ONE);
      }
    }  
  }

  for( scanctc = ptsig->PRCN->PCTC ; scanctc ; scanctc = scanctc->NEXT )
  {
    ptloctc = (loctc_list*)( scanctc->DATA );
    if( ptloctc->SIG1 == ptsig && ptloctc->NODE1 )
    {
      ptnode1 = givelonode( ptsig, ptloctc->NODE1 );
      ptnode1->CTCLIST = addchain( ptnode1->CTCLIST, ptloctc );
    }
    if( ptloctc->SIG2 == ptsig && ptloctc->NODE2 )
    {
      ptnode2 = givelonode( ptsig, ptloctc->NODE2 );
      ptnode2->CTCLIST = addchain( ptnode2->CTCLIST, ptloctc );
    }
  }

  for( scancon = (chain_list*)(pt->DATA) ; scancon ; scancon = scancon->NEXT )
  {
    ptcon = (locon_list*)(scancon->DATA);

    for( scannum = ptcon->PNODE ; scannum ; scannum = scannum->NEXT )
    {
      ptnode = givelonode( ptsig, scannum->DATA );

      if( RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_LOCON ) )
      {
        ptlocon = getptype(ptnode->USER, RCN_LOCON );
        ptlocon->DATA=addchain((chain_list*)(ptlocon->DATA),ptcon);
      }
      else
      {
        RCN_SETFLAG( ptnode->FLAG, RCN_FLAG_LOCON );
        ptnode->USER = addptype( ptnode->USER, RCN_LOCON, addchain(NULL,ptcon));
      }
    }
  }
}

/*******************************************************************************
*                                                                              *
*******************************************************************************/

chain_list*	gettabnode( prcnet )
lorcnet_list		*prcnet;
{
  ptype_list		*pt;

  pt = getptype( prcnet->USER, RCN_LONODE );
  if( pt )
    return( (chain_list*)pt->DATA );
  return( NULL );
}

/*******************************************************************************
*                                                                              *
*******************************************************************************/

void delrcnlocon( ptloc )
 locon_list	*ptloc;
{
  rcn_checkwritesignal(ptloc->SIG);

  freenum( ptloc->PNODE );
}

/*******************************************************************************
*                                                                              *
*******************************************************************************/

int chkloopmain( ptsig, flag, org, from )
losig_list	*ptsig;
unsigned char	 flag;
long		 org;
lowire_list	*from;
{
  lonode_list	*ptnode;
  long		 current_node;
  int		 r;
  chain_list	*scanchain;
  lowire_list	*current_wire;

  ptnode = getlonode(NULL,org);
  if( RCN_GETFLAG(ptnode->FLAG, flag) )
    return(2);

  RCN_SETFLAG( ptnode->FLAG, flag );

  /*
  Récursif sur les branches multiples
  Itératif entre 2 wires
  */

  for( scanchain = ptnode->WIRELIST ; scanchain ; scanchain = scanchain->NEXT )
  {
    current_wire = (lowire_list*)(scanchain->DATA);
    if( current_wire != from )
    {
      current_node = (current_wire->NODE1==org ? current_wire->NODE2 :
                                                 current_wire->NODE1 );
      ptnode = getlonode( NULL, current_node );

      while( RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_TWO ) )
      {
        if( RCN_GETFLAG(ptnode->FLAG,flag) )
          return(2);

        RCN_SETFLAG(ptnode->FLAG,flag);

        current_wire=(ptnode->WIRELIST->DATA==current_wire ?
                      ptnode->WIRELIST->NEXT->DATA :
                      ptnode->WIRELIST->DATA                  );

        current_node=(current_wire->NODE1==current_node ? current_wire->NODE2 :
                                                 current_wire->NODE1 );

        ptnode=getlonode(NULL,current_node);
      }

      if( RCN_GETFLAG(ptnode->FLAG,RCN_FLAG_ONE))
      {
        if(RCN_GETFLAG(ptnode->FLAG,flag))
          return(2);
        RCN_SETFLAG(ptnode->FLAG,flag);
      }

      if( RCN_GETFLAG(ptnode->FLAG,RCN_FLAG_CROSS))
      {
        r = chkloopmain(ptsig,flag,current_node,current_wire);
        if(r)
          return(r);
      }
    }
  }
  return(0);
}

int chkloop( ptsig )
losig_list	*ptsig;
{
  int		 r=0;
  long		 i;
  lonode_list	*ptnode;
  lowire_list   *scanwire;

  if( ! ptsig->PRCN ) {
    rcn_error( 25, AVT_WARNING );
    return 0 ;
  }

  getlonode(ptsig,0) ;

  clearallwireflag(ptsig,RCN_FLAG_PASS);

  for( scanwire = ptsig->PRCN->PWIRE ; scanwire ; scanwire = scanwire->NEXT )
  {
    i = scanwire->NODE1;
    ptnode = givelonode( ptsig, i );
    if( !RCN_GETFLAG(ptnode->FLAG,RCN_FLAG_PASS)  )
    {
      r = chkloopmain( ptsig, RCN_FLAG_PASS, i, NULL );

      if(r)
         break;
    }
  }

  clearallwireflag(ptsig,RCN_FLAG_PASS);

  return(r);
}

/*******************************************************************************
* void clearallwireflag( ptrcnet,flag )                                        *
*******************************************************************************/

void clearallwireflag(losig, flag)
losig_list      *losig;
unsigned char    flag;
{
  ptype_list		*head;
  lowire_list           *pwire;
  lonode_list           *ptnode;

  head = getptype( losig->PRCN->USER, RCN_LONODE );
  if( ! head )
    return;

  for( pwire = losig->PRCN->PWIRE ; pwire ; pwire = pwire->NEXT ) {
    ptnode = getlonode( losig, pwire->NODE1 );
    RCN_CLEARFLAG( ptnode->FLAG, flag );
    ptnode = getlonode( losig, pwire->NODE2 );
    RCN_CLEARFLAG( ptnode->FLAG, flag );
  }
}

int	rclevel( ptsig )
losig_list	*ptsig;
{
  if( ! ptsig->PRCN )
    return( MBK_RC_A );
  if( ptsig->PRCN->PWIRE != NULL && ptsig->PRCN->PCTC != NULL )
    return( MBK_RC_E );
  if( ptsig->PRCN->PWIRE == NULL && ptsig->PRCN->PCTC == NULL )
    return( MBK_RC_B );
  if( ptsig->PRCN->PCTC == NULL )
    return( MBK_RC_C );
  if( ptsig->PRCN->PWIRE == NULL )
    return( MBK_RC_D );

  /* never reached */
  return(0);
}

chain_list      *getallctc( ptfig )
lofig_list      *ptfig;
{
  chain_list    *headctc;
  losig_list    *scansig;
  chain_list    *scanctc;
  loctc_list    *ptctc;

  if( rcn_getlofigcache( ptfig ) ) {
    rcn_error( 26, AVT_WARNING );
    return NULL ;
  }

  headctc = NULL;
  
  for( scansig = ptfig->LOSIG ; scansig ; scansig = scansig->NEXT )
  {
    if( scansig->PRCN )
    {
      for( scanctc = scansig->PRCN->PCTC ; scanctc ; scanctc = scanctc->NEXT )
      {
        ptctc = (loctc_list*)(scanctc->DATA);
        
        if( ! getptype( ptctc->USER, RCN_REPORTED ) )
        {
          ptctc->USER = addptype( ptctc->USER, RCN_REPORTED, NULL );
          headctc = addchain( headctc, ptctc );
        }
      }
    }  
  }

  for( scanctc = headctc ; scanctc ; scanctc = scanctc->NEXT )
  {
    ptctc = (loctc_list*)(scanctc->DATA);

    ptctc->USER = delptype( ptctc->USER, RCN_REPORTED );
  }

  return( headctc );
}

void chkrcn( ptsig )
losig_list	*ptsig;
{
  int            trouve;
  lonode_list   *ptnode;
  chain_list    *scanchain;
  chain_list    *verifchain;
  lowire_list   *scanwire;
  int            ctcok;
  int            wireok;
  int            conok;
  int            i;
  int            nbwire;
  ptype_list    *ptptype;
  locon_list    *ptcon;
  num_list      *scannum;
  chain_list    *scanchcon;
  loctc_list    *ptctc;
  loctc_list    *ptctc2;
  losig_list    *sig2;
   

  
  if( !ptsig->PRCN )
    return;

  printf( "Verification coherence vue RC pour signal %ld [%s].\n",
          ptsig->INDEX,
          gettabnode( ptsig->PRCN ) ? "TABLE" : "SANS TABLE"
        );
  
  ctcok  = 1;
  wireok = 1;
  conok  = 1;

  for( scanchain = ptsig->PRCN->PCTC ; scanchain ; scanchain = scanchain->NEXT )
  {
    if( !scanchain->DATA )
    {
      ctcok = 0;
      printf( "   [ 6] CTC nulle sur le signal %ld.\n", ptsig->INDEX );
    }
    ptctc = (loctc_list*)(scanchain->DATA);

    if( ptctc->SIG1 != ptsig && ptctc->SIG2 != ptsig )
    {
      ctcok = 0;
      printf( "   [ 1] CTC pas sur le signal entre %ld.%ld et %ld.%ld.\n",
              ptctc->SIG1->INDEX,
              ptctc->NODE1,
              ptctc->SIG2->INDEX,
              ptctc->NODE2
            );
    }
 
  }
  
  if( gettabnode( ptsig->PRCN ) )
  {
    /* Verification des CTC */

    for( scanchain = ptsig->PRCN->PCTC ;
         scanchain ;
         scanchain = scanchain->NEXT
       )
    {
      ptctc = (loctc_list*)(scanchain->DATA);

      sig2 = NULL;

      if( ptctc->SIG1 != ptsig )
        sig2 = ptctc->SIG1;
      if( ptctc->SIG2 != ptsig )
        sig2 = ptctc->SIG2;

      if( ptctc->SIG1 != ptsig && ptctc->SIG2 != ptsig )
      {
      }
      else
      {
        if( sig2 )
        {
          trouve = 0;
           
          for( verifchain = sig2->PRCN->PCTC ;
               verifchain ;
               verifchain = verifchain->NEXT
             )
          {
            if( verifchain->DATA == ptctc )
            {
              if( trouve )
              {
                ctcok = 0;
                printf( "   [ 2] CTC %ld.%ld et %ld.%ld definie plusieurs fois sur le signal %ld.\n",
                         ptctc->SIG1->INDEX,
                         ptctc->NODE1,
                         ptctc->SIG2->INDEX,
                         ptctc->NODE2,
                         sig2->INDEX
                      );
              }
              trouve = 1;
            }
          }

          if( !trouve )
          {
            ctcok = 0;
            printf( "   [ 3] CTC %ld.%ld et %ld.%ld non definie sur le signal %ld.\n",
                     ptctc->SIG1->INDEX,
                     ptctc->NODE1,
                     ptctc->SIG2->INDEX,
                     ptctc->NODE2,
                     sig2->INDEX
                  );
          }
        }

        if( ptsig == ptctc->SIG1 && ptctc->NODE1 )
        {
          ptnode = getlonode( ptsig, ptctc->NODE1 );
          
          trouve = 0;
          for( verifchain = ptnode->CTCLIST ;
               verifchain ;
               verifchain = verifchain->NEXT
             )
          {
            if( verifchain->DATA == ptctc )
            {
              if( trouve )
              {
                ctcok = 0;
                printf( "   [ 4] CTC %ld.%ld et %ld.%ld definie plusieurs fois sur le noeud %ld.\n",
                         ptctc->SIG1->INDEX,
                         ptctc->NODE1,
                         ptctc->SIG2->INDEX,
                         ptctc->NODE2,
                         ptctc->NODE1
                  );
              }
              trouve = 1;
            }            
          }
          if( !trouve )
          {
            ctcok = 0;
            printf( "   [ 5] CTC %ld.%ld et %ld.%ld non definie sur le noeud %ld.\n",
                     ptctc->SIG1->INDEX,
                     ptctc->NODE1,
                     ptctc->SIG2->INDEX,
                     ptctc->NODE2,
                     ptctc->NODE1
                  );
          }
        }

        if( ptsig == ptctc->SIG2 && ptctc->NODE2 )
        {
          ptnode = getlonode( ptsig, ptctc->NODE2 );
          
          trouve = 0;
          for( verifchain = ptnode->CTCLIST ;
               verifchain ;
               verifchain = verifchain->NEXT
             )
          {
            if( verifchain->DATA == ptctc )
            {
              if( trouve )
              {
                ctcok = 0;
                printf( "   [ 6] CTC %ld.%ld et %ld.%ld definie plusieurs fois sur le noeud %ld.\n",
                         ptctc->SIG1->INDEX,
                         ptctc->NODE1,
                         ptctc->SIG2->INDEX,
                         ptctc->NODE2,
                         ptctc->NODE2
                  );
              }
              trouve = 1;
            }            
          }
          if( !trouve )
          {
            ctcok = 0;
            printf( "   [ 7] CTC %ld.%ld et %ld.%ld non definie sur le noeud %ld.\n",
                     ptctc->SIG1->INDEX,
                     ptctc->NODE1,
                     ptctc->SIG2->INDEX,
                     ptctc->NODE2,
                     ptctc->NODE2
                  );
          }
        }
      }

      for( verifchain = scanchain->NEXT ;
           verifchain ;
           verifchain = verifchain->NEXT
         ) 
      {
        ptctc2 = (loctc_list*)(verifchain->DATA);

        if( ( ptctc->SIG1  == ptctc2->SIG1  &&
              ptctc->NODE1 == ptctc2->NODE1 &&
              ptctc->SIG2  == ptctc2->SIG2  &&
              ptctc->NODE2 == ptctc2->NODE2    ) ||          
            ( ptctc->SIG2  == ptctc2->SIG1  &&
              ptctc->NODE2 == ptctc2->NODE1 &&
              ptctc->SIG1  == ptctc2->SIG2  &&
              ptctc->NODE1 == ptctc2->NODE2    )   )
        {
          ctcok = 0;
          printf( "   [ 8] Plusieurs CTC entre %ld.%ld et %ld.%ld c=%g.\n",
                  ptctc->SIG1->INDEX,
                  ptctc->NODE1,
                  ptctc->SIG2->INDEX,
                  ptctc->NODE2,
                  ptctc->CAPA
                );
        }
      }
    }   

    for( i=1 ; i < ptsig->PRCN->NBNODE ; i++ )
    {
      ptnode = getlonode( ptsig, i );
      if( !ptnode )
        continue;

      for( scanchain = ptnode->CTCLIST ;
           scanchain ;
           scanchain = scanchain->NEXT
         )
      {
        ptctc = (loctc_list*)scanchain->DATA;
        
        if( ptctc->SIG1 != ptsig && ptctc->SIG2 != ptsig )
        {
          ctcok = 0 ;
          printf( "   [ 9] CTC pas sur le signal entre %ld.%ld et %ld.%ld sur noeud %d.\n",
                  ptctc->SIG1->INDEX,
                  ptctc->NODE1,
                  ptctc->SIG2->INDEX,
                  ptctc->NODE2,
                  i
                );
        }
        
        if( !( (ptctc->SIG1 == ptsig && ptctc->NODE1 == i ) ||
               (ptctc->SIG2 == ptsig && ptctc->NODE2 == i )   )  )
        {
          ctcok = 0 ;
          printf( "   [11] CTC entre %ld.%ld et %ld.%ld sur mauvais noeud %d.\n",
                  ptctc->SIG1->INDEX,
                  ptctc->NODE1,
                  ptctc->SIG2->INDEX,
                  ptctc->NODE2,
                  i
                );
        }
      }
    }

    /* Verification des wires */

    for( scanwire = ptsig->PRCN->PWIRE ; scanwire ; scanwire = scanwire->NEXT )
    {
      if( RCN_GETFLAG( scanwire->FLAG, RCN_FLAG_DEL ) )
        continue;
      ptnode = getlonode( ptsig, scanwire->NODE1 );

      trouve = 0;
      for( scanchain = ptnode->WIRELIST ;
           scanchain ;
           scanchain = scanchain->NEXT
         )
      {
        if( scanchain->DATA == scanwire )
        {
          if( trouve == 1 )
          {
            wireok = 0;
            printf( "   [12] WIRE entre %ld et %ld existe plusieurs fois sur le noeud %ld.\n",
                    scanwire->NODE1,
                    scanwire->NODE2,
                    ptnode->INDEX
                  );
          }
          trouve = 1;
        }
      }
      
      if( trouve == 0 )
      {
        wireok = 0;
        printf( "   [13] WIRE entre %ld et %ld n'existe pas sur le noeud %ld.\n",
                scanwire->NODE1,
                scanwire->NODE2,
                ptnode->INDEX
              );
      }
      
      ptnode = getlonode( ptsig, scanwire->NODE2 );

      trouve = 0;
      for( scanchain = ptnode->WIRELIST ;
           scanchain ;
           scanchain = scanchain->NEXT
         )
      {
        if( scanchain->DATA == scanwire )
        {
          if( trouve == 1 )
          {
            wireok = 0;
            printf( "   [14] WIRE entre %ld et %ld existe plusieurs fois sur le noeud %ld.\n",
                    scanwire->NODE1,
                    scanwire->NODE2,
                    ptnode->INDEX
                  );
          }
          trouve = 1;
        }
      }
      
      if( trouve == 0 )
      {
        wireok = 0;
        printf( "   [15] WIRE entre %ld et %ld n'existe pas sur le noeud %ld.\n",
                scanwire->NODE1,
                scanwire->NODE2,
                ptnode->INDEX
              );
      }
    }

    for( i=1 ; i < ptsig->PRCN->NBNODE ; i++ )
    {
      ptnode = getlonode( ptsig, i );
      if( !ptnode )
        continue;

      nbwire = 0;
      for( scanchain = ptnode->WIRELIST ;
           scanchain ;
           scanchain = scanchain->NEXT
         )
      {
        nbwire++;
        for( scanwire = ptsig->PRCN->PWIRE ;
             scanwire ;
             scanwire = scanwire->NEXT
           )
        {
           if( scanwire == scanchain->DATA )
             break;
        }
        if( !scanwire )
        {
           wireok = 0;
           printf( "   [16] Wire %16lX inexistant trouve sur le noeud %d.\n",
                   (long)scanchain->DATA,
                   i
                 );
        }
      }

      switch( nbwire )
      {
      case 0 :
        if( RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_ONE   ) ||
            RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_TWO   ) ||
            RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_CROSS )    )
        {
          printf( "   [17] Mauvais indicateur de nombre de wire sur le noeud %ld.\n",
                  ptnode->INDEX
                );
          wireok = 0;
        }
        break;
      case 1 :
        if( !RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_ONE   ) ||
             RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_TWO   ) ||
             RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_CROSS )    )
        {
          printf( "   [18] Mauvais indicateur de nombre de wire sur le noeud %ld.\n",
                  ptnode->INDEX
                );
          wireok = 0;
        }
        break;
      case 2 :
        if(  RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_ONE   ) ||
            !RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_TWO   ) ||
             RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_CROSS )    )
        {
          printf( "   [19] Mauvais indicateur de nombre de wire sur le noeud %ld.\n",
                  ptnode->INDEX
                );
          wireok = 0;
        }
        break;
      default : 
        if(  RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_ONE   ) ||
             RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_TWO   ) ||
            !RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_CROSS )    )
        {
          printf( "   [20] Mauvais indicateur de nombre de wire sur le noeud %ld.\n",
                  ptnode->INDEX
                );
          wireok = 0;
        }
      }
    }

    /* Verification connecteurs */

    ptptype = getptype( ptsig->USER, LOFIGCHAIN );
    if( !ptptype )
      printf( "  Pas de lofigchain -> connecteurs non testes.\n" );
    else
    {
      for( scanchain = (chain_list*)ptptype->DATA ;
           scanchain ;
           scanchain = scanchain->NEXT
         )
      {
        ptcon = (locon_list*)(scanchain->DATA) ; 
        for( scannum = ptcon->PNODE ; scannum ; scannum = scannum->NEXT )
        {
          ptnode = getlonode( ptsig, scannum->DATA );
          if( !RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_LOCON ) )
          {
            printf( "   [21] Le noeud %ld devrait porter l'indicateur RCN_FLAG_LOCON pour le connecteur %s.\n",
                    ptnode->INDEX,
                    ptcon->NAME
                  );
            conok = 0;
          }
          
          ptptype = getptype( ptnode->USER, RCN_LOCON );
          if( !ptptype )
            printf( "   [22] Pas de ptype RCN_LOCON sur le noeud %ld (locon %s).\n",
                    ptnode->INDEX,
                    ptcon->NAME
                  );
          else
          {
            trouve = 0;
            for( scanchcon = (chain_list*)ptptype->DATA ;
                 scanchcon ;
                 scanchcon = scanchcon->NEXT
               )
            {
              if( scanchcon->DATA == ptcon )
              {
                if( trouve == 1 )
                {
                  printf( "   [23] Locon %s defini plusieurs fois dans le node %ld.\n",
                          ptcon->NAME,
                          ptnode->INDEX
                        );
                  conok = 0;
                }
                trouve = 1;
              }
            }

            if( trouve == 0 )
            {
              printf( "   [24] Locon %s non defini dans le node %ld.\n",
                      ptcon->NAME,
                      ptnode->INDEX
                    );
              conok = 0;
            }
          }
        }
      }

      for( i = 0 ; i < ptsig->PRCN->NBNODE ; i++ )
      {
        ptnode  = getlonode( ptsig, i );
        if( !ptnode )
          continue;

        ptptype = getptype( ptnode->USER, RCN_LOCON );

        if( RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_LOCON ) && ptptype )
        {
          for( scanchcon = ptptype->DATA ;
               scanchcon ;
               scanchcon = scanchcon->NEXT )
          {
            ptcon = (locon_list*)(scanchcon->DATA);
            trouve = 0;
            for( scannum = ptcon->PNODE; scannum ; scannum = scannum->NEXT )
            {
              if( scannum->DATA == i )
              {
                if( trouve == 1 )
                {
                  printf( "   [25] Le noeud %d est defini plusieurs fois dans le locon %s.\n",
                          i,
                          ptcon->NAME
                        );
                  conok = 0;
                }
                trouve = 1;
              }
            }
            if( trouve == 0 )
            {
              printf( "   [26] Le noeud %d n'est pas defini dans le locon %s.\n",
                      i,
                      ptcon->NAME
                    );
              conok = 0;
            }
          }
        }

        if( RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_LOCON ) && ! ptptype )
        {
          printf( "   [27] Pas de champs ptype associe au RCN_FLAG_LOCON.\n" );
          conok = 0;
        }

        if( !RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_LOCON ) && ptptype )
        {
          printf( "   [28] Champs ptype non associe au RCN_FLAG_LOCON.\n" );
          conok = 0;
        }
      }
    }
  }
  
  if( wireok )
    printf( " - structures WIRE coherentes.\n" );
  else
    printf( " - structures WIRE incoherentes.\n" );

  if( ctcok )
    printf( " - structures CTC coherentes.\n" );
  else
    printf( " - structures CTC incoherentes.\n" );

  if( conok )
    printf( " - Indications CONNECTEUR coherentes.\n" );
  else
    printf( " - Indications CONNECTEUR incoherentes.\n" );

  if( !wireok || !ctcok || !conok )
    EXIT(1);
}

void rcn_displaytable( void )
{
  static int n=0;
  int i;
  printf("table %d\n", n );
  for( i=0; i<RCNCACHETABLE_SIZE ; i++ ) {
    printf( "  %d %08lX %2d %2d\n", i, (unsigned long) RCNCACHETABLE_LINE[i].LOSIG, RCNCACHETABLE_LINE[i].PREV, RCNCACHETABLE_LINE[i].NEXT );
  }
  printf( "RCNCACHETABLE_FREE  = %2d\n", RCNCACHETABLE_FREE );
  printf( "RCNCACHETABLE_FIRST = %2d\n", RCNCACHETABLE_FIRST );
  printf( "RCNCACHETABLE_LAST  = %2d\n", RCNCACHETABLE_LAST );
}

void rcn_addlosigincachetable( losig )
losig_list *losig;
{
  int         i;
  int         line;
  ptype_list *ptl;
  int         nextline;
  
  if( RCNCACHETABLE_LINE == NULL ) {
    // Premier appel : on construit le cache
    RCNCACHETABLE_LINE = (rcncacheline*) mbkalloc( sizeof( rcncacheline ) * 
                                                   RCNCACHETABLE_SIZE        );
    RCNCACHETABLE_FREE = 0;                                                    
    for( i=0 ; i<RCNCACHETABLE_SIZE ; i++ ) {
      RCNCACHETABLE_LINE[i].LOSIG = NULL;
      RCNCACHETABLE_LINE[i].PREV  = -1;
      RCNCACHETABLE_LINE[i].NEXT  = i+1;
    }
    RCNCACHETABLE_LINE[RCNCACHETABLE_SIZE-1].NEXT = -1;
  }

  ptl = getptype( losig->USER, RCN_CACHELINE );
  
  if( ptl ) {

    // C'est un rajeunissement de la ligne de cache

    line = (int)((long)ptl->DATA);

    if( line == RCNCACHETABLE_FIRST ) return;

    rcn_removelinefromcachetable( line );
    
    RCNCACHETABLE_LINE[ line ].PREV  = -1 ;
    RCNCACHETABLE_LINE[ line ].NEXT  = RCNCACHETABLE_FIRST ;
    RCNCACHETABLE_LINE[ line ].LOSIG = losig ;

    if( RCNCACHETABLE_FIRST != -1 )
      RCNCACHETABLE_LINE[ RCNCACHETABLE_FIRST ].PREV = line ;

  }
  
  else {

    if( RCNCACHETABLE_FREE == -1 ) {
      line = RCNCACHETABLE_LAST;
      freetable( RCNCACHETABLE_LINE[line].LOSIG );
    }

    line = RCNCACHETABLE_FREE;
    RCNCACHETABLE_FREE = RCNCACHETABLE_LINE[ RCNCACHETABLE_FREE ].NEXT;

    RCNCACHETABLE_LINE[ line ].PREV  = -1 ;
    RCNCACHETABLE_LINE[ line ].NEXT  = RCNCACHETABLE_FIRST ;
    RCNCACHETABLE_LINE[ line ].LOSIG = losig ;

    if( RCNCACHETABLE_LINE[ line ].NEXT != -1 ) {
      nextline = RCNCACHETABLE_LINE[ line ].NEXT ;
      RCNCACHETABLE_LINE[ nextline ].PREV = line ;
    }

    losig->USER = addptype( losig->USER, RCN_CACHELINE, (void*)((long)line) );
  }

  RCNCACHETABLE_FIRST = line;
  if( RCNCACHETABLE_LAST == -1 )
    RCNCACHETABLE_LAST = line;
}

void rcn_removelinefromcachetable( line )
int line;
{
  int prevline;
  int nextline;
  
  prevline = RCNCACHETABLE_LINE[ line ].PREV ;
  nextline = RCNCACHETABLE_LINE[ line ].NEXT ;

  if( nextline != -1 ) RCNCACHETABLE_LINE[ nextline ].PREV = prevline ;
  if( prevline != -1 ) RCNCACHETABLE_LINE[ prevline ].NEXT = nextline ;
 
  if( line == RCNCACHETABLE_FIRST )
    RCNCACHETABLE_FIRST = nextline ;
  if( line == RCNCACHETABLE_LAST )
    RCNCACHETABLE_LAST = prevline ;

  RCNCACHETABLE_LINE[ line ].NEXT = RCNCACHETABLE_FREE;
  RCNCACHETABLE_FREE = line ;
  RCNCACHETABLE_LINE[ line ].PREV = -1;
  RCNCACHETABLE_LINE[ line ].LOSIG = NULL;
}

void rcn_removelosigfromcachetable( losig )
losig_list      *losig;
{
  ptype_list *ptl;
  int         line;

  ptl = getptype( losig->USER, RCN_CACHELINE );
  if( !ptl ) return;

  line = (int)((long)ptl->DATA);

  rcn_removelinefromcachetable( line );

  losig->USER = delptype( losig->USER, RCN_CACHELINE );
}

/* Regroupe les CTC en double entre deux noeuds sur un signal. Renvoie le
nombre de ctc effacées. */

int rcn_mergectclosig( losig_list *losig )
{
  ht *htnodeviclist;
  ht *htsigagrlist;
  ht *htnodeagrlist;
  long nodevic;
  long nodeagr;
  losig_list *sigagr;
  int n=0;
  chain_list *headht;
  chain_list *scanht;
  chain_list *scanctc;
  chain_list *nextctc;
  loctc_list *loctc;
  loctc_list *existctc;
  
  if( !losig->PRCN || !losig->PRCN->PCTC || !losig->PRCN->PCTC->NEXT )
    return 0;

  if( gettabnode( losig->PRCN ) )
    freetable( losig );

  htnodeviclist = addht(10);
  headht = addchain( NULL, htnodeviclist );

  for( scanctc = losig->PRCN->PCTC ; scanctc ; scanctc = nextctc ) {
    nextctc = scanctc->NEXT;
    
    loctc = (loctc_list*)scanctc->DATA;

    if( loctc->USER )
      continue;
 
    nodevic = rcn_ctcnode( loctc, losig );
    nodeagr = rcn_ctcothernode( loctc, losig );
    sigagr  = rcn_ctcothersig( loctc, losig );

    htsigagrlist = (ht*)gethtitem( htnodeviclist, (void*)nodevic );
    
    if( (long)htsigagrlist == EMPTYHT ) {
      htsigagrlist = addht(5);
      addhtitem( htnodeviclist, (void*)nodevic, (long)htsigagrlist );
      headht = addchain( headht, htsigagrlist );
    }

    htnodeagrlist = (ht*)gethtitem( htsigagrlist, (void*) sigagr );

    if( (long)htnodeagrlist == EMPTYHT ) {
      htnodeagrlist = addht(5);
      addhtitem( htsigagrlist, (void*)sigagr, (long)htnodeagrlist );
      headht = addchain( headht, htnodeagrlist );
    }

    existctc = (loctc_list*)gethtitem( htnodeagrlist, (void*) nodeagr );
    if( (long) existctc == EMPTYHT ) 
      addhtitem( htnodeagrlist, (void*)nodeagr, (long) loctc );
    else {
      // On a une ctc en double
      existctc->CAPA = existctc->CAPA + loctc->CAPA;
      delloctc( loctc ); loctc=NULL;
      n++;
    }
  }

  for( scanht = headht ; scanht ; scanht = scanht->NEXT )
    delht( (ht*)scanht->DATA );
  freechain( headht );
  return n;
}

/* Regroupe les CTC en double entre deux noeuds sur la figure. Renvoie le
nombre de ctc effacées. */

int rcn_mergectclofig( lofig_list *lofig )
{
  losig_list    *losig;
  int            nbremoved = 0;
  
  for( losig = lofig->LOSIG ; losig ; losig = losig->NEXT ) {
    nbremoved+=rcn_mergectclosig( losig );
  }

  return nbremoved;
}

void delloctc( loctc_list *ptctc )
{
  chain_list    *prevchainctc;
  chain_list    *currchainctc;
  chain_list    *nextchainctc;
  loctc_list    *nextctc;
  chain_list    *prev1;
  chain_list    *prev2;
  lonode_list   *ptnode;
  chain_list    *scanchain;

  // Efface la ctc des listes chainées.
 
  freeloctcparams (ptctc);

  if( ptctc->PREV1 ) {
    prevchainctc = ptctc->PREV1;
    currchainctc = prevchainctc->NEXT;
    nextchainctc = currchainctc->NEXT;

    prevchainctc->NEXT = nextchainctc;
    currchainctc->NEXT = NULL;
    freechain( currchainctc ) ; currchainctc=NULL;

    if( nextchainctc ) {
      nextctc = (loctc_list*) nextchainctc->DATA;
      if( ptctc->SIG1 == nextctc->SIG1 )
        nextctc->PREV1 = prevchainctc;
      else
        nextctc->PREV2 = prevchainctc;
    }
  }
  else {
    currchainctc = ptctc->SIG1->PRCN->PCTC;
    nextchainctc = currchainctc->NEXT;

    ptctc->SIG1->PRCN->PCTC = nextchainctc;
    currchainctc->NEXT = NULL;
    freechain( currchainctc ) ; currchainctc = NULL;

    if( nextchainctc ) {
      nextctc = (loctc_list*) nextchainctc->DATA;
      if( ptctc->SIG1 == nextctc->SIG1 )
        nextctc->PREV1 = NULL;
      else
        nextctc->PREV2 = NULL;
    }
  }

  if( ptctc->PREV2 ) {
    prevchainctc = ptctc->PREV2;
    currchainctc = prevchainctc->NEXT;
    nextchainctc = currchainctc->NEXT;

    prevchainctc->NEXT = nextchainctc;
    currchainctc->NEXT = NULL;
    freechain( currchainctc ) ; currchainctc=NULL;

    if( nextchainctc ) {
      nextctc = (loctc_list*) nextchainctc->DATA;
      if( ptctc->SIG2 == nextctc->SIG2 )
        nextctc->PREV2 = prevchainctc;
      else
        nextctc->PREV1 = prevchainctc;
    }
  }
  else {
    currchainctc = ptctc->SIG2->PRCN->PCTC;
    nextchainctc = currchainctc->NEXT;

    ptctc->SIG2->PRCN->PCTC = nextchainctc;
    currchainctc->NEXT = NULL;
    freechain( currchainctc ) ; currchainctc = NULL;

    if( nextchainctc ) {
      nextctc = (loctc_list*) nextchainctc->DATA;
      if( ptctc->SIG2 == nextctc->SIG2 )
        nextctc->PREV2 = NULL;
      else
        nextctc->PREV1 = NULL;
    }
  }
  
  /* cleanning element in table */

  if( gettabnode( ptctc->SIG1->PRCN ) && ptctc->NODE1 )
  {
    ptnode = getlonode( ptctc->SIG1, ptctc->NODE1 );
    prev1 = NULL;

    for( scanchain = ptnode->CTCLIST ; scanchain ; scanchain = scanchain->NEXT )
    {
      if( scanchain->DATA == ptctc )
        break;
      prev1 = scanchain;
    }

    if( prev1 )
      prev1->NEXT = scanchain->NEXT;
    else
      ptnode->CTCLIST = scanchain->NEXT;

    scanchain->NEXT = NULL;
    freechain( scanchain );
  }

  if( gettabnode( ptctc->SIG2->PRCN ) && ptctc->NODE2 )
  {
    ptnode = getlonode( ptctc->SIG2, ptctc->NODE2 );
    prev2 = NULL;

    for( scanchain = ptnode->CTCLIST ; scanchain ; scanchain = scanchain->NEXT )
    {
      if( scanchain->DATA == ptctc )
        break;
      prev2 = scanchain;
    }

    if( prev2 )
      prev2->NEXT = scanchain->NEXT;
    else
      ptnode->CTCLIST = scanchain->NEXT;

    scanchain->NEXT = NULL;
    freechain( scanchain );
  }

  /* Really clean the loctc */

  freeloctc( ptctc );
  
}

/******************************************************************************\
Renvoie la somme des capacités de couplage entre deux signaux.
\******************************************************************************/

float rcn_getcouplingcapacitance( lofig_list *lofig,
                                  losig_list *sig1,
                                  losig_list *sig2
                                )
{
  float          capa = 0.0 ;
  float          capa1 ;
  float          capa2 ;
  chain_list    *chain ;
  chain_list    *head ;
  loctc_list    *ctc ;
  losig_list    *siglocked ;
  
  if( !sig1->PRCN || !sig2->PRCN )
    return 0.0 ;

  if( mbk_LosigIsVDD(sig1) || mbk_LosigIsVSS(sig1) ||
      mbk_LosigIsVDD(sig2) || mbk_LosigIsVSS(sig2) )
    return 0.0 ;
      
  capa1 = rcn_getcapa( lofig, sig1 ) ;
  capa2 = rcn_getcapa( lofig, sig2 ) ;

  if( capa1 < capa2 ) {
    siglocked = sig1 ;
    rcn_lock_signal( lofig, siglocked );
    head = sig1->PRCN->PCTC ;
  }
  else {
    siglocked = sig2 ;
    rcn_lock_signal( lofig, siglocked );
    head = sig2->PRCN->PCTC ;
  }

  for( chain = head ; chain ; chain = chain->NEXT ) {
    ctc = (loctc_list*)chain->DATA ;
    if( ( ctc->SIG1 == sig1 && ctc->SIG2 == sig2 ) ||
        ( ctc->SIG2 == sig1 && ctc->SIG1 == sig2 )    )
      capa = capa + ctc->CAPA ;
  }

  rcn_unlock_signal( lofig, siglocked );
 
  return capa ;
}

/******************************************************************************\
erreur rcn
\******************************************************************************/

void rcn_error( int code, int type )
{
  avt_errmsg( RCN_ERRMSG, "000", type, code );
}

void rcn_add_low_capa_ifneeded(losig_list *ls, losig_list *gnd)
{
  lowire_list *lw;
  long l;
  mbk_tableint *mti;
  chain_list *cl;
  num_list *nl;
  ptype_list *pt;

  if (ls->PRCN!=NULL && ls->PRCN->PWIRE!=NULL)
  {
    mti=creatmbk_tableint();

    if ((pt=getptype(ls->USER, LOFIGCHAIN))!=NULL)
      {
        for (cl=(chain_list *)pt->DATA; cl!=NULL; cl=cl->NEXT)
          {
            for (nl=((locon_list *)cl->DATA)->PNODE; nl!=NULL; nl=nl->NEXT)
              {
                if ((l=tstmbk_tableint(mti, nl->DATA))==-1)
                  setmbk_tableint(mti, nl->DATA, 1);
                else
                  setmbk_tableint(mti, nl->DATA, l+1);
              }
          }
      }

    for (lw=ls->PRCN->PWIRE; lw!=NULL; lw=lw->NEXT)
    {
      if ((l=tstmbk_tableint(mti, lw->NODE1))==-1)
        setmbk_tableint(mti, lw->NODE1, 1);
      else
        setmbk_tableint(mti, lw->NODE1, l+1);

      if ((l=tstmbk_tableint(mti, lw->NODE2))==-1)
        setmbk_tableint(mti, lw->NODE2, 1);
      else
        setmbk_tableint(mti, lw->NODE2, l+1);
     }

    for (lw=ls->PRCN->PWIRE; lw!=NULL; lw=lw->NEXT)
    {
      if ((l=tstmbk_tableint(mti, lw->NODE1))==1)
        addloctc(ls, lw->NODE1, gnd, 0, 1e-5);
      if ((l=tstmbk_tableint(mti, lw->NODE2))==1)
        addloctc(ls, lw->NODE2, gnd, 0, 1e-5);
    }

    freembk_tableint(mti);
  }
}

int rcn_isCapaDiode (loctc_list *ctc)
{
  int res=0;
  ptype_list *pt;
  if ( (pt=getptype(ctc->USER,CAPA_IS_DIODE))!=NULL)
    return (int)(long)pt->DATA;
  return res;
}

void rcn_unsetCapaDiode( loctc_list *ctc ) 
{
  if( getptype( ctc->USER, CAPA_IS_DIODE ) )
    ctc->USER = delptype( ctc->USER, CAPA_IS_DIODE );
}

void rcn_setCapaDiode( loctc_list *ctc, int cnt )
{
  ptype_list *pt;
  if( (pt=getptype( ctc->USER, CAPA_IS_DIODE ))==NULL )
    ctc->USER = addptype( ctc->USER, CAPA_IS_DIODE, (void *)(long)cnt);
  else 
    pt->DATA=(void *)(((long)pt->DATA) + cnt);
}

int rcn_capacitancetooutput(int cachemode, losig_list *ptlosig, loctc_list *ptctc)
{
  int alim1, alim2;
  alim1=mbk_LosigIsVSS(ptctc->SIG1);
  alim2=mbk_LosigIsVSS(ptctc->SIG2);
  if ((ptlosig == ptctc->SIG1 && ((!cachemode || !alim2) && ptctc->SIG1 <= ptctc->SIG2))
      || (ptlosig == ptctc->SIG2 && ((!cachemode || !alim1) && ptctc->SIG1 >= ptctc->SIG2)))
     return 1;
  return 0;
}

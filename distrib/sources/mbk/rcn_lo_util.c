
#include "rcn.h"
#include "mlu_lib.h"
#include MSL_H

/******************************************************************************/

void		duplorcnet( ptsig_dest, ptsig_src )
losig_list	*ptsig_dest;
losig_list	*ptsig_src;
{
  lowire_list	*scanlowire;
  lowire_list	*newwire;
  ptype_list    *ptuser;

  if( ptsig_dest->PRCN ) {
    rcn_error( 27, AVT_WARNING );
    return ;
  }
  
  if( !ptsig_src->PRCN )
    return;

  addlorcnet( ptsig_dest );
  
  /* Capacité du signal */
  rcn_setcapa( ptsig_dest, rcn_getcapa( NULL, ptsig_src ) );
  
  /* Wire constituant le signal */
  for( scanlowire = ptsig_src->PRCN->PWIRE ;
       scanlowire ;
       scanlowire = scanlowire->NEXT
     )
  {

    if( RCN_GETFLAG( scanlowire->FLAG, RCN_FLAG_DEL ) )
      continue;
    newwire = heaplowire();
    
    newwire->NEXT  = ptsig_dest->PRCN->PWIRE;
    newwire->RESI  = scanlowire->RESI;
    newwire->CAPA  = scanlowire->CAPA;
    newwire->NODE1 = scanlowire->NODE1;
    newwire->NODE2 = scanlowire->NODE2;
    newwire->FLAG  = scanlowire->FLAG;
    newwire->USER  = NULL;
      
    if ((ptuser = getptype(scanlowire->USER, RESINAME))!=NULL)
       newwire->USER = addptype(newwire->USER, RESINAME, ptuser->DATA);

    if ((ptuser = getptype(scanlowire->USER, OPT_PARAMS)))
        newwire->USER = addptype(newwire->USER, OPT_PARAMS, dupoptparamlst((optparam_list *)ptuser->DATA));

    ptsig_dest->PRCN->PWIRE = newwire;
  }

  /* CTC sur le signal : non gere */
  ptsig_dest->PRCN->PCTC = NULL;

  /* NBNODE */
  ptsig_dest->PRCN->NBNODE = ptsig_src->PRCN->NBNODE ;

  /* USER */
  ptsig_dest->PRCN->USER = NULL;
}

/******************************************************************************/

todolist*        breakloop2( ptsig, ptnode, curwire, headtodo, cleanwire )
losig_list      *ptsig;
lonode_list     *ptnode;
lowire_list     *curwire;
todolist        *headtodo;
chain_list      **cleanwire;
{
  lowire_list   *tmpwire;
  chain_list    *scanchain;
  todolist      *newtodo;

  if( curwire )
  {
    if( RCN_GETFLAG( curwire->FLAG, RCN_FLAG_PASS ) ) {
      rcn_error( 28, AVT_WARNING );
      return NULL ;
    }

    RCN_SETFLAG( curwire->FLAG, RCN_FLAG_PASS );
  }
  
  do
  {
    /* On trouve une boucle */
    if( RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_PASS ) )
    {
      tmpwire = addlowire( ptsig,
                           curwire->FLAG,
                           curwire->RESI,
                           curwire->CAPA,
                           curwire->NODE1,
                           addlonode( ptsig, NULL )
                         );
      *cleanwire = addchain( *cleanwire, curwire );
      RCN_SETFLAG( tmpwire->FLAG, RCN_FLAG_PASS );
      RCN_SETFLAG( curwire->FLAG, RCN_FLAG_PASS );
    }
    else
    {
      RCN_SETFLAG( ptnode->FLAG, RCN_FLAG_PASS );
    }
   
    /* Recherche un wire non parcouru sur le noeud courant */
    for( scanchain = ptnode->WIRELIST ;
         scanchain ;
         scanchain = scanchain->NEXT
       )
    {
      curwire = (lowire_list*)(scanchain->DATA);
      if( ! RCN_GETFLAG( curwire->FLAG, RCN_FLAG_PASS ) )
        break;
    }

    if( ! scanchain )
      break; /* on quitte le while */
    
    /* Ajoute les autre wire non parcouru sur le noeud dans une
     * structure todo */
    
    newtodo = NULL;

    if( scanchain )
    {
      for( scanchain = scanchain->NEXT ;
           scanchain ;
           scanchain = scanchain->NEXT
         )
      {
        tmpwire = (lowire_list*)(scanchain->DATA);
  
        if( ! RCN_GETFLAG( tmpwire->FLAG, RCN_FLAG_PASS ) )
        {
          if( !newtodo )
          {
            newtodo            = (todolist*)(mbkalloc( sizeof(todolist) ) );
            newtodo->next      = headtodo ;
            headtodo           = newtodo;
            newtodo->ptnode    = ptnode;
            newtodo->head_wire = NULL;
          }
          newtodo->head_wire = addchain( newtodo->head_wire, tmpwire );
        }
      }
    }

    ptnode = getlonode( NULL,
                        ( curwire->NODE1 == ptnode->INDEX ) ? curwire->NODE2 :
                                                              curwire->NODE1
                      );
    
    RCN_SETFLAG( curwire->FLAG, RCN_FLAG_PASS );
  }
  while(1); /* c'est ben vrai! */

  return( headtodo );
}

int              breakloop( ptsig )
losig_list      *ptsig;
{
  
  todolist              *headtodo ;  
  todolist              *curtodo ;  
  todolist              *scantodo ;  
  todolist              *newtodo ;  
  lowire_list           *firstwire;
  lowire_list           *curwire;
  lonode_list           *ptnode;
  chain_list            *cleanwire;
  chain_list            *scanchain;
  
  if( !ptsig->PRCN || !gettabnode( ptsig->PRCN ) ) {
    return 0 ;
    rcn_error( 29, AVT_WARNING );
  }
  
  clearallwireflag( ptsig, RCN_FLAG_PASS );
  cleanwire = NULL;

  getlonode(ptsig,0) ;

  /* Boucle pour parcourir les differentes composantes connexes */

  for( firstwire  = ptsig->PRCN->PWIRE ;
       firstwire ;
       firstwire = firstwire->NEXT
     )
  {
    if( RCN_GETFLAG( firstwire->FLAG, RCN_FLAG_DEL ) )
      continue;

    ptnode = getlonode( NULL, firstwire->NODE1 );
    
    if( ! RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_PASS ) )
    {
      /* Corps d'une des composante connexe du reseau */
     
      headtodo = breakloop2( ptsig, ptnode, NULL, NULL, &cleanwire );

      while( headtodo )
      {
        newtodo = NULL;
        
        for( scantodo = headtodo ; scantodo ; scantodo = scantodo->next )
        {
          for( scanchain = scantodo->head_wire ;
               scanchain ;
               scanchain = scanchain->NEXT
             )
          {
          
            curwire = (lowire_list*)( scanchain->DATA );
          
            if( ! RCN_GETFLAG( curwire->FLAG, RCN_FLAG_PASS ) )
            {
               
              newtodo = breakloop2(  ptsig,
                                     getlonode( NULL,
                                                ( scantodo->ptnode->INDEX ==
                                                              curwire->NODE1 ) ?
                                                                curwire->NODE2 :
                                                                curwire->NODE1
                                             ),
                                      curwire,
                                      newtodo,
                                     &cleanwire
                                   );
            }
          }
        }
        
        while( headtodo )
        {
          curtodo = headtodo->next;
          freechain( headtodo->head_wire );
          mbkfree( headtodo );
          headtodo = curtodo;
        }

        headtodo = newtodo;
      }
    }
  }

  for( scanchain = cleanwire ; scanchain ; scanchain = scanchain->NEXT )
  {
    curwire = (lowire_list*)( scanchain->DATA );
    dellowire( ptsig, curwire->NODE1, curwire->NODE2 );    
  }

  freechain ( cleanwire );

  return(1);
}

int reduce_rcn( ptsig )
losig_list *ptsig;
{
  lowire_list *scanwire;
  lonode_list *ptnode;
  chain_list  *todo;
  chain_list  *doing;
  chain_list  *scanctc;
  loctc_list  *ptctc;
  float        resi;
  int          node1;
  int          node2;
  lowire_list *wire1;
  lowire_list *wire2;
  int          another;

  rcn_checkwritesignal(ptsig);

  if( !gettabnode( ptsig->PRCN ) ) {
    rcn_error( 30, AVT_WARNING );
    return 0 ;
  }

  do
  {
    another = 0;

    todo = NULL;
    rcn_setcapa( ptsig, 0.0 );
  
    for( scanwire = ptsig->PRCN->PWIRE ; scanwire ; scanwire = scanwire->NEXT )
    {
      if( RCN_GETFLAG( scanwire->FLAG, RCN_FLAG_DEL ) )
        continue;
 
      rcn_addcapa( ptsig, scanwire->CAPA );
      scanwire->CAPA = 0.0;
      ptnode = getlonode( ptsig, scanwire->NODE1 );
      if(   RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_TWO )   &&
          ! RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_LOCON ) &&
          ! RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_PASS )
        )
      {
        todo = addchain( todo, ptnode );
        RCN_SETFLAG( ptnode->FLAG, RCN_FLAG_PASS );
      }
      ptnode = getlonode( ptsig, scanwire->NODE2 );
      if(   RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_TWO )   &&
          ! RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_LOCON ) &&
          ! RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_PASS )
        )
      {
        todo = addchain( todo, ptnode );
        RCN_SETFLAG( ptnode->FLAG, RCN_FLAG_PASS );
      }
    }
  
    for( scanctc = ptsig->PRCN->PCTC ; scanctc ; scanctc = scanctc->NEXT )
    {
      ptctc = (loctc_list*)( scanctc->DATA );
      rcn_addcapa( ptsig, ptctc->CAPA );
    }
  
    for( doing = todo ; doing ; doing = doing->NEXT )
    {
      ptnode = (lonode_list*)( doing->DATA );
      if( ptnode->CTCLIST )
        continue;
 
      another = 1; 
      wire1 = (lowire_list*)( ptnode->WIRELIST->DATA );
      wire2 = (lowire_list*)( ptnode->WIRELIST->NEXT->DATA );
  
      if( ptnode->WIRELIST->NEXT->NEXT )
      {
        fprintf( stderr, "Anomalie.\n" );
      }
  
      resi = wire1->RESI + wire2->RESI ;
      node1 = ( wire1->NODE1 == ptnode->INDEX ) ? wire1->NODE2 : wire1->NODE1 ;
      node2 = ( wire2->NODE1 == ptnode->INDEX ) ? wire2->NODE2 : wire2->NODE1 ;
     
      setdellowire( ptsig, wire1 ); 
      setdellowire( ptsig, wire2 ); 
      wire1 = NULL;
      wire2 = NULL;
      
      scanwire = getlowire( ptsig, node1, node2 );
      if( scanwire )
        addpararesiwire( scanwire, resi );
      else
        addlowire( ptsig,
                   0,
                   resi,
                   0.0,
                   node1,
                   node2
                 );
    }
    freechain( todo );
  
    mergedellowire( ptsig );
  }
  while( another );

  return(1);
}

void connexe_rec( losig_list *ptsig,
                  lowire_list *from,
                  lonode_list *ptnode,
                  int index
                )
{
  ptype_list  *ptl;
  chain_list  *scanwire;
  lonode_list *next;

  /* Le noeud par lequel on arrive est deja numerote */

  /* Propagation sur les branches droites */
  while( RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_TWO ) )
  {
    if( (lowire_list*)(ptnode->WIRELIST->DATA) == from )
      from = (lowire_list*)(ptnode->WIRELIST->NEXT->DATA);
    else
      from = (lowire_list*)(ptnode->WIRELIST->DATA);
    
    if( from->NODE1 == ptnode->INDEX )
      ptnode = getlonode( ptsig, from->NODE2 );
    else
      ptnode = getlonode( ptsig, from->NODE1 );

    ptl = getptype( ptnode->USER, CONNEXE );
    if( (int)((long)ptl->DATA) != 0 )
    {
      if( (int)((long)ptl->DATA) != index ) 
        rcn_error( 31, AVT_WARNING);
      return;
    }
    else
      ptl->DATA = (void*)((long)index);
  }

  /* On arrive soit sur un noeud terminal, soit sur un embranchement */
  for( scanwire = ptnode->WIRELIST ; scanwire ; scanwire = scanwire->NEXT )
  {
    from = ( lowire_list* )( scanwire->DATA );
    if( from->NODE1 == ptnode->INDEX )
      next = getlonode( ptsig, from->NODE2 );
    else
      next = getlonode( ptsig, from->NODE1 );

    ptl = getptype( next->USER, CONNEXE );
    if( (int)((long)ptl->DATA) == 0 )
    {
      ptl->DATA = (void*)((long)index);
      connexe_rec( ptsig, from, next, index ); 
    }
    else
    {
      if( (int)((long)ptl->DATA) != index ) {
        rcn_error( 32, AVT_WARNING );
        return ;
      }
    }
  }

  return;
}

int connexe( losig_list *ptsig )
{
  lowire_list     *scanlowire;
  ptype_list      *ptl;
  lonode_list     *ptnode;
  int              index;

  if( !gettabnode( ptsig->PRCN ) ) {
    rcn_error( 33, AVT_WARNING );
    return 0 ;
  }

  for( scanlowire = ptsig->PRCN->PWIRE ;
       scanlowire ;
       scanlowire = scanlowire->NEXT
     )
  {
    ptnode = getlonode( ptsig, scanlowire->NODE1 );
    ptl = getptype( ptnode->USER, CONNEXE );
    if( !ptl )
      ptnode->USER = addptype( ptnode->USER, CONNEXE, (void*)0 );
    else
      ptl->DATA = (void*)0;

    ptnode = getlonode( ptsig, scanlowire->NODE2 );
    ptl = getptype( ptnode->USER, CONNEXE );
    if( !ptl )
      ptnode->USER = addptype( ptnode->USER, CONNEXE, (void*)0 );
    else
      ptl->DATA = (void*)0;
  }

  index = 0;

  for( scanlowire = ptsig->PRCN->PWIRE ;
       scanlowire ;
       scanlowire = scanlowire->NEXT
     )
  {
    ptnode = getlonode( ptsig, scanlowire->NODE1 );
    ptl = getptype( ptnode->USER, CONNEXE );
    
    if( (int)((long)ptl->DATA) != 0 )
      continue;
 
    index++;

    ptl->DATA = (void*)((long)index);
    connexe_rec( ptsig, scanlowire, ptnode, index );

    ptnode = getlonode( ptsig, scanlowire->NODE2 );
    ptl = getptype( ptnode->USER, CONNEXE );
    ptl->DATA = (void*)((long)index);
    connexe_rec( ptsig, scanlowire, ptnode, index );

  }

  return( index );
}

void clean_connexe( ptsig )
losig_list *ptsig ;
{
  lowire_list *wire ;
  lonode_list *ptnode ;
  for( wire = ptsig->PRCN->PWIRE ; wire ; wire = wire->NEXT ) {
    ptnode = getlonode( ptsig, wire->NODE1 );
    ptnode->USER = testanddelptype( ptnode->USER, CONNEXE );
    ptnode = getlonode( ptsig, wire->NODE2 );
    ptnode->USER = testanddelptype( ptnode->USER, CONNEXE );
  }
}

chain_list*     getway( ptsig, org, dest )
losig_list      *ptsig;
long             org;
long             dest;
{
  lonode_list *ptnode;

  clearallwireflag( ptsig, RCN_FLAG_PASS );
  ptnode = getlonode( ptsig, org );
  if( rcn_testandmarknode( ptnode ) ) 
    return NULL;
  return getwayrec( ptsig, org, dest, NULL );
}

chain_list*     getwayrec( ptsig, org, dest, prec )
losig_list      *ptsig;
long             org;
long             dest;
lowire_list     *prec;
{
  chain_list    *scanchain;
  lowire_list   *current_wire;
  long           current_node;
  lonode_list   *ptnode;
  chain_list    *chemin;
  int            loop;

  ptnode = getlonode( ptsig, org );

  for( scanchain = ptnode->WIRELIST ; scanchain ; scanchain = scanchain->NEXT )
  {
    current_wire = (lowire_list*)(scanchain->DATA);
    
    if( current_wire != prec )
    {
      current_node = current_wire->NODE1 == org ? current_wire->NODE2 :
                                                  current_wire->NODE1   ;
      ptnode = getlonode( NULL, current_node );
        if( rcn_testandmarknode( ptnode ) ) continue;

      loop = 0;
      while( current_node != dest && RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_TWO ) && loop==0 )
      {
        current_wire = ( ptnode->WIRELIST->DATA == current_wire ) ?
                       ptnode->WIRELIST->NEXT->DATA :
                       ptnode->WIRELIST->DATA ;
        
        current_node = ( current_wire->NODE1 == current_node ) ?
                       current_wire->NODE2 :
                       current_wire->NODE1   ;
        
        ptnode       = getlonode( NULL, current_node );
        loop = rcn_testandmarknode( ptnode ) ;
      }

      if( current_node == dest && loop==0)
      {
        current_node = ( current_wire->NODE1 == current_node ) ?
                       current_wire->NODE2 :
                       current_wire->NODE1   ;
        
        ptnode = getlonode( NULL, current_node );
        chemin = addchain( NULL, current_wire );

        while( current_node != org )
        {
          current_wire = ( ptnode->WIRELIST->DATA == current_wire ) ?
                         ptnode->WIRELIST->NEXT->DATA :
                         ptnode->WIRELIST->DATA ;
          
          current_node = ( current_wire->NODE1 == current_node ) ?
                         current_wire->NODE2 :
                         current_wire->NODE1 ;
          
          ptnode = getlonode( NULL, current_node );
          chemin = addchain( chemin, current_wire );
        }
        return( chemin );
      }

      if( RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_CROSS ) && loop == 0 )
      {
        if( (chemin = getwayrec( ptsig, current_node, dest, current_wire )) )
        {
          current_node = ( current_wire->NODE1 == current_node ) ?
                         current_wire->NODE2 :
                         current_wire->NODE1 ;
          
          ptnode = getlonode( NULL, current_node );
          chemin = addchain( chemin, current_wire );

          while( current_node != org )
          {
            current_wire = ( ptnode->WIRELIST->DATA == current_wire ) ?
                           ptnode->WIRELIST->NEXT->DATA :
                           ptnode->WIRELIST->DATA ;
            
            current_node = ( current_wire->NODE1 == current_node ) ?
                           current_wire->NODE2 :
                           current_wire->NODE1 ;
            
            ptnode = getlonode( NULL, current_node );
            chemin = addchain( chemin, current_wire );
          }
          return( chemin );
        }
      }
    }
  }
  return( NULL );
}
static int rcn_get_a_r_way_sub(losig_list *ptsig, long org, long dest, chain_list **res)
{
  chain_list    *scanchain;
  lowire_list   *current_wire;
  long           current_node;
  lonode_list   *ptnode;
  chain_list    *chemin;
  int            loop;

  if (org==dest) { *res=NULL; return 1; }
  ptnode = getlonode( ptsig, org );  
  if( rcn_testandmarknode( ptnode ) ) return 0;

  for( scanchain = ptnode->WIRELIST ; scanchain ; scanchain = scanchain->NEXT )
  {
    current_wire = (lowire_list*)(scanchain->DATA);
    current_node = current_wire->NODE1 == org ? current_wire->NODE2 :
                                                  current_wire->NODE1   ;
    if (rcn_get_a_r_way_sub(ptsig, current_node, dest, res))
    {
       *res=addchain(*res, current_wire);
       return 1;
    }
  }
  return 0;
}
chain_list *rcn_get_a_r_way(losig_list *ptsig, long org, long dest)
{
  chain_list *res;
  clearallwireflag( ptsig, RCN_FLAG_PASS );
  if (rcn_get_a_r_way_sub(ptsig, org, dest, &res)) return res;
  return NULL;
}

void rcn_dumprcn( losig_list *losig, 
                  long start, 
                  char *fname,       /* nom du fichier ou null pour stdout */
                  lowire_list *wire, /* a mettre à NULL */
                  FILE *file         /* a mettre à NULL */
                )
{
  lonode_list   *curnode;
  chain_list    *scan;
  int            encore;
  loctc_list    *ctc;
  char           first=0;

  if( !file ) {
    first=1;
    for( wire = losig->PRCN->PWIRE ; wire ; wire = wire->NEXT )
      RCN_CLEARFLAG( wire->FLAG, RCN_FLAG_PASS );
    if( fname )
      file = mbkfopen( fname, NULL, "w" );
    else
      file = stdout;
  }

  if( wire==NULL ) {
    fprintf( file, "*** RCN DUMP FOR SIGNAL %s ***\n", getsigname( losig ) );
    for( scan = losig->PRCN->PCTC ; scan ; scan=scan->NEXT ) {
      ctc = (loctc_list*)scan->DATA;
      fprintf( file,
              "C %5ld %3ld - %5ld %3ld : %e\n",
              ctc->SIG1->INDEX,
              ctc->NODE1,
              ctc->SIG2->INDEX,
              ctc->NODE2,
              ctc->CAPA
            );
    }
  }

  curnode = getlonode( losig, start );

  if( wire ) {
    
    do {
      if( RCN_GETFLAG( wire->FLAG, RCN_FLAG_PASS ) ) {
        fprintf( file, "\n" );
        return;
      }
      RCN_SETFLAG( wire->FLAG, RCN_FLAG_PASS );
  
      if( curnode->INDEX == wire->NODE1 )
        fprintf( file, "W %3ld %3ld %e\n", wire->NODE1, wire->NODE2, wire->RESI );
      else
        fprintf( file, "W %3ld %3ld %e\n", wire->NODE2, wire->NODE1, wire->RESI );
      
      if( wire->NODE1 == curnode->INDEX )
        curnode = getlonode( losig, wire->NODE2 );
      else
        curnode = getlonode( losig, wire->NODE1 );
        
      if( RCN_GETFLAG( curnode->FLAG, RCN_FLAG_TWO ) ) {
        if( wire == curnode->WIRELIST->DATA )
          wire = (lowire_list*)curnode->WIRELIST->NEXT->DATA;
        else
          wire = (lowire_list*)curnode->WIRELIST->DATA;
        encore = 1;
      }
      else
        encore = 0;
    }
    while( encore );

  }
  fprintf( file, "\n" );
  
  if( RCN_GETFLAG( curnode->FLAG, RCN_FLAG_CROSS ) || wire == NULL ) {
    for( scan = curnode->WIRELIST ; scan ; scan = scan->NEXT ) {
      if( scan->DATA != wire ) {
        rcn_dumprcn( losig, 
                     curnode->INDEX, 
                     NULL, 
                     (lowire_list*)scan->DATA, 
                     file 
                   );
      }
    }
  }

  if( first ) { /* premier appel */
    for( wire = losig->PRCN->PWIRE ; wire ; wire = wire->NEXT )
      RCN_CLEARFLAG( wire->FLAG, RCN_FLAG_PASS );
    if( file != stdout )
      fclose(file);
  }
}

// Marque un noeud. Renvoie 1 si il état déjà marqué, et 0 sinon.

int rcn_testandmarknode( lonode_list *ptnode )
{
  if( RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_PASS ) )
    return 1;
  RCN_SETFLAG( ptnode->FLAG, RCN_FLAG_PASS );
  return 0;
}


/******************************************************************************\
Fonctions permettant de mémoriser de l'information entre deux losig. L'ordre
victime agresseur est significatif.
La variable 'ptype' sert à différencier plusieurs informations différentes. 
C'est un identifiant de ptype ordinaire, mais dont le dernier bit est géré par 
ces fonctions. 
ex : le ptype 1702  ( 11010100110 ) devient dans le USER des losig 1702 ou 1703.
L'argument data doit être différent de EMPTYHT.

********************************************************************************

Principe :

Dans beaucoups de cas, il y a très peu de couplage. Une ptype_list pour stocker
l'agresseur (champs DATA) et l'information (champs TYPE) est donc satisfaisante.
Si on récupère successivement toutes les informations de couplage, cela conduit
à un algos en n².

Lorsque le n² devient inacceptable ( RCN_MAXCHAIN²), les ptype_list sont
remplacées par des tables de hash. Le gain est double : la consomation mémoire
( la table de hash est un tableau de 2 valeurs, il n'y a pas de champs NEXT)
et le temps de calcul sont réduit.

Q : Pourquoi ne pas avoir utilisé alors systématiquement une table de hash ?

Parceque le malloc() standard utilisé par défaut dans addht() consomme en
réalité 24 octets au lieu de 12, et si on prend un petit nombre initial (par
exemple 2), le malloc des htitem consomme 24 octets au lieu de 16. Pour les 
petits nombres d'agresseurs actifs (jusqu'à 6), la table de hash est donc plus 
coûteuse en mémoire, pour un temps CPU plus grand à cause de la fonction de
hash.

\******************************************************************************/

long rcn_getcoupledinformation( victime, agresseur, ptype )
losig_list      *victime;
losig_list      *agresseur;
long             ptype;
{
  ptype_list    *ptl;
  long           v;
  long           ptype_ptype;
  long           ptype_thash;

  ptype_ptype = ptype & (~((long)0x1));
  ptype_thash = ptype | ((long)0x1);

  ptl = getptype( victime->USER, ptype_thash );

  if( ptl ) {

    v = gethtitem( ((ht*)(ptl->DATA)), agresseur );
    if( v != EMPTYHT )
      return v ;

    return 0l;
  }

  ptl = getptype( victime->USER, ptype_ptype );
  
  if( ptl ) {
  
    for( ptl = ((ptype_list*)(ptl->DATA)) ; 
         ptl && (ptl->DATA) != agresseur ;
         ptl = ptl->NEXT );
         
    if( ptl )
      return ptl->TYPE ;
      
    return 0l;
  }

  return 0l;
}

void rcn_setcoupledinformation( victime, agresseur, data, ptype )
losig_list      *victime;
losig_list      *agresseur;
long             data;
long             ptype;
{
  ptype_list    *ptl;
  ptype_list    *scan;
  int            n;
  ht            *newht;
  long           ptype_ptype;
  long           ptype_thash;

  ptype_ptype = ptype & (~((long)0x1));
  ptype_thash = ptype | ((long)0x1);

  ptl = getptype( victime->USER, ptype_thash );

  if( ptl ) {
    sethtitem( ((ht*)(ptl->DATA)), agresseur, (long)data );
    return ;
  }

  ptl = getptype( victime->USER, ptype_ptype );

  if( !ptl ) {

    victime->USER = addptype( victime->USER,
                              ptype_ptype,
                              NULL
                            );
    ptl = victime->USER;
  }
 
  for( scan = ((ptype_list*)(ptl->DATA)), n=0 ; 
       scan && scan->DATA != agresseur ;
       scan = scan->NEXT, n++ );
         
  if( scan ) {
    scan->TYPE = data;
    return ;
  }

  if( n > RCN_MAXCHAIN ) {

    newht = addht( RCN_MAXCHAIN + 1 );

    for( scan = ((ptype_list*)( ptl->DATA )) ; scan ; scan = scan->NEXT )
      addhtitem( newht, scan->DATA, scan->TYPE );
    addhtitem( newht, agresseur, data );
   
    freeptype( ((ptype_list*)(ptl->DATA)) );
    victime->USER = delptype( victime->USER, ptype_ptype );
    ptl = NULL;

    victime->USER = addptype( victime->USER, ptype_thash, newht );
      
  }
  else {

    ptl->DATA = addptype( ((ptype_list*)(ptl->DATA)),
                                           data,
                                           agresseur
                                         );
  }
}

void rcn_freecoupledinformation( losig_list *losig, long ptype )
{
  ptype_list    *ptl;
  long           ptype_ptype;
  long           ptype_thash;

  ptype_ptype = ptype & (~((long)0x1));
  ptype_thash = ptype | ((long)0x1);

  ptl = getptype( losig->USER, ptype_ptype );
  if( ptl ) {
    freeptype( (ptype_list*)ptl->DATA );
    losig->USER = delptype( losig->USER, ptype_ptype );
  }

  ptl = getptype( losig->USER, ptype_thash );
  if( ptl ) {
    delht( (ht*)ptl->DATA );
    losig->USER = delptype( losig->USER, ptype_thash );
  }
}

/******************************************************************************\
Fonctions de parcours générique des arbres rc.
renvoie :
 RCNTREETRIPOK     : tout s'est bien passé
 RCNTREETRIPUSERKO : une fonction utilisateur a renvoyée 0
 RCNTREETRIPLOOP   : une boucle a été détectée.

Algo iteratif sur les résistances mises bout à bout pour éviter trop
de recursion qui peuvent provoquer un débordement de pile.
Au retour, tous les noeuds parcourus ont l'indicateur RCN_FLAG_PASS positionné.
maxdepth est la profondeur de parcours maximum. Si 0, le parcours est illimité.
\******************************************************************************/
int  rcn_treetrip( losig_list  *losig,
                   lonode_list *lonode,
                   int          fn_up( losig_list*, 
                                       lonode_list*, 
                                       lowire_list*,
                                       void*
                                     ),
                   int          fn_down( losig_list*,
                                         lonode_list*,
                                         chain_list*,
                                         ht*,
                                         void*
                                       ),
                   int          fn_pack_up( losig_list*,
                                            lonode_list*,
                                            void*,
                                            void*
                                          ),
                   void *userdata,
                   int   maxdepth
                 )
{
  int r ;

  if( maxdepth <= 0 )
    maxdepth = INT_MAX ;

  clearallwireflag( losig, RCN_FLAG_PASS );
  rcn_build_para( losig );

  if( fn_up ) {
    if( ! fn_up( losig, lonode, NULL, userdata ) )
      return RCNTREETRIPUSERKO ;
  }

  RCN_SETFLAG( lonode->FLAG, RCN_FLAG_PASS );

  r = rcn_treetrip_rec( losig, 
                        lonode, 
                        fn_up, 
                        fn_down, 
                        fn_pack_up, 
                        NULL, 
                        NULL,
                        userdata,
                        maxdepth
                      );

  rcn_clean_para( losig );
  return r ;
}

int  rcn_treetrip_rec( losig_list  *losig,
                       lonode_list *lonode,
                       int          fn_up( losig_list*, 
                                           lonode_list*, 
                                           lowire_list*,
                                           void*
                                         ),
                       int          fn_down( losig_list*,
                                             lonode_list*,
                                             chain_list*,
                                             ht*,
                                             void*
                                           ),
                       int          fn_pack_up( losig_list*,
                                                lonode_list*,
                                                void*,
                                                void*
                                              ),
                       lowire_list *from,
                       void        *prevpack,
                       void        *userdata,
                       int          maxdepth
                     )
{
  lonode_list *start ;
  lonode_list *firstnode ;
  lowire_list *wire ;
  char         doitagain ;
  chain_list  *bellow ;
  ht          *htpack ;
  chain_list  *chpack ;
  void        *pack ;
  chain_list  *list ;
  int          r ;
  chain_list  *headpack ;
  chain_list  *scanpack ;
  chain_list  *nodeout ;
  lowire_list *last ;
  int          error ;
  
  firstnode = lonode ;
  doitagain = 1 ;
  
  /******* parcours dans le sens montant **************************************/
  do {
    if( lonode != firstnode ) {
    
      if( RCN_GETFLAG( lonode->FLAG, RCN_FLAG_PASS ) )
        return RCNTREETRIPLOOP ;
      RCN_SETFLAG( lonode->FLAG, RCN_FLAG_PASS );
      
      if( fn_up ) {
        if( ! fn_up( losig, lonode, from, userdata ) )
          return RCNTREETRIPUSERKO;
      }
      maxdepth--;
    }
    
    wire = NULL ;
    
    if( maxdepth > 0 ) {
      if( ( RCN_GETFLAG( lonode->FLAG, RCN_FLAG_TWO ) == RCN_FLAG_TWO ) &&  
          from != NULL ) {
        if( (lowire_list*)lonode->WIRELIST->DATA == from )
          wire = (lowire_list*)lonode->WIRELIST->NEXT->DATA ;
        else
          wire = (lowire_list*)lonode->WIRELIST->DATA ;
      }
      else {
        if( ( RCN_GETFLAG( lonode->FLAG, RCN_FLAG_ONE ) == RCN_FLAG_ONE ) &&
            from == NULL ) {
          wire = (lowire_list*)lonode->WIRELIST->DATA ;
        }
      }
    }
    
    if( wire && RCN_GETFLAG( wire->FLAG, RCN_FLAG_IGNORE ) )
      wire = NULL ;
    
    if( wire ) {
      lonode = getlonode( losig,
                          (wire->NODE1 == lonode->INDEX ) ? wire->NODE2 : 
                                                            wire->NODE1 
                        );
      from = wire ;
    }
    else
      doitagain = 0 ;
  }
  while( doitagain );
  
  /******* Intersection de plusieurs résistances ******************************/

  /* propagation montante */

  htpack = NULL;
  bellow = NULL ;
  error  = RCNTREETRIPOK ;
  if( maxdepth > 0 ) {
    for( list = lonode->WIRELIST ; list ; list = list->NEXT ) {
    
      wire = (lowire_list*)list->DATA ;
      
      if( wire != from && !RCN_GETFLAG( wire->FLAG, RCN_FLAG_IGNORE ) ) {
      
        pack = rcn_getpack( losig, wire );
        
        if( pack ) {
       
          if( pack != prevpack ) {
            if( !htpack )
              htpack = addht(5);
            
            chpack = (void*)gethtitem( htpack, pack );
            if( chpack == (void*)EMPTYHT ) {
              chpack = addchain( NULL, wire );
            }
            else {
              chpack = addchain( chpack, wire );
            }
            addhtitem( htpack, pack, (long)chpack );
          }
        }
        else {
          bellow = addchain( bellow, wire );
          start = getlonode( losig,
                             ( wire->NODE1 == lonode->INDEX ) ? wire->NODE2 :
                                                                wire->NODE1
                           );
      
          if( RCN_GETFLAG( start->FLAG, RCN_FLAG_PASS ) ) {
            error = RCNTREETRIPLOOP ;
            break ;
          }
          RCN_SETFLAG( start->FLAG, RCN_FLAG_PASS );

          if( fn_up )  {
            r = fn_up( losig, start, wire, userdata );
            if( !r ) {
              error = RCNTREETRIPUSERKO ;
              break ;
            }
          }

          maxdepth-- ;
         
          if( maxdepth > 0 ) {
            r = rcn_treetrip_rec( losig,
                                  start,
                                  fn_up,
                                  fn_down,
                                  fn_pack_up,
                                  wire,
                                  NULL,
                                  userdata,
                                  maxdepth
                                );
            if( r != RCNTREETRIPOK ) {
              error = r;
              break;
            }
          }
        }
      }
    }
  }

  headpack = NULL ;
  
  if( error == RCNTREETRIPOK ) {

    if( htpack ) {
    
      headpack = GetAllHTElems( htpack );
      
      for( scanpack = headpack ; scanpack ; scanpack = scanpack->NEXT ) {
      
        chpack = scanpack->DATA ;
        wire = (lowire_list*)chpack->DATA;
        pack = rcn_getpack( losig, wire );
        
        if( fn_pack_up ) {
          r = fn_pack_up( losig, lonode, pack, userdata );
          if( !r ) {
            error = RCNTREETRIPUSERKO ;
            break ;
          }
        }

        maxdepth-- ;
        
        nodeout = rcn_expandpack( losig, lonode, chpack, 1 );
        rcn_expandpack( losig, lonode, chpack, 0 );
        
        for( list = nodeout ; list ; list = list->NEXT ) {
        
          if( RCN_GETFLAG( ((lonode_list*)list->DATA)->FLAG, RCN_FLAG_PASS ) )
          {
            error = RCNTREETRIPLOOP ;
            break ;
          }
          RCN_SETFLAG( ((lonode_list*)list->DATA)->FLAG, RCN_FLAG_PASS ) ;

          r = rcn_treetrip_rec( losig,
                                (lonode_list*)list->DATA,
                                fn_up,
                                fn_down,
                                fn_pack_up,
                                NULL,
                                pack,
                                userdata,
                                maxdepth
                              );
          if( r != RCNTREETRIPOK ) {
            error = r ;
            break ;
          }
        }

        freechain( nodeout );

        if( error != RCNTREETRIPOK )
          break ;
      }
    }
  }

  /* propagation descendante */

  if( error == RCNTREETRIPOK ) {
    if( fn_down ) {
      r= fn_down( losig, lonode, bellow, htpack, userdata );
      if( !r )
        error = RCNTREETRIPUSERKO ;
    }
  }
 
  /* lé ménache */
  if( htpack ) {
      freechain(headpack);
      headpack = GetAllHTElems( htpack );
      for( scanpack = headpack ; scanpack ; scanpack = scanpack->NEXT ) {
        chpack = scanpack->DATA ;
        freechain( chpack );
      }
      freechain( headpack );
    delht( htpack );
  }
  freechain( bellow );
  bellow = NULL;

  if( error != RCNTREETRIPOK ) 
    return error ;

  /******* parcours dans le sens descendant ***********************************/

  doitagain = 1 ;
  last = NULL ;
  
  do {
  
    if( fn_down ) {
      if( last ) {
        bellow = addchain( NULL, last );
        r = fn_down( losig, lonode, bellow, NULL, userdata );
        freechain( bellow );
        if( !r )
          return RCNTREETRIPUSERKO ;
      }
    }


    last = from ;

    if( lonode != firstnode ) {
      lonode = getlonode( losig, 
                          ( from->NODE1 == lonode->INDEX ) ? from->NODE2 : 
                                                             from->NODE1
                        );
      if( lonode != firstnode ) {
        from = ( (lowire_list*)lonode->WIRELIST->DATA == from ) ?
                        (lowire_list*)lonode->WIRELIST->NEXT->DATA :
                        (lowire_list*)lonode->WIRELIST->DATA ;
      }
    }
    else
      doitagain = 0 ;
  } 
  while( doitagain );

  return RCNTREETRIPOK ;
}

/******************************************************************************\
Renvoie tous les noeuds à l'exterieur d'un pack contenant les résistances 
données dans hwire.
\******************************************************************************/
chain_list* rcn_expandpack( losig_list *losig, 
                            lonode_list *node, 
                            chain_list *hwire,
                            char        search
                          )
{
  chain_list  *chain ;
  lowire_list *wire ;
  void        *pack ;
  int          inode ;
  lonode_list *pnode ;
  chain_list  *todo ;
  chain_list  *scan ;
  lowire_list *twire ;
  void        *tpack ;
  chain_list  *found ;
  chain_list  *bellow ;
  
  found = NULL ;
  
  if( search ) {
    pack = rcn_getpack( losig, (lowire_list*)hwire->DATA );
    RCN_SETFLAG( node->FLAG, RCN_FLAG_PASS2 );
  }
  else {
    RCN_CLEARFLAG( node->FLAG, RCN_FLAG_PASS2 );
  }

  for( chain = hwire ; chain ; chain = chain->NEXT ) {
    wire = (lowire_list*)chain->DATA ;
    inode = ( wire->NODE1 == node->INDEX ? wire->NODE2 : wire->NODE1 );
    pnode = getlonode( losig, inode );
    if( search ) {
      if( !RCN_GETFLAG( pnode->FLAG, RCN_FLAG_PASS2 ) ) {
        todo = NULL ;
        found = addchain( found, pnode );
        for( scan = pnode->WIRELIST ; scan ; scan = scan->NEXT ) {
          twire = (lowire_list*)scan->DATA;
          if( RCN_GETFLAG( twire->FLAG, RCN_FLAG_IGNORE ) )
            continue ;
          tpack = rcn_getpack( losig, twire );
          if( wire != twire ) {
            if( tpack == pack )
              todo = addchain( todo, twire );
          }
        }
        if( todo ) {
          bellow = rcn_expandpack( losig, pnode, todo, search );
          found = append( bellow, found );
          freechain( todo );
        }
      }
    }
    else {
      if( RCN_GETFLAG( pnode->FLAG, RCN_FLAG_PASS2 ) ) {
        todo = NULL ;
        for( scan = pnode->WIRELIST ; scan ; scan = scan->NEXT ) {
          twire = (lowire_list*)scan->DATA ;
          if( RCN_GETFLAG( twire->FLAG, RCN_FLAG_IGNORE ) )
            continue ;
          if( wire != twire )
            todo = addchain( todo, twire );
        }
        if( todo ) {
          rcn_expandpack( losig, pnode, todo, search );
          freechain( todo );
        }
      }
    }
  }

  return found ;
}

/******************************************************************************\
Pour gérer les ensembles de résistance dans les arbres RC.
\******************************************************************************/
void rcn_setpack( losig_list *losig, lowire_list *wire, void *pack )
{
  ptype_list *ptl ;

  ptl = getptype( wire->USER, RCNWIREPACK );
  if( ptl )
    ptl->DATA = pack ;
  else
    wire->USER = addptype( wire->USER, RCNWIREPACK, pack );

  losig = NULL ;
}

void* rcn_getpack( losig_list *losig, lowire_list *wire )
{
  ptype_list *ptl ;
  void       *pack ;

  ptl = getptype( wire->USER, RCNWIREPACK );
  if( ptl )
    pack = ptl->DATA ;
  else
    pack = NULL ;

  losig = NULL ;
  return pack ;
}

void rcn_clearpack( losig_list *losig, lowire_list *wire )
{
  ptype_list *ptl ;

  ptl = getptype( wire->USER, RCNWIREPACK );
  if( ptl )
    wire->USER = delptype( wire->USER, RCNWIREPACK );
  losig = NULL ;
}

/******************************************************************************\
Pour gérer les résistances en parallèle dans les réseaux RC.

Parmis n resistances parallèles, une seule est à conserver. Elle contient un
ptype RCNPARA qui contient une chain_list des autres résistances en parallèles.
Ces autres résistances sont marquées RCN_FLAG_IGNORE.
Marque aussi les résistances entre deux même noeud avec RCN_FLAG_IGNORE.

La construction de cette information se fait avec rcn_build_para(). Il faut la
libérer avec rcn_clean_para().

Problème avec l'utilisation des caches RC : cette information ne doit pas
être conservée. Pour ne pas reconstruire plusieurs fois cette information si 
ce n'est pas nécessaire, on utilise le flag RCNCACHEFLAG_NOPARA dans le losig
pour indiquer que la recherche a déjà été effectuée et qu'elle n'a rien donnée.
\******************************************************************************/

void rcn_build_para( losig_list *losig )
{
  lowire_list *wire ;

  if( RCN_GETFLAG( losig->RCNCACHE, RCNCACHEFLAG_NOPARA ) ) 
    return ;

  RCN_SETFLAG( losig->RCNCACHE, RCNCACHEFLAG_NOPARA );

  for( wire = losig->PRCN->PWIRE ; wire ; wire = wire->NEXT ) {
    if( rcn_build_para_from_node( losig, getlonode( losig, wire->NODE1 ) ) )
      RCN_CLEARFLAG( losig->RCNCACHE, RCNCACHEFLAG_NOPARA );
    if( rcn_build_para_from_node( losig, getlonode( losig, wire->NODE2 ) ) )
      RCN_CLEARFLAG( losig->RCNCACHE, RCNCACHEFLAG_NOPARA );
  }
}

int rcn_build_para_from_node( losig_list *losig, lonode_list *lonode )
{
  chain_list    *chain ;
  lowire_list   *wire ;
  int            inode ;
  lonode_list   *pnode ;
  chain_list    *chain2 ;
  lowire_list   *retwire ;
  int            found ;
  ptype_list    *ptl ;

  found = 0 ;
  
  for( chain = lonode->WIRELIST ; chain ; chain = chain->NEXT ) {
  
    wire = (lowire_list*)chain->DATA ;
    
    if( RCN_GETFLAG( wire->FLAG, RCN_FLAG_IGNORE ) )
      continue ;
      
    inode = (wire->NODE1 == lonode->INDEX ? wire->NODE2 : wire->NODE1 );
    
    if( inode == lonode->INDEX ) {
      /* Résistance sur un seul noeud */
      RCN_SETFLAG( wire->FLAG, RCN_FLAG_IGNORE ) ;
      found = 1 ;
      continue ;
    }

    if( inode > lonode->INDEX ) {
    
      pnode = getlonode( losig, inode );
      
      for( chain2 = pnode->WIRELIST ; chain2 ; chain2 = chain2->NEXT ) {
      
        retwire = (lowire_list*)chain2->DATA ;

        if( retwire == wire )
          continue ;

        if( RCN_GETFLAG( retwire->FLAG, RCN_FLAG_IGNORE ) )
          continue ;
        
        if( retwire->NODE1 == lonode->INDEX || 
            retwire->NODE2 == lonode->INDEX    ) {

          /* Résistance retwire et parallele avec wire */
          ptl = getptype( wire->USER, RCNPARA ) ;
          if( ptl ) 
            ptl->DATA = addchain( (chain_list*)ptl->DATA, retwire );
          else
            wire->USER = addptype( wire->USER, 
                                   RCNPARA, 
                                   addchain( NULL, retwire ) 
                                 );

          RCN_SETFLAG( retwire->FLAG, RCN_FLAG_IGNORE );
          found = 1 ;
        }
      }
    }
  }

  return found ;
}

void rcn_clean_para( losig_list *losig )
{
  lowire_list   *wire ;
  ptype_list    *ptl ;
  
  for( wire = losig->PRCN->PWIRE ; wire ; wire = wire->NEXT ) {
    ptl = getptype( wire->USER, RCNPARA ) ;
    if( ptl ) {
      freechain( (chain_list*) ptl->DATA );
      wire->USER = delptype( wire->USER, RCNPARA );
    }
    RCN_CLEARFLAG( wire->FLAG, RCN_FLAG_IGNORE );
  }
}

float rcn_get_resi_para( lowire_list *wire ) 
{
  float          resi ;
  ptype_list    *ptl ;
  lowire_list   *pwire ;
  chain_list    *chain ;

  resi = wire->RESI ;
  ptl = getptype( wire->USER, RCNPARA );
  if( ptl ) {
    for( chain = (chain_list*)ptl->DATA ; chain ; chain = chain->NEXT ) {
      pwire = (lowire_list*)chain->DATA ;
      resi = ( resi * pwire->RESI ) / ( resi + pwire->RESI );
    }
  }
  return resi ;
}

/******************************************************************************\
Applique la fonction fn() sur toutes les résistances d'un réseau RC accessibles
depuis le noeud start. L'ordre de parcour est en profondeur d'abord, sauf si des
boucles sont présentes.
Au retour de la fonction, tous les noeuds parcourus ont l'indicateur
RCN_FLAG_PASS positionné.
Algo iteratif sur les résistances mises bout à bout pour éviter trop
de recursion qui peuvent provoquer un débordement de pile.

code de retour : RCNTRIPCONNEXE_OK ou RCNTRIPCONNEXE_USERKO.
fn doit renvoyer 1 si ok, ou 0 si ko.

\******************************************************************************/
int rcn_tripconnexe( losig_list *losig,
                     int         startnode,
                     int         fn( losig_list*, 
                                     lowire_list*,
                                     void*
                                   ),
                     void       *userdata
                   )
{
  lonode_list *ptnode ;
  lowire_list *wire ;
  chain_list  *chwire ;

  for( wire = losig->PRCN->PWIRE ; wire ; wire = wire->NEXT ) 
    RCN_CLEARFLAG( wire->FLAG, RCN_FLAG_PASS );
  
  ptnode = getlonode( losig, startnode );

  for( chwire = ptnode->WIRELIST ; chwire ; chwire = chwire->NEXT ) {
    wire = (lowire_list*)chwire->DATA ;
    if( rcn_tripconnexe_rec( losig, ptnode, wire, fn, userdata ) !=
        RCNTRIPCONNEXE_OK )
      break ;
  }

  if( chwire )
    return RCNTRIPCONNEXE_USERKO ;
  return RCNTRIPCONNEXE_OK ;
}

int rcn_tripconnexe_rec( losig_list  *losig,
                         lonode_list *from,
                         lowire_list *wire,
                         int          fn( losig_list*, 
                                          lowire_list*,
                                          void*
                                        ),
                         void        *userdata
                       )
{
  lonode_list *othernode ;
  chain_list  *chwire ;

  if( RCN_GETFLAG( wire->FLAG, RCN_FLAG_PASS ) ) {
    /* a loop is found */
    return RCNTRIPCONNEXE_OK ;
  }
    
  if( fn( losig, wire, userdata ) == 0 )
    return RCNTRIPCONNEXE_USERKO ;
  RCN_SETFLAG( wire->FLAG, RCN_FLAG_PASS );

  if( wire->NODE1 == from->INDEX )
    othernode = getlonode( losig, wire->NODE2 );
  else
    othernode = getlonode( losig, wire->NODE1 );

  while( RCN_GETFLAG( othernode->FLAG, RCN_FLAG_TWO ) ) {

    if( wire == (lowire_list*)othernode->WIRELIST->DATA )
      wire = (lowire_list*)othernode->WIRELIST->NEXT->DATA ;
    else
      wire = (lowire_list*)othernode->WIRELIST->DATA ;

    if( RCN_GETFLAG( wire->FLAG, RCN_FLAG_PASS ) )
      break ;

    if( fn( losig, wire, userdata ) == 0 )
      return RCNTRIPCONNEXE_USERKO ;
    RCN_SETFLAG( wire->FLAG, RCN_FLAG_PASS );
      
    if( wire->NODE1 == othernode->INDEX )
      othernode = getlonode( losig, wire->NODE2 );
    else
      othernode = getlonode( losig, wire->NODE1 );
  }

  for( chwire = othernode->WIRELIST ; chwire ; chwire = chwire->NEXT ) {
    if( rcn_tripconnexe_rec( losig, 
                             othernode, 
                             (lowire_list*)chwire->DATA, 
                             fn, 
                             userdata 
                           ) != RCNTRIPCONNEXE_OK )
    break ;
  }

  if( chwire )
    return RCNTRIPCONNEXE_USERKO;
  return RCNTRIPCONNEXE_OK ;
}

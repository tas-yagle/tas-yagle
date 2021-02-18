/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : RCX - AWE support.                                          */
/*    Fichier : trc_awe_generalized_moment.c                                */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Grégoire Avot                                             */
/*                                                                          */
/****************************************************************************/

/* CVS informations :

Revision : $$
Author   : $$
Date     : $$

*/

#define AWE_NORMALISE
#include "trc.h"

HeapAlloc HEAP_AWE_MATRIX_NODE ;
HeapAlloc HEAP_AWE_HYBRID_MATRIX ;
HeapAlloc HEAP_AWE_HYBRID_NODE ;

float AWE_MIN_RESI_VALUE = 0.1 ;
/******************************************************************************\
build_awe_moment_with_loop()
build moment for any interconnect.
\******************************************************************************/

void build_awe_moment_with_loop( losig_list *losig, 
                                 locon_list *driver, 
                                 RCXFLOAT    extcapa, 
                                 rcx_slope  *slope, 
                                 char        type, 
                                 RCXFLOAT    coefctc )
{
  wirefromlocon_list *connexe ;
  wirefromlocon_list *group ;
  awe_matrix_node    *valid_node ;
  awe_matrix_node    *hybrid_node ;
  awe_hybrid_matrix  *head ;
  chain_list         *chain ;
  lowire_list        *wire ;
  num_list           *tmpnum ;
  num_list           *num ;
  lonode_list        *startnode ;
  ptype_list         *ptl ;

  avt_logenterfunction(LOGTRC,2,"build_awe_moment_with_loop()\n" );

  CreateHeap( sizeof( awe_matrix_node ),   1024, &HEAP_AWE_MATRIX_NODE );
  CreateHeap( sizeof( awe_hybrid_matrix ),   16, &HEAP_AWE_HYBRID_MATRIX );
  CreateHeap( sizeof( awe_hybrid_node ),     16, &HEAP_AWE_HYBRID_NODE );

  connexe = rcx_get_wire_connexe( losig, driver->PNODE );
 
  for( group = connexe ; group ; group = group->NEXT ) {

    avt_log(LOGTRC,2,"new connexe component\n");

    if( !group->WIRELIST )
      continue ;

    for( chain = group->WIRELIST ; chain ; chain = chain->NEXT ) {
      wire = (lowire_list*)chain->DATA ;
      if( wire->RESI > AWE_MIN_RESI_VALUE )
        break ;
    }
    if( !chain ) {
      avt_log(LOGTRC,2,"no valid wire inside\n" );
      continue ;
    }
    
    awe_build_equiv_lonode( losig, group->WIRELIST );

    if( RCX_AWE_ONE_PNODE == 'Y' ) {
   
      avt_log(LOGTRC,2,"one pnode for driver\n");
      for( num = driver->PNODE ; num ; num = num->NEXT ) {
        avt_log(LOGTRC,2,"driver=%ld\n",num->DATA );
        
        startnode = getlonode( losig, num->DATA ) ;
        ptl = getptype( startnode->USER, WIRECONNEXE ) ;
        
        if( ptl && ptl->DATA == group ) {
        
          tmpnum = addnum( NULL, num->DATA );

          avt_log(LOGTRC,2,"sort node\n");
          sort_node( losig, tmpnum, group, 
                     extcapa, slope, type, coefctc, 
                     &valid_node, &hybrid_node 
                   );
          avt_log(LOGTRC,2,"build node index\n");
          build_node_index( valid_node );
          avt_log(LOGTRC,2,"sort hybrid node\n");
          head = build_sort_hybrid( losig, hybrid_node );
          avt_log(LOGTRC,2,"building awe matrix\n");
          build_awe( losig, 
                     driver, 
                     num->DATA, 
                     valid_node, 
                     head, 
                     extcapa, 
                     group->WIRELIST 
                   );
          avt_log(LOGTRC,2,"cleanning\n"); 
          free_hybrid( head );
          free_sort_node( valid_node  );
          free_sort_node( hybrid_node );
          freenum( tmpnum );
        }
      }
    }
    else {
      avt_log(LOGTRC,2,"multi pnode for driver\n");
      avt_log(LOGTRC,2,"sort node\n");
      sort_node( losig, driver->PNODE, group, 
                 extcapa, slope, type, coefctc, 
                 &valid_node, &hybrid_node 
               );

      avt_log(LOGTRC,2,"build node index\n");
      build_node_index( valid_node );
      avt_log(LOGTRC,2,"sort hybrid node\n");
      head = build_sort_hybrid( losig, hybrid_node );
      avt_log(LOGTRC,2,"building awe matrix\n");
      build_awe( losig, driver, 0, valid_node, head, extcapa, group->WIRELIST );
      
      avt_log(LOGTRC,2,"cleanning\n"); 
      free_hybrid( head );
      free_sort_node( valid_node  );
      free_sort_node( hybrid_node );
    }

    awe_clean_equiv_lonode( losig, group->WIRELIST );
  }

  rcx_free_wire_connexe( losig, driver->PNODE, connexe );
  DeleteHeap( &HEAP_AWE_MATRIX_NODE   );
  DeleteHeap( &HEAP_AWE_HYBRID_NODE   );
  DeleteHeap( &HEAP_AWE_HYBRID_MATRIX );

  avt_logexitfunction(LOGTRC,2);
}

/******************************************************************************\
awe_get_equiv_lonode()
return the real lonode to take into account nul wires
\******************************************************************************/
lonode_list* awe_get_equiv_lonode( lonode_list *lonode )
{
  ptype_list *ptl ;

  ptl = getptype( lonode->USER, AWE_EQUIV_NODE ) ;

  if( ptl ) 
    lonode = (lonode_list*)ptl->DATA ;

  return lonode ;
}

/******************************************************************************\
awe_build_equiv_lonode()
build equivalent lonode in order to handle null wires.
\******************************************************************************/
void awe_build_equiv_lonode( losig_list *losig, chain_list *headwire )
{
  lowire_list *wire ;
  chain_list  *chain ;
  chain_list  *head ;
  lonode_list *o_equiv, *n_equiv ;
  chain_list *w_equiv ;

  avt_logenterfunction(LOGTRC,2,"awe_build_equiv_lonode");

  for( chain = headwire ; chain ; chain = chain->NEXT ) {
    wire = (lowire_list*)chain->DATA ;
    RCN_CLEARFLAG( wire->FLAG, RCN_FLAG_PASS );
  }

  for( chain = headwire ; chain ; chain = chain->NEXT ) {
  
    wire = (lowire_list*)chain->DATA ;
    if( RCN_GETFLAG( wire->FLAG, RCN_FLAG_PASS ) )
      continue ;
    if( wire->RESI > AWE_MIN_RESI_VALUE ) 
      continue ;
    head = NULL ;
    
    awe_find_nul_wire_rec( &head, losig, getlonode( losig, wire->NODE1 ) ) ;
    awe_find_nul_wire_rec( &head, losig, getlonode( losig, wire->NODE2 ) ) ;
    
    n_equiv = getlonode( losig, ((lowire_list*)head->DATA)->NODE1 ) ;
    o_equiv = getlonode( losig, ((lowire_list*)head->DATA)->NODE2 ) ;
    o_equiv->USER = addptype( o_equiv->USER, AWE_EQUIV_NODE, n_equiv ) ;
    
    for( w_equiv = head->NEXT ; w_equiv ; w_equiv = w_equiv->NEXT ) {
      wire = (lowire_list*)w_equiv->DATA ;
      o_equiv = getlonode( losig, wire->NODE1 );
      if( !getptype( o_equiv->USER, AWE_EQUIV_NODE ) )
        o_equiv->USER = addptype( o_equiv->USER, AWE_EQUIV_NODE, n_equiv ) ;
      o_equiv = getlonode( losig, wire->NODE2 );
      if( !getptype( o_equiv->USER, AWE_EQUIV_NODE ) )
        o_equiv->USER = addptype( o_equiv->USER, AWE_EQUIV_NODE, n_equiv ) ;
    }
    
    freechain( head );
  }

  avt_logexitfunction(LOGTRC,2);
}

void awe_find_nul_wire_rec( chain_list **head, 
                            losig_list *losig, 
                            lonode_list *lonode 
                          )
{
  chain_list  *chain ;
  lowire_list *wire ;
  
  for( chain = lonode->WIRELIST ; chain ; chain = chain->NEXT ) {
    wire = (lowire_list*)chain->DATA ;
    if( ! RCN_GETFLAG( wire->FLAG, RCN_FLAG_PASS ) ) {
      if( wire->RESI <= AWE_MIN_RESI_VALUE ) {
        *head = addchain( *head, wire );
        RCN_SETFLAG( wire->FLAG, RCN_FLAG_PASS );
        awe_find_nul_wire_rec( head, 
                               losig, 
                               getlonode( losig, 
                      wire->NODE1 == lonode->INDEX ? wire->NODE2 : wire->NODE1 
                                        ) 
                             ) ;
      }
    }
  }
}

/******************************************************************************\
awe_clean_equiv_lonode()
clean all information added by awe_build_equiv_lonode().
\******************************************************************************/
void awe_clean_equiv_lonode( losig_list *losig, chain_list *headwire )
{
  chain_list *chain ;
  lowire_list *wire ;
  lonode_list *node ;

  avt_logenterfunction(LOGTRC,2,"awe_clean_equiv_lonode()\n");
  for( chain = headwire ; chain ; chain = chain->NEXT ) {
    wire = (lowire_list*)chain->DATA ;
    node = getlonode( losig, wire->NODE1 );
    if( getptype( node->USER, AWE_EQUIV_NODE ) )
      node->USER = delptype( node->USER, AWE_EQUIV_NODE );
    node = getlonode( losig, wire->NODE2 );
    if( getptype( node->USER, AWE_EQUIV_NODE ) )
      node->USER = delptype( node->USER, AWE_EQUIV_NODE );
  }
  avt_logexitfunction(LOGTRC,2);
}

/******************************************************************************\
sort_node()
sort node for the wirefromlocon_list* in the valid node list and the hybrid
node list.
\******************************************************************************/
void sort_node( losig_list         *losig, 
                num_list           *driver, 
                wirefromlocon_list *group,
                RCXFLOAT            extcapa,
                rcx_slope          *slope,
                char                type,
                RCXFLOAT            coefctc,
                awe_matrix_node   **valid_node,
                awe_matrix_node   **hybrid_node
              )
{
  num_list        *num ;
  lonode_list     *ptnode ;
  ptype_list      *ptl ;
  chain_list      *chain ;
  lowire_list     *wire ;
  awe_matrix_node *mtxnode ;

  *valid_node = NULL ;
  *hybrid_node = NULL ;

  for( chain = group->WIRELIST ; chain ; chain = chain->NEXT ) {
    wire = (lowire_list*)chain->DATA ;
    ptnode = getlonode( losig, wire->NODE1 );
    RCN_CLEARFLAG( ptnode->FLAG, RCN_FLAG_PASS );
    ptnode = getlonode( losig, wire->NODE2 );
    RCN_CLEARFLAG( ptnode->FLAG, RCN_FLAG_PASS );
  }
  
  for( num = driver ; num ; num = num->NEXT ) {
  
    ptnode = getlonode( losig, num->DATA );

    ptl = getptype( ptnode->USER, WIRECONNEXE );
    if( !ptl || !ptl->DATA ) {
      awe_error(8,AVT_FATAL);
    }
      
    if( ptl->DATA == group ) {
      ptnode = awe_get_equiv_lonode( ptnode );
      if( ! get_matrix_node( ptnode ) ) {
        mtxnode = add_awe_matrix_node( ptnode );
        mtxnode->FLAGS = mtxnode->FLAGS | AWE_FLAG_DRIVER ;
        mtxnode->NEXT = *valid_node ;
        (*valid_node) = mtxnode ;
      }
    }
  }

  for( chain = group->WIRELIST ; chain ; chain = chain->NEXT ) {
  
    wire = (lowire_list*)chain->DATA ;
    
    ptnode = getlonode( losig, wire->NODE1 );
    test_sort_node( losig, ptnode, 
                    extcapa, slope, type, coefctc
                  );
    
    ptnode = getlonode( losig, wire->NODE2 );
    test_sort_node( losig, ptnode, 
                    extcapa, slope, type, coefctc 
                  );
  }
  
  for( chain = group->WIRELIST ; chain ; chain = chain->NEXT ) {
  
    wire = (lowire_list*)chain->DATA ;
    
    ptnode = getlonode( losig, wire->NODE1 );
    mtxnode = get_matrix_node( ptnode );
    if( mtxnode ) {
      if( mtxnode->FLAGS == 0 ) {
        if( mtxnode->CAPA > 0.0 ) {
          mtxnode->FLAGS = AWE_FLAG_NORMAL ;
          mtxnode->NEXT = *valid_node ;
          (*valid_node) = mtxnode ;
        }
        else {
          mtxnode->FLAGS = AWE_FLAG_HYBRID ;
          mtxnode->NEXT = *hybrid_node ;
          (*hybrid_node) = mtxnode ;
        }
      }
    }
    ptnode = getlonode( losig, wire->NODE2 );
    mtxnode = get_matrix_node( ptnode );
    if( mtxnode ) {
      if( mtxnode->FLAGS == 0 ) {
        if( mtxnode->CAPA > 0.0 ) {
          mtxnode->FLAGS = AWE_FLAG_NORMAL ;
          mtxnode->NEXT = *valid_node ;
          (*valid_node) = mtxnode ;
        }
        else {
          mtxnode->FLAGS = AWE_FLAG_HYBRID ;
          mtxnode->NEXT = *hybrid_node ;
          (*hybrid_node) = mtxnode ;
        }
      }
    }
  }
}

/******************************************************************************\
test_sort_node()
add a node in the correct list.
\******************************************************************************/
void test_sort_node( losig_list         *losig, 
                     lonode_list        *ptnode,
                     RCXFLOAT            extcapa,
                     rcx_slope          *slope,
                     char                type,
                     RCXFLOAT            coefctc
                   )
{
  awe_matrix_node *mtxnode ;
  lonode_list     *equivnode ;

  if( RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_PASS ) )
    return ;

  RCN_SETFLAG( ptnode->FLAG, RCN_FLAG_PASS );
  
  equivnode = awe_get_equiv_lonode( ptnode );

  mtxnode = get_matrix_node( equivnode );
  if( ! mtxnode ) {
    mtxnode = add_awe_matrix_node( equivnode );
    mtxnode->CAPA =
             rcx_getnodecapa( losig, equivnode, extcapa, slope, type, TRC_HALF, coefctc );
  }

  if( ptnode != equivnode )
    mtxnode->CAPA = mtxnode->CAPA +
                rcx_getnodecapa( losig, ptnode, extcapa, slope, type, TRC_HALF, coefctc );
}

/******************************************************************************\
add_awe_matrix_node()
\******************************************************************************/
awe_matrix_node* add_awe_matrix_node( lonode_list *ptnode )
{
  awe_matrix_node *mtxnode ;

  mtxnode         = (awe_matrix_node*)AddHeapItem( &HEAP_AWE_MATRIX_NODE );
    
  mtxnode->INDEX  = -1 ;
  mtxnode->LONODE = ptnode ;
  mtxnode->CAPA   = 0.0 ;
  mtxnode->FLAGS  = 0 ;
  ptnode->USER = addptype( ptnode->USER, AWE_NODE_M, mtxnode );

  return mtxnode ;
}

/******************************************************************************\
free_sort_node()
free the awe_matrix_node list and all memory allocated in sort_node().
\******************************************************************************/
void free_sort_node( awe_matrix_node *nodelist )
{
  awe_matrix_node *scan, *next ;

  for( scan = nodelist ; scan ; scan = next ) {
    next = scan->NEXT ;
    scan->LONODE->USER = delptype( scan->LONODE->USER, AWE_NODE_M );
    DelHeapItem( &HEAP_AWE_MATRIX_NODE, scan );
  }
}

/******************************************************************************\
build_node_index()
build index for node that are not on the driver. driver is identified with the
WIRECONNEXE ptype.
\******************************************************************************/
void build_node_index( awe_matrix_node *nodelist )
{
  awe_matrix_node *scan ;
  int              index ;

  index = 0 ;

  for( scan = nodelist ; scan ; scan = scan->NEXT ) {
    if( ( scan->FLAGS & AWE_FLAG_DRIVER ) != AWE_FLAG_DRIVER ) {
      scan->INDEX = index ;
      index++;
    }
  }
}

/******************************************************************************\
get_matrix_node()
return then awe_matrix_node associed to a lonode_list.
\******************************************************************************/
awe_matrix_node* get_matrix_node( lonode_list *ptnode )
{
  ptype_list *ptl ;

  ptl = getptype( ptnode->USER, AWE_NODE_M );
  if( !ptl )
    return NULL ;
  return (awe_matrix_node*)ptl->DATA ;
}

/******************************************************************************\
build_sort_hybrid()
find all element constituing an hybrid matrix.
\******************************************************************************/
awe_hybrid_matrix* build_sort_hybrid( losig_list *losig, 
                                      awe_matrix_node *hybrid_node 
                                    )
{
  
  awe_matrix_node   *scan ;
  awe_hybrid_matrix *hmat ;
  awe_hybrid_matrix *head ;
  lowire_list       *wire ;
  lonode_list       *ptnode ;
  chain_list        *chwire ;
  chain_list        *chnode ;
  awe_hybrid_node   *hnode ;
 
  for( wire = losig->PRCN->PWIRE ; wire ; wire = wire->NEXT ) {
    RCN_CLEARFLAG( wire->FLAG, RCN_FLAG_PASS );
    ptnode = getlonode( losig, wire->NODE1 );
    RCN_CLEARFLAG( ptnode->FLAG, RCN_FLAG_PASS );
    ptnode = getlonode( losig, wire->NODE2 );
    RCN_CLEARFLAG( ptnode->FLAG, RCN_FLAG_PASS );
  }

  head = NULL ;

  for( scan = hybrid_node ; scan ; scan = scan->NEXT ) {
   
    if( RCN_GETFLAG( scan->LONODE->FLAG, RCN_FLAG_PASS ) )
      continue ;
    
    hmat = alloc_hybrid_matrix();
    hmat->DIM = 0;
    hmat->NEXT = head ;
    head = hmat ;
    
    set_hybrid_from_node( scan->LONODE, hmat, NO );
    RCN_SETFLAG( scan->LONODE->FLAG, RCN_FLAG_PASS );

    for( chwire = scan->LONODE->WIRELIST ; chwire ; chwire = chwire->NEXT ) {
      wire = (lowire_list*)chwire->DATA ;
      build_sort_hybrid_rec( losig, scan->LONODE, wire, hmat );
    }
  }

  /* fill all index with -1 value */
  
  for( hmat = head ; hmat ; hmat = hmat->NEXT ) {
    for( chnode = hmat->INTNODELIST ; chnode ; chnode = chnode->NEXT ) {
      ptnode = (lonode_list*)chnode->DATA ;
      hnode = get_hybrid_from_node( ptnode, hmat );
      hnode->INDEX = hmat->DIM ;
      (hmat->DIM)++;
    }
  }
  
  return head ;
}

/******************************************************************************\
build_fill_hybrid_list()
fill all hybrid matrix.
\******************************************************************************/
void build_fill_hybrid_list( losig_list *losig, 
                             awe_hybrid_matrix *hybrid_list,
                             double k
                           )
{
  awe_hybrid_matrix *scan ;

  avt_logenterfunction(LOGTRC,2,"build_fill_hybrid_list()");
  for( scan = hybrid_list ; scan ; scan = scan->NEXT ) {
    if( scan->MATRIX ) 
      mbk_FreeMatrix( scan->MATRIX );
    scan->MATRIX = mbk_CreateMatrix( scan->DIM, scan->DIM );
    build_fill_hybrid( losig, scan, k );
  }
  avt_logexitfunction(LOGTRC,2);
}

/******************************************************************************\
build_fill_hybrid()
fill an hybrid matrix.
\******************************************************************************/
void build_fill_hybrid( losig_list *losig, 
                        awe_hybrid_matrix *matrix,
                        double k
                      )
{
  chain_list      *chain ;
  lowire_list     *wire ;
  int              n1 ;
  int              n2 ;
  double           val ;
  lonode_list     *ptnode ;
  awe_hybrid_node *hnode ;

  avt_logenterfunction(LOGTRC,2,"build_fill_hybrid()");

  for( chain = matrix->WIRELIST ; chain ; chain = chain->NEXT ) {
  
    wire = (lowire_list*)chain->DATA ;
    if( wire->RESI <= AWE_MIN_RESI_VALUE )
      continue ;
    
    ptnode = awe_get_equiv_lonode(getlonode( losig, wire->NODE1 ));
    hnode = get_hybrid_from_node( ptnode, matrix );
    n1 = hnode->INDEX ;
    
    ptnode = awe_get_equiv_lonode(getlonode( losig, wire->NODE2 ));
    hnode = get_hybrid_from_node( ptnode, matrix );
    n2 = hnode->INDEX ;
    
    val = 1.0 / (wire->RESI/k) ;

    awe_add_admi( matrix->MATRIX, n1, n2, val );
  }

  avt_log(LOGTRC,2,"reduce maxtrix dimension\n");
  mbk_MatrixReduce( matrix->MATRIX, matrix->SDIM );
  avt_logexitfunction(LOGTRC,2);

}

/******************************************************************************\
build_hybrid_rec()
recursively found wire and node constituing an hybrid matrix.
\******************************************************************************/
void build_sort_hybrid_rec( losig_list        *losig, 
                            lonode_list       *from, 
                            lowire_list       *wire,
                            awe_hybrid_matrix *hmat
                          )
{
  awe_matrix_node *nodemat ;
  chain_list      *chwire ;
  lonode_list     *lonode ;
  char             encore ;

  if( RCN_GETFLAG( wire->FLAG, RCN_FLAG_PASS ) )
    return ;

  RCN_SETFLAG( wire->FLAG, RCN_FLAG_PASS );
  if( wire->RESI > AWE_MIN_RESI_VALUE )
    hmat->WIRELIST = addchain( hmat->WIRELIST, wire );

  if( wire->NODE1 == from->INDEX )
    lonode = getlonode( losig, wire->NODE2 );
  else
    lonode = getlonode( losig, wire->NODE1 );

  nodemat = get_matrix_node( awe_get_equiv_lonode( lonode ) ) ;
  encore='Y' ;
  if( nodemat ) {
    if( ( nodemat->FLAGS & AWE_FLAG_HYBRID ) == AWE_FLAG_HYBRID )
      set_hybrid_from_node( nodemat->LONODE, hmat, NO );
    else {
      set_hybrid_from_node( nodemat->LONODE, hmat, YES );
      encore='N' ;
    }
  }

  RCN_SETFLAG( lonode->FLAG, RCN_FLAG_PASS );

  if( encore == 'Y' ) {
    for( chwire = lonode->WIRELIST ; chwire ; chwire = chwire->NEXT ) {

      build_sort_hybrid_rec( losig, 
                             lonode, 
                             (lowire_list*)chwire->DATA, 
                             hmat 
                           );
    }
  }
}

/******************************************************************************\
set_hybrid_from_node()
external is YES or NO. If set to YES, zero or a positive index is created, else
-1 is used.
\******************************************************************************/
void set_hybrid_from_node( lonode_list       *lonode, 
                           awe_hybrid_matrix *hmat, 
                           char               external
                         )
{
  ptype_list      *ptl ;
  awe_hybrid_node *hn ;
  
  ptl = getptype( lonode->USER, AWE_NODE_H );
  if( !ptl ) {
    lonode->USER = addptype( lonode->USER, AWE_NODE_H, NULL );
    ptl = lonode->USER ;
  }
  
  for( hn = (awe_hybrid_node*)ptl->DATA ; hn ; hn = hn->NEXT )
    if( hn->HYBRID==hmat )
      return ;
  
  
  hn = AddHeapItem( &HEAP_AWE_HYBRID_NODE );
  hn->NEXT  = (awe_hybrid_node*)ptl->DATA ;
  ptl->DATA = hn ;
  hn->HYBRID = hmat ;
  if( external == YES ) {
    hmat->EXTNODELIST = addchain( hmat->EXTNODELIST, lonode );
    hn->INDEX = hmat->DIM ;
    (hmat->DIM)++;
    hmat->SDIM++ ;
  }
  else {
    hn->INDEX = -1 ;
    hmat->INTNODELIST = addchain( hmat->INTNODELIST, lonode );
  }
}

/******************************************************************************\
get_hybrid_from_node()
return the awe_hybrid_node structure associed to lonode corresponding to the
hmat awe_hybrid_matrix. If is not provided, return the head of the list.
\******************************************************************************/
awe_hybrid_node* get_hybrid_from_node( lonode_list *lonode, 
                                       awe_hybrid_matrix *hmat 
                                     )
{
  ptype_list       *ptl ;
  awe_hybrid_node  *hn=NULL ;
  
  ptl = getptype( lonode->USER, AWE_NODE_H );
  if( ptl ) {
    hn = (awe_hybrid_node*)ptl->DATA ;
    if( hmat ) {
      while( hn ) {
        if( hn->HYBRID == hmat ) {
          break ;
        }
        hn = hn->NEXT ;
      }
    }
  }

  return hn;
}

/******************************************************************************\
free_hybrid()
\******************************************************************************/
void free_hybrid( awe_hybrid_matrix *hlist)
{
  awe_hybrid_matrix *scan ;
  awe_hybrid_matrix *next ;
  chain_list        *chnode ;
  chain_list        *headnode[2] ;
  ptype_list        *ptl ;
  lonode_list       *ptnode ;
  int                n ;
  awe_hybrid_node   *hnode ;
  awe_hybrid_node   *hprev ;

  for( scan = hlist ; scan ; scan = next ) {
    next = scan->NEXT ;
    if( scan->MATRIX )
      mbk_FreeMatrix( scan->MATRIX );
    freechain( scan->WIRELIST );
    headnode[0] = scan->INTNODELIST ;
    headnode[1] = scan->EXTNODELIST ;
    for( n=0 ; n<=1 ; n++ ) {
      for( chnode = headnode[n] ; chnode ; chnode = chnode->NEXT ) {
        ptnode = (lonode_list*)chnode->DATA ;
        ptl = getptype( ptnode->USER, AWE_NODE_H );
        hprev = NULL ;
        for( hnode = (awe_hybrid_node*)ptl->DATA ; 
             hnode->HYBRID != scan ; 
             hnode = hnode->NEXT 
           )
          hprev = hnode ;
        if( hprev )
          hprev->NEXT = hnode->NEXT ;
        else
          ptl->DATA = hnode->NEXT ;
        DelHeapItem( &HEAP_AWE_HYBRID_NODE, hnode );
        if( !ptl->DATA )
          ptnode->USER = delptype( ptnode->USER, AWE_NODE_H );
      }
    }
    freechain( scan->INTNODELIST );
    freechain( scan->EXTNODELIST );
    DelHeapItem( &HEAP_AWE_HYBRID_MATRIX, scan );
  }
}

/******************************************************************************\
alloc_hybrid_matrix()
\******************************************************************************/
awe_hybrid_matrix* alloc_hybrid_matrix( void )
{
  awe_hybrid_matrix *pt ;

  pt = (awe_hybrid_matrix*)AddHeapItem( &HEAP_AWE_HYBRID_MATRIX );
  pt->NEXT = NULL ;
  pt->MATRIX = NULL ;
  pt->WIRELIST = NULL ;
  pt->INTNODELIST = NULL ;
  pt->EXTNODELIST = NULL ;
  pt->DIM = -1 ;
  pt->SDIM = 0 ;

  return pt ;
}

/******************************************************************************\
awe_add_admi()
add the admittance in matrix.
\******************************************************************************/
void awe_add_admi( mbk_matrix *matrix, int i, int j, double val )
{
  double prev ;

  /* impossible cases. just for debug only. */
  if( matrix->nbx != matrix->nby || 
      i < 0 || j < 0 || i >= matrix->nbx || j >= matrix->nbx ) {
    awe_error(10,AVT_FATAL);
  }

  /* beware to operation order for speed optimization */
  prev = mbk_GetMatrixValue( matrix, i, i );
  mbk_SetMatrixValue( matrix, i, i, prev + val );
 
  /* get not necessary. just for debug only */
  prev = mbk_GetMatrixValue( matrix, i, j );
  mbk_SetMatrixValue( matrix, i, j, prev - val );
  
  prev = mbk_GetMatrixValue( matrix, j, j );
  mbk_SetMatrixValue( matrix, j, j, prev + val );
 
  /* get not necessary. just for debug only */
  prev = mbk_GetMatrixValue( matrix, j, i );
  mbk_SetMatrixValue( matrix, j, i, prev - val );
}

/******************************************************************************\
build_awe()
Build awe moment.
If single_driver is different from 0, then it represent the single driver for
the net. Mainly for comparison with awe calculation without matrix.
\******************************************************************************/
void build_awe( losig_list        *losig, 
                locon_list        *driver,
                int                single_driver,
                awe_matrix_node   *valid_node,
                awe_hybrid_matrix *hybrid_list,
                double             extcapa,
                chain_list        *wirelist
              )
{
  mbk_matrix    *matu ;
  mbk_matrix    *matl ;
  double        *m0 ;
  double        *m[AWE_MAX_MOMENT] ;
  double        *t ;
  int            n, i ;
  double         k ;
  double         x ;

  avt_logenterfunction(LOGTRC,2,"build_awe()");

  /* build the A matrix */
  avt_log(LOGTRC,2,"build the A matrix\n" );
  build_fill_hybrid_list( losig, hybrid_list, 1.0 );
  matu = build_awe_matrix( losig, valid_node, hybrid_list, 1.0, wirelist );
  if( !matu )
    return ;

  /* find A=LU */
  avt_log(LOGTRC,2,"find a=lu\n");
  matl = mbk_CreateMatrix( matu->nbx, matu->nby );
  if( mbk_CreateLUMatrix( matu, matl, NULL ) == 1 ) {
    mbk_FreeMatrix( matu );
    mbk_FreeMatrix( matl );
    avt_log(LOGTRC,2,"an error occured\n");
    avt_logexitfunction(LOGTRC,2);
    return ;
  }
 
  avt_log(LOGTRC,2,"building vector\n");
  /* Storage for moment from 1 to n on all nodes. m[0] is m1.
     m[moment][node] */
  n = matu->nbx ;
  for( i=0 ; i<AWE_MAX_MOMENT ; i++ )
    m[i] = (double*)mbkalloc( sizeof( double ) * n );

  /* The solution vector m0 */
  m0 = (double*)mbkalloc( sizeof( double ) * n );
  for( i=0 ; i<n ; i++ )
    m0[i] = 1.0 ;
  
  /* Compute m1 */
  avt_log(LOGTRC,2,"compute m1\n");
  t    = (double*)mbkalloc( sizeof( double ) * n );
  mbk_MatrixSolveUsingArray( matl, m0, t );
  mbk_MatrixSolveUsingArray( matu, t, m[0] );
  
  #ifdef AWE_NORMALISE
    avt_log(LOGTRC,2,"normalisation\n");
    /* find the biggest moment at first order */
    k = fabs( m[0][0] );
    for( i=1 ; i<n ; i++ ) {
      x = fabs( m[0][i] );
      if( x > k )
        k = x;
    }
    avt_log(LOGTRC,2, "k=%g\n", k);
    /* restart all operations with normalisation */
    mbk_FreeMatrix( matu );
    
    avt_log(LOGTRC,2,"build the A matrix\n" );
    build_fill_hybrid_list( losig, hybrid_list, k );
    matu = build_awe_matrix( losig, valid_node, hybrid_list, k, wirelist );
    
    avt_log(LOGTRC,2,"find a=lu\n");
    if( mbk_CreateLUMatrix( matu, matl, NULL ) == 1 ) {
      mbk_FreeMatrix( matu );
      mbk_FreeMatrix( matl );
      avt_log(LOGTRC,2,"an error occured\n");
      avt_logexitfunction(LOGTRC,2);
      return ;
    }

    avt_log(LOGTRC,2,"compute m1\n");
    mbk_MatrixSolveUsingArray( matl, m0, t );
    mbk_MatrixSolveUsingArray( matu, t, m[0] );
    
  #else
  
    k = 1.0 ;

  #endif

  /* build moment from m1 to mn */
  avt_log(LOGTRC,2,"building moment\n");
  for( i=1 ; i<AWE_MAX_MOMENT ; i++ ) {
    mbk_MatrixSolveUsingArray( matl, m[i-1], t );
    mbk_MatrixSolveUsingArray( matu, t, m[i] );
  }

  avt_log(LOGTRC,2,"cleanning\n");
  mbkfree( t );
  mbkfree( m0 );
  mbk_FreeMatrix( matu );
  mbk_FreeMatrix( matl );

  avt_log(LOGTRC,2,"cleanning\n");
  awebuildinfomatrix( losig, driver, single_driver, k, extcapa, m );
  for( i=0 ; i<AWE_MAX_MOMENT ; i++ )
    mbkfree(m[i]);
 
  driver = NULL ;

  avt_logexitfunction(LOGTRC,2);
}

/******************************************************************************\
awebuildinfomatrix()
build final awe information on loads
\******************************************************************************/
void awebuildinfomatrix( losig_list   *losig, 
                         locon_list   *driver, 
                         int           single_driver,
                         double        k, 
                         double        extcapa, 
                         double      **tabmoment ) 
{
  chain_list      *scanlocon ;
  num_list        *scannode ;
  locon_list      *load;
  lonode_list     *ptnode;
  awe_matrix_node *mnode;
  double           m[AWE_MAX_MOMENT];
  int              i;

  for( scanlocon = (chain_list*)(getptype( losig->USER, LOFIGCHAIN )->DATA ) ;
       scanlocon ;
       scanlocon = scanlocon->NEXT
     ) {
    
    load = (locon_list*)(scanlocon->DATA);
    if( !rcx_isvalidlocon( load ) )
      continue;
    
    if( load != driver ) {
      
      for( scannode = load->PNODE ; scannode ; scannode = scannode->NEXT ) {

        ptnode = awe_get_equiv_lonode( getlonode( losig, scannode->DATA ) );
        
        // lorsqu'on a plusieurs composantes connexes, il est possible
        // de ne pas avoir de ptype construits
        mnode = get_matrix_node( ptnode );
       
        if( mnode ) {
        
          if( (mnode->FLAGS & AWE_FLAG_NORMAL) == AWE_FLAG_NORMAL ) {

            for( i=0 ; i<AWE_MAX_MOMENT ; i++ )
              m[i] = tabmoment[i][mnode->INDEX] ;
              
            aweaddnodeinfo( losig,
                            driver,
                            single_driver, 
                            load,
                            scannode->DATA,
                            m,
                            k,
                            extcapa
                          );
          }

          if( (mnode->FLAGS & AWE_FLAG_DRIVER) == AWE_FLAG_DRIVER ) {

            for( i=0 ; i<AWE_MAX_MOMENT ; i++ )
              m[i] = 0.0 ;
              
            aweaddnodeinfo( losig,
                            driver,
                            single_driver, 
                            load,
                            scannode->DATA,
                            m,
                            k,
                            extcapa
                          );
          }
          
          if( (mnode->FLAGS & AWE_FLAG_HYBRID) == AWE_FLAG_HYBRID ) {
            awe_buildinfomatrix_hybrid( losig,
                                        driver,
                                        single_driver,
                                        load,
                                        mnode,
                                        tabmoment,
                                        k,
                                        extcapa
                                      );
          }
        }
        else {

          for( i=0 ; i<AWE_MAX_MOMENT ; i++ )
            m[i] = 0.0 ;
              
          aweaddnodeinfo( losig,
                          driver,
                          single_driver, 
                          load,
                          scannode->DATA,
                          m,
                          k,
                          extcapa
                        );
        }
      }
    }
  }
  avt_logexitfunction(LOGTRC,2);
}

/******************************************************************************\
awe_buildinfomatrix_hybrid()
\******************************************************************************/
void awe_buildinfomatrix_hybrid( losig_list       *losig,
                                 locon_list       *driver,
                                 int               single_driver,
                                 locon_list       *load,
                                 awe_matrix_node  *mnode,
                                 double          **tabmoment,
                                 double            k,
                                 double            extcapa
                               )
{
  awe_hybrid_node   *hybrid ;
  mbk_matrix        *u, *l ;
  int                n ;
  chain_list        *scan ;
  lowire_list       *wire ;
  int                base ;
  double           **i ;
  double           **v ;
  double            *t ;
  int                j, p ;
  lonode_list       *lonode1 ;
  lonode_list       *lonode2 ;
  awe_hybrid_node   *hnode1 ;
  awe_hybrid_node   *hnode2 ;
  void              *ptmp ;
  double             m ;
  double             prev ;
  
  hybrid = get_hybrid_from_node( mnode->LONODE, NULL );
  if( !hybrid || hybrid->NEXT ) {
    awe_error(9,AVT_FATAL);
  }

  /* Par construction, les noeuds externes vont de 0 à base-1, et les noeuds
     internes ont des index au delà. Pour construire la matrice des moments des
     noeuds internes, il faut décaler leur noeuds internes de la valeur base. */
     
  for( scan = hybrid->HYBRID->INTNODELIST, n=0 ; 
       scan ; 
       scan = scan->NEXT, n++ );
  for( scan = hybrid->HYBRID->EXTNODELIST, base=0 ; 
       scan ; 
       scan = scan->NEXT, base++ );
  
  u  = mbk_CreateMatrix( n, n );
  l  = mbk_CreateMatrix( n, n );

  /* the solution vector */
  v = (double**)mbkalloc( sizeof( double* ) * AWE_MAX_MOMENT );
  for( j=0 ; j<AWE_MAX_MOMENT ; j++ ) {
    v[j] = (double*)mbkalloc( sizeof( double ) * n );
  }

  /* the excitation vector */
  i = (double**)mbkalloc( sizeof( double* ) * AWE_MAX_MOMENT );
  for( j=0 ; j<AWE_MAX_MOMENT ; j++ ) {
    i[j] = (double*)mbkalloc( sizeof( double ) * n );
    for( p=0 ; p<n ; p++ )
      i[j][p]=0.0 ;
  }

  /* an all usage temporary vector */
  if( n > AWE_MAX_MOMENT )
    t = (double*)mbkalloc( sizeof( double ) * n );
  else
    t = (double*)mbkalloc( sizeof( double ) * AWE_MAX_MOMENT );
  
  /* build the 'u' matrix */
  for( scan = hybrid->HYBRID->WIRELIST, n=0 ; scan ; scan = scan->NEXT, n++ ) {
  
    wire = (lowire_list*)scan->DATA ;
    if( wire->RESI <= AWE_MIN_RESI_VALUE )
      continue ;
    
    lonode1 = awe_get_equiv_lonode(getlonode( losig, wire->NODE1 ));
    hnode1  = get_hybrid_from_node( lonode1, hybrid->HYBRID );
    
    lonode2 = awe_get_equiv_lonode(getlonode( losig, wire->NODE2 ));
    hnode2  = get_hybrid_from_node( lonode2, hybrid->HYBRID );

    if( hnode1->INDEX >= base && hnode2->INDEX >= base )

      /* internal wire */
      awe_add_admi( u, hnode1->INDEX-base, hnode2->INDEX-base, 1.0/wire->RESI );
      
    else {
      
      /* wire on the boundary of the hybrid matrix */
      if( hnode2->INDEX < base ) {
        ptmp    = hnode1 ;
        hnode1  = hnode2 ;
        hnode2  = ptmp ;
        ptmp    = lonode1 ;
        lonode1 = lonode2 ;
        lonode2 = ptmp ;
      }
     
      /* lonode1 & hnode1 is node on the outside of hybrid matrix */
      mnode = get_matrix_node( lonode1 );

      for( j=0 ; j<AWE_MAX_MOMENT ; j++ ) {
      
        if( mnode->INDEX >= 0 )
          m = tabmoment[j][mnode->INDEX] / wire->RESI ;
        else
          m = 0.0 ;
          
        i[j][hnode2->INDEX-base] = i[j][hnode2->INDEX-base] + m ;
      }
      
      prev = mbk_GetMatrixValue( u, 
                                 hnode2->INDEX-base, 
                                 hnode2->INDEX-base
                               );
      mbk_SetMatrixValue( u, 
                          hnode2->INDEX-base, 
                          hnode2->INDEX-base, 
                          prev+1.0/wire->RESI 
                        );
    }
  }

  if( mbk_CreateLUMatrix( u ,l, NULL ) != 0 ) {
    avt_errmsg( TRC_ERRMSG, "052", AVT_WARNING, rcx_getsigname( losig ), getloconrcxname( driver ) );
  }

  /* build the solution for all moment */
  for( j=0 ; j<AWE_MAX_MOMENT ; j++ ) {
    mbk_MatrixSolveUsingArray( l, i[j], t );
    mbk_MatrixSolveUsingArray( u, t, v[j] );
  }

  /* and extract moment for wanted node */
  for( j=0 ; j<AWE_MAX_MOMENT ; j++ )
    t[j] = v[j][hybrid->INDEX-base] ;

  aweaddnodeinfo( losig, driver, single_driver, load, 
                  mnode->LONODE->INDEX, t, k, extcapa );

  /* clean */

  for( j=0 ; j<AWE_MAX_MOMENT ; j++ ) {
    mbkfree( i[j] );
    mbkfree( v[j] );
  }
  mbkfree( i );
  mbkfree( v );
  mbkfree( t );
  mbk_FreeMatrix( l );
  mbk_FreeMatrix( u );
}

/******************************************************************************\
build_awe_matrix()
build the awe matrix A=-C^-1.R
\******************************************************************************/
mbk_matrix* build_awe_matrix( losig_list *losig, 
                              awe_matrix_node *node_list, 
                              awe_hybrid_matrix *hybrid_list,
                              double k,
                              chain_list *wirelist
                            )
{
  mbk_matrix      *matrix_r ;
  double          *matrix_ic ;
  int              n ;
  int              n1 ;
  awe_matrix_node *node ;
  awe_matrix_node *awenode1 ;
  awe_matrix_node *awenode2 ;
  lowire_list     *wire ;
  lonode_list     *lonode1 ;
  lonode_list     *lonode2 ;
  lonode_list     *eqnode1 ;
  lonode_list     *eqnode2 ;
  double           val ;
  double           prev ;
  chain_list      *chainext1 ;
  chain_list      *chainext2 ;
  awe_hybrid_node *hnode1 ;
  awe_hybrid_node *hnode2 ;
  awe_hybrid_matrix *hybrid ;
  chain_list      *chwire ;

  avt_logenterfunction(LOGTRC,2, "build_awe_matrix()" );

  /* initialise matrix */
  avt_log(LOGTRC,2,"initialise\n");
  n=-1;
  for( node = node_list ; node ; node = node->NEXT ) {
    if( node->INDEX > n )
      n = node->INDEX ;
  }
  if( n==-1 )
    return NULL ;

  matrix_r  = mbk_CreateMatrix( n+1, n+1 );
  matrix_ic = (double*)mbkalloc( sizeof( double ) * ( n + 1 ) );
  for( n1=0 ; n1<=n ; n1++ )
    matrix_ic[n1] = 0.0 ;

  /* filling the matrix */
  avt_log(LOGTRC,2,"filling\n");
  for( node = node_list ; node ; node = node->NEXT ) {

    if( node->INDEX >= 0 )
      matrix_ic[ node->INDEX ] = -1.0 / node->CAPA ;
  }

  for( chwire = wirelist ; chwire ; chwire = chwire->NEXT ) {
    wire = (lowire_list*)chwire->DATA ;

    if( wire->RESI <= AWE_MIN_RESI_VALUE )
      continue ;

    lonode1 = getlonode( losig, wire->NODE1 );
    lonode2 = getlonode( losig, wire->NODE2 );

    eqnode1 = awe_get_equiv_lonode( lonode1 );
    eqnode2 = awe_get_equiv_lonode( lonode2 );

    awenode1 = get_matrix_node( eqnode1 );
    awenode2 = get_matrix_node( eqnode2 );

    val = 1.0 / ( wire->RESI / k );

    if( ( awenode1->FLAGS & AWE_FLAG_NORMAL ) == AWE_FLAG_NORMAL &&
        ( awenode2->FLAGS & AWE_FLAG_NORMAL ) == AWE_FLAG_NORMAL    ) {
      awe_add_admi( matrix_r, awenode1->INDEX, awenode2->INDEX, val );
    }

    if( ( awenode1->FLAGS & AWE_FLAG_NORMAL ) == AWE_FLAG_NORMAL &&
        ( awenode2->FLAGS & AWE_FLAG_DRIVER ) == AWE_FLAG_DRIVER    ) {
      prev = mbk_GetMatrixValue( matrix_r, awenode1->INDEX, awenode1->INDEX );
      mbk_SetMatrixValue( matrix_r, 
                          awenode1->INDEX, awenode1->INDEX, 
                          prev + val 
                        );
    }

    if( ( awenode1->FLAGS & AWE_FLAG_DRIVER ) == AWE_FLAG_DRIVER &&
        ( awenode2->FLAGS & AWE_FLAG_NORMAL ) == AWE_FLAG_NORMAL    ) {
      prev = mbk_GetMatrixValue( matrix_r, awenode2->INDEX, awenode2->INDEX );
      mbk_SetMatrixValue( matrix_r, 
                          awenode2->INDEX, awenode2->INDEX, 
                          prev + val 
                        );
    }
  }

  /* building hybrid value in matrix_r */
  avt_log(LOGTRC,2,"building hybrid values\n");
  for( hybrid = hybrid_list ; hybrid ; hybrid = hybrid->NEXT ) {
  
    for( chainext1 = hybrid->EXTNODELIST ; 
         chainext1 ; 
         chainext1 = chainext1->NEXT ) {
         
      lonode1  = (lonode_list*)chainext1->DATA ;
      hnode1   = get_hybrid_from_node( lonode1, hybrid );
      awenode1 = get_matrix_node( lonode1 );
      
      for( chainext2 = hybrid->EXTNODELIST ;
           chainext2 ; 
           chainext2 = chainext2->NEXT ) {
           
        lonode2  = (lonode_list*)chainext2->DATA ;
        hnode2   = get_hybrid_from_node( lonode2, hybrid );
        awenode2 = get_matrix_node( lonode2 );
       
        /* test if nodes on EXTNODELIST are not drivers */
        if( awenode2->INDEX >= 0 && awenode1->INDEX >= 0 ) {
          val = 
            mbk_GetMatrixValue( hybrid->MATRIX, hnode2->INDEX, hnode1->INDEX )
            + mbk_GetMatrixValue( matrix_r, awenode2->INDEX, awenode1->INDEX );

          mbk_SetMatrixValue( matrix_r, awenode2->INDEX, awenode1->INDEX, val );
        }
      }
    }
  }

  avt_log(LOGTRC,2,"matrix product\n");
  mbk_MatrixTraverse( matrix_r, 
                      (int (*)(int, int, double*, void*))awe_rc_product, 
                      (void*)matrix_ic 
                    );
  
  mbkfree( matrix_ic );

  avt_logexitfunction(LOGTRC,2);
  return matrix_r ;
}

int awe_rc_product( int x, int y, double *val, double *data )
{
  *val = *val * data[ y ] ;
  x=0;

  return 0;
}

/******************************************************************************\
debug function
\******************************************************************************/

void awe_display_hybrid( losig_list *losig, awe_hybrid_matrix *hybrid )
{
  chain_list      *chain ;
  lowire_list     *wire ;
  lonode_list     *lonode ;
  awe_hybrid_node *hnode ;

  printf( "WIRELIST :\n" );
  for( chain = hybrid->WIRELIST ; chain ; chain = chain->NEXT ) {
    wire = (lowire_list*)chain->DATA ;
    printf( "  r %ld(%ld) %ld(%ld) %f\n", 
            wire->NODE1,
            awe_get_equiv_lonode( getlonode( losig, wire->NODE1 ) )->INDEX,
            wire->NODE2,
            awe_get_equiv_lonode( getlonode( losig, wire->NODE2 ) )->INDEX,
            wire->RESI
          );
  }
  printf( "INTNODELIST :\n" );
  for( chain = hybrid->INTNODELIST ; chain ; chain = chain->NEXT ) {
    lonode = (lonode_list*)chain->DATA ;
    hnode  = get_hybrid_from_node( lonode, hybrid );
    printf( "  %ld(%ld) -> %d\n", lonode->INDEX, 
            awe_get_equiv_lonode( lonode )->INDEX,
            hnode->INDEX
          );
  }
  printf( "EXTNODELIST :\n" );
  for( chain = hybrid->EXTNODELIST ; chain ; chain = chain->NEXT ) {
    lonode = (lonode_list*)chain->DATA ;
    hnode  = get_hybrid_from_node( lonode, hybrid );
    printf( "  %ld(%ld) -> %d\n", lonode->INDEX, 
            awe_get_equiv_lonode( lonode )->INDEX,
            hnode->INDEX
          );
  }
  
}

#include AVT_H
#include MUT_H
#include "trc.h"

/* 
Ajoute un ptype RCXPLM_NODE sur tous les noeuds de la composante connexe
contenant un long int, numéroté depuis 0 sur les noeuds du driver, puis les
autres noeuds. Un numéro inférieur ou égal à 0 signifie que c'est un driver. 
Renvoie le nombre de noeuds numérotés.
*/
void rcxplm_build_node_index( losig_list         *losig,
                              wirefromlocon_list *group, 
                              num_list           *driver, 
                              long               *nbnode,
                              long               *nbdriver
                            )
{
  long         index ;
  num_list    *num ;
  ptype_list  *ptl ;
  lonode_list *node ;
  chain_list  *chain ;
  lowire_list *wire ;
  
  index = 0 ;

  for( num = driver ; num ; num = num->NEXT ) {
 
    node = getlonode( losig, num->DATA );
    ptl = getptype( node->USER, WIRECONNEXE );

    if( ptl && ptl->DATA == group ) {

      node = awe_get_equiv_lonode(node);

      if( !getptype( node->USER, RCXPLM_NODE ) ) {
        node->USER = addptype( node->USER, RCXPLM_NODE, (void*)(-index) );
        index++;
      }
    }
  }

  *nbdriver = index ;
  
  for( chain = group->WIRELIST ; chain ; chain = chain->NEXT ) {

    wire = (lowire_list*)chain->DATA ;
   
    if( wire->RESI <= AWE_MIN_RESI_VALUE )
      continue ;

    node = awe_get_equiv_lonode(getlonode( losig, wire->NODE1 ));
    if( !getptype( node->USER, RCXPLM_NODE ) ) 
      node->USER = addptype( node->USER, RCXPLM_NODE, (void*)index++ );
      
    node = awe_get_equiv_lonode(getlonode( losig, wire->NODE2 ));
    if( !getptype( node->USER, RCXPLM_NODE ) ) 
      node->USER = addptype( node->USER, RCXPLM_NODE, (void*)index++ );
  }

  *nbnode = index ;
}

/*
Libère les numéros créés par la fonction rcxplm_build_node_index().
*/
void rcxplm_free_node_index( losig_list *losig, wirefromlocon_list *group )
{
  chain_list  *chain ;
  lowire_list *wire ;
  lonode_list *node ;

  for( chain = group->WIRELIST ; chain ; chain = chain->NEXT ) {

    wire = (lowire_list*)chain->DATA ;
    if( wire->RESI <= AWE_MIN_RESI_VALUE )
      continue ;
    
    node = awe_get_equiv_lonode(getlonode( losig, wire->NODE1 ));
    if( getptype( node->USER, RCXPLM_NODE ) ) 
      node->USER = delptype( node->USER, RCXPLM_NODE );
      
    node = awe_get_equiv_lonode(getlonode( losig, wire->NODE2 ));
    if( getptype( node->USER, RCXPLM_NODE ) ) 
      node->USER = delptype( node->USER, RCXPLM_NODE );
  }
 
}

/*
Crée les matrices.
*/
void rcxplm_fill_matrix( losig_list         *losig,
                         mbk_matrix         *r,
                         double             *c,
                         int                 nbnode,
                         wirefromlocon_list *group,
                         RCXFLOAT            extcapa,
                         rcx_slope          *slope,
                         char                type,
                         char                coefctc
                       )
{
  chain_list  *chain ;
  lowire_list *wire ;
  lonode_list *ptnode1 ;
  lonode_list *ptnode2 ;
  long         n1 ;
  long         n2 ;
  double       prev ;
  double       val ;
  double       capa ;
  char        *tab ;

  for( chain = group->WIRELIST ; chain ; chain = chain->NEXT ) {
  
    wire   = (lowire_list*)chain->DATA ;
    if( wire->RESI <= AWE_MIN_RESI_VALUE )
      continue ;

    val    = 1.0 / wire->RESI ;

    ptnode1 = awe_get_equiv_lonode(getlonode( losig, wire->NODE1 )) ;
    n1     = (long)getptype( ptnode1->USER, RCXPLM_NODE )->DATA ;

    ptnode2 = awe_get_equiv_lonode(getlonode( losig, wire->NODE2 )) ;
    n2     = (long)getptype( ptnode2->USER, RCXPLM_NODE )->DATA ;

    if( n1>0 && n2>0 ) {

      /* wire non connecté directement sur un driver */
      
      prev = mbk_GetMatrixValue( r, n1, n1 );
      mbk_SetMatrixValue( r, n1, n1, prev-val );
      
      prev = mbk_GetMatrixValue( r, n2, n2 );
      mbk_SetMatrixValue( r, n2, n2, prev-val );
      
      prev = mbk_GetMatrixValue( r, n1, n2 );
      mbk_SetMatrixValue( r, n1, n2, prev+val );
      
      prev = mbk_GetMatrixValue( r, n2, n1 );
      mbk_SetMatrixValue( r, n2, n1, prev+val );

    }
    else {
     
      if( n1>0 && n2<=0 ) {

        mbk_SetMatrixValue( r, -n2, -n2, 1.0 );

        prev = mbk_GetMatrixValue( r, n1, -n2 );
        mbk_SetMatrixValue( r, n1, -n2, prev+val );

        prev = mbk_GetMatrixValue( r, n1, n1 );
        mbk_SetMatrixValue( r, n1, n1, prev-val );
      }
      
      if( n1<=0 && n2>0 ) {

        mbk_SetMatrixValue( r, -n1, -n1, 1.0 );

        prev = mbk_GetMatrixValue( r, n2, -n1 );
        mbk_SetMatrixValue( r, n2, -n1, prev+val );

        prev = mbk_GetMatrixValue( r, n2, n2 );
        mbk_SetMatrixValue( r, n2, n2, prev-val );
      }

      if( n1<=0 && n2<=0 ) {
        mbk_SetMatrixValue( r, -n1, -n1, 1.0 );
        mbk_SetMatrixValue( r, -n2, -n2, 1.0 );
      }

    }
  }

  for( n1=0 ; n1<nbnode ; n1++ )
    c[n1] = 0.0 ;

  tab = (char*)mbkalloc( sizeof( char ) * losig->PRCN->NBNODE );
  for( n1 = 0 ; n1 < losig->PRCN->NBNODE ; n1++ )
    tab[n1] = 0 ;
  
  for( chain = group->WIRELIST ; chain ; chain = chain->NEXT ) {
  
    wire   = (lowire_list*)chain->DATA ;
    
    ptnode1 = getlonode( losig, wire->NODE1 );
    ptnode2 = getlonode( losig, wire->NODE2 );

    if( !tab[ptnode1->INDEX-1] ) {
      tab[ptnode1->INDEX-1] = 1 ;
      capa = rcx_getnodecapa( losig, ptnode1, extcapa, slope, type, TRC_HALF, coefctc );
      ptnode1 = awe_get_equiv_lonode( ptnode1 );
      n1 = (long)getptype( ptnode1->USER, RCXPLM_NODE )->DATA ;
      if( n1<0 ) n1=-n1 ;
      c[n1]=c[n1]+ capa ;
    }

    if( !tab[ptnode2->INDEX-1] ) {
      tab[ptnode2->INDEX-1] = 1 ;
      capa = rcx_getnodecapa( losig, ptnode2, extcapa, slope, type, TRC_HALF, coefctc );
      ptnode2 = awe_get_equiv_lonode( ptnode2 );
      n2 = (long)getptype( ptnode2->USER, RCXPLM_NODE )->DATA ;
      if( n2<0 ) n2=-n2 ;
      c[n2]=c[n2]+ capa ;
    }
  }

  mbkfree( tab );
}

/*
Crée le vecteur solution pour y2 et plus .
*/
void rcxplm_create_s( long nbnode, 
                      long nbdriver, 
                      double *c, 
                      double *u, 
                      double *s 
                    )
{
  int i;
  
  for( i=0 ; i<nbdriver ; i++ ) 
    s[i] = 0.0 ;
  for( i=nbdriver ; i<nbnode ; i++ ) 
    s[i] = c[i]*u[i];
}

void rcxplm_get_y( long    nbdriver, 
                   double *x1, 
                   double *x2, 
                   double *x3, 
                   double *yg1, 
                   double *yg2, 
                   double *yg3 
                 )
{
  int i;
  
  *yg1 = 0.0 ;
  for( i=0 ; i<nbdriver ; i++ ) 
    *yg1 = *yg1 + x1[i] ;
  
  *yg2 = 0.0 ;
  for( i=0 ; i<nbdriver ; i++ ) 
    *yg2 = *yg2 + x2[i] ;
  
  *yg3 = 0.0 ;
  for( i=0 ; i<nbdriver ; i++ ) 
    *yg3 = *yg3 + x3[i] ;
}

/* 
Calcule l'admittance équivalente au 3° ordre d'un réseau RC.
*/
void rcx_piload_matrix( losig_list *losig, 
                        num_list   *driver, 
                        RCXFLOAT   *y1, 
                        RCXFLOAT   *y2, 
                        RCXFLOAT   *y3, 
                        RCXFLOAT    extcapa, 
                        rcx_slope  *slope, 
                        char        type, 
                        RCXFLOAT    coefctc 
                       )
{
  wirefromlocon_list *connexe ;
  wirefromlocon_list *group ;
  long                nbnode ;
  long                nbdriver ;
  mbk_matrix         *r ;
  double             *c ;
  mbk_matrix         *matl ;
  double             *x1 ;
  double             *x2 ;
  double             *x3 ;
  double             *s ;
  double             *x ;
  double              yg1 ;
  double              yg2 ;
  double              yg3 ;
  
  *y1 = 0.0 ;
  *y2 = 0.0 ;
  *y3 = 0.0 ;
  
  connexe = rcx_get_wire_connexe( losig, driver );

  for( group = connexe ; group ; group = group->NEXT ) {
  
    if( !group->WIRELIST )
      continue ;

    awe_build_equiv_lonode( losig, group->WIRELIST );

    rcxplm_build_node_index( losig, group, driver, &nbnode, &nbdriver );

    r = mbk_CreateMatrix( nbnode, nbnode );
    c = (double*)mbkalloc( sizeof( double ) * nbnode );

    rcxplm_fill_matrix( losig, 
                        r, c, 
                        nbnode, 
                        group, 
                        extcapa, 
                        slope, 
                        type, 
                        coefctc 
                      );

    matl = mbk_CreateMatrix( r->nbx, r->nby );
    
    if( mbk_CreateLUMatrix( r, matl, NULL ) != 1 ) {

      x  = (double*)mbkalloc( sizeof( double ) * nbnode );
      s  = (double*)mbkalloc( sizeof( double ) * nbnode );
      x1 = (double*)mbkalloc( sizeof( double ) * nbnode );
      x2 = (double*)mbkalloc( sizeof( double ) * nbnode );
      x3 = (double*)mbkalloc( sizeof( double ) * nbnode );
      
      mbk_MatrixSolveUsingArray( matl, c, x );
      mbk_MatrixSolveUsingArray( r, x, x1 );

      rcxplm_create_s( nbnode, nbdriver, c, x1, s );

      mbk_MatrixSolveUsingArray( matl, s, x );
      mbk_MatrixSolveUsingArray( r, x, x2 );
      
      rcxplm_create_s( nbnode, nbdriver, c, x2, s );

      mbk_MatrixSolveUsingArray( matl, s, x );
      mbk_MatrixSolveUsingArray( r, x, x3 );

      mbkfree( x );
      mbkfree( s );

      rcxplm_get_y( nbdriver, x1, x2, x3, &yg1, &yg2, &yg3 );
      
      mbkfree( x1 );
      mbkfree( x2 );
      mbkfree( x3 );

      *y1 = *y1 + yg1 ;
      *y2 = *y2 + yg2 ;
      *y3 = *y3 + yg3 ;
    }
    
    mbk_FreeMatrix( r );
    mbk_FreeMatrix( matl );
    mbkfree( c );

    rcxplm_free_node_index( losig, group );
    awe_clean_equiv_lonode( losig, group->WIRELIST );
  }

  rcx_free_wire_connexe( losig, driver, connexe );
}

#include "tas.h"

void fillloconsize( locon_list *locon, sizelocon *size )
{
  num_list   *pnode ;
  ptype_list *ptl ;
  char        found ;
  chain_list *chain ;
  
  size->NB++ ;
  size->SIZE = size->SIZE + sizeof( locon_list );
  size->SIZENAME = size->SIZENAME + (strlen( locon->NAME )+1 )*sizeof( char );

  for( pnode = locon->PNODE ; pnode ; pnode = pnode->NEXT ) {
    size->SIZE = size->SIZE + sizeof( num_list );
  }

  for( ptl = locon->USER ; ptl ; ptl = ptl->NEXT ) {
  
    size->SIZEPTYPE = size->SIZEPTYPE + sizeof( ptype_list );
  
    found = 0 ;
    
    if( ptl->TYPE == RCXNAME ) {
      size->SIZERCXNAME = size->SIZERCXNAME + (strlen( (char*)ptl->DATA )+1 )*sizeof( char );
      found = 1 ;
    }

    if( ptl->TYPE == PNODENAME ) {
      for( chain = (chain_list*)ptl->DATA ; chain ; chain = chain->NEXT ) {
        size->SIZEPNODENAME = size->SIZEPNODENAME + sizeof( chain_list ) + (strlen( (char*)chain->DATA )+1 )*sizeof( char );
      }
      found = 1 ;
    }

    if( ptl->TYPE == RCX_LOCONCAPA_SPACE )
      found = 1 ;

    if( !found )
      printf( "unknown ptype 0x%X / %lu for locon %s\n", (unsigned)ptl->TYPE, ptl->TYPE, locon->NAME );
  }
  
}

void filllotrssize( lotrs_list *lotrs, sizelotrs *size )
{
  ptype_list *ptl ;
  int         found=0;
  chain_list *chain ;
  
  if( !lotrs )
    return ;

  size->NB++ ;
  size->SIZE = size->SIZE + sizeof( lotrs_list );
  if( lotrs->GRID )   fillloconsize( lotrs->GRID,   &size->LOCON );
  if( lotrs->SOURCE ) fillloconsize( lotrs->SOURCE, &size->LOCON );
  if( lotrs->DRAIN )  fillloconsize( lotrs->DRAIN,  &size->LOCON );
  if( lotrs->BULK )   fillloconsize( lotrs->BULK,   &size->LOCON );
  if( lotrs->TRNAME ) 
    size->SIZENAME = size->SIZENAME + (strlen(lotrs->TRNAME)+1)*sizeof(char) ;

  for( ptl = lotrs->USER ; ptl ; ptl = ptl->NEXT ) {
  
    size->SIZEPTYPE = size->SIZEPTYPE + sizeof( ptype_list );

    if( ptl->TYPE == MBK_TRANS_PARALLEL ||
        ptl->TYPE == TAS_TRANS_LINK        ) {
      for( chain = (chain_list*)ptl->DATA ; chain ; chain = chain->NEXT ) 
        size->SIZE = size->SIZE + sizeof( chain_list );
      found = 1 ;
    }

    if( ptl->TYPE == CNS_INDEX        ||
        ptl->TYPE == ELP_LOTRS_MODEL  ||
        ptl->TYPE == CNS_DRIVINGCONE  ||
        ptl->TYPE == TAS_TRANS_USED   ||
        ptl->TYPE == TAS_CAPA_USED    ||
        ptl->TYPE == TAS_TRANS_SWITCH    )
      found = 1 ;
    
    if( !found )
      printf( "unknown ptype 0x%X / %lu for lotrs %s\n", (unsigned)ptl->TYPE, ptl->TYPE, lotrs->TRNAME );
  }
}

void fillcnssize( cnsfig_list *cns, sizecns *size )
{
  lotrs_list *lotrs ;

  for( lotrs = cns->LOTRS ; lotrs ; lotrs = lotrs->NEXT ) {
    filllotrssize( lotrs, &size->SIZELOTRS );
  }
}

void displaycnssize( sizecns *size )
{
  size_t sizelotrs ;
  size_t sizelotrslocon ;

  sizelotrslocon = size->SIZELOTRS.LOCON.SIZE +
                   size->SIZELOTRS.LOCON.SIZENAME +
                   size->SIZELOTRS.LOCON.SIZERCXNAME +
                   size->SIZELOTRS.LOCON.SIZEPTYPE +
                   size->SIZELOTRS.LOCON.SIZEPNODENAME ;
  sizelotrs = size->SIZELOTRS.SIZE + 
              size->SIZELOTRS.SIZENAME + 
              size->SIZELOTRS.SIZEPTYPE +
              sizelotrslocon ;

  printf( "lotrs : %10lu\n", (unsigned long)sizelotrs );
  printf( "  lotrs       %6d -> %10lu\n", size->SIZELOTRS.NB, (unsigned long)size->SIZELOTRS.SIZE );
  printf( "    name                %10lu\n", (unsigned long)size->SIZELOTRS.SIZENAME );
  printf( "    ptype               %10lu\n", (unsigned long)size->SIZELOTRS.SIZEPTYPE );
  printf( "  locon                 %10lu\n", (unsigned long)sizelotrslocon );
  printf( "    locon     %6d -> %10lu\n", size->SIZELOTRS.LOCON.NB, (unsigned long)size->SIZELOTRS.LOCON.SIZE );
  printf( "    name                %10lu\n", (unsigned long)size->SIZELOTRS.LOCON.SIZENAME );
  printf( "    rcxname             %10lu\n", (unsigned long)size->SIZELOTRS.LOCON.SIZERCXNAME );
  printf( "    ptype               %10lu\n", (unsigned long)size->SIZELOTRS.LOCON.SIZEPTYPE );
  printf( "    pnodename           %10lu\n", (unsigned long)size->SIZELOTRS.LOCON.SIZEPNODENAME );
}

void tas_displaymemoryusage( cnsfig_list *cnsfig )
{
  cone_list   *cone ;
  edge_list   *incone ;
  ptype_list  *ptl ;
  branch_list *tabbranch[3] ;
  branch_list *branch ;
  locon_list  *tablocon[2] ;
  locon_list  *locon ;
  int          i ;
  link_list   *link ;
  sizecns      size ;

  int         nbdelay = 0 ;
  int         nbfront = 0 ;
  int         nbcaraclink = 0 ;
  int         nbcaraccon = 0 ;
  int         nbptypecone = 0 ;
  int         nbptypeedge = 0 ;
  int         nbptypelink = 0 ;
  int         nbptypelocon = 0 ;
  int         nbptype = 0 ;
  int         sizedelay = 0 ;
  int         sizefront = 0 ;
  int         sizecaraclink = 0 ;
  int         sizecaraccon = 0 ;
  int         sizeptype = 0 ;
  int         total ;

  if( !cnsfig )
    return ;

  size.SIZELOTRS.SIZE                = 0 ;
  size.SIZELOTRS.NB                  = 0 ;
  size.SIZELOTRS.SIZENAME            = 0 ;
  size.SIZELOTRS.SIZEPTYPE           = 0 ;
  size.SIZELOTRS.LOCON.SIZE          = 0 ;
  size.SIZELOTRS.LOCON.NB            = 0 ;
  size.SIZELOTRS.LOCON.SIZEPTYPE     = 0 ;
  size.SIZELOTRS.LOCON.SIZENAME      = 0 ;
  size.SIZELOTRS.LOCON.SIZERCXNAME   = 0 ;
  size.SIZELOTRS.LOCON.SIZEPNODENAME = 0 ;

  fillcnssize( cnsfig, &size );
  displaycnssize( &size );


  for( cone = cnsfig->CONE ; cone ; cone = cone->NEXT ) {

    for( ptl = cone->USER ; ptl ; ptl = ptl->NEXT ) {
    
      nbptypecone++ ;

      if( ptl->TYPE == TAS_SLOPE_MAX )
        nbfront++ ;

      if( ptl->TYPE == TAS_SLOPE_MIN )
        nbfront++ ;
    }
    
    for( incone = cone->INCONE ; incone ; incone = incone->NEXT ) {

    
      for( ptl = incone->USER ; ptl ; ptl = ptl->NEXT ) {

        nbptypeedge++ ;

        if( ptl->TYPE == TAS_DELAY_MAX )
          nbdelay++ ;

        if( ptl->TYPE == TAS_DELAY_MIN )
          nbdelay++ ;
      }
    }

    tabbranch[0]=cone->BREXT ;
    tabbranch[1]=cone->BRVDD ;
    tabbranch[2]=cone->BRVSS ;

    for( i=0 ; i<=2 ; i++ ) {

      for( branch = tabbranch[i] ; branch ; branch = branch->NEXT ) {

        for( link = branch->LINK ; link ; link = link->NEXT ) {

          for( ptl = link->USER ; ptl ; ptl = ptl->NEXT ) {
            nbptypelink++ ;

            if( ptl->TYPE == TAS_LINK_CARAC )
              nbcaraclink++ ;
          }
        }
      }
    }
  }

  tablocon[0] = cnsfig->LOCON ;
  tablocon[1] = cnsfig->INTCON ;

  for( i=0 ; i<=1 ; i++ ) {
  
    for( locon = tablocon[i] ; locon ; locon = locon->NEXT ) {
    
      for( ptl = locon->USER ; ptl ; ptl = ptl->NEXT ) {
     
        nbptypelocon++ ;

        if( ptl->TYPE == TAS_CON_CARAC )
          nbcaraccon++ ;

        if( ptl->TYPE == TAS_SLOPE_MIN )
          nbcaraccon++ ;

        if( ptl->TYPE == TAS_SLOPE_MAX )
          nbcaraccon++ ;

        if( ptl->TYPE == TAS_DELAY_MAX ) 
          nbcaraccon++ ;

        if( ptl->TYPE == TAS_DELAY_MIN )
          nbcaraccon++ ;
      }
    }
  }

  sizedelay     = nbdelay     * sizeof( delay_list ) ;
  sizefront     = nbfront     * sizeof( front_list ) ;
  sizecaraclink = nbcaraclink * sizeof( caraclink_list ) ;
  sizecaraccon  = nbcaraccon  * sizeof( caraccon_list ) ;
  nbptype       = nbptypecone + nbptypeedge + nbptypelink + nbptypelocon ;
  sizeptype     = nbptype     * sizeof( ptype_list ) ;
  
  total = sizedelay + sizefront + sizecaraclink + sizecaraccon + sizeptype ;

  printf( "structure delay_list     : %6d %10d\n", nbdelay,     sizedelay     );
  printf( "structure front_list     : %6d %10d\n", nbfront,     sizefront     );
  printf( "structure caraclink_list : %6d %10d\n", nbcaraclink, sizecaraclink );
  printf( "structure caraccon_list  : %6d %10d\n", nbcaraccon,  sizecaraccon  );
  printf( "ptype cone               : %6d\n", nbptypecone  );
  printf( "ptype edge               : %6d\n", nbptypeedge  );
  printf( "ptype link               : %6d\n", nbptypelink  );
  printf( "ptype locon              : %6d\n", nbptypelocon );
  printf( "structure ptype          : %6d %10d\n", nbptype,   sizeptype );
  printf( "                   total : %10d\n", total );

}

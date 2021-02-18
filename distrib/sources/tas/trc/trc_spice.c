#include "trc.h"

int rcx_spidrivenet( rcxfile *file, lofig_list *lofig, losig_list *losig )
{
  rcx_list      *rcx;
  num_list      *node;
  int            idx;
  lowire_list   *wire;
  loctc_list    *ctc;
  chain_list    *scan;
  chain_list    *internal;
  float          c;
  int            n;

  rcx = getrcx( losig );
  if( !rcx )
    return 0;
  
  rcn_lock_signal( lofig, losig );
 
  rcx_file_print( file, "* Connecteur order on subckt\n" );
  
  for( internal = rcx->RCXEXTERNAL ; internal ; internal = internal->NEXT )
    for( node=((locon_list*)internal->DATA)->PNODE ; node ; node=node->NEXT )
      rcx_file_print( file, "*  %s\n", getloconrcxname((locon_list*)internal->DATA));
      
  for( internal = rcx->RCXINTERNAL ; internal ; internal = internal->NEXT ) {
    for( node=((locon_list*)internal->DATA)->PNODE ; node ; node=node->NEXT )
      rcx_file_print( file, "*  %s\n", getloconrcxname((locon_list*)internal->DATA) );
  }

  rcx_file_print( file, ".subckt %s ", rcx_getsigname( losig ) );
  
  for( internal = rcx->RCXEXTERNAL ; internal ; internal = internal->NEXT )
    for( node=((locon_list*)internal->DATA)->PNODE ; node ; node=node->NEXT )
      rcx_file_print( file, "%ld ", node->DATA );
      
  for( internal = rcx->RCXINTERNAL ; internal ; internal = internal->NEXT ) {
    for( node=((locon_list*)internal->DATA)->PNODE ; node ; node=node->NEXT )
      rcx_file_print( file, "%ld ", node->DATA );
  }

  rcx_file_print( file, "\n" );

  idx=0;
  for( wire = losig->PRCN->PWIRE ; wire ; wire = wire->NEXT ) {
    idx++;
    rcx_file_print( file, "R%d %ld %ld %g\n", idx,
                                              wire->NODE1,
                                              wire->NODE2,
                                              wire->RESI
                  );
    if( wire->CAPA > 0.0 ) {
      rcx_file_print( file, "Ca%d %ld 0 %gpF\n", idx,
                                                 wire->NODE1,
                                                 wire->CAPA/2.0
                    );
      rcx_file_print( file, "Cb%d %ld 0 %gpF\n", idx,
                                                 wire->NODE2,
                                                 wire->CAPA/2.0
                    );
    }
  }
  
  idx=0;
  for( scan = losig->PRCN->PCTC ; scan ; scan = scan->NEXT ) {
    ctc = (loctc_list*)scan->DATA;
    if( rcn_ctcnode( ctc, losig ) ) {
      idx++;
      rcx_file_print( file, "C%d %ld 0 %gpF\n", idx,
                                                rcn_ctcnode( ctc, losig ),
                                                ctc->CAPA
                    );
    }                      
  }

  idx=0;
  for( internal = rcx->RCXEXTERNAL ; internal ; internal = internal->NEXT ) {
    c = rcx_getloconcapa( ((locon_list*)internal->DATA), TRC_SLOPE_UNK, TRC_CAPA_NOM, TRC_HALF, NULL );
    if( c > 0.0 ) {
      for( node=((locon_list*)internal->DATA)->PNODE, n=0 ; node ; node=node->NEXT, n++ );
      for( node =((locon_list*)internal->DATA)->PNODE ; node ; node = node->NEXT ) {
        idx++;
        rcx_file_print( file, "Cx%d %ld 0 %gpF\n", idx,
                                                   node->DATA,
                                                   c/n
                      );
      }
    }
  }
      
  for( internal = rcx->RCXINTERNAL ; internal ; internal = internal->NEXT ) {
    c = rcx_getloconcapa( ((locon_list*)internal->DATA), TRC_SLOPE_UNK, TRC_CAPA_NOM, TRC_HALF, NULL );
    if( c>0.0 ) {
      for( node = ((locon_list*)internal->DATA)->PNODE, n=0 ; 
           node ; 
           node = node->NEXT, n++ );
      for( node = ((locon_list*)internal->DATA)->PNODE ; 
           node ; 
           node = node->NEXT ) {
        idx++;
        rcx_file_print( file, "Cx%d %ld 0 %gpF\n", idx,
                                                   node->DATA,
                                                   c/n
                      );
      }
    }
  }

  rcx_file_print( file, ".ends\n\n" );

  rcn_unlock_signal( lofig, losig );
  return 1;
}

/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TRC Version 1.01                                            */
/*    Fichier : trc_rcx.c                                                   */
/*                                                                          */
/*    (c) copyright 1997-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Gregoire AVOT                                             */
/*                                                                          */
/****************************************************************************/

/* CVS informations :

Revision : $Revision: 1.17 $
Author   : $Author: gregoire $
Date     : $Date: 2005/06/01 16:52:56 $

*/

#include "trc.h"

unsigned long int rcx_cache_parse( lofig_list *lofig, losig_list *losig )
{
  int nbligne = 0;
  unsigned long int l;
  
  l = rcxparsesignalparasitics( NULL,
                            rcx_getlofigname( lofig ),
                            &nbligne, 
                            lofig,
                            NULL,
                            losig,
                            NULL,
                            rcx_getvss( lofig ),
                            rcx_getvssni( lofig ),
                            RCXPARSE_CACHE,
                            NULL
                          ) ;

  return l;
}

/*******************************************************************************
* rcx_cache_set|isset_lofig()                                                  *
********************************************************************************
* Positionne ou récupère le mode d'une lofig : avec ou sans cache.             *
*******************************************************************************/
void rcx_set_cache_lofig( lofig_list *lofig, 
                          char *filename,
                          char *extname,
                         unsigned long int (*fn_load)(lofig_list*, losig_list*),
                         void (*fn_free)(lofig_list*)
                        )
{
  static int pool=-1;
  FILE *ptf;
  
  if( RCX_USECACHE == 'Y' ) {
    // On vérifie si on peut faire de l'accès direct sur le fichier avant
    // d'activer le cache.
    ptf = mbkfopen( filename, extname, "r" );
    if( ptf ) {
      if( MBKFOPEN_FILTER == YES ) {
        if( RCN_CACHE_SIZE > 0 )
          avt_errmsg( TRC_ERRMSG, "003", AVT_WARNING, MBKFOPEN_NAME );
      }
      else {
        if( pool==-1 )
          pool=rcn_getnewpoolcache();
        rcn_enable_cache( lofig, fn_load, fn_free, pool );
      }
      fclose( ptf );
    }
  }
}

int rcx_isset_cache_lofig( lofig_list *lofig )
{
  if( rcn_getlofigcache( lofig ) )
    return 1;
  return 0;
}

FILE* rcx_cache_getlofigfile( lofig_list *lofig )
{
  int file ;
  ptype_list *ptl;
  ptl = getptype( lofig->USER, RCX_FILEPOS );
  if( ptl ) {
    file = (int)(long)ptl->DATA ;
    return mbk_cache_get_file( file );
  }
  return NULL;
}

void rcx_cache_setlofigfile( lofig_list *lofig, FILE *ptf, char *filename )
{
  ptype_list *ptl;
  int         file ;
  int         prevfile ;

  file = mbk_cache_set_file( ptf, filename, RCXFILEEXTENTION );
  ptl = getptype( lofig->USER, RCX_FILEPOS );
  if( ptl ) {
    prevfile = (int)(long)ptl->DATA ;
    mbk_cache_clear_file( prevfile );
    ptl->DATA = (void*)(long)file ;
  }
  else
    lofig->USER = addptype( lofig->USER, RCX_FILEPOS, (void*)(long)file );
}

void rcx_cache_clearlofigfile( lofig_list *lofig )
{
  ptype_list *ptl;
  int         file ;
  ptl = getptype( lofig->USER, RCX_FILEPOS );
  if( ptl ) {
    file = (int)(long)ptl->DATA ;
    lofig->USER = delptype( lofig->USER, RCX_FILEPOS );
    mbk_cache_clear_file( file );
    
  }
}

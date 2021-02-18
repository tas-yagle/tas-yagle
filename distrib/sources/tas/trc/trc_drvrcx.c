#include "trc.h"

rcxfile* rcx_init_driver( lofig_list *lofig, int iscache )
{
  time_t         counter;
  rcxfile       *file;
  char           buffer[1024];

  file = rcx_file_alloc();
 
  // Stratégie de compression / cache : si le cache est actif, il ne faut pas
  // compresser le fichier.
 
  if( iscache ) {
    file->ISCACHE = 1 ;
    if( OUT_FILTER )
      avt_errmsg( TRC_ERRMSG, "051", AVT_WARNING, lofig->NAME );
    file->FD = mbkfopen_ext( lofig->NAME, RCXFILEEXTENTION, "w", 0, 0 );
  }
  else {
    file->ISCACHE = 0 ;
    file->FD = mbkfopen_ext( lofig->NAME, RCXFILEEXTENTION, "w", 0, 1 );
  }

  if( !file->FD ) {
    avt_errmsg( TRC_ERRMSG, "011", AVT_FATAL, lofig->NAME, RCXFILEEXTENTION );
  }

  file->FILENAME = namealloc( MBKFOPEN_NAME );

  time(&counter);
  
  rcx_file_print( file, "RCX VERSION 5\n\n" );
  
  sprintf(buffer,"RCX data for instance %s\n",lofig->NAME);
  avt_printExecInfoCustom(file, "#", buffer, "",
                          (void(*)(void*,...))rcx_file_print);
                        
  return file;
}

void rcx_drive_end( rcxfile *file, lofig_list *lofig )
{
  losig_list *scansig;
  
  for( scansig = lofig->LOSIG ; scansig ; scansig = scansig->NEXT )
    rcxprintbellow( file, scansig );
  rcx_file_print( file, "\n" );

  rcxprintinstance( file, lofig );
  rcx_file_print( file, "\n" );
  
  rcx_file_print( file, "EOF\n" );

  fclose( file->FD );
  mbkfree( file );
}

void rcxprintinstance( rcxfile *file, lofig_list *lofig )
{
  loins_list *scanloins;
  for( scanloins = lofig->LOINS ; scanloins ; scanloins = scanloins->NEXT ) {
    if( rcxneeddriveloins( scanloins ) ) { 
      rcx_file_print( file, "INSTANCE %s MODEL %s\n", rcx_getinsname(scanloins),
                                                      scanloins->FIGNAME
                    );
    }
  }
  rcx_file_print( file, "\n" );
}

void rcx_file_print( rcxfile *file, ... ) 
{
  va_list  arg;
  char    *fmt;

  va_start( arg, file );
  fmt = va_arg( arg, char* );
  if( vfprintf( file->FD, fmt, arg ) < 0 ) {
    avt_errmsg( TRC_ERRMSG, "012", AVT_FATAL, file->FILENAME );
  }
}

rcxfile* rcx_file_alloc( void )
{
  rcxfile *file;
  file = mbkalloc( sizeof( rcxfile ) );
  file->FD = NULL;
  file->FILENAME = NULL;

  return file;
}

/******************************************************************************\
rcx_driver_sort_losig()

Renvoie une liste chainée des signaux à driver dans le fichier RCX dans cet
ordre. Lorsque le champs DATA est nul, il faut appeller la fonction 
rcx_end_external().
\******************************************************************************/

chain_list* rcx_driver_sort_losig( lofig_list *lofig )
{
  chain_list *head=NULL;
  losig_list *scanlosig;
  locon_list *scanlocon;

  for( scanlocon = lofig->LOCON ; scanlocon ; scanlocon = scanlocon->NEXT )
   scanlocon->SIG->FLAGS=0;

  for( scanlocon = lofig->LOCON ; scanlocon ; scanlocon = scanlocon->NEXT )
  {
    if (scanlocon->SIG->FLAGS==0)
      head = addchain( head, scanlocon->SIG );
    scanlocon->SIG->FLAGS=1;
  }
  
  head = addchain( head, NULL );
  
  for( scanlosig = lofig->LOSIG ; scanlosig ; scanlosig = scanlosig->NEXT ) {
    if( scanlosig->TYPE == 'I' ) 
      head = addchain( head, scanlosig );
  }

  return reverse(head);
}

void rcx_end_external( rcxfile *file )
{
  rcx_file_print( file, "ENDEXTERNAL\n\n" );
}

void rcx_drive_signal_end( rcxfile *file, losig_list *losig )
{
  rcx_file_print( file, "\n" );
  losig=NULL; //unused
}

void rcx_drive_signal_header( rcxfile *file, losig_list *losig, rcx_list *rcx )
{
  char *signame;
  char  buffer[1024], buf1[1024];
  chain_list *cl;

  if( GETFLAG( rcx->FLAG, SAVESIG ) ) {

    signame = rcx_getsigname( losig );

    strcpy( buffer, signame);
    rcx_vect( buffer );
    
    rcx_file_print( file, "SIGNAL %s", buffer);    
    for (cl=rcx->RCXEXTERNAL; cl!=NULL; cl=cl->NEXT)
    {
      strcpy(buf1, ((locon_list *)cl->DATA)->NAME);
      rcx_vect( buf1 );
      if (strcmp(buffer, buf1)!=0) rcx_file_print( file, " +%s", buf1);
    }
    rcx_file_print( file, rcx_islosigexternal( losig ) ? " EXTERNAL":" INTERNAL");
                  
    if( GETFLAG( rcx->FLAG, MODELRCN     ) ) rcx_file_print( file, " RCN" );
    if( GETFLAG( rcx->FLAG, RCXBREAKLOOP ) ) rcx_file_print( file, " B" );
    if( GETFLAG( rcx->FLAG, RCXNOCTC     ) ) rcx_file_print( file, " G" );
    if( GETFLAG( rcx->FLAG, RCXNOWIRE    ) ) rcx_file_print( file, " W" );
    if( GETFLAG( rcx->FLAG, RCXIGNORE    ) ) rcx_file_print( file, " I" );
    rcx_file_print( file, "\n" );
  }
}

void rcx_drive_begin_net( rcxfile *file )
{
  rcx_file_print( file, "beginparasitics\n" );
}

void rcx_drive_end_net( rcxfile *file )
{
  rcx_file_print( file, "endparasitics\n" );
}

void rcx_drive_wire( rcxfile *file, int n1, int n2, float r, float c )
{
  rcx_file_print( file, "W %d %d %g %g\n", n1, n2, r, c );
}

void rcx_drive_ground_capa( rcxfile *file, int n, float c )
{
  rcx_file_print( file, "K %d %c %g\n", n, RCXFILECAR_GND, c );
}

void rcx_drive_ctcni_capa( rcxfile *file, int n, float c )
{
  rcx_file_print( file, "K %d %c %g\n", n, RCXFILECAR_NI, c );
}

void rcx_drive_ctc_capa( rcxfile *file, 
                         int      n, 
                         float    c, 
                         char    *agrname, 
                         int      nodeagr 
                       )
{
  char buffer[1024];
  strcpy( buffer, agrname );
  rcx_vect( buffer );
 
  rcx_file_print( file, "K %d %s %d %g\n", n, buffer, nodeagr, c );
}

void rcxprintorigin( rcxfile *file, losig_list *losig )
{
  char buffer[1024];
  chain_list *scan;

  for( scan = rcx_getoriginlist(losig) ; scan ; scan = scan->NEXT ) {
    strcpy( buffer, (char*)(scan->DATA) );
    rcx_vect( buffer );
    rcx_file_print( file, "ORIGIN %s\n", buffer );
  }
}

void rcxprintbellow( rcxfile *file, losig_list *losig )
{
  char buffer1[1024];
  char buffer2[1024];
  char *refname;

  refname = rcx_isbellow( losig );

  if( !refname )
    return;
  
  strcpy( buffer1, rcx_getsigname( losig ) );
  rcx_vect( buffer1 );
  strcpy( buffer2, refname );
  rcx_vect( buffer2 );
  rcx_file_print( file, "BELLOW %s %s\n", buffer1, buffer2 );
}

void rcxprintlocon( rcxfile *file, rcx_list *rcxdata )
{
  ht          *nomlocon;
  locon_list  *testlocon;
  locon_list  *locon;
  char        *conname;
  char         type;
  char         buffer[1024];
  num_list    *n1, *n2;
  chain_list  *scanchain;
  int          i, n, j ;
  chain_list **sortlocon=NULL;
  float        *tabcapa;
  int          index=1;
  int          prevpos ;
  char         increase_n ;
  ptype_list  *ptl ;
  num_list    *headup, *headdn ;
  
  /* Note : Toute la difficulté est de prendre en compte les locons qui ont le
     meme getloconrcxname() mais qui ne sont physiquement pas les meme. Dans ce
     cas on ne garde qu'un seul de ces locons. Ceux éliminés sont renommés 
     pour conserver les capacités.
  */

  tabcapa = alloca(sizeof(float)*32);

  n=countchain(rcxdata->RCXINTERNAL)+countchain(rcxdata->RCXEXTERNAL)+1;
  sortlocon = (chain_list**)alloca( sizeof( chain_list* ) * n );
  nomlocon  = addht(50);

  /* Les éléments du tableau sortlocon sont des chain_list de locon ayant
     le meme getloconrcxname(). A partir d'un getloconrcxname(), on retrouve
     l'index dans le tableau sortlocon avec la table de hash nomlocon. */

  n=0;
  if( rcxdata->RCXEXTERNAL ) {
    for (scanchain=rcxdata->RCXEXTERNAL; scanchain; scanchain=scanchain->NEXT)
    {
      sortlocon[n] = addchain( NULL, scanchain->DATA );
      conname = getloconrcxname( (locon_list *)scanchain->DATA );
      addhtitem( nomlocon, conname, n );
      n++;
    }
  }

  for( scanchain = rcxdata->RCXINTERNAL ; 
       scanchain ; 
       scanchain = scanchain->NEXT 
     ) {
     
    locon = (locon_list*)scanchain->DATA;
    if( !locon->PNODE ) continue;
    conname = getloconrcxname( locon );

    prevpos = gethtitem( nomlocon, conname );

    if( prevpos == EMPTYHT ) { 
      sortlocon[n] = addchain( NULL, locon );
      addhtitem( nomlocon, conname, n );
      n++ ;
    }
    else {
      sortlocon[prevpos] = addchain( sortlocon[prevpos], locon );
    }
  }

  delht( nomlocon ) ; nomlocon = NULL ;

  /* Pour chaque liste dans le tableau sortlocon, vérifie et déplace les
     locon qui ne sont pas physiquement identiques. Le nombre 'n' d'éléments 
     dans le tableau peut être augmenté, et les listes contenir des champs 
     DATA inutilisés.
  */

  for( i=0 ; i<n ; i++ ) {
  
    if( ! sortlocon[i]->NEXT )
      /* Rien a faire : il n'y a qu'un seul locon avec ce nom */
      continue ;

    locon = (locon_list*)sortlocon[i]->DATA ;

    /* Elimine les connecteurs physiques différents */
    sortlocon[n] = NULL ;
    increase_n = 'N' ;
    for( scanchain = sortlocon[i]->NEXT ; 
         scanchain ; 
         scanchain = scanchain->NEXT 
       ) {
       
      testlocon = (locon_list*)scanchain->DATA ;
      
      for( n1 = locon->PNODE, n2 = testlocon->PNODE ;
           n1 && n2 && n1->DATA == n2->DATA ;
           n1 = n1->NEXT, n2 = n2->NEXT ) ;
           
      if( n1 || n2 ) {
        sortlocon[n] = addchain( sortlocon[n], testlocon );
        scanchain->DATA = NULL ;
        increase_n = 'Y' ;
        sprintf( buffer, "rcx_duplicate_#%d_%s", 
                 index++, getloconrcxname( locon ) 
               );
        ptl = getptype( testlocon->USER, RCX_NAME_FOR_DRIVE );
        if( ptl ) 
          ptl->DATA = namealloc( buffer );
        else
          testlocon->USER = addptype( testlocon->USER, 
                                      RCX_NAME_FOR_DRIVE, 
                                      namealloc( buffer ) 
                                    );
      }
    }

    if( increase_n == 'Y' )
      n++;
  }

  /* Calcule les capacités et drive les connecteurs */
  for( i=0 ; i<n ; i++ ) {
  
    for( j=0 ; j<32 ; j++ ) tabcapa[j]=0.0 ;

    for( scanchain = sortlocon[i] ; scanchain ; scanchain = scanchain->NEXT ) {
      locon = scanchain->DATA ;
      if( !locon )
        continue ;
        
      switch( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE ) {
      
      case ELP_CAPA_LEVEL0 :
      
        tabcapa[RCX_CAPA_NOM_MAX] = tabcapa[RCX_CAPA_NOM_MAX] +
                   rcx_getloconcapa( locon, TRC_SLOPE_UNK, TRC_CAPA_NOM_MAX, TRC_HALF, NULL );
        tabcapa[RCX_CAPA_NOMF_MAX] = tabcapa[RCX_CAPA_NOMF_MAX] +
                   rcx_getloconcapa( locon, TRC_SLOPE_UNK, TRC_CAPA_NOM_MAX, TRC_END, NULL );

        if( RCX_CAPA_NOM_MIN != RCX_CAPA_NOM_MAX ) {
          tabcapa[RCX_CAPA_NOM_MIN] = tabcapa[RCX_CAPA_NOM_MIN] +
                     rcx_getloconcapa( locon, TRC_SLOPE_UNK, TRC_CAPA_NOM_MIN, TRC_HALF, NULL );
          tabcapa[RCX_CAPA_NOMF_MIN] = tabcapa[RCX_CAPA_NOMF_MIN] +
                     rcx_getloconcapa( locon, TRC_SLOPE_UNK, TRC_CAPA_NOM_MIN, TRC_END, NULL );
        }
                     
        break ;
        
      case ELP_CAPA_LEVEL1 :
      
        tabcapa[RCX_CAPA_UP_NOM_MAX] = tabcapa[RCX_CAPA_UP_NOM_MAX] + 
               rcx_getloconcapa( locon, TRC_SLOPE_UP, TRC_CAPA_NOM_MAX, TRC_HALF, NULL );
        tabcapa[RCX_CAPA_UP_NOMF_MAX] = tabcapa[RCX_CAPA_UP_NOMF_MAX] + 
               rcx_getloconcapa( locon, TRC_SLOPE_UP, TRC_CAPA_NOM_MAX, TRC_END, NULL );

        if( RCX_CAPA_UP_NOMF_MAX != RCX_CAPA_UP_NOMF_MIN ) {
          tabcapa[RCX_CAPA_UP_NOM_MIN] = tabcapa[RCX_CAPA_UP_NOM_MIN] + 
                 rcx_getloconcapa( locon, TRC_SLOPE_UP, TRC_CAPA_NOM_MIN, TRC_HALF, NULL );
          tabcapa[RCX_CAPA_UP_NOMF_MIN] = tabcapa[RCX_CAPA_UP_NOMF_MIN] + 
                 rcx_getloconcapa( locon, TRC_SLOPE_UP, TRC_CAPA_NOM_MIN, TRC_END, NULL );
        }
                 
        tabcapa[RCX_CAPA_DW_NOM_MAX] = tabcapa[RCX_CAPA_DW_NOM_MAX] +
               rcx_getloconcapa( locon, TRC_SLOPE_DOWN, TRC_CAPA_NOM_MAX, TRC_HALF, NULL );
        tabcapa[RCX_CAPA_DW_NOMF_MAX] = tabcapa[RCX_CAPA_DW_NOMF_MAX] +
               rcx_getloconcapa( locon, TRC_SLOPE_DOWN, TRC_CAPA_NOM_MAX, TRC_END, NULL );

        if( RCX_CAPA_DW_NOM_MAX != RCX_CAPA_DW_NOM_MIN ) {
          tabcapa[RCX_CAPA_DW_NOM_MIN] = tabcapa[RCX_CAPA_DW_NOM_MIN] +
                 rcx_getloconcapa( locon, TRC_SLOPE_DOWN, TRC_CAPA_NOM_MIN, TRC_HALF, NULL );
          tabcapa[RCX_CAPA_DW_NOMF_MIN] = tabcapa[RCX_CAPA_DW_NOMF_MIN] +
                 rcx_getloconcapa( locon, TRC_SLOPE_DOWN, TRC_CAPA_NOM_MIN, TRC_END, NULL );
        }
               
        break ;
        
      case ELP_CAPA_LEVEL2 :
      
        tabcapa[RCX_CAPA_UP_MIN] = tabcapa[RCX_CAPA_UP_MIN] +
                    rcx_getloconcapa( locon, TRC_SLOPE_UP, TRC_CAPA_MIN, TRC_HALF, NULL );
        tabcapa[RCX_CAPA_UP_MAX] = tabcapa[RCX_CAPA_UP_MAX] +
                    rcx_getloconcapa( locon, TRC_SLOPE_UP, TRC_CAPA_MAX, TRC_HALF, NULL );
        tabcapa[RCX_CAPA_DW_MIN] = tabcapa[RCX_CAPA_DW_MIN] +
                    rcx_getloconcapa( locon, TRC_SLOPE_DOWN, TRC_CAPA_MIN, TRC_HALF, NULL );
        tabcapa[RCX_CAPA_DW_MAX] = tabcapa[RCX_CAPA_DW_MAX] +
                    rcx_getloconcapa( locon, TRC_SLOPE_DOWN, TRC_CAPA_MAX, TRC_HALF, NULL );
                    
        tabcapa[RCX_CAPA_UP_NOM_MAX] = tabcapa[RCX_CAPA_UP_NOM_MAX] +
                    rcx_getloconcapa( locon, TRC_SLOPE_UP, TRC_CAPA_NOM_MAX, TRC_HALF, NULL );
        tabcapa[RCX_CAPA_UP_NOMF_MAX] = tabcapa[RCX_CAPA_UP_NOMF_MAX] +
                    rcx_getloconcapa( locon, TRC_SLOPE_UP, TRC_CAPA_NOM_MAX, TRC_END, NULL );

        if( RCX_CAPA_UP_NOM_MAX != RCX_CAPA_UP_NOM_MIN ) {
          tabcapa[RCX_CAPA_UP_NOM_MIN] = tabcapa[RCX_CAPA_UP_NOM_MIN] +
                      rcx_getloconcapa( locon, TRC_SLOPE_UP, TRC_CAPA_NOM_MIN, TRC_HALF, NULL );
          tabcapa[RCX_CAPA_UP_NOMF_MIN] = tabcapa[RCX_CAPA_UP_NOMF_MIN] +
                      rcx_getloconcapa( locon, TRC_SLOPE_UP, TRC_CAPA_NOM_MIN, TRC_END, NULL );
        }
                    
        tabcapa[RCX_CAPA_DW_NOM_MAX] = tabcapa[RCX_CAPA_DW_NOM_MAX] +
                    rcx_getloconcapa( locon, TRC_SLOPE_DOWN, TRC_CAPA_NOM_MAX, TRC_HALF, NULL );
        tabcapa[RCX_CAPA_DW_NOMF_MAX] = tabcapa[RCX_CAPA_DW_NOMF_MAX] +
                    rcx_getloconcapa( locon, TRC_SLOPE_DOWN, TRC_CAPA_NOM_MAX, TRC_END, NULL );

        if( RCX_CAPA_DW_NOM_MAX != RCX_CAPA_DW_NOM_MIN ) {
          tabcapa[RCX_CAPA_DW_NOM_MIN] = tabcapa[RCX_CAPA_DW_NOM_MIN] +
                      rcx_getloconcapa( locon, TRC_SLOPE_DOWN, TRC_CAPA_NOM_MIN, TRC_HALF, NULL );
          tabcapa[RCX_CAPA_DW_NOMF_MIN] = tabcapa[RCX_CAPA_DW_NOMF_MIN] +
                      rcx_getloconcapa( locon, TRC_SLOPE_DOWN, TRC_CAPA_NOM_MIN, TRC_END, NULL );
        }
                      
        break ;
      }
    }
 
    locon = (locon_list*)sortlocon[i]->DATA ;
    type = trc_getlocondir(locon) ;
    ptl = getptype( locon->USER, RCX_NAME_FOR_DRIVE );
    if( ptl ) 
      conname = (char*)ptl->DATA ;
    else
      conname = getloconrcxname( locon );
    strcpy( buffer, conname );
    rcx_vect( buffer );

    rcx_file_print( file,
                    "C %s %c",
                    buffer, 
                    type 
                  );

    switch( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE ) {
    case ELP_CAPA_LEVEL0 : 
      if( tabcapa[RCX_CAPA_NOM_MIN] != tabcapa[RCX_CAPA_NOM_MAX] )
        rcx_file_print( file, " %g %g %g %g", 
                        tabcapa[RCX_CAPA_NOM_MIN], 
                        tabcapa[RCX_CAPA_NOM_MAX],
                        tabcapa[RCX_CAPA_NOMF_MIN], 
                        tabcapa[RCX_CAPA_NOMF_MAX]
                      );
      else
        rcx_file_print( file, " %g %g", 
                        tabcapa[RCX_CAPA_NOM_MAX],
                        tabcapa[RCX_CAPA_NOMF_MAX]
                      );
      break ;
    case ELP_CAPA_LEVEL1 : 
      if( tabcapa[RCX_CAPA_UP_NOM_MIN] != tabcapa[RCX_CAPA_UP_NOM_MAX] ||
          tabcapa[RCX_CAPA_DW_NOM_MIN] != tabcapa[RCX_CAPA_DW_NOM_MAX]    )
        rcx_file_print( file, " %g %g %g %g %g %g %g %g", 
                        tabcapa[RCX_CAPA_UP_NOM_MIN], tabcapa[RCX_CAPA_UP_NOM_MAX], 
                        tabcapa[RCX_CAPA_DW_NOM_MIN], tabcapa[RCX_CAPA_DW_NOM_MAX],
                        tabcapa[RCX_CAPA_UP_NOMF_MIN], tabcapa[RCX_CAPA_UP_NOMF_MAX], 
                        tabcapa[RCX_CAPA_DW_NOMF_MIN], tabcapa[RCX_CAPA_DW_NOMF_MAX] 
                      );
      else
        rcx_file_print( file, " %g %g %g %g", 
                        tabcapa[RCX_CAPA_UP_NOM_MAX], 
                        tabcapa[RCX_CAPA_DW_NOM_MAX],
                        tabcapa[RCX_CAPA_UP_NOMF_MAX], 
                        tabcapa[RCX_CAPA_DW_NOMF_MAX]
                      );
      break ;
    case ELP_CAPA_LEVEL2 :
      if( tabcapa[RCX_CAPA_UP_NOM_MIN] != tabcapa[RCX_CAPA_UP_NOM_MAX] ||
          tabcapa[RCX_CAPA_DW_NOM_MIN] != tabcapa[RCX_CAPA_DW_NOM_MAX]    ) 
        rcx_file_print( file, " %g %g %g %g %g %g %g %g %g %g %g %g", 
                        tabcapa[RCX_CAPA_UP_MIN],
                        tabcapa[RCX_CAPA_UP_NOM_MIN], tabcapa[RCX_CAPA_UP_NOM_MAX], 
                        tabcapa[RCX_CAPA_UP_MAX],
                        tabcapa[RCX_CAPA_DW_MIN],
                        tabcapa[RCX_CAPA_DW_NOM_MIN], tabcapa[RCX_CAPA_DW_NOM_MAX],
                        tabcapa[RCX_CAPA_DW_MAX],
                        tabcapa[RCX_CAPA_UP_NOMF_MIN], tabcapa[RCX_CAPA_UP_NOMF_MAX], 
                        tabcapa[RCX_CAPA_DW_NOMF_MIN], tabcapa[RCX_CAPA_DW_NOMF_MAX]
                      );
      else
        rcx_file_print( file, " %g %g %g %g %g %g %g %g", 
                        tabcapa[RCX_CAPA_UP_MIN],
                        tabcapa[RCX_CAPA_UP_NOM_MAX], 
                        tabcapa[RCX_CAPA_UP_MAX],
                        tabcapa[RCX_CAPA_DW_MIN],
                        tabcapa[RCX_CAPA_DW_NOM_MAX],
                        tabcapa[RCX_CAPA_DW_MAX],
                        tabcapa[RCX_CAPA_UP_NOMF_MAX], 
                        tabcapa[RCX_CAPA_DW_NOMF_MAX]
                      );
      break ;
    }

    headup = rcx_getnodebytransition( locon, TRC_SLOPE_UP );
    headdn = rcx_getnodebytransition( locon, TRC_SLOPE_DOWN );

    rcx_file_print( file, " : " );
    for( n1 = locon->PNODE ; n1 ; n1 = n1->NEXT )
      rcx_file_print( file ,"%ld ", n1->DATA );
    if( headup != locon->PNODE ) {
      rcx_file_print( file, " : " );
      for( n1 = headup ; n1 ; n1 = n1->NEXT )
        rcx_file_print( file ,"%ld ", n1->DATA );
      rcx_file_print( file, " : " );
      for( n1 = headdn ; n1 ; n1 = n1->NEXT )
        rcx_file_print( file ,"%ld ", n1->DATA );
    }
    rcx_file_print( file, "\n" );

    /* Libération des chain courantes */
    for( scanchain = sortlocon[i] ; scanchain ; scanchain = scanchain->NEXT ) {
      locon = scanchain->DATA ;
      if( !locon )
        continue ;
      ptl = getptype( locon->USER, RCX_NAME_FOR_DRIVE );
      if( ptl ) 
        locon->USER = delptype( locon->USER, RCX_NAME_FOR_DRIVE );
    }
    freechain( sortlocon[i] ) ; sortlocon[i]=NULL ;
  }

  if( rcxdata->DRIVERSUP ) {
    rcx_file_print( file, "driverup " );
    for( n1 = rcxdata->DRIVERSUP->PNODE ; n1 ; n1 = n1->NEXT ) 
      rcx_file_print( file, "%ld ", n1->DATA );
    rcx_file_print( file, "\n" );
  }
  if( rcxdata->DRIVERSDW ) {
    rcx_file_print( file, "driverdown " );
    for( n1 = rcxdata->DRIVERSDW->PNODE ; n1 ; n1 = n1->NEXT ) 
      rcx_file_print( file, "%ld ", n1->DATA );
    rcx_file_print( file, "\n" );
  }
}

int rcxneeddriveloins( loins_list *loins )
{
  locon_list    *scanlocon;
  for( scanlocon = loins->LOCON ; scanlocon ; scanlocon = scanlocon->NEXT )
    if( getrcx( scanlocon->SIG ) )
      break;
  if( scanlocon ) return 1;
  return 0;
}

void rcx_vect( s )
char *s;
{
  int i,p1;

  if( s == NULL )
    return;
  if( s[0] == '\0' )
    return;

 
  /* Positionne i sur le premier caractere non espace a la fin de la chaine */
  i = strlen( s ) ;
  do
    i--;
  while( s[i] == ' ' && i >0 );

  /* passe un eventuel paquet de nombres */
  if( i )
  {
    p1 = i;
    while( isdigit( (int)s[i] ) && i >0 )
      i--;
    if( p1 != i && s[i] == ' ' )
    {
      s[i]    = '[';
      s[p1+1] = ']';
      s[p1+2] = '\0' ;
    }
  }
}

char trc_getlocondir(locon)
locon_list *locon ;
{
 switch(locon->DIRECTION)
  {
   case IN : return IN;
   case OUT : return OUT;
   case INOUT : return INOUT;
   case UNKNOWN : return UNKNOWN;
   case TRISTATE : return TRISTATE;
   case TRANSCV : return TRANSCV;
   default : return UNKNOWN;
  }
}

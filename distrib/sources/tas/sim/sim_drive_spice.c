/******************************************************************************/
/*                                                                            */
/*                      Chaine de CAO & VLSI   AVERTEC                        */
/*                                                                            */
/*    Produit : SIM Version 2.00                                              */
/*    Fichier : sim_drive_spice.c                                             */
/*                                                                            */
/*    (c) copyright 2000 AVERTEC                                              */
/*    Tous droits reserves                                                    */
/*                                                                            */
/*    Auteur(s) : Grégoire AVOT                                               */
/*                                                                            */
/******************************************************************************/

#include SIM_H
#include MSL_H
#include "sim.h"

/******************************************************************************\

Fonction de haut niveau pour driver et simuler un modèle de simulation.

\******************************************************************************/

void sim_run_simu_drive_header(FILE *file, drive_spice *context)
{
  sim_model  *model;
  char        quote;
  chain_list *techno;
  char       *subcktfilename;
  int         tool;

  model = context->MODEL;
  subcktfilename = context->SUBCKTFILENAME ;

  spi_print( file, "*** SIM library ***\n\n\n" );

  fprintf(file,".OPTION TNOM=%g\n", V_FLOAT_TAB[__SIM_TNOM].VALUE);

  switch (( tool = sim_parameter_get_tool( model ) )) {
    case SIM_TOOL_LTSPICE:
      spi_print( file, ".SAVE V(*)\n\n" );
      break;
  }
  spi_print( file, ".TEMP %g\n\n", sim_parameter_get_temperature( model ) );
  quote = '\"' ;

  for( techno = sim_parameter_get_techno_file_head( model ) ;
       techno ;
       techno = techno->NEXT 
     )
    spi_print( file, ".include %c%s%c\n", quote, (char*)(techno->DATA), quote );
  spi_print( file, "\n\n" );

  if (context->OUTPUT_SPICE_FILE_POINTER==NULL)
  {
    char *cur, *c, *buf=mbkstrdup(subcktfilename);
    
    cur=strtok_r(buf, ",", &c);
    while (cur!=NULL)
    {
      spi_print( file, ".include %c%s%c\n", quote, cur, quote );
      cur=strtok_r(NULL, ",", &c);
    }
    spi_print( file, "\n" );
    mbkfree(buf);
  }
}

static void sim_extractlabel(char *filename, sim_model *model)
{
  if (model->NBMC>0)
    {
      switch( sim_parameter_get_tool( model ) ) {
      case SIM_TOOL_ELDO: 
      case SIM_TOOL_TITAN:
      case SIM_TOOL_TITAN7:
      case SIM_TOOL_HSPICE: 
        mfe_fileextractlabel(filename,model->LRULES,model->LLABELS,model->NBMC);
        break;
      case SIM_TOOL_LTSPICE:
      case SIM_TOOL_NGSPICE:
      case SIM_TOOL_SPICE:
        mfe_read_montecarlo_ltspice(filename, model->LLABELS);
        break;
      }
    } 
  else
    {
      mfe_fileextractlabel(filename,model->LRULES,model->LLABELS,model->NBMC);
    }
}

char sim_run_simu_spice( sim_model *model, int run, char *outputname, FILE *outputfile)
{
  drive_spice    context;
  char          *outfile, buf[1024];

  context.MODEL      = model;
  context.DRIVEDFILE = NULL;
  context.CURRENT    = NULL;
  context.NBCURRENT  = 1;
  context.NBPINMEAS  = 1;
  context.OUTPUT_SPICE_FILE_NAME=outputname;
  context.OUTPUT_SPICE_FILE_POINTER=outputfile;
          
  if(run!=SIM_RUN_EXEC)
    sim_run_simu_spice_setenv( &context );

  sim_run_simu_spice_create_cmdfilename( &context );
    
  if (run & SIM_RUN_DRIVE)
  {
    sim_measure_detail_clean( model );

    if( !sim_run_simu_spice_drive_subckt( &context ) ) {
      sim_run_simu_spice_restoreenv( &context );
      return SIM_SIMU_CANTDRIVEFILE;
    }

    /* Greg le 14/10/03
    sim_parameter_set_allow_overwrite_files( model, SIM_YES);
    */
    
    if( !sim_run_simu_spice_drive_cmd( &context )) {
      sim_run_simu_spice_restoreenv( &context );
      return SIM_SIMU_CANTDRIVEFILE;
    }
  }

  /* Greg le 14/10/03
  SIM_SPICESTRING = sim_parameter_get_tool_cmd( model );
  SIM_SPICESTDOUT = sim_parameter_get_stdoutput_file( model );
  */

  if( run & (SIM_RUN_EXEC|SIM_RUN_READ)) {
    if (run & (SIM_RUN_EXEC))
    {
//      getcwd( context.CURDIR, 10000 ); // pas thread-safe chdir
//      chdir( WORK_LIB );
      if( !sim_execspice( context.CMDFILENAME, 
                        (run & SIM_RUN_MULTI)?2:0, 
                        SIM_SPICENAME, 
                        sim_parameter_get_tool_cmd( model ), 
                        sim_parameter_get_stdoutput_file( model ) 
                      ) ) {
        if(run!=SIM_RUN_EXEC)
        {
          sim_run_simu_spice_drive_remove_files( &context );
          sim_run_simu_spice_restoreenv( &context );
        }
        return SIM_SIMU_CANTRUN;
      }
//      chdir( context.CURDIR );
    }
    if (run & SIM_RUN_READ)
    {
      sim_set_output_file( model, context.CMDFILENAME );
      outfile = sim_get_output_file(model);

      outfile   = sim_getjoker(SIM_SPICEOUT,outfile);
      sprintf(buf,"%s/%s",WORK_LIB,outfile);
      mbkfree(outfile);
      sim_extractlabel(buf, model);

      sim_run_simu_spice_drive_remove_files( &context );
      sim_run_simu_spice_restoreenv( &context );
      efg_remove_figure_alias( model->FIG );
    }
    return SIM_SIMU_COMPLETED;
  }
  sim_run_simu_spice_restoreenv( &context );
  return SIM_SIMU_COMPLETED;
} 

/******************************************************************************\

Les deux fonctions qui suivent gérent l'environnement pour contrôler en détail
le driver Spice.

\******************************************************************************/

void sim_run_simu_spice_setenv( drive_spice *context )
{
  SIM_FLOAT tmax;
  
  strcpy( context->MEMOUTLO, OUT_LO );
  context->MEMNAMENODES=V_BOOL_TAB[__MBK_SPI_NAMENODES].VALUE;
  V_BOOL_TAB[__MBK_SPI_NAMENODES].VALUE = 1;
  strcpy( OUT_LO, "spi" );

  tmax = sim_input_get_latest_event( context->MODEL );
  
  if( tmax > sim_parameter_get_tran_tmax( context->MODEL ) ) {
    sim_warning( "At least, one input has an event (%g%s) greater than Tmax (%g%s). Tmax updated to %g%s",
                 tmax, 
                 !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_TIME, 
                 sim_parameter_get_tran_tmax( context->MODEL ), 
                 !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_TIME, 
                 tmax, 
                 !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_TIME 
               );
    // zinaps : 2/3/2004, on ajoute 5% au tmax
    sim_parameter_set_tran_tmax( context->MODEL, tmax*1.05 ); // +5%
  }
}

void sim_run_simu_spice_restoreenv( drive_spice *context )
{
  drive_current *current, *next;

  V_BOOL_TAB[__MBK_SPI_NAMENODES].VALUE = context->MEMNAMENODES;

  strcpy( OUT_LO, context->MEMOUTLO );

  // Libérations

  for( current = context->CURRENT ; current ; current = next ) {
    next = current->NEXT ;

    mbkfree( current->MEASPINNAME );
    mbkfree( current->PINNAME );
    mbkfree( current );
  }
}

/******************************************************************************\

Libère les fichiers créés par SIM. 

\******************************************************************************/

void sim_run_simu_spice_drive_remove_files( drive_spice *context )
{
  chain_list    *scan;
    
  for( scan = context->DRIVEDFILE ; scan ; scan = scan->NEXT ) {
    if( sim_parameter_get_remove_files(context->MODEL) == SIM_YES ) {
      if( context->OUTPUT_SPICE_FILE_NAME!=NULL && strcmp( context->OUTPUT_SPICE_FILE_NAME, (char*)scan->DATA ) ) {
        if( unlink( (char*)scan->DATA ) != 0 )
          sim_warning( "Can't remove file %s\n", (char*)scan->DATA );
      }
    }
    mbkfree( scan->DATA );
  }
  freechain( context->DRIVEDFILE ); context->DRIVEDFILE = NULL;
}

/******************************************************************************\

Cette fonction est appellée par le driver Spice, juste avant le ENDS. On 
mémorise dans la structure CURRENT les sources de tension pour la mesure du
courant.

\******************************************************************************/

void sim_run_simu_spice_drive_ngspice_current( FILE *ptf, 
                                               lofig_list *lofig, 
                                               void *context 
                                             )
{
  sim_measure   *measure;
  chain_list    *headins, *chins ;
  locon_list    *locon;
  loins_list    *loins;
  num_list      *inodelist, *inode;
  num_list      *pnodelist, *pnode;
  drive_current *new;
  char           tmpbuf[BUFSIZ];
 
  // Recherche parmis la liste des mesures les locon qui font parti de 
  // la lofig courante (passée en paramètre).
  
  measure=NULL;
  while( (measure = sim_measure_scan( ((drive_spice*)context)->MODEL, measure ))
       ) {

    if( ( sim_measure_get_what( measure ) == SIM_MEASURE_CURRENT ) &&
        ( sim_measure_get_type( measure ) == SIM_MEASURE_LOCON   )    ) {
      sim_find_by_name( sim_measure_get_name( measure ),
                        sim_model_get_lofig( ((drive_spice*)context)->MODEL ),
                        &locon,
                        NULL,
                        &headins
                      );
      if( locon && locon->SIG->PRCN ) {
        
        for( chins = headins ; chins && chins->NEXT ; chins = chins->NEXT );
        if( chins )
          loins = (loins_list*)(chins->DATA);
        else
          loins = NULL;

        if( ( loins && loins->FIGNAME == lofig->NAME ) ||
            ( !loins && lofig == 
              sim_model_get_lofig( ((drive_spice*)context)->MODEL ) ) )
          {
         
          // On mémorise les "trous" laissés pour les mesures de courants dans
          // la structure CURRENT.
          
          inodelist = spi_getinode( locon );
          pnodelist = locon->PNODE;
          
          if( pnodelist ) {
            for( pnode = pnodelist, inode = inodelist ; 
                 pnode && inode ;
                 pnode = pnode->NEXT, inode = inode->NEXT
               ) {
               
              new = mbkalloc( sizeof( drive_current ) );
              new->NEXT = ((drive_spice*)context)->CURRENT ;
              ((drive_spice*)context)->CURRENT = new;
              sim_run_simu_spice_name_signal( ((drive_spice*)context)->MODEL,
                                              headins,
                                              locon->SIG,
                                              inode->DATA,
                                              1,
                                              tmpbuf
                                            );
              new->PINNAME = mbkstrdup(tmpbuf) ;
              sim_run_simu_spice_name_signal( ((drive_spice*)context)->MODEL,
                                              headins,
                                              locon->SIG,
                                              pnode->DATA,
                                              0,
                                              tmpbuf
                                            );
              new->MEASPINNAME = mbkstrdup( tmpbuf );
              new->MEAS = measure;
            }
          }
          else {

            inode = inodelist;

            new = mbkalloc( sizeof( drive_current ) );
            new->NEXT = ((drive_spice*)context)->CURRENT ;
            ((drive_spice*)context)->CURRENT = new;
            sim_run_simu_spice_name_signal( ((drive_spice*)context)->MODEL, 
                                            headins, 
                                            locon->SIG, 
                                            inode->DATA, 
                                            1, 
                                            tmpbuf 
                                          );
            new->PINNAME = mbkstrdup( tmpbuf );
            sim_run_simu_spice_name_signal( ((drive_spice*)context)->MODEL, 
                                            headins, 
                                            locon->SIG, 
                                            SPI_NONODES, 
                                            0, 
                                            tmpbuf 
                                          );
            new->MEASPINNAME = mbkstrdup( tmpbuf );
            new->MEAS = measure;

          }
        }
      }
      freechain( headins );
    }
  }

  ptf=NULL; // avoid a warning
}

/******************************************************************************\

Cette fonction est appellée par le driver Spice, juste avant le ENDS. On inclus
les mesures de courants dans les trous formés par les directives spi*inode. A
chaque fois qu'une source de tension est drivée, on mémorise son nom dans les 
sim_measure_detail. On fera le print du courant dans la fonction 
sim_run_simu_spice_drive_measure() avec les noms mémorisés dans les 
sim_measure_detail.

\******************************************************************************/

void sim_run_simu_spice_drive_eldo_current( FILE *ptf, 
                                            lofig_list *lofig, 
                                            void *context 
                                          )
{
  sim_measure *measure;
  chain_list  *headins, *chins ;
  locon_list  *locon;
  loins_list  *loins;
  num_list    *inodelist, *inode;
  num_list    *pnodelist, *pnode;
  char         srcname[BUFSIZ], hiername[BUFSIZ], tmpbuf[BUFSIZ];
 
  // Recherche parmis la liste des mesures les locon qui font parti de 
  // la lofig courante (passée en paramètre).
  
  measure=NULL;
  while( (measure = sim_measure_scan( ((drive_spice*)context)->MODEL, measure ))
       ) {

    if( ( sim_measure_get_what( measure ) == SIM_MEASURE_CURRENT ) &&
        ( sim_measure_get_type( measure ) == SIM_MEASURE_LOCON   )    ) {
      sim_find_by_name( sim_measure_get_name( measure ),
                        sim_model_get_lofig( ((drive_spice*)context)->MODEL ),
                        &locon,
                        NULL,
                        &headins
                      );
      if( locon && locon->SIG->PRCN ) {
        
        for( chins = headins ; chins && chins->NEXT ; chins = chins->NEXT );
        if( chins )
          loins = (loins_list*)(chins->DATA);
        else
          loins = NULL;

        if( ( loins && loins->FIGNAME == lofig->NAME ) ||
            ( !loins && lofig == 
              sim_model_get_lofig( ((drive_spice*)context)->MODEL ) ) )
          {
      
          sim_run_simu_spice_name_signal( ((drive_spice*)context)->MODEL,
                                          headins,
                                          NULL,
                                          0,
                                          0,
                                          hiername
                                        );

          // On drive les sources de tension dans les "trous" laissés entre
          // chaque connecteur et le reste de la netlist.
          
          inodelist = spi_getinode( locon );
          pnodelist = locon->PNODE;
          
          if( pnodelist ) {
            for( pnode = pnodelist, inode = inodelist ; 
                 pnode && inode ;
                 pnode = pnode->NEXT, inode = inode->NEXT
               ) {
              sprintf( srcname, 
                       "Vimeas%d", 
                       ((drive_spice*)context)->NBCURRENT++ 
                     );
              spi_print( ptf,
                         "%s %s %s dc 0%s\n",
                         srcname,
                         spinamenodedetail( locon->SIG, inode->DATA, 1 ),
                         spinamenodedetail( locon->SIG, pnode->DATA, 0 ),
                         !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":"v"
                       );

              // pas conforme aux spec des simulateurs.
              sprintf( tmpbuf, "%s%s", hiername, srcname );
              sim_measure_set_detail( measure, tmpbuf,tmpbuf );
            }
          }
          else {
            inode = inodelist;
            sprintf( srcname, 
                     "Vimeas%d", 
                     ((drive_spice*)context)->NBCURRENT++ 
                   );
            spi_print( ptf,
                       "%s %s %s dc 0%s\n",
                       srcname,
                       spinamenodedetail( locon->SIG, inode->DATA, 1 ),
                       spinamenodedetail( locon->SIG, SPI_NONODES, 0 ),
                       !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":"v"
                     );

            // pas conforme aux spec des simulateurs.
            sprintf( tmpbuf, "%s%s", hiername, srcname );
            sim_measure_set_detail( measure, tmpbuf, tmpbuf );
          }
        }
      }
      freechain( headins );
    }
  }
}

/******************************************************************************\

Les deux fonctions qui suivent servent à récupérer un index de noeud unique sur
un signal sans avoir à modifier le NBNODE.

\******************************************************************************/

int sim_run_simu_spice_drive_get_index_inode( losig_list *losig )
{
  long v;
  ptype_list *ptl;
  ptl = getptype( losig->USER, SIM_INODE_INDEX );
  if( !ptl ) {
    losig->USER = addptype( losig->USER, 
                            SIM_INODE_INDEX, 
                            (void*)losig->PRCN->NBNODE 
                          );
    ptl = losig->USER;
  }
  
  v = (long)ptl->DATA;
  (*(long*)&ptl->DATA)++;

  return (int)v;
}

void sim_run_simu_spice_drive_clear_index_inode( lofig_list *lofig )
{
  losig_list *losig;
  for( losig=lofig->LOSIG ; losig ; losig = losig->NEXT ) {
    if( getptype( losig->USER, SIM_INODE_INDEX ) )
      losig->USER = delptype( losig->USER, SIM_INODE_INDEX );
  }
}

/******************************************************************************\

Mesure de courant à l'intérieur d'un circuit :
Les deux fonctions qui suivent servent à introduire des "trous" dans la netlist
au format Spice au moyen des directives Inode. Ces trous seront comblé par
des générateurs de tension nulle qui serviront à mesurer le courant.

Cas de Ngspice :
Dans ngspice, on ne peut pas faire un print d'une source de tension à 
l'intérieur d'un subckt. On peut par contre déclarer la source de tension à
l'extérieur de ce subckt. C'est la fonction 
sim_run_simu_spice_drive_eldo_current() qui créera les sources de tension, et la
fonction sim_run_simu_spice_drive_measure() qui créera les .print.

Cas de Eldo :
On peut faire un print des sources de tension situées à l'intérieur d'un subckt.
la fonction sim_run_simu_spice_drive_eldo_current() crée les sources de tension,
et la fonction sim_run_simu_spice_drive_measure() fera les print.
\******************************************************************************/

void sim_run_simu_spice_drive_set_inode( drive_spice *context, 
                                         lofig_list *lofig
                                       )
{
  sim_measure *scan;
  chain_list  *headins, *chins ;
  locon_list  *locon;
  loins_list  *loins;
  num_list    *inodelist;
  num_list    *pnode;
 
  // Recherche les locons qui font partie de la lofig courante.

  scan=NULL;
  while( (scan = sim_measure_scan( context->MODEL, scan )) ) {
    if( ( sim_measure_get_what( scan ) == SIM_MEASURE_CURRENT ) &&
        ( sim_measure_get_type( scan ) == SIM_MEASURE_LOCON   )    ) {
      sim_find_by_name( sim_measure_get_name( scan ),
                        sim_model_get_lofig( context->MODEL ),
                        &locon,
                        NULL,
                        &headins
                      );
      if( locon && locon->SIG->PRCN ) {
        for( chins = headins ; chins && chins->NEXT ; chins = chins->NEXT );
        if( chins )
          loins = (loins_list*)(chins->DATA);
        else
          loins = NULL;
        if( !loins || ( loins && loins->FIGNAME == lofig->NAME ) ) {

          // Le locon appartient à la lofig courante, et on a une mesure de
          // courant à réaliser dessus
          
          inodelist = NULL;
          if( locon->PNODE ) {
            for( pnode = locon->PNODE ; pnode ; pnode = pnode->NEXT ) {
              inodelist = addnum( inodelist, 
                          sim_run_simu_spice_drive_get_index_inode( locon->SIG )
                                );
            }
          }
          else {
            inodelist = addnum( inodelist,
                          sim_run_simu_spice_drive_get_index_inode( locon->SIG )
                              );
          }
          spi_setinode( locon, (num_list*)reverse( (chain_list*)inodelist ) );
        }
        else {
          if( loins ) {
            /* Comme la hiérarchie n'est pas forcement drivée, on ne peut pas
               la modifier pour y rajouter des "trous". */
            fprintf( stderr, "can't drive internal current measure %s\n",
                             sim_measure_get_name( scan ) );
          }
        }
      }
      freechain( headins );
    }
  }

  switch( sim_parameter_get_tool( context->MODEL ) ) {
  case SIM_TOOL_ELDO: 
  case SIM_TOOL_TITAN:
  case SIM_TOOL_TITAN7:
  case SIM_TOOL_HSPICE: 
  case SIM_TOOL_MSPICE: 
    spi_setfuncinode( sim_run_simu_spice_drive_eldo_current, context );
    break;
  case SIM_TOOL_LTSPICE:
  case SIM_TOOL_NGSPICE:
  case SIM_TOOL_SPICE:
    spi_setfuncinode( sim_run_simu_spice_drive_ngspice_current, context );
    break;
  }
}

void sim_run_simu_spice_drive_del_inode( drive_spice *context,
                                         lofig_list *lofig
                                       )
{
  locon_list    *locon;
  lotrs_list    *lotrs;
  loins_list    *loins;

  for( locon = lofig->LOCON ; locon ; locon = locon->NEXT )
    spi_clearinode( locon );
  for( lotrs = lofig->LOTRS ; lotrs ; lotrs = lotrs->NEXT ) {
    spi_clearinode( lotrs->DRAIN );
    spi_clearinode( lotrs->GRID );
    spi_clearinode( lotrs->SOURCE );
    if (lotrs->BULK) spi_clearinode( lotrs->BULK );
  }
  for( loins = lofig->LOINS ; loins ; loins = loins->NEXT ) {
    for( locon = loins->LOCON ; locon ; locon = locon->NEXT ) {
      spi_clearinode( locon );
    }
  }
  sim_run_simu_spice_drive_clear_index_inode( lofig );

  context=NULL; // avoid a warning.
}

/******************************************************************************\

FUNC : sim_drive_corresp_alias

\******************************************************************************/
void sim_drive_corresp_alias ( drive_spice *context, lofig_list *fig )
{
  FILE       *file;
  lotrs_list *lotrs;
  losig_list *losig;
  char *signame,filename[1024];
  char *origsigname,*origtrsname;

  if ( V_BOOL_TAB[__EFG_CORRESP_ALIAS].VALUE ) {
    if (context->OUTPUT_SPICE_FILE_POINTER==NULL)
      {
        if (context->OUTPUT_SPICE_FILE_NAME==NULL)
          sprintf (filename,"%s/%s.%s",WORK_LIB,fig->NAME,OUT_LO);
        else
          sprintf (filename,"%s",context->OUTPUT_SPICE_FILE_NAME);
        file = fopen (filename,"a");
      }
    else 
      file = context->OUTPUT_SPICE_FILE_POINTER;

    if ( file !=NULL) {
      if ( V_STR_TAB[__EFG_SIG_ALIAS].VALUE ) {
        fprintf (file,"******************************\n");
        fprintf (file,"*    Alias   ===>   Signal   *\n");
        fprintf (file,"******************************\n\n");
        for ( losig = fig->LOSIG ; losig ; losig=losig->NEXT ) {
          signame = getsigname (losig);
          origsigname = efg_get_origsigname(losig);
          if ( efg_SigIsAlim (losig) == 'N' && origsigname ) {
            fprintf (file,"* %s ===> %s\n",signame,origsigname);
            //efg_del_corresp_alias (losig);
          }
        }
        fprintf(file,"\n");
      }
      if ( V_STR_TAB[__EFG_TRS_ALIAS].VALUE ) {
        fprintf (file,"******************************\n");
        fprintf (file,"*    Alias   ===> Transistor *\n");
        fprintf (file,"******************************\n\n");
        for ( lotrs = fig->LOTRS ; lotrs ; lotrs=lotrs->NEXT ) {
          origtrsname = efg_get_origtrsname(lotrs);
          if ( origtrsname ) {
            fprintf (file,"* %s ===> %s\n",lotrs->TRNAME,origtrsname);
            //efg_del_corresp_alias_trs (lotrs);
          }
        }
      }
      
      if (context->OUTPUT_SPICE_FILE_POINTER==NULL)
        fclose (file);
    }
  }
}

/******************************************************************************\

Ecrit sur disque le ou les fichiers qu'on va simuler. Cette fonction s'occupe
de créer les emplacement nécessaires pour introduire les mesures de courant.

\******************************************************************************/

int  sim_run_simu_spice_drive_subckt( drive_spice *context )
{
  FILE *test;
  char name[BUFSIZ];
  chain_list *drive=NULL;
  chain_list *headdrive,*modelchain;
  lofig_list *figure,*figins;
  int oldspicemode;

  if (strcmp(context->MODEL->SUBCKTFILENAME,"")!=0) 
   {
     strcpy(context->SUBCKTFILENAME, context->MODEL->SUBCKTFILENAME);
     return 1;
   }

  if (context->OUTPUT_SPICE_FILE_POINTER==NULL)
    {
      headdrive = sim_getfiglist( sim_model_get_lofig( context->MODEL ) );
      for( drive = headdrive ; drive ; drive = drive->NEXT ) {

        figure = (lofig_list*)(drive->DATA);
        if (context->OUTPUT_SPICE_FILE_NAME!=NULL && figure==sim_model_get_lofig(context->MODEL))
          sprintf( name, "%s/%s", WORK_LIB, context->OUTPUT_SPICE_FILE_NAME);
        else
          sprintf( name, "%s/%s.%s", WORK_LIB, figure->NAME, OUT_LO );
   
        if( sim_parameter_get_drive_netlist( context->MODEL ) == SIM_YES ) {
          if( sim_parameter_get_allow_overwrite_files( context->MODEL ) == SIM_NO )
            {
              test = mbkfopen( name, NULL, READ_TEXT );
              if( test ) {
                fclose( test );
                freechain( headdrive );
                return 0;
              }
            }
      
          sim_run_simu_spice_drive_set_inode( context, figure );

          oldspicemode=SPI_LOWRESISTANCE_AS_TENSION;
          SPI_LOWRESISTANCE_AS_TENSION=0;

          for ( modelchain = figure->MODELCHAIN ; modelchain ; modelchain=modelchain->NEXT ) {
            if ((figins = getloadedlofig ( (char*)modelchain->DATA ))) {
              if ( !filepath ( figins->NAME , IN_LO)) {
                spicesavelofig ( figins );
                sim_drive_corresp_alias ( context, figins );
              }
            }
          }
          if (context->OUTPUT_SPICE_FILE_NAME!=NULL && figure==sim_model_get_lofig(context->MODEL))
            {
              FILE *f;
              if ((f=fopen( name, "wt"))==NULL) return 0;
              spicesavelofiginfile( figure, f, 0);
              fclose(f);
            }
          else
            spicesavelofig( figure );

          sim_drive_corresp_alias ( context, figure );

          SPI_LOWRESISTANCE_AS_TENSION=oldspicemode;

          sim_run_simu_spice_drive_del_inode( context, figure );
      
          context->DRIVEDFILE = addchain( context->DRIVEDFILE, mbkstrdup( name ) );
        }
      }
      freechain( headdrive );

      sprintf( context->SUBCKTFILENAME, 
               "%s.%s", 
               sim_model_get_lofig( context->MODEL )->NAME, 
               OUT_LO 
             );
    }
  else
    {
      
      sim_run_simu_drive_header(context->OUTPUT_SPICE_FILE_POINTER, context);

      headdrive = sim_getallfiglist(sim_model_get_lofig(context->MODEL));
      headdrive = append(headdrive, addchain(NULL, sim_model_get_lofig(context->MODEL)));
      for( drive = headdrive ; drive!=NULL ; drive = drive->NEXT ) 
        {
          figure = (lofig_list*)(drive->DATA);
          sim_run_simu_spice_drive_set_inode( context, figure );
          
          oldspicemode=SPI_LOWRESISTANCE_AS_TENSION;
          SPI_LOWRESISTANCE_AS_TENSION=0;
          
          spicesavelofiginfile( figure, context->OUTPUT_SPICE_FILE_POINTER, 0);
          
          sim_drive_corresp_alias ( context, figure );
          
          SPI_LOWRESISTANCE_AS_TENSION=oldspicemode;
          
          sim_run_simu_spice_drive_del_inode( context, figure );
          
        }
      
//      context->DRIVEDFILE = addchain(context->DRIVEDFILE, strdup(context->OUTPUT_SPICE_FILE_NAME));
      freechain(headdrive);
    }

  return 1;
}

/******************************************************************************\

Crée un nom de fichier pour la simulation. 
Pour l'instant, ça ne fait pas grand chose, mais on peut peut être prévoir 
vérifier si un fichier du même nom n'existe pas déjà.

\******************************************************************************/

void sim_run_simu_spice_create_cmdfilename( drive_spice *context )
{
  if (context->OUTPUT_SPICE_FILE_POINTER!=NULL)
    sprintf( context->CMDFILENAME, "%s", context->OUTPUT_SPICE_FILE_NAME);
  else
    {
      if (context->OUTPUT_SPICE_FILE_NAME!=NULL)
        sprintf( context->CMDFILENAME, "cmd_%s", context->OUTPUT_SPICE_FILE_NAME);
      else
        sprintf( context->CMDFILENAME, "cmd_%s.spi", context->MODEL->FIGNAME );
    }
}

/******************************************************************************\

Drive toutes les entrées pour la simulation.

\******************************************************************************/
static double thres_2_real(sim_model *sc, double slope)
{
  return slope+sim_parameter_get_slopeVTHL(sc)*slope+(1-sim_parameter_get_slopeVTHH(sc))*slope;
}

static char *sim_getanodename(sim_model   *model, char *name, char *bufname)
{
  locon_list *locon;
  losig_list *losig;
  chain_list *headins;
  long num;
  
      sim_find_by_name( name,
                      sim_model_get_lofig(model),
                      &locon,
                      &losig,
                      &headins
                    );
                   
    if( locon || losig )
    {
    if( locon ) {
      if( locon->PNODE )
        num = locon->PNODE->DATA;
      else 
        num = SPI_NONODES;
      losig = locon->SIG;
    }
    else {
      if( losig ) {
        num = sim_run_simu_spice_get_one_node( losig );
      }
    }
   
        sim_run_simu_spice_name_signal( model,
                                        headins,
                                        losig,
                                        num,
                                        0,
                                        bufname
                                      );
        return bufname;
    }
    return NULL;

}

void sim_run_simu_spice_drive_input( drive_spice *context )
{
  locon_list *locon;
  char        type;
  char        subtype;
  char        level;
  SIM_FLOAT   trise;
  SIM_FLOAT   tstart;
  SIM_FLOAT   tfall;
  SIM_FLOAT   period;
  char       *pattern;
  SIM_FLOAT   alim;
  SIM_FLOAT   v1, v2, t;
  int         n, i;
  sim_input   *input;
  num_list    *scannum, *headnum;
  sim_model   *model;
  char         bufname[1024];
  losig_list  *losig;
  chain_list  *headins;
  int          srcidx=1;
  int          tool;
  ptype_list *pt;

  model = context->MODEL ;
 
  tool = sim_parameter_get_tool( model );
  input=NULL;
  while( (input = sim_input_scan( model, input ) ) )
  {
    type = sim_input_get_type( input );

    if( type == SIM_NC )
      continue;

    sim_find_by_name( sim_input_get_name( input ),
                      sim_model_get_lofig( model ),
                      &locon,
                      &losig,
                      &headins
                    );
                   
    if( !locon && !losig )
      continue ;

   
    switch( type ) {
    case SIM_STUCK :
      spi_print( context->PTF, 
                 "* Static input %s\n", 
                 sim_input_get_name( input ) 
               );
      break;
    case SIM_SLOPE :
      spi_print( context->PTF, 
                 "* Dynamic input %s\n", 
                 sim_input_get_name( input ) 
               );
      break;
    case SIM_FUNC :
      spi_print( context->PTF, 
                 "* Mathematic function input %s\n", 
                 sim_input_get_name( input ) 
               );
      break;
    }

    headnum=NULL;
    if( locon ) {
      if( locon->PNODE ) {
        headnum = dupnumlst( locon->PNODE );
      }
      else {
        headnum = addnum( NULL, SPI_NONODES );
      }
      losig = locon->SIG;
    }
    else {
      if( losig ) {
        headnum = addnum( NULL, sim_run_simu_spice_get_one_node( losig ) );
      }
    }
   
    if (!(headnum && (pt=getptype(losig->USER, LOFIGCHAIN))!=NULL && pt->DATA==NULL && (losig->PRCN==NULL || (losig->PRCN->PCTC==NULL && losig->PRCN->PWIRE==NULL))))
    {
      for( scannum = headnum ; scannum ; scannum = scannum->NEXT ) {

        sim_run_simu_spice_name_signal( model,
                                        headins,
                                        losig,
                                        scannum->DATA,
                                        0,
                                        bufname
                                      );

        switch( type ) {
        case SIM_MIMIC:
           {
             char bufnode[1024], bufvdd[1024], bufvss[1024];
             spi_print( context->PTF, "* Voltage mimic input %s, source %s, revert=%s\n",
                             sim_input_get_name( input ), input->UINPUT.INPUT_MIMIC.REF_NODE,
                             input->UINPUT.INPUT_MIMIC.revert?"yes":"no");
             if (sim_getanodename(model, input->UINPUT.INPUT_MIMIC.REF_NODE, bufnode)!=NULL
                  && sim_getanodename(model, input->UINPUT.INPUT_MIMIC.REF_VDDNODE, bufvdd)!=NULL
                  && sim_getanodename(model, input->UINPUT.INPUT_MIMIC.REF_VSSNODE, bufvss)!=NULL)
              {
                spi_print( context->PTF, "E%d %s %s ", srcidx++, bufname, bufvss);
                if (input->UINPUT.INPUT_MIMIC.revert)
                  spi_print( context->PTF, "%s %s", bufvdd, bufnode);
                else
                  spi_print( context->PTF, "%s %s", bufnode, bufvdd);
                spi_print( context->PTF," 1\n");
              }
             else
               spi_print( context->PTF,"* failed\n");

             break;
           }

        case SIM_STUCK :
       
          spi_print( context->PTF, "V%d %s 0 ", srcidx++, bufname );
          subtype = sim_input_get_stuck_type( input );
          
          switch( subtype )
          {
          case SIM_STUCK_LEVEL :

            spi_print( context->PTF, "dc %g%s",
                     sim_input_get_stuck_level( input ) == SIM_ZERO ?
                                            sim_input_get_vss(input) : sim_input_get_vdd(input),
                       !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_VOLTAGE
                     );
            break;
            
          case SIM_STUCK_VALUE :
            spi_print( context->PTF, "dc %g%s",
                       sim_input_get_stuck_voltage( input ),
                       !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_VOLTAGE
                     );
            break;
          }

          break;

        case SIM_SLOPE :

          spi_print( context->PTF, "V%d %s 0 ", srcidx++, bufname );
          subtype = sim_input_get_slope_type( input );
          
          switch( subtype )
          {
          case SIM_SLOPE_SINGLE :
            sim_input_get_slope_single( input, &level, &trise, &tstart );
            trise=thres_2_real(model, trise);
            if( level == SIM_RISE ) {
              v1 = sim_input_get_vss(input);
              v2 = sim_input_get_vdd(input) ;
            }
            else {
              v1 = sim_input_get_vdd(input) ;
              v2 = sim_input_get_vss(input);
            }

            if( tstart > 0.0 ) {
              if (( tool == SIM_TOOL_TITAN ) || ( tool == SIM_TOOL_TITAN7 ))
                spi_print( context->PTF, 
                         "PWL ( 0%s %g%s %g%s %g%s %g%s %g%s %g%s %g%s )",
                         !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_TIME,
                         v1,
                         !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_VOLTAGE,
                
                         tstart,
                         !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_TIME,
                         v1,
                         !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_VOLTAGE,
                
                         tstart+trise,
                         !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_TIME,
                         v2,
                         !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_VOLTAGE,
                
                         sim_parameter_get_tran_tmax( model ),
                         !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_TIME,
                         v2,
                         !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_VOLTAGE
                       );
              else
                spi_print( context->PTF, 
                         "DC 0 PWL ( 0%s %g%s %g%s %g%s %g%s %g%s %g%s %g%s )",
                         !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_TIME,
                         v1,
                         !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_VOLTAGE,
                
                         tstart,
                         !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_TIME,
                         v1,
                         !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_VOLTAGE,
                
                         tstart+trise,
                         !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_TIME,
                         v2,
                         !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_VOLTAGE,
                
                         sim_parameter_get_tran_tmax( model ),
                         !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_TIME,
                         v2,
                         !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_VOLTAGE
                       );
            }
            else {
              if (( tool == SIM_TOOL_TITAN ) || ( tool == SIM_TOOL_TITAN7 ))
                spi_print( context->PTF, 
                         "PWL ( %g%s %g%s %g%s %g%s %g%s %g%s )",
                
                         tstart,
                         !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_TIME,
                         v1,
                         !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_VOLTAGE,
                
                         tstart+trise,
                         !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_TIME,
                         v2,
                         !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_VOLTAGE,
                
                         sim_parameter_get_tran_tmax( model ),
                         !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_TIME,
                         v2,
                         !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_VOLTAGE
                       );
              else
                spi_print( context->PTF, 
                         "DC 0 PWL ( %g%s %g%s %g%s %g%s %g%s %g%s )",
                
                         tstart,
                         !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_TIME,
                         v1,
                         !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_VOLTAGE,
                
                         tstart+trise,
                         !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_TIME,
                         v2,
                         !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_VOLTAGE,
                
                         sim_parameter_get_tran_tmax( model ),
                         !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_TIME,
                         v2,
                         !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_VOLTAGE
                       );
            }
            break ;
            
          case SIM_SLOPE_PATTERN :
          
            sim_input_get_slope_pattern( input, &trise, &tfall, &period, &pattern );
            alim=sim_input_get_vdd(input);
            trise=thres_2_real(model, trise);
            tfall=thres_2_real(model, tfall);
            t=0.0;

            if (( tool == SIM_TOOL_TITAN ) || ( tool == SIM_TOOL_TITAN7 ))
              spi_print( context->PTF, "PWL ( %g%s %g%s ",
                                       0.0, !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_TIME,
                                       pattern[0]=='1' ? alim : sim_input_get_vss(input), 
                                       !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_VOLTAGE
                       );
            else
              spi_print( context->PTF, "DC 0 PWL ( %g%s %g%s ",
                                       0.0, !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_TIME,
                                       pattern[0]=='1' ? alim : sim_input_get_vss(input), 
                                       !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_VOLTAGE
                       );

            for( n=1 ; pattern[n]!='\0' ; n++ ) {
              if( pattern[n-1] == '0' && pattern[n] == '1' ) {
                spi_print( context->PTF,
                           "%g%s %g%s %g%s %g%s ",

                           ((SIM_FLOAT)n)*period, !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_TIME,
                           sim_input_get_vss(input), !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_VOLTAGE,

                           ((SIM_FLOAT)n)*period+trise, !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_TIME,
                           alim, !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_VOLTAGE
                         );
                t = ((SIM_FLOAT)n)*period+trise;
              }
              if( pattern[n-1] == '1' && pattern[n] == '0' ) {
                spi_print( context->PTF, "%g%s %g%s %g%s %g%s ",

                           ((SIM_FLOAT)n)*period, !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_TIME,
                           alim, !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_VOLTAGE,

                           ((SIM_FLOAT)n)*period+tfall, !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_TIME,
                           sim_input_get_vss(input), !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_VOLTAGE
                         );
                t = ((SIM_FLOAT)n)*period+tfall;
              }
            }
            if( pattern[n-1] == '1' ) {
              spi_print( context->PTF, 
                         "%g%s %g%s )",
                         ((SIM_FLOAT)n)*period,
                         !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_TIME,
                         alim,
                         !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_VOLTAGE
                       );
            }
            else {
              spi_print( context->PTF, 
                         "%g%s %g%s )",
                         ((SIM_FLOAT)n)*period,
                         !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_TIME,
                         sim_input_get_vss(input),
                         !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_VOLTAGE
                       );
            }
          }
          break;
        
        case SIM_FUNC:
          spi_print( context->PTF, "V%d %s 0 ", srcidx++, bufname );
          n = TABSIZE;
          sim_calc_func( model, input, context->TAB, &n );
          if (( tool == SIM_TOOL_TITAN ) || ( tool == SIM_TOOL_TITAN7 ))
            spi_print( context->PTF, "PWL ( " );
          else
            spi_print( context->PTF, "DC 0 PWL ( " );
          for( i = 0; i < n ; i = i+2 ) {
            if( ! input->DELTAVAR ) {
              spi_print( context->PTF, 
                         "%g%s %g%s ", 
                         context->TAB[i  ], !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_TIME,
                         context->TAB[i+1], !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_VOLTAGE
                       );
            }
            else {
              spi_print( context->PTF, 
                         "'%g%s+%s' %g%s ", 
                         context->TAB[i  ], !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_TIME, input->DELTAVAR,
                         context->TAB[i+1], !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_VOLTAGE
                       );
            }
          }
          spi_print( context->PTF, ")" );
          break;

        }

        spi_print( context->PTF, "\n\n" );
      }
    }
    freechain( headins );
    freenum( headnum );
  }
}

/****************************************************************************/
/*{{{                    sim_run_simu_spice_drive_spice_measure()           */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void sim_run_simu_spice_drive_spice_measure(drive_spice *context)
{
  sim_printMeasCmd(context->MODEL,context->PTF);
}

/*}}}************************************************************************/

/******************************************************************************\

Drive toutes les conditions initiales pour la simulation.

\******************************************************************************/

void sim_run_simu_spice_drive_ic( drive_spice *context )
{
  sim_ic        *ic;
  chain_list    *headnode, *scan;
  lofig_list    *lofig;
  sim_model     *model;
  FILE          *file;
  char          *ic_name;
  num_list      *num;
  char          tmpbuf[1024];
  locon_list    *locon;
  losig_list    *losig;
  chain_list    *headins;
  int            node;
  ht            *htabcon2ic=NULL;
  int            icsig,iccon;

  model = context->MODEL;
  file = context->PTF;
  lofig = sim_model_get_lofig( context->MODEL );
 
  ic = NULL;
  while( (ic = sim_ic_scan( model, ic )) ) { 
    icsig = iccon = 0;

    headnode = NULL;
    ic_name = sim_ic_get_name( ic );
    spi_print( file, "* Ic for locon %s\n", ic_name );
    
  
    switch( sim_ic_get_locate( ic ) ) {
    case SIM_IC_LOCON :
      iccon = 1;

      if ( !htabcon2ic )
        htabcon2ic = addht (10);
      
      sim_find_by_name( ic_name,
                        lofig,
                        &locon,
                        NULL,
                        &headins
                      );

      if( locon ) {
        if( locon->PNODE ) {
          for( num = locon->PNODE ; num ; num = num->NEXT ) {
            sim_run_simu_spice_name_signal( context->MODEL,
                                            headins,
                                            locon->SIG,
                                            num->DATA,
                                            0,
                                            tmpbuf
                                          );
            //headnode = addchain( headnode, strdup( tmpbuf ) );
            headnode = addchain( headnode, namealloc( tmpbuf ) );
          }
        }
        else {
          sim_run_simu_spice_name_signal( context->MODEL,
                                          headins,
                                          locon->SIG,
                                          SPI_NONODES,
                                          0,
                                          tmpbuf
                                        );
          //headnode = addchain( headnode, strdup( tmpbuf ) );
          headnode = addchain( headnode, namealloc( tmpbuf ) );
        }
      }
      break;

    case SIM_IC_SIGNAL :
      icsig = 1;
    
      sim_find_by_name( ic_name,
                        lofig,
                        NULL,
                        &losig,
                        &headins
                      );
      if( losig ) {
        node = sim_run_simu_spice_get_one_node( losig );
        sim_run_simu_spice_name_signal( context->MODEL,
                                        headins,
                                        losig,
                                        node,
                                        0,
                                        tmpbuf
                                      );
        //headnode = addchain( headnode, strdup( tmpbuf ) );
        headnode = addchain( headnode, namealloc( tmpbuf ) );
      }
      break;
    default :
      headnode = NULL;
    }

    for( scan = headnode ; scan ; scan = scan->NEXT ) {
      if ( htabcon2ic ) {
        if ( gethtitem (htabcon2ic,scan->DATA) != DELETEHT &&
             gethtitem (htabcon2ic,scan->DATA) != EMPTYHT ) {
          spi_print( file, "* v(%s) has already been initialized\n", (char*)scan->DATA);
          continue;
        }
      }


      
      switch( sim_ic_get_type( ic ) ) {
      case SIM_IC_VOLTAGE:
        spi_print( file, ".IC v(%s)=%g%s\n", 
                       (char*)scan->DATA,
                       sim_ic_get_voltage( ic ),
                       !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_VOLTAGE
               );
        if ( htabcon2ic )
          addhtitem ( htabcon2ic,scan->DATA, 0 );
        break;
      case SIM_IC_LEVEL:
        if( sim_ic_get_level( ic ) == SIM_ZERO )
          spi_print( file, ".IC v(%s)=%g%s\n", 
                         (char*)scan->DATA,
                         sim_ic_get_vss(ic),
                         !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_VOLTAGE
                 );
        else
          spi_print( file, ".IC v(%s)=%g%s\n", 
                         (char*)scan->DATA,
                         sim_ic_get_vdd(ic),
                         !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_VOLTAGE
                 );
        if ( htabcon2ic )
          addhtitem ( htabcon2ic,scan->DATA, 0 );
        break;
      }
    }

    /*
    for (scan = headnode; scan; scan = scan->NEXT)
      mbkfree(scan->DATA);
      */
    freechain( headnode );
    spi_print( file, "\n" );
  }
  if ( htabcon2ic ) delht ( htabcon2ic );
}

/******************************************************************************\

Drive toutes les mesures pour la simulation.

Mesures de tension :
Les sim_measure_detail sont crées ici, en même temps qu'on fait les print.

Mesures de courant :
Les sources de tension ont été créés, il n'y a qu'a faire un print des 
sim_measure_detail.

\******************************************************************************/

void sim_run_simu_spice_drive_measure( drive_spice *context )
{
  sim_measure   *measure;
  sim_measure   *prevmeasure;
  sim_measure_detail *detail;
  num_list      *headnode, *scan;
  chain_list    *headins;
  int           nbsrc=1;
  char          srcname[1024];
  char          bufname[1024];
  FILE          *file;
  sim_model     *model;
  lofig_list    *lofig;
  char          *print;
  char          *meas_name;
  int           alias;
  locon_list    *locon;
  losig_list    *losig;
  int           node;
  int           printcon,printsig;
  ht           *htabcon2print =NULL; // is used to not print many times the same locon
 
  model = context->MODEL;
  file = context->PTF;
  lofig = sim_model_get_lofig( model );

  measure = NULL;

  alias = 0;
  /*
  if (( str = getenv("SIM_NO_ALIAS") ))
    if ( !strcasecmp (str, "yes") ) alias = 0;
    */

  while( (measure = sim_measure_scan( model, measure )) ) {
    printcon = 0;
    printsig = 0;

    switch( sim_measure_get_what( measure ) ) {

    case SIM_MEASURE_VOLTAGE :
      printcon = 1;

      if ( !htabcon2print )
        htabcon2print = addht (10);
      
      headnode = NULL;
      meas_name = sim_measure_get_name( measure );

      switch( sim_measure_get_type( measure ) ) {
      case SIM_MEASURE_LOCON :

        sim_find_by_name( meas_name,
                          lofig,
                          &locon,
                          NULL,
                          &headins
                        );

        if( !locon )
          continue;

        spi_print( file, "* Voltage measure for locon %s\n", 
                         meas_name
                 );

        if( locon->PNODE )
          headnode = (num_list*)dupchainlst( (chain_list*) locon->PNODE );
        else
          headnode = addnum( NULL, SPI_NONODES );
        
        losig = locon->SIG;
        break;

      case SIM_MEASURE_SIGNAL :
      
        printsig = 1;
        sim_find_by_name( meas_name,
                          lofig,
                          NULL,
                          &losig,
                          &headins
                        );

        if( !losig )
          continue;

        spi_print( file, "* Voltage measure for signal %s\n", 
                       sim_measure_get_name( measure ) 
               );
        
        headnode = addnum( NULL, 
                           sim_run_simu_spice_get_one_node( losig )
                         );
                           
        break;
      }

      for( scan = headnode ; scan ; scan = scan->NEXT ) {
        node = scan->DATA;
        sim_run_simu_spice_name_signal( model,
                                        headins,
                                        losig,
                                        node,
                                        0,
                                        bufname
                                      );
        if( sim_measure_get_what( measure ) == SIM_MEASURE_VOLTAGE ) {
          if( strlen( bufname ) > 8 && alias ) {
            sprintf( srcname, "als_%d", nbsrc++ );
            if ( gethtitem ( htabcon2print, srcname ) != EMPTYHT &&
                 gethtitem ( htabcon2print, srcname ) != DELETEHT )
              printcon = 0;
            if ( printsig || printcon ) {
              spi_print( file, "* node %s is an alias to node %s\n", 
                               srcname, 
                               bufname
                       );
              spi_print( file, "V%s %s %s dc 0%s\n", 
                               srcname, 
                               srcname, bufname,
                               !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":"v"
                       );
              print = namealloc(srcname);
            }
          }
          else 
            print = namealloc(bufname);

          if ( htabcon2print && printcon ) {
            if ( gethtitem ( htabcon2print, print ) != EMPTYHT &&
                 gethtitem ( htabcon2print, print ) != DELETEHT )
              printcon = 0;
          }
          if ( printsig || printcon ) {
            spi_print( file, ".print tran v(%s)\n", print );
            sim_measure_set_detail( measure, bufname, print );
          }
          else 
            spi_print( file, "* v(%s) has already been printed \n", print );
          if ( htabcon2print && printcon )
            addhtitem ( htabcon2print, print, 0);
          if ( !printcon && !printsig ) {
            sim_measure_clear( model, meas_name, SIM_MEASURE_LOCON, SIM_MEASURE_VOLTAGE);
            measure = prevmeasure;
          }
        }
      }
      freechain( headins );
      freenum( headnode );
      spi_print( file, "\n" );

      break;

    case SIM_MEASURE_CURRENT :
      spi_print( file, "* Current measure for locon %s\n", 
                       sim_measure_get_name( measure ) 
               );
      detail = NULL;
      while( (detail=sim_measure_detail_scan( measure, detail )) ) {
        spi_print( file, ".print tran I(%s)\n", 
                         sim_measure_detail_get_name( detail ) 
                 );
      }
      spi_print( file, "\n" );
      break;
    }
    prevmeasure = measure;
  }
  if ( htabcon2print ) delht ( htabcon2print );
}

/******************************************************************************\

Crée l'appel au subckt du circuit à simuler.

\******************************************************************************/

void sim_run_simu_spice_drive_x( drive_spice *context )
{
  num_list      *scan;
  locon_list    *locon;
  FILE          *file;
  sim_model     *model; 
  lofig_list    *lofig; 
  SIM_FLOAT val;
  int cnt=1;
  ptype_list *pt;
  
  file = context->PTF ;
  model = context->MODEL ;
  lofig = sim_model_get_lofig( model );

  spi_print( file, "X%s ", lofig->NAME );
 
  lofig->LOCON = (locon_list*) reverse( (chain_list*) lofig->LOCON );
  for( locon = lofig->LOCON ; locon ; locon = locon->NEXT ) {

    if( locon->PNODE ) {
      for( scan = locon->PNODE ; scan ; scan = scan->NEXT ) 
        spi_print( file,
                   "%s ", 
                   sim_run_simu_spice_drive_pin( 
                         locon->NAME,
                         spinamenode( locon->SIG, scan->DATA ),
                         context
                                               )
                 );
    }
    else {
      spi_print( file,
                 "%s ", 
                 sim_run_simu_spice_drive_pin( 
                       locon->NAME,
                       spinamenode( locon->SIG, SPI_NONODES ),
                       context
                                             )
               );
    }
  }

  spi_print( file, "%s\n\n", lofig->NAME );

  for( locon = lofig->LOCON ; locon ; locon = locon->NEXT ) 
  {
    if ((pt=getptype(locon->USER, SIM_EXT_CAPA))!=NULL)
    {
      val=*(SIM_FLOAT *)pt->DATA;
      if (val!=0) 
        spi_print( file, "Coutput%d %s 0 %g\n", cnt++,
                 sim_run_simu_spice_drive_pin( locon->NAME, spinamenode( locon->SIG, SPI_NONODES ), context),
                 val
               );
    }
  }
 if (cnt!=1) spi_print( file, "\n" );

 lofig->LOCON = (locon_list*) reverse( (chain_list*) lofig->LOCON );

  // unused parameter : avoid a warning
  model = NULL;
}

/******************************************************************************\

Création des alimentations générales.

\******************************************************************************/

void sim_run_simu_spice_drive_alim( drive_spice *context )
{
  locon_list    *locon;
  num_list      *node;
  FILE          *file; 
  sim_model     *model; 
  lofig_list    *lofig;
  int            nvdd = 0;
  int            nvss = 0;
  float          alim;
  losig_list *losig;
  char *signame;

  model = context->MODEL;
  lofig = sim_model_get_lofig( model );
  file  = context->PTF;

  for( locon = lofig->LOCON ; locon ; locon = locon->NEXT ) {
    if( mbk_LosigIsVDD(locon->SIG) ) {
      if ( !getlosigalim (locon->SIG, &alim)) {
        if( locon->PNODE )
          for( node = locon->PNODE ; node ; node = node->NEXT )
            spi_print( file, 
                     "VDD%d %s 0 dc %g%s\n", 
                     nvdd++,
                     spinamenode( locon->SIG, node->DATA ),
                     sim_parameter_get_alim( model, 'i' ),
                     !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_VOLTAGE     
                   );
        else
          spi_print( file, 
                   "VDD%d %s 0 dc %g%s\n", 
                   nvdd++,
                   spinamenode( locon->SIG, SPI_NONODES ),
                   sim_parameter_get_alim( model, 'i' ),
                   !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_VOLTAGE     
                 );
      }
      else
        sim_input_set_stuck_voltage ( model, locon->NAME, alim);
        
    }
    if( mbk_LosigIsVSS(locon->SIG) && (!getlosigalim(locon->SIG,NULL)) ) {
      if ( !getlosigalim (locon->SIG, &alim) ) {
        if( locon->PNODE )
          for( node = locon->PNODE ; node ; node = node->NEXT )
            spi_print( file, 
                     "VSS%d %s 0 dc %g%s\n", 
                     nvss++,
                     spinamenode( locon->SIG, node->DATA ),
                     0.0,
                     !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_VOLTAGE     
                   );
        else
          spi_print( file, 
                   "VSS%d %s 0 dc %g%s\n", 
                   nvss++,
                   spinamenode( locon->SIG, SPI_NONODES ),
                   0.0,
                   !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_VOLTAGE     
                 );
      }
      else
        sim_input_set_stuck_voltage ( model, locon->NAME, alim);
        
    }
  }

  for ( losig = lofig->LOSIG ; losig ; losig = losig->NEXT ) {
    signame = getsigname(losig);
    if (losig->TYPE==INTERNAL && !strcmp(signame,"0")) continue;
    if ( getlosigalim (losig,&alim) )
      sim_input_set_stuck_voltage ( model, signame, alim);
  }

  spi_print( file, "\n" );
}

/******************************************************************************\

Création du fichier de simulation général, celui qui va étre passé en paramètre
au simulateur.

\******************************************************************************/
int  sim_run_simu_spice_drive_cmd( drive_spice *context )
{
  FILE       *file;
  char        name[BUFSIZ];
  sim_model  *model;
  lofig_list *lofig;
  char       *cmdfilename;
  int         tool;
  char       *pt;

  cmdfilename = context->CMDFILENAME ;
  model = context->MODEL;
  lofig = sim_model_get_lofig( model );

  tool = sim_parameter_get_tool( model );
  if (context->OUTPUT_SPICE_FILE_NAME==NULL && context->OUTPUT_SPICE_FILE_POINTER==NULL)
    sprintf( name, "%s/%s", WORK_LIB, cmdfilename );
  else
    sprintf( name, "%s", cmdfilename );
  
  if (context->OUTPUT_SPICE_FILE_POINTER==NULL)
    {
      if( sim_parameter_get_allow_overwrite_files( model ) == SIM_NO ) {
        file = mbkfopen( name, NULL, READ_TEXT );
        if( file ) {
          fclose( file );
          return 0;
        }
      }
      
      file = mbkfopen( cmdfilename, NULL, WRITE_TEXT );
      if( !file ) {
        return 0;
      }
      
      context->DRIVEDFILE = addchain( context->DRIVEDFILE, mbkstrdup( name ) );
    }
  else
    file=context->OUTPUT_SPICE_FILE_POINTER;

  context->PTF = file;

  if (context->OUTPUT_SPICE_FILE_POINTER==NULL)
    sim_run_simu_drive_header(file, context);

  sim_run_simu_spice_drive_x( context );

  spi_print( file, "* Main supply *************************************\n\n" );
  sim_run_simu_spice_drive_alim( context );

  spi_print( file, "* Current measure *********************************\n\n" );
  sim_run_simu_spice_drive_measure_current( context );

  spi_print( file, "* Initial state ***********************************\n\n" );
  sim_run_simu_spice_drive_ic( context );

  spi_print( file, "* User specified input ****************************\n\n" );
  sim_run_simu_spice_drive_delta( context );
  spi_print( file, "\n" );

  spi_print( file, "* Input stimuli ***********************************\n\n" );
  sim_run_simu_spice_drive_input( context );

  spi_print( file, "* Measure *****************************************\n\n" );
  sim_run_simu_spice_drive_measure( context );

  spi_print( file, "* Spice Measure ***********************************\n\n" );
  sim_run_simu_spice_drive_spice_measure(context);

  if ( SIM_SPICE_OPTIONS ) {

    /* check if the string is non empty */
    pt=SIM_SPICE_OPTIONS ;
    while( *pt && *pt==' ' ) pt++ ;

    /* if the string is non empty, write option in file */
    if( *pt ) {
      spi_print( file, "* Options *****************************************\n\n" );
      spi_print( file, ".option %s\n",SIM_SPICE_OPTIONS);
    }
  }

  if (( tool == SIM_TOOL_TITAN ) || ( tool == SIM_TOOL_TITAN7 )) {
    if (context->OUTPUT_SPICE_FILE_NAME==NULL && context->OUTPUT_SPICE_FILE_POINTER==NULL)
       spi_print( file, ".save cmd_%s\n",context->MODEL->FIGNAME);
    else if (context->OUTPUT_SPICE_FILE_POINTER!=NULL)
       spi_print( file, ".save %s\n",context->OUTPUT_SPICE_FILE_NAME);
    else 
       spi_print( file, ".save cmd_%s\n",context->OUTPUT_SPICE_FILE_NAME);
  }
  if( sim_parameter_get_simu_step( model ) == SIM_UNDEF ) {
    spi_print( file, ".tran %g%s %g%s\n", sim_parameter_get_tran_step( model ),
                                        !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_TIME,
                                        sim_parameter_get_tran_tmax( model ),
                                        !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_TIME             );
  }
  else {
    spi_print( file, ".tran %g%s %g%s\n", sim_parameter_get_simu_step( model ),
                                        !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_TIME,
                                        sim_parameter_get_tran_tmax( model ),
                                        !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":SIM_UNIT_LABEL_SPICE_TIME             );
  }

  if( model->NBMC > 0 )
    {
      switch( sim_parameter_get_tool( model ) )
        {
        case SIM_TOOL_ELDO: 
        case SIM_TOOL_HSPICE: 
        case SIM_TOOL_TITAN7: 
          break;
        case SIM_TOOL_TITAN: 
          spi_print( file, ".mc runs = %d\n", model->NBMC );
          break;
        case SIM_TOOL_LTSPICE:
        case SIM_TOOL_NGSPICE:
        case SIM_TOOL_SPICE:
          spi_print( file, ".step param montecarlo_run_var 1 %d 1\n", model->NBMC );
          break;
        break;
        }
    }

  spi_print( file, ".end\n" );

  if (context->OUTPUT_SPICE_FILE_POINTER==NULL)
    fclose( file );

  return 1;
}

/******************************************************************************\

Calcul les points correspondant à n'importe quelle fonction mathématique.
L'argument 'result' est un tableau qui contiendra les résultats sous la forme
de couples (x,y). 'nbelem' doit contenir la taille maximum du tableau, et en
sortie il contient le nombre de valeurs dans le tableau.

\******************************************************************************/

void sim_calc_func( sim_model *model, 
                    sim_input *input, 
                    SIM_FLOAT *result, 
                    int *nbelem )
{
  SIM_FLOAT  t;
  SIM_FLOAT  tmax;
  SIM_FLOAT  step;
  void      *userdata;
  SIM_FLOAT (*call)(SIM_FLOAT t, void *data);
  int        maxelem;

  SIM_FLOAT  t1, t2, v1, v2, v3e, v3r, a, b ;
  SIM_FLOAT  st, sv ; /* prendre en compte un signal initialement stable */
  SIM_FLOAT  epsilon, vf;

  maxelem = *nbelem;
  *nbelem = 0;
  
  sim_input_get_func( input, &call, &userdata ) ;
  tmax    = sim_parameter_get_tran_tmax( model ) ;
  step    = sim_parameter_get_tran_step( model ) ;
  epsilon = sim_parameter_get_alim( model, 'i' ) / 100.0 ;
  
  t1 = 0.0 ;              v1 = call( t1, userdata );
  t2 = t1+step ;          v2 = call( t2, userdata );
  a  = (v2-v1)/(t2-t1) ;  b  = v1-a*t1;
 
  result[ (*nbelem)++ ] = t1;
  result[ (*nbelem)++ ] = v1;
  st = t1 ; sv = v1 ;

  vf=call( tmax, userdata );

  
  if( fabs( v2-v1 ) > epsilon ) {
    result[ (*nbelem)++ ] = t2;
    result[ (*nbelem)++ ] = v2;
    st = -1.0 ;
  }

  for( t = t2+step ; t <= tmax && *nbelem < maxelem  ; t = t + step ) {
    v3e = a*t+b;
    v3r = call( t, userdata );

    if (fabs(vf-v3r) <= epsilon) {
      result[ (*nbelem)++ ] = t;
      result[ (*nbelem)++ ] = vf;
      break;
    } else if( fabs( v3e-v3r ) > epsilon ) {
      if( st > 0.0 ) {
        if( fabs( v3r - sv ) > epsilon ) {
          result[ (*nbelem)++ ] = st;
          result[ (*nbelem)++ ] = sv;
        }
        st = -1.0 ;
      }
      if( *nbelem < maxelem ) {
        result[ (*nbelem)++ ] = t;
        result[ (*nbelem)++ ] = v3r;
      }
      t1=t2; v1=v2;
      t2=t; v2=v3r;
      a=(v2-v1)/(t2-t1); b=v1-a*t1;
    }
    else {
      if( st >= 0.0 ) {
        st = t ;
        sv = v3r ;
      }
    }
  }
}

/******************************************************************************\

Crée les sources de tension externes au subckt. Cela concerne les mesures de
courant sur les connecteurs du subckt, ou dans le cas de ngspice les mesures
de courant dans le subckt.  Les mesures à réaliser ont été mémorisées dans la 
structure CURRENT par les fonctions sim_run_simu_spice_drive_pin() et
sim_run_simu_spice_drive_ngspice_current().

\******************************************************************************/

void sim_run_simu_spice_drive_measure_current( drive_spice *context )
{
  drive_current *scan;
  char           tmp[BUFSIZ];
  
  for( scan = context->CURRENT ; scan ; scan = scan->NEXT ) {
    sprintf( tmp, "Vimeas%d", context->NBCURRENT++ );
    spi_print( context->PTF, "%s %s %s dc 0%s\n", tmp,
                                                 scan->PINNAME,
                                                 scan->MEASPINNAME,
                                                 !V_BOOL_TAB[__SPI_DRIVE_USE_UNITS].VALUE?"":"v"
             );
    sim_measure_set_detail( scan->MEAS, tmp,tmp );
  }
  spi_print( context->PTF, "\n" );
}

/******************************************************************************\

Cette fonction est à utiliser lorsqu'on veut mesurer le courant entrant dans le 
circuit. Au lieu de référencer directement un noeud de l'interface de 
l'instance, on passe par cette fonction qui renvoie éventuellement un autre 
noeud si jamais une mesure existe sur ce noeud. Dans ce dernier cas, on
note les deux noeuds dans la structure CURRENT, et on drivera plus tards les
générateurs de tension nulle, avec la fonction 
sim_run_simu_spice_drive_measure_current().

\******************************************************************************/

char* sim_run_simu_spice_drive_pin( char *loconname, 
                                    char *pinname,
                                    drive_spice *context )
{
  sim_measure   *imeas;
  drive_current *new;
  char          tmp[BUFSIZ];

  if( !loconname ) return pinname;
  
  imeas = sim_measure_get( context->MODEL,
                           loconname,
                           SIM_MEASURE_LOCON,
                           SIM_MEASURE_CURRENT
                         );
                         
  if( !imeas ) return pinname;

  new = mbkalloc( sizeof( drive_current ) );
  new->NEXT = context->CURRENT ;
  context->CURRENT = new;

  new->PINNAME = mbkstrdup( pinname );

  sprintf( tmp, "pinmeas%d", context->NBPINMEAS++ );
  
  new->MEASPINNAME = mbkstrdup( tmp );
  new->MEAS = imeas;

  return new->MEASPINNAME;
}

/******************************************************************************\

Crée une chaine de caractère correspondant à un noeud dans le fichier de
commande Spice drivé. 
Si losig vaut NULL, seule la hiérarchie est créée.

\******************************************************************************/

void sim_run_simu_spice_name_signal( sim_model    *model,
                                     chain_list   *headins,
                                     losig_list   *losig,
                                     int           pnode,
                                     char          force,
                                     char         *hiername
                                   )
{
  chain_list *chins ;
  loins_list *loins ;
  char tmpbuf[BUFSIZ];
  char driveexternal;
  chain_list *chain ;
  num_list   *num ;
  ptype_list *ptl ;
  locon_list *locon ;

  /* Si on est au top level, et que le noeud est externe, on le référence 
     directement sur l'exterieur */
  driveexternal = NO;
  if( !headins && losig->TYPE=='E' ) {
    if( pnode == SPI_NONODES )
      driveexternal = YES;
    else {
      ptl = getptype( losig->USER, LOFIGCHAIN );
      if( ptl ) {
        for( chain = (chain_list*)ptl->DATA ; chain ; chain = chain->NEXT ) {
          locon = (locon_list*)chain->DATA;
          if( locon->TYPE == 'E' )
            break;
          locon=NULL;
        }
        if( locon && locon->PNODE ) {
          for( num = locon->PNODE ; num ; num = num->NEXT ) {
            if( num->DATA == pnode )
              break;
          }
          if( num )
            driveexternal = YES;
        }
      }
    }
  }
  
  /* Nom de la figure */
  if( driveexternal == NO ) {
    switch( sim_parameter_get_tool( model ) )
    {
      case SIM_TOOL_ELDO: 
      case SIM_TOOL_HSPICE: 
      case SIM_TOOL_MSPICE: 
      case SIM_TOOL_NGSPICE:
      case SIM_TOOL_TITAN7: 
        sprintf( hiername, "x%s%c", 
                 sim_model_get_lofig( model )->NAME,
                 SPI_INS_SEPAR
               );
        break;
      case SIM_TOOL_TITAN: 
        sprintf( hiername, "%cx%s", 
                 SPI_INS_SEPAR,
                 sim_model_get_lofig( model )->NAME
               );
        break;
      case SIM_TOOL_LTSPICE:
      case SIM_TOOL_SPICE:
        sprintf( hiername, "%s:", 
                 sim_model_get_lofig( model )->NAME
               );
        break;
    }
  }
  else {
    hiername[0]='\0';
  }
  
  /* Voyage au travers de la hiérarchie */

  for( chins = headins ; chins ; chins = chins->NEXT ) {
    loins = (loins_list*)(chins->DATA);
    switch( sim_parameter_get_tool( model ) )
    {
    case SIM_TOOL_ELDO: 
    case SIM_TOOL_HSPICE: 
    case SIM_TOOL_MSPICE: 
    case SIM_TOOL_NGSPICE:
    case SIM_TOOL_TITAN7:
      sprintf( tmpbuf, "%sx%s.", hiername, loins->INSNAME );
      strcpy( hiername, tmpbuf );
      break;
    case SIM_TOOL_TITAN: 
      sprintf( tmpbuf, ".x%s%s", loins->INSNAME, hiername );
      strcpy( hiername, tmpbuf );
      break;
    case SIM_TOOL_LTSPICE:
    case SIM_TOOL_SPICE:
      sprintf( tmpbuf, "%s%s:", hiername, loins->INSNAME );
      strcpy( hiername, tmpbuf );
      break;
    }
  }

  if( losig ) {
    /* Création de noeud */
          
    switch( sim_parameter_get_tool( model ) )
    {
    case SIM_TOOL_ELDO: 
    case SIM_TOOL_HSPICE: 
    case SIM_TOOL_MSPICE: 
    case SIM_TOOL_SPICE:
    case SIM_TOOL_NGSPICE:
    case SIM_TOOL_LTSPICE:
    case SIM_TOOL_TITAN7: 
      sprintf( tmpbuf, "%s%s", 
               hiername,
               spinamenodedetail( losig, pnode, force )
             );
      strcpy( hiername, tmpbuf );
      break;
    case SIM_TOOL_TITAN: 
      sprintf( tmpbuf, "%s%s", 
               spinamenodedetail( losig, pnode, force ),
               hiername
             );
      strcpy( hiername, tmpbuf );
      break;
    }
  }
}

/******************************************************************************\
Lorsqu'on doit faire référence à un signal sans plus d'information, renvoie
un noeud "significatif" sur ce signal.
\******************************************************************************/

int sim_run_simu_spice_get_one_node( losig_list *losig )
{
  ptype_list *ptl;
  chain_list *chain;
  locon_list *locon;
  locon_list *found=NULL;
  int         node=0;
  
  if( !losig->PRCN )
    return SPI_NONODES;
  
  ptl = getptype( losig->USER, LOFIGCHAIN );
  if( ptl ) {
  
    if( losig->TYPE == EXTERNAL ) {
    
      for( chain = (chain_list*)ptl->DATA ; chain ; chain = chain->NEXT ) {
        locon = (locon_list*)chain->DATA;
        if( locon->TYPE == EXTERNAL && locon->PNODE ) {
          found = locon ;
          break ;
        }
      }
    }
    
    if( !found ) {
      for( chain = (chain_list*)ptl->DATA ; chain ; chain = chain->NEXT ) {
        locon = (locon_list*)chain->DATA;
        if( locon->PNODE ) {
          found = locon ;
          break;
        }
      }
    }
  }

  if( found )
    node = locon->PNODE->DATA ;
   
  if( !node ) {
    if( losig->PRCN->PWIRE ) {
      node = losig->PRCN->PWIRE->NODE1 ;
    }
  }

  if( !node ) {
    if( losig->PRCN->PCTC ) {
      node = rcn_ctcnode( (loctc_list*) losig->PRCN->PCTC->DATA, losig );
    }
  }

  if( !node )
    node = SPI_NONODES ;

  return node ;
}

void sim_run_simu_spice_drive_delta( drive_spice *context )
{
  sim_input *input=NULL;
  sim_model *model;
  
  model = context->MODEL ;
  while( (input = sim_input_scan( model, input ) ) ) {
    if( input->DELTAVAR ) {
      spi_print( context->PTF, ".param %s=0.0\n", input->DELTAVAR );
    }
  }
}


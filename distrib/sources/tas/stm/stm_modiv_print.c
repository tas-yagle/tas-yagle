/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modiv_print.c                                           */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Grégoire Avot                                             */
/*                                                                          */
/****************************************************************************/

#include "stm.h"

void stm_modiv_print( FILE *f, timing_iv *iv )
{
  int ne;
  int ns;

  fprintf( f, "\t\tiv (\n" );
 
  fprintf( f, "\t\t\tnve ( %d )\n", iv->NVE );
  fprintf( f, "\t\t\tnvs ( %d )\n", iv->NVS );
 
  fprintf( f, "\t\t\tve ( " );
  for( ne = 0 ; ne < iv->NVE ; ne++ ) {
    fprintf( f, "%g ", iv->VE[ne] );
  }
  fprintf( f, ")\n" );

  fprintf( f, "\t\t\tvs ( " );
  for( ns = 0 ; ns < iv->NVS ; ns++ ) {
    fprintf( f, "%g ", iv->VS[ns] );
  }
  fprintf( f, ")\n" );

  fprintf( f, "\t\t\tcurrent (\n" );
  for( ne = 0 ; ne < iv->NVE ; ne++ ) {
    fprintf( f, "\t\t\t    ( " );
    for( ns = 0 ; ns < iv->NVS ; ns++ ) {
      fprintf( f, "%g ", stm_modiv_getis( iv, ne, ns ) );
    }
    fprintf( f, ")\n" );
  }
  fprintf( f, "\t\t\t)\n" );
  
  fprintf( f, "\t\t\tconf ( %.5g %.5g %.5g %.5g)\n", iv->IV_CONF.PCONF0,
                                                     iv->IV_CONF.PCONF1,
                                                     iv->IV_CONF.IRAP,
                                                     iv->IV_CONF.CI
         );
  fprintf( f, "\t\t\tinput ( %.5g %.5g %.5g %.5g)\n", iv->IV_INPUT.VT,
                                                      iv->IV_INPUT.VI,
                                                      iv->IV_INPUT.VF,
                                                      iv->IV_INPUT.VTH
         );
  fprintf( f, "\t\t\tinitial ( %.5g)\n", iv->IV_INIT.VI );
  fprintf( f, "\t\t\t)\n" );
  fprintf( f, "\t\t   )\n" );
}

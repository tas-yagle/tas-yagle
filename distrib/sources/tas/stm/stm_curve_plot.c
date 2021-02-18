/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_curve_plot.c                                            */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gregoire Avot, Olivier Bichaut                            */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include "stm.h"

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

void stm_curve_plot_filename( stm_curve *curve, 
                              char *nameout, 
                              char *suffix, 
                              char *buf 
                            )
{
  int i ;
  if(nameout == NULL)
    nameout = curve->MODEL->NAME ;

  strcpy(buf,nameout) ;
  if( suffix )
    strcat( buf, suffix );

  for(i = 0 ; buf[i] ; i++)
     if(buf[i] == '/')
       buf[i] = '_' ;
}

int stm_curve_plot( stm_curve *curve , char *nameout )
{
  FILE *file ;
  char  buf[1024] ;
  int   i ;

  if( !curve ) 
    return 0;
    
  stm_curve_plot_filename( curve, nameout, NULL, buf );

  file = mbkfopen(buf,"dat",WRITE_TEXT) ;

  fprintf(file,"# time\n") ;

  for(i = 0 ; i < curve->TIME.NBPOINTS ; i++)
    {
     fprintf(file," %g %g %g %g %g\n",(curve->TIME.DATA+i)->T*1.0e-12,
                                   (curve->TIME.DATA+i)->VE,
                                   (curve->TIME.DATA+i)->VS,
                                   (curve->TIME.DATA+i)->I,
                                   (curve->TIME.DATA+i)->ICONF) ;
    }

  fclose(file) ;

  file = mbkfopen(buf,"plt",WRITE_TEXT) ;

  fprintf(file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
  fprintf(file,"set xlabel \"TIME (PS)\"\n") ;
  fprintf(file,"set ylabel \"VOLTAGE (V)\"\n") ;
  fprintf(file,"plot [0.0:%g] ",curve->TMAX*1e-12) ;
  fprintf(file,"'%s.dat' using 1:2 title 'VE' with lines,",buf) ; 
  fprintf(file,"'%s.dat' using 1:3 title 'VS' with lines\n",buf) ; 
  fprintf(file,"pause -1 'Hit CR to finish'\n") ;
  fprintf(file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
  fprintf(file,"set xlabel \"TIME (PS)\"\n") ;
  fprintf(file,"set ylabel \"IDS (A)\"\n") ;
  fprintf(file,"plot [0.0:%g] ",curve->TMAX*1e-12) ;
  fprintf(file,"'%s.dat' using 1:4 title 'IDS' with lines,",buf) ; 
  fprintf(file,"'%s.dat' using 1:5 title 'ICONF' with lines\n",buf) ; 
  fprintf(file,"pause -1 'Hit CR to finish'\n") ;

  fclose(file) ;

  stm_curve_plot_static( curve, nameout );
  return 0;
}

int stm_curve_plot_static( stm_curve *curve, char *nameout )
{
  FILE *file ;
  char  buf[1024] ;
  char  bufve[1024] ;
  char  bufvs[1024] ;
  int   ns,ne;

  // i = f(ve)

  stm_curve_plot_filename( curve, nameout, "_i=f(ve)", bufve );

  file = mbkfopen(bufve,"dat",WRITE_TEXT) ;

  fprintf(file,"# i = f(ve)|vs=cte\n") ;

  for( ne = 0 ; ne < curve->STATIC.NBVE ; ne++ ) {
    fprintf( file, "%g", curve->STATIC.VE[ne] );
    for( ns = 0 ; ns < curve->STATIC.NBVS ; ns++ ) {
      fprintf( file, " %g", *(STM_CURVE_STATIC_GET_PTR_I(curve,ne,ns)) );
    }
    fprintf( file, "\n" );
  }

  fclose(file) ;
  
  // i = f(vs)

  stm_curve_plot_filename( curve, nameout, "_i=f(vs)", bufvs );

  file = mbkfopen(bufvs,"dat",WRITE_TEXT) ;

  fprintf(file,"# i = f(vs)|ve=cte\n") ;

  for( ns = 0 ; ns < curve->STATIC.NBVS ; ns++ ) {
    fprintf( file, "%g", curve->STATIC.VS[ns] );
    for( ne = 0 ; ne < curve->STATIC.NBVE ; ne++ ) {
      fprintf( file, " %g", *(STM_CURVE_STATIC_GET_PTR_I(curve,ne,ns)) );
    }
    fprintf( file, "\n" );
  }

  fclose(file) ;

  // Commandes

  stm_curve_plot_filename( curve, nameout, "_static", buf );
  file = mbkfopen( buf, "plt", WRITE_TEXT) ;

  fprintf(file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE) ;
  fprintf(file,"set xlabel \"Voltage VE (V)\"\n") ;
  fprintf(file,"set ylabel \"Current (A)\"\n") ;
  fprintf(file,"plot ");
  for( ns = 0 ; ns < curve->STATIC.NBVS ; ns++ ) {
    if( ns ) fprintf(file,", ");
    fprintf(file,"'%s.dat' using 1:%d title 'vs=%.2g' with lines", bufve, ns+2, curve->STATIC.VS[ns] );
  }
  fprintf(file,"\n");
  fprintf(file,"pause -1 'Hit CR to finish'\n") ;
  fprintf(file,"set xlabel \"Voltage VS (V)\"\n") ;
  fprintf(file,"plot ");
  for( ne = 0 ; ne < curve->STATIC.NBVE ; ne++ ) {
    if( ne ) fprintf(file,", ");
    fprintf(file,"'%s.dat' using 1:%d title 've=%.2g' with lines", bufvs, ne+2, curve->STATIC.VE[ne] );
  }
  fprintf(file,"\n");
  fprintf(file,"pause -1 'Hit CR to finish'\n") ;

  fclose(file) ;

  return 0;
}

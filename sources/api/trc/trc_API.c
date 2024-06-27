#include AVT_H
#include API_H
#include MUT_H
#include MLO_H
#include RCN_H
#include TRC_H
#define API_USE_REAL_TYPES
#include "trc_API.h"
#include STM_H

int trc_GetDelayAWE( Netlist *netlist, Connector *begin, Connector *end, double smax, double smin, double *tmax, double *tmin, double *fmax, double *fmin )
{
  losig_list *losig;
  rcx_slope   slopemax;

  losig = ((locon_list*)begin)->SIG;

  smax = smax * 1.0e12;
  smin = smin * 1.0e12;
  
  RCX_PTRTESTANDSET( tmax, -1.0 );
  RCX_PTRTESTANDSET( tmin, -1.0 );
  RCX_PTRTESTANDSET( fmax, -1.0 );
  RCX_PTRTESTANDSET( fmin, -1.0 );

  if( !losig->PRCN || !losig->PRCN->PWIRE ) return 0;
  if( chkloop( losig ) ) return 0;
  if( smax < 0.0 || smin < 0.0 ) return 0;
  if( rcx_crosstalkactive( RCX_QUERY ) != RCX_NOCROSSTALK ) return 0;

  slopemax.SENS   = TRC_SLOPE_UP ;
  slopemax.F0MAX  = smax ;
  slopemax.FCCMAX = smax ;
  slopemax.CCA    = -1.0 ;
  slopemax.MIN.slope = smin ;            slopemax.MAX.slope = smax ;
  slopemax.MIN.slnrm = smin ;            slopemax.MAX.slnrm = smax ;
  slopemax.MIN.vend  = 5.0 ;             slopemax.MAX.vend  = 5.0 ;
  slopemax.MIN.vt    = 0.7 ;             slopemax.MAX.vt    = 0.7 ;
  slopemax.MIN.vdd   = 5.0 ;             slopemax.MAX.vdd   = 5.0 ;
  slopemax.MIN.vsat  = -1.0 ;            slopemax.MAX.vsat  = -1.0 ;
  slopemax.MIN.rlin  = -1.0 ;            slopemax.MAX.rlin  = -1.0 ;
  slopemax.MIN.vth   = 2.5 ;             slopemax.MAX.vth   =  2.5 ;
  slopemax.MIN.r     = -1.0 ;            slopemax.MAX.r     = -1.0 ;
  slopemax.MIN.c1    = -1.0 ;            slopemax.MAX.c1    = -1.0 ;
  slopemax.MIN.c2    = -1.0 ;            slopemax.MAX.c2    = -1.0 ;
  slopemax.MIN.pwl   = NULL ;            slopemax.MAX.pwl   = NULL ;

  getawedelay( ((lofig_list*)netlist),
               losig,
               ((locon_list*)begin),
               ((locon_list*)end),
               &slopemax,
               5.0,
               5.0,
               0.0,
               tmax,
               tmin,
               fmax,
               fmin,
               NULL,
               NULL
             );
  RCX_PTRTESTANDSET( tmax, *tmax * 1.0e-12 );
  RCX_PTRTESTANDSET( tmin, *tmin * 1.0e-12 );
  RCX_PTRTESTANDSET( fmax, *fmax * 1.0e-12 );
  RCX_PTRTESTANDSET( fmin, *fmin * 1.0e-12 );
  return 1;
}

Netlist* trc_GetRcxFile( char *figname )
{
  lofig_list *lofig ;

  avtenv();
  mbkenv();
  rcx_env();
  stmenv();

  figname = namealloc( figname );

  lofig = addlofig( figname );

  printf( "reading rcx file\n" );
  if( !rcxparse( NULL, lofig, figname, NULL ) ) {
    printf( "an error occured when parsing file.\n" );
    return(0);
  }
  printf( "file parsed.\n" );

  return lofig ;
}

int trc_DisplayDelayForAllNet( Netlist *netlist, int level )
{
  losig_list *losig ;

  for( losig = ((lofig_list*)netlist)->LOSIG ; losig ; losig = losig->NEXT )
    trc_delayforlosig( ((lofig_list*)netlist), losig, 0, level );
  return 1 ;
}

void trc_DisplayDelayForLosig( Netlist *netlist, char *netname )
{
  losig_list *losig ;

  netname = namealloc( netname );
  losig = rcx_gethtrcxsig( NULL, ((lofig_list*)netlist), netname );
  if( !losig ) {
    printf( "can't find signal %s\n", netname );
    return ;
  }
  trc_delayforlosig( ((lofig_list*)netlist), losig, 1, 1 );
}

void trc_DisplayFigureSize( Netlist *netlist ) 
{
  rcx_displaymemoryusage( (lofig_list*)netlist );
}

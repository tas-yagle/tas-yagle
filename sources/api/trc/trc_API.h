#include "avt_API_types.h"
/*
  MAN trc_GetDelayAWE
  DESCRIPTION
  trc_GetDelayAWE calculate a delay on an interconnect (without loop of resistance) using the AWE algorithm. smin and smax are input slope, tmin, tmax, fmin and fmax are pointer on variable where delays and slopes are stored. NULL value is allowed.
  RETURN VALUE
  trc_GetDelayAWE return 1 if no error occured, else 0.
  ERRORS
  none.
*/
int trc_GetDelayAWE( Netlist *netlist, Connector *begin, Connector *end, double smax, double smin, double *tmax, double *tmin, double *fmax, double *fmin );
int trc_DisplayDelayForAllNet( Netlist *netlist, int level ) ;
void trc_DisplayFigureSize( Netlist *netlist ) ;
Netlist* trc_GetRcxFile( char *figname ) ;
void trc_DisplayDelayForLosig( Netlist *netlist, char *netname ) ;

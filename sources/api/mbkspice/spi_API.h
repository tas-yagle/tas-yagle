#include "avt_API_types.h"

/*
MAN spi_DriveSignal
CATEG gns
DESCRIPTION
spi_DriveSignal drive the RC description of losig in a file in Spice compatible format.
RETURN VALUE
spi_DriveSignal return 1 if no error occured, else 0.
ERRORS
none.
*/

int spi_DriveSignal( Signal *losig );
void drive_spice (Netlist *fig, char *filename);

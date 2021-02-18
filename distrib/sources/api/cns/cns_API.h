#include <stdio.h>
#include "avt_API_types.h"

/*
  MAN cns_LoadConeFigure
  DESCRIPTION
  RETURN VALUE
  ERRORS
*/

ConeFigure *cns_LoadConeFigure(char *name);

/*
  MAN cns_GetConeFigure
  DESCRIPTION
  RETURN VALUE
  ERRORS
*/

ConeFigure *cns_GetConeFigure(char *name);

/*
  MAN cns_SaveVerboseConeFile
  DESCRIPTION
  RETURN VALUE
  ERRORS
*/

void cns_SaveVerboseConeFile(ConeFigure *);

/*
  MAN cns_DisplayPowerSupplies
  DESCRIPTION
  RETURN VALUE
  ERRORS
*/

void cns_DisplayPowerSupplies(FILE *f, ConeFigure *);
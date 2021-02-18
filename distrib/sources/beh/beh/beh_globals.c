
/* ###--------------------------------------------------------------### */
/* file		: beh_globals.c						*/
/* date		: Aug 27 1997						*/
/* version	: v110							*/
/* authors	: Pirouz BAZARGAN SABET					*/
/* content	: low-level function					*/
/* ###--------------------------------------------------------------### */

#include <stdio.h>
#include MUT_H
#include LOG_H
#include BEH_H

unsigned int   BEH_LINNUM     = 1         ;

unsigned char  BEH_TIMEUNIT   = BEH_TU__PS;
float          BEH_CNV_FS     = 1.0E-3    ;
float          BEH_CNV_PS     = 1.0       ;
float          BEH_CNV_NS     = 1.0E3     ;
float          BEH_CNV_US     = 1.0E6     ;
float          BEH_CNV_MS     = 1.0E9     ;

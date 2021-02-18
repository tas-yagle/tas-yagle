#include "avt_API_types.h"

#define TmgCell   void
#define TmgTable  void
#define TmgModel  void

#define STM_NOTYPE           'N'
#define STM_LOAD             'L'
#define STM_INPUT_SLEW       'I'
#define STM_CLOCK_SLEW       'C'

/*
 MAN 
 DESCRIPTION
 RETURN VALUE
 none.
 ERRORS
 none.
*/
TmgCell  *stm_AddCell (char *cellname);

TmgModel *stm_ModCreateTable (char *name, int nx, int ny, char xtype, char ytype);

void stm_ModTblSetXrangeVal (TmgModel *model, int xpos, double value);

void stm_ModTblSetYrangeVal (TmgModel *model, int ypos, double value);

void stm_ModTblSetXrange (TmgModel *model, List *xrange);

void stm_ModTblSetYrange (TmgModel *model, List *yrange);

void stm_ModTblSet2Dval (TmgModel *model, int xpos, int ypos, double value);

char *stm_CellAddModel (TmgCell *cell, TmgModel *model, int noshare);

void stm_DriveCell (char *cellname);

/*
 MAN stm_ModTblAddRangeVal
 CATEG gns
 DESCRIPTION
 stm_ModTblAddRangeVal adds a pointer on a double value into a 'List', it allows
 to add a value on a look up table axis.
 RETURN VALUE
 stm_ModTblAddRangeVal returns a 'List'.
 ERRORS
 none.
*/
extern List *stm_ModTblAddRangeVal (List *range, double value);

/*
 MAN stm_ModTblCreateRange
 CATEG gns
 DESCRIPTION
 stm_ModTblCreateRange creates a range which is a list of pointer
 on a double value representing a value of an axis.
 'range' is a string containing the differents value of the range separted
 by the caracter ':'.
 Example : to add the value 2.0 and 3.5 to a range, use stm_ModTblCreateRange("2.0:3.5") 
 RETURN VALUE
 stm_ModTblCreateRange returns a 'List'.
 ERRORS
 none.
*/
extern List *stm_ModTblCreateRange (char *range);

/*
 MAN stm_FreeTblRange
 CATEG gns
 DESCRIPTION
 stm_FreeTblRange frees a 'List' that contains an item which is a pointer on
 a double type.
 RETURN VALUE
 none.
 ERRORS
 none.
*/
extern void  stm_FreeTblRange (List *range);

void SET_DELAY(double value);
void SET_SLOPE(double value);
double GET_OUTPUT_CAPA();
double GET_INPUT_SLOPE();
double GET_COMMAND_INPUT_SLOPE();
double GET_DELAY();
double GET_SLOPE();
void CALL_SIMULATION();
void CALL_SIMULATION_ENV();
void CALL_CTK_ENV();

/*
 MAN 
 DESCRIPTION
 RETURN VALUE
 none.
 ERRORS
 none.
*/
extern void stm_createCache();

extern double stm_getSlope   (double inslope, double outload);

extern double stm_getDelay   (double inslope, double outload);

/*
 MAN 
 DESCRIPTION
 RETURN VALUE
 none.
 ERRORS
 none.
*/
extern void stm_storeSlope (double inslope, double outload, double slope);

/*
 MAN 
 DESCRIPTION
 RETURN VALUE
 none.
 ERRORS
 none.
*/
extern void stm_storeDelay (double inslope, double outload, double delay);

/*
 MAN stm_delayThresholdOK
 CATEG gns
 DESCRIPTION
 Set the slot in the current delay cache for inslope and outload.
 It's possible to have a threshold of tolerence.
 Finally Test in the current slot if there is a value stored 
 RETURN VALUE
 Return 0 if the slot is empty, 1 otherwise. 
 ERRORS
 none.
 SEE ALSO
 stm_sup_storeDelay, stm_sup_getDelay.
*/
extern int stm_delayThresholdOK(double inslope, double outload, double threshold);

/*
 MAN stm_slopeThresholdOK
 CATEG gns
 DESCRIPTION
 Set the slot in the current slope cache for inslope and outload.
 It's possible to have a threshold of tolerence.
 Finally Test in the current slot if there is a value stored 
 RETURN VALUE
 Return 0 if the slot is empty, 1 otherwise. 
 ERRORS
 none.
 SEE ALSO
 stm_sup_storeSlope, stm_sup_getSlope.
*/
extern int stm_slopeThresholdOK(double inslope, double outload, double threshold);

/*
 MAN stm_sup_storeDelay
 CATEG gns
 DESCRIPTION
 Store delay to the current cache slot, specified by the previous call of
 stm_delayThresholdOK.
 RETURN VALUE
 none.
 ERRORS
 none.
 SEE ALSO
 stm_delayThresholdOK.
*/
extern void stm_sup_storeDelay(double delay);

/*
 MAN stm_sup_storeSlope
 CATEG gns
 DESCRIPTION
 Store slope to the current cache slot, specified by the previous call of
 stm_slopeThresholdOK.
 RETURN VALUE
 none.
 ERRORS
 none.
 SEE ALSO
 stm_slopeThresholdOK.
*/
extern void stm_sup_storeSlope(double slope);

/*
 MAN stm_sup_getDelay
 CATEG gns
 DESCRIPTION
 Get the stored delay from the current cache slot, specified by the previous
 call of stm_delayThresholdOK.
 RETURN VALUE
 The expected delay.
 ERRORS
 none.
 SEE ALSO
 stm_delayThresholdOK.
*/
extern double stm_sup_getDelay();

/*
 MAN stm_sup_getSlope
 CATEG gns
 DESCRIPTION
 Get the stored slope from the current cache slot, specified by the previous
 call of stm_slopeThresholdOK.
 RETURN VALUE
 The expected slope.
 ERRORS
 none.
 SEE ALSO
 stm_slopeThresholdOK.
*/
extern double stm_sup_getSlope();

/*
 MAN 
 DESCRIPTION
 RETURN VALUE
 none.
 ERRORS
 none.
*/
void stm_printVariation(FILE *fp, double real, double found);

/*
 MAN 
 DESCRIPTION
 RETURN VALUE
 none.
 ERRORS
 none.
*/
extern void SET_DELAY_AND_SLOPE(char *in, char *out, char *type, double delay, double slope);

/*
 MAN 
 DESCRIPTION
 RETURN VALUE
 none.
 ERRORS
 none.
*/
extern void stm_API_ActionTerminate();

/*
 MAN stm_arround 
 CATEG gns
 DESCRIPTION
 Round by truncating with the given precision.
 RETURN VALUE
 The rounded value.
 ERRORS
 none.
*/
extern double stm_arround(double a, double precision);

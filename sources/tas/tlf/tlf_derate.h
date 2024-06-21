/****************************************************************************/
/*     defines                                                              */
/****************************************************************************/

/* Transition to compute derating in Process, Temperature and Voltage       */
#define TLF_FALLING  0  /* Falling transition                               */
#define TLF_RISING   1  /* Rising  transition                               */ 


/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

extern double tlf_derate_temp(double T0, double T1, int transition) ;
extern double tlf_derate_tension(double V0, double V1, int transition) ;
extern void   tlf_drive_derate(FILE *f, double mintemp, double typtemp, 
                               double maxtemp, double minvolt, double typvolt, 
                               double maxvolt) ;

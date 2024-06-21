/****************************************************************************/
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*                                                                          */
/*    Title         : VCD Parser engine                                     */
/*    Last modified : 2003-07-17                                            */
/*                                                                          */
/****************************************************************************/

typedef struct WScan_EventList WScan_EventList;
typedef struct WScan_SignalList WScan_SignalList;

/*==========================================================================*/
/* DataBase                                                                 */
/*==========================================================================*/
extern int WScan_ParseFiles (char *vcd_file, char *cor_file);
/* Creates the VCD DataBase from the .vcd file and the .cor file            */
/* Returns 0 if successfull, 1 otherwise                                    */

/*--------------------------------------------------------------------------*/
extern void WScan_FreeDB ();
/* Frees the DataBase                                                       */

/*==========================================================================*/
/* Signals                                                                  */
/*==========================================================================*/
extern WScan_SignalList *WScan_GetSignalsList ();
/* Returns the list of signals in the VCD DataBase. These signals are those */
/* present in the .vcd file and mapped thru the correspondaces of the .cor  */
/* file.                                                                    */

/*--------------------------------------------------------------------------*/
extern WScan_SignalList *WScan_GetNextSig (WScan_SignalList *sig);
/* Returns the next signal in the list of signals, NULL if EOL              */

/*--------------------------------------------------------------------------*/
extern char *WScan_GetSigName (WScan_SignalList *sig);
/* Returns the name of a signal from the signals list                       */

/*==========================================================================*/
/* Time                                                                     */
/*==========================================================================*/
extern void WScan_GoToInitTime ();
/* Sets the current simulation time to -1 (init time)                       */  

/*--------------------------------------------------------------------------*/
extern void WScan_GoToNextTime ();
/* Sets the current simulation time to the next time where events are       */
/* occuring. If end of simulation reached, time is not incremented,         */
/* i.e next time == current time                                            */

/*--------------------------------------------------------------------------*/
extern long WScan_GetTime ();
/* Returns the current simulation time                                      */ 

/*==========================================================================*/
/* Events                                                                   */
/*==========================================================================*/
extern WScan_EventList *WScan_GetEvents ();
/* Returns the list of events occuring at the current simulation time.      */
/* (As it duplicates an internal list, it should be freed with the          */
/* WScan_FreeEvents function)                                               */

/*--------------------------------------------------------------------------*/
extern void WScan_FreeEvents (WScan_EventList *events);
/* Frees a list of events                                                   */

/*--------------------------------------------------------------------------*/
extern long WScan_CountEvents (WScan_EventList *events);
/* Returns the number of events in a list of events                         */ 

/*--------------------------------------------------------------------------*/
extern WScan_EventList *WScan_GetNextEvent (WScan_EventList *event);
/* Returns the next event in the list of events, NULL if EOL                */ 

/*--------------------------------------------------------------------------*/
extern char *WScan_GetEventSigName (WScan_EventList *event);
/* Returns the name of the signal on which the event has occured            */

/*--------------------------------------------------------------------------*/
extern char WScan_GetEventValue (WScan_EventList *event);
/* returns the new value of the signal on which the event has occured       */


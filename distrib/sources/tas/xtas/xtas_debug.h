/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_debug.h                                                */
/*                                                                          */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles AUGUSTINS                                          */
/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/

extern int            XtasDrawingAreaWidth ;
extern int            XtasDrawingAreaHeight ;
                      
extern int            XtasMargin ;

extern char          *XtasDebugSigName ;
extern char          *XtasDebugOtherSigName ;
extern stbdebug_list *XtasDebugList ;
extern Widget         XtasDebugWidget ;
extern Widget         XtasGraphicWindow ;
extern long           XtasDebugChronoType ;
                      

extern Widget         XtasDebugLabelPeriod ;
extern Widget         XtasDebugLabelSetup  ;
extern Widget         XtasDebugLabelHold   ;
extern Widget         XtasDebugLabelMode   ;
extern Widget         XtasDebugLabelDelta  ;
extern Widget         XtasDebugLabelT1     ;
extern Widget         XtasDebugLabelT2     ;


extern Widget         XtasSetDebugSignal       __P((
			                                        char* 
											      )) ;
extern void         XtasSelectSignalCallback __P((
                                                    Widget ,
                                                    XtPointer ,
                                                    XtPointer
                                                  )) ;
extern void         XtasDebugButtonsCallback __P((
                                                    Widget ,
                                                    XtPointer ,
                                                    XtPointer
                                                  )) ;
extern void         XtasDebugCloseCallback __P(( 	Widget ,
                                               		XtPointer ,
                                               		XtPointer 
                                              	)) ;
extern void         XtasDebugInfo2Callback __P(( 	Widget ,
                                               		XtPointer ,
                                               		XtPointer 
                                              	)) ;
extern void         XtasDebugInfoCallback __P(( 	Widget ,
                                               		XtPointer ,
                                               		XtPointer 
                                              	)) ;
extern void         XtasDebugGetPathCallback __P((
                                                    Widget ,
                                                    XtPointer ,
                                                    XtPointer
                                                )) ;
extern void         XtasDebugGetPathCmdCallback __P((
                                                    Widget ,
                                                    XtPointer ,
                                                    XtPointer
                                                )) ;
extern void         XtasStbDebugCallback   __P((
                                               		Widget ,
                                               		XtPointer ,
                                               		XtPointer
                                              	)) ;

extern void         XtasDestroyDebugWidget __P(()) ;

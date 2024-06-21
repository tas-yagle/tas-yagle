/****************************************************************************/
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_ctk_info.h                                             */
/*                                                                          */
/*    Author(s) : Caroline BLED                       Date : 03/24/2003     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
extern  Widget  XtasNoiseParamWidget;
extern  Widget  XtasNoiseResultsWidget;
extern  Widget  XtasNoiseScoresWidget;

extern  void    XtasNoiseScoreConfigCallback    __P((
                                                    Widget,
                                                    XtPointer,
                                                    XtPointer
                                                   ));

extern  void    XtasDisplayCtkInfo              __P((
                                                    Widget,
                                                    ttvevent_list *,
                                                    XalMessageWidgetStruct *
                                                   ));

extern  void    XtasNoiseCloseCallback          __P((
                                                    Widget,
                                                    XtPointer,
                                                    XtPointer
                                                   ));

extern  void    XtasNoiseCallback               __P((
                                                    Widget,
                                                    XtPointer,
                                                    XtPointer
                                                   ));

extern  void    XtasNoiseDetailCallback         __P((
                                                    Widget,
                                                    XtPointer,
                                                    XtPointer
                                                   ));

extern  void    XtasNoiseCtkInfoCallback        __P((
                                                    Widget,
                                                    XtPointer,
                                                    XtPointer
                                                   ));

extern  void    XtasNoiseButtonsCallback        __P((
                                                    Widget,
                                                    XtPointer,
                                                    XtPointer
                                                   ));

extern  Widget  XtasCreateNoiseTextualScrollList __P((
                                                    Widget, 
                                                    XtasWindowStruct *,
                                                    Widget
                                                   ));

extern  Widget  XtasCreateNoiseGraphicalScrollList __P((
                                                    Widget, 
                                                    XtasWindowStruct *,
                                                    Widget
                                                   ));

extern  void    XtasNoiseTreatTopCallback       __P((
                                                    Widget,
                                                    XtPointer,
                                                    XtPointer
                                                   ));


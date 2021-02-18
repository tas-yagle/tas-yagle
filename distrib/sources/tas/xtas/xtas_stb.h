/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_stb.h                                                  */
/*                                                                          */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/

#define  XTAS_STB_SETUP  ((long) 0x80000001 )
#define  XTAS_STB_HOLD   ((long) 0x80000002 )

typedef struct  XtasStbParamStruct {
    char    report          ;
    char    out             ;
    char    analysis_var    ;
    char    graph_var       ;
    char    mode_var        ;
    char    ctk_var         ;
    char    rcx_ctk_model   ;
    char    rcx_ctk_noise   ;
    char    rcx_ctk_slope_noise   ;
    char    rcx_ctk_slope_delay   ;
    long    ctk_margin      ;
    long    min_slope_change        ;
    int     max_last_iter           ;
    long    ctk_report_delay_min    ;
    long    ctk_report_slope_min    ;
    float   ctk_report_ctk_min      ;
    long    ctk_report_noise_min    ;
    float   cache_size      ;
     
}xtas_stb_param_struct;

extern char          *XtasStbSigName ;
extern char           STBCONT;
extern char           STBFILE;
extern void XtasRemoveStbPathSession    __P(( XtasWindowStruct *)) ;
extern void XtasDispInfoCallback        __P(( 
                                             Widget,
                                             XtPointer,
                                             XtPointer
                                           )) ;
extern void XtasRemoveStbSession        __P((XtasWindowStruct*)) ;
extern void XtasStbHelpCallback         __P((
                                             Widget ,
                                             XtPointer  ,
                                             XtPointer
                                           )) ;
extern void XtasStbCloseCallback        __P((
                                             Widget ,
                                             XtPointer ,
                                             XtPointer
                                           )) ;
extern void XtasStbButtonsCallback      __P((
                                             Widget ,
                                             XtPointer ,
                                             XtPointer
                                           )) ;

extern void XtasStbFocusCallback        __P((
                                             Widget ,
                                             XtPointer ,
                                             XtPointer
                                           )) ;
extern void XtasLosingStbFocusCallback  __P((
                                             Widget ,
                                             XtPointer ,
                                             XtPointer
                                           )) ;
extern void XtasStbDetailCallback      __P((
                                             Widget ,
                                             XtPointer ,
                                             XtPointer
                                           )) ;
extern void XtasStbGetPathCallback     __P((
                                             Widget ,
                                             XtPointer ,
                                             XtPointer
                                           )) ;
extern void XtasStbGetPathCmdCallback  __P((
                                             Widget ,
                                             XtPointer ,
                                             XtPointer
                                           )) ;
extern void XtasStbAnalysisCallback    __P(( )) ;

extern void XtasStbSaveCallback        __P((
                                             Widget ,
                                             XtPointer ,
                                             XtPointer
                                           )) ;
extern void XtasStbSaveSelectFileOk    __P(( )); 


/****************************************************************************/
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_ctk_noise_button.h                                     */
/*                                                                          */
/*    Author(s) :  Caroline BLED                      Date : 28/03/2003     */
/*                                                                          */
/*    Modified by :                                   Date : ../../....     */
/*    Modified by :                                   Date : ../../....     */
/*    Modified by :                                   Date : ../../....     */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*                                                                          */
/****************************************************************************/

/*-------------------------*/
/* Button's menu structure */
/*-------------------------*/

static struct XalButtonsItem XtasNoiseButtonMenus[] = {
    { XTAS_MAIN_MAP  , True, XtasMainPopupCallback,       (XtPointer)0 },
    { XTAS_CLOS_MAP  , True, XtasNoiseCloseCallback,      (XtPointer)0 },
    { XTAS_INFO_MAP  , True, XtasNoiseDetailCallback,     (XtPointer)0 },
    { XTAS_CTKI_MAP  , True, XtasNoiseCtkInfoCallback,    (XtPointer)0 },
    { XTAS_SCORE_MAP , True, XtasNoiseScoreConfigCallback,(XtPointer)0 },
    { -1 , False, NULL, NULL}
};


static struct XalButtonsItem XtasNoiseButtonOptions[] = {
    { XTAS_MAIN_MAP  , True, XtasDummyCallback,  "File/Back To Main"     },
    { XTAS_CLOS_MAP  , True, XtasDummyCallback,  "File/Close"            },
    { XTAS_INFO_MAP  , True, XtasDummyCallback,  "View/Signals Info"     },
    { XTAS_CTKI_MAP  , True, XtasDummyCallback,  "View/Crosstalk Info"   },
    { XTAS_SCORE_MAP , True, XtasDummyCallback,  "Options/Scores Configuration" },
    { -1 , False, NULL, NULL}
};

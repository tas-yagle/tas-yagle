/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_ctk_noise_menu.h                                       */
/*                                                                          */
/*    Author(s) :   Caroline BLED                     Date : 28/03/2003     */
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
/* menu structure          */
/*-------------------------*/

static struct XalMenuItem XtasNoiseMenuFile[] = {
    { "To Main",      &xmPushButtonGadgetClass, 'M', False, False, "Ctrl<Key>M", "Ctrl+M",
                      XtasMainPopupCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { "Close",        &xmPushButtonGadgetClass, 'C', False,  False, "Ctrl<Key>E", "Ctrl+E", 
                      XtasNoiseCloseCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { "Load an INF file",        &xmPushButtonGadgetClass, 'L', True,  False, "Ctrl<Key>L", "Ctrl+L", 
                      XtasSelectInfFileCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { NULL , NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL}
};

static struct XalMenuItem XtasNoiseMenuView[] = {
    { "Signal Info",    &xmPushButtonGadgetClass, 'I', False, False, "Ctrl<Key>I", "Ctrl+I",
                            XtasNoiseDetailCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { "Crosstalk Info", &xmPushButtonGadgetClass, 'C', False, False, "Ctrl<Key>K", "Ctrl+K",
                            XtasNoiseCtkInfoCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { NULL , NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL}
};

//static struct XalMenuItem XtasNoiseMenuTools[] = {
//    { NULL , NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL}
//    { NULL }
//};

static struct XalMenuItem XtasNoiseMenuOptions[] = {
    { "Buttons Menu",  &xmPushButtonGadgetClass,    'B', False, False, "Ctrl<Key>B", "Ctrl+B",
                          XtasNoiseButtonsCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { "Scores Configuration", &xmPushButtonGadgetClass, 'C', True, False, "Ctrl<Key>S", "Ctrl+S",
                            XtasNoiseScoreConfigCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { NULL , NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL}
};

static struct XalMenuItem XtasNoiseMenuHelp[] = {
    { "On Main",    &xmPushButtonGadgetClass, 'M', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_MAIN|XTAS_SRC_NOISERES), (XalMenuItemStruct *)NULL },
    { "On File",    &xmPushButtonGadgetClass, 'F', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_FILE|XTAS_SRC_NOISERES), (XalMenuItemStruct *)NULL },
    { "On View",    &xmPushButtonGadgetClass, 'V', False, False, NULL, NULL,
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_VIEW|XTAS_SRC_NOISERES), (XalMenuItemStruct *)NULL },
//    { "On Tools",   &xmPushButtonGadgetClass, 'V', False, False, NULL, NULL,
//                       XtasHelpCallback, (XtPointer)(XTAS_HELP_TOOLS|XTAS_SRC_NOISERES), (XalMenuItemStruct *)NULL },
    { "On Options", &xmPushButtonGadgetClass, 'H', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_OPTIONS|XTAS_SRC_NOISERES), (XalMenuItemStruct *)NULL },
    { NULL , NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL}
};


static struct XalMenuItem XtasNoiseMenu[] = {
    { "File",    &xmCascadeButtonGadgetClass, 'F', False, False, NULL, NULL, NULL,
                                                  (XtPointer)0, XtasNoiseMenuFile },
    { "View",    &xmCascadeButtonGadgetClass, 'V', False, False, NULL, NULL, NULL,
                                                   (XtPointer)0, XtasNoiseMenuView },
//    { "Tools",   &xmCascadeButtonGadgetClass, 'T', False, False, NULL, NULL, NULL,
//                                                  (XtPointer)0, XtasNoiseMenuTools },
    { "Options", &xmCascadeButtonGadgetClass, 'O', False, False, NULL, NULL, NULL,
                                                  (XtPointer)0, XtasNoiseMenuOptions },
    { "Help",    &xmCascadeButtonGadgetClass, 'H', False, False, NULL, NULL, NULL, 
                                                  (XtPointer)0, XtasNoiseMenuHelp },
    { NULL , NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL}
};


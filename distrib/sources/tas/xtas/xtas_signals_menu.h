/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_signals_menu.h                                         */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Author(s) : Nizar ABDALLAH                      Date : 01/04/1993     */
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

static struct XalMenuItem XtasSignalsMenuFile[] = {
    //{ "Save Screen",  &xmPushButtonGadgetClass, 'S', False, False, "Ctrl<Key>S", "Ctrl+S",
    //                  XtasSignalsSaveCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { "To Main",      &xmPushButtonGadgetClass, 'M', False, False, "Ctrl<Key>M", "Ctrl+M",
                      XtasMainPopupCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { "Close",        &xmPushButtonGadgetClass, 'C', False,  False, "Ctrl<Key>E", "Ctrl+E", 
                      XtasFileCloseCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { "Load INF file",        &xmPushButtonGadgetClass, 'L', True,  False, "Ctrl<Key>L", "Ctrl+L", 
                      XtasSelectInfFileCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

/*-------------------------*/
/* Connectors              */
/*-------------------------*/

static struct XalMenuItem XtasSignalsMenuViewC[] = {
    { "Signal Info",      &xmPushButtonGadgetClass, 'I', False, False, "Ctrl<key>I", "Ctrl+I",
                              XtasSignalsDetailCallback, (XtPointer)XTAS_C_CONNECTOR, (XalMenuItemStruct *)NULL },
    { "Crosstalk Info",   &xmPushButtonGadgetClass, 'C', False, False, "Ctrl<key>K", "Ctrl+K",
                              XtasSignalsCtkInfoCallback, (XtPointer)XTAS_C_CONNECTOR, (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

static struct XalMenuItem XtasSignalsMenuOptC[] = {
    { "Buttons Menu",  &xmPushButtonGadgetClass,    'B', False, False, "Ctrl<key>B", "Ctrl+B",
                          XtasSignalsButtonsCallback, (XtPointer)XTAS_C_CONNECTOR, (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

/*-------------------------*/
/* Commands                */
/*-------------------------*/

static struct XalMenuItem XtasSignalsMenuViewO[] = {
    { "Signal Info",      &xmPushButtonGadgetClass, 'I', False, False, "Ctrl<key>I", "Ctrl+I",
                              XtasSignalsDetailCallback, (XtPointer)XTAS_C_COMMANDS, (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

static struct XalMenuItem XtasSignalsMenuOptO[] = {
    { "Buttons Menu",  &xmPushButtonGadgetClass,    'B', False, False, "Ctrl<key>B", "Ctrl+B",
                          XtasSignalsButtonsCallback, (XtPointer)XTAS_C_COMMANDS,
                          (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

/*-------------------------*/
/* Mem points              */
/*-------------------------*/

static struct XalMenuItem XtasSignalsMenuViewM[] = {
    { "Signal Info",      &xmPushButtonGadgetClass, 'I', False, False, "Ctrl<key>I", "Ctrl+I",
                              XtasSignalsDetailCallback, (XtPointer)XTAS_C_MEMORIZE, (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

static struct XalMenuItem XtasSignalsMenuOptM[] = {
    { "Buttons Menu",  &xmPushButtonGadgetClass,    'B', False, False, "Ctrl<key>B", "Ctrl+B",
                          XtasSignalsButtonsCallback, (XtPointer)XTAS_C_MEMORIZE,
                          (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

/*-------------------------*/
/* Precharge               */
/*-------------------------*/

static struct XalMenuItem XtasSignalsMenuViewP[] = {
    { "Signal Info",      &xmPushButtonGadgetClass, 'I', False, False, "Ctrl<key>I", "Ctrl+I",
                              XtasSignalsDetailCallback, (XtPointer)XTAS_C_PRECHARGE, (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

static struct XalMenuItem XtasSignalsMenuOptP[] = {
    { "Buttons Menu",  &xmPushButtonGadgetClass,    'B', False, False, "Ctrl<key>B", "Ctrl+B",
                          XtasSignalsButtonsCallback, (XtPointer)XTAS_C_PRECHARGE,
                          (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

/*-------------------------*/
/* Break                   */
/*-------------------------*/

static struct XalMenuItem XtasSignalsMenuViewB[] = {
    { "Signal Info",      &xmPushButtonGadgetClass, 'I', False, False, "Ctrl<key>I", "Ctrl+I",
                              XtasSignalsDetailCallback, (XtPointer)XTAS_C_BREAK, (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

static struct XalMenuItem XtasSignalsMenuOptB[] = {
    { "Buttons Menu",  &xmPushButtonGadgetClass,    'B', False, False, "Ctrl<key>B", "Ctrl+B",
                          XtasSignalsButtonsCallback, (XtPointer)XTAS_C_BREAK,
                          (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

/*-------------------------*/
/* All sigs                */
/*-------------------------*/

static struct XalMenuItem XtasSignalsMenuViewA[] = {
    { "Signal Info",      &xmPushButtonGadgetClass, 'I', False, False, "Ctrl<key>I", "Ctrl+I",
                              XtasSignalsDetailCallback, (XtPointer)XTAS_C_OTHERSIGS, (XalMenuItemStruct *)NULL },
    { "Crosstalk Info",   &xmPushButtonGadgetClass, 'C', False, False, "Ctrl<key>K", "Ctrl+K",
                              XtasSignalsCtkInfoCallback, (XtPointer)XTAS_C_OTHERSIGS, (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

static struct XalMenuItem XtasSignalsMenuOptA[] = {
    { "Buttons Menu",  &xmPushButtonGadgetClass,    'B', False, False, "Ctrl<key>B", "Ctrl+B",
                          XtasSignalsButtonsCallback, (XtPointer)XTAS_C_OTHERSIGS,
                          (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

/*-------------------------*/
/* Help                    */
/*-------------------------*/

static struct XalMenuItem XtasSignalsMenuHelpX[] = {
    { "On Main",    &xmPushButtonGadgetClass, 'M', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_MAIN|XTAS_SRC_SIGNALS), (XalMenuItemStruct *)NULL },
    { "On File",    &xmPushButtonGadgetClass, 'F', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_FILE|XTAS_SRC_SIGNALS), (XalMenuItemStruct *)NULL },
    { "On View",    &xmPushButtonGadgetClass, 'V', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_VIEW|XTAS_SRC_SIGNALS), (XalMenuItemStruct *)NULL },
    { "On Options", &xmPushButtonGadgetClass, 'O', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_OPTIONS|XTAS_SRC_SIGNALS), (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};


/* Help for AllSigs window -> crosstalk Information only on this window */
static struct XalMenuItem XtasSignalsMenuHelpA[] = {
    { "On Main",    &xmPushButtonGadgetClass, 'M', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_MAIN|XTAS_SRC_ALLSIGS), (XalMenuItemStruct *)NULL },
    { "On File",    &xmPushButtonGadgetClass, 'F', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_FILE|XTAS_SRC_ALLSIGS), (XalMenuItemStruct *)NULL },
    { "On View",    &xmPushButtonGadgetClass, 'V', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_VIEW|XTAS_SRC_ALLSIGS), (XalMenuItemStruct *)NULL },
    { "On Options", &xmPushButtonGadgetClass, 'O', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_OPTIONS|XTAS_SRC_ALLSIGS), (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

/*-------------------------*/
/* Menu bar                */
/*-------------------------*/

static struct XalMenuItem XtasSignalsMenu[] = {
    { "File",    &xmCascadeButtonGadgetClass, 'F', False, False, NULL, NULL, NULL,
                                                  (XtPointer)0, XtasSignalsMenuFile },
    { "View",    &xmCascadeButtonGadgetClass, 'V', False, False, NULL, NULL, NULL, 
                                                   (XtPointer)0, (XalMenuItemStruct *)NULL },
    { "Options", &xmCascadeButtonGadgetClass, 'O', False, False, NULL, NULL, NULL,
                                                  (XtPointer)0, (XalMenuItemStruct *)NULL },
    { "Help",    &xmCascadeButtonGadgetClass, 'H', False, False, NULL, NULL, NULL, 
                                                  (XtPointer)0, (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};


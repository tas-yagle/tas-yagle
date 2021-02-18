/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_stb_menu.h                                             */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Author(s) :                                     Date : 01/04/1993     */
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

static struct XalMenuItem XtasDebugMenuFile[] = {
    { "To Main",      &xmPushButtonGadgetClass, 'M', False, False, "Ctrl<Key>M", "Ctrl+M",
                      XtasMainPopupCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { "Close",        &xmPushButtonGadgetClass, 'C', False,  False, "Ctrl<Key>E", "Ctrl+E", 
                      XtasDebugCloseCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { "Load an INF file",        &xmPushButtonGadgetClass, 'L', True,  False, "Ctrl<Key>L", "Ctrl+L", 
                      XtasSelectInfFileCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

static struct XalMenuItem XtasDebugMenuView[] = {
    { "Signal Info", &xmPushButtonGadgetClass, 'I', False, False, "Ctrl<Key>I", "Ctrl+I",
                      XtasDebugInfoCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

static struct XalMenuItem XtasDebugMenuTools[] = {
    { "Get Paths", &xmPushButtonGadgetClass, 'P', False, False, "Ctrl<Key>P","Ctrl+P",
                      XtasDebugGetPathCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { "Get Command Paths", &xmPushButtonGadgetClass, 'C', False, False, "Ctrl<Key>C","Ctrl+C",
                      XtasDebugGetPathCmdCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { "Debug Error", &xmPushButtonGadgetClass, 'D', True, False, NULL, NULL,
                      XtasStbDebugCallback, (XtPointer)XTAS_DEBUG, (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

static struct XalMenuItem XtasDebugMenuOptions[] = {
    { "Buttons Menu",  &xmPushButtonGadgetClass,    'B', False, False, "Ctrl<Key>B", "Ctrl+B",
                          XtasDebugButtonsCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

static struct XalMenuItem XtasDebugMenuHelp[] = {
    { "On Main",    &xmPushButtonGadgetClass, 'M', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_MAIN|XTAS_SRC_DEBUG), (XalMenuItemStruct *)NULL },
    { "On File",    &xmPushButtonGadgetClass, 'F', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_FILE|XTAS_SRC_DEBUG), (XalMenuItemStruct *)NULL },
    { "On View",    &xmPushButtonGadgetClass, 'V', False, False, NULL, NULL,
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_VIEW|XTAS_SRC_DEBUG), (XalMenuItemStruct *)NULL },
    { "On Tools",   &xmPushButtonGadgetClass, 'T', False, False, NULL, NULL,
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_TOOLS|XTAS_SRC_DEBUG), (XalMenuItemStruct *)NULL },
    { "On Options", &xmPushButtonGadgetClass, 'O', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_OPTIONS|XTAS_SRC_DEBUG), (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};


static struct XalMenuItem XtasDebugMenu[] = {
    { "File",    &xmCascadeButtonGadgetClass, 'F', False, False, NULL, NULL, NULL,
                                                  (XtPointer)0, XtasDebugMenuFile },
    { "View",    &xmCascadeButtonGadgetClass, 'V', False, False, NULL, NULL, NULL,
                                                    (XtPointer)0, XtasDebugMenuView },
    { "Tools",   &xmCascadeButtonGadgetClass, 'T', False, False, NULL, NULL, NULL,
                                                  (XtPointer)0, XtasDebugMenuTools},
    { "Options", &xmCascadeButtonGadgetClass, 'O', False, False, NULL, NULL, NULL,
                                                  (XtPointer)0, XtasDebugMenuOptions },
    { "Help",    &xmCascadeButtonGadgetClass, 'H', False, False, NULL, NULL, NULL, 
                                                  (XtPointer)0, XtasDebugMenuHelp },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

static struct XalMenuItem XtasSelectDebugMenuView[] = {
    { "Connectors",        &xmPushButtonGadgetClass, 'C', False, False, NULL, NULL,
                              XtasSelectSignalCallback,     (XtPointer)XTAS_C_CONNECTOR,
                              (XalMenuItemStruct *)NULL },
    { "Commands",          &xmPushButtonGadgetClass, 'O', False, False, NULL, NULL, 
                              XtasSelectSignalCallback,     (XtPointer)XTAS_C_COMMANDS,
                              (XalMenuItemStruct *)NULL },
    { "Registers",         &xmPushButtonGadgetClass, 'R', False, False, NULL, NULL, 
                              XtasSelectSignalCallback,     (XtPointer)XTAS_C_MEMORIZE, 
                              (XalMenuItemStruct *)NULL },
    { "Precharged Points", &xmPushButtonGadgetClass, 'P', False, False, NULL, NULL, 
                              XtasSelectSignalCallback,     (XtPointer)XTAS_C_PRECHARGE,
                              (XalMenuItemStruct *)NULL },
    { "Break Points",      &xmPushButtonGadgetClass, 'B', False, False, NULL, NULL, 
                              XtasSelectSignalCallback,     (XtPointer)XTAS_C_BREAK,
                              (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

static struct XalMenuItem XtasSelectDebugSignalMenu[] = {
    { "Select", &xmCascadeButtonGadgetClass, 'S', False, False, NULL, NULL, NULL, 
                                           (XtPointer)0, XtasSelectDebugMenuView },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

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

static struct XalMenuItem XtasStbMenuFile[] = {
    { "To Main",      &xmPushButtonGadgetClass, 'M', False, False, "Ctrl<Key>M", "Ctrl+M",
                      XtasMainPopupCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { "Close",        &xmPushButtonGadgetClass, 'C', False,  False, "Ctrl<Key>E", "Ctrl+E", 
                      XtasStbCloseCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { "Save Screen",  &xmPushButtonGadgetClass, 'S', True, False, "Ctrl<Key>S", "Ctrl+S",
                      XtasStbSaveCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { "Load INF file",        &xmPushButtonGadgetClass, 'L', True,  False, "Ctrl<Key>L", "Ctrl+L", 
                      XtasSelectInfFileCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

static struct XalMenuItem XtasStbMenuView[] = {
    { "Signal Info",    &xmPushButtonGadgetClass, 'I', False, False, "Ctrl<Key>I", "Ctrl+I",
                            XtasStbDetailCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

static struct XalMenuItem XtasStbMenuTools[] = {
    { "Get Paths",         &xmPushButtonGadgetClass, 'P', False, False, "Ctrl<Key>P", "Ctrl+P",
                           XtasStbGetPathCallback,    (XtPointer)0,        (XalMenuItemStruct *)NULL },

    { "Get Command Paths", &xmPushButtonGadgetClass, 'C', False, False, "Ctrl<Key>C", "Ctrl+C",
                           XtasStbGetPathCmdCallback, (XtPointer)0,        (XalMenuItemStruct *)NULL },

    { "Noise Analysis",    &xmPushButtonGadgetClass, 'N', False, False,  NULL,         NULL,
                           XtasNoiseCallback,         (XtPointer)0,        (XalMenuItemStruct *)NULL },

    { "Debug Error",       &xmPushButtonGadgetClass, 'D', True,  False,  NULL,         NULL,
                           XtasStbDebugCallback,      (XtPointer)XTAS_STB, (XalMenuItemStruct *)NULL },

    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

static struct XalMenuItem XtasStbMenuOptions[] = {
    { "Buttons Menu",  &xmPushButtonGadgetClass,    'B', False, False, "Ctrl<Key>B","Ctrl+B",
                          XtasStbButtonsCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

static struct XalMenuItem XtasStbMenuHelp[] = {
    { "On Main",    &xmPushButtonGadgetClass, 'M', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_MAIN|XTAS_SRC_STABRES), (XalMenuItemStruct *)NULL },
    { "On File",    &xmPushButtonGadgetClass, 'F', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_FILE|XTAS_SRC_STABRES), (XalMenuItemStruct *)NULL },
    { "On View",    &xmPushButtonGadgetClass, 'V', False, False, NULL, NULL,
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_VIEW|XTAS_SRC_STABRES), (XalMenuItemStruct *)NULL },
    { "On Tools",   &xmPushButtonGadgetClass, 'T', False, False, NULL, NULL,
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_TOOLS|XTAS_SRC_STABRES), (XalMenuItemStruct *)NULL },
    { "On Options", &xmPushButtonGadgetClass, 'O', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_OPTIONS|XTAS_SRC_STABRES), (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};


static struct XalMenuItem XtasStbMenu[] = {
    { "File",    &xmCascadeButtonGadgetClass, 'F', False, False, NULL, NULL, NULL,
                                                  (XtPointer)0, XtasStbMenuFile },
    { "View",    &xmCascadeButtonGadgetClass, 'V', False, False, NULL, NULL, NULL,
                                                   (XtPointer)0, XtasStbMenuView },
    { "Tools",   &xmCascadeButtonGadgetClass, 'T', False, False, NULL, NULL, NULL,
                                                  (XtPointer)0, XtasStbMenuTools },
    { "Options", &xmCascadeButtonGadgetClass, 'O', False, False, NULL, NULL, NULL,
                                                  (XtPointer)0, XtasStbMenuOptions },
    { "Help",    &xmCascadeButtonGadgetClass, 'H', False, False, NULL, NULL, NULL, 
                                                  (XtPointer)0, XtasStbMenuHelp },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};


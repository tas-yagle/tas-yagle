/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_paths_menu.h                                           */
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

static struct XalMenuItem XtasPathsMenuFile[] = {
    { "To Main",      &xmPushButtonGadgetClass, 'M', False, False, "Ctrl<Key>M", "Ctrl+M",
                      XtasMainPopupCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { "Close",        &xmPushButtonGadgetClass, 'C', False,  False, "Ctrl<Key>E", "Ctrl+E", 
                      XtasPathsCloseCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { "Save Screen",  &xmPushButtonGadgetClass, 'S', True, False, "Ctrl<Key>S", "Ctrl+S",
                      XtasPathsSaveCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { "Load an INF file",        &xmPushButtonGadgetClass, 'L', True,  False, "Ctrl<Key>L", "Ctrl+L", 
                      XtasSelectInfFileCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

static struct XalMenuItem XtasPathsMenuTools[] = {
    { "Get Paths",       &xmPushButtonGadgetClass, 'P', False, False, "Ctrl<Key>P","Ctrl+P",
                            XtasPathsGetPathCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { "Get Command Paths", &xmPushButtonGadgetClass, 'C', False, False, "Ctrl<Key>C","Ctrl+C",
                            XtasPathsGetPathCmdCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { "Path Detail",     &xmPushButtonGadgetClass, 'D', False, False, NULL, NULL,
                            XtasPathsDetailCallback,     (XtPointer)'M', (XalMenuItemStruct *)NULL },
    { "All Paths",       &xmPushButtonGadgetClass, 'A', False, False, NULL, NULL,
                            XtasParaButtonCallback,  (XtPointer)'P', (XalMenuItemStruct *)NULL },
//    { "False Path Test", &xmPushButtonGadgetClass, 'F', True, False, NULL, NULL,
//                            XtasPathsFalsCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

static struct XalMenuItem XtasPathsMenuView[] = {
    { "Signal Info",    &xmPushButtonGadgetClass, 'I', False, False, "Ctrl<Key>I","Ctrl+I",
                            XtasSignalInfoCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

static struct XalMenuItem XtasPathsMenuOptions[] = {
    { "Buttons Menu",  &xmPushButtonGadgetClass,    'B', False, False, NULL, NULL,
                          XtasPathsButtonsCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};


static struct XalMenuItem XtasPathsMenuHelp[] = {
    { "On Main",    &xmPushButtonGadgetClass, 'M', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_MAIN|XTAS_SRC_PATHS), (XalMenuItemStruct *)NULL },
    { "On File",    &xmPushButtonGadgetClass, 'F', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_FILE|XTAS_SRC_PATHS), (XalMenuItemStruct *)NULL },
    { "On View",    &xmPushButtonGadgetClass, 'V', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_VIEW|XTAS_SRC_PATHS), (XalMenuItemStruct *)NULL },
    { "On Tools",   &xmPushButtonGadgetClass, 'T', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_TOOLS|XTAS_SRC_PATHS), (XalMenuItemStruct *)NULL },
    { "On Options", &xmPushButtonGadgetClass, 'O', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_OPTIONS|XTAS_SRC_PATHS), (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

static struct XalMenuItem XtasPathsMenu[] = {
    { "File",    &xmCascadeButtonGadgetClass, 'F', False, False, NULL, NULL, NULL,
                                                  (XtPointer)0, XtasPathsMenuFile },
    { "View",   &xmCascadeButtonGadgetClass, 'V', False, False, NULL, NULL, NULL, 
                                                   (XtPointer)0, XtasPathsMenuView },
    { "Tools",   &xmCascadeButtonGadgetClass, 'T', False, False, NULL, NULL, NULL, 
                                                   (XtPointer)0, XtasPathsMenuTools },
    { "Options", &xmCascadeButtonGadgetClass, 'O', False, False, NULL, NULL, NULL,
                                                  (XtPointer)0, XtasPathsMenuOptions },
    { "Help",    &xmCascadeButtonGadgetClass, 'H', False, False, NULL, NULL, NULL, 
                                                  (XtPointer)0, XtasPathsMenuHelp },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};


/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_detpath_menu.h                                         */
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

static struct XalMenuItem XtasDetailMenuFile[] = {
    { "To Main",      &xmPushButtonGadgetClass, 'M', False, False, "Ctrl<Key>M", "Ctrl+M",
                      XtasMainPopupCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { "Close",        &xmPushButtonGadgetClass, 'C', False,  False, "Ctrl<Key>E", "Ctrl+E", 
                      XtasDetailCloseCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { "Save Screen",  &xmPushButtonGadgetClass, 'S', True, False, "Ctrl<Key>S", "Ctrl+S",
                      XtasDetailSaveCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { "Load INF file",        &xmPushButtonGadgetClass, 'L', True,  False, "Ctrl<Key>L", "Ctrl+L", 
                      XtasSelectInfFileCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

static struct XalMenuItem XtasDetailMenuView[] = {
    { "Signal Info",    &xmPushButtonGadgetClass, 'I', False, False, "Ctrl<Key>I", "Ctrl+I",
                            XtasDetailDetailCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { "Crosstalk Info", &xmPushButtonGadgetClass, 'C', False, False, "Ctrl<Key>K", "Ctrl+K",
                            XtasCtkInfoCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { "Visualize a Path",    &xmPushButtonGadgetClass, 'V', True, False, NULL, NULL,
                            XtasVisualPathCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

static struct XalMenuItem XtasDetailMenuViewDelay[] = {
    { "Signal Info",    &xmPushButtonGadgetClass, 'I', False, False, "Ctrl<Key>I", "Ctrl+I",
                            XtasDetailDetailCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

static struct XalMenuItem XtasDetailMenuViewSimu[] = {
    { "Signal Info",    &xmPushButtonGadgetClass, 'I', False, False, "Ctrl<Key>I", "Ctrl+I",
                            XtasDetailDetailCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { "Visualize a Path",    &xmPushButtonGadgetClass, 'V', True, False, NULL, NULL,
                            XtasVisualPathCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

static struct XalMenuItem XtasDetailMenuTools[] = {
    { "Simulate a Path",    &xmPushButtonGadgetClass, 'S', False, False, NULL, NULL,
                            XtasSimuPathCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

static struct XalMenuItem XtasDetailMenuOptions[] = {
    { "Buttons Menu",  &xmPushButtonGadgetClass,    'B', False, False, "Ctrl<Key>B", "Ctrl+B",
                          XtasDetailButtonsCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

static struct XalMenuItem XtasDetailMenuHelp[] = {
    { "On Main",    &xmPushButtonGadgetClass, 'M', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_MAIN|XTAS_SRC_DETAIL), (XalMenuItemStruct *)NULL },
    { "On File",    &xmPushButtonGadgetClass, 'F', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_FILE|XTAS_SRC_DETAIL), (XalMenuItemStruct *)NULL },
    { "On View",    &xmPushButtonGadgetClass, 'V', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_VIEW|XTAS_SRC_DETAIL), (XalMenuItemStruct *)NULL },
    { "On Tools",   &xmPushButtonGadgetClass, 'T', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_TOOLS|XTAS_SRC_DETAIL), (XalMenuItemStruct *)NULL },
    { "On Options", &xmPushButtonGadgetClass, 'O', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_OPTIONS|XTAS_SRC_DETAIL), (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

static struct XalMenuItem XtasDetailMenuHelpCtk[] = {
    { "On Main",    &xmPushButtonGadgetClass, 'M', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_MAIN|XTAS_SRC_DETAILCTK), (XalMenuItemStruct *)NULL },
    { "On File",    &xmPushButtonGadgetClass, 'F', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_FILE|XTAS_SRC_DETAILCTK), (XalMenuItemStruct *)NULL },
    { "On View",    &xmPushButtonGadgetClass, 'V', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_VIEW|XTAS_SRC_DETAILCTK), (XalMenuItemStruct *)NULL },
    { "On Tools",   &xmPushButtonGadgetClass, 'T', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_TOOLS|XTAS_SRC_DETAILCTK), (XalMenuItemStruct *)NULL },
    { "On Options", &xmPushButtonGadgetClass, 'O', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_OPTIONS|XTAS_SRC_DETAILCTK), (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

static struct XalMenuItem XtasDetailMenuHelpDelay[] = {
    { "On Main",    &xmPushButtonGadgetClass, 'M', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_MAIN|XTAS_SRC_DELAY), (XalMenuItemStruct *)NULL },
    { "On File",    &xmPushButtonGadgetClass, 'F', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_FILE|XTAS_SRC_DELAY), (XalMenuItemStruct *)NULL },
    { "On View",    &xmPushButtonGadgetClass, 'V', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_VIEW|XTAS_SRC_DELAY), (XalMenuItemStruct *)NULL },
    { "On Options", &xmPushButtonGadgetClass, 'O', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_OPTIONS|XTAS_SRC_DELAY), (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

static struct XalMenuItem XtasDetailMenuHelpSimu[] = {
    { "On Main",    &xmPushButtonGadgetClass, 'M', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_MAIN|XTAS_SRC_DETAILSIMU), (XalMenuItemStruct *)NULL },
    { "On File",    &xmPushButtonGadgetClass, 'F', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_FILE|XTAS_SRC_DETAILSIMU), (XalMenuItemStruct *)NULL },
    { "On View",    &xmPushButtonGadgetClass, 'V', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_VIEW|XTAS_SRC_DETAILSIMU), (XalMenuItemStruct *)NULL },
    { "On Options", &xmPushButtonGadgetClass, 'O', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_OPTIONS|XTAS_SRC_DETAILSIMU), (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

static struct XalMenuItem XtasDetailMenu[] = {   /* fenetre detail simple*/
    { "File",    &xmCascadeButtonGadgetClass, 'F', False, False, NULL, NULL, NULL,
                                                  (XtPointer)0, XtasDetailMenuFile },
    { "View",    &xmCascadeButtonGadgetClass, 'V', False, False, NULL, NULL, NULL, 
                                                   (XtPointer)0, XtasDetailMenuView },
    { "Tools",    &xmCascadeButtonGadgetClass, 'T', False, False, NULL, NULL, NULL, 
                                                   (XtPointer)0, XtasDetailMenuTools },
    { "Options", &xmCascadeButtonGadgetClass, 'O', False, False, NULL, NULL, NULL,
                                                  (XtPointer)0, XtasDetailMenuOptions },
    { "Help",    &xmCascadeButtonGadgetClass, 'H', False, False, NULL, NULL, NULL, 
                                                  (XtPointer)0, XtasDetailMenuHelp },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

static struct XalMenuItem XtasDetailMenuCtk[] = {   /* fenetre detail avec ctk*/
    { "File",    &xmCascadeButtonGadgetClass, 'F', False, False, NULL, NULL, NULL,
                                                  (XtPointer)0, XtasDetailMenuFile },
    { "View",    &xmCascadeButtonGadgetClass, 'V', False, False, NULL, NULL, NULL, 
                                                   (XtPointer)0, XtasDetailMenuView },
    { "Tools",    &xmCascadeButtonGadgetClass, 'T', False, False, NULL, NULL, NULL, 
                                                   (XtPointer)0, XtasDetailMenuTools },
    { "Options", &xmCascadeButtonGadgetClass, 'O', False, False, NULL, NULL, NULL,
                                                  (XtPointer)0, XtasDetailMenuOptions },
    { "Help",    &xmCascadeButtonGadgetClass, 'H', False, False, NULL, NULL, NULL, 
                                                  (XtPointer)0, XtasDetailMenuHelpCtk },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

static struct XalMenuItem XtasDetailMenuDelay[] = {   /* fenetre path simple*/
    { "File",    &xmCascadeButtonGadgetClass, 'F', False, False, NULL, NULL, NULL,
                                                  (XtPointer)0, XtasDetailMenuFile },
    { "View",    &xmCascadeButtonGadgetClass, 'V', False, False, NULL, NULL, NULL, 
                                                   (XtPointer)0, XtasDetailMenuViewDelay},
    { "Options", &xmCascadeButtonGadgetClass, 'O', False, False, NULL, NULL, NULL,
                                                  (XtPointer)0, XtasDetailMenuOptions },
    { "Help",    &xmCascadeButtonGadgetClass, 'H', False, False, NULL, NULL, NULL, 
                                                  (XtPointer)0, XtasDetailMenuHelpDelay },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

static struct XalMenuItem XtasDetailMenuSimu[] = {   /* fenetre path simple*/
    { "File",    &xmCascadeButtonGadgetClass, 'F', False, False, NULL, NULL, NULL,
                                                  (XtPointer)0, XtasDetailMenuFile },
    { "View",    &xmCascadeButtonGadgetClass, 'V', False, False, NULL, NULL, NULL, 
                                                   (XtPointer)0, XtasDetailMenuViewSimu},
    { "Options", &xmCascadeButtonGadgetClass, 'O', False, False, NULL, NULL, NULL,
                                                  (XtPointer)0, XtasDetailMenuOptions },
    { "Help",    &xmCascadeButtonGadgetClass, 'H', False, False, NULL, NULL, NULL, 
                                                  (XtPointer)0, XtasDetailMenuHelpSimu },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};


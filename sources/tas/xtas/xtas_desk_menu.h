/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_desk_menu.h                                            */
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

static struct XalMenuItem XtasDeskMenuFile[] = {
    { "Open",    &xmPushButtonGadgetClass, 'O', False, False, "Ctrl<Key>O", "Ctrl+O", 
                XtasOpenDataBaseCallback , (XtPointer)0, (XalMenuItemStruct *)NULL },
/*
    { "Save",   &xmPushButtonGadgetClass, 'S', False, False, "Ctrl<Key>S", "Ctrl+S",
                XtasFileSaveCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
*/
    { "Load an INF file",        &xmPushButtonGadgetClass, 'L', True,  False, "Ctrl<Key>L", "Ctrl+L", 
                      XtasSelectInfFileCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { "Exit",   &xmPushButtonGadgetClass, 'E', True,  False, "Ctrl<Key>E", "Ctrl+E", 
                XtasFileExitCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { NULL , NULL, (char) NULL, False, False, NULL, NULL, NULL, NULL, NULL}
};

static struct XalMenuItem XtasDeskMenuView[] = {
    { "Connectors",        &xmPushButtonGadgetClass, 'C', False, False, NULL, NULL,
                              XtasDeskSignalsViewCallback,     (XtPointer)XTAS_C_CONNECTOR,
                              (XalMenuItemStruct *)NULL },
    { "Registers",         &xmPushButtonGadgetClass, 'R', False, False, NULL, NULL, 
                              XtasDeskSignalsViewCallback,     (XtPointer)XTAS_C_MEMORIZE, 
                              (XalMenuItemStruct *)NULL },
    { "Commands",          &xmPushButtonGadgetClass, 'o', False, False, NULL, NULL, 
                              XtasDeskSignalsViewCallback,     (XtPointer)XTAS_C_COMMANDS, 
                              (XalMenuItemStruct *)NULL },
    { "Precharged Points", &xmPushButtonGadgetClass, 'P', False, False, NULL, NULL, 
                              XtasDeskSignalsViewCallback,     (XtPointer)XTAS_C_PRECHARGE,
                              (XalMenuItemStruct *)NULL },
    { "Break Points",      &xmPushButtonGadgetClass, 'B', False, False, NULL, NULL, 
                              XtasDeskSignalsViewCallback,     (XtPointer)XTAS_C_BREAK,
                              (XalMenuItemStruct *)NULL },
    { "Internal Signals",  &xmPushButtonGadgetClass, 'S', False, False, NULL, NULL, 
                              XtasDeskSignalsViewCallback,     (XtPointer)XTAS_C_OTHERSIGS,
                              (XalMenuItemStruct *)NULL },
    { NULL , NULL, (char) NULL, False, False, NULL, NULL, NULL, NULL, NULL}
};

static struct XalMenuItem XtasDeskMenuTools[] = {
    { "Get Paths", &xmPushButtonGadgetClass, 'P', False, False, "Ctrl<Key>P", "Ctrl+P",
                      XtasMainToolsCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { "Get Delays",  &xmPushButtonGadgetClass, 'D', False, False, NULL, NULL,
                          XtasGetDelayCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { "Stability",  &xmPushButtonGadgetClass, 'S', True, False, NULL, NULL,
                          XtasStbAnalysisCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
/*    { "Timing Analysis",  &xmPushButtonGadgetClass, 'T', True, False, NULL, NULL,
                          XtasNewSessionCallback, (XtPointer)TAS_TOOL, (XalMenuItemStruct *)NULL },
    { "Timing Model Abstraction",  &xmPushButtonGadgetClass, 'A', False, False, NULL, NULL,
                          XtasNewSessionCallback, (XtPointer)TMA_TOOL, (XalMenuItemStruct *)NULL },
    { "XYagle",  &xmPushButtonGadgetClass, 'X', True, False, NULL, NULL,
                          XtasXyagleCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },*/
    { NULL , NULL, (char) NULL, False, False, NULL, NULL, NULL, NULL, NULL}
};

static struct XalMenuItem XtasDeskMenuConfig[] = {
    { "Memory Size",  &xmPushButtonGadgetClass,    'M', False, False, NULL, NULL,
                           XtasOptionsMemorySizeCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { "Buttons Menu",  &xmPushButtonGadgetClass,    'B', True, False, "Ctrl<Key>B", "Ctrl+B",
                          XtasOptionsButtonsCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { "Display Type",  &xmPushButtonGadgetClass,    'D', True, False, NULL, NULL,
                          XtasOptionsDisplayTypeCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { NULL , NULL, (char) NULL, False, False, NULL, NULL, NULL, NULL, NULL}
};

static struct XalMenuItem XtasDeskMenuHelp[] = {
    { "On File",    &xmPushButtonGadgetClass, 'F', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_FILE|XTAS_SRC_DESK), (XalMenuItemStruct *)NULL },
    { "On View",    &xmPushButtonGadgetClass, 'V', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_VIEW|XTAS_SRC_DESK), (XalMenuItemStruct *)NULL },
    { "On Tools",   &xmPushButtonGadgetClass, 'T', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_TOOLS|XTAS_SRC_DESK), (XalMenuItemStruct *)NULL }, 
    { "On Config", &xmPushButtonGadgetClass, 'C', False, False, NULL, NULL, 
                       XtasHelpCallback, (XtPointer)(XTAS_HELP_OPTIONS|XTAS_SRC_DESK), (XalMenuItemStruct *)NULL }, 
    { "On Version", &xmPushButtonGadgetClass, 'e', True,  False,  NULL, NULL, 
                       XtasHelpOnVersionCallback, (XtPointer)0, (XalMenuItemStruct *)NULL },
    { NULL , NULL, (char) NULL, False, False, NULL, NULL, NULL, NULL, NULL}
};

static struct XalMenuItem XtasDeskMenu[] = {
    { "File",    &xmCascadeButtonGadgetClass, 'F', False, False, NULL, NULL, NULL,
                                                  (XtPointer)0, XtasDeskMenuFile },
    { "View",    &xmCascadeButtonGadgetClass, 'V', False, False, NULL, NULL, NULL, 
                                                   (XtPointer)0, XtasDeskMenuView },
    { "Tools",   &xmCascadeButtonGadgetClass, 'T', False, False, NULL, NULL, NULL, 
                                                  (XtPointer)0, XtasDeskMenuTools },
    { "Config",  &xmCascadeButtonGadgetClass, 'C', False, False, NULL, NULL, NULL, 
                                                  (XtPointer)0, XtasDeskMenuConfig },
    { "Help",    &xmCascadeButtonGadgetClass, 'H', False, False, NULL, NULL, NULL, 
                                                  (XtPointer)0, XtasDeskMenuHelp },
    { NULL , NULL, (char) NULL, False, False, NULL, NULL, NULL, NULL, NULL}
};


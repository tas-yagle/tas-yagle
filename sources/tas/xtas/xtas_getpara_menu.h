/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_getpara_menu.h                                         */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Author(s) : Mathieu OKUYAMA                     Date : 07/04/1998     */
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

static struct XalMenuItem XtasGetParaMenuView[] = {
    { "Signals",           &xmPushButtonGadgetClass, 'S', False, False, NULL, NULL,
                              XtasParaSignalCallback,     (XtPointer)XTAS_C_OTHERSIGS,
                              (XalMenuItemStruct *)NULL },

    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

static struct XalMenuItem XtasGetParaOnPathMenu[] = {
    { "On Path", &xmCascadeButtonGadgetClass, 'S', False, False, NULL, NULL, NULL, 
                                           (XtPointer)0, XtasGetParaMenuView },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};


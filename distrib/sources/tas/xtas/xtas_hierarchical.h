/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_hierarchical.h                                         */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Author(s) : Mathieu OKUYAMA                     Date : 06/22/1998     */
/*                                                                          */
/*    Modified by :                                   Date : ../../....     */
/*    Modified by :                                   Date : ../../....     */
/*    Modified by :                                   Date : ../../....     */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*                                                                          */
/****************************************************************************/


typedef struct XtasHier
    {
     Widget instance ;
     Widget figure ;
     XtasSearchSigParamStruct *param ;
    }
XtasHierStruct;


extern void XtasHViewCancelCallback        __P((
                                                 Widget ,
                                                 XtPointer ,
                                                 XtPointer 
                                              )) ;
extern void XtasHViewOkCallback            __P((
                                                 Widget ,
                                                 XtPointer ,
                                                 XtPointer
                                              )) ;
extern void XtasHViewAllLevelCallback      __P((
                                                 Widget ,
                                                 XtPointer ,
                                                 XtPointer
                                              )) ;
extern void XtasHierarchicalViewCallback   __P((
                                                 Widget ,
                                                 XtPointer ,
                                                 XtPointer 
                                              )) ;

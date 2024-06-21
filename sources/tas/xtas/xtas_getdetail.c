/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_getdetail.c                                            */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Author(s) : Nizar ABDALLAH                      Date : 01/04/1993     */
/*                                                                          */
/*    Modified by : Mathieu OKUYAMA                   Date : 05/21/1998     */
/*                                                                          */
/*    Modified by : Stephane PICAULT                  Date : 10/13/1998     */
/*    Modified by :                                   Date : ../../....     */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*                                                                          */
/****************************************************************************/


/*--------------------------------------------------------------------------*/
/*                             INCLUDE FILES                                */
/*--------------------------------------------------------------------------*/
#include "xtas.h"

/*---------------------------------------------------------------------------*/
/*                               FUNCTIONS                                   */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasGetDetailPathsOkCallback                                   */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .path_set :                                                    */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for "Get Detail" Widget OK button        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasPathsDetailCallback (widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    XtasWindowStruct  *tas_winfos = (XtasWindowStruct *)client_data ;
    XtasPathListSetStruct *path_set = (XtasPathListSetStruct *)tas_winfos->userdata ;

	char unvalid_instance_mask;
	Widget window;
	long type = path_set->PARAM->SCAN_TYPE;
	ttvfig_list *ttvfig;

    if (path_set->PARAM->LOOK_PATH) {

	    ttvfig = path_set->PARAM->LOOK_PATH->FIG;
	    unvalid_instance_mask = 0;

	    path_set->PARAM->SCAN_TYPE &= ~(TTV_FIND_DUAL);

	    path_set->PARAM->LOOK_PATH->FIG = NULL;

	    XtasSetLabelString (XtasDeskMessageField, XTAS_PATINFO);
	    XalSetCursor (XtasTopLevel, WAIT);
	    XalForceUpdate (XtasTopLevel);

	    if ((unvalid_instance_mask) || !(window = XtasPathDetailList (path_set->TOP_LEVEL, path_set)))
		    XalDrawMessage (XtasWarningWidget, XTAS_NPATWAR);

	    XtasSetLabelString (XtasDeskMessageField, XTAS_NULINFO);
	    XalSetCursor (XtasTopLevel, NORMAL);

	    path_set->PARAM->LOOK_PATH->FIG = ttvfig;
	    path_set->PARAM->SCAN_TYPE = type;
    }
    else 
        XalDrawMessage( XtasWarningWidget, XTAS_NPATERR );
}

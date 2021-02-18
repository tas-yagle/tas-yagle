/****************************************************************************/
/*                                                                          */
/*                         ALLIANCE  CAD  FRAMEWORK                         */
/*                                                                          */
/*    Tool : libXal  Version 1.01                                           */
/*    File : ForceUpdate.c                                                  */
/*                                                                          */
/*    (c) copyright 1993 MASI Laboratory. CAO & VLSI CAD Team               */
/*    All rights reserved.                                                  */
/*    Support : e-mail cao-vlsi@masi.ibp.fr                                 */
/*                                                                          */
/*    Author(s) : Nizar ABDALLAH                      Date : 02/02/1993     */
/*                                                                          */
/*    Modified by :                                   Date : ../../....     */
/*    Modified by :                                   Date : ../../....     */
/*    Modified by :                                   Date : ../../....     */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/* This function is a superset of XmUpdateDisplay() in that it will ensure  */
/* that a window's contents are visible before returning. The monitoring    */
/* of window states is necessary because attempts to map a window are       */
/* subject to the whim of the window manager. This introduces a signifi-    */
/* cant delay before the window is actually mapped and exposed.             */
/*                                                                          */
/****************************************************************************/


/*--------------------------------------------------------------------------*/
/*                             INCLUDE FILES                                */
/*--------------------------------------------------------------------------*/
#include <Xm/Xm.h>

/*-----------------------------*/
/* Motif for Alliance includes */
/*-----------------------------*/
#include MUT_H

#include XAL_H

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XalForceUpdate                                                 */
/*                                                                           */
/* IN  ARGS : .w : The widget that will serve to get the display pointer     */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Forces and updates the contents of a specific window.          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XalForceUpdate(w)
Widget w;
{
Display           *dpy;
XEvent            event;
Widget            diashell,  topshell;
Window            diawindow, topwindow;
XWindowAttributes xwa;
XtAppContext      cxt = XtWidgetToApplicationContext(w);

/*---------------------------------------*/
/* Locate the shell we are interested in */
/*---------------------------------------*/
for (diashell = w; !XtIsShell(diashell);diashell = XtParent(diashell)) ;

/*------------------------------------*/
/* Locate it's primary window's shell */
/*------------------------------------*/
for (topshell = diashell; !XtIsTopLevelShell(topshell);topshell = XtParent(topshell)) ;

/*-----------------------------------------------------------------------*/
/* If the dialog shell is not realized, don't bother. Nothing can happen */
/*-----------------------------------------------------------------------*/
if (XtIsRealized(diashell) && XtIsRealized(topshell))
    {
    dpy       = XtDisplay( topshell );
    diawindow = XtWindow( diashell );
    topwindow = XtWindow( topshell );

    /*----------------------------------*/
    /* Wait for the dialog to be mapped */
    /*----------------------------------*/
    while ((XGetWindowAttributes( dpy, diawindow, &xwa )) &&
	   (xwa.map_state != IsViewable) )
        {
        if ((XGetWindowAttributes( dpy, topwindow, &xwa )) && 
            (xwa.map_state != IsViewable )) 
            break;
        XtAppNextEvent( cxt, &event );
        XtDispatchEvent( &event );
        }
    }
XmUpdateDisplay( topshell );
}



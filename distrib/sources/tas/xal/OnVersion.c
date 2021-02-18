/****************************************************************************/
/*                                                                          */
/*                         ALLIANCE  CAD  FRAMEWORK                         */
/*                                                                          */
/*    Tool : libXal Version 1.01                                            */
/*    File :      OnVersion.c                                               */
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
/* This file contains a function to manage an "On Version" message          */
/* about the current tool. This function can be called from a menu selected */
/* item or other push button.                                               */
/*                                                                          */
/* We create the message box at the first call and then we manage it or we  */
/* unmanage it depending on the user actions.                               */
/*                                                                          */
/****************************************************************************/


/*--------------------------------------------------------------------------*/
/*                             INCLUDE FILES                                */
/*--------------------------------------------------------------------------*/

/*----------------*/
/* Motif includes */
/*----------------*/
#include <Xm/MessageB.h>

/*-----------------------------*/
/* Motif for Alliance includes */
/*-----------------------------*/
#include MUT_H

#include XAL_H


/*--------------------------------------------------------------------------*/
/*                             WORK VARIABLES                               */
/*--------------------------------------------------------------------------*/
static Widget  xal_version_dialog;
static Boolean xal_version_up = False;



/*--------------------------------------------------------------------------*/
/*                              FUNCTIONS                                   */
/*--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XalUnversionCallback                                           */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .client_data : Not significant.                                */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : A useful  function  for   the Help/On Version command, when    */
/*            the OK button has been pressed.                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XalUnversionCallback( widget, client_data, call_data )
Widget  widget;
XtPointer client_data;
XtPointer call_data;
{
if (xal_version_up == True)
    {
    XtUnmanageChild( xal_version_dialog );
    xal_version_up = False;
    }
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XalCreateOnVersion                                             */
/*                                                                           */
/* IN  ARGS : .Parent : The parent widget of the On Version box.             */
/*            .str_message : Message to put in the On Version box.           */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Creates a message box for the On Version message.              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XalCreateOnVersion( parent, str_message )
Widget parent;
char   *str_message;
{
char     *message;
char     *message_default = "Vx.x";
XmString motif_string;
Arg      args[10];
int      n;

if ( !xal_version_dialog )
    {
    message = ( str_message == NULL ) ? message_default : str_message;

    /*--------------------*/
    /* Set up the message */
    /*--------------------*/
    motif_string  = XmStringCreateLtoR( message, XmSTRING_DEFAULT_CHARSET );

    n = 0;
    XtSetArg( args[n], XmNmessageString, motif_string      ); n++;
    XtSetArg( args[n], XmNtitle,         "On Version....." ); n++;
    xal_version_dialog = XmCreateMessageDialog( parent, "XalOnVersion", args, n );

    /*-----------------------------*/
    /* Callback to unmanage dialog */
    /*-----------------------------*/
    XtAddCallback( xal_version_dialog, XmNokCallback, XalUnversionCallback, (XtPointer)NULL );

    /*--------------------------------------------*/
    /* Get rid of the "Help" and "Cancel" buttons */
    /*--------------------------------------------*/
    XtUnmanageChild( XmMessageBoxGetChild( xal_version_dialog, XmDIALOG_HELP_BUTTON   ));
    XtUnmanageChild( XmMessageBoxGetChild( xal_version_dialog, XmDIALOG_CANCEL_BUTTON ));

    XmStringFree( motif_string );
    }

if (xal_version_up == False)
    {
    XtManageChild( xal_version_dialog );
    xal_version_up = True;
    }
}



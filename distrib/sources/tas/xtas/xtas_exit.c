/****************************************************************************/
/*                                                                          */
/*                         ALLIANCE  CAD  FRAMEWORK                         */
/*                                                                          */
/*    Tool : libXal   Version 1.01                                          */
/*    File :      Exit.c                                                    */
/*                                                                          */
/*    (c) copyright 1993 MASI Laboratory. CAO & VLSI CAD Team               */
/*    All rights reserved.                                                  */
/*    Support : e-mail cao-vlsi@masi.ibp.fr                                 */
/*                                                                          */
/*    Author(s) : Nizar ABDALLAH                      Date : 02/02/1993     */
/*                                                                          */
/*    Modified by : Nizar                             Date : 02/03/1993     */
/*    We don't let the user do nothing except answering the exit question.  */
/*                                                                          */
/*    Modified by :                                   Date : ../../....     */
/*    Modified by :                                   Date : ../../....     */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/* This file offers a user's entry that describes one way for an application*/
/* to exit. It surely does nothing special but makes all tools of a         */
/* framework look  homogeneous.                                             */
/*                                                                          */
/****************************************************************************/

/*--------------------------------------------------------------------------*/
/*                             INCLUDE FILES                                */
/*--------------------------------------------------------------------------*/
/*----------------*/
/* Motif includes */
/*----------------*/
#include "xtas.h"

/*--------------------------------------------------------------------------*/
/*                              VARIABLES				    */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*                              FUNCTIONS                                   */
/*--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XalReallyExit                                                  */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .root_widget : Main question box widget id.                    */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : This is the callback function of Exit box when OK is pressed.  */
/*            Close the display and exit from application.                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XalReallyExit( widget, root_widget, call_data )
Widget  widget;
XtPointer  root_widget;
XtPointer call_data;
{
XtCloseDisplay( XtDisplay( root_widget ) );
MBK_EXIT_KILL = 'N' ;
XyagExitErrorMessage (0);
EXIT( 0 );
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XalCancelExit                                                  */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .question : Question box id.                                   */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : This is the callback function of Exit box when OK is pressed.  */
/*            Close the display and exit from application.                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XalCancelExit( widget, client_data, call_data )
Widget  widget;
XtPointer  client_data;
XtPointer call_data;
{
// XalLeaveLimitedLoop();
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XalExit                                                        */
/*                                                                           */
/* IN  ARGS : .Parent : The parent widget of the Exit box.                   */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : This is a useful function to handle Exit application command.  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XalExit( parent )
Widget parent;
{
Arg       args[10];
int       n;
Atom      WM_DELETE_WINDOW;
static Widget    question = NULL ;
XmString  motif_string;

char      *message = "Are You Sure You Want To Exit?";


if (!question)
{ 
/*--------------------------------------------*/
/* Create an "Are you sure ?" question dialog */
/*--------------------------------------------*/
motif_string = XmStringCreateLtoR( message, XmSTRING_DEFAULT_CHARSET );

n = 0;
XtSetArg( args[n], XmNmessageString, motif_string ); n++;
XtSetArg( args[n], XmNtitle,         "SURE?" ); n++;
XtSetArg( args[n], XmNdialogStyle,    XmDIALOG_APPLICATION_MODAL  ); n++;
question = XmCreateQuestionDialog( parent, "XalExit", args, n);

/*------------------------------*/
/* Get rid of the "Help" button */
/*------------------------------*/
XtUnmanageChild( XmMessageBoxGetChild( question, XmDIALOG_HELP_BUTTON ));

/*---------------------------------------------------------*/
/* Set up callback for activation, for OK & CANCEL buttons */
/*---------------------------------------------------------*/
XtAddCallback( question, XmNokCallback, XalReallyExit, question );
XtAddCallback( question, XmNcancelCallback, XalCancelExit, question );

/*---------------------------*/
/* Free storage for XmString */
/*---------------------------*/
XmStringFree( motif_string );

/*--------------------------------------------------*/
/* Add a callback for the WM_DELETE_WINDOW protocol */
/*--------------------------------------------------*/
 
WM_DELETE_WINDOW = XmInternAtom(XtDisplay(parent), "WM_DELETE_WINDOW", False);
XmAddWMProtocolCallback(XtParent(question), WM_DELETE_WINDOW, XalCancelExit, (XtPointer) question);

 
}  


XtManageChild( question );

// XalLimitedLoop(question);  

}


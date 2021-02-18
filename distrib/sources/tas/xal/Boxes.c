/****************************************************************************/
/*                                                                          */
/*                         ALLIANCE  CAD  FRAMEWORK                         */
/*                                                                          */
/*    Tool : libXal Version 1.01                                            */
/*    File :      Boxes.c                                                   */
/*                                                                          */
/*    (c) copyright 1993 MASI Laboratory. CAO & VLSI CAD Team               */
/*    All rights reserved.                                                  */
/*    Support : e-mail cao-vlsi@masi.ibp.fr                                 */
/*                                                                          */
/*    Author(s) : Nizar ABDALLAH                      Date : 02/02/1993     */
/*                                                                          */
/*    Modified by :Caroline BLED                      Date : 10/31/2002     */
/*    Modified by :                                   Date : ../../....     */
/*    Modified by :                                   Date : ../../....     */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/* Two functions are provided in this file to draw an Error or a Warning    */
/* message in a box. It also forces the user to validate the message before */
/* letting him do anything else.                                            */
/*                                                                          */
/****************************************************************************/


/*--------------------------------------------------------------------------*/
/*                             INCLUDE FILES                                */
/*--------------------------------------------------------------------------*/

/*----------------*/
/* Motif includes */
/*----------------*/
#include <Xm/MessageB.h>
#include <Xm/Protocols.h>
#include <Xm/Text.h>

/*-----------------------------*/
/* Motif for Alliance includes */
/*-----------------------------*/
#include MUT_H

#include XAL_H

#define XAL_MAX_MESS_LINES     20 

/*--------------------------------------------------------------------------*/
/*                              FUNCTIONS                                   */
/*--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XalLeaveBox                                                    */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .root_widget : Main box widget id.                             */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : This is the callback function of Exit box when OK is pressed.  */
/*            Close the display and exit from application.                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XalLeaveBox( widget, root_widget, call_data )
Widget  widget;
XtPointer  root_widget;
XtPointer call_data;
{
    Widget  messw = (Widget) root_widget;
    XtUnmanageChild(messw);
    XalSetCursor (XtParent (messw), NORMAL);
//    XalLeaveLimitedLoop();
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XalTextLineCount                                               */
/*                                                                           */
/* IN  ARGS : .text : the text source.                                       */
/*                                                                           */
/* OUT ARGS : int : line number in the text                                  */
/*                                                                           */
/* OBJECT   : Counts the number of lines in a string character text          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
int XalTextLineCount( text )
char *text;
{
    char    *tmp;
    int      count=0;

    for( tmp=text ; *tmp != '\0' ; tmp++)
        if(*tmp == '\n')    
            count++;

    return count;
}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XalCreateTraceBox                                              */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget id.                                */
/*                                                                           */
/* OUT ARGS : The ErrorBox widget id                                         */
/*                                                                           */
/* OBJECT   : Creates a Motif Error Box to be used later.                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
XalMessageWidgetStruct *XalCreateTraceBox( parent )
Widget parent;
{
int      n;
Arg      args[10];
Atom     WM_DELETE_WINDOW;
Widget   widget, text_scrolled;
XmString text;
XalMessageWidgetStruct  *trace_struct;


trace_struct = (XalMessageWidgetStruct *)mbkalloc(sizeof(XalMessageWidgetStruct));
/* Trace Widget have only a long message widget */
trace_struct->short_mess_widget = NULL;


/* Creation of the long message widget */

text= XmStringCreateSimple("Close");
n = 0;
XtSetArg( args[n], XmNtitle,          "Trace Window" ); n++;
XtSetArg( args[n], XmNdeleteResponse, XmDO_NOTHING    ); n++;
XtSetArg( args[n], XmNokLabelString,  text    ); n++;
XtSetArg( args[n], XmNdialogStyle,    XmDIALOG_APPLICATION_MODAL  ); n++;
widget = XmCreateWorkingDialog( parent, "XalTraceBox", args, n );
XmStringFree(text);
/*-------------------------------------------*/
/* Get rid of "Help" & "Cancel" push Buttons */
/*-------------------------------------------*/
XtUnmanageChild( XmMessageBoxGetChild( widget, XmDIALOG_HELP_BUTTON ) );
XtUnmanageChild( XmMessageBoxGetChild( widget, XmDIALOG_CANCEL_BUTTON ) );
XtUnmanageChild( XmMessageBoxGetChild( widget, XmDIALOG_SYMBOL_LABEL ) );
XtUnmanageChild( XmMessageBoxGetChild( widget, XmDIALOG_MESSAGE_LABEL ) );
XtAddCallback( widget, XmNokCallback, XalLeaveBox, (XtPointer)widget );

n = 0;
XtSetArg( args[n], XmNeditable, False ); n++;
XtSetArg( args[n], XmNcursorPositionVisible, False ); n++;
XtSetArg( args[n], XmNeditMode, XmMULTI_LINE_EDIT ); n++;
XtSetArg( args[n], XmNheight, 750 ); n++;
XtSetArg( args[n], XmNwidth, 580 ); n++;
text_scrolled = XmCreateScrolledText(widget,"XalMessageScrolled",args,n);
/*--------------------------------------------------*/
/* Add a callback for the WM_DELETE_WINDOW protocol */
/*--------------------------------------------------*/
WM_DELETE_WINDOW = XmInternAtom(XtDisplay(parent), "WM_DELETE_WINDOW", False);
XmAddWMProtocolCallback(XtParent(widget), WM_DELETE_WINDOW, XalLeaveBox, (XtPointer)widget);
trace_struct->long_mess_widget = widget;

return( trace_struct);
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XalCreateErrorBox                                              */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget id.                                */
/*                                                                           */
/* OUT ARGS : The ErrorBox widget id                                         */
/*                                                                           */
/* OBJECT   : Creates a Motif Error Box to be used later.                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
XalMessageWidgetStruct *XalCreateErrorBox( parent )
Widget parent;
{
int      n;
Arg      args[10];
Atom     WM_DELETE_WINDOW;
Widget   widget, text_scrolled;
Pixel    bg;
XalMessageWidgetStruct  *error_struct;

error_struct = (XalMessageWidgetStruct *)mbkalloc(sizeof(XalMessageWidgetStruct));

/* Creation of the short message widget */
n = 0;
XtSetArg( args[n], XmNtitle,          "!!! ERROR !!!" ); n++;
XtSetArg( args[n], XmNdeleteResponse, XmDO_NOTHING    ); n++;
XtSetArg( args[n], XmNdialogStyle,    XmDIALOG_APPLICATION_MODAL  ); n++;
widget = XmCreateErrorDialog( parent, "XalErrorBox", args, n );

/*-------------------------------------------*/
/* Get rid of "Help" & "Cancel" push Buttons */
/*-------------------------------------------*/
XtUnmanageChild( XmMessageBoxGetChild( widget, XmDIALOG_HELP_BUTTON ) );
XtUnmanageChild( XmMessageBoxGetChild( widget, XmDIALOG_CANCEL_BUTTON ) );
XtAddCallback( widget, XmNokCallback, XalLeaveBox, (XtPointer)widget );
WM_DELETE_WINDOW = XmInternAtom(XtDisplay(parent), "WM_DELETE_WINDOW", False);
XmAddWMProtocolCallback(XtParent(widget), WM_DELETE_WINDOW, XalLeaveBox, (XtPointer)widget);
error_struct->short_mess_widget = widget;



/* Creation of the long message widget */
n = 0;
XtSetArg( args[n], XmNtitle,          "!!! ERROR !!!" ); n++;
XtSetArg( args[n], XmNdeleteResponse, XmDO_NOTHING    ); n++;
XtSetArg( args[n], XmNdialogStyle,    XmDIALOG_APPLICATION_MODAL  ); n++;
widget = XmCreateErrorDialog( parent, "XalErrorBox", args, n );

/*-------------------------------------------*/
/* Get rid of "Help" & "Cancel" push Buttons */
/*-------------------------------------------*/
XtUnmanageChild( XmMessageBoxGetChild( widget, XmDIALOG_HELP_BUTTON ) );
XtUnmanageChild( XmMessageBoxGetChild( widget, XmDIALOG_CANCEL_BUTTON ) );
XtUnmanageChild( XmMessageBoxGetChild( widget, XmDIALOG_SYMBOL_LABEL ) );
XtUnmanageChild( XmMessageBoxGetChild( widget, XmDIALOG_MESSAGE_LABEL ) );
XtAddCallback( widget, XmNokCallback, XalLeaveBox, (XtPointer)widget );

n = 0;
XtSetArg( args[n], XmNeditable, False ); n++;
XtSetArg( args[n], XmNcursorPositionVisible, False ); n++;
XtSetArg( args[n], XmNeditMode, XmMULTI_LINE_EDIT ); n++;
XtSetArg( args[n], XmNheight, 450 ); n++;
XtSetArg( args[n], XmNwidth, 450 ); n++;
text_scrolled = XmCreateScrolledText(widget,"XalMessageScrolled",args,n);

/*--------------------------------------------------*/
/* Add a callback for the WM_DELETE_WINDOW protocol */
/*--------------------------------------------------*/
WM_DELETE_WINDOW = XmInternAtom(XtDisplay(parent), "WM_DELETE_WINDOW", False);
XmAddWMProtocolCallback(XtParent(widget), WM_DELETE_WINDOW, XalLeaveBox, (XtPointer)widget);
error_struct->long_mess_widget = widget;

return( error_struct );
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XalCreateWarningBox                                            */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget id.                                */
/*                                                                           */
/* OUT ARGS : The Warning box widget id.                                     */
/*                                                                           */
/* OBJECT   : Creates a Motif Warning Box to be used later.                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
XalMessageWidgetStruct *XalCreateWarningBox( parent )
Widget parent;
{
int      n;
Arg      args[10];
Atom     WM_DELETE_WINDOW;
Widget   widget, text_scrolled;
Pixel    bg;
XalMessageWidgetStruct  *warning_struct;

warning_struct = (XalMessageWidgetStruct *)mbkalloc(sizeof(XalMessageWidgetStruct));

/* Creation of the short message widget */
n = 0;
XtSetArg( args[n], XmNtitle,          "!!! WARNING !!!" ); n++;
XtSetArg( args[n], XmNdeleteResponse, XmDO_NOTHING      ); n++;
XtSetArg( args[n], XmNdialogStyle,    XmDIALOG_APPLICATION_MODAL  ); n++;
widget = XmCreateWarningDialog( parent, "XalWarningBox", args, n );


/*-------------------------------------------*/
/* Get rid of "Help" & "Cancel" push Buttons */
/*-------------------------------------------*/
XtUnmanageChild( XmMessageBoxGetChild( widget, XmDIALOG_HELP_BUTTON ) );
XtUnmanageChild( XmMessageBoxGetChild( widget, XmDIALOG_CANCEL_BUTTON ) );
XtAddCallback( widget, XmNokCallback, XalLeaveBox, widget );
/*--------------------------------------------------*/
/* Add a callback for the WM_DELETE_WINDOW protocol */
/*--------------------------------------------------*/
WM_DELETE_WINDOW = XmInternAtom(XtDisplay(parent), "WM_DELETE_WINDOW", False);
XmAddWMProtocolCallback(XtParent(widget), WM_DELETE_WINDOW, XalLeaveBox, (XtPointer )widget);
warning_struct->short_mess_widget = widget;

/* Creation of the long message widget */
n = 0;
XtSetArg( args[n], XmNtitle,          "!!! WARNING !!!" ); n++;
XtSetArg( args[n], XmNdeleteResponse, XmDO_NOTHING      ); n++;
XtSetArg( args[n], XmNdialogStyle,    XmDIALOG_APPLICATION_MODAL  ); n++;
widget = XmCreateWarningDialog( parent, "XalWarningBox", args, n );


/*-------------------------------------------*/
/* Get rid of "Help" & "Cancel" push Buttons */
/*-------------------------------------------*/
XtUnmanageChild( XmMessageBoxGetChild( widget, XmDIALOG_HELP_BUTTON ) );
XtUnmanageChild( XmMessageBoxGetChild( widget, XmDIALOG_CANCEL_BUTTON ) );
XtUnmanageChild( XmMessageBoxGetChild( widget, XmDIALOG_SYMBOL_LABEL ) );
XtUnmanageChild( XmMessageBoxGetChild( widget, XmDIALOG_MESSAGE_LABEL ) );
XtAddCallback( widget, XmNokCallback, XalLeaveBox, widget );

n = 0;
XtSetArg( args[n], XmNeditable, False ); n++;
XtSetArg( args[n], XmNcursorPositionVisible, False ); n++;
XtSetArg( args[n], XmNeditMode, XmMULTI_LINE_EDIT ); n++;
XtSetArg( args[n], XmNheight, 450 ); n++;
XtSetArg( args[n], XmNwidth, 450 ); n++;
text_scrolled = XmCreateScrolledText(widget,"XalMessageScrolled",args,n);
/*--------------------------------------------------*/
/* Add a callback for the WM_DELETE_WINDOW protocol */
/*--------------------------------------------------*/
WM_DELETE_WINDOW = XmInternAtom(XtDisplay(parent), "WM_DELETE_WINDOW", False);
XmAddWMProtocolCallback(XtParent(widget), WM_DELETE_WINDOW, XalLeaveBox, (XtPointer )widget);
warning_struct->long_mess_widget = widget;


return( warning_struct );
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XalDrawMessage                                                 */
/*                                                                           */
/* IN  ARGS : .widget : The precreated Dialog Box Widget id.                 */
/*            .message : The message to be drawn.                            */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Displays a message in a created dialog and forces the user to  */
/*            answer before leaving the box.                                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XalDrawMessage( mess_struct, message )
XalMessageWidgetStruct *mess_struct;
char   message[];
{
int      n;
Arg      args[10];
XmString motif_string;
Widget   text_scrolled;
int      count;
char     c; 

/*--------------------*/
/* Set up the message */
/*--------------------*/
XalSetCursor (XtParent (mess_struct->long_mess_widget), PIRATE);
if(( mess_struct->short_mess_widget == NULL) || ((XalTextLineCount(message) > XAL_MAX_MESS_LINES ))){
  text_scrolled = XtNameToWidget(mess_struct->long_mess_widget, "*XalMessageScrolled");
  XmTextSetString(text_scrolled, message); 
  XtManageChild(text_scrolled);
  XtManageChild(mess_struct->long_mess_widget);
//  XalLimitedLoop(mess_struct->long_mess_widget);

}
else {
  motif_string = XmStringCreateLtoR( message, XmSTRING_DEFAULT_CHARSET );
  n = 0;
  XtSetArg( args[n], XmNmessageString, motif_string ); n++;
  XtSetValues( mess_struct->short_mess_widget, args, n );
  XmStringFree( motif_string );
  XtManageChild(mess_struct->short_mess_widget);
//  XalLimitedLoop(mess_struct->short_mess_widget);
  
}

/*---------------------------*/
/* Free storage for XmString */
/*---------------------------*/

/* XBell(XtDisplay(widget), 50); */

}



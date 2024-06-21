/****************************************************************************/
/*                                                                          */
/*                         ALLIANCE  CAD  FRAMEWORK                         */
/*                                                                          */
/*    Tool : libXal Version 1.01                                            */
/*    File : LimitedLoop.c                                                  */
/*                                                                          */
/*    (c) copyright 1993 MASI Laboratory. CAO & VLSI CAD Team               */
/*    All rights reserved.                                                  */
/*    Support : e-mail cao-vlsi@masi.ibp.fr                                 */
/*                                                                          */
/*    Author(s) : Nizar ABDALLAH & Olivier Florent    Date : 02/03/1993     */
/*                                                                          */
/*    Modified by :                                   Date : ../../....     */
/*    Modified by :                                   Date : ../../....     */
/*    Modified by :                                   Date : ../../....     */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/* This function limits the user actions to a specific window. So you can   */
/* force the user to answer some important question. Otherwise the appli-   */
/* cation will beep. The cursor will also change his look outside the       */
/* specific window. To leave this loop, you must provide a call to the      */
/* XalLeaveLimitedLoop.                                                     */
/*                                                                          */
/****************************************************************************/


/*--------------------------------------------------------------------------*/
/*                             INCLUDE FILES                                */
/*--------------------------------------------------------------------------*/


/*----------------*/
/* Motif includes */
/*----------------*/
#include <X11/Intrinsic.h>


/*-----------------------------*/
/* Motif for Alliance includes */
/*-----------------------------*/
#include MUT_H

#include XAL_H 


/*--------------------------------------------------------------------------*/
/*                             WORK VARIABLES                               */
/*--------------------------------------------------------------------------*/
static int XalLockLoop = 0;


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XalLimitedLoop                                                 */
/*                                                                           */
/* IN  ARGS : .w : The widget that will serve to get the display pointer     */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Forces X server to treat one window's events.                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XalLimitedLoop(MyWidget)

Widget MyWidget;
{
Widget       DialogShell, WShell;
XEvent       Event;
XAnyEvent    *AnyEvent;
XtAppContext Context;

Context = XtWidgetToApplicationContext(MyWidget);

XalLockLoop = 1;

XalSetCursor( XtParent(MyWidget), PIRATE ) ;  

for ( WShell = MyWidget; 
      ! XtIsShell( WShell );
      WShell = XtParent( WShell ) );

while( XalLockLoop == 1 )
  {
    XtAppNextEvent( Context, &Event );

    AnyEvent = (XAnyEvent *)( &Event );

    for ( DialogShell = XtWindowToWidget( AnyEvent->display, AnyEvent->window );
         ((DialogShell != NULL ) && (!XtIsShell( DialogShell )));
         DialogShell = XtParent(DialogShell));

    switch ( AnyEvent->type )
    {
      case KeyRelease    :
      case ButtonRelease : 
 
        if (WShell == DialogShell) XtDispatchEvent(&Event);

        break;
 
      case KeyPress      :
      case ButtonPress   :
       if (WShell != DialogShell)
        {
          XBell( AnyEvent->display, 0 );
          XtPopup( WShell, XtGrabNone ); 
        }
        else
        {
          XtDispatchEvent( &Event );
        }
        break;
 
      default :

        XtDispatchEvent( &Event );
    }
  }

XalLockLoop = 1;

XalSetCursor( MyWidget, NORMAL ); 
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XalLeaveLimitedLoop                                            */
/*                                                                           */
/* IN  ARGS : None                                                           */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Ends the limited loop execution.                               */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XalLeaveLimitedLoop()
{
XalLockLoop = 0;
}



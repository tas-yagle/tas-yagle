/****************************************************************************/
/*                                                                          */
/*                         ALLIANCE  CAD  FRAMEWORK                         */
/*                                                                          */
/*    Tool : libXal   Version 1.01                                          */
/*    File :      Cursor.c                                                  */
/*                                                                          */
/*    (c) copyright 1993 MASI Laboratory. CAO & VLSI CAD Team               */
/*    All rights reserved.                                                  */
/*    Support : e-mail cao-vlsi@masi.ibp.fr                                 */
/*                                                                          */
/*    Author(s) : Nizar ABDALLAH                      Date : 02/02/1993     */
/*                                                                          */
/*    Modified by : Nizar                             Date : 02/03/1993     */
/*          Add the function XalPirateCursor.                               */
/*                                                                          */
/*    Modified by : Lionel PROTZENKO                  Date : 04/04/1997     */
/*    Modified by :                                   Date : ../../....     */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/* This file describes a set of functions which provide manipulations on    */
/* cursor look. One is to create a cursor with a particular look. This is   */
/* an internal function not accessible to users.                            */
/*                                                                          */
/* When called, the second function makes the cursor looks like a watch.    */
/* This function can be used to tell the user that some non visible         */
/* treatments are in progress.                                              */
/*                                                                          */
/* When called, the third function makes the cursor looks like a pirate flag*/
/*                                                                          */
/* The last  one makes the cursor appears normally. This function can be    */
/* called after a treatment to tell the user that it's up to him again. All */
/* the bufferized events are cleared before returning.                      */
/*                                                                          */
/****************************************************************************/


/*--------------------------------------------------------------------------*/
/*                             INCLUDE FILES                                */
/*--------------------------------------------------------------------------*/
/*----------------*/
/* Motif includes */
/*----------------*/
#include <X11/cursorfont.h>
#include <X11/Intrinsic.h>

/*-----------------------------*/
/* Motif for Alliance includes */
/*-----------------------------*/
#include MUT_H

#include XAL_H

/*--------------------------------------------------------------------------*/
/*                             WORK VARIABLES                               */
/*--------------------------------------------------------------------------*/
static Widget  xal_topshell, xal_diashell;

static Cursor xal_busycursor   = (Cursor)NULL;
static Cursor xal_normalcursor = (Cursor)NULL;
static Cursor xal_piratecursor = (Cursor)NULL;


/*--------------------------------------------------------------------------*/
/*                              FUNCTIONS                                   */
/*--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XalSetPointer                                                  */
/*                                                                           */
/* IN  ARGS : .widget : The widget that will serve to get the display pointer*/
/*            .cursorid : A previous created cursor.                         */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Makes cursorid the current cursor of the display.              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XalSetPointer( widget, cursorid )
Widget widget;
Cursor cursorid;
{
XDefineCursor(XtDisplay(widget), XtWindow(widget), cursorid);
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XalSetCursor                                                   */
/*                                                                           */
/* IN  ARGS : .w : The widget that will serve to get the display pointer.    */
/*            .t : The type of the cursor wanted			     */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Makes the cursor pointer appears as defined type               */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XalSetCursor( w, t )
Widget w;
int    t;
{
Display *dpy = NULL;
XEvent  event;

if ( w != (Widget)0 ) 
  {
  /*---------------------------------------*/
  /* Locate the shell we are interested in */
  /*---------------------------------------*/
  for (xal_diashell = w; !XtIsShell(xal_diashell);xal_diashell = XtParent(xal_diashell)) ;

  /*------------------------------------*/
  /* Locate it's primary window's shell */
  /*------------------------------------*/
  for (xal_topshell = xal_diashell; !XtIsTopLevelShell(xal_topshell);xal_topshell = XtParent(xal_topshell)) ;

  /*--------------------------------------------------------*/
  /* If this is the first time, then create the cursor type */
  /*--------------------------------------------------------*/

  if ( t == NORMAL ) 
        dpy = XtDisplay( xal_diashell ) ;
  
  switch ( t ) {
    case NORMAL : if ( !xal_normalcursor )
                    xal_normalcursor = XCreateFontCursor( dpy,XC_top_left_arrow );
			
    		  /*------------------------------*/
    		  /* Cancel all bufferized events */
    		  /*------------------------------*/
    		  while (XCheckMaskEvent( dpy,
           		                  ButtonPressMask | ButtonReleaseMask |
					  ButtonMotionMask | PointerMotionMask |
					  KeyPressMask, &event) );
        	  /*------------*/
        	  /* do nothing */
        	  /*------------*/
 
    		  XalSetPointer(xal_topshell,xal_normalcursor);
    		  XalForceUpdate( xal_topshell ); 
		  break; 

    case WAIT   : if ( !xal_busycursor )
                    {  
	     	      dpy = XtDisplay( xal_diashell ) ;
      		      xal_busycursor = XCreateFontCursor( dpy,XC_watch ) ;
		    }

		  XalSetPointer(xal_topshell, xal_busycursor);
    		  XalForceUpdate( xal_topshell ); 
                  break; 

    case PIRATE : if ( !xal_piratecursor )
		    {
                      dpy = XtDisplay( xal_diashell ) ;
      		      xal_piratecursor = XCreateFontCursor( dpy,XC_pirate ) ;
    	 	    }

		  XalSetPointer(xal_topshell, xal_piratecursor);
    		  XalForceUpdate( xal_topshell ); 
                  break;

    default     : break; 
    }
  }
}

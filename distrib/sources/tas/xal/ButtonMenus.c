/****************************************************************************/
/*                                                                          */
/*                         ALLIANCE  CAD  FRAMEWORK                         */
/*                                                                          */
/*    Tool : libXal Version 1.01                                            */
/*    File :      ButtonMenus.c                                             */
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
/* This file provides a very beautiful way to show the most useful user's   */
/* command by displaying a row of drawing buttons from bitmap files.        */
/* It is enough flexible to let you use it in a custom way.                 */
/*                                                                          */
/****************************************************************************/


/*--------------------------------------------------------------------------*/
/*                             INCLUDE FILES                                */
/*--------------------------------------------------------------------------*/

/*----------------*/
/* Motif includes */
/*----------------*/
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>

/*-----------------------------*/
/* Motif for Alliance includes */
/*-----------------------------*/
#include MUT_H

#include XAL_H


#define XalMAX_ARGS 31

XalBitmapsStruct *XalBitmaps = NULL ;

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XalButtonMenus                                                 */
/*                                                                           */
/* IN  ARGS : .parent        : Parent widget for the rowcol of buttons.      */
/*            .menus         : The icons menu structure.                     */
/*            .args          : The user rowcol args.                         */
/*            .n             : User rowcol args number.                      */
/*            .button_width  : The button width for all Icons.               */
/*            .button_height : The button width for all Icons.               */
/*                                                                           */
/*                                                                           */
/* OUT ARGS : The widget id of the row that contains all the buttons.        */
/*                                                                           */
/* OBJECT   : This function provides an easy way to create some set of       */
/*            drawable buttons associated with some user functions.          */
/*            In order to be flexible, this function accepts some user       */
/*            args for the rowcolumn widget.                                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/
Widget XalButtonMenus( parent, menus, args, n, button_width, button_height )
Widget parent;
struct XalButtonsItem *menus;
Arg    args[];
int    n;
int    button_width, button_height;
{
Widget      row_widget, button, err_w;
char *      filename ;
Arg         l_args[XalMAX_ARGS];
GC          gc;
Pixel       fg, bg;
Pixmap      pixmap;
int         i;

if ( n >= XalMAX_ARGS )
    {
    err_w = XalCreateErrorBox( parent );
    XtManageChild( err_w );
    XalForceUpdate( err_w );
    XalDrawMessage( err_w, "XalButtonMenus: Resource table is too large" );
    MBK_EXIT_KILL = 'N' ;
    EXIT( 1 );
    }

/*------------------------------------------------*/
/* Copy the incoming args into the local arg list */
/*------------------------------------------------*/
for (i=0; i<n; i++) l_args[i] = args[i];

/*----------------------------------------*/
/* Create row widget for buttons icons    */
/*----------------------------------------*/
XtSetArg( l_args[n], XmNpacking,     XmPACK_COLUMN ); n++;
XtSetArg( l_args[n], XmNentryCallback, NULL ); n++;
row_widget = XmCreateRowColumn( parent, "XalButtonMenusRow", l_args, n );

/*-----------------------------------------------------------*/
/* get the foreground and background colors of the rowcol    */
/* so the gc (DrawnButtons) will use them to render pixmaps. */
/*-----------------------------------------------------------*/
XtVaGetValues(row_widget, XmNforeground, &fg, XmNbackground, &bg, NULL);
gc = XCreateGC(XtDisplay(row_widget), RootWindowOfScreen(XtScreen(row_widget)), 0, 0);
XSetForeground(XtDisplay(row_widget), gc, fg);
XSetBackground(XtDisplay(row_widget), gc, bg);

/*---------------------------------------------------*/
/* Simply loop thru the bitmaps creating the buttons */
/*---------------------------------------------------*/
for (i = 0; menus[i].pix_file != -1 ; i++)
    {
    if ( menus[i].selected == True )
        {
        /*----------------------------------------------------------*/
        /* the pixmap is taken from the name given in the structure */
        /*----------------------------------------------------------*/
        filename = XalBitmaps[menus[i].pix_file].name ;
        pixmap = XmGetPixmap(XtScreen(row_widget), filename , fg, bg);
        if(pixmap == XmUNSPECIFIED_PIXMAP)
         {
          pixmap = XCreatePixmapFromBitmapData( XtDisplay(row_widget),
                                   RootWindowOfScreen(XtScreen(row_widget)),
                                   XalBitmaps[menus[i].pix_file].bitmap,
                                   XalBitmaps[menus[i].pix_file].width,
                                   XalBitmaps[menus[i].pix_file].height,
                                   fg, bg,
                                   DefaultDepthOfScreen(XtScreen(row_widget)));
         }
        button = XtVaCreateManagedWidget("dbutton", xmPushButtonWidgetClass, row_widget,
                                         XmNlabelType,         XmPIXMAP,
                                         XmNlabelPixmap,       pixmap,
                                         XmNwidth,             button_width,
                                         XmNheight,            button_height,
                                         XmNpushButtonEnabled, True,
                                         XmNshadowType,        XmSHADOW_ETCHED_OUT,
                                         NULL);

        /*-------------------------------------------------*/
        /* if this button is selected, execute the program */
        /*-------------------------------------------------*/
        XtAddCallback(button, XmNactivateCallback, menus[i].callback, menus[i].callback_data);
        }
    }
return(row_widget);
}



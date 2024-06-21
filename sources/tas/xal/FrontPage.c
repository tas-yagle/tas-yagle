/****************************************************************************/
/*                                                                          */
/*                         ALLIANCE  CAD  FRAMEWORK                         */
/*                                                                          */
/*    Tool : libXal Version 1.01                                            */
/*    File :      FrontPage.c                                               */
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
/* This is a complicated function of the libXal  library. In order to       */
/* present a homogeneous aspect of all ALLIANCE cad framework tools, we can */
/* start by having the same front page. To use this function, each tool must*/
/* include <Xal101.h> file.                                                 */
/*                                                                          */
/* Concerning the programming precautions, you must take care of reserving  */
/* a main window of 850x450 and a minimum of 650x350 so that the front page */
/* may be entirely visible.                                                 */
/*                                                                          */
/* The other advice is to add in your resource's list the following line :  */
/*                                                                          */
/*           "*.XalToolName.foreground: Red"                                */
/*                                                                          */
/* We choose to avoid hard-coding colors in our routines; We provide enough */
/* flexibility for the user to configure colors through resource databases  */
/* or interactively in the application.                                     */
/*                                                                          */
/* We first wrote this function for all ALLIANCE Motif Tools. This explains */
/* why we have default values for most arguments. But you can pass whatever */
/* you like in order to use this function for other frameworks.             */
/*                                                                          */
/* The last obligation is to pass an xmFormWidgetClass widget as the parent */
/* so that all attachments could be correctly done.                         */
/*                                                                          */
/* Note : The ToolName will appear as a PushButton. When pressed, the X     */
/*        server will call your callback function passed as argument.       */
/*                                                                          */
/****************************************************************************/


/*--------------------------------------------------------------------------*/
/*                             INCLUDE FILES                                */
/*--------------------------------------------------------------------------*/

#include <time.h>

/*----------------*/
/* Motif includes */
/*----------------*/
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/PushB.h>
#include <Xm/TextF.h>
#include <Xm/Text.h>

/*-----------------------------*/
/* Motif for Alliance includes */
/*-----------------------------*/
#include MUT_H

#include XAL_H
#include "Logo.h"


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XalFrontPage                                                   */
/*                                                                           */
/* IN  ARGS : .parent     : Parent widget, MUST BE A FORM CLASS.             */
/*            .cad_title  : 1st title; if NULL then "ALLIANCE CAD FRAMEWORK".*/
/*            .tool_name  : Tool Name; if NULL then "xxxxxx".                */
/*	      .tool_title : Description; if NULL then BLANK.                 */
/*            .version    : Version; if NULL then "Version x.x".             */
/*            .right      : Copyrights; if NULL then "(c) MASI...".          */
/*            .callback_func : Function to be executed when the tool         */
/*                             name PushButton is pressed.                   */
/*                                                                           */
/* OUT ARGS : The widget id of the frame that contains all the front page.   */
/*                                                                           */
/* OBJECT   : Creates a front page within a frame widget.                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XalFrontPage( parent, cad_title, cad_version, cad_patch, tool_name, tool_title )
Widget parent;
char   *cad_title, *cad_version, *cad_patch, *tool_name, *tool_title;
{
Widget 	    PixmapLabel;
Widget      form_t, frame_widget, label_widget;
XmString    text, text_bis, text_ter;
Display     *dpy;
XmFontList  fontlist;
XFontStruct *font;
Arg         args[20];
int         n, ecart;
time_t timer;
char day[4], month[4];
int year, nday, hour, minute, second;
char copyright[16];
char cad_versionpatch[1024];

char     *def_cad_title    = "AVERTEC Release ";
char     *def_cad_version  = "x.x";
char     *def_cad_patch    = "px";
char     *def_tool_name    = "xxxxxxxx";
char     *def_tool_title   = "        ";
char     *rights_1         = "\251 ";
char     *rights_2         = "\nE-mail support: support@avertec.com";

time(&timer);
sscanf(ctime(&timer), "%s %s %d %d:%d:%d %d", day, month, &nday, &hour, &minute, &second, &year); 

dpy      = XtDisplay( parent );
font     = XLoadQueryFont( dpy, "-*-helvetica-bold-o-*--24-*" );
fontlist = XmFontListCreate( font, "front_charset1" );
font     = XLoadQueryFont( dpy, "-*-helvetica-bold-r-*--18-*" );
fontlist = XmFontListAdd( fontlist, font, "front_charset2" );
font     = XLoadQueryFont( dpy, "-*-helvetica-bold-r-*--12-*" );
fontlist = XmFontListAdd( fontlist, font, "front_charset4" );


if ( cad_title    == NULL ) cad_title    = def_cad_title;
if ( cad_version  == NULL ) cad_version  = def_cad_version;
if ( cad_patch    == NULL ) cad_patch    = def_cad_patch;
if ( tool_name    == NULL ) tool_name    = def_tool_name;
if ( tool_title   == NULL ) tool_title   = def_tool_title;


/*----------------------------------------*/
/* Create frame widget to add a 3D effect */
/*----------------------------------------*/
n = 0;
XtSetArg( args[n], XmNtopAttachment,    XmATTACH_POSITION  ); n++;
XtSetArg( args[n], XmNtopPosition,      8                  ); n++;
XtSetArg( args[n], XmNbottomAttachment, XmATTACH_POSITION  ); n++;
XtSetArg( args[n], XmNbottomPosition,   92                 ); n++;
XtSetArg( args[n], XmNleftAttachment,   XmATTACH_POSITION  ); n++;
XtSetArg( args[n], XmNleftPosition,     4                 ); n++;
XtSetArg( args[n], XmNrightAttachment,  XmATTACH_POSITION  ); n++;
XtSetArg( args[n], XmNrightPosition,    96                 ); n++;
frame_widget = XmCreateFrame( parent, "XalFrontFrame", args, n );
XtManageChild( frame_widget );

/*----------------------------------------------*/
/* Create a Form widget to manage child widgets */
/*----------------------------------------------*/
n = 0;
form_t = XtCreateManagedWidget( "XalFrontForm", xmFormWidgetClass, frame_widget, args, n );

/*--------------------------------------*/
/* Create an icon to make things pretty */
/*--------------------------------------*/
#ifndef AVERTEC
label_widget = XmCreateLabel( form_t, "ColorXtasLogo", args, n);
PixmapLabel = XalCreatePixmapLabel( label_widget, LogoAsim, NULL );
XtManageChild( PixmapLabel );
#endif



sprintf (cad_versionpatch, "%s%s", cad_version, cad_patch);

text_bis     = XmStringCreateLtoR( cad_title,   "front_charset2" );
text_ter     = XmStringCreateLtoR( cad_versionpatch, "front_charset2" );
text         = XmStringConcat( text_bis, text_ter );
label_widget = XtVaCreateManagedWidget( "XalCadTitle", xmLabelWidgetClass, form_t,
                                         XmNfontList,        fontlist,
                                         XmNlabelString,     text,
                                         XmNtopAttachment,   XmATTACH_POSITION,
                                         XmNtopPosition,     10,
                                         XmNrightAttachment, XmATTACH_POSITION,
                                         XmNrightPosition,   100,
                                         XmNleftAttachment,  XmATTACH_POSITION,
                                         XmNleftPosition,    0,
                                         NULL);
XmStringFree( text_bis ); XmStringFree( text_ter ); XmStringFree( text );

ecart        = strlen( tool_name ) * 3 / 2;
text     = XmStringCreateLtoR( tool_name,        "front_charset1" );
label_widget = XtVaCreateManagedWidget( "XalToolName", xmLabelWidgetClass, form_t,
                                         XmNfontList,         fontlist,
                                         XmNlabelString,      text,
                                         XmNtopAttachment,    XmATTACH_WIDGET,
                                         XmNtopWidget,        label_widget,
                                         XmNtopOffset,        5,
                                         XmNrightAttachment,  XmATTACH_POSITION,
                                         XmNrightPosition,    55+ecart,
                                         XmNleftAttachment,   XmATTACH_POSITION,
                                         XmNleftPosition,     45-ecart,
                                         NULL);

XmStringFree( text );

text = XmStringCreateLtoR( tool_title, "front_charset2" );
label_widget = XtVaCreateManagedWidget( "XalToolTitle", xmLabelWidgetClass, form_t,
                                         XmNfontList,        fontlist,
                                         XmNlabelString,     text,
                                         XmNtopAttachment,   XmATTACH_WIDGET,
                                         XmNtopWidget,       label_widget,
                                         XmNtopOffset,       5,
                                         XmNrightAttachment, XmATTACH_POSITION,
                                         XmNrightPosition,   100,
                                         XmNleftAttachment,  XmATTACH_POSITION,
                                         XmNleftPosition,    0,
                                         NULL);
XmStringFree( text );

text     = XmStringCreateLtoR( rights_1,  "front_charset4" );
sprintf(copyright,"1998-%d",year);
text_bis = XmStringCreateLtoR( copyright, "front_charset4" );
text_ter = XmStringConcat( text, text_bis );
XmStringFree( text ); XmStringFree( text_bis );

text_bis = XmStringCreateLtoR( rights_2, "front_charset4" );
text     = XmStringConcat( text_ter, text_bis );
label_widget = XtVaCreateManagedWidget( "XalToolRights", xmLabelWidgetClass, form_t,
                                         XmNfontList,         fontlist,
                                         XmNlabelString,      text,
                                         XmNtopAttachment,    XmATTACH_WIDGET,
                                         XmNtopWidget,        label_widget,
                                         XmNtopOffset,        5,
                                         XmNrightAttachment,  XmATTACH_FORM,
                                         XmNleftAttachment,   XmATTACH_FORM,
//                                         XmNbottomAttachment,  XmATTACH_POSITION,
//                                         XmNbottomPosition,    90,
                                         NULL);
XmStringFree( text_bis ); XmStringFree( text_ter ); XmStringFree( text );

}



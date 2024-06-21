/*------------------------------------------------------------\
|                                                             |
| Tool    :                    XYAG                           |
|                                                             |
| File    :                  Panel.c                          |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                        Lester Anthony                       |
|                                                             |
\------------------------------------------------------------*/

# include <stdio.h>
# include <time.h>
# include <X11/Intrinsic.h>
# include <Xm/Xm.h>
# include <Xm/Frame.h>
# include <Xm/Form.h>
# include <Xm/List.h>
# include <Xm/Text.h>
# include <Xm/TextF.h>
# include <Xm/PushB.h>
# include <Xm/DialogS.h>
# include <Xm/Label.h>
# include <Xm/LabelG.h>


# include MUT_H
# include MLO_H
# include XSB_H
# include XTB_H
# include XMX_H
# include XMH_H

# include "XMH_panel.h" 


#ifdef AVERTEC
#include AVT_H
#include "AVERTECcolor.xpm"
#else
#include "LIP6color.xpm"
#endif

   XyagPanelItem XyagHelpPresentPanel =

         {
           "Xyag present",
           1,
           0,
           XYAG_HELP_PRESENT_X,
           XYAG_HELP_PRESENT_Y,
           700,
           360,
           0,
           0,
           (Widget)NULL,
           (Widget)NULL,
           (Widget)NULL,
           (Widget)NULL,
           (XyagPanelButtonItem *)NULL
         };

   int XyagHelpPresentDefaultValues[ 5 ] =

         {
           XYAG_HELP_PRESENT_X,
           XYAG_HELP_PRESENT_Y,
           700, 360, 0
         };

/*------------------------------------------------------------\
|                    XyagBuildPresentPanel                   |
\------------------------------------------------------------*/
void XyagFrontPage( parent, cad_title, cad_version, cad_patch, tool_name, tool_title )
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
Widget      PanelButton;

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
//n = 0;
//XtSetArg( args[n], XmNtopAttachment,    XmATTACH_POSITION  ); n++;
//XtSetArg( args[n], XmNtopPosition,      8                  ); n++;
//XtSetArg( args[n], XmNbottomAttachment, XmATTACH_POSITION  ); n++;
//XtSetArg( args[n], XmNbottomPosition,   92                 ); n++;
//XtSetArg( args[n], XmNleftAttachment,   XmATTACH_POSITION  ); n++;
//XtSetArg( args[n], XmNleftPosition,     4                 ); n++;
//XtSetArg( args[n], XmNrightAttachment,  XmATTACH_POSITION  ); n++;
//XtSetArg( args[n], XmNrightPosition,    96                 ); n++;
//frame_widget = XmCreateFrame( parent, "XalFrontFrame", args, n );
//XtManageChild( frame_widget );

/*----------------------------------------------*/
/* Create a Form widget to manage child widgets */
/*----------------------------------------------*/
//n = 0;
//form_t = XtCreateManagedWidget( "XalFrontForm", xmFormWidgetClass, frame_widget, args, n );
form_t = parent;

/*--------------------------------------*/
/* Create an icon to make things pretty */
/*--------------------------------------*/

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


text     = XmStringCreateLtoR( "\n",         "front_charset2" );
text_bis = XmStringCreateLtoR( tool_title, "front_charset2" );
text     = XmStringConcat( text, text_bis );
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
sprintf(copyright,"1999-%d",year);
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

text = XmStringCreateLtoR ("OK", "front_charset2");
PanelButton = XtVaCreateManagedWidget( "",
                                          xmPushButtonWidgetClass, 
                                          form_t,
                                          XmNfontList        , fontlist,
                                          XmNlabelString     , text,
                                          XmNshadowThickness , 3,
                                          XmNtopAttachment   , XmATTACH_WIDGET,
                                          XmNtopWidget       , label_widget,
                                          XmNtopOffset       , 15,
                                          XmNrightAttachment , XmATTACH_POSITION,
                                          XmNrightPosition   , 60,
                                          XmNleftAttachment  , XmATTACH_POSITION,
                                          XmNleftPosition    , 40,
                                          NULL
                                        );
XmStringFree( text  );
  XtAddCallback( PanelButton, 
                 XmNactivateCallback,
                 XyagExitDialogCallback, NULL );

}


void XyagBuildPresentPanel()

{
  Widget       PanelLabel;
  Widget       PanelButton;
  Pixmap       PanelPixmap;
  XmString     PanelString;
  XmString     PanelString1;
  XmString     PanelString2;
  XmFontList   PanelFontList;
  XFontStruct *PanelFont;
  Display     *PanelDisplay;
  char         Buffer[ 64 ];
  Arg          Args[3];
  int          n;
  Widget       form;

//  PanelDisplay  = XtDisplay( XyagMainWindow );
//  PanelFont     = XLoadQueryFont( PanelDisplay, "-*-helvetica-bold-o-*--24-*" );
//  PanelFontList = XmFontListCreate( PanelFont, "Panel_charset1" );
//  PanelFont     = XLoadQueryFont( PanelDisplay, "-*-helvetica-bold-r-*--18-*" );
//  PanelFontList = XmFontListAdd( PanelFontList, PanelFont, "Panel_charset2" );
//  PanelFont     = XLoadQueryFont( PanelDisplay, "-*-helvetica-bold-r-*--12-*" );
//  PanelFontList = XmFontListAdd( PanelFontList, PanelFont, "Panel_charset4" );

  XtSetArg( Args[0], XmNshadowType    , XmSHADOW_ETCHED_IN );
  XtSetArg( Args[1], XmNdeleteResponse, XmDO_NOTHING       );
  sprintf(Buffer, "Welcome to %s", XYAGLE_NAME);
  XtSetArg( Args[2], XmNtitle         , mbkstrdup(Buffer));

  XyagHelpPresentPanel.PANEL = 

    XmCreateFormDialog( XyagMainWindow, XyagHelpPresentPanel.TITLE, Args, 3);

  XtAddCallback( XyagHelpPresentPanel.PANEL, XmNdestroyCallback,
                 XyagDestroyDialogCallback, NULL );

  XyagHelpPresentPanel.PANEL_FORM = 

     XtVaCreateManagedWidget( "",
                              xmFormWidgetClass,
                              XyagHelpPresentPanel.PANEL,
                              XmNtopAttachment    , XmATTACH_FORM,
                              XmNbottomAttachment , XmATTACH_FORM,
                              XmNleftAttachment   , XmATTACH_FORM,
                              XmNrightAttachment  , XmATTACH_FORM,
                              NULL
                            );

  XyagHelpPresentPanel.FRAME = 

     XtVaCreateManagedWidget( "",
                              xmFrameWidgetClass,
                              XyagHelpPresentPanel.PANEL_FORM,
                              XmNtopAttachment   , XmATTACH_POSITION,
                              XmNtopPosition     , 05,
                              XmNbottomAttachment, XmATTACH_POSITION,
                              XmNbottomPosition  , 95,
                              XmNleftAttachment  , XmATTACH_POSITION,
                              XmNleftPosition    , 05,
                              XmNrightAttachment , XmATTACH_POSITION,
                              XmNrightPosition   , 95,
                              NULL
                            );

  XyagHelpPresentPanel.FORM = 

    XtVaCreateManagedWidget( "",
                             xmFormWidgetClass,
                             XyagHelpPresentPanel.FRAME,
                             XmNtopAttachment   , XmATTACH_POSITION,
                             XmNtopPosition     , 05,
                             XmNbottomAttachment, XmATTACH_POSITION,
                             XmNbottomPosition  , 95,
                             XmNleftAttachment  , XmATTACH_POSITION,
                             XmNleftPosition    , 05,
                             XmNrightAttachment , XmATTACH_POSITION,
                             XmNrightPosition   , 95,
                             NULL
                           );


  

#ifdef AVERTEC
   PanelPixmap = XyagCreateColorPixmap( XyagMainWindow, AVERTECcolor);
#else
   PanelPixmap = XyagCreateColorPixmap( XyagMainWindow, LIP6color);
#endif
 
   PanelLabel = XtVaCreateManagedWidget( "",
                                        xmLabelGadgetClass, 
                                        XyagHelpPresentPanel.FORM,
		                                XmNlabelType   , XmPIXMAP,
                                        XmNlabelPixmap , PanelPixmap,
                                        XmNtopAttachment, XmATTACH_FORM,
                                        XmNbottomAttachment, XmATTACH_FORM,
                                        XmNleftAttachment, XmATTACH_FORM,
			                               NULL
                                      );

   n = 0;
   XtSetArg (Args[n], XmNtopAttachment,      XmATTACH_FORM  ); n++;
   XtSetArg (Args[n], XmNbottomAttachment,   XmATTACH_FORM   ); n++;
   XtSetArg (Args[n], XmNleftAttachment,     XmATTACH_WIDGET   ); n++;
   XtSetArg (Args[n], XmNleftWidget,         PanelLabel       ); n++;
   form = XtCreateManagedWidget( "XyagForm", xmFormWidgetClass, XyagHelpPresentPanel.FORM, Args, n );

XyagFrontPage( form,      
            NULL,                  
            AVERTEC_VERSION,      
            PATCH_NUM,      
            "XYAGLE",             
            XYAGLE_ABOUT);            
}

/*------------------------------------------------------------\
|                                                             |
|                    XyagEnterPresentPanel                    |
|                                                             |
\------------------------------------------------------------*/

void XyagEnterPresentPanel()

{
  XyagEnterPanel( &XyagHelpPresentPanel );
  XyagLimitedLoop( XyagHelpPresentPanel.PANEL );
  XyagExitPanel( &XyagHelpPresentPanel );
}

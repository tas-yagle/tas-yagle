/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                            |
|                                                             |
| File    :                  Motif.c                          |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                        Lester Anthony                       |
|                                                             |
\------------------------------------------------------------*/

#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/MainW.h>

/*#include MUT_H
#include MLO_H
#include XSB_H
#include XTB_H
#include XMX_H
#include XMS_H*/
#include BEH_H
#include BHL_H
#include MUT_H
#include MLO_H
#include LOG_H
#include CGV_H
#include BVL_H
#include BEF_H

#include XSB_H
#include XYA_H
#include XMX_H
#include XTB_H
#include XMT_H
#include INF_H
#include YAG_H



#include "XYA_error.h"
#include "XMX_motif.h"
#include "XMX_graphic.h"
#include "XMX_event.h"
#include "XMX_menu.h"
#include "XMX_color.h"
#include "XMX_panel.h"
#include "XMX_icon.h"
#include "XMX_scroll.h"

XtAppContext    XyagleApplication;
Widget          XyagTopLevel;
Widget          XyagMainWindow;
Widget          XyagViewport;

Dimension       XyagViewDx;
Dimension       XyagViewDy;



static String   XyagFallBacks[] =
{
#ifdef xcones
    "*.XyagMainWindow.iconName : XCones",
#else
    "*.XyagMainWindow.iconName : Xyagle",
#endif
    "*XyagMenuBar*fontList : -adobe-helvetica-bold-r-normal--*-120-*-*-*-*-iso8859-*",
    "*foreground : black",
    "*background : gray",
    "*fontList : -*-helvetica-medium-r-*--12-*",
    NULL
};

#ifdef xcones
static char     XyagTopLevelTitle[128] = "XCones : ";
#else
static char     XyagTopLevelTitle[128] = "Xyagle : ";
#endif

#ifdef AVERTEC_LICENSE
void 
XyagleAvtlicence()
{
    if (avt_givetoken("YAGLE_LICENSE_SERVER", "xyagle") != AVT_VALID_TOKEN) XyagExit(1);
    XtAppAddTimeOut(XyagleApplication, 60000, XyagleAvtlicence, NULL);
}
#endif

void DisplayMessage(char *content)
{
  XyagWarningMessage(XyagMainWindow, content);
}


/*------------------------------------------------------------\
|                     XyagChangeTopLevelTitle                 |
\------------------------------------------------------------*/

void 
XyagChangeTopLevelTitle(Title)
    char           *Title;
{
    int n = 7; /* Xyagle = 6 characters + 1 blank */
    
    if(XyagCallbackFileQuitFonction != NULL) {
        strcpy (XyagTopLevelTitle, "Xtas Schematics ");
        n = 16;
    }
    
    if (Title != (char *) NULL) {
           strcpy(XyagTopLevelTitle + n, Title);
    }
    else {
        XyagTopLevelTitle[n] = '\0';
    }

    XtVaSetValues(XyagTopLevel, XmNtitle, XyagTopLevelTitle, NULL);
}

/*------------------------------------------------------------\
|                    XyagInitializeRessources                 |
\------------------------------------------------------------*/

void 
XyagInitializeRessources(argc, argv, ptcolormap)
    int            *argc;
    char           *argv[];
    Colormap       *ptcolormap;
{
    Arg             Args[2];
    
    MBK_EXIT_KILL = 'Y';

    XyagTopLevel = XtVaAppInitialize(&XyagleApplication,
                                     XYAGLE_NAME,
                                     NULL,
                                     0,
                                     argc,
                                     argv,
                                     XyagFallBacks,
                                     NULL);

    if (!ptcolormap) XyagInitializeColorMap();
    else XyagSetColorMap(*ptcolormap);

    XyagLoadColors();
     
    XyagLoadTopLevelConfig();

    XyagMainWindow = XtVaCreateManagedWidget("XyagMainWindow",
                                             xmMainWindowWidgetClass,
                                             XyagTopLevel,
                                             XmNscrollingPolicy, XmAPPLICATION_DEFINED,
                                             NULL);

    XyagInitializeIcon();
    XyagInitializeMenu();
    XyagInitializeScroll();
    XyagInitializePanel();
    XyagInitializeGraphic();
    XyagInitializeEvent();
    XyagInitializeColors();
    XyagInitializeGraphicContext();

    XmMainWindowSetAreas(XyagMainWindow, XyagMenuBar, NULL, XyagHScroll, XyagVScroll, XyagGraphicWindow);

    XtSetArg(Args[0], XmNallowShellResize, True);
    XtSetArg(Args[1], XmNdeleteResponse, XmDO_NOTHING);
    XtSetValues(XyagTopLevel, Args, 2);

    XtRealizeWidget(XyagTopLevel);

#ifdef AVERTEC_LICENSE
    XyagleAvtlicence();
#endif
}

/*------------------------------------------------------------\
|                    XyagCreateMainWindow                     |
\------------------------------------------------------------*/

Widget 
XyagCreateMainWindow(toplevel,application, ptcolormap)
Widget toplevel ;
XtAppContext application ;
Colormap *ptcolormap;
{
    Arg      Args[10];

     XyagleApplication = application ;
     XyagTopLevel = XtVaCreatePopupShell( NULL, topLevelShellWidgetClass,
                                          toplevel,
                                          NULL) ;

    if (!ptcolormap) XyagInitializeColorMap();
    else XyagSetColorMap(*ptcolormap);

    XyagLoadColors();
     
    XyagLoadTopLevelConfig();

    XyagMainWindow = XtVaCreateManagedWidget("XyagMainWindow",
                                       xmMainWindowWidgetClass,
                                       XyagTopLevel,
                                       XmNscrollingPolicy,
                                       XmAPPLICATION_DEFINED,
                                       NULL);

    XyagInitializeIcon();
    XyagInitializeMenu();
    XyagInitializeScroll();
    XyagInitializePanel();
    XyagInitializeGraphic();
    XyagInitializeEvent();
    XyagInitializeColors();
    XyagInitializeGraphicContext();

    XtSetArg(Args[0], XmNallowShellResize, True);
    XtSetArg(Args[1], XmNdeleteResponse, XmDO_NOTHING);
    XtSetValues(XyagTopLevel, Args, 2);

    XmMainWindowSetAreas(XyagMainWindow, XyagMenuBar, NULL, XyagHScroll, XyagVScroll, XyagGraphicWindow);
 
    XyagInitializeErrorMessage(0);
    return(XyagMainWindow) ;
}

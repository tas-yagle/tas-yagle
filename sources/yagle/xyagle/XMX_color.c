/*------------------------------------------------------------\
|                                                             |
| Tool    :                    XYAG                           |
|                                                             |
| File    :                  Colors.c                         |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                        Lester Anthony                       |
|                                                             |
\------------------------------------------------------------*/

#include <stdio.h>
#include <Xm/Xm.h>

#include MUT_H
#include MLO_H
#include XSB_H
#include XMX_H

#include "XMX_color.h"

GC              XyagBackgroundGC;
GC              XyagGridGC;
GC              XyagAcceptDrawGC;
GC              XyagConnectDrawGC;
GC              XyagLayerDrawGC[XYAG_MAX_LAYER];

GC              XyagSmallTextGC;
GC              XyagMediumTextGC;
GC              XyagLargeTextGC;
GC              XyagXorGCXor;
GC              XyagXorGCInvert;
GC              XyagXorGC;

XFontStruct    *XyagLargeTextFont;
XFontStruct    *XyagMediumTextFont;
XFontStruct    *XyagSmallTextFont;

Colormap        XyagColorMap;

/*------------------------------------------------------------\
|                      Layer Color Define                     |
\------------------------------------------------------------*/

static char    *XyagColorName[5];

static int      XyagColor[XYAG_MAX_COLOR];

/*------------------------------------------------------------\
|                      XyagSwitchColormap                     |
\------------------------------------------------------------*/

void 
XyagSetColorMap(map)
    Colormap map;
{
    XyagColorMap = map;
}

/*------------------------------------------------------------\
|                      XyagSwitchColormap                     |
\------------------------------------------------------------*/

void 
XyagInitializeColorMap()
{
    Display        *ADisplay;
    Screen         *AScreen;

    ADisplay = XtDisplay(XyagTopLevel);
    AScreen = XtScreen(XyagTopLevel);
    XyagColorMap = DefaultColormapOfScreen(AScreen);

    if (XYAG_SWITCH_COLOR_MAP) {
        XyagColorMap = XCopyColormapAndFree(ADisplay, XyagColorMap);
        XInstallColormap(ADisplay, XyagColorMap);
        XtVaSetValues(XyagTopLevel, XmNcolormap, XyagColorMap, NULL);
    }
}

/*------------------------------------------------------------\
|                      XyagInitializeColors                   |
\------------------------------------------------------------*/

void 
XyagInitializeColors()
{
    XColor          ForgetIt;
    XColor          GetIt;
    int             Counter;
    char            Error;
    char           *ColorName;

    Error = False;

    XyagColorName[0] = XYAG_BACKGROUND_COLOR_NAME;
    XyagColorName[1] = XYAG_FOREGROUND_COLOR_NAME;
    XyagColorName[2] = XYAG_CURSOR_COLOR_NAME;
    XyagColorName[3] = XYAG_ACCEPT_COLOR_NAME;
    XyagColorName[4] = XYAG_CONNECT_COLOR_NAME;

    for (Counter = 0; Counter < XYAG_MAX_COLOR; Counter++) {
        if (Counter < 5) ColorName = XyagColorName[Counter];
        else ColorName = XYAG_LAYER_NAME_TABLE[Counter - 5][1];

        if (ColorName != NULL) {
            if (!XAllocNamedColor(XyagGraphicDisplay, XyagColorMap, ColorName, &GetIt, &ForgetIt)) {
                fprintf(stderr, "\n[XYAGLE ERR] Color name '%s' is not in X11 database !\n", ColorName);
                Error = True;
            }
            XyagColor[Counter] = GetIt.pixel;
        }
        else XyagColor[Counter] = XyagColor[0];
    }

    //if (Error == True) XyagExit(1);
}

/*------------------------------------------------------------\
|                 XyagChangeCursorContext                    |
\------------------------------------------------------------*/

void 
XyagChangeCursorContext()
{
    XGCValues       GraphicValue;

    if (XYAG_XOR_CURSOR == XYAG_TRUE) {
        XyagXorGC = XyagXorGCXor;
    }
    else {
        XyagXorGC = XyagXorGCInvert;
    }
}

/*------------------------------------------------------------\
|                  XyagInitializeGraphicContext               |
\------------------------------------------------------------*/

void 
XyagInitializeGraphicContext()
{
    XGCValues       GraphicValue;
    int             Counter;
    Window          Root;

    XyagLargeTextFont = XLoadQueryFont(XyagGraphicDisplay, XYAG_LARGE_TEXT_FONT_NAME);

    XyagMediumTextFont = XLoadQueryFont(XyagGraphicDisplay, XYAG_MEDIUM_TEXT_FONT_NAME);

    XyagSmallTextFont = XLoadQueryFont(XyagGraphicDisplay, XYAG_SMALL_TEXT_FONT_NAME);

    GraphicValue.foreground = XyagColor[0];     /* Black */
    GraphicValue.background = XyagColor[0];
    GraphicValue.plane_mask = AllPlanes;

    Root = RootWindowOfScreen(XtScreen(XyagGraphicWindow));

    XyagBackgroundGC = XCreateGC(XyagGraphicDisplay, Root, (GCForeground|GCBackground|GCPlaneMask), &GraphicValue);

    GraphicValue.foreground = XyagColor[1];     /* White */

    XyagGridGC = XCreateGC(XyagGraphicDisplay, Root, (GCForeground|GCBackground|GCPlaneMask), &GraphicValue);

    GraphicValue.foreground = XyagColor[1];     /* Text  */

    XyagSmallTextGC = XCreateGC(XyagGraphicDisplay, Root, (GCForeground|GCBackground|GCPlaneMask), &GraphicValue);

    XyagMediumTextGC = XCreateGC(XyagGraphicDisplay, Root, (GCForeground|GCBackground|GCPlaneMask), &GraphicValue);

    XyagLargeTextGC = XCreateGC(XyagGraphicDisplay, Root, (GCForeground|GCBackground|GCPlaneMask), &GraphicValue);

    XSetFont(XyagGraphicDisplay, XyagSmallTextGC, XyagSmallTextFont->fid);

    XSetFont(XyagGraphicDisplay, XyagMediumTextGC, XyagMediumTextFont->fid);

    XSetFont(XyagGraphicDisplay, XyagLargeTextGC, XyagLargeTextFont->fid);

    GraphicValue.background = XyagColor[2];     /* Xor Color */
    GraphicValue.foreground = XyagColor[2];     /* Xor Color */
    GraphicValue.function = GXxor;

    XyagXorGCXor = XCreateGC(XyagGraphicDisplay, Root, (GCForeground|GCBackground|GCFunction|GCPlaneMask), &GraphicValue);

    GraphicValue.function = GXinvert;

    XyagXorGCInvert = XCreateGC(XyagGraphicDisplay, Root, (GCFunction|GCPlaneMask), &GraphicValue);

    XyagChangeCursorContext();

    GraphicValue.background = XyagColor[0];     /* Black        */
    GraphicValue.foreground = XyagColor[3];     /* Accept Color */

    XyagAcceptDrawGC = XCreateGC(XyagGraphicDisplay, Root, (GCForeground|GCBackground|GCPlaneMask), &GraphicValue);

    GraphicValue.foreground = XyagColor[4];     /* Connect Color */

    XyagConnectDrawGC = XCreateGC(XyagGraphicDisplay, Root, (GCForeground|GCBackground|GCPlaneMask), &GraphicValue);

    for (Counter = 0; Counter < XYAG_MAX_LAYER; Counter++) {
        GraphicValue.foreground = XyagColor[Counter + 5];
        XyagLayerDrawGC[Counter] = XCreateGC(XyagGraphicDisplay, Root, (GCForeground|GCBackground|GCPlaneMask), &GraphicValue);
    }
}

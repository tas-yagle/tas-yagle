/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                            |
|                                                             |
| File    :                  Pixmap.c                         |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                        Lester Anthony                       |
|                                                             |
\------------------------------------------------------------*/

#include <X11/Intrinsic.h>
#include <X11/xpm.h>
#include <Xm/Xm.h>
#include <Xm/Label.h>

#include MUT_H
#include MLO_H
#include XTB_H
#include XSB_H

#include "XTB_pixmap.h"

/*------------------------------------------------------------\
|                     XyagCreatePixmap                        |
\------------------------------------------------------------*/

Pixmap 
XyagCreatePixmap(MainWidget, IconBits, IconWidth, IconHeight)
    Widget          MainWidget;
    char           *IconBits;
    int             IconWidth;
    int             IconHeight;
{
    Pixel           Foreground;
    Pixel           Background;
    Pixmap          IconPixmap;

    XtVaGetValues(MainWidget,
                  XmNforeground, &Foreground,
                  XmNbackground, &Background,
                  NULL);

    IconPixmap = XCreatePixmapFromBitmapData(XtDisplay(MainWidget),
                                             RootWindowOfScreen(XtScreen(MainWidget)),
                                             IconBits, IconWidth, IconHeight,
                                             Foreground, Background,
                                             DefaultDepthOfScreen(XtScreen(MainWidget)));

    return (IconPixmap);
}

/*------------------------------------------------------------\
|                     XyagCreateColorPixmap                   |
\------------------------------------------------------------*/

Pixmap 
XyagCreateColorPixmap(MainWidget, IconData)
    Widget          MainWidget;
    char          **IconData;
{
    Display        *Dsp;
    Screen         *Scr;
    int             Depth;
    Pixmap          NormalPixmap;
    XpmAttributes   XpmAttr;
    Pixel           Background;
    XpmColorSymbol  XpmTransparentColor[1] = {{NULL, "none", 0}};

    Dsp = XtDisplayOfObject(MainWidget);
    Scr = XtScreenOfObject(MainWidget);
    XtVaGetValues(XtIsSubclass(MainWidget, coreWidgetClass) ? MainWidget : XtParent(MainWidget),
                  XmNdepth, &Depth,
                  XmNbackground, &Background,
                  NULL);

    XpmTransparentColor[0].pixel = Background;
    XpmAttr.valuemask = XpmColorSymbols | XpmCloseness | XpmDepth;
    XpmAttr.colorsymbols = XpmTransparentColor;
    XpmAttr.numsymbols = 1;
    XpmAttr.closeness = 65535;
    XpmAttr.depth = Depth;

    if ((XpmCreatePixmapFromData(Dsp, RootWindowOfScreen(Scr), IconData, &NormalPixmap, NULL, &XpmAttr)) != XpmSuccess) {
        return 0;
    }
    else return NormalPixmap;
}

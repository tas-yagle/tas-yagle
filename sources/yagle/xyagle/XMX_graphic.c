/*------------------------------------------------------------\
|                                                             |
| Tool    :                    XYAG                           |
|                                                             |
| File    :                 Graphic.c                         |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                        Lester Anthony                       |
|                                                             |
\------------------------------------------------------------*/

#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/DrawingA.h>

#include MUT_H
#include MLO_H
#include XSB_H
#include XTB_H
#include XMX_H

#include "XMX_graphic.h"
#include "XMX_grid.h"
#include "XMX_scroll.h"

Widget          XyagGraphicWindow;
Display        *XyagGraphicDisplay = NULL;

Dimension       XyagGraphicDx = 0;
Dimension       XyagGraphicDy = 0;
Dimension       XyagOldGraphicDx = 0;
Dimension       XyagOldGraphicDy = 0;

Pixmap          XyagGraphicPixmap = 0;

/*------------------------------------------------------------\
|                 XyagInitializeGraphicWindow                |
\------------------------------------------------------------*/

void 
XyagInitializeGraphicWindow()
{
    XtVaGetValues(XyagGraphicWindow,
                  XmNwidth, &XyagGraphicDx,
                  XmNheight, &XyagGraphicDy,
                  NULL);

    XyagOldGraphicDx = XyagGraphicDx;
    XyagOldGraphicDy = XyagGraphicDy;

    XyagGraphicPixmap =

        XCreatePixmap(XyagGraphicDisplay,
                      RootWindowOfScreen(XtScreen(XyagGraphicWindow)),
                      XyagGraphicDx,
                      XyagGraphicDy,
                      DefaultDepthOfScreen(XtScreen(XyagGraphicWindow)));

    XyagClearGraphicWindow(0, 0, XyagGraphicDx, XyagGraphicDy);
    XyagInitializeUnitGrid();

    XyagSetMouseCursor(XyagGraphicWindow, XYAG_NORMAL_CURSOR);
}

/*------------------------------------------------------------\
|                      XyagClearGraphicWindow                 |
\------------------------------------------------------------*/

void 
XyagClearGraphicWindow(GraphicX, GraphicY, GraphicDx, GraphicDy)
    long            GraphicX;
    long            GraphicY;
    long            GraphicDx;
    long            GraphicDy;
{
    XFillRectangle(XyagGraphicDisplay, XyagGraphicPixmap,
                   XyagBackgroundGC,
                   GraphicX, GraphicY, GraphicDx, GraphicDy);
}

/*------------------------------------------------------------\
|                      XyagRefreshGraphicWindow               |
\------------------------------------------------------------*/

void 
XyagRefreshGraphicWindow(GraphicX, GraphicY, GraphicDx, GraphicDy)
    Dimension       GraphicX;
    Dimension       GraphicY;
    Dimension       GraphicDx;
    Dimension       GraphicDy;
{
    XCopyArea(XyagGraphicDisplay, XyagGraphicPixmap,
              XtWindow(XyagGraphicWindow),
              XyagBackgroundGC,
              GraphicX, GraphicY, GraphicDx, GraphicDy, GraphicX, GraphicY);
}


/*------------------------------------------------------------\
|                      XyagResizeGraphicWindow                |
\------------------------------------------------------------*/

void 
XyagResizeGraphicWindow()
{
    Pixmap          OldPixmap;
    Dimension       SourceY;
    Dimension       TargetY;
    Dimension       TargetDx;
    Dimension       TargetDy;

    XyagOldGraphicDx = XyagGraphicDx;
    XyagOldGraphicDy = XyagGraphicDy;

    XtVaGetValues(XyagGraphicWindow,
                  XmNwidth, &XyagGraphicDx,
                  XmNheight, &XyagGraphicDy,
                  NULL);

    if ((XyagOldGraphicDx != XyagGraphicDx) ||
        (XyagOldGraphicDy != XyagGraphicDy)) {
        OldPixmap = XyagGraphicPixmap;

        XyagGraphicPixmap = XCreatePixmap(XyagGraphicDisplay,
                                          RootWindowOfScreen(XtScreen(XyagGraphicWindow)),
                                          XyagGraphicDx, XyagGraphicDy,
                                          DefaultDepthOfScreen(XtScreen(XyagGraphicWindow)));

        XyagClearGraphicWindow(0, 0, XyagGraphicDx, XyagGraphicDy);

        XyagResizeUnitGrid();
        XyagComputeScrollBar();

        if (XyagOldGraphicDx < XyagGraphicDx) {
            TargetDx = XyagOldGraphicDx;
            XyagDisplayFigure(TargetDx, 0, XyagGraphicDx, XyagGraphicDy);
        }
        else {
            TargetDx = XyagGraphicDx;
        }

        if (XyagOldGraphicDy < XyagGraphicDy) {
            SourceY = 0;
            TargetDy = XyagOldGraphicDy;
            TargetY = XyagGraphicDy - XyagOldGraphicDy;
            XyagDisplayFigure(0, TargetDy, TargetDx, XyagGraphicDy);
        }
        else {
            TargetDy = XyagGraphicDy;
            TargetY = 0;
            SourceY = XyagOldGraphicDy - XyagGraphicDy;
        }

        XCopyArea(XyagGraphicDisplay, OldPixmap,
                  XyagGraphicPixmap,
                  XyagBackgroundGC,
                  0, SourceY, TargetDx, TargetDy, 0, TargetY);

        XFreePixmap(XyagGraphicDisplay, OldPixmap);
    }
}

/*------------------------------------------------------------\
|                     XyagInitializeGraphic                   |
\------------------------------------------------------------*/

void 
XyagInitializeGraphic()
{
    XyagGraphicWindow = XtVaCreateManagedWidget("XyagGraphicWindow",
                                                 xmDrawingAreaWidgetClass,
                                                 XyagMainWindow,
                                                 XmNbackground, 1,
                                                 XmNforeground, 0,
                                                 NULL);

    XyagGraphicDisplay = XtDisplay(XyagGraphicWindow);
}

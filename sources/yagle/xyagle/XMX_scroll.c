#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/ScrollBar.h>

#include MUT_H
#include MLO_H
#include CGV_H
#include XSB_H
#include XTB_H
#include XMX_H
#include XMS_H
#include XYA_H

#include "XMX_motif.h"
#include "XMX_graphic.h"
#include "XMX_event.h"
#include "XMX_menu.h"
#include "XMX_color.h"
#include "XMX_panel.h"
#include "XMX_icon.h"
#include "XMX_scroll.h"

Widget        XyagVScroll;
Widget        XyagHScroll;

int           XyagScrollStep;

void
XyagInitializeScroll()
{
    XyagVScroll = XtVaCreateManagedWidget("XyagVScroll",
                                        xmScrollBarWidgetClass,
                                        XyagMainWindow,
                                        XmNorientation, XmVERTICAL,
                                        XmNprocessingDirection, XmMAX_ON_TOP,
                                        XmNminimum, 0,
                                        XmNmaximum, 1,
                                        XmNsliderSize, 1,
                                        XmNvalue, 0,
                                        NULL);

    XtAddCallback(XyagVScroll, XmNvalueChangedCallback, CallbackVScroll, NULL);
    XtAddCallback(XyagVScroll, XmNdragCallback, CallbackVScroll, NULL);

    XyagHScroll = XtVaCreateManagedWidget("XyagHScroll",
                                        xmScrollBarWidgetClass,
                                        XyagMainWindow,
                                        XmNorientation, XmHORIZONTAL,
                                        XmNminimum, 0,
                                        XmNmaximum, 1,
                                        XmNsliderSize, 1,
                                        XmNvalue, 0,
                                        NULL);

    XtAddCallback(XyagHScroll, XmNvalueChangedCallback, CallbackHScroll, NULL);
    XtAddCallback(XyagHScroll, XmNdragCallback, CallbackHScroll, NULL);

}

void 
CallbackVScroll(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    XtPointer       ClientData;
    XtPointer      *CallData;
{
    long            offset;
    long            XyagOldPixelGridY;
    int             value;

    XtVaGetValues(MyWidget, XmNvalue, &value, NULL);
    XyagUnitGridY = value * XyagScrollStep;
    XyagOldPixelGridY = XyagPixelGridY;
    XyagPixelGridY = (float)XyagUnitGridY * XyagUnitGridStep;

    if (XyagPixelGridY > XyagOldPixelGridY) {
        offset = XyagPixelGridY - XyagOldPixelGridY;
        XCopyArea( XyagGraphicDisplay,
            XyagGraphicPixmap,
            XyagGraphicPixmap,
            XyagBackgroundGC,
            0, 0,
            XyagGraphicDx,
            XyagGraphicDy - offset,
            0, offset
        ); 

        XyagDisplayFigure( 0, XyagGraphicDy - offset, XyagGraphicDx, XyagGraphicDy ); 
        XyagRefreshGraphicWindow( 0, 0, XyagGraphicDx, XyagGraphicDy);
    }
    else {
        offset = XyagOldPixelGridY - XyagPixelGridY;

        XCopyArea( XyagGraphicDisplay,
            XyagGraphicPixmap,
            XyagGraphicPixmap,
            XyagBackgroundGC,
            0, offset,
            XyagGraphicDx,
            XyagGraphicDy - offset,
            0, 0
        ); 

        XyagDisplayFigure( 0, 0, XyagGraphicDx, offset ); 
        XyagRefreshGraphicWindow( 0, 0, XyagGraphicDx, XyagGraphicDy);
    }
}

void 
CallbackHScroll(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    XtPointer       ClientData;
    XtPointer      *CallData;
{
    long            offset;
    long            XyagOldPixelGridX;
    int             value;

    XtVaGetValues(MyWidget, XmNvalue, &value, NULL);
    XyagUnitGridX = value * XyagScrollStep;
    XyagOldPixelGridX = XyagPixelGridX;
    XyagPixelGridX = (float)XyagUnitGridX * XyagUnitGridStep;

    if (XyagPixelGridX > XyagOldPixelGridX) {
        offset = XyagPixelGridX - XyagOldPixelGridX;
        XCopyArea( XyagGraphicDisplay,
            XyagGraphicPixmap,
            XyagGraphicPixmap,
            XyagBackgroundGC,
            offset, 0,
            XyagGraphicDx - offset,
            XyagGraphicDy,
            0, 0
        ); 

        XyagDisplayFigure(XyagGraphicDx - offset, 0, XyagGraphicDx, XyagGraphicDy); 
        XyagRefreshGraphicWindow(0, 0, XyagGraphicDx, XyagGraphicDy);
    }
    else {
        offset = XyagOldPixelGridX - XyagPixelGridX;

        XCopyArea( XyagGraphicDisplay,
            XyagGraphicPixmap,
            XyagGraphicPixmap,
            XyagBackgroundGC,
            0, 0,
            XyagGraphicDx - offset,
            XyagGraphicDy,
            offset, 0
        ); 

        XyagDisplayFigure(0, 0, offset, XyagGraphicDy); 
        XyagRefreshGraphicWindow(0, 0, XyagGraphicDx, XyagGraphicDy);
    }
}

void
XyagComputeScrollBar()
{
    int             minimum;
    int             maximum;
    int             pagesize;
    int             value;

    XyagScrollStep = 8;

    minimum = (XyagBoundXmin/XYAG_UNIT)-32;
    minimum = (XyagUnitGridX < minimum) ? XyagUnitGridX : minimum;
    minimum = minimum/XyagScrollStep;

    maximum = (XyagBoundXmax/XYAG_UNIT)+32;
    maximum = (XyagUnitGridX+XyagUnitGridDx > maximum) ? XyagUnitGridX+XyagUnitGridDx : maximum;
    maximum = maximum/XyagScrollStep + 1;

    pagesize = XyagUnitGridDx/XyagScrollStep;
    value = XyagUnitGridX/XyagScrollStep;
    
    if (pagesize<1) pagesize=1;

    XtVaSetValues(XyagHScroll,
                  XmNminimum, minimum,
                  XmNmaximum, maximum,
                  XmNsliderSize, pagesize,
                  XmNvalue, value,
                  XmNpageIncrement, pagesize,
                  NULL);

    minimum = (XyagBoundYmin/XYAG_UNIT)-32;
    minimum = (XyagUnitGridY < minimum) ? XyagUnitGridY : minimum;
    minimum = minimum/XyagScrollStep;

    maximum = (XyagBoundYmax/XYAG_UNIT)+32;
    maximum = (XyagUnitGridY+XyagUnitGridDy > maximum) ? XyagUnitGridY+XyagUnitGridDy : maximum;
    maximum = maximum/XyagScrollStep + 1;

    pagesize = XyagUnitGridDy/XyagScrollStep;
    value = XyagUnitGridY/XyagScrollStep;

    if (pagesize<1) pagesize=1;


    XtVaSetValues(XyagVScroll,
                  XmNminimum, minimum,
                  XmNmaximum, maximum,
                  XmNsliderSize, pagesize,
                  XmNvalue, value,
                  XmNpageIncrement, pagesize,
                  NULL);
}


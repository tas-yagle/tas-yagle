/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                            |
|                                                             |
| File    :                   View.c                          |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                        Lester Anthony                       |
|                                                             |
\------------------------------------------------------------*/

#include <stdio.h>
#include <math.h>
#include <signal.h>
#include <limits.h>
#include <Xm/Xm.h>

#include MUT_H
#include SLB_H
#include MLO_H
#include CGV_H
#include XSB_H
#include XYA_H
#include XMX_H

#include "XMX_view.h"

static long     XyagGraphicX1;
static long     XyagGraphicX2;
static long     XyagGraphicY1;
static long     XyagGraphicY2;
static XFontStruct *XyagTextFont;
static GC       XyagTextGC;
static GC       XyagDrawGC;
static char     XyagInterrupt = 0;

static xyagview_list *XyagHeadView = NULL;

#define STOP_IN_LONG_SPACE(x) if (x<0) x=0; else if (x>16000) x=16000

/*------------------------------------------------------------\
|                         XyagAllocView                       |
\------------------------------------------------------------*/

xyagview_list *
XyagAllocView()
{
    xyagview_list  *view;

    view = (xyagview_list *)mbkalloc(sizeof(xyagview_list));
    view->NEXT = NULL;
    view->OBJECT = NULL;
    return view;
}

/*------------------------------------------------------------\
|                          XyagFreeView                       |
\------------------------------------------------------------*/

void 
XyagFreeView(FreeView)
    xyagview_list  *FreeView;
{
    mbkfree(FreeView);
}

/*------------------------------------------------------------\
|                        XyagAddViewLater                     |
\------------------------------------------------------------*/

void 
XyagAddViewLater(Obj)
    xyagobj_list   *Obj;
{
    xyagview_list  *View;

    View = XyagAllocView();

    View->OBJECT = Obj;
    View->NEXT = XyagHeadView;
    XyagHeadView = View;
}

/*------------------------------------------------------------\
|                        XyagDelView                          |
\------------------------------------------------------------*/

void 
XyagDelView()
{
    xyagview_list  *DelView;
    xyagview_list  *View;

    View = XyagHeadView;
    XyagHeadView = (xyagview_list *) NULL;

    while (View != (xyagview_list *) NULL) {
        DelView = View;
        View = View->NEXT;
        XyagFreeView(DelView);
    }
}

/*------------------------------------------------------------\
|                     XyagInterruptDisplay                    |
\------------------------------------------------------------*/

void 
XyagInterruptDisplay()
{
    XEvent          Event;
    KeySym          Key;
    char            Text;

    signal(SIGALRM, XyagInterruptDisplay);
    alarm(1);

    if (XCheckTypedEvent(XyagGraphicDisplay, KeyPress, &Event)) {
        XLookupString(&Event.xkey, &Text, 1, &Key, 0);

        if (Text == '\003') {
            XyagInterrupt = 1;
            XBell(XyagGraphicDisplay, 0);
        }
    }
}

/*------------------------------------------------------------\
|                     XyagFlushEventDisplay                   |
\------------------------------------------------------------*/

void 
XyagFlushEventDisplay()
{
    XEvent          Event;

    while (XCheckTypedEvent(XyagGraphicDisplay, KeyPress, &Event));
}

/*------------------------------------------------------------\
|                       XyagComputeHexagon                    |
\------------------------------------------------------------*/

char XyagComputeHexagon(long long X1, long long Y1, long long X2, long long Y2, XPoint *Points)
{
    long long            StepX;
    long long            StepY;

    StepY = (Y2 - Y1) >> 1;
    StepX = StepY >> 1;

    if ((StepY > 0) && (StepX > 0)) {
        Points[0].x = X1 + StepX;
        Points[0].y = Y1;
        Points[1].x = X1;
        Points[1].y = Y1 + StepY;
        Points[2].x = X1 + StepX;
        Points[2].y = Y2;
        Points[3].x = X2 - StepX;
        Points[3].y = Y2;
        Points[4].x = X2;
        Points[4].y = Y1 + StepY;
        Points[5].x = X2 - StepX;
        Points[5].y = Y1;
        Points[6].x = X1 + StepX;
        Points[6].y = Y1;

        return (XYAG_TRUE);
    }

    return (XYAG_FALSE);
}

/*------------------------------------------------------------\
|                       XyagDisplayArrow                      |
\------------------------------------------------------------*/

char XyagComputeArrow(long long X1, long long Y1, long long X2, long long Y2, XPoint *Points)
{
    float           LineX;
    float           LineY;
    float           HalfX;
    float           HalfY;
    float           CrossX;
    float           CrossY;
    float           Norm;

    LineX = (float) (X2 - X1);
    LineY = (float) (Y2 - Y1);

    if ((LineX != 0.0) || (LineY != 0.0)) {
        Norm = sqrt((LineX * LineX) + (LineY * LineY));
        Norm = Norm / XyagUnitGridStep;
        LineX = LineX / Norm;
        LineY = LineY / Norm;
        HalfX = LineX / 2;
        HalfY = LineY / 2;
        CrossX = -HalfY;
        CrossY = HalfX;

        Points[0].x = X1 + LineX;
        Points[0].y = Y1 + LineY;
        Points[1].x = Points[0].x + LineX + HalfX + CrossX;
        Points[1].y = Points[0].y + LineY + HalfY + CrossY;
        Points[2].x = Points[0].x + LineX + HalfX - CrossX;
        Points[2].y = Points[0].y + LineY + HalfY - CrossY;

        return (XYAG_TRUE);
    }

    return (XYAG_FALSE);
}

/*------------------------------------------------------------\
|                  XyagDrawUserDefinedObject                  |
\------------------------------------------------------------*/
void XyagDrawUserDefinedObject(Obj)
     xyagobj_list   *Obj;
{
    motif_draw  *ptm_draw;

    for(ptm_draw=Obj->SYMBOL->MOTIF->DRAW;ptm_draw;ptm_draw=ptm_draw->NEXT)
      {
      
      long long X1r , X2r , Y1r , Y2r ;
      
      long long Xorigin = Obj->X - Obj->SYMBOL->BOX->X_MIN ;
      long long Yorigin = Obj->Y - Obj->SYMBOL->BOX->Y_MIN ;
              
      if(ptm_draw->TYPE == MOTIF_T_LINE)
       {
        motif_line *ptm_data;
        ptm_data=ptm_draw->DATA.MLINE;
        
        X1r = ( ( Xorigin + ptm_data->START_X ) * XyagUnitGridStep / XYAG_UNIT ) - XyagPixelGridX ;
        Y1r = ( ( Yorigin - ptm_data->START_Y ) * XyagUnitGridStep / XYAG_UNIT ) - XyagPixelGridY ;
        X2r = ( ( Xorigin + ptm_data->END_X )   * XyagUnitGridStep / XYAG_UNIT ) - XyagPixelGridX ;
        Y2r = ( ( Yorigin - ptm_data->END_Y )   * XyagUnitGridStep / XYAG_UNIT ) - XyagPixelGridY ;
        
        XDrawLine(XyagGraphicDisplay,
                  XyagGraphicPixmap,
                  XyagDrawGC,
                  X1r , XyagGraphicDy - Y1r ,
                  X2r , XyagGraphicDy - Y2r );
       }  
      if(ptm_draw->TYPE == MOTIF_T_ARC)
       {
        motif_arc *ptm_data;
        ptm_data=ptm_draw->DATA.MARC;
        
        X1r = ( ( Xorigin + ptm_data->CORNER_X ) * XyagUnitGridStep / XYAG_UNIT ) - XyagPixelGridX ;
        Y1r = ( ( Yorigin - ptm_data->CORNER_Y ) * XyagUnitGridStep / XYAG_UNIT ) - XyagPixelGridY ;
        X2r = ptm_data->R * 2 * XyagUnitGridStep / XYAG_UNIT ;
        
        XDrawArc(XyagGraphicDisplay,
                 XyagGraphicPixmap,
                 XyagDrawGC,
                 X1r , XyagGraphicDy - Y1r ,
                 X2r,  X2r,
                 ptm_data->ALPHA ,
                 ptm_data->DELTA );
        }  
    }      
}

/*------------------------------------------------------------\
|                     XyagDisplayOneObject                    |
\------------------------------------------------------------*/

void 
XyagDisplayOneObject(Obj)
    xyagobj_list   *Obj;
{
    XPoint          Points[7];
    int             Index;
    long long            X1r;
    long long            X2r;
    long long            Y1r;
    long long            Y2r;
    long long            DeltaX;
    long long            DeltaY, tmpy0, tmpy1;

    if (IsXyagLineLeft(Obj)) {
        X1r = Obj->X + Obj->DX;
        X2r = Obj->X;
    }
    else {
        X1r = Obj->X;
        X2r = Obj->X + Obj->DX;
    }

    if (IsXyagLineDown(Obj)) {
        Y1r = Obj->Y + Obj->DY;
        Y2r = Obj->Y;
    }
    else {
        Y1r = Obj->Y;
        Y2r = Obj->Y + Obj->DY;
    }

    X1r = (float) (X1r) * XyagUnitGridStep;
    Y1r = (float) (Y1r) * XyagUnitGridStep;
    X2r = (float) (X2r) * XyagUnitGridStep;
    Y2r = (float) (Y2r) * XyagUnitGridStep;

    X1r = (X1r / XYAG_UNIT) - XyagPixelGridX;
    X2r = (X2r / XYAG_UNIT) - XyagPixelGridX;
    Y1r = (Y1r / XYAG_UNIT) - XyagPixelGridY;
    Y2r = (Y2r / XYAG_UNIT) - XyagPixelGridY;

    DeltaX = X2r - X1r;
    DeltaY = Y2r - Y1r;

    if (DeltaX <= 0)
        DeltaX = 1;
    if (DeltaY <= 0)
        DeltaY = 1;

    switch (GetXyagObjectType(Obj)) {
    case XYAG_OBJECT_LINE:
      tmpy0=XyagGraphicDy - Y1r;
      tmpy1=XyagGraphicDy - Y2r;
/*      printf("<%lld %lld> <%lld %lld> %ld\n",X1r,Y1r, X2r, Y2r, XyagGraphicDy);
      printf("\t<%d %d> <%lld %lld>\n",(int)tmpy0,(int)tmpy1, tmpy0, tmpy1);
*/
      STOP_IN_LONG_SPACE(X1r);
      STOP_IN_LONG_SPACE(X2r);
      STOP_IN_LONG_SPACE(tmpy0);
      STOP_IN_LONG_SPACE(tmpy1);
      
        XDrawLine(XyagGraphicDisplay,
                  XyagGraphicPixmap,
                  XyagDrawGC,
                  X1r, tmpy0,
                  X2r, tmpy1);
        break;

    case XYAG_OBJECT_ARROW:

        XDrawLine(XyagGraphicDisplay,
                  XyagGraphicPixmap,
                  XyagDrawGC,
                  X1r, XyagGraphicDy - Y1r,
                  X2r, XyagGraphicDy - Y2r);

        if (XyagComputeArrow(X1r, Y1r, X2r, Y2r, Points)) {
            Points[0].y = XyagGraphicDy - Points[0].y;
            Points[1].y = XyagGraphicDy - Points[1].y;
            Points[2].y = XyagGraphicDy - Points[2].y;

            XFillPolygon(XyagGraphicDisplay,
                         XyagGraphicPixmap,
                         XyagDrawGC, Points, 3,
                         Convex,
                         CoordModeOrigin);
        }

        break;

    case XYAG_OBJECT_HEXAGON:

        if (XyagComputeHexagon(X1r, Y1r, X2r, Y2r, Points)) {
            for (Index = 0; Index < 7; Index++) {
                Points[Index].y = XyagGraphicDy - Points[Index].y;
            }

            XDrawLines(XyagGraphicDisplay,
                       XyagGraphicPixmap,
                       XyagDrawGC, Points, 7,
                       CoordModeOrigin);
        }

        break;

    case XYAG_OBJECT_CIRCLE:

        XDrawArc(XyagGraphicDisplay,
                 XyagGraphicPixmap,
                 XyagDrawGC,
                 X1r, XyagGraphicDy - Y2r,
                 DeltaX, DeltaY, 0, 23040);
        break;

    case XYAG_OBJECT_RECTANGLE:

        XDrawRectangle(XyagGraphicDisplay,
                       XyagGraphicPixmap,
                       XyagDrawGC,
                       X1r, XyagGraphicDy - Y2r,
                       DeltaX, DeltaY);

        break;

    case XYAG_OBJECT_TRIANGLE:

        XDrawLine(XyagGraphicDisplay,
                  XyagGraphicPixmap,
                  XyagDrawGC,
                  X1r, XyagGraphicDy - Y1r,
                  X2r, (XyagGraphicDy - Y1r - (DeltaY >> 1)));

        XDrawLine(XyagGraphicDisplay,
                  XyagGraphicPixmap,
                  XyagDrawGC,
                  X1r, XyagGraphicDy - Y1r,
                  X1r, XyagGraphicDy - Y2r);

        XDrawLine(XyagGraphicDisplay,
                  XyagGraphicPixmap,
                  XyagDrawGC,
                  X1r, XyagGraphicDy - Y2r,
                  X2r, (XyagGraphicDy - Y1r - (DeltaY >> 1)));

        break;

    case XYAG_OBJECT_SLIB:

        XyagDrawUserDefinedObject(Obj);

        break;
    
    case XYAG_OBJECT_CELL:

        XDrawRectangle(XyagGraphicDisplay,
                       XyagGraphicPixmap,
                       XyagDrawGC,
                       X1r, XyagGraphicDy - Y2r,
                       DeltaX, DeltaY);

        break;
    }
}

/*------------------------------------------------------------\
|                     XyagDisplayOneString                    |
\------------------------------------------------------------*/

void 
XyagDisplayOneString(Obj)
    xyagobj_list   *Obj;
{
    long long            X1r;
    long long            X2r;
    long long            Y1r;
    long long            Y2r;
    long long            DeltaX;
    long long            DeltaY;
    long long            WidthText;
    long long            HeightText;
    int             Length;

    if (IsXyagLineLeft(Obj)) {
        X1r = Obj->X + Obj->DX;
        X2r = Obj->X;
    }
    else {
        X1r = Obj->X;
        X2r = Obj->X + Obj->DX;
    }

    if (IsXyagLineDown(Obj)) {
        Y1r = Obj->Y + Obj->DY;
        Y2r = Obj->Y;
    }
    else {
        Y1r = Obj->Y;
        Y2r = Obj->Y + Obj->DY;
    }

    X1r = (float) (X1r) * XyagUnitGridStep;
    Y1r = (float) (Y1r) * XyagUnitGridStep;
    X2r = (float) (X2r) * XyagUnitGridStep;
    Y2r = (float) (Y2r) * XyagUnitGridStep;

    X1r = (X1r / XYAG_UNIT) - XyagPixelGridX;
    X2r = (X2r / XYAG_UNIT) - XyagPixelGridX;
    Y1r = (Y1r / XYAG_UNIT) - XyagPixelGridY;
    Y2r = (Y2r / XYAG_UNIT) - XyagPixelGridY;

    DeltaX = X2r - X1r;
    DeltaY = Y2r - Y1r;

    if (DeltaX <= 0)
        DeltaX = 1;
    if (DeltaY <= 0)
        DeltaY = 1;

    Length = strlen(Obj->NAME);
    WidthText = XTextWidth(XyagTextFont, Obj->NAME, Length);
    HeightText = XyagTextFont->ascent;

    if (Obj->TYPE == XYAG_OBJECT_TEXT_RIGHT) {
        XDrawString(XyagGraphicDisplay,
                    XyagGraphicPixmap,
                    XyagTextGC,
                    X1r,
                    XyagGraphicDy - ((Y1r + Y2r - HeightText) >> 1),
                    Obj->NAME, Length);
    }
    else if (Obj->TYPE == XYAG_OBJECT_TEXT_LEFT) {
        XDrawString(XyagGraphicDisplay,
                    XyagGraphicPixmap,
                    XyagTextGC,
                    X2r - WidthText,
                    XyagGraphicDy - ((Y1r + Y2r - HeightText) >> 1),
                    Obj->NAME, Length);
    }
    else {
        XDrawString(XyagGraphicDisplay,
                    XyagGraphicPixmap,
                    XyagTextGC,
                    (X2r + X1r - WidthText) >> 1,
                    XyagGraphicDy - ((Y1r + Y2r - HeightText) >> 1),
                    Obj->NAME, Length);
    }
}

/*------------------------------------------------------------\
|                     XyagDisplayFigure                       |
\------------------------------------------------------------*/

void 
XyagDisplayFigure(GraphicX1, GraphicY1, GraphicX2, GraphicY2)
    long            GraphicX1;
    long            GraphicY1;
    long            GraphicX2;
    long            GraphicY2;
{
    xyagview_list  *View;
    xyagobj_list   *Obj;
    long long            X1;
    long long            Y1;
    long long            X2;
    long long            Y2;
    int             Layer;
    char            DisplayText;
    extern int      CGV_SCALE;
    
    DisplayText = 1;

    XyagGraphicX1 = GraphicX1;
    XyagGraphicX2 = GraphicX2;
    XyagGraphicY1 = GraphicY1;
    XyagGraphicY2 = GraphicY2;

    if (XyagUnitGridStep*CGV_SCALE > 10.0) {
        XyagTextGC = XyagLargeTextGC;
        XyagTextFont = XyagLargeTextFont;
    }
    else if (XyagUnitGridStep*CGV_SCALE > 6.0) {
        XyagTextGC = XyagMediumTextGC;
        XyagTextFont = XyagMediumTextFont;
    }
    else if (XyagUnitGridStep*CGV_SCALE > 3.0) {
        XyagTextGC = XyagSmallTextGC;
        XyagTextFont = XyagSmallTextFont;
    }
    else {
        DisplayText = XYAG_FORCE_DISPLAY;
    }

    XyagClearGraphicWindow(GraphicX1,
                           XyagGraphicDy - GraphicY2,
                           GraphicX2 - GraphicX1,
                           GraphicY2 - GraphicY1);

    if (XyagFigure == (xyagfig_list *) NULL)
        return;

    X1 = GraphicX1 + XyagPixelGridX;
    X2 = GraphicX2 + XyagPixelGridX;
    Y1 = GraphicY1 + XyagPixelGridY;
    Y2 = GraphicY2 + XyagPixelGridY;

//    printf("gridstep=%g\n",XyagUnitGridStep);
    X1 = (X1 / XyagUnitGridStep);
    Y1 = (Y1 / XyagUnitGridStep);
    X2 = (X2 / XyagUnitGridStep);
    Y2 = (Y2 / XyagUnitGridStep);

    if (X2 >= 0) {
        X2 = X2 + 1;
    }
    if (Y2 >= 0) {
        Y2 = Y2 + 1;
    }
    if (X1 <= 0) {
        X1 = X1 - 1;
    }
    if (Y1 <= 0) {
        Y1 = Y1 - 1;
    }

    X1 = X1 * XYAG_UNIT;
    X2 = X2 * XYAG_UNIT;
    Y1 = Y1 * XYAG_UNIT;
    Y2 = Y2 * XYAG_UNIT;

    XyagInterrupt = 0;

    signal(SIGALRM, XyagInterruptDisplay);
    alarm(1);

    for (Layer = 0; Layer < XYAG_MAX_LAYER; Layer++) {
        if (XyagInterrupt)
            break;

        if (XYAG_ACTIVE_LAYER_TABLE[Layer] == 0)
            continue;

        XyagDrawGC = XyagLayerDrawGC[Layer];

        for (Obj = XyagFigure->OBJECT[Layer];
             Obj != (xyagobj_list *) NULL;
             Obj = Obj->NEXT) {
            if (XyagInterrupt)
                break;

            if (XYAG_ACTIVE_LAYER_TABLE[Obj->LAYER] == 0)
                continue;

            if ((Obj->X <= X2) &&
                (Obj->Y <= Y2) &&
                (Obj->X + Obj->DX >= X1) &&
                (Obj->Y + Obj->DY >= Y1)) {
                if ((IsXyagAccept(Obj)) ||
                    (IsXyagConnect(Obj))) {
                    XyagAddViewLater(Obj);
                }
                else {
                    XyagDisplayOneObject(Obj);

                    if (Obj->NAME != (char *) 0) {
                        if (DisplayText) {
                            if (((IsXyagCgvCon(Obj) &&
                            XYAG_ACTIVE_NAME_TABLE[XYAG_CONNECTOR_NAME])) ||
                                ((IsXyagCgvBox(Obj) &&
                             XYAG_ACTIVE_NAME_TABLE[XYAG_INSTANCE_NAME]))) {
                                XyagDisplayOneString(Obj);
                            }
                        }
                    }
                }
            }
        }
    }

    for (View = XyagHeadView;
         View != (xyagview_list *) NULL;
         View = View->NEXT) {
        Obj = View->OBJECT;

        if (IsXyagAccept(Obj)) {
            XyagDrawGC = XyagAcceptDrawGC;
        }
        else {
            XyagDrawGC = XyagConnectDrawGC;
        }

        XyagDisplayOneObject(Obj);

        if (Obj->NAME != (char *) 0) {
            if (DisplayText) {
                XyagDisplayOneString(Obj);
            }
        }
    }

    XyagDelView();

    alarm(0);
    XyagFlushEventDisplay();
}

/*------------------------------------------------------------\
|                     XyagDisplayObject                       |
\------------------------------------------------------------*/

void 
XyagDisplayObject(Obj)
    xyagobj_list   *Obj;
{
    xyagobj_list   *ScanObj;
    long long            Xmin;
    long long            Ymin;
    long long            Xmax;
    long long            Ymax;

    Xmin = Obj->X;
    Ymin = Obj->Y;
    Xmax = Xmin + Obj->DX;
    Ymax = Ymin + Obj->DY;

    ScanObj = Obj->LINK;

    while (ScanObj != Obj) {
        if (Xmin > ScanObj->X) {
            Xmin = ScanObj->X;
        }

        if (Xmax < (ScanObj->X + ScanObj->DX)) {
            Xmax = ScanObj->X + ScanObj->DX;
        }

        if (Ymin > ScanObj->Y) {
            Ymin = ScanObj->Y;
        }

        if (Ymax < (ScanObj->Y + ScanObj->DY)) {
            Ymax = ScanObj->Y + ScanObj->DY;
        }

        ScanObj = ScanObj->LINK;
    }

    Xmin = (float) (Xmin - XYAG_UNIT) * XyagUnitGridStep;
    Ymin = (float) (Ymin - XYAG_UNIT) * XyagUnitGridStep;
    Xmax = (float) (Xmax + XYAG_UNIT) * XyagUnitGridStep;
    Ymax = (float) (Ymax + XYAG_UNIT) * XyagUnitGridStep;

    Xmin = (Xmin / XYAG_UNIT) - XyagPixelGridX - 1;
    Xmax = (Xmax / XYAG_UNIT) - XyagPixelGridX + 1;
    Ymin = (Ymin / XYAG_UNIT) - XyagPixelGridY - 1;
    Ymax = (Ymax / XYAG_UNIT) - XyagPixelGridY + 1;

    if (Xmin < 0)
        Xmin = 0;
    if (Ymin < 0)
        Ymin = 0;

    if (Xmax > XyagGraphicDx)
        Xmax = XyagGraphicDx;
    if (Ymax > XyagGraphicDy)
        Ymax = XyagGraphicDy;

    if ((Xmax > 0) &&
        (Ymax > 0) &&
        (Xmin < XyagGraphicDx) &&
        (Ymin < XyagGraphicDy)) {
        XyagDisplayFigure(Xmin, Ymin, Xmax, Ymax);

        XyagRefreshGraphicWindow(Xmin, XyagGraphicDy - Ymax,
                                 Xmax - Xmin, Ymax - Ymin);
    }
}

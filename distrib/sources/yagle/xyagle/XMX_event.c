/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                            |
|                                                             |
| File    :                   Event.c                         |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                        Lester Anthony                       |
|                        Picault Stephane                     |
|                                                             |
\------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>

#include MUT_H
#include MLO_H
#include CGV_H
#include XSB_H
#include XYA_H
#include XMX_H
#include XTB_H
#include XME_H
#include XMV_H
#include XMS_H
#include XMH_H

#include "XMX_motif.h"
#include "XMX_grid.h"
#include "XMX_graphic.h"
#include "XMX_cursor.h"
#include "XMX_event.h"
#include "XMV_view.h"
#include "XMF_file.h"
#include "XME_menu.h"

extern void (* XyagCallbackFileQuitFonction)();

static String   XyagEventTranslation =
    "<Btn1Down>:    CallbackEvent(  0 )\n\
     <Btn3Down>:    CallbackEvent(  1 )\n\
     <Motion>:      CallbackEvent(  2 )\n\
     <EnterWindow>: CallbackEvent(  3 )\n\
     <LeaveWindow>: CallbackEvent(  4 )\n\
     <Key>osfUp:    CallbackEvent(  5 )\n\
     <Key>osfDown:  CallbackEvent(  6 )\n\
     <Key>osfLeft:  CallbackEvent(  7 )\n\
     <Key>osfBackSpace: CallbackEvent(  9 )\n\
     <Key>osfRight: CallbackEvent(  8 )\n";

/*------------------------------------------------------------\
|                         Event Count                         |
\------------------------------------------------------------*/

static int      XyagMaxEventEditTable[] =
{
    2,                          /* XYAG_EDIT_MEASURE   */
    1,                          /* XYAG_EDIT_IDENTIFY  */
    1,                          /* XYAG_EDIT_SELECT    */
    1,                          /* XYAG_EDIT_BEHAVIOUR */
    1,                          /* XYAG_VIEW           */
    1,                          /* XYAG_VIEW           */
    1,                          /* XYAG_VIEW           */
    1,                          /* XYAG_VIEW           */
    1,                          /* XYAG_VIEW           */
    1,                          /* XYAG_VIEW           */
    1 // go thru hierarchy
};

static int      XyagInputEventEditTable[] =
{
    XYAG_INPUT_HALF_BOX,        /* XYAG_EDIT_MEASURE   */
    XYAG_INPUT_POINT,           /* XYAG_EDIT_IDENTIFY  */
    XYAG_INPUT_POINT,           /* XYAG_EDIT_SELECT    */
    XYAG_INPUT_POINT,           /* XYAG_EDIT_BEHAVIOUR */
    XYAG_INPUT_POINT,
    XYAG_INPUT_POINT,
    XYAG_INPUT_POINT,
    XYAG_INPUT_POINT,
    XYAG_INPUT_POINT,
    XYAG_INPUT_POINT
};

static int      XyagMaxEventZoomTable[] =
{
    1,                          /* XYAG_ZOOM_CENTER            */
    2,                          /* XYAG_ZOOM_IN                */
    2                           /* XYAG_ZOOM_PAN               */
};

static int      XyagInputEventZoomTable[] =
{
    XYAG_INPUT_POINT,           /* XYAG_ZOOM_CENTER            */
    XYAG_INPUT_BOX,             /* XYAG_ZOOM_IN                */
    XYAG_INPUT_LINE             /* XYAG_ZOOM_PAN               */
};

/*------------------------------------------------------------\
|                          Translation                        |
\------------------------------------------------------------*/

/*------------------------------------------------------------\
|                      Expose Edit Zoom                       |
\------------------------------------------------------------*/

static long     XyagUnitEventZoomX[2] = {0, 0};
static long     XyagUnitEventZoomY[2] = {0, 0};
static int      XyagMaxEventZoom = 0;
int             XyagCountEventZoom = 0;

static long     XyagUnitEventEditX[2] = {0, 0};
static long     XyagUnitEventEditY[2] = {0, 0};
static int      XyagMaxEventEdit = 2;
int             XyagCountEventEdit = 0;

static int      XyagFirstExpose = XYAG_TRUE;

/*------------------------------------------------------------\
|                          Input                              |
\------------------------------------------------------------*/

static int      XyagInputMode = XYAG_INPUT_HALF_BOX;
static int      XyagSaveInputMode = XYAG_INPUT_HALF_BOX;
static int      XyagEditMode = XYAG_EDIT_MEASURE;
static int      XyagSaveEditMode = XYAG_EDIT_MEASURE;

/*------------------------------------------------------------\
|                      XyagChangeEditMode                     |
\------------------------------------------------------------*/

void 
XyagChangeEditMode(EditMode)
    int             EditMode;
{
    if (EditMode & XYAG_ZOOM_MARK) {
        EditMode = EditMode & ~XYAG_ZOOM_MARK;

        XyagCountEventZoom = 0;
        XyagMaxEventZoom = XyagMaxEventZoomTable[EditMode];

        if (!(XyagEditMode & XYAG_ZOOM_MARK)) {
            XyagSaveEditMode = XyagEditMode;
            XyagSaveInputMode = XyagInputMode;
        }

        XyagInputMode = XyagInputEventZoomTable[EditMode];
        XyagEditMode = EditMode | XYAG_ZOOM_MARK;
    }
    else {
        if (XyagEditMode & XYAG_ZOOM_MARK) {
            XyagSaveEditMode = EditMode;
            XyagSaveInputMode = XyagInputEventEditTable[EditMode];
            XyagCountEventEdit = 0;
            XyagMaxEventEdit = XyagMaxEventEditTable[EditMode];
        }
        else {
            if (XyagEditMode != EditMode) {
                XyagCountEventEdit = 0;
                XyagMaxEventEdit = XyagMaxEventEditTable[EditMode];
                XyagEditMode = EditMode;
                XyagInputMode = XyagInputEventEditTable[EditMode];
            }
        }

        XyagEditUnselectAll();
    }
}

/*------------------------------------------------------------\
|                      XyagTreatEventZoom                     |
\------------------------------------------------------------*/

void 
XyagTreatEventZoom()
{
    switch (XyagEditMode) {
    case XYAG_ZOOM_CENTER:
        XyagZoomCenter(XyagUnitEventZoomX[0], XyagUnitEventZoomY[0]);
        break;

    case XYAG_ZOOM_IN:
        XyagZoomIn(XyagUnitEventZoomX[0], XyagUnitEventZoomY[0], XyagUnitEventZoomX[1], XyagUnitEventZoomY[1]);
        break;

    case XYAG_ZOOM_PAN:
        XyagZoomPan(XyagUnitEventZoomX[0], XyagUnitEventZoomY[0], XyagUnitEventZoomX[1], XyagUnitEventZoomY[1]);
        break;
    }
}

/*------------------------------------------------------------\
|                       XyagTreatEventEdit                    |
\------------------------------------------------------------*/

void 
XyagTreatEventEdit()
{
    long            DeltaX;
    long            DeltaY;
    long            X1;
    long            X2;
    long            Y1;
    long            Y2;

    if (XyagFigure == NULL) {
        XyagWarningMessage(XyagMainWindow, "Load a figure before... Please");
        return;
    }

    X1 = XyagUnitEventEditX[0];
    X2 = XyagUnitEventEditX[1];
    Y1 = XyagUnitEventEditY[0];
    Y2 = XyagUnitEventEditY[1];

    if ((XyagInputMode == XYAG_INPUT_LSTRING) ||
        (XyagInputMode == XYAG_INPUT_ORTHO) ||
        (XyagInputMode == XYAG_INPUT_SORTHO)) {
        DeltaX = X1 - X2;
        DeltaY = Y1 - Y2;

        if (DeltaX < 0)
            DeltaX = -DeltaX;
        if (DeltaY < 0)
            DeltaY = -DeltaY;

        if (DeltaX > DeltaY) {
            Y2 = Y1;

            XyagUnitEventEditX[0] = X2;
            XyagUnitEventEditY[0] = Y2;
        }
        else {
            X2 = X1;

            XyagUnitEventEditX[0] = X2;
            XyagUnitEventEditY[0] = Y2;
        }
    }

    if (XyagInputMode == XYAG_INPUT_LSTRING) {
        XyagCountEventEdit = 1;
    }
    else {
        XyagCountEventEdit = 0;
    }

    switch (XyagEditMode) {
    case XYAG_EDIT_MEASURE:
        break;

    case XYAG_EDIT_IDENTIFY:
        XyagEditIdentify(X1, Y1);
        break;

    case XYAG_EDIT_BEHAVIOUR:
        XyagEditBehaviour(X1, Y1);
        break;

    case XYAG_EDIT_SELECT:    
        XyagEditSelect(X1, Y1);
	XyagZoomRefresh();
        break;

    case XYAG_EDIT_TREE:
      XyagThruTree(X1, Y1);
      break;

    case XYAG_EDIT_FORWARDEXTRACT:
        XyagViewDep(X1, Y1, CGV_FROM_CNS | CGV_SEL_SUCC_LOGIC, 0);
        break;

    case XYAG_EDIT_BACKWARDEXTRACT:
        XyagViewDep(X1, Y1, CGV_FROM_CNS | CGV_SEL_PREC_LOGIC, 0);
        break;

    case XYAG_EDIT_BOTHEXTRACT:
        XyagViewDep(X1, Y1, CGV_FROM_CNS | CGV_SEL_PREC_LOGIC | CGV_SEL_SUCC_LOGIC, 0);
        break;

    case XYAG_EDIT_FORWARDHILITE:
        XyagViewDep(X1, Y1, CGV_FROM_CNS | CGV_SEL_SUCC_LOGIC, 1);
        break;

    case XYAG_EDIT_BACKWARDHILITE:
        XyagViewDep(X1, Y1, CGV_FROM_CNS | CGV_SEL_PREC_LOGIC, 1);
        break;

    case XYAG_EDIT_BOTHHILITE:
        XyagViewDep(X1, Y1, CGV_FROM_CNS | CGV_SEL_PREC_LOGIC | CGV_SEL_SUCC_LOGIC, 1);
        break;
    }
}

/*------------------------------------------------------------\
|                       CallbackEvent                         |
\------------------------------------------------------------*/

void 
CallbackEvent(MyWidget, Event, Args, Argc)
    Widget          MyWidget;
    XButtonEvent   *Event;
    String         *Args;
    int            *Argc;
{
    Display        *EventDisplay;
    int             MouseEvent;
    Position        OldUnitCursorX;
    Position        OldUnitCursorY;

    EventDisplay = Event->display;

    MouseEvent = atoi(Args[0]);

    if (MouseEvent != XYAG_LEAVE) {
        OldUnitCursorX = XyagUnitCursorX;
        OldUnitCursorY = XyagUnitCursorY;

        XyagComputeCursor(Event->x, Event->y);

        switch (MouseEvent) {
        case XYAG_B1DN:

            if (XyagEditMode & XYAG_ZOOM_MARK) {
                if (XyagCountEventZoom < XyagMaxEventZoom) {
                    XyagUnitEventZoomX[XyagCountEventZoom] = XyagUnitCursorX;
                    XyagUnitEventZoomY[XyagCountEventZoom] = XyagUnitCursorY;

                    XyagCountEventZoom = XyagCountEventZoom + 1;

                    if (XyagCountEventZoom == XyagMaxEventZoom) {
                        XyagTreatEventZoom();

                        XyagInputMode = XyagSaveInputMode;
                        XyagEditMode = XyagSaveEditMode;
                        XyagSaveEditMode = XYAG_EDIT_MEASURE;
                        XyagSaveInputMode = XYAG_INPUT_HALF_BOX;
                        XyagCountEventZoom = 0;
                        XyagMaxEventZoom = 0;

                        XyagComputeCursor(Event->x, Event->y);
                    }
                }
            }
            else {
                if (XyagCountEventEdit < XyagMaxEventEdit) {
                    XyagUnitEventEditX[XyagCountEventEdit] = XyagUnitCursorX;
                    XyagUnitEventEditY[XyagCountEventEdit] = XyagUnitCursorY;

                    XyagCountEventEdit = XyagCountEventEdit + 1;
                    if (XyagCountEventEdit == XyagMaxEventEdit) {
                        XyagTreatEventEdit();
                    }
                }
            }

            break;

        case XYAG_B3DN:

            if (XyagEditMode & XYAG_ZOOM_MARK) {
                if (XyagCountEventZoom != 0) {
                    XyagCountEventZoom = XyagCountEventZoom - 1;
                }
                else {
                    XyagInputMode = XyagSaveInputMode;
                    XyagEditMode = XyagSaveEditMode;
                    XyagSaveEditMode = XYAG_EDIT_MEASURE;
                    XyagSaveInputMode = XYAG_INPUT_HALF_BOX;
                    XyagCountEventZoom = 0;
                    XyagMaxEventZoom = 0;
                }
            }
            else {
                if (XyagCountEventEdit != 0) {
                    XyagCountEventEdit = XyagCountEventEdit - 1;
                }
                else {
                    XyagEditUnselectAll();

                    XyagInputMode = XYAG_INPUT_HALF_BOX;
                    XyagEditMode = XYAG_EDIT_MEASURE;
                    XyagCountEventEdit = 0;
                    XyagMaxEventEdit = 2;
                }
            }

            break;

        case XYAG_MOTION:
            break;

        case XYAG_ENTER:
            break;

        case XYAG_KEY_UP:
            XyagZoomUp();
            break;

        case XYAG_KEY_DN:
            XyagZoomDown();
            break;

        case XYAG_KEY_LEFT:
            XyagZoomLeft();
            break;

        case XYAG_KEY_RIGHT:
            XyagZoomRight();
            break;
	case XYAG_KEY_BACKSPACE:
	  CallbackPreviousExtract(NULL, NULL, NULL);
	  break;
        }
    }
}

/*------------------------------------------------------------\
|                       CallbackResize                        |
\------------------------------------------------------------*/

void 
CallbackResize(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    XtPointer       ClientData;
    XmDrawingAreaCallbackStruct *CallData;
{
    if (XyagFirstExpose == XYAG_FALSE) {
        XyagSetMouseCursor(XyagGraphicWindow, XYAG_WATCH_CURSOR);
        XyagResizeGraphicWindow();
        XyagRefreshGraphicWindow(0, 0, XyagGraphicDx, XyagGraphicDy);
        XyagSetMouseCursor(XyagGraphicWindow, XYAG_NORMAL_CURSOR);
    }
}

/*------------------------------------------------------------\
|                       CallbackExpose                        |
\------------------------------------------------------------*/

void 
CallbackExpose(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    XtPointer       ClientData;
    XmDrawingAreaCallbackStruct *CallData;
{
    XExposeEvent   *ExposeEvent;

    ExposeEvent = (XExposeEvent *) CallData->event;

    if (XyagFirstExpose == XYAG_TRUE) {
        XyagFirstExpose = XYAG_FALSE;
        XyagInitializeGraphicWindow();
        XyagLoadConfig(XYAG_FALSE);
        if (XyagFirstFileName != (char *) NULL) {
            XyagFileOpen(XyagFirstFileName);
            XyagFirstFileName = NULL;
        }
        else {
            XyagChangeTopLevelTitle((char *) NULL);
            XyagRefreshGraphicWindow(0, 0, XyagGraphicDx, XyagGraphicDy);
        }
        if(XyagCallbackFileQuitFonction == NULL)
            XyagEnterPresentPanel();
    }
    else {
        XyagRefreshGraphicWindow(ExposeEvent->x, ExposeEvent->y, ExposeEvent->width, ExposeEvent->height);
    }
}

/*------------------------------------------------------------\
|                      XyagInitializeEvent                    |
\------------------------------------------------------------*/

void 
XyagInitializeEvent()
{
    XtActionsRec    NewActions;

    XtVaSetValues(XyagGraphicWindow,
                  XmNtranslations, XtParseTranslationTable(XyagEventTranslation),
                  NULL);

    NewActions.string = "CallbackEvent";
    NewActions.proc = CallbackEvent;

    XtAppAddActions(XyagleApplication, &NewActions, 1);

    XtAddCallback(XyagGraphicWindow,
                  XmNresizeCallback, CallbackResize,
                  NULL);

    XtAddCallback(XyagGraphicWindow,
                  XmNexposeCallback, CallbackExpose,
                  NULL);
}

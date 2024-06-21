/*------------------------------------------------------------\
|                                                             |
| Tool    :                    XYAG                           |
|                                                             |
| File    :                   Panel.c                         |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                        Lester Anthony                       |
|                                                             |
\------------------------------------------------------------*/

#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/FileSB.h>
#include <Xm/SelectioB.h>
#include <Xm/PushBG.h>
#include <Xm/Text.h>
#include <Xm/CascadeBG.h>

#include BEH_H
#include BHL_H
#include MUT_H
#include MLO_H
#include LOG_H
#include BVL_H
#include CGV_H
#include XSB_H
#include XYA_H
#include XMX_H
#include XTB_H
#include XMF_H

#include "XMV_panel.h"
#include "XMV_view.h"
#include "XMX_color.h"

cgvfig_list    *XyagFigureCgvTmp;

/*------------------------------------------------------------\
|                        Panel Layer                          |
\------------------------------------------------------------*/

static char    *XyagViewLayerButtonBackground = "Black";
static char    *XyagViewLayerButtonForeground = "White";

static char    *XyagPanelLayerButtonName[] =
{
    "All visible",
    "All invisible",
    "I/O Names",
    "Gate Names",
    "Apply",
    "Close"
};

static char    *XyagViewLayerCursorType[] =
{
    "Invert",
    "Xor"
};

static char    *XyagViewLayerForceDisplay[] =
{
    "Quick display",
    "Force display"
};

XyagPanelButtonItem XyagViewLayerButton[] =
{
    {
        &XyagPanelLayerButtonName[0],
        NULL, NULL, 0, 0,
        NULL,
        NULL,
        0, 0,
        1, 1,
        CallbackLayerAllVisible,
        (XtPointer) NULL,
        (Widget) NULL
    }
    ,
    {
        &XyagPanelLayerButtonName[1],
        NULL, NULL, 0, 0,
        NULL,
        NULL,
        1, 0,
        1, 1,
        CallbackLayerAllInvisible,
        (XtPointer) NULL,
        (Widget) NULL
    }
    ,
    {
        &XYAG_LAYER_NAME_TABLE[0][0],
        NULL, NULL, 0, 0,
        &XYAG_LAYER_NAME_TABLE[0][1],
        &XYAG_LAYER_NAME_TABLE[0][2],
        0, 1,
        1, 1,
        CallbackLayerVisible,
        (XtPointer) 0,
        (Widget) NULL
    }
    ,
    {
        &XYAG_LAYER_NAME_TABLE[0][0],
        NULL, NULL, 0, 0,
        &XYAG_LAYER_NAME_TABLE[0][1],
        &XYAG_LAYER_NAME_TABLE[0][2],
        1, 1,
        1, 1,
        CallbackLayerInvisible,
        (XtPointer) 0,
        (Widget) NULL
    }
    ,
    {
        &XYAG_LAYER_NAME_TABLE[1][0],
        NULL, NULL, 0, 0,
        &XYAG_LAYER_NAME_TABLE[1][1],
        &XYAG_LAYER_NAME_TABLE[1][2],
        0, 2,
        1, 1,
        CallbackLayerVisible,
        (XtPointer) 1,
        (Widget) NULL
    }
    ,
    {
        &XYAG_LAYER_NAME_TABLE[1][0],
        NULL, NULL, 0, 0,
        &XYAG_LAYER_NAME_TABLE[1][1],
        &XYAG_LAYER_NAME_TABLE[1][2],
        1, 2,
        1, 1,
        CallbackLayerInvisible,
        (XtPointer) 1,
        (Widget) NULL
    }
    ,
    {
        &XYAG_LAYER_NAME_TABLE[2][0],
        NULL, NULL, 0, 0,
        &XYAG_LAYER_NAME_TABLE[2][1],
        &XYAG_LAYER_NAME_TABLE[2][2],
        0, 3,
        1, 1,
        CallbackLayerVisible,
        (XtPointer) 2,
        (Widget) NULL
    }
    ,
    {
        &XYAG_LAYER_NAME_TABLE[2][0],
        NULL, NULL, 0, 0,
        &XYAG_LAYER_NAME_TABLE[2][1],
        &XYAG_LAYER_NAME_TABLE[2][2],
        1, 3,
        1, 1,
        CallbackLayerInvisible,
        (XtPointer) 2,
        (Widget) NULL
    }
    ,
    {
        &XYAG_LAYER_NAME_TABLE[3][0],
        NULL, NULL, 0, 0,
        &XYAG_LAYER_NAME_TABLE[3][1],
        &XYAG_LAYER_NAME_TABLE[3][2],
        0, 4,
        1, 1,
        CallbackLayerVisible,
        (XtPointer) 3,
        (Widget) NULL
    }
    ,
    {
        &XYAG_LAYER_NAME_TABLE[3][0],
        NULL, NULL, 0, 0,
        &XYAG_LAYER_NAME_TABLE[3][1],
        &XYAG_LAYER_NAME_TABLE[3][2],
        1, 4,
        1, 1,
        CallbackLayerInvisible,
        (XtPointer) 3,
        (Widget) NULL
    }
    ,
    {
        &XYAG_LAYER_NAME_TABLE[4][0],
        NULL, NULL, 0, 0,
        &XYAG_LAYER_NAME_TABLE[4][1],
        &XYAG_LAYER_NAME_TABLE[4][2],
        0, 5,
        1, 1,
        CallbackLayerVisible,
        (XtPointer) 4,
        (Widget) NULL
    }
    ,
    {
        &XYAG_LAYER_NAME_TABLE[4][0],
        NULL, NULL, 0, 0,
        &XYAG_LAYER_NAME_TABLE[4][1],
        &XYAG_LAYER_NAME_TABLE[4][2],
        1, 5,
        1, 1,
        CallbackLayerInvisible,
        (XtPointer) 4,
        (Widget) NULL
    }
    ,
    {
        &XyagPanelLayerButtonName[2],
        NULL, NULL, 0, 0,
        &XyagViewLayerButtonForeground,
        &XyagViewLayerButtonBackground,
        0, 6,
        1, 1,
        CallbackNameVisible,
        (XtPointer) 0,
        (Widget) NULL
    }
    ,
    {
        &XyagPanelLayerButtonName[2],
        NULL, NULL, 0, 0,
        &XyagViewLayerButtonForeground,
        &XyagViewLayerButtonBackground,
        1, 6,
        1, 1,
        CallbackNameInvisible,
        (XtPointer) 0,
        (Widget) NULL
    }
    ,
    {
        &XyagPanelLayerButtonName[3],
        NULL, NULL, 0, 0,
        &XyagViewLayerButtonForeground,
        &XyagViewLayerButtonBackground,
        0, 7,
        1, 1,
        CallbackNameVisible,
        (XtPointer) 1,
        (Widget) NULL
    }
    ,
    {
        &XyagPanelLayerButtonName[3],
        NULL, NULL, 0, 0,
        &XyagViewLayerButtonForeground,
        &XyagViewLayerButtonBackground,
        1, 7,
        1, 1,
        CallbackNameInvisible,
        (XtPointer) 1,
        (Widget) NULL
    }
    ,
    {
        &XyagViewLayerCursorType[0],
        NULL, NULL, 0, 0,
        NULL,
        NULL,
        0, 8,
        1, 1,
        CallbackLayerCursor,
        (XtPointer) NULL,
        (Widget) NULL
    }
    ,
    {
        &XyagViewLayerForceDisplay[0],
        NULL, NULL, 0, 0,
        NULL,
        NULL,
        1, 8,
        1, 1,
        CallbackLayerForceDisplay,
        (XtPointer) NULL,
        (Widget) NULL
    }
    ,
    {
        &XyagPanelLayerButtonName[4],
        NULL, NULL, 0, 0,
        NULL,
        NULL,
        0, 9,
        1, 1,
        CallbackLayerApply,
        (XtPointer) NULL,
        (Widget) NULL
    }
    ,
    {
        &XyagPanelLayerButtonName[5],
        NULL, NULL, 0, 0,
        NULL,
        NULL,
        1, 9,
        1, 1,
        CallbackCloseLayer,
        (XtPointer) NULL,
        (Widget) NULL
    }
    ,
	{
	  0, NULL, NULL, 0, 0, NULL, NULL, 0, 0, 0, 0, NULL, NULL, NULL
	}
};

XyagPanelItem   XyagViewLayerPanel =
{
    "Layer",
    1,
    0,
    XYAG_VIEW_LAYER_X,
    XYAG_VIEW_LAYER_Y,
    200,
    250,
    2,
    10,
    (Widget) NULL,
    (Widget) NULL,
    (Widget) NULL,
    (Widget) NULL,
    XyagViewLayerButton
};

int             XyagViewLayerDefaultValues[5] =
{
    XYAG_VIEW_LAYER_X,
    XYAG_VIEW_LAYER_Y,
    200, 250, 0
};

/*------------------------------------------------------------\
|                       Panel Message                         |
\------------------------------------------------------------*/

static char    *XyagPanelMessageButtonName[] =
{
    "Text",
    "Close"
};

XyagPanelButtonItem XyagViewMessageButton[] =
{
    {
        &XyagPanelMessageButtonName[0],
        "Nothing", NULL, 0, 0,
        NULL,
        NULL,
        0, 0,
        8, 9,
        NULL,
        (XtPointer) NULL,
        (Widget) NULL
    }
    ,
    {
        &XyagPanelMessageButtonName[1],
        NULL, NULL, 0, 0,
        NULL,
        NULL,
        3, 9,
        2, 1,
        CallbackViewCloseMessage,
        (XtPointer) NULL,
        (Widget) NULL
    }
    ,
	{
	  0, NULL, NULL, 0, 0, NULL, NULL, 0, 0, 0, 0, NULL, NULL, NULL
	}
};

XyagPanelItem   XyagViewMessagePanel =
{
    "Message",
    1,
    0,
    XYAG_VIEW_MESSAGE_X,
    XYAG_VIEW_MESSAGE_Y,
    360,
    250,
    8,
    10,
    (Widget) NULL,
    (Widget) NULL,
    (Widget) NULL,
    (Widget) NULL,
    XyagViewMessageButton
};

int             XyagViewMessageDefaultValues[5] =
{
    XYAG_VIEW_MESSAGE_X,
    XYAG_VIEW_MESSAGE_Y,
    360, 250, 0
};


/*------------------------------------------------------------\
|                          Functions                          |
\------------------------------------------------------------*/

/*------------------------------------------------------------\
|                         Zoom Callback                       |
\------------------------------------------------------------*/

/*------------------------------------------------------------\
|                       CallbackZoomRefresh                   |
\------------------------------------------------------------*/

void 
CallbackZoomRefresh(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    XyagZoomRefresh();
}

/*------------------------------------------------------------\
|                     CallbackZoomPrevious                    |
\------------------------------------------------------------*/

void 
CallbackZoomPrevious(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    XyagZoomUndo();
}

/*------------------------------------------------------------\
|                       CallbackZoomLess                      |
\------------------------------------------------------------*/

void 
CallbackZoomLess(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    XyagZoomLess();
}

/*------------------------------------------------------------\
|                        CallbackZoomMore                     |
\------------------------------------------------------------*/

void 
CallbackZoomMore(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    XyagZoomMore();
}

/*------------------------------------------------------------\
|                       CallbackZoomSet                       |
\------------------------------------------------------------*/

void 
CallbackZoomSet(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    XyagZoomSet();
}

/*------------------------------------------------------------\
|                        CallbackZoomIn                       |
\------------------------------------------------------------*/

void 
CallbackZoomIn(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    XyagChangeEditMode(XYAG_ZOOM_IN);
}

/*------------------------------------------------------------\
|                        CallbackZoomCenter                   |
\------------------------------------------------------------*/

void 
CallbackZoomCenter(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    XyagChangeEditMode(XYAG_ZOOM_CENTER);
}

/*------------------------------------------------------------\
|                        CallbackZoomPan                      |
\------------------------------------------------------------*/

void 
CallbackZoomPan(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    XyagChangeEditMode(XYAG_ZOOM_PAN);
}

/*------------------------------------------------------------\
|                        CallbackZoomFit                      |
\------------------------------------------------------------*/

void 
CallbackZoomFit(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    XyagZoomFit();
}

/*------------------------------------------------------------\
|                      Callback For Layer                     |
\------------------------------------------------------------*/

/*------------------------------------------------------------\
|                    CallbackLayerAllVisible                  |
\------------------------------------------------------------*/

void 
CallbackLayerAllVisible(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    XyagLayerAllVisible();
}

/*------------------------------------------------------------\
|                    CallbackLayerAllInvisible                |
\------------------------------------------------------------*/

void 
CallbackLayerAllInvisible(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    XyagLayerAllInvisible();
}

/*------------------------------------------------------------\
|                     CallbackLayerVisible                    |
\------------------------------------------------------------*/

void 
CallbackLayerVisible(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    if (XYAG_ACTIVE_LAYER_TABLE[(int) ClientData] == 0) {
        XyagSetLayerVisible((int) ClientData);
    }
}

/*------------------------------------------------------------\
|                     CallbackLayerInvisible                  |
\------------------------------------------------------------*/

void 
CallbackLayerInvisible(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    if (XYAG_ACTIVE_LAYER_TABLE[(int) ClientData]) {
        XyagSetLayerInvisible((int) ClientData);
    }
}

/*------------------------------------------------------------\
|                      CallbackNameVisible                    |
\------------------------------------------------------------*/

void 
CallbackNameVisible(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    if (XYAG_ACTIVE_NAME_TABLE[(int) ClientData] == 0) {
        XyagSetNameVisible(ClientData);
    }
}

/*------------------------------------------------------------\
|                      CallbackNameInvisible                  |
\------------------------------------------------------------*/

void 
CallbackNameInvisible(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    if (XYAG_ACTIVE_NAME_TABLE[(int) ClientData]) {
        XyagSetNameInvisible(ClientData);
    }
}

/*------------------------------------------------------------\
|                       CallbackLayerCursor                   |
\------------------------------------------------------------*/

void 
CallbackLayerCursor(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    XYAG_XOR_CURSOR = !XYAG_XOR_CURSOR;
    XyagChangeCursor(XYAG_XOR_CURSOR);

    XyagChangeCursorContext();
}

/*------------------------------------------------------------\
|                       CallbackLayerForceDisplay             |
\------------------------------------------------------------*/

void 
CallbackLayerForceDisplay(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    XYAG_FORCE_DISPLAY = !XYAG_FORCE_DISPLAY;
    XyagChangeForceDisplay(XYAG_FORCE_DISPLAY);
}


/*------------------------------------------------------------\
|                      XyagChangeCursor                      |
\------------------------------------------------------------*/

void 
XyagChangeCursor(Type)
    char            Type;
{
    int             Pos;
    XmString        Label;

    Pos = 2 + ((XYAG_MAX_LAYER + XYAG_MAX_ACTIVE_NAME) << 1);
    Label = XmStringCreateSimple(XyagViewLayerCursorType[(int)Type]);

    XtVaSetValues(XyagViewLayerButton[Pos].BUTTON,
                  XmNlabelString, Label,
                  XmNresizable, False,
                  NULL);

    XmStringFree(Label);
}

/*------------------------------------------------------------\
|                      XyagChangeForceDisplay                |
\------------------------------------------------------------*/

void 
XyagChangeForceDisplay(Index)
    char            Index;
{
    int             Pos;
    XmString        Label;

    Pos = 3 + ((XYAG_MAX_LAYER + XYAG_MAX_ACTIVE_NAME) << 1);
    Label = XmStringCreateSimple(XyagViewLayerForceDisplay[(int)Index]);

    XtVaSetValues(XyagViewLayerButton[Pos].BUTTON,
                  XmNlabelString, Label,
                  XmNresizable, False,
                  NULL);

    XmStringFree(Label);
}


/*------------------------------------------------------------\
|                      CallbackLayerApply                     |
\------------------------------------------------------------*/

void 
CallbackLayerApply(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    XyagZoomRefresh();
}

/*------------------------------------------------------------\
|                      CallbackCloseLayer                     |
\------------------------------------------------------------*/

void 
CallbackCloseLayer(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    XyagExitPanel(&XyagViewLayerPanel);
}

/*------------------------------------------------------------\
|                       XyagSetLayerVisible                   |
\------------------------------------------------------------*/

void 
XyagSetLayerVisible(Layer)
    int             Layer;
{
    int             Index;
    char           *Background;
    char           *Foreground;

    Index = (Layer << 1) + 2;

    if (XyagViewLayerButton[Index].BUTTON != NULL) {
        XYAG_ACTIVE_LAYER_TABLE[Layer] = 1;

        XtVaSetValues(XyagViewLayerButton[Index + 1].BUTTON,
                      XtVaTypedArg,
                      XmNbackground,
                      XmRString,
                      XYAG_BLACK_COLOR_NAME,
                      strlen(XYAG_BLACK_COLOR_NAME) + 1,
                      NULL
            );

        XtVaSetValues(XyagViewLayerButton[Index + 1].BUTTON,
                      XtVaTypedArg,
                      XmNforeground,
                      XmRString,
                      XYAG_BLACK_COLOR_NAME,
                      strlen(XYAG_BLACK_COLOR_NAME) + 1,
                      NULL
            );

        Background = *XyagViewLayerButton[Index].BACKGROUND;
        Foreground = *XyagViewLayerButton[Index].FOREGROUND;

        XtVaSetValues(XyagViewLayerButton[Index].BUTTON,
                      XtVaTypedArg,
                      XmNbackground,
                      XmRString,
                      Background,
                      strlen(Background) + 1,
                      NULL
            );

        XtVaSetValues(XyagViewLayerButton[Index].BUTTON,
                      XtVaTypedArg,
                      XmNforeground,
                      XmRString,
                      Foreground,
                      strlen(Foreground) + 1,
                      NULL
            );
    }
}

/*------------------------------------------------------------\
|                        XyagSetNameVisible                   |
\------------------------------------------------------------*/

void 
XyagSetNameVisible(Name)
    int             Name;
{
    int             Index;
    char           *Background;
    char           *Foreground;

    Index = 2 + ((XYAG_MAX_LAYER + Name) << 1);

    if (XyagViewLayerButton[Index].BUTTON != NULL) {
        XYAG_ACTIVE_NAME_TABLE[Name] = 1;

        XtVaSetValues(XyagViewLayerButton[Index + 1].BUTTON,
                      XtVaTypedArg,
                      XmNbackground,
                      XmRString,
                      XYAG_BLACK_COLOR_NAME,
                      strlen(XYAG_BLACK_COLOR_NAME) + 1,
                      NULL
            );

        XtVaSetValues(XyagViewLayerButton[Index + 1].BUTTON,
                      XtVaTypedArg,
                      XmNforeground,
                      XmRString,
                      XYAG_BLACK_COLOR_NAME,
                      strlen(XYAG_BLACK_COLOR_NAME) + 1,
                      NULL
            );

        Background = *XyagViewLayerButton[Index].BACKGROUND;
        Foreground = *XyagViewLayerButton[Index].FOREGROUND;

        XtVaSetValues(XyagViewLayerButton[Index].BUTTON,
                      XtVaTypedArg,
                      XmNbackground,
                      XmRString,
                      Background,
                      strlen(Background) + 1,
                      NULL
            );

        XtVaSetValues(XyagViewLayerButton[Index].BUTTON,
                      XtVaTypedArg,
                      XmNforeground,
                      XmRString,
                      Foreground,
                      strlen(Foreground) + 1,
                      NULL
            );
    }
}

/*------------------------------------------------------------\
|                     XyagSetLayerInvisible                   |
\------------------------------------------------------------*/

void 
XyagSetLayerInvisible(Layer)
    int             Layer;
{
    int             Index;
    char           *Background;
    char           *Foreground;

    Index = 3 + (Layer << 1);

    if (XyagViewLayerButton[Index].BUTTON != NULL) {
        XYAG_ACTIVE_LAYER_TABLE[Layer] = 0;

        XtVaSetValues(XyagViewLayerButton[Index - 1].BUTTON,
                      XtVaTypedArg,
                      XmNbackground,
                      XmRString,
                      XYAG_BLACK_COLOR_NAME,
                      strlen(XYAG_BLACK_COLOR_NAME) + 1,
                      NULL
            );

        XtVaSetValues(XyagViewLayerButton[Index - 1].BUTTON,
                      XtVaTypedArg,
                      XmNforeground,
                      XmRString,
                      XYAG_BLACK_COLOR_NAME,
                      strlen(XYAG_BLACK_COLOR_NAME) + 1,
                      NULL
            );

        Background = *XyagViewLayerButton[Index].BACKGROUND;
        Foreground = *XyagViewLayerButton[Index].FOREGROUND;

        XtVaSetValues(XyagViewLayerButton[Index].BUTTON,
                      XtVaTypedArg,
                      XmNbackground,
                      XmRString,
                      Background,
                      strlen(Background) + 1,
                      NULL
            );

        XtVaSetValues(XyagViewLayerButton[Index].BUTTON,
                      XtVaTypedArg,
                      XmNforeground,
                      XmRString,
                      Foreground,
                      strlen(Foreground) + 1,
                      NULL
            );
    }
}

/*------------------------------------------------------------\
|                      XyagSetNameInvisible                   |
\------------------------------------------------------------*/

void 
XyagSetNameInvisible(Name)
    int             Name;
{
    int             Index;
    char           *Background;
    char           *Foreground;

    Index = 3 + ((XYAG_MAX_LAYER + Name) << 1);

    if (XyagViewLayerButton[Index].BUTTON != NULL) {
        XYAG_ACTIVE_NAME_TABLE[Name] = 0;

        XtVaSetValues(XyagViewLayerButton[Index - 1].BUTTON,
                      XtVaTypedArg,
                      XmNbackground,
                      XmRString,
                      XYAG_BLACK_COLOR_NAME,
                      strlen(XYAG_BLACK_COLOR_NAME) + 1,
                      NULL
            );

        XtVaSetValues(XyagViewLayerButton[Index - 1].BUTTON,
                      XtVaTypedArg,
                      XmNforeground,
                      XmRString,
                      XYAG_BLACK_COLOR_NAME,
                      strlen(XYAG_BLACK_COLOR_NAME) + 1,
                      NULL
            );

        Background = *XyagViewLayerButton[Index].BACKGROUND;
        Foreground = *XyagViewLayerButton[Index].FOREGROUND;

        XtVaSetValues(XyagViewLayerButton[Index].BUTTON,
                      XtVaTypedArg,
                      XmNbackground,
                      XmRString,
                      Background,
                      strlen(Background) + 1,
                      NULL
            );

        XtVaSetValues(XyagViewLayerButton[Index].BUTTON,
                      XtVaTypedArg,
                      XmNforeground,
                      XmRString,
                      Foreground,
                      strlen(Foreground) + 1,
                      NULL
            );
    }
}

/*------------------------------------------------------------\
|                       XyagLayerAllVisible                   |
\------------------------------------------------------------*/

void 
XyagLayerAllVisible()
{
    int             Index;

    for (Index = 0; Index < XYAG_MAX_LAYER; Index++) {
        XyagSetLayerVisible(Index);
    }

    for (Index = 0; Index < XYAG_MAX_ACTIVE_NAME; Index++) {
        XyagSetNameVisible(Index);
    }
}

/*------------------------------------------------------------\
|                     XyagInitializeLayer                     |
\------------------------------------------------------------*/

void 
XyagInitializeLayer()
{
    int             Index;

    for (Index = 0; Index < XYAG_MAX_LAYER; Index++) {
        if (XYAG_ACTIVE_LAYER_TABLE[Index] == 0) {
            XyagSetLayerInvisible(Index);
        }
        else {
            XyagSetLayerVisible(Index);
        }
    }

    for (Index = 0; Index < XYAG_MAX_ACTIVE_NAME; Index++) {
        if (XYAG_ACTIVE_NAME_TABLE[Index] == 0) {
            XyagSetNameInvisible(Index);
        }
        else {
            XyagSetNameVisible(Index);
        }
    }
}

/*------------------------------------------------------------\
|                     XyagLayerAllInvisible                   |
\------------------------------------------------------------*/

void 
XyagLayerAllInvisible()
{
    int             Index;

    for (Index = 0; Index < XYAG_MAX_LAYER; Index++) {
        XyagSetLayerInvisible(Index);
    }

    for (Index = 0; Index < XYAG_MAX_ACTIVE_NAME; Index++) {
        XyagSetNameInvisible(Index);
    }
}

/*------------------------------------------------------------\
|                     Callbacks For Message                   |
\------------------------------------------------------------*/

void 
CallbackViewCloseMessage(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    XyagExitPanel(&XyagViewMessagePanel);
}

void XyagDisplayMessage()
{
    char           *Message;

    Message = XyagGetErrorMessage();

    if (Message != (char *) NULL) {
        XmTextSetString(XyagViewMessageButton[0].BUTTON, Message);

        XyagEnterPanel(&XyagViewMessagePanel);
    }
    else {
        XmTextSetString(XyagViewMessageButton[0].BUTTON, "");
    }
}



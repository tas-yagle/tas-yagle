/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                           |
|                                                             |
| File    :                   Menu.c                          |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                        Lester Anthony                       |
|                                                             |
\------------------------------------------------------------*/

#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/PushBG.h>
#include <Xm/CascadeBG.h>

#include MUT_H
#include MLO_H
#include XSB_H
#include XTB_H
#include XMX_H
#include XMV_H

#include "XMV_menu.h"
#include "XMV_view.h"
#include "XMV_panel.h"

char            XyagFirstViewLayer = 1;

/*------------------------------------------------------------\
|                           View Menu                         |
\------------------------------------------------------------*/

XyagMenuItem    XyagViewMenu[] =
{
    {
        "Refresh",
        'R',
        "Ctrl<Key>R",
        "Ctrl+R",
        &xmPushButtonGadgetClass,
        False,
        False,
        False,
        False,
        CallbackZoomRefresh,
        (XtPointer) NULL,
        (Widget) NULL,
        (Widget) NULL,
        (XyagMenuItem *) NULL
    }
    ,
    {
        "Fit Window",
        'F',
        "Ctrl<Key>T",
        "Ctrl+T",
        &xmPushButtonGadgetClass,
        False,
        False,
        False,
        False,
        CallbackZoomFit,
        (XtPointer) NULL,
        (Widget) NULL,
        (Widget) NULL,
        (XyagMenuItem *) NULL
    }
    ,
    {"line",0,NULL,NULL,NULL,False,True,False,False,NULL,NULL,NULL,NULL,NULL}
    ,
    {
        "Zoom In",
        'I',
        "Ctrl<Key>I",
        "Ctrl+I",
        &xmPushButtonGadgetClass,
        False,
        False,
        False,
        False,
        CallbackZoomMore,
        (XtPointer) NULL,
        (Widget) NULL,
        (Widget) NULL,
        (XyagMenuItem *) NULL
    }
    ,
    {
        "Zoom Out",
        'O',
        "Ctrl<Key>U",
        "Ctrl+U",
        &xmPushButtonGadgetClass,
        False,
        False,
        False,
        False,
        CallbackZoomLess,
        (XtPointer) NULL,
        (Widget) NULL,
        (Widget) NULL,
        (XyagMenuItem *) NULL
    }
    ,
    {
        "Zoom Select",
        'Z',
        "Alt<Key>Z",
        "Ctrl+Z",
        &xmPushButtonGadgetClass,
        False,
        False,
        False,
        False,
        CallbackZoomIn,
        (XtPointer) NULL,
        (Widget) NULL,
        (Widget) NULL,
        (XyagMenuItem *) NULL
    }
    ,
    {"line",0,NULL,NULL,NULL,False,True,False,False,NULL,NULL,NULL,NULL,NULL}
    ,
    {
        "Zoom Set...",
        'S',
        NULL,
        NULL,
        &xmPushButtonGadgetClass,
        False,
        False,
        False,
        False,
        CallbackZoomSet,
        (XtPointer) NULL,
        (Widget) NULL,
        (Widget) NULL,
        (XyagMenuItem *) NULL
    }
    ,
    {"line",0,NULL,NULL,NULL,False,True,False,False,NULL,NULL,NULL,NULL,NULL}
    ,
    {
        "Previous",
        'P',
        NULL,
        NULL,
        &xmPushButtonGadgetClass,
        False,
        False,
        False,
        False,
        CallbackZoomPrevious,
        (XtPointer) NULL,
        (Widget) NULL,
        (Widget) NULL,
        (XyagMenuItem *) NULL
    }
    ,
    {
	  NULL, 0, NULL, NULL, NULL, 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL
	}
    
};

/*------------------------------------------------------------\
|                    CallbackViewMessage                     |
\------------------------------------------------------------*/

void
CallbackViewMessage(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    XyagEnterPanel(&XyagViewMessagePanel);
}

/*------------------------------------------------------------\
|                      CallbackViewLayer                      |
\------------------------------------------------------------*/

void 
CallbackViewLayer(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    if (XyagFirstViewLayer) {
        XyagFirstViewLayer = 0;
        XyagInitializeLayer();
    }

    XyagEnterPanel(&XyagViewLayerPanel);
}


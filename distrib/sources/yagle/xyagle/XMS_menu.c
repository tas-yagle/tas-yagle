/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                            |
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

#include MUT_H
#include MLO_H
#include XSB_H
#include XTB_H
#include XMS_H
#include XMT_H

#include "XMS_menu.h"
#include "XMV_menu.h"
#include "XMS_panel.h"
#include "XMS_setup.h"


/*------------------------------------------------------------\
|                         Options Menu                        |
\------------------------------------------------------------*/

XyagMenuItem    XyagSetupMenu[] =
{
    {
        "Default Config",
        'D',
        NULL,
        NULL,
        &xmPushButtonGadgetClass,
        False,
        False,
        False,
        False,
        CallbackSetupDefaultConfig,
        (XtPointer) NULL,
        (Widget) NULL,
        (Widget) NULL,
        (XyagMenuItem *) NULL
    }
    ,
    {
        "Load Config",
        'L',
        NULL,
        NULL,
        &xmPushButtonGadgetClass,
        False,
        False,
        False,
        False,
        CallbackSetupLoadConfig,
        (XtPointer) NULL,
        (Widget) NULL,
        (Widget) NULL,
        (XyagMenuItem *) NULL
    }
    ,
    {
        "Save Config",
        'S',
        NULL,
        NULL,
        &xmPushButtonGadgetClass,
        False,
        False,
        False,
        False,
        CallbackSetupSaveConfig,
        (XtPointer) NULL,
        (Widget) NULL,
        (Widget) NULL,
        (XyagMenuItem *) NULL
    }
    ,
    {"line",0,NULL,NULL,NULL,False,True,False,False,NULL,NULL,NULL,NULL,NULL}
    ,
    {
        "Layers...",
        'L',
        "Meta<Key>L",
        "Meta+L",
        &xmPushButtonGadgetClass,
        False,
        False,
        False,
        False,
        CallbackViewLayer,
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
|                CallbackSetupInformations                    |
\------------------------------------------------------------*/

void 
CallbackSetupInformations(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    XyagDisplayInformations();
}

/*------------------------------------------------------------\
|                  CallbackSetupLoadConfig                    |
\------------------------------------------------------------*/

void 
CallbackSetupLoadConfig(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    XyagLoadConfig(XYAG_TRUE);
}

/*------------------------------------------------------------\
|                  CallbackSetupSaveConfig                    |
\------------------------------------------------------------*/

void 
CallbackSetupSaveConfig(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    XyagSaveConfig();
}

/*------------------------------------------------------------\
|                CallbackSetupDefaultConfig                   |
\------------------------------------------------------------*/

void 
CallbackSetupDefaultConfig(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    XyagDefaultConfig();
}


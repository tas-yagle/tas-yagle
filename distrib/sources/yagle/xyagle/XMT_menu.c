/*------------------------------------------------------------\
|                                                             |
| Tool    :                  XYAG XMT                         |
|                                                             |
| File    :                 Yag Menu.c                        |
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
#include CGV_H
#include XSB_H
#include XTB_H
#include XYA_H
#include XMV_H
#include XMT_H
#include XMX_H
#include XMS_H

#include "XMT_menu.h"
#include "XMV_menu.h"
#include "XMS_menu.h"
#include "XME_menu.h"

/*------------------------------------------------------------\
|                          Windows Menu                       |
\------------------------------------------------------------*/

XyagMenuItem    XyagWindowsMenu[] =
{
    {
        "Show Structure",
        'S',
        "Meta<Key>S",
        "Meta+S",
        &xmPushButtonGadgetClass,
        False,
        False,
        False,
        False,
        CallbackEditIdentify,
        (XtPointer) NULL,
        (Widget) NULL,
        (Widget) NULL,
        (XyagMenuItem *) NULL
    }
    ,
#ifndef xcones
    {
        "Show Behaviour",
        'B',
        "Meta<Key>B",
        "Meta+B",
        &xmPushButtonGadgetClass,
        False,
        False,
        False,
        False,
        CallbackEditBehaviour,
        (XtPointer) NULL,
        (Widget) NULL,
        (Widget) NULL,
        (XyagMenuItem *) NULL
    }
    ,
#endif
    {
        "Show Message",
        'M',
        "Meta<Key>M",
        "Meta+M",
        &xmPushButtonGadgetClass,
        False,
        False,
        False,
        False,
        CallbackViewMessage,
        (XtPointer) NULL,
        (Widget) NULL,
        (Widget) NULL,
        (XyagMenuItem *) NULL
    }
    ,
    {
        "Show Info",
        'I',
        "Meta<Key>I",
        "Meta+I",
        &xmPushButtonGadgetClass,
        False,
        False,
        False,
        False,
        CallbackSetupInformations,
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



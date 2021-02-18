/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                            |
|                                                             |
| File    :                   Panel.c                         |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                        Lester Anthony                       |
|                                                             |
\------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include <Xm/Xm.h>
#include <Xm/FileSB.h>
#include <Xm/SelectioB.h>
#include <Xm/PushBG.h>
#include <Xm/Text.h>

#include MUT_H
#include MLO_H
#include CGV_H
#include XSB_H
#include XTB_H
#include XYA_H
#include XMS_H

#include "XMS_panel.h"

static char    *XyagPanelInformationsButtonName[] =
{
    "Text",
    "Close"
};

XyagPanelButtonItem XyagSetupInformationsButton[] =
{
    {
        &XyagPanelInformationsButtonName[0],
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
        &XyagPanelInformationsButtonName[1],
        NULL, NULL, 0, 0,
        NULL,
        NULL,
        3, 9,
        2, 1,
        XyagExitDialogCallback,
        (XtPointer) NULL,
        (Widget) NULL
    }
    ,
    {
        NULL, NULL, NULL, 0, 0, NULL, NULL, 0, 0, 0, 0, NULL, NULL, NULL
    }
};

XyagPanelItem   XyagSetupInformationsPanel =
{
    "Informations",
    1,
    0,
    XYAG_SETUP_INFORMATIONS_X,
    XYAG_SETUP_INFORMATIONS_Y,
    360,
    250,
    8,
    10,
    (Widget) NULL,
    (Widget) NULL,
    (Widget) NULL,
    (Widget) NULL,
    XyagSetupInformationsButton
};

int             XyagSetupInformationsDefaultValues[5] =
{
    XYAG_SETUP_INFORMATIONS_X,
    XYAG_SETUP_INFORMATIONS_Y,
    360, 250, 0
};

/*------------------------------------------------------------\
|                   XyagDisplayInformations                  |
\------------------------------------------------------------*/

void 
XyagDisplayInformations()
{
    char           *Message;

    Message = XyagGetInformations();
    XmTextSetString(XyagSetupInformationsButton[0].BUTTON, Message);
    XyagEnterPanel(&XyagSetupInformationsPanel);
    XyagLimitedLoop(XyagSetupInformationsPanel.PANEL);
    XyagExitPanel(&XyagSetupInformationsPanel);
}

/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                            |
|                                                             |
| File    :                  Menu.c                           |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                        Lester Anthony                       |
|                                                             |
\------------------------------------------------------------*/

#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/PushBG.h>
#include <Xm/CascadeBG.h>
#include <Xm/RowColumn.h>

#include MUT_H
#include MLO_H
#include CGV_H
#include XSB_H
#include XTB_H
#include XMF_H
#include XME_H
#include XMV_H
#include XMT_H
#include XMS_H
#include XMH_H
#include XMX_H

#include "XMX_menu.h"

/*------------------------------------------------------------\
|                          Menu Bar                           |
\------------------------------------------------------------*/

Widget          XyagMenuBar;

XyagMenuItem    XyagMainMenu[] =
{
    {
        "File",
        'F',
        NULL,
        NULL,
        &xmCascadeButtonGadgetClass,
        True,
        False,
        False,
        False,
        NULL,
        (XtPointer) NULL,
        (Widget) NULL,
        (Widget) NULL,
        XyagFileMenu
    }
    ,
    {
        "Edit",
        'E',
        NULL,
        NULL,
        &xmCascadeButtonGadgetClass,
        True,
        False,
        False,
        False,
        NULL,
        (XtPointer) NULL,
        (Widget) NULL,
        (Widget) NULL,
        XyagEditMenu
    }
    ,
    {
        "View",
        'V',
        NULL,
        NULL,
        &xmCascadeButtonGadgetClass,
        True,
        False,
        False,
        False,
        NULL,
        (XtPointer) NULL,
        (Widget) NULL,
        (Widget) NULL,
        XyagViewMenu
    }
    ,
    {
        "Windows",
        'W',
        NULL,
        NULL,
        &xmCascadeButtonGadgetClass,
        True,
        False,
        False,
        False,
        NULL,
        (XtPointer) NULL,
        (Widget) NULL,
        (Widget) NULL,
        XyagWindowsMenu
    }
    ,
    {
        "Options",
        'O',
        NULL,
        NULL,
        &xmCascadeButtonGadgetClass,
        True,
        False,
        False,
        False,
        NULL,
        (XtPointer) NULL,
        (Widget) NULL,
        (Widget) NULL,
        XyagSetupMenu
    }
    ,
    {
        "Help",
        'H',
        NULL,
        NULL,
        &xmCascadeButtonGadgetClass,
        True,
        False,
        False,
        True,
        NULL,
        (XtPointer) NULL,
        (Widget) NULL,
        (Widget) NULL,
        XyagHelpMenu
    }
    ,
    {
        NULL, 0, NULL, NULL, NULL, 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL
    }
};

/*------------------------------------------------------------\
|                      XyagInitializeMenu                     |
\------------------------------------------------------------*/

void 
XyagInitializeMenu()
{
    XyagMenuBar = XmCreateMenuBar(XyagMainWindow, "XyagMenuBar", NULL, 0);
    XyagBuildMenus(XyagMenuBar, XyagMainMenu);
    XtManageChild(XyagMenuBar);
}

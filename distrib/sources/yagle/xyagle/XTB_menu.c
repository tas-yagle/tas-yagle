/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                            |
|                                                             |
| File    :                  menu.c                           |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                        Lester Anthony                       |
|                                                             |
\------------------------------------------------------------*/

#include <Xm/Xm.h>
#include <X11/Intrinsic.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/SeparatoG.h>
#include <Xm/Separator.h>

#include MUT_H
#include MLO_H
#include XTB_H
#include XSB_H

#include "XTB_menu.h"

static char     MenuBuffer[MENU_BUFFER_SIZE];

/*------------------------------------------------------------\
|                       XyagBuildMenus                        |
\------------------------------------------------------------*/

void 
XyagBuildMenus(Father, Menu)
    Widget          Father;
    XyagMenuItem   *Menu;
{

    Widget          Button;
    Widget          SubMenu;
    XmString        Text;
    Arg             Args[2];
    int             Counter;

    for (Counter = 0; Menu[Counter].LABEL != NULL; Counter++) {
        if (Menu[Counter].SEPARATOR == True) {
            XtCreateManagedWidget("Separator", xmSeparatorWidgetClass, Father, NULL, 0);
            continue;
        }

        Button = XtVaCreateManagedWidget(Menu[Counter].LABEL,
                                         *Menu[Counter].CLASS, Father,
                                         XmNmnemonic,
                                         Menu[Counter].MNEMONIC,
                                         NULL);
        Menu[Counter].BUTTON = Button;

        if (Menu[Counter].SHORT_KEY != NULL) {
            XtVaSetValues(Button, XmNaccelerator, Menu[Counter].SHORT_KEY, NULL);
        }

        if (Menu[Counter].SHORT_KEY != NULL) {
            Text = XmStringCreateSimple(Menu[Counter].SHORT_KEY_TEXT);
            XtVaSetValues(Button, XmNacceleratorText, Text, NULL);
            XmStringFree(Text);
        }

        if (Menu[Counter].CALLBACK != NULL) {
            XtAddCallback(Button, XmNactivateCallback, Menu[Counter].CALLBACK, Menu[Counter].CALLDATA);
        }

        if (Menu[Counter].HELP == True) {
            XtSetArg(Args[0], XmNmenuHelpWidget, Button);
            XtSetValues(Father, Args, 1);
        }

        if (Menu[Counter].NEXT != NULL) {
            strcpy(MenuBuffer, Menu[Counter].LABEL);
            strcat(MenuBuffer, " Menu");

            SubMenu = XmCreatePulldownMenu(Father, MenuBuffer, Args, 0);

            Menu[Counter].MENU = SubMenu;

            if (Menu[Counter].TEAROFF == True) {
                XtVaSetValues(SubMenu, XmNtearOffModel, XmTEAR_OFF_ENABLED, NULL);
            }
            if (Menu[Counter].TITLE == True) {
                Text = XmStringCreateLtoR(MenuBuffer, XmSTRING_DEFAULT_CHARSET);
                XtVaCreateManagedWidget("MenuTitle", xmLabelWidgetClass, SubMenu, XmNlabelString, Text, NULL);
                XmStringFree(Text);
                XtCreateManagedWidget("Separator", xmSeparatorWidgetClass, SubMenu, NULL, 0);
            }

            XtSetArg(Args[0], XmNsubMenuId, SubMenu);
            XtSetValues(Button, Args, 1);

            XyagBuildMenus(SubMenu, Menu[Counter].NEXT);
        }
        else Menu[Counter].MENU = (Widget) NULL;
    }
}

/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                            |
|                                                             |
| File    :                  dialog.c                         |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                        Lester Anthony                       |
|                                                             |
\------------------------------------------------------------*/

#include <stdlib.h>
#include <Xm/Xm.h>
#include <Xm/FileSB.h>
#include <Xm/MessageB.h>
#include <Xm/SelectioB.h>
#include <Xm/SeparatoG.h>

#include MUT_H
#include MLO_H
#include XTB_H
#include XSB_H
#include XMX_H
#include XMV_H
#include XME_H
#include XMT_H

#include "XTB_dialog.h"
#include "XME_search.h"
#include "XYA_error.h"
#include "XMV_view.h"
#include "XMF_file.h"

Widget          XyagFileSelectDialog = NULL;

static int      XyagLockLoop = 0;

static void     CallbackConfirmQuitOk();
static void     CallbackZoomSetOk();
static void     CallbackFindOk();
static void     CallbackDepthSetOk();
static void     CallbackDepthSetToggle();

static void
simple_callback(dialog, client_data, cbs)
    Widget          dialog;
    XtPointer       client_data;
    XmAnyCallbackStruct *cbs;
{
    XtUnmanageChild(dialog);
}

/*------------------------------------------------------------\
|                 XyagDestroyDialogCallback                  |
\------------------------------------------------------------*/

void
XyagDestroyDialogCallback(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    XyagLockLoop = 0;
    XyagExitErrorMessage(1);
    XyagExit(1);
}

/*------------------------------------------------------------\
|                   XyagExitDialogCallback                   |
\------------------------------------------------------------*/

void
XyagExitDialogCallback()
{
    XyagLockLoop = 0;
}

/*------------------------------------------------------------\
|                       XyagLimitedLoop                       |
\------------------------------------------------------------*/

void
XyagLimitedLoop(MyWidget)
    Widget          MyWidget;
{
    Widget          DialogShell, WShell;
    XEvent          Event;
    XAnyEvent      *AnyEvent;
    XtAppContext    Context;

    Context = XtWidgetToApplicationContext(MyWidget);

    XyagLockLoop = 1;

//    XyagSetMouseCursor(XtParent(MyWidget),XYAG_PIRATE_CURSOR);

    for (WShell = MyWidget; !XtIsShell(WShell); WShell = XtParent(WShell));

    while (XyagLockLoop == 1) {
        XtAppNextEvent(Context, &Event);

        AnyEvent = (XAnyEvent *) (&Event);

        for (DialogShell = XtWindowToWidget(AnyEvent->display, AnyEvent->window);
             ((DialogShell != NULL) && (!XtIsShell(DialogShell)));
             DialogShell = XtParent(DialogShell));

        switch (AnyEvent->type) {
        case KeyRelease:
        case ButtonRelease:
            if (WShell == DialogShell) {
                XtDispatchEvent(&Event);
            }
            break;

        case KeyPress:
        case ButtonPress:
            if (WShell != DialogShell) {
                //AnyEvent->window = XtWindow(WShell);
                XtPopup( WShell, XtGrabNone ); 
            }
            else
            {
                XtDispatchEvent( &Event );
            }
            break;

        default:
            XtDispatchEvent(&Event);
        }
    }

XyagLockLoop = 1;

XyagSetMouseCursor( MyWidget, XYAG_NORMAL_CURSOR ); 

}
/*------------------------------------------------------------\
|                      XyagLeaveLimitedLoop                   |
\------------------------------------------------------------*/
void XyagLeaveLimitedLoop()
{
    XyagLockLoop = 0;
}


/*------------------------------------------------------------\
|                      XyagWarningMessage                    |
\------------------------------------------------------------*/

void
XyagWarningMessage(Father, Subject)
    Widget          Father;
    char           *Subject;
{
    static Widget   dialog;
    XmString        label, title;
    Arg             args[9];
    int             n = 0;
    char            buffer[256];

    label = XmStringCreateSimple(Subject);

    if (!dialog) {
        sprintf(buffer,"%s: Warning",XYAGLE_NAME);
        title = XmStringCreateSimple(buffer);
        XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL);
        n++;
        XtSetArg(args[n], XmNmessageString, label);
        n++;
        dialog = XmCreateWarningDialog(Father, "warningDialog", args, n);
        XtVaSetValues(dialog, XmNdialogTitle, title, NULL);
        XtAddCallback(dialog, XmNokCallback, simple_callback, NULL);
        XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
        XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));
    }
    else {
        XtVaSetValues(dialog, XmNmessageString, label, NULL);
    }

    XmStringFree(label);
    XtManageChild(dialog);
}

/*------------------------------------------------------------\
|                      XyagErrorMessage                      |
\------------------------------------------------------------*/

void
XyagErrorMessage(Father, Subject)
    Widget          Father;
    char           *Subject;
{
    static Widget   errorDialog;
    XmString        label, title;
    Arg             args[9];
    int             n = 0;
    char            buffer[256];

    label = XmStringCreateSimple(Subject);

    if (!errorDialog) {
        sprintf(buffer,"%s: Error",XYAGLE_NAME);
        title = XmStringCreateSimple(buffer);
        XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL);
        n++;
        XtSetArg(args[n], XmNmessageString, label);
        n++;
        errorDialog = XmCreateErrorDialog(Father, "errorDialog", args, n);
        XtVaSetValues(errorDialog, XmNdialogTitle, title, NULL);
        XtAddCallback(errorDialog, XmNokCallback, simple_callback, NULL);
        XtUnmanageChild(XmMessageBoxGetChild(errorDialog, XmDIALOG_CANCEL_BUTTON));
        XtUnmanageChild(XmMessageBoxGetChild(errorDialog, XmDIALOG_HELP_BUTTON));
    }
    else {
        XtVaSetValues(errorDialog, XmNmessageString, label, NULL);
    }

    XmStringFree(label);
    XtManageChild(errorDialog);
}

/*------------------------------------------------------------\
|                      XyagWorkingDialog                      |
\------------------------------------------------------------*/

Widget
XyagWorkingDialog(Father, Subject)
    Widget          Father;
    char           *Subject;
{
    static Widget   workingDialog;
    XmString        label, title;
    Arg             args[9];
    int             n = 0;
    char            buffer[256];

    label = XmStringCreateSimple(Subject);

    if (!workingDialog) {
        sprintf(buffer,"%s: Working",XYAGLE_NAME);
        title = XmStringCreateSimple(buffer);
        XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL);
        n++;
        XtSetArg(args[n], XmNmessageString, label);
        n++;
        workingDialog = XmCreateWorkingDialog(Father, "workingDialog", args, n);
        XtVaSetValues(workingDialog, XmNdialogTitle, title, NULL);
        XtAddCallback(workingDialog, XmNcancelCallback, simple_callback, NULL);
        XtUnmanageChild(XmMessageBoxGetChild(workingDialog, XmDIALOG_OK_BUTTON));
        XtUnmanageChild(XmMessageBoxGetChild(workingDialog, XmDIALOG_HELP_BUTTON));
    }
    else {
        XtVaSetValues(workingDialog, XmNmessageString, label, NULL);
    }

    XmStringFree(label);
    XtManageChild(workingDialog);
    return workingDialog;
}


/*------------------------------------------------------------\
|                      XyagConfirmQuit                       |
\------------------------------------------------------------*/

void
XyagConfirmQuit()
{
    static Widget   quitDialog;
    XmString        label, title, yes, no;
    Arg             args[9];
    int             n = 0;
    char            buffer[256];


    if (!quitDialog) {
        sprintf(buffer,"%s: Confirm",XYAGLE_NAME);
        title = XmStringCreateSimple(buffer);
        sprintf(buffer,"Do you really want to quit %s?",XYAGLE_NAME);
        label = XmStringCreateSimple(buffer);
        yes = XmStringCreateSimple("YES");
        no = XmStringCreateSimple("NO");
        XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
        XtSetArg(args[n], XmNmessageString, label); n++;
        XtSetArg(args[n], XmNokLabelString, yes); n++;
        XtSetArg(args[n], XmNcancelLabelString, no); n++;
        quitDialog = XmCreateQuestionDialog(XyagMainWindow, "confirmQuitDialog", args, n);
        XtVaSetValues(quitDialog, XmNdialogTitle, title, NULL);
        XtAddCallback(quitDialog, XmNokCallback, CallbackConfirmQuitOk, NULL);
        XtAddCallback(quitDialog, XmNcancelCallback, simple_callback, NULL);
        XtUnmanageChild(XmMessageBoxGetChild(quitDialog, XmDIALOG_HELP_BUTTON));
    }

    XtManageChild(quitDialog);
}


/*------------------------------------------------------------\
|                  CallbackConfirmQuitOk                     |
\------------------------------------------------------------*/

static void
CallbackConfirmQuitOk(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    XtUnmanageChild(MyWidget);
    XtCloseDisplay(XtDisplay(XtParent(MyWidget)));
    XyagExitErrorMessage(0);
    XyagExit(0);
}

/*------------------------------------------------------------\
|                      XyagZoomSet                            |
\------------------------------------------------------------*/

void
XyagZoomSet()
{
    static Widget   zoomSetDialog;
    XmString        label, title;
    Arg             args[9];
    int             n = 0;
    char            buffer[256];


    if (!zoomSetDialog) {
        sprintf(buffer,"%s: Prompt",XYAGLE_NAME);
        title = XmStringCreateSimple(buffer);
        label = XmStringCreateSimple("Enter the zoom percentage:");
        XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
        XtSetArg(args[n], XmNselectionLabelString, label); n++;
        XtSetArg(args[n], XmNautoUnmanage, False); n++;
        zoomSetDialog = XmCreatePromptDialog(XyagMainWindow, "zoomSetDialog", args, n);
        XtVaSetValues(zoomSetDialog, XmNdialogTitle, title, NULL);
        XtAddCallback(zoomSetDialog, XmNokCallback, CallbackZoomSetOk, NULL);
        XtAddCallback(zoomSetDialog, XmNcancelCallback, simple_callback, NULL);
        XtUnmanageChild(XmSelectionBoxGetChild(zoomSetDialog, XmDIALOG_HELP_BUTTON));
    }

    XtManageChild(zoomSetDialog);
}

/*------------------------------------------------------------\
|                      CallbackZoomSetOk                      |
\------------------------------------------------------------*/

static void
CallbackZoomSetOk(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    XmSelectionBoxCallbackStruct *CallData;
{
    char           *ZoomSet;
    int             ZoomPercent;

    XmStringGetLtoR(CallData->value, XmSTRING_DEFAULT_CHARSET, &ZoomSet);

    if (ZoomSet != NULL) {
        ZoomPercent = atoi(ZoomSet);

        if ((ZoomPercent < XYAG_PERCENT_ZOOM_MIN) || (ZoomPercent > XYAG_PERCENT_ZOOM_MAX)) {
            XyagErrorMessage(XyagMainWindow, "The value must be between 5% and 95%");
        }
        else {
            XyagPercentZoom = ZoomPercent;
            XtUnmanageChild(MyWidget);
        }
    }
}

/*------------------------------------------------------------\
|                      XyagDepthSet                           |
\------------------------------------------------------------*/

void
XyagDepthSet()
{
    static Widget   depthSetDialog;
    static Widget   depthSetToggle;
    XmString        label, title;
    XmString        btn1, btn2, btn3;
    Arg             args[9];
    int             n = 0;
    char            buffer[256];

    if (!depthSetDialog) {
        sprintf(buffer,"%s: Prompt",XYAGLE_NAME);
        title = XmStringCreateSimple(buffer);
        label = XmStringCreateSimple("Enter depth for Hilite/Extract:");
        XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
        XtSetArg(args[n], XmNselectionLabelString, label); n++;
        XtSetArg(args[n], XmNautoUnmanage, False); n++;
        depthSetDialog = XmCreatePromptDialog(XyagMainWindow, "depthSetDialog", args, n);
        XtVaSetValues(depthSetDialog, XmNdialogTitle, title, NULL);
        XtAddCallback(depthSetDialog, XmNokCallback, CallbackDepthSetOk, NULL);
        XtAddCallback(depthSetDialog, XmNcancelCallback, simple_callback, NULL);
        XtUnmanageChild(XmSelectionBoxGetChild(depthSetDialog, XmDIALOG_HELP_BUTTON));

        btn1 = XmStringCreateSimple("Backwards");
        btn2 = XmStringCreateSimple("Forwards");
        btn3 = XmStringCreateSimple("Both Ways");
        depthSetToggle = XmVaCreateSimpleRadioBox(depthSetDialog, "depthSetToggle", 0, CallbackDepthSetToggle,
                                                  XmVaRADIOBUTTON, btn1, 0, NULL, NULL,
                                                  XmVaRADIOBUTTON, btn2, 0, NULL, NULL,
                                                  XmVaRADIOBUTTON, btn3, 0, NULL, NULL,
                                                  NULL);
        XtManageChild(depthSetToggle);
    }

    XtManageChild(depthSetDialog);
}

/*------------------------------------------------------------\
|                      CallbackDepthSetToggle                 |
\------------------------------------------------------------*/

static void
CallbackDepthSetToggle(MyWidget, ClientData, CallData)
    Widget                         MyWidget;
    caddr_t                        ClientData;
    XmToggleButtonCallbackStruct  *CallData;
{
    if (CallData->set == False) return;
    switch ((int)ClientData) {
    case 0:
        XyagDependencyMode = XYAG_BACKWARD;
        break;
    case 1:
        XyagDependencyMode = XYAG_FORWARD;
        break;
    case 2:
        XyagDependencyMode = XYAG_BOTH;
        break;
    }
}

/*------------------------------------------------------------\
|                      CallbackDepthSetOk                     |
\------------------------------------------------------------*/

static void
CallbackDepthSetOk(MyWidget, ClientData, CallData)
    Widget                         MyWidget;
    caddr_t                        ClientData;
    XmSelectionBoxCallbackStruct  *CallData;
{
    char *DepthSet;
    int   DepthOfDep;

    XmStringGetLtoR(CallData->value, XmSTRING_DEFAULT_CHARSET, &DepthSet);


    if (DepthSet != NULL) {
        if (!strcmp(DepthSet, "X\0")) {
            XyagDepthOfDep = -2;
            XtUnmanageChild(MyWidget);
        }
        else {
            DepthOfDep = strtol(DepthSet, NULL, 10);
            if (DepthOfDep <= 0) {
                XyagErrorMessage(XyagMainWindow, "The Value must be strictly positive (or 'X' for MAX Depth)!");
            }
            else {
                XyagDepthOfDep = DepthOfDep;
                XtUnmanageChild(MyWidget);
            }
        }
    }
    else {
        XyagErrorMessage(XyagMainWindow, "The Value must be strictly positive (or 'X' for MAX Depth)!");
    }
}

/*------------------------------------------------------------\
|                      XyagFind                               |
\------------------------------------------------------------*/

void
XyagFind()
{
    static Widget   findDialog;
    XmString        label, title;
    Arg             args[9];
    int             n = 0;
    char            buffer[256];


    if (!findDialog) {
        sprintf(buffer,"%s: Prompt",XYAGLE_NAME);
        title = XmStringCreateSimple(buffer);
        label = XmStringCreateSimple("Enter gate or connector name:");
        XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
        XtSetArg(args[n], XmNselectionLabelString, label); n++;
        XtSetArg(args[n], XmNautoUnmanage, False); n++;
        findDialog = XmCreatePromptDialog(XyagMainWindow, "findDialog", args, n);
        XtVaSetValues(findDialog, XmNdialogTitle, title, NULL);
        XtAddCallback(findDialog, XmNokCallback, CallbackFindOk, NULL);
        XtAddCallback(findDialog, XmNcancelCallback, simple_callback, NULL);
        XtUnmanageChild(XmSelectionBoxGetChild(findDialog, XmDIALOG_HELP_BUTTON));
    }

    XtManageChild(findDialog);
}

/*------------------------------------------------------------\
|                      CallbackFindOk                         |
\------------------------------------------------------------*/

static void 
CallbackFindOk(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    XmSelectionBoxCallbackStruct *CallData;
{
    char           *NameSet;

    XmStringGetLtoR(CallData->value, XmSTRING_DEFAULT_CHARSET, &NameSet);
    XtUnmanageChild(MyWidget);

    NameSet = XyagPostTreatString(NameSet);

    if (NameSet != (char *) NULL) {
        NameSet = namealloc(NameSet);
        XyagEditSearchObject(NameSet);
    }
}

/*------------------------------------------------------------\
|                      XyagFileSelect                         |
\------------------------------------------------------------*/

void
XyagFileSelect(Filter, labelstr, callbackOk, labelExtraStr, callbackExtra)
    XmString        Filter;
    char           *labelstr;
    void            callbackOk();
    char           *labelExtraStr;
    void            callbackExtra();
{
    static Widget   fileSelectDialog;
    XmString        label, title, labelExtra;
    Arg             args[9];
    int             n = 0;
    char            buffer[256];


    if (!fileSelectDialog) {
        sprintf(buffer,"%s: File Select",XYAGLE_NAME);
        title = XmStringCreateSimple(buffer);
        label = XmStringCreateSimple(labelstr);
        XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
        XtSetArg(args[n], XmNselectionLabelString, label); n++;
        XtSetArg(args[n], XmNautoUnmanage, False); n++;
        fileSelectDialog = XmCreateFileSelectionDialog(XyagMainWindow, "fileSelectDialog", args, n);
        XtVaSetValues(fileSelectDialog, XmNdialogTitle, title, NULL);
        XtAddCallback(fileSelectDialog, XmNokCallback, callbackOk, NULL);
        XtAddCallback(fileSelectDialog, XmNcancelCallback, simple_callback, NULL);
        XtUnmanageChild(XmFileSelectionBoxGetChild(fileSelectDialog, XmDIALOG_FILTER_LABEL));
        XtUnmanageChild(XmFileSelectionBoxGetChild(fileSelectDialog, XmDIALOG_FILTER_TEXT));
        if (callbackExtra != NULL) {
            XtAddCallback(fileSelectDialog, XmNhelpCallback, callbackExtra, NULL);
        }
        else {
            XtUnmanageChild(XmSelectionBoxGetChild(fileSelectDialog, XmDIALOG_HELP_BUTTON));
        }
        if (labelExtraStr != NULL) {
            labelExtra = XmStringCreateSimple(labelExtraStr);
            XtVaSetValues(fileSelectDialog, XmNhelpLabelString, labelExtra, NULL);
            XmStringFree(labelExtra);
        }
    }
    else {
        XtRemoveAllCallbacks(fileSelectDialog, XmNokCallback);
        XtAddCallback(fileSelectDialog, XmNokCallback, callbackOk, NULL);
        XtRemoveAllCallbacks(fileSelectDialog, XmNhelpCallback);
        if (callbackExtra != NULL) {
            XtManageChild(XmSelectionBoxGetChild(fileSelectDialog, XmDIALOG_HELP_BUTTON));
            XtAddCallback(fileSelectDialog, XmNhelpCallback, callbackExtra, NULL);
        }
        else {
            XtUnmanageChild(XmSelectionBoxGetChild(fileSelectDialog, XmDIALOG_HELP_BUTTON));
        }
        if (labelExtraStr != NULL) {
            labelExtra = XmStringCreateSimple(labelExtraStr);
            XtVaSetValues(fileSelectDialog, XmNhelpLabelString, labelExtra, NULL);
            XmStringFree(labelExtra);
        }
    }
    XtVaSetValues(fileSelectDialog, XmNpattern, Filter, NULL);

    XtManageChild(fileSelectDialog);
}


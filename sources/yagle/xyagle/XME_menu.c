/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                            |
|                                                             |
| File    :                   Menu.c                          |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                        Lester Anthony                       |
|                        Picault Stephane                     |
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
#include XME_H
#include XMX_H

#include "XME_menu.h"
#include "XMT_menu.h"
#include "XMT_panel.h"
#include "XME_edit.h"

int     XyagDependencyMode = 0;

/*------------------------------------------------------------\
|                     EDIT menu definition                    |
\------------------------------------------------------------*/

XyagMenuItem    XyagEditMenu[] =
{
    {
        "Select",
        'S',
        "Ctrl<Key>S",
        "Ctrl+S",
        &xmPushButtonGadgetClass,
        False,
        False,
        False,
        False,
        CallbackEditSelect,
        (XtPointer) NULL,
        (Widget) NULL,
        (Widget) NULL,
        (XyagMenuItem *) NULL
    }
    ,
    {"line",0,NULL,NULL,NULL,False,True,False,False,NULL,NULL,NULL,NULL,NULL}
    ,
    {
        "Extract",
        'E',
        "Ctrl<Key>E",
        "Ctrl+E",
        &xmPushButtonGadgetClass,
        False,
        False,
        False,
        False,
        CallbackExtract,
        (XtPointer) NULL,
        (Widget) NULL,
        (Widget) NULL,
        (XyagMenuItem *) NULL
    }
    ,
    {
        "Hilite",
        'H',
        "Ctrl<Key>H",
        "Ctrl+H",
        &xmPushButtonGadgetClass,
        False,
        False,
        False,
        False,
        CallbackHilite,
        (XtPointer) NULL,
        (Widget) NULL,
        (Widget) NULL,
        (XyagMenuItem *) NULL
    }
    ,
    {
      "Go thru hierarchy",
      'G',
      "Ctrl<Key>G",
      "Ctrl+G",
      &xmPushButtonGadgetClass,
      False,
      False,
      False,
      False,
      CallbackGoThru,
      (XtPointer) NULL,
      (Widget) NULL,
      (Widget) NULL,
      (XyagMenuItem *) NULL
    }
    ,
    {"line",0,NULL,NULL,NULL,False,True,False,False,NULL,NULL,NULL,NULL,NULL}
    ,
    {
        "Set Depth...",
        'D',
        NULL,
        NULL,
        &xmPushButtonGadgetClass,
        False,
        False,
        False,
        False,
        CallbackSetDepth,
        (XtPointer) NULL,
        (Widget) NULL,
        (Widget) NULL,
        (XyagMenuItem *) NULL
    }
    ,
    {"line",0,NULL,NULL,NULL,False,True,False,False,NULL,NULL,NULL,NULL,NULL}
    ,
    {
        "Back",
        'B',
        "",
        "Backspace",
        &xmPushButtonGadgetClass,
        False,
        False,
        False,
        False,
        CallbackPreviousExtract,
        (XtPointer) NULL,
        (Widget) NULL,
        (Widget) NULL,
        (XyagMenuItem *) NULL
    }
    ,
    {
        "Full Figure",
        'u',
        NULL,
        NULL,
        &xmPushButtonGadgetClass,
        False,
        False,
        False,
        False,
        CallbackFullExtract,
        (XtPointer) NULL,
        (Widget) NULL,
        (Widget) NULL,
        (XyagMenuItem *) NULL
    }
    ,
    {"line",0,NULL,NULL,NULL,False,True,False,False,NULL,NULL,NULL,NULL,NULL}
    ,
    {
        "Find...",
        'F',
        "Ctrl<Key>F",
        "Ctrl+F",
        &xmPushButtonGadgetClass,
        False,
        False,
        False,
        False,
        CallbackEditSearch,
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
|                     Callbacks for Hilite                    |
\------------------------------------------------------------*/

void 
CallbackHilite(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    switch (XyagDependencyMode) {
    case XYAG_BACKWARD:
        XyagChangeEditMode(XYAG_EDIT_BACKWARDHILITE);
        break;
    case XYAG_FORWARD:
        XyagChangeEditMode(XYAG_EDIT_FORWARDHILITE);
        break;
    case XYAG_BOTH:
        XyagChangeEditMode(XYAG_EDIT_BOTHHILITE);
        break;
    }
}

void 
CallbackGoThru(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
  XyagChangeEditMode(XYAG_EDIT_TREE);
}

void 
CallbackSetDepth(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    XyagDepthSet();
}

/*------------------------------------------------------------\
|                     Callbacks for Extract                   |
\------------------------------------------------------------*/

void 
CallbackExtract(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    switch (XyagDependencyMode) {
    case XYAG_BACKWARD:
        XyagChangeEditMode(XYAG_EDIT_BACKWARDEXTRACT);
        break;
    case XYAG_FORWARD:
        XyagChangeEditMode(XYAG_EDIT_FORWARDEXTRACT);
        break;
    case XYAG_BOTH:
        XyagChangeEditMode(XYAG_EDIT_BOTHEXTRACT);
        break;
    }
}

void 
CallbackFullExtract(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
/*    if (XyagCnsMode) {
        if (XyagFigureCgv == NULL)
            return;
        if (XyagFigureCgv->NEXT == NULL)
            return;

        while (XyagFigureCgv->NEXT != NULL) {
            XyagFigureCgv = rmvcgvfig(XyagFigureCgv);
        }

        XyagRecomputeBound = XYAG_TRUE;

        if (XyagFigureCgv != (cgvfig_list *) NULL) {
            XyagAddFigure();
        }
        XyagInitializeZoom();
        XyagDisplayFigure(0, 0, XyagGraphicDx, XyagGraphicDy);

        XyagRefreshGraphicWindow(0, 0, XyagGraphicDx, XyagGraphicDy);

        XyagSetMouseCursor(XyagGraphicWindow, XYAG_NORMAL_CURSOR);
    }
    else {
        XyagWarningMessage(XyagMainWindow, "Works only with CNS");
    }
*/
  if (OPEN_STACK==NULL)
    XyagWarningMessage(XyagMainWindow, "No First Figure !");
  else if (OPEN_STACK->NEXT==NULL)
    XyagWarningMessage(XyagMainWindow, "This is already the first Figure !");
  else
    {
      chain_list *cl=OPEN_STACK, *prev=NULL;
      while (cl->NEXT!=NULL) { prev=cl; cl=cl->NEXT;}
      
      prev->NEXT=NULL;
      freechain(prev);
      OPEN_STACK=cl;
      XyagRecomputeBound = XYAG_TRUE;

      xyagSaveState();
      XyagFigureCgv = (cgvfig_list *)OPEN_STACK->DATA;
      xyagRetreiveState();
      XyagChangeTopLevelTitle(XyagFigureCgv->NAME);
//      XyagInitializeZoom();

      XyagDisplayFigure(0, 0, XyagGraphicDx, XyagGraphicDy);
      
      XyagRefreshGraphicWindow(0, 0, XyagGraphicDx, XyagGraphicDy);
      
      XyagSetMouseCursor(XyagGraphicWindow, XYAG_NORMAL_CURSOR);
      XyagZoomRefresh();
    }
}

void 
CallbackPreviousExtract(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
/*    if (XyagCnsMode) {
        XyagRecomputeBound = XYAG_TRUE;

        if (XyagFigureCgv != NULL && XyagFigureCgv->NEXT != NULL) {
            XyagFigureCgv = rmvcgvfig(XyagFigureCgv);

            if (XyagFigureCgv != NULL) {
                XyagAddFigure();
            }
            XyagInitializeZoom();
            XyagDisplayFigure(0, 0, XyagGraphicDx, XyagGraphicDy);

            XyagRefreshGraphicWindow(0, 0, XyagGraphicDx, XyagGraphicDy);

            XyagSetMouseCursor(XyagGraphicWindow, XYAG_NORMAL_CURSOR);
        }
        else {
            XyagWarningMessage(XyagMainWindow, "No Previous Figure !");
        }
    }
    else {
        XyagWarningMessage(XyagMainWindow, "Works only with CNS");
    }
*/
  if (OPEN_STACK==NULL || OPEN_STACK->NEXT==NULL)
    XyagWarningMessage(XyagMainWindow, "No Previous Figure !");
  else
    {
      chain_list *cl=OPEN_STACK;
      XyagRecomputeBound = XYAG_TRUE;
      OPEN_STACK=OPEN_STACK->NEXT;
      cl->NEXT=NULL;
      freechain(cl);
      xyagSaveState();
      XyagFigureCgv = (cgvfig_list *)OPEN_STACK->DATA;
      xyagRetreiveState();
      XyagChangeTopLevelTitle(XyagFigureCgv->NAME);
//      XyagInitializeZoom();

      XyagDisplayFigure(0, 0, XyagGraphicDx, XyagGraphicDy);
      
      XyagRefreshGraphicWindow(0, 0, XyagGraphicDx, XyagGraphicDy);
      
      XyagSetMouseCursor(XyagGraphicWindow, XYAG_NORMAL_CURSOR);
      XyagZoomRefresh();
    }
}

/*------------------------------------------------------------\
|                     CallbackEditIdentify                    |
\------------------------------------------------------------*/

void 
CallbackEditIdentify(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    XyagChangeEditMode(XYAG_EDIT_IDENTIFY);
}

/*------------------------------------------------------------\
|                    CallbackEditBehaviour                    |
\------------------------------------------------------------*/

void 
CallbackEditBehaviour(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    XyagChangeEditMode(XYAG_EDIT_BEHAVIOUR);
}

/*------------------------------------------------------------\
|                     CallbackEditSelect                   |
\------------------------------------------------------------*/

void 
CallbackEditSelect(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    XyagChangeEditMode(XYAG_EDIT_SELECT);
}

/*------------------------------------------------------------\
|                     CallbackEditSearch                      |
\------------------------------------------------------------*/

void 
CallbackEditSearch(MyWidget, ClientData, CallData)
    Widget          MyWidget;
    caddr_t         ClientData;
    caddr_t         CallData;
{
    XyagFind();
}

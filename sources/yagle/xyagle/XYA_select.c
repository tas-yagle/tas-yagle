/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                            |
|                                                             |
| File    :                 Select.c                          |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                        Lester Anthony                       |
|                                                             |
\------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <setjmp.h>

#include MUT_H
#include SLB_H
#include MLO_H
#include CGV_H
#include XSB_H
#include XYA_H

#include "XMV_view.h"
#include "XYA_select.h"

xyagselect_list *XyagHeadSelect = NULL;
xyagselect_list *XyagHeadConnect = NULL;

/*------------------------------------------------------------\
|                         Alloc Functions                     |
\------------------------------------------------------------*/

xyagselect_list *
XyagAllocSelect()
{
    xyagselect_list  *select;

    select = (xyagselect_list *)mbkalloc(sizeof(xyagselect_list));
    select->NEXT = NULL;
    select->OBJECT = NULL;
    return select;
}

/*------------------------------------------------------------\
|                         Free Functions                      |
\------------------------------------------------------------*/

void 
XyagFreeSelect(Select)
    xyagselect_list *Select;
{
    mbkfree(Select);
}

/*------------------------------------------------------------\
|                          XyagSelectObject                   |
\------------------------------------------------------------*/

void 
XyagSelectObject(Object)
    xyagobj_list   *Object;
{
    xyagobj_list   *Scan;

    Scan = Object;
    do {
        SetXyagSelect(Scan);
        Scan = Scan->LINK;
    }
    while (Scan != Object);
}

/*------------------------------------------------------------\
|                          XyagUnselectObject                 |
\------------------------------------------------------------*/

void 
XyagUnselectObject(Object)
    xyagobj_list   *Object;
{
    xyagobj_list   *Scan;

    Scan = Object;
    do {
        ClearXyagSelect(Scan);
        Scan = Scan->LINK;
    }
    while (Scan != Object);
}

/*------------------------------------------------------------\
|                          XyagAcceptObject                   |
\------------------------------------------------------------*/

void 
XyagAcceptObject(Object)
    xyagobj_list   *Object;
{
    xyagobj_list   *Scan;

    Scan = Object;
    do {
        SetXyagAccept(Scan);
        Scan = Scan->LINK;
    }
    while (Scan != Object);
}

/*------------------------------------------------------------\
|                          XyagRejectObject                   |
\------------------------------------------------------------*/

void 
XyagRejectObject(Object)
    xyagobj_list   *Object;
{
    xyagobj_list   *Scan;

    Scan = Object;
    do {
        ClearXyagAccept(Scan);
        Scan = Scan->LINK;
    }
    while (Scan != Object);
}

/*------------------------------------------------------------\
|                          XyagConnectObject                  |
\------------------------------------------------------------*/

void 
XyagConnectObject(Object)
    xyagobj_list   *Object;
{
    xyagobj_list   *Scan;

    Scan = Object;
    do {
        SetXyagConnect(Scan);
        Scan = Scan->LINK;
    }
    while (Scan != Object);
}

/*------------------------------------------------------------\
|                         XyagDisconnectObject                |
\------------------------------------------------------------*/

void 
XyagDisconnectObject(Object)
    xyagobj_list   *Object;
{
    xyagobj_list   *Scan;

    Scan = Object;
    do {
        ClearXyagConnect(Scan);
        Scan = Scan->LINK;
    }
    while (Scan != Object);
}

/*------------------------------------------------------------\
|                          XyagAddSelect                      |
\------------------------------------------------------------*/

void 
XyagAddSelect(Object)
    xyagobj_list   *Object;
{
    xyagselect_list *Select;

    Select = XyagAllocSelect();
    Select->NEXT = XyagHeadSelect;
    Select->OBJECT = Object;
    XyagHeadSelect = Select;

    XyagSelectObject(Object);
}

/*------------------------------------------------------------\
|                          XyagAddConnect                     |
\------------------------------------------------------------*/

void 
XyagAddConnect(Object)
    xyagobj_list   *Object;
{
    xyagselect_list *Connect;

    Connect = XyagAllocSelect();
    Connect->NEXT = XyagHeadConnect;
    Connect->OBJECT = Object;
    XyagHeadConnect = Connect;

    XyagConnectObject(Object);
}

/*------------------------------------------------------------\
|                         XyagDelSelect                       |
\------------------------------------------------------------*/

void 
XyagDelSelect()
{
    xyagselect_list *Select;
    xyagselect_list *DelSelect;

    Select = XyagHeadSelect;
    XyagHeadSelect = (xyagselect_list *) NULL;

    while (Select != (xyagselect_list *) NULL) {
        XyagRejectObject(Select->OBJECT);
        DelSelect = Select;
        Select = Select->NEXT;
        XyagFreeSelect(DelSelect);
    }
}

/*------------------------------------------------------------\
|                        XyagPurgeSelect                      |
\------------------------------------------------------------*/

void 
XyagPurgeSelect()
{
    xyagselect_list *DelSelect;
    xyagselect_list *Select;
    xyagselect_list **Previous;

    Previous = &XyagHeadSelect;
    Select = XyagHeadSelect;

    while (Select != (xyagselect_list *) NULL) {
        if (!IsXyagAccept(Select->OBJECT)) {
            DelSelect = Select;
            Select = Select->NEXT;
            *Previous = Select;
            XyagFreeSelect(DelSelect);
        }
        else {
            Previous = &Select->NEXT;
            Select = Select->NEXT;
        }
    }
}

/*------------------------------------------------------------\
|                          XyagDelConnect                     |
\------------------------------------------------------------*/

void 
XyagDelConnect()
{
    xyagselect_list *Connect;
    xyagselect_list *DelConnect;

    Connect = XyagHeadConnect;
    XyagHeadConnect = (xyagselect_list *) NULL;

    while (Connect != (xyagselect_list *) NULL) {
        XyagDisconnectObject(Connect->OBJECT);
        DelConnect = Connect;
        Connect = Connect->NEXT;
        XyagFreeSelect(DelConnect);
    }
}

void xyagSaveState()
{
  if (XyagFigureCgv!=NULL)
    {
      XyagFigureCgv->data0=XyagHeadConnect;
      XyagFigureCgv->data1=XyagHeadSelect;
      XyagFigureCgv->data2=XyagFigure;
      XyagFigureCgv->data_ZOOM=XyagZoomSave();
    }
  XyagHeadConnect=XyagHeadSelect=NULL;
}

void xyagRetreiveState()
{
  if (XyagFigureCgv!=NULL)
    {
      XyagHeadConnect=XyagFigureCgv->data0;
      XyagHeadSelect=XyagFigureCgv->data1;
      XyagFigure=XyagFigureCgv->data2;
      XyagZoomRestore(XyagFigureCgv->data_ZOOM);
    }
  else
    {
      XyagHeadConnect=XyagHeadSelect=NULL; 
      XyagFigure=NULL;
      XyagZoomRestore(NULL);
    }
}

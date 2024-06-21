/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                            |
|                                                             |
| File    :                  Search.c                         |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                        Lester Anthony                       |
|                                                             |
\------------------------------------------------------------*/

#include <stdio.h>
#include <math.h>
#include <Xm/Xm.h>
#include <Xm/PushBG.h>

#include MUT_H
#include MLO_H
#include CGV_H
#include XSB_H
#include XTB_H
#include XMX_H
#include XYA_H
#include XME_H
#include XMV_H

#include "XME_search.h"

int             XyagSearchNumber;
char           *XyagSearchString;

/*------------------------------------------------------------\
|                    XyagEditInitializeSearch                 |
\------------------------------------------------------------*/

void 
XyagEditInitializeSearch(Name)
    char           *Name;
{
    char           *Star;

    Star = strrchr(Name, '*');

    if (Star != (char *) NULL) {
        XyagSearchNumber = (int) (Star - Name);
    }
    else {
        XyagSearchNumber = 0;
    }

    XyagSearchString = Name;
}

/*------------------------------------------------------------\
|                    XyagEditSearchCompare                    |
\------------------------------------------------------------*/

int 
XyagEditSearchCompare(Name)
    char           *Name;
{
    if (Name != (char *) NULL) {
        if (XyagSearchNumber) {
            return (!strncmp(XyagSearchString, Name, XyagSearchNumber));
        }
        else {
            return (Name == XyagSearchString);
        }
    }

    return 0;
}

/*------------------------------------------------------------\
|                       XyagEditSearchObject                  |
\------------------------------------------------------------*/

void 
XyagEditSearchObject(ObjectName)
    char           *ObjectName;
{
    xyagobj_list   *Obj;
    xyagselect_list *Search;
    short           Layer;
    long            X1;
    long            Y1;

    if (XyagFigure == NULL) return;

    XyagEditInitializeSearch(ObjectName);

    for (Layer = 0; Layer < XYAG_MAX_LAYER; Layer++) {
        if (XYAG_ACTIVE_LAYER_TABLE[Layer] == 0) continue;

        for (Obj = XyagFigure->OBJECT[Layer]; Obj != NULL; Obj = Obj->NEXT) {
            if ((Obj->NAME != (char *) 0) && (!IsXyagSelect(Obj))) {
                if (XyagEditSearchCompare(Obj->NAME)) {
                    XyagAddSelect(Obj);
                }
            }
        }
    }

    for (Search = XyagHeadSelect; Search != NULL; Search = Search->NEXT) {
        XyagUnselectObject(Search->OBJECT);
    }

    if (XyagHeadSelect == NULL) {
        XyagWarningMessage(XyagMainWindow, "No object found !");
    }
    else {
      if (XyagHeadConnect != NULL) {
        XyagDelConnect();
        XyagZoomRefresh();
      }

      Obj = XyagHeadSelect->OBJECT;
      XyagAddConnect(Obj);
      X1 = (Obj->X + (Obj->DX >> 1)) / XYAG_UNIT;
      Y1 = (Obj->Y + (Obj->DY >> 1)) / XYAG_UNIT;
      XyagZoomCenter(X1, Y1);
      XyagDelSelect();
    }
}

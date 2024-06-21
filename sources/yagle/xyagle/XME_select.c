/*------------------------------------------------------------\
|                                                             |
| Tool    :                    XYAG                           |
|                                                             |
| File    :                   Edit.c                          |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                        Lester Anthony                       |
|                                                             |
\------------------------------------------------------------*/

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <Xm/Xm.h>
#include <Xm/PushBG.h>
#include <Xm/CascadeBG.h>

#include MUT_H
#include MLO_H
#include CGV_H
#include XSB_H
#include XTB_H
#include XMX_H
#include XYA_H
#include XME_H
#include XMV_H

#include "XME_select.h"
#include "XME_panel.h"

static char     XyagSelectBuffer[XYAG_SELECT_BUFFER_SIZE];

/*------------------------------------------------------------\
|                  XyagEditSelectObject                       |
\------------------------------------------------------------*/

void 
XyagEditSelectObject(Number)
    int             Number;
{
    xyagselect_list *Select;
    int             Counter;

    Select = XyagHeadSelect;

    for (Counter = 0; Counter < Number; Counter++) {
        Select = Select->NEXT;
    }

    if (IsXyagAccept(Select->OBJECT)) {
        XyagRejectObject(Select->OBJECT);
    }
    else {
        XyagAcceptObject(Select->OBJECT);
    }

    XyagDisplayObject(Select->OBJECT);
}

/*------------------------------------------------------------\
|                       Select Functions                      |
\------------------------------------------------------------*/
/*------------------------------------------------------------\
|                      XyagEditUnselectAll                    |
\------------------------------------------------------------*/

void 
XyagEditUnselectAll()
{
    xyagobj_list   *Obj;

    if (XyagHeadSelect != NULL) {
        if (XyagHeadSelect->NEXT == NULL) {
            Obj = XyagHeadSelect->OBJECT;
            XyagDelSelect();
            XyagDisplayObject(Obj);
        }
        else {
            XyagDelSelect();
            XyagZoomRefresh();
        }
    }
}

/*------------------------------------------------------------\
|                      XyagEditObjectNearPoint                |
\------------------------------------------------------------*/

char 
XyagEditObjectNearPoint(Obj, X1, Y1)
    xyagobj_list   *Obj;
    long            X1;
    long            Y1;
{
    short           Type;
    long            X1r;
    long            Y1r;
    long            X2r;
    long            Y2r;
    float           VectorX;
    float           VectorY;
    float           LineX;
    float           LineY;
    float           Norm;
    float           Distance;

    Type = GetXyagObjectType(Obj);

    if ((Obj->X <= X1) && (Obj->Y <= Y1) && (Obj->X + Obj->DX >= X1) && (Obj->Y + Obj->DY >= Y1)) {
        if ((Type == XYAG_OBJECT_LINE) || (Type == XYAG_OBJECT_ARROW)) {
            if ((Obj->DX == 0) && (Obj->DY == 0)) {
                if ((Obj->X == X1) && (Obj->Y == Y1)) {
                    return (XYAG_TRUE);
                }
            }
            else {
                if (IsXyagLineLeft(Obj)) {
                    X1r = Obj->X + Obj->DX;
                    X2r = Obj->X;
                }
                else {
                    X1r = Obj->X;
                    X2r = Obj->X + Obj->DX;
                }

                if (IsXyagLineDown(Obj)) {
                    Y1r = Obj->Y + Obj->DY;
                    Y2r = Obj->Y;
                }
                else {
                    Y1r = Obj->Y;
                    Y2r = Obj->Y + Obj->DY;
                }

                LineX = (float) (X2r - X1r);
                LineY = (float) (Y2r - Y1r);
                Norm = sqrt((LineX * LineX) + (LineY * LineY));
                LineX /= Norm;
                LineY /= Norm;

                VectorX = (float) (X2r - X1);
                VectorY = (float) (Y2r - Y1);

                Distance = fabs((VectorX * LineY) - (VectorY * LineX));

                if (Distance < (float) XYAG_UNIT) {
                    return (XYAG_TRUE);
                }
            }
        }
        else {
            return (XYAG_TRUE);
        }
    }

    return (XYAG_FALSE);
}

/*------------------------------------------------------------\
|                       XyagEditSelectPoint                   |
\------------------------------------------------------------*/

void 
XyagEditSelectPoint(X1, Y1)
    long            X1;
    long            Y1;
{
    xyagobj_list   *Obj;
    xyagselect_list *Select;
    int             Layer;

    X1 = X1 * XYAG_UNIT;
    Y1 = Y1 * XYAG_UNIT;

    for (Layer = 0; Layer < XYAG_MAX_LAYER; Layer++) {
        if (XYAG_ACTIVE_LAYER_TABLE[Layer] == 0)
            continue;

        for (Obj = XyagFigure->OBJECT[Layer]; Obj; Obj = Obj->NEXT) {
            if (XYAG_ACTIVE_LAYER_TABLE[Obj->LAYER] == 0) continue;

            if (!IsXyagSelect(Obj)) {
                if (XyagEditObjectNearPoint(Obj, X1, Y1)) {
                    XyagAddSelect(Obj);
                }
            }
        }
    }

    for (Select = XyagHeadSelect; Select; Select = Select->NEXT) {
        XyagUnselectObject(Select->OBJECT);
    }
}

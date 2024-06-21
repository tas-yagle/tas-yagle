/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                            |
|                                                             |
| File    :                  Bound.c                          |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                        Lester Anthony                       |
|                                                             |
\------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

#include MUT_H
#include SLB_H
#include MLO_H
#include CGV_H
#include XSB_H
#include XYA_H

#include "XYA_bound.h"

long            XyagBoundXmin = 0;
long            XyagBoundYmin = 0;
long            XyagBoundXmax = 0;
long            XyagBoundYmax = 0;

char            XyagRecomputeBound = XYAG_TRUE;

/*------------------------------------------------------------\
|                     XyagComputeBound                       |
\------------------------------------------------------------*/

char 
XyagComputeBound()
{
    xyagobj_list   *Obj;
    short           Layer;
    short           FirstBound;

    if (XyagFigure == (xyagfig_list *) NULL) {
        return (XYAG_FALSE);
    }

    if (XyagRecomputeBound == XYAG_FALSE) {
        return (XYAG_TRUE);
    }

    FirstBound = 1;

    for (Layer = 0; Layer < XYAG_MAX_LAYER; Layer++) {
        for (Obj = XyagFigure->OBJECT[Layer];
             Obj != (xyagobj_list *) NULL;
             Obj = Obj->NEXT) {
            if (FirstBound) {
                XyagBoundXmin = Obj->X;
                XyagBoundYmin = Obj->Y;
                XyagBoundXmax = Obj->X + Obj->DX;
                XyagBoundYmax = Obj->Y + Obj->DY;
                FirstBound = 0;
            }
            else {
                if (Obj->X < XyagBoundXmin) {
                    XyagBoundXmin = Obj->X;
                }
                if (Obj->Y < XyagBoundYmin) {
                    XyagBoundYmin = Obj->Y;
                }
                if ((Obj->X + Obj->DX) > XyagBoundXmax) {
                    XyagBoundXmax = Obj->X + Obj->DX;
                }
                if ((Obj->Y + Obj->DY) > XyagBoundYmax) {
                    XyagBoundYmax = Obj->Y + Obj->DY;
                }
            }
        }
    }

    XyagBoundXmin = XyagBoundXmin - (XYAG_UNIT << 1);
    XyagBoundYmin = XyagBoundYmin - (XYAG_UNIT << 1);
    XyagBoundXmax = XyagBoundXmax + (XYAG_UNIT << 1);
    XyagBoundYmax = XyagBoundYmax + (XYAG_UNIT << 1);

    XyagRecomputeBound = XYAG_FALSE;
    return (XYAG_TRUE);
}

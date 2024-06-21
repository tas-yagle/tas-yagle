/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                            |
|                                                             |
| File    :                   Grid.c                          |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                        Lester Anthony                       |
|                                                             |
\------------------------------------------------------------*/

#include <stdio.h>
#include <Xm/Xm.h>

#include MUT_H
#include MLO_H
#include XSB_H
#include CGV_H
#include XYA_H
#include XMX_H

#include "XMX_grid.h"

float           XyagUnitGridStep;

long            XyagUnitGridX;
long            XyagUnitGridY;
long            XyagUnitGridDx;
long            XyagUnitGridDy;

long            XyagPixelGridX;
long            XyagPixelGridY;

/*------------------------------------------------------------\
|                  XyagInitializeUnitGrid                    |
\------------------------------------------------------------*/

void 
XyagInitializeUnitGrid()
{
    XyagUnitGridX = XYAG_DEFAULT_GRID_X;
    XyagUnitGridY = XYAG_DEFAULT_GRID_Y;
    XyagUnitGridDx = XYAG_DEFAULT_GRID_DX;
    XyagUnitGridDy = XYAG_DEFAULT_GRID_DY;

    XyagComputeUnitGrid();
}

/*------------------------------------------------------------\
|                   XyagComputeUnitGrid                      |
\------------------------------------------------------------*/

void 
XyagComputeUnitGrid()
{
    float           StepX;
    float           StepY;

    StepX = (float) (XyagGraphicDx) / (float) (XyagUnitGridDx);
    StepY = (float) (XyagGraphicDy) / (float) (XyagUnitGridDy);

    if (StepX < StepY) {
        XyagUnitGridStep = StepX;
        XyagUnitGridDy = 1 + (XyagGraphicDy / StepX);
    }
    else {
        XyagUnitGridStep = StepY;
        XyagUnitGridDx = 1 + (XyagGraphicDx / StepY);
    }

    XyagPixelGridX = (float) (XyagUnitGridX) * XyagUnitGridStep;
    XyagPixelGridY = (float) (XyagUnitGridY) * XyagUnitGridStep;
}

/*------------------------------------------------------------\
|                    XyagResizeUnitGrid                      |
\------------------------------------------------------------*/

void 
XyagResizeUnitGrid()
{
    XyagUnitGridDx = 1 + (XyagGraphicDx / XyagUnitGridStep);
    XyagUnitGridDy = 1 + (XyagGraphicDy / XyagUnitGridStep);
}

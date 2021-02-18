/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                           |
|                                                             |
| File    :                  Cursor.c                         |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                        Lester Anthony                       |
|                                                             |
\------------------------------------------------------------*/

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>

#include MUT_H
#include MLO_H
#include XSB_H
#include XMX_H

#include "XMX_grid.h"
#include "XMX_cursor.h"

/*------------------------------------------------------------\
|                         Coordinates                         |
\------------------------------------------------------------*/

Position        XyagCursorX = 0;
Position        XyagCursorY = 0;

long            XyagUnitCursorX = 0;
long            XyagUnitCursorY = 0;
long            XyagPixelCursorX = 0;
long            XyagPixelCursorY = 0;

/*------------------------------------------------------------\
|                      XyagComputeCursor                     |
\------------------------------------------------------------*/

void 
XyagComputeCursor(X, Y)
    Position        X;
    Position        Y;
{
    Y = XyagGraphicDy - Y;

    XyagUnitCursorX = X + XyagPixelGridX;
    XyagUnitCursorY = Y + XyagPixelGridY;

    if (XyagUnitCursorX < 0) {
        XyagUnitCursorX = ((float) (XyagUnitCursorX) / XyagUnitGridStep) - 0.5;
    }
    else {
        XyagUnitCursorX = ((float) (XyagUnitCursorX) / XyagUnitGridStep) + 0.5;
    }

    if (XyagUnitCursorY < 0) {
        XyagUnitCursorY = ((float) (XyagUnitCursorY) / XyagUnitGridStep) - 0.5;
    }
    else {
        XyagUnitCursorY = ((float) (XyagUnitCursorY) / XyagUnitGridStep) + 0.5;
    }

    XyagPixelCursorX = ((float) (XyagUnitCursorX) * XyagUnitGridStep);
    XyagPixelCursorY = ((float) (XyagUnitCursorY) * XyagUnitGridStep);

    XyagCursorX = XyagPixelCursorX - XyagPixelGridX;
    XyagCursorY = XyagPixelCursorY - XyagPixelGridY;
    XyagCursorY = XyagGraphicDy - XyagCursorY;
}

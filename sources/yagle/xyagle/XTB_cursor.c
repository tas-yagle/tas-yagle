/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                            |
|                                                             |
| File    :                  cursor.c                         |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                        Lester Anthony                       |
|                                                             |
\------------------------------------------------------------*/

#include <stdio.h>
#include <Xm/Xm.h>
#include <X11/cursorfont.h>
#include <X11/Intrinsic.h>

#include MUT_H
#include MLO_H
#include XTB_H
#include XSB_H

#include "XTB_cursor.h"

static XyagMouseCursor XyagMouseCursorArray[XYAG_MAX_CURSOR] =
{
    {XC_left_ptr, True},
    {XC_watch, True},
    {XC_pirate, True},
    {XC_cross_reverse, True},
    {0, True}
};

static char     XyagCursorMaskOr[8] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static char     XyagCursorMaskAnd[8] =
{
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

/*------------------------------------------------------------\
|                      XyagSetMouseCursor                    |
\------------------------------------------------------------*/

void 
XyagSetMouseCursor(MainWidget, CursorType)
    Widget          MainWidget;
    int             CursorType;
{
    Display        *DisplayId;
    Window          MainWindow;
    XColor          White;
    XColor          Black;
    XColor          ForgetIt;
    Colormap        ColorMap;
    Pixmap          MaskOr;
    Pixmap          MaskAnd;
    Cursor          Type;
    Cursor          NewCursor;

    DisplayId = XtDisplay(MainWidget);
    MainWindow = XtWindow(MainWidget);

    Type = XyagMouseCursorArray[CursorType].CURSOR;

    if (XyagMouseCursorArray[CursorType].MAKE == True) {
        if (Type != 0) {
            NewCursor = XCreateFontCursor(DisplayId, Type);
        }
        else {
            ColorMap = DefaultColormapOfScreen(XtScreen(MainWidget));

            XAllocNamedColor(DisplayId, ColorMap, "black", &Black, &ForgetIt);
            XAllocNamedColor(DisplayId, ColorMap, "white", &White, &ForgetIt);

            MaskOr =

                XCreatePixmapFromBitmapData(DisplayId,
                                            MainWindow,
                                            XyagCursorMaskOr,
                                            8, 8,
                                            Black.pixel,
                                            White.pixel, 1);

            MaskAnd =

                XCreatePixmapFromBitmapData(DisplayId,
                                            MainWindow,
                                            XyagCursorMaskAnd,
                                            8, 8,
                                            Black.pixel,
                                            White.pixel, 1);

            NewCursor = XCreatePixmapCursor(DisplayId,
                                            MaskAnd, MaskOr,
                                            &Black, &White, 0, 0);
        }

        XyagMouseCursorArray[CursorType].CURSOR = NewCursor;
        XyagMouseCursorArray[CursorType].MAKE = False;

        Type = NewCursor;
    }

    XDefineCursor(DisplayId, MainWindow, Type);

    XmUpdateDisplay(MainWidget);
}

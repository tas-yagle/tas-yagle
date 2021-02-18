/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                            |
|                                                             |
| File    :                   icon.c                          |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                        Lester Anthony                       |
|                                                             |
\------------------------------------------------------------*/

# include <Xm/Xm.h>
# include <X11/Intrinsic.h>
# include <X11/xpm.h>

# include MUT_H
# include MLO_H
# include XTB_H
# include XSB_H

# include "XTB_icon.h"

/*------------------------------------------------------------\
|                        XyagSetIcon                          |
\------------------------------------------------------------*/

void
XyagSetIcon(ShellWidget, IconBits, IconWidth, IconHeight )
    Widget  ShellWidget;
    char   *IconBits;
    int     IconWidth;
    int     IconHeight;
{
    Pixmap IconPixmap;

    IconPixmap = XCreatePixmapFromBitmapData(XtDisplay(ShellWidget), RootWindowOfScreen(XtScreen(ShellWidget)), IconBits, IconWidth, IconHeight, 0, 1, DefaultDepthOfScreen(XtScreen(ShellWidget)));
    XtVaSetValues(ShellWidget, XmNiconPixmap, IconPixmap, XmNiconic, False, NULL);
}

/*------------------------------------------------------------\
|                        XyagSetColorIcon                     |
\------------------------------------------------------------*/

void
XyagSetColorIcon(ShellWidget, IconData)
    Widget  ShellWidget;
    char  **IconData;
{
    Display        *Dsp;
    Screen         *Scr;
    int            Depth;
    Pixmap         IconPixmap;
    XpmAttributes  XpmAttr;
    Pixel          Background;
    XpmColorSymbol XpmTransparentColor[1] = {{ NULL, "none", 0 }};

    Dsp = XtDisplayOfObject(ShellWidget);
    Scr = XtScreenOfObject(ShellWidget);
    XtVaGetValues(XtIsSubclass(ShellWidget, coreWidgetClass) ? ShellWidget : XtParent(ShellWidget), XmNdepth, &Depth, XmNbackground, &Background, NULL);

    XpmTransparentColor[0].pixel = Background;
    XpmAttr.valuemask = XpmColorSymbols | XpmCloseness | XpmDepth;
    XpmAttr.colorsymbols = XpmTransparentColor;
    XpmAttr.numsymbols = 1;
    XpmAttr.closeness = 65535;
    XpmAttr.depth = Depth;

    if ((XpmCreatePixmapFromData(Dsp, RootWindowOfScreen(Scr), IconData, &IconPixmap, NULL, &XpmAttr)) == XpmSuccess){
        XtVaSetValues(ShellWidget, XmNiconPixmap, IconPixmap, XmNiconic, False, NULL );
    }
}

/****************************************************************************/
/*                                                                          */
/*                         ALLIANCE  CAD  FRAMEWORK                         */
/*                                                                          */
/*    Tool : libXal Version 1.01                                            */
/*    File :     PixmapLabel.c                                              */
/*                                                                          */
/*    (c) copyright 1993 MASI Laboratory. CAO & VLSI CAD Team               */
/*    All rights reserved.                                                  */
/*    Support : e-mail cao-vlsi@masi.ibp.fr                                 */
/*                                                                          */
/*    Author(s) : Nizar ABDALLAH                      Date : 02/02/1993     */
/*                                                                          */
/*    Modified by :                                   Date : ../../....     */
/*    Modified by :                                   Date : ../../....     */
/*    Modified by :                                   Date : ../../....     */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/* This function is to be called by all ALLIANCE tools in order to present  */
/* the same iconic image.                                                   */
/*                                                                          */
/****************************************************************************/
#include <stdio.h>

#include <Xm/Xm.h>
#include <Xm/Label.h>
#include <X11/xpm.h>

/*-----------------------------*/
/* Motif for Alliance includes */
/*-----------------------------*/
#include MUT_H

#include XAL_H

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XalCreatePixmapLabel                                           */
/*                                                                           */
/* IN  ARGS : .LabelWidget : set a pixmap on a label widget                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Forces the WM to execute the set icon.                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
Widget XalCreatePixmapLabel(LabelWidget, Normal, Insensitive)
Widget   LabelWidget;
char   **Normal;
char   **Insensitive;
{
    Display        *Dsp;
    Screen         *Scr;
    int            Depth;
    int            PixmapX, PixmapY;
    unsigned int   PixmapHeight, PixmapWidth, PixmapBorder, PixmapDepth;
    Window         RootWindow;
    Pixmap         NormalPixmap, InsensitivePixmap;
    XpmAttributes  XpmAttr;
    Boolean        RecomputeSize;
    Pixel          Background;
    GC             gc;
    XGCValues      GCValues;
    Pixmap         Stipple;
    XpmColorSymbol XpmTransparentColor[1] = {{ NULL, "none", 0 }};
    static char    StippleBitmap[8] = { 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA };
    
    Dsp = XtDisplayOfObject(LabelWidget);
    Scr = XtScreenOfObject(LabelWidget);
    XtVaGetValues(XtIsSubclass(LabelWidget, coreWidgetClass) ? LabelWidget : XtParent(LabelWidget), 
                  XmNdepth,      &Depth, 
		  XmNbackground, &Background, 
		  NULL);
    
    XpmTransparentColor[0].pixel = Background;
    XpmAttr.valuemask    = XpmColorSymbols | XpmCloseness | XpmDepth;
    XpmAttr.colorsymbols = XpmTransparentColor;
    XpmAttr.numsymbols   = 1;
    XpmAttr.closeness    = 65535; 
    XpmAttr.depth        = Depth;

    if ((XpmCreatePixmapFromData(Dsp, RootWindowOfScreen(Scr),
                                 Normal, &NormalPixmap, NULL, &XpmAttr)) != XpmSuccess){
        return ( NULL ) ;
       }
    
    Stipple = XCreateBitmapFromData(Dsp, RootWindowOfScreen(Scr),
					    StippleBitmap, 8, 8);
    GCValues.foreground = Background;
    GCValues.fill_style = FillStippled;
    GCValues.stipple    = Stipple;
    gc = XtGetGC(LabelWidget, 
                 GCForeground | GCFillStyle | GCStipple,
                 &GCValues);
    XGetGeometry(Dsp, NormalPixmap, &RootWindow, &PixmapX, &PixmapY,
                 &PixmapWidth, &PixmapHeight, &PixmapBorder, &PixmapDepth);
    InsensitivePixmap = XCreatePixmap(Dsp, RootWindowOfScreen(Scr),
				      PixmapWidth, PixmapHeight, PixmapDepth);
    XCopyArea(Dsp, NormalPixmap, InsensitivePixmap, gc, 0, 0, PixmapWidth, PixmapHeight, 0, 0);
    XFillRectangle(Dsp, InsensitivePixmap, gc, 0, 0, PixmapWidth, PixmapHeight);
	    
    XtReleaseGC(LabelWidget, gc);
    XFreePixmap(Dsp, Stipple);

    XtVaGetValues(LabelWidget, XmNrecomputeSize, &RecomputeSize, NULL);
    XtVaSetValues(LabelWidget, XmNrecomputeSize, True, NULL);
    XtVaSetValues(LabelWidget,
                  XmNlabelType,              XmPIXMAP,
                  XmNlabelPixmap,            NormalPixmap,
                  XmNlabelInsensitivePixmap, InsensitivePixmap,
                  NULL);
    XtVaSetValues(LabelWidget, XmNrecomputeSize, RecomputeSize, NULL);
    return ( LabelWidget );

} /* XalCreatePixmapLabel */

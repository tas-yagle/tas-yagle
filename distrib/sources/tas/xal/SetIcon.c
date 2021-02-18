/****************************************************************************/
/*                                                                          */
/*                         ALLIANCE  CAD  FRAMEWORK                         */
/*                                                                          */
/*    Tool : libXal Version 1.01                                            */
/*    File :      SetIcon.c                                                 */
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


/*--------------------------------------------------------------------------*/
/*                             INCLUDE FILES                                */
/*--------------------------------------------------------------------------*/

/*----------------*/
/* Motif includes */
/*----------------*/
#include <Xm/Xm.h>
#include <X11/xpm.h>

/*-----------------------------*/
/* Motif for Alliance includes */
/*-----------------------------*/

#include MUT_H

#include XAL_H

#include "Icon.h"

static Pixmap xal_pixmap;


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XalForceIcon                                                   */
/*                                                                           */
/* IN  ARGS : .shell_w : The shell widget for whom we want to create the icon*/
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Forces the WM to execute the set icon.                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XalForceIcon( shell_w, dpy )
Widget  shell_w;
Display *dpy;
{
Window   window,root;
unsigned int x, y, width, height, border_width, depth;

XtVaGetValues(shell_w, XmNiconWindow, &window, NULL);

/*---------------------------------------------------------------------------*/
/* If there is no window associated with the shell, create one. Make it at   */
/* least as big as the pixmap we're going to use. The icon window only needs */
/* to be a simple window.                                                    */
/*---------------------------------------------------------------------------*/
if (!window)
    {
    if (!XGetGeometry(dpy, xal_pixmap, &root, &x, &y, &width, &height, &border_width, &depth) ||
        !(window = XCreateSimpleWindow(dpy, root, 0, 0, width, height, (unsigned)0,
                                       CopyFromParent, CopyFromParent)))
        {
        XtVaSetValues(shell_w, XmNiconPixmap, xal_pixmap, NULL);
        return;
        }
    XtVaSetValues(shell_w, XmNiconWindow, window, NULL);
    }
XSetWindowBackgroundPixmap(dpy, window, xal_pixmap);
XClearWindow(dpy, window);
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XalSetIcon                                                     */
/*                                                                           */
/* IN  ARGS : .shell_w : The shell widget for whom we want to create the icon*/
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Icon creation function.                                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XalSetIcon( shell_w )
Widget shell_w;
{
Display  *dpy    = XtDisplay(shell_w);
Screen   *screen = XtScreen(shell_w);
XpmAttributes  XpmAttr;
XpmColorSymbol XpmTransparentColor[1] = {{ NULL, "none", 0 }};

XpmTransparentColor[0].pixel = WhitePixelOfScreen(screen) ;
XpmAttr.valuemask    = XpmColorSymbols | XpmCloseness | XpmDepth;
XpmAttr.colorsymbols = XpmTransparentColor;
XpmAttr.numsymbols   = 1;
XpmAttr.closeness    = 65535;
XpmAttr.depth        = DefaultDepthOfScreen(screen);

if ((XpmCreatePixmapFromData(dpy, RootWindowOfScreen(screen),
              Icon, &xal_pixmap, NULL, &XpmAttr)) != XpmSuccess)
     {

xal_pixmap = XCreatePixmapFromBitmapData( dpy,
                                      RootWindowOfScreen(screen),
                                      Icon_bits, Icon_width, Icon_height,
                                      BlackPixelOfScreen(screen),
                                      WhitePixelOfScreen(screen),
                                      DefaultDepthOfScreen(screen));

       }

if ( xal_pixmap != XmUNSPECIFIED_PIXMAP )
    XalForceIcon( shell_w, dpy );
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XalCustomIcon                                                  */
/*                                                                           */
/* IN  ARGS : .shell_w : The shell widget for whom we want to create the icon*/
/*            .pix_file : Bitmap filename containing Icon pixmap.            */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Icon creation function.                                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XalCustomIcon( shell_w, pix_file )
Widget shell_w;
int   pix_file;
{
Display  *dpy    = XtDisplay(shell_w);
Screen   *screen = XtScreen(shell_w);

if(XalBitmaps != NULL)
 {
  xal_pixmap = XalGetPixmap( shell_w , pix_file, BlackPixelOfScreen(screen),
                                                 WhitePixelOfScreen(screen));
 }
else
 {
  xal_pixmap = XmUNSPECIFIED_PIXMAP ;
 }

if ( xal_pixmap != XmUNSPECIFIED_PIXMAP )
    XalForceIcon( shell_w, dpy );
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XalGetPixmap                                                   */
/*                                                                           */
/* IN  ARGS : .shell_w : The shell widget for whom we want to create the icon*/
/*            .pix_file : Bitmap filename containing Icon pixmap.            */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Icon creation function.                                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
Pixmap XalGetPixmap (widget, imageindex, foreground, background)
Widget  widget;
int     imageindex;
Pixel   foreground;
Pixel   background;
{
 Pixmap pixmap;

 pixmap = XmGetPixmap( XtScreen(widget), XalBitmaps[imageindex].name, 
                       foreground , background);

 if ( pixmap == XmUNSPECIFIED_PIXMAP )
   {
    pixmap = XCreatePixmapFromBitmapData( XtDisplay(widget),
                                   RootWindowOfScreen(XtScreen(widget)),
                                   XalBitmaps[imageindex].bitmap,
                                   XalBitmaps[imageindex].width,
                                   XalBitmaps[imageindex].height,
                                   foreground, background,
                                   DefaultDepthOfScreen(XtScreen(widget)));
   }

 return ( pixmap) ;
}

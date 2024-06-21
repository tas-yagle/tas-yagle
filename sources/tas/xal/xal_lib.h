/****************************************************************************/
/*                                                                          */
/*                         ALLIANCE  CAD  FRAMEWORK                         */
/*                                                                          */
/*    Tool : libXal   Version 1.01                                          */
/*    File : Xal102.h                                                       */
/*                                                                          */
/*    (c) copyright 1993 MASI Laboratory. CAO & VLSI CAD Team               */
/*    All rights reserved.                                                  */
/*    Support : e-mail cao-vlsi@masi.ibp.fr                                 */
/*                                                                          */
/*    Author(s) : Nizar ABDALLAH                      Date : 02/02/1993     */
/*                                                                          */
/*    Modified by : Lionel PROTZENKO                  Date : 04/04/1997     */
/*    Modified by :                                   Date : ../../....     */
/*    Modified by :                                   Date : ../../....     */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/* This file is to be included in programs in order to use the all libXal   */
/* functions.                                                               */
/*                                                                          */
/****************************************************************************/

#ifndef XAL
#define XAL

/* BuildMenus.c structure */
typedef struct XalMenuItem
    {
    char                 *label;        /* the label for the item            */
    WidgetClass          *wid_class;    /* pushbutton, label...              */
    char                 mnemonic;      /* mnemonic; NULL if none            */
    Boolean              separator;     /* True if needs separator           */
    Boolean              sub_title;     /* True if needs submenus title      */
    char                 *accelerator;  /* accelerator; NULL if none         */
    char                 *accel_text;   /* to be converted to compound strin */
    void                 (*callback)(); /* routine to call; NULL if none     */
    XtPointer            callback_data; /* client_data for callback()        */
    struct XalMenuItem   *subitems;     /* pullright menu items, if not NULL */
    }
XalMenuItemStruct;


/* ButtonMenus.c structure */
typedef struct XalButtonsItem
    {
    int                  pix_file;     /* Bitmap file for the icon          */
    Boolean              selected;      /* True if user asks to draw it      */
    void                 (*callback)(); /* routine to call; NULL if none     */
    XtPointer            callback_data; /* client_data for callback()        */
    }
XalButtonsItemStruct;

typedef struct XalBitmaps
    {
     const char *name ;
     char *bitmap ;
     int   width ;
     int   height ;
    }
XalBitmapsStruct;

typedef struct XalMessageWidget
    {
     Widget short_mess_widget;
     Widget long_mess_widget;
    }
XalMessageWidgetStruct;


/* Some defines for XalSetCursor function */
#define NORMAL 0
#define WAIT   1
#define PIRATE 2

/* variable */

extern XalBitmapsStruct *XalBitmaps ;

/* Cursor.c functions */
extern void XalSetPointer                  __P((
                                                 Widget ,
                                                 Cursor
                                              )) ;
extern void XalSetCursor                   __P((
                                                 Widget ,
                                                 int
                                              )) ;


/* ForceUpdate.c functions */
extern void XalForceUpdate                 __P(( Widget )) ;


/* FrontPage.c functions */
extern void XalFrontPage                   __P((
                                                 Widget ,
                                                 char * ,
                                                 char * ,
                                                 char * ,
                                                 char * ,
                                                 char * 
                                              )) ;


/* BuildMenus.c functions */
extern void XalBuildMenus                  __P((
                                                 Widget ,
                                                 struct XalMenuItem *
                                              )) ;


/* ButtonMenus.c functions */
extern Widget XalButtonMenus               __P((
                                                 Widget ,
                                                 struct XalButtonsItem * ,
                                                 Arg [],
                                                 int ,
                                                 int ,
                                                 int
                                               )) ;


/* OnVersion.c functions */
extern void XalUnversionCallback            __P(( 
                                                  Widget ,
                                                  XtPointer ,
                                                  XtPointer
                                               )) ;
extern void XalOnVersionCallback            __P((
                                                  Widget ,
                                                  char *
                                               )) ;
extern void XalCreateOnVersion              __P((
                                                  Widget ,
                                                  char *
                                               )) ;


/* SetIcon.c functions */
extern void XalForceIcon                    __P((
                                                  Widget ,
                                                  Display *
                                               )) ;
extern void XalSetIcon                      __P(( Widget )) ;
extern void XalCustomIcon                   __P((
                                                  Widget ,
                                                  int
                                               )) ;
extern Pixmap XalGetPixmap                  __P((
                                                  Widget ,
                                                  int ,
                                                  Pixel ,
                                                  Pixel
                                               )) ;


/* LimitedLoop.c functions */
extern void XalLimitedLoop                  __P(( Widget )) ;
extern void XalLeaveLimitedLoop             __P(( void )) ;


/* Boxes.c functions */
extern void XalLeaveBox                     __P((
                                                  Widget ,
                                                  XtPointer ,
                                                  XtPointer
                                               )) ;
extern XalMessageWidgetStruct   *XalCreateErrorBox             __P(( Widget )) ;
extern XalMessageWidgetStruct   *XalCreateWarningBox           __P(( Widget )) ;
extern XalMessageWidgetStruct   *XalCreateTraceBox             __P(( Widget )) ;
extern void   XalDrawMessage                __P((
                                                  XalMessageWidgetStruct *,
                                                  char * 
                                               )) ;

/* PixmapLabel.c functions */
extern Widget XalCreatePixmapLabel          __P(( Widget ,
                                                  char ** ,
                                                  char **
                                               )) ;

#endif

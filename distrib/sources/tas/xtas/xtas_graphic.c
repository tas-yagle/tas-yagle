/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_graphic.c                                              */
/*                                                                          */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles AUGUSTINS                                          */
/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*                             INCLUDE FILES                                */
/*--------------------------------------------------------------------------*/
#include "xtas.h"

/*--------------------------------------------------------------------------*/
/*                             DEFINES                                      */
/*--------------------------------------------------------------------------*/

#define XTAS_PATTERN_WIDTH             16
#define XTAS_PATTERN_HEIGHT            16
#define XTAS_PATTERN_SQUARE            32
#define XTAS_MAX_PATTERN               38

/* #define XTAS_SCALE(x, xmin, xmax, w)   ((((x) - (xmin)) * (w)) / ((xmax) - (xmin))) */
#define XTAS_SCALE(x, xmin, xmax, w)    ((((float)(x) / ((xmax) - (xmin))) - ((float)(xmin) / ((xmax) - (xmin)))) * (w)) 

#define XTAS_UNSCALE(x, xmin, xmax, w) ((((x) * ((xmax) - (xmin))) / (w)) + (xmin))
#define MAX(x, y)                      (((x) > (y)) ? (x) : (y))
#define MIN(x, y)                      (((x) > (y)) ? (y) : (x))

/* signals type */

#define XTAS_SLOPE_MASK ((char)0xff)
#define XTAS_STABLE     ((char)0x1)
#define XTAS_UNSTABLE   ((char)0x2)
#define XTAS_INPUT      ((char)0x4)
#define XTAS_OUTPUT     ((char)0x8)
#define XTAS_SPECOUT    ((char)0x10)
#define XTAS_PERIODIC   ((char)0x20)
#define XTAS_UNPERIODIC ((char)0x40)
#define XTAS_ERROR      ((char)0x80)

#define XTAS_MAXTABVALUE 1024 
#define XTAS_TABERROR    6000

/*--------------------------------------------------------------------------*/
/*                             GLOBALS                                      */
/*--------------------------------------------------------------------------*/

static long XtasTabValue[XTAS_MAXTABVALUE] ;

static XtasZoomContextStruct *Xtas_ZoomHead ;

static      Pixmap XtasPattern[XTAS_MAX_PATTERN] ;

static char XtasPatternBits[XTAS_MAX_PATTERN][XTAS_PATTERN_SQUARE] =

  {        
    { 
      0x7f, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xf7, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0x7f, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xf7, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
    }
    ,     
    { 
      0x77, 0x77, 0xbb, 0xbb, 0xdd, 0xdd, 0xee, 0xee, 0x77, 0x77, 0xbb, 0xbb,
      0xdd, 0xdd, 0xee, 0xee, 0x77, 0x77, 0xbb, 0xbb, 0xdd, 0xdd, 0xee, 0xee,
      0x77, 0x77, 0xbb, 0xbb, 0xdd, 0xdd, 0xee, 0xee
    }
    ,    
    {
      0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55,
      0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55,
      0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55
    }
    ,   
    { 
      0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa,
      0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa,
      0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa
    }
    ,  
    { 
      0x88, 0x88, 0x44, 0x44, 0x22, 0x22, 0x11, 0x11, 0x88, 0x88, 0x44, 0x44,
      0x22, 0x22, 0x11, 0x11, 0x88, 0x88, 0x44, 0x44, 0x22, 0x22, 0x11, 0x11,
      0x88, 0x88, 0x44, 0x44, 0x22, 0x22, 0x11, 0x11
    }
    , 
    { 
      0x55, 0x55, 0xff, 0xff, 0x55, 0x55, 0xff, 0xff, 0x55, 0x55, 0xff, 0xff,
      0x55, 0x55, 0xff, 0xff, 0x55, 0x55, 0xff, 0xff, 0x55, 0x55, 0xff, 0xff,
      0x55, 0x55, 0xff, 0xff, 0x55, 0x55, 0xff, 0xff
    }
    ,
    { 
      0x11, 0x11, 0x00, 0x00, 0x44, 0x44, 0x00, 0x00, 0x11, 0x11, 0x00, 0x00,
      0x44, 0x44, 0x00, 0x00, 0x11, 0x11, 0x00, 0x00, 0x44, 0x44, 0x00, 0x00,
      0x11, 0x11, 0x00, 0x00, 0x44, 0x44, 0x00, 0x00
    }
    ,
    { 
      0xaa, 0xaa, 0x11, 0x11, 0xaa, 0xaa, 0x44, 0x44, 0xaa, 0xaa, 0x11, 0x11,
      0xaa, 0xaa, 0x44, 0x44, 0xaa, 0xaa, 0x11, 0x11, 0xaa, 0xaa, 0x44, 0x44,
      0xaa, 0xaa, 0x11, 0x11, 0xaa, 0xaa, 0x44, 0x44
    }
    ,      
    {
      0x44, 0x44, 0x11, 0x11, 0x44, 0x44, 0x11, 0x11, 0x44, 0x44, 0x11, 0x11,
      0x44, 0x44, 0x11, 0x11, 0x44, 0x44, 0x11, 0x11, 0x44, 0x44, 0x11, 0x11,
      0x44, 0x44, 0x11, 0x11, 0x44, 0x44, 0x11, 0x11
    }
    ,     
    {
      0x88, 0x88, 0x22, 0x22, 0x88, 0x88, 0x22, 0x22, 0x88, 0x88, 0x22, 0x22,
      0x88, 0x88, 0x22, 0x22, 0x88, 0x88, 0x22, 0x22, 0x88, 0x88, 0x22, 0x22,
      0x88, 0x88, 0x22, 0x22, 0x88, 0x88, 0x22, 0x22
    }
    ,    
    {
      0x22, 0x22, 0x88, 0x88, 0x22, 0x22, 0x88, 0x88, 0x22, 0x22, 0x88, 0x88,
      0x22, 0x22, 0x88, 0x88, 0x22, 0x22, 0x88, 0x88, 0x22, 0x22, 0x88, 0x88,
      0x22, 0x22, 0x88, 0x88, 0x22, 0x22, 0x88, 0x88
    }
    ,   
    {
      0x11, 0x11, 0x44, 0x44, 0x11, 0x11, 0x44, 0x44, 0x11, 0x11, 0x44, 0x44,
      0x11, 0x11, 0x44, 0x44, 0x11, 0x11, 0x44, 0x44, 0x11, 0x11, 0x44, 0x44,
      0x11, 0x11, 0x44, 0x44, 0x11, 0x11, 0x44, 0x44
    }
    ,  
    { 
      0x00, 0x00, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x22,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00
    }
    , 
    {
      0xff, 0xff, 0xaa, 0xaa, 0xff, 0xff, 0xaa, 0xaa, 0xff, 0xff, 0xaa, 0xaa,
      0xff, 0xff, 0xaa, 0xaa, 0xff, 0xff, 0xaa, 0xaa, 0xff, 0xff, 0xaa, 0xaa,
      0xff, 0xff, 0xaa, 0xaa, 0xff, 0xff, 0xaa, 0xaa
    }
    ,      
    {
      0x00, 0x00, 0x00, 0x00, 0x44, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x44, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x44, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x44, 0x44, 0x00, 0x00
    }
    ,     
    {
      0x44, 0x44, 0x00, 0x00, 0x11, 0x11, 0x00, 0x00, 0x44, 0x44, 0x00, 0x00,
      0x11, 0x11, 0x00, 0x00, 0x44, 0x44, 0x00, 0x00, 0x11, 0x11, 0x00, 0x00,
      0x44, 0x44, 0x00, 0x00, 0x11, 0x11, 0x00, 0x00
    } 
    ,
    { 
      0xee, 0xee, 0x77, 0x77, 0xbb, 0xbb, 0xdd, 0xdd, 0xee, 0xee, 0x77, 0x77,
      0xbb, 0xbb, 0xdd, 0xdd, 0xee, 0xee, 0x77, 0x77, 0xbb, 0xbb, 0xdd, 0xdd,
      0xee, 0xee, 0x77, 0x77, 0xbb, 0xbb, 0xdd, 0xdd
    }
    ,
    {
      0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa,
      0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa,
      0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa
    }
    ,
    {
      0xdd, 0xdd, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0xd5, 0xd5, 0xaa, 0xaa,
      0x55, 0x55, 0xaa, 0xaa, 0xdd, 0xdd, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa,
      0xdd, 0xd5, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa
    }
    ,
    {
      0x77, 0x77, 0xaa, 0xaa, 0xd5, 0xd5, 0xaa, 0xaa, 0x77, 0x77, 0xaa, 0xaa,
      0xdd, 0x5d, 0xaa, 0xaa, 0x77, 0x77, 0xaa, 0xaa, 0xd5, 0xd5, 0xaa, 0xaa,
      0x77, 0x77, 0xaa, 0xaa, 0x5d, 0xdd, 0xaa, 0xaa
    }
    ,
    {
      0x55, 0x55, 0xbb, 0xbb, 0x55, 0x55, 0xfe, 0xfe, 0x55, 0x55, 0xbb, 0xbb,
      0x55, 0x55, 0xee, 0xef, 0x55, 0x55, 0xbb, 0xbb, 0x55, 0x55, 0xfe, 0xfe,
      0x55, 0x55, 0xbb, 0xbb, 0x55, 0x55, 0xef, 0xef
    }
    ,
    {
      0xff, 0xff, 0xaa, 0xaa, 0x77, 0x77, 0xaa, 0xaa, 0xff, 0xff, 0xaa, 0xaa,
      0x77, 0x7f, 0xaa, 0xaa, 0xff, 0xff, 0xaa, 0xaa, 0x77, 0x77, 0xaa, 0xaa,
      0xff, 0xff, 0xaa, 0xaa, 0x7f, 0x7f, 0xaa, 0xaa
    }
    ,
    {
      0xff, 0xff, 0xaa, 0xaa, 0xff, 0xff, 0xaa, 0xaa, 0xff, 0xff, 0xaa, 0xaa,
      0xff, 0xff, 0xaa, 0xaa, 0xff, 0xff, 0xaa, 0xaa, 0xff, 0xff, 0xaa, 0xaa,
      0xff, 0xff, 0xaa, 0xaa, 0xff, 0xff, 0xaa, 0xaa
    }
    ,
    {
      0x55, 0x55, 0xff, 0xff, 0xdd, 0xdd, 0xff, 0xff, 0x55, 0x55, 0xff, 0xff,
      0x5d, 0xdd, 0xff, 0xff, 0x55, 0x55, 0xff, 0xff, 0xdd, 0xdd, 0xff, 0xff,
      0x55, 0x55, 0xff, 0xff, 0x5d, 0x5d, 0xff, 0xff
    }
    ,
    {
      0xee, 0xee, 0xff, 0xff, 0xbb, 0xba, 0xff, 0xff, 0xee, 0xee, 0xff, 0xff,
      0xab, 0xbb, 0xff, 0xff, 0xee, 0xee, 0xff, 0xff, 0xbb, 0xba, 0xff, 0xff,
      0xee, 0xee, 0xff, 0xff, 0xbb, 0xab, 0xff, 0xff
    }
    ,
    {
      0xff, 0xff, 0xee, 0xee, 0xff, 0xff, 0xfb, 0xfb, 0xff, 0xff, 0xee, 0xee,
      0xff, 0xff, 0xbf, 0xbb, 0xff, 0xff, 0xee, 0xee, 0xff, 0xff, 0xfb, 0xfb,
      0xff, 0xff, 0xee, 0xee, 0xff, 0xff, 0xbf, 0xbf
    }
    ,
    {
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbb, 0xbb, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xfb, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbb, 0xbb,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xfb
    }
    ,
    {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0x88, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0x88,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x08
    }
    ,
    {
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x77, 0x77, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xf7, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x77, 0x77,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xf7
    }
    ,
    {
      0x10, 0x10, 0x00, 0x00, 0x44, 0x44, 0x00, 0x00, 0x01, 0x11, 0x00, 0x00,
      0x44, 0x44, 0x00, 0x00, 0x10, 0x10, 0x00, 0x00, 0x44, 0x44, 0x00, 0x00,
      0x01, 0x01, 0x00, 0x00, 0x44, 0x44, 0x00, 0x00
    }
    ,
    {
      0x00, 0x00, 0x11, 0x51, 0x00, 0x00, 0x44, 0x44, 0x00, 0x00, 0x15, 0x15,
      0x00, 0x00, 0x44, 0x44, 0x00, 0x00, 0x51, 0x11, 0x00, 0x00, 0x44, 0x44,
      0x00, 0x00, 0x15, 0x15, 0x00, 0x00, 0x44, 0x44
    }
    ,
    {
      0x00, 0x00, 0xaa, 0xaa, 0x00, 0x00, 0x8a, 0x88, 0x00, 0x00, 0xaa, 0xaa,
      0x00, 0x00, 0x88, 0x88, 0x00, 0x00, 0xaa, 0xaa, 0x00, 0x00, 0x8a, 0x8a,
      0x00, 0x00, 0xaa, 0xaa, 0x00, 0x00, 0x88, 0x88
    }
    ,
    {
      0x55, 0x55, 0x00, 0x00, 0x55, 0x55, 0x00, 0x00, 0x55, 0x55, 0x00, 0x00,
      0x55, 0x55, 0x00, 0x00, 0x55, 0x55, 0x00, 0x00, 0x55, 0x55, 0x00, 0x00,
      0x55, 0x55, 0x00, 0x00, 0x55, 0x55, 0x00, 0x00
    }
    ,
    {
      0x55, 0x55, 0x00, 0x00, 0x55, 0x55, 0x88, 0x88, 0x55, 0x55, 0x00, 0x00,
      0x55, 0x55, 0x80, 0x80, 0x55, 0x55, 0x00, 0x00, 0x55, 0x55, 0x88, 0x88,
      0x55, 0x55, 0x00, 0x00, 0x55, 0x55, 0x88, 0x80
    }
    ,
    {
      0x22, 0x22, 0x55, 0x55, 0x80, 0x80, 0x55, 0x55, 0x22, 0x22, 0x55, 0x55,
      0x88, 0x08, 0x55, 0x55, 0x22, 0x22, 0x55, 0x55, 0x80, 0x80, 0x55, 0x55,
      0x22, 0x22, 0x55, 0x55, 0x08, 0x08, 0x55, 0x55
    }
    ,
    {
      0x88, 0x88, 0x55, 0x55, 0x22, 0xa2, 0x55, 0x55, 0x88, 0x88, 0x55, 0x55,
      0x2a, 0x2a, 0x55, 0x55, 0x88, 0x88, 0x55, 0x55, 0xa2, 0x22, 0x55, 0x55,
      0x88, 0x88, 0x55, 0x55, 0x2a, 0x2a, 0x55, 0x55
    }
    ,
    {
      0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x54, 0x54, 0xaa, 0xaa, 0x55, 0x55,
      0xaa, 0xaa, 0x44, 0x44, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x44, 0x54,
      0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x44, 0x44
    }
} ;

Display  *XtasGraphicDisplay = NULL ;  
Pixmap    XtasGraphicPixmap ;  
Pixmap    XtasSaveGraphicPixmap ;  
Pixmap    XtasTextPixmap ;  
Pixmap    XtasSHPixmap ;  
Pixmap    XtasSaveSHPixmap ;  
Pixmap    XtasCaptionPixmap ;  

stbdebug_list *XtasDebug ;

Boolean   XtasClear ;
Boolean   XtasDraw ;

GC        XtasColor[16][3] ;

#define SOLID   0
#define DASH    1
#define STIPPLE 2

#define GRID          0
#define BACKGROUND    1
#define ERROR_CHRONO  2
#define ERROR         3
#define NO_ERROR      4
#define GOOD_CHRONO   5
#define ORIGIN_CHRONO 6
#define SPECOUT       7
#define CLOCK         8
#define VERIFY        9
#define MHOLD         10
#define MSETUP        11
#define ZOOM          12
#define ERROR_ZOOM    13
#define VHOLD         14
#define VSETUP        15

long      XtasPeriod ;
long      XtasTmin ;
long      XtasTmax ;

int       XtasTextHOffset ;      /* 10 */
int       XtasTextVOffset ;      /* 13 */
int       XtasInterSig ;         /* 30 */
int       XtasHSlope ;           /*  5 */
int       XtasVSlope ;           /* 10 */
int       XtasNbGridDivisions ;  /*  7 */
int       XtasVerticalPos ;

int       XtasTextZoneWidth ;
int       XtasTextZoneHeight ;
int       XtasCaptionZoneWidth ;
int       XtasCaptionZoneHeight ;
int       XtasGraphicZoneWidth ;
int       XtasGraphicZoneHeight ;

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasRound                                                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/

long XtasRound(float x)
{
    return (long)(x + 0.5) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasInitValTab                                                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void XtasInitValTab()
{
    int i ;
    
    for(i = 0 ; i < XTAS_MAXTABVALUE ; i++) {
        XtasTabValue[i] = LONG_MAX ;
    }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasAddValTab                                                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void XtasAddValTab(long value)
{
    int i, j ;

    for(i = 0 ; i < XTAS_MAXTABVALUE ; i++) {
        if(value <= XtasTabValue[i]) {
            if(value == XtasTabValue[i])
                break ;
            else {
                for(j = XTAS_MAXTABVALUE - 1 ; j > i ; j--) {
                    XtasTabValue[j] = XtasTabValue[j-1] ;
                }
                XtasTabValue[i] = value ;
                break ;
            }
        }
    }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasGetValFromTab                                              */
/*                                                                           */
/*---------------------------------------------------------------------------*/

long XtasGetValFromTab(long value)
{
    int i ; 
    long res ;
    long error_before, error_current, error_next ;

    for(i = 0 ; i < XTAS_MAXTABVALUE ; i++) {
        if((value > XtasTabValue[i] - XTAS_TABERROR) &&
           (value < XtasTabValue[i] + XTAS_TABERROR)) {
            error_before  = labs(XtasTabValue[i-1] - value) ;
            error_current = labs(XtasTabValue[i] - value) ;
            error_next    = labs(XtasTabValue[i+1] - value) ;
            
            if((error_before <= error_current) && 
               (error_before <= error_next)) {
                res = XtasTabValue[i-1] ;
                break ;
            }
            else if((error_current <= error_before) && 
                    (error_current <= error_next)) {
                res = XtasTabValue[i] ;
                break ;
            }
            else
                res = XtasTabValue[i+1] ;
        }
    }
    if(i == XTAS_MAXTABVALUE)
        res = value ;

    return res ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasPutZoomContext                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void XtasPutZoomContext(tmin, tmax)
    long tmin ;
    long tmax ;
{
    XtasZoomContextStruct *ptzoom ;

    if(Xtas_ZoomHead) {
        if((Xtas_ZoomHead->TMIN == tmin) && (Xtas_ZoomHead->TMAX == tmax))
            return ;
    }
    
    ptzoom = (XtasZoomContextStruct*)mbkalloc(sizeof(XtasZoomContextStruct)) ;

    ptzoom->NEXT  = Xtas_ZoomHead ;
    ptzoom->TMIN  = tmin ;
    ptzoom->TMAX  = tmax ;

    Xtas_ZoomHead = ptzoom ; 
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasGetPrevZoomContext                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void XtasGetPrevZoomContext()
{
    XtasZoomContextStruct *ptzoom ;
    
    if(Xtas_ZoomHead->NEXT) {
        ptzoom = Xtas_ZoomHead ;
        Xtas_ZoomHead = Xtas_ZoomHead->NEXT ;
        mbkfree(ptzoom) ;
    }

    XtasTmin = Xtas_ZoomHead->TMIN ;
    XtasTmax = Xtas_ZoomHead->TMAX ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasFreeZoomContext                                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void XtasFreeZoomContext()
{
    XtasZoomContextStruct *ptzoom ;

    if(Xtas_ZoomHead) {
        while(Xtas_ZoomHead) {
            ptzoom = Xtas_ZoomHead ;
            Xtas_ZoomHead = Xtas_ZoomHead->NEXT ;
            mbkfree(ptzoom) ;
        }
    }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasSetupGC                                                    */
/*                                                                           */
/* OBJECT   : Initialize colormap and graphical contexts                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/

GC XtasSetStippleColor(colormap, color, stipple)
    Colormap colormap ;
    char    *color ;
    int      stipple ;
{
    XColor    c ;
    XColor    dc ;
    XGCValues values;
    
    XAllocNamedColor(XtasGraphicDisplay, colormap, color, &c, &dc);
    values.foreground  = c.pixel ;
    values.fill_style  = FillStippled;
    values.stipple     = XtasPattern[stipple];
    
    return (XtGetGC(XtasGraphicWindow, GCFillStyle|GCStipple|GCForeground, &values)) ;
}
/*---------------------------------------------------------------------------*/

GC XtasSetDashColor(colormap, color, dash)
    Colormap colormap ;
    char    *color ;
    int      dash ;
{
    XColor    c ;
    XColor    dc ;
    XGCValues values;
    
    XAllocNamedColor(XtasGraphicDisplay, colormap, color, &c, &dc);
    values.foreground  = c.pixel ;
    values.line_style  = LineDoubleDash ;
    values.dashes      = dash ;
    
    return (XtGetGC(XtasGraphicWindow, GCDashList|GCLineStyle|GCForeground, &values)) ;
}
/*---------------------------------------------------------------------------*/

GC XtasSetSolidColor(colormap, color, width)
    Colormap colormap ;
    char    *color ;
    int      width ;
{
    XColor    c ;
    XColor    dc ;
    XGCValues values;
    
    XAllocNamedColor(XtasGraphicDisplay, colormap, color, &c, &dc);
    values.foreground  = c.pixel ;
    values.line_width  = width ;
    
    return (XtGetGC(XtasGraphicWindow, GCForeground | GCLineWidth, &values)) ;
}

/*---------------------------------------------------------------------------*/

void XtasSetupGC()
{
    Colormap colormap = DefaultColormapOfScreen(XtScreen(XtasGraphicWindow));
    
    /* GRID          */
    XtasColor[GRID][SOLID] = XtasSetSolidColor(colormap, "PeachPuff", 1) ;
    XtasColor[GRID][DASH] = XtasSetDashColor(colormap, "PeachPuff", 1) ;
    
    /* BACKGROUND    */
    XtasColor[BACKGROUND][SOLID] = XtasSetSolidColor(colormap, "black", 1) ;
    
    /* ERROR_CHRONO  */
    XtasColor[ERROR_CHRONO][SOLID] = XtasSetSolidColor(colormap, "red", 1) ;
    XtasColor[ERROR_CHRONO][STIPPLE] = XtasSetStippleColor(colormap, "red", 4) ;
    XtasColor[ERROR_CHRONO][DASH] = XtasSetDashColor(colormap, "red", 3) ;
    
    /* ERROR         */
    XtasColor[ERROR][SOLID] = XtasSetSolidColor(colormap, "red", 1) ;
    
    /* NO_ERROR      */
    XtasColor[NO_ERROR][SOLID] = XtasSetSolidColor(colormap, "white", 1) ;
    
    /* GOOD_CHRONO   */
    XtasColor[GOOD_CHRONO][SOLID] = XtasSetSolidColor(colormap, "green", 1) ;
    XtasColor[GOOD_CHRONO][STIPPLE] = XtasSetStippleColor(colormap, "green", 4) ;
    XtasColor[GOOD_CHRONO][DASH] = XtasSetDashColor(colormap, "green", 3) ;
    
    /* ORIGIN_CHRONO */
    XtasColor[ORIGIN_CHRONO][SOLID] = XtasSetSolidColor(colormap, "orange", 1) ;
    XtasColor[ORIGIN_CHRONO][STIPPLE] = XtasSetStippleColor(colormap, "orange", 4) ;
    XtasColor[ORIGIN_CHRONO][DASH] = XtasSetDashColor(colormap, "orange", 3) ;
    
    /* SPECOUT       */
    XtasColor[SPECOUT][SOLID] = XtasSetSolidColor(colormap, "DarkGrey", 1) ;
    XtasColor[SPECOUT][STIPPLE] = XtasSetStippleColor(colormap, "DarkGrey", 4) ;
    XtasColor[SPECOUT][DASH] = XtasSetDashColor(colormap, "DarkGrey", 3) ;
    
    /* CLOCK         */ 
    XtasColor[CLOCK][SOLID] = XtasSetSolidColor(colormap, "DarkGrey", 1) ;
    XtasColor[CLOCK][DASH] = XtasSetDashColor(colormap, "DarkGrey", 3) ;
    
    /* VERIF         */
    XtasColor[VERIFY][SOLID] = XtasSetSolidColor(colormap, "DarkGrey", 2) ;
    XtasColor[VERIFY][STIPPLE] = XtasSetStippleColor(colormap, "DarkGrey", 2) ;
    
    /* HOLD VERIF     */
    XtasColor[VHOLD][SOLID] = XtasSetSolidColor(colormap, "magenta", 1) ;
    XtasColor[VHOLD][STIPPLE] = XtasSetStippleColor(colormap, "magenta", 2) ;

    /* SETUP VERIF    */
    XtasColor[VSETUP][SOLID] = XtasSetSolidColor(colormap, "yellow", 1) ;
    XtasColor[VSETUP][STIPPLE] = XtasSetStippleColor(colormap, "yellow", 2) ;

    /* HOLD MARGIN   */
    XtasColor[MHOLD][SOLID] = XtasSetSolidColor(colormap, "purple", 1) ;
    XtasColor[MHOLD][STIPPLE] = XtasSetStippleColor(colormap, "purple", 2) ;

    /* SETUP MARGIN  */
    XtasColor[MSETUP][SOLID] = XtasSetSolidColor(colormap, "lightblue", 1) ;
    XtasColor[MSETUP][STIPPLE] = XtasSetStippleColor(colormap, "lightblue", 2) ;

    /* ZOOM          */
    XtasColor[ZOOM][SOLID] = XtasSetSolidColor(colormap, "white", 1) ; 
    XtasColor[ZOOM][STIPPLE] = XtasSetStippleColor(colormap, "white", 2) ;

    /* ERROR_ZOOM  */
    XtasColor[ERROR_ZOOM][SOLID] = XtasSetSolidColor(colormap, "red", 1) ;
    XtasColor[ERROR_ZOOM][STIPPLE] = XtasSetStippleColor(colormap, "red", 2) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTIONS : XtasRefreshGraphicZone                                        */
/*             XtasClearGraphicZone                                          */
/*             XtasRefreshSHZone                                             */
/*             XtasClearSHZone                                               */
/*             XtasRefreshTextZone                                           */
/*             XtasClearTextZone                                             */
/*             XtasRefreshCaptionDnZone                                      */
/*             XtasClearCaptionDnZone                                        */
/*             XtasRefreshCaptionUpZone                                      */
/*             XtasClearCaptionUpZone                                        */
/*                                                                           */
/* OBJECT    : Refresh / Clear all the zones of the drawing area             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*                              Graphic Zone                                 */
/*---------------------------------------------------------------------------*/

void XtasRefreshGraphicZone() 
{
     XCopyArea(XtasGraphicDisplay, 
               XtasGraphicPixmap, 
               XtWindow(XtasGraphicWindow), 
               XtasColor[BACKGROUND][SOLID], 
               0, 
               0, 
               XtasGraphicZoneWidth, 
               XtasGraphicZoneHeight, 
               XtasTextZoneWidth, 
               XtasCaptionZoneHeight);  
}

/*---------------------------------------------------------------------------*/

void XtasClearGraphicZone()
{
    XFillRectangle(XtasGraphicDisplay, 
               XtasGraphicPixmap, 
               XtasColor[BACKGROUND][SOLID], 
               0, 
               0, 
               XtasGraphicZoneWidth, 
               XtasGraphicZoneHeight) ;

    XtasRefreshGraphicZone() ;
}

/*---------------------------------------------------------------------------*/
/*                              SH Zone                                      */
/*---------------------------------------------------------------------------*/

void XtasRefreshSHZone() 
{
     XCopyArea(XtasGraphicDisplay, 
               XtasSHPixmap, 
               XtWindow(XtasGraphicWindow), 
               XtasColor[BACKGROUND][SOLID], 
               0, 
               0, 
               XtasTextZoneWidth, 
               XtasTextZoneHeight, 
               XtasDrawingAreaWidth/* - XtasTextZoneWidth*/, 
               XtasCaptionZoneHeight);  
}

/*--------------------------------------------------------------------------*/

void XtasClearSHZone()
{
    XFillRectangle(XtasGraphicDisplay, 
               XtasSHPixmap, 
               XtasColor[BACKGROUND][SOLID], 
               0, 
               0, 
               XtasTextZoneWidth, 
               XtasTextZoneHeight) ;

    XtasRefreshSHZone() ;
}
/*--------------------------------------------------------------------------*/
/*                             Text Zone                                    */
/*--------------------------------------------------------------------------*/

void XtasRefreshTextZone() 
{
     XCopyArea(XtasGraphicDisplay, 
               XtasTextPixmap, 
               XtWindow(XtasGraphicWindow), 
               XtasColor[BACKGROUND][SOLID], 
               0, 
               0, 
               XtasTextZoneWidth, 
               XtasTextZoneHeight, 
               0, 
               XtasCaptionZoneHeight);  
}

/*--------------------------------------------------------------------------*/

void XtasClearTextZone()
{
    XFillRectangle(XtasGraphicDisplay, 
               XtasTextPixmap, 
               XtasColor[BACKGROUND][SOLID], 
               0, 
               0, 
               XtasTextZoneWidth, 
               XtasTextZoneHeight) ;

    XtasRefreshTextZone() ;
}

/*--------------------------------------------------------------------------*/
/*                             CaptionUp Zone                               */
/*--------------------------------------------------------------------------*/

void XtasRefreshCaptionUpZone() 
{
     XCopyArea(XtasGraphicDisplay, 
               XtasCaptionPixmap, 
               XtWindow(XtasGraphicWindow), 
               XtasColor[BACKGROUND][SOLID], 
               0, 
               0, 
               XtasCaptionZoneWidth, 
               XtasCaptionZoneHeight, 
               0, 
               0);  
}

/*--------------------------------------------------------------------------*/

void XtasClearCaptionUpZone()
{
    XFillRectangle(XtasGraphicDisplay, 
               XtasCaptionPixmap, 
               XtasColor[BACKGROUND][SOLID], 
               0, 
               0, 
               XtasCaptionZoneWidth, 
               XtasCaptionZoneHeight) ;

    XtasRefreshCaptionUpZone() ;
}

/*--------------------------------------------------------------------------*/
/*                             CaptionDn Zone                               */
/*--------------------------------------------------------------------------*/

void XtasRefreshCaptionDnZone() 
{
     XCopyArea(XtasGraphicDisplay, 
               XtasCaptionPixmap, 
               XtWindow(XtasGraphicWindow), 
               XtasColor[BACKGROUND][SOLID], 
               0, 
               0, 
               XtasCaptionZoneWidth, 
               XtasCaptionZoneHeight, 
               0, 
               XtasGraphicZoneHeight + XtasCaptionZoneHeight);  
}

/*--------------------------------------------------------------------------*/

void XtasClearCaptionDnZone()
{
    XFillRectangle(XtasGraphicDisplay, 
               XtasCaptionPixmap, 
               XtasColor[BACKGROUND][SOLID], 
               0, 
               0, 
               XtasCaptionZoneWidth, 
               XtasCaptionZoneHeight) ;

    XtasRefreshCaptionDnZone() ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTIONS : XtasRefreshGraphicWindow                                      */
/*             XtasClearGraphicWindow                                        */
/*                                                                           */
/* OBJECT    : Refresh / Clear the drawing area                              */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void XtasClearGraphicWindow()
{
    XtasClearTextZone() ;
    XtasClearSHZone() ;
    XtasClearCaptionUpZone() ;
    XtasClearCaptionDnZone() ;
    XtasClearGraphicZone() ;
}

/*----------------------------------------------------------------------------*/

void XtasRefreshGraphicWindow()
{
    XtasRefreshTextZone() ;
    XtasRefreshSHZone() ;
    XtasRefreshCaptionUpZone() ;
    XtasRefreshCaptionDnZone() ;
    XtasRefreshGraphicZone() ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDrawPeriod                                                 */
/*                                                                           */
/* IN  ARGS : gc    : the graphical context                                  */
/*            tckup : instant time of the rising slope                       */
/*            tckdn : instant time of the falling slope                      */
/*            T     : clock period                                           */
/*                                                                           */
/* OBJECT   : Draw one clock period                                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void XtasDrawPeriod(gc, tckup, tckdn, T)
    GC   gc ;
    float tckup ;
     float tckdn ;
     float T ; 
{
    long tho ;
    long h ;
    long slope0 ;
    long slope1 ;

    XtasAddValTab((long)(tckup * 1000)) ;
    XtasAddValTab((long)(tckdn * 1000)) ;

    if (tckdn >= tckup) {
        h = XtasVSlope ;
        slope0 = tckup ;
        slope1 = tckdn ;
    }
    else {
        h = -XtasVSlope ;
        slope0 = tckdn ;
        slope1 = tckup ;
    }
        
    tho = (T - slope1 + slope0) / 2 ;
    
    XDrawLine(XtasGraphicDisplay, XtasGraphicPixmap, gc, 
              slope0 - tho, XtasVerticalPos + h, 
              slope0,       XtasVerticalPos + h) ;
    
    XDrawLine(XtasGraphicDisplay, XtasGraphicPixmap, gc, 
              slope0,       XtasVerticalPos + h, 
              slope0,       XtasVerticalPos - h) ;
    
    XDrawLine(XtasGraphicDisplay, XtasGraphicPixmap, gc, 
              slope0,       XtasVerticalPos - h,
              slope1,       XtasVerticalPos - h) ;
    
    XDrawLine(XtasGraphicDisplay, XtasGraphicPixmap, gc,
              slope1,       XtasVerticalPos - h,
              slope1,       XtasVerticalPos + h) ;
    
    XDrawLine(XtasGraphicDisplay, XtasGraphicPixmap, gc,
              slope1,       XtasVerticalPos + h,
              slope1 + tho, XtasVerticalPos + h) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDrawInterval                                               */
/*                                                                           */
/* IN  ARGS : gc    : the graphical context                                  */
/*            d     : instant time of instability                            */
/*            u     : instant time of stability                              */
/*                                                                           */
/* OBJECT   : Draw one interval                                              */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void XtasDrawInterval(gc, d, u)
    GC   gc ;
    float d ;
    float u ;
{
    long oldhslope ;

    XtasAddValTab((long)(d * 1000)) ;
    XtasAddValTab((long)(u * 1000)) ;

    if (u > d) {
        oldhslope = XtasHSlope ;
        if (u - d < XtasHSlope * 2) XtasHSlope = (u - d) / 2 ;
        
        XDrawLine(XtasGraphicDisplay, XtasGraphicPixmap, gc, 
                d,              XtasVerticalPos,
                d + XtasHSlope, XtasVerticalPos + XtasVSlope) ;
        XDrawLine(XtasGraphicDisplay, XtasGraphicPixmap, gc, 
                d + XtasHSlope, XtasVerticalPos + XtasVSlope,
                u - XtasHSlope, XtasVerticalPos + XtasVSlope) ;
        XDrawLine(XtasGraphicDisplay, XtasGraphicPixmap, gc, 
                u - XtasHSlope, XtasVerticalPos + XtasVSlope,
                u,              XtasVerticalPos) ;
        XDrawLine(XtasGraphicDisplay, XtasGraphicPixmap, gc, 
                d,              XtasVerticalPos,
                d + XtasHSlope, XtasVerticalPos - XtasVSlope) ;
        XDrawLine(XtasGraphicDisplay, XtasGraphicPixmap, gc, 
                d + XtasHSlope, XtasVerticalPos - XtasVSlope,
                u - XtasHSlope, XtasVerticalPos - XtasVSlope) ;
        XDrawLine(XtasGraphicDisplay, XtasGraphicPixmap, gc, 
                u - XtasHSlope, XtasVerticalPos - XtasVSlope,
                u,              XtasVerticalPos) ;
        
        XtasHSlope = oldhslope ;
    }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDrawFilledInterval                                         */
/*                                                                           */
/* IN  ARGS : gc    : the graphical context                                  */
/*            d     : instant time of instability                            */
/*            u     : instant time of stability                              */
/*                                                                           */
/* OBJECT   : Draw one filled interval                                       */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void XtasDrawFilledInterval(gc, d, u)
    GC gc ;
    float d ;
    float u ;
{
    XPoint points[7] ;
    long oldhslope ;

    XtasAddValTab((long)(d * 1000)) ;
    XtasAddValTab((long)(u * 1000)) ;

    if (u > d) {
        oldhslope = XtasHSlope ;
        if (u - d < XtasHSlope * 2) XtasHSlope = (u - d) / 2 ;
    
        points[0].x = d ;
        points[0].y = XtasVerticalPos ;
        
        points[1].x = d + XtasHSlope ;
        points[1].y = XtasVerticalPos + XtasVSlope ;
        
        points[2].x = u - XtasHSlope ; 
        points[2].y = XtasVerticalPos + XtasVSlope ;
        
        points[3].x = u ;             
        points[3].y = XtasVerticalPos ;        
        
        points[4].x = u - XtasHSlope ;
        points[4].y = XtasVerticalPos - XtasVSlope ;
                  
        points[5].x = d + XtasHSlope ; 
        points[5].y = XtasVerticalPos - XtasVSlope ;
        
        points[6].x = d ;
        points[6].y = XtasVerticalPos ;
        
        XFillPolygon(XtasGraphicDisplay, XtasGraphicPixmap, gc, 
                points, 7,
                Nonconvex, 
                CoordModeOrigin
                );
        
        XtasHSlope = oldhslope ;
    }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDrawIntervalDn                                             */
/*                                                                           */
/* IN  ARGS : gc    : the graphical context                                  */
/*            d     : instant time of instability                            */
/*            u     : instant time of stability                              */
/*                                                                           */
/* OBJECT   : Draw the bottom part of an interval                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void XtasDrawIntervalDn(gc, d, u)
    GC gc ;
    float d ;
    float u ;
{
    long oldhslope ;

    XtasAddValTab((long)(d * 1000)) ;
    XtasAddValTab((long)(u * 1000)) ;

    if (u > d) {
        oldhslope = XtasHSlope ;
        if (u - d < XtasHSlope * 2) XtasHSlope = (u - d) / 2 ;
        
        XDrawLine(XtasGraphicDisplay, XtasGraphicPixmap, gc, 
                d,              XtasVerticalPos,
                d + XtasHSlope, XtasVerticalPos+ XtasVSlope) ;
        XDrawLine(XtasGraphicDisplay, XtasGraphicPixmap, gc, 
                d + XtasHSlope, XtasVerticalPos+ XtasVSlope,
                u - XtasHSlope, XtasVerticalPos+ XtasVSlope) ;
        XDrawLine(XtasGraphicDisplay, XtasGraphicPixmap, gc, 
                u - XtasHSlope, XtasVerticalPos+ XtasVSlope,
                u,              XtasVerticalPos) ;
        
        XtasHSlope = oldhslope ;
    }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDrawSeparator                                              */
/*                                                                           */
/* IN  ARGS : gc    : the graphical context                                  */
/*            d     : instant time of instability                            */
/*            u     : instant time of stability                              */
/*                                                                           */
/* OBJECT   : Draw a High Impedance interval                                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void XtasDrawSeparator (void)
{
    XDrawLine(XtasGraphicDisplay, XtasGraphicPixmap, XtasColor[GRID][DASH], 
            0,                    XtasVerticalPos,
            XtasGraphicZoneWidth, XtasVerticalPos) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDrawIntervalHZ                                             */
/*                                                                           */
/* IN  ARGS : gc    : the graphical context                                  */
/*            d     : instant time of instability                            */
/*            u     : instant time of stability                              */
/*                                                                           */
/* OBJECT   : Draw a High Impedance interval                                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void XtasDrawIntervalHZ(gc, d, u)
    GC gc ;
    float d ;
    float u ;
{
    XtasAddValTab((long)(d * 1000)) ;
    XtasAddValTab((long)(u * 1000)) ;

    XDrawLine(XtasGraphicDisplay, XtasGraphicPixmap, gc, 
            d, XtasVerticalPos,
            u, XtasVerticalPos) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDrawIntervalUp                                             */
/*                                                                           */
/* IN  ARGS : gc    : the graphical context                                  */
/*            d     : instant time of instability                            */
/*            u     : instant time of stability                              */
/*                                                                           */
/* OBJECT   : Draw the top part of an interval                               */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void XtasDrawIntervalUp(gc, d, u)
    GC gc ;
    float d ;
    float u ;
{
    long oldhslope ;

    XtasAddValTab((long)(d * 1000)) ;
    XtasAddValTab((long)(u * 1000)) ;


    if (u > d) {
        oldhslope = XtasHSlope ;
        if (u - d < XtasHSlope * 2) XtasHSlope = (u - d) / 2 ;
        
        XDrawLine(XtasGraphicDisplay, XtasGraphicPixmap, gc, 
                d,              XtasVerticalPos,
                d + XtasHSlope, XtasVerticalPos - XtasVSlope) ;
        XDrawLine(XtasGraphicDisplay, XtasGraphicPixmap, gc, 
                d + XtasHSlope, XtasVerticalPos - XtasVSlope,
                u - XtasHSlope, XtasVerticalPos - XtasVSlope) ;
        XDrawLine(XtasGraphicDisplay, XtasGraphicPixmap, gc, 
                u - XtasHSlope, XtasVerticalPos - XtasVSlope,
                u,              XtasVerticalPos) ;
        
        XtasHSlope = oldhslope ;
    }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDrawStateRep                                               */
/*                                                                           */
/* IN  ARGS : x0 x1 y0 y1 : coordinates of the rectangle                     */
/*                                                                           */
/* OBJECT   : Draw a rectangle                                               */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void XtasDrawStateRep(x0, x1, y0, y1, setup, hold)
    long x0 ;
    long x1 ;
    long y0 ;
    long y1 ;
    long setup ;
    long hold ;
{
    long sx0, sx1 ;
    
    sx0 = XTAS_SCALE(x0 - setup, 
                     XtasTmin,
                     XtasTmax, 
                     XtasGraphicZoneWidth) ;
    
    sx1 = XTAS_SCALE(x1 + hold, 
                     XtasTmin,
                     XtasTmax, 
                     XtasGraphicZoneWidth) ;
    
//    XFillRectangle(XtasGraphicDisplay, XtasGraphicPixmap, XtasColor[VERIFY][STIPPLE], 
//                   MIN(sx0, sx1),  y0,
//                   sx1 - sx0, labs(y1 - y0)) ;
    XFillRectangle(XtasGraphicDisplay, XtasGraphicPixmap, XtasColor[VERIFY][STIPPLE], 
                   sx0,  y0,
                   sx1 - sx0, labs(y1 - y0)) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDrawEdgeRep                                                */
/*                                                                           */
/* IN  ARGS : x y0 y1 : coordinates of the segment                           */
/*                                                                           */
/* OBJECT   : Draw a vertical segment                                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void XtasDrawEdgeRep(x, y0, y1, setup, hold, holdperiod)
    long x ;
    long y0 ;
    long y1 ;
    long setup ;
    long hold ;
    long holdperiod;
{
    long sx ;
    long sh ;
    long ss ;
    
    sx = XTAS_SCALE(x, XtasTmin, XtasTmax, XtasGraphicZoneWidth) ;
    ss = XTAS_SCALE(x - setup, XtasTmin, XtasTmax, XtasGraphicZoneWidth) ;
    sh = XTAS_SCALE(x - holdperiod + hold, XtasTmin, XtasTmax, XtasGraphicZoneWidth) ;
    
    if (setup && hold) {
        /*
        XFillRectangle (XtasGraphicDisplay, XtasGraphicPixmap, XtasColor[VERIFY][STIPPLE], 
                       MIN (ss, sh), y0,
                       labs (sh - ss), labs (y1 - y0)) ;
        */
        XtasAddValTab ((long)(ss*1000));
        XtasAddValTab ((long)(sh*1000));
        XDrawLine(XtasGraphicDisplay, XtasGraphicPixmap, XtasColor[MSETUP][SOLID], ss, y0, ss, y1) ; 
        XDrawLine(XtasGraphicDisplay, XtasGraphicPixmap, XtasColor[MHOLD][SOLID], sh, y0, sh, y1) ; 
    } 
    if (holdperiod) {
        ss = XTAS_SCALE(x, XtasTmin, XtasTmax, XtasGraphicZoneWidth) ;
        sh = XTAS_SCALE(x - holdperiod, XtasTmin, XtasTmax, XtasGraphicZoneWidth) ;
        XtasAddValTab ((long)(ss*1000));
        XtasAddValTab ((long)(sh*1000));
        XDrawLine(XtasGraphicDisplay, XtasGraphicPixmap, XtasColor[VSETUP][SOLID], ss, y0, ss, y1) ; 
        XDrawLine(XtasGraphicDisplay, XtasGraphicPixmap, XtasColor[VHOLD][SOLID], sh, y0, sh, y1) ; 
        
    }else {
        XtasAddValTab ((long)(sx*1000));
        XDrawLine(XtasGraphicDisplay, XtasGraphicPixmap, XtasColor[VERIFY][SOLID], sx, y0, sx, y1) ; 
                   
    }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDrawCaption                                                */
/*                                                                           */
/* IN  ARGS : gc    : the graphical context                                  */
/*            text  : text to be written                                     */
/*            x, y  : coordinates of the text                                */
/*                                                                           */
/* OBJECT   : Draw a text in the caption zones (up and dn)                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void XtasDrawCaption(gc, text, x, y)
    GC   gc ;
    char *text ;
    long x ;
    long y ;
{
    int l = strlen(text) ;
    XDrawString(XtasGraphicDisplay, XtasCaptionPixmap, gc, x - (l * 6) / 2, y, text, l);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDrawText                                                   */
/*                                                                           */
/* IN  ARGS : gc    : the graphical context                                  */
/*            text  : text to be written                                     */
/*            x, y  : coordinates of the text                                */
/*                                                                           */
/* OBJECT   : Draw a text in the text zone                                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void XtasDrawText(gc, text, x, y)
    GC   gc ;
    char *text ;
    long x ;
    long y ;
{
    int i, l, nbcar, carwidth ;
    int j ;
    char str[1024] ;

    
    carwidth = 7 ;
    l = strlen(text) ;
    
    if (l * carwidth > XtasTextZoneWidth) {
        nbcar = XtasTextZoneWidth / carwidth ;
        XDrawString(XtasGraphicDisplay, XtasTextPixmap, gc, x, y, text, nbcar);

        for(j = 1 ; j < l/nbcar+1 ; j++) {
            for (i = j*nbcar ; i <= l ; i++) {
                str[i - j*nbcar] = text[i] ;
            }
            if(j == l/nbcar)
                 XDrawString(XtasGraphicDisplay, XtasTextPixmap, gc, x, y + 12*j, str, l-j*nbcar);
            else
                 XDrawString(XtasGraphicDisplay, XtasTextPixmap, gc, x, y + 12*j, str, nbcar);
        }
    } else
        XDrawString(XtasGraphicDisplay, XtasTextPixmap, gc, x, y, text, l);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDrawSH                                                     */
/*                                                                           */
/* IN  ARGS : ptdebug : the current debug_list                               */
/*                                                                           */
/* OBJECT   : Draw setup time and hold time in the SH zone                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void XtasDrawSH(ptdebug)
    stbdebug_list *ptdebug ;
{
    
    char period[1024] ;
    char setup[1024] ;
    char hold[1024] ;
    XmString    text;
    Pixel       fg;

    XtVaGetValues (XtasDebugLabelPeriod, XmNforeground, &fg, NULL);

    if (ptdebug) {
        sprintf(period, "period = %.1f ps", ptdebug->PERIODE/TTV_UNIT) ;
        XtasSetLabelString (XtasDebugLabelPeriod, period);
        
        if(ptdebug->SETUP != STB_NO_TIME) {
            sprintf (setup, "setup = %.1f ps", ptdebug->SETUP/TTV_UNIT) ;
            text = XmStringCreateSimple (setup);
            if (ptdebug->SETUP < 0) {
                XtVaSetValues (XtasDebugLabelSetup, XtVaTypedArg, XmNforeground, XtRString, "red", 4, XmNlabelString, text, NULL);
                XmStringFree (text);
            }
            else  {
                XtVaSetValues (XtasDebugLabelSetup, XmNforeground, fg, XmNlabelString, text, NULL);
                XmStringFree (text);
            }
        }
        
        if(ptdebug->HOLD != STB_NO_TIME) {
            sprintf (hold, "hold = %.1f ps", ptdebug->HOLD/TTV_UNIT) ;
            text = XmStringCreateSimple (hold);
            if (ptdebug->HOLD < 0) {
                XtVaSetValues (XtasDebugLabelHold, XtVaTypedArg, XmNforeground, XtRString, "red", 4, XmNlabelString, text, NULL);
                XmStringFree (text);
            }
            else {
                XtVaSetValues (XtasDebugLabelHold, XmNforeground, fg, XmNlabelString, text, NULL);
                XmStringFree (text);
            }                 
        }
    }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasComputeGridStep                                            */
/*                                                                           */
/* IN  ARGS : l : the real width of the graphical zone                       */
/*                                                                           */
/* OUT ARGS : grid step                                                      */
/*                                                                           */
/* OBJECT   : Computes an entire grid step                                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/

long XtasComputeGridStep(long l)
{
    int scale = 1 ;
    int realstep, stretchedstep ;
    int s ;

    realstep = l / XtasNbGridDivisions ;
    realstep = realstep ? realstep : 1 ;

    s = realstep;
    while (s>=10) {
        s = realstep / scale ;
        scale *= 10 ;
    }
    if (scale > 10)
        scale /= 10 ;
    stretchedstep = (realstep / scale) * scale ;
    
    return (stretchedstep) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDrawGrid                                                   */
/*                                                                           */
/* OBJECT   : Draw the grid                                                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void XtasDrawGrid()
{
    long rstep, steppos, rsteppos = 0 ;
    char *caption = (char*)mbkalloc(1024 * sizeof(char)) ;

    rstep = XtasComputeGridStep(XtasTmax - XtasTmin) ;
    
    if (XtasTmin < 0) {
        while (rsteppos > XtasTmin) {
            rsteppos -= rstep ;
        }
        rsteppos += rstep ;
    }
    if (XtasTmin >= 0) {
        while (rsteppos < XtasTmin) {
            rsteppos += rstep ;
        }
//        rsteppos -= rstep ;
    }
        
    while (rsteppos < XtasTmax) {
        steppos  = XTAS_SCALE(rsteppos, 
                              XtasTmin, 
                              XtasTmax, 
                              XtasGraphicZoneWidth) ;
        XDrawLine(XtasGraphicDisplay, XtasGraphicPixmap, XtasColor[GRID][DASH], 
                  steppos, 0, 
                  steppos, XtasGraphicZoneHeight) ; 
        sprintf(caption, "%.1f", rsteppos/TTV_UNIT) ;
        XtasDrawCaption(XtasColor[GRID][SOLID], 
                     caption, 
                     steppos + XtasTextZoneWidth, 
                     XtasTextVOffset) ;
        rsteppos += rstep ;
    }

    XDrawLine(XtasGraphicDisplay, XtasGraphicPixmap, XtasColor[GRID][DASH], 
            0, 0, 
            0, XtasGraphicZoneHeight) ; 
    
    XDrawLine(XtasGraphicDisplay, XtasGraphicPixmap, XtasColor[GRID][DASH], 
            XtasGraphicZoneWidth - 1, 0, 
            XtasGraphicZoneWidth - 1, XtasGraphicZoneHeight) ; 
    
    XDrawLine(XtasGraphicDisplay, XtasGraphicPixmap, XtasColor[GRID][DASH], 
            0,                    0, 
            XtasGraphicZoneWidth, 0) ; 
    
    XDrawLine(XtasGraphicDisplay, XtasGraphicPixmap, XtasColor[GRID][DASH], 
            0,                    XtasGraphicZoneHeight - 1, 
            XtasGraphicZoneWidth, XtasGraphicZoneHeight - 1) ; 

    mbkfree(caption) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDrawClock                                                  */
/*                                                                           */
/* IN  ARGS : tckup : instant time of the rising slope                       */
/*            tckdn : instant time of the falling slope                      */
/*                                                                           */
/* OBJECT   : Draw the clock on x periods                                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void XtasDrawClock(tckup, tckdn, period)
    long tckup ;
    long tckdn ;
    long period;
{
    float sT ;
    float stckup, stckupref;
    float stckdn, stckdnref ;
    long slope0, slope1 ;
    long tho ;


    stckupref  = XTAS_SCALE((float)tckup, 
                            XtasTmin,
                            XtasTmax, 
                            XtasGraphicZoneWidth) ;
    stckdnref  = XTAS_SCALE((float)tckdn, 
                            XtasTmin, 
                            XtasTmax, 
                            XtasGraphicZoneWidth) ;
    sT = ((float)period * XtasGraphicZoneWidth) / 
                        ((XtasTmax - XtasTmin)) ;

    XtasDrawPeriod(XtasColor[CLOCK][SOLID], stckupref, stckdnref, sT) ;
    
    stckup = stckupref + sT ;
    stckdn = stckdnref + sT ;

    slope0 = MIN(stckup, stckdn) ;
    slope1 = MAX(stckup, stckdn) ;
    tho    = (sT - slope1 + slope0) / 2 ;
    
    while (slope0 - tho < XtasGraphicZoneWidth) {
        XtasDrawPeriod(XtasColor[CLOCK][DASH], stckup, stckdn, sT) ;
        stckup += sT ;
        stckdn += sT ;
        slope0  = MIN(stckup, stckdn) ;
        slope1  = MAX(stckup, stckdn) ;
        tho     = (sT - slope1 + slope0) / 2 ;
    }
    
    stckup = stckupref - sT ;
    stckdn = stckdnref - sT ;
    
    while (slope1 + tho > 0) {
        XtasDrawPeriod(XtasColor[CLOCK][DASH], stckup, stckdn, sT) ;
        stckup -= sT ;
        stckdn -= sT ;
        slope0  = MIN(stckup, stckdn) ;
        slope1  = MAX(stckup, stckdn) ;
        tho     = (sT - slope1 + slope0) / 2 ;
    }
}
    
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDrawPeriodicInterval                                       */
/*                                                                           */
/* IN  ARGS : d     : instant time of instability                            */
/*            u     : instant time of stability                              */
/*            type  : input, output, specout                                 */
/*            slope : up, down, both                                         */
/*                                                                           */
/* OBJECT   : Draw the interval on x periods                                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void XtasDrawPeriodicInterval(d, u, type, slope)
    long d ;
     long u ;
    char type ;
    char slope ;
{
     float sT ;
    float sd, sdref ;
     float su, suref ;
    GC   solid_gc ;
    GC   dash_gc ;
    GC   stipple_gc ;

    switch (type & (XTAS_INPUT | XTAS_OUTPUT | XTAS_SPECOUT | XTAS_ERROR)) {
        case XTAS_INPUT : 
            solid_gc   = XtasColor[GOOD_CHRONO][SOLID] ;
            dash_gc    = XtasColor[GOOD_CHRONO][DASH] ;
            stipple_gc = XtasColor[GOOD_CHRONO][STIPPLE] ;
        break ;
        case XTAS_OUTPUT :
            solid_gc   = XtasColor[GOOD_CHRONO][SOLID] ;
            dash_gc    = XtasColor[GOOD_CHRONO][DASH] ;
            stipple_gc = XtasColor[GOOD_CHRONO][STIPPLE] ;
        break ;
        case XTAS_INPUT | XTAS_ERROR: 
            solid_gc   = XtasColor[ORIGIN_CHRONO][SOLID] ;
            dash_gc    = XtasColor[ORIGIN_CHRONO][DASH] ;
            stipple_gc = XtasColor[ORIGIN_CHRONO][STIPPLE] ;
        break ;
        case XTAS_OUTPUT | XTAS_ERROR :
            solid_gc   = XtasColor[ERROR_CHRONO][SOLID] ;
            dash_gc    = XtasColor[ERROR_CHRONO][DASH] ;
            stipple_gc = XtasColor[ERROR_CHRONO][STIPPLE] ;
        break ;
        case XTAS_SPECOUT :
            solid_gc   = XtasColor[SPECOUT][SOLID] ;
            dash_gc    = XtasColor[SPECOUT][DASH] ;
            stipple_gc = XtasColor[SPECOUT][STIPPLE] ;
        break ;
    }
    
    if (u > d) {
        sdref  = XTAS_SCALE((float)d, 
                            XtasTmin, 
                            XtasTmax, 
                            XtasGraphicZoneWidth) ;
        suref  = XTAS_SCALE((float)u, 
                            XtasTmin, 
                            XtasTmax, 
                            XtasGraphicZoneWidth) ;

        sT = (float)XtasPeriod / (XtasTmax - XtasTmin) ;
        sT = sT*XtasGraphicZoneWidth;

        if (type & XTAS_UNSTABLE) {
            XtasDrawFilledInterval(solid_gc, sdref, suref) ;
            XtasDrawInterval(solid_gc, sdref, suref) ;
        }

        if (type & XTAS_STABLE) 
            switch (slope & XTAS_SLOPE_MASK) {
                case STB_UP | STB_DN :
                    XtasDrawInterval(solid_gc, sdref, suref) ;
                    break ;
                case STB_UP :
                    XtasDrawIntervalUp(solid_gc, sdref, suref) ;
                    break ;
                case STB_DN :
                    XtasDrawIntervalDn(solid_gc, sdref, suref) ;
                    break ;
                case STB_UP | STB_HZ :
                    XtasDrawIntervalHZ(solid_gc, sdref, suref) ;
                    break ;
                case STB_DN | STB_HZ :
                    XtasDrawIntervalHZ(solid_gc, sdref, suref) ;
                    break ;
            }
                
    
        if (type & XTAS_PERIODIC) {
            sd = sdref + sT ;
            su = suref + sT ;
            while (sd < XtasGraphicZoneWidth) {
                if (type & XTAS_UNSTABLE) {
                    XtasDrawFilledInterval(stipple_gc, sd, su) ;
                    XtasDrawInterval(solid_gc, sd, su) ;
                }  
                if (type & XTAS_STABLE) 
                    switch (slope & XTAS_SLOPE_MASK) {
                        case STB_UP | STB_DN :
                            XtasDrawInterval(dash_gc, sd, su) ;
                            break ;
                        case STB_UP :
                            XtasDrawIntervalUp(dash_gc, sd, su) ;
                            break ;
                        case STB_DN :
                            XtasDrawIntervalDn(dash_gc, sd, su) ;
                            break ;
                        case STB_UP | STB_HZ :
                            XtasDrawIntervalHZ(dash_gc, sd, su) ;
                            break ;
                        case STB_DN | STB_HZ :
                            XtasDrawIntervalHZ(dash_gc, sd, su) ;
                            break ;
                    }
                sd += sT ;
                su += sT ;
            }
            
            sd = sdref - sT ;
            su = suref - sT ;
            while (su > 0) {
                if (type & XTAS_UNSTABLE) {
                    XtasDrawFilledInterval(stipple_gc, sd, su) ;
                    XtasDrawInterval(solid_gc, sd, su) ;
                }  
                if (type & XTAS_STABLE) 
                    switch (slope & XTAS_SLOPE_MASK) {
                        case STB_UP | STB_DN :
                            XtasDrawInterval(dash_gc, sd, su) ;
                            break ;
                        case STB_UP :
                            XtasDrawIntervalUp(dash_gc, sd, su) ;
                            break ;
                        case STB_DN :
                            XtasDrawIntervalDn(dash_gc, sd, su) ;
                            break ;
                        case STB_UP | STB_HZ :
                            XtasDrawIntervalHZ(dash_gc, sd, su) ;
                            break ;
                        case STB_DN | STB_HZ :
                            XtasDrawIntervalHZ(dash_gc, sd, su) ;
                            break ;
                    }
                sd -= sT ;
                su -= sT ;
            }
        }
    }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasPeriodic                                                   */
/*                                                                           */
/* IN  ARGS : hpair : interval                                               */
/*                                                                           */
/* OUT ARGS : true / false                                                   */
/*                                                                           */
/* OBJECT   : Is the interval's size more than a period ?                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/

int XtasPeriodic (hpair) 
    stbpair_list *hpair ;
{
    stbpair_list *ptpair ;
    stbpair_list *qpair ;

    if (!hpair)
        return (0) ;

    for (ptpair = hpair ; ptpair ; ptpair = ptpair->NEXT)
        qpair = ptpair ;
    
    return (qpair->U - hpair->D <= XtasPeriod) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasFindTmin                                                   */
/*                                                                           */
/* IN  ARGS : ptdebug : the current debug_list                               */
/*                                                                           */
/* OUT ARGS : tmin                                                           */
/*                                                                           */
/* OBJECT   : Finds the minimum instant time in the debug list               */
/*                                                                           */
/*---------------------------------------------------------------------------*/

long XtasFindTmin (stbdebug_list *ptdebug)
{
    long             tmin;
    stbchrono_list  *ptchrono;
    
    if (!ptdebug)
        return 0;
    
    tmin = LONG_MAX;
    
    if (ptdebug->CKREFUP_MIN != STB_NO_TIME) tmin = MIN (tmin, ptdebug->CKREFUP_MIN);
    if (ptdebug->CKREFDN_MIN != STB_NO_TIME) tmin = MIN (tmin, ptdebug->CKREFDN_MIN);
    if (ptdebug->CKUP_MIN != STB_NO_TIME) tmin = MIN (tmin, ptdebug->CKUP_MIN);
    if (ptdebug->CKDN_MIN != STB_NO_TIME) tmin = MIN (tmin, ptdebug->CKDN_MIN);
    if (ptdebug->CKORGUP_MIN != STB_NO_TIME) tmin = MIN (tmin, ptdebug->CKORGUP_MIN);
    if (ptdebug->CKORGDN_MIN != STB_NO_TIME) tmin = MIN (tmin, ptdebug->CKORGDN_MIN);

    for (ptchrono = ptdebug->CHRONO; ptchrono; ptchrono = ptchrono->NEXT) {
        if (ptchrono->SPECS)
            tmin = MIN(tmin, ptchrono->SPECS->D);
        if (ptchrono->SPECS_U)
            tmin = MIN(tmin, ptchrono->SPECS_U->D);
        if (ptchrono->SPECS_D)
            tmin = MIN(tmin, ptchrono->SPECS_D->D);
        if (ptchrono->SIG1S)
            tmin = MIN(tmin, ptchrono->SIG1S->D);
        if (ptchrono->SIG1S_U)
            tmin = MIN(tmin, ptchrono->SIG1S_U->D);
        if (ptchrono->SIG1S_D)
            tmin = MIN(tmin, ptchrono->SIG1S_D->D);
        if (ptchrono->SIG2S)
            tmin = MIN(tmin, ptchrono->SIG2S->D);
        if (ptchrono->SIG2S_U)
            tmin = MIN(tmin, ptchrono->SIG2S_U->D);
        if (ptchrono->SIG2S_D)
            tmin = MIN(tmin, ptchrono->SIG2S_D->D);
    }


    return tmin;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasFindTmax                                                   */
/*                                                                           */
/* IN  ARGS : ptdebug : the current debug_list                               */
/*                                                                           */
/* OUT ARGS : tmax                                                           */
/*                                                                           */
/* OBJECT   : Finds the maximum instant time in the debug list               */
/*                                                                           */
/*---------------------------------------------------------------------------*/

long XtasFindTmax (stbdebug_list *ptdebug)
{
    long             tmax;
    stbchrono_list  *ptchrono;
    long             periodsave;
    
    if (!ptdebug)
        return 0;
    
    tmax = -LONG_MAX;
    
    if (ptdebug->CKREFUP_MAX != STB_NO_TIME) tmax = MAX (tmax, ptdebug->CKREFUP_MAX);
    if (ptdebug->CKREFDN_MAX != STB_NO_TIME) tmax = MAX (tmax, ptdebug->CKREFDN_MAX);
    if (ptdebug->CKUP_MAX != STB_NO_TIME) tmax = MAX (tmax, ptdebug->CKUP_MAX);
    if (ptdebug->CKDN_MAX != STB_NO_TIME) tmax = MAX (tmax, ptdebug->CKDN_MAX);
    if (ptdebug->CKORGUP_MAX != STB_NO_TIME) tmax = MAX (tmax, ptdebug->CKORGUP_MAX);
    if (ptdebug->CKORGDN_MAX != STB_NO_TIME) tmax = MAX (tmax, ptdebug->CKORGDN_MAX);

    for (ptchrono = ptdebug->CHRONO; ptchrono; ptchrono = ptchrono->NEXT) {
        if (ptchrono->SIG1S) {
            periodsave = XtasPeriod;
            if (ptdebug->CKORGPERIOD != STB_NO_TIME)
                XtasPeriod = ptdebug->CKORGPERIOD>XtasPeriod?ptdebug->CKORGPERIOD:XtasPeriod;
            if ((!XtasPeriodic(ptchrono->SIG1S)) && (XtasPeriod > 0))
                XtasPeriod = XtasPeriod * (1+ ((ptchrono->SIG1S->U-ptchrono->SIG1S->D)/XtasPeriod));
            tmax = MAX (tmax, ptchrono->SIG1S->U + XtasPeriod);
            XtasPeriod = periodsave;
        }
        if (ptchrono->SIG1S_U) {
            periodsave = XtasPeriod;
            if (ptdebug->CKORGPERIOD != STB_NO_TIME)
                XtasPeriod = ptdebug->CKORGPERIOD>XtasPeriod?ptdebug->CKORGPERIOD:XtasPeriod;
            if ((!XtasPeriodic(ptchrono->SIG1S_U)) && (XtasPeriod > 0)) 
                XtasPeriod = XtasPeriod * (1+ ((ptchrono->SIG1S_U->U-ptchrono->SIG1S_U->D)/XtasPeriod));
            tmax = MAX (tmax, ptchrono->SIG1S_U->U + XtasPeriod);
            XtasPeriod = periodsave;
        }
        if (ptchrono->SIG1S_D) {
            periodsave = XtasPeriod;
            if (ptdebug->CKORGPERIOD != STB_NO_TIME)
                XtasPeriod = ptdebug->CKORGPERIOD>XtasPeriod?ptdebug->CKORGPERIOD:XtasPeriod;
            if ((!XtasPeriodic(ptchrono->SIG1S_D)) && (XtasPeriod > 0))
                XtasPeriod = XtasPeriod * (1+ ((ptchrono->SIG1S_D->U-ptchrono->SIG1S_D->D)/XtasPeriod));
            tmax = MAX (tmax, ptchrono->SIG1S_D->U + XtasPeriod);
            XtasPeriod = periodsave;
        }
        if (ptchrono->SIG2S) {
            periodsave = XtasPeriod;
            if ((!XtasPeriodic(ptchrono->SIG2S)) && (XtasPeriod > 0))
                XtasPeriod = XtasPeriod * (1+ ((ptchrono->SIG2S->U-ptchrono->SIG2S->D)/XtasPeriod));
            tmax = MAX (tmax, ptchrono->SIG2S->U + XtasPeriod);
            XtasPeriod = periodsave;
        }
        if (ptchrono->SIG2S_U) {
            periodsave = XtasPeriod;
            if ((!XtasPeriodic(ptchrono->SIG2S_U)) && (XtasPeriod > 0))
                XtasPeriod = XtasPeriod * (1+ ((ptchrono->SIG2S_U->U-ptchrono->SIG2S_U->D)/XtasPeriod));
            tmax = MAX (tmax, ptchrono->SIG2S_U->U + XtasPeriod);
            XtasPeriod = periodsave;
        }
        if (ptchrono->SIG2S_D) {
            periodsave = XtasPeriod;
            if ((!XtasPeriodic(ptchrono->SIG2S_D)) && (XtasPeriod > 0))
                XtasPeriod = XtasPeriod * (1+ ((ptchrono->SIG2S_D->U-ptchrono->SIG2S_D->D)/XtasPeriod));
            tmax = MAX (tmax, ptchrono->SIG2S_D->U + XtasPeriod);
            XtasPeriod = periodsave;
        }
        if (ptchrono->SPECS) 
            tmax = MAX (tmax, ptchrono->SPECS->U + XtasPeriod);
        if (ptchrono->SPECS_U) 
            tmax = MAX (tmax, ptchrono->SPECS_U->U + XtasPeriod);
        if (ptchrono->SPECS_D) 
            tmax = MAX (tmax, ptchrono->SPECS_D->U + XtasPeriod);
    }


    return tmax;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasFault                                                      */
/*                                                                           */
/* IN  ARGS : hpair : interval                                               */
/*                                                                           */
/* OUT ARGS : interval + 1 period                                            */
/*                                                                           */
/* OBJECT   : adds a period duration to an interval                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/

stbpair_list *XtasFault(pairlist)
    stbpair_list *pairlist ;
{
    if (pairlist)
        pairlist->D -= XtasPeriod ;
    return (pairlist) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasStretch                                                    */
/*                                                                           */
/* IN  ARGS : pairlist : interval list                                       */
/*                                                                           */
/* OUT ARGS : interval list                                                  */
/*                                                                           */
/* OBJECT   : puts the first interval at the end of the list                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/

stbpair_list *XtasStretch(pairlist)
    stbpair_list *pairlist ;
{
    stbpair_list *ptpair = NULL ;
    stbpair_list *qpair ;
    stbpair_list *newpair ;
    
    if (!pairlist)
        return (NULL) ;
    
    newpair = stb_dupstbpairlist(pairlist) ; 
    for (ptpair = newpair ; ptpair ; ptpair = ptpair->NEXT)
        qpair = ptpair ;

    qpair->NEXT = stb_addstbpair(NULL, pairlist->D + XtasPeriod, pairlist->U + XtasPeriod) ;
    ptpair = stb_delstbpair(newpair, newpair) ;

    return (ptpair) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDrawSignalChrono                                           */
/*                                                                           */
/* IN  ARGS : signame : signal name                                          */
/*            stbpair : list of intervals                                    */
/*            sigtype : input, output, specout                               */
/*                                                                           */
/* OBJECT   : Draws a chrono for one signal                                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void XtasDrawSignalChrono(signame, stbpair, sigtype, slope)
    char         *signame ;
    stbpair_list *stbpair ;
    char          sigtype ;
    char          slope   ;
{
    long           lastu ;
    stbpair_list *ptstbpair ;
    
    if (stbpair) {
        lastu = -LONG_MAX ;
        switch (sigtype) {
            case XTAS_INPUT : 
                XtasDrawText(XtasColor[GOOD_CHRONO][SOLID], signame, XtasTextHOffset, XtasVerticalPos) ;
                break ;
            case XTAS_INPUT | XTAS_ERROR : 
                XtasDrawText(XtasColor[ORIGIN_CHRONO][SOLID], signame, XtasTextHOffset, XtasVerticalPos) ;
                break ;
            case XTAS_OUTPUT : 
                XtasDrawText(XtasColor[GOOD_CHRONO][SOLID], signame, XtasTextHOffset, XtasVerticalPos) ;
                break ;
            case XTAS_OUTPUT | XTAS_ERROR : 
                XtasDrawText(XtasColor[ERROR_CHRONO][SOLID], signame, XtasTextHOffset, XtasVerticalPos) ;
                break ;
            case XTAS_SPECOUT : 
                XtasDrawText(XtasColor[SPECOUT][SOLID], signame, XtasTextHOffset, XtasVerticalPos) ;
                break ;
        }
        if (XtasPeriodic(stbpair)) {
            for (ptstbpair = stbpair ; ptstbpair ; ptstbpair = ptstbpair->NEXT) {
                if (lastu != -LONG_MAX)  
                    XtasDrawPeriodicInterval(lastu, 
                                             ptstbpair->D, 
                                             XTAS_STABLE | sigtype | XTAS_PERIODIC,
                                             slope) ;
                XtasDrawPeriodicInterval(ptstbpair->D, 
                                         ptstbpair->U, 
                                         XTAS_UNSTABLE | sigtype | XTAS_PERIODIC, 
                                         slope) ;
                lastu = ptstbpair->U ;
            }
            XtasDrawPeriodicInterval(lastu - XtasPeriod, 
                                     stbpair->D, 
                                     XTAS_STABLE | sigtype | XTAS_PERIODIC,
                                     slope) ;
        } else {
            for (ptstbpair = stbpair ; ptstbpair ; ptstbpair = ptstbpair->NEXT) {
                if (lastu != -LONG_MAX)  
                    XtasDrawPeriodicInterval(lastu, 
                                             ptstbpair->D, 
                                             XTAS_STABLE | sigtype | XTAS_UNPERIODIC,
                                             slope) ;
                XtasDrawPeriodicInterval(ptstbpair->D, 
                                         ptstbpair->U, 
                                         XTAS_UNSTABLE | sigtype | XTAS_UNPERIODIC, 
                                         slope) ;
                lastu = ptstbpair->U ;
            }
        }
    }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDrawChronoCallback                                         */
/*                                                                           */
/* IN  ARGS : w                                                              */
/*            client_data                                                    */
/*            call_data   : selected signal                                  */
/*                                                                           */
/* OBJECT   : Draws the complete chrono                                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void XtasDrawChrono()
{
    long                   lastu ;
    long                  offset ;
    stbdebug_list        *ptdebug ;
    stbchrono_list       *ptchrono ;
    stbpair_list         *ptstbpair ;
    stbpair_list         *sig1slist = NULL ;
    stbpair_list         *sig2slist = NULL ;
    stbpair_list         *specslist = NULL ;
    char                  buf[1024] ;
    char                  name1[1024] ;
    char                  name2[1024] ;
    long                  periodsave;
    int setupdir, holddir;
       
    
    ptdebug = XtasDebug ;
    
    if (ptdebug->detail[0].setup.VALUE==ptdebug->SETUP) setupdir=0; else setupdir=1;
    if (ptdebug->detail[0].hold.VALUE==ptdebug->HOLD) holddir=0; else holddir=1;

    if(XtasDebugOtherSigName != NULL)
      mbkfree(XtasDebugOtherSigName) ;

    ttv_getsigname(XtasMainParam->ttvfig,buf,ptdebug->SIG1) ;
    ttv_getsigname(XtasMainParam->ttvfig,name1,ptdebug->SIG1) ;
    ttv_getsigname(XtasMainParam->ttvfig,name2,ptdebug->SIG2) ;
    XtasDebugOtherSigName = mbkalloc(strlen(buf)+1) ;
    strcpy(XtasDebugOtherSigName,buf) ;
    
    
    XtasClearGraphicWindow() ;
    XtasDrawGrid() ;
    XtasRefreshGraphicWindow() ;
    XtasVerticalPos = 30 ;
    XtasInitValTab() ;
    
    /* CLOCK REF */    
    if (ptdebug->CKREFUP_MIN != STB_NO_TIME && ptdebug->CKREFDN_MIN != STB_NO_TIME
     && ptdebug->CKREFUP_MAX != STB_NO_TIME && ptdebug->CKREFDN_MAX != STB_NO_TIME) {

        XtasDrawClock (ptdebug->CKREFUP_MIN, ptdebug->CKREFDN_MIN, XtasPeriod);
        XtasDrawClock (ptdebug->CKREFUP_MAX, ptdebug->CKREFDN_MAX, XtasPeriod);

        if (ptdebug->CKNAME)
            XtasDrawText(XtasColor[CLOCK][SOLID], ptdebug->CKNAME, XtasTextHOffset, XtasVerticalPos) ;
        else
            XtasDrawText(XtasColor[CLOCK][SOLID], "ck_ref", XtasTextHOffset, XtasVerticalPos) ;

        XtasVerticalPos += XtasInterSig ;
        XtasDrawSeparator() ;
        XtasVerticalPos += XtasInterSig ;
    }

    XtasDrawSH(ptdebug) ;    

    for (ptchrono = ptdebug->CHRONO ; ptchrono ; ptchrono = ptchrono->NEXT) {
        

        /* CLOCK */
        /*
        if (!(ptdebug->SIG1->TYPE & TTV_SIG_C)) {
            if (ptdebug->CKORGUP_MIN != STB_NO_TIME && ptdebug->CKORGDN_MIN != STB_NO_TIME
             && ptdebug->CKORGUP_MAX != STB_NO_TIME && ptdebug->CKORGDN_MAX != STB_NO_TIME) {
                if (ptdebug->CKORGPERIOD != STB_NO_TIME) {
                    XtasDrawClock(ptdebug->CKORGUP_MIN, ptdebug->CKORGDN_MIN, ptdebug->CKORGPERIOD) ;
                    XtasDrawClock(ptdebug->CKORGUP_MAX, ptdebug->CKORGDN_MAX, ptdebug->CKORGPERIOD) ;
                }
                else {
                    XtasDrawClock(ptdebug->CKORGUP_MIN, ptdebug->CKORGDN_MIN, XtasPeriod) ;
                    XtasDrawClock(ptdebug->CKORGUP_MAX, ptdebug->CKORGDN_MAX, XtasPeriod) ;
                }
                XtasVerticalPos += XtasInterSig ;
            }
        }
        */
        
        /* ORIGIN */
        if (ptchrono->SIG1S && ((XtasDebugChronoType & XTAS_DEBUG_IUD) == XTAS_DEBUG_IUD)) {
            periodsave = XtasPeriod;
            if (ptdebug->CKORGPERIOD != STB_NO_TIME)
                XtasPeriod = ptdebug->CKORGPERIOD;
            sig1slist = XtasStretch(ptchrono->SIG1S) ;
            if ((!XtasPeriodic(ptchrono->SIG1S)) && (XtasPeriod > 0))
                XtasPeriod = XtasPeriod * (1+ ((ptchrono->SIG1S->U-ptchrono->SIG1S->D)/XtasPeriod));
            if (ptdebug->SETUP > 0 && ptdebug->HOLD > 0) {
                XtasDrawSignalChrono(name1, sig1slist, XTAS_INPUT, STB_UP | STB_DN) ;
                XtasVerticalPos += XtasInterSig ;
            } else {
                XtasDrawSignalChrono(name1, sig1slist, XTAS_INPUT | XTAS_ERROR, STB_UP | STB_DN) ;
                XtasVerticalPos += XtasInterSig ;
            }
            XtasPeriod = periodsave;
        }

        if (ptchrono->SIG1S_U && ((XtasDebugChronoType & XTAS_DEBUG_IU) == XTAS_DEBUG_IU)) {
            periodsave = XtasPeriod;
            if (ptdebug->CKORGPERIOD != STB_NO_TIME)
                XtasPeriod = ptdebug->CKORGPERIOD;
            sig1slist = XtasStretch(ptchrono->SIG1S_U) ;
            if ((!XtasPeriodic(ptchrono->SIG1S_U)) && (XtasPeriod > 0))
                XtasPeriod = XtasPeriod * (1+ ((ptchrono->SIG1S_U->U-ptchrono->SIG1S_U->D)/XtasPeriod));
            if (ptdebug->SETUP > 0 && ptdebug->HOLD > 0) {
                XtasDrawSignalChrono(name1, sig1slist, XTAS_INPUT, STB_UP) ;
                XtasVerticalPos += XtasInterSig ;
            } else {
                XtasDrawSignalChrono(name1, sig1slist, XTAS_INPUT | XTAS_ERROR, STB_UP) ;
                XtasVerticalPos += XtasInterSig ;
            }
            XtasPeriod = periodsave;
        }

        if (ptchrono->SIG1S_D && ((XtasDebugChronoType & XTAS_DEBUG_ID) == XTAS_DEBUG_ID)) {
            periodsave = XtasPeriod;
            if (ptdebug->CKORGPERIOD != STB_NO_TIME)
                XtasPeriod = ptdebug->CKORGPERIOD;
            sig1slist = XtasStretch(ptchrono->SIG1S_D) ;
            if ((!XtasPeriodic(ptchrono->SIG1S_D)) && (XtasPeriod > 0))
                XtasPeriod = XtasPeriod * (1+ ((ptchrono->SIG1S_D->U-ptchrono->SIG1S_D->D)/XtasPeriod));
            if (ptdebug->SETUP > 0 && ptdebug->HOLD > 0) {
                XtasDrawSignalChrono(name1, sig1slist, XTAS_INPUT, STB_DN) ;
                XtasVerticalPos += XtasInterSig ;
            } else {
                XtasDrawSignalChrono(name1, sig1slist, XTAS_INPUT | XTAS_ERROR, STB_DN) ;
                XtasVerticalPos += XtasInterSig ;
            }
            XtasPeriod = periodsave;
        }

        /* VERIF CLOCK */
        if (ptchrono->TYPE) {
            if ((ptchrono->VERIF & STB_VERIF_STATE) && (ptchrono->VERIF & STB_UP)) 
               XtasDrawStateRep(ptchrono->CKUP_MIN, 
                                ptchrono->CKDN_MAX < ptchrono->CKUP_MIN ? ptchrono->CKDN_MAX+XtasPeriod:ptchrono->CKDN_MAX,
                                XtasVerticalPos - XtasInterSig / 2, 
                                XtasVerticalPos + XtasInterSig + XtasInterSig / 2,
                                ptdebug->MARGESETUP-ptdebug->detail[setupdir].setup.skew+ptdebug->detail[setupdir].setup.data_margin-ptdebug->detail[setupdir].setup.clock_margin,
                                ptdebug->MARGEHOLD-ptdebug->detail[holddir].hold.skew-ptdebug->detail[holddir].hold.data_margin+ptdebug->detail[holddir].hold.clock_margin) ;
            if ((ptchrono->VERIF & STB_VERIF_STATE) && (ptchrono->VERIF & STB_DN)) 
               XtasDrawStateRep(ptchrono->CKDN_MIN, 
                                ptchrono->CKUP_MAX < ptchrono->CKDN_MIN ? ptchrono->CKUP_MAX+XtasPeriod:ptchrono->CKUP_MAX,
                                XtasVerticalPos - XtasInterSig / 2, 
                                XtasVerticalPos + XtasInterSig + XtasInterSig / 2,
                                ptdebug->MARGESETUP-ptdebug->detail[setupdir].setup.skew+ptdebug->detail[setupdir].setup.data_margin-ptdebug->detail[setupdir].setup.clock_margin,
                                ptdebug->MARGEHOLD-ptdebug->detail[holddir].hold.skew-ptdebug->detail[holddir].hold.data_margin+ptdebug->detail[holddir].hold.clock_margin) ;

            if ((ptchrono->VERIF & STB_VERIF_EDGE) && (ptchrono->VERIF & STB_UP)) 
               XtasDrawEdgeRep(ptchrono->CKUP_MIN, 
                               XtasVerticalPos - XtasInterSig / 2, 
                               XtasVerticalPos + XtasInterSig + XtasInterSig / 2,
                               ptdebug->MARGESETUP-ptdebug->detail[setupdir].setup.skew+ptdebug->detail[setupdir].setup.data_margin-ptdebug->detail[setupdir].setup.clock_margin,
                               ptdebug->MARGEHOLD-ptdebug->detail[holddir].hold.skew-ptdebug->detail[holddir].hold.data_margin+ptdebug->detail[holddir].hold.clock_margin, 
                               ptdebug->HOLDPERIOD) ;
            if ((ptchrono->VERIF & STB_VERIF_EDGE) && (ptchrono->VERIF & STB_DN)) 
               XtasDrawEdgeRep(ptchrono->CKDN_MIN, 
                               XtasVerticalPos - XtasInterSig / 2, 
                               XtasVerticalPos + XtasInterSig + XtasInterSig / 2,
                               ptdebug->MARGESETUP-ptdebug->detail[setupdir].setup.skew+ptdebug->detail[setupdir].setup.data_margin-ptdebug->detail[setupdir].setup.clock_margin,
                               ptdebug->MARGEHOLD-ptdebug->detail[holddir].hold.skew-ptdebug->detail[holddir].hold.data_margin+ptdebug->detail[holddir].hold.clock_margin,
                               ptdebug->HOLDPERIOD) ;
        }

        
        /* CLOCK */
        if (ptchrono->CKUP_MIN != STB_NO_TIME && ptchrono->CKDN_MIN != STB_NO_TIME
         && ptchrono->CKUP_MAX != STB_NO_TIME && ptchrono->CKDN_MAX != STB_NO_TIME) {
            if (ptdebug->CMD_EVENT) {
                if ((ptdebug->CMD_EVENT->TYPE & TTV_NODE_UP) && (ptchrono->VERIF & STB_UP)) {
                    XtasDrawClock(ptchrono->CKDN_MIN, ptchrono->CKUP_MIN, XtasPeriod) ;
                    XtasDrawClock(ptchrono->CKDN_MAX, ptchrono->CKUP_MAX, XtasPeriod) ;
                }
                else
                if ((ptdebug->CMD_EVENT->TYPE & TTV_NODE_DOWN) && (ptchrono->VERIF & STB_DN)) {
                    XtasDrawClock(ptchrono->CKDN_MIN, ptchrono->CKUP_MIN, XtasPeriod) ;
                    XtasDrawClock(ptchrono->CKDN_MAX, ptchrono->CKUP_MAX, XtasPeriod) ;
                }
                else
                if ((ptdebug->CMD_EVENT->TYPE & TTV_NODE_DOWN) && (ptchrono->VERIF & STB_UP)) {
                    XtasDrawClock(ptchrono->CKUP_MIN, ptchrono->CKDN_MIN, XtasPeriod) ;
                    XtasDrawClock(ptchrono->CKUP_MAX, ptchrono->CKDN_MAX, XtasPeriod) ;
                }
                else
                if ((ptdebug->CMD_EVENT->TYPE & TTV_NODE_UP) && (ptchrono->VERIF & STB_DN)) {
                    XtasDrawClock(ptchrono->CKUP_MIN, ptchrono->CKDN_MIN, XtasPeriod) ;
                    XtasDrawClock(ptchrono->CKUP_MAX, ptchrono->CKDN_MAX, XtasPeriod) ;
                }
            }
            if (ptdebug->CMDNAME) XtasDrawText(XtasColor[CLOCK][SOLID], ptdebug->CMDNAME, XtasTextHOffset, XtasVerticalPos) ;
            XtasVerticalPos += XtasInterSig ;
        }
        
        if ((XtasDebugChronoType & XTAS_DEBUG_OUD) == XTAS_DEBUG_OUD) {
        /* VERIF SPECOUT */
            if (ptchrono->SPECS) 
                specslist = XtasStretch(ptchrono->SPECS) ;
            if (!ptchrono->TYPE) {
                if (ptchrono->SPECS) {
                    for (ptstbpair = specslist ; ptstbpair ; ptstbpair = ptstbpair->NEXT) {
                        lastu = -LONG_MAX ;
                        if (lastu != -LONG_MAX) 
                            XtasDrawStateRep(lastu, 
                                             ptstbpair->D,
                                             XtasVerticalPos - XtasInterSig / 2, 
                                             XtasVerticalPos + XtasInterSig + XtasInterSig / 2,
                                             ptdebug->MARGESETUP-ptdebug->detail[setupdir].setup.skew+ptdebug->detail[setupdir].setup.data_margin-ptdebug->detail[setupdir].setup.clock_margin,
                                             ptdebug->MARGEHOLD-ptdebug->detail[holddir].hold.skew-ptdebug->detail[holddir].hold.data_margin+ptdebug->detail[holddir].hold.clock_margin) ;
                        lastu = ptstbpair->U ;
                    }
                    XtasDrawStateRep(lastu - XtasPeriod, 
                                     specslist->D, 
                                     XtasVerticalPos - XtasInterSig / 2, 
                                     XtasVerticalPos + XtasInterSig + XtasInterSig / 2,
                                     ptdebug->MARGESETUP-ptdebug->detail[setupdir].setup.skew+ptdebug->detail[setupdir].setup.data_margin-ptdebug->detail[setupdir].setup.clock_margin,
                                     ptdebug->MARGEHOLD-ptdebug->detail[holddir].hold.skew-ptdebug->detail[holddir].hold.data_margin+ptdebug->detail[holddir].hold.clock_margin) ;
                }
            }
        
            /* ERROR */
            if (ptchrono->SIG2S) {
                periodsave = XtasPeriod;
                if ((!XtasPeriodic(ptchrono->SIG2S)) && (XtasPeriod > 0))
                    XtasPeriod = XtasPeriod * (1+ ((ptchrono->SIG2S->U-ptchrono->SIG2S->D)/XtasPeriod));
                sig2slist = XtasStretch(ptchrono->SIG2S) ;
                if (ptdebug->SETUP > 0 && ptdebug->HOLD > 0) {
                    XtasDrawSignalChrono(name2, sig2slist, XTAS_OUTPUT, ptchrono->SLOPE) ;
                    XtasVerticalPos += XtasInterSig ;
                } else {
                    XtasDrawSignalChrono(name2, sig2slist, XTAS_OUTPUT | XTAS_ERROR, ptchrono->SLOPE);
                    XtasVerticalPos += XtasInterSig ;
                }
                XtasPeriod = periodsave;
            }

            /* SPECOUT */
            if (ptchrono->SPECS) {
                XtasDrawSignalChrono(strdup("set_output_delay"), specslist, XTAS_SPECOUT, STB_UP | STB_DN) ;
                XtasVerticalPos += XtasInterSig ;
            }
        }
        
        if ((XtasDebugChronoType & XTAS_DEBUG_OU) == XTAS_DEBUG_OU) {
            /* VERIF SPECOUT */
            if (ptchrono->SPECS_U) 
                specslist = XtasStretch(ptchrono->SPECS_U) ;
            if (!ptchrono->TYPE) {
                if (ptchrono->SPECS_U) {
                    for (ptstbpair = specslist ; ptstbpair ; ptstbpair = ptstbpair->NEXT) {
                        lastu = -LONG_MAX ;
                        if (lastu != -LONG_MAX) 
                            XtasDrawStateRep(lastu, 
                                             ptstbpair->D,
                                             XtasVerticalPos - XtasInterSig / 2, 
                                             XtasVerticalPos + XtasInterSig + XtasInterSig / 2,
                                             ptdebug->MARGESETUP,
                                             ptdebug->MARGEHOLD) ;
                        lastu = ptstbpair->U ;  
                    }
                    XtasDrawStateRep(lastu - XtasPeriod, 
                                     specslist->D, 
                                     XtasVerticalPos - XtasInterSig / 2, 
                                     XtasVerticalPos + XtasInterSig + XtasInterSig / 2,
                                     ptdebug->MARGESETUP,
                                     ptdebug->MARGEHOLD) ;
                }   
            }
            
            /* ERROR */
            if (ptchrono->SIG2S_U) {
                periodsave = XtasPeriod;
                if ((!XtasPeriodic(ptchrono->SIG2S_U)) && (XtasPeriod > 0))
                    XtasPeriod = XtasPeriod * (1+ ((ptchrono->SIG2S_U->U-ptchrono->SIG2S_U->D)/XtasPeriod));
                sig2slist = XtasStretch(ptchrono->SIG2S_U) ;
                if (ptdebug->SETUP > 0 && ptdebug->HOLD > 0) {
                    XtasDrawSignalChrono(name2, sig2slist, XTAS_OUTPUT, STB_UP) ;
                    XtasVerticalPos += XtasInterSig ;
                } else {
                    XtasDrawSignalChrono(name2, sig2slist, XTAS_OUTPUT | XTAS_ERROR, STB_UP) ;
                    XtasVerticalPos += XtasInterSig ;
                }
                XtasPeriod = periodsave;
            }
    
            /* SPECOUT */
            if (ptchrono->SPECS_U) {
                XtasDrawSignalChrono(strdup("set_output_delay"), specslist, XTAS_SPECOUT, STB_UP | STB_DN) ;
                XtasVerticalPos += XtasInterSig ;
            }
        }
        
        if ((XtasDebugChronoType & XTAS_DEBUG_OD) && XTAS_DEBUG_OD) {
            /* VERIF SPECOUT */
            if (ptchrono->SPECS_D) 
                specslist = XtasStretch(ptchrono->SPECS_D) ;
            if (!ptchrono->TYPE) {
                if (ptchrono->SPECS_D) {
                    for (ptstbpair = specslist ; ptstbpair ; ptstbpair = ptstbpair->NEXT) {
                        lastu = -LONG_MAX ;
                        if (lastu != -LONG_MAX) 
                            XtasDrawStateRep(lastu, 
                                             ptstbpair->D,
                                             XtasVerticalPos - XtasInterSig / 2, 
                                             XtasVerticalPos + XtasInterSig + XtasInterSig / 2,
                                             ptdebug->MARGESETUP,
                                             ptdebug->MARGEHOLD) ;
                        lastu = ptstbpair->U ;
                    }
                    XtasDrawStateRep(lastu - XtasPeriod, 
                                     specslist->D, 
                                     XtasVerticalPos - XtasInterSig / 2, 
                                     XtasVerticalPos + XtasInterSig + XtasInterSig / 2,
                                     ptdebug->MARGESETUP,
                                     ptdebug->MARGEHOLD) ;
                }   
            }
            /* ERROR */
            if (ptchrono->SIG2S_D) {
                periodsave = XtasPeriod;
                if ((!XtasPeriodic(ptchrono->SIG2S_D)) && (XtasPeriod > 0))
                    XtasPeriod = XtasPeriod * (1+ ((ptchrono->SIG2S_D->U-ptchrono->SIG2S_D->D)/XtasPeriod));
                sig2slist = XtasStretch(ptchrono->SIG2S_D) ;
                if (ptdebug->SETUP > 0 && ptdebug->HOLD > 0) {
                    XtasDrawSignalChrono(name2, sig2slist, XTAS_OUTPUT, STB_DN) ;
                    XtasVerticalPos += XtasInterSig ;
                } else {
                    XtasDrawSignalChrono(name2, sig2slist, XTAS_OUTPUT | XTAS_ERROR, STB_DN) ;
                    XtasVerticalPos += XtasInterSig ;
                }
                XtasPeriod = periodsave;
            }

        
            /* SPECOUT */
            if (ptchrono->SPECS_D) {
                XtasDrawSignalChrono(strdup("set_output_delay"), specslist, XTAS_SPECOUT, STB_DN) ;
                XtasVerticalPos += XtasInterSig ;
            }   
        }    
        
        XtasDrawSeparator() ;
        XtasVerticalPos += XtasInterSig ;
    }
    
    XtasRefreshGraphicWindow() ;
    stb_freestbpair(sig1slist) ;
    stb_freestbpair(sig2slist) ;
    stb_freestbpair(specslist) ;
    XtasDraw = True ;
}

void XtasDrawChronoCallback(w, client_data, call_data)
    Widget    w;
    XtPointer client_data;
    XtPointer call_data;
{
    int                   i ;
    long                  l ;
    XmListCallbackStruct *cbs ;
    
    cbs = (XmListCallbackStruct*)call_data ;
    XtasDebug = XtasDebugList ;
    
    if (XtasDebug && cbs) {
        for (i = 1 ; i < cbs->item_position ; i++) 
            XtasDebug = XtasDebug->NEXT ;
        
        XtasFreeZoomContext() ;
        XtasPeriod = XtasDebug->PERIODE ;
        XtasTmin = XtasFindTmin(XtasDebug) /*- XtasPeriod*/;
        XtasTmax = XtasFindTmax(XtasDebug) /*+ XtasPeriod*/;

        l = XtasTmax - XtasTmin;

        XtasTmin = XtasTmin - (l/10);
        XtasTmax = XtasTmax + (l/10);
        
        if (XtasTmin!=LONG_MAX && XtasTmax!=LONG_MIN) XtasDrawChrono () ;
        XtasDraw = True ;
    }
}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasRefreshCallback                                            */
/*                                                                           */
/* IN  ARGS : w                                                              */
/*            client_data                                                    */
/*            call_data                                                      */
/*                                                                           */
/* OBJECT   : Refresh the graphical zone                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void XtasRefreshCallback(w, client_data, call_data)
    Widget                w ;
    XtPointer             client_data ;
    XtPointer             call_data ;
{
    if (XtasClear) {
        XtasClearGraphicWindow() ;
        XtasTextHOffset       = 5 ;
        XtasTextVOffset       = 14 ;
        XtasInterSig          = 50 ;
        XtasVerticalPos       = XtasInterSig ;
        XtasHSlope            = 5 ; 
        XtasVSlope            = 10 ; 
        XtasPeriod            = 642 ;
        XtasTmin              = 80 - 642;
        XtasTmax              = 350 + 642;
        XtasNbGridDivisions   = 7 ;
        XtasClear             = False ;
        XtasDrawGrid() ;
    }
    XtasRefreshGraphicWindow() ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasInitPatterns                                               */
/*                                                                           */
/* OBJECT   : Init patterns for filling                                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void XtasInitPatterns()
{
    int i ;
    for (i = 0 ; i < XTAS_MAX_PATTERN ; i++) {
        XtasPattern[i]  = XCreateBitmapFromData( XtasGraphicDisplay,
                                                 RootWindowOfScreen(XtScreen(XtasGraphicWindow)),
                                                 XtasPatternBits[i],
                                                 XTAS_PATTERN_WIDTH,
                                                 XTAS_PATTERN_HEIGHT ); 
    }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : draw_cbk                                                       */
/*                                                                           */
/* OBJECT   : Init graphics                                                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void draw_cbk(w, event, args, num_args) 
    Widget        w ;
    XButtonEvent *event ;
    String       *args ;
    int          *num_args ;
{

    static int xorg ;
    static float      x1, xg ;
    int        xdest ;
    static float      x2, xd ;
    char       x1str[1024] ;
    char       x2str[1024] ;
    char       delta[1024] ;
    char        mode[1024] ;
    static Boolean motion ;
    XmString   text;

    
    if (XtasDraw) {
        XtasPutZoomContext(XtasTmin, XtasTmax) ; 
        /* zoom */
        if (!strcmp (args[0], "down3")) {
            XtasGetPrevZoomContext() ;
            XtasDrawChrono() ;
        }

        if (!strcmp (args[0], "down2")) {
            if (event->x > XtasTextZoneWidth && event->x < XtasTextZoneWidth + XtasGraphicZoneWidth) {
                xorg = XtasGetValFromTab ((event->x - XtasTextZoneWidth) * 1000) / 1000 + XtasTextZoneWidth;
                xg = (float)XtasGetValFromTab ((event->x - XtasTextZoneWidth) * 1000) / 1000 + XtasTextZoneWidth;
                x1 = XTAS_UNSCALE (xg - XtasTextZoneWidth, 
                                   XtasTmin, XtasTmax, 
                                   XtasGraphicZoneWidth) ;
            }
            if (event->x <= XtasTextZoneWidth) {
                xorg = XtasTextZoneWidth ;
                x1 = XTAS_UNSCALE (xorg - XtasTextZoneWidth, 
                                   XtasTmin, 
                                   XtasTmax, XtasGraphicZoneWidth) ;
            }
            if (event->x >= XtasTextZoneWidth + XtasGraphicZoneWidth) {
                xorg = XtasTextZoneWidth + XtasGraphicZoneWidth ;
                x1 = XTAS_UNSCALE (xorg - XtasTextZoneWidth, 
                                   XtasTmin, 
                                   XtasTmax, XtasGraphicZoneWidth) ;
            }

             XCopyArea(XtasGraphicDisplay, 
                   XtasSHPixmap,
                   XtasSaveSHPixmap, 
                   XtasColor[BACKGROUND][SOLID], 
                   0, 
                   0, 
                   XtasTextZoneWidth, 
                   XtasTextZoneHeight, 
                   0, 
                   0) ;  
             XCopyArea(XtasGraphicDisplay, 
                   XtasGraphicPixmap,
                   XtasSaveGraphicPixmap, 
                   XtasColor[BACKGROUND][SOLID], 
                   0, 
                   0, 
                   XtasGraphicZoneWidth, 
                   XtasGraphicZoneHeight, 
                   0, 
                   0) ;  
            
            motion = False ;
            sprintf (mode, "ZOOM:") ;
//            XDrawString(XtasGraphicDisplay, XtasSHPixmap, XtasColor[ZOOM][SOLID], 10, 180, mode, strlen (mode));
            XtasSetLabelString (XtasDebugLabelMode, mode);
            sprintf (x1str, "t1 = %ld ps", XtasRound(x1)) ;
//            XDrawString(XtasGraphicDisplay, XtasSHPixmap, XtasColor[ZOOM][SOLID], 10, 200, x1str, strlen (x1str));
            XtasSetLabelString (XtasDebugLabelT1, x1str);
            XDrawLine (XtasGraphicDisplay, XtasGraphicPixmap, XtasColor[ERROR_ZOOM][SOLID],
                    xorg - XtasTextZoneWidth, 0,
                    xorg - XtasTextZoneWidth, XtasGraphicZoneHeight) ;
            XtasRefreshGraphicZone () ;
            XtasRefreshSHZone () ;
        }
        
        if (!strcmp (args[0], "up2") ) {
             XCopyArea(XtasGraphicDisplay, 
                   XtasSaveGraphicPixmap,
                   XtasGraphicPixmap, 
                   XtasColor[BACKGROUND][SOLID], 
                   0, 
                   0, 
                   XtasGraphicZoneWidth, 
                   XtasGraphicZoneHeight, 
                   0, 
                   0) ;  
             XCopyArea(XtasGraphicDisplay, 
                   XtasSaveSHPixmap,
                   XtasSHPixmap, 
                   XtasColor[BACKGROUND][SOLID], 
                   0, 
                   0, 
                   XtasTextZoneWidth, 
                   XtasTextZoneHeight, 
                   0, 
                   0) ;  
            XtasRefreshGraphicZone () ;
            XtasRefreshSHZone () ;
            XtasSetLabelString (XtasDebugLabelMode, " ");
            XtasSetLabelString (XtasDebugLabelT1,   " ");
            XtasSetLabelString (XtasDebugLabelT2,   " ");
            XtasSetLabelString (XtasDebugLabelDelta," ");

            if((XtasRound(x1) != XtasRound(x2)) && (motion == True)) {
                motion = False ; 
                if((XtasComputeGridStep(XtasRound(x2) - XtasRound(x1)) > 0) || 
                  ((XtasComputeGridStep(XtasRound(x1) - XtasRound(x2)) > 0))) {
                   if(XtasRound(x1) < XtasRound(x2)) {
                       XtasTmin = XtasRound(x1) ;
                       XtasTmax = XtasRound(x2) ;
                   }
                   else {
                       XtasTmin = XtasRound(x2) ;
                       XtasTmax = XtasRound(x1) ;
                   }
                   XtasPutZoomContext(XtasTmin, XtasTmax) ;
                   XtasDrawChrono() ;
                }
            }
        }
        if (!strcmp (args[0], "motion2")) {
             XCopyArea(XtasGraphicDisplay, 
                   XtasSaveSHPixmap,
                   XtasSHPixmap, 
                   XtasColor[BACKGROUND][SOLID], 
                   0, 
                   0, 
                   XtasTextZoneWidth, 
                   XtasTextZoneHeight, 
                   0, 
                   0) ;  
             XCopyArea(XtasGraphicDisplay, 
                   XtasSaveGraphicPixmap, 
                   XtasGraphicPixmap,
                   XtasColor[BACKGROUND][SOLID], 
                   0, 
                   0, 
                   XtasGraphicZoneWidth, 
                   XtasGraphicZoneHeight, 
                   0, 
                   0);  
            
            if (event->x <= XtasTextZoneWidth) {
                xdest = XtasTextZoneWidth ;
                x2 = XTAS_UNSCALE(xdest - XtasTextZoneWidth, 
                                  XtasTmin, 
                                  XtasTmax, XtasGraphicZoneWidth) ;
            }
            if (event->x >= XtasTextZoneWidth + XtasGraphicZoneWidth) {
                xdest = XtasGraphicZoneWidth + XtasTextZoneWidth ;
                x2 = XTAS_UNSCALE(xdest - XtasTextZoneWidth, 
                                  XtasTmin, 
                                  XtasTmax, XtasGraphicZoneWidth) ;
            }
            if (event->x > XtasTextZoneWidth && event->x < XtasTextZoneWidth + XtasGraphicZoneWidth) {
                xdest = XtasGetValFromTab ((event->x - XtasTextZoneWidth) * 1000) / 1000 + XtasTextZoneWidth;
                xd = (float)XtasGetValFromTab ((event->x - XtasTextZoneWidth) * 1000) / 1000 + XtasTextZoneWidth;
                x2 = XTAS_UNSCALE(xd - XtasTextZoneWidth, 
                                  XtasTmin, 
                                  XtasTmax, XtasGraphicZoneWidth) ;
            }
            
            
            motion = True ;
//            sprintf (mode, "ZOOM :") ;
////            XDrawString(XtasGraphicDisplay, XtasSHPixmap, XtasColor[ZOOM][SOLID], 10, 180, mode, strlen (mode));
//            XtasSetLabelString (XtasDebugLabelMode, mode);
//            sprintf (x1str, "t1 = %.1f ps", XtasRound(x1)/TTV_UNIT) ;
////            XDrawString(XtasGraphicDisplay, XtasSHPixmap, XtasColor[ZOOM][SOLID], 10, 200, x1str, strlen (x1str));
//            XtasSetLabelString (XtasDebugLabelT1, x1str);
            sprintf (x2str, "t2 = %.1f ps", XtasRound(x2)/TTV_UNIT) ;
//            XDrawString(XtasGraphicDisplay, XtasSHPixmap, XtasColor[ZOOM][SOLID], 10, 220, x2str, strlen (x2str));
            text = XmStringCreateSimple (x2str);
            XtVaSetValues (XtasDebugLabelT2, XmNlabelString, text,NULL);
            XmStringFree (text);
            XmUpdateDisplay (XtasDebugLabelT2);
            sprintf (delta, "dt = %.1f ps", XtasRound(fabs (x1 - x2))/TTV_UNIT) ;
//            XDrawString(XtasGraphicDisplay, XtasSHPixmap, XtasColor[ZOOM][SOLID], 10, 240, delta, strlen (delta));
            text = XmStringCreateSimple (delta);
            XtVaSetValues (XtasDebugLabelDelta, XmNlabelString, text,NULL);
            XmStringFree (text);
            XmUpdateDisplay (XtasDebugLabelDelta);
    
            if((XtasComputeGridStep(XtasRound(x2) - XtasRound(x1)) > 0) || 
              ((XtasComputeGridStep(XtasRound(x1) - XtasRound(x2)) > 0))) {
                XDrawLine (XtasGraphicDisplay, XtasGraphicPixmap, XtasColor[ZOOM][SOLID],
                       xorg - XtasTextZoneWidth, 0,
                       xorg - XtasTextZoneWidth, XtasGraphicZoneHeight) ;
                XFillRectangle (XtasGraphicDisplay, 
                       XtasGraphicPixmap, 
                       XtasColor[ZOOM][STIPPLE], 
                         MIN (xorg, xdest) - XtasTextZoneWidth, 
                         0, 
                       abs (xdest - xorg), 
                       XtasGraphicZoneHeight) ;
            }
            else {
                XDrawLine (XtasGraphicDisplay, XtasGraphicPixmap, XtasColor[ERROR_ZOOM][SOLID],
                       xorg - XtasTextZoneWidth, 0,
                       xorg - XtasTextZoneWidth, XtasGraphicZoneHeight) ;
                XFillRectangle (XtasGraphicDisplay, 
                       XtasGraphicPixmap, 
                       XtasColor[ERROR_ZOOM][STIPPLE], 
                         MIN (xorg, xdest) - XtasTextZoneWidth, 
                         0, 
                       abs (xdest - xorg), 
                       XtasGraphicZoneHeight) ;
            }
            XtasRefreshGraphicZone() ;
            XtasRefreshSHZone () ;
        }
        /* mesure */
        if (!strcmp (args[0], "down")) {
            if (event->x > XtasTextZoneWidth && event->x < XtasTextZoneWidth + XtasGraphicZoneWidth) {
                xorg = XtasGetValFromTab ((event->x - XtasTextZoneWidth) * 1000) / 1000 + XtasTextZoneWidth;
                xg = (float)XtasGetValFromTab ((event->x - XtasTextZoneWidth) * 1000) / 1000 + XtasTextZoneWidth;
                x1 = XTAS_UNSCALE (xg - XtasTextZoneWidth, 
                                   XtasTmin, XtasTmax, 
                                   XtasGraphicZoneWidth) ;
            }
            if (event->x <= XtasTextZoneWidth) {
                xorg = XtasTextZoneWidth ;
                x1 = XTAS_UNSCALE (xorg - XtasTextZoneWidth, 
                                   XtasTmin, 
                                   XtasTmax, XtasGraphicZoneWidth) ;
            }
            if (event->x >= XtasTextZoneWidth + XtasGraphicZoneWidth) {
                xorg = XtasTextZoneWidth + XtasGraphicZoneWidth ;
                x1 = XTAS_UNSCALE (xorg - XtasTextZoneWidth, 
                                   XtasTmin, 
                                   XtasTmax, XtasGraphicZoneWidth) ;
            }

             XCopyArea(XtasGraphicDisplay, 
                   XtasSHPixmap,
                   XtasSaveSHPixmap, 
                   XtasColor[BACKGROUND][SOLID], 
                   0, 
                   0, 
                   XtasTextZoneWidth, 
                   XtasTextZoneHeight, 
                   0, 
                   0) ;  
             XCopyArea(XtasGraphicDisplay, 
                   XtasGraphicPixmap,
                   XtasSaveGraphicPixmap, 
                   XtasColor[BACKGROUND][SOLID], 
                   0, 
                   0, 
                   XtasGraphicZoneWidth, 
                   XtasGraphicZoneHeight, 
                   0, 
                   0) ;  
            
            sprintf (x1str, "t1 = %.1f ps", XtasRound(x1)/TTV_UNIT) ;
//            XDrawString(XtasGraphicDisplay, XtasSHPixmap, XtasColor[MSETUP][SOLID], 10, 200, x1str, strlen (x1str));
            XtasSetLabelString (XtasDebugLabelT1, x1str);
            XDrawLine (XtasGraphicDisplay, XtasGraphicPixmap, XtasColor[MSETUP][SOLID],
                    xorg - XtasTextZoneWidth, 0,
                    xorg - XtasTextZoneWidth, XtasGraphicZoneHeight) ;
            XtasRefreshGraphicZone () ;
            XtasRefreshSHZone () ;
        }
        
        if (!strcmp (args[0], "up")) {
             XCopyArea(XtasGraphicDisplay, 
                   XtasSaveGraphicPixmap,
                   XtasGraphicPixmap, 
                   XtasColor[BACKGROUND][SOLID], 
                   0, 
                   0, 
                   XtasGraphicZoneWidth, 
                   XtasGraphicZoneHeight, 
                   0, 
                   0) ;  
             XCopyArea(XtasGraphicDisplay, 
                   XtasSaveSHPixmap,
                   XtasSHPixmap, 
                   XtasColor[BACKGROUND][SOLID], 
                   0, 
                   0, 
                   XtasTextZoneWidth, 
                   XtasTextZoneHeight, 
                   0, 
                   0) ;  
            XtasRefreshGraphicZone () ;
            XtasRefreshSHZone () ;
            XtasSetLabelString (XtasDebugLabelMode, " ");
            XtasSetLabelString (XtasDebugLabelT1,   " ");
            XtasSetLabelString (XtasDebugLabelT2,   " ");
            XtasSetLabelString (XtasDebugLabelDelta," ");
        }
        if (!strcmp (args[0], "motion")) {
             XCopyArea(XtasGraphicDisplay, 
                   XtasSaveSHPixmap,
                   XtasSHPixmap, 
                   XtasColor[BACKGROUND][SOLID], 
                   0, 
                   0, 
                   XtasTextZoneWidth, 
                   XtasTextZoneHeight, 
                   0, 
                   0) ;  
             XCopyArea(XtasGraphicDisplay, 
                   XtasSaveGraphicPixmap, 
                   XtasGraphicPixmap,
                   XtasColor[BACKGROUND][SOLID], 
                   0, 
                   0, 
                   XtasGraphicZoneWidth, 
                   XtasGraphicZoneHeight, 
                   0, 
                   0);  
            
            if (event->x <= XtasTextZoneWidth) {
                xdest = XtasTextZoneWidth ;
                x2 = XTAS_UNSCALE(xdest - XtasTextZoneWidth, 
                                  XtasTmin, 
                                  XtasTmax, XtasGraphicZoneWidth) ;
            }
            if (event->x >= XtasTextZoneWidth + XtasGraphicZoneWidth) {
                xdest = XtasGraphicZoneWidth + XtasTextZoneWidth ;
                x2 = XTAS_UNSCALE(xdest - XtasTextZoneWidth, 
                                  XtasTmin, 
                                  XtasTmax, XtasGraphicZoneWidth) ;
            }
            if (event->x > XtasTextZoneWidth && event->x < XtasTextZoneWidth + XtasGraphicZoneWidth) {
                xdest = XtasGetValFromTab ((event->x - XtasTextZoneWidth) * 1000) / 1000 + XtasTextZoneWidth;
                xd = (float)XtasGetValFromTab ((event->x - XtasTextZoneWidth) * 1000) / 1000 + XtasTextZoneWidth;
                x2 = XTAS_UNSCALE(xd - XtasTextZoneWidth, 
                                  XtasTmin, 
                                  XtasTmax, XtasGraphicZoneWidth) ;
            }
            
            
//            sprintf (x1str, "t1 = %.1f ps", XtasRound(x1)/TTV_UNIT) ;
////            XDrawString(XtasGraphicDisplay, XtasSHPixmap, XtasColor[MSETUP][SOLID], 10, 200, x1str, strlen (x1str));
//            XtasSetLabelString (XtasDebugLabelT1, x1str);
            sprintf (x2str, "t2 = %.1f ps", XtasRound(x2)/TTV_UNIT) ;
            text = XmStringCreateSimple (x2str);
            XtVaSetValues (XtasDebugLabelT2, XmNlabelString, text,NULL);
            XmStringFree (text);
            XmUpdateDisplay (XtasDebugLabelT2);
//            XDrawString(XtasGraphicDisplay, XtasSHPixmap, XtasColor[MSETUP][SOLID], 10, 220, x2str, strlen (x2str));
            sprintf (delta, "dt = %.1f ps", XtasRound(fabs (x1 - x2))/TTV_UNIT) ;
            text = XmStringCreateSimple (delta);
            XtVaSetValues (XtasDebugLabelDelta, XmNlabelString, text,NULL);
            XmStringFree (text);
            XmUpdateDisplay (XtasDebugLabelDelta);
//            XDrawString(XtasGraphicDisplay, XtasSHPixmap, XtasColor[MSETUP][SOLID], 10, 240, delta, strlen (delta));
    
            XDrawLine (XtasGraphicDisplay, XtasGraphicPixmap, XtasColor[MSETUP][SOLID],
                   xorg - XtasTextZoneWidth, 0,
                   xorg - XtasTextZoneWidth, XtasGraphicZoneHeight) ;
            XFillRectangle (XtasGraphicDisplay, 
                   XtasGraphicPixmap, 
                   XtasColor[MSETUP][STIPPLE], 
                     MIN (xorg, xdest) - XtasTextZoneWidth, 
                     0, 
                   abs (xdest - xorg), 
                   XtasGraphicZoneHeight) ;
            XtasRefreshGraphicZone() ;
            XtasRefreshSHZone () ;
        } 
    }
}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasInitGraphics                                               */
/*                                                                           */
/* OBJECT   : Init graphics                                                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void XtasInitGraphics()
{
    XtActionsRec actions ;

    XtasTextZoneWidth     = 120 ;
    XtasCaptionZoneHeight = 20 ; 
    XtasTextZoneHeight    = XtasDrawingAreaHeight - 2 * XtasCaptionZoneHeight ;
    XtasCaptionZoneWidth  = XtasDrawingAreaWidth ;
    XtasGraphicZoneWidth  = XtasDrawingAreaWidth - /*2 **/ XtasTextZoneWidth - 25 ;
    XtasGraphicZoneHeight = XtasDrawingAreaHeight - 2 * XtasCaptionZoneHeight ;
    
    XtAddCallback(XtasGraphicWindow, XmNexposeCallback, XtasRefreshCallback, NULL) ; 

    XtasGraphicDisplay = XtDisplay(XtasGraphicWindow) ;
    
    XtasSaveGraphicPixmap = XCreatePixmap(XtasGraphicDisplay,
                                   RootWindowOfScreen(XtScreen(XtasGraphicWindow)),
                                   XtasGraphicZoneWidth, 
                                   XtasGraphicZoneHeight, 
                                   DefaultDepthOfScreen(XtScreen(XtasGraphicWindow)));  
    
    XtasGraphicPixmap = XCreatePixmap(XtasGraphicDisplay,
                                   RootWindowOfScreen(XtScreen(XtasGraphicWindow)),
                                   XtasGraphicZoneWidth, 
                                   XtasGraphicZoneHeight, 
                                   DefaultDepthOfScreen(XtScreen(XtasGraphicWindow)));  
    
    XtasSaveSHPixmap = XCreatePixmap(XtasGraphicDisplay,
                                   RootWindowOfScreen(XtScreen(XtasGraphicWindow)),
                                   XtasTextZoneWidth, 
                                   XtasTextZoneHeight, 
                                   DefaultDepthOfScreen(XtScreen(XtasGraphicWindow)));  
    
    XtasSHPixmap = XCreatePixmap(XtasGraphicDisplay,
                                   RootWindowOfScreen(XtScreen(XtasGraphicWindow)),
                                   XtasTextZoneWidth, 
                                   XtasTextZoneHeight, 
                                   DefaultDepthOfScreen(XtScreen(XtasGraphicWindow)));  
    
    XtasTextPixmap = XCreatePixmap(XtasGraphicDisplay,
                                   RootWindowOfScreen(XtScreen(XtasGraphicWindow)),
                                   XtasTextZoneWidth, 
                                   XtasTextZoneHeight, 
                                   DefaultDepthOfScreen(XtScreen(XtasGraphicWindow)));  
    
    XtasCaptionPixmap = XCreatePixmap(XtasGraphicDisplay,
                                   RootWindowOfScreen(XtScreen(XtasGraphicWindow)),
                                   XtasCaptionZoneWidth, 
                                   XtasCaptionZoneHeight, 
                                   DefaultDepthOfScreen(XtScreen(XtasGraphicWindow)));  
    
    XtasInitPatterns() ;
    XtasSetupGC() ;
    XtasClear = True ;
    XtasDraw = False ;

    actions.string = "draw_cbk";
    actions.proc = draw_cbk;
    XtAppAddActions(XtasApplicationContext, &actions, 1);
}

/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                            |
|                                                             |
| File    :                 Object.c                          |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                        Lester Anthony                       |
|                                                             |
\------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <signal.h>
#include <string.h>
#include <setjmp.h>

#include MUT_H
#include SLB_H
#include BEH_H
#include MLO_H
#include CGV_H
#include CNS_H
#include XSB_H
#include XYA_H

#include "XYA_place.h"
#include "XYA_object.h"
#include "XYA_cgvfig.h"
#include "XME_beh.h"
#include "XYA_error.h"

xyagfig_list   *XyagFigure = NULL;

/*------------------------------------------------------------\
|                         Alloc Functions                     |
\------------------------------------------------------------*/

xyagfig_list *
XyagAllocFigure()
{
    xyagfig_list  *figure;
    int            i;

    figure = (xyagfig_list *)mbkalloc(sizeof(xyagfig_list));
    figure->NEXT = NULL;
    figure->NAME = NULL;
    for (i=0; i<XYAG_MAX_LAYER; i++) {
        figure->OBJECT[i] = NULL;
    }
    return figure;
}

xyagobj_list *
XyagAllocObject()
{
    xyagobj_list  *object;

    object = (xyagobj_list *)mbkalloc(sizeof(xyagobj_list));
    object->NEXT = NULL;
    object->LINK = NULL;
    object->NAME = NULL;
    object->LAYER = 0;
    object->TYPE = 0;
    object->X = 0;
    object->Y = 0;
    object->DX = 0;
    object->DY = 0;
    object->USER = NULL;
    return object;
}

/*------------------------------------------------------------\
|                         Free Functions                      |
\------------------------------------------------------------*/

void 
XyagFreeFigure(Figure)
    xyagfig_list   *Figure;
{
    mbkfree(Figure);
}

void 
XyagFreeObject(Object)
    xyagobj_list   *Object;
{
    mbkfree(Object);
}

/*------------------------------------------------------------\
|                          XyagAddCircle                      |
\------------------------------------------------------------*/

xyagobj_list   *
XyagAddCircle(X, Y, R, Layer)
    long            X;
    long            Y;
    long            R;
    short           Layer;
{
    xyagobj_list   *Object;

    Object = XyagAllocObject();
    Object->TYPE = XYAG_OBJECT_CIRCLE;
    Object->LAYER = Layer;
    Object->NEXT = XyagFigure->OBJECT[Layer];
    Object->X = X - R;
    Object->Y = Y - R;
    Object->DX = (R << 1);
    Object->DY = (R << 1);

    XyagFigure->OBJECT[Layer] = Object;
    return (Object);
}

/*------------------------------------------------------------\
|                          XyagAddLine                        |
\------------------------------------------------------------*/

xyagobj_list   *
XyagAddLine(X1, Y1, X2, Y2, LayerFrom, LayerTo)
    long            X1;
    long            Y1;
    long            X2;
    long            Y2;
    short           LayerFrom;
    short           LayerTo;
{
    xyagobj_list   *Object;

    Object = XyagAllocObject();
    Object->LAYER = LayerFrom;
    Object->TYPE = XYAG_OBJECT_LINE;
    Object->NEXT = XyagFigure->OBJECT[LayerTo];

    if (X1 > X2) {
        SetXyagLineLeft(Object);

        Object->X = X2;
        Object->DX = X1 - X2;
    }
    else {
        Object->X = X1;
        Object->DX = X2 - X1;
    }

    if (Y1 > Y2) {
        SetXyagLineDown(Object);

        Object->Y = Y2;
        Object->DY = Y1 - Y2;
    }
    else {
        Object->Y = Y1;
        Object->DY = Y2 - Y1;
    }

    XyagFigure->OBJECT[LayerTo] = Object;

    return (Object);
}

/*------------------------------------------------------------\
|                          XyagAddArrow                       |
\------------------------------------------------------------*/

xyagobj_list   *
XyagAddArrow(X1, Y1, X2, Y2, LayerFrom, LayerTo)
    long            X1;
    long            Y1;
    long            X2;
    long            Y2;
    short           LayerFrom;
    short           LayerTo;
{
    xyagobj_list   *Object;

    Object = XyagAllocObject();
    Object->LAYER = LayerFrom;
    Object->TYPE = XYAG_OBJECT_ARROW;
    Object->NEXT = XyagFigure->OBJECT[LayerTo];

    if (X1 > X2) {
        SetXyagLineLeft(Object);

        Object->X = X2;
        Object->DX = X1 - X2;
    }
    else {
        Object->X = X1;
        Object->DX = X2 - X1;
    }

    if (Y1 > Y2) {
        SetXyagLineDown(Object);

        Object->Y = Y2;
        Object->DY = Y1 - Y2;
    }
    else {
        Object->Y = Y1;
        Object->DY = Y2 - Y1;
    }

    XyagFigure->OBJECT[LayerTo] = Object;

    return (Object);
}

/*------------------------------------------------------------\
|                          XyagAddUserDefined                 |
\------------------------------------------------------------*/

xyagobj_list   *
XyagAddUserDefined(ptmotif,X, Y, DX, DY, Layer)
    symbol_list    *ptmotif;
    long            X;
    long            Y;
    long            DX;
    long            DY;
    short           Layer;
{
    xyagobj_list   *Object;

    Object = XyagAllocObject();
    Object->TYPE = XYAG_OBJECT_SLIB;
    Object->LAYER = Layer;
    Object->NEXT = XyagFigure->OBJECT[Layer];
    Object->X = X;
    Object->Y = Y;
    Object->DX = DX;
    Object->DY = DY;
    Object->SYMBOL = ptmotif;

    XyagFigure->OBJECT[Layer] = Object;

    return (Object);
}

/*------------------------------------------------------------\
|                          XyagAddTriangle                    |
\------------------------------------------------------------*/

xyagobj_list   *
XyagAddTriangle(X, Y, DX, DY, Layer)
    long            X;
    long            Y;
    long            DX;
    long            DY;
    short           Layer;
{
    xyagobj_list   *Object;

    Object = XyagAllocObject();
    Object->TYPE = XYAG_OBJECT_TRIANGLE;
    Object->LAYER = Layer;
    Object->NEXT = XyagFigure->OBJECT[Layer];
    Object->X = X;
    Object->Y = Y;
    Object->DX = DX;
    Object->DY = DY;

    XyagFigure->OBJECT[Layer] = Object;
    return (Object);
}

/*------------------------------------------------------------\
|                            XyagAddCell                      |
\------------------------------------------------------------*/

xyagobj_list   *
XyagAddCell(X, Y, DX, DY, Layer)
    long            X;
    long            Y;
    long            DX;
    long            DY;
    short           Layer;
{
    xyagobj_list   *Object;

    Object = XyagAllocObject();
    Object->TYPE = XYAG_OBJECT_CELL;
    Object->LAYER = Layer;
    Object->NEXT = XyagFigure->OBJECT[Layer];
    Object->X = X;
    Object->Y = Y;
    Object->DX = DX;
    Object->DY = DY;

    XyagFigure->OBJECT[Layer] = Object;

    return (Object);
}


/*------------------------------------------------------------\
|                          XyagAddRectangle                   |
\------------------------------------------------------------*/

xyagobj_list   *
XyagAddRectangle(X, Y, DX, DY, Layer)
    long            X;
    long            Y;
    long            DX;
    long            DY;
    short           Layer;
{
    xyagobj_list   *Object;

    Object = XyagAllocObject();
    Object->TYPE = XYAG_OBJECT_RECTANGLE;
    Object->LAYER = Layer;
    Object->NEXT = XyagFigure->OBJECT[Layer];
    Object->X = X;
    Object->Y = Y;
    Object->DX = DX;
    Object->DY = DY;

    XyagFigure->OBJECT[Layer] = Object;

    return (Object);
}

/*------------------------------------------------------------\
|                          XyagAddText                        |
\------------------------------------------------------------*/

xyagobj_list   *
XyagAddText(X, Y, Type, Name, Layer, scale)
    long            X;
    long            Y;
    long            Type;
    char           *Name;
    short           Layer;
    long            scale;
{
    xyagobj_list   *Object;
    long            Width;

    Width = strlen(Name) * scale * XYAG_UNIT * 2 / 3;

    if (Type == XYAG_OBJECT_TEXT_CENTER) {
        X = X - (Width >> 1);
    }

    if (Type == XYAG_OBJECT_TEXT_LEFT) {
        X = X - Width;
    }

    Object = XyagAllocObject();
    Object->LAYER = Layer;
    Object->TYPE = Type;
    Object->NEXT = XyagFigure->OBJECT[Layer];
    Object->NAME = namealloc(Name);
    Object->X = X;
    Object->Y = Y;
    Object->DX = Width;


    XyagFigure->OBJECT[Layer] = Object;

    return (Object);
}

/*------------------------------------------------------------\
|                          XyagAddHexagon                     |
\------------------------------------------------------------*/

xyagobj_list   *
XyagAddHexagon(X, Y, DX, DY, Layer)
    long            X;
    long            Y;
    long            DX;
    long            DY;
    short           Layer;
{
    xyagobj_list   *Object;

    Object = XyagAllocObject();
    Object->TYPE = XYAG_OBJECT_HEXAGON;
    Object->LAYER = Layer;
    Object->NEXT = XyagFigure->OBJECT[Layer];
    Object->X = X;
    Object->Y = Y;
    Object->DX = DX;
    Object->DY = DY;

    XyagFigure->OBJECT[Layer] = Object;

    return (Object);
}

/*------------------------------------------------------------\
|                          XyagAddFigure                      |
\------------------------------------------------------------*/

xyagfig_list   *
XyagAddFigure()
{
    XyagFigure = XyagAllocFigure();
    XyagFigure->NAME = XyagFigureCgv->NAME;

    XyagFigureCgv->data2=XyagFigure;

    XyagTraceFigure(XyagFigureCgv);

    return (XyagFigure);
}

/*------------------------------------------------------------\
|                          XyagLoadFigure                     |
\------------------------------------------------------------*/

chain_list *OPEN_STACK=NULL;

void 
XyagLoadFigure(char *FileName)
{
  char *c;
  char temp[200];
  cgvfig_list *old=XyagFigureCgv;

  strcpy(temp, FileName);
  if ((c=mbkIsLofigExt(FileName,"cns"))!=NULL)    
    {
      *c='\0';
      XyagFigureCgv = Xyaggetcgvfig(FileName, CGV_FROM_CNS, temp);
      
      if (XyagFigureCgv != NULL)
	{
	  OPEN_STACK=addchain(OPEN_STACK, XyagFigureCgv);
	  if (CGV_WAS_PRESENT==0)
	    {
	      XyagAddFigure();
            }
          
          XyagEditBehBefig = Xyaggetbefig(FileName);
          XyagCleanErrorMessage();
	}
      else
	{
	  XyagFigureCgv=old;
	  DisplayMessage("Failed to open file !");
	}
	
    }
  else
     if ((c=mbkFileIsLofig(FileName))!=NULL)
       {
	 char ext[50], savelo[10];
	 *c='\0';
	 strcpy(savelo, IN_LO);
	 strcpy(ext,c+1);
	 if ((c=strchr(ext,'.'))!=NULL) *c='\0';
	 strcpy(IN_LO, ext);
	 XyagFigureCgv = Xyaggetcgvfig(FileName, CGV_FROM_LOFIG, temp);
	 strcpy(IN_LO, savelo);
	 if (XyagFigureCgv != NULL)
	   {
	     OPEN_STACK=addchain(OPEN_STACK, XyagFigureCgv);
	     if (CGV_WAS_PRESENT==0) 
	       {
		 XyagAddFigure();
	       }
	   }
	 else
	   {
	     XyagFigureCgv=old;
	     DisplayMessage("Failed to open file !");
	   }
       }
     else
       DisplayMessage("Can not handle this file !");
}

int XyagLoadFigureByType(char *Name, int type, int silent)
{
  cgvfig_list *old=XyagFigureCgv;
  int bad=0;
  switch(type)
    {
    case CGV_FROM_CNS:
      XyagFigureCgv = Xyaggetcgvfig(Name, CGV_FROM_CNS, NULL);
      
      if (XyagFigureCgv != NULL && CGV_WAS_PRESENT==0) 
	{
	  XyagAddFigure();
	}
      if (XyagFigureCgv != NULL)
        {
          XyagEditBehBefig = Xyaggetbefig(Name);
          XyagCleanErrorMessage();
        }

      if (XyagFigureCgv==NULL)
	{
	  XyagFigureCgv=old;
	  if (!silent) DisplayMessage("Failed to open file !");
          bad=1;
	}
      else
	OPEN_STACK=addchain(OPEN_STACK, XyagFigureCgv);
      break;
    case CGV_FROM_LOFIG:
      {
	XyagFigureCgv = Xyaggetcgvfig(Name, CGV_FROM_LOFIG, NULL);
	if (XyagFigureCgv != NULL && CGV_WAS_PRESENT==0) 
	  {
	    XyagAddFigure();
	  }

	if (XyagFigureCgv==NULL)
	  {
	    XyagFigureCgv=old;
	    if (!silent) DisplayMessage("Failed to open file !");
            bad=1;
	  }
	else
	  OPEN_STACK=addchain(OPEN_STACK, XyagFigureCgv);
	break;
      }
    default:
      DisplayMessage("This file type is not handled !");
      bad=1;
    }
  return bad;
}

void XyagGetCone(cone_list *cn)
{
  XyagFigureCgv=getcgvfig_from_cone(cn);
  if (CGV_WAS_PRESENT==0) 
    {
      XyagAddFigure();
    }
  OPEN_STACK=addchain(OPEN_STACK, XyagFigureCgv);
}
/*------------------------------------------------------------\
|                          XyagDelFigure                      |
\------------------------------------------------------------*/

void 
XyagDelFigure()
{
    xyagobj_list   *ScanObject;
    xyagobj_list   *DelObject;
    short           Layer;

    if (XyagFigure == (xyagfig_list *) NULL) return;

    for (Layer = 0; Layer < XYAG_MAX_LAYER; Layer++) {
        ScanObject = XyagFigure->OBJECT[Layer];

        while (ScanObject != (xyagobj_list *) NULL) {
            DelObject = ScanObject;
            ScanObject = ScanObject->NEXT;
            XyagFreeObject(DelObject);
        }
    }

    XyagFreeFigure(XyagFigure);
    rmvcgvfig(XyagFigureCgv);
    XyagFigure = NULL;
    XyagFigureCgv = NULL;
}

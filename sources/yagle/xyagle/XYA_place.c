/*------------------------------------------------------------\
|                                                             |
| Tool    :                  XYAG CGV                         |
|                                                             |
| File    :                  Trace.c                          |
|                                                             |
| Authors :              Miramond Benoit                      |
|                        Picault Stephane                     |
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
#include MLO_H
#include CNS_H
#include XSB_H
#include CGV_H
#include XYA_H

#include "XYA_place.h"
#include "XYA_cgvfig.h"

#define IsCgvWireHorizontal(Z)    ((Z)->DY == (Z)->Y)
#define IsCgvWireVertical(Z)      ((Z)->DX == (Z)->X)

/*------------------------------------------------------------\
|                           Trace Con                         |
\------------------------------------------------------------*/

void 
trace_con(con_list, X, Y, LayerCon, scale)
    cgvcon_list    *con_list;
    long            X;
    long            Y;
    long            scale;
    unsigned char   LayerCon;
{
    xyagobj_list   *Object1;
    xyagobj_list   *Object2;
    cgvcon_list    *CgvCon;
    locon_list     *LoCon;
    cgvbox_list    *Box;
//    cone_list      *CnsCone;
    int             ObjectType;
    long            X_trace;
    long            Y_trace;
    long            Y_con;
    long            X_con;
    long            DeltaX;
    int     ray;

    ray=scale * XYAG_UNIT / 2;
    if (ray==0) ray=1;
    for (CgvCon = con_list; CgvCon; CgvCon = CgvCon->NEXT) {
        X_con = (CgvCon->X_REL * XYAG_UNIT);
        Y_con = (CgvCon->Y_REL * XYAG_UNIT) - scale * XYAG_UNIT;

        if (IsCgvConOut(CgvCon)) {
            X_con -= scale * (XYAG_UNIT * 2);
        }

        if (IsCgvConExternal(CgvCon)) {
            if (IsCgvConIn(CgvCon)) {
                ObjectType = XYAG_OBJECT_TEXT_LEFT;
            }
            else {
                ObjectType = XYAG_OBJECT_TEXT_RIGHT;
            }
        }
        else {
            if (IsCgvConIn(CgvCon)) {
                ObjectType = XYAG_OBJECT_TEXT_RIGHT;
            }
            else {
                ObjectType = XYAG_OBJECT_TEXT_LEFT;
            }
        }

        if (ObjectType == XYAG_OBJECT_TEXT_LEFT) {
            DeltaX = -XYAG_UNIT;
        }
        else {
            DeltaX = 3 * XYAG_UNIT;
        }

        X_trace = X_con + X;
        Y_trace = Y_con + Y;

        if (!IsCgvConFake(CgvCon)) {
	  if (IsCgvConExternal(CgvCon))
            Object1 = XyagAddHexagon(X_trace, Y_trace, scale * XYAG_UNIT * 2, scale * XYAG_UNIT * 2, LayerCon);
	  else
	    {
	      if (!IsCgvConIn(CgvCon)) {
                Object1 = XyagAddRectangle(X_trace+scale*3*XYAG_UNIT/2, Y_trace+scale*XYAG_UNIT/2, scale*XYAG_UNIT , scale*XYAG_UNIT , LayerCon);
		DeltaX+=scale*3*XYAG_UNIT/2;
	      }
	      else {
                Object1 = XyagAddRectangle(X_trace-scale*XYAG_UNIT/2, Y_trace+scale*XYAG_UNIT/2, scale*XYAG_UNIT , scale*XYAG_UNIT , LayerCon);
	      }
	    }
	    
        }
        else {
	  if (!IsCgvConIn(CgvCon)) {
	    Object1 = XyagAddCircle(X_trace + scale*XYAG_UNIT * 1 / 2, Y_trace + scale*XYAG_UNIT, ray, LayerCon);
	    
	  }
	  else {
	    Object1 = XyagAddCircle(X_trace + scale*XYAG_UNIT * 3 / 2, Y_trace + scale*XYAG_UNIT, ray, LayerCon);
	  }
        }

        Object2 = XyagAddText(X_trace + DeltaX, Y_trace + XYAG_UNIT, ObjectType, CgvCon->NAME, LayerCon,scale);
        Object1->LINK = Object2;
        Object2->LINK = Object1;

        Object1->USER = (void *) CgvCon;
        Object2->USER = (void *) CgvCon;

        SetXyagCgvCon(Object1);
        SetXyagCgvCon(Object2);

	Box=(cgvbox_list *)CgvCon->ROOT;
//	printf("%s\n",Box->NAME);
/*	if (!IsCgvConExternal(CgvCon))
	  Box->USER=Object1;*/
	CgvCon->USER=Object1;
/*	((cgvbox_list *)CgvCon->ROOT)->USER=Object1;
        if (CgvCon->SOURCE_TYPE == CGV_SOURCE_CNSCONE) {
            CnsCone = (cone_list *) CgvCon->SOURCE;
            CnsCone->USER = addptype(CnsCone->USER, XYAG_USER_OBJECT_CGV, (void *) Object1);
        }
*/
        if (CgvCon->SOURCE_TYPE == CGV_SOURCE_LOCON) {
            LoCon = (locon_list *) CgvCon->SOURCE;
            LoCon->USER = addptype(LoCon->USER, XYAG_USER_OBJECT_CGV, (void *) Object1);
        }
    }
}

/*------------------------------------------------------------\
|                          XyagTrace                          |
\------------------------------------------------------------*/

void 
XyagTraceFigure(CgvFig)
    cgvfig_list    *CgvFig;
{
    cgvbox_list    *CgvBox;
    cgvnet_list    *CgvNet;
    cgvwir_list    *CgvWir;
    cgvwir_list    *ScanWir;
    xyagobj_list   *Object;
    xyagobj_list   *FirstObj;
    xyagobj_list   *PrevObj;
    long            X_trace;
    long            X_fig;
    long            X_box;
    long            X_wir;
    long            Dx;
    long            DX_wir;
    long            MaxY_wire;
    long            MinY_wire;
    long            Y_trace;
    long            Y_fig;
    long            Y_box;
    long            Y_wir;
    long            Dy;
    long            DY_wir;
    int             cptmax_wire = 0;
    int             cptmin_wire = 0;
    unsigned char   Layer;
    long            scale;

    /* Fig */

    X_fig = 0;
    Y_fig = 0;

    /* box   */
    if (CgvFig->SCALE == 0)
             scale = 1;
    else
             scale = CgvFig->SCALE;


    for (CgvBox = CgvFig->BOX; CgvBox; CgvBox = CgvBox->NEXT) {
        if (IsCgvBoxTransparence(CgvBox) || IsCgvBoxCluster(CgvBox)) {
            Layer = XYAG_CONSTRUCTION_LAYER;
        }
        else {
            Layer = XYAG_CGVBOX_LAYER;
        }

        X_box = CgvBox->X * XYAG_UNIT;
        Y_box = CgvBox->Y * XYAG_UNIT;
        Dx = CgvBox->DX * XYAG_UNIT;
        Dy = CgvBox->DY * XYAG_UNIT;

        X_trace = X_box + X_fig;
        Y_trace = Y_box + Y_fig;

        if( CgvBox->SYMBOL == NULL) {
          if (CgvBox->SOURCE_TYPE == CGV_SOURCE_CNSCONE) {
              Object = XyagAddTriangle(X_trace, Y_trace, Dx, Dy, Layer);
          }
          else if (CgvBox->SOURCE_TYPE == CGV_SOURCE_CNSCELL) {
              Object = XyagAddCell(X_trace, Y_trace, Dx, Dy, Layer);
          }
          else {
              Object = XyagAddRectangle(X_trace, Y_trace, Dx, Dy, Layer);
          }
        }
        else {
           Object = XyagAddUserDefined(CgvBox->SYMBOL,X_trace, Y_trace, Dx, Dy, Layer);
        }
        Object->LINK = XyagAddText(X_trace + (Dx >> 1), Y_trace + (Dy >> 1), XYAG_OBJECT_TEXT_CENTER, CgvBox->NAME, Layer,scale);
        Object->LINK->LINK = Object;
        Object->USER = (void *) CgvBox;
        Object->LINK->USER = (void *) CgvBox;
        
        SetXyagCgvBox(Object);
        SetXyagCgvBox(Object->LINK);
	CgvBox->USER=Object;

        if (CgvBox->SOURCE_TYPE == CGV_SOURCE_CNSCONE) {
//	  CgvBox->USER=Object;
/*
            CnsCone = (cone_list *) CgvBox->SOURCE;
            CnsCone->USER = addptype(CnsCone->USER, XYAG_USER_OBJECT_CGV, (void *) Object);
*/
        }
        else if (CgvBox->SOURCE_TYPE == CGV_SOURCE_CNSCELL) {
//	   CgvBox->USER=Object;
/*            CnsCell = (cell_list *) CgvBox->SOURCE;
            CnsCell->USER = addptype(CnsCell->USER, XYAG_USER_OBJECT_CGV, (void *) Object);*/
        }
        else 
	  {
            /* Cons des box */
	    if (CgvBox->SYMBOL==NULL)
	      {
		cgvcon_list    *CgvCon;
		xyagobj_list  *tmp, *tmp0;
		trace_con(CgvBox->CON_IN, X_trace, Y_trace, Layer, CgvFig->SCALE);
		trace_con(CgvBox->CON_OUT, X_trace, Y_trace, Layer, CgvFig->SCALE);
		
		for (CgvCon=CgvBox->CON_IN; CgvCon!=NULL; CgvCon=CgvCon->NEXT)
		  {
		    tmp=(xyagobj_list *)CgvCon->USER;
		    tmp0=Object->LINK;
		    Object->LINK=tmp->LINK;
		    tmp->LINK=tmp0;
		  }
		for (CgvCon=CgvBox->CON_OUT; CgvCon!=NULL; CgvCon=CgvCon->NEXT)
		  {
		    tmp=(xyagobj_list *)CgvCon->USER;
		    tmp0=Object->LINK;
		    Object->LINK=tmp->LINK;
		    tmp->LINK=tmp0;
		  }

	      }
        }
    }

    /*  Con  externes  */

    trace_con(CgvFig->CON_IN, X_fig, Y_fig, XYAG_CGVCONIN_LAYER, CgvFig->SCALE);
    trace_con(CgvFig->CON_OUT, X_fig, Y_fig, XYAG_CGVCONOUT_LAYER, CgvFig->SCALE);

    /* Nets */

    for (CgvNet = CgvFig->NET; CgvNet; CgvNet = CgvNet->NEXT) {
        FirstObj = NULL;
        PrevObj = NULL;

        for (CgvWir = CgvNet->WIRE; CgvWir; CgvWir = CgvWir->NEXT) {
            X_wir = CgvWir->X * XYAG_UNIT;
            Y_wir = CgvWir->Y * XYAG_UNIT;
            DX_wir = CgvWir->DX * XYAG_UNIT;
            DY_wir = CgvWir->DY * XYAG_UNIT;

            X_trace = X_fig + X_wir;
            Y_trace = Y_fig + Y_wir;

            Object = XyagAddLine(X_trace, Y_trace, DX_wir + X_fig, DY_wir + Y_fig, XYAG_CGVNET_LAYER, XYAG_CGVNET_LAYER);

            if (FirstObj == NULL) {
                FirstObj = Object;
		CgvNet->USER=Object;
            }

            Object->USER = (void *) CgvNet;
            SetXyagCgvNet(Object);
            Object->LINK = Object;

            if (PrevObj != NULL) {
                PrevObj->LINK = Object;
            }

            PrevObj = Object;

            if (IsCgvWireVertical(CgvWir)) {
                cptmax_wire = 0;
                cptmin_wire = 0;

                if (CgvWir->Y < CgvWir->DY) {
                    MaxY_wire = CgvWir->DY;
                    MinY_wire = CgvWir->Y;
                }
                else {
                    MaxY_wire = CgvWir->Y;
                    MinY_wire = CgvWir->DY;
                }

                for (ScanWir = CgvNet->WIRE; ScanWir; ScanWir = ScanWir->NEXT) {
                    if (IsCgvWireHorizontal(ScanWir)) {
                        if ((ScanWir->DX == CgvWir->X) || (ScanWir->X == CgvWir->X)) {
                            if ((MinY_wire < ScanWir->Y) && (ScanWir->Y < MaxY_wire)) {
                                Object = XyagAddCircle(X_trace, Y_fig + (ScanWir->DY * XYAG_UNIT), CgvFig->SCALE * XYAG_UNIT / 4, XYAG_CGVNET_LAYER);
                                Object->USER = (void *) CgvNet;
                                SetXyagCgvNet(Object);
                                PrevObj->LINK = Object;
                                PrevObj = Object;
                            }

                            if (MaxY_wire != MinY_wire) {
                                if (MaxY_wire == ScanWir->Y) {
                                    cptmax_wire++;
                                }
                                if (MinY_wire == ScanWir->Y) {
                                    cptmin_wire++;
                                }
                            }
                        }
                    }
                }
                if (cptmax_wire >= 2) {
                    Object = XyagAddCircle(X_trace, Y_fig + MaxY_wire * XYAG_UNIT, CgvFig->SCALE * XYAG_UNIT / 4, XYAG_CGVNET_LAYER);
                    Object->USER = (void *) CgvNet;
                    SetXyagCgvNet(Object);
                    PrevObj->LINK = Object;
                    PrevObj = Object;
                }

                if (cptmin_wire >= 2) {
                    Object = XyagAddCircle(X_trace, Y_fig + MinY_wire * XYAG_UNIT, CgvFig->SCALE * XYAG_UNIT / 4, XYAG_CGVNET_LAYER);
                    Object->USER = (void *) CgvNet;
                    SetXyagCgvNet(Object);
                    PrevObj->LINK = Object;
                    PrevObj = Object;
                }
            }
        }
	if (PrevObj != NULL) 
	  {
	    PrevObj->LINK = FirstObj;
	  }
#if 0
        if (PrevObj != NULL) {
            PrevObj->LINK = FirstObj;
            Object = NULL;
            if (CgvNet->SOURCE_TYPE == CGV_SOURCE_CNSCONE) {
                CnsCone = (cone_list *) CgvNet->SOURCE;
                if (CnsCone != NULL) {
                    Object = (xyagobj_list *) getptype(CnsCone->USER, XYAG_USER_OBJECT_CGV)->DATA;
                }
            }

            if (CgvNet->SOURCE_TYPE == CGV_SOURCE_CNSCELL) {
                CnsCell = (cell_list *) CgvNet->SOURCE;
                if (CnsCell != NULL) {
                    Object = (xyagobj_list *) getptype(CnsCell->USER, XYAG_USER_OBJECT_CGV)->DATA;
                }
            }

            if (CgvNet->SOURCE_TYPE == CGV_SOURCE_LOCON) {
                LoCon = (locon_list *) CgvNet->SOURCE;
                if (LoCon != NULL) {
                    Object = (xyagobj_list *) getptype(LoCon->USER, XYAG_USER_OBJECT_CGV)->DATA;
                }
            }

            if (Object != NULL) {
                ScanObject = Object;
                while (ScanObject->LINK != Object) {
                    ScanObject = ScanObject->LINK;
                }
                PrevObj->LINK = Object;
                ScanObject->LINK = FirstObj;
            }
        }
#endif
    }
}

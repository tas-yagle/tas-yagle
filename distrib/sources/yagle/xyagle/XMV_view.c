/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                            |
|                                                             |
| File    :                   View.c                          |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                        Lester Anthony                       |
|                                                             |
\------------------------------------------------------------*/

#include <stdio.h>
#include <Xm/Xm.h>

#include MUT_H
#include MLO_H
#include CNS_H
#include XSB_H
#include CGV_H
#include XYA_H
#include XTB_H
#include XMX_H
#include XMV_H
#include XMF_H
#include CGV_H

#include "XMV_view.h"
#include "XMV_panel.h"
#include "XME_select.h"
#include "XMX_scroll.h"

xyagzoom       *XyagHeadZoom = NULL;

int             XyagPercentZoom = XYAG_PERCENT_ZOOM;
int             XyagPercentMoveX = XYAG_PERCENT_MOVE;
int             XyagPercentMoveY = XYAG_PERCENT_MOVE;

int             XyagDepthOfDep = 1;

void XyagGetCone(cone_list *cn);
/*------------------------------------------------------------\
|                        XyagAllocZoom                        |
\------------------------------------------------------------*/

xyagzoom *
XyagAllocZoom()
{
    xyagzoom       *zoom;

    zoom = (xyagzoom *)mbkalloc(sizeof(xyagzoom));
    zoom->NEXT = NULL;
    zoom->X = 0;
    zoom->Y = 0;
    zoom->DX = 0;
    zoom->DY = 0;

    return zoom;
}

/*------------------------------------------------------------\
|                        XyagFreeZoom                         |
\------------------------------------------------------------*/

void 
XyagFreeZoom(FreeZoom)
    xyagzoom       *FreeZoom;
{
    mbkfree(FreeZoom);
}

/*------------------------------------------------------------\
|                        XyagAddZoom                          |
\------------------------------------------------------------*/

void 
XyagAddZoom()
{
    xyagzoom       *XyagZoom;

    XyagZoom = XyagAllocZoom();
    XyagZoom->X = XyagUnitGridX;
    XyagZoom->Y = XyagUnitGridY;
    XyagZoom->DX = XyagUnitGridDx;
    XyagZoom->DY = XyagUnitGridDy;
    XyagZoom->NEXT = XyagHeadZoom;
    XyagHeadZoom = XyagZoom;
}

/*------------------------------------------------------------\
|                          XyagDelZoom                        |
\------------------------------------------------------------*/

char 
XyagDelZoom()
{
    xyagzoom       *XyagZoom;

    if (XyagHeadZoom != (xyagzoom *) NULL) {
        XyagZoom = XyagHeadZoom;
        XyagHeadZoom = XyagHeadZoom->NEXT;
        XyagFreeZoom(XyagZoom);
        return (XYAG_TRUE);
    }

    return (XYAG_FALSE);
}

/*------------------------------------------------------------\
|                      XyagInitializeZoom                     |
\------------------------------------------------------------*/

void 
XyagInitializeZoom()
{
    while (XyagDelZoom() != XYAG_FALSE);

    if (XyagComputeBound() == XYAG_FALSE) {
        XyagInitializeUnitGrid();
    }
    else {
        XyagUnitGridX = (XyagBoundXmin / XYAG_UNIT) - 1;
        XyagUnitGridY = (XyagBoundYmin / XYAG_UNIT) - 1;
        XyagUnitGridDx = (XyagBoundXmax / XYAG_UNIT);
        XyagUnitGridDy = (XyagBoundYmax / XYAG_UNIT);
        XyagUnitGridDx = XyagUnitGridDx - XyagUnitGridX + 1;
        XyagUnitGridDy = XyagUnitGridDy - XyagUnitGridY + 1;

        XyagComputeUnitGrid();

        if (XyagUnitGridDx > 1.1 * (XyagBoundXmax - XyagBoundXmin) / XYAG_UNIT) {
            XyagUnitGridX -= (XyagUnitGridDx - (XyagBoundXmax - XyagBoundXmin) / XYAG_UNIT) / 2;
        }
        XyagPixelGridX = (float) (XyagUnitGridX) * XyagUnitGridStep;
        if (XyagUnitGridDy > 1.1 * (XyagBoundYmax - XyagBoundYmin) / XYAG_UNIT) {
            XyagUnitGridY -= (XyagUnitGridDy - (XyagBoundYmax - XyagBoundYmin) / XYAG_UNIT) / 2;
        }
        XyagPixelGridY = (float) (XyagUnitGridY) * XyagUnitGridStep;

    }

    XyagComputeScrollBar();
}

/*------------------------------------------------------------\
|                         XyagZoomUndo                        |
\------------------------------------------------------------*/

void 
XyagZoomUndo()
{
    if (XyagHeadZoom == (xyagzoom *) NULL) {
        XyagErrorMessage(XyagMainWindow, "No previous zoom !");
    }
    else {
        XyagUnitGridX = XyagHeadZoom->X;
        XyagUnitGridY = XyagHeadZoom->Y;
        XyagUnitGridDx = XyagHeadZoom->DX;
        XyagUnitGridDy = XyagHeadZoom->DY;
        XyagComputeUnitGrid();
        XyagComputeScrollBar();
        XyagDisplayFigure(0, 0, XyagGraphicDx, XyagGraphicDy);
        XyagRefreshGraphicWindow(0, 0, XyagGraphicDx, XyagGraphicDy);
        XyagDelZoom();
    }
}


xyagzoom *XyagZoomSave()
{
  xyagzoom *temp;
  XyagAddZoom();
  temp=XyagHeadZoom;
  XyagHeadZoom=NULL;
  return temp;
}

void XyagZoomRestore(xyagzoom *zoom)
{
  if (zoom==NULL) { XyagHeadZoom=NULL; XyagInitializeZoom(); return; }
  XyagHeadZoom=zoom;
  XyagZoomUndo();
}

/*------------------------------------------------------------\
|                         XyagZoomRight                       |
\------------------------------------------------------------*/

void 
XyagZoomRight()
{
    long            Delta;
    long            Offset;
    long            XyagOldPixelGridX;

    Delta = (XyagPercentMoveX * XyagUnitGridDx) / 100;

    if (Delta == 0)
        Delta = 1;

    XyagUnitGridX = XyagUnitGridX + Delta;
    XyagOldPixelGridX = XyagPixelGridX;
    XyagPixelGridX = (float) (XyagUnitGridX) * XyagUnitGridStep;
    Offset = XyagPixelGridX - XyagOldPixelGridX;

    XCopyArea(XyagGraphicDisplay,
              XyagGraphicPixmap,
              XyagGraphicPixmap,
              XyagBackgroundGC,
              Offset, 0,
              XyagGraphicDx - Offset,
              XyagGraphicDy,
              0, 0
        );

    XyagComputeScrollBar();
    XyagDisplayFigure(XyagGraphicDx - Offset, 0,
                      XyagGraphicDx, XyagGraphicDy);
    XyagRefreshGraphicWindow(0, 0,
                             XyagGraphicDx, XyagGraphicDy);
}

/*------------------------------------------------------------\
|                         XyagZoomLeft                        |
\------------------------------------------------------------*/

void 
XyagZoomLeft()
{
    long            Delta;
    long            Offset;
    long            XyagOldPixelGridX;

    Delta = (XyagPercentMoveX * XyagUnitGridDx) / 100;

    if (Delta == 0)
        Delta = 1;

    XyagUnitGridX = XyagUnitGridX - Delta;
    XyagOldPixelGridX = XyagPixelGridX;
    XyagPixelGridX = (float) (XyagUnitGridX) * XyagUnitGridStep;
    Offset = XyagOldPixelGridX - XyagPixelGridX;

    XCopyArea(XyagGraphicDisplay,
              XyagGraphicPixmap,
              XyagGraphicPixmap,
              XyagBackgroundGC,
              0, 0,
              XyagGraphicDx - Offset,
              XyagGraphicDy,
              Offset, 0
        );

    XyagComputeScrollBar();
    XyagDisplayFigure(0, 0, Offset, XyagGraphicDy);
    XyagRefreshGraphicWindow(0, 0, XyagGraphicDx, XyagGraphicDy);
}

/*------------------------------------------------------------\
|                          XyagZoomDown                       |
\------------------------------------------------------------*/

void 
XyagZoomDown()
{
    long            Delta;
    long            Offset;
    long            XyagOldPixelGridY;

    Delta = (XyagPercentMoveY * XyagUnitGridDy) / 100;

    if (Delta == 0)
        Delta = 1;

    XyagUnitGridY = XyagUnitGridY - Delta;
    XyagOldPixelGridY = XyagPixelGridY;
    XyagPixelGridY = (float) (XyagUnitGridY) * XyagUnitGridStep;
    Offset = XyagOldPixelGridY - XyagPixelGridY;

    XCopyArea(XyagGraphicDisplay,
              XyagGraphicPixmap,
              XyagGraphicPixmap,
              XyagBackgroundGC,
              0, Offset,
              XyagGraphicDx,
              XyagGraphicDy - Offset,
              0, 0
        );

    XyagComputeScrollBar();
    XyagDisplayFigure(0, 0, XyagGraphicDx, Offset);
    XyagRefreshGraphicWindow(0, 0, XyagGraphicDx, XyagGraphicDy);
}

/*------------------------------------------------------------\
|                          XyagZoomUp                         |
\------------------------------------------------------------*/

void 
XyagZoomUp()
{
    long            Delta;
    long            Offset;
    long            XyagOldPixelGridY;

    Delta = (XyagPercentMoveY * XyagUnitGridDy) / 100;

    if (Delta == 0)
        Delta = 1;

    XyagUnitGridY = XyagUnitGridY + Delta;
    XyagOldPixelGridY = XyagPixelGridY;
    XyagPixelGridY = (float) (XyagUnitGridY) * XyagUnitGridStep;
    Offset = XyagPixelGridY - XyagOldPixelGridY;

    XCopyArea(XyagGraphicDisplay,
              XyagGraphicPixmap,
              XyagGraphicPixmap,
              XyagBackgroundGC,
              0, 0,
              XyagGraphicDx,
              XyagGraphicDy - Offset,
              0, Offset
        );

    XyagComputeScrollBar();
    XyagDisplayFigure(0, XyagGraphicDy - Offset,
                      XyagGraphicDx, XyagGraphicDy);
    XyagRefreshGraphicWindow(0, 0, XyagGraphicDx, XyagGraphicDy);
}

/*------------------------------------------------------------\
|                         XyagZoomMore                        |
\------------------------------------------------------------*/

void 
XyagZoomMore()
{
    long            DeltaX;
    long            DeltaY;

    DeltaX = (XyagPercentZoom * XyagUnitGridDx) / 100;
    DeltaY = (XyagPercentZoom * XyagUnitGridDy) / 100;

    if ((DeltaX >= 2) &&
        (DeltaY >= 2)) {
        XyagAddZoom();

        XyagUnitGridX = XyagUnitGridX + (DeltaX >> 1);
        XyagUnitGridY = XyagUnitGridY + (DeltaY >> 1);
        XyagUnitGridDx = XyagUnitGridDx - DeltaX;
        XyagUnitGridDy = XyagUnitGridDy - DeltaY;

        XyagComputeUnitGrid();
        XyagComputeScrollBar();
        XyagDisplayFigure(0, 0, XyagGraphicDx, XyagGraphicDy);
        XyagRefreshGraphicWindow(0, 0, XyagGraphicDx, XyagGraphicDy);
    }
}

/*------------------------------------------------------------\
|                         XyagZoomLess                        |
\------------------------------------------------------------*/

void 
XyagZoomLess()
{
    long            DeltaX;
    long            DeltaY;

    if (XyagUnitGridStep<1e-4) return;

    DeltaX = 100 * XyagUnitGridDx / (100 - XyagPercentZoom);
    DeltaY = 100 * XyagUnitGridDy / (100 - XyagPercentZoom);

    XyagAddZoom();

    XyagUnitGridX = XyagUnitGridX - ((DeltaX - XyagUnitGridDx) >> 1);
    XyagUnitGridY = XyagUnitGridY - ((DeltaY - XyagUnitGridDy) >> 1);
    XyagUnitGridDx = DeltaX;
    XyagUnitGridDy = DeltaY;

    XyagComputeUnitGrid();
    XyagComputeScrollBar();
    XyagDisplayFigure(0, 0, XyagGraphicDx, XyagGraphicDy);
    XyagRefreshGraphicWindow(0, 0, XyagGraphicDx, XyagGraphicDy);
}

/*------------------------------------------------------------\
|                         XyagZoomFit                         |
\------------------------------------------------------------*/

void 
XyagZoomFit()
{
    if (XyagComputeBound() == XYAG_FALSE) {
        XyagErrorMessage(XyagMainWindow, "No element to display !");
    }
    else {
        XyagAddZoom();

        XyagUnitGridX = (XyagBoundXmin / XYAG_UNIT) - 1;
        XyagUnitGridY = (XyagBoundYmin / XYAG_UNIT) - 1;
        XyagUnitGridDx = XyagBoundXmax / XYAG_UNIT;
        XyagUnitGridDy = XyagBoundYmax / XYAG_UNIT;
        XyagUnitGridDx = XyagUnitGridDx - XyagUnitGridX + 1;
        XyagUnitGridDy = XyagUnitGridDy - XyagUnitGridY + 1;

        XyagComputeUnitGrid();

        if (XyagUnitGridDx > 1.1 * (XyagBoundXmax - XyagBoundXmin) / XYAG_UNIT) {
            XyagUnitGridX -= (XyagUnitGridDx - (XyagBoundXmax - XyagBoundXmin) / XYAG_UNIT) / 2;
        }
        XyagPixelGridX = (float) (XyagUnitGridX) * XyagUnitGridStep;
        if (XyagUnitGridDy > 1.1 * (XyagBoundYmax - XyagBoundYmin) / XYAG_UNIT) {
            XyagUnitGridY -= (XyagUnitGridDy - (XyagBoundYmax - XyagBoundYmin) / XYAG_UNIT) / 2;
        }
        XyagPixelGridY = (float) (XyagUnitGridY) * XyagUnitGridStep;

        XyagComputeScrollBar();
        XyagDisplayFigure(0, 0, XyagGraphicDx, XyagGraphicDy);
        XyagRefreshGraphicWindow(0, 0, XyagGraphicDx, XyagGraphicDy);
    }
}

/*------------------------------------------------------------\
|                         XyagZoomCenter                      |
\------------------------------------------------------------*/

void 
XyagZoomCenter(UnitX, UnitY)
    long            UnitX;
    long            UnitY;
{
    XyagUnitGridX = UnitX - (XyagUnitGridDx >> 1);
    XyagUnitGridY = UnitY - (XyagUnitGridDy >> 1);

    XyagComputeUnitGrid();
    XyagComputeScrollBar();
    XyagDisplayFigure(0, 0, XyagGraphicDx, XyagGraphicDy);
    XyagRefreshGraphicWindow(0, 0, XyagGraphicDx, XyagGraphicDy);
}

/*------------------------------------------------------------\
|                          XyagZoomIn                         |
\------------------------------------------------------------*/

void 
XyagZoomIn(UnitX1, UnitY1, UnitX2, UnitY2)
    long            UnitX1;
    long            UnitY1;
    long            UnitX2;
    long            UnitY2;
{
    long            Swap;

    if ((UnitX1 != UnitX2) &&
        (UnitY1 != UnitY2)) {
        if (UnitX1 > UnitX2) {
            Swap = UnitX1;
            UnitX1 = UnitX2;
            UnitX2 = Swap;
        }

        if (UnitY1 > UnitY2) {
            Swap = UnitY1;
            UnitY1 = UnitY2;
            UnitY2 = Swap;
        }

        XyagAddZoom();

        XyagUnitGridX = UnitX1;
        XyagUnitGridY = UnitY1;
        XyagUnitGridDx = UnitX2 - UnitX1;
        XyagUnitGridDy = UnitY2 - UnitY1;

        XyagComputeUnitGrid();
        XyagComputeScrollBar();
        XyagDisplayFigure(0, 0, XyagGraphicDx, XyagGraphicDy);
        XyagRefreshGraphicWindow(0, 0, XyagGraphicDx, XyagGraphicDy);
    }
}

/*------------------------------------------------------------\
|                          XyagZoomPan                        |
\------------------------------------------------------------*/

void 
XyagZoomPan(UnitX1, UnitY1, UnitX2, UnitY2)
    long            UnitX1;
    long            UnitY1;
    long            UnitX2;
    long            UnitY2;
{
    if ((UnitX1 != UnitX2) ||
        (UnitY1 != UnitY2)) {
        XyagUnitGridX = XyagUnitGridX + (UnitX1 - UnitX2);
        XyagUnitGridY = XyagUnitGridY + (UnitY1 - UnitY2);

        XyagComputeUnitGrid();
        XyagComputeScrollBar();
        XyagDisplayFigure(0, 0, XyagGraphicDx, XyagGraphicDy);
        XyagRefreshGraphicWindow(0, 0, XyagGraphicDx, XyagGraphicDy);
    }
}

/*------------------------------------------------------------\
|                          XyagZoomRefresh                    |
\------------------------------------------------------------*/

void 
XyagZoomRefresh()
{
    XyagDisplayFigure(0, 0, XyagGraphicDx, XyagGraphicDy);
    XyagRefreshGraphicWindow(0, 0, XyagGraphicDx, XyagGraphicDy);
}

/*------------------------------------------------------------\
|                         XyagConeDepRec                      |
\------------------------------------------------------------*/

chain_list *
XyagConeDepRec(RetList, CnsCone, Mode, depth,ptcell)
    chain_list     *RetList;
    cone_list      *CnsCone;
    int             Mode;
    int             depth;
    cell_list      *ptcell;
{
    edge_list      *CnsEdge;
    int             i;

    if (((depth > 0) || (depth <= -1)) || CnsCone->CELLS != NULL ) {

        if(CnsCone->CELLS == NULL) {
             i = 1; 
             ptcell = NULL;}
        else if(CnsCone->CELLS != NULL) {
             if(CnsCone->CELLS->NEXT == NULL && (cell_list *)CnsCone->CELLS->DATA != ptcell) {
                 i = 1; 
                 ptcell = (cell_list *)CnsCone->CELLS->DATA; }
             else
                 i = 0;
        }
        
        if (getptype(CnsCone->USER, 0xFFF00002) == NULL) {
            if ( i == 1 )
                RetList = addchain(RetList, CnsCone);
            
            CnsCone->USER = addptype(CnsCone->USER, 0xFFF00002, NULL);
        }
        for (CnsEdge = CnsCone->INCONE;
             CnsEdge != (edge_list *) 0;
             CnsEdge = CnsEdge->NEXT) {
            if ((Mode & CGV_SEL_PREC_LOGIC) && !(CnsEdge->TYPE & CNS_EXT)) {
                if (getptype(CnsEdge->UEDGE.CONE->USER, 0xFFF00002) == NULL) {
                    RetList = XyagConeDepRec(RetList, CnsEdge->UEDGE.CONE,
                                             CGV_SEL_PREC_LOGIC,
                                             depth - i,ptcell);
                }
            }
        }

        for (CnsEdge = CnsCone->OUTCONE;
             CnsEdge != (edge_list *) 0;
             CnsEdge = CnsEdge->NEXT) {
            if ((Mode & CGV_SEL_SUCC_LOGIC) && !(CnsEdge->TYPE & CNS_EXT)) {
                if (getptype(CnsEdge->UEDGE.CONE->USER, 0xFFF00002) == NULL) {

                    RetList = XyagConeDepRec(RetList, CnsEdge->UEDGE.CONE,
                                             CGV_SEL_SUCC_LOGIC,
                                             depth - i,ptcell);
                    if (i == 0)
                         RetList = XyagConeDepRec(RetList, CnsEdge->UEDGE.CONE,
                                             CGV_SEL_PREC_LOGIC,
                                             1, ptcell); 
                }
            }
        }
    }
    return (RetList);
}

/*------------------------------------------------------------\
|                         XyagConeDep                         |
\------------------------------------------------------------*/

chain_list *
XyagConeDep(List, Mode, depth)
    chain_list     *List;
    int             Mode;
    int             depth;
{
    chain_list     *RetList;
    chain_list     *ScanList;
    cone_list      *CnsCone;
    chain_list     *ptchaincell;
    chain_list     *ptchaincone;
    cell_list      *ptcell;
    cone_list      *ptcone;

    for (ScanList = List;
         ScanList != (chain_list *) 0;
         ScanList = ScanList->NEXT) {
        CnsCone = (cone_list *) ScanList->DATA;

        if (CnsCone != (cone_list *) 0) {
            RetList = XyagConeDepRec(NULL, CnsCone, Mode, depth,NULL);
        }
    }

    for (ScanList = RetList;
         ScanList != (chain_list *) 0;
         ScanList = ScanList->NEXT) {
        CnsCone = (cone_list *) ScanList->DATA;
        CnsCone->USER = delptype(CnsCone->USER, 0xFFF00002);

        if(CnsCone->CELLS !=NULL) {
            for(ptchaincone=CnsCone->CELLS;ptchaincone;ptchaincone=ptchaincone->NEXT){
                ptcell = (cell_list *)ptchaincone->DATA;
                for(ptchaincell=ptcell->CONES;ptchaincell;ptchaincell=ptchaincell->NEXT){
                    ptcone = (cone_list *)ptchaincell->DATA;
                    if(getptype(ptcone->USER, 0xFFF00002) != NULL)
                        ptcone->USER = delptype(CnsCone->USER, 0xFFF00002);
                }
            }
        }
    }

    return (RetList);
}


/*------------------------------------------------------------\
|                         XyagViewDep                         |
\------------------------------------------------------------*/

void XyagViewDep(X1, Y1, Mode, Mode2)
    long            X1;
    long            Y1;
    int             Mode;
    int             Mode2;
{
    xyagselect_list *Select;

    XyagEditSelectPoint(X1, Y1);

    if (XyagHeadSelect == (xyagselect_list *) NULL) {
        XyagWarningMessage(XyagMainWindow, "No element found !");
    }
    else 
      {
//	XyagDelConnect();
/*        for (*/Select = XyagHeadSelect; /*Select; Select = Select->NEXT) 
	  {*/
	    if (Mode2 == 0) {
/*	      if (!XyagAddDepExtract(Select->OBJECT, Mode))
		break;*/
	      if (IsXyagCgvBox(Select->OBJECT)) 
		{
		  cgv_extract(XyagFigureCgv, (cgvbox_list *)Select->OBJECT->USER, Mode, XyagDepthOfDep-1);
		}
	      else
		if (IsXyagCgvCon(Select->OBJECT)) 
		  {
		    cgv_extract_net(XyagFigureCgv, (cgvcon_list *)Select->OBJECT->USER, Mode, XyagDepthOfDep-1);
		  }

	      XyagDelSelect();


	      xyagSaveState();
	      XyagRecomputeBound = XYAG_TRUE;
	      XyagFigureCgv = finish_extract(XyagFigureCgv);

	      if (XyagFigureCgv != NULL) 
		{
		  OPEN_STACK=addchain(OPEN_STACK, XyagFigureCgv);
		  printf("addfigure\n");
		  XyagAddFigure();
		}

	      xyagRetreiveState();

	      XyagDisplayMessage();
//	      XyagInitializeZoom();
	      XyagDisplayFigure(0, 0, XyagGraphicDx, XyagGraphicDy);
	      
	      XyagRefreshGraphicWindow(0, 0, XyagGraphicDx, XyagGraphicDy);
	      
	      XyagSetMouseCursor(XyagGraphicWindow, XYAG_NORMAL_CURSOR);

            }
            else {
	      /*if (!XyagAddDepHilite(Select->OBJECT, Mode))
		break;*/
	      XyagDelConnect();
	       if (IsXyagCgvBox(Select->OBJECT)) 
		 cgv_hilite(XyagFigureCgv, (cgvbox_list *)Select->OBJECT->USER, Mode, XyagDepthOfDep-1, XyagAddConnect);
	       else
		 if (IsXyagCgvCon(Select->OBJECT)) 
		   cgv_hilite_net(XyagFigureCgv, (cgvcon_list *)Select->OBJECT->USER, Mode, XyagDepthOfDep-1, XyagAddConnect);
               XyagDelSelect();
            }
//	  }
	XyagZoomRefresh();
    }
 
}

void XyagThruTree(long X1, long Y1)
{
    xyagselect_list *Select;
    cgvbox_list *box;
    int nochange=0;

    XyagEditSelectPoint(X1, Y1);

    mbkSwitchContext(XYAG_ctx); // dans XYAGLE
    cnsSwitchContext(CNS_ctx); // dans XYAGLE

    if (XyagHeadSelect == NULL) 
      {
        XyagWarningMessage(XyagMainWindow, "No element found !");
      }
    else 
      {
	Select=XyagHeadSelect;

	if (IsXyagCgvCon(Select->OBJECT))
	  XyagWarningMessage(XyagMainWindow, "Can not go thru a connector !");
	else
	  {
	    if (IsXyagCgvBox(Select->OBJECT))
	      {
		box=(cgvbox_list *)Select->OBJECT->USER;
		if (box->SOURCE_TYPE == CGV_SOURCE_LOINS)
		  {
		    char temp[16];
		    loins_list *li=box->SOURCE; 
		    XyagSetMouseCursor(XyagGraphicWindow, XYAG_WATCH_CURSOR);		    
		    xyagSaveState();

		    XyagRecomputeBound = XYAG_TRUE;

		    if (strcmp(getcgvfileext(XyagFigureCgv),"cns")==0) nochange=1;

		    if (!nochange)
		      {
			strcpy(temp,IN_LO);
			strcpy(IN_LO, getcgvfileext(XyagFigureCgv));
		      }
		    XyagLoadFigureByType(li->FIGNAME, CGV_FROM_LOFIG, 0);

		    if (!nochange)
		      strcpy(IN_LO,temp);

		    xyagRetreiveState();
		    
		    if (XyagFigure != NULL) XyagChangeTopLevelTitle(XyagFigure->NAME);
		    else XyagChangeTopLevelTitle( NULL);

		    //XyagDisplayMessage();
		    
//		    XyagInitializeZoom();
		    XyagDisplayFigure(0, 0, XyagGraphicDx, XyagGraphicDy);
		    XyagRefreshGraphicWindow(0, 0, XyagGraphicDx, XyagGraphicDy);
		    XyagSetMouseCursor(XyagGraphicWindow, XYAG_NORMAL_CURSOR);
		    
		  }
                else if (box->SOURCE_TYPE == CGV_SOURCE_CNSCONE)
		  {
		    char temp[16];
                    cone_list *cn=box->SOURCE;

                    if (cn->TYPE & CNS_VDD)
                      XyagWarningMessage(XyagMainWindow, "This is a VDD alimentation !");
                    else
                      if (cn->TYPE & CNS_VSS)
                        XyagWarningMessage(XyagMainWindow, "This is a GND alimentation !");
                      else
                        if (cn->BRVDD==NULL && cn->BRVSS==NULL && cn->BREXT==NULL)
                          XyagWarningMessage(XyagMainWindow, "This is an undriven signal !");
                        else
                          {
                            XyagSetMouseCursor(XyagGraphicWindow, XYAG_WATCH_CURSOR);		    
                            xyagSaveState();
                            
                            XyagRecomputeBound = XYAG_TRUE;
                            
                            XyagGetCone(cn);
                            
                            xyagRetreiveState();
                            
                            if (XyagFigure != NULL) XyagChangeTopLevelTitle(XyagFigure->NAME);
                            else XyagChangeTopLevelTitle( NULL);
                            
//                            XyagInitializeZoom();
                            XyagDisplayFigure(0, 0, XyagGraphicDx, XyagGraphicDy);
                            XyagRefreshGraphicWindow(0, 0, XyagGraphicDx, XyagGraphicDy);
                            XyagSetMouseCursor(XyagGraphicWindow, XYAG_NORMAL_CURSOR);
                          }
		  }
                else
                  XyagWarningMessage(XyagMainWindow, "Can not go thru this object !");
	      }
	  }
      }

    cnsSwitchContext(CNS_ctx); // hors XYAGLE
    mbkSwitchContext(XYAG_ctx); // hors XYAGLE
}

void XyagHiLight(cgv_interaction *itr, int complete)
{
  if (XyagFigureCgv==NULL) return;

  XyagDelConnect();
  cgv_HiLight(XyagFigureCgv, itr, complete, XyagAddConnect);
  XyagDelSelect();
  XyagZoomRefresh();
}

void XyagExtract(cgv_interaction *itr, int complete)
{

  if (XyagFigureCgv==NULL) return;

  xyagSaveState();
  XyagRecomputeBound = XYAG_TRUE;
  XyagFigureCgv = cgv_Extract(XyagFigureCgv, itr, complete);

  if (XyagFigureCgv != NULL) 
    {
      OPEN_STACK=addchain(OPEN_STACK, XyagFigureCgv);
      printf("addfigure\n");
      XyagAddFigure();
    }


  xyagRetreiveState();
  
  XyagDisplayMessage();

//  XyagInitializeZoom();
  XyagDisplayFigure(0, 0, XyagGraphicDx, XyagGraphicDy);
  
  XyagRefreshGraphicWindow(0, 0, XyagGraphicDx, XyagGraphicDy);
  
  XyagSetMouseCursor(XyagGraphicWindow, XYAG_NORMAL_CURSOR);
}

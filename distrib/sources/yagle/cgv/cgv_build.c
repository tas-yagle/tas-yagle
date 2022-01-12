/*------------------------------------------------------------\
|                                                             |
| Tool    :                     Cgv                           |
|                                                             |
| File    :                 Cgv Build                         |
|                                                             |
| Authors :              Picault  Stephane                    |
|                        Miramond   Benoit                    |
|                                                             |
| Date    :                  04.03.98                         |
|                                                             |
\------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include MUT_H
#include SLB_H
#include CGV_H

#define BOX_SIZE_MIN 5
#define BOX_SIZE_MAX (BOX_SIZE_MIN*3)

//#define DEBUG

int CGV_SCALE = 1 ;

static int CgvNetCompare __P((cgvnet_list **, cgvnet_list **));
static int CheckNetIntersection __P((chain_list *, cgvnet_list *));

/*------------------------------------------------------------\
|                Function Loc Destroy Transparence            |
\------------------------------------------------------------*/

static void 
loc_destroytransparence(Figure, Transparence)
    cgvfig_list    *Figure;
    cgvbox_list    *Transparence;
{
    cgvnet_list    *CgvNet;
    cgvwir_list    *CgvWire;

    CgvNet = Transparence->CON_IN->NET;
    CgvWire = addcgvwir(Figure, CgvNet);

    CgvWire->X = Transparence->X + Transparence->CON_IN->X_REL;
    CgvWire->Y = Transparence->Y + Transparence->CON_IN->Y_REL;
    CgvWire->DX = Transparence->X + Transparence->CON_OUT->X_REL;
    CgvWire->DY = Transparence->Y + Transparence->CON_OUT->Y_REL;

    /*  return(0); */
}

/*------------------------------------------------------------\
|                    Function Loc Destroy Cluster             |
\------------------------------------------------------------*/

static void 
loc_destroycluster(Figure, Cluster)
    cgvfig_list    *Figure;
    cgvbox_list    *Cluster;
{
    cgvwir_list    *CgvWire;
    cgvbox_list    *Box;
    cgvbox_list    *Box1;
    cgvbox_list    *Box2;
    cgvcon_list    *Con;
    cgvcon_list    *ConCast;
    cgvnet_list    *CgvNet;
    cgvcon_list    *ClusterCon;
    chain_list     *ScanBox;
    chain_list     *ScanCon;

    ScanBox = (chain_list *) Cluster->SOURCE;
    Box = (cgvbox_list *) ScanBox->DATA;
    ScanBox = ScanBox->NEXT;
    Box2 = (cgvbox_list *) ScanBox->DATA;
    ScanBox = ScanBox->NEXT;
    Box1 = (cgvbox_list *) ScanBox->DATA;


    Box1->USER = (void *) Cluster->USER;
    Box1->X = Cluster->X;
    Box1->Y = Cluster->Y;
    Box1->DX = (Cluster->DX / 3);

    for (Con = Box1->CON_OUT;
         Con != (cgvcon_list *) 0;
         Con = Con->NEXT) {
        Con->X_REL = Box1->DX + 2;
    }

    for (Con = Box1->CON_IN;
         Con != (cgvcon_list *) 0;
         Con = Con->NEXT) {
        ClusterCon = (cgvcon_list *) Con->USER;

        CgvNet = Con->NET;

        for (ScanCon = CgvNet->CON_NET;
             ScanCon != (chain_list *) 0;
             ScanCon = ScanCon->NEXT) {
            ConCast = (cgvcon_list *) ScanCon->DATA;
            if (ConCast == ClusterCon) {
                ScanCon->DATA = (void *) Con;
            }
        }
    }

    Box2->USER = (void *) Cluster->USER;
    Box2->X = Cluster->X;
    Box2->Y = Cluster->Y + Box1->DY;
    Box2->DX = (Cluster->DX / 3);

    for (Con = Box2->CON_OUT;
         Con != (cgvcon_list *) 0;
         Con = Con->NEXT) {
        Con->X_REL = Box2->DX + 2;
    }

    for (Con = Box2->CON_IN;
         Con != (cgvcon_list *) 0;
         Con = Con->NEXT) {
        ClusterCon = (cgvcon_list *) Con->USER;

        CgvNet = Con->NET;

        for (ScanCon = CgvNet->CON_NET;
             ScanCon != (chain_list *) 0;
             ScanCon = ScanCon->NEXT) {
            ConCast = (cgvcon_list *) ScanCon->DATA;
            if (ConCast == ClusterCon) {
                ScanCon->DATA = (void *) Con;
            }
        }
    }

    Box->USER = (void *) Cluster->USER;
    Box->X = Cluster->X + ((Cluster->DX * 2) / 3);
    Box->Y = Cluster->Y + (Cluster->CON_OUT->Y_REL - 4);
    Box->DX = (Cluster->DX / 3);

    for (Con = Box->CON_OUT;
         Con != (cgvcon_list *) 0;
         Con = Con->NEXT) {
        Con->X_REL = Box->DX + 2;

        ClusterCon = (cgvcon_list *) Con->USER;

        CgvNet = Con->NET;

        for (ScanCon = CgvNet->CON_NET;
             ScanCon != (chain_list *) 0;
             ScanCon = ScanCon->NEXT) {
            ConCast = (cgvcon_list *) ScanCon->DATA;
            if (ConCast == ClusterCon) {
                ScanCon->DATA = (void *) Con;
            }
        }
    }

    CgvWire = addcgvwir(Figure, Box1->CON_OUT->NET);

    CgvWire->X = Box1->X + Box1->CON_OUT->X_REL;
    CgvWire->Y = Box1->Y + Box1->CON_OUT->Y_REL;
    CgvWire->DX = Box->X + Box->CON_IN->NEXT->X_REL;
    CgvWire->DY = Box->Y + Box->CON_IN->NEXT->Y_REL;

    CgvWire = addcgvwir(Figure, Box2->CON_OUT->NET);

    CgvWire->X = Box2->X + Box2->CON_OUT->X_REL;
    CgvWire->Y = Box2->Y + Box2->CON_OUT->Y_REL;
    CgvWire->DX = Box->X + Box->CON_IN->X_REL;
    CgvWire->DY = Box->Y + Box->CON_IN->Y_REL;
}


/*------------------------------------------------------------\
|                   Function  Loc Build Cluster               |
\------------------------------------------------------------*/
/*
static void 
loc_buildcluster(Figure)
    cgvfig_list    *Figure;
{
    cgvcon_list    *Con1;
    cgvbox_list    *Box;
    cgvcon_list    *Con2;
    cgvcon_list    *CgvCon;
    cgvcon_list    *ConCast;
    cgvcon_list    *ClusterCon;
    cgvbox_list    *Box1;
    cgvbox_list    *Box2;
    cgvbox_list    *Cluster;
    cgvnet_list    *CgvNet;

    chain_list     *ScanCon;
    int             Go;


    for (Box = Figure->BOX;
         Box != (cgvbox_list *) 0;
         Box = Box->NEXT) {

        Go = 1;

        if ((!IsCgvBoxCluster(Box)) &&
            (!IsCgvBoxClusterised(Box)) &&
            (Box->NUMBER_IN == 2) &&
            (Box->NUMBER_OUT == 1)) {
            Con2 = (cgvcon_list *) Box->CON_IN;
            Con1 = (cgvcon_list *) Box->CON_IN->NEXT;

            if ((Con1->NET->NUMBER_OUT == 1) && (Con2->NET->NUMBER_OUT == 1)
            && (Con1->NET->NUMBER_IN == 1) && (Con2->NET->NUMBER_IN == 1)) {
                for (ScanCon = Con1->NET->CON_NET;
                     ScanCon != (chain_list *) 0;
                     ScanCon = ScanCon->NEXT) {
                    ConCast = (cgvcon_list *) ScanCon->DATA;

                    if (ConCast != Con1) {
                        if (ConCast->ROOT_TYPE == CGV_ROOT_CON_BOX) {
                            Box1 = ConCast->ROOT;

                            if (IsCgvBoxClusterised(Box1)
                                || IsCgvBoxCluster(Box1)
                                || (Box1->NUMBER_OUT != 1)) {
                                Go = 0;
                            }
                        }
                        else
                            Go = 0;
                    }
                }

                if (Go == 1) {
                    for (ScanCon = Con2->NET->CON_NET;
                         ScanCon != (chain_list *) 0;
                         ScanCon = ScanCon->NEXT) {
                        ConCast = (cgvcon_list *) ScanCon->DATA;

                        if (ConCast != Con2) {
                            if (ConCast->ROOT_TYPE == CGV_ROOT_CON_BOX) {
                                Box2 = ConCast->ROOT;

                                if (IsCgvBoxClusterised(Box2)
                                    || IsCgvBoxCluster(Box2)
                                    || (Box2->NUMBER_OUT != 1)) {
                                    Go = 0;
                                }
                            }
                            else {
                                Go = 0;
                            }
                        }
                    }
                }
                if (Go == 1) {
                    Cluster = addcgvbox(Figure, "Cluster");

                    SetCgvBoxCluster(Cluster);

                    SetCgvBoxClusterised(Box1);
                    Cluster->SOURCE = (chain_list *) addchain((chain_list *) Cluster->SOURCE
                                                            ,(void *) Box1);

                    Box1->CON_IN = (cgvcon_list *) reverse((chain_list *) Box1->CON_IN);

                    for (CgvCon = Box1->CON_IN;
                         CgvCon != (cgvcon_list *) 0;
                         CgvCon = CgvCon->NEXT) {
                        ClusterCon = addcgvboxconin(Figure, Cluster, CgvCon->NAME);

                        CgvNet = CgvCon->NET;

                        for (ScanCon = CgvNet->CON_NET;
                             ScanCon != (chain_list *) 0;
                             ScanCon = ScanCon->NEXT) {
                            ConCast = (cgvcon_list *) ScanCon->DATA;
                            if (ConCast == CgvCon) {
                                ScanCon->DATA = (void *) ClusterCon;
                            }
                        }

                        ClusterCon->NET = CgvNet;

                        CgvCon->USER = (void *) ClusterCon;
                    }

                    Box1->CON_IN = (cgvcon_list *) reverse((chain_list *) Box1->CON_IN);

                    SetCgvBoxClusterised(Box2);
                    Cluster->SOURCE = (chain_list *) addchain((chain_list *) Cluster->SOURCE
                                                            ,(void *) Box2);

                    Box2->CON_IN = (cgvcon_list *) reverse((chain_list *) Box2->CON_IN);

                    for (CgvCon = Box2->CON_IN;
                         CgvCon != (cgvcon_list *) 0;
                         CgvCon = CgvCon->NEXT) {
                        ClusterCon = addcgvboxconin(Figure, Cluster, CgvCon->NAME);

                        CgvNet = CgvCon->NET;

                        for (ScanCon = CgvNet->CON_NET;
                             ScanCon != (chain_list *) 0;
                             ScanCon = ScanCon->NEXT) {
                            ConCast = (cgvcon_list *) ScanCon->DATA;
                            if (ConCast == CgvCon) {
                                ScanCon->DATA = (void *) ClusterCon;
                            }
                        }

                        ClusterCon->NET = CgvNet;

                        CgvCon->USER = (void *) ClusterCon;

                    }

                    Box2->CON_IN = (cgvcon_list *) reverse((chain_list *) Box2->CON_IN);

                    SetCgvBoxClusterised(Box);
                    Cluster->SOURCE = (chain_list *) addchain((chain_list *) Cluster->SOURCE
                                                              ,(void *) Box);

                    for (CgvCon = Box->CON_OUT;
                         CgvCon != (cgvcon_list *) 0;
                         CgvCon = CgvCon->NEXT) {
                        ClusterCon = addcgvboxconout(Figure, Cluster, CgvCon->NAME);

                        CgvNet = CgvCon->NET;

                        for (ScanCon = CgvNet->CON_NET;
                             ScanCon != (chain_list *) 0;
                             ScanCon = ScanCon->NEXT) {
                            ConCast = (cgvcon_list *) ScanCon->DATA;
                            if (ConCast == CgvCon) {
                                ScanCon->DATA = (void *) ClusterCon;
                            }
                        }

                        ClusterCon->NET = CgvNet;
                        CgvCon->USER = (void *) ClusterCon;
                    }
                }
            }
        }
    }
}
*/

/*------------------------------------------------------------\
|                  Function  Loc Build Place First            |
\------------------------------------------------------------*/
/*
static cgvcol_list *
loc_buildplacefirst(Figure)
    cgvfig_list    *Figure;
{
    cgvcol_list    *ColMax;
    cgvcel_list    *Cell;
    cgvcol_list    *Column;
    cgvbox_list    *Box;
    cgvcon_list    *Con;

    long            MaxCumulY = 0;
    long            CumulY;

#ifndef __ALL__WARNING__
    Figure = NULL;
#endif

    for (Column = HEAD_CGVCOL;
         Column != (cgvcol_list *) 0;
         Column = Column->NEXT) {
        CumulY = 0;

        for (Cell = Column->CELL;
             Cell != (cgvcel_list *) 0;
             Cell = Cell->NEXT) {
            if (Cell->TYPE == CGV_CEL_BOX) {
                Box = (cgvbox_list *) Cell->ROOT;
                Box->Y = CumulY;
                CumulY += Box->DY + 5*CGV_SCALE;
                if (Box->DX > Column->MAX_DX_CELL) {
                    Column->MAX_DX_CELL = Box->DX;
                }
            }
            else if (Cell->TYPE == CGV_CEL_CON) {
                Con = (cgvcon_list *) Cell->ROOT;
                Con->Y_REL = CumulY;
                CumulY += 3*CGV_SCALE;
                if (2*CGV_SCALE > Column->MAX_DX_CELL) {
                    Column->MAX_DX_CELL = 2*CGV_SCALE;
                }
            }
        }
        if (MaxCumulY < CumulY) {
            ColMax = Column;
            MaxCumulY = CumulY;
        }
    }
    for (Cell = ColMax->CELL;
         Cell != (cgvcel_list *) 0;
         Cell = Cell->NEXT) {
        if (Cell->TYPE == CGV_CEL_BOX) {
            SetCgvBoxPlaced((cgvbox_list *) Cell->ROOT);
        }
        if (Cell->TYPE == CGV_CEL_CON) {
            SetCgvConPlaced((cgvcon_list *) Cell->ROOT);
        }

    }
    return (ColMax);
}
*/


/*------------------------------------------------------------\
|                Function  Loc Build Place In                 |
\------------------------------------------------------------*/

/*
static cgvfig_list *
loc_buildplacein(Figure, ColumnFrom, ColumnTo)
    cgvfig_list    *Figure;
    cgvcol_list    *ColumnFrom;
    cgvcol_list    *ColumnTo;
{
    cgvcel_list    *CellTo;
    cgvcon_list    *ConTo;
    cgvbox_list    *BoxTo;

    cgvcel_list    *CellFrom;
    cgvbox_list    *BoxFrom;
    cgvcon_list    *ConFrom;

    chain_list     *Chain;
    char            Begin = 1;
    long            CumulY;

    for (CellTo = ColumnTo->LAST_CELL; CellTo; CellTo = CellTo->PREV) {
        if (CellTo->TYPE == CGV_CEL_BOX) {
            BoxTo = (cgvbox_list *) CellTo->ROOT;

            for (ConTo = BoxTo->CON_IN; ConTo; ConTo = ConTo->NEXT) {
                for (Chain = ConTo->NET->CON_NET; Chain; Chain = Chain->NEXT) {
                    ConFrom = (cgvcon_list *) Chain->DATA;

                    if (IsCgvConInternal(ConFrom) && IsCgvConOut(ConFrom)) {
                        BoxFrom = (cgvbox_list *) ConFrom->ROOT;

                        if (IsCgvBoxTaged(BoxFrom) && !IsCgvBoxPlaced(BoxFrom)) {
                            CellFrom = (cgvcel_list *) BoxFrom->USER;
                            if (CellFrom->COL == ColumnFrom) {
                                if (Begin) {
                                    // juste une idee de grandeur 
                                    CumulY = BoxTo->Y + 2 * (BoxFrom->DY) + (BoxTo->DY);
                                    Begin = 0;
                                }

                                if ((BoxTo->Y + ConTo->Y_REL - ConFrom->Y_REL + BoxFrom->DY) <= CumulY) {
                                    BoxFrom->Y = BoxTo->Y + ConTo->Y_REL - ConFrom->Y_REL;
                                }
                                else {
                                    BoxFrom->Y = CumulY - BoxFrom->DY;
                                }
                                CumulY = BoxFrom->Y - CGV_SCALE;
                                SetCgvBoxPlaced(BoxFrom);
                            }
                        }
                    }
                    if (IsCgvConExternal(ConFrom) && IsCgvConIn(ConFrom)
                    && !IsCgvConPlaced(ConFrom) && IsCgvConTaged(ConFrom)) {
                        CellFrom = (cgvcel_list *) ConFrom->USER;
                        if (CellFrom->COL == ColumnFrom) {

                            if (!IsCgvConPlaced(ConFrom)) {
                                if (Begin) {
                                    CumulY = BoxTo->Y + 2 * (BoxFrom->DY) + (BoxTo->DY);
                                    Begin = 0;
                                }

                                if (BoxTo->Y + ConTo->Y_REL <= CumulY) {
                                    ConFrom->Y_REL = BoxTo->Y + ConTo->Y_REL;
                                }
                                else {
                                    ConFrom->Y_REL = CumulY - 5*CGV_SCALE;
                                }
                                SetCgvConPlaced(ConFrom);
                                CumulY = ConFrom->Y_REL - CGV_SCALE;
                            }
                        }
                    }
                }
            }
        }
    }
    return (Figure);
}
*/

/*------------------------------------------------------------\
|                Function  Loc Build Place Out                |
\------------------------------------------------------------*/

/*
static cgvfig_list *
loc_buildplaceout(Figure, ColumnFrom, ColumnTo)
    cgvfig_list    *Figure;
    cgvcol_list    *ColumnFrom;
    cgvcol_list    *ColumnTo;
{
    chain_list     *Chain;

    char            Begin = 1;

    cgvbox_list    *FirstBox = NULL;
    cgvbox_list    *BoxFrom;
    cgvcon_list    *ConFrom;
    cgvcel_list    *CellFrom;

    cgvcon_list    *ConTo;
    cgvcel_list    *CellTo;
    cgvbox_list    *BoxTo;
    long            CumulY;

    for (CellTo = ColumnTo->LAST_CELL; CellTo; CellTo = CellTo->PREV) {
        if (CellTo->TYPE == CGV_CEL_BOX) {
            BoxTo = (cgvbox_list *) CellTo->ROOT;

            for (ConTo = BoxTo->CON_IN; ConTo && !IsCgvBoxPlaced(BoxTo); ConTo = ConTo->NEXT) {
                for (Chain = ConTo->NET->CON_NET; Chain && !IsCgvBoxPlaced(BoxTo); Chain = Chain->NEXT) {
                    ConFrom = (cgvcon_list *) Chain->DATA;
                    if (ConFrom != ConTo) {
                        if (IsCgvConInternal(ConFrom) && IsCgvConOut(ConFrom)) {
                            BoxFrom = (cgvbox_list *) ConFrom->ROOT;
                            if (BoxFrom->NUMBER_OUT == 0) {
                                continue;
                            }
                            
                            if (IsCgvBoxTaged(BoxFrom)) {
                                CellFrom = (cgvcel_list *) BoxFrom->USER;
                                if (CellFrom->COL == ColumnFrom) {
                                    if (Begin) {
                                        // juste une idee de grandeur  
                                        CumulY = BoxFrom->Y +  2 * (BoxTo->DY + BoxFrom->DY);
                                        Begin = 0;
                                    }
                                    if ((BoxFrom->Y + ConFrom->Y_REL - ConTo->Y_REL + BoxTo->DY) <= CumulY) {
                                        SetCgvBoxPlaced(BoxTo);
                                        BoxTo->Y = BoxFrom->Y + ConFrom->Y_REL - ConTo->Y_REL;
                                        CumulY = BoxTo->Y - CGV_SCALE;
                                    }
                                    else {
                                        //BoxTo->Y =  CumulY - BoxTo->DY;
                                    }
                                    // 3 fev 2000
                                    if (FirstBox != NULL) {
                                        if (!IsCgvBoxPlaced(FirstBox)) {
                                            FirstBox->Y = BoxTo->Y + BoxTo->DY + 2*CGV_SCALE;
                                            SetCgvBoxPlaced(FirstBox);
                                        }
                                    }
                                    // fin
                                }
                            }
                        }
                    }
                }
            }
            if (!IsCgvBoxPlaced(BoxTo)) {
                // 3 fev 2000
                if (Begin) {
                    FirstBox = BoxTo;
                }
                // fin
                else {
                    BoxTo->Y = CumulY - BoxTo->DY;
                    SetCgvBoxPlaced(BoxTo);
                    CumulY = BoxTo->Y - CGV_SCALE;
                }
            }
        }

        if (CellTo->TYPE == CGV_CEL_CON) {
            ConTo = (cgvcon_list *) CellTo->ROOT;

            for (Chain = ConTo->NET->CON_NET; Chain && !IsCgvConPlaced(ConTo); Chain = Chain->NEXT) {
                ConFrom = (cgvcon_list *) Chain->DATA;
                if (ConFrom != ConTo) {
                    if (IsCgvConInternal(ConFrom) && IsCgvConOut(ConFrom)) {
                        BoxFrom = (cgvbox_list *) ConFrom->ROOT;
                        if (IsCgvBoxTaged(BoxFrom)) {
                            CellFrom = (cgvcel_list *) BoxFrom->USER;
                            if (CellFrom->COL == ColumnFrom) {
                                if (Begin) {
                                    CumulY = BoxFrom->Y + 2 * (BoxFrom->DY);
                                    Begin = 0;
                                }
                                if ((BoxFrom->Y + ConFrom->Y_REL + 2*CGV_SCALE) <= CumulY) {
                                    SetCgvConPlaced(ConTo);
                                    ConTo->Y_REL = BoxFrom->Y + ConFrom->Y_REL;
                                    CumulY = ConTo->Y_REL - CGV_SCALE;
                                }
                                else {
                                    // ConTo->Y_REL =  CumulY;
                                }
                            }
                        }
                    }
                }
            }
            if (Begin && IsCgvConPlaced(ConTo)) {
                ConTo->Y_REL = CumulY;
                SetCgvConPlaced(ConTo);
                CumulY = ConTo->Y_REL - CGV_SCALE;
            }
        }
    }
    return (Figure);
}
*/

/*------------------------------------------------------------\
|                  Function  Loc Build Place                  |
\------------------------------------------------------------*/
/*
static cgvfig_list *
loc_buildplace(Figure)
    cgvfig_list    *Figure;
{

    cgvcol_list    *Column;
    cgvcol_list    *MaxColumn;

    MaxColumn = loc_buildplacefirst(Figure);

    for (Column = MaxColumn; Column->NEXT; Column = Column->NEXT) {
        loc_buildplaceout(Figure, Column, Column->NEXT);
    }

    HEAD_CGVCOL = (cgvcol_list *) reverse((chain_list *) HEAD_CGVCOL);

    for (Column = MaxColumn; Column->NEXT; Column = Column->NEXT) {
        loc_buildplacein(Figure, Column->NEXT, Column);
    }

    HEAD_CGVCOL = (cgvcol_list *) reverse((chain_list *) HEAD_CGVCOL);

    return (Figure);
}
*/

/*------------------------------------------------------------\
|                 Function  Loc Routage Canal                 |
\------------------------------------------------------------*/

static long 
loc_routage_canal(Figure, ColumnFrom, ColumnTo, Xdep)
    cgvfig_list    *Figure;
    cgvcol_list    *ColumnFrom;
    cgvcol_list    *ColumnTo;
    long            Xdep;
{
    cgvcel_list    *ScanCell;
    cgvbox_list    *CgvBox;
    cgvnet_list    *CgvNet;
    cgvnet_list   **CgvNetTable;
    cgvwir_list    *CgvWire;
    cgvcon_list    *ScanCon;
    cgvcon_list    *CgvCon;
    chain_list     *CgvNetList = NULL;
    chain_list     *CurChannelNets = NULL;
    chain_list     *ptchain;
    long            numcgvnet = 0;
    long            channel;
    long            i = 0;

    Xdep += ColumnFrom->MAX_DX_CELL;

    /* Traverse Canal Input and Output Connectors to */
    /* obtain list of CgvNets in Canal and their     */
    /* vertical extremities with direction           */

    /* Channel Input Connectors                              */

    for (ScanCell = ColumnFrom->CELL; ScanCell; ScanCell = ScanCell->NEXT) {
        if (ScanCell->TYPE == CGV_CEL_BOX) {
            CgvBox = (cgvbox_list *) ScanCell->ROOT;
            for (ScanCon = CgvBox->CON_OUT; ScanCon; ScanCon = ScanCon->NEXT) {
                CgvNet = ScanCon->NET;
                if (!IsCgvNetCanal(CgvNet)) {
                    numcgvnet++;
                    SetCgvNetCanal(CgvNet);
                    CgvNetList = addchain(CgvNetList, CgvNet);
                    CgvNet->YMAX = CgvBox->Y + ScanCon->Y_REL;
                    CgvNet->YMIN = CgvBox->Y + ScanCon->Y_REL;
                }
                else if ((CgvBox->Y + ScanCon->Y_REL) > (CgvNet->YMAX)) {
                    CgvNet->YMAX = CgvBox->Y + ScanCon->Y_REL;
                }
                else if ((CgvBox->Y + ScanCon->Y_REL) < (CgvNet->YMIN)) {
                    CgvNet->YMIN = CgvBox->Y + ScanCon->Y_REL;
                }
            }
        }
        else if (ScanCell->TYPE == CGV_CEL_CON) {
            CgvCon = (cgvcon_list *) ScanCell->ROOT;

            CgvNet = CgvCon->NET;
            if (!IsCgvNetCanal(CgvNet)) {
                numcgvnet++;
                SetCgvNetCanal(CgvNet);
                CgvNetList = addchain(CgvNetList, CgvNet);
                CgvNet->YMAX = CgvCon->Y_REL;
                CgvNet->YMIN = CgvCon->Y_REL;
            }
            if ((CgvCon->Y_REL) > (CgvNet->YMAX)) {
                CgvNet->YMAX = CgvCon->Y_REL;
            }
            if ((CgvCon->Y_REL) < (CgvNet->YMIN)) {
                CgvNet->YMIN = CgvCon->Y_REL;
            }
        }
    }

    /* Channel Output connectors */

    for (ScanCell = ColumnTo->CELL; ScanCell; ScanCell = ScanCell->NEXT) {
        if (ScanCell->TYPE == CGV_CEL_BOX) {
            CgvBox = (cgvbox_list *) ScanCell->ROOT;
            for (ScanCon = CgvBox->CON_IN; ScanCon; ScanCon = ScanCon->NEXT) {
                CgvNet = ScanCon->NET;
                if (!IsCgvNetCanal(CgvNet)) {
                    numcgvnet++;
                    SetCgvNetCanal(CgvNet);
                    CgvNetList = addchain(CgvNetList, CgvNet);
                    CgvNet->YMAX = CgvBox->Y + ScanCon->Y_REL;
                    SetCgvNetMaxOutput(CgvNet);
                    CgvNet->YMIN = CgvBox->Y + ScanCon->Y_REL;
                    SetCgvNetMinOutput(CgvNet);
                }
                else if ((CgvBox->Y + ScanCon->Y_REL) > (CgvNet->YMAX)) {
                    CgvNet->YMAX = CgvBox->Y + ScanCon->Y_REL;
                    SetCgvNetMaxOutput(CgvNet);
                }
                else if ((CgvBox->Y + ScanCon->Y_REL) < (CgvNet->YMIN)) {
                    CgvNet->YMIN = CgvBox->Y + ScanCon->Y_REL;
                    SetCgvNetMinOutput(CgvNet);
                }
            }
        }
        else if (ScanCell->TYPE == CGV_CEL_CON) {
            CgvCon = (cgvcon_list *) ScanCell->ROOT;

            CgvNet = CgvCon->NET;
            if (!IsCgvNetCanal(CgvNet)) {
                numcgvnet++;
                SetCgvNetCanal(CgvNet);
                CgvNetList = addchain(CgvNetList, CgvNet);
                CgvNet->YMAX = CgvCon->Y_REL;
                SetCgvNetMaxOutput(CgvNet);
                CgvNet->YMIN = CgvCon->Y_REL;
                SetCgvNetMinOutput(CgvNet);
            }
            if ((CgvCon->Y_REL) > (CgvNet->YMAX)) {
                CgvNet->YMAX = CgvCon->Y_REL;
                SetCgvNetMaxOutput(CgvNet);
            }
            if ((CgvCon->Y_REL) < (CgvNet->YMIN)) {
                CgvNet->YMIN = CgvCon->Y_REL;
                SetCgvNetMinOutput(CgvNet);
            }
        }
    }

    /* build table of CgvNets and sort                       */

    CgvNetTable = (cgvnet_list **) mbkalloc(numcgvnet * sizeof(cgvnet_list *));
    for (ptchain = CgvNetList; ptchain; ptchain = ptchain->NEXT) {
        CgvNetTable[i++] = (cgvnet_list *) ptchain->DATA;
    }
    qsort(CgvNetTable, numcgvnet, sizeof(cgvnet_list *), (void *) CgvNetCompare);

    /* Allocate a channel number to each CgvNet              */

    channel = Xdep + 4*CGV_SCALE;
    for (i = 0; i < numcgvnet; i++) {
        if ((CgvNetTable[i])->YMAX == (CgvNetTable[i])->YMIN) {
            (CgvNetTable[i])->CANAL = channel;
            continue;
        }
        if (CheckNetIntersection(CurChannelNets, CgvNetTable[i])) {
            channel += 2*CGV_SCALE;
            if (CurChannelNets)
                freechain(CurChannelNets);
            CurChannelNets = addchain(NULL, CgvNetTable[i]);
        }
        else
            CurChannelNets = addchain(CurChannelNets, CgvNetTable[i]);
        (CgvNetTable[i])->CANAL = channel;
    }
    mbkfree(CgvNetTable);

    /* Draw horizontal wires from input column to vertical wire */

    for (ScanCell = ColumnFrom->CELL; ScanCell; ScanCell = ScanCell->NEXT) {
        if (ScanCell->TYPE == CGV_CEL_BOX) {
            CgvBox = ScanCell->ROOT;

            for (ScanCon = CgvBox->CON_OUT; ScanCon; ScanCon = ScanCon->NEXT) {
                CgvNet = ScanCon->NET;
                if (CgvNet->CON_NET->NEXT == NULL) continue;
                CgvWire = addcgvwir(Figure, CgvNet);
                CgvWire->X = CgvBox->X + ScanCon->X_REL;
                CgvWire->DX = CgvNet->CANAL;
                CgvWire->Y = CgvBox->Y + ScanCon->Y_REL;
                CgvWire->DY = CgvBox->Y + ScanCon->Y_REL;
            }
        }
        else if (ScanCell->TYPE == CGV_CEL_CON) {
            CgvCon = (cgvcon_list *) ScanCell->ROOT;
            CgvNet = CgvCon->NET;
            CgvWire = addcgvwir(Figure, CgvNet);
            CgvWire->X = CgvCon->X_REL + 2*CGV_SCALE;
            CgvWire->DX = CgvNet->CANAL;
            CgvWire->Y = CgvCon->Y_REL;
            CgvWire->DY = CgvCon->Y_REL;
        }
    }

    /* Draw horizontal wires from vertical wire to output column */

    for (ScanCell = ColumnTo->CELL; ScanCell; ScanCell = ScanCell->NEXT) {
        if (ScanCell->TYPE == CGV_CEL_BOX) {
            CgvBox = ScanCell->ROOT;
            CgvBox->X = channel + 6*CGV_SCALE + (ColumnTo->MAX_DX_CELL - CgvBox->DX) / 2;

            for (ScanCon = CgvBox->CON_IN; ScanCon; ScanCon = ScanCon->NEXT) {
                CgvNet = ScanCon->NET;
                if (CgvNet->CON_NET->NEXT == NULL) continue;
                CgvWire = addcgvwir(Figure, CgvNet);
                CgvWire->X = CgvBox->X + ScanCon->X_REL;
                CgvWire->DX = CgvNet->CANAL;
                CgvWire->Y = CgvBox->Y + ScanCon->Y_REL;
                CgvWire->DY = CgvBox->Y + ScanCon->Y_REL;
            }
        }
        else if (ScanCell->TYPE == CGV_CEL_CON) {
            CgvCon = (cgvcon_list *) ScanCell->ROOT;
            CgvCon->X_REL = channel + 6*CGV_SCALE;
            if (!(IsCgvConIn(CgvCon) && IsCgvConExternal(CgvCon))) {
                CgvNet = CgvCon->NET;
                CgvWire = addcgvwir(Figure, CgvNet);
                CgvWire->X = CgvCon->X_REL - 2*CGV_SCALE;
                CgvWire->DX = CgvNet->CANAL;
                CgvWire->Y = CgvCon->Y_REL;
                CgvWire->DY = CgvCon->Y_REL;
            }
        }
    }

    /* Draw vertical wires */

    for (ptchain = CgvNetList; ptchain; ptchain = ptchain->NEXT) {
        CgvNet = (cgvnet_list *) ptchain->DATA;
        ClearCgvNetCanal(CgvNet);
        ClearCgvNetDir(CgvNet);
        if (CgvNet->YMIN == CgvNet->YMAX) {
            continue;
        }
        CgvWire = addcgvwir(Figure, CgvNet);
        CgvWire->X = CgvNet->CANAL;
        CgvWire->DX = CgvNet->CANAL;
        CgvWire->Y = CgvNet->YMIN;
        CgvWire->DY = CgvNet->YMAX;
    }
    freechain(CgvNetList);

    return (channel + 6*CGV_SCALE);
}

/*------------------------------------------------------------\
|                 Function  CgvNetCompare                     |
\------------------------------------------------------------*/

static int 
CgvNetCompare(net1, net2)
    cgvnet_list   **net1;
    cgvnet_list   **net2;
{
    long            type1, type2;

    type1 = GetCgvNetDirType(*net1);
    type2 = GetCgvNetDirType(*net2);

    if (type1 > type2) {
        return (1);
    }
    if (type1 < type2) {
        return (-1);
    }

    switch (type1) {
    case 0:
        return (((*net1)->YMAX - (*net1)->YMIN) - ((*net2)->YMAX - (*net2)->YMIN));
        break;
    case CGV_NET_MINDIR:
        return ((*net1)->YMIN - (*net2)->YMIN);
        break;
    case CGV_NET_MAXDIR:
        return ((*net2)->YMAX - (*net1)->YMAX);
        break;
    case CGV_NET_MAXDIR | CGV_NET_MINDIR:
        return (((*net2)->YMAX - (*net2)->YMIN) - ((*net1)->YMAX - (*net1)->YMIN));
        break;
    }
    return 0;
}

/*------------------------------------------------------------\
|                 Function  CgvNetCompare                     |
\------------------------------------------------------------*/

static int 
CheckNetIntersection(NetList, Net)
    chain_list     *NetList;
    cgvnet_list    *Net;
{
    chain_list     *ptchain;
    cgvnet_list    *CmpNet;

    for (ptchain = NetList;
         ptchain != (chain_list *) 0;
         ptchain = ptchain->NEXT) {
        CmpNet = (cgvnet_list *) ptchain->DATA;
        if ((Net->YMIN > CmpNet->YMAX) || (Net->YMAX < CmpNet->YMIN)) {
            continue;
        }
        else {
            break;
        }
    }
    if (ptchain != NULL) {
        return (1);
    }
    else {
        return (0);
    }
}


/*------------------------------------------------------------\
|                      Function  Loc Route                    |
\------------------------------------------------------------*/

cgvfig_list *loc_route(Figure)
    cgvfig_list    *Figure;
{
    cgvcol_list    *Column_from;
    cgvcol_list    *Column_to;

    long            Xdep = 0;

    for (Column_from = HEAD_CGVCOL;
         Column_from->NEXT != (cgvcol_list *) 0;
         Column_from = Column_from->NEXT) {
        Column_to = Column_from->NEXT;
        Xdep = loc_routage_canal(Figure, Column_from, Column_to, Xdep);
    }
    return (Figure);
}


/*------------------------------------------------------------\
|                Function  Loc Add Transparence               |
\------------------------------------------------------------*/

cgvbox_list *loc_add_transparence(Figure, Net)
    cgvfig_list    *Figure;
    cgvnet_list    *Net;
{
    cgvbox_list    *Box;
    cgvcon_list    *Con_in;
    cgvcon_list    *Con_out;
    static int      num = 1;
    char            buffer[1024];


    sprintf(buffer,"transparence_%d", num++);
    Box = addcgvbox(Figure, namealloc(buffer));
    SetCgvBoxTransparence(Box);

    Con_out = addcgvboxconout(Figure, Box, "T_out");
    Con_in = addcgvboxconin(Figure, Box, "T_in");

    addcgvnetcon(Net, Con_out);
    addcgvnetcon(Net, Con_in);

    return (Box);
}


/*------------------------------------------------------------\
|                   Function  Loc Test Trans                  |
+-------------------------------------------------------------+
|     renvoie  1  si un Net traverse une colonne              |
|              0  sinon                                       |
\------------------------------------------------------------*/

int 
loc_test_trans(Net, Column)
    cgvnet_list    *Net;
    cgvcol_list    *Column;
{
    cgvcel_list    *CgvCel;
    cgvcon_list    *CgvCon;
    cgvbox_list    *CgvBox;
    chain_list     *ChainCon;

    for (ChainCon = Net->CON_NET;
         ChainCon != (chain_list *) 0;
         ChainCon = ChainCon->NEXT) {
        CgvCon = (cgvcon_list *) ChainCon->DATA;
        if (IsCgvConInternal(CgvCon)) {
            CgvBox = (cgvbox_list *) CgvCon->ROOT;
            if (IsCgvBoxTransparence(CgvBox)) {
                CgvCel = (cgvcel_list *) CgvBox->USER;
                if (CgvCel->COL == Column) {
                    return (0);
                }
            }
        }
    }
    return (1);
}

/*------------------------------------------------------------\
|        loc_isanyconbefore(Cell,FirstColumn)                 |
\------------------------------------------------------------*/
static int
loc_isanyconbefore(Cell,FirstColumn)
    cgvcel_list   *Cell;
    cgvcol_list   *FirstColumn;
{
    cgvcol_list   *Current;
    cgvcol_list   *Previous;
    cgvcol_list   *Box2Col;
    cgvbox_list   *CellBox;
    cgvbox_list   *Box2;
    cgvnet_list   *Net;
    cgvcon_list   *Con2;
    chain_list    *Chain;
    cgvcel_list   *Cell2Col;
    
    Current = Cell->COL;
    CellBox = (cgvbox_list*)Cell->ROOT; // CellBox est une transparence
    Net = CellBox->CON_OUT->NET; 

    for (Chain = Net->CON_NET; Chain;Chain = Chain->NEXT) {
        Con2 = (cgvcon_list *) Chain->DATA;
        Box2 = (cgvbox_list*)Con2->ROOT;
        if (IsCgvBoxTaged(Box2) && !IsCgvBoxTransparence(Box2) ) {
             Cell2Col = (cgvcel_list*)Box2->USER;
             Box2Col  = Cell2Col->COL;
             for (Previous=FirstColumn; Previous != Current->NEXT; Previous = Previous->NEXT) {
                 if (Previous == Box2Col) return 1;
             }
                
        }
        else if (IsCgvConExternal(Con2))
             return 1;
    }
    return 0;
}

/*------------------------------------------------------------\
|              loc_placelostcell(FirstColumn)                 |
\------------------------------------------------------------*/
void loc_placelostcell(FirstColumn,Figure)
cgvcol_list   *FirstColumn;
cgvfig_list    *Figure;
{

cgvcon_list    *Con;
cgvcon_list    *Con2;
chain_list     *Chain;
cgvbox_list    *Box;
cgvbox_list    *Box2;
cgvcol_list    *Column;
cgvcol_list    *Column2;
cgvcel_list    *Cell;
cgvcel_list    *Cell2;
cgvcel_list    *ptnextcell;

for (Column = FirstColumn;Column;Column=Column->NEXT) {
   for (Cell = Column->CELL; Cell; Cell = ptnextcell) {
       if (Cell->TYPE == CGV_CEL_BOX) {
           Box = (cgvbox_list *) Cell->ROOT;
           
           if (Box->CON_OUT) 
               Con = Box->CON_OUT;
           else
               continue;
           
           if (Column->NEXT) 
               Column2=Column->NEXT;
           else
               Column2=Column;
           
           for (Chain = Con->NET->CON_NET; Chain; Chain = Chain->NEXT) {
               Con2 = (cgvcon_list *) Chain->DATA;
               if (Con2 != Con) {
                   Box2 = (cgvbox_list *) Con2->ROOT;
                   if (IsCgvConIn(Con2) && !IsCgvBoxTaged(Box2)  
                                   && Con2->ROOT_TYPE == CGV_ROOT_CON_BOX){
                        if(IsCgvBoxTransparence(Box)) {
                           if(!loc_isanyconbefore(Cell,FirstColumn)) { 
                             Cell2 = Cell;
                             if (Cell2->PREV) {
                               Cell = Cell2->PREV;
                               if (Cell2->NEXT) {
                                 Cell->NEXT = Cell2->NEXT;
                                 Cell->NEXT->PREV = Cell;
                               } // if (Cell2->NEXT)
                               else {
                                 Cell->NEXT = NULL;
                                 Column->LAST_CELL = Cell;
                               } //  !Cell2->NEXT
                             } // if (Cell2->PREV)
                             else {
                               Column->CELL = Cell2->NEXT;
                               if (Cell2->NEXT) {
                                 Cell2->NEXT->PREV = NULL;
                               } // if (Cell2->NEXT)
                               else {
                                 Cell->NEXT = NULL;
                                 Column->LAST_CELL = Cell;
                               } //  !Cell2->NEXT
                             } // !Cell2->PREV
                             cgvdeltransparence(Cell2,Figure);
                             Column->NUMBER_CELL--;
                           } // if(!loc_isanyconbefore(Cell,FirstColumn))
                        } // if(IsCgvBoxTransparence(Box))
                        else {
                          SetCgvBoxTaged(Box2);
                          Cell2 = addcgvcel(Column2);
                          addcgvboxtocgvcel(Box2, Cell2);
                          Column2->NUMBER_CELL++;
                        } // !IsCgvBoxTransparence(Box)
                   } // if (IsCgvConIn(Con2))
               } // if  Con2 != Con
           } // for Chain  
       } // if (Cell->TYPE == CGV_CEL_BOX)  
       ptnextcell = Cell->NEXT;
   }    // for Cell
} // for Column

       
}

/*------------------------------------------------------------\
|               Function  Loc Placement Initial               |
+-------------------------------------------------------------+
|       placement initial, dans les colonnes                  |
\------------------------------------------------------------*/
cgvbox_list *removebox(cgvbox_list *list, cgvbox_list *b)
{
  cgvbox_list *c0;
  if (b==list)
    {
      return list->NEXT;
    }
  else
    {
      for (c0=list; c0!=NULL && c0->NEXT!=b; c0=c0->NEXT) ;
      if (c0!=NULL) c0->NEXT=c0->NEXT->NEXT;
    }
  return list;
}

void removefake(cgvfig_list *cgvf, cgvcol_list *col)
{
  cgvcel_list *c, *cel, *cel0;
  cgvcon_list *con, *boxcon, *c0;
  cgvbox_list *box;
  chain_list *cl, *pred;
  cgvcol_list *col0;

  for (c=col->CELL; c!=NULL; c=c->NEXT)
    {
      if (c->TYPE == CGV_CEL_CON)
	{
	  con=(cgvcon_list *)c->ROOT;
//	  printf("check con  %s\n",con->NAME);
	  if (strcmp(con->NAME,"$$**$$")==0)
	    {
	      if (col->CELL==c)
		{
		  col->CELL=col->CELL->NEXT;
		}
	      else
		{
		  for (cel=col->CELL; cel->NEXT!=c; cel=cel->NEXT) ;
		  cel->NEXT=cel->NEXT->NEXT;
		}
	      col->NUMBER_CELL--;

	      if (con==cgvf->CON_OUT)
		{
		  cgvf->CON_OUT=cgvf->CON_OUT->NEXT;
		}
	      else
		{
		  for (c0=cgvf->CON_OUT; c0->NEXT!=con; c0=c0->NEXT) ;
		  c0->NEXT=c0->NEXT->NEXT;
		}
	      cgvf->NUMBER_OUT--;

	      for (cl=con->NET->CON_NET; cl!=NULL; cl=cl->NEXT)
		{
		  boxcon=(cgvcon_list *)cl->DATA;
//		  printf("%s\n",boxcon->NAME);
		  if (IsCgvConExternal(boxcon) && IsCgvConOut(boxcon)) continue;

		  box=(cgvbox_list *)boxcon->ROOT;
		  if (IsCgvBoxTransparence(box)) 
		    {
		      cel0=(cgvcel_list *)box->USER;
		      col0=col->NEXT;
		      if (col0->CELL==cel0)
			{
			  col0->CELL=col0->CELL->NEXT;
			}
		      else
			{
			  for (cel=col0->CELL; cel!=NULL && cel->NEXT!=cel0; cel=cel->NEXT) ;
			  if (cel!=NULL) cel->NEXT=cel->NEXT->NEXT;
			}
		      col0->NUMBER_CELL--;
		      cgvf->BOX=removebox(cgvf->BOX, box);
		      continue;
		    }

		  pred=cl;

		  //printf("remove on %s\n",box->NAME);
		  
		  if (boxcon==box->CON_OUT)
		    {
		      box->CON_OUT=box->CON_OUT->NEXT;
		    }
		  else
		    {
		      for (c0=box->CON_OUT; c0->NEXT!=boxcon; c0=c0->NEXT) ;
		      c0->NEXT=c0->NEXT->NEXT;
		    }
//                  printf("remove on %s, out=%d, %p\n",box->NAME,box->NUMBER_OUT,box->CON_OUT);
		}
	      con->NET->CON_NET=NULL;
	    }
	}
    }
}

void removecon(chain_list **ch, cgvcon_list *con)
{
  chain_list *cl=*ch, *temp;
  if (cl->DATA==con) { *ch=cl->NEXT; cl->NEXT=NULL; freechain(cl); return ;}
  for (; cl->NEXT!=NULL; cl=cl->NEXT)
    if (cl->NEXT->DATA==con)
      {
        temp=cl->NEXT;
        cl->NEXT=temp->NEXT; temp->NEXT=NULL; freechain(temp); 
        break;
      }
  if (cl==NULL) avt_errmsg(CGV_ERRMSG, "001", AVT_ERROR, 1);
                // printf("NOT FOUND\n");
}
int cgv_has_input(chain_list *cl)
{
  cgvcon_list *con;
  while (cl!=NULL)
    {
      con=(cgvcon_list *)cl->DATA;
      if (IsCgvConIn(con) && IsCgvConExternal(con)) return 1;      
      cl=cl->NEXT;
    }
  return 0;
}
cgvcol_list *merge(cgvfig_list *cgvf, cgvcol_list *old, cgvcol_list *new)
{
  cgvcol_list *prev_new, *oldold, *prev_old, *nextnew;
  cgvcel_list *c, *cel, *next;
  cgvbox_list *Box, *pBox, *nextb;
  chain_list *cl;
  cgvcon_list *con;
  ht *h;

  old = (cgvcol_list *) reverse((chain_list *) old);
  oldold=old;
  new = (cgvcol_list *) reverse((chain_list *) new);  
  
  while (old!=NULL && new!=NULL)
    {
      h=addht(100);
      for (c=old->CELL; c!=NULL; c=c->NEXT)
        {
          if (c->TYPE == CGV_CEL_BOX)
	    {
              Box=(cgvbox_list *)c->ROOT;
	      if (IsCgvBoxTransparence(Box))
                  addhtitem(h, Box->CON_IN->NET, (long)c);
            }
        }
      for (c=new->CELL; c!=NULL; c=c->NEXT) 
        { 
          if (c->TYPE == CGV_CEL_BOX)
	    {
              Box=(cgvbox_list *)c->ROOT;
	      if (IsCgvBoxTransparence(Box))
                {
                  if (gethtitem(h, Box->CON_IN->NET)!=EMPTYHT)
                    {
                      Box->NAME=NULL;
                    }
                  else
                    {
                      addhtitem(h, Box->CON_IN->NET, (long)c);
                    }
                }
            }
          c->COL=old; old->NUMBER_CELL++;
        }
      delht(h);
      old->CELL=(cgvcel_list *)append((chain_list *)old->CELL, (chain_list *)new->CELL);
      prev_old=old;
      old=old->NEXT;
      prev_new=new;
      new=new->NEXT;
    }

  if (prev_new!=NULL)
  while (old!=NULL)
    {      
      for (c=prev_new->CELL; c!=NULL; c=c->NEXT)
	{
	  if (c->TYPE == CGV_CEL_BOX)
	    {
	      Box=(cgvbox_list *)c->ROOT;
	      if (Box->NAME!=NULL && IsCgvBoxTransparence(Box) && cgv_has_input(Box->CON_IN->NET->CON_NET) && loc_test_trans(Box->CON_IN->NET, old))
		{
		  Box = loc_add_transparence(cgvf, Box->CON_IN->NET);
		  Box->NAME="sup0";
		  SetCgvBoxTaged(Box);
		  cel = addcgvcel(old);
		  addcgvboxtocgvcel(Box, cel);
		  SetCgvBoxTaged(Box);
		  SetCgvBoxReTaged(Box);
		}
	    }
	}
      old=old->NEXT;
    }

  if (prev_old!=NULL)
  while (new!=NULL)
    {
      nextnew=new->NEXT;
      prev_old->NEXT=new;
      new->NEXT=NULL;
      for (c=prev_old->CELL; c!=NULL; c=c->NEXT)
	{
	  if (c->TYPE == CGV_CEL_BOX)
	    {
	      Box=(cgvbox_list *)c->ROOT;
	      if (IsCgvBoxTransparence(Box) && cgv_has_input(Box->CON_IN->NET->CON_NET) 
		  && loc_test_trans(Box->CON_IN->NET, new))
		{
		  Box = loc_add_transparence(cgvf, Box->CON_IN->NET);
		  Box->NAME="sup";
		  SetCgvBoxTaged(Box);
		  cel = addcgvcel(new);
		  addcgvboxtocgvcel(Box, cel);
		  SetCgvBoxTaged(Box);
		  SetCgvBoxReTaged(Box);
		}
	    }
	}
      prev_old=new;// oldold=prev_old;
      new=nextnew;
    }

  for (Box=cgvf->BOX, pBox=NULL; Box!=NULL; Box=nextb)
    {
      nextb=Box->NEXT;
      if (Box->NAME==NULL)
        {
          if (pBox==NULL) cgvf->BOX=Box->NEXT;
          else pBox->NEXT=Box->NEXT;          
        }
      else
        pBox=Box;
    }

  oldold=(cgvcol_list *) reverse((chain_list *) oldold);

  for (prev_old=NULL, old=oldold; old!=NULL; prev_old=old, old=old->NEXT)
    {
      for (c=old->CELL; c!=NULL; c=c->NEXT)
	{
	  if (c->TYPE == CGV_CEL_BOX)
	    {
              Box=(cgvbox_list *)c->ROOT;
              if (Box->NAME==NULL) continue;
//              printf("-- %s",Box->NAME);
              if (IsCgvBoxTransparence(Box) && !cgv_has_input(Box->CON_IN->NET->CON_NET))
                {
                  for (cl=Box->CON_IN->NET->CON_NET; cl!=NULL; cl=cl->NEXT)
                    {
                      con=(cgvcon_list *)cl->DATA;
                      if (IsCgvConExternal(con)) continue;
                      pBox=(cgvbox_list *)con->ROOT;
                      if (pBox==Box || pBox->NAME==NULL) continue;
                      cel=pBox->USER;
                      if (cel->COL==prev_old || cel->COL==old) 
                        {
//                          printf(">%s<",pBox->NAME);
                          break;
                        }
                    }
                  if (cl==NULL)
                    {
                      Box->NAME=NULL;
//                      printf(" X");
                    }
                }
//              printf(" -\n");
            }
        }
    }

  for (old=oldold; old!=NULL; old=old->NEXT)
    {      
      for (c=old->CELL, cel=NULL; c!=NULL; c=next)
	{
          next=c->NEXT;
	  if (c->TYPE == CGV_CEL_BOX)
	    {
              Box=(cgvbox_list *)c->ROOT;
              if (Box->NAME==NULL)
                {
                  printf("remove\n");
                  old->NUMBER_CELL--;
                  if (cel==NULL) old->CELL=c->NEXT;
                  else cel->NEXT=c->NEXT;
                  removecon(&Box->CON_IN->NET->CON_NET, Box->CON_IN);
                  removecon(&Box->CON_OUT->NET->CON_NET, Box->CON_OUT);
                  continue;
                }
              else cel=c;
            }
          else
            cel=c;
        }

       for (c=old->CELL; c!=NULL; c=c->NEXT)
	{
	  if (c->TYPE == CGV_CEL_BOX)
	    {
              Box=(cgvbox_list *)c->ROOT;
              if (Box->NAME==NULL) EXIT(8);
            }
        }
    }

  for (Box=cgvf->BOX, pBox=NULL; Box!=NULL; Box=nextb)
    {
      nextb=Box->NEXT;
      if (Box->NAME==NULL)
        {
          if (pBox==NULL) cgvf->BOX=Box->NEXT;
          else pBox->NEXT=Box->NEXT;          
        }
      else
        pBox=Box;
    }

  for (Box=cgvf->BOX; Box!=NULL; Box=Box->NEXT)
    {
      if (Box->NAME==NULL) EXIT(9);
    }
  
  return oldold;
}

void markhitfromout(chain_list *cl)
{
  cgvcon_list *c;
  cgvbox_list *b;
  while (cl!=NULL)
    {
      c=(cgvcon_list *)cl->DATA;
      if (IsCgvConOut(c) && IsCgvConInternal(c))
	{
	  b=(cgvbox_list *)c->ROOT;
	  if (!IsCgvBoxHit(b))
	    {
	      SetCgvBoxHit(b);
	      for (c=b->CON_IN; c!=NULL; c=c->NEXT)
                {
                  if (c->NET->CANAL==0)
                    {
                      markhitfromout(c->NET->CON_NET);
                      c->NET->CANAL=1;
                    }
                }
	    }
	}
      cl=cl->NEXT;
    }
}

static int unplaced=0;
static cgvfig_list *loc_placement_initial(cgvfig_list *Figure)
{
    int             cpt;
    cgvcon_list    *Con;
    cgvcon_list    *Con2;
    chain_list     *Chain;

    cgvbox_list    *Box;
    cgvbox_list    *Box2;
    cgvbox_list    *Box4;
    cgvcol_list    *Column2, *firstcolumn;
    cgvcol_list    *Column;
    cgvcel_list    *Cell;
    cgvcel_list    *Cell2;

    cgvbox_list    *BoxBox4, *b;
    cgvcel_list    *BoxCell2;
    cgvnet_list *net;

    long            cpt_transparence = 0;
    int             nb_conin = 0;

    // on compte les connecteurs d'entree +

    nb_conin = 0;
    
    for (Con = Figure->CON_IN;
         Con != (cgvcon_list *) 0;
         Con = Con->NEXT) {
        nb_conin++;
    }

    /* connecteurs externes de sortie dans la premiere colonne */

    firstcolumn = Column2 = addcgvcol();
    Column = addcgvcol();

    for (net=Figure->NET; net!=NULL; net=net->NEXT) net->CANAL=0; // marquage

    for (Con = Figure->CON_OUT; Con != NULL; Con = Con->NEXT) {
      if (IsCgvConTaged(Con)) continue;

        addcgvcontocgvcel(Con, addcgvcel(Column2));
	SetCgvConTaged(Con);
	markhitfromout(Con->NET->CON_NET);
        for (Chain = Con->NET->CON_NET;
             Chain != (chain_list *) 0;
             Chain = Chain->NEXT) {
            if (loc_test_trans(Con->NET, Column)) {
                Box2 = loc_add_transparence(Figure, Con->NET);
                /*cpt_transparence++;*/
                SetCgvBoxTaged(Box2);
                Cell2 = addcgvcel(Column);
                addcgvboxtocgvcel(Box2, Cell2);
            }
        }
    }
#ifdef DEBUG
    fprintf(stdout,"loop:");fflush(stdout);
#endif
    while ((Column->NUMBER_CELL != 0)
           && (Column->NUMBER_CELL != cpt_transparence)) {
        Column2 = Column;
        Column = addcgvcol();
#ifdef DEBUG
	fprintf(stdout,".");fflush(stdout);
#endif

        cpt_transparence = 0;

        cpt = Column2->NUMBER_CELL;

        for (Cell = Column2->CELL;
             Cell != (cgvcel_list *) 0;
             Cell = Cell->NEXT) {
            if (Cell->TYPE == CGV_CEL_BOX) {
                Box = (cgvbox_list *) Cell->ROOT;
                SetCgvBoxReTaged(Box);
            }
        }

	for (Cell = Column2->LAST_CELL; Cell!=NULL; Cell = (cgvcel_list *) Cell->PREV) 
	  {
	    if (Cell->TYPE == CGV_CEL_BOX) 
	      {
		Box = (cgvbox_list *) Cell->ROOT;
/*		if (!IsCgvBoxTransparence(Box))
		for (Con = Box->CON_IN; Con ; Con = Con->NEXT) 
		  {
		    for (Chain = Con->NET->CON_NET; Chain != NULL; Chain = Chain->NEXT) 
		      {
			Con2 = (cgvcon_list *) Chain->DATA;
			if (Con != Con2) 
			  {
			    b=(cgvbox_list *) Con2->ROOT;
			    if (IsCgvConIn(Con2) && IsCgvConInternal(Con2)
				&& !IsCgvBoxTaged(b))
			      {
				b->misc.UNDER=Box;
				SetCgvBoxTaged(b);
				SetCgvBoxReTaged(b);
				Cell2 = addcgvcel(Column2);
				addcgvboxtocgvcel(b, Cell2);
//				printf("add %s under %s\n",b->NAME,Box->NAME);
			      }
			  }
		      }
		    
		  }
*/
		if (!IsCgvBoxTransparence(Box))
		for (Con = Box->CON_OUT; Con ; Con = Con->NEXT) 
		  {
		    for (Chain = Con->NET->CON_NET; Chain != NULL; Chain = Chain->NEXT) 
		      {
			Con2 = (cgvcon_list *) Chain->DATA;
			if (Con != Con2) 
			  {
			    b=(cgvbox_list *) Con2->ROOT;
			    if (IsCgvConOut(Con2) && IsCgvConInternal(Con2)
				&& !IsCgvBoxReTaged(b))
			      {
                                if (!IsCgvBoxTaged(b))
                                  {
                                    b->misc.UNDER=Box;
                                    SetCgvBoxTaged(b);
                                    SetCgvBoxReTaged(b);
                                    Cell2 = addcgvcel(Column2);
                                    addcgvboxtocgvcel(b, Cell2);
                                    //				printf("add %s under %s\n",b->NAME,Box->NAME);
                                  }
                                else
                                  if (!IsCgvBoxReTaged(b) && loc_test_trans(Con2->NET, Column2))
                                    {
                                      Box4 = loc_add_transparence(Figure, Con2->NET);
                                      //Box4->NAME="2";
                                      SetCgvBoxTaged(Box4);
                                      SetCgvBoxReTaged(Box4);
                                      Cell2 = addcgvcel(Column2);
                                      addcgvboxtocgvcel(Box4, Cell2);
                                      Box4->misc.UNDER=Box;
                                    }
                              }

			    if (!IsCgvBoxHit(b) && IsCgvConIn(Con2) && IsCgvConInternal(Con2)
				&& !IsCgvBoxTaged(b))
			      {
//				b->misc.UNDER=Box;
				SetCgvBoxTaged(b);
//				SetCgvBoxReTaged(b);
				Cell2 = addcgvcel(Column);
				addcgvboxtocgvcel(b, Cell2);
//				printf("add %s under %s\n",b->NAME,Box->NAME);
			      }
			  }
		      }
		    
		  }
	      }
	  }


        for (Cell = Column2->LAST_CELL; Cell!=NULL; Cell = Cell->PREV) 
	  {
	    if (Cell->TYPE == CGV_CEL_BOX) {
	      Box = (cgvbox_list *) Cell->ROOT;
	      for (Con = Box->CON_IN; Con ; Con = Con->NEXT) 
	      {
		for (Chain = Con->NET->CON_NET; Chain != NULL; Chain = Chain->NEXT) 
		  {
		    Con2 = (cgvcon_list *) Chain->DATA;
		    b=(cgvbox_list *) Con2->ROOT;
		    if (Con != Con2) 
		      {
			if (IsCgvConOut(Con2) && IsCgvConInternal(Con2)
			    && !IsCgvBoxTaged(b))
			  {
			    SetCgvBoxTaged(b);
			    Cell2 = addcgvcel(Column);
			    addcgvboxtocgvcel(b, Cell2);
			  }
			if (IsCgvConIn(Con2) && IsCgvConExternal(Con2) 
			    /*&& !IsCgvBoxTaged(b)*/ && loc_test_trans(Con2->NET, Column)) 
			  {
			    Box4 = loc_add_transparence(Figure, Con2->NET);
			    Box4->NAME="3";
			    cpt_transparence++;
			    SetCgvBoxTaged(Box4);
			    Cell2 = addcgvcel(Column);
			    addcgvboxtocgvcel(Box4, Cell2);
//			    Box4->misc.UNDER=b;
			  }
			if (IsCgvConIn(Con2) && IsCgvConInternal(Con2) 
			    && !IsCgvBoxReTaged(b) && loc_test_trans(Con2->NET, Column)) 
			  {
			    Box4 = loc_add_transparence(Figure, Con2->NET);
//                            printf("=2=> %s\n", Box->NAME);
			    Box4->NAME="2";
			    cpt_transparence++;
			    SetCgvBoxTaged(Box4);
			    Cell2 = addcgvcel(Column);
			    addcgvboxtocgvcel(Box4, Cell2);
			    Box4->misc.UNDER=b;
			  }
			if (IsCgvConOut(Con2) && IsCgvConExternal(Con2) 
			    && loc_test_trans(Con2->NET, Column2)) 
			  {
//                            printf("=1=> %s\n", Box->NAME);
			    Box4 = loc_add_transparence(Figure, Con2->NET);
			    Box4->NAME="1";
			    SetCgvBoxTaged(Box4);
			    SetCgvBoxReTaged(Box4);
			    Cell2 = addcgvcel(Column2);
			    addcgvboxtocgvcel(Box4, Cell2);
//			    Box4->misc.UNDER=b;
			  }
/*			if (IsCgvConOut(Con2) && IsCgvConInternal(Con2) && !IsCgvBoxReTaged(b)
			    && loc_test_trans(Con->NET, Column2)) 
			  {
			    Box4 = loc_add_transparence(Figure, Con2->NET);
			    Box4->NAME="5";
			    SetCgvBoxTaged(Box4);
			    SetCgvBoxReTaged(Box4);
			    Cell2 = addcgvcel(Column2);
			    addcgvboxtocgvcel(Box4, Cell2);
			    Box4->misc.UNDER=b;
			  }*/
		      }
		  }

	      }
	      if (!IsCgvBoxTransparence(Box))
	      for (Con = Box->CON_OUT; Con ; Con = Con->NEXT) 
		{
		  for (Chain = Con->NET->CON_NET; Chain != NULL; Chain = Chain->NEXT) 
		    {
		      Con2 = (cgvcon_list *) Chain->DATA;
		      b=(cgvbox_list *) Con2->ROOT;
		      if (Con != Con2) 
			{
			  if (IsCgvConIn(Con2) && IsCgvConInternal(Con2)
			      && loc_test_trans(Con->NET, Column2))
			    {
			      if (!IsCgvBoxReTaged(b) || ((cgvcel_list *)b->USER)->COL==Column2)
				{
				  BoxBox4 = loc_add_transparence(Figure, Con->NET);
				  //BoxBox4->NAME = namealloc("OUT_In_Int_Tag");
				  SetCgvBoxTaged(BoxBox4);
				  SetCgvBoxReTaged(BoxBox4);
				  BoxCell2 = addcgvcel(Column2);
				  addcgvboxtocgvcel(BoxBox4, BoxCell2);
				  BoxBox4->misc.UNDER=b;
				}
			    }
			  if (IsCgvConIn(Con2) && IsCgvConExternal(Con2)
			      && loc_test_trans(Con->NET, Column2)) 
			    {
			      Box4 = loc_add_transparence(Figure, Con2->NET);
			      //Box4->NAME="6";
			      SetCgvBoxTaged(Box4);
			      SetCgvBoxReTaged(Box4);
			      Cell2 = addcgvcel(Column2);
			      addcgvboxtocgvcel(Box4, Cell2);
			      Box4->misc.UNDER=b;
			    }
			}
		    }
		}		  
	    }
	  }
    }
        
// les connecteurs externes d'entrees...        +
#ifdef DEBUG
    fprintf(stdout,"\n");
#endif
/*
    for (Cell = Column2->CELL; Cell != NULL; Cell = Cell->NEXT) 
      {
	if (Cell->TYPE == CGV_CEL_BOX) {
	  Box = (cgvbox_list *) Cell->ROOT;
	  SetCgvBoxReTaged(Box);
	}
      }
*/

   for (Box=Figure->BOX; Box!=NULL; Box=Box->NEXT)
      {
	if (!IsCgvBoxTaged(Box) && !IsCgvBoxTransparence(Box))
	  {
	    cgvnet_list *n;
	    cgvcon_list *c;
	    cgvcol_list *oldHEAD_CGVCOL;

	    n=addcgvnet(Figure);
            Box->CON_OUT=(cgvcon_list *)reverse((chain_list *)Box->CON_OUT);
	    c=addcgvboxcon(Figure, Box, "$$**$$", CGV_CON_OUT);
            Box->CON_OUT=(cgvcon_list *)reverse((chain_list *)Box->CON_OUT);
            Box->NUMBER_OUT--;
	    addcgvnetcon(n, c);
	    c=addcgvfigcon(Figure, "$$**$$", CGV_CON_OUT);
	    addcgvnetcon(n, c);
	    oldHEAD_CGVCOL=HEAD_CGVCOL;
	    HEAD_CGVCOL=NULL;
#ifdef DEBUG
	    fprintf(stdout,"placing unplaced from %s\n",Box->NAME);
#endif
	    unplaced++;
	    Figure=loc_placement_initial(Figure);
	    unplaced--;

//	    HEAD_CGVCOL=oldHEAD_CGVCOL;
	    Column=HEAD_CGVCOL=merge(Figure, oldHEAD_CGVCOL, HEAD_CGVCOL);
	  }
      }

    if (unplaced!=0) return Figure;
    
    Column2 = Column;
    Column = addcgvcol();
    cpt = Column2->NUMBER_CELL;

    for (Cell = Column2->CELL; Cell!=NULL; Cell = (cgvcel_list *) Cell->NEXT) 
      {
        if (Cell->TYPE == CGV_CEL_BOX) 
	  {
            Box = (cgvbox_list *) Cell->ROOT;
            Con2 = Box->CON_IN;
            for (Chain = Con2->NET->CON_NET; Chain != NULL; Chain = Chain->NEXT) 
	      {
		Con = (cgvcon_list *) Chain->DATA;
		if (IsCgvConExternal(Con) && IsCgvConIn(Con) && !IsCgvConTaged(Con)) 
		  {
		    Cell2 = addcgvcel(Column);
		    addcgvcontocgvcel(Con, Cell2);
		    SetCgvConTaged(Con);
		  }
	      }
	  }
      }

 
    
/*    printf("lost cell\n");
  loc_placelostcell(Column,Figure);
*/
  for (Con = Figure->CON_IN; Con != NULL; Con = Con->NEXT) 
    {
      if (!IsCgvConTaged(Con))
	{
//	  printf("-- %s\n",Con->NAME);
	  Cell2 = addcgvcel(Column);
	  addcgvcontocgvcel(Con, Cell2);
	  SetCgvConUnTaged(Con);
        }
    }


    
    return (Figure);
}

/*------------------------------------------------------------\
|                    Function  Loc Build Box                  |
\------------------------------------------------------------*/

void default_box(cgvbox_list *Box,  int H_ref)
{
  cgvcon_list    *Con;
  long            Y_cont1;
  long            Y_cont2;
  float            H_cell_ref;
  float            H_cell_x;
/*
  if(Box->SOURCE_TYPE == CGV_SOURCE_CNSCELL) {
    H_cell_ref   = XYAG_DEFAULT_FACTOR * H_ref;
    H_cell_x     = XYAG_DEFAULT_FACTOR;}
  else */{
    H_cell_ref   = H_ref*0.7;
    H_cell_x     = 1;
  }
  
  if (Box->NUMBER_IN >= Box->NUMBER_OUT) {
    Box->DY = H_cell_ref * (Box->NUMBER_IN)*CGV_SCALE;
    Y_cont1 = Box->DY - (H_cell_ref / 2)*CGV_SCALE;
    Y_cont2 = Box->DY - (Box->NUMBER_IN - Box->NUMBER_OUT) * (H_cell_ref / 2)*CGV_SCALE - (H_cell_ref / 2)*CGV_SCALE;
  }
  else {
    Box->DY = H_cell_ref * ((Box->NUMBER_OUT)*CGV_SCALE);
    Y_cont1 = Box->DY - (Box->NUMBER_OUT - Box->NUMBER_IN) * (H_cell_ref / 2)*CGV_SCALE - (H_cell_ref / 2)*CGV_SCALE;
    Y_cont2 = Box->DY - (H_cell_ref / 2)*CGV_SCALE;
  }
  if (Box->DY<3*CGV_SCALE) Box->DY=3*CGV_SCALE;
  if (IsCgvBoxCluster(Box)) {
    //Box->DX = BOX_SIZE_MAX*CGV_SCALE;
    Box->DX = Box->DY * (1+exp(-(Box->NUMBER_IN))) / H_cell_x ;
  }
  else {
    if (IsCgvBoxTransparence(Box)) {
      //Box->DX = BOX_SIZE_MAX*CGV_SCALE;
      Box->DX = Box->DY * (1+exp(-(Box->NUMBER_IN))) / H_cell_x ;
    }
    else {
      if ( (Box->DY / (H_cell_x*1.5) ) >= BOX_SIZE_MAX*CGV_SCALE) {
	Box->DX = BOX_SIZE_MAX*CGV_SCALE;
      }
      else {
	if ( (Box->DY / (H_cell_x*1.5) ) <= BOX_SIZE_MIN*CGV_SCALE) {
	  Box->DX = BOX_SIZE_MIN*CGV_SCALE;
	}
	else {
	  Box->DX = Box->DY  / (H_cell_x*1.5) ;
	}
      }
    }
  }

  for (Con = Box->CON_IN;
       Con != (cgvcon_list *) 0;
       Con = Con->NEXT) {
    Con->X_REL = 0;
    Con->Y_REL = Y_cont1;
    Y_cont1 = Y_cont1 - H_cell_ref*CGV_SCALE;
  }
  for (Con = Box->CON_OUT;
       Con != (cgvcon_list *) 0;
       Con = Con->NEXT) {
    Con->X_REL = Box->DX ;
    Con->Y_REL = Y_cont2;
    Y_cont2 = Y_cont2 - H_cell_ref*CGV_SCALE;
  }
}

cgvfig_list *loc_buildbox(cgvfig_list *Figure)
{
    cgvbox_list    *Box;
    long H_ref;

    if ( Figure->LIBRARY != NULL) 
        H_ref = Figure->LIBRARY->REFHEIGHT ;
    else
        H_ref = XYAG_DEFAULT_HEIGHT;
    
    for (Box = Figure->BOX;
         Box != (cgvbox_list *) 0;
         Box = Box->NEXT) {
        if (Box->SYMBOL != NULL) {
                
            slib_box    *ptbound;
            cgvcon_list *ptcgvcon;
            pin         *ptslibpin;
 
            ptbound = Box->SYMBOL->BOX;
            
            Box->DX = ptbound->X_MAX - ptbound->X_MIN ;
            Box->DY = ptbound->Y_MAX - ptbound->Y_MIN ;
             
            for(ptcgvcon=Box->CON_IN;ptcgvcon;ptcgvcon=ptcgvcon->NEXT) {
              if (ptcgvcon->SUB_NAME!=NULL)
                ptslibpin = slib_getpin(Box->SYMBOL,ptcgvcon->SUB_NAME);
              else
                ptslibpin = slib_getpin(Box->SYMBOL,ptcgvcon->NAME);

              if (ptslibpin==NULL)
                {
                  default_box(Box, H_ref);
                  Box->SYMBOL=NULL;
                  break;
                }
              ptcgvcon->X_REL = ptslibpin->X - ptbound->X_MIN ;
              ptcgvcon->Y_REL = ptslibpin->Y - ptbound->Y_MIN ;
            }
            if (ptcgvcon) continue;
            for(ptcgvcon=Box->CON_OUT;ptcgvcon;ptcgvcon=ptcgvcon->NEXT) {
	      if (strcmp(ptcgvcon->NAME,"$$**$$")==0) continue; // "fake for unplaced" connector
             if (ptcgvcon->SUB_NAME!=NULL)
               ptslibpin = slib_getpin(Box->SYMBOL,ptcgvcon->SUB_NAME);
             else
               ptslibpin = slib_getpin(Box->SYMBOL,ptcgvcon->NAME);
             if (ptslibpin==NULL)
               {
                 default_box(Box, H_ref);
                 Box->SYMBOL=NULL;
                 break;
               }
             ptcgvcon->X_REL = ptslibpin->X - ptbound->X_MIN ;
             ptcgvcon->Y_REL = ptslibpin->Y - ptbound->Y_MIN ;
            }
            
        }
        else {
	  default_box(Box, H_ref);
        }
    }
    return (Figure);
}

/*------------------------------------------------------------\
|                    Function  Loc Clean All                  |
\------------------------------------------------------------*/

void loc_clean_all(Figure)
    cgvfig_list    *Figure;
{
    cgvbox_list    *ScanBox;

    for (ScanBox = Figure->BOX;
         ScanBox != (cgvbox_list *) 0;
         ScanBox = ScanBox->NEXT) {
        if (IsCgvBoxCluster(ScanBox)) {
            loc_destroycluster(Figure, ScanBox);
        }
        if (IsCgvBoxTransparence(ScanBox)) {
            loc_destroytransparence(Figure, ScanBox);
        }
    }
    delcgvcol();
}

/*------------------------------------------------------------\
|                       Function  Set Scale                   |
\------------------------------------------------------------*/

void
cgv_setscale(scale)
       int        scale;
{
        CGV_SCALE = scale;
}

/*------------------------------------------------------------\
|                         Function  Build                     |
\------------------------------------------------------------*/


/* zinaps */
FILE *f;

int comp_y(const void *a, const void *b)
{
  cgvcel_list *a0=*(cgvcel_list **)a;
  cgvcel_list *b0=*(cgvcel_list **)b;
  int ay, by;
  if (a0->TYPE == CGV_CEL_BOX) ay=((cgvbox_list *)a0->ROOT)->Y;
  else ay=((cgvcon_list *)a0->ROOT)->Y_REL;
  if (b0->TYPE == CGV_CEL_BOX) by=((cgvbox_list *)b0->ROOT)->Y;
  else by=((cgvcon_list *)b0->ROOT)->Y_REL;
  if (ay<by) return -1;
  if (ay>by) return 1;
  return 0;
}

int comp_name(const void *a, const void *b)
{
  cgvcel_list **a0=(cgvcel_list **)a;
  cgvcel_list **b0=(cgvcel_list **)b;
  if (strcmp(((cgvcon_list *)(*a0)->ROOT)->NAME,((cgvcon_list *)(*b0)->ROOT)->NAME)<0) return -1;
  if (strcmp(((cgvcon_list *)(*a0)->ROOT)->NAME,((cgvcon_list *)(*b0)->ROOT)->NAME)>0) return 1;
  return 0;
}


#define POUR_SORTIES 0
#define POUR_ENTREES 1
#define FAILED_MOYENNE -123456

int addmarge(cgvbox_list *box)
{
  if (!IsCgvBoxTransparence(box)) return MARGE;
  return 0;
}

//static int DEBUG=0;

int comp_moyenne(cgvcon_list *con, cgvcol_list *pred, int dir)
{
  chain_list *cl;
  cgvcon_list *con0;
  cgvcel_list *c;
  int cmoyenne=0, cnb=0, hastransp=1;
  cgvbox_list *b0;
  for (cl = con->NET->CON_NET; cl!=NULL; cl=cl->NEXT) 
    {
      con0 = (cgvcon_list *) cl->DATA;
      b0=(cgvbox_list *)con0->ROOT;

      if (con0!=con/* && IsCgvBoxTaged(b0)*/)
	{
	  if (IsCgvConExternal(con0))
	    {
	      c=(cgvcel_list *)con0->USER;
	      if (c->COL==pred && ((dir==POUR_SORTIES && IsCgvConIn(con0)) || (dir==POUR_ENTREES && IsCgvConOut(con0))))
		{
		  if (hastransp) { cnb=0; cmoyenne=0; hastransp=0; }
		  cmoyenne+=con0->Y_REL; cnb++; 
//		  if (DEBUG)  printf("c+(%s): %d ",con0->NAME,con0->Y_REL);
		}
	    }
	  else
	    {
	      c=(cgvcel_list *)b0->USER;
	      if ((!IsCgvBoxTransparence(b0) 
		   && ((dir==POUR_SORTIES && IsCgvConOut(con0)) 
		       || (dir==POUR_ENTREES && IsCgvConIn(con0))))
		  || (hastransp && IsCgvBoxTransparence(b0) && IsCgvConIn(con0)))
		{
		  if (/*IsCgvBoxTaged(b0) &&*/ c->COL==pred)
		    {
		      if (!IsCgvBoxTransparence(b0) && hastransp)
			{ cnb=0; cmoyenne=0; hastransp=0; }
		      cmoyenne+=b0->Y+con0->Y_REL; cnb++;
//		      if (DEBUG)  printf("c1(%s)+: %d ",con0->NAME,con0->Y_REL);
		    }
		}
	    }
	}
    }
  if (cnb==0) return FAILED_MOYENNE;
  return cmoyenne/cnb;
}

int comp_moyenne_last_chance(cgvcon_list *con)
{
  chain_list *cl;
  cgvcon_list *con0;
  int cmoyenne=0, cnb=0;
  cgvbox_list *b0;
  for (cl = con->NET->CON_NET; cl!=NULL; cl=cl->NEXT) 
    {
      con0 = (cgvcon_list *) cl->DATA;
      b0=(cgvbox_list *)con0->ROOT;
//      if (!IsCgvBoxTaged(b0)) continue;
      if (con0!=con)
	{
	  if (IsCgvConExternal(con0))
	    {
	      cmoyenne+=con0->Y_REL; cnb++; 
	    }
	  else
	    {
	      if (!IsCgvBoxTransparence(b0)
		  || (IsCgvBoxTransparence(b0) && IsCgvConIn(con0)))
		{
		  cmoyenne+=b0->Y+con0->Y_REL; cnb++;
		}
	    }
	}
    }
  if (cnb==0) return FAILED_MOYENNE;
  return cmoyenne/cnb;
}
int comp_moyenne_sans_transparence(cgvcon_list *con)
{
  chain_list *cl;
  cgvcon_list *con0;
//  cgvcel_list *c;
  int upper=12345678, y=FAILED_MOYENNE;
  cgvbox_list *b0;
  for (cl = con->NET->CON_NET; cl!=NULL; cl=cl->NEXT) 
    {
      con0 = (cgvcon_list *) cl->DATA;
      b0=(cgvbox_list *)con0->ROOT;
      if (!IsCgvBoxTaged(b0)) continue;
      if (con0!=con)
	{
	  if (IsCgvConExternal(con0))
	    {
	      y=con0->Y_REL;
	      if (y<upper) upper=y;
	    }
	  else
	    {
	      if (!IsCgvBoxTransparence(b0))
		{
		  y=b0->Y-MARGE;
		  if (y<upper) upper=y;
		}
	    }
	}
    }
  return y;
}


chain_list *move_all_transparence_to(cgvcon_list *con, int y)
{
  chain_list *cl, *cl1=NULL;
  cgvcon_list *con0;
  cgvbox_list *b0;
  for (cl = con->NET->CON_NET; cl!=NULL; cl=cl->NEXT) 
    {
      con0 = (cgvcon_list *) cl->DATA;
      b0=(cgvbox_list *)con0->ROOT;
      if (!IsCgvConExternal(con0) && !IsCgvBoxTaged(b0) &&
	  (!IsCgvBoxTransparence(b0) || (IsCgvBoxTransparence(b0) && IsCgvConIn(con0))))
	{
	  b0->Y=y;
	  cl1=addchain(cl1, b0);
	}
    }
  return cl1;
}



int notplacedmode=0;

void aligner_par_rapport_aux_entrees(cgvcol_list *cur, cgvcol_list *pred, int avoidinterface)
{
  cgvcon_list *con;
  cgvcel_list *cell;
  int cmoyenne, cnb, tmoyenne, tmoyenne0, tnb;
  cgvbox_list *box;

  if (pred==NULL)
    {
      int y=0, cnt;
      cgvcel_list **tab;
      if (!avoidinterface)
	{
	  for (cell = cur->CELL, cnt=0; cell; cell = cell->NEXT, cnt++) ;
	  
	  tab=(cgvcel_list **)mbkalloc(sizeof(cgvcel_list *)*cnt);
	  for (cell = cur->CELL, cnt=0; cell; cell = cell->NEXT, cnt++) tab[cnt]=cell;
	  
	  qsort(tab, cnt, sizeof(cgvcel_list *), comp_name);
	  
	  for (y= 0; y<cnt; y++) 
	    {
	      con = (cgvcon_list *) tab[y]->ROOT;
	      con->Y_REL = y*100*CGV_SCALE;
	      box = (cgvbox_list *)con->ROOT;
//	      fprintf(f,"con %s: y=%d\n",con->NAME, y);
	    }
	  mbkfree(tab);
	}
    }
  else
    {
      for (cell = cur->CELL; cell; cell = cell->NEXT) 
	{
	  
	  if (cell->TYPE == CGV_CEL_BOX) 
	    {	      
	      // les boites normales
	      box = (cgvbox_list *) cell->ROOT;
	      tnb=0; tmoyenne=0;
//	      if (strcmp(box->NAME,"tr_00589")==0) DEBUG=1;
	      for (con = box->CON_OUT; con!=NULL; con=con->NEXT)
		{
		  cmoyenne=comp_moyenne(con, pred, POUR_ENTREES);
		  if (cmoyenne!=FAILED_MOYENNE)
		    {
		      tmoyenne+=cmoyenne-con->Y_REL; tnb++;
		    }
		}
	      for (con = box->CON_IN, cnb=0, tmoyenne0=0; con!=NULL; con=con->NEXT)
		{
		  cmoyenne=comp_moyenne(con, cur->NEXT, POUR_SORTIES);
		  if (cmoyenne!=FAILED_MOYENNE)
		    {
		      tmoyenne0+=cmoyenne-con->Y_REL; cnb++;
		    }
		}
//	      if (strcmp(box->NAME,"transparence_49")==0) printf("(0)%s: %d %d\n",box->NAME,tnb, cnb);
	      if (tnb==0 || cnb==0) {SetCgvBoxUnTaged(box);/*printf("2.untag %d %d %s\n",tnb, cnb,box->NAME);*/}
	      if (tnb==0) { tmoyenne=tmoyenne0; tnb=cnb; }
	      if (tnb!=0) box->Y=(tmoyenne/tnb);
//	      DEBUG=0;
//	      fprintf(f,"/ %s: tmoy=%d (%d) %s\n",box->NAME,tmoyenne, tnb,IsCgvBoxTaged(box)?"tagged":"x");
	    }
	  else
	    {
	      con = (cgvcon_list *) cell->ROOT;
	      cmoyenne=comp_moyenne(con, pred, POUR_ENTREES);
//	      if (cmoyenne==FAILED_MOYENNE) {/*fprintf(f,"on %s.%s\n",box->NAME,con->NAME);*/EXIT(5);}
	      if (cmoyenne!=FAILED_MOYENNE) con->Y_REL=cmoyenne;
//	      fprintf(f,">%s> %s: cmoy=%d (%d)\n",con->NAME,box->NAME,cnb, cmoyenne);
	    }
	}
    }
}

void aligner_par_rapport_aux_sorties(cgvcol_list *cur, cgvcol_list *pred, int avoidinterface)
{
  cgvcon_list *con;
  cgvcel_list *cell;
  int cmoyenne, cnb, tmoyenne, tnb, tmoyenne0;
  cgvbox_list *box;

  if (pred==NULL)
    {
      int y=0;
      if (!avoidinterface)
	{
	  for (cell = cur->CELL; cell; cell = cell->NEXT) 
	    {
	      con = (cgvcon_list *) cell->ROOT;
	      con->Y_REL = y;
//	      fprintf(f,"con %s: y=%d\n",con->NAME, y);    
	      y+=100*CGV_SCALE;
	    }
	}
    }
  else
    {
      for (cell = cur->CELL; cell; cell = cell->NEXT) 
	{

	  if (cell->TYPE == CGV_CEL_BOX) 
	    {	      
	      // les boites normales
	      box = (cgvbox_list *) cell->ROOT;
	      tnb=0; tmoyenne=0;
	      for (con = box->CON_IN; con!=NULL; con=con->NEXT)
		{
		  cmoyenne=comp_moyenne(con, pred, POUR_SORTIES);
		  if (cmoyenne!=FAILED_MOYENNE)
		    {
		      tmoyenne+=cmoyenne-con->Y_REL; tnb++;
		    }
		}
	      for (con = box->CON_OUT, cnb=0, tmoyenne0=0; con!=NULL; con=con->NEXT)
		{
		  cmoyenne=comp_moyenne(con, cur->NEXT, POUR_ENTREES);
		  if (cmoyenne!=FAILED_MOYENNE)
		    {
		      tmoyenne0+=cmoyenne-con->Y_REL; cnb++;
		    }
		}
//	      if (strcmp(box->NAME,"transparence_49")==0) printf("(1)%s: %d %d\n",box->NAME,tnb, cnb);
	      if (tnb==0 || cnb==0) {SetCgvBoxUnTaged(box);/*printf("untag %d %d %s\n",tnb, cnb,box->NAME);*/}
	      if (tnb==0) { tmoyenne=tmoyenne0; tnb=cnb; }

	      if (tnb!=0) box->Y=(tmoyenne/tnb);
//	      fprintf(f,"> %s: tmoy=%d (%d) %s\n",box->NAME,tmoyenne, tnb,IsCgvBoxTaged(box)?"tagged":"x");
	    }
	  else
	    {
	      con = (cgvcon_list *) cell->ROOT;

	      cmoyenne=comp_moyenne(con, pred, POUR_SORTIES);
//	      if (cmoyenne==FAILED_MOYENNE) {fprintf(f,"on %s.%s\n",box->NAME,con->NAME);EXIT(8);}
	      if (cmoyenne!=FAILED_MOYENNE) con->Y_REL=cmoyenne;

//	      fprintf(f,">%s> %s: cmoy=%d (%d)\n",con->NAME,box->NAME, cmoyenne, cnb);
	    }
	}
    }
}
void justuntagiffree(cgvcol_list *cur, cgvcol_list *pred)
{
  cgvcon_list *con;
  cgvcel_list *cell;
  int cmoyenne, cnb, tmoyenne, tnb, tmoyenne0;
  cgvbox_list *box;

  if (pred==NULL) return;

  for (cell = cur->CELL; cell; cell = cell->NEXT) 
    {
      
      if (cell->TYPE == CGV_CEL_BOX) 
	{	      
	  // les boites normales
	  box = (cgvbox_list *) cell->ROOT;
	  tnb=0; tmoyenne=0;
	  for (con = box->CON_IN; con!=NULL; con=con->NEXT)
	    {
	      cmoyenne=comp_moyenne(con, pred, POUR_SORTIES);
	      if (cmoyenne!=FAILED_MOYENNE)
		{
		  tmoyenne+=cmoyenne-con->Y_REL; tnb++;
		}
	    }
	  for (con = box->CON_OUT, cnb=0, tmoyenne0=0; con!=NULL; con=con->NEXT)
	    {
	      cmoyenne=comp_moyenne(con, cur->NEXT, POUR_ENTREES);
	      if (cmoyenne!=FAILED_MOYENNE)
		{
		  tmoyenne0+=cmoyenne-con->Y_REL; cnb++;
		}
	    }
	  if (tnb==0 || cnb==0) {SetCgvBoxUnTaged(box);/*printf("3. untag %d %d %s\n",tnb, cnb,box->NAME);*/}
	  
	}
    }
}

void aligner_par_rapport_aux_sorties_et_aux_entrees(cgvcol_list *cur, cgvcol_list *pred)
{
  cgvcon_list *con;
  cgvcel_list *cell;
  int cmoyenne, tmoyenne, tnb;
  cgvbox_list *box;

#ifndef __ALL__WARNING__
  pred = NULL;  
#endif
 
  for (cell = cur->CELL; cell; cell = cell->NEXT) 
    {
      if (cell->TYPE == CGV_CEL_BOX) 
	{	      
	  // les boites normales
	  box = (cgvbox_list *) cell->ROOT;
	  if (!IsCgvBoxTaged(box))
	    {
	      tnb=0; tmoyenne=0;
	      for (con = box->CON_IN; con!=NULL; con=con->NEXT)
		{
//		  cmoyenne=comp_moyenne(con, pred, POUR_SORTIES);
		  cmoyenne=comp_moyenne_sans_transparence(con);
		  if (cmoyenne!=FAILED_MOYENNE)
		    {
		      tmoyenne+=cmoyenne-con->Y_REL; tnb++;
		    }
		}
	      for (con = box->CON_OUT; con!=NULL; con=con->NEXT)
		{
//		  cmoyenne=comp_moyenne(con, cur->NEXT, POUR_ENTREES);
		  cmoyenne=comp_moyenne_sans_transparence(con);
		  if (cmoyenne!=FAILED_MOYENNE)
		    {
		      tmoyenne+=cmoyenne-con->Y_REL; tnb++;
		    }
		}
	      
	      if (tnb!=0) 
		{
		  box->Y=(tmoyenne/tnb);
//		  fprintf(f,"MOVED %s: tmoy=%d (%d)\n",box->NAME,tmoyenne, tnb);
		}
	    }
	}
    }
}

int find_space_for(cgvcol_list *c, int y, int size)
{
  cgvcel_list *cell, **tab;
  int cnt=0, i;
  cgvbox_list *box, *box1;

  for (cell = c->CELL, cnt=0; cell; cell = cell->NEXT) 
    {
//      box=(cgvbox_list *)cell->ROOT;
   /*   if (IsCgvBoxTaged(box))*/ cnt++;
    }

  tab=(cgvcel_list **)mbkalloc(sizeof(cgvcel_list *)*cnt);
  for (cell = c->CELL, cnt=0; cell; cell = cell->NEXT)
    {
/*      box=(cgvbox_list *)cell->ROOT;
      if (IsCgvBoxTaged(box))*/ { tab[cnt++]=cell; }
    }

  qsort(tab, cnt, sizeof(cgvcel_list *), comp_y);

  for (i=cnt-1;i>=0;i--)
    {
      box=(cgvbox_list *)tab[i]->ROOT;
      if (box->Y<y) break;
    }

  if (i<0)
    {
      if (y-size<=box->Y) {mbkfree(tab);return y;}
    }
  else
  if (i==cnt-1)
    {
      if (y>=box->Y+box->DY) {mbkfree(tab);return y;}
    }
  else
    {
      box1=(cgvbox_list *)tab[i+1]->ROOT;
      if (box1->Y-(box->Y+box->DY+addmarge(box))>=size && box1->Y-size<y) {mbkfree(tab); return box1->Y-size;}
      if (y>=box->Y+box->DY && y+size<=box1->Y){mbkfree(tab);return y;}
    }

  i--;
//  if (i>=0) fprintf(f,"h(%s),",box->NAME);
  while (i>=0)
    {
      box=(cgvbox_list *)tab[i]->ROOT;
      box1=(cgvbox_list *)tab[i+1]->ROOT;
//      fprintf(f,"%s-%s=%d,",box1->NAME,box->NAME,box1->Y-(box->Y+box->DY));
      if (box1->Y-(box->Y+box->DY+addmarge(box))>=size) {mbkfree(tab);return box1->Y-size;}
      i--;
    }
  box=(cgvbox_list *)tab[0]->ROOT;
  mbkfree(tab);
//  fprintf(f,"B");
  return box->Y-size;
}

int find_space_for2(cgvcol_list *c, int y, int size)
{
  cgvcel_list *cell, **tab;
  int cnt=0, i;
  cgvbox_list *box, *box1;

  for (cell = c->CELL, cnt=0; cell; cell = cell->NEXT) 
    {
//      box=(cgvbox_list *)cell->ROOT;
   /*   if (IsCgvBoxTaged(box))*/ cnt++;
    }

  tab=(cgvcel_list **)mbkalloc(sizeof(cgvcel_list *)*cnt);
  for (cell = c->CELL, cnt=0; cell; cell = cell->NEXT)
    {
      tab[cnt++]=cell;
    }

  qsort(tab, cnt, sizeof(cgvcel_list *), comp_y);

  for (i=cnt-1;i>=0;i--)
    {
      box=(cgvbox_list *)tab[i]->ROOT;
      if (box->Y<y) break;
    }

  if (i<0)
    {
      if (y-size<=box->Y) {mbkfree(tab);return y;}
    }
  else
  if (i==cnt-1)
    {
      if (y>=box->Y+box->DY) {mbkfree(tab);return y;}
    }
  else
    {
      box1=(cgvbox_list *)tab[i+1]->ROOT;
      if (box1->Y-(box->Y+box->DY+addmarge(box))>=size && box1->Y-size<y) {mbkfree(tab); return box1->Y-size;}
      if (y>=box->Y+box->DY && y+size<=box1->Y){mbkfree(tab);return y;}
    }

  i++;
//  if (i>=0) fprintf(f,"h(%s),",box->NAME);
  while (i<cnt-1)
    {
      box=(cgvbox_list *)tab[i]->ROOT;
      box1=(cgvbox_list *)tab[i+1]->ROOT;
//      fprintf(f,"%s-%s=%d,",box1->NAME,box->NAME,box1->Y-(box->Y+box->DY));
      if (box1->Y-(box->Y+box->DY+addmarge(box))>=size) {mbkfree(tab);return box1->Y-size;}
      i++;
    }
  box=(cgvbox_list *)tab[cnt-1]->ROOT;
  mbkfree(tab);
//  fprintf(f,"B");
  return box->Y+box->DY+addmarge(box);
}

int tasser_les_non_places(cgvcol_list *c, int moy)
{
  cgvcel_list *cell, **tab;
  int cnt=0, i, cury=FAILED_MOYENNE, cury1=FAILED_MOYENNE, cc=0, concc=0;
  int bcury, bcury1;
  cgvbox_list *box;
  cgvcon_list *con;

  for (cell = c->CELL, cnt=0; cell; cell = cell->NEXT) cnt++;

  if (cnt==0) return 0;

  tab=(cgvcel_list **)mbkalloc(sizeof(cgvcel_list *)*cnt);
  for (cell = c->CELL, cnt=0; cell; cell = cell->NEXT)
    {
      tab[cnt++]=cell;
    }

  qsort(tab, cnt, sizeof(cgvcel_list *), comp_y);

  cury=FAILED_MOYENNE; //((cgvbox_list *)(tab[cnt-1]->ROOT))->Y;
  for (i=cnt-1;i>=0;i--)
    {
      if (tab[i]->TYPE == CGV_CEL_BOX)
	{
	  box=(cgvbox_list *)tab[i]->ROOT;
//	  printf("TG: %s\n",box->NAME);
	  if (IsCgvBoxTaged(box))
	    {
	      if (cury==FAILED_MOYENNE) { cury=box->Y+box->DY+addmarge(box); bcury=cury;/* printf("-- top=%s\n",box->NAME);*/ }
	      cury1=box->Y;
	    }
	  if (!IsCgvBoxTaged(box)) 
	    {
	      cc++;
	      //	  fprintf(f,"isuntag %s\n",box->NAME);
	    }
	}
      else
	{
	  con = (cgvcon_list *) tab[i]->ROOT;
//	  printf("%s: %d\n",con->NAME, con->Y_REL);
	  if (IsCgvConTaged(con))
	    {
	      if (cury==FAILED_MOYENNE) { cury=con->Y_REL+MARGE; bcury=cury; }
	      cury1=con->Y_REL;
	    }

	  if (!IsCgvConTaged(con))
	    {
	      /*printf("%s\n",con->NAME);*/
	      cc++;
	      concc++;
	    }
	}
    }
//  printf("cc=%d, cury=%d, moy=%d\n",cc,cury,moy);
  if (cc==0 || (concc==0 && cury==FAILED_MOYENNE)) { mbkfree(tab); return 0; }
  if (cury==FAILED_MOYENNE) { cury1=cury=moy; bcury=cury; }

  bcury1=cury1;

  for (i=0;i<cnt;i++)
    {
      if (tab[i]->TYPE == CGV_CEL_BOX)
	{   
	  box=(cgvbox_list *)tab[i]->ROOT;
	  //      if (!IsCgvBoxTaged(box)) printf("-1> %s %d\n",box->NAME,box->Y);
	  if (!IsCgvBoxTaged(box) && box->Y>=bcury) 
	    {
	      box->Y=cury;
	      cury=box->Y+box->DY+addmarge(box);
//	      printf("2. %s\n",box->NAME);
	    }
	}
      else
	{
	  con = (cgvcon_list *) tab[i]->ROOT;
//	  printf("%s: [%d]\n",con->NAME, bcury);
	  if (!IsCgvConTaged(con) && con->Y_REL>=bcury)
	    {
//	      printf("2. %s\n",con->NAME);
	      con->Y_REL=cury+ MARGE;
	      cury=con->Y_REL;
//	      SetCgvConTaged(con);
	    }
	}
    }

  for (i=cnt-1;i>=0;i--)
    {
       if (tab[i]->TYPE == CGV_CEL_BOX)
	{   
	  box=(cgvbox_list *)tab[i]->ROOT;
	  //      if (!IsCgvBoxTaged(box)) printf("-2> %s %d\n",box->NAME,box->Y);
	  if (!IsCgvBoxTaged(box) && box->Y<=bcury1) 
	    {
	      box->Y=cury1-(box->DY+addmarge(box));
	      cury1=box->Y;
//	      printf("1. %s\n",box->NAME);
	    }
	}
       else
	 {
	   con = (cgvcon_list *) tab[i]->ROOT;
//	   printf("%s: [%d]\n",con->NAME, bcury1);
	   if (!IsCgvConTaged(con) && con->Y_REL<=bcury1)
	    {
//	      printf("1. %s\n",con->NAME);
	      con->Y_REL=cury1-MARGE;
	      cury1=con->Y_REL;
//	      SetCgvConTaged(con);
	    }
	 }
    }
  mbkfree(tab);
  return cc;
}

void essayer_d_aligner_par_rapport_aux_entrees(cgvcol_list *cur, cgvcol_list *pred)
{
  cgvcon_list *con;
  cgvcel_list *cell;
  long cmoyenne, y, tmoyenne, tnb, besty;
  cgvbox_list *box;

  for (cell = cur->CELL; cell; cell = cell->NEXT) 
    {
	  
      if (cell->TYPE == CGV_CEL_BOX) 
	{	      
	  // les boites normales
	  box = (cgvbox_list *) cell->ROOT;
	  tnb=0; tmoyenne=0;
	  besty=FAILED_MOYENNE;
	  for (con = box->CON_OUT; con!=NULL; con=con->NEXT)
	    {
	      cmoyenne=comp_moyenne(con, pred, POUR_ENTREES);
	      if (cmoyenne!=FAILED_MOYENNE)
		{
		  tmoyenne+=cmoyenne-con->Y_REL; tnb++;
		}
	    }
	  if (tnb!=0) {besty=tmoyenne/tnb;}
	  else
	    {
	      for (con = box->CON_OUT; con!=NULL; con=con->NEXT)
		{
		  cmoyenne=comp_moyenne(con, pred, POUR_SORTIES);
		  if (cmoyenne!=FAILED_MOYENNE)
		    {
		      tmoyenne+=cmoyenne-con->Y_REL; tnb++;
		    }
		}
	      if (tnb!=0) {besty=tmoyenne/tnb;}
	      else
		{
		  for (con = box->CON_OUT; con!=NULL; con=con->NEXT)
		    {
		      cmoyenne=comp_moyenne(con, cur, POUR_SORTIES);
		      if (cmoyenne!=FAILED_MOYENNE)
			{
			  tmoyenne+=cmoyenne-con->Y_REL; tnb++;
			}
		    }
		  if (tnb!=0) {besty=tmoyenne/tnb;}
		  else
		    {
		      for (con = box->CON_OUT; con!=NULL; con=con->NEXT)
			{
			  cmoyenne=comp_moyenne(con, cur, POUR_ENTREES);
			  if (cmoyenne!=FAILED_MOYENNE)
			    {
			      tmoyenne+=cmoyenne-con->Y_REL; tnb++;
			    }
			}
		      if (tnb!=0) {besty=tmoyenne/tnb;}
		    }
		}
	    }

	  //	      fprintf(f,"try %s: tmoy=%d \n",box->NAME, tmoyenne/tnb);
	  
	  if (besty!=FAILED_MOYENNE)
	    {	      
	      int y0;
	      y=find_space_for(cur, besty, box->DY+addmarge(box));
	      y0=find_space_for2(cur, besty, box->DY+addmarge(box));
/*	      if (strcmp(box->NAME,"transparence_36")==0
		  || strcmp(box->NAME,"nand3_d")==0
		  )
		printf("%s: y=%d y0=%d, besty=%d  %d %d\n",box->NAME,y,y0,besty,tmoyenne,tnb);*/
	      if (labs(box->Y-besty)>labs(besty-y))
		box->Y=y;
	      if (labs(box->Y-besty)>labs(besty-y0))
		box->Y=y0;
	    }	  
	}
      else
	{
	  con = (cgvcon_list *) cell->ROOT;
	  cmoyenne=comp_moyenne(con, pred, POUR_ENTREES);
	  con->Y_REL=cmoyenne;	  
	}
    }
}

chain_list *aligner_les_transparences_de_boucle(cgvcol_list *cur)
{
  cgvcel_list *cell;
  int y;
  cgvbox_list *box;

  for (cell = cur->CELL; cell; cell = cell->NEXT) 
    {
      if (cell->TYPE == CGV_CEL_BOX) 
	{	      
	  // les boites normales
	  box = (cgvbox_list *) cell->ROOT;
	  if (!IsCgvBoxTaged(box)/* && IsCgvBoxTransparence(box)*/)
	    {
	      y=comp_moyenne_sans_transparence(box->CON_IN);
	      return move_all_transparence_to(box->CON_IN, y);
	    }
	}
    }
  return NULL;
}

static cgvfig_list *loc_laisser_pendouiller_out_vers_in(cgvfig_list *Figure, int avoidinterface)
{
    cgvcol_list    *Column_from=NULL;
    cgvcol_list    *Column_to;

    HEAD_CGVCOL = (cgvcol_list *) reverse((chain_list *) HEAD_CGVCOL);
#ifdef DEBUG
    fprintf(stdout,"pendouillage:");
#endif
    for (Column_to = HEAD_CGVCOL;
         Column_to != NULL;
         Column_to = Column_to->NEXT) {
#ifdef DEBUG
      fprintf(stdout,"."); fflush(stdout);
#endif
      aligner_par_rapport_aux_entrees(Column_to, Column_from, avoidinterface);
      Column_from = Column_to;

    }
#ifdef DEBUG
    fprintf(stdout,"\n");
#endif
    HEAD_CGVCOL = (cgvcol_list *) reverse((chain_list *) HEAD_CGVCOL);
//    fclose(f);
    return (Figure);
}
/*
static cgvfig_list *loc_laisser_pendouiller_in_vers_out(cgvfig_list *Figure, int avoidinterface)
{
    cgvcol_list    *Column_from=NULL;
    cgvcol_list    *Column_to;

    for (Column_to = HEAD_CGVCOL;
         Column_to != NULL;
         Column_to = Column_to->NEXT) {
       
      aligner_par_rapport_aux_sorties(Column_to, Column_from, avoidinterface);
      Column_from = Column_to;

    }

    return (Figure);
}
*/

int colsize(cgvcol_list *cur)
{
  cgvcel_list *cell;
  int size=0;
  cgvbox_list *box;

  for (cell = cur->CELL; cell; cell = cell->NEXT) 
    {
      if (cell->TYPE == CGV_CEL_BOX) 
	{	      
	  // les boites normales
	  box = (cgvbox_list *) cell->ROOT;
//	  fprintf(f,"%s ",box->NAME);
	  size+=box->DY;
	}
    }

//  fprintf(f,"= %d\n",size);
  return size;
}

void displaycol(cgvcol_list *c)
{
  cgvcel_list *cell;
  cgvbox_list *box;
 
  for (cell = c->CELL; cell; cell = cell->NEXT) 
    {
      if (cell->TYPE == CGV_CEL_BOX) 
	{	      
	  box = (cgvbox_list *) cell->ROOT;
//	  printf("->%s\n",box->NAME);
/*	  if (strcmp(box->NAME,"tr_00589")==0)
	    printf("%s :HERE\n",box->NAME);*/
	}
    }
}

static cgvcol_list *highestcolumn()
{
    cgvcol_list    *c, *best;
    int high=0, a;

    for (c = HEAD_CGVCOL; c != NULL; c = c->NEXT) 
      {
	if ((a=colsize(c))>=high)
	  {
	    high=a; best=c;
	  }
      }

//    fprintf(f,"best col size=%d\n",high);
    displaycol(best);
    return best;
}
static void cleantags(cgvfig_list *cgvf)
{
    cgvcol_list    *c;
    cgvbox_list *box;

#ifndef __ALL__WARNING__
  	cgvf = NULL;  
#endif
    for (c = HEAD_CGVCOL; c != NULL; c = c->NEXT) 
      {
	cgvcel_list *cell;
	for (cell = c->CELL; cell; cell = cell->NEXT) 
	  {	
	    if (cell->TYPE == CGV_CEL_BOX)
	      {
		box=(cgvbox_list *) cell->ROOT;
		SetCgvBoxTaged(box);
	      }
	    else
	      {
//		SetCgvConTaged((cgvcon_list *) cell->ROOT);
	      }
//	    fprintf(f,"tag %s\n",box->NAME);
	  }
      }

}

static void untagfloatingcon(cgvcol_list *c)
{
  cgvcel_list *cell;
  cgvcon_list *con;
  for (cell = c->CELL; cell; cell = cell->NEXT)
    {
      con=(cgvcon_list *)cell->ROOT;
      if (con->NET->CON_NET==NULL || con->NET->CON_NET->NEXT==NULL)
        SetCgvConUnTaged(con);
    }
}

void tasser_cellules(cgvcol_list *c, int *centre)
{
  cgvcel_list *cell, **tab;
  int cnt=0, i, base=0;
  cgvbox_list *box;

  for (cell = c->CELL, cnt=0; cell; cell = cell->NEXT, cnt++) 
    {
       if (cell->TYPE == CGV_CEL_BOX) 
	 {
	   box=(cgvbox_list *)cell->ROOT;
	   if (box->misc.UNDER!=NULL) box->Y = box->misc.UNDER->Y - (box->misc.UNDER->DY/2);
	 }
    }

  tab=(cgvcel_list **)mbkalloc(sizeof(cgvcel_list *)*cnt);
  for (cell = c->CELL, cnt=0; cell; cell = cell->NEXT, cnt++) tab[cnt]=cell;

  qsort(tab, cnt, sizeof(cgvcel_list *), comp_y);

  for (i=0;i<cnt;i++)
    {
      if (tab[i]->TYPE == CGV_CEL_BOX) 
	{
	  box=(cgvbox_list *)tab[i]->ROOT;
	  box->Y=base;
	  base=base+box->DY+addmarge(box);
	}
      else
	{
	  ((cgvcon_list *)tab[i]->ROOT)->Y_REL=base;
	  base=base+MARGE;
	}
    }
  *centre=base/2;
  mbkfree(tab);
}

void getcellinfo(cgvcel_list *c, int *y, int *dy, int *marge)
{
 if (c->TYPE == CGV_CEL_BOX) 
   {
     *y=((cgvbox_list *)c->ROOT)->Y;
     *dy=((cgvbox_list *)c->ROOT)->DY;
     if (!IsCgvBoxTransparence((cgvbox_list *)c->ROOT)) *marge=MARGE;
     else *marge=0;
   }
 else
   {
     *y=((cgvcon_list *)c->ROOT)->Y_REL;
     *dy=MARGE;
     *marge=0;
   }
}
void setcelly(cgvcel_list *c, int y)
{
 if (c->TYPE == CGV_CEL_BOX) ((cgvbox_list *)c->ROOT)->Y=y;
 else ((cgvcon_list *)c->ROOT)->Y_REL=y;
}

int getcelly(cgvcel_list *c)
{
 if (c->TYPE == CGV_CEL_BOX) return ((cgvbox_list *)c->ROOT)->Y;
 return ((cgvcon_list *)c->ROOT)->Y_REL;
}

void organiser_cellules(cgvcol_list *c, int centre)
{
  cgvcel_list *cell, **tab;
  int cnt=0, i, spaceunder, spaceover, j;
  int y0, y1, dy0, dy1, m0, m1;

  for (cell = c->CELL, cnt=0; cell; cell = cell->NEXT, cnt++) ;

  tab=(cgvcel_list **)mbkalloc(sizeof(cgvcel_list *)*cnt);
  for (cell = c->CELL, cnt=0; cell; cell = cell->NEXT, cnt++) tab[cnt]=cell;

  qsort(tab, cnt, sizeof(cgvcel_list *), comp_y);

//  printf("new\n");
  for (i=0;i<cnt;i++)
    {
/*      if (tab[i]->TYPE != CGV_CEL_BOX) 
	printf("%s\n",((cgvcon_list *)tab[i]->ROOT)->NAME);
*/
//      fprintf(f,"->\n");
//      box=(cgvbox_list *)tab[i]->ROOT;
      getcellinfo(tab[i], &y0, &dy0, &m0);
      if (i==0) spaceunder=100000;
      else 
	{
	  getcellinfo(tab[i-1], &y1, &dy1, &m1);
	  /*box1=(cgvbox_list *)tab[i-1]->ROOT;*/
	  spaceunder=y0 - ( y1 + dy1 + m1 ); 
//      fprintf(f, "cunder: y=%d dy=%d y1=%d\n",box->Y, box->DY, box1->Y);
      }

      if (i==cnt-1) spaceover=100000;
      else 
	{
	  getcellinfo(tab[i+1], &y1, &dy1, &m1);
	  //box1=(cgvbox_list *)tab[i+1]->ROOT;
	  spaceover=y1 - ( y0 + dy0 + m0);
//        fprintf(f, "cover: y=%d dy=%d y1=%d\n",box1->Y, box1->DY, box->Y);
      }
	
//      fprintf(f, "mv: %s u=%d o=%d\n",box->NAME, spaceunder, spaceover);
      if (spaceunder<0 && spaceover>0 && spaceover>=-spaceunder)
	{ 
	  setcelly(tab[i], getcelly(tab[i]) + -spaceunder);
	  /* fprintf(f,"(y+%d)",-spaceunder); */}
      else
	if (spaceover<0 && spaceunder>0 && spaceunder>=-spaceover)
	{
	  setcelly(tab[i], getcelly(tab[i]) - -spaceover);
//	  box->Y -= -spaceover;
	  /*fprintf(f,"(y-%d)",-spaceover); */
	}
      else if (spaceover<0 || spaceunder<0)
	{
	  int movedir;
	  if (spaceover<0 && spaceunder<0)
	    {
	      if (getcelly(tab[i])<centre) movedir=1; else movedir=0;
	    }
	  else
	    {
	      if (spaceover<0) movedir=1; else movedir=0;
	    }
	  if (movedir==1)
	    {
	      setcelly(tab[i], getcelly(tab[i]) - -spaceover);
//	      box->Y -= -spaceover;
//	      fprintf(f,"(2)y-%d=%d)",-spaceover,box->Y);
//	      lastbox=box;
	      y0=getcelly(tab[i]);
	      for (j=i-1; j>=0; j--) 
		{
//		  box1=(cgvbox_list *)tab[j]->ROOT;
		  getcellinfo(tab[j], &y1, &dy1, &m1);
		  if (y0>= y1 + dy1 + m1) break;
		  setcelly(tab[j], y0 - (dy1 + m1));
//		  box1->Y=lastbox->Y-(box1->DY+addmarge(box1));
//		  fprintf(f,"(%s moveddw %d)",box1->NAME,box1->Y);
		  getcellinfo(tab[j], &y0, &dy0, &m0);
//		  y0=y1;
//		  lastbox=box1;
		}
	    }
	  else
	    {
	      setcelly(tab[i], getcelly(tab[i]) + -spaceunder);
//	      box->Y += -spaceunder;
//	      fprintf(f,"(2)y+%d)",-spaceunder);
//	      lastbox=box;
	      getcellinfo(tab[i], &y0, &dy0, &m0);
	      for (j=i+1; j<cnt; j++) 
		{
//		  box1=(cgvbox_list *)tab[j]->ROOT;
		  getcellinfo(tab[j], &y1, &dy1, &m1);
		  if (y1>= y0 + dy0 + m0) break;
		  setcelly(tab[j], y0 + dy0 + m0);
//		  box1->Y=lastbox->Y+lastbox->DY+addmarge(lastbox);
//		  fprintf(f,"(%s movedup %d)",box1->NAME,box1->Y);
//		  lastbox=box1;
		  getcellinfo(tab[j], &y0, &dy0, &m0);
		}
	    }
	}
/*      else
	fprintf(f,"GOOD");

      fprintf(f,"<-\n");
	*/
    }
  mbkfree(tab);
}

void addfakecon(cgvfig_list *cgvf)
{
  cgvbox_list *box=cgvf->BOX;
  cgvnet_list *n;
  cgvcon_list *c;
  while (box!=NULL)
    {
      if (box->NUMBER_OUT==0)
	{
	  n=addcgvnet(cgvf);
	  c=addcgvboxcon(cgvf, box, "**$$**", CGV_CON_OUT);
	  addcgvnetcon(n, c);
	  c=addcgvfigcon(cgvf, "$$**$$", CGV_CON_OUT);
	}
      box=box->NEXT;
    }
}


void updatecolumninfos(cgvcol_list *Column)
{
  cgvcel_list *Cell;
  cgvbox_list *Box;
  cgvcon_list *Con;

  while (Column!=NULL)
    {
      for (Cell = Column->CELL; Cell != NULL; Cell = Cell->NEXT) 
	{
	  if (Cell->TYPE == CGV_CEL_BOX) 
	    {
	      Box = (cgvbox_list *) Cell->ROOT;
	      if (Box->DX > Column->MAX_DX_CELL) 
		{
		  Column->MAX_DX_CELL = Box->DX;
		}
	    }
	  else 
	    if (Cell->TYPE == CGV_CEL_CON) 
	      {
		Con = (cgvcon_list *) Cell->ROOT;
		if (2*CGV_SCALE > Column->MAX_DX_CELL) 
		  {
		    Column->MAX_DX_CELL = 2*CGV_SCALE;
		  }
	      }
	}
      Column=Column->NEXT;
    }
}

void aligner_en_avant_best(cgvcol_list *best, int centre, cgvfig_list *Figure)
{
  cgvcol_list *c, *pred, *pred0;
  int y;

  pred0=best->NEXT;
  HEAD_CGVCOL = (cgvcol_list *) reverse((chain_list *) HEAD_CGVCOL);


#ifdef DEBUG
  fprintf(stdout,"left:");
#endif

  for (c = best->NEXT, pred=best; c != NULL; pred=c, c = c->NEXT)
    {
#ifdef DEBUG
      fprintf(stdout,".");fflush(stdout);
#endif
      aligner_par_rapport_aux_entrees(c, pred, 1);
      organiser_cellules(c, centre);
      /*if (c->NEXT!=NULL)*/ tasser_les_non_places(c, centre);
    }
#ifdef DEBUG
  fprintf(stdout,"\n");
#endif

  pred0=best->NEXT;
  HEAD_CGVCOL = (cgvcol_list *) reverse((chain_list *) HEAD_CGVCOL);  

#ifdef DEBUG
  fprintf(stdout,"right:");
#endif
  for (c = best->NEXT, pred=best; c != NULL; pred=c, c = c->NEXT)
    {
#ifdef DEBUG
      fprintf(stdout,".");fflush(stdout);
#endif
      aligner_par_rapport_aux_sorties(c, pred, 1);
      organiser_cellules(c, centre);
     /* if (c->NEXT!=NULL)*/ tasser_les_non_places(c, centre);
    }
#ifdef DEBUG
  fprintf(stdout,"\n");
#endif


  HEAD_CGVCOL = (cgvcol_list *) reverse((chain_list *) HEAD_CGVCOL);  

#ifdef DEBUG
  fprintf(stdout,"whole:");
#endif
  
  for (c = HEAD_CGVCOL->NEXT, pred=HEAD_CGVCOL; c != NULL ; pred=c, c = c->NEXT)
    {
      if (c->NUMBER_CELL<800)
        {
#ifdef DEBUG
          fprintf(stdout,".");fflush(stdout);
#endif
        }
      else
        {
#ifdef DEBUG
          fprintf(stdout,"_");fflush(stdout);
#endif
          if (c==best) tasser_les_non_places(best, centre);
          continue;
        }
      /*	  cgvcel_list *cel;
		  for (cel=c->CELL; cel!=NULL; cel=cel->NEXT)
		  if (cel->TYPE==CGV_CEL_BOX && strcmp(((cgvbox_list *)cel->ROOT)->NAME,"transparence_188")==0) break;
		  if (cel!=NULL) break;
      */
      for (y=0;y<5;y++)
	{
	  essayer_d_aligner_par_rapport_aux_entrees(c, pred);
	  organiser_cellules(c, centre);
	}
      
      if (c==best) tasser_les_non_places(best, centre);
      /*
	if (tasser_les_non_places(c))
	{
	for (y=0;y<2;y++)
	{
	essayer_d_aligner_par_rapport_aux_entrees(c, pred);
	}
	}
      */
      //organiser_cellules(c, centre);
    }
#ifdef DEBUG
  fprintf(stdout,"\n");
#endif
  /*
    tasser_les_non_places(best);
  */
  tasser_les_non_places(pred, centre);
  untagfloatingcon(HEAD_CGVCOL);
  tasser_les_non_places(HEAD_CGVCOL, centre);

  removefake(Figure, HEAD_CGVCOL);
  HEAD_CGVCOL = (cgvcol_list *) reverse((chain_list *) HEAD_CGVCOL);  
}


void 
buildcgvfig(Figure)
    cgvfig_list    *Figure;
{
  cgvcol_list *best;
  int centre;
#ifdef DEBUG
    fprintf(stdout, "--- Detection des clusters ---\n");
    fflush(stdout);
#endif
/*    loc_buildcluster(Figure);*/
#ifdef DEBUG
    fprintf(stdout, "----- Placement  initial -----\n");
    fflush(stdout);
#endif
//    addfakecon(Figure);
    loc_placement_initial(Figure);
#ifdef DEBUG
    fprintf(stdout, "--- Fabrication des boites ---\n");
    fflush(stdout);
#endif
    loc_buildbox(Figure);
#ifdef DEBUG
    fprintf(stdout, "---------  Placement ---------\n");
    fflush(stdout);
#endif
//    loc_buildplace(Figure);
    updatecolumninfos(HEAD_CGVCOL);

/* zinaps was here */
//    f=fopen("r","wt");
//    if (strchr(Figure->NAME,'#')==NULL)
      {
	cleantags(Figure);
	loc_laisser_pendouiller_out_vers_in(Figure, 0);
	best=highestcolumn();
	tasser_cellules(best, &centre);
	//    cleantags();
	aligner_en_avant_best(best, centre, Figure);
      }

//    fclose(f);

/* --------------- */
#ifdef DEBUG
    fprintf(stdout, "----------  Routage ----------\n");
    fflush(stdout);
#endif
    loc_route(Figure);
#ifdef DEBUG
    fprintf(stdout, "---------- Nettoyage  --------\n");
    fflush(stdout);
#endif
    loc_clean_all(Figure);
}

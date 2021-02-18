/*------------------------------------------------------------\
|                                                             |
| Tool    :                     CGV                           |
|                                                             |
| File    :                 cgv_dealloc.c                     |
|                                                             |
| Date    :                   04.03.98                        |
|                                                             |
| Authors :               Picault  Stephane                   |
|                         Miramond   Benoit                   |
|                                                             |
\------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

#include MUT_H
#include SLB_H
#include CGV_H

/*------------------------------------------------------------\
|                      Cgv Del Functions                      |
\------------------------------------------------------------*/

/*------------------------------------------------------------\
|                        Cgv Del Connector List               |
\------------------------------------------------------------*/

static void 
loc_delcgvcon(HeadCon)
    cgvcon_list    *HeadCon;
{
    cgvcon_list    *ScanCon;
    cgvcon_list    *DelCon;

    ScanCon = HeadCon;

    while (ScanCon != (cgvcon_list *) 0) {
        DelCon = ScanCon;
        ScanCon = ScanCon->NEXT;

        freecgvcon(DelCon);
    }
}

/*------------------------------------------------------------\
|                        Cgv Del Wire List                    |
\------------------------------------------------------------*/

static void 
loc_delcgvwir(HeadWire)
    cgvwir_list    *HeadWire;
{
    cgvwir_list    *ScanWire;
    cgvwir_list    *DelWire;

    ScanWire = HeadWire;

    while (ScanWire != (cgvwir_list *) 0) {
        DelWire = ScanWire;
        ScanWire = ScanWire->NEXT;

        freecgvwir(DelWire);
    }
}


/*------------------------------------------------------------\
|                        Cgv Del Net List                     |
\------------------------------------------------------------*/

static void 
loc_delcgvnet(HeadNet)
    cgvnet_list    *HeadNet;
{
    cgvnet_list    *ScanNet;
    cgvnet_list    *DelNet;

    ScanNet = HeadNet;

    while (ScanNet != (cgvnet_list *) 0) {
        DelNet = ScanNet;
        ScanNet = ScanNet->NEXT;

        loc_delcgvwir(DelNet->WIRE);

        if (DelNet->CON_NET != (chain_list *) 0) {
            freechain(DelNet->CON_NET);
        }

        freecgvnet(DelNet);
    }
}

/*------------------------------------------------------------\
|                        Cgv Del Box List                     |
\------------------------------------------------------------*/

static void 
loc_delcgvbox(HeadBox)
    cgvbox_list    *HeadBox;
{
    cgvbox_list    *ScanBox;
    cgvbox_list    *DelBox;

    ScanBox = HeadBox;

    while (ScanBox != (cgvbox_list *) 0) {
        DelBox = ScanBox;
        ScanBox = ScanBox->NEXT;

        loc_delcgvcon(DelBox->CON_IN);
        loc_delcgvcon(DelBox->CON_OUT);

        freecgvbox(DelBox);
    }
}

/*------------------------------------------------------------\
|                 Cgv Del Box From Figure                     |
\------------------------------------------------------------*/

static void 
loc_delcgvboxfromFigure(Box,Figure)
    cgvbox_list    *Box;
    cgvfig_list    *Figure;
{
    cgvbox_list    *ScanBox;
    cgvbox_list    *NextBox;
    cgvbox_list    *PrevBox = NULL;

    for(ScanBox = Figure->BOX ; ScanBox != (cgvbox_list *) 0; ScanBox = NextBox){
       NextBox = ScanBox->NEXT;
       if(ScanBox == Box){
            if(PrevBox == NULL) {
                     Figure->BOX = NextBox;
             }
             else {
                     PrevBox->NEXT = NextBox;
             }
             ScanBox->NEXT = NULL; 

             freecgvcon(ScanBox->CON_IN);
             freecgvcon(ScanBox->CON_OUT);

             freecgvbox(ScanBox);
             break;
        }
       PrevBox = ScanBox;
    }
}


/*------------------------------------------------------------\
|                        Cgv Del Transparence                 |
\------------------------------------------------------------*/
void
cgvdeltransparence(Cell,Figure)
     cgvcel_list  *Cell;
     cgvfig_list  *Figure;
{
     cgvbox_list *Box;
     cgvcon_list *ConBox;
     cgvcon_list *ConNet;
     cgvnet_list *NetBox;
     chain_list  *Chain, *PrevChain, *NextChain;
     
     Box = (cgvbox_list *) Cell->ROOT;

     ConBox = Box->CON_IN;
     NetBox = ConBox->NET;
     PrevChain = NULL;
     for (Chain = NetBox->CON_NET; Chain != (chain_list *) 0; Chain = NextChain) {
         ConNet = (cgvcon_list *) Chain->DATA;
         NextChain = Chain->NEXT;
         if(ConNet == ConBox){
             if(PrevChain == NULL) {
                     NetBox->CON_NET = NextChain;
             }
             else {
                     PrevChain->NEXT = NextChain;
             }
             Chain->NEXT = NULL;
             freechain(Chain);
             break;
         }
         PrevChain = Chain;
     }
     
     ConBox = Box->CON_OUT;
     NetBox = ConBox->NET;
     PrevChain = NULL;
     for (Chain = NetBox->CON_NET; Chain != (chain_list *) 0; Chain = NextChain) {
         ConNet = (cgvcon_list *) Chain->DATA;
         NextChain = Chain->NEXT;
         if(ConNet == ConBox){
             if(PrevChain == NULL) {
                     NetBox->CON_NET = NextChain;
             }
             else {
                     PrevChain->NEXT = NextChain;
             }
             Chain->NEXT = NULL;
             freechain(Chain);
             break;
         }
         PrevChain = Chain;
     }
     
     loc_delcgvboxfromFigure(Box,Figure);
     freecgvcel(Cell);
     
}

/*------------------------------------------------------------\
|                        Cgv Del Figure                       |
\------------------------------------------------------------*/

int 
delcgvfig(Name)
    char           *Name;
{
    cgvfig_list    *Figure;
    cgvfig_list   **Previous;

    Name = namealloc(Name);
    Previous = &HEAD_CGVFIG;

    for (Figure = HEAD_CGVFIG;
         Figure != (cgvfig_list *) 0;
         Figure = Figure->NEXT) {
        if (Figure->NAME == Name)
            break;

        Previous = &Figure->NEXT;
    }

    if (Figure == (cgvfig_list *) 0)
        return (0);

    *Previous = Figure->NEXT;

    loc_delcgvcon(Figure->CON_IN);
    loc_delcgvcon(Figure->CON_OUT);

    loc_delcgvnet(Figure->NET);
    loc_delcgvbox(Figure->BOX);

    freecgvfig(Figure);

    return (1);
}

/*------------------------------------------------------------\
|                        Cgv Rmv Figure                       |
\------------------------------------------------------------*/

cgvfig_list    *
rmvcgvfig(CgvFig)
    cgvfig_list    *CgvFig;
{
    cgvfig_list    *Figure;
    cgvfig_list   **Previous;

    Previous = &HEAD_CGVFIG;

    for (Figure = HEAD_CGVFIG;
         Figure != (cgvfig_list *) 0;
         Figure = Figure->NEXT) {
        if (Figure == CgvFig)
            break;

        Previous = &Figure->NEXT;
    }

    if (Figure == (cgvfig_list *) 0)
        return ((cgvfig_list *) 0);

    *Previous = Figure->NEXT;

    loc_delcgvcon(Figure->CON_IN);
    loc_delcgvcon(Figure->CON_OUT);

    loc_delcgvnet(Figure->NET);
    loc_delcgvbox(Figure->BOX);

    freecgvfig(Figure);

    return (*Previous);
}

void freecgvfigure(cgvfig_list *Figure)
{

    loc_delcgvcon(Figure->CON_IN);
    loc_delcgvcon(Figure->CON_OUT);

    loc_delcgvnet(Figure->NET);
    loc_delcgvbox(Figure->BOX);

    freecgvfig(Figure);
}

/*------------------------------------------------------------\
|                        Cgv Del Cell List                    |
\------------------------------------------------------------*/

static void 
loc_delcgvcel(HeadCell)
    cgvcel_list    *HeadCell;
{
    cgvcel_list    *ScanCell;
    cgvcel_list    *DelCell;

    ScanCell = HeadCell;

    while (ScanCell != (cgvcel_list *) 0) {
        DelCell = ScanCell;
        ScanCell = ScanCell->NEXT;

        freecgvcel(DelCell);
    }
}

/*------------------------------------------------------------\
|                      Cgv Del Column List                    |
\------------------------------------------------------------*/

void 
delcgvcol()
{
    cgvcol_list    *ScanColumn;
    cgvcol_list    *DelColumn;

    ScanColumn = HEAD_CGVCOL;

    while (ScanColumn != (cgvcol_list *) 0) {
        loc_delcgvcel(ScanColumn->CELL);
        DelColumn = ScanColumn;
        ScanColumn = ScanColumn->NEXT;

        freecgvcol(DelColumn);
    }
    HEAD_CGVCOL = (cgvcol_list *) 0;
}

/*------------------------------------------------------------\
|                     Cgv Free Functions                      |
\------------------------------------------------------------*/

/*------------------------------------------------------------\
|                      Cgv Free Figure                        |
\------------------------------------------------------------*/

void 
freecgvfig(Figure)
    cgvfig_list    *Figure;
{
    mbkfree(Figure);
}

/*------------------------------------------------------------\
|                        Cgv Free Box                         |
\------------------------------------------------------------*/

void 
freecgvbox(Box)
    cgvbox_list    *Box;
{
    mbkfree(Box);
}

/*------------------------------------------------------------\
|                        Cgv Free Connector                   |
\------------------------------------------------------------*/

void 
freecgvcon(Connector)
    cgvcon_list    *Connector;
{
    mbkfree(Connector);
}

/*------------------------------------------------------------\
|                        Cgv Free Net                         |
\------------------------------------------------------------*/

void 
freecgvnet(Net)
    cgvnet_list    *Net;
{
    mbkfree(Net);
}

/*------------------------------------------------------------\
|                        Cgv Free Wire                        |
\------------------------------------------------------------*/

void 
freecgvwir(Wire)
    cgvwir_list    *Wire;
{
    mbkfree(Wire);
}

/*------------------------------------------------------------\
|                        Cgv Free Cell                        |
\------------------------------------------------------------*/

void 
freecgvcel(Cell)
    cgvcel_list    *Cell;
{
    mbkfree(Cell);
}

/*------------------------------------------------------------\
|                        Cgv Free Colum                       |
\------------------------------------------------------------*/

void 
freecgvcol(Colum)
    cgvcol_list    *Colum;
{
    mbkfree(Colum);
}

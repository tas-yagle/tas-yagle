/*------------------------------------------------------------\
|                                                             |
| Tool    :                     CGV                           |
|                                                             |
| File    :                   cgv_alloc.c                     |
|                                                             |
| Authors :               Picault Stephane                    |
|                         Miramond  Benoit                    |
|                         Lester Anthony                      |
|                                                             |
\------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

#include MUT_H
#include SLB_H
#include CGV_H

cgvfig_list *HEAD_CGVFIG = NULL;
cgvcol_list *HEAD_CGVCOL = NULL;

/*------------------------------------------------------------\
|                      Cgv Add Functions                      |
\------------------------------------------------------------*/

/*------------------------------------------------------------\
|                        Cgv Add Connector                    |
\------------------------------------------------------------*/

cgvnet_list *
addcgvnetcon(Net, Con)
    cgvnet_list    *Net;
    cgvcon_list    *Con;
{
    Con->NET = Net;
    Net->CON_NET = addchain(Net->CON_NET, Con);

    if (IsCgvConIn(Con)) {
        Net->NUMBER_IN++;
    }
    else {
        Net->NUMBER_OUT++;
    }
    return (Net);
}


/*------------------------------------------------------------\
|                        Cgv Add Figure                       |
\------------------------------------------------------------*/

cgvfig_list *
addcgvfig(Name,scale)
    char           *Name;
    long            scale;

{
    cgvfig_list    *Figure;

    Figure = (cgvfig_list *)mbkalloc(sizeof(cgvfig_list));
    Figure->NEXT = HEAD_CGVFIG;
    Figure->NAME = namealloc(Name);
    Figure->CON_IN = NULL;
    Figure->NUMBER_IN = 0;
    Figure->CON_OUT = NULL;
    Figure->NUMBER_OUT = 0;
    Figure->BOX = NULL;
    Figure->NET = NULL;
    Figure->SOURCE = NULL;
    Figure->SOURCE_TYPE = 0;
    Figure->X = 0;
    Figure->Y = 0;
    Figure->FLAGS = 0;
    Figure->LIBRARY = NULL;
    Figure->SCALE = scale;
    Figure->USER = NULL;

    cgv_setscale(scale);
    HEAD_CGVFIG = Figure;
    return Figure;
}

/*------------------------------------------------------------\
|                          Cgv Add Box                        |
\------------------------------------------------------------*/

cgvbox_list *
addcgvbox(Figure, Name)
    cgvfig_list    *Figure;
    char           *Name;
{
    cgvbox_list    *Box;

    Box = (cgvbox_list *)mbkalloc(sizeof(cgvbox_list));
    Box->NEXT = Figure->BOX;
    Box->NAME = namealloc(Name);
    Box->CON_IN = NULL;
    Box->NUMBER_IN = 0;
    Box->CON_OUT = NULL;
    Box->NUMBER_OUT = 0;
    Box->SOURCE = NULL;
    Box->SOURCE_TYPE = 0;
    Box->X = 0;
    Box->Y = 0;
    Box->DX = 0;
    Box->DY = 0;
    Box->FLAGS = 0;
    Box->SYMBOL = NULL;
    Box->USER = NULL;
    Box->misc.UNDER=NULL;
    Figure->BOX = Box;
    return Box;
}

/*------------------------------------------------------------\
|                       Cgv Add Box Connector                 |
\------------------------------------------------------------*/

cgvcon_list *
addcgvboxcon(Figure, Box, Name, Type)
    cgvfig_list    *Figure;
    cgvbox_list    *Box;
    char           *Name;
    unsigned char   Type;
{
    cgvcon_list    *Connector;

#ifndef __ALL__WARNING__
	Figure = NULL;
#endif
    Connector = (cgvcon_list *)mbkalloc(sizeof(cgvcon_list));
    Connector->NAME = namealloc(Name);
    Connector->DIR = 0;
    Connector->NET = NULL;
    Connector->SOURCE = NULL;
    Connector->SOURCE_TYPE = 0;
    Connector->X_REL = 0;
    Connector->Y_REL = 0;
    Connector->FLAGS = 0;
    Connector->USER = NULL;
    Connector->SUB_NAME=NULL;

    if (Type == CGV_CON_IN) {
        Connector->NEXT = Box->CON_IN;
        Box->CON_IN = Connector;
        Box->NUMBER_IN++;
    }
    else {
        Connector->NEXT = Box->CON_OUT;
        Box->CON_OUT = Connector;
        Box->NUMBER_OUT++;
    }

    Connector->TYPE = Type;
    SetCgvConInternal(Connector);
    Connector->ROOT_TYPE = CGV_ROOT_CON_BOX;
    Connector->ROOT = Box;

    return (Connector);
}

/*------------------------------------------------------------\
|                   Cgv Add Figure Connector                  |
\------------------------------------------------------------*/

cgvcon_list *
addcgvfigcon(Figure, Name, Type)
    cgvfig_list    *Figure;
    char           *Name;
    unsigned char   Type;
{
    cgvcon_list    *Connector;

    Connector = (cgvcon_list *)mbkalloc(sizeof(cgvcon_list));
    Connector->NAME = namealloc(Name);
    Connector->DIR = 0;
    Connector->NET = NULL;
    Connector->SOURCE = NULL;
    Connector->SOURCE_TYPE = 0;
    Connector->X_REL = 0;
    Connector->Y_REL = 0;
    Connector->FLAGS = 0;
    Connector->USER = NULL;
    Connector->SUB_NAME = NULL;

    if (Type == CGV_CON_IN) {
        Connector->NEXT = Figure->CON_IN;
        Figure->CON_IN = Connector;
        Figure->NUMBER_IN++;
    }
    else {
        Connector->NEXT = Figure->CON_OUT;
        Figure->CON_OUT = Connector;
        Figure->NUMBER_OUT++;
    }

    Connector->TYPE = Type;
    SetCgvConExternal(Connector);
    Connector->ROOT_TYPE = CGV_ROOT_CON_FIG;
    Connector->ROOT = Figure;

    return Connector;
}

/*------------------------------------------------------------\
|                        Cgv Add Net                          |
\------------------------------------------------------------*/

cgvnet_list *
addcgvnet(Figure)
    cgvfig_list    *Figure;
{
    cgvnet_list    *Net;

    Net = (cgvnet_list *)mbkalloc(sizeof(cgvnet_list));
    Net->NEXT = Figure->NET;
    Net->CON_NET = NULL;
    Net->WIRE = NULL;
    Net->SOURCE = NULL;
    Net->SOURCE_TYPE = 0;
    Net->FLAGS = 0;
    Net->NUMBER_IN = 0;
    Net->NUMBER_OUT = 0;
    Net->CANAL = 0;
    Net->YMIN = 0;
    Net->YMAX = 0;
    Net->USER = NULL;
    Net->NAME=NULL;

    Figure->NET = Net;
    return Net;
}

/*------------------------------------------------------------\
|                        Cgv Add Wire                         |
\------------------------------------------------------------*/

cgvwir_list *
addcgvwir(Figure, Net)
    cgvfig_list    *Figure;
    cgvnet_list    *Net;
{
    cgvwir_list    *Wire;

#ifndef __ALL__WARNING__
	Figure = NULL;
#endif
    Wire = (cgvwir_list *)mbkalloc(sizeof(cgvwir_list));
    Wire->NEXT = Net->WIRE;
    Wire->NET = NULL;
    Wire->X = 0;
    Wire->Y = 0;
    Wire->DX = 0;
    Wire->DY = 0;
    Wire->FLAGS = 0;
    Wire->USER = NULL;

    Net->WIRE = Wire;
    return Wire;
}

/*------------------------------------------------------------\
|                        Cgv Add Cell                         |
\------------------------------------------------------------*/

cgvcel_list *
addcgvcel(Column)
    cgvcol_list    *Column;
{
    cgvcel_list    *Cell;

    Cell = (cgvcel_list *)mbkalloc(sizeof(cgvcel_list));
    Cell->COL = Column;
    Cell->NEXT = Column->CELL;
    Cell->PREV = NULL;
    Cell->ROOT = NULL;
    Cell->TYPE = 0;

    if (Column->CELL != NULL) {
        Column->CELL->PREV = Cell;
    }
    else {
        Column->LAST_CELL = Cell;
    }

    Column->CELL = Cell;
    Column->NUMBER_CELL++;

    return Cell;
}

/*------------------------------------------------------------\
|                        Cgv Add Box to Cell                  |
\------------------------------------------------------------*/

cgvcel_list *
addcgvboxtocgvcel(CgvBox, CgvCel)
    cgvbox_list    *CgvBox;
    cgvcel_list    *CgvCel;
{
    CgvCel->ROOT = CgvBox;
    CgvCel->TYPE = CGV_CEL_BOX;
    CgvBox->USER = CgvCel;
    return CgvCel;
}

/*------------------------------------------------------------\
|                        Cgv Add Con to Cell                  |
\------------------------------------------------------------*/

cgvcel_list *
addcgvcontocgvcel(CgvCon, CgvCel)
    cgvcon_list    *CgvCon;
    cgvcel_list    *CgvCel;
{
    CgvCel->ROOT = CgvCon;
    CgvCel->TYPE = CGV_CEL_CON;
    CgvCon->USER = CgvCel;
    return CgvCel;
}

/*------------------------------------------------------------\
|                        Cgv Add Column                       |
\------------------------------------------------------------*/

cgvcol_list *
addcgvcol()
{
    cgvcol_list    *Column;

    Column = (cgvcol_list *)mbkalloc(sizeof(cgvcol_list));
    Column->NEXT = HEAD_CGVCOL;
    Column->CELL = NULL;
    Column->LAST_CELL = NULL;
    Column->NUMBER_CELL = 0;
    Column->MAX_DX_CELL = 0;

    HEAD_CGVCOL = Column;
    return Column;
}


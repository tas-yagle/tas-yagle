/*------------------------------------------------------------\
|                                                             |
| Tool    :                     Scg                           |
|                                                             |
| File    :                 cgv_cnsfig.c                      |
|                                                             |
|                                                             |
| Authors :               Picault  Stephane                   |
|                         Miramond   Benoit                   |
|                         Lester Anthony                      |
|                                                             |
\------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include "cgv_mapping.h"


#include MUT_H
#include SLB_H
#include BEH_H
#include CGV_H
#include CNS_H
#include MLO_H
#include CNS_H
#include INF_H
#include YAG_H

symbol_list    *cgv_getcnssymbol();
symbol_list    *cgv_getcellsymbol();
static chain_list *all_sigs;

/*------------------------------------------------------------\
|                          isconemaillonconext                |
\------------------------------------------------------------*/

int
isconemaillonconext(CnsCone)
    cone_list      *CnsCone;
{
    if (CnsCone->BRVDD != NULL)
        return 0;
    if (CnsCone->BRVSS != NULL)
        return 0;
    if (CnsCone->BREXT != NULL) {
        if ((CnsCone->BREXT->NEXT == NULL) && (CnsCone->BREXT->LINK->NEXT == NULL)) {
            return 1;
        }
    }
    return 0;
}

/*------------------------------------------------------------\
|                          patch_sel                          |
\------------------------------------------------------------*/

cgvfig_list    *
patch_sel(CgvFigure)
    cgvfig_list    *CgvFigure;
{
    cgvbox_list    *ScanBox;
    cgvcon_list    *ScanConOut, *CgvCon;
    cgvnet_list    *ScanNet;
    cone_list      *Cone, *ScanCone;
    edge_list      *ScanEdge;
    int             go;

    for (ScanBox = CgvFigure->BOX; ScanBox; ScanBox = ScanBox->NEXT) {
        for (ScanConOut = ScanBox->CON_OUT; ScanConOut; ScanConOut = ScanConOut->NEXT) {
            go = 0;
            ScanNet = ScanConOut->NET;

            if (ScanNet != NULL) {
                if (ScanNet->SOURCE_TYPE == CGV_SOURCE_CNSCONE) {
                    ScanCone = (cone_list *) ScanNet->SOURCE;

                    for (ScanEdge = ScanCone->OUTCONE; ScanEdge; ScanEdge = ScanEdge->NEXT) {
                        if ((ScanEdge->TYPE & CNS_EXT) != CNS_EXT) {
                            Cone = ScanEdge->UEDGE.CONE;
                            if (getptype(Cone->USER, CGV_BOX_TYPE) == NULL) {
                                go = 1;
                                break;
                            }
                        }
                    }
                }
            }
            if (go == 1) {
                CgvCon = addcgvfigcon(CgvFigure, ScanCone->NAME, CGV_CON_OUT);
                addcgvnetcon(ScanNet, CgvCon);
                ScanNet->NAME = ScanCone->NAME;
                SetCgvConFake(CgvCon);
            }
        }
    }

    return (CgvFigure);
}

/*------------------------------------------------------------\
|                          cgvsearchcone                      |
\------------------------------------------------------------*/

cone_list      *
cgvsearchcone(CnsFigure, Name)
    cnsfig_list    *CnsFigure;
    char           *Name;
{
    cone_list      *ScanCone;

    for (ScanCone = CnsFigure->CONE; ScanCone; ScanCone = ScanCone->NEXT) {
        if (namealloc(Name) == ScanCone->NAME) {
            return (ScanCone);
        }
    }

    return NULL;
}

/*------------------------------------------------------------\
|                          loc_add_con_ext                    |
\------------------------------------------------------------*/

cgvcon_list    *
loc_add_con_ext(CgvFigure, LoCon)
    cgvfig_list    *CgvFigure;
    locon_list     *LoCon;
{
    cgvnet_list    *CgvNet;
    cgvcon_list    *CgvCon;
    long            CgvDir;

    if (LoCon->DIRECTION == CNS_VDDC) {
        return ((cgvcon_list *) 0);
    }
    if (LoCon->DIRECTION == CNS_VSSC) {
        return ((cgvcon_list *) 0);
    }

    if (LoCon->DIRECTION == IN) {
        CgvDir = CGV_CON_IN;
    }
    else {
        CgvDir = CGV_CON_OUT;
    }

    CgvCon = addcgvfigcon(CgvFigure, LoCon->NAME, CgvDir);
    CgvCon->SOURCE = (void *) LoCon;
    CgvCon->SOURCE_TYPE = CGV_SOURCE_LOCON;

    CgvNet = addcgvnet(CgvFigure);
    CgvNet->SOURCE_TYPE = CGV_SOURCE_LOCON;
    CgvNet->SOURCE = LoCon;
    CgvNet->NAME = LoCon->NAME;
    LoCon->USER = addptype(LoCon->USER, CGV_NET_TYPE, CgvNet);
    if (LoCon->SIG) {
        LoCon->SIG->USER = addptype(LoCon->SIG->USER, CGV_NET_TYPE, CgvNet);
        all_sigs = addchain(all_sigs, LoCon->SIG);
    }

    addcgvnetcon(CgvNet, CgvCon);

    return (CgvCon);
}

/*------------------------------------------------------------\
|             WARNING  A RETIRER                              |
\------------------------------------------------------------*/
char           *
getinputpin(cgvbox_list * CgvBox, char *default_name)
{
    symbol_list    *ptsym;
    draw_list      *ptdraw;

    ptsym = CgvBox->SYMBOL;
    if (ptsym == NULL)
        return default_name;

    for (ptdraw = ptsym->DRAW; ptdraw; ptdraw = ptdraw->NEXT) {
        if (ptdraw->TYPE == SLIB_T_PIN) {
            if (ptdraw->DATA.SPIN->DIRECTION == SLIB_PIN_LEFT) {
                ptdraw->TYPE = 0;
                return ptdraw->DATA.SPIN->NAME;
            }
        }
    }
    return default_name;
}

char           *
getoutputpin(cgvbox_list * CgvBox, char *default_name)
{
    symbol_list    *ptsym;
    draw_list      *ptdraw;

    ptsym = CgvBox->SYMBOL;
    if (ptsym == NULL)
        return default_name;

    for (ptdraw = ptsym->DRAW; ptdraw; ptdraw = ptdraw->NEXT) {
        if (ptdraw->TYPE == SLIB_T_PIN) {
            if (ptdraw->DATA.SPIN->DIRECTION == SLIB_PIN_RIGHT) {
                return ptdraw->DATA.SPIN->NAME;
            }
        }
    }
    return default_name;
}

void 
restorepin(cgvbox_list * CgvBox)
{
    symbol_list    *ptsym;
    draw_list      *ptdraw;

    ptsym = CgvBox->SYMBOL;
    if (ptsym == NULL)
        return;

    for (ptdraw = ptsym->DRAW; ptdraw; ptdraw = ptdraw->NEXT) {
        if (ptdraw->TYPE == 0) {
            ptdraw->TYPE = SLIB_T_PIN;
        }
    }
}

/*------------------------------------------------------------\
|                      intputofcell                           |
\------------------------------------------------------------*/
int
inputofcell(edge_list * LoconCone, cell_list * CnsCell)
{
    chain_list     *ChainCone;

    if ((LoconCone->TYPE & CNS_EXT) == CNS_EXT) {
        return 1;
    }
    else {
        for (ChainCone = CnsCell->CONES; ChainCone; ChainCone = ChainCone->NEXT) {
            if (LoconCone->UEDGE.CONE == (cone_list *) ChainCone->DATA)
                break;
        }
        if (ChainCone == NULL)
            return 1;
    }
    return 0;
}

/*------------------------------------------------------------\
|                    allreadyaconin                           |
\------------------------------------------------------------*/
int
allreadyaconin(cgvbox_list * CgvBox, char *name)
{
    cgvcon_list    *Connector;

    for (Connector = CgvBox->CON_IN; Connector; Connector = Connector->NEXT) {
        if (Connector->NAME == name)
            break;
    }
    if (Connector == NULL)
        return 0;

    return 1;
}

/*------------------------------------------------------------\
|                       loc_add_conincell                     |
\------------------------------------------------------------*/
cgvfig_list    *
loc_add_conincell(cgvfig_list * CgvFigure, cell_list * CnsCell)
{

    chain_list     *CnsConeChaine;
    cone_list      *CnsCone;
    cgvbox_list    *CgvBox;
    cgvnet_list    *CgvNet;
    cgvcon_list    *CgvCon;
    cgvcon_list    *CgvConExt;
    locon_list     *LoCon;
    edge_list      *CnsEdge;
    cone_list      *CnsConeIn;
    char           *name;

    if (getptype(CnsCell->USER, CGV_BOX_TYPE) == NULL) {
        return (NULL);
    }

    CgvBox = (cgvbox_list *) getptype(CnsCell->USER, CGV_BOX_TYPE)->DATA;

    for (CnsConeChaine = CnsCell->CONES; CnsConeChaine; CnsConeChaine = CnsConeChaine->NEXT) {
        CnsCone = (cone_list *) CnsConeChaine->DATA;

        if ((CnsCone->TYPE & CNS_POWER) == CNS_POWER) {
            return (CgvFigure);
        }

        //connecteurs d entree +
        for (CnsEdge = CnsCone->INCONE; CnsEdge; CnsEdge = CnsEdge->NEXT) {
            if (inputofcell(CnsEdge, CnsCell) == 0)
                continue;

            if ((CnsEdge->TYPE & CNS_EXT) == CNS_EXT) {
                LoCon = CnsEdge->UEDGE.LOCON;

                if ((CnsCone->TYPE & CNS_EXT) == CNS_EXT) {
                    if (getchain(getptype(CnsCone->USER, CNS_EXT)->DATA, LoCon) != NULL
                        && ((CnsCone->BRVDD != NULL) && (CnsCone->BRVSS != NULL))) {
                        continue;
                    }
                }

                if (getptype(LoCon->USER, CGV_NET_TYPE) != NULL) {
                    CgvNet = (cgvnet_list *) getptype(LoCon->USER, CGV_NET_TYPE)->DATA;
                    name = CnsEdge->UEDGE.LOCON->NAME;
                }
                else {
                    if (LoCon->FLAGS == 0) {
                        CgvConExt = loc_add_con_ext(CgvFigure, LoCon);
                        CgvNet = CgvConExt->NET;
                    }
                    else
                        CgvNet = addcgvnet(CgvFigure);
                    LoCon->USER = addptype(LoCon->USER, CGV_NET_TYPE, CgvNet);
                    if (LoCon->SIG) {
                        LoCon->SIG->USER = addptype(LoCon->SIG->USER, CGV_NET_TYPE, CgvNet);
                        all_sigs = addchain(all_sigs, LoCon->SIG);
                    }

                    name = LoCon->NAME;
                }
            }
            else {
                name = CnsEdge->UEDGE.CONE->NAME;
                CnsConeIn = CnsEdge->UEDGE.CONE;

                if (getptype(CnsConeIn->USER, CGV_NET_TYPE) != NULL) {
                    CgvNet = (cgvnet_list *) getptype(CnsConeIn->USER, CGV_NET_TYPE)->DATA;
                }
                else {
                    if (isconemaillonconext(CnsConeIn)) {
                        LoCon = CnsConeIn->INCONE->UEDGE.LOCON;
                        if (getptype(LoCon->USER, CGV_NET_TYPE) != NULL) {
                            CgvNet = (cgvnet_list *) getptype(LoCon->USER, CGV_NET_TYPE)->DATA;
                            name = LoCon->NAME;
                        }
                        else {
                            if (LoCon->FLAGS == 0) {
                                CgvConExt = loc_add_con_ext(CgvFigure, LoCon);
                                CgvNet = CgvConExt->NET;
                            }
                            else
                                CgvNet = addcgvnet(CgvFigure);
                            LoCon->USER = addptype(LoCon->USER, CGV_NET_TYPE, CgvNet);
                            if (LoCon->SIG) {
                                LoCon->SIG->USER = addptype(LoCon->SIG->USER, CGV_NET_TYPE, CgvNet);
                                all_sigs = addchain(all_sigs, LoCon->SIG);
                            }

                            name = LoCon->NAME;
                        }
                    }
                    else if ((CnsConeIn->TYPE & CNS_POWER) != CNS_POWER) {
                        LoCon = (locon_list *) mbkalloc(sizeof(locon_list));
                        LoCon->NAME = name;
                        LoCon->DIRECTION = IN;
                        CnsConeIn->USER = addptype(CnsConeIn->USER, CGV_NET_TYPE, CgvNet);
                        CgvConExt = loc_add_con_ext(CgvFigure, LoCon);
                        SetCgvConFake(CgvConExt);
                        CgvNet = CgvConExt->NET;
                    }
                    else {
                        continue;
                    }
                }
            }

            if (allreadyaconin(CgvBox, name) == 1)
                continue;

            CgvCon = addcgvboxconin(CgvFigure, CgvBox, getinputpin(CgvBox, name));

            CgvCon->ROOT = CgvBox;

            CgvCon->SOURCE = CnsEdge;
            CgvCon->SOURCE_TYPE = CGV_SOURCE_CNSEDGE;

            addcgvnetcon(CgvNet, CgvCon);
        }

        CgvBox->CON_IN = (cgvcon_list *) reverse((chain_list *) CgvBox->CON_IN);
        restorepin(CgvBox);

    }
    return (CgvFigure);
}

/*------------------------------------------------------------\
|                          loc_add_conin                      |
\------------------------------------------------------------*/

cgvfig_list    *
loc_add_conin(CgvFigure, CnsCone, CnsFigure)
    cgvfig_list    *CgvFigure;
    cone_list      *CnsCone;
    cnsfig_list    *CnsFigure;
{
    cgvbox_list    *CgvBox;
    cgvnet_list    *CgvNet;
    cgvcon_list    *CgvCon;
    cgvcon_list    *CgvConExt;
    locon_list     *LoCon;
    edge_list      *CnsEdge;
    cone_list      *CnsConeIn;
    char           *name;

    //locon_list * lc;
#ifndef __ALL__WARNING__
    CnsFigure = NULL;
#endif

    if ((CnsCone->TYPE & CNS_POWER) == CNS_POWER) {
        return (CgvFigure);
    }


    if (getptype(CnsCone->USER, CGV_BOX_TYPE) == NULL) {
        return (NULL);
    }

    CgvBox = (cgvbox_list *) getptype(CnsCone->USER, CGV_BOX_TYPE)->DATA;

    //connecteurs d entree +
    for (CnsEdge = CnsCone->INCONE; CnsEdge; CnsEdge = CnsEdge->NEXT) {
        if ((CnsEdge->TYPE & CNS_EXT) == CNS_EXT) {
            LoCon = CnsEdge->UEDGE.LOCON;

            if ((CnsCone->TYPE & CNS_EXT) == CNS_EXT) {
                if (getchain(getptype(CnsCone->USER, CNS_EXT)->DATA, LoCon) != NULL
                && ((CnsCone->BRVDD != NULL) && (CnsCone->BRVSS != NULL))) {
                    continue;
                }
            }

            if (getptype(LoCon->USER, CGV_NET_TYPE) != NULL) {
                CgvNet = (cgvnet_list *) getptype(LoCon->USER, CGV_NET_TYPE)->DATA;
                name = CnsEdge->UEDGE.LOCON->NAME;
            }
            else {
                if (LoCon->FLAGS == 0) {
                    CgvConExt = loc_add_con_ext(CgvFigure, LoCon);
                    CgvNet = CgvConExt->NET;
                }
                else
                    CgvNet = addcgvnet(CgvFigure);
                LoCon->USER = addptype(LoCon->USER, CGV_NET_TYPE, CgvNet);
                if (LoCon->SIG) {
                    LoCon->SIG->USER = addptype(LoCon->SIG->USER, CGV_NET_TYPE, CgvNet);
                    all_sigs = addchain(all_sigs, LoCon->SIG);
                }

                name = LoCon->NAME;
            }
            CgvNet->NAME = name;
        }
        else {
            name = CnsEdge->UEDGE.CONE->NAME;
            CnsConeIn = CnsEdge->UEDGE.CONE;

            if (getptype(CnsConeIn->USER, CGV_NET_TYPE) != NULL) {
                CgvNet = (cgvnet_list *) getptype(CnsConeIn->USER, CGV_NET_TYPE)->DATA;
            }
            else {
                if (isconemaillonconext(CnsConeIn)) {
                    LoCon = CnsConeIn->INCONE->UEDGE.LOCON;
                    if (getptype(LoCon->USER, CGV_NET_TYPE) != NULL) {
                        CgvNet = (cgvnet_list *) getptype(LoCon->USER, CGV_NET_TYPE)->DATA;
                        name = LoCon->NAME;
                    }
                    else {
                        if (LoCon->FLAGS == 0) {
                            CgvConExt = loc_add_con_ext(CgvFigure, LoCon);
                            CgvNet = CgvConExt->NET;
                        }
                        else
                            CgvNet = addcgvnet(CgvFigure);
                        LoCon->USER = addptype(LoCon->USER, CGV_NET_TYPE, CgvNet);
                        if (LoCon->SIG) {
                            LoCon->SIG->USER = addptype(LoCon->SIG->USER, CGV_NET_TYPE, CgvNet);
                            all_sigs = addchain(all_sigs, LoCon->SIG);
                        }

                        name = LoCon->NAME;

                    }
                }
                else if ((CnsConeIn->TYPE & CNS_POWER) != CNS_POWER) {
                    LoCon = (locon_list *) mbkalloc(sizeof(locon_list));
                    LoCon->NAME = name;
                    LoCon->DIRECTION = IN;
                    CnsConeIn->USER = addptype(CnsConeIn->USER, CGV_NET_TYPE, CgvNet);
                    CgvConExt = loc_add_con_ext(CgvFigure, LoCon);
                    SetCgvConFake(CgvConExt);
                    CgvNet = CgvConExt->NET;
                }
                else {
                    continue;
                }
            }
        }

        CgvCon = addcgvboxconin(CgvFigure, CgvBox, getinputpin(CgvBox, name));

        CgvCon->ROOT = CgvBox;

        CgvCon->SOURCE = CnsEdge;
        CgvCon->SOURCE_TYPE = CGV_SOURCE_CNSEDGE;

        addcgvnetcon(CgvNet, CgvCon);
    }

    CgvBox->CON_IN = (cgvcon_list *) reverse((chain_list *) CgvBox->CON_IN);
    restorepin(CgvBox);

    return (CgvFigure);
}

/*------------------------------------------------------------\
|                      outputofcell                           |
\------------------------------------------------------------*/
int
outputofcell(cone_list *CnsCone, cell_list * CnsCell)
{
    edge_list      *LoconCone;
    chain_list     *ChainCone;

    for (LoconCone = CnsCone->OUTCONE; LoconCone; LoconCone = LoconCone->NEXT) {
        if ((LoconCone->TYPE & CNS_EXT) == CNS_EXT) {
            return 1;
        }
        else {
            for (ChainCone = CnsCell->CONES; ChainCone; ChainCone = ChainCone->NEXT) {
                if (LoconCone->UEDGE.CONE == (cone_list *) ChainCone->DATA)
                    break;
            }
            if (ChainCone == NULL)
                return 1;
        }
    }
    return 0;
}

/*------------------------------------------------------------\
|                          loc_add_cell                       |
\------------------------------------------------------------*/

cgvbox_list    *
loc_add_cell(CgvFigure, CnsCell)
    cgvfig_list    *CgvFigure;
    cell_list      *CnsCell;
{
    cgvbox_list    *CgvBox;
    cgvnet_list    *CgvNet;
    cgvcon_list    *CgvCon;
    locon_list     *LoCon;
    chain_list     *CnsConeChaine;
    cone_list      *CnsCone;
    chain_list     *ptchain;
    ptype_list     *ptuser;
    char           *CellName;
    char            buff[1024];

    ptuser = getptype(CnsCell->USER, (long) CNS_INDEX);

    if (CnsCell->TYPE == (YAG_CELL_MS_SC | CNS_UNKNOWN)) {
        sprintf(buff, "ms_sc_i%ld", *(long *) ptuser->DATA);
        CellName = namealloc(buff);
    }
    else if (CnsCell->TYPE == (YAG_CELL_MSS_SC | CNS_UNKNOWN)) {
        sprintf(buff, "mss_sc_i%ld", *(long *) ptuser->DATA);
        CellName = namealloc(buff);
    }
    else if (CnsCell->TYPE == (YAG_CELL_MSR_SC | CNS_UNKNOWN)) {
        sprintf(buff, "msr_sc_i%ld", *(long *) ptuser->DATA);
        CellName = namealloc(buff);
    }
    else if (CnsCell->TYPE == (YAG_CELL_MSNR_SC | CNS_UNKNOWN)) {
        sprintf(buff, "msnr_sc_i%ld", *(long *) ptuser->DATA);
        CellName = namealloc(buff);
    }
    else if (CnsCell->TYPE == (YAG_CELL_MS_SC_RT | CNS_UNKNOWN)) {
        sprintf(buff, "ms_sc_rt_i%ld", *(long *) ptuser->DATA);
        CellName = namealloc(buff);
    }
    else if (CnsCell->TYPE == (YAG_CELL_MSS_SC_RT | CNS_UNKNOWN)) {
        sprintf(buff, "mss_sc_rt_i%ld", *(long *) ptuser->DATA);
        CellName = namealloc(buff);
    }
    else if (CnsCell->TYPE == (YAG_CELL_MSR_SC_RT | CNS_UNKNOWN)) {
        sprintf(buff, "msr_sc_rt_i%ld", *(long *) ptuser->DATA);
        CellName = namealloc(buff);
    }
    else if (CnsCell->TYPE == (YAG_CELL_MSNR_SC_RT | CNS_UNKNOWN)) {
        sprintf(buff, "msnr_sc_rt_i%ld", *(long *) ptuser->DATA);
        CellName = namealloc(buff);
    }
    else if (CnsCell->TYPE == (YAG_CELL_MSDIFF | CNS_UNKNOWN)) {
        sprintf(buff, "msdiff_i%ld", *(long *) ptuser->DATA);
        CellName = namealloc(buff);
    }
    else if (CnsCell->TYPE == (YAG_CELL_MS_SX | CNS_UNKNOWN)) {
        sprintf(buff, "ms_sx_i%ld", *(long *) ptuser->DATA);
        CellName = namealloc(buff);
    }
    else if (CnsCell->TYPE == (YAG_CELL_MS_SX_RT | CNS_UNKNOWN)) {
        sprintf(buff, "ms_sx_rt_i%ld", *(long *) ptuser->DATA);
        CellName = namealloc(buff);
    }
    else if (CnsCell->TYPE == (YAG_CELL_FFT2 | CNS_UNKNOWN)) {
        sprintf(buff, "fft2_i%ld", *(long *) ptuser->DATA);
        CellName = namealloc(buff);
    }
    else if (CnsCell->TYPE == (YAG_CELL_FD2R | CNS_UNKNOWN)) {
        sprintf(buff, "fd2r_i%ld", *(long *) ptuser->DATA);
        CellName = namealloc(buff);
    }
    else if (CnsCell->TYPE == (YAG_CELL_FD2S | CNS_UNKNOWN)) {
        sprintf(buff, "fd2s_i%ld", *(long *) ptuser->DATA);
        CellName = namealloc(buff);
    }
    else if (CnsCell->TYPE == (YAG_CELL_LD1 | CNS_UNKNOWN)) {
        sprintf(buff, "ld1_i%ld", *(long *) ptuser->DATA);
        CellName = namealloc(buff);
    }
    else if (CnsCell->TYPE == (YAG_CELL_LD1R | CNS_UNKNOWN)) {
        sprintf(buff, "ld1r_i%ld", *(long *) ptuser->DATA);
        CellName = namealloc(buff);
    }
    else if (CnsCell->TYPE == (YAG_CELL_BINAND | CNS_UNKNOWN)) {
        sprintf(buff, "binand_i%ld", *(long *) ptuser->DATA);
        CellName = namealloc(buff);
    }
    else if (CnsCell->TYPE == (YAG_CELL_BINOR | CNS_UNKNOWN)) {
        sprintf(buff, "binor_i%ld", *(long *) ptuser->DATA);
        CellName = namealloc(buff);
    }
    else if (CnsCell->TYPE == (YAG_CELL_LATCH | CNS_UNKNOWN)) {
        sprintf(buff, "latch_i%ld", *(long *) ptuser->DATA);
        CellName = namealloc(buff);
    }
    else {
        sprintf(buff, "cell_i%ld", *(long *) ptuser->DATA);
        CellName = namealloc(buff);
    }


    CgvBox = addcgvbox(CgvFigure, CellName);
    CgvBox->SOURCE = (void *) CnsCell;
    CgvBox->SOURCE_TYPE = CGV_SOURCE_CNSCELL;
    CgvBox->SYMBOL = cgv_getcellsymbol(CgvFigure->LIBRARY, NULL);

    CnsCell->USER = addptype(CnsCell->USER, CGV_BOX_TYPE, CgvBox);

    for (CnsConeChaine = CnsCell->CONES; CnsConeChaine; CnsConeChaine = CnsConeChaine->NEXT) {
        CnsCone = (cone_list *) CnsConeChaine->DATA;

        if (outputofcell(CnsCone, CnsCell) == 0) continue;

        if ((CnsCone->TYPE & CNS_EXT) == CNS_EXT) {
            if ((ptuser = getptype(CnsCone->USER, CNS_EXT)) == NULL) continue;
            for (ptchain = (chain_list *)ptuser->DATA; ptchain ; ptchain = ptchain->NEXT) {
                LoCon = (locon_list *)ptchain->DATA;
                if (LoCon->DIRECTION != IN) {
                    if (getptype(LoCon->USER, CGV_NET_TYPE) == NULL) continue;
                    CgvNet = (cgvnet_list *) getptype(LoCon->USER, CGV_NET_TYPE)->DATA;
                }
                else CgvNet = addcgvnet(CgvFigure);
            }
        }
        else CgvNet = addcgvnet(CgvFigure);

        CgvNet->SOURCE_TYPE = CGV_SOURCE_CNSCELL;
        CgvNet->SOURCE = CnsCell;

        CnsCone->USER = addptype(CnsCone->USER, CGV_NET_TYPE, CgvNet);

        CgvCon = addcgvboxconout(CgvFigure, CgvBox, getoutputpin(CgvBox, CnsCone->NAME));

        CgvCon->ROOT = CgvBox;

        CgvCon->SOURCE_TYPE = CGV_SOURCE_CNSCELL;
        CgvCon->SOURCE = CnsCell;

        addcgvnetcon(CgvNet, CgvCon);
        CgvNet->NAME = CnsCone->NAME;
    }
    return (CgvBox);
}

/*------------------------------------------------------------\
|                          loc_add_cone                       |
\------------------------------------------------------------*/

cgvbox_list    *
loc_add_cone(CgvFigure, CnsCone)
    cgvfig_list    *CgvFigure;
    cone_list      *CnsCone;
{
    cgvbox_list    *CgvBox;
    cgvnet_list    *CgvNet;
    cgvcon_list    *CgvCon;
    locon_list     *LoCon;
    chain_list     *ptchain;
    ptype_list     *ptuser;

    if ((CnsCone->TYPE & CNS_POWER) == CNS_POWER) {
        return (0);
    }
    if ((isconemaillonconext(CnsCone)) == 1) {
        return 0;
    }

    CgvBox = addcgvbox(CgvFigure, CnsCone->NAME);
    CgvBox->SOURCE = (void *) CnsCone;
    CgvBox->SOURCE_TYPE = CGV_SOURCE_CNSCONE;
    if (CnsCone->BRVDD == NULL && CnsCone->BRVSS == NULL && CnsCone->BREXT == NULL) {
        CgvBox->SYMBOL = cgv_getselfcellsymbol(CgvFigure->LIBRARY, namealloc("cross"));
    }
    else
        CgvBox->SYMBOL = cgv_getcnssymbol(CgvFigure->LIBRARY, CnsCone);


    CnsCone->USER = addptype(CnsCone->USER, CGV_BOX_TYPE, CgvBox);

    if ((CnsCone->TYPE & CNS_EXT) == CNS_EXT) {
        if ((ptuser = getptype(CnsCone->USER, CNS_EXT)) == NULL) return 0;

        for (ptchain = (chain_list *)ptuser->DATA; ptchain; ptchain = ptchain->NEXT) {
            LoCon = (locon_list *)ptchain->DATA;
            if (LoCon->DIRECTION != IN) {
                if (getptype(LoCon->USER, CGV_NET_TYPE) == NULL) {
                    if (LoCon->FLAGS == 0) loc_add_con_ext(CgvFigure, LoCon);
                    else {
                        CgvNet = addcgvnet(CgvFigure);
                        LoCon->USER = addptype(LoCon->USER, CGV_NET_TYPE, CgvNet);
                        if (LoCon->SIG) {
                            LoCon->SIG->USER = addptype(LoCon->SIG->USER, CGV_NET_TYPE, CgvNet);
                            all_sigs = addchain(all_sigs, LoCon->SIG);
                        }
                    }
                }
                else CgvNet = (cgvnet_list *) getptype(LoCon->USER, CGV_NET_TYPE)->DATA;
            }
            else CgvNet = addcgvnet(CgvFigure);
        }
    }
    else {
        CgvNet = addcgvnet(CgvFigure);
    }

    CgvNet->SOURCE_TYPE = CGV_SOURCE_CNSCONE;
    CgvNet->SOURCE = CnsCone;

    CnsCone->USER = addptype(CnsCone->USER, CGV_NET_TYPE, CgvNet);

    CgvCon = addcgvboxconout(CgvFigure, CgvBox, getoutputpin(CgvBox, CnsCone->NAME));

    if (CnsCone->BRVDD == NULL && CnsCone->BRVSS == NULL && CnsCone->BREXT == NULL)
        CgvCon->NAME = namealloc("mid");

    CgvCon->ROOT = CgvBox;

    CgvCon->SOURCE_TYPE = CGV_SOURCE_CNSCONE;
    CgvCon->SOURCE = CnsCone;

    addcgvnetcon(CgvNet, CgvCon);
    CgvNet->NAME = CnsCone->NAME;

    return (CgvBox);
}

/*------------------------------------------------------------\
|                          cgvlistcone_rec                    |
\------------------------------------------------------------*/

chain_list     *
cgvlistcone_rec(ChainCone, Cone, Mode, Depth, CgvFigure, ptcell)
    chain_list     *ChainCone;
    cone_list      *Cone;
    int             Depth;
    cgvfig_list    *CgvFigure;
    cell_list      *ptcell;
    int             Mode;

{
    edge_list      *CnsEdge;
    int             i = 0;

    if (((Depth > 0) || (Depth <= -1)) || Cone->CELLS != NULL) {

        if (Cone->CELLS == NULL) {
            i = 1;
            ptcell = NULL;
        }
        else if (Cone->CELLS != NULL) {
            if (Cone->CELLS->NEXT == NULL && (cell_list *) Cone->CELLS->DATA != ptcell) {
                i = 1;
                ptcell = (cell_list *) Cone->CELLS->DATA;
            }
            else
                i = 0;
        }

        if (getptype(Cone->USER, CGV_MARK_TYPE) == NULL) {
            if (i == 1)
                ChainCone = addchain(ChainCone, Cone);

            Cone->USER = addptype(Cone->USER, CGV_MARK_TYPE, NULL);
        }

        for (CnsEdge = Cone->INCONE; CnsEdge; CnsEdge = CnsEdge->NEXT) {
            if ((CnsEdge->TYPE & CNS_EXT) == CNS_EXT) {
                if (CnsEdge->UEDGE.LOCON->FLAGS == 0)
                    loc_add_con_ext(CgvFigure, CnsEdge->UEDGE.LOCON);
            }
            else {
                if (Mode & CGV_SEL_PREC_LOGIC) {
                    if (getptype(CnsEdge->UEDGE.CONE->USER, CGV_MARK_TYPE) == NULL) {
                        ChainCone = cgvlistcone_rec(ChainCone, CnsEdge->UEDGE.CONE, CGV_SEL_PREC_LOGIC, Depth - i, CgvFigure, ptcell);
                    }
                }
            }
        }

        for (CnsEdge = Cone->OUTCONE; CnsEdge; CnsEdge = CnsEdge->NEXT) {
            if ((CnsEdge->TYPE & CNS_EXT) == CNS_EXT) {
                if (CnsEdge->UEDGE.LOCON->FLAGS == 0)
                    loc_add_con_ext(CgvFigure, CnsEdge->UEDGE.LOCON);
            }
            else {
                if (getptype(CnsEdge->UEDGE.CONE->USER, CGV_MARK_TYPE) == NULL) {
                    if (Mode & CGV_SEL_SUCC_LOGIC) {
                        ChainCone = cgvlistcone_rec(ChainCone, CnsEdge->UEDGE.CONE, CGV_SEL_SUCC_LOGIC, Depth - i, CgvFigure, ptcell);
                        if (i == 0)
                            ChainCone = cgvlistcone_rec(ChainCone, CnsEdge->UEDGE.CONE, CGV_SEL_PREC_LOGIC, 1, CgvFigure, ptcell);
                    }
                }
            }
        }
    }
    return (ChainCone);
}

/*------------------------------------------------------------\
|                          cgvlistcone                        |
\------------------------------------------------------------*/

chain_list     *
cgvlistcone(CnsFigure, NameList, Mode, Depth, CgvFigure)
    cnsfig_list    *CnsFigure;
    chain_list     *NameList;
    int             Mode;
    int             Depth;
    cgvfig_list    *CgvFigure;
{
    chain_list     *ScanNameList;
    chain_list     *ChainCone;
    cone_list      *Cone;

    ChainCone = NULL;

    for (ScanNameList = NameList; ScanNameList; ScanNameList = ScanNameList->NEXT) {
        if (Mode & CGV_FROM_CHAIN_NAME) {
            Cone = cgvsearchcone(CnsFigure, (char *) ScanNameList->DATA);
        }
        else {
            Cone = (cone_list *) ScanNameList->DATA;
        }

        if (Cone != NULL) {
            ChainCone = cgvlistcone_rec(NULL, Cone, Mode, Depth, CgvFigure, NULL);
        }
    }
    return (ChainCone);
}

#if 0
/*------------------------------------------------------------\
|                  loc_getcgvfig_from_cnsfig_sel              |
\------------------------------------------------------------*/

cgvfig_list    *
loc_getcgvfig_from_cnsfig_sel(CgvFigure, CnsFigure, NameList, Mode, Depth)
    cgvfig_list    *CgvFigure;
    cnsfig_list    *CnsFigure;
    chain_list     *NameList;
    int             Mode;
    int             Depth;
{
    chain_list     *ConeList;
    chain_list     *ScanChain;
    cone_list      *ScanCone;
    chain_list     *CellChain;


    ConeList = cgvlistcone(CnsFigure, NameList, Mode, Depth, CgvFigure);

    for (ScanChain = ConeList; ScanChain; ScanChain = ScanChain->NEXT) {
        ScanCone = (cone_list *) ScanChain->DATA;
        if (ScanCone->CELLS == NULL) {
            loc_add_cone(CgvFigure, ScanCone);
        }
        else {
            for (CellChain = ScanCone->CELLS; CellChain; CellChain = CellChain->NEXT) {
                if (CellChain != NULL)
                    loc_add_cell(CgvFigure, (cell_list *) CellChain->DATA);
            }
        }
    }

    for (ScanChain = ConeList; ScanChain; ScanChain = ScanChain->NEXT) {
        ScanCone = (cone_list *) ScanChain->DATA;
        if (ScanCone->CELLS == NULL)
            loc_add_conin(CgvFigure, ScanCone);
        else {
            for (CellChain = ScanCone->CELLS; CellChain; CellChain = CellChain->NEXT) {
                if (CellChain != NULL)
                    loc_add_conincell(CgvFigure, (cell_list *) CellChain->DATA);
            }
        }
    }

    patch_sel(CgvFigure);

    return (CgvFigure);
}

#endif
/*------------------------------------------------------------\
|                    loc_getcgvfig_from_cnsfig                |
\------------------------------------------------------------*/

cgvfig_list    *
loc_getcgvfig_from_cnsfig(CgvFigure, CnsFigure)
    cgvfig_list    *CgvFigure;
    cnsfig_list    *CnsFigure;
{
    locon_list     *LoCon;
    cone_list      *CnsCone;
    cell_list      *CnsCell;
    loins_list     *li;
    cgvbox_list    *CgvBox;
    cgvnet_list    *CgvNet;
    losig_list     *LoSig;
    unsigned char   CgvDir;
    cgvcon_list    *CgvCon;
    ptype_list     *PType;
    chain_list     *cl;
    long            counter = 0;
    long           *ptlong;

    //connecteurs externes +

        all_sigs = NULL;

    for (LoCon = CnsFigure->INTCON; LoCon; LoCon = LoCon->NEXT)
        LoCon->FLAGS = 1;

    for (LoCon = CnsFigure->LOCON; LoCon; LoCon = LoCon->NEXT) {
        loc_add_con_ext(CgvFigure, LoCon);
        LoCon->FLAGS = 0;
    }

    CgvFigure->CON_IN = (cgvcon_list *) reverse((chain_list *) CgvFigure->CON_IN);
    CgvFigure->CON_OUT = (cgvcon_list *) reverse((chain_list *) CgvFigure->CON_OUT);

    //box

        // box a partir des cones sans cells
        for (CnsCone = CnsFigure->CONE; CnsCone; CnsCone = CnsCone->NEXT) {
        if (CnsCone->CELLS == NULL || !CGV_MAKE_CELLS)
            loc_add_cone(CgvFigure, CnsCone);
    }

    //box a partir des cells
        if (CGV_MAKE_CELLS) {
        for (CnsCell = CnsFigure->CELL; CnsCell; CnsCell = CnsCell->NEXT) {
            counter++;
            if (getptype(CnsCell->USER, (long) CNS_INDEX) == NULL) {
                ptlong = (long *) mbkalloc(sizeof(long));
                *ptlong = counter;
                CnsCell->USER = addptype(CnsCell->USER, (long) CNS_INDEX, (void *) ptlong);
            }
            loc_add_cell(CgvFigure, CnsCell);
        }
    }

    //box a partir des cones sans cells
        for (CnsCone = CnsFigure->CONE; CnsCone; CnsCone = CnsCone->NEXT) {
        if (CnsCone->CELLS == NULL || !CGV_MAKE_CELLS)
            loc_add_conin(CgvFigure, CnsCone, CnsFigure);
    }

    for (li = CnsFigure->LOINS; li != NULL; li = li->NEXT) {
        char           *n;

        CgvBox = addcgvbox(CgvFigure, li->INSNAME);
        CgvBox->SOURCE = (void *) li;
        CgvBox->SOURCE_TYPE = CGV_SOURCE_LOINS;
        n = getcorrespgate(li->FIGNAME);
        //printf("%s is %s\n", li->FIGNAME, n != NULL ? n : "?");
        if (n == NULL)
            n = li->FIGNAME;
        CgvBox->SYMBOL = cgv_getlofigcellsymbol(CgvFigure->LIBRARY, n);
        if (CgvBox->SYMBOL == NULL)
            CgvBox->SYMBOL = cgv_getselfcellsymbol(CgvFigure->LIBRARY, n);
        //printf("i:%s\n", CgvBox->NAME);
        for (LoCon = li->LOCON; LoCon; LoCon = LoCon->NEXT) {
            if (!((mbk_LosigIsVDD(LoCon->SIG)) || (mbk_LosigIsVSS(LoCon->SIG)))) {
                if (LoCon->DIRECTION == IN)
                    CgvDir = CGV_CON_IN;
                else
                    CgvDir = CGV_CON_OUT;

                CgvCon = addcgvboxcon(CgvFigure, CgvBox, LoCon->NAME, CgvDir);
                CgvCon->SOURCE = (void *) LoCon;
                CgvCon->SOURCE_TYPE = CGV_SOURCE_LOCON;
                if (n != li->FIGNAME) {
                    if (CgvBox->SYMBOL)
                        CgvCon->SUB_NAME = getcorrespgatepin(li->FIGNAME, LoCon->NAME);
                    else
                        avt_errmsg(CGV_ERRMSG, "002", AVT_WARNING, 1);
                    //printf("warning: can find corresponding pin for %s.%s\n", li->FIGNAME, LoCon->NAME);
                }
                LoSig = LoCon->SIG;
                PType = getptype(LoSig->USER, CGV_NET_TYPE);
                if (PType == NULL) {
                    CgvNet = addcgvnet(CgvFigure);
                    CgvNet->SOURCE = (void *) LoSig;
                    CgvNet->SOURCE_TYPE = CGV_SOURCE_LOSIG;
                    if (LoSig->NAMECHAIN != NULL)
                        CgvNet->NAME = (char *) LoSig->NAMECHAIN->DATA;
                    else
                        CgvNet->NAME = namealloc("?");
                    LoSig->USER = addptype(LoSig->USER, CGV_NET_TYPE, CgvNet);
                }
                else
                    CgvNet = (cgvnet_list *) PType->DATA;
                addcgvnetcon(CgvNet, CgvCon);
                //printf("\t%s => %s | %s => %s\n", CgvCon->NAME, CgvNet->NAME, LoCon->NAME, LoSig->NAMECHAIN->DATA);
            }
        }
    }
    //box a partir des cells
        if (CGV_MAKE_CELLS) {
        for (CnsCell = CnsFigure->CELL; CnsCell; CnsCell = CnsCell->NEXT) {
            loc_add_conincell(CgvFigure, CnsCell);
        }
    }

    for (cl = all_sigs; cl != NULL; cl = cl->NEXT) {
        if (getptype(((losig_list *) cl->DATA)->USER, CGV_NET_TYPE) != NULL)
            ((losig_list *) cl->DATA)->USER = delptype(((losig_list *) cl->DATA)->USER, CGV_NET_TYPE);
    }
    freechain(all_sigs);
    return (CgvFigure);
}

/*------------------------------------------------------------\
|                         map cell                            |
\------------------------------------------------------------*/

char           *
cgv_mapcell(cell_list * ptcell)
{
    //extern char  *cgv_cellmapping_table[][3];
    unsigned int    i = 0;
    int             j = 0;

    while (i < sizeof(cgv_cellmapping_table) / sizeof(*cgv_cellmapping_table)
           && cgv_cellmapping_table[i][1] && ptcell != NULL) {
        if (ptcell->TYPE == atol(cgv_mapping_table[i][1])) {
            if (j == atoi(cgv_cellmapping_table[i][2]))
                return namealloc(cgv_cellmapping_table[i][0]);
            else
                return NULL;
        }
        i++;
    }
    return NULL;
}

/*------------------------------------------------------------\
|                         map cone                            |
\------------------------------------------------------------*/

char           *
cgv_mapcone(cone_list *ptcone)
{
    //extern char  *cgv_mapping_table[][3];
    ptype_list     *ptuser = NULL;
    edge_list      *ptin;
    unsigned int    i = 0;
    int             j = 0;

    ptin = ptcone->INCONE;

    for (; ptin; ptin = ptin->NEXT) {
        if ((ptin->TYPE & CNS_EXT) != 0) {
            if (getchain(getptype(ptcone->USER, CNS_EXT)->DATA, ptin->UEDGE.PTR) != NULL) continue;
        }
        j++;
    }

    ptuser = getptype(ptcone->USER, CNS_SIGNATURE);

    while (i < sizeof(cgv_mapping_table) / sizeof(*cgv_mapping_table)
           && cgv_mapping_table[i][1] && ptuser != NULL) {
        if (!strcasecmp(ptuser->DATA, cgv_mapping_table[i][1])) {
            if (j == atoi(cgv_mapping_table[i][2]))
                return namealloc(cgv_mapping_table[i][0]);
            else
                return NULL;
        }
        i++;
    }
    return NULL;
}

/*------------------------------------------------------------\
|         Get symbol name from cell and return symbol         |
\------------------------------------------------------------*/

symbol_list    *
cgv_getcellsymbol(ptlib, ptcell)
    library        *ptlib;
    cell_list      *ptcell;
{
    symbol_list    *ptsym;
    char           *symname;

    if (ptlib == NULL)
        return NULL;
    symname = cgv_mapcell(ptcell);

    if ((ptsym = slib_getsymbol(ptlib, symname)) == NULL) {
        if (SLIB_LIBRARY != NULL)
            ptsym = slib_get_flat_symbol(SLIB_LIBRARY, symname);
        if (ptsym == NULL)
            ptsym = slib_get_flat_symbol(CGV_LIB, symname);
        if (ptsym != NULL) {
            slib_put_symbol(ptlib, ptsym);
            ptsym->MOTIF = slib_motifconvert(ptsym);
        }
    }
    return ptsym;
}


/*------------------------------------------------------------\
|          Get symbol name from cone and return symbol        |
\------------------------------------------------------------*/

symbol_list    *
cgv_getcnssymbol(ptlib, ptcone)
    library        *ptlib;
    cone_list      *ptcone;
{
    symbol_list    *ptsym;
    char           *symname;

    if (ptlib == NULL)
        return NULL;
    symname = cgv_mapcone(ptcone);

    if ((ptsym = slib_getsymbol(ptlib, symname)) == NULL) {
        if (CGV_LIB == NULL)
            return NULL;
        ptsym = slib_get_flat_symbol(CGV_LIB, symname);
        if (ptsym != NULL) {
            slib_put_symbol(ptlib, ptsym);
            ptsym->MOTIF = slib_motifconvert(ptsym);
        }
    }
    return ptsym;
}

/*------------------------------------------------------------\
|                  Get Cgv Figure From cns fig                |
\------------------------------------------------------------*/

cgvfig_list    *
getcgvfig_from_cnsfig(char *FileName, char *filename)
{
    cnsfig_list    *CnsFigure, *oldhead0;
    cgvfig_list    *CgvFigure;
    long            scale;
    lofig_list     *oldhead;
    cnsfig_list    *temp0;
    ht             *oldht;

    /* if (CNS_HEADCNSFIG != NULL) { dellofig(CNS_HEADCNSFIG->NAME);
     * CNS_HEADCNSFIG = delcnsfig(CNS_HEADCNSFIG, CNS_HEADCNSFIG); } */
    CgvFigure = getcgvfile(FileName, CGV_SOURCE_CNSFIG, 0, filename);
    CGV_WAS_PRESENT = 1;
    if (CgvFigure != NULL)
        return CgvFigure;

    CGV_WAS_PRESENT = 0;
    if (filename != NULL)
        temp0 = NULL;
    else
        temp0 = getloadedcnsfig(FileName);
    if (temp0 == NULL) {
        if (filepath(FileName, "cns") == NULL)
            return NULL;
        oldhead = HEAD_LOFIG;
        oldht = HT_LOFIG;
        HEAD_LOFIG = NULL;
        HT_LOFIG = NULL;
        oldhead0 = CNS_HEADCNSFIG;
        CNS_HEADCNSFIG = NULL;
        CnsFigure = getcnsfig(FileName, NULL);
    }
    else
        CnsFigure = temp0;

    if (CGV_LIB || SLIB_LIBRARY) {
        if (SLIB_LIBRARY)
            scale = SLIB_LIBRARY->GRID;
        else
            scale = CGV_LIB->GRID;
    }
    else
        scale = 1024;

    //printf("scale=%d\n", scale);

    CgvFigure = addcgvfig(CnsFigure->NAME, scale);
    CgvFigure->SOURCE = CnsFigure;
    CgvFigure->SOURCE_TYPE = CGV_SOURCE_CNSFIG;
    CgvFigure->data0 = CgvFigure->data1 = CgvFigure->data2 = CgvFigure->data_ZOOM = NULL;

    if (CGV_LIB || SLIB_LIBRARY) {
        if (SLIB_LIBRARY) {
            CgvFigure->LIBRARY = slib_create_lib(CnsFigure->NAME);
            CgvFigure->LIBRARY->REFHEIGHT = SLIB_LIBRARY->REFHEIGHT;
        }
        else {
            CgvFigure->LIBRARY = slib_create_lib(CnsFigure->NAME);
            CgvFigure->LIBRARY->REFHEIGHT = CGV_LIB->REFHEIGHT;
        }
    }

    CgvFigure->LIBRARY->GRID = scale;


    CgvFigure = loc_getcgvfig_from_cnsfig(CgvFigure, CnsFigure);

    addcgvfile(CnsFigure->NAME, CGV_SOURCE_CNSFIG, 0, filename, CgvFigure, HEAD_LOFIG, CnsFigure, temp0 != NULL);
    if (temp0 == NULL) {
        delht(HT_LOFIG);
        HEAD_LOFIG = oldhead;
        HT_LOFIG = oldht;
        CNS_HEADCNSFIG = oldhead0;
    }

    buildcgvfig(CgvFigure);
    strcpy(getcgvfileext(CgvFigure), "cns");
    return CgvFigure;
}

/*------------------------------------------------------------\
|                                                             |
| Tool    :                  XYAG XME                         |
|                                                             |
| File    :                   Beh.c                           |
|                                                             |
| Authors :              Picault Stephane                     |
|                        Lester Anthony                       |
|                                                             |
\------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

# include <Xm/Xm.h>
# include <Xm/Form.h>
# include <Xm/Frame.h>
# include <Xm/DrawingA.h>

#include MUT_H
#include MLO_H
#include MLU_H
#include LOG_H
#include BEH_H
#include BHL_H
#include BVL_H
#include XSB_H
#include XTB_H
#include XMX_H

#include "XME_beh.h"

befig_list     *XyagEditBehBefig;
char           *XyagTempBehFile;

/*------------------------------------------------------------\
|                      XyagBehToCns                           |
\------------------------------------------------------------*/

char *
XyagBehToCns(name)
    char           *name;
{
    int             i = 0;
    int             j = 0;
    char            NewName[255];

    while (name[i] != '\0') {
        if ((name[i] == '.') || (name[i] == '[')) {
            NewName[j++] = '_';
        }
        else {
            if ((name[i] != ']')) {
                NewName[j++] = name[i];
            }
        }
        i++;
    }
    NewName[j] = '\0';

    return (namealloc(NewName));
}

/*------------------------------------------------------------\
|                 Xyag  Search  BeAux  by Name                |
\------------------------------------------------------------*/

static beaux_list *
XyagSearchBeAux(beh, name)
    befig_list     *beh;
    char           *name;
{
    beaux_list     *ScanBeAux;

    for (ScanBeAux = beh->BEAUX; ScanBeAux; ScanBeAux = ScanBeAux->NEXT) {
        if (name == ScanBeAux->NAME) break;
    }
    return ScanBeAux;
}

/*------------------------------------------------------------\
|                 Xyag  Search  BePor  by name                |
\------------------------------------------------------------*/

static bepor_list *
XyagSearchBePor(beh, name)
    befig_list     *beh;
    char           *name;
{
    bepor_list     *ScanBePor;

    for (ScanBePor = beh->BEPOR; ScanBePor; ScanBePor = ScanBePor->NEXT) {
        if (name == ScanBePor->NAME) break;
    }
    return ScanBePor;
}

/*------------------------------------------------------------\
|                 Xyag  Search  BeOut  by name                |
\------------------------------------------------------------*/

static beout_list *
XyagSearchBeOut(beh, name)
    befig_list     *beh;
    char           *name;
{

    beout_list     *ScanBeOut;

    for (ScanBeOut = beh->BEOUT; ScanBeOut; ScanBeOut = ScanBeOut->NEXT) {
        if (name == ScanBeOut->NAME) break;
    }
    return ScanBeOut;
}

/*------------------------------------------------------------\
|                 Xyag  Search  BeReg  by name                |
\------------------------------------------------------------*/

static bereg_list *
XyagSearchBeReg(beh, name)
    befig_list     *beh;
    char           *name;
{
    bereg_list     *ScanBeReg;

    for (ScanBeReg = beh->BEREG; ScanBeReg; ScanBeReg = ScanBeReg->NEXT) {
        if (name == ScanBeReg->NAME) break;
    }
    return ScanBeReg;
}

/*------------------------------------------------------------\
|                 Xyag  Search  BeBus  by name                |
\------------------------------------------------------------*/

static bebus_list *
XyagSearchBeBus(beh, name)
    befig_list     *beh;
    char           *name;
{
    bebus_list     *ScanBeBus;

    for (ScanBeBus = beh->BEBUS; ScanBeBus; ScanBeBus = ScanBeBus->NEXT) {
        if (name == ScanBeBus->NAME) break;
    }
    return ScanBeBus;
}

/*------------------------------------------------------------\
|                 Xyag  Search  BeBux  by name                |
\------------------------------------------------------------*/

static bebux_list *
XyagSearchBeBux(beh, name)
    befig_list     *beh;
    char           *name;
{
    bebux_list     *ScanBeBux;

    for (ScanBeBux = beh->BEBUX; ScanBeBux; ScanBeBux = ScanBeBux->NEXT) {
        if (name == ScanBeBux->NAME) break;
    }
    return ScanBeBux;
}

/*------------------------------------------------------------\
|                       XyagEditBehDisplayBeStuff             |
\------------------------------------------------------------*/

void 
XyagEditBehDisplayBeStuff(name)
    char           *name;
{
    FILE           *File;
    bepor_list     *BePor;
    beaux_list     *BeAux;
    beout_list     *BeOut;
    bereg_list     *BeReg;
    bebus_list     *BeBus;
    bebux_list     *BeBux;
    char            Buffer[512];
    char           *modname;
    int             found = 0;

    int             NumProcess = 0;

    modname = XyagBehToCns(name);
    settimeunit(XyagEditBehBefig->TIME_UNIT);

    sprintf(Buffer, "/tmp/xyag_%d.tmp", (int)getpid());
    XyagTempBehFile = namealloc(Buffer);
    File = fopen(XyagTempBehFile, "w");

    if (File == NULL) {
        fprintf(stdout, "%s\n", strerror(errno));
        XyagErrorMessage(XyagMainWindow, "Cannot open temporary file");
        return;
    }

    fprintf(File, "\n-- %s\n\n\n", name);

    if (!found) {
        BeAux = XyagSearchBeAux(XyagEditBehBefig, modname);
        if (BeAux != NULL) {
            print_one_beaux(File, BeAux);
            found = 1;
        }
    }
    if (!found) {
        BeOut = XyagSearchBeOut(XyagEditBehBefig, modname);
        if (BeOut != NULL) {
            print_one_beout(File, BeOut);
            found = 1;
        }
    }
    if (!found) {
        BeReg = XyagSearchBeReg(XyagEditBehBefig, modname);
        if (BeReg != NULL) {
            print_one_bereg(File, BeReg, &NumProcess);
            found = 1;
        }
    }
    if (!found) {
        BeBus = XyagSearchBeBus(XyagEditBehBefig, modname);
        if (BeBus != NULL) {
            print_one_bebus(File, BeBus, &NumProcess);
            found = 1;
        }
    }
    if (!found) {
        BeBux = XyagSearchBeBux(XyagEditBehBefig, modname);
        if (BeBux != NULL) {
            print_one_bebux(File, BeBux, &NumProcess);
            found = 1;
        }
    }
    if (!found) {
        BeAux = XyagSearchBeAux(XyagEditBehBefig, name);
        if (BeAux != NULL) {
            print_one_beaux(File, BeAux);
            found = 1;
        }
    }
    if (!found) {
        BeOut = XyagSearchBeOut(XyagEditBehBefig, name);
        if (BeOut != NULL) {
            print_one_beout(File, BeOut);
            found = 1;
        }
    }
    if (!found) {
        BeReg = XyagSearchBeReg(XyagEditBehBefig, name);
        if (BeReg != NULL) {
            print_one_bereg(File, BeReg, &NumProcess);
            found = 1;
        }
    }
    if (!found) {
        BeBus = XyagSearchBeBus(XyagEditBehBefig, name);
        if (BeBus != NULL) {
            print_one_bebus(File, BeBus, &NumProcess);
            found = 1;
        }
    }
    if (!found) {
        BeBux = XyagSearchBeBux(XyagEditBehBefig, name);
        if (BeBux != NULL) {
            print_one_bebux(File, BeBux, &NumProcess);
            found = 1;
        }
    }

    fclose(File);
}

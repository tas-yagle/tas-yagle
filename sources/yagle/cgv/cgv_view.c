/*------------------------------------------------------------\
|                                                             |
| Tool    :                     Cgv                           |
|                                                             |
| File    :                 cgv_view.c                        |
|                                                             |
| Date    :                   11.03.97                        |
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
|                        Cgv View Wire                        |
\------------------------------------------------------------*/

void 
viewcgvwir(Wire)
    cgvwir_list    *Wire;
{
    fprintf(stdout, "\n\t\t--> Wire");
    fprintf(stdout, "\n\t\t\tX : %ld", Wire->X);
    fprintf(stdout, "\n\t\t\tY : %ld", Wire->Y);
    fprintf(stdout, "\n\t\t\tDX : %ld", Wire->DX);
    fprintf(stdout, "\n\t\t\tDX : %ld", Wire->DY);
    fprintf(stdout, "\n\t\t\tFLAGS : %lx", Wire->FLAGS);
    fprintf(stdout, "\n\t\t\tUSER  : %lx", (long) Wire->USER);
    fprintf(stdout, "\n\t\t<-- Wire");
}

/*------------------------------------------------------------\
|                        Cgv View Net                         |
/------------------------------------------------------------*/

void 
viewcgvnet(Net)
    cgvnet_list    *Net;
{
    cgvwir_list    *ScanWire;
    cgvcon_list    *ScanCon;
    chain_list     *ScanChain;
    char           *RootName;

    fprintf(stdout, "\n\t--> Net");
    fprintf(stdout, "\n\t\tNUMBER_IN   : %ld", Net->NUMBER_IN);
    fprintf(stdout, "\n\t\tNUMBER_OUT  : %ld", Net->NUMBER_OUT);
    fprintf(stdout, "\n\t\tFLAGS : %lx", Net->FLAGS);

    fprintf(stdout, "\n\t\tSOURCE_TYPE : %d", (int) Net->SOURCE_TYPE);

    fprintf(stdout, "\n\t\tUSER        : %lx", (long) Net->USER);

    fprintf(stdout, "\n\t\tCON_NET     :");

    for (ScanChain = Net->CON_NET;
         ScanChain != (chain_list *) 0;
         ScanChain = ScanChain->NEXT) {
        ScanCon = (cgvcon_list *) ScanChain->DATA;

        if (ScanCon->ROOT_TYPE == CGV_ROOT_CON_FIG) {
            RootName = ((cgvfig_list *) ScanCon->ROOT)->NAME;
            fprintf(stdout, "\n\t\t\t|_\t%s of fig %s", ScanCon->NAME, RootName);
        }
        else {
            RootName = ((cgvbox_list *) ScanCon->ROOT)->NAME;
            fprintf(stdout, "\n\t\t\t|_\t%s of box %s", ScanCon->NAME, RootName);
        }
    }

    fprintf(stdout, "\n\t\tWIRE :");

    for (ScanWire = Net->WIRE;
         ScanWire != (cgvwir_list *) 0;
         ScanWire = ScanWire->NEXT) {
        viewcgvwir(ScanWire);
    }

    fprintf(stdout, "\n\t<-- Net");
}

/*------------------------------------------------------------\
|                        Cgv View Connector                   |
\------------------------------------------------------------*/

void 
viewcgvcon(Con, Blank)
    cgvcon_list    *Con;
    char           *Blank;
{
    fprintf(stdout, "\n%s--> Con", Blank);
    fprintf(stdout, "\n%s\tNAME        : %s", Blank, Con->NAME);
    fprintf(stdout, "\n%s\tFLAGS       : %lx", Blank, Con->FLAGS);
    fprintf(stdout, "\n%s\tSOURCE_TYPE : %d", Blank, (int) Con->SOURCE_TYPE);
    fprintf(stdout, "\n%s\tUSER        : %lx", Blank, (long) Con->USER);
    fprintf(stdout, "\n%s\tDIR         : %d", Blank, (int) Con->DIR);
    fprintf(stdout, "\n%s\tTYPE        : Ox%x", Blank, (int) Con->TYPE);
    fprintf(stdout, "\n%s\tROOT_TYPE   : %d", Blank, (int) Con->ROOT_TYPE);
    fprintf(stdout, "\n%s\tX_REL       : %ld", Blank, Con->X_REL);
    fprintf(stdout, "\n%s\tY_REL       : %ld", Blank, Con->Y_REL);
    fprintf(stdout, "\n%s<-- Con", Blank);
}

/*------------------------------------------------------------\
|                     Cgv View Box                            |
\------------------------------------------------------------*/

void 
viewcgvbox(Box)
    cgvbox_list    *Box;
{
    cgvcon_list    *ScanCon;

    fprintf(stdout, "\n\t--> Box");
    fprintf(stdout, "\n\t\tNAME        : %s", Box->NAME);
    fprintf(stdout, "\n\t\tFLAGS       : %lx", Box->FLAGS);
    fprintf(stdout, "\n\t\tSOURCE_TYPE : %d", (int) Box->SOURCE_TYPE);
    fprintf(stdout, "\n\t\tUSER        : %lx", (long) Box->USER);
    fprintf(stdout, "\n\t\tNUMBER_IN   : %ld", Box->NUMBER_IN);
    fprintf(stdout, "\n\t\tNUMBER_OUT  : %ld", Box->NUMBER_OUT);
    fprintf(stdout, "\n\t\tX           : %ld", Box->X);
    fprintf(stdout, "\n\t\tY           : %ld", Box->Y);
    fprintf(stdout, "\n\t\tDX          : %ld", Box->DX);
    fprintf(stdout, "\n\t\tDY          : %ld", Box->DY);

    fprintf(stdout, "\n\t\tCON_IN      : ");

    for (ScanCon = Box->CON_IN;
         ScanCon != (cgvcon_list *) 0;
         ScanCon = ScanCon->NEXT) {
        viewcgvcon(ScanCon, "\t\t");
    }

    fprintf(stdout, "\n\t\tCON_OUT     : ");

    for (ScanCon = Box->CON_OUT;
         ScanCon != (cgvcon_list *) 0;
         ScanCon = ScanCon->NEXT) {
        viewcgvcon(ScanCon, "\t\t");
    }

    fprintf(stdout, "\n\t<-- Box");
}

/*------------------------------------------------------------\
|                        Cgv View Figure                      |
\------------------------------------------------------------*/

void 
viewcgvfig(Figure)
    cgvfig_list    *Figure;
{
    cgvcon_list    *ScanCon;
    cgvbox_list    *ScanBox;
    cgvnet_list    *ScanNet;

    fprintf(stdout, "\n--> Figure");
    fprintf(stdout, "\nNAME        : %s", Figure->NAME);

    fprintf(stdout, "\nFLAGS       : %lx", Figure->FLAGS);
    fprintf(stdout, "\nSOURCE_TYPE : %d", (int) Figure->SOURCE_TYPE);
    fprintf(stdout, "\nUSER        : %lx", (long) Figure->USER);
    fprintf(stdout, "\nNUMBER_IN   : %ld", Figure->NUMBER_IN);
    fprintf(stdout, "\nNUMBER_OUT  : %ld", Figure->NUMBER_OUT);
    fprintf(stdout, "\nX           : %ld", Figure->X);
    fprintf(stdout, "\nY           : %ld", Figure->Y);

    fprintf(stdout, "\nCON_IN      : ");

    for (ScanCon = Figure->CON_IN;
         ScanCon != (cgvcon_list *) 0;
         ScanCon = ScanCon->NEXT) {
        viewcgvcon(ScanCon, "\t");
    }

    fprintf(stdout, "\nCON_OUT     : ");

    for (ScanCon = Figure->CON_OUT;
         ScanCon != (cgvcon_list *) 0;
         ScanCon = ScanCon->NEXT) {
        viewcgvcon(ScanCon, "\t");
    }

    fprintf(stdout, "\nBOX         : ");

    for (ScanBox = Figure->BOX;
         ScanBox != (cgvbox_list *) 0;
         ScanBox = ScanBox->NEXT) {
        viewcgvbox(ScanBox);
    }

    fprintf(stdout, "\nNET         : ");

    for (ScanNet = Figure->NET;
         ScanNet != (cgvnet_list *) 0;
         ScanNet = ScanNet->NEXT) {
        viewcgvnet(ScanNet);
    }

    fprintf(stdout, "\n<-- Figure\n");
}

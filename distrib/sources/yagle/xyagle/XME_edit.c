#include <stdio.h>
#include <math.h>
#include <Xm/Xm.h>
#include <Xm/PushBG.h>

#include MUT_H
#include MLO_H
#include CNS_H
#include CGV_H
#include XSB_H
#include XTB_H
#include XMX_H
#include XYA_H
#include XME_H
#include XMV_H

#include "XME_edit.h"
#include "XME_beh.h"
#include "XME_select.h"
#include "XME_panel.h"

static char     XyagIdentifyMessage[XYAG_IDENTIFY_MESSAGE_SIZE];
static char     XyagIdentifyBuffer[XYAG_IDENTIFY_BUFFER_SIZE];

static char    *XyagScanIdentify;
static long     XyagIdentifyLength;

static void XyagIdentifyBranch();

/*------------------------------------------------------------\
|                       Behaviour Buffer                      |
\------------------------------------------------------------*/

static char     XyagBehMessage[XYAG_BEH_MESSAGE_SIZE];
static char     XyagBehBuffer[XYAG_BEH_BUFFER_SIZE];

static char    *XyagScanBeh;
static long     XyagBehLength;

/*------------------------------------------------------------\
|                         XyagAddBeh                          |
\------------------------------------------------------------*/

short XyagAddBeh(Obj)

   xyagobj_list   *Obj;
{
   FILE           *File;
   long            len;
   long            Length;
   char           *Name;
   char            Buffer[512];
   cgvbox_list    *CgvBox;
   cgvnet_list    *CgvNet;
   cgvcon_list    *CgvCon;


   XyagBehBuffer[0] = '\0';

   if (IsXyagCgvBox(Obj))
   {
      CgvBox = (cgvbox_list *)Obj->USER;

      if (CgvBox->SOURCE_TYPE == CGV_SOURCE_CNSCONE)
      {
         Name = ((cone_list *)(CgvBox->SOURCE))->NAME;
         XyagEditBehDisplayBeStuff( Name );
      }
   }
   if (IsXyagCgvNet(Obj))
   {
      CgvNet = (cgvnet_list *)Obj->USER;

      if (CgvNet->SOURCE_TYPE == CGV_SOURCE_CNSCONE)
      {
         Name = ((cone_list *)(CgvNet->SOURCE))->NAME;
         XyagEditBehDisplayBeStuff( Name );
      }
   }
   if (IsXyagCgvCon(Obj))
   {
      CgvCon = (cgvcon_list *)Obj->USER;

      if (CgvCon->SOURCE_TYPE == CGV_SOURCE_LOCON)
      { 
         Name = ((locon_list *)(CgvCon->SOURCE))->NAME;
         XyagEditBehDisplayBeStuff( Name );
      }
   }

   File = fopen( XyagTempBehFile, "r" );

   if ( File != (FILE *)0 )
   {
      while(  ( len = read( fileno( File), Buffer, 512 ) ) > 0)
      {
         strncat( XyagBehBuffer, Buffer, len );
      }
      fclose( File );
   }
 
   Length = strlen(XyagBehBuffer);

   if ((XyagBehLength + Length) < (XYAG_BEH_MESSAGE_SIZE - 1))
   {
      strcpy(XyagScanBeh, XyagBehBuffer);

      XyagScanBeh += Length;
      XyagBehLength += Length;
      return (XYAG_TRUE);
   }

   return( XYAG_FALSE );
}


/*------------------------------------------------------------\
|                        XyagEditBehaviour                    |
\------------------------------------------------------------*/

void XyagEditBehaviour( X1, Y1)

   long     X1;
   long     Y1;
{
   xyagselect_list *Select;
   
   if ( XyagEditBehBefig == NULL )
   {
      XyagWarningMessage( XyagMainWindow, "Behaviour Description can't be found..." );
      return;
   }
   
   strcpy( XyagBehMessage, "No element found");

   XyagScanBeh = XyagBehMessage;
   XyagBehLength = 0;

   XyagEditSelectPoint( X1, Y1);
   
   for ( Select = XyagHeadSelect;
	      Select != (xyagselect_list *)0;
	      Select = Select->NEXT)
   {
	   if (!XyagAddBeh(Select->OBJECT))
	      break;
   }

   XyagDelSelect();

   XyagDisplayEditBeh(XyagBehMessage);
   XyagEnterPanel(&XyagEditBehPanel); 

}

/*============================================================================*
 | function XyagIdentifyCone();                                               |
 *============================================================================*/
void 
XyagIdentifyCone(ptcone0)
    cone_list      *ptcone0;
{
    long            counter;
    locon_list     *ptlocon;
    char            cone_type[100];
    char            cone_tectype[100];
    char            edgetype[100];
    edge_list      *ptedge;
    cone_list      *ptcone1;
    branch_list    *ptbranch;
    ptype_list     *ptuser;
    char            Buffer[1024];

    *XyagIdentifyBuffer = '\0';
    strcat(XyagIdentifyBuffer, "Gate structure\n\n");
    sprintf(Buffer, "Name:    %s\n", ptcone0->NAME);
    strcat(XyagIdentifyBuffer, Buffer);
    sprintf(Buffer, "Type:    ");
    strcat(XyagIdentifyBuffer, Buffer);
    strcpy(cone_type, "");
    if ((ptcone0->TYPE & CNS_MEMSYM) == CNS_MEMSYM)
        strcat(cone_type, "MemSym,");
    if ((ptcone0->TYPE & CNS_LATCH) == CNS_LATCH)
        strcat(cone_type, "Latch,");
    if ((ptcone0->TYPE & CNS_RS) == CNS_RS)
        strcat(cone_type, "RS,");
    if ((ptcone0->TYPE & CNS_FLIP_FLOP) == CNS_FLIP_FLOP)
        strcat(cone_type, "Flip-Flop,");
    if ((ptcone0->TYPE & CNS_MASTER) == CNS_MASTER)
        strcat(cone_type, "Master,");
    if ((ptcone0->TYPE & CNS_SLAVE) == CNS_SLAVE)
        strcat(cone_type, "Slave,");
    if ((ptcone0->TYPE & CNS_TRI) == CNS_TRI)
        strcat(cone_type, "Tristate,");
    if ((ptcone0->TYPE & CNS_CONFLICT) == CNS_CONFLICT)
        strcat(cone_type, "Conflict,");
    if ((ptcone0->TYPE & CNS_EXT) == CNS_EXT)
        strcat(cone_type, "External,");
    if ((ptcone0->TYPE & CNS_VDD) == CNS_VDD)
        strcat(cone_type, "Vdd,");
    if ((ptcone0->TYPE & CNS_GND) == CNS_GND)
        strcat(cone_type, "Gnd,");
    if ((ptcone0->TYPE & CNS_VSS) == CNS_VSS)
        strcat(cone_type, "Vss,");

    if (strlen(cone_type) > 0) cone_type[strlen(cone_type)-1] = '\0';
    sprintf(Buffer, "%s\n", cone_type);
    strcat(XyagIdentifyBuffer, Buffer);

    strcat(XyagIdentifyBuffer, "TecType: ");
    strcpy(cone_tectype, "");
    if ((ptcone0->TECTYPE & CNS_DUAL_CMOS) == CNS_DUAL_CMOS)
        strcat(cone_tectype, "CMOS dual,");
    else if ((ptcone0->TECTYPE & CNS_CMOS) == CNS_CMOS)
        strcat(cone_tectype, "CMOS,");
    if ((ptcone0->TECTYPE & CNS_VDD_DEGRADED) == CNS_VDD_DEGRADED)
        strcat(cone_tectype, "Degraded Vdd,");
    if ((ptcone0->TECTYPE & CNS_GND_DEGRADED) == CNS_GND_DEGRADED)
        strcat(cone_tectype, "Degraded Gnd,");
    if ((ptcone0->TECTYPE & CNS_VSS_DEGRADED) == CNS_VSS_DEGRADED)
        strcat(cone_tectype, "Degraded Vss,");

    if (strlen(cone_tectype) > 0) cone_tectype[strlen(cone_tectype)-1] = '\0';
    sprintf(Buffer, "%s\n\n", cone_tectype);
    strcat(XyagIdentifyBuffer, Buffer);

    /* ------------------------------------------------------------------------
       cone inputs
       ------------------------------------------------------------------------ */
    strcat(XyagIdentifyBuffer, "INPUTS:\n");
    for (ptedge = ptcone0->INCONE; ptedge != NULL; ptedge = ptedge->NEXT) {
        if ((ptedge->TYPE & CNS_CONE) == CNS_CONE) {
            ptcone1 = ptedge->UEDGE.CONE;
            if (ptcone1 != NULL) {
                sprintf(Buffer, "    %s", ptcone1->NAME);
                strcat(XyagIdentifyBuffer, Buffer);
                strcpy(edgetype, "");
                if ((ptedge->TYPE & CNS_COMMAND) == CNS_COMMAND) {
                    strcat(edgetype, "Command ");
                }
                if ((ptedge->TYPE & CNS_BLEEDER) == CNS_BLEEDER) {
                    strcat(edgetype, "Bleeder ");
                }
                if ((ptedge->TYPE & CNS_FEEDBACK) == CNS_FEEDBACK) {
                    strcat(edgetype, "Feedback ");
                }
                if ((ptedge->TYPE & CNS_HZCOM) == CNS_HZCOM) {
                    strcat(edgetype, "HZ_Command ");
                }
                if ((ptedge->TYPE & CNS_LOOP) == CNS_LOOP) {
                    strcat(edgetype, "Loop ");
                }
                if (strlen(edgetype) > 0) {
                    edgetype[strlen(edgetype)-1] = '\0';
                    sprintf(Buffer, " (%s)", edgetype);
                    strcat(XyagIdentifyBuffer, Buffer);
                }
                strcat(XyagIdentifyBuffer, "\n");
            }
        }
        else if ((ptedge->TYPE & CNS_VDD) == CNS_VDD) {
            ptcone1 = ptedge->UEDGE.CONE;
            if (ptcone1 != NULL) {
                sprintf(Buffer, "    %s (Vdd)\n", ptcone1->NAME);
                strcat(XyagIdentifyBuffer, Buffer);
            }
        }
        else if ((ptedge->TYPE & CNS_GND) == CNS_GND) {
            ptcone1 = ptedge->UEDGE.CONE;
            if (ptcone1 != NULL) {
                sprintf(Buffer, "    %s (Gnd)\n", ptcone1->NAME);
                strcat(XyagIdentifyBuffer, Buffer);
            }
        }
        else if ((ptedge->TYPE & CNS_VSS) == CNS_VSS) {
            ptcone1 = ptedge->UEDGE.CONE;
            if (ptcone1 != NULL) {
                sprintf(Buffer, "    %s (Vss)\n", ptcone1->NAME);
                strcat(XyagIdentifyBuffer, Buffer);
            }
        }
        else if ((ptedge->TYPE & CNS_EXT) == CNS_EXT) {
            ptlocon = ptedge->UEDGE.LOCON;
            if (ptlocon != NULL) {
                sprintf(Buffer, "    %s (External)\n", ptlocon->NAME);
                strcat(XyagIdentifyBuffer, Buffer);
            }
        }
    }

    /* ------------------------------------------------------------------------
       cone outputs
       ------------------------------------------------------------------------ */
    strcat(XyagIdentifyBuffer, "OUTPUTS:\n");
    for (ptedge = ptcone0->OUTCONE; ptedge != NULL; ptedge = ptedge->NEXT) {
        if ((ptedge->TYPE & CNS_CONE) == CNS_CONE) {
            ptcone1 = ptedge->UEDGE.CONE;
            if (ptcone1 != NULL) {
                sprintf(Buffer, "    %s", ptcone1->NAME);
                strcat(XyagIdentifyBuffer, Buffer);
                strcpy(edgetype, "");
                if ((ptedge->TYPE & CNS_COMMAND) == CNS_COMMAND) {
                    strcat(edgetype, "Command ");
                }
                if ((ptedge->TYPE & CNS_BLEEDER) == CNS_BLEEDER) {
                    strcat(edgetype, "Bleeder ");
                }
                if ((ptedge->TYPE & CNS_FEEDBACK) == CNS_FEEDBACK) {
                    strcat(edgetype, "Feedback ");
                }
                if ((ptedge->TYPE & CNS_LOOP) == CNS_LOOP) {
                    strcat(edgetype, "Loop ");
                }
                if (strlen(edgetype) > 0) {
                    edgetype[strlen(edgetype)-1] = '\0';
                    sprintf(Buffer, " (%s)", edgetype);
                    strcat(XyagIdentifyBuffer, Buffer);
                }
                strcat(XyagIdentifyBuffer, "\n");
            }
        }
        else if ((ptedge->TYPE & CNS_EXT) == CNS_EXT) {
            ptlocon = ptedge->UEDGE.LOCON;
            if (ptlocon != NULL) {
                sprintf(Buffer, "    %s (External)\n", ptlocon->NAME);
                strcat(XyagIdentifyBuffer, Buffer);
            }
        }
    }

    /* ------------------------------------------------------------------------
       cone branches
       ------------------------------------------------------------------------ */
    strcat(XyagIdentifyBuffer, "\nBRANCHES:\n");
    counter = 0;
    for (ptbranch = ptcone0->BRVDD; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
        XyagIdentifyBranch(ptbranch, ++counter);
    }
    for (ptbranch = ptcone0->BRGND; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
        XyagIdentifyBranch(ptbranch, ++counter);
    }
    for (ptbranch = ptcone0->BRVSS; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
        XyagIdentifyBranch(ptbranch, ++counter);
    }
    for (ptbranch = ptcone0->BREXT; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
        XyagIdentifyBranch(ptbranch, ++counter);
    }

    /* ------------------------------------------------------------------------
       cone signature
       ------------------------------------------------------------------------ */
    if ((ptuser = getptype(ptcone0->USER, CNS_SIGNATURE)) != NULL) {
        sprintf(Buffer, "\nSIGNATURE: %s\n", (char *)ptuser->DATA);
        strcat(XyagIdentifyBuffer, Buffer);
    }
}


/*============================================================================*
 | function XyagIdentifyBranch();                                                        |
 *============================================================================*/
static void 
XyagIdentifyBranch(ptbranch, counter)
    branch_list    *ptbranch;
    long            counter;

{
    long           *ptlong;
    char            branch_type[100];
    char            lotrs_type[100];
    link_list      *ptlink = NULL;
    lotrs_list     *ptlotrs = NULL;
    ptype_list     *ptptype = NULL;
    locon_list     *ptlocon = NULL;
    ptype_list     *ptuser;
    char            Buffer[255];

    if (ptbranch->LINK != NULL) {
        ptlong = (long *) mbkalloc(sizeof(long));
        *ptlong = counter;
        if ((ptptype = getptype(ptbranch->USER, (long) CNS_INDEX)) != NULL) {
            ptptype->DATA = (void *) ptlong;
        }
        else {
            ptbranch->USER = addptype(ptbranch->USER, (long) CNS_INDEX, (void *) ptlong);
        }
        sprintf(Buffer, "  %ld) ", counter);
        strcat(XyagIdentifyBuffer, Buffer);

        if ((ptbranch->TYPE & CNS_VDD) == CNS_VDD)
            strcat(XyagIdentifyBuffer, "Vdd");
        else if ((ptbranch->TYPE & CNS_GND) == CNS_GND)
            strcat(XyagIdentifyBuffer, "Gnd");
        else if ((ptbranch->TYPE & CNS_VSS) == CNS_VSS)
            strcat(XyagIdentifyBuffer, "Vss");
        else if ((ptbranch->TYPE & CNS_EXT) == CNS_EXT)
            strcat(XyagIdentifyBuffer, "Ext");

        strcpy(branch_type, "");
        if ((ptbranch->TYPE & CNS_PARALLEL_INS) == CNS_PARALLEL_INS)
            strcat(branch_type, "Parallel instance,");
        else if ((ptbranch->TYPE & CNS_PARALLEL) == CNS_PARALLEL)
            strcat(branch_type, "Parallel,");
        if ((ptbranch->TYPE & CNS_DEGRADED) == CNS_DEGRADED)
            strcat(branch_type, "Degraded,");
        if ((ptbranch->TYPE & CNS_NOT_FUNCTIONAL) == CNS_NOT_FUNCTIONAL)
            strcat(branch_type, "Non-Functional,");
        if ((ptbranch->TYPE & CNS_BLEEDER) == CNS_BLEEDER)
            strcat(branch_type, "Bleeder,");
        if ((ptbranch->TYPE & CNS_FEEDBACK) == CNS_FEEDBACK)
            strcat(branch_type, "Feedback,");

        if (strlen(branch_type) > 0) {
            branch_type[strlen(branch_type)-1] = '\0';
            sprintf(Buffer, " (%s)", branch_type);
            strcat(XyagIdentifyBuffer, Buffer);
        }
        strcat(XyagIdentifyBuffer, "\n");
        for (ptlink = ptbranch->LINK; ptlink != NULL; ptlink = ptlink->NEXT) {
            if ((ptlink->TYPE & CNS_EXT) != CNS_EXT) {
                ptlotrs = ptlink->ULINK.LOTRS;
                if (ptlotrs != NULL) {
                    if (MLO_IS_TRANSN(ptlotrs->TYPE)) {
                        strcat(XyagIdentifyBuffer, "    TN ");
                    }
                    else if (MLO_IS_TRANSP(ptlotrs->TYPE)) {
                        strcat(XyagIdentifyBuffer, "    TP ");
                    }
                    if (ptlotrs->TRNAME != NULL) {
                        if (isdigit((int)*(ptlotrs->TRNAME))) sprintf(Buffer, "tr_%s ", ptlotrs->TRNAME);
                        else sprintf(Buffer, "%s ", ptlotrs->TRNAME);
                        strcat(XyagIdentifyBuffer, Buffer);
                    }
                    strcpy(lotrs_type, "");
                    if ((ptlink->TYPE & CNS_DIODE_UP) == CNS_DIODE_UP)
                        strcat(lotrs_type, "Diode Up,");
                    if ((ptlink->TYPE & CNS_DIODE_DOWN) == CNS_DIODE_DOWN)
                        strcat(lotrs_type, "Diode Down,");
                    if ((ptlink->TYPE & CNS_RESIST) == CNS_RESIST)
                        strcat(lotrs_type, "Resist,");
                    if ((ptlink->TYPE & CNS_CAPA) == CNS_CAPA)
                        strcat(lotrs_type, "Capa,");
                    if ((ptlink->TYPE & CNS_SWITCH) == CNS_SWITCH)
                        strcat(lotrs_type, "Switch,");
                    if ((ptlink->TYPE & CNS_COMMAND) == CNS_COMMAND)
                        strcat(lotrs_type, "Command,");

                    if (strlen(lotrs_type) > 0) {
                        lotrs_type[strlen(lotrs_type)-1] = '\0';
                        sprintf(Buffer, "(%s) ", lotrs_type);
                        strcat(XyagIdentifyBuffer, Buffer);
                    }
                    ptuser = getptype(ptlotrs->USER, CNS_DRIVINGCONE);
                    if (ptuser != NULL) {
                        sprintf(Buffer, "Driven by '%s', ", ((cone_list *)ptuser->DATA)->NAME);
                        strcat(XyagIdentifyBuffer, Buffer);
                    }
                    sprintf(Buffer, "W=%ld, L=%ld\n", ptlotrs->WIDTH, ptlotrs->LENGTH);
                    strcat(XyagIdentifyBuffer, Buffer);
                }
            }
            else {
                if ((ptlink->TYPE & CNS_IN) == CNS_IN) {
                    strcat(XyagIdentifyBuffer, "    In ");
                }
                else if ((ptlink->TYPE & CNS_INOUT) == CNS_INOUT) {
                    strcat(XyagIdentifyBuffer, "    InOut ");
                }
                ptlocon = ptlink->ULINK.LOCON;
                if (ptlocon != NULL) {
                    sprintf(Buffer, "%s\n", ptlocon->NAME);
                    strcat(XyagIdentifyBuffer, Buffer);
                }
            }
        }
    }
}

/*------------------------------------------------------------\
|                                                             |
|                         XyagNetIdentify                     |
|                                                             |
\------------------------------------------------------------*/

void 
XyagNetIdentify(CgvNet)
    cgvnet_list    *CgvNet;
{
    char           *Scan;
    chain_list     *ScanChain;
    cgvcon_list    *CgvCon;


    /* *****************  Identify des NET ******************** */

    Scan = XyagIdentifyBuffer;
    strcpy(Scan, "SIGNAL : \n\n");
    Scan += strlen(Scan);

    for (ScanChain = CgvNet->CON_NET;
         ScanChain != (chain_list *) 0;
         ScanChain = ScanChain->NEXT) {
        CgvCon = (cgvcon_list *) ScanChain->DATA;
        if (CgvCon->ROOT_TYPE == CGV_ROOT_CON_BOX) {
            if (!IsCgvBoxTransparence((cgvbox_list *) CgvCon->ROOT)) {
                sprintf(Scan, "> CONNECTOR : %s of INSTANCE %s\n", CgvCon->NAME
                        ,((cgvbox_list *) (CgvCon->ROOT))->NAME);
                Scan += strlen(Scan);
            }
        }
    }
}

/*------------------------------------------------------------\
|                     XyagAddIdentify                         |
\------------------------------------------------------------*/

short 
XyagAddIdentify(Obj)
    xyagobj_list   *Obj;
{
    long            Length;
    cgvbox_list    *CgvBox;
    cgvnet_list    *CgvNet;
    cgvcon_list    *CgvCon;
    char            StrCon[XYAG_IDENTIFY_BUFFER_SIZE];


    StrCon[0] = '\0';
    XyagIdentifyBuffer[0] = '\0';

    /* ************** Identify des BOX *********** */
    if (IsXyagCgvBox(Obj)) {
        CgvBox = (cgvbox_list *) Obj->USER;

        if (CgvBox->SOURCE_TYPE == CGV_SOURCE_LOINS) {
            sprintf(XyagIdentifyBuffer,
                    "INSTANCE : %s\nMODEL    : %s\n\n",
                    ((loins_list *) (CgvBox->SOURCE))->INSNAME,
                    ((loins_list *) (CgvBox->SOURCE))->FIGNAME);
        }
        else if (CgvBox->SOURCE_TYPE == CGV_SOURCE_CNSCONE) {
            XyagIdentifyCone((cone_list *) CgvBox->SOURCE);
        }
    }

    /* ******** Identify des CON ************** */

    else if (IsXyagCgvCon(Obj)) {
        CgvCon = (cgvcon_list *) Obj->USER;

        if (CgvCon->SOURCE_TYPE == CGV_SOURCE_LOCON) {
            sprintf(XyagIdentifyBuffer,
             "CONNECTOR : %s\n\n", ((locon_list *) (CgvCon->SOURCE))->NAME);
        }
    }
    /* ******************* Identify des NET*********** */
    else if (IsXyagCgvNet(Obj)) {
        CgvNet = (cgvnet_list *) Obj->USER;

        if (CgvNet->SOURCE != NULL) {

            if (CgvNet->SOURCE_TYPE == CGV_SOURCE_LOSIG) {
                if (CgvNet->CON_NET != NULL) {
                    XyagNetIdentify(CgvNet);
                }
            }
            else if (CgvNet->SOURCE_TYPE == CGV_SOURCE_CNSCONE) {
                if (CgvNet->SOURCE != NULL) {
                    XyagIdentifyCone((cone_list *) CgvNet->SOURCE);
                }
            }
            else if (CgvNet->SOURCE_TYPE == CGV_SOURCE_LOCON) {
                if (CgvNet->SOURCE != NULL) {
                    sprintf(XyagIdentifyBuffer,
                            "CONNECTOR : %s\n\n", ((locon_list *) (CgvNet->SOURCE))->NAME);
                }
            }
        }
    }

    Length = strlen(XyagIdentifyBuffer);

    if ((XyagIdentifyLength + Length) < (XYAG_IDENTIFY_MESSAGE_SIZE - 1)) {
        strcpy(XyagScanIdentify, XyagIdentifyBuffer);

        XyagScanIdentify += Length;
        XyagIdentifyLength += Length;

        return (XYAG_TRUE);
    }

    return (XYAG_FALSE);
}

/*------------------------------------------------------------\
|                         XyagEditIdentify                    |
\------------------------------------------------------------*/

void 
XyagEditIdentify(X1, Y1)
    long            X1;
    long            Y1;
{
    xyagselect_list *Select;

    strcpy(XyagIdentifyMessage, "No element found !");

    XyagScanIdentify = XyagIdentifyMessage;
    XyagIdentifyLength = 0;

    XyagEditSelectPoint(X1, Y1);

    for (Select = XyagHeadSelect; Select; Select = Select->NEXT) {
        if (!XyagAddIdentify(Select->OBJECT)) break;
    }

    XyagDelSelect();

    XyagDisplayEditIdentify(XyagIdentifyMessage);
    XyagEnterPanel(&XyagEditIdentifyPanel);
}

/*------------------------------------------------------------\
|                         XyagEditSelect                      |
\------------------------------------------------------------*/

void 
XyagEditSelect(X1, Y1)
    long            X1;
    long            Y1;
{
    xyagselect_list *Select;
    xyagobj_list   *Obj;

    if (XyagHeadConnect != NULL) {
        XyagDelConnect();
        XyagZoomRefresh();
    }

    XyagEditSelectPoint(X1, Y1);

    if (XyagHeadSelect == NULL) {
        XyagWarningMessage(XyagMainWindow, "No element found !");
    }
    else {
        XyagAcceptObject(XyagHeadSelect->OBJECT);
        XyagDisplayObject(XyagHeadSelect->OBJECT);
    }

    XyagPurgeSelect();

    for (Select = XyagHeadSelect; Select; Select = Select->NEXT) {
        XyagAddConnect(Select->OBJECT);
    }

    if (XyagHeadSelect != NULL) {
        Obj = XyagHeadSelect->OBJECT;
        XyagDelSelect();
        XyagDisplayObject(Obj);
    }
}

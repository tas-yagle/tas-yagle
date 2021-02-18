/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_error.c                                                 */
/*                                                                          */
/*    (c) copyright 1993 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Anthony LESTER                        le : ../../....     */
/*                                                                          */
/*    Modifie par : Anthony LESTER                      le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include "yag_headers.h"

/****************************************************************************
 *                         function yagWarning();                           *
 ****************************************************************************/
void 
yagWarning(int code, char *str1, char *str2, char *str3, int num)
{
    lotrs_list     *trans;
    char           *name, *gridname;
    char           *name1, *name2;
    ptype_list     *ptuser;
    chain_list     *ptchain;
    losig_list     *ptsig;
    char            empty[1];
    long            type, X, Y;

    empty[0] = 0;

    if (code == 0) code = num;
    else {
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "[WRN %02d] ", code);
    }

    switch (code) {

      case WAR_SUPPLY_CONNECTOR:
        if (str2 == NULL) str2 = empty;
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "%s connector detected%s : '%s'\n", str1, str2, str3);
        break;

      case WAR_INTERNAL_SUPPLY:
        if (str2 == NULL) str2 = empty;
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "%s internal signal detected%s : ", str1, str2);
        ptsig = (losig_list *) str3;
        for (ptchain = ptsig->NAMECHAIN; ptchain; ptchain = ptchain->NEXT) {
            fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "%s", (char *) ptchain->DATA);
            if (ptchain->NEXT != NULL) fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, ", ");
        }
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "\n");
        break;

      case WAR_SUPPLY_UNCONNECTED:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Possible unconnected supply ? : signal '%s' (%d)\n", str3, num);
        break;

      case WAR_BLEED_LOOP:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Loop between 2 gates '%s' (bleeder found)\n", str2);
        break;

      case WAR_LEVELHOLD_LOOP:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Loop between 2 gates '%s' (level-hold found)\n", str2);
        break;

      case WAR_LATCH_LOOP:
        if (str1 != NULL) type = ((cell_list *) str1)->TYPE & ~CNS_UNKNOWN;
        else type = 0;
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Loop between 2 gates at %ld '%s' (latch found)\n", (long) str3, str2);
        break;

      case WAR_MASTER_LATCH_LOOP:
        if (str1 != NULL) type = ((cell_list *) str1)->TYPE & ~CNS_UNKNOWN;
        else type = 0;
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Loop between 2 gates at %ld '%s' (master latch found)\n", (long) str3, str2);
        break;

      case WAR_SLAVE_LATCH_LOOP:
        if (str1 != NULL) type = ((cell_list *) str1)->TYPE & ~CNS_UNKNOWN;
        else type = 0;
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Loop between 2 gates at %ld '%s' (slave latch found)\n", (long) str3, str2);
        break;

      case WAR_BISTABLE_LOOP:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Loop between 2 gates '%s' and '%s' (RS found)\n", str1, str2);
        break;

      case WAR_FLIP_FLOP_LOOP:
        if (str1 != NULL) type = ((cell_list *) str1)->TYPE & ~CNS_UNKNOWN;
        else type = 0;
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Loop between 2 gates at %ld '%s' (flip-flop found)\n", (long) str3, str2);
        break;

      case WAR_UNKNOWN_LOOP:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "*** CHECK *** Loop between 2 gates '%s' and '%s' (nothing found)\n", str1, str2);
        break;

      case WAR_STUCK_LOOP:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Stuck or disabled Loop between 2 gates '%s' and '%s'\n", str1, str2);
        break;

      case WAR_UNMARKEDLATCH_LOOP:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Unmarked latch loop at %ld '%s'\n", (long) str3, str2);
        break;

      case WAR_PRECHARGE:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Precharge found at at %ld '%s'\n", (long) str3, str2);
        break;

      case WAR_UNMARKED_PRECHARGE:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Unmarked precharge at %ld '%s'\n", (long) str3, str2);
        break;

      case WAR_RESISTANCE:
        trans = (lotrs_list *) str3;
        if (trans->TRNAME != NULL) name = trans->TRNAME;
        else name = empty;
        if ((X = trans->X) == LONG_MIN) X = 0;
        if ((Y = trans->Y) == LONG_MIN) Y = 0;
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Transistor %s '%s' (X=%ld,Y=%ld) is a resistance\n", str2, name, X, Y);
        break;

      case WAR_DIODE:
        trans = (lotrs_list *) str3;
        if (trans->TRNAME != NULL) name = trans->TRNAME;
        else name = empty;
        if ((X = trans->X) == LONG_MIN) X = 0;
        if ((Y = trans->Y) == LONG_MIN) Y = 0;
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Transistor '%s' (X=%ld,Y=%ld) is a diode\n", name, X, Y);
        break;

      case WAR_ALWAYSOFF:
        trans = (lotrs_list *) str3;
        if (trans->TRNAME != NULL) name = trans->TRNAME;
        else name = empty;
        if ((X = trans->X) == LONG_MIN) X = 0;
        if ((Y = trans->Y) == LONG_MIN) Y = 0;
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Transistor %s '%s' (X=%ld,Y=%ld) is always off\n", str2, name, X, Y);
        break;

      case WAR_CAPACITANCE:
        trans = (lotrs_list *) str3;
        if (trans->TRNAME != NULL) name = trans->TRNAME;
        else name = empty;
        if ((X = trans->X) == LONG_MIN) X = 0;
        if ((Y = trans->Y) == LONG_MIN) Y = 0;
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Transistor '%s' (X=%ld,Y=%ld) is a capacitance\n", name, X, Y);
        break;

      case WAR_UNCONNECTED:
        trans = (lotrs_list *) str3;
        if (trans->TRNAME != NULL) name = trans->TRNAME;
        else name = empty;
        if ((X = trans->X) == LONG_MIN) X = 0;
        if ((Y = trans->Y) == LONG_MIN) Y = 0;
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "%s of transistor '%s' (X=%ld,Y=%ld) is not connected\n", str2, name, X, Y);
        break;

      case WAR_UNUSED:
        trans = (lotrs_list *) str3;
        if (trans->TRNAME != NULL) name = trans->TRNAME;
        else name = empty;
        if ((X = trans->X) == LONG_MIN) X = 0;
        if ((Y = trans->Y) == LONG_MIN) Y = 0;
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Transistor %s '%s' (X=%ld,Y=%ld) is not used\n", str2, name, X, Y);
        break;

      case WAR_DOESNT_DRIVE:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Signal %s does not drive anything\n", str2);
        break;

      case WAR_CONFLICT:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Conflict may occur on signal '%s'\n", str2);
        break;

      case WAR_HZ:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "HZ state may occur on signal '%s'\n", str2);
        break;

      case WAR_LATCH_CONFLICT:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "*** CHECK *** Unresolved conflict on latch '%s' (check constraints)\n", str2);
        break;

      case WAR_BUS_CONFLICT:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "*** CHECK *** Unresolved conflict on bus '%s' (check constraints)\n", str2);
        break;

      case WAR_LATCH_HZ:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "*** CHECK *** Unresolved HZ state on latch '%s' (check constraints)\n", str2);
        break;

      case WAR_CON_UNUSED:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Connector '%s' unused\n", str2);
        break;

      case WAR_SAME_S:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Same level of _s between signals '%s' and '%s'\n", str1, str2);
        break;

      case WAR_NO_INPUTS:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Signal '%s' has no drivers, possibly unrouted\n", str2);
        break;

      case WAR_MULTIPLE_BEOBJ:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Signal '%s' has multiple behavioural drivers\n", str2);
        break;

      case WAR_BEPOR_CONFLICT:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Behavioural conflict on connector '%s'\n", str2);
        break;

      case WAR_BIDIR_BLOCKED:
        trans = (lotrs_list *) str3;
        if (trans->TRNAME != NULL) name = trans->TRNAME;
        else name = empty;
        if ((X = trans->X) == LONG_MIN) X = 0;
        if ((Y = trans->Y) == LONG_MIN) Y = 0;
        ptuser = getptype(trans->USER, CNS_DRIVINGCONE);
        gridname = empty;
        if (ptuser != NULL && ptuser->DATA != NULL) {
            gridname = ((cone_list *) ptuser->DATA)->NAME;
        }
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Transistor %s '%s' (X=%ld,Y=%ld) (S='%s':G='%s':D='%s') seems bidirectional - BLOCKED\n", str2, name, X, Y, (char *) trans->SOURCE->SIG->NAMECHAIN->DATA, gridname, (char *) trans->DRAIN->SIG->NAMECHAIN->DATA);
        break;

      case WAR_SPLIT_TIMING:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Rise/Fall timing separated on signal '%s'\n", str2);
        break;

      case WAR_GLITCHER:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Glitcher detected on signal '%s'\n", str1);
        break;

      case WAR_STUCK_ZERO:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Signal '%s' is stuck at Zero\n", str2);
        break;

      case WAR_STUCK_ONE:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Signal '%s' is stuck at One\n", str2);
        break;

      case WAR_VALUE_S:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Signal '%s' has orientation level %d\n", str2, num);
        break;

      case WAR_CLOCK_GATING:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Clock gating detected between data '%s' and clock '%s' (active '%d')\n", str1, str2, num);
        break;

      case WAR_DELAYED_RS:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Delayed RS structure (non-overlapping clocks?) detected between '%s' and '%s'\n", str1, str2);
        break;

      case WAR_BADCONE:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "*** CHECK *** Gate '%s' has long current paths, verify MUTEX constraints\n", str2);
        break;

      default:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Unrecognized warning\n");
        break;
    }
}


/****************************************************************************
 *                         function yagError();                             *
 ****************************************************************************/
void 
yagError(code, str1, str2, str3, num2, num3)
    int             code;
    char           *str1;
    char           *str2;
    char           *str3;
    int             num2;
    int             num3;

{
#ifndef __ALL__WARNING__
    str3 = NULL;
#endif
    if (code == 0) code = num2;
    else {
        if (YAG_CONTEXT->YAG_NOTSTRICT) {
            if (code != ERR_GATE_UNDRIVEN && code != ERR_CON_DIR && code != ERR_NO_VDD_SIG && code != ERR_NO_VSS_SIG)
                YAG_CONTEXT->YAGLE_NB_ERR++;
        }
        else YAG_CONTEXT->YAGLE_NB_ERR++;
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "[ERR %02d] ", code);
    }

    switch (code) {

      case ERR_SHORT_CIRCUIT:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Possible short circuit ? : signal '%s'\n", str2);
        break;

      case ERR_TRANS_DIRCONF:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Directional conflict on transistor (X=%d,Y=%d)\n", num2, num3);
        break;

      case ERR_CON_DIR:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Bad direction on connector '%s'\n", str1);
        break;

      case ERR_NOT_DRIVEN:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Signal %s is not driven \n", str2);
        break;

      case ERR_GATE_UNDRIVEN:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Transistor-gate signal '%s' is not driven\n", str2);
        break;

      case ERR_NO_VDD_SIG:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "No VDD signal in the circuit \n");
        break;

      case ERR_NO_VSS_SIG:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "No VSS signal in the circuit \n");
        break;

      default:
        fprintf(YAG_CONTEXT->YAGLE_ERR_FILE, "Unrecognized error\n");
        break;

    }
    fflush(YAG_CONTEXT->YAGLE_ERR_FILE);
}

/****************************************************************************
 *                         function yagBug();                               *
 ****************************************************************************/
void 
yagBug(code, str1, str2, str3, num)
    int             code;
    char           *str1;
    char           *str2;
    char           *str3;
    int             num;
{
    if (code == 0)
        code = num;
    else {
        avt_log(LOGYAG, 0, "\n[YAG BD ERR %d] executing '%s' ", code, str1);
    }

    switch (code) {

      case DBG_NULL_PTR:
        avt_log(LOGYAG, 0, "NULL Argument pointer \n");
        break;

      case DBG_NO_SIGUSER:
        avt_log(LOGYAG, 0, "Signal '%s' without USER '%s'\n", str2, str3);
        break;

      case DBG_NO_INCONEUSER:
        avt_log(LOGYAG, 0, "Incone of cone '%s' without USER '%s'\n", str2, str3);
        break;

      case DBG_NO_CONEMODEL:
        avt_log(LOGYAG, 0, "Cone '%s' without model \n", str2);
        break;

      case DBG_NO_PATH_USER:
        avt_log(LOGYAG, 0, "Path of cone '%s' without USER '%s'\n", str2, str3);
        break;

      case DBG_TABPATH_OVR:
        avt_log(LOGYAG, 0, "TabPath overflow\n");
        break;

      case DBG_LOST_OUTCONE:
        avt_log(LOGYAG, 0, "Outcone '%s' of cone '%s' not found\n", str2, str3);
        break;

      case DBG_LOST_INCONE:
        avt_log(LOGYAG, 0, "Incone '%s' of cone '%s' not found\n", str2, str3);
        break;

      case DBG_NO_INCONE:
        avt_log(LOGYAG, 0, "Incone is missing in cone '%s'\n", str2);
        break;

      case DBG_NO_OUTCONE:
        avt_log(LOGYAG, 0, "Outcone is missing in cone '%s'\n", str2);
        break;

      case DBG_NO_LATCHCMD:
        avt_log(LOGYAG, 0, "Command of latch '%s' not found\n", str2);
        break;

      case DBG_NO_CONEUSER:
        avt_log(LOGYAG, 0, "Cone '%s' without USER '%s'\n", str2, str3);
        break;

      case DBG_ILL_PATHTYPE:
        avt_log(LOGYAG, 0, "Unknown type of path in cone '%s'\n", str2);
        break;

      case DBG_NO_CONUSER:
        avt_log(LOGYAG, 0, "Connector '%s' without USER '%s'\n", str2, str3);
        break;

      case DBG_NO_CONTEXT:
        avt_log(LOGYAG, 0, "No execution context (missing yagenv)\n");
        break;

      case DBG_ILL_LINKTYPE:
        avt_log(LOGYAG, 0, "Unknown type of link in cone '%s'\n", str2);
        break;

      case DBG_EXTCONE_NOPATH:
        avt_log(LOGYAG, 0, "Cone '%s' EXT without path\n", str2);
        break;

      case DBG_NO_ABL:
        avt_log(LOGYAG, 0, "Cone '%s' without ABL\n", str2);
        break;

      case DBG_NO_FATHER:
        avt_log(LOGYAG, 0, "Graph node '%s' has no father '%s'\n", str3, str2);
        break;

      case DBG_NO_GRAPHNODE:
        avt_log(LOGYAG, 0, "Graph node '%s' does not exist\n", str2);
        break;

      case DBG_BAD_ABL:
        avt_log(LOGYAG, 0, "ABL of cone '%s' is incorrect\n", str2);
        break;

      case DBG_NO_BDD:
        avt_log(LOGYAG, 0, "Missing BDD for '%s' in cone '%s'\n", str3, str2);
        break;

      case DBG_NO_CONLINK:
        avt_log(LOGYAG, 0, "Cone '%s' EXT without connector link\n", str2);
        break;

      case DBG_ILL_CELLTYPE:
        avt_log(LOGYAG, 0, "Unknown cell type\n");
        break;

      case DBG_ILL_CONTYPE:
        avt_log(LOGYAG, 0, "Unknown connector type\n");
        break;

      case DBG_MULT_CON:
        avt_log(LOGYAG, 0, "Multiple external connectors on '%s'\n", str2);
        break;

      default:
        avt_log(LOGYAG, 0, "Unrecognized bug\n");
        break;
    }
    avt_log(LOGYAG, 0, "CONTINUE WITH CAUTION\n");
}

/****************************************************************************
 *                         function yagMessage();                           *
 ****************************************************************************/
void 
yagMessage(code, str1)
    int             code;
    char           *str1;
{
    switch (code) {

      case MES_LOADING:
        avt_log(LOGYAG, 0, "Loading the figure %-21s", str1);
        avt_log(LOGYAG, 1, "\n");
        break;

      case MES_DISASSEMBLING:
        avt_log(LOGYAG, 0, "Disassembling the figure %s\n", str1);
        break;

      case MES_UNFLAT:
        avt_log(LOGYAG, 0, "Creating figure %-24s", str1);
        avt_log(LOGYAG, 1, "\n");
        break;

      case MES_FLATTENING:
        avt_log(LOGYAG, 0, "Flattening the figure                   ");
        avt_log(LOGYAG, 1, "\n");
        break;

      case MES_TRANS_CHECKING:
        avt_log(LOGYAG, 0, "Transistor netlist checking             ");
        avt_log(LOGYAG, 1, "\n");
        break;

      case MES_DUAL_EXTRACT:
        avt_log(LOGYAG, 0, "Extracting CMOS duals                   ");
        avt_log(LOGYAG, 1, "\n");
        break;

      case MES_BLEED_EXTRACT:
        avt_log(LOGYAG, 0, "Extracting bleeders                     ");
        avt_log(LOGYAG, 1, "\n");
        break;

      case MES_TRANS_ORIENT:
        avt_log(LOGYAG, 0, "Transistor orientation\n");
        break;

      case MES_BUILD_CONES:
        avt_log(LOGYAG, 0, "Building the cones\n");
        break;

      case MES_MAKING_GATES:
        avt_log(LOGYAG, 0, "Making gates                            ");
        avt_log(LOGYAG, 1, "\n");
        break;

      case MES_GENIUS:
        avt_log(LOGYAG, 0, "Hierarchical recognition                ");
        avt_log(LOGYAG, 1, "\n");
        break;

      case MES_MAKING_CELLS:
        avt_log(LOGYAG, 0, "Making cells                            ");
        avt_log(LOGYAG, 1, "\n");
        break;

      case MES_VERIF_EXTCON:
        avt_log(LOGYAG, 0, "External connector verification         ");
        avt_log(LOGYAG, 1, "\n");
        break;

      case MES_DUAL_DETECTION:
        avt_log(LOGYAG, 0, "Dual gate detection \n");
        break;

      case MES_LATCH_DETECTION:
        avt_log(LOGYAG, 0, "Latches detection                       ");
        avt_log(LOGYAG, 1, "\n");
        break;

      case MES_RELAX:
        avt_log(LOGYAG, 0, "Relaxation                              ");
        avt_log(LOGYAG, 1, "\n");
        break;

      case MES_BUILD_BEFIG:
        avt_log(LOGYAG, 0, "Building the behavioural figure         ");
        avt_log(LOGYAG, 1, "\n");
        break;

      case MES_SUPPRESS_BEAUX:
        avt_log(LOGYAG, 0, "Simplification of expressions           ");
        avt_log(LOGYAG, 1, "\n");
        break;

      case MES_BUILD_LOFIG:
        avt_log(LOGYAG, 0, "Building the logical figure             ");
        avt_log(LOGYAG, 1, "\n");
        break;

      case MES_FIG_CHECKING:
        avt_log(LOGYAG, 0, "Checking the yagle figure               ");
        avt_log(LOGYAG, 1, "\n");
        break;

      case MES_MS_DETECTION:
        avt_log(LOGYAG, 0, "Master Slave detection                  ");
        avt_log(LOGYAG, 1, "\n");
        break;

      case MES_SAVE_CNS:
        avt_log(LOGYAG, 0, "Saving the figure '%s.cns'\n", str1);
        break;

      case MES_TRANS_ERASING:
        avt_log(LOGYAG, 0, "Erasing the transistor netlist\n");
        break;

      case MES_MAKING_NETLIST:
        avt_log(LOGYAG, 0, "Generating an hierarchical netlist\n");
        break;

      case MES_GEN_FILE:
        avt_log(LOGYAG, 0, "Generating the Data Flow\n");
        break;

      case MES_READING_INF:
        avt_log(LOGYAG, 0, "Reading file '%s.inf'\n", str1);
        break;

      case MES_ELPCAPA:
        avt_log(LOGYAG, 0, "Updating capacitances                   ");
        avt_log(LOGYAG, 1, "\n");
        break;

      case MES_TAS_TIMING:
        avt_log(LOGYAG, 0, "Calculating timing                      ");
        avt_log(LOGYAG, 1, "\n");
        break;

      case MES_TAS_TECHNO:
        avt_log(LOGYAG, 0, "Characterizing transistors              ");
        avt_log(LOGYAG, 1, "\n");
        break;

      case MES_COMPLETED:
        avt_log(LOGYAG, 0, "Execution COMPLETED \n");
        break;

      case MES_NO_TECHNO_FILE:
        avt_log(LOGYAG, 0, "No technology parameters - Timing Disabled\n");
        break;

      case MES_EMPTY_CIRCUIT:
        avt_log(LOGYAG, 0, "No transistors or instances in circuit\n");
        break;

      default:
        avt_log(LOGYAG, 0, "This is not a legal message !?! Fingers crossed\n");
        break;

    }
}

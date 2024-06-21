/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_error.h                                                 */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

/* REP Warning codes */

#define WAR_SUPPLY_CONNECTOR    1
#define WAR_INTERNAL_SUPPLY     2
#define WAR_SUPPLY_UNCONNECTED  3
#define WAR_RESISTANCE          4
#define WAR_DIODE               5
#define WAR_ALWAYSOFF           6
#define WAR_CAPACITANCE         7
#define WAR_PARALLEL            8
#define WAR_UNCONNECTED         9
#define WAR_UNUSED              10
#define WAR_BLEED_LOOP          11
#define WAR_LATCH_LOOP          12
#define WAR_BISTABLE_LOOP       13
#define WAR_FLIP_FLOP_LOOP      14
#define WAR_UNKNOWN_LOOP        15
#define WAR_DOESNT_DRIVE        16
#define WAR_CONFLICT            17
#define WAR_HZ                  18
#define WAR_CON_UNUSED          19
#define WAR_SAME_S              20
#define WAR_NO_INPUTS           21
#define WAR_MULTIPLE_BEOBJ      22
#define WAR_BEPOR_CONFLICT      23
#define WAR_BIDIR_BLOCKED       24
#define WAR_SPLIT_TIMING        25
#define WAR_GLITCHER            26
#define WAR_UNMARKEDLATCH_LOOP  27
#define WAR_PRECHARGE           28
#define WAR_UNMARKED_PRECHARGE  29
#define WAR_STUCK_ZERO          30
#define WAR_STUCK_ONE           31
#define WAR_VALUE_S             32
#define WAR_MASTER_LATCH_LOOP   33
#define WAR_SLAVE_LATCH_LOOP    34
#define WAR_CLOCK_GATING        35
#define WAR_LEVELHOLD_LOOP      36
#define WAR_BADCONE             37
#define WAR_DELAYED_RS          38
#define WAR_STUCK_LOOP          39
#define WAR_LATCH_CONFLICT      40
#define WAR_BUS_CONFLICT        41
#define WAR_LATCH_HZ            42

/* REP Error codes */

#define ERR_SHORT_CIRCUIT       1
#define ERR_TRANS_DIRCONF       3
#define ERR_CON_DIR             4
#define ERR_NOT_DRIVEN          5
#define ERR_GATE_UNDRIVEN       6
#define ERR_NO_VDD_SIG          7
#define ERR_NO_VSS_SIG          8
  
/* Debug codes */

#define DBG_NULL_PTR            1
#define DBG_NO_SIGUSER          2
#define DBG_NO_INCONEUSER       3
#define DBG_NO_CONEMODEL        4
#define DBG_NO_PATH_USER        5
#define DBG_TABPATH_OVR         6
#define DBG_LOST_OUTCONE        7
#define DBG_LOST_INCONE         8
#define DBG_NO_INCONE           9
#define DBG_NO_OUTCONE          10
#define DBG_NO_LATCHCMD         11
#define DBG_NO_CONEUSER         12
#define DBG_ILL_PATHTYPE        13
#define DBG_NO_CONUSER          14
#define DBG_NO_CONTEXT          15
#define DBG_ILL_LINKTYPE        16
#define DBG_EXTCONE_NOPATH      17
#define DBG_NO_ABL              18
#define DBG_NO_FATHER           19
#define DBG_BAD_ABL             20
#define DBG_NO_BDD              21
#define DBG_NO_CONLINK          22
#define DBG_ILL_CELLTYPE        23
#define DBG_ILL_CONTYPE         24
#define DBG_MULT_CON            25
#define DBG_NO_GRAPHNODE        26

/* Message codes */

#define MES_LOADING             1
#define MES_FLATTENING          2
#define MES_TRANS_CHECKING      3
#define MES_DUAL_EXTRACT        4
#define MES_BLEED_EXTRACT       5
#define MES_TRANS_ORIENT        6
#define MES_BUILD_CONES         7
#define MES_MAKING_GATES        8
#define MES_MAKING_CELLS        9
#define MES_VERIF_EXTCON        10
#define MES_DUAL_DETECTION      11
#define MES_LATCH_DETECTION     12
#define MES_RELAX               13
#define MES_BUILD_BEFIG         14
#define MES_BUILD_LOFIG         15
#define MES_FIG_CHECKING        16
#define MES_MS_DETECTION        17
#define MES_SAVE_CNS            18
#define MES_TRANS_ERASING       19
#define MES_MAKING_NETLIST      20
#define MES_GEN_FILE            21
#define MES_READING_INF         22
#define MES_ELPCAPA             23
#define MES_COMPLETED           24
#define MES_SUPPRESS_BEAUX      25
#define MES_GENIUS              26
#define MES_UNFLAT              27
#define MES_DISASSEMBLING       28
#define MES_TAS_TIMING          29
#define MES_NO_TECHNO_FILE      30
#define MES_EMPTY_CIRCUIT       31
#define MES_TAS_TECHNO          32

void            yagWarning(int code, char *str1, char *str2, char *str3, int num);
void            yagError(int code, char *str1, char *str2, char *str3, int num2, int num3);
void            yagBug(int code, char *str1, char *str2, char *str3, int num);
void            yagMessage(int code, char *str1);

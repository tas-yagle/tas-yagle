/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TTV Version 1                                               */
/*    Fichier : ttv_lib.h                                                   */
/*                                                                          */
/*    (c) copyright 1995-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : DIOURY Karim                                              */
/*                                                                          */
/****************************************************************************/
/* les structures et base de donnees de TTV                                 */
/****************************************************************************/

#ifndef __TTV_LIB_H__
#define __TTV_LIB_H__

/* LISTE DES INCLUDE */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#undef   CMIN
#undef   CMAX

/* LISTE DES DEFINE */

/* defines generaux */
#define TTV_MAX_BLOC       256                  /* taille des blocs          */
#define TTV_MAX_SBLOC      256                  /* taille des sblocs         */
#define TTV_MAX_LBLOC      64                   /* taille des lblocs         */
#define TTV_ERROR          1                    /* ttv error                 */
#define TTV_WARNING        2                    /* ttv warning               */
#define TTV_DATE_LOCAL     1                    /* date et heure local       */
#define TTV_DATE_GMT       2                    /* date et heure gmt         */
#define TTV_LANG_E         'E'                  /* langue anglaise           */ 
#define TTV_LANG_F         'F'                  /* langue francaise          */ 
#define TTV_SAVE_PATH      'Y'                  /* sauve les chemins         */ 
#define TTV_NOTSAVE_PATH   'N'                  /* ne sauve pas les chemins  */ 
#define TTV_SAVE_ALLPATH   'A'                  /* sauve tous les chemins    */ 
#define TTV_OLD_MAX        (LONG_MAX          ) /* age le plus vieux         */ 
#define TTV_OLD_MIN        (LONG_MAX>>1       ) /* rajeunissement            */ 
#define TTV_ALLOC_MAX      (LONG_MAX          ) /* nb d'allocation max s & l */ 
#define TTV_UP             'U'                  /* transition montante       */
#define TTV_DOWN           'D'                  /* transition descendante    */
#define TTV_DELAY_MAX      (LONG_MAX          ) /* delai maximum autorise    */
#define TTV_DELAY_MIN      (LONG_MIN          ) /* delai minimum autorise    */
#define TTV_MAX_PATHNB     (INT_MAX           ) /* delai minimum autorise    */
#define TTV_MAX_PERIOD     (CHAR_MAX          ) /* delai minimum autorise    */
#define TTV_NO_PHASE       ((char)-1          ) /* no phase                  */

/* status des ttvfig */
#define TTV_STS_MARQUE     ((long) 0x10000000 ) /* ttvfig marque             */
#define TTV_STS_MODEL      ((long) 0x20000000 ) /* ttvfig model              */
#define TTV_STS_NOT_FREE   ((long) 0x00080000 ) /* ttvfig a conserver        */
#define TTV_STS_FREE_2     ((long) 0x00040000 ) /* ttvfig a liberer en 2     */
#define TTV_STS_FREE_1     ((long) 0x00020000 ) /* ttvfig a liberer en 1     */
#define TTV_STS_FREE_0     ((long) 0x00010000 ) /* ttvfig a liberer en 0     */
#define TTV_STS_FREE_MASK  ((long) 0x000F0000 ) /* ttvfig a conserver        */
#define TTV_STS_HEAD       ((long) 0x00100000 ) /* sommet de la ttvfig       */
#define TTV_STS_DUAL_P     ((long) 0x00200000 ) /* dual des liens P          */
#define TTV_STS_DUAL_J     ((long) 0x00400000 ) /* dual des liens J          */
#define TTV_STS_DUAL_T     ((long) 0x00800000 ) /* dual des liens T          */
#define TTV_STS_DUAL_PJT   ((long) 0x00e00000 ) /* dual des liens PJT        */
#define TTV_STS_DUAL_F     ((long) 0x01000000 ) /* dual des liens F          */
#define TTV_STS_DUAL_E     ((long) 0x02000000 ) /* dual des liens E          */
#define TTV_STS_DUAL_D     ((long) 0x04000000 ) /* dual des liens D          */
#define TTV_STS_DUAL_FED   ((long) 0x07000000 ) /* dual des liens FED        */
#define TTV_STS_DUAL       ((long) 0x07e00000 ) /* dual des liens            */
#define TTV_STS_HEADER     ((long) 0x00000001 ) /* ttvfig avec H et X        */
#define TTV_STS_LOCK       ((long) 0x00000002 ) /* ttvfig locker             */
#define TTV_STS_C          ((long) 0x00000010 ) /* noeud C                   */
#define TTV_STS_L          ((long) 0x00000020 ) /* noeud QLRI                */
#define TTV_STS_S          ((long) 0x00000040 ) /* noeud S                   */
#define TTV_STS_SE         ((long) 0x00000080 ) /* SE ref dans la ttvfig     */
#define TTV_STS_P          ((long) 0x00000100 ) /* chemin complet            */
#define TTV_STS_J          ((long) 0x00000200 ) /* chemin partiel sur un T   */
#define TTV_STS_T          ((long) 0x00000400 ) /* chemin connecteur         */
#define TTV_STS_F          ((long) 0x00001000 ) /* temps portes interne      */
#define TTV_STS_E          ((long) 0x00002000 ) /* temps porte externe       */
#define TTV_STS_D          ((long) 0x00004000 ) /* temps porte connecteur    */
#define TTV_STS_DENOTINPT  ((long) 0x00008000 ) /* DE pas dans PT            */
#define TTV_STS_DTX        ((long) 0x000070f1 ) /* fichier dtx               */
#define TTV_STS_TTX        ((long) 0x00000731 ) /* fichier ttx               */
#define TTV_STS_ALL        ((long) 0x000077f1 ) /* ttvfig complet            */
#define TTV_STS_L_P        ((long) 0x00000120 ) /* QLRIN+P                   */
#define TTV_STS_L_J        ((long) 0x00000220 ) /* QLRIN+J                   */
#define TTV_STS_L_PJ       ((long) 0x00000320 ) /* QLRIN+PJ                  */
#define TTV_STS_LS         ((long) 0x00000060 ) /* QLRIN+S                   */
#define TTV_STS_LS_F       ((long) 0x00001060 ) /* QLRIN+S+F                 */
#define TTV_STS_LS_E       ((long) 0x00002060 ) /* QLRIN+S+E                 */
#define TTV_STS_LS_FE      ((long) 0x00003060 ) /* QLRIN+S+EF                */
#define TTV_STS_CL         ((long) 0x00000030 ) /* CQLRIN                    */
#define TTV_STS_CL_T       ((long) 0x00000430 ) /* CQLRIN+T                  */
#define TTV_STS_CL_PJT     ((long) 0x00000730 ) /* CQLRIN+PJT                */
#define TTV_STS_CLS        ((long) 0x00000070 ) /* CQLRIN+S                  */
#define TTV_STS_CLS_D      ((long) 0x00004070 ) /* CQLRIN+S+D                */
#define TTV_STS_CLS_FED    ((long) 0x00007070 ) /* CQLRIN+S+FED              */

/* type user des ttvfig */
#define TTV_STS_HTAB_L     ((long) 0x00000001 ) /* table de hash pour L      */
#define TTV_STS_HTAB_S     ((long) 0x00000002 ) /* table de hash pour S      */
#define TTV_FIG_POS_S      ((long) 0x00000003 ) /* position de S dans .dtx   */
#define TTV_FIG_POS_D      ((long) 0x00000004 ) /* position de D dans .dtx   */
#define TTV_FIG_POS_E      ((long) 0x00000005 ) /* position de E dans .dtx   */
#define TTV_FIG_POS_F      ((long) 0x00000006 ) /* position de F dans .dtx   */
#define TTV_FIG_POS_T      ((long) 0x00000007 ) /* position de T dans .ttx   */
#define TTV_FIG_POS_J      ((long) 0x00000008 ) /* position de J dans .ttx   */
#define TTV_FIG_POS_P      ((long) 0x00000009 ) /* position de P dans .ttx   */
#define TTV_FIG_LINE_S     ((long) 0x0000000a ) /* position de S dans .dtx   */
#define TTV_FIG_LINE_D     ((long) 0x0000000b ) /* position de D dans .dtx   */
#define TTV_FIG_LINE_E     ((long) 0x0000000c ) /* position de E dans .dtx   */
#define TTV_FIG_LINE_F     ((long) 0x0000000d ) /* position de F dans .dtx   */
#define TTV_FIG_LINE_T     ((long) 0x0000000e ) /* position de T dans .ttx   */
#define TTV_FIG_LINE_J     ((long) 0x0000000f ) /* position de J dans .ttx   */
#define TTV_FIG_LINE_P     ((long) 0x00000010 ) /* position de P dans .ttx   */
#define TTV_FIG_FALSEPATH  ((long) 0x00000011 ) /* faux chemins              */
#define TTV_FIG_SIGBLOCTAB ((long) 0x00000012 ) /* tableau de signal         */
#define TTV_FIG_LOFIG      ((long) 0x00000013 ) /* lofig associe             */
#define TTV_FIG_HTAB_SIG   ((long) 0x00000014 ) /* table hash signaux        */
#define TTV_FIG_INFLIST    ((long) 0x00000015 ) /* info fichier inf          */
#define TTV_FIG_TYPE       ((long) 0x00000016 ) /* type de ttvfig en memoire */
#define TTV_FIG_SAXIS      ((long) 0x00000017 ) /* ht slope axis             */
#define TTV_FIG_CAXIS      ((long) 0x00000018 ) /* ht capa axis              */
#define TTV_FIG_CAPAOUT_HANDLED   ((long) 0x00000019 ) 
#define TTV_FIG_DIRECTIVES_HEAP ((long) 0x0000001a ) 
#define TTV_SSTA_STORE 0x0000001b
#define TTV_SSTA_STORE_STATS 0x0000001c
#define TTV_FIG_FULL_MEMORY 0x0000001d
#define TTV_CELL_LEAKAGE_POWER 0x0000001e
#define TTV_LEAKAGE_POWER 0x0000001f
#define TTV_FIG_CBHSEQ 0x00000020
#define TTV_FIG_VCDPOWER 0x00000021

/* type des signaux */
#define TTV_SIG_C          ((long) 0x00000001 ) /* signal connecteur         */
#define TTV_SIG_CI         ((long) 0x00000003 ) /* signal connecteur in      */
#define TTV_SIG_CO         ((long) 0x00000005 ) /* signal connecteur out     */
#define TTV_SIG_CZ         ((long) 0x00000015 ) /* signal connecteur hz      */
#define TTV_SIG_CB         ((long) 0x00000027 ) /* signal connecteur inout   */
#define TTV_SIG_CT         ((long) 0x00000047 ) /* signal connecteur tstate  */
#define TTV_SIG_CX         ((long) 0x00000081 ) /* signal connecteur unknow  */
#define TTV_SIG_N          ((long) 0x00000008 ) /* signal noeud con pour RC  */
#define TTV_SIG_CQ         ((long) 0x00000101 ) /* signal connecteur cmd     */
#define TTV_SIG_CL         ((long) 0x00000201 ) /* signal connecteur cmd     */
#define TTV_SIG_CR         ((long) 0x00000401 ) /* signal connecteur cmd     */
#define TTV_SIG_NQ         ((long) 0x00000108 ) /* con rc pour cmd           */
#define TTV_SIG_NL         ((long) 0x00000208 ) /* con rc pour cmd           */
#define TTV_SIG_NR         ((long) 0x00000408 ) /* con rc pour cmd           */
#define TTV_SIG_Q          ((long) 0x00000100 ) /* signal latch command      */
#define TTV_SIG_QR         ((long) 0x00000120 ) /* signal latch reset        */
#define TTV_SIG_L          ((long) 0x00000200 ) /* signal latch              */
#define TTV_SIG_LSL        ((long) 0x00000220 ) /* signal latch slave        */
#define TTV_SIG_LL         ((long) 0x00004200 ) /* signal latch latch        */
#define TTV_SIG_LF         ((long) 0x00008200 ) /* signal flipflop           */
#define TTV_SIG_LR         ((long) 0x00004210 ) /* signal RS latch nand      */
#define TTV_SIG_LS         ((long) 0x00004220 ) /* signal RS latch nor       */
#define TTV_SIG_R          ((long) 0x00000400 ) /* signal precharge          */
#define TTV_SIG_I          ((long) 0x00000800 ) /* signal intermediaire      */
#define TTV_SIG_S          ((long) 0x00001000 ) /* signal non CQLRIN         */
#define TTV_SIG_B          ((long) 0x00002000 ) /* signal break              */
#define TTV_SIG_TYPEALL    ((long) 0x00003F09 ) /* signal type               */

/* marquage des signaux */
#define TTV_SIG_TYPE       ((long) 0x0000FFFF ) /* masquage du type signal   */
#define TTV_SIG_TYPECON    ((long) 0x000000FF ) /* masquage des connecteurs  */
#define TTV_SIG_LEVEL      ((long) 0x003F0000 ) /* masquage des connecteurs  */
#define TTV_SIG_EXT        ((long) 0x00400000 ) /* signal exterieur          */
#define TTV_SIG_MARQUE     ((long) 0x10000000 ) /* signal marque             */
#define TTV_SIG_F          ((long) 0x20000000 ) /* signal libre              */
#define TTV_SIG_BYPASSOUT  ((long) 0x40000000 ) /* signal non-important OUT  */
#define TTV_SIG_BYPASSIN   ((long) 0x80000000 ) /* signal non-important IN   */
#define TTV_SIG_MASKIN     ((long) 0x01000000 ) /* signal mask pour in       */
#define TTV_SIG_MASKOUT    ((long) 0x02000000 ) /* signal mask pour out      */
#define TTV_SIG_MINSIG     ((long) 0x04000000 ) /* marquage signaux d'entree */
#define TTV_SIG_MOUTSIG    ((long) 0x08000000 ) /* marquage signaux sortie   */

/* type user des signaux */
#define TTV_SIG_INDEX      ((long) 0x00000001 ) /* index du signal           */
#define TTV_CMD_INDEX      ((long) 0x00000002 ) /* index de la command       */
#define TTV_SIG_CMD        ((long) 0x00000003 ) /* cmd pour L et R           */
#define TTV_SIG_CMDNEW     ((long) 0x00000004 ) /* nouveau cmd ttvfig pere   */
#define TTV_SIG_CMDOLD     ((long) 0x00000005 ) /* ancien cmd ttvfig fils    */
#define TTV_SIG_INSIG      ((long) 0x00000006 ) /* liste signaux d'entree    */
#define TTV_SIG_OUTSIG     ((long) 0x00000007 ) /* liste signaux de sortie   */
#define TTV_SIG_CLOCK      ((long) 0x00000008 ) /* signal clock              */
#define TTV_SIG_ASYNCHRON  ((long) 0x00000009 ) /* asynchronous reset        */
#define TTV_SIG_CAPAS      ((long) 0x0000000a ) /* contient les 6 capaval    */
#define TTV_SIG_MULTICYCLE_OUT ((long) 0x0000000b ) /* regles multicycle avec le 'out' matchant le signal*/
#define TTV_SIG_MULTICYCLE_IN  ((long) 0x0000000c ) /* regles multicycle avec le 'in' matchant le signal*/
//#define TTV_SIG_PATH_DELAY_MARGINS ((long) 0x0000000d ) /* marquage pour le delay margin */
#define TTV_SIG_FLAGS     ((long) 0x0000000e ) /* marquage pour le no cross */
#define TTV_SIG_MARKED    ((long) 0x0000000f ) /* marquage pour le no cross */
#define TTV_SIG_OUTPUT_CAPACITANCE ((long) 0x00000010 ) // capaout/signal
#define TTV_SIG_DONE_PTYPE ((long) 0x00000011 ) // capaout/signal
#define TTV_SIG_PATH_DELAY_MARGINS_REGEX ((long) 0x00000012 ) /* marquage pour le delay margin */
#define TTV_SIG_TMP_MARKED    ((long) 0x00000013 ) /* marquage temporaire */
#define TTV_SIG_UTD_OUTPUT_CAPA 0x00000014 /* UDT output capacitance */
#define TTV_SIG_STRICT_SETUP 0x00000015 /* UDT output capacitance */
#define TTV_SIG_DIRECTIVES 0x00000016
#define TTV_SSTA_SIG_INDEX 0x00000017
#define TTV_SIG_FALSEPATH_START 0x00000018
#define TTV_SIG_FALSEPATH_END 0x00000019
#define TTV_SIG_SWING   0x00000020
#define TTV_SIG_VCDPOWER 0x00000021

/* type des evenements */
#define TTV_NODE_MARQUE        ((long) 0x10000000 ) /* noeud marque              */
#define TTV_NODE_CMDMARQUE     ((long) 0x20000000 ) /* cmd marque Pour L         */
#define TTV_NODE_NODE          ((long) 0x40000000 ) /* noeud marque              */
#define TTV_NODE_ROOT          ((long) 0x80000000 ) /* noeud marque              */
#define TTV_NODE_UP            ((long) 0x00000001 ) /* noeud niveau haut         */
#define TTV_NODE_DOWN          ((long) 0x00000002 ) /* noeud niveau bas          */
#define TTV_NODE_STOP          ((long) 0x00000100 ) /* noeud point d'arret       */
#define TTV_NODE_IMAX          ((long) 0x00000200 ) /* noeud fact pour max       */
#define TTV_NODE_IMIN          ((long) 0x00000400 ) /* noeud fact pour min       */
#define TTV_NODE_ONLYEND       ((long) 0x00000800 ) /* pas de front              */
#define TTV_NODE_END           ((long) 0x00002000 ) /* fin de chaine             */
#define TTV_NODE_INR           ((long) 0x00004000 ) /* fin de chaine             */
#define TTV_NODE_INS           ((long) 0x00008000 ) /* fin de chaine             */
#define TTV_NODE_INHZ          ((long) 0x00000020 ) /* fin de chaine             */
#define TTV_NODE_BYPASSOUT     ((long) 0x00000040 ) /* pas de front              */
#define TTV_NODE_BYPASSIN      ((long) 0x00000080 ) /* pas de front              */
#define TTV_NODE_FALSEOUT      ((long) 0x00000004 ) /* pas de chemin             */
#define TTV_NODE_FALSEIN       ((long) 0x00000008 ) /* pas de chemin             */
#define TTV_NODE_MARK_1        ((long) 0x01000000 ) /* marquage temporaire 1     */
#define TTV_NODE_FALSEPATHNODE ((long) 0x02000000 ) /* marquage temporaire 1     */

/* type user des evenements */
#define TTV_NODE_DUALLINE  ((long) 0x00000001 ) /* liens dual                */
#define TTV_NODE_DUALPATH  ((long) 0x00000002 ) /* chemins dual              */
#define TTV_NODE_INCHAIN   ((long) 0x00000003 ) /* entree vue par un noeud   */
#define TTV_NODE_IN        ((long) 0x00000004 ) /* noeud d'entree            */
#define TTV_NODE_AVNODE    ((long) 0x00000005 ) /* noeud precedent           */
#define TTV_NODE_DELAYLOOP ((long) 0x00000006 ) /* delay de bascule          */
#define TTV_NODE_CMDFF     ((long) 0x00000007 ) /* command towards flipflop  */
#define TTV_NODE_CMDLL     ((long) 0x00000008 ) /* command towards latch     */
#define TTV_NODE_DELAY     ((long) 0x00000009 ) /* command towards latch     */
#define TTV_NODE_LATCH     ((long) 0x00000010 ) /* command towards latch     */
#define TTV_NODE_CLOCK     ((long) 0x00000011 ) /* command towards latch     */
#define TTV_NODE_PROPINFO  ((long) 0x00000012 ) /* propagated informations   */
#define TTV_NODE_UTD_INPUT_SLOPE ((long) 0x00000013 ) /* UTD input slope */
#define TTV_NODE_TMP_MARKED   ((long) 0x00000014 ) /* marquage temporaire */
#define TTV_NODE_PROPINFO_CACHE  ((long) 0x00000015 ) /* propagated informations   */
#define TTV_NODE_CRITIC_PARA_INFO ((long) 0x00000016 ) 
#define TTV_NODE_VCD_INFO ((long) 0x00000017 )

/* type des finds */
#define TTV_FIND_INFIFO    ((long) 0x00000001 ) /* noeud deja empile         */
#define TTV_FIND_OUTFIFO   (~(TTV_FIND_INFIFO)) /* noeud non-empile          */
#define TTV_FIND_BEGIN     ((long) 0x00000002 ) /* debut de chaine           */
#define TTV_FIND_BYPASS    ((long) 0x00000004 ) /* bypass du node */

/* type des lines */
#define TTV_LINE_P         ((long) 0x00000001 ) /* lien de type P            */
#define TTV_LINE_J         ((long) 0x00000002 ) /* lien de type J            */
#define TTV_LINE_T         ((long) 0x00000004 ) /* lien de type T            */
#define TTV_LINE_F         ((long) 0x00000008 ) /* lien de type F            */
#define TTV_LINE_E         ((long) 0x00000010 ) /* lien de type E            */
#define TTV_LINE_D         ((long) 0x00000020 ) /* lien de type D            */
#define TTV_LINE_TYPE      ((long) 0x0000003F ) /* lien de type E            */

/* index des delay des line */
#define TTV_DELAY_P        0                    /* position du delay P       */
#define TTV_DELAY_J        1                    /* position du delay J       */
#define TTV_DELAY_T        2                    /* position du delay T       */
#define TTV_DELAY_F        3                    /* position du delay F       */
#define TTV_DELAY_E        4                    /* position du delay E       */
#define TTV_DELAY_D        5                    /* position du delay D       */
#define TTV_DELAY_END      (ttvdelay_list*)-1   /* fin du tableau            */

/* marquage des lines */
#define TTV_LINE_MARQUE    ((long) 0x10000000 ) /* lien marque               */
#define TTV_LINE_FR        ((long) 0x20000000 ) /* lien libre                */
#define TTV_LINE_INFIG     ((long) 0x40000000 ) /* lien dans la figure       */
#define TTV_LINE_HZ        ((long) 0x00000100 ) /* lien de type HZ           */
#define TTV_LINE_EV        ((long) 0x00000200 ) /* lien de type evaluation   */
#define TTV_LINE_PR        ((long) 0x00000400 ) /* lien de type precharge    */
#define TTV_LINE_RC        ((long) 0x00000800 ) /* lien de type rc           */
#define TTV_LINE_DEPTMAX   ((long) 0x00001000 ) /* lien D ou E dans un PT M  */
#define TTV_LINE_DEPTMIN   ((long) 0x00002000 ) /* lien D ou E dans un PT m  */
#define TTV_LINE_DENPTMAX  ((long) 0x00004000 ) /* lien D ou E pas dans PT M */
#define TTV_LINE_DENPTMIN  ((long) 0x00008000 ) /* lien D ou E pas dans PT m */
#define TTV_LINE_PREV      ((long) 0x00010000 ) /* lien avec un ptype prev   */
#define TTV_LINE_ROOT      ((long) 0x00020000 ) /* premier lien d'une liste  */
#define TTV_LINE_PREV_DUAL ((long) 0x00040000 ) /* lien avec un ptype prev   */
#define TTV_LINE_ROOT_DUAL ((long) 0x00080000 ) /* premier lien d'une liste  */
#define TTV_LINE_R         ((long) 0x00000040 ) /* lien reset dans une rs    */
#define TTV_LINE_S         ((long) 0x00000080 ) /* lien set dans une rs      */
#define TTV_LINE_A         ((long) 0x00900000 ) /* lien de type access       */
#define TTV_LINE_U         ((long) 0x00a00000 ) /* lien de type setup        */
#define TTV_LINE_O         ((long) 0x00c00000 ) /* lien de type hold         */
#define TTV_LINE_CONT      ((long) 0x00800000 ) /* lien de type contrainte   */
#define TTV_LINE_NOINDEX   ((int ) 0x80000000 ) /* lien sans index           */

/* type des lines */
#define TTV_LINE_SETUP     ((long) 0x00000001 ) /* lien cont setup           */
#define TTV_LINE_HOLD      ((long) 0x00000002 ) /* lien cont hold            */
#define TTV_LINE_ACCESS    ((long) 0x00000004 ) /* lien cont access          */
#define TTV_LINE_REMOVAL   ((long) 0x00000008 ) /* lien cont removal         */
#define TTV_LINE_RECOVRY   ((long) 0x00000010 ) /* lien cont recovry         */
#define TTV_LINE_SKEW      ((long) 0x00000020 ) /* lien cont skew            */
#define TTV_LINE_WIDTH     ((long) 0x00000040 ) /* lien cont width           */
#define TTV_LINE_NEW       ((long) 0x00000080 ) /* lien nouveau              */

/* type user des lines */
#define TTV_LINE_CMDMAX    ((long) 0x00000001 ) /* command de lien pour max  */
#define TTV_LINE_CMDMIN    ((long) 0x00000002 ) /* command de lien pour min  */
#define TTV_LINE_PREVLINE  ((long) 0x00000003 ) /* lien precedant pour free  */
#define TTV_LINE_PREVLDUAL ((long) 0x00000004 ) /* lien dual precedant       */
#define TTV_LINE_INSNAME   ((long) 0x00000005 ) /* lien dual precedant       */
#define TTV_LINE_MODNAME   ((long) 0x00000006 ) /* lien dual precedant       */
#define TTV_LINE_CONTR     ((long) 0x00000007 ) /* lien de type contrainte   */
#define TTV_SSTA_LINE_INDEX 0x00000008

/* type de recherche de chemin */
#define TTV_FIND_MARQUE    ((long) 0x10000000 ) /* marquage de la recherche  */
#define TTV_FIND_MAX       ((long) 0x00000001 ) /* recherche des chemins max */
#define TTV_FIND_MIN       ((long) 0x00000002 ) /* recherche des chemins mix */
#define TTV_FIND_LINE      ((long) 0x00000004 ) /* recherche avec liens      */
#define TTV_FIND_PATH      ((long) 0x00000008 ) /* recherche avec chemins    */
#define TTV_FIND_HZ        ((long) 0x00000010 ) /* recherche des chemins HZ  */
#define TTV_FIND_PR        ((long) 0x00000020 ) /* recherche des chemins PR  */
#define TTV_FIND_EV        ((long) 0x00000040 ) /* recherche des chemins HZ  */
#define TTV_FIND_HIER      ((long) 0x00000080 ) /* recherche stop a CB et CT */
#define TTV_FIND_DUAL      ((long) 0x00000100 ) /* recherche avec liens dual */
#define TTV_FIND_CRITIC    ((long) 0x00000200 ) /* recherche avec critic     */
#define TTV_FIND_NOTSAVE   ((long) 0x00000400 ) /* recherche sans sauvegarde */
#define TTV_FIND_NOTCLASS  ((long) 0x00000800 ) /* recherche sans classement */
#define TTV_FIND_FACTLOC   ((long) 0x00001000 ) /* factorisation local       */
#define TTV_FIND_FACTGLO   ((long) 0x00002000 ) /* factorisation global      */
#define TTV_FIND_R         ((long) 0x00004000 ) /* s'arrete aux commandes    */
#define TTV_FIND_S         ((long) 0x00008000 ) /* s'arrete aux commandes    */
#define TTV_FIND_CMD       ((long) 0x00010000 ) /* s'arrete aux commandes    */
#define TTV_FIND_MASKOR    ((long) 0x00020000 ) /* recherche mask or         */
#define TTV_FIND_MASKAND   ((long) 0x00040000 ) /* recherche mask and        */
#define TTV_FIND_MASKNOT   ((long) 0x00080000 ) /* recherche mask not        */
#define TTV_FIND_NOT_UPUP  ((long) 0x00100000 ) /* recherche chemin up up    */
#define TTV_FIND_NOT_UPDW  ((long) 0x00200000 ) /* recherche chemin up dw    */
#define TTV_FIND_NOT_DWUP  ((long) 0x00400000 ) /* recherche chemin dw up    */
#define TTV_FIND_NOT_DWDW  ((long) 0x00800000 ) /* recherche chemin dw dw    */
#define TTV_FIND_RC        ((long) 0x01000000 ) /* recherche de delay rc     */
#define TTV_FIND_GATE      ((long) 0x02000000 ) /* recherche de delay porte  */
#define TTV_FIND_SIG       ((long) 0x04000000 ) /* recherche avec des signaux*/
#define TTV_FIND_CARAC     ((long) 0x08000000 ) /* chemin caracterisees      */
#define TTV_FIND_LUT       ((long) 0x20000000 ) /* chemin avec lut           */
#define TTV_FIND_DELAY     ((long) 0x40000000 ) /* chemin avec delay pire    */
#define TTV_FIND_LINETOP   ((long) 0x80000000 ) /* chemin avec delay pire    */
#define TTV_FIND_ALL       ((long) 0x00001000 ) /* recherche tout les chemins*/
#define TTV_FIND_ACCESS    ((long) 0x00002000 ) /* recherche des access      */
#define TTV_FIND_SETUP     ((long) 0x00004000 ) /* recherche des setup       */
#define TTV_FIND_HOLD      ((long) 0x00008000 ) /* recherche des hold        */
#define TTV_FIND_PATHDET   ((long) 0x01000000 ) /* recherche de detail de ch */

/* type de fichier pour le driver et le parser */
#define TTV_FILE_TTX       ((long) 0x00000001 ) /* fichier ttx               */
#define TTV_FILE_DTX       ((long) 0x00000002 ) /* fichier dtx               */
#define TTV_FILE_TTV       ((long) 0x00000004 ) /* fichier ttv               */
#define TTV_FILE_DTV       ((long) 0x00000008 ) /* fichier dtv               */
#define TTV_FILE_INF       ((long) 0x00000010 ) /* parse le fichier inf      */
#define TTV_FILE_CTX       ((long) 0x00000020 ) /* parse le fichier inf      */
#define TTV_FILE_MARQUE    ((long) 0x10000000 ) /* marquage                  */

/* delai et front des evenements et des liens */
#define TTV_NOTIME         (LONG_MIN          ) /* pas de temps              */
#define TTV_NOSLOPE        (LONG_MIN          ) /* pas de front              */
#define TTV_NORES          ((long) 0x00000000 ) /* pas de resistance         */
#define TTV_NOS            (LONG_MIN          ) /* pas de s                  */
#define TTV_NOCAPA         (-1.0e5            ) /* pas de capa               */

/* type de calcul des delais */
#define TTV_MODE_LINE      'L'                  /* sauvegarde dans la line   */
#define TTV_MODE_DELAY     'D'                  /* sauvegarde dans le delay  */
#define TTV_MODE_NOTSTORE  'S'                  /* pas de sauvegarde         */

/* type de point d'arret */
#define TTV_SIG_BEGINSEARCH  ((long) 0x00000001 )
#define TTV_SIG_ENDSEARCH    ((long) 0x00000002 )
#define TTV_SIG_BEGINPATH    ((long) 0x00000004 )
#define TTV_SIG_ENDPATH      ((long) 0x00000008 )
#define TTV_SIG_CROSS        ((long) 0x00000010 )

/* type de recherche */
#define TTV_SEARCH_CRITIC     'C' 
#define TTV_SEARCH_PATH       'p' 
#define TTV_SEARCH_CRITICPATH 'P'

/* type user des paths */
#define TTV_PATH_PREVIOUS_NODE 0x00000001 /* node before path end, use only with stb */
#define TTV_PATH_PERIOD_CHANGE 0x00000002 /* period change due to filters */
#define TTV_PATH_OUTPUT_PHASE  0x00000003
#define TTV_PATH_END_VT        0x00000004
#define TTV_PATH_PHASE_INFO    0x00000005

/* type user des critic */
#define TTV_CRITIC_TABMC       0x00000001

/* pseudo fonction */
#define TTV_LINETYPE(line)   (((line->TYPE & TTV_LINE_RC) == TTV_LINE_RC) ? \
                             TTV_FIND_RC : (long)0)

/* variables */
extern int TTV_MARK_MODE;
#define TTV_MARK_MODE_DO  1
#define TTV_MARK_MODE_USE 2

// mask pour le champ clock_flag de ttvcritic
#define TTV_NODE_FLAG_ISCLOCK         1
#define TTV_NODE_FLAG_FIRST_OF_DETAIL 2
#define TTV_NODE_FLAG_ISLATCH_ACCESS  4
#define TTV_NODE_FLAG_NOALLOC         8

#define TTV_SIG_GATE_CONFLICT            90
#define TTV_SIG_GATE_TRISTATE            91
#define TTV_SIG_GATE_TRISTATE_CONFLICT   92
#define TTV_SIG_GATE_DUAL_CMOS           93
#define TTV_SIG_GATE_MEMSYM              94
#define TTV_SIG_GATE_NAND_RS             95
#define TTV_SIG_GATE_NOR_RS              96
#define TTV_SIG_GATE_RS                  97
#define TTV_SIG_GATE_LATCH               98
#define TTV_SIG_GATE_FLIPFLOP            99

// mask pour les flags de signaux
#define TTV_SIG_FLAGS_DONTCROSS 0x1
#define TTV_SIG_FLAGS_ISDATA    0x2
#define TTV_SIG_FLAGS_ISSLAVE   0x4
#define TTV_SIG_FLAGS_ISMASTER  0x8
//#define TTV_SIG_FLAGS_ISSET     0x10
//#define TTV_SIG_FLAGS_ISRESET   0x20
#define TTV_SIG_FLAGS_TRANSPARENT 0x40

#define TTV_FINDSTRUCT_FLAGS_NOFILTERMAX 1
#define TTV_FINDSTRUCT_FLAGS_FORCEDTRANSPARENCY 2
#define TTV_FINDSTRUCT_FLAGS_MUSTBECLOCK 4
#define TTV_FINDSTRUCT_FLAGS_ISIDEAL 8
#define TTV_FINDSTRUCT_FLAGS_LAST_CROSSED_IS_PRECH 16
#define TTV_FINDSTRUCT_CROSS_ENABLED 32

#define TTV_TPS_FINDSTRUCT_THRU_FILTER_NOCROSS 1

/* STRUCTURES DE TTV */

typedef struct ttvnode_vcd_info
{
  struct ttvsig             *PREVSIG;
  int                        TIME;
  float                      SLOPE;
} ttvnode_vcd_info ;

typedef struct ttvtrans_vcd_power
{
  struct ttvtrans_vcd_power *NEXT;
  struct ttvtrans_vcd_power *PREV;
  struct ttvsig             *SIG;
  int                        TIME;
  float                      NRJ;
} ttvtrans_vcd_power ;

typedef struct {
  int   NB ;
  long *DELAY ;
  long *SLOPE ;
} ttvcriticmc ;

typedef struct ttvfind_stb_stuff
{
  struct ttvfind_stb_stuff *NEXT;
  char PHASE, STARTPHASE;
  char FLAGS;
  long CROSSDELAY ;          /* instant de transparence */
  long NOMORECROSSDELAY ;    /* instant de fin de transparence */
  long IDEAL_CROSSDELAY ;          /* instant de transparence */
  long IDEAL_NOMORECROSSDELAY ;    /* instant de fin de transparence */
  long CROSSDELAY_MIN ;      /* delay min qui peut crosser*/
  long PERIOD_CHANGE;
} ttvfind_stb_stuff;

typedef struct ttvpath_stb_stuff
{
  struct ttvpath_stb_stuff *NEXT;
  char STARTPHASE;
  long STARTTIME ;
  long PERIOD_CHANGE;
} ttvpath_stb_stuff;

typedef struct ttvfind                          /* evenement d'un signal     */
{
  struct ttvfind      *NEXT ;                /* pointeur sur le suivant   */
  long                 TYPE ;                /* type dans la fifo         */
  long                 DELAY ;               /* delay pour les parcours   */
  struct ttvline      *OUTLINE ;             /* retour de chemin          */
  char                 FIFO ;                /* fifo level                */
  char                 THRU_FILTER;
  char                 FLAGS;
  char PERIODE ;             /* phase max d'origine       */
  char ORGPHASE ;            /* phase min d'origine       */
//  char ORGPHASE, PHASE ;            /* phase min d'origine       */
  ttvfind_stb_stuff   *STB;
}
ttvfind_list ;

typedef struct ttvevent                         /* evenement d'un signal     */
    {
     struct ttvsig       *ROOT ;                /* pointeur sur le signal    */
     long                 TYPE ;                /* type de signal            */
     struct ttvline      *INLINE ;              /* lien de l'evenement       */
     struct ttvline      *INPATH ;              /* chemin de l'evenement     */
     struct ttvfind      *FIND ;                /* info de recherche         */
     ptype_list          *USER ;                /* champs utilisateur        */ 
    }
ttvevent_list ;

typedef struct ttvsig                           /* signal pour le timing     */
    {
     char                *NAME ;                /* nom du signal             */
     char                *NETNAME ;             /* nom du net                */
     float                CAPA ;                /* capacite associe          */
     struct ttvfig       *ROOT ;                /* ttvfig du signal          */
     long                 TYPE ;                /* type de signal            */
     struct ttvevent      NODE[2] ;             /* 2 evenement du signal     */
     ptype_list        *USER ;                /* champs utilisateur        */ 
     int PNODE[2];  // pas dans le champ NODE pour gagner en 64bits
    }
ttvsig_list ;

typedef struct ttvsig_capas             
    {
     float                CAPAUP;      
     float                CAPAUPMIN;  
     float                CAPAUPMAX; 
     float                CAPADN;   
     float                CAPADNMIN;   
     float                CAPADNMAX;   
    }ttvsig_capas;

typedef struct ttvsbloc                         /* liste de blocs de signaux */
    {
     struct ttvsbloc     *NEXT ;                /* next pour liste chainee   */
     struct ttvsig        SIG[TTV_MAX_SBLOC] ;  /* tableau de signaux        */
    }
ttvsbloc_list ;

typedef struct ttvdelay                        /* delay modifier             */
    {
     long                 VALMAX ;             /* valeur du temps max        */
     long                 FMAX ;               /* valeur du front max        */
     long                 VALMIN ;             /* valeur du temps min        */
     long                 FMIN ;               /* valeur du front min        */
     float                CMAX ;               /* valeur de capa max         */
     float                CMIN ;               /* valeur de capa min         */
     float                RDRIVERMAX ;
     float                RDRIVERMIN ;
     float                CDRIVERMAX ;
     float                CDRIVERMIN ;
     ptype_list          *USER ;
    }
ttvdelay_list ;


typedef struct ttvline                         /* lien entre evenement       */
    {
     struct ttvline      *NEXT ;               /* champs next pour liste     */
     struct ttvfig       *FIG ;                /* ttvfif pere du lien        */
     struct ttvevent     *ROOT ;               /* evenement destination      */
     long                 TYPE ;               /* type de lien               */
     int                  INDEX ;              /* index du lien              */
     struct ttvevent     *NODE ;               /* evenement source           */
     long                 VALMAX ;             /* valeur du temps max        */
     long                 FMAX ;               /* valeur du front max        */
     long                 VALMIN ;             /* valeur du temps min        */
     long                 FMIN ;               /* valeur du front min        */
     char                *MDMAX ;              /* modele de temps max        */
     char                *MDMIN ;              /* modele de temps min        */
     char                *MFMAX ;              /* modele de temps max        */
     char                *MFMIN ;              /* modele de temps min        */
     ptype_list        *USER ;               /* champs utilisateur         */
    }
ttvline_list ;

typedef struct ttv_onepropinfo
{
  long                 DELAY ;
  long                 SLEW ;
  stm_pwl              *PWL;
  long                 FIND_DELAY;
  float                R ;
  float                C1 ;
  float                C2 ;
  float                RDRIVER ;
  float                CAPA ;
  float                ENERGY ;
} ttv_onepropinfo;

typedef struct ttvpropinfo
{
  ttv_onepropinfo      CTK, *REF;
  ttvline_list        *PREVIOUS_LINE ;
}
ttvpropinfo_list ;

typedef struct ttvlbloc                        /* liste de blocs de liens    */
    {
     struct ttvlbloc     *NEXT ;               /* next pour liste chainee    */
     struct ttvline       LINE[TTV_MAX_LBLOC] ;/* tableau de liens           */
    }
ttvlbloc_list ;

typedef struct ttvcritic                       /* detail de chemin           */
    {
     struct ttvcritic    *NEXT ;               /* noeud suivant              */
     struct ttvfig       *FIG ;                /* figure du delay            */
     char                *NAME ;               /* nom du noeud               */
     char                *NETNAME ;            /* nom du signal              */
     char                *MODNAME ;            /* nom du model               */
     char                *LINEMODELNAME;            /* nom du model               */
     char                *INSNAME ;            /* nom de l'instance          */
     long                 TYPE ;               /* type de recherche          */
     long                 SIGTYPE ;            /* type de signal             */
     long                 DATADELAY ;          /* delay de la data           */
     long                 DELAY ;              /* delay                      */
     long                 SLOPE ;              /* front                      */
     long                 REFDELAY ;           /* delay                      */
     long                 REFSLOPE ;           /* front                      */
     long                 SIMDELAY ;           /* delay                      */
     long                 SIMSLOPE ;           /* front                      */
     float                CAPA ;               /* nom du noeud               */
     float                OUTPUT_CAPA ;        /* nom du noeud               */
     long                 PROP;               /* propriete                  */
     ptype_list          *USER ;              /*  champs utilisateur         */
     ttvevent_list       *NODE ;              /*  beware, use only in stb or non cache   */
     ttvline_list        *LINE ;              /*  beware, use only in stb or non cache   */
     int                  PNODE;               /* node on the rc network */
     char                 SNODE ;              /* type de front              */
     char                 NODE_FLAG;          /* flag de clock              */
    }
ttvcritic_list ;

typedef struct ttvpath                         /* detail de chemin           */
    {
     struct ttvpath      *NEXT ;               /* noeud suivant              */
     struct ttvfig       *FIG ;                /* figure du delay            */
     struct ttvevent     *ROOT ;               /* noeud d'arrive             */
     struct ttvevent     *NODE ;               /* noeud de depart            */
     struct ttvevent     *CMD ;                /* command                    */
     struct ttvevent     *LATCH ;              /* latch                      */
     struct ttvevent     *CMDLATCH ;           /* latch                      */
     struct ttvcritic    *CRITIC ;             /* detai du chemin            */
     ptype_list          *LATCHLIST ;          /* latch list                 */
     long                 DATADELAY ;          /* stabilite de la data       */
     long                 ACCESS ;             /* access                     */
     long                 REFACCESS ;          /* access                     */
     long                 TYPE ;               /* type de recherche          */
     long                 DELAY ;              /* delay                      */
     long                 SLOPE ;              /* front                      */
     long                 REFDELAY ;           /* delay                      */
     long                 REFSLOPE ;           /* front                      */
     long                 DELAYSTART ;         /* instant de debut           */
     long                 SLOPESTART ;         /* front de debut             */
     timing_model        *MD ;                 /* Model delay                */
     timing_model        *MF ;                 /* Model front                */
     long                 CROSSMINDELAY;       /* delai min qui peut crosser */
     ptype_list          *USER ;              /*  champs utilisateur         */
     long                 TTV_MORE_SEARCH_OPTIONS;
     long                 CLOCKPATHDELAY;
     int                  NBMC ;               /* nombre de monte carlo      */
     char                 PHASE;
    }
ttvpath_list ;

typedef struct ttvinfo                        /* information sur la ttvfig   */
    {
     const char          *TOOLNAME ;          /* outil tas ou htas           */
     const char          *TOOLVERSION ;       /* version de l'outil          */
     char                *TECHNONAME ;        /* technologie elp             */
     char                *TECHNOVERSION ;     /* version de la technologie   */
     char                *FIGNAME ;           /* nom de modele               */
     struct ttvfig       *MODEL ;             /* model de figure             */
     chain_list          *INSTANCES ;         /* liste des intances          */
     long                 LEVEL ;             /* niveau hierarchique         */
     int                  TTVYEAR ;           /* annee de generation         */
     int                  TTVMONTH ;          /* mois de generation          */
     int                  TTVDAY ;            /* jour de generation          */
     int                  TTVHOUR ;           /* heure de generation         */
     int                  TTVMIN ;            /* minute de generation        */
     int                  TTVSEC ;            /* seconde de generation       */
     long                 SLOPE ;             /* front d'entree utilser      */
     float                CAPAOUT ;           /* capacite utiliser en sortie */
     float                STHHIGH ;           /* capacite utiliser en sortie */
     float                STHLOW ;            /* capacite utiliser en sortie */
     float                DTH ;               /* capacite utiliser en sortie */
     float                TEMP ;              /* temperature                 */
     float                TNOM ;              /* temperature nominal         */
     float                VDD ;               /* capacite utiliser en sortie */
     ptype_list          *USER ;              /*  champs utilisateur         */
     char                *FILENAME ;          /* nom du fichier              */
     int                  DTB_VERSION;
    }
ttvinfo_list ;

typedef struct ttvfig                         /* figure ttv                  */
    {
     struct ttvfig       *NEXT ;              /* ttvfig suivante de la liste */
     struct ttvfig       *ROOT ;              /* ttvfig pere de la ttvfig    */
     long                 OLD ;               /* age de la ttvfig            */
     char                *INSNAME ;           /* nom d'instance              */
     long                 STATUS ;            /* etat de la ttvfig           */
     struct ttvinfo      *INFO ;              /* information sur la ttvfig   */
     struct ttvsig      **CONSIG ;            /* tableau des connecteurs     */
     long                 NBCONSIG ;          /* nb de connecteurs           */
     struct ttvsig      **NCSIG ;             /* tableau des extremites RC   */
     long                 NBNCSIG ;           /* nb des extremites RC        */
     struct chain        *INS ;               /* liste des instances         */
     struct ttvsig      **ELCMDSIG ;          /* tableau des commands ext    */
     long                 NBELCMDSIG ;        /* nb de commands ext          */
     struct ttvsig      **ILCMDSIG ;          /* tableau des commands int    */
     long                 NBILCMDSIG ;        /* nb de commands int          */
     struct ttvsig      **ELATCHSIG ;         /* tableau des latchs ext      */
     long                 NBELATCHSIG ;       /* nb de latchs ext            */
     struct ttvsig      **ILATCHSIG ;         /* tableau des latchs int      */
     long                 NBILATCHSIG ;       /* nb de latchs int            */
     struct ttvsig      **EBREAKSIG ;         /* tableau des breaks ext      */
     long                 NBEBREAKSIG ;       /* nb de breaks ext            */
     struct ttvsig      **IBREAKSIG ;         /* tableau des breaks int      */
     long                 NBIBREAKSIG ;       /* nb de breaks int            */
     struct ttvsig      **EPRESIG ;           /* tableau des precharges ext  */
     long                 NBEPRESIG ;         /* nb de precharges ext        */
     struct ttvsig      **IPRESIG ;           /* tableau des precharges int  */
     long                 NBIPRESIG ;         /* nb de precharges int        */
     struct ttvsig      **EXTSIG ;            /* tableau des signaux exter   */
     long                 NBEXTSIG ;          /* nb de signaux exter         */
     struct ttvsig      **INTSIG ;            /* tableau des signaux inter   */
     long                 NBINTSIG ;          /* nb de signaux inter         */
     struct ttvsig      **ESIG ;              /* tableau des signaux externe */
     long                 NBESIG ;            /* nb de signaux externe       */
     struct ttvsbloc     *ISIG ;              /* tableau des signaux interne */
     long                 NBISIG ;            /* nb de signaux interne       */
     struct ttvlbloc     *PBLOC ;             /* tableau de chemin complet   */
     long                 NBPBLOC ;           /* nb de chemin complet        */
     struct ttvlbloc     *JBLOC ;             /* tableau de chemin sur T     */
     long                 NBJBLOC ;           /* nb de chemin sur T          */
     struct ttvlbloc     *TBLOC ;             /* tableau de chemin incomplet */
     long                 NBTBLOC ;           /* nb de chemin incomplet      */
     struct ttvlbloc     *FBLOC ;             /* tableau de liens interne    */
     long                 NBFBLOC ;           /* nb de liens interne         */
     struct ttvlbloc     *EBLOC ;             /* tableau de liens complet    */
     long                 NBEBLOC ;           /* nb de liens complet         */
     struct ttvlbloc     *DBLOC ;             /* tableau de liens incomplet  */
     long                 NBDBLOC ;           /* nb de liens incomplet       */
     struct ttvdelay   ***DELAY ;             /* delay modifie               */
     ptype_list        *USER ;              /* champs utilisateur          */
     unsigned int SIGN;
    }
ttvfig_list ;

typedef struct
{
  int RuleNumber;
  float Multiplier;
  long Flags; // inf miscdata->lval
} ttv_MultiCycleInfo;

#define TTV_MULTICYCLE_ANYINPUT    0x1000000 /* a eviter: les collisions avec les flags multicycle de inf*/

typedef struct stb_directive
{
  struct stb_directive *next;
  ttvsig_list *target2;
  char *from;
  long margin;
  char filter, target1_dir, target2_dir, operation;
} ttv_directive;

typedef struct
{
  int nb, nbsig;
  int nbpvt, key;
  unsigned int globalseed;
  unsigned int mainseed;
} ssta_entry_header;

typedef struct
{
  int delaymin;
  int slopemin;
  int delaymax;
  int slopemax;
} ssta_entry_values;

typedef struct
{
  float min, max;
  double moy, var;
} ttv_line_delay_ssta_info;

typedef struct
{
  ttv_line_delay_ssta_info delaymax, delaymin;
} ttv_line_ssta_info;

typedef struct
{
  float low, high;
} ttv_swing_info;
/* VARIABLES DE TTV */

extern ttvfig_list *TTV_LIST_TTVFIG ;
extern chain_list *TTV_HEAD_TTVFIG ;
extern long TTV_SEUIL_MAX ;
extern long TTV_SEUIL_MIN ;
extern long TTV_INFO_CAPARM ;
extern long TTV_INFO_CAPASW ;
extern short TTV_INFO_ARGC ;
extern char **TTV_INFO_ARGV ;
extern char TTV_PATH_SAVE ;
extern char *TTV_INFO_TOOLDATE ;
extern chain_list *TTV_NAME_IN ;
extern chain_list *TTV_NAME_OUT ;
extern char TTV_LANG ;
extern long TTV_NUMB_SIG ;
extern long TTV_NUMB_LINE ;
extern long TTV_MAX_SIG ;
extern long TTV_MAX_LINE ;
extern long TTV_MAX_FCYCLE ;
extern float TTV_UNIT;
extern int TTV_QUIET_MODE;
extern long TTV_PROPAGATE_FORCED_SLOPE;
extern float TTV_PROPAGATE_FORCED_CAPA;
/* FONCTION DE TTV */

extern ttvfig_list*       ttv_allocttvfig      __P(( char*,
                                                     char*,
                                                     ttvfig_list*
                                                  ));
extern void               ttv_lockttvfig       __P(( ttvfig_list* ));
extern void               ttv_unlockttvfig     __P(( ttvfig_list* ));
extern int                ttv_freettvfig       __P(( ttvfig_list* ));
extern int                ttv_freettvfiglist   __P(( chain_list* ));
extern void               ttv_freeall          __P(( void ));
extern int                ttv_freeallttvfig    __P(( ttvfig_list* ));
extern int                ttv_freepathlist     __P(( ttvpath_list* ));
extern int                ttv_freecriticlist   __P(( ttvcritic_list* ));
extern ttvsig_list**      ttv_allocreflist     __P(( chain_list*, long ));
extern chain_list*        ttv_chainreflist     __P(( ttvfig_list*,
                                                     ttvsig_list**,
                                                     long 
                                                  ));
extern int                ttv_freereflist      __P(( ttvfig_list*,
                                                     ttvsig_list**,
                                                     long 
                                                  ));
extern void               ttv_addcaracline   __P(( ttvline_list*,
                                                     char *,
                                                     char *,
                                                     char *,
                                                     char * 
                                                  ));
extern int                ttv_calcaracline   __P(( ttvline_list*,
                                                   ttvline_list*,
                                                   long,
                                                   int 
                                                  ));
extern void               ttv_addinfreelist    __P(( ttvfig_list*, long ));
extern int                ttv_delinfreelist    __P(( ptype_list *, ttvfig_list* ));
extern int                ttv_freettvfigmemory __P(( ttvfig_list*, long ));
extern int                ttv_freememoryiffull __P(( ttvfig_list*, long ));
extern int                ttv_freememoryifmax  __P(( ttvfig_list*, long ));
extern void               ttv_fifopush         __P(( ttvevent_list* ));
extern ttvevent_list*     ttv_fifopop          __P(( void ));
extern void               ttv_fifoclean        __P(( void ));
extern void               ttv_fifodelete       __P(( void ));
extern ttvcritic_list*    ttv_getcritic        __P(( ttvfig_list*,
                                                     ttvfig_list*,
                                                     ttvevent_list*,
                                                     ttvevent_list*,
                                                     ttvevent_list*,
                                                     ttvevent_list*,
                                                     long
                                                  ));
extern ttvpath_list*      ttv_getcriticpath    __P(( ttvfig_list*,
                                                     ttvfig_list*,
                                                     chain_list*,
                                                     chain_list*,
                                                     long,
                                                     long,
                                                     int,
                                                     long
                                                  ));
extern ttvpath_list*      ttv_getcriticaccess  __P(( ttvfig_list*,
                                                     ttvfig_list*,
                                                     chain_list*,
                                                     chain_list*,
                                                     chain_list*,
                                                     long,
                                                     long,
                                                     int,
                                                     long
                                                  ));
extern ttvpath_list*      ttv_getpath          __P(( ttvfig_list*,
                                                     ttvfig_list*,
                                                     ttvsig_list*,
                                                     chain_list*,
                                                     long,
                                                     long,
                                                     long
                                                  ));
extern ttvpath_list*      ttv_getpathnocross   __P(( ttvfig_list*,
                                                     ttvfig_list*,
                                                     ttvsig_list*,
                                                     chain_list*,
                                                     long,
                                                     long,
                                                     long
                                                  ));
extern ttvpath_list*      ttv_getaccessnocross   __P(( ttvfig_list*,
                                                     ttvfig_list*,
                                                     ttvsig_list*,
                                                     chain_list*,
                                                     long,
                                                     long,
                                                     long
                                                  ));
extern ttvpath_list*      ttv_getpathsig       __P(( ttvfig_list*,
                                                     ttvfig_list*,
                                                     ttvsig_list*,
                                                     chain_list*,
                                                     long,
                                                     long,
                                                     long
                                                  ));

extern ttvpath_list*      ttv_getpathsignocross __P(( ttvfig_list*,
                                                     ttvfig_list*,
                                                     ttvsig_list*,
                                                     chain_list*,
                                                     long,
                                                     long,
                                                     long
                                                  ));
extern ttvpath_list*      ttv_getaccesssignocross __P(( ttvfig_list*,
                                                     ttvfig_list*,
                                                     ttvsig_list*,
                                                     chain_list*,
                                                     long,
                                                     long,
                                                     long
                                                  ));

extern ttvpath_list*      ttv_getaccess        __P(( ttvfig_list*,
                                                     ttvfig_list*,
                                                     ttvsig_list*,
                                                     chain_list*,
                                                     chain_list*,
                                                     long,
                                                     long,
                                                     long
                                                  ));
extern ttvpath_list*      ttv_getsigaccess     __P(( ttvfig_list*,
                                                     ttvfig_list*,
                                                     ttvsig_list*,
                                                     chain_list*,
                                                     chain_list*,
                                                     long,
                                                     long,
                                                     long
                                                  ));
extern ttvpath_list*      ttv_getallpath       __P(( ttvfig_list*,
                                                     ttvfig_list*,
                                                     chain_list*,
                                                     chain_list*,
                                                     long,
                                                     long,
                                                     int,
                                                     long
                                                  ));
extern ttvpath_list*      ttv_getallaccess     __P(( ttvfig_list*,
                                                     ttvfig_list*,
                                                     chain_list*,
                                                     chain_list*,
                                                     chain_list*,
                                                     long,
                                                     long,
                                                     int,
                                                     long
                                                  ));
extern chain_list*        ttv_getcriticpara    __P(( ttvfig_list*,
                                                     ttvfig_list*,
                                                     ttvevent_list*,
                                                     ttvevent_list*,
                                                     long,
                                                     long,
                                                     chain_list*,
                                                     long
                                                  ));
extern int                ttv_freecriticpara   __P(( chain_list* )) ;
extern ttvpath_list*      ttv_getpara          __P(( ttvfig_list*,
                                                     ttvfig_list*,
                                                     ttvevent_list*,
                                                     ttvevent_list*,
                                                     long,
                                                     long,
                                                     long,
                                                     chain_list*,
                                                     int
                                                  ));
extern chain_list*        ttv_getdelay         __P(( ttvfig_list*,
                                                     ttvfig_list*,
                                                     chain_list*,
                                                     chain_list*,
                                                     long,
                                                     long,
                                                     long
                                                  ));
extern chain_list*        ttv_getconstraint    __P(( ttvfig_list*,
                                                     ttvfig_list*,
                                                     ttvsig_list*,
                                                     long,
                                                     long,
                                                     int
                                                  ));
long ttv_getconstraintquick(ttvfig_list *ttvfig,ttvfig_list *figpath,ttvevent_list *tve,ttvevent_list *cmd,long type, ttvline_list **rline);

extern int                ttv_freedelaylist    __P(( chain_list* ));
extern void               ttv_builtpath        __P(( ttvfig_list*,
                                                     long
                                                  ));
extern char               ttv_getdir           __P(( ttvsig_list* )) ;
extern void ttv_drittv(ttvfig_list *ttvfig, long type, long find, char *filename);

extern void               ttv_drittvold        __P(( ttvfig_list*,
                                                     long,
                                                     long
                                                  ));
extern void               ttv_detectinter      __P(( ttvfig_list*, int )) ;
extern ttvsig_list*       ttv_addsig           __P(( ttvfig_list*,
                                                     char*,
                                                     char*,
                                                     float,
                                                     long
                                                  ));
extern chain_list*        ttv_addrefsig        __P(( ttvfig_list*,
                                                     char *name,
                                                     char*,
                                                     float,
                                                     long,
                                                     chain_list*
                                                  ));
extern long               ttv_gettypesig       __P(( ttvsig_list* ));
extern void               ttv_delrefsig        __P(( ttvsig_list* ));
extern ttvline_list*      ttv_addline          __P(( ttvfig_list*,
                                                     ttvevent_list*,
                                                     ttvevent_list*,
                                                     long,
                                                     long,
                                                     long,
                                                     long,
                                                     long
                                                  ));
extern void               ttv_addconttype     __P(( ttvline_list*,
                                                     long
                                                  ));
extern long               ttv_getconttype     __P(( ttvline_list* ));
extern void               ttv_delconttype     __P(( ttvline_list* ));

extern void               ttv_addmodelline     __P(( ttvline_list*,
                                                      char *,
                                                      char *
                                                   ));
extern void               ttv_getmodelline     __P(( ttvline_list*,
                                                      char **,
                                                      char **
                                                   ));
extern void               ttv_delmodelline     __P(( ttvline_list* ));
extern int                ttv_delline          __P(( ttvline_list* )) ;
extern long               ttv_getlinetype      __P(( ttvline_list*));
extern long               ttv_getnewlinetype   __P(( ttvline_list*,
                                                     ttvsig_list*,
                                                     ttvsig_list*
                                                  ));
extern void               ttv_delcmd           __P(( ttvfig_list*,
                                                     ttvsig_list*
                                                   ));
extern ttvevent_list*     ttv_getcmd           __P(( ttvfig_list*,
                                                     ttvevent_list*
                                                  ));
extern ptype_list*        ttv_addcmd           __P(( ttvline_list*,
                                                     long,
                                                     ttvevent_list*
                                                  ));
extern chain_list*        ttv_getlrcmd         __P(( ttvfig_list*,
                                                     ttvsig_list*
                                                  ));
extern ttvevent_list*     ttv_getlinecmd       __P(( ttvfig_list*, 
                                                     ttvline_list*, 
                                                     long
                                                  ));
extern ttvfig_list*       ttv_gethtmodel      __P(( char *));
extern ttvfig_list*       ttv_getttvfig        __P(( char*,
                                                     long
                                                  ));
extern ttvfig_list*       ttv_givehead         __P(( char*,
                                                     char*,
                                                     ttvfig_list*
                                                  ));
extern ttvfig_list*       ttv_givettvfig       __P(( char*,
                                                     char*,
                                                     ttvfig_list*
                                                  ));
extern ttvfig_list*       ttv_getttvins       __P(( ttvfig_list*,
                                                    char*,
                                                    char*
                                                 ));
extern chain_list*        ttv_getttvfiglist    __P(( ttvfig_list* ));
extern char*              ttv_getinsname       __P(( ttvfig_list*,
                                                     char*,
                                                     ttvfig_list*
                                                  ));
extern char*              ttv_getsigname       __P(( ttvfig_list*,
                                                     char*,
                                                     ttvsig_list*
                                                  ));
extern char*              ttv_getnetname       __P(( ttvfig_list*,
                                                     char*,
                                                     ttvsig_list*
                                                  ));
extern ttvsig_list*       ttv_getsig           __P(( ttvfig_list*,
                                                     char*
                                                  ));
extern ttvsig_list*       ttv_getsigbyhash     __P(( ttvfig_list*,
                                                     char*
                                                  ));
extern ttvsig_list*       ttv_getsigbyname     __P(( ttvfig_list*,
                                                     char*,
                                                     long
                                                  ));
extern ttvsig_list*       ttv_getsigbyinsname  __P(( ttvfig_list*,
                                                     char*,
                                                     long
                                                   ));
extern ttvsig_list*       ttv_getsigbyindex    __P(( ttvfig_list*,
                                                     long,
                                                     long
                                                  ));
extern chain_list*        ttv_getsigbytype     __P(( ttvfig_list*,
                                                     ttvfig_list*,
                                                     long,
                                                     chain_list*
                                                  ));

chain_list *ttv_getsigbytype_and_netname(ttvfig_list *ttvfig,ttvfig_list *ttvins,long type,chain_list *mask);

extern chain_list*        ttv_getsignamelist   __P(( ttvfig_list*,
                                                     ttvfig_list*,
                                                     chain_list*
                                                  ));
extern void               ttv_freenamelist     __P(( chain_list* ));
extern chain_list*        ttv_getallsigbytype  __P(( ttvfig_list*,
                                                     long,
                                                     chain_list*
                                                  ));
extern void               ttv_setsiglevel      __P(( ttvsig_list*, long ));
extern long               ttv_getsiglevel      __P(( ttvsig_list* ));
extern void               ttv_delsiglevel      __P(( ttvfig_list*,
                                                     ttvsig_list*
                                                  ));
extern int                ttv_islinelevel      __P(( ttvfig_list*,
                                                     ttvline_list*,
                                                     long
                                                  ));
extern void               ttv_expfigsig       __P(( ttvfig_list*,
                                                    ttvsig_list*,
                                                    long,
                                                    long,
                                                    long,
                                                    long
                                                 ));
extern FILE *              ttv_openfile       __P((ttvfig_list *,
                                                     long,
                                                     char *
                                                  ));
extern long                ttv_existefile     __P((ttvfig_list *,
                                                   long
                                                 ));
extern ttvfig_list*       ttv_parsttvfig       __P(( ttvfig_list*,
                                                     long,
                                                     long
                                                  ));
extern void               ttv_builthtabfig     __P(( ttvfig_list*, long ));
extern void               ttv_builthtabins     __P(( ttvfig_list*, long ));
extern void               ttv_builthtabttvfig  __P(( ttvfig_list*, long ));
extern void               ttv_freehtabfig      __P(( ttvfig_list*, long ));
extern void               ttv_freehtabttvfig   __P(( ttvfig_list*, long ));

extern char*              ttv_revect           __P(( char* ));
extern char*              ttv_devect           __P(( char* ));
extern int                ttv_testmask         __P(( ttvfig_list*,
                                                     ttvsig_list*,
                                                     chain_list*
                                                  ));
extern int                ttv_jokersubst       __P(( char*, 
                                                     char*, 
                                                     char 
                                                  ));
extern void               ttv_checkfigins      __P(( ttvfig_list*,
                                                     ttvfig_list*
                                                  ));
extern void               ttv_checkallinstool  __P(( ttvfig_list* ));
extern void               ttv_setttvlevel      __P(( ttvfig_list* ));
extern void               ttv_setsigttvfiglevel __P(( ttvfig_list* ));
extern void               ttv_setttvdate        __P(( ttvfig_list*, long ));
extern void               ttv_setcachesize      __P(( double, double ));
void ttv_disablecache(ttvfig_list *ttvfig);
extern double             ttv_getsigcachesize   __P(( void ));
extern double             ttv_getlinecachesize  __P(( void ));
extern double             ttv_getsigmemorysize  __P(( void ));
extern double             ttv_getlinememorysize __P(( void ));
extern void               ttv_cleantagttvfig    __P(( long ));
extern char*              ttv_getstmmodelname   __P(( ttvfig_list*,
                                                      ttvline_list *, 
                                                      int,
                                                      char
                                                   ));
extern int                ttv_islocononlyend    __P(( ttvfig_list *,
                                                      ttvevent_list *,
                                                      long
                                                   )) ;
extern int                ttv_islineonlyend     __P(( ttvfig_list *,
                                                      ttvline_list *,
                                                      long
                                                   )) ;
extern void               ttv_getinffile        __P(( ttvfig_list* ));
extern chain_list*        ttv_getclocklist      __P(( ttvfig_list* ));
chain_list *ttv_getasyncsiglist(ttvfig_list* ttvfig);

extern chain_list*        ttv_getclocksiglist   __P(( ttvfig_list* ));

extern chain_list*        ttv_addlooplist       __P(( ttvfig_list*, 
                                                      chain_list* 
                                                   ));
extern chain_list*        ttv_detectloop        __P(( ttvfig_list*,
                                                       long
                                                   )); 
extern void               ttv_printloop         __P(( int,
                                                      chain_list*,
                                                      char*
                                                   ));
extern int                ttv_existeline        __P(( ttvfig_list*,
                                                      ttvevent_list*,
                                                      ttvevent_list*,
                                                      long
                                                   ));
extern ttvline_list       *ttv_getline          __P(( ttvfig_list*,
                                                      ttvfig_list*,
                                                      ttvevent_list*,
                                                      ttvevent_list*,
                                                      ttvevent_list*,
                                                      long,
                                                      long,
                                                      int,
                                                      int
                                                   ));
extern ttvfig_list*       ttv_builtrefsig       __P(( ttvfig_list* ));
extern ttvdelay_list      *ttv_getlinedelay     __P(( ttvline_list*));
extern ttvdelay_list      *ttv_addlinedelay     __P(( ttvline_list*,
                                                      long,
                                                      long,
                                                      long,
                                                      long,
                                                      float,
                                                      float
                                                   ));
extern void                ttv_dellinedelay     __P(( ttvline_list*));
extern long                ttv_getdelaymax      __P(( ttvline_list*));
extern long                ttv_getdelaymin      __P(( ttvline_list*));
extern long                ttv_getslopemax      __P(( ttvline_list*));
extern long                ttv_getslopemin      __P(( ttvline_list*));
extern int                 ttv_calcnodedelay    __P(( ttvline_list*,
                                                      long,
                                                      long,
                                                      long,
                                                      stm_pwl*,
                                                      long,
                                                      long,
                                                      stm_pwl*
                                                    ));
extern long                ttv_calclinedelayslope   __P(( ttvline_list*,
                                                          long,
                                                          long,
                                                          stm_pwl*,
                                                          stm_pwl*,
                                                          float,
                                                          float,
                                                          float,
                                                          float,
                                                          float,
                                                          float,
                                                          float,
                                                          float,
                                                          float,
                                                          long*,
                                                          long*,
                                                          stm_pwl**,
                                                          long*,
                                                          long*,
                                                          stm_pwl**,
                                                          char,
                                                          float,
                                                          float,
                                                          double*,
                                                          double*,
                                                          double*,
                                                          float,
                                                          float,
                                                          double*,
                                                          double*,
                                                          double*,
                                                          float*,
                                                          float*
                                                       ));
extern void                ttv_calcgatercdelayslope __P(( ttvline_list*,
                                                          ttvline_list*,
                                                          long,
                                                          long,
                                                          float,
                                                          long*,
                                                          long*,
                                                          long*,
                                                          long*,
                                                          char
                                                       ));
extern void            ttv_updatenodedelayslope     __P(( ttvfig_list*,
                                                          long, 
                                                          ttvevent_list*,
                                                          long, 
                                                          char 
                                                       ));
extern long                ttv_calcnodedelayslope   __P(( ttvfig_list*,
                                                          long,
                                                          ttvevent_list *,
                                                          long,
                                                          char 
                                                       ));
extern long                ttv_getdelaynode         __P(( ttvfig_list*,
                                                          long,
                                                          ttvevent_list *,
                                                          long
                                                       ));
extern long                ttv_getslopenode         __P(( ttvfig_list*,
                                                          long,
                                                          ttvevent_list *,
                                                          long,
                                                          long
                                                       ));
extern chain_list         *ttv_levelise             __P(( ttvfig_list *,
                                                          long,
                                                          long
                                                        ));
extern void                ttv_movedelayline        __P(( ttvfig_list *,
                                                          long
                                                       ));
extern void                ttv_cleanfigmodel        __P(( ttvfig_list *,
                                                          long,
                                                          long
                                                       ));
chain_list *ttv_calcfigdelay(ttvfig_list *ttvfig,chain_list *chainnode,long level,long type, char mode, int (*init_func)(ttvevent_list *, void *), int (*end_func)(ttvevent_list *, void *), void *data, int nosonstraint);
extern void                ttv_freefigdelay         __P(( chain_list * ));
extern long                ttv_absmaxdiffslope      __P(( long, 
                                                          long, 
                                                          long
                                                       ));
extern ttvdelay_list      *ttv_getnodedelay         __P(( ttvevent_list* ));
extern void                ttvenv                   __P(( void ));
extern int                 ttv_isemptyttvins        __P(( ttvfig_list *, 
                                                          long 
                                                       ));
extern void                ttv_flatttvfigfromlist   __P(( ttvfig_list *,
                                                          chain_list *,
                                                          long
                                                       ));
extern void                ttv_flatttvfig           __P(( ttvfig_list *,
                                                          chain_list *,
                                                          long
                                                       ));
extern int                 ttv_ctxparse             __P(( ttvfig_list* ));
extern void                ttv_ctxdrive             __P(( ttvfig_list* ));
extern ttvsig_list*        ttv_getttvsig            __P(( ttvfig_list *,
                                                          long,
                                                          long,
                                                          char *,
                                                          losig_list *,
                                                          char
                                                       ));
ttvsig_list*               ttv_getttvsig_sub(ttvfig_list *figdest, losig_list *losig, long level, long type);
extern void                ttv_addsigcapas          __P(( ttvsig_list *ttvsig,
                                                          float cu,
                                                          float cumin,
                                                          float cumax,
                                                          float cd,
                                                          float cdmin,
                                                          float cdmax
                                                       ));
extern ttvsig_capas*       ttv_getsigcapas          __P(( ttvsig_list *ttvsig,
                                                          float *cu,
                                                          float *cumin,
                                                          float *cumax,
                                                          float *cd,
                                                          float *cdmin,
                                                          float *cdmax
                                                       ));
extern void                ttv_getallsigcapas       __P(( ttvsig_list *ttvsig,
                                                          float *cu,
                                                          float *cumin,
                                                          float *cumax,
                                                          float *cd,
                                                          float *cdmin,
                                                          float *cdmax
                                                       ));
extern void                ttv_delsigcapas          __P(( ttvsig_list *ttvsig
                                                       ));
extern void                ttv_setallsigcapas       __P(( lofig_list *lofig,
                                                          losig_list *losig,
                                                          ttvsig_list *ttvsig
                                                       ));
extern int                 ttv_isttvsigdriver       __P(( ttvfig_list *, 
                                                          long,
                                                          long,
                                                          ttvsig_list*   
                                                       ));
extern ttvfig_list*        ttv_getttvinsbyhiername  __P(( ttvfig_list *,
                                                          char *
                                                       ));
extern losig_list*         ttv_getlosigfromevent    __P(( ttvfig_list *,
                                                          ttvsig_list *,
                                                          char **,
                                                          chain_list **,
                                                          lofig_list **
                                                       ));
extern lofig_list         *ttv_getrcxlofig    __P(( ttvfig_list*));
extern void                ttv_delrcxlofig    __P(( ttvfig_list*));
extern void                ttv_addaxis        __P(( inffig_list *ifl, ttvfig_list *ttvfig));
extern void                ttv_readcell       __P(( char * ));
extern ttvfig_list *ttv_readcell_TTVFIG;
extern ttvpath_list*       ttv_classpath      __P(( ttvpath_list*, long ));
extern ptype_list*     ttv_getlatchaccess __P(( ttvfig_list*,
                                                ttvevent_list*,
                                                long
                                             ));

void ttv_post_traitment(ttvfig_list *ttvfig);
long ttv_calcnodeconstraint(ttvfig_list *ttvfig, long level, ttvevent_list *node, long type, char mode);
void ttv_init_stm(ttvfig_list *ttvfig);


// propagation functions

long ttv_RecomputeLineChain(ttvfig_list *tvf, ttvfig_list *tvi, chain_list *ordered_line_list, long inputSLEW, long *outputSLEW, float *energy, float capa, long type, long mode, int doref);
void ttv_RecomputeCleanLineChain(chain_list *ordered_line_list);
chain_list *ttv_BuildLineList(ttvfig_list *ttvfig, ttvevent_list *root, ttvevent_list *node, long type, int *usectk);
void ttv_RecomputeFreeLineChain(chain_list *ordered_line_list);
void ttv_CleanPropagation_PRECISION_LEVEL1(chain_list *line_list);
chain_list *ttv_DoPropagation_PRECISION_LEVEL1(ttvfig_list *tvf, ttvfig_list *tvi, ttvevent_list *node, ttvevent_list *root, long type);
void ttv_DoCharacPropagation(ttvfig_list *tvf, ttvfig_list *tvi, ttvevent_list *node, ttvevent_list *root, long type, chain_list *slope_list, chain_list *capa_list, chain_list **resd, chain_list **ress, chain_list **rese, chain_list *inputlinelist);
ttvpropinfo_list *ttv_get_prop_info(ttvevent_list *node);
long ttv_GET_FINDDELAY(ttvevent_list *node, int ref);
long ttv_GET_LINE_DELAY(ttvline_list *line, long type, int ref);
long ttv_GET_LINE_SLEW(ttvline_list *line, long type, int ref);
int ttv_SetPrecisionLevel(int val);
void ttv_set_search_exclude_type(long typestart, long typeend, long *oldstart, long *oldend);
int ttv_testnetnamemask(ttvfig_list *ttvfig, ttvsig_list *ptsig, chain_list *mask);
void ttv_FreeMarkedSigList();

extern ttvpath_list*      ttv_allocpath        __P(( ttvpath_list*,
                                                     ttvfig_list*,
                                                     ttvevent_list*,
                                                     ttvevent_list*,
                                                     ttvevent_list*,
                                                     ttvevent_list*,
                                                     ttvevent_list*,
                                                     ptype_list *,
                                                     long,
                                                     long,
                                                     long,
                                                     long,
                                                     long,
                                                     long,
                                                     long,
                                                     long,
                                                     long,
                                                     long,
                                                     timing_model*,
                                                     timing_model*,
                                                     long,
                                                     char,
                                                     long
                                                  ));

ttvcritic_list *ttv_alloccritic(ttvcritic_list *headcritic,ttvfig_list *ttvfigh,ttvfig_list *ttvfig,ttvevent_list *node,long type,long data,long delay,long slope,long newdelay,long newslope, char nodeflags, char *linemodelname, ttvline_list *line);

float ttv_get_signal_output_capacitance(ttvfig_list *tvf, ttvsig_list *tvs);

void ttv_activate_real_access_mode(int val);
void ttv_activate_path_and_access_mode(int val);

void ttv_getmargin(inffig_list *myfig, ttvsig_list *tvs, char *infregex, char *infsection, float *factor, long *delta);
chain_list *ttv_getallmarginregex(inffig_list *myfig);
ht *ttv_buildclockdetailht(NameAllocator *na, ttvfig_list *tvf, ttvpath_list *clockp, long findmode);
void ttv_checkpathoverlap(NameAllocator *na, ttvfig_list *tvf, ttvpath_list *datap, ht *cht, long findmode, long *overlapc, long *overlapd);

int ttv_mark_signals_with_corresponding_path_margin_regex(ttvfig_list *tvf);
void ttv_unmark_signals_corresponding_path_margin_regex(ttvfig_list *tvf);
int ttv_get_path_margins(ttvfig_list *tvf, ttvpath_list *datapath, ttvpath_list *clockpath, long *margedata, long *margeclock, long *overlapc, long *overlapd,int clockisdata, int dataisclock);
void ttv_checkinfchange(ttvfig_list *tvf);
extern long            ttv_getnodeslope   __P(( ttvfig_list *, ttvfig_list *, ttvevent_list *, long *, long )) ;

#define TTV_MORE_OPTIONS_REAL_ACCESS                    0x1
#define TTV_MORE_OPTIONS_PATH_AND_ACCESS                0x2
#define TTV_MORE_OPTIONS_ENABLE_STOP_ON_TTV_SIG_N       0x4 
#define TTV_MORE_OPTIONS_MULTIPLE_COMMAND_CRITIC_ACCESS 0x8
#define TTV_MORE_OPTIONS_SIMPLE_CRITIC                  0x10
#define TTV_MORE_OPTIONS_DIFF_ENDING_CRITIC_COMMAND     0x20
#define TTV_MORE_OPTIONS_MUST_BE_CLOCK                  0x40
#define TTV_MORE_OPTIONS_DONT_FILTER_ENDING_NODE        0x80
#define TTV_MORE_OPTIONS_FLAG_PROGATE_USED              0x100
#define TTV_MORE_OPTIONS_FLAG_THRU_FILTER_FOUND         0x200
#define TTV_MORE_OPTIONS_ONLYHZ                         0x400
#define TTV_MORE_OPTIONS_ONLYNOTHZ                      0x800
#define TTV_MORE_OPTIONS_ADD_ASYNCHRONOUS_PATHS         0x1000
#define TTV_MORE_OPTIONS_CROSS_STOP_NODE_IF_NONSTOP     0x2000
#define TTV_MORE_OPTIONS_MOST_DIRECT_ACCESS             0x4000
#define TTV_MORE_OPTIONS_USE_DATA_START                 0x8000
#define TTV_MORE_OPTIONS_USE_CLOCK_START                0x10000
#define TTV_MORE_OPTIONS_USE_NODE_NAME_ONLY             0x20000
#define TTV_MORE_OPTIONS_KEEP_PHASE_INFO                0x40000
#define TTV_MORE_OPTIONS_CRITIC_PARA_SEARCH             0x80000
#define TTV_MORE_OPTIONS_ADD_ASYNCHRONOUS_ACCESS        0x100000
//#define TTV_MORE_OPTIONS_ENABLE_START_ON_TTV_SIG_Q 8

extern long TTV_MORE_SEARCH_OPTIONS;
extern chain_list *TTV_EXPLICIT_CLOCK_NODES, *TTV_EXPLICIT_START_NODES, *TTV_EXPLICIT_END_NODES;
extern int ttv_canbeinfalsepath(ttvevent_list *ev, char where);
int ttv_hasaccessfalsepath(ttvfig_list *ttvfig,ttvevent_list *eventout, chain_list *clocks);

extern chain_list *ttv_getoutputlines(ttvevent_list *node, long type);
ttvevent_list *ttv_opposite_event(ttvevent_list *cmd_ev);
void ttv_experim_setstopaftergate1(int val);
extern ttvpath_list* ttv_getpathnocross_v2 __P(( ttvfig_list*, ttvfig_list*, ttvsig_list*, chain_list*, long, long, long,int));
extern ttvpath_list* ttv_getaccessnocross_v2 __P(( ttvfig_list*, ttvfig_list*, ttvsig_list*, chain_list*, long, long, long,int));
ttvpath_list *filter_parallel_to_node(ttvpath_list *pth);

#define TTV_SEARCH_INFORMATIONS_CRITIC_FALSEPATH  0x1
long ttv_get_search_informations();
void ttv_reset_search_informations();
void ttv_activate_search_charac_mode(chain_list *slopes_in, chain_list *capas_in);
void ttv_retreive_search_charac_results(chain_list **delays, chain_list **slopes, chain_list **energy);

long ttv_getloadedfigtypes(ttvfig_list *tvf);
chain_list *ttv_getendpoints(ttvfig_list *tvf, chain_list *sigl, long type);
ttvpath_list *ttv_create_one_node_path(ttvfig_list *ttvfig,ttvfig_list *ttvins,ttvevent_list *node,long type);
void ttv_activate_multi_cmd_critic_mode(int val);
void ttv_activate_simple_critic(int val);
void ttv_activate_multi_end_cmd_critic_mode(int val);
void ttv_search_mode(int set, long mask);

long ttv_testsigflag(ttvsig_list *tvs, long mask);
void ttv_setsigflag(ttvsig_list *tvs, long mask);
void ttv_resetsigflag(ttvsig_list *tvs, long mask);
void ttv_setsigflaggate(ttvsig_list *tvs, char num);
int ttv_getgateinfo(int num, char **name, int *nbinput);
int ttv_findgateinfo(char *sign);

void ttv_update_slope_and_output_capa(ttvfig_list *tf);
int ttv_check_update_slope_and_output_capa(ttvfig_list *tf);
void ttv_init_refsig(ttvfig_list *ttvfig, ttvsig_list *ptsig, char *name, char *net, float capa, long type);

extern long            ttv_getinittime           __P(( ttvfig_list *, ttvevent_list *, char, long, char, char *)) ;
ttvpath_list *ttv_duppath(ttvpath_list *pt);

int ttv_isgateoutput(ttvfig_list *tvf, ttvsig_list *tvs,long type);
int ttv_has_strict_setup(ttvevent_list *tve);
int ttv_addsig_addrcxpnode(int mode);

chain_list *ttv_getconnectoraxis(ttvfig_list *ttvfig, int slope, char *name);
int ttv_get_path_margins_info(ttvfig_list *tvf, ttvpath_list *path, float *factor, long *delta);
ttvcritic_list *ttv_dupcritic(ttvcritic_list *pt);

extern void               ttv_addhtmodel      __P(( ttvfig_list*));
extern void               ttv_delhtmodel      __P(( ttvfig_list*));
extern ttvfig_list*       ttv_gethtmodel      __P(( char *));
ptype_list *ttv_getlatchdatacommands(ttvfig_list *ttvfig,ttvevent_list *latch,long type);
float ttv_getline_vt(ttvfig_list *ttvfig,ttvfig_list *figpath,ttvevent_list *tve,ttvevent_list *cmd,long type);

ptype_list *ttv_get_directive_slopes(ttvevent_list *tve, int setup, int clockisdata, int dataisclock);
ttv_directive *ttv_get_directive(ttvsig_list *tvs);
chain_list *ttv_getmatchingdirectivenodes(ttvfig_list *tvf, chain_list *mask, int nonstop);

ttvcriticmc*    ttv_alloccriticmc( int n );
void            ttv_freecriticmc( ttvcriticmc *mc );

void            ttv_delassociatedcriticmc( ttvcritic_list *c );
void            ttv_setassociatedcriticmc( ttvcritic_list *c, ttvcriticmc *mc );
ttvcriticmc*    ttv_getassociatedcriticmc( ttvcritic_list *c );

unsigned int ttv_signtimingfigure( ttvfig_list *tf );
int   ttv_isfalsepath ( ttvfig_list*, ttvevent_list *, ttvevent_list *, long, int clocktoclock, ttvevent_list *latch);

void ttv_sendttvdelays(ttvfig_list *tvf, int ctk, int nbpvt);
int ttv_ssta_loadresults(ttvfig_list *tvf, chain_list *filenames, chain_list *order);
int ttv_ssta_set_run_number(ttvfig_list *tvf, int num);
int ttv_ssta_buildlinestat(ttvfig_list *tvf);
ttv_line_ssta_info *ttv_ssta_get_ssta_info(ttvfig_list *tvf);
int ttv_ssta_getline_index(ttvline_list *tvl);
int ttv_ssta_getsig_index(ttvsig_list *tvs);
float ttv_get_signal_capa(ttvsig_list *tvs);

void ttv_set_ttv_getpath_clock_list(chain_list *lst);
void ttv_freepathstblist(ttvpath_stb_stuff *head);

#define _LONG_TO_DOUBLE(x) ((x)*1e-12/TTV_UNIT)
#define _DOUBLE_TO_LONG(x) mbk_long_round((x)*1e12*TTV_UNIT)

int ttv_get_signal_swing(ttvfig_list *tvf, ttvsig_list *tvs, float *low, float *high);
void ttv_set_signal_swing(ttvsig_list *tvs, float low, float high);

char *ttv_getnetname_sub(ttvfig_list *ttvfig,char *ptname,ttvsig_list *ptsig, int retonly);
char *ttv_getsigname_sub(ttvfig_list *ttvfig,char *ptname,ttvsig_list *ptsig, int retonly);

chain_list *ttv_get_signals_from_netname(ttvfig_list *tvf, ht **h, char *netname);
void ttv_free_signals_from_netname(ht *h);

#endif // __TTV_LIB_H__

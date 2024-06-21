/******************************************************************************/
/*                                                                            */
/*                      Chaine de CAO & VLSI   AVERTEC                        */
/*                                                                            */
/*    Produit : LIB Version 1.00                                              */
/*    Fichier : lib100.h                                                      */
/*                                                                            */
/*    (c) copyright 2000 AVERTEC                                              */
/*    Tous droits reserves                                                    */
/*                                                                            */
/*    Auteur(s) : Gilles Augustins                                            */
/*                                                                            */
/******************************************************************************/

#ifndef LIB
#define LIB

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>
#include MUT_H
#include MLO_H
#include MLU_H
#include ELP_H
#include MCC_H
#include TUT_H
#include RCN_H
#include TRC_H
#define API_HIDE_TOKENS
#include STM_H
#include TTV_H
#include BEH_H
#include CBH_H

#ifdef AVERTEC
#include AVT_H
#endif

/******************************************************************************/
/*     defines                                                                */
/******************************************************************************/
#define LIB_NOM_PROCESS    1.0
#define LIB_DERATE_PROCESS 0.0
#define LIB_FALLING        0    /* falling transistion */
#define LIB_RISING         1    /* rising  transistion */

/*** context define ***/
#define                     LIB_TYPE ((long)0x01)
#define                    CELL_TYPE ((long)0x02)
#define                     PIN_TYPE ((long)0x03)
#define                      FF_TYPE ((long)0x04)
#define                   LATCH_TYPE ((long)0x05) 
#define              STATETABLE_TYPE ((long)0x06) 
#define                LU_TEMPL_TYPE ((long)0x07) 
#define                  TIMING_TYPE ((long)0x08) 
#define                DLY_RISE_TYPE ((long)0x09) 
#define                DLY_FALL_TYPE ((long)0x0A) 
#define                SLW_RISE_TYPE ((long)0x0B) 
#define                SLW_FALL_TYPE ((long)0x0C) 
#define             RISE_CONSTR_TYPE ((long)0x0D) 
#define             FALL_CONSTR_TYPE ((long)0x0E) 
#define               WIRE_LOAD_TYPE ((long)0x0F) 
#define                     BUS_TYPE ((long)0x10) 
#define                    TYPE_TYPE ((long)0x11) 
#define         WIRE_LOAD_TABLE_TYPE ((long)0x12) 
#define            POWER_SUPPLY_TYPE ((long)0x13) 
#define               NO_ACTION_TYPE ((long)0xFF)

/*** simple attribute define ***/
#define       SA_NO_INFO                ((long)0x1 )
#define       SA_TIME_UNIT              ((long)0x2 )
#define       SA_RES_UNIT               ((long)0x3 )
#define       SA_SLEW_LOW_FALL_UNIT     ((long)0x4 )
#define       SA_SLEW_LOW_RISE_UNIT     ((long)0x5 )
#define       SA_SLEW_UP_FALL_UNIT      ((long)0x6 )
#define       SA_SLEW_UP_RISE_UNIT      ((long)0x7 )
#define       SA_VAR1                   ((long)0x8 )
#define       SA_VAR2                   ((long)0x9 )
#define       SA_DIR                    ((long)0xA )
#define       SA_CAPA                   ((long)0xB )
#define       SA_FUNC                   ((long)0xC )
#define       SA_TS_FUNC                ((long)0xD )
#define       SA_X_FUNC                 ((long)0xE )
#define       SA_REL_PIN                ((long)0xF )
#define       SA_TIM_SENS               ((long)0x10)
#define       SA_TIM_TYPE               ((long)0x11)
#define       SA_CK                     ((long)0x12)
#define       SA_NEXT_ST                ((long)0x13)
#define       SA_CLEAR                  ((long)0x14)
#define       SA_PRESET                 ((long)0x15)
#define       SA_CPVAR1                 ((long)0x16)
#define       SA_CPVAR2                 ((long)0x17)
#define       SA_CK_ON                  ((long)0x18)
#define       SA_CK_ON_ALSO             ((long)0x19)
#define       SA_ENABLE                 ((long)0x1A)
#define       SA_DATA                   ((long)0x1B)
#define       SA_INTR_RISE              ((long)0x1C)
#define       SA_INTR_FALL              ((long)0x1D)
#define       SA_SLOPE_RISE             ((long)0x1E)
#define       SA_SLOPE_FALL             ((long)0x1F)
#define       SA_RESI                   ((long)0x20)
#define       SA_DEFAULT_WIRE_LOAD      ((long)0x21)
#define       SA_BASE_TYPE              ((long)0x22)
#define       SA_BIT_FROM               ((long)0x23)
#define       SA_BIT_TO                 ((long)0x24)
#define       SA_BIT_WIDTH              ((long)0x25)
#define       SA_DATA_TYPE              ((long)0x26)
#define       SA_DOWNTO                 ((long)0x27)
#define       SA_BUS_TYPE               ((long)0x28)
#define       SA_BUS_NAMING_STYLE       ((long)0x29)
#define       SA_NOM_VOLTAGE            ((long)0x2A)
#define       SA_NOM_TEMP               ((long)0x2B)
#define       SA_INPUT_DTH_RISE         ((long)0x2C)
#define       SA_INPUT_DTH_FALL         ((long)0x2D)
#define       SA_OUTPUT_DTH_RISE        ((long)0x2E)
#define       SA_OUTPUT_DTH_FALL        ((long)0x2F)
#define       SA_DEFAULT_SLOPE_RISE     ((long)0x30)
#define       SA_DEFAULT_SLOPE_FALL     ((long)0x31)
#define       SA_DEFAULT_POWER_RAIL     ((long)0x32)
#define       SA_INPUT_SIGNAL_LEVEL     ((long)0x33)
#define       SA_OUTPUT_SIGNAL_LEVEL    ((long)0x34)

/*** complex attribute define ***/
#define       CA_NO_INFO                ((long)0x1 )
#define       CA_CAPA_UNIT              ((long)0x2 )
#define       CA_INDEX1                 ((long)0x3 )
#define       CA_INDEX2                 ((long)0x4 )
#define       CA_VALUES                 ((long)0x5 )
#define       CA_FANOUT_LENGTH          ((long)0x6 )
#define       CA_FANOUT_CAPACITANCE     ((long)0x7 )
#define       CA_FANOUT_RESISTANCE      ((long)0x8 )
#define       CA_POWER_RAIL             ((long)0x9 )
#define       CA_RAIL_CONNECTION        ((long)0x10 )

#define       LIB_DRIVED_LINE           ((long) 0x20070124 )
#define       LIB_MAX_CAPACITANCE       0x20080624
#define       LIB_GENERATED_CLOCK_INFO  0x20080725
#define       LIB_GENERATED_CLOCK_MASTERS 0x20080728
#define       LIB_CONNECTOR_ENERGY_MODEL_NAME 0x20090415

/*** units ***/
/* time */
#define     LIB_NS      'N'     /* nanosecond */
#define     LIB_PS      'P'     /* picosecond */

/* resistance */
#define     LIB_OHM     'O'     /* ohm        */ 
#define     LIB_KOHM    'K'     /* kilo-ohm   */

/* capacitance */
#define     LIB_PF      'P'     /* picofarad  */
#define     LIB_FF      'F'     /* femtofarad */


/*** timing sense ***/
#define     LIB_POS   'P'       /* positive unate */
#define     LIB_NEG   'I'       /* negative unate */
#define     LIB_NON   'N'       /*      non unate */

/*** timing type ***/
#define     LIB_SETUP_RISE  'S' /* setup_rising   */
#define     LIB_SETUP_FALL  's' /* setup_falling  */
#define     LIB_HOLD_RISE   'H' /* hold_rising    */
#define     LIB_HOLD_FALL   'h' /* hold_falling   */
#define     LIB_RISE_EDGE   'E' /* rising_edge    */
#define     LIB_FALL_EDGE   'e' /* falling_edge   */
#define     LIB_TS_ENABLE   'T' /* falling_edge   */
#define     LIB_TS_DISABLE  't' /* falling_edge   */
#define     LIB_NOTYPE      'N' /* aucun des cas precedents */


/******************************************************************************/
/*     globals                                                                */
/******************************************************************************/
extern ht  *tut_tablasharea;     
extern char *TMA_TUNIT;
extern chain_list *LIB_TTVFIG_LIST;
extern char *LIB_BUS_DELIM;
/******************************************************************************/
/*     structures                                                             */
/******************************************************************************/

/******************************************************************************/
/*     functions                                                              */
/******************************************************************************/


extern void lib_drive (chain_list*, chain_list*, char*, char*) ;
extern void lib_parse (char*);
void lib_parse_this_file (char *filename, char *ext);
extern chain_list *lib_load (char*);
extern void libenv() ;
extern void lib_driveheader       (FILE*, char*, ttvinfo_list*);
extern char *lib_get_tg_pinname(char *str);
#endif


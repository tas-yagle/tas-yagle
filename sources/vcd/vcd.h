#ifndef VCD
#define VCD

/******************************************************************************/
/* includes                                                                   */
/******************************************************************************/

#include MUT_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/******************************************************************************/
/* structures                                                                 */
/******************************************************************************/

typedef struct trans_list {
    struct trans_list *NEXT;
    struct trans_list *PREV;
    int                TIME;
    char              *VAL;
    int                INDEX;
    int                BIT;
    float              NRJ;
} trans_list;

typedef struct t_vcd_db {
    /* signals entry */
    trans_list **SIG_ENTRY;
    chain_list **SIG_TAB;
    ht          *SIG_HT;
    ht          *IDX_HT;
    int          SIG_TAB_SZ;
    int          SIG_IDX;
    /* times entry */
    chain_list **TIME_ENTRY; //DATA = trans_list*
    int         *TIME_TAB;
    int          TIME_TAB_SZ;
    int          TIME_IDX;
    /* misc */
    int          TIME_BASE;
    float        TIME_UNIT;

    /* interfacing */
    int          CUR_TIME_IDX;
    chain_list  *CUR_STATE;
    float       *NRJ_ENTRY;
} t_vcd_db;

extern void vcd_parsefile (char*, char*);
extern void cdl_parse (char*);
extern trans_list *cdl_get_waveform (char *signal);
extern chain_list *cdl_get_sig_list ();
extern chain_list *cdl_getnextsig (chain_list *sig);
extern char *cdl_getsigname (chain_list *sig);
extern chain_list *cdl_get_current_events ();
extern void cdl_free_event_list (chain_list *events);
extern long vcd_count_events (chain_list *events);
extern chain_list *cdl_getnextevent (chain_list *event);
extern chain_list *cdl_geteventsig (chain_list *event);
extern char cdl_geteventval (chain_list *event);
extern void cdl_goto_next_time ();
extern void vcd_goto_first_time ();
extern char vcd_goto_next_time ();
extern void vcd_goto_init_time ();
extern void vcd_add_nrj (float nrj);
extern void vcd_add_trans_nrj (trans_list *trans, float nrj);
extern long vcd_get_time ();
extern void vcd_free_db ();
extern float vcd_TimeUnit(char *name);
extern void  vcd_list_sigs ();
extern long  vcd_get_time_from_index  (long index);
extern float vcd_get_nrj_from_index   (long index);
extern long  vcd_get_max_index        ();
extern float vcd_get_ts ();

#endif

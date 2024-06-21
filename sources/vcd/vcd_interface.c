/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : VCD Version 2.00                                            */
/*    Fichier : vcd_interface.c                                             */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include VCD_H
#include "vcd_parse.h" 

/****************************************************************************/
/*     globals                                                              */
/****************************************************************************/

/****************************************************************************/
/*     externs                                                              */
/****************************************************************************/

int vcd_db_check ()
{
    if (!vcd_db) {
        fprintf (stderr, "[VCD ERR] no VCD database loaded\n");
        return 0;
    }
    else return 1;
}

/****************************************************************************/

void vcd_list_sigs ()
{
    int i;
    chain_list *chain;
    char *signame;

    if (!vcd_db_check ()) return;

    for (i = 0; i < vcd_db->SIG_IDX; i++) {
        for(chain = vcd_db->SIG_TAB[i]; chain; chain = chain->NEXT){
            signame = (char*)chain->DATA;
            if (isvss (signame) || isglobalvss (signame)) continue; 
            if (isvdd (signame) || isglobalvdd (signame)) continue; 
            fprintf (stdout, "   %s\n", signame);
        }
    }
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

void vcd_print_waveform (trans_list *waveform)
{
    trans_list *trans;
    chain_list *chain;

    if (!vcd_db_check ()) return;
        
    for(chain = vcd_db->SIG_TAB[waveform->INDEX]; chain; chain = chain->NEXT){    
        fprintf (stdout, "%s : ", (char*)chain->DATA);
    }
    for (trans = waveform; trans; trans = trans->NEXT)
        fprintf (stdout, " (%d, '%s')", trans->TIME, trans->VAL);
    fprintf (stdout, "\n");
}
/****************************************************************************/

trans_list *vcd_get_waveform (char *signal)
{
    long index;
    char        buf[1024];
     
    if (!vcd_db_check ()) return NULL;
    
    sscanf(signal,"%[^[][%*d:%*d]",buf);

    if ((index = gethtitem (vcd_db->SIG_HT, namealloc (buf))) == EMPTYHT) {
        fprintf (stderr, "[VCD ERR] signal '%s' does not exist in VCD file\n", signal);
        return NULL;
    }

    return vcd_db->SIG_ENTRY[index];
}

/****************************************************************************/

trans_list *vcd_get_first_event (trans_list *waveform)
{
    return waveform;
}

/****************************************************************************/

trans_list *vcd_get_next_event (trans_list *event)
{
    if (event->NEXT)
        return event->NEXT;
    else 
        return event;
}

/****************************************************************************/

trans_list *vcd_get_prev_event (trans_list *event)
{
    if (event->PREV)
        return event->PREV;
    else 
        return event;
}

/****************************************************************************/

void vcd_print_event (trans_list *event)
{
    fprintf (stdout, "\n   (%d, '%s')\n", event->TIME, event->VAL);
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

chain_list *vcd_get_current_events ()
{
    
    if (!vcd_db_check ()) return NULL;

    return vcd_db->TIME_ENTRY[vcd_db->CUR_TIME_IDX];
}

/****************************************************************************/

long vcd_count_events (chain_list *events)
{
    chain_list *ch;
    int count = 0;

    if (!vcd_db_check ()) return 0;
    
    for (ch = events; ch; ch = ch->NEXT) count++;

    return count;
}

/****************************************************************************/

void vcd_print_current_events (chain_list *events)
{
    chain_list *ch;
    trans_list *trans;
    chain_list *chain;

    if (!vcd_db_check ()) return;
    
    fprintf (stdout, "%d : ", ((trans_list*)events->DATA)->TIME);
    for (ch = events; ch; ch = ch->NEXT) {
        trans = (trans_list*)ch->DATA;
        for(chain = vcd_db->SIG_TAB[trans->INDEX]; chain; chain = chain->NEXT){
            fprintf (stdout, " (%s, '%s')", (char*)chain->DATA, trans->VAL);
        }
    }
    fprintf (stdout, "\n");
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

static void vcd_goto_init_state ()
{
    if (vcd_db->CUR_STATE) freechain (vcd_db->CUR_STATE);
    vcd_db->CUR_STATE = dupchainlst (vcd_db->TIME_ENTRY[0]);
}

/****************************************************************************/

static void vcd_goto_state (long time) 
{
    int         i;
    trans_list *trans;
    chain_list *ch;

    if (!vcd_db_check ()) return;

    if (!vcd_db->CUR_STATE) { // search from time 0
        for (i = 0; i < vcd_db->SIG_IDX; i++)
            for (trans = vcd_db->SIG_ENTRY[i]; trans; trans = trans->NEXT)
                if (!trans->NEXT || (trans->NEXT->TIME > time) || (trans->TIME == time)) {
                    vcd_db->CUR_STATE = addchain (vcd_db->CUR_STATE, trans);
                    break;
                }
    }
    else if (time > vcd_db->TIME_TAB[vcd_db->CUR_TIME_IDX]) { // update current state forward
        for (ch = vcd_db->CUR_STATE; ch; ch = ch->NEXT)
            for (trans = (trans_list*)ch->DATA; trans; trans = trans->NEXT)
                if (!trans->NEXT || (trans->NEXT->TIME > time) || (trans->TIME == time)) {
                    ch->DATA = trans;
                    break;
                }
    }
    else if (time < vcd_db->TIME_TAB[vcd_db->CUR_TIME_IDX]) // update current state backward
        for (ch = vcd_db->CUR_STATE; ch; ch = ch->NEXT)
            for (trans = (trans_list*)ch->DATA; trans; trans = trans->PREV)
                if (!trans->PREV || (trans->TIME <= time)) {
                    ch->DATA = trans;
                    break;
                }
}

/****************************************************************************/

chain_list *vcd_get_state () 
{
    if (!vcd_db_check ()) return NULL;
    
    return vcd_db->CUR_STATE;
}

/****************************************************************************/

void vcd_print_state (chain_list *state)
{
    chain_list *ch;
    trans_list *trans;
    chain_list *chain;

    if (!vcd_db_check ()) return;

    for (ch = state; ch; ch = ch->NEXT) {
        trans = (trans_list*)ch->DATA;
        for(chain = vcd_db->SIG_TAB[trans->INDEX]; chain; chain = chain->NEXT){
            fprintf (stdout, "%s : '%s' (t = %d)\n", (char*)chain->DATA, trans->VAL, trans->TIME);
        }
    }
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

long vcd_get_time_from_index (long index)
{
    if (!vcd_db_check ()) return 0;

    return vcd_db->TIME_TAB[index];
}

/****************************************************************************/

float vcd_get_nrj_from_index (long index)
{
    if (!vcd_db_check ()) return 0;

    return vcd_db->NRJ_ENTRY[index];
}

/****************************************************************************/

long vcd_get_max_index ()
{
    if (!vcd_db_check ()) return 0;

    return vcd_db->TIME_IDX;
}

/****************************************************************************/

long vcd_get_time ()
{
    if (!vcd_db_check ()) return 0;

    return vcd_db->TIME_TAB[vcd_db->CUR_TIME_IDX];
}

/****************************************************************************/

void vcd_goto_time (long time)
{
    int i;
    
    if (!vcd_db_check ()) return;

    vcd_goto_state (time);

    for (i = 0; i < vcd_db->TIME_IDX; i++)
        if (vcd_db->TIME_TAB[i] >= time) break;
    vcd_db->CUR_TIME_IDX = i;
}

/****************************************************************************/

char vcd_goto_next_time ()
{
    long time;

    if (!vcd_db_check ()) return 0;

    if (vcd_db->CUR_TIME_IDX < vcd_db->TIME_IDX) {
        vcd_db->CUR_TIME_IDX++; 
        time = vcd_get_time ();
        vcd_db->CUR_TIME_IDX--; 
        vcd_goto_state (time);
        vcd_db->CUR_TIME_IDX++; 
        return 1;
    }

    return 0;
}

/****************************************************************************/

void vcd_goto_prev_time ()
{
    long time;

    if (!vcd_db_check ()) return;

    if (vcd_db->CUR_TIME_IDX >= 0) {
        vcd_db->CUR_TIME_IDX--;
        time = vcd_get_time ();
        vcd_db->CUR_TIME_IDX++; 
        vcd_goto_state (time);
        vcd_db->CUR_TIME_IDX--;
    }
}

/****************************************************************************/

void vcd_goto_init_time ()
{
    if (!vcd_db_check ()) return;

    vcd_db->CUR_TIME_IDX = 0;
    vcd_goto_init_state ();
}

/****************************************************************************/

void vcd_goto_first_time ()
{
    if (!vcd_db_check ()) return;

    vcd_goto_init_state ();
    vcd_db->CUR_TIME_IDX = 0;
    vcd_goto_state (0);
    vcd_db->CUR_TIME_IDX = 1;
}

/****************************************************************************/

void vcd_add_nrj (float nrj)
{
    if (!vcd_db_check ()) return;
    
    vcd_db->NRJ_ENTRY[vcd_db->CUR_TIME_IDX] = nrj;
}

/****************************************************************************/

void vcd_add_trans_nrj (trans_list *trans, float nrj)
{
    if (!vcd_db_check ()) return;
    
    trans->NRJ = nrj;
}


#include "vcd_interface.h"
#include "vcd_cdl_interface.h"

/*------------------------------------------------------------------*/
/*------------------------------------------------------------------*/

int WScan_ParseFiles (char *vcd_file, char *cor_file)
{
    FILE *vcd, *cor;

    if ((vcd = mbkfopen (vcd_file, NULL, READ_TEXT))) fclose (vcd);
    else fprintf (stderr, "[WScan ERR] Can't find file %s\n", vcd_file);

    if ((cor = mbkfopen (cor_file, NULL, READ_TEXT))) fclose (cor);
    else fprintf (stderr, "[WScan ERR] Can't find file %s\n", cor_file);

    if (cor && vcd) {
        vcd_parsefile (vcd_file, NULL);
        cdl_parse (cor_file);
    }
    else return 0;

    return 1;
}

/*------------------------------------------------------------------*/
/*------------------------------------------------------------------*/

trans_list *WScan_GetWaveform (char *signal)
{
    return cdl_get_waveform (signal);
}

/*------------------------------------------------------------------*/
/* Signals                                                          */
/*------------------------------------------------------------------*/

chain_list *WScan_GetSignalsList ()
{
    return cdl_get_sig_list ();
}

chain_list *WScan_GetNextSig (chain_list *sig)
{
    return cdl_getnextsig (sig);
}

char *WScan_GetSigName (chain_list *sig)
{
    return cdl_getsigname (sig);
}


/*------------------------------------------------------------------*/
/* Events                                                           */
/*------------------------------------------------------------------*/

chain_list *WScan_GetEvents ()
{
    return cdl_get_current_events ();
}

void WScan_FreeEvents (chain_list *events)
{
    cdl_free_event_list (events);
}

long WScan_CountEvents (chain_list *events)
{
    return vcd_count_events (events);
}

chain_list *WScan_GetNextEvent (chain_list *event)
{
    return cdl_getnextevent (event);
}

char *WScan_GetEventSigName (chain_list *event)
{
    chain_list *chain;
    if((chain = cdl_geteventsig (event)) != NULL)
        return (char*)chain->DATA;
    else
        return NULL;
}

char WScan_GetEventValue (chain_list *event)
{
    return cdl_geteventval (event);
}

/*------------------------------------------------------------------*/
/* Time                                                             */
/*------------------------------------------------------------------*/

void WScan_GoToNextTime ()
{
    cdl_goto_next_time ();
}

void WScan_GoToInitTime ()
{
    vcd_goto_init_time ();
}

long WScan_GetTime ()
{
    return vcd_get_time ();
}

/*------------------------------------------------------------------*/
/* DataBase                                                         */
/*------------------------------------------------------------------*/

void WScan_FreeDB ()
{
    vcd_free_db ();
}

/* functions for the pattern.c file */

#include "tbg.h"

void AssignBin (char *port_name, char *value)
{
    char bval[10240];
    unsigned int range, i;

    t_port *port = tbg_GetPort (port_name);

    if (!port) {
        fprintf (stderr, "Port %s does not exist in design %s\n", port_name, DESIGN_NAME);
        EXIT (1);
    }

    range = abs (port->B0 - port->B1) + 1;
    bval[0] = 'A'; /* affect */

    port->FLAG = 1;

    if (strlen (value) != range) {
        fprintf (stderr, "type mismatch for port %s\n", port_name);
        EXIT (1);
    } else
        for (i = 0; i < range; i++)
            if (port->B0 < port->B1)
                bval[i + 1] = value[i];
            else
                bval[range - i] = value[i];
    bval[i + 1] = '\0';

    //port->VALUE = strdup (bval);
    port->VALUE = strdup (bval + 1);
}

/* ------------------------------------------------------------------------- */

void Assign (char *port_name, int value)
{
    char bval[10240];
    int  range;

    t_port *port = tbg_GetPort (port_name);

    if (!port) {
        fprintf (stderr, "Port %s does not exist in design %s\n", port_name, DESIGN_NAME);
        EXIT (1);
    }

    range = abs (port->B0 - port->B1) + 1;
    bval[0] = 'A'; /* affect */

    port->FLAG = 1;

    if (range == 1)
        switch (value) {
            case DONTCARE:  bval[1] = '-'; bval[2] = '\0'; break;
            case WEAK_UNK:  bval[1] = 'W'; bval[2] = '\0'; break;
            case FORCE_UNK: bval[1] = 'X'; bval[2] = '\0'; break;
            case 0:         bval[1] = '0'; bval[2] = '\0'; break;               
            case 1:         bval[1] = '1'; bval[2] = '\0'; break;  
            default: 
                fprintf (stderr, "type mismatch for port %s\n", port_name);
                EXIT (1);
        }

    if (range > 1) {
        dec_to_bin (&bval[0] + 1, value, range);
        if (port->B0 > port->B1)
            reverse_endianess (&bval[0] + 1);
    }

    //port->VALUE = strdup (bval);
    port->VALUE = strdup (bval + 1);
}

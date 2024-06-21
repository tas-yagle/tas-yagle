/* functions for the pattern.c file */

#include "tbg.h"

void Check (char *port_name, int expected_value)
{
    char bval[1024];
    int  range;

    t_port *port = tbg_GetPort (port_name);

    if (!port) {
        fprintf (stderr, "Port %s does not exist in design %s\n", port_name, DESIGN_NAME);
        EXIT (1);
    }

    port->FLAG = 1;
    range = abs (port->B0 - port->B1) + 1;
    bval[0] = 'C'; /* check */

    if (range == 1)
        switch (expected_value) {
            case DONTCARE: bval[1] = '-'; bval[2] = '\0'; break;
            case 0:        bval[1] = '0'; bval[2] = '\0'; break;               
            case 1:        bval[1] = '1'; bval[2] = '\0'; break;  
            default: 
                fprintf (stderr, "type mismatch for port %s\n", port_name);
                EXIT (1);
        }

    if (range > 1) {
        dec_to_bin (&bval[0] + 1, expected_value, range);
        if (port->B0 > port->B1)
            reverse_endianess (&bval[0] + 1);
    }

    port->VALUE = strdup (bval);
}

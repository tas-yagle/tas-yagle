/* functions for the pattern.c file */

#include "tbg.h"

long ValMax (char *port_name)
{
    int i;
    long sum = 0;
    int  range;
    t_port *port = tbg_GetPort (port_name);

    if (!port) {
        fprintf (stderr, "Port %s does not exist in design %s\n", port_name, DESIGN_NAME);
        EXIT (1);
    }
    
    range = abs (port->B0 - port->B1) + 1;

    for (i = 0; i < range; i++)
        sum += exp2 (i);

    return sum;
}

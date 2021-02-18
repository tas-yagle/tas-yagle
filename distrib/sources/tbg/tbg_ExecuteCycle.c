/* functions for the pattern.c file */

#include "tbg.h"

void ExecuteCycle ()
{
    t_port *port;

    for (port = HEAD_PORT; port; port = port->NEXT)
        if (port->VALUE)
            fprintf (PAT_FILE, " %s", port->VALUE);
    fprintf (PAT_FILE, "\n");
    
}

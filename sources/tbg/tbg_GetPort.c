#include "tbg.h"

t_port *tbg_GetPort (char *port_name)
{
    t_port *port;

    for (port = HEAD_PORT; port; port = port->NEXT)
        if (!strcasecmp (port->NAME, port_name))
            return port;

    return NULL;
}

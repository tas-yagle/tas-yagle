/* functions for the pattern.c file */

#include "tbg.h"

void Period (int t, char *unit)
{
    if (!strcasecmp (unit, "ps"))
        UNIT = 'P';
    else if (!strcasecmp (unit, "ns"))
        UNIT = 'N';
    else if (!strcasecmp (unit, "us"))
        UNIT = 'U';
    else if (!strcasecmp (unit, "ms"))
        UNIT = 'M';
    else if (!strcasecmp (unit, "s"))
        UNIT = 'S';
    else {
        fprintf (stderr, " - error: unknown unit %s, available units are 'ps', 'ns', 'us', 'ms' and 's\n", unit);
        UNIT = 'N';
    }

    PERIOD = t;
}

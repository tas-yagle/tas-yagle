/* functions for the pattern.c file */

#include "tbg.h"

ht *GENERICS_HT = NULL;
chain_list *GENERICS_CHAIN = NULL;
chain_list *VALUES_CHAIN = NULL;

extern int vhdldebug;

void Options (int argc, char **argv)
{
    char *buf, *p;
    int i;

    mbkenv ();

    GENERICS_HT = addht (10);
    
    for (i = 1; i < argc; i++) {
        buf = strdup (argv[i]);
        if ((p = strchr (buf, '='))) {
            p++;
            *(p - 1) = '\0';
            addhtitem (GENERICS_HT, namealloc (buf), atoi (p));
            GENERICS_CHAIN = addchain (GENERICS_CHAIN, namealloc (buf));
            VALUES_CHAIN = addchain (VALUES_CHAIN, (void*)atoi (p));
        }
        mbkfree(buf);
    }
}

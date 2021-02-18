#include "tbg.h"

t_port *HEAD_PORT = NULL;
FILE   *PAT_FILE = NULL;
char   *PAT_FILE_NAME = NULL;
char   *OUT_FILE_NAME = NULL;
char   *DESIGN_NAME = NULL;
int     LINE = 1;
int     PERIOD = 10;
char    UNIT = 'N';

/*--------------------------------------------------------------*/
/*--------- useful functions -----------------------------------*/
/*--------------------------------------------------------------*/

/*--------- log2 -----------------------------------------------*/
unsigned long log2 (unsigned long n)
{
    unsigned long log = 0;
    while (n /= 2)
        log++;
    return log;
}

/*--------- exp2 -----------------------------------------------*/
unsigned long exp2 (unsigned long n)
{
    long exp = 1;
    while (n--)
        exp *= 2;
    return exp;
}

/*--------- dec_to_bin -----------------------------------------*/
void reverse_endianess (char *bin)
{
    int i, l;
    char *buf;
    
    l = strlen (bin);
    buf = (char*)mbkalloc (l * sizeof (char));

    for (i = 0; i < l; i++)
        buf[l - i - 1] = bin[i];
    for (i = 0; i < l; i++)
        bin[i] = buf[i];

    mbkfree(buf);
}

/*--------- dec_to_bin -----------------------------------------*/
void dec_to_bin (char *bin, unsigned long dec, int bits)   /* big endian */
{
    int i;
    int exit = 0;

    for (i = 0; i < bits; i++) {
        switch (dec) {
            case DONTCARE: bin[i] = '-'; exit = 1; break;
            case WEAK_UNK: bin[i] = 'W'; exit = 1; break;
            case FORCE_UNK: bin[i] = 'X'; exit = 1; break;
            default: bin[i] = '0';
        }
    }
    bin[i] = '\0';

    if (exit) return;

    for (i = 0; i < bits; i++) {
        bin[i] = (dec & 1) + '0';
        dec >>= 1;
    }
    bin[i] = '\0';
}



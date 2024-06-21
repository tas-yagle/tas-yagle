/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_mod_name.c                                              */
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

#include "stm.h" 

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

char *stm_slwmname_extract (char *name)
{
    char buf[1024];

    if (!name)
        return NULL;

    if (strstr (name, "dly/slw")) {
        sprintf (buf, "slw%s", name + 7);
    }
    else
        sprintf (buf, "slw%s", name);

    return namealloc (buf);
}


/****************************************************************************/

char *stm_dlymname_extract (char *name)
{
    char buf[1024];

    if (!name)
        return NULL;

    if (strstr (name, "dly/slw")) {
        sprintf (buf, "dly%s", name + 7);
    }
    else
        sprintf (buf, "dly%s", name);

    return namealloc (buf);
}

/****************************************************************************/

char *stm_mod_name (char *input, char in_tr, char *output, char out_tr, unsigned int mtype, char minmax, int num)
{
    char name[4096];
    char tr[2];
    char buffer[128];
    char *type = &buffer[0];
    int  i = 0;

    if (mtype & STM_DELAY) {
        sprintf (type, "dly");
        type += 3;
    }
    if (mtype & STM_SLEW) {
        if (type != &buffer[0])
            *type++ = '/';
        sprintf (type, "slw");
        type += 3;
    }
    if (mtype & STM_ACCESS) {
        if (type != &buffer[0])
            *type++ = '/';
        sprintf (type, "acs");
        type += 3;
    }
    if (mtype & STM_HOLD) {
        if (type != &buffer[0])
            *type++ = '/';
        sprintf (type, "hld");
        type += 3;
    }
    if (mtype & STM_SETUP) {
        if (type != &buffer[0])
            *type++ = '/';
        sprintf (type, "stp");
        type += 3;
    }
    type = &buffer[0];

    if (in_tr == STM_UP)
        tr[0] = '1';
    else
    if (in_tr == STM_DN)
        tr[0] = '0';
    else
        tr[0] = 'X';
    
    if (out_tr == STM_UP)
        tr[1] = '1';
    else
    if (out_tr == STM_DN)
        tr[1] = '0';
    else
        tr[1] = 'X';

    switch (minmax) {
        case STM_MIN:
            if (num)
                sprintf (name, "%s__%s/%s__%c%c_min_%d", type, input, output,tr[0], tr[1], num);
            else
                sprintf (name, "%s__%s/%s__%c%c_min", type, input, output,tr[0], tr[1]);
            break;
        case STM_MAX:
            if (num)
                sprintf (name, "%s__%s/%s__%c%c_max_%d", type, input, output,tr[0], tr[1], num);
            else
                sprintf (name, "%s__%s/%s__%c%c_max", type, input, output,tr[0], tr[1]);
            break;
        default:
            if (num)
                sprintf (name, "%s__%s/%s__%c%c_%d", type, input, output,tr[0], tr[1], num);
            else
                sprintf (name, "%s__%s/%s__%c%c", type, input, output,tr[0], tr[1]);
            break;
    }

    /* remplacement de '[' ou ']' par '_' */
    i = 0;
    while (name[i] != '\0') {
        if (name[i] == '[' || name[i] == ']')
            name[i] = '_';
        i++;
    }
    /* fin remplacement */

    return namealloc (name);
}

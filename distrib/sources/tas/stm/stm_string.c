/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_string.c                                                */
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

void stm_end (char *s)
{
    do
        s++;
    while (*s);
}

/****************************************************************************/

int stm_lsb (char *vect_ident)
{
    int i = 0, lsb = 0;
    while (vect_ident[i++] != '\0') 
        if (vect_ident[i] == '[') {
            lsb = vect_ident[++i] - 48;
            while (vect_ident[++i] != ':')
                lsb = lsb * 10 + vect_ident[i] - 48; 
        }
    return lsb;
}

/****************************************************************************/

int stm_msb (char *vect_ident)
{
    int i = 0, msb = 0;
    while (vect_ident[i++] != '\0') 
        if (vect_ident[i] == ':') {
            msb = vect_ident[++i] - 48;
            while (vect_ident[++i] != ']')
                msb = msb * 10 + vect_ident[i] - 48; 
        }
    return msb;
}

/****************************************************************************/

char *stm_basename (char *vect_ident)
{
    int i = 0;
    char *basename = (char*)mbkalloc (STM_BUFSIZE * sizeof (char));
    
    while (vect_ident[i] != '[' && vect_ident[i] != '\0') {
        basename[i] = vect_ident[i];
        i++;
    }
    basename[i] = '\0';
    return basename;
}

/****************************************************************************/

char *stm_vect (char *ident, int p)
{
    char *vectname = (char*)mbkalloc(STM_BUFSIZE * sizeof (char));
    sprintf (vectname, "%s[%d]", ident, p);
    return vectname;
}

/****************************************************************************/

char *stm_unquote (char *qstr)
{
    int   i = 1; 
    char str[1024];

    while (qstr[i] != '\0')
        str[i - 1] = qstr[i++];
    str[i - 2] = '\0';

    return namealloc (str);
}

/****************************************************************************/

char *stm_pack (char *s)
{
    int i = 0;
    
    while (s[i++] != '\0')
        if (s[i] == ' ' || s[i] == '\t')
            s[i] = '_'; 

    return s;
}

/****************************************************************************/

char *stm_rename (char *str)
{
    char *num;
    char ss[1024];
    char *s = mbkstrdup (str);
    char *c;
    int n;

    if ((c = strchr (s, '@'))) {
        num = c + 1;
        *c = '\0';
        n = atoi (num);
        n++;
        sprintf (ss, "%s@%d", s, n);
    } else 
        sprintf (ss, "%s@1", s);

    mbkfree(s);
    return namealloc (ss);
}

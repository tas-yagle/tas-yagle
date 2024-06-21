/****************************************************************************/
/*                                                                          */
/*                      Chaine de verification                              */
/*                                                                          */
/*    Produit : AVT Version 1                                               */
/*    Fichier : avt_util.c                                                  */
/*                                                                          */
/*    (c) copyright 1998-1999 AVERTEC                                       */
/*    Tous droits reserves                                                  */
/*    Support : e-mail support@avertec.com                                  */
/*                                                                          */
/*    Auteur(s) : AUGUSTINS Gilles                                          */
/*                                                                          */
/****************************************************************************/

#include <time.h>
#include "avt_headers.h"

void avt_date (char *date)
{
    time_t counter;
    struct tm *ltime;
    char *month = NULL;
    int year;

    time (&counter);
    ltime = localtime (&counter);

    switch (ltime->tm_mon) {
        case 0: month = "Jan"; break;
        case 1: month = "Feb"; break;
        case 2: month = "Mar"; break;
        case 3: month = "Apr"; break;
        case 4: month = "May"; break;
        case 5: month = "Jun"; break;
        case 6: month = "Jul"; break;
        case 7: month = "Aug"; break;
        case 8: month = "Sep"; break;
        case 9: month = "Oct"; break;
        case 10: month = "Nov"; break;
        case 11: month = "Dec"; break;
    }

    year = ltime->tm_year + 1900;

    sprintf (date, "%d %s %d", ltime->tm_mday, month, year);
}

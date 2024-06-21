/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STB Version 1.00                                            */
/*    Fichier : stb_error.c                                                 */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                Anthony LESTER                                            */
/*                                                                          */
/****************************************************************************/

#include STB_H

#include "stb_error.h"
#include "stb_util.h"

/*****************************************************************************/
/*                        fonction stberror                                  */
/*****************************************************************************/
void 
stb_error(errorindex, message, number, type)
    int             errorindex;
    char           *message;
    int             number;
    int             type;
{
    int severity;

    if(STB_SILENT == 'Y' && type != STB_FATAL)
       return ;
    switch (type) {
    case STB_FATAL:
        severity=AVT_FATAL;
        break;
    case STB_NONFATAL:
        severity=AVT_ERROR;
        break;
    case STB_WARNING:
        severity=AVT_WARNING;
        break;
    }

    switch (errorindex) {
    case ERR_MULTIPLE_CLOCKS:
        avt_errmsg(STB_ERRMSG, "001", severity, message);
        break;
    case ERR_MULTIPLE_PHASES: 
        avt_errmsg(STB_ERRMSG, "002", severity, message);
        break;
    case ERR_BAD_MEMORY:
        avt_errmsg(STB_ERRMSG, "003", severity, message);
        break;
    case ERR_NO_CLOCK:
        avt_errmsg(STB_ERRMSG, "004", severity, message);
        break;
    case ERR_NO_DATA:
        avt_errmsg(STB_ERRMSG, "005", severity, message);
        break;
    case ERR_NO_RCLOCK:
        avt_errmsg(STB_ERRMSG, "006", severity, message);
        break;
    case ERR_NO_FCLOCK:
        avt_errmsg(STB_ERRMSG, "007", severity, message);
        break;
    case ERR_BAD_CLOCKS:
        avt_errmsg(STB_ERRMSG, "008", severity, message);
        break;
    case ERR_NO_CLOCK_CONNECTOR:
        avt_errmsg(STB_ERRMSG, "009", severity, message, number);
        break;
    case ERR_NO_CONNECTOR:
        avt_errmsg(STB_ERRMSG, "010", severity, message, number);
        break;
    case ERR_NO_COMMAND:
        avt_errmsg(STB_ERRMSG, "011", severity, message, number);
        break;
    case ERR_NO_INDEX:
        avt_errmsg(STB_ERRMSG, "012", severity, message);
        break;
    case ERR_MULTIPLE_COMMANDS:
        avt_errmsg(STB_ERRMSG, "013", severity, message);
        break;
    case ERR_MONOPHASE:
        avt_errmsg(STB_ERRMSG, "014", severity, message);
        break;
    case ERR_MULTIPLE_DOMAIN:
        avt_errmsg(STB_ERRMSG, "015", severity, message);
        break;
    case ERR_MULTIPLE_EQUIVALENCE:
        avt_errmsg(STB_ERRMSG, "016", severity, message);
        break;
    case ERR_DOMAIN_NOT_EQUIVALENT:
        avt_errmsg(STB_ERRMSG, "017", severity, message);
        break;
    case ERR_MULTIPLE_CKPATH:
        avt_errmsg(STB_ERRMSG, "018", severity, message);
        break;
    case ERR_MULTIPLE_CONSTRAINT:
        avt_errmsg(STB_ERRMSG, "019", severity, message);
        break;
    case ERR_CROSSING_DOMAIN:
        avt_errmsg(STB_ERRMSG, "020", severity, message);
        break;
    case ERR_UNDEFINED_PERIOD:
        avt_errmsg(STB_ERRMSG, "021", severity, message);
        break;
    case ERR_INCOHERENT_PERIOD:
        avt_errmsg(STB_ERRMSG, "022", severity, message);
        break;
    case ERR_CANNOT_OPEN:
        avt_errmsg(STB_ERRMSG, "023", severity, message);
        break;
    case ERR_CANNOT_CLOSE:
        avt_errmsg(STB_ERRMSG, "024", severity, message);
        break;
    case ERR_NO_FIGURE:
        avt_errmsg(STB_ERRMSG, "025", severity, message);
        break;
    case ERR_CANNOT_RUN:
        avt_errmsg(STB_ERRMSG, "026", severity, message);
        break;
    case ERR_SYNTAX:
        avt_errmsg(STB_ERRMSG, "027", severity, message);
        break;
    case ERR_UNKNOWN_PHASE:
        avt_errmsg(STB_ERRMSG, "028", severity, message);
        break;
    case ERR_UNMATCHED_INTERVALS:
        avt_errmsg(STB_ERRMSG, "029", severity, message);
        break;
    case ERR_UNKNOWN_CLOCK:
        avt_errmsg(STB_ERRMSG, "030", severity, message);
        break;
    case ERR_UNKNOWN_SUFFIX:
        avt_errmsg(STB_ERRMSG, "031", severity, message);
        break;
    case ERR_ONLY_HOLD:
        avt_errmsg(STB_ERRMSG, "032", severity, message);
        break;
    case ERR_ONLY_SETUP:
        avt_errmsg(STB_ERRMSG, "033", severity, message);
        break;
    case ERR_ENV:
        avt_errmsg(STB_ERRMSG, "034", severity, message);
        break;
    case ERR_NOT_FOUND_NODE:
        avt_errmsg(STB_ERRMSG, "035", severity, message);
        break;
    case ERR_NOT_FOUND_MEMORY:
        avt_errmsg(STB_ERRMSG, "036", severity, message);
        break;
    default:
        avt_errmsg(STB_ERRMSG, "037", severity, message);
        break;
    }
}

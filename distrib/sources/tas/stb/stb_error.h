/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STB Version 1.00                                            */
/*    Fichier : stb_error.h                                                 */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                Anthony LESTER                                            */
/*                                                                          */
/****************************************************************************/

#define STB_FATAL                 0
#define STB_NONFATAL              1
#define STB_WARNING               2

#define ERR_MULTIPLE_CLOCKS       100
#define ERR_NO_CLOCK              101
#define ERR_NO_INDEX              102
#define ERR_MULTIPLE_COMMANDS     103
#define ERR_BAD_CLOCKS            104
#define ERR_NO_CLOCK_CONNECTOR    105
#define ERR_NO_CONNECTOR          106
#define ERR_NO_COMMAND            107
#define ERR_MONOPHASE             108
#define ERR_MULTIPLE_PHASES       109
#define ERR_BAD_MEMORY            110
#define ERR_MULTIPLE_DOMAIN       111
#define ERR_MULTIPLE_EQUIVALENCE  112
#define ERR_DOMAIN_NOT_EQUIVALENT 113
#define ERR_MULTIPLE_CKPATH       114
#define ERR_CROSSING_DOMAIN       115
#define ERR_UNDEFINED_PERIOD      116
#define ERR_INCOHERENT_PERIOD     117
#define ERR_MULTIPLE_CONSTRAINT   118
#define ERR_ONLY_SETUP            119
#define ERR_ONLY_HOLD             120
#define ERR_NO_RCLOCK             121
#define ERR_NO_FCLOCK             122
#define ERR_NO_DATA               123
#define ERR_ENV                   124
#define ERR_NOT_FOUND_NODE        125
#define ERR_NOT_FOUND_MEMORY      126

#define ERR_CANNOT_OPEN           1000
#define ERR_CANNOT_CLOSE          1002
#define ERR_NO_FIGURE             1003
#define ERR_CANNOT_RUN            1008

#define ERR_SYNTAX                1001
#define ERR_UNKNOWN_PHASE         1004
#define ERR_UNKNOWN_CLOCK         1005
#define ERR_UNMATCHED_INTERVALS   1006
#define ERR_UNKNOWN_SUFFIX        1007

#define WAR_ALWAYS_STABLE         200

extern FILE *STB_STDERR ;

void stb_error __P((int errorindex, char *message, int number, int type));

/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : SPF Version 1.00                                            */
/*    Fichier : spf_annot.h                                                 */
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

#define KEEP__TRANSISTOR  0x1
#define KEEP__RESISTANCE  0x2
#define KEEP__INSTANCE  0x4
#define KEEP__NONE  0x8
#define KEEP__CAPA  0x20
#define KEEP__DIODE  0x40
#define KEEP__ALL  (KEEP__CAPA|KEEP__TRANSISTOR|KEEP__RESISTANCE|KEEP__INSTANCE|KEEP__DIODE|0x1000)

#define SPF_ORIG_CAPA_TOT 0xfab70213
#define SPF_ORIG_CAPA_SUM 0xfab70214

#include SPF_H
extern losig_list *groundlosig;
extern mbk_match_rules SPF_IGNORE_SIGNAL;
/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

extern int spf_ground (chain_list *groundlist, char *nodename) ;
void spf_prepare_lofig(lofig_list *ptlofig);
void spf_create_losig_htable(lofig_list *lofig);
void spf_destroy_losig_htable();
losig_list *getlosigbyname(char *name);
long findindexinlofig(lofig_list *lofig);
void getsignalandnode(lofig_list *Lofig, losig_list *currentsignal, char *nametofind, ht *nodelist, char *ground, losig_list **signalfound, long *nodefound, int noforce);
void spf_destroyexistingRC(lofig_list *lofig);
char *spf_spi_devect( char *nom );
loins_list *getloinsbyname(char *name);
void spf_destroy_loins_htable();
void spf_destroy_lotrs_htable();
void spf_create_loins_htable(lofig_list *lofig);
void spf_create_lotrs_htable(lofig_list *lofig);
extern char *spf_deqstring (char *str);
extern char *spf_rename (char *str);

lotrs_list *getlotrsbyname(char *name);
void spf_initBUSDelimiters(char *leftb, char *rightb);
void spf_setBUSDelimiters(char leftb, char rightb);
void spf_setDelimiter(char c);
void spf_setDivider(char c);
char spf_getDivider();
char spf_getDelimiter();
void spf_checklofigsignal(lofig_list *lf);


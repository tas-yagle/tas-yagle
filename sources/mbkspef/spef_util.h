/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : SPEF Version 1.00                                           */
/*    Fichier : spef_annot.h                                                */
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

#define SPEF_RESISIG 0xfab70214
#define SPEF_ATTRIB_HT 0xfab70221

#include SPE_H
extern it* namemaptable;
extern ht* ht_con;
extern ht* portmap_ht;
extern losig_list *spef_groundlosig;
/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

extern void spef_create_losig_htable(lofig_list *lofig);
extern void spef_destroy_losig_htable();
extern losig_list *spef_getlosigbyname(char *name);
extern long spef_findindexinlofig(lofig_list *lofig);
extern void spef_destroyexistingRC(lofig_list *lofig);
extern void spef_parser_error(char *message);
extern char *spef_spi_devect( char *nom );
extern loins_list *spef_getloinsbyname(char *name);
extern void spef_destroy_loins_htable();
extern void spef_create_loins_htable(lofig_list *lofig);
extern char *spef_rename (char *str);
extern char spef_recupnodeandsig (lofig_list *Lofig, char *str, char *ground, losig_list **ptsig, long *node, long **pnode);
extern chain_list *spef_addloctc(chain_list *ctclist,losig_list *sig1,losig_list *sig2,long node1,long node2,float value, loctc_list **lctc);
extern char *spef_deqstring (char *str);
extern void spef_treatafterctclist(chain_list **ctclist);
extern void spef_remove_htctc();
extern char spef_sufdelim(char delim);
extern void spef_initinfo(spef_info *info);
extern void spef_createinfo();
lotrs_list *spef_getlotrsbyname(char *name);
void spef_ht_stats();
void spef_setcurnet(char *name, losig_list *ls);
losig_list *spef_getcurnet();
char *spef_check_keep(char *name, char ch, int nalloc);
extern int SPEF_CARDS, SPEF_PRESERVE_RC;
extern int spef_quiet;

void spef_examine_set_nodes(losig_list *ls);
void spef_examine_add_shortcircuit(int num, locon_list *lc);
void spef_checkpnode(locon_list *lc, long *node);
void spef_checkpnode_parasitic(long *nm, losig_list *ls, long *node);

lowire_list *spef_getresibyname(char *name);
loctc_list *spef_getcapabyname(char *name);


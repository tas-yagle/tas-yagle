/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : Grammar for Verilog                                         */
/*    Fichier : mgl_util.h                                                  */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Anthony LESTER                                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

/* function prototypes */

#ifdef __cplusplus
extern "C" {
#endif
void                mgl_deltab(struct dct_entry **head, char *key_str, char *ctx_str);
struct dct_entry  **mgl_initab();
void                mgl_addtab(struct dct_entry **head, char *key_str, char *ctx_str, int field, long valu);
long                 mgl_chktab(struct dct_entry **head, char *key_str, char *ctx_str, int field);
void                mgl_fretab(struct dct_entry **pt_hash);
char               *mgl_avers();
char               *mgl_vlgname(char *name);
void                mgl_treatname(char *name, char *new_name);
void               *mgl_vectnam(void *pt_list, int *left, int *right, char **name, char type);
lofig_list         *mgl_fill(struct lofig *lofig_P, struct lofig *lofig_A);
losig_list         *mgl_addlosig(struct lofig *ptfig, int index, char type, char ptype, char *name, int left, int right);
locon_list         *mgl_addlocon(struct lofig *ptfig, losig_list *ptsig, char dir, char *name, int left, int right);
locon_list         *mgl_orientlocon(struct lofig *ptfig, char dir, char *name, int left, int right);
losig_list         *mgl_getlosig(struct lofig *ptfig, char *name, struct dct_entry **hshtab, mgl_scompcontext *context);
loins_list         *mgl_addloins(struct lofig *ptfig, char *modelname, char *insname, chain_list *loconnames, chain_list *sigchain);
void                mgl_assign(lofig_list *ptfig, char *lident, char *rident, struct dct_entry **hshtab, mgl_scompcontext *context);
chain_list         *mgl_sortsig(lofig_list *ptmodel, char *insname, chain_list *loconnames, chain_list *sigchain, lofig_list *ptfig, int *ptindex);
char               *mgl_getcasename(ht *caseht, char *text);
void                mgl_getloconrange(locon_list *ptheadlocon, char *name, int *left, int *right);
losig_list         *mgl_givevdd(lofig_list *ptfig, char *insname, int *ptindex);
losig_list         *mgl_givevss(lofig_list *ptfig, char *insname, int *ptindex);
void                mgl_clean_lorcnet(lofig_list *ptfig);
#ifdef __cplusplus
}
#endif



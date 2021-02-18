/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : AVERTEC global tools                                        */
/*    Fichier : mbk_vector.h                                                */
/*                                                                          */
/*    © copyright 2000 AVERTEC                                              */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Antony PINTO                                              */
/*                                                                          */
/****************************************************************************/

#ifndef MBK_VECTOR_H
#define MBK_VECTOR_H

#include MUT_H

typedef struct mbv_tab
{
  int       LEFT;
  int       RIGHT;
  int       CPT;
  union 
  {
    int *NUM;
    char **NOM;
    void *TAB;
  } TABLE;
}
mbv_tab;

void     mbv_addAtIndex (mbv_tab *table, char *corresp, int index);

mbv_tab *mbv_initName   (ht *tbl, char *key, int left, int right);
int      mbv_addCorresp (ht *tbl, char *key, char *corresp);
mbv_tab *mbv_get        (ht *tbl, char *key);
void     mbv_free       (ht *tbl, char *key);
void mbv_freeht(ht *source);
char    *mbv_getCorresp (ht *tbl, char *key, char *buf);
char    *mbv_getCorrespAndDel(ht *tbl, char *key, char *buf);
void     mbv_prefix     (ht *tbl, char *prefix);
void     mbv_changeRef  (ht *orig, ht *dest, chain_list *orc, chain_list *dec);
void     mbv_useAdvancedNameAllocator(void);
void     mbv_dumpCorresp(ht *table);
void     mbv_setNoCorrepondance(char *noCorrespString);
ht *mbv_duplicateht(ht *source);
#endif

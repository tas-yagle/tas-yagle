/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : AVERTEC global tools                                        */
/*    Fichier : mbk_fileextract.h                                           */
/*                                                                          */
/*    © copyright 2000 AVERTEC                                              */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Antony PINTO                                              */
/*                                                                          */
/****************************************************************************/

#ifndef MBK_FILEEXTRACT_H
#define MBK_FILEEXTRACT_H

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include MUT_H

typedef struct label_list
{
  char                  *LABEL;
  double                 VALUE;
}
label_list;

typedef struct extract_rules
{
  struct extract_rules  *NEXT;      /* next rule(s)                         */
  char                  *BEGIN;     /* mark begin for rule recognition      */
  char                  *END;       /* mark end for rule recognition        */ 
  int                    TYPE;      /* recognition type : line or array     */
  char                 **TOKEN;     /* list of token to be recognised       */
  int                    NBTOKEN;   /* number of token                      */
  int                    ACTIVATED; /* rule activated or not                */
  int                    LINE;      /* line # in case of table recognition  */
  label_list            *LBLLIST;   /* list of label recognised             */
  int                    NBELT;     /* nb token found in a line for a table */
  int                    NBELT1ST;  /* nb token found in the 1st line for a */
                                    /* table                                */
}
extract_rules;

typedef struct label_table
{
  struct label_table    *NEXT;
  char                  *LABEL;
  union {
    double   NUMBER;
    struct {
      double  *TABLE;
      int      NBMC;
      int      MAXMC;
    } MC ;
  } VALUE ;
  int                    FLAG;      /* is label found                       */
}
label_table;

typedef struct label_ref
{
  label_table           *LABTBL;
  ht                    *REFTBL;
}
label_ref;

#define         RULE_UNACTIVATED   ((int) 0)
#define         RULE_ACTIVATED     ((int) 1)

#define         RULE_TYPE_LINE     ((int) 4)
#define         RULE_TYPE_TABLE    ((int) 5)

#define         MFE_CHAR_IN_LINE   ((int) 3000)
#define         MFE_LINE_SIZE      (sizeof(char)*MFE_CHAR_IN_LINE)

#define         LABL_FOUND         ((int) 0)
#define         LABL_NOTFOUND      ((int) 1)
/* error code */

#define         RULE_TYPE_ERR      ((int) 6)  /* rule type          */
#define         RULE_BEGE_ERR      ((int) 7)  /* begin an end mark  */
#define         RULE_RULE_ERR      ((int) 8)  /* token              */
#define         RULE_TOOT_ERR      ((int) 9)  /* too much token     */
#define         RULE_UTOK_ERR      ((int) 10) /* unknown token      */ 
#define         RULE_MATC_ERR      ((int) 11) /* match error        */
#define         RULE_FTOK_ERR      ((int) 12) /* miss token err     */
#define         LABL_UTBL_ERR      ((int) 31) /* unknown table      */
#define         LABL_ULBL_ERR      ((int) 32) /* unknown label      */
#define         LABL_LEXI_WAR      ((int) 33) /* existant label     */
#define         LABL_PLBL_ERR      ((int) 34) /* label probleme     */
#define         MFE_LONGLINE_ERR   ((int) 55)

int  mfe_addrule(extract_rules **ruleslist, char *rule);
int  mfe_delrule(extract_rules **ruleslist, extract_rules *rule);
void mfe_freerules(extract_rules **ruleslist);
void mfe_duprules(extract_rules **ruleslist, extract_rules *rule);
/* be carefull label must be nameallocated */
int  mfe_addlabel(label_ref **table, char *label, int nbmc);
int  mfe_dellabel(label_ref **table, char *label);
void mfe_freelabels(label_ref **table, int cleanmc);
void mfe_duplabels(label_ref **newr, label_ref *ref);

int  mfe_fileextractlabel(char *filename, extract_rules *rules, label_ref *ref, int nbmc);

int  mfe_labelvalue(label_ref *ref, char *label, double *value, int nbmc);
void mfe_cleanLabel(label_ref *ref);

#endif

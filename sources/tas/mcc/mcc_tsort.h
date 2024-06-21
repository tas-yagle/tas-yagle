#include <stdlib.h>
#include <string.h>
#include MUT_H
#include AVT_H
#include MLO_H
#include MLU_H

//-------------------------------------------------------------------------

typedef struct tlist_t {
    struct tlist_t *NEXT;
    char           *MODEL;
    ht             *TRANSHT;
    chain_list     *SIZES;
    chain_list     *MSIZES;
} tlist_t;

typedef struct couple_t {
    struct couple_t *NEXT;
    char            *PMODEL;
    char            *NMODEL;
    char            *PSIZE;
    char            *NSIZE;
} couple_t;

extern int       mcc_GetL             (char *size);
extern int       mcc_GetW             (char *size);
extern char     *mcc_GetCoupledPTrans (char *model, chain_list *plist, chain_list *nlist);
extern char     *mcc_GetCoupledNTrans (char *model, chain_list *plist, chain_list *nlist);
extern int       mcc_IsACouple        (int lp, int ln, int wp, int wn);
extern couple_t *mcc_MakeCouples      (tlist_t *models);
extern tlist_t  *mcc_SortModels       (lofig_list *fig, double m);
extern void      mcc_CleanModels      (tlist_t *models);
extern void      mcc_DumpModels       (tlist_t *models);
extern void      mcc_DumpCouples      (couple_t *head_couple);
extern char     *mcc_DumpScript       (couple_t *couples,char *options);
extern char     *mcc_MakeScript (lofig_list *fig, double m, char *options);

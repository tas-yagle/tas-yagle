/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : Structural Verilog Compiler                                 */
/*    Fichier : mgl_util.c                                                  */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Anthony LESTER                                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include MUT_H
#include AVT_H
#include MLO_H
#include RCN_H
#include "mgl_type.h"
#include "mgl_parse.h"
#include "mgl_util.h"
#include "mgl_error.h"

static struct dct_entry  *MGL_DCEHED;	/* free dct_entry's head	*/
static struct dct_recrd  *MGL_DCRHED;	/* free dct_recrd's head	*/

static struct dct_entry *mgl_addent(struct dct_entry *head, char *key);
static struct dct_recrd *mgl_addrcd(struct dct_recrd *head, char *key);

// copie de mbk_util.c
#define HASH_MULT 314159
#define HASH_PRIME 516595003

static int HASH_FUNC(char *inputname, char *name, int code)
{
   do { 
      while (*inputname) {
         /*if (CASE_SENSITIVE == 'N') *name = tolowertable[(int)*inputname++];
         else*/ *name = *inputname++;
         code += (code ^ (code >> 1)) + HASH_MULT * (unsigned char) *name++;
         while (code >= HASH_PRIME)
            code -= HASH_PRIME;
      }
      *name = '\0';
      code %= MGL_HSZDFN;
   } while (0);

   return code;
}


/* ###--------------------------------------------------------------### */
/*  function : mgl_deltab                                               */
/* ###--------------------------------------------------------------### */

void
mgl_deltab(struct dct_entry **head, char *key_str, char *ctx_str)
{
    int             found = 0;
    int             index;
    struct dct_entry *entry_pnt;
    struct dct_entry *last_entry = NULL;
    struct dct_recrd *recrd_pnt;
    struct dct_recrd *last_recrd = NULL;
    char name[500];

    // par Fabrice le 11/2/2002
    //index = (int) key_str % MGL_HSZDFN;
    index=HASH_FUNC(key_str, name, 0);
    entry_pnt = head[index];

    while (entry_pnt != NULL) {
        if (entry_pnt->key == key_str) {
            found = 1;
            break;
        }
        last_entry = entry_pnt;
        entry_pnt = entry_pnt->next;
    }

    if (found == 1) {
        found = 0;
        recrd_pnt = entry_pnt->data;
        while (recrd_pnt != NULL) {
            if (recrd_pnt->key == ctx_str) {
                found = 1;
                break;
            }
            last_recrd = recrd_pnt;
            recrd_pnt = recrd_pnt->next;
        }

        if (found == 1) {
            if (last_recrd == NULL)
                entry_pnt->data = recrd_pnt->next;
            else
                last_recrd->next = recrd_pnt->next;

            recrd_pnt->next = MGL_DCRHED;
            MGL_DCRHED = recrd_pnt;

            if (entry_pnt->data == NULL) {
                if (last_entry == NULL)
                    head[index] = entry_pnt->next;
                else
                    last_entry->next = entry_pnt->next;

                entry_pnt->next = MGL_DCEHED;
                MGL_DCEHED = entry_pnt;
            }
        }
    }
}

/* ###--------------------------------------------------------------### */
/*  function : mgl_initab                                               */
/* ###--------------------------------------------------------------### */
struct dct_entry **
mgl_initab()
 {
    struct dct_entry **head;
    int             i;

    head = (struct dct_entry **)
        mbkalloc(sizeof(struct dct_entry *) * MGL_HSZDFN);

    for (i = 0; i < MGL_HSZDFN; i++)
        head[i] = NULL;

    return (head);
}

/* ###--------------------------------------------------------------### */
/*  function : mgl_addtab                                               */
/* ###--------------------------------------------------------------### */
void
mgl_addtab(struct dct_entry **head, char *key_str, char *ctx_str, int field, long valu)
{
    int             found = 0;
    int             index;
    struct dct_entry *entry_pnt;
    struct dct_recrd *recrd_pnt;
    char name[500];
    
    // par Fabrice le 11/2/2002
    //index = (int) key_str % MGL_HSZDFN;
    index=HASH_FUNC(key_str, name, 0);

    entry_pnt = head[index];

    while (entry_pnt != NULL) {
        if (entry_pnt->key == key_str) {
            found = 1;
            break;
        }
        entry_pnt = entry_pnt->next;
    }

    if (found == 0) {
        head[index] = mgl_addent(head[index], key_str);
        entry_pnt = head[index];
    }

    found = 0;
    recrd_pnt = entry_pnt->data;
    while (recrd_pnt != NULL) {
        if (recrd_pnt->key == ctx_str) {
            found = 1;
            break;
        }
        recrd_pnt = recrd_pnt->next;
    }

    if (found == 0) {
        entry_pnt->data = mgl_addrcd(entry_pnt->data, ctx_str);
        recrd_pnt = entry_pnt->data;
    }

    switch (field) {
    case 0:
        recrd_pnt->fd0_val = valu;
        break;
    case 1:
        recrd_pnt->fd1_val = valu;
        break;
    case 2:
        recrd_pnt->fd2_val = valu;
        break;
    case 3:
        recrd_pnt->fd3_val = valu;
        break;
    case 4:
        recrd_pnt->fd4_val = valu;
        break;
    case 5:
        recrd_pnt->fd5_val = valu;
        break;
    case 6:
        recrd_pnt->fd6_val = valu;
        break;
    case 7:
        recrd_pnt->pnt_val = valu;
        break;
    }

}

/* ###--------------------------------------------------------------### */
/*  function : mgl_chktab                                               */
/* ###--------------------------------------------------------------### */
long
mgl_chktab(struct dct_entry **head, char *key_str, char *ctx_str, int field)
{
    int             found = 0;
    long             valu = 0;
    struct dct_entry *entry_pnt;
    struct dct_recrd *recrd_pnt;
    char name[500];
    
    // par Fabrice le 11/2/2002
    //    entry_pnt = head[(int) key_str % MGL_HSZDFN];
    entry_pnt = head[HASH_FUNC(key_str, name, 0)];

    while (entry_pnt != NULL) {
        if (entry_pnt->key == key_str) {
            found = 1;
            break;
        }
        entry_pnt = entry_pnt->next;
    }

    if (found == 1) {
        found = 0;
        recrd_pnt = entry_pnt->data;
        while (recrd_pnt != NULL) {
            if (recrd_pnt->key == ctx_str) {
                found = 1;
                break;
            }
            recrd_pnt = recrd_pnt->next;
        }
        if (found == 1) {
            switch (field) {
            case 0:
                valu = recrd_pnt->fd0_val;
                break;
            case 1:
                valu = recrd_pnt->fd1_val;
                break;
            case 2:
                valu = recrd_pnt->fd2_val;
                break;
            case 3:
                valu = recrd_pnt->fd3_val;
                break;
            case 4:
                valu = recrd_pnt->fd4_val;
                break;
            case 5:
                valu = recrd_pnt->fd5_val;
                break;
            case 6:
                valu = recrd_pnt->fd6_val;
                break;
            case 7:
                valu = recrd_pnt->pnt_val;
                break;
            }
        }
    }

    return (valu);
}

/* ###--------------------------------------------------------------### */
/*  function : mgl_fretab                                               */
/* ###--------------------------------------------------------------### */
void
mgl_fretab(struct dct_entry **pt_hash)
{
    struct dct_entry *pt_entry;
    struct dct_entry *pt_nxtentry;
    struct dct_recrd *pt_record;
    int             i;

    if (pt_hash != NULL) {
        for (i = 0; i < MGL_HSZDFN; i++) {
            if ((pt_entry = pt_hash[i]) != NULL) {
                while (pt_entry != NULL) {
                    pt_record = pt_entry->data;

                    while (pt_record->next != NULL)
                        pt_record = pt_record->next;

                    pt_record->next = MGL_DCRHED;
                    MGL_DCRHED = pt_entry->data;

                    pt_nxtentry = pt_entry->next;
                    pt_entry->next = MGL_DCEHED;
                    MGL_DCEHED = pt_entry;
                    pt_entry = pt_nxtentry;
                }
            }
        }
        mbkfree(pt_hash);
    }
}

/* ###--------------------------------------------------------------### */
/*  function : mgl_addent                                               */
/* ###--------------------------------------------------------------### */
static struct dct_entry *
mgl_addent(struct dct_entry *head, char *key)
{
    struct dct_entry *entry;
    int             i;

    if (MGL_DCEHED == NULL) {
        MGL_DCEHED = (struct dct_entry *) mbkalloc(sizeof(struct dct_entry) * MGL_ALODFN);

        entry = MGL_DCEHED;
        for (i = 1; i < MGL_ALODFN; i++) {
            entry->next = entry + 1;
            entry++;
        }
        entry->next = NULL;
    }

    entry = MGL_DCEHED;
    MGL_DCEHED = MGL_DCEHED->next;

    entry->next = head;
    entry->data = NULL;
    entry->key = key;

    return (entry);
}

/* ###--------------------------------------------------------------### */
/*  function : mgl_addrcd                                               */
/* ###--------------------------------------------------------------### */
static struct dct_recrd *
mgl_addrcd(struct dct_recrd *head, char *key)
{
    struct dct_recrd *recrd;
    int             i;

    if (MGL_DCRHED == NULL) {
        MGL_DCRHED = (struct dct_recrd *) mbkalloc(sizeof(struct dct_recrd) * MGL_ALODFN);

        recrd = MGL_DCRHED;
        for (i = 1; i < MGL_ALODFN; i++) {
            recrd->next = recrd + 1;
            recrd++;
        }
        recrd->next = NULL;
    }

    recrd = MGL_DCRHED;
    MGL_DCRHED = MGL_DCRHED->next;

    recrd->next = head;
    recrd->fd0_val = 0;
    recrd->fd1_val = 0;
    recrd->fd2_val = 0;
    recrd->fd3_val = 0;
    recrd->fd4_val = 0;
    recrd->fd5_val = 0;
    recrd->fd6_val = 0;
    recrd->pnt_val = 0;
    recrd->key = key;

    return (recrd);
}

/* ###--------------------------------------------------------------### */
/*  function : mgl_avers                                                */
/* ###--------------------------------------------------------------### */
char *
mgl_avers()
{
    return ("-- V 1.0 --");
}

/* ###--------------------------------------------------------------### */
/*  function : mgl_vlgname                                              */
/* ###--------------------------------------------------------------### */
char *
mgl_vlgname(char *name)
{
    char           *new_name;
    char           *prv_name;
    char           *tmp_name;
    char            buffer[200];
    int             i, j, flag, number;
    static struct dct_entry **namtab = NULL;

    if (namtab == NULL)
        namtab = mgl_initab();

    tmp_name = namealloc(name);
    new_name = (char *) mgl_chktab(namtab, tmp_name, NULL, MGL_PNTDFN);

    if (mgl_chktab(namtab, tmp_name, NULL, MGL_NAMDFN) == 0) {
        i = 0;
        j = 0;
        number = 0;
        flag = 1;
        while (tmp_name[i] != '\0') {
            buffer[j] = tmp_name[i];
            if (((tmp_name[i] >= 'a') && (tmp_name[i] <= 'z')) ||
                ((tmp_name[i] >= 'A') && (tmp_name[i] <= 'Z')) ||
                ((tmp_name[i] >= '0') && (tmp_name[i] <= '9') && (i != 0)) ||
                ((tmp_name[i] == '(') || (tmp_name[i] == ')'))) {
                flag = 0;
            }
            else if ((tmp_name[i] >= '0') && (tmp_name[i] <= '9') && (i == 0)) {
                strcpy(&buffer[j], "noname");
                j += 6;
                buffer[j] = tmp_name[i];
            }
            else {
                if (flag == 1)
                    buffer[j++] = 'v';
                buffer[j] = '_';
                flag = 1;
            }
            i++;
            j++;
        }
        if (buffer[j - 1] == '_')
            j--;
        buffer[j] = '\0';
        new_name = namealloc(buffer);

        prv_name = new_name;
        while (mgl_chktab(namtab, new_name, NULL, MGL_NEWDFN) != 0) {
            new_name = prv_name;
            sprintf(buffer, "%s_%d", new_name, number++);
            prv_name = new_name;
            new_name = namealloc(buffer);
        }
        mgl_addtab(namtab, new_name, NULL, MGL_NEWDFN, 1);
        mgl_addtab(namtab, tmp_name, NULL, MGL_PNTDFN, (long) new_name);
        mgl_addtab(namtab, tmp_name, NULL, MGL_NAMDFN, 1);
    }

    return (new_name);
}


/* ###--------------------------------------------------------------### */
/*  function : mgl_treatname                                            */
/* ###--------------------------------------------------------------### */
void
mgl_treatname(char *name, char *new_name)
{
    char           *blank_space;

    /* Transformation des blancs en parentheses */
    strcpy(new_name, name);
    blank_space = strchr(new_name, ' ');
    if (blank_space != NULL) {
        *blank_space = '(';
        blank_space = strchr(new_name, '\0');
        /* Transformation du dernier caractere en ) */
        if (blank_space != NULL) {
            *blank_space = ')';
            blank_space++;
            *blank_space = '\0';
        }
    }
    strcpy(new_name, mgl_vlgname(new_name));
}

/* ###--------------------------------------------------------------### */
/*  function : mgl_vectnam                                              */
/* ###--------------------------------------------------------------### */
void *
mgl_vectnam(void *pt_list, int *left, int *right, char **name, char type)
{
    char           *blank_space;
    char           *sig_name;
    char            name_tmp[200];
    char            number[200];
    losig_list     *ptsig;
    locon_list     *ptcon;
    char            END = 0;

    /* Case losig_list */
    if (type == 0) {
        ptsig = (losig_list *) pt_list;
        if (ptsig->TYPE == 'I') {
            *left = *right = -1;
            sig_name = getsigname(ptsig);
            *name = (char *) mbkalloc(strlen(sig_name) + 1);
            strcpy(*name, sig_name);
            blank_space = strchr(*name, ' ');
            if (blank_space != NULL) {
                strcpy(number, blank_space);
                *right = atoi(number);
                *left = *right;
                *blank_space = '\0';
            }

            while (!END) {
                if (ptsig->NEXT != NULL) {
                    strcpy(name_tmp, getsigname(ptsig->NEXT));
                    blank_space = strchr(name_tmp, ' ');
                    if (blank_space != NULL) {
                        strcpy(number, blank_space);
                        *blank_space = '\0';
                        if (!strcmp(*name, name_tmp)) {
                            *left = atoi(number);
                            ptsig = ptsig->NEXT;
                        }
                        else END = 1;
                    }
                    else END = 1;
                }
                else END = 1;
            }
            return (ptsig);
        }
        else {
            *name = NULL;
            return (ptsig);
        }
    }

    /*case locon_list */
    if (type == 1) {
        ptcon = (locon_list *) pt_list;
        /* Extract the name and number of an element */
        *left = *right = -1;
        sig_name = ptcon->NAME;
        *name = (char *) mbkalloc(strlen(sig_name) + 1);
        strcpy(*name, sig_name);
        blank_space = strchr(*name, ' ');
        if (blank_space != NULL) {
            strcpy(number, blank_space);
            *right = atoi(number);
            *left = *right;
            *blank_space = '\0';
        }

        while (END != 1) {
            if (ptcon->NEXT != NULL) {
                strcpy(name_tmp, ptcon->NEXT->NAME);
                blank_space = strchr(name_tmp, ' ');
                if (blank_space != NULL) {
                    strcpy(number, blank_space);
                    *blank_space = '\0';
                    if (!strcmp(*name, name_tmp)) {
                        *right = atoi(number);
                        ptcon = ptcon->NEXT;
                    }
                    else END = 1;
                }
                else END = 1;
            }
            else END = 1;
        }
        return (ptcon);
    }
    /* To avoid Warning from GCC */
    return (NULL);
}

/* ###--------------------------------------------------------------### */
/*  function : mgl_fill                                                 */
/*  content  : Fill a lofig of mode 'P' with another lofig of mode 'A'  */
/* ###--------------------------------------------------------------### */
struct lofig *
mgl_fill(struct lofig *lofig_P, struct lofig *lofig_A)
{
    struct locon   *ptlocon_P, *ptlocon_A;
    struct chain   *ptchain;
    struct lofig   *ptlofig;
    struct losig   *ptlosig;

    /* MODELCHAIN */
    ptchain = lofig_P->MODELCHAIN;
    lofig_P->MODELCHAIN = lofig_A->MODELCHAIN;

    /* LOCON */
    ptlocon_P = lofig_P->LOCON;
    ptlocon_A = lofig_A->LOCON;

    while (ptlocon_A != NULL) {
        if (ptlocon_A->NAME == ptlocon_P->NAME) {
            ptlocon_P->SIG = ptlocon_A->SIG;
        }
        else {
            
                fprintf(stderr,
                               "\n*** mbk error *** bad consistency in figure %s,\n external interface are different\n",
                               lofig_P->NAME);
        }
        ptlocon_A = ptlocon_A->NEXT;
        ptlocon_P = ptlocon_P->NEXT;
    }

    /* LOSIG */
    ptlosig = lofig_P->LOSIG;
    lofig_P->LOSIG = lofig_A->LOSIG;

    /* LOINS */
    lofig_P->LOINS = lofig_A->LOINS;

    /* LOTRS */
    lofig_P->LOTRS = lofig_A->LOTRS;

    /* USER  */
    lofig_P->USER = lofig_A->USER;

    /* MODE  */
    lofig_P->MODE = 'A';

    /* Freeing the memory zone unusable */

    freechain(ptchain);

    while (lofig_A->LOCON != NULL) {
        dellocon(lofig_A, lofig_A->LOCON->NAME);
    }

    ptlofig = addlofig(" bidon");
    ptlofig->LOSIG = ptlosig;
    dellofig(ptlofig->NAME);

    return (lofig_P);
}

/* ###--------------------------------------------------------------### */
/* function     : mgl_addlosig                                          */
/* description  : create one or more losig structures (for an array a   */
/*                losig is created for each bit)                        */
/* called func. : addlosig, addchain                                    */
/* ###--------------------------------------------------------------### */

losig_list *
mgl_addlosig(lofig_list *ptfig, int index, char type, char ptype, char *name, int left, int right)
{
    char            extname[1024];
    short           i;
    short           inc = 1;
    struct chain   *pt_chlst;
    struct losig   *ptsig;

    if ((left == -1) && (right == -1)) {
        pt_chlst = addchain(NULL, name);
        ptsig = addlosig(ptfig, index, pt_chlst, type);

        if (ptype != '0')
            ptsig->USER = addptype(ptsig->USER, ptype, NULL);
    }
    else {
        if (left >= right)      /* array */
            inc = -1;

        for (i = left; i != (right + inc); i += inc) {
            if (MBK_DEVECT) sprintf(extname, "%s %d", name, i);
            else sprintf(extname, "%s[%d]", name, i);
            pt_chlst = addchain(NULL, extname);
            ptsig = addlosig(ptfig, index, pt_chlst, type);

            if (ptype != '0')
                ptsig->USER = addptype(ptsig->USER, ptype, NULL);

            index++;
        }
    }
    return (ptsig);
}

/* ###--------------------------------------------------------------### */
/* function     : mgl_addlocon                                          */
/* description  : create one or more locon structures (for an array a   */
/*                locon is created for each bit)                        */
/* called func. : addlocon, addchain                                    */
/* ###--------------------------------------------------------------### */

locon_list *
mgl_addlocon(lofig_list *ptfig, losig_list *ptsig, char dir, char *name, int left, int right)
{
    char            extname[1024];
    short           i;
    short           inc = 1;
    struct locon   *ptcon;
    struct locon   *ptcontmp;

    if ((left == -1) && (right == -1)) {
        ptcon = addlocon(ptfig, name, ptsig, dir);
    }
    else {
        if (left >= right)
            inc = -1;

        for (i = left; i != (right + inc); i += inc) {
            if (MBK_DEVECT) sprintf(extname, "%s %d", name, i);
            else sprintf(extname, "%s[%d]", name, i);
            ptcon = addlocon(ptfig, extname, NULL, dir);
        }

        if (ptsig != NULL) {
            ptcontmp = ptcon;
            for (i = left; i != (right + inc); i += inc) {
                ptcontmp->SIG = ptsig;
                ptcontmp = ptcontmp->NEXT;
                ptsig = ptsig->NEXT;
            }
        }
    }
    return (ptcon);
}

/* ###--------------------------------------------------------------### */
/* function     : mgl_orientlocon                                       */
/* description  : create one or more locon structures (for an array a   */
/*                locon is created for each bit)                        */
/* called func. : addlocon, addchain                                    */
/* ###--------------------------------------------------------------### */

void mgl_clean_lorcnet(lofig_list *ptfig)
{
  losig_list *ls;
  
  for (ls=ptfig->LOSIG; ls!=NULL; ls=ls->NEXT)
  {
    if (ls->PRCN) freelorcnet(ls);
  }
}


locon_list *
mgl_orientlocon(lofig_list *ptfig, char dir, char *name, int left, int right)
{
    char            extname[1024];
    short           i;
    short           inc = 1;
    struct locon   *ptcon;

    if ((left == -1) && (right == -1)) {
        ptcon = getlocon(ptfig, name);
        ptcon->DIRECTION = dir;
    }
    else {
        if (left >= right)
            inc = -1;

        for (i = left; i != (right + inc); i += inc) {
            if (MBK_DEVECT) sprintf(extname, "%s %d", name, i);
            else sprintf(extname, "%s[%d]", name, i);
            ptcon = getlocon(ptfig, extname);
            ptcon->DIRECTION = dir;
        }
    }
    return (ptcon);
}

/* ###--------------------------------------------------------------### */
/* function     : mgl_getlosig                                          */
/* ###--------------------------------------------------------------### */

losig_list *
mgl_getlosig(lofig_list *ptfig, char *name, struct dct_entry **hshtab, mgl_scompcontext *context)
{
    char        extname[1024];
    losig_list *ptlosig;
    locon_list *ptcon;
    char       *radical;
    int         left, right, index;
    int         count;

    if ((radical = vectorradical(name)) == name) {
        ptlosig = (losig_list *)mgl_chktab(hshtab, name, ptfig->NAME, MGL_PNTDFN);
    }
    else {
        ptlosig = (losig_list *)mgl_chktab(hshtab, radical, ptfig->NAME, MGL_PNTDFN);
        left = mgl_chktab(hshtab, radical, ptfig->NAME, MGL_LFTDFN);
        right = mgl_chktab(hshtab, radical, ptfig->NAME, MGL_RGTDFN);
        index = vectorindex(name);
        if (!MBK_DEVECT) {
            sprintf(extname, "%s[%d]", radical, index);
            name = namealloc(extname);
        }
        if ((index <= left && index >= right) || (index <= right && index >= left)) {
            count = abs(index - right);
            while (count-- > 0 && ptlosig != NULL) ptlosig = ptlosig->NEXT;
        }
        else avt_errmsg(MGL_ERRMSG, "005", AVT_ERROR, name, context->LINENUM);
        if (ptlosig == NULL || ptlosig->NAMECHAIN->DATA != name) {
            ptcon = getlocon(ptfig, name);
            if (ptcon != NULL) {
                ptlosig = ptcon->SIG;
            }
            else ptlosig = mbk_quickly_getlosigbyname(ptfig, name);
        }
    }
    return ptlosig;
}

/* ###--------------------------------------------------------------### */
/* function     : mgl_addloins                                          */
/* ###--------------------------------------------------------------### */

loins_list *
mgl_addloins(struct lofig *ptfig, char *modelname, char *insname, chain_list *loconnames, chain_list *sigchain)
{
    loins_list *ptloins;
    locon_list *ptlocon;
    chain_list *ptchain, *ptchain1;

    ptloins = (loins_list *)mbkalloc(sizeof(loins_list));

    ptloins->INSNAME = insname;
    ptloins->FIGNAME = modelname;
    ptloins->LOCON = NULL;
    ptloins->USER = NULL;
    ptloins->NEXT = ptfig->LOINS;
    ptfig->LOINS = ptloins;

	/* update model list   */
	for (ptchain = ptfig->MODELCHAIN; ptchain; ptchain = ptchain->NEXT)
	   if (ptchain->DATA == (void *)modelname)
	      break;

	if (!ptchain)
	   ptfig->MODELCHAIN = addchain(ptfig->MODELCHAIN, (void *)modelname);

    for (ptchain = sigchain, ptchain1 = loconnames; ptchain && ptchain1; ptchain = ptchain->NEXT, ptchain1 = ptchain1->NEXT) {
        ptlocon = (locon_list *)mbkalloc(sizeof(locon_list));
        ptlocon->NAME = (char *)ptchain1->DATA;
        ptlocon->DIRECTION = UNKNOWN;
        ptlocon->TYPE = 'I';
        ptlocon->SIG = (losig_list *)ptchain->DATA;
        ptlocon->ROOT = ptloins;
        ptlocon->USER = NULL;
        ptlocon->PNODE = NULL;
        ptlocon->NEXT = ptloins->LOCON;
        ptloins->LOCON = ptlocon;
    }

    return ptloins;
}

/* ###--------------------------------------------------------------### */
/* function     : mgl_assign                                            */
/* ###--------------------------------------------------------------### */

void
mgl_assign(lofig_list *ptfig, char *lident, char *rident, struct dct_entry **hshtab, mgl_scompcontext *context)
{
    losig_list *ptlsig;
    losig_list *ptrsig;
    losig_list *pttempsig;
    ptype_list *ptluser;
    ptype_list *ptruser;
    chain_list *ptchain;
    
    ptlsig = (losig_list *)mgl_getlosig(ptfig, lident, hshtab, context);
    ptrsig = (losig_list *)mgl_getlosig(ptfig, rident, hshtab, context);
    /* quietly give-up if signal does not exist */
    if (ptlsig == NULL || ptrsig == NULL) return;

    if (ptrsig->TYPE == EXTERNAL && ptlsig->TYPE == EXTERNAL) {
        fprintf(stderr, "Warning : external signals '%s' and '%s' have been merged\n", lident, rident);
        ptrsig->NAMECHAIN = append(ptrsig->NAMECHAIN, ptlsig->NAMECHAIN);
        ptlsig->NAMECHAIN = NULL; 
    }
    else {
        /* swap if rhs is external */
        if (ptrsig->TYPE == EXTERNAL) {
            pttempsig = ptrsig;
            ptrsig = ptlsig;
            ptlsig = pttempsig;
        }
    
        /* change rsig to external if lsig external */
        if (ptlsig->TYPE == EXTERNAL) ptrsig->TYPE = EXTERNAL;

        /* merge names unless external */
        if (ptrsig->TYPE == EXTERNAL) {
            freechain(ptrsig->NAMECHAIN);
            ptrsig->NAMECHAIN = ptlsig->NAMECHAIN;
        }
        else {
            ptrsig->NAMECHAIN = append(ptrsig->NAMECHAIN, ptlsig->NAMECHAIN);
        }
        ptlsig->NAMECHAIN = NULL; 
    }
    
    /* merge connectors */
    ptluser = getptype(ptlsig->USER, LOFIGCHAIN);
    if (ptluser != NULL) {
        for (ptchain = (chain_list *)ptluser->DATA; ptchain; ptchain = ptchain->NEXT) {
            ((locon_list *)ptchain->DATA)->SIG = ptrsig;
        }
        ptruser = getptype(ptrsig->USER, LOFIGCHAIN);
        ptruser->DATA = append((chain_list *)ptruser->DATA, (chain_list *)ptluser->DATA);
        ptlsig->USER = delptype(ptlsig->USER, LOFIGCHAIN);
    }
    
    ptlsig->INDEX = 0;
}

/* ###--------------------------------------------------------------### */
/* function     : mgl_sortsig                                           */
/* ###--------------------------------------------------------------### */

/* Reorder signals to match model. First try order or reverse order */
/* matching. If that fails then we apply the popular n^2 technique  */

chain_list *
mgl_sortsig(lofig_list *ptmodel, char *insname, chain_list *loconnames, chain_list *sigchain, lofig_list *ptfig, int *ptindex)
{
    locon_list *ptmodelcon;
    losig_list *ptnewsig;
    chain_list *ptchain, *ptchain1;
    chain_list *reschain = NULL;
    char        buffer[1024];
    int         i=0, j=0, reversed=0;

    for (ptchain = sigchain; ptchain != NULL; ptchain = ptchain->NEXT) i++;
    for (ptchain = loconnames; ptchain != NULL; ptchain = ptchain->NEXT) {
        j++;
    }
    if (i != j) {
        fflush(stdout);
        fputs("*** mbk error ***\n", stderr);
        fputs("mgl_sortsig : inconsistency between connectors", stderr);
        fprintf(stderr, " and signals in instance '%s'\n", insname);
        EXIT(1);
    }
    i = 0;
    j = 0;

    if (sigchain != NULL) {
        if (ptmodel->LOCON != NULL && ptmodel->LOCON->NAME != (char *)loconnames->DATA) {
            loconnames = reverse(loconnames);
            sigchain = reverse(sigchain);
            reversed=1;
        }
    }
        
    for (ptmodelcon = ptmodel->LOCON, ptchain = loconnames; 
         ptmodelcon != NULL && ptchain != NULL;
         ptmodelcon = ptmodelcon->NEXT, ptchain = ptchain->NEXT) {
        if (ptmodelcon->NAME != (char *)ptchain->DATA) break;
    }

    /* order matching fails so try name matching */
    if (ptmodelcon != NULL || ptchain != NULL) {
        for (ptmodelcon = ptmodel->LOCON; ptmodelcon != NULL; ptmodelcon = ptmodelcon->NEXT) i++;
        for (ptchain = loconnames; ptchain != NULL; ptchain = ptchain->NEXT) j++;
        if (i < j) {
            fflush(stdout);
            fputs("*** mbk error ***\n", stderr);
            fputs("mgl_sortsig : connector number inconsistency between model", stderr);
            fprintf(stderr, " '%s' and instance '%s'\n", ptmodel->NAME, insname);
            EXIT(1);
        }
        for (ptmodelcon = ptmodel->LOCON; ptmodelcon != NULL; ptmodelcon = ptmodelcon->NEXT) {
            for (ptchain = loconnames, ptchain1 = sigchain; ptchain; ptchain = ptchain->NEXT, ptchain1 = ptchain1->NEXT) {
                if ((char *)ptchain->DATA == ptmodelcon->NAME) {
                    reschain = addchain(reschain, ptchain1->DATA);
                    break;
                } 
            }
            if (ptchain == NULL) {
                sprintf(buffer, "%s_%s", insname, ptmodelcon->NAME);
                ptnewsig = addlosig(ptfig, *ptindex, addchain(NULL, namealloc(buffer)), 'I');
                (*ptindex)++;
                reschain = addchain(reschain, ptnewsig);
            }
        }
        freechain(sigchain);
        reschain = reverse(reschain);
    }
    else reschain = sigchain;

    if (reversed)
     {
       loconnames = reverse(loconnames);
     }
    return reschain;
}

/* ###--------------------------------------------------------------### */
/* function     : mgl_getloconrange                                     */
/* ###--------------------------------------------------------------### */

void
mgl_getloconrange(locon_list *ptheadlocon, char *name, int *left, int *right)
{
    locon_list *ptlocon;
    int         index, match;

    *left = -1;
    *right = -1;
    for (ptlocon = ptheadlocon; ptlocon; ptlocon = ptlocon->NEXT) {
        if (strcmp(vectorradical(ptlocon->NAME), name) == 0) match = 1;
        else match = 0;
        if (match) {
            index = vectorindex(ptlocon->NAME);
            if (*right == -1) *right = index;
            *left = index;
        }
    }
}

/* ###--------------------------------------------------------------### */
/* function     : mgl_givevdd                                           */
/* ###--------------------------------------------------------------### */

/* Identify a VDD signal on module interface or create a VDD local  */
/* to the instance                                                  */

losig_list *
mgl_givevdd(lofig_list *ptfig, char *insname, int *ptindex)
{
    locon_list *ptlocon;
    losig_list *ptnewsig;
    char        buffer[1024];

    for (ptlocon = ptfig->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
        if (mbk_LosigIsVDD(ptlocon->SIG)) return ptlocon->SIG;
    }
    
    sprintf(buffer, "%s_vdd", insname);
    ptnewsig = addlosig(ptfig, *ptindex, addchain(NULL, namealloc(buffer)), 'I');
    mbk_SetLosigVDD(ptnewsig);
    (*ptindex)++;

    return ptnewsig;
}

/* ###--------------------------------------------------------------### */
/* function     : mgl_givevss                                           */
/* ###--------------------------------------------------------------### */

/* Identify a VSS signal on module interface or create a VDD local  */
/* to the instance                                                  */

losig_list *
mgl_givevss(lofig_list *ptfig, char *insname, int *ptindex)
{
    locon_list *ptlocon;
    losig_list *ptnewsig;
    char        buffer[1024];

    for (ptlocon = ptfig->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
        if (mbk_LosigIsVSS(ptlocon->SIG)) return ptlocon->SIG;
    }
    
    sprintf(buffer, "%s_vss", insname);
    ptnewsig = addlosig(ptfig, *ptindex, addchain(NULL, namealloc(buffer)), 'I');
    mbk_SetLosigVSS(ptnewsig);
    (*ptindex)++;

    return ptnewsig;
}


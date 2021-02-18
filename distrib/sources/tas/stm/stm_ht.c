/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_ht.c                                                    */
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

#include "stm.h"

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

/* Random hash function due to Don. E. Knuth, The Stanford Graph Base.
 * Truly better than the previous one from my own experimentations. */
#define STM_HASH_MULT  314159
#define STM_HASH_PRIME 516595003
/* namealloc hash table size: the 1230th prime */
#define STM_HASH_VAL   100007

static int HASH_FUNC (char *inputname)
{
    unsigned char c;

    int code = 0;
    while (*inputname) {
        c = *inputname++;
        code += (code ^ (code >> 1)) + STM_HASH_MULT * c;
        while (code >= STM_HASH_PRIME)
            code -= STM_HASH_PRIME;
    }
    code %= STM_HASH_VAL;

    return code;
}

/****************************************************************************/
/* function addht, create a hash table                                      */
/****************************************************************************/

stm_ht *stm_addht (unsigned long len)
{
    stm_ht *pTable;
    stm_htitem *pEl;
    unsigned int i;

    if (len == 0) {
        avt_errmsg (STM_ERRMSG,"008", AVT_ERROR);
        return NULL;
    }

    pTable = (stm_ht*)mbkalloc (sizeof (struct stm_htable));
    pTable->length = len;
    pEl = (stm_htitem*)mbkalloc (len * (sizeof (struct stm_htitem)));
    pTable->pElem = pEl;
    for (i = 0; i < len; i++) {
        pEl[i].key = NULL;
        pEl[i].value = STM_EMPTYHT;
    }
    pTable->count = 0;
    return pTable;
}

/****************************************************************************/
/* function delht, delete a hash table                                      */
/****************************************************************************/

void stm_delht (stm_ht *pTable)
{
    stm_htitem *pEl;

    pEl = pTable->pElem;
    mbkfree (pEl);
    mbkfree (pTable);
}

/****************************************************************************/
/* function gethtitem, get an element in a hash table                       */
/****************************************************************************/

long stm_gethtitem (stm_ht *pTable, char *key)
{
    long co = 0;
    long indice = 0;
    stm_htitem * pEl;

    indice = HASH_FUNC (key) % pTable->length;
    do {
        if (co++ > STM_HMAX_CALLS) {
            if ((pTable->count > (pTable->length) * 2 / 10) || (co >= pTable->length)) {
                stm_reallocht (pTable);
                return stm_gethtitem (pTable, key);
            }
        }

        pEl = (pTable->pElem) + indice;
        if (pEl->value != STM_EMPTYHT && pEl->value != STM_DELETEHT) {
            if (!strcmp (key, pEl->key))
                return pEl->value;
        } else if (pEl->value == STM_EMPTYHT)
            return STM_EMPTYHT;
        indice = (indice + 1) % pTable->length;
    } while (1);
}

/****************************************************************************/
/* function addhtitem, get an element in a hash table                       */
/****************************************************************************/

long stm_addhtitem (stm_ht *pTable, char *key, long value)
{
    int indice = 0;
    stm_htitem *pEl;
    int co = 0;

    if (value == STM_EMPTYHT || value == STM_DELETEHT) {
        avt_errmsg (STM_ERRMSG,"009", AVT_ERROR);
        return 0;
    }

    if (pTable->count++ > (pTable->length) * 8 / 10) {
        stm_reallocht (pTable);
        return stm_addhtitem (pTable, key, value);
    }

    indice = HASH_FUNC (key) % pTable->length;
    do {
        if (co++ > STM_HMAX_CALLS) { 
		    if (pTable->count > (pTable->length) * 2 / 10 || (co >= pTable->length)) {
                stm_reallocht (pTable);
                return stm_addhtitem (pTable, key, value);
            } 
	    }
        pEl = (pTable->pElem) + indice;
        if (pEl->value == STM_EMPTYHT || pEl->value == STM_DELETEHT) {
            pEl->value = value;
            pEl->key = strdup (key);
            return value;
        } else if (!strcmp (pEl->key, key)) {
            pTable->count--;
            pEl->value = value;
            return value;
        }
        indice = (indice + 1) % pTable->length;
    } while (1);
}

/****************************************************************************/
/* function delhtitem, delete an element in a hash table                    */
/****************************************************************************/

long stm_delhtitem (stm_ht *pTable, char *key)
{
    int indice = 0;
    stm_htitem *pEl;
    int co = 0;

    indice = HASH_FUNC (key) % pTable->length;
    do {
        if (co++ > STM_HMAX_CALLS) {
  	      if (pTable->count > (pTable->length) * 2 / 10 || (co >= pTable->length)) {
            stm_reallocht (pTable);
            return stm_delhtitem (pTable, key);
          }
        }

        pEl = (pTable->pElem) + indice;
        if (pEl->value != STM_EMPTYHT && pEl->value != STM_DELETEHT) {
            if (!strcmp (key, pEl->key)) {
                pTable->count--;
                pEl->value = STM_DELETEHT;
                mbkfree (pEl->key);
                return pEl->value;
            }
        } else if (pEl->value == STM_EMPTYHT)
            return STM_EMPTYHT;
        indice = (indice + 1) % pTable->length;
    } while (1);
}

/****************************************************************************/
/* realloc space to adapt hash table size to number of entries              */
/****************************************************************************/

void stm_reallocht (stm_ht *pTable)
{
    stm_ht *tabBis;
    stm_htitem *pEl;
    int i;
    double ratio;

    pEl = pTable->pElem;

    ratio = (double)pTable->count / (double)pTable->length;
    if (ratio > 0.8) ratio = 1;
    else if (ratio < 0.3) ratio = 0.3;
    
    tabBis = stm_addht ((unsigned long)((double)(pTable->length) * 5.0 * ratio));
    for (i = 0; i < pTable->length; i++) {
        if (pEl->value != STM_EMPTYHT && pEl->value != STM_DELETEHT)
            stm_addhtitem (tabBis, pEl->key, pEl->value);
        pEl++;
    }
    mbkfree (pTable->pElem);
    pTable->length = tabBis->length;
    pTable->pElem = tabBis->pElem;
    pTable->count = tabBis->count;
    mbkfree (tabBis);
}


/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Fichier : stm_modfct_cache.h                                          */
/*                                                                          */
/*    © copyright 2003 AVERTEC                                              */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) :   Antony PINTO                                            */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MODFCT_SHARE_H
#define STM_MODFCT_SHARE_H

#include STM_H

stm_share           *stm_modfct_getShared(timing_function *fct);
int                  stm_modfct_freeShare(stm_share *share);
int                  stm_modfct_addShareCleanning(long type, void(*func)(void *));
extern ptype_list   *stm_modfct_getSharedType(timing_function *fct, long type);
extern ptype_list   *stm_modfct_addSharedType(timing_function *fct, long type, void *data, void (*func)(void *));
int                  stm_modfct_unShare(stm_share *share);
stm_share           *stm_modfct_newShare(void);
void                 stm_modfct_share(timing_function *fct, timing_function *orig);

#endif

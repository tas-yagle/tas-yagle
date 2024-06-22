/******************************************************************************\
*                                                                              *
*                           Alliance VLSI CAD system                           *
*    +---+                                                                     *
*   / \ / \           Laboratoire LIP6 - Département ASIM                      *
*  /   /   \        Universite Paris 6 - Pierre et Marie Curie                 *
* +---+ \   \                                                                  *
*  \   \ +---+                 Address : LIP6                                  *
*   \   /   /                            4, place Jussieu                      *
*    \ / \ /                             75252 Paris Cedex 05                  *
*     +---+                              France                                *
*                                  Web : www-asim.lip6.fr                      *
*                              Support : alliance-support@lip6.fr              *
*                                                                              *
********************************************************************************
*                                                                              *
*  Tool        : TLC v 1.00                                                    *
*  Author(s)   : AVOT Gregoire                                                 *
*  Last update : 20 Mai 1998                                                   *
*                                                                              *
*******************************************************************************/

#ifndef TLC
#define TLC

typedef struct sloconparam
{
  struct sloconparam	*NEXT;
  float			 CAPA;  /* en pif */
} tlc_loconparam;

#define TLC_LOCONPRM ((long)1666)
#define TLC_ALIM     ((long)19980520)

extern float TLC_COEF_CTC;
extern float TLC_MAXSCALE_CAPAFACTOR;
extern float TLC_MINSCALE_CAPAFACTOR;
extern float TLC_MAXSCALE_RESIFACTOR;
extern float TLC_MINSCALE_RESIFACTOR;

/* Informations on locon */

inline void     tlc_setmaxfactor        __P(( void )) ;
inline void     tlc_setminfactor        __P(( void )) ;
inline float    tlc_getcapafactor       __P(( void )) ;
inline float    tlc_getresifactor       __P(( void )) ;
extern void     tlcenv                  __P(( void )) ;
extern void     tlc_mergeloconparam     __P(( locon_list*, locon_list* ));
extern void            tlc_setloconparam       __P(( locon_list*, float ));
extern tlc_loconparam* tlc_getloconparam       __P(( locon_list* ));
extern void            tlc_delloconparam       __P(( locon_list* ));
extern void            tlc_replaceloconparam   __P(( locon_list*, float ));

/* Informations on losig */

extern void            tlc_setlosigalim        __P(( losig_list* ));
extern void            tlc_resetlosigalim      __P(( losig_list* ));
extern int             tlc_islosigalim         __P(( losig_list* ));

/* Verifications pour le developpement uniquement */

#define TLC_CHECK

#endif

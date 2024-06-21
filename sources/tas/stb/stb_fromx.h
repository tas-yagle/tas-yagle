/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : AVERTEC global tools                                        */
/*    Fichier : stb_fromx.h                                                 */
/*                                                                          */
/*    © copyright 2000 AVERTEC                                              */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Antony PINTO                                              */
/*                                                                          */
/****************************************************************************/

#ifndef STB_FROMX_H
#define STB_FROMX_H

typedef struct stb_fromX_struct
{
  stbfig_list   *STBFIG;
  inffig_list   *INFFIG;
  double         UNIT;
} stb_fromX_t;

int
stb_fromX                       ( stb_fromX_t       *fxt,
                                  stb_parse         *stab);

int stb_fromX_STEP1_traditional(stb_fromX_t *fxt, stb_parse *stab);
int stb_fromX_STEP2_stability(stb_fromX_t *fxt, stb_parse *stab);
int stb_fromX_STEP3_finish(stb_fromX_t *fxt, stb_parse *stab);

#endif

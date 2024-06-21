/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : AVERTEC global tools                                        */
/*    Fichier : inf_stb.h                                                   */
/*                                                                          */
/*    © copyright 2004 AVERTEC                                              */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Antony PINTO                                              */
/*                                                                          */
/****************************************************************************/

#ifndef INF_STB_H
#define INF_STB_H

#define INF_STB_NOTHING          (char) -1

#define INF_STB_BEFORE           (char) 1
#define INF_STB_AFTER            (char) 2

#define INF_STB_RISING           (char) 1
#define INF_STB_FALLING          (char) 2
#define INF_STB_SLOPEALL         (char) 3

#define INF_STB_SPECIN           (char) 1
#define INF_STB_SPECOUT          (char) 2
#define INF_STB_STBOUT_SPECIN    (char) 3
#define INF_STB_STBOUT_SPECOUT   (char) 4
#define INF_STB_STBOUT_SPECMEM   (char) 5
#define INF_STB_STBOUT_SPECINODE (char) 6

#define INF_STB_STABLE           (char) 1
#define INF_STB_UNSTABLE         (char) 2

#define INF_STB_HZ_NO_PRECH      (char) 1
#define INF_STB_HZ_NO_EVAL       (char) 2

typedef struct inf_stb_parse_spec_stability {
  struct inf_stb_parse_spec_stability   *NEXT;
  chain_list                            *TIME_LIST;
  char                                  *CKNAME;
  char                                   CKEDGE;
  char                                   RELATIVITY;
  char                                   STABILITY;
  double                                 DELAY;
} inf_stb_p_s_stab;

typedef struct inf_stb_parse_spec_struct {
  struct inf_stb_parse_spec_struct      *NEXT;
  inf_stb_p_s_stab                      *STABILITY;
  char                                  *CKNAME;
  char                                   CKEDGE;
  char                                   SPECTYPE;
  char                                   DATAEDGE;
  char                                   HZOPTION;
} inf_stb_p_s;

/* ------------------- parse struct ----------------------- */
inf_stb_p_s *
inf_stb_parse_spec_alloc                ( inf_stb_p_s       *next);
inf_stb_p_s_stab *
inf_stb_parse_spec_stab_alloc           ( inf_stb_p_s_stab  *next);
void
inf_stb_parse_spec_free                 ( inf_stb_p_s       *pt);
void
inf_stb_parse_spec_stab_free            ( inf_stb_p_s_stab  *pt);
inf_stb_p_s *
inf_stb_parse_spec_del                  ( inf_stb_p_s       *pt);
inf_stb_p_s_stab *
inf_stb_parse_spec_stab_del             ( inf_stb_p_s_stab  *pt);
inf_stb_p_s *inf_stb_spec_exists(inf_stb_p_s *pt, inf_stb_p_s *list);

#endif

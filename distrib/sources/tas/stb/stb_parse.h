/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STB Version 1.00                                            */
/*    Fichier : stb_parse.h                                                 */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                Anthony LESTER                                            */
/*    03082004  : Antony PINTO                                              */
/*                several functions moved from stb.yac                      */
/*                                                                          */
/****************************************************************************/

#ifndef STB_PARSE_H
#define STB_PARSE_H

/*****************************************************************************
* constants                                                                  *
*****************************************************************************/

#define STBYY_AFTER  0
#define STBYY_BEFORE 1

typedef struct stb_parse_struct {
  stbfig_list   *PARSEDFIG;
  chain_list    *PTINSCHAIN;
  chain_list    *PTSTABLELIST;
  chain_list    *PTUNSTABLELIST;
  chain_list    *CKDOMAINS;
  chain_list    *CKEQVT;
  char           DEFAULTPHASE;
  char           DOMAININDEX;
  char           EQVTINDEX;
  int            INVERTED;
  int            IDEAL;
  int            VIRTUAL;
  void          (*CLEANFUNC)(void);
} stb_parse;

typedef struct stb_parse_pulse_struct {
  long supmin;
  long supmax;
  long sdnmin;
  long sdnmax;
  long period;
  char *master_clock;
  int master_edges;
} stb_parse_pulse;

typedef struct stb_parse_doublet_struct {
  long start;
  long end;
  long signe ;
} stb_parse_doublet;              

/* ------------------- parse clock ----------------------- */
void
stb_parse_cklocon       ( stb_parse         *stab,
                          char              *ident,
                          stb_parse_pulse   *pulse, inffig_list *ifl);
void
stb_parse_ckdeclar      ( void *fxt0, stb_parse         *stab);
void
stb_parse_domain_groups ( stb_parse         *stab,
                          chain_list        *group);
stbpriority *
stb_parse_ckprio_pair   ( stb_parse         *stab,
                          char              *id1,
                          char              *id3);
chain_list *
stb_parse_ckname_list   ( stb_parse         *stab,
                          chain_list        *cklist,
                          char              *ident);
chain_list *
stb_parse_group         ( chain_list        *cklist,
                          long               period);
void
stb_parse_eqvt_groups   ( stb_parse         *stab,
                          chain_list        *group);
void
stb_parse_eqvt_clocks   ( stb_parse         *stab);

/* ------------------- parse command ----------------------- */
void
stb_parse_command       ( stb_parse         *stab,
                          char              *ident,
                          char               state);
void
stb_parse_comdefault    ( stb_parse         *stab,
                          char               state);

/* ------------------- parse node ----------------------- */
void
stb_parse_mem           ( stb_parse         *stab,
                          char              *ident,
                          char               edge_option,
                          int                source_phase,
                          char               hz_option);
void
stb_parse_node          ( stb_parse         *stab,
                          char              *ident,
                          char               edge_option,
                          int                source_phase,
                          char               hz_option);

void
stb_parse_outnode       ( stb_parse         *stab,
                          char              *ident,
                          char               edge_option,
                          int                source_phase,
                          char               hz_option);

void
stb_parse_innode        ( stb_parse         *stab,
                          char              *ident,
                          char               edge_option,
                          int                source_phase,
                          char               hz_option);
void
stb_parse_freeStab      ( stb_parse         *stab);

/* ------------------- parse pin ----------------------- */
void
stb_parse_inlocon       ( stb_parse         *stab,
                          char              *ident,
                          char               edge_option,
                          int                source_phase);
void
stb_parse_outlocon      ( stb_parse         *stab,
                          char              *ident,
                          char               edge_option,
                          int                source_phase);
void
stb_parse_indefault     ( stb_parse         *stab,
                          char               edge_option,
                          int                source_phase);
void
stb_parse_outdefault    ( stb_parse         *stab,
                          char               edge_option,
                          int                source_phase);

/* ------------------- parse struct ----------------------- */
void
stb_parse_init          ( stb_parse         *stab,
                          void              (*cleanfunc)(void));
void
stb_parse_stb_file      ( stb_parse         *stab);
void
stb_parse_name          ( stb_parse         *stab,
                          char              *ident,
                          int                done);
void
stb_parse_relative_phase( stb_parse         *stab,
                          stb_parse_doublet *relative_phase,
                          char               relative_type,
                          char              *ident,
                          int                edge_option);
int
stb_parse_phase         ( stb_parse         *stab,
                          char              *ident,
                          int                edge_option);
void
stb_parse_stability     ( stb_parse         *stab,
                          char               stability_type,
                          long               integer,
                          stb_parse_doublet *relative_phase,
                          chain_list        *inschain);
void
stb_parse_wenable       ( stb_parse         *stab);

int stb_loadstbfig __P((stbfig_list *ptstbfig));
stb_parse *stb_convert_inf_to_stb_1 __P((stbfig_list *ptstbfig));
int stb_convert_inf_to_stb_2 __P((stbfig_list *ptstbfig, stb_parse *stab));
chain_list* getStartForNode_HT __P((stbfig_list *parsedfig, char *ident, int type));
void cleanStartForNode_HT();
void stb_parse_memory(stb_parse *stab, char *ident, int state);
chain_list *stb_auto_create_equivalent_groups(stb_parse *stab);
chain_list *stb_merge_equiv_if_needed(chain_list *equiv, chain_list *equiv_auto);

#endif

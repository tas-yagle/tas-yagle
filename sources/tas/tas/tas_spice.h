/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TAS Version 5                                               */
/*    Fichier : tas_spice.h                                                 */
/*                                                                          */
/*    (c) copyright 1991-2002 Avertec                                       */
/*    Tous droits reserves                                                  */
/*    Support : contact@avertec.com                                         */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                Marc  KUOCH                                               */
/*                                                                          */
/****************************************************************************/

/* define */
#define TAS_CALC_RC_ON_SPISIG 0x00000007
#define TAS_SIG_LOINSNAME     0x00000051
#define TAS_OLD_INPUT_START   0x00000FFF

/* structure */
typedef struct tas_old_input {
  char    *NAME;
  double   TSTART;
  double   CRITICDELAY;
} tas_old_input;

/* declaration de fonction */
extern void        tas_simu_set_progression ( void (*callfn)( void*, char*, int ) );
extern void        tas_free_spisig_marks    ( spisig_list *head);
extern int         tas_get_rc_delay         ( spisig_list *spisig);
extern void        tas_flag_rc_delay2spisig ( spisig_list *head_spisig,
                                              ttvcritic_list *head_critic);
extern sim_model  *tas_get_sim_mod          (lofig_list *figext,
                                             ttvcritic_list *critic,
                                             loins_list **origloins,
                                             chain_list **gen_loins,
                                             chain_list **gen_lotrs,
                                             ptype_list **gen_losig);
extern double      tas_get_input_slope      ( ttvcritic_list* );

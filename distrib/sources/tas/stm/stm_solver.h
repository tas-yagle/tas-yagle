typedef struct {
  float I_EPSILON;      // Ecart maximum autours de i (A)
  float V_EPSILON;      // Ecart maximum autours de v (V)
  int   MAXITER;        // Nombre maximum d'itérations
  int   DICHOTOMIE;     // Normalement, on calcul le i proportionnellement à la
                        // tension max et à la tension min trouvée. Cette 
                        // convergence étant lente vers la fin, il est plus
                        // rapide de prendre le millieu dès que le nb d'itér
                        // est inférieur à cette valeur
  float MINDELTAVOLTAGE;// Valeur de tension de branche en dessous de laquelle
                        // on ne simule rien (V)
  float MINCURRENT;     // Courant estimé dans la branche en dessous du quel
                        // on ne simule rien.
} stm_solver_i_param;

#define STM_SOLVER_INEG '-'
#define STM_SOLVER_IPOS '+'

/******************************************************************************\
Liste des fonctions de calcul.
\******************************************************************************/
extern char stm_solver_calc_vds( stm_solver_maillon *trans, float vs, float ids, float *vds );
extern char stm_solver_calc_ids( stm_solver_maillon *trans, float vs, float vd, float *ids );
char stm_solver_estim_imax( stm_solver_maillon_list *branch, float vmax, float *imax );
extern char stm_solver_i( stm_solver_maillon_list *head, float vbr, float *imax );
int stm_solver_pi_load_ts( stm_solver_maillon_list *head, stm_solver_maillon *commute, float (*fn_vin)( void *dat_vin, float t ), float *dat_vin, float r, float c1, float c2, float vi, float vth, float *ts, float *fs, stm_solver_parameter *param );


/******************************************************************************\
Liste des fonctions de gestion des données.
\******************************************************************************/
extern stm_solver_maillon* stm_solver_new_maillon( void );
extern void stm_solver_free_maillon( stm_solver_maillon *maillon );
extern void stm_solver_add_model( stm_solver_maillon *maillon, char (*fids)( void*, float, float, float*), char (*fvds)( void*, float, float, float*), chain_list *datids, chain_list *datvds );
extern stm_solver_maillon_list * stm_solver_maillon_addchain( stm_solver_maillon_list *head, stm_solver_maillon *elem);
extern void stm_solver_maillon_freechain( stm_solver_maillon_list *head );
extern stm_solver_maillon_list* stm_solver_maillon_reverse( stm_solver_maillon_list *head );
extern void stm_drive_ids( stm_solver_maillon *maillon, float vgs, float vdsmax, float vdsstep, char *filename);


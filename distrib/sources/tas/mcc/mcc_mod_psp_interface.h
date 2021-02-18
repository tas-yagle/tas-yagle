
#define PSP_UNDEF 314159265

#define PSP_OP_PARAM_IDS  0
#define PSP_OP_PARAM_VTH  1
#define PSP_OP_PARAM_WEFF 2
#define PSP_OP_PARAM_NB   3

#define PSP_FIND_MPARAM 0x01
#define PSP_FIND_IPARAM 0x02

/* USER field of mcc_modellist* */
#define PSPHASHINSTANCEPARAM 0x00001
/* index des parametres d'instance. global à un modele                  */
#define PSPHASHMODELPARAM    0x00002
/* index des parametres de modele. global à un modele                   */
#define PSPCACHECHARGE       0x00003
/* charges. depend de chaque instance                                   */
#define PSPCACHEMODEL        0x00004
/* tableau des couples idx-valeur des parametres de modele. global à un 
   modèle                                                               */
#define PSPCACHEINSTANCE     0x00005
/* tableau des couples idx-valeur des parametres d'instance. 
   depend de chaque instance                                            */
#define PSPCACHETRS          0x00006
/* cache du model psp integrale. depend de chaque instance              */

typedef struct {
  void              *mdata ;
  void              *idata ;
  SK_INST_VARIABLES *variables ;
} cachemodel ;

typedef struct {
  SK_MODEL          *model ;
  mcc_modellist     *mccmodel ;
  void              *mdata ;
  void              *idata ;
  SK_INST_VARIABLES *variables ; /* point somewhere in the idata bloc memory */
  SK_SIM_DATA        simdata ;
  sk_unint           tabid[PSP_OP_PARAM_NB] ;
  char               cleanmidata ;
} trs ;

typedef struct {
  sk_unint         *index ;
  sk_real          *value ;
  sk_unint          n ;
  sk_unint          max ;
} pspmodelparam ;

typedef struct {
  int               n ;
  char            **param ;
  sk_real          *value ;
} psptunedparam ;

int initialize( trs             *ptr, 
                mcc_modellist   *mccmodel, 
                elp_lotrs_param *lotrsparam,
                double           L,
                double           W,
                double           temp,
                psptunedparam   *tuned
              );
void terminate( trs *ptr );
sk_unint get_id_param( trs *ptr, int param );
void set_polarization( trs *ptr, double vgs, double vds, double vbs );
int psp_loaddynamiclibrary( void );
void mcc_clean_psp_interface( mcc_modellist *mccmodel, int check );

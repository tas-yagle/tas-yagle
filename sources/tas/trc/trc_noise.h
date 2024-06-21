typedef struct {
  rcxparam      *PARAM;
  RCXFLOAT       CGLOBALE;
  RCXFLOAT       CLOCALE;
} agrnoise;

typedef struct {
  rcxparam      *PARAM;
  RCXFLOAT       CGLOBALE;
  RCXFLOAT       CLOCALE;
  double         TA;
  double         CC;
  double         CCM;
  double         DT;
} agrnoise2;

typedef struct {
  agrnoise2     *tabagr;
  int            nbagr;
  double         tamax;
  double         vdd;
  double         rv;
  double         fv;
  double         tv;
  double         ct;
} agrnoise_data ;

extern RCXFLOAT rcx_noisedelay_scr( lofig_list*, losig_list*, RCXFLOAT, rcx_slope*, RCXFLOAT, RCXFLOAT, char );
RCXFLOAT rcx_noise_basic_scr( RCXFLOAT, RCXFLOAT, RCXFLOAT, RCXFLOAT, RCXFLOAT );
extern rcx_signal_noise* rcx_noise_scr_detail( lofig_list*, losig_list *victime, RCXFLOAT rv, RCXFLOAT vdd, int *nbagr );
RCXFLOAT rcx_noise_scr_1( lofig_list *lofig, losig_list *victime, rcx_slope *fvmiller, RCXFLOAT rv, RCXFLOAT vdd, char type );


RCXFLOAT rcx_noise_scr_2( lofig_list *lofig, losig_list *victime, rcx_slope *fvmiller, RCXFLOAT fv, RCXFLOAT rv, RCXFLOAT vdd, char type );
int rcx_noise_fn( agrnoise_data *infos, double bt, double *f );
double rcx_noise_allagr( agrnoise_data *infos, double bt );
void rcx_noise_complete_tabagr( agrnoise2 *tabagr, int nbagr, double tamax, double ct );
void rcx_noise_get_tamax( agrnoise2 *tabagr, int nbagr, int *idmax, double *tamax );
void rcx_noise_calc_ta( agrnoise2 *tabagr, int nbagr, rcx_slope *fvmiller, RCXFLOAT fv, char type, losig_list *victime );
void rcx_noise_trace_noise( agrnoise_data *infos, double tmax );

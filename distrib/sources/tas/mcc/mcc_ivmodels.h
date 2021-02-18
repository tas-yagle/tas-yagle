
typedef struct {
  chain_list *HEADTRS ;
  double      VSR ;
  double      VDRMIN ;
  double      VDRMAX ;
  double      IMIN ;
  double      IMAX ;
  double      IDS ;
} fn_sum_info ;

int fn_sum_current( fn_sum_info*, double, double* );

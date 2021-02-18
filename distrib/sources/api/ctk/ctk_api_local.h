
typedef struct {
  ttvsig_list     *TTVSIG ;
  char            *NETNAME ;
  char             ACT_WORST;
  char             ACT_BEST;
  char             ACT_MUTEX_WORST;
  char             ACT_MUTEX_BEST;
  char             NOISE_RISE_PEAK ;
  char             NOISE_RISE_EXCLUDED ;
  char             NOISE_FALL_PEAK ;
  char             NOISE_FALL_EXCLUDED ;
  float            CC;

} ctk_aggressor ; 

typedef struct {
  char *TOKEN ;
  char  VALUE ;
} ctk_corresp ;

extern lofig_list *CTK_NETLIST;
extern ctk_struct *CTK_STRUCT;

void ctk_default_action_0c2c();
void ctk_default_action_miller();

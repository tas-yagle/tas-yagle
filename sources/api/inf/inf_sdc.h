#define INFSDC_MIN              0x10    
#define INFSDC_MAX              0x20
#define INFSDC_MIN_AND_MAX      0x30    

#define INFSDC_RISE             0x04
#define INFSDC_FALL             0x08
#define INFSDC_RISE_AND_FALL    0x0c 

#define INFSDC_SOURCE           0x80


#define INFSDC_ADD              0x40

#define INFSDC_INPUT            0x01
#define INFSDC_OUTPUT           0x02
#define INFSDC_CK_FALL          0x400

#define INFSDC_SETUP            0x100
#define INFSDC_HOLD             0x200

/* Structures */
typedef struct  infsdc_struct_edges {
    struct infsdc_struct_edges *NEXT;
    int EDGE;
    int SHIFT;
}infsdc_edges;


extern  double      INFSDC_TIME_MUL;
extern  int         SDC2STB_ERRORS;

extern  chain_list  *infsdc_listToCharChainlistSimple (char *list,int MODE);
extern  char        *infsdc_get_clocks                (char *patterns);
extern  void         infsdc_create_clock              (char **argv);
extern  void         infsdc_set_clock_latency         (char **argv);
extern  void         infsdc_create_generated_clock    (char **argv);
extern  void         infsdc_set_input_delay           (char **argv); 
extern  void         infsdc_set_input_transition      (char **argv); 
extern  void         infsdc_set_load                  (char **argv);
extern  void         infsdc_set_output_delay          (char **argv); 
extern  void         infsdc_addClockSpec              (char *name, chain_list *con, double up, double dn, double period, int flags, char *master, int redge, int fedge);
extern  void         infsdc_addClockDomain            (char *name, double period);
extern  void         infsdc_addClockLatency           (chain_list *obj_l, int mode, double delay);
extern  void         infsdc_addDelay                  (char *name, char *ckname, int mode, double delay);
extern  void         infsdc_set_case_analysis         (char **argv);
extern  void         infsdc_set_false_path            (char **argv);
extern  void         infsdc_set_disable_timing        (char **argv);
extern  void         infsdc_set_multicycle_path       (char **argv);
void infsdc_set_clock_uncertainty (char **argv);
void infsdc_set_clock_groups (char **argv);
void  infsdc_not_supported               (char  *command);
void infsdc_set_max_delay (char **argv);
void infsdc_set_min_delay (char **argv);

extern  char        *sdc2stb_init                     (int argc, char **argv);
extern  char        *sdc2stb_getfilename              (void);
extern  void         sdc2stb_updateLineno             (int lineno);


extern double DISPLAY_time_unit, DISPLAY_capa_unit;
extern char *DISPLAY_time_string, *DISPLAY_capa_string, *DISPLAY_time_format, *DISPLAY_signed_time_format, *DISPLAY_capa_format;
extern char DISPLAY_number_justify;
extern int DISPLAY_nodemode;
extern char probemode;
extern char DISPLAY_simdiffmode;

typedef struct {
  int enabled;
  struct {
    double val;
    char *label;
    int noacc;
  } add[10];
} PATH_MORE_INFO_TYPE;

extern PATH_MORE_INFO_TYPE PATH_MORE_INFO;

#define PATH_NOISE_PTYPE 0xfab60314

typedef struct
{
  char T, N, A, C, I, P;
} PATH_NOISE_TYPE;
// columns for path list

#define COL_INDEX      0    //<- shared
#define COL_PATH_SEP0  1
#define COL_STARTTIME  2
#define COL_PATH_SEP1  3
#define COL_STARTSLOPE 4
#define COL_PATH_SEP2  5
#define COL_PATHDELAY  6
#define COL_PATH_SEP3  7
#define COL_TOTALDELAY 8
#define COL_PATH_SEP4  9
#define COL_DATALAG    10
#define COL_PATH_SEP5  11
#define COL_ENDSLOPE   12
#define COL_PATH_SEP6  13
#define COL_STARTNODE_DIR 14
#define COL_STARTNODE  15 
#define COL_PATH_SEP7  16
#define COL_ENDNODE_DIR 17
#define COL_ENDNODE    18

// columns for detail list

#define COL_SIM_ACC     0
#define COL_SIM_DELTA   1
#define COL_SIM_SLOPE   2
#define COL_SIM_ERROR   3
#define COL_DET_SEP_SIM 4
#define COL_REF_ACC     5
#define COL_REF_DELTA   6
#define COL_REF_SLOPE   7
#define COL_DET_SEP_REF 8
#define COL_REF_LAG_ACC     9
#define COL_REF_LAG_DELTA   10
#define COL_DET_SEP_REF_LAG 11

#define COL_SSTA_MIN    12
#define COL_SSTA_MOY    13
#define COL_SSTA_MAX    14
#define COL_SSTA_SIGMA    15


#define COL_CTK_ACC     16
#define COL_CTK_DELTA   17
#define COL_CTK_SLOPE   18
#define COL_DET_SEP_CTK 19
#define COL_CTK_LAG_ACC     20
#define COL_CTK_LAG_DELTA   21

#define COL_CTK_SCORE_TOT     22
#define COL_CTK_SCORE_NOISE   23
#define COL_CTK_SCORE_CTK     24
#define COL_CTK_SCORE_ACTIV   25
#define COL_CTK_SCORE_INTERV  26
#define COL_CTK_SCORE_PROBA   27

#define COL_DET_SEP_CTK_LAG 28
#define COL_CAPA          29
#define COL_DET_SEP_CAPA  30
#define COL_NODETYPE       31
#define COL_DET_SEP_NODETYPE  32
#define COL_NODENAME      33
#define COL_DET_SEP_NODENAME  34
#define COL_NETNAME       35
#define COL_DET_SEP_NETNAME   36
#define COL_LINETYPE     37
#define COL_DET_SEP_LINETYPE 38
#define COL_TRANSISTORS   39
#define COL_DET_SEP_TRANSISTORS 40
#define INFO_CMD     41
#define GLOBAL_COL_REF  42
#define GLOBAL_COL_CTK   43
#define GLOBAL_COL_SIM   44
#define GLOBAL_COL_CTKLAG   45
#define GLOBAL_COL_REFLAG   46

// columns for stb error list

//#define COL_INDEX 0
#define COL_START 1
#define COL_ERRL_SEP0 2
#define COL_LATCH 3
#define COL_ERRL_SEP1 4
#define COL_NAME  5
#define COL_ERRL_SEP2 6
#define COL_SETUP 7
#define COL_ERRL_SEP3 8
#define COL_HOLD  9

// columns for slack report

#define COL_SLACK_SEP5 2
#define COL_SLACK   3
#define COL_SLACK_SEP0 4 
#define COL_DV_TIME 5
#define COL_DV_REL 6
#define COL_SLACK_SEP1 7
#define COL_DR_TIME 8
#define COL_DR_REL 9
#define COL_SLACK_SEP2 10
#define COL_STARTEND 11
#define COL_SLACK_SEP3 12
#define COL_DATAPATHMARGIN 13
#define COL_CLOCKPATHMARGIN 14
#define COL_INTRINSIC_MARGIN 15
#define COL_SLACK_SEP6 16
#define COL_FROMNODEDIR 17
#define COL_FROMNODE 18
#define COL_SLACK_SEP4 19
#define COL_TONODEDIR 20
#define COL_TONODE 21
#define COL_THRUDEBUG 22

// columns for con2latcherror

#define COL_CONNECTOR_DIR     1
#define COL_CONNECTOR         2
#define COL_C2L_SEP_CONNECTOR 3
#define COL_LATCH_DIR         4
#define COL_LATCH_C2L         5
#define COL_C2L_SEP_LATCH     6
#define COL_SETUP_C2L         7
#define COL_DATAMAX           8
#define COL_DATAMAX_MARGIN    9
#define COL_CLOCKMIN          10
#define COL_CLOCKMIN_MARGIN   11
#define COL_C2L_SEP_SETUPHOLD 12
#define COL_HOLD_C2L          13
#define COL_DATAMIN           14
#define COL_DATAMIN_MARGIN    15
#define COL_CLOCKMAX          16
#define COL_CLOCKMAX_MARGIN   17

// clock path
#define COL_CP_NAME        2
#define COLUMN_DISTANCE    5



// detail control
extern long detail_forced_mode;
extern int ffdebug;

#define RELATIVE_DETAIL 1
#define NOSUPINFO_DETAIL       2
#define IDEAL_CLOCK 4
//

extern char STAB_CONFIG_SHOW[];
extern char C2L_CONFIG_SHOW[];


typedef void (*more_func)(char *dec, FILE *f);




char *FormaT(double val, char *buf, char *formatnum);
void ttv_DisplayRoute(FILE *f, ttvevent_list *tve, char type, chain_list *pthlist, int prech);
void _ttv_Board_SetValue(char *tab, int scol, Board *B, int col, char *val);
void _ttv_Board_SetSize(char *tab, int scol, Board *B, int col, int size, char align);
void _ttv_Board_SetSep(char *tab, int scol, Board *B, int col);

#define SLACK_PTYPE 0xfab50819
#define DEBUG_PTYPE 0xfab50928
#define MORE_PTYPE 0xfab60517

void ttvapi_setprefix(char *val);
char *ttv_GetFullSignalName_COND(ttvfig_list *tf, ttvsig_list *sg);
char *ttv_GetDetailSignalName_COND(ttvcritic_list *tc);
double stb_periodmove(char dir, stbdebug_list *dbl);
double stb_getperiodmove(char dir, stbdebug_list *dbl);

void ttv_setsearchexclude(long valstart, long valend, long *oldvalstart, long *oldvalend);
chain_list *ttv_internal_getclockpaths(ttvfig_list *tf, char *clock, char *cmd, int number, char *minmax);
int ttv_IsClock(ttvsig_list *ts);
long ttvfigtype(ttvfig_list *tvf);
char *ttv_internal_getclockliststring(ttvfig_list *tf, char *clock);
long ttv_setsearchmode(long val);

void ttv_GetNodeSpecout(ttvevent_list *latch, double unit, char *unitformat, char *res, stbdebug_list *dbl, ttvpath_list *tp, int setuphold, long dec);
void ttv_GetNodeClock(ttvevent_list *tve, ttvevent_list *latch, double unit, char *unitstring, char *unitformat, char *res, stbdebug_list *dbl, ttvpath_list *tp);

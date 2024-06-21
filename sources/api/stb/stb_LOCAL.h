

typedef struct 
{
  stbdebug_list *dbl;
  char input_event;
  char latch_event;
} SH_info;


typedef struct
{
  ttvpath_list *path;
  float datavalid, /*datavalidclock,*/ datareq, val, period, slack, lag;
  char /*datavalidclockdir,*/ dirout, dirin, access, hzpath;
  unsigned char phase;
  stbdebug_list debugl;  
  int jump;
  short flags;
} slackinfo;

typedef struct
{
  char setuphold, custom;
  ttvpath_list *data_valid, *data_required;
  slackinfo SI;
} slackobject;

long stbsetdebugflag(long val);
double getsetuphold(ttvsig_list *sig, int setup, int dir, ttvsig_list *siginput, ttvevent_list *in_ev, ttvevent_list *cmd, SH_info *shi);
double getsetupholdinstab(stbdebug_list *dbl, int setup, int dir);

extern long stb_SUPMODE;

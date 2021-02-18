
typedef struct
{
  int count;
  ttvfig_list *tf;
//  inffig_list *ifl;
//  HeapAlloc myheap;
  chain_list *directivedata;
  chain_list *connectordone;
} ConstraintObjectManagement;

typedef struct
{
  int setup;
  ttvpath_list *datapath, *clockpath;
  double margedata, margeclock, overlapdata, overlapclock;
  double intrinsic_margin, clocklatency;
  char *intrinsic_margin_model;
  ttvevent_list *masterck;
  ConstraintObjectManagement *com;
} ConstraintObject;


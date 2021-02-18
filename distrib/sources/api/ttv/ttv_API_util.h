#define TTVAPI_NONE -1
#define TTVAPI_DN 0
#define TTVAPI_UP 1
#define TTVAPI_EITHER 2

#define TTV_API_ONLY_HZ 1
#define TTV_API_ONLY_NOTHZ 2

#define TTV_GETDETAIL_CHECK_IDEAL 0xfab71212

cnsfig_list *ttvutil_cnsload(char *figname, inffig_list *ifl);
char ttv_getUorD(char dir);
chain_list *ttv_internal_GetPaths(ttvfig_list *tf, char *clock, char *start, char *end, char *dir, long number, char *all, char *path, char *minmax);
chain_list *ttv_internal_GetPaths_EXPLICIT(ttvfig_list *tf, chain_list *clock, chain_list *start, chain_list *end, char *dir, long number, char *all, char *path, char *minmax);
void ttv_DirectionStringToIndices(char *dir, int *a, int *b);
chain_list *ttv_GetClockPath(ttvpath_list *tp, ttvevent_list **connector, ttvevent_list **pathcmd);
int ttv_DirectionToIndex(char dir);
int ttv_DirectionStringToIndicesHZ(char *dir);
double ttv_GetLatchIntrinsicQuick(ttvfig_list *tvf, ttvevent_list *latch, ttvevent_list *cmd, int setup, ttvline_list **rline);

//#define _TODL(x) ((x)*1e-12/TTV_UNIT)
//#define _TOLD(x) mbk_long_round((x)*1e12*TTV_UNIT)
//double ttv_GetLatchIntrinsicQuick(ttvfig_list *tvf, ttvevent_list *latch, ttvevent_list *cmd, int setup);

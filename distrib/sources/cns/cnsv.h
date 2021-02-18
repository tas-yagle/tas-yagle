 
/* 10/26/95 Cone Netlist Structure variables: cnsv.h                          */

/* CNS environment variables */

extern char    *CNS_VERSION;
//extern int      CNS_DRIVE_NORMAL;
//extern int      CNS_DRIVE_VERBOSE;
//extern int      CNS_DRIVE_FULL;
extern int      CNS_TRACE_MODE;
extern int      CNS_DEBUG_MODE;
extern int      CNS_VIEW_LEVEL;
extern char    *CNS_TECHNO;
extern int      CNS_POWERNUM;
extern int      CNS_MAXLINKNUM;


/* CNS structure variables */

extern cnsfig_list *CNS_HEADCNSFIG;


/* CNS connector names */

extern char    *CNS_VDDNAME;
extern char    *CNS_GNDNAME;
extern char    *CNS_VSSNAME;
extern char    *CNS_GRIDNAME;
extern char    *CNS_SOURCENAME;
extern char    *CNS_DRAINNAME;
extern char    *CNS_BULKNAME;


/* CNS structure debug variables */

extern int      CNS_DEBUG_LEVEL;
extern int      CNS_LC_VIEW;
extern int      CNS_LT_VIEW;
extern int      CNS_CF_VIEW;
extern int      CNS_CE_VIEW;
extern int      CNS_IC_VIEW;
extern int      CNS_OC_VIEW;
extern int      CNS_BH_VIEW;
extern int      CNS_LK_VIEW;

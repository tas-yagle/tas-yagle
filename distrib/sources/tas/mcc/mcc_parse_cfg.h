typedef struct moddef {
  struct moddef *NEXT ;
  char          *NAME ;
  double         VALUE ;
} moddeflist ;

typedef struct modmodel {
  struct modmodel *NEXT ;
  int              MCCTYPE ;
  int              CRITERION ;
  union {
    struct {
      double   VALUE ;
      char    *PARAM ;
    }     PARAM ;
    char *MODEL ;
  }                VALUE ;
  moddeflist      *DEFAULT ;
  char            *EXTNAME;
} modmodellist ;
#define MCC_CRIT_PARAM 1
#define MCC_CRIT_MODEL 2

typedef struct modcfg {
  struct modcfg   *NEXT ;
  char            *NAME ;
  modmodellist    *MODLIST ;
} modcfglist ;

modcfglist* mcc_getmodcfg( void );

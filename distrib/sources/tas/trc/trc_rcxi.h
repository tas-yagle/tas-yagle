/******************************************************************************\
Fonctions visibles depuis l'extérieur.
\******************************************************************************/

extern char rcxi_error[1024] ;

extern int rcxi_getcapa( lofig_list *lofig, losig_list *losig, char level, float defslope, int type, float *capa );
extern int rcxi_getnodecapa( lofig_list *lofig, losig_list *losig, lonode_list *lonode, char level, float defslope, int type, float coef, float *capa );
extern int rcxi_getresi( lofig_list *lofig, losig_list *losig, int type, float *resi );
extern int rcxi_getpiload( lofig_list *lofig, losig_list *losig, int pin, char level, float defslope, int type, float *r, float *c1, float *c2 );
extern int rcxi_getmiller( lofig_list *lofig, losig_list *victim, losig_list *aggressor, char level, float defslope, int type, float *miller );
extern char* rcxi_getfigname( lofig_list *lofig );
extern int rcxi_getagrlist( lofig_list *lofig, losig_list *losig, chain_list **head );
extern int rcxi_freeagrlist( lofig_list *lofig, losig_list *losig, chain_list *head );
extern rcxparam* rcxi_getrcxparam( lofig_list *lofig, losig_list *losig );
extern int rcxi_getcoefctc( lofig_list *lofig, losig_list *losig, char level, float defslope, char type, float *coef );

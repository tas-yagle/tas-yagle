extern RCXFLOAT rcx_getloconcapa( locon_list *locon, char, char, char, char* );
extern void rcx_setloconcapa_l0( lofig_list *lofig, loins_list *loins, locon_list *locon, char domain, RCXFLOAT capamin, RCXFLOAT capamax );
extern void rcx_setloconcapa_l1( lofig_list *lofig, loins_list *loins, locon_list *locon, char domain, RCXFLOAT capa_up_min, RCXFLOAT capa_up_max, RCXFLOAT capa_dw_min, RCXFLOAT capa_dw_max);
extern void rcx_setloconcapa_l2( lofig_list *lofig, loins_list *loins, locon_list *locon, char domain, RCXFLOAT capa_up_min, RCXFLOAT capa_up_nom_min, RCXFLOAT capa_up_nom_max, RCXFLOAT capa_up_max, RCXFLOAT capa_dw_min, RCXFLOAT capa_dw_nom_min, RCXFLOAT capa_dw_nom_max, RCXFLOAT capa_dw_max);
extern void rcx_addloconcapa_l0( lofig_list *lofig, loins_list *loins, locon_list *locon, char domain, RCXFLOAT capamin, RCXFLOAT capamax );
extern void rcx_addloconcapa_l1( lofig_list *lofig, loins_list *loins, locon_list *locon, char domain, RCXFLOAT capa_up_min, RCXFLOAT capa_up_max, RCXFLOAT capa_dw_min, RCXFLOAT capa_dw_max);
extern void rcx_addloconcapa_l2( lofig_list *lofig, loins_list *loins, locon_list *locon, char domain, RCXFLOAT capa_up_min, RCXFLOAT capa_up_nom_min, RCXFLOAT capa_up_nom_max, RCXFLOAT capa_up_max, RCXFLOAT capa_dw_min, RCXFLOAT capa_dw_nom_min, RCXFLOAT capa_dw_nom_max, RCXFLOAT capa_dw_max);
float* rcx_loconcapa_addspace( lofig_list *lofig, loins_list *loins, locon_list *locon );
float* rcx_loconcapa_getspace( locon_list *locon );
HeapAlloc* rcx_loconcapa_giveheap( lofig_list *lofig, loins_list *loins );
void rcx_loconcapa_freeheap( lofig_list *lofig, loins_list *loins );
extern RCXFLOAT rcx_getsigcapa ( lofig_list*, losig_list*, char, char, char, rcx_slope*, char, char );
RCXFLOAT rcx_getnodecapa ( losig_list*, lonode_list*, RCXFLOAT, rcx_slope*, char, char, RCXFLOAT );
extern float rcx_getcapa( lofig_list *lofig, losig_list *losig );
void rcx_freeloconcapa( lofig_list*, loins_list*, locon_list* );
char rcx_loconcapa_freespace( lofig_list*, loins_list*, locon_list* );
void rcx_transfert_capa_from_instance_to_figure( loins_list *loins, lofig_list *lofig, locon_list *locon );
void rcx_add_capa_from_instance_to_figure( loins_list *loins, lofig_list *lofig, locon_list *loconint, locon_list *loconext );
extern float rcx_get_all_locon_capa( losig_list *losig, char slope, char type, char domain );
void rcx_add_capa_locon( lofig_list *lofig, locon_list *locona, locon_list *loconb );




// Résistance par défaut quand le modèle de bruit n'est pas SCR.
extern RCXFLOAT STB_NOISE_DEFAULT_RESI;

chain_list* stb_ctk_noise_event_list( stbfig_list *stbfig, ttvevent_list *evtvic, chain_list *headevent, rcx_signal_noise *tab, char calcul, RCXFLOAT *v );
timing_model* stb_ctk_noise_get_model_stm( ttvfig_list *ttvfig, ttvevent_list *event, long level, long type );
void stb_ctk_noise_node_gaplist( stbfig_list *stbfig, ttvevent_list *ttv_victime, lofig_list*, losig_list *lo_victime, stbgap_list *gaplist, chain_list *alwaysactif, long level, long type, long calcul, char**, float*, float* );
void stb_addttvsigfast( ttvfig_list *topfig, char *insname, losig_list *losig, ttvsig_list *ttvsig );
ttvsig_list* stb_getttvsigfast( ttvfig_list *topfig, char *insname, losig_list *losig );
int stb_ctk_noise_compare( stb_ctk_tab_stat *n1, stb_ctk_tab_stat *n2 );
void stb_ctk_noise_report( stbfile *report, stb_ctk_stat *tabnoise );
void stb_ctk_report_detail( stbfig_list *stbfig, ttvevent_list *victim, chain_list *head, long calcul, chain_list *excluded );
char* stb_geteventdomainnamelist( stbfig_list *stbfig, ttvevent_list *node );
int stb_ctk_noise_node( stbfig_list *stbfig, lofig_list *figvic, stb_ctk_detail *detail, chain_list *headagr, long level, long type, char **ovr_mod, float *ovr_max, float *ovr_real, char **und_mod, float *und_max, float *und_real );


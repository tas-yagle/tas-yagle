
extern stm_simu_pi_load_parameter stm_simu_param_standard;
char stm_simu_pi_load_ts( char (*fn_is)( void *dat_is, float ve, float vs, float *is ), char (*fn_ve)( void *dat_ve, float t, float *v ), void  *dat_is, void  *dat_ve, float r, float c1, float c2, float vi, float vth, float *ts, float *fs, stm_simu_pi_load_parameter*, char *plotcurvename );

// Fonctions de modèle de courant et de tension.
char stm_simu_mcc_ids( param_mcc_ids *param, float vgs, float vds, float *ids );
char stm_simu_tanh( stm_simu_tanh_param *param, float t, float *v );

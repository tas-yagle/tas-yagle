
int avtAnnotationDeviceConnectorSetting_init(char *var, char *val, char *result);
int tasSimulateInverter_init(char *var, char *env, char *result);
int tpiv_inverter_config_init(char *var, char *config, char *result);
int yagSimpleLatchDetection_init(char *var, char *val, char *result);
int yagDetectClockGating_init(char *var, char *val, char *result);
int avtTransparentPrecharge_init(char *var, char *val, int *result);
int ttvIgnoreMaxFilter_init(char *var, char *val, char *result);
int avtParasiticCacheSize_init(char *var, char *val, char *result);
int avt_parse_time (char *var, char *val, float *result); 
int avt_parse_capa (char *var, char *val, float *result);
int avt_parse_mem (char *var, char *val, float *result);
void avt_init_model(chain_list **list, char *str);
int avt_init_model_tn (char *var, char *val, char *result);
int avt_init_model_tp (char *var, char *val, char *result); 
int avt_init_model_dn (char *var, char *val, char *result); 
int avt_init_model_dp (char *var, char *val, char *result); 


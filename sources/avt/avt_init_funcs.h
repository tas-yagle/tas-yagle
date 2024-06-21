
int avtAnnotationDeviceConnectorSetting_init(const char *var, const char *val, char *result);
int tasSimulateInverter_init(const char *var, const char *env, char *result);
int tpiv_inverter_config_init(const char *var, const char *config, char *result);
int yagSimpleLatchDetection_init(const char *var, const char *val, char *result);
int yagDetectClockGating_init(const char *var, const char *val, char *result);
int avtTransparentPrecharge_init(const char *var, const char *val, int *result);
int ttvIgnoreMaxFilter_init(const char *var, const char *val, char *result);
int avtParasiticCacheSize_init(const char *var, const char *val, char *result);
int avt_parse_time (const char *var, const char *val, float *result);
int avt_parse_capa (const char *var, const char *val, float *result);
int avt_parse_mem (const char *var, const char *val, float *result);
void avt_init_model(chain_list **list, const char *str);
int avt_init_model_tn (const char *var, const char *val, char *result);
int avt_init_model_tp (const char *var, const char *val, char *result);
int avt_init_model_dn (const char *var, const char *val, char *result);
int avt_init_model_dp (const char *var, const char *val, char *result);


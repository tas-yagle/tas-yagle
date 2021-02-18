
extern int rcx_set_connector_origin_group(locon_list *lc, int number);
extern int rcx_set_connector_destination_group(locon_list *lc, int number);
extern int rcx_get_connector_origin_group(locon_list *lc);
extern int rcx_get_connector_destination_group(locon_list *lc);

extern rcx_timings **rcx_get_rc_timings(locon_list *origin, locon_list *destination, char dir);
extern rcx_timings *rcx_get_computed_data(rcx_timings *mytimings, float input_slope, float *computed_delay, float *computed_slope);
extern void get_computed_data(rcx_timings *mytimings, float input_slope, float *computed_delay, float *computed_slope);
extern void rcx_add_rc_timing(locon_list *origin, locon_list *destination, float input_slope, float computed_delay, float computed_slope, char dir);

extern void rcx_free_signal_rc_timings(losig_list *ls);
extern void rcx_create_signal_rc_timings(losig_list *ls);
extern int rcx_has_rc_timings(losig_list *ls);

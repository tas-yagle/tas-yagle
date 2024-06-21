
void stb_get_clock_local_latency(ttvsig_list *tvs, long *rmin, long *rmax, long *fmin, long *fmax);
int stb_compute_clock_latency(stbfig_list *sb, long type);
long stb_get_generated_clock_skew(ptype_list *geneclockinfo, ttvevent_list *tve0, ttvevent_list *tve1, ttvevent_list **common_node);
void stb_free_built_generated_clock_info(ptype_list *geneclockinfo);
ptype_list *stb_build_generated_clock_info(chain_list *clocklist);




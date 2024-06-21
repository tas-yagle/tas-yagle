
//void stb_setdirectives (ttvfig_list *tvf, inffig_list *ifl);
stbdebug_list *stb_calc_directive_setuphold(stbfig_list *stbfig, stbdebug_list *dbl, stbnode *node, long margin, long computeflags, ptype_list *geneclockinfo);
//stb_directive *stb_get_directive(ttvsig_list *tvs);
int stb_get_filter_directive_info(stbfig_list *sf, ttvevent_list *tve, char phase, long *start, long *startmax, long *end, long *endmin, char *state, long *move, ttvevent_list **clockedsig, stbck *origedge);
//long stb_getfilterlag(long abs_time, ttvevent_list *tve, long type);
int stb_has_filter_directive(ttvevent_list *tve);

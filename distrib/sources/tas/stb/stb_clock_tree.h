
void stb_free_clock_tree_info(ttvevent_list *node);
void stb_mark_path_diverging_node(stbfig_list *sbf, ttvpath_list *pth);
void stb_clock_tree_mark_divergence(stbfig_list *stbfig);
int stb_get_common_node_info(ttvevent_list *a, ttvevent_list *b, int maxdepth, long *min, long *max, ttvevent_list **common_node, int hza, int hzb);
ttvevent_list *stb_get_top_node(ttvevent_list *a, int hza);
long stb_get_generated_clock_common_node_info(ptype_list *geneclockinfo, ttvevent_list *a, ttvevent_list *b, int hza, int hzb, ttvevent_list **common);

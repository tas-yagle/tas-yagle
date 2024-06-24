//#define RELAX_CORRECT_DEBUG
typedef struct node_path_info node_path_info;

void stb_compute_falsepath_and_falseslack_effect(stbfig_list *sf, stbnode *node, ttvline_list *line, ttvevent_list *linecmd, stbck *nodeck, int flags);
valid_range_info *get_valid_range_info(ttvline_list *tl);
void stb_clean_relax_correction_info (stbfig_list *stbfig);
void stb_clean_relax_correction_path_info (stbfig_list *stbfig);
void stb_set_relax(int val);
node_path_info *stb_assign_paths(stbfig_list *stbfig, ttvevent_list *tve);



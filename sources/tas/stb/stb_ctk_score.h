extern char STB_CTK_STAT_COMPARE ;

int stb_ctk_good_score( int score );
void stb_ctk_set_score( int noise, int ctk, int interval, int activity );
int stb_ctk_score_activity( stbfig_list *stbfig, stb_ctk_detail *detail );
int stb_ctk_score_ctk( stbfig_list *stbfig, stb_ctk_detail *detail );
int stb_ctk_score_interval( stbfig_list *stbfig, stb_ctk_detail *detail );
int stb_ctk_score_noise( stbfig_list *stbfig, stb_ctk_detail *detail );
void stb_ctk_calc_score( stbfig_list *stbfig, stb_ctk_detail *ctkdetail, stb_ctk_tab_stat *stat );
float stb_ctk_signal_threshold( stbfig_list *stbfig, ttvevent_list *event );
float stb_ctk_signal_threshold_from_input( stbfig_list *stbfig, ttvevent_list *event );
int stb_ctk_score_compare( stb_ctk_tab_stat *n1, stb_ctk_tab_stat *n2 );
int stb_ctk_qsort_float( float *f1, float *f2 );
void stb_ctk_fix_min( stb_ctk_stat *stat );
int stb_ctk_stat_to_keep( stb_ctk_tab_stat *stat );
float stb_ctk_signal_get_vdd( stbfig_list *stbfig, ttvevent_list *event );

#define RCXPLM_NODE 20052007

void rcx_piload_matrix( losig_list *losig, num_list *driver, RCXFLOAT *y1, RCXFLOAT *y2, RCXFLOAT *y3, RCXFLOAT extcapa, rcx_slope *slope, char type, RCXFLOAT coefctc );
void rcxplm_build_node_index( losig_list *losig, wirefromlocon_list *group, num_list *driver, long *nbnode, long *nbdriver );
void rcxplm_free_node_index( losig_list *losig, wirefromlocon_list *group );
void rcxplm_fill_matrix( losig_list *losig, mbk_matrix *r, double *c, int nbnode, wirefromlocon_list *group, RCXFLOAT extcapa, rcx_slope *slope, char type, char coefctc );
void rcxplm_create_s( long nbnode, long nbdriver, double *c, double *u, double *s );
void rcxplm_get_y( long nbdriver, double *x1, double *x2, double *x3, double *yg1, double *yg2, double *yg3 );

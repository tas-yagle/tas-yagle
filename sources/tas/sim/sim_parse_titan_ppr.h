char sim_get_ppr_data( char *string, int n, double *data );
void sim_parse_titan_ppr( char *filename, 
                          char *printname[], 
                          int   nbprint, 
                          int   nbdata, 
                          double **tabprint, 
                          double tmax, 
                          double step
                        );

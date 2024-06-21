extern int          caracmode            __P(( cone_list*,
                                               edge_list*,
										       double**, 
											   int*, 
											   double**, 
											   int*
                                            ));
extern long         valfupfdownbis       __P(( branch_list*,
			                                   long,
                                               double
                                            ));

extern long         frontbis             __P(( cone_list*,
			                                   long,
                                               double,
                                               long*,
                                               long*
                                            ));
extern void         tas_AffectCaracCon   __P((cnsfig_list*)) ;
extern double*      autocarac            __P((cone_list*,
                                              int,
                                              double*
                                             ));
extern double       pround               __P((double, 
			                                  int 
											));
extern double*      tas_DynamicSlopes    __P((long, int));
extern double*      tas_DynamicCapas     __P((double, int));
void                tas_Compute_Delay    __P((cone_list *,
                                              edge_list *,
                                              long ,
                                              double ,
                                              double ,
                                              delay_list **,
                                              delay_list **,
                                              double *
                                            ));

extern double      *tas_DupTab           __P((double*, int)) ;
extern void         tas_getslewparams    __P((branch_list*,
                                              double*,
                                              double*,
                                              char
                                            ));
extern long         tas_getslewparamsduo __P((cone_list*,
                                              double*,
                                              double*,
                                              double*,
                                              double*
                                            ));


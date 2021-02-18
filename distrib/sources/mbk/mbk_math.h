/******************************************************************************\

Bibliothèque de manipulation des objets mathématiques MBK.

\******************************************************************************/

// Définitions locales 

typedef struct mbk_pwl_point {
  struct mbk_pwl_point  *PREV;
  struct mbk_pwl_point  *NEXT;
  double                 X;     // Point d'abscisse
  double                 Y;     // Ordonnée
  double                 P;     // Pente sur ce point
  double                 O;     // Origine de la pente
} mbk_pwl_point_list ;

#define LAPLACEDATA(pt,i) ((mbk_laplace_data*) ((pt)->DATA + i * (pt)->SIZE ))

mbk_pwl_point_list* mbk_pwl_alloc_point( void );
void mbk_pwl_free_point( mbk_pwl_point_list *pt );
void mbk_pwl_free_point_list( mbk_pwl_point_list *head );
void mbk_pwl_free_param_point_list( mbk_pwl_param_point_list *head );
mbk_pwl* mbk_pwl_alloc_pwl( int n );
char mbk_pwl_calc_deriv( char (*fn)(void*, double, double*), 
                         void *data,
                         double x0, 
                         double x1, 
                         double *p 
                       );
char mbk_pwl_fill_point( char (*fn)(void*, double, double*),
                         void *data,
                         double x,
                         double x1,
                         double x2,
                         mbk_pwl_point_list *point
                       );
int mbk_qsort_dbl_cmp( double *x1, double *x2 );
mbk_pwl* mbk_pwl_create_surface( char (*fn)(void*, double, double*),
                                 void *data,
                                 double x0,
                                 double x1,
                                 mbk_pwl_param *param
                               );
mbk_pwl* mbk_pwl_create_default( char (*fn)(void*, double, double*),
                                 void *data,
                                 double x0,
                                 double x1,
                                 mbk_pwl_param *param
                               );
mbk_laplace* mbk_alloc_laplace( int n, int size );
mbk_pwl* mbk_pwl_point_to_pwl( mbk_pwl_point_list *head );
mbk_pwl_point_list* mbk_pwl_param_point_to_point( 
                                        char (*fn)(void*, double, double* ),
                                        void *data,
                                        double x0,
                                        double x1,
                                        mbk_pwl_param *param
                                                );
long mbk_long_round(double val);


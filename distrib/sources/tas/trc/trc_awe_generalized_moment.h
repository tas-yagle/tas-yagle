typedef struct awe_matrix_node {
  struct awe_matrix_node *NEXT ;
  RCXFLOAT                CAPA ;
  int                     INDEX ; /* -1 mean a lonode on driver */
  lonode_list            *LONODE ;
  unsigned int            FLAGS ;
  
} awe_matrix_node ;

typedef struct awe_hybrid_matrix {
  struct awe_hybrid_matrix      *NEXT ;
  mbk_matrix                    *MATRIX ;
  chain_list                    *WIRELIST ;
  chain_list                    *INTNODELIST ;
  chain_list                    *EXTNODELIST ;
  int                            DIM ;
  int                            SDIM ;
} awe_hybrid_matrix ;

typedef struct awe_hybrid_node {
  struct awe_hybrid_node        *NEXT ;
  awe_hybrid_matrix             *HYBRID ;
  int                            INDEX ;
} awe_hybrid_node ;

#define AWE_FLAG_HYBRID   ((unsigned int)0x00000001)
#define AWE_FLAG_NORMAL   ((unsigned int)0x00000002)
#define AWE_FLAG_DRIVER   ((unsigned int)0x00000004)

#define AWE_NODE_M 0x5243581D
#define AWE_NODE_H 0x5243581E
#define AWE_EQUIV_NODE 0x52435820

extern float AWE_MIN_RESI_VALUE ;

void build_awe_moment_with_loop( losig_list *losig, 
                                 locon_list *driver, 
                                 RCXFLOAT    extcapa, 
                                 rcx_slope  *slope, 
                                 char        type, 
                                 RCXFLOAT    coefctc );
void sort_node( losig_list         *losig, 
                num_list           *driver, 
                wirefromlocon_list *group,
                RCXFLOAT            extcapa,
                rcx_slope          *slope,
                char                type,
                RCXFLOAT            coefctc,
                awe_matrix_node   **valid_node,
                awe_matrix_node   **hybrid_node
              );
void test_sort_node( losig_list         *losig, 
                     lonode_list        *mtxnode,
                     RCXFLOAT            extcapa,
                     rcx_slope          *slope,
                     char                type,
                     RCXFLOAT            coefctc
                   );
void free_sort_node( awe_matrix_node *nodelist );
void build_node_index( awe_matrix_node *nodelist );
awe_matrix_node* get_matrix_node( lonode_list *ptnode );
awe_matrix_node* add_awe_matrix_node( lonode_list *ptnode );
awe_hybrid_matrix* build_sort_hybrid( losig_list *losig, 
                                      awe_matrix_node *hybrid_node 
                                    );
void build_sort_hybrid_rec( losig_list        *losig, 
                            lonode_list       *from, 
                            lowire_list       *wire,
                            awe_hybrid_matrix *hmat
                          );
void set_hybrid_from_node( lonode_list       *lonode, 
                           awe_hybrid_matrix *hmat, 
                           char               external 
                         );
awe_hybrid_node* get_hybrid_from_node( lonode_list *lonode, 
                                       awe_hybrid_matrix *hmat 
                                     );
void free_hybrid( awe_hybrid_matrix *hlist);
awe_hybrid_matrix* alloc_hybrid_matrix( void );
void build_fill_hybrid_list( losig_list *losig, 
                             awe_hybrid_matrix *hybrid_list,
                             double k
                                         );
void build_fill_hybrid( losig_list *losig, 
                        awe_hybrid_matrix *matrix,
                        double k
                      );
void awe_add_admi( mbk_matrix *matrix, int i, int j, double val );
void build_awe( losig_list *losig, locon_list *driver, int single_driver, awe_matrix_node *node_list, awe_hybrid_matrix *hybrid_list, double extcapa, chain_list *wirelist );

mbk_matrix* build_awe_matrix( losig_list *losig, awe_matrix_node *node_list, awe_hybrid_matrix *hybrid_list, double k, chain_list *wirelist );
void awebuildinfomatrix( losig_list *losig, locon_list *driver, int single_driver, double k, double extcapa, double **tabmoment ) ;
void awe_buildinfomatrix_hybrid( losig_list *losig, locon_list *driver, int single_driver, locon_list *load, awe_matrix_node *mnode, double **tabmoment, double k, double extcapa );
int awe_rc_product( int x, int y, double *val, double *data );
lonode_list* awe_get_equiv_lonode( lonode_list *lonode );
void awe_build_equiv_lonode( losig_list *losig, chain_list *headwire );
void awe_clean_equiv_lonode( losig_list *losig, chain_list *headwire );
void awe_find_nul_wire_rec( chain_list **head, losig_list *losig, lonode_list *lonode );

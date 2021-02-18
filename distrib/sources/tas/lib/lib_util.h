/******************************************/
/* lib_util.h                             */
/******************************************/
/******************************************************************************/
/* INCLUDE                                                                    */
/******************************************************************************/
#ifndef LIB_UTIL_H
#define LIB_UTIL_H

#include LIB_H
#include BEH_H
#include CBH_H


/******************************************************************************/
/* DEFINE                                                                     */
/******************************************************************************/
#define     TYPE_STRING 'S'
#define     TYPE_FLOAT  'F'

/* directions for pin */
#define     IN      'I'
#define     OUT     'O'
#define     IO      'B'
#define     IOT     'T'
#define     TRI     'Z'
#define     INT     'X'
/******************************************************************************/
/* STRUCTURES                                                                 */
/******************************************************************************/
typedef struct s_lib_group {
    
    struct s_lib_group *NEXT;   
    long                TYPE;       /* identificateur du groupe */
    chain_list         *NAMES;      /* liste des noms du groupe */
    chain_list         *S_ATTR;     /* liste de simple attribute */
    ht                 *S_ATTRHT;   /* hash table des simple attribute (non utilisee encore)*/
    chain_list         *C_ATTR;     /* liste de complex attribute */
    ht                 *C_ATTRHT;   /* hash table des complex attribute (non utilisee encore)*/
    struct s_lib_group *SONS;       /* liste des (sous-)groupes inclus dans ce groupe */
    struct s_lib_group *LAST_SON;   /* pointeur sur le dernier groupe fils */
    struct s_lib_group *OWNER;      /* pointeur sur le groupe qui inclut ce groupe */
    
} lib_group;

typedef struct s_lib_simple_attr {
    
    long                TYPE;
    char               *VALUE;
    lib_group          *OWNER;  /* pointeur sur le groupe auquel appartient cet attribut */
   
} lib_s_attr;

typedef struct s_lib_complex_attr {
    
    long                TYPE;
    chain_list         *VALUES;
    lib_group          *OWNER;  /* pointeur sur le groupe auquel appartient cet attribut */
   
} lib_c_attr;


typedef struct s_scale_unit {
    char UNIT;
    float FACTOR;
} lib_scale_unit;


    
                

/******************************************************************************/
/* GLOBALS                                                                    */
/******************************************************************************/
extern  num_list        *LIB_PTRCONTEXT;
extern  lib_group       *LIB_GP_HEAD;
extern  lib_group       *LIB_CURRENT_GP;
extern  char             LIB_TRACE_MODE;
extern  lib_scale_unit  *LIB_TIME_U, *LIB_CAPA_U, *LIB_RES_U;
extern  char            *LIB_BUS_DELIM;
extern  ptype_list      *libparserpowerptype;

/******************************************************************************/
/* EXTERNS                                                                    */
/******************************************************************************/

/******************************************************************************/
/* FONCTIONS                                                                  */
/******************************************************************************/
extern  char            *lib_unquote(char *source);
extern  void             lib_push_context(long data);
extern  void             lib_pop_context();
extern  long             lib_get_context();
extern  long             lib_give_context(char *ident);
extern  void             lib_setenv();
extern  char             lib_get_char_dir(char *str_dir);
extern  void             lib_aff_cbhcomb(cbhcomb *p);
extern  void             lib_aff_cbhseq(cbhseq *p);
extern  void             lib_init(void);
extern  char            *lib_chainlistToStr(chain_list *pchain);
extern  chain_list      *lib_strToChainFloat(char *s);
extern  void             lib_affchain(chain_list *p, char *format, char type);
extern  lib_group       *lib_creategroup(char *type, chain_list *names);
extern  lib_group       *lib_addgroup(lib_group *g, lib_group *newgp);
extern  void             lib_pushgroup(char *type, chain_list *names);
extern  void             lib_popgroup(void);
extern  long             lib_get_s_attr_type(char *name);
extern  lib_s_attr      *lib_create_s_attr(long type, char *val);
extern  void             lib_add_s_attr(char *name, char *val);
extern  char            *lib_get_s_attr_val(lib_group *g, long type);
extern  long             lib_get_c_attr_type(char *name);
extern  lib_c_attr      *lib_create_c_attr(long type, chain_list *val);
extern  void             lib_add_c_attr(char *name, chain_list *val);
extern  chain_list      *lib_get_c_attr_val(lib_group *g, long type);
extern  lib_group       *lib_get_son_group(lib_group *g, long type);
extern  lib_scale_unit  *lib_get_time_unit(char *attr_val);
extern  lib_scale_unit  *lib_get_res_unit(char *attr_val);
extern  lib_scale_unit  *lib_get_capa_unit(chain_list *attr_val);
extern  lib_group       *lib_del_group(lib_group *g);
extern  void             lib_aff_hgp(lib_group *g, int ntabs);
extern  char            *lib_get_str_type(long type);
extern  void             lib_del_one_s_attr(lib_group *g, long sa_type);
extern  void             lib_del_all_s_attr(lib_group *g);
extern  void             lib_del_one_c_attr(lib_group *g, long sa_type);
extern  void             lib_del_all_c_attr(lib_group *g);
extern  void             lib_func_get_bus_size(lib_group *g, int *b_from, int *b_to);


#endif





#define __node_ struct s_noeud
#define subckt struct s_circuit

#define TAG_ALIM 0x10
#define TAG_MASK 0x0f

struct __equi_;

typedef struct __names_
{
  struct __names_ *next;
  char *ins;
  char *con;
} __names_;

typedef struct __item_
{
  struct __item_ *next;
  __node_ *a, *b;
  float value;
  char type;
} __item_;


typedef struct __equi_
{
  struct __equi_ *next;
  union
  {
    struct __equi_ *prev;
    char *nom;
  } a;
  __node_ *nodes, *lastnode;
  __item_ *items;
  int firstline;
  int lastline;
  union
  {
    int nbnodes;
    int SIGNAL;
  } b;
  int nbelem;  
  char tag;
} __equi_;


void _spispef_complete_node(subckt *cir, char *name, __node_ *n, int mark, int justcreated);
void _spispef_mergenodes(subckt *cir, __node_ *a, __node_ *b);
void _spispef_setnodename(subckt *cir, __node_ * n, char *ins, char *con);
void SpiceToSpef(char *name, char *ext, chain_list *cirs, long freespace);

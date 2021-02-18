typedef struct s_todo
{
  struct s_todo       *next;
  lonode_list         *ptnode;
  chain_list          *head_wire;
} todolist ;

#define RCNWIREPACK 0x27022004
#define RCN_MAXCHAIN 6

extern void             duplorcnet(losig_list*, losig_list*);
extern int              breakloop(losig_list*);
       todolist*        breakloop2(losig_list*, lonode_list*, lowire_list*, todolist*, chain_list**);
extern int              reduce_rcn(losig_list*);
extern int              connexe(losig_list*);
       void             connexe_rec(losig_list*, lowire_list*, lonode_list*, int);
extern void             clean_connexe(losig_list*);
extern chain_list*      getway(losig_list*, long, long);
       chain_list*      getwayrec(losig_list*, long, long, lowire_list* );
       void             rcn_dumprcn(losig_list*, long, char*, lowire_list*, FILE* );
       int              rcn_testandmarknode(lonode_list*);

extern void             rcn_setcoupledinformation( losig_list*, losig_list*, long, long );
extern void             rcn_freecoupledinformation( losig_list *losig, long );

int rcn_treetrip_rec( losig_list *losig, lonode_list *lonode, int fn_up( losig_list*, lonode_list*, lowire_list*, void* ), int fn_down( losig_list*, lonode_list*, chain_list*, ht*, void* ), int fn_pack_up( losig_list*, lonode_list*, void*, void* ), lowire_list *from, void *prevpack, void *userdata, int maxdepth );
extern int rcn_treetrip( losig_list *losig, lonode_list *lonode, int fn_up( losig_list*, lonode_list*, lowire_list*, void* ), int fn_down( losig_list*, lonode_list*, chain_list*, ht*, void* ), int fn_pack_up( losig_list*, lonode_list*, void*, void* ), void *userdata, int maxdepth );
extern void* rcn_getpack( losig_list *losig, lowire_list *wire );
extern void rcn_setpack( losig_list *losig, lowire_list *wire, void *pack );
extern void rcn_clearpack( losig_list *losig, lowire_list *wire );
chain_list* rcn_expandpack( losig_list *losig, lonode_list *node, chain_list *hwire, char search );
extern void rcn_clean_para( losig_list *losig );
int rcn_build_para_from_node( losig_list *losig, lonode_list *lonode );
extern void rcn_build_para( losig_list *losig );
extern float rcn_get_resi_para( lowire_list *wire ) ;
extern int rcn_tripconnexe_rec( losig_list  *losig, lonode_list *from, lowire_list *wire, int fn( losig_list*, lowire_list*, void*), void *userdata );
extern int rcn_tripconnexe( losig_list *losig, int startnode, int fn( losig_list*, lowire_list*, void*), void *userdata );
int rcn_tripconnexe_rec( losig_list  *losig, lonode_list *from, lowire_list *wire, int fn( losig_list*, lowire_list*, void*), void *userdata );
extern long rcn_getcoupledinformation( losig_list *victime, losig_list *agresseur, long ptype );


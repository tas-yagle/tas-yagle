typedef struct st_rcncacheline {
  losig_list    *LOSIG;
  int            PREV;
  int            NEXT;
}
rcncacheline;

/* Les réseaux RC */

extern lorcnet_list*    addlorcnet(losig_list*);
extern lorcnet_list*    givelorcnet(losig_list*);
extern int              freelorcnet(losig_list*);
extern int	        rclevel(losig_list*);
       lorcnet_list*    heaprcnet(void);
       void             freercnet(lorcnet_list*);
       void             chkrcn(losig_list*);

/* La capacité due aux RC */

extern float            rcn_getcapa(lofig_list*, losig_list*);
extern float            rcn_setcapa(losig_list*, float);
extern float            rcn_addcapa(losig_list*, float);
extern float            rcn_removecapa(losig_list*,float);
extern char             rcn_existcapa(lofig_list*,losig_list*);
extern void             rcn_synccapa( lofig_list*, losig_list* );
extern char             rcn_isnetcapaok(losig_list*);
extern void             rcn_setnetcapaok(losig_list*);
extern float            rcn_calccapa(losig_list*);
extern float            rcn_getnetcapa(losig_list*);
extern float            rcn_getcouplingcapacitance( lofig_list*, losig_list*, losig_list *);

/* Les wires */

extern lowire_list*     addlowire(losig_list*, unsigned char, float, float, long, long);
extern void             addlowireparam(lowire_list *ptlowire, char *param, float value, char *expr);
extern float            getlowireparam(lowire_list *ptlowire, void *param, char **expr, int *status);
extern void             freelowireparams(lowire_list *ptlowire);
extern int              dellowire(losig_list*, long, long);
extern lowire_list*     getlowire(losig_list*, long, long);
extern void             mergedellowire(losig_list* );
extern void             setdellowire(losig_list*, lowire_list*);
extern void             addcapawire(lowire_list*, float);
extern void             setcapawire(lowire_list*, float);
extern void             addresiwire(lowire_list*, float);
extern void             setresiwire(lowire_list*, float);
extern void             addpararesiwire(lowire_list*, float );
extern lowire_list*     heaplowire(void);
extern void             freelowire(lowire_list*);
extern void             rcn_changewirenodes( losig_list*, lowire_list*, int, int );
 
extern chain_list *rcn_get_a_r_way(losig_list *ptsig, long org, long dest);
extern int rcn_capacitancetooutput(int cachemode, losig_list *ptlosig, loctc_list *ptctc);
/* Les CTC */

extern loctc_list*      addloctc(losig_list*, long int, losig_list*, long int, float);
extern void             addloctcparam(loctc_list *ptloctc, char *param, float value, char *expr);
extern float            getloctcparam(loctc_list *ptloctc, void *param, char **expr, int *status);
extern void             freeloctcparams(loctc_list *ptloctc);
extern void             delloctc(loctc_list*);
extern void             freectclist(lofig_list*, chain_list*);
extern chain_list*      getallctc(lofig_list*);
extern int              rcn_mergectclosig(losig_list*);
extern int              rcn_mergectclofig(lofig_list*);
extern loctc_list*      heaploctc(void);
extern void             freeloctc(loctc_list*);
extern void             rcn_changectcnodes( loctc_list*, int, int );
extern int              rcn_isCapaDiode (loctc_list *ctc);
extern void             rcn_setCapaDiode (loctc_list *ctc, int cnt);
extern void             rcn_unsetCapaDiode (loctc_list *ctc);

/* Les noeuds */

extern long             addlonode(losig_list*, locon_list*);
extern lonode_list*     getlonode(losig_list*, long );
extern lonode_list*     givelonode(losig_list*, long);
extern void             clearallwireflag(losig_list*, unsigned char);
extern void             freetable(losig_list*);
extern void             buildtable(losig_list*);
extern chain_list*	gettabnode(lorcnet_list*);

/* Les PNODEs */

extern chain_list*      getloconnode(lonode_list*);
extern void             setloconnode(locon_list*, long);
extern void             delloconnode(locon_list*, long);
extern void             delrcnlocon(locon_list*);

/* Cache des tables de noeud */

       void             rcn_addlosigincachetable(losig_list*);
       void             rcn_removelinefromcachetable(int);
       void             rcn_removelosigfromcachetable(losig_list*);

/* Des services */

extern int              chkloop( losig_list* );
       int              chkloopmain( losig_list*, unsigned char, long, lowire_list*);
       void             rcn_error( int code, int type );
       
extern void rcn_add_low_capa_ifneeded(losig_list *ls, losig_list *gnd);


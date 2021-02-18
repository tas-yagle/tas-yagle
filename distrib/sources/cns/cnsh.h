
/*10/14/97 Cone Netlist Structure functions: cnsh.h                          */

void            cnsenv(void);

void            viewbranch(branch_list *ptbranch, int depth);
void            viewcell(cell_list *ptcell, int depth);
void            viewcone(cone_list *ptcone, int depth);
void            displaycone(cone_list *ptcone);
void            displayconelog(int lib, int loglevel, cone_list *ptcone);
void            viewcnsfig(cnsfig_list *ptcnsfig, int depth);
void            viewcnslocon(locon_list *ptlocon, int depth);
void            viewcnslotrs(lotrs_list *ptlotrs, int depth);
void            viewcnsptype(ptype_list *ptptype);
void            viewedge(edge_list *ptedge, int depth);
void            viewincone(edge_list *ptincone, int depth);
void            viewlink(link_list *ptlink, int depth);
void            viewoutcone(edge_list *ptoutcone, int depth);

void           *initbranchmem(void);
void           *initcellmem(void);
void           *initcnsfigmem(void);
void           *initconemem(void);
void           *initedgemem(void);
void           *initlinkmem(void);

void           *freelink(link_list *ptlink);
void           *freelklist(link_list *ptlklist);
void           *freeedge(edge_list *ptedge);
void           *freeeelist(edge_list *pteelist);
void           *freecone(cone_list *ptcone);
void           *freecelist(cone_list *ptcelist);
void           *freecnsfig(cnsfig_list *ptcnsfig);
void           *freecglist(cnsfig_list *ptcglist);
void           *freebranch(branch_list *ptbranch);
void           *freebrlist(branch_list *ptbrlist);
void           *freecell(cell_list *ptcell);
void           *freecllist(cell_list *ptcllist);
void            freelotrsconefig(cnsfig_list *ptcnsfig);
void            freeconelotrsfig(cnsfig_list *ptcnsfig);
void            freeconelotrs(cone_list *ptcone);

branch_list    *addbranch(branch_list *ptbranchhead, long type, link_list *ptlink);
cell_list      *addcell(cell_list *ptcellhead, long type, chain_list *ptchain, befig_list *ptbefig);
cnsfig_list    *addcnsfig(cnsfig_list *ptcnsfighead, char *name, locon_list *ptlocon, locon_list *ptintcon, lotrs_list *ptlotrs, loins_list *ptloins, cone_list *ptcone, cell_list *ptcell, lofig_list *ptlofig, befig_list *ptbefig);
void            addcnsintlocon(cnsfig_list *ptcnsfig, long index, char *name, char direction);
void            addcnslocon(cnsfig_list *ptcnsfig, long index, char *name, char direction);
locon_list     *addcnslocontolist(locon_list *ptheadlocon, long index, char *name, char direction);
void            addcnslotrs(cnsfig_list *ptcnsfig, long index, cone_list *drvcn, long x, long y, long w, long l, long ps, long pd, long xs, long xd, char type, losig_list *ptgrid, losig_list *ptsource, losig_list *ptdrain, losig_list *ptbulk, char *name);
cone_list      *addcone(cone_list *ptconehead, long index, char *name, long type, long tectype, long xmax, long xmin, long ymax, long ymin, edge_list *ptincone, edge_list *ptoutcone, branch_list *ptbrext, branch_list *ptbrvdd, branch_list *ptbrgnd, branch_list *ptbrvss, chain_list *ptchain);
void            addconebranch(cone_list *ptcone, branch_list *ptconebr, long type, link_list *ptlink);
void            addconelotrs(cone_list *ptcone);
void            addconelotrsfig(cnsfig_list *ptcnsfig);
edge_list      *addedge(edge_list *ptedgehead, long type, void *data);
void            addincone(cone_list *ptcone, long type, void *data);
link_list      *addlink(link_list *ptlinkhead, long type, void *data, losig_list *ptsig);
void            addoutcone(cone_list *ptcone, long type, void *data);

cnsfig_list    *getcnsfig(char *name, lofig_list *ptlofig);
lotrs_list     *getcnslotrs(cnsfig_list *ptcnsfig, long index);
locon_list     *getcnslocon(cnsfig_list *ptcnsfig, long index, char *name);
cone_list      *getcone(cnsfig_list *ptcnsfig, long index, char *name);
cnsfig_list    *getloadedcnsfig(char *name);

branch_list    *delbranch(branch_list *ptbranchhead, branch_list *ptbranch2del);
cell_list      *delcell(cell_list *ptcellhead, cell_list *ptcell2del);
cnsfig_list    *delcnsfig(cnsfig_list *ptcnsfighead, cnsfig_list *ptcnsfig2del);
cone_list      *delcone(cone_list *ptconehead, cone_list *ptcone2del);
void            delconebranch(cone_list *ptcone, branch_list *ptconebr, branch_list *ptbranch2del);
edge_list      *deledge(edge_list *ptedgehead, edge_list *ptedge2del);
void            delincone(cone_list *ptcone, long type, void *data);
link_list      *dellink(link_list *ptlinkhead, link_list *ptlink2del);
void            deloutcone(cone_list *ptcone, long type, void *data);

branch_list    *appendbranch(branch_list *ptbranch1, branch_list *ptbranch2);
cell_list      *appendcell(cell_list *ptcell1, cell_list *ptcell2);
cnsfig_list    *appendcnsfig(cnsfig_list *ptcnsfig1, cnsfig_list *ptcnsfig2);
cone_list      *appendcone(cone_list *ptcone1, cone_list *ptcone2);
void            appendconebranch(cone_list *ptcone, branch_list *ptconebr, branch_list *ptbranch2);
edge_list      *appendedge(edge_list *ptedge1, edge_list *ptedge2);
void            appendincone(cone_list *ptcone1, cone_list *ptcone2);
link_list      *appendlink(link_list *ptlink1, link_list *ptlink2);
void            appendoutcone(cone_list *ptcone1, cone_list *ptcone2);

void            cnsSwitchContext(cnsContext *ctx);
cnsContext     *cnsCreateContext(void);
void            cnsFreeContext(mbkContext *ctx);

chain_list     *parabrs(branch_list *ptbranch);
short           testparabrs(branch_list *ptbranch1, branch_list *ptbranch2);
short           brlotrsnum(branch_list *ptbrlist);
short           cnbrlotrsnum(cone_list *ptcone);
short           lotrsnum(link_list *ptlklist);

short           treatbr(FILE *ptfile, branch_list *ptbranch, long counter);
short           treatbrv(FILE *ptfile, branch_list *ptbranch, long counter);
void            cnsfigchain(cnsfig_list *ptcnsfig);
void            cnslofigchain(lofig_list *ptlofig, cnsfig_list *ptcnsfig);
void            savecnsfig(cnsfig_list *ptcnsfig, lofig_list *ptlofig);
void            savecnslofig(cnsfig_list *ptcnsfig, lofig_list *ptlofig, int complete);
void            savecnvfig(cnsfig_list *ptcnsfig);
int             cnsfigyyparse(void);
void            setcnslofig(lofig_list *existingfig);
cnsfig_list    *loadcnsfig(char *filename, lofig_list *ptlofig);
void            cone2locon(cone_list *ptcone, locon_list *ptlocon);
void            cone2lotrs(cone_list *ptcone, lotrs_list *ptlotrs);
void            locon2cone(locon_list *ptlocon, cone_list *ptcone);
void            lotrs2cone(lotrs_list *ptlotrs, cone_list *ptcone);
void            cnsstats(cnsfig_list *ptcnsfig, FILE *ptfile);
int             checkincone(cone_list *ptcone, branch_list *ptbranch, link_list *ptlink, void *data);
void            coneparabrs(cone_list *ptcone);

void            cnsAddConeInversion(cone_list *ptexprcone, cone_list *ptcone);
chain_list     *cnsMakeBranchExpr(branch_list *ptbranch, long linkmask, int simplify);
void            cnsMakeExtBranchExpr(branch_list *ptbranch, abl_pair *branch_abl, long linkmask, int simplify);
void            cnsMakeConeExpr(cone_list *ptcone, abl_pair *cone_abl, long branchmask, long linkmask, int simplify);
void            cnsConeFunction(cone_list *ptcone, int simplify);
int             cnsCalcConeState(cone_list *ptcone, long defaultstate);
void            cnsCalcFigState(cnsfig_list *ptcnsfig, cbhseq *ptcbhseq);
void            cnsCleanFigState(cnsfig_list *ptcnsfig, cbhseq *ptcbhseq);

int             cns_getlotrsalim(lotrs_list *lotrs, char type, float *alim);
alim_list      *cns_get_multivoltage(cone_list *ptcone);
alim_list      *cns_get_signal_multivoltage(losig_list *ls);
alim_list      *cns_get_lotrs_multivoltage(lotrs_list *lotrs);
void            cns_addmultivoltage_cone(cnsfig_list *cnsfig, inffig_list *ifl, cone_list *ptcone);
void            cns_addmultivoltage(inffig_list *ifl, cnsfig_list *ptcnsfig);
void            cns_delmultivoltage(cnsfig_list *cnsfig);

void            cnsConeFunction_once(cone_list *ptcone, int simplify);
chain_list     *cns_get_cone_external_connectors(cone_list *cn);
locon_list     *cns_get_one_cone_external_connector(cone_list *cn);
unsigned int    cns_signcns( cnsfig_list *cnsfig );


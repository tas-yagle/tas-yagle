/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_utilbeh.h                                               */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

union beobj {
    bepor_list *BEPOR;
    berin_list *BERIN;
    beaux_list *BEAUX;
    bebux_list *BEBUX;
    beout_list *BEOUT;
    bebus_list *BEBUS;
    bereg_list *BEREG;
};

#define YAG_BEAUX   ((long) 0x00000001 )
#define YAG_BEBUX   ((long) 0x00000002 )
#define YAG_BEBUS   ((long) 0x00000004 )
#define YAG_BEOUT   ((long) 0x00000008 )
#define YAG_BEREG   ((long) 0x00000010 )
#define YAG_BERIN   ((long) 0x00000020 )
#define YAG_BEPOR   ((long) 0x00000040 )

befig_list     *yagInitBefig(char *name);
void            yagCleanBehVect(void);
void            yagInitBehDefined(void);
void            yagCleanBehDefined(void);
void            yagCleanBequad(befig_list *ptbefig);
void            yagAddBerinVect(befig_list *ptbefig, char *name);
bepor_list     *yagAddBeporVect(befig_list *ptbefig, char *name, char dir, char type);
beout_list     *yagAddBeoutVect(befig_list *ptbefig, char *name, chain_list *expr, bequad_list *ptbequad, cone_list *ptcone, unsigned int time);
beaux_list     *yagAddBeauxVect(befig_list *ptbefig, char *name, chain_list *expr, bequad_list *ptbequad, cone_list *ptcone, unsigned int time);
bebus_list     *yagAddBebusVect(befig_list *ptbefig, char *name, biabl_list *ptbiabl, binode_list *ptbinode, cone_list *ptcone);
bebux_list     *yagAddBebuxVect(befig_list *ptbefig, char *name, biabl_list *ptbiabl, binode_list *ptbinode, cone_list *ptcone);
bereg_list     *yagAddBeregVect(befig_list *ptbefig, char *name, biabl_list *ptbiabl, binode_list *ptbinode, cone_list *ptcone);
int             yagBehDefined(char *name, long *pttype, union beobj *ptbeobj);
void            yagBehSetDefine(char *name, long type, union beobj ubeobj);
bepor_list     *yagBeporDefined(char *name);
beaux_list     *yagBeauxDefined(char *name);
bebux_list     *yagBebuxDefined(char *name);
beout_list     *yagBeoutDefined(char *name);
bebus_list     *yagBebusDefined(char *name);
bereg_list     *yagBeregDefined(char *name);
berin_list     *yagGetBerin(befig_list *ptbefig, char *name);
beout_list     *yagGetBeout(befig_list *ptbefig, char *name);
beaux_list     *yagGetBeaux(befig_list *ptbefig, char *name);
bebus_list     *yagGetBebus(befig_list *ptbefig, char *name);
bebux_list     *yagGetBebux(befig_list *ptbefig, char *name);
bereg_list     *yagGetBereg(befig_list *ptbefig, char *name);
beaux_list     *yagGetBedly(befig_list *ptbefig, char *name);
binode_list    *yagMakeBinode(biabl_list *ptbiabl, cone_list *ptcone);
bequad_list    *yagMakeBequadCone(cone_list *ptcone);
bequad_list    *yagMakeBequadConeList(chain_list *conelist);
biabl_list     *yagCopyBiabl(biabl_list *ptbiabl);
void yagCheckMissingDeclarations(befig_list *bef);


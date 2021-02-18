/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_cellbeh.c                                               */
/*                                                                          */
/*    (c) copyright 1995 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Anthony LESTER                        le : 10/11/1995     */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include "yag_headers.h"

befig_list *
yagBuildBefigMSSC(name, D_sig, D_cone, D, CK, S, R, NR, reg, Q, QN, fall_edge, conelist)
    char           *name;
    losig_list     *D_sig;
    cone_list      *D_cone;
    char           *D, *CK, *S, *R, *NR;
    char           *reg;
    char           *Q, *QN;
    int             fall_edge;
    chain_list     *conelist;
{
    befig_list     *ptbefig;
    cone_list      *ptdrivecone;
    biabl_list     *ptbiabl = NULL;
    chain_list     *value;
    chain_list     *condition;
    chain_list     *trigger;
    ptype_list     *ptuser;
    char            label[16];
    char            extname[YAGBUFSIZE];
    char           *lclname;
    char           *drivename;
    cone_list      *regcone = NULL;
    cone_list      *outputcone = NULL;
    chain_list     *delaycones = NULL;

    if (conelist != NULL && YAG_CONTEXT->YAG_TAS_TIMING != YAG_NO_TIMING) {
        regcone = (cone_list *)yagGetChainInd(conelist, 4);
        outputcone = (cone_list *)yagGetChainInd(conelist, 5);
        delaycones = addchain(NULL, yagGetChainInd(conelist, 3));
        delaycones = addchain(delaycones, yagGetChainInd(conelist, 2));
    }

    ptbefig = beh_addbefig(NULL, name);

    if (D_sig != NULL) {
        ptuser = getptype(D_sig->USER, YAG_CONE_PTYPE);
        if (ptuser != NULL ) ptdrivecone = (cone_list *)ptuser->DATA;
        if (ptuser == NULL || (ptdrivecone->TYPE & YAG_TEMPCONE) != 0) {
            yagAddGlueCone(D_sig, D_cone);
        }
    }

    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, D, 'I', 'B');
    ptbefig->BERIN = beh_addberin(ptbefig->BERIN, D);

    if (delaycones != NULL) {
        sprintf(extname, "%s_tasdelay", D);
        drivename = namealloc(extname);
        ptbefig->BEAUX = beh_addbeaux(ptbefig->BEAUX, drivename, createAtom(D), (pNode)yagMakeBequadConeList(delaycones), 0);
    }
    else drivename = D;

    sprintf(label, "lb%d", YAG_CONTEXT->YAG_BIABL_INDEX++);
    value = createAtom(drivename);
    condition = createExpr(AND);
    if (fall_edge) addQExpr(condition, notExpr(createAtom(CK)));
    else addQExpr(condition, createAtom(CK));
    trigger = createExpr(STABLE);
    addQExpr(trigger, createAtom(CK));
    addQExpr(condition, notExpr(trigger));
    ptbiabl = beh_addbiabl(ptbiabl, label, condition, value);

    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, CK, 'I', 'B');
    ptbefig->BERIN = beh_addberin(ptbefig->BERIN, CK);

    sprintf(extname, "%s'delayed", CK);
    lclname = namealloc(extname);
    ptbefig->BEDLY = beh_addbeaux(ptbefig->BEDLY, lclname, createAtom(CK), NULL, 0);
    ptbefig->BERIN = beh_addberin(ptbefig->BERIN, lclname);

    if (S != NULL) {
        ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, S, 'I', 'B');
        ptbefig->BERIN = beh_addberin(ptbefig->BERIN, S);
        sprintf(label, "lb%d", YAG_CONTEXT->YAG_BIABL_INDEX++);
        value = createAtom("'0'");
        condition = createAtom(S);
        ptbiabl = beh_addbiabl(ptbiabl, label, condition, value);
        if (ptbiabl->NEXT != NULL) ptbiabl->NEXT->FLAG |= BEH_CND_PRECEDE;
    }
    if (R != NULL) {
        ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, R, 'I', 'B');
        ptbefig->BERIN = beh_addberin(ptbefig->BERIN, R);
        sprintf(label, "lb%d", YAG_CONTEXT->YAG_BIABL_INDEX++);
        value = createAtom("'1'");
        condition = createAtom(R);
        ptbiabl = beh_addbiabl(ptbiabl, label, condition, value);
        if (ptbiabl->NEXT != NULL) ptbiabl->NEXT->FLAG |= BEH_CND_PRECEDE;
    }
    if (NR != NULL) {
        ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, NR, 'I', 'B');
        ptbefig->BERIN = beh_addberin(ptbefig->BERIN, NR);
        sprintf(label, "lb%d", YAG_CONTEXT->YAG_BIABL_INDEX++);
        value = createAtom("'1'");
        condition = notExpr(createAtom(NR));
        ptbiabl = beh_addbiabl(ptbiabl, label, condition, value);
        if (ptbiabl->NEXT != NULL) ptbiabl->NEXT->FLAG |= BEH_CND_PRECEDE;
    }

    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, Q, 'O', 'B');
    ptbefig->BEOUT = beh_addbeout(ptbefig->BEOUT, Q, createAtom(reg), NULL, 0);
    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, QN, 'O', 'B');
    ptbefig->BEOUT = beh_addbeout(ptbefig->BEOUT, QN, notExpr(createAtom(reg)), (pNode)yagMakeBequadCone(outputcone), 0);
    ptbefig->BEREG = beh_addbereg(ptbefig->BEREG, reg, ptbiabl, yagMakeBinode(ptbiabl, regcone),0);
    ptbefig->BERIN = beh_addberin(ptbefig->BERIN, reg);

    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, "vdd", 'I', 'B');
    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, "vss", 'I', 'B');

    return ptbefig;
}

befig_list *
yagBuildBefigMSDIFF(name, D, W, reg, Q, QN, conelist)
    char           *name;
    char           *D, *W;
    char           *reg;
    char           *Q, *QN;
    chain_list     *conelist;
{
    befig_list     *ptbefig;
    biabl_list     *ptbiabl = NULL;
    chain_list     *value, *condition, *trigger;
    char            label[16];
    char            extname[YAGBUFSIZE];
    char           *lclname;
    char           *drivename;
    cone_list      *regcone = NULL;
    cone_list      *outputcone = NULL;
    chain_list     *delaycones = NULL;

    ptbefig = beh_addbefig(NULL, name);

    if (conelist != NULL && YAG_CONTEXT->YAG_TAS_TIMING != YAG_NO_TIMING) {
        regcone = (cone_list *)yagGetChainInd(conelist, 5);
        outputcone = (cone_list *)yagGetChainInd(conelist, 6);
        delaycones = addchain(NULL, yagGetChainInd(conelist, 4));
        delaycones = addchain(delaycones, yagGetChainInd(conelist, 3));
    }

    if (delaycones != NULL) {
        sprintf(extname, "%s_tasdelay", D);
        drivename = namealloc(extname);
        ptbefig->BEAUX = beh_addbeaux(ptbefig->BEAUX, drivename, createAtom(D), (pNode)yagMakeBequadConeList(delaycones), 0);
    }
    else drivename = D;

    sprintf(label, "lb%d", YAG_CONTEXT->YAG_BIABL_INDEX++);
    value = createAtom(drivename);
    condition = createExpr(AND);
    addQExpr(condition, notExpr(createAtom(W)));
    trigger = createExpr(STABLE);
    addQExpr(trigger, createAtom(W));
    addQExpr(condition, notExpr(trigger));
    ptbiabl = beh_addbiabl(ptbiabl, label, condition, value);

    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, W, 'I', 'B');
    ptbefig->BERIN = beh_addberin(ptbefig->BERIN, W);

    sprintf(extname, "%s'delayed", W);
    lclname = namealloc(extname);
    ptbefig->BEDLY = beh_addbeaux(ptbefig->BEDLY, lclname, createAtom(W), NULL, 0);
    ptbefig->BERIN = beh_addberin(ptbefig->BERIN, lclname);

    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, D, 'I', 'B');
    ptbefig->BERIN = beh_addberin(ptbefig->BERIN, D);

    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, Q, 'O', 'B');
    ptbefig->BEOUT = beh_addbeout(ptbefig->BEOUT, Q, createAtom(reg), NULL, 0);
    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, QN, 'O', 'B');
    ptbefig->BEOUT = beh_addbeout(ptbefig->BEOUT, QN, notExpr(createAtom(reg)), (pNode)yagMakeBequadCone(outputcone), 0);
    ptbefig->BEREG = beh_addbereg(ptbefig->BEREG, reg, ptbiabl, yagMakeBinode(ptbiabl, regcone),0);
    ptbefig->BERIN = beh_addberin(ptbefig->BERIN, reg);

    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, "vdd", 'I', 'B');
    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, "vss", 'I', 'B');

    return ptbefig;
}

befig_list *
yagBuildBefigMSSX(name, D, CK, reg, Q, fall_edge, conelist)
    char           *name;
    char           *D, *CK;
    char           *reg;
    char           *Q;
    int             fall_edge;
    chain_list     *conelist;
{
    befig_list     *ptbefig;
    biabl_list     *ptbiabl = NULL;
    chain_list     *value;
    chain_list     *condition;
    chain_list     *trigger;
    char            label[16];
    char            extname[YAGBUFSIZE];
    char           *lclname;
    char           *drivename;
    cone_list      *regcone = NULL;
    cone_list      *outputcone = NULL;
    chain_list     *delaycones = NULL;

    ptbefig = beh_addbefig(NULL, name);

    if (conelist != NULL && YAG_CONTEXT->YAG_TAS_TIMING != YAG_NO_TIMING) {
        regcone = (cone_list *)yagGetChainInd(conelist, 4);
        outputcone = (cone_list *)yagGetChainInd(conelist, 5);
        delaycones = addchain(NULL, yagGetChainInd(conelist, 2));
    }

    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, D, 'I', 'B');
    ptbefig->BERIN = beh_addberin(ptbefig->BERIN, D);

    if (delaycones != NULL) {
        sprintf(extname, "%s_tasdelay", D);
        drivename = namealloc(extname);
        ptbefig->BEAUX = beh_addbeaux(ptbefig->BEAUX, drivename, createAtom(D), (pNode)yagMakeBequadConeList(delaycones), 0);
    }
    else drivename = D;

    sprintf(label, "lb%d", YAG_CONTEXT->YAG_BIABL_INDEX++);
    value = notExpr(createAtom(drivename));
    condition = createExpr(AND);
    if (fall_edge) addQExpr(condition, notExpr(createAtom(CK)));
    else addQExpr(condition, createAtom(CK));
    trigger = createExpr(STABLE);
    addQExpr(trigger, createAtom(CK));
    addQExpr(condition, notExpr(trigger));
    ptbiabl = beh_addbiabl(ptbiabl, label, condition, value);

    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, CK, 'I', 'B');
    ptbefig->BERIN = beh_addberin(ptbefig->BERIN, CK);

    sprintf(extname, "%s'delayed", CK);
    lclname = namealloc(extname);
    ptbefig->BEDLY = beh_addbeaux(ptbefig->BEDLY, lclname, createAtom(CK), NULL, 0);
    ptbefig->BERIN = beh_addberin(ptbefig->BERIN, lclname);

    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, Q, 'O', 'B');
    ptbefig->BEOUT = beh_addbeout(ptbefig->BEOUT, Q, createAtom(reg), (pNode)yagMakeBequadCone(outputcone), 0);
    ptbefig->BEREG = beh_addbereg(ptbefig->BEREG, reg, ptbiabl, yagMakeBinode(ptbiabl, regcone),0);
    ptbefig->BERIN = beh_addberin(ptbefig->BERIN, reg);

    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, "vdd", 'I', 'B');
    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, "vss", 'I', 'B');

    return ptbefig;
}

befig_list *
yagBuildBefigFFT2(name, D_sig, D_cone, D, CPN, LD, RE, reg, Q, QN, conelist)
    char           *name;
    losig_list     *D_sig;
    cone_list      *D_cone;
    char           *D, *CPN, *LD, *RE;
    char           *reg;
    char           *Q, *QN;
    chain_list     *conelist;
{
    befig_list     *ptbefig;
    cone_list      *ptdrivecone;
    biabl_list     *ptbiabl = NULL;
    chain_list     *value, *condition, *trigger;
    ptype_list     *ptuser;
    char            label[16];
    char            extname[YAGBUFSIZE];
    char           *lclname;
    cone_list      *regcone = NULL;
    cone_list      *outputcone = NULL;
    chain_list     *delaycones = NULL;

    ptbefig = beh_addbefig(NULL, name);

    if (conelist != NULL && YAG_CONTEXT->YAG_TAS_TIMING != YAG_NO_TIMING) {
        regcone = (cone_list *)yagGetChainInd(conelist, 7);
        outputcone = (cone_list *)yagGetChainInd(conelist, 8);
        delaycones = addchain(NULL, yagGetChainInd(conelist, 5));
    }

    if (D_sig != NULL) {
        ptuser = getptype(D_sig->USER, YAG_CONE_PTYPE);
        if (ptuser != NULL) ptdrivecone = (cone_list *)ptuser->DATA;
        if (ptuser == NULL || (ptdrivecone->TYPE & YAG_TEMPCONE) != 0) {
            yagAddGlueCone(D_sig, D_cone);
        }
    }

    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, name, 'I', 'B');
    ptbefig->BERIN = beh_addberin(ptbefig->BERIN, name);

    sprintf(label, "lb%d", YAG_CONTEXT->YAG_BIABL_INDEX++);
    value = createAtom(Q);
    condition = createExpr(AND);
/*    addQExpr(condition, notExpr(createAtom(LD)));
    addQExpr(condition, createAtom(RE));*/
    addQExpr(condition, notExpr(createAtom(CPN)));
    trigger = createExpr(STABLE);
    addQExpr(trigger, createAtom(CPN));
    addQExpr(condition, notExpr(trigger));
    ptbiabl = beh_addbiabl(ptbiabl, label, condition, value);

    sprintf(label, "lb%d", YAG_CONTEXT->YAG_BIABL_INDEX++);
    value = createAtom("'1'");
    condition = createExpr(AND);
    addQExpr(condition, notExpr(createAtom(LD)));
    addQExpr(condition, notExpr(createAtom(RE)));
    ptbiabl = beh_addbiabl(ptbiabl, label, condition, value);
    if (ptbiabl->NEXT != NULL) ptbiabl->NEXT->FLAG |= BEH_CND_PRECEDE;

    sprintf(label, "lb%d", YAG_CONTEXT->YAG_BIABL_INDEX++);
    value = notExpr(createAtom(D));
    condition = createAtom(LD);
    ptbiabl = beh_addbiabl(ptbiabl, label, condition, value);
    if (ptbiabl->NEXT != NULL) ptbiabl->NEXT->FLAG |= BEH_CND_PRECEDE;

    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, CPN, 'I', 'B');
    ptbefig->BERIN = beh_addberin(ptbefig->BERIN, CPN);

    sprintf(extname, "%s'delayed", CPN);
    lclname = namealloc(extname);
    ptbefig->BEDLY = beh_addbeaux(ptbefig->BEDLY, lclname, createAtom(CPN), NULL, 0);
    ptbefig->BERIN = beh_addberin(ptbefig->BERIN, lclname);

    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, LD, 'I', 'B');
    ptbefig->BERIN = beh_addberin(ptbefig->BERIN, LD);
    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, RE, 'I', 'B');
    ptbefig->BERIN = beh_addberin(ptbefig->BERIN, RE);
    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, Q, 'O', 'B');
    ptbefig->BEOUT = beh_addbeout(ptbefig->BEOUT, Q, notExpr(createAtom(reg)), (pNode)yagMakeBequadCone(outputcone), 0);
    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, QN, 'O', 'B');
    ptbefig->BEOUT = beh_addbeout(ptbefig->BEOUT, QN, createAtom(reg), NULL, 0);
    ptbefig->BEREG = beh_addbereg(ptbefig->BEREG, reg, ptbiabl, yagMakeBinode(ptbiabl, regcone),0);
    ptbefig->BERIN = beh_addberin(ptbefig->BERIN, reg);

    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, "vdd", 'I', 'B');
    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, "vss", 'I', 'B');

    return ptbefig;
}

befig_list *
yagBuildBefigFD2R(name, D_sig, D_cone, D, CPN, RE, reg, Q, QN, conelist)
    char           *name;
    losig_list     *D_sig;
    cone_list      *D_cone;
    char           *D, *CPN, *RE;
    char           *reg;
    char           *Q, *QN;
    chain_list     *conelist;
{
    befig_list     *ptbefig;
    cone_list      *ptdrivecone;
    biabl_list     *ptbiabl = NULL;
    chain_list     *value, *condition, *trigger;
    ptype_list     *ptuser;
    char            label[16];
    char            extname[YAGBUFSIZE];
    char           *lclname;
    cone_list      *regcone = NULL;
    cone_list      *outputcone = NULL;
    chain_list     *delaycones = NULL;

    ptbefig = beh_addbefig(NULL, name);

    if (conelist != NULL && YAG_CONTEXT->YAG_TAS_TIMING != YAG_NO_TIMING) {
        regcone = (cone_list *)yagGetChainInd(conelist, 3);
        outputcone = (cone_list *)yagGetChainInd(conelist, 4);
        delaycones = addchain(NULL, yagGetChainInd(conelist, 1));
    }

    if (D_sig != NULL) {
        ptuser = getptype(D_sig->USER, YAG_CONE_PTYPE);
        if (ptuser != NULL) ptdrivecone = (cone_list *)ptuser->DATA;
        if (ptuser == NULL || (ptdrivecone->TYPE & YAG_TEMPCONE) != 0) {
            yagAddGlueCone(D_sig, D_cone);
        }
    }

    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, D, 'I', 'B');
    ptbefig->BERIN = beh_addberin(ptbefig->BERIN, D);

    sprintf(label, "lb%d", YAG_CONTEXT->YAG_BIABL_INDEX++);
    value = notExpr(createAtom(D));
    condition = createExpr(AND);
/*    addQExpr(condition, createAtom(RE));*/
    addQExpr(condition, notExpr(createAtom(CPN)));
    trigger = createExpr(STABLE);
    addQExpr(trigger, createAtom(CPN));
    addQExpr(condition, notExpr(trigger));
    ptbiabl = beh_addbiabl(ptbiabl, label, condition, value);

    sprintf(label, "lb%d", YAG_CONTEXT->YAG_BIABL_INDEX++);
    value = createAtom("'1'");
    condition = notExpr(createAtom(RE));
    ptbiabl = beh_addbiabl(ptbiabl, label, condition, value);
    if (ptbiabl->NEXT != NULL) ptbiabl->NEXT->FLAG |= BEH_CND_PRECEDE;

    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, CPN, 'I', 'B');
    ptbefig->BERIN = beh_addberin(ptbefig->BERIN, CPN);

    sprintf(extname, "%s'delayed", CPN);
    lclname = namealloc(extname);
    ptbefig->BEDLY = beh_addbeaux(ptbefig->BEDLY, lclname, createAtom(CPN), NULL, 0);
    ptbefig->BERIN = beh_addberin(ptbefig->BERIN, lclname);

    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, RE, 'I', 'B');
    ptbefig->BERIN = beh_addberin(ptbefig->BERIN, RE);
    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, Q, 'O', 'B');
    ptbefig->BEOUT = beh_addbeout(ptbefig->BEOUT, Q, notExpr(createAtom(reg)), (pNode)yagMakeBequadCone(outputcone), 0);
    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, QN, 'O', 'B');
    ptbefig->BEOUT = beh_addbeout(ptbefig->BEOUT, QN, createAtom(reg), NULL, 0);
    ptbefig->BEREG = beh_addbereg(ptbefig->BEREG, reg, ptbiabl, yagMakeBinode(ptbiabl, regcone),0);

    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, "vdd", 'I', 'B');
    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, "vss", 'I', 'B');

    return ptbefig;
}

befig_list *
yagBuildBefigFD2S(name, D_sig, D_cone, D, CPN, RE, reg, Q, QN, conelist)
    char           *name;
    losig_list     *D_sig;
    cone_list      *D_cone;
    char           *D, *CPN, *RE;
    char           *reg;
    char           *Q, *QN;
    chain_list     *conelist;
{
    befig_list     *ptbefig;
    cone_list      *ptdrivecone;
    biabl_list     *ptbiabl = NULL;
    chain_list     *value, *condition, *trigger;
    ptype_list     *ptuser;
    char            label[16];
    char            extname[YAGBUFSIZE];
    char           *lclname;
    cone_list      *regcone = NULL;
    cone_list      *outputcone = NULL;
    chain_list     *delaycones = NULL;

    ptbefig = beh_addbefig(NULL, name);

    if (conelist != NULL && YAG_CONTEXT->YAG_TAS_TIMING != YAG_NO_TIMING) {
        regcone = (cone_list *)yagGetChainInd(conelist, 3);
        outputcone = (cone_list *)yagGetChainInd(conelist, 4);
        delaycones = addchain(NULL, yagGetChainInd(conelist, 1));
    }

    if (D_sig != NULL) {
        ptuser = getptype(D_sig->USER, YAG_CONE_PTYPE);
        if (ptuser != NULL) ptdrivecone = (cone_list *)ptuser->DATA;
        if (ptuser == NULL || (ptdrivecone->TYPE & YAG_TEMPCONE) != 0) {
            yagAddGlueCone(D_sig, D_cone);
        }
    }

    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, D, 'I', 'B');
    ptbefig->BERIN = beh_addberin(ptbefig->BERIN, D);

    sprintf(label, "lb%d", YAG_CONTEXT->YAG_BIABL_INDEX++);
    value = notExpr(createAtom(D));
    condition = createExpr(AND);
/*    addQExpr(condition, createAtom(RE));*/
    addQExpr(condition, notExpr(createAtom(CPN)));
    trigger = createExpr(STABLE);
    addQExpr(trigger, createAtom(CPN));
    addQExpr(condition, notExpr(trigger));
    ptbiabl = beh_addbiabl(ptbiabl, label, condition, value);

    sprintf(label, "lb%d", YAG_CONTEXT->YAG_BIABL_INDEX++);
    value = createAtom("'0'");
    condition = notExpr(createAtom(RE));
    ptbiabl = beh_addbiabl(ptbiabl, label, condition, value);
    if (ptbiabl->NEXT != NULL) ptbiabl->NEXT->FLAG |= BEH_CND_PRECEDE;

    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, CPN, 'I', 'B');
    ptbefig->BERIN = beh_addberin(ptbefig->BERIN, CPN);

    sprintf(extname, "%s'delayed", CPN);
    lclname = namealloc(extname);
    ptbefig->BEDLY = beh_addbeaux(ptbefig->BEDLY, lclname, createAtom(CPN), NULL, 0);
    ptbefig->BERIN = beh_addberin(ptbefig->BERIN, lclname);

    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, RE, 'I', 'B');
    ptbefig->BERIN = beh_addberin(ptbefig->BERIN, RE);
    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, Q, 'O', 'B');
    ptbefig->BEOUT = beh_addbeout(ptbefig->BEOUT, Q, notExpr(createAtom(reg)), (pNode)yagMakeBequadCone(outputcone), 0);
    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, QN, 'O', 'B');
    ptbefig->BEOUT = beh_addbeout(ptbefig->BEOUT, QN, createAtom(reg), NULL, 0);
    ptbefig->BEREG = beh_addbereg(ptbefig->BEREG, reg, ptbiabl, yagMakeBinode(ptbiabl, regcone),0);
    ptbefig->BERIN = beh_addberin(ptbefig->BERIN, reg);

    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, "vdd", 'I', 'B');
    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, "vss", 'I', 'B');

    return ptbefig;
}


/****************************************************************************/
/*                                                                          */
/*                 Chaine de CAO & VLSI   Alliance                          */
/*                                                                          */
/*   Produit : TMA Version 1                                                */
/*   Fichier : tma_blackbox.c                                               */
/*                                                                          */
/*   (c) copyright 1997-1998 Laboratoire LIP6 equipe ASIM                   */
/*   Tous droits reserves                                                   */
/*   Support : e-mail alliance-support@asim.lip6.fr                         */
/*                                                                          */
/*   Auteur(s) : Gilles AUGUSTINS                                           */
/*                                                                          */
/****************************************************************************/

#include "tma.h"

/****************************************************************************/

typedef struct info_pin {
   ttvevent_list *EVENT;
   long           DELAY;
   long           SLOPE;
   long           R;
   long           S;
   timing_model  *MSDATA;
   timing_model  *MDDATA;
   timing_model  *MDCLOCK;
   timing_model  *MSCLOCK;
   long           CSTR;
   timing_model  *MDCSTR;
   timing_model  *MSCSTR;
} info_pin;

chain_list *DUPMODLIST = NULL;

/****************************************************************************/

char *tma_GenerateName (ttvfig_list *fig, ttvevent_list *ev1, ttvevent_list *ev2, unsigned int type, char minmax)
{
    char itr, otr;
    char output[1024];
    char input[1024];

    if ((ev1->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
        itr = STM_UP;
    else
    if ((ev1->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN)
        itr = STM_DN;

    if ((ev2->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
        otr = STM_UP;
    else
    if ((ev2->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN)
        otr = STM_DN;

    ttv_getsigname (fig, input, ev1->ROOT);
    ttv_getsigname (fig, output, ev2->ROOT);
    
    return stm_generate_name (fig->INFO->FIGNAME, input, itr, output, otr, type, minmax);
}

/****************************************************************************/

void tma_DupConnectorList (ttvfig_list *newfig, ttvfig_list *fig, chain_list *filter)
{
   int               i, cnt=0;
   chain_list       *cchain = NULL, *cl, *ord=NULL;
   ttvsig_list      *sigc, *nsigc;
   ttvsig_list      *ttvsig;
   float             cu,cumin,cumax,cd,cdmin,cdmax;
   float             low, high;
 
   for (i = 0; i < fig->NBCONSIG; i++) {
      sigc   = fig->CONSIG[i];
      for (cl=filter; cl!=NULL; cl=cl->NEXT)
         if (mbk_TestREGEX(sigc->NETNAME, (char *)cl->DATA)) break;
      if (filter==NULL || cl!=NULL)
      {
        cchain = ttv_addrefsig (newfig, sigc->NAME, sigc->NETNAME, sigc->CAPA, sigc->TYPE & ~(TTV_SIG_S|TTV_SIG_Q|TTV_SIG_R|TTV_SIG_L|TTV_SIG_B), cchain);
        ttvsig = ( ttvsig_list *)cchain->DATA;
        if ( ttv_getsigcapas ( sigc, &cu, &cumin, &cumax, &cd, &cdmin, &cdmax)) 
          ttv_addsigcapas ( ttvsig, cu, cumin, cumax, cd, cdmin, cdmax);
        cnt++;
        ord=addchain(ord, sigc);
      }
   }
   
   ord=reverse(ord);
   
   newfig->CONSIG   = ttv_allocreflist (cchain, cnt);
   newfig->NBCONSIG = cnt;
   
   for (i = 0; i < newfig->NBCONSIG; i++, ord=delchain(ord, ord)) {
      sigc   = (ttvsig_list *)ord->DATA;
      nsigc  = newfig->CONSIG[i];
      if (getptype (sigc->USER, TTV_SIG_CLOCK))
         nsigc->USER = addptype (nsigc->USER, TTV_SIG_CLOCK, NULL);
      if (getptype (sigc->USER, TTV_SIG_ASYNCHRON))
         nsigc->USER = addptype (nsigc->USER, TTV_SIG_ASYNCHRON, NULL);
      if (!ttv_get_signal_swing(fig, sigc, &low, &high))
         ttv_set_signal_swing(nsigc, low, high);
   }
}

/****************************************************************************/
static chain_list *VARLIST;

double tma_Leakage_Calculation(cnsfig_list *cnsfig)
{
    cone_list *cone;
    double value = 0.0;
    branch_list *branch;

    for(cone = cnsfig->CONE; cone; cone = cone->NEXT){
//        if(cone->BREXT && (cone->BREXT->LINK->TYPE & (CNS_IN | CNS_INOUT))) continue;
        if(cone->TECTYPE & (CNS_ZERO|CNS_STATE_ZERO)){
            for(branch = cone->BRVDD; branch; branch = branch->NEXT){
                value += tas_get_current_leakage_2( branch, branch->LINK, branch->LINK)
                        *tas_getparam(branch->LINK->ULINK.LOTRS, 'M', TP_VDDmax);
            }
            for(branch = cone->BREXT; branch; branch = branch->NEXT){
                if(branch->LINK->TYPE & (CNS_IN | CNS_INOUT)) continue;
                value += tas_get_current_leakage_2( branch, branch->LINK, branch->LINK)
                        *tas_getparam(branch->LINK->ULINK.LOTRS, 'M', TP_VDDmax);
            }
        }
        else if(cone->TECTYPE & (CNS_ONE|CNS_STATE_ONE)){
            for(branch = cone->BRVSS; branch; branch = branch->NEXT){
                value += tas_get_current_leakage_2( branch, branch->LINK, branch->LINK)
                        *tas_getparam(branch->LINK->ULINK.LOTRS, 'M', TP_VDDmax);
            }
            for(branch = cone->BREXT; branch; branch = branch->NEXT){
                if(branch->LINK->TYPE & (CNS_IN | CNS_INOUT)) continue;
                value += tas_get_current_leakage_2( branch, branch->LINK, branch->LINK)
                        *tas_getparam(branch->LINK->ULINK.LOTRS, 'M', TP_VDDmax);
            }
        }else if((cone->TECTYPE & CNS_STATE_UNKNOWN) == CNS_STATE_UNKNOWN){
            avt_log( LOGTMA, 9, "UNKNOWN:%s\n", cone->NAME);
            value = -1.0;
            break;
        }
    }
    
    return value;
}

chain_list *tma_get_leakage_power_pattern(chain_list *chainlocon)
{
    ptype_list     *ptype;
    chain_list     *chain, *chainexpr;

    if(chainlocon && chainlocon->NEXT){    
        chainexpr = createExpr(AND);
    }
    for(chain = chainlocon; chain; chain = chain->NEXT){
        ptype = getptype(((locon_list*)chain->DATA)->USER, CNS_TYPELOCON);
        if(((long)ptype->DATA & (CNS_INIT_ONE|CNS_ONE)) != 0){
            if(chainlocon && chainlocon->NEXT){
                addQExpr (chainexpr, createAtom(((locon_list*)chain->DATA)->NAME));
            }else{
                chainexpr = createAtom(((locon_list*)chain->DATA)->NAME);
            }
        }else if(((long)ptype->DATA & (CNS_INIT_ZERO|CNS_ZERO)) != 0){
            if(chainlocon && chainlocon->NEXT){
                addQExpr (chainexpr, notExpr(createAtom(((locon_list*)chain->DATA)->NAME)));
            }else{
                chainexpr = notExpr(createAtom(((locon_list*)chain->DATA)->NAME));
            }
        }
    }
    return chainexpr;
}

tma_leak_pow_list *tma_get_tlp(tma_leak_pow_list *tmaleakpow, float value, chain_list *list_locon)
{
    tma_leak_pow_list *tlp;
    pCircuit        circuit;
    int             numinputs;
    pNode           bdd1, bdd2, resbdd;
    chain_list     *chainexpr;
    char           value1[32], value2[32];

    if(value <= 0.0) return tmaleakpow;
    sprintf(value1, "%.2g", value);
    
    for(tlp = tmaleakpow; tlp; tlp = tlp->NEXT){
        sprintf(value2, "%.2g", tlp->VALUE);
        if(!strcmp(value1, value2)){
            
            numinputs = countchain(list_locon);
            circuit = initializeCct(namealloc("circuit"), numinputs, 10);
            addInputCct_no_NA(circuit, ((locon_list*)list_locon->DATA)->NAME);

            chainexpr = tma_get_leakage_power_pattern(list_locon);
            bdd1 = ablToBddCct(circuit, chainexpr);
            freeExpr(chainexpr);
            bdd2 = ablToBddCct(circuit, tlp->PATTERN);
            resbdd = applyBinBdd(OR, bdd1, bdd2);
            freeExpr(tlp->PATTERN);
            tlp->PATTERN = bddToAblCct(circuit, resbdd);
            destroyCct(circuit);
            break;
        }
    }
    if(!tlp){
        tlp = (tma_leak_pow_list*) malloc(sizeof(tma_leak_pow_list));
        tlp->NEXT = tmaleakpow;
        tlp->VALUE = value;
        tlp->PATTERN = tma_get_leakage_power_pattern(list_locon);
        tmaleakpow = tlp;
    }
    return tmaleakpow;
}

static tma_leak_pow_list* enumLocon(tmaleakpow, cnsfig, list_locon, ptcbhseq)
    tma_leak_pow_list *tmaleakpow;
    cnsfig_list    *cnsfig;
    chain_list     *list_locon;
    cbhseq         *ptcbhseq;
{
    ptype_list     *ptype;
    double          value;

    VARLIST = addchain(VARLIST, list_locon->DATA);
    ptype = getptype(((locon_list*)list_locon->DATA)->USER, CNS_TYPELOCON);
    if(!ptype){
        ((locon_list*)list_locon->DATA)->USER = addptype(((locon_list*)list_locon->DATA)->USER, CNS_TYPELOCON, (void*)CNS_INIT_ZERO);
    }else{
        ptype->DATA = (void *)((long)ptype->DATA & ~(CNS_INIT_ZERO|CNS_INIT_ONE));
        ptype->DATA = (void *)((long)ptype->DATA | CNS_INIT_ZERO);
    }

    if (list_locon->NEXT != NULL) {
        tmaleakpow = enumLocon(tmaleakpow, cnsfig, list_locon->NEXT, ptcbhseq);
    }
    else {
        cnsCalcFigState(cnsfig, ptcbhseq);
        value = tma_Leakage_Calculation(cnsfig);
        cnsCleanFigState(cnsfig, ptcbhseq);
        tmaleakpow = tma_get_tlp(tmaleakpow, value, VARLIST);
    }
    VARLIST = delchain(VARLIST, VARLIST);

    VARLIST = addchain(VARLIST, list_locon->DATA);
    ptype = getptype(((locon_list*)list_locon->DATA)->USER, CNS_TYPELOCON);
    if(!ptype){
        ((locon_list*)list_locon->DATA)->USER = addptype(((locon_list*)list_locon->DATA)->USER, CNS_TYPELOCON, (void*)CNS_INIT_ONE);
    }else{
        ptype->DATA = (void *)((long)ptype->DATA & ~(CNS_INIT_ZERO|CNS_INIT_ONE));
        ptype->DATA = (void *)((long)ptype->DATA | CNS_INIT_ONE);
    }

    if (list_locon->NEXT != NULL) {
        tmaleakpow = enumLocon(tmaleakpow, cnsfig, list_locon->NEXT, ptcbhseq);
    }
    else {
        cnsCalcFigState(cnsfig, ptcbhseq);
        value = tma_Leakage_Calculation(cnsfig);
        cnsCleanFigState(cnsfig, ptcbhseq);
        tmaleakpow = tma_get_tlp(tmaleakpow, value, VARLIST);
    }
    VARLIST = delchain(VARLIST, VARLIST);
    ptype = getptype(((locon_list*)list_locon->DATA)->USER, CNS_TYPELOCON);
    if(ptype) ptype->DATA = (void *)((long)ptype->DATA & ~(CNS_INIT_ONE));

    return tmaleakpow;

}

void tma_AddCellLeakagePower (ttvfig_list *newfig, ttvfig_list *fig)
{
   ptype_list  *ptype;
   chain_list  *cl, *chain, *input_locons;
   locon_list  *locon;
   ttvsig_list *ttvsig;
   float low, high, alim, cell_leakage_power = 0.0;
   char *figname = fig->INFO->FIGNAME;
   cnsfig_list *cf;
   const char *where;
   inffig_list *ifl;
   tma_leak_pow_list *tmaleakpow = NULL, *tlp;
   cbhseq *ptcbhseq;
   char *statepin;

   if((( V_INT_TAB[ __AVT_POWER_CALCULATION ].VALUE == 1 ) 
    || ( V_INT_TAB[ __AVT_POWER_CALCULATION ].VALUE == 3 )
    || ( V_INT_TAB[ __AVT_POWER_CALCULATION ].VALUE == 4 ))
    && (getptype(newfig->USER, TTV_FIG_CBHSEQ) || (!getptype(newfig->USER, TTV_FIG_CBHSEQ) && !fig->NBELATCHSIG && !fig->NBILATCHSIG))){
       
       ifl=getloadedinffig(figname);
       if ((cf=getloadedcnsfig(figname))==NULL)
         {
           cnsenv();
           where=filepath (figname,"cns");
           if (where!=NULL)
             {
               avt_log(LOGFILEACCESS, 0, "Loading Cone netlist \"%s\"\n", where);
               cf=getcnsfig(figname, NULL);
               cns_addmultivoltage(ifl, cf);
             }
         }
   
       if((ptype = getptype(newfig->USER, TTV_FIG_CBHSEQ)) != NULL){
           ptcbhseq = (cbhseq*)(ptype->DATA);
           statepin = ptcbhseq->STATEPIN;
       }else{
           ptcbhseq = NULL;
           statepin = NULL;
       }
       
       input_locons = NULL;
       for(locon = cf->LOCON; locon; locon = locon->NEXT){
           if((locon->DIRECTION == 'I') || (locon->DIRECTION == 'T') || (locon->NAME==statepin)){
               if((ptype = getptype(locon->USER, CNS_TYPELOCON)) != NULL){
                   if(((long)ptype->DATA & (CNS_ZERO|CNS_ONE)) == 0){
                       input_locons = addchain(input_locons, locon);
                   }
               }else{
                   input_locons = addchain(input_locons, locon);
               }
           }
       }
           
       VARLIST = NULL;
       if(input_locons){
           tmaleakpow = enumLocon(tmaleakpow, cf, input_locons, ptcbhseq);
           if(tmaleakpow){
               newfig->USER = addptype(newfig->USER, TTV_LEAKAGE_POWER, tmaleakpow);
           }
       }
   
   }

   
   if(( V_INT_TAB[ __AVT_POWER_CALCULATION ].VALUE == 2 ) 
       || ((( V_INT_TAB[ __AVT_POWER_CALCULATION ].VALUE == 1 ) 
       || ( V_INT_TAB[ __AVT_POWER_CALCULATION ].VALUE == 3 )
       || ( V_INT_TAB[ __AVT_POWER_CALCULATION ].VALUE == 4 )) && !tmaleakpow)){
       cl = ttv_getsigbytype_and_netname(fig,NULL,TTV_SIG_TYPEALL,NULL);
       for (chain = cl; chain; chain = chain->NEXT){
           ttvsig=(ttvsig_list *)chain->DATA;
           if((ptype = getptype(ttvsig->USER, TAS_LEAKAGE)) != NULL){
               ttv_get_signal_swing(fig, ttvsig, &low, &high);
               alim = high - low;
               cell_leakage_power += alim * (((leakage_list*)ptype->DATA)->L_UP_MAX + ((leakage_list*)ptype->DATA)->L_DN_MAX) / 2.0;
           }
       }
       newfig->USER = addptype(newfig->USER, TTV_CELL_LEAKAGE_POWER, NULL);
       *(float*)&newfig->USER->DATA = cell_leakage_power;
   }else if(( V_INT_TAB[ __AVT_POWER_CALCULATION ].VALUE == 1 ) 
         || ( V_INT_TAB[ __AVT_POWER_CALCULATION ].VALUE == 3 )
         || ( V_INT_TAB[ __AVT_POWER_CALCULATION ].VALUE == 4 )){
       for (tlp = tmaleakpow; tlp; tlp = tlp->NEXT){
           cell_leakage_power += tlp->VALUE;
       }
       if(tmaleakpow){
           cell_leakage_power /= countchain((chain_list*)tmaleakpow);
           newfig->USER = addptype(newfig->USER, TTV_CELL_LEAKAGE_POWER, NULL);
           *(float*)&newfig->USER->DATA = cell_leakage_power;
       }
   }
}
    
/****************************************************************************/

ttvfig_list *tma_DupTtvFigHeader (char *newfigname, ttvfig_list *fig)
{
   ttvfig_list *newfig;
   lofig_list  *lofig = ttv_getrcxlofig(fig);
   char        *libfile;
   ptype_list  *ptype;

   newfig = ttv_givehead (newfigname, newfigname, NULL);
   ttv_setttvlevel (newfig);
   newfig->INFO->TOOLNAME      = "TMA";
   newfig->INFO->TOOLVERSION   = namealloc(AVT_FULLVERSION);
   newfig->INFO->TECHNONAME    = fig->INFO->TECHNONAME;   
   newfig->INFO->TECHNOVERSION = fig->INFO->TECHNOVERSION;
   newfig->INFO->SLOPE         = fig->INFO->SLOPE;      
   newfig->INFO->CAPAOUT       = fig->INFO->CAPAOUT;      
   newfig->INFO->STHHIGH       = fig->INFO->STHHIGH;
   newfig->INFO->STHLOW        = fig->INFO->STHLOW;
   newfig->INFO->DTH           = fig->INFO->DTH;
   newfig->INFO->TEMP          = fig->INFO->TEMP;
   newfig->INFO->VDD           = fig->INFO->VDD;
   ttv_setttvdate (newfig, TTV_DATE_LOCAL);
   newfig->STATUS |= TTV_STS_TTX;
   if(lofig)
       newfig->INFO->USER = addptype(newfig->INFO->USER, TTV_FIG_LOFIG, lofig);

   if((ptype = getptype(fig->USER, TTV_FIG_CBHSEQ)) != NULL){
       newfig->USER =  addptype(newfig->USER, TTV_FIG_CBHSEQ, ptype->DATA);
   }

   if (V_STR_TAB[__TMA_READFILE].SET){
      libfile = V_STR_TAB[__TMA_READFILE].VALUE;
      if(libfile && strstr(libfile, ".lib"))
         tut_parse(libfile, 1);
      else if(libfile && strstr(libfile, ".tlf"))
         tut_parse(libfile, 2);
      else if(libfile)
         tut_parse(libfile, 3);
   }
   
   tma_AddCellLeakagePower (newfig, fig);

   return newfig;
}

/****************************************************************************/

int tma_IsUp (long type) 
{
   return (type & TTV_NODE_UP) == TTV_NODE_UP;
}

/****************************************************************************/

int tma_IsDown (long type) 
{
   return (type & TTV_NODE_DOWN) == TTV_NODE_DOWN;
}

/****************************************************************************/

void tma_CreateAccess (ttvfig_list  *fig, 
                       char         *nodename, 
                       long          nodetype,
                       char         *rootname, 
                       long          roottype,
                       timing_model *mdmin, 
                       timing_model *mdmax, 
                       timing_model *msmin, 
                       timing_model *msmax, 
                       long          dmin, 
                       long          dmax, 
                       long          smin, 
                       long          smax)
{
   ttvline_list *linet = NULL, 
                *lined = NULL;

   ttvsig_list *node, *root;

   char *mdmaxname = NULL;
   char *mdminname = NULL;
   char *msmaxname = NULL;
   char *msminname = NULL;

   root = ttv_getsigbyname (fig, rootname, TTV_SIG_C);
   node = ttv_getsigbyname (fig, nodename, TTV_SIG_C);

   if (tma_IsUp (roottype) && tma_IsUp (nodetype)) {
      linet = ttv_addline (fig, &root->NODE[1], &node->NODE[1], 
                      dmax, smax, dmin, smin, TTV_LINE_T | TTV_LINE_A);
      lined = ttv_addline (fig, &root->NODE[1], &node->NODE[1], 
                      dmax, smax, dmin, smin, TTV_LINE_D | TTV_LINE_A);
   }
   else
   if (tma_IsUp (roottype) && tma_IsDown (nodetype)) {
      linet = ttv_addline (fig, &root->NODE[1], &node->NODE[0], 
                      dmax, smax, dmin, smin, TTV_LINE_T | TTV_LINE_A);
      lined = ttv_addline (fig, &root->NODE[1], &node->NODE[0], 
                      dmax, smax, dmin, smin, TTV_LINE_D | TTV_LINE_A);
   }
   else
   if (tma_IsDown (roottype) && tma_IsUp (nodetype)) {
      linet = ttv_addline (fig, &root->NODE[0], &node->NODE[1], 
                      dmax, smax, dmin, smin, TTV_LINE_T | TTV_LINE_A);
      lined = ttv_addline (fig, &root->NODE[0], &node->NODE[1], 
                      dmax, smax, dmin, smin, TTV_LINE_D | TTV_LINE_A);
   }
   else
   if (tma_IsDown (roottype) && tma_IsDown (nodetype)) {
      linet = ttv_addline (fig, &root->NODE[0], &node->NODE[0], 
                      dmax, smax, dmin, smin, TTV_LINE_T | TTV_LINE_A);
      lined = ttv_addline (fig, &root->NODE[0], &node->NODE[0], 
                      dmax, smax, dmin, smin, TTV_LINE_D | TTV_LINE_A);
   }
   
   if (mdmax)
      mdmaxname = mdmax->NAME;
   if (mdmin)
      mdminname = mdmin->NAME;
   if (msmax)
      msmaxname = msmax->NAME;
   if (msmin)
      msminname = msmin->NAME;

   ttv_addcaracline (linet, mdmaxname, mdminname, msmaxname, msminname);
   ttv_addcaracline (lined, mdmaxname, mdminname, msmaxname, msminname);

   if (mdmin)
      stm_storemodel (fig->INFO->FIGNAME, mdmin->NAME, mdmin, 0);
   if (mdmax)
      stm_storemodel (fig->INFO->FIGNAME, mdmax->NAME, mdmax, 0);
   if (msmin)
      stm_storemodel (fig->INFO->FIGNAME, msmin->NAME, msmin, 0);
   if (msmax)
      stm_storemodel (fig->INFO->FIGNAME, msmax->NAME, msmax, 0);
}

/****************************************************************************/

void tma_CreatePath (ttvfig_list  *fig, 
                     char         *nodename, 
                     long          nodetype,
                     char         *rootname, 
                     long          roottype,
                     timing_model *mdmin, 
                     timing_model *mdmax, 
                     timing_model *msmin, 
                     timing_model *msmax, 
                     long          dmin, 
                     long          dmax, 
                     long          smin, 
                     long          smax,
                     long          type)
{
   ttvline_list  *linet = NULL, 
                 *lined = NULL;
   ttvsig_list   *signode, 
                 *sigroot;
   ttvevent_list *root, 
                 *node;
   char          *mdmaxname = NULL,
                 *mdminname = NULL,
                 *msmaxname = NULL,
                 *msminname = NULL;
  
   avt_logenterfunction( LOGTMA, 2, "tma_CreatePath()" );

   if (!(sigroot = ttv_getsigbyname (fig, rootname, TTV_SIG_C)))
      sigroot = ttv_getsigbyname (fig, rootname, TTV_SIG_Q);

   signode = ttv_getsigbyname (fig, nodename, TTV_SIG_C);

   if (tma_IsUp (roottype) && tma_IsUp (nodetype)) {
      root = &sigroot->NODE[1];
      node = &signode->NODE[1];
   }
   else
   if (tma_IsUp (roottype) && tma_IsDown (nodetype)) {
      root = &sigroot->NODE[1];
      node = &signode->NODE[0];
   }
   else
   if (tma_IsDown (roottype) && tma_IsUp (nodetype)) {
      node = &signode->NODE[1];
      root = &sigroot->NODE[0];
   }
   else
   if (tma_IsDown (roottype) && tma_IsDown (nodetype)) {
      node = &signode->NODE[0];
      root = &sigroot->NODE[0];
   }
  
   avt_log( LOGTMA, 2, "add a line from %s to %s : dmax=%ld dmin=%ld smax=%ld smin=%ld\n",
                       node->ROOT->NAME, root->ROOT->NAME, dmax, dmin, smax, smin );
                       
   linet = ttv_addline (fig, root, node, dmax, smax, dmin, smin, TTV_LINE_T);
   lined = ttv_addline (fig, root, node, dmax, smax, dmin, smin, TTV_LINE_D);
   linet->TYPE |= type;
   lined->TYPE |= type;

   if (mdmax) {
      mdmaxname = tma_GenerateName (fig, node, root, STM_DELAY, STM_MAX);
      stm_storemodel (fig->INFO->FIGNAME, mdmaxname, stm_mod_duplicate (NULL, mdmax), 0);
   }
   if (mdmin) {
      mdminname = tma_GenerateName (fig, node, root, STM_DELAY, STM_MIN);
      stm_storemodel (fig->INFO->FIGNAME, mdminname, stm_mod_duplicate (NULL, mdmin), 0);
   }
   if (msmax) {
      msmaxname = tma_GenerateName (fig, node, root, STM_SLEW, STM_MAX);
      stm_storemodel (fig->INFO->FIGNAME, msmaxname, stm_mod_duplicate (NULL, msmax), 0);
   }
   if (msmin) {
      msminname = tma_GenerateName (fig, node, root, STM_SLEW, STM_MIN);
      stm_storemodel (fig->INFO->FIGNAME, msminname, stm_mod_duplicate (NULL, msmin), 0);
   }
   
   ttv_addcaracline (linet, mdmaxname, mdminname, msmaxname, msminname);
   ttv_addcaracline (lined, mdmaxname, mdminname, msmaxname, msminname);

   avt_logexitfunction( LOGTMA, 2 );
}


/****************************************************************************/

void tma_CreateSetup (ttvfig_list  *fig, 
                      char         *nodename, 
                      long          nodetype,
                      char         *ckname, 
                      long          cktype,
                      timing_model *cmodel, 
                      long         setup) 
{
   ttvline_list *linet = NULL, 
                *lined = NULL;
   ttvsig_list  *node, 
                *ck;

   ck   = ttv_getsigbyname (fig, ckname, TTV_SIG_C);
   node = ttv_getsigbyname (fig, nodename, TTV_SIG_C);
   
   if (tma_IsUp (cktype) && tma_IsUp (nodetype)) {
      linet = ttv_addline (fig, &ck->NODE[1], &node->NODE[1], 
                      setup, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_T | TTV_LINE_U);
      lined = ttv_addline (fig, &ck->NODE[1], &node->NODE[1], 
                      setup, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_D | TTV_LINE_U);
   }
   else
   if (tma_IsUp (cktype) && tma_IsDown (nodetype)) {
      linet = ttv_addline (fig, &ck->NODE[1], &node->NODE[0], 
                      setup, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_T | TTV_LINE_U);
      lined = ttv_addline (fig, &ck->NODE[1], &node->NODE[0], 
                      setup, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_D | TTV_LINE_U);
   }
   else
   if (tma_IsDown (cktype) && tma_IsUp (nodetype)) {
      linet = ttv_addline (fig, &ck->NODE[0], &node->NODE[1], 
                      setup, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_T | TTV_LINE_U);
      lined = ttv_addline (fig, &ck->NODE[0], &node->NODE[1], 
                      setup, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_D | TTV_LINE_U);
   }
   else
   if (tma_IsDown (cktype) && tma_IsDown (nodetype)) {
      linet = ttv_addline (fig, &ck->NODE[0], &node->NODE[0], 
                      setup, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_T | TTV_LINE_U);
      lined = ttv_addline (fig, &ck->NODE[0], &node->NODE[0], 
                      setup, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_D | TTV_LINE_U);
   }
   

   if (cmodel) {
      cmodel->UMODEL.TABLE->XTYPEBIS = 1;
      ttv_addcaracline (linet, cmodel->NAME, NULL, NULL, NULL);
      ttv_addcaracline (lined, cmodel->NAME, cmodel->NAME, NULL, NULL);
      stm_storemodel (fig->INFO->FIGNAME, cmodel->NAME, cmodel, 0);
   }
}

/****************************************************************************/

void tma_CreateHold (ttvfig_list  *fig, 
                     char         *nodename, 
                     long          nodetype,
                     char         *ckname, 
                     long          cktype,
                     timing_model *cmodel, 
                     long          hold) 
{
   ttvline_list *linet = NULL, 
             *lined = NULL;
   ttvsig_list  *node, 
             *ck;

   ck   = ttv_getsigbyname (fig, ckname, TTV_SIG_C);
   node = ttv_getsigbyname (fig, nodename, TTV_SIG_C);
   
   if (tma_IsUp (cktype) && tma_IsUp (nodetype)) {
      linet = ttv_addline (fig, &ck->NODE[1], &node->NODE[1], 
                      hold, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_T | TTV_LINE_O);
      lined = ttv_addline (fig, &ck->NODE[1], &node->NODE[1], 
                      hold, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_D | TTV_LINE_O);
   }
   else
   if (tma_IsUp (cktype) && tma_IsDown (nodetype)) {
      linet = ttv_addline (fig, &ck->NODE[1], &node->NODE[0], 
                      hold, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_T | TTV_LINE_O);
      lined = ttv_addline (fig, &ck->NODE[1], &node->NODE[0], 
                      hold, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_D | TTV_LINE_O);
   }
   else
   if (tma_IsDown (cktype) && tma_IsUp (nodetype)) {
      linet = ttv_addline (fig, &ck->NODE[0], &node->NODE[1], 
                      hold, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_T | TTV_LINE_O);
      lined = ttv_addline (fig, &ck->NODE[0], &node->NODE[1], 
                      hold, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_D | TTV_LINE_O);
   }
   else
   if (tma_IsDown (cktype) && tma_IsDown (nodetype)) {
      linet = ttv_addline (fig, &ck->NODE[0], &node->NODE[0], 
                      hold, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_T | TTV_LINE_O);
      lined = ttv_addline (fig, &ck->NODE[0], &node->NODE[0], 
                      hold, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_D | TTV_LINE_O);
   }
   

   if (cmodel) {
      cmodel->UMODEL.TABLE->XTYPEBIS = 1;
      ttv_addcaracline (linet, cmodel->NAME, NULL, NULL, NULL);
      ttv_addcaracline (lined, cmodel->NAME, cmodel->NAME, NULL, NULL);
      stm_storemodel (fig->INFO->FIGNAME, cmodel->NAME, cmodel, 0);
   }
}

/****************************************************************************/

ttvpath_list *tma_GetPath (ttvfig_list   *fig, 
                           ttvevent_list *node, 
                           ttvevent_list *root, 
                           long           minmax)
{
   chain_list   mask;
   chain_list  *a_mask;
   ttvsig_list *sig;
   ttvpath_list *pathlist = NULL;
   ttvpath_list *path;
   long         type = TTV_FIND_NOTCLASS;

   if (TMA_CONTEXT->TMA_TYPE_TTVFIG == 'T')
      type |= TTV_FIND_PATH;
   else
   if (TMA_CONTEXT->TMA_TYPE_TTVFIG == 'D')
      type |= TTV_FIND_LINE;
         
   if (!node && root) { 
      a_mask = NULL;
      type  |= minmax | TTV_FIND_SIG | TTV_FIND_CARAC;
      sig    = root->ROOT;
      if (tma_IsUp (root->TYPE))
         type |= TTV_FIND_NOT_DWDW | TTV_FIND_NOT_UPDW;
      if (tma_IsDown (root->TYPE))
         type |= TTV_FIND_NOT_DWUP | TTV_FIND_NOT_UPUP;
   }
   else
   if (node && !root) {
      a_mask = NULL;
      type  |= minmax | TTV_FIND_SIG | TTV_FIND_DUAL | TTV_FIND_CARAC;
      sig    = node->ROOT;
      if (tma_IsUp (node->TYPE))
         type |= TTV_FIND_NOT_DWUP | TTV_FIND_NOT_DWDW;
      if (tma_IsDown (node->TYPE))
         type |= TTV_FIND_NOT_UPDW | TTV_FIND_NOT_UPUP;
   }
   else
   if (node && root) {
      mask.DATA = node->ROOT;
      mask.NEXT = NULL;
      a_mask    = &mask;
      type     |= minmax | TTV_FIND_SIG | TTV_FIND_CARAC;
      sig       = root->ROOT;
      if (tma_IsUp (node->TYPE) && tma_IsUp (root->TYPE))
         type |= TTV_FIND_NOT_UPDW | TTV_FIND_NOT_DWDW | TTV_FIND_NOT_DWUP;
      if (tma_IsUp (node->TYPE) && tma_IsDown (root->TYPE))
         type |= TTV_FIND_NOT_UPUP | TTV_FIND_NOT_DWDW | TTV_FIND_NOT_DWUP;
      if (tma_IsDown (node->TYPE) && tma_IsUp (root->TYPE)) 
         type |= TTV_FIND_NOT_UPDW | TTV_FIND_NOT_DWDW | TTV_FIND_NOT_UPUP;
      if (tma_IsDown (node->TYPE) && tma_IsDown (root->TYPE)) 
         type |= TTV_FIND_NOT_UPDW | TTV_FIND_NOT_UPUP | TTV_FIND_NOT_DWUP;
   }
   else
      return NULL;

   pathlist = ttv_getpathnocross_v2 (fig, NULL, sig, a_mask, TTV_DELAY_MAX, TTV_DELAY_MIN, type,1);

   if (V_BOOL_TAB[__TMA_ALLOW_ACCESS_AS_CLOCKPATH].VALUE){
       if (pathlist && root && ((root->ROOT->TYPE & TTV_SIG_Q) == TTV_SIG_Q)){
           for (path = pathlist; path; path = path->NEXT){
               if(tma_IsClock(path->NODE->ROOT)) break;
           }
           if(!path){
               tma_FreePathList (pathlist);
               pathlist = ttv_getaccessnocross_v2 (fig, NULL, sig, a_mask, TTV_DELAY_MAX, TTV_DELAY_MIN, type,1);
           }
       }
                   
       if (!pathlist && root && ((root->ROOT->TYPE & TTV_SIG_Q) == TTV_SIG_Q))
           pathlist = ttv_getaccessnocross_v2 (fig, NULL, sig, a_mask, TTV_DELAY_MAX, TTV_DELAY_MIN, type,1);
   }
   
   return pathlist;

}

/****************************************************************************/

void tma_ExpanseBreak (ttvfig_list *fig, ttvsig_list *sig) 
{
   if (TMA_CONTEXT->TMA_TYPE_TTVFIG == 'D')
      ttv_expfigsig (fig, sig, sig->ROOT->INFO->LEVEL, fig->INFO->LEVEL, TTV_STS_DUAL, TTV_FILE_DTX); 
   else
   if (TMA_CONTEXT->TMA_TYPE_TTVFIG == 'T')
      ttv_expfigsig (fig, sig, sig->ROOT->INFO->LEVEL, fig->INFO->LEVEL, TTV_STS_DUAL, TTV_FILE_TTX); 
}

/****************************************************************************/

chain_list *tma_AddMaxList (chain_list    *maxchain, 
                            ttvevent_list *event, 
                            long           dmax, 
                            timing_model  *mdclock, 
                            timing_model  *msclock, 
                            timing_model  *mddata, 
                            timing_model  *msdata,
                            long           cstr, 
                            timing_model  *mdcstr,
                            timing_model  *mscstr,
                            long           smax)
{
   chain_list *ch;
   int         exist = 0;
   info_pin   *info;

   avt_logenterfunction( LOGTMA, 2, "tma_AddMaxList()" );
   avt_log( LOGTMA, 2, "%c %s delay=%ld slope=%ld\n", 
                       ( event->TYPE & TTV_NODE_UP ) == TTV_NODE_UP ? 'U' : 'D',
                       event->ROOT->NAME, dmax, smax
          );
   
   for (ch = maxchain; ch; ch = ch->NEXT) {
      info = (info_pin*)ch->DATA;
      if (info->EVENT == event) {
         exist = 1;
         if (dmax > info->DELAY) {
            avt_log( LOGTMA, 2, "previous value replaced\n" );
            info->DELAY   = dmax;
            info->SLOPE   = smax;
            info->MSDATA  = msdata;
            info->MDCLOCK = mdclock;
            info->MSCLOCK = msclock;
            info->MDDATA  = mddata;
            info->CSTR    = cstr;
            info->MDCSTR   = mdcstr;
            info->MSCSTR   = mscstr;
         }
         else
            avt_log( LOGTMA, 2, "previous value keept\n" );
      }
   }
   
   if (!exist) {
     avt_log( LOGTMA, 2, "new value added\n" );
      info = (info_pin*)mbkalloc (sizeof (struct info_pin));
      info->EVENT   = event;
      info->DELAY   = dmax;
      info->SLOPE   = smax;
      info->MSDATA  = msdata;
      info->MDCLOCK = mdclock;
      info->MSCLOCK = msclock;
      info->MDDATA  = mddata;
      info->CSTR    = cstr;
      info->MDCSTR   = mdcstr;
      info->MSCSTR   = mscstr;
      avt_logexitfunction( LOGTMA, 2 );
      return addchain (maxchain, info);
   }

   avt_logexitfunction( LOGTMA, 2 );
   return maxchain;
}

/****************************************************************************/

chain_list *tma_AddMinList (chain_list    *minchain, 
                            ttvevent_list *event, 
                            long           dmin, 
                            timing_model  *mdclock, 
                            timing_model  *msclock, 
                            timing_model  *mddata, 
                            timing_model  *msdata,
                            long           cstr, 
                            timing_model  *mdcstr,
                            timing_model  *mscstr,
                            long           smin)
{
   chain_list *ch;
   int         exist = 0;
   info_pin   *info;

   avt_logenterfunction( LOGTMA, 2, "tma_AddMinList()" );
   avt_log( LOGTMA, 2, "%c %s delay=%ld slope=%ld\n", 
                       ( event->TYPE & TTV_NODE_UP ) == TTV_NODE_UP ? 'U' : 'D',
                       event->ROOT->NAME, dmin, smin
          );

   for (ch = minchain; ch; ch = ch->NEXT) {
      info = (info_pin*)ch->DATA;
      if (info->EVENT == event) {
         exist = 1;
         if (dmin < info->DELAY) {
            avt_log( LOGTMA, 2, "previous value replaced\n" );
            info->DELAY   = dmin;
            info->SLOPE   = smin;
            info->MDCLOCK = mdclock;
            info->MSCLOCK = msclock;
            info->MDDATA  = mddata;
            info->MSDATA  = msdata;
            info->CSTR    = cstr;
            info->MDCSTR   = mdcstr;
            info->MSCSTR   = mscstr;
         }
         else
            avt_log( LOGTMA, 2, "previous value keept\n" );
      }
   }
   
   if (!exist) {
      avt_log( LOGTMA, 2, "new value added\n" );
      info = (info_pin*)mbkalloc (sizeof (struct info_pin));
      info->EVENT   = event;
      info->DELAY   = dmin;
      info->SLOPE   = smin;
      info->MDCLOCK = mdclock;
      info->MSCLOCK = msclock;
      info->MDDATA  = mddata;
      info->MSDATA  = msdata;
      info->CSTR    = cstr;
      info->MDCSTR   = mdcstr;
      info->MSCSTR   = mscstr;
      avt_logexitfunction( LOGTMA, 2 );
      return addchain (minchain, info);
   }

   avt_logexitfunction( LOGTMA, 2 );

   return minchain;
}

/****************************************************************************/

void tma_FreeList (chain_list *chain)
{
   chain_list *ch;

   for (ch = chain; ch; ch = ch->NEXT)
      mbkfree(ch->DATA);
   if (chain)
      freechain (chain);
}

/****************************************************************************/

void tma_ConvSList (ttvfig_list   *ofig, ttvfig_list   *fig, ttvevent_list *node, chain_list    *setuplist)
{
    chain_list   *setup;
    char          ckname[1024];
    char          ndname[1024];
    char         *mname = NULL;
    timing_model *mddata,
                 *msdata,
                 *mdclock,
                 *msclock,
                 *mdcstr,
                 *mscstr,
                 *n_model,
                 *msetup = NULL;
    float         cstr;
    info_pin     *info;
    ttvsig_list  *sig;


    ttv_getsigname (ofig, ndname, node->ROOT);
    if (setuplist)
        if ((sig = ttv_getsigbyname (fig, ndname, TTV_SIG_C)))
            sig->TYPE |= TTV_SIG_B;
    for (setup = setuplist; setup; setup = setup->NEXT)  {
        info = (info_pin*)setup->DATA;
        ttv_getsigname (ofig, ckname, info->EVENT->ROOT);
        mddata  = info->MDDATA;
        msdata  = info->MSDATA;
        mdclock = info->MDCLOCK;
        msclock = info->MSCLOCK;
        mdcstr  = info->MDCSTR;
        mscstr  = info->MSCSTR;
        cstr    = info->CSTR/TTV_UNIT + V_INT_TAB[__TMA_MARGIN].VALUE;

        mname = tma_GenerateName (ofig, node, info->EVENT, STM_SETUP | STM_DELAY, STM_MAX);

        if (!mddata && mdclock) {
            if (mdcstr)
                msetup = stm_mod_mergecc (mname, n_model = stm_mod_neg (mdclock), msclock, mdcstr, 0, -1);
            else
                msetup = stm_mod_mergecc (mname, n_model = stm_mod_neg (mdclock), msclock, NULL, cstr, -1);
            stm_mod_destroy (n_model);
        } else
        if (mddata && !mdclock) {
            if (mdcstr)
                msetup = stm_mod_mergecd (mname, mddata, msdata, mdcstr, 0, -1);
            else
                msetup = stm_mod_mergecd (mname, mddata, msdata, NULL, cstr, -1);
        }
        else
        if (!mddata && !mdclock) {
            if (mdcstr)
                msetup = stm_mod_duplicate (mname, mdcstr);
            else
                msetup = NULL;
        }
        if (mddata && mdclock) {
            msetup = stm_mod_mergec (mname, mddata, n_model = stm_mod_neg (mdclock), cstr, -1, -1);
            stm_mod_destroy (n_model);
        } 
      
        if ((sig = ttv_getsigbyname (fig, ckname, TTV_SIG_C)))
            sig->TYPE |= TTV_SIG_B;
      
        tma_CreateSetup (fig, ndname, node->TYPE, ckname, info->EVENT->TYPE, 
                       msetup, info->DELAY + V_INT_TAB[__TMA_MARGIN].VALUE);
    }
}

/****************************************************************************/

void tma_ConvHList (ttvfig_list   *ofig, 
                    ttvfig_list   *fig, 
                    ttvevent_list *node, 
                    chain_list   *holdlist)
{
    chain_list   *hold;
    char          ckname[1024];
    char          ndname[1024];
    char         *mname = NULL;
    timing_model *mddata,
                 *msdata,
                 *mdcstr,
                 *mscstr,
                 *mdclock,
                 *msclock,
                 *n_model,
                 *mhold = NULL;
    float         cstr;
    info_pin     *info;
    ttvsig_list  *sig;

    ttv_getsigname (ofig, ndname, node->ROOT);
    if (holdlist)
        if ((sig = ttv_getsigbyname (fig, ndname, TTV_SIG_C)))
            sig->TYPE |= TTV_SIG_B;
    for (hold = holdlist; hold; hold = hold->NEXT)  {
        info = (info_pin*)hold->DATA;
        ttv_getsigname (ofig, ckname, info->EVENT->ROOT);
        mddata  = info->MDDATA;
        msdata  = info->MSDATA;
        mdclock = info->MDCLOCK;
        msclock = info->MSCLOCK;
        mdcstr = info->MDCSTR;
        mscstr = info->MSCSTR;
        cstr   = info->CSTR/TTV_UNIT + V_INT_TAB[__TMA_MARGIN].VALUE;

        mname = tma_GenerateName (ofig, node, info->EVENT, STM_DELAY | STM_HOLD, STM_MAX);
      
        if (!mddata && mdclock) {
            if (mdcstr)
                mhold = stm_mod_mergecc (mname, mdclock, msclock, mdcstr, 0, -1);
            else
                mhold = stm_mod_mergecc (mname, mdclock, msclock, NULL, cstr, -1);
        } else
        if (mddata && !mdclock) {
            if (mdcstr)
                mhold = stm_mod_mergecd (mname, n_model = stm_mod_neg (mddata), msdata, mdcstr, 0, -1);
            else
                mhold = stm_mod_mergecd (mname, n_model = stm_mod_neg (mddata), msdata, NULL, cstr, -1);
            stm_mod_destroy (n_model);
        }
        else
        if (!mddata && !mdclock) {
            if (mdcstr)
                mhold = stm_mod_duplicate (mname, mdcstr);
            else
                mhold = NULL;
        }
        if (mddata && mdclock) {
            mhold = stm_mod_mergec (mname, n_model = stm_mod_neg (mddata), mdclock, cstr, -1, -1);
            stm_mod_destroy (n_model);
        } 
      
        if ((sig = ttv_getsigbyname (fig, ckname, TTV_SIG_C)))
            sig->TYPE |= TTV_SIG_B;
      
        tma_CreateHold (fig, ndname, node->TYPE, ckname, info->EVENT->TYPE, 
                      mhold, info->DELAY + V_INT_TAB[__TMA_MARGIN].VALUE);
    }
}

/****************************************************************************/

chain_list *tma_UpdateHList (chain_list   *holdlist, 
                             chain_list   *maxcklist, 
                             long          dmin, 
                             timing_model *mdmin, 
                             timing_model *msmin, 
                             long          hold,
                             timing_model *hmodel)
{
   chain_list *ck;
   info_pin   *info;

   for (ck = maxcklist; ck; ck = ck->NEXT) {
      info = (info_pin*)ck->DATA;
      holdlist = tma_AddMaxList (holdlist, info->EVENT, info->DELAY - dmin + hold, 
                                 info->MDCLOCK, info->MSCLOCK, mdmin, msmin, hold, hmodel, NULL, 0);
   }

   return holdlist;
}

/****************************************************************************/

chain_list *tma_UpdateSList (chain_list   *setuplist, 
                                 chain_list   *mincklist, 
                                 long          dmax, 
                                 timing_model *mdmax, 
                                 timing_model *msmax, 
                                 long          setup,
                                 timing_model *smodel)
{
   chain_list *ck;
   info_pin   *info;

   for (ck = mincklist; ck; ck = ck->NEXT) {
      info = (info_pin*)ck->DATA;
      setuplist = tma_AddMaxList (setuplist, info->EVENT, dmax - info->DELAY + setup, 
                                  info->MDCLOCK, info->MSCLOCK, mdmax, msmax, setup, smodel, NULL, 0);
   }

   return setuplist;
}

/****************************************************************************/

int tma_IsLatchCell (ttvfig_list *fig)
{
   if (fig->NBELATCHSIG == 1 && !fig->NBILATCHSIG)
      if ((((*fig->ELATCHSIG)->TYPE & TTV_SIG_LL) == TTV_SIG_LL)
         && !(((*fig->ELATCHSIG)->TYPE & TTV_SIG_LR) == TTV_SIG_LR 
           || ((*fig->ELATCHSIG)->TYPE & TTV_SIG_LS) == TTV_SIG_LS))
         return 1;
   return 0;
}
   
/****************************************************************************/

int tma_IsFlipFlopCell (ttvfig_list *fig)
{
   if (fig->NBELATCHSIG == 1 && !fig->NBILATCHSIG)
      if (((*fig->ELATCHSIG)->TYPE & TTV_SIG_LF) == TTV_SIG_LF)
         return 1;
   return 0;
}
   
/****************************************************************************/

void tma_ConvAList  (ttvfig_list   *ofig, 
                     ttvfig_list   *fig, 
                     ttvevent_list *node,
                     chain_list    *aminlist, 
                     chain_list    *amaxlist)
{
    chain_list   *amax, *amin;
    info_pin     *infomin, *infomax;
    char          ckname[1024];
    char          ndname[1024];
    long          damin, damax;
    long          smin, smax;
    float         cstr;
    long          rmin, rmax;
    long          hmin, hmax;
    char         *mdaminname = NULL,
                 *mdamaxname = NULL,
                 *newmsminname = NULL,
                 *newmsmaxname = NULL;
    timing_model *mdamin = NULL,
                 *mdamax = NULL,
                 *newmsmin = NULL,
                 *newmsmax = NULL,
                 *msdata_min,
                 *msdata_max,
                 *mddata_min,
                 *mddata_max,
                 *mscstr_max,
                 *mscstr_min,
                 *mdcstr_max,
                 *mdcstr_min,
                 *msclock_min,
                 *mdclock_min,
                 *msclock_max,
                 *mdclock_max;
    int           flipflop;
    int           latch;
    ttvsig_list  *sig;
    ht *htslope_axis = NULL;
    ht *htcapa_axis = NULL;
    ptype_list *ptype;

    if ((ptype = getptype(ofig->USER, TTV_FIG_SAXIS)) != NULL)
        htslope_axis = (ht*)ptype->DATA; 
    if ((ptype = getptype(ofig->USER, TTV_FIG_CAXIS)) != NULL)
        htcapa_axis = (ht*)ptype->DATA; 

    flipflop = tma_IsFlipFlopCell (ofig);
    latch   = tma_IsLatchCell (ofig);
   
    ttv_getsigname (ofig, ndname, node->ROOT);
    if ((sig = ttv_getsigbyname (fig, ndname, TTV_SIG_C))) {
        if (latch)
            sig->TYPE |= TTV_SIG_LL;
        if (flipflop)
            sig->TYPE |= TTV_SIG_LF;
    }
    for (amax = amaxlist; amax; amax = amax->NEXT) {
      
        infomax = (info_pin*)amax->DATA;
        ttv_getsigname (ofig, ckname, infomax->EVENT->ROOT);
        stm_addaxisvalues(htslope_axis, htcapa_axis, namealloc(ckname), namealloc(ndname));
        damax       = infomax->DELAY;
        smax        = infomax->SLOPE;
        rmax        = infomax->R;
        hmax        = infomax->S;
        msdata_max  = infomax->MSDATA;
        mddata_max  = infomax->MDDATA;
        mdclock_max = infomax->MDCLOCK;
        msclock_max = infomax->MSCLOCK;
        mdcstr_max  = stm_mod_reduce(NULL, infomax->MDCSTR, -1, -1, STM_COPY, STM_DELAY);
        mscstr_max  = stm_mod_reduce(NULL, infomax->MSCSTR, -1, -1, STM_COPY, STM_SLEW);
        cstr        = infomax->CSTR/TTV_UNIT;

        mdamaxname    = NULL;
        newmsmaxname  = NULL;
        mdamax        = NULL;
        newmsmax      = NULL;

       
        mdamaxname    = tma_GenerateName (ofig, infomax->EVENT, node, STM_ACCESS | STM_DELAY, STM_MAX);
        newmsmaxname  = tma_GenerateName (ofig, infomax->EVENT, node, STM_ACCESS | STM_SLEW, STM_MAX);
        
        
        if (!mdclock_max && !mddata_max) {
            mdamax = stm_mod_duplicate (mdamaxname, mdcstr_max);
            newmsmax = stm_mod_duplicate (newmsmaxname, mscstr_max);
        }
        else
        if (mdclock_max && !mddata_max) {
            if (mdcstr_max && mscstr_max)
                mdamax = stm_mod_merge (mdamaxname, mdclock_max, mdcstr_max, 0, -1, -1);
            else
                mdamax = stm_mod_merge (mdamaxname, mdclock_max, NULL, cstr, -1, -1);
            newmsmax = stm_mod_duplicate (newmsmaxname, mscstr_max);
        }
        else
        if (!mdclock_max && mddata_max) {
            if (mdcstr_max && mscstr_max)
                mdamax = stm_mod_merge (mdamaxname, mdcstr_max, mddata_max, 0, -1, -1);
            else
                mdamax = stm_mod_merge (mdamaxname, NULL, mddata_max, cstr, -1, -1);
            newmsmax = stm_mod_duplicate (newmsmaxname, msdata_max);
        }
        else
        if (mdclock_max && mddata_max) {
            mdamax = stm_mod_merge (mdamaxname, mdclock_max, mddata_max, cstr, -1, -1);
            newmsmax = stm_mod_duplicate (newmsmaxname, msdata_max);
        }
      
        stm_mod_destroy(mscstr_max);
        stm_mod_destroy(mdcstr_max);

        damin        = TTV_NOTIME;
        mdamin       = NULL;
        smin         = TTV_NOSLOPE;
        msdata_min   = NULL;
        rmin         = TTV_NORES;
        hmin         = TTV_NORES;
        mdaminname   = NULL; 
        newmsminname = NULL;
        newmsmin     = NULL;
      
        for (amin = aminlist; amin; amin = amin->NEXT) {
            infomin = (info_pin*)amin->DATA;
            if (infomin->EVENT == infomax->EVENT) {
                damin       = infomin->DELAY;
                smin        = infomin->SLOPE;
                rmin        = infomin->R;
                hmin        = infomin->S;
                msdata_min  = infomin->MSDATA;
                mddata_min  = infomin->MDDATA;
                msclock_min = infomin->MSCLOCK;
                mdclock_min = infomin->MDCLOCK;
                mdcstr_min  = stm_mod_reduce(NULL, infomin->MDCSTR, -1, -1, STM_COPY, STM_DELAY);
                mscstr_min  = stm_mod_reduce(NULL, infomin->MSCSTR, -1, -1, STM_COPY, STM_SLEW);
                cstr        = infomin->CSTR / TTV_UNIT;

                mdaminname   = tma_GenerateName (ofig, infomin->EVENT, node, STM_ACCESS | STM_DELAY, STM_MIN);
                newmsminname = tma_GenerateName (ofig, infomin->EVENT, node, STM_ACCESS | STM_SLEW, STM_MIN);
        
                if (!mdclock_min && !mddata_min) {
                    mdamin = stm_mod_duplicate (mdaminname, mdcstr_min);
                    newmsmin = stm_mod_duplicate (newmsminname, mscstr_min);
                }
                else
                if (mdclock_min && !mddata_min) {
                    if (mdcstr_min && mscstr_min)
                        mdamin = stm_mod_merge (mdaminname, mdclock_min, mdcstr_min, 0, -1, -1);
                    else
                        mdamin = stm_mod_merge (mdaminname, mdclock_min, NULL, cstr, -1, -1);
                    newmsmin = stm_mod_duplicate (newmsminname, mscstr_min);
                }
                else
                if (!mdclock_min && mddata_min) {
                    if (mdcstr_min && mscstr_min)
                        mdamin = stm_mod_merge (mdaminname, mdcstr_min, mddata_min, 0, -1, -1);
                    else
                        mdamin = stm_mod_merge (mdaminname, NULL, mddata_min, cstr, -1, -1);
                    newmsmin = stm_mod_duplicate (newmsminname, msdata_min);
                }
                else
                if (mdclock_min && mddata_min) {
                    mdamin = stm_mod_merge (mdaminname, mdclock_min, mddata_min, cstr, -1, -1);
                    newmsmin = stm_mod_duplicate (newmsminname, msdata_min);
                }
                stm_mod_destroy(mscstr_min);
                stm_mod_destroy(mdcstr_min);

                break;
            }
        }
      
        stm_delaxisvalues();
        if ((sig = ttv_getsigbyname (fig, ckname, TTV_SIG_C)))
            sig->TYPE |= TTV_SIG_B;
      
        tma_CreateAccess (fig, ckname, infomax->EVENT->TYPE, ndname, node->TYPE, 
            mdamin, mdamax, newmsmin, newmsmax, damin, damax, smin, smax);
    }
}

/****************************************************************************/

int tma_IsAccessEvent (ttvevent_list *event)
{
   ttvline_list *line;

   if (TMA_CONTEXT->TMA_TYPE_TTVFIG == 'T') {
      for (line = event->INPATH; line; line = line->NEXT) 
         if ((line->TYPE & TTV_LINE_A) == TTV_LINE_A)
            return 1;
   } 
   else
   if (TMA_CONTEXT->TMA_TYPE_TTVFIG == 'D') {
      for (line = event->INLINE; line; line = line->NEXT) 
         if ((line->TYPE & TTV_LINE_A) == TTV_LINE_A)
            return 1;
   }

   return 0;
}

/****************************************************************************/

int tma_IsHoldEvent (ttvevent_list *event)
{
   ttvline_list *line;

   if (TMA_CONTEXT->TMA_TYPE_TTVFIG == 'T') {
      for (line = event->INPATH; line; line = line->NEXT) 
         if ((line->TYPE & TTV_LINE_O) == TTV_LINE_O)
            return 1;
   } 
   else
   if (TMA_CONTEXT->TMA_TYPE_TTVFIG == 'D') {
      for (line = event->INLINE; line; line = line->NEXT) 
         if ((line->TYPE & TTV_LINE_O) == TTV_LINE_O)
            return 1;
   }

   return 0;
}

/****************************************************************************/

int tma_IsSetupEvent (ttvevent_list *event)
{
   ttvline_list *line;

   if (TMA_CONTEXT->TMA_TYPE_TTVFIG == 'T') {
      for (line = event->INPATH; line; line = line->NEXT) 
         if ((line->TYPE & TTV_LINE_U) == TTV_LINE_U)
            return 1;
   } 
   else
   if (TMA_CONTEXT->TMA_TYPE_TTVFIG == 'D') {
      for (line = event->INLINE; line; line = line->NEXT) 
         if ((line->TYPE & TTV_LINE_U) == TTV_LINE_U)
            return 1;
   }

   return 0;
}

/****************************************************************************/

int tma_IsBreakCmd (ttvevent_list *event)
{
   ttvline_list *line;
                  
   if (TMA_CONTEXT->TMA_TYPE_TTVFIG == 'T') {
      for (line = event->INPATH; line; line = line->NEXT) 
         if ((line->TYPE & TTV_LINE_O) == TTV_LINE_O || (line->TYPE & TTV_LINE_U) == TTV_LINE_U)
            return 1;
   } 
   else
   if (TMA_CONTEXT->TMA_TYPE_TTVFIG == 'D') {
      for (line = event->INLINE; line; line = line->NEXT) 
         if ((line->TYPE & TTV_LINE_O) == TTV_LINE_O || (line->TYPE & TTV_LINE_U) == TTV_LINE_U)
            return 1;
   }

   return 0;
}

/****************************************************************************/

ttvevent_list *tma_OppositeEvent (ttvevent_list *event)
{
   if (!event)
      return NULL;

   if (tma_IsUp (event->TYPE))
      return (&event->ROOT->NODE[0]);
   else
      return (&event->ROOT->NODE[1]);
}

/****************************************************************************/

void tma_AddLatchSig (ttvfig_list *fig)
{
   ttvsig_list *sig;
   int          i, 
                nblatchsig   = 0;
   chain_list  *latchsigchain = NULL;
   
   for (i = 0; i < fig->NBCONSIG; i++) {
      sig = fig->CONSIG[i];
      if ((sig->TYPE & TTV_SIG_LL) == TTV_SIG_LL) {
         latchsigchain = addchain (latchsigchain, sig);
         nblatchsig++;
      }
      else
      if ((sig->TYPE & TTV_SIG_LF) == TTV_SIG_LF) {
         latchsigchain = addchain (latchsigchain, sig);
         nblatchsig++;
      }
   }

   fig->NBELATCHSIG = nblatchsig;
   fig->ELATCHSIG   = ttv_allocreflist (latchsigchain, nblatchsig);
}

/****************************************************************************/

void tma_AddBreakSig (ttvfig_list *fig) 
{
   ttvsig_list *sig;
   int          i, 
                nbbreaksig   = 0;
   chain_list  *breaksigchain = NULL;
   
   for (i = 0; i < fig->NBCONSIG; i++) {
      sig = fig->CONSIG[i];
      if ((sig->TYPE & TTV_SIG_B) == TTV_SIG_B) {
         breaksigchain = addchain (breaksigchain,sig);
         nbbreaksig++;
      }
   }

   fig->NBEBREAKSIG = nbbreaksig;
   fig->EBREAKSIG   = ttv_allocreflist (breaksigchain, nbbreaksig);
}

/****************************************************************************/

void tma_FreeModelList ()
{
   chain_list *ch;
   for (ch = DUPMODLIST; ch; ch = ch->NEXT)
      if (ch->DATA)
         stm_mod_destroy ((timing_model*)ch->DATA);
   if (DUPMODLIST)
      freechain (DUPMODLIST);
}

/****************************************************************************/

void tma_AddModelList (timing_model *model)
{
   if (model)
      DUPMODLIST = addchain (DUPMODLIST, model);
}

/****************************************************************************/

void tma_InitModelList ()
{
   DUPMODLIST = NULL;
}

/****************************************************************************/

int tma_IsAsynchronous (ttvsig_list *sig)
{
   if (getptype (sig->USER, TTV_SIG_ASYNCHRON))
      return 1;
   else
      return 0;
}

/****************************************************************************/

int tma_IsClock (ttvsig_list *sig)
{
   if (getptype (sig->USER, TTV_SIG_CLOCK))
      return 1;
   else
      return 0;
}

/****************************************************************************/

int tma_IsEvLine (ttvevent_list *node, ttvevent_list *root)
{
   ttvline_list *line;
   
   if (TMA_CONTEXT->TMA_TYPE_TTVFIG == 'T') {
      for (line = root->INPATH; line; line = line->NEXT)
         if (line->NODE == node)
            if ((line->TYPE & TTV_LINE_EV) == TTV_LINE_EV)
               return 1;
      
   }
   else
   if (TMA_CONTEXT->TMA_TYPE_TTVFIG == 'D') {
      for (line = root->INLINE; line; line = line->NEXT)
         if (line->NODE == node)
            if ((line->TYPE & TTV_LINE_EV) == TTV_LINE_EV)
               return 1;
   }

   return 0;
}

/****************************************************************************/

int tma_IsPrLine (ttvevent_list *node, ttvevent_list *root)
{
   ttvline_list *line;
   
   if (TMA_CONTEXT->TMA_TYPE_TTVFIG == 'T') {
      for (line = root->INPATH; line; line = line->NEXT)
         if (line->NODE == node)
            if ((line->TYPE & TTV_LINE_PR) == TTV_LINE_PR)
               return 1;
   }
   else
   if (TMA_CONTEXT->TMA_TYPE_TTVFIG == 'D') {
      for (line = root->INLINE; line; line = line->NEXT)
         if (line->NODE == node)
            if ((line->TYPE & TTV_LINE_PR) == TTV_LINE_PR)
               return 1;
   }

   return 0;
}

/****************************************************************************/

chain_list *tma_AddHList (chain_list    *holdlist, 
                          long           hold, 
                          timing_model  *hmodel,
                          long           dmin, 
                          timing_model  *mdmin, 
                          timing_model  *msmin, 
                          ttvpath_list  *cklist,
                          ttvevent_list *event) 
{
   chain_list   *maxcklist = NULL;
   ttvpath_list *ck;
   timing_model *mdck;
   timing_model *msck;
   
   if (!cklist && event){
      if (tma_IsClock (event->ROOT)) {
          maxcklist = tma_AddMaxList (maxcklist, event, 0, NULL, NULL, NULL, NULL, 0, NULL, NULL, 0);
      }
   }else{
      for (ck = cklist; ck; ck = ck->NEXT)
         if (tma_IsClock (ck->NODE->ROOT)) {
            tma_AddModelList (mdck = stm_mod_duplicate (NULL, ck->MD));
            tma_AddModelList (msck = stm_mod_duplicate (NULL, ck->MF));
            maxcklist = tma_AddMaxList (maxcklist, ck->NODE, ck->DELAY, mdck, msck, NULL, NULL, 0, NULL, NULL, 0);
         }
   }
   holdlist = tma_UpdateHList (holdlist, maxcklist, dmin, mdmin, msmin, hold, hmodel);
   tma_FreeList (maxcklist);
   return holdlist;
}

/****************************************************************************/

chain_list *tma_AddSList (chain_list    *setuplist, 
                          long           setup, 
                          timing_model  *smodel,
                          long           dmax, 
                          timing_model  *mdmax, 
                          timing_model  *msmax, 
                          ttvpath_list  *cklist,
                          ttvevent_list *event) 
{
   chain_list   *mincklist = NULL;
   ttvpath_list *ck;
   timing_model *mdck;
   timing_model *msck;
   
   if (!cklist && event){
      if (tma_IsClock (event->ROOT)) {
          mincklist = tma_AddMinList (mincklist, event, 0, NULL, NULL, NULL, NULL, 0, NULL, NULL, 0);
      }
   }else{
      for (ck = cklist; ck; ck = ck->NEXT)
         if (tma_IsClock (ck->NODE->ROOT)) {
            tma_AddModelList (mdck = stm_mod_duplicate (NULL, ck->MD));
            tma_AddModelList (msck = stm_mod_duplicate (NULL, ck->MF));
            mincklist = tma_AddMinList (mincklist, ck->NODE, ck->DELAY, mdck, msck, NULL, NULL, 0, NULL, NULL, 0);
         }
   }
   setuplist = tma_UpdateSList (setuplist, mincklist, dmax, mdmax, msmax, setup, smodel);
   tma_FreeList (mincklist);
   return setuplist;
}

/****************************************************************************/

chain_list *tma_AddAMaxList (chain_list    *accesslist, 
                             long           access, 
                             timing_model  *mdaccess, 
                             timing_model  *msaccess, 
                             long           d, 
                             timing_model  *md, 
                             long           s, 
                             timing_model  *ms, 
                             ttvpath_list  *cklist,
                             ttvevent_list *event) 
{
   ttvpath_list *ck;
   timing_model *mdck;
   timing_model *msck;
   long          da;

   if (!cklist && event) {
      if (tma_IsClock (event->ROOT) || tma_IsAsynchronous (event->ROOT)) {
         da = d + access;
         accesslist = tma_AddMaxList (accesslist, event, da, NULL, NULL, md, ms, access, mdaccess, msaccess, s);
      }
   }else{
      for (ck = cklist; ck; ck = ck->NEXT) 
         if (tma_IsClock (ck->NODE->ROOT) || tma_IsAsynchronous (ck->NODE->ROOT)) {
            da =  ck->DELAY + d + access;
            if (ck->MD && ck->MF) {
                tma_AddModelList (mdck = stm_mod_duplicate (NULL, ck->MD));
                tma_AddModelList (msck = stm_mod_duplicate (NULL, ck->MF));
                accesslist = tma_AddMaxList (accesslist, ck->NODE, da, mdck, msck, md, ms, access, mdaccess, msaccess, s);
            }
            else
                accesslist = tma_AddMaxList (accesslist, ck->NODE, da, NULL, NULL, md, ms, access + ck->DELAY, mdaccess, msaccess, s);
         }
   }
   return accesslist;
}

/****************************************************************************/

chain_list *tma_AddAMinList (chain_list    *accesslist, 
                             long           access, 
                             timing_model  *mdaccess, 
                             timing_model  *msaccess, 
                             long           d, 
                             timing_model  *md, 
                             long           s, 
                             timing_model  *ms, 
                             ttvpath_list  *cklist,
                             ttvevent_list *event) 
{
   ttvpath_list *ck;
   timing_model *msck;
   timing_model *mdck;
   long          da;

   if (!cklist && event) {
      if (tma_IsClock (event->ROOT) || tma_IsAsynchronous (event->ROOT)) {
         da = d + access;
         accesslist = tma_AddMinList (accesslist, event, da, NULL, NULL, md, ms, access, mdaccess, msaccess, s);
      }
   }else{
      for (ck = cklist; ck; ck = ck->NEXT) 
         if (tma_IsClock (ck->NODE->ROOT) || tma_IsAsynchronous (ck->NODE->ROOT)) {
            da =  ck->DELAY + d + access;
            if (ck->MD && ck->MF) {
                tma_AddModelList (mdck = stm_mod_duplicate (NULL, ck->MD));
                tma_AddModelList (msck = stm_mod_duplicate (NULL, ck->MF));
                accesslist = tma_AddMinList (accesslist, ck->NODE, da, mdck, msck, md, ms, access, mdaccess, msaccess, s);
            }
            else
                accesslist = tma_AddMinList (accesslist, ck->NODE, da, NULL, NULL, md, ms, access + ck->DELAY, mdaccess, msaccess, s);
         }
   }
   return accesslist;
}

/****************************************************************************/

void tma_FreePathList (ttvpath_list *path)
{
   if (path)
      ttv_freepathlist (path);
}

/****************************************************************************/

void tma_MarkLatchCmd (ttvfig_list *fig, ttvsig_list *sig)
{
   ttvevent_list *cmd;
   chain_list    *cmdlist, *ptcmd;
   
   cmdlist = ttv_getlrcmd (fig, sig);
   for (ptcmd = cmdlist; ptcmd; ptcmd = ptcmd->NEXT) {
      cmd = (ttvevent_list*)ptcmd->DATA;
      if ((sig->TYPE & TTV_SIG_LF) == TTV_SIG_LF) {
         if (!getptype (cmd->USER, TTV_NODE_CMDFF)) 
            cmd->USER = addptype (cmd->USER, TTV_NODE_CMDFF, NULL);
      }
      else
      if ((sig->TYPE & TTV_SIG_LL) == TTV_SIG_LL
      && !((sig->TYPE & TTV_SIG_LR) == TTV_SIG_LR || (sig->TYPE & TTV_SIG_LS) == TTV_SIG_LS)) {
         if (!getptype (cmd->USER, TTV_NODE_CMDLL)) 
            cmd->USER = addptype (cmd->USER, TTV_NODE_CMDLL, NULL);
      }
   }
   freechain(cmdlist);
}

/****************************************************************************/

void tma_MarkCmd (ttvfig_list *fig)
{
   int i;
   
   for (i = 0; i < fig->NBELATCHSIG; i++) tma_MarkLatchCmd (fig, fig->ELATCHSIG[i]);
   for (i = 0; i < fig->NBILATCHSIG; i++) tma_MarkLatchCmd (fig, fig->ILATCHSIG[i]);
}

/****************************************************************************/
void tma_CreateBreakSH (ttvfig_list *fig, ttvevent_list *cmd,
                                          ttvevent_list *root, 
                                          timing_model *mdmin, 
                                          timing_model *mdmax, 
                                          long dmin, 
                                          long dmax, 
                                          timing_model *msmin, 
                                          timing_model *msmax, 
                                          chain_list **setuplist, chain_list **holdlist)
{
    chain_list    *ch;
    ptype_list    *ptype;
    ttvpath_list  *cklist;
    ttvline_list  *line;
    long valmin = 0, valmax = 0;
    timing_model *cmodel;
   
    tma_ExpanseBreak (fig, root->ROOT);
    ptype = getptype (root->USER, TTV_NODE_DUALPATH);
    if(!ptype)
        ptype = getptype (root->USER, TTV_NODE_DUALLINE);
    if (ptype) {
        for (ch = (chain_list*)ptype->DATA; ch; ch = ch->NEXT) {
            line = (ttvline_list*)ch->DATA;
            
            if (((line->TYPE & TTV_LINE_U) == TTV_LINE_U) && ((!cmd) || (cmd == line->ROOT))) {
                if (ttv_has_strict_setup(root)){
                    if (!(cklist = tma_GetPath (fig, NULL, tma_OppositeEvent (line->ROOT), TTV_FIND_MIN)))
                        cklist = tma_GetPath (fig, NULL, tma_OppositeEvent (line->ROOT), TTV_FIND_MAX); 
                    valmax = 0;
                }else{
                    if (!(cklist = tma_GetPath (fig, NULL, line->ROOT, TTV_FIND_MIN)))
                        cklist = tma_GetPath (fig, NULL, line->ROOT, TTV_FIND_MAX); 
                    valmax = line->VALMAX;
                }
                
                if (!(cmodel = stm_getmodel (fig->INFO->FIGNAME, line->MDMIN)))
                    cmodel = stm_getmodel (fig->INFO->FIGNAME, line->MDMAX);

                *setuplist = tma_AddSList (*setuplist, valmax, cmodel, dmax, mdmax, msmax, cklist, NULL);
                if(((line->ROOT->ROOT->TYPE  & TTV_SIG_C) == TTV_SIG_C) && ((line->ROOT->ROOT->TYPE & TTV_SIG_BYPASSIN) != TTV_SIG_BYPASSIN))
                    *setuplist = tma_AddSList (*setuplist, valmax, cmodel, dmax, mdmax, msmax, NULL, line->ROOT);
                tma_FreePathList (cklist);
            }
            
            if (((line->TYPE & TTV_LINE_O) == TTV_LINE_O) && ((!cmd) || (cmd == line->ROOT))) {
                cklist = tma_GetPath (fig, NULL, line->ROOT, TTV_FIND_MAX);

                valmin = line->VALMIN == TTV_NOTIME ? line->VALMAX:line->VALMIN;
                if (!(cmodel = stm_getmodel (fig->INFO->FIGNAME, line->MDMIN)))
                    cmodel = stm_getmodel (fig->INFO->FIGNAME, line->MDMAX);
                    
                *holdlist = tma_AddHList (*holdlist, valmin, cmodel, dmin, mdmin, msmin, cklist, NULL);
                if(((line->ROOT->ROOT->TYPE  & TTV_SIG_C) == TTV_SIG_C) && ((line->ROOT->ROOT->TYPE & TTV_SIG_BYPASSIN) != TTV_SIG_BYPASSIN))
                    *holdlist = tma_AddHList (*holdlist, valmin, cmodel, dmin, mdmin, msmin, NULL, line->ROOT);
                tma_FreePathList (cklist);
            }
        }

        if (tma_IsHoldEvent (root) && tma_IsSetupEvent (root)) { /* command breakpoint */
            if (!(cklist = tma_GetPath (fig, NULL, root, TTV_FIND_MIN)))
                cklist = tma_GetPath (fig, NULL, root, TTV_FIND_MAX); 
            *setuplist = tma_AddSList (*setuplist, 0, NULL, dmax, mdmax, msmax, cklist, NULL);
            tma_FreePathList (cklist);
            cklist = tma_GetPath (fig, NULL, root, TTV_FIND_MAX);
            *holdlist = tma_AddHList (*holdlist, 0, NULL, dmin, mdmin, msmin, cklist, NULL);
            tma_FreePathList (cklist);
        }
        else
        if (tma_IsSetupEvent (root)) { /* command breakpoint */
            if (!(cklist = tma_GetPath (fig, NULL, root, TTV_FIND_MIN)))
                cklist = tma_GetPath (fig, NULL, root, TTV_FIND_MAX); 
            *setuplist = tma_AddSList (*setuplist, 0, NULL, dmax, mdmax, msmax, cklist, NULL);
            tma_FreePathList (cklist);
        }
        else
        if (tma_IsHoldEvent (root)) { /* command breakpoint */
            cklist = tma_GetPath (fig, NULL, root, TTV_FIND_MAX);
            *holdlist = tma_AddHList (*holdlist, 0, NULL, dmin, mdmin, msmin, cklist, NULL);
            tma_FreePathList (cklist);
        }
    }
}

/****************************************************************************/

void tma_CreateBreakAccess (ttvfig_list *fig, ttvevent_list *node, timing_model *mdmin, timing_model *mdmax,
                  timing_model *msmin, timing_model *msmax, long dmin, long dmax, long smin, long smax,
                  chain_list **aminlist, chain_list **amaxlist, long deltamin, long deltamax)
{
    ttvpath_list  *cklist;
    ttvline_list  *line;
    ttvline_list  *inpath = NULL;
    timing_model  *mdaccess_max;
    timing_model  *msaccess_max;
    timing_model  *mdaccess_min;
    timing_model  *msaccess_min;

    tma_ExpanseBreak (fig, node->ROOT);

    if (TMA_CONTEXT->TMA_TYPE_TTVFIG == 'T')
       inpath = node->INPATH;
    else
    if (TMA_CONTEXT->TMA_TYPE_TTVFIG == 'D')
       inpath = node->INLINE;

    for (line = inpath; line; line = line->NEXT) {
        if ((line->TYPE & TTV_LINE_A) == TTV_LINE_A) {

            mdaccess_max = stm_getmodel (fig->INFO->FIGNAME, line->MDMAX);
            msaccess_max = stm_getmodel (fig->INFO->FIGNAME, line->MFMAX);
            if (!(mdaccess_min = stm_getmodel (fig->INFO->FIGNAME, line->MDMIN))) mdaccess_min = mdaccess_max;
            if (!(msaccess_min = stm_getmodel (fig->INFO->FIGNAME, line->MFMIN))) msaccess_min = msaccess_max;

            if ((line->NODE->ROOT->TYPE  & TTV_SIG_C) != TTV_SIG_C) {

                // max
                cklist = tma_GetPath (fig, NULL, line->NODE, TTV_FIND_MAX); 
                
                *amaxlist = tma_AddAMaxList (*amaxlist, line->VALMAX + deltamax, mdaccess_max, msaccess_max, 
                        dmax, mdmax, smax, msmax, cklist, NULL);
                
                tma_FreePathList (cklist);
                
                // min
                if (!(cklist = tma_GetPath (fig, NULL, line->NODE, TTV_FIND_MIN)))
                    cklist = tma_GetPath (fig, NULL, line->NODE, TTV_FIND_MAX); 
                
                *aminlist = tma_AddAMinList (*aminlist, line->VALMIN + deltamin, mdaccess_min, msaccess_min, 
                        dmin, mdmin, smin, msmin, cklist, NULL);
                
                tma_FreePathList (cklist);
            }
            else if((line->NODE->ROOT->TYPE & TTV_SIG_BYPASSIN) != TTV_SIG_BYPASSIN){
                *amaxlist = tma_AddAMaxList (*amaxlist, line->VALMAX + deltamax, mdaccess_max, msaccess_max, 
                        dmax, mdmax, smax, msmax, NULL, line->NODE);
                *aminlist = tma_AddAMinList (*aminlist, line->VALMIN + deltamin, mdaccess_min, msaccess_min, 
                        dmin, mdmin, smin, msmin, NULL, line->NODE);
            }
        }
    }
}

/****************************************************************************/

void tma_GetSTBdelta (stbfig_list *stbfig, ttvevent_list *latch, long *deltamin, long *deltamax) 
{
   int verbose = V_BOOL_TAB[__TMA_VERBOSE].VALUE;
    stb_getstbdelta(stbfig, latch,deltamin,deltamax, NULL) ;
    
    if (verbose) {
        //fprintf (stdout, "deltamax = %ld\n", *deltamax);
        //fprintf (stdout, "deltamin = %ld\n", *deltamin);
    }
}

/****************************************************************************/

void tma_FindTasSH( ttvevent_list *cmd, ttvevent_list *latch, long *setup, long *hold )
{
    ttvline_list *line ;
    *hold  = 0;
    *setup = 0;

    if (TMA_CONTEXT->TMA_TYPE_TTVFIG == 'D')
        line = cmd->INLINE;
    else if (TMA_CONTEXT->TMA_TYPE_TTVFIG == 'T')
        line = cmd->INPATH;
    
    while( line ) {
    
        if( ( line->TYPE & TTV_LINE_O ) == TTV_LINE_O && line->NODE == latch ) {
        
            if( line->VALMIN != TTV_NOTIME )
                *hold = line->VALMIN ;
            else
                *hold = line->VALMAX ;
        }
            
        if( ( line->TYPE & TTV_LINE_U ) == TTV_LINE_U && line->NODE == latch )
            *setup = line->VALMAX ;

        line = line->NEXT;
    }
    if (ttv_has_strict_setup(latch))
        *setup = 0;
}

/****************************************************************************/

ttvfig_list *tma_CreateBlackBox (ttvfig_list *fig, char *suffix)
{
   chain_list    *ptcmd,
                 *cmdlist,
                 *holdlist, 
                 *setuplist,
                 *aminlist, 
                 *amaxlist;
   ttvfig_list   *blackbox;
   char          *blackboxname,
                 *figname;
   timing_model  *mdmin,
                 *mdmax,
                 *msmin,
                 *msmax,
                 *trmodelmin,
                 *trmodelmax; 
   char           buf[1024],
                  nname[1024],
                  rname[1024];
   int            i;
   ttvsig_list   *sigc;
   ttvpath_list  *pathmaxlist, 
                 *pathmax, 
                 *pathmin,
                 *transpmaxlist,
                 *transpmax,
                 *transpmin,
                 *asynclist,
                 *asyncpath,
                 *ptprev,
                 *ptnext,
                 *cklist;
   ttvevent_list *root, *node, *cmd;
   long           dmax, dmin, 
                  smax, smin;
   int            event;
   long           deltamin, deltamax, type;
   stbnode       *stbnod;
   long           setup,
                  hold;
   int verbose = V_BOOL_TAB[__TMA_VERBOSE].VALUE;
   stbfig_list   *stbfig = NULL;

   if (!fig)
      return NULL;

   stbfig = stb_getstbfig(fig);

   figname = fig->INFO->FIGNAME;

   avt_log( LOGTMA, 1, "timing figure is %s\n", figname );

   if (suffix) {
      sprintf (buf, "%s_%s", figname, suffix);
      blackboxname = namealloc (buf);
   } else
      blackboxname = figname;

   blackbox = tma_DupTtvFigHeader (blackboxname, fig);
   stm_addcell (blackboxname);
   tma_DupConnectorList (blackbox, fig, NULL);

   tma_MarkCmd (fig);

   for (i = 0; i < fig->NBCONSIG; i++) {
      sigc = fig->CONSIG[i];

      if (verbose)
         avt_trace (1, stdout, "Processing pin %s\n", sigc->NAME);

      avt_log( LOGTMA, 1, "Processing pin %s\n", sigc->NAME );

      if (tma_IsClock (sigc)) {
        avt_log( LOGTMA, 1, "%s is a clock. continue\n", sigc->NAME );
        continue;
      }

      //----------------------------------------------------------------------------------------------------
      // Setup and Hold 
      //----------------------------------------------------------------------------------------------------

      if (((sigc->TYPE & TTV_SIG_CI) == TTV_SIG_CI || (sigc->TYPE & TTV_SIG_CT) == TTV_SIG_CT) && !((sigc->TYPE & TTV_SIG_CB) == TTV_SIG_CB)) { 

         if ((sigc->TYPE & TTV_SIG_BYPASSIN) == TTV_SIG_BYPASSIN) continue;

         if (verbose)
             avt_trace (2, stdout, "Computing SETUP, HOLD and PATH times\n");
         avt_log( LOGTMA, 1, "Computing SETUP, HOLD and PATH times\n");

         for (event = 0; event < 2; event++) {
            pathmaxlist = tma_GetPath (fig, &sigc->NODE[event], NULL, TTV_FIND_MAX);
            setuplist   = NULL;
            holdlist    = NULL;
            tma_InitModelList ();

            // pas de chemin : report d'eventuels setup/hold de breakpoints a l'interface
            if (!pathmaxlist) {
                    avt_log( LOGTMA, 2, "No path found. Reporting existing setup/hold to the boundary\n" );
                    tma_CreateBreakSH (fig, NULL, &sigc->NODE[event], NULL, NULL, 0, 0, NULL, NULL, &setuplist, &holdlist);
            }

            for (pathmax = pathmaxlist; pathmax; pathmax = pathmax->NEXT) {
               root   = pathmax->ROOT;
               node   = pathmax->NODE;
               cmd    = pathmax->CMD; 
               pathmin = tma_GetPath (fig, node, root, TTV_FIND_MIN);
               dmax   = pathmax->DELAY;
               smax   = pathmax->SLOPE;
               tma_AddModelList (mdmax = stm_mod_duplicate (NULL, pathmax->MD));
               tma_AddModelList (msmax = stm_mod_duplicate (NULL, pathmax->MF));

               if (pathmin) {
                  dmin  = pathmin->DELAY;
                  smin  = pathmin->SLOPE;
                  tma_AddModelList (mdmin = stm_mod_duplicate (NULL, pathmin->MD));
                  tma_AddModelList (msmin = stm_mod_duplicate (NULL, pathmin->MF));
               } else {
                  dmin  = pathmax->DELAY;
                  smin  = pathmax->SLOPE;
                  tma_AddModelList (mdmin = stm_mod_duplicate (NULL, pathmax->MD));
                  tma_AddModelList (msmin = stm_mod_duplicate (NULL, pathmax->MF));
               }

               avt_log( LOGTMA, 2, "path from %s to %s cmd %s : dmax=%ld dmin=%ld smax=%ld smin=%ld\n",
                                   node ? node->ROOT->NAME : "null",
                                   root ? root->ROOT->NAME : "null",
                                   cmd  ? cmd->ROOT->NAME : "null",
                                   dmax, dmin, smax, smin
                      );


               // Flip-flop -------------------------------------------------------------------------

               if (((root->ROOT->TYPE & TTV_SIG_LF) == TTV_SIG_LF)) {
                  ttv_getsigname (fig, rname, root->ROOT);
                  if(cmd) { 
                      if (verbose) avt_trace (3, stdout, "To flip-flop %s\n", rname);
                      if ((cmd->ROOT->TYPE & TTV_SIG_C) != TTV_SIG_C) {
                         if (!(cklist = tma_GetPath (fig, NULL, cmd, TTV_FIND_MIN)))
                            cklist = tma_GetPath (fig, NULL, cmd, TTV_FIND_MAX); 
                         setuplist = tma_AddSList (setuplist, 0, NULL, dmax, mdmax, msmax, cklist, NULL);
                         tma_FreePathList (cklist);
                         cklist = tma_GetPath (fig, NULL, cmd, TTV_FIND_MAX); 
                         holdlist = tma_AddHList (holdlist, 0, NULL, dmin, mdmin, msmin, cklist, NULL);
                         tma_FreePathList (cklist);
                      } else {
                         setuplist = tma_AddSList (setuplist, 0, NULL, dmax, mdmax, msmax, NULL, cmd);
                         holdlist = tma_AddHList (holdlist, 0, NULL, dmin, mdmin, msmin, NULL, cmd);
                      }
                  }
               }
               
               // Latch model TAS -------------------------------------------------------------------

               if ((root->ROOT->TYPE & TTV_SIG_LL) == TTV_SIG_LL
                && !((root->ROOT->TYPE & TTV_SIG_LR) == TTV_SIG_LR)
                && !((root->ROOT->TYPE & TTV_SIG_LS) == TTV_SIG_LS)
                && !(tma_IsHoldEvent (root) || tma_IsSetupEvent (root))) { 
                  ttv_getsigname (fig, rname, root->ROOT);
                  if (verbose) avt_trace (3, stdout, "To latch %s\n", rname);
                  if (cmd) { 
                      tma_FindTasSH( tma_OppositeEvent (cmd), root, &setup, &hold );
                      if ((cmd->ROOT->TYPE & TTV_SIG_C) != TTV_SIG_C) {
                         if (ttv_has_strict_setup(root)){
                            if (!(cklist = tma_GetPath (fig, NULL, cmd, TTV_FIND_MIN)))
                               cklist = tma_GetPath (fig, NULL, cmd, TTV_FIND_MAX); 
                         }else{
                            if (!(cklist = tma_GetPath (fig, NULL, tma_OppositeEvent (cmd), TTV_FIND_MIN)))
                               cklist = tma_GetPath (fig, NULL, tma_OppositeEvent (cmd), TTV_FIND_MAX); 
                         }
                         setuplist = tma_AddSList (setuplist, setup, NULL, dmax, mdmax, msmax, cklist, NULL);
                         tma_FreePathList (cklist);
                         cklist = tma_GetPath (fig, NULL, tma_OppositeEvent (cmd), TTV_FIND_MAX); 
                         holdlist = tma_AddHList (holdlist, hold, NULL, dmin, mdmin, msmin, cklist, NULL);
                         tma_FreePathList (cklist);
                      } else {
                         if (ttv_has_strict_setup(root)){
                            setuplist = tma_AddSList (setuplist, setup, NULL, dmax, mdmax, msmax, NULL, cmd);
                         }else{
                            setuplist = tma_AddSList (setuplist, setup, NULL, dmax, mdmax, msmax, NULL, tma_OppositeEvent (cmd));
                         }
                         holdlist = tma_AddHList (holdlist, hold, NULL, dmin, mdmin, msmin, NULL, tma_OppositeEvent (cmd));
                      }
                  }
                  if (tma_IsLatchCell (fig) && (!getptype (node->ROOT->USER, TTV_SIG_ASYNCHRON))) { // transparancy 
                     transpmaxlist = tma_GetPath (fig, root, NULL, TTV_FIND_MAX);
                     for (transpmax = transpmaxlist; transpmax; transpmax = transpmax->NEXT) {
                        if ((transpmax->ROOT->ROOT->TYPE & TTV_SIG_CO) == TTV_SIG_CO) {
                           transpmin = tma_GetPath (fig, transpmax->NODE, transpmax->ROOT, TTV_FIND_MIN); 
                           ttv_getsigname (fig, nname, node->ROOT);
                           ttv_getsigname (fig, rname, transpmax->ROOT->ROOT);
                           trmodelmax = NULL;
                           trmodelmin = NULL;
                           if (transpmin) {
                              if (mdmin && transpmin->MD)
                                trmodelmin = stm_mod_merge (NULL, mdmin, transpmin->MD, 0, -1, -1);
                              if (mdmax && transpmax->MD)
                                trmodelmax = stm_mod_merge (NULL, mdmax, transpmax->MD, 0, -1, -1);
                              tma_CreatePath (blackbox, nname, node->TYPE, rname, transpmax->ROOT->TYPE, 
                                          trmodelmin, trmodelmax,
                                          transpmin->MF, transpmax->MF,
                                          dmin + transpmin->DELAY, dmax + transpmax->DELAY, 
                                          transpmin->SLOPE, transpmax->SLOPE, 0);
                              stm_mod_destroy (trmodelmin);
                              stm_mod_destroy (trmodelmax);
                           } else {
                              if (mdmax && transpmax->MD)
                                trmodelmax = stm_mod_merge (NULL, mdmax, transpmax->MD, 0, -1, -1);
                              tma_CreatePath (blackbox, nname, node->TYPE, rname, transpmax->ROOT->TYPE, 
                                          NULL, trmodelmax,
                                          NULL, transpmax->MF,
                                          TTV_NOTIME, dmax + transpmax->DELAY, 
                                          TTV_NOSLOPE, transpmax->SLOPE, 0);
                              stm_mod_destroy (trmodelmax);
                           }
                        }
                     }
                  }
               }

               // Anything with setup or hold lines on it -------------------------------------------
               
               ttv_getsigname (fig, rname, root->ROOT);
               if (((root->ROOT->TYPE & TTV_SIG_LL) == TTV_SIG_LL)  // latch model blackbox or tasMemoryCharacterization
                   && !((root->ROOT->TYPE & TTV_SIG_LR) == TTV_SIG_LR)
                   && !((root->ROOT->TYPE & TTV_SIG_LS) == TTV_SIG_LS)) {
                   if (verbose) avt_trace (3, stdout, "To Latch Black %s\n", rname);
                   tma_CreateBreakSH (fig, tma_OppositeEvent (cmd), root, mdmin, mdmax, dmin, dmax, msmin, msmax, &setuplist, &holdlist);
               } else {
                   if (verbose) avt_trace (3, stdout, "To breakpoint %s\n", rname);
                   tma_CreateBreakSH (fig, cmd, root, mdmin, mdmax, dmin, dmax, msmin, msmax, &setuplist, &holdlist);
               }
               
               // Precharge -------------------------------------------------------------------------
              
               if ((root->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R) {
                  ttv_getsigname (fig, rname, root->ROOT);
                  if (verbose)
                      avt_trace (3, stdout, "To precharged signal %s\n", rname);
                  if (tma_IsPrLine (node, root)) { // Precharge line
                     
                     if (!(cklist = tma_GetPath (fig, NULL, root, TTV_FIND_MIN | TTV_FIND_PR)))
                        cklist = tma_GetPath (fig, NULL, root, TTV_FIND_MAX | TTV_FIND_PR);
                     setuplist = tma_AddSList (setuplist, 0, NULL, dmax, mdmax, msmax, cklist, NULL);
                     tma_FreePathList (cklist);
                    
                     cklist = tma_GetPath (fig, NULL, tma_OppositeEvent (root), TTV_FIND_MAX | TTV_FIND_EV);
                     holdlist = tma_AddHList (holdlist, 0, NULL, dmin, mdmin, msmin, cklist, NULL);
                     tma_FreePathList (cklist);
                  } 
                  if (tma_IsEvLine (node, root)) { // Eval Line
                   
                     if (!(cklist = tma_GetPath (fig, NULL, root, TTV_FIND_MIN | TTV_FIND_EV)))
                        cklist = tma_GetPath (fig, NULL, root, TTV_FIND_MIN | TTV_FIND_EV);
                     setuplist = tma_AddSList (setuplist, 0, NULL, dmax, mdmax, msmax, cklist, NULL);
                     tma_FreePathList (cklist);
                  
                     /* TODO : le min des clocks max des latchs attaques par la precharge */
                     cklist = tma_GetPath (fig, NULL, tma_OppositeEvent (root), TTV_FIND_MAX | TTV_FIND_PR);
                     holdlist = tma_AddHList (holdlist, 0, NULL, dmin, mdmin, msmin, cklist, NULL);
                     tma_FreePathList (cklist);
                  }
               }

               // Command -------------------------------------------------------------------------
               
               if ((root->ROOT->TYPE & TTV_SIG_Q) == TTV_SIG_Q) { 
                  ttv_getsigname (fig, rname, root->ROOT);
                  if (verbose) avt_trace (3, stdout, "To command %s\n", rname);
                  if (getptype (root->USER, TTV_NODE_CMDLL)) {
                     if ((stbnod = stb_getstbnode (root))) {
                        if ((stbnod->CK->VERIF & STB_NO_VERIF) == STB_NO_VERIF)
                            cklist = NULL;
                        else
                        if ((stbnod->CK->VERIF & STB_VERIF_EDGE) == STB_VERIF_EDGE) {
                            if (!(cklist = tma_GetPath (fig, NULL, tma_OppositeEvent (root), TTV_FIND_MIN)))
                                cklist = tma_GetPath (fig, NULL, tma_OppositeEvent (root), TTV_FIND_MAX); 
                        }
                        else {
                            if (!(cklist = tma_GetPath (fig, NULL, root, TTV_FIND_MIN)))
                                cklist = tma_GetPath (fig, NULL, root, TTV_FIND_MAX); 
                        }
                     }
                     else {
                        if (!(cklist = tma_GetPath (fig, NULL, root, TTV_FIND_MIN)))
                           cklist = tma_GetPath (fig, NULL, root, TTV_FIND_MAX); 
                     }
                     setuplist = tma_AddSList (setuplist, 0, NULL, dmax, mdmax, msmax, cklist, NULL);
                     tma_FreePathList (cklist);
                     cklist = tma_GetPath (fig, NULL, tma_OppositeEvent (root), TTV_FIND_MAX);
                     holdlist = tma_AddHList (holdlist, 0, NULL, dmin, mdmin, msmin, cklist, NULL);
                     tma_FreePathList (cklist);
                  }
                  else
                  if (getptype (root->USER, TTV_NODE_CMDFF)) {
                     if (!(cklist = tma_GetPath (fig, NULL, root, TTV_FIND_MIN)))
                        cklist = tma_GetPath (fig, NULL, root, TTV_FIND_MAX); 
                     setuplist = tma_AddSList (setuplist, 0, NULL, dmax, mdmax, msmax, cklist, NULL);
                     tma_FreePathList (cklist);
                     cklist = tma_GetPath (fig, NULL, tma_OppositeEvent (root), TTV_FIND_MAX);
                     holdlist = tma_AddHList (holdlist, 0, NULL, dmin, mdmin, msmin, cklist, NULL);
                     tma_FreePathList (cklist);
                  }
               }
               tma_FreePathList (pathmin);
            }

            tma_ConvSList (fig, blackbox, &sigc->NODE[event], setuplist);
            tma_ConvHList (fig, blackbox, &sigc->NODE[event], holdlist);
            tma_FreeList (setuplist);
            tma_FreeList (holdlist);
            tma_FreePathList (pathmaxlist);
            tma_FreeModelList ();
         }
      }

      //----------------------------------------------------------------------------------------------------
      // Access
      //----------------------------------------------------------------------------------------------------
      
      if ((((sigc->TYPE & TTV_SIG_CO) == TTV_SIG_CO || (sigc->TYPE & TTV_SIG_CB) == TTV_SIG_CB || (sigc->TYPE & TTV_SIG_CT) == TTV_SIG_CT))) {
          
         if (verbose) avt_trace (2, stdout, "Computing ACCESS times\n");
         avt_log( LOGTMA, 1, "Computing ACCESS times\n" );

         for (event = 0; event < 2; event++) {
            pathmaxlist = tma_GetPath (fig, NULL, &sigc->NODE[event], TTV_FIND_MAX);
            aminlist = NULL;
            amaxlist = NULL;
            tma_InitModelList ();

            // Report des access des breakpoints directement connectes a l'interface 
            if (!pathmaxlist) {
                avt_log( LOGTMA, 2, "No path found. Reporting existing access to the boundary\n" );
                tma_CreateBreakAccess (fig, &sigc->NODE[event], NULL, NULL, NULL, NULL, 0, 0, 0, 0, &aminlist, &amaxlist, 0, 0);
            }

            for (pathmax = pathmaxlist; pathmax; pathmax = pathmax->NEXT) {

               deltamin = 0;
               deltamax = 0;

               node   = pathmax->NODE;
               root   = pathmax->ROOT;
               cmd    = pathmax->CMD;

               // Get corresponding min path
               pathmin = tma_GetPath (fig, node, root, TTV_FIND_MIN);
               dmax   = pathmax->DELAY;
               smax   = pathmax->SLOPE;
               tma_AddModelList (mdmax = stm_mod_duplicate (NULL, pathmax->MD));
               tma_AddModelList (msmax   = stm_mod_duplicate (NULL, pathmax->MF));

               if (pathmin) {
                  dmin  = pathmin->DELAY;
                  smin  = pathmin->SLOPE;
                  tma_AddModelList (mdmin = stm_mod_duplicate (NULL, pathmin->MD));
                  tma_AddModelList (msmin = stm_mod_duplicate (NULL, pathmin->MF));
               } else {
                  dmin  = pathmax->DELAY;
                  smin  = pathmax->SLOPE;
                  tma_AddModelList (mdmin = stm_mod_duplicate (NULL, pathmax->MD));
                  tma_AddModelList (msmin = stm_mod_duplicate (NULL, pathmax->MF));
               }

               avt_log( LOGTMA, 2, "path from %s to %s cmd %s : dmax=%ld dmin=%ld smax=%ld smin=%ld\n",
                                   node ? node->ROOT->NAME : "null",
                                   root ? root->ROOT->NAME : "null",
                                   cmd  ? cmd->ROOT->NAME : "null",
                                   dmax, dmin, smax, smin
                      );
               tma_FreePathList (pathmin);

               // Add Transparancy delta
               if (((node->ROOT->TYPE & TTV_SIG_LL) == TTV_SIG_LL) 
                 && !(((node->ROOT->TYPE & TTV_SIG_LR) == TTV_SIG_LR) || ((node->ROOT->TYPE & TTV_SIG_LS) == TTV_SIG_LS))) {
                  tma_GetSTBdelta (stbfig, node, &deltamin, &deltamax);
                  avt_log( LOGTMA, 2, "transparancy : deltamin=%ld deltamax=%ld\n", deltamin, deltamax );
               }

               // Flip-flop ---------------------------------------------------------------
               
               if (((node->ROOT->TYPE & TTV_SIG_LF) == TTV_SIG_LF)
                 && !(((node->ROOT->TYPE & TTV_SIG_LR) == TTV_SIG_LR) || ((node->ROOT->TYPE & TTV_SIG_LS) == TTV_SIG_LS))
                 && ((pathmax->TYPE & TTV_FIND_HZ) != TTV_FIND_HZ)) {
                  ttv_getsigname (fig, nname, node->ROOT);

                  if (verbose)
                      avt_trace (3, stdout, "From flip-flop %s\n", nname);

                  avt_log( LOGTMA, 1, "from flip-flop %s\n", nname );

                  cmdlist = ttv_getlrcmd (fig, node->ROOT); 
                  for (ptcmd = cmdlist; ptcmd; ptcmd = ptcmd->NEXT) {
                     cmd = (ttvevent_list*)ptcmd->DATA;
                     if ((cmd->ROOT->TYPE & TTV_SIG_C) != TTV_SIG_C) {
                        cklist = tma_GetPath (fig, NULL, cmd, TTV_FIND_MAX); 
                        amaxlist = tma_AddAMaxList (amaxlist, 0, NULL, NULL, dmax, mdmax, smax, msmax, cklist, NULL);
                        tma_FreePathList (cklist);
                        if (!(cklist = tma_GetPath (fig, NULL, cmd, TTV_FIND_MIN)))
                           cklist = tma_GetPath (fig, NULL, cmd, TTV_FIND_MAX); 
                        aminlist = tma_AddAMinList (aminlist, 0, NULL, NULL, dmin, mdmin, smin, msmin, cklist, NULL);
                        tma_FreePathList (cklist);
                     } else {
                        amaxlist = tma_AddAMaxList (amaxlist, 0, NULL, NULL, dmax, mdmax, smax, msmax, NULL, cmd);
                        aminlist = tma_AddAMinList (aminlist, 0, NULL, NULL, dmin, mdmin, smin, msmin, NULL, cmd);
                     }
                  }
                  if (cmdlist) freechain (cmdlist);
               }

               // Latch ---------------------------------------------------------------
               

               if (((node->ROOT->TYPE & TTV_SIG_LL) == TTV_SIG_LL) 
                 && !(((node->ROOT->TYPE & TTV_SIG_LR) == TTV_SIG_LR) || ((node->ROOT->TYPE & TTV_SIG_LS) == TTV_SIG_LS))
                 && !tma_IsAccessEvent (node)
                 && ((pathmax->TYPE & TTV_FIND_HZ) != TTV_FIND_HZ)) {

                  ttv_getsigname (fig, nname, node->ROOT);
  
                  if (verbose)
                      avt_trace (3, stdout, "From latch %s\n", nname);
                      
                  avt_log( LOGTMA, 1, "from latch %s\n", nname );

                  cmdlist = ttv_getlrcmd (fig, node->ROOT); 
                  for (ptcmd = cmdlist; ptcmd; ptcmd = ptcmd->NEXT) {

                     cmd = (ttvevent_list*)ptcmd->DATA;

                     if ((cmd->ROOT->TYPE & TTV_SIG_C) != TTV_SIG_C) {

                        cklist = tma_GetPath (fig, NULL, cmd, TTV_FIND_MAX); 
                        amaxlist = tma_AddAMaxList (amaxlist, deltamax, NULL, NULL, dmax, mdmax, smax, msmax, cklist, NULL);
                        tma_FreePathList (cklist);
                        if (!(cklist = tma_GetPath (fig, NULL, cmd, TTV_FIND_MIN)))
                           cklist = tma_GetPath (fig, NULL, cmd, TTV_FIND_MAX); 
                        aminlist = tma_AddAMinList (aminlist, deltamin, NULL, NULL, dmin, mdmin, smin, msmin, cklist, NULL);
                        tma_FreePathList (cklist);
                     } else {
                        if((cmd->TYPE & TTV_NODE_BYPASSIN) != TTV_NODE_BYPASSIN){
                           amaxlist = tma_AddAMaxList (amaxlist, deltamax, NULL, NULL, dmax, mdmax, smax, msmax, NULL, cmd);
                           aminlist = tma_AddAMinList (aminlist, deltamin, NULL, NULL, dmin, mdmin, smin, msmin, NULL, cmd);
                        }
                     }
                  }
                  if (cmdlist)
                     freechain (cmdlist);
               }
               
               // Asynchron ------------------------------------------------------------
               
               if (((node->ROOT->TYPE & TTV_SIG_LL) == TTV_SIG_LL) 
                 && !(((node->ROOT->TYPE & TTV_SIG_LR) == TTV_SIG_LR) || ((node->ROOT->TYPE & TTV_SIG_LS) == TTV_SIG_LS))
                 && ((pathmax->TYPE & TTV_FIND_HZ) != TTV_FIND_HZ)) {

                  asynclist = tma_GetPath (fig, NULL, node, TTV_FIND_MAX);
                  ptprev = NULL;
                  for(asyncpath = asynclist; asyncpath; asyncpath = ptnext){
                      ptnext = asyncpath->NEXT;
                      if(!getptype (asyncpath->NODE->ROOT->USER, TTV_SIG_ASYNCHRON)){
                          asyncpath->NEXT = NULL;
                          tma_FreePathList (asyncpath);
                          if(ptprev == NULL){
                              asynclist = ptnext;
                          }else{
                              ptprev->NEXT = ptnext;
                          }
                      }else{
                          ptprev = asyncpath;
                      }
                  }
                  if (asynclist){
                      amaxlist = tma_AddAMaxList (amaxlist, deltamax, NULL, NULL, dmax, mdmax, smax, msmax, asynclist, NULL);
                      tma_FreePathList (asynclist);
                  }
                  if (!(asynclist = tma_GetPath (fig, NULL, node, TTV_FIND_MIN)))
                      asynclist = tma_GetPath (fig, NULL, node, TTV_FIND_MAX); 
                  ptprev = NULL;
                  for(asyncpath = asynclist; asyncpath; asyncpath = ptnext){
                      ptnext = asyncpath->NEXT;
                      if(!getptype (asyncpath->NODE->ROOT->USER, TTV_SIG_ASYNCHRON)){
                          asyncpath->NEXT = NULL;
                          tma_FreePathList (asyncpath);
                          if(ptprev == NULL){
                              asynclist = ptnext;
                          }else{
                              ptprev->NEXT = ptnext;
                          }
                      }else{
                          ptprev = asyncpath;
                      }
                  }
                  if (asynclist){
                      aminlist = tma_AddAMinList (aminlist, deltamin, NULL, NULL, dmin, mdmin, smin, msmin, asynclist, NULL);
                      tma_FreePathList (asynclist);
                  }
               }
               
               // Anything with access lines on it -------------------------------------------
              
               if((pathmax->TYPE & TTV_FIND_HZ) != TTV_FIND_HZ)
                   avt_log( LOGTMA, 1, "from anything\n"  );
                   tma_CreateBreakAccess (fig, node, mdmin, mdmax, msmin, msmax, dmin, dmax, smin, smax, &aminlist, &amaxlist, deltamin, deltamax);

               // Precharge -------------------------------------------------------------------------

               if (((node->ROOT->TYPE & TTV_SIG_R) == TTV_SIG_R) && ((pathmax->TYPE & TTV_FIND_HZ) != TTV_FIND_HZ)) {
                  ttv_getsigname (fig, nname, node->ROOT);
                  if (verbose)
                      avt_trace (3, stdout, "From precharged signal %s\n", nname);
                  avt_log( LOGTMA, 1, "From precharged signal %s\n", nname);
                  cklist = tma_GetPath (fig, NULL, node, TTV_FIND_MAX | TTV_FIND_EV);
                  amaxlist = tma_AddAMaxList (amaxlist, 0, NULL, NULL, dmax, mdmax, smax, msmax, cklist, NULL);
                  tma_FreePathList (cklist);
                  if (!(cklist = tma_GetPath (fig, NULL, node, TTV_FIND_MIN | TTV_FIND_EV)))
                     cklist = tma_GetPath (fig, NULL, node, TTV_FIND_MAX | TTV_FIND_EV);
                  aminlist = tma_AddAMinList (aminlist, 0, NULL, NULL, dmin, mdmin, smin, msmin, cklist, NULL);
                  tma_FreePathList (cklist);
               }
               
               // Connector -------------------------------------------------------------------------
               
               if ((node->ROOT->TYPE & TTV_SIG_C) == TTV_SIG_C) { 
                  ttv_getsigname (fig, nname, node->ROOT);
                  ttv_getsigname (fig, rname, root->ROOT);
                  if (verbose)
                      avt_trace (3, stdout, "From connector %s\n", nname);
                  avt_log( LOGTMA, 1, "From connector %s\n", nname);
                  if ((pathmax->TYPE & TTV_FIND_HZ) == TTV_FIND_HZ){
                     type = TTV_LINE_HZ;
                  }else{
                     type = 0;
                  }
                  if (pathmin) 
                     tma_CreatePath (blackbox, nname, node->TYPE, rname, root->TYPE, 
                                 mdmin, mdmax, msmin, msmax, 
                                 dmin, dmax, smin, smax, type);
                  else
                     tma_CreatePath (blackbox, nname, node->TYPE, rname, root->TYPE, 
                                 NULL, mdmax, NULL, msmax, 
                                 TTV_NOTIME, dmax, TTV_NOSLOPE, smax, type);
               }
            }

            tma_ConvAList (fig, blackbox, &sigc->NODE[event], aminlist, amaxlist);
            tma_FreeList (aminlist);
            tma_FreeList (amaxlist);
            tma_FreePathList (pathmaxlist);
            tma_FreeModelList ();
         }
      }
   }

   tma_AddLatchSig (blackbox);
   tma_AddBreakSig (blackbox);

   return blackbox;
}

/****************************************************************************/

void tma_AddInsertDelays (ttvfig_list *blackbox, ttvfig_list *fig)
{
   ttvpath_list *insert, *savinsert;
   int           i, flag=0;
   ttvsig_list  *sig;
   ttvsig_list  *internal;
   long          findtype;
   long          type;
   timing_model *mdmin, *mdmax, *msmin, *msmax;
   char          nodename[1024], rootname[1024];
   long          rmin, rmax, hmin, hmax, dmin, dmax, smax, smin;
   long          nodetype, roottype;

   for (i = 0; i < fig->NBCONSIG; i++) {
      sig = fig->CONSIG[i];
      if (tma_IsClock (sig)) {

         /* internal clock sig */
         if(!flag){
             blackbox->ELCMDSIG = ttv_allocreflist (ttv_addrefsig (blackbox, namealloc ("insertion"), namealloc ("insertion"), 0.0, TTV_SIG_Q, NULL), 1);
             blackbox->NBELCMDSIG = 1;
             flag = 1;
         }
         internal = blackbox->ELCMDSIG[0];
         blackbox->STATUS |= TTV_STS_L;
         ttv_getsigname (blackbox, nodename, sig);
         ttv_getsigname (blackbox, rootname, internal);
         
         if (TMA_CONTEXT->TMA_TYPE_TTVFIG == 'T')
            findtype = TTV_FIND_CARAC | TTV_FIND_SIG | TTV_FIND_DUAL | TTV_FIND_PATH;
         else
         if (TMA_CONTEXT->TMA_TYPE_TTVFIG == 'D')
            findtype = TTV_FIND_CARAC | TTV_FIND_SIG | TTV_FIND_DUAL | TTV_FIND_LINE;

         /* uu */
         nodetype = TTV_NODE_UP;
         roottype = TTV_NODE_UP;
         mdmax = NULL;
         msmax = NULL;
         dmax  = TTV_NOTIME;
         smax  = TTV_NOSLOPE;
         rmax  = TTV_NORES;
         hmax  = TTV_NOS;
         mdmin = NULL;
         msmin = NULL;
         dmin  = TTV_NOTIME;
         smin  = TTV_NOSLOPE;
         rmin  = TTV_NORES;
         hmin  = TTV_NOS;
         type = findtype | TTV_FIND_MAX | TTV_FIND_NOT_DWUP | TTV_FIND_NOT_DWDW | TTV_FIND_NOT_UPDW; 
         if ((savinsert=insert = ttv_getpathnocross_v2 (fig, NULL, sig, NULL, TTV_DELAY_MAX, TTV_DELAY_MIN, type,1))) {
            mdmax = insert->MD;
            msmax = insert->MF;
            dmax  = insert->DELAY;
            smax  = insert->SLOPE;
            if(!msmax && mdmax){
                msmax = stm_mod_create_fcst (NULL, smax, STM_MOD_MODTBL);
                stm_mod_update (msmax, mdmax->VTH, mdmax->VDD, mdmax->VT, mdmax->VF);
            }
         }
         type = findtype | TTV_FIND_MIN | TTV_FIND_NOT_DWUP | TTV_FIND_NOT_DWDW | TTV_FIND_NOT_UPDW;
         if ((insert = ttv_getpathnocross_v2 (fig, NULL, sig, NULL, TTV_DELAY_MAX, TTV_DELAY_MIN, type,1))) {
            mdmin = insert->MD;
            msmin = insert->MF;
            dmin  = insert->DELAY;
            smin  = insert->SLOPE;
            if(!msmin && mdmin){
                msmin = stm_mod_create_fcst (NULL, smin, STM_MOD_MODTBL);
                stm_mod_update (msmin, mdmin->VTH, mdmin->VDD, mdmin->VT, mdmin->VF);
            }
         }
         tma_CreatePath (blackbox, nodename, nodetype, rootname, roottype, mdmin, mdmax, 
                         msmin, msmax, dmin, dmax, smin, smax, 0);
         ttv_freepathlist(insert);
         ttv_freepathlist(savinsert);

         /* ud */
         nodetype = TTV_NODE_UP;
         roottype = TTV_NODE_DOWN;
         mdmax = NULL;
         msmax = NULL;
         dmax  = TTV_NOTIME;
         smax  = TTV_NOSLOPE;
         mdmin = NULL;
         msmin = NULL;
         dmin  = TTV_NOTIME;
         smin  = TTV_NOSLOPE;
         type = findtype | TTV_FIND_MAX | TTV_FIND_NOT_DWUP | TTV_FIND_NOT_DWDW | TTV_FIND_NOT_UPUP;
         if ((savinsert=insert = ttv_getpathnocross_v2 (fig, NULL, sig, NULL, TTV_DELAY_MAX, TTV_DELAY_MIN, type,1))) {
            mdmax = insert->MD;
            msmax = insert->MF;
            dmax  = insert->DELAY;
            smax  = insert->SLOPE;
            if(!msmax && mdmax){
                msmax = stm_mod_create_fcst (NULL, smax, STM_MOD_MODTBL);
                stm_mod_update (msmax, mdmax->VTH, mdmax->VDD, mdmax->VT, mdmax->VF);
            }
         }
         type = findtype | TTV_FIND_MIN | TTV_FIND_NOT_DWUP | TTV_FIND_NOT_DWDW | TTV_FIND_NOT_UPUP; 
         if ((insert = ttv_getpathnocross_v2 (fig, NULL, sig, NULL, TTV_DELAY_MAX, TTV_DELAY_MIN, type,1))) {
            mdmin = insert->MD;
            msmin = insert->MF;
            dmin  = insert->DELAY;
            smin  = insert->SLOPE;
            if(!msmin && mdmin){
                msmin = stm_mod_create_fcst (NULL, smin, STM_MOD_MODTBL);
                stm_mod_update (msmin, mdmin->VTH, mdmin->VDD, mdmin->VT, mdmin->VF);
            }
         }
         tma_CreatePath (blackbox, nodename, nodetype, rootname, roottype, mdmin, mdmax, 
                         msmin, msmax, dmin, dmax, smin, smax, 0);
         ttv_freepathlist(insert);
         ttv_freepathlist(savinsert);

         /* du */
         nodetype = TTV_NODE_DOWN;
         roottype = TTV_NODE_UP;
         mdmax = NULL;
         msmax = NULL;
         dmax  = TTV_NOTIME;
         smax  = TTV_NOSLOPE;
         mdmin = NULL;
         msmin = NULL;
         dmin  = TTV_NOTIME;
         smin  = TTV_NOSLOPE;
         type = findtype | TTV_FIND_MAX | TTV_FIND_NOT_UPUP | TTV_FIND_NOT_UPDW | TTV_FIND_NOT_DWDW;
         if ((savinsert=insert = ttv_getpathnocross_v2 (fig, NULL, sig, NULL, TTV_DELAY_MAX, TTV_DELAY_MIN, type,1))) {
            mdmax = insert->MD;
            msmax = insert->MF;
            dmax  = insert->DELAY;
            smax  = insert->SLOPE;
            if(!msmax && mdmax){
                msmax = stm_mod_create_fcst (NULL, smax, STM_MOD_MODTBL);
                stm_mod_update (msmax, mdmax->VTH, mdmax->VDD, mdmax->VT, mdmax->VF);
            }
         }
         type = findtype | TTV_FIND_MIN | TTV_FIND_NOT_UPUP | TTV_FIND_NOT_UPDW | TTV_FIND_NOT_DWDW;
         if ((insert = ttv_getpathnocross_v2 (fig, NULL, sig, NULL, TTV_DELAY_MAX, TTV_DELAY_MIN, type,1))) {
            mdmin = insert->MD;
            msmin = insert->MF;
            dmin  = insert->DELAY;
            smin  = insert->SLOPE;
            if(!msmin && mdmin){
                msmin = stm_mod_create_fcst (NULL, smin, STM_MOD_MODTBL);
                stm_mod_update (msmin, mdmin->VTH, mdmin->VDD, mdmin->VT, mdmin->VF);
            }
         }
         tma_CreatePath (blackbox, nodename, nodetype, rootname, roottype, mdmin, mdmax, 
                         msmin, msmax, dmin, dmax, smin, smax, 0);
         ttv_freepathlist(insert);
         ttv_freepathlist(savinsert);

         /* dd */
         nodetype = TTV_NODE_DOWN;
         roottype = TTV_NODE_DOWN;
         mdmax = NULL;
         msmax = NULL;
         dmax  = TTV_NOTIME;
         smax  = TTV_NOSLOPE;
         mdmin = NULL;
         msmin = NULL;
         dmin  = TTV_NOTIME;
         smin  = TTV_NOSLOPE;
         type = findtype | TTV_FIND_MAX | TTV_FIND_NOT_UPUP | TTV_FIND_NOT_UPDW | TTV_FIND_NOT_DWUP; 
         if ((savinsert=insert = ttv_getpathnocross_v2 (fig, NULL, sig, NULL, TTV_DELAY_MAX, TTV_DELAY_MIN, type,1))) {
            mdmax = insert->MD;
            msmax = insert->MF;
            dmax  = insert->DELAY;
            smax  = insert->SLOPE;
            if(!msmax && mdmax){
                msmax = stm_mod_create_fcst (NULL, smax, STM_MOD_MODTBL);
                stm_mod_update (msmax, mdmax->VTH, mdmax->VDD, mdmax->VT, mdmax->VF);
            }
         }
         type = findtype | TTV_FIND_MIN | TTV_FIND_NOT_UPUP | TTV_FIND_NOT_UPDW | TTV_FIND_NOT_DWUP;
         if ((insert = ttv_getpathnocross_v2 (fig, NULL, sig, NULL, TTV_DELAY_MAX, TTV_DELAY_MIN, type,1))) {
            mdmin = insert->MD;
            msmin = insert->MF;
            dmin  = insert->DELAY;
            smin  = insert->SLOPE;
            if(!msmin && mdmin){
                msmin = stm_mod_create_fcst (NULL, smin, STM_MOD_MODTBL);
                stm_mod_update (msmin, mdmin->VTH, mdmin->VDD, mdmin->VT, mdmin->VF);
            }
         }
         tma_CreatePath (blackbox, nodename, nodetype, rootname, roottype, mdmin, mdmax, 
                         msmin, msmax, dmin, dmax, smin, smax, 0);
         ttv_freepathlist(savinsert);
         ttv_freepathlist(insert);
      }
   }
}

/****************************************************************************/

int tma_IsInList (chain_list *list, char *name)
{
   chain_list *ptlist;

   for (ptlist = list; ptlist; ptlist = ptlist->NEXT) {
      if (ptlist->DATA == name)
         return 1;
   }
   return 0;
}

/****************************************************************************/

void tma_DetectClocksFromBeh (ttvfig_list *fig, befig_list *befig)
{
   int i;
   ttvsig_list  *sig;
   inffig_list *ifl=NULL;
   cbhseq       *ptcbhseq;
   chain_list   *clocklist = NULL;
   chain_list   *setlist = NULL;
   chain_list   *resetlist = NULL;

   initializeBdd (0);
   
   if (!befig)
      return;
   
   if (!(ptcbhseq = cbh_getseqfunc (befig, CBH_LIB_MODE))) {
      return;
   }

   fig->USER = addptype(fig->USER, TTV_FIG_CBHSEQ, ptcbhseq);

   if (ptcbhseq->CLOCK) {
      clocklist = supportChain_listExpr (ptcbhseq->CLOCK);
      if (ptcbhseq->SLAVECLOCK) {
         clocklist = append(clocklist, supportChain_listExpr (ptcbhseq->SLAVECLOCK));
      }
   }
   if (ptcbhseq->SET)
      setlist = supportChain_listExpr (ptcbhseq->SET);
   if (ptcbhseq->RESET)
      resetlist = supportChain_listExpr (ptcbhseq->RESET);

   for (i = 0; i < fig->NBCONSIG; i++) {
      sig = fig->CONSIG[i];
      if ((sig->TYPE & TTV_SIG_CI) == TTV_SIG_CI) {
         if (tma_IsInList (clocklist, sig->NAME))
            if (!getptype (sig->USER, TTV_SIG_CLOCK))
            {
              if ((ifl=getloadedinffig(fig->INFO->FIGNAME))==NULL)
                 ifl=addinffig(fig->INFO->FIGNAME);
              inf_AddInt(ifl, INF_LOADED_LOCATION, sig->NAME, INF_CLOCK_TYPE, 0, NULL);
            }
//		         sig->USER = addptype (sig->USER, TTV_SIG_CLOCK, 0) ;
         if (tma_IsInList (setlist, sig->NAME))
            if (!getptype (sig->USER, TTV_SIG_ASYNCHRON))
            {
              if ((ifl=getloadedinffig(fig->INFO->FIGNAME))==NULL)
                 ifl=addinffig(fig->INFO->FIGNAME);
              inf_AddString(ifl, INF_LOADED_LOCATION, sig->NAME, INF_ASYNCHRON, NULL, NULL);
            }
//		         sig->USER = addptype (sig->USER, TTV_SIG_ASYNCHRON, 0) ;
         if (tma_IsInList (resetlist, sig->NAME))
            if (!getptype (sig->USER, TTV_SIG_ASYNCHRON))
            {
              if ((ifl=getloadedinffig(fig->INFO->FIGNAME))==NULL)
                 ifl=addinffig(fig->INFO->FIGNAME);
              inf_AddString(ifl, INF_LOADED_LOCATION, sig->NAME, INF_ASYNCHRON, NULL, NULL);
            }
//         sig->USER = addptype (sig->USER, TTV_SIG_ASYNCHRON, 0) ;
      }
   }
   freechain(clocklist);
   freechain(setlist);
   freechain(resetlist);
   if (ifl!=NULL) ttv_getinffile(fig);
}

/****************************************************************************/

void tma_UpdateSetReset (ttvfig_list *fig, befig_list *befig)
{
   int          i, event;
   cbhseq       *ptcbhseq;
   chain_list   *setlist = NULL;
   chain_list   *resetlist = NULL;
   ttvsig_list  *sig;
   ttvline_list *line;
   chain_list   *dellist = NULL,
                *ptlist;
   ptype_list   *ptype;

   if (!befig)
      return;
   
   if((ptype = getptype(fig->USER, TTV_FIG_CBHSEQ)) == NULL) return;
   
   ptcbhseq = (cbhseq*)ptype->DATA;
   
   if (ptcbhseq->SET)
        setlist = supportChain_listExpr (ptcbhseq->SET);
   if (ptcbhseq->RESET)
        resetlist = supportChain_listExpr (ptcbhseq->RESET);

   for (i = 0; i < fig->NBCONSIG; i++) {
      sig = fig->CONSIG[i];
      if ((sig->TYPE & TTV_SIG_CO) == TTV_SIG_CO || (sig->TYPE & TTV_SIG_CB) == TTV_SIG_CB) {
         for (event = 0; event < 2; event++) {
            for (line = sig->NODE[event].INLINE; line; line = line->NEXT) {
               if (tma_IsInList (setlist, line->NODE->ROOT->NAME)) {
                  switch (cbh_calcsense (befig, ptcbhseq->SET, line->NODE->ROOT->NAME)) {
                     case CBH_INVERT :
                        if (tma_IsUp (line->NODE->TYPE))
                           dellist = addchain (dellist, line);
                        else {
                           if ((line->ROOT->ROOT->NAME == ptcbhseq->PIN) && (tma_IsDown (line->ROOT->TYPE)))
                              dellist = addchain (dellist, line);
                           else 
                           if ((line->ROOT->ROOT->NAME == ptcbhseq->NEGPIN) && (tma_IsUp (line->ROOT->TYPE)))
                              dellist = addchain (dellist, line);
                        }
                     break;
                     case CBH_NONINVERT :
                        if (tma_IsDown (line->NODE->TYPE))
                           dellist = addchain (dellist, line);
                        else {
                           if ((line->ROOT->ROOT->NAME == ptcbhseq->PIN) && (tma_IsDown (line->ROOT->TYPE)))
                              dellist = addchain (dellist, line);
                           else 
                           if ((line->ROOT->ROOT->NAME == ptcbhseq->NEGPIN) && (tma_IsUp (line->ROOT->TYPE)))
                              dellist = addchain (dellist, line);
                        }
                     break;
                  }
               }
               else
               if (tma_IsInList (resetlist, line->NODE->ROOT->NAME)) {
                  switch (cbh_calcsense (befig, ptcbhseq->RESET, line->NODE->ROOT->NAME)) {
                     case CBH_INVERT :
                        if (tma_IsUp (line->NODE->TYPE))
                           dellist = addchain (dellist, line);
                        else {
                           if ((line->ROOT->ROOT->NAME == ptcbhseq->PIN) && (tma_IsUp (line->ROOT->TYPE)))
                              dellist = addchain (dellist, line);
                           else 
                           if ((line->ROOT->ROOT->NAME == ptcbhseq->NEGPIN) && (tma_IsDown (line->ROOT->TYPE)))
                              dellist = addchain (dellist, line);
                        }
                     break;
                     case CBH_NONINVERT :
                        if (tma_IsDown (line->NODE->TYPE))
                           dellist = addchain (dellist, line);
                        else {
                           if ((line->ROOT->ROOT->NAME == ptcbhseq->PIN) && (tma_IsUp (line->ROOT->TYPE)))
                              dellist = addchain (dellist, line);
                           else 
                           if ((line->ROOT->ROOT->NAME == ptcbhseq->NEGPIN) && (tma_IsDown (line->ROOT->TYPE)))
                              dellist = addchain (dellist, line);
                        }
                     break;
                  }
               }
            }
         }
      }
   }
   for (i = 0; i < fig->NBCONSIG; i++) {
      sig = fig->CONSIG[i];
      if ((sig->TYPE & TTV_SIG_CO) == TTV_SIG_CO || (sig->TYPE & TTV_SIG_CB) == TTV_SIG_CB) {
         for (event = 0; event < 2; event++) {
            for (line = sig->NODE[event].INPATH; line; line = line->NEXT) {
               if (tma_IsInList (setlist, line->NODE->ROOT->NAME)) {
                  switch (cbh_calcsense (befig, ptcbhseq->SET, line->NODE->ROOT->NAME)) {
                     case CBH_INVERT :
                        if (tma_IsUp (line->NODE->TYPE))
                           dellist = addchain (dellist, line);
                        else {
                           if ((line->ROOT->ROOT->NAME == ptcbhseq->PIN) && (tma_IsDown (line->ROOT->TYPE)))
                              dellist = addchain (dellist, line);
                           else 
                           if ((line->ROOT->ROOT->NAME == ptcbhseq->NEGPIN) && (tma_IsUp (line->ROOT->TYPE)))
                              dellist = addchain (dellist, line);
                        }
                     break;
                     case CBH_NONINVERT :
                        if (tma_IsDown (line->NODE->TYPE))
                           dellist = addchain (dellist, line);
                        else {
                           if ((line->ROOT->ROOT->NAME == ptcbhseq->PIN) && (tma_IsDown (line->ROOT->TYPE)))
                              dellist = addchain (dellist, line);
                           else 
                           if ((line->ROOT->ROOT->NAME == ptcbhseq->NEGPIN) && (tma_IsUp (line->ROOT->TYPE)))
                              dellist = addchain (dellist, line);
                        }
                     break;
                  }
               }
               else
               if (tma_IsInList (resetlist, line->NODE->ROOT->NAME)) {
                  switch (cbh_calcsense (befig, ptcbhseq->RESET, line->NODE->ROOT->NAME)) {
                     case CBH_INVERT :
                        if (tma_IsUp (line->NODE->TYPE))
                           dellist = addchain (dellist, line);
                        else {
                           if ((line->ROOT->ROOT->NAME == ptcbhseq->PIN) && (tma_IsUp (line->ROOT->TYPE)))
                              dellist = addchain (dellist, line);
                           else 
                           if ((line->ROOT->ROOT->NAME == ptcbhseq->NEGPIN) && (tma_IsDown (line->ROOT->TYPE)))
                              dellist = addchain (dellist, line);
                        }
                     break;
                     case CBH_NONINVERT :
                        if (tma_IsDown (line->NODE->TYPE))
                           dellist = addchain (dellist, line);
                        else {
                           if ((line->ROOT->ROOT->NAME == ptcbhseq->PIN) && (tma_IsUp (line->ROOT->TYPE)))
                              dellist = addchain (dellist, line);
                           else 
                           if ((line->ROOT->ROOT->NAME == ptcbhseq->NEGPIN) && (tma_IsDown (line->ROOT->TYPE)))
                              dellist = addchain (dellist, line);
                        }
                     break;
                  }
               }
            }
         }
      }
   }
   for (ptlist = dellist; ptlist; ptlist = ptlist->NEXT) {
      line = (ttvline_list*)ptlist->DATA;
      ttv_delline (line);
   }
   freechain(setlist);
   freechain(resetlist);
   freechain (dellist);
}

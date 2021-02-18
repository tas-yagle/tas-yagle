/******************************************/
/* lib_func.c                             */
/******************************************/
/******************************************************************************/
/* INCLUDE                                                                    */
/******************************************************************************/
#include "lib_func.h"

extern eqt_ctx     *libEqtCtx;
/******************************************************************************/
/* FUNCTIONS                                                                  */
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/* LIB_FUNC_TREAT_CELL                                                        */
/* traitement a la fin d'un groupe "cell"                                     */
/*----------------------------------------------------------------------------*/
lofig_list   *lib_func_treat_cell(lib_group *g)
{
    char        *cellname;
    lofig_list  *ptrlofig;
    cbhseq      *ptrseq=NULL;
    char        mout='N';


    
    cellname  = namealloc ((char *)((g->NAMES)->DATA));
    
    if(!(ptrlofig = getloadedlofig(cellname))){
        ptrlofig = addlofig(cellname);
        ptrlofig->MODE = 'P';
        addcatalog(ptrlofig->NAME);
    }


    ptrseq = lib_func_create_cbhseq(g); 

    lib_func_treat_pin(cellname, g->SONS, ptrlofig, ptrseq, &mout);
    
    if(ptrseq && (mout == 'N')) {
        cbh_addseqtolofig(ptrlofig, ptrseq);
        if(LIB_TRACE_MODE == 'A')
            lib_aff_cbhseq(cbh_getseqfromlofig(ptrlofig));
    }

    if(mout == 'N') {
        cbh_classlofig(ptrlofig);
        return ptrlofig;
    }

    else
        return NULL;
    
}
/*----------------------------------------------------------------------------*/
/* LIB_FUNC_TREAT_PIN                                                         */
/*                                                                            */
/*----------------------------------------------------------------------------*/
void    lib_func_treat_pin (char *cellname, lib_group *g, lofig_list *plofig, cbhseq *pseq, char *m)
{
    lib_group   *p, *b;
    lib_s_attr  *a;
    char        *pinname, *radical, *style, name[512];
    char         dir, s_d = 'Y';
    lofig_list  *pl;
    int          bit_from, bit_to, i;
    char        *direction;
    
    
    pl = plofig;
    for(p=g ; p ; p=p->NEXT) {
        if(p->TYPE == BUS_TYPE) {
            radical = namealloc (p->NAMES->DATA);
            lib_func_get_bus_size (p, &bit_from, &bit_to);
            direction = lib_get_s_attr_val(p, SA_DIR);
            if (direction == NULL) { /* la direction n'est pas precisee dans le bus group */
                for (b=p->SONS ; b&&(direction==NULL) ; b=b->NEXT) {
                    if (b->TYPE == PIN_TYPE) {
                        /* la direction doit etre la meme pour tous les pins d'un bus */
                        /* la premiere direction trouvee est donc la bonne */
                        direction = lib_get_s_attr_val(b, SA_DIR);
                    }
                }
                /* sauvegarde de la direction dans le groupe bus */
                a = lib_create_s_attr(SA_DIR, strdup(direction));
                p->S_ATTR = addchain(p->S_ATTR, a);
            }
            
            dir = lib_get_char_dir(direction);
//            style = lib_get_s_attr_val (LIB_GP_HEAD, SA_BUS_NAMING_STYLE);
//            if (style) {
//                if ( strstr(style,"%s") < strstr(style,"%d") )
//                    s_d = 'Y';   /* le format est %s puis %d */
//                else
//                    s_d = 'N';   /* le format est %d puis %s */
//            }
 
            if (bit_from < bit_to)
                for (i = bit_from ; i <= bit_to ; i++ ) {
                /* get the pin name */
//                if (s_d == 'Y')
//                    sprintf (name, style, radical, i);
//                else
//                    sprintf (name, style, i, radical);
                sprintf (name, "%s %d", radical, i);
                pinname = namealloc (name);

               
                lib_func_complete_pin (cellname, pinname, plofig, p, pseq, m, &dir);
                }
            else
                for (i = bit_from ; i >= bit_to ; i-- ) {
                /* get the pin name */
//                if (s_d == 'Y')
//                    sprintf (name, style, radical, i);
//                else
//                    sprintf (name, style, i, radical);
                sprintf (name, "%s %d", radical, i);
                pinname = namealloc (name);

               
                lib_func_complete_pin (cellname, pinname, plofig, p, pseq, m, &dir);
                }

        }

        else if(p->TYPE == PIN_TYPE) {
            pinname = namealloc(p->NAMES->DATA);
            dir = lib_get_char_dir(lib_get_s_attr_val(p, SA_DIR));
            lib_func_complete_pin (cellname, pinname, plofig, p, pseq, m, &dir);
        }
    }
}
/*----------------------------------------------------------------------------*/
/* LIB_FUNC_COMPLETE_PIN                                                      */
/*                                                                            */
/*----------------------------------------------------------------------------*/
void    lib_func_complete_pin (char *cellname, char *pinname, lofig_list *plofig, lib_group *p, cbhseq *pseq, char *m, char *dir)
{
    cbhcomb     *pcomb = NULL;
    locon_list  *plocon;


    if((*dir == IO) || (*dir == OUT) || (*dir == TRI)) {
        pcomb = lib_func_create_cbhcomb(cellname,pinname, plofig, p, pseq, m);
        if(pcomb) { 
            if(pcomb->HZFUNC && *dir == OUT)
                *dir = TRI;
            else if(pcomb->HZFUNC && *dir == IO)
                *dir = IOT;
        } 
    }

    if(*m == 'N') 
        plocon = lib_func_complete_locon(pinname, plofig, *dir);

    if(pcomb) {
        cbh_addcombtolocon(plocon, pcomb);
        if(LIB_TRACE_MODE == 'A')
            lib_aff_cbhcomb(cbh_getcombfromlocon(plocon));
    }
}


/*----------------------------------------------------------------------------*/
/* LIB_FUNC_COMPLETE_LOFIG                                                    */
/*                                                                            */
/*----------------------------------------------------------------------------*/
locon_list  *lib_func_complete_locon(char *pname, lofig_list *plofig, char dir)
{
    locon_list  *c, *plocon=NULL;
    losig_list  *s, *plosig;
    int          i=0;
    
    if(dir != INT) {
        for(c=plofig->LOCON; (c && (strcasecmp(c->NAME,pname))) ; c=c->NEXT) ;
        if(!c) { /* connecteur inexistant */
            for (s=plofig->LOSIG; s ; s=s->NEXT) { 
                /* recherche du premier index libre */
                if(i<s->INDEX)
                    i=s->INDEX;
            }
            i++;

            plosig = addlosig(plofig, i, addchain(NULL, (void *)pname), 'E');
            plocon = addlocon(plofig, pname, plosig, dir);
        }
        else { /*locon et losig existent deja */
            plocon=c;
            plocon->DIRECTION = dir; /* redirection du connecteur */
        }
    }
    return plocon;
}



/*----------------------------------------------------------------------------*/
/* LIB_FUNC_CREATE_CBHCOMB                                                    */
/* Cree et remplit la cbhcomb pour la cellule.                                */
/*----------------------------------------------------------------------------*/
cbhcomb *lib_func_create_cbhcomb(char *cname, char *pname, lofig_list *plofig, lib_group *g, cbhseq *pseq, char *m)
{
    cbhcomb *comb;
    char *func, *hzfunc, *xfunc;
    char *str;
    
    
    comb = cbh_newcomb();
    comb->NAME = namealloc(pname);

    
    if((func = lib_get_s_attr_val(g, SA_FUNC))) {
        str = eqt_ConvertStr(func);
        comb->FUNCTION = eqt_StrToAbl(libEqtCtx, str);
        mbkfree (str);
        if(pseq) {
            if(!strcasecmp(func, pseq->NAME)) {
                if(!(pseq->PIN))
                    pseq->PIN = pname;
                else {
                    lib_func_multout(cname, plofig, pseq, comb);
                    *m = 'O';
                    return NULL;
                }
            }
            if(!strcasecmp(func, pseq->NEGNAME)) {
                if(!(pseq->NEGPIN))
                    pseq->NEGPIN = pname;
                else {
                    lib_func_multout(cname, plofig, pseq, comb);
                    *m = 'O';
                    return NULL;
                }
            }
        }
    }
    
    if((hzfunc = lib_get_s_attr_val(g, SA_TS_FUNC))) {
        str = eqt_ConvertStr(hzfunc);
        comb->HZFUNC = eqt_StrToAbl(libEqtCtx, str);
        mbkfree (str);
    }
    if((xfunc = lib_get_s_attr_val(g, SA_X_FUNC))) {
        str = eqt_ConvertStr(xfunc);
        comb->CONFLICT = eqt_StrToAbl(libEqtCtx, str);
        mbkfree (str);
    }
    return comb;
}

/*----------------------------------------------------------------------------*/
/* LIB_FUNC_MULTOUT                                                           */
/* Cree et remplit la cbhseq pour la cellule.                                 */
/*----------------------------------------------------------------------------*/
void    lib_func_multout(char *cellname, lofig_list *p, cbhseq *pseq, cbhcomb *pcomb)
{
    locon_list  *lc;
    cbhcomb     *c;
    
    if(LIB_TRACE_MODE == 'A')
        fprintf(stderr,"WARNING: multiple output for latch/register in cell %s\n", cellname);
    cbh_delcomb(pcomb);
    pcomb = NULL;
    cbh_delseq(pseq);
    pseq = NULL;
    for(lc=p->LOCON ; lc ; lc=lc->NEXT) {
        if((c = cbh_getcombfromlocon(lc))) {
            cbh_delcombtolocon(lc);
        }
    }
    p = p->NEXT ;
    dellofig(cellname);        
}

/*----------------------------------------------------------------------------*/
/* LIB_FUNC_CREATE_CBHSEQ                                                     */
/* Cree et remplit la cbhseq pour la cellule.                                 */
/*----------------------------------------------------------------------------*/
cbhseq  *lib_func_create_cbhseq(lib_group *g)
{
    lib_group   *p;
    chain_list  *sa;
    cbhseq      *seq;
    char        *str;


    if((p=lib_get_son_group(g,FF_TYPE))) {
        seq = cbh_newseq();
        seq->SEQTYPE = CBH_FLIPFLOP;
        seq->NAME = namealloc((char *)(p->NAMES->DATA));
        seq->NEGNAME = namealloc((char *)(p->NAMES->NEXT->DATA));
        for (sa=p->S_ATTR ; sa ; sa=sa->NEXT) {
            switch(((lib_s_attr *)(sa->DATA))->TYPE) {
                case SA_CK_ON:  
                    str = eqt_ConvertStr(((lib_s_attr *)(sa->DATA))->VALUE);
                    seq->CLOCK = eqt_StrToAbl(libEqtCtx, str);
                    mbkfree (str);
                    break;
                case SA_CK_ON_ALSO:  
                    str = eqt_ConvertStr(((lib_s_attr *)(sa->DATA))->VALUE);
                    seq->SLAVECLOCK = eqt_StrToAbl(libEqtCtx, str);
                    mbkfree (str);
                    break;
                case SA_NEXT_ST: 
                    str = eqt_ConvertStr(((lib_s_attr *)(sa->DATA))->VALUE);
                    seq->DATA = eqt_StrToAbl(libEqtCtx, str);
                    mbkfree (str);
                    break;
                case SA_CLEAR:  
                    str = eqt_ConvertStr(((lib_s_attr *)(sa->DATA))->VALUE);
                    seq->RESET = eqt_StrToAbl(libEqtCtx, str);
                    mbkfree (str);
                    break;
                case SA_PRESET:  
                    str = eqt_ConvertStr(((lib_s_attr *)(sa->DATA))->VALUE);
                    seq->SET = eqt_StrToAbl(libEqtCtx, str);
                    mbkfree (str);
                    break;
                case SA_CPVAR1:  
                    str = eqt_ConvertStr(((lib_s_attr *)(sa->DATA))->VALUE);
                    seq->RSCONF = eqt_StrToAbl(libEqtCtx, str);
                    mbkfree (str);
                    break;
                case SA_CPVAR2:  
                    str = eqt_ConvertStr(((lib_s_attr *)(sa->DATA))->VALUE);
                    seq->RSCONFNEG = eqt_StrToAbl(libEqtCtx, str);
                    mbkfree (str);
                    break;

                default:
                    break;
            }
        }
        lib_del_all_s_attr(p);
        return seq;
    }

    else if((p=lib_get_son_group(g,LATCH_TYPE))) {
        seq = cbh_newseq();
        seq->SEQTYPE = CBH_LATCH;
        seq->NAME = namealloc((char *)(p->NAMES->DATA));
        seq->NEGNAME = namealloc((char *)(p->NAMES->NEXT->DATA));
        for (sa=p->S_ATTR ; sa ; sa=sa->NEXT) {
            switch(((lib_s_attr *)(sa->DATA))->TYPE) {
                case SA_ENABLE: 
                    str = eqt_ConvertStr(((lib_s_attr *)(sa->DATA))->VALUE);
                    seq->CLOCK = eqt_StrToAbl(libEqtCtx, str);
                    mbkfree (str);
                    break;
                case SA_DATA:  
                    str = eqt_ConvertStr(((lib_s_attr *)(sa->DATA))->VALUE);
                    seq->DATA = eqt_StrToAbl(libEqtCtx, str);
                    mbkfree (str);
                    break;
                case SA_CLEAR:  
                    str = eqt_ConvertStr(((lib_s_attr *)(sa->DATA))->VALUE);
                    seq->RESET = eqt_StrToAbl(libEqtCtx, str);
                    mbkfree (str);
                    break;
                case SA_PRESET: 
                    str = eqt_ConvertStr(((lib_s_attr *)(sa->DATA))->VALUE);
                    seq->SET = eqt_StrToAbl(libEqtCtx, str);
                    mbkfree (str);
                    break;
                case SA_CPVAR1:  
                    str = eqt_ConvertStr(((lib_s_attr *)(sa->DATA))->VALUE);
                    seq->RSCONF = eqt_StrToAbl(libEqtCtx, str);
                    mbkfree (str);
                    break;
                case SA_CPVAR2:  
                    str = eqt_ConvertStr(((lib_s_attr *)(sa->DATA))->VALUE);
                    seq->RSCONFNEG = eqt_StrToAbl(libEqtCtx, str);
                    mbkfree (str);
                    break;

                default:
                    break;
            }
        }
        lib_del_all_s_attr(p);
        return seq;
    }

    else if((p=lib_get_son_group(g,STATETABLE_TYPE))) {
        seq = cbh_newseq();
        seq->SEQTYPE = CBH_UNKNOWN;
        lib_del_all_s_attr(p);
        return seq;
    }

    else
        return NULL;
}

/******************************************/
/* lib_timing.c                           */
/******************************************/
/******************************************************************************/
/* INCLUDE                                                                    */
/******************************************************************************/
#include "lib_timing.h"


#define     TYPE_STRING 'S'
#define     TYPE_FLOAT  'F'

/******************************************************************************/
/* GLOBALS                                                                    */
/******************************************************************************/
chain_list      *LIB_TTVFIG_LIST = NULL;

/******************************************************************************/
/* FUNCTIONS                                                                  */
/******************************************************************************/
/*----------------------------------------------------------------------------*/
/* LIB_TIM_GET_AXE_TYPE:                                                      */
/* renvoie le type des axes des templates                                     */
/*----------------------------------------------------------------------------*/
char lib_tim_get_axe_type(lib_group *g, long attr_type)
{
    char *attr_val;
    
    attr_val = lib_get_s_attr_val(g, attr_type);
    
    if(!attr_val)
        return STM_NOTYPE;
    
    else {
        if(!strcasecmp(attr_val,"input_net_transition"))
            return STM_INPUT_SLEW;
    
        else if(!strcasecmp(attr_val,"total_output_net_capacitance"))
            return STM_LOAD;
        
        else if(!strcasecmp(attr_val,"constrained_pin_transition"))
            return STM_INPUT_SLEW;
        
        else if(!strcasecmp(attr_val,"related_pin_transition"))
            return STM_CLOCK_SLEW;
    
        else
            return STM_NOTYPE;
    }
    
}



/*----------------------------------------------------------------------------*/
/* LIB_TIM_GET_AXE_VALUES :                                                   */
/* Renvoie la liste chainee contenant les valeurs des axes de template a partir*/
/* du groupe et de l'identficateur de l'attribut qui contient ces valeurs     */
/*----------------------------------------------------------------------------*/
chain_list  *lib_tim_get_axe_values(lib_group *g, long attr_type)
{
    chain_list *ch;

    ch = lib_get_c_attr_val(g, attr_type);
    if(ch) 
        return lib_strToChainFloat((char *)(ch->DATA));
    
    else
        return NULL;
}

/*----------------------------------------------------------------------------*/
/* LIB_TIM_TREAT_POWER_SUPPLY :                                               */
/*----------------------------------------------------------------------------*/
void lib_tim_treat_power_supply(void)
{
    chain_list *chain, *ca_chain;
    char *default_power_rail;
    char *name;
    float alim;

    default_power_rail = lib_get_s_attr_val(LIB_CURRENT_GP, SA_DEFAULT_POWER_RAIL);
    for(chain = LIB_CURRENT_GP->C_ATTR; chain; chain = chain->NEXT){
        if(((lib_c_attr*)chain->DATA)->TYPE == CA_POWER_RAIL){
            ca_chain = ((lib_c_attr*)chain->DATA)->VALUES;
            name = namealloc((char*)ca_chain->DATA);
            alim = (float) atof((char*)ca_chain->NEXT->DATA);
            libparserpowerptype = addptype(libparserpowerptype, (long)name, NULL);
            *(float*)&libparserpowerptype->DATA = alim;
        }
    }
}

/*----------------------------------------------------------------------------*/
/* LIB_TIM_GET_ALIM :                                                         */
/*----------------------------------------------------------------------------*/
float lib_tim_get_alim(char *name)
{
    ptype_list *ptype;

    for(ptype = libparserpowerptype; ptype; ptype = ptype->NEXT){
        if(!strcasecmp(name, (char*)ptype->TYPE)){
            return *(float*)&ptype->DATA;
        }
    }

    return -1.0;
}

/*----------------------------------------------------------------------------*/
/* LIB_TIM_STORE_TEMPLATE :                                                   */
/* cree et remplit un template dans stm                                       */
/*----------------------------------------------------------------------------*/
void    lib_tim_store_template(void)
{
    chain_list *xchain=NULL, *ychain=NULL, *ch, *pchn;
    int nx, ny;
    char xtype, ytype;
    char *tname;
    timing_ttable *ttemplate;

    xchain = lib_tim_get_axe_values(LIB_CURRENT_GP, CA_INDEX1);
    ychain = lib_tim_get_axe_values(LIB_CURRENT_GP, CA_INDEX2);

    xtype = lib_tim_get_axe_type(LIB_CURRENT_GP, SA_VAR1);
    ytype = lib_tim_get_axe_type(LIB_CURRENT_GP, SA_VAR2);


    for(nx=0, ch=xchain ; ch ; ch=ch->NEXT) nx++;
    for(ny=0, ch=ychain ; ch ; ch=ch->NEXT) ny++;

    tname = namealloc((char *)(LIB_CURRENT_GP->NAMES->DATA));

    ttemplate = stm_modtbl_addtemplate(tname, nx, ny, xtype, ytype);


    if((xtype == STM_INPUT_SLEW) || (xtype == STM_CLOCK_SLEW)) {
        if(LIB_TIME_U->UNIT == LIB_NS)
            stm_modtbl_settemplateXrange(ttemplate, xchain, LIB_TIME_U->FACTOR*1e3);
        else if(LIB_TIME_U->UNIT == LIB_PS)
            stm_modtbl_settemplateXrange(ttemplate, xchain, LIB_TIME_U->FACTOR*1);
    }
    if(xtype == STM_LOAD) {
        if(LIB_CAPA_U->UNIT == LIB_PF)
            stm_modtbl_settemplateXrange(ttemplate, xchain, LIB_CAPA_U->FACTOR*1e3);
        else if(LIB_CAPA_U->UNIT == LIB_FF)
            stm_modtbl_settemplateXrange(ttemplate, xchain, LIB_CAPA_U->FACTOR*1);
    }

    if((ytype == STM_INPUT_SLEW) || (ytype == STM_CLOCK_SLEW)) {
        if(LIB_TIME_U->UNIT == LIB_NS)
            stm_modtbl_settemplateYrange(ttemplate, ychain, LIB_TIME_U->FACTOR*1e3);
        else if(LIB_TIME_U->UNIT == LIB_PS)
            stm_modtbl_settemplateYrange(ttemplate, ychain, LIB_TIME_U->FACTOR*1);
    }
    if(ytype == STM_LOAD) {
        if(LIB_CAPA_U->UNIT == LIB_PF)
            stm_modtbl_settemplateYrange(ttemplate, ychain, LIB_CAPA_U->FACTOR*1e3);
        else if(LIB_CAPA_U->UNIT == LIB_FF)
            stm_modtbl_settemplateYrange(ttemplate, ychain, LIB_CAPA_U->FACTOR*1);
    }
    
    if(xchain) {
        for(pchn=xchain ; pchn ; pchn=pchn->NEXT)
            mbkfree(pchn->DATA);
        freechain(xchain);
    }
    if(ychain) {
        for(pchn=ychain ; pchn ; pchn=pchn->NEXT)
            mbkfree(pchn->DATA);
        freechain(ychain);
    }
    lib_del_all_s_attr(LIB_CURRENT_GP);
    lib_del_all_c_attr(LIB_CURRENT_GP);

//    if(LIB_TRACE_MODE == 'A')
  //      stm_modtbl_templateprint(stdout, ttemplate);
}
/*----------------------------------------------------------------------------*/
/* LIB_TIM_GIVE_SENSE:                                                        */
/* donne le mode de sortie: inverseuse ou non inverseuse                      */
/*----------------------------------------------------------------------------*/
char    lib_tim_give_sense(lib_group *g)
{
    char *str_sense;

    if((str_sense = lib_get_s_attr_val(g, SA_TIM_SENS))){
        if(!strcasecmp(str_sense,"positive_unate"))
            return LIB_POS;
        else if(!strcasecmp(str_sense,"negative_unate"))
            return LIB_NEG;
        else 
            return LIB_NON;
    }
    else
        return LIB_NON;
}
/*----------------------------------------------------------------------------*/
/* LIB_TIM_GIVE_TIMING_TYPE:                                                  */
/* donne le type de model: setup, hold ou edge.                               */
/*----------------------------------------------------------------------------*/
char    lib_tim_give_timing_type(lib_group *g)
{
    char *str_ttype;

    if((str_ttype = lib_get_s_attr_val(g, SA_TIM_TYPE))){
        if(!strcasecmp(str_ttype,"setup_rising"))
            return LIB_SETUP_RISE;
        else if(!strcasecmp(str_ttype,"setup_falling"))
            return LIB_SETUP_FALL;
        else if(!strcasecmp(str_ttype,"hold_rising"))
            return LIB_HOLD_RISE;
        else if(!strcasecmp(str_ttype,"hold_falling"))
            return LIB_HOLD_FALL;
        else if(!strcasecmp(str_ttype,"rising_edge"))
            return LIB_RISE_EDGE;
        else if(!strcasecmp(str_ttype,"falling_edge"))
            return LIB_FALL_EDGE;
        else if(!strcasecmp(str_ttype,"three_state_enable"))
            return LIB_TS_ENABLE;
        else if(!strcasecmp(str_ttype,"three_state_disable"))
            return LIB_TS_DISABLE;
        else 
            return LIB_NOTYPE;
    }
    else
        return LIB_NOTYPE;
}
/*----------------------------------------------------------------------------*/
/* LIB_TIM_GET_MODELNAME:                                                     */
/* cree le nom de model en fonction du groupe et du sense.                    */
/*----------------------------------------------------------------------------*/
char    *lib_tim_give_mname_with_number(char *cellname, char *prefixe, char *namebuf, char *suffixe)
{
    int     i;
    char    buf1[1020];
    char    buf2[1024]; /* taille de buf1 + 4 pour le '_' + 2 chiffres + '\0' */ 
    short   trouve=0;
    char   *mname;  

    sprintf(buf1, "%s__%s_%s",prefixe, namebuf, suffixe);
    
    for(i=1 ; !trouve ; i++) {
        sprintf(buf2, "%s_%d", buf1, i);
        if(!stm_getmodel(cellname, buf2))
            trouve = 1;
    }
            
    mname = (char *)mbkalloc(sizeof(char) * (strlen(buf2)+1));
    strcpy(mname, buf2); 
    return mname;
}



/*----------------------------------------------------------------------------*/
/* LIB_TIM_GET_MODELNAME_GROUP:                                               */
/* cree le nom de model en fonction du groupe et du positive unate.           */
/*----------------------------------------------------------------------------*/
char    *lib_tim_give_modelname_group(char *cellname, long gtype, char *namebuf, char tim_type, int itype, int otype)
{
    char *res;
    char *prefixe; 
    char *suffixe;


    switch(tim_type) {
        case LIB_RISE_EDGE  :
        case LIB_FALL_EDGE  :
            prefixe = (char *)mbkalloc(sizeof(char) * (strlen("access")+1));
            strcpy(prefixe,"access");
            break;

        case LIB_HOLD_RISE  :
        case LIB_HOLD_FALL  :
            prefixe = (char *)mbkalloc(sizeof(char) * (strlen("hold")+1));
            strcpy(prefixe, "hold");
            break;

        case LIB_SETUP_RISE :
        case LIB_SETUP_FALL :
            prefixe = (char *)mbkalloc(sizeof(char) * (strlen("setup")+1));
            strcpy(prefixe, "setup");
            break;

        
            
        default     :
            switch(gtype) {
                case DLY_RISE_TYPE  :
                case DLY_FALL_TYPE  :
                    prefixe = (char *)mbkalloc(sizeof(char) * (strlen("dly")+1));
                    strcpy(prefixe,"dly");
                    break;
                
                case SLW_RISE_TYPE  :
                case SLW_FALL_TYPE  :
                    prefixe = (char *)mbkalloc(sizeof(char) * (strlen("slw")+1));
                    strcpy(prefixe,"slw");
                    break;

                default :
                    prefixe = (char *)mbkalloc(sizeof(char) * (strlen("dly")+1));
                    strcpy(prefixe,"dly");
                    break;
            }
            break;
    }          
   
    suffixe = (char *) mbkalloc(sizeof(char) * 3);
    if(itype == elpRISE)
        strcpy(suffixe,"1");
    else if(itype == elpFALL)
        strcpy(suffixe,"0");
    else suffixe = NULL;

    
    if(suffixe) {
        if(otype == elpRISE)
            strcat(suffixe,"1");
        else if(otype == elpFALL)
            strcat(suffixe,"0");
        else suffixe = NULL;
    }

    
    if(prefixe && suffixe) {
        res = lib_tim_give_mname_with_number(cellname, prefixe, namebuf, suffixe);
      mbkfree(prefixe);
      mbkfree(suffixe);
    } 
    else res = NULL;
    
    return res;

}
/*----------------------------------------------------------------------------*/
/* LIB_TIM_GIVE_TBL_VALUES :                                                  */
/* renvoie la liste chainee contenant les valeurs des tables de modeles       */
/*----------------------------------------------------------------------------*/
chain_list  *lib_tim_give_tbl_values(chain_list *val)
{
    chain_list *ch, *yd, *xyd=NULL;
    
    if(!(val->NEXT)) /* table  1D */
        return (lib_strToChainFloat((char *)(val->DATA)));
    else {/* table  2D */
        for(ch = val ; ch ; ch=ch->NEXT) {
            yd = lib_strToChainFloat((char *)(ch->DATA));
            xyd = addchain(xyd,yd);
        }
        return reverse(xyd) ;
    }
            

}
/*----------------------------------------------------------------------------*/
/* LIB_TIM_STORE_WIRE_LOAD :                                                  */
/* cree et remplit un model de fanout_length                                  */
/*----------------------------------------------------------------------------*/
void lib_tim_store_default_wire_load(void)
{
    char *cname;
    char mcapname[1024];
    char mresname[1024];
    char *wireloadname;
    
    cname = (char*)LIB_CURRENT_GP->NAMES->DATA;
    wireloadname = (char*)lib_get_s_attr_val(LIB_CURRENT_GP, SA_DEFAULT_WIRE_LOAD);
    if(wireloadname){
        sprintf(mcapname,"%s_cap",wireloadname);
        sprintf(mresname,"%s_res",wireloadname);
        if(!STM_PROPERTIES)
            stm_prop_create (stm_getmodel(cname, mresname), stm_getmodel(cname, mcapname));
    }
}
/*----------------------------------------------------------------------------*/
/* LIB_TIM_STORE_WIRE_LOAD :                                                  */
/* cree et remplit un model de fanout_length                                  */
/*----------------------------------------------------------------------------*/
void lib_tim_store_wire_load(void)
{
    timing_model *m;
    chain_list *chain=NULL, *xchain=NULL, *xdata=NULL;
    int nx=0;
    char *cname;
    float *val;
    float scale;
    char mname[1024];

    for(chain = LIB_CURRENT_GP->C_ATTR; chain; chain = chain->NEXT){
        if(((lib_c_attr*)chain->DATA)->TYPE == CA_FANOUT_LENGTH){
            val = (float *) mbkalloc(sizeof(float));
            *val = atof(((lib_c_attr*)chain->DATA)->VALUES->DATA) + 1;
            xchain = addchain(xchain, (void *)val);
            val = (float *) mbkalloc(sizeof(float));
            *val = atof(((lib_c_attr*)chain->DATA)->VALUES->NEXT->DATA);
            xdata = addchain(xdata, (void *)val);
            nx++;
        }
    }
    cname = (char*)LIB_CURRENT_GP->OWNER->NAMES->DATA;
    /*Capacitance model*/
    sprintf(mname,"%s_cap",(char*)LIB_CURRENT_GP->NAMES->DATA);
    scale = atof(lib_get_s_attr_val(LIB_CURRENT_GP, SA_CAPA));
    if(LIB_CAPA_U->UNIT==LIB_PF)
        scale *= 1000 * LIB_CAPA_U->FACTOR;
    else if(LIB_CAPA_U->UNIT==LIB_FF)
        scale *= LIB_CAPA_U->FACTOR;
    m = stm_addtblmodel (cname, mname, nx, 0, STM_NOTYPE, STM_NOTYPE);
    stm_storemodel (cname, mname, m, 0);
    stm_modtbl_setXrange (m->UMODEL.TABLE, xchain, 1);
    stm_modtbl_set1Dset (m->UMODEL.TABLE, xdata, scale);
    
    /*Resistance model*/
    sprintf(mname,"%s_res",(char*)LIB_CURRENT_GP->NAMES->DATA);
    scale = atof(lib_get_s_attr_val(LIB_CURRENT_GP, SA_RESI));
    if(LIB_RES_U->UNIT==LIB_KOHM)
        scale *= 1000 * LIB_RES_U->FACTOR;
    else if(LIB_RES_U->UNIT==LIB_OHM)
        scale *= LIB_RES_U->FACTOR;
    m = stm_addtblmodel (cname, mname, nx, 0, STM_NOTYPE, STM_NOTYPE);
    stm_storemodel (cname, mname, m, 0);
    stm_modtbl_setXrange (m->UMODEL.TABLE, xchain, 1);
    stm_modtbl_set1Dset (m->UMODEL.TABLE, xdata, scale);

    for (chain=xchain; chain!=NULL; chain=chain->NEXT) mbkfree(chain->DATA);
    freechain(xchain);
    for (chain=xdata; chain!=NULL; chain=chain->NEXT) mbkfree(chain->DATA);
    freechain(xdata);
}
/*----------------------------------------------------------------------------*/
/* LIB_TIM_STORE_WIRE_LOAD_TABLE :                                            */
/* cree et remplit un model de fanout_length, fanout_capacitance et resistance*/
/*----------------------------------------------------------------------------*/
void lib_tim_store_wire_load_table(void)
{
    timing_model *m;
    chain_list *chain=NULL;
    chain_list *xchaincapa=NULL, *xdatacapa=NULL, *xchainres=NULL, *xdatares=NULL;
    int nxcapa=0, nxres=0;
    char *cname;
    float *val;
    float scale;
    char mname[1024];

    for(chain = LIB_CURRENT_GP->C_ATTR; chain; chain = chain->NEXT){
        if(((lib_c_attr*)chain->DATA)->TYPE == CA_FANOUT_CAPACITANCE){
            val = (float *) mbkalloc(sizeof(float));
            *val = atof(((lib_c_attr*)chain->DATA)->VALUES->DATA) + 1;
            xchaincapa = addchain(xchaincapa, (void *)val);
            val = (float *) mbkalloc(sizeof(float));
            *val = atof(((lib_c_attr*)chain->DATA)->VALUES->NEXT->DATA);
            xdatacapa = addchain(xdatacapa, (void *)val);
            nxcapa++;
        }
        if(((lib_c_attr*)chain->DATA)->TYPE == CA_FANOUT_RESISTANCE){
            val = (float *) mbkalloc(sizeof(float));
            *val = atof(((lib_c_attr*)chain->DATA)->VALUES->DATA) + 1;
            xchainres = addchain(xchainres, (void *)val);
            val = (float *) mbkalloc(sizeof(float));
            *val = atof(((lib_c_attr*)chain->DATA)->VALUES->NEXT->DATA);
            xdatares = addchain(xdatares, (void *)val);
            nxres++;
        }
    }
    cname = (char*)LIB_CURRENT_GP->OWNER->NAMES->DATA;
    /*Capacitance model*/
    if(xchaincapa){
        sprintf(mname,"%s_cap",(char*)LIB_CURRENT_GP->NAMES->DATA);
        if(LIB_CAPA_U->UNIT==LIB_PF)
            scale = 1000 * LIB_CAPA_U->FACTOR;
        else if(LIB_CAPA_U->UNIT==LIB_FF)
            scale = LIB_CAPA_U->FACTOR;
        m = stm_addtblmodel (cname, mname, nxcapa, 0, STM_NOTYPE, STM_NOTYPE);
        stm_storemodel (cname, mname, m, 0);
        stm_modtbl_setXrange (m->UMODEL.TABLE, xchaincapa, 1);
        stm_modtbl_set1Dset (m->UMODEL.TABLE, xdatacapa, scale);
    }
    
    /*Resistance model*/
    if(xchainres){
        sprintf(mname,"%s_res",(char*)LIB_CURRENT_GP->NAMES->DATA);
        if(LIB_RES_U->UNIT==LIB_KOHM)
            scale = 1000 * LIB_RES_U->FACTOR;
        else if(LIB_RES_U->UNIT==LIB_OHM)
            scale = LIB_RES_U->FACTOR;
        m = stm_addtblmodel (cname, mname, nxres, 0, STM_NOTYPE, STM_NOTYPE);
        stm_storemodel (cname, mname, m, 0);
        stm_modtbl_setXrange (m->UMODEL.TABLE, xchainres, 1);
        stm_modtbl_set1Dset (m->UMODEL.TABLE, xdatares, scale);
    }
}
/*----------------------------------------------------------------------------*/
/* LIB_TIM_STORE_MODEL_GROUP :                                                */
/* cree et remplit un model decrit dans un group dans stm                     */
/*----------------------------------------------------------------------------*/
timing_model    *lib_tim_store_model_group(char *cname, char *pname, lib_group *g, char ttype, int itype, int otype, char *relpin)
{
    timing_model *m=NULL;
    char *mname, *gname;
    timing_ttable *ttemplate;
    chain_list *xchain=NULL, *ychain=NULL;
    float const_val;
    char mnamebuf[512];
    chain_list  *ch, *xydata=NULL, *pchn, *pchn2;
    int nx, ny;
    char xtype, ytype;
    
    
    sprintf(mnamebuf, "%s/%s", relpin, pname);
            
    gname = (char *)(g->NAMES->DATA);
    mname = lib_tim_give_modelname_group(cname, g->TYPE, mnamebuf, ttype, itype, otype);
    ch = lib_get_c_attr_val(g, CA_VALUES);

    if(!strcasecmp(gname,"scalar")) {
        if(mname) {
            const_val = (float) atof(ch->DATA);
            m = stm_addtblmodel(cname, mname, 0, 0, STM_NOTYPE, STM_NOTYPE);

            if(LIB_TIME_U->UNIT == LIB_NS)
                stm_modtbl_setconst(m->UMODEL.TABLE, const_val*(LIB_TIME_U->FACTOR)*1e3);
            else if(LIB_TIME_U->UNIT == LIB_PS)
                stm_modtbl_setconst(m->UMODEL.TABLE, const_val*(LIB_TIME_U->FACTOR)*1);

            mbkfree(mname);

            
        }
    }

    else if((ttemplate = stm_modtbl_gettemplate(gname))) {
        if(mname) {
            xchain = lib_tim_get_axe_values(g, CA_INDEX1);
            ychain = lib_tim_get_axe_values(g, CA_INDEX2);

            if(!xchain || !ychain){
                m = stm_addmodel(cname, mname);
                m->UMODEL.TABLE = stm_modtbl_create_ftemplate(ttemplate);
                m->UTYPE = STM_MOD_MODTBL;
                stm_storemodel(cname, mname, m, 0);
            }else{
                for(nx=0, pchn=xchain ; pchn ; pchn=pchn->NEXT) nx++;
                for(ny=0, pchn=ychain ; pchn ; pchn=pchn->NEXT) ny++;
                m = stm_addmodel(cname, mname);
                m->UMODEL.TABLE = stm_modtbl_create (nx, ny, ttemplate->XTYPE, ttemplate->YTYPE);
                m->UTYPE = STM_MOD_MODTBL;
                stm_storemodel(cname, mname, m, 0);
            }
           
            if (xchain) {
                if((ttemplate->XTYPE == STM_INPUT_SLEW) || (ttemplate->XTYPE == STM_CLOCK_SLEW)) {
                    if(LIB_TIME_U->UNIT == LIB_NS)
                        stm_modtbl_setXrange(m->UMODEL.TABLE, xchain, (LIB_TIME_U->FACTOR * 1e3));
                    else if(LIB_TIME_U->UNIT == LIB_PS)
                        stm_modtbl_setXrange(m->UMODEL.TABLE, xchain, (LIB_TIME_U->FACTOR * 1));
                }
                
                if(ttemplate->XTYPE == STM_LOAD) {
                    if(LIB_CAPA_U->UNIT == LIB_PF)
                        stm_modtbl_setXrange(m->UMODEL.TABLE, xchain, (LIB_CAPA_U->FACTOR * 1e3));
                    else if(LIB_CAPA_U->UNIT == LIB_FF)
                        stm_modtbl_setXrange(m->UMODEL.TABLE, xchain, (LIB_CAPA_U->FACTOR * 1));
                }
            }
            
            if (ychain) {
                if((ttemplate->YTYPE == STM_INPUT_SLEW) || (ttemplate->YTYPE == STM_CLOCK_SLEW)) {
                    if(LIB_TIME_U->UNIT == LIB_NS)
                        stm_modtbl_setYrange(m->UMODEL.TABLE, ychain, (LIB_TIME_U->FACTOR * 1e3));
                    else if(LIB_TIME_U->UNIT == LIB_PS)
                        stm_modtbl_setYrange(m->UMODEL.TABLE, ychain, (LIB_TIME_U->FACTOR * 1));
                }
                
                if(ttemplate->YTYPE == STM_LOAD) {
                    if(LIB_CAPA_U->UNIT == LIB_PF)
                        stm_modtbl_setYrange(m->UMODEL.TABLE, ychain, (LIB_CAPA_U->FACTOR * 1e3));
                    else if(LIB_CAPA_U->UNIT == LIB_FF)
                        stm_modtbl_setYrange(m->UMODEL.TABLE, ychain, (LIB_CAPA_U->FACTOR * 1));
                }
            }

            xydata = lib_tim_give_tbl_values(ch);
            
            if(!(ch->NEXT)) {/* table 1D */
                if(LIB_TIME_U->UNIT == LIB_NS) 
                    stm_modtbl_set1Dset(m->UMODEL.TABLE, xydata, (LIB_TIME_U->FACTOR * 1e3));
                else if(LIB_TIME_U->UNIT == LIB_PS) 
                    stm_modtbl_set1Dset(m->UMODEL.TABLE, xydata, (LIB_TIME_U->FACTOR * 1));
                for(pchn=xydata ; pchn ; pchn=pchn->NEXT)
                    mbkfree(pchn->DATA);
                freechain(xydata);
                        
//                if(LIB_TRACE_MODE == 'A') 
  //                  stm_modtbl_print(stdout,(stm_getmodel(cname, mname))->UMODEL.TABLE);
            }
            else { /*table 2D*/
                if(LIB_TIME_U->UNIT == LIB_NS) 
                    stm_modtbl_set2Dset(m->UMODEL.TABLE, xydata, (LIB_TIME_U->FACTOR * 1e3));
                else if(LIB_TIME_U->UNIT == LIB_PS) 
                    stm_modtbl_set2Dset(m->UMODEL.TABLE, xydata, (LIB_TIME_U->FACTOR * 1));
                  for(pchn=xydata ; pchn ; pchn=pchn->NEXT){
                    for(pchn2=pchn->DATA; pchn2 ; pchn2=pchn2->NEXT)
                        mbkfree(pchn2->DATA);
                    freechain(pchn->DATA);
                  }
                  freechain(xydata);
            }
//            lib_del_all_s_attr(g);
//            lib_del_all_c_attr(g);

            if(xchain) {
                for(pchn=xchain; pchn ; pchn=pchn->NEXT)
                    mbkfree(pchn->DATA);
                freechain(xchain);
            }
            if(ychain) {
                for(pchn=ychain; pchn ; pchn=pchn->NEXT)
                    mbkfree(pchn->DATA);
                freechain(ychain);
            }
            
            
            mbkfree(mname);
        }
    }

    else if((g->TYPE == SLW_RISE_TYPE) || (g->TYPE == SLW_FALL_TYPE)) {
        /* groupes dans lesquels on peut decrire un modele sans template */
        /* le type des axes est obligatoirement STM_INPUT_SLEW et STM_LOAD */
        mname = gname;
        
        xchain = lib_tim_get_axe_values(g, CA_INDEX1);
        ychain = lib_tim_get_axe_values(g, CA_INDEX2);

        if(xchain) {
            if(ychain) {
                xtype = STM_INPUT_SLEW;
                ytype = STM_LOAD;
            }
            else
                xtype = STM_LOAD;
                ytype = STM_NOTYPE;
        }
        else {
            xtype = STM_NOTYPE;
            ytype = STM_NOTYPE;
        }
        
        for(nx=0, pchn=xchain ; pchn ; pchn=pchn->NEXT) nx++;
        for(ny=0, pchn=ychain ; pchn ; pchn=pchn->NEXT) ny++;
        
        xydata = lib_tim_give_tbl_values(ch);                
        
        m = stm_addtblmodel(cname, mname, nx, ny, xtype, ytype);

        
        if(xtype == STM_INPUT_SLEW) {
            if(LIB_TIME_U->UNIT == LIB_NS)
                stm_modtbl_setXrange(m->UMODEL.TABLE, xchain, (LIB_TIME_U->FACTOR * 1e3));
            else if(LIB_TIME_U->UNIT == LIB_PS)
                stm_modtbl_setXrange(m->UMODEL.TABLE, xchain, (LIB_TIME_U->FACTOR * 1));
        }
        else if(xtype == STM_LOAD) {
            if(LIB_CAPA_U->UNIT == LIB_PF)
                stm_modtbl_setXrange(m->UMODEL.TABLE, xchain, (LIB_CAPA_U->FACTOR * 1e3));
            else if(LIB_CAPA_U->UNIT == LIB_FF)
                stm_modtbl_setXrange(m->UMODEL.TABLE, xchain, (LIB_CAPA_U->FACTOR * 1));
        }
 
        
        if(ytype == STM_LOAD) {
            if(LIB_CAPA_U->UNIT == LIB_PF)
                stm_modtbl_setYrange(m->UMODEL.TABLE, ychain, (LIB_CAPA_U->FACTOR * 1e3));
            else if(LIB_CAPA_U->UNIT == LIB_FF)
                stm_modtbl_setYrange(m->UMODEL.TABLE, ychain, (LIB_CAPA_U->FACTOR * 1));
        }

        
        if(!(ch->NEXT)) {/* table 1D */
            if(LIB_TIME_U->UNIT == LIB_NS) 
                stm_modtbl_set1Dset(m->UMODEL.TABLE, xydata, (LIB_TIME_U->FACTOR * 1e3));
            else if(LIB_TIME_U->UNIT == LIB_PS) 
                stm_modtbl_set1Dset(m->UMODEL.TABLE, xydata, (LIB_TIME_U->FACTOR * 1));
            for(pchn=xydata ; pchn ; pchn=pchn->NEXT)
                mbkfree(pchn->DATA);
            freechain(xydata);
        }
        
        else { /*table 2D*/
            if(LIB_TIME_U->UNIT == LIB_NS) 
                stm_modtbl_set2Dset(m->UMODEL.TABLE, xydata, (LIB_TIME_U->FACTOR * 1e3));
            else if(LIB_TIME_U->UNIT == LIB_PS) 
                stm_modtbl_set2Dset(m->UMODEL.TABLE, xydata, (LIB_TIME_U->FACTOR * 1));
            for(pchn=xydata ; pchn ; pchn=pchn->NEXT){
                for(pchn2=pchn->DATA; pchn2 ; pchn2=pchn2->NEXT)
                    mbkfree(pchn2->DATA);
                freechain(pchn->DATA);
            }
            freechain(xydata);        
        }
        
//        lib_del_all_s_attr(g);
//        lib_del_all_c_attr(g);

        if(xchain) {
            for(pchn=xchain; pchn ; pchn=pchn->NEXT)
                mbkfree(pchn->DATA);
            freechain(xchain);
        }
        if(ychain) {
            for(pchn=ychain; pchn ; pchn=pchn->NEXT)
                mbkfree(pchn->DATA);
            freechain(ychain);
        }

        
            mbkfree(mname);
        
    }
    
    return  m;
}

            
/*----------------------------------------------------------------------------*/
/* LIB_TIM_SUPPRESS_CONFLICT :                                                */
/* Si un timing group de timing type threestate existe dans un pin, cette     */
/* fonction supprime les timing group d'un autre timing type, pour le meme    */
/* related pin.                                                               */
/* Suppression du conflit des timing groupe threestate et combinational pour  */
/* le meme related pin.                                                       */
/*----------------------------------------------------------------------------*/
void     lib_tim_suppress_conflict (lib_group *g)
{
    lib_group *son, *son2;
    char       ttype, ttype2;
    char      *relpin, *relpin2;


    /* detects if there is a timing group with a timig type 'three_state_enable or disable' */
    for (son=g->SONS; son ; son=son->NEXT) {
        if (son->TYPE==TIMING_TYPE) {
            ttype = lib_tim_give_timing_type(son);
            if ((ttype==LIB_TS_ENABLE) || (ttype==LIB_TS_DISABLE)) {
                relpin = namealloc(lib_get_s_attr_val(son, SA_REL_PIN));
                son2=g->SONS;
                while (son2) {
                    if (son2->TYPE==TIMING_TYPE) {
                        relpin2 = namealloc(lib_get_s_attr_val(son2, SA_REL_PIN));
                        if (relpin2 == relpin) {
                            ttype2 = lib_tim_give_timing_type(son2);
                            if ((ttype2!=LIB_TS_ENABLE) && (ttype2!=LIB_TS_DISABLE)) {
                                son2 = lib_del_group (son2);
                            }
                            else 
                                son2 = son2->NEXT;
                        }
                        else 
                            son2 = son2->NEXT;
                    }
                    else 
                        son2 = son2->NEXT;
                }
            }
        }
    }
            
    


}

/*----------------------------------------------------------------------------*/
/* LIB_TIM_TREAT_PIN :                                                        */
/* Traitement de fin de cellule.                                              */
/*----------------------------------------------------------------------------*/
void     lib_tim_treat_pin(lib_group *g, ttvfig_list *f, char *cellname, ht *pinht, ht *pincapaht)
{
    lib_group   *p;
    char         ttype;
    
    //lib_tim_suppress_conflict (g);
    for(p=g->SONS ; p ; p=p->NEXT) {
        if(p->TYPE == TIMING_TYPE) 
            lib_tim_addcaracline(p, f, cellname, namealloc((char *)(g->NAMES->DATA)), pinht, pincapaht);
    }

}
    
/*----------------------------------------------------------------------------*/
/* LIB_TIM_ADD_SWING :                                                        */
/*----------------------------------------------------------------------------*/
void     lib_tim_add_swing(lib_group *g, ttvfig_list *f, ht* pht)
{
    int i=0;
    ttvsig_list *ttvsig;
    chain_list *chain, *ca_chain;
    lib_group *p, *t;
    float alim;
    char *str, *name;
    
    for(chain = g->C_ATTR; chain; chain = chain->NEXT){
        if(((lib_c_attr*)chain->DATA)->TYPE == CA_RAIL_CONNECTION){
            i++;
            ca_chain = ((lib_c_attr*)chain->DATA)->VALUES;
        }
    }

    for (p=g->SONS ; p ; p=p->NEXT) {
        if (p->TYPE==PIN_TYPE){
            if(i == 0){
                ttvsig = (ttvsig_list *)gethtitem(pht, namealloc((char *)(p->NAMES->DATA)));
                if((str = lib_get_s_attr_val(p, SA_OUTPUT_SIGNAL_LEVEL))) {
                    name = str;
                    alim = lib_tim_get_alim(name);
                    if(alim >=0.0) ttv_set_signal_swing(ttvsig, 0.0, alim);
                }else if((str = lib_get_s_attr_val(p, SA_INPUT_SIGNAL_LEVEL))) {
                    name = str;
                    alim = lib_tim_get_alim(name);
                    if(alim >=0.0) ttv_set_signal_swing(ttvsig, 0.0, alim);
                }
            }else if(i == 1){
                ttvsig = (ttvsig_list *)gethtitem(pht, namealloc((char *)(p->NAMES->DATA)));
                name = (char*)ca_chain->NEXT->DATA;
                alim = lib_tim_get_alim(name);
                if(alim >=0.0) ttv_set_signal_swing(ttvsig, 0.0, alim);
            }else if(i > 1){
                ttvsig = (ttvsig_list *)gethtitem(pht, namealloc((char *)(p->NAMES->DATA)));
                if((str = lib_get_s_attr_val(p, SA_OUTPUT_SIGNAL_LEVEL))) {
                    name = str;
                }else if((str = lib_get_s_attr_val(p, SA_INPUT_SIGNAL_LEVEL))) {
                    name = str;
                }
                alim = lib_tim_get_alim(name);
                if(alim >=0.0) ttv_set_signal_swing(ttvsig, 0.0, alim);
            }
        }else if (p->TYPE==BUS_TYPE) {
            for (t=p->SONS ; t ; t=t->NEXT){
                if (t->TYPE==PIN_TYPE){
                    if(i == 0){
                        ttvsig = (ttvsig_list *)gethtitem(pht, namealloc((char *)(t->NAMES->DATA)));
                        if((str = lib_get_s_attr_val(t, SA_OUTPUT_SIGNAL_LEVEL))) {
                            name = str;
                            alim = lib_tim_get_alim(name);
                            if(alim >=0.0) ttv_set_signal_swing(ttvsig, 0.0, alim);
                        }else if((str = lib_get_s_attr_val(t, SA_INPUT_SIGNAL_LEVEL))) {
                            name = str;
                            alim = lib_tim_get_alim(name);
                            if(alim >=0.0) ttv_set_signal_swing(ttvsig, 0.0, alim);
                        }
                    }else if(i == 1){
                        ttvsig = (ttvsig_list *)gethtitem(pht, namealloc((char *)(t->NAMES->DATA)));
                        name = (char*)ca_chain->NEXT->DATA;
                        alim = lib_tim_get_alim(name);
                        if(alim >=0.0) ttv_set_signal_swing(ttvsig, 0.0, alim);
                    }else if(i > 1){
                        ttvsig = (ttvsig_list *)gethtitem(pht, namealloc((char *)(t->NAMES->DATA)));
                        if((str = lib_get_s_attr_val(t, SA_OUTPUT_SIGNAL_LEVEL))) {
                            name = str;
                        }else if((str = lib_get_s_attr_val(t, SA_INPUT_SIGNAL_LEVEL))) {
                            name = str;
                        }
                        alim = lib_tim_get_alim(name);
                        if(alim >=0.0) ttv_set_signal_swing(ttvsig, 0.0, alim);
                    }
                }
            }
        }
    }
}

/*----------------------------------------------------------------------------*/
/* LIB_TIM_TREAT_ENDOFCELL :                                                  */
/* Traitement de fin de cellule.                                              */
/*----------------------------------------------------------------------------*/
void     lib_tim_treat_cell(lib_group *g, lofig_list *plofig)
{
    lib_group *p, *t;
    char *cellname, *val;
    ttvfig_list *f;
    ht *pinht, *pincapaht;
    chain_list *clockch;
    //chain_list *ch;
    long    i;
    htitem *titem;
    char version[20];
    char *radical, *style, name[512], *pinname, s_d;
    int   bit_from, bit_to;
    float thfallhigh, thfalllow, thrisehigh, thriselow;
    float inputdthrise, inputdthfall, outputdthrise, outputdthfall;
    float power_supply, temp;
    void *nextkey ;
    long nextitem ;

    thfallhigh = LIB_DEFAULT_SA_SLEW_UP_FALL_UNIT;
    thfalllow = LIB_DEFAULT_SA_SLEW_LOW_FALL_UNIT;
    thrisehigh = LIB_DEFAULT_SA_SLEW_UP_RISE_UNIT;
    thriselow = LIB_DEFAULT_SA_SLEW_LOW_RISE_UNIT;
    inputdthrise = LIB_DEFAULT_SA_INPUT_DTH_RISE;
    inputdthfall = LIB_DEFAULT_SA_INPUT_DTH_FALL;
    outputdthrise = LIB_DEFAULT_SA_OUTPUT_DTH_RISE;
    outputdthfall = LIB_DEFAULT_SA_OUTPUT_DTH_FALL;
    power_supply = LIB_DEFAULT_SA_NOM_VOLTAGE;
    temp = LIB_DEFAULT_SA_NOM_TEMP;
    
    if ((val=lib_get_s_attr_val(LIB_GP_HEAD, SA_SLEW_UP_FALL_UNIT))!=NULL)
      thfallhigh = atof(val)/100;
    if ((val=lib_get_s_attr_val(LIB_GP_HEAD, SA_SLEW_LOW_FALL_UNIT))!=NULL)
      thfalllow = atof(val)/100;
    if ((val=lib_get_s_attr_val(LIB_GP_HEAD, SA_SLEW_UP_RISE_UNIT))!=NULL)
      thrisehigh = atof(val)/100;
    if ((val=lib_get_s_attr_val(LIB_GP_HEAD, SA_SLEW_LOW_RISE_UNIT))!=NULL)
      thriselow = atof(val)/100;
    if ((val=lib_get_s_attr_val(LIB_GP_HEAD, SA_INPUT_DTH_RISE))!=NULL)
      inputdthrise = atof(val)/100;
    if ((val=lib_get_s_attr_val(LIB_GP_HEAD, SA_INPUT_DTH_FALL))!=NULL)
      inputdthfall = atof(val)/100;
    if ((val=lib_get_s_attr_val(LIB_GP_HEAD, SA_OUTPUT_DTH_RISE))!=NULL)
      outputdthrise = atof(val)/100;
    if ((val=lib_get_s_attr_val(LIB_GP_HEAD, SA_OUTPUT_DTH_FALL))!=NULL)
      outputdthfall = atof(val)/100;
    if ((val=lib_get_s_attr_val(LIB_GP_HEAD, SA_NOM_VOLTAGE))!=NULL)
      power_supply = atof(val);
    if ((val=lib_get_s_attr_val(LIB_GP_HEAD, SA_NOM_TEMP))!=NULL)
      temp = atof(val);
     /* 
    thfallhigh = (atof(lib_get_s_attr_val(LIB_GP_HEAD, SA_SLEW_UP_FALL_UNIT)))/100;
    thfalllow = (atof(lib_get_s_attr_val(LIB_GP_HEAD, SA_SLEW_LOW_FALL_UNIT)))/100;
    thrisehigh = (atof(lib_get_s_attr_val(LIB_GP_HEAD, SA_SLEW_UP_RISE_UNIT)))/100;
    thriselow = (atof(lib_get_s_attr_val(LIB_GP_HEAD, SA_SLEW_LOW_RISE_UNIT)))/100;
    inputdthrise = (atof(lib_get_s_attr_val(LIB_GP_HEAD, SA_INPUT_DTH_RISE)))/100;
    inputdthfall = (atof(lib_get_s_attr_val(LIB_GP_HEAD, SA_INPUT_DTH_FALL)))/100;
    outputdthrise = (atof(lib_get_s_attr_val(LIB_GP_HEAD, SA_OUTPUT_DTH_RISE)))/100;
    outputdthfall = (atof(lib_get_s_attr_val(LIB_GP_HEAD, SA_OUTPUT_DTH_FALL)))/100;
    power_supply = (atof(lib_get_s_attr_val(LIB_GP_HEAD, SA_NOM_VOLTAGE)));
    temp = (atof(lib_get_s_attr_val(LIB_GP_HEAD, SA_NOM_TEMP)));
    */
    if((thfallhigh != thrisehigh) || (thfalllow != thriselow))
        fprintf (stderr, "***lib warning: slew thresholds are not the same for rising and falling***\n"); 
    if((inputdthrise != inputdthfall) || (inputdthrise != outputdthrise) || (inputdthrise != outputdthfall))
        fprintf (stderr, "***lib warning: input and output thresholds are not the same***\n"); 

    pinht = addht(100);
    pincapaht = addht(100);
    
    cellname = namealloc((char *)(g->NAMES->DATA));
    stm_addcell(cellname);
    
    
    sprintf(version, "%.2f", elpTechnoVersion);
    
    f = ttv_givehead(cellname, cellname, NULL);
    ttv_lockttvfig(f);
    ttv_setttvlevel(f);
    LIB_TTVFIG_LIST = addchain(LIB_TTVFIG_LIST, f);
    
    f->INFO->TOOLNAME       =   "lib2ttv";
    f->INFO->TOOLVERSION    =   "1.0";
    if (elpTechnoName[0] == '\0') 
      f->INFO->TECHNONAME     =   namealloc("unknown");
    else
      f->INFO->TECHNONAME     =   namealloc(elpTechnoName);
    f->INFO->TECHNOVERSION  =   namealloc(version);
    f->INFO->SLOPE          =   STM_DEF_SLEW * TTV_UNIT;
    f->INFO->CAPAOUT        =   STM_DEF_LOAD;
    f->INFO->STHHIGH        =   V_FLOAT_TAB[ __SIM_VTH_HIGH ].VALUE;
    f->INFO->STHLOW         =   V_FLOAT_TAB[ __SIM_VTH_LOW ].VALUE;
	f->INFO->DTH            =   outputdthrise;
	f->INFO->TEMP           =   temp;
	f->INFO->VDD            =   power_supply;

    ttv_setttvdate(f, TTV_DATE_LOCAL);
    
    
    clockch = lib_tim_addrefsig(g->SONS, f, pinht, pincapaht); 

    lib_tim_add_swing(g, f, pinht); 

    for (p=g->SONS ; p ; p=p->NEXT) {
        if (p->TYPE==PIN_TYPE)
            lib_tim_treat_pin (p, f, cellname, pinht, pincapaht);
        else if (p->TYPE==BUS_TYPE) 
            for (t=p->SONS ; t ; t=t->NEXT)
                if (t->TYPE==PIN_TYPE)
                    lib_tim_treat_pin (t, f, cellname, pinht, pincapaht);
    }

    lib_tim_getsigtype(f, plofig);
    lib_tim_allocbreaklatch(f);
    f->STATUS |= TTV_STS_DTX;
    f->STATUS |= TTV_STS_TTX;
   
    scanhtkey( pincapaht, 1, &nextkey, &nextitem );
    while( nextitem != EMPTYHT ) {
      mbkfree( (void *)nextitem );
      scanhtkey( pincapaht, 0, &nextkey, &nextitem );
    }
    delht(pincapaht);
    delht(pinht);
 //   for(ch = clockch ; ch ; ch=ch->NEXT)
 //       mbkfree(ch->DATA);
    freechain(clockch);

}
            
/*----------------------------------------------------------------------------*/
/* LIB_TIM_ADDREFSIG :                                                        */
/* construit la ttvsig du connecteur.                                         */
/*----------------------------------------------------------------------------*/
chain_list  *lib_tim_addrefsig(lib_group *g, ttvfig_list *fig, ht *pht, ht *pcapht)
{
    lib_group *p;
    char *pinname;
    char pdir;
    long dir;
    char *pcap;
    float *capa, *cp;
    chain_list *consigchain=NULL, *breaksigchain=NULL, *ckch=NULL;
    int nbconsig=0, nbbreaksig=0;
    int i;
    char *str;
    char *radical, *style, name[512], s_d, bufstyle[1024];
    int   bit_from, bit_to;

    for (p=g ; p ; p=p->NEXT) {
        if((p->TYPE == PIN_TYPE) || (p->TYPE == BUS_TYPE)) {
            pdir = lib_get_char_dir(lib_get_s_attr_val(p,SA_DIR));
            dir = lib_tim_get_long_dir(pdir, lib_get_s_attr_val(p,SA_TS_FUNC));
            if(LIB_TRACE_MODE == 'A')
                printf("PINNAME: %s DIR: %ld\n",pinname, dir);

            if(pdir != INT) {
                capa = (float *) mbkalloc (sizeof(float));
                if((pcap = lib_get_s_attr_val(p,SA_CAPA))) {
                    if(LIB_CAPA_U->UNIT == LIB_PF)
                        *capa = ((float) atof(pcap))*LIB_CAPA_U->FACTOR*1e3;
                    else if(LIB_CAPA_U->UNIT == LIB_FF)
                        *capa = ((float) atof(pcap))*LIB_CAPA_U->FACTOR*1;
                }

                else
                    *capa = 0;

                if (p->TYPE == PIN_TYPE) {
                    pinname = namealloc((char *)(p->NAMES->DATA));
                    if((str = lib_get_s_attr_val(p, SA_CK))) {
                        if(!strcasecmp(str,"true")) 
                            ckch = addchain(ckch, namealloc(pinname));
                    }
                    addhtitem(pcapht, namealloc(pinname), (long)capa);
                    consigchain = ttv_addrefsig(fig, namealloc(pinname), namealloc(pinname), *capa, dir, consigchain);
                    nbconsig++;
                }

                else { /* bus */
                    radical = namealloc (p->NAMES->DATA);
                    lib_func_get_bus_size (p, &bit_from, &bit_to);
                    style = lib_get_s_attr_val (LIB_GP_HEAD, SA_BUS_NAMING_STYLE);
                    if (!style){
                        if(LIB_BUS_DELIM[1] != '\0')
                            sprintf(bufstyle, "%%s%c%%d%c", LIB_BUS_DELIM[0], LIB_BUS_DELIM[1]);
                        else
                            sprintf(bufstyle, "%%s%c%%d", LIB_BUS_DELIM[0]);
                        style = bufstyle;
                    }
                    if (style) {
                        if ( strstr(style,"%s") < strstr(style,"%d") )
                            s_d = 'Y';   /* le format est %s puis %d */
                        else
                            s_d = 'N';   /* le format est %d puis %s */
                    }

                    if (bit_from < bit_to) {
                        for (i = bit_to ; i >= bit_from ; i-- ) {
                            cp = (float*) mbkalloc (sizeof(float));
                            *cp = *capa;
                            /* get the pin name */
                            if (s_d == 'Y')
                                sprintf (name, style, radical, i);
                            else
                                sprintf (name, style, i, radical);
                            pinname = namealloc (name);

                            addhtitem(pcapht, namealloc(pinname), (long)cp);
                            consigchain = ttv_addrefsig(fig, namealloc(pinname), namealloc(pinname), *cp, dir, consigchain);
                            nbconsig++;
                        }
                        mbkfree (capa);
                    }
                    else {
                        for (i = bit_to ; i <= bit_from ; i++ ) {
                            cp = (float*) mbkalloc (sizeof(float));
                            *cp = *capa;
                            /* get the pin name */
                            if (s_d == 'Y')
                                sprintf (name, style, radical, i);
                            else
                                sprintf (name, style, i, radical);
                            pinname = namealloc (name);

                            addhtitem(pcapht, namealloc(pinname), (long)cp);
                            consigchain = ttv_addrefsig(fig, namealloc(pinname), namealloc(pinname), *cp, dir, consigchain);
                            nbconsig++;
                        }
                        mbkfree (capa);
                    }
                }
            }else{
                capa = (float *) mbkalloc (sizeof(float));
                if((pcap = lib_get_s_attr_val(p,SA_CAPA))) {
                    if(LIB_CAPA_U->UNIT == LIB_PF)
                        *capa = ((float) atof(pcap))*LIB_CAPA_U->FACTOR*1e3;
                    else if(LIB_CAPA_U->UNIT == LIB_FF)
                        *capa = ((float) atof(pcap))*LIB_CAPA_U->FACTOR*1;
                }

                else
                    *capa = 0;
                if (p->TYPE == PIN_TYPE) {
                    pinname = namealloc((char *)(p->NAMES->DATA));
                    if((str = lib_get_s_attr_val(p, SA_CK))) {
                        if(!strcasecmp(str,"true")) 
                            ckch = addchain(ckch, namealloc(pinname));
                    }
                    addhtitem(pcapht, namealloc(pinname), (long)capa);
                    breaksigchain = ttv_addrefsig(fig, namealloc(pinname), namealloc(pinname), *capa, dir, breaksigchain);
                    nbbreaksig++;
                }
            }
        }
    }

    fig->CONSIG = ttv_allocreflist(consigchain, nbconsig);
    fig->NBCONSIG = nbconsig;
    for(i=0; i < fig->NBCONSIG ; i++)
        addhtitem(pht, namealloc(fig->CONSIG[i]->NAME), (long)fig->CONSIG[i]);
    fig->EBREAKSIG = ttv_allocreflist(breaksigchain, nbbreaksig);
    fig->NBEBREAKSIG = nbbreaksig;
    for(i=0; i < fig->NBEBREAKSIG ; i++)
        addhtitem(pht, namealloc(fig->EBREAKSIG[i]->NAME), (long)fig->EBREAKSIG[i]);


    return ckch;
}

/*----------------------------------------------------------------------------*/
/* LIB_TIM_GET_LONG_DIR :                                                     */
/* donne le long correspondant a la direction du connecteur.                  */                                                 
/*----------------------------------------------------------------------------*/
long     lib_tim_get_long_dir(char dir, char *ts)
{
    long d;
    
    switch(dir) {
        case IN     :
            d = TTV_SIG_CI;
            break;

        case OUT    :
            if(ts)
                d = TTV_SIG_CZ;
            else
                d = TTV_SIG_CO;
            break;

        case IO     :
            if(ts)
                d = TTV_SIG_CT;
            else
                d = TTV_SIG_CB;
            break;

        case TRI    :
            d = TTV_SIG_CZ;
            break;
            
        case INT    :
            d = TTV_SIG_B;
            break;
            
        default :
            d = TTV_SIG_CX;
            break;

    }

    return d;
}

void lib_addline(ttvfig_list *fig, char *group_name, ttvevent_list *node, ttvevent_list *root, float dly, float slw, timing_model *dlymodel, timing_model *slwmodel, long type)
{
    ttvline_list *line;
    
    if(group_name && !strncmp("maxd", group_name, 4)){
        line = ttv_getline(fig, fig, node, root, NULL, type, 0, 1, 0);
        if(line){
            line->VALMAX = dly*TTV_UNIT;
            line->FMAX = slw*TTV_UNIT;
            if(dlymodel) line->MDMAX = dlymodel->NAME;
            if(slwmodel) line->MFMAX = slwmodel->NAME;
        }else{
            line = ttv_addline(fig, root, node, mbk_long_round(dly*TTV_UNIT), mbk_long_round(slw*TTV_UNIT), mbk_long_round(dly*TTV_UNIT), mbk_long_round(slw*TTV_UNIT), type);
            if(dlymodel && slwmodel)
                ttv_addcaracline(line, dlymodel->NAME, dlymodel->NAME, slwmodel->NAME, slwmodel->NAME);
            else if(dlymodel)
                ttv_addcaracline(line, dlymodel->NAME, dlymodel->NAME, NULL, NULL);
        }
    }else if(group_name && !strncmp("mind", group_name, 4)){
        line = ttv_getline(fig, fig, node, root, NULL, type, 0, 1, 0);
        if(line){
            line->VALMIN = dly*TTV_UNIT;
            line->FMIN = slw*TTV_UNIT;
            if(dlymodel) line->MDMIN = dlymodel->NAME;
            if(slwmodel) line->MFMIN = slwmodel->NAME;
        }else{
            line = ttv_addline(fig, root, node, mbk_long_round(dly*TTV_UNIT), mbk_long_round(slw*TTV_UNIT), mbk_long_round(dly*TTV_UNIT), mbk_long_round(slw*TTV_UNIT), type);
            if(dlymodel && slwmodel)
                ttv_addcaracline(line, dlymodel->NAME, dlymodel->NAME, slwmodel->NAME, slwmodel->NAME);
            else if(dlymodel)
                ttv_addcaracline(line, dlymodel->NAME, dlymodel->NAME, NULL, NULL);
        }
    }else{
        line = ttv_addline(fig, root, node, mbk_long_round(dly*TTV_UNIT), mbk_long_round(slw*TTV_UNIT), mbk_long_round(dly*TTV_UNIT), mbk_long_round(slw*TTV_UNIT), type);
        if(dlymodel && slwmodel)
            ttv_addcaracline(line, dlymodel->NAME, dlymodel->NAME, slwmodel->NAME, slwmodel->NAME);
        else if(dlymodel)
            ttv_addcaracline(line, dlymodel->NAME, dlymodel->NAME, NULL, NULL);
    }
}    
/*----------------------------------------------------------------------------*/
/* LIB_TIM_ADDCARACLINE :                                                     */
/* fait le ttv_addcaracline pour tous les models.                             */
/*----------------------------------------------------------------------------*/
void lib_tim_addcaracline(lib_group *g, ttvfig_list *fig, char *cellname, char *pinname, ht *pht, ht *pcapht)
{
    timing_model *dlymodelrise=NULL, *dlymodelfall=NULL;    
    timing_model *slwmodelrise=NULL, *slwmodelfall=NULL;   
    lib_group *p;
    char *relpin, *val;
    ttvsig_list *signode, *sigroot;
    ttvevent_list *noderise=NULL, *nodefall=NULL, *rootrise=NULL, *rootfall=NULL;
    ttvline_list *line;
    int itype=9, otype;
    char ttype, sense;
    double thrisemin, thrisemax, thfallmin, thfallmax, voltage, th_rate_rise=1.0, th_rate_fall=1.0;
    long linetype = 0;
    float dlyfall=0, dlyrise=0, slwfall=0, slwrise=0;
    long capaitem;
    float capaval, low, high;
    char *group_name;
   
    char mess[1024];
    

    lofig_list  *lofig;
    locon_list  *locon;
    cbhcomb     *comb;
    
    sense = lib_tim_give_sense(g);
    ttype = lib_tim_give_timing_type(g);
    relpin = namealloc(lib_get_s_attr_val(g, SA_REL_PIN));


    if(LIB_GP_HEAD->TYPE == LIB_TYPE){
         thfallmax = LIB_DEFAULT_SA_SLEW_UP_FALL_UNIT;
         thfallmin = LIB_DEFAULT_SA_SLEW_LOW_FALL_UNIT;
         thrisemax = LIB_DEFAULT_SA_SLEW_UP_RISE_UNIT;
         thrisemin = LIB_DEFAULT_SA_SLEW_LOW_RISE_UNIT;
         voltage = LIB_DEFAULT_SA_NOM_VOLTAGE;
         slwrise = LIB_DEFAULT_SA_SLOPE_RISE;
         slwfall = LIB_DEFAULT_SA_SLOPE_FALL;
         
         if ((val=lib_get_s_attr_val(LIB_GP_HEAD, SA_SLEW_UP_FALL_UNIT))!=NULL)
           thfallmax = atof(val)/100;
         if ((val=lib_get_s_attr_val(LIB_GP_HEAD, SA_SLEW_LOW_FALL_UNIT))!=NULL)
           thfallmin = atof(val)/100;
         if ((val=lib_get_s_attr_val(LIB_GP_HEAD, SA_SLEW_UP_RISE_UNIT))!=NULL)
           thrisemax = atof(val)/100;
         if ((val=lib_get_s_attr_val(LIB_GP_HEAD, SA_SLEW_LOW_RISE_UNIT))!=NULL)
           thrisemin = atof(val)/100;
         if ((val=lib_get_s_attr_val(LIB_GP_HEAD, SA_NOM_VOLTAGE))!=NULL)
           voltage = atof(val);
         if ((val=lib_get_s_attr_val(LIB_GP_HEAD, SA_DEFAULT_SLOPE_RISE))!=NULL)
           slwrise = atof(val);
         if ((val=lib_get_s_attr_val(LIB_GP_HEAD, SA_DEFAULT_SLOPE_FALL))!=NULL)
           slwfall = atof(val);
         th_rate_rise = (V_FLOAT_TAB[ __SIM_VTH_HIGH ].VALUE - V_FLOAT_TAB[ __SIM_VTH_LOW ].VALUE)/(thrisemax - thrisemin);
         th_rate_fall = (V_FLOAT_TAB[ __SIM_VTH_HIGH ].VALUE - V_FLOAT_TAB[ __SIM_VTH_LOW ].VALUE)/(thfallmax - thfallmin);

    /*thrisemin = (atof(lib_get_s_attr_val(LIB_GP_HEAD, SA_SLEW_LOW_RISE_UNIT)))/100;
        thrisemax = (atof(lib_get_s_attr_val(LIB_GP_HEAD, SA_SLEW_UP_RISE_UNIT)))/100;
        thfallmin = (atof(lib_get_s_attr_val(LIB_GP_HEAD, SA_SLEW_LOW_FALL_UNIT)))/100;
        thfallmax = (atof(lib_get_s_attr_val(LIB_GP_HEAD, SA_SLEW_UP_FALL_UNIT)))/100;
        voltage = (atof(lib_get_s_attr_val(LIB_GP_HEAD, SA_NOM_VOLTAGE)));*/
    }
    
    capaitem = gethtitem(pcapht, pinname);
    if(capaitem != EMPTYHT) 
        capaval = (*(float *)capaitem) + STM_DEF_LOAD;
    
    else 
        capaval = STM_DEF_LOAD;

    if(ttype == LIB_TS_DISABLE)
        linetype = TTV_LINE_HZ;

    if(!g->SONS)  
        lib_tim_addline_const(g, fig, pht, linetype, ttype, sense, pinname, relpin);
   
    else {
        if(!((ttype == LIB_NOTYPE) && (sense == LIB_NON))) {
            
            for(p=g->SONS; p ; p=p->NEXT) {
                switch(p->TYPE) {
                    case DLY_RISE_TYPE :
                        signode = (ttvsig_list *)gethtitem(pht, relpin);
                        sigroot = (ttvsig_list *)gethtitem(pht, pinname);
                        if(((long)signode != EMPTYHT) && ((long)sigroot != EMPTYHT)) {
                            rootrise = &sigroot->NODE[1];
                            otype = elpRISE;
                            if((ttype == LIB_TS_ENABLE) || (ttype == LIB_TS_DISABLE)) {
                                switch(sense) {
                                    case LIB_POS:
                                        noderise = &signode->NODE[1];
                                        itype = elpRISE;
                                        break;
                                    case LIB_NEG:
                                        noderise = &signode->NODE[0];
                                        itype = elpFALL;
                                        break;
                                    default :
                                        lofig = getlofig (fig->INFO->FIGNAME, 'P'); 
                                        locon = getlocon (lofig, pinname);
                                        comb = cbh_getcombfromlocon (locon);
                                        switch (cbh_calcsense_abl (comb->HZFUNC, relpin)) {
                                            case CBH_INVERT : /* same as LIB_NEG */
                                                noderise = &signode->NODE[0];
                                                itype = elpFALL;
                                                break;
                                            case CBH_NONINVERT :    /* same as LIB_POS */
                                                noderise = &signode->NODE[1];
                                                itype = elpRISE;
                                                break;
                                            case CBH_NONE :  /* no solution */          
                                                sprintf (mess, "in cell %s: ambiguous functionality \"three_state\" of pin %s.\nDelay between pin %s and pin %s not added.",
                                                        cellname, pinname, pinname, relpin);
                                                fprintf (stderr, "***lib warning: %s***\n", mess); 
                                                break;
                                        }
                                        break;
                                }
                            }else{
                                switch(sense) {
                                    case LIB_POS:
                                        noderise = &signode->NODE[1];
                                        itype = elpRISE;
                                        break;
                                    case LIB_NEG:
                                        noderise = &signode->NODE[0];
                                        itype = elpFALL;
                                        break;
                                    default:
                                        if(ttype == LIB_RISE_EDGE) {
                                            noderise = &signode->NODE[1];
                                            itype = elpRISE;
                                            linetype = TTV_LINE_A;
                                        }
                                        else if(ttype == LIB_FALL_EDGE) {
                                            noderise = &signode->NODE[0];
                                            itype = elpFALL;
                                            linetype = TTV_LINE_A;
                                        }
                                        break;
                                }
                            }
                            dlymodelrise = lib_tim_store_model_group(cellname, pinname, p, ttype, itype, otype, relpin);
                            if(itype == elpRISE) 
                                stm_mod_shrinkslewaxis_with_rate(dlymodelrise,th_rate_rise);
                            else if(itype == elpFALL)
                                stm_mod_shrinkslewaxis_with_rate(dlymodelrise, th_rate_fall);
                           
                            dlyrise = stm_mod_delay(dlymodelrise, capaval, STM_DEF_SLEW, NULL, "unknown"); 
                            if (!ttv_get_signal_swing(fig, sigroot, &low, &high)) voltage = high - low;
                            stm_mod_update(dlymodelrise, voltage/2.0, voltage, voltage/5.0, voltage);
                        }
                       
                        break;
                    
                    case DLY_FALL_TYPE :
                        signode = (ttvsig_list *)gethtitem(pht, relpin);
                        sigroot = (ttvsig_list *)gethtitem(pht, pinname);
                        if(((long)signode != EMPTYHT) && ((long)sigroot != EMPTYHT)) {
                            rootfall= &sigroot->NODE[0];
                            otype = elpFALL;
                            if((ttype == LIB_TS_ENABLE) || (ttype == LIB_TS_DISABLE)) {
                                switch(sense) {
                                    case LIB_POS:
                                        nodefall = &signode->NODE[1];
                                        itype = elpRISE;
                                        break;
                                    case LIB_NEG:
                                        nodefall = &signode->NODE[0];
                                        itype = elpFALL;
                                        break;
                                    default :
                                        lofig = getlofig (fig->INFO->FIGNAME, 'P'); 
                                        locon = getlocon (lofig, pinname);
                                        comb = cbh_getcombfromlocon (locon);
                                        switch (cbh_calcsense_abl (comb->HZFUNC, relpin)) {
                                            case CBH_INVERT : /* same as LIB_NEG */
                                                noderise = &signode->NODE[0];
                                                itype = elpFALL;
                                                break;
                                            case CBH_NONINVERT :    /* same as LIB_POS */
                                                noderise = &signode->NODE[1];
                                                itype = elpRISE;
                                                break;
                                            case CBH_NONE :  /* no solution */          
                                                sprintf (mess, "in cell %s: ambiguous functionality \"three_state\" of pin %s.\nDelay between pin %s and pin %s not added.",
                                                        cellname, pinname, pinname, relpin);
                                                fprintf (stderr, "***lib warning: %s***\n", mess); 
                                                break;
                                        }
                                        break;
                                }
                            }else{
                                switch(sense) {
                                    case LIB_POS:
                                        nodefall = &signode->NODE[0];
                                        itype = elpFALL;
                                        break;
                                    case LIB_NEG:
                                        nodefall = &signode->NODE[1];
                                        itype = elpRISE;
                                        break;
                                    default:
                                        if(ttype == LIB_RISE_EDGE) {
                                            nodefall = &signode->NODE[1];
                                            itype = elpRISE;
                                            linetype = TTV_LINE_A;
                                        }
                                        else if(ttype == LIB_FALL_EDGE) {
                                            nodefall = &signode->NODE[0];
                                            itype = elpFALL;
                                            linetype = TTV_LINE_A;
                                        }
                                        break;
                                }
                            }
                            dlymodelfall = lib_tim_store_model_group(cellname, pinname, p, ttype, itype, otype, relpin);
                            if(itype == elpRISE) 
                                stm_mod_shrinkslewaxis_with_rate(dlymodelfall, th_rate_rise);
                            else if(itype == elpFALL)
                                stm_mod_shrinkslewaxis_with_rate(dlymodelfall, th_rate_fall);
                                
                            dlyfall = stm_mod_delay(dlymodelfall, capaval, STM_DEF_SLEW, NULL, "unknown"); 
                            if (!ttv_get_signal_swing(fig, sigroot, &low, &high)) voltage = high - low;
                            stm_mod_update(dlymodelfall, voltage/2.0, voltage, voltage/5.0, 0.0);
                        }
                        break;
                        
                    case SLW_RISE_TYPE :
                        signode = (ttvsig_list *)gethtitem(pht, relpin);
                        sigroot = (ttvsig_list *)gethtitem(pht, pinname);
                        if(((long)signode != EMPTYHT) && ((long)sigroot != EMPTYHT)) {
                            rootrise = &sigroot->NODE[1];
                            otype = elpRISE;
                            if((ttype == LIB_TS_ENABLE) || (ttype == LIB_TS_DISABLE)) {
                                switch(sense) {
                                    case LIB_POS:
                                        noderise = &signode->NODE[1];
                                        itype = elpRISE;
                                        break;
                                    case LIB_NEG:
                                        noderise = &signode->NODE[0];
                                        itype = elpFALL;
                                        break;
                                    default :
                                        lofig = getlofig (fig->INFO->FIGNAME, 'P'); 
                                        locon = getlocon (lofig, pinname);
                                        comb = cbh_getcombfromlocon (locon);
                                        switch (cbh_calcsense_abl (comb->HZFUNC, relpin)) {
                                            case CBH_INVERT : /* same as LIB_NEG */
                                                noderise = &signode->NODE[0];
                                                itype = elpFALL;
                                                break;
                                            case CBH_NONINVERT :    /* same as LIB_POS */
                                                noderise = &signode->NODE[1];
                                                itype = elpRISE;
                                                break;
                                            case CBH_NONE :  /* no solution */          
                                                sprintf (mess, "in cell %s: ambiguous functionality \"three_state\" of pin %s.\nDelay between pin %s and pin %s not added.",
                                                        cellname, pinname, pinname, relpin);
                                                fprintf (stderr, "***lib warning: %s***\n", mess); 
                                                break;
                                        }
                                        break;
                                }
                            }else{
                                switch(sense) {
                                    case LIB_POS:
                                        noderise = &signode->NODE[1];
                                        itype = elpRISE;
                                        break;
                                    case LIB_NEG:
                                        noderise = &signode->NODE[0];
                                        itype = elpFALL;
                                        break;
                                    default:
                                        if(ttype == LIB_RISE_EDGE) {
                                            noderise = &signode->NODE[1];
                                            itype = elpRISE;
                                        }
                                        else if(ttype == LIB_FALL_EDGE) {
                                            noderise = &signode->NODE[0];
                                            itype = elpFALL;
                                        }
                                        break;
                                }
                            }
                            slwmodelrise = lib_tim_store_model_group(cellname, pinname, p, ttype, itype, otype, relpin);
                            
                            if(itype == elpRISE) 
                                stm_mod_shrinkslewaxis_with_rate(slwmodelrise, th_rate_rise);
                            else if(itype == elpFALL)
                                stm_mod_shrinkslewaxis_with_rate(slwmodelrise, th_rate_fall);
                                
                            if(otype == elpRISE) 
                                stm_mod_shrinkslewdata_with_rate(slwmodelrise, th_rate_rise);
                            else if(otype == elpFALL)
                                stm_mod_shrinkslewdata_with_rate(slwmodelrise, th_rate_fall);
                             
                            slwrise = stm_mod_slew(slwmodelrise, capaval, STM_DEF_SLEW, NULL, NULL, "unknown"); 
                            if (!ttv_get_signal_swing(fig, sigroot, &low, &high)) voltage = high - low;
                            stm_mod_update(slwmodelrise, voltage/2.0, voltage, voltage/5.0, voltage);
                        } 
                        break;    
        
                    case SLW_FALL_TYPE :
                        signode = (ttvsig_list *)gethtitem(pht, relpin);
                        sigroot = (ttvsig_list *)gethtitem(pht, pinname);
                        if(((long)signode != EMPTYHT) && ((long)sigroot != EMPTYHT)) {
                            rootfall = &sigroot->NODE[0];
                            otype = elpFALL;
                            if((ttype == LIB_TS_ENABLE) || (ttype == LIB_TS_DISABLE)) {
                                switch(sense) {
                                    case LIB_POS:
                                        nodefall = &signode->NODE[1];
                                        itype = elpRISE;
                                        break;
                                    case LIB_NEG:
                                        nodefall = &signode->NODE[0];
                                        itype = elpFALL;
                                        break;
                                    default :
                                        lofig = getlofig (fig->INFO->FIGNAME, 'P'); 
                                        locon = getlocon (lofig, pinname);
                                        comb = cbh_getcombfromlocon (locon);
                                        switch (cbh_calcsense_abl (comb->HZFUNC, relpin)) {
                                            case CBH_INVERT : /* same as LIB_NEG */
                                                noderise = &signode->NODE[0];
                                                itype = elpFALL;
                                                break;
                                            case CBH_NONINVERT :    /* same as LIB_POS */
                                                noderise = &signode->NODE[1];
                                                itype = elpRISE;
                                                break;
                                            case CBH_NONE :  /* no solution */          
                                                sprintf (mess, "in cell %s: ambiguous functionality \"three_state\" of pin %s.\nDelay between pin %s and pin %s not added.",
                                                        cellname, pinname, pinname, relpin);
                                                fprintf (stderr, "***lib warning: %s***\n", mess); 
                                                break;
                                        }
                                        break;
                                }
                            }else{
                                switch(sense) {
                                    case LIB_POS:
                                        nodefall = &signode->NODE[0];
                                        itype = elpFALL;
                                        break;
                                    case LIB_NEG:
                                        nodefall = &signode->NODE[1];
                                        itype = elpRISE;
                                        break;
                                    default:
                                        if(ttype == LIB_RISE_EDGE) {
                                            nodefall = &signode->NODE[1];
                                            itype = elpRISE;
                                        }
                                        else if(ttype == LIB_FALL_EDGE) {
                                            nodefall = &signode->NODE[0];
                                            itype = elpFALL;
                                        }
                                        break;
                                }
                            }
                            slwmodelfall = lib_tim_store_model_group(cellname, pinname, p, ttype, itype, otype, relpin);
                            
                            if(itype == elpRISE) 
                                stm_mod_shrinkslewaxis_with_rate(slwmodelfall, th_rate_rise);
                            else if(itype == elpFALL)
                                stm_mod_shrinkslewaxis_with_rate(slwmodelfall, th_rate_fall);
                                
                            if(otype == elpRISE) 
                                stm_mod_shrinkslewdata_with_rate(slwmodelfall, th_rate_rise);
                            else if(otype == elpFALL)
                                stm_mod_shrinkslewdata_with_rate(slwmodelfall, th_rate_fall);
                             
                            slwfall = stm_mod_slew(slwmodelfall, capaval, STM_DEF_SLEW, NULL, NULL, "unknown"); 
                            if (!ttv_get_signal_swing(fig, sigroot, &low, &high)) voltage = high - low;
                            stm_mod_update(slwmodelfall, voltage/2.0, voltage, voltage/5.0, 0.0);
                        } 
                        break;  
                       
                    case RISE_CONSTR_TYPE :
                        signode = (ttvsig_list *)gethtitem(pht, pinname);
                        sigroot = (ttvsig_list *)gethtitem(pht, relpin);
                        if(((long)signode != EMPTYHT) && ((long)sigroot != EMPTYHT)) {
                            noderise = &signode->NODE[1];
                            itype = elpRISE;
                            switch(ttype) {
                                case LIB_SETUP_RISE :
                                    rootrise = &sigroot->NODE[1];
                                    linetype = TTV_LINE_U;
                                    otype = elpRISE;
                                    break;
                                case LIB_HOLD_RISE  :
                                    rootrise = &sigroot->NODE[1];
                                    linetype = TTV_LINE_O;
                                    otype = elpRISE;
                                    break;

                                case LIB_SETUP_FALL :
                                    rootrise = &sigroot->NODE[0];
                                    linetype = TTV_LINE_U;
                                    otype = elpFALL;
                                    break;
                                case LIB_HOLD_FALL  :
                                    rootrise = &sigroot->NODE[0];
                                    linetype = TTV_LINE_O;
                                    otype = elpFALL;
                                    break;
                            }
                            
                            dlymodelrise = lib_tim_store_model_group(cellname, relpin, p, ttype, itype, otype, pinname);
                            
                            if(itype == elpRISE) 
                                stm_mod_shrinkslewaxis_with_rate(dlymodelrise, th_rate_rise);
                            else if(itype == elpFALL)
                                stm_mod_shrinkslewaxis_with_rate(dlymodelrise, th_rate_fall);
                             
                            dlyrise = stm_mod_constraint(dlymodelrise, STM_DEF_SLEW, STM_DEF_SLEW);
                            if (!ttv_get_signal_swing(fig, signode, &low, &high)) voltage = high - low;
                            stm_mod_update(dlymodelrise, voltage/2.0, voltage, voltage/5.0, voltage);
                        }
                        break; 

                    case FALL_CONSTR_TYPE :
                        signode = (ttvsig_list *)gethtitem(pht, pinname);
                        sigroot = (ttvsig_list *)gethtitem(pht, relpin);
                        if(((long)signode != EMPTYHT) && ((long)sigroot != EMPTYHT)) {
                            nodefall = &signode->NODE[0];
                            itype = elpFALL;
                            switch(ttype) {
                                case LIB_SETUP_RISE :
                                    rootfall = &sigroot->NODE[1];
                                    linetype = TTV_LINE_U;
                                    otype = elpRISE;
                                    break ;
                                    
                                case LIB_HOLD_RISE  :
                                    rootfall = &sigroot->NODE[1];
                                    linetype = TTV_LINE_O;
                                    otype = elpRISE;
                                    break;

                                case LIB_SETUP_FALL :
                                    rootfall = &sigroot->NODE[0];
                                    linetype = TTV_LINE_U;
                                    otype = elpFALL;
                                    break;

                                case LIB_HOLD_FALL  :
                                    rootfall = &sigroot->NODE[0];
                                    linetype = TTV_LINE_O;
                                    otype = elpFALL;
                                    break;
                            }
                            
                            dlymodelfall = lib_tim_store_model_group(cellname, relpin, p, ttype, itype, otype, pinname);
                            
                            if(itype == elpRISE) 
                                stm_mod_shrinkslewaxis_with_rate(dlymodelfall, th_rate_rise);
                            else if(itype == elpFALL)
                                stm_mod_shrinkslewaxis_with_rate(dlymodelfall, th_rate_fall);
                             
                            dlyfall = stm_mod_constraint(dlymodelfall, STM_DEF_SLEW, STM_DEF_SLEW);
                            if (!ttv_get_signal_swing(fig, signode, &low, &high)) voltage = high - low;
                            stm_mod_update(dlymodelfall, voltage/2.0, voltage, voltage/5.0, 0.0);
                        }
                        break; 
                }
            }

            if(dlymodelrise) {
                if((linetype == TTV_LINE_U) || (linetype == TTV_LINE_O)){
                    line = ttv_addline(fig, rootrise, noderise, dlyrise*TTV_UNIT, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_T | linetype);
                    ttv_addcaracline(line, dlymodelrise->NAME, NULL, NULL, NULL);
                    line = ttv_addline(fig, rootrise, noderise, dlyrise*TTV_UNIT, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_D | linetype);
                    ttv_addcaracline(line, dlymodelrise->NAME, NULL, NULL, NULL);
                }else{
                    if(g->NAMES) group_name = (char*)g->NAMES->DATA;
                    else group_name = NULL;
                    lib_addline(fig, group_name, noderise, rootrise, dlyrise, slwrise, dlymodelrise, slwmodelrise, TTV_LINE_T | linetype);
                    lib_addline(fig, group_name, noderise, rootrise, dlyrise, slwrise, dlymodelrise, slwmodelrise, TTV_LINE_D | linetype);
                }
            }
            
            if(dlymodelfall) {
                if((linetype == TTV_LINE_U) || (linetype == TTV_LINE_O)){
                    line = ttv_addline(fig, rootfall, nodefall, dlyfall*TTV_UNIT, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_T | linetype);
                    ttv_addcaracline(line, dlymodelfall->NAME, NULL, NULL, NULL);
                    line = ttv_addline(fig, rootfall, nodefall, dlyfall*TTV_UNIT, 0, TTV_NOTIME, TTV_NOSLOPE, TTV_LINE_D | linetype);
                    ttv_addcaracline(line, dlymodelfall->NAME, NULL, NULL, NULL);
                }else{
                    if(g->NAMES) group_name = (char*)g->NAMES->DATA;
                    else group_name = NULL;
                    lib_addline(fig, group_name, nodefall, rootfall, dlyfall, slwfall, dlymodelfall, slwmodelfall, TTV_LINE_T | linetype);
                    lib_addline(fig, group_name, nodefall, rootfall, dlyfall, slwfall, dlymodelfall, slwmodelfall, TTV_LINE_D | linetype);
                }
            }
        }
    } 
    
}

/*----------------------------------------------------------------------------*/
/* LIB_TIM_ADDLINE_CONST :                                                    */
/* Fait le addline pour les constantes (intrinsic et slope).                  */
/*----------------------------------------------------------------------------*/
void    lib_tim_addline_const(lib_group *g, ttvfig_list *fig, ht *pinht, long linetype, char tim_type, char sense, char *pinname, char *relpin)
{
    chain_list *sa;
    ttvsig_list *signode, *sigroot;
    ttvevent_list *noderise=NULL, *nodefall=NULL, *rootrise=NULL, *rootfall=NULL;
    ttvline_list *line;
    double thrisemin, thrisemax, thfallmin, thfallmax, th_rate_rise=1.0, th_rate_fall=1.0;
    float dlyrise=0, dlyfall=0, slwrise=0, slwfall=0;
    float f=1;
    char *group_name, *val;


    if(LIB_GP_HEAD->TYPE == LIB_TYPE){
         thfallmax = LIB_DEFAULT_SA_SLEW_UP_FALL_UNIT;
         thfallmin = LIB_DEFAULT_SA_SLEW_LOW_FALL_UNIT;
         thrisemax = LIB_DEFAULT_SA_SLEW_UP_RISE_UNIT;
         thrisemin = LIB_DEFAULT_SA_SLEW_LOW_RISE_UNIT;
         slwrise = LIB_DEFAULT_SA_SLOPE_RISE;
         slwfall = LIB_DEFAULT_SA_SLOPE_FALL;
         
         if ((val=lib_get_s_attr_val(LIB_GP_HEAD, SA_SLEW_UP_FALL_UNIT))!=NULL)
           thfallmax = atof(val)/100;
         if ((val=lib_get_s_attr_val(LIB_GP_HEAD, SA_SLEW_LOW_FALL_UNIT))!=NULL)
           thfallmin = atof(val)/100;
         if ((val=lib_get_s_attr_val(LIB_GP_HEAD, SA_SLEW_UP_RISE_UNIT))!=NULL)
           thrisemax = atof(val)/100;
         if ((val=lib_get_s_attr_val(LIB_GP_HEAD, SA_SLEW_LOW_RISE_UNIT))!=NULL)
           thrisemin = atof(val)/100;
         if ((val=lib_get_s_attr_val(LIB_GP_HEAD, SA_DEFAULT_SLOPE_RISE))!=NULL)
           slwrise = atof(val);
         if ((val=lib_get_s_attr_val(LIB_GP_HEAD, SA_DEFAULT_SLOPE_FALL))!=NULL)
           slwfall = atof(val);
         th_rate_rise = (V_FLOAT_TAB[ __SIM_VTH_HIGH ].VALUE - V_FLOAT_TAB[ __SIM_VTH_LOW ].VALUE)/(thrisemax - thrisemin);
         th_rate_fall = (V_FLOAT_TAB[ __SIM_VTH_HIGH ].VALUE - V_FLOAT_TAB[ __SIM_VTH_LOW ].VALUE)/(thfallmax - thfallmin);
    }

    for(sa = g->S_ATTR ; sa ; sa = sa->NEXT) {
        switch(((lib_s_attr *)(sa->DATA))->TYPE) {
            case SA_INTR_RISE   :
                switch (tim_type) {
                    case LIB_RISE_EDGE  :
                        signode = (ttvsig_list *)gethtitem(pinht, relpin);
                        sigroot = (ttvsig_list *)gethtitem(pinht, pinname);
                        if(((long)signode != EMPTYHT) && ((long)sigroot != EMPTYHT)) {
                            rootrise = &sigroot->NODE[1];
                            noderise = &signode->NODE[1];
                            linetype = TTV_LINE_A;
                        }
                        break;

                    case LIB_FALL_EDGE  :
                        signode = (ttvsig_list *)gethtitem(pinht, relpin);
                        sigroot = (ttvsig_list *)gethtitem(pinht, pinname);
                        if(((long)signode != EMPTYHT) && ((long)sigroot != EMPTYHT)) {
                            rootrise = &sigroot->NODE[1];
                            noderise = &signode->NODE[0];
                            linetype = TTV_LINE_A;
                        }
                        break;
                        

                    case LIB_SETUP_RISE :
                        signode = (ttvsig_list *)gethtitem(pinht, pinname);
                        sigroot = (ttvsig_list *)gethtitem(pinht, relpin);
                        if(((long)signode != EMPTYHT) && ((long)sigroot != EMPTYHT)) {
                            rootrise = &sigroot->NODE[1];
                            noderise = &signode->NODE[1];
                            linetype = TTV_LINE_U;
                        }
                        break;
                        
                    case LIB_HOLD_RISE  :    
                        signode = (ttvsig_list *)gethtitem(pinht, pinname);
                        sigroot = (ttvsig_list *)gethtitem(pinht, relpin);
                        if(((long)signode != EMPTYHT) && ((long)sigroot != EMPTYHT)) {
                            rootrise = &sigroot->NODE[1];
                            noderise = &signode->NODE[1];
                            linetype = TTV_LINE_O;
                        }
                        break;

                    case LIB_SETUP_FALL  :
                        signode = (ttvsig_list *)gethtitem(pinht, pinname);
                        sigroot = (ttvsig_list *)gethtitem(pinht, relpin);
                        if(((long)signode != EMPTYHT) && ((long)sigroot != EMPTYHT)) {
                            rootrise = &sigroot->NODE[0];
                            noderise = &signode->NODE[1];
                            linetype = TTV_LINE_U;
                        }
                        break;

                    case LIB_HOLD_FALL  :
                        signode = (ttvsig_list *)gethtitem(pinht, pinname);
                        sigroot = (ttvsig_list *)gethtitem(pinht, relpin);
                        if(((long)signode != EMPTYHT) && ((long)sigroot != EMPTYHT)) {
                            rootrise = &sigroot->NODE[0];
                            noderise = &signode->NODE[1];
                            linetype = TTV_LINE_O;
                        }
                        break;


                    default :
                        signode = (ttvsig_list *)gethtitem(pinht, relpin);
                        sigroot = (ttvsig_list *)gethtitem(pinht, pinname);
                        if(((long)signode != EMPTYHT) && ((long)sigroot != EMPTYHT)) {
                            rootrise = &sigroot->NODE[1];
                            if(sense == LIB_POS)
                                noderise = &signode->NODE[1];
                            else if(sense == LIB_NEG)
                                noderise = &signode->NODE[0];
                        }
                        break;
                }
                dlyrise = (float) atof(lib_get_s_attr_val(g,SA_INTR_RISE));
                break;

            case SA_INTR_FALL   :
                switch (tim_type) {
                    case LIB_RISE_EDGE  :
                        signode = (ttvsig_list *)gethtitem(pinht, relpin);
                        sigroot = (ttvsig_list *)gethtitem(pinht, pinname);
                        if(((long)signode != EMPTYHT) && ((long)sigroot != EMPTYHT)) {
                            rootfall = &sigroot->NODE[0];
                            nodefall = &signode->NODE[1];
                            linetype = TTV_LINE_A;
                        }
                        break;

                    case LIB_FALL_EDGE  :
                        signode = (ttvsig_list *)gethtitem(pinht, relpin);
                        sigroot = (ttvsig_list *)gethtitem(pinht, pinname);
                        if(((long)signode != EMPTYHT) && ((long)sigroot != EMPTYHT)) {
                            rootfall = &sigroot->NODE[0];
                            nodefall = &signode->NODE[0];
                            linetype = TTV_LINE_A;
                        }
                        break;
                        

                    case LIB_SETUP_RISE :
                        signode = (ttvsig_list *)gethtitem(pinht, pinname);
                        sigroot = (ttvsig_list *)gethtitem(pinht, relpin);
                        if(((long)signode != EMPTYHT) && ((long)sigroot != EMPTYHT)) {
                            rootfall = &sigroot->NODE[1];
                            nodefall = &signode->NODE[0];
                            linetype = TTV_LINE_U;
                        }
                        break;

                    case LIB_HOLD_RISE  :    
                        signode = (ttvsig_list *)gethtitem(pinht, pinname);
                        sigroot = (ttvsig_list *)gethtitem(pinht, relpin);
                        if(((long)signode != EMPTYHT) && ((long)sigroot != EMPTYHT)) {
                            rootfall = &sigroot->NODE[1];
                            nodefall = &signode->NODE[0];
                            linetype = TTV_LINE_O;
                        }
                        break;

                    case LIB_SETUP_FALL  :
                        signode = (ttvsig_list *)gethtitem(pinht, pinname);
                        sigroot = (ttvsig_list *)gethtitem(pinht, relpin);
                        if(((long)signode != EMPTYHT) && ((long)sigroot != EMPTYHT)) {
                            rootfall = &sigroot->NODE[0];
                            nodefall = &signode->NODE[0];
                            linetype = TTV_LINE_U;
                        }
                        break;

                    case LIB_HOLD_FALL  :
                        signode = (ttvsig_list *)gethtitem(pinht, pinname);
                        sigroot = (ttvsig_list *)gethtitem(pinht, relpin);
                        if(((long)signode != EMPTYHT) && ((long)sigroot != EMPTYHT)) {
                            rootfall = &sigroot->NODE[0];
                            nodefall = &signode->NODE[0];
                            linetype = TTV_LINE_O;
                        }
                        break;


                    default :
                        signode = (ttvsig_list *)gethtitem(pinht, relpin);
                        sigroot = (ttvsig_list *)gethtitem(pinht, pinname);
                        if(((long)signode != EMPTYHT) && ((long)sigroot != EMPTYHT)) {
                            rootfall = &sigroot->NODE[0];
                            if(sense == LIB_POS)
                                nodefall = &signode->NODE[0];
                            else if(sense == LIB_NEG)
                                nodefall = &signode->NODE[1];
                        }
                        break;
                }
                dlyfall = (float) atof(lib_get_s_attr_val(g,SA_INTR_FALL));
                break;
            
            case SA_SLOPE_RISE   :
                signode = (ttvsig_list *)gethtitem(pinht, relpin);
                sigroot = (ttvsig_list *)gethtitem(pinht, pinname);
                if(((long)signode != EMPTYHT) && ((long)sigroot != EMPTYHT)) {
                    rootrise = &sigroot->NODE[1];
                    if(sense == LIB_POS)
                        noderise = &signode->NODE[1];
                    else if(sense == LIB_NEG)
                        noderise = &signode->NODE[0];
                }
                slwrise = (float) atof(lib_get_s_attr_val(g,SA_SLOPE_RISE));
                break;

            case SA_SLOPE_FALL   :
                signode = (ttvsig_list *)gethtitem(pinht, relpin);
                sigroot = (ttvsig_list *)gethtitem(pinht, pinname);
                if(((long)signode != EMPTYHT) && ((long)sigroot != EMPTYHT)) {
                    rootfall = &sigroot->NODE[0];
                    if(sense == LIB_POS)
                        nodefall = &signode->NODE[0];
                    else if(sense == LIB_NEG)
                        nodefall = &signode->NODE[1];
                }
                slwfall = (float) atof(lib_get_s_attr_val(g,SA_SLOPE_FALL));
                break;

            default     :
                break;
        }
    }

    f = LIB_TIME_U->FACTOR;
    if(LIB_TIME_U->UNIT == LIB_NS) 
        f *= 1e3 ;
    
    if(rootrise && noderise) {
        if((linetype == TTV_LINE_U) || (linetype == TTV_LINE_O)){
            line = ttv_addline(fig, rootrise, noderise, mbk_long_round(dlyrise*f*TTV_UNIT), 0, mbk_long_round(dlyrise*f*TTV_UNIT), 0, TTV_LINE_T | linetype);
            line = ttv_addline(fig, rootrise, noderise, mbk_long_round(dlyrise*f*TTV_UNIT), 0, mbk_long_round(dlyrise*f*TTV_UNIT), 0, TTV_LINE_D | linetype);
        }else{
            slwrise *= th_rate_rise;
            if((slwrise*f*TTV_UNIT) < 1) slwrise = 1.0/f/TTV_UNIT;
            if(g->NAMES) group_name = (char*)g->NAMES->DATA;
            else group_name = NULL;
            lib_addline(fig, group_name, noderise, rootrise, dlyrise*f, slwrise*f, NULL, NULL, TTV_LINE_T | linetype);
            lib_addline(fig, group_name, noderise, rootrise, dlyrise*f, slwrise*f, NULL, NULL, TTV_LINE_D | linetype);
        }
    }

    if(rootfall && nodefall) {
        if((linetype == TTV_LINE_U) || (linetype == TTV_LINE_O)){
            line = ttv_addline(fig, rootfall, nodefall, mbk_long_round(dlyfall*f*TTV_UNIT), 0, mbk_long_round(dlyfall*f*TTV_UNIT), 0, TTV_LINE_T | linetype);
            line = ttv_addline(fig, rootfall, nodefall, mbk_long_round(dlyfall*f*TTV_UNIT), 0, mbk_long_round(dlyfall*f*TTV_UNIT), 0, TTV_LINE_D | linetype);
        }else{
            slwfall *= th_rate_fall;
            if((slwfall*f*TTV_UNIT) < 1) slwfall = 1.0/f/TTV_UNIT;
            if(g->NAMES) group_name = (char*)g->NAMES->DATA;
            else group_name = NULL;
            lib_addline(fig, group_name, nodefall, rootfall, dlyfall*f, slwfall*f, NULL, NULL, TTV_LINE_T | linetype);
            lib_addline(fig, group_name, nodefall, rootfall, dlyfall*f, slwfall*f, NULL, NULL, TTV_LINE_D | linetype);
        }
    }

}
/*----------------------------------------------------------------------------*/
/* LIB_TIM_ISCKPIN :                                                          */
/* Renvoie 1 si le connecteur est une clock, 0 sinon.                         */
/*----------------------------------------------------------------------------*/

int lib_tim_isckpin (chain_list *ckchain, char *pname) 
{
    chain_list *ch ;

    for (ch = ckchain ; ch ; ch = ch->NEXT) 
        if (ch->DATA == pname) 
            return (1) ;
    return (0) ;
}

static void lib_add_latch_cmd(ttvsig_list *tvs, ttvevent_list *cmd)
{
  chain_list *actual=NULL;
  ptype_list *pt;
  if ((pt=getptype(tvs->USER, TTV_SIG_CMD))==NULL)
    pt=tvs->USER=addptype(tvs->USER, TTV_SIG_CMD, NULL);
  for (actual=(chain_list *)pt->DATA; actual!=NULL && actual->DATA!=cmd; actual=actual->NEXT) ;
  if (actual==NULL) pt->DATA=addchain((chain_list *)pt->DATA, cmd);
}

/*----------------------------------------------------------------------------*/
/* LIB_TIM_GETSIGTYPE :                                                       */
/* Attribue un type au connecteur .                                           */
/*----------------------------------------------------------------------------*/
void     lib_tim_getsigtype(ttvfig_list *fig, lofig_list *pfig)
{
    ttvline_list *line;
    ttvsig_list  *sig;
    cbhseq  *ptrseq;
    int i, j;
    
    ptrseq = cbh_getseqfromlofig(pfig);

    for (i = 0; i < fig->NBCONSIG; i++) {
        sig = fig->CONSIG[i];
        
        for (j = 0; j < 2; j++) {
            /* typer B, LL (latch), LF (flip-flop) */
            for (line = sig->NODE[j].INLINE; line; line = line->NEXT) {
                /* access */
                if ((line->TYPE & TTV_LINE_A) == TTV_LINE_A) { 
                    if(ptrseq) {
                        if(ptrseq->PIN){
                            if(!strcasecmp(sig->NAME, ptrseq->PIN)) {
                                switch(ptrseq->SEQTYPE) {
                                    case CBH_LATCH  :
                                        sig->TYPE |= TTV_SIG_B;
                                        ttv_setsigflaggate(sig, TTV_SIG_GATE_LATCH);
                                        break;
                                    case CBH_FLIPFLOP   :
                                        sig->TYPE |= TTV_SIG_B;
                                        ttv_setsigflaggate(sig, TTV_SIG_GATE_FLIPFLOP);
                                        break;
        
                                    default     :
                                        sig->TYPE |= TTV_SIG_B;
                                        break;
                                }
                            }
                        }
                        if(ptrseq->NEGPIN){
                            if(!strcasecmp(sig->NAME, ptrseq->NEGPIN)) {
                                switch(ptrseq->SEQTYPE) {
                                    case CBH_LATCH  :
                                        sig->TYPE |= TTV_SIG_B;
                                        ttv_setsigflaggate(sig, TTV_SIG_GATE_LATCH);
                                        break;
                                    case CBH_FLIPFLOP   :
                                        sig->TYPE |= TTV_SIG_B;
                                        ttv_setsigflaggate(sig, TTV_SIG_GATE_FLIPFLOP);
                                        break;
        
                                    default     :
                                        sig->TYPE |= TTV_SIG_B;
                                        break;
                                }
                            }
                        }
                    }
                    else    
                        sig->TYPE |= TTV_SIG_B;
                    if((sig->TYPE & TTV_SIG_B) != TTV_SIG_B){
                        line->NODE->ROOT->TYPE&=~TTV_SIG_B;
                        line->NODE->ROOT->TYPE|=TTV_SIG_Q;
                        lib_add_latch_cmd(sig, line->NODE);
                        ttv_addcmd(line,TTV_LINE_CMDMAX,line->NODE) ;
                        ttv_addcmd(line,TTV_LINE_CMDMIN,line->NODE) ;
                    }
                }
                                
                /* hold, setup */
                if (((line->TYPE & TTV_LINE_O) == TTV_LINE_O) || ((line->TYPE & TTV_LINE_U) == TTV_LINE_U)) { 
                    sig->TYPE |= TTV_SIG_B;
                    line->NODE->ROOT->TYPE |= TTV_SIG_B;
                }
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
/* LIB_TIM_ALLOCBREAKLATCH :                                                  */
/* Alloue la liste des breaksig et des latchsig a la ttvfig .                 */
/*----------------------------------------------------------------------------*/
void    lib_tim_allocbreaklatch(ttvfig_list *fig)
{

    chain_list *breaksigchain = NULL, *latchsigchain = NULL, *cmdsigchain = NULL;
    int nbbreaksig=0, nblatchsig=0, nbcmd=0;
    int i;
    ttvsig_list *sig;


    for (i = 0; i < fig->NBCONSIG; i++) {
         sig = fig->CONSIG[i];
         if ((sig->TYPE & TTV_SIG_B) == TTV_SIG_B) {
             breaksigchain = addchain(breaksigchain,sig);
             nbbreaksig++;
         }
         if ((sig->TYPE & TTV_SIG_L) == TTV_SIG_L) {
             latchsigchain = addchain(latchsigchain,sig);
             nblatchsig++;
         }
         if ((sig->TYPE & TTV_SIG_Q) == TTV_SIG_Q) {
             cmdsigchain = addchain(cmdsigchain,sig);
             nbcmd++;
         }
     }
    for (i = 0; i < fig->NBEBREAKSIG; i++) {
         sig = fig->EBREAKSIG[i];
         breaksigchain = addchain(breaksigchain,sig);
         nbbreaksig++;
    }

     fig->NBEBREAKSIG = nbbreaksig;
     fig->EBREAKSIG   = ttv_allocreflist (breaksigchain, nbbreaksig);
     fig->NBELATCHSIG = nblatchsig;
     fig->ELATCHSIG   = ttv_allocreflist (latchsigchain, nblatchsig);
     fig->NBELCMDSIG = nbcmd;
     fig->ELCMDSIG   = ttv_allocreflist (cmdsigchain, nbcmd);


}



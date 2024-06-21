/******************************************/
/* lib_util.c                             */
/******************************************/


/******************************************************************************/
/* INCLUDE                                                                    */
/******************************************************************************/
#include "lib_util.h"


/******************************************************************************/
/* GLOBALS                                                                    */
/******************************************************************************/
num_list        *LIB_PTRCONTEXT=NULL;
lib_group       *LIB_CURRENT_GP;
lib_group       *LIB_GP_HEAD;
char             LIB_TRACE_MODE;
lib_scale_unit  *LIB_TIME_U=NULL, *LIB_CAPA_U=NULL, *LIB_RES_U=NULL;
char            *LIB_BUS_DELIM;
ptype_list      *libparserpowerptype=NULL;

/******************************************************************************/
/* FONCTIONS                                                                  */
/******************************************************************************/
/*----------------------------------------------------------------------------*/
/* LIB_UNQUOTE                                                                */
/* prend une chaine de caracteres entre quotes pour la renvoyer sans quotes   */
/*----------------------------------------------------------------------------*/
char *lib_unquote(char *source)
{
    char    *dest;
    int     i = 0, j = 0;

    dest = (char *)mbkalloc(sizeof(char) * (strlen(source)+1));
    while (*(source+j) != '\0') {
        if (*(source+j) != '"') {
            if ((*(source+j) == '\\') && (*(source+j+1) == '"')) {
                *(dest+i) = *(source+j+1);
                j++;
                i++;
            }else{
                *(dest+i) = *(source+j);
                i++;
            }
        }
        j++;
    }
    *(dest+i) = '\0';

    mbkfree(source);

    return (dest);
}
/*----------------------------------------------------------------------------*/
/* PUSH_CONTEXT                                                               */
/* empile le contexte du group                                                */
/*----------------------------------------------------------------------------*/
void lib_push_context(long data)
{
    LIB_PTRCONTEXT = addnum(LIB_PTRCONTEXT, data);
}
/*----------------------------------------------------------------------------*/
/* POP_CONTEXT                                                                */
/* depile le contexte                                                         */
/*----------------------------------------------------------------------------*/
void lib_pop_context()
{
    num_list *pt;

    if (LIB_PTRCONTEXT != NULL) 
    {
        pt = LIB_PTRCONTEXT;              
        LIB_PTRCONTEXT = LIB_PTRCONTEXT->NEXT;
        pt->NEXT = NULL;
        freenum(pt);
    }
    else
        fprintf(stderr," Impossible de depiler le contexte : pas de contexte! \n");
}


/*----------------------------------------------------------------------------*/
/* GET_CONTEXT                                                                */
/* renvoie le contexte courant                                                */
/*----------------------------------------------------------------------------*/
long lib_get_context()
{
    if(LIB_PTRCONTEXT != NULL) 
        return (LIB_PTRCONTEXT->DATA);
    else 
        return 0x0;
}

/*----------------------------------------------------------------------------*/
/* GIVE_CONTEXT                                                               */
/* renvoie le "long" correspondant a l'identificateur                         */
/*----------------------------------------------------------------------------*/
long lib_give_context(char *ident)
{
    long cntxt;

    if (lib_get_context() == NO_ACTION_TYPE)
        cntxt = NO_ACTION_TYPE;
    else
    {    
        if (!strcasecmp(ident, "library"))
            cntxt = LIB_TYPE;
        else if (!strcasecmp(ident, "wire_load"))
            cntxt = WIRE_LOAD_TYPE;
        else if (!strcasecmp(ident, "wire_load_table"))
            cntxt = WIRE_LOAD_TABLE_TYPE;
        else if (!strcasecmp(ident, "cell"))
            cntxt = CELL_TYPE;
        else if (!strcasecmp(ident, "pin"))
            cntxt = PIN_TYPE;
        else if (!strcasecmp(ident, "bus"))
            cntxt = BUS_TYPE;
        else if (!strcasecmp(ident, "type"))
            cntxt = TYPE_TYPE;
        else if ((!strcasecmp(ident, "ff")) || (!strcasecmp(ident, "ff_bank")))
            cntxt = FF_TYPE;
        else if ((!strcasecmp(ident, "latch")) || (!strcasecmp(ident, "latch_bank")))
            cntxt = LATCH_TYPE;
        else if (!strcasecmp(ident, "statetable")) 
            cntxt = STATETABLE_TYPE;
        else if (!strcasecmp(ident, "lu_table_template")) 
            cntxt = LU_TEMPL_TYPE;
        else if (!strcasecmp(ident, "timing")) 
            cntxt = TIMING_TYPE;
        else if ((!strcasecmp(ident, "cell_fall")) || (!strcasecmp(ident, "fall_propagation")))
            cntxt = DLY_FALL_TYPE;
        else if ((!strcasecmp(ident, "cell_rise")) || (!strcasecmp(ident, "rise_propagation")))
            cntxt = DLY_RISE_TYPE;
        else if (!strcasecmp(ident, "fall_transition"))
            cntxt = SLW_FALL_TYPE;
        else if (!strcasecmp(ident, "rise_transition")) 
            cntxt = SLW_RISE_TYPE;
        else if (!strcasecmp(ident, "fall_constraint"))
            cntxt = FALL_CONSTR_TYPE;
        else if (!strcasecmp(ident, "rise_constraint"))
            cntxt = RISE_CONSTR_TYPE;
        else if (!strcasecmp(ident, "power_supply"))
            cntxt = POWER_SUPPLY_TYPE;
        else
            cntxt = NO_ACTION_TYPE;
    }
    
    return cntxt;     
}

/*----------------------------------------------------------------------------*/
/* LIB_SETENV                                                                 */
/* positionne la variable LIB_TRACE_MODE pour l'affichage                     */
/*----------------------------------------------------------------------------*/
void lib_setenv()
{
    char *str;

    str = getenv("LIB_TRACE_MODE") ;
    str = V_STR_TAB[LIB_TRACE_MODE].VALUE;
    if(str != NULL){
        if (!strcasecmp(str,"all"))
            LIB_TRACE_MODE = 'A';
        else if (!strcasecmp(str,"v"))
            LIB_TRACE_MODE = 'V' ;
        else
            LIB_TRACE_MODE = 'N' ;
    }
    else
        LIB_TRACE_MODE = 'N' ;
        
}
/*----------------------------------------------------------------------------*/
/* GET_CHAR_DIR:                                                              */
/* donne le caractere de direction pour un connecteur.                        */
/*----------------------------------------------------------------------------*/
char lib_get_char_dir(char *str_dir)
{
    char dir;
    
    if(!strcasecmp(str_dir,"input"))
        dir = IN;
    if(!strcasecmp(str_dir,"output"))
        dir = OUT;
    if(!strcasecmp(str_dir,"inout"))
        dir = IO;
    if(!strcasecmp(str_dir,"internal"))
        dir = INT;
    if(!strcasecmp(str_dir,"tristate"))
        dir = TRI;

    return dir;
}

/*----------------------------------------------------------------------------*/
/* AFF_CBHSEQ:                                                                */
/* affiche le contenu de la cbhseq                                            */
/*----------------------------------------------------------------------------*/
void lib_aff_cbhseq(cbhseq *p)
{
    avt_log(LOGLIB,2,"Structure CBHSEQ:\n");
    avt_log(LOGLIB,2,"SEQTYPE: %d\n", p->SEQTYPE);
    if(p->NAME != NULL)
    avt_log(LOGLIB,2,"NAME: %s\n", p->NAME);
    if(p->NEGNAME != NULL)
    avt_log(LOGLIB,2,"NEGNAME: %s\n", p->NEGNAME);
    if(p->PIN != NULL)
    avt_log(LOGLIB,2,"PIN: %s\n", p->PIN);
    if(p->NEGPIN != NULL)
        avt_log(LOGLIB,2,"NEGPIN: %s\n", p->NEGPIN);
    if(p->CLOCK != NULL)
    {
        avt_log(LOGLIB,2,"CLOCK:");
        displayExprLog(LOGLIB,2,p->CLOCK);
    }
    if(p->SLAVECLOCK != NULL)
    {
        avt_log(LOGLIB,2,"SLAVECLOCK:");
        displayExprLog(LOGLIB,2,p->SLAVECLOCK);
    }
    if(p->DATA != NULL)
    {
        avt_log(LOGLIB,2,"DATA:");
        displayExprLog(LOGLIB,2,p->DATA);    
    }
    if(p->RESET != NULL)
    {
        avt_log(LOGLIB,2,"RESET:");
        displayExprLog(LOGLIB,2,p->RESET);
    }
    if(p->SET != NULL)
    {
        avt_log(LOGLIB,2,"SET:");
        displayExprLog(LOGLIB,2,p->SET);    
    }
    if(p->RSCONF != NULL)
    {
        avt_log(LOGLIB,2,"RSCONF:");
        displayExprLog(LOGLIB,2,p->RSCONF);
    }
    if(p->RSCONFNEG != NULL)
    {
        avt_log(LOGLIB,2,"RSCONFNEG:");
        displayExprLog(LOGLIB,2,p->RSCONFNEG);       
    }

}           
/*----------------------------------------------------------------------------*/
/* AFF_CBHCOMB:                                                               */
/* affiche le contenu de la cbhcomb                                           */
/*----------------------------------------------------------------------------*/
void lib_aff_cbhcomb(cbhcomb *p)
{
    avt_log(LOGLIB,2,"Structure CBHCOMB:\n");
    avt_log(LOGLIB,2,"\tNAME: %s\n", p->NAME);
    if(p->FUNCTION != NULL)
    {
        avt_log(LOGLIB,2,"\tFUNCTION: ");
        displayExprLog(LOGLIB,2,p->FUNCTION);
    }
    if(p->HZFUNC != NULL)
    {
        avt_log(LOGLIB,2,"\tHZFUNCTION: ");
        displayExprLog(LOGLIB,2,p->HZFUNC);
    }
    if(p->CONFLICT != NULL)
    {       
        avt_log(LOGLIB,2,"\tCONFLICT: ");
        displayExprLog(LOGLIB,2,p->CONFLICT);    
    }


}
/*----------------------------------------------------------------------------*/
/* LIB_INIT :                                                                 */
/* initialise les differents pointeurs ou varialbes utilises pour le timing   */
/*----------------------------------------------------------------------------*/
void    lib_init(void)
{
    LIB_GP_HEAD = NULL;
    LIB_CURRENT_GP = NULL;
    cbh_newclassifier();
    cbh_createcct();
}

/*----------------------------------------------------------------------------*/
/* LIB_CHAINLISTTOSTR :                                                       */
/* Transforme une liste chainee en chaine de caracteres.                      */
/*----------------------------------------------------------------------------*/
char *lib_chainlistToStr(chain_list *pchain)
{
    chain_list *p;
    char s[32];
    char *str;
    
    strcpy(s,(char *)pchain->DATA);
   
    for(p=pchain->NEXT ; p != NULL ; p=p->NEXT)
            strcat(s,(char *)p->DATA);
    
    str = (char*)mbkalloc((sizeof(char)*strlen(s))+1);
    strcpy(str,s);
    return str;
}

/*----------------------------------------------------------------------------*/
/* LIB_STRTOCHAINFLOAT :                                                      */
/* place dans une liste chainee les elements de la chaine de caracteres s     */
/* separes par la chaine de caracteres d                                      */
/*----------------------------------------------------------------------------*/
chain_list *lib_strToChainFloat(char *s)
{
    float *val;
    char *str, *chn, *d, dest[strlen(s)+1];
    chain_list *c;
    int j;
    char flag = 'N';
    char *src;   
    
   
    chn = mbkstrdup(s);
    
    val = (float *) mbkalloc(sizeof(float));
    
    if (strchr(chn,','))
        d = ","; /* le delimiteur de valeur est la virgule */
    else {         /*  le delimiteur de valeur est un espace */  
        d = " ";   /* on traite la chaine pour n'avoir qu'un seul espace 
                      entre deux valeurs */
       for (j = 0, src = chn; *src != '\0' ; src++) {
           if (*src != ' ') {
               flag = 'N';
               dest[j] = *src;
               j++;
           }
           else if (flag == 'N') {
               if (src != chn) {
                dest[j] = *src;
                j++;
               }
               flag = 'Y';
           }
       } 
       dest[j] = '\0';
       mbkfree (chn);
       chn = (char *) mbkalloc (sizeof (char) * (j + 1));
       strcpy(chn, dest);
    }
        
    str = strtok(chn,d);
    *val = (float) atof(str);
    c = addchain(NULL, (void *)val);
    while((str=strtok(NULL,d)) != NULL) {
        val = (float *) mbkalloc(sizeof(float));
        *val = (float) atof(str);           
        c = addchain(c, (void *)val);
    }
    
    mbkfree(chn);

    return (reverse(c));
}       

/*----------------------------------------------------------------------------*/
/* LIB_AFFCHAIN :                                                             */
/* Affiche tous les champs DATA de la liste chainee passee en argument        */
/*----------------------------------------------------------------------------*/
void lib_affchain(chain_list *p, char *format, char type)
{
    chain_list *d;

    for(d=p; d != NULL; d=d->NEXT) {
        if(type == TYPE_STRING)
            printf(format, (char *)(d->DATA));
        else if(type == TYPE_FLOAT)
            printf(format, *((float *)(d->DATA)));
    }

    printf("\n");
}
/*----------------------------------------------------------------------------*/
/* LIB_CREATEGROUP :                                                          */
/* initialise les champs du nouveau groupe et renvoie le pointeur sur la      */
/* nouvelle structure allouee                                                 */
/*----------------------------------------------------------------------------*/
lib_group   *lib_creategroup(char *type, chain_list *names)
{
    lib_group *p;
    chain_list *ch, *n=NULL;


    p = (lib_group*)mbkalloc(sizeof(lib_group));
    p->NEXT = NULL;
    p->OWNER = NULL;
    p->TYPE = lib_give_context(type);
    for(ch=names ; ch ; ch=ch->NEXT) {
        n = addchain(n, namealloc(ch->DATA));
        mbkfree(ch->DATA);
    }
    freechain(names);
    p->NAMES = reverse(n);
    p->S_ATTR = NULL;
    p->C_ATTR = NULL;
    p->S_ATTRHT = NULL;
    p->C_ATTRHT = NULL;
    p->SONS = NULL;
    
    return p;
}

/*----------------------------------------------------------------------------*/
/* LIB_ADDGROUP :                                                             */
/* Ajoute le groupe newgp au debut de la liste dont le pointeur de tete est   */
/* passe en argument. renvoie le nouveau pointeur de tete de liste            */
/*----------------------------------------------------------------------------*/
lib_group   *lib_addgroup(lib_group *g, lib_group *newgp)
{
    if(g) {
        if(!g->SONS) {
            g->SONS = newgp;
            g->LAST_SON = newgp;
        }
        
        else {
            g->LAST_SON->NEXT = newgp;
            g->LAST_SON = newgp ;
        }
    }

    return newgp;
}
/*----------------------------------------------------------------------------*/
/* LIB_PUSHGROUP :                                                            */
/* Empile un groupe dans la liste chainee de groupes.                         */
/*----------------------------------------------------------------------------*/
void    lib_pushgroup(char *type, chain_list *names)
{
    lib_group *p;
    
    //printf("GROUP: %s(%s)\n", type, (names) ? ((char *)(names->DATA)):" ");
    p = lib_creategroup(type, names);
    
    if(!LIB_GP_HEAD)
        LIB_GP_HEAD = p;
    
    p->OWNER = LIB_CURRENT_GP;
    LIB_CURRENT_GP = lib_addgroup(LIB_CURRENT_GP, p);

    
}

/*----------------------------------------------------------------------------*/
/* LIB_POPGROUP :                                                             */
/* Depile un groupe dans la liste chainee de groupes.                         */
/*----------------------------------------------------------------------------*/
void    lib_popgroup(void)
{
    LIB_CURRENT_GP = LIB_CURRENT_GP->OWNER;
}
/*----------------------------------------------------------------------------*/
/* LIB_GET_S_ATTR_TYPE :                                                      */
/* Donne le type (code sur un long) d'un simple attribute en fonction de son  */
/* nom.                                                                       */
/*----------------------------------------------------------------------------*/
long    lib_get_s_attr_type(char *name)
{
    long type;
        
    switch(lib_get_context())
    {
        case LIB_TYPE    :
            if(!strcasecmp(name,"time_unit"))
                type = SA_TIME_UNIT;
            else if(!strcasecmp(name,"pulling_resistance_unit"))
                type = SA_RES_UNIT;
            else if(!strcasecmp(name,"nom_voltage"))
                type = SA_NOM_VOLTAGE;
            else if(!strcasecmp(name,"nom_temperature"))
                type = SA_NOM_TEMP;
            else if(!strcasecmp(name,"slew_lower_threshold_pct_fall"))
                type = SA_SLEW_LOW_FALL_UNIT;
            else if(!strcasecmp(name,"slew_lower_threshold_pct_rise"))
                type = SA_SLEW_LOW_RISE_UNIT;
            else if(!strcasecmp(name,"slew_upper_threshold_pct_fall"))
                type = SA_SLEW_UP_FALL_UNIT;
            else if(!strcasecmp(name,"slew_upper_threshold_pct_rise"))
                type = SA_SLEW_UP_RISE_UNIT;
            else if(!strcasecmp(name,"input_threshold_pct_rise"))
                type = SA_INPUT_DTH_RISE;
            else if(!strcasecmp(name,"input_threshold_pct_fall"))
                type = SA_INPUT_DTH_FALL;
            else if(!strcasecmp(name,"output_threshold_pct_rise"))
                type = SA_OUTPUT_DTH_RISE;
            else if(!strcasecmp(name,"output_threshold_pct_fall"))
                type = SA_OUTPUT_DTH_FALL;
            else if(!strcasecmp(name,"default_wire_load"))
                type = SA_DEFAULT_WIRE_LOAD;
            else if(!strcasecmp(name,"bus_naming_style"))
                type = SA_BUS_NAMING_STYLE;
            else if(!strcasecmp(name,"default_slope_rise"))
                type = SA_DEFAULT_SLOPE_RISE;
            else if(!strcasecmp(name,"default_slope_fall"))
                type = SA_DEFAULT_SLOPE_FALL;
            else
                type = SA_NO_INFO;

            break;

        case POWER_SUPPLY_TYPE   :
            if(!strcasecmp(name,"default_power_rail"))
                type = SA_DEFAULT_POWER_RAIL;
            else 
                type = SA_NO_INFO;
            break;
            
        case WIRE_LOAD_TYPE   :
            if(!strcasecmp(name,"capacitance"))
                type = SA_CAPA ;
            else if(!strcasecmp(name,"resistance"))
                type = SA_RESI ;
            else 
                type = SA_NO_INFO;
            
            break;
    
        case LU_TEMPL_TYPE   :
            if(!strcasecmp(name,"variable_1"))
                type = SA_VAR1 ;
            else if(!strcasecmp(name,"variable_2"))
                type = SA_VAR2;
            else 
                type = SA_NO_INFO;
            
            break;
    
        case CELL_TYPE   :
            type = SA_NO_INFO;

            break;

        case TYPE_TYPE    :
            if(!strcasecmp(name,"base_type"))
                type = SA_BASE_TYPE;
            else if(!strcasecmp(name,"data_type"))
                type = SA_DATA_TYPE;
            else if(!strcasecmp(name,"bit_width"))
                type = SA_BIT_WIDTH;
            else if(!strcasecmp(name,"bit_from"))
                type = SA_BIT_FROM;
            else if(!strcasecmp(name,"bit_to"))
                type = SA_BIT_TO;
            else if(!strcasecmp(name,"downto"))
                type = SA_DOWNTO;
            else
                type = SA_NO_INFO;

            break;

        case BUS_TYPE    :
            if(!strcasecmp(name,"bus_type"))
                type = SA_BUS_TYPE;
            else if(!strcasecmp(name,"direction"))
                type = SA_DIR;
            else if(!strcasecmp(name,"clock"))
                type = SA_CK;
            else if(!strcasecmp(name,"capacitance"))
                type = SA_CAPA;
            else if(!strcasecmp(name,"function"))
                type = SA_FUNC;
            else if(!strcasecmp(name,"three_state"))
                type = SA_TS_FUNC;
            else if(!strcasecmp(name,"x_func"))
                type = SA_X_FUNC;
            else
                type = SA_NO_INFO;

            break;

        case PIN_TYPE    :
            if(!strcasecmp(name,"direction"))
                type = SA_DIR;
            else if(!strcasecmp(name,"input_signal_level"))
                type = SA_INPUT_SIGNAL_LEVEL;
            else if(!strcasecmp(name,"output_signal_level"))
                type = SA_OUTPUT_SIGNAL_LEVEL;
            else if(!strcasecmp(name,"clock"))
                type = SA_CK;
            else if(!strcasecmp(name,"capacitance"))
                type = SA_CAPA;
            else if(!strcasecmp(name,"function"))
                type = SA_FUNC;
            else if(!strcasecmp(name,"three_state"))
                type = SA_TS_FUNC;
            else if(!strcasecmp(name,"x_func"))
                type = SA_X_FUNC;
            else
                type = SA_NO_INFO;

            break;
           
        case TIMING_TYPE    :
            if(!strcasecmp(name,"related_pin"))
                type = SA_REL_PIN;
            else if(!strcasecmp(name,"timing_sense"))
                type = SA_TIM_SENS;
            else if(!strcasecmp(name,"timing_type"))
                type = SA_TIM_TYPE;
            else if(!strcasecmp(name,"intrinsic_rise"))
                type = SA_INTR_RISE;
            else if(!strcasecmp(name,"intrinsic_fall"))
                type = SA_INTR_FALL;
            else if(!strcasecmp(name,"slope_rise"))
                type = SA_SLOPE_RISE;
            else if(!strcasecmp(name,"slope_fall"))
                type = SA_SLOPE_FALL;
            else
                type = SA_NO_INFO;

            break;

        case DLY_RISE_TYPE  :
        case DLY_FALL_TYPE  :
        case SLW_RISE_TYPE  :
        case SLW_FALL_TYPE  :
        case RISE_CONSTR_TYPE:
        case FALL_CONSTR_TYPE:
            type = SA_NO_INFO;

            break;

        case FF_TYPE     :
            if(!strcasecmp(name, "clocked_on"))
                type = SA_CK_ON ;
            else if(!strcasecmp(name, "next_state"))
                type = SA_NEXT_ST ;
            else if(!strcasecmp(name, "clear"))
                type = SA_CLEAR ;
            else if(!strcasecmp(name, "preset"))
                type = SA_PRESET ;
            else if(!strcasecmp(name, "clear_preset_var1"))
                type = SA_CPVAR1 ;
            else if(!strcasecmp(name, "clear_preset_var2"))
                type = SA_CPVAR2 ;
            else if(!strcasecmp(name, "clocked_on_also"))
                type = SA_CK_ON_ALSO ;
            else
                type = SA_NO_INFO;

            break;

        case LATCH_TYPE  :
            if(!strcasecmp(name, "enable"))
                type = SA_ENABLE ;
            else if(!strcasecmp(name, "data_in"))
                type = SA_DATA ;
            else if(!strcasecmp(name, "clear"))
                type = SA_CLEAR ;
            else if(!strcasecmp(name, "preset"))
                type = SA_PRESET ;
            else if(!strcasecmp(name, "clear_preset_var1"))
                type = SA_CPVAR1 ;
            else if(!strcasecmp(name, "clear_preset_var2"))
                type = SA_CPVAR2 ;
            else
                type = SA_NO_INFO;
            break;

        default     :
            return SA_NO_INFO;

    }

    return type;
}

/*----------------------------------------------------------------------------*/
/* LIB_CREATE_S_ATTR :                                                        */
/* initialise les champs du nouvel attribut et renvoie le pointeur sur la     */
/* nouvelle structure allouee                                                 */
/*----------------------------------------------------------------------------*/
lib_s_attr *lib_create_s_attr(long type, char *val)
{
    lib_s_attr *p;
    
    p = (lib_s_attr*)mbkalloc(sizeof(lib_s_attr));
    p->OWNER = LIB_CURRENT_GP;
    p->TYPE = type;
    p->VALUE = val;

    return p;
}

/*----------------------------------------------------------------------------*/
/* LIB_ADD_S_ATTR :                                                           */
/* ajoute le nouveau simple attribute a la liste des simple attribute du      */
/* groupe courant.                                                            */
/*----------------------------------------------------------------------------*/
void    lib_add_s_attr(char *name, char *val)
{
    lib_s_attr *p;
    long t;

    if((t = lib_get_s_attr_type(name)) != SA_NO_INFO) {
        p = lib_create_s_attr(t, val);
        LIB_CURRENT_GP->S_ATTR = addchain(LIB_CURRENT_GP->S_ATTR, p);
        if(t == SA_TIME_UNIT)
            LIB_TIME_U = lib_get_time_unit(val);
        if(t == SA_RES_UNIT)
            LIB_RES_U = lib_get_res_unit(val);
    }

    else 
        mbkfree((char *)val);
}

/*----------------------------------------------------------------------------*/
/* LIB_GET_S_ATTR :                                                           */
/* Renvoie la valeur du simple attribute correspondant au nom et au groupe    */
/* passe en parametre                                                         */
/*----------------------------------------------------------------------------*/
char    *lib_get_s_attr_val(lib_group *g, long type)
{
    chain_list *p;
    lib_s_attr *a;

    if(!g || !(g->S_ATTR))
        return NULL;

    else {
        for(p=(g->S_ATTR) ; p ; p=p->NEXT) {
            a = (lib_s_attr *)(p->DATA);
            if((a->TYPE) == type)
                return (a->VALUE);
        }

        return NULL;
    }
}
                    
/*----------------------------------------------------------------------------*/
/* LIB_GET_C_ATTR_TYPE :                                                      */
/* Donne le type (code sur un long) d'un complex attribute en fonction de son */
/* nom.                                                                       */
/*----------------------------------------------------------------------------*/
long    lib_get_c_attr_type(char *name)
{
    long type;
        
    switch(lib_get_context())
    {
        case LIB_TYPE    :
            if(!strcasecmp(name,"capacitive_load_unit"))
                type = CA_CAPA_UNIT;
            else
                type = CA_NO_INFO;

            break;

        case POWER_SUPPLY_TYPE   :
            if(!strcasecmp(name,"power_rail"))
                type = CA_POWER_RAIL;
            break;
            
        case WIRE_LOAD_TYPE   :
            if(!strcasecmp(name,"fanout_length"))
                type = CA_FANOUT_LENGTH;
            else
                type = CA_NO_INFO;

            break;

        case WIRE_LOAD_TABLE_TYPE   :
            if(!strcasecmp(name,"fanout_length"))
                type = CA_FANOUT_LENGTH;
            else if(!strcasecmp(name,"fanout_capacitance"))
                type = CA_FANOUT_CAPACITANCE;
            else if(!strcasecmp(name,"fanout_resistance"))
                type = CA_FANOUT_RESISTANCE;
            else
                type = CA_NO_INFO;

            break;
        case LU_TEMPL_TYPE   :
            if(!strcasecmp(name,"index_1"))
                type = CA_INDEX1;
            else if(!strcasecmp(name,"index_2"))
                type = CA_INDEX2;
            else
                type = CA_NO_INFO;

            break;

        case CELL_TYPE   :
            if(!strcasecmp(name,"rail_connection"))
                type = CA_RAIL_CONNECTION;
            else
                type = CA_NO_INFO;

            break;

        case PIN_TYPE    :
            type = CA_NO_INFO;

            break;

        case BUS_TYPE    :
            type = CA_NO_INFO;

            break;

        
        case TIMING_TYPE :
            type = CA_NO_INFO;

        case DLY_RISE_TYPE  :
        case DLY_FALL_TYPE  :
        case SLW_RISE_TYPE :
        case SLW_FALL_TYPE :
        case RISE_CONSTR_TYPE:
        case FALL_CONSTR_TYPE:
            if(!strcasecmp(name,"index_1"))
                type = CA_INDEX1 ;
            else if(!strcasecmp(name,"index_2"))
                type = CA_INDEX2 ;
            else if(!strcasecmp(name,"values"))
                type = CA_VALUES ;
            else
                type = CA_NO_INFO;

            break;

        case FF_TYPE     :
            type = CA_NO_INFO;

            break;

        case LATCH_TYPE  :
            type = CA_NO_INFO;

            break;
                
        default     :
            return CA_NO_INFO;
    }

    return type;            
}


/*----------------------------------------------------------------------------*/
/* LIB_CREATE_C_ATTR :                                                        */
/* initialise les champs du nouvel attribut et renvoie le pointeur sur la     */
/* nouvelle structure allouee                                                 */
/*----------------------------------------------------------------------------*/
lib_c_attr *lib_create_c_attr(long type, chain_list *val)
{
    lib_c_attr *p;

    p = (lib_c_attr*)mbkalloc(sizeof(lib_c_attr));
    p->OWNER = LIB_CURRENT_GP;
    p->TYPE = type;
    p->VALUES = val;
    
    return p;
}


/*----------------------------------------------------------------------------*/
/* LIB_ADD_C_ATTR :                                                           */
/* ajoute le nouveau complex attribute a la liste des complex attribute du    */
/* groupe courant.                                                            */
/*----------------------------------------------------------------------------*/
void    lib_add_c_attr(char *name, chain_list *val)
{
    lib_c_attr *p;
    long t;
    chain_list *ch;

    if((t = lib_get_c_attr_type(name)) != CA_NO_INFO) {
        p = lib_create_c_attr(t, val);
        LIB_CURRENT_GP->C_ATTR = addchain(LIB_CURRENT_GP->C_ATTR, p);
        if(t == CA_CAPA_UNIT)
            LIB_CAPA_U = lib_get_capa_unit(val);
    }

    else { 
        for (ch=val ; ch ; ch=ch->NEXT)
            mbkfree((char *)(ch->DATA));
        freechain(val);
    }
}

/*----------------------------------------------------------------------------*/
/* LIB_GET_C_ATTR :                                                           */
/* Renvoie le pointeur de tete des valeur du complex attribute correspondant  */
/* au nom et au groupe passe en parametre.                                    */
/*----------------------------------------------------------------------------*/
chain_list  *lib_get_c_attr_val(lib_group *g, long type)
{
    chain_list *p;
    lib_c_attr *a;

    if(!g || !(g->C_ATTR))
        return NULL;

    else {
        for(p=(g->C_ATTR) ; p ; p=p->NEXT) {
            a = (lib_c_attr *)(p->DATA);
            if((a->TYPE) == type)
                return (a->VALUES);
        }

        return NULL;
    }

}

/*----------------------------------------------------------------------------*/
/* LIB_GET_SON_GROUP :                                                        */
/* Renvoie le pointeur sur la premiere structure du groupe fils qui           */
/* correspond au type indique en parametre.                                   */
/*----------------------------------------------------------------------------*/
lib_group   *lib_get_son_group(lib_group *g, long type)
{
    lib_group *p;
    
    if(!g || !(g->SONS))
        return NULL;

    else {
        for(p=g->SONS ; p ; p=p->NEXT) {
            if((p->TYPE) == type)
                return p;
        }
        
        
        return NULL;
    }

}
/*----------------------------------------------------------------------------*/
/* LIB_GET_TIME_UNIT :                                                        */
/* Extrait l'unite de temps et son facteur d'echelle a partir du simple       */
/* attribute dont le groupe et le nom sont passes en argument.                */
/* Renvoie le pointeur sur la structure scale_unit remplie.                   */
/*----------------------------------------------------------------------------*/
lib_scale_unit  *lib_get_time_unit(char *attr_val)
{
    lib_scale_unit  *s;
    char *p, *c, buf[10];
    int i;


    
    s = (lib_scale_unit *) mbkalloc (sizeof(lib_scale_unit));
    
    if((p=strstr(attr_val, "ns")) != NULL) 
        s->UNIT = LIB_NS;
    
    else if((p=strstr(attr_val, "ps")) != NULL) 
        s->UNIT = LIB_PS;

    for(i=0, c=attr_val ; c != p ; i++, c++)
        buf[i] = *c;
    buf[i]='\0';
    
    s->FACTOR = atof(buf);
    
    //lib_del_one_s_attr(LIB_CURRENT_GP,SA_TIME_UNIT);
    return s;
     
}

/*----------------------------------------------------------------------------*/
/* LIB_GET_RES_UNIT :                                                         */
/* Extrait l'unite de resistance et son facteur d'echelle a partir du simple  */
/* attribute dont le groupe et le nom sont passes en argument.                */
/* Renvoie le pointeur sur la structure scale_unit remplie.                   */
/*----------------------------------------------------------------------------*/
lib_scale_unit  *lib_get_res_unit(char *attr_val)
{
    lib_scale_unit  *s;
    char *p, *c, buf[10];
    int i;


    s = (lib_scale_unit *) mbkalloc (sizeof(lib_scale_unit));
    
    if((p=strstr(attr_val, "kohm")) != NULL) 
        s->UNIT = LIB_KOHM;
    
    else if((p=strstr(attr_val, "ohm")) != NULL) 
        s->UNIT = LIB_OHM;

    for(i=0, c=attr_val ; c != p ; i++, c++)
        buf[i] = *c;
    buf[i]='\0';
    
    s->FACTOR = atof(buf);
    //lib_del_one_s_attr(LIB_CURRENT_GP,SA_RES_UNIT);
    
    return s;
}

/*----------------------------------------------------------------------------*/
/* LIB_GET_CAPA_UNIT :                                                        */
/* Extrait l'unite de capacite et son facteur d'echelle a partir du simple    */
/* attribute dont le groupe et le nom sont passes en argument.                */
/* Renvoie le pointeur sur la structure scale_unit remplie.                   */
/*----------------------------------------------------------------------------*/
lib_scale_unit  *lib_get_capa_unit(chain_list *attr_val)
{
    lib_scale_unit  *s;
    char *u;

    
    s = (lib_scale_unit *) mbkalloc (sizeof(lib_scale_unit));
    u = (char *)(attr_val->NEXT->DATA);
    
    if(!strcasecmp(u, "pf")) 
        s->UNIT = LIB_PF;
    
    else if(!strcasecmp(u, "ff")) 
        s->UNIT = LIB_FF;

    s->FACTOR = atof((char *)(attr_val->DATA));
    lib_del_one_c_attr(LIB_CURRENT_GP,SA_RES_UNIT);
    return s;

}

/*----------------------------------------------------------------------------*/
/* LIB_DEL_GROUP :                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
lib_group    *lib_del_group(lib_group *g)
{
    lib_group *p, *n, *pr;

    if(g->SONS) {
        p=g->SONS;
        while(p) {
            p=lib_del_group(p);
        }
        g->SONS = NULL;
    }
    lib_del_all_s_attr(g);
    lib_del_all_c_attr(g);
    freechain(g->NAMES); 
    if(!(g->OWNER)) {
        mbkfree((lib_group *)g);
        return NULL;
    }

    else {
        
        if(g->OWNER->SONS == g) 
            g->OWNER->SONS=g->NEXT;    
        
        else {
            for(pr=g->OWNER->SONS ; ((pr->NEXT) && (pr->NEXT != g)) ; pr=pr->NEXT);
            if(pr->NEXT) {
                pr->NEXT = g->NEXT;
                if (g == g->OWNER->LAST_SON)
                    g->OWNER->LAST_SON = pr;
            }
            else  {           
                g->OWNER->SONS=NULL;
                g->OWNER->LAST_SON = NULL;
            }
        }
        
        n=g->NEXT;
        mbkfree((lib_group *)g);
        return n;
    }
        
}
/*----------------------------------------------------------------------------*/
/* LIB_DEL_ONE_S_ATTR :                                                       */
/*                                                                            */
/*----------------------------------------------------------------------------*/
void    lib_del_one_s_attr(lib_group *g, long sa_type)
{
    chain_list *p, *pr=NULL;
    lib_s_attr  *sa;
    
    p=g->S_ATTR;
    while(p){
        sa = p->DATA;
        if(sa->TYPE == sa_type){
            mbkfree((char *)(sa->VALUE));
            if(pr)
                pr->NEXT = p->NEXT;
            else
                g->S_ATTR = p->NEXT;
            mbkfree((lib_s_attr *)sa);
            mbkfree((chain_list *)p);
            p=NULL;
        }
        else {
            pr = p;
            p = p->NEXT;
        }
    }
        
}
/*----------------------------------------------------------------------------*/
/* LIB_DEL_ALL_S_ATTR :                                                       */
/*                                                                            */
/*----------------------------------------------------------------------------*/
void    lib_del_all_s_attr(lib_group *g)
{
    chain_list  *p;

    for(p=g->S_ATTR ; p ; p=p->NEXT) {
        mbkfree(((lib_s_attr *)(p->DATA))->VALUE);
        ((lib_s_attr *)(p->DATA))->OWNER = NULL;
        mbkfree((lib_s_attr *)(p->DATA));
    }
    freechain(g->S_ATTR);
    g->S_ATTR=NULL;
        
}

/*----------------------------------------------------------------------------*/
/* LIB_DEL_ONE_C_ATTR :                                                       */
/*                                                                            */
/*----------------------------------------------------------------------------*/
void    lib_del_one_c_attr(lib_group *g, long ca_type)
{
    chain_list *p, *pr=NULL, *ch;
    lib_c_attr  *ca;

    p=g->C_ATTR; 
    while(p) {
        ca = (lib_c_attr *)(p->DATA);
        if(ca->TYPE == ca_type){
            for(ch=ca->VALUES; ch ; ch=ch->NEXT) 
                mbkfree((char *)(ch->DATA));
            freechain(ca->VALUES);
            mbkfree((lib_c_attr *)ca);
                
            if(pr)
                pr->NEXT = p->NEXT;
            else
                g->C_ATTR = p->NEXT;
            mbkfree((chain_list *)p);
            p=NULL;
        }
        else {
            pr = p;
            p=p->NEXT;
        }
    }
        
}
/*----------------------------------------------------------------------------*/
/* LIB_DEL_ALL_C_ATTR :                                                       */
/*                                                                            */
/*----------------------------------------------------------------------------*/
void    lib_del_all_c_attr(lib_group *g)
{
    chain_list  *p, *ch;

    for(p=g->C_ATTR ; p ; p=p->NEXT) {
        for(ch=((lib_c_attr *)(p->DATA))->VALUES ; ch ; ch=ch->NEXT)
            mbkfree((char *)(ch->DATA));
        freechain(((lib_c_attr *)(p->DATA))->VALUES);
        mbkfree((lib_c_attr *)(p->DATA));
    }
    freechain(g->C_ATTR);
    g->C_ATTR=NULL;
        
}

/*----------------------------------------------------------------------------*/
/* LIB_TIM_AFF_HGP :                                                          */
/* afiche la hierarchie des groupes                                           */
/*----------------------------------------------------------------------------*/
void    lib_aff_hgp(lib_group *g, int ntabs)
{
    int i;
    lib_group *p;

    for(p=g ; p ; p=p->NEXT) {
        for(i=ntabs ; i > 0 ; i--)
            printf("\t");
        printf("%s(%s) {\n", lib_get_str_type(p->TYPE), (g->NAMES)? (char *)(p->NAMES->DATA):"");
        if(p->SONS)
            lib_aff_hgp(p->SONS, ntabs+1);
    }

}
    
/*----------------------------------------------------------------------------*/
/* LIB_TIM_GET_STR_TYPE :                                                     */
/* donne la chaine de caracteres correspondant au type du groupe              */
/*----------------------------------------------------------------------------*/
char     *lib_get_str_type(long type)
{
    switch(type) {
        case LIB_TYPE:
            return "LIBRARY";
        case LU_TEMPL_TYPE:
            return "LU_TABLE_TEMPLATE";
        case CELL_TYPE:
            return "CELL";
        case PIN_TYPE:
            return "PIN";
        case TIMING_TYPE:
            return "TIMING";
        case FF_TYPE:
            return "FF";
        case LATCH_TYPE:
            return "LATCH";
        case STATETABLE_TYPE:
            return "STATETABLE";
        case DLY_RISE_TYPE:
            return "CELL_RISE";
        case DLY_FALL_TYPE:
            return "CELL_FALL";
        case SLW_RISE_TYPE:
            return "RISE_TRANSITION";
        case SLW_FALL_TYPE:
            return "FALL_TRANSITION";
        case RISE_CONSTR_TYPE:
            return "RISE_CONSTRAINTS";
        case FALL_CONSTR_TYPE:
            return "FALL_CONSTRAINTS";

        default:
            return "UNKNOWN GROUP";
    }
            
}

/*----------------------------------------------------------------------------*/
/* LIB_FUNC_GET_BUS_SIZE                                                      */
/*                                                                            */
/*----------------------------------------------------------------------------*/
void    lib_func_get_bus_size (lib_group *g, int *b_from, int *b_to)
{
    char        *bus_type_name;
    lib_group   *type_gp;
    int          trouve = 0, width;
    char        *value;
    char         downto;
    
    

    bus_type_name = lib_get_s_attr_val (g, SA_BUS_TYPE);

    /* egt the good type group */
    type_gp = lib_get_son_group (LIB_GP_HEAD, TYPE_TYPE);
    while (type_gp && (trouve == 0)) {
        if (!strcasecmp((char *)type_gp->NAMES->DATA, bus_type_name))
            trouve = 1;
        else
            type_gp = type_gp->NEXT;
    }
   
    /* bit_from est donne */
    if ((value = lib_get_s_attr_val (type_gp, SA_BIT_FROM))) {
        *b_from = atoi (value);
        if ((value = lib_get_s_attr_val (type_gp, SA_BIT_TO)))
            *b_to = atoi (value);
        else {
            if ((value = lib_get_s_attr_val (type_gp, SA_BIT_WIDTH))) 
                width = atoi (value);
            else
                width = 1; /* valeur par defaut dans LIBERTY */

            if ((value = lib_get_s_attr_val (type_gp, SA_DOWNTO))) {
               if(!strcasecmp(value,"true"))
                       downto = 'Y';
               else
                       downto = 'N';
            }
            else
                downto = 'N'; /* valeur par defaut dans LIBERTY */


            if (downto == 'N') /* low to high */
               *b_to = *b_from + (width-1);
            else
               *b_to = *b_from - (width-1);
        }
    }

    /* bit_from n'est pas donne mais bit_to si */
    else if ((value = lib_get_s_attr_val (type_gp, SA_BIT_TO))) {
        *b_to = atoi (value);
        if ((value = lib_get_s_attr_val (type_gp, SA_BIT_WIDTH))) 
            width = atoi (value);
        else
            width = 1; /* valeur par defaut dans LIBERTY */


        if ((value = lib_get_s_attr_val (type_gp, SA_DOWNTO))) {
           if(!strcasecmp(value,"true"))
                   downto = 'Y';
           else
                   downto = 'N';
        }
        else
            downto = 'N'; /* valeur par defaut dans LIBERTY */


        if (downto == 'N') /* low to high */
           *b_from = *b_to - (width-1);
        else
           *b_from = *b_to + (width-1);
    }


    /* ni bit_from ni bit_to ne sont donnes mais bit_width si */
    else if ((value = lib_get_s_attr_val (type_gp, SA_BIT_WIDTH))) {
        width = atoi (value);
        if ((value = lib_get_s_attr_val (type_gp, SA_DOWNTO))) {
           if(!strcasecmp(value,"true"))
                   downto = 'Y';
           else
                   downto = 'N';
        }
        else
            downto = 'N'; /* valeur par defaut dans LIBERTY */

        if (downto == 'N') {
            *b_from = 0; /* valeur par defaut dans LIBERTY */
            *b_to   = *b_from + (width-1);
        }
        else {
            *b_to   = 0; /* valeur par defaut dans LIBERTY */
            *b_from = *b_to + (width-1);
        }
    }

    /* il n'a rien d'indique (ou que downto): par defaut bit_width=1  */
    /* donc bit_from = bit_to = 0 par defaut                          */
    else 
        *b_to = *b_from = 0;
    
 
}



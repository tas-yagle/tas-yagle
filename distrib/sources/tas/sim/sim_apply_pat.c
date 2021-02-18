/******************************************************************************/
/*                                                                            */
/*                      Chaine de CAO & VLSI   AVERTEC                        */
/*                                                                            */
/*    Produit : SIM Version 2.00                                              */
/*    Fichier : sim_apply_pat.c                                               */
/*                                                                            */
/*    (c) copyright 2000 AVERTEC                                              */
/*    Tous droits reserves                                                    */
/*                                                                            */
/*    Auteur(s) : Marc KUOCH                                                  */
/*                                                                            */
/******************************************************************************/

#include SIM_H
#include "sim.h"

//-----------------------------------------------------------------------------
//
char sim_get_type_node (sim_model *model,char *node)
{
    char                type_node='x';
    locon_list          *locon;
    losig_list          *losig;

    if (!model || !node) return type_node ;

    sim_find_by_name( node,
                      sim_model_get_lofig( model ),
                      &locon,
                      &losig,
                      NULL
                    );
    if( losig )
      type_node = 's';
    if( locon )
      type_node = 'c';
    return type_node;
}

//-----------------------------------------------------------------------------
//
void sim_apply_input (sim_model *model, spisig_list *path, 
                      SIM_FLOAT start_time,SIM_FLOAT slope, int delay_type)
{
    spisig_list *firstsig;
    char        *name,*slopelabel,*delaylabel,*nrc1;
    long         event;
    char         sens,buf[3],transition[3];
    sim_measure *measure;
    int          nbck=0, nbdata=0, ick=0, idata=0 ;
    char         deltavarbuf[1024];
    double vdd, vss;
    double vssl, vssh, vddl, vddh;

    for( firstsig = path ; firstsig ; firstsig = firstsig->NEXT ) {
      if( firstsig->START ) {
        if( firstsig->CLK ) 
          nbck++;
        else
          nbdata++;
      }
    }

    for( firstsig = path ; firstsig ; firstsig = firstsig->NEXT ) {
      if( !firstsig->START )
        continue ;

      event = efg_GetSpiSigEvent (firstsig);
      if (event == ((long)EFG_SIG_SET_RISE))
          sens = SIM_RISE;
      else
          sens = SIM_FALL;
      name = efg_GetSpiSigName (firstsig);
      SIM_VT_SLOPEIN = firstsig->VTH;
  
      if( V_INT_TAB[__SIM_TOOL].VALUE == SIM_TOOL_NGSPICE ) {
        deltavarbuf[0]='\0';
      }
      else {
        if( firstsig->CLK ) {
          ick++;
          if( nbck==1 )
            sprintf( deltavarbuf, "SKEWCK" );
          else
            sprintf( deltavarbuf, "SKEWCK%d", ick );
        }
        else {
          idata++;
          if( nbdata==1 ) 
            sprintf( deltavarbuf, "SKEWDATA" );
          else
            sprintf( deltavarbuf, "SKEWDATA%d", idata );
        }
      }
      
      vdd=sim_parameter_get_alim(model, 'i');
      vss=sim_parameter_get_vss(model, 'i');

      sim_get_multivoltage_values(firstsig->CONE, firstsig->SRCSIG, &vssl, &vssh, &vddl, &vddh);

      sim_parameter_set_alim(model, vddh, 'i');
      sim_parameter_set_vss(model, vssl, 'i');

      sim_input_set_slope_tanh (model,
                                name,
                                start_time, 
                                slope,
                                sens,
                                deltavarbuf[0]=='\0' ? NULL : deltavarbuf 
                              );

      sim_parameter_set_alim(model, vdd, 'i');
      sim_parameter_set_vss(model, vss, 'i');

      SIM_VT_SLOPEIN = -1.0;
      
      if ( V_BOOL_TAB[__SIM_USE_PRINT].VALUE )
        measure = sim_measure_set_locon (model,name);
/*      if (efg_IsWireOnSpiSig (firstsig) == 1) {
        sprintf (transition ,"%c%c",sens,sens);
        slopelabel = sim_create_label ( name,NULL );
        sprintf (buf,"%c",sens);
        sim_addMeasSlope(model, 
                         slopelabel,
                         name, buf, delay_type);
        nrc1 =  efg_GetSpiSigLoconRc (firstsig,'O');
        delaylabel = sim_create_label ( name, nrc1 );
        slopelabel = sim_create_label ( nrc1, NULL );
        sim_add_rc_label_slope ( firstsig, slopelabel);
        sim_add_rc_label_delay ( firstsig, delaylabel);
        sim_addMeas (model, 
                     delaylabel,
                     slopelabel,
                     name, nrc1,
                     transition,
                     delay_type
                     );

      }
*/
    }
}

//-----------------------------------------------------------------------------
//

void sim_print_node (sim_model *model, chain_list *sig2print)
{
    char       *node;
    chain_list *chain;
    locon_list *locon;
    losig_list *losig;

    if (!model || !sig2print)
    return;

    for (chain = sig2print ; chain ; chain = chain->NEXT) {
        node = (char*)chain->DATA;
        sim_find_by_name( node,
                          sim_model_get_lofig( model ),
                          &locon,
                          &losig,
                          NULL
                        );
        if( losig )
            sim_measure_set_signal (model, node);
        else {
            if( locon )
                sim_measure_set_locon (model, node);
        }
    }
}

//-----------------------------------------------------------------------------
//
void sim_apply_correl (sim_model *model, chain_list *correl, cnsfig_list *cnsfig)
{
    double vssl, vssh, vddl, vddh;
    cone_list *cone;
    losig_list *ls;
    char *dname, *oname, *vddname, *vssname;
    chain_list *cor;
    gsp_correl *gc;
    
    for (cor=correl; cor!=NULL; cor=cor->NEXT)
    {
      gc=(gsp_correl *)cor->DATA;
      if ((dname=efg_get_origsigname((losig_list *)gc->dest))==NULL) dname=getsigname((losig_list *)gc->dest);
      if ((oname=efg_get_origsigname((losig_list *)gc->orig))==NULL) oname=getsigname((losig_list *)gc->orig);
      
      cone=getcone(cnsfig, 0, oname);
      sim_get_multivoltage_values(cone, NULL, &vssl, &vssh, &vddl, &vddh);
      ls=efg_try_to_add_alim_to_the_context(NULL, vddh);
      if ((vddname=efg_get_origsigname(ls))==NULL) vddname=getsigname(ls);
      ls=efg_try_to_add_alim_to_the_context(NULL, vssl);
      if ((vssname=efg_get_origsigname(ls))==NULL) vssname=getsigname(ls);
      sim_input_set_slope_mimic( model,  dname, oname, vddname, vssname, gc->tog?1:0);
    }
}

void sim_apply_stuck (sim_model *model, ptype_list *stucklist, cnsfig_list *cnsfig)
{
    char       *node;
    ptype_list *ptype;
    char        stucklevel;
    double vssl, vssh, vddl, vddh;
    cone_list *cone;
    losig_list *ls;
    char *orgname;
    
    if ((!model || !stucklist) || (gsp_is_pat_indpd(stucklist) == 'Y')) 
        return;
    for (ptype = stucklist ; ptype ; ptype = ptype->NEXT) {
        if (ptype->TYPE == GSP_STUCK_INDPD) continue;
        node = (char*)ptype->DATA;
        if (ptype->TYPE == GSP_STUCK_ONE)
            stucklevel = SIM_ONE;
        else
            stucklevel = SIM_ZERO;
        ls=mbk_quickly_getlosigbyname(model->FIG, node);
        if ((orgname=efg_get_origsigname(ls))==NULL) orgname=getsigname(ls);
        cone=getcone(cnsfig, 0, orgname);
        if (cone!=NULL)
        {
          sim_get_multivoltage_values(cone, NULL, &vssl, &vssh, &vddl, &vddh);
          if (stucklevel == SIM_ONE)
            sim_input_set_stuck_voltage(model, node, vddh);
          else
            sim_input_set_stuck_voltage(model, node, vssl);
        }
        else
          sim_input_set_stuck_level (model, node,stucklevel);
    }
}

//-----------------------------------------------------------------------------
// contient les infos sur les nodes (event...) du chemin a sensibiliser
// le 1er element du path est le dernier noeud du chemin a sensibiliser
// le dernier element du path est le 1er noeud du chemin a sensibiliser

static int avoid_ic(spisig_list *ssig)
{
  if ((ssig->CONE->TYPE & (CNS_LATCH|CNS_RS|CNS_FLIP_FLOP|CNS_PRECHARGE|CNS_MEMSYM|CNS_MASTER|CNS_SLAVE))!=0) return 0;
  if (ssig->ADDIC) return 0;
  if ((ssig->CONE->TECTYPE & CNS_DUAL_CMOS)==CNS_DUAL_CMOS) return 1;
  return 0;
}

void sim_apply_init_print (sim_model *model, spisig_list *path)
{
    spisig_list *spisig;
    char       *name;
    char        level;
    char        locate;
    long        event;
    sim_measure *measure;
    // sim_ic     *ic;
    locon_list *locon;
    losig_list *losig;
    double vssl, vssh, vddl, vddh, levelv;
    ptype_list *pt, *p1;
    int lv, xm;

    if (!model || !path)
        return;

    if ((pt=getptype(model->FIG->USER, EFG_ADDITIONAL_IC))!=NULL)
    {
      ptype_list *pt0;
      cone_list *cn;
      pt0=(ptype_list *)pt->DATA;
      while (pt0!=NULL)
      {
        cn=(cone_list *)pt0->DATA;
        sim_get_multivoltage_values(cn, NULL, &vssl, &vssh, &vddl, &vddh);
        xm=1;
        if ((p1=getptype(cn->USER, EFG_CONE_STUCK_GSP))!=NULL)
          xm=(int)(long)p1->DATA;
        if (xm==0) levelv=vssl; else levelv=vddh;
        cn->USER=testanddelptype(cn->USER, EFG_CONE_STUCK_GSP);
        if (!sim_ic_get (model,(char *)pt0->TYPE,SIM_IC_SIGNAL) )
           sim_ic_set_voltage(model,(char *)pt0->TYPE, SIM_IC_SIGNAL,levelv);
        pt0=pt0->NEXT;
      }
      freeptype((ptype_list *)pt->DATA);
      model->FIG->USER=delptype(model->FIG->USER, EFG_ADDITIONAL_IC);
    }
    
    // Init internal node on path
    for (spisig = path; spisig->NEXT ; spisig=spisig->NEXT) {
        event = efg_GetSpiSigEvent (spisig);

        sim_get_multivoltage_values(spisig->CONE, spisig->SRCSIG, &vssl, &vssh, &vddl, &vddh);

        if (spisig->ADDIC & EFG_ADDIC_FORCE_1) lv=1;
        else if (spisig->ADDIC & EFG_ADDIC_FORCE_0) lv=0;
        else if (event == ((long)EFG_SIG_SET_RISE)) lv=0;
        else lv=1;
        if (lv==0)
            level = SIM_ZERO, levelv=vssl;
        else
            level = SIM_ONE, levelv=vddh;
        name = efg_GetSpiSigName  (spisig);

        // NO RC on signal
        if (!efg_IsWireOnSpiSig (spisig) ) {
          sim_find_by_name( name,
                            sim_model_get_lofig( model ),
                            &locon,
                            &losig,
                            NULL
                          );
          if( locon )
              locate = SIM_IC_LOCON;
          else
              locate = SIM_IC_SIGNAL;
          if (!avoid_ic(spisig) && !sim_ic_get (model,name,locate) )
             sim_ic_set_voltage(model,name, locate,levelv);
//            sim_ic_set_level (model,name, locate,level);
          if (locate == SIM_IC_SIGNAL)
              sim_measure_set_signal (model, name);
          else
              sim_measure_set_locon (model, name);
        }
        else {
            locate = SIM_IC_LOCON;
            if (spisig == path || spisig->DESTSIG->TYPE == 'E') {
              measure = sim_measure_set_locon (model, name);
              if (!avoid_ic(spisig) && !sim_ic_get (model,name,locate) )
                sim_ic_set_voltage(model,name, SIM_IC_LOCON,levelv);
              //ic = sim_ic_set_level (model,name, SIM_IC_LOCON,level);
            }
            if ((name = efg_GetSpiSigLoconRc (spisig,'I'))) {
              measure = sim_measure_set_locon (model, name);
              if (!avoid_ic(spisig) && !sim_ic_get (model,name,locate) )
                sim_ic_set_voltage(model,name, SIM_IC_LOCON,levelv);
//                ic = sim_ic_set_level (model,name, SIM_IC_LOCON,level);
            }
            if ((name = efg_GetSpiSigLoconRc (spisig,'O'))) {
              measure = sim_measure_set_locon (model, name);
              if (!avoid_ic(spisig) && !sim_ic_get (model,name,locate) )
                sim_ic_set_voltage(model,name, SIM_IC_LOCON,levelv);
//                ic = sim_ic_set_level (model,name, SIM_IC_LOCON,level);
            }
        }
    }
}

//-----------------------------------------------------------------------------
//

sim_model *sim_apply_pat (lofig_list *lofig, SIM_FLOAT start_time,
                          SIM_FLOAT slope, ptype_list *patterns, 
                          spisig_list *path, int runspice, int delay_type,
                          char multiplepath, cnsfig_list *cf, int nbmc, chain_list *correl )
{
    sim_model *model = NULL;
    chain_list *added_capa = NULL;
    char *lastsig;
    
    if (!lofig)
    return NULL;
    else {
        if (SIM_OUT_CAPA_VAL > 0.0 && runspice == 1) {
          lastsig = efg_GetSpiSigName (efg_GetLastSpiSig (path));
          added_capa = efg_set_extra_capa (lofig,lastsig,SIM_OUT_CAPA_VAL*1.0e-3);
        }
        model = sim_model_create (lofig->NAME,nbmc);
        sim_model_set_lofig( model, lofig );
        sim_parameter_set_drive_netlist( model, SIM_YES );
        
        sim_apply_alims ( model, lofig );
        if (slope > 0.0)
          sim_apply_input (model,path,start_time,slope,delay_type);
        sim_apply_stuck (model,patterns,cf);
        sim_apply_correl (model, correl, cf);

        sim_apply_init_meas (model, path, delay_type,multiplepath);
        if( !multiplepath ) {
          if ((runspice))
            sim_run_simu_spice( model, 1, NULL, NULL );
        }
        if ( added_capa )
          efg_del_extra_capa ( added_capa );
    }
    return model;
}

//-----------------------------------------------------------------------------
//

sim_model *sim_apply_pat2cone (lofig_list *origlofig,cnsfig_list *cnsfig, cone_list *cone,
                               char *input, char in_event, SIM_FLOAT slope, char out_event, float capaout, int delay_type)
{
    chain_list  *pathsig = NULL;
    ptype_list  *spipatterns = NULL;
    spisig_list *path = NULL;
    lofig_list  *figext = NULL;
    sim_model   *model = NULL;
    chain_list  *extra_capa = NULL;
    chain_list  *chaincone = NULL;
    chain_list  *chaininstance = NULL;
    chain_list  *cone_onpath = NULL, *allcorrel=NULL;
    SIM_FLOAT    start_time = 0.0;
    int err;
    // TODO mettre le start_time en parametre

    // build spisig
    if (out_event == SIM_RISE)
        pathsig = efg_AddPathEvent (pathsig,cone->NAME,EFG_RISE,0,-1,-1, -1, -1, NULL, 0, NULL);
    else
        pathsig = efg_AddPathEvent (pathsig,cone->NAME,EFG_FALL,0,-1,-1, -1, -1, NULL, 0, NULL);
    if (in_event == SIM_RISE)
        pathsig = efg_AddPathEvent (pathsig,input,EFG_RISE,0,-1,-1, -1, -1, NULL, 0, NULL);
    else
        pathsig = efg_AddPathEvent (pathsig,input,EFG_FALL,0,-1,-1, -1, -1, NULL, 0, NULL);

    path = efg_BuildSpiSigList(origlofig,pathsig,NULL,1,&err,0);
    
    // build figext and find  pat   
    if (cone != NULL)
        cone_onpath = addchain (cone_onpath,cone);

    efg_extract_fig ( origlofig,
                      &figext,
                      cnsfig,
                      path,
 	 				  &chaincone,
 	 				  &chaininstance,
 	 				  NULL,
 	 				  NULL,
 	 				  NULL,
 	 				  NULL,
 	 				  cone_onpath,
                      NULL,
                      1
                    );

    // Add capa out 
    extra_capa = efg_set_extra_capa (figext,cone->NAME,capaout*1.0e12);

    spipatterns =  gsp_get_patterns(origlofig,
                                    cnsfig,
                                    figext,
                                    chaincone,
                                    chaininstance,
                                    delay_type,
                                    path,
                                    &allcorrel
                                    );

    // launch simu 
    model = sim_apply_pat (figext,
                           start_time,
                           slope, 
                           spipatterns, 
                           path,
                           1,
                           delay_type,
                           0,
                           cnsfig,
                           0,
                           allcorrel
                           );
    gsp_freecorrel(allcorrel);
    // liberation
    if (extra_capa != NULL) {
        efg_del_extra_capa (extra_capa);
        freechain (extra_capa);
    }
    if (path != NULL) efg_FreeSpiSigList ( path );
    if (spipatterns != CBH_GOOD_TRANS) freeptype (spipatterns);
    if (pathsig != NULL) efg_FreePathEvent (pathsig);
    if (cone_onpath != NULL) freechain (cone_onpath);

    return model;
}

//-----------------------------------------------------------------------------
//

void sim_get_delay_slope_cone (lofig_list *origlofig,
                               cnsfig_list *cnsfig,
                               cone_list *cone,
                               char *input,
                               char  in_event,
                               SIM_FLOAT in_slope, 
                               char out_event,
                               SIM_FLOAT capaout,
                               SIM_FLOAT *delay,
                               SIM_FLOAT *slope,
                               int delay_type)
{  
    char type_output;
    char type_input;
    char transition[3];
    sim_model *model = NULL;
    SIM_FLOAT vth_low,vth_high;
    char *output;
    char extfigname[1024];

    if (!origlofig || !cnsfig || !cone) return;
    sprintf (extfigname,"%s_ext",origlofig->NAME);
    output = cone->NAME;
    model = sim_apply_pat2cone (origlofig,
                                cnsfig,
                                cone,
                                input,
                                in_event,
                                in_slope,
                                out_event,
                                capaout,
                                delay_type);
    if (!model) {
        fprintf (stderr, "[SIM WAR] simulation failed on figure %s !!!\n",origlofig->NAME);
        *delay = 0.0;
        *slope = 0.0;
        if (getloadedlofig (extfigname) != NULL)
            dellofig (extfigname);
    }
    else {
        // read spice file    
        sim_parse_spiceout (model);
        // get type of node    
        sprintf (transition,"%c%c",in_event,out_event);
        type_input = sim_get_type_node (model,input);
        type_output = sim_get_type_node (model,output);




        // get delay
        sim_getMeasDelay (model, sim_create_label (input,output), delay);
/*        *delay = sim_get_trans_delay (model, SIM_MAX, type_input, input,
                                     type_output, output, transition);
  */      
        vth_high = sim_parameter_get_slopeVTHH (model);
        vth_low  = sim_parameter_get_slopeVTHL (model);

        sim_getMeasSlope (model, sim_create_label (output,NULL), slope);

//        *slope    = sim_getslope (model, SIM_MAX, type_output, output);
        sim_timing_free_all ();
        sim_model_clear (extfigname);
        dellofig (extfigname);
    }
}

//-----------------------------------------------------------------------------
// Access func for rc label slope
//
void sim_add_label (spisig_list *spisig,  long type, char *label)
{
  ptype_list *ptype;

  if ( spisig ) 
    {
      ptype = getptype (spisig->USER,SIM_RC_SLOPE_LABEL);
      if ( !ptype ) ptype=spisig->USER = addptype ( spisig->USER, SIM_RC_SLOPE_LABEL, NULL);
      ptype->DATA = addptype ( (ptype_list *)ptype->DATA, type, label);
    }
}

//-----------------------------------------------------------------------------
void sim_free_label (spisig_list *spisig)
{
  ptype_list *ptype;

  if ( spisig ) 
    {
      if ((ptype = getptype (spisig->USER, SIM_RC_SLOPE_LABEL))) 
        {
          freeptype((ptype_list*)ptype->DATA);
          spisig->USER = delptype (spisig->USER,SIM_RC_SLOPE_LABEL);
        }
    }
}


//-----------------------------------------------------------------------------
char *sim_get_label_delay (spisig_list *spisig, long type)
{
  ptype_list *ptype;

  if ( spisig ) 
    {
      if ((ptype = getptype (spisig->USER, SIM_RC_SLOPE_LABEL)))
        {
          if ((ptype = getptype ((ptype_list *)ptype->DATA, type))!=NULL)
            return (char*)ptype->DATA;
        }
    }
  return NULL;
}

//-----------------------------------------------------------------------------
//
char *sim_create_label ( char *name1, char *name2)
{
  char buf[1024], buf1[1024];

  name1=sim_devect(name1);
  if ( name2 ) {
    name2=sim_devect(name2);
    // delay label
    strcpy(buf1, spi_makename(mbk_vect(name2,'_','\0')));
    sprintf (buf,"%s_%s___%s",SIM_DELAY_LABEL_STRING,spi_makename(mbk_vect(name1,'_','\0')),buf1);
  }
  else 
    // slope label
    sprintf (buf,"%s_%s",SIM_SLOPE_LABEL_STRING,spi_makename(mbk_vect(name1,'_','\0')));
  return namealloc (buf);
}

//-----------------------------------------------------------------------------
// contient les infos sur les nodes (event...) du chemin a sensibiliser
// le 1er element du path est le dernier noeud du chemin a sensibiliser
// le dernier element du path est le 1er noeud du chemin a sensibiliser
// Drive des .meas au lieu de .print

void sim_apply_delay_to_vt(sim_model *model, spisig_list *spisig, char *name, char event, int delay_type)
{
  double savevthstart, savevthend;
  double vthlow,vthhigh, vssl, vssh, vddl, vddh;
  float factor;
  char transition[3];

  savevthstart=sim_parameter_get_delayVTHSTART(model);
  savevthend=sim_parameter_get_delayVTHEND(model);
  sim_get_multivoltage_values(spisig->CONE, spisig->SRCSIG, &vssl, &vssh, &vddl, &vddh);
  if (event=='D') factor=spisig->VTHHZ/(vddh-vssl);
  else factor=(vddh-spisig->VTHHZ)/(vddh-vssl);
  sim_parameter_set_alim(model, vddh, 'i');
  sim_parameter_set_vss(model, vssl, 'i');
  sim_parameter_set_alim(model, vddh, 'o');
  sim_parameter_set_vss(model, vssl, 'o');
  sim_parameter_set_delayVTH(model, savevthstart, factor);

  sprintf (transition,"%c%c",event,event);
  sim_addMeas (model, namealloc(SIM_DELAY_TO_VT_LABEL), NULL, name, name, transition, delay_type);

  sim_parameter_set_delayVTH(model, savevthstart, savevthend);
}


void sim_apply_init_meas (sim_model *model, spisig_list *path, int delay_type, char multiplepath)
{
    spisig_list *spisig,*firstspisig,*lastspisig;
    char       *name,*name2,
               *totaldelaylabel,
               *delaylabel,
               *slopelabel;
    char        level,transition[3];
    char       *nrc1,*nrc2;
    char        locate,buf[3];
    long        event;
    char        event1,event2;
    locon_list  *locon;
    losig_list  *losig;
    double      vthlow,vthhigh, vssl, vssh, vddl, vddh, levelv;
    double savevthstart, savevthend;
    ptype_list *pt;
    int lv, xm;

    if (!model || !path)
        return;

    if ((pt=getptype(model->FIG->USER, EFG_ADDITIONAL_IC))!=NULL)
    {
      ptype_list *pt0, *p1;
      cone_list *cn;
      pt0=(ptype_list *)pt->DATA;
      while (pt0!=NULL)
      {
        cn=(cone_list *)pt0->DATA;
        sim_get_multivoltage_values(cn, NULL, &vssl, &vssh, &vddl, &vddh);
        xm=1;
        if ((p1=getptype(cn->USER, EFG_CONE_STUCK_GSP))!=NULL)
          xm=(int)(long)p1->DATA;
        if (xm==0) levelv=vssl; else levelv=vddh;
        cn->USER=testanddelptype(cn->USER, EFG_CONE_STUCK_GSP);
        if (!sim_ic_get (model,(char *)pt0->TYPE,SIM_IC_SIGNAL) )
           sim_ic_set_voltage(model,(char *)pt0->TYPE, SIM_IC_SIGNAL,levelv);
        pt0=pt0->NEXT;
      }
      freeptype((ptype_list *)pt->DATA);
      model->FIG->USER=delptype(model->FIG->USER, EFG_ADDITIONAL_IC);
    }
    
    vthlow = sim_parameter_get_slopeVTHL (model);
    vthhigh = sim_parameter_get_slopeVTHH (model);

    for (spisig = path; spisig ; spisig=spisig->NEXT) 
      {
        if (!spisig->START)
          {
            event = efg_GetSpiSigEvent (spisig);
            name = efg_GetSpiSigName  (spisig);
            sim_get_multivoltage_values(spisig->CONE, spisig->SRCSIG, &vssl, &vssh, &vddl, &vddh);
            if (spisig->ADDIC & EFG_ADDIC_FORCE_1) lv=1;
            else if (spisig->ADDIC & EFG_ADDIC_FORCE_0) lv=0;
            else if (event == ((long)EFG_SIG_SET_RISE)) lv=0;
            else lv=1;

            if (spisig->LATCHCMDDIR & EFG_SPISIG_HZ_MASK) lv=(lv+1)&1;

            if (lv==0) levelv=vssl;
            else levelv=vddh;

            sim_find_by_name( name, sim_model_get_lofig(model), &locon, &losig, NULL);

            if( locon ) locate = SIM_IC_LOCON; else locate = SIM_IC_SIGNAL;

            if (!avoid_ic(spisig) && !sim_ic_get (model,name,locate) )
              sim_ic_set_voltage(model,name, locate,levelv);
          }
      }
    if (!multiplepath)
      {
        for (spisig = path; spisig ; spisig=spisig->NEXT) 
          {
            event = efg_GetSpiSigEvent (spisig);
            if (event == ((long)EFG_SIG_SET_RISE)) event1 = 'U'; else event1 = 'D';
            if(!spisig->START)
              {
                if ( spisig->NEXT ) 
                  {
                    event = efg_GetSpiSigEvent (spisig->NEXT);
                    if (event == ((long)EFG_SIG_SET_RISE)) event2 = 'U'; else event2 = 'D';

                    name2 = efg_GetSpiSigLoconRc (spisig->NEXT,'O');
                    if (!name2) name2 = efg_GetSpiSigName  ( spisig->NEXT );

                    savevthstart=sim_parameter_get_delayVTHSTART(model);
                    savevthend=sim_parameter_get_delayVTHEND(model);
                    if (spisig->LATCHCMDDIR & EFG_SPISIG_HZ_MASK)
                      {
                        char buf[1024];                    
                        double factor;

                        name=name2;
                        sprintf(buf, "%s__HZ", efg_GetSpiSigName  (spisig));
                        delaylabel = sim_create_label ( name2, buf );
                        sim_get_multivoltage_values(spisig->NEXT->CONE, spisig->NEXT->SRCSIG, &vssl, &vssh, &vddl, &vddh);
                    
                        if (event1=='D') factor=spisig->VTHHZ/(vddh-vssl);
                        else factor=(vddh-spisig->VTHHZ)/(vddh-vssl);

                        sim_parameter_set_delayVTH(model, savevthstart, factor);

                        sim_parameter_set_alim(model, vddh, 'o');
                        sim_parameter_set_vss(model, vssl, 'o');
                      }
                    else
                      {
                        name = efg_GetSpiSigLoconRc (spisig,'I');
                        if (!name) name = efg_GetSpiSigName  (spisig);
                        delaylabel = sim_create_label ( name2, name );
                        sim_get_multivoltage_values(spisig->CONE, spisig->SRCSIG, &vssl, &vssh, &vddl, &vddh);
                        sim_parameter_set_alim(model, vddh, 'o');
                        sim_parameter_set_vss(model, vssl, 'o');
                      }


                    sim_get_multivoltage_values(spisig->NEXT->CONE, spisig->NEXT->SRCSIG, &vssl, &vssh, &vddl, &vddh);
                    sim_parameter_set_alim(model, vddh, 'i');
                    sim_parameter_set_vss(model, vssl, 'i');

                    sprintf (transition,"%c%c",event2,event1);
                    sim_addMeas (model, delaylabel, NULL, name2, name, transition, delay_type);
                    sim_add_label(spisig, SIM_DELAY_LABEL, delaylabel);

                    sim_parameter_set_delayVTH(model, savevthstart, savevthend);

                    if (!(spisig->LATCHCMDDIR & EFG_SPISIG_HZ_MASK))
                      {
                        // Slope threshold
                        if ( spisig->VTHLOW > 0.0 ) sim_parameter_set_slopeVTHL (model,spisig->VTHLOW);
                        else sim_parameter_set_slopeVTHL (model,vthlow);
                        if ( spisig->VTHHIGH > 0.0 ) sim_parameter_set_slopeVTHH (model,spisig->VTHHIGH);
                        else sim_parameter_set_slopeVTHH (model,vthhigh);
                    
                        slopelabel = sim_create_label ( name, NULL );
                    
                        sprintf ( buf, "%c",event1);
                    
                        sim_addMeasSlope (model, slopelabel, name, buf, delay_type);
                        sim_add_label(spisig, SIM_SLOPE_LABEL, slopelabel);
                      }

                    if (spisig->LATCHCMDDIR & EFG_SPISIG_DELAY_TO_HZ_MASK)
                      sim_apply_delay_to_vt(model, spisig, name, event1, delay_type);
                  }
              }

            if (efg_IsWireOnSpiSig (spisig))
              {
                sim_get_multivoltage_values(spisig->CONE, spisig->SRCSIG, &vssl, &vssh, &vddl, &vddh);
                sim_parameter_set_alim(model, vddh, 'i');
                sim_parameter_set_vss(model, vssl, 'i');
                sim_parameter_set_alim(model, vddh, 'o');
                sim_parameter_set_vss(model, vssl, 'o');
                nrc1 = efg_GetSpiSigLoconRc (spisig,'I');
                if (!nrc1) nrc1 = efg_GetSpiSigName  (spisig);
                nrc2 = efg_GetSpiSigLoconRc (spisig,'O');
                if (!nrc2) nrc2 = efg_GetSpiSigName  (spisig);
                
                slopelabel = sim_create_label ( nrc2, NULL );
                sim_addMeasSlope(model, slopelabel, nrc1, &event1, delay_type);
                sim_add_label(spisig, SIM_RC_SLOPE_LABEL, slopelabel);
                delaylabel = sim_create_label ( nrc1, nrc2 );
                sim_add_label(spisig, SIM_RC_DELAY_LABEL, delaylabel);
                sprintf ( transition , "%c%c",event1,event1);
                sim_addMeas (model, delaylabel, slopelabel, nrc1, nrc2, transition, delay_type);

                if (spisig->LATCHCMDDIR & EFG_SPISIG_DELAY_TO_HZ_MASK)
                  sim_apply_delay_to_vt(model, spisig, nrc2, event1, delay_type);

              }

            if (spisig->END && (pt=getptype(spisig->USER, EFG_SPISIG_LOOP_SPISIG))!=NULL)
              {
                spisig_list *loopsig;
                loopsig=(spisig_list *)pt->DATA;
                event = efg_GetSpiSigEvent (loopsig);
                if (event == ((long)EFG_SIG_SET_RISE)) event2 = 'U'; else event2 = 'D';
                
                name2 = efg_GetSpiSigLoconRc (loopsig,'O');
                if (!name2) name2 = efg_GetSpiSigName  ( loopsig );
                name = efg_GetSpiSigLoconRc (spisig,'O');
                if (!name) name = efg_GetSpiSigName  (spisig);
                delaylabel = namealloc(SIM_LOOP_FEEDBACK_DELAY_LABEL);
                sim_get_multivoltage_values(spisig->CONE, spisig->SRCSIG, &vssl, &vssh, &vddl, &vddh);
                sim_parameter_set_alim(model, vddh, 'o');
                sim_parameter_set_vss(model, vssl, 'o');
                sim_get_multivoltage_values(loopsig->CONE, spisig->SRCSIG, &vssl, &vssh, &vddl, &vddh);
                sim_parameter_set_alim(model, vddh, 'i');
                sim_parameter_set_vss(model, vssl, 'i');                
                sprintf (transition,"%c%c",event1,event2);
                sim_addMeas (model, delaylabel, NULL, name, name2, transition, delay_type);
              }
          }
      }


#if 0
    // Init internal node on path
    for (spisig = path; spisig ; spisig=spisig->NEXT) 
      {

        if( spisig->START )
          continue ;

        nrc1=nrc2=NULL;

        event = efg_GetSpiSigEvent (spisig);

        // Slope threshold
        if ( spisig->VTHLOW > 0.0 )
           sim_parameter_set_slopeVTHL (model,spisig->VTHLOW);
        else
           sim_parameter_set_slopeVTHL (model,vthlow);
        if ( spisig->VTHHIGH > 0.0 )
           sim_parameter_set_slopeVTHH (model,spisig->VTHHIGH);
        else
           sim_parameter_set_slopeVTHH (model,vthhigh);

        sim_get_multivoltage_values(spisig->CONE, &vssl, &vssh, &vddl, &vddh);
        if (spisig->ADDIC & EFG_ADDIC_FORCE_1) lv=1;
        else if (spisig->ADDIC & EFG_ADDIC_FORCE_0) lv=0;
        else if (event == ((long)EFG_SIG_SET_RISE)) lv=0;
        else lv=1;
        if (lv==0) {
            level = SIM_ZERO;
            levelv=vssl;
        }
        else {
            level = SIM_ONE;
            levelv=vddh;
        }
        if (event == ((long)EFG_SIG_SET_RISE))
          event1 = 'U';
        else
          event1 = 'D';

        name = efg_GetSpiSigName  (spisig);

        // NO RC on signal
        if (!efg_IsWireOnSpiSig (spisig) )
          {
            sim_find_by_name( name,
                              sim_model_get_lofig( model ),
                              &locon,
                              &losig,
                              NULL
                              );
            if( locon )
              locate = SIM_IC_LOCON;
            else
              locate = SIM_IC_SIGNAL;

            if (!avoid_ic(spisig) && !sim_ic_get (model,name,locate) )
              sim_ic_set_voltage(model,name, locate,levelv);
            
            if(!multiplepath ) {
              slopelabel = sim_create_label ( name, NULL );
              sprintf ( buf, "%c",event1);
              
              sim_get_multivoltage_values(spisig->CONE, spisig->SRCSIG, &vssl, &vssh, &vddl, &vddh);
              sim_parameter_set_alim(model, vddh, 'o');
              sim_parameter_set_vss(model, vssl, 'o');
              sim_addMeasSlope (model, slopelabel, name, buf, delay_type);
            }
          }
        else 
          {
            locate = SIM_IC_LOCON;
            sim_find_by_name( name,
                              sim_model_get_lofig( model ),
                              &locon,
                              &losig,
                              NULL
                              );
            if( locon )
              locate = SIM_IC_LOCON;
            else
              locate = SIM_IC_SIGNAL;
            // rajoute les et slope des delay rc
            
            sim_get_multivoltage_values(spisig->CONE, spisig->SRCSIG, &vssl, &vssh, &vddl, &vddh);
            sim_parameter_set_alim(model, vddh, 'o');
            sim_parameter_set_vss(model, vssl, 'o');
            sim_parameter_set_alim(model, vddh, 'i');
            sim_parameter_set_vss(model, vssl, 'i');
            
            sprintf ( transition , "%c%c",event1,event1);
            if (spisig == path || spisig->DESTSIG->TYPE == 'E') 
              {
                if (!avoid_ic(spisig) && !sim_ic_get (model,name,locate) )
                  sim_ic_set_voltage(model,name, locate,levelv);
              }
            if ( spisig == path ) 
              {
                if( !multiplepath) 
                  {
                    nrc1 = efg_GetSpiSigLoconRc (spisig,'I');
                    slopelabel = sim_create_label ( nrc1, NULL );
                    
                    sim_addMeasSlope(model, 
                                     slopelabel,
                                     nrc1, &event1, delay_type);
                    sim_add_rc_label_slope ( spisig , slopelabel);
                    nrc2 = name;
                    delaylabel = sim_create_label ( nrc1, nrc2 );
                    slopelabel = sim_create_label ( nrc2, NULL );
                    sim_add_rc_label_delay ( spisig , delaylabel);
                    sim_add_rc_label_slope ( spisig , slopelabel);
                    sim_addMeas (model, 
                                 delaylabel,
                                 slopelabel,
                                 nrc1, nrc2,
                                 transition,
                                 delay_type
                                 );
                  }
              }
            else 
              {           
                nrc1 = efg_GetSpiSigLoconRc (spisig,'I') ;
                if ( nrc1 ) 
                  {
                    if (!avoid_ic(spisig) && !sim_ic_get (model,nrc1,locate) )
                      sim_ic_set_voltage(model,name, locate,levelv);
                    if( !multiplepath) 
                      {
                        slopelabel = sim_create_label ( nrc1,NULL );
                        sim_addMeasSlope(model, 
                                         slopelabel,
                                         nrc1, &event1, delay_type);
                        sim_add_rc_label_slope ( spisig , slopelabel);
                      }
                  }
                else 
                  {
                    nrc1 = efg_GetSpiSigName (spisig);
                  }

                if ((nrc2 = efg_GetSpiSigLoconRc (spisig,'O'))) 
                  {
                    if (!avoid_ic(spisig) && !sim_ic_get (model,nrc2,locate) )
                      sim_ic_set_voltage(model,name, locate,levelv);
                    if( !multiplepath) 
                      {
                        delaylabel = sim_create_label ( nrc1, nrc2 );
                        slopelabel = sim_create_label ( nrc2, NULL );
                        sim_add_rc_label_slope ( spisig , slopelabel);
                        sim_add_rc_label_delay ( spisig , delaylabel);
                        sim_addMeas (model, 
                                     delaylabel,
                                     NULL,
                                     nrc1, nrc2,
                                     transition,
                                     delay_type
                                     );
                        sim_addMeasSlope(model, 
                                         slopelabel,
                                         nrc2, &event1,delay_type);
                      }
                  }
              }
          }

        // GATE DELAY
        if ( spisig->NEXT && !multiplepath) {
          event = efg_GetSpiSigEvent (spisig->NEXT);
          if (event == ((long)EFG_SIG_SET_RISE))
            event2 = 'U';
          else 
            event2 = 'D';
          name2 = efg_GetSpiSigLoconRc (spisig->NEXT,'O');
          if ( !name2 )
            name2 = efg_GetSpiSigName  ( spisig->NEXT );
          name = efg_GetSpiSigLoconRc (spisig,'I');
          if ( !name )
            name = efg_GetSpiSigName  (spisig);

          delaylabel = sim_create_label ( name2, name );

          sim_get_multivoltage_values(spisig->CONE, spisig->SRCSIG, &vssl, &vssh, &vddl, &vddh);
          sim_parameter_set_alim(model, vddh, 'o');
          sim_parameter_set_vss(model, vssl, 'o');
          sim_get_multivoltage_values(spisig->NEXT->CONE, spisig->NEXT->SRCSIG, &vssl, &vssh, &vddl, &vddh);
          sim_parameter_set_alim(model, vddh, 'i');
          sim_parameter_set_vss(model, vssl, 'i');

          sprintf (transition,"%c%c",event2,event1);
          sim_addMeas (model,
                       delaylabel,
                       NULL,
                       name2,
                       name, 
                       transition, delay_type);
        }
    }
#endif

    // pour avoir le delai total
    if ( path && !multiplepath ) 
      {
        firstspisig = efg_GetFirstSpiSig(path);
        lastspisig = efg_GetLastSpiSig(path);
        
        sim_get_multivoltage_values(lastspisig->CONE, lastspisig->SRCSIG, &vssl, &vssh, &vddl, &vddh);
        sim_parameter_set_alim(model, vddh, 'o');
        sim_parameter_set_vss(model, vssl, 'o');
        sim_get_multivoltage_values(firstspisig->CONE, firstspisig->SRCSIG, &vssl, &vssh, &vddl, &vddh);
        sim_parameter_set_alim(model, vddh, 'i');
        sim_parameter_set_vss(model, vssl, 'i');    
        
        name = efg_GetSpiSigName (firstspisig);
        name2 = efg_GetSpiSigName (lastspisig);
        if (efg_GetSpiSigEvent(firstspisig) == ((long)EFG_SIG_SET_RISE)) 
          event1 = 'U';
        else
          event1 = 'D';
        if (efg_GetSpiSigEvent(lastspisig) == ((long)EFG_SIG_SET_RISE)) 
          event2 = 'U';
        else
          event2 = 'D';
        sprintf (transition,"%c%c",event1,event2);
        totaldelaylabel = sim_create_label (name,name2);
        sim_addMeasDelay (model,totaldelaylabel,name,name2,transition,delay_type);
      }
}

//-----------------------------------------------------------------------------
// Func sim_apply_alims

void sim_apply_alims (sim_model *model, lofig_list *fig)
{
  losig_list *losig;
  float       alim;
  char       *signame;
  char       *origname;

  for ( losig = fig->LOSIG ; losig ; losig = losig->NEXT ) {
    signame = getsigname(losig);
    if (!strcmp(signame,"0")) continue;
    origname = efg_get_origsigname (losig);
    if ( !origname ) {
      if ( getlosigalim (losig,&alim) )
        sim_input_set_stuck_voltage ( model, signame, alim);
      else if ( mbk_LosigIsVSS(losig) && losig->TYPE != EXTERNAL )
        sim_input_set_stuck_voltage ( model, signame, 0.0);
      else if ( mbk_LosigIsVDD(losig) && losig->TYPE != EXTERNAL )
        sim_input_set_stuck_voltage ( model, signame, V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE);
    }
    else {
      if ( getlosigalim (losig,&alim) )
        sim_input_set_stuck_voltage ( model, signame, alim);
      else if ( mbk_LosigIsVSS(losig) && losig->TYPE != EXTERNAL )
        sim_input_set_stuck_voltage ( model, signame, 0.0);
      else if ( mbk_LosigIsVDD(losig) && losig->TYPE != EXTERNAL )
        sim_input_set_stuck_voltage ( model, signame, V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE);
      else if ( mbk_LosigIsVSS(losig) )
        sim_input_set_stuck_voltage ( model, signame, 0.0);
      else if ( mbk_LosigIsVDD(losig) )
        sim_input_set_stuck_voltage ( model, signame, V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE);
    }
  }
}

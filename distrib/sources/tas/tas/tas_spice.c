/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TAS Version 5                                               */
/*    Fichier : tas_spice.c                                                 */
/*                                                                          */
/*    (c) copyright 1991-2002 Avertec                                       */
/*    Tous droits reserves                                                  */
/*    Support : contact@avertec.com                                         */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                Marc  KUOCH                                               */
/*                                                                          */
/****************************************************************************/
/* simulation spice d'un chemin                                             */
/****************************************************************************/
#include <pthread.h>
#include <semaphore.h>

#include "tas.h"
#include GEN_H

void (*TAS_SIMU_PROGRESS)(void*, char*, int) = NULL;

/*****************************************************************************\
tas_simu_set_progression 
\*****************************************************************************/
void tas_simu_set_progression ( void (*callfn)( void*, char*, int ) )
{
  TAS_SIMU_PROGRESS = callfn;
}


/*****************************************************************************\
tas_free_old_sim_input  
\*****************************************************************************/
void tas_free_old_sim_input (tas_old_input *oldinput)
{
  if (oldinput)
    mbkfree (oldinput);
}

/*****************************************************************************\
tas_set_old_sim_input  
\*****************************************************************************/
tas_old_input *tas_set_old_sim_input (char *name, double oldtstart,double criticdelay)
{
  tas_old_input *oldinput;

  oldinput = (tas_old_input*)mbkalloc(sizeof(tas_old_input));
  oldinput->NAME   = name;
  oldinput->TSTART = oldtstart;
  oldinput->CRITICDELAY = criticdelay;

  return oldinput;
}

/*****************************************************************************\
tas_get_commut_time

Renvoie un delai et slope apres avoir lancer une simulation

\*****************************************************************************/
double tas_get_commut_time (sim_model *model,char *nodename, int run)
{
  double tc = 0.0;

  if (!model) return 0.0;
  sim_run_simu ( model, run, NULL, NULL );
  sim_parse_spiceout (model);
  tc = sim_get_commut_instant (model,
                               nodename, 
                               'X',
                               V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE);
  return tc;
}

/*****************************************************************************\
tas_free_spisig_marks
\*****************************************************************************/
void tas_free_spisig_marks ( spisig_list *head)
{
  spisig_list *spisig;

  for (spisig = head ; spisig ; spisig=spisig->NEXT) {
    if (getptype (spisig->USER,TAS_CALC_RC_ON_SPISIG) != NULL) 
      spisig->USER = delptype (spisig->USER,TAS_CALC_RC_ON_SPISIG);
    sim_free_label (spisig);
  }
}

/*****************************************************************************\
Retourne 1 s'il faut rechercher le delay RC, 0 sinon
\*****************************************************************************/
int tas_get_rc_delay ( spisig_list *spisig)
{
  int res = 0;

  if (getptype (spisig->USER,TAS_CALC_RC_ON_SPISIG) != NULL) 
    res = 1;

  return res;
}

/*****************************************************************************/
/* tas_flag_rc_delay2spisig                                                  */
/*****************************************************************************/
void tas_flag_rc_delay2spisig ( spisig_list *head_spisig,
                                ttvcritic_list *head_critic)
{
  ttvcritic_list *c;
  spisig_list    *spisig;
  char *devectname;

  for (c = head_critic ; c ; c=c->NEXT) {
    if ((c->TYPE & TTV_FIND_RC) == TTV_FIND_RC) {
      devectname = mbk_devect (c->NETNAME,"[","]");
      if ((spisig = efg_GetSpiSig (head_spisig,devectname)) != NULL) {
        spisig->USER = addptype (spisig->USER,TAS_CALC_RC_ON_SPISIG,NULL);
      }
    }
  }
}


/*****************************************************************************\
FUNCTION : tas_get_sim_model_from_fct

Retourne le model sim equivalent a une timing_function

\*****************************************************************************/
void tas_get_sim_model_from_fct (timing_function *fct,
                                 sim_model  **model,
                                 char       **insname,
                                 chain_list **gen_loins,
                                 chain_list **gen_lotrs,
                                 ptype_list **gen_losig)
{
  subinst_t *sins;
  ptype_list *ptype;

  sins = gen_get_hier_instance (LATEST_GNS_RUN, fct->INS);
  if (!sins) {
    fprintf (stderr, "could not retreive instance %s\n", fct->INS);
    return ;
  }
  if (insname) *insname = sins->INSNAME;

  in_genius_context_of (LATEST_GNS_RUN, sins);
  stm_set_current_arc(fct);

  stm_call_simulation_env();

  if ((*model = sim_get_ctx()))
    gen_grab_all_real_corresp((*model)->FIG, gen_lotrs, gen_losig, gen_loins);

  for (ptype = *gen_losig ; ptype ; ptype=ptype->NEXT) {
    losig_list *sig = (losig_list*)ptype->DATA;

    if ( efg_SigIsAlim (sig) == 'N' &&
         !getptype (sig->USER,TAS_SIG_LOINSNAME))
      sig->USER = addptype (sig->USER,TAS_SIG_LOINSNAME,sins->INSNAME);
  }

  stm_set_current_arc(NULL);
  out_genius_context_of ();
}

/*****************************************************************************\
FUNCTION : tas_get_sim_mod_by_line

Retourne le model sim equivalent a une line.

\*****************************************************************************/
sim_model *tas_get_sim_model_by_line (ttvline_list *line,
                                      char       **insname,
                                      chain_list **gen_loins,
                                      chain_list **gen_lotrs,
                                      ptype_list **gen_losig)
{
  sim_model *model = NULL;
  char *stm_modelname;
  timing_model *tmg_model;
  timing_function *fct = NULL;

  if (!line) return NULL;
  stm_modelname = line->MDMAX;
  if ((tmg_model = stm_getmodel (line->FIG->INSNAME,stm_modelname))) {
    if (tmg_model->UTYPE == STM_MOD_MODFCT) {
      fct = tmg_model->UMODEL.FUNCTION;
      tas_get_sim_model_from_fct (fct,
                                  &model,
                                  insname,
                                  gen_loins,
                                  gen_lotrs,
                                  gen_losig);
    }
  }
  return model;
}

/*****************************************************************************\
FUNCTION : tas_get_line_from_critic

\*****************************************************************************/
ttvline_list *tas_get_line_from_critic (ttvcritic_list *critic)
{
  ttvcritic_list *nextcritic;
  ttvevent_list  *rootevent;
  ttvevent_list  *destevent;
  ttvsig_list    *rootsig;
  ttvsig_list    *destsig;
  ttvline_list *line = NULL;

  if (!critic)
    return NULL;
    if ((critic->NEXT))
      nextcritic = critic->NEXT;
    else
      return NULL;

  if (!(rootsig = ttv_getsig(critic->NEXT->FIG,critic->NAME)))
    return NULL;

  if (critic->SNODE == TTV_UP) 
    rootevent = &rootsig->NODE[1];
  else
    rootevent = &rootsig->NODE[0];

  if (!(destsig = ttv_getsig(critic->NEXT->FIG,critic->NEXT->NAME)))
    return NULL;

  if (nextcritic->SNODE == TTV_UP) 
    destevent = &destsig->NODE[1];
  else
    destevent = &destsig->NODE[0];
  for (line = destevent->INLINE ; line ; line=line->NEXT) {
    // attention a la convention des noms
    if (line->ROOT == destevent && line->NODE == rootevent)
      break;
  }

  return line;
}


/*****************************************************************************\
FUNCTION : tas_get_line_from_critic

\*****************************************************************************/
float tas_get_vth_from_critic (ttvcritic_list *critic)
{
  ttvline_list *line;
  timing_model *tmg_model;
  ttvcritic_list *c;
  char *stm_modelname;
  float vth = -1.0;

  for (c = critic ; c ; c=c->NEXT) {
      line = tas_get_line_from_critic ( c );
      if ( line ) {
        stm_modelname = line->MDMAX;
        if ((tmg_model = stm_getmodel (line->FIG->INSNAME,stm_modelname))) {
          vth = stm_mod_vt (tmg_model);
          break;
        }
      }
  }
  return vth;
}

/*****************************************************************************\
FUNCTION : tas_get_sim_mod_model

Retourne le model sim equivalent a une ttvcritic_list .
Ce model est mis a jour au niveau des differents noms des signaux / connecteur

\*****************************************************************************/
sim_model *tas_get_sim_model (ttvcritic_list *critic,
                              char       **insname,
                              chain_list **gen_loins,
                              chain_list **gen_lotrs,
                              ptype_list **gen_losig
                              )
{
  sim_model      *model = NULL;
  ttvline_list   *line;

  if (!critic)
    return NULL;

  line = tas_get_line_from_critic (critic);

  model = tas_get_sim_model_by_line (line,insname,gen_loins,gen_lotrs,gen_losig);
  return model;
}

/*****************************************************************************\
FUNCTION : tas_get_sim_mod

Retourne le model sim equivalent a une critic list.
Ce model est mis a jour au niveau des differents noms des signaux / connecteur

\*****************************************************************************/
sim_model *tas_get_sim_mod (lofig_list *lofig,
                            ttvcritic_list *critic,
                            loins_list **origloins,
                            chain_list **gen_loins,
                            chain_list **gen_lotrs,
                            ptype_list **gen_losig)
{
  sim_model  *gns_sim_model = NULL;
  lofig_list *figins;
  loins_list *loins;
  locon_list *loconins,*loconfig;
  char       *insname;
  double      input_slope,tstart;
  tas_old_input *oldinput;
  ptype_list *shift;
  int         nbinput=0;
  
  // mise a jour des fronts et capa out
  stm_set_output_load (0.0);
  if ((input_slope = TAS_CONTEXT->FRONT_CON) < 0.0)
      input_slope = SIM_SLOP;
  /* to comment when ready
  input_slope = critic->SLOPE/TTV_UNIT*1.0e-12;
  SIM_SLOP = input_slope;
  */
  stm_set_input_slope (input_slope);
  if ((gns_sim_model = tas_get_sim_model (critic,&insname,gen_loins,gen_lotrs,gen_losig))) {
    loins = efg_get_hier_loins (lofig,insname);
    if ((loins)) {
      // => mark instance 
      efg_dont_copy_ins (loins);
      if (origloins)
        *origloins = loins;
      figins = gns_sim_model->FIG;

      // on rattache cette figure utilisateur a l'instance
      efg_set_fig2ins (loins,figins);

      // tas_update_sim_model
      for (loconins = loins->LOCON ; loconins ; loconins=loconins->NEXT) {
        char *conname,*ctxt,*signame;

        rightunconcatname (loconins->NAME,&ctxt,&conname);
        if (!conname)
          conname = loconins->NAME;

        loconfig = efg_get_locon (figins,conname);
        if ( loconfig ) {
          // si le signal est sur le chemin, on en supprime eventuellement
          // le front de la simulation car il provient du yagcore
          signame = getsigname (loconins->SIG);
          if ((signame  == mbk_devect (critic->NETNAME,"[","]")) ||
             (signame  == namealloc (critic->NETNAME))) {
            // recupere la valeur du start time on this locon
            if ( (tstart = sim_input_get_tstart (gns_sim_model,loconfig->NAME)) > 0.0 ) {
              if (!getptype (gns_sim_model->USER,TAS_OLD_INPUT_START)) {
                oldinput = tas_set_old_sim_input (concatname (loins->INSNAME,loconfig->NAME),
                                                  tstart,critic->DELAY);
                gns_sim_model->USER = addptype (gns_sim_model->USER,
                                                TAS_OLD_INPUT_START,
                                                oldinput);
              }
              sim_input_clear( gns_sim_model, loconfig->NAME);
              nbinput++;
            }
          }

          sim_update_model_locon_name (gns_sim_model,
                                       loconfig->NAME,
                                       concatname (loins->INSNAME,
                                                   loconfig->NAME)
                                      );
        }
      }
      if ( nbinput == 1 ) { // if there is only one PWL for the simulation, don t need
                            // to update start time in the future : this input is only deleted
        shift = getptype (gns_sim_model->USER,TAS_OLD_INPUT_START);
        if ( shift ) {
          oldinput = (tas_old_input*)shift->DATA;
          tas_free_old_sim_input (oldinput);
          gns_sim_model->USER = delptype (gns_sim_model->USER,
                                          TAS_OLD_INPUT_START);
        }
      }

      sim_update_model_signal_name (gns_sim_model,loins->INSNAME);
    }
    //sim_shift_input (gns_sim_model, 100e-12);
  }

  return gns_sim_model;
}

/*****************************************************************************\
FUNCTION : tas_set_con_sig_name

\*****************************************************************************/
void tas_set_con_sig_name (losig_list *sig, char *name)
{
  ptype_list *lofigchain;
  chain_list *conect;
  locon_list *locon;

  if (!sig || !name) return;

  sig->NAMECHAIN->DATA = name;
  lofigchain = getptype (sig->USER,LOFIGCHAIN);
  for (conect = (chain_list*)lofigchain->DATA ; conect ; conect=conect->NEXT) {
    locon = (locon_list*)conect->DATA;
    if ( locon->TYPE != 'T')
      locon->NAME = name;
  }
}

/*****************************************************************************/
/* tas_get_delay_slope                                                     */
/*****************************************************************************/
void tas_get_delay_slope ( char *filename,
                           sim_model *model,
                           spisig_list *path,
                           ttvpath_list *pth,
                           char dstype,
                           double input_slope)
{

  spisig_list *spisig,*firstspisig,*lastspisig;
  double delay, slope;
  ttvcritic_list *c;
  char *cname;
  ttvcritic_list *critic;
  ttvcriticmc *mcdata ;
  int i;

  critic=pth->CRITIC;
  path = (spisig_list*)reverse ((chain_list*)path);
  firstspisig = efg_GetFirstSpiSig (path);
  lastspisig = efg_GetLastSpiSig (path);

  critic->SIMSLOPE = mbk_long_round(input_slope*1e12*TTV_UNIT);
  critic->SIMDELAY = critic->DELAY;

  c=critic->NEXT, spisig=firstspisig;
  while (spisig!=lastspisig->NEXT && c!=NULL)
    {
      delay=slope=-1;
      if (getptype(spisig->USER, EFG_SPISIG_LOOP_SPISIG)!=NULL)
        {
          sim_getMeasDelay(model, namealloc(SIM_LOOP_FEEDBACK_DELAY_LABEL), &delay);
          pth->USER=addptype(pth->USER, SIM_LOOP_FEEDBACK_DELAY_PTYPE, (void *)mbk_long_round(delay*1e12*TTV_UNIT));
        }

      if (spisig->LATCHCMDDIR & EFG_SPISIG_DELAY_TO_HZ_MASK)
        {
          sim_getMeasDelay(model, namealloc(SIM_DELAY_TO_VT_LABEL), &delay);
          pth->USER=addptype(pth->USER, SIM_DELAY_TO_VT_PTYPE, (void *)mbk_long_round(delay*1e12*TTV_UNIT));
        }

      cname=c->NETNAME;
      if (!strcmp(cname,spisig->critic_NAME))
        {
          if( model->NBMC ) {

              mcdata = ttv_alloccriticmc( model->NBMC ) ;
              ttv_setassociatedcriticmc( c, mcdata );
              
              if ((c->TYPE & TTV_FIND_RC)!=0)
                {
                  for( i=0 ; i<model->NBMC ; i++ ) {
                    sim_getMcMeas(model, sim_get_label_delay(spisig, SIM_RC_DELAY_LABEL), &delay, i );
                    if( delay != -1 )
                      mcdata->DELAY[i] = mbk_long_round( delay*1e12*TTV_UNIT);
                    else
                      mcdata->DELAY[i] = TTV_NOTIME ;
                    sim_getMcMeas(model, sim_get_label_delay(spisig, SIM_RC_DELAY_LABEL), &slope, i );
                    if( slope != -1 ) 
                      mcdata->SLOPE[i] = mbk_long_round( slope*1e12*TTV_UNIT);
                    else
                      mcdata->SLOPE[i] = TTV_NOTIME ;
                  }
                }
              else
                {
                  for( i=0 ; i<model->NBMC ; i++ ) {
                    sim_getMcMeas(model, sim_get_label_delay(spisig, SIM_DELAY_LABEL), &delay, i );
                    if( delay!=-1)
                      mcdata->DELAY[i] = mbk_long_round(delay*1e12*TTV_UNIT);
                    else
                      mcdata->DELAY[i] = TTV_NOTIME ;
                  }
                  
                  if((spisig->LATCHCMDDIR & EFG_SPISIG_HZ_MASK)!=0) {
                    for( i=0 ; i<model->NBMC ; i++ ) 
                      mcdata->SLOPE[i] = TTV_NOTIME ;
                  }
                  else {
                    for( i=0 ; i<model->NBMC ; i++ ) {
                      sim_getMcMeas(model, sim_get_label_delay(spisig, SIM_SLOPE_LABEL), &slope, i );
                      if( slope!=-1)
                        mcdata->SLOPE[i] = mbk_long_round(slope*1e12*TTV_UNIT);
                      else
                        mcdata->SLOPE[i] = TTV_NOTIME ;
                    }
                  }
                }
          }
          else {
              if ((c->TYPE & TTV_FIND_RC)!=0)
                {
                  sim_getMeasDelay(model, sim_get_label_delay(spisig, SIM_RC_DELAY_LABEL), &delay);
                  sim_getMeasSlope(model, sim_get_label_delay(spisig, SIM_RC_SLOPE_LABEL), &slope);
                  if (delay!=-1)
                    c->SIMDELAY = mbk_long_round(delay*1e12*TTV_UNIT);
                  else
                    c->SIMDELAY = TTV_NOTIME;
                  if (slope!=-1)
                    c->SIMSLOPE = mbk_long_round(slope*1e12*TTV_UNIT);
                  else
                    c->SIMSLOPE = TTV_NOTIME;
                }
              else
                {
                  sim_getMeasDelay(model, sim_get_label_delay(spisig, SIM_DELAY_LABEL), &delay);
                  if (delay!=-1)
                    c->SIMDELAY = mbk_long_round(delay*1e12*TTV_UNIT);
                  else
                    c->SIMDELAY = TTV_NOTIME;
                  if ((spisig->LATCHCMDDIR & EFG_SPISIG_HZ_MASK)!=0)
                    slope=0;
                  else
                    sim_getMeasSlope(model, sim_get_label_delay(spisig, SIM_SLOPE_LABEL), &slope);
                  if (slope!=-1)
                    c->SIMSLOPE = mbk_long_round(slope*1e12*TTV_UNIT);
                  else
                    c->SIMSLOPE = TTV_NOTIME;
                }
          }
          c=c->NEXT;
        }
      else
        spisig=spisig->NEXT;
    }
  path = (spisig_list*)reverse ((chain_list*)path);
        
#if 0

  ttvcritic_list *c = NULL;
  char buf[1024];
  FILE *file = NULL;
  double delayrc,delay,delaytotal = 0.0;
  double nextdelayrc;
  double slope;
  char *node1,*node2;
  char *spisigname1,*spisigname2;
  char event1,event2,type1,type2;
  spisig_list *spisig,*firstspisig,*lastspisig;
  char *nodercin,*nodercout;
  int valid; // valid = 1 <=> calcul valid dans reso RC
  int write_file = 0;
  int read_meas = 0;
  double simvthh = SIM_VTH_HIGH;
  double simvthl = SIM_VTH_LOW;

  if (!filename || !model || !path)
    return;

  // Put the right SFHL and SFHH
  
//  if ( SIM_USE_MEAS == SIM_YES )
    read_meas = 1;
  if ( !critic ) {
    write_file = 1;
    sprintf (buf,"%s_res",filename);
    file = mbkfopen(buf,"dat",WRITE_TEXT) ;
  }

  if ( !write_file || file != NULL) {
      path = (spisig_list*)reverse ((chain_list*)path);
      firstspisig = efg_GetFirstSpiSig (path);
      lastspisig = efg_GetLastSpiSig (path);
      if (critic != NULL) {
          (*critic)->SIMSLOPE = mbk_long_round(input_slope*1e12*TTV_UNIT);
//          (*critic)->SIMSLOPE = (*critic)->SLOPE;
          (*critic)->SIMDELAY = (*critic)->DELAY; // starttime //(long)0;
          c = *critic;
          tas_flag_rc_delay2spisig ( path,*critic);
      }
      if ( write_file )
        fprintf (file,"  Tp        Total     Slope       Signal\n\n");
      for (spisig = path ; spisig; spisig=spisig->NEXT) {
        if (efg_GetSpiSigNum(spisig)==-1) continue;
        delayrc = 0.0;
        nextdelayrc = 0.0;
        nodercin = nodercout = NULL;

        // node name
        node1 = efg_GetSpiSigName (spisig);
        spisigname1 = sim_devect (node1);
        if (spisig->NEXT != NULL) {
          node2 = efg_GetSpiSigName (spisig->NEXT);
          spisigname2 = sim_devect (node2);
          type2 = sim_get_type_node (model,node2);
        }
        // node type
        if (efg_IsWireOnSpiSig (spisig) == 1 &&
            spisig != firstspisig && 
            spisig != lastspisig ) {
            node1 = efg_GetSpiSigLoconRc(spisig,'I');
            if (!node1)
              node1 = spisigname1;
            type1 = 'C';
        }
        else 
          type1 = sim_get_type_node (model,node1);
        if (spisig->NEXT != NULL) {
          if (efg_IsWireOnSpiSig (spisig->NEXT) == 1 &&
            spisig->NEXT != firstspisig && 
            spisig->NEXT != lastspisig ) {
            type2 = 'C';
            node2 = efg_GetSpiSigLoconRc(spisig->NEXT,'O');
            if (!node2)
              node2 = spisigname2;
          }
          else
            type2 = sim_get_type_node (model,node2);
        }
        // node event
        if (efg_GetSpiSigEvent (spisig) == (long)EFG_SIG_SET_RISE)
            event1 = 'U';
        else
            event1 = 'D';
        if (spisig->NEXT != NULL) {
          if (efg_GetSpiSigEvent (spisig->NEXT) == (long)EFG_SIG_SET_RISE)
              event2 = 'U';
          else
              event2 = 'D';
          sprintf (buf,"%c%c",event1,event2);
        }

        if (efg_IsWireOnSpiSig (spisig) == 1 &&
            (!critic || (tas_get_rc_delay (spisig) == 1))) {
          // le signal a des reso rc
          delay = 0.0;
          slope = 0.0;

          valid = sim_get_rc_delay_slope_meas (model,
                                                spisig,
                                               &delayrc, &slope,
                                                dstype);
          if (!valid) {
            delayrc = 0.0;
            slope = 0.0;
          }
          //delayrc = fabs(delayrc);
          if (!nodercin) nodercin = "X";
          if (!nodercout) nodercout = "X";
          delaytotal += delayrc;
          if (spisig == firstspisig) {
            if ( write_file ) {
              fprintf (file,"    %6ld pS ",(long)0);
              fprintf (file,"%6ld pS ",(long)0);
              fprintf (file,"%6ld pS ",(long)0);
              fprintf (file,"%c ",event1);
              fprintf (file,"%s\n",nodercin);
            }
          }
          if ( write_file ) {
            fprintf (file," RC %6ld pS ",(long)(delayrc*1.0e12));
            fprintf (file,"%6ld pS ",(long)(delaytotal*1.0e12));
            fprintf (file,"%6ld pS ",(long)(slope*1.0e12));
            fprintf (file,"%c ",event1);
            fprintf (file,"%s\n",nodercout);
          }
          if (c != NULL && c->NEXT != NULL) {
              c->NEXT->SIMDELAY = mbk_long_round(delayrc*1e12*TTV_UNIT);
              c->NEXT->SIMSLOPE = mbk_long_round(slope*1e12*TTV_UNIT);
              c = c->NEXT;
          }
        }
        // GATE DELAY
        if (efg_IsWireOnSpiSig (spisig) == 1 ) {
          if ( spisig == firstspisig )
            node1 = efg_GetSpiSigLoconRc(spisig,'O');
          else if ( spisig == lastspisig )
            node1 = efg_GetSpiSigLoconRc(spisig,'I');
          else
            node1 = efg_GetSpiSigLoconRc(spisig,'O');
        }
        else 
          node1 = spisigname1;
        if (efg_IsWireOnSpiSig (spisig->NEXT) == 1 ) 
          node2 = efg_GetSpiSigLoconRc(spisig->NEXT,'I');
        else 
          node2 = spisigname2;
        if (spisig->NEXT && read_meas ) {
          valid = sim_getMeasDelay (model, 
                                    sim_create_label (node1,node2),
                                    &delay
                                   );
          if ( valid )
            delaytotal += delay;
        }
/*        if ((long)(delay*1.0e12) == 0)
            slope = 0.0;
        else {*/
          slope = 0.0;

            sim_getMeasSlope (model, 
                              sim_create_label (node2,NULL),
                              &slope
                             );
//        }
        if (spisig == firstspisig && nodercin == NULL) {
          if ( write_file ) {
            fprintf (file,"    %6ld pS ",(long)0);
            fprintf (file,"%6ld pS ",(long)0);
            fprintf (file,"%6ld pS ",(long)0);
            fprintf (file,"%c ",event1);
            fprintf (file,"%s\n",node1);
          }
        }
        if ( write_file ) {
          fprintf (file," GT %6ld pS ",(long)(delay*1.0e12));
          fprintf (file,"%6ld pS ",(long)(delaytotal*1.0e12));
          fprintf (file,"%6ld pS ",(long)(slope*1.0e12));
          fprintf (file,"%c ",event2);
          fprintf (file,"%s\n",node2);
        }
        if (c != NULL && c->NEXT != NULL) {
            c->NEXT->SIMDELAY = mbk_long_round(delay*1e12*TTV_UNIT);
            c->NEXT->SIMSLOPE = mbk_long_round(slope*1e12*TTV_UNIT);
            c = c->NEXT;
        }
      }
    sim_timing_free_all ();
    path = (spisig_list*)reverse ((chain_list*)path);

    if ( write_file && fclose (file) != 0) {
       sprintf (buf,"%s_res.dat",filename);
       tas_error(10,buf,TAS_WARNING) ;
    }
  }
  else if ( write_file && !file) {
    sprintf (buf,"%s_res.dat",filename);
    tas_error(8,buf,TAS_WARNING) ;
  }
  SIM_VTH_HIGH = simvthh;
  SIM_VTH_LOW =  simvthl;
#endif
}

/*****************************************************************************\
                          function tas_get_input_slope ()                         
\*****************************************************************************/
double tas_get_input_slope (ttvcritic_list *critic)
{
  double slope = SIM_SLOP;

  if ( critic )
    slope = critic->SLOPE/TTV_UNIT;

  return slope;
}

/*****************************************************************************\
                          function tas_simu_netlist ()                         
   drive le spice des signaux du .inf                                      
   Retourne 1 si la simulation et la lecture des resultats sont corrects
\*****************************************************************************/

int float_equal (double a, double b) 
{
    return (mbk_long_round(a * 1000) == mbk_long_round(b * 1000));
}


static spisig_list *findspisigincritic(spisig_list *head, char *name)
{
  while (head!=NULL)
  {
    if (head->critic_NAME!=NULL && strcmp(head->critic_NAME, name)==0) return head;
    head=head->NEXT;
  }
  return NULL;
}

typedef struct 
{
  int id;
  sim_model *model;
  char filename[1024];
  mbk_sem_t *simDispo;
} sim_parm;

mbk_sem_t simDispo ;

static void *sim_simu_thread(void *p)
{
  sim_parm *sp=(sim_parm *)p;
  sim_run_simu ( sp->model, SIM_RUN_EXEC|SIM_RUN_MULTI, sp->filename, stderr);
  if (V_BOOL_TAB[__SIM_ALLOW_THREADS].VALUE)
  {
    mbk_sem_post(sp->simDispo);
    pthread_exit(NULL);
  }
  return NULL;
}

static double tas_getvt(ttvfig_list *tvf, char *model)
{
  timing_model *tmg_model;
  if ((tmg_model = stm_getmodel (tvf->INFO->FIGNAME,model))!=NULL)
    return stm_mod_vt (tmg_model);
  return -1;
}

long tas_estim_path_delay( ttvpath_list *path, float slope /* pS */, float capa /* fF */)
{
  ttvcritic_list *critic ;
  long f, t ;
  long totald = 0 ;
  float addcapa ;
  float gated, gatef ;
  
  f = (long)(slope*TTV_UNIT*(2-(SIM_VTH_HIGH-SIM_VTH_LOW)));
  totald = f ;
  
  for( critic = path->CRITIC->NEXT ; critic ; critic = critic->NEXT ) {
  
    if( ( critic->TYPE & TTV_FIND_RC ) == TTV_FIND_RC ) {
    
      if( critic->NEXT == NULL )
        addcapa = capa ;
      else
        addcapa = 0.0 ;

      t = ((long)(((float)f)/((float)critic->SLOPE)*(addcapa+critic->CAPA)/critic->CAPA)) * critic->DELAY ;
      f = f+t ;
      
    }
    else {
    
      if( critic->NEXT == NULL ||
          ( ( critic->NEXT->TYPE & TTV_FIND_RC ) == TTV_FIND_RC && critic->NEXT->NEXT == NULL ) )
        addcapa = capa ;
      else
        addcapa = 0.0 ;
      stm_gettiming( path->FIG->INFO->FIGNAME, critic->LINEMODELNAME, critic->LINEMODELNAME, ((float)f)/TTV_UNIT, NULL, NULL, critic->CAPA + addcapa, &gated, &gatef, NULL, NULL, NULL, '?' );
      t = ((long)(gated*TTV_UNIT));
      f = ((long)(gatef*TTV_UNIT));
    }
    totald = totald + t ;
  }
  totald = totald + f*(2-(SIM_VTH_HIGH-SIM_VTH_LOW)) ;

  return totald ;
}

long tas_estim_path_delay_list( chain_list *pathlist, tas_spice_charac_struct *tscs )
{
  float maxslope ;
  float slope ;
  float maxcapa ;
  float capa ;
  long  maxpathdelay ;
  long  pathdelay ;
  ttvpath_list *path ;
  chain_list *chain ;

  if( tscs ) {
    for( chain = tscs->slopes, maxslope=-1.0 ; chain ; chain = chain->NEXT ) {
      slope=*((float *)(&(chain->DATA)));
      if( slope > 0.0 && slope > maxslope )
        maxslope = slope ;
    }
    
    for( chain = tscs->capas, maxcapa=-1.0 ; chain ; chain = chain->NEXT ) {
      capa=*((float *)(&(chain->DATA)));
      if( capa > 0.0 && capa > maxcapa )
        maxcapa = capa ;
    }
  
    maxslope = maxslope * 1e12 ;

    if( maxcapa < 0.0 )
      maxcapa = 0.0 ;
    maxcapa  = maxcapa  * 1e15 ;
  }

  maxpathdelay = -1 ;
  for( chain = pathlist ; chain ; chain = chain->NEXT ) {
    path = (ttvpath_list*)chain->DATA ;
    if( path->DELAY != TTV_NOTIME ) {
      if( tscs )
        pathdelay = tas_estim_path_delay( path, maxslope, maxcapa );
      else
        pathdelay = path->DELAY+path->SLOPE+path->SLOPESTART ;
      if( pathdelay > 0 && pathdelay > maxpathdelay )
        maxpathdelay = pathdelay ;
    }
  }
  
  return maxpathdelay ;
}

int  tas_simu_netlist (ttvfig_list *ttvfig,
                       lofig_list *lofig,
                       cnsfig_list *cnsfig,
                       spisig_list *spisig,
                       chain_list  *criticlist,
                       char dstype, // SIM_MIN or SIM_MAX
                       int run,
                       char *outputfilename,
                       FILE *outputfile,
                       tas_spice_charac_struct *tscs,
                       int mcruns,
                       int plot
                      )
{
  ttvcritic_list *ptcritic = NULL;
  ptype_list *spipatterns,
             *list_simmodel = NULL;
  lofig_list *figext=NULL;
  sim_model *model = NULL,*gns_sim_model = NULL;
  ptype_list *ptype, *siglist = NULL;
  spisig_list *spisignal;
  loins_list *origloins = NULL;
  ttvpath_list *tpath;
  chain_list *chaincone     = NULL, *path = NULL;
  chain_list *chaininstance = NULL, *chain;
  chain_list *gen_loins = NULL;
  chain_list *gen_lotrs = NULL;
  chain_list *gen_locon = NULL;
  chain_list *gen_losig = NULL;
  chain_list *sig2plot = NULL, *name2plot = NULL;
  chain_list *added_capa = NULL;
  double input_slope;
  char *env, *lastsig;
  int res = 0, err;
  ht *htsigname = NULL; // correspondance des noms des sig : orig => hierarchiq
  float vth = -1.0;
  int use_tas_shiftdelay = 1;
  int ko = 0;
  char figextname[1024];
  char old_simoutload = EFG_CALC_EQUI_CAPA;
  spisig_list *firstspisig=NULL;
  char corner, cmddir;
  chain_list *chcritic;
  ttvcritic_list *critic ;
  char multiplepath=0, *hzmodel;
  long maxdelay ;
  float fmaxdelay ;
  float oldtransienttime, delaytovt_VT ;
  chain_list *allcorrel=NULL;


  GSP_FOUND_SOLUTION_NUMBER=0;

  if (tscs!=NULL) tscs->result=NULL;

  if (ttvfig) {
    if (!V_FLOAT_TAB[__SIM_POWER_SUPPLY].SET) 
      V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE = ttvfig->INFO->VDD;
    if (!float_equal (V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE, ttvfig->INFO->VDD))
        fprintf (stderr, "[TAS WARN] SimPowerSupply and database voltage are different\n");

    if (!V_FLOAT_TAB[__SIM_TEMP].SET)
      V_FLOAT_TAB[__SIM_TEMP].VALUE = ttvfig->INFO->TEMP;
    if (!float_equal (V_FLOAT_TAB[__SIM_TEMP].VALUE, ttvfig->INFO->TEMP)) 
        fprintf (stderr, "[TAS WARN] SimTemperature and database temperature are different\n");

    if (V_FLOAT_TAB[__SIM_INPUT_SLOPE].SET) SIM_SLOP = V_FLOAT_TAB[__SIM_INPUT_SLOPE].VALUE * 1.0E12;
    else SIM_SLOP = ttvfig->INFO->SLOPE / TTV_UNIT;
    if (!float_equal (SIM_SLOP, ttvfig->INFO->SLOPE / TTV_UNIT)) 
        fprintf (stderr, "[TAS WARN] SimSlope and database slope are different\n");

    if (V_FLOAT_TAB[__SIM_VTH_HIGH].SET) SIM_VTH_HIGH = V_FLOAT_TAB[__SIM_VTH_HIGH].VALUE;
    else SIM_VTH_HIGH = ttvfig->INFO->STHHIGH;
    if (!float_equal (SIM_VTH_HIGH, ttvfig->INFO->STHHIGH)) 
        fprintf (stderr, "[TAS WARN] SimVthHigh and database high threshold are different\n");

    if (V_FLOAT_TAB[__SIM_VTH_LOW].SET) SIM_VTH_LOW = V_FLOAT_TAB[__SIM_VTH_LOW].VALUE;
    else SIM_VTH_LOW = ttvfig->INFO->STHLOW;
    if (!float_equal (SIM_VTH_LOW, ttvfig->INFO->STHLOW))
        fprintf (stderr, "[TAS WARN] SimVthLow and database low threshold are different\n");
  }
  
  path=NULL;

  if (!lofig || TAS_CONTEXT->TAS_CNS_LOAD == 'Y')
    return res;
  corner = (dstype == SIM_MIN) ? 'b':'w';
  if ( !cnsfig )
   {
    fprintf (stderr,"[TAS ERR] no cns file, can't simulate\n");
    return res;
   }

#ifdef AVERTEC_LICENSE
   if(avt_givetoken("HITAS_LICENSE_SERVER", "cpe")!=AVT_VALID_TOKEN) EXIT(1);
#endif

  if (((env = avt_gethashvar ("TAS_USE_CRITICSHIFT")) != NULL) && (!strcasecmp (env,"no")))
    use_tas_shiftdelay = 0;

  if (!spisig && !criticlist) return res;

  // delete prev figext...
  
  efg_get_fig_name(lofig, figextname);
  if ( (figext = getloadedlofig (figextname)) ) {
    efg_FreeMarksOnExtFig (figext);
    dellofig (figext->NAME);
    figext = NULL;
    efg_FreeMarksOnFig (lofig);
  }

  oldtransienttime = -1.0 ;
  maxdelay = 2 * tas_estim_path_delay_list( criticlist, tscs );
  if( maxdelay > 0 ) {
    fmaxdelay = ((float)maxdelay*1e-12)/TTV_UNIT ;
    if( fmaxdelay > V_FLOAT_TAB[ __SIM_TIME ].VALUE ) {
      oldtransienttime = V_FLOAT_TAB[ __SIM_TIME ].VALUE ;
      V_FLOAT_TAB[ __SIM_TIME ].VALUE = fmaxdelay ;
    }
  }
  
  if ( ELP_LOAD_FILE_TYPE == ELP_DONTLOAD_FILE ) // <=> LoadSpecifiedElpFile = no
    EFG_CALC_EQUI_CAPA = EFG_OUT_TRANSISTOR;
   
  if( !spisig && criticlist ) {

    long innode, outnode;
    ttvcritic_list *prevcritic;

    for( chcritic = criticlist ; chcritic ; chcritic = chcritic->NEXT ) {
    
      if( path ) {
        path = efg_AddPathEvent( path, NULL, 0, 0, -1, -1, -1, -1, NULL, 0, NULL );
        multiplepath=1;
      }
      prevcritic=NULL;
      for ( ptcritic = (tpath=(ttvpath_list*)chcritic->DATA)->CRITIC ; ptcritic ; prevcritic=ptcritic, ptcritic = ptcritic->NEXT) {
          innode=-1, outnode=-1;
          if (prevcritic && (ptcritic->TYPE & TTV_FIND_RC)!=0)
          {
             innode=prevcritic->PNODE;
             outnode=ptcritic->PNODE;
          }

          if (tpath->CMD!=NULL && ptcritic->NEXT==NULL)
          {
             char buf[1024];
             ttv_getnetname(ttvfig,buf,tpath->CMD->ROOT) ;
             lastsig=namealloc(buf);
             if (tpath->CMD->TYPE & TTV_NODE_UP) cmddir=EFG_SPISIG_CMDDIR_MASK; else cmddir=0;
          }
          else
             lastsig=NULL, cmddir=0;

          if ((ptcritic->TYPE & TTV_FIND_HZ)==TTV_FIND_HZ && ptcritic->NEXT==NULL)
            cmddir|=EFG_SPISIG_HZ_MASK, hzmodel=ptcritic->LINEMODELNAME;

          if (ptcritic->NEXT==NULL && (ptype=getptype(tpath->USER, TTV_PATH_END_VT))!=NULL)
            cmddir|=EFG_SPISIG_DELAY_TO_HZ_MASK, delaytovt_VT=*(float *)&ptype->DATA;

          path = efg_AddPathEvent ( path , 
                                    ptcritic->NETNAME ,
                                    ptcritic->SNODE == TTV_UP ? EFG_RISE:EFG_FALL, 0, -1, -1, innode, outnode,lastsig,cmddir,
                                    prevcritic==NULL?NULL:prevcritic->NETNAME
                                    );

        // get sim_model from genius
        if ((gns_sim_model = tas_get_sim_mod(lofig,
                                             ptcritic,
                                             &origloins,
                                             &gen_loins,
                                             &gen_lotrs,
                                             &siglist))) {
          list_simmodel = addptype (list_simmodel,(long)origloins,gns_sim_model);
        }
      }
    }

    path = reverse (path);
    spisig = efg_BuildSpiSigList (lofig,path,NULL,1,&err,0);
    if ((spisignal = efg_GetLastSpiSig(spisig))!=NULL)
      {
        if ((spisignal->LATCHCMDDIR & EFG_SPISIG_HZ_MASK)!=0)
          spisignal->VTHHZ=tas_getvt(ttvfig, hzmodel);
        if ((spisignal->LATCHCMDDIR & EFG_SPISIG_DELAY_TO_HZ_MASK)!=0)
          spisignal->VTHHZ=delaytovt_VT;
      }

    efg_FreePathEvent(path);
    firstspisig = efg_GetFirstSpiSig (spisig);
    spisig = efg_AddLatchLoopFeebackCone(lofig, cnsfig, spisig);

    // update output capacitance
    for( chcritic = criticlist ; chcritic ; chcritic = chcritic->NEXT ) 
     {
       for ( ptcritic = ((ttvpath_list*)chcritic->DATA)->CRITIC ; ptcritic ; ptcritic = ptcritic->NEXT) 
        {
          if (ptcritic->OUTPUT_CAPA>0)
          {
             if (!tscs || ptcritic->NEXT!=NULL)
             {
              if ((spisignal=findspisigincritic(spisig, ptcritic->NETNAME))!=NULL)
                efg_AddExtraCapa(spisignal, ptcritic->OUTPUT_CAPA);
             }
          }
        }
     }
    
    if ( criticlist ) {
        vth = tas_get_vth_from_critic ( ((ttvpath_list*)criticlist->DATA)->CRITIC ) ;
        if ( vth > 0.0 ) 
          firstspisig->VTH = vth;
    }
    for (ptype = list_simmodel ; ptype ; ptype = ptype->NEXT) {
      loins_list *origins = (loins_list*)ptype->TYPE;
      locon_list *loconins;

      for (loconins = origins->LOCON ; loconins ; loconins=loconins->NEXT) {
        if (efg_SigIsAlim (loconins->SIG) == 'N' )
          gen_locon = addchain (gen_locon,loconins);
      }
    }

    if (siglist) {
      htsigname = addht (50);
      for (ptype = siglist ; ptype ; ptype=ptype->NEXT) {
        char *orig_name;
        char *hier_name;
        losig_list *hiersig;
        ptype_list *ptypex;

        hiersig = (losig_list *)ptype->DATA;
        orig_name = getsigname (hiersig);
        hier_name = getsigname ((losig_list *)ptype->TYPE);
        ptypex = getptype (hiersig->USER, TAS_SIG_LOINSNAME);
        if (ptypex != NULL) {
          hier_name = concatname ((char*)ptypex->DATA,hier_name);
          hiersig->USER = delptype (hiersig->USER,TAS_SIG_LOINSNAME);
        }
        addhtitem (htsigname,(void*)orig_name,(long)hier_name);
        gen_losig  = addchain (gen_losig,ptype->DATA);
      }
    }
  }
  // 1st stage : preparing simulation

  // => extract the figure
  if ( TAS_SIMU_PROGRESS )
    TAS_SIMU_PROGRESS ( NULL, "Extracting critical path", 1 );
  efg_extract_fig ( lofig,
                    &figext,
                    cnsfig,
                    spisig,
                    &chaincone,
                    &chaininstance,
                    gen_loins,
                    gen_lotrs,
                    gen_locon,
                    gen_losig,
                    NULL,
                    NULL,
                    1
                  );

  if ( !figext ) {
    avt_errmsg( TAS_ERRMSG, "074", AVT_WARNING );
    //fprintf (stderr,"[TAS ERR] can't extract path to simulate\n");
    ko = 1;
  }

  if ( !ko ) {
    if (tscs==NULL)
      {
        // if an out load capacitance is specified 
        if (SIM_OUT_CAPA_VAL > 0.0) {
          efg_AddExtraCapa(efg_GetLastSpiSig (spisig), SIM_OUT_CAPA_VAL);
        }
        
        
        for (spisignal=spisig; spisignal!=NULL; spisignal=spisignal->NEXT)
          {
            if (spisignal->ADDCAPA>0)
              {
                lastsig = efg_GetSpiSigName (efg_GetLastSpiSig (spisignal));
                added_capa = append(efg_set_extra_capa (figext,lastsig,spisignal->ADDCAPA*1.0e-3), added_capa);
              }
          }
      }
    
    // user update signal & locon name a faire pour tous les signaux
    for (chain = gen_losig ; chain ; chain=chain->NEXT) {
      char *origname;
      long newname;
      long destsig;
      losig_list *srcsig = (losig_list*)chain->DATA;
    
      origname = getsigname (srcsig);
      destsig = (long)efg_get_ht_sig (figext,origname);
      if (destsig != EMPTYHT && destsig != DELETEHT) {
        newname = gethtitem (htsigname,origname);
        if (newname != DELETEHT || newname != EMPTYHT) {
          tas_set_con_sig_name ((losig_list*)destsig,(char*)newname);
          if ( (spisignal = efg_GetSpiSigByName (spisig,origname)) )
            efg_UpdateSpiSigName (spisignal,(char*)newname);
        }
      }
    }
  }
  
  // liberations
  if (gen_loins) freechain (gen_loins);
  if (gen_lotrs) freechain (gen_lotrs);
  if (gen_losig) freechain (gen_losig);
  if (siglist)   freeptype (siglist);
  if (htsigname) delht     (htsigname);

  // => get spice patterns
  if ( !ko ) {
    if ( TAS_SIMU_PROGRESS )
      TAS_SIMU_PROGRESS ( NULL, "Getting patterns for simulation",1 );
    spipatterns =  gsp_get_patterns(lofig,
                                    cnsfig,
                                    figext,
                                    chaincone,
                                    chaininstance,
                                    corner,
                                    spisig,
                                    &allcorrel
                                    );
    
    if ( !spipatterns && !efg_GetFirstSpiSig (spisig)->END) {
      avt_errmsg( TAS_ERRMSG, "075", AVT_WARNING );
      //fprintf (stderr,"[TAS ERR] can't get patterns to simulate the path\n");
      ko = 1;
    }
  }

  if ( !ko ) {
      if ((input_slope = TAS_CONTEXT->FRONT_CON) < 0.0)
          input_slope = SIM_SLOP;
      input_slope *= 1.0e-12;
    
    if (!efg_get_locon (figext,efg_GetSpiSigName(firstspisig)))
      input_slope = -1.0;
    
    model = sim_apply_pat (figext, 
                           SIM_INPUT_START,
                           input_slope,
                           spipatterns,
                           spisig,
                           0,
                           dstype,
                           multiplepath,
                           cnsfig,
                           mcruns,
                           allcorrel
                           );
    
    gsp_freecorrel(allcorrel);
    freeptype(spipatterns);

    for (ptype = list_simmodel ; ptype ; ptype = ptype->NEXT) {
      // ptype->TYPE = nom d instance correspondant au model sim
      ptype_list *shift;
      tas_old_input *oldinput;
      double         tc;
      gns_sim_model = (sim_model*)ptype->DATA;
    
      shift = getptype (gns_sim_model->USER,TAS_OLD_INPUT_START);
      if ( shift ) {
        oldinput = (tas_old_input*)shift->DATA;
        if ( use_tas_shiftdelay )
          // to get shift delay for other input from gns rules, we use tas delay*1.1
          tc = oldinput->CRITICDELAY*1.1 + input_slope;
        else
          tc = tas_get_commut_time (model,oldinput->NAME, run);
        if (tc > 0.0)
          sim_shift_input (gns_sim_model,tc - oldinput->TSTART);
        tas_free_old_sim_input (oldinput);
        gns_sim_model->USER = delptype (gns_sim_model->USER,
                                        TAS_OLD_INPUT_START);
      }
      sim_model_merge (model,gns_sim_model,((loins_list*)ptype->TYPE)->INSNAME);
      sim_free_context (gns_sim_model);
    }
  }

  if ( model ) {  
    if (figext!=NULL) rcn_mergectclofig(figext);
    // launch simu
    if ( TAS_SIMU_PROGRESS )
      TAS_SIMU_PROGRESS ( NULL, "Running simulation",1 );

    if (tscs!=NULL)
    {
      chain_list *sl, *cp, *ch;
      float slope, capa;
      FILE *mf;
      char buf0[256];
      int a,b ;
      chain_list *ac = NULL;
      char *lastsig;
      ttvpath_list *tmp;

      tscs->result=NULL;

      for (sl=tscs->slopes, a=0; sl!=NULL; sl=sl->NEXT, a++)
        {
          slope=*(float *)&sl->DATA;
          ch=NULL;
          for (cp=tscs->capas, b=0; cp!=NULL; cp=cp->NEXT, b++)
            {
              capa=*(float *)&cp->DATA;
              sprintf(buf0,"%s_%dx%d_spicedeck",figextname,a,b);
              if ((mf=fopen(buf0,"w"))!=NULL)
                {
                  model->FIGNAME=figext->NAME;
                  lastsig = efg_GetSpiSigName (efg_GetLastSpiSig (spisig));
                  if (capa>0)
                    ac = efg_set_extra_capa (figext,lastsig,capa*1e12);
                  else
                    ac=NULL;
                  
                  sim_apply_input (model, spisig, 0, slope, dstype);

                  sim_run_simu ( model, SIM_RUN_DRIVE, buf0, mf);
                  
                  if (ac!=NULL)
                    {
                      efg_del_extra_capa ( ac );
                      freechain(ac);
                    }

                  fclose(mf);
                }
            }
        }

      {
        sim_parm *sp;
        pthread_t *threads;
//        pthread_attr_t pthread_custom_attr;
        mbk_sem_t simDispo;
        int i, nb, bufidx;
        char buf[2048];
        char *argv[1024];

        nb=countchain(tscs->slopes)*countchain(tscs->capas);
        sp=mbkalloc(nb*sizeof(sim_parm));
        threads=(pthread_t *)malloc(nb*sizeof(*threads));
//        pthread_attr_init(&pthread_custom_attr);

        mbk_sem_init(&simDispo,tscs->maxthreads);

        sprintf(buf0,"%s_[0-%d]x[0-%d]_spicedeck",figextname,countchain(tscs->slopes)-1,countchain(tscs->capas)-1);
        i = 0;
        bufidx=0;
        while ((argv[i] = sim_getarg (sim_parameter_get_tool_cmd(model), buf0, i, buf, &bufidx)))
          i++;

        avt_fprintf (stdout, "¤+RUN: ");
        for (i=0; argv[i]!=NULL; i++)
          fprintf (stdout, "%s ", argv[i]);
        avt_fprintf (stdout, "¤. ... ");
        fflush(stdout);

        for (sl=tscs->slopes, a=0, i=0; sl!=NULL; sl=sl->NEXT, a++)
          {
            for (cp=tscs->capas, b=0; cp!=NULL; cp=cp->NEXT, b++)
              {
                if (V_BOOL_TAB[__SIM_ALLOW_THREADS].VALUE)
                  mbk_sem_wait(&simDispo);
                sp[i].id=i;
                sp[i].model=model;
                sp[i].simDispo=&simDispo;
                sprintf(sp[i].filename,"%s_%dx%d_spicedeck",figextname,a,b);
                if (V_BOOL_TAB[__SIM_ALLOW_THREADS].VALUE)                        
                  pthread_create(&threads[i], NULL, sim_simu_thread, &sp[i]);
                else
                  sim_simu_thread(&sp[i]);
                i++;
              }
          }
        if (V_BOOL_TAB[__SIM_ALLOW_THREADS].VALUE)
        {
          for (i=0; i<nb; i++)
            pthread_join(threads[i],NULL);
        }
        mbk_sem_destroy(&simDispo);

        avt_fprintf (stdout, "\n");
        fflush(stdout);

        mbkfree(threads);
        mbkfree(sp);

      }
      for (sl=tscs->slopes, a=0; sl!=NULL; sl=sl->NEXT, a++)
        {
          ch=NULL;
          slope=*(float *)&sl->DATA;
          for (cp=tscs->capas, b=0; cp!=NULL; cp=cp->NEXT, b++)
            {
              sprintf(buf0,"%s_%dx%d_spicedeck",figextname,a,b);
              ch=addchain(ch, tmp=ttv_duppath((ttvpath_list*)criticlist->DATA));
              sim_run_simu ( model, SIM_RUN_READ, buf0, stderr);
              sim_parse_spiceout (model);

              tas_get_delay_slope (figext->NAME,
                                   model,
                                   spisig,
                                   tmp,
                                   dstype,
                                   slope);
            }
          tscs->result=addchain(tscs->result, reverse(ch));
        }
      tscs->result=reverse(tscs->result);

      model->FIGNAME=figext->NAME;
    }
    else
      if (sim_run_simu ( model, run?SIM_RUN_ALL:SIM_RUN_DRIVE, outputfilename, outputfile) != SIM_SIMU_COMPLETED) {
        ptcritic=((ttvpath_list*)criticlist->DATA)->CRITIC;
        ptcritic->SIMSLOPE = 0;
        ptcritic->SIMDELAY = 0;

        if ( added_capa ) efg_del_extra_capa ( added_capa );
        if (list_simmodel != NULL) freeptype ( list_simmodel );
        if (spisig != NULL)
          {
            tas_free_spisig_marks (spisig);
            efg_FreeSpiSigList ( spisig );
          }
        if (model != NULL) sim_model_clear (model->FIG->NAME);
        if (figext != NULL)
          {
            efg_FreeMarksOnExtFig (figext);
            dellofig (figext->NAME);
          }
        efg_FreeMarksOnFig (lofig);
        efg_freecontext ();
        
        EFG_CALC_EQUI_CAPA = old_simoutload;
        if( oldtransienttime > 0.0 )
          V_FLOAT_TAB[ __SIM_TIME ].VALUE = oldtransienttime ;
        return 0;
      }

    if (tscs==NULL && run && !criticlist->NEXT)
      {
        // get simu results
        if ( TAS_SIMU_PROGRESS )
          TAS_SIMU_PROGRESS ( NULL, "Parsing spice results",1 );
        sim_parse_spiceout (model);
  
        // drive file to plot
        if ( plot ) {
          critic = ((ttvpath_list*)criticlist->DATA)->CRITIC;
          for (; critic; critic = critic->NEXT) name2plot = addchain(name2plot, critic->NAME);
          name2plot = reverse(name2plot);
          for (spisignal = spisig ; spisignal ; spisignal = spisignal->NEXT)
            sig2plot = addchain (sig2plot, spisignal);
          sim_drive_plot_file (model,sig2plot, name2plot);
          freechain (sig2plot);
          freechain (name2plot);
        }
 
        tas_get_delay_slope (figext->NAME,
                             model,
                             spisig,
                             (ttvpath_list*)criticlist->DATA,
                             dstype,
                             input_slope);
      }
    res = 1;
  }
  if ( added_capa ) efg_del_extra_capa ( added_capa );
  if (list_simmodel != NULL) freeptype ( list_simmodel );
  if (spisig != NULL)
  {
    tas_free_spisig_marks (spisig);
    efg_FreeSpiSigList ( spisig );
  }
  if (model != NULL) sim_model_clear (model->FIG->NAME);
  if (figext != NULL)
   {
    efg_FreeMarksOnExtFig (figext);
    dellofig (figext->NAME);
   }
  efg_FreeMarksOnFig (lofig);
  efg_freecontext ();
 

#ifdef AVERTEC_LICENSE
   if(avt_givetoken("HITAS_LICENSE_SERVER", "cpe")!=AVT_VALID_TOKEN) EXIT(1);
#endif

  EFG_CALC_EQUI_CAPA = old_simoutload;
  if( oldtransienttime > 0.0 )
    V_FLOAT_TAB[ __SIM_TIME ].VALUE = oldtransienttime ;
  return res;
}

#define MAX_CRITIC 10000
#define TEMP_CRITIC_PTYPE 0xfab60705

typedef struct
{
  int delay;
  int delaymin;
  ttvcritic_list *tc;
  int onpath;
} sort_critic;

int sort_critic_func(const void *a0, const void *b0)
{
  sort_critic *a=(sort_critic *)a0, *b=(sort_critic *)b0;
  if (a->delay<b->delay) return -1;
  if (a->delay>b->delay) return 1;
  return 0;
}

int ttv_path_is_activable(ttvfig_list *tvf, cnsfig_list *cf, ttvpath_list *check_path, chain_list *pathlist, char *divname, char *convname)
{
  chain_list *cl, *ch, *ordered_pathlist, *cone_onpath, *chaincone;
  ttvcritic_list *tc, *lasttc;
  ttvpath_list *pth;
  long val;
  int nb=0, i, err;
  sort_critic *tab;
  chain_list *path;
  ptype_list *patterns;
  spisig_list *spisig;
  lofig_list *lf;
  AdvancedNameAllocator *ana=NULL;
  ptype_list *pt;
  int setnumberonly;
  int cmddir;
  char *cmdsig;
  chain_list *allcorrel;

//  ana=CreateAdvancedNameAllocator(CASE_SENSITIVE);

  lf=getloadedlofig(cf->NAME);

  for (lasttc=check_path->CRITIC; lasttc!=NULL && lasttc->NEXT!=NULL; lasttc=lasttc->NEXT) ;

  for (cl=pathlist, ch=NULL; cl!=NULL; cl=cl->NEXT)
    {
      pth=(ttvpath_list *)cl->DATA;
      tc=pth->CRITIC;
      if (tc==NULL) continue;
      if (pth!=check_path) ch=addchain(ch, tc);
    }
  ordered_pathlist=/*reverse(*/addchain(ch, check_path->CRITIC)/*)*/;

  tab=mbkalloc(sizeof(sort_critic)*MAX_CRITIC);
  
  for (cl=ordered_pathlist; cl!=NULL; cl=cl->NEXT)
    {
      tc=(ttvcritic_list *)cl->DATA;
      val=0;
      if (divname!=NULL)
        {
          while (tc!=NULL && strcmp(tc->NAME, divname)!=0) tc=tc->NEXT;
          if (tc==NULL) {printf("ttv_path_activable: error 1\n");exit(7);}
        }
      setnumberonly=0;
      while (tc!=NULL)
        {
          if (tc->REFDELAY>0)
            val+=tc->REFDELAY;
          else
            val+=1;

          if ((pt=getptype(tc->USER, TEMP_CRITIC_PTYPE))!=NULL)
            i=(int)(long)pt->DATA;
          else
            {
              if (ana==NULL) ana=CreateAdvancedNameAllocator(CASE_SENSITIVE);
              i=AdvancedNameAlloc(ana, tc->NETNAME);
              tc->USER=addptype(tc->USER, TEMP_CRITIC_PTYPE, (void *)(long)i);
            }

//          for (i=0; i<nb && strcmp(tc->NETNAME, tab[i].tc->NETNAME)!=0; i++) ;
          for (err=nb; err<=i; err++) tab[err].tc=NULL, tab[err].delay=INT_MAX;
          if (i>=nb || tab[i].tc==NULL) 
            {
              if (i>=nb) nb=i+1;
              if (!setnumberonly)
                {
                  tab[i].delay=tab[i].delaymin=val; 
                  if (cl==ordered_pathlist) tab[i].onpath=1;
                  else tab[i].onpath=0;              
                  tab[i].tc=tc;
                }
            }

          if (!setnumberonly)
            {
              if (tab[i].onpath)
                {
                  if (tab[i].tc->SNODE!=tc->SNODE)
                    setnumberonly=1; //break; // wrong event to propagate
                  else
                    val=tab[i].delay; // sinon, resynchronisation du chemin sur celui que l'on veut tester
                }
              else
                {                  
                  if (val<tab[i].delaymin) tab[i].delaymin=val;
                  if (val>tab[i].delay) tab[i].delay=val;
                }
            }
//          printf("add. %s %c\n",tab[i].tc->NETNAME, tab[i].tc->SNODE==TTV_UP?'u':'d');
          if (convname!=NULL && strcmp(tc->NETNAME, convname)==0) break;
          tc=tc->NEXT;
        }
    }

  freechain(ordered_pathlist);
  if (ana!=NULL) FreeAdvancedNameAllocator(ana);

  qsort(tab, nb, sizeof(*tab), sort_critic_func);

  path=NULL;
  for (i=0; i<nb; i++)
    {
      if (tab[i].tc==NULL) continue;
      if (check_path->CMD!=NULL && tab[i].tc==lasttc)
        {
          char buf[1024];
          ttv_getnetname(tvf,buf,check_path->CMD->ROOT) ;
          cmdsig=namealloc(buf);
          if (check_path->CMD->TYPE & TTV_NODE_UP) cmddir=EFG_SPISIG_CMDDIR_MASK; else cmddir=0;
        }
      else
        cmdsig=NULL, cmddir=0;
      if ((tab[i].tc->TYPE & TTV_FIND_HZ)==TTV_FIND_HZ && tab[i].tc->NEXT==NULL)
        cmddir|=EFG_SPISIG_HZ_MASK;
      path=efg_AddPathEvent(path, tab[i].tc->NETNAME, tab[i].tc->SNODE==TTV_UP?EFG_RISE:EFG_FALL,!tab[i].onpath?1:0,tab[i].delaymin, tab[i].delay, -1, -1, cmdsig, cmddir, NULL);
//      printf("%d. %s %c %c [%d %d]\n",i, tab[i].tc->NETNAME, tab[i].tc->SNODE==TTV_UP?'u':'d', tab[i].onpath?'x':'o', tab[i].delaymin, tab[i].delay);
    }

  mbkfree(tab);
  path = reverse (path);
  spisig = efg_BuildSpiSigList (lf, path, NULL, 1, &err, 1);
  efg_FreePathEvent(path);

  if (!err)
   {
    cone_onpath = efg_getcone_onpath (spisig, cf);

    efg_createcontext ();
    efg_setsrcfig2context  (lf);
    efg_setspisig2context (spisig);

    chaincone=cone_onpath->NEXT;
    gsp_new_numbercones (lf->NAME,chaincone);

    GSP_QUIET_MODE=1;
    GSP_REAL_ORDER_SET=1;

    allcorrel=NULL;

    patterns=gsp_spisetinputs_from_cone(lf->NAME, chaincone, 'w', &allcorrel);

    gsp_freecorrel(allcorrel);
    
    GSP_REAL_ORDER_SET=0;
    GSP_QUIET_MODE=0;

    gsp_FreeMarksOnCnsfigFromChainCone((cone_onpath && cone_onpath->DATA==NULL)?cone_onpath->NEXT:cone_onpath);
//    gsp_FreeMarksOnCnsfig (cf);
    tas_free_spisig_marks (spisig);
    efg_DelHierSigPtypeFromSpisig(spisig);
//    efg_FreeMarksOnFig (lf);
    efg_FreeSpiSigList (spisig);
    efg_freecontext ();

    freeptype(patterns);
    freechain(cone_onpath);
   }
  else
   {
     tas_free_spisig_marks (spisig);
     efg_DelHierSigPtypeFromSpisig(spisig);
     efg_FreeSpiSigList (spisig);
     return 2; // dans le doute, mais on signal l'erreur
   }

  if (patterns==NULL) return 0;
  return 1;
}

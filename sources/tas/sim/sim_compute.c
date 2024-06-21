/*****************************************************************************/
/*                                                                           */
/*                      Chaine de CAO & VLSI   AVERTEC                       */
/*                                                                           */
/*    Produit : SIM Version 2.00                                             */
/*    Fichier : sim_compute.c                                                */
/*                                                                           */
/*    (c) copyright 2000 AVERTEC                                             */
/*    Tous droits reserves                                                   */
/*                                                                           */
/*    Auteur(s) : Gilles Augustins                                           */
/*                Marc   Kuoch                                               */
/*                                                                           */
/*****************************************************************************/

#include SIM_H
#include "sim.h"
#include "sim_meas.h"

chain_list *SIM_HEAD_TMG = NULL; // init for each action !!!!

/*---------------------------------------------------------------------------*/

char sim_gettype (char type)
{
  char res;

  if (type == 'C' || (type == 'c'))
    res = SIM_MEASURE_LOCON;
  else if (type == 'S' || (type == 's'))
    res = SIM_MEASURE_SIGNAL;
  else
    res = 'X';
  return res;
}

/*---------------------------------------------------------------------------*/

sim_timing *sim_GetTmgList (sim_model *model, char type1, char *name1,
                            char type2, char *name2)
{
  SIM_FLOAT           tpfinal,tpinit;
  SIM_FLOAT           vth,vthl,vthh,vthe;
  SIM_FLOAT           time, step, vddmax;
  SIM_FLOAT          *tab1,*tab2;
  SIM_FLOAT           delay = 0.0;
  SIM_FLOAT           slope = 0.0;
  sim_measure        *m1 = NULL,*m2 = NULL;
  sim_measure_detail *md1 = NULL, *md2  = NULL;
  int                 indice = 1;
  char                trans1 = 'X';
  char                trans2 = 'X';
  char                t1,t2;
  sim_timing         *delay_list = NULL;

  if ((!name1 && !name2) || (!name2))
    return NULL;
  else {
    t1 = sim_gettype (type1);
    t2 = sim_gettype (type2);
    if (((t1 == 'X') && (name1)) || (t2 == 'X'))
      return NULL;
    if (name1) {
      m1 = sim_measure_get ( model,
                             name1,
                             t1,
                             SIM_MEASURE_VOLTAGE
                             );
    }
    m2 = sim_measure_get ( model,
                           name2,
                           t2,
                           SIM_MEASURE_VOLTAGE
                           );
  }
  if ((!m1 && name1) || (!m2))
    return NULL;

  /* Results array */
   
  time = sim_parameter_get_tran_tmax (model);
  step = sim_parameter_get_tran_step (model);
  vddmax = sim_parameter_get_alim (model, 'i');
  vth  = sim_parameter_get_delayVTHSTART  (model)*vddmax;
  vthe  = sim_parameter_get_delayVTHEND  (model)*vddmax;
  vthl = sim_parameter_get_slopeVTHL (model)*vddmax;
  vthh = sim_parameter_get_slopeVTHH (model)*vddmax;
    

  if (!m1 && m2) {
    // Only fill slope
    while ((md2 = sim_measure_detail_scan(m2,md2))) {
      /* Compute timings */
      tab2 = sim_measure_detail_get_data( md2 );
      indice = 0;
      sim_get_delay_slope (model,tab2,&tpfinal,&slope,&indice,&trans2,
                           vthe,vthl,vthh,time);
      if (tpfinal < 0.0)
        tpfinal= 0.0;

      delay = tpfinal - tpinit;
      // Fill the timing structure
      delay_list  = sim_timing_add (delay_list,NULL,name2);
      sim_timing_setnodesname (delay_list,NULL,
                               sim_measure_detail_get_name (md2));
      sim_timing_setslope (delay_list,slope);
      sim_timing_setnodeevent (delay_list,trans2);
    }
  }
  else
    while ((md1 = sim_measure_detail_scan(m1,md1))) {
      indice = 0;
      tab1 = sim_measure_detail_get_data( md1 );
      sim_get_delay_slope (model,tab1,&tpinit,&slope,&indice,&trans1,
                           vth,vthl,vthh,time);
      if (tpinit < 0.0)
        tpinit = 0.0;
      while ((md2 = sim_measure_detail_scan(m2,md2))) {
        /* Compute timings */
        tab2 = sim_measure_detail_get_data( md2 );
        indice = 0;
        sim_get_delay_slope (model,tab2,&tpfinal,&slope,&indice,&trans2,
                             vthe,vthl,vthh,time);
        if (tpfinal < 0.0)
          tpfinal= 0.0;

        delay = tpfinal - tpinit;
        // Fill the timing structure
        delay_list  = sim_timing_add (delay_list,name1,name2);
        sim_timing_setnodesname (delay_list,sim_measure_detail_get_name (md1),
                                 sim_measure_detail_get_name (md2));
        sim_timing_setdelay (delay_list,delay);
        sim_timing_setslope (delay_list,slope);
        sim_timing_setrootevent (delay_list,trans1);
        sim_timing_setnodeevent (delay_list,trans2);
      }
    }
  if (delay_list != NULL)
    SIM_HEAD_TMG = addchain (SIM_HEAD_TMG,delay_list);
  return delay_list;
}

/*---------------------------------------------------------------------------*/

SIM_FLOAT sim_get_tmg_delay (sim_timing *tmglist, char delaytype)
{
  sim_timing *tmg;
  SIM_FLOAT   delay =  0.0;
  SIM_FLOAT   dmin  = -1.0;
  SIM_FLOAT   dmax  = -1.0;

  for (tmg = tmglist ; tmg ; tmg = tmg->NEXT) {
    delay = tmg->DELAY;
    if (dmin < 0.0)
      dmin = delay;
    if (dmax < 0.0)
      dmax = delay;
    if (delay < dmin)
      dmin = delay;
    if (delay > dmax)
      dmax = delay;
  }
  if (delaytype == SIM_MIN)
    delay =  dmin;
  else if (delaytype == SIM_MAX)
    delay = dmax;
  else {
    fprintf(stderr, "[SIM WAR] sim_get_tmg_delay : unknown delaytype, delay max assumed\n");
    delay = dmax;
  }
  return delay;
}

/*---------------------------------------------------------------------------*/

SIM_FLOAT sim_get_tmg_slope (sim_timing *tmglist, char stype)
{
  sim_timing *tmg;
  SIM_FLOAT   slope =  0.0;
  SIM_FLOAT   smin  = -1.0;
  SIM_FLOAT   smax  = -1.0;

  for (tmg = tmglist ; tmg ; tmg = tmg->NEXT) {
    slope = tmg->SLOPE;
    if (smin < 0.0)
      smin = slope;
    if (smax < 0.0)
      smax = slope;
    if (slope < smin)
      smin = slope;
    if (slope > smax)
      smax = slope;
  }

  if (stype == SIM_MIN)
    slope =  smin;
  else if (stype == SIM_MAX)
    slope = smax;
  else {
    fprintf(stderr, "[SIM WAR] sim_get_tmg_slope : unknown slopetype, slope max assumed\n");
    slope = smax;
  }
  return slope;
}

/*---------------------------------------------------------------------------*/

SIM_FLOAT sim_getdelay (sim_model *model, char delaytype, char type1, char *name1,
                        char type2, char *name2)
{
  sim_timing *tmglist;

  if ((!name1) || (!name2))
    return 0.0;
  if ((tmglist = sim_timing_get(name1,name2)) == NULL )
    tmglist = sim_GetTmgList (model, type1, name1,type2, name2);
  /* on detruit l'ancien tmg car on n'avait pas specifie l'event entre name1 et name2*/
  else {
    sim_timing_free (tmglist);
    tmglist = sim_GetTmgList (model, type1, name1,type2, name2);
  }

  return sim_get_tmg_delay(tmglist,delaytype);
}

/*---------------------------------------------------------------------------*/

SIM_FLOAT sim_getslope (sim_model *model, char stype, char type, char *name)
{
  sim_timing *tmglist;

  if (!name)
    return 0.0;
  if ((tmglist = sim_timing_get(NULL,name)) == NULL )
    tmglist = sim_GetTmgList (model, 'X', NULL,type, name);
  /* on detruit l'ancien tmg car on n'avait pas specifie l'event entre name1 et name2*/
  else {
    sim_timing_free (tmglist);
    tmglist = sim_GetTmgList (model, 'X', NULL,type, name);
  }

  return sim_get_tmg_slope(tmglist,stype);
}

/*---------------------------------------------------------------------------*/

SIM_FLOAT sim_get_trans_delay (sim_model *model, char delaytype, char type1,
                               char *name1, char type2, char *name2,
                               char *transition, SIM_FLOAT th_in, SIM_FLOAT th_out)
{
  //    SIM_FLOAT           vth,vthl,vthh;
  SIM_FLOAT           tpinit = -1.0, lasttp;
  SIM_FLOAT           tpfinal = -1.0;
  SIM_FLOAT           time, step, vddmax;
  SIM_FLOAT          *tab1,*tab2;
  SIM_FLOAT           delay;
  SIM_FLOAT           dmin = -1.0, dmax = -1.0,slope=0.0;
  sim_measure        *m1,*m2;
  sim_measure_detail *md1 = NULL, *prevmd1, *prevmd2,*md2  = NULL;
  int                 indice;
  char                trans = 'X';
  //    char               *endstr;
  char                trans1,trans2;
  long                ind1 = 1,ind2 = 1,ind;
  char                t1,t2,delay_ko;
  sim_timing         *tmglist = NULL;
  char               trans_dir[2];
  long               trans_num[2];

  sim_getTransition(model, transition, trans_dir, trans_num);
  trans1=toupper(trans_dir[0]); ind1=trans_num[0];
  trans2=toupper(trans_dir[1]); ind2=trans_num[1];

  /*
    vth  = sim_parameter_get_delayVTH  (model)*vddmax;
    vthl = sim_parameter_get_slopeVTHL (model)*vddmax;
    vthh = sim_parameter_get_slopeVTHH (model)*vddmax;
  */
  if ((!name1) || (!name2) || (ind1 <= 0) || (ind2 <= 0) || 
      ((trans1 != 'U') && (trans1 != 'D')) || ((trans2 != 'U') && (trans2 != 'D')))
    return 0.0;
  else {
    t1 = sim_gettype (type1);
    t2 = sim_gettype (type2);
    if ((t1 == 'X') || (t2 == 'X'))
      return 0.0;
    m1 = sim_measure_get ( model,
                           name1,
                           t1, 
                           SIM_MEASURE_VOLTAGE
                           );
    m2 = sim_measure_get ( model,
                           name2,
                           t2,
                           SIM_MEASURE_VOLTAGE
                           );
  }
  if ((!m1) || (!m2))
    return 0.0;

  /* Results array */
   
  time = sim_parameter_get_tran_tmax (model);
  step = sim_parameter_get_tran_step (model);
  vddmax = sim_parameter_get_alim (model, 'i');
    
  while ((md1 = sim_measure_detail_scan(m1,md1))) {
    prevmd1 = md1;
    tab1 = sim_measure_detail_get_data( md1 );
    indice = 0;
    ind = 1;
    lasttp=tpinit;
    while (ind <= ind1 ) {
      sim_get_delay_slope (model,tab1,&tpinit,&slope,&indice,&trans,
                           th_in,th_in*0.90,th_in*1.10,time);                 
      if (tpinit <= 0.0) {
        tpinit = 0.0;
        /*
          fprintf(stderr, "[SIM WAR]: transition %c %ld for %s not found\n",
          trans1,ind1,name1);
        */
        if (ind1==SIM_MEASURE_LAST && lasttp>0)
          tpinit=lasttp;
        else
          delay_ko = 'Y';
        break;
      }
      if (trans == trans1)
        {
          ind++;
          lasttp=tpinit;
        }
    }
    while ((md2 = sim_measure_detail_scan(m2,md2))) {
      prevmd2 = md2;
      delay_ko = 'N';
      /* Compute timings */
      tab2 = sim_measure_detail_get_data( md2 );
      indice = 0;
      ind = 1;
      lasttp=tpfinal;
      while (ind <= ind2 ) {
        sim_get_delay_slope (model,tab2,&tpfinal,&slope,&indice,&trans,
                             th_out,th_out*0.90,th_out*1.10,time);
        if (tpfinal <= 0.0) {
          tpfinal = 0.0;
          /*
            fprintf(stderr, "[SIM WAR]: transition %c %ld for %s not found\n",
            trans2,ind2,name2);
          */
          if (ind2==SIM_MEASURE_LAST && lasttp>0)
            tpfinal=lasttp;
          else
            delay_ko = 'Y';
          break;
        }
        if (trans == trans2)
          {
            ind++;
            lasttp=tpfinal;
          }
      }
      if (delay_ko == 'N')
        delay = tpfinal - tpinit;
      else
        delay = 0.0;
      if (dmin < 0.0)
        dmin = delay;
      if (dmax < 0.0)
        dmax = delay;
      if (delay < dmin)
        dmin = delay;
      if (delay > dmax)
        dmax = delay;
    }
  }

  if (delaytype == SIM_MIN)
    delay =  dmin;
  else if (delaytype == SIM_MAX)
    delay = dmax;
  else {
    fprintf(stderr, "[SIM WAR] getdelay between %s %s: unknown delaytype, delay max assumed\n",name1,name2);
    delay = dmax;
  }
  // Fill timing structure
  // on met a jour le delay s'il existait
  if (delay_ko == 'N') {
    if ((tmglist = sim_timing_get_by_event (name1,name2,transition)) == NULL) {
      tmglist  = sim_timing_add (tmglist,name1,name2);
      sim_timing_setnodesname (tmglist,sim_measure_detail_get_name (prevmd1),
                               sim_measure_detail_get_name (prevmd2));
      sim_timing_setdelay (tmglist,delay);
      sim_timing_setslope (tmglist,slope);
      sim_timing_setrootevent (tmglist,trans1);
      sim_timing_setnodeevent (tmglist,trans2);
      sim_timing_setrooteventidx (tmglist,ind1);
      sim_timing_setnodeeventidx (tmglist,ind2);
      SIM_HEAD_TMG = addchain (SIM_HEAD_TMG,tmglist);
    }
    else 
      sim_timing_setdelay (tmglist,delay);
  }
  return delay;
}

/*---------------------------------------------------------------------------*/

SIM_FLOAT sim_get_trans_slope (sim_model *model, char slopetype, char typenode,char *name, char *transition, SIM_FLOAT th_in, SIM_FLOAT th_out) 
{
  SIM_FLOAT       tc,slope=-1.0,smin = -1.0, smax = -1.0, lastslope;
  //    SIM_FLOAT       vth,vthl,vthh;
  SIM_FLOAT       time, step;
  SIM_FLOAT      *tab;
  sim_measure    *m;
  sim_measure_detail *md = NULL;
  char            t,trans2find,trans;
  int             indice,ind;
  long            num;
  char            trans_dir[2];
  long            trans_num[2];


  if ((!name))
    return 0.0;
  else {
    if ((t = sim_gettype (typenode)) == 'X')
      return 0.0;
    if ((m = sim_measure_get( model, 
                              name, 
                              t, 
                              SIM_MEASURE_VOLTAGE )) == NULL)
      return 0.0;
  }
    
  // Results array 
  time = sim_parameter_get_tran_tmax (model);
  step = sim_parameter_get_tran_step (model);
  /*    vddmax = sim_parameter_get_alim (model, 'i');
        vth  = sim_parameter_get_delayVTH  (model);
        vthl = sim_parameter_get_slopeVTHL (model);
        vthh = sim_parameter_get_slopeVTHH (model);*/

  sim_getTransition(model, transition, trans_dir, trans_num);
  trans2find=toupper(trans_dir[0]); num=trans_num[0];
  //    trans2=toupper(trans[1]); ind2=trans_num[1];
  /*
    trans2find = transition[0];
    if (transition[1]=='l' || transition[1]=='L')
    {
    num=SIM_MEASURE_LAST;
    }
    else
    num = strtol (&transition[1],NULL,0);
  */


  while ((md = sim_measure_detail_scan(m,md))) {
    tab = sim_measure_detail_get_data( md );
    indice = 0;
    ind = 1;
    lastslope=slope;
    while (ind <= num ) {
      sim_get_delay_slope (model,tab,&tc,&slope,&indice,&trans,
                           (th_in+th_out)/2,th_in,th_out,time);
      if (tc <= 0.0) {
        tc = 0.0;
        if (num==SIM_MEASURE_LAST && lastslope>0)
          slope=lastslope;
        else
          {
            avt_error("sim", -1, AVT_ERR, "Transition \"%c%ld\" of signal '%s' not found\n", trans2find, num, name);
          }
        break;
      }
      if (trans == trans2find)
        {
          ind++;
          lastslope=slope;
        }

    }

    if (smin < 0.0)
      smin = slope;
    if (smax < 0.0)
      smax = slope;
    if (slope <= smin)
      smin = slope;
    if (slope >= smax)
      smax = slope;
  }
  if (slopetype == SIM_MIN)
    slope =  smin;
  else if (slopetype == SIM_MAX)
    slope = smax;
  else {
    fprintf(stderr, "[SIM WAR] getslope for %s: unknown slopetype, slope max assumed\n",name);
    slope = smax;
  }
  return slope;
}

/*- Retourne le 1er temps de passage a v = voltage --------------------------*/

SIM_FLOAT sim_get_commut_instant (sim_model *model, char *name, char type,
                                  SIM_FLOAT voltage)
{
  SIM_FLOAT       time,step,vddmax,vth,vthl,vthh;
  SIM_FLOAT       delay, slope;
  sim_measure        *m = NULL;
  sim_measure_detail *md = NULL;
  SIM_FLOAT          *tab;
  char                event;
  int                 indice;
  char                type_node;

  if (!model || !name) return 0.0;
    
  time = sim_parameter_get_tran_tmax (model);
  step = sim_parameter_get_tran_step (model);
  vddmax = sim_parameter_get_alim (model, 'i');
  vthl = sim_parameter_get_slopeVTHL (model)*vddmax;
  vthh = sim_parameter_get_slopeVTHH (model)*vddmax;
    
  vth  = voltage/* / vddmax*/;

  type_node = sim_gettype (type);
  if (type_node != 'X') {
    m = sim_measure_get ( model,
                          name,
                          type_node, 
                          SIM_MEASURE_VOLTAGE
                          );
  }
  else {
    m = sim_measure_get ( model,
                          name,
                          SIM_MEASURE_LOCON, 
                          SIM_MEASURE_VOLTAGE
                          );
    if ( !m )
      m = sim_measure_get ( model,
                            name,
                            SIM_MEASURE_SIGNAL, 
                            SIM_MEASURE_VOLTAGE
                            );
  }
  if (m != NULL) 
    md = sim_measure_detail_scan(m,md) ;
  else {
    fprintf (stderr, "[SIM WAR] sim_get_commut_instant : can't get measure on %s\n",name);
    return 0.0;
  }
  if (md != NULL)
    tab = sim_measure_detail_get_data( md );

  if (!tab) return 0.0;

  indice = 0;
  sim_get_delay_slope (model,tab,&delay,&slope,
                       &indice,&event,
                       vth,vthl,vthh,time);

  return delay;
}

/*---------------------------------------------------------------------------*/

void sim_get_delay_slope (sim_model *model, SIM_FLOAT *tab, SIM_FLOAT *delay, 
                          SIM_FLOAT *slope, int *indice, char *trans,
                          SIM_FLOAT vth, SIM_FLOAT vthl, SIM_FLOAT vthh, SIM_FLOAT time)
{
  SIM_FLOAT       step ;

  if (!tab) {*delay=-1; return;}
    
  /* Results array */
  step   = sim_parameter_get_tran_step (model);
  //    vddmax = sim_parameter_get_alim (model, 'i');

  sim_GetDelaySlopeByIndex (tab,delay,slope,vth,vthl,vthh,
                            time,step,indice,trans);

}

/*---------------------------------------------------------------------------*/

char sim_get_tc_between_2_indices (sim_model *model, 
                                   SIM_FLOAT *tab,
                                   SIM_FLOAT *delay, 
                                   SIM_FLOAT *slope,
                                   int *indice1,
                                   int indice2,
                                   char *trans,
                                   SIM_FLOAT vth,
                                   SIM_FLOAT vthl,
                                   SIM_FLOAT vthh,
                                   SIM_FLOAT time)
{
  SIM_FLOAT       step, vddmax;
  char            event = 'X';

  if (!tab)
    return event;
    
  /* Results array */
  step = sim_parameter_get_tran_step (model);
  vddmax = sim_parameter_get_alim (model, 'i');

  sim_GetDelaySlopeByIndex (tab,delay,slope,vth*vddmax,vthl*vddmax,vthh*vddmax,
                            time,step,indice1,trans);
  if ((indice2 > 0) && (*indice1 <= indice2))
    event = *trans;
  else if ((indice2 <= 0) && (*delay > 0.0))
    event = *trans;
  return event;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : sim_get_setup_time                                             */ 
/*                                                                           */
/*---------------------------------------------------------------------------*/

SIM_FLOAT sim_get_setup_time (sim_model *model, char *data, 
                              SIM_FLOAT tstart_d, SIM_FLOAT tslope_d, char sens_d,
                              char *cmd, SIM_FLOAT tstart_min_c, SIM_FLOAT tstart_max_c, 
                              SIM_FLOAT tslope_c, char sens_c,
                              char *mem, int data_val)
{
  SIM_FLOAT       time, step;
  SIM_FLOAT       setup = 0.0;
  SIM_FLOAT       delay = 0.0;
  SIM_FLOAT       init_time;
  int             init_mem;
  SIM_FLOAT       time_g;
  SIM_FLOAT       time_d, th_in, th_out;
  char            last_mem_event,bad_mem_event;
  char            trans_data_cmd[8];
  char            trans_data_mem[8];
  char            trans_data_mem_bad[8];
  char           *label_data_mem;
  char           *label_data_mem_bad;
  char           *label_data_cmd;
  char            buf[1024];
  int             valid;
  char dir='|';

  time = sim_parameter_get_tran_tmax (model);
  step = sim_parameter_get_tran_step (model);

  switch (data_val) {
  case 0 :  init_mem = SIM_ONE;
    sprintf(trans_data_mem,"%cD",sens_d);
    sprintf(trans_data_mem_bad,"%cUL",sens_d);
    bad_mem_event = SIM_RISE;
    break;
  case 1 :  init_mem = SIM_ZERO;
    sprintf(trans_data_mem,"%cU",sens_d);
    sprintf(trans_data_mem_bad,"%cDL",sens_d);
    bad_mem_event = SIM_FALL;
    break;
  default : fprintf(stderr,"[SIM WAR] sim_get_setup_time data value must be 1 or 0\n");
    return 0.0;
  }
  sprintf (trans_data_cmd,"%c%c",sens_d,sens_c);

  sim_get_THR(model, &th_in, &th_out, NULL, NULL);

  sim_ic_set_level (model, mem, SIM_IC_SIGNAL, init_mem);
    
  if ( V_BOOL_TAB[__SIM_USE_PRINT].VALUE ) {
    sim_measure_set_signal (model, mem);
    sim_measure_set_locon (model, data);
    sim_measure_set_locon (model, cmd);
  }
  if ( V_BOOL_TAB[__SIM_USE_MEAS].VALUE ) {
    label_data_cmd = sim_create_label (data,cmd);
    label_data_mem = sim_create_label (data,mem);
    sprintf (buf,"%s_last",mem);
    label_data_mem_bad = sim_create_label (data,buf);
    sim_addMeasDelay (model,label_data_cmd,data,cmd,trans_data_cmd, SIM_MAX);
    sim_addMeasDelay (model,label_data_mem,data,mem,trans_data_mem, SIM_MAX);
    sim_addMeasDelay (model,label_data_mem_bad,data,mem,trans_data_mem_bad, SIM_MAX);
  }

  sim_input_set_slope_single (model, data, sens_d, tslope_d, tstart_d,SIM_INPUT_LOCON);

  time_g = tstart_min_c;
  if (time_g <= 0.0)
    time_g = 5.0*step;
  time_d = tstart_max_c;
  if (time_d <= 0.0)
    time_d = 5.0*step;

  init_time = (time_g+time_d)/2.0;
  while (fabs(time_d - time_g) > step) {
    avt_error("sim", -1, AVT_INFO, "Trying setup: %gs (%c)\n",init_time-tstart_d, dir);
    sim_input_set_slope_single (model, cmd, sens_c, tslope_c,init_time,SIM_INPUT_LOCON);
    /* Simulate and getdelay */
    sim_run_simu (model, SIM_RUN_ALL, NULL, NULL);
    sim_parse_spiceout (model);
    if ( V_BOOL_TAB[__SIM_USE_PRINT].VALUE ) {
      delay = sim_get_trans_delay  (model, SIM_MAX,'c',data,'s',mem,trans_data_mem, th_in, th_out);
      if (fabs(delay) <= 0.0) 
        time_g = init_time, dir='>';
      else {
        last_mem_event = sim_get_last_node_event (model,mem,'s');
        if (last_mem_event != bad_mem_event) {
          time_d = init_time, dir='<';
          setup = sim_get_trans_delay  (model, SIM_MAX,'c',data,'c',
                                        cmd,trans_data_cmd, th_in, th_out);
        }
        else
          time_g = init_time, dir='>';
      }
    }
    if ( V_BOOL_TAB[__SIM_USE_MEAS].VALUE ) {
      valid = sim_getMeasDelay (model,label_data_mem,&delay);
      if (!valid || (fabs(delay) <= 0.0)) 
        time_g = init_time;
      else {
        valid = sim_getMeasDelay (model,label_data_mem_bad,&delay);
        if ( !valid ) {
          time_d = init_time;
          valid = sim_getMeasDelay (model,label_data_cmd,&setup);
        }
        else
          time_g = init_time;
      }
    }
    init_time = (time_d+time_g)/2.0;
  }
  return setup;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : sim_get_hold_time                                              */ 
/*                                                                           */
/*---------------------------------------------------------------------------*/

SIM_FLOAT sim_get_hold_time (sim_model *model, char *data, SIM_FLOAT tstart_d,
                             SIM_FLOAT tslope_d, char sens_d, char *cmd,
                             SIM_FLOAT tstart_min_c, SIM_FLOAT tstart_max_c, 
                             SIM_FLOAT tslope_c,char sens_c,char *mem, int data_val)
{
  SIM_FLOAT       time, step;
  SIM_FLOAT       hold  = 0.0;
  SIM_FLOAT       delay = 1.0;
  SIM_FLOAT       time_g,time_d,init_time, th_in, th_out;
  int             init_mem;
  char            trans_cmd_data[8];
  char            trans_data_mem[8];
  char            trans_data_mem_bad[8];
  char            last_mem_event,bad_mem_event;
  char           *label_data_mem;
  char           *label_data_mem_bad;
  char           *label_cmd_data;
  char            buf[1024];
  int             valid;
  char dir='|';

  time = sim_parameter_get_tran_tmax (model);
  step = sim_parameter_get_tran_step (model);

  switch (data_val) {
  case 0 :  init_mem = SIM_ONE;
    sprintf(trans_data_mem,"%cD",sens_d);
    sprintf(trans_data_mem_bad,"%cUL",sens_d);
    bad_mem_event = SIM_RISE;
    break;
  case 1 :  init_mem = SIM_ZERO;
    sprintf(trans_data_mem,"%cU",sens_d);
    sprintf(trans_data_mem_bad,"%cDL",sens_d);
    bad_mem_event = SIM_FALL;
    break;
  default : fprintf(stderr,"[SIM WAR] sim_get_hold_time data value must be 1 or 0\n");
    return 0.0;
                  
  }

  sim_get_THR(model, &th_in, &th_out, NULL, NULL);

  sprintf(trans_cmd_data,"%c%c",sens_c,sens_d);
  sim_ic_set_level (model, mem, SIM_IC_SIGNAL, init_mem);
  if ( V_BOOL_TAB[__SIM_USE_PRINT].VALUE ) {
    sim_measure_set_signal (model, mem);
    sim_measure_set_locon (model, data);
    sim_measure_set_locon (model, cmd);
  }
  if ( V_BOOL_TAB[__SIM_USE_MEAS].VALUE ) {
    label_cmd_data = sim_create_label (data,cmd);
    label_data_mem = sim_create_label (data,mem);
    sprintf (buf,"%s_last",mem);
    label_data_mem_bad = sim_create_label (data,buf);
    sim_addMeasDelay (model,label_cmd_data,cmd,data,trans_cmd_data, SIM_MAX);
    sim_addMeasDelay (model,label_data_mem,data,mem,trans_data_mem, SIM_MAX);
    sim_addMeasDelay (model,label_data_mem_bad,data,mem,trans_data_mem_bad, SIM_MAX);
  }
  sim_input_set_slope_single (model, data, sens_d, tslope_d, tstart_d,SIM_INPUT_LOCON);

  time_g = tstart_min_c;
  if (time_g < 0.0)
    time_g = 5.0*step;
  time_d = tstart_max_c;
  if (time_d < 0.0)
    time_d = 5.0*step;
  init_time = (time_g+time_d)/2.0;

  while (fabs(time_d - time_g) > step) {
    avt_error("sim", -1, AVT_INFO, "Trying hold: %gs (%c)\n",init_time-tstart_d, dir);
    sim_input_set_slope_single (model, cmd, sens_c, tslope_c, init_time,SIM_INPUT_LOCON);
    /* Simulate and getdelay */
    sim_run_simu (model, SIM_RUN_ALL, NULL, NULL);
    sim_parse_spiceout (model);
    if ( V_BOOL_TAB[__SIM_USE_PRINT].VALUE ) {
      delay = sim_get_trans_delay (model, SIM_MAX,'c',data,'s',mem,trans_data_mem, th_in, th_out);
      if (fabs(delay) <= 0.0) 
        time_d = init_time, dir='<';
      else {
        last_mem_event = sim_get_last_node_event (model,mem,'s');
        if (last_mem_event != bad_mem_event) {
          time_g = init_time, dir='>';
          hold = sim_get_trans_delay  (model, SIM_MAX,'c',cmd,'c',
                                       data,trans_cmd_data, th_in, th_out);
        }
        else
          time_d = init_time, dir='<';
      }
    }
    if ( V_BOOL_TAB[__SIM_USE_MEAS].VALUE ) {
      valid = sim_getMeasDelay (model, label_data_mem,&delay);
      if (!valid || (fabs(delay) <= 0.0)) 
        time_d = init_time;
      else {
        valid = sim_getMeasDelay (model,label_data_mem_bad,&delay);
        if ( !valid ) {
          time_g = init_time;
          valid = sim_getMeasDelay (model,label_cmd_data,&hold);
        }
        else
          time_d = init_time;
      }
    }

    init_time = (time_d+time_g)/2.0;
  }
  return hold;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : sim_get_access_time                                            */ 
/*                                                                           */
/*---------------------------------------------------------------------------*/

SIM_FLOAT sim_get_access_time (sim_model *model, char *dout, int dout_val, char *cmd, 
                               SIM_FLOAT tstart_c, SIM_FLOAT tslope_c, char sens_c,
                               char *mem, int mem_val, double *res_slope)
{
  SIM_FLOAT       time, step;
  int             init_mem;
  int             init_out;
  SIM_FLOAT          access, th_in, th_out, slopelow, slopehigh;
  char            trans_cmd_dout[8];
  char           *label_cmd_dout;

  time = sim_parameter_get_tran_tmax (model);
  step = sim_parameter_get_tran_step (model);

  switch (mem_val) {
  case 0 :  init_mem = SIM_ZERO;
    break;
  case 1 :  init_mem = SIM_ONE;
    break;
  default : fprintf(stderr,"[SIM WAR] sim_get_access_time mem value must be 1 or 0\n");
    return 0.0;
  }
  switch (dout_val) {
  case 0 :  init_out = SIM_ONE;
    sprintf(trans_cmd_dout,"%cDl",sens_c);
    break;
  case 1 :  init_out = SIM_ZERO;
    sprintf(trans_cmd_dout,"%cUl",sens_c);
    break;
  default : fprintf(stderr,"[SIM WAR] sim_get_access_time dout value must be 1 or 0\n");
    return 0.0;
  }

  sim_get_THR(model, &th_in, &th_out, &slopelow, &slopehigh);

  sim_ic_set_level (model, mem, SIM_IC_SIGNAL, init_mem);
  sim_ic_set_level (model, dout, SIM_IC_LOCON, init_out);
  if ( V_BOOL_TAB[__SIM_USE_PRINT].VALUE ) {
    sim_measure_set_locon (model, dout);
    sim_measure_set_locon (model, cmd);
  }
  if ( V_BOOL_TAB[__SIM_USE_MEAS].VALUE ) {
    label_cmd_dout = sim_create_label (cmd,dout);
    sim_addMeasDelay (model,label_cmd_dout,cmd,dout,trans_cmd_dout,SIM_MAX);
    if (res_slope!=NULL) sim_addMeasSlope (model,"label_for_myslope",dout,dout_val==0?"D":"U",SIM_MAX);
  }
  sim_input_set_slope_single (model, cmd, sens_c,tslope_c,tstart_c,SIM_INPUT_LOCON);
  /* Simulate and getdelay */
  sim_run_simu (model, SIM_RUN_ALL, NULL, NULL);
  sim_parse_spiceout (model);
  if ( V_BOOL_TAB[__SIM_USE_PRINT].VALUE )
    {
      access = sim_get_trans_delay (model, SIM_MAX,'c',cmd,'c',dout,trans_cmd_dout, th_in, th_out);
      if (res_slope!=NULL) *res_slope=sim_get_trans_slope(model, SIM_MAX, 'c', dout, dout_val==0?"D":"U", slopelow, slopehigh);
    }
  if ( V_BOOL_TAB[__SIM_USE_MEAS].VALUE )
    {
      sim_getMeasDelay(model, label_cmd_dout, &access);
      if (res_slope!=NULL) sim_getMeasDelay(model, label_cmd_dout, res_slope);
    }

  return access;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  Fonctions d'acces a la structure sim_timing                              */
/*                                                                           */
/*---------------------------------------------------------------------------*/

sim_timing *sim_timing_add (sim_timing *head_timing, char *root4usr, char *node4usr)
{
  sim_timing *pt_timing = (sim_timing*)mbkalloc (sizeof (struct sim_timing));

  pt_timing->NEXT = NULL;
  pt_timing->ROOT4USR = root4usr;
  pt_timing->NODE4USR = node4usr;
  pt_timing->ROOT = NULL;
  pt_timing->NODE = NULL;
  pt_timing->DELAY = 0.0;
  pt_timing->SLOPE = 0.0;
  pt_timing->ROOTEVENT = 'x';
  pt_timing->IDXREVENT = 0;
  pt_timing->NODEEVENT = 'x';
  pt_timing->IDXNEVENT = 0;
    
  if (head_timing)
    pt_timing->NEXT = head_timing;

  return pt_timing;
}

/*---------------------------------------------------------------------------*/

sim_timing *sim_timing_get (char *root4usr, char *node4usr)
{
  chain_list *chain;
  sim_timing *tmg,*res = NULL;

  for (chain = SIM_HEAD_TMG ; chain ; chain = chain->NEXT) {
    tmg = (sim_timing*)chain->DATA;
    if ((root4usr && tmg->ROOT4USR && !strcasecmp(root4usr,tmg->ROOT4USR)) && 
        (!strcasecmp(node4usr,tmg->NODE4USR) ||
         !strcasecmp(node4usr,tmg->NODE))) {
      res = tmg;
      break;
    }
  }
  return res;
}

/*---------------------------------------------------------------------------*/

sim_timing *sim_timing_get_by_event (char *root4usr, char *node4usr, char *event)
{
  chain_list *chain;
  sim_timing *tmg,*res = NULL;
  char       rootevent,nodeevent;
  char      *endstr;
  long       idxrootevent = 0,idxnodeevent = 0;

  rootevent    = event[0];
  idxrootevent = strtol (&event[1],&endstr,0);
  nodeevent    = endstr[0];
  idxnodeevent = strtol (&endstr[1],&endstr,0);

  for (chain = SIM_HEAD_TMG ; chain ; chain = chain->NEXT) {
    tmg = (sim_timing*)chain->DATA;
    if ((root4usr && tmg->ROOT4USR && !strcasecmp(root4usr,tmg->ROOT4USR)) && 
        (!strcasecmp(node4usr,tmg->NODE4USR) ||
         !strcasecmp(node4usr,tmg->NODE))) {
      if ((sim_timing_getrootevent(tmg) == rootevent) &&
          (sim_timing_getnodeevent(tmg) == nodeevent) &&
          (sim_timing_getrooteventidx(tmg) == idxrootevent) &&
          (sim_timing_getnodeeventidx(tmg) == idxnodeevent)) {
        res = tmg;
        break;
      }
    }
  }
  return res;
}

/*---------------------------------------------------------------------------*/

void sim_timing_setnodesname (sim_timing *pt_timing, char *root, char *node)
{
  if (pt_timing) {
    if (root != NULL)
      pt_timing->ROOT = strdup (root);
    if (node != NULL)
      pt_timing->NODE = strdup (node);
  }
}

/*---------------------------------------------------------------------------*/

void sim_timing_setdelay (sim_timing *pt_timing, SIM_FLOAT delay)
{
  if (pt_timing) 
    pt_timing->DELAY = delay;
}

/*---------------------------------------------------------------------------*/

void sim_timing_setslope (sim_timing *pt_timing, SIM_FLOAT slope)
{
  if (pt_timing)
    pt_timing->SLOPE = slope;
}

/*---------------------------------------------------------------------------*/

void sim_timing_setnodeeventidx (sim_timing *pt_timing, long idx)
{
  if (pt_timing) 
    pt_timing->IDXNEVENT = idx;
}

/*---------------------------------------------------------------------------*/

void sim_timing_setrooteventidx (sim_timing *pt_timing, long idx)
{
  if (pt_timing) 
    pt_timing->IDXREVENT = idx;
}

/*---------------------------------------------------------------------------*/

void sim_timing_setnodeevent (sim_timing *pt_timing, char event)
{
  if (pt_timing) 
    pt_timing->NODEEVENT = event;
}

/*---------------------------------------------------------------------------*/

void sim_timing_setrootevent (sim_timing *pt_timing, char event)
{
  if (pt_timing) 
    pt_timing->ROOTEVENT = event;
}

/*---------------------------------------------------------------------------*/

SIM_FLOAT sim_timing_getdelay (sim_timing *pt_timing)
{
  if (pt_timing) 
    return pt_timing->DELAY;
  else
    return 0.0;
}

/*---------------------------------------------------------------------------*/

SIM_FLOAT sim_timing_getdelaytype (sim_timing *pt_timing, char delaytype)
{
  SIM_FLOAT dmin = 10e10;
  SIM_FLOAT dmax = -10e10;
  SIM_FLOAT res;
  sim_timing *tmg,*headtmg;
    
  if (!pt_timing)
    res = 0.0;
  else {
    headtmg = sim_timing_get (pt_timing->ROOT4USR,pt_timing->NODE4USR);
    for (tmg = headtmg ; tmg ; tmg = tmg->NEXT) {
      if (tmg->DELAY <= dmin)
        dmin = tmg->DELAY;
      if (tmg->DELAY >= dmax)
        dmax = tmg->DELAY;
    }
  }
  if (delaytype == SIM_MIN)
    res = dmin;
  else
    res = dmax;
  return res;
}

/*---------------------------------------------------------------------------*/

SIM_FLOAT sim_timing_getslope (sim_timing *pt_timing)
{
  if (pt_timing)
    return pt_timing->SLOPE;
  else
    return 0.0;
}

/*---------------------------------------------------------------------------*/

SIM_FLOAT sim_timing_getslopetype (sim_timing *pt_timing, char slopetype)
{
  SIM_FLOAT smin = 10e10;
  SIM_FLOAT smax = -10e10;
  SIM_FLOAT res;
  sim_timing *tmg,*headtmg;
    
  if (!pt_timing)
    res = 0.0;
  else {
    headtmg = sim_timing_get (pt_timing->ROOT4USR,pt_timing->NODE4USR);
    for (tmg = headtmg ; tmg ; tmg = tmg->NEXT) {
      if (tmg->SLOPE <= smin)
        smin = tmg->SLOPE;
      if (tmg->SLOPE >= smax)
        smax = tmg->SLOPE;
    }
  }
  if (slopetype == SIM_MIN)
    res = smin;
  else
    res = smax;
  return res;
}

/*---------------------------------------------------------------------------*/

char sim_timing_getnodeevent (sim_timing *pt_timing)
{
  if (pt_timing) 
    return pt_timing->NODEEVENT;
  else
    return 'X';
}

/*---------------------------------------------------------------------------*/

long sim_timing_getnodeeventidx (sim_timing *pt_timing)
{
  if (pt_timing) 
    return pt_timing->IDXNEVENT;
  else
    return 'X';
}

/*---------------------------------------------------------------------------*/

long sim_timing_getrooteventidx (sim_timing *pt_timing)
{
  if (pt_timing) 
    return pt_timing->IDXREVENT;
  else
    return 'X';
}

/*---------------------------------------------------------------------------*/

char sim_timing_getrootevent (sim_timing *pt_timing)
{
  if (pt_timing) 
    return pt_timing->ROOTEVENT;
  else
    return 'X';
}

/*---------------------------------------------------------------------------*/

void sim_timing_free (sim_timing *head_timing)
{
  sim_timing *pt_timing = head_timing, 
    *free_timing;

  if (!pt_timing) return;
  while (pt_timing->NEXT) {
    free_timing = pt_timing;
    pt_timing = pt_timing->NEXT;
    mbkfree(free_timing);
  }
  mbkfree(pt_timing);
}

/*---------------------------------------------------------------------------*/

void sim_timing_free_all (void)
{
  sim_timing *pt_timing;
  chain_list *chain;

  for (chain = SIM_HEAD_TMG ; chain ; chain = chain->NEXT) {
    pt_timing = (sim_timing*)chain->DATA;
    sim_timing_free (pt_timing);
  }
  if (SIM_HEAD_TMG) {
    freechain (SIM_HEAD_TMG);
    SIM_HEAD_TMG = NULL;
  }
}

/*---------------------------------------------------------------------------*/

void sim_timing_print (void)
{
  sim_timing *timing;
  chain_list *chain;
  char       *root4usr = "NULL"; 
  char       *root = "NULL"; 
    
  for (chain = SIM_HEAD_TMG ; chain ; chain = chain->NEXT) {
    timing = (sim_timing*)chain->DATA;
    while (timing) {
      if (!timing->ROOT4USR)
        root4usr = "NULL";
      else
        root4usr = timing->ROOT4USR;
      if (!timing->ROOT)
        root = "NULL";
      else
        root = timing->ROOT;
      fprintf (stdout,"\n--------------\n");
      fprintf (stdout,"ROOT4USR : %s\n",root4usr);
      fprintf (stdout,"NODE4USR : %s\n",timing->NODE4USR);
      fprintf (stdout,"ROOT : %s\n",root);
      fprintf (stdout,"NODE : %s\n",timing->NODE);
      fprintf (stdout,"DELAY : %g\n",timing->DELAY);
      fprintf (stdout,"SLOPE : %g\n",timing->SLOPE);
      fprintf (stdout,"ROOTEVENT : %c\n",timing->ROOTEVENT);
      fprintf (stdout,"NODEEVENT : %c\n",timing->NODEEVENT);
      fprintf (stdout,"--------------\n");
      timing = timing->NEXT;
    }
  }

}
/*---------------------------------------------------------------------------*/

SIM_FLOAT sim_find_vmax (SIM_FLOAT *tab,int ind1, int ind2 , int *index_vmax)
{
  SIM_FLOAT res = tab[0];
  int       i;

  for (i = ind1 ; i <= ind2 ; i++) {
    if (tab[i] >= res) {
      res = tab[i];
      *index_vmax = i;
    }
  }
  return res;
}

/*---------------------------------------------------------------------------*/

SIM_FLOAT sim_find_vmin (SIM_FLOAT *tab,int ind1, int ind2 , int *index_vmin)
{
  SIM_FLOAT res = tab[0];
  int       i;

  for (i = ind1 ; i <= ind2 ; i++) {
    if (tab[i] <= res) {
      res = tab[i];
      *index_vmin = i;
    }
  }
  return res;
}

/*---------------------------------------------------------------------------*/

char sim_get_last_node_event (sim_model *model,char *name, char type)
{
  SIM_FLOAT time,
    step,
    vddmax,
    vth,
    vthl,
    vthh;
  SIM_FLOAT delay = 1.0;
  SIM_FLOAT slope ;
  SIM_FLOAT *tab1 = NULL;
  int indice;
  sim_measure        *m1 = NULL;
  sim_measure_detail *md1 = NULL;
  char event = 'X'; 

  time = sim_parameter_get_tran_tmax (model);
  step = sim_parameter_get_tran_step (model);
  vddmax = sim_parameter_get_alim (model, 'i');
  // beware: pas de diff entre start & end : a voir
  vth  = sim_parameter_get_delayVTHSTART  (model)*vddmax;
  vthl = sim_parameter_get_slopeVTHL (model)*vddmax;
  vthh = sim_parameter_get_slopeVTHH (model)*vddmax;
    
  m1 = sim_measure_get ( model,
                         name,
                         type, 
                         SIM_MEASURE_VOLTAGE
                         );
  if (m1 != NULL) 
    md1 = sim_measure_detail_scan(m1,md1) ;
  if (md1 != NULL)
    tab1 = sim_measure_detail_get_data( md1 );

  if (!tab1) return event;

  indice = 0;
  while (delay > 0) {
    sim_get_delay_slope (model,tab1,&delay,&slope,&indice,&event,
                         vth,vthl,vthh,time);
  }
  return event;
}

/*---------------------------------------------------------------------------*/

void sim_get_nodes_events (sim_model *model,char *filename,char *node1, 
                           char type1, char *node2, char type2, char type)
{
  SIM_FLOAT time, step, vddmax, vth, vthl, vthh, vthe;
  SIM_FLOAT ic1 = 1.0;
  SIM_FLOAT ic2 = 1.0;
  SIM_FLOAT slope;
  SIM_FLOAT *tab1 = NULL;
  SIM_FLOAT *tab2 = NULL;
  int indice1,indice2,indice3;
  sim_measure        *m1 = NULL;
  sim_measure_detail *md1 = NULL;
  sim_measure        *m2 = NULL;
  sim_measure_detail *md2 = NULL;
  char event1, event2, out_event; 
  int state,i;
  char t1,t2;
  FILE *file;

  if (!(file = fopen (filename,"w"))) {
    fprintf (stderr, "sim_get_nodes_events : can't open file %s\n",filename);
    return;
  }

  time = sim_parameter_get_tran_tmax (model);
  step = sim_parameter_get_tran_step (model);
  vddmax = sim_parameter_get_alim (model, 'i');
  vth  = sim_parameter_get_delayVTHSTART  (model)*vddmax;
  vthe  = sim_parameter_get_delayVTHEND  (model)*vddmax;
  vthl = sim_parameter_get_slopeVTHL (model)*vddmax;
  vthh = sim_parameter_get_slopeVTHH (model)*vddmax;

  t1 = sim_gettype (type1);
  t2 = sim_gettype (type2);
    
  m1 = sim_measure_get ( model,
                         node1,
                         t1, 
                         SIM_MEASURE_VOLTAGE
                         );
  if (m1 != NULL) 
    md1 = sim_measure_detail_scan(m1,md1) ;
  if (md1 != NULL)
    tab1 = sim_measure_detail_get_data( md1 );
  m2 = sim_measure_get ( model,
                         node2,
                         t2, 
                         SIM_MEASURE_VOLTAGE
                         );
  if (m2 != NULL) 
    md2 = sim_measure_detail_scan(m2,md2) ;
  if (md2 != NULL)
    tab2 = sim_measure_detail_get_data( md2 );

  if ((!tab1) || (!tab2)) return;

  if (tab2[2] <= vth)
    state = 0;
  else
    state = 1;
  indice1 = 0;
  while (ic1 > 0) {
    sim_get_delay_slope (model,tab1,&ic1,
                         &slope,&indice1,&event1,
                         vth,vthl,vthh,time);
    if (ic1 <= 0.0) break;
    if (((type == SIM_RISE) && (event1 == SIM_RISE)) ||
        ((type == SIM_FALL) && (event1 == SIM_FALL))) {
      indice2 = indice1;
      indice3 = indice1;
      for (i = 0 ; i <= 1 ; i++) {
        sim_get_delay_slope (model,tab1,&ic2,
                             &slope,&indice3,&event2,
                             vth,vthl,vthh,time);
      }
      if (ic2 <= 0.0)
        indice3 = 0;
      out_event = sim_get_tc_between_2_indices (model, 
                                                tab2,
                                                &ic2, 
                                                &slope,
                                                &indice2,
                                                indice3,
                                                &out_event,
                                                vthe,
                                                vthl,
                                                vthh,
                                                time);
      if (out_event == SIM_RISE) {
        if (state == 0) {
          state = 1;
          fprintf (file,"%d %g\n",state,ic2-ic1);
        }
        else
          fprintf (file,"%d\n",state);
      }
      else if (out_event == SIM_FALL) {
        if (state == 1) {
          state = 0;
          fprintf (file,"%d %g\n",state,ic2-ic1);
        }
        else
          fprintf (file,"%d\n",state);
      }
      else
        fprintf (file,"%d\n",state);
    }
  }
  fclose (file);
}

// fonction ki enleve le nom du top level par rapport au simulateur
char *sim_strip_top_name (sim_model *model,char *name)
{
  char stringhier[BUFSIZ];
  char *str2strip,*str;

  switch( sim_parameter_get_tool( model ) )
    {
    case SIM_TOOL_ELDO: 
    case SIM_TOOL_HSPICE:
    case SIM_TOOL_MSPICE:
    case SIM_TOOL_TITAN:
    case SIM_TOOL_TITAN7:
    case SIM_TOOL_NGSPICE:
      sprintf( stringhier, "x%s.", sim_model_get_lofig( model )->NAME );
      break;
    case SIM_TOOL_LTSPICE:
    case SIM_TOOL_SPICE:
      sprintf( stringhier, "%s:", sim_model_get_lofig( model )->NAME );
      break;
    }
  str2strip = mbkstrdup (stringhier);
  str = strstr (name,str2strip);
  if (str != NULL) 
    str = mbkstrdup (str+strlen(str2strip));
  else
    str = mbkstrdup (name);
  mbkfree (str2strip);

  return str;
}

/*---------------------------------------------------------------------------*\
  sim_get_rc_delay_slope

  Renvoi le delai RC min ou max sur un signal
  \*---------------------------------------------------------------------------*/

int sim_get_rc_delay_slope (sim_model *model, spisig_list *spisig,
                            SIM_FLOAT *delay, SIM_FLOAT *slope,
                            char **nodercin, char **nodercout,
                            char type  // SIM_MIN or SIM_MAX
                            )
{
  char *node;
  char *stripname1;
  char *stripname2;
  sim_measure *m,*m2;
  SIM_FLOAT *tabin =NULL,*tabout = NULL, vddmax;
  SIM_FLOAT  vth,vthl,vthh,time,tcin,tcout,vthe;
  SIM_FLOAT  dref,dmin = 1.0e20,
    dmax = -1.0,
    sref,smin = 1.0e20,
    smax = -1.0;
  char *nodein_min = NULL, *nodein_max = NULL, 
    *nodeout_min = NULL, *nodeout_max = NULL;
  sim_measure_detail *md1 = NULL;
  sim_measure_detail *md2 = NULL;
  int indice,flag_neg, valid = 0;
  locon_list *locon;
  losig_list *losig;

  if (!spisig || !model)
    return  valid;
  node = efg_GetSpiSigName (spisig);

  time = sim_parameter_get_tran_tmax (model);
  vddmax = sim_parameter_get_alim (model, 'i');
  vth  = sim_parameter_get_delayVTHSTART  (model)*vddmax;
  vthe  = sim_parameter_get_delayVTHEND  (model)*vddmax;
  vthl = sim_parameter_get_slopeVTHL (model)*vddmax;
  vthh = sim_parameter_get_slopeVTHH (model)*vddmax;

  sim_find_by_name( node,
                    sim_model_get_lofig (model),
                    &locon,
                    &losig,
                    NULL
                    );

  if( losig ) 
    m = sim_measure_get ( model,
                          node,
                          SIM_MEASURE_SIGNAL,
                          SIM_MEASURE_VOLTAGE
                          );
  if( locon ) 
    m = sim_measure_get ( model,
                          node,
                          SIM_MEASURE_LOCON,
                          SIM_MEASURE_VOLTAGE
                          );
  if (!m)
    return valid;

  while ((md1 = sim_measure_detail_scan(m,md1))) {
    m2  = sim_measure_get ( model,
                            //(char*)(spisig->LOCONRC->DATA),
                            efg_GetSpiSigLoconRc (spisig,'O'),
                            SIM_MEASURE_LOCON,
                            SIM_MEASURE_VOLTAGE
                            );
    while ((md2 = sim_measure_detail_scan(m2,md2))) {
      valid = 1;
      flag_neg = 0;

      stripname1 = sim_strip_top_name (model,sim_measure_detail_get_nodename( md1 ));
      stripname2 = sim_strip_top_name (model,sim_measure_detail_get_nodename( md2 ));
      tabin = sim_measure_detail_get_data( md1 );
      tabout = sim_measure_detail_get_data( md2 );
      if (!tabin) {
        fprintf (stderr, "[SIM WAR] sim_get_rc_delay_slope : no data for node %s !\n",stripname1);
        return valid;
      }
      if (!tabout) {
        fprintf (stderr, "[SIM WAR] sim_get_rc_delay_slope : no data for node %s !\n",stripname2);
        return valid;
      }

      indice = 1;
      sim_get_delay_slope (model,tabin,&tcin,&sref,&indice,NULL,
                           vth,vthl,vthh,time);
      if (tcin <= 0.0) {
        fprintf(stderr, "[SIM WAR] sim_get_rc_delay_slope : no commutation found for %s !\n",stripname1);
        return valid;
      }
      indice = 1;
      sim_get_delay_slope (model,tabout,&tcout,&sref,&indice,NULL,
                           vthe,vthl,vthh,time);
      if (tcout <= 0.0) {
        fprintf(stderr, "[SIM WAR] sim_get_rc_delay_slope : no commutation found for %s !\n",stripname2);
        return valid;
      }
      dref = tcout - tcin;
      if (dref < 0) {
        flag_neg = 1;
        dref = fabs (dref);
      }
      if (dref >= dmax) {
        dmax = dref ;
        if (nodein_max != NULL) mbkfree (nodein_max);
        if (nodeout_max != NULL) mbkfree (nodeout_max);
        if (flag_neg == 0) {
          nodein_max = mbkstrdup (stripname1);
          nodeout_max = mbkstrdup (stripname2);
        }
        else {
          nodein_max = mbkstrdup (stripname2);
          nodeout_max = mbkstrdup (stripname1);
        }
      }
      if (dref <= dmin) {
        dmin = dref ;
        if (nodein_min != NULL) mbkfree (nodein_min);
        if (nodeout_min != NULL) mbkfree (nodeout_min);
        if (flag_neg == 0) {
          nodein_min = mbkstrdup (stripname1);
          nodeout_min = mbkstrdup (stripname2);
        }
        else {
          nodein_min = mbkstrdup (stripname2);
          nodeout_min = mbkstrdup (stripname1);
        }
      }
      if (sref >= smax)
        smax = sref ;
      if (sref <= smax)
        smin = sref ;
      mbkfree (stripname1);
      mbkfree (stripname2);
    }
  }
  if (delay != NULL) {
    if (type == SIM_MIN)
      *delay = dmin;
    else
      *delay = dmax;
  }
  if (slope != NULL) {
    if (type == SIM_MIN)
      *slope = smin;
    else
      *slope = smax;
  }
  if (nodercin != NULL) {
    if (type == SIM_MIN) {
      if (nodein_max != NULL) mbkfree (nodein_max);
      *nodercin = nodein_min;
    }
    else
      if (nodein_min != NULL) mbkfree (nodein_min);
    *nodercin = nodein_max;
  }
  if (nodercout != NULL) {
    if (type == SIM_MIN) {
      if (nodeout_max != NULL) mbkfree (nodein_max);
      *nodercout = nodeout_min;
    }
    else
      if (nodeout_min != NULL) mbkfree (nodeout_min);
    *nodercout = nodeout_max;
  }
  return valid;
}

/*---------------------------------------------------------------------------*\
  sim_get_rc_delay_slope_meas

  Renvoi le delai RC min ou max sur un signal
  \*---------------------------------------------------------------------------*/
#if 0
int sim_get_rc_delay_slope_meas (sim_model *model, spisig_list *spisig,
                                 SIM_FLOAT *ptdelay, SIM_FLOAT *ptslope,
                                 char type  // SIM_MIN or SIM_MAX
                                 )
{
  chain_list *delay_label;
  chain_list *slope_label;
  chain_list *chain;
  char       *label;
  int         start = 1;
  double      delay,slope;
  double      dmax=0.0,dmin=0.0,smax=0.0,smin=0.0;
  int         valid = 0;

  // Delay
  delay_label = sim_get_rc_label_delay ( spisig );
  for ( chain = delay_label ; chain ; chain=chain->NEXT ) {
    label = (char*)chain->DATA;
    if (sim_getMeasDelay(model, label, &delay)) {
      if ( start ) {
        dmin = dmax = delay;
        start = 0;
        valid = 1;
      }
      else {
        if ( delay >= dmax )
          dmax = delay;
        if ( delay <= dmax )
          dmin = delay;
      }
    }
  }
  // Slope
  start=1;
  slope_label = sim_get_rc_label_slope ( spisig );
  for ( chain = slope_label ; chain ; chain=chain->NEXT ) {
    label = (char*)chain->DATA;
    if (sim_getMeasSlope(model, label, &slope)) {
      if ( start ) {
        smin = smax = slope;
        start = 0;
        valid = 1;
      }
      else {
        if ( slope >= smax )
          smax = slope;
        if ( slope <= smax )
          smin = slope;
      }
    }
  }
  if ( type == SIM_MIN ) {
    if ( ptdelay )
      *ptdelay = dmin;
    if ( ptslope )
      *ptslope = smin;
  }
  else {
    if ( ptdelay )
      *ptdelay = dmax;
    if ( ptslope )
      *ptslope = smax;
  }
  return valid;
}
#endif

void sim_dump_delay (sim_model *model, char *name1, char *name2, char *transition, void (*func)(char *start, float starttime, float startslope, char *end, float endtime, float endslope, void *data), void *data)
{
  SIM_FLOAT           vth,vthl,vthh,vthe;
  SIM_FLOAT           tpinit = -1.0, lasttp;
  SIM_FLOAT           tpfinal = -1.0;
  SIM_FLOAT           time, step;
  SIM_FLOAT          *tab1,*tab2;
  SIM_FLOAT           delay, vddmax;
  SIM_FLOAT           dmin = -1.0, dmax = -1.0,slope=0.0, slope_in=0.0;
  sim_measure        *m1,*m2;
  sim_measure_detail *md1 = NULL, *prevmd1, *prevmd2,*md2  = NULL;
  int                 indice;
  char                trans = 'X';
  char                trans1,trans2;
  long                ind1 = 1,ind2 = 1,ind;
  char                t1,t2,delay_ko;
  sim_timing         *tmglist = NULL;
  char               trans_dir[2], type1, type2;
  long               trans_num[2];

  type1=sim_get_type_node(model, name1);
  type2=sim_get_type_node(model, name2);

  sim_getTransition(model, transition, trans_dir, trans_num);
  trans1=toupper(trans_dir[0]); ind1=trans_num[0];
  trans2=toupper(trans_dir[1]); ind2=trans_num[1];
  vddmax = sim_parameter_get_alim (model, 'i');
  vth  = sim_parameter_get_delayVTHSTART  (model)*vddmax;
  vthe  = sim_parameter_get_delayVTHEND  (model)*vddmax;
  vthl = sim_parameter_get_slopeVTHL (model)*vddmax;
  vthh = sim_parameter_get_slopeVTHH (model)*vddmax;

  if ((!name1) || (!name2) || (ind1 <= 0) || (ind2 <= 0) || 
      ((trans1 != 'U') && (trans1 != 'D')) || ((trans2 != 'U') && (trans2 != 'D')))
    return;

  t1 = sim_gettype (type1);
  t2 = sim_gettype (type2);
  if ((t1 == 'X') || (t2 == 'X'))
    return;
  m1 = sim_measure_get ( model,
                         name1,
                         t1, 
                         SIM_MEASURE_VOLTAGE
                         );
  m2 = sim_measure_get ( model,
                         name2,
                         t2,
                         SIM_MEASURE_VOLTAGE
                         );
  if ((!m1) || (!m2))
    return;

  /* Results array */
   
  time = sim_parameter_get_tran_tmax (model);
  step = sim_parameter_get_tran_step (model);
  vddmax = sim_parameter_get_alim (model, 'i');
    
  while ((md1 = sim_measure_detail_scan(m1,md1))) {
    prevmd1 = md1;
    while ((md2 = sim_measure_detail_scan(m2,md2))) {
      prevmd2 = md2;
      delay_ko = 'N';
      /* Compute timings */
      tab1 = sim_measure_detail_get_data( md1 );
      indice = 0;
      ind = 1;
      lasttp=tpinit;
      while (ind <= ind1 ) {
        sim_get_delay_slope (model,tab1,&tpinit,&slope_in,&indice,&trans,
                             vth,vthl,vthh,time);                 
        if (tpinit <= 0.0) {
          tpinit = 0.0;
          /*
            fprintf(stderr, "[SIM WAR]: transition %c %ld for %s not found\n",
            trans1,ind1,name1);
          */
          if (ind1==SIM_MEASURE_LAST && lasttp>0)
            tpinit=lasttp;
          else
            delay_ko = 'Y';
          break;
        }
        if (trans == trans1)
          {
            ind++;
            lasttp=tpinit;
          }
      }
      tab2 = sim_measure_detail_get_data( md2 );
      indice = 0;
      ind = 1;
      lasttp=tpfinal;
      while (ind <= ind2 ) {
        sim_get_delay_slope (model,tab2,&tpfinal,&slope,&indice,&trans,
                             vthe,vthl,vthh,time);
        if (tpfinal <= 0.0) {
          tpfinal = 0.0;
          /*
            fprintf(stderr, "[SIM WAR]: transition %c %ld for %s not found\n",
            trans2,ind2,name2);
          */
          if (ind2==SIM_MEASURE_LAST && lasttp>0)
            tpfinal=lasttp;
          else
            delay_ko = 'Y';
          break;
        }
        if (trans == trans2)
          {
            ind++;
            lasttp=tpfinal;
          }
      }
      if (delay_ko == 'N')
        delay = tpfinal - tpinit;
      else
        delay = 0.0;
      if (dmin < 0.0)
        dmin = delay;
      if (dmax < 0.0)
        dmax = delay;
      if (delay < dmin)
        dmin = delay;
      if (delay > dmax)
        dmax = delay;

      func(md1->NODE_NAME, tpinit, slope_in, md2->NODE_NAME, tpfinal, slope, data);
    }
  }

  // Fill timing structure
  // on met a jour le delay s'il existait
  if (delay_ko == 'N') {
    if ((tmglist = sim_timing_get_by_event (name1,name2,transition)) == NULL) {
      tmglist  = sim_timing_add (tmglist,name1,name2);
      sim_timing_setnodesname (tmglist,sim_measure_detail_get_name (prevmd1),
                               sim_measure_detail_get_name (prevmd2));
      sim_timing_setdelay (tmglist,delay);
      sim_timing_setslope (tmglist,slope);
      sim_timing_setrootevent (tmglist,trans1);
      sim_timing_setnodeevent (tmglist,trans2);
      sim_timing_setrooteventidx (tmglist,ind1);
      sim_timing_setnodeeventidx (tmglist,ind2);
      SIM_HEAD_TMG = addchain (SIM_HEAD_TMG,tmglist);
    }
    else 
      sim_timing_setdelay (tmglist,delay);
  }
}

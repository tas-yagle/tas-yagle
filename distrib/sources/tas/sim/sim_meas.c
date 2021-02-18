/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : AVERTEC global tools                                        */
/*    Fichier : sim_meas.c                                                  */
/*                                                                          */
/*    © copyright 2000 AVERTEC                                              */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Antony PINTO                                              */
/*                                                                          */
/****************************************************************************/

#include "sim_meas.h"

#ifndef __func__
#define __func__ __FILE__
#endif

sim_meas_cmd     SIM_MEAS;
static int set_SIM_MEAS=0;

static void printMeasCmd_int(FILE *fd, sim_meas_arg *arg);
static int existMeasArg(sim_model *model, sim_meas_arg **arg, char *label);
static void freeMeasArg_int(sim_meas_arg *arg);
static sim_meas_print *allocprn(sim_model *model, char *delay, char *sig1, char *sig2, char sigt[2], char *transition, int delay_type, SIM_FLOAT th_in, SIM_FLOAT th_out);
static void addMeas(sim_model *model, char *delay, char *slope, char *sig1, char *sig2, char *transition, int check, int delay_type);
static sim_meas_print *freePrn(sim_meas_print *prn);

/****************************************************************************/
/*{{{                    External functions                                 */
/*{{{                    sim_decodeMeasCmd()                                */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void sim_decodeMeasCmd(char *env)
{
  char           buf[2048], minibuf[256];
  int            i, j, k, l,dep, type, ind, envlen;

  envlen    = (int)strlen(env);
  memset(&SIM_MEAS,0,sizeof(SIM_MEAS));
  set_SIM_MEAS=1;
  
  for (i = 0, j = 0, dep = 0; i < envlen; i++)
  {
    buf[j++]      = env[i];
    if (env[i] == '%')
    {
      type    = 0;
      ind     = 0;
      switch (env[++i])
      {
        case 'n' :
             type ++;
        case 't' :
             type ++;
        case 'v' :
             type ++;
        case 's' :
             type ++;
             switch (env[++i])
             {
               case '2' :
                    ind ++;
               case '1' :
                    SIM_MEAS.ARGORDER[dep]    = type + SIM_MEAS_A4S*ind;
                    if (type == SIM_MEAS_TRANS)
                    {
                      if (env[++i] == '{')
                      {
                        k     = 0;
                        l     = 0;
                        while (l < 2)
                        {
                          if (env[++i] != ' ')
                            minibuf[k]    = env[i];
                          if (minibuf[k] == ',' || minibuf[k] == '}')
                          {
                            minibuf[k]    = '\0';
                            SIM_MEAS.TRANS[ind][l++] =
                              mbkstrdup(minibuf);
                            k             = -1;
                          }
                          k ++;
                          if (i > envlen)
                            sim_err("Measure command malformed");
                        }
                      }
                      else
                        sim_war("Token %%%c must have a list of data",
                                env[i-2]);
                    }
                    else if (type == SIM_MEAS_NTRANS)
                    {
                      if (env[++i] == '{')
                      {
                        k     = 0;
                        while (env[++i] != '}')
                          if (env[i] != ' ')
                            minibuf[k++]  = env[i];
                          else if (i > envlen)
                            sim_err("Measure command malformed");
                        minibuf[k]    = '\0';
                        SIM_MEAS.LAST[ind] =
                          mbkstrdup(minibuf);
                      }
                      else
                        sim_war("Token %%%c must have a data",env[i-2]);
                    }
                    break;
               default :
                    sim_war("Token %%%c must have an index (1,2)",env[i-1]);
             }
             break;
        case 'l' :
             SIM_MEAS.ARGORDER[dep]   = SIM_MEAS_LABEL;
             break;
        default :
             sim_war("Unknown token : %%%c",env[i]);
      }
      buf[j++]          = 's';
      dep ++;
    }
  }
  buf[j++]              = '\n';
  buf[j]                = '\0';
  SIM_MEAS.CMDSTR   = mbkstrdup(buf);
}

void sim_decodeMeasCmdFree()
{
  int            i, j;
  if (!set_SIM_MEAS) return;

  for (i=0; i<SIM_MEAS_NBSIG; i++)
    {
      if (SIM_MEAS.LAST[i]!=NULL) mbkfree(SIM_MEAS.LAST[i]);
      for (j=0; j<2; j++)
       if (SIM_MEAS.TRANS[i][j]!=NULL) mbkfree(SIM_MEAS.TRANS[i][j]);
      
    }
  if (SIM_MEAS.CMDSTR!=NULL) mbkfree(SIM_MEAS.CMDSTR);
}

/*}}}************************************************************************/
/*{{{                    sim_affectMeasCmdArg()                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void sim_affectMeasCmdArg(sim_model *model, char *label,
                          char *sig1, char trans1, int ntrans1, double val1,
                          char *sig2, char trans2, int ntrans2, double val2)
{
  sim_meas_arg  *arg;
  int            i;
  long           ntrans[SIM_MEAS_NBSIG] = { ntrans1, ntrans2 };
  double         val[SIM_MEAS_NBSIG]    = { val1   , val2    };
  char           trans[SIM_MEAS_NBSIG]  = { trans1 , trans2  };
  char          *sig[SIM_MEAS_NBSIG]    = { sig1   , sig2    };
  char           buf[256];
  
  if (existMeasArg(model,&arg,label))
    freeMeasArg_int(arg);
  else
  {
    arg                 = mbkalloc(sizeof(sim_meas_arg));
    arg->NEXT           = model->LARGS;
    model->LARGS        = arg;
  }
  
  arg->REFCMD               = &SIM_MEAS;
  arg->ARG[SIM_MEAS_LABEL]  = label;
  
  for (i = 0; i < SIM_MEAS_NBSIG; i ++)
  {
    /* signal name */
    arg->ARG[SIM_MEAS_SIG + SIM_MEAS_A4S*i]     = mbkstrdup(sig[i]);
    
    /* transition type */
    if (trans[i] == 'U' || trans[i] == 'u')
      arg->ARG[SIM_MEAS_TRANS + SIM_MEAS_A4S*i] =
        SIM_MEAS.TRANS[i][SIM_MEAS_UP];
    else if (trans[i] == 'D' || trans[i] == 'd')
      arg->ARG[SIM_MEAS_TRANS + SIM_MEAS_A4S*i] =
        SIM_MEAS.TRANS[i][SIM_MEAS_DN];
    else
    {
      sim_war("Unknown type of transition");
      arg->ARG[SIM_MEAS_TRANS + SIM_MEAS_A4S*i] = NULL;
    }
    
    /* threshold value */
    sprintf(buf,"%g",val[i]);
    arg->ARG[SIM_MEAS_VALUE + SIM_MEAS_A4S*i]   = mbkstrdup(buf);
    
    /* number of transition */
    if (ntrans[i] != SIM_MEASURE_LAST)
      sprintf(buf,"%ld",ntrans[i]);
    else
      sprintf(buf,"%s",SIM_MEAS.LAST[i]);
    
    arg->ARG[SIM_MEAS_NTRANS + SIM_MEAS_A4S*i]  = mbkstrdup(buf);
  }
}

/*}}}************************************************************************/
/*{{{                    sim_freeMeasArg()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void sim_freeMeasArg(sim_model *model, char *label)
{
  sim_meas_arg  *arg, *tmp;
 
  for (arg = (sim_meas_arg *)&(model->LARGS); arg->NEXT; arg = arg->NEXT)
    if (arg->NEXT->ARG[SIM_MEAS_LABEL] == label)
    {
      freeMeasArg_int(arg->NEXT);
      tmp       = arg->NEXT;
      arg->NEXT = arg->NEXT->NEXT;
      mbkfree(tmp);
      break;
    }
}

/*}}}************************************************************************/
/*{{{                    sim_freeMeasAllArg()                               */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void sim_freeMeasAllArg(sim_model *model)
{
  sim_meas_arg  *arg, *tmp;
 
  for (arg = (sim_meas_arg *)&(model->LARGS); arg->NEXT; )
  {
    freeMeasArg_int(arg->NEXT);
    tmp         = arg->NEXT;
    arg->NEXT   = arg->NEXT->NEXT;
    mbkfree(tmp);
  }
}

/*}}}************************************************************************/
/*{{{                    sim_freePrn()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void sim_freePrn(sim_model *model)
{
  model->LPRN   = freePrn(model->LPRN);
}

/*}}}************************************************************************/
/*{{{                    sim_addMeas()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void sim_addMeas(sim_model *model, char *delay, char *slope,
                 char *sig1, char *sig2, char *transition, int delay_type)
{
  addMeas(model,delay,slope,sig1,sig2,transition,1,delay_type);
}

/*}}}************************************************************************/
/*{{{                    sim_addMeasDelay()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void sim_addMeasDelay(sim_model *model, char *delay,
                      char *sig1, char *sig2, char *transition, int delay_type)
{
  addMeas(model,delay,NULL,sig1,sig2,transition,1,delay_type);
}

/*}}}************************************************************************/
/*{{{                    sim_addMeasSlope()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void sim_addMeasSlope(sim_model *model, char *slope, char *sig,
                      char *transition, int delay_type)
{
  addMeas(model,NULL,slope,NULL,sig,transition,1,delay_type);
}

/*}}}************************************************************************/
/*{{{                    sim_addMeasWithoutNameCheck()                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void sim_addMeasWithoutNameCheck(sim_model *model, char *delay, char *slope,
                                 char *sig1, char *sig2, char *transition, int delay_type)
{
  addMeas(model,delay,slope,sig1,sig2,transition,0,delay_type);
}

/*}}}************************************************************************/
/*{{{                    sim_addMeasDelayWithoutNameCheck()                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void sim_addMeasDelayWithoutNameCheck(sim_model *model, char *delay,
                                      char *sig1, char *sig2,
                                      char *transition, int delay_type)
{
  addMeas(model,delay,NULL,sig1,sig2,transition,0,delay_type);
}

/*}}}************************************************************************/
/*{{{                    sim_addMeasSlopeWithoutNameCheck()                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void sim_addMeasSlopeWithoutNameCheck(sim_model *model, char *slope,
                                      char *sig, char *transition, int delay_type)
{
  addMeas(model,NULL,slope,NULL,sig,transition,0,delay_type);
}

/*}}}************************************************************************/
/*{{{                    sim_getMeas()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
//int sim_getMeas(sim_model *model, char *label, double *value)
//{
//  return !(mfe_labelvalue(model->LLABELS,namealloc(label),value));
//}

int sim_getMcMeas( sim_model *model, char *label, double *value, int idmc )
{
  int res = 0;

  if( V_BOOL_TAB[__SIM_USE_MEAS].VALUE ) {

    res = mfe_labelvalue(model->LLABELS,label,value,idmc);
    
    return !res;
  }
  else if (V_BOOL_TAB[__SIM_USE_PRINT].VALUE) {
  
  }

  return 0 ;
}

/*}}}************************************************************************/
/*{{{                    sim_getMeasSlope()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int sim_getMeasSlope(sim_model *model, char *label, double *value)
{
  return sim_getMeas(model,NULL,label,NULL,value);
}

/*}}}************************************************************************/
/*{{{                    sim_getMeasDelay()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int sim_getMeasDelay(sim_model *model, char *label, double *value)
{
  return sim_getMeas(model,label,NULL,value,NULL);
}

/*}}}************************************************************************/
/*{{{                    sim_getMeas()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int sim_getMeas(sim_model *model, char *delay, char *slope,
                double *dval, double *sval)
{
  if (V_BOOL_TAB[__SIM_USE_MEAS].VALUE)
  {
    int          res = 0;

    if (delay)
      res      += mfe_labelvalue(model->LLABELS,delay,dval,-1);
    if (slope)
      res      += mfe_labelvalue(model->LLABELS,slope,sval,-1);
    
    return !res;
  }
  else if (V_BOOL_TAB[__SIM_USE_PRINT].VALUE)
  {
    sim_meas_print  *prnx;
    int              done = 0;

    /* read output file */
    sim_parse_spiceout(model);    
    
    if (!delay)
      done ++;
    else
      delay     = min_namealloc(delay);
    if (!slope)
      done ++;
    else
      slope     = min_namealloc(slope);

    for (prnx = model->LPRN; prnx && done < 2; prnx = prnx->NEXT)
      if (prnx->LABEL == delay)
      {
        *dval   = sim_get_trans_delay(model,prnx->DELAY_TYPE,
                                      prnx->SIGTYPE[0],prnx->SIGNAME[0],
                                      prnx->SIGTYPE[1],prnx->SIGNAME[1],
                                      prnx->TRANSITION, prnx->TH_IN, prnx->TH_OUT);
        done ++;
      }
      else if (prnx->LABEL == slope)
      {
        *sval   = sim_get_trans_slope(model,prnx->DELAY_TYPE,
                                      prnx->SIGTYPE[1],prnx->SIGNAME[1],
                                      prnx->TRANSITION, prnx->TH_IN, prnx->TH_OUT);
        done ++;
      }
    
    return (done < 2) ? 0 : 1;
  }
  else
    return 0;
}

/*}}}************************************************************************/
/*{{{                    sim_printMeasCmd()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void sim_printMeasCmd(sim_model *model, FILE *fd)
{
  printMeasCmd_int(fd,(sim_meas_arg *)(model->LARGS));
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Internal functions                                 */
/****************************************************************************/
/*{{{                    getNodeName()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline int getNodeName(sim_model *model, char *signame,
                              char **nodename, char *type)
{
  locon_list    *con;
  losig_list    *sig;
  chain_list    *inslist;
  int            res = 1, node;
  char           buf[1024];
  
  sim_find_by_name(signame,model->FIG,&con,&sig,&inslist);
  
  if (con)
  {
    *type       = SIM_IC_LOCON;
    if (con->PNODE)
      node      = con->PNODE->DATA;
    else
      node      = sim_run_simu_spice_get_one_node(con->SIG);
    /* implicit declaration cause it don't have to be there */
    /* it should be in sim_drive_spice.c                    */
    sim_run_simu_spice_name_signal(model,inslist,con->SIG,node,0,buf);
    *nodename   = mbkstrdup(buf);
  }
  else if (sig)
  {
    *type       = SIM_IC_SIGNAL;
    node        = sim_run_simu_spice_get_one_node(sig);
    sim_run_simu_spice_name_signal(model,inslist,sig,node,0,buf);
    *nodename   = mbkstrdup(buf);
  }
  else
  {
    fprintf(stderr,"[SIM_WAR] : Problem occured in creating name for %s\n",
            signame);
    res     =  0;
  }
  
  return res;
}

/*}}}************************************************************************/
/*{{{                    addMeas()                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void addMeas(sim_model *model, char *delay, char *slope,
                    char *sig1, char *sig2, char *transition, int check, int delay_type)
{
  long           ntrans[2];
  double         val[2], alim, th_in, th_out, slopelow, slopehigh;
  char           trans[2], *labelname, *sig[2] = {sig1, sig2}, *signame;
  char           sigt[2], buf[2];
  int            i;
  
  if (!model)
    return;
  else
  {
    alim            = sim_parameter_get_alim(model, 'i');
    sim_get_THR(model, &th_in, &th_out, &slopelow, &slopehigh);
    /* make signal name */
    if (check)
    {
      for (i = 0; i < 2; i ++)
        if (sig[i])
        {
          signame   = sig[i];
          
          if (!getNodeName(model,signame,&sig[i],&sigt[i]))
          {
            if (i==1) mbkfree(sig[0]);
            return;
          }
        }
    }
    else
      for (i = 0; i < 2; i ++)
        sig[i]      = (sig[i]) ? mbkstrdup(sig[i]) : NULL;
    
    sim_getTransition(model,transition,trans,ntrans);

    /* add print */
    if (V_BOOL_TAB[__SIM_USE_PRINT].VALUE)
    {
      sim_meas_print    *prn;

      if (delay)
      {
        labelname   = min_namealloc(delay);
        prn         = allocprn(model,labelname,sig1,sig2,sigt,transition,delay_type, th_in, th_out);
        switch (sigt[0])
        {
          case SIM_IC_SIGNAL :
               sim_measure_set_signal(model,sig1);
               break;
          case SIM_IC_LOCON :
               sim_measure_set_locon(model,sig1);
               break;
        }
      }
      if (slope)
      {
        labelname   = min_namealloc(slope);
        if (!delay)
          prn       = allocprn(model,labelname,sig2,sig2,sigt,transition,delay_type, slopelow, slopehigh);
        else
        {
          if (ntrans[1] == SIM_MEASURE_LAST)
            sprintf(buf,"%cL",trans[1]);
          else
            sprintf(buf,"%c%ld",trans[1],ntrans[1]);
          prn       = allocprn(model,labelname,sig2,sig2,sigt,buf,delay_type,slopelow, slopehigh);
        }
      }
      switch (sigt[1])
      {
        case SIM_IC_SIGNAL :
             sim_measure_set_signal(model,sig2);
             break;
        case SIM_IC_LOCON :
             sim_measure_set_locon(model,sig2);
             break;
      }
    }
    
    /* add measure */
    if (V_BOOL_TAB[__SIM_USE_MEAS].VALUE)
    {
      if (!SIM_MEAS.CMDSTR)
      {
        fprintf(stderr,"[SIM_ERR] : Need a configuration of drive"
                " rules : simmeascmd\n");
        mbkexit(1);
      }
      else
      {
        if (delay)
        {
          labelname     = min_namealloc(delay);
          val[0]        = th_in;
          val[1]        = th_out;
          sim_affectMeasCmdArg(model,labelname,
                               sig[0],trans[0],ntrans[0],val[0],
                               sig[1],trans[1],ntrans[1],val[1]);
          mfe_addlabel(&(model->LLABELS),labelname,model->NBMC);
        }
        if (slope)
        {
          labelname     = min_namealloc(slope);
          if (!delay)
/*            sig[1]      = mbkstrdup(sig[1]);
          else*/
          {
            trans[1]    = trans[0];
            ntrans[1]   = ntrans[0];
          }
          
          i             = (trans[1] == 'U' || trans[1] == 'u') ? 0 : 1;
          val[i%2]      = slopelow;
          val[(i+1)%2]  = slopehigh;
          
          sim_affectMeasCmdArg(model,labelname,
                               sig[1],trans[1],ntrans[1],val[0],
                               sig[1],trans[1],ntrans[1],val[1]);
          mfe_addlabel(&(model->LLABELS),labelname,model->NBMC);
        }
      }
      for (i=0; i<2; i++)
        if (sig[i]!=NULL) mbkfree(sig[i]);
    }
  }
}

/*}}}************************************************************************/
/*{{{                    sim_getTransition()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int sim_getTransition(sim_model *model, char *trans, char t[2], long n[2])
{
  int           i;
  char          *pt;
   
  for (i = 0, pt = trans; i < 2; i ++)
  {
    t[i]        = pt[0];
    if (pt[1] == 'L' || pt[1] == 'l')
    {
      n[i]      = SIM_MEASURE_LAST;
      pt        = &(pt[2]);
    }
    else if (!(n[i] = strtol(&(pt[1]),&pt,10)))
      n[i]      = 1;
    if (!pt[0])
      break;
  }

  switch (sim_parameter_get_tool(model))
  {
  }
  
  return i + 1;
}

/*}}}************************************************************************/
/*{{{                    printMeasCmd_int()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void printMeasCmd_int(FILE *fd, sim_meas_arg *arg)
{
  if (arg)
  {
    printMeasCmd_int(fd,arg->NEXT);
    spi_print(fd,arg->REFCMD->CMDSTR,
              arg->ARG[SIM_MEAS.ARGORDER[0]],
              arg->ARG[SIM_MEAS.ARGORDER[1]],
              arg->ARG[SIM_MEAS.ARGORDER[2]],
              arg->ARG[SIM_MEAS.ARGORDER[3]],
              arg->ARG[SIM_MEAS.ARGORDER[4]],
              arg->ARG[SIM_MEAS.ARGORDER[5]],
              arg->ARG[SIM_MEAS.ARGORDER[6]],
              arg->ARG[SIM_MEAS.ARGORDER[7]],
              arg->ARG[SIM_MEAS.ARGORDER[8]]);
  }
}

/*}}}************************************************************************/
/*{{{                    existMeasArg()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int existMeasArg(sim_model *model, sim_meas_arg **arg, char *label)
{
  for (*arg = model->LARGS; *arg; *arg = (*arg)->NEXT)
    if ((*arg)->ARG[SIM_MEAS_LABEL] == label)
      break;

  return ((*arg) != NULL);
}

/*}}}************************************************************************/
/*{{{                    freeMeasArg_int()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void freeMeasArg_int(sim_meas_arg *arg)
{
  int i;
    for (i = 0; i < SIM_MEAS_NBSIG; i ++)
    {
      if (arg->ARG[SIM_MEAS_VALUE + SIM_MEAS_A4S*i]!=NULL) mbkfree(arg->ARG[SIM_MEAS_VALUE + SIM_MEAS_A4S*i]);
    }
    for (i = 0; i < SIM_MEAS_NBSIG; i ++)
    {
      if (arg->ARG[SIM_MEAS_NTRANS + SIM_MEAS_A4S*i]!=NULL) mbkfree(arg->ARG[SIM_MEAS_NTRANS + SIM_MEAS_A4S*i]);
    }
//    mbkfree(arg->ARG[SIM_MEAS_NTRANS + SIM_MEAS_A4S]);
/*  mbkfree(arg->ARG[SIM_MEAS_VALUE]);
  mbkfree(arg->ARG[SIM_MEAS_VALUE + SIM_MEAS_A4S]);*/
  
  if (arg->ARG[SIM_MEAS_SIG + SIM_MEAS_A4S] == arg->ARG[SIM_MEAS_SIG])
  {
    /* case of slope */
    //mbkfree(arg->ARG[SIM_MEAS_NTRANS]);
    mbkfree(arg->ARG[SIM_MEAS_SIG]);
  }
  else
  {
    /* case of delay */
//    mbkfree(arg->ARG[SIM_MEAS_NTRANS]);
    mbkfree(arg->ARG[SIM_MEAS_SIG]);
    mbkfree(arg->ARG[SIM_MEAS_SIG + SIM_MEAS_A4S]);
  }
}

/*}}}************************************************************************/
/*{{{                    allocprn()                                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static sim_meas_print *allocprn(sim_model *model, char *label, char *sig1,
                                char *sig2, char sigt[2], char *transition, int delay_type, SIM_FLOAT th_in, SIM_FLOAT th_out)
{
  sim_meas_print    *prn;

  prn               = mbkalloc(sizeof(sim_meas_print));
  
  prn->NEXT         = model->LPRN;
  prn->LABEL        = label;
  prn->SIGNAME[0]   = sig1;
  prn->SIGNAME[1]   = sig2;
  prn->SIGTYPE[0]   = sigt[0];
  prn->SIGTYPE[1]   = sigt[1];
  prn->TRANSITION   = mbkstrdup(transition);
  prn->DELAY_TYPE   = delay_type;
  prn->TH_IN=th_in;
  prn->TH_OUT=th_out;
  model->LPRN       = prn;

  return prn;
}

/*}}}************************************************************************/
/*{{{                    freePrn()                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static sim_meas_print *freePrn(sim_meas_print *prn)
{
  if (prn)
  {
    sim_meas_print  *res;

    res     = freePrn(prn->NEXT);
    mbkfree(prn->TRANSITION);
    mbkfree(prn);

    return res;
  }
  return NULL;
}

/*}}}************************************************************************/
/*}}}************************************************************************/
#if 0
/*{{{        Test function for sim_meas                                     */
/*                                                                          */
/* /usr/local/bin/gcc -o main -L/users/disk01/antony/dev/Solaris/lib -Xlinker -z -Xlinker allextract -lSim100 -lTut100 -lSim100 -lGsp100 -lEfg100 -lCbh100 -lEqt100 -lTrc102 -lTlc100 -lApi100 -lInf112 -lCns211 -lElp110 -lAvt100 -lMlu501 -lMcl413 -lMal100 -lMsl700 -lMgl100 -lMvl409 -lMel407 -lMhl403 -lMlo501 -lSpf100 -lSpe100 -lBef100 -lBgl100 -lBvl202 -lBeg100 -lBhl110 -lBeh110 -llog202 -lRcn200 -lMut325 -lstdc++ -lnsl -ldl -lm -Xlinker -z -Xlinker defaultextract */
/*                                                                          */
/****************************************************************************/
int main()
{
  sim_model model[4];
  
  memset(model,0,sizeof(sim_model) * 4);
  
  sim_decodeMeasCmd(".meas tran %l"
    " TRIG v(%s1) val=%v1 %t1{rise1,fall1}=%n1{LAST1}"
    " TARG v(%s2) val=%v2 %t2{rise2,fall2}=%n2{LAST2}");
  sim_affectMeasCmdArg(&model[0],"test1","a",'U',15,1.5,"b",'D',16,1.6);
  printf("-**** test 1 ****-\n");
  sim_printMeasCmd(&model[0],stdout);
  
  sim_decodeMeasCmd(".measure tran %l"
    " TRIG v(%s1) TARG v(%s2)"
    " val=%v1 val=%v2"
    " %t1{rise1,fall1}=%n5{LAST1} %y2{rise2,fall2}=%n2{LAST2}");
  sim_affectMeasCmdArg(&model[1],"test2","a",'U',15,1.5,"b",'D',16,1.6);
  printf("-**** test 2 ****-\n");
  sim_printMeasCmd(&model[1],stdout);
  
  sim_decodeMeasCmd(".measure tran"
    " val=%v1 val=%v2"
    " TRIG v(%s1) TARG v(%s2)"
    " %t1{rise1,fall1}=%n1{LAST1} %t2{rise2,fall2}=%n2{LAST2} %l");
  sim_affectMeasCmdArg(&model[2],"test3","a",'D',SIM_MEASURE_LAST,1.5,"b",'D',25,1.6);
  printf("-**** test 3 ****-\n");
  sim_printMeasCmd(&model[2],stdout);
  
  sim_decodeMeasCmd(".meas tran %l"
    " TRIG v(%s1) val=%v1 %t1{rise1,fall1}=%n1{LAST1}"
    " TARG v(%s2) val=%v2 %t2{rise2,fall2}=%n2{LAST2}");
  sim_affectMeasCmdArg(&model[3],"test4","a",'U',1      ,1.5,"b",'D',16,1.6);
  sim_affectMeasCmdArg(&model[3],"test5","b",'U',SIM_MEASURE_LAST,1.5,"c",'D',17,1.7);
  sim_affectMeasCmdArg(&model[3],"test6","a",'D',1      ,1.5,"b",'U',18,1.4);
  sim_affectMeasCmdArg(&model[3],"test7","a",'D',1      ,1.5,"c",'D',19,1.5);
  sim_freeMeasArg(&model[3],"test4");
  sim_freeMeasAllArg(&model[3]);
  printf("-**** test 4 ****-\n");
  sim_printMeasCmd(&model[3],stdout);
  sim_printMeasCmd(&model[2],stdout);
  
  return 0;
}

/*}}}************************************************************************/
#endif

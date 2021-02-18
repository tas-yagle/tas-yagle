#include AVT_H
#include MUT_H
#include MLO_H
#include "trc.h"

#include "trc_timings.h"


int rcx_has_rc_timings(losig_list *ls)
{
  if (getptype(ls->USER, RCX_RC_TIMINGS_PTYPE)!=NULL) return 1;
  return 0;
}

int rcx_set_connector_origin_group(locon_list *lc, int number)
{
  ptype_list *p;
  char buf[1024];

  if ((p=getptype(lc->SIG->USER, RCX_RC_TIMINGS_PTYPE))!=NULL)
    {
      avt_errmsg( TRC_ERRMSG, "017", AVT_ERROR, mbk_putconname(lc, buf) );
      return 1;
    }

  if ((p=getptype(lc->USER, RCX_ORIGIN_GROUP_PTYPE))==NULL)
    p=lc->USER=addptype(lc->USER, RCX_ORIGIN_GROUP_PTYPE, NULL);
  p->DATA=(void *)(long)number;      
  return 0;
}

int rcx_set_connector_destination_group(locon_list *lc, int number)
{
  ptype_list *p;
  if ((p=getptype(lc->SIG->USER, RCX_RC_TIMINGS_PTYPE))!=NULL)
    {
      return 1;
    }

  if ((p=getptype(lc->USER, RCX_DESTINATION_GROUP_PTYPE))==NULL)
    p=lc->USER=addptype(lc->USER, RCX_DESTINATION_GROUP_PTYPE, NULL);
  p->DATA=(void *)(long)number;      
  return 0;
}

int rcx_get_connector_origin_group(locon_list *lc)
{
  ptype_list *p;
  if ((p=getptype(lc->USER, RCX_ORIGIN_GROUP_PTYPE))==NULL)
    {
      return -1;
    }
  return (int)(long)p->DATA;
}

int rcx_get_connector_destination_group(locon_list *lc)
{
  ptype_list *p;
  if ((p=getptype(lc->USER, RCX_DESTINATION_GROUP_PTYPE))==NULL)
    {
      return -1;
    }
  return (int)(long)p->DATA;
}

rcx_timings **rcx_get_rc_timings(locon_list *origin, locon_list *destination, char dir)
{
  ptype_list *p;
  int orig_index, dest_index;
  rcx_delays *delays;
  char buf[1024];
  if ((p=getptype(origin->SIG->USER, RCX_RC_TIMINGS_PTYPE))==NULL)
    {
      avt_errmsg( TRC_ERRMSG, "018", AVT_ERROR );
      return NULL;
    }
  orig_index=rcx_get_connector_origin_group(origin);
  if (orig_index==-1) return NULL;
  dest_index=rcx_get_connector_destination_group(destination);
  if (dest_index==-1) return NULL;
  delays=(rcx_delays *)p->DATA;
  if (orig_index<0 || orig_index>=delays->nb_origins)
    {
      if (orig_index==-1)
        avt_errmsg( TRC_ERRMSG, "019", AVT_ERROR, mbk_putconname(origin, buf) );
      else
        avt_errmsg( TRC_ERRMSG, "020", AVT_ERROR, mbk_putconname(origin, buf) );
      return NULL;
    }
  if (dest_index<0 || dest_index>=delays->nb_destinations)
    {
      if (dest_index==-1)
        avt_errmsg( TRC_ERRMSG, "021", AVT_ERROR, mbk_putconname(destination, buf) );
      else
        avt_errmsg( TRC_ERRMSG, "022", AVT_ERROR, mbk_putconname(destination, buf) );
      return NULL;
    }
  if (dir=='u')
    return &delays->origin_destination_array_UU[orig_index*delays->nb_destinations+orig_index];
  else
    return &delays->origin_destination_array_DD[orig_index*delays->nb_destinations+orig_index];
}

rcx_timings *rcx_get_computed_data(rcx_timings *mytimings, float input_slope, float *computed_delay, float *computed_slope)
{
  rcx_timings *rct;

  for( rct = mytimings ; rct ; rct = rct->NEXT ) {
    if( rct->input_slope > 1.0 ) {
      if( fabs( ( input_slope - rct->input_slope ) / rct->input_slope ) < 0.01 )
        break ;
    }
    else {
      if( input_slope < 1.0 )
        break ;
    }
  }

  if (rct==NULL) return NULL;
  *computed_delay=rct->computed_delay;
  *computed_slope=rct->computed_slope;
  return rct;
}

void rcx_add_rc_timing(locon_list *origin, locon_list *destination, float input_slope, float computed_delay, float computed_slope, char dir)
{
  rcx_timings *rct, **head;
  float a, b;
  head=rcx_get_rc_timings(origin, destination, dir);
  rct=rcx_get_computed_data(head?*head:NULL, input_slope, &a, &b);
  if (rct==NULL)
    {
      rct=(rcx_timings *)mbkalloc(sizeof(rcx_timings));
      rct->input_slope=input_slope;
      rct->NEXT=*head;
      *head=rct;
    }
  rct->computed_delay=computed_delay;
  rct->computed_slope=computed_slope;
}

void rcx_free_signal_rc_timings(losig_list *ls)
{
  ptype_list *p;
  rcx_delays *delays;
  rcx_timings *rct, *next;
  int i;
  chain_list *cl;
  locon_list *lc;

  if ((p=getptype(ls->USER, RCX_RC_TIMINGS_PTYPE))==NULL)
    {
      avt_errmsg( TRC_ERRMSG, "023", AVT_ERROR );
      return;
    }
  delays=(rcx_delays *)p->DATA;
  for (i=0; i<delays->nb_destinations*delays->nb_origins; i++)
    {
      for (rct=delays->origin_destination_array_UU[i], next=rct->NEXT; rct!=NULL; rct=next)
        mbkfree(rct);
      for (rct=delays->origin_destination_array_DD[i], next=rct->NEXT; rct!=NULL; rct=next)
        mbkfree(rct);
    }
  mbkfree(delays->origin_destination_array_UU);
  mbkfree(delays->origin_destination_array_DD);
  mbkfree(delays);
  lc->SIG->USER=delptype(lc->SIG->USER, RCX_RC_TIMINGS_PTYPE);
  cl=(chain_list *)getptype(lc->SIG->USER, LOFIGCHAIN)->DATA;
  while (cl!=NULL)
    {
      lc=(locon_list *)cl->DATA;
      if (getptype(lc->USER, RCX_ORIGIN_GROUP_PTYPE)!=NULL)
        lc->USER=delptype(lc->USER, RCX_ORIGIN_GROUP_PTYPE);
      if (getptype(lc->USER, RCX_DESTINATION_GROUP_PTYPE)!=NULL)
        lc->USER=delptype(lc->USER, RCX_DESTINATION_GROUP_PTYPE);
      cl=cl->NEXT;
    }
}

void rcx_create_signal_rc_timings(losig_list *ls)
{
  ptype_list *p;
  rcx_delays *delays;
  int i, maxo=-1, maxd=-1;
  chain_list *cl;
  locon_list *lc;
  
  if ((p=getptype(ls->USER, RCX_RC_TIMINGS_PTYPE))!=NULL)
    {
      avt_errmsg( TRC_ERRMSG, "024", AVT_ERROR );
      return;
    }

  cl=(chain_list *)getptype(ls->USER, LOFIGCHAIN)->DATA;
  while (cl!=NULL)
    {
      lc=(locon_list *)cl->DATA;
      if ((i=rcx_get_connector_origin_group(lc))>maxo) maxo=i;
      if ((i=rcx_get_connector_destination_group(lc))>maxd) maxd=i;
      cl=cl->NEXT;
    }
  
  if (maxo==-1 || maxd==-1)
    {
      avt_errmsg( TRC_ERRMSG, "025", AVT_ERROR );
      return;
    }

  delays=(rcx_delays *)mbkalloc(sizeof(rcx_delays));
  delays->nb_destinations=maxd+1;
  delays->nb_origins=maxo+1;
  delays->origin_destination_array_UU=(rcx_timings **)mbkalloc(sizeof(rcx_timings *)*delays->nb_destinations*delays->nb_origins);
  delays->origin_destination_array_DD=(rcx_timings **)mbkalloc(sizeof(rcx_timings *)*delays->nb_destinations*delays->nb_origins);
  for (i=0; i<delays->nb_destinations*delays->nb_origins; i++)
    {
      delays->origin_destination_array_UU[i]=NULL;
      delays->origin_destination_array_DD[i]=NULL;
    }
  
  ls->USER=addptype(ls->USER, RCX_RC_TIMINGS_PTYPE, delays);
}

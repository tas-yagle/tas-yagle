
static ttvline_list *TTV_COM_CURRENT_LINE;
static int TTV_COM_DELAY_COMPUTATION_TYPE; // 0->MIN 1->MAX

static int ttv_communication(int op, void *data)
{
  switch(op)
    {
    case TTV_getcurrentline:
      *(ttvline_list **)data=TTV_COM_CURRENT_LINE;
      return 0;
    case TTV_getdelaytype:
      *(int *)data=TTV_COM_DELAY_COMPUTATION_TYPE;
      return 0;
    case TTV_getttvfig:
      *(ttvfig_list **)data=TTV_COM_CURRENT_LINE->FIG;
      return 0;
    }
  return 1;
}

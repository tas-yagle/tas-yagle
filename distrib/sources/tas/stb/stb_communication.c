
static stbfig_list *STB_COM_STBFIG;

int stb_communication(int op, void *data)
{
  switch(op)
    {
    case STB_getstbfig:
      *(stbfig_list **)data=STB_COM_STBFIG;
      return 0;
    }
  return 1;
}

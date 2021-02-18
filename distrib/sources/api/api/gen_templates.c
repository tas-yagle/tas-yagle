#include API_H

api_define_type *create_adt(char * orig, char *dest)
{
  api_define_type *adt;
  adt=(api_define_type *)mbkalloc(sizeof(api_define_type));
  adt->ORIG=mbkstrdup(orig);
  adt->DEST=mbkstrdup(dest);
  return adt;
}

chain_list *dup_adt_list(chain_list *cl)
{
  api_define_type *adt;
  if (cl==NULL) return cl;
  adt=(api_define_type *)mbkalloc(sizeof(api_define_type));
  adt->ORIG=mbkstrdup(((api_define_type *)cl->DATA)->ORIG);
  adt->DEST=mbkstrdup(((api_define_type *)cl->DATA)->DEST);
  return addchain(dup_adt_list(cl->NEXT), adt);
}

void free_adt_list(chain_list *cl)
{
  chain_list *ch;
  api_define_type *adt;
  for (ch=cl; ch!=NULL; ch=ch->NEXT)
    {
      adt=(api_define_type *)ch->DATA;
      mbkfree(adt->ORIG);
      mbkfree(adt->DEST);
      mbkfree(adt);
    }
  freechain(cl);
}

int gen_find_template_corresp(ht *base_ht, chain_list *defines, char *name, char *result)
{
  char buf[1024], *end, *start;
  chain_list *cl;
  api_define_type *adt;
  long l;


  strcpy(buf, name);
  start=buf;

  do
    {      
      end=strchr(start, '.');
      if (end!=NULL) *end='\0';
      for (cl=defines; cl!=NULL; cl=cl->NEXT)
        {
          adt=(api_define_type *)cl->DATA;
          if (strcasecmp(adt->ORIG, start)==0) break;
        }
      if (cl!=NULL && end!=NULL) 
        {
          if (base_ht==NULL)
            {
              strcpy(result, name);
              return 0;
            }
          if ((l=gethtitem(base_ht, namealloc(start)))==EMPTYHT)
            break;
          else
            defines=((template_corresp *)l)->defines;
          start=end+1;
        }
    } while (cl!=NULL && end!=NULL);

  if (end==NULL)
    {
      if (cl==NULL)
        strcpy(result, start);
      else
        strcpy(result, adt->DEST);
      return 0;
    }
  strcpy(result,"<not found>");
  return 1;
}

template_corresp *gen_get_template_corresp(ht *base_ht, char *name)
{
  long l;
  if ((l=gethtitem(base_ht, namealloc(name)))==EMPTYHT) return NULL;
  return (template_corresp *)l;
}

template_corresp *gen_add_template_corresp(ht *base_ht, char *new_name, char *orig_name, chain_list *defines)
{
  template_corresp *tc;

  tc=(template_corresp *)mbkalloc(sizeof(template_corresp));
  tc->new_name=namealloc(new_name);
  tc->orig_name=namealloc(orig_name);
  tc->defines=dup_adt_list(defines);

  addhtitem(base_ht, tc->new_name, (long)tc);

  return tc;
}

int gen_find_reverse_template_corresp(chain_list *defines, char *name, char *result)
{
  chain_list *cl;
  api_define_type *adt;

  for (cl=defines; cl!=NULL; cl=cl->NEXT)
    {
      adt=(api_define_type *)cl->DATA;
      if (strcasecmp(adt->DEST, name)==0) break;
    }
  
  if (cl==NULL)
    {
      strcpy(result,"<not found>");
      return 1;
    }

  strcpy(result, adt->ORIG);
  return 0;
}

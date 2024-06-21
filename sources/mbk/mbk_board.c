
#include AVT_H
#include MUT_H

#define SEPARATOR " "

Board *Board_CreateBoard()
{
  Board *b;
  int i;
  b=(Board *)mbkalloc(sizeof(Board));
  for (i=0;i<MAXCOL;i++) b->prop[i].size=0, b->prop[i].align='l', b->prop[i].sep=' ';

  b->list=NULL;
  return b;
}

void Board_SetSize(Board *B, int col, int size, char align)
{  
  B->prop[col].size=size;
  B->prop[col].align=align;
}

BoardColumn *Board_NewLine(Board *B)
{  
  BoardColumn *bc;
  int i;
  bc=(BoardColumn *)mbkalloc(sizeof(BoardColumn)*MAXCOL);
  for (i=0;i<MAXCOL;i++)
    {
      if (B->prop[i].sep!=' ') bc[i].value=mbkstrdup(SEPARATOR);
      else bc[i].value=NULL;
    }
  B->list=addchain(B->list, bc);
  return bc;
}

void Board_SetSep(Board *B, int col)
{  
  int l;
  BoardColumn *bc;
  
  B->prop[col].sep='|';
  l=avt_text_real_length(SEPARATOR);
  B->prop[col].size=l;
}

void Board_NewSeparation(Board *B)
{  
  B->list=addchain(B->list, NULL);
}

void Board_SetValue(Board *B, int col, char *val)
{
  BoardColumn *bc;
  int l;
  if (B->list==NULL) exit(4);
  if (B->prop[col].size==0) return;
  bc=(BoardColumn *)B->list->DATA;
  if (bc[col].value!=NULL) mbkfree(bc[col].value);
  bc[col].value=mbkstrdup(val);
  if (val[0]!='é' && val[0]!='è' && val[0]!='ç' && (l=avt_text_real_length(val))>B->prop[col].size) B->prop[col].size=l;
}


static void center(char *buf, int size, char *txt)
{
  int txts, i, j, diff;
  txts=avt_text_real_length(txt);
  diff=size-txts;
  for (i=0; i<diff/2; i++) buf[i]=' ';
  for (j=0; txt[j]!='\0'; j++) buf[i++]=txt[j];
  diff-=diff/2;
  while (diff>0) buf[i++]=' ', diff--;
  buf[i++]='\0';
}


void Board_DisplaySeparation_sub(FILE *f, int lib, int lev, Board *B, char *LP)
{
  chain_list *cl;
  BoardColumn *bc;
  int i, j;
  char format[20];

  if (B->list==NULL) return;
  
  bc=(BoardColumn *)B->list->DATA;
  if (f!=NULL)
    avt_fprintf(f,"%s", LP);
  else
    avt_log(lib, lev, "%s", LP);
  for (i=0;i<MAXCOL;i++)
    {          
      if (B->prop[i].size>0)
        {
          for (j=0; j<B->prop[i].size+1; j++)
            {
              if (f!=NULL)
                avt_fprintf(f,"_");
              else
                avt_log(lib, lev, "_");
            }
        }
    }
  if (f!=NULL)
    avt_fprintf(f,"\n");
  else
    avt_log(lib, lev, "\n");
}
void Board_DisplaySeparation(FILE *f, Board *B, char *LP)
{
  Board_DisplaySeparation_sub(f, 0, 0, B, LP);
}

void Board_Display_sub(FILE *f, int lib, int lev, Board *B, char *LP)
{
  chain_list *cl;
  BoardColumn *bc;
  int i, cnt, diff, title_print_tag=0, totsize;
  char format[1024];
  char title[1024];

  B->list=reverse(B->list);
  for (cl=B->list; cl!=NULL; cl=cl->NEXT)
    {
      bc=(BoardColumn *)cl->DATA;
      if (bc==NULL)
        Board_DisplaySeparation(f, B, LP);
      else
        {
          if (f!=NULL)
            avt_fprintf(f,"%s", LP);
          else
            avt_log(lib, lev, "%s", LP);
          for (i=0;i<MAXCOL;i++)
            {          
              if (B->prop[i].size>0) 
                {
                  if (bc[i].value!=NULL) diff=strlen(bc[i].value)-avt_text_real_length(bc[i].value);
                  else diff=0;
                  title_print_tag=0;
                  if (bc[i].value!=NULL)
                    {
                      if (bc[i].value[0]=='é')
                        {
                          totsize=0;
                          strcpy(title, &bc[i].value[1]);
                          totsize=B->prop[i].size+diff+1-1;
                          title_print_tag=1;
                        }
                      else if (bc[i].value[0]=='è' || bc[i].value[0]=='ç') 
                        {
                          if (strcmp(&bc[i].value[1],"")!=0)
                            {
                              strcat(title,"_");
                              strcat(title, &bc[i].value[1]);                              
                            }

                          title_print_tag=1;
                          totsize+=B->prop[i].size+diff+1+1-1;
                          if (bc[i].value[0]=='è')
                            {
                              center(format, totsize, title);
                              if (f!=NULL)
                                avt_fprintf(f, "%s ", format);
                              else
                                avt_log(lib, lev, format);
                            }
                        }
                    }
                  
                  if (!title_print_tag)
                    {
                      if (B->prop[i].align=='r') sprintf(format,"%%%ds ", B->prop[i].size+diff);
                      else if (B->prop[i].align=='l')  sprintf(format,"%%-%ds ", B->prop[i].size+diff);
                      else if (B->prop[i].align!='c') exit(8);
                      if (B->prop[i].align=='c')
                        {
                          if (bc[i].value!=NULL)
                            center(format, B->prop[i].size+diff+1, bc[i].value);
                          else
                            center(format, B->prop[i].size+diff+1, "");
                          if (f!=NULL)
                            avt_fprintf(f, "%s ", format);
                          else
                            avt_log(lib, lev, format);
                        }
                      else
                        {
                          if (bc[i].value!=NULL)
                            {
                              if (f!=NULL)
                                avt_fprintf(f, format, bc[i].value);
                              else
                                avt_log(lib, lev, format, bc[i].value);
                            }
                          else
                            {
                              if (f!=NULL)
                                avt_fprintf(f, format, "");
                              else
                                avt_log(lib, lev, format, "");
                            }
                        }
                    }
                }
            }
          if (f!=NULL)
            avt_fprintf(f,"" AVT_RESET "\n");
          else
            avt_log(lib, lev, "\n");
        }
    }
  B->list=reverse(B->list);
}

void Board_Display(FILE *f, Board *B, char *LP)
{
  Board_Display_sub(f, 0, 0, B, LP);
}

void Board_FreeBoard(Board *B)
{
  chain_list *cl;
  BoardColumn *bc;
  int i;
  char format[20];

  for (cl=B->list; cl!=NULL; cl=delchain(cl, cl))
    {
      bc=(BoardColumn *)cl->DATA;
      if (bc!=NULL)
        {
          for (i=0;i<MAXCOL;i++)
            {
              if (bc[i].value!=NULL) mbkfree(bc[i].value);
            }
          mbkfree(bc);
        }
    }
  mbkfree(B);
}

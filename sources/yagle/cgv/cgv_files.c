#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include MUT_H
#include MLO_H
#include CNS_H
#include SLB_H
#include CGV_H


typedef struct cgv_file
{
  struct cgv_file *NEXT;
  char *name;
  char *filename;
  char ext[16];
  int type;
  cgvfig_list *cgv_struct;
  lofig_list *lf;
  cnsfig_list *cf;
  int cut; 
  int inmem;
} cgv_file;

int CGV_WAS_PRESENT=0;
cgv_file *HEAD_CGV=NULL;

void addcgvfile(char *name, int type, int cut, char *filename, cgvfig_list *cgvstruct, void *lofig, void *cnsf, int inmem)
{
  cgv_file *cgvf=(cgv_file *)mbkalloc(sizeof(cgv_file));
  cgvf->name=namealloc(name);
  cgvf->filename=namealloc(filename);
  cgvf->type=type;
  cgvf->cgv_struct=cgvstruct;
  cgvf->lf=(lofig_list *)lofig;
  cgvf->cf=(cnsfig_list *)cnsf;
  cgvf->cut=cut;
  cgvf->inmem=inmem;
  cgvf->NEXT=HEAD_CGV;
  cgvstruct->USER=addptype(cgvstruct->USER, CGV_FILE_TYPE, cgvf);
  HEAD_CGV=cgvf;
//  printf("%s added in list\n",cgvf->name);
}

char *getcgvfileext(cgvfig_list *cgvstruct)
{
  cgv_file *cgvf;
  cgvf=(cgv_file *)getptype(cgvstruct->USER, CGV_FILE_TYPE)->DATA;
  return cgvf->ext;
}

cgvfig_list *getcgvfile(char *name, int type, int cut, char *filename)
{
  cgv_file *cgvf;
  name=namealloc(name);
  filename=namealloc(filename);
  for (cgvf=HEAD_CGV; cgvf!=NULL; cgvf=cgvf->NEXT)
    {
      if (cgvf->name==name && cgvf->type==type && cgvf->cut==cut)
        {
          if (filename==NULL || (filename==cgvf->filename)) break;
        }
    }
  if (cgvf==NULL) {/*printf("%s not found in list\n",name);*/return NULL;}
  /*printf("%s found in list\n",cgvf->name);*/
  
  return cgvf->cgv_struct;
}

void removecgvfiles(char *name, void (*did)(cgvfig_list *removed))
{
  cgv_file *cgvf, *prev=NULL, *next;
  for (cgvf=HEAD_CGV; cgvf!=NULL; cgvf=next)
    {
      next=cgvf->NEXT;
      if (cgvf->name==name)
        {
          if (prev==NULL) HEAD_CGV=cgvf->NEXT; else prev->NEXT=cgvf->NEXT;
          if (cgvf->cut==0 && cgvf->inmem==0)
            {
//              printf("remove '%s' [%d]\n",name,cgvf->type);
              switch(cgvf->type)
                {
                case CGV_SOURCE_CNSFIG:
                  cgvf->cf->NEXT=CNS_HEADCNSFIG;
                  CNS_HEADCNSFIG=cgvf->cf;
                  CNS_HEADCNSFIG = delcnsfig(CNS_HEADCNSFIG, CNS_HEADCNSFIG);
                  break;
                case CGV_SOURCE_LOFIG:
                  dellofig(cgvf->lf->NAME);
//                  freelofig(cgvf->lf);
                  /*cgvf->lf->NEXT=HEAD_LOFIG;
                  HEAD_LOFIG=cgvf->lf;
                  dellofig(cgvf->lf->NAME);*/
                  break;
                }
            }
          if (did) did(cgvf->cgv_struct);
          freecgvfigure(cgvf->cgv_struct);
          mbkfree(cgvf);
        }
      else prev=cgvf;
    }
}

void removeallcgvfiles(void (*did)(cgvfig_list *removed))
{
  cgv_file *cgvf, *next;
  for (cgvf=HEAD_CGV; cgvf!=NULL; cgvf=next)
    {
      next=cgvf->NEXT;
      if (cgvf->inmem==0)
        {
//          printf("remove '%s' [%d]\n",cgvf->name,cgvf->type);
          switch(cgvf->type)
            {
            case CGV_SOURCE_CNSFIG:
              cgvf->cf->NEXT=CNS_HEADCNSFIG;
              CNS_HEADCNSFIG=cgvf->cf;
              CNS_HEADCNSFIG = delcnsfig(CNS_HEADCNSFIG, CNS_HEADCNSFIG);
              break;
            case CGV_SOURCE_LOFIG:
              dellofig(cgvf->lf->NAME);
              //freelofig(cgvf->lf);
              /*cgvf->lf->NEXT=HEAD_LOFIG;
                HEAD_LOFIG=cgvf->lf;
                dellofig(cgvf->lf->NAME);*/
              break;
            }
        }
      if (did) did(cgvf->cgv_struct);
      freecgvfigure(cgvf->cgv_struct);
      mbkfree(cgvf);
    }
  HEAD_CGV=NULL;
}


#include <string.h>
#include <stdlib.h>

#include AVT_H
#include API_H
#include MUT_H

#include "database_API.h"

extern char *gen_info();

typedef struct variable
{
  struct variable *NEXT;
  char *name;
  char type;
  long value;
} variable;

typedef struct
{
  char *name;
  ht *H;
  variable *VAR;
} database;

chain_list *ALL_DATABASE;

void database_API_AtLoad_Initialize()// commentaire pour desactiver l'ajout de token
{
  ALL_DATABASE=NULL;
//  printf("init database\n");
}
static database *getdatabase(char *name)
{
  chain_list *cl;
  for (cl=ALL_DATABASE; cl!=NULL && strcmp(((database *)cl->DATA)->name, name)!=0; cl=cl->NEXT) ;
  if (cl==NULL) return NULL;
  return (database *)cl->DATA;
}

void dtb_Create(char *name)
{
  database *dtb;
  if (getdatabase(name)!=NULL) return;
  dtb=(database *)mbkalloc(sizeof(database));
  dtb->VAR=NULL;
  dtb->name=strdup(name);
  dtb->H=addht(128);
  ALL_DATABASE=addchain(ALL_DATABASE, dtb);
}

static variable *getentry(database *dtb, char *name)
{
  variable *v;
  long l;
  l=gethtitem(dtb->H, namealloc(name));
  if (l!=EMPTYHT) return (variable *)l;
  for (v=dtb->VAR; v!=NULL && strcmp(v->name,name)!=0; v=v->NEXT) ;
  return v;
}

static void distroyentry(variable *var)
{
  switch(var->type)
    {
    case 's':
    case 'd':
      mbkfree((void *)var->value);
    }
}

void dtb_RemoveEntry(char *dtbname, char *name)
{
 database *dtb;
 variable *v, *pred;
 if ((dtb=getdatabase(dtbname))==NULL) return;
 for (pred=NULL, v=dtb->VAR; v!=NULL && strcmp(v->name, name)!=0; pred=v, v=v->NEXT) ;
 if (v==NULL) return;

 delhtitem(dtb->H, namealloc(name));
 distroyentry(v);
 if (pred==NULL) dtb->VAR=v->NEXT;
 else pred->NEXT=v->NEXT;
 mbkfree(v->name);
 mbkfree(v);
}

void dtb_Clean(char *name)
{
  database *dtb;
  variable *v, *next;
  if ((dtb=getdatabase(name))==NULL) return;
  for (v=dtb->VAR; v!=NULL; v=next)
    {
      next=v->NEXT;
      distroyentry(v);
      mbkfree(v);
    }
  dtb->VAR=NULL;
  delht(dtb->H);
  dtb->H=addht(128);
}

void dtb_SetInt(char *dtbname, char *name, int value)
{
  variable *v;
  database *dtb;
  if ((dtb=getdatabase(dtbname))==NULL) return;
   if ((v=getentry(dtb, name))==NULL)
    {
      v=(variable *)mbkalloc(sizeof(variable));
      v->NEXT=dtb->VAR; dtb->VAR=v;
      v->name=strdup(name);
      addhtitem(dtb->H, namealloc(name), (long)v);
    }
  else
    distroyentry(v);
  v->type='i';
  *(int *)&v->value=value;
}
void dtb_SetDouble(char *dtbname, char *name, double value)
{
  variable *v;
  database *dtb;
  if ((dtb=getdatabase(dtbname))==NULL) return;
  if ((v=getentry(dtb, name))==NULL)
    {
      v=(variable *)mbkalloc(sizeof(variable));
      v->NEXT=dtb->VAR; dtb->VAR=v;
      v->name=strdup(name);
      addhtitem(dtb->H, namealloc(name), (long)v);
    }
  else
    distroyentry(v);
  v->type='d';
  v->value=(long)mbkalloc(sizeof(double));
  *(double *)v->value=value;
}
void dtb_SetLong(char *dtbname, char *name, long value)
{
  variable *v;
  database *dtb;
  if ((dtb=getdatabase(dtbname))==NULL) return;
  if ((v=getentry(dtb, name))==NULL)
    {
      v=(variable *)mbkalloc(sizeof(variable));
      v->NEXT=dtb->VAR; dtb->VAR=v;
      addhtitem(dtb->H, namealloc(name), (long)v);
    }
  else
    distroyentry(v);
  v->name=strdup(name);
  v->type='l';
  *(long *)&v->value=value;
}

void dtb_SetString(char *dtbname, char *name, char *value)
{
  variable *v;
  database *dtb;
  if ((dtb=getdatabase(dtbname))==NULL) return;
  if ((v=getentry(dtb, name))==NULL)
    {
      v=(variable *)mbkalloc(sizeof(variable));
      v->NEXT=dtb->VAR; dtb->VAR=v;
      v->name=strdup(name);
      addhtitem(dtb->H, namealloc(name), (long)v);
    }
  else
    distroyentry(v);
  v->type='s';
  *(char **)&v->value=strdup(value);
}

void dtb_SetChar(char *dtbname, char *name, char value)
{
  variable *v;
  database *dtb;
  if ((dtb=getdatabase(dtbname))==NULL) return;
  if ((v=getentry(dtb, name))==NULL)
    {
      v=(variable *)mbkalloc(sizeof(variable));
      v->NEXT=dtb->VAR; dtb->VAR=v;
      v->name=strdup(name);
      addhtitem(dtb->H, namealloc(name), (long)v);
    }
  else
    distroyentry(v);
  v->type='c';
  *(char *)&v->value=value;
}

void dtb_Save(char *name)
{
  database *dtb;
  variable *v;
  FILE *f;
  char nn[200];
  if ((dtb=getdatabase(name))==NULL) return;
  sprintf(nn,".%s.dtb",dtb->name);
  if ((f=fopen(nn, "wt"))==NULL)
    {
      fprintf(stderr,"%s: could not save database '%s' in file '%s'\n",gen_info(),dtb->name,nn);
      return;
    }
  for (v=dtb->VAR; v!=NULL; v=v->NEXT)
    {
      switch(v->type)
        {
        case 'i' : fprintf(f, "%c %-10s = %d\n", v->type, v->name, *(int *)v->value); break;
        case 'l' : fprintf(f, "%c %-10s = %ld\n", v->type, v->name, *(long *)v->value); break;
        case 'd' : fprintf(f, "%c %-10s = %e\n", v->type, v->name, *(double *)v->value); break;
        case 'c' : fprintf(f, "%c %-10s = '%c'\n", v->type, v->name, *(char *)v->value); break;
        case 's' : fprintf(f, "%c %-10s = \"%s\"\n", v->type, v->name, (char *)v->value); break;
        }
    }
  fclose(f);
}


int dtb_Load(char *name)
{
  FILE *f;
  char nn[200], var[200];
  char type;
  int i, ret=1;

//  dtb_Clean(name);
  if (getdatabase(name)==NULL) dtb_Create(name);
  sprintf(nn,".%s.dtb",name);
  if ((f=fopen(nn, "rt"))==NULL) return 1;

  while (fscanf(f,"%s",nn)>0)
    {
      type=nn[0];
      if (fscanf(f,"%s",var)<=0) goto emergency_exit;
      if (fscanf(f,"%s",nn)<=0 || strcmp(nn,"=")!=0) goto emergency_exit;
      switch(type)
        {
        case 'i' : 
          fscanf(f,"%s",nn); dtb_SetInt(name, var, atoi(nn)); break;
        case 'l' :
          fscanf(f,"%s",nn); dtb_SetLong(name, var, atol(nn)); break;
        case 'd' :
          fscanf(f,"%s",nn); dtb_SetDouble(name, var, atof(nn)); break;
        case 'c' :
          fscanf(f,"%s",nn); dtb_SetChar(name, var, nn[0]); break;
        case 's' :
          while(fgetc(f)!='"' && !feof(f)) ;
          if (feof(f)) goto emergency_exit;
          i=0;
          while((nn[i++]=fgetc(f))!='"' && !feof(f)) ;
          if (feof(f)) goto emergency_exit;
          nn[i-1]='\0';
          dtb_SetString(name, var, nn); break;
        }
    }
  ret=0;
 emergency_exit:
  fclose(f);
  return ret;
}

double dtb_GetDouble(char *dtbname, char *name)
{
  variable *v;
  database *dtb;
  if ((dtb=getdatabase(dtbname))==NULL) return 0.0;
  if ((v=getentry(dtb, name))==NULL) return 0.0;
  if (v->type!='d') return 0.0;  
  return *(double *)v->value;
}

int dtb_GetInt(char *dtbname, char *name)
{
  variable *v;
  database *dtb;
  if ((dtb=getdatabase(dtbname))==NULL) return 0;
  if ((v=getentry(dtb, name))==NULL) return 0;
  if (v->type!='i') return 0;  
  return *(int *)&v->value;
}

long dtb_GetLong(char *dtbname, char *name)
{
  variable *v;
  database *dtb;
  if ((dtb=getdatabase(dtbname))==NULL) return 0;
  if ((v=getentry(dtb, name))==NULL) return 0;
  if (v->type!='l') return 0;  
  return *(long *)&v->value;
}

char dtb_GetChar(char *dtbname, char *name)
{
  variable *v;
  database *dtb;
  if ((dtb=getdatabase(dtbname))==NULL) return ' ';
  if ((v=getentry(dtb, name))==NULL) return ' ';
  if (v->type!='c') return ' ';  
  return *(char *)&v->value;
}

char *dtb_GetString(char *dtbname, char *name)
{
  variable *v;
  database *dtb;
  if ((dtb=getdatabase(dtbname))==NULL) return NULL;
  if ((v=getentry(dtb, name))==NULL) return NULL;
  if (v->type!='s') return NULL;  
  return (char *)v->value;
}

#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <ctype.h>
#include MUT_H
#include AVT_H

#define MBK_JOK '*'
#define MBKREGEXBUFSIZE 1024
#define MBKMAXREGEXCACHE 4097
#define CACHE_ID_CHAR (char) 002 //STX

static char CONFIG_REGEX_MODE=' ';
static regex_t latest_regular_expression;
static char *latest_regular_expression_string=NULL;
static ht *regex_cache_index=NULL;
typedef struct 
{
  char *orig;
  regex_t regex;
} regex_cache_type;
static regex_cache_type *regex_cache;
static int regex_cnt;

char *mbk_index_regex(char *r)
{
  int casef;
  char buf0[MBKREGEXBUFSIZE];
  char *nr;
  long l;
  char tui[5];
  if (r==NULL || r[0]!='%') return r;
  if (regex_cache_index==NULL)
  {
    regex_cache_index=addht(50);
    regex_cnt=0;
    regex_cache=(regex_cache_type *)mbkalloc(sizeof(regex_cache_type)*MBKMAXREGEXCACHE);
  }
  if (regex_cnt>=MBKMAXREGEXCACHE) return r;
  if (CASE_SENSITIVE!='Y')
    casef=REG_ICASE;
  else
    casef=0;
  nr=namealloc(r);
  if ((l=gethtitem(regex_cache_index, nr))!=EMPTYHT) return (char *)l;
  sprintf(buf0, "^(%s)$", &r[1]);
  if (regcomp(&regex_cache[regex_cnt].regex, buf0, REG_EXTENDED | REG_NOSUB | REG_NEWLINE | casef)!=0) return r;
  regex_cache[regex_cnt].orig=nr;

  tui[0]='%';
  tui[1]=CACHE_ID_CHAR;
  tui[2]=((regex_cnt>>8) & 0xff) + 1;
  tui[3]=regex_cnt & 0xff;
  tui[4]='\0';
  r=namealloc(tui);
  addhtitem(regex_cache_index, nr, (long)r);

  regex_cnt++;
  return r;
}

int mbk_get_index_regex(char *refname)
{
  int index;
  if (regex_cache_index!=NULL && refname[0]==CACHE_ID_CHAR)
    {
      index=((unsigned char)refname[2]) | (((unsigned char)refname[1]-1)<<8);
      return index;
    }
  return -1;
}

char *mbk_get_reverse_index_regex(char *refname)
{
  int index;
  if (refname[0]!='%' || (refname[0]=='%' && (index=mbk_get_index_regex(&refname[1]))<0)) return refname;
  return regex_cache[index].orig;
}
static int mbk_TestJoker(char *testname, char *oldname);

static inline void checkregexmode()
{
  char *env;
  if (CONFIG_REGEX_MODE==' ')
    {
      CONFIG_REGEX_MODE='T';
      if ((env=getenv("AVT_REGEX_MODE"))!=NULL)
        {
          if (strcasecmp(env,"simple")==0) CONFIG_REGEX_MODE='T';
          else if (strcasecmp(env,"standard")==0) CONFIG_REGEX_MODE='C';
        }
    }
}

char mbk_GetREGEXMODE()
{
  checkregexmode();
  return CONFIG_REGEX_MODE;
}

void mbk_CreateREGEX(mbk_match_rules *mr, char casef, int canbenonnameallocated)
{
  checkregexmode();
  mr->QUICKFIND=NULL;
  mr->reg_list=NULL;
  mr->name_list=NULL;
  mr->casef=tolower(casef);
  mr->cbna=canbenonnameallocated;
}

void mbk_FreeREGEX(mbk_match_rules *mr)
{
  chain_list *cl;

  if (mr->QUICKFIND!=NULL) delht(mr->QUICKFIND);
  for (cl=mr->reg_list; cl!=NULL; cl=cl->NEXT)
    {
      regfree((regex_t *)cl->DATA);
      mbkfree(cl->DATA);
    }
  freechain(mr->reg_list);
  freechain(mr->name_list);
}

void mbk_AddREGEX(mbk_match_rules *mr, char *name)
{
  char buf0[2048];
  regex_t *regular_expression;
  int casef;

  if (mr->casef!='y')
    casef=REG_ICASE;
  else
    casef=0;

  if ((name[0]=='%' || CONFIG_REGEX_MODE=='C')/* && (strchr(name,'.')!=NULL || strchr(name,'*')!=NULL
                                 || strchr(name,'[')!=NULL || strchr(name,']')!=NULL)*/)
    {      
      sprintf(buf0, "^(%s)$", name[0]=='%'?&name[1]:name);
      regular_expression=(regex_t *)mbkalloc(sizeof(regex_t));
      if (regcomp(regular_expression, buf0, REG_EXTENDED | REG_NOSUB | REG_NEWLINE | casef)==0)
        {
          mr->reg_list=append(mr->reg_list, addchain(NULL, regular_expression));
        }
      else mbkfree(regular_expression);
    }
  else
  {
    if (mbk_isregex_name(name))
      mr->name_list=addchain(mr->name_list, name);
    else
    {
      if (mr->QUICKFIND==NULL) mr->QUICKFIND=addht(64);
      addhtitem(mr->QUICKFIND, name, 0);
      if (mr->cbna)
        mr->name_list=addchain(mr->name_list, name);
    }
  }
  
}

static char *sub_elt(char *sub, char **addSub, char jok)
{
  char            buff[MBKREGEXBUFSIZE];
  char           *res = buff;

  while ((*sub != jok) && (*sub != '\0')) {
    *res++ = *sub++;
    (*addSub)++;
  }

  *res = '\0';

  res = (char *) mbkalloc(strlen(buff) + 1);
  strcpy(res, buff);
  return (res);
}

static char *motif_in_name(char *nom, char *motif)
{
  char           *pt1 = NULL;
  char           *pt2 = NULL;
  char           *pt3 = NULL;

  for (pt1 = nom; (*pt1) != '\0'; pt1++) {
    pt3 = motif;
     if (CASE_SENSITIVE=='Y')
       for (pt2 = pt1; (((*pt2) != '\0') && ((*pt3) != '\0') && (*pt2 == *pt3)); pt3++, pt2++);
     else
       for (pt2 = pt1; (((*pt2) != '\0') && ((*pt3) != '\0') && (tolower(*pt2) == tolower(*pt3))); pt3++, pt2++);
    if (*pt3 == '\0')
      return (pt2);
  }
  return (NULL);
}

static char *joker_subst(char *name, char *subMotif, char *subName)
{
  char           *res;
  char            buff[MBKREGEXBUFSIZE];


  if ((*subMotif == MBK_JOK) && (*subName != MBK_JOK)) {
    sprintf(buff, "%s", sub_elt(subName, &subName, MBK_JOK));
    strcat(buff, name);
  }
  else
    strcpy(buff, name);

  while (*subMotif != '\0') {
    char           *motif1;
    char           *motif2;

    if (*subMotif == MBK_JOK) {
      subMotif++;
      subName++;
    }
    if ((*subMotif == '\0') && (*subName == '\0')) {
      break;
    }
    else if ((*subMotif == '\0') && (*subName != '\0')) {
      strcat(buff, subName);
      break;
    }
    else if ((*subMotif != '\0') && (*subName == '\0')) {
      motif1 = sub_elt(subMotif, &subMotif, MBK_JOK);
      buff[strlen(buff) - strlen(motif1)] = '\0';
      break;
    }
    else {
      motif1 = sub_elt(subMotif, &subMotif, MBK_JOK);
      motif2 = sub_elt(subName, &subName, MBK_JOK);
      name = motif_in_name(name, motif1);
      buff[(strlen(buff) - strlen(name) - strlen(motif1))] = '\0';
      strcat(buff, motif2);
      strcat(buff, name);
    }
  }

  res = (char *) mbkalloc(strlen(buff) + 1);
  strcpy(res, buff);
  return (res);
}

char *mbk_REGEXJokerName(char *name, char *oldname, char *newname)
{
    char           *pt = oldname;

    while (*pt != '\0') {
        if (*pt == MBK_JOK)
            break;
        pt++;
    }

    if (*pt == '\0')
        return NULL;

    if (mbk_TestREGEX(name, oldname)) {
        return (joker_subst(name, oldname, newname));
    }
    else
        return NULL;
}


static int mbk_TestJoker(char *testname, char *oldname)
{
  char           *name = testname;
  char           *sub = oldname;
  char           *motif;
  int             i, j;

  if (*sub != MBK_JOK) {
    if (CASE_SENSITIVE=='Y')
      {
        while (*sub == *name && *sub && *name ) {
          sub++;
          name++;
        }
      }
    else
      {
        while (tolower(*sub) == tolower(*name) && *sub && *name ) {
          sub++;
          name++;
        }
      }
    if (*sub != MBK_JOK)
      return 0;
    sub = oldname;
    name = testname;
  }

  if (sub[strlen(sub)] != MBK_JOK) {
    i = strlen(sub);
    j = strlen(name);
    if (CASE_SENSITIVE=='Y')
      {
        while (i>=0 && j>=0 && sub[i] == name[j]) {
          j--;
          i--;
        }
      }
    else
      {
        while (i>=0 && j>=0 && tolower(sub[i]) == tolower(name[j]) ) {
          j--;
          i--;
        }
      }
    if (i<0 || (i>=0 && sub[i] != MBK_JOK))
      return 0;
  }

  while (*sub != '\0') {
    if (*sub == MBK_JOK)
      sub++;
    if (*sub == '\0')
      return 1;
    motif = sub_elt(sub, &sub, MBK_JOK);
    name = motif_in_name(name, motif);
    mbkfree(motif);
    if (name == NULL)
      return 0;
  }
  return 1;
}

/*****************************************************************************
 *                         function yagTestNameMatch()                       *
 *****************************************************************************/
static int mbk_TestREGEX_TASLIKE(char *testname, char *refname)
{
  if (strchr(refname, MBK_JOK) != NULL) {
    if (mbk_TestJoker(testname, refname)) {
      return 1;
    }
  }
  else if (mbk_casestrcmp(testname,refname)==0) {
    return 1;
  }
  return 0;
}

static int mbk_TestREGEX_CLIKE(char *testname, char *refname)
{
  char buf0[MBKREGEXBUFSIZE];
  regmatch_t pmatch[1];
  char *cacheregname;
  regex_t *cacheregex;
  int index;

  // seule exception aux regex C
  if (refname[0]=='*' && refname[1]=='\0') return 1;
  // ----

  if (1/*strchr(refname,'.')!=NULL || strchr(refname,'*')!=NULL
      || strchr(refname,'[')!=NULL || strchr(refname,']')!=NULL*/)
    {
      index=mbk_get_index_regex(refname);
      if (index>=0)
      {
        cacheregname=regex_cache[index].orig;
        cacheregex=&regex_cache[index].regex;
      }
      else 
      {
        if (latest_regular_expression_string==NULL || strcmp(latest_regular_expression_string, refname)!=0)
        {
          int casef;
          if (CASE_SENSITIVE!='Y')
            casef=REG_ICASE;
          else
            casef=0;

          if (latest_regular_expression_string!=NULL)
            {
              mbkfree(latest_regular_expression_string);
              regfree(&latest_regular_expression);
            }
          sprintf(buf0, "^(%s)$", refname);
          if (regcomp(&latest_regular_expression, buf0, REG_EXTENDED | REG_NOSUB | REG_NEWLINE | casef)==0)
            {
              latest_regular_expression_string=strdup(refname);
            }
          else latest_regular_expression_string=NULL;
        }
        cacheregname=latest_regular_expression_string;
        cacheregex=&latest_regular_expression;
      }

      if (cacheregname!=NULL)
        {
          if (regexec (cacheregex, testname, 0, pmatch, 0)!=REG_NOMATCH)
            return 1;
        }
      return 0;  
    }
  else
    return mbk_casestrcmp(testname,refname)==0;
}


int mbk_TestREGEX(char *testname, char *refname)
{
  checkregexmode();

  if (refname[0]=='%')
    return mbk_TestREGEX_CLIKE(testname, &refname[1]);
  else if (CONFIG_REGEX_MODE=='C')
    return mbk_TestREGEX_CLIKE(testname, refname);
  else 
    return mbk_TestREGEX_TASLIKE(testname, refname);
}


int mbk_CheckREGEX(mbk_match_rules *mr, char *name)
{
  chain_list *cl;
  regmatch_t pmatch[1];

  if (mr==NULL) return 0;

  if (mr->QUICKFIND!=NULL && gethtitem(mr->QUICKFIND, name)!=EMPTYHT) return 1;

  for (cl=mr->name_list; cl!=NULL; cl=cl->NEXT)
    {
      if (mbk_TestREGEX_TASLIKE(name, (char *)cl->DATA)) return 1;
    }

  for (cl=mr->reg_list; cl!=NULL; cl=cl->NEXT)
    {
      if (regexec ((regex_t *)cl->DATA, name, 0, pmatch, 0)!=REG_NOMATCH)
        return 1;                        
    }
 /* 
  for (cl=mr->name_list; cl!=NULL; cl=cl->NEXT)
    {
      if ((mr->casef=='y' && strcmp(name, (char *)cl->DATA)==0)
          || (mr->casef!='y' && strcasecmp(name, (char *)cl->DATA)==0)) return 1;
    }
    */

  return 0;
}

int mbk_isregex_name(char *name)
{
  if (strchr(name,'*')!=NULL || strchr(name,'%')!=NULL) return 1;
  return 0;
}



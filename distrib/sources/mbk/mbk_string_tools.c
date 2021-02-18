#include <string.h>
#include <stdlib.h>

int mbk_FindNumber(char *source, char *rule, int *index)
{
  int i;
  char *c, *start, *end;

  start=source;
  c=strchr(rule,'?');
  if (c==NULL) return 1;
  i=c-rule;
  if (strncmp(source, rule, i)!=0) return 1;
  if (source[i]<'0' || source[i]>'9') return 1;
  *index=strtol(&source[i], &end, 10);
  c++;
  if (end==NULL && *c=='\0') return 0;
  if (strcmp(end, c)==0) return 0;
  return 1;
}

#include <stdio.h>

int main(int ac, char *av[])
{
  FILE *f;
  int c, trigger=0, endpatch=0, incomment=0, prev=0, deact=0;
  
  if (ac<3)
    {
      fprintf(stderr,"%s <input c file> <code to add>\n", av[0]);
      return 2;
    }
  f=fopen(av[1], "rt");
  if (f==NULL) return 1;

  while ((c=fgetc(f))!=EOF)
    {
      switch(c)
        {
        case ')':
          putchar(c); 
          if (!incomment)
            {
              if (!endpatch) trigger=1; 
            }
          break;
        case '{':
          putchar(c);
          if (!incomment)
            {
              if (!endpatch && trigger)
                {
                  if (!deact)
                    {
                      printf(av[2]);      
                      printf(" {");
                    }
                  endpatch++;
                  trigger=0;
                }
              else if (endpatch>0) endpatch++;
//              printf("<%d>",endpatch);
            }
          break;
        case '}':
          putchar(c);
          if (!incomment)
            {
              if (endpatch>=1)
                {
                  if (endpatch==1 && deact==0) 
                    putchar('}');
                  endpatch--;
                  if (endpatch==0) deact=0;
                }
//              printf("<%d>",endpatch);
            }
          break;
        case '\n':
          incomment=0;
        case '\t':
        case ' ':
          putchar(c);
          break;
        case '/':
          if (prev=='/') 
            {
              incomment=1;
              if (trigger==1) deact=1;
            }
          putchar(c);
          break;
        default:
          putchar(c);
          if (!incomment)
            {
              if (trigger==1) trigger=0;
            }
        }
      prev=c;
    }
  fclose(f);
  return 0;
}

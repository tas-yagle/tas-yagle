#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include AVT_H
#include API_H
#include MUT_H

extern char *gen_info();

char *char_to_string(int size, char caract)
{
  static char t[2048];
  int i;

  for (i=0;i<size;i++)
    t[i]=caract;
  t[i]='\0';
  return t;
}

char *onehot_to_bit(int size, int bitnum)
{
  static char t[2048];
  int i;
   if (bitnum<=0 || bitnum>size)
     {
       fprintf(stderr,"%s: onehot_to_bit: Out of bounds (size=%d, bitnum=%d)\n",gen_info(),size,bitnum);
       EXIT(1);
     }   
  for (i=0;i<size;i++)
    t[i]='0';
  t[i]='\0';
  t[size-bitnum]='1';
  return t;
}

char *onehot_to_hexa(int size, int bitnum)
{
  static char t[2048];
  int i, digit_number, index;

  if (bitnum<=0 || bitnum>size)
     {
       fprintf(stderr,"%s: onehot_to_hexa: Out of bounds (size=%d, bitnum=%d)\n",gen_info(),size,bitnum);
       EXIT(1);
     }   

  digit_number=size/4+(size%4?1:0);
  index=bitnum/4+(bitnum%4?1:0);

  for (i=0;i<digit_number;i++)
    t[i]='0';
  t[i]='\0';

  switch ((bitnum-1)%4) 
    {
    case 0: t[digit_number-index] = '1'; break;
    case 1: t[digit_number-index] = '2'; break;
    case 2: t[digit_number-index] = '4'; break;
    case 3: t[digit_number-index] = '8'; break;
    }
  return t;
}

char *onehot_to_octa(int size, int bitnum)
{
  static char t[2048];
  int i, digit_number, index;

  if (bitnum<=0 || bitnum>size)
     {
       fprintf(stderr,"%s: onehot_to_octa: Out of bounds (size=%d, bitnum=%d)\n",gen_info(),size,bitnum);
       EXIT(1);
     }   

  digit_number=size/3+(size%3?1:0);
  index=bitnum/3+(bitnum%3?1:0);

  for (i=0;i<digit_number;i++)
    t[i]='0';
  t[i]='\0';

  switch ((bitnum-1)%3) 
    {
    case 0: t[digit_number-index] = '1'; break;
    case 1: t[digit_number-index] = '2'; break;
    case 2: t[digit_number-index] = '4'; break;
    }
  return t;
}

char *onecold_to_bit(int size, int bitnum)
{
  static char t[2048];
  int i;
   if (bitnum<=0 || bitnum>size)
     {
       fprintf(stderr,"%s: onecold_to_bit: Out of bounds (size=%d, bitnum=%d)\n",gen_info(),size,bitnum);
       EXIT(1);
     }   
  for (i=0;i<size;i++)
    t[i]='1';
  t[i]='\0';
  t[size-bitnum]='0';
  return t;
}

char *onecold_to_hexa(int size, int bitnum)
{
  static char t[2048];
  int i, digit_number, index;

  if (bitnum<=0 || bitnum>size)
     {
       fprintf(stderr,"%s: onecold_to_hexa: Out of bounds (size=%d, bitnum=%d)\n",gen_info(),size,bitnum);
       EXIT(1);
     }   

  digit_number=size/4+(size%4?1:0);
  index=bitnum/4+(bitnum%4?1:0);

  for (i=0;i<digit_number;i++)
    t[i]='F';
  t[i]='\0';

  switch ((bitnum-1)%4) 
    {
    case 0: t[digit_number-index] = 'E'; break;
    case 1: t[digit_number-index] = 'D'; break;
    case 2: t[digit_number-index] = 'B'; break;
    case 3: t[digit_number-index] = '7'; break;
    }
  return t;
}

char *onecold_to_octa(int size, int bitnum)
{
  static char t[2048];
  int i, digit_number, index;

  if (bitnum<=0 || bitnum>size)
     {
       fprintf(stderr,"%s: onecold_to_octa: Out of bounds (size=%d, bitnum=%d)\n",gen_info(),size,bitnum);
       EXIT(1);
     }   

  digit_number=size/3+(size%3?1:0);
  index=bitnum/3+(bitnum%3?1:0);

  for (i=0;i<digit_number;i++)
    t[i]='7';
  t[i]='\0';

  switch ((bitnum-1)%3) 
    {
    case 0: t[digit_number-index] = '6'; break;
    case 1: t[digit_number-index] = '5'; break;
    case 2: t[digit_number-index] = '3'; break;
    }
  return t;
}

char *genius_date()
{
  time_t         secondes;
  struct tm     *jours;
  char temp[1000];

  time( &secondes );
  jours = localtime( &secondes );
  sprintf(temp,
	  "Date ( dd/mm/yyyy hh:mm:ss ): %2d/%02d/%04d at %2d:%02d:%02d",
	  jours->tm_mday,
	  jours->tm_mon+1,
	  jours->tm_year+1900,
	  jours->tm_hour,
	  jours->tm_min,
	  jours->tm_sec
          );
  return strdup(temp);
}

char *bitstring_to_hexastring(char *value)
{
  static char temp[2048];
  int cnt, n=198, j;
  char val;
  temp[199]=(char)0;
  for (j=strlen(value)-1, cnt=0, val=0;j>=0;j--)
    {
      cnt++;
      if (cnt>=5)
	{
	  if (val<10)
	    temp[n]='0'+(char)val;
	  else
	    temp[n]='a'+(char)(val-10);
	  n--;
	  val=0;
	  cnt=1;
	}
      val=val >> 1;
      if (value[j]=='1') val=val+ (1<<3);
    }
  if (cnt<=4)
    {
      if (val<10)
	temp[n]='0'+(char)val;
      else
	temp[n]='a'+(char)(val-10);
    }
  return &temp[n];
}

long genius_top()
{
  return mbkprocessmemoryusage()/1024;
}

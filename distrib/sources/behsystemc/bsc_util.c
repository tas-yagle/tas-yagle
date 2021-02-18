/****************************************************************************/
/*                                                                          */
/* file         : bsc_drive.c                                               */
/* date         : March 2003                                                */
/* version      : v100                                                      */
/* author(s)    : PINTO A.                                                  */
/*                                                                          */
/* description  : Utilities for SystemC HDL driver                          */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include MUT_H
#include LOG_H
#include BEH_H
#include BEG_H
#include "bsc_utype.h"
#include "bsc_util.h"
#include "bsc_utdef.h"


char        *bsc_getBitStr(char *str);
static int   bsc_2puiss(int exp);
char        *bsc_bitToHex(char *str);

/****************************************************************************/
/*{{{                    bgl_mystrcat()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static char *bsc_mystrcat(char *str1, char *str2, ... )
{
  va_list    arg;
  char       buf[64];

  va_start(arg,str2);
  vsprintf(buf,str2,arg);
  va_end  (arg);
  return strcat(str1,buf);
}

/*}}}************************************************************************/
/*{{{                    bsc_name()                                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static struct beden **namtab=NULL;

void bsc_freenames(void)
{
  beh_fretab(namtab);
  namtab = NULL;
}

char *bsc_name (char *name)
{
  char                 *new_name;
  char                 *prv_name;
  char                 *tmp_name;
  char                  buffer[200];
  int                   i,j,flag,number;
  static char          *keywrd [] =
  {
    "abs"         , "access"       , "after"       , "alias"        ,
    "all"         , "and"          , "architecture", "array"        ,
    "assert"      , "attribute"    , "begin"       , "bit"          ,
    "bit_vector"  , "block"        , "body"        , "buffer"       ,
    "bus"         , "case"         , "component"   , "configuration",
    "constant"    , "disconnect"   , "downto"      , "else"         ,
    "elsif"       , "end"          , "entity"      , "error"        ,
    "exit"        , "file"         , "for"         , "function"     ,
    "generate"    , "generic"      , "guarded"     , "if"           ,
    "in"          , "inout"        , "is"          , "label"        ,
    "library"     , "linkage"      , "loop"        , "map"          ,
    "mod"         , "mux_bit"      , "mux_vector"  , "nand"         ,
    "natural"     , "new"          , "next"        , "nor"          ,
    "not"         , "null"         , "of"          , "on"           ,
    "open"        , "or"           , "others"      , "out"          ,
    "package"     , "port"         , "procedure"   , "process"      ,
    "range"       , "record"       , "reg_bit"     , "reg_vector"   ,
    "register"    , "rem"          , "report"      , "return"       ,
    "select"      , "severity"     , "signal"      , "stable"       ,
    "subtype"     , "then"         , "to"          , "transport"    ,
    "type"        , "units"        , "until"       , "use"          ,
    "variable"    , "wait"         , "warning"     , "when"         ,
    "while"       , "with"         , "wor_bit"     , "wor_vector"   ,
    "xor"
  };

  if (namtab == NULL)
  {
    namtab  = beh_initab ();
    for (i = 0; i < 93; i ++)
      beh_addtab (namtab, namealloc (keywrd [i]), NULL, BSC_NEWDFN , 1);
  }

  tmp_name = namealloc (name);
  new_name = (char *) beh_chktab (namtab,tmp_name,NULL,BSC_PNTDFN);

  if (beh_chktab (namtab,tmp_name,NULL,BSC_NAMDFN) == 0)
  {
    i       = 0;
    j       = 0;
    number  = 0;
    flag    = 1;
    while (tmp_name[i] != '\0')
    {
      buffer[j] = tmp_name[i];
      if ( ((tmp_name[i] >= 'a') && (tmp_name[i] <= 'z')) ||
           ((tmp_name[i] >= 'A') && (tmp_name[i] <= 'Z')) ||
           ((tmp_name[i] >= '0') && (tmp_name[i] <= '9') && (i != 0)))
        flag = 0;
      else
      {
        if (flag == 1)
          buffer[j++]   = 'v';
        buffer[j]       = '_';
        flag            = 1;
      }
      i ++;
      j ++;
    }
    if (buffer[j-1] == '_')
      buffer[j++]   = '0';
    buffer[j]       = '\0';
    new_name        = namealloc (buffer);

    prv_name        = new_name;
    while (beh_chktab (namtab,new_name,NULL,BSC_NEWDFN) != 0)
    {
      new_name      = prv_name;
      sprintf (buffer,"%s_%d",new_name,number++);
      prv_name      = new_name;
      new_name      = namealloc (buffer);
    }
    beh_addtab (namtab,new_name,NULL,BSC_NEWDFN,1);
    beh_addtab (namtab,tmp_name,NULL,BSC_PNTDFN,(long)new_name);
    beh_addtab (namtab,tmp_name,NULL,BSC_NAMDFN,1);
  }

  return (new_name);
}

/*}}}************************************************************************/
/*{{{                    bsc_vectorize()                                    */
/*                                                                          */
/* description : put parenthesis on element of bussed signals           */
/*                                                                          */
/****************************************************************************/
char *bsc_vectorize(char *name)
{
  char *new_name  ;
  char  tmp1 [256];
  int   i          = 0;
  int   j          = 0;
  int   left;
  int   right;
  static struct beden **tab = NULL;

  if (tab == NULL)
    tab     = beh_initab ();

  new_name  = (char *) beh_chktab (tab, name, NULL, BSC_PNTDFN);
  if (new_name == NULL)
  {
    strcpy (tmp1, name);

    while ((name [i] != '\0') && (name [i] != ' ' ) && (name [i] != '\''))
      i ++;

    tmp1 [i]    = '\0';
    new_name    = bsc_name (tmp1);

    if (name [i] != '\0')
    {
      if (name [i] == ' ')
      {
        //// added by anto
        j       = i;
        while ((name[j]!='\0')&&(name[j]!=':'))
          j++;
        if (name[j]!='\0')
        {
          left  = atoi(&name[i+1]);
          right = atoi(&name[j+1]);
          sprintf (tmp1,"%s(%d %s %d)",new_name,
                   left,(left<right)?"TO":"DOWNTO",right);
        }
        ////
        else
          sprintf (tmp1, "%s(%s)", new_name, &name[i+1]); 
      }
      else
        sprintf (tmp1, "%s'%s"  , new_name, &name[i+1]); 
      new_name  = namealloc (tmp1);
    }
    beh_addtab (tab, name, NULL, BSC_PNTDFN, (long)new_name);
  }

  return (new_name);
}

/*}}}************************************************************************/
/*{{{                    bsc_vectnam()                                      */
/*                                                                          */
/*  analyze a list of signal and return the bounds of the vectorised        */
/*    signals, if they occure.                                              */
/* attention !!! cette fonction part du principe que ->NAME est toujours    */
/*               le 2 eme champs de chaque structure beXXX                  */
/*                                                                          */
/****************************************************************************/
void *bsc_vectnam(void *pt_list, int *left, int *right, char **name, char type)
/* If type = 0 bepor_list, type = 1 bebux_list*/
/* If type = 2 bereg_list, type = 3 beaux_list*/
{
  char          *blank_space;
  char          *sig_name;
  char           name_tmp[200];
  char           number[200];
  bepor_list    *ptpor;
  bebux_list    *ptbux;
  beaux_list    *ptaux;
  bereg_list    *ptreg;
  char           END     = 0;

  /* Case bepor_list */
  if(type == 0)
  {
    ptpor       = (bepor_list *)pt_list;
    *left       = *right = -1;
    sig_name    = ptpor->NAME;
    *name       = (char*)mbkalloc(strlen(sig_name) + 1);
    strcpy(*name,sig_name);
    blank_space = strchr(*name,' ');
    if (blank_space != NULL)
    {
      strcpy(number,blank_space);
      *left         = atoi(number);
      *right        = *left;
      *blank_space  = '\0';
    }

    while(!END)
    {
      if(ptpor->NEXT != NULL)
      {
        strcpy(name_tmp,(ptpor->NEXT)->NAME);
        blank_space = strchr(name_tmp,' ');
        if(blank_space!=NULL)
        {
          strcpy(number,blank_space);
          *blank_space = '\0';
          if(!strcmp(*name,name_tmp))
          {
            *right  = atoi(number);
            ptpor   = ptpor->NEXT;
          }
          else
            END     = 1;
        }
        else
          END       = 1;
      }
      else
        END         = 1;
    }
    return(ptpor);
  }

  /*case bebux_list */
  if(type==1)
  {
    ptbux       = (bebux_list *)pt_list;
    /* Extract the name and number of an element */
    *left       = *right    = -1;
    sig_name    = ptbux->NAME;
    *name       = (char *)mbkalloc(strlen(sig_name) + 1);
    strcpy(*name,sig_name);
    blank_space = strchr(*name,' ');
    if (blank_space != NULL)
    {
      strcpy(number,blank_space);
      *right        = atoi(number);
      *left         = *right;
      *blank_space  = '\0';
    }

    while(END != 1)
    {
      if(ptbux->NEXT != NULL)
      {
        strcpy(name_tmp,ptbux->NEXT->NAME);
        blank_space     = strchr(name_tmp,' ');
        if(blank_space!=NULL)
        {
          strcpy(number,blank_space);
          *blank_space  = '\0';
          if(!strcmp(*name,name_tmp))
          {
            *right  = atoi(number);
            ptbux   = ptbux->NEXT;
          }
          else
            END = 1;
        }
        else
          END   = 1;
      }
      else
        END     = 1;
    }
    return(ptbux);
  }

  /*case bereg_list */
  if(type==2)
  {
    ptreg = (bereg_list *)pt_list;
    /* Extract the name and number of an element */
    *left       = *right    = -1;
    sig_name    = ptreg->NAME;
    *name       = (char *)mbkalloc(strlen(sig_name) + 1);
    strcpy(*name,sig_name);
    blank_space = strchr(*name,' ');
    if (blank_space != NULL)
    {
      strcpy(number,blank_space);
      *right        = atoi(number);
      *left         = *right;
      *blank_space  = '\0';
    }

    while(END != 1)
    {
      if(ptreg->NEXT != NULL)
      {
        strcpy(name_tmp,ptreg->NEXT->NAME);
        blank_space     = strchr(name_tmp,' ');
        if(blank_space!=NULL)
        {
          strcpy(number,blank_space);
          *blank_space  = '\0';
          if(!strcmp(*name,name_tmp))
          {
            *right      = atoi(number);
            ptreg       = ptreg->NEXT;
          }
          else
            END     = 1;
        }
        else
          END   = 1;
      }
      else
        END     = 1;
    }
    return(ptreg);
  }

  /*case beaux_list */
  if(type==3)
  {
    ptaux       = (beaux_list *)pt_list;
    /* Extract the name and number of an element */
    *left       = *right    = -1;
    sig_name    = ptaux->NAME;
    *name       = (char *)mbkalloc(strlen(sig_name) + 1);
    strcpy(*name,sig_name);
    blank_space = strchr(*name,' ');
    if (blank_space != NULL)
    {
      strcpy(number,blank_space);
      *right        = atoi(number);
      *left         = *right;
      *blank_space  = '\0';
    }

    while(END != 1)
    {
      if(ptaux->NEXT != NULL)
      {
        strcpy(name_tmp,ptaux->NEXT->NAME);
        blank_space     = strchr(name_tmp,' ');
        if(blank_space!=NULL)
        {
          strcpy(number,blank_space);
          *blank_space  = '\0';
          if(!strcmp(*name,name_tmp))
          {
            *right  = atoi(number);
            ptaux   = ptaux->NEXT;
          }
          else
            END     = 1;
        }
        else
          END   = 1;
      }
      else
        END = 1;
    }
    return(ptaux);
  }

  return NULL;
}

/*}}}************************************************************************/
/*{{{                    bsc_printabl()                                     */
/*                                                                          */
/*  put a \n for a better presentation of an abl                            */
/*                                                                          */
/****************************************************************************/
char *bsc_printabl(char *chaine)
{
  char *chaine_tmp = NULL;
  char *blanc = NULL;

  chaine_tmp = chaine;
  while (strlen(chaine_tmp)>60)
  {
    chaine_tmp = &chaine_tmp[60];
    do
    {
      blanc = strchr(chaine_tmp,' ');
      if (blanc != NULL) chaine_tmp = blanc+1;
    }
    while ( blanc &&
            ( *(blanc-1) == '=' || *(blanc+1) == '=' || *(blanc+1) == '('));

    if (blanc)
      *blanc = '\n';
  }
  return (chaine);
}

/*}}}************************************************************************/
/*{{{                    bsc_abl2str()                                      */
/*                                                                          */
/* return a string corresponding to an expression                           */
/*                                                                          */
/****************************************************************************/
char *bsc_abl2str (chain_list *expr, char *chaine, int *size_pnt)
{
  char         *oper    ;
  struct chain *operand ;
  char         *tmp;
  int           i;


  /* ###--------------------------------------------------------### */
  /*    if there is not enough space left allocate a bigger block*/
  /* ###--------------------------------------------------------### */

  if ((size_t)*size_pnt < (strlen (chaine) + 256))
  {
    *size_pnt = *size_pnt + 512;
    chaine  = (char *) mbkrealloc (chaine, *size_pnt);
  }

  /* ###------------------------------------------------------### */
  /*    if the expression is a terminal (atom) add its name to */
  /*         the previous string                                  */
  /* ###------------------------------------------------------### */

  if (expr->NEXT == NULL)
  {
    if (isBitStr(expr->DATA))
    {
      tmp   = bsc_getBitStr(expr->DATA);
      strcat (chaine, tmp);
      mbkfree(tmp);
    }
    else
      strcat (chaine, bsc_vectorize (expr->DATA));
  }

  else
  {
    /* ###------------------------------------------------------### */
    /*    if the expression is not a terminal :                     */
    /*      - for unary operators (not, stable) add                */
    /*        "operator ( operand )" to the previous string         */
    /*                                                              */
    /*      - for binary operators (and, or, nor, xor, ...) add     */
    /*        "(operand operator operand operator operand ...)"    */
    /* ###------------------------------------------------------### */

    operand  = (struct chain *)expr->NEXT->DATA;

    switch (OPER(expr))
    {
      case STABLE :
           bsc_mystrcat(chaine,"%s'STABLE",bsc_vectorize (operand->DATA));
           break;
           // added by anto to compute vectabl
      case REPLICATE:
           strcat (chaine,"(");
           for (i = 0; i < REP_NB(expr); i ++)
           {
             strcat(chaine,bsc_vectorize (operand->DATA));
             if (i != REP_NB(expr)-1)
               strcat(chaine," & ");
           }
           strcat (chaine, ")");
           break;
      case NOT :
           strcat (chaine, "not (");
           chaine = bsc_abl2str (operand, chaine, size_pnt);
           strcat (chaine, ")");
           break;
      default :
           oper = operToChar (OPER(expr));
           strcat (chaine,"(");
           while ( (expr = expr->NEXT) )
           {
             chaine = bsc_abl2str (expr->DATA, chaine, size_pnt);
             if (expr->NEXT)
               bsc_mystrcat(chaine, " %s ",oper);
           }
           strcat (chaine, ")");
    }
  }

  return (chaine);
}

/*}}}************************************************************************/
/*{{{                    bsc_getBitStr()                                    */
/*                                                                          */
/* create an easy to drive copy of the bitstring                            */
/*                                                                          */
/****************************************************************************/
char *bsc_getBitStr(char *str)
{
  char       buf    [4096];
  int        i, n, j;
  
  n     = strlen(str);
  for (i = j = n; i >= 0; i --)
  {
    switch (str[i])
    {
      case 'u' : buf[j--] = 'U';      break;
      case 'z' : buf[j--] = 'Z';      break;
      case 'd' : buf[j--] = 'D';      break;
      default  : buf[j--] = str[i];   break;
    }
  }
  if (n > 18)
    return bsc_bitToHex(buf);
  else
    return mbkstrdup(buf);
}

/*}}}************************************************************************/
/*{{{                    bsc_2puiss()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int bsc_2puiss(int exp)
{
  if (exp)
    return 2 * bsc_2puiss(exp - 1);
  else
    return 1;
}

/*}}}************************************************************************/
/*{{{                    bsc_bitToHex()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
char *bsc_bitToHex(char *str)
{
  char       buf    [512];
  int        i, n, j;
  char       tmp, pos, go;

  n         = strlen(str);
  j         = (n - 2) / 4 + 3 + (((n - 2) % 4) ? 1 : 0);
  
  buf[0]    = 'X';
  tmp       = 0;
  for (i = n; i >= 0; i --)
  {
    switch (str[i])
    {
      case 'u' : case 'z' : case 'd' : case '\'' :
           return str;
      case '"' :
           if ((n-1) - i)
             buf[j--]   = (tmp < 10) ? '0' + tmp : 'A' + tmp % 10 ;
           else
             go         = 0;
           buf[j--]     = '"';
           break;
      case '\0' :
           buf[j--]     = str[i];
           break;
      default :
           if (!(pos = (n - (i+2))% 4))
           {
             if (go)
             {
               buf[j--] = (tmp < 10) ? '0' + tmp : 'A' + tmp % 10 ;
               tmp      = 0;
             }
             else
               go       = 1;
           }
           tmp       += (str[i] - '0') * bsc_2puiss(pos);
    }
  }
  return mbkstrdup(buf);
}

/*}}}************************************************************************/
/*{{{                    bsc_abl2strbool()                                  */
/*                                                                          */
/* This is a modified version of the previous function that is meant        */ 
/*  to be synopsys compliant regarding the boolean nature of a number       */
/*  of variables and signals.                                               */
/*                                                                          */
/****************************************************************************/
char *bsc_abl2strbool (chain_list *expr, char *chaine, int *size_pnt)
{
  char  *oper    ;
  struct chain *operand ;
  int    size;

  /* ###------------------------------------------------------### */
  /*  if there is not enough space left allocate a bigger block   */
  /* ###------------------------------------------------------### */

  if ((size_t)*size_pnt < (strlen (chaine) + 256)) 
  {
    *size_pnt = *size_pnt + 512;
    chaine  = (char *) mbkrealloc (chaine, *size_pnt);
  }

  /* ###------------------------------------------------------### */
  /*    if the expression is a terminal (atom) add its name to    */
  /* the previous string                                          */
  /* ###------------------------------------------------------### */

  if (ATOM(expr)) 
  {
    // added by anto to compute vectabl
    if (isBitStr(expr->DATA)) 
      strcat (chaine, expr->DATA);
    else 
    {
      size = getAtomSize(expr);
      bsc_mystrcat( chaine,"%s = %s", bsc_vectorize (expr->DATA),
                    genOneBitStr(size));
    }
  } 
  else 
  {
    /* ###------------------------------------------------------### */
    /*    if the expression is not a terminal :                     */
    /*      - for unary operators (not, stable) add                 */
    /*        "operator ( operand )" to the previous string         */
    /*                                                              */
    /*      - for binary operators (and, or, nor, xor, ...) add     */
    /*        "(operand operator operand operator operand ...)"     */
    /* ###------------------------------------------------------### */

    operand  = (struct chain *)expr->NEXT->DATA;

    switch (OPER(expr))
    {
      case STABLE :
           bsc_mystrcat(chaine, "not (%s'EVENT)",
                        bsc_vectorize (operand->DATA));
           break;
      case NOT :
           if (operand->NEXT != NULL
               && (long)((chain_list * )operand->DATA)->DATA == STABLE)
           {
             strcat (chaine,
                     bsc_vectorize (((chain_list * )operand->NEXT->DATA)->DATA));
             strcat (chaine, "'EVENT");
           } 
           // added by anto to compute vectabl
           else if (isBitStr(operand->DATA)) 
             strcat (chaine, operand->DATA);
           else
           {
             char          *buf;
             int            bufsize = 512;
             char           buffer[128];

             size = getAtomSize(operand);

             buf    = mbkalloc(bufsize);
             buf[0] = '\0';
             bsc_abl2str(operand,buf,&bufsize);
             sprintf(buffer, "%s = %s", buf, genZeroBitStr(size));
             mbkfree(buf);
             strcat (chaine, buffer);
           }
           break;
           // added by anto to compute vectabl
      case CAT :
           {
             char          *bitstr;
             char          *buf;
             int            bufsize = 512;
             chain_list    *tmpExpr;

             buf    = mbkalloc(bufsize);
             bitstr = beg_boolDelNot(expr,&tmpExpr);

             buf[0] = '\0';
             bsc_abl2str(tmpExpr,buf,&bufsize);
             bsc_mystrcat(chaine, "STD_LOGIC_VECTOR'%s = %s",buf,bitstr);
             mbkfree(buf);
             freeExpr(tmpExpr);
           }
           break ;
      default :
           oper = operToChar (OPER(expr));
           strcat (chaine, "(");
           while ((expr = expr->NEXT)) 
           {
             chaine = bsc_abl2strbool (expr->DATA, chaine, size_pnt);
             if (expr->NEXT) 
               bsc_mystrcat(chaine, " %s ", oper);
           }
           strcat (chaine, ")");
    }
  }
  return (chaine);
}

/*}}}************************************************************************/

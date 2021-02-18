#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include MUT_H
#include LOG_H
#include BEH_H
#include BEG_H
#include BGL_H
#include AVT_H
#include "bgl_type.h"
#include "bgl_error.h"
#include "bgl_util.h"

#ifndef __func__
#define __func__ ""
#endif


static char *vlg_bitToHex(char *str, char *buf);
static int vlg_2puiss(int exp);

/****************************************************************************/
/*{{{                    bgl_mystrcat()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static char *bgl_mystrcat(char *str1, char *str2, ... )
{
  va_list    arg;
  char       buf[2048];

  va_start(arg,str2);
  vsprintf(buf,str2,arg);
  va_end  (arg);
  return strcat(str1,buf);
}

/*}}}************************************************************************/
/*{{{                    bgl_vlogname()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static struct beden **namtab = NULL;
    
void bgl_freenames(void)
{
  beh_fretab(namtab);
  namtab = NULL;
}

char *bgl_vlogname(char *name)
{
  char           *new_name, *tmp_name, *name_policy;
  char            buffer[1024];
  int             i, badname = 0;
  char            c;
  static char    *keywrd[] =
  {
    "always"       , "and"          , "assign"       , "attribute"    ,
    "begin"        , "buf"          , "bufif0"       , "bufif1"       ,
    "case"         , "casex"        , "casez"        , "cmos"         ,
    "deassign"     , "default"      , "defparam"     , "disable"      ,
    "edge"         , "else"         , "end"          , "endattribute" ,
    "endcase"      , "endfunction"  , "endmodule"    , "endprimitive" ,
    "endspecify"   , "endtable"     , "endtask"      , "event"        ,
    "for"          , "force"        , "forever"      , "fork"         ,
    "function"     , "highz0"       , "highz1"       , "if"           ,
    "ifnone"       , "initial"      , "inout"        , "input"        ,
    "integer"      , "join"         , "large"        , "medium"       ,
    "module"       , "macromodule"  , "nand"         , "negedge"      ,
    "nmos"         , "nor"          , "not"          , "notif0"       ,
    "notif1"       , "or"           , "output"       , "parameter"    ,
    "pmos"         , "posedge"      , "primitive"    , "pull0"        ,
    "pull1"        , "pulldown"     , "pullup"       , "rcmos"        ,
    "real"         , "realtime"     , "reg"          , "release"      ,
    "repeat"       , "rnmos"        , "rpmos"        , "rtran"        ,
    "rtranif0"     , "rtranif1"     , "scalared"     , "signed"       ,
    "small"        , "specify"      , "specparam"    , "strength"     ,
    "strong0"      , "strong1"      , "supply0"      , "supply1"      ,
    "table"        , "task"         , "time"         , "tran"         ,
    "tranif0"      , "tranif1"      , "tri"          , "tri0"         ,
    "tri1"         , "triand"       , "trior"        , "trireg"       ,
    "unsigned"     , "vectored"     , "wait"         , "wand"         ,
    "weak0"        , "weak1"        , "while"        , "wire"         ,
    "wor"          , "xnor"         , "xor"          , NULL
  };

  if (namtab == NULL)
  {
    namtab  = beh_initab();
    for (i = 0; keywrd[i]; i++)
      beh_addtab(namtab, namealloc(keywrd[i]), NULL, BGL_NEWDFN, 1);
  }

  tmp_name  = namealloc(name);
  new_name  = (char *) beh_chktab(namtab, tmp_name, NULL, BGL_PNTDFN);

  if (beh_chktab(namtab, tmp_name, NULL, BGL_NAMDFN) == 0)
  {
    if (V_BOOL_TAB[__VERILOG_KEEP_NAMES].VALUE)
    {
      i = 0;
      while ((c = name[i++]) != '\0')
      {
        if (!(isalpha((int)c) || c == '_' || ((i != 1) && (isdigit((int)c) || c == '$'))))
        {
          badname = 1;
          break;
        }
      }
      if (badname || beh_chktab(namtab, name, NULL, BGL_NEWDFN) != 0)
      {
        strcpy(buffer, "\\");
        strcat(buffer, name);
        strcat(buffer, " ");
      }
      else
      {
        strcpy(buffer, name);
      }
      new_name        = namealloc(buffer);

      beh_addtab(namtab, tmp_name, NULL, BGL_PNTDFN, (long) new_name);
      beh_addtab(namtab, tmp_name, NULL, BGL_NAMDFN, 1);
    }
    else
    {
      char  *prv_name;
      int    j, number, flag;

      i       = 0;
      j       = 0;
      number  = 0;
      flag    = 1;
      while (tmp_name[i] != '\0')
      {
        buffer[j]         = tmp_name[i];
        if (((tmp_name[i] >= 'a') && (tmp_name[i] <= 'z')) ||
            ((tmp_name[i] >= 'A') && (tmp_name[i] <= 'Z')) ||
            ((tmp_name[i] >= '0') && (tmp_name[i] <= '9')))
          flag = 0;
        else
        {
          if (flag == 1)
            buffer[j++]   = 'v';
          buffer[j]       = '_';
          flag            = 1;
        }
        if ((tmp_name[i] >= '0') && (tmp_name[i] <= '9') && i == 0)
        {
          buffer[j++]   = 'v';
          buffer[j++]   = '_';
          buffer[j]   = tmp_name[i];
        }
        i++;
        j++;
      }
      if (buffer[j - 1] == '_')
        buffer[j++]   = '0';
      buffer[j]       = '\0';
      new_name        = namealloc(buffer);

      prv_name        = new_name;
      while (beh_chktab(namtab, new_name, NULL, BGL_NEWDFN) != 0)
      {
        new_name      = prv_name;
        sprintf(buffer, "%s_%d", new_name, number++);
        prv_name      = new_name;
        new_name      = namealloc(buffer);
      }
      beh_addtab(namtab, new_name, NULL, BGL_NEWDFN, 1);
      beh_addtab(namtab, tmp_name, NULL, BGL_PNTDFN, (long) new_name);
      beh_addtab(namtab, tmp_name, NULL, BGL_NAMDFN, 1);
    }
  }

  return (new_name);
}

/*}}}************************************************************************/
/*{{{                    bgl_getRadical()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
char *
bgl_getRadical(char *name, int *res)
{
  int        i;
  char      *new_name;
  char       tmp[1024];
  
  i         = 0;
  strcpy(tmp, name);

  while ((name[i] != '\0') && (name[i] != ' ') && (name[i] != '\''))
    i++;

  tmp[i]    = '\0';
  new_name  = bgl_vlogname(tmp);

  if (res)
    *res    = i;

  return new_name;
}

/*}}}************************************************************************/
/* ###--------------------------------------------------------------### */
/* function     : bgl_vectorize                                         */
/* description  : put parenthesis on element of bussed signals          */
/* called func. : mbkalloc                                              */
/* ###--------------------------------------------------------------### */
char *
bgl_vectorize_sub(char *name, char *buf)
{
  char           *new_name;
  char            tmp1[256];
  int             i = 0;
  static struct beden **tab = NULL;

  if (tab == NULL)
    tab = beh_initab();

  if (buf!=NULL || (new_name = (char *) beh_chktab(tab, name, NULL, BGL_PNTDFN)) == NULL)
  {
    new_name    = bgl_getRadical(name,&i);

    if (name[i] != '\0')
    {
      if (name[i] == ' ')
      {
        name[i] = '\0';
        sprintf(tmp1, "%s[%s]", new_name, &name[i + 1]);
        name[i] = ' ';
      }
      else
        sprintf(tmp1, "%s'%s", new_name, &name[i + 1]);
      
      if (buf!=NULL) strcpy(buf, tmp1);
      new_name = namealloc(tmp1);
    }
    else if (buf!=NULL) strcpy(buf, new_name);
                
    if (buf!=NULL) beh_addtab(tab, name, NULL, BGL_PNTDFN, (long)new_name);
  }

  if (buf!=NULL) return buf;
  return (new_name);
}

char * bgl_vectorize(char *name)
{
  return bgl_vectorize_sub(name, NULL);
}

/* ###----------------------------------------------------------------### */
/*  function : bgl_vectnam                                                */
/*  analyze a list of signal and return the bounds of the vectorised      */
/*    signals, if they occure.                                            */
/* attention !!! cette fonction part du principe que ->NAME est toujours  */
/*               le 2 eme champs de chaque structure beXXX                */
/* ###----------------------------------------------------------------### */

void *
bgl_vectnam(void *pt_list, int *left, int *right, char **name, char type)
        /* If type = 0 bepor_list, type = 1 bebux_list  */
        /* If type = 2 bereg_list, type = 3 beaux_list  */
        /* If type = 4 bebus_list */
{
    char           *blank_space;
    char           *sig_name;
    char            name_tmp[200];
    char            number[200];
    bepor_list     *ptpor;
    bebux_list     *ptbux;
    bebus_list     *ptbus;
    beaux_list     *ptaux;
    bereg_list     *ptreg;
    char            END = 0;

    /* Case bepor_list */
    if (type == 0) {
        ptpor = (bepor_list *) pt_list;
        *left = *right = -1;
        sig_name = ptpor->NAME;
        *name = (char *) mbkalloc(strlen(sig_name) + 1);
        strcpy(*name, sig_name);
        blank_space = strchr(*name, ' ');
        if (blank_space != NULL) {
            strcpy(number, blank_space);
            *left = atoi(number);
            *right = *left;
            *blank_space = '\0';
        }

        while (!END) {
            if (ptpor->NEXT != NULL) {
                strcpy(name_tmp, (ptpor->NEXT)->NAME);
                blank_space = strchr(name_tmp, ' ');
                if (blank_space != NULL) {
                    strcpy(number, blank_space);
                    *blank_space = '\0';
                    if (!strcmp(*name, name_tmp)) {
                        *right = atoi(number);
                        ptpor = ptpor->NEXT;
                    }
                    else
                        END = 1;
                }
                else
                    END = 1;
            }
            else
                END = 1;
        }
        return (ptpor);
    }

    /*case bebux_list */
    if (type == 1) {
        ptbux = (bebux_list *) pt_list;
        /* Extract the name and number of an element */
        *left = *right = -1;
        sig_name = ptbux->NAME;
        *name = (char *) mbkalloc(strlen(sig_name) + 1);
        strcpy(*name, sig_name);
        blank_space = strchr(*name, ' ');
        if (blank_space != NULL) {
            strcpy(number, blank_space);
            *right = atoi(number);
            *left = *right;
            *blank_space = '\0';
        }

        while (END != 1) {
            if (ptbux->NEXT != NULL) {
                strcpy(name_tmp, ptbux->NEXT->NAME);
                blank_space = strchr(name_tmp, ' ');
                if (blank_space != NULL) {
                    strcpy(number, blank_space);
                    *blank_space = '\0';
                    if (!strcmp(*name, name_tmp)) {
                        *right = atoi(number);
                        ptbux = ptbux->NEXT;
                    }
                    else
                        END = 1;
                }
                else
                    END = 1;
            }
            else
                END = 1;
        }
        return (ptbux);
    }

    /*case bereg_list */
    if (type == 2) {
        ptreg = (bereg_list *) pt_list;
        /* Extract the name and number of an element */
        *left = *right = -1;
        sig_name = ptreg->NAME;
        *name = (char *) mbkalloc(strlen(sig_name) + 1);
        strcpy(*name, sig_name);
        blank_space = strchr(*name, ' ');
        if (blank_space != NULL) {
            strcpy(number, blank_space);
            *right = atoi(number);
            *left = *right;
            *blank_space = '\0';
        }

        while (END != 1) {
            if (ptreg->NEXT != NULL) {
                strcpy(name_tmp, ptreg->NEXT->NAME);
                blank_space = strchr(name_tmp, ' ');
                if (blank_space != NULL) {
                    strcpy(number, blank_space);
                    *blank_space = '\0';
                    if (!strcmp(*name, name_tmp)) {
                        *right = atoi(number);
                        ptreg = ptreg->NEXT;
                    }
                    else
                        END = 1;
                }
                else
                    END = 1;
            }
            else
                END = 1;
        }
        return (ptreg);
    }

    /*case beaux_list */
    if (type == 3) {
        ptaux = (beaux_list *) pt_list;
        /* Extract the name and number of an element */
        *left = *right = -1;
        sig_name = ptaux->NAME;
        *name = (char *) mbkalloc(strlen(sig_name) + 1);
        strcpy(*name, sig_name);
        blank_space = strchr(*name, ' ');
        if (blank_space != NULL) {
            strcpy(number, blank_space);
            *right = atoi(number);
            *left = *right;
            *blank_space = '\0';
        }

        while (END != 1) {
            if (ptaux->NEXT != NULL) {
                strcpy(name_tmp, ptaux->NEXT->NAME);
                blank_space = strchr(name_tmp, ' ');
                if (blank_space != NULL) {
                    strcpy(number, blank_space);
                    *blank_space = '\0';
                    if (!strcmp(*name, name_tmp)) {
                        *right = atoi(number);
                        ptaux = ptaux->NEXT;
                    }
                    else
                        END = 1;
                }
                else
                    END = 1;
            }
            else
                END = 1;
        }
        return (ptaux);
    }

    /*case bebux_list */
    if (type == 4) {
        ptbus = (bebus_list *) pt_list;
        /* Extract the name and number of an element */
        *left = *right = -1;
        sig_name = ptbus->NAME;
        *name = (char *) mbkalloc(strlen(sig_name) + 1);
        strcpy(*name, sig_name);
        blank_space = strchr(*name, ' ');
        if (blank_space != NULL) {
            strcpy(number, blank_space);
            *right = atoi(number);
            *left = *right;
            *blank_space = '\0';
        }

        while (END != 1) {
            if (ptbus->NEXT != NULL) {
                strcpy(name_tmp, ptbus->NEXT->NAME);
                blank_space = strchr(name_tmp, ' ');
                if (blank_space != NULL) {
                    strcpy(number, blank_space);
                    *blank_space = '\0';
                    if (!strcmp(*name, name_tmp)) {
                        *right = atoi(number);
                        ptbus = ptbus->NEXT;
                    }
                    else
                        END = 1;
                }
                else
                    END = 1;
            }
            else
                END = 1;
        }
        return (ptbus);
    }

    return NULL;
}

/* ###--------------------------------------------------------------### */
/*  function : bgl_printabl                                             */
/*  content  : put a \n for a better presentation of an abl             */
/* ###--------------------------------------------------------------### */
char           *
bgl_printabl(char *chaine)
{
    char           *chaine_tmp = NULL;
    char           *blanc = NULL;

    chaine_tmp = chaine;
    while (strlen(chaine_tmp) > 60) {
        chaine_tmp = &chaine_tmp[60];
        do {
            blanc = strchr(chaine_tmp, ' ');
            if (blanc != NULL)
                chaine_tmp = blanc + 1;
        } while (blanc != NULL
                 && (*(blanc - 1) == '=' || *(blanc + 1) == '='
                     || *(blanc + 1) == '['));

        if (blanc != NULL) {
            *blanc = '\n';
        }
    }
    return (chaine);
}

/****************************************************************************/
/*{{{                    bgl_getBitStr()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
char *bgl_getBitStr(char *bitStr, char *buf)
{
  int        i, n, j;

  n     = strlen(bitStr);
  sprintf(buf, "%d'b", n - 2);
  
  j     = strlen(buf);
  
  for (i = 0; i <= n; i ++)
  {
    switch (bitStr[i])
    {
      case 'u' : buf[j++] = 'x';        break;
      case 'z' : buf[j++] = 'z';        break;
      case 'd' : buf[j++] = 'd';        break;
      case '\'': case '"' :             break; 
      default  : buf[j++] = bitStr[i];  break;
    }
  }

  if (n > 18)
    return vlg_bitToHex(bitStr,buf);
  else
    return buf;
}

/*}}}************************************************************************/
/*{{{                    vlg_2puiss()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int vlg_2puiss(int exp)
{
  if (exp)
    return 2 * vlg_2puiss(exp - 1);
  else
    return 1;
}

/*}}}************************************************************************/
/*{{{                    vlg_bitToHex()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
char *vlg_bitToHex(char *str, char *res)
{
  char       buf[512];
  int        i, n, j;
  char       tmp, pos, go;

  n         = strlen(str);
  j         = (n - 2) / 4 + 1 + (((n - 2) % 4) ? 1 : 0);
  
  sprintf(res, "%d'", n - 2);
  buf[0]    = 'h';
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
           tmp       += (str[i] - '0') * vlg_2puiss((int)pos);
    }
  }
  strcat(res,buf);
  
  return res;
}

/*}}}************************************************************************/

/* ###--------------------------------------------------------------### */
/* function     : bgl_abl2str                                           */
/* description  : return a string corresponding to an expression        */
/* called func. :                                                       */
/* ###--------------------------------------------------------------### */

char           *
bgl_abl2str(chain_list * expr, char *chaine, int *size_pnt)
{
  char           *oper;
  char           *tmp;
  char           bufbit[4096];

  /* ###--------------------------------------------------------### */
  /*    if there is not enough space left allocate a bigger block */
  /* ###--------------------------------------------------------### */

  if ((size_t)*size_pnt < (strlen(chaine) + 256))
  {
    *size_pnt = *size_pnt + 512;
    chaine = (char *) mbkrealloc(chaine, *size_pnt);
  }

  /* ###------------------------------------------------------### */
  /*    if the expression is a terminal (atom) add its name to     */
  /*         the previous string                                  */
  /* ###------------------------------------------------------### */

  if (ATOM(expr))
    if (isBitStr(expr->DATA))
    {
      tmp       = bgl_getBitStr(expr->DATA,bufbit);
      strcat(chaine,tmp);
    }
    else
      strcat(chaine, bgl_vectorize_sub(expr->DATA, bufbit));
  else
  {
    /* ###------------------------------------------------------### */
    /*    if the expression is not a terminal :                     */
    /*      - for unary operators (not, stable) add                 */
    /*        "operator ( operand )" to the previous string         */
    /*                                                              */
    /*      - for binary operators (and, or, nor, xor, ...) add     */
    /*        "(operand operator operand operator operand ...)"         */
    /* ###------------------------------------------------------### */
    
    switch (OPER(expr))
    {
      case NOT : case NOR : case NAND :
           strcat(chaine, "~(");
           oper = bgl_oper2char(OPER(expr));
           while ((expr = expr->NEXT) != NULL)
           {
             chaine = bgl_abl2str(expr->DATA, chaine, size_pnt);
             if (expr->NEXT)
               bgl_mystrcat(chaine, " %s ",oper);
           }
           strcat(chaine, ")");
           break;
      case CAT :
           bgl_mystrcat(chaine,"{");
           while ((expr = expr->NEXT))
           {
             chaine = bgl_abl2str(expr->DATA, chaine, size_pnt);
             if (expr->NEXT)
               bgl_mystrcat(chaine," , ");
           }
           bgl_mystrcat(chaine,"}");
           break;
      case REPLICATE :
           bgl_mystrcat(chaine,"{ %d {",REP_NB(expr));
           while ((expr = expr->NEXT))
           {
             chaine = bgl_abl2str(expr->DATA, chaine, size_pnt);
             if (expr->NEXT)
               strcat(chaine," , ");
           }
           strcat(chaine,"}}");
           break;
      default :
           oper = bgl_oper2char(OPER(expr));
           strcat(chaine, "(");
           while ((expr = expr->NEXT) != NULL)
           {
             chaine = bgl_abl2str(expr->DATA, chaine, size_pnt);
             if (expr->NEXT)
               bgl_mystrcat(chaine, " %s ",oper);
           }
           strcat(chaine, ")");
    }
  }
  return (chaine);
}

/* This is a modified version of the previous function that is meant      */
/*  to be synopsys compliant regarding the boolean nature of a number     */
/*  of variables and signals.                                             */

char *
bgl_abl2strbool(chain_list * expr, char *chaine, int *size_pnt)
{
  char          *oper;
  struct chain  *operand;
  char          *tmp;
  char           bufbit[4096];
  char           bufbit1[2048];
  
  /* ###------------------------------------------------------### */
  /*    if there is not enough space left allocate a bigger block   */
  /* ###------------------------------------------------------### */

  if ((size_t)*size_pnt < (strlen(chaine) + 256))
  {
    *size_pnt = *size_pnt + 512;
    chaine = (char *) mbkrealloc(chaine, *size_pnt);
  }

  /* ###------------------------------------------------------### */
  /*    if the expression is a terminal (atom) add its name to   */
  /* the previous string                                         */
  /* ###------------------------------------------------------### */

  if (ATOM(expr))
    if (isBitStr(expr->DATA))
    {
      tmp = bgl_getBitStr(expr->DATA,bufbit);
      strcat(chaine,tmp);
    }
    else
    {
      char       buffer[128];
      int        size;
      
      size = getAtomSize(expr);
      tmp  = bgl_getBitStr(genOneBitStr(size),bufbit);
      sprintf(buffer, "%s === %s", bgl_vectorize_sub(expr->DATA, bufbit1),tmp);
      strcat (chaine, buffer);
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

    operand  = CADR(expr);

    switch (OPER(expr))
    {
      case NOT  :
           // added by anto to compute vectabl
           if (isBitStr(operand->DATA)) 
             strcat (chaine, operand->DATA);
           else
           {
             char      *buf;
             int        bufsize = 512;
             int        size;

             buf    = mbkalloc(bufsize);
             buf[0] = '\0';
             size   = getAtomSize(operand);
             tmp    = bgl_getBitStr(genZeroBitStr(size),bufbit);
             bgl_abl2str(operand, buf,&bufsize);
             bgl_mystrcat(chaine, "%s === %s",buf ,tmp);
             mbkfree(buf);
           }
           break;
      case CAT  :
           {
             char          *bitstr;
             char          *buf;
             int            bufsize = 512;
             chain_list    *tmpExpr;

             buf    = mbkalloc(bufsize);
             bitstr = bgl_getBitStr(beg_boolDelNot(expr,&tmpExpr),bufbit);

             buf[0] = '\0';
             bgl_abl2str(tmpExpr,buf,&bufsize);
             bgl_mystrcat(chaine,"%s === %s",buf,bitstr);
             mbkfree(buf);
             freeExpr(tmpExpr);
           }
           break;
      case REPLICATE :
           bgl_mystrcat(chaine,"{ %d {",REP_NB(expr));
           while ((expr = expr->NEXT))
           {
             chaine = bgl_abl2str(expr->DATA, chaine, size_pnt);
             if (expr->NEXT)
               strcat(chaine," , ");
           }
           strcat(chaine,"}}");
           break;
      default :
           oper = bgl_oper2charbool(OPER(expr));
           strcat(chaine, "(");
           while ((expr = expr->NEXT) != NULL)
           {
             chaine = bgl_abl2strbool(expr->DATA, chaine, size_pnt);
             if (expr->NEXT)
               bgl_mystrcat(chaine, " %s ",oper);
           }
           strcat(chaine, ")");
    }
  }
  return (chaine);
}

/* ###--------------------------------------------------------------### */
/* function     : bgl_oper2char                                         */
/* description  : return a string corresponding to a bitwise operator   */
/* ###--------------------------------------------------------------### */

char *
bgl_oper2char(short oper)
{
  switch (oper)
  {
    case NOT :
         return (namealloc("~"));
    case NAND : case AND :
         return (namealloc("&"));
    case NOR : case OR  :
         return (namealloc("|"));
    case XOR :
         return (namealloc("^"));
    case NXOR :
         return (namealloc("~^"));
    default :
         avt_errmsg(BGL_ERRMSG,"000",AVT_FATAL,"200");
//         printf("\nablError - %s: unknown operator %d\n",__func__,oper);
//         EXIT(-1);
         return NULL;
  }
}

/* ###--------------------------------------------------------------### */
/* function     : bgl_oper2charbool                                     */
/* description  : return a string corresponding to a logical operator   */
/* ###--------------------------------------------------------------### */

char *
bgl_oper2charbool(short oper)
{
  switch (oper)
  {
    case NOT:
         return (namealloc("!"));
    case NAND: case AND:
         return (namealloc("&&"));
    case NOR: case OR:
         return (namealloc("||"));
    default:
         avt_errmsg(BGL_ERRMSG,"000",AVT_FATAL,"201");
//         printf("\nablError %s: unknown operator %d\n",__func__,oper);
//         EXIT(-1);
         return NULL;
  }
}

int
bgl_writeprimitives(FILE *fp, char *name, chain_list *ptabl, unsigned int delay)
{
    chain_list *expr;
    int         depth;
    char buf0[2048];
    char buf1[2048];

    depth = profExpr(ptabl);
    if (depth == 0) {
        fprintf(fp, "    buf ");
        if (delay != 0) fprintf(fp, "#%u ", delay);
        fprintf(fp, "(%s, %s);\n", bgl_vectorize_sub(name, buf0), bgl_vectorize_sub(VALUE_ATOM(ptabl), buf1));
        return TRUE;
    }
    if (!BGL_ONLY_PRIMITIVES && depth > 1) return FALSE;

    if (depth == 1) {
        switch (OPER(ptabl)) {
            case OR:
                fprintf(fp, "    or "); break;
            case AND:
                fprintf(fp, "    and "); break;
            case XOR:
                fprintf(fp, "    xor "); break;
            case NOT:
                fprintf(fp, "    not "); break;
            case NOR:
                fprintf(fp, "    nor "); break;
            case NAND:
                fprintf(fp, "    nand "); break;
            case NXOR:
                fprintf(fp, "    nxor "); break;
        }
        if (delay != 0) fprintf(fp, "#%u ", delay);
        fprintf(fp, "(%s", bgl_vectorize_sub(name, buf0));
        for (expr = CDR(ptabl); expr; expr = CDR(expr)) {
            fprintf(fp, ", %s", bgl_vectorize_sub(VALUE_ATOM(CAR(expr)), buf0));
        }
        fprintf(fp, ");\n");
        return TRUE;
    }
    return FALSE;
}

#if 0
/* ###--------------------------------------------------------------### */
/* function     : bgl_addgen                                            */
/* description  : create one or more BEGEN structures                   */
/*                For a scalar a BEGEN is created at the head of        */
/*                existing BEGEN list.                                  */
/*                For an array (including arraies of one element) a set */
/*                of BEGENs are created in a sorted list. BEGEN related */
/*                to the index i of the array is named `name(i)`. The   */
/*                head of the list represents the right bound of the    */
/*                array. This list is then chained to the head of       */
/*                existing BEGEN list.                                  */
/* called func. : beh_addbegen, namealloc                               */
/* ###--------------------------------------------------------------### */

struct begen   *
bgl_addgen(struct begen *lastgeneric, chain_list * nat_lst,
           chain_list * nam_lst, char *type, short left, short right, bgl_bcompcontext *context)

    /* pointer on the last begen structure      */
    /* generic's value list                     */
    /* generic's name list                      */
    /* generic's type                   */
    /* array's left bound (= -1 if scalar)      */
    /* array's right bound (= -1 if scalar)     */
{
    char            extname[100];
    char           *name;
    struct begen   *ptgen;
    struct chain   *ptauxnam;
    struct chain   *ptauxnat;
    int             i;
    int             inc = 1;

    ptgen = lastgeneric;
    ptauxnam = nam_lst;
    ptauxnat = nat_lst;

    if ((left == -1) && (right == -1))
        if ((ptauxnat != NULL) && (ptauxnat->NEXT == NULL))
            while (ptauxnam != NULL) {
                name = namealloc((char *) ptauxnam->DATA);
                ptgen =
                    beh_addbegen(ptgen, name, type, (void *) ptauxnat->DATA);
                ptauxnam = ptauxnam->NEXT;
            }
        else
            bgl_error(75, NULL, context);
    else {
        if (left >= right)
            inc = -1;
        while (ptauxnam != NULL) {
            for (i = left; i != (right + inc); i += inc) {
                sprintf(extname, "%s %d", (char *) ptauxnam->DATA, i);
                name = namealloc(extname);
                if (ptauxnat != NULL) {
                    ptgen =
                        beh_addbegen(ptgen, name, type,
                                     (void *) ptauxnat->DATA);
                    ptauxnat = ptauxnat->NEXT;
                }
                else
                    bgl_error(75, NULL, context);
            }
            if (ptauxnat != NULL)
                bgl_error(75, NULL, context);
            ptauxnat = nat_lst;
            ptauxnam = ptauxnam->NEXT;
        }
    }
    return (ptgen);
}
#endif

/* ###--------------------------------------------------------------### */
/* function     : bgl_cpyabllst                                         */
/* description  : duplicate bgl_abllst structure                        */
/* called func. : addchain, reverse, copyExpr                           */
/* ###--------------------------------------------------------------### */

chain_list     *
bgl_cpyabllst(chain_list * abllst)
{
    struct chain   *ptabllst = NULL;

    while (abllst != NULL) {
        ptabllst =
            addchain(ptabllst, copyExpr((struct chain *) abllst->DATA));
        abllst = abllst->NEXT;
    }
    ptabllst = reverse(ptabllst);
    return (ptabllst);
}

/* ###--------------------------------------------------------------### */
/* function     : bgl_cpyablstr                                         */
/* description  : duplicate bgl_expr structure                           */
/* called func. : mbkalloc <mbk>, bgl_cpyabllst                         */
/* ###--------------------------------------------------------------### */

bgl_expr
bgl_cpyablstr(bgl_expr ablstr)
{
    bgl_expr        ptablstr;

    ptablstr.IDENT = NULL;
    ptablstr.WIDTH = ablstr.WIDTH;
    ptablstr.LIST_ABL = bgl_cpyabllst(ablstr.LIST_ABL);
    return (ptablstr);
}

/* ###--------------------------------------------------------------### */
/* function     : mgl_delcaseht                                         */
/* ###--------------------------------------------------------------### */

void
bgl_delcaseht(ht * caseht)
{
    htitem         *ptelem = caseht->pElem;
    long            i;

    if (caseht != NULL) {
        for (i = 0; i < caseht->length; i++) {
            if (ptelem->value != EMPTYHT && ptelem->value != DELETEHT) {
                mbkfree((char *) ptelem->value);
            }
            ptelem++;
        }
        delht(caseht);
    }
}

/* ###--------------------------------------------------------------### */
/* function     : mgl_getcasename                                       */
/* ###--------------------------------------------------------------### */

static char    *
casecode(str)
    char           *str;
{
    char            buffer[1024];
    int             i = 0;
    int             j = 0;

    strcpy(buffer, "cc_");
    j += 3;
    while (str[i] != 0) {
        if (isupper((int)str[i]))
            buffer[j++] = 'u';
        buffer[j++] = str[i++];
    }
    buffer[j] = 0;

    return namealloc(buffer);
}

char           *
bgl_getcasename(ht * caseht, char *text)
{
    char           *ptname, *ptcasename;

    ptname = namealloc(text);
    if ((ptcasename = (char *) gethtitem(caseht, ptname)) == (char *) EMPTYHT) {
        addhtitem(caseht, ptname, (long) mbkstrdup(text));
        return (ptname);
    }
    else if (strcmp(ptcasename, text) != 0) {
        return (casecode(text));
    }
    else
        return (ptname);
}

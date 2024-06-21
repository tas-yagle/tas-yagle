
/* ###--------------------------------------------------------------### */
/* file         : bvl_util.c                                            */
/* date         : Oct 30 1995                                           */
/* version      : v111                                                  */
/* author(s)    : Pirouz BAZARGAN SABET                                 */
/*                Antony PINTO                                          */
/* description  : This file contains some utility functions :           */
/*                bvl_error  , bvl_y_error                              */
/* ###--------------------------------------------------------------### */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>
#include MUT_H
#include LOG_H
#include BEH_H
#include BEG_H
#include AVT_H
#include "bvl_utype.h"
#include "bvl_util.h"
#include "bvl_utdef.h"

extern char              *bgl_vlogname (char *name);

char BVL_CURFIL[200];

static int vhd_2puiss(int exp);

/****************************************************************************/
/*{{{                    bvl_error()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void bvl_error (int code, char *str1)
{
  BVL_ERRFLG++;
//  fprintf (stderr,"`%s` Error %d line %d :",BVL_CURFIL,code,BEH_LINNUM);

  switch (code)
  {
    case 1:
         avt_errmsg(BVL_ERRMSG,"001",AVT_ERROR,BVL_CURFIL,BEH_LINNUM,str1);
//         fprintf (stderr,"`%s` is incompatible with the entity name\n",str1);
         break;
    case 2:
         avt_errmsg(BVL_ERRMSG,"002",AVT_ERROR,BVL_CURFIL,BEH_LINNUM);
//         fprintf (stderr,"bad entity declaration\n");
         break;
    case 3:
         avt_errmsg(BVL_ERRMSG,"003",AVT_ERROR,BVL_CURFIL,BEH_LINNUM);
//         fprintf (stderr,"bad port clause declaration\n");
         break;
    case 4:
         avt_errmsg(BVL_ERRMSG,"004",AVT_ERROR,BVL_CURFIL,BEH_LINNUM,str1);
//         fprintf (stderr,"port `%s` already declared\n",str1);
         break;
    case 5:
         avt_errmsg(BVL_ERRMSG,"005",AVT_ERROR,BVL_CURFIL,BEH_LINNUM);
//         fprintf (stderr,"illegal port declaration (mode, type, kind)\n");
         break;
    case 6:
         avt_errmsg(BVL_ERRMSG,"006",AVT_ERROR,BVL_CURFIL,BEH_LINNUM);
//         fprintf (stderr,"bad port declaration\n");
         break;
    case 7:
         avt_errmsg(BVL_ERRMSG,"007",AVT_ERROR,BVL_CURFIL,BEH_LINNUM,str1);
//         fprintf (stderr,"`%s` is incompatible with the architecture name\n",str1);
         break;
    case 8:
         avt_errmsg(BVL_ERRMSG,"008",AVT_ERROR,BVL_CURFIL,BEH_LINNUM);
//         fprintf (stderr,"bad architecture declaration\n");
         break;
    case 9:
         avt_errmsg(BVL_ERRMSG,"009",AVT_ERROR,BVL_CURFIL,BEH_LINNUM);
//         fprintf (stderr,"illegal declaration\n");
         break;
    case 10:
         avt_errmsg(BVL_ERRMSG,"010",AVT_ERROR,BVL_CURFIL,BEH_LINNUM,str1);
//         fprintf (stderr,"signal `%s` already declared\n",str1);
         break;
    case 11:
         avt_errmsg(BVL_ERRMSG,"011",AVT_ERROR,BVL_CURFIL,BEH_LINNUM);
//         fprintf (stderr,"illegal signal declaration (type, kind)\n");
         break;
    case 17:
         avt_errmsg(BVL_ERRMSG,"012",AVT_ERROR,BVL_CURFIL,BEH_LINNUM,str1);
//         fprintf (stderr,"`%s` unknown port or signal\n",str1);
         break;
    case 18:
         avt_errmsg(BVL_ERRMSG,"013",AVT_ERROR,BVL_CURFIL,BEH_LINNUM);
//         fprintf (stderr,"illegal concurrent statement\n");
         break;
    case 19:
         avt_errmsg(BVL_ERRMSG,"014",AVT_ERROR,BVL_CURFIL,BEH_LINNUM,str1);
//         fprintf (stderr,"label `%s` already declared\n",str1);
         break;
    case 20:
         avt_errmsg(BVL_ERRMSG,"015",AVT_ERROR,BVL_CURFIL,BEH_LINNUM,str1);
//         fprintf (stderr,"`%s` is incompatible with the block's label\n",str1);
         break;
    case 21:
         avt_errmsg(BVL_ERRMSG,"016",AVT_ERROR,BVL_CURFIL,BEH_LINNUM,str1);
//         fprintf (stderr,"input port `%s` cannot be assigned\n",str1);
         break;
    case 22:
         avt_errmsg(BVL_ERRMSG,"017",AVT_ERROR,BVL_CURFIL,BEH_LINNUM,str1);
//         fprintf (stderr,"illegal unguarded signal assignment for `%s`\n", str1);
         break;
    case 23:
         avt_errmsg(BVL_ERRMSG,"018",AVT_ERROR,BVL_CURFIL,BEH_LINNUM,str1);
//         fprintf (stderr,"ilegal guarded signal assignment `%s`\n",str1);
         break;
    case 25:
         avt_errmsg(BVL_ERRMSG,"019",AVT_ERROR,BVL_CURFIL,BEH_LINNUM);
//         fprintf (stderr,"some choices missing in the selected signal assignment\n");
         break;
    case 26:
         avt_errmsg(BVL_ERRMSG,"020",AVT_ERROR,BVL_CURFIL,BEH_LINNUM,str1);
//         fprintf (stderr,"output port `%s` cannot be read\n",str1);
         break;
    case 28:
         avt_errmsg(BVL_ERRMSG,"021",AVT_ERROR,BVL_CURFIL,BEH_LINNUM);
//         fprintf (stderr,"duplicate choice in selected signal assignment\n");
         break;
    case 30:
         avt_errmsg(BVL_ERRMSG,"022",AVT_ERROR,BVL_CURFIL,BEH_LINNUM);
//         fprintf (stderr,"illegal use of OTHERS in selected signal assignment\n");
         break;
    case 32:
         avt_errmsg(BVL_ERRMSG,"023",AVT_ERROR,BVL_CURFIL,BEH_LINNUM);
//         fprintf (stderr,"null array not supported\n");
         break;
    case 33:
         avt_errmsg(BVL_ERRMSG,"024",AVT_ERROR,BVL_CURFIL,BEH_LINNUM);
//         fprintf (stderr,"incompatible constraint and type\n");
         break;
    case 35:
         avt_errmsg(BVL_ERRMSG,"025",AVT_ERROR,BVL_CURFIL,BEH_LINNUM,str1);
//         fprintf (stderr,"illegal assignment of `%s` (widths mismatch)\n",str1);
         break;
    case 36:
         avt_errmsg(BVL_ERRMSG,"026",AVT_ERROR,BVL_CURFIL,BEH_LINNUM,str1);
//         fprintf (stderr,"signal `%s` used out of declared range\n",str1);
         break;
    case 38:
         avt_errmsg(BVL_ERRMSG,"027",AVT_ERROR,BVL_CURFIL,BEH_LINNUM);
//         fprintf (stderr,"width or/and type mismatch\n");
         break;
    case 39:
         avt_errmsg(BVL_ERRMSG,"028",AVT_ERROR,BVL_CURFIL,BEH_LINNUM,str1);
//         fprintf (stderr,"signal `%s` assigned more than once\n",str1);
         break;
    case 40:
         avt_errmsg(BVL_ERRMSG,"029",AVT_ERROR,BVL_CURFIL,BEH_LINNUM,str1);
//         fprintf (stderr,"signal `%s` never assigned\n",str1);
         break;
    case 41:
         avt_errmsg(BVL_ERRMSG,"030",AVT_ERROR,BVL_CURFIL,BEH_LINNUM,str1);
//         fprintf (stderr,"illegal condition on signal `%s`\n",str1);
         break;

    case 68:
         avt_errmsg(BVL_ERRMSG,"031",AVT_ERROR,BVL_CURFIL,BEH_LINNUM);
//         fprintf (stderr,"BEPOR type is unknown\n");
         break;
    case 73:
         avt_errmsg(BVL_ERRMSG,"032",AVT_ERROR,BVL_CURFIL,BEH_LINNUM,str1);
//         fprintf (stderr,"`%s` is not a bit string litteral\n",str1);
         break;
    case 74:
         avt_errmsg(BVL_ERRMSG,"033",AVT_ERROR,BVL_CURFIL,BEH_LINNUM);
//         fprintf (stderr,"bad generic declaration\n");
         break;
    case 75:
         avt_errmsg(BVL_ERRMSG,"034",AVT_ERROR,BVL_CURFIL,BEH_LINNUM);
//         fprintf (stderr,"bad generic element\n");
         break;
    case 76:
         avt_errmsg(BVL_ERRMSG,"035",AVT_ERROR,BVL_CURFIL,BEH_LINNUM,str1);
//         fprintf (stderr,"`%s`: when expression must be a constant\n",str1);
         break;
    case 77:
         avt_errmsg(BVL_ERRMSG,"036",AVT_ERROR,BVL_CURFIL,BEH_LINNUM);
//         fprintf (stderr,"illegal generic declaration (type, kind)\n");
         break;
    case 78:
         avt_errmsg(BVL_ERRMSG,"037",AVT_ERROR,BVL_CURFIL,BEH_LINNUM);
//         fprintf (stderr,"illegal constant declaration (type, kind)\n");
         break;
    case 79:
         avt_errmsg(BVL_ERRMSG,"038",AVT_ERROR,BVL_CURFIL,BEH_LINNUM,str1);
//         fprintf (stderr,"illegal use of attribute STABLE on `%s`\n",str1);
         break;
    case 80:
         avt_errmsg(BVL_ERRMSG,"039",AVT_ERROR,BVL_CURFIL,BEH_LINNUM);
//         fprintf (stderr,"different delays not supported on waveforms\n");
         break;

    default:
         avt_errmsg(BVL_ERRMSG,"040",AVT_ERROR,BVL_CURFIL,BEH_LINNUM);
//         fprintf (stderr,"syntax error\n");
         break;
  }

  if (BVL_ERRFLG > V_INT_TAB[__VHDL_MAXERR].VALUE)
  {
//    fprintf (stderr,"Too many errors. Cannot continue further more\n");
//    fprintf (stderr,"\nHave a nice day...\n");
//    EXIT (1);
    avt_errmsg(BVL_ERRMSG,"041",AVT_FATAL);
  }

}

/*}}}************************************************************************/
/*{{{                    vbe_bcomperror()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void vbe_bcomperror(char *str)
{
  BVL_ERRFLG++;
  avt_errmsg(BVL_ERRFLG,"042",AVT_ERROR,BVL_CURFIL,BEH_LINNUM,str);
//  fprintf(stderr,"`%s` Error line %d : %s\n",BVL_CURFIL,BEH_LINNUM,str);
}

/*}}}************************************************************************/
/*{{{                    vhd_bcomperror()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void vhd_bcomperror(char *str)
{
  BVL_ERRFLG++;
  avt_errmsg(BVL_ERRFLG,"042",AVT_ERROR,BVL_CURFIL,BEH_LINNUM,str);
  //fprintf(stderr,"`%s` Error line %d : %s\n",BVL_CURFIL,BEH_LINNUM,str);
}

/*}}}************************************************************************/
/*{{{                    bgl_mystrcat()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static char *bvl_mystrcat(char *str1, char *str2, ... )
{
  va_list    arg;
  char       buf[1024];

  va_start(arg,str2);
  vsprintf(buf,str2,arg);
  va_end  (arg);

  return strcat(str1,buf);
}

/*}}}************************************************************************/
/*{{{                    bvl_vhdlname()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static struct beden **namtab=NULL;

void bvl_freenames(void)
{
  beh_fretab(namtab);
  namtab = NULL;
}

char *bvl_vhdlname_sub (char *name, char *buf)
{
  char                 *new_name;
  char                 *prv_name;
  char                 *tmp_name;
  int                   i,j,flag,number;
  char buffer[1024];
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
      beh_addtab (namtab, namealloc (keywrd [i]), NULL, BVL_NEWDFN , 1);
  }

  if (buf!=NULL) strcpy(buf, name);
  tmp_name = namealloc (name);
  new_name = (char *) beh_chktab (namtab,tmp_name,NULL,BVL_PNTDFN);

  if (buf!=NULL || beh_chktab (namtab,tmp_name,NULL,BVL_NAMDFN) == 0)
  {
    i       = 0;
    j       = 0;
    number  = 0;
    flag    = 1;
    while (tmp_name[i] != '\0')
    {
      if (i == 0 && isdigit((int)tmp_name[i])) {
          buffer[j++] = 'v';
          buffer[j++] = '_';
      }
      buffer[j] = tmp_name[i];
      if ( ((tmp_name[i] >= 'a') && (tmp_name[i] <= 'z')) ||
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
      i++;
      j++;
    }
    if (buffer[j-1] == '_')
      buffer[j++]   = '0';
    buffer[j]       = '\0';
    if (buf!=NULL) strcpy(buf,buffer);
    new_name        = namealloc (buffer);

    prv_name        = new_name;
    while (beh_chktab (namtab,new_name,NULL,BVL_NEWDFN) != 0)
    {
      new_name      = prv_name;
      sprintf (buffer,"%s_%d",new_name,number++);
      prv_name      = new_name;
      if (buf!=NULL) strcpy(buf,buffer);
      new_name      = namealloc (buffer);
    }
    beh_addtab (namtab,new_name,NULL,BVL_NEWDFN,1);
    beh_addtab (namtab,tmp_name,NULL,BVL_PNTDFN,(long)new_name);
    beh_addtab (namtab,tmp_name,NULL,BVL_NAMDFN,1);
  }

  return (new_name);
}

char *bvl_vhdlname (char *name)
{
  char buf[1024];
  return bvl_vhdlname_sub(name, NULL);
}

/*}}}************************************************************************/
/*{{{                    bvl_vectorize()                                                   */
/*                                                                          */
/* description : put parenthesis on element of bussed signals               */
/* called func.: mbkalloc                                                   */
/*                                                                          */
/****************************************************************************/
char *bvl_vectorize(char *name)
{
  char *new_name  ;
  char  tmp1[256];
  int   i          = 0;
  int   j          = 0;
  int   left;
  int   right;
  static struct beden **tab = NULL;

  if (tab == NULL)
    tab     = beh_initab ();

  new_name  = (char *) beh_chktab (tab, name, NULL, BVL_PNTDFN);
  if (new_name == NULL)
  {
    strcpy (tmp1, name);

    while ((name [i] != '\0') && (name [i] != ' ' ) && (name [i] != '\''))
      i ++;

    tmp1 [i]    = '\0';
    new_name    = bvl_vhdlname (tmp1);

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
    beh_addtab(tab,name,NULL,BVL_PNTDFN,(long)new_name);
  }

  return (new_name);
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*  analyze a list of signal and return the bounds of the vectorised        */
/*    signals, if they occure.                                              */
/* attention !!! cette fonction part du principe que ->NAME est toujours    */
/*               le 2 eme champs de chaque structure beXXX                  */
/*                                                                          */
/****************************************************************************/
void *bvl_vectnam(void *pt_list, int *left, int *right, char **name, char type)
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
/*{{{                    bvl_printabl()                                     */
/*                                                                          */
/*  function : bvl_printabl                                                 */
/*  content  : put a \n for a better presentation of an abl                 */
/*                                                                          */
/****************************************************************************/
char *bvl_printabl(char *chaine)
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
/*{{{                    bvl_abl2str()                                      */
/*                                                                          */
/* function: bvl_abl2str                                                    */
/* description: return a string corresponding to an expression              */
/* called func.:                                                            */
/*                                                                          */
/****************************************************************************/
char *bvl_abl2str (chain_list *expr, char *chaine, int *size_pnt)
{
  char			 buf[4096];
  char         	*oper, *name;
  struct chain 	*operand ;
  int            i;


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
      strcat(chaine,vhd_getBitStr(expr->DATA,buf));
    else
      strcat(chaine,bvl_vectorize(expr->DATA));
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
           beg_get_vectname(operand->DATA,&name,NULL,NULL,BEG_SEARCH_ABL);
           bvl_mystrcat(chaine,"%s'STABLE",bvl_vhdlname(name));
           break;
           // added by anto to compute vectabl
      case REPLICATE:
           strcat (chaine,"(");
           for (i = 0; i < REP_NB(expr); i ++)
           {
             strcat(chaine,bvl_vectorize(operand->DATA));
             if (i != REP_NB(expr)-1)
               strcat(chaine," & ");
           }
           strcat (chaine, ")");
           break;
      case NOT :
           strcat (chaine, "not (");
           chaine = bvl_abl2str (operand, chaine, size_pnt);
           strcat (chaine, ")");
           break;
      default :
           oper = operToChar (OPER(expr));
           strcat (chaine,"(");
           while ( (expr = expr->NEXT) )
           {
             chaine = bvl_abl2str (expr->DATA, chaine, size_pnt);
             if (expr->NEXT)
               bvl_mystrcat(chaine, " %s ",oper);
           }
           strcat (chaine, ")");
    }
  }

  return (chaine);
}

/*}}}************************************************************************/
/*{{{                    vhd_getBitStr()                                    */
/*                                                                          */
/* create an easy to drive copy of the bitstring                            */
/*                                                                          */
/****************************************************************************/
char *vhd_getBitStr(char *str, char *buf)
{
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
    return vhd_bitToHex(buf);
  else
    return buf;
}

/*}}}************************************************************************/
/*{{{                    vhd_2puiss()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int vhd_2puiss(int exp)
{
  if (exp)
    return 2 * vhd_2puiss(exp - 1);
  else
    return 1;
}

/*}}}************************************************************************/
/*{{{                    vhd_bitToHex()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
char *vhd_bitToHex(char *buf)
{
  int        i, n, j;
  char       tmp, pos, go, str[4096];

  strcpy(str,buf);
  
  n         = strlen(str);
  j         = (n - 2) / 4 + 3 + (((n - 2) % 4) ? 1 : 0);

  buf[0]    = 'X';
  tmp       = 0;
  for (i = n; i >= 0; i --)
  {
    switch (str[i])
    {
      case 'u' : case 'z' : case 'd' : case '\'' :
		   strcpy(buf,str);
           return buf;
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
           tmp       += (str[i] - '0') * vhd_2puiss(pos);
    }
  }
  
  return buf;
}

/*}}}************************************************************************/
/*{{{                    bvl_abl2strbool()                                  */
/*                                                                          */
/* This is a modified version of the previous function that is meant        */
/*  to be synopsys compliant regarding the boolean nature of a number       */
/*  of variables and signals.                                               */
/*                                                                          */
/****************************************************************************/
char *bvl_abl2strbool(chain_list *expr, char *chaine, int *size_pnt)
{
  char  *oper, *name;
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
      bvl_mystrcat( chaine,"%s = %s", bvl_vectorize (expr->DATA),
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
           beg_get_vectname(operand->DATA,&name,NULL,NULL,BEG_SEARCH_ABL);
           bvl_mystrcat(chaine, "not (%s'EVENT)",bvl_vhdlname(name));
           break;
      case NOT :
           if (operand->NEXT && OPER(operand) == STABLE)
           {
             beg_get_vectname(VALUE_ATOM(CADR(operand)),&name,NULL,NULL,BEG_SEARCH_ABL);
             //strcat(chaine,bvl_vectorize(((chain_list*)operand->NEXT->DATA)->DATA));
             bvl_mystrcat(chaine, "%s'EVENT",bvl_vhdlname(name));
             //strcat(chaine, "'EVENT");
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
             bvl_abl2str(operand,buf,&bufsize);
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
             bvl_abl2str(tmpExpr,buf,&bufsize);
             bvl_mystrcat(chaine, "STD_LOGIC_VECTOR'%s = %s",buf,bitstr);
             mbkfree(buf);
             freeExpr(tmpExpr);
           }
           break ;
      default :
           oper = operToChar (OPER(expr));
           strcat (chaine, "(");
           while ((expr = expr->NEXT))
           {
             chaine = bvl_abl2strbool (expr->DATA, chaine, size_pnt);
             if (expr->NEXT)
               bvl_mystrcat(chaine, " %s ", oper);
           }
           strcat (chaine, ")");
    }
  }
  return (chaine);
}

/*}}}************************************************************************/
/*{{{                    bvl_revect()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static char *bvl_revect(char *name)
{
  char           buffer[1024];
  char          *pt;

  if ((pt = strchr(name, ' ')) == NULL)
    return mbkstrdup(name);
  else
  {
    strncpy(buffer, name, pt-name);
    buffer[pt-name] = 0;
    strcat(buffer, "(");
    strcat(buffer, pt+1);
    strcat(buffer, ")");

    return mbkstrdup(buffer);
  }
}

/*}}}************************************************************************/
/*{{{                    bvl_printcorrep()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline void bvl_printcorrep(FILE *fp, char *behname,
                                   int left, int right, ht *globalht, int vlog, ht *corht)
{
  chain_list    *chainx;
  char           vname[1024], name[1024], *tmp, *cirname, *vhdname, buf[1024];
  int            inc, i;

  inc               = (right >= left) ? 1 : -1;
  if (!vlog)
    vhdname           = bvl_vhdlname(behname);
  else 
    vhdname           = bgl_vlogname(behname);
  for (i = left; i != right+inc; i += inc)
  {
    if (left < 0)
    {
      sprintf(vname,"%s",vhdname);
      sprintf(name,"%s",behname);
    }
    else
    {
      sprintf(vname,"%s(%d)",vhdname,i);
      sprintf(name,"%s %d",behname,i);
    }

    cirname = mbv_getCorresp(globalht,name,buf);

    if (cirname == NULL)
      cirname       = name;

    if (corht!=NULL/* && chainx!=NULL*/ && strstr(cirname,"<nocorresp>")==NULL)
       addhtitem(corht, namealloc(name), (long)namealloc(cirname));
                       
    if (fp!=NULL)
    {
      fprintf(fp, "%s -> %s\n",vname,tmp=bvl_revect(cirname));
      mbkfree(tmp);
    }
  }
}

/*}}}************************************************************************/
/*{{{                    bvl_drivecorresp()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void bvl_drivecorresp(char *name, befig_list *ptbefig, int vlog)
{
  FILE           *fp;
  berin_list     *rin;
  bepor_list     *por;
  bevectpor_list *vpor;
  chain_list     *htlist=NULL, *nhl=NULL;
  ptype_list     *ptuser;
  char            buf[1024];
  ht *globalht, *doneht;
  bereg_list *bereg;
  bevectreg_list *bevectreg;
  bebus_list *bebus;
  bevectbus_list *bevectbus;
  beaux_list *beaux;
  bevectaux_list *bevectaux;
  bebux_list *bebux;
  bevectbux_list *bevectbux;
  beout_list *beout;
  bevectout_list *bevectout;
         
  
  ptuser            = getptype(ptbefig->USER, BEG_CORRESP_PTYPE);

  if (ptuser != NULL)
    htlist = (chain_list *)ptuser->DATA;
  else
    {  
      ht *tht;
      if ((tht=begGetCorrespHT(name))!=NULL)
        htlist = nhl= addchain(NULL, tht);
    }

  if (htlist==NULL) return;

  globalht=mbv_buildglobalht(htlist);
  doneht=addht(10000);
  
  if (!(fp = mbkfopen(bvl_vhdlname(name), "cor", WRITE_TEXT)))
  {
    beh_error(107, NULL);
    EXIT(1);
  }

  sprintf(buf, "VHDL correspondence file generated from `%s`\n",
          ptbefig->NAME);
  avt_printExecInfo(fp, "#", buf, "#");
 
  for (rin = ptbefig->BERIN; rin; rin = rin->NEXT)
    if (strncmp(rin->NAME, "yagaux_", 7) && gethtitem(doneht, rin->NAME)==EMPTYHT)
    {
      bvl_printcorrep(fp,rin->NAME,rin->LEFT,rin->RIGHT,globalht,vlog,NULL);
      addhtitem(doneht, rin->NAME, 0);
    }

  for (por = ptbefig->BEPOR; por; por = por->NEXT)
    if (gethtitem(doneht, por->NAME)==EMPTYHT)
    {
      bvl_printcorrep(fp,por->NAME,-1,-1,globalht,vlog,NULL);
      addhtitem(doneht, por->NAME, 0);
    }

  for (vpor = ptbefig->BEVECTPOR; vpor; vpor = vpor->NEXT)
    if (gethtitem(doneht, vpor->NAME)==EMPTYHT)
    {
      bvl_printcorrep(fp,vpor->NAME,vpor->LEFT,vpor->RIGHT,globalht,vlog,NULL);
      addhtitem(doneht, vpor->NAME, 0);
    }


  for (beaux = ptbefig->BEAUX; beaux; beaux = beaux->NEXT)
    if (strncmp(beaux->NAME, "yagaux_", 7) && gethtitem(doneht, beaux->NAME)==EMPTYHT)
    {
      bvl_printcorrep(fp,beaux->NAME,-1,-1,globalht,vlog,NULL);
      addhtitem(doneht, beaux->NAME, 0);
    }

  for (bevectaux = ptbefig->BEVECTAUX; bevectaux; bevectaux = bevectaux->NEXT)
    if (strncmp(bevectaux->NAME, "yagaux_", 7) && gethtitem(doneht, bevectaux->NAME)==EMPTYHT)
    {
      bvl_printcorrep(fp,bevectaux->NAME,bevectaux->LEFT,bevectaux->RIGHT,globalht,vlog,NULL);
      addhtitem(doneht, bevectaux->NAME, 0);
    }

  for (bebus = ptbefig->BEBUS; bebus; bebus = bebus->NEXT)
    if (gethtitem(doneht, bebus->NAME)==EMPTYHT)
    {
      bvl_printcorrep(fp,bebus->NAME,-1,-1,globalht,vlog,NULL);
      addhtitem(doneht, bebus->NAME, 0);
    }

  for (bevectbus = ptbefig->BEVECTBUS; bevectbus; bevectbus = bevectbus->NEXT)
    if (gethtitem(doneht, bevectbus->NAME)==EMPTYHT)
    {
      bvl_printcorrep(fp,bevectbus->NAME,bevectbus->LEFT,bevectbus->RIGHT,globalht,vlog,NULL);
      addhtitem(doneht, bevectbus->NAME, 0);
    }
  
  for (bebux = ptbefig->BEBUX; bebux; bebux = bebux->NEXT)
    if (gethtitem(doneht, bebux->NAME)==EMPTYHT)
    {
      bvl_printcorrep(fp,bebux->NAME,-1,-1,globalht,vlog,NULL);
      addhtitem(doneht, bebux->NAME, 0);
    }

  for (bevectbux = ptbefig->BEVECTBUX; bevectbux; bevectbux = bevectbux->NEXT)
    if (gethtitem(doneht, bevectbux->NAME)==EMPTYHT)
    {
      bvl_printcorrep(fp,bevectbux->NAME,bevectbux->LEFT,bevectbux->RIGHT,globalht,vlog,NULL);
      addhtitem(doneht, bevectbux->NAME, 0);
    }

  for (bereg = ptbefig->BEREG; bereg; bereg = bereg->NEXT)
    if (gethtitem(doneht, bereg->NAME)==EMPTYHT)
    {
      bvl_printcorrep(fp,bereg->NAME,-1,-1,globalht,vlog,NULL);
      addhtitem(doneht, bereg->NAME, 0);
    }

  for (bevectreg = ptbefig->BEVECTREG; bevectreg; bevectreg = bevectreg->NEXT)
    if (gethtitem(doneht, bevectreg->NAME)==EMPTYHT)
    {
      bvl_printcorrep(fp,bevectreg->NAME,bevectreg->LEFT,bevectreg->RIGHT,globalht,vlog,NULL);
      addhtitem(doneht, bevectreg->NAME, 0);
    }

  delht(doneht);
  delht(globalht);
  freechain(nhl);
  fclose(fp);
}

ht *bvl_buildfinalcorrespht(befig_list *ptbefig)
{
  berin_list     *rin;
  bepor_list     *por;
  bevectpor_list *vpor;
  chain_list     *htlist=NULL, *nhl=NULL;
  ptype_list     *ptuser;
  char            buf[1024];
  ht *resht;
  ht *globalht, *doneht;
  bereg_list *bereg;
  bevectreg_list *bevectreg;
  bebus_list *bebus;
  bevectbus_list *bevectbus;
  beaux_list *beaux;
  bevectaux_list *bevectaux;
  bebux_list *bebux;
  bevectbux_list *bevectbux;
  beout_list *beout;
  bevectout_list *bevectout;

  ptuser            = getptype(ptbefig->USER, BEG_CORRESP_PTYPE);

  if (ptuser != NULL)
    htlist = (chain_list *)ptuser->DATA;
  else
    {  
      ht *tht;
      if ((tht=begGetCorrespHT(ptbefig->NAME))!=NULL)
        htlist = nhl= addchain(NULL, tht);
    }

  if (htlist==NULL) return NULL;

  globalht=mbv_buildglobalht(htlist);
  doneht=addht(10000);

  resht=addht(10000);
  for (rin = ptbefig->BERIN; rin; rin = rin->NEXT)
    if (strncmp(rin->NAME, "yagaux_", 7) && gethtitem(doneht, rin->NAME)==EMPTYHT)
    {
      bvl_printcorrep(NULL,rin->NAME,rin->LEFT,rin->RIGHT,globalht,0,resht);
      addhtitem(doneht, rin->NAME, 0);
    }

  for (por = ptbefig->BEPOR; por; por = por->NEXT)
    if (gethtitem(doneht, por->NAME)==EMPTYHT)
    {
      bvl_printcorrep(NULL,por->NAME,-1,-1,globalht,0,resht);
      addhtitem(doneht, por->NAME, 0);
    }

  for (vpor = ptbefig->BEVECTPOR; vpor; vpor = vpor->NEXT)
    if (gethtitem(doneht, vpor->NAME)==EMPTYHT)
    {
      bvl_printcorrep(NULL,vpor->NAME,vpor->LEFT,vpor->RIGHT,globalht,0,resht);
      addhtitem(doneht, vpor->NAME, 0);
    }


  for (beaux = ptbefig->BEAUX; beaux; beaux = beaux->NEXT)
    if (strncmp(beaux->NAME, "yagaux_", 7) && gethtitem(doneht, beaux->NAME)==EMPTYHT)
    {
      bvl_printcorrep(NULL,beaux->NAME,-1,-1,globalht,0,resht);
      addhtitem(doneht, beaux->NAME, 0);
    }

  for (bevectaux = ptbefig->BEVECTAUX; bevectaux; bevectaux = bevectaux->NEXT)
    if (strncmp(bevectaux->NAME, "yagaux_", 7) && gethtitem(doneht, bevectaux->NAME)==EMPTYHT)
    {
      bvl_printcorrep(NULL,bevectaux->NAME,bevectaux->LEFT,bevectaux->RIGHT,globalht,0,resht);
      addhtitem(doneht, bevectaux->NAME, 0);
    }

  for (bebus = ptbefig->BEBUS; bebus; bebus = bebus->NEXT)
    if (gethtitem(doneht, bebus->NAME)==EMPTYHT)
    {
      bvl_printcorrep(NULL,bebus->NAME,-1,-1,globalht,0,resht);
      addhtitem(doneht, bebus->NAME, 0);
    }

  for (bevectbus = ptbefig->BEVECTBUS; bevectbus; bevectbus = bevectbus->NEXT)
    if (gethtitem(doneht, bevectbus->NAME)==EMPTYHT)
    {
      bvl_printcorrep(NULL,bevectbus->NAME,bevectbus->LEFT,bevectbus->RIGHT,globalht,0,resht);
      addhtitem(doneht, bevectbus->NAME, 0);
    }
  
  for (bebux = ptbefig->BEBUX; bebux; bebux = bebux->NEXT)
    if (gethtitem(doneht, bebux->NAME)==EMPTYHT)
    {
      bvl_printcorrep(NULL,bebux->NAME,-1,-1,globalht,0,resht);
      addhtitem(doneht, bebux->NAME, 0);
    }

  for (bevectbux = ptbefig->BEVECTBUX; bevectbux; bevectbux = bevectbux->NEXT)
    if (gethtitem(doneht, bevectbux->NAME)==EMPTYHT)
    {
      bvl_printcorrep(NULL,bevectbux->NAME,bevectbux->LEFT,bevectbux->RIGHT,globalht,0,resht);
      addhtitem(doneht, bevectbux->NAME, 0);
    }

  for (bereg = ptbefig->BEREG; bereg; bereg = bereg->NEXT)
    if (gethtitem(doneht, bereg->NAME)==EMPTYHT)
    {
      bvl_printcorrep(NULL,bereg->NAME,-1,-1,globalht,0,resht);
      addhtitem(doneht, bereg->NAME, 0);
    }

  for (bevectreg = ptbefig->BEVECTREG; bevectreg; bevectreg = bevectreg->NEXT)
    if (gethtitem(doneht, bevectreg->NAME)==EMPTYHT)
    {
      bvl_printcorrep(NULL,bevectreg->NAME,bevectreg->LEFT,bevectreg->RIGHT,globalht,0,resht);
      addhtitem(doneht, bevectreg->NAME, 0);
    }

  delht(doneht);
  delht(globalht);
  freechain(nhl);
  return resht;
}

/*}}}************************************************************************/

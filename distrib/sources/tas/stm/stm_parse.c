/******************************************************************************/
/*                                                                            */
/*                      Chaine de CAO & VLSI   AVERTEC                        */
/*                                                                            */
/*    Produit : SPF Version 1.00                                              */
/*    Fichier : spf_parse.c                                                   */
/*                                                                            */
/*    (c) copyright 2000 AVERTEC                                              */
/*    Tous droits reserves                                                    */
/*                                                                            */
/*    Auteur(s) : Gilles Augustins                                            */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*     includes                                                               */
/******************************************************************************/

#include "stm.h"



#define STM_PARSEHEADER 'h'
#define STM_PARSECELL   'c'
#define STM_PARSEHELEM  'o'
#define STM_PARSENAME   'n'

/******************************************************************************/
/*     globals                                                                */
/******************************************************************************/

int   STM_LINE ;
char *STM_FILE ;
char *STM_CNAME=NULL;
char *STM_MNAME=NULL;

/******************************************************************************/
/*     externs                                                                */
/******************************************************************************/
 
extern void  stmrestart(FILE *);
extern void  stmmodelrestart(FILE *);
extern int   stmparse() ;
extern int   stmmodelparse() ;
extern FILE *stmin ;
extern FILE *stmmodelin ;

/******************************************************************************/
/*     functions                                                              */
/******************************************************************************/
/*----------------------------------------------------------------------------*/
/*     FUNCTION :   stm_parseerror                                            */
/*          ARG :   NONE.                                                     */
/*       RETURN :   NONE.                                                     */
/*  DESCRIPTION :   Print a message of parse error with corresponding file    */
/*----------------------------------------------------------------------------*/
void    stm_parseerror (void) 
{
    avt_errmsg (STM_ERRMSG, "001", AVT_FATAL, STM_FILE, STM_LINE);
}

/*----------------------------------------------------------------------------*/
/*     FUNCTION :   stm_deletespaces                                          */
/*          ARG :   string character to suppress spaces.                      */
/*       RETURN :   pointer on the first character that it is not a space or a*/
/*                  tabulation. NULL if it is '\0' or '\n'.                   */
/*  DESCRIPTION :   delete spaces at the head of the string specified.        */
/*----------------------------------------------------------------------------*/
char*   stm_deletespaces (char *str) 
{
    char    *p;
    
    for (p=str ; ((*p==' ')||(*p=='\t'))&&(*p!='\0')&&(*p!='\n') ; p++);
    if ((*p=='\0')||(*p=='\n'))  
        p = NULL;     
        
    return p;
}

/*----------------------------------------------------------------------------*/
/*     FUNCTION :   stm_getnextword                                           */
/*       ARG(1) :   string into the serach have to be made.                   */
/*       ARG(2) :   string buffer allocated to get new line.                  */
/*       ARG(3) :   File descriptor of the current file.                      */        
/*       RETURN :   pointer on the first word found.                          */
/*  DESCRIPTION :   searches the next word in the file starting in the string */
/*                  specified. If this string does not contain any word, this */
/*                  function load the next line of the file and continues the */
/*                  search into that new string.                              */
/*----------------------------------------------------------------------------*/
char*   stm_getnextword (char *str, char *buf, FILE *file)
{
    int      size=1024;
    char    *begin = NULL, *s;
    

    if (str)
       begin = stm_deletespaces (str);

    while (begin == NULL) {
        s = fgets (buf, size, file);
        STM_LINE++;
        if (s==NULL) {
            return NULL; 
        }
        begin = stm_deletespaces (buf);
    }

    return begin;
}

/*----------------------------------------------------------------------------*/
/*     FUNCTION :   stm_searchkeyword                                         */
/*       ARG(1) :   string into the serach have to be made.                   */
/*       ARG(2) :   flag representing the word to search.                     */
/*       RETURN :   1 if the word has been found, else 0.                     */
/*  DESCRIPTION :   search the word specified into the string specified.      */
/*----------------------------------------------------------------------------*/
int     stm_searchkeyword (char *str, char keyword)
{
    int res=0;
    
    switch (keyword) {
        case    STM_PARSEHEADER :   if (!strcmp(str, "header"))
                                        res = 1;
                                    else
                                        res = 0;
                                    break;
                                
        case    STM_PARSECELL   :   if (!strcmp(str, "cell"))
                                        res = 1;
                                    else
                                        res = 0;
                                    break;
                                
        case    STM_PARSENAME   :   if (!strcmp(str, "name"))
                                        res = 1;
                                    else
                                        res = 0;
                                    break;
                                
        case    STM_PARSEHELEM  :   if (!strcmp(str, "library"))
                                        res = 1;
                                    else if (!strcmp(str, "technology"))
                                        res = 1;
                                    else if (!strcmp(str, "date"))
                                        res = 1;
                                    else if (!strcmp(str, "vendor"))
                                        res = 1;
                                    else if (!strcmp(str, "environment"))
                                        res = 1;
                                    else if (!strcmp(str, "version"))
                                        res = 1;
                                    else
                                        res = 0;
                                    break;
                                
        default                 :   res = 0;
                                    break;
    }

    return res;
}

/*----------------------------------------------------------------------------*/
/*     FUNCTION :   stm_parsekeyword                                          */
/*       ARG(1) :   string where the search starts.                           */
/*       ARG(2) :   string buffer allocated to get new line.                  */
/*       ARG(3) :   flag representing the word to search.                     */
/*       ARG(4) :   file descriptor of the current file.                      */
/*       RETURN :   pointer on the opening bracket just after the word        */
/*                  specified on or not on the same line.                     */
/*  DESCRIPTION :   search the keyword specified starting in the string       */
/*                  specified. First, searches the first opening bracket then */
/*                  check if the word placed just before is the word specified*/
/*----------------------------------------------------------------------------*/
char *stm_parsekeyword (char *line, char *buf, char keyword, FILE *file)
{
    char    *str, *s, *c;
    
    str = stm_getnextword (line, buf, file);
    if (str==NULL) {
        mbkfree (buf);
        stm_parseerror ();
    }
    s = strchr (str, '(');
    if (s) {
        for (c=s-1 ; *c==' ' ; c--);
        c++;
        *c = '\0';
    }
    else {
        c = strchr (str, '\n');
        for (c=c-1 ; *c==' ' ; c--);
        c++;
        *c = '\0';
    }
    if (stm_searchkeyword (str, keyword) == 0) {
        mbkfree (buf);
        stm_parseerror (); /*exit */  
    }
    if (s == NULL) { /* pas de '(' sur la meme ligne, header devrait etre seul*/
        str = stm_getnextword (NULL, buf, file);
        if ((str==NULL) || (*str != '(')){
            mbkfree (buf);
            stm_parseerror (); /* exit */
        }
        else 
            s = str;
    }    

    return s;

}

/*----------------------------------------------------------------------------*/
/*     FUNCTION :   stm_parseheader                                           */
/*       ARG(1) :   string buffer allocated to get new line.                  */
/*       ARG(2) :   file descriptor of the current file.                      */
/*       RETURN :   return a pointer on the next character immediatly after   */
/*                  the last closing bracker of the header section.           */
/*  DESCRIPTION :   parse the whole header section.                           */
/*----------------------------------------------------------------------------*/
char*   stm_parseheader (char *buf, FILE *file)
{
    char    *s;
    int     pcount=0;

    s = stm_parsekeyword (NULL, buf, STM_PARSEHEADER, file);
    s++; /* juste apres la parenthese ouvrante */
    pcount++; /* pcount = 1 */
    
    while (pcount > 0) {
        s = stm_parsekeyword (s, buf, STM_PARSEHELEM, file);
        s++;
        pcount++;
        s = stm_getnextword (s, buf, file);
        if ((s==NULL)||(*s!='"')) {
            mbkfree (buf);
            stm_parseerror ();
        }
        s++;
        s
 = strchr (s, '"');
        if (s==NULL) {
            mbkfree (buf);
            stm_parseerror ();
        }
        s++;
        s = stm_getnextword (s, buf, file);
        if ((s==NULL)||(*s!=')')) {
            mbkfree (buf);
            stm_parseerror ();
        }
        pcount--;
        s++;
        s = stm_getnextword (s, buf, file);
        if (s==NULL) {
            mbkfree (buf);
            stm_parseerror ();
        }
        if (*s==')')
            pcount--;
    }
    s++;

    return s;
}


/*----------------------------------------------------------------------------*/
/*     FUNCTION :   stm_parsemodel                                            */
/*       ARG(1) :   offset from the beginning of the file corresponding to the*/
/*                  beginning of the model to parse.                          */
/*       ARG(2) :   line of the beginning of the model.                       */
/*       ARG(3) :   file descriptor of the current file.                      */
/*       RETURN :   NONE.                                                     */
/*  DESCRIPTION :   parse one section model. Positions to the offset and then */
/*                  call the stmmodelparse (lex&yacc) function.               */
/*----------------------------------------------------------------------------*/
void    stm_parsemodel (MBK_OFFSET_MAX offset_model, int line, FILE *file)
{
    static int vierge = 1;
    MBK_OFFSET_MAX  offset;

    offset = offset_model;
    STM_LINE = line;
    if(vierge == 0)
        stmmodelrestart (file);
    vierge = 0;
    if (!mbkfseek (file, &offset, SEEK_SET))
        avt_errmsg (STM_ERRMSG, "036", AVT_FATAL);
    else {
        stmmodelparse ();
        if (STM_PRELOAD=='Y')
            stm_cache_setfilepos (STM_CNAME, STM_MNAME, &offset);
    }
}


/*----------------------------------------------------------------------------*/
/*     FUNCTION :   stm_parsemodels                                           */
/*       ARG(1) :   file descriptor of the current file.                      */
/*       RETURN :   NONE.                                                     */
/*  DESCRIPTION :   Parse all the models of one cell.                         */
/*                  1. Get a new line and the current position                */
/*                  2. Check if it is the beginning of a model                */
/*                     if not go to 1.                                        */
/*                     else                                                   */
/*                  3. save the current position and if it is not the first   */
/*                     model, parses the previous model.                      */
/*----------------------------------------------------------------------------*/
void    stm_parsemodels (FILE *file)
{
    char             buf[1024];
    int              size=1024;

    MBK_OFFSET_MAX   prev_offset=0;
    MBK_OFFSET_MAX   current_offset=0;
    int              prev_line=0;
    int              current_line=0;

    int              cpt=0;
    char            *c;

    current_line  = STM_LINE;
    while (cpt >= 0) {
        if (!mbkftell (file, &current_offset)) {
            avt_errmsg (STM_ERRMSG, "035", AVT_FATAL);
        }
        if (fgets (buf, size, file) == NULL) {
            STM_LINE = current_line+1; 
            stm_parseerror ();
        }
        else {
            current_line++;
            for (c=buf ; *c!='\0' ; c++) {
                if (*c=='(')
                    cpt++;
                else if (*c==')')
                    cpt--;
            }
            if ((c=strstr (buf, "model")) != NULL) {
                c--;
                while (((*c==' ')||(*c=='\t'))&&(c!=buf)) {
                    c--;
                }
                if (c==buf) {
                    if (prev_offset!=0)  {
                        stm_parsemodel (prev_offset, prev_line, file);
                        if (!mbkfseek (file, &current_offset, SEEK_SET))
                            avt_errmsg (STM_ERRMSG, "036", AVT_FATAL);
                        if (fgets (buf, size, file) == NULL) {
                            STM_LINE = current_line+1; 
                            stm_parseerror ();
                        }
                    }
                    prev_offset = current_offset;
                    prev_line = current_line;
                }
            }
        }
    }
    /* parse du dernier modele */
    if (cpt == -1) {
        if (prev_offset!=0) {
            stm_parsemodel (prev_offset, prev_line, file);
        }
    }
    else {
        stm_parseerror ();
    }

    if (!mbkfseek (file, &current_offset, SEEK_SET))
        avt_errmsg (STM_ERRMSG, "036", AVT_FATAL);
    STM_LINE = current_line;
}

/*----------------------------------------------------------------------------*/
/*     FUNCTION :   stm_parsecell                                             */
/*       ARG(1) :   string where the parse starts.                            */
/*       ARG(2) :   string buffer allocated to get new line.                  */
/*       ARG(3) :   file descriptor of the current file.                      */
/*       RETURN :   NONE.                                                     */
/*  DESCRIPTION :   parse the whole cell section.                             */
/*                  Calls the stm_addcell() function.                         */
/*----------------------------------------------------------------------------*/
void    stm_parsecell (char *str, char *buf, FILE *file, char *filename, char *extension)
{
    char    *begin, *end;
    int      pcount=0;

    begin = stm_parsekeyword (str, buf, STM_PARSECELL, file);
    begin++;
    pcount++;
    begin = stm_parsekeyword (begin, buf, STM_PARSENAME, file);
    begin++;
    pcount++;
    begin = stm_getnextword (begin, buf, file);
    if (begin==NULL) {
        mbkfree (buf);
        stm_parseerror ();
    }
    end = strchr (begin, ')');
    if (end==NULL) {
        mbkfree (buf);
        stm_parseerror ();
    }
    *end = '\0';
    end++;
    pcount--;    
    
    STM_CNAME = namealloc (begin); 
    stm_addcell (STM_CNAME);
    stm_cache_setfile (STM_CNAME, file, filename, extension);

    stm_parsemodels (file);
    STM_LINE--;
    begin = stm_getnextword (NULL, buf, file);
    if ((begin==NULL)||(*begin != ')')) {
        mbkfree (buf);
        stm_parseerror ();
    }
    begin++;
    pcount--;
    if (pcount!=0) {
        mbkfree (buf);
        stm_parseerror ();
    }
        
    begin = stm_getnextword (begin, buf, file);
    if (begin) {
        mbkfree (buf);
        stm_parseerror ();
    }
}


/*----------------------------------------------------------------------------*/
/*     FUNCTION :   stm_parse                                                 */
/*       ARG(1) :   name of the file to parse.                                */
/*       ARG(2) :   extension of the file to parse.                           */
/*       RETURN :   NONE.                                                     */
/*  DESCRIPTION :   parse a STM file.                                         */
/*----------------------------------------------------------------------------*/
void    stm_parse (char *filename, char *extension)
{
    char buf[1024];
    char *s;
    static int vierge = 1;

    if ((stmin = mbkfopen (filename, extension, "r"))) {
        STM_FILE = filename ;
        STM_LINE = 1 ;
       
        if ((STM_CACHE_SIZE>0ul)&&(MBKFOPEN_FILTER==NO))
            stm_enable_cache ( (char(*)(void*,void*))stm_cache_isactive, 
                               (unsigned long int (*)(void*,void*))stm_cache_parse_model, 
                               (unsigned long int (*)(void*,void*))stm_cache_free_model);
        if ((STM_CACHE!=NULL)&&(MBKFOPEN_FILTER==NO)) {
            stmmodelin = stmin;
            STM_PRELOAD = 'Y';
            s = stm_parseheader (buf, stmmodelin);
            stm_parsecell (s, buf, stmmodelin, filename, extension);
            STM_PRELOAD = 'N';
        }
        else {
            if(vierge == 0)
                stmrestart(stmin);
            vierge = 0;
            stmparse ();
            fclose (stmin) ; 
        }
    } 
    
}

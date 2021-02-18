/****************************************************************************/
/*                                                                          */
/*                      Chaine de verification                              */
/*                                                                          */
/*    Produit : AVT Version 1                                               */
/*    Fichier : avt_license_util.c                                          */
/*                                                                          */
/*    (c) copyright 1998-1999 AVERTEC                                       */
/*    Tous droits reserves                                                  */
/*                                                                          */
/****************************************************************************/

#include "avt_headers.h"

#ifndef NOFLEX
static VENDORCODE code;
static LM_HANDLE *lm_job = 0;
static chain_list *lm_token_used = NULL;
static char  *lm_licfile = NULL;
static int hb_count = 1;
static int hb_minutes = 1;
static int hb_status = 1;
#endif

static chain_list *supertokens_called = NULL;
static char *first_supertoken_called = NULL;

static char *MAIN_TOKEN_AVT = "AVT";

#ifndef NOFLEX

static inline int givetoken_flex(char *env, char *tool, int nbtoken);

// Les fonctions FLEXLM doivent etre comprises dans ce IF pour etre excluses
/*}}}************************************************************************/
/*{{{                         deltoken_flex()                               */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline
int deltoken_flex(LM_HANDLE *job,chain_list *tokens)
{
  chain_list *chain;
  for (chain = tokens ; chain != NULL; chain = chain->NEXT) {
    lc_checkin(job,(char *)chain->DATA,0);
  } // display info
  lc_free_job(job);
  return (1);
}

/*}}}************************************************************************/
/*{{{                      flex_license()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline
char *flex_license()
{
    char       *str;
    char        buf[1024];


    if ((str = V_STR_TAB[__AVT_LICENSE_FILE].VALUE) != NULL) {
        strcpy(buf, str);
    }
    else if ((str = getenv("AVT_TOOLS_DIR")) != NULL) {
        strcpy(buf, str);
        strcat(buf,"/etc");
    }
    else strcpy(buf,"/etc");
    return namealloc(buf);
}

/*}}}************************************************************************/
/*{{{                         flex_exit()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline
void flex_exit()
{
    avt_errmsg(AVT_ERRMSG,"036",AVT_FATAL);
}

/*}}}************************************************************************/
/*{{{                        flex_error()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline
void flex_error(int exit,int status, LM_HANDLE *job, char *msg, char *token)
{
    avt_errmsg(AVT_ERRMSG,"034",AVT_ERROR,lc_get_errno(job),msg);
    lc_perror(job,token);

    if(exit == AVT_FATAL) flex_exit();
    status = 0;
}


/*}}}************************************************************************/
/*{{{                         flex_reconn()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline
int flex_reconn(char *token,int nb)
{
    LM_HANDLE *ptjob;
    int        count,done;

    count=0;
    done=0;

    while( count <= nb && !done ) {
        ptjob=lc_first_job(lm_job);
        while(ptjob) {
            if ( !(hb_status=lc_heartbeat(ptjob, &hb_count, hb_minutes))  ) {
                done=1;
            }
            else {
                done=0;
                break;
            }
            ptjob=lc_next_job(ptjob);
        }

        if (!done) {
            sleep(10);
            count ++;
            flex_error(AVT_WARNING,hb_status,lm_job,"reconnecting to license server.",token);
        }
    }
    return done;
}

/*}}}************************************************************************/
/*{{{                         flex_newjob()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline
int flex_newjob(char *env, char *tool, int nbtoken)
{
    chain_list *chain;
    int         status=1;
    int         addtool=1;

    if ( strcmp(MAIN_TOKEN_AVT,tool) ) {
        givetoken_flex(env,MAIN_TOKEN_AVT,nbtoken);
    }

 // check if a token is already taken for this tool
    for (chain = lm_token_used ; chain != NULL; chain = chain->NEXT) {
        if (strcmp((char *)chain->DATA,tool) == 0) {
            addtool=0;
            break;
        }
    }

    if( !lm_job || addtool == 1 ) {
        if ( (status=lc_new_job(lm_job, lc_new_job_arg2, &code, &lm_job)) ) {
            flex_error(AVT_FATAL,status,lm_job,"checking new job","lc_new_job");
        }
        return 1;
    }
    return 0;
}

/*}}}************************************************************************/
/*{{{                         flex_getjob()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline
LM_HANDLE * flex_getjob(char *tool,LM_HANDLE *job)
{
    LM_HANDLE *ptjob=lc_first_job(job);

    while (ptjob) {
        if (! lc_status(ptjob, tool) ) return ptjob;
        ptjob=lc_next_job(ptjob);
    }
    return NULL;
}

/*}}}************************************************************************/
/*{{{                   flex_attributes()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline
void flex_attributes(LM_HANDLE *job, char *token)
{
    int status;

    /* set any flexlm job options here */
#ifdef MANUAL_HEARTBEAT
    if ( (status=lc_set_attr(job, LM_A_CHECK_INTERVAL, (LM_A_VAL_TYPE) -1)) ) {
        flex_error(AVT_FATAL,status,job,"setting LM_A_CHECK_INTERVAL",token);
    }
    if ( (status=lc_set_attr(job, LM_A_RETRY_INTERVAL,(LM_A_VAL_TYPE) -1)) ) {
        flex_error(AVT_FATAL,status,job,"setting LM_A_RETRY_INTERVAL",token);
    }
    if ( (status=lc_set_attr(job, LM_A_RETRY_COUNT, (LM_A_VAL_TYPE) -1)) ) {
        flex_error(AVT_FATAL,status,job,"setting LM_A_RETRY_COUNT",token);
    }
#endif
    
    /* short error message (flex_error displays full message) */
    if ((status = lc_set_attr(job, LM_A_LONG_ERRMSG, (LM_A_VAL_TYPE)0))) {
        flex_error(AVT_FATAL,status,job,"setting LM_A_LONG_ERRMSG",token);
    }
}

/*}}}************************************************************************/
/*{{{                    givetoken_flex()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline
int givetoken_flex(char *env, char *tool, int nbtoken)
{
    int                status=1;
    char              *version = mbkalloc(strlen(AVT_VERSION) + 1);

    strcpy(version,AVT_VERSION);

    if( !lm_licfile) lm_licfile=flex_license();

    if ( flex_newjob(env,tool,nbtoken) ) {
        if ( (status=lc_set_attr(lm_job, LM_A_LICENSE_DEFAULT,(LM_A_VAL_TYPE)lm_licfile)) ) {
            flex_error(AVT_FATAL,status,lm_job,"setting LM_A_LICENSE_DEFAULT",tool);
        }
        flex_attributes(lm_job,tool);
        if ( (status=lc_checkout(lm_job, tool, version,nbtoken,LM_CO_NOWAIT, &code, LM_DUP_NONE)) ) {
            flex_error(AVT_FATAL,status,lm_job,"checking out new token",tool);
        }
        else lm_token_used = addchain(lm_token_used,tool);
    }
#ifdef MANUAL_HEARTBEAT
    else {
        if ( !(status=flex_reconn(tool,360)) ) {
            flex_error(AVT_FATAL,status,lm_job,"trying to reach server",tool);
        }
    }
#endif
    return 1;
}
#endif

/*}}}************************************************************************/
/*{{{                    avt_supertokenmatch()                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
char *avt_tokenshift(token)
char *token;
{
  char *pt=MAIN_TOKEN_AVT;
  unsigned int i=0;

  while (AVT_SUPER_TOKEN_MATCH[i]) {
    if (!strcmp (token, AVT_SUPER_TOKEN_MATCH[i])) {
      pt = AVT_SUPER_TOKEN_MATCH[i];
      return pt;
    } else {
      i += 2;
    }
  }

  return pt;
}

int avt_tokenName_old2new(char *old, char **new)
{
  int        i;

  i = 0;
  if (AVT_PRODUCT_TOKEN_MATCH)
  {
    while (AVT_PRODUCT_TOKEN_MATCH[i])
      if (!strcmp (old, AVT_PRODUCT_TOKEN_MATCH[i]))
      {
        *new = AVT_PRODUCT_TOKEN_MATCH[i + 1];
        return 0;
      }
      else
        i += 2;
  }

  return 1;
}

/*}}}************************************************************************/
/*{{{                    avt_tokenmatch()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
char *avt_tokenmatch(tool)
char *tool;
{
  char *token;
  char *sptoken = MAIN_TOKEN_AVT;
  char *pt=NULL,*buff=NULL,*ptest=NULL,*testref=NULL;
  const char *separ = ",";
  chain_list *ptchain;

  if(first_supertoken_called)
    testref=first_supertoken_called;
  else
    testref=MAIN_TOKEN_AVT;

  if (avt_tokenName_old2new(tool,&token))
    token = MAIN_TOKEN_AVT;

  if ( strrchr(token,',') == NULL )
    return token;
  else {
    buff = mbkstrdup (token);
    pt = strtok(buff,separ);
    while(pt) {
      sptoken = avt_supertokenmatch(pt);
      if ( !strcmp(testref,sptoken) && testref!=MAIN_TOKEN_AVT) {
        token=avt_tokenshift(pt);
        mbkfree (buff);
        return token;
      }
      ptchain = supertokens_called;
      while(ptchain){
        ptest = (char *)ptchain->DATA;
        if ( !strcmp(ptest,sptoken) && ptest!=MAIN_TOKEN_AVT) {
          token=avt_tokenshift(pt);
          mbkfree (buff);
          return token;
        }
        ptchain = ptchain->NEXT;
      }
      pt = strtok (NULL, separ);
    }
  }

  mbkfree (buff);
  return MAIN_TOKEN_AVT;
}

char *avt_supertokenmatch(token)
char *token;
{
  char        *supertoken = MAIN_TOKEN_AVT;
  chain_list  *chain = NULL;
  int          addsupertoken=1;
  unsigned int i=0;

  if (AVT_SUPER_TOKEN_MATCH) {
    while (AVT_SUPER_TOKEN_MATCH[i]) {
      if (!strcmp (token, AVT_SUPER_TOKEN_MATCH[i])) {
        supertoken = AVT_SUPER_TOKEN_MATCH[i + 1];
        break;
      } else {
        i += 2;
      }
    }
  }

  for (chain = supertokens_called ; chain != NULL; chain = chain->NEXT) {
    if (strcmp((char *)chain->DATA,supertoken) == 0) {
      addsupertoken=0;
      break;
    }
  }

  if(addsupertoken == 1) {
    supertokens_called = addchain(supertokens_called,supertoken);
    if( (!first_supertoken_called) && (supertoken != MAIN_TOKEN_AVT) )
        first_supertoken_called = supertoken;
  }

  return supertoken;
}

/*}}}************************************************************************/
/*{{{                    avt_givetoken()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int avt_givetoken(env,tool)
char *env;
char *tool;
{
  char              *token = avt_tokenmatch(tool);
  char              *sptoken = avt_supertokenmatch(token);
  int                t,s;

#ifdef NOFLEX
  return AVT_VALID_TOKEN;
#else
  if( MBK_SOCK != -1 ) {
    t = mbk_comslavegivetoken( env, tool );
    return t ;
  }
  else {
    t = givetoken_flex(env,token,1);
    s = givetoken_flex(env,sptoken,1);
    if( s*t )
      return AVT_VALID_TOKEN ;
  }
  return 0 ;
#endif
}


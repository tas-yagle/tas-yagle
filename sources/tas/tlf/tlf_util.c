/******************************************/
/* tlf_util.c                             */
/******************************************/
/******************************************************************************/
/* INCLUDE                                                                    */
/******************************************************************************/
#include "tlf_util.h"


char TLF_TRACE_MODE = 'N' ;
char *LIBRARY_TLF_NAME ;



char *tlf_chainlistToStr(chain_list *pchain)
{
    chain_list *p;
    char s[32];
    char *str;
    
    strcpy(s,(char *)pchain->DATA);
   
    for(p=pchain->NEXT ; p != NULL ; p=p->NEXT)
            strcat(s,(char *)p->DATA);
    
    str = (char*)mbkalloc((sizeof(char)*strlen(s))+1);
    strcpy(str,s);
    return str;
}


void tlf_aff_cbhcomb(cbhcomb *p)
{
    avt_log(LOGTLF,2,"Structure CBHCOMB:\n");
    avt_log(LOGTLF,2,"\tNAME: %s\n", p->NAME);
    if(p->FUNCTION != NULL)
    {
        avt_log(LOGTLF,2,"\tFUNCTION: ");
        displayExprLog(LOGTLF,2,p->FUNCTION);
    }
    if(p->HZFUNC != NULL)
    {
        avt_log(LOGTLF,2,"\tHZFUNCTION: ");
        displayExprLog(LOGTLF,2,p->HZFUNC);
    }
    if(p->CONFLICT != NULL)
    {       
        avt_log(LOGTLF,2,"\tCONFLICT: ");
        displayExprLog(LOGTLF,2,p->CONFLICT);    
    }
}

void tlf_aff_cbhseq(cbhseq *p)
{
    avt_log(LOGTLF,2,"Structure CBHSEQ:\n");
    avt_log(LOGTLF,2,"SEQTYPE: %d\n", p->SEQTYPE);
    if(p->NAME != NULL)
        avt_log(LOGTLF,2,"NAME: %s\n", p->NAME);
    if(p->NEGNAME != NULL)
        avt_log(LOGTLF,2,"NEGNAME: %s\n", p->NEGNAME);
    if(p->PIN != NULL)
        avt_log(LOGTLF,2,"PIN: %s\n", p->PIN);
    if(p->NEGPIN != NULL)
        avt_log(LOGTLF,2,"NEGPIN: %s\n", p->NEGPIN);
    if(p->CLOCK != NULL)
    {
        avt_log(LOGTLF,2,"CLOCK:");
        displayExprLog(LOGTLF,2,p->CLOCK);
    }
    if(p->SLAVECLOCK != NULL)
    {
        avt_log(LOGTLF,2,"SLAVECLOCK:");
        displayExprLog(LOGTLF,2,p->SLAVECLOCK);
    }
    if(p->DATA != NULL)
    {
        avt_log(LOGTLF,2,"DATA:");
        displayExprLog(LOGTLF,2,p->DATA);    
    }
    if(p->RESET != NULL)
    {
        avt_log(LOGTLF,2,"RESET:");
        displayExprLog(LOGTLF,2,p->RESET);
    }
    if(p->SET != NULL)
    {
        avt_log(LOGTLF,2,"SET:");
        displayExprLog(LOGTLF,2,p->SET);    
    }
    if(p->RSCONF != NULL)
    {
        avt_log(LOGTLF,2,"RSCONF:");
        displayExprLog(LOGTLF,2,p->RSCONF);
    }
    if(p->RSCONFNEG != NULL)
    {
        avt_log(LOGTLF,2,"RSCONFNEG:");
        displayExprLog(LOGTLF,2,p->RSCONFNEG);       
    }

}           

/*****************************************************************************
*                           fonction tlf_setenv()                            *
*****************************************************************************/
void tlf_setenv()
{
    char *str;

    str = getenv("TLF_TRACE_MODE") ;
    if(str != NULL){
        TLF_TRACE_MODE = (strcmp(str,"yes") == 0 ) ? 'Y' : 'N' ;
    }
}

/*****************************************************************************
*                           fonction tlf_treatname()                         *
*****************************************************************************/
char *tlf_treatname(char *str)
{
    char ss[1024];
    char name[1024];
    int i, j;

    sprintf (name, str);
    /* enlevement de '"' */
    i = 0;
    j = 0;
    while (name[i] != '\0') {
        if (name[i] != '"'){
            ss[j] = name[i];
            j++;
        }
        i++;
    }
    ss[j] = '\0';
    return namealloc (ss);
}

/*****************************************************************************
*                           fonction tlf_getlibname()                        *
*****************************************************************************/
char *tlf_getlibname(void)
{
    return LIBRARY_TLF_NAME;
}

/****************************************************************************
*                           fonction tlf_multout()                          *
*****************************************************************************/
/* appelee quand on a detecte plusieurs sorties de latch/registre */
short tlf_multout(lofig_list **lofig, cbhcomb **comb, cbhseq **seq,
                  char *cname)
{
    locon_list  *lc;
    cbhcomb     *c;
    
    if (TLF_TRACE_MODE == 'Y')
        fprintf(stderr,"WARNING: multiple output for latch/register in cell %s\n", cname);
    if (*comb)
    {
        cbh_delcomb(*comb);
        *comb = NULL;
    }
    if (*seq)
    {
        cbh_delseq(*seq);
        *seq = NULL;
    }
    for ( lc = (*lofig)->LOCON; lc; lc = lc->NEXT)
        if ((c = cbh_getcombfromlocon(lc)))
            cbh_delcombtolocon(lc);
    *lofig  = (*lofig)->NEXT ;
    dellofig(cname);        
	
	return 1;
}

/*----------------------------------------------------------------------------*/
/* AFFCHAIN :                                                                 */
/* Affiche tous les champs DATA de la liste chainee passee en argument        */
/*----------------------------------------------------------------------------*/
void tlf_affchain(chain_list *p, char *format, char type)
{
    chain_list *d;

    for(d=p; d != NULL; d=d->NEXT) {
        if(type == TYPE_STRING)
            printf(format, (char *)(d->DATA));
        else if(type == TYPE_FLOAT)
            printf(format, *((float *)(d->DATA)));
    }

    printf("\n");
}
/*----------------------------------------------------------------------------*/
/* LOOKINGFORLOOP:                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
short tlf_lookingForExpr(chain_list *ch, char *expr)
{
    short res=0;   
    chain_list *p,*c;

    p = supportChain_listExpr(ch);

    for(c=p ; c != NULL ; c=c->NEXT) {
        if(!strcasecmp(c->DATA,expr))
                res=1;
    }
    
    freechain(p);
    return res;
}

/*----------------------------------------------------------------------------*/
/* REPLACEINABL :                                                             */
/*                                                                            */
/*----------------------------------------------------------------------------*/
chain_list *tlf_replaceInAbl(chain_list *abl, char *oldExpr, char *newExpr)
{
    chain_list *newabl, *expr;
    
    expr = createAtom(newExpr);

    newabl = substExpr(abl,oldExpr,expr);
    freeExpr(abl);
    freeExpr(expr);
    
    return newabl;
}



/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : SPF Version 1.00                                            */
/*    Fichier : spef_drive.c                                                */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Olivier BICHAUT                                           */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include <ctype.h>
#include "spef_drive.h"
#include "spef_util.h"

#define SPEF_NAME_INDEX 0xfab60614

/****************************************************************************/
/*     globals                                                              */
/****************************************************************************/
/****************************************************************************/
/*     externs                                                              */
/****************************************************************************/

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

/****************************************************************************/
/* function spef_addslash() ajoute des backslash pour qu'un nom contenant   */
/* des [ ] ne soit pas pris comme un vecteur                                */
/****************************************************************************/
static void addnameindex(ptype_list **user, long *val)
{
  *user=addptype(*user, SPEF_NAME_INDEX, (void *)*val);
  (*val)++;
}
static void removenameindex(ptype_list **user)
{
  *user=testanddelptype(*user, SPEF_NAME_INDEX);
}

static char *spef_getitem(void *me, char type, char *buf)
{
  ptype_list *pt;
  switch(type)
  {
    case 'i': if ((pt=getptype(((loins_list *)me)->USER, SPEF_NAME_INDEX))!=NULL)
                sprintf(buf, "*%ld", (long)pt->DATA);
              else
                strcpy(buf, ((loins_list *)me)->INSNAME);
              break;
    case 't': if ((pt=getptype(((lotrs_list *)me)->USER, SPEF_NAME_INDEX))!=NULL)
                sprintf(buf, "*%ld", (long)pt->DATA);
              else
                strcpy(buf, ((lotrs_list *)me)->TRNAME!=NULL?((lotrs_list *)me)->TRNAME:"?");
              break;
    case 's': if ((pt=getptype(((losig_list *)me)->USER, SPEF_NAME_INDEX))!=NULL)
                sprintf(buf, "*%ld", (long)pt->DATA);
              else
                exit(6);
              break;
    default:
              strcpy(buf, "??");
  }
  return buf;
}

char *spef_addslash(char *str)
{
    char ss[1024];
    int i = 0, j = 0;

    while (str[i] != '\0'){
        if ((str[i] == SPEF_INFO->PREFIX_BUS_DELIMITER) || (str[i] == SPEF_INFO->SUFFIX_BUS_DELIMITER)){
            ss[j] = '\\'; 
            j++;
            ss[j] = str[i]; 
            j++;
            i++;
        }else{
            ss[j] = str[i]; 
            i++;
            j++;
        }
    }
    ss[j] = '\0';
 
    return namealloc (ss);
}
/****************************************************************************/
/* function spef_printheader() ecrit l'entete du fichier spef               */
/****************************************************************************/
void spef_printheader(FILE *file, lofig_list *ptfig)
{
    chain_list *chain;
	time_t counter;
	char  *date;
  	time (&counter) ;
	date = ctime (&counter) ;
	date[strlen (date) - 1] = '\0' ;
    if(!SPEF_INFO){
        spef_createinfo();
    }
    fprintf(file, "*SPEF %s\n", SPEF_INFO->SPEF);
    fprintf(file, "*DESIGN \"%s\"\n", ptfig->NAME);
    fprintf(file, "*DATE \"%s\"\n", date);
    fprintf(file, "*VENDOR \"AVERTEC\"\n");
    fprintf(file, "*PROGRAM \"spef_driver\"\n");
    fprintf(file, "*VERSION \"1.0\"\n");
    
    fprintf(file, "*DESIGN_FLOW ");
    for(chain = SPEF_INFO->DESIGN_FLOW; chain; chain = chain->NEXT)
        fprintf(file, "%s ", (char*)chain->DATA);
    fprintf(file, "\n");

    fprintf(file, "*DIVIDER %c\n", SPEF_INFO->DIVIDER);
    fprintf(file, "*DELIMITER %c\n", SPEF_INFO->DELIMITER);
    fprintf(file, "*BUS_DELIMITER %c%c\n", SPEF_INFO->PREFIX_BUS_DELIMITER, SPEF_INFO->SUFFIX_BUS_DELIMITER);
    if(SPEF_INFO->SPEF_T_UNIT == 'N')
        fprintf(file, "*T_UNIT %g NS\n", SPEF_INFO->SPEF_T_SCALE);
    else if(SPEF_INFO->SPEF_T_UNIT == 'P')
        fprintf(file, "*T_UNIT %g PS\n", SPEF_INFO->SPEF_T_SCALE);
    if(SPEF_INFO->SPEF_CAP_UNIT == 'P')
        fprintf(file, "*C_UNIT %g PF\n", SPEF_INFO->SPEF_CAP_SCALE);
    else
        fprintf(file, "*C_UNIT %g FF\n", SPEF_INFO->SPEF_CAP_SCALE * 1000.0);
    if(SPEF_INFO->SPEF_RES_UNIT == 'O')
        fprintf(file, "*R_UNIT %g OHM\n", SPEF_INFO->SPEF_RES_SCALE);
    else
        fprintf(file, "*R_UNIT %g KOHM\n", SPEF_INFO->SPEF_RES_SCALE / 1000.0);
    if(SPEF_INFO->SPEF_L_UNIT == 'H')
        fprintf(file, "*L_UNIT %g HENRY\n", SPEF_INFO->SPEF_L_SCALE);
    else if(SPEF_INFO->SPEF_L_UNIT == 'M')
        fprintf(file, "*L_UNIT %g MH\n", SPEF_INFO->SPEF_L_SCALE);
    else if(SPEF_INFO->SPEF_L_UNIT == 'U')
        fprintf(file, "*L_UNIT %g UH\n", SPEF_INFO->SPEF_L_SCALE);
}
/****************************************************************************/
/* function spef_printconn() ecrit les connecteurs d'un net                 */
/****************************************************************************/
static char *getpnode(locon_list *lc, char *buf, int incompmode)
{
  if (incompmode && lc->PNODE!=NULL && lc->PNODE->NEXT==NULL) sprintf(buf," *x %ld", lc->PNODE->DATA);
  else strcpy(buf,"");
  return buf;

}
static char *convertname(char *name, char *buf)
{
  if (strchr(name, ' '))
    sprintf(buf, "%s%c%d%c", vectorradical(name), SPEF_INFO->PREFIX_BUS_DELIMITER,
                        vectorindex(name), SPEF_INFO->SUFFIX_BUS_DELIMITER);
  else if (strchr(name, SPEF_INFO->PREFIX_BUS_DELIMITER))
    strcpy(buf, spef_addslash(name));
  else
    strcpy(buf, name);
  
  return buf;
}

void spef_printconn(FILE *file, losig_list *ptsig)
{
    ptype_list *ptype;
    chain_list *chain;
    locon_list *ptcon;
    num_list   *num;
    char        direction;
    char buf[1024], buf1[10], buf2[1024];
    ht *modeht;
    int incompmode=0;
    
    fprintf(file, "\n*CONN\n");
    ptype = getptype(ptsig->USER, LOFIGCHAIN);
    if(ptype) {
        modeht=addht(1000);
        for(chain = (chain_list *)ptype->DATA; chain; chain = chain->NEXT) {
          ptcon = (locon_list *)chain->DATA ;
          if (ptcon->PNODE!=NULL) {
            if (gethtitem(modeht, (void *)ptcon->PNODE->DATA)!=EMPTYHT) break;
            else addhtitem(modeht, (void *)ptcon->PNODE->DATA, 0);
          }          
        }
        delht(modeht);
        if (chain!=NULL) incompmode=1;
        for(chain = (chain_list *)ptype->DATA; chain; chain = chain->NEXT) {
            ptcon = (locon_list *)chain->DATA ;
            if(ptcon->DIRECTION != 'X')
                direction = ptcon->DIRECTION;
            else
                direction = 'I';
            for(num = ptcon->PNODE; num; num = num->NEXT) {
                if(ptcon->TYPE == 'I'){
                    fprintf(file, "*I %s%c%s %c *D %s%s\n", spef_getitem((loins_list*)ptcon->ROOT, 'i', buf), SPEF_INFO->DELIMITER, convertname(ptcon->NAME, buf2), direction, ((loins_list*)ptcon->ROOT)->FIGNAME, getpnode(ptcon, buf1, incompmode));
                }
                else // zinaps : 24/4/2003
                  if(ptcon->TYPE == 'T'){
                    fprintf(file, "*I %s%c%c %c *D %s%s\n", spef_getitem((lotrs_list*)ptcon->ROOT, 't', buf), SPEF_INFO->DELIMITER, tolower(ptcon->NAME[0]), direction, getlotrsmodel((lotrs_list*)ptcon->ROOT), getpnode(ptcon, buf1, incompmode));
                  } // end zinaps
                  else{
                    fprintf(file, "*P %s %c%s\n", convertname(ptcon->NAME, buf), direction, getpnode(ptcon, buf1, incompmode));
                }
            }
        }
    }
}
/****************************************************************************/
/* function spef_printnameorindex() ecrit un nom de net ou son index.node   */
/****************************************************************************/
void spef_printnameorindex(FILE *file, losig_list *ptsig, long node)
{
    ptype_list *ptype;
    chain_list *chain;
    locon_list *ptcon;
    num_list   *num;
    char buf[2048];
    
    ptype = getptype(ptsig->USER, LOFIGCHAIN);
    if(ptype)
        for(chain = (chain_list *)ptype->DATA; chain; chain = chain->NEXT) {
            ptcon = (locon_list *)chain->DATA ;
            for(num = ptcon->PNODE; num; num = num->NEXT) {
                if(num->DATA == node){
                    if(ptcon->TYPE == 'I'){
                        if (strchr(ptcon->NAME, ' '))
                            fprintf(file, "%s%c%s%c%d%c ", spef_getitem((loins_list*)ptcon->ROOT, 'i', buf), SPEF_INFO->DELIMITER, vectorradical(ptcon->NAME), 
                            SPEF_INFO->PREFIX_BUS_DELIMITER, vectorindex(ptcon->NAME), SPEF_INFO->SUFFIX_BUS_DELIMITER);
                        else if (strchr(ptcon->NAME, SPEF_INFO->PREFIX_BUS_DELIMITER))
                            fprintf(file, "%s%c%s ", spef_getitem((loins_list*)ptcon->ROOT, 'i', buf), SPEF_INFO->DELIMITER, spef_addslash(ptcon->NAME));
                        else
                            fprintf(file, "%s%c%s ", spef_getitem((loins_list*)ptcon->ROOT, 'i', buf), SPEF_INFO->DELIMITER, ptcon->NAME);
                    }
                    else  // start zinaps
                      if(ptcon->TYPE == 'T'){
                        fprintf(file, "%s%c%c ", spef_getitem((lotrs_list*)ptcon->ROOT, 't', buf), SPEF_INFO->DELIMITER, tolower(ptcon->NAME[0]));
                    } // end zinaps
                    else{
                        if (strchr(ptcon->NAME, ' '))
                            fprintf(file, "%s%c%d%c ", vectorradical(ptcon->NAME), SPEF_INFO->PREFIX_BUS_DELIMITER, 
                            vectorindex(ptcon->NAME), SPEF_INFO->SUFFIX_BUS_DELIMITER);
                        else if (strchr(ptcon->NAME, SPEF_INFO->PREFIX_BUS_DELIMITER))
                            fprintf(file, "%s ", spef_addslash(ptcon->NAME));
                        else
                            fprintf(file, "%s ", ptcon->NAME);
                    }
                    return ;
                }
            }
        }
    fprintf(file, "%s%c%ld ", spef_getitem(ptsig, 's', buf), SPEF_INFO->DELIMITER, node);
    return ;
}
/****************************************************************************/
/* function spef_printnamemap() ecrit la table d'index des signaux          */
/****************************************************************************/
static int spef_isalim(losig_list *losig)
{
  return mbk_LosigIsVSS(losig) || mbk_LosigIsVDD(losig);
}

void spef_printnamemap(FILE *file, lofig_list *ptfig)
{
    losig_list *losig;
    lotrs_list *lt;
    loins_list *li;
    lorcnet_list *prcn;
    long index = 1;

    fprintf(file, "\n*NAME_MAP\n");
    for(losig=ptfig->LOSIG;losig;losig=losig->NEXT){
        if(spef_isalim(losig))
            continue;
        prcn = losig->PRCN;
/*        if(prcn)
            if(prcn->PCTC || prcn->PWIRE){*/
                if (strchr(getsigname(losig), ' '))
                    fprintf(file, "*%ld %s%c%d%c\n", index, vectorradical(getsigname(losig)),
                    SPEF_INFO->PREFIX_BUS_DELIMITER, vectorindex(getsigname(losig)), SPEF_INFO->SUFFIX_BUS_DELIMITER);
                else if (strchr(getsigname(losig), SPEF_INFO->PREFIX_BUS_DELIMITER))
                    fprintf(file, "*%ld %s\n", index, spef_addslash(getsigname(losig)));
                else
                    fprintf(file, "*%ld %s\n", index, getsigname(losig));
                addnameindex(&losig->USER, &index);
            }
//    }
    for(lt=ptfig->LOTRS;lt!=NULL;lt=lt->NEXT)
    {
      if (lt->TRNAME!=NULL && strlen(lt->TRNAME)>7)
        {
          fprintf(file, "*%ld %s\n", index, lt->TRNAME);
          addnameindex(&lt->USER, &index);
        }
    }
    for(li=ptfig->LOINS;li!=NULL;li=li->NEXT)
    {
      if (strlen(li->INSNAME)>7)
        {
          fprintf(file, "*%ld %s\n", index, li->INSNAME);
          addnameindex(&li->USER, &index);
        }
    }
  
}

static void spef_cleannamemap(lofig_list *ptfig)
{
    losig_list *losig;
    lotrs_list *lt;
    loins_list *li;
    lorcnet_list *prcn;

    for(losig=ptfig->LOSIG;losig;losig=losig->NEXT){
      removenameindex(&losig->USER); 
    }
    for(lt=ptfig->LOTRS;lt!=NULL;lt=lt->NEXT)
    {
      removenameindex(&lt->USER); 
    }
    for(li=ptfig->LOINS;li!=NULL;li=li->NEXT)
    {
      removenameindex(&li->USER); 
    }
}

/****************************************************************************/
/* function spef_printports() ecrit l'interface du circuit                  */
/****************************************************************************/
void spef_printports(FILE *file, lofig_list *ptfig)
{
    locon_list *locon;
    
    fprintf(file, "\n*PORTS\n");
    for(locon=ptfig->LOCON;locon;locon=locon->NEXT){
        if (strchr(locon->NAME, ' '))
            fprintf(file, "%s%c%d%c %c\n", vectorradical(locon->NAME), SPEF_INFO->PREFIX_BUS_DELIMITER,
            vectorindex(locon->NAME), SPEF_INFO->SUFFIX_BUS_DELIMITER, locon->DIRECTION);
        else
            fprintf(file, "%s %c\n", locon->NAME, locon->DIRECTION);
    }
}
/****************************************************************************/
/* function spef_printdnets() drive les RC de tous les signaux              */
/****************************************************************************/
void spef_printdnets(FILE *file, lofig_list *ptfig)
{
    losig_list *losig;
    lorcnet_list *prcn;
    lowire_list  *wire;
    loctc_list   *ctc;
    chain_list *chain;
    long nbcap = 0;
    long nbres = 0;
    char buf[2048];
    int doneprintcapa;
    
    for(losig=ptfig->LOSIG;losig;losig=losig->NEXT){
        if(spef_isalim(losig))
            continue;
        rcn_refresh_signal(ptfig, losig);
        prcn = losig->PRCN;
        doneprintcapa=0;
        if(prcn){
            if(prcn->PCTC || prcn->PWIRE){
                if(SPEF_INFO->SPEF_CAP_UNIT == 'P')
                    fprintf(file, "\n*D_NET %s %g\n", spef_getitem(losig, 's', buf), rcn_getcapa( ptfig, losig ));
                else if(SPEF_INFO->SPEF_CAP_UNIT == 'F')
                    fprintf(file, "\n*D_NET %s %g\n", spef_getitem(losig, 's', buf), rcn_getcapa( ptfig,losig ) * 1000.0);
                spef_printconn(file, losig);
                if(prcn->PCTC){ /* drive la liste des capas */
                    fprintf(file, "*CAP\n");
                    doneprintcapa=1;
                    nbcap = 1;
                    for(chain=prcn->PCTC;chain;chain=chain->NEXT){
                        ctc=(loctc_list*)chain->DATA;
                        if (!spef_isalim(ctc->SIG1) || !spef_isalim(ctc->SIG2))
                        {
                          fprintf(file, "%ld ", nbcap);
                          nbcap++;
                          if(!spef_isalim(ctc->SIG1))
                              spef_printnameorindex(file, ctc->SIG1, ctc->NODE1);
                          if(!spef_isalim(ctc->SIG2))
                              spef_printnameorindex(file, ctc->SIG2, ctc->NODE2);
                          if(SPEF_INFO->SPEF_CAP_UNIT == 'P')
                              fprintf(file, "%g\n", ctc->CAPA);
                          else if(SPEF_INFO->SPEF_CAP_UNIT == 'F')
                              fprintf(file, "%g\n", ctc->CAPA * 1000.0);
                        }
                    }
                }
                if(prcn->PWIRE){ /* drive la liste des capas des resistances */
                    for(wire=prcn->PWIRE;wire;wire=wire->NEXT){
                        if(wire->CAPA > 0.0) {
                            if (!doneprintcapa) {
                               doneprintcapa=1;
                               fprintf(file, "*CAP\n");
                               nbcap = 1;
                            }
                            fprintf(file, "%ld ", nbcap);
                            nbcap++;
                            spef_printnameorindex(file, losig, wire->NODE1);
                            if(SPEF_INFO->SPEF_CAP_UNIT == 'P')
                                fprintf(file, "%g\n", wire->CAPA/2.0);
                            else if(SPEF_INFO->SPEF_CAP_UNIT == 'F')
                                fprintf(file, "%g\n", wire->CAPA * 500.0);
                            fprintf(file, "%ld ", nbcap);
                            nbcap++;
                            spef_printnameorindex(file, losig, wire->NODE2);
                            if(SPEF_INFO->SPEF_CAP_UNIT == 'P')
                                fprintf(file, "%g\n", wire->CAPA/2.0);
                            else if(SPEF_INFO->SPEF_CAP_UNIT == 'F')
                                fprintf(file, "%g\n", wire->CAPA * 500.0);
                        }
                    }
                }
                if(prcn->PWIRE){ /* drive la liste des resistances */
                    fprintf(file, "*RES\n");
                    nbres = 1;
                    for(wire=prcn->PWIRE;wire;wire=wire->NEXT){
                        fprintf(file, "%ld ", nbres);
                        nbres++;
                        spef_printnameorindex(file, losig, wire->NODE1);
                        spef_printnameorindex(file, losig, wire->NODE2);
                        if(SPEF_INFO->SPEF_RES_UNIT == 'O')
                            fprintf(file, "%g\n", wire->RESI);
                        if(SPEF_INFO->SPEF_RES_UNIT == 'K')
                            fprintf(file, "%g\n", wire->RESI / 1000.0);
                    }
                }
                fprintf(file, "\n*END\n");
            }
        }
    }
}
/****************************************************************************/
void spef_drive(lofig_list *ptfig, FILE *outputfile)
{
    FILE *file;
    char buf[1024];
    
    if (outputfile!=NULL) file=outputfile;
    else
    {
     sprintf(buf,"%s.spef", ptfig->NAME);
     file= fopen (buf, "w+");
    }
    
    spef_printheader(file, ptfig);
    spef_printnamemap(file, ptfig);
    spef_printports(file, ptfig);
    spef_printdnets(file, ptfig);
    spef_cleannamemap(ptfig);
    if (outputfile==NULL) fclose(file);
}

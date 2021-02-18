/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TAS Version 5                                               */
/*    Fichier : tas_util.c                                                  */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Payam KIANI                                               */
/*                Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/
/* traitement des options de TAS:                                           */
/* -h : aide                                                                */
/* -s : silence                                                             */
/* -c : genere le fichier .cns                                              */
/* -n : genere le fichier .ttv et dtv                                       */
/* -in: donne le type du format d'entree                                    */
/* -t : generation du fichier ".dtv" et ".dtx"                              */
/* -t$: idem "-t" mais le programme s'arrete juste                          */
/*      apres la generation du ".dtv" et ".dtv"                             */
/* -x : composition des chemins critiques                                   */
/* -e : generation du fichier ".slo"                                        */
/* -a : sauvegarde de tous les chemins                                      */
/* -u : unflat du niveau transistor                                         */
/* -y : ignore les instances blackbox                                       */
/* -f : factorisation de chemin                                             */
/* -tec=<fichier> : fichier technologie                                     */
/* -out=<fichier> : nom de fichier de sortie                                */
/* -pch      : prise en compte des precharges. Elle                         */
/*             ne peut etre active qu'avec la                               */
/*             nouvelle chaine (fichier ttv)                                */
/* -ctc=<valeur>  : pourcentage de capacite de couplage                     */
/* -opc=<valeur>  : pourcentage de prise en compte                          */
/*                  des capas ramenees                                      */
/* -swc=<valeur>  : pourcentage pour ramener les                            */
/*                  capacites sur des switches a                            */
/*                  l'entree du bloc                                        */
/* -hr            : tas en mode hierarchique                                */
/* -lv            : pas de hierarchie chemin par connecteur complet         */
/* -nvx           : genere le fichier .ttx et dtx et .dtv et .ttv           */
/* -nr            : pas de prise en compte des resistances d'interconnexion */
/* -mg            : additionne les temps rc et les temps de portes          */
/* -nm            : efface les modeles des fichiers stm                     */
/* -bk            : casse les boucles qui sont dans les reseaux RC          */
/* -rc            : saugarde du fichier rc                                  */
/* -w             : verifie les delai d'elmore                              */
/* -max           : pas de chaine courtes                                   */
/* -pwl           : generateur tgh                                          */
/* -pwl$          : comme -pwl, mais sortie apres                           */
/* -d             : latches differntiels                                    */
/* -cout=<valeur> : capas des connecteurs OUT                               */
/* -slope=<valeur>: front d'entre des connecteurs                           */
/* -carac=<scm_int|scm_cpl|lut_int|lut_cpl> : mode de caracterisation scm   */
/*                  ou lut a l'interface ou complet                         */
/* -lm=<valeur>   : taille de memoire limite                                */
/* -xout=nom      : seul cette sortie est concerne par tas                  */
/* -xin=nom       : seul cette entree est concerne par tas                  */
/* -str           : analyse de stabilite                                    */
/* -pcd           : preserve connecteurs direction                          */
/* -uk            : TAS anglophone                                          */
/* -fr            : TAS francophone                                         */
/* -fcl : option YAGLE. reconnaisance de netlist transistor                 */
/* -xfcl : option YAGLE. arrete apres reconnaisance de netlist transistor   */
/* -gns : option YAGLE. reconnaissance hierarchique                         */
/* -xg : option YAGLE. arrete apres reconnaissance hierarchique             */
/* -i : option YAGLE. Permet de lire le fichier .inf                        */
/* -o : option YAGLE. Orientation des trannsistors avec _s                  */
/* -b : option YAGLE. Active l'orientation des trs.                         */
/* -p=: option YAGLE. Donne la profondeur de foconf                         */
/* -nv: option YAGLE. vectorisation des noms                                */
/* -z : option YAGLE. Analyse fonctionnel a travers des noeuds HZ           */
/* -cl: option YAGLE. partage du cone de commande                           */
/* -fl: option YAGLE. detection de bascule                                  */
/* -la: option YAGLE. dtection de latch automatque                          */
/* -ls: option YAGLE. la bascule c'est le point esclave                     */
/* -nl: option YAGLE. desactive la phase de recherche de latch              */
/* -rpt: option YAGLE. desactive la phase de recherche de latch             */
/****************************************************************************/

#include "tas.h"

tas_context_list *TAS_CONTEXT = NULL;
struct information TAS_INFO            ;

/****************************************************************************/

void tas_init ()
{
    TAS_CONTEXT->TAS_CAPARAPREC         = 1.0 ;
    TAS_CONTEXT->TAS_CAPASWITCH         = 1.0 ;
    TAS_CONTEXT->TAS_CAPAOUT            = 0.0 ;
    TAS_CONTEXT->TAS_FILENAME           = NULL ;
    TAS_CONTEXT->TAS_FILEIN             = NULL ;
    TAS_CONTEXT->TAS_FILEOUT            = NULL ;
    TAS_CONTEXT->TAS_TOOLNAME           = "hitas" ;
    TAS_CONTEXT->TAS_RC_FILE            = NULL ;
    TAS_CONTEXT->TAS_LIMITMEM           = (double)1000.0 ;
    TAS_CONTEXT->TAS_DEBUG_MODE         = 'N' ;
    TAS_CONTEXT->TAS_VALID_SCM          = 'N' ;
    TAS_CONTEXT->TAS_CURVE              = 'N' ;
    TAS_CONTEXT->TAS_CURVE_START        = (float)0.0 ;
    TAS_CONTEXT->TAS_CURVE_END          = (float)100.0 ;
    TAS_CONTEXT->TAS_DELAY_PROP         = 'Y' ;
    TAS_CONTEXT->TAS_DELAY_SWITCH       = 'N' ;
    TAS_CONTEXT->TAS_NO_PROP            = 'N' ;
    TAS_CONTEXT->TAS_SHORT_MODELNAME    = 'N' ;
    TAS_CONTEXT->TAS_SIMU_CONE          = 'N' ;
    TAS_CONTEXT->TAS_LEVEL              = 1  ;
    TAS_CONTEXT->TAS_PERFILE            = 'X' ;
    TAS_CONTEXT->TAS_FACTORISE          = 'N' ;
    TAS_CONTEXT->TAS_FIND_MIN           = 'Y' ;
    TAS_CONTEXT->TAS_SLOFILE            = 'N' ;
    TAS_CONTEXT->TAS_PERFINT            = 'Y' ;
    TAS_CONTEXT->TAS_INT_END            = 'Y' ;
    TAS_CONTEXT->TAS_NOTAS              = 'N' ;
    TAS_CONTEXT->TAS_DIF_LATCH          = 'N' ;
    TAS_CONTEXT->TAS_LANG               = 'E' ;
    TAS_CONTEXT->TAS_PWL                = 'N' ;
    TAS_CONTEXT->TAS_CNS_FILE           = 'Y' ;
    TAS_CONTEXT->TAS_CNS_LOAD           = 'N' ;
    TAS_CONTEXT->TAS_CNS_ANNOTATE_LOFIG = 'N' ;
    TAS_CONTEXT->TAS_TREATPRECH         = 'N' ;
    TAS_CONTEXT->TAS_SILENTMODE         = 'N' ;
    TAS_CONTEXT->TAS_HIER               = 'N' ;
    TAS_CONTEXT->TAS_NHIER              = 'N' ;
    TAS_CONTEXT->TAS_CALCRCN            = 'Y' ;
    TAS_CONTEXT->TAS_MERGERCN           = 'N' ;
    TAS_CONTEXT->TAS_SUPSTMMODEL        = 'N' ;
    TAS_CONTEXT->TAS_BREAKLOOP          = 'N' ;
    TAS_CONTEXT->TAS_SUPBLACKB          = 'N' ;
    TAS_CONTEXT->TAS_IGNBLACKB          = 'N' ;
    TAS_CONTEXT->TAS_FLATCELLS          = 'N' ;
    TAS_CONTEXT->TAS_CHECKRCDELAY       = 'N' ;
    TAS_CONTEXT->TAS_CARAC              = 'Y' ;
    TAS_CONTEXT->TAS_CARAC_MEMORY       = 'Y' ;
    TAS_CONTEXT->TAS_STABILITY          = 'N' ;
    TAS_CONTEXT->TAS_PRES_CON_DIR       = 'N' ;
    TAS_CONTEXT->TAS_CALCRCX            = 'N' ;
    TAS_CONTEXT->TAS_SAVE_BEFIG         = 'N' ;
    TAS_CONTEXT->TAS_CARACMODE          = TAS_SCM_CPL ;
    TAS_CONTEXT->TAS_TRANSINSNAME       = NULL ;
    TAS_CONTEXT->TAS_HIERLOFIG          = NULL ;
    TAS_CONTEXT->TAS_LOFIG              = NULL ;
    TAS_CONTEXT->TAS_BEFIG              = NULL ;
    TAS_CONTEXT->TAS_CNSFIG             = NULL ;
    TAS_CONTEXT->TAS_SAVERCN            = TAS_RCN ;
    TAS_CONTEXT->TAS_CHECK_PROP         = 'Y' ;
    TAS_CONTEXT->FRONT_NOT_SHRINKED     = TAS_NOFRONT ;
    TAS_CONTEXT->FRONT_CON              = TAS_NOFRONT ;
    TAS_CONTEXT->CARAC_VALUES           = 2;
    TAS_CONTEXT->INF_SIGLIST            = NULL;
}

/****************************************************************************/
/*                         fonction tas_yaginit()                           */
/****************************************************************************/
void tas_yaginit(void)
{
    YAG_CONTEXT->YAG_MAKE_CELLS = FALSE ;
    YAG_CONTEXT->YAG_NOTSTRICT  = TRUE ;
}



/****************************************************************************/
/*                         fonction tas_version()                           */
/* fixe la date et le numero de version.                                    */
/****************************************************************************/
int tas_version()

{
strcpy(TAS_CONTEXT->TAS_NB_VERSION,AVT_FULLVERSION) ;

strcpy(TAS_CONTEXT->TAS_DATE_VER,AVT_DATE_A) ;

return(0);
}

/****************************************************************************/
/*                         fonction tas_GenPwl()                            */
/* ecrit un signal de la forme tangente hyperbolique pour la commutation    */
/* d'entree.                                                                */
/****************************************************************************/
int tas_GenPwl(file,vt,vinit,front,name)
FILE      *file ;
double    vinit ,
          vt    ,
          front ;
char      *name ;

{
double vfin = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE - vinit ,
       temps                 ;

/* si front nul => echelon */
if(front == 0.0)
    {
    tas_error(25,name,TAS_WARNING) ;
    fprintf(file,"\n.IC V(%s)=%.2f\nVin %s 0 DC %.2fV\n",name,vinit,
            name,vfin) ;
    }

/*-------------------------------------------------------------------------*/
/* de 0 a la tension de seuil le signal est continu et au-dela, il devient */
/* de la forme tgh, de maniere continu et derivable.                       */
/* La derivee du signal a l'instant t=t0 ou le niveau atteint est Vt est:  */
/*                                                                         */
/*                        S'=(Vfin-Vt)/front                               */
/* D'ou l'equation de la tangente au point (t0,Vt):                        */
/*                        V-Vt = ((Vfin-Vt)/front)*(t-t0)                  */
/* Si on dit que cette droite passe par Vinit:                             */
/*                       t0 = -front*((Vinit-Vt)/(Vfin-Vt))                */
/*-------------------------------------------------------------------------*/

fprintf(file,"Vin %s 0 PWL(\n",name) ;
for(temps = 0.0 ; temps < (5*front) ; temps += (front/10.0))
    fprintf(file,"+%.3fNS %.4fV\n",temps, stm_get_v (temps, vt, vinit, vfin, front)) ;
fprintf(file,"+)\n") ;

return(0);
}


/****************************************************************************/
/*                          fonction tas_PwlFile()                          */
/* cree deux fichier PwlRise et PwlFall l'un contenant un signal tgh montant*/
/* et l'autre descendant.                                                   */
/****************************************************************************/
int tas_PwlFile()

{
FILE *fup = mbkfopen("PwlRise",NULL,WRITE_TEXT) ,
     *fdw = mbkfopen("PwlFall",NULL,WRITE_TEXT) ;
double vtn,vtp,vddmax;

if((fup == NULL) || (fdw == NULL))
    {
    char straux[100] ;

    strcpy(straux,"'PwlRise' or 'PwlFall'") ;
    tas_error(8,straux,TAS_WARNING) ;
    return(0) ;
    }
vtn = STM_DEFAULT_VTN;
vtp = STM_DEFAULT_VTP;
vddmax = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
tas_GenPwl(fup,vtn,0.0,(TAS_CONTEXT->FRONT_CON/(1000.0*TTV_UNIT)),"<noeud>") ;
tas_GenPwl(fdw,vtp,vddmax,(TAS_CONTEXT->FRONT_CON/(1000.0*TTV_UNIT)),"<noeud>") ;

if((fclose(fup) != 0) || (fclose(fdw) != 0))
    {
    char straux[100] ;

    strcpy(straux,"'PwlRise' or 'PwlFall'") ;
    tas_error(10,straux,TAS_WARNING) ;
    }

return(0) ;
}

/****************************************************************************/
/*                      fonction tas_PwlFileInput()                         */
/* Drive un type de front (up ou down) sur l'entree designee par 'name' dans*/
/* un fichier.                                                              */
/****************************************************************************/
int tas_PwlFileInput(file,typefront,name)
FILE *file;
char  typefront;
char *name;
{
double vtn,vtp,vddmax;

vtn = STM_DEFAULT_VTN;
vtp = STM_DEFAULT_VTP;
vddmax = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
if (typefront == 'U')
   tas_GenPwl(file,vtn,0.0,(TAS_CONTEXT->FRONT_CON/1000.0),name) ;
else
   tas_GenPwl(file,vddmax-vtp,vddmax,(TAS_CONTEXT->FRONT_CON/1000.0),name) ;

return(0) ;
}

/*****************************************************************************
*                           fonction tas_setenv()                            *
*****************************************************************************/
int tas_setenv()

{
    tas_context_list *newcontext;
    char *str ;
    tas_context_list *next;

    newcontext = (tas_context_list *)mbkalloc(sizeof(tas_context_list));
    next = TAS_CONTEXT;
    if (TAS_CONTEXT != NULL) {
        *newcontext = *TAS_CONTEXT;
        TAS_CONTEXT = newcontext;
    }
    else {
        TAS_CONTEXT = newcontext;
        tas_init();
    }
    newcontext->NEXT = next;

/* variable mode trace */
    TAS_CONTEXT->TAS_SILENTMODE = V_BOOL_TAB[__TAS_SILENT_MODE].VALUE ? 'Y' : 'N' ;
str = getenv("TAS_DEBUG_MODE") ;
if(str != NULL)
    {
    TAS_CONTEXT->TAS_DEBUG_MODE = (strcmp(str,"yes") == 0 ) ? 'Y' : 'N' ;
    }
    TAS_CONTEXT->TAS_SAVE_BEFIG = V_BOOL_TAB[__TAS_SAVE_BEFIG].VALUE? 'Y' : 'N' ;
str = getenv("TAS_VALID_SCM") ;
if(str != NULL)
    {
    TAS_CONTEXT->TAS_VALID_SCM = (strcmp(str,"yes") == 0 ) ? 'Y' : 'N' ;
    }
str = getenv("TAS_CURVE") ;
if(str != NULL)
    {
    TAS_CONTEXT->TAS_CURVE = (strcmp(str,"yes") == 0 ) ? 'Y' : 'N' ;
    }
str = getenv("TAS_CURVE_START") ;
if(str != NULL)
    {
    TAS_CONTEXT->TAS_CURVE_START = atof(str);
    }
str = getenv("TAS_CURVE_END") ;
if(str != NULL)
    {
    TAS_CONTEXT->TAS_CURVE_END = atof(str);
    }
    TAS_CONTEXT->TAS_DELAY_PROP = V_BOOL_TAB[__TAS_DELAY_PROP].VALUE? 'Y' : 'N' ;
    if(!V_STR_TAB[__TAS_DELAY_SWITCH].VALUE)
        avt_error("hitas", -1, AVT_WAR, "use of TAS_DELAY_SWITCH with a value different than \"no\" is deprecated\n");
    TAS_CONTEXT->TAS_NO_PROP = V_BOOL_TAB[__TAS_NO_PROP].VALUE ? 'Y' : 'N' ;
    TAS_CONTEXT->TAS_SHORT_MODELNAME = V_BOOL_TAB[__TAS_SHORT_MODELNAME].VALUE ? 'Y' : 'N' ;
str = getenv("TAS_RC_FILE") ;
if(str != NULL)
    {
    TAS_CONTEXT->TAS_RC_FILE = namealloc(str) ;
    }
    TAS_CONTEXT->TAS_SIMU_CONE = V_BOOL_TAB[__TAS_SIMU_CONE].VALUE ? 'Y' : 'N' ;
TAS_CONTEXT->TAS_LEVEL = V_INT_TAB[ __TAS_SIMULATION_LEVEL].VALUE  ;
    TAS_CONTEXT->TAS_SLOFILE = V_BOOL_TAB[__TAS_SLOFILE].VALUE ? 'Y' : 'N' ;
    TAS_CONTEXT->TAS_PERFINT = V_BOOL_TAB[__TAS_PERFINT].VALUE ? 'Y' : 'N' ;
        if(V_BOOL_TAB[__TAS_INT_END].VALUE){
            TAS_CONTEXT->TAS_PERFINT = 'Y';
            TAS_CONTEXT->TAS_INT_END = 'Y';
        }else{
            TAS_CONTEXT->TAS_INT_END = 'N';
        }
    TAS_CONTEXT->TAS_FACTORISE = V_BOOL_TAB[__TAS_FACTORISE].VALUE ? 'Y' : 'N' ;
    TAS_CONTEXT->TAS_SUPBLACKB = V_BOOL_TAB[__TAS_SUPBLACKB].VALUE ? 'Y' : 'N' ;
    TAS_CONTEXT->TAS_IGNBLACKB = V_BOOL_TAB[__TAS_IGNBLACKB].VALUE ? 'Y' : 'N' ;
    TAS_CONTEXT->TAS_FLATCELLS = V_BOOL_TAB[__TAS_FLATCELLS].VALUE ? 'Y' : 'N' ;
str = getenv("TAS_INF") ;
if(str != NULL)
    {
     avt_error("hitas", -1, AVT_WAR, "use of TAS_INF is deprecated\n");
    }
    TAS_CONTEXT->TAS_CNS_FILE = V_BOOL_TAB[__TAS_CNS_FILE].VALUE? 'Y' : 'N' ;
    TAS_CONTEXT->TAS_DIF_LATCH = V_BOOL_TAB[__TAS_DIF_LATCH].VALUE ? 'Y' : 'N' ;
     if(V_BOOL_TAB[__TAS_HIER].VALUE ){
     TAS_CONTEXT->TAS_HIER = 'Y' ;
     TAS_CONTEXT->TAS_TOOLNAME = "hitas" ;
     }
    TAS_CONTEXT->TAS_BREAKLOOP = V_BOOL_TAB[__TAS_BREAKLOOP].VALUE ? 'Y' : 'N' ;
    TAS_CONTEXT->TAS_MERGERCN = V_BOOL_TAB[__TAS_MERGERCN].VALUE ? 'Y' : 'N' ;
    TAS_CONTEXT->TAS_TREATPRECH = V_BOOL_TAB[__TAS_TREATPRECH].VALUE? 'Y' : 'N' ;
    TAS_CONTEXT->TAS_CALCRCN = V_BOOL_TAB[__TAS_CALCRCN].VALUE ? 'Y' : 'N' ;
    TAS_CONTEXT->TAS_CARAC_MEMORY = V_BOOL_TAB[__TAS_CARAC_MEMORY].VALUE ? 'Y' : 'N' ;
    TAS_CONTEXT->TAS_STABILITY = V_BOOL_TAB[__TAS_STABILITY].VALUE ? 'Y' : 'N' ;
    TAS_CONTEXT->TAS_PRES_CON_DIR = V_BOOL_TAB[__TAS_PRES_CON_DIR].VALUE ? 'Y' : 'N' ;
str = getenv("TAS_CHECK_PROP") ;
if(str != NULL)
    {
    TAS_CONTEXT->TAS_CHECK_PROP = (strcmp(str,"yes") == 0 ) ? 'Y' : 'N' ;
    }
/*str = getenv("TAS_CARACMODE") ;
if(str != NULL){
    if(!strcmp(str,"no_model")){
	    TAS_CONTEXT->TAS_CARACMODE = 0 ;
		TAS_CONTEXT->TAS_CARAC = 'N' ;
    }
    else if(!strcmp(str,"lut_int")){
        TAS_CONTEXT->TAS_CARACMODE = TAS_LUT_INT;
		TAS_CONTEXT->TAS_CARAC = 'Y' ;
    }
    else if(!strcmp(str,"lut_cpl")){
	    TAS_CONTEXT->TAS_CARACMODE = TAS_LUT_CPL;
		TAS_CONTEXT->TAS_CARAC = 'Y' ;
    }
    else if(!strcmp(str,"scm_int")){
	    TAS_CONTEXT->TAS_CARACMODE = TAS_SCM_INT;
		TAS_CONTEXT->TAS_CARAC = 'Y' ;
    }
    else if(!strcmp(str,"scm_cpl")){
	    TAS_CONTEXT->TAS_CARACMODE = TAS_SCM_CPL;
		TAS_CONTEXT->TAS_CARAC = 'Y' ;
    }
    else if(!strcmp(str,"ply_int")){
	    TAS_CONTEXT->TAS_CARACMODE = TAS_PLY_INT;
		TAS_CONTEXT->TAS_CARAC = 'Y' ;
    }
    else if(!strcmp(str,"ply_cpl")){
	    TAS_CONTEXT->TAS_CARACMODE = TAS_PLY_CPL;
		TAS_CONTEXT->TAS_CARAC = 'Y' ;
    }
}*/
    if (V_FLOAT_TAB[__FRONT_CON].SET)
    {
      TAS_CONTEXT->FRONT_CON = V_FLOAT_TAB[__FRONT_CON].VALUE * TTV_UNIT;
      TAS_CONTEXT->FRONT_NOT_SHRINKED = TAS_CONTEXT->FRONT_CON;
    }
    TAS_CONTEXT->TAS_CAPAOUT = V_FLOAT_TAB[__TAS_CAPAOUT].VALUE ;
    TAS_CONTEXT->TAS_CAPARAPREC = V_FLOAT_TAB[__TAS_CAPARAPREC].VALUE / 100.0 ;
    TAS_CONTEXT->TAS_CAPASWITCH = V_FLOAT_TAB[__TAS_CAPASWITCH].VALUE / 100.0 ;
    TAS_CONTEXT->TAS_FIND_MIN = V_BOOL_TAB[__TAS_FIND_MIN].VALUE ? 'Y' : 'N' ;
str = V_STR_TAB[__TAS_FILE_NAME].VALUE ;
if(str != NULL)
    {
    TAS_CONTEXT->TAS_FILENAME = namealloc(str);
    }
str = V_STR_TAB[__TAS_FIG_NAME].VALUE ;
if(str != NULL)
    {
    TAS_CONTEXT->TAS_FILEIN = namealloc (str);
    }
if(TAS_CONTEXT->TAS_FILEOUT == NULL)
    TAS_CONTEXT->TAS_FILEOUT = TAS_CONTEXT->TAS_FILEIN ;

#ifdef AVERTEC
str = getenv("TAS_IN_LO") ;

if(str != NULL)
  strcpy(IN_LO,str) ;
else
 {
  if(V_STR_TAB[__MBK_IN_LO].VALUE == NULL)
    strcpy(IN_LO,DEFAULTIN) ;
 }
#endif
  
/* variable langage */
YAG_CONTEXT->YAGLE_LANG = elpLang = TAS_CONTEXT->TAS_LANG = 'E' ;
TTV_LANG = TTV_LANG_E ;
str = V_STR_TAB[__AVT_LANGUAGE].VALUE ;
if(str != NULL)
    {
    YAG_CONTEXT->YAGLE_LANG = elpLang = TAS_CONTEXT->TAS_LANG =
    (strcmp(str,"english") == 0 ) ? 'E' : 'F' ;
    TTV_LANG =
    (strcmp(str,"english") == 0 ) ? TTV_LANG_E : TTV_LANG_F ;
    }

return(0) ;
}

/*****************************************************************************
*                            fonction tas_restore()                          *
*****************************************************************************/
void tas_restore()
{
    tas_context_list *oldcontext;

    if (TAS_CONTEXT == NULL){
        tas_error(68,NULL,TAS_WARNING) ;
    }else{
        oldcontext = TAS_CONTEXT;
        TAS_CONTEXT = TAS_CONTEXT->NEXT;
        mbkfree(oldcontext);
    }
}


/*****************************************************************************
*                            fonction perfpath()                             *
*****************************************************************************/
void perfpath(pathopt)
char *pathopt ;

{
short i ;

pathopt += 3 ;

/* recherche du carcatere ':' dans l'argument */
for(i = 0 ; (i != (int)strlen(pathopt)) && (pathopt[i] != ':') ; i++) ;

if(i == (int)strlen(pathopt))
    {
    TTV_SEUIL_MIN = atol(pathopt) ;
    TTV_SEUIL_MAX = TTV_DELAY_MAX ;

    if((TTV_SEUIL_MIN == 0) && (strcmp(pathopt,"0") != 0) &&
       (strcmp(pathopt,"00") != 0))
    tas_error(26,NULL,TAS_NULL) ;
    }
else /* alors il y a un ':' */
    {
    char *aux = pathopt ;

    aux[i] = '\0' ; /* isolation du seuil MIN */

    if(i != 0)
        {
        TTV_SEUIL_MIN = atol(aux) ;

        if((TTV_SEUIL_MIN == 0) && (strcmp(aux,"0") != 0) &&
           (strcmp(aux,"00") != 0))
        tas_error(26,NULL,TAS_NULL) ;
        }
    else TTV_SEUIL_MIN = TTV_DELAY_MIN ;

    aux += i+1 ; /* isolation du seuil MAX */

    if(aux[0] != '\0')
        {
        TTV_SEUIL_MAX = atol(aux) ;

        if((TTV_SEUIL_MAX == 0) && (strcmp(aux,"0") != 0) &&
           (strcmp(aux,"00") != 0))
        tas_error(26,NULL,TAS_NULL) ;
        }
    else TTV_SEUIL_MAX = TTV_DELAY_MAX ;
    }
 
return ;
}


/*****************************************************************************
*                            fonction retkey()                               *
* renvoie la position de '=' dans chaine si il existe, -1 sinon              *
*****************************************************************************/
short retkey(chaine)
char *chaine ;

{
short i ;

for(i = 0 ; (i<(int)strlen(chaine)) && (chaine[i] != '=') ; i ++) ;

return((i == (int)strlen(chaine)) ? -1 : i) ;
}


/*****************************************************************************
*                            fonction optionword()                           *
* pour les options sur plusieurs lettres.                                    *
*****************************************************************************/
int optionword(chaine)
char *chaine ;

{
short i ;
char distrib_buf[80];

if(chaine[1] == 'x' && chaine[2] == '=') perfpath(chaine) ;
else /* autres cas */
    {
    short eq ;

    if((eq=retkey(chaine)) == -1) /* les options n'ayant pas de '=' */
        {
             if(strcmp(chaine,"-pch") == 0) TAS_CONTEXT->TAS_TREATPRECH = 'Y' ;
        else if(strcmp(chaine,"-lv") == 0) TAS_CONTEXT->TAS_NHIER = 'Y' ;
        else if(strcmp(chaine,"-nvx") == 0) TAS_CONTEXT->TAS_PERFILE = 'A' ;
        else if(strcmp(chaine,"-fcl") == 0) YAG_CONTEXT->YAG_FCL_DETECT = TRUE ;
        else if(strcmp(chaine,"-xfcl") == 0){ 
                 YAG_CONTEXT->YAG_FCL_DETECT = TRUE ;
                 YAG_CONTEXT->YAG_ONLY_FCL = TRUE;
        }
        else if(strcmp(chaine,"-gns") == 0) YAG_CONTEXT->YAG_GENIUS = TRUE;
        else if(strcmp(chaine,"-xg") == 0){
                 YAG_CONTEXT->YAG_GENIUS = TRUE;
                 YAG_CONTEXT->YAG_ONLY_GENIUS = TRUE;
        }
        else if(strcmp(chaine,"-fl") == 0) YAG_CONTEXT->YAG_MAKE_CELLS = TRUE ;
        else if(strcmp(chaine,"-la") == 0) YAG_CONTEXT->YAG_LOOP_ANALYSIS = TRUE ;
        else if(strcmp(chaine,"-cl") == 0) YAG_CONTEXT->YAG_CELL_SHARE = TRUE ;
        else if(strcmp(chaine,"-nl") == 0) YAG_CONTEXT->YAG_DETECT_LATCHES = FALSE ;
        else if(strcmp(chaine,"-rpt") == 0) YAG_CONTEXT->YAG_REMOVE_PARA = TRUE ;
        else if(strcmp(chaine,"-nr") == 0) TAS_CONTEXT->TAS_CALCRCN = 'N' ;
        //else if(strcmp(chaine,"-bk") == 0) TAS_CONTEXT->TAS_BREAKLOOP = 'Y' ;
        else if(strcmp(chaine,"-mg") == 0) TAS_CONTEXT->TAS_MERGERCN = 'Y' ;
        else if(strcmp(chaine,"-nm") == 0) TAS_CONTEXT->TAS_SUPSTMMODEL = 'Y' ;
        else if(strcmp(chaine,"-rc") == 0)
              {
               if(TAS_CONTEXT->TAS_RC_FILE == NULL)
                 TAS_CONTEXT->TAS_RC_FILE = namealloc("") ;
              }
        else if(strcmp(chaine,"-rcn") == 0) TAS_CONTEXT->TAS_SAVERCN = TAS_RCN ;
        else if(strcmp(chaine,"-elm") == 0) TAS_CONTEXT->TAS_SAVERCN = TAS_ELM ;
        else if(strcmp(chaine,"-hr") == 0)
              {
               TAS_CONTEXT->TAS_HIER = 'Y' ;
               TAS_CONTEXT->TAS_TOOLNAME = "hitas" ;
              }
        else if(strcmp(chaine,"-min") == 0) {
            fprintf (stderr, "WARNING: -min option is obsolete, minimum paths are computed by default.\n");
            fprintf (stderr, "WARNING: -max option to disable minimum paths.\n");
        }
        else if(strcmp(chaine,"-max") == 0) TAS_CONTEXT->TAS_FIND_MIN = 'N' ;
        else if(strcmp(chaine,"-d") == 0) TAS_CONTEXT->TAS_DIF_LATCH = 'Y' ;
        else if(strcmp(chaine,"-str") == 0) TAS_CONTEXT->TAS_STABILITY = 'Y' ;
        else if(strcmp(chaine,"-pcd") == 0) TAS_CONTEXT->TAS_PRES_CON_DIR = 'Y' ;
        else if(strcmp(chaine,"-pwl") == 0) TAS_CONTEXT->TAS_PWL = 'Y' ;
        else if(strcmp(chaine,"-pwl$") == 0)
            {
            TAS_CONTEXT->TAS_PWL = 'Y' ;
            TAS_CONTEXT->TAS_NOTAS = 'Y' ;
            }
        else if(strcmp(chaine,"-uk") == 0)
            {
            YAG_CONTEXT->YAGLE_LANG = elpLang = TAS_CONTEXT->TAS_LANG = 'E' ;
            TTV_LANG = TTV_LANG_E ;
            tas_version() ;
            }
        else if(strcmp(chaine,"-fr") == 0)
            {
            YAG_CONTEXT->YAGLE_LANG = elpLang = TAS_CONTEXT->TAS_LANG = 'F' ;
            TTV_LANG = TTV_LANG_F ;
            tas_version() ;
            }
        else if(strcmp(chaine,"-t$") == 0){
            TAS_CONTEXT->TAS_PERFINT = TAS_CONTEXT->TAS_INT_END = 'Y' ;
        }

        else for(i = 1 ; i < (int)strlen(chaine) ; i ++)
        switch(chaine[i])
            {
            case 'c' : TAS_CONTEXT->TAS_CNS_FILE = 'Y' ; break ;
            case 'd' : TAS_CONTEXT->TAS_DIF_LATCH = 'Y' ; break ;
            case 'a' : TTV_PATH_SAVE = TTV_SAVE_PATH ;
                       break ;
            case 'e' : TAS_CONTEXT->TAS_SLOFILE = 'Y' ; break ;
            case 't' : TAS_CONTEXT->TAS_PERFINT = 'Y' ; break ;
            case 'n' : TAS_CONTEXT->TAS_PERFILE = 'V' ; break ;
            case 'f' : TAS_CONTEXT->TAS_FACTORISE = 'Y' ; break ;
            case 'u' : TAS_CONTEXT->TAS_SUPBLACKB = 'Y' ; break ;
            case 'y' : TAS_CONTEXT->TAS_IGNBLACKB = 'Y' ; break ;
            case 'q' : TAS_CONTEXT->TAS_FLATCELLS = 'Y' ; break ;
            case 'w' : TAS_CONTEXT->TAS_CHECKRCDELAY = 'Y' ; break ;
            case 'z' : YAG_CONTEXT->YAG_PROP_HZ = TRUE ; break ;
            case 'x' : TTV_SEUIL_MAX = TTV_DELAY_MAX ;
                       TTV_SEUIL_MIN = TTV_DELAY_MIN ;
                       break ;
            case 's' : TAS_CONTEXT->TAS_SILENTMODE = 'Y' ;
                       break ;

            /* options YAGLE */
            case 'b' : YAG_CONTEXT->YAG_ORIENT = TRUE ; break ;
            case 'o' : YAG_CONTEXT->YAG_HELP_S = TRUE ; break ;
            case 'i' : avt_sethashvar("avtReadInformationFile", inf_GetDefault_AVT_INF_Value());
                       //avt_error("hitas", -1, AVT_WAR, "option -i is deprecated\n");
                       break ;
            /* fin des options YAGLE */

            default  : tas_error(26,NULL,TAS_NULL) ;
            }
        } /* fin de autres options melangees */
    else /* cas des options ayant '=' dans la ligne de commande  */
        {
        char aux[150] ;
        char* caracmode ;

        strcpy(aux,chaine) ;
        aux[eq] = '\0' ; /* on isole le mot avant '=' */

        if(eq != 0) /* premier caractere pas '=' */
            {
            if(strcmp(aux,"-tec") == 0) 
                 {
                  strcpy(elpTechnoFile,chaine+eq+1) ;
                  ELP_LOAD_FILE_TYPE = ELP_LOADELP_FILE;
                 }
            else if (strcmp(aux,"-out") == 0) 
                 {
                  TAS_CONTEXT->TAS_FILEOUT = namealloc(chaine+eq+1) ;
                  if(TAS_CONTEXT->TAS_RC_FILE != NULL)
                   {
                    if(strlen(TAS_CONTEXT->TAS_RC_FILE) == 0)
                      TAS_CONTEXT->TAS_RC_FILE = TAS_CONTEXT->TAS_FILEOUT ;
                   }
                 }
            else if (strcmp(aux,"-fig") == 0) 
                 {
                  TAS_CONTEXT->TAS_FILEIN = namealloc(chaine+eq+1) ;
                 }
            else if(strcmp(aux,"-opc") == 0)
                {
                TAS_CONTEXT->TAS_CAPARAPREC = ((double)atoi(chaine+eq+1)) / 100.0 ;

                if(((TAS_CONTEXT->TAS_CAPARAPREC == 0.0) && (strcmp(chaine+eq+1,"0") != 0)) ||
                   (TAS_CONTEXT->TAS_CAPARAPREC < 0.0)                                        )
                tas_error(26,NULL,TAS_NULL) ;
                }
            else if(strcmp(aux,"-ctc") == 0)
                {
                TLC_COEF_CTC = ((double)atoi(chaine+eq+1)) / 100.0 ;

                if(((TLC_COEF_CTC == 0.0) && (strcmp(chaine+eq+1,"0") != 0)) ||
                   (TLC_COEF_CTC < 0.0) || (TLC_COEF_CTC > 2.0))
                tas_error(26,NULL,TAS_NULL) ;
                }
            else if(strcmp(aux,"-swc") == 0)
                {
                TAS_CONTEXT->TAS_CAPASWITCH = ((double)atoi(chaine+eq+1)) / 100.0 ;

                if(((TAS_CONTEXT->TAS_CAPASWITCH == 0.0) && (strcmp(chaine+eq+1,"0") != 0)) ||
                   (TAS_CONTEXT->TAS_CAPASWITCH < 0.0)                                        )
                tas_error(26,NULL,TAS_NULL) ;
                }
            else if(strcmp(aux,"-cout") == 0) /* capas de charge */
                {
                if((TAS_CONTEXT->TAS_CAPAOUT = atof(chaine+eq+1)) == 0.0)
                tas_error(26,NULL,TAS_NULL) ;
                }
            else if(strcmp(aux,"-carac") == 0) /* mode de caracterisation */
                {
                caracmode=namealloc((char *)(chaine+eq+1));
                if(!strcmp(caracmode,"no_model")){ /* pas de modeles */
				    TAS_CONTEXT->TAS_CARACMODE = 0 ;
		    		TAS_CONTEXT->TAS_CARAC = 'N' ;
                }
                else if(!strcmp(caracmode,"lut_int")){ /* tables a l'interface */
				    TAS_CONTEXT->TAS_CARACMODE = TAS_LUT_INT;
		    		TAS_CONTEXT->TAS_CARAC = 'Y' ;
                }
                else if(!strcmp(caracmode,"lut_cpl")){ /* tables complet */
				    TAS_CONTEXT->TAS_CARACMODE = TAS_LUT_CPL;
		    		TAS_CONTEXT->TAS_CARAC = 'Y' ;
                }
                else if(!strcmp(caracmode,"scm_int")){ /* modeles scm a l'interface */
				    TAS_CONTEXT->TAS_CARACMODE = TAS_SCM_INT;
		    		TAS_CONTEXT->TAS_CARAC = 'Y' ;
                }
                else if(!strcmp(caracmode,"scm_cpl")){ /* modeles scm complet */
				    TAS_CONTEXT->TAS_CARACMODE = TAS_SCM_CPL;
		    		TAS_CONTEXT->TAS_CARAC = 'Y' ;
                }
                else if(!strcmp(caracmode,"ply_int")){ /* polynomes a l'interface */
				    TAS_CONTEXT->TAS_CARACMODE = TAS_PLY_INT;
		    		TAS_CONTEXT->TAS_CARAC = 'Y' ;
                }
                else if(!strcmp(caracmode,"ply_cpl")){ /* polynomes complet */
				    TAS_CONTEXT->TAS_CARACMODE = TAS_PLY_CPL;
		    		TAS_CONTEXT->TAS_CARAC = 'Y' ;
                }
                else
                    tas_error(26,NULL,TAS_NULL) ;
                }
            else if(strcmp(aux,"-slope") == 0) /* front d'entree */
                {
                if((TAS_CONTEXT->FRONT_CON = atol(chaine+eq+1) * TTV_UNIT) <= (long)0)
                tas_error(26,NULL,TAS_NULL) ;
                if((TAS_CONTEXT->FRONT_NOT_SHRINKED = atol(chaine+eq+1) * TTV_UNIT) <= (long)0)
                tas_error(26,NULL,TAS_NULL) ;
                }
            else if(strcmp(aux,"-lm") == 0) /* taille de memoire limite */
                {
                if((TAS_CONTEXT->TAS_LIMITMEM = atof(chaine+eq+1)) <= (double)0.0)
                tas_error(26,NULL,TAS_NULL) ;
                }
            else if(strcmp(aux,"-xout") == 0) /* sortie */
                {
                 TTV_NAME_OUT=addchain(TTV_NAME_OUT,
                              (void *)namealloc((char *)(chaine+eq+1)));
                }
            else if(strcmp(aux,"-u") == 0) /* unflat */
                {
                 TAS_CONTEXT->TAS_TRANSINSNAME=(char *)namealloc((char *)(chaine+eq+1));
                 TAS_CONTEXT->TAS_SUPBLACKB = 'Y' ;
                }
             else if(strcmp(aux,"-f") == 0) /* factorisation des chemins */
                {
                 if(strcmp(chaine+eq+1,"g" ) == 0)
                  TAS_CONTEXT->TAS_FACTORISE = 'G' ;
                 else if((TTV_MAX_FCYCLE = atol(chaine+eq+1)) > 0)
                  TAS_CONTEXT->TAS_FACTORISE = 'Y' ;
                 else tas_error(26,NULL,TAS_NULL) ;
                }
            else if(strcmp(aux,"-xin") == 0) /* entree */
                {
                 TTV_NAME_IN=addchain(TTV_NAME_IN,
                             (void *)namealloc((char *)(chaine+eq+1)));
                }
            else if(strcmp(aux,"-in") == 0) /* type du fichier d'entree */
                {
                /* test du format. TAS n'accepte pas de EDIF ou HILO */
                if((strcmp(chaine+eq+1,"al" ) != 0) &&  /* ALLIANCE */
                   (strcmp(chaine+eq+1,"fdn") != 0) &&  /* DAX      */
                   (strcmp(chaine+eq+1,"fne") != 0) &&  /* VTI      */
                   (strcmp(chaine+eq+1,"hns") != 0) &&  /* VTI      */
                   (strcmp(chaine+eq+1,"hdn") != 0) &&  /* DAX      */
                   (strcmp(chaine+eq+1,"spi") != 0) &&  /* SPICE    */
                   (strcmp(chaine+eq+1,"sp") != 0) &&   /* SPICE    */
                   (strcmp(chaine+eq+1,"vlg") != 0) &&  /* VERILOG  */
                   (strcmp(chaine+eq+1,"vhd") != 0) &&  /* VHDL     */
                   (strcmp(chaine+eq+1,"cns") != 0)   ) /* CNS      */
                tas_error(26,NULL,TAS_NULL) ;

                if(strcmp(chaine+eq+1,"cns") == 0) 
                        TAS_CONTEXT->TAS_CNS_LOAD = 'Y'; 
                else strcpy(IN_LO,chaine+eq+1) ;
                }

            /* options YAGLE */
            else if(strcmp(aux,"-p") == 0)
                {
                YAG_CONTEXT->YAG_DEPTH = atoi(chaine+eq+1) ;
                YAG_CONTEXT->YAG_USE_FCF = TRUE ;

                if((YAG_CONTEXT->YAG_DEPTH == 0) && (strcmp(chaine+eq+1,"0") != 0))
                tas_error(26,NULL,TAS_NULL) ;

                if(YAG_CONTEXT->YAG_DEPTH == 0)
                YAG_CONTEXT->YAG_USE_FCF = FALSE ;
                }

            else tas_error(26,NULL,TAS_NULL) ;
            }
        else tas_error(26,NULL,TAS_NULL) ;
        }
    } /* fin des autres cas */

  return(0);
}


/*****************************************************************************
*                            fonction tas_option()                           *
*****************************************************************************/
int tas_option(argc,argv,preload)
int argc;
char *argv[];
char preload;

{
short i, eq;
char help = 'n' ;
char aux[150] ;

if(preload){
    for(i = 1 ; i != argc ; i++)
        if((eq=retkey(argv[i])) != -1){
            strcpy(aux,argv[i]) ;
            aux[eq] = '\0' ;
            if(strcmp(aux,"-tec") == 0) 
                strcpy(elpTechnoFile,argv[i]+eq+1) ;
        }
    return (0);
}

tas_version() ;
if(argc == 1) tas_error(26,NULL,TAS_NULL) ;

for(i = 1 ; i != argc ; i++)
    {
    if(argv[i][0] != '-')
        {
        TAS_CONTEXT->TAS_FILENAME = namealloc(argv[i]) ;
        if(TAS_CONTEXT->TAS_FILEOUT == NULL)
         {
          TAS_CONTEXT->TAS_FILEOUT = TAS_CONTEXT->TAS_FILENAME ;
         }
        }
    else
        {
             if(strcmp(argv[i],"-e") == 0) TAS_CONTEXT->TAS_SLOFILE   = 'Y' ;
        else if(strcmp(argv[i],"-t") == 0) TAS_CONTEXT->TAS_PERFINT   = 'Y' ;
        else if(strcmp(argv[i],"-n") == 0) TAS_CONTEXT->TAS_PERFILE   = 'V' ;
        else if(strcmp(argv[i],"-f") == 0) TAS_CONTEXT->TAS_FACTORISE = 'Y' ;
        else if(strcmp(argv[i],"-u") == 0) TAS_CONTEXT->TAS_SUPBLACKB = 'Y' ;
        else if(strcmp(argv[i],"-y") == 0) TAS_CONTEXT->TAS_IGNBLACKB = 'Y' ;
        else if(strcmp(argv[i],"-q") == 0) TAS_CONTEXT->TAS_FLATCELLS = 'Y' ;
        else if(strcmp(argv[i],"-w") == 0) TAS_CONTEXT->TAS_CHECKRCDELAY = 'Y' ;
        else if(strcmp(argv[i],"-a") == 0) 
              {
               TTV_PATH_SAVE = TTV_SAVE_PATH ;
              }
        else if(strcmp(argv[i],"-h") == 0) help = 'y' ;
        else if(strcmp(argv[i],"-z") == 0) YAG_CONTEXT->YAG_PROP_HZ = FALSE ;
        else if(strcmp(argv[i],"-x") == 0)
            {
            TTV_SEUIL_MAX = TTV_DELAY_MAX ;
            TTV_SEUIL_MIN = TTV_DELAY_MIN ;
            }
        else if(strcmp(argv[i],"-c") == 0) TAS_CONTEXT->TAS_CNS_FILE = 'Y' ;
        else if(strcmp(argv[i],"-s") == 0)
            {
            TAS_CONTEXT->TAS_SILENTMODE = 'Y' ;
            }

        /* options YAGLE */
        else if(strcmp(argv[i],"-i") == 0) avt_sethashvar("avtReadInformationFile", inf_GetDefault_AVT_INF_Value()); //avt_error("hitas", -1, AVT_WAR, "option -i is deprecated\n");
        else if(strcmp(argv[i],"-b") == 0) YAG_CONTEXT->YAG_ORIENT = TRUE ;
        else if(strcmp(argv[i],"-o") == 0) YAG_CONTEXT->YAG_HELP_S = TRUE ;
        /* fin des options YAGLE */

        else optionword(argv[i]) ;
        }
    }

if((TAS_CONTEXT->TAS_PERFINT == 'Y') || ((STB_CTK_VAR & STB_CTK) == STB_CTK))
    STB_GRAPH_VAR = STB_DET_GRAPH;
else 
    STB_GRAPH_VAR = STB_RED_GRAPH;
    

if(help == 'y')
tas_error(0,NULL,TAS_NULL) ;

if((TAS_CONTEXT->TAS_FILENAME == NULL) && (TAS_CONTEXT->TAS_NOTAS == 'N'))
tas_error(26,NULL,TAS_NULL) ;

if(TAS_CONTEXT->TAS_SILENTMODE == 'Y')
 tas_CloseTerm() ;

 if((TTV_NAME_IN == NULL) && (TTV_NAME_OUT != NULL))
   TTV_NAME_IN = addchain(NULL,(void *)namealloc("*")) ;
 if((TTV_NAME_IN != NULL) && (TTV_NAME_OUT == NULL))
   TTV_NAME_OUT = addchain(NULL,(void *)namealloc("*")) ;
 if((TTV_NAME_IN == NULL) && (TTV_NAME_OUT == NULL) && 
    (TTV_PATH_SAVE == TTV_SAVE_PATH))
  {
   TTV_PATH_SAVE = TTV_SAVE_ALLPATH ;
  }

 /*
 if(TAS_CONTEXT->TAS_CNS_LOAD == 'Y')
   TAS_CONTEXT->TAS_CALCRCN = 'N' ;
   */

if(TAS_CONTEXT->TAS_RC_FILE != NULL)
  {
   if(strlen(TAS_CONTEXT->TAS_RC_FILE) == 0)
   TAS_CONTEXT->TAS_RC_FILE = TAS_CONTEXT->TAS_FILEOUT ;
  }

 return(0);
}

/*****************************************************************************/
/*                            fonction tas_CloseTerm()                       */
/* renvoie les messages envoyes sur stdout vers /dev/null (mode silencieux). */
/*****************************************************************************/
int tas_CloseTerm()

{
int  out ;
char sout[256] ;

if(TAS_CONTEXT->TAS_FILEOUT == NULL)
  return(0) ;

sprintf(sout,"%s.tou",TAS_CONTEXT->TAS_FILEOUT) ;
if((out = open(sout,O_WRONLY|O_CREAT|O_TRUNC,
                  (S_IRWXU & ~S_IXUSR)|S_IRGRP|S_IROTH)) != -1)
 {
  TAS_CONTEXT->TAS_SAVE_OUT = dup(1) ;
  if(dup2(out,1) != 1)
   tas_error(8,sout,TAS_ERROR) ;
  else
   close(out) ;
 }
else
 {
  tas_error(8,sout,TAS_ERROR) ;
 }

sprintf(sout,"%s.ter",TAS_CONTEXT->TAS_FILEOUT) ;
if((out = open(sout,O_WRONLY|O_CREAT|O_TRUNC,
                  (S_IRWXU & ~S_IXUSR)|S_IRGRP|S_IROTH)) != -1)
 {
  TAS_CONTEXT->TAS_SAVE_ERR = dup(2) ;
  if(dup2(out,2) != 2)
   tas_error(8,sout,TAS_ERROR) ;
  else
   close(out) ;
 }
else
 {
  tas_error(8,sout,TAS_ERROR) ;
 }

return(0) ;
}


/*****************************************************************************/
/*                            fonction tas_RestoreTerm()                     */
/* retablit le renvoie des messages envoyes sur stdout vers l'ecran.         */
/*****************************************************************************/
int tas_RestoreTerm()

{
char sout[256] ;

if(TAS_CONTEXT->TAS_FILEOUT == NULL)
  return(0) ;

close(1) ;
sprintf(sout,"%s.tou",TAS_CONTEXT->TAS_FILEOUT) ;
if(dup2(TAS_CONTEXT->TAS_SAVE_OUT,1) != 1)
 tas_error(10,sout,TAS_ERROR) ;
else
 close(TAS_CONTEXT->TAS_SAVE_OUT) ;

close(2) ;
sprintf(sout,"%s.ter",TAS_CONTEXT->TAS_FILEOUT) ;
if(dup2(TAS_CONTEXT->TAS_SAVE_ERR,2) != 2)
 tas_error(10,sout,TAS_ERROR) ;
else
 close(TAS_CONTEXT->TAS_SAVE_ERR) ;

TAS_CONTEXT->TAS_SILENTMODE = 'N' ;

return(0);
}


/*****************************************************************************/
/*                            fonction tas_GetKill()                         */
/* lorsque le signal ^C est envoye au process, cette fonction retablit la    */
/* sortie stdout, si elle a ete detrournee.                                  */
/*****************************************************************************/
void tas_GetKill(sig)
int sig;
{
if(TAS_CONTEXT->TAS_SILENTMODE == 'Y')
tas_RestoreTerm() ;

EXIT(sig) ;
}

/*****************************************************************************/
/*                            fonction tas_handler_core()                    */
/* trap le core si TAS_DEBUG n'est pas active                                */
/*****************************************************************************/
void tas_handler_core()
{
 tas_error(61,NULL,TAS_ERROR) ;
}

/*****************************************************************************/
/*                           fonction InitInfo()                             */
/* initialise les champs d'une variable d'information.                       */
/*****************************************************************************/
struct information tas_InitInfo(info)
struct information info ;

{
info.lo_t = info.db_t = info.el_t = info.gr_t = info.pr_t = 0 ;
info.comp = info.nb_trans = info.nb_cone = 0 ;

return(info) ;
}



/*****************************************************************************/
/*                           fonction PrintTime()                            */
/* affiche les temps systemes et utilisateurs.                               */
/*****************************************************************************/
int tas_PrintTime(start,end,temps)
struct rusage start ,
              end   ;
long          temps ;

{
long user  = (long)(((10L*end.ru_utime.tv_sec)+(end.ru_utime.tv_usec/100000L))
            - ((10L*start.ru_utime.tv_sec)+(start.ru_utime.tv_usec/100000L))) ,
     syst  = (long)(((10L*end.ru_stime.tv_sec)+(end.ru_stime.tv_usec/100000L))
            - ((10L*start.ru_stime.tv_sec)+(start.ru_stime.tv_usec/100000L))) ;
long userM = (long)((user/10L)/60L)     ,
     userS = (long)((user/10L)%60L)     ,
     userD = (long)(user%10L)           ;
long systM = (long)((syst/10L)/60L)     ,
     systS = (long)((syst/10L)%60L)     ,
     systD = (long)(syst%10L)           ;

fprintf(stdout," %02ldmin %02lds\n",(long)(temps/60L),(long)(temps%60L)) ;
fprintf(stdout,"hitas user   : %02ld'%02ld.%ld''\n",userM,userS,userD) ;
fprintf(stdout,"      system : %02ld'%02ld.%ld''\n",systM,systS,systD) ;
fflush(stdout) ;
return(0);
}
/*****************************************************************************/
/*                           fonction tas_addcapalink()                      */
/*****************************************************************************/
void tas_addcapalink(link_list *link, float capa)
{
    caraclink_list *clink;

    clink = TAS_GETCLINK(link);
    if(clink->CLINK == TAS_NOCAPA)
        clink->CLINK = tas_getcapalink(NULL, link, 'X') + capa;
    else
        clink->CLINK += capa ;
}
/*****************************************************************************/
/*                           fonction tas_gettotalcapa()                     */
/*****************************************************************************/
float tas_gettotalcapa(lofig_list *lofig, losig_list *losig, int capatype)
{
    float capa = 0.0;
    chain_list *chain;
    
    if(!lofig)
        lofig = TAS_CONTEXT->TAS_LOFIG;
    if(elpIsCapaSig(lofig, losig, capatype)){
        return ((elpGetCapaSig(lofig, losig, capatype) + rcn_getcapa(lofig,losig)) * 1000.0);
    }else{
        for(chain = (chain_list*)getptype(losig->USER,LOFIGCHAIN)->DATA; chain; chain = chain->NEXT)
            capa += elpGetCapaFromLocon((locon_list*)chain->DATA,ELP_CAPA_TYPICAL,TAS_CASE);
        elpAddCapaSig(lofig, losig, capa, capatype);
        return ((capa  + rcn_getcapa(lofig,losig)) * 1000.0);
    }
}
/*****************************************************************************/
/*                           fonction tas_getcapalink()                      */
/*****************************************************************************/
float tas_getcapalink(lofig_list *lofig, link_list *link, char event)
{
    caraclink_list *clink;
    int capatype;
    
    switch ( event ) {
      case 'U' : //capatype = ELP_CAPA_UP;
                 capatype = ELP_CAPA_TYPICAL;
                 break;
      case 'D' : //capatype = ELP_CAPA_DN;
                 capatype = ELP_CAPA_TYPICAL;
                 break;
      default:   capatype = ELP_CAPA_TYPICAL;
                 break;
    }
    clink = TAS_GETCLINK(link);
    if(clink->CLINK != TAS_NOCAPA)// TODO : maybe CLINKU and CLIMKD...
        return clink->CLINK;
    else
        return tas_gettotalcapa(lofig, link->SIG, capatype);
}
/*****************************************************************************/
/*                           fonction tas_get_lotrs_current()                */
/*****************************************************************************/
int tas_get_lotrs_current(lotrs_list *lotrs, long width, long length, float vgs, float vds, float vbs, float *ids)
{
    int flag;
    mcc_trans_mcc *trs;
    float vdd = tas_getparam (lotrs, TAS_CASE, TP_VDDmax);
    
    trs = mcc_create_trans_mcc(lotrs, vdd, TAS_CASE,NULL, vgs, vbs );
    trs->TRLENGTH = length;
    trs->TRWIDTH = width;
    flag = mcc_mcc_ids(trs, 0.0, vds, ids);
    mcc_delete_trans_mcc(trs);
    return flag;
}
/*****************************************************************************/
/*                           fonction tas_get_current_rate()                 */
/*****************************************************************************/
float tas_get_current_rate(link_list *link)
{
    lotrs_list *lotrs1 = link->ULINK.LOTRS;
    ptype_list *ptype;
    lotrs_list *lotrs2;
    link_list *link2;
    float ids1, vgs1, vds1, vbs1;
    float ids2, vgs2, vds2, vbs2;
    float rate;
    float vdd;
    int flag1, flag2;

    ptype = getptype(lotrs1->USER, TAS_TRANS_SWITCH);
    if(!ptype) return 1.0;
    lotrs2 = ptype->DATA;

    if((lotrs1->TYPE & CNS_TN)==CNS_TN){
        vdd = tas_getparam (lotrs1, TAS_CASE, TP_VDDmax);
        vgs1 = vdd;
        vds1 = 3.0 * vdd / 4.0;
        vbs1 = 0.0;
    }else{
        vdd = tas_getparam (lotrs1, TAS_CASE, TP_VDDmax);
        vgs1 = -vdd;
        vds1 = -3.0 * vdd / 4.0;
        vbs1 = 0.0;
    }
    if((lotrs2->TYPE & CNS_TN)==CNS_TN){
        vdd = tas_getparam (lotrs2, TAS_CASE, TP_VDDmax);
        vgs2 = 3.0 * vdd / 4.0;
        vds2 = 3.0 * vdd / 4.0;
        vbs2 = -vdd / 4.0;
    }else{
        vdd = tas_getparam (lotrs2, TAS_CASE, TP_VDDmax);
        vgs2 = -3.0 * vdd / 4.0;
        vds2 = -3.0 * vdd / 4.0;
        vbs2 = vdd / 4.0;
    }
    ptype = getptype(lotrs2->USER, TAS_TRANS_LINK);
    if(!ptype){
        return 1.0;
    }else{
        link2 = ((chain_list*)ptype->DATA)->DATA;
        flag1 = tas_get_lotrs_current(lotrs1, TAS_GETWIDTH(link), TAS_GETLENGTH(link), vgs1, vds1, vbs1, &ids1);
        flag2 = tas_get_lotrs_current(lotrs2, TAS_GETWIDTH(link2), TAS_GETLENGTH(link2), vgs2, vds2, vbs2, &ids2);
        if((flag1==0) || (flag2==0))
            rate = 1.0;
        else
            rate = 1.0 + fabs(ids2/ids1);
    }
    
    return rate;
}

/*****************************************************************************/
/*                           fonction stockage polarisation                  */
/*****************************************************************************/
void tas_set_vpol( link_list *link, float v )
{
    float       vdd;
    long        longvalue;
    ptype_list *ptl;

    vdd = tas_getparam (link->ULINK.LOTRS, TAS_CASE, TP_VDDmax);
    longvalue = ((long)(v/vdd*((double)TAS_SCALE_FLOAT)));
    ptl = getptype( link->USER, TAS_LINK_VPOL );
    if( !ptl ) {
      link->USER = addptype( link->USER, TAS_LINK_VPOL, 0 );
      ptl = link->USER ;
    }
    ptl->DATA = (void*)longvalue;
}

float tas_get_vpol( link_list *link )
{
    float       vdd;
    float       floatvalue;
    long        longvalue;
    ptype_list *ptl;
    
    ptl = getptype( link->USER, TAS_LINK_VPOL );
    if( !ptl ) 
      return 0.0;

    vdd        = tas_getparam (link->ULINK.LOTRS, TAS_CASE, TP_VDDmax);
    longvalue  = (long)ptl->DATA;
    floatvalue = ((float)longvalue)*vdd/((float)TAS_SCALE_FLOAT);
    return floatvalue;
}

void tas_clean_vpol( link_list *link )
{
    ptype_list *ptl;
    for( ; link ; link = link->NEXT ) {
        ptl = getptype( link->USER, TAS_LINK_VPOL );
        if( ptl )
          link->USER = delptype( link->USER, TAS_LINK_VPOL );
    }
}

long tas_get_length_active(lotrs_list* lotrs)
{
    long lactive;
    
    elpLotrsGetShrinkDim(lotrs,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         &lactive,
                         NULL,
                         TAS_CASE
                        );

    return lactive;  
}

long tas_get_width_active(lotrs_list* lotrs, long eff_width)
{
    long wactive;
    long dweff;
    long dwact;
    long wmlt;
    long nb;

    elpGetDeltaShrinkWidth (lotrs,&dweff,&dwact,&wmlt,TAS_CASE);
        
    nb = (long)(((double)eff_width / ((double)lotrs->WIDTH + (double)dweff)) + 0.5);
    if(!nb) nb = 1;
    wactive = nb * (lotrs->WIDTH * wmlt + dwact);

    return wactive;  
}

void tas_SetPinSlew (locon_list *locon, float rise, float fall)
{
  inf_carac     *caracslope = NULL ;
  ptype_list    *ptype ;
  front_list    *front ;
  
  ptype = getptype (locon->USER, TAS_SLOPE_MAX) ; 
  front = ptype ?  (front_list*)ptype->DATA : NULL ;
  if (!front)
    return ;

  if (rise<=0 && fall<=0)
    { 
      front->FUP = TAS_CONTEXT->FRONT_CON ;
      front->FDOWN = TAS_CONTEXT->FRONT_CON ;
    }
  else
    {
      if (rise>0)
        front->FUP = rise * TTV_UNIT ;
      if (fall>0)
        front->FDOWN = fall * TTV_UNIT ;
    }

  if(TAS_CONTEXT->TAS_FIND_MIN == 'Y')
    {
      ptype = getptype (locon->USER, TAS_SLOPE_MIN) ; 
      front = ptype ?  (front_list*)ptype->DATA : NULL ;
      if (!front)
        return ;
      
      if (rise<=0 && fall<=0)
        { 
          front->FUP = TAS_CONTEXT->FRONT_CON ;
          front->FDOWN = TAS_CONTEXT->FRONT_CON ;
        }
      else
        {
          if (rise>0)
            front->FUP = rise * TTV_UNIT ;
          if (fall>0)
            front->FDOWN = fall * TTV_UNIT ;
        }
    }
}

long tas_get_pinslew(locon_list *locon, char sens)
{
    ptype_list *ptype = NULL;
    long slew;
    
    if (TAS_PATH_TYPE == 'M')
        ptype = getptype (locon->USER, TAS_SLOPE_MAX) ; 
    else if (TAS_PATH_TYPE == 'm')
        ptype = getptype (locon->USER, TAS_SLOPE_MIN) ; 

    if (ptype){
        if (sens == 'U')
            slew = ((front_list*)ptype->DATA)->FUP;
        else if (sens == 'D')
            slew = ((front_list*)ptype->DATA)->FDOWN;
    }else{
        slew = TAS_CONTEXT->FRONT_CON;
    }
    if(slew == TAS_NOFRONT)
        slew =  TAS_CONTEXT->FRONT_CON;
    return slew;
}

void tas_update_mcctemp (inffig_list *ifl)
{
  double temp;
  char msg[1024];
/*
  temp = infGetOpCond (ifl, NULL, INF_TEMP );
  if ( infValueIsValid (temp) ) {
*/
  if (inf_GetDouble(ifl, INF_OPERATING_CONDITION, INF_TEMPERATURE, &temp)) {
    if ( ELP_LOAD_FILE_TYPE != ELP_DONTLOAD_FILE ) {
      if ( (long)(1000.0*temp) != (1000.0* elpGeneral[elpTEMP])) {
        sprintf (msg," MCC_temp = %g, ELP_temp = %g",temp,elpGeneral[elpTEMP]);
        tas_error (69,msg,TAS_WARNING);
      }
    }
    MCC_TEMP = temp;
    elpGeneral[elpTEMP] = MCC_TEMP ;
    V_FLOAT_TAB[__SIM_TEMP].VALUE = temp;
  }
}

int tas_check_prop(link_list *link, double vdd, double vddin, double vt, double gate_th)
{
    if(TAS_CONTEXT->TAS_CHECK_PROP == 'N'){
        return 1;
    }else{
        if((link->ULINK.LOTRS->TYPE & CNS_TN) == CNS_TN){
            if((vddin + vt) > gate_th){
                return 1;
            }
        }else{
            if((vddin + vt) > (vdd - gate_th)){
                return 1;
            }
        }
        return 0;
    }
}

void tas_log_multivoltage (lofig_list *lofig)
{
    float alim;
    lotrs_list *lotrs;
    char *name;

    avt_log (LOGTAS, 1, "===============================\n");
    avt_log (LOGTAS, 1, "TRANSISTOR ALIM               \n");
    avt_log (LOGTAS, 1, "===============================\n");
    for (lotrs = lofig->LOTRS; lotrs; lotrs = lotrs->NEXT) {
        name = lotrs->TRNAME? lotrs->TRNAME: "???"; 
        if (cns_getlotrsalim(lotrs, 'M', &alim)) avt_log (LOGTAS, 1, "%s -> %gV\n", name, alim);
    }
}

chain_list* tas_find_corresp_link_from_lotrs( cone_list *cone, lotrs_list *lotrs, int where ) 
{
  int i ;
  branch_list *headbranch[3] ;
  branch_list *branch ;
  chain_list *chain ;
  link_list *link ;

  chain = NULL ;

  if( ( where & CNS_VDD ) == CNS_VDD )
    headbranch[0] = cone->BRVDD ;
  else
    headbranch[0] = NULL ;

  if( ( where & CNS_VSS ) == CNS_VSS )
    headbranch[1] = cone->BRVSS ;
  else
    headbranch[1] = NULL ;

  if( ( where & CNS_EXT ) == CNS_EXT )
    headbranch[2] = cone->BREXT ;
  else
    headbranch[2] = NULL ;
  
  for( i=0 ; i<=2 ; i++ ) {
    for( branch = headbranch[i] ; branch ; branch = branch->NEXT ) {
      for( link = branch->LINK ; link ; link = link->NEXT ) {
        if( link->TYPE != CNS_EXT && link->ULINK.LOTRS == lotrs )
          chain = addchain( chain, link );
      }
    }
  }

  return chain ;
}

typedef struct
{
  char *name;
  int dir;
} disable_gate_delay_info;

void tas_add_disable_gate_delay(cnsfig_list *cf)
{
  cone_list *cn;
  chain_list *maincl, *cl, *ch, *all;
  disable_gate_delay_info *dgd;
  inf_assoc *assoc;
  char *name;
  inffig_list *ifl;

  if ((ifl=getloadedinffig(cf->NAME))==NULL) return;

  all=NULL;
  maincl=inf_GetEntriesByType(ifl, INF_DISABLE_GATE_DELAY, INF_ANY_VALUES);

  while (maincl!=NULL)
  {
    name = (char *)maincl->DATA;
    inf_GetPointer (ifl, name, INF_DISABLE_GATE_DELAY, (void **)&cl);
    ch=NULL;
    while (cl!=NULL)
     {
       assoc = (inf_assoc *) cl->DATA;
       dgd=(disable_gate_delay_info *)mbkalloc(sizeof(disable_gate_delay_info));

       dgd->dir=assoc->lval;
       dgd->name=assoc->orig;
       ch=addchain(ch, dgd);
       cl=cl->NEXT;
     }

    all=addchain(all, ch);

    for (cn=cf->CONE; cn!=NULL; cn=cn->NEXT)
    {
      if (mbk_TestREGEX(cn->NAME, name)) 
      {
        cn->USER=addptype(cn->USER, TAS_DISABLE_GATE_DELAY, ch);
      }
    }
    maincl=delchain(maincl, maincl);
  }

  if (all)
   cf->USER=addptype(cf->USER, TAS_ALL_DISABLE_GATE_DELAY, all);
}

void tas_remove_disable_gate_delay(cnsfig_list *cf)
{
  cone_list *cn;
  ptype_list *pt;
  chain_list *cl, *ch;

  if ((pt=getptype(cf->USER, TAS_ALL_DISABLE_GATE_DELAY))!=NULL)
  {
    for (ch=(chain_list *)pt->DATA; ch!=NULL; ch=delchain(ch,ch))
    {
      for (cl=(chain_list *)ch->DATA; cl!=NULL; cl=delchain(cl, cl))
      {
        mbkfree(cl->DATA);
      }
    }
    cf->USER=delptype(cf->USER, TAS_ALL_DISABLE_GATE_DELAY);
  }
  for (cn=cf->CONE; cn!=NULL; cn=cn->NEXT)
    {
      cn->USER=testanddelptype(cn->USER, TAS_DISABLE_GATE_DELAY);
    }
}

int tas_disable_gate_delay(char *input, cone_list *cn, int dirin, int dirout)
{
  int mask;
  ptype_list *pt;
  chain_list *cl;
  disable_gate_delay_info *dgd;

  if ((pt=getptype(cn->USER, TAS_DISABLE_GATE_DELAY))!=NULL)
  {
    mask=((dirin+1)<<4) | (dirout+1);
    for (cl=(chain_list *)pt->DATA; cl!=NULL; cl=cl->NEXT)
    {
      dgd=(disable_gate_delay_info *)cl->DATA;
      if ((dgd->dir & mask)==mask && mbk_TestREGEX(input, dgd->name))
         return 1;
    }
  }
  return 0;
}

int tas_is_degraded_memsym(branch_list *branch, cone_list *cone, char type_tran)
{
    if(!branch || !cone){
        return 0;
    }
    if(((cone->TYPE & CNS_MEMSYM) == CNS_MEMSYM) && ((branch->TYPE & CNS_VDD_DEGRADED) == CNS_VDD_DEGRADED) && (type_tran == 'U')){
        return 1;
    }else if(((cone->TYPE & CNS_MEMSYM) == CNS_MEMSYM) && ((branch->TYPE & CNS_VSS_DEGRADED) == CNS_VSS_DEGRADED) && (type_tran == 'D')) { 
        return 1;
    }else{
        return 0;
    }
}

void tas_RemoveRC(lofig_list *lf)
{
  losig_list *ls;
  lowire_list *lw, *nlw;
  int hascache=rcn_getlofigcache( lf )!=NULL;

  for (ls=lf->LOSIG; ls!=NULL; ls=ls->NEXT)
  {
    if (hascache)
      rcn_flush_signal(lf, ls);
    else
    {
      if (ls->PRCN)
        {
          for (lw=ls->PRCN->PWIRE; lw!=NULL; lw=nlw)
          {
             nlw=lw->NEXT;
             freelowire( lw );
          }
          ls->PRCN->PWIRE=NULL;
          while( ls->PRCN->PCTC )
            delloctc( (loctc_list*)ls->PRCN->PCTC->DATA );
          ls->PRCN->PCTC=NULL;
        }
    }
  }
}

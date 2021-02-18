#include    <stdio.h>
#include    <stdlib.h>
#include    <math.h>
#include    AVT_H
#include    MUT_H
#include    INF_H
#include    SIM_H
#include    "inf_sdc.h"

#define TAS_SECTION 1
#define YAG_SECTION 2

double INFSDC_TIME_MUL = 1e-12;
double INFSDC_CAPA_MUL = 1e-12;
static ht *INFSDC_CLOCK_NAME = NULL;
int INFSDC_LINENO = 0;            /* line number in processing file */
int SDC2STB_ERRORS = 0;

extern inffig_list *__giveinf ();

/****************************************************************************/

void infsdc_gettimeunit ()
{
    char *str, *c, *tok;
    char buf[1024];

    str = V_STR_TAB[__STB_FOREIGN_TIME_UNIT].VALUE;
            
    if (!str) {
        INFSDC_TIME_MUL = 1e-12;
        INFSDC_CAPA_MUL = 1e-12;
        return;
    }

  strcpy(buf, str);
  tok=strtok_r(buf, " ", &c);

  while (tok!=NULL)
    {
      if (strcasecmp(tok,"1ps")==0) INFSDC_TIME_MUL = 1e-12;
      else if (strcasecmp(tok,"ps")==0) INFSDC_TIME_MUL = 1e-12;
      else if (strcasecmp(tok,"10ps")==0) INFSDC_TIME_MUL = 10e-12;
      else if (strcasecmp(tok,"100ps")==0) INFSDC_TIME_MUL = 100e-12;
      else if (strcasecmp(tok,"1ns")==0) INFSDC_TIME_MUL = 1e-9;
      else if (strcasecmp(tok,"ns")==0) INFSDC_TIME_MUL = 1e-9;
      else if (strcasecmp(tok,"1ff")==0) INFSDC_CAPA_MUL = 1e-15;
      else if (strcasecmp(tok,"ff")==0) INFSDC_CAPA_MUL = 1e-15;
      else if (strcasecmp(tok,"1nf")==0) INFSDC_CAPA_MUL = 1e-9;
      else if (strcasecmp(tok,"nf")==0) INFSDC_CAPA_MUL = 1e-9;
      else if (strcasecmp(tok,"1pf")==0) INFSDC_CAPA_MUL = 1e-12;
      else if (strcasecmp(tok,"pf")==0) INFSDC_CAPA_MUL = 1e-12;
      else avt_errmsg(INF_API_ERRMSG, "021", AVT_ERROR, tok);
      //avt_error("infapi", -1, AVT_ERR, "bad value '%s' for 'stbUnits'\n",tok);
  
      tok=strtok_r(NULL, " ", &c);
    }
}

/****************************************************************************/

chain_list *infsdc_listToIntChainlist (char *list)
{
    char *str;
    chain_list *c;

    str = strtok (list, " \t");
    c = addchain (NULL, (void *)(long)atoi (str));
    while ((str = strtok (NULL, " \t")) != NULL)
        c = addchain (c, (void *)(long)atoi (str));

    return c;
}

/****************************************************************************/

chain_list *infsdc_listToCharChainlistSimple (char *list, int MODE)
{
    char *str;
    chain_list *c = NULL;

    str = strtok (list, " \t");
    if (str) {
        if (MODE==TAS_SECTION)
            c = addchain (c, (void *)infTasVectName (str));
        else
            c = addchain (c, (void *)mbk_decodeanyvector(str));

        while ((str = strtok (NULL, " \t")) != NULL)
            if (MODE==TAS_SECTION)
              c = addchain (c, (void *)infTasVectName (str));
            else
              c = addchain (c, (void *)mbk_decodeanyvector(str));
        return reverse (c);
    }
    else
        return NULL;
}


/****************************************************************************/

chain_list *infsdc_getclocks (inffig_list * ifl, chain_list * obj_l)
{
    chain_list *cklist = NULL;
    chain_list *obj, *clock_defined, *ch;
    long res;
    int trouve = 0;


    clock_defined = inf_GetEntriesByType (ifl, INF_CLOCK_TYPE, INF_ANY_VALUES);
    if (clock_defined == NULL) {
        return NULL;
    }
    if (!strcmp ((char *)obj_l->DATA, "default"))
        return clock_defined;

    for (obj = obj_l; obj; obj = obj->NEXT) {
        trouve = 0;
        if ((res = gethtitem (INFSDC_CLOCK_NAME, (char *)obj->DATA)) != EMPTYHT) {
            chain_list *ch;
            for (ch=(chain_list *)res; ch!=NULL; ch=ch->NEXT)
               if (getchain(cklist, ch->DATA)==NULL) 
                cklist = addchain (cklist, ch->DATA);
            trouve = 1;
        }
        else {
            for (ch = clock_defined; ch; ch = ch->NEXT)
                if (mbk_TestREGEX( (char *)ch->DATA, (char *)obj->DATA)) { //obj->DATA == ch->DATA) {
                    cklist = addchain (cklist, (char *)ch->DATA);
                    trouve = 1;
//                    break;
                }
            if (!trouve && mbk_isregex_name((char *)obj->DATA))
               cklist = addchain (cklist, obj->DATA), trouve=1;

        }
        if (trouve == 0) {
            avt_errmsg (SDC_ERRMSG, "002", AVT_ERR, INFSDC_LINENO, (char *)obj_l->DATA);
            SDC2STB_ERRORS++;
        }
    }

    return cklist;
}

/****************************************************************************/

char *infsdc_getclock (inffig_list * ifl, char *ckname)
{
    chain_list *clock_defined, *ch;
    long res;

    if (!INFSDC_CLOCK_NAME)
        return NULL;

    res = gethtitem (INFSDC_CLOCK_NAME, (char *)ckname);
    if ((res != EMPTYHT) && (res != (long)0))    /*inconnu ou clock virtuelle */
        return ((chain_list *) res)->DATA;

    clock_defined = inf_GetEntriesByType (ifl, INF_CLOCK_TYPE, INF_ANY_VALUES);
    if (clock_defined == NULL) {
        return NULL;
    }
    for (ch = clock_defined; ch; ch = ch->NEXT)
        if (ckname == ch->DATA) {
            freechain(clock_defined);
            return ckname;
        }

    freechain(clock_defined);
    return NULL;
}

/****************************************************************************/

chain_list *infsdc_verifAmbigu (char *str)
{
    chain_list *c = NULL, *ch = NULL;
    int cmp = 0;
    char *p;

    /* il faut compter le nombre d'elements = nb d'espaces+1 pour 
     * savoir si on est dans l'ambiguite ou pas*/
    for (p = str; *p != '\0'; p++) {
        if (*p == ' ')
            cmp++;
    }
    if (cmp == 0) {                /* un seul element dans la liste */
        ch = infsdc_listToCharChainlistSimple (namealloc(str), TAS_SECTION);
        c = addchain (c, (void *)ch);
    }
    else if (cmp == 1) {        /* deux elements -> ambiguite */
        avt_errmsg (SDC_ERRMSG, "001", AVT_WAR, INFSDC_LINENO);
        SDC2STB_ERRORS++;
        p = strchr (str, ' ');
        *p = '\0';
        ch = infsdc_listToCharChainlistSimple (str, TAS_SECTION);
        c = addchain (c, (void *)ch);
        p++;                    /*  2e elem */
        ch = infsdc_listToCharChainlistSimple (p, TAS_SECTION);
        c = addchain (c, (void *)ch);
    }
    else {                        /* trois elements ou plus => c'est forcement un OU */
        ch = infsdc_listToCharChainlistSimple (str, TAS_SECTION);
        c = addchain (c, (void *)ch);
    }

    return c;
}

/****************************************************************************/

chain_list *infsdc_listToCharChainlistDouble (char *list)
{
    char *dup, *pspace, *pright, *pleft;
    chain_list *c = NULL, *ch = NULL;

    dup = (char *)mbkalloc (sizeof (char) * (strlen (list) + 1));
    strcpy (dup, list);
    /* dans le cas de la commande set_false_path il existe une ambiguite
     * sur les deux premiers elements de la liste:
     * si on a : set_false_path -through U1 -through U2
     *      ou : set_false_path -through {U1 U2}
     *      dans les deux cas le parser de Synopsys renvoie la meme liste:
     *      {U1 U2} on ne sait donc plus si c'est un OU ou un ET entre U1 et U2
     *      Il a ete decide que dans ce cas on traitait comme un ET
     *      Cependant si on a la liste {U1 U2 U3} on sait sans ambiguite
     *      qu'il s'agit d'un OU entre U1 U2 et U3.
     *      en effet le parser Synopsys cree une nouvelle liste pour chaque element:
     *      set new_list [list old_list new_elem] */

    /* on cherche donc en premier lieu si on est dans le cas ambigu ci-dessus */
    if ((pright = strchr (dup, '}')) == NULL) {    /* liste simple */
        c = infsdc_verifAmbigu (dup);
        *dup = '\0';
    }
    else {
        *pright = '\0';
        pleft = strrchr (dup, '{');
        if (pleft == dup) {        /* debut de liste */
            c = infsdc_verifAmbigu (dup);
            dup = pright + 1;
        }
        else if (*(pleft - 1) == '{') {    /* on est au debut de la liste forcement */
            c = infsdc_verifAmbigu (pleft + 1);
            pspace = strchr (pright + 1, ' ');
            dup = pspace + 1;
        }
        else {                    /* on est pas au debut de la liste, pas d'ambiguite */
            while (*dup == '{')
                dup++;
            *pright = '}';
        }
    }

    while (*dup != '\0') {
        if (*dup == '{') {        /* liste dans la liste */
            dup++;
            pright = strchr (dup, '}');
            if (pright) {
                *pright = '\0';
                ch = infsdc_listToCharChainlistSimple (dup, TAS_SECTION);
                if (ch)
                    c = addchain (c, (void *)ch);
                dup = pright + 1;
            }
            pspace = strchr (dup, ' ');
            if (pspace)
                dup = pspace + 1;
            else {                /* c'est le dernier element (seul) de la liste */
                *dup = '\0';
            }
        }
        else {
            pspace = strchr (dup, ' ');
            if (pspace) {
                *pspace = '\0';
                if ((pright = strchr (dup, '}')) != NULL)
                    *pright = '\0';
                ch = infsdc_listToCharChainlistSimple (dup, TAS_SECTION);
                c = addchain (c, (void *)ch);
                dup = pspace + 1;
            }
            else {                /* c'est le dernier element (seul) de la liste */
                ch = infsdc_listToCharChainlistSimple (dup, TAS_SECTION);
                c = addchain (c, (void *)ch);
                *dup = '\0';
            }
        }
    }
    return reverse (c);
}

/****************************************************************************/

infsdc_edges *infsdc_newEdges (chain_list * edges, chain_list * shifts)
{
    infsdc_edges *new, *head = NULL;

    if ((!edges) && (shifts)) {
        edges = addchain (NULL, (void *)1);
        edges = addchain (edges, (void *)2);
        edges = addchain (edges, (void *)3);
    }
    while (edges) {
        new = (infsdc_edges *) mbkalloc (sizeof (infsdc_edges));
        new->EDGE = (int)(long)edges->DATA;
        if (shifts) {
            new->SHIFT = (int)(long)shifts->DATA;
            shifts = shifts->NEXT;
        }
        else
            new->SHIFT = 0;

        new->NEXT = head;        /* ajout en tete de liste */
        head = new;                /* mise a jour de la tete de liste */

        edges = edges->NEXT;
    }
/*   la liste des structures retournee est dans l'ordre initial car les 
 *   chain_list donnees en argument n'ont pas subi de reverse */
    return head;
}

/****************************************************************************/

int infsdc_isPowOfTwo (int num)
{
    int n, i = 0;

    n = num;
    while (n > 0) {
        if ((n & 0x1) == 0x1)
            i++;
        n >>= 1;
    }
    if (i != 1)
        return 0;
    else
        return 1;
}

/****************************************************************************/

void infsdc_addGeneratedClock (char *name, char *source, float factor, infsdc_edges * edges_list, int dc,
                               chain_list * con)
{
    infsdc_edges *e, *ne;
    double ed[3], f;                /* tableau des fronts */
    int i;

    inffig_list *ifl;
    char *ref;
    double up, down, period;
    int val, redge, fedge;

    ifl = __giveinf ();
    ref = infsdc_getclock (ifl, source);

    if (ref && inf_GetDouble (ifl, ref, INF_CLOCK_PERIOD, &period)) {                    /* si l'horloge de reference est connue */
//        inf_GetDouble (ifl, ref, INF_CLOCK_PERIOD, &period);
        inf_GetDouble (ifl, ref, INF_MIN_RISE_TIME, &up);
        inf_GetDouble (ifl, ref, INF_MIN_FALL_TIME, &down);
        fedge=0; redge=1;

        if (((fabs (factor)) > 1) && (infsdc_isPowOfTwo ((fabs (factor)) / 1) == 1))
        {
            /* the factor is a power of 2, the rising edges of the master clock */
            /* are used to determine the edges of the generated clock */
            fedge=1;
            down = up + (period * (fabs (factor) / 2));
        }
        else {                    /* the factor is not a power of 2, the edges are scaled from the master clock */
            up = up * (fabs (factor));
            down = down * (fabs (factor));
        }
        period = period * (fabs (factor));

        if (edges_list) {
            for (i = 0, e = edges_list; e; e = ne, i++) {
                ne=e->NEXT;
                if ((e->EDGE % 2) == 0)    /* edge pair -> DOWN */
                {
                    if (i==0) redge=0; else fedge=0;
                    ed[i] = down + (period * ((e->EDGE - 1) / 2)) + (e->SHIFT * INFSDC_TIME_MUL);
                }
                else            /* edge impair -> UP */
                {
                    if (i==0) redge=1; else fedge=1;
                    ed[i] = up + (period * ((e->EDGE - 1) / 2)) + (e->SHIFT * INFSDC_TIME_MUL);
                }
                mbkfree(e);
            }

            period = ed[2] - ed[0];    /* periode entre 2 fronts montants */
            up = ed[0];
            down = ed[1];
        }

        if (dc > 0) {
            if (up < down) {
                down = up + ((dc * period) / 100.0);    /* etat haut de la duree du duty cycle */

                if (down >= period)    /* on a deborde de la periode */
                    down = down - period;
            }
            else {
                up = down + (((100 - dc) * period) / 100.0);

                if (up >= period)    /* on a deborde de la periode */
                    up = up - period;
            }
        }
        if (down >= period)
            down = down - period;
        if (up >= period)
            up = up - period;
        
        val=0;
        inf_GetInt(INF_FIG, ref, INF_CLOCK_TYPE, &val);

        if ((val & INF_CLOCK_INVERTED)!=0) {        /* option invert positionnee : */
            /* on inverse up et down       */
            f = up;
            up = down;
            down = f;
            redge=(redge+1) & 1;
            fedge=(fedge+1) & 1;
        }

        if (!name)
            name = (char *)con->DATA;
        if (con)
            infsdc_addClockSpec (name, con, up, down, period,factor<0?INF_CLOCK_INVERTED:0, infTasVectName(ref), redge, fedge);
    }

    else {
        avt_errmsg (SDC_ERRMSG, "002", AVT_ERR, INFSDC_LINENO, source);
        SDC2STB_ERRORS++;
    }
}

/****************************************************************************/

void infsdc_addClockSpec (char *name, chain_list * con, double up, double dn, double period, int flags, char *master, int redge, int fedge)
{
    static int i;
    inffig_list *ifl;
    chain_list *cklist, *ch;
    char *domain = NULL, buf[64];
    double ckperiod;
    int trouve = 0;
    long res;
    long lperiod, lckperiod;

    ifl = __giveinf ();

    cklist = inf_GetEntriesByType (ifl, INF_CLOCK_TYPE, INF_ANY_VALUES);
    if (cklist == NULL) {
        i = 1;
        sprintf (buf, "agroup%d", i++);
        domain = infTasVectName (buf);
    }
    for (ch = cklist; ch && (trouve == 0); ch = ch->NEXT) {
        if (!inf_GetDouble (ifl, (char *)ch->DATA, INF_CLOCK_PERIOD, &ckperiod)) continue;
        
        lperiod=mbk_long_round(period*1e14);
        lckperiod=mbk_long_round(ckperiod*1e14);
        if (period > ckperiod) {
            if (lperiod % lckperiod==0/*(fmod (period, ckperiod)) == 0*/) {    /* multiple de la periode */
                if (inf_GetString (ifl, (char *)ch->DATA, INF_ASYNC_CLOCK_GROUP, &domain) == 0)
                    domain = NULL;
                else
                    trouve = 1;
            }
        }
        else {
            if (lckperiod % lperiod==0/*(fmod (ckperiod, period)) == 0*/) {    /* multiple de la periode */
                if (inf_GetString (ifl, (char *)ch->DATA, INF_ASYNC_CLOCK_GROUP, &domain) == 0)
                    domain = NULL;
                else
                    trouve = 1;
            }
        }
    }
    freechain(cklist);
    if (domain == NULL) {
        sprintf (buf, "agroup%d", i++);
        domain = infTasVectName (buf);
    }
    else if (trouve)
    {
      strcpy(buf, domain);
      domain = infTasVectName (buf);
    }

     if (name!=NULL && getchain(con, name)==NULL) {                    /* clock virtuelle */
        if (name!=master)
        {
          inf_AddInt (ifl, INF_LOADED_LOCATION, name, INF_CLOCK_TYPE, INF_CLOCK_VIRTUAL|flags, NULL);
          inf_AddDouble (ifl, INF_LOADED_LOCATION, name, INF_MIN_RISE_TIME, up, NULL);
          inf_AddDouble (ifl, INF_LOADED_LOCATION, name, INF_MAX_RISE_TIME, up, NULL);
          inf_AddDouble (ifl, INF_LOADED_LOCATION, name, INF_MIN_FALL_TIME, dn, NULL);
          inf_AddDouble (ifl, INF_LOADED_LOCATION, name, INF_MAX_FALL_TIME, dn, NULL);
          inf_AddDouble (ifl, INF_LOADED_LOCATION, name, INF_CLOCK_PERIOD, period, NULL);
          inf_AddString (ifl, INF_LOADED_LOCATION, name, INF_ASYNC_CLOCK_GROUP, domain, NULL);
          if (master!=NULL)
            {
              inf_AddString(ifl, INF_LOADED_LOCATION, name, INF_MASTER_CLOCK, master, NULL);
              inf_AddInt(ifl, INF_LOADED_LOCATION, name, INF_MASTER_CLOCK_EDGES, (redge<<1)|fedge, NULL);
            }
        }
    }

    for (ch = con; ch; ch = ch->NEXT) {
        if ((char *)ch->DATA!=master)
        {
          inf_AddInt (ifl, INF_LOADED_LOCATION, (char *)ch->DATA, INF_CLOCK_TYPE, flags, NULL);
          inf_AddDouble (ifl, INF_LOADED_LOCATION, (char *)ch->DATA, INF_MIN_RISE_TIME, up, NULL);
          inf_AddDouble (ifl, INF_LOADED_LOCATION, (char *)ch->DATA, INF_MAX_RISE_TIME, up, NULL);
          inf_AddDouble (ifl, INF_LOADED_LOCATION, (char *)ch->DATA, INF_MIN_FALL_TIME, dn, NULL);
          inf_AddDouble (ifl, INF_LOADED_LOCATION, (char *)ch->DATA, INF_MAX_FALL_TIME, dn, NULL);
          inf_AddDouble (ifl, INF_LOADED_LOCATION, (char *)ch->DATA, INF_CLOCK_PERIOD, period, NULL);
          inf_AddString (ifl, INF_LOADED_LOCATION, (char *)ch->DATA, INF_ASYNC_CLOCK_GROUP, domain, NULL);
          if (master!=NULL)
            {
              inf_AddString(ifl, INF_LOADED_LOCATION, (char *)ch->DATA, INF_MASTER_CLOCK, master, NULL);
              inf_AddInt(ifl, INF_LOADED_LOCATION, (char *)ch->DATA, INF_MASTER_CLOCK_EDGES, (redge<<1)|fedge, NULL);
            }
        }
    }
    if (name==NULL) name=(char *)con->DATA;
 
    if (INFSDC_CLOCK_NAME == NULL)
        INFSDC_CLOCK_NAME = addht (64);

    res = gethtitem (INFSDC_CLOCK_NAME, name);
    if (res == EMPTYHT)
      addhtitem (INFSDC_CLOCK_NAME, name, (long)con);
    else
      avt_errmsg(INF_API_ERRMSG, "022", AVT_WARNING, name); 

    //avt_error ("sdcapi", 3, AVT_ERR, "Clock '%s' is already defined.\n", name);
}

/****************************************************************************/

void infsdc_addClockLatency (chain_list * obj_l, int mode, double delay)
{
    chain_list *cklist, *ch;
    double val;
    inffig_list *ifl;
    int ival;

    ifl = __giveinf ();
    cklist = infsdc_getclocks (ifl, obj_l);

    for (ch = cklist; ch; ch = ch->NEXT) {
        if ((mode & INFSDC_FALL) == INFSDC_FALL) {
            if ((mode & INFSDC_MIN) == INFSDC_MIN) {
/*              inf_GetDouble (ifl, (char *)ch->DATA, INF_MIN_FALL_TIME, &val);
                inf_AddDouble (ifl, INF_LOADED_LOCATION, (char *)ch->DATA, INF_MIN_FALL_TIME,
                               val + (delay * INFSDC_TIME_MUL), NULL);*/
                val=0; //inf_GetDouble (ifl, (char *)ch->DATA, INF_LATENCY_FALL_MIN, &val);
                inf_AddDouble (ifl, INF_LOADED_LOCATION, (char *)ch->DATA, INF_LATENCY_FALL_MIN, val + (delay * INFSDC_TIME_MUL), NULL);

            }
            if ((mode & INFSDC_MAX) == INFSDC_MAX) {
/*              inf_GetDouble (ifl, (char *)ch->DATA, INF_MAX_FALL_TIME, &val);
                inf_AddDouble (ifl, INF_LOADED_LOCATION, (char *)ch->DATA, INF_MAX_FALL_TIME,
                               val + (delay * INFSDC_TIME_MUL), NULL);*/
                val=0; //inf_GetDouble (ifl, (char *)ch->DATA, INF_LATENCY_FALL_MAX, &val);
                inf_AddDouble (ifl, INF_LOADED_LOCATION, (char *)ch->DATA, INF_LATENCY_FALL_MAX, val + (delay * INFSDC_TIME_MUL), NULL);
            }
        }
        if ((mode & INFSDC_RISE) == INFSDC_RISE) {
            if ((mode & INFSDC_MIN) == INFSDC_MIN) {
/*              inf_GetDouble (ifl, (char *)ch->DATA, INF_MIN_RISE_TIME, &val);
                inf_AddDouble (ifl, INF_LOADED_LOCATION, (char *)ch->DATA, INF_MIN_RISE_TIME,
                               val + (delay * INFSDC_TIME_MUL), NULL);*/
                val=0; //inf_GetDouble (ifl, (char *)ch->DATA, INF_LATENCY_RISE_MIN, &val);
                inf_AddDouble (ifl, INF_LOADED_LOCATION, (char *)ch->DATA, INF_LATENCY_RISE_MIN, val + (delay * INFSDC_TIME_MUL), NULL);
            }
            if ((mode & INFSDC_MAX) == INFSDC_MAX) {
/*                inf_GetDouble (ifl, (char *)ch->DATA, INF_MAX_RISE_TIME, &val);
                inf_AddDouble (ifl, INF_LOADED_LOCATION, (char *)ch->DATA, INF_MAX_RISE_TIME,
                               val + (delay * INFSDC_TIME_MUL), NULL);*/
                val=0; //inf_GetDouble (ifl, (char *)ch->DATA, INF_LATENCY_RISE_MAX, &val);
                inf_AddDouble (ifl, INF_LOADED_LOCATION, (char *)ch->DATA, INF_LATENCY_RISE_MAX, val + (delay * INFSDC_TIME_MUL), NULL);
            }
        }
    }
    if ((mode & INFSDC_SOURCE) != INFSDC_SOURCE) {
        for (ch = cklist; ch; ch = ch->NEXT)
        {
           if (!inf_GetInt (ifl, (char *)ch->DATA, INF_CLOCK_TYPE, &ival)) ival=0;
           inf_AddInt (ifl, INF_LOADED_LOCATION, (char *)ch->DATA, INF_CLOCK_TYPE, ival|INF_CLOCK_IDEAL, NULL);
        }
    }
}

/****************************************************************************/

static inf_stb_p_s_stab *infsdc_addsubstab (char *ckname, int reltype, int stabtype, int edgetype, double delay)
{
    inf_stb_p_s_stab *ispss;
    chain_list *time_l = NULL;
    double *dp;

    ispss = inf_stb_parse_spec_stab_alloc (NULL);
    if (ckname == NULL) {
        dp = (double *)mbkalloc (sizeof (double));
        *dp = delay;
        time_l = addchain (NULL, dp);
        ispss->TIME_LIST = time_l;
    }
    ispss->DELAY = delay;
    ispss->RELATIVITY = reltype;
    ispss->STABILITY = stabtype;
    ispss->CKNAME = ckname;
    ispss->CKEDGE = edgetype;

    return ispss;
}

/****************************************************************************/

static void infsdc_addstab (inffig_list * ifl, char *name, misc_type * mscdata, misc_type * mscck, int hzopt,
                            inf_stb_p_s_stab * stab, int stabtype, char *inftype)
{
    inf_stb_p_s *ispst, *old, *match;
    inf_stb_p_s_stab *s;

    ispst = inf_stb_parse_spec_alloc (NULL);
    ispst->STABILITY = stab;
    ispst->DATAEDGE = mscdata->edgetype;
    ispst->CKNAME = mscck->nom;
    ispst->CKEDGE = mscck->edgetype;
    ispst->SPECTYPE = stabtype;
    ispst->HZOPTION = hzopt;
    ispst->NEXT = NULL;
    if (!inf_GetPointer (ifl, name, inftype, (void **)&old)) {
        inf_AddPointer (ifl, INF_LOADED_LOCATION, name, inftype, INF_SPECIN, ispst, NULL);
    }
    else {
        if ((match = inf_stb_spec_exists (ispst, old)) == NULL)
            append ((chain_list *) old, (chain_list *) ispst);
        else {
            old = match;
            s = old->STABILITY;
            if (stab->NEXT != NULL) {
                inf_stb_parse_spec_stab_free (s);
                old->STABILITY = stab;
                ispst->STABILITY = NULL;
            }
            else {
                if (s->STABILITY == stab->STABILITY) {
                    stab->NEXT = s->NEXT;
                    s->NEXT = NULL;
                    old->STABILITY = stab;
                    inf_stb_parse_spec_stab_free (s);
                }
                else if (s->NEXT) {
                    if (s->NEXT->STABILITY == stab->STABILITY) {
                        inf_stb_parse_spec_stab_free (s->NEXT);
                        s->NEXT = stab;
                    }
                }
                else {            // stability differente et une seule stab memorisee 
                    s->NEXT = stab;
                }
                ispst->STABILITY = NULL;
            }
            inf_stb_parse_spec_del (ispst);
        }
    }
}

/****************************************************************************/

void infsdc_addDelay (char *name, char *ckname, int mode, double delay)
{
    misc_type mscdata, mscck;
    inf_stb_p_s_stab *ispss, *ispss0;
    int edge;
    inffig_list *ifl;
    char *res;

    ifl = __giveinf ();
    if (ckname) {
        res = infsdc_getclock (ifl, ckname);
        if (res) {
/*            ckname = res;
            inf_GetInt (ifl, ckname, INF_CLOCK_TYPE, &virtual);
            if ((virtual & INF_CLOCK_VIRTUAL) == INF_CLOCK_VIRTUAL) {
                avt_errmsg (SDC_ERRMSG, "003", AVT_ERR, INFSDC_LINENO);
                SDC2STB_ERRORS++;
                return;
            }
*/
        }
        else {
            avt_errmsg (SDC_ERRMSG, "002", AVT_ERR, INFSDC_LINENO, ckname);
            SDC2STB_ERRORS++;
            return;
        }
    }

    if ((mode & INFSDC_CK_FALL) == INFSDC_CK_FALL)
        edge = INF_STB_FALLING;
    else
        edge = INF_STB_RISING;

    ispss = NULL;
    ispss0 = NULL;
    mscck.nom = ckname;
    mscck.edgetype = edge;

    if ((mode & INFSDC_RISE_AND_FALL) == INFSDC_RISE_AND_FALL)
        mscdata.edgetype = INF_STB_SLOPEALL;
    else if ((mode & INFSDC_RISE) == INFSDC_RISE)
        mscdata.edgetype = INF_STB_RISING;
    else if ((mode & INFSDC_FALL) == INFSDC_FALL)
        mscdata.edgetype = INF_STB_FALLING;

    if ((mode & INFSDC_INPUT) == INFSDC_INPUT) {
        if ((mode & INFSDC_MIN) == INFSDC_MIN) {
            if (ckname == NULL)
                ispss = infsdc_addsubstab (ckname, INF_STB_NOTHING, INF_STB_UNSTABLE, edge, delay * INFSDC_TIME_MUL);
            else
                ispss = infsdc_addsubstab (ckname, INF_STB_AFTER, INF_STB_UNSTABLE, edge, delay * INFSDC_TIME_MUL);
        }
        if ((mode & INFSDC_MAX) == INFSDC_MAX) {
            if (ckname == NULL)
                ispss0 = infsdc_addsubstab (ckname, INF_STB_NOTHING, INF_STB_STABLE, edge, delay * INFSDC_TIME_MUL);
            else
                ispss0 = infsdc_addsubstab (ckname, INF_STB_AFTER, INF_STB_STABLE, edge, delay * INFSDC_TIME_MUL);
        }
        if (ispss == NULL)
            ispss = ispss0;
        else
            ispss->NEXT = ispss0;
        infsdc_addstab (ifl, name, &mscdata, &mscck, INF_STB_NOTHING, ispss, INF_STB_SPECIN, INF_SPECIN);
    }
    else if ((mode & INFSDC_OUTPUT) == INFSDC_OUTPUT) {
        if ((mode & INFSDC_MAX) == INFSDC_MAX) {
            if (ckname == NULL)
                ispss = infsdc_addsubstab (ckname, INF_STB_NOTHING, INF_STB_STABLE, edge, delay * INFSDC_TIME_MUL);
            else
                ispss = infsdc_addsubstab (ckname, INF_STB_BEFORE, INF_STB_STABLE, edge, delay * INFSDC_TIME_MUL);
        }
        if ((mode & INFSDC_MIN) == INFSDC_MIN) {
            if (ckname == NULL)
                ispss0 = infsdc_addsubstab (ckname, INF_STB_NOTHING, INF_STB_UNSTABLE, edge, delay * INFSDC_TIME_MUL);
            else
                ispss0 =
                    infsdc_addsubstab (ckname, INF_STB_AFTER, INF_STB_UNSTABLE, edge, -fabs (delay * INFSDC_TIME_MUL));
        }
        if (ispss == NULL)
            ispss = ispss0;
        else
            ispss->NEXT = ispss0;
        infsdc_addstab (ifl, name, &mscdata, &mscck, INF_STB_NOTHING, ispss, INF_STB_SPECOUT, INF_SPECOUT);
    }
}

/****************************************************************************/

void infsdc_addCaseAnalysis (char *value, chain_list * con)
{
    chain_list *c;
    char *name;

    if ((!strcmp (value, "0")) || (!strcmp (value, "zero"))) {
        for (c = con; c; c = c->NEXT) {
            name = (char *)c->DATA;
            inf_AddInt (__giveinf (), INF_LOADED_LOCATION, name, INF_STUCK, 0, NULL);
        }
    }
    else if ((!strcmp (value, "1")) || (!strcmp (value, "one"))) {
        for (c = con; c; c = c->NEXT) {
            name = (char *)c->DATA;
            inf_AddInt (__giveinf (), INF_LOADED_LOCATION, name, INF_STUCK, 1, NULL);
        }
    }
    else if ((!strcmp (value, "rise")) || (!strcmp (value, "rising"))) {
        for (c = con; c; c = c->NEXT) {
            name = (char *)c->DATA;
            inf_AddString (__giveinf (), INF_LOADED_LOCATION, name, INF_NOFALLING, NULL, NULL);
        }
    }
    else if ((!strcmp (value, "fall")) || (!strcmp (value, "falling"))) {
        for (c = con; c; c = c->NEXT) {
            name = (char *)c->DATA;
            inf_AddString (__giveinf (), INF_LOADED_LOCATION, name, INF_NORISING, NULL, NULL);
        }
    }
}

/****************************************************************************/

int infsdc_falsePathAlreadyExist (ptype_list * newfp)
{
    chain_list *ch;
    ptype_list *p, *pnew;
    int trouve = 0;
    inffig_list *ifl;


    ifl = __giveinf ();
    for (ch = ifl->LOADED.INF_FALSEPATH; (ch && (trouve == 0)); ch = ch->NEXT) {    /* parcours de la liste des faux chemins */
        trouve = 1;
        for (p = (ptype_list *) ch->DATA; (p && (trouve == 1)); p = p->NEXT) {    /* parcours du faux chemin */
            trouve = 0;
            for (pnew = newfp; (pnew && (trouve == 0)); pnew = pnew->NEXT) {    /* parcours du nouveau faux chemin */
                if ((p->DATA == pnew->DATA) && ((p->TYPE == pnew->TYPE) || (p->TYPE == INF_UPDOWN &&    /* INF_UPDOWN englobe INFUP ET INFDOWN */
                                                                            (pnew->TYPE == INF_DOWN
                                                                             || pnew->TYPE == INF_UP))))
                    trouve = 1;
            }
        }
    }
    return trouve;
}

/****************************************************************************/

void infsdc_addThroughFalsePath (ptype_list * from, chain_list * through, ptype_list * to)
{
    ptype_list *p;
    chain_list *c;
    inffig_list *ifl;

    if (through == NULL)
        return;

    for (c = (chain_list *) through->DATA; c; c = c->NEXT) {
        p = dupptypelst (from);
        p = addptype (p, INF_UPDOWN, c->DATA);
        if (through->NEXT == NULL) {    /* dernier niveau */
            if (to) {
                to->NEXT = p;
                p = to;
            }
            p = (ptype_list *) reverse ((chain_list *) p);
            if ((infsdc_falsePathAlreadyExist (p)) == 0) {
                ifl = __giveinf ();
                ifl->LOADED.INF_FALSEPATH = addchain (ifl->LOADED.INF_FALSEPATH, (void *)p);
            }
        }
        else
            infsdc_addThroughFalsePath (p, through->NEXT, to);
    }
    freeptype (from);
}

/****************************************************************************/

void infsdc_addFalsePath (chain_list * from, chain_list * through, chain_list * to, long mode)
{
    ptype_list *pfrom = NULL, *pto = NULL;
    long type = INF_UPDOWN;
    inffig_list *ifl;
    chain_list *cfrom, *cto;


    if (mode == INFSDC_RISE) {
        if (to == NULL)
            return;                /* on ne connait pas le point d'arrivee -> ne pas driver */
        else
            type = INF_UP;
    }
    else if (mode == INFSDC_FALL) {
        if (to == NULL)
            return;                /* on ne connait pas le point d'arrivee -> ne pas driver */
        else
            type = INF_DOWN;
    }
    else
        type = INF_UPDOWN;

    if (from) {
        for (cfrom = from; cfrom; cfrom = cfrom->NEXT) {
            for (cto = to; cto; cto = cto->NEXT) {
                pfrom = addptype (NULL, INF_UPDOWN, (void *)cfrom->DATA);
                pto = addptype (NULL, type, (void *)cto->DATA);    /* to est deja namealloque */
                if (through == NULL) {    /* from et to sont differents de NULL */
                    pfrom->NEXT = pto;
                    if ((infsdc_falsePathAlreadyExist (pfrom)) == 0) {
                        ifl = __giveinf ();
                        ifl->LOADED.INF_FALSEPATH = addchain (ifl->LOADED.INF_FALSEPATH, pfrom);
                    }
                }
                else
                    infsdc_addThroughFalsePath (pfrom, through, pto);
            }
        }
    }
    else if (to) {
        for (cto = to; cto; cto = cto->NEXT) {
            pto = addptype (NULL, type, (void *)cto->DATA);    /* to est deja namealloque */
            infsdc_addThroughFalsePath (pfrom, through, pto);
        }
    }
    else {                        /* plusieurs through */
        infsdc_addThroughFalsePath (pfrom, through, pto);
    }
}

/****************************************************************************/

void infsdc_addNoCheck (chain_list * con, long check)
{
    chain_list *c;
    char *name;
    int prev;

    for (c = con; c; c = c->NEXT) {
        prev = 0;
        name = (char *)c->DATA;
        inf_GetInt (__giveinf (), name, INF_NOCHECK, &prev);
        if (check & INFSDC_SETUP)
            inf_AddInt (__giveinf (), INF_LOADED_LOCATION, name, INF_NOCHECK, INF_NOCHECK_SETUP | prev, NULL);
        else if (check & INFSDC_HOLD)
            inf_AddInt (__giveinf (), INF_LOADED_LOCATION, name, INF_NOCHECK, INF_NOCHECK_HOLD | prev, NULL);
    }
}


/****************************************************************************/

void infsdc_addBypass (chain_list * con, char *mode, long event)
{
    chain_list *c;
    char *name;

    for (c = con; c; c = c->NEXT) {
        name = (char *)c->DATA;
        if (!event)
            inf_AddString (__giveinf (), INF_LOADED_LOCATION, name, INF_BYPASS, mode, NULL);
        else if (event == INFSDC_FALL)
            inf_AddString (__giveinf (), INF_LOADED_LOCATION, name, INF_NOFALLING, NULL, NULL);
        else if (event == INFSDC_RISE)
            inf_AddString (__giveinf (), INF_LOADED_LOCATION, name, INF_NORISING, NULL, NULL);
    }
}

/****************************************************************************/

void infsdc_addMulticyclePath (long flag, chain_list * from, chain_list * to, double value)
{
    chain_list *flp, *tlp;
    char *from_name, *to_name;

    for (flp = from; flp; flp = flp->NEXT) {
        from_name = infTasVectName ((char *)flp->DATA);
        for (tlp = to; tlp; tlp = tlp->NEXT) {
            to_name = infTasVectName ((char *)tlp->DATA);
            inf_AddAssociation (__giveinf (), INF_LOADED_LOCATION, INF_MULTICYCLE_PATH, "", from_name, to_name, flag,
                                value, NULL);
        }
    }
}
static void infsdc_addClockUncertainty (long flag, chain_list * from, chain_list * to, double value)
{
    chain_list *flp, *tlp, *newfrom, *newto;
    char *from_name, *to_name;

    if (strcmp((char *)from->DATA,"*")!=0)
      newfrom=infsdc_getclocks(__giveinf (), from);
    else
      newfrom=dupchainlst(from);
    if (strcmp((char *)to->DATA,"*")!=0)
      newto=infsdc_getclocks(__giveinf (), to);
    else
      newto=dupchainlst(to);
    
    for (flp = newfrom; flp; flp = flp->NEXT) {
        from_name = infTasVectName ((char *)flp->DATA);
        for (tlp = newto; tlp; tlp = tlp->NEXT) {
            to_name = infTasVectName ((char *)tlp->DATA);
            inf_AddAssociation (__giveinf (), INF_LOADED_LOCATION, INF_CLOCK_UNCERTAINTY, "", from_name, to_name, flag,
                                value, NULL);
        }
    }

    freechain(newfrom);
    freechain(newto);
}

static void infsdc_addClockGroups (char *name, chain_list * grp)
{
    chain_list *tlp, *newfrom;
    static int cg=0;
    int cg0=0;
    char *to_name, buf[256];

    while (grp!=NULL)
    {
      newfrom=infsdc_getclocks(__giveinf (), (chain_list *)grp->DATA);
      sprintf(buf,"%s%d", name==NULL?"ClockGroup":name, name==NULL?cg++:cg0++);
      for (tlp = newfrom; tlp; tlp = tlp->NEXT) 
      {
          to_name = infTasVectName ((char *)tlp->DATA);
          inf_AddString (__giveinf (), INF_LOADED_LOCATION, to_name, INF_ASYNC_CLOCK_GROUP, buf, NULL);
      }
      freechain(newfrom);
      grp=grp->NEXT;
    }
}

// ===========================================================================
// SDC Functions
// ===========================================================================

// ---------------------------------------------------------------------------
// set_load
// ---------------------------------------------------------------------------

void infsdc_set_load (char **argv)
{
    char *name = NULL;
    chain_list *con = NULL;
    float capacitance = 0;
    inffig_list *ifl;
    chain_list *ch;
    int i = 0;

    infsdc_gettimeunit ();
    
    while (argv[i]) {
        if (!strcmp (argv[i], "-min") || !strcmp (argv[i], "-max") 
                                      || !strcmp (argv[i], "-substract_pin_load") 
                                      || !strcmp (argv[i], "-pin_load") 
                                      || !strcmp (argv[i], "-wire_load")) {
            avt_errmsg (SDC_ERRMSG, "004", AVT_ERR, INFSDC_LINENO);
            i++;
        }
        else if (!strcmp (argv[i], "value")) {
            capacitance = (atof (strtok (argv[i + 1], " \t")))*INFSDC_CAPA_MUL;
            i+=2;
        }
        else if (!strcmp (argv[i], "objects")) {
            con = infsdc_listToCharChainlistSimple (argv[i + 1], TAS_SECTION);
            i+=2;
        }
        else i++;
    }
    
    ifl = __giveinf ();
    for (ch = con; ch; ch = ch->NEXT) {
        name = (char*)ch->DATA; 
        if (!strcmp (name, "default")) name = namealloc ("*");
        inf_AddDouble (ifl, INF_LOADED_LOCATION, name, INF_OUTPUT_CAPACITANCE, capacitance, NULL);
    }
    freechain(con);
    ifl->changed|=INF_CHANGED_RECOMP;
}

// ---------------------------------------------------------------------------
// set_input_transition
// ---------------------------------------------------------------------------

void infsdc_set_input_transition (char **argv)
{
    int i = 0, min = 0, max = 0, rise = 0, fall = 0;
    char *name = NULL;
    chain_list *con = NULL;
    double transition = 0;
    inffig_list *ifl;
    chain_list *ch;

    infsdc_gettimeunit ();
    
    while (argv[i]) {
        if (!strcmp (argv[i], "-rise")) {
            rise = 1;
            i++;
        }
        else if (!strcmp (argv[i], "-fall")) {
            fall = 1;
            i++;
        }
        else if (!strcmp (argv[i], "-min")) {
            min = 1;
            i++;
        }
        else if (!strcmp (argv[i], "-max")) {
            max = 1;
            i++;
        }
        else if (!strcmp (argv[i], "transition")) {
            transition = (atof (strtok (argv[i + 1], " \t"))) * INFSDC_TIME_MUL;
            i+=2;
        }
        else if (!strcmp (argv[i], "port_list")) {
            con = infsdc_listToCharChainlistSimple (argv[i + 1], TAS_SECTION);
            i+=2;
        }
        else i++;
    }
    
    ifl = __giveinf ();
    for (ch = con; ch; ch = ch->NEXT) {
        name = (char*)ch->DATA;
        if (!strcmp (name, "default")) name = strdup ("*");
        if (!rise && !fall) rise=fall=1;
        if (rise) {
            inf_AddDouble (ifl, INF_LOADED_LOCATION, name, INF_PIN_RISING_SLEW, transition, NULL);
        }
        if (fall) {
            inf_AddDouble (ifl, INF_LOADED_LOCATION, name, INF_PIN_FALLING_SLEW, transition, NULL);
        }
    }
    freechain(con);
    ifl->changed|=INF_CHANGED_RECOMP;
}

// ---------------------------------------------------------------------------
// create_clock
// ---------------------------------------------------------------------------

void infsdc_create_clock (char **argv)
{
    int i;
    char *name = NULL;
    chain_list *con = NULL;
    double up = 0, dn = 0, period = 0;


    infsdc_gettimeunit ();
    i = 0;
    while (argv[i]) {
        if (!strcmp (argv[i], "-name"))
            name = infTasVectName (argv[i + 1]);
        else if (!strcmp (argv[i], "-period"))
            period = (atof (argv[i + 1])) * INFSDC_TIME_MUL;
        else if (!strcmp (argv[i], "-waveform")) {
            up = (atof (strtok (argv[i + 1], " \t"))) * INFSDC_TIME_MUL;
            dn = (atof (strtok (NULL, " \t"))) * INFSDC_TIME_MUL;
        }
        else if (!strcmp (argv[i], "port_pin_list"))
            con = infsdc_listToCharChainlistSimple (argv[i + 1], TAS_SECTION);

        i += 2;
    }

/*    if (!name)
        name = (char *)con->DATA;*/
    if ((up == 0) && (dn == 0))
        up = (period) / 2;

    infsdc_addClockSpec (name, con, up, dn, period, 0, NULL, 0, 0);
}

// ---------------------------------------------------------------------------
// create_generated_clock
// ---------------------------------------------------------------------------

void infsdc_create_generated_clock (char **argv)
{
    int i;
    char *name = NULL, *source = NULL;
    float factor = 1;            /* pour les options: multiply_by divide_by invert 
                                   c'est le facteur multiplicatif de la periode.*/
    double dc = 0;
    chain_list *edges = NULL, *shifts = NULL;
    chain_list *con = NULL;
    infsdc_edges *edges_list = NULL;

    infsdc_gettimeunit ();
    i = 0;
    while (argv[i]) {
        if (!strcmp (argv[i], "-name"))
            name = infTasVectName (argv[i + 1]);
        else if (!strcmp (argv[i], "-source"))
            source = infTasVectName (argv[i + 1]);
        else if (!strcmp (argv[i], "-divide_by"))
            factor = (atof (argv[i + 1]));
        else if (!strcmp (argv[i], "-multiply_by"))
            factor = 1 / (atof (argv[i + 1]));
        else if (!strcmp (argv[i], "-invert"))
            factor = factor * (-1);
        else if (!strcmp (argv[i], "-duty_cycle"))
            dc = atof (argv[i + 1]);
        else if (!strcmp (argv[i], "-edges"))
            edges = infsdc_listToIntChainlist (argv[i + 1]);
        else if (!strcmp (argv[i], "-edge_shift"))
            shifts = infsdc_listToIntChainlist (argv[i + 1]);
        else if (!strcmp (argv[i], "port_pin_list")) {
            con = infsdc_listToCharChainlistSimple (argv[i + 1], TAS_SECTION);
        }
        i += 2;
    }

    edges_list = infsdc_newEdges (edges, shifts);
    infsdc_addGeneratedClock (name, source, factor, edges_list, dc, con);
    freechain(edges);
    freechain(shifts);
}

// ---------------------------------------------------------------------------
// set_clock_latency
// ---------------------------------------------------------------------------

void infsdc_set_clock_latency (char **argv)
{
    int i = 0;
    double delay = 0;
    int mode = INFSDC_RISE_AND_FALL | INFSDC_MIN_AND_MAX;
    chain_list *cklist = NULL;

    infsdc_gettimeunit ();
    while (argv[i]) {
        if (!strcmp (argv[i], "delay"))
            delay = atof (argv[i + 1]);
        else if (!strcmp (argv[i], "-source")) {
            mode |= INFSDC_SOURCE;
        }
        else if (!strcmp (argv[i], "-rise")) {
            mode &= ~INFSDC_FALL;
            mode |= INFSDC_RISE;
        }
        else if (!strcmp (argv[i], "-fall")) {
            mode &= ~INFSDC_RISE;
            mode |= INFSDC_FALL;
        }
        else if (!strcmp (argv[i], "-max")) {
            mode &= ~INFSDC_MIN;
            mode |= INFSDC_MAX;
        }
        else if (!strcmp (argv[i], "-min")) {
            mode &= ~INFSDC_MAX;
            mode |= INFSDC_MIN;
        }
        else if (!strcmp (argv[i], "-late")) {
            mode &= ~INFSDC_MIN;
            mode |= INFSDC_MAX;
        }
        else if (!strcmp (argv[i], "-early")) {
            mode &= ~INFSDC_MAX;
            mode |= INFSDC_MIN;
        }
        else if (!strcmp (argv[i], "object_list"))
            cklist = infsdc_listToCharChainlistSimple (argv[i + 1], TAS_SECTION);
        i += 2;
    }

    infsdc_addClockLatency (cklist, mode, delay);
}

// ---------------------------------------------------------------------------
// set_input_delay
// ---------------------------------------------------------------------------

void infsdc_set_input_delay (char **argv)
{
    int i;
    long mode = INFSDC_MIN_AND_MAX | INFSDC_RISE_AND_FALL | INFSDC_INPUT;
    double delay = 0;
    char *clock = NULL;
    chain_list *con = NULL;


    infsdc_gettimeunit ();
    i = 0;
    while (argv[i]) {
        if (!strcmp (argv[i], "-clock"))
        {
          chain_list *c;
          c = infsdc_listToCharChainlistSimple (argv[i + 1], TAS_SECTION);
          clock = infTasVectName ((char *)c->DATA);
          freechain(c);
        }
        else if (!strcmp (argv[i], "-clock_fall"))
            mode |= INFSDC_CK_FALL;
        else if (!strcmp (argv[i], "-rise")) {
            mode &= ~INFSDC_FALL;
            mode |= INFSDC_RISE;
        }
        else if (!strcmp (argv[i], "-fall")) {
            mode &= ~INFSDC_RISE;
            mode |= INFSDC_FALL;
        }
        else if (!strcmp (argv[i], "-min")) {
            mode &= ~INFSDC_MAX;
            mode |= INFSDC_MIN;
        }
        else if (!strcmp (argv[i], "-max")) {
            mode &= ~INFSDC_MIN;
            mode |= INFSDC_MAX;
        }
        else if (!strcmp (argv[i], "-add_delay"))
            mode |= INFSDC_ADD;
        else if (!strcmp (argv[i], "delay_value"))
            delay = atof (argv[i + 1]);
        else if (!strcmp (argv[i], "port_pin_list"))
            con = infsdc_listToCharChainlistSimple (argv[i + 1], TAS_SECTION);

        i += 2;
    }

    while (con) {
        infsdc_addDelay ((char *)con->DATA, clock, mode, delay);
        con = delchain(con, con);
    }
}

// ---------------------------------------------------------------------------
// set_output_delay
// ---------------------------------------------------------------------------

void infsdc_set_output_delay (char **argv)
{
    int i;
    long mode = INFSDC_MIN_AND_MAX | INFSDC_RISE_AND_FALL | INFSDC_OUTPUT;
    double delay = 0;
    char *clock = NULL;
    chain_list *con = NULL;


    infsdc_gettimeunit ();
    i = 0;
    while (argv[i]) {
        if (!strcmp (argv[i], "-clock"))
            clock = infTasVectName (argv[i + 1]);
        else if (!strcmp (argv[i], "-clock_fall"))
            mode |= INFSDC_CK_FALL;
        else if (!strcmp (argv[i], "-rise")) {
            mode &= ~INFSDC_FALL;
            mode |= INFSDC_RISE;
        }
        else if (!strcmp (argv[i], "-fall")) {
            mode &= ~INFSDC_RISE;
            mode |= INFSDC_FALL;
        }
        else if (!strcmp (argv[i], "-min")) {
            mode &= ~INFSDC_MAX;
            mode |= INFSDC_MIN;
        }
        else if (!strcmp (argv[i], "-max")) {
            mode &= ~INFSDC_MIN;
            mode |= INFSDC_MAX;
        }
        else if (!strcmp (argv[i], "-add_delay"))
            mode |= INFSDC_ADD;
        else if (!strcmp (argv[i], "delay_value"))
            delay = atof (argv[i + 1]);
        else if (!strcmp (argv[i], "port_pin_list"))
            con = infsdc_listToCharChainlistSimple (argv[i + 1], TAS_SECTION);

        i += 2;
    }

    while (con) {
        infsdc_addDelay ((char *)con->DATA, clock, mode, delay);
        con = delchain(con, con);
    }
}

// ---------------------------------------------------------------------------
// set_case_analysis
// ---------------------------------------------------------------------------

void infsdc_set_case_analysis (char **argv)
{
    int i, mode;
    char *value = NULL;
    chain_list *con = NULL;

    infsdc_gettimeunit ();
    i = 0;
    while (argv[i]) {
        if (!strcmp (argv[i], "value"))
            value = namealloc (argv[i + 1]);
        i += 2;
    }
    if (value!=NULL &&
       (!strcmp (value, "rise") || !strcmp (value, "rising")
        || !strcmp (value, "fall") || !strcmp (value, "falling"))) mode=TAS_SECTION;
    else mode=YAG_SECTION;

    i = 0;
    while (argv[i]) {
        if (!strcmp (argv[i], "value"))
            value = namealloc (argv[i + 1]);
        else if (!strcmp (argv[i], "port_pin_list"))
            con = infsdc_listToCharChainlistSimple (argv[i + 1], mode);
        i += 2;
    }

    infsdc_addCaseAnalysis (value, con);
    freechain(con);
}

// ---------------------------------------------------------------------------
// set_false_path
// ---------------------------------------------------------------------------

void infsdc_set_false_path (char **argv)
{
    int i;
    long mode = 0x0;
    long check = 0x0;
    chain_list *from = NULL, *to = NULL, *through = NULL;

    infsdc_gettimeunit ();
    i = 0;
    while (argv[i]) {
        if (!strcmp (argv[i], "-from"))
            from = infsdc_listToCharChainlistSimple (argv[i + 1], TAS_SECTION);
        else if (!strcmp (argv[i], "-through"))
            through = infsdc_listToCharChainlistDouble (argv[i + 1]);
        else if (!strcmp (argv[i], "-to"))
            to = infsdc_listToCharChainlistSimple (argv[i + 1], TAS_SECTION);
        else if (!strcmp (argv[i], "-rise"))
            mode = INFSDC_RISE;
        else if (!strcmp (argv[i], "-fall"))
            mode = INFSDC_FALL;
        else if (!strcmp (argv[i], "-setup"))
            check |= INFSDC_SETUP;
        else if (!strcmp (argv[i], "-hold"))
            check |= INFSDC_HOLD;
        i += 2;
    }

    if (check && to) {
        infsdc_addNoCheck (to, check);
    }
    else if (from || through || to) {
        if (to==NULL) to=addchain(to, namealloc("*"));
        if (from==NULL) from=addchain(from, namealloc("*"));
        infsdc_addFalsePath (from, through, to, mode);
    }

    freechain(from);
    freechain(to);
    freechain(through);
}

// ---------------------------------------------------------------------------
// set_disable_timing
// ---------------------------------------------------------------------------

void infsdc_set_disable_timing (char **argv)
{
    int i;
    chain_list *con = NULL;

    infsdc_gettimeunit ();
    i = 0;
    while (argv[i]) {
        if (!strcmp (argv[i], "object_list"))
            con = infsdc_listToCharChainlistSimple (argv[i + 1], TAS_SECTION);
        i += 2;
    }
    infsdc_addBypass (con, INF_ALL, 0);
}

// ---------------------------------------------------------------------------
// set_multicycle_path
// ---------------------------------------------------------------------------

void infsdc_set_multicycle_path (char **argv)
{
    int i;
    chain_list *from = NULL;
    chain_list *to = NULL;
    double value;
    long flag = INF_MULTICYCLE_SETUP | INF_MULTICYCLE_RISE | INF_MULTICYCLE_FALL;

    infsdc_gettimeunit ();
    i = 0;
    while (argv[i]) {
        if (!strcmp (argv[i], "-setup")) {
            flag &= ~INF_MULTICYCLE_HOLD;
            flag |= INF_MULTICYCLE_SETUP;
        }
        else if (!strcmp (argv[i], "-hold")) {
            flag &= ~INF_MULTICYCLE_SETUP;
            flag |= INF_MULTICYCLE_HOLD;
        }
        else if (!strcmp (argv[i], "-rise")) {
            flag &= ~INF_MULTICYCLE_FALL;
            flag |= INF_MULTICYCLE_RISE;
        }
        else if (!strcmp (argv[i], "-fall")) {
            flag &= ~INF_MULTICYCLE_RISE;
            flag |= INF_MULTICYCLE_FALL;
        }
        else if (!strcmp (argv[i], "-start")) {
            flag &= ~INF_MULTICYCLE_END;
            flag |= INF_MULTICYCLE_START;
        }
        else if (!strcmp (argv[i], "-end")) {
            flag &= ~INF_MULTICYCLE_START;
            flag |= INF_MULTICYCLE_END;
        }
        else if (!strcmp (argv[i], "-from"))
            from = infsdc_listToCharChainlistSimple (argv[i + 1], TAS_SECTION);
        else if (!strcmp (argv[i], "-to"))
            to = infsdc_listToCharChainlistSimple (argv[i + 1], TAS_SECTION);
        else if (!strcmp (argv[i], "path_multiplier"))
            value = atof (argv[i + 1]);

        i += 2;
    }
    if (((flag & INF_MULTICYCLE_END) != INF_MULTICYCLE_END) && ((flag & INF_MULTICYCLE_START) != INF_MULTICYCLE_START)) {
        if ((flag & INF_MULTICYCLE_SETUP) == INF_MULTICYCLE_SETUP)
            flag |= INF_MULTICYCLE_END;
        else
            flag |= INF_MULTICYCLE_START;
    }
    if (!from)
        from = addchain (NULL, "*");
    if (!to)
        to = addchain (NULL, "*");

    infsdc_addMulticyclePath (flag, from, to, value);

    freechain(to);
    freechain(from);
}

/****************************************************************************/

void infsdc_not_supported (char *command)
{
    avt_errmsg (SDC_ERRMSG, "000", AVT_ERR, command);
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
// OBSOLETE
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

static misc_type CURRENT_DATA = { NULL, NULL, 0, 0, NULL, 0, 0, 0, 0, 0 }, CURRENT_CK;
static int spectype;
static char *section;


/****************************************************************************/

void inf_DefineStability (chain_list * cl)
{
    char *ckname;
    int stabtype, rel, edge, ct;
    double delay;
    inf_stb_p_s_stab *ispss;

    if (CURRENT_DATA.nom == NULL) {
        avt_errmsg(INF_API_ERRMSG, "023", AVT_ERROR); 
//        avt_error ("infapi", 6, AVT_ERR, "inf_DefineStability command ignored\n");
        return;
    }

    if (cl == NULL) {
        avt_errmsg(INF_API_ERRMSG, "024", AVT_ERROR); 
//        avt_error ("infapi", 5, AVT_ERR, "missing parameter\n");
        return;
    }

    if (strcasecmp ((char *)cl->DATA, "stable") == 0)
        stabtype = INF_STB_STABLE;
    else if (strcasecmp ((char *)cl->DATA, "unstable") == 0)
        stabtype = INF_STB_UNSTABLE;
    else {
        avt_errmsg(INF_API_ERRMSG, "025", AVT_ERROR, (char *)cl->DATA); 
//        avt_error ("infapi", 5, AVT_ERR, "invalid parameter '%s'\n", (char *)cl->DATA);
        return;
    }

    cl = cl->NEXT;
    if (cl == NULL) {
        avt_errmsg(INF_API_ERRMSG, "024", AVT_ERROR); 
//        avt_error ("infapi", 5, AVT_ERR, "missing parameter\n");
        return;
    }

    delay = atof ((char *)cl->DATA);
    if (fabs (delay) >= 1)
        delay *= 1e-12;

    ct = 0;
    ckname = NULL;
    edge = 0;

    cl = cl->NEXT;
    if (cl != NULL) {

        ct = 1;
        if (strcasecmp ((char *)cl->DATA, "after") == 0)
            rel = INF_STB_AFTER;
        else if (strcasecmp ((char *)cl->DATA, "before") == 0)
            rel = INF_STB_BEFORE;
        else {
            ct = 0;
            ckname = NULL;
            edge = 0;
        }

        if (ct == 1) {
            cl = cl->NEXT;
            if (cl == NULL) {
                avt_errmsg(INF_API_ERRMSG, "024", AVT_ERROR); 
//                avt_error ("infapi", 5, AVT_ERR, "missing parameter\n");
                return;
            }

            ckname = infTasVectName ((char *)cl->DATA);

            cl = cl->NEXT;
            if (cl == NULL) {
                avt_errmsg(INF_API_ERRMSG, "024", AVT_ERROR); 
//                avt_error ("infapi", 5, AVT_ERR, "missing parameter\n");
                return;
            }

            if (strcasecmp ((char *)cl->DATA, "rising") == 0)
                edge = INF_STB_RISING;
            else if (strcasecmp ((char *)cl->DATA, "falling") == 0)
                edge = INF_STB_FALLING;
            else {
                avt_errmsg(INF_API_ERRMSG, "025", AVT_ERROR, (char *)cl->DATA); 
//                avt_error ("infapi", 5, AVT_ERR, "invalid parameter '%s'\n", (char *)cl->DATA);
                return;
            }
        }

        cl = cl->NEXT;
        if (cl != NULL) {
            avt_errmsg(INF_API_ERRMSG, "008", AVT_ERROR, (char *)cl->DATA); 
//            avt_error ("infapi", 5, AVT_WAR, "too many parameters starting at '%s'\n", (char *)cl->DATA);
        }
    }

    ispss = infsdc_addsubstab (ckname, rel, stabtype, edge, delay);

    infsdc_addstab (__giveinf (), CURRENT_DATA.nom, &CURRENT_DATA, &CURRENT_CK, INF_STB_NOTHING, ispss, spectype,
                    section);

}

void infsdc_set_clock_uncertainty (char **argv)
{
    int i;
    chain_list *from = NULL;
    chain_list *to = NULL;
    double value;
    long flag = INF_CLOCK_UNCERTAINTY_SETUP | INF_CLOCK_UNCERTAINTY_HOLD | INF_CLOCK_UNCERTAINTY_END_FALL
      | INF_CLOCK_UNCERTAINTY_END_RISE | INF_CLOCK_UNCERTAINTY_START_RISE | INF_CLOCK_UNCERTAINTY_START_FALL;

    infsdc_gettimeunit ();
    i = 0;
    while (argv[i]) {
      if (!strcmp (argv[i], "-setup")) {
        flag &= ~INF_CLOCK_UNCERTAINTY_HOLD;
        flag |= INF_CLOCK_UNCERTAINTY_SETUP;
      }
      else if (!strcmp (argv[i], "-hold")) {
        flag &= ~INF_CLOCK_UNCERTAINTY_SETUP;
        flag |=INF_CLOCK_UNCERTAINTY_HOLD;
      }
      else if (!strcmp (argv[i], "-rise")) {
        flag &= ~INF_CLOCK_UNCERTAINTY_END_FALL;
        flag |= INF_CLOCK_UNCERTAINTY_END_RISE;
      }
      else if (!strcmp (argv[i], "-fall")) {
        flag &= ~INF_CLOCK_UNCERTAINTY_END_RISE;
        flag |= INF_CLOCK_UNCERTAINTY_END_FALL;
      }
      else if (!strcmp (argv[i], "-from"))
        from = infsdc_listToCharChainlistSimple (argv[i + 1], TAS_SECTION);
      else if (!strcmp (argv[i], "-to"))
        to = infsdc_listToCharChainlistSimple (argv[i + 1], TAS_SECTION);
      else if (!strcmp (argv[i], "-rise_from"))
        {
          from = infsdc_listToCharChainlistSimple (argv[i + 1], TAS_SECTION);
          flag &= ~INF_CLOCK_UNCERTAINTY_START_FALL;
          flag |= INF_CLOCK_UNCERTAINTY_START_RISE;
        }
      else if (!strcmp (argv[i], "-fall_from"))
        {
          from = infsdc_listToCharChainlistSimple (argv[i + 1], TAS_SECTION);
          flag &= ~INF_CLOCK_UNCERTAINTY_START_RISE;
          flag |= INF_CLOCK_UNCERTAINTY_START_FALL;
        }
      else if (!strcmp (argv[i], "-rise_to"))
        {
          to = infsdc_listToCharChainlistSimple (argv[i + 1], TAS_SECTION);
          flag &= ~INF_CLOCK_UNCERTAINTY_END_FALL;
          flag |= INF_CLOCK_UNCERTAINTY_END_RISE;
        }
      else if (!strcmp (argv[i], "-fall_to"))
        {
          to = infsdc_listToCharChainlistSimple (argv[i + 1], TAS_SECTION);
          flag &= ~INF_CLOCK_UNCERTAINTY_END_RISE;
          flag |= INF_CLOCK_UNCERTAINTY_END_FALL;
        }
      else if (!strcmp (argv[i], "object_list"))
        {
          to = infsdc_listToCharChainlistSimple (argv[i + 1], TAS_SECTION);
        }
      else if (!strcmp (argv[i], "uncertainty"))
        value = atof (argv[i + 1])* INFSDC_TIME_MUL;
      
      i += 2;
    }

    if (!from)
        from = addchain (NULL, "*");
    if (!to)
        to = addchain (NULL, "*");

    infsdc_addClockUncertainty(flag, from, to, value);

    freechain(to);
    freechain(from);
}

void infsdc_set_clock_groups (char **argv)
{
    int i, async=0;
    char *groupname=NULL;
    chain_list *allgroups=NULL;

    infsdc_gettimeunit ();
    i = 0;
    while (argv[i]) {
      if (!strcmp (argv[i], "-exclusive")) {
        async=0;
      }
      else if (!strcmp (argv[i], "-asynchronous")) {
        async=1;
      }
      else if (!strcmp (argv[i], "-name")) {
        groupname=namealloc(argv[i + 1]);
      }
      else if (!strcmp (argv[i], "-group"))
        allgroups=addchain(allgroups, infsdc_listToCharChainlistSimple (argv[i + 1], TAS_SECTION));
      
      i += 2;
    }

    infsdc_addClockGroups(groupname, allgroups);

    while (allgroups!=NULL)
    {
      freechain((chain_list *)allgroups->DATA);
      allgroups=delchain(allgroups, allgroups);
    }
}



static void infsdc_set_minmax_delay (char **argv, int max)
{
    int i;
    chain_list *from = NULL, *cl;
    chain_list *to = NULL;
    double value=0;
    int val;
    splitint *si=(splitint *)&val;

    si->cval.a=0;
    si->cval.b=0;
    si->cval.c=max?INF_DIRECTIVE_BEFORE:INF_DIRECTIVE_AFTER;
    si->cval.d=0;
    
    infsdc_gettimeunit ();
    i = 0;
    while (argv[i]) {
      if (!strcmp (argv[i], "-rise")) {
        si->cval.b=INF_DIRECTIVE_RISING;
      }
      else if (!strcmp (argv[i], "-fall")) {
        si->cval.b=INF_DIRECTIVE_FALLING;
      }
      else if (!strcmp (argv[i], "-from"))
        from = infsdc_listToCharChainlistSimple (argv[i + 1], TAS_SECTION);
      else if (!strcmp (argv[i], "-to"))
        to = infsdc_listToCharChainlistSimple (argv[i + 1], TAS_SECTION);
      else if (!strcmp (argv[i], "-rise_from"))
        {
          from = infsdc_listToCharChainlistSimple (argv[i + 1], TAS_SECTION);
          si->cval.d=INF_DIRECTIVE_RISING;
        }
      else if (!strcmp (argv[i], "-fall_from"))
        {
          from = infsdc_listToCharChainlistSimple (argv[i + 1], TAS_SECTION);
          si->cval.d=INF_DIRECTIVE_FALLING;
        }
      else if (!strcmp (argv[i], "-rise_to"))
        {
          to = infsdc_listToCharChainlistSimple (argv[i + 1], TAS_SECTION);
          si->cval.b=INF_DIRECTIVE_RISING;
        }
      else if (!strcmp (argv[i], "-fall_to"))
        {
          to = infsdc_listToCharChainlistSimple (argv[i + 1], TAS_SECTION);
          si->cval.b=INF_DIRECTIVE_FALLING;
        }
      else if (!strcmp (argv[i], "delay_value"))
        value = atof (argv[i + 1])* INFSDC_TIME_MUL;
      
      i += 2;
    }

    si->cval.d|=INF_DIRECTIVE_DELAY;
    if (!from)
        from = addchain (NULL, "*");
    if (!to)
        to = addchain (NULL, "*");

    while (to!=NULL)
    {
      for (cl=from; cl!=NULL; cl=cl->NEXT)
      {
        inf_AddAssociation(__giveinf (), INF_LOADED_LOCATION, INF_DIRECTIVES, "", infTasVectName((char *)to->DATA), infTasVectName((char *)cl->DATA), val, value, NULL);
      }
      to=delchain(to, to);
    }
    freechain(from);
}

void infsdc_set_max_delay (char **argv)
{
  infsdc_set_minmax_delay(argv, 1);
}
void infsdc_set_min_delay (char **argv)
{
  infsdc_set_minmax_delay(argv, 0);
}


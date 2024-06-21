/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : VCD Version 2.00                                            */
/*    Fichier : vcd_parse.c                                                 */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include VCD_H
#include "ctype.h" 
#include "vcd_parse.h" 
#include "vcd_util.h"

#define VCD_NOT_A_TIME_LINE  (-1)
#define VCD_TAB_BASE         (128)
char *VCD_MODULE_NAME=NULL;
char  VCD_PARSE_MODULE=0;
long  VCD_INDEX=1;

/****************************************************************************/
/*     globals                                                              */
/****************************************************************************/

int       vcd_line;
t_vcd_db *vcd_db = NULL;

/****************************************************************************/
/*     externs                                                              */
/****************************************************************************/

extern int   vcddebug;
extern int   vcdparse ();
extern int   vcdrestart (FILE *f);
extern FILE *vcdin;

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

static long lpow (long b, long e)
{
    int i;
    long v = 1;

    for (i = 0; i < e; i++) v *= b; 
    return v;
}

/****************************************************************************/
                
long vcd_getindex (char *alias)
{
    long index;
    
    if (! vcd_db->IDX_HT ||
        (index = gethtitem(vcd_db->IDX_HT,sensitive_namealloc(alias))) == EMPTYHT)
      return 0;
    else
      return index;

/*    int i = 0;
    int index = 0;
    int p = 0;

    while (alias[i] != '\0')
        index += ((long)alias[i++] - 33) * lpow (94, p++);

    return index;*/
}

/****************************************************************************/

void vcd_init_db ()
{
    vcd_db = (t_vcd_db*)mbkalloc (sizeof (struct t_vcd_db));
    vcd_db->SIG_TAB = (chain_list**)mbkalloc (VCD_TAB_BASE * sizeof (chain_list*));
    memset(vcd_db->SIG_TAB, 0, VCD_TAB_BASE * sizeof (chain_list*));
    vcd_db->SIG_TAB_SZ = VCD_TAB_BASE;
    vcd_db->SIG_IDX = 0;
    vcd_db->SIG_ENTRY = NULL;

    vcd_db->TIME_ENTRY = (chain_list**)mbkalloc (VCD_TAB_BASE * sizeof (chain_list*));
    memset(vcd_db->TIME_ENTRY, 0, VCD_TAB_BASE * sizeof (chain_list*));
    vcd_db->TIME_TAB = (int*)mbkalloc (VCD_TAB_BASE * sizeof (int));
    vcd_db->TIME_TAB[0] = -1;
    vcd_db->TIME_TAB_SZ = VCD_TAB_BASE;
    vcd_db->TIME_IDX = 0;

    vcd_db->CUR_TIME_IDX = 0;
    vcd_db->CUR_STATE = NULL;
    vcd_db->NRJ_ENTRY = (float*)mbkalloc (VCD_TAB_BASE * sizeof (float));
    memset(vcd_db->NRJ_ENTRY, 0, VCD_TAB_BASE * sizeof (float));

    vcd_db->IDX_HT = addht(100);
}

/****************************************************************************/

void vcd_addvar (char *signal, char *alias)
{
//    long index = vcd_getindex (alias);
    long index;
    long tab_sz;

    if(!VCD_PARSE_MODULE) return;
    
    if (!vcd_db) vcd_init_db ();

    if((index = gethtitem(vcd_db->IDX_HT,sensitive_namealloc(alias))) != EMPTYHT){
        /*fprintf(stdout, "Doublon: %s\n", alias)*/;
    }else{
        index = VCD_INDEX++;
        addhtitem(vcd_db->IDX_HT,sensitive_namealloc(alias), index);
    }

    while (index >= vcd_db->SIG_TAB_SZ) {
        tab_sz = vcd_db->SIG_TAB_SZ;
        vcd_db->SIG_TAB_SZ += VCD_TAB_BASE;
        vcd_db->SIG_TAB = (chain_list**)mbkrealloc (vcd_db->SIG_TAB, vcd_db->SIG_TAB_SZ * sizeof (chain_list*));
        memset(&vcd_db->SIG_TAB[tab_sz], 0, VCD_TAB_BASE * sizeof (chain_list*));
    }
    
    if ((*signal) == '\\'){
        signal++;
        vcd_db->SIG_TAB[index] = addchain(vcd_db->SIG_TAB[index], namealloc(signal));
    }else{
        vcd_db->SIG_TAB[index] = addchain(vcd_db->SIG_TAB[index], signal);
    }

    vcd_db->SIG_IDX = index;
}

/****************************************************************************/

void vcd_set_ts (int time_base, float time_unit)
{
    if (!vcd_db) vcd_init_db ();

    vcd_db->TIME_BASE = time_base;
    vcd_db->TIME_UNIT = time_unit;
}

/****************************************************************************/

char vcd_getmodule (char *line)
{
  char buf[1024];
  char *tok, *c;
  char module=0;
  char *modulename=NULL;
  
  strcpy(buf, line);
  tok=strtok_r(buf, " \t", &c);
  if(tok){
      if(!strcmp(tok, "module"))
          module = 1;
  }
  while (tok && VCD_MODULE_NAME)
    {
      if(!strcmp(tok, VCD_MODULE_NAME) && module)
          modulename = tok;
      tok=strtok_r(NULL, " ", &c);
    }
  if(module && modulename){
      VCD_PARSE_MODULE = 1;
  }else{
      VCD_PARSE_MODULE = 0;
  }
  return module;
}

/****************************************************************************/

int vcd_gettime (char *line)
{
    if (line[0] == '#') return atol (line + 1);
    else return VCD_NOT_A_TIME_LINE;
}

/****************************************************************************/

char *vcd_getval (char *line, long *alias)
{
    char ref;
    int i, j, miss, size;

    if (line[0] == 'b') { // vector
        char *buf, *buf2;
        
        if (toupper(line[1]) == 'X'){
            ref = 'X';
        }else if (toupper(line[1]) == 'Z'){
            ref = 'Z';
        }else{
            ref = '0';
        }
        buf2        = alloca(strlen(line)*sizeof(char));
        for (i = 1, j = 0; line[i] != ' '; i++)
            buf2[j++]   = line[i];
        buf2[j]     = '\0';
        *alias      = vcd_getindex (line + i + 1);
        if((*alias > vcd_db->SIG_IDX) || !vcd_db->SIG_TAB[*alias]) return NULL;
        size        = vcd_getVectSize((char*)(vcd_db->SIG_TAB[*alias]->DATA));
        miss        = size - j;
        buf         = alloca((size+1)*sizeof(char));
        
        for (j = 0; j < miss; j ++)
          buf[j]    = ref;
        for (j = miss ; j <= size; j ++)
          buf[j]    = buf2[j - miss];
        return strdup (buf);
    } 
    else 
    if (strchr ("01XZUWxzuw", line[0])) {
        char buf[2];
      
        *alias = vcd_getindex (line + 1);
        if((*alias > vcd_db->SIG_IDX) || !vcd_db->SIG_TAB[*alias]) return NULL;
        buf[0] = line[0];
        buf[1] = '\0';
        return strdup (buf);
    }
    else {
	    fprintf (stderr, "[WScan] Parse error line %d\n", vcd_line);
        //exit (EXIT_FAILURE);
        return NULL;
    }
}


/****************************************************************************/

void vcd_parseline (char *line)
{
    static int time = -1;
    int lt, i;
    long index;
    trans_list *trans;
    char *val;
    
    if (!vcd_db->SIG_ENTRY){
        vcd_db->SIG_ENTRY = (trans_list**)mbkalloc ((vcd_db->SIG_IDX + 1) * sizeof (trans_list*));
        memset(vcd_db->SIG_ENTRY, 0, (vcd_db->SIG_IDX + 1) * sizeof (trans_list*));
    }

    if ((lt = vcd_gettime (line)) != VCD_NOT_A_TIME_LINE) { // time line
        time = lt;
        vcd_db->TIME_IDX++;
        if (vcd_db->TIME_IDX >= vcd_db->TIME_TAB_SZ) {
            vcd_db->TIME_TAB_SZ += VCD_TAB_BASE;
            vcd_db->TIME_ENTRY = (chain_list**)mbkrealloc (vcd_db->TIME_ENTRY, vcd_db->TIME_TAB_SZ * sizeof (chain_list*));
            vcd_db->NRJ_ENTRY = (float*)mbkrealloc (vcd_db->NRJ_ENTRY, vcd_db->TIME_TAB_SZ * sizeof (float));
            for (i = vcd_db->TIME_TAB_SZ - VCD_TAB_BASE; i < vcd_db->TIME_TAB_SZ; i++) 
                vcd_db->TIME_ENTRY[i] = NULL;
            for (i = vcd_db->TIME_TAB_SZ - VCD_TAB_BASE; i < vcd_db->TIME_TAB_SZ; i++) 
                vcd_db->NRJ_ENTRY[i] = 0.0;
            vcd_db->TIME_TAB = (int*)mbkrealloc (vcd_db->TIME_TAB, vcd_db->TIME_TAB_SZ * sizeof (int));
        }
        vcd_db->TIME_TAB[vcd_db->TIME_IDX] = time;
        return;
    }
    else { // value line
        val = vcd_getval (line, &index);
        if(!val) return;
        if (index > vcd_db->SIG_IDX || index < 0) {
	        fprintf (stderr, "[WScan] Parse error line %d\n", vcd_line);
            //exit (EXIT_FAILURE);
        }

        if(vcd_db->TIME_ENTRY[vcd_db->TIME_IDX] && (((trans_list*)vcd_db->TIME_ENTRY[vcd_db->TIME_IDX]->DATA)->INDEX == index)){
            vcd_db->SIG_ENTRY[index]->VAL = val;
        }else{
            trans        = (trans_list*)mbkalloc (sizeof (struct trans_list));
            trans->BIT   = -1;
            trans->TIME  = time;
            trans->VAL   = val;
            trans->INDEX = index;
            trans->NEXT  = vcd_db->SIG_ENTRY[index];
            vcd_db->SIG_ENTRY[index] = trans;
            vcd_db->TIME_ENTRY[vcd_db->TIME_IDX] = addchain (vcd_db->TIME_ENTRY[vcd_db->TIME_IDX], trans);
        }
    }
}

/****************************************************************************/

void vcd_update ()
{
    int i;
    trans_list *trans;
    char        buf[1024];
    chain_list *chain;

    if (!vcd_db) return;

    for (i = 0; i < vcd_db->SIG_IDX; i++) {
        if(vcd_db->SIG_ENTRY[i]){
            vcd_db->SIG_ENTRY[i] = (trans_list*)reverse ((chain_list*)vcd_db->SIG_ENTRY[i]);
            vcd_db->SIG_ENTRY[i]->PREV = NULL;
            for (trans = vcd_db->SIG_ENTRY[i]; trans->NEXT; trans = trans->NEXT)
                trans->NEXT->PREV = trans;
        }
    }

    vcd_db->SIG_HT = addht (101);
    for (i = 0; i < vcd_db->SIG_IDX; i++) {
        if(vcd_db->SIG_ENTRY[i] && vcd_db->SIG_TAB[i]){
            for(chain = vcd_db->SIG_TAB[i]; chain; chain = chain->NEXT){
                sscanf((char*)chain->DATA,"%[^[][%*d:%*d]",buf);
                addhtitem (vcd_db->SIG_HT, namealloc(buf), (long)i);
            }
        }
    }

    vcd_db->CUR_STATE = dupchainlst (vcd_db->TIME_ENTRY[0]); // init state
}

/****************************************************************************/

void vcd_dump_by_sig ()
{
    int i;
    trans_list *trans;
    chain_list *chain;

    for (i = 0; i < vcd_db->SIG_IDX; i++) {
        for(chain = vcd_db->SIG_TAB[i]; chain; chain = chain->NEXT){
            fprintf (stdout, "%s : ", (char*)chain->DATA);
        }
        for (trans = vcd_db->SIG_ENTRY[i]; trans; trans = trans->NEXT)
            fprintf (stdout, " (%d, '%s')", trans->TIME, trans->VAL);
        fprintf (stdout, "\n");
    }
}

/****************************************************************************/

void vcd_free_db ()
{
    trans_list *tr, *tr2free;
    int i;

    if (!vcd_db) return;

    for (i = 0; i < vcd_db->SIG_IDX; i++){
        if(vcd_db->SIG_TAB[i])
            freechain(vcd_db->SIG_TAB[i]);
    }
    mbkfree (vcd_db->SIG_TAB);
    for (i = 0; i < vcd_db->SIG_IDX; i++)
        if ((tr = vcd_db->SIG_ENTRY[i])) {
            while (tr) {
                tr2free = tr;
                tr = tr->NEXT;
                mbkfree (tr2free->VAL);
                mbkfree (tr2free);
            }
        }
    delht (vcd_db->SIG_HT);
    delht (vcd_db->IDX_HT);
    mbkfree (vcd_db->SIG_ENTRY);

    /* time */
    mbkfree (vcd_db->TIME_TAB);
    for (i = 0; i < vcd_db->TIME_IDX; i++)
        if (vcd_db->TIME_ENTRY[i]) 
            freechain (vcd_db->TIME_ENTRY[i]);

    mbkfree (vcd_db->TIME_ENTRY);
    mbkfree (vcd_db->NRJ_ENTRY);

    freechain (vcd_db->CUR_STATE);

    mbkfree (vcd_db);
    vcd_db = NULL;
}

/****************************************************************************/

void vcd_dump_by_time ()
{
    int i;
    chain_list *ch;
    trans_list *trans;
    chain_list *chain;

    for (i = 0; i < vcd_db->TIME_IDX; i++) {
        fprintf (stdout, "%d : ", vcd_db->TIME_TAB[i]);
        for (ch = vcd_db->TIME_ENTRY[i]; ch; ch = ch->NEXT) {
            trans = (trans_list*)ch->DATA;
            for(chain = vcd_db->SIG_TAB[trans->INDEX]; chain; chain = chain->NEXT){
                fprintf (stdout, " (%s, '%s')", (char*)chain->DATA, trans->VAL);
            }
        }
        fprintf (stdout, "\n");
    }

}

/****************************************************************************/

void vcd_handler (int sig)
{
    switch (sig) {
        case SIGBUS:
        case SIGSEGV:
            fprintf (stderr, "[WScan] Fatal error, please contact Avertec support\n");
            break;
        case SIGINT:
            fprintf (stderr, "[WScan] Program aborted by user");
            break;
    }
    fflush (stderr);
    exit (EXIT_FAILURE);
}

/****************************************************************************/

void vcd_parsefile (char *name, char *modulename)
{
    static int needs_restart = 0;

    signal (SIGSEGV, vcd_handler);
    signal (SIGBUS, vcd_handler);

    VCD_MODULE_NAME = modulename;

    if (!needs_restart)
        mbkenv ();

    if ((vcdin = fopen (name, "r"))) {
        if (needs_restart) {
            fprintf (stderr, "RESTART\n");
            vcdrestart (vcdin);
            vcd_free_db ();
        } else needs_restart = 1;
    	vcd_line = 1;
        //vcddebug = 1;
    	vcdparse ();
    	fclose (vcdin); 
        vcd_update ();
	} 
    else 
        fprintf (stderr, "[VCD ERR] can't open file '%s'\n", name);
}

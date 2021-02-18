/****************************************************************************/
/*     Environnement for derating                                           */
/****************************************************************************/

#include LIB_H

/****************************************************************************/
/*     globals                                                              */
/****************************************************************************/

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

void libenv ()
{
    char *env ;
    static int doneenv=0;

    /* positionnement des variables globales pour les deratings */

    env = V_STR_TAB[__LIB_BUS_DELIMITER].VALUE ;
    if(env != NULL){
        if(!strcmp(env, "[]") || 
           !strcmp(env, "{}") || 
           !strcmp(env, "()") || 
           !strcmp(env, "<>") || 
           !strcmp(env, "_") || 
           !strcmp(env, "-"))
            LIB_BUS_DELIM =  namealloc(env);
        else
            LIB_BUS_DELIM = namealloc("[]") ;
    }else
        LIB_BUS_DELIM = namealloc("[]") ;
    
    
    stmenv () ;
    
  if (doneenv==0)
    readlibfile("lib", lib_parse, 0);
  doneenv=1;
}

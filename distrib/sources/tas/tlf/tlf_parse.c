/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : TLF Version 1.00                                            */
/*    Fichier : tlf_parse.c                                                 */
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

#include TLF_H
#include "tlf_parse.h" 
#include "tlf_env.h" 

/****************************************************************************/
/*     defines                                                              */
/****************************************************************************/


/****************************************************************************/
/*     globals                                                              */
/****************************************************************************/

int         TLF_LINE          ;
char       *TLF_TOOLNAME      = "tlf2ttv";
char       *TLF_TOOLVERSION   = "1.0" ;
chain_list *TLF_TTVFIG_LIST   = NULL ;
char        TLF_TIME_UNIT     = TLF_NS ;
char        TLF_CAPA_UNIT     = TLF_PF ;
char        TLF_RES_UNIT      = TLF_KOHM ;

int         GCF_LINE ;
char        GCF_MODE ;
FILE       *GCF_OUT ;
ht         *ATTRIBUTE_HT = NULL ;
eqt_ctx    *tlfEqtCtx = NULL;

/****************************************************************************/
/*     externs                                                              */
/****************************************************************************/

extern int   tlf3parse() ;
extern void  tlf3restart();
extern FILE *tlf3in ;

extern int   tlf4parse() ;
extern void  tlf4restart();
extern FILE *tlf4in ;

extern int   gcfparse () ;
extern FILE *gcfin ;

extern int   tlf4debug;
/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

void parsetlf3 (char *tlf_file)
{
  avt_error("library", -1, AVT_INFO, "loading TLF3  '¤2%s¤.'\n", tlf_file);
  tlf_parse (tlf_file, 3);
}

/****************************************************************************/

void parsetlf4 (char *tlf_file)
{
  avt_error("library", -1, AVT_INFO, "loading TLF4  '¤2%s¤.'\n", tlf_file);
  tlf_parse (tlf_file, 4);
}

/****************************************************************************/

void tlf_parse (char *tlf_file, int version)
{
 static char viergetlf3 = 1 ;
 static char viergetlf4 = 1 ;
 
    if (version == 3) {
        if ((tlf3in = mbkfopen (tlf_file, TLF_IN, READ_TEXT))) {
            if(viergetlf3 == 0)
            tlf3restart(tlf3in) ;
            viergetlf3 = 0 ;
            TLF_LINE = 1 ;
            tlfEqtCtx = eqt_init (EQT_NB_VARS);
            tlf3parse () ;
            eqt_term (tlfEqtCtx);
            fclose (tlf3in) ; 
       } else { 
            fprintf (stderr, "TLF ERROR : no file %s\n", tlf_file);
            EXIT (EXIT_FAILURE) ;
       }
    }
    
    if (version == 4) {
        if ((tlf4in = mbkfopen (tlf_file, TLF_IN, READ_TEXT))) {
            if(viergetlf4 == 0)
            tlf4restart(tlf4in) ;
            viergetlf4 = 0 ;
            TLF_LINE = 1 ;
            tlfEqtCtx = eqt_init (EQT_NB_VARS);
            tlf4parse () ;
            eqt_term (tlfEqtCtx);
            fclose (tlf4in) ; 
       } else { 
            fprintf (stderr, "TLF ERROR : no file %s\n", tlf_file);
            EXIT (EXIT_FAILURE) ;
       }
    }
}

/****************************************************************************/

chain_list *tlf_load (char *filename, int version)
{
    tlf_parse (filename, version) ;
    return (TLF_TTVFIG_LIST) ;
}

/****************************************************************************/

void tlf_gcf2stb (char *file)
{
    char *filename ;
    filename = (char*)mbkalloc (TLF_BUFSIZE * sizeof (char)) ;
    
    sprintf (filename, "%s.%s", file, "gcf") ;
    
    if ((gcfin = mbkfopen (file, "tlf", READ_TEXT))) {
        sprintf (filename, "%s.%s", file, "stb") ;
        //GCF_OUT = fopen (filename, "w+") ;
        GCF_OUT = mbkfopen (file, "stb", WRITE_TEXT) ;
        GCF_LINE = 1 ;
        GCF_MODE = CLOCK_DETECT ;
        gcfparse () ;
        GCF_MODE = CLOCK_DUMP ;
        rewind (gcfin) ;
        gcfparse () ;
        GCF_MODE = DISABLE_DUMP ;
        rewind (gcfin) ;
        gcfparse () ;
        GCF_MODE = SPECIN_DUMP ;
        rewind (gcfin) ;
        gcfparse () ;
        GCF_MODE = SPECOUT_DUMP ;
        rewind (gcfin) ;
        gcfparse () ;
        fclose (gcfin) ; 
        fclose (GCF_OUT) ; 
    } else { 
        printf ("File %s does not seem to exist\n", filename) ;
        EXIT (EXIT_FAILURE) ;
    }
}

/****************************************************************************/

long ftol (float f)
{
    long l ;
    long d ;

    d = (long)(f * 10) - (long)f * 10 ;
    if (d <= 5)
        l = (long)f ;
    if (d > 5 && f > 0)
        l = (long)f + 1 ;
    if (d > 5 && f < 0)
        l = (long)f - 1 ;
        if(f == 0.0)
                l = (long)0 ;

    return (l) ;
}

/****************************************************************************/

long ftolstretch (float f)
{
    long l ;
    
    l = ftol (f) ;

    l = l < GCF_MIN_CONSTRAINT ? GCF_MIN_CONSTRAINT : l ;
    l = l > GCF_MAX_CONSTRAINT ? GCF_MAX_CONSTRAINT : l ;
    
    return (l) ;
}

/****************************************************************************/

int is_attribute (char *ident)
{
    long attribute ;
    
    if (ATTRIBUTE_HT)
        attribute = gethtitem (ATTRIBUTE_HT, ident) ;
    else
        return 0 ;

    if ((attribute != EMPTYHT) && (attribute != DELETEHT))
        return 1 ;
    else 
        return 0 ;
}

/******************************************************************************/

void set_attribute (char *attribute, char target)
{
    if (!ATTRIBUTE_HT) 
        ATTRIBUTE_HT = addht (ATTRIBUTE_HT_SIZE) ;
            
    addhtitem (ATTRIBUTE_HT, attribute, (long)target) ;
}

/******************************************************************************/

void callback_attribute (char *attribute, char *arg)
{
    char *gcc_silent; 
    gcc_silent = attribute;
    gcc_silent = arg;
    /* fprintf (stdout, "calling %s (%s)\n", attribute, arg) ; */
}

/******************************************************************************/

int isclockpin (chain_list *isclockpinchain, char *pname) 
{
    chain_list *ch ;

    for (ch = isclockpinchain ; ch ; ch = ch->NEXT) 
        if (ch->DATA == pname) 
            return (1) ;
    return (0) ;
}
    
/******************************************************************************/

char getoutputtype (char *pname, chain_list *flipflops, chain_list *latchs) 
{
    chain_list *ch ;

    for (ch = flipflops ; ch ; ch = ch->NEXT) 
        if (ch->DATA == pname) 
            return 'F' ;
    for (ch = latchs ; ch ; ch = ch->NEXT) 
        if (ch->DATA == pname) 
            return 'L' ;
    return 'B' ;
}

/******************************************************************************/
void tlf3reset()
{
}

/******************************************************************************/
void tlf4reset()
{
    if (ATTRIBUTE_HT){
        delht(ATTRIBUTE_HT);
        ATTRIBUTE_HT = NULL;
    }
    TLF_TIME_UNIT = TLF_NS ;    
    TLF_CAPA_UNIT = TLF_PF ;
}

/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TTV Verison 1                                               */
/*    Fichier : ttv_falsepath.h                                             */
/*                                                                          */
/*    (c) copyright 1995-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

typedef struct ttvfalsepath               
    {
     struct ttvfalsepath *NEXT ;
     ttvfig_list         *FIG ;
     char                *SIGIN ;    
     long                 SLOPEIN ; 
     char                *SIGOUT ;  
     long                 SLOPEOUT ;
     char                *CLOCK ;
     ptype_list        *SIGLIST ;
    }
ttvfalsepath_list ;


extern void   ttv_addfalsepath      __P(( ttvfig_list*, 
                                          char *,
                                          long,
                                          char *,
                                          long,
                                          char *,
                                          ptype_list *i,
                                          ht *nametosig,
                                          NameAllocator *NA
                                       ));

extern void   ttv_freefalsepath     __P(( ttvfig_list* ));

extern int   ttv_isfalsepath        __P(( ttvfig_list*,
                                          ttvevent_list *,
                                          ttvevent_list *,
                                          long,
                                          int clocktoclock,
                                          ttvevent_list *latch
                                       ));

ht *ttv_buildquickaccessht_forfalsepath(ttvfig_list *ttvfig, NameAllocator *NA);
void ttv_freeequickaccessht_forfalsepath(ht *h, NameAllocator *NA);


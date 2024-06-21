// USER des lonode

#define RCXLOOPCAPA 0x5243580B

// USER des lowire

#define RCX_NOWAY 0x5243580C


/*
void rcx_delayloop( lofig_list *lofig,      // La figure
                    losig_list *losig,      // Le signal
                    locon_list *locon,      // Le locon emetteur
                    char        sens,       // Le type de transition
                    RCXFLOAT       smin,    // Front d'entrée minimum
                    RCXFLOAT       smax,    //                maximum
                    RCXFLOAT       vmax,    // La tension finale
                    RCXFLOAT       vt,      // Le seuil d'un transistor
                    RCXFLOAT       extcapa, // La capacité externe
                    RCXFLOAT      *dmax,    // Le délai maximum calculé
                    RCXFLOAT      *dmin,    //          minimum
                    RCXFLOAT      *fmax,    // Le front maximum calculé
                    RCXFLOAT      *fmin,    //          minimum
                    locon_list   **cmax,    // Là où dmax a été trouvé
                    locon_list   **cmin     //       dmin
                  );

RCXFLOAT rcx_loopelmcapa( losig_list  *losig, 
                       long         node,
                       char         sens,      // Le type de transition
                       RCXFLOAT        smin,      // Front d'entrée minimum
                       RCXFLOAT        smax,      //                maximum
                       RCXFLOAT        vmax,      // La tension finale
                       RCXFLOAT        vt,        // Le seuil d'un transistor
                       RCXFLOAT        extcapa,   // La capacité externe
                       RCXFLOAT        coefctc,
                       lowire_list *wire
                     );

void  rcx_loopelmore( losig_list *losig, 
                      locon_list *source, 
                      char  sens,      // Le type de transition
                      RCXFLOAT smin,      // Front d'entrée minimum
                      RCXFLOAT smax,      //                maximum
                      RCXFLOAT vmax,      // La tension finale
                      RCXFLOAT vt,        // Le seuil d'un transistor
                      RCXFLOAT extcapa,   // La capacité externe
                      RCXFLOAT coefctc,
                      RCXFLOAT *dmax,
                      RCXFLOAT *dmin,
                      locon_list **cmax,
                      locon_list **cmin
                    );
*/

extern void rcx_delayloop( lofig_list *lofig, losig_list *losig, locon_list *locon, rcx_slope *slopemiller, RCXFLOAT smax, RCXFLOAT smin, RCXFLOAT vmax, RCXFLOAT vt, RCXFLOAT extcapa, RCXFLOAT *dmax, RCXFLOAT *dmin, RCXFLOAT *fmax, RCXFLOAT *fmin, locon_list **cmax, locon_list **cmin );
RCXFLOAT rcx_loopelmcapa( losig_list *losig, long node, rcx_slope *slopemiller, RCXFLOAT smax, RCXFLOAT smin, RCXFLOAT vmax, RCXFLOAT vt, RCXFLOAT extcapa, RCXFLOAT coefctc, lowire_list *wire );
void rcx_loopelmore( losig_list *losig, locon_list *source, rcx_slope *slopemiller, RCXFLOAT smax, RCXFLOAT smin, RCXFLOAT vmax, RCXFLOAT vt, RCXFLOAT extcapa, RCXFLOAT coefctc, RCXFLOAT *dmax, RCXFLOAT *dmin, locon_list **cmax, locon_list **cmin);
void rcx_clearnoloop( losig_list* );
void rcx_makenolooprec( losig_list*, lonode_list*, lowire_list* );
void rcx_makenoloop( losig_list* );
void rcx_loopelmdelay( losig_list *losig, long start, RCXFLOAT *dmax, RCXFLOAT *dmin, locon_list **cmax, locon_list **cmin, lowire_list *wire, RCXFLOAT ed);
void rcx_loopelmclean( losig_list *losig );
locon_list* rcx_loopgetlocon( lonode_list *ptnode );

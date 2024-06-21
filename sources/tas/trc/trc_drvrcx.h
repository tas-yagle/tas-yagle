// Ouverture du fichier
rcxfile* rcx_init_driver( lofig_list *lofig, int iscache );

// Fermeture du fichier
void rcx_drive_end( rcxfile *file, lofig_list *lofig );

// Affiche la liste des instances
void rcxprintinstance( rcxfile *file, lofig_list *lofig );

// Sort une ligne dans le fichier
void rcx_file_print( rcxfile *file, ... );

// Alloue une structure rcxfile
rcxfile* rcx_file_alloc( void );

// Renvoie une liste chainée des losig, les externes, puis null, puis les 
// internes.
chain_list* rcx_driver_sort_losig( lofig_list *lofig );

// Sort le séparateur signaux externes / internes
void rcx_end_external( rcxfile *file );

// Sort l'en tête d'un signal
void rcx_drive_signal_header( rcxfile *file, losig_list *losig, rcx_list *rcx );

// Sort la fin d'un signal
void rcx_drive_signal_end( rcxfile *file, losig_list *losig );

// Sort le début de la description d'un RC
void rcx_drive_begin_net( rcxfile *file );

// Sort la fin de la description d'un RC
void rcx_drive_end_net( rcxfile *file );

// Drive une résistance
void rcx_drive_wire( rcxfile *file, int n1, int n2, float r, float c );

// Drive une capacité à la masse
void rcx_drive_ground_capa( rcxfile *file, int n, float c );

// Drive une capacité de couplage ni
void rcx_drive_ctcni_capa( rcxfile *file, int n, float c );

// Drive une capacité de couplage
void rcx_drive_ctc_capa( rcxfile *file, int n, float c, char *agrname, int nodeagr );

// Drive l'origine des signaux
void rcxprintorigin( rcxfile *file, losig_list *losig );

// Drive les signaux bellow
void rcxprintbellow( rcxfile *file, losig_list *losig );

// Drive les locon
void rcxprintlocon( rcxfile *file, rcx_list *rcxdata );

// Fonctions internes
int rcxneeddriveloins( loins_list *loins );
void rcx_vect( char *s );
char trc_getlocondir(locon_list*);

#define RCX_NAME_FOR_DRIVE 0x5243581D

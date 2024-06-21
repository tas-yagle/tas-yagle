
#define CTK_AGR_IDX 200509291
#define CTK_AGR_BUF 1024

#define CTKAGRSEPAR " \n"

typedef struct {
  int           PB_MAX ;
  int           NBSIG ;
  ttvsig_list **TABSIG ;
} agrstbfiledrive ;

typedef struct {
  int            NBLINE ;
  int            ERROR ;
  ttvevent_list *EVENT ;
  int            NBSIG ;
  ttvsig_list  **TABSIG ;
} agrstbfileparse ;

typedef union {
  agrstbfiledrive DRIVE ;
  agrstbfileparse PARSE ;
} agrstbfileaccess ;

typedef struct {
  FILE             *FD ;
  stbfig_list      *STBFIG ;
  agrstbfileaccess  ACCESS ;
} agrstbfile ;


void stb_ctk_parse_agression( stbfig_list *stbfig );
void stb_agr_info( agrstbfile *file, ... );
agrstbfile* stb_ctk_open_agr_file( stbfig_list *stbfig, char *mode );
void stb_ctk_agr_set_index( ttvsig_list *ttvsig, int index );
int stb_ctk_agr_get_index( ttvsig_list *ttvsig );
void stb_ctk_clear_agr_index( ttvsig_list *ttvsig );
void stb_ctk_agr_headers( agrstbfile *file );
void stb_ctk_agr_signals( agrstbfile *file );
void stb_ctk_agr_clean( agrstbfile *file );
void stb_ctk_agr_clean( agrstbfile *file );
void stb_ctk_agr_drive( agrstbfile *file );
void stb_ctk_close_agr_file( agrstbfile *file );
void stb_ctk_drive_agression( stbfig_list *stbfig );
int stb_ctk_parse_header( agrstbfile *file, char *line );
int stb_ctk_parse_signals( agrstbfile *file, char *line );
int stb_ctk_parse_agr( agrstbfile *file, char *line );
void stb_ctk_agr_nbsignals( agrstbfile *file );

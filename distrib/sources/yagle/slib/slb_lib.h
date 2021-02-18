#ifndef SLIB
#define SLIB

#include MUT_H
#include EQT_H

#define FALSE 0
#define TRUE  1

#define SLIB_PIN_UP    0
#define SLIB_PIN_DOWN  1 
#define SLIB_PIN_LEFT  2 
#define SLIB_PIN_RIGHT 3

#define SLIB_T_LINE   2
#define SLIB_T_ARC    3
#define SLIB_T_CIRCLE 4
#define SLIB_T_PIN    5

#define MOTIF_T_LINE 2
#define MOTIF_T_ARC  3


// structure de calcul pour les transformations sur les symbols

typedef struct vecteur {
    double  ANGLE;
    double  NORME;
    int     SIGX;
    long    X;
    long    Y;
    } vecteur ;

// structures propres a MOTIF

typedef struct motif_arc{
    long   CORNER_X;
    long   CORNER_Y;
    long   R;
    long   ALPHA;
    long   DELTA;
    } motif_arc ;

typedef struct motif_line{
    long   START_X;
    long   START_Y;
    long   END_X;
    long   END_Y;
    } motif_line;

typedef union {
    motif_line   *MLINE;
    motif_arc    *MARC;
    void   *MOBJ;
    } motif_obj;

typedef struct motif_draw {
    struct motif_draw *NEXT;
    int                TYPE;
    motif_obj          DATA;
    } motif_draw ;
    
typedef struct motif_symlist {
    struct motif_symlist *NEXT;
    char                 *NAME;
    motif_draw           *DRAW;
    } motif_symlist ;

typedef struct motif_lib {
    char          *NAME;
    motif_symlist *SYMB;
    } motif_lib ;

// structures propres a SLIB

typedef struct slib_box {
    long    X_MIN;
    long    X_MAX;
    long    Y_MIN;
    long    Y_MAX;
    } slib_box;

typedef struct pin{
    char   *NAME;
    long   X;
    long   Y;
    int    DIRECTION;
    } pin ;

typedef struct circle{
    long   X;
    long   Y;
    long   RADIUS;
    } circle ;
    
typedef struct arc{
    long   START_X;
    long   START_Y;
    long   END_X;
    long   END_Y;
    long   CENTER_X;
    long   CENTER_Y;
    } arc ;

typedef struct line{
    long   START_X;
    long   START_Y;
    long   END_X;
    long   END_Y;
    } line;

typedef union {
    line   *SLINE;
    arc    *SARC;
    circle *SCIRCLE;
    pin    *SPIN;
    void   *SOBJ;
    } graph_obj;

typedef struct draw_list {
    struct draw_list   *NEXT;
    int                TYPE;
    graph_obj          DATA;
    } draw_list ;

typedef struct sub_list {
    struct sub_list    *NEXT;
    struct symbol_list *ROOT;
    long                X;
    long                Y;
    long                ANGLE;
    } sub_list ;

typedef struct symbol_list {
    struct symbol_list  *NEXT;
    char                *NAME;
    sub_list            *SUB;
    draw_list           *DRAW;
    motif_symlist       *MOTIF;
    slib_box           *BOX;
    } symbol_list ;

   
typedef struct library {
    char         *NAME;
    symbol_list  *SYMB;
    int           GRID;
    int           REFHEIGHT;
    } library ;    

// fonction assurant le parse du fichier slib
library     *slib_get_library();
// 
// fonctions 'outils' notaement pour EQT et la 'route_grid' de SLIB
void         slib_set_function(eqt_ctx *ctx);
int          slib_sign(float value);
void         slib_set_lib_grid(library *ptlib, int grid_value);
long         slib_apply_grid(double value, int grid_value);

// fonctions d'allocations des differents champs de la structure
library     *slib_create_lib(char *name);
void         slib_put_symbol(library *ptlib, symbol_list *ptsym);
symbol_list *slib_add_symbol(library *ptlib, char *name);
void         slib_add_subsymbol(library *ptlib, symbol_list *ptsym, char *name, double center_x, double center_y, double angle);
void         slib_add_draw_line(symbol_list *ptsym, double arg_0, double arg_1, double arg_2, double arg_3, int grid_value);
void         slib_add_draw_arc(symbol_list *ptsym, double arg_0, double arg_1, double arg_2, double arg_3, double arg_4, double arg_5, int grid_value);
void         slib_add_draw_circle(symbol_list *ptsym, double arg_0, double arg_1, float arg_2, int grid_value);
void         slib_add_draw_pin(symbol_list *ptsym, char *name, double x, double y, int direction, int grid_value);

// liberation des allocations memoires
void         slib_free_box(symbol_list *ptsym);            
void         slib_free_motif(motif_symlist *ptm_sym);
void         slib_free_subsymbols(symbol_list *ptsym);
void         slib_free_draw(symbol_list *ptsym);
void         slib_free_symbols(library *ptlib);
void         slib_free_library(library *ptlib);

// fonctions permettant de gerer les coordonnees de points apres les subsymbols
void         slib_vecteur(vecteur *v, double dalpha, double xdebut, double ydebut, double xfinal, double yfinal);
void         slib_coordonnee(vecteur *v, int grid, double dx, double dy);

//fonction d'appel d'un symbol et de mise a plat
pin         *slib_getpin(symbol_list *ptsym, char *name);   
symbol_list *slib_getsymbol(library *ptlib, char *name);
symbol_list *slib_get_flat_symbol(library *ptlib, char *name);
void         slib_flatsymbol(symbol_list *ptsym, long dx,long dy, long dalpha,symbol_list *ptresult);
void         slib_flatcalcul(vecteur *name, double dx, double dy, double dalpha, long x, long y);
void         slib_xbounding_box(slib_box *ptm_box, long value);
void         slib_ybounding_box(slib_box *ptm_box, long value);
void         slib_arc_box(symbol_list *ptresult,long x1,long y1,long x2,long y2,long x3,long y3);
void         slib_line_box(symbol_list *ptresult,long x1,long y1,long x2,long y2);
void         slib_circle_box(symbol_list *ptresult,long x,long y,long R);

// fonctions de conversion d'une structure SLIB en une structure MOTIF

motif_symlist *slib_motifconvert(symbol_list *ptsym);
void           slib_add_motif_line(motif_draw *ptm_draw, long startx,long starty,long endx,long endy);
void           slib_add_motif_arc(motif_draw *ptm_draw, long startx,long starty,long endx,long endy,long centerx,long centery);
void           slib_add_motif_circle(motif_draw *ptm_draw, long cx, long cy, long rayon);
//void           slib_add_motif_pin();

// fonctions d'affichages et de test graphique
void         slib_make_motif_file(library *ptlib);
void         slib_display_motif(FILE *file, library *ptlib, symbol_list *ptsym);
void         slib_write_motif(FILE *file,motif_symlist *ptm_sym);

// fonctions d'affichages et de test thread
void         slib_view_structslib(library *ptlib);
void         slib_view_structmotif(library *ptlib);
void         slib_view_all(library *ptlib);
void         slib_return_box(symbol_list *pts);


// library cree par le parser
library     *slib_load_library(char *buff);

extern library *SLIB_LIBRARY;
#endif


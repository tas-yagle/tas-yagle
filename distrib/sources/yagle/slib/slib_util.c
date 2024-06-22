#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include AVT_H
#include SLB_H
#include "slib_util.h"

extern int slibdebug ;
int slibParseLine = 1 ;

int slibparse();
eqt_ctx *slibEqtCtx;

/******************************************************************************/
/* Fonctions pour le parse de SLIB                                            */
/******************************************************************************/

/******************************************************************************/
/* Ajout d'expressions dans EQT                                               */
/******************************************************************************/

double eqt_ceil(double p)
{
  double r ;
  
  r = (int)(p);
  
  if ( r < p )
          r = r + 1 ;
  
  return r ;
}

double eqt_sqrt(double p)
{
  return sqrt(p) ;
} 

double eqt_maximum(double p, double q)
{
  double r ;

 if ( p >= q )
         r = p ;
 else
         r = q ;

 return r ;
}

double eqt_minimum(double p, double q)
{
 double r ;

 if ( p <= q )
         r = p ;
 else
         r = q ;

 return r ;
}

void    slib_set_function(eqt_ctx *ctx)
{
    eqt_addfunction(ctx,"ceil",&eqt_ceil);
    eqt_addfunction(ctx,"sqrt",&eqt_sqrt);
    eqt_addfunction2(ctx,"maximum",&eqt_maximum);
    eqt_addfunction2(ctx,"minimum",&eqt_minimum);
}

/******************************************************************************/
/* Application des arrondis de slib                                           */
/******************************************************************************/

int slib_sign(float value)
{
  int sign;
  
  if (value < 0 )
     sign = -1; 
  else 
     sign = 1; 

  return sign ;  
}       // end of  slib_sign



void slib_set_lib_grid(library *ptlib, int grid_value)
{
   ptlib->GRID = grid_value;
}       // end of  slib_set_lib_grid

long slib_apply_grid(double value, int grid_value)
{
  if(value == 0)
          return 0;
  else if (value < 0 )
          return (long) ( value * grid_value - 0.5 ) ;
  else if (value > 0)
          return (long) ( value * grid_value + 0.5 ) ;

  return 0;
}       // end of  apply_slib_grid

/******************************************************************************/
/* FOnction allouant la memoire pour SLIB                                     */
/******************************************************************************/

void  slib_put_symbol(library *ptlib, symbol_list *ptsym)
{
    ptsym->NEXT  = ptlib->SYMB;
    ptlib->SYMB  = ptsym;
    
}       // end of  slib_put_symbol

symbol_list *slib_add_symbol(library *ptlib, char *name)
{
    symbol_list *ptsym;
    
    ptsym = (symbol_list *)mbkalloc(sizeof(symbol_list));
    ptsym->NEXT  = ptlib->SYMB;
    ptsym->NAME  = namealloc(name);
    ptsym->SUB   = NULL;
    ptsym->DRAW  = NULL;
    ptsym->MOTIF = NULL;
    ptsym->BOX   = NULL;
    ptlib->SYMB = ptsym;
    return (ptsym);
}       // end of  slib_add_symbol

library *slib_create_lib(char *name)
{
    library *ptlib;
    
    ptlib = (library *)mbkalloc(sizeof(library));
    ptlib->NAME = namealloc(name);
    ptlib->SYMB = NULL;
    ptlib->GRID = 0;
    ptlib->REFHEIGHT=0;
    return (ptlib);
}       // end of  slib_create_lib

void slib_add_subsymbol(library *ptlib, symbol_list *ptsym, char *name, double center_x, double center_y, double angle)
{
    sub_list    *ptsub;
    symbol_list *symbols;
    symbol_list *ptroot;
    
    name = namealloc(name);
    
    for(symbols=ptlib->SYMB;symbols;symbols=symbols->NEXT)
    {
      if(symbols->NAME == name)
      {
        ptroot=symbols;
        break;
      } // end of if(ptroot->NAME == name)
    }   // end for(symbols=LIBRARY->SYMB;symbols; ...
    ptsub = (sub_list *)mbkalloc(sizeof(sub_list));
    ptsub->NEXT   = ptsym->SUB;
    ptsub->ROOT   = ptroot;
    ptsub->X      = slib_apply_grid(center_x, ptlib->GRID);
    ptsub->Y      = slib_apply_grid(center_y, ptlib->GRID);
    ptsub->ANGLE  = angle;
    ptsym->SUB    = ptsub;
}       // end of  slib_add_subsymbol

void slib_add_draw_line(symbol_list *ptsym, double arg_0, double arg_1, double arg_2, double arg_3, int grid_value)
{
    draw_list *ptdraw;
    line      *ptdata;
    
    ptdraw = (draw_list *)mbkalloc(sizeof (struct draw_list));
    ptdraw->NEXT = ptsym->DRAW;
    ptdraw->TYPE = SLIB_T_LINE;
     
    ptdata = (line *)mbkalloc(sizeof (struct line));
    ptdata->START_X = slib_apply_grid(arg_0, grid_value) ;
    ptdata->START_Y = slib_apply_grid(arg_1, grid_value) ;
    ptdata->END_X   = slib_apply_grid(arg_2, grid_value) ;
    ptdata->END_Y   = slib_apply_grid(arg_3, grid_value) ;
    ptdraw->DATA.SLINE = ptdata;
    ptsym->DRAW  = ptdraw;
    
}   // end of slib_add_draw_line

void slib_add_draw_arc(symbol_list *ptsym, double arg_0, double arg_1, double arg_2, double arg_3, double arg_4, double arg_5, int grid_value)
{
    draw_list *ptdraw;
    arc *ptdata;

    ptdraw = (draw_list *)mbkalloc(sizeof (struct draw_list));
    ptdraw->NEXT = ptsym->DRAW;
    ptdraw->TYPE = SLIB_T_ARC;

    ptdata = (arc *)mbkalloc(sizeof (struct arc));
    ptdata->START_X = slib_apply_grid(arg_0, grid_value) ;
    ptdata->START_Y = slib_apply_grid(arg_1, grid_value) ;
    ptdata->END_X   = slib_apply_grid(arg_2, grid_value) ;
    ptdata->END_Y   = slib_apply_grid(arg_3, grid_value) ;
    ptdata->CENTER_X= slib_apply_grid(arg_4, grid_value) ;
    ptdata->CENTER_Y= slib_apply_grid(arg_5, grid_value) ;
    ptdraw->DATA.SARC = ptdata;
    ptsym->DRAW  = ptdraw;
    
}   // end of  void slib_add_draw_arc

void slib_add_draw_circle(symbol_list *ptsym, double arg_0, double arg_1, float arg_2, int grid_value)
{
    draw_list *ptdraw;
    circle *ptdata;
    
    ptdraw = (draw_list *)mbkalloc(sizeof (struct draw_list));
    ptdraw->NEXT = ptsym->DRAW;
    ptdraw->TYPE = SLIB_T_CIRCLE;

    ptdata = (circle *)mbkalloc(sizeof (struct circle));
    ptdata->X     = slib_apply_grid(arg_0, grid_value) ;
    ptdata->Y     = slib_apply_grid(arg_1, grid_value) ;
    ptdata->RADIUS= slib_apply_grid(arg_2, grid_value) ;
    ptdraw->DATA.SCIRCLE = ptdata;
    ptsym->DRAW  = ptdraw;
    
}   // end of  void slib_add_draw_circle

void slib_add_draw_pin(symbol_list *ptsym, char *name, double x, double y, int dir, int grid_value)
{
    draw_list *ptdraw;
    pin       *ptdata;
    
    ptdraw = (draw_list *)mbkalloc(sizeof (struct draw_list));
    ptdraw->NEXT = ptsym->DRAW;
    ptdraw->TYPE = SLIB_T_PIN;
    

    ptdata = (pin *)mbkalloc(sizeof (struct pin));
    ptdata->NAME     = name ;
    ptdata->X        = slib_apply_grid(x, grid_value) ;
    ptdata->Y        = slib_apply_grid(y, grid_value) ;
    ptdata->DIRECTION= dir ;
    ptdraw->DATA.SPIN = ptdata;
    ptsym->DRAW  = ptdraw;

}   // end of  void slib_add_draw_pin

/******************************************************************************/
/* FOnction de liberation de memoire pour SLIB                                */
/******************************************************************************/
void        slib_free_motif_draw(motif_symlist *ptsym)
{
    motif_draw   *ptdraw;
    motif_draw   *ptnextdraw;
    
    for(ptdraw=ptsym->DRAW;ptdraw;ptdraw=ptnextdraw)
          {
               ptnextdraw = ptdraw->NEXT;
               mbkfree(ptdraw->DATA.MOBJ);
               mbkfree(ptdraw);
          } // end for(ptdraw=ptsym->DRAW;ptdraw;ptdraw=ptdraw->NEXT)
}           // end of  slib_free_draw(symbol_list *ptsym)

void        slib_free_motif(motif_symlist *ptm_sym)
{
    motif_symlist *nextptm_sym;
    
    for(;ptm_sym;ptm_sym=nextptm_sym)
      {
       slib_free_motif_draw(ptm_sym);
       nextptm_sym = ptm_sym->NEXT;
       mbkfree(ptm_sym);
      } // end for(ptm_sym=ptsym->MOTIF;ptm_sym;ptm_sym=nextptm_sym)
}       // end slib_free_motif(symbol_list *ptsym)
       
void        slib_free_box(symbol_list *ptsym)            
{    
    mbkfree(ptsym->BOX);
}

void        slib_free_subsymbols(symbol_list *ptsym)
{
    sub_list    *ptsub;
    sub_list    *nextptsub;
    
    for(ptsub=ptsym->SUB;ptsub;ptsub=nextptsub)
      {
        nextptsub = ptsub->NEXT;
        mbkfree(ptsub);
      }     // end for(ptsub=ptsym->SUB;ptsub;ptsub=ptsub->NEXT)
}           // end of  slib_free_subsymbols(symbol_list *ptsym)

void        slib_free_draw(symbol_list *ptsym)
{
    draw_list   *ptdraw;
    draw_list   *ptnextdraw;
    
    for(ptdraw=ptsym->DRAW;ptdraw;ptdraw=ptnextdraw)
          {
               ptnextdraw = ptdraw->NEXT;
               mbkfree(ptdraw->DATA.SOBJ);
               mbkfree(ptdraw);
          } // end for(ptdraw=ptsym->DRAW;ptdraw;ptdraw=ptdraw->NEXT)
}           // end of  slib_free_draw(symbol_list *ptsym)

void        slib_free_symbols(library *ptlib)
{
    symbol_list *ptsym;
    symbol_list *ptnextsym;
    
    for(ptsym=ptlib->SYMB;ptsym;ptsym=ptnextsym)
      {
              ptnextsym = ptsym->NEXT;
              slib_free_subsymbols(ptsym);
              slib_free_draw(ptsym);
              slib_free_motif(ptsym->MOTIF);
              slib_free_box(ptsym);
              mbkfree(ptsym);
      }         // end for(ptsym=ptlib->SYMB;ptsym;ptsym=ptsym->NEXT)
}               // end of  slib_free_symbols(library *ptlib)

void        slib_free_library(library *ptlib)
{
    slib_free_symbols(ptlib);
    mbkfree(ptlib);
}               // end of  slib_free_library(library *ptlib)

/******************************************************************************/
/* FOnction gerant les calcul pour les sub-symbols et la trigo                */
/******************************************************************************/

void slib_vecteur(vecteur *v, double dalpha, double xdebut, double ydebut, double xfinal, double yfinal)
{
    // donne l'angle et la norme pour deux points donnes
    double x, y;

    x = xfinal - xdebut;
    y = yfinal - ydebut ;

    if (x == 0)
      { v->SIGX = 1; }
    else 
       { v->SIGX = x / sqrt(pow(x,2)) ; }

    if ( x == 0 && y ==0 )
       { v->NORME = 0 ; v->ANGLE = 0 ; }
    else
       {
         v->NORME = sqrt( pow(x,2) + pow(y,2) )  ; //correction de sigX en (PI -ANGLE) sinon erreur en cas de dalpha changeant le signe.
         v->ANGLE = ( asin( y / v->NORME ) + (dalpha * PI / 180) ) ;
       }
}

void slib_coordonnee(vecteur *v, int grid, double dx, double dy)
{
    v->X = slib_apply_grid( (dx + v->SIGX * (v->NORME * cos(v->ANGLE)) / grid ) , grid) ;
    v->Y = slib_apply_grid( (dy +           (v->NORME * sin(v->ANGLE)) / grid ) , grid) ;

} // end of slib_coordonnee

/******************************************************************************/
/* FOnction d'appel de symbols                                                */
/******************************************************************************/
symbol_list *slib_getsymbol(library *ptlib, char *name)
{
  symbol_list *ptsym;
  
  name = namealloc(name);
  
  for(ptsym=ptlib->SYMB;ptsym;ptsym=ptsym->NEXT) { 
          if(ptsym->NAME == name) break; 
  } // end for(ptsym=ptlib->SYMB;ptsym;ptsym=ptsym->NEXT)

  return ptsym ;
}    // end of slib_getsymbol

void slib_xbounding_box(slib_box *ptm_box, long value)
{
    if(ptm_box->X_MIN > value)
        { ptm_box->X_MIN = value; }
    if(ptm_box->X_MAX < value)
        { ptm_box->X_MAX = value; }
}       // end of slib_xbounding_box

void slib_ybounding_box(slib_box *ptm_box, long value)
{
    if(ptm_box->Y_MIN > value)
        { ptm_box->Y_MIN = value; }
    if(ptm_box->Y_MAX < value)
        { ptm_box->Y_MAX = value; }
}       // end of slib_ybounding_box

void slib_flatcalcul(vecteur *name, double dx, double dy, double dalpha, long x, long y)
{
  slib_vecteur(name,dalpha,0,0,x,y);
  slib_coordonnee(name,1,dx,dy);
}

void slib_circle_box(symbol_list *ptresult,long x,long y,long R)
{
  slib_xbounding_box(ptresult->BOX,x-R);
  slib_xbounding_box(ptresult->BOX,x+R);
  slib_ybounding_box(ptresult->BOX,y-R);
  slib_ybounding_box(ptresult->BOX,y+R);
}

void slib_arc_box(symbol_list *ptresult,long ax,long ay,long bx,long by,long cx,long cy)
{
  vecteur *vecteur_c2a;
  vecteur *vecteur_c2b;

  vecteur_c2a = (vecteur *)mbkalloc(sizeof(vecteur));
  vecteur_c2b = (vecteur *)mbkalloc(sizeof(vecteur));
  
  slib_vecteur(vecteur_c2a,0,cx,cy,ax,ay);
  slib_vecteur(vecteur_c2b,0,cx,cy,bx,by);

  if( vecteur_c2a->ANGLE * vecteur_c2b->ANGLE > 0 )
    {
     slib_xbounding_box(ptresult->BOX,ax);
     slib_xbounding_box(ptresult->BOX,bx);
     slib_ybounding_box(ptresult->BOX,ay);
     slib_ybounding_box(ptresult->BOX,by);
    }
  else
    {
     slib_xbounding_box(ptresult->BOX,vecteur_c2a->NORME);
     slib_ybounding_box(ptresult->BOX,ay);
     slib_ybounding_box(ptresult->BOX,by);
    }      
}

void slib_line_box(symbol_list *ptresult,long x1,long y1,long x2,long y2)
{
  slib_xbounding_box(ptresult->BOX,x1);
  slib_xbounding_box(ptresult->BOX,x2);
  slib_ybounding_box(ptresult->BOX,y1);
  slib_ybounding_box(ptresult->BOX,y2);
}

void slib_flatsymbol(symbol_list *ptsym, long dx,long dy, long dalpha,symbol_list *ptresult)
{
  sub_list    *ptsub;
  draw_list   *ptdraw;

  for(ptdraw=ptsym->DRAW;ptdraw;ptdraw=ptdraw->NEXT)
    {
    if(ptdraw->TYPE == SLIB_T_LINE)
      {                     
        line *ptdata;
        vecteur *start;
        vecteur *end;
        
        start = (vecteur *)mbkalloc(sizeof(vecteur));
        end   = (vecteur *)mbkalloc(sizeof(vecteur));
        ptdata = ptdraw->DATA.SLINE;
        
        slib_flatcalcul(start,dx,dy,dalpha,ptdata->START_X,ptdata->START_Y);
        slib_flatcalcul(end,dx,dy,dalpha,ptdata->END_X,ptdata->END_Y);
        
        slib_add_draw_line(ptresult,start->X,start->Y,end->X,end->Y,1);
        
        slib_line_box(ptresult,start->X,start->Y,end->X,end->Y);
        
        mbkfree(start);
        mbkfree(end);
      }// end  if(ptdraw->TYPE == SLIB_T_LINE)
    if(ptdraw->TYPE == SLIB_T_ARC)
      {                     
        arc *ptdata;
        vecteur *start;
        vecteur *end;
        vecteur *center;
        
        start  = (vecteur *)mbkalloc(sizeof(vecteur));
        end    = (vecteur *)mbkalloc(sizeof(vecteur));
        center = (vecteur *)mbkalloc(sizeof(vecteur));
        ptdata = ptdraw->DATA.SARC;

        slib_flatcalcul(start,dx,dy,dalpha,ptdata->START_X,ptdata->START_Y);
        slib_flatcalcul(end,dx,dy,dalpha,ptdata->END_X,ptdata->END_Y);
        slib_flatcalcul(center,dx,dy,dalpha,ptdata->CENTER_X,ptdata->CENTER_Y);
        
        slib_add_draw_arc(ptresult,start->X,start->Y,end->X,end->Y,center->X,center->Y,1);
        
        slib_arc_box(ptresult,start->X,start->Y,end->X,end->Y,center->X,center->Y);
                
        mbkfree(start);
        mbkfree(end);
        mbkfree(center);
      }// end  if(ptdraw->TYPE == SLIB_T_ARC)
    if(ptdraw->TYPE == SLIB_T_CIRCLE)
      {                     
        circle *ptdata;
        vecteur *rond;

        rond   = (vecteur *)mbkalloc(sizeof(vecteur));
        ptdata = ptdraw->DATA.SCIRCLE;

        slib_flatcalcul(rond,dx,dy,dalpha,ptdata->X,ptdata->Y);
        
        slib_add_draw_circle(ptresult,rond->X,rond->Y,ptdata->RADIUS,1);

        slib_circle_box(ptresult,rond->X,rond->Y,ptdata->RADIUS);

        mbkfree(rond);
      }// end  if(ptdraw->TYPE == SLIB_T_CIRCLE)
    if(ptdraw->TYPE == SLIB_T_PIN)
      {                     
        pin *ptdata;
        vecteur *connecteur;

        connecteur = (vecteur *)mbkalloc(sizeof(vecteur));
        ptdata     = ptdraw->DATA.SPIN;
              
        slib_flatcalcul(connecteur,dx,dy,dalpha,ptdata->X,ptdata->Y);
        
        slib_add_draw_pin(ptresult,ptdata->NAME,connecteur->X,connecteur->Y,ptdata->DIRECTION,1);

        mbkfree(connecteur);
      }// end  if(ptdraw->TYPE == SLIB_T_PIN)
    }    // end for(ptdraw=ptsym->DRAW;ptdraw;ptdraw=ptdraw->NEXT)
  for(ptsub=ptsym->SUB;ptsub;ptsub=ptsub->NEXT)
    {
      long sub_x, sub_y, sub_alpha;
      sub_x     = ptsub->X  + dx   ;
      sub_y     = ptsub->Y  + dy   ;
      sub_alpha = ptsub->ANGLE + dalpha ;
      
      slib_flatsymbol(ptsub->ROOT,sub_x,sub_y,sub_alpha,ptresult);
      
    }    // end for(ptsub=ptsym->SUB;ptsub;ptsub=ptsub->NEXT)
}       // end of slib_flatsymbol

pin         *slib_getpin(symbol_list *ptsym, char *name)
{
    draw_list *ptdraw;

    name = namealloc(name);

    for(ptdraw=ptsym->DRAW;ptdraw;ptdraw=ptdraw->NEXT)
    {
       if(ptdraw->TYPE == SLIB_T_PIN ) 
               if(ptdraw->DATA.SPIN->NAME == name)
                       return ptdraw->DATA.SPIN ;
    }
    return NULL;
}
void slib_alloc_box(symbol_list *ptresult)
{
   slib_box   *pt_box;

   pt_box    = (slib_box *)mbkalloc(sizeof(slib_box));
   ptresult->BOX  = pt_box;  
   ptresult->BOX->X_MIN = 0;
   ptresult->BOX->X_MAX = 0;
   ptresult->BOX->Y_MIN = 0;
   ptresult->BOX->Y_MAX = 0;
}

symbol_list *slib_get_flat_symbol(library *ptlib, char *name)
{
  symbol_list *ptsym;            //symbol de ptlib que l'on parcourent
  symbol_list *ptresult = NULL;  // symbol que l'on retourne
  
  for(ptsym=ptlib->SYMB;ptsym;ptsym=ptsym->NEXT)
  {
    if(ptsym->NAME == name)
    {  
        
        ptresult  = (symbol_list *)mbkalloc(sizeof(symbol_list));
        ptresult->NEXT = NULL;
        ptresult->NAME = namealloc(name);
        ptresult->SUB  = NULL;
        ptresult->DRAW = NULL;
        ptresult->MOTIF= NULL;
        slib_alloc_box(ptresult);
        slib_flatsymbol(ptsym,0,0,0,ptresult);      
     }  // end of if(ptsym->NAME= name)
  }     // end for(ptsym=ptlib->SYMB:ptsym;ptsym=ptsym->NEXT)
  
  return (ptresult);

}       // end of slib_get_flat_symbol

/******************************************************************************/
/* FOnction de convertion a MOTIF sur un symbol a plat                        */
/******************************************************************************/

void slib_add_motif_line(motif_draw *ptm_draw,long x1,long y1,long x2,long y2)
{
    motif_line *ptm_data;
    ptm_data = (motif_line *)mbkalloc(sizeof (struct motif_line));
    
    y1 = - y1;
    y2 = - y2;
    
    ptm_data->START_X = x1 ;
    ptm_data->START_Y = y1 ;
    ptm_data->END_X   = x2 ;
    ptm_data->END_Y   = y2 ;

    ptm_draw->DATA.MLINE = ptm_data ;
}       // end of slib_add_motif_line

void slib_add_motif_arc(motif_draw *ptm_draw,long ax,long ay,long bx,long by,long cx,long cy)
{
    vecteur *vecteur_c2a;
    vecteur *vecteur_c2b;
    long  delta ;
    
    motif_arc *ptm_data;
    ptm_data = (motif_arc *)mbkalloc(sizeof (struct motif_arc));
    
    ay = - ay;
    by = - by;
    cy = - cy;

    vecteur_c2a = (vecteur *)mbkalloc(sizeof(vecteur));
    vecteur_c2b = (vecteur *)mbkalloc(sizeof(vecteur));
    
    slib_vecteur(vecteur_c2a,0,cx,cy,ax,ay);
    slib_vecteur(vecteur_c2b,0,cx,cy,bx,by);
    
    ptm_data->CORNER_X = cx - ( vecteur_c2a->NORME ) ;
    ptm_data->CORNER_Y = cy - ( vecteur_c2a->NORME ) ;
    ptm_data->ALPHA = ( vecteur_c2a->ANGLE * 180 * 64 / PI ) ;
    ptm_data->DELTA = ( vecteur_c2b->ANGLE * 180 * 64 / PI ) ;
    ptm_data->R = vecteur_c2a->NORME ;
    
    if (ptm_data->ALPHA * ptm_data->DELTA > 0)
      {
            if( abs(ay-cy) < abs(by-cy) )
                    ptm_data->CORNER_Y = ptm_data->CORNER_Y + slib_sign(ptm_data->ALPHA) * 2 * slib_sign(ay-cy) * ( ay - cy ) ;
            else
                    ptm_data->CORNER_Y = ptm_data->CORNER_Y + slib_sign(ptm_data->ALPHA) * 2 * slib_sign(by-cy) * ( by - cy ) ;
      }  // end of if (ptm_data->ALPHA * ptm_data->DELTA > 0)
    
    delta = abs( ptm_data->ALPHA - ptm_data->DELTA );
    if( ptm_data->DELTA < ptm_data->ALPHA )
         ptm_data->ALPHA = ptm_data->DELTA; 

    ptm_data->DELTA = delta ;
    
    mbkfree(vecteur_c2a);
    mbkfree(vecteur_c2b); 

    ptm_draw->DATA.MARC = ptm_data ;
}       // end of slib_add_motif_arc

void slib_add_motif_circle(motif_draw *ptm_draw,long cx, long cy, long rayon)
{
    motif_arc *ptm_data;
    ptm_data = (motif_arc *)mbkalloc(sizeof (struct motif_arc));
   
    cy = - cy;
    
    ptm_data->CORNER_X = cx - rayon ;
    ptm_data->CORNER_Y = cy - rayon ;
    ptm_data->ALPHA = 0 ;
    ptm_data->DELTA = 360 * 64 ;
    ptm_data->R = rayon ;

    ptm_draw->DATA.MARC = ptm_data ;
}       // end of slib_add_motif_circle
    
// cette fonction prend un ptsym deja a plat
motif_symlist *slib_motifconvert(symbol_list *ptsym) 
{
    draw_list     *ptdraw;     // draw du symbol SLIB
    motif_symlist *ptmotif;    // structutre motif que l'on va affecter
    motif_draw    *ptm_draw;  // draw du symbol MOTIF

    ptmotif  = (motif_symlist *)mbkalloc(sizeof(motif_symlist));
    
    ptmotif->NAME = ptsym->NAME;
    ptmotif->NEXT = NULL;
    ptmotif->DRAW = NULL;

    for(ptdraw= ptsym->DRAW;ptdraw;ptdraw=ptdraw->NEXT)
      {
      if(ptdraw->TYPE == SLIB_T_LINE)
      {                     
        line       *ptdata;
        ptdata    = ptdraw->DATA.SLINE;
        
        ptm_draw = (motif_draw *)mbkalloc(sizeof(motif_draw));
        ptm_draw->NEXT = ptmotif->DRAW;
        ptm_draw->TYPE = MOTIF_T_LINE ;
        slib_add_motif_line(ptm_draw, ptdata->START_X,ptdata->START_Y,ptdata->END_X,ptdata->END_Y);
        
        ptmotif->DRAW = ptm_draw ;
      }// end  if(ptdraw->TYPE == SLIB_T_LINE)
    if(ptdraw->TYPE == SLIB_T_ARC)
      {                     
        arc       *ptdata;
        ptdata   = ptdraw->DATA.SARC;
        
        ptm_draw = (motif_draw *)mbkalloc(sizeof(motif_draw));
        ptm_draw->NEXT = ptmotif->DRAW;
        ptm_draw->TYPE = MOTIF_T_ARC ;
        slib_add_motif_arc(ptm_draw, ptdata->START_X,ptdata->START_Y,ptdata->END_X,ptdata->END_Y,ptdata->CENTER_X,ptdata->CENTER_Y);
        
        ptmotif->DRAW = ptm_draw ;
      }// end  if(ptdraw->TYPE == SLIB_T_ARC)
    if(ptdraw->TYPE == SLIB_T_CIRCLE)
      {                     
        circle *ptdata;
        ptdata = ptdraw->DATA.SCIRCLE;
        
        ptm_draw = (motif_draw *)mbkalloc(sizeof(motif_draw));
        ptm_draw->NEXT = ptmotif->DRAW;
        ptm_draw->TYPE = MOTIF_T_ARC ;
        slib_add_motif_circle(ptm_draw,ptdata->X,ptdata->Y,ptdata->RADIUS);

        ptmotif->DRAW = ptm_draw ;
      }// end  if(ptdraw->TYPE == SLIB_T_CIRCLE)
    if(ptdraw->TYPE == SLIB_T_PIN)
      {                     
       /* pin *ptdata;
        ptdata = ptdraw->DATA.SPIN;
        
        ptm_draw = (motif_draw *)mbkalloc(sizeof(motif_draw));
        ptm_draw->NEXT = ptmotif->DRAW;
        ptm_draw->TYPE = NULL ;
        slib_add_motif_pin();

        ptmotif->DRAW = ptm_draw ;*/
      }  // end  if(ptdraw->TYPE == SLIB_T_PIN)
    }    // end for(ptdraw=ptsym->DRAW;ptdraw;ptdraw=ptdraw->NEXT)
    return ptmotif;
}        // end of slib_motifconvert

/******************************************************************************/
/* Affichage des valeurs SLIB ou MOTIF par thread                             */
/******************************************************************************/

void slib_return_box(symbol_list *pts)
{
  draw_list *ptdraw;
  long largeur, hauteur;

  largeur = pts->BOX->X_MAX - pts->BOX->X_MIN;
  hauteur = pts->BOX->Y_MAX - pts->BOX->Y_MIN;

  printf("symbol SLIB : %s\n",pts->NAME);
  printf("\tLARGEUR du symbol SLIB : %ld\n",largeur);
  printf("\tHAUTEUR du symbol SLIB : %ld\n",hauteur);

  for(ptdraw=pts->DRAW;ptdraw;ptdraw=ptdraw->NEXT)
     {
       if(ptdraw->TYPE == SLIB_T_PIN)
         {
           printf("\t");
           printf("NOM du pin : %s  ",ptdraw->DATA.SPIN->NAME);
           printf("Direction : %d  ",ptdraw->DATA.SPIN->DIRECTION);
           printf("X : %ld  ",ptdraw->DATA.SPIN->X);
           printf("Y : %ld  ",ptdraw->DATA.SPIN->Y);
           printf("\n");
         }// end of if(ptdraw->TYPE == SLIB_T_PIN)
     }    // end for(ptdraw=pts->DRAW;pts;pts=pts->NEXT)
}         // end of slib_return_box

void slib_view_structslib(library *ptlib)
{
  symbol_list *ptsym;
  
  for(ptsym=ptlib->SYMB;ptsym;ptsym=ptsym->NEXT)
     {
     symbol_list *pts ;
     draw_list   *ptdraw;
     
     pts = slib_get_flat_symbol(ptlib,ptsym->NAME);
     printf("symbol SLIB : %s\n",pts->NAME);
     
     for(ptdraw=pts->DRAW;ptdraw;ptdraw=ptdraw->NEXT)
       {   
       if(ptdraw->TYPE == SLIB_T_LINE )
                printf("\tLIGNE : X1 = %ld ; Y1 = %ld ; X2 = %ld ; Y2 = %ld ;\n",ptdraw->DATA.SLINE->START_X,ptdraw->DATA.SLINE->START_Y,ptdraw->DATA.SLINE->END_X,ptdraw->DATA.SLINE->END_Y);  
       if(ptdraw->TYPE == SLIB_T_ARC )
               printf("\tARC    : X1 = %ld ; Y1 = %ld ; X2 = %ld ; Y2 = %ld ; X3 = %ld ; Y3 = %ld ;\n",ptdraw->DATA.SARC->START_X,ptdraw->DATA.SARC->START_Y,ptdraw->DATA.SARC->END_X,ptdraw->DATA.SARC->END_Y,ptdraw->DATA.SARC->CENTER_X,ptdraw->DATA.SARC->CENTER_Y);
       if(ptdraw->TYPE == SLIB_T_CIRCLE )
               printf("\tCERCLE : X  = %ld ; Y  = %ld ; RADIUS = %ld ;\n",ptdraw->DATA.SCIRCLE->X,ptdraw->DATA.SCIRCLE->Y,ptdraw->DATA.SCIRCLE->RADIUS);
       }  // end for(ptdraw=pts->DRAW;ptdraw;ptdraw=ptdraw->NEXT)
     
     slib_return_box(pts);
     }    // end for(ptsym=ptlib->SYMB;ptsym;ptsym=ptsym->NEXT)
}         // end of slib_view_structslib

void slib_view_structmotif(library *ptlib)
{
   symbol_list *ptsym;
       
   for(ptsym=ptlib->SYMB;ptsym;ptsym=ptsym->NEXT)
     {
     symbol_list   *pts ;
     motif_symlist *ptm;
     motif_draw    *ptm_draw;
     
     pts = slib_get_flat_symbol(ptlib,ptsym->NAME);
     ptm = slib_motifconvert(pts);
             printf("symbol MOTIF : %s\n",ptm->NAME);
     for(ptm_draw=ptm->DRAW;ptm_draw;ptm_draw=ptm_draw->NEXT)
       {
       if(ptm_draw->TYPE == MOTIF_T_LINE )
                printf("\tLIGNE : X1 = %ld ; Y1 = %ld ; X2 = %ld ; Y2 = %ld ;\n",ptm_draw->DATA.MLINE->START_X,ptm_draw->DATA.MLINE->START_Y,ptm_draw->DATA.MLINE->END_X,ptm_draw->DATA.MLINE->END_Y);  
       if(ptm_draw->TYPE == MOTIF_T_ARC )
                printf("\tARC   : X  = %ld ; Y  = %ld ; H  = %ld ; W  = %ld ; A1 = %ld ; A2 = %ld ;\n",ptm_draw->DATA.MARC->CORNER_X,ptm_draw->DATA.MARC->CORNER_Y,ptm_draw->DATA.MARC->R *2,ptm_draw->DATA.MARC->R *2,ptm_draw->DATA.MARC->ALPHA,ptm_draw->DATA.MARC->DELTA);
       } // end for(ptm_draw=ptm->DRAW;ptm_draw;ptm_draw=ptm_draw->NEXT)
     }   // end for(ptsym=ptlib->SYMB;ptsym;ptsym=ptsym->NEXT)
}        // end of slib_view_structmotif

void slib_view_all(library *ptlib)
{
    printf("\n");
    printf("##################################################\n");
    printf("# POUR LA STRUCTURE A PLAT SLIB :                #\n");
    printf("##################################################\n");
    printf("\n");
    slib_view_structslib(ptlib);
    printf("\n");
    printf("##################################################\n");
    printf("# POUR LA STRUCTURE A PLAT MOTIF :               #\n");
    printf("##################################################\n");
    printf("\n");
    slib_view_structmotif(ptlib);
}    // end of slib_view_all

/******************************************************************************/
/* Test d'affichage des symbols par outils graphiques                         */
/******************************************************************************/

void slib_write_motif(FILE *file,motif_symlist *ptm_sym)
{
    motif_draw  *ptm_draw;

    for(ptm_draw=ptm_sym->DRAW;ptm_draw;ptm_draw=ptm_draw->NEXT)
      {
      if(ptm_draw->TYPE == MOTIF_T_LINE)
       {
        motif_line *ptm_data;
        ptm_data=ptm_draw->DATA.MLINE;
        fprintf(file,"\tXDrawLine(display, win, gc, %f, %f, %f, %f );\n",ptm_data->START_X * SCA + ORG,ptm_data->START_Y * SCA + ORG,ptm_data->END_X * SCA + ORG,ptm_data->END_Y * SCA + ORG); 
        }  // end of if(ptm_draw->TYPE == MOTIF_T_LINE)
      if(ptm_draw->TYPE == MOTIF_T_ARC)
       {
        motif_arc *ptm_data;
        ptm_data=ptm_draw->DATA.MARC;
        fprintf(file,"\tXDrawArc(display, win, gc, %f, %f, %f, %f, %ld, %ld );\n",ptm_data->CORNER_X * SCA + ORG,ptm_data->CORNER_Y * SCA + ORG,ptm_data->R * 2 * SCA,ptm_data->R * 2 * SCA,ptm_data->ALPHA,ptm_data->DELTA);
        }  // end of if(ptm_draw->TYPE == MOTIF_T_ARC)
    }      // end for(ptm_draw=ptm_sym->DRAW;ptm_draw;ptm_draw=ptm_draw->NEXT)
}          // end of slib_write_motif

void slib_display_motif(FILE *file, library *ptlib, symbol_list *ptsym)
{
    symbol_list   *ptflatten;
    motif_symlist *ptm_sym;

    ptflatten = slib_get_flat_symbol(ptlib, ptsym->NAME);
    ptm_sym   = slib_motifconvert(ptflatten);
      
    slib_write_motif(file,ptm_sym);
}        // end of slib_display_motif

void slib_make_motif_file(library *ptlib)
{
    symbol_list *ptsym;
    draw_list   *ptdraw;
    
    int top_level_symbol;
    FILE *file;
    file = mbkfopen("slib_view_symbol","c",WRITE_TEXT);
    //debut de redaction du fichier file pour la visualisation avec motif
    fprintf(file,"#include </usr/openwin/include/X11/StringDefs.h>\n");
    fprintf(file,"#include </usr/dt/include/Xm/Xm.h>\n");
    fprintf(file,"#include </usr/dt/include/Xm/PushB.h>\n");
    fprintf(file,"#include </usr/dt/include/Xm/RowColumn.h>\n");
    fprintf(file,"#include </usr/dt/include/Xm/DrawingA.h>\n");
    fprintf(file,"#include </usr/dt/include/Xm/CascadeB.h>\n");
    fprintf(file,"#include </usr/dt/include/Xm/MainW.h>\n");
    fprintf(file,"#include </usr/dt/include/Xm/Form.h>\n");
    fprintf(file,"#include <Xm/ScrolledW.h>\n\n");
    fprintf(file,"Display *display;\nScreen *screen_ptr;\nGC gc; \n\n");
    fprintf(file,"void quit_call()\n{\n\tprintf(\"Quitting program\");\n\texit(0);\n}\n\n");
    //debut de redaction des fonctions graphiques file pour la visualisation avec motif
        for(ptsym=ptlib->SYMB;ptsym;ptsym=ptsym->NEXT)
          {
             //si le symbol n'a pas de PIN on ne souhaite pas le creer
             top_level_symbol = FALSE ;             
             for(ptdraw=ptsym->DRAW;ptdraw;ptdraw=ptdraw->NEXT)
             { if(ptdraw->TYPE == SLIB_T_PIN){ top_level_symbol = TRUE; } }
            if(top_level_symbol == TRUE){
                    
            fprintf(file,"void %s_symbol(Widget w, XtPointer client_data, XtPointer event_data )\n",ptsym->NAME);
            fprintf(file,"{\n");
            fprintf(file,"\tWindow win = XtWindow(client_data);\n\n");      
            slib_display_motif(file,ptlib,ptsym); //la fonction est récursive
            fprintf(file,"}\n");
            }       // end  if(top_level_symbol == TRUE)
          }         // end for(ptsym=ptlib->SYMB;ptsym;ptsym=ptsym->NEXT)
    //debut de redaction du main pour la visualisation avec motif
    fprintf(file,"main(argc, argv)\n");
    fprintf(file,"int argc;\nchar *argv[];\n");
    fprintf(file,"{\n");
    fprintf(file,"\tWidget toplevel, select_w, draw_w, scroll_w, form_w, main_w, menu_bar, quit");
          for(ptsym=ptlib->SYMB;ptsym;ptsym=ptsym->NEXT)
          { fprintf(file,", %s_button",ptsym->NAME); } // end for(ptsym=ptlib->SYMB;ptsym;ptsym=ptsym->NEXT)
          fprintf(file,";\n");
    fprintf(file,"\tXtAppContext  app;\n");
    fprintf(file,"\tXmString label;\n");
    fprintf(file,"\tXGCValues gcv;\n");
    fprintf(file,"\n");
    fprintf(file,"\ttoplevel = XtVaAppInitialize(&app, \"SLIB TEST\", NULL, 0, &argc, argv, NULL, NULL);\n");
    fprintf(file,"\tmain_w = XtVaCreateManagedWidget(\"main_window\", xmMainWindowWidgetClass, toplevel, NULL);\n");
    fprintf(file,"\tmenu_bar = XmCreateMenuBar(main_w, \"main_list\", NULL, 0);\n");
    fprintf(file,"\tXtManageChild(menu_bar);\n");
    fprintf(file,"\tquit = XtVaCreateManagedWidget(\"Quit\", xmCascadeButtonWidgetClass, menu_bar, XmNmnemonic, 'Q', NULL);\n");
    fprintf(file,"\tXtAddCallback(quit, XmNactivateCallback, quit_call, NULL);\n");
    fprintf(file,"\tform_w = XtVaCreateManagedWidget(\"form\", xmFormWidgetClass, main_w, NULL);\n");
    fprintf(file,"\tscroll_w = XtVaCreateManagedWidget(\"box\", xmScrolledWindowWidgetClass, form_w, XmNscrollingPolicy, XmAUTOMATIC, XmNtopAttachment, XmATTACH_FORM, XmNbottomAttachment, XmATTACH_FORM, XmNleftAttachment, XmATTACH_FORM, NULL);\n");
    fprintf(file,"\tselect_w = XtVaCreateManagedWidget(\"box\", xmRowColumnWidgetClass, scroll_w, NULL);\n");
    fprintf(file,"\tdraw_w = XtVaCreateManagedWidget(\"draw\", xmDrawingAreaWidgetClass, form_w, XmNunitType , Xm1000TH_INCHES, XmNwidth, 4000, XmNheight, 4000, XmNtopAttachment, XmATTACH_FORM, XmNbottomAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM, XmNleftAttachment, XmATTACH_WIDGET, XmNleftWidget, select_w, NULL);\n");
    fprintf(file,"\tdisplay = XtDisplay(draw_w);\n\tscreen_ptr = XtScreen(draw_w);\n\tgcv.foreground = BlackPixelOfScreen(screen_ptr);\n\tgc = XCreateGC(display, RootWindowOfScreen(screen_ptr), GCForeground, &gcv);\n");    
    fprintf(file,"\t\n");    
       for(ptsym=ptlib->SYMB;ptsym;ptsym=ptsym->NEXT)
          {
             //si le symbol n'a pas de PIN on ne souhaite pas le creer
             top_level_symbol = FALSE ;             
             for(ptdraw=ptsym->DRAW;ptdraw;ptdraw=ptdraw->NEXT)
             { if(ptdraw->TYPE == SLIB_T_PIN){ top_level_symbol = TRUE; } }
            if(top_level_symbol == TRUE){
            
            fprintf(file,"\t\tlabel = XmStringCreateSimple(\"%s\");\n",ptsym->NAME);
            fprintf(file,"\t\t%s_button = XtVaCreateManagedWidget(\"pushme\", xmPushButtonWidgetClass, select_w, XmNlabelString, label, NULL);\n",ptsym->NAME);
            fprintf(file,"\t\tXmStringFree(label);\n");
            fprintf(file,"\t\tXtAddCallback(%s_button, XmNactivateCallback, %s_symbol, draw_w);\n",ptsym->NAME,ptsym->NAME);
            fprintf(file,"\t\n");
            }       // end  if(top_level_symbol == TRUE)
          }         // end for(ptsym=ptlib->SYMB;ptsym;ptsym=ptsym->NEXT)    
    fprintf(file,"\tXtRealizeWidget(toplevel);\n");
    fprintf(file,"\tXtAppMainLoop(app);\n");
    fprintf(file,"}\n");
    fclose(file);
}                   // end of  slib_drawing_test(library *ptlib)


/******************************************************************************/
/* fonction pour le parse d'un fichier SLIB                                   */
/******************************************************************************/

//slib_name ne comporte pas l'extension .slib
library     *slib_get_library()
{
    static int virgin = 1 ;
    int slibrestart();
    char *str ;
    char  buff[1024];
    slibdebug = 0;
    
    buff[0] = 0;
    
    str = V_STR_TAB[__SLIB_LIBRARY_NAME].VALUE;
    
    if(!str){
        str = getenv("AVT_TOOLS_DIR");
        if (str) 
             sprintf(buff,"%s/etc/%s",str,"avt.slib");
    }
    else{
        sprintf(buff,"%s",str);
    }
    
    slibin = fopen(buff,"r");
    
    slibParseLine = 1;    
    
    if(slibin)
        {
        if(virgin == 0)
     	  slibrestart(slibin) ;
        
        virgin = 0 ;
    
        slibEqtCtx = eqt_init(700);
        slibparse();
        eqt_term(slibEqtCtx);
        fclose(slibin);
        }
    else
        SLIB_LIBRARY = NULL ;

    return SLIB_LIBRARY;
}

library     *slib_load_library(char *buff)
{
    static int virgin = 1 ;
    int slibrestart();
    slibdebug = 0;
    
    slibin = fopen(buff,"r");
    
    slibParseLine = 1;    
    
    if(slibin)
        {
        if(virgin == 0)
     	  slibrestart(slibin) ;
        
        virgin = 0 ;
    
        slibEqtCtx = eqt_init(700);
        slibparse();
        eqt_term(slibEqtCtx);
        fclose(slibin);
        }
    else
        SLIB_LIBRARY = NULL ;

    return SLIB_LIBRARY;
}

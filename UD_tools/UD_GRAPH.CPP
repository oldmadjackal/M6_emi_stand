/*********************************************************************/
/*                                                                   */
/*               ������� ������������ ����������                     */
/*                                                                   */
/*********************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include <windows.h>

#include "UserDlg.h"

#pragma warning(disable : 4996)

/*---------------------------------- �������� ����������� ���������� */ 

  struct UDg_metrics {
                           HDC  hDC ;           /* �������� */
                          HRGN  hRGN ;          /* ������ ���� ��������� */
                           int  x_size ;        /* ������ ������� ���� */
                           int  y_size ;        
                        double  x_min ;         /* ��������� ���� */
                        double  x_max ;
                        double  y_min ;
                        double  y_max ;
                        double  dx   ;          /* ������������ ���� */
                        double  dy   ;
                           int  y_dir ;         /* ����������� ��� Y: */
                                                /*        0 - ����    */
                                                /*        1 - �����   */
                     } ;

  static  UDg_metrics  udg_metrics ;  

/*-------------------------------------------------------------------*/ 


/*********************************************************************/
/*                                                                   */
/*                    ������� ���� �����������                       */

  int  UDg_setviewport(HDC  hDC, int  x_base, int  y_base, 
                                 int  x_size, int  y_size )

{
  int  status ;
  
  
   if(hDC!=NULL) {
          status=SetViewportOrgEx(hDC, x_base, y_base, NULL) ; 
       if(status==0)  return(-1) ;

          status=SetViewportExtEx(hDC, x_size, y_size, NULL) ; 
       if(status==0)  return(-1) ;
                 }

         udg_metrics.x_size=x_size ;
         udg_metrics.y_size=y_size ;

      if(udg_metrics.hRGN)  DeleteObject(udg_metrics.hRGN) ;
         udg_metrics.hRGN =CreateRectRgn(x_base,        y_base,
                                         x_base+x_size, y_base+y_size) ;

                           SelectClipRgn(hDC, udg_metrics.hRGN) ;
    return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*              ��������������� ���� �����������                     */

  int  UDg_setwindow(HDC  hDC, int dir, double  x_min, double  y_min, 
                                        double  x_max, double  y_max )

{
        udg_metrics.x_min=x_min ;
        udg_metrics.x_max=x_max ;
        udg_metrics.y_min=y_min ;
        udg_metrics.y_max=y_max ;
        udg_metrics.y_dir=dir ;

        udg_metrics.dx=(udg_metrics.x_max-udg_metrics.x_min)
                      /udg_metrics.x_size ;
        udg_metrics.dy=(udg_metrics.y_max-udg_metrics.y_min)
                      /udg_metrics.y_size ;

    return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*              ������ �������� ��������� �� �������                 */

  int  UDg_xpos_w(HDC  hDC, double  x)

{
   int  pos ;

#pragma warning(disable : 4244)
     pos=(x-udg_metrics.x_min)/udg_metrics.dx+0.5-1 ;
#pragma warning(default : 4244)

  return(pos) ;
}


  int  UDg_ypos_w(HDC  hDC, double  y)

{
   int  pos ;

#pragma warning(disable : 4244)

                          pos=(y-udg_metrics.y_min)/udg_metrics.dy+0.5 ;
   if(udg_metrics.y_dir)  pos=udg_metrics.y_size-pos ;

#pragma warning(default : 4244)

  return(pos) ;
}


/*********************************************************************/
/*                                                                   */
/*              ������ ������� ��������� �� ��������                 */

  double  UDg_xcoord_w(HDC  hDC, int  x, double *x_min, double *x_max)

{
   double  w_min ;
   double  w_max ;

      w_min=((double)x+0.5)*udg_metrics.dx+udg_metrics.x_min ;
      w_max=w_min+udg_metrics.dx ;

   if(x_min!=NULL)  *x_min=w_min ;
   if(x_max!=NULL)  *x_max=w_max ;

  return((w_min+w_max)/2.) ;
}

  double  UDg_ycoord_w(HDC  hDC, int  y, double *y_min, double *y_max)

{
   double  w_min ;
   double  w_max ;


   if(udg_metrics.y_dir) {
                               y=udg_metrics.y_size-y ;
                           w_min=(y-0.5)*udg_metrics.dy+udg_metrics.y_min ;
                           w_max=w_min+udg_metrics.dy ;
                         } 
   else                  {
                           w_min=(y-0.5)*udg_metrics.dy+udg_metrics.y_min ;
                           w_max=w_min+udg_metrics.dy ;
                         } 

   if(y_min!=NULL)  *y_min=w_min ;
   if(y_max!=NULL)  *y_max=w_max ;

  return((w_min+w_max)/2.) ;
}


/*********************************************************************/
/*                                                                   */
/*                   ��������� �����                                 */

  int  UDg_setpixel_w(HDC  hDC, double  x, double  y, COLORREF  color)

{
     SetPixelV(hDC, UDg_xpos_w(hDC, x), UDg_ypos_w(hDC, y), color) ;

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                     ������� ���� � �����                          */

  int  UDg_moveto_w(HDC  hDC, double  x, double  y)

{
        MoveToEx(hDC, UDg_xpos_w(hDC, x),
                      UDg_ypos_w(hDC, y), NULL) ;
  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                   ��������� ����� � �����                         */

  int  UDg_lineto_w(HDC  hDC, double  x, double  y)

{
        LineTo(hDC, UDg_xpos_w(hDC, x), UDg_ypos_w(hDC, y)) ;

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                   ��������� ������� �����                         */

  int  UDg_marker_w(HDC  hDC, double  x, double  y, int  size)

{
  int  x_pxl ;
  int  y_pxl ;

             
           x_pxl=UDg_xpos_w(hDC, x) ;
           y_pxl=UDg_ypos_w(hDC, y) ;

     Rectangle(hDC, x_pxl-size, y_pxl-size, x_pxl+size, y_pxl+size) ;

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                   ��������� ��������                              */

  int  UDg_polygon_w(HDC  hDC, UD_point_w *points, int  points_cnt)

{
  static  POINT  poligon[200] ;
            int  i ; 

/*------------------------------------------------- ������� �������� */

        if(points_cnt<2)  return(0) ;

/*------------------------------------------------- ��������� ������ */

/*---------------------------------------------- �������� ���������� */

/*--------------------------------------------------- �������� ����� */

    for(i=0 ; i<points_cnt ; i++) {
                 poligon[i].x=UDg_xpos_w(hDC, points[i].x) ;
                 poligon[i].y=UDg_ypos_w(hDC, points[i].y) ;
                                  }
/*------------------------------------------------------ ����������� */

                    Polygon(hDC, poligon, points_cnt) ;

/*-------------------------------------------------------------------*/

  return(0) ;
}

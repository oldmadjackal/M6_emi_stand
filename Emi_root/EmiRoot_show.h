/*********************************************************************/
/*                                                                   */
/*               ������� ������������ ����������                     */
/*                                                                   */
/*********************************************************************/

#include "gl\gl.h"
#include "gl\glu.h"

/*--------------------------------------------- ��������� ���������� */

 typedef  struct {
                      char  name[256] ;
                      HWND  hWnd ;
                     HGLRC  hOpenGL ;               
                    double  Look_x ;
                    double  Look_y ;
                    double  Look_z ;
                    double  Look_azim ;
                    double  Look_elev ;
                    double  Look_roll ;
                    double  Range_strobe ;
                    double  Zoom ;
                      char  AtObject[256] ;
                      char  FromObject[256] ;

                 }  WndContext ;

/*-------------------------------------------------------- ��������� */

    int  EmiRoot_show_init       (void) ;            /* ������������� ������� ����������� */
    int  EmiRoot_show_getlist    (int) ;             /* �������������� ������ ����������� ������ */
   void  EmiRoot_show_releaselist(int) ;             /* ������������ ������ ����������� ������ */
    int  EmiRoot_first_context   (char *) ;          /* ���������� ������ �������� */
    int  EmiRoot_next_context    (char *) ;          /* ���������� ��������� �������� */
   void  EmiRoot_show_context    (char *) ;          /* ���������� ������� �������� */
 double  EmiRoot_get_context     (char *) ;          /* ������ �������� ��������� */
   void  EmiRoot_read_context    (std::string *) ;   /* ������� ��������� ��������� */
   void  EmiRoot_write_context   (std::string *) ;   /* �������� ��������� ��������� */
    int  EmiRoot_redraw          (char *) ;          /* ������������ ���� */
    int  EmiRoot_look            (char *, char *,    /* ������ � ������ ������ ������ */
                                  double *, double *, double *,
                                  double *, double *, double * ) ;
    int  EmiRoot_zoom            (char *, char *,    /* ������ � ����� ������ ������ */
                                        double * ) ;
    int  EmiRoot_lookat          (char *, char *,    /* ������ � ������ ���������� ������ */ 
                                          char * ) ;
    int  EmiRoot_lookfrom        (char *, char *,    /* ������ � ������ ��������� ������ */ 
                                          char * ) ;
    int  EmiRoot_lookat_point    (WndContext *) ;    /* ��������� ����� ���������� */

/*------------------------------------------------------ ����������� */

   LRESULT CALLBACK  EmiRoot_show_prc(HWND, UINT, WPARAM, LPARAM) ;

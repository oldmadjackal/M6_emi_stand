/*********************************************************************/
/*                                                                   */
/*               СИСТЕМА ГРАФИЧЕСКОГО ИНТЕРФЕЙСА                     */
/*                                                                   */
/*********************************************************************/

#include "gl\gl.h"
#include "gl\glu.h"

/*--------------------------------------------- Контексты Рендеринга */

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
                    double  Zoom ;
                      char  AtObject[256] ;
                    
                 }  WndContext ;

/*-------------------------------------------------------- Интерфейс */

    int  EmiRoot_show_init       (void) ;            /* Инициализация системы отображения */
    int  EmiRoot_show_getlist    (int) ;             /* Резервирование номера дисплейного списка */
   void  EmiRoot_show_releaselist(int) ;             /* Освобождение номера дисплейного списка */
    int  EmiRoot_first_context   (char *) ;          /* Установить первый контекст */
    int  EmiRoot_next_context    (char *) ;          /* Установить следующий контекст */
   void  EmiRoot_show_context    (char *) ;          /* Отобразить текущий контекст */
 double  EmiRoot_get_context     (char *) ;          /* Выдать параметр контекста */
   void  EmiRoot_read_context    (std::string *) ;   /* Считать параметры контекста */
   void  EmiRoot_write_context   (std::string *) ;   /* Записать параметры контекста */
    int  EmiRoot_redraw          (char *) ;          /* Перерисовать окно */
    int  EmiRoot_look            (char *, char *,    /* Работа с точкой зрения камеры */
                                  double *, double *, double *,
                                  double *, double *, double * ) ;
    int  EmiRoot_zoom            (char *, char *,    /* Работа с полем зрения камеры */
                                        double * ) ;
    int  EmiRoot_lookat          (char *, char *,    /* Работа с точкой наблюдения камеры */ 
                                          char * ) ;
    int  EmiRoot_lookat_point    (WndContext *) ;    /* Обработка точки наблюдения */

/*------------------------------------------------------ Обработчики */

   LRESULT CALLBACK  EmiRoot_show_prc(HWND, UINT, WPARAM, LPARAM) ;

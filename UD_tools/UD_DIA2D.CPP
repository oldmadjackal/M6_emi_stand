/*********************************************************************/
/*                                                                   */
/*               СИСТЕМА ГРАФИЧЕСКОГО ИНТЕРФЕЙСА                     */
/*                                                                   */
/*********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <search.h>
#include <windows.h>

#include "UserDlg.h"

#pragma warning(disable : 4996)


   void  UDi_diagram_2D_init  (HDC, UD_diag_data *, UD_metrics *) ;
   void  UDi_diagram_2D_scale (HDC, UD_diag_data *, UD_metrics *) ;
   void  UDi_diagram_2D_sort  (     UD_diag_data * ) ;
   void  UDi_diagram_2D_sizing(HDC, UD_diag_data *, UD_metrics *) ;
   void  UDi_diagram_2D_clear (HDC, UD_diag_data *, UD_metrics *) ;
   void  UDi_diagram_2D_grid  (HDC, UD_diag_data *, UD_metrics *) ;
   void  UDi_diagram_2D_graph (HDC, UD_diag_data *, UD_metrics *) ;

   void  UDi_2Dscan_change    (HDC, UD_diag_data *, UD_metrics *, int) ;

   void  UDi_scale_group   (int, int, int, double *, double *) ; 
   void  UDi_2Dscan_values (UD_diag_data *, UD_metrics *, int, int) ;


/*********************************************************************/
/*                                                                   */
/*                 Окно 2-мерных графиков                            */

 LRESULT CALLBACK  UD_diagram_2D_prc(  HWND  hWnd,     UINT  Msg,
 			             WPARAM  wParam, LPARAM  lParam)
{
             HDC  hDC ;
     PAINTSTRUCT  PaintCfg ;
            RECT  Rect ;
      UD_metrics *metrics ;
             int  zoom_flag ;           /* Флаг масштабного окна */
             int  zoom_num ;            /* Номер масштабного окна */
    UD_diag_data *data ;                /* Описание элемента */
            char  data_ptr[32] ;        /* Адрес описания, кодированный */     
             int  scanner_oper ;        /* Операция работы со сканером */ 
      static int  scanner_flag ;        /* Флаг работы со сканером */ 
      static int  x_scanner ;           /* Положение сканера */ 
      static int  y_scanner ; 
      static int  zone_flag ;           /* Флаг работы со спец-зоной */ 
      static int  x_zone_beg ;          /* Координаты спец-зоны */ 
      static int  y_zone_beg ;
      static int  x_zone_end ;
      static int  y_zone_end ;
      static int  smooth_zoom_flag ;    /* Флаг работы с плавным ZOOM-ом */ 
             int  x_mouse ;             /* Координаты мыши при движении */ 
             int  y_mouse ;
      static int  x_mouse_prv ;         /* Сохранение координат мыши при движении */ 
      static int  y_mouse_prv ;
          double  x_zoom_min ;          /* Координаты врезки по базовому окну */ 
          double  y_zoom_min ;
          double  x_zoom_max ;
          double  y_zoom_max ;
          double  x_zoom_size ;
          double  y_zoom_size ;
          double  delta ;
          double  base ;
          double  base_n ;
          double  zoom ;
          double  shift ;
             int  tmp ;
             int  i ;

/*------------------------------------------------------- Подготовка */

          data=NULL ; 

   if(Msg==WM_PAINT       ||
      Msg==WM_LBUTTONUP   ||
      Msg==WM_LBUTTONDOWN ||
      Msg==WM_RBUTTONUP   ||
      Msg==WM_RBUTTONDOWN ||
      Msg==WM_MOUSEMOVE     ) {

        SendMessage(hWnd, WM_GETTEXT, (WPARAM)sizeof(data_ptr),
                                      (LPARAM)       data_ptr  ) ;

                        data=(UD_diag_data *)UD_ptr_decode(data_ptr) ;

                              }

/*---------------------------------------------- Определение статуса */

                                              zoom_flag= 0 ;
                                              zoom_num =-1 ;  
     if(data!=NULL) {

         for(i=0 ; i<data->zooms_cnt ; i++)                         /* Проверяем, нет ли окна среди списка */
                if(hWnd==data->zooms[i].hWnd) {  zoom_num =i ;      /*   масштабных изображений            */
                                                 zoom_flag=1 ;   }

         if(zoom_flag)  metrics=&data->zooms[zoom_num].metrics ;
         else           metrics=&data->metrics ;

                        metrics->zoom_flag=zoom_flag ;
                        metrics->zoom_num =zoom_num ;
                    }
/*------------------------------------------ Обработка спец-операций */

   if(data!=NULL) {
/*- - - - - - - - - - - - - "Перемещение сканера"/"Удаление сканера" */
     if(data->spec_oper==_UD_SCANNER_MOVE ||
        data->spec_oper==_UD_SCANNER_CLEAR  ) {

           if(data->hWnd_scan==hWnd)  return(0) ; 

              InvalidateRect(hWnd, NULL, false) ;
              hDC=BeginPaint(hWnd, &PaintCfg) ;

           UDi_2Dscan_change(hDC, data, metrics, data->spec_oper) ;

                ValidateRect(hWnd, NULL) ;
                    EndPaint(hWnd, &PaintCfg) ;

                        return(0) ;
                                              }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                  }
/*--------------------------------------------------- Общая разводка */

  switch(Msg) {

/*----------------------------------------------- Основные сообщения */

    case WM_CREATE:   break ;

    case WM_COMMAND:  break ;

/*-------------------------------------------------------- Отрисовка */

    case WM_PAINT:   {
/*- - - - - - - - - - - - - - - - - - - - - - - -  Извлечение данных */
                        InvalidateRect(hWnd, NULL, false) ;

        if(data==NULL) {
                          ValidateRect(hWnd,  NULL) ;
                              break ;
                       }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - Вывод данных */
        if(data->data_list    ==NULL ||
           data->data_list_cnt==  0    ) {   
                                           ValidateRect(hWnd,  NULL) ;
                                                break ;
                                         }

              hDC=BeginPaint(hWnd, &PaintCfg) ;

               GetClientRect(hWnd, &Rect) ;
                     metrics->x_base=Rect.left ;
                     metrics->y_base=Rect.top ;
                     metrics->x_size=Rect.right -Rect.left ;
                     metrics->y_size=Rect.bottom-Rect.top ;
                        
                  SetBkColor(hDC, data->back_color) ;

       UDi_diagram_2D_init  (hDC, data, metrics) ;                  /* Начальная установка параметров */
       UDi_diagram_2D_scale (hDC, data, metrics) ;                  /* Формирование шкал данных */
       UDi_diagram_2D_sort  (     data ) ;                          /* Упорядочивание точек потоков данных */
       UDi_diagram_2D_sizing(hDC, data, metrics) ;                  /* Распределение зон оцифровки и поля отрисовки */
       UDi_diagram_2D_clear (hDC, data, metrics) ;                  /* Очистка перерисовываемых зон */
       UDi_diagram_2D_grid  (hDC, data, metrics) ;                  /* Отрисовка шкал и оцифровки */
       UDi_diagram_2D_graph (hDC, data, metrics) ;                  /* Отрисовка графиков */

               ValidateRect(hWnd,  NULL) ;
                   EndPaint(hWnd, &PaintCfg) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			     break ;
		     }
/*--------------------------------------------- Нажатие левой кнопки */

    case WM_LBUTTONDOWN: {
                            if(data==NULL)  break ; 
/*- - - - - - - - - - - - - - - - - - - -  Фиксируем стартовую точку */
     if((wParam & MK_SHIFT  )==MK_SHIFT  ||
        (wParam & MK_CONTROL)==MK_CONTROL  ) {
         
                             x_mouse_prv=LOWORD(lParam) ; 
                             y_mouse_prv=HIWORD(lParam) ;
                                
               if(zoom_flag)  smooth_zoom_flag=1 ;
                                             }
/*- - - - - - - - - - - - - - - - - - - Начало инструментальной зоны */
     else                                    {

                              x_zone_beg=LOWORD(lParam) ; 
                              y_zone_beg=HIWORD(lParam) ; 
                              x_zone_end= -1 ; 
                              y_zone_end= -1 ; 

                                zone_flag=1 ;

                          SetCapture(hWnd) ;                        /* Захватываем мышь */
                                             }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
     		                      break ;
                         }
/*--------------------------------- Нажатие/отпускание правой кнопки */

    case WM_RBUTTONUP:
    case WM_RBUTTONDOWN: {

                            if(data==NULL)  break ; 

                               data->hWnd_scan=hWnd ;
/*- - - - - - - - - - - - - - - - - - - - - Запуск/остановка сканера */
               x_scanner=LOWORD(lParam) ;                           /* Берем положение сканера */
               y_scanner=HIWORD(lParam) ;          

            if(x_scanner< metrics->draw_x_base ||                   /* Обработка выхода за границу */
               x_scanner>=metrics->draw_x_base+
                          metrics->draw_x_size ||
               y_scanner< metrics->draw_y_base ||
               y_scanner>=metrics->draw_y_base+
                          metrics->draw_y_size   )  break ;

            if(Msg==WM_RBUTTONUP) {
                                         ReleaseCapture() ;         /* Освобождаемся */
                                    scanner_flag= 0 ;
                                    scanner_oper=_UD_SCANNER_CLEAR ; 
                                  }
            else                  {
                                             SetCapture(hWnd) ;     /* Захватываем мышь */
                                    scanner_flag= 1 ;
                                    scanner_oper=_UD_SCANNER_MOVE ; 
                                  }
/*- - - - - - - - - - - - - - - - - - - - Отрисовка/удаление сканера */
                   InvalidateRect(hWnd, NULL, false) ;
                   hDC=BeginPaint(hWnd, &PaintCfg) ;

                 UDi_cross_invert(hDC, metrics, x_scanner, y_scanner) ;

                     ValidateRect(hWnd, NULL) ;
                         EndPaint(hWnd, &PaintCfg) ;
/*- - - - - - - - - - - - - - - - - - - - Отработка сканер-процедуры */
                UDi_2Dscan_values(data, metrics,                    /* Рассчет значений по сканеру */
                                      x_scanner, y_scanner) ; 

         if(data->ctrl_proc)
                  data->ctrl_proc(scanner_oper, zoom_num, data) ;   /*  Вызываем процедуру обработки */   
/*- - - - - - - - - - - - - - - - - - - Отработка отключения сканера */
            if(Msg==WM_RBUTTONUP) {
                                       x_scanner=-1 ;               /* Сбрасываем положение сканера */
                                       y_scanner=-1 ;
                                  }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
     		                      break ;
                         }
/*---------------------------------------------------- Движение мыши */

    case WM_MOUSEMOVE:   {

#define  ZOOM  data->zooms[zoom_num]

                             x_mouse=LOWORD(lParam) ; 
                             y_mouse=HIWORD(lParam) ; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - Сканер */
          if((wParam & MK_RBUTTON)==MK_RBUTTON) {

            if(data==NULL)  break ; 
               data->hWnd_scan=hWnd ;

            if(scanner_flag==0) {
                                    SetCapture(hWnd) ;
                                  x_scanner=-1 ; 
                                  y_scanner=-1 ;
                                }

            if(x_mouse< metrics->draw_x_base ||                   /* Обработка выхода за границу */
               x_mouse>=metrics->draw_x_base+
                        metrics->draw_x_size ||
               y_mouse< metrics->draw_y_base ||
               y_mouse>=metrics->draw_y_base+
                        metrics->draw_y_size   ) {  scanner_flag= 0 ;
                                                    scanner_oper=_UD_SCANNER_CLEAR ;  }
            else                                 {  scanner_flag= 1 ;
                                                    scanner_oper=_UD_SCANNER_MOVE ;  }

                              InvalidateRect(hWnd, NULL, false) ;
                              hDC=BeginPaint(hWnd, &PaintCfg) ;
              
        for(i=0 ; i<2 ; i++) {
                     UDi_cross_invert(hDC, metrics, x_scanner, y_scanner) ;

                               x_scanner=x_mouse ; 
                               y_scanner=y_mouse ;

                if(scanner_flag==0)  break ;
                             }

                                ValidateRect(hWnd, NULL) ;
                                    EndPaint(hWnd, &PaintCfg) ;

                if(scanner_flag==0) {
                                       ReleaseCapture() ;
                                          x_scanner=-1 ; 
                                          y_scanner=-1 ;
                                              break ;
                                    }

                  UDi_2Dscan_values(data, metrics,                  /* Рассчет значений по сканеру */
                                        x_scanner, y_scanner) ; 

         if(data->ctrl_proc)                                        /* Вызываем процедуру обработки */   
                    data->ctrl_proc(scanner_oper, zoom_num, data) ;

                                    break ;
                                                }
/*- - - - - - - - - - - - - - - - - - - - - -  Инструментальная зона */
     else if(zone_flag) {
                           InvalidateRect(hWnd, NULL, false) ;
                           hDC=BeginPaint(hWnd, &PaintCfg) ;
              
        for(i=0 ; i<2 ; i++) {
                                Rect.left  =x_zone_beg ;
                                Rect.right =x_zone_end ;
                                Rect.top   =y_zone_beg ;
                                Rect.bottom=y_zone_end ;

                     UDi_zone_invert(hDC, metrics, &Rect) ;

                               x_zone_end=LOWORD(lParam) ; 
                               y_zone_end=HIWORD(lParam) ; 
                             }

                        ValidateRect(hWnd, NULL) ;
                            EndPaint(hWnd, &PaintCfg) ;

           if(x_zone_beg>metrics->draw_x_base   &&                  /* Если при работе с инструментальной */
              x_zone_beg<metrics->draw_x_base+                      /*  зоной мы выходим за границы       */
                         metrics->draw_x_size-1 &&                  /*  окна графика - отрабатываем       */
              x_zone_end>metrics->draw_x_base   &&                  /*  отпускание левой кнопки           */ 
              x_zone_end<metrics->draw_x_base+
                         metrics->draw_x_size-1 &&
              y_zone_beg>metrics->draw_y_base   &&
              y_zone_beg<metrics->draw_y_base+
                         metrics->draw_y_size-1 &&
              y_zone_end>metrics->draw_y_base   &&
              y_zone_end<metrics->draw_y_base+
                          metrics->draw_y_size-1  )  break ;
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - ZOOM - сдвиг */
     else  if( smooth_zoom_flag             && 
              (wParam & MK_SHIFT)==MK_SHIFT   ) {

                             delta=ZOOM.x_max-ZOOM.x_min ;
                             shift=delta*(x_mouse-x_mouse_prv)/     /* Рассчет сдвига */
                                     metrics->draw_x_size    ;
                       ZOOM.x_min-=shift ;                          /* Пересчет границ по X */
                       ZOOM.x_max-=shift ;

                if(ZOOM.x_min<0.) {  ZOOM.x_min=  0. ;              /* Контроль границ по X */
                                     ZOOM.x_max=delta ;  }
                if(ZOOM.x_max>1.) {  ZOOM.x_min=  1.-delta ;
                                     ZOOM.x_max=  1. ;  }

                             delta=ZOOM.y_max-ZOOM.y_min ;
                             shift=delta*(y_mouse-y_mouse_prv)/     /* Рассчет сдвига */
                                     metrics->draw_y_size    ;
                       ZOOM.y_min+=shift ;                          /* Пересчет границ по X */
                       ZOOM.y_max+=shift ;

                if(ZOOM.y_min<0.) {  ZOOM.y_min=  0. ;              /* Контроль границ по Y */
                                     ZOOM.y_max=delta ;  }
                if(ZOOM.y_max>1.) {  ZOOM.y_min=  1.-delta ;
                                     ZOOM.y_max=  1. ;  }

             SendMessage(hWnd, WM_PAINT, (WPARAM)0, (LPARAM)0) ;

                        x_mouse_prv=x_mouse ;
                        y_mouse_prv=y_mouse ;

                                break ;
                                                }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - ZOOM - масштаб */
     else  if( smooth_zoom_flag                && 
              (wParam & MK_CONTROL)==MK_CONTROL   ) {

                            delta=x_mouse-x_mouse_prv ;
                             base=metrics->draw_x_size ;

         if(delta!=0) {
                 if(delta>0)  zoom=1+10.*delta /base ;              /* Рассчет изменения масштаба по X */
                 else         zoom=1/(1+10.*(-delta)/base) ;

                           base   = ZOOM.x_max-ZOOM.x_min ;         /* Рассчет нового диапазона по оси X */
                           base_n =base*zoom ;
            if(base_n>1.)  base_n =1. ;

                       ZOOM.x_min+=(base-base_n)/2. ;               /* Пересчет границ по X */
                       ZOOM.x_max-=(base-base_n)/2. ;
                      }

                if(ZOOM.x_min<0.) {  ZOOM.x_min=  0. ;              /* Контроль границ по X */
                                     ZOOM.x_max=base_n ;      }
                if(ZOOM.x_max>1.) {  ZOOM.x_min=  1.-base_n ;
                                     ZOOM.x_max=  1. ;        }

                            delta=y_mouse-y_mouse_prv ;
                             base=metrics->draw_y_size ;

         if(delta!=0) {
                 if(delta>0)  zoom=1+10.*delta /base ;              /* Рассчет изменения масштаба по Y */
                 else         zoom=1/(1+10.*(-delta)/base) ;

                           base   =ZOOM.y_max-ZOOM.y_min ;          /* Рассчет нового диапазона по оси Y */
                           base_n =base*zoom ;
            if(base_n>1.)  base_n =1. ;

                       ZOOM.y_min+=(base-base_n)/2. ;               /* Пересчет границ по Y */
                       ZOOM.y_max-=(base-base_n)/2. ;
                      }

                if(ZOOM.y_min<0.) {  ZOOM.y_min=  0. ;              /* Контроль границ по Y */
                                     ZOOM.y_max=base_n ;      }
                if(ZOOM.y_max>1.) {  ZOOM.y_min=  1.-base_n ;
                                     ZOOM.y_max=  1. ;        }

             SendMessage(hWnd, WM_PAINT, (WPARAM)0, (LPARAM)0) ;

                        x_mouse_prv=x_mouse ;
                        y_mouse_prv=y_mouse ;

                                break ;
                                                    }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#undef  ZOOM
                         }
/*------------------------------------------ Отпускание левой кнопки */

    case WM_LBUTTONUP:   {
                               smooth_zoom_flag=0 ;

                             if(!zone_flag)  break ;
                                 zone_flag=0 ;

                          ReleaseCapture() ;                        /* Освобождаем мышь */

                             if(x_zone_end<0)  break ;
/*- - - - - - - - - - - - - - - - - - Удаление инструментальной зоны */
                    InvalidateRect(hWnd, NULL, false) ;
                    hDC=BeginPaint(hWnd, &PaintCfg) ;

                               Rect.left  =x_zone_beg ;
                               Rect.right =x_zone_end ;
                               Rect.top   =y_zone_beg ;
                               Rect.bottom=y_zone_end ;

                   UDi_zone_invert(hDC, metrics, &Rect) ;

                      ValidateRect(hWnd, NULL) ;
                          EndPaint(hWnd, &PaintCfg) ;
/*- - - - - - - - - - - - - - - - - - - - Упорядочивание границ зоны */
         if(x_zone_beg > x_zone_end) {         tmp=x_zone_end ;
                                        x_zone_end=x_zone_beg ;
                                        x_zone_beg=  tmp ;       }
         if(y_zone_beg > y_zone_end) {         tmp=y_zone_end ;
                                        y_zone_end=y_zone_beg ;
                                        y_zone_beg=  tmp ;       }
/*- - - - - - - - - - - - - -  Координаты врезки в родительском окне */
         if(abs(x_zone_beg-x_zone_end)<4 ||                         /* Игнорируем зоны со слишком */    
            abs(y_zone_beg-y_zone_end)<4   )  break ;               /*    короткими стронами      */

                x_zoom_min=(x_zone_beg-metrics->draw_x_base)        /* Определение "координат" врезки */
                          / (double)metrics->draw_x_size ;
                x_zoom_max=(x_zone_end-metrics->draw_x_base)
                          / (double)metrics->draw_x_size ;
                y_zoom_min=(metrics->draw_y_base+
                            metrics->draw_y_size-y_zone_end)
                          / (double)metrics->draw_y_size ;
                y_zoom_max=(metrics->draw_y_size-y_zone_beg)
                          / (double)metrics->draw_y_size ;
/*- - - - - - - - - - - - - - - - - -  Нормализация координат врезки */
                 if(x_zoom_min<0.)  x_zoom_min=0. ; 
                 if(x_zoom_max>1.)  x_zoom_max=1. ; 
                 if(y_zoom_min<0.)  y_zoom_min=0. ; 
                 if(y_zoom_max>1.)  y_zoom_max=1. ; 
/*- - - - - - - - - - - - - - - - - - -  Подготовка нового ZOOM-окна */
     if(data           ==NULL ||                                    /* Если нет ZOOM-процедуры */
        data->ctrl_proc==NULL   )  break ;

#define  ZOOMS  data->zooms
#define  Z_CNT  data->zooms_cnt

          ZOOMS=(struct UD_zoom_scale *)                            /* Размещаем новое описание */    
                     realloc(ZOOMS, (Z_CNT+1)*sizeof(*ZOOMS)) ;
/*- - - - - - - - - - - - - - - - - Координаты врезки в базовом окне */
      if(zoom_flag) {                                               /* Если родительское окно - */
                              x_zoom_size=ZOOMS[zoom_num].x_max-    /*   ZOOM-окно              */
                                          ZOOMS[zoom_num].x_min ;
                              y_zoom_size=ZOOMS[zoom_num].y_max-
                                          ZOOMS[zoom_num].y_min ;
                       ZOOMS[Z_CNT].x_min=ZOOMS[zoom_num].x_min+
                                          x_zoom_size*x_zoom_min ;
                       ZOOMS[Z_CNT].x_max=ZOOMS[zoom_num].x_min+
                                          x_zoom_size*x_zoom_max ;
                       ZOOMS[Z_CNT].y_min=ZOOMS[zoom_num].y_min+
                                          y_zoom_size*y_zoom_min ;
                       ZOOMS[Z_CNT].y_max=ZOOMS[zoom_num].y_min+
                                          y_zoom_size*y_zoom_max ;
                    }
      else          {                                               /* Если родительское окно - */
                       ZOOMS[Z_CNT].x_min=x_zoom_min ;              /*   базовое окно           */
                       ZOOMS[Z_CNT].x_max=x_zoom_max ;
                       ZOOMS[Z_CNT].y_min=y_zoom_min ;
                       ZOOMS[Z_CNT].y_max=y_zoom_max ;
                    }
/*- - - - - - - - - - - - - - - - Вызов процедуры создания ZOOM-окна */
                             Z_CNT++ ;

                data->ctrl_proc(_UD_ZOOM, Z_CNT-1, data) ;          /*  Вызываем процедуру обработки */                             
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
     		                      break ;
                         }
/*------------------------------------------------- Прочие сообщения */

    default :        {
		return( DefWindowProc(hWnd, Msg, wParam, lParam) ) ;
			    break ;
		     }
/*-------------------------------------------------------------------*/
	      }
/*-------------------------------------------------------------------*/

    return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                  Начальная установка параметров                   */

   void  UDi_diagram_2D_init(HDC  hDC, UD_diag_data *view_decl, 
                                         UD_metrics *metrics)

{
      GetTextMetrics(hDC, &metrics->font) ;
}


/*********************************************************************/
/*                                                                   */
/*                  Формирование шкал данных                         */

   void  UDi_diagram_2D_scale(HDC  hDC, UD_diag_data *view_decl, 
                                          UD_metrics *metrics)

{
     UD_data_list *data_decl ;      /* Описание потока данных */
  UD_data_2Dpoint *data ;           /* Массив данных */
 	      int  data_list_cnt ;  /* Число массивов данных */
    UD_data_scale *spec_scale ;     /* Описание специальной разбивки шкалы */
    UD_zoom_scale *zoom ;           /* Параметры масштабирования */
           double  range ;          /* Диапазон шкалы */
 	   double  step ;           /* Шаг сетки*/
 	   double  pixel ;          /* Оценка шака пиксела */    
              int  i ;
              int  j ;

#define   X_MIN      data_decl->x_min
#define   X_MAX      data_decl->x_max
#define   Y_MIN      data_decl->y_min
#define   Y_MAX      data_decl->y_max
#define   X_0_GRID   data_decl->x_grid_0
#define   Y_0_GRID   data_decl->y_grid_0
#define   X_STEP     data_decl->x_grid_step
#define   Y_STEP     data_decl->y_grid_step

/*------------------------------------------- Определение диапазонов */

	       data_list_cnt=view_decl->data_list_cnt ;
		  spec_scale=view_decl->spec_scale ;
/*- - - - - - - - - - - - - - - - -  Определение диапазонов массивов */
	   UDi_scale_group(_UD_CLEAR, 0, 0, NULL, NULL) ;           /* Сброс групповых диапазонов */

   for(i=0 ; i<data_list_cnt ; i++) {                               /* CIRCLE.1 - По массивам данных */

	   data_decl=                   view_decl->data_list[i] ;   /* Извлекаем описание массива данных */
	   data     =(UD_data_2Dpoint *)data_decl->data ;

		if(!data_decl->use_flag)  continue ;

				      X_MIN=data[0].x ;             /* Иниц.диапазонов */
				      X_MAX=data[0].x ;
				      Y_MIN=data[0].y ;
				      Y_MAX=data[0].y ;

	  for(j=1 ; j<data_decl->data_cnt ; j++) {                  /* Опр.диапазоны */
		 if(data[j].x<X_MIN)  X_MIN=data[j].x ;
		 if(data[j].x>X_MAX)  X_MAX=data[j].x ;
		 if(data[j].y<Y_MIN)  Y_MIN=data[j].y ;
		 if(data[j].y>Y_MAX)  Y_MAX=data[j].y ;
						}

	   if(data_decl->x_scale_type==_UD_ZERO_SCALE) {            /* Обработка шкалы в             */
					   if(X_MIN>0.)  X_MIN=0. ; /*  обязательным включением нуля */
					   if(X_MAX<0.)  X_MAX=0. ;
						       }
	   if(data_decl->y_scale_type==_UD_ZERO_SCALE) {
					   if(Y_MIN>0.)  Y_MIN=0. ;
					   if(Y_MAX<0.)  Y_MAX=0. ;
						       }

	if(spec_scale!=NULL                    &&                   /* Обработка внешнего задания шкал */
	   spec_scale->scale_regime & _UD_X_SCALE) {
				   X_MIN=spec_scale->x_min ;
				   X_MAX=spec_scale->x_max ;
						   }
	if(spec_scale!=NULL                    &&
	   spec_scale->scale_regime & _UD_Y_SCALE) {
				   Y_MIN=spec_scale->y_min ;
				   Y_MAX=spec_scale->y_max ;
						   }

	   if(X_MIN==X_MAX)  X_MAX=X_MIN+1. ;                       /* Если пустой диалпазон */
	   if(Y_MIN==Y_MAX)  Y_MAX=Y_MIN+1. ;
/*- - - - - - - - - - - - - - - - - Обработка совмещенных диапазонов */
	   if(data_decl->x_scale_group)                             /* Работа с групповыми диапазонами */
	       UDi_scale_group(_UD_SET, 'X',
				data_decl->x_scale_group,
					      &X_MIN, &X_MAX) ;
	   if(data_decl->y_scale_group)
	       UDi_scale_group(_UD_SET, 'Y',
				data_decl->y_scale_group,
					      &Y_MIN, &Y_MAX) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
				    }                               /* CONTINUE.1 */

/*----------------------------------- Задание совмещенных диапазонов */

   for(i=0 ; i<data_list_cnt ; i++) {                               /* CIRCLE.2 - По массивам данных */

	       data_decl=view_decl->data_list[i] ;                  /* Извлекаем описание массива данных */
	   if(!data_decl->use_flag)  continue ;

	   if(data_decl->x_scale_group)                             /* Работа с групповыми диапазонами */
	       UDi_scale_group(_UD_GET, 'X',
				data_decl->x_scale_group,
					      &X_MIN, &X_MAX) ;
	   if(data_decl->y_scale_group)
	       UDi_scale_group(_UD_GET, 'Y',
				data_decl->y_scale_group,
					      &Y_MIN, &Y_MAX) ;
				    }                               /* CONTINUE.2 */
/*------------------------------------ Подстройка диапазонов под шаг */

   for(i=0 ; i<data_list_cnt ; i++) {                               /* CIRCLE.3 - По массивам данных */

	       data_decl=view_decl->data_list[i] ;                  /* Извлекаем описание массива данных */

	   if(!data_decl->use_flag)  continue ;

	      step  = UDi_range_step(X_MAX-X_MIN) ;
             pixel  =(X_MAX-X_MIN)/metrics->x_size ;
             X_MIN += pixel ;
	     X_MIN -= fmod(fabs(X_MIN), step) ;
             X_MAX -= pixel ;
	     X_MAX += step-fmod(fabs(X_MAX), step) ;
             X_STEP = step ; 
                
	      step  = UDi_range_step(Y_MAX-Y_MIN) ;
             pixel  =(Y_MAX-Y_MIN)/metrics->y_size ;
             Y_MIN += pixel ;
	     Y_MIN -= fmod(fabs(Y_MIN), step) ;
             Y_MAX -= pixel ;
	     Y_MAX += step-fmod(fabs(Y_MAX), step) ;
             Y_STEP = step ; 
				    }                               /* CONTINUE.3 */ 
/*--------------------------------------- Масштабирование диапазонов */

     if(metrics->zoom_flag) {

        for(i=0 ; i<data_list_cnt ; i++) {                          

	      data_decl=view_decl->data_list[i] ;                   /* Извлекаем описание массива данных */
          if(!data_decl->use_flag)  continue ;

                       zoom =&view_decl->zooms[metrics->zoom_num] ;

                         range=X_MAX-X_MIN ;
                      X_0_GRID=X_MIN ;
                      X_MIN  +=range*    zoom->x_min ;
                      X_MAX  -=range*(1.-zoom->x_max) ;
                      X_STEP  =UDi_range_step(X_MAX-X_MIN) ;

                         range=Y_MAX-Y_MIN ;
                      Y_0_GRID=Y_MIN ;
                      Y_MIN  +=range*    zoom->y_min ;
                      Y_MAX  -=range*(1.-zoom->y_max) ;
                      Y_STEP  =UDi_range_step(Y_MAX-Y_MIN) ;

                                         }                     
                            }
/*------------------------------ Фиксация параметров базового потока */

        for(i=0 ; i<data_list_cnt ; i++) {

	      data_decl=view_decl->data_list[i] ;                   /* Извлекаем описание массива данных */
          if(!data_decl->use_flag)  continue ;
          if(!data_decl->grid_flag)  continue ;

                              metrics->stream_base=   i ;
                              metrics->x_base_min =X_MIN ;
                              metrics->x_base_max =X_MAX ;
                              metrics->y_base_min =Y_MIN ;
                              metrics->y_base_max =Y_MAX ;

                                             break ;
                                         }
/*-------------------------------------------------------------------*/

#undef   X_MIN
#undef   X_MAX
#undef   Y_MIN
#undef   Y_MAX
#undef   X_STEP
#undef   Y_STEP
}


/*********************************************************************/
/*                                                                   */
/*                 Упорядочивание точек потоков данных               */

  int  UDi_2Dpoints_sort(const void *, const void *) ;

   void  UDi_diagram_2D_sort(UD_diag_data *view_decl) 
{
     UD_data_list *data_decl ;      /* Описание потока данных */
  UD_data_2Dpoint *data ;           /* Массив данных */
 	      int  data_list_cnt ;  /* Число массивов данных */
              int  i ;


	       data_list_cnt=view_decl->data_list_cnt ;

   for(i=0 ; i<data_list_cnt ; i++) {                               /* CIRCLE.1 - По массивам данных */

	       data_decl= view_decl->data_list[i] ;                 /* Извлекаем описание массива данных */
	       data     =(UD_data_2Dpoint *)data_decl->data ;

		if(!data_decl->use_flag)  continue ;

	  if(data_decl->type!=_UD_MONO_DATA)  continue ;            /* Упоряд.только монотонные данные */

              qsort(data, data_decl->data_cnt,                      /* Сортируем данные */
                           sizeof(*data), UDi_2Dpoints_sort) ;

				    }                               /* CONTINUE.1 */
}


  int  UDi_2Dpoints_sort(const void *elem1, const void *elem2)
{

   if( ((UD_data_2Dpoint *)elem1)->x >  
       ((UD_data_2Dpoint *)elem2)->x  )  return( 1) ;
   if( ((UD_data_2Dpoint *)elem1)->x <  
       ((UD_data_2Dpoint *)elem2)->x  )  return(-1) ;
                                         return( 0) ;
}


/*********************************************************************/
/*                                                                   */
/*          Распределение зон оцифровки и поля отрисовки             */

   void  UDi_diagram_2D_sizing(HDC  hDC, UD_diag_data *view_decl, 
                                           UD_metrics *metrics)
{
     UD_data_list *data_decl ;      /* Описание потока данных */
    UD_value_zone  zone ;           /* Описание поля оцифровки */
              int  size ;
              int  i ;

/*----------------------------- Определяем 'высоту' поля X-оцифровки */

            metrics->x_scale_h=metrics->font.tmHeight ;

/*------------------------------ Определение ширины поля Y-оцифровки */

			size=0 ;

   for(i=0 ; i<view_decl->data_list_cnt ; i++) {                    /* CIRCLE.1 - По массивам данных */

	     data_decl=view_decl->data_list[i] ;

	 if(!data_decl->use_flag)  continue ;

	    UDi_value_zone(data_decl->y_min,                        /* Опр.параметры зоны оцифровки  */
                           data_decl->y_max,                        /*   для текущего массива данных */
                           data_decl->y_grid_step, &zone) ;         

	 if(size<zone.len)  size=zone.len ;                         /* Выбираем самую широкую зону */

				               }                    /* CONTINUE.1 */

         metrics->y_scale_w=size*(metrics->font.tmAveCharWidth+     /* Пересчитываем в экранные единицы */
                                  metrics->font.tmMaxCharWidth )/2 ;    
/*----------------------------- Определение параметров поля графиков */
        
	metrics->draw_x_base=metrics->y_scale_w ;
	metrics->draw_y_base= 0 ;

	metrics->draw_x_size=metrics->x_size-metrics->y_scale_w ;
	metrics->draw_y_size=metrics->y_size-metrics->x_scale_h ;

/*-------------------------------------------------------------------*/
}


/*********************************************************************/
/*                                                                   */
/*                  Очистка перерисовываемых зон                     */

   void  UDi_diagram_2D_clear(HDC  hDC,  UD_diag_data *view_decl, 
                                           UD_metrics *metrics)

{
    RECT  Rect ;
  HBRUSH  Brush ;

/*------------------------------------------------------- Подготовка */

        Brush=CreateSolidBrush(view_decl->back_color) ;

/*------------------------------------------ Полная очистка элемента */


                    Rect.left  =metrics->x_base ;
                    Rect.top   =metrics->y_base ;
                    Rect.right =metrics->x_base+metrics->x_size ;
                    Rect.bottom=metrics->y_base+metrics->y_size ;

	  FillRect(hDC, &Rect, Brush) ;

/*------------------------------------------------------- Завершение */

        DeleteObject(Brush) ;

/*-------------------------------------------------------------------*/

}


/*********************************************************************/
/*                                                                   */
/*                  Отрисовка шкал и оцифровка                       */

   void  UDi_diagram_2D_grid(HDC  hDC, UD_diag_data *view_decl, 
                                         UD_metrics *metrics)
{
     UD_data_list *data_decl ;               /* Описание потока данных */
             HPEN  hPen ;                    /* Перо */
             HPEN  hPen_prev ;               /* Перо, предыдущее */
           double  value ;
           double  step ;
    UD_value_zone  zone ;
              int  i ;

#define                    _VALUES_MAX   20
    UD_value_zone  x_value[_VALUES_MAX] ;    /* Описание оцифровки X-шкал */
              int  x_value_cnt ;
    UD_value_zone  y_value[_VALUES_MAX] ;    /* Описание оцифровки Y-шкал */
              int  y_value_cnt ;

/*--------------------------------- Подготовка графических установок */

     hPen     =         CreatePen(PS_DASH, 0, view_decl->fore_color) ;
     hPen_prev=(HPEN)SelectObject(hDC, hPen) ;

/*--------------------------------------- Отрисовка сетки под график */

        UDg_setviewport(hDC, metrics->draw_x_base,                  /* Уст.рабочее окно */
                             metrics->draw_y_base, 
                             metrics->draw_x_size,
                             metrics->draw_y_size ) ; 

		    x_value_cnt=0 ;
		    y_value_cnt=0 ;

   for(i=0 ; i<view_decl->data_list_cnt ; i++) {                    /* CIRCLE.1 - По массивам данных */

	       data_decl=view_decl->data_list[i] ;                  /* Уст.поток данных */
	   if(!data_decl-> use_flag)  continue ;                    
	   if(!data_decl->grid_flag)  continue ;                    

	 UDg_setwindow(hDC, 1, data_decl->x_min,                    /* Масштабируем рабочее окно */
                               data_decl->y_min,       
                               data_decl->x_max, 
                               data_decl->y_max ) ;              
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Отработка по X */
	       step=data_decl->x_grid_step ;                        /* Определяем шаг по оси X */

       for(value=data_decl->x_grid_0 ;
           value<data_decl->x_max-step/4. ; value+=step) {          /* CIRCLE - Идем по узлам сетки в плюс */

          if(value<data_decl->x_min)  continue ;

		UDg_moveto_w(hDC, value, data_decl->y_min) ;        /* Рисуем линию сетки */
		UDg_lineto_w(hDC, value, data_decl->y_max) ;

	   if(x_value_cnt<_VALUES_MAX) {
		 x_value[x_value_cnt].value=                value ;
		 x_value[x_value_cnt].pos  =UDg_xpos_w(hDC, value) ;
		         x_value_cnt++ ;
				       }
							 }          /* CONTINUE */

       for(value=data_decl->x_grid_0 ;
           value>data_decl->x_min-step/4. ; value-=step) {          /* CIRCLE - Идем по узлам сетки в минус */

          if(value>data_decl->x_max)  continue ;

		UDg_moveto_w(hDC, value, data_decl->y_min) ;        /* Рисуем линию сетки */
		UDg_lineto_w(hDC, value, data_decl->y_max) ;

	   if(x_value_cnt<_VALUES_MAX) {
		 x_value[x_value_cnt].value=                value ;
		 x_value[x_value_cnt].pos  =UDg_xpos_w(hDC, value) ;
		         x_value_cnt++ ;
				       }
							 }          /* CONTINUE */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Отработка по Y */
	       step=data_decl->y_grid_step ;                        /* Определяем шаг по оси Y */

       for(value=data_decl->y_grid_0 ;
           value<data_decl->y_max-step/4. ; value+=step) {          /* CIRCLE - Идем по узлам сетки в плюс */

          if(value<data_decl->y_min)  continue ;

		UDg_moveto_w(hDC, data_decl->x_min, value) ;        /* Рисуем линию сетки */
		UDg_lineto_w(hDC, data_decl->x_max, value) ;

	   if(y_value_cnt<_VALUES_MAX) {
		 y_value[y_value_cnt].value=                value ;
		 y_value[y_value_cnt].pos  =UDg_ypos_w(hDC, value) ;
		         y_value_cnt++ ;
				       }
							 }          /* CONTINUE */

       for(value=data_decl->y_grid_0 ;
           value>data_decl->y_min-step/4. ; value-=step) {          /* CIRCLE - Идем по узлам сетки минус */

          if(value>data_decl->y_max)  continue ;

		UDg_moveto_w(hDC, data_decl->x_min, value) ;        /* Рисуем линию сетки */
		UDg_lineto_w(hDC, data_decl->x_max, value) ;

	   if(y_value_cnt<_VALUES_MAX) {
		 y_value[y_value_cnt].value=                value ;
		 y_value[y_value_cnt].pos  =UDg_ypos_w(hDC, value) ;
		         y_value_cnt++ ;
				       }
							 }          /* CONTINUE */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
				       break ;
				               }

    UDg_setviewport(hDC, 0, 0, metrics->x_size, metrics->y_size) ;  /* Восстановление экранной зоны */

/*--------------------------------------------------- Оцифровка шкал */

                 SetTextColor(hDC, view_decl->grid_color) ;
                   SetBkColor(hDC, view_decl->back_color) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Отработка по X */
               UDi_value_zone(data_decl->x_min,
                              data_decl->x_max, 
                              data_decl->x_grid_step, &zone) ;

   for(i=0 ; i<x_value_cnt ; i+=2) {
	  UDi_value_cvt(x_value[i].value, &zone, 0) ;
	        TextOut(hDC, metrics->draw_x_base+x_value[i].pos,
                             metrics->draw_y_base+metrics->draw_y_size,
                                      zone.buff, strlen(zone.buff)) ;
				  }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Отработка по Y */
               UDi_value_zone(data_decl->y_min,
                              data_decl->y_max,
                              data_decl->y_grid_step, &zone) ;

   for(i=0 ; i<y_value_cnt ; i++) {
	  UDi_value_cvt(y_value[i].value, &zone, 0) ;
	        TextOut(hDC,   0,
                             metrics->draw_y_base-
                              metrics->font.tmHeight/2+
                                y_value[i].pos,
                                      zone.buff, strlen(zone.buff)) ;
				  }
/*--------------------------------- Отрисовка рамки области графиков */

              DeleteObject(hPen) ;
            hPen=CreatePen(PS_SOLID, 0, view_decl->fore_color) ;
             SelectObject(hDC, hPen) ;

       MoveToEx(hDC, metrics->draw_x_base, 
                     metrics->draw_y_base, NULL) ;
         LineTo(hDC, metrics->draw_x_base+metrics->draw_x_size-1, 
                     metrics->draw_y_base ) ;
         LineTo(hDC, metrics->draw_x_base+metrics->draw_x_size-1, 
                     metrics->draw_y_base+metrics->draw_y_size) ;
         LineTo(hDC, metrics->draw_x_base, 
                     metrics->draw_y_base+metrics->draw_y_size) ;
         LineTo(hDC, metrics->draw_x_base, 
                     metrics->draw_y_base ) ;

/*----------------------------- Восстановление графических установок */

             SelectObject(hDC, hPen_prev) ;
             DeleteObject(hPen) ;

/*-------------------------------------------------------------------*/

}


/*********************************************************************/
/*                                                                   */
/*                          Отрисовка графиков                       */


   void  UDi_diagram_2D_graph(HDC  hDC, UD_diag_data *view_decl, 
                                          UD_metrics *metrics)

{
     UD_data_list *data_decl ;      /* Описание потока данных */
  UD_data_2Dpoint *data ;           /* Массив данных */
             HPEN  hPen ;           /* Перо */
             HPEN  hPen_prev ;      /* Перо, предыдущее */
              int  i ;
              int  j ;

/*-------------------------------------------------- Подготовка окна */

        UDg_setviewport(hDC, metrics->draw_x_base,                  /* Уст.рабочее окно */
                             metrics->draw_y_base, 
                             metrics->draw_x_size,
                             metrics->draw_y_size ) ; 
            
/*----------------------------------------------- Отрисовка графиков */

   for(i=0 ; i<view_decl->data_list_cnt ; i++) {                    /* CIRCLE.1 - По массивам данных */

	   data_decl=                   view_decl->data_list[i] ;   /* Уст.поток данных */
	   data     =(UD_data_2Dpoint *)data_decl->data ;

       if(!data_decl->use_flag)  continue ;
/*- - - - - - - - - - - - - - - - - - - - - - - - Подготовка графика */
	 UDg_setwindow(hDC, 1, data_decl->x_min,                    /* Масштабируем рабочее окно */
                               data_decl->y_min,       
                               data_decl->x_max, 
                               data_decl->y_max ) ;              

     hPen     =         CreatePen(data_decl->line_style, 0,         /* Подготовка пера */ 
                                  data_decl->color         ) ;      
     hPen_prev=(HPEN)SelectObject(hDC, hPen) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Отрисовка точек */
     if(data_decl->type==_UD_POINT_DATA) {

	 for(j=0 ; j<data_decl->data_cnt ; j++)
              UDg_marker_w(hDC, data[j].x, data[j].y, 2) ;
                                         }
/*- - - - - - - - - - - - - - - - - - - - - - - -  Отрисовка графика */
     else                                {

		       UDg_moveto_w(hDC, data[0].x, data[0].y) ;

	 for(j=1 ; j<data_decl->data_cnt ; j++)
		       UDg_lineto_w(hDC, data[j].x, data[j].y) ;
                                         }
/*- - - - - - - - - - - - - - - - - - - - -  Откат установок графики */
             SelectObject(hDC, hPen_prev) ;
             DeleteObject(hPen) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
				               }                    /* CONTINUE.1 */
/*----------------------------- Восстановление графических установок */

    UDg_setviewport(hDC, 0, 0, metrics->x_size, metrics->y_size) ;  /* Восстановление экранной зоны */

/*-------------------------------------------------------------------*/
}


/*********************************************************************/
/*                                                                   */
/*                    Работа с групповыми шкалами                    */
/*                                                                   */
/*  Параметры:                                                       */
/*	  oper_code     Операция:                                    */
/*			   _UD_CLEAR - очистить таблицу шкал         */
/*			   _UD_SET   - установить шкалу по диапазону */
/*	        	   _UD_GET   - выдать диапазон шкалы         */
/*	     y_flag     Литера оси: 'X', 'Y', ...                    */
/*	      group     Идентификатор группы                         */
/*        value_min     Диапазон                                     */
/*        value_max                                                  */

  struct UD_scale_group {                   /* Описание групповой шкалы */
			     int  xy_flag ;  /* Литера оси: 'X', 'Y', ... */
			     int  group ;    /* Идентификатор группы */
			  double  min ;      /* Диапазон */
			  double  max ;
			} ;

#define  _SGROUP_MAX  10
 static  UD_scale_group  ud_sgroup[_SGROUP_MAX] ;  /* Таблица групповых шкал */
 static             int  ud_sgroup_cnt ;           /* Число групповых шкал */


    void  UDi_scale_group(   int  oper_code,
	                     int  xy_flag,
	                     int  group,
                          double *value_min,
                          double *value_max ) 
{
   int  i ;

/*-------------------------------------------------- Очистка таблицы */

   if(oper_code==_UD_CLEAR) {
				ud_sgroup_cnt=0 ;
				      return ;
			    }
/*---------------------------------------- Установка/коррекция шкалы */

   if(oper_code==_UD_SET) {

	for(i=0 ; i<ud_sgroup_cnt ; i++)                            /* Ищем группу по списку */
	  if(ud_sgroup[i].xy_flag==xy_flag &&
	     ud_sgroup[i].group  ==group     )  break ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - -  Новая шкала */
      if(i==ud_sgroup_cnt) {
			        if(i==_SGROUP_MAX)  return ;        /* Контроль переполнения таблицы */

			      ud_sgroup[i].min    =*value_min ;    /* Установка шкалы */
			      ud_sgroup[i].max    =*value_max ;
			      ud_sgroup[i].xy_flag=    xy_flag ;
			      ud_sgroup[i].group  =     group ;

				      ud_sgroup_cnt++ ;
			   }
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Коррекция шкалы */
      else                 {

	  if(*value_min<ud_sgroup[i].min)  ud_sgroup[i].min=*value_min ;
	  if(*value_max>ud_sgroup[i].max)  ud_sgroup[i].max=*value_max ;
			   }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

				  return ;
			  }
/*------------------------------------------- Выдача диапазона шкалы */

   if(oper_code==_UD_GET) {

      for(i=0 ; i<ud_sgroup_cnt ; i++)
	if(ud_sgroup[i].xy_flag==xy_flag &&
	   ud_sgroup[i].group  ==group     ) {
				   *value_min=ud_sgroup[i].min ;
				   *value_max=ud_sgroup[i].max ;
					     }

			       return ;
			   }
/*-------------------------------------------------------------------*/
}


/*********************************************************************/
/*                                                                   */
/*                  Рассчет значений по сканеру                      */

   void  UDi_2Dscan_values(UD_diag_data *view_decl,
                             UD_metrics *metrics, int  x, int  y)

{
     UD_data_list *data_decl ;      /* Описание потока данных */
     UD_data_list *base_decl ;      /* Описание ведущего потока данных */
  UD_data_2Dpoint *data ;           /* Массив данных */
           double  x_scn_min ;
           double  x_scn_max ;
           double  x_scanner ;
           double  y_scanner ;
           double  y_scn_value ;
              int  y_scn_null ;
              int  k ;
              int  i ;
              int  j ;

/*-------------------------------------------------- Подготовка окна */

        UDg_setviewport(NULL, metrics->draw_x_base,                 /* Уст.рабочее окно */
                              metrics->draw_y_base, 
                              metrics->draw_x_size,
                              metrics->draw_y_size ) ; 

	  UDg_setwindow(NULL, 1, metrics->x_base_min,               /* Масштабируем рабочее окно */
                                 metrics->y_base_min,       
                                 metrics->x_base_max, 
                                 metrics->y_base_max ) ;              
            
/*---------------------------------------- Отработка ведущего потока */

        base_decl=view_decl->data_list[metrics->stream_base ] ;     /* Определяем ведущий поток */
/*- - - - - - - - - - - - - - - - -  Определяем растровые координаты */
        x_scanner=UDg_xcoord_w(NULL, x-metrics->draw_x_base,     
                                         &x_scn_min, &x_scn_max) ;
        y_scanner=UDg_ycoord_w(NULL, y-metrics->draw_y_base,     
                                                       NULL, NULL) ;
/*- - - - - - - - - - - - - - - - - - - - Определяем "ведущую" точку */
	         data=(UD_data_2Dpoint *)base_decl->data ;
           y_scn_null=  1 ;
                    k=  0 ;

         for(j=0 ; j<base_decl->data_cnt ; j++) {

           if(data[j].x>=x_scn_min &&
              data[j].x< x_scn_max   )
            if(data[j].y>y_scn_value || 
                  y_scn_null==1        ) {
                                           x_scanner  =data[j].x ;
                                           y_scn_value=data[j].y ;
                                           y_scn_null = 0 ;
                                         } 

           if(y_scn_null) {  
            if(data[j].x<x_scn_min &&
               data[j].x>data[k].x   )  k=j ;
                          }
                                                }                                                 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Заносим данные */
         if(y_scn_null             &&  
            data[k  ].x< x_scn_min &&
            data[k+1].x>=x_scn_max   ) {
                      y_scn_value=data[k].y-(x_scanner-data[k+1].x)*
                                            (data[k].y-data[k+1].y)/
                                            (data[k].x-data[k+1].x) ;
                      y_scn_null = 0 ;
                                       }

           base_decl->x_scanner  =x_scanner ;
           base_decl->y_scanner  =y_scanner ;
           base_decl->y_scn_null =y_scn_null ;
           base_decl->y_scn_value=y_scn_value ;

/*------------------------------------------------ Отработка потоков */

   for(i=0 ; i<view_decl->data_list_cnt ; i++) {                    /* CIRCLE.1 - По массивам данных */

	   data_decl=                   view_decl->data_list[i] ;   /* Уст.поток данных */
	   data     =(UD_data_2Dpoint *)data_decl->data ;

        if(data_decl==base_decl)  continue ;                        /* Отсекаем ведущий поток */ 

             data_decl->y_scn_null=1 ;                              /* По умолчанию - потоки не определены */

       if(!data_decl->use_flag)  continue ;
       if(data_decl->x_scale_group!=                                /* Только для потоков той же */
          base_decl->x_scale_group  )  continue ;                   /*  X-группы, что и ведущий  */
/*- - - - - - - - - - - - - - - - - - - - - Определение координаты Y */
           if(data[0].x==x_scanner)  j=1 ;
      else if(data[0].x< x_scanner) {
              for(j=1 ; j<data_decl->data_cnt ; j++)  
                if(data[j].x>=x_scanner)  break ;
                                    }
      else                          {
              for(j=1 ; j<data_decl->data_cnt ; j++)  
                if(data[j].x<=x_scanner)  break ;
                                    }

           if(j<data_decl->data_cnt) {
                    y_scn_null = 0 ;
                    y_scn_value=data[j].y+(x_scanner-data[j].x)*
                                          (data[j-1].y-data[j].y)/
                                          (data[j-1].x-data[j].x) ;
                                     } 
           else                      {
                    y_scn_null = 1 ;
                    y_scn_value= 0 ;
                                     }
/*- - - - - - - - - - - - - - - - - - - - - - - - - Занесение данных */
             data_decl->x_scanner  =x_scanner ;
             data_decl->y_scn_value=y_scn_value ;
             data_decl->y_scn_null =y_scn_null ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                               }                    /* CONTINUE.1 */

/*-------------------------------------------- Восстановление экрана */

    UDg_setviewport(NULL, 0, 0, metrics->x_size, metrics->y_size) ; /* Восстановление экранной зоны */

/*-------------------------------------------------------------------*/

}


/*********************************************************************/
/*                                                                   */
/*       Отработка специальной операции "Перемещение сканера"        */

   void  UDi_2Dscan_change(HDC hDC, UD_diag_data *view_decl, 
                                      UD_metrics *metrics, int  oper)
{
     UD_data_list *base_decl ;      /* Описание ведущего потока данных */
              int  x ;
              int  y ;
              int  x_prv ;
              int  y_prv ;

/*-------------------------------------------------- Подготовка окна */

        UDg_setviewport(hDC, metrics->draw_x_base,                  /* Уст.рабочее окно */
                             metrics->draw_y_base, 
                             metrics->draw_x_size,
                             metrics->draw_y_size ) ; 

	  UDg_setwindow(hDC, 1, metrics->x_base_min,                /* Масштабируем рабочее окно */
                                metrics->y_base_min,       
                                metrics->x_base_max, 
                                metrics->y_base_max ) ;              

        base_decl=view_decl->data_list[metrics->stream_base ] ;     /* Определяем ведущий поток */

/*----------------------------------------------- Пересчет координат */

             x=UDg_xpos_w(hDC, base_decl->x_scanner)+metrics->draw_x_base ;
             y=UDg_ypos_w(hDC, base_decl->y_scanner)+metrics->draw_y_base ;

    if(base_decl->scanner_prv_flag) {
         x_prv=UDg_xpos_w(hDC, base_decl->x_scanner_prv)+metrics->draw_x_base ;
         y_prv=UDg_ypos_w(hDC, base_decl->y_scanner_prv)+metrics->draw_y_base ;
                                    }
/*-------------------------------------------- Восстановление экрана */

    UDg_setviewport(hDC, 0, 0, metrics->x_size, metrics->y_size) ;  /* Восстановление экранной зоны */

/*---------------------------------------------- Перерисовка сканера */

    if(base_decl->scanner_prv_flag) 
                      UDi_cross_invert(hDC, metrics, x_prv, y_prv) ;

    if(view_decl->spec_oper!=_UD_SCANNER_CLEAR)
                      UDi_cross_invert(hDC, metrics, x, y) ;

/*------------------------------------- Сохранение состояния сканера */

         base_decl->x_scanner_prv=base_decl->x_scanner ;
         base_decl->y_scanner_prv=base_decl->y_scanner ;

    if(view_decl->spec_oper==_UD_SCANNER_CLEAR)
                    base_decl->scanner_prv_flag=0 ; 
    else            base_decl->scanner_prv_flag=1 ; 

/*-------------------------------------------------------------------*/

}



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

   void  UDi_area_2D_init      (HDC, UD_area_data *, UD_metrics *) ;
   void  UDi_area_2D_scale     (HDC, UD_area_data *, UD_metrics *) ;
   void  UDi_area_2D_sizing    (HDC, UD_area_data *, UD_metrics *) ;
   void  UDi_area_2D_clear     (HDC, UD_area_data *, UD_metrics *) ;
   void  UDi_area_2D_frame     (HDC, UD_area_data *, UD_metrics *) ;
   void  UDi_area_2D_rare      (HDC, UD_area_data *, UD_metrics *) ;
   void  UDi_area_2D_ceil      (HDC, UD_area_data *, UD_metrics *) ;
   void  UDi_area_2D_scalar    (HDC, UD_area_data *, UD_metrics *) ;
   void  UDi_area_2D_isoline   (HDC, UD_area_data *, UD_metrics *) ;
   void  UDi_area_2D_vector    (HDC, UD_area_data *, UD_metrics *) ;
   void  UDi_area_2D_profile   (HDC, UD_area_data *, UD_metrics *) ;
   void  UDi_area_2D_obstacle  (HDC, UD_area_data *, UD_metrics *) ;
   void  UDi_area_2D_grid      (HDC, UD_area_data *, UD_metrics *) ;

   void  UDi_scanner_vectors   (HDC, UD_metrics *, UD_area_data *) ;

   void  UDi_area_2D_arrow     (HDC, double, double, double, double) ;
    int  UDi_area_2D_links     (UD_area_2Dpoint *, int, int, int *) ;
    int  UDi_area_2D_spot      (UD_area_2Dpoint *, int, int, int *) ;
   void  UDi_area_2D_p_area    (UD_data_2Dpoint *, int *, int, int, UD_point_w *) ;
    int  UDi_area_2D_graphloops(int, UD_area_2Dpoint *, int *) ;

   void  UDi_a2Dscan_position  (UD_metrics *, int, int *, int *, double *, double *) ;
   void  UDi_area_2D_prof_pnts (UD_area_data *, UD_profile *) ;


/*********************************************************************/
/*                                                                   */
/*                 Окно 2-мерных полей                               */

 LRESULT CALLBACK  UD_area_2D_prc(  HWND  hWnd,     UINT  Msg,
 			          WPARAM  wParam, LPARAM  lParam)
{
             HDC  hDC_wnd ;
             HDC  hDC_tmp ;
             HDC *hDC ;
         HBITMAP *hBM ;
     PAINTSTRUCT  PaintCfg ;
            RECT  Rect ;
      UD_metrics *metrics ;
             int  zoom_flag ;           /* Флаг масштабного окна */
             int  zoom_num ;            /* Номер масштабного окна */
    UD_area_data *data ;                /* Описание элемента */
            char  data_ptr[32] ;        /* Адрес описания, кодированный */     
             int  scanner_oper ;        /* Операция работы со сканером */ 
      static int  scanner_flag ;        /* Флаг работы со сканером */ 
      static int  x_scanner ;           /* Положение сканера */ 
      static int  y_scanner ; 
      static int  zone_flag ;           /* Флаг работы со спец-зоной */ 
      static int  profile_flag ;        /* Флаг работы с сечением */ 
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

           data=(UD_area_data *)UD_ptr_decode(data_ptr) ;

                              }
/*--------------- Обработка отсоединения родительского потока данных */

     if(data             !=NULL && 
        data->parent_data==NULL   )  data=NULL ;

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

                             InvalidateRect(hWnd, NULL, false) ;
                         hDC_wnd=BeginPaint(hWnd, &PaintCfg) ;

        if(zoom_flag) {  hDC=&data->zooms[zoom_num].hDC ;
                         hBM=&data->zooms[zoom_num].hBitMap ;  }
        else          {  hDC=&data->hDC ;
                         hBM=&data->hBitMap ;                  }

              BitBlt(hDC_wnd,                                       /* Восстановление фона */
                     metrics->x_base, metrics->y_base, 
                     metrics->x_size, metrics->y_size, 
                    *hDC, 
                     metrics->x_base, metrics->y_base, 
                     SRCCOPY                           ) ;

      if(data->spec_oper==_UD_SCANNER_MOVE) {

        UDi_a2Dscan_position(metrics, 0, &x_mouse, &y_mouse,        /* рассчет экранной позиции */
                                           &data->x_scanner,
                                           &data->y_scanner ) ;
            UDi_cross_invert(hDC_wnd, metrics, x_mouse, y_mouse) ;  /* Отрисовка позиционного креста */
         UDi_scanner_vectors(hDC_wnd, metrics, data) ;              /* Отрисовка сканер-векторов */
                                            }             

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
        if(data->points    ==NULL ||
           data->points_cnt==  0    ) {   
                                ValidateRect(hWnd,  NULL) ;
                                        break ;
                                      }

                         hDC_wnd=BeginPaint(hWnd, &PaintCfg) ;

        if(zoom_flag) {  hDC=&data->zooms[zoom_num].hDC ;
                         hBM=&data->zooms[zoom_num].hBitMap ;  }
        else          {  hDC=&data->hDC ;
                         hBM=&data->hBitMap ;                  }

       if(*hBM!=NULL)  DeleteObject(*hBM) ;
       if(*hDC!=NULL)  DeleteDC    (*hDC) ;

          *hDC=CreateCompatibleDC    ( hDC_wnd) ; 
          *hBM=CreateCompatibleBitmap( hDC_wnd, GetDeviceCaps(hDC_wnd, HORZRES), 
                                                GetDeviceCaps(hDC_wnd, VERTRES) ) ; 
                         SelectObject(*hDC, *hBM) ;

//                         data->hDC    =hDC ;
//                         data->hBitMap=hBM ;

               GetClientRect(hWnd, &Rect) ;
                     metrics->x_base=Rect.left ;
                     metrics->y_base=Rect.top ;
                     metrics->x_size=Rect.right -Rect.left ;
                     metrics->y_size=Rect.bottom-Rect.top ;
                        
                    SetBkColor(*hDC, data->back_color) ;

          UDi_area_2D_init    (*hDC, data, metrics) ;               /* Начальная установка параметров */
          UDi_area_2D_sizing  (*hDC, data, metrics) ;               /* Распределение зон оцифровки и поля отрисовки */
          UDi_area_2D_scale   (*hDC, data, metrics) ;               /* Формирование шкал данных */
          UDi_area_2D_clear   (*hDC, data, metrics) ;               /* Очистка перерисовываемых зон */
          UDi_area_2D_frame   (*hDC, data, metrics) ;               /* Отрисовка рамки */

          UDi_area_2D_rare    (*hDC, data, metrics) ;               /* Прореживание сетки данных */
          UDi_area_2D_isoline (*hDC, data, metrics) ;               /* Отрисовка изолиний */
          UDi_area_2D_scalar  (*hDC, data, metrics) ;               /* Отрисовка скалярного поля */
          UDi_area_2D_vector  (*hDC, data, metrics) ;               /* Отрисовка векторов */
          UDi_area_2D_grid    (*hDC, data, metrics) ;               /* Отрисовка шкал и оцифровки */
          UDi_area_2D_ceil    (*hDC, data, metrics) ;               /* Отрисовка структуры ячеек */
          UDi_area_2D_obstacle(*hDC, data, metrics) ;               /* Отрисовка препятствий и накладных элементов */
          UDi_area_2D_profile (*hDC, data, metrics) ;               /* Отрисовка линий сечений */

            BitBlt(hDC_wnd, 
                   metrics->x_base, metrics->y_base, 
                   metrics->x_size, metrics->y_size, 
                  *hDC,
                   metrics->x_base, metrics->y_base, 
                   SRCCOPY                           ) ;

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
     else                  {

                                    x_zone_beg=LOWORD(lParam) ; 
                                    y_zone_beg=HIWORD(lParam) ; 
                                    x_zone_end= -1 ; 
                                    y_zone_end= -1 ; 

                                     zone_flag=1 ;

                                    SetCapture(hWnd) ;              /* Захватываем мышь */
                           }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
     		                      break ;
                         }
/*--------------------------------- Нажатие/отпускание правой кнопки */

    case WM_RBUTTONUP:   {
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Сечение */
       if(profile_flag) {
                                    profile_flag=0 ;

                                  ReleaseCapture() ;                /* Освобождаем мышь */

                             x_mouse=LOWORD(lParam) ; 
                             y_mouse=HIWORD(lParam) ; 

#define  PROFS  data->profiles
#define  P_CNT  data->profiles_cnt

          PROFS=(struct UD_profile *)                              /* Размещаем новое описание */
                     realloc(PROFS, (P_CNT+1)*sizeof(*PROFS)) ;

             UDi_a2Dscan_position(metrics, 1, &x_mouse_prv, &y_mouse_prv, 
                                               &PROFS[P_CNT].x_1, 
                                               &PROFS[P_CNT].y_1 ) ;
             UDi_a2Dscan_position(metrics, 1, &x_mouse, &y_mouse, 
                                               &PROFS[P_CNT].x_2, 
                                               &PROFS[P_CNT].y_2 ) ;
                                                      P_CNT++ ;

            UDi_area_2D_prof_pnts(data, &PROFS[P_CNT-1]) ;          /* Определяем набор точек сечения */

        if(data->ctrl_proc!=NULL) 
                data->ctrl_proc(_UD_PROFILE, P_CNT-1, data) ;       /* Вызываем процедуру обработки */

              SendMessage(hWnd, WM_PAINT, (WPARAM)0, (LPARAM)0) ;   /* Перерисовываем */

                                     x_mouse_prv=0 ;
                                     y_mouse_prv=0 ;
                                    
                                          break ;

#undef  PROFS
#undef  P_CNT
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                         }   
    case WM_RBUTTONDOWN: {
                              if(data==NULL)  break ;
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Задание сечения */
     if((wParam & MK_CONTROL)==MK_CONTROL) {

                                   profile_flag= 1 ;

                                    x_mouse_prv=LOWORD(lParam) ; 
                                    y_mouse_prv=HIWORD(lParam) ;

                                        SetCapture(hWnd) ;          /* Захватываем мышь */

                                                break ;
                                           } 
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
/*- - - - - - - - - - - - - - - - - - - - Отработка сканер-процедуры */
             UDi_a2Dscan_position(metrics, 1, &x_scanner, &y_scanner, 
                                               &data->x_scanner, 
                                               &data->y_scanner ) ;

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
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Сечение */
          if(profile_flag) {

              if(x_mouse_prv==0 ||
                 y_mouse_prv==0   )  break ;

                InvalidateRect(hWnd, NULL, false) ;
            hDC_wnd=BeginPaint(hWnd, &PaintCfg) ;

        if(zoom_flag) {  hDC=&data->zooms[zoom_num].hDC ;
                         hBM=&data->zooms[zoom_num].hBitMap ;  }
        else          {  hDC=&data->hDC ;
                         hBM=&data->hBitMap ;                  }

              BitBlt(hDC_wnd, 
                     metrics->x_base, metrics->y_base, 
                     metrics->x_size, metrics->y_size, 
                    *hDC, 
                     metrics->x_base, metrics->y_base, 
                     SRCCOPY                           ) ;

            MoveToEx(hDC_wnd, x_mouse_prv, y_mouse_prv, NULL) ;
              LineTo(hDC_wnd, x_mouse    , y_mouse          ) ;

                 ValidateRect(hWnd,  NULL) ;
                     EndPaint(hWnd, &PaintCfg) ;

                           }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - Сканер */
     else if((wParam & MK_RBUTTON)==MK_RBUTTON) {

            if(data==NULL)  break ; 

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

                if(scanner_flag==0) {
                                       ReleaseCapture() ;
                                          x_scanner=-1 ; 
                                          y_scanner=-1 ;
                                              break ;
                                    }

                                          x_scanner=x_mouse ; 
                                          y_scanner=y_mouse ;

               UDi_a2Dscan_position(metrics, 1, &x_scanner, &y_scanner,
                                                 &data->x_scanner, 
                                                 &data->y_scanner ) ;

              if(data->ctrl_proc)                                   /* Вызываем процедуру обработки */   
                    data->ctrl_proc(scanner_oper, zoom_num, data) ;

                                    break ;
                                                }
/*- - - - - - - - - - - - - - - - - - - - - -  Инструментальная зона */
     else if(zone_flag) {
                           InvalidateRect(hWnd, NULL, false) ;
                       hDC_tmp=BeginPaint(hWnd, &PaintCfg) ;
              
        for(i=0 ; i<2 ; i++) {
                                Rect.left  =x_zone_beg ;
                                Rect.right =x_zone_end ;
                                Rect.top   =y_zone_beg ;
                                Rect.bottom=y_zone_end ;

                     UDi_zone_invert(hDC_tmp, metrics, &Rect) ;

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
     		                      break ;
                         }
/*------------------------------------------ Отпускание левой кнопки */

    case WM_LBUTTONUP:   {
                               smooth_zoom_flag=0 ;
/*- - - - - - - - - - - - - - - - - - - - - -  Инструментальная зона */
                             if(!zone_flag)  break ;
                                 zone_flag=0 ;

                          ReleaseCapture() ;                        /* Освобождаем мышь */

                             if(x_zone_end<0)  break ;
/*- - - - - - - - - - - - - - - - - - Удаление инструментальной зоны */
                    InvalidateRect(hWnd, NULL, false) ;
                hDC_tmp=BeginPaint(hWnd, &PaintCfg) ;

                               Rect.left  =x_zone_beg ;
                               Rect.right =x_zone_end ;
                               Rect.top   =y_zone_beg ;
                               Rect.bottom=y_zone_end ;

                   UDi_zone_invert(hDC_tmp, metrics, &Rect) ;

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

#undef  ZOOMS
#undef  Z_CNT

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

   void  UDi_area_2D_init(HDC  hDC, UD_area_data *view_decl, 
                                      UD_metrics *metrics)

{
      GetTextMetrics(hDC, &metrics->font) ;
}


/*********************************************************************/
/*                                                                   */
/*          Распределение зон оцифровки и поля отрисовки             */

   void  UDi_area_2D_sizing(HDC  hDC, UD_area_data *view_decl, 
                                           UD_metrics *metrics)
{
              int  size ;

/*----------------------------- Определяем 'высоту' поля X-оцифровки */

         metrics->x_scale_h=metrics->font.tmHeight ;

/*------------------------------ Определение ширины поля Y-оцифровки */

   if(metrics->zoom_flag)  size= 9 ;
   else                    size= 7 ;

         metrics->y_scale_w=size*metrics->font.tmAveCharWidth ;     /* Пересчитываем в экранные единицы */

/*----------------------------- Определение параметров поля графиков */

	metrics->draw_x_base=metrics->y_scale_w ;
	metrics->draw_y_base= 0 ;

	metrics->draw_x_size=metrics->x_size-metrics->y_scale_w ;
	metrics->draw_y_size=metrics->y_size-metrics->x_scale_h ;

/*-------------------------------------------------------------------*/
}


/*********************************************************************/
/*                                                                   */
/*                  Формирование шкал данных                         */

   void  UDi_area_2D_scale(HDC  hDC, UD_area_data *view_decl, 
                                       UD_metrics *metrics)

{
  UD_area_2Dpoint *points ;
              int  points_cnt ;
    UD_zoom_scale *zoom ;           /* Параметры масштабирования */
           double  range ;
           double  x_step ;
           double  y_step ;
           double  v_len ;
           double  v_len_max ;
               int  i ;

#define   X_MIN      view_decl->x_min
#define   X_MAX      view_decl->x_max
#define   Y_MIN      view_decl->y_min
#define   Y_MAX      view_decl->y_max

/*------------------------------------------- Определение диапазонов */

                points    =view_decl->points ;
                points_cnt=view_decl->points_cnt ;

				          X_MIN=points[0].x ;       /* Иниц.диапазонов */
				          X_MAX=points[0].x ;
				          Y_MIN=points[0].y ;
				          Y_MAX=points[0].y ;

	  for(i=1 ; i<points_cnt ; i++) {                           /* Опр.диапазоны */
		   if(points[i].x<X_MIN)  X_MIN=points[i].x ;
		   if(points[i].x>X_MAX)  X_MAX=points[i].x ;
		   if(points[i].y<Y_MIN)  Y_MIN=points[i].y ;
		   if(points[i].y>Y_MAX)  Y_MAX=points[i].y ;
				        }

                    view_decl->x_data_min =X_MIN ;
                    view_decl->x_data_max =X_MAX ;
                    view_decl->y_data_min =Y_MIN ;
                    view_decl->y_data_max =Y_MAX ;

                    view_decl->x_grid_0   =X_MIN ;
                    view_decl->y_grid_0   =Y_MIN ;
                
/*------------------------------------- Обработка пропорциональности */

   if(view_decl->prop_scale) {

           x_step=(X_MAX-X_MIN)/((double)metrics->draw_x_size) ;
           y_step=(Y_MAX-Y_MIN)/((double)metrics->draw_y_size) ;

        if(x_step<y_step)  X_MAX=X_MIN+metrics->draw_x_size*y_step ;
        if(x_step>y_step)  Y_MAX=Y_MIN+metrics->draw_y_size*x_step ;

                             }
/*-------------------------------------------- Расширение диапазонов */

                range =X_MAX-X_MIN ;
                X_MIN-=range/10. ;  
                X_MAX+=range/10. ;

                range =Y_MAX-Y_MIN ;
                Y_MIN-=range/10. ;  
                Y_MAX+=range/10. ;

/*--------------------------------------- Масштабирование диапазонов */

   if(metrics->zoom_flag) {

                       zoom =&view_decl->zooms[metrics->zoom_num] ;

                         range=X_MAX-X_MIN ;
                      X_MIN  +=range*    zoom->x_min ;
                      X_MAX  -=range*(1.-zoom->x_max) ;

                         range=Y_MAX-Y_MIN ;
                      Y_MIN  +=range*    zoom->y_min ;
                      Y_MAX  -=range*(1.-zoom->y_max) ;
                          }
/*-------------------------------------- Определение шагов оцифровки */

           view_decl->x_grid_step=UDi_range_step(X_MAX-X_MIN) ;

   if(view_decl->prop_scale)
           view_decl->y_grid_step=view_decl->x_grid_step/2. ;
   else    view_decl->y_grid_step=UDi_range_step(Y_MAX-Y_MIN)/2. ;
   
/*--------------------------- Определение максимальной длины вектора */

  if(view_decl->type & _UD_VECTOR_DATA) {

                v_len_max=-1 ;

      for(i=1 ; i<points_cnt ; i++) {                               /* Опр.диапазоны */

        if(points[i].x<X_MIN || 
           points[i].x>X_MAX ||  
           points[i].y<Y_MIN || 
           points[i].y>Y_MAX   )  continue ;

             v_len=sqrt(points[i].x_vector*points[i].x_vector+
                        points[i].y_vector*points[i].y_vector ) ;
          if(v_len>v_len_max)  v_len_max=v_len ;
                                    }

             if(v_len_max>0)  view_decl->v_len_max=v_len_max ;
             else             view_decl->v_len_max=   1. ;
                                        }
/*----------------------------------------------- Фиксация в метрике */

                      metrics->x_base_min =X_MIN ;
                      metrics->x_base_max =X_MAX ;
                      metrics->y_base_min =Y_MIN ;
                      metrics->y_base_max =Y_MAX ;

/*-------------------------------------------------------------------*/
}


/*********************************************************************/
/*                                                                   */
/*                  Очистка перерисовываемых зон                     */

   void  UDi_area_2D_clear(HDC  hDC, UD_area_data *view_decl, 
                                       UD_metrics *metrics)

{
    RECT  Rect ;
  HBRUSH  hBrush ;

/*------------------------------------------------------- Подготовка */

       hBrush=CreateSolidBrush(view_decl->back_color) ;

/*------------------------------------------ Полная очистка элемента */

                    Rect.left  =metrics->x_base ;
                    Rect.top   =metrics->y_base ;
                    Rect.right =metrics->x_base+metrics->x_size ;
                    Rect.bottom=metrics->y_base+metrics->y_size ;

	  FillRect(hDC, &Rect, hBrush) ;

/*------------------------------------------------------- Завершение */

                DeleteObject(hBrush) ;

/*-------------------------------------------------------------------*/
}


/*********************************************************************/
/*                                                                   */
/*                         Отрисовка рамки                           */

   void  UDi_area_2D_frame(HDC  hDC, UD_area_data *view_decl,
                                       UD_metrics *metrics)
{
}


/*********************************************************************/
/*                                                                   */
/*                  Отрисовка шкал и оцифровка                       */

   void  UDi_area_2D_grid(HDC  hDC, UD_area_data *view_decl, 
                                      UD_metrics *metrics)
{
             HPEN  hPen ;                    /* Перо */
             HPEN  hPen_prev ;               /* Перо, предыдущее */
           double  value ;
           double  step ;
    UD_value_zone  zone ;
              int  i ;

#define                    _VALUES_MAX   30
    UD_value_zone  x_value[_VALUES_MAX] ;    /* Описание оцифровки X-шкал */
              int  x_value_cnt ;
    UD_value_zone  y_value[_VALUES_MAX] ;    /* Описание оцифровки Y-шкал */
              int  y_value_cnt ;

/*--------------------------------- Подготовка графических установок */

     hPen     =         CreatePen(PS_DASH, 0, view_decl->grid_color) ;
     hPen_prev=(HPEN)SelectObject(hDC, hPen) ;

/*--------------------------------------- Отрисовка сетки под график */

        UDg_setviewport(hDC, metrics->draw_x_base,                  /* Уст.рабочее окно */
                             metrics->draw_y_base, 
                             metrics->draw_x_size,
                             metrics->draw_y_size ) ; 

		    x_value_cnt=0 ;
		    y_value_cnt=0 ;

	 UDg_setwindow(hDC, 1, view_decl->x_min,                    /* Масштабируем рабочее окно */
                               view_decl->y_min,       
                               view_decl->x_max, 
                               view_decl->y_max ) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Отработка по X */
	       step=view_decl->x_grid_step ;                        /* Определяем шаг по оси X */

       for(value=view_decl->x_grid_0 ;
           value<view_decl->x_data_max+0.01*step ; value+=step) {   /* CIRCLE.2 - Идем по узлам сетки */

          if(value<view_decl->x_min)  continue ;

		UDg_moveto_w(hDC, value, view_decl->y_min) ;        /* Рисуем линию сетки */
		UDg_lineto_w(hDC, value, view_decl->y_data_max) ;

	   if(x_value_cnt<_VALUES_MAX) {
		 x_value[x_value_cnt].value=                value ;
		 x_value[x_value_cnt].pos  =UDg_xpos_w(hDC, value) ;
		         x_value_cnt++ ;
				       }
							        }   /* CONTINUE.2 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Отработка по Y */
	       step=view_decl->y_grid_step ;                        /* Определяем шаг по оси X */

       for(value=view_decl->y_grid_0 ;
           value<view_decl->y_data_max+0.01*step ; value+=step) {   /* CIRCLE.3 - Идем по узлам сетки */

          if(value<view_decl->y_min)  continue ;

		UDg_moveto_w(hDC, view_decl->x_min, value) ;        /* Рисуем линию сетки */
		UDg_lineto_w(hDC, view_decl->x_data_max, value) ;

	   if(y_value_cnt<_VALUES_MAX) {
		 y_value[y_value_cnt].value=                value ;
		 y_value[y_value_cnt].pos  =UDg_ypos_w(hDC, value) ;
		         y_value_cnt++ ;
				       }
							        }   /* CONTINUE.3 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    UDg_setviewport(hDC, 0, 0, metrics->x_size, metrics->y_size) ;  /* Восстановление экранной зоны */

/*--------------------------------------------------- Оцифровка шкал */

                 SetTextColor(hDC, view_decl->grid_color) ;
                   SetBkColor(hDC, view_decl->back_color) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Отработка по X */
               UDi_value_zone(view_decl->x_min,
                              view_decl->x_max, 
                              view_decl->x_grid_step, &zone) ;

   for(i=0 ; i<x_value_cnt ; i+=2) {
	  UDi_value_cvt(x_value[i].value, &zone, 0) ;
	        TextOut(hDC, metrics->draw_x_base+x_value[i].pos
                            -metrics->font.tmAveCharWidth*zone.digits/2,
                             metrics->draw_y_base+metrics->draw_y_size,
                                      zone.buff, strlen(zone.buff)) ;
				  }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Отработка по Y */
               UDi_value_zone(view_decl->y_min,
                              view_decl->y_max,
                              view_decl->y_grid_step, &zone) ;

   for(i=0 ; i<y_value_cnt ; i++) {
	  UDi_value_cvt(y_value[i].value, &zone, 0) ;
	        TextOut(hDC,   0,
                             metrics->draw_y_base-
                              metrics->font.tmHeight/2+
                                y_value[i].pos,
                                      zone.buff, strlen(zone.buff)) ;
				  }
/*--------------------------------- Отрисовка рамки области графиков */

#ifdef REMARK
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

#endif
/*----------------------------- Восстановление графических установок */

             SelectObject(hDC, hPen_prev) ;
             DeleteObject(hPen) ;

/*-------------------------------------------------------------------*/

}


/*********************************************************************/
/*                                                                   */
/*                   Прореживание сетки данных                       */

   void  UDi_area_2D_rare(HDC  hDC, UD_area_data *view_decl, 
                                      UD_metrics *metrics)

{
  UD_area_2Dpoint *points ;
              int  points_cnt ;
              int  i ;


                points    =view_decl->points ;
                points_cnt=view_decl->points_cnt ;

   for(i=0 ; i<points_cnt ; i++) {
                                        points[i].rare_active=1 ;
                                 }
}


/*********************************************************************/
/*                                                                   */
/*                    Отрисовка структуры ячеек                      */

   void  UDi_area_2D_ceil(HDC  hDC, UD_area_data *view_decl, 
                                      UD_metrics *metrics)

{
  UD_area_2Dpoint *points ;
              int  points_cnt ;
             HPEN  hPen ;           /* Перо */
             HPEN  hPen_prev ;      /* Перо, предыдущее */
              int  draw[100] ;      /* Список связанных активных точек */
              int  draw_cnt ;
              int  i ;
              int  j ;

/*-------------------------------------------------- Контроль режима */

      if(!(view_decl->type & _UD_GRID_DATA))  return ;

/*-------------------------------------------------- Подготовка окна */

        UDg_setviewport(hDC, metrics->draw_x_base,                  /* Уст.рабочее окно */
                             metrics->draw_y_base, 
                             metrics->draw_x_size,
                             metrics->draw_y_size ) ; 

	  UDg_setwindow(hDC, 1, view_decl->x_min,                   /* Масштабируем рабочее окно */
                                view_decl->y_min,       
                                view_decl->x_max, 
                                view_decl->y_max ) ;              

     hPen     =         CreatePen(PS_SOLID, 0,                      /* Подготовка пера */    
                                      view_decl->fore_color) ;      
     hPen_prev=(HPEN)SelectObject(hDC, hPen) ;
            
/*-------------------------------------------- Отрисовка сетки ячеек */

                points    =view_decl->points ;
                points_cnt=view_decl->points_cnt ;

   for(i=0 ; i<points_cnt ; i++) {                                  /* CIRCLE.1 - Перебираем ячейки */
        
       if(!points[i].rare_active)  continue ;                       /* Пассивные точки игнорируем */

         draw_cnt=UDi_area_2D_links(points, points_cnt, i, draw) ;  /* Составляем список связанных активных точек */

    for(j=0 ; j<draw_cnt ; j++) {                                   /* Отрисовываем связи */    
       UDg_moveto_w(hDC, points[     i ].x, points[     i ].y) ; 
       UDg_lineto_w(hDC, points[draw[j]].x, points[draw[j]].y) ;
                                }
                                 }                                  /* CONTINUE.1 */

/*----------------------------- Восстановление графических установок */

       SelectObject(hDC, hPen_prev) ;
       DeleteObject(hPen) ;

    UDg_setviewport(hDC, 0, 0, metrics->x_size, metrics->y_size) ;  /* Восстановление экранной зоны */

/*-------------------------------------------------------------------*/
}


/*********************************************************************/
/*                                                                   */
/*           Отрисовка препятствий и границ расчетной сетки          */

   void  UDi_area_2D_obstacle(HDC  hDC, UD_area_data *view_decl, 
                                          UD_metrics *metrics)

{
  UD_area_2Dpoint *points ;
              int  points_cnt ;
  UD_data_2Dpoint *spots ;
              int  spots_cnt ;
              int  draw[100] ;        /* Список связанных активных точек */
              int  draw_cnt ;
       UD_point_w  polygon[100] ;     /* Полигон заливки */
             HPEN  hPen ;             /* Перо */
             HPEN  hPen_prv ;         /* Перо */
           HBRUSH  hBrush ;           /* Цветная кисть */
         COLORREF  color ;
           double  x1 ;
           double  y1 ;
           double  x2 ;
           double  y2 ;
              int  i ;
              int  j ;

/*----------------------------------------------------- Задание окна */

        UDg_setviewport(hDC, metrics->draw_x_base,                  /* Уст.рабочее окно */
                             metrics->draw_y_base, 
                             metrics->draw_x_size,
                             metrics->draw_y_size ) ; 

	  UDg_setwindow(hDC, 1, view_decl->x_min,                   /* Масштабируем рабочее окно */
                                view_decl->y_min,       
                                view_decl->x_max, 
                                view_decl->y_max ) ;              

/*------------------------------------------------- Подготовка цвета */

      color   =                RGB(80, 80, 80) ;
      hPen    =         CreatePen(PS_SOLID, 0, color) ;             /* Подготовка пера */    
      hBrush  =  CreateSolidBrush(color) ;                          /* Подготовка кисти */

      hPen_prv=(HPEN)SelectObject(hDC, hPen) ;
                     SelectObject(hDC, hBrush) ;
            
/*------------------------------------------- Прорисовка препятствий */

                points    =view_decl->points ;
                points_cnt=view_decl->points_cnt ;
                 spots    =view_decl->spots ;
                 spots_cnt=view_decl->spots_cnt ;
                   
   for(i=0 ; i<points_cnt ; i++) {                                  /* CIRCLE.1 - Перебираем ячейки */

        if(!points[i].obstacle   )  continue ;
        if(!points[i].rare_active)  continue ;                      /* Пассивные точки игнорируем */

         draw_cnt=UDi_area_2D_spot  (points, points_cnt, i, draw) ; /* Составляем список точек границ пятна ячейки */
                  UDi_area_2D_p_area(spots, draw, draw_cnt,         /* Формирование полигона закраски */ 
                                                i, polygon   ) ;
                       UDg_polygon_w(hDC, polygon, draw_cnt) ;      /* Отрисовка полигона */

                                 }                                  /* CONTINUE.1 */

/*------------------------------- Прорисовка границ рассчетной сетки */

                     SelectObject(hDC, hPen_prv) ;
                     DeleteObject(hPen) ;

                   hPen=CreatePen(PS_SOLID, 0, RGB(255,0,0)) ;
               (HPEN)SelectObject(hDC, hPen) ;

   for(i=0 ; i<points_cnt ; i++) {                                  /* CIRCLE.2 - Перебираем ячейки */

        if(!points[i].boundary   )  continue ;
        if(!points[i].rare_active)  continue ;                      /* Пассивные точки игнорируем */

     for(j=0 ; j<points[i].link_cnt ; j++)
      if(       points[i].link_list[j] > i   && 
         points[points[i].link_list[j]].boundary) {

                      x1=points[       i              ].x ;
                      y1=points[       i              ].y ;
                      x2=points[points[i].link_list[j]].x ;
                      y2=points[points[i].link_list[j]].y ;

                           UDg_moveto_w(hDC, x1, y1) ;
                           UDg_lineto_w(hDC, x2, y2) ;
                         UDg_setpixel_w(hDC, x2, y2, color) ;
                                                  }
                                 }
/*----------------------------------- Прорисовка накладных элементов */

#define   F   view_decl->figures[i]

   for(i=0 ; i<view_decl->figures_cnt ; i++) {

                     SelectObject(hDC, hPen_prv) ;
                     DeleteObject(hPen) ;

                   hPen=CreatePen(PS_SOLID, 2, F.color) ;
               (HPEN)SelectObject(hDC, hPen) ;

     for(j=0 ; j<F.elements_cnt ; j++) {
       if(F.elements[j].type==_UD_LINE_ELEM) {
             UDg_moveto_w(hDC, F.elements[j].x1, F.elements[j].y1) ;
             UDg_lineto_w(hDC, F.elements[j].x2, F.elements[j].y2) ;
                                             }
                                       }
                                             }

#undef    F

/*----------------------------- Восстановление графических установок */

       SelectObject(hDC, hPen_prv) ;
       DeleteObject(hPen) ;
       DeleteObject(hBrush) ;

    UDg_setviewport(hDC, 0, 0, metrics->x_size, metrics->y_size) ;  /* Восстановление экранной зоны */

/*-------------------------------------------------------------------*/
}


/*********************************************************************/
/*                                                                   */
/*                      Отрисовка скалярных данных                   */

   void  UDi_area_2D_scalar(HDC  hDC, UD_area_data *view_decl, 
                                        UD_metrics *metrics)

{
  UD_area_2Dpoint *points ;
              int  points_cnt ;
  UD_data_2Dpoint *spots ;
              int  spots_cnt ;
              int  draw[100] ;        /* Список связанных активных точек */
              int  draw_cnt ;
       UD_point_w  polygon[100] ;     /* Полигон заливки */
             HPEN  hPen ;             /* Перо */
             HPEN  hPen_prev ;        /* Перо, предыдущее */
             HPEN  hPen_color[2] ;    /* Цветное перо */
           HBRUSH  hBrush_color[2] ;  /* Цветная кисть */
              int  pen_num ;          /* Номер пера */
         COLORREF  color ;
           double  s_value ;
              int  i ;

/*-------------------------------------------------- Контроль режима */

      if(!(view_decl->type & _UD_SCALAR_DATA))  return ;

      if( (view_decl->type & _UD_ISOLINE_DATA))  return ;

/*------------------------------------------------------- Подготовка */


                        hPen_color[0]=NULL ;
                        hPen_color[1]=NULL ;

/*----------------------------------------------------- Задание окна */

        UDg_setviewport(hDC, metrics->draw_x_base,                  /* Уст.рабочее окно */
                             metrics->draw_y_base, 
                             metrics->draw_x_size,
                             metrics->draw_y_size ) ; 

	  UDg_setwindow(hDC, 1, view_decl->x_min,                   /* Масштабируем рабочее окно */
                                view_decl->y_min,       
                                view_decl->x_max, 
                                view_decl->y_max ) ;              

     hPen     =         CreatePen(PS_SOLID, 0,                      /* Подготовка пера */    
                                      view_decl->fore_color) ;      
     hPen_prev=(HPEN)SelectObject(hDC, hPen) ;
            
/*---------------------------------------------------- Перебор точек */

                points    =view_decl->points ;
                points_cnt=view_decl->points_cnt ;
                 spots    =view_decl->spots ;
                 spots_cnt=view_decl->spots_cnt ;
                   
   for(pen_num=0, i=0 ; i<points_cnt ; pen_num=!pen_num, i++) {     /* CIRCLE.1 - Перебираем ячейки */

        if(points[i].obstacle)  continue ;

/*------------------------------------------------- Отрисовка растра */

    if(view_decl->scalar_regime & _UD_RASTER_SIZE) {

             s_value=points[i].value ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - -  Режим цвета */
   if(view_decl->scalar_regime & _UD_FIXED_COLOR ||
      view_decl->scalar_regime & _UD_SMART_COLOR   ) {

         color=UDi_assign_color(view_decl->scalar_regime, 
                                 s_value, view_decl->scalar_colors) ;
                                                     }
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Отрисовка точки */
          UDg_setpixel_w(hDC, points[i].x, points[i].y, color) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                   }
/*------------------------------------------------ Отрисовка заливки */

    else                                           { 
        
        if(!points[i].rare_active)  continue ;                      /* Пассивные точки игнорируем */

             s_value=points[i].value ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - -  Режим цвета */
   if(view_decl->scalar_regime & _UD_FIXED_COLOR ||
      view_decl->scalar_regime & _UD_SMART_COLOR   ) {

         color=UDi_assign_color(view_decl->scalar_regime,           /* Определение цвета */    
                                 s_value, view_decl->scalar_colors) ;

          hPen_color[pen_num]=CreatePen(PS_SOLID, 0, color) ;       /* Подготовка пера */                                           
                           SelectObject(hDC, hPen_color[pen_num]) ;
                           DeleteObject(hPen_color[!pen_num]) ;

        hBrush_color[pen_num]=CreateSolidBrush(color) ;             /* Подготовка кисти */    
                                  SelectObject(hDC, hBrush_color[pen_num]) ;
                                  DeleteObject(hBrush_color[!pen_num]) ;
                                                     }
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Отрисовка точки */
         draw_cnt=UDi_area_2D_spot  (points, points_cnt, i, draw) ; /* Составляем список точек границ пятна ячейки */
                  UDi_area_2D_p_area(spots, draw, draw_cnt,         /* Формирование полигона закраски */ 
                                                i, polygon   ) ;
                       UDg_polygon_w(hDC, polygon, draw_cnt) ;      /* Отрисовка полигона */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                   }
/*---------------------------------------------------- Перебор точек */

                                                              }     /* CONTINUE.1 */

/*----------------------------- Восстановление графических установок */

       SelectObject(hDC, hPen_prev) ;
       DeleteObject(hPen) ;
       DeleteObject(hPen_color[pen_num]) ;
       DeleteObject(hBrush_color[pen_num]) ;

    UDg_setviewport(hDC, 0, 0, metrics->x_size, metrics->y_size) ;  /* Восстановление экранной зоны */

/*-------------------------------------------------------------------*/
}


/*********************************************************************/
/*                                                                   */
/*                 Отрисовка изолиний скалярных данных               */

   void  UDi_area_2D_isoline(HDC  hDC, UD_area_data *view_decl, 
                                         UD_metrics *metrics)

{
  UD_area_2Dpoint *points ;
              int  points_cnt ;
              int  loops[200] ;       /* Список точек циклов графа */
              int  loops_cnt ;
              int *loop ;
              int  loop_size ;
             HPEN  hPen ;             /* Перо */
             HPEN  hPen_prev ;        /* Перо, предыдущее */
             HPEN  hPen_color[2] ;    /* Цветное перо */
              int  pen_num ;          /* Номер пера */
           double  x1 ;
           double  y1 ;
           double  x2 ;
           double  y2 ;
         COLORREF  color ;
              int  layer1min ;
              int  layer1max ;
              int  layer2min ;
              int  layer2max ;
              int  layer ;
              int  i ;
              int  j ;
              int  k1 ;
              int  k2 ;

/*-------------------------------------------------- Контроль режима */

      if(!(view_decl->type & _UD_SCALAR_DATA))  return ;

      if(!(view_decl->type & _UD_ISOLINE_DATA))  return ;

/*------------------------------------------------------- Подготовка */

                        hPen_color[0]=NULL ;
                        hPen_color[1]=NULL ;

/*--------------------------------- Построение сетки граничных точек */

                points    =view_decl->points ;
                points_cnt=view_decl->points_cnt ;
                   
   for(i=0 ; i<points_cnt ; i++) {                                  /* Определение номера слоя */  
            points[i].layer =UDi_assign_index(points[i].value,
                                               view_decl->scalar_colors) ;
            points[i].inside=0 ;
                                 }

   for(i=0 ; i<points_cnt ; i++) {                                  /* Определение граничных точек */

     for(j=0 ; j<points[i].link_cnt ; j++)
      if(points[points[i].link_list[j]].layer!=points[i].layer)  break ;

      if(j==points[i].link_cnt)  points[i].inside=1 ;
                                 }
/*----------------------------------------------------- Задание окна */

        UDg_setviewport(hDC, metrics->draw_x_base,                  /* Уст.рабочее окно */
                             metrics->draw_y_base, 
                             metrics->draw_x_size,
                             metrics->draw_y_size ) ; 

	  UDg_setwindow(hDC, 1, view_decl->x_min,                   /* Масштабируем рабочее окно */
                                view_decl->y_min,       
                                view_decl->x_max, 
                                view_decl->y_max ) ;              

     hPen     =         CreatePen(PS_SOLID, 0,                      /* Подготовка пера */    
                                      view_decl->fore_color) ;      
     hPen_prev=(HPEN)SelectObject(hDC, hPen) ;
            
/*---------------------------------------------------- Перебор точек */

                points    =view_decl->points ;
                points_cnt=view_decl->points_cnt ;
                   
   for(pen_num=0, i=0 ; i<points_cnt ; i++) {                       /* CIRCLE.1 - Перебираем ячейки */

            if(points[i].inside)  continue ;

/*------------------------------------------------- Отрисовка растра */

#ifdef  _TEST_AND_DEBUG

            x1=points[i].value ;

         color=UDi_assign_color(_UD_FIXED_COLOR, 
                                 x1, view_decl->scalar_colors) ;
                                                     
          UDg_setpixel_w(hDC, points[i].x, points[i].y, color) ;

#endif

/*----------------------------------------------- Поиск циклов графа */

        loops_cnt=UDi_area_2D_graphloops(i, points, loops) ;

/*---------------------------------------------- Отрисовка "фронтов" */

     for(loop=loops, loop_size=1, j=1 ; j<loops_cnt ; 
                     loop_size++, j++                ) {
                
       if(loops[j]==*loop) {
/*- - - - - - - - - - - - - - - - - -  Обработкание одиночного цикла */
        for(k1=0 ; k1<loop_size ; k1++) {

          if(points[loop[k1  ]].layer==
             points[loop[k1+1]].layer  )  continue ;

         for(k2=k1+1 ; k2<loop_size ; k2++) {

           if(points[loop[k2  ]].layer==
              points[loop[k2+1]].layer  )  continue ;

                 x1=(points[loop[k1]].x+points[loop[k1+1]].x)/2. ;
                 y1=(points[loop[k1]].y+points[loop[k1+1]].y)/2. ;
                 x2=(points[loop[k2]].x+points[loop[k2+1]].x)/2. ;
                 y2=(points[loop[k2]].y+points[loop[k2+1]].y)/2. ;

             layer1min=__min(points[loop[k1]].layer, points[loop[k1+1]].layer) ;
             layer1max=__max(points[loop[k1]].layer, points[loop[k1+1]].layer) ;
             layer2min=__min(points[loop[k2]].layer, points[loop[k2+1]].layer) ;
             layer2max=__max(points[loop[k2]].layer, points[loop[k2+1]].layer) ;

          if(layer1min>=layer2max ||
             layer2min>=layer1max   )  continue ;

              layer=__min(layer1max, layer2max) ;
            
              color=view_decl->scalar_colors[layer].color ;

          hPen_color[pen_num]=CreatePen(PS_SOLID, 0, color) ;       /* Подготовка пера */                                           
                           SelectObject(hDC, hPen_color[pen_num]) ;
                           DeleteObject(hPen_color[!pen_num]) ;

                           UDg_moveto_w(hDC, x1, y1) ;              /* Отрисовка фронта */
                           UDg_lineto_w(hDC, x2, y2) ;
                         UDg_setpixel_w(hDC, x2, y2, color) ;

                                               pen_num=!pen_num ;
                                            }
                                        }
/*- - - - - - - - - - - - - - - - - - - Фиксация начала нового цикла */
                                loop     =loops+j+1 ;
                                loop_size=0 ;  
                                        j++ ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                           }
                                                       }
/*---------------------------------------------------- Перебор точек */

                                                              }     /* CONTINUE.1 */

/*----------------------------- Восстановление графических установок */

       SelectObject(hDC, hPen_prev) ;
       DeleteObject(hPen) ;
       DeleteObject(hPen_color[pen_num]) ;

    UDg_setviewport(hDC, 0, 0, metrics->x_size, metrics->y_size) ;  /* Восстановление экранной зоны */

/*-------------------------------------------------------------------*/
}


/*********************************************************************/
/*                                                                   */
/*                    Отрисовка векторных данных                     */

   void  UDi_area_2D_vector(HDC  hDC, UD_area_data *view_decl, 
                                        UD_metrics *metrics)

{
  UD_area_2Dpoint *points ;
              int  points_cnt ;
             HPEN  hPen ;           /* Перо */
             HPEN  hPen_prev ;      /* Перо, предыдущее */
             HPEN  hPen_color[2] ;  /* Цветное перо */
              int  pen_num ;        /* Номер пера */
         COLORREF  color ;
           double  v_const ;        /* Длина вектора для режима постоянных векторов */
           double  v_arrow ;        /* Размер стрелочки */
           double  v_value ;        /* Еличина вектора */
           double  scale ;
           double  dx ;
           double  dy ;
              int  i ;

/*-------------------------------------------------- Контроль режима */

      if(!(view_decl->type & _UD_VECTOR_DATA))  return ;

/*------------------------------------------------------- Подготовка */

     v_arrow=  5. ;
     v_const=sqrt((view_decl->x_max-view_decl->x_min)*
                  (view_decl->x_max-view_decl->x_min) +
                  (view_decl->y_max-view_decl->y_min)*
                  (view_decl->y_max-view_decl->y_min)  )/15. ;

                        hPen_color[0]=NULL ;
                        hPen_color[1]=NULL ;

/*----------------------------------------------------- Задание окна */

        UDg_setviewport(hDC, metrics->draw_x_base,                  /* Уст.рабочее окно */
                             metrics->draw_y_base, 
                             metrics->draw_x_size,
                             metrics->draw_y_size ) ; 

	  UDg_setwindow(hDC, 1, view_decl->x_min,                   /* Масштабируем рабочее окно */
                                view_decl->y_min,       
                                view_decl->x_max, 
                                view_decl->y_max ) ;              

     hPen     =         CreatePen(PS_SOLID, 0,                      /* Подготовка пера */    
                                      view_decl->fore_color) ;      
     hPen_prev=(HPEN)SelectObject(hDC, hPen) ;
            
/*----------------------------------------------- Отрисовка векторов */

                points    =view_decl->points ;
                points_cnt=view_decl->points_cnt ;
                   

   for(pen_num=0, i=0 ; i<points_cnt ; pen_num=!pen_num, i++) {     /* CIRCLE.1 - Перебираем ячейки */
        
        if(!points[i].rare_active)  continue ;                      /* Пассивные точки игнорируем */

        if(points[i].x_vector==0. &&                                /* Если скорости нет */
           points[i].y_vector==0.   )  continue ;

             v_value=sqrt(points[i].x_vector*points[i].x_vector+
                          points[i].y_vector*points[i].y_vector ) ;
/*- - - - - - - - - - - - - - - - - - - - - - -  Режим длины вектора */
   if(view_decl->vector_regime & _UD_FIXED_LEN) {

        scale=v_value/v_const ;
                                                }
   else                                         {

        scale=view_decl->v_len_max/v_const ;
                                                } 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - -  Режим цвета */
   if(view_decl->vector_regime & _UD_FIXED_COLOR ||
      view_decl->vector_regime & _UD_SMART_COLOR   ) {

         color=UDi_assign_color(view_decl->vector_regime, 
                                 v_value, view_decl->vector_colors) ;

          hPen_color[pen_num]=CreatePen(PS_SOLID, 0, color) ;       /* Подготовка пера */                                           
                           SelectObject(hDC, hPen_color[pen_num]) ;
                           DeleteObject(hPen_color[!pen_num]) ;
                                                     }
/*- - - - - - - - - - - - - - - - - - - - - - - -  Отрисовка вектора */
                    dx=points[i].x_vector/scale ;
                    dy=points[i].y_vector/scale ;

          UDg_moveto_w(hDC, points[i].x,    points[i].y   ) ;       /* Тело вектора */
          UDg_lineto_w(hDC, points[i].x+dx, points[i].y+dy) ;

     UDi_area_2D_arrow(hDC, points[i].x,    points[i].y,            /* "Стрелочка" вектора */
                            points[i].x+dx, points[i].y+dy ) ;       
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                              }     /* CONTINUE.1 */
/*----------------------------- Восстановление графических установок */

       SelectObject(hDC, hPen_prev) ;
       DeleteObject(hPen_color[pen_num]) ;
       DeleteObject(hPen) ;

    UDg_setviewport(hDC, 0, 0, metrics->x_size, metrics->y_size) ;  /* Восстановление экранной зоны */

/*-------------------------------------------------------------------*/
}


/*********************************************************************/
/*                                                                   */
/*                    Отрисовка векторных данных                     */

   void  UDi_scanner_vectors(HDC  hDC, UD_metrics *metrics, 
                                     UD_area_data *view_decl)

{
             HPEN  hPen ;           /* Перо */
             HPEN  hPen_prev ;      /* Перо, предыдущее */
             HPEN  hPen_color[2] ;  /* Цветное перо */
              int  pen_num ;        /* Номер пера */
           double  v_const ;        /* Длина вектора для режима постоянных векторов */
           double  v_arrow ;        /* Размер стрелочки */
           double  v_value ;        /* Еличина вектора */
           double  scale ;
           double  dx ;
           double  dy ;
              int  i ;

/*------------------------------------------------------- Подготовка */

     v_arrow=  5. ;
     v_const=sqrt((metrics->x_base_max-metrics->x_base_min)*
                  (metrics->x_base_max-metrics->x_base_min) +
                  (metrics->y_base_max-metrics->y_base_min)*
                  (metrics->y_base_max-metrics->y_base_min)  )/7. ;

                        hPen_color[0]=NULL ;
                        hPen_color[1]=NULL ;

/*----------------------------------------------------- Задание окна */

        UDg_setviewport(hDC,    metrics->draw_x_base,               /* Уст.рабочее окно */
                                metrics->draw_y_base, 
                                metrics->draw_x_size,
                                metrics->draw_y_size ) ; 

	  UDg_setwindow(hDC, 1, metrics->x_base_min,                /* Масштабируем рабочее окно */
                                metrics->y_base_min,       
                                metrics->x_base_max, 
                                metrics->y_base_max ) ;              

     hPen     =         CreatePen(PS_SOLID, 0,                      /* Подготовка пера */    
                                      view_decl->fore_color) ;      
     hPen_prev=(HPEN)SelectObject(hDC, hPen) ;
            
/*----------------------------------------------- Отрисовка векторов */

#define   V       view_decl->v_scanner_list
#define   V_CNT   view_decl->v_scanner_cnt


   for(pen_num=0, i=0 ; i<V_CNT ; pen_num=!pen_num, i++) {          /* CIRCLE.1 - Перебираем сканер-вектора */
        
        if(V[i].x_vector==0. &&                                     /* Если скорости нет... */
           V[i].y_vector==0.   )  continue ;
/*- - - - - - - - - - - - - - - - - - - - - -  Подготовка аттрибутов */
             v_value=sqrt(V[i].x_vector*V[i].x_vector+
                          V[i].y_vector*V[i].y_vector ) ;
               scale=v_value/v_const ;

          hPen_color[pen_num]=CreatePen(PS_SOLID, 0, V[i].color) ;  /* Подготовка пера */                                           
                           SelectObject(hDC, hPen_color[pen_num]) ;
                           DeleteObject(hPen_color[!pen_num]) ;                                                      
/*- - - - - - - - - - - - - - - - - - - - - - - -  Отрисовка вектора */
                              dx=V[i].x_vector/scale ;
                              dy=V[i].y_vector/scale ;

               UDg_moveto_w(hDC, V[i].x,    V[i].y   ) ;            /* Тело вектора */
               UDg_lineto_w(hDC, V[i].x+dx, V[i].y+dy) ;

          UDi_area_2D_arrow(hDC, V[i].x,    V[i].y,                 /* "Стрелочка" вектора */
                                 V[i].x+dx, V[i].y+dy ) ;       
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                              }     /* CONTINUE.1 */

#undef    V
#undef    V_CNT

/*----------------------------- Восстановление графических установок */

       SelectObject(hDC, hPen_prev) ;
       DeleteObject(hPen_color[pen_num]) ;
       DeleteObject(hPen) ;

    UDg_setviewport(hDC, 0, 0, metrics->x_size, metrics->y_size) ;  /* Восстановление экранной зоны */

/*-------------------------------------------------------------------*/
}


/*********************************************************************/
/*                                                                   */
/*                    Отрисовка линий сечений                        */

   void  UDi_area_2D_profile(HDC  hDC, UD_area_data *view_decl, 
                                         UD_metrics *metrics)

{
     HPEN  hPen ;           /* Перо */
     HPEN  hPen_prev ;      /* Перо, предыдущее */
      int  i ;

/*-------------------------------------------------- Подготовка окна */

        UDg_setviewport(hDC, metrics->draw_x_base,                  /* Уст.рабочее окно */
                             metrics->draw_y_base, 
                             metrics->draw_x_size,
                             metrics->draw_y_size ) ; 

	  UDg_setwindow(hDC, 1, view_decl->x_min,                   /* Масштабируем рабочее окно */
                                view_decl->y_min,       
                                view_decl->x_max, 
                                view_decl->y_max ) ;              
            
/*------------------------------------------ Отрисовка линий сечений */

#define  PROFS  view_decl->profiles
#define  P_CNT  view_decl->profiles_cnt

   for(i=0 ; i<P_CNT ; i++) {                                       /* CIRCLE.1 - Перебираем сечения */

         hPen     =         CreatePen(PROFS[i].line_style, 0,       /* Подготовка пера */    
                                          view_decl->fore_color) ;
         hPen_prev=(HPEN)SelectObject(hDC, hPen) ;

                 UDg_moveto_w(hDC, PROFS[i].x_1, PROFS[i].y_1) ;    /* Отрисовка */
                 UDg_lineto_w(hDC, PROFS[i].x_2, PROFS[i].y_2) ;

                 SelectObject(hDC, hPen_prev) ;                     /* Восстановление пера */
                 DeleteObject(hPen) ;
                            }                                       /* CONTINUE.1 */

#undef  PROFS
#undef  P_CNT

/*----------------------------- Восстановление графических установок */

    UDg_setviewport(hDC, 0, 0, metrics->x_size, metrics->y_size) ;  /* Восстановление экранной зоны */

/*-------------------------------------------------------------------*/
}


/*********************************************************************/
/*                                                                   */
/*                    Отрисовка стрелочки вектора                    */

   void  UDi_area_2D_arrow(HDC  hDC, double  x_from, double  y_from, 
                                     double  x_to,   double  y_to )

{
  double  dir_norm ;    /* Нормировочный коэф. направления */
  double  dir_cos ;     /* Косинус направления */
  double  dir_sin ;     /* Синус направления */
  double  x_c ;
  double  y_c ;
  double  x_b ;
  double  y_b ;
  double  x_1 ;
  double  y_1 ;
  double  x_2 ;
  double  y_2 ;

/*----------------------------------- Расчет координат острия и базы */

               x_b=UDg_xpos_w(hDC, x_from) ;
               y_b=UDg_ypos_w(hDC, y_from) ;
               x_c=UDg_xpos_w(hDC, x_to) ;
               y_c=UDg_ypos_w(hDC, y_to) ;

/*------------------------------------------ Расчет коэф.направления */

          dir_norm=sqrt((x_c-x_b)*(x_c-x_b)+(y_c-y_b)*(y_c-y_b)) ;
       if(dir_norm==0.)  return ;

          dir_cos =(x_c-x_b)/dir_norm ;
          dir_sin =(y_c-y_b)/dir_norm ;

/*----------------------------------------------- Расчет "стрелочки" */

                x_1=x_c-5.0*dir_cos+3.0*dir_sin ;
                y_1=y_c-5.0*dir_sin-3.0*dir_cos ;
                x_2=x_c-5.5*dir_cos-3.5*dir_sin ;
                y_2=y_c-5.5*dir_sin+3.5*dir_cos ;

/*-------------------------------------------- Отрисовка "стрелочки" */

            MoveToEx(hDC, x_1, y_1, NULL) ;
              LineTo(hDC, x_c, y_c      ) ;
              LineTo(hDC, x_2, y_2      ) ;

/*-------------------------------------------------------------------*/

}


/*********************************************************************/
/*                                                                   */
/*              Построение списка связанных активных точек           */

    int  UDi_area_2D_links(UD_area_2Dpoint *points,
                                       int  points_cnt, 
                                       int  point_num,
                                       int *draw       )              
{
    struct {  int  idx ;
              int  ptr ;  } scan[100] ;
              int           n_scan ;
              int           n ;
              int           draw_cnt ;

#define  S    scan[n_scan]


                         n_scan= 0 ;
                          S.idx=point_num ;
                          S.ptr= 0 ;

                       draw_cnt= 0 ;
                
        while(1) {

           if(S.ptr>=points[S.idx].link_cnt) {
                                if(n_scan==0)  break ;

                                              n_scan-- ;
                                               S.ptr++ ;     
                                               continue ;
                                             }

                     n=points[S.idx].link_list[S.ptr] ;
                                    
           if(points[n].rare_active) {
                      if(n>point_num) {  draw[draw_cnt]=n ;
                                              draw_cnt++ ;  }
                                                 S.ptr++ ;
                                                 continue ;
                                     }
           else                      {
                                                n_scan++ ;
                                                 S.idx=n ;
                                                 S.ptr=0 ;
                                                 continue ;
                                     }
                 }
#undef   S

   return(draw_cnt) ;
}


/*********************************************************************/
/*                                                                   */
/*           Построение списка точек границы пятна заливки           */

    int  UDi_area_2D_spot(UD_area_2Dpoint *points,
                                      int  points_cnt, 
                                      int  point_num,
                                      int *draw       )              
{
/*
    struct {  int  idx ;
              int  ptr ;  } scan[100] ;
              int           n_scan ;
              int           n ;
*/
              int           draw_cnt ;

#define  S    scan[n_scan]

/*------------------------------------------------- Входной контроль */

        if(points[point_num].spot_list==NULL)  return(0) ;

/*------------------------------- Обработка случая отсутствия ужатия */

                        draw[0] =points[point_num].spot_list[0] ;
                        draw[1] =points[point_num].spot_list[1] ;
                        draw[2] =points[point_num].spot_list[2] ;
                        draw[3] =points[point_num].spot_list[3] ;
                        draw_cnt=4 ;

/*-------------------------------------------------------------------*/

#undef   S

   return(draw_cnt) ;
}


/*********************************************************************/
/*                                                                   */
/*                  Построение полигона заливки                      */

   void  UDi_area_2D_p_area(UD_data_2Dpoint *spots,
                                        int *index, 
                                        int  index_cnt,
                                        int  point_num,
                                 UD_point_w *polygon   )              
{
       int  i ;

/*------------------------------------------------- Входной контроль */

        if(index_cnt<1)  return ;

/*-------------------------------- Построение списка координат точек */

     for(i=0 ; i<index_cnt ; i++) {
                                     polygon[i].x=spots[index[i]].x ;
                                     polygon[i].y=spots[index[i]].y ;
                                  }
/*-------------------------------------------------------------------*/
}


/*********************************************************************/
/*                                                                   */
/*                 Поиск кратчайших циклов графа                     */

   int  UDi_area_2D_graphloops(int  n_base, UD_area_2Dpoint *points, 
                                                        int *nodes)

{
  typedef struct {
                    int  branch ;   /* Код ветви */
                    int  step ;     /* Номер шага */
                    int  node ;     /* Индекс узла */
                    int  link ;     /* Ссылка на предыдущий шаг */
                    int  prev ;     /* Ссылка на предыдущий узел */
                 } Graph_branch ;

  Graph_branch  branches[1024] ;
           int  branches_cnt ;
           int  loop_flag ;
           int  step ;
           int  nodes_cnt ;
           int  tmp[100] ;
           int  i ;
           int  j ;
           int  k ;
           int  m ;

/*---------------------------------------------------- Инициализация */

                nodes_cnt =0 ;
                 loop_flag=0 ;

/*----------------------------------------- Построение базиса ветвей */

    for(i=0 ; i<points[n_base].link_cnt ; i++) {

                 branches[i].node  =points[n_base].link_list[i] ;
                 branches[i].link  =-1 ;
                 branches[i].prev  =n_base ;
                 branches[i].branch= i ;
                 branches[i].step  = 0 ;
                                               }

                         branches_cnt=i ;

/*--------------------------------- Построение ветвей до пересечения */

    for(step=0 ; ; step++) {                                        /* CIRCLE.1 */
/*- - - - - - - - - - - - - - - - - - - - - - - - Наращивание ветвей */
     for(i=branches_cnt-1 ; i>=0 ; i--) {

               if(branches[i].step!=step)  break ;

                           k=branches[i].node ;  
        for(j=0 ; j<points[k].link_cnt ; j++) {

           if(points[k].link_list[j]==branches[i].prev)  continue ;

               branches[branches_cnt].node  =  points[k].link_list[j] ;
               branches[branches_cnt].link  =         i ;
               branches[branches_cnt].branch=branches[i].branch ;
               branches[branches_cnt].step  = step+1 ;
                        branches_cnt++ ;  
                                              }
                                        }
/*- - - - - - - - - - - - - - - - - - -  Контроль пересечения ветвей */
     for(i=branches_cnt-1 ; i>=0 ; i--) {                           /* CIRCLE.2 - Перебор ветвей */

               if(branches[i].step!=step+1)  break ;                /* Анализируем только "верхние" узлы */
    
        for(j=branches_cnt-1 ; j>=0 ; j--) {

           if(branches[j].step<step)  break ;                       /* Анализируем на глубины двух последних слоев */

           if(branches[j].branch>=branches[i].branch)  continue ;   /* Во избежание задваивания берем только */
                                                                    /*  ветви с меньшими номерами...         */
           if(branches[j].node==branches[i].node) {                 /* IF.1 - Если найден цикл */

                         loop_flag=1 ;

            for(k=branches[i].link, m=branches[i].step-1 ; ;        /* Извлекаем первую полуветвь */
                k=branches[k].link, m--                      ) {
                         tmp[m]=branches[k].node ;
                    if(branches[k].prev==n_base)  break ;
                                                               }

            for(k=j,                m=branches[i].step ; ;          /* Присоединяем вторую полуветвь */
                k=branches[k].link, m++                    ) {
                         tmp[m]=branches[k].node ;
                    if(branches[k].prev==n_base)  break ;
                                                             }

                         nodes[nodes_cnt]=n_base ;                  /* Начинаем цикл базовой точкой */
                               nodes_cnt++ ;

                          k=branches[j].step+branches[i].step+1 ;
                 memcpy(&nodes[nodes_cnt], tmp, k*sizeof(int)) ;    /* Добавляем соединенные полуветви */
                               nodes_cnt+=k ;

                         nodes[nodes_cnt]=n_base ;                  /* Заканчиваем цикл базовой точкой */
                               nodes_cnt++ ;
                                                  }                 /* END.1 */
                                           }

                                        }                           /* CONTINUE.2 */ 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                               if(loop_flag)  break ;
                           }                                        /* CONTINUE.1 */ 
/*-------------------------------------------------------------------*/

  return(nodes_cnt) ;
}


/*********************************************************************/
/*                                                                   */
/*                  Рассчет положения сканера в окне                 */

   void  UDi_a2Dscan_position(UD_metrics *metrics,
                                     int  flag,  
                                     int *x, 
                                     int *y,
                                  double *x_value, 
                                  double *y_value )

{

/*-------------------------------------------------- Подготовка окна */

        UDg_setviewport(NULL, metrics->draw_x_base,                 /* Уст.рабочее окно */
                              metrics->draw_y_base, 
                              metrics->draw_x_size,
                              metrics->draw_y_size ) ; 

	  UDg_setwindow(NULL, 1, metrics->x_base_min,               /* Масштабируем рабочее окно */
                                 metrics->y_base_min,       
                                 metrics->x_base_max, 
                                 metrics->y_base_max ) ;              
            
/*-------------------------- Определяем оконные/растровые координаты */

   if(flag) {

      *x_value=UDg_xcoord_w(NULL, *x-metrics->draw_x_base, NULL, NULL) ;
      *y_value=UDg_ycoord_w(NULL, *y-metrics->draw_y_base, NULL, NULL) ;
            }
   else     {
              *x=UDg_xpos_w(NULL, *x_value)+metrics->draw_x_base ;
              *y=UDg_ypos_w(NULL, *y_value)+metrics->draw_y_base ;
            }
/*-------------------------------------------- Восстановление экрана */

    UDg_setviewport(NULL, 0, 0, metrics->x_size, metrics->y_size) ; /* Восстановление экранной зоны */

/*-------------------------------------------------------------------*/

}


/*********************************************************************/
/*                                                                   */
/*             Построение списка точек сечения на сетке              */

   void  UDi_area_2D_prof_pnts (UD_area_data *view_decl, 
                                  UD_profile *profile   )

{
  UD_area_2Dpoint *points ;
              int  points_cnt ;
              int  xy_flag ;
           double  k1 ;
           double  k2 ;
           double  a1 ;
           double  a2 ;
           double  c1 ;
           double  c2 ;
           double  x ;
           double  y ;
           double  coef_1 ;
              int  prof_pnts_max ;
              int  i ;
              int  j ;

#define    P   points

/*------------------------------------------------------- Подготовка */

    if(fabs(profile->x_2-profile->x_1) >
       fabs(profile->y_2-profile->y_1)  ) {
                                             xy_flag=1  ;

         k1=(profile->y_2-profile->y_1)/(profile->x_2-profile->x_1) ;
         a1= profile->y_2-profile->x_2*k1 ;
                                          }
    else                                  {
                                             xy_flag=0  ;

         k1=(profile->x_2-profile->x_1)/(profile->y_2-profile->y_1) ;
         a1= profile->x_2-profile->y_2*k1 ;
                                          }
/*---------------------------------------- Определение точек сечения */

                    points    =view_decl->points ;
                    points_cnt=view_decl->points_cnt ;

                 prof_pnts_max= 0 ;
           profile->points_cnt= 0 ;
           profile->points    =NULL ;

   for(i=0 ; i<points_cnt ; i++) {                                  /* CIRCLE.1 - Перебираем ячейки */

       if(xy_flag)  c1=P[i].y-P[i].x*k1 ;
       else         c1=P[i].x-P[i].y*k1 ;

     for(j=0 ; j<P[i].link_cnt ; j++) {                             /* CIRCLE.2 - Перебираем связи */    

       if(i<P[i].link_list[j])  continue ;
/*- - - - - - - - - - - - Проверяем пересечение связью линии сечения */
       if(xy_flag)  c2=P[P[i].link_list[j]].y-P[P[i].link_list[j]].x*k1 ;
       else         c2=P[P[i].link_list[j]].x-P[P[i].link_list[j]].y*k1 ;
        
          if((c1>a1 && c2>a1) ||                                    /* Если точки по одну сторону сечения... */
             (c1<a1 && c2<a1)   )  continue ;
/*- - - - - - - - - - - - - - - - - - - Определяем точку пересечения */
       if(xy_flag) {                
         if(P[P[i].link_list[j]].x-P[i].x!=0) {
              k2=(P[P[i].link_list[j]].y-P[i].y)/(P[P[i].link_list[j]].x-P[i].x) ;
              a2= P[i].y-P[i].x*k2 ;
              x =-(a1-a2)/(k1-k2) ;
                                              }
         else                                 {
              x = P[i].x ;
                                              }

              y = a1+k1*x ;

         if((x<profile->x_1 && x<profile->x_2) ||                   /* Не попадаем в отрезок сечения */
            (x>profile->x_1 && x>profile->x_2)   )  continue ;
                   }
       else        {
         if(P[P[i].link_list[j]].y-P[i].y!=0) {
              k2=(P[P[i].link_list[j]].x-P[i].x)/(P[P[i].link_list[j]].y-P[i].y) ;
              a2= P[i].x-P[i].y*k2 ;
              y =-(a1-a2)/(k1-k2) ;
                                              }
         else                                 {
              y = P[i].y ;
                                              }

              x = a1+k1*y ;

         if((y<profile->y_1 && y<profile->y_2) ||                   /* Не попадаем в отрезок сечения */
            (y>profile->y_1 && y>profile->y_2)   )  continue ;
                   }
/*- - - - - - - - - - - - - - - - - - - - - - - - - Выделение памяти */
       if(profile->points_cnt>=prof_pnts_max) {

             prof_pnts_max+=sqrt((double)points_cnt)*2 ;
            profile->points=(struct UD_profile_point *)
                              realloc(profile->points,
                                       prof_pnts_max*sizeof(profile->points[0])) ;
         if(profile->points==NULL) {
                                      profile->points_cnt=0 ;
                                          return ;
                                   } 
                                              }
/*- - - - - - - - - - - - - - - - - - -  Занесение точки пересечения */
                 coef_1=1-hypot(                     x-P[i].x, 
                                                     y-P[i].y )/
                          hypot(P[P[i].link_list[j]].x-P[i].x, 
                                P[P[i].link_list[j]].y-P[i].y ) ;

           profile->points[profile->points_cnt].x      =  x ;
           profile->points[profile->points_cnt].y      =  y ;
           profile->points[profile->points_cnt].index_1=  i ;
           profile->points[profile->points_cnt].index_2=P[i].link_list[j] ;
           profile->points[profile->points_cnt].coef_1 =  coef_1 ;
                           profile->points_cnt++ ; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                      }                             /* CONTINUE.2 */
                                 }                                  /* CONTINUE.1 */
/*-------------------------------------------------------------------*/

#undef    P

}

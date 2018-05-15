/********************************************************************/
/*								    */
/*		МОДУЛЬ УПРАВЛЕНИЯ КОМПОНЕНТОМ "ПОИСКОВАЯ РЛС" 	    */
/*								    */
/*                   Диалоговые процедуры                           */
/*                                                                  */
/********************************************************************/

#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <math.h>

#include "..\Emi_root\controls.h"
#include "..\Matrix\Matrix.h"

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Unit\RSS_Unit.h"
#include "..\RSS_Kernel\RSS_Kernel.h"
#include "..\RSS_Model\RSS_Model.h"

#include "U_SearchRadar.h"

#pragma warning(disable : 4996)

#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text) ;

#include "resource.h"


   void *Ptr_decode(char *text) ;
   char *Ptr_incode(void *ptr ) ;


/*********************************************************************/
/*								     */
/* 	     Обработчик сообщений диалогового окна HELP	             */

    BOOL CALLBACK  Unit_SearchRadar_Help_dialog(  HWND hDlg,     UINT Msg, 
 		                                WPARAM wParam, LPARAM lParam) 
{
  RSS_Module_SearchRadar  Module ;
                     int  elm ;         /* Идентификатор элемента диалога */
                     int  status ;
                     int  index ;
                     int  insert_flag ;
                    char *help ;
                    char  text[512] ;
                    char *end ;
                     int  i ;

/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
   for(i=0 ; Module.mInstrList[i].name_full!=NULL ; i++) {

     if(Module.mInstrList[i].help_row==NULL)  continue ;

         index=LB_ADD_ROW (IDC_HELP_LIST, Module.mInstrList[i].help_row) ;
               LB_SET_ITEM(IDC_HELP_LIST, index, Module.mInstrList[i].help_full) ;
                                                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}

/*------------------------------------------------ Отработка событий */

    case WM_COMMAND:    {

	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - - - - - - - - -  Раскрытие справки */
       if(elm==IDC_HELP_LIST) {

        if(status==LBN_DBLCLK) {

             index=        LB_GET_ROW (elm) ;
              help=(char *)LB_GET_ITEM(elm) ;
                           LB_GET_TEXT(elm, index, text) ;
           if(text[0]!='#' || 
              help   ==NULL  )  return(FALSE) ;

               status=LB_GET_TEXT(elm, index+1, text) ;
           if( status==LB_ERR ||
              text[0]==  '#'    )  insert_flag=1 ;
           else                    insert_flag=0 ;

         do {
                   memset(text, 0, sizeof(text)) ;
                  strncpy(text, help, sizeof(text)-1) ;
               end=strchr(text, '\n') ;
           if( end!=NULL)  *end=0 ;

           if(insert_flag) {  LB_INS_ROW(elm, index+1, text) ;
                                              index++ ;        }
              help=strchr(help, '\n') ;
           if(help==NULL)  break ;
              help++ ;
              
            } while(1) ;
                               }

			            return(FALSE) ;
                              }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			  return(FALSE) ;
			     break ;
			}
/*--------------------------------------------------------- Закрытие */

    case WM_CLOSE:      {
                            EndDialog(hDlg, 0) ;
  			       return(FALSE) ;
			              break ;
			}
/*----------------------------------------------------------- Прочее */

    default :        {
			  return(FALSE) ;
			    break ;
		     }
/*-------------------------------------------------------------------*/
	      }
/*-------------------------------------------------------------------*/

    return(TRUE) ;
}


/*********************************************************************/
/*								     */
/* 	     Обработчик сообщений диалогового окна ROUND             */

 typedef struct {  RSS_Unit_SearchRadar *data ;
                                   HWND  hDlg ; }  Indicator_context ;


#define  _IND_MAX  10

   BOOL CALLBACK  Unit_SearchRadar_Show_dialog(  HWND  hDlg,     UINT  Msg, 
                                               WPARAM  wParam, LPARAM  lParam) 
{
  static Indicator_context   contexts[_IND_MAX] ;
      RSS_Unit_SearchRadar  *context ;
                      HWND   hElem  ;
                       int   elm ;           /* Идентификатор элемента диалога */
                       int   status ;
                      char   title[1024] ;
                       int   i ;

/*-------------------------------------------- Определение контекста */

                                    context=NULL ;
    for(i=0 ; i<_IND_MAX ; i++) 
      if(contexts[i].hDlg==hDlg) {  context=contexts[i].data ;
                                               break ;           }

/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {

                 context=(RSS_Unit_SearchRadar *)lParam ;
/*- - - - - - - - - - - - - - - - - -  Контроль повторного контекста */
           for(i=0 ; i<_IND_MAX ; i++) 
             if(contexts[i].data==context) {
                            EndDialog(contexts[i].hDlg, 0) ;
                                      contexts[i].hDlg=NULL ;
                                           }
/*- - - - - - - - - - - - - - - - - - - - - -  Регистрация контекста */
           for(i=0 ; i<_IND_MAX ; i++) 
             if(contexts[i].hDlg==NULL)  break ;

                contexts[i].hDlg=  hDlg ;
                contexts[i].data=context ;
/*- - - - - - - - - - - - - - - - - - - - - - - -  Пропись заголовка */
              sprintf(title, "Индикатор обстановки: %s.%s",
                                  context->Owner->Name, context->Name) ;
          SendMessage(hDlg, WM_SETTEXT, 0, (LPARAM)title) ;
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
                      hElem=GetDlgItem(hDlg, IDC_INDICATOR) ;
          SendMessage(hElem, WM_SETTEXT, NULL, 
                       (LPARAM)Ptr_incode((void *)context)) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}
/*-------------------------------------------- Отработка перерисовки */

    case WM_PAINT:      {

                      hElem=GetDlgItem(hDlg, IDC_INDICATOR) ;
          SendMessage(hElem, WM_PAINT, NULL, NULL) ;
                          
 			    return(FALSE) ;
			       break ;
			}
/*------------------------------------------------ Отработка событий */

    case WM_COMMAND:    {
                            status=HIWORD(wParam) ;
                               elm=LOWORD(wParam) ;

			  return(FALSE) ;
			     break ;
			}
/*--------------------------------------------------------- Закрытие */

    case WM_CLOSE:      {

         for(i=0 ; i<_IND_MAX ; i++)                                /* Убираем запись об индикаторе */
           if(contexts[i].hDlg==hDlg) {  contexts[i].hDlg=NULL ;    /*   из списка контекстов       */
                                                 break ;         }

                             context->hWnd=NULL ;

                            EndDialog(hDlg, 0) ;
  			       return(FALSE) ;
			              break ;
			}
/*----------------------------------------------------------- Прочее */

    default :        {
			  return(FALSE) ;
			    break ;
		     }
/*-------------------------------------------------------------------*/
	      }
/*-------------------------------------------------------------------*/

    return(TRUE) ;
}


/*********************************************************************/
/*                                                                   */
/*            Элемент - индикатор круговых диаграмм                  */

 LRESULT CALLBACK  Unit_SearchRadar_Indicator_prc(
                                        HWND  hWnd,     UINT  Msg,
 			              WPARAM  wParam, LPARAM  lParam
                                                 )
{
                  HDC  hDC_wnd ;
                  HDC  hDC ;
              HBITMAP  hBM ;
             COLORREF  color ;
          PAINTSTRUCT  PaintCfg ;
                 HPEN  Pen ;
                 HPEN  Pen_prv ;
               HBRUSH  Brush ;
               HBRUSH  Brush_prv ;
                 RECT  Rect ;
 RSS_Unit_SearchRadar *data ;               /* Описание индикатора */
                 char  data_ptr[32] ;       /* Адрес описания, кодированный */
             Matrix2d  Sum_Matrix ;
             Matrix2d  Oper_Matrix ;
             Matrix2d  Point ;
           RSS_Object *target ;
           RSS_Object *center ;
                  int  rad ;
                  int  x_c ;
                  int  y_c ;
                  int  x_t ;
                  int  y_t ;
               double  angle ;
               double  dx, dz ;
                  int  i ;

#define  _PI  3.1415926

/*------------------------------------------------ Извлечение данных */

   if(Msg==WM_PAINT        ||
      Msg==WM_LBUTTONDBLCLK  ) {
  
        SendMessage(hWnd, WM_GETTEXT, (WPARAM)sizeof(data_ptr),
                                      (LPARAM)       data_ptr  ) ;

           data=(RSS_Unit_SearchRadar *)Ptr_decode(data_ptr) ;
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
/*- - - - - - - - - - - - - - - - - -  Создание "теневого" контекста */
       hDC_wnd=BeginPaint(hWnd, &PaintCfg) ;

           hDC=CreateCompatibleDC    (hDC_wnd) ; 
           hBM=CreateCompatibleBitmap(hDC_wnd, GetDeviceCaps(hDC_wnd, HORZRES), 
                                               GetDeviceCaps(hDC_wnd, VERTRES) ) ; 
                         SelectObject(hDC, hBM) ;
/*- - - - - - - - - - - - - - - - - - - - - Форматирование диаграммы */
               GetClientRect(hWnd, &Rect) ;

                 x_c=(Rect.left+Rect.right)/2 ;
                 y_c=(Rect.bottom+Rect.top)/2 ;
                 rad=(Rect.bottom-Rect.top)/2-10 ;
/*- - - - - - - - - - - - - - - - - - - - - - - -  Очистка поля окна */
             Brush    =    CreateSolidBrush(RGB(192, 192, 192)) ;
             Brush_prv=(HBRUSH)SelectObject(hDC, Brush) ;

                                   FillRect(hDC, &Rect, Brush) ;

                               SelectObject(hDC, Brush_prv) ;
                               DeleteObject(Brush) ;
/*- - - - - - - - - - - - - - - - - - - -  Форматирование циферблата */
               Pen    =           CreatePen(PS_SOLID, 0, 0) ;
               Pen_prv=(HPEN)  SelectObject(hDC, Pen) ;

                         MoveToEx(hDC, x_c-2, y_c,   NULL) ;
                           LineTo(hDC, x_c+3, y_c  ) ;
                         MoveToEx(hDC, x_c,   y_c-2, NULL) ;
                           LineTo(hDC, x_c,   y_c+3) ;

                         MoveToEx(hDC, x_c, y_c+rad+1, NULL) ;
                           LineTo(hDC, x_c, y_c+rad+6) ;
                         MoveToEx(hDC, x_c, y_c-rad-1, NULL) ;
                           LineTo(hDC, x_c, y_c-rad-6) ;

                         MoveToEx(hDC, x_c+rad+1, y_c, NULL) ;
                           LineTo(hDC, x_c+rad+6, y_c) ;
                         MoveToEx(hDC, x_c-rad-1, y_c, NULL) ;
                           LineTo(hDC, x_c-rad-6, y_c) ;

#pragma warning(disable : 4244)
               for(i=0 ; i<36 ; i++) {
                                                      angle=_PI*i/18. ;
                        SetPixelV(hDC, x_c+(rad+2)*cos(angle), 
                                       y_c+(rad+2)*sin(angle), 0) ;
                                     }
#pragma warning(default : 4244)
/*- - - - - - - - - - - - - - - - - - - Формирование диаграммы угроз */

                  center=data->Owner ;

                    Sum_Matrix.LoadE      (4, 4) ;
//                 Oper_Matrix.Load4d_roll(center->a_roll) ;
//                  Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
//                 Oper_Matrix.Load4d_elev(center->a_elev) ;
//                  Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
                   Oper_Matrix.Load4d_azim( center->a_azim) ;
                    Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
                   Oper_Matrix.Load4d_base(-center->x_base, -center->y_base, -center->z_base) ;
                    Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;

           for(i=0 ; i<data->threats_cnt ; i++) {

                  target=data->threats[i] ;

                      Point.LoadZero(4, 1) ;
                      Point.SetCell (0, 0, target->x_base) ;
                      Point.SetCell (1, 0, target->y_base) ;
                      Point.SetCell (2, 0, target->z_base) ;
                      Point.SetCell (3, 0,   1   ) ;

                      Point.LoadMul (&Sum_Matrix, &Point) ;         /* Рассчитываем координаты точки относительно носителя */

                 dx=Point.GetCell (0, 0) ;
                 dz=Point.GetCell (2, 0) ;

                x_t=x_c-dx*rad/data->range_max ;
                y_t=y_c-dz*rad/data->range_max ;

                         color    =                 RGB(255, 0, 0) ;
                           Pen    =           CreatePen(PS_SOLID, 0, color) ;
                           Pen_prv=(HPEN)  SelectObject(hDC, Pen) ;
                         Brush    =    CreateSolidBrush(color) ;
                         Brush_prv=(HBRUSH)SelectObject(hDC, Brush) ;

#pragma warning(disable : 4244)
                                  Rectangle(hDC, x_t-1, y_t-1, x_t+1, y_t+1) ;
#pragma warning(default : 4244)

                               SelectObject(hDC, Brush_prv) ;
                               SelectObject(hDC, Pen_prv) ;
                               DeleteObject(Brush) ;
                               DeleteObject(Pen) ;
                                                }
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Вывод диаграммы */
            BitBlt(hDC_wnd, 
                   Rect.left, Rect.top, 
                   Rect.right-Rect.left, Rect.bottom-Rect.top, 
                   hDC,
                   Rect.left, Rect.top, 
                   SRCCOPY                           ) ;

                    ValidateRect(hWnd,  NULL) ;
                        EndPaint(hWnd, &PaintCfg) ;
/*- - - - - - - - - - - - - - - - - - - - - -  Освобождение ресурсов */
            if(hBM!=NULL)  DeleteObject(hBM) ;
            if(hDC!=NULL)  DeleteDC    (hDC) ;
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
/*                Преобразование адреса данных в строку              */
/*                Преобразование строки в адрес данных               */

#define  _UD_PTR_PREFIX   "UD_Pointer:"

  union {
          unsigned  long  code ;
                    void *ptr ;  } Ptr_cvt ;


   char *Ptr_incode(void *ptr)

{
  static char  ptr_text[128] ;
         char  tmp[32] ;


              Ptr_cvt.ptr=ptr ;
        ultoa(Ptr_cvt.code, tmp, 16) ;

       strcpy(ptr_text, _UD_PTR_PREFIX) ;
       strcat(ptr_text,     tmp       ) ;

   return(ptr_text) ;
}


   void *Ptr_decode(char *text)

{
  char *end ;


        end=strstr(text, _UD_PTR_PREFIX) ;
     if(end==NULL)  return(NULL) ;

        end=text+strlen(_UD_PTR_PREFIX) ;

          Ptr_cvt.code=strtoul(end, &end, 16) ;

   return(Ptr_cvt.ptr) ;
}



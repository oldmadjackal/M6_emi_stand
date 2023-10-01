/********************************************************************/
/*								    */
/*		МОДУЛЬ ЗАДАЧИ "СОЗДАНИЕ ЛАНДШАФТА"    		    */
/*								    */
/*                   Диалоговые процедуры                           */
/*                                                                  */
/********************************************************************/

#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <math.h>

#include "..\Emi_root\controls.h"

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Kernel\RSS_Kernel.h"

#include "T_Land.h"

#pragma warning(disable : 4996)


#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text) ;

#define  CREATE_DIALOG  CreateDialogIndirectParam

#include "resource.h"

   void *Ptr_decode(char *text) ;
   char *Ptr_incode(void *ptr ) ;

/*--------------------------------------------------- Форматы данных */

#define    _FRAME_FORMAT  "% 6.2lf(% 5.2lf) %-10.10s %s"
#define   _OBJECT_FORMAT  "%s"

/*------------------------------------ Обработчики элементов диалога */

  union WndProc_par {
                     LONG_PTR            par ;
                      LRESULT (CALLBACK *call)(HWND, UINT, WPARAM, LPARAM) ; 
                    } ;

  static union WndProc_par  Scenario_WndProc ;
  static union WndProc_par  Tmp_WndProc ;

   LRESULT CALLBACK  Task_Land_Scenario_WndProc(HWND, UINT, WPARAM, LPARAM) ;

/*---------------------------------------------------- Прототипы п/п */


/*********************************************************************/
/*								     */
/* 	     Обработчик сообщений диалогового окна HELP	             */

  INT_PTR CALLBACK  Task_Land_Help_dialog(  HWND hDlg,     UINT Msg, 
                                          WPARAM wParam, LPARAM lParam) 
{
     RSS_Module_Land  Module ;
                 int  elm ;         /* Идентификатор элемента диалога */
             LRESULT  status ;
             LRESULT  index ;
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
/* 	     Обработчик сообщений диалогового окна MAP	             */

  INT_PTR CALLBACK  Task_Land_Map_dialog(  HWND hDlg,     UINT Msg, 
                                         WPARAM wParam, LPARAM lParam) 
{
                HWND  hElem  ;
                 int  elm ;         /* Идентификатор элемента диалога */
                 int  status ;

/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
                      hElem=GetDlgItem(hDlg, IDC_MAP) ;
          SendMessage(hElem, WM_SETTEXT, NULL, 
                       (LPARAM)Ptr_incode((void *)lParam)) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}
/*-------------------------------------------- Отработка перерисовки */

    case WM_PAINT:      {

                      hElem=GetDlgItem(hDlg, IDC_MAP) ;
          SendMessage(hElem, WM_PAINT, NULL, NULL) ;
                          
 			    return(FALSE) ;
			       break ;
			}
/*------------------------------------------------ Отработка событий */

    case WM_COMMAND:    {

	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;
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
/* 	     Обработчик сообщений диалогового окна EDIT	             */

  INT_PTR CALLBACK  Task_Land_View_dialog(  HWND hDlg,     UINT Msg, 
                                          WPARAM wParam, LPARAM lParam) 
{
     static RSS_Module_Land *Module ;
              static  HFONT  font ;        /* Шрифт */
                        int  elm ;         /* Идентификатор элемента диалога */
                        int  status ;
                    LRESULT  index ;
                       char  text[512] ;
                        int  i ;

/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {
                             Module=(RSS_Module_Land *)lParam ;
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Пропись шрифтов */
        if(font==NULL)
           font=CreateFont(14, 0, 0, 0, FW_THIN, 
                                 false, false, false,
                                  ANSI_CHARSET,
                                   OUT_DEFAULT_PRECIS,
                                    CLIP_DEFAULT_PRECIS,
                                     DEFAULT_QUALITY,
                                      VARIABLE_PITCH,
                                       "Courier New Cyr") ;
         SendMessage(ITEM(IDC_SCENARIO), WM_SETFONT, (WPARAM)font, 0) ;
         SendMessage(ITEM(IDC_OBJECTS),  WM_SETFONT, (WPARAM)font, 0) ;
/*- - - - - - - - - - - - - - - - - - - - - Выдача "кадров" сценария */
                       LB_CLEAR(IDC_SCENARIO) ;

        for(i=0 ; i<Module->mScenario_cnt ; i++) {

                  sprintf(text, _FRAME_FORMAT, Module->mScenario[i].t,
                                               Module->mScenario[i].dt,
                                               Module->mScenario[i].action,
                                               Module->mScenario[i].command ) ;
               LB_ADD_ROW(IDC_SCENARIO, text) ;
                                                 }
/*- - - - - - - - - - - - - - - - - - - - - - Выдача списка объектов */
                       LB_CLEAR(IDC_OBJECTS) ;

        for(i=0 ; i<Module->mObjects_cnt ; i++) {

                  sprintf(text, _OBJECT_FORMAT, Module->mObjects[i].object->Name) ;
               LB_ADD_ROW(IDC_OBJECTS, text) ;
                                                }
/*- - - - - - - - - - - - - - - - -  Перехват обработчиков сообщений */
                Tmp_WndProc.call=Task_Land_Scenario_WndProc ;
           Scenario_WndProc.par =GetWindowLongPtr(ITEM(IDC_SCENARIO), GWLP_WNDPROC) ;
                                 SetWindowLongPtr(ITEM(IDC_SCENARIO), GWLP_WNDPROC, Tmp_WndProc.par) ;
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}
/*------------------------------------------------ Отработка событий */

    case WM_COMMAND:    {

	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - Раскрытие строки */
       if(   elm==IDC_SCENARIO &&
          status==LBN_DBLCLK     ) {

                     index=LB_GET_ROW (elm) ;
                           LB_GET_TEXT(elm, index, text) ;

			              return(FALSE) ;
                                   }
/*- - - - - - - - - - - - - - - - - - - - DEBUG: "Отработать" строку */
       if(elm==IDC_STEP) {

            index=LB_GET_ROW(IDC_SCENARIO) ;                        /* Опр.индекс "кадра" сценария */
         if(index==LB_ERR              ) {
              SEND_ERROR("Секция LAND: Выбирете исполняемый кадр") ;
                                            return(FALSE) ;
                                         }

                      LB_GET_TEXT(IDC_SCENARIO, index, text) ;

         if(text[0]==0) {
              SEND_ERROR("Секция LAND: Сценарий полностью исполнен") ;
                                            return(FALSE) ;
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
/*        Обработчик сообщений диалогового окна VARIABLES            */

  INT_PTR CALLBACK  Task_Land_Vars_dialog(  HWND hDlg,     UINT Msg, 
                                          WPARAM wParam, LPARAM lParam) 
{
     RSS_Module_Land  Module ;
       static  HFONT  font ;        /* Шрифт */
                 int  elm ;         /* Идентификатор элемента диалога */
                 int  status ;
                char  record[1024] ;
                char  iface_signal[1024] ;
                char  iface_data[1024] ;
                char  tmp[1024] ;
                 int  i ;

#define  VARS        Module.mVariables
#define  VARS_CNT    Module.mVariables_cnt

/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Пропись шрифтов */
        if(font==NULL)
           font=CreateFont(14, 0, 0, 0, FW_THIN, 
                                 false, false, false,
                                  ANSI_CHARSET,
                                   OUT_DEFAULT_PRECIS,
                                    CLIP_DEFAULT_PRECIS,
                                     DEFAULT_QUALITY,
                                      VARIABLE_PITCH,
                                       "Courier New Cyr") ;
         SendMessage(ITEM(IDC_VARS_LIST), WM_SETFONT, (WPARAM)font, 0) ;
         SendMessage(ITEM(IDC_EXEC_LIST), WM_SETFONT, (WPARAM)font, 0) ;
/*- - - - - - - - - - - - - - - - -  Инициализация списка переменных */
                      LB_CLEAR(IDC_VARS_LIST) ;

         for(i=0 ; i<VARS_CNT ; i++) {
                       sprintf(tmp, "%10.10s = %s", VARS[i].name, 
                                                    VARS[i].value) ;
                    LB_ADD_ROW(IDC_VARS_LIST, tmp) ;
                                     }
/*- - - - - - - - - - - - - - - - - Инициализация списка результатов */
/*
            Module.mExecIFace.vDecode(iface_signal, iface_data) ;

          LB_CLEAR(IDC_EXEC_LIST) ;
           sprintf(tmp, "%10.10s = %s:%s",  "@Buff",  iface_signal, iface_data) ;
        LB_ADD_ROW(IDC_EXEC_LIST, tmp) ;
           sprintf(tmp, "%10.10s = %.10lf", "@Value", Module.mExecValue) ;
        LB_ADD_ROW(IDC_EXEC_LIST, tmp) ;
           sprintf(tmp, "%10.10s = %d",     "@Error", Module.mExecError) ;
        LB_ADD_ROW(IDC_EXEC_LIST, tmp) ;
           sprintf(tmp, "%10.10s = %d",     "@Fail",  Module.mExecFail) ;
        LB_ADD_ROW(IDC_EXEC_LIST, tmp) ;
*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}
/*------------------------------------ Отработка внутренних сообений */

    case WM_USER:  {
/*- - - - - - - - - - - - - - - - - - Обновление данных о переменных */
        if(wParam==_USER_SHOW) {

         for(i=0 ; i<VARS_CNT ; i++) {                              /* Обновление списка переменных */
                       sprintf(tmp, "%10.10s = %s", VARS[i].name, 
                                                    VARS[i].value) ;

                        memset(record, 0, sizeof(record)) ;
                   LB_GET_TEXT(IDC_VARS_LIST, i, record) ;        

               if(       record[0]==0) { 
                                          LB_ADD_ROW(IDC_VARS_LIST, tmp) ;
                                       } 
          else if(strcmp(record, tmp)) {
                                          LB_DEL_ROW(IDC_VARS_LIST, i) ;
                                          LB_INS_ROW(IDC_VARS_LIST, i, tmp) ;
                                       } 
                                     }

//          Module.mExecIFace.vDecode(iface_signal, iface_data) ;   /* Обновление данных интерфейса внутренних связей */

              sprintf(tmp, "%10.10s = %s:%s",  "@Buff",  iface_signal, iface_data) ;
               memset(record, 0, sizeof(record)) ;
          LB_GET_TEXT(IDC_EXEC_LIST, 0, record) ;        

        if(!strcmp(record, tmp))   return(FALSE) ;                  /* Если данные не изменились... */

/*
              LB_CLEAR(IDC_EXEC_LIST) ;
               sprintf(tmp, "%10.10s = %s:%s",  "@Buff",  iface_signal, iface_data) ;
            LB_ADD_ROW(IDC_EXEC_LIST, tmp) ;
               sprintf(tmp, "%10.10s = %.10lf", "@Value", Module.mExecValue) ;
            LB_ADD_ROW(IDC_EXEC_LIST, tmp) ;
               sprintf(tmp, "%10.10s = %d",     "@Error", Module.mExecError) ;
            LB_ADD_ROW(IDC_EXEC_LIST, tmp) ;
               sprintf(tmp, "%10.10s = %d",     "@Fail",  Module.mExecFail) ;
            LB_ADD_ROW(IDC_EXEC_LIST, tmp) ;
*/
                                       return(FALSE) ;
                               }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			  return(FALSE) ;
  			     break ;
  		   }
/*------------------------------------------------ Отработка событий */

    case WM_COMMAND:    {

	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - - - - - Очистить список переменных */
       if(elm==IDC_VARS_CLEAR) {
                                               VARS_CNT=0 ;

                                  LB_CLEAR(IDC_VARS_LIST) ;

			              return(FALSE) ;
                               }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			  return(FALSE) ;
			     break ;
			}
/*--------------------------------------------------------- Закрытие */

    case WM_CLOSE:      {
                            Module.mVariables_Window=NULL ;
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

#undef  VAR
#undef  VAR_CNT

    return(TRUE) ;
}


/*********************************************************************/
/*								     */
/*	          Новыe обработчики элементов ListBox                */    

  LRESULT CALLBACK  Task_Land_Scenario_WndProc(  HWND  hWnd,     UINT  Msg,
                                               WPARAM  wParam, LPARAM  lParam) 
{
    HMENU  hMenu ;
  LRESULT  result ;
      int  menu_flag ;        /* Флаг отображения контекстного меню */
     RECT  Rect ;
      int  x ;
      int  y ;

/*------------------------------------------------------- Подготовка */

/*----------------------------------- Подмена правой кнопки на левую */

                              menu_flag= 0 ;

   if(Msg==WM_RBUTTONDOWN)          Msg=WM_LBUTTONDOWN ;
   if(Msg==WM_RBUTTONUP  ) {        Msg=WM_LBUTTONUP ;
                              menu_flag= 1 ;            }
                          
/*-------------------------------- Вызов штатной процедуры обработки */

      result=CallWindowProc(Scenario_WndProc.call,
                                        hWnd, Msg, wParam, lParam) ;
   if(!menu_flag)  return(result) ;

/*------------------------------------- Обработка меню правой кнопки */

   if(menu_flag) {
                    GetWindowRect(hWnd, &Rect) ;

                         x=LOWORD(lParam) ;
                         y=HIWORD(lParam) ;

                     hMenu=CreatePopupMenu() ;
          AppendMenu(hMenu, MF_STRING, IDC_INSERT_ROW, "Добавить строку") ;
          AppendMenu(hMenu, MF_STRING, IDC_DELETE_ROW, "Удалить строку") ;
          AppendMenu(hMenu, MF_STRING, IDC_SET_COMMENT, "Закомментировать/раскомментировать") ;

      TrackPopupMenu(hMenu, 0, Rect.left+x, Rect.top+y+4,
                                      0, GetParent(hWnd), NULL) ;

                     DestroyMenu(hMenu) ;
                 }
/*-------------------------------------------------------------------*/

  return(result) ;
}


/*********************************************************************/
/*                                                                   */
/*            Элемент - ???                                          */

 LRESULT CALLBACK  Task_Land_Map_prc(
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
      RSS_Module_Land *data ;               /* Источник данных */
                 char  data_ptr[32] ;       /* Адрес описания, кодированный */
                TRACE *trace ;
//         RSS_Object *center ;
               double  x ;
               double  z ;
               double  x_c ;
               double  z_c ;
               double  x_min ;
               double  x_max ;
               double  z_min ;
               double  z_max ;
               double  dx ;
               double  dz ;
               double  step ;
                  int  x_scr ;
                  int  z_scr ;
                  int  x_prv ;
                  int  z_prv ;
                  int  i ;
                  int  j ;

#pragma warning(disable : 4244)

/*------------------------------------------------ Извлечение данных */

   if(Msg==WM_PAINT        ||
      Msg==WM_LBUTTONDBLCLK  ) {
  
        SendMessage(hWnd, WM_GETTEXT, (WPARAM)sizeof(data_ptr),
                                      (LPARAM)       data_ptr  ) ;

           data=(RSS_Module_Land *)Ptr_decode(data_ptr) ;
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
/*- - - - - - - - - - - - - - - - - - - Определение зоны отображения */
                     x_min=-1.0 ;
                     x_max= 1.0 ;
                     z_min=-1.0 ;
                     z_max= 1.0 ;

       for(i=0 ; i<data->mObjects_cnt ; i++) {

         if(data->mObjects[i].object==NULL)  continue ;

                     x=data->mObjects[i].object->state.x ;
                     z=data->mObjects[i].object->state.z ;

         if(i==0) {
                     x_min=x ;
                     x_max=x ;
                     z_min=z ;
                     z_max=z ;
                  }
         else     {
                    if(x<x_min)  x_min=x ;
                    if(x>x_max)  x_max=x ;
                    if(z<z_min)  z_min=z ;
                    if(z>z_max)  z_max=z ;
                  }
                                             }

     if(x_min==x_max) {
                         x_min-=1.0 ;
                         x_max+=1.0 ;
                      }   
     if(z_min==z_max) {
                         z_min-=1.0 ;
                         z_max+=1.0 ;
                      }   

     if(data->mMapRegime & _MAP_KEEP_RANGE) {                       /* Режим сохранения максимального охвата */

           if(x_min<data->mMapXmin)  data->mMapXmin=x_min ;
           else                               x_min=data->mMapXmin ;
           if(x_max>data->mMapXmax)  data->mMapXmax=x_max ;
           else                               x_max=data->mMapXmax ;
           if(z_min<data->mMapZmin)  data->mMapZmin=z_min ;
           else                               z_min=data->mMapZmin ;
           if(z_max>data->mMapZmax)  data->mMapZmax=z_max ;
           else                               z_max=data->mMapZmax ;
                                            } 
/*- - - - - - - - - - - - - - - - - -  Создание "теневого" контекста */
       hDC_wnd=BeginPaint(hWnd, &PaintCfg) ;

           hDC=CreateCompatibleDC    (hDC_wnd) ; 
           hBM=CreateCompatibleBitmap(hDC_wnd, GetDeviceCaps(hDC_wnd, HORZRES), 
                                               GetDeviceCaps(hDC_wnd, VERTRES) ) ; 
                         SelectObject(hDC, hBM) ;
/*- - - - - - - - - - - - - - - - - - - - - Форматирование диаграммы */
               GetClientRect(hWnd, &Rect) ;

            dx=fabs((x_max-x_min)/(Rect.right-Rect.left)) ;
            dz=fabs((z_max-z_min)/(Rect.bottom-Rect.top)) ;

         if(dx<dz) dx =dz ;
                   dx*=1.05 ;

            x_c  =(x_max+x_min)/2. ;
            z_c  =(z_max+z_min)/2. ;
            x_min=x_c-dx*(Rect.right -Rect.left)/2. ;
            x_max=x_c+dx*(Rect.right -Rect.left)/2. ;
            z_min=z_c-dx*(Rect.bottom-Rect.top )/2. ;
            z_max=z_c+dx*(Rect.bottom-Rect.top )/2. ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - -  Рассчёт сетки */
                     x=fabs(x_max-x_min) ;
                     z=fabs(z_max-z_min) ;
            if(x<z)  x=z ;

         for(step=0.01 ; step<100000. ; step*=10.) {
               if(x/(1.*step)<5) {  step=1.*step ;  break ;  }
               if(x/(2.*step)<5) {  step=2.*step ;  break ;  }
               if(x/(5.*step)<5) {  step=5.*step ;  break ;  }
                                                   }
/*- - - - - - - - - - - - - - - - - - - - - - - -  Очистка поля окна */
             Brush    =    CreateSolidBrush(RGB(192, 192, 192)) ;
             Brush_prv=(HBRUSH)SelectObject(hDC, Brush) ;

                                   FillRect(hDC, &Rect, Brush) ;

                               SelectObject(hDC, Brush_prv) ;
                               DeleteObject(Brush) ;
/*- - - - - - - - - - - - - - - - - - - - - - - Форматирование сетки */
               Pen    =           CreatePen(PS_SOLID, 0, RGB(148, 148, 148)) ;
               Pen_prv=(HPEN)  SelectObject(hDC, Pen) ;

       if(x_min<0) 
        for(x=0. ; x>x_min ; x-=step) {  
             MoveToEx(hDC, (x-x_min)/dx, 0,   NULL) ;
               LineTo(hDC, (x-x_min)/dx, Rect.bottom-Rect.top) ;
                                       }

       if(x_max>0) 
        for(x=0. ; x<x_max ; x+=step) {  
             MoveToEx(hDC, (x-x_min)/dx, 0,   NULL) ;
               LineTo(hDC, (x-x_min)/dx, Rect.bottom-Rect.top) ;
                                       }

       if(z_min<0) 
        for(z=0. ; z>z_min ; z-=step) {  
             MoveToEx(hDC, 0,                    (z-z_min)/dx, NULL) ;
               LineTo(hDC, Rect.right-Rect.left, (z-z_min)/dx) ;
                                       }

       if(z_max>0) 
        for(z=0. ; z<z_max ; z+=step) {  
             MoveToEx(hDC, 0,                    (z-z_min)/dx, NULL) ;
               LineTo(hDC, Rect.right-Rect.left, (z-z_min)/dx) ;
                                       }
/*- - - - - - - - - - - - - - - - - -  Отображение объектов на карте */
       for(i=0 ; i<data->mObjects_cnt ; i++) {

         if(data->mObjects[i].object==NULL)  continue ;

                     x_scr=(data->mObjects[i].object->state.x-x_min)/dx ;
                     z_scr=(data->mObjects[i].object->state.z-z_min)/dx ;

                 color=RGB(0, 0, 0) ;

          for(j=0 ; j<data->mMapColors_cnt ; j++)                   /* Если имя или шаблон имени объекта есть в списке расцвети - */
            if(strchr(data->mMapColors[j].object, '#')!=NULL) {     /*  - устанавливаем цвет по нему                              */
                 if( strlen(data->mObjects[i].object->Name)>=
                     strlen(data->mMapColors[j].object    )  )
                  if(!memicmp(data->mMapColors[j].object,
                              data->mObjects[i].object->Name,
                       strlen(data->mMapColors[j].object)-1  )) {  color=data->mMapColors[j].color ;  break ;  }
                                                              }
            else                                              {
                  if(!stricmp(data->mMapColors[j].object,
                              data->mObjects[i].object->Name)) {  color=data->mMapColors[j].color ;  break ;  }
                                                              }

                           Pen    =           CreatePen(PS_SOLID, 0, color) ;
                           Pen_prv=(HPEN)  SelectObject(hDC, Pen) ;
                         Brush    =    CreateSolidBrush(color) ;
                         Brush_prv=(HBRUSH)SelectObject(hDC, Brush) ;

                                    Ellipse(hDC, x_scr-2, z_scr-2, x_scr+2, z_scr+2) ;

                               SelectObject(hDC, Brush_prv) ;
                               SelectObject(hDC, Pen_prv) ;
                               DeleteObject(Brush) ;
                               DeleteObject(Pen) ;
                                                }
/*- - - - - - - - - - - - - - - - - -  Трассировка объектов на карте */
       for(i=0 ; i<data->mObjects_cnt ; i++) {

         if(data->mObjects[i].object==NULL)  continue ;

          for(j=0 ; j<data->mMapTraces_cnt ; j++)
            if(!stricmp(data->mMapTraces[j].object,
                        data->mObjects[i].object->Name))  break ;  

            if(j>=data->mMapTraces_cnt)  continue ;

               trace=&data->mMapTraces[j] ;

            if(trace->points_cnt!=0)                                /* Если объект стоит на месте... */
             if(trace->points[trace->points_cnt-1].x==
                 data->mObjects[i].object->state.x     &&
                trace->points[trace->points_cnt-1].z==
                 data->mObjects[i].object->state.z       )  continue ;

            if(trace->points_cnt==trace->points_max) {
                trace->points_max+=  1000 ;
                trace->points     =(RSS_Point *)
                                     realloc(trace->points, 
                                             trace->points_max*sizeof(*trace->points)) ;
                                                     }

                trace->points[trace->points_cnt].x=data->mObjects[i].object->state.x ;
                trace->points[trace->points_cnt].z=data->mObjects[i].object->state.z ;
                              trace->points_cnt++ ;

                 color=RGB(0, 0, 0) ;

          for(j=0 ; j<data->mMapColors_cnt ; j++)                   /* Если имя или шаблон имени объекта есть в списке расцветки - */
            if(strchr(data->mMapColors[j].object, '#')!=NULL) {     /*  - устанавливаем цвет по нему                               */
                 if( strlen(data->mObjects[i].object->Name)>=
                     strlen(data->mMapColors[j].object    )  )
                  if(!memicmp(data->mMapColors[j].object,
                              data->mObjects[i].object->Name,
                       strlen(data->mMapColors[j].object)-1  )) {  color=data->mMapColors[j].color ;  break ;  }
                                                              }
            else                                              {
                  if(!stricmp(data->mMapColors[j].object,
                              data->mObjects[i].object->Name)) {  color=data->mMapColors[j].color ;  break ;  }
                                                              }

                           Pen    =           CreatePen(PS_SOLID, 0, color) ;
                           Pen_prv=(HPEN)  SelectObject(hDC, Pen) ;


          for(j=0 ; j<trace->points_cnt ; j++) {
  
                       x_scr=(trace->points[j].x-x_min)/dx ;
                       z_scr=(trace->points[j].z-z_min)/dx ;

                    if(j==0           )  MoveToEx(hDC, x_scr, z_scr, NULL) ;
               else if(x_scr!=x_prv ||
                       z_scr!=z_prv   )  LineTo  (hDC, x_scr, z_scr) ;

                       x_prv=x_scr ;
                       z_prv=z_scr ;
                                               }

                               SelectObject(hDC, Pen_prv) ;
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

#pragma warning(default : 4244)
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



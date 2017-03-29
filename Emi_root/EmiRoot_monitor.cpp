/*********************************************************************/
/*                                                                   */
/*         СИСТЕМА ГРАФИЧЕСКОГО 2D-МОДЕЛИРОВАНИЯ МАНИПУЛЯТОРОВ       */
/*                                                                   */
/*                   Процедуры мониторинга                           */
/*                                                                   */
/*********************************************************************/

#include <windows.h>
#include <stdio.h>
#include <io.h>

#include "Controls.h"

//#include "..\RSS_Feature\RSS_Feature.h"
//#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Kernel\RSS_Kernel.h"

#include "EmiRoot.h"

#include "resource.h"

#pragma warning(disable : 4996)


/*--------------------------------------------- Системные переменные */

 extern  RSS_Module_Main  Kernel ;    /* Системное ядро */

#define  SEND_ERROR(text)      SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                           (WPARAM)_USER_ERROR_MESSAGE,      \
                                           (LPARAM) text)
#define  SHOW_COMMAND(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                           (WPARAM)_USER_SHOW_COMMAND,       \
                                           (LPARAM) text)
#define  SHOW_THREAD(text)     SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                           (WPARAM)_USER_THREAD_MESSAGE,     \
                                           (LPARAM) text)

/*-------------------------------------------------- Описание потока */

 typedef  struct {  DWORD  Thread_id ;
                     char  desc[1024] ; }  WorkThread ;


/*********************************************************************/
/*								     */
/* 	                Мониторинг рабочих потоков                   */

  int  EmiRoot_threads(char *action, char *thread_desc)
{
  static  HWND  hMonitor ;
    WorkThread  thread_data ;
          char  text[1024] ;
          char *end ;

/*-------------------------------- Создание монитора рабочих потоков */

     if(!stricmp(action, "CREATE")) {

         hMonitor=CreateDialog(GetModuleHandle(NULL), "IDD_THREADS",
                                  GetActiveWindow(), Main_Threads_dialog) ;
      if(hMonitor==NULL) {
                 sprintf(text, "Thread monitor create error %d", GetLastError()) ;
           EmiRoot_error(text, MB_ICONERROR) ;
	                          return(-1) ;
                         }
                                    }
/*----------------------------------- Показ монитора рабочих потоков */

     if(!stricmp(action, "SHOW")) {

  	          ShowWindow(hMonitor, SW_SHOW) ;
                                  }
/*--------------------------------- Скрытие монитора рабочих потоков */

     if(!stricmp(action, "HIDE")) {

  	          ShowWindow(hMonitor, SW_HIDE) ;
                                  }
/*-------------------------------------- Регистрация рабочего потока */

     if(!stricmp(action, "ADD")) {

                     thread_data.Thread_id=GetCurrentThreadId() ;
             strncpy(thread_data.desc, thread_desc, sizeof(thread_data.desc)) ;

          end=strchr(thread_data.desc, '\r') ;
       if(end!=NULL)  *end=0 ;
          end=strchr(thread_data.desc, '\n') ;
       if(end!=NULL)  *end=0 ;

          SendMessage(hMonitor, WM_USER, (WPARAM)_USER_THREAD_ADD,
                                         (LPARAM)&thread_data) ;
                                 }
/*--------------------------------------- Исключение рабочего потока */

     if(!stricmp(action, "DELETE")) {

                     thread_data.Thread_id=GetCurrentThreadId() ;

          SendMessage(hMonitor, WM_USER, (WPARAM)_USER_THREAD_DELETE,
                                         (LPARAM)&thread_data) ;
                                    }
/*---------------------------------------- Остановка рабочего потока */

     if(!stricmp(action, "STOP")) {
                                       RSS_Kernel::kernel->stop=1 ;
                                  }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/*********************************************************************/
/*								     */
/* 	     Обработчик сообщений диалогового окна THREADS           */

    BOOL CALLBACK  Main_Threads_dialog(  HWND hDlg,     UINT Msg, 
 		                       WPARAM wParam, LPARAM lParam) 
{
    WorkThread *thread_data ;
         DWORD  thread_id ;
           int  elm ;         /* Идентификатор элемента диалога */
           int  status ;
           int  index ;
           int  cnt ;
          char  text[1024] ;

/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}
/*------------------------------------ Отработка внутренних сообений */

    case WM_USER:  {
/*- - - - - - - - - - - - - - - - - - -  Регистрация рабочего потока */
        if(wParam==_USER_THREAD_ADD) {

             thread_data=(WorkThread *)lParam ;
        
         index=LB_ADD_ROW(IDC_THREADS_LIST, thread_data->desc) ;
              LB_SET_ITEM(IDC_THREADS_LIST, index, thread_data->Thread_id) ;

              SHOW_THREAD(thread_data->desc) ;
                                       return(FALSE) ;
                                     }
/*- - - - - - - - - - - - - - - - - - - - Исключение рабочего потока */
        if(wParam==_USER_THREAD_DELETE) {

             thread_data=(WorkThread *)lParam ;
       
                     cnt=LB_GET_COUNT(IDC_THREADS_LIST) ;

           for(index=0 ; index<cnt ; index++) {
                 thread_id=LB_ROW_ITEM(IDC_THREADS_LIST, index) ;
              if(thread_id==thread_data->Thread_id) {
                           LB_DEL_ROW(IDC_THREADS_LIST, index) ;
                                         break ;
                                                    }
                                              }

              if(cnt>1) {
                 LB_GET_TEXT(IDC_THREADS_LIST, cnt-2, text) ;
                        }
              else      {
                                strcpy(text, "") ;
                        }

                           SHOW_THREAD(text) ;

                                            return(FALSE) ;
                                        }
/*- - - - - - - - - - - - - - - - - - - - - - - -  Завершение работы */
        if(wParam==_KEY_QUIT) {
                                   EndDialog(hDlg, 0) ;
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
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			  return(FALSE) ;
			     break ;
			}
/*--------------------------------------------------------- Закрытие */

    case WM_CLOSE:      {
                           EmiRoot_threads("HIDE", NULL) ;
//                               EndDialog(hDlg, 0) ;
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
/* 	     Отображение списка присоединенных модулей               */

  int  EmiRoot_modules(char *action)
{

/*----------------------------------------- Создание окна со списком */

     if(!stricmp(action, "CREATE")) {

          DialogBox(GetModuleHandle(NULL), "IDD_MODULES",
                        GetActiveWindow(), Main_Modules_dialog) ;

                                    }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/*********************************************************************/
/*								     */
/* 	     Обработчик сообщений диалогового окна MODULES           */

    BOOL CALLBACK  Main_Modules_dialog(  HWND hDlg,     UINT Msg, 
 		                       WPARAM wParam, LPARAM lParam) 
{
    static  HFONT  font ;        /* Шрифт */
 class RSS_Kernel *entry ;
              int  elm ;         /* Идентификатор элемента диалога */
              int  status ;
              int  index ;
             char  tmp[1024] ;
              int  i ;

/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {
/*- - - - - - - - - - - - - - - - - - - - - - - - Подготовка шрифтов */
       if(font==NULL)
          font=CreateFont(16, 8, 0, 0, FW_THIN, 
                           false, false, false,
                             ANSI_CHARSET,
                              OUT_DEFAULT_PRECIS,
                               CLIP_DEFAULT_PRECIS,
                                DEFAULT_QUALITY,
                                 VARIABLE_PITCH,
                                  "Courier") ;

               SETFONT(IDC_MODULES_LIST, font) ;
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
     for(i=0 ; i<Kernel.modules_cnt ; i++) {

        Kernel.modules[i].entry->vGetParameter("$$MODULE_NAME", tmp) ;

              index=LB_ADD_ROW (IDC_MODULES_LIST, tmp) ;
                    LB_SET_ITEM(IDC_MODULES_LIST, index, Kernel.modules[i].entry) ;
                                           }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}
/*------------------------------------------------ Отработка событий */

    case WM_COMMAND:    {

	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - - - - Работа с параметрами задания */
       if(   elm==IDC_MODULES_LIST &&
          status==LBN_DBLCLK         ) {
 
              entry=(class RSS_Kernel *)LB_GET_ITEM(elm)  ;
              entry->vExecuteCmd("&?") ;

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



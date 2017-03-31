/*********************************************************************/
/*                                                                   */
/*         СИСТЕМА ГРАФИЧЕСКОГО 2D-МОДЕЛИРОВАНИЯ МАНИПУЛЯТОРОВ       */
/*                                                                   */
/*********************************************************************/

#include <windows.h>
#include <process.h>
#include <malloc.h>
#include <stdio.h>
#include <direct.h>
#include <time.h>

//#include "..\RSS_Feature\RSS_Feature.h"
//#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Kernel\RSS_Kernel.h"

#include "..\Matrix\Matrix.h"
#include "..\UD_tools\UserDlg.h"

#include "EmiRoot_show.h"

#define  __EMI_ROOT_MAIN__
#include "EmiRoot.h"

#include "resource.h"

//#include "Controls.h"

#pragma warning(disable : 4996)


/*--------------------------------------------- Системные переменные */

        RSS_Module_Main  Kernel ;    /* Системное ядро */

#define  SEND_ERROR(text)      SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,        \
                                         (LPARAM) text)
#define  SHOW_COMMAND(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_SHOW_COMMAND,         \
                                         (LPARAM) text)
#define  SEND_CHECK(text)      SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_CHECK_MESSAGE,        \
                                         (LPARAM) text)

/*-------------------------------------- Обработка элементов диалога */

  static  HINSTANCE  hInst ;
  static   WNDCLASS  FrameWindow ;
  static       HWND  hFrameWindow ;
  static       HWND  hDialog ;

#define  SETs(elm, s)  SetDlgItemText(hDlg, elm, s)
#define  GETs(elm, s)  GetDlgItemText(hDlg, elm, s, 512)

#define  ITEM(e)  GetDlgItem(hDlg, e)

/*------------------------------------------ Поток исполнения команд */

       DWORD WINAPI  Command_Thread(LPVOID) ;

             HANDLE  hCommandEnd ;                    /* Событие - завершение команды */
                int  WaitCommandEnd ;                 /* Флаг ожидания следующей команды */

/*------------------------------------ Обработчики элементов диалога */

  union WndProc_par {
                        long            par ;
                     LRESULT (CALLBACK *call)(HWND, UINT, WPARAM, LPARAM) ; 
                    } ;

  static union WndProc_par  Cmd_WndProc ;
  static union WndProc_par  Tmp_WndProc ;

   LRESULT CALLBACK  EmiRoot_Cmd_WndProc(HWND, UINT, WPARAM, LPARAM) ;

/*------------------------------------ Процедуры обработки сообщений */

   LRESULT CALLBACK  EmiRoot_window_processor(HWND, UINT, WPARAM, LPARAM) ;
      BOOL CALLBACK  EmiRoot_main_dialog     (HWND, UINT, WPARAM, LPARAM) ;

      UINT CALLBACK  iEmiRoot_read_HookProc  (HWND, UINT, WPARAM, LPARAM) ;


/*********************************************************************/
/*                                                                   */
/*                               MAIN                                */

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
   char  ClassName[512] ;
    MSG  SysMessage ;
   char  text[512] ;
    int  i ;

/*------------------------------------- Определение рабочего раздела */

                   getcwd(__cwd, sizeof(__cwd)) ;

/*------------------------------- Подготовка элементов синхронизации */

    hCommandEnd=OpenEvent(EVENT_ALL_ACCESS, false, "CommandEnd") ;  /* Открываем событие */ 

/*---------------------------------------- Подгрузка рабочих модулей */

	    Kernel.keyword="EmiStand" ;

	    Kernel.SetKernel(NULL, hInstance) ;
  	    Kernel.Load     ("") ;

         if(Kernel.modules_cnt==0)
                      EmiRoot_error("No active modules loaded", MB_ICONERROR) ;

       for(i=0 ; i<Kernel.modules_cnt ; i++)
                     Kernel.modules[i].entry->vStart() ;

       for(i=0 ; i<Kernel.modules_cnt ; i++)
                     Kernel.modules[i].entry->vInit() ;

/*------------------------------------------- Подготовка компонентов */

                      UD_init() ;                                   /* Иниц.системы диалога */

                      EmiRoot_show_init() ;                         /* Инициализация системы отображения */

      RSS_Kernel::display.GetList        =EmiRoot_show_getlist ;    /* Пропись функций интерфейса */
      RSS_Kernel::display.ReleaseList    =EmiRoot_show_releaselist ;        
      RSS_Kernel::display.SetFirstContext=EmiRoot_first_context ;
      RSS_Kernel::display.SetNextContext =EmiRoot_next_context ;
      RSS_Kernel::display.ShowContext    =EmiRoot_show_context ;
      RSS_Kernel::display.GetContextPar  =EmiRoot_get_context ;

/*---------------------------------------------------- Инициализация */

                      hInst=hInstance ;

      sprintf(ClassName, "EmiRoot_Frame_%d", _getpid()) ;

/*------------------------------- Регистрация класса первичного окна */

	FrameWindow.lpszClassName= ClassName ;
	FrameWindow.hInstance    = hInstance ;
	FrameWindow.lpfnWndProc  = EmiRoot_window_processor ;
	FrameWindow.hCursor      = LoadCursor(NULL, IDC_ARROW) ;
	FrameWindow.hIcon        =  NULL ;
	FrameWindow.lpszMenuName =  NULL ;
	FrameWindow.hbrBackground=(HBRUSH__ *)GetStockObject(WHITE_BRUSH) ;
	FrameWindow.style        =    0 ;
	FrameWindow.hIcon        =  NULL ;

    if(!RegisterClass(&FrameWindow)) {
                 sprintf(text, "EmiRoot_Frame register error %d", GetLastError()) ;
           EmiRoot_error(text, MB_ICONERROR) ;
	                                return(-1) ;
				     }
/*----------------------------------------- Создание первичного окна */

    hFrameWindow=CreateWindow(ClassName, "EmiRoot", 
			      WS_OVERLAPPEDWINDOW | WS_DLGFRAME,
			      CW_USEDEFAULT, CW_USEDEFAULT, 
			      CW_USEDEFAULT, CW_USEDEFAULT,
				       NULL, NULL, 
				  hInstance, NULL ) ;

/*---------------------------------------- Создание диалогового окна */

        hDialog=CreateDialog(hInst, "IDD_MAIN",
		                  hFrameWindow, EmiRoot_main_dialog) ;

    if(hDialog==NULL) {
                         sprintf(text, "Dialog load error %d", GetLastError()) ;
                   EmiRoot_error(text, MB_ICONERROR) ;
	                          return(-1) ;
                      }

		  ShowWindow(hFrameWindow, SW_HIDE) ;
		UpdateWindow(hFrameWindow) ;
  		  ShowWindow(hDialog, SW_HIDE) ;

                  ShowWindow(hFrameWindow, SW_SHOW) ;
  	          ShowWindow(hDialog, SW_SHOW) ;

	             Kernel.kernel_wnd=hDialog ;
	             Kernel.active_wnd=hDialog ;

/*-------------------------------- Создание монитора рабочих потоков */

             EmiRoot_threads("CREATE", NULL) ;

/*------------------------------------------ Главный диалоговый цикл */

        while(GetMessage(&SysMessage, NULL, 0, 0)) {
          
                       TranslateMessage(&SysMessage) ;
                        DispatchMessage(&SysMessage) ;
	                                           }
/*------------------------------------------------ Завершение работы */

        UnregisterClass(ClassName, GetModuleHandle(NULL)) ;

            CloseHandle(hCommandEnd) ;

/*-------------------------------------------------------------------*/

	return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                 Обработка системных сообщений                     */

  int  EmiRoot_system(void)

{
      MSG  SysMessage ;

/*------------------------------------ Обработка системных сообщений */

      while( PeekMessage(&SysMessage, NULL, 0, 0, PM_NOREMOVE) ) {

              if(SysMessage.message==WM_QUIT) {
                                                       break ;
                                              }

             PeekMessage(&SysMessage, NULL, 0, 0, PM_REMOVE) ;
	TranslateMessage(&SysMessage) ;
	 DispatchMessage(&SysMessage) ;
						                 }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*								     */
/*		Обработчик сообщений "рамочного" окна		     */

 LRESULT CALLBACK  EmiRoot_window_processor(  HWND  hWnd,     UINT  Msg,
 			                    WPARAM  wParam, LPARAM  lParam)
{
  HWND  hChild ;
  RECT  Rect ;


  switch(Msg) {

/*---------------------------------------------------- Создание окна */

    case WM_CREATE:   break ;

/*------------------------------------------------ Системные команды */

    case WM_COMMAND: {
			      return(FALSE) ;
  			           break ;
                     } 

/*------------------------------------------------- Получение фокуса */

    case WM_SETFOCUS:  {
                          SetFocus(GetDlgItem(hDialog, IDC_COMMAND)) ;

			      return(FALSE) ;
  			           break ;
  		       }
/*------------------------------------------------ Изменение размера */

    case WM_SIZE:   {

                    hChild=GetWindow(hWnd, GW_CHILD) ;

        if(wParam==SIZE_RESTORED  ||
           wParam==SIZE_MAXIMIZED   )
                if(IsWindow(hChild)) {
                     GetWindowRect(hChild, &Rect) ;
                        MoveWindow(hChild,  0, 0,
                                              LOWORD(lParam),
                                              HIWORD(lParam), true) ;
                                       } 
  			     break ;
  		    }
/*---------------------------------------------------- Удаление окна */
 
    case WM_DESTROY: {

               if(hWnd!=hFrameWindow)  break ;
                
			PostQuitMessage(0) ;  
			     break ;
		     }
/*------------------------------------------- Обработка по-умолчанию */

    default :        {
		return( DefWindowProc(hWnd, Msg, wParam, lParam) ) ;
			    break ;
		     }
/*-------------------------------------------------------------------*/
	      }

    return(0) ;
}


/*********************************************************************/
/*								     */
/*	   Обработчик сообщений диалогового окна VIEW	             */	

    BOOL CALLBACK  EmiRoot_main_dialog(  HWND  hDlg,     UINT  Msg, 
 			               WPARAM  wParam, LPARAM  lParam) 
{
                 HWND  hPrn ;
                 RECT  wr ;      /* Габарит окна */
	          int  x_screen ;    /* Габарит экрана */	
                  int  y_screen ;
                  int  x_shift ;     /* Центрующий сдвиг */	
                  int  y_shift ;
                 RECT  Rect_base ;
                 RECT  Rect_real ;
                 RECT  Rect ;
                  int  x0_corr ;
                  int  y0_corr ;
                  int  x1_corr ;
                  int  y1_corr ;
                  int  dx ;
                  int  dy ;
                  int  x_size ;
                  int  y_size ;
                  int  x ;
                  int  y ;
                  int  xs ;
                  int  ys ;
                HFONT  font ;           /* Шрифт */
                  int  elm ;            /* Идентификатор элемента диалога */
                  int  status ;
                  int  i ; 
                 char  command[1024] ;
                 char *end ;

  static  struct {
                   int  elem ;
                   int  x ;
                   int  y ;
                   int  xs ;
                   int  ys ;
                 }  loc_pos[]={
                                {IDC_SHOW,           0, 0, 1, 1},
                                {IDC_COMMAND,        0, 1, 1, 0},
                                {IDC_STATUS_INFO,    1, 0, 0, 1},
                                {IDC_CMD_PREFIX,     0, 1, 0, 0},
                                {IDC_CMD_POSTFIX,    1, 1, 0, 0},
                                {IDC_TEXT_ARROW,     0, 1, 0, 0},
                                {IDC_TEXT_ARROW_ALT, 0, 1, 0, 0},
                                {IDC_CMD_ARROW,      0, 1, 0, 0},
                                {IDC_CMD_ARROW_ALT,  0, 1, 0, 0},
                                {IDC_STEP_ARROW,     0, 1, 0, 0},
                                {IDC_STEP_ARROW_ALT, 0, 1, 0, 0},
                                {0}                       } ;

/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {
/*- - - - - - - - - - - - - - - - - - Тест координирования элементов */
                GetWindowRect(          hDlg,    &Rect_base) ;
                GetWindowRect(ITEM(IDC_TESTPOS), &Rect     ) ;

                      dx     =-(Rect_base.right -Rect.left) ;
                      dy     =-(Rect_base.bottom-Rect.top ) ;
                       x_size=  Rect_base.right -Rect_base.left ;
                       y_size=  Rect_base.bottom-Rect_base.top ;

                 SetWindowPos(ITEM(IDC_TESTPOS), 0,
                               x_size+dx, y_size+dy, 0, 0, 
                                SWP_NOSIZE | SWP_NOZORDER) ;
                GetWindowRect(ITEM(IDC_TESTPOS), &Rect_real) ;

                        x1_corr=Rect.left-Rect_real.left ;
                        y1_corr=Rect.top -Rect_real.top ;

                             dx=Rect.left-Rect_base.left ;
                             dy=Rect.top -Rect_base.top  ;

                 SetWindowPos(ITEM(IDC_TESTPOS), 0,
                                      dx, dy, 0, 0, 
                                SWP_NOSIZE | SWP_NOZORDER) ;
                GetWindowRect(ITEM(IDC_TESTPOS), &Rect_real) ;
                        x0_corr=Rect.left-Rect_real.left ;
                        y0_corr=Rect.top -Rect_real.top ;
/*- - - - - - - - - - - - -  Фиксация положения и размеров элементов */
     for(i=0 ; loc_pos[i].elem ; i++) {

                GetWindowRect(ITEM(loc_pos[i].elem), &Rect) ;

        if(loc_pos[i].x )  loc_pos[i].x =Rect.left-Rect_base.right+x1_corr ;
        else               loc_pos[i].x =Rect.left-Rect_base.left +x0_corr ;

        if(loc_pos[i].y )  loc_pos[i].y =Rect.top-Rect_base.bottom+y1_corr ;
        else               loc_pos[i].y =Rect.top-Rect_base.top   +y0_corr ;

        if(loc_pos[i].xs)  loc_pos[i].xs= (     Rect.right-Rect.left     ) 
                                         -(Rect_base.right-Rect_base.left) ;
        else               loc_pos[i].xs=Rect.right-Rect.left ;

        if(loc_pos[i].ys)  loc_pos[i].ys= (     Rect.bottom-Rect.top     ) 
                                         -(Rect_base.bottom-Rect_base.top) ;
        else               loc_pos[i].ys=       Rect.bottom-Rect.top ;
                                      }
/*- - - - - - - - - - - - - - - - - - - Подравнивание рамочного окна */
       x_screen=GetSystemMetrics(SM_CXSCREEN) ;
       y_screen=GetSystemMetrics(SM_CYSCREEN) ;

           hPrn=GetParent( hDlg) ;
            GetWindowRect( hDlg, &wr) ;

        x_shift=(x_screen-(wr.right-wr.left+1))/2 ;
        y_shift=(y_screen-(wr.bottom-wr.top+1))/2 ;

         AdjustWindowRect(&wr, GetWindowLong(hPrn, GWL_STYLE), false) ;
               MoveWindow( hPrn,  x_shift,
                                  y_shift,
                                 wr.right-wr.left+1,
                                 wr.bottom-wr.top+1, true) ;
/*- - - - - - - - - - - - - - - - -  Перехват обработчиков сообщений */
       Tmp_WndProc.call=EmiRoot_Cmd_WndProc ;
       Cmd_WndProc.par =GetWindowLong(ITEM(IDC_COMMAND), GWL_WNDPROC) ;
                        SetWindowLong(ITEM(IDC_COMMAND), GWL_WNDPROC,
                                                   Tmp_WndProc.par) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Пропись шрифтов */
                font=CreateFont(14, 7, 0, 0, FW_THIN, 
                                 false, false, false,
                                  ANSI_CHARSET,
                                   OUT_DEFAULT_PRECIS,
                                    CLIP_DEFAULT_PRECIS,
                                     DEFAULT_QUALITY,
                                      VARIABLE_PITCH,
                                       "Courier New Cyr") ;
                    SendMessage(ITEM(IDC_STATUS_INFO),
                                  WM_SETFONT, (WPARAM)font, 0) ;
/*- - - - - - - - - - - - - - - - -  Инициализация значеий элементов */
            SETs(IDC_COMMAND, "@Tests\\Markers.emi") ;
/*- - - - - - - - - - - - - - - - - - - - - - - Инициализация фокуса */
                          SetFocus(ITEM(IDC_COMMAND)) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}
/*------------------------------------------------ Отработка событий */

    case WM_SIZE: {

                GetWindowRect(hDlg, &Rect_base) ;

                       x_size=Rect_base.right -Rect_base.left ;
                       y_size=Rect_base.bottom-Rect_base.top ;

     for(i=0 ; loc_pos[i].elem ; i++) {

        if(loc_pos[i].x<0)  x =x_size+loc_pos[i].x ;
        else                x =       loc_pos[i].x ;
        
        if(loc_pos[i].y<0)  y =y_size+loc_pos[i].y ;
        else                y =       loc_pos[i].y ;

        if(loc_pos[i].xs<0) xs=x_size+loc_pos[i].xs ;
        else                xs=       loc_pos[i].xs ;

        if(loc_pos[i].ys<0) ys=y_size+loc_pos[i].ys ;
        else                ys=       loc_pos[i].ys ;

           SetWindowPos(ITEM(loc_pos[i].elem),  0,
                                x, y, xs, ys, 
                                  SWP_NOZORDER | SWP_NOCOPYBITS) ;
                                      }

			  return(FALSE) ;
  			     break ;
  		  }
/*------------------------------------ Отработка внутренних сообений */

    case WM_USER:  {
/*- - - - - - - - - - - - - - - - - - - - - - - - Отображение данных */
        if(wParam==_USER_SHOW) {

                    Kernel.ShowExecute((char *)lParam) ;

                                    return(FALSE) ;
                               }
/*- - - - - - - - - - - - -  Исполнение в контексте потока сообщений */
        if(wParam==_USER_CHANGE_CONTEXT) {

                  ((RSS_Kernel *)lParam)->vChangeContext() ;

                                            return(FALSE) ;
                                         }
/*- - - - - - - - - - - - - - - - - "Внешнее" редактирование задания */
        if(wParam==_USER_EXECUTE_COMMAND) {

                             GETs(IDC_COMMAND, command) ;
                             SETs(IDC_COMMAND,   ""   ) ;

               end=strchr(command, '\r') ;
            if(end!=NULL)  strcpy(end, end+1) ;
               end=strchr(command, '\n') ;
            if(end!=NULL)  strcpy(end, end+1) ;

                EmiRoot_command_start(command) ;

                             return(FALSE) ;
                                          }
/*- - - - - - - - - - - - - - - -  Пропись префикса командной строки */
        if(wParam==_USER_COMMAND_PREFIX) {

                            SETs(IDC_CMD_PREFIX, (char *)lParam) ;
                			      return(FALSE) ;
                                         }
/*- - - - - - - - - - - - - - - -  Задание секции прямого управления */
        if(wParam==_USER_DIRECT_COMMAND) {

                       strcpy(__direct_command, (char *)lParam) ;
                			      return(FALSE) ;
                                         }
/*- - - - - - - - - - - - - - - - - - - Пропись объекта по умолчанию */
        if(wParam==_USER_DEFAULT_OBJECT) {

                strncpy(__object_def, (char *)lParam, sizeof(__object_def)-1) ;
                			      return(FALSE) ;
                                         }
/*- - - - - - - - - - - - - - - - - - - - - - -  Сообщение об ошибке */
        if(wParam==_USER_ERROR_MESSAGE) {

                      EmiRoot_error((char *)lParam, MB_ICONERROR) ;
                			      return(FALSE) ;
                                        }
/*- - - - - - - - - - - - - - - - - - - - - - -  Сообщение об ошибке */
        if(wParam==_USER_INFO_MESSAGE) {

                      EmiRoot_error((char *)lParam, MB_ICONINFORMATION) ;
                			      return(FALSE) ;
                                       }
/*- - - - - - - - - - - - - - - - - - - - - - -  Отображение команды */
        if(wParam==_USER_SHOW_COMMAND) {

                            SETs(IDC_COMMAND, (char *)lParam) ;
                                             return(FALSE) ;
                                       }
/*- - - - - - - - - - - - - - - - - - - - - - - Сообщение об статусе */
        if(wParam==_USER_SHOW_INFO) {

                            SETs(IDC_STATUS_INFO, (char *)lParam) ;
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
			  return(FALSE) ;
			     break ;
			}
/*----------------------------------------------------------- Прочее */

//  case WM_PAINT:    break ;

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
/*	          Обработка сообщений об ошибках                     */    

   int  EmiRoot_error(char *message, int  icon)

{
  char *title ;
  char *body ;
  char  text[1024] ;


           memset(text, 0, sizeof(text)) ;
          strncpy(text, message, sizeof(text)-1) ;

      body=strchr(text, '@') ;  
   if(body!=NULL) {  title=text ;
                     *body=0 ;
                      body++ ;        }
   else           {  title="Geo_2D" ;
                      body=text ;     }

       MessageBox(NULL, body, title, icon | MB_SYSTEMMODAL) ;

  return(0) ;
}



/*********************************************************************/
/*								     */
/*	          Новыe обработчики элементов                        */    

  LRESULT CALLBACK  EmiRoot_Cmd_WndProc(  HWND  hWnd,     UINT  Msg,
	                                WPARAM  wParam, LPARAM  lParam) 
{
 LRESULT  result ;
    BYTE  key_status[256] ;
    char  text[1024] ;
    char *arrow ;
   short  status ;

/*-------------------------------- Проверка наличия команды в строке */

        SendMessage(hWnd, WM_GETTEXT, (WPARAM)(sizeof(text)-1), (LPARAM)text) ;

/*-------------------------------------- Обработка клавиш на НАЖАТИЕ */

  if(Msg==WM_KEYDOWN   ||
     Msg==WM_SYSKEYDOWN  ) {

                       memset(key_status, 0, sizeof(key_status)) ;
             GetKeyboardState(key_status) ;
/*- - - - - - - - - - - - - - - - - - - - - - - <Left-Right-Up-Down> */
   if(text[0]==0)
    if(wParam==VK_LEFT  ||
       wParam==VK_RIGHT ||
       wParam==VK_UP    ||
       wParam==VK_DOWN    ) {

            if(wParam==VK_LEFT )  arrow="left" ;
            if(wParam==VK_RIGHT)  arrow="right" ;
            if(wParam==VK_UP   )  arrow="up" ;
            if(wParam==VK_DOWN )  arrow="down" ;
        
              sprintf(text, "$arrow$$%s$", arrow) ;

               status=GetAsyncKeyState(VK_LCONTROL) ;  
            if(status & 0x8000)  strcat(text, "$ctrl$") ;

               status=GetAsyncKeyState(VK_SHIFT) ;
            if(status & 0x8000)  strcat(text, "$shift$") ;

//          if(Msg==WM_SYSKEYUP)  strcat(text, "$alt$") ;

        SendMessage(hWnd, WM_SETTEXT, NULL, (LPARAM)text) ;

        SendMessage(hDialog, WM_USER,
                     (WPARAM)_USER_EXECUTE_COMMAND, (LPARAM)NULL) ;

                                        return(FALSE) ;
                            }
/*- - - - - - - - - - - - - - - - - - - - - - -  <PageUp>/<PageDown> */
    if(wParam==VK_PRIOR ||
       wParam==VK_NEXT    ) {

         if(wParam==VK_PRIOR)  strcpy(text, "$prev_command$") ;
         if(wParam==VK_NEXT )  strcpy(text, "$next_command$") ;

        SendMessage(hWnd, WM_SETTEXT, NULL, (LPARAM)text) ;

        SendMessage(hDialog, WM_USER,
                     (WPARAM)_USER_EXECUTE_COMMAND, (LPARAM)NULL) ;

                                        return(FALSE) ;
                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                           }
/*---------------------------------------- Обработка клавиш на ОТРЫВ */

  if(Msg==WM_KEYUP   ||
     Msg==WM_SYSKEYUP  ) {
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - -  <Enter> */
    if(wParam==VK_RETURN) {

        SendMessage(hDialog, WM_USER,
                     (WPARAM)_USER_EXECUTE_COMMAND, (LPARAM)NULL) ;

                                        return(FALSE) ;
                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                        }
/*-------------------------------- Вызов штатной процедуры обработки */

      result=CallWindowProc(Cmd_WndProc.call,
                               hWnd, Msg, wParam, lParam) ;

/*-------------------------------------------------------------------*/

  return(result) ;
}


/********************************************************************/
/*								    */
/*                       Запуск команд                              */

   int  EmiRoot_command_start(char *cmd)

{
       HANDLE  hThread ;
        DWORD  hPID ;
 static  char  command[1024] ;

/*------------------------------ Синхронизация с предыдущей командой */

                            ResetEvent(hCommandEnd) ;

   if(WaitCommandEnd) {

                    WaitForSingleObject(hCommandEnd, INFINITE) ;
                             ResetEvent(hCommandEnd) ;
                      }
/*------------------------------------- Запуск командного процессора */

//             SEND_CHECK("") ;                                     /* Сброс сообщения о контроле условий сцены */

                  strncpy(command, cmd, sizeof(command)) ;          /* Перенесение контекста команды */

     hThread=CreateThread(NULL, 0, 
                            Command_Thread, command, 0, &hPID) ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                THREAD - Поток исполнения команд                   */

  DWORD WINAPI  Command_Thread(LPVOID  Pars)

{
  char command[1024] ;

/*------------------------------------------------------- Подготовка */

                  strncpy(command, (char *)Pars, sizeof(command)) ;

          EmiRoot_threads("ADD", command) ;                         /* Регистрируем рабочий поток */

/*------------------------------------------------ Выпонение команды */

                EmiRoot_command_processor(command) ;

/*------------------------------------------------------- Завершение */

     if(WaitCommandEnd)  SetEvent(hCommandEnd) ;

              EmiRoot_threads("DELETE", NULL) ;                     /* Исключаем рабочий поток */

/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*                    Процессор команд                              */

   int  EmiRoot_command_processor(char *command)

{
  static     int  file_rd_flag ;         /* Флаг считывания командного файла */
  static     int  file_wr_flag ;         /* Флаг записи командного файла */
  static    char  postfix[1024] ;        /* Блок постфиксных команд */
  static    char  arrow_cmd[1024] ;      /* Стрелочные команды */
  static    char  arrow_SHT_cmd[1024] ;
  static    char  arrow_step[32] ;       /* Шаги стрелочных команд */
  static    char  arrow_SHT_step[32] ;
  static     int  arrow_wait ;
             int  arrow_command ;
            char  object[32] ;
             int  postfix_flag ;         /* Флаг обработки потфиксной команды */
            HWND  hDlg ;
             int  status ;
            char  cmd_buff[1024] ;
            char *next ;
            char *name ;
            char *end ;
            char *quotes ;
            char *word2 ;
            char *word3 ;
            char  work[1024] ;
            char  text[1024] ;
             int  size ;
             int  i ;

  static    char *cmd_history[32] ;      /* История команд */
  static     int  cmd_history_ptr  ;     /* Индекс команды в истории команд */
            char *cmd_new ;

typedef  struct {
                   char  name[32] ;
                   char  postfix[1024] ;
                   char  arrow_cmd[1024] ;
                   char  arrow_SHT_cmd[1024] ;
                } EmiRoot_commands ;

  static  EmiRoot_commands *commands_pool[100] ;   /* Список наборов команд */

#define   OBJECTS       RSS_Kernel::kernel->kernel_objects 
#define   OBJECTS_CNT   RSS_Kernel::kernel->kernel_objects_cnt 

/*------------------------------------------------------- Подготовка */

                        hDlg=hDialog ;

                      RSS_Kernel::kernel->stop=0 ;

                           arrow_command=0 ;

/*---------------------------------------------------- Инициализация */

        if(    arrow_step[0]==0)      strcpy(arrow_step, "0.1") ;
        if(arrow_SHT_step[0]==0)  strcpy(arrow_SHT_step, "5.0") ;

/*-------------------------------------------- Цикл командных файлов */

  do {                                                              /* CIRCLE.0 */
                               WaitCommandEnd=0 ;

        if(file_rd_flag) {                                          /* Считывание команды из файла */
                            status=EmiRoot_command_read(&command) ;
                         if(status) {   file_rd_flag=0 ; 
                                      SHOW_COMMAND("") ;
                                           break ;        }                             

                               WaitCommandEnd=1 ;
                         }

                EmiRoot_command_normalise(command) ;                /* Нормализация команды */

        if(file_wr_flag) {                                          /* Запись команды в файл */
                            EmiRoot_command_write(command) ;
                         }
/*----------------------------------------- Работа с историей команд */
/*- - - - - - - - - - - - - - - -  Восстановление предыдущей команды */
     if(!stricmp(command, "$prev_command$")) {

                                        cmd_history_ptr++ ;

          if(cmd_history_ptr<0)  cmd_history_ptr=0 ;

          if(            cmd_history_ptr >= 32 )  cmd_history_ptr=31 ;
          if(cmd_history[cmd_history_ptr]==NULL)  cmd_history_ptr-- ;

          if(cmd_history[cmd_history_ptr]!=NULL)
               SHOW_COMMAND(cmd_history[cmd_history_ptr]) ;

                                                 return(0) ;                                      
                                             }
/*- - - - - - - - - - - - - - - - - Восстановление следующей команды */
     if(!stricmp(command, "$next_command$")) {

                                         cmd_history_ptr-- ;
                if(cmd_history_ptr<0) {  cmd_history_ptr=0 ;
                                           SHOW_COMMAND("") ;  }
                else                       SHOW_COMMAND(cmd_history[cmd_history_ptr]) ;

                                                   return(0) ;
                                             }
/*- - - - - - - - - - - - - - - - - - - - Сохранение текущей команды */
       while(        cmd_history[0]==NULL    ||                     /* BLOCK.1 */
             stricmp(cmd_history[0], command)  ) {                    

                         cmd_history_ptr=-1 ;

          if(strchr(command, '$')!=NULL)  break ;

          if(command[0]==0)  break ;

             cmd_new=(char *)calloc(1, strlen(command)+8) ;
          if(cmd_new==NULL)  break ;

              if(cmd_history[31]!=NULL)  free(cmd_history[31]) ;

          for(i=31 ; i>0 ; i--)  cmd_history[i]=cmd_history[i-1] ;

                     strcpy(cmd_new, command) ;
             cmd_history[0]=cmd_new ;

                                break ;
                                               }                    /* BLOCK.1 */
/*---------------------------------------------- Специальные команды */
/*- - - - - - - - - - - - - - - - - Запуск создания командного файла */
   if(command[0]=='@' &&
      command[1]=='@'   ) {

            memmove(command+6, command+2, strlen(command+1)+1) ;
            memmove(command,  "#save ", 6) ;

                 EmiRoot_command_write(command) ;

                            file_wr_flag=1 ;
                                 continue ;
                           }
/*- - - - - - - - - - - - - - - - - - -  Исполнение командного файла */
   if(command[0]=='@') {

            memmove(command+6, command+1, strlen(command+1)+1) ;
            memmove(command,  "#file ", 6) ;

                             file_rd_flag=1 ;
                                 continue ;
                       }
/*- - - - - - - - - - - - - - - - - - Исполнение постфиксной команды */
   if(command[0]=='.' &&
      command[1]== 0    ) {
                                  strcpy(command, postfix) ;
                          }
/*- - - - - - - - - - - - - - - - - - -  Задание постфиксной команды */
   if(command[0]=='.' &&
      command[1]!='.'   ) {

                 memset(postfix, 0, sizeof(postfix)) ;
                strncpy(postfix, command+1, sizeof(postfix)-1) ;

                   SETs(IDC_CMD_POSTFIX, postfix) ;
         EmiRoot_system() ;

                                continue ;
                          }
/*- - - - - - - - - - - - - - - - - - - Удаление постфиксной команды */
   if(command[0]=='.' &&
      command[1]=='.'   ) {

                     memset(postfix, 0, sizeof(postfix)) ;

                       SETs(IDC_CMD_POSTFIX, postfix) ;
             EmiRoot_system() ;

                                continue ;
                          }
/*- - - - - - - - - - - - - - - - - Задание SHIFT-стрелочной команды */
   if(strstr(command, ">>")!=NULL) {

    if(isdigit(command[2])    ||
               command[2]=='.'  ){
                                strncpy(arrow_SHT_step, command+2, sizeof(arrow_SHT_step)) ;
                                   SETs(IDC_STEP_ARROW_ALT, arrow_SHT_step) ;
                                 }
    else                         {
     if(!strcmp(command, ">>"))         arrow_SHT_cmd[0]=0 ;
     else                       strncpy(arrow_SHT_cmd, command, sizeof(arrow_SHT_cmd)) ;

                                   SETs(IDC_CMD_ARROW_ALT, arrow_SHT_cmd) ;
                                 }

                                         EmiRoot_system() ;    
                                            continue ;
                                   } 
/*- - - - - - - - - - - - - - - - - - - - Задание стрелочной команды */
   if(strstr(command, ">")!=NULL) {

    if(isdigit(command[1])    ||
               command[1]=='.'  ){
                                strncpy(arrow_step, command+1, sizeof(arrow_step)) ;
                                   SETs(IDC_STEP_ARROW, arrow_step) ;
                                   SETs(IDC_STEP_ARROW_ALT, arrow_SHT_step) ;
                                 }
    else                         {
     if(!strcmp(command, ">" ))         arrow_cmd[0]=0 ;
     else                       strncpy(arrow_cmd, command, sizeof(arrow_cmd)) ;

                                   SETs(IDC_CMD_ARROW, arrow_cmd) ;
                                   SETs(IDC_CMD_ARROW_ALT, arrow_SHT_cmd) ;
                                 }

                                           EmiRoot_system() ;    
                                              continue ;
                                  } 
/*- - - - - - - - - - - - - - - - - - - -  Сохранение набора команд  */
   if(!memicmp(command, "!save ", strlen("!save "))) {

#define  CP  commands_pool

            name=command+strlen("!save ") ;

        for(i=0 ; CP[i] ; i++)
          if(!stricmp(CP[i]->name, name))  break ;

          if(CP[i]==NULL) {
                CP[i]=(EmiRoot_commands *)calloc(1, sizeof(*CP[i])) ;
                          }

          if(CP[i]!=NULL) {
              strncpy(CP[i]->name,          name,          sizeof(CP[i]->name)-1) ;
              strncpy(CP[i]->postfix,       postfix,       sizeof(CP[i]->postfix)-1) ;
              strncpy(CP[i]->arrow_cmd,     arrow_cmd,     sizeof(CP[i]->arrow_cmd)-1) ;
              strncpy(CP[i]->arrow_SHT_cmd, arrow_SHT_cmd, sizeof(CP[i]->arrow_SHT_cmd)-1) ;
                          }
                                    continue ;
#undef  CP
                                                     }
/*- - - - - - - - - - - - - - - - - -  Восстановление набора команд  */
   if(command[0]=='!') {

#define  CP  commands_pool

            name=command+1 ;

        for(i=0 ; CP[i] ; i++)
          if(!stricmp(CP[i]->name, name))  break ;

          if(CP[i]==NULL) {
                             sprintf(text, "Неизвестный набор команд: %s", strupr(name)) ;
                       EmiRoot_error(text, MB_ICONERROR) ;
                              continue ;
                          }

              strncpy(name,          CP[i]->name,          sizeof(name)-1) ;
              strncpy(postfix,       CP[i]->postfix,       sizeof(postfix)-1) ;
              strncpy(arrow_cmd,     CP[i]->arrow_cmd,     sizeof(arrow_cmd)-1) ;
              strncpy(arrow_SHT_cmd, CP[i]->arrow_SHT_cmd, sizeof(arrow_SHT_cmd)-1) ;

                                   SETs(IDC_CMD_POSTFIX, postfix) ;
                                   SETs(IDC_CMD_ARROW, arrow_cmd) ;
                                   SETs(IDC_CMD_ARROW_ALT, arrow_SHT_cmd) ;

                                     EmiRoot_system() ;    
                                        continue ;

#undef  CP
                       }
/*------------------------------------ Обработка "стрелочных" команд */

   if(strstr(command, "$arrow$")!=NULL) {

     if(arrow_wait) {
                       return(0) ;
                    }

               arrow_command=1 ;
               arrow_wait   =1 ;

     if(strstr(command, "$shift$")!=NULL) {  strcpy(text, arrow_SHT_cmd) ;
                                                    word2=arrow_SHT_step ;  }
     else                                 {  strcpy(text, arrow_cmd) ;
                                                    word2=arrow_step ;      }

     if(text[0]==0)  continue ;

      size=strlen(command)+strlen(word2)+3 ;
       end=strrchr(text, '>') ;
           memmove(end+size+1, end+1, strlen(end+1)+1) ;
                   end[1]=0 ;
            strcat(end, " ") ;
            strcat(end, command) ;
            strcat(end, " ") ;
            strcat(end, word2) ;
                   end[size]=' ' ;

                     command=text ;

     if(postfix[0]) {   strcat(command,  "%"   ) ;
                        strcat(command, postfix) ;   }

                                        }
/*-------------------------------------------- Цикл вложенных команд */

                    postfix_flag=0 ;

   for(next=command ; next!=NULL ; command=next+1) {                /* CIRCLE.1 */

     for(next=command ; *next ; next++) {
       if(*next==';')            break ;
       if(*next=='%') {  postfix_flag=1 ;
                                 break ;  }
                                        }

      if(*next==0   )  next=NULL ;
      if( next!=NULL) *next=  0 ;

                               strcpy(cmd_buff, command) ;
            EmiRoot_command_normalise(cmd_buff) ;

                              command=cmd_buff ;

/*--------------------------------- Подстановка объекта по-умолчанию */

   if(strchr(command, '*')!=NULL) {

        do {                                                        /* Подстановка производится до первой кавычки в строке */
                 end=strchr(command, '*') ;
              if(end==NULL)  break ;

                 quotes=strchr(command, '"') ;
              if(quotes!=NULL && quotes<end)  break ;

                  memmove(end+strlen(__object_def), end+1, strlen(end+1)+1) ;
                   memcpy(end, __object_def, strlen(__object_def)) ;

           } while(1) ;

        do {                                                        /* Затираем кавычки */ 
                 end=strchr(command, '"') ;
              if(end==NULL)  break ;

                     strcpy(end, end+1) ;

           } while(1) ;
                                 }
/*----------------------------------------- Разыменовывание объектов */

     do {                                                           /* BLOCK.1 */
/*- - - - - - - - - - - - - - - - - - - - - -  Идентификация объекта */
                 memset(object, 0, sizeof(object)) ;
                strncpy(object, command, sizeof(object)-1) ;
             end=strchr(object, ' ') ;                              /* Вырезаем первое слово команды */
          if(end!=NULL)  *end=0 ;

        for(i=0 ; i<OBJECTS_CNT ; i++)                              /* Ишем обьект по имени */
          if(!stricmp(OBJECTS[i]->Name, object))  break ;

          if(i==OBJECTS_CNT)  break ;                               /* Если такого нет... */

             memset(__object_def, 0, sizeof(__object_def)) ;
            strncpy(__object_def, object, sizeof(__object_def)-1) ; /* Фиксируем обьект по-умолчанию */

          if(end==NULL) {  command[0]=0 ;  break ;  }               /* Если в команде только */
                                                                    /*   указание обЪекта... */
/*- - - - - - - - - - - - - - - - - - - - Переформировывание команды */
       do {
                word2=strchr(command, ' ') ;
             if(word2==NULL) {
                                 word2="" ;
                                 word3="" ;
                               *object= 0 ;
                                  break ;
                             }
                word2++ ;
                word3=strchr(word2, ' ') ;
             if(word3==NULL)    word3="" ;
             else            { *word3= 0 ;
                                word3++ ;  } 
                           
          } while(0) ;

           sprintf(work, "%s %s %s %s", OBJECTS[i]->Type,           /* Переформировываем команду */
                                        word2,
                                        object,
                                        word3            ) ;
                   command=work ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        } while(0) ;                                                /* BLOCK.1 */

/*------------------------------ Опрос командных процессоров модулей */

            if(command[0]==0)  continue ;

                           iDebug(command, NULL) ;

    for(i=0 ; i<Kernel.modules_cnt ; i++) {
      
       if(        postfix_flag          ||
                 __direct_command[0]==0 ||
          strcmp(__direct_command, 
                     Kernel.modules[i].entry->identification)==0 ) {
             status=Kernel.modules[i].entry->vExecuteCmd(command) ;
          if(status==-1)  return(-1) ;
          if(status== 0)    break ;
                                                                   }
                                          }

       if(i<Kernel.modules_cnt)  continue ;

/*--------------------------------- Вызов командного процессора ядра */

        status=Kernel.vExecuteCmd(command) ;
     if(status==1) {
       if(command[0]!=0) {
                             sprintf(text, "Неизвестная команда: %s", command) ;
                       EmiRoot_error(text, MB_ICONERROR) ;
                  if(arrow_command)  arrow_wait=0 ;
                                return(-1) ;
                         }
                   } 
/*-------------------------------------------- Цикл вложенных команд */

                          EmiRoot_system() ;

                                                   }                /* CONTINUE.1 */
/*-------------------------------------------- Цикл командных файлов */

     } while(file_rd_flag) ;                                        /* CONTINUE.0 */

/*-------------------------------------------------------------------*/

     if(arrow_command)  arrow_wait=0 ;

#undef   OBJECTS
#undef   OBJECTS_CNT

   return(0) ;
}


/********************************************************************/
/*								    */
/*                 Обработчик командного файла                      */

   int  EmiRoot_command_read(char **command)

{
  static  int  step ;
  static char  buff[1024] ;
       double  value ;
         char *end ;
         char  text[1024] ;

#define       _FILES_MAX  8
  static struct {   FILE *file ;
                     int  row ; 
                    char  name[512] ;
                }  files[_FILES_MAX] ;  
  static      int  file_num=-1 ;

/*------------------------------------------- Цикл извлечения команд */

   while(1) {                                                       /* CIRCLE.1 */
                  memset(buff, 0, sizeof(buff)) ;
                  memset(text, 0, sizeof(text)) ;
/*- - - - - - - - - - - - - - - - - - -  Извлечение команды из файлы */
       if(file_num>=0) {
                          Sleep(step) ;

          end=fgets(buff, sizeof(buff)-1, files[file_num].file) ;   /* Извлекаем очередную строку */
       if(end==NULL) {                                             /* Если файл закончился... */
                        fclose(files[file_num].file) ;
                                     file_num-- ;
                                  if(file_num<0)  return(-1) ;
                                                   continue ;
                     }

                                 files[file_num].row++ ;

           sprintf(text, "Командный файл: %s\n"
                         "        Строка: %d\n", 
                                 files[file_num].name, 
                                 files[file_num].row  ) ;
                       }
/*- - - - - - - - - - - - - - - - - - - -  Обработка внешней команды */
       else            {
                          strncpy(buff, *command, sizeof(buff)-1) ;
                       }      
/*--------------------------------- Предварительная обработка строки */

                EmiRoot_command_normalise(buff) ;                   /* Нормализация строки */

            if(*buff==';')  continue ;                              /* Если комментарий... */

/*---------------------------- Команда - завершение командного файла */

         if(!memicmp(buff, "#exit", 
                    strlen("#exit" ))) {
                                           return(-1) ;
                                       }
/*------------------------------------- Команда - Межкомандная пауза */

         if(!memicmp(buff, "#step ", 
                    strlen("#step " ))) {

               value=strtod(buff+strlen("#step "), &end) ;
            if(value < 0. ||
               value >10. ||
                *end!= 0    ) {
                                  strcat(text, "Некорректное значение в операторе #STEP") ;
                           EmiRoot_error(text, MB_ICONERROR) ;
                                      return(-1) ;
                              }  

                  step=value*1000 ;
                                        }
/*-------------------------------------------------- Команда - Пауза */

    else if(!memicmp(buff, "#pause ", 
                    strlen("#pause "))) {

               value=strtod(buff+strlen("#pause "), &end) ;
            if(value < 0. ||
                *end!= 0    ) {
                                  strcat(text, "Некорректное значение в операторе #PAUSE") ;
                           EmiRoot_error(text, MB_ICONERROR) ;
                                   return(-1) ;
                              }  

                          value=value*1000-step ;
                    Sleep(value) ;

                                        }
/*--------------------------------------------- Команда - Примечание */

    else if(!memicmp(buff, "#remark ", 
                    strlen("#remark "))) {

            MessageBox(NULL, buff+strlen("#remark "), "Emi Stand",
                               MB_ICONINFORMATION | MB_TASKMODAL) ;
                                         }
/*----------------------------------------- Команда - Вложенный файл */

    else if(!memicmp(buff, "#file ", 
                    strlen("#file "))) {

#define  F   files[file_num+1]

            if(file_num+1>=_FILES_MAX) {
                        strcat(text, "#FILE: Слишком много вложенных файлов") ;
                 EmiRoot_error(text, MB_ICONERROR) ;
                         return(-1) ;
                                       }

                     memset(F.name, 0, sizeof(F.name)) ;
                    sprintf(F.name, "%s\\%s", __cwd, buff+strlen("#file ")) ;

               F.file=fopen(F.name, "rt") ;
            if(F.file==NULL) {
                         strcat(text, "#FILE: Ошибка открытия файла ") ;
                         strcat(text, F.name) ;
                  EmiRoot_error(text, MB_ICONERROR) ;
                                  return(-1) ;
                             }

               F.row=0 ;
                file_num++ ;
#undef   F
                                       }
/*------------------------------------------------ 'Внешняя' команда */

    else                              {                
                                                break ;  
                                      }
/*------------------------------------------- Цикл извлечения команд */
            }                                                       /* CONTINUE.1 */
/*-------------------------------------------------------------------*/

                  SHOW_COMMAND(buff) ;
                EmiRoot_system() ;

                      *command=buff ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*                   Запись командного файла                        */

   int  EmiRoot_command_write(char *command)

{
   static  FILE *file ;

/*---------------------------------------- Открытие командного файла */

    if(!memicmp(command, "#save ", strlen("#save "))) {

         if(file!=NULL)  fclose(file) ;

             command+=strlen("#save ") ;
         if(*command==0)  return(0) ;

               file=fopen(command, "at") ;

                        return(0) ;
                                                      }
/*----------------------------------------------- Сохранение команды */

       if(file==NULL)  return(-1) ;

         fwrite(command, 1, strlen(command), file) ;
         fwrite("\n",    1, strlen("\n"),    file) ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*                 Нормализация командной строки                    */

  void  EmiRoot_command_normalise(char *command)

{
     char *tmp ;

/*----------------------------------- Замена спецсимволов на пробелы */

      for(tmp=command ; *tmp ; tmp++)
        if(*tmp=='\t' ||
           *tmp=='\r' ||
           *tmp=='\n'   )  *tmp=' ' ;

/*------------------------------ Отсечка начальных/конечных пробелов */

      for(tmp=command ; *tmp==' ' ; tmp++) ;

        if(tmp!=command)  strcpy(command, tmp) ;

      for(tmp=command+strlen(command)-1 ; 
           tmp>=command && *tmp==' ' ;  tmp--) *tmp= 0 ;

/*------------------------------------------- Ужатие парных пробелов */

     while(1) {
                    tmp=strstr(command, "  ") ;
                 if(tmp==NULL)  break ;

                    strcpy(tmp, tmp+1) ;
              }
/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*                   Отладочная печать в файла                      */

   void  iDebug(char *text, char *path,  int  condition)
{
    if(condition)  iDebug(text, path) ;
}


   void  iDebug(char *text, char *path)
{
   static int  init_flag ;
         FILE *file ;         


    if(path==NULL) {
                             path="commands.log" ;
      if(!init_flag)  unlink(path) ;
          init_flag=1 ;
                   }

       file=fopen(path, "at") ;
    if(file==NULL)  return ;

           fwrite(text, 1, strlen(text), file) ;
           fwrite("\n", 1, strlen("\n"), file) ;
           fclose(file) ;
}


/********************************************************************/
/********************************************************************/
/**							           **/
/**            ОПИСАНИЕ КЛАССА ОБЩЕГО МОДУЛЯ УПРАВЛЕНИЯ            **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*                            Список команд                         */

  struct RSS_Module_Main_instr  RSS_Module_Main_InstrList[]={

 { "help",      "?",     "# HELP   - список доступных команд", 
                          NULL,
                         &RSS_Module_Main::cHelp   },
 { "show",      "s",     "#SHOW   - отобразить рабочее пространство", 
                          NULL,
                        &RSS_Module_Main::cShowScene  },
 { "kill",      "k",     "# KILL - удалить объект",
                         " KILL[/Q] <Имя>\n"
                         "   Удаляет именованный обьект из системы",
                         &RSS_Module_Main::cKill },
 { "all",       "all",   "# ALL - выдать список объектов",
                          NULL,
                         &RSS_Module_Main::cAll },
 { "lookinfo",  "li",    "# LOOKINFO - показать параметры камеры ",
                          NULL,
                         &RSS_Module_Main::cLookInfo },
 { "eye",       "eye",   "# EYE - задать точку положения глаза",
                         " EYE    <X> <Y> <Z>\n"
                         " EYE/X  <X>\n"
                         " EYE/+X <X>\n"
                         " EYE>   <Шаг>\n"
                         " EYE>>  <Шаг>\n",
                         &RSS_Module_Main::cLookPoint },
 { "look",      "look",  "# LOOK - задать направление взгляда",
                         " LOOK    <A> <E> <R>\n"
                         " LOOK/A  <A>\n"
                         " LOOK/+A <A>\n"
                         " LOOK>   <Шаг>\n"
                         " LOOK>>  <Шаг>\n",
                         &RSS_Module_Main::cLookDirection },
 { "lookat",    "la",    "# LOOKAT - задать точку(объект) наблюдения",
                         " LOOKAT <Имя объекта>\n",
                         &RSS_Module_Main::cLookAt },
 { "zoom",      "z",     "# ZOOM - управление полем зрения камеры",
                         " ZOOM <Ширина поля зрения>\n"
                         " ZOOM+\n"
                         " ZOOM-\n",
                         &RSS_Module_Main::cLookZoom },
 { "read",      "read",  "# READ - считать данные из файла",
                         " READ [<Файл>]  \n",
                         &RSS_Module_Main::cRead },
 { "write",     "write", "# WRITE - записать данные в файл",
                         " WRITE [<Файл>]  \n"
                         " WRITE/О [<Файл>] <Объект1> <Объект2>...\n",
                         &RSS_Module_Main::cWrite },
 { "threads",   "th",    "# THREADS - монитор рабочих потоков", 
                          NULL,
                         &RSS_Module_Main::cThreads   },
 { "stop",      "stop",  "# STOP - остановить рабочий поток",
                          NULL,
                         &RSS_Module_Main::cStop   },
 { "modules",   "mod",   "# MODULES - список подключенных модулей", 
                          NULL,
                         &RSS_Module_Main::cModules   },
 { "memory",    "mem",   "# MEMORY - управление памятью", 
                         " MEMORY ON    -  включение регистрации памяти \n"
                         " MEMORY LIST  -  сброс дампа памяти \n",
                         &RSS_Module_Main::cMemory   },
 { "srand",     "srand", "# SRAND - управление генератором случайных последовательностей", 
                         " SRAND <Число> -  задание старт-точки генератора, Число>1 \n"
                         " SRAND AUTO    -  перевод старт-точки в автоматический режим \n",
                         &RSS_Module_Main::cSrand    },
 {  NULL }
                                                            } ;

/********************************************************************/
/*								    */
/*		     Общие члены класса             		    */

    struct RSS_Module_Main_instr *RSS_Module_Main::mInstrList ;

/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Module_Main::RSS_Module_Main(void)

{
	   keyword="EmiStand" ;
    identification="Module_Main" ;

        mInstrList=RSS_Module_Main_InstrList ;
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Module_Main::~RSS_Module_Main(void)

{
}


/********************************************************************/
/*								    */
/*		        Выполнить команду       		    */

  int  RSS_Module_Main::vExecuteCmd(const char *cmd)

{
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  i ;

/*------------------------------------------------------- Подготовка */

             memset(command, 0, sizeof(command)) ;
            strncpy(command, cmd, sizeof(command)-1) ;

/*--------------------------------------------- Выделение инструкции */

       instr=command ;                                              /* Выделяем слово с названием команды */

     for(end=instr ; *end!= 0  &&
                     *end!=' ' &&
                     *end!='-' &&
                     *end!='+' &&
                     *end!='>' &&
                     *end!='/'    ; end++) ;

      if(*end!= 0 &&
         *end!=' '  )  memmove(end+1, end, strlen(end)+1) ;

      if(*end!=0) {  *end=0 ;  end++ ;  }
      else            end="" ;

   for(i=0 ; mInstrList[i].name_full!=NULL ; i++)                   /* Ищем команду в списке */
     if(!stricmp(instr, mInstrList[i].name_full) ||
        !stricmp(instr, mInstrList[i].name_shrt)   )   break ;

     if(mInstrList[i].name_full==NULL) {                            /* Если такой команды нет... */
                                            return(1) ;
                                       }

     if(mInstrList[i].process!=NULL)                                /* Выполнение команды */
                       (this->*mInstrList[i].process)(end) ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		        Получить параметр       		    */

     int  RSS_Module_Main::vGetParameter(char *name, char *value)

{
   char  text[128] ;

        if(!stricmp(name, "?1")) {
                                 }
   else if(!stricmp(name, "?2")) {
                                 }
   else                          {
                sprintf(text, "Запрос неизвестного параметра: %s", name) ;
             SEND_ERROR(text) ;
                                       return(-1) ;
                                 }

   return(0) ;
}


/********************************************************************/
/*								    */
/*                   Выполнить целевой функционал                   */

    void  RSS_Module_Main::vProcess(void)

{
   int  i ;
        
#define   MODULES       this->kernel->modules 
#define   MODULES_CNT   this->kernel->modules_cnt 

    for(i=0 ; i<MODULES_CNT ; i++) MODULES[i].entry->vProcess() ;

#undef    MODULES
#undef    MODULES_CNT

}


/********************************************************************/
/*								    */
/*                    Отобразить связанные данные                   */

    void  RSS_Module_Main::vShow(char *layer)

{
    SendMessage(hDialog, WM_USER, _USER_SHOW, (LPARAM)layer) ;
}


/********************************************************************/
/*								    */
/*		        Записать данные в строку		    */

    void  RSS_Module_Main::vWriteSave(std::string *text)

{
  std::string  buff ;
          int  status ;

/*----------------------------------------------- Заголовок описания */

     *text="#BEGIN MODULE MAIN\n" ;

/*----------------------------------------------------------- Данные */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Параметры окон */
          status=this->display.SetFirstContext(NULL) ;
    while(status==0) {
                         EmiRoot_write_context(&buff) ;

                             *text+="WINDOW " ;
                             *text+= buff ;
                             *text+="\n" ;
                 
          status=this->display.SetNextContext(NULL) ;
                     }
/*------------------------------------------------ Концовка описания */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        Считать данные из строки		    */

    void  RSS_Module_Main::vReadSave(std::string *data)

{
        char *buff ;
         int  buff_size ;
 std::string  decl ;
        char *work ;
        char  text[1024] ;
        char *end ;

/*----------------------------------------------- Контроль заголовка */

   if(memicmp(data->c_str(), "#BEGIN MODULE MAIN\n", 
                      strlen("#BEGIN MODULE MAIN\n")))  return ;

/*------------------------------------------------ Извлечение данных */

              buff_size=data->size()+16 ;
              buff     =(char *)calloc(1, buff_size) ;

       strcpy(buff, data->c_str()) ;
        
/*---------------------------------------------- Построчная разборка */

                work=buff+strlen("#BEGIN MODULE MAIN\n") ;

    for( ; ; work=end+1) {                                          /* CIRCLE.0 */
                                      end=strchr(work, '\n') ;
                                   if(end==NULL)  break ;
                                     *end=0 ;

/*---------------------------------------------------- Описание окна */

          if(!memicmp(work, "WINDOW", strlen("WINDOW"))) {

                                          decl=work+strlen("WINDOW") ;
                    EmiRoot_read_context(&decl) ;
                                                         }
/*---------------------------------------------------------- ??????? */

     else if(!memicmp(work, "??????", strlen("??????"))) {

                                                         }
/*----------------------------------------- Неизвестный спецификатор */

     else                                                {

                sprintf(text, "Module MAIN: "
                              "Неизвестная спецификация %20.20s", work) ;
             SEND_ERROR(text) ;
                                 break ;
                                                         }
/*---------------------------------------------- Построчная разборка */
                         }                                          /* CONTINUE.0 */
/*-------------------------------------------- Освобождение ресурсов */

                free(buff) ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции HELP                    */

  int  RSS_Module_Main::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Main_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции SHOW                    */

  int  RSS_Module_Main::cShowScene(char *cmd)

{
         this->vShow(NULL) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции Kill                    */
/*								    */
/*      KILL <Имя>                                                  */

  int  RSS_Module_Main::cKill(char *cmd)

{
   int   quiet_flag ;      /* Флаг режима "молчания" */
  char *name ;
  char *end ;
   int  i ;
   int  j ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*-------------------------------------- Дешифровка командной строки */
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
                      quiet_flag=0 ;

       if(*cmd=='/') {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("Некорректный формат команды") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, 'q')!=NULL ||
                   strchr(cmd, 'Q')!=NULL   )  quiet_flag=1 ;

                           cmd=end+1 ;
                     }
/*- - - - - - - - - - - - - - - - - - - - - - - -  Разбор параметров */        
                  name=cmd ;                                        /* Извлекаем имя объекта */
                   end=strchr(name, ' ') ;
                if(end!=NULL)  *end=0 ;

    if(name[0]==0) {                                                /* Если имя не задано... */
               SEND_ERROR("Не задано имя объекта") ;
                          return(-1) ;
                   }
/*------------------------------------------- Удаление всех объектов */

    if(!stricmp(name, "ALL")) {

       for(i=0 ; i<OBJECTS_CNT ; i++) {
                                         OBJECTS[i]->vFree() ;      /* Освобождение ресурсов */
                                 delete  OBJECTS[i] ;
                                      }

                                         OBJECTS_CNT=0 ;

                                   this->kernel->vShow("REFRESH") ;
                                         return(0) ;
                              }
/*------------------------------------------- Поиск обьекта по имени */

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, name))  break ;

    if(i==OBJECTS_CNT) {                                            /* Если имя не задано... */
        if(!quiet_flag)  SEND_ERROR("Объекта с таким именем НЕ существует") ;
                             return(-1) ;
                       }
/*------------------------------------------------- Удаление обьекта */

                                         OBJECTS[i]->vFree() ;      /* Освобождение ресурсов */
                                 delete  OBJECTS[i] ;

       for(j=i+1 ; j<OBJECTS_CNT ; j++)  OBJECTS[j-1]=OBJECTS[j] ;  /* Поджатие списка объектов */

                     OBJECTS_CNT-- ;

/*---------------------------------------------- Обновление картинки */

                       this->kernel->vShow("REFRESH") ;

/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

   return(0) ;
}

/********************************************************************/
/*								    */
/*		      Реализация инструкции All                     */

  int  RSS_Module_Main::cAll(char *cmd)

{
/*-------------------------------------- Дешифровка командной строки */

/*----------------------------------------------- Выполнение диалога */

    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_OBJECTS_LIST", RT_DIALOG),
			   GetActiveWindow(), Main_ObjectsList_dialog) ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции EYE                     */
/*								    */
/*         EYE    <X> <Y> <Z>                                       */
/*         EYE/X  <X>                                               */
/*         EYE/+X <X>                                               */
/*         EYE>   <Код стрелочки> <Шаг>                             */
/*         EYE>>  <Код стрелочки> <Шаг>                             */

  int  RSS_Module_Main::cLookPoint(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX   4

             char  *pars[_PARS_MAX] ;
             char **xyz ;
           double   coord[_COORD_MAX] ;
              int   coord_cnt ;
           double   inverse ;
              int   xyz_flag ;          /* Флаг режима одной координаты */
              int   xyz_idx ;
              int   delta_flag ;        /* Флаг режима приращений */
              int   arrow_flag ;        /* Флаг стрелочного режима */
             char  *arrows ;
           double   look_x ;
           double   look_y ;
           double   look_z ;
           double   look_a ;
           double   look_e ;
           double   look_r ;
         Matrix2d   Sum_Matrix ;
         Matrix2d   Oper_Matrix ;  
         Matrix2d   Point_Matrix ;
             char  *error ;
             char  *end ;
              int   status ;
              int   i ;

/*---------------------------------------- Разборка командной строки */
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
                        xyz_flag=0 ;
                      delta_flag=0 ;
                      arrow_flag=0 ;

       if(*cmd=='/' ||
          *cmd=='+'   ) {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("Некорректный формат команды") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, '+')!=NULL   )  delta_flag= 1 ;

                if(strchr(cmd, 'x')!=NULL ||
                   strchr(cmd, 'X')!=NULL   )    xyz_flag='X' ;
           else if(strchr(cmd, 'y')!=NULL ||
                   strchr(cmd, 'Y')!=NULL   )    xyz_flag='Y' ;
           else if(strchr(cmd, 'z')!=NULL ||
                   strchr(cmd, 'Z')!=NULL   )    xyz_flag='Z' ;

                           cmd=end+1 ;
                        }

  else if(*cmd=='>') {
                           delta_flag=1 ;
                           arrow_flag=1 ;

                          cmd=strchr(cmd, ' ') ;
                       if(cmd==NULL)  return(0) ;
                          cmd++ ;
                     } 
/*- - - - - - - - - - - - - - - - - - - - - - - -  Разбор параметров */        
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

                      xyz=&pars[0] ;   
                  inverse=   1. ; 

/*------------------------------------- Обработка стрелочного режима */

    if(arrow_flag) {                        
                         arrows=pars[0] ;

      if(strstr(arrows, "left" )!=NULL   ) {  xyz_idx=0 ;  inverse=-1. ;  }
      if(strstr(arrows, "right")!=NULL   ) {  xyz_idx=0 ;  inverse= 1. ;  }  
      if(strstr(arrows, "up"   )!=NULL &&
         strstr(arrows, "ctrl" )==NULL   ) {  xyz_idx=1 ;  inverse= 1. ;  }
      if(strstr(arrows, "down" )!=NULL &&
         strstr(arrows, "ctrl" )==NULL   ) {  xyz_idx=1 ;  inverse=-1. ;  }
      if(strstr(arrows, "up"   )!=NULL &&
         strstr(arrows, "ctrl" )!=NULL   ) {  xyz_idx=2 ;  inverse= 1. ;  }
      if(strstr(arrows, "down" )!=NULL &&
         strstr(arrows, "ctrl" )!=NULL   ) {  xyz_idx=2 ;  inverse=-1. ;  }
           
                          xyz=&pars[1] ;
                   }
/*------------------------------------------------- Разбор координат */

    for(i=0 ; xyz[i]!=NULL && i<_COORD_MAX ; i++) {

             coord[i]=strtod(xyz[i], &end) ;
        if(*end!=0) {  
                       SEND_ERROR("Некорректное значение координаты") ;
                                       return(-1) ;
                    }
                                                  }

                             coord_cnt=  i ;

                                 error= NULL ;
      if(arrow_flag ||
           xyz_flag   ) {
               if(coord_cnt==0)  error="Не указана координата или ее приращение" ;
                        }
      else              {
               if(coord_cnt <3)  error="Должно быть указаны 3 координаты" ;
                        }

      if(error!=NULL)  SEND_ERROR(error) ;

      if(arrow_flag && coord_cnt>1)  coord[0]=coord[coord_cnt-1] ;  /* Для стрелочного режима берем в качестве шага */
                                                                    /*  самое последнее значение                    */
/*-------------------------------------- Получение параметров камеры */

       status=EmiRoot_look("Main", "Get", &look_x, &look_y, &look_z,
                                          &look_a, &look_e, &look_r ) ;
    if(status) {
                   SEND_ERROR("Неизвестное окно") ;
                      return(-1) ;
               }

/*--------------------- Определение проекций стрелочных перемещений  */

               Point_Matrix.LoadZero   (3, 1) ;
               Point_Matrix.SetCell    (xyz_idx, 0, inverse*coord[0]) ;

                 Sum_Matrix.Load3d_azim( look_a) ;
                Oper_Matrix.Load3d_elev( look_e) ;
                 Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
                Oper_Matrix.Load3d_roll( look_r) ;
                 Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
               Point_Matrix.LoadMul    (&Sum_Matrix, &Point_Matrix) ;

/*------------------------------------------------ Пропись координат */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - -  Стрелочки */
        if(arrow_flag) {
                           look_x+=Point_Matrix.GetCell(0, 0) ;
                           look_y+=Point_Matrix.GetCell(1, 0) ;
                           look_z+=Point_Matrix.GetCell(2, 0) ;
                       }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - Приращения */
   else if(delta_flag) {

              if(xyz_flag=='X')   look_x+=inverse*coord[0] ;
         else if(xyz_flag=='Y')   look_y+=inverse*coord[0] ;                 
         else if(xyz_flag=='Z')   look_z+=inverse*coord[0] ;
                       }
/*- - - - - - - - - - - - - - - - - - - - - - -  Абсолютные значения */
   else                {

              if(xyz_flag=='X')   look_x=coord[0] ;
         else if(xyz_flag=='Y')   look_y=coord[0] ;                 
         else if(xyz_flag=='Z')   look_z=coord[0] ;
         else                   {
                                  look_x=coord[0] ;
                                  look_y=coord[1] ;
                                  look_z=coord[2] ;
                                }
                       }
/*-------------------------------------- Установка параметров камеры */

       status=EmiRoot_look("Main", "Set", &look_x, &look_y, &look_z,
                                             NULL,    NULL,    NULL ) ;
    if(status) {
                    return(-1) ;
               }
/*------------------------------------------------------ Перерисовка */

            EmiRoot_redraw("Main") ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции LOOK                    */
/*								    */
/*        LOOK    <A> <E> <R>                                       */
/*        LOOK/A  <A>                                               */
/*        LOOK/+A <A>                                               */
/*        LOOK>   <Код стрелочки> <Шаг>                             */
/*        LOOK>>  <Код стрелочки> <Шаг>                             */

  int  RSS_Module_Main::cLookDirection(char *cmd) 

{
#define  _COORD_MAX   3
#define   _PARS_MAX   4

             char  *pars[_PARS_MAX] ;
             char **xyz ;
           double   coord[_COORD_MAX] ;
              int   coord_cnt ;
           double   inverse ;
              int   xyz_flag ;          /* Флаг режима одной координаты */
              int   delta_flag ;        /* Флаг режима приращений */
              int   arrow_flag ;        /* Флаг стрелочного режима */
             char  *arrows ;
           double   look_a ;
           double   look_e ;
           double   look_r ;
             char  *error ;
             char  *end ;
              int   status ;
              int   i ;

/*---------------------------------------- Разборка командной строки */
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
                        xyz_flag=0 ;
                      delta_flag=0 ;
                      arrow_flag=0 ;

       if(*cmd=='/' ||
          *cmd=='+'   ) {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("Некорректный формат команды") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, '+')!=NULL   )  delta_flag= 1 ;

                if(strchr(cmd, 'a')!=NULL ||
                   strchr(cmd, 'A')!=NULL   )    xyz_flag='A' ;
           else if(strchr(cmd, 'e')!=NULL ||
                   strchr(cmd, 'E')!=NULL   )    xyz_flag='E' ;
           else if(strchr(cmd, 'r')!=NULL ||
                   strchr(cmd, 'R')!=NULL   )    xyz_flag='R' ;

                           cmd=end+1 ;
                        }

  else if(*cmd=='>') {
                           delta_flag=1 ;
                           arrow_flag=1 ;

                          cmd=strchr(cmd, ' ') ;
                       if(cmd==NULL)  return(0) ;
                          cmd++ ;
                     } 
/*- - - - - - - - - - - - - - - - - - - - - - - -  Разбор параметров */        
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

                      xyz=&pars[0] ;   
                  inverse=   1. ; 

/*------------------------------------- Обработка стрелочного режима */

    if(arrow_flag) {                        
                         arrows=pars[0] ;

      if(strstr(arrows, "left" )!=NULL &&
         strstr(arrows, "ctrl" )==NULL   ) {  xyz_flag='A' ;  inverse=-1. ;  }
      if(strstr(arrows, "right")!=NULL &&  
         strstr(arrows, "ctrl" )==NULL   ) {  xyz_flag='A' ;  inverse= 1. ;  }  

      if(strstr(arrows, "up"   )!=NULL   ) {  xyz_flag='E' ;  inverse= 1. ;  }
      if(strstr(arrows, "down" )!=NULL   ) {  xyz_flag='E' ;  inverse=-1. ;  }

      if(strstr(arrows, "left" )!=NULL &&
         strstr(arrows, "ctrl" )!=NULL   ) {  xyz_flag='R' ;  inverse=-1. ;  }
      if(strstr(arrows, "right")!=NULL &&  
         strstr(arrows, "ctrl" )!=NULL   ) {  xyz_flag='R' ;  inverse= 1. ;  }  
           
                          xyz=&pars[1] ;   
                   }
/*------------------------------------------------- Разбор координат */

    for(i=0 ; xyz[i]!=NULL && i<_COORD_MAX ; i++) {

             coord[i]=strtod(xyz[i], &end) ;
        if(*end!=0) {  
                       SEND_ERROR("Некорректное значение координаты") ;
                                       return(-1) ;
                    }
                                                  }

                             coord_cnt=  i ;

                                 error= NULL ;
      if(xyz_flag) {
               if(coord_cnt==0)  error="Не указана координата или ее приращение" ;
                   }
      else         {
               if(coord_cnt <3)  error="Должно быть указаны 3 координаты" ;
                   }

      if(error!=NULL)  SEND_ERROR(error) ;

      if(arrow_flag && coord_cnt>1)  coord[0]=coord[coord_cnt-1] ;  /* Для стрелочного режима берем в качестве шага */
                                                                    /*  самое последнее значение                    */
/*-------------------------------------- Получение параметров камеры */

       status=EmiRoot_look("Main", "Get",    NULL,    NULL,    NULL, 
                                          &look_a, &look_e, &look_r ) ;
    if(status) {
                   SEND_ERROR("Неизвестное окно") ;
                      return(-1) ;
               }
/*------------------------------------------------ Пропись координат */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - Приращения */
   if(delta_flag) {

          if(xyz_flag=='A')   look_a+=inverse*coord[0] ;
     else if(xyz_flag=='E')   look_e+=inverse*coord[0] ;                 
     else if(xyz_flag=='R')   look_r+=inverse*coord[0] ;
                  }
/*- - - - - - - - - - - - - - - - - - - - - - -  Абсолютные значения */
   else           {

          if(xyz_flag=='A')   look_a=coord[0] ;
     else if(xyz_flag=='E')   look_e=coord[0] ;                 
     else if(xyz_flag=='R')   look_r=coord[0] ;
     else                   {
                              look_a=coord[0] ;
                              look_e=coord[1] ;
                              look_r=coord[2] ;
                            }
                  }
/*- - - - - - - - - - - - - - - - - - - - - -  Нормализация значений */
     while(look_a> 180.)  look_a-=360. ;
     while(look_a<-180.)  look_a+=360. ;

     while(look_e> 180.)  look_e-=360. ;
     while(look_e<-180.)  look_e+=360. ;

     while(look_r> 180.)  look_r-=360. ;
     while(look_r<-180.)  look_r+=360. ;

/*-------------------------------------- Установка параметров камеры */

       status=EmiRoot_look("Main", "Set",    NULL,    NULL,    NULL, 
                                          &look_a, &look_e, &look_r ) ;
    if(status) {
                    return(-1) ;
               }
/*------------------------------------------------------ Перерисовка */

            EmiRoot_redraw("Main") ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции ZOOM                    */
/*								    */
/*        ZOOM    <Ширина поля зрения>                              */
/*        ZOOM+                                                     */
/*        ZOOM-                                                     */

  int  RSS_Module_Main::cLookZoom(char *cmd) 

{
              int   delta_flag ;        /* Флаг режима приращений */
           double   par ;
           double   zoom ;
             char  *end ;
              int   status ;

/*---------------------------------------- Разборка командной строки */
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
      if(*cmd=='-' ||
         *cmd=='+'   ) {
                          if(*cmd=='+')  par=1./1.5 ;
                          if(*cmd=='-')  par=   1.5 ;

                                delta_flag=1 ;
                       }
/*- - - - - - - - - - - - - - - - - - - - - - - -  Разбор параметров */
     else              {
            par=strtod(cmd, &end) ;
         if(par<=0. || *end!=0 ) {  
                       SEND_ERROR("Некорректное значение") ;
                                       return(-1) ;
                                 }

                                delta_flag=0 ;
                       } 
/*-------------------------------------- Получение параметров камеры */

       status=EmiRoot_zoom("Main", "Get", &zoom) ;
    if(status) {
                   SEND_ERROR("Неизвестное окно") ;
                      return(-1) ;
               }
/*------------------------------------------------ Пропись координат */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - Приращения */
   if(delta_flag) {
                        zoom*=par ;
                  }
/*- - - - - - - - - - - - - - - - - - - - - - -  Абсолютные значения */
   else           {
                        zoom =par ;
                  }
/*-------------------------------------- Установка параметров камеры */

       status=EmiRoot_zoom("Main", "Set", &zoom) ;
    if(status) {
                    return(-1) ;
               }
/*------------------------------------------------------ Перерисовка */

                   this->vShow("ZOOM") ;

                    EmiRoot_redraw("Main") ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции LOOKAT                  */

  int  RSS_Module_Main::cLookAt(char *cmd) 

{

/*-------------------------------------------------- Пропись на окно */

                    EmiRoot_lookat("Main", "Set", cmd) ;

/*------------------------------------------------------ Перерисовка */

                    EmiRoot_redraw("Main") ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции LOOKINFO                */

  int  RSS_Module_Main::cLookInfo(char *cmd) 

{
        int   all_flag ;
     double   look_x ;
     double   look_y ;
     double   look_z ;
     double   look_a ;
     double   look_e ;
     double   look_r ;
     double   zoom ;
       char   look_at[128] ;
       char   text[4096] ;
        int   status ;

/*------------------------------------------ Разбор командной строки */

                all_flag=0 ;

/*------------------------------------------------ Извлечение данных */

       status=EmiRoot_look("Main", "Get", &look_x, &look_y, &look_z,
                                          &look_a, &look_e, &look_r ) ;
    if(status) {
                   SEND_ERROR("Неизвестное окно") ;
                      return(-1) ;
               }

              EmiRoot_zoom  ("Main", "Get", &zoom) ;
              EmiRoot_lookat("Main", "Get",  look_at) ;

/*-------------------------------------------- Формирование описания */

      sprintf(text, "Window %s\r\n"
                    "From X % 7.3lf\r\n"
                    "     Y % 7.3lf\r\n"
                    "     Z % 7.3lf\r\n"
                    "Ang. A % 7.3lf\r\n"
                    "     E % 7.3lf\r\n"
                    "     R % 7.3lf\r\n"
                    "Zoom   % 7.3lf\r\n"
                    "LookAt %s\r\n"
                    "\r\n",
                        "Main",
                         look_x, look_y, look_z,
                         look_a, look_e, look_r,
                         zoom  , look_at
                    ) ;

/*-------------------------------------------------- Выдача описания */
/*- - - - - - - - - - - - - - - - - - - - - - - - - В отдельное окно */
     if(all_flag) {

                  }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - В главное окно */
     else         {

        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_SHOW_INFO, (LPARAM)text) ;
                  }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции READ                    */

  int  RSS_Module_Main::cRead(char *cmd)

{
   OPENFILENAME  file_choice ;
           char *dat_filter="Все файлы\0*.*\0"
                            "Файлы объектов\0*.sob\0"
                            "Файлы сохранения\0*.sav\0"
                            "\0\0" ;
           char  path[FILENAME_MAX] ;
           FILE *file ;
    std::string  buff ;
           char  text[4096] ;
            int  section_flag ;
            int  n_sect ;
            int  status ;
           char *end ;
            int  i ;

/*------------------------------------------------- Разборка команды */

            sprintf(text, "RSS_Module_Main::cRead.0 heap=%d", _heapchk()) ;
             iDebug(text, NULL) ;

      strncpy(path, cmd, sizeof(path)-1) ;

/*---------------------------------------------- Задание имени файла */

    if(path[0]==0) {

           memset( path, 0, sizeof(path)) ;

	   memset(&file_choice, 0, sizeof(file_choice))  ;
  		   file_choice.lStructSize =sizeof(file_choice) ;   /* Форм.описание диалога */
		   file_choice.hwndOwner   = NULL ;
		   file_choice.hInstance   =GetModuleHandle(NULL) ;
		   file_choice.lpstrFilter =dat_filter ;
		   file_choice.nFilterIndex=  1  ;
  		   file_choice.lpstrFile   = path ;
		   file_choice.nMaxFile    =sizeof(path) ;
		   file_choice.lpstrTitle  ="Укажите файл с данными" ;
		   file_choice.Flags       = OFN_ENABLEHOOK   |
                                             OFN_FILEMUSTEXIST ;
		   file_choice.lpfnHook    = iEmiRoot_read_HookProc ;

  	  status=GetOpenFileName(&file_choice) ;	            /* Запрос пути */
                          _chdir(__cwd) ;

       if(status==0)  return(-1) ;                                  /* Если путь не выбран... */

                   }
/*--------------------------------------------------- Открытие файла */

       file=fopen(path, "rt") ;
    if(file==NULL) {
                       SEND_ERROR("Ошибка открытия файла") ;
                                       return(-1) ;
                   }

          sprintf(text, "cRead: %s", path) ;
           iDebug(text, NULL) ;

/*------------------------------------------------ Считывание данных */

                  section_flag=0 ;
                        n_sect=0 ;

     while(1) {                                                     /* CIRCLE.1 - Построчно читаем файл */
/*- - - - - - - - - - - - - - - - - - -  Считывание очередной строки */
                      memset(text, 0, sizeof(text)) ;
                   end=fgets(text, sizeof(text)-1, file) ;          /* Считываем строку */
                if(end==NULL)  break ;

            if(text[0]==';')  continue ;                            /* Проходим комментарий */

                end=text+strlen(text)-1 ;                           /* Удаляем символ конца строки */
            if(*end=='\n')  *end=0 ;

              iDebug(text, NULL) ;
/*- - - - - - - - - - - - - - - - - - - - - - -  Начало/конец секции */
            if(!memicmp(text, "#BEGIN ", strlen("#BEGIN "))) {

                                   if(n_sect==0) {  buff=text ; buff+="\n" ;  }
                                      n_sect++ ;
                                section_flag=1 ;
                                                             }
       else if(!memicmp(text, "#END",    strlen("#END"))   ) {

                                      n_sect-- ;
                                   if(n_sect==0)  section_flag=0 ;
                                                             }
       else if(section_flag                                ) {

                                        buff+=text ;
                                        buff+="\n" ;
                                           continue ;
                                                             }
/*- - - - - - - - - - - - - - - - - - - - -  Контроль полноты данных */
       if(n_sect!=0 || buff.empty())  continue ;

             memset(text, 0, sizeof(text)) ;
            strncpy(text, buff.c_str(), sizeof(text)-1) ;
/*- - - - - - - - - - - - - - - - - - - - - Обработка данных модулей */
       if(!memicmp(text, "#BEGIN MODULE ", 
                  strlen("#BEGIN MODULE "))) {

            this->kernel->vReadSave(&buff) ;                        /* Чтение данных ядра */

       for(i=0 ; i<this->kernel->modules_cnt ; i++)                 /* Чтение данных модулей */
            this->kernel->modules[i].entry->vReadSave(&buff) ;

                                                continue ;
                                             }
/*- - - - - - - - - - - - - - - - - - - -  Обработка данных объектов */
       if(!memicmp(text, "#BEGIN OBJECT ", 
                  strlen("#BEGIN OBJECT "))) {

       for(i=0 ; i<this->kernel->modules_cnt ; i++)                 /* Чтение данных объектов */
            this->kernel->modules[i].entry->vReadSave(&buff) ;

                                                continue ;
                                             }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
              }                                                     /* CONTINUE.1 */
/*--------------------------------------------------- Закрытие файла */

                fclose(file) ;

/*------------------------------------------------------ Перерисовка */

                this->cShowScene("") ;

/*-------------------------------------------------------------------*/

         sprintf(text, "RSS_Module_Main::cRead.E heap=%d", _heapchk()) ;
          iDebug(text, NULL) ;

   return(0) ;
}


  UINT CALLBACK  iEmiRoot_read_HookProc(  HWND hDlg,     UINT uiMsg, 
                                    WPARAM wParam, LPARAM lParam )
{
   if(uiMsg==WM_INITDIALOG) {
      SetWindowPos(hDlg, HWND_TOPMOST, 0, 0, 0, 0,
                              SWP_NOMOVE | SWP_NOSIZE) ;
        ShowWindow(hDlg, SW_SHOWNORMAL) ;
                            }
  return(FALSE) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции WRITE                   */
/*								    */
/*	WRITE [<Имя файла>]                                         */
/*	WRITE/O [<Имя файла>] <Имя объекта 1> ... <Имя объекта N>   */

  int  RSS_Module_Main::cWrite(char *cmd) 

{
   OPENFILENAME  file_choice ;
           char *dat_filter="Все файлы\0*.*\0"
                            "Файлы объектов\0*.sob\0"
                            "Файлы сохранения\0*.sav\0"
                            "\0\0" ;
           char  path[FILENAME_MAX] ;
           char *objects[64] ;
           FILE *file ;
    std::string  buff ;
            int  status ;
     const char *data ; 
           char *end ; 
            int  i ;
            int  j ;

/*------------------------------------------------- Разборка команды */

                    memset(objects, 0, sizeof(objects)) ;

   if(*cmd=='/') {
                   end=strchr(cmd, ' ') ;
                if(end==NULL) {  
                       SEND_ERROR("Некорректный формат команды") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(*cmd!='o' ||
                   *cmd!='O'   ) {
                       SEND_ERROR("Неизвестный ключ команды") ;
                                       return(-1) ;
                                 }

                    cmd=end+1 ;
                   end=strchr(cmd, ' ') ;
                if(end==NULL) {  
                       SEND_ERROR("Некорректный формат команды") ;
                                       return(-1) ;
                              }
                   *end=0 ;

                     strncpy(path, cmd, sizeof(path)-1) ;           /* Извлечение пути файла */

              for(cmd=end+1, i=0 ; 
                      end!=NULL && i<64 ; cmd=end+1, i++) {
                             objects[i]=cmd ;
                                    end=strchr(cmd, ' ') ;
                    if(end!=NULL)  *end=0 ;
                                                          }
                 }
   else          {
                     strncpy(path, cmd, sizeof(path)-1) ;
                 }
/*---------------------------------------------- Задание имени файла */

    if(path[0]==0) {

           memset( path, 0, sizeof(path)) ;

	   memset(&file_choice, 0, sizeof(file_choice))  ;
  		   file_choice.lStructSize =sizeof(file_choice) ;   /* Форм.описание диалога */
		   file_choice.hwndOwner   = NULL ;
		   file_choice.hInstance   =GetModuleHandle(NULL) ;
		   file_choice.lpstrFilter =dat_filter ;
		   file_choice.nFilterIndex=  1  ;
  		   file_choice.lpstrFile   = path ;
		   file_choice.nMaxFile    =sizeof(path) ;
		   file_choice.lpstrTitle  ="Укажите файл для сохранения" ;
		   file_choice.Flags       = OFN_ENABLEHOOK ;
		   file_choice.lpfnHook    = iEmiRoot_read_HookProc ;

  	  status=GetOpenFileName(&file_choice) ;	            /* Запрос пути */
                          _chdir(__cwd) ;

       if(status==0)  return(-1) ;                                  /* Если путь не выбран... */

                   }
/*--------------------------------------------------- Открытие файла */

       file=fopen(path, "wt") ;
    if(file==NULL) {
                       SEND_ERROR("Ошибка открытия файла") ;
                                       return(-1) ;
                   }
/*---------------------------------------------------- Запись данных */
/*- - - - - - - - - - - - - - - - - - - - - - - Полная запись данных */
    if(objects[0]==NULL) {

            this->kernel->vWriteSave(&buff) ;                       /* Запись данных ядра */

                    data=buff.c_str() ;       
             fwrite(data, 1, strlen(data), file) ;
          if(ferror(file)) {
                       SEND_ERROR("Ошибка записи файла") ;
                                       return(-1) ;
                           }

       for(i=0 ; i<this->kernel->modules_cnt ; i++) {               /* Запись данных модулей */

            this->kernel->modules[i].entry->vWriteSave(&buff) ;
        
            data=buff.c_str() ;
         if(data==NULL || strlen(data)<10)  continue ;
       
             fwrite(data, 1, strlen(data), file) ;
          if(ferror(file)) {
                       SEND_ERROR("Ошибка записи файла") ;
                                       return(-1) ;
                           }
                                                    }

      if(this->kernel->kernel_objects)
       for(i=0 ; i<this->kernel->kernel_objects_cnt ; i++) {        /* Запись данных объектов */

            this->kernel->kernel_objects[i]->vWriteSave(&buff) ;
        
            data=buff.c_str() ;
         if(data==NULL || strlen(data)<10)  continue ;
       
             fwrite(data, 1, strlen(data), file) ;
          if(ferror(file)) {
                       SEND_ERROR("Ошибка записи файла") ;
                                       return(-1) ;
                           }
                                                           }
         
                         }
/*- - - - - - - - - - - - - - - Запись данных для выбранных объектов */
    else                 {

      if(this->kernel->kernel_objects)
       for(i=0 ; i<this->kernel->kernel_objects_cnt ; i++) {        /* Запись данных объектов */

         for(j=0 ; objects[j]!=NULL ; j++)                          /* Проверяем объект по списку записи */
           if(!stricmp(objects[j], 
                        this->kernel->kernel_objects[i]->Name))  break ;

           if(objects[j]==NULL)  continue ;                         /* Если объект не сохраняем... */

            this->kernel->kernel_objects[i]->vWriteSave(&buff) ;
        
            data=buff.c_str() ;
         if(data==NULL || strlen(data)<10)  continue ;
       
             fwrite(data, 1, strlen(data), file) ;
          if(ferror(file)) {
                       SEND_ERROR("Ошибка записи файла") ;
                                       return(-1) ;
                           }
                                                           }
                         }
/*--------------------------------------------------- Закрытие файла */

              fclose(file) ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции THREADS                 */

  int  RSS_Module_Main::cThreads(char *cmd)

{ 
      EmiRoot_threads("SHOW", NULL) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции STOP                    */

  int  RSS_Module_Main::cStop(char *cmd)

{ 
      EmiRoot_threads("STOP", NULL) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции MODULES                 */

  int  RSS_Module_Main::cModules(char *cmd)

{ 
    EmiRoot_modules("CREATE") ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции MEMORY                  */

  int  RSS_Module_Main::cMemory(char *cmd)

{ 
/*---------------------------------------- Запуск регистрации памяти */

   if(!stricmp(cmd, "ON"  )) {
                                gMemOnOff(_MEMCHECK_ACTUAL) ;
                             }
/*---------------------------------------- Формирование дампа памяти */

   if(!stricmp(cmd, "LIST")) {
                                gMemList("memory.list") ;
                             }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*                   Реализация инструкции SRAND                    */

  int  RSS_Module_Main::cSrand(char *cmd)

{ 
   char *end ;
   long  value ;

/*------------------------------------------------------- Подготовка */

       value=strtoul(cmd, &end, 10) & 0x00007fffL ;

/*--------------------------------- Перевод старт-точки в авто-режим */

   if(!stricmp(cmd, "AUTO"  )) {
                                  this->srand_fixed=0 ;

                                    srand(time(NULL)) ;
                               }
/*----------------------------------- Задание страт-точки генератора */

   else
   if(value>1) {
                                  this->srand_fixed=1 ;
                                    srand(value) ;
               }
/*---------------------------------- Обработка некорректного формата */

   else        {
                       SEND_ERROR("Некорректный формат команды") ;
               }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*                    Отображение данных                            */

    void  RSS_Module_Main::ShowExecute(char *layer)

{
   int  status ;
   int  i ;

/*-------------------------------------------- Формирование объектов */

#define   MODULES       this->kernel->modules 
#define   MODULES_CNT   this->kernel->modules_cnt 

  if(layer==NULL) {

    for(i=0 ; i<MODULES_CNT ; i++) MODULES[i].entry->vShow("SCENE") ;
    for(i=0 ; i<MODULES_CNT ; i++) MODULES[i].entry->vShow("INDICATORS") ;
    for(i=0 ; i<MODULES_CNT ; i++) MODULES[i].entry->vShow("MARKERS") ;
                  }
  else            {

    for(i=0 ; i<MODULES_CNT ; i++) MODULES[i].entry->vShow(layer) ;
                  }
#undef    MODULES
#undef    MODULES_CNT

/*--------------------------------------------- Отображения объектов */

          status=this->display.SetFirstContext(NULL) ;
    while(status==0) {
             
                 this->display.ShowContext(NULL) ;
          status=this->display.SetNextContext(NULL) ;
                     }
/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*                   Отладочная печать в файла                      */

   void  RSS_Module_Main::iDebug(char *text, char *path,  int  condition)
{
    if(condition)  iDebug(text, path) ;
}


   void  RSS_Module_Main::iDebug(char *text, char *path)
{
   static int  init_flag ;
         FILE *file ;         


    if(path==NULL) {
                             path="m_main.log" ;
      if(!init_flag)  unlink(path) ;
          init_flag=1 ;
                   }

       file=fopen(path, "at") ;
    if(file==NULL)  return ;

           fwrite(text, 1, strlen(text), file) ;
           fwrite("\n", 1, strlen("\n"), file) ;
           fclose(file) ;
}



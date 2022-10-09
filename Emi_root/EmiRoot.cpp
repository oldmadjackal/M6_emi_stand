/*********************************************************************/
/*                                                                   */
/*         ������� ������������ 2D-������������� �������������       */
/*                                                                   */
/*********************************************************************/

#include <windows.h>
#include <process.h>
#include <malloc.h>
#include <io.h>
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

/*--------------------------------------------- ��������� ���������� */

        RSS_Module_Main  Kernel ;    /* ��������� ���� */

#define  SEND_ERROR(text)      SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,        \
                                         (LPARAM) text)
#define  SHOW_COMMAND(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_SHOW_COMMAND,         \
                                         (LPARAM) text)
#define  SEND_CHECK(text)      SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_CHECK_MESSAGE,        \
                                         (LPARAM) text)

/*-------------------------------------- ��������� ��������� ������� */

  static  HINSTANCE  hInst ;
  static   WNDCLASS  FrameWindow ;
  static       HWND  hFrameWindow ;
  static       HWND  hDialog ;

#define  SETs(elm, s)  SetDlgItemText(hDlg, elm, s)
#define  GETs(elm, s)  GetDlgItemText(hDlg, elm, s, 512)

#define  ITEM(e)  GetDlgItem(hDlg, e)

/*------------------------------------------ ����� ���������� ������ */

       DWORD WINAPI  Command_Thread(LPVOID) ;

             HANDLE  hCommandEnd ;                    /* ������� - ���������� ������� */
                int  WaitCommandEnd ;                 /* ���� �������� ��������� ������� */

/*------------------------------------ ����������� ��������� ������� */

  union WndProc_par {
                      LONG_PTR            par ;
                       LRESULT (CALLBACK *call)(HWND, UINT, WPARAM, LPARAM) ; 
                    } ;

  static union WndProc_par  Cmd_WndProc ;
  static union WndProc_par  Tmp_WndProc ;

   LRESULT CALLBACK  EmiRoot_Cmd_WndProc(HWND, UINT, WPARAM, LPARAM) ;

/*------------------------------------ ��������� ��������� ��������� */

   LRESULT CALLBACK  EmiRoot_window_processor(HWND, UINT, WPARAM, LPARAM) ;
   INT_PTR CALLBACK  EmiRoot_main_dialog     (HWND, UINT, WPARAM, LPARAM) ;

  UINT_PTR CALLBACK  iEmiRoot_read_HookProc  (HWND, UINT, WPARAM, LPARAM) ;


/*********************************************************************/
/*                                                                   */
/*                               MAIN                                */

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
   char  ClassName[512] ;
    HDC  hDCScreen ;
    MSG  SysMessage ;
   char  text[512] ;
    int  init_flag ;
    int  i ;

/*------------------------------------- ����������� �������� ������� */

                   getcwd(__cwd, sizeof(__cwd)) ;

/*------------------------------- ���������� ��������� ������������� */

    hCommandEnd=OpenEvent(EVENT_ALL_ACCESS, false, "CommandEnd") ;  /* ��������� ������� */ 

/*---------------------------------------- ��������� ������� ������� */

	    Kernel.keyword="EmiStand" ;

	    Kernel.SetKernel(NULL, hInstance) ;
  	    Kernel.Load     ("") ;

         if(Kernel.modules_cnt==0)
                      EmiRoot_error("No active modules loaded", MB_ICONERROR) ;

       for(i=0 ; i<Kernel.modules_cnt ; i++)
                     Kernel.modules[i].entry->vStart() ;

       for(i=0 ; i<Kernel.modules_cnt ; i++)
                     Kernel.modules[i].entry->vInit() ;

/*------------------------------------------- ���������� ����������� */

                      UD_init() ;                                   /* ����.������� ������� */

                      EmiRoot_show_init() ;                         /* ������������� ������� ����������� */

      RSS_Kernel::display.GetList        =EmiRoot_show_getlist ;    /* ������� ������� ���������� */
      RSS_Kernel::display.ReleaseList    =EmiRoot_show_releaselist ;        
      RSS_Kernel::display.SetFirstContext=EmiRoot_first_context ;
      RSS_Kernel::display.SetNextContext =EmiRoot_next_context ;
      RSS_Kernel::display.ShowContext    =EmiRoot_show_context ;
      RSS_Kernel::display.GetContextPar  =EmiRoot_get_context ;

/*----------------------------------- ����������� ����������� ������ */

                                          hDCScreen=GetDC(NULL); 

      RSS_Kernel::display.x=GetDeviceCaps(hDCScreen, HORZRES) ;
      RSS_Kernel::display.y=GetDeviceCaps(hDCScreen, VERTRES) ;
                                ReleaseDC(NULL, hDCScreen)   ;

/*---------------------------------------------------- ������������� */

                      hInst=hInstance ;

      RSS_Kernel::show_time_step=0.1 ;
      RSS_Kernel::calc_time_step=0.1 ;

/*------------------------------- ����������� ������ ���������� ���� */

      sprintf(ClassName, "EmiRoot_Frame_%d", _getpid()) ;

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
/*----------------------------------------- �������� ���������� ���� */

    hFrameWindow=CreateWindow(ClassName, "EmiRoot", 
			      WS_OVERLAPPEDWINDOW | WS_DLGFRAME,
			      CW_USEDEFAULT, CW_USEDEFAULT, 
			      CW_USEDEFAULT, CW_USEDEFAULT,
				       NULL, NULL, 
				  hInstance, NULL ) ;

/*---------------------------------------- �������� ����������� ���� */

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

/*-------------------------------- �������� �������� ������� ������� */

             EmiRoot_threads("CREATE", NULL) ;

/*------------------------------------------ ������� ���������� ���� */

                               init_flag=1 ;

        while(GetMessage(&SysMessage, NULL, 0, 0)) {

          if(init_flag) {
                               init_flag=0 ;
                           SendMessage(hDialog, WM_USER,
                                         (WPARAM)_USER_EXECUTE_START, (LPARAM)NULL) ;
                        } 
          
                       TranslateMessage(&SysMessage) ;
                        DispatchMessage(&SysMessage) ;
	                                           }
/*------------------------------------------------ ���������� ������ */

        UnregisterClass(ClassName, GetModuleHandle(NULL)) ;

            CloseHandle(hCommandEnd) ;

/*-------------------------------------------------------------------*/

	return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                 ��������� ��������� ���������                     */

  int  EmiRoot_system(void)

{
      MSG  SysMessage ;

/*------------------------------------ ��������� ��������� ��������� */

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
/*		���������� ��������� "���������" ����		     */

 LRESULT CALLBACK  EmiRoot_window_processor(  HWND  hWnd,     UINT  Msg,
 			                    WPARAM  wParam, LPARAM  lParam)
{
  HWND  hChild ;
  RECT  Rect ;


  switch(Msg) {

/*---------------------------------------------------- �������� ���� */

    case WM_CREATE:   break ;

/*------------------------------------------------ ��������� ������� */

    case WM_COMMAND: {
			      return(FALSE) ;
  			           break ;
                     } 

/*------------------------------------------------- ��������� ������ */

    case WM_SETFOCUS:  {
                          SetFocus(GetDlgItem(hDialog, IDC_COMMAND)) ;

			      return(FALSE) ;
  			           break ;
  		       }
/*------------------------------------------------ ��������� ������� */

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
/*---------------------------------------------------- �������� ���� */
 
    case WM_DESTROY: {

               if(hWnd!=hFrameWindow)  break ;
                
			PostQuitMessage(0) ;  
			     break ;
		     }
/*------------------------------------------- ��������� ��-��������� */

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
/*	   ���������� ��������� ����������� ���� VIEW	             */	

  INT_PTR CALLBACK  EmiRoot_main_dialog(  HWND  hDlg,     UINT  Msg, 
                                        WPARAM  wParam, LPARAM  lParam) 
{
                 HWND  hPrn ;
                 RECT  wr ;      /* ������� ���� */
	          int  x_screen ;    /* ������� ������ */	
                  int  y_screen ;
                  int  x_shift ;     /* ���������� ����� */	
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
                HFONT  font ;           /* ����� */
                  int  elm ;            /* ������������� �������� ������� */
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

/*------------------------------------------------- ������� �������� */

  switch(Msg) {

/*---------------------------------------------------- ������������� */

    case WM_INITDIALOG: {
/*- - - - - - - - - - - - - - - - - - ���� ��������������� ��������� */
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
/*- - - - - - - - - - - - -  �������� ��������� � �������� ��������� */
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
/*- - - - - - - - - - - - - - - - - - - ������������� ��������� ���� */
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
/*- - - - - - - - - - - - - - - - -  �������� ������������ ��������� */
       Tmp_WndProc.call=EmiRoot_Cmd_WndProc ;
       Cmd_WndProc.par =GetWindowLongPtr(ITEM(IDC_COMMAND), GWLP_WNDPROC) ;
                        SetWindowLongPtr(ITEM(IDC_COMMAND), GWLP_WNDPROC,
                                                            Tmp_WndProc.par) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - -  ������� ������� */
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
/*- - - - - - - - - - - - - - - - -  ������������� ������� ��������� */
//          SETs(IDC_COMMAND, "@Tests\\rogov_search.emi") ;
/*- - - - - - - - - - - - - - - - - - - - - - - ������������� ������ */
                          SetFocus(ITEM(IDC_COMMAND)) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}
/*------------------------------------------------ ��������� ������� */

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
/*------------------------------------ ��������� ���������� �������� */

    case WM_USER:  {
/*- - - - - - - - - - - - - - - - - - - - - - - - ����������� ������ */
        if(wParam==_USER_SHOW) {

                    Kernel.ShowExecute((char *)lParam) ;

                                    return(FALSE) ;
                               }
/*- - - - - - - - - - - - -  ���������� � ��������� ������ ��������� */
        if(wParam==_USER_CHANGE_CONTEXT) {

                  ((RSS_Transit *)lParam)->vExecute() ;

                                            return(FALSE) ;
                                         }
/*- - - - - - - - - - - - - - ���������� ���������� ���������� ����� */
        if(wParam==_USER_EXECUTE_START ) {

           if(_access("start.emi", 0x00)==0) 
                          EmiRoot_command_start("@start.emi") ;

                             return(FALSE) ;
                                          }
/*- - - - - - - - - - - - - - - - - "�������" �������������� ������� */
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
/*- - - - - - - - - - - - - - - -  ������� �������� ��������� ������ */
        if(wParam==_USER_COMMAND_PREFIX) {

                            SETs(IDC_CMD_PREFIX, (char *)lParam) ;
                			      return(FALSE) ;
                                         }
/*- - - - - - - - - - - - - - - -  ������� ������ ������� ���������� */
        if(wParam==_USER_DIRECT_COMMAND) {

                       strcpy(__direct_command, (char *)lParam) ;
                			      return(FALSE) ;
                                         }
/*- - - - - - - - - - - - - - - - - - - ������� ������� �� ��������� */
        if(wParam==_USER_DEFAULT_OBJECT) {

                strncpy(__object_def, (char *)lParam, sizeof(__object_def)-1) ;
                			      return(FALSE) ;
                                         }
/*- - - - - - - - - - - - - - - - - - - - - - -  ��������� �� ������ */
        if(wParam==_USER_ERROR_MESSAGE) {

                      EmiRoot_error((char *)lParam, MB_ICONERROR) ;
                			      return(FALSE) ;
                                        }
/*- - - - - - - - - - - - - - - - - - - - - - -  ��������� �� ������ */
        if(wParam==_USER_INFO_MESSAGE) {

                      EmiRoot_error((char *)lParam, MB_ICONINFORMATION) ;
                			      return(FALSE) ;
                                       }
/*- - - - - - - - - - - - - - - - - - - - - - -  ����������� ������� */
        if(wParam==_USER_SHOW_COMMAND) {

                            SETs(IDC_COMMAND, (char *)lParam) ;
                                             return(FALSE) ;
                                       }
/*- - - - - - - - - - - - - - - - - - - - - - - ��������� �� ������� */
        if(wParam==_USER_SHOW_INFO) {

                            SETs(IDC_STATUS_INFO, (char *)lParam) ;
                                             return(FALSE) ;
                                    }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			  return(FALSE) ;
  			     break ;
  		   }
/*------------------------------------------------ ��������� ������� */

    case WM_COMMAND:    {

	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			  return(FALSE) ;
			     break ;
			}
/*--------------------------------------------------------- �������� */

    case WM_CLOSE:      {
			  return(FALSE) ;
			     break ;
			}
/*----------------------------------------------------------- ������ */

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
/*	          ��������� ��������� �� �������                     */    

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
/*	          ����e ����������� ���������                        */    

  LRESULT CALLBACK  EmiRoot_Cmd_WndProc(  HWND  hWnd,     UINT  Msg,
	                                WPARAM  wParam, LPARAM  lParam) 
{
 LRESULT  result ;
    BYTE  key_status[256] ;
    char  text[1024] ;
    char *arrow ;
   short  status ;

/*-------------------------------- �������� ������� ������� � ������ */

        SendMessage(hWnd, WM_GETTEXT, (WPARAM)(sizeof(text)-1), (LPARAM)text) ;

/*-------------------------------------- ��������� ������ �� ������� */

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
/*---------------------------------------- ��������� ������ �� ����� */

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
/*-------------------------------- ����� ������� ��������� ��������� */

      result=CallWindowProc(Cmd_WndProc.call,
                               hWnd, Msg, wParam, lParam) ;

/*-------------------------------------------------------------------*/

  return(result) ;
}


/********************************************************************/
/*								    */
/*                       ������ ������                              */

   int  EmiRoot_command_start(char *cmd)

{
       HANDLE  hThread ;
        DWORD  hPID ;
 static  char  command[1024] ;

/*------------------------------ ������������� � ���������� �������� */

                            ResetEvent(hCommandEnd) ;

   if(WaitCommandEnd) {

                    WaitForSingleObject(hCommandEnd, INFINITE) ;
                             ResetEvent(hCommandEnd) ;
                      }
/*------------------------------------- ������ ���������� ���������� */

//             SEND_CHECK("") ;                                     /* ����� ��������� � �������� ������� ����� */

                  strncpy(command, cmd, sizeof(command)) ;          /* ����������� ��������� ������� */

     hThread=CreateThread(NULL, 0, 
                            Command_Thread, command, 0, &hPID) ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                THREAD - ����� ���������� ������                   */

  DWORD WINAPI  Command_Thread(LPVOID  Pars)

{
  char command[1024] ;

/*------------------------------------------------------- ���������� */

                  strncpy(command, (char *)Pars, sizeof(command)) ;

          EmiRoot_threads("ADD", command) ;                         /* ������������ ������� ����� */

/*------------------------------------------------ ��������� ������� */

                EmiRoot_command_processor(command) ;

/*------------------------------------------------------- ���������� */

     if(WaitCommandEnd)  SetEvent(hCommandEnd) ;

              EmiRoot_threads("DELETE", NULL) ;                     /* ��������� ������� ����� */

/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*                    ��������� ������                              */

   int  EmiRoot_command_processor(char *command)

{
  static     int  file_rd_flag ;         /* ���� ���������� ���������� ����� */
  static     int  file_wr_flag ;         /* ���� ������ ���������� ����� */
  static    char  postfix[1024] ;        /* ���� ����������� ������ */
  static    char  arrow_cmd[1024] ;      /* ���������� ������� */
  static    char  arrow_SHT_cmd[1024] ;
  static    char  arrow_step[32] ;       /* ���� ���������� ������ */
  static    char  arrow_SHT_step[32] ;
  static     int  arrow_wait ;
             int  arrow_command ;
            char  o_name[128] ;          /* ��� ������� */ 
            char *u_name ;               /* ��� ���������� */
      RSS_Object *object ;
      RSS_Object *unit ;
             int  postfix_flag ;         /* ���� ��������� ����������� ������� */
            HWND  hDlg ;
             int  status ;
            char  cmd_buff[1024] ;
            char *next ;
             int  q_flag ;
            char *name ;
            char *end ;
            char *quotes ;
            char *word2 ;
            char *word3 ;
          double  value ;
            char  work[1024] ;
            char  text[1024] ;
             int  size ;
             int  i ;

  static    char *cmd_history[32] ;      /* ������� ������ */
  static     int  cmd_history_ptr  ;     /* ������ ������� � ������� ������ */
            char *cmd_new ;

typedef  struct {
                   char  name[32] ;
                   char  postfix[1024] ;
                   char  arrow_cmd[1024] ;
                   char  arrow_SHT_cmd[1024] ;
                } EmiRoot_commands ;

  static  EmiRoot_commands *commands_pool[100] ;   /* ������ ������� ������ */

#define   OBJECTS       RSS_Kernel::kernel->kernel_objects 
#define   OBJECTS_CNT   RSS_Kernel::kernel->kernel_objects_cnt 

/*------------------------------------------------------- ���������� */

                        hDlg=hDialog ;

                      RSS_Kernel::kernel->stop=0 ;

                           arrow_command=0 ;

/*---------------------------------------------------- ������������� */

        if(    arrow_step[0]==0)      strcpy(arrow_step, "0.1") ;
        if(arrow_SHT_step[0]==0)  strcpy(arrow_SHT_step, "5.0") ;

/*-------------------------------------------- ���� ��������� ������ */

  do {                                                              /* CIRCLE.0 */
                               WaitCommandEnd=0 ;

        if(file_rd_flag) {                                          /* ���������� ������� �� ����� */
                            status=EmiRoot_command_read(&command) ;
                         if(status) {   file_rd_flag=0 ; 
                                      SHOW_COMMAND("") ;
                                           break ;        }                             

                               WaitCommandEnd=1 ;
                         }

                EmiRoot_command_normalise(command, 0) ;             /* ������������ ������� */

        if(file_wr_flag) {                                          /* ������ ������� � ���� */
                            EmiRoot_command_write(command) ;
                         }
/*----------------------------------------- ������ � �������� ������ */
/*- - - - - - - - - - - - - - - -  �������������� ���������� ������� */
     if(!stricmp(command, "$prev_command$")) {

                                        cmd_history_ptr++ ;

          if(cmd_history_ptr<0)  cmd_history_ptr=0 ;

          if(            cmd_history_ptr >= 32 )  cmd_history_ptr=31 ;
          if(cmd_history[cmd_history_ptr]==NULL)  cmd_history_ptr-- ;

          if(cmd_history[cmd_history_ptr]!=NULL)
               SHOW_COMMAND(cmd_history[cmd_history_ptr]) ;

                                                 return(0) ;                                      
                                             }
/*- - - - - - - - - - - - - - - - - �������������� ��������� ������� */
     if(!stricmp(command, "$next_command$")) {

                                         cmd_history_ptr-- ;
                if(cmd_history_ptr<0) {  cmd_history_ptr=0 ;
                                           SHOW_COMMAND("") ;  }
                else                       SHOW_COMMAND(cmd_history[cmd_history_ptr]) ;

                                                   return(0) ;
                                             }
/*- - - - - - - - - - - - - - - - - - - - ���������� ������� ������� */
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
/*---------------------------------------------- ����������� ������� */
/*- - - - - - - - - - - - - - - - - ������ �������� ���������� ����� */
   if(command[0]=='@' &&
      command[1]=='@'   ) {

            memmove(command+6, command+2, strlen(command+1)+1) ;
            memmove(command,  "#save ", 6) ;

                 EmiRoot_command_write(command) ;

                            file_wr_flag=1 ;
                                 continue ;
                           }
/*- - - - - - - - - - - - - - - - - - -  ���������� ���������� ����� */
   if(command[0]=='@') {

            memmove(command+6, command+1, strlen(command+1)+1) ;
            memmove(command,  "#file ", 6) ;

                             file_rd_flag=1 ;
                                 continue ;
                       }
/*- - - - - - - - - - - - - - - - - - ���������� ����������� ������� */
   if(command[0]=='.' &&
      command[1]== 0    ) {
                                  strcpy(command, postfix) ;
                          }
/*- - - - - - - - - - - - - - - - - - -  ������� ����������� ������� */
   if(command[0]=='.' &&
      command[1]!='.'   ) {

                 memset(postfix, 0, sizeof(postfix)) ;
                strncpy(postfix, command+1, sizeof(postfix)-1) ;

                   SETs(IDC_CMD_POSTFIX, postfix) ;
         EmiRoot_system() ;

                                continue ;
                          }
/*- - - - - - - - - - - - - - - - - - - �������� ����������� ������� */
   if(command[0]=='.' &&
      command[1]=='.'   ) {

                     memset(postfix, 0, sizeof(postfix)) ;

                       SETs(IDC_CMD_POSTFIX, postfix) ;
             EmiRoot_system() ;

                                continue ;
                          }
/*- - - - - - - - - - - - - - - - - ������� SHIFT-���������� ������� */
   if(strstr(command, ">>")!=NULL) {

       if(!strcmp(command, ">>")) {
                                      arrow_SHT_cmd[0]=0 ;
                                  }
       else                       {

               value=strtod(command+2, &end) ;
            if(value>0. && *end==0) 
                         strncpy(arrow_SHT_step, command+2, sizeof(arrow_SHT_step)) ;
            else         strncpy(arrow_SHT_cmd,  command,   sizeof(arrow_SHT_cmd)) ;
                                  }

                            SETs(IDC_CMD_ARROW_ALT, arrow_SHT_cmd) ;
                                       EmiRoot_system() ;    
                                            continue ;
                                   } 
/*- - - - - - - - - - - - - - - - - - - - ������� ���������� ������� */
   if(strstr(command, ">")!=NULL) {

       if(!strcmp(command, ">" )) {
                                     arrow_cmd[0]=0 ;
                                  }
       else                       {

               value=strtod(command+1, &end) ;
            if(value>0. && *end==0) 
                         strncpy(arrow_step, command+1, sizeof(arrow_step)) ;
            else         strncpy(arrow_cmd,  command,   sizeof(arrow_cmd)) ;
                                  }

                              SETs(IDC_CMD_ARROW,  arrow_cmd) ;
                                         EmiRoot_system() ;    
                                              continue ;
                                  } 
/*- - - - - - - - - - - - - - - - - - - -  ���������� ������ ������  */
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
/*- - - - - - - - - - - - - - - - - -  �������������� ������ ������  */
   if(command[0]=='!') {

#define  CP  commands_pool

            name=command+1 ;

        for(i=0 ; CP[i] ; i++)
          if(!stricmp(CP[i]->name, name))  break ;

          if(CP[i]==NULL) {
                             sprintf(text, "����������� ����� ������: %s", strupr(name)) ;
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
/*------------------------------------ ��������� "����������" ������ */

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

      size=(int)strlen(command)+(int)strlen(word2)+3 ;
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
/*-------------------------------------------- ���� ��������� ������ */

                    postfix_flag=0 ;
                          q_flag=0 ;

   for(next=command ; next!=NULL ; command=next+1) {                /* CIRCLE.1 */

     for(next=command ; *next ; next++) {

       if(*next=='"')  q_flag=!q_flag ;

       if(q_flag)  continue ;

       if(*next==';')            break ;
//     if(*next=='%') {  postfix_flag=1 ;
//                               break ;  }
                                        }

      if(*next==0   )  next=NULL ;
      if( next!=NULL) *next=  0 ;

                               strcpy(cmd_buff, command) ;
            EmiRoot_command_normalise(cmd_buff, 1) ;

                              command=cmd_buff ;

/*--------------------------------- ����������� ������� ��-��������� */

   if(strchr(command, '*')!=NULL) {

        do {                                                        /* ����������� ������������ �� ������ ������� � ������ */
                 end=strchr(command, '*') ;
              if(end==NULL)  break ;

                 quotes=strchr(command, '"') ;
              if(quotes!=NULL && quotes<end)  break ;

                  memmove(end+strlen(__object_def), end+1, strlen(end+1)+1) ;
                   memcpy(end, __object_def, strlen(__object_def)) ;

           } while(1) ;

        do {                                                        /* �������� ������� */ 
                 end=strchr(command, '"') ;
              if(end==NULL)  break ;

                     strcpy(end, end+1) ;

           } while(1) ;
                                 }
/*----------------------------------------- ��������������� �������� */

     do {                                                           /* BLOCK.1 */
/*- - - - - - - - - - - - - -  �������������� ��� �������/���������� */
                 memset(o_name, 0, sizeof(o_name)) ;
                strncpy(o_name, command, sizeof(o_name)-1) ;
             end=strchr(o_name, ' ') ;                              /* �������� ������ ����� ������� */
          if(end!=NULL)  *end=0 ;

          if(o_name[0]!='.') {
                   u_name=strchr(o_name, '.') ;                     /* ���� ���� -��������� ����� ������� � ���������� */
                if(u_name!=NULL)  {
                                     *u_name=0 ;
                                      u_name++ ;
                                  }
                             } 
/*- - - - - - - - - - - - - - - - - - - - - -  ������������� ������� */
                     object=NULL ;

        for(i=0 ; i<OBJECTS_CNT ; i++)                              /* ���� ������ �� ����� */
          if(!stricmp(OBJECTS[i]->Name, o_name)) {
                                                   object=OBJECTS[i] ;
                                                        break ;
                                                 } 
          if(object==NULL)  break ;                                 /* ���� ������ ���... */
/*- - - - - - - - - - - - - - - - - - - - - ������������� ���������� */
       if(u_name!=NULL) {
                            unit=NULL ;

         for(i=0 ; i<object->Units.List_cnt ; i++)                  /* ���� ��������� �� ����� */
          if(!stricmp(object->Units.List[i].object->Name, u_name)) {
                         unit=object->Units.List[i].object ;
                                               break ;
                                                                   }

                                     if(unit==NULL)  break ;        /* ���� ������ ���... */

                            object=unit ;
                        }
/*- - - - - - - - - - - - - - - - - -  ��������� ������ ��-��������� */
             memset(__object_def, 0, sizeof(__object_def)) ;
            strncpy(__object_def, o_name, sizeof(__object_def)-1) ;

        if(u_name!=NULL) {
            strncat(__object_def, ".",    sizeof(__object_def)-1) ;
            strncat(__object_def, u_name, sizeof(__object_def)-1) ;
                         }

          if(end==NULL) {  command[0]=0 ;  break ;  }               /* ���� � ������� ������ */
                                                                    /*   �������� �������... */
/*- - - - - - - - - - - - - - - - - - - - ������������������ ������� */
       do {
                word2=strchr(command, ' ') ;
             if(word2==NULL) {
                                 word2="" ;
                                 word3="" ;
                               *o_name= 0 ;
                                  break ;
                             }
                word2++ ;
                word3=strchr(word2, ' ') ;
             if(word3==NULL)    word3="" ;
             else            { *word3= 0 ;
                                word3++ ;  } 
                           
          } while(0) ;

           sprintf(work, "%s %s %s %s", object->Type,               /* ����������������� ������� */
                                        word2,
                                      __object_def,
                                        word3            ) ;
                   command=work ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        } while(0) ;                                                /* BLOCK.1 */

/*------------------------------ ����� ��������� ����������� ������� */

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

/*--------------------------------- ����� ���������� ���������� ���� */

        status=Kernel.vExecuteCmd(command) ;
     if(status==1) {
       if(command[0]!=0) {
                             sprintf(text, "����������� �������: %s", command) ;
                       EmiRoot_error(text, MB_ICONERROR) ;
                  if(arrow_command)  arrow_wait=0 ;
                                return(-1) ;
                         }
                   } 
/*-------------------------------------------- ���� ��������� ������ */

                          EmiRoot_system() ;

                                                   }                /* CONTINUE.1 */
/*-------------------------------------------- ���� ��������� ������ */

     } while(file_rd_flag) ;                                        /* CONTINUE.0 */

/*-------------------------------------------------------------------*/

     if(arrow_command)  arrow_wait=0 ;

#undef   OBJECTS
#undef   OBJECTS_CNT

   return(0) ;
}


/********************************************************************/
/*								    */
/*                 ���������� ���������� �����                      */

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

/*------------------------------------------- ���� ���������� ������ */

   while(1) {                                                       /* CIRCLE.1 */
                  memset(buff, 0, sizeof(buff)) ;
                  memset(text, 0, sizeof(text)) ;
/*- - - - - - - - - - - - - - - - - - -  ���������� ������� �� ����� */
       if(file_num>=0) {
                          Sleep(step) ;

          end=fgets(buff, sizeof(buff)-1, files[file_num].file) ;   /* ��������� ��������� ������ */
       if(end==NULL) {                                             /* ���� ���� ����������... */
                        fclose(files[file_num].file) ;
                                     file_num-- ;
                                  if(file_num<0)  return(-1) ;
                                                   continue ;
                     }

                                 files[file_num].row++ ;

           sprintf(text, "��������� ����: %s\n"
                         "        ������: %d\n", 
                                 files[file_num].name, 
                                 files[file_num].row  ) ;
                       }
/*- - - - - - - - - - - - - - - - - - - -  ��������� ������� ������� */
       else            {
                          strncpy(buff, *command, sizeof(buff)-1) ;
                       }      
/*--------------------------------- ��������������� ��������� ������ */

                EmiRoot_command_normalise(buff, 0) ;                /* ������������ ������ */

            if(*buff==';')  continue ;                              /* ���� �����������... */

/*---------------------------- ������� - ���������� ���������� ����� */

         if(!memicmp(buff, "#exit", 
                    strlen("#exit" ))) {
                                           return(-1) ;
                                       }
/*------------------------------------- ������� - ������������ ����� */

         if(!memicmp(buff, "#step ", 
                    strlen("#step " ))) {

               value=strtod(buff+strlen("#step "), &end) ;
            if(value < 0. ||
               value >10. ||
                *end!= 0    ) {
                                  strcat(text, "������������ �������� � ��������� #STEP") ;
                           EmiRoot_error(text, MB_ICONERROR) ;
                                      return(-1) ;
                              }  

#pragma warning(disable : 4244)
                  step=value*1000 ;
#pragma warning(default : 4244)
                                        }
/*-------------------------------------------------- ������� - ����� */

    else if(!memicmp(buff, "#pause ", 
                    strlen("#pause "))) {

               value=strtod(buff+strlen("#pause "), &end) ;
            if(value < 0. ||
                *end!= 0    ) {
                                  strcat(text, "������������ �������� � ��������� #PAUSE") ;
                           EmiRoot_error(text, MB_ICONERROR) ;
                                   return(-1) ;
                              }  

#pragma warning(disable : 4244)
                          value=value*1000-step ;
                    Sleep(value) ;
#pragma warning(default : 4244)

                                        }
/*--------------------------------------------- ������� - ���������� */

    else if(!memicmp(buff, "#remark ", 
                    strlen("#remark "))) {

            MessageBox(NULL, buff+strlen("#remark "), "Emi Stand",
                               MB_ICONINFORMATION | MB_TASKMODAL) ;
                                         }
/*----------------------------------------- ������� - ��������� ���� */

    else if(!memicmp(buff, "#file ", 
                    strlen("#file "))) {

#define  F   files[file_num+1]

            if(file_num+1>=_FILES_MAX) {
                        strcat(text, "#FILE: ������� ����� ��������� ������") ;
                 EmiRoot_error(text, MB_ICONERROR) ;
                         return(-1) ;
                                       }

                     memset(F.name, 0, sizeof(F.name)) ;
                    sprintf(F.name, "%s\\%s", __cwd, buff+strlen("#file ")) ;

               F.file=fopen(F.name, "rt") ;
            if(F.file==NULL) {
                         strcat(text, "#FILE: ������ �������� ����� ") ;
                         strcat(text, F.name) ;
                  EmiRoot_error(text, MB_ICONERROR) ;
                                  return(-1) ;
                             }

               F.row=0 ;
                file_num++ ;
#undef   F
                                       }
/*------------------------------------------------ '�������' ������� */

    else                              {                
                                                break ;  
                                      }
/*------------------------------------------- ���� ���������� ������ */
            }                                                       /* CONTINUE.1 */
/*-------------------------------------------------------------------*/

                  SHOW_COMMAND(buff) ;
                EmiRoot_system() ;

                      *command=buff ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*                   ������ ���������� �����                        */

   int  EmiRoot_command_write(char *command)

{
   static  FILE *file ;

/*---------------------------------------- �������� ���������� ����� */

    if(!memicmp(command, "#save ", strlen("#save "))) {

         if(file!=NULL)  fclose(file) ;

             command+=strlen("#save ") ;
         if(*command==0)  return(0) ;

               file=fopen(command, "at") ;

                        return(0) ;
                                                      }
/*----------------------------------------------- ���������� ������� */

       if(file==NULL)  return(-1) ;

         fwrite(command, 1, strlen(command), file) ;
         fwrite("\n",    1, strlen("\n"),    file) ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*                 ������������ ��������� ������                    */

  void  EmiRoot_command_normalise(char *command, int  q_flag)

{
     char *tmp ;

/*----------------------------------- ������ ������������ �� ������� */

                   for(tmp=command ; *tmp ; tmp++)
                     if(*tmp=='\t' ||
                        *tmp=='\r' ||
                        *tmp=='\n'   )  *tmp=' ' ;

    if(q_flag) {
                   for(tmp=command ; *tmp ; tmp++)
                     if(*tmp=='"')  *tmp=' ' ;
               } 
/*------------------------------ ������� ���������/�������� �������� */

      for(tmp=command ; *tmp==' ' ; tmp++) ;

        if(tmp!=command)  strcpy(command, tmp) ;

      for(tmp=command+strlen(command)-1 ; 
           tmp>=command && *tmp==' ' ;  tmp--) *tmp= 0 ;

/*------------------------------------------- ������ ������ �������� */

     while(1) {
                    tmp=strstr(command, "  ") ;
                 if(tmp==NULL)  break ;

                    strcpy(tmp, tmp+1) ;
              }
/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*                   ���������� ������ � �����                      */

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
/**            �������� ������ ������ ������ ����������            **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*                            ������ ������                         */

  struct RSS_Module_Main_instr  RSS_Module_Main_InstrList[]={

 { "help",      "?",     "# HELP   - ������ ��������� ������", 
                          NULL,
                         &RSS_Module_Main::cHelp   },
 { "show",      "s",     "#SHOW   - ���������� ������� ������������", 
                          NULL,
                        &RSS_Module_Main::cShowScene  },
 { "kill",      "k",     "# KILL - ������� ������",
                         " KILL[/Q] <���>\n"
                         "   ������� ����������� ������"
                         " KILL[/Q] <�������>%\n"
                         "   ������� ��� �������, ��� ������� ���������� � ��������� ���������",
                         &RSS_Module_Main::cKill },
 { "all",       "all",   "# ALL - ������ ������ ��������",
                          NULL,
                         &RSS_Module_Main::cAll },
 { "lookinfo",  "li",    "# LOOKINFO - �������� ��������� ������ ",
                          NULL,
                         &RSS_Module_Main::cLookInfo },
 { "eye",       "eye",   "# EYE - ������ ����� ��������� �����",
                         " EYE    <X> <Y> <Z>\n"
                         " EYE/X  <X>\n"
                         " EYE/+X <X>\n"
                         " EYE>   <���>\n"
                         " EYE>>  <���>\n",
                         &RSS_Module_Main::cLookPoint },
 { "look",      "look",  "# LOOK - ������ ����������� �������",
                         " LOOK    <A> <E> <R>\n"
                         " LOOK/A  <A>\n"
                         " LOOK/+A <A>\n"
                         " LOOK>   <���>\n"
                         " LOOK>>  <���>\n",
                         &RSS_Module_Main::cLookDirection },
 { "lookat",    "la",    "# LOOKAT - ������ �����(������) ����������",
                         " LOOKAT <��� �������>\n",
                         &RSS_Module_Main::cLookAt },
 { "lookfrom",  "lf",    "# LOOKFROM - ������ �����(������), �� �������� ������ ����������",
                         " LOOKFROM <��� �������>\n",
                         &RSS_Module_Main::cLookFrom },
 { "lookshift", "ls",    "# LOOKSHIFT - ������ ����� ����� ���������� ������������ �������, ��������� LOOKAT",
                         " LOOKSHIFT <d�> <dY> <dZ>\n",
                         &RSS_Module_Main::cLookShift },
 { "zoom",      "z",     "# ZOOM - ���������� ����� ������ ������",
                         " ZOOM <������ ���� ������>\n"
                         " ZOOM+\n"
                         " ZOOM-\n",
                         &RSS_Module_Main::cLookZoom },
 { "read",      "read",  "# READ - ������� ������ �� �����",
                         " READ [<����>]  \n",
                         &RSS_Module_Main::cRead },
 { "write",     "write", "# WRITE - �������� ������ � ����",
                         " WRITE [<����>]  \n"
                         " WRITE/� [<����>] <������1> <������2>...\n",
                         &RSS_Module_Main::cWrite },
 { "threads",   "th",    "# THREADS - ������� ������� �������", 
                          NULL,
                         &RSS_Module_Main::cThreads   },
 { "stop",      "stop",  "# STOP - ���������� ������� �����",
                          NULL,
                         &RSS_Module_Main::cStop   },
 { "next",      "next",  "# NEXT - ��������� ������� ����� � ��������� ������",
                          NULL,
                         &RSS_Module_Main::cNext   },
 { "resume",    "resume","# RESUME - ��������� ������� ����� � ����������� ������",
                          NULL,
                         &RSS_Module_Main::cResume   },
 { "modules",   "mod",   "# MODULES - ������ ������������ �������", 
                          NULL,
                         &RSS_Module_Main::cModules   },
 { "memory",    "mem",   "# MEMORY - ���������� �������", 
                         " MEMORY ON    -  ��������� ����������� ������ \n"
                         " MEMORY LIST  -  ����� ����� ������ \n",
                         &RSS_Module_Main::cMemory   },
 { "srand",     "srand", "# SRAND - ���������� ����������� ��������� �������������������", 
                         " SRAND <�����> -  ������� �����-����� ����������, �����>1 \n"
                         " SRAND AUTO    -  ������� �����-����� � �������������� ����� \n",
                         &RSS_Module_Main::cSrand    },
 { "time",      "time",  "# TIME - ������� ������� ������� ��������� �������������",
                         " TIME <�����> -  ������� ������� ������ ������� ��� ��������� � �����������, � ��������\n"
                         " TIME/C <�����> -  ������� ������ ������� ���������, � ��������\n"
                         " TIME/S <�����> -  ������� ������ ������� �����������, � ��������\n",
                         &RSS_Module_Main::cTime    },
 {  NULL }
                                                            } ;

/********************************************************************/
/*								    */
/*		     ����� ����� ������             		    */

    struct RSS_Module_Main_instr *RSS_Module_Main::mInstrList ;

/********************************************************************/
/*								    */
/*		       ����������� ������			    */

     RSS_Module_Main::RSS_Module_Main(void)

{
	   keyword="EmiStand" ;
    identification="Module_Main" ;

        mInstrList=RSS_Module_Main_InstrList ;
}


/********************************************************************/
/*								    */
/*		        ���������� ������			    */

    RSS_Module_Main::~RSS_Module_Main(void)

{
}


/********************************************************************/
/*								    */
/*		        ��������� �������       		    */

  int  RSS_Module_Main::vExecuteCmd(const char *cmd)

{
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  i ;

/*------------------------------------------------------- ���������� */

             memset(command, 0, sizeof(command)) ;
            strncpy(command, cmd, sizeof(command)-1) ;

/*--------------------------------------------- ��������� ���������� */

       instr=command ;                                              /* �������� ����� � ��������� ������� */

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

   for(i=0 ; mInstrList[i].name_full!=NULL ; i++)                   /* ���� ������� � ������ */
     if(!stricmp(instr, mInstrList[i].name_full) ||
        !stricmp(instr, mInstrList[i].name_shrt)   )   break ;

     if(mInstrList[i].name_full==NULL) {                            /* ���� ����� ������� ���... */
                                            return(1) ;
                                       }

     if(mInstrList[i].process!=NULL)                                /* ���������� ������� */
                       (this->*mInstrList[i].process)(end) ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		        �������� ��������       		    */

     int  RSS_Module_Main::vGetParameter(char *name, char *value)

{
   char  text[128] ;

        if(!stricmp(name, "?1")) {
                                 }
   else if(!stricmp(name, "?2")) {
                                 }
   else                          {
                sprintf(text, "������ ������������ ���������: %s", name) ;
             SEND_ERROR(text) ;
                                       return(-1) ;
                                 }

   return(0) ;
}


/********************************************************************/
/*								    */
/*                   ��������� ������� ����������                   */

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
/*                    ���������� ��������� ������                   */

    void  RSS_Module_Main::vShow(char *layer)

{
     SendMessage(hDialog, WM_USER, _USER_SHOW, (LPARAM)layer) ;
}


/********************************************************************/
/*								    */
/*		        �������� ������ � ������		    */

    void  RSS_Module_Main::vWriteSave(std::string *text)

{
  std::string  buff ;
          int  status ;

/*----------------------------------------------- ��������� �������� */

     *text="#BEGIN MODULE MAIN\n" ;

/*----------------------------------------------------------- ������ */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - ��������� ���� */
          status=this->display.SetFirstContext(NULL) ;
    while(status==0) {
                         EmiRoot_write_context(&buff) ;

                             *text+="WINDOW " ;
                             *text+= buff ;
                             *text+="\n" ;
                 
          status=this->display.SetNextContext(NULL) ;
                     }
/*------------------------------------------------ �������� �������� */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        ������� ������ �� ������		    */

    void  RSS_Module_Main::vReadSave(std::string *data)

{
        char *buff ;
         int  buff_size ;
 std::string  decl ;
        char *work ;
        char  text[1024] ;
        char *end ;

/*----------------------------------------------- �������� ��������� */

   if(memicmp(data->c_str(), "#BEGIN MODULE MAIN\n", 
                      strlen("#BEGIN MODULE MAIN\n")))  return ;

/*------------------------------------------------ ���������� ������ */

              buff_size=(int)data->size()+16 ;
              buff     =(char *)calloc(1, buff_size) ;

       strcpy(buff, data->c_str()) ;
        
/*---------------------------------------------- ���������� �������� */

                work=buff+strlen("#BEGIN MODULE MAIN\n") ;

    for( ; ; work=end+1) {                                          /* CIRCLE.0 */
                                      end=strchr(work, '\n') ;
                                   if(end==NULL)  break ;
                                     *end=0 ;

/*---------------------------------------------------- �������� ���� */

          if(!memicmp(work, "WINDOW", strlen("WINDOW"))) {

                                          decl=work+strlen("WINDOW") ;
                    EmiRoot_read_context(&decl) ;
                                                         }
/*---------------------------------------------------------- ??????? */

     else if(!memicmp(work, "??????", strlen("??????"))) {

                                                         }
/*----------------------------------------- ����������� ������������ */

     else                                                {

                sprintf(text, "Module MAIN: "
                              "����������� ������������ %20.20s", work) ;
             SEND_ERROR(text) ;
                                 break ;
                                                         }
/*---------------------------------------------- ���������� �������� */
                         }                                          /* CONTINUE.0 */
/*-------------------------------------------- ������������ �������� */

                free(buff) ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� HELP                    */

  int  RSS_Module_Main::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Main_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� SHOW                    */

  int  RSS_Module_Main::cShowScene(char *cmd)

{
         this->vShow(NULL) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� Kill                    */
/*								    */
/*      KILL <���>                                                  */

  int  RSS_Module_Main::cKill(char *cmd)

{
   int   quiet_flag ;      /* ���� ������ "��������" */
   int   batch_flag ;      /* ���� �������� �������� (��� ���������� ������) */
  char *name ;
  char  object_name[128] ;
  char *end ;
   int  prefix_flag ;
   int  i ;
   int  j ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*-------------------------------------- ���������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
                      quiet_flag=0 ;
                      batch_flag=0 ;

       if(*cmd=='/') {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("������������ ������ �������") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, 'q')!=NULL ||
                   strchr(cmd, 'Q')!=NULL   )  quiet_flag=1 ;
                if(strchr(cmd, 'b')!=NULL ||
                   strchr(cmd, 'B')!=NULL   )  batch_flag=1 ;

                           cmd=end+1 ;
                     }
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������ ���������� */        
                  name=cmd ;                                        /* ��������� ��� ������� */
                   end=strchr(name, ' ') ;
                if(end!=NULL)  *end=0 ;

    if(name[0]==0) {                                                /* ���� ��� �� ������... */
               SEND_ERROR("�� ������ ��� �������") ;
                          return(-1) ;
                   }
/*------------------------------------------- �������� ���� �������� */

    if(!stricmp(name, "ALL") ||
       !stricmp(name, "*"  )   ) {

       for(i=0 ; i<OBJECTS_CNT ; i++) {
                                         OBJECTS[i]->vFree() ;      /* ������������ �������� */
                                 delete  OBJECTS[i] ;
                                      }

                                         OBJECTS_CNT=0 ;

                                   this->kernel->vShow("REFRESH") ;
                                         return(0) ;
                                 }
/*-------------------------------------------- ������ �� "���������" */

                        prefix_flag=0 ;

       end=strchr(name, '%') ;
    if(end!=NULL) {
                        prefix_flag=1 ;
                               *end=0 ;
                  }

/*----------------------------------- ���������� �������� �������/�� */

   do {
/*- - - - - - - - - - - - - - - - - - - - - - ����� ������� �� ����� */
     if(prefix_flag) {

         for(i=0 ; i<OBJECTS_CNT ; i++)
            if(!memicmp(OBJECTS[i]->Name, name, strlen(name)))  break ;

            if(i==OBJECTS_CNT)  break ;
                     }
     else            {

         for(i=0 ; i<OBJECTS_CNT ; i++)
            if(!stricmp(OBJECTS[i]->Name, name))  break ;

            if(i==OBJECTS_CNT) {                                    /* ���� ��� �� ������... */
                 if(!quiet_flag)  SEND_ERROR("������� � ����� ������ �� ����������") ;
                                  return(-1) ;
                               }
                     }
/*- - - - - - - - - - - - - - - - - - �������� ������������� ������� */
                                     EmiRoot_lookfrom("Main", "Get", object_name) ;
     if(!strcmp(name, object_name))  EmiRoot_lookfrom("Main", "Set", "") ;

                                     EmiRoot_lookat("Main", "Get", object_name, NULL, NULL, NULL) ;
     if(!strcmp(name, object_name))  EmiRoot_lookat("Main", "Set", "", NULL, NULL, NULL) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - �������� ������� */
                                         OBJECTS[i]->vFree() ;      /* ������������ �������� */
                                 delete  OBJECTS[i] ;

       for(j=i+1 ; j<OBJECTS_CNT ; j++)  OBJECTS[j-1]=OBJECTS[j] ;  /* �������� ������ �������� */

                     OBJECTS_CNT-- ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - �������� ������� */
       if(!prefix_flag)  break ;

      } while(1) ;

/*---------------------------------------------- ���������� �������� */

      if(!batch_flag)  this->kernel->vShow("REFRESH") ;

/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

   return(0) ;
}

/********************************************************************/
/*								    */
/*		      ���������� ���������� All                     */

  int  RSS_Module_Main::cAll(char *cmd)

{
  RSS_Transit_Main  Context ; 

/*-------------------------------------- ���������� ��������� ������ */

/*------------------------------------------------- �������� ������� */

          strcpy(Context.action, "ALL") ;

     SendMessage(RSS_Kernel::kernel_wnd, 
                  WM_USER, (WPARAM)_USER_CHANGE_CONTEXT, 
                           (LPARAM)&Context              ) ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� EYE                     */
/*								    */
/*         EYE    <X> <Y> <Z>                                       */
/*         EYE/X  <X>                                               */
/*         EYE/+X <X>                                               */
/*         EYE>   <��� ���������> <���>                             */
/*         EYE>>  <��� ���������> <���>                             */

  int  RSS_Module_Main::cLookPoint(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX   4

             char  *pars[_PARS_MAX] ;
             char **xyz ;
           double   coord[_COORD_MAX] ;
              int   coord_cnt ;
           double   inverse ;
              int   xyz_flag ;          /* ���� ������ ����� ���������� */
              int   xyz_idx ;
              int   quiet_flag ;        /* ���� ����������� ��������� */
              int   delta_flag ;        /* ���� ������ ���������� */
              int   arrow_flag ;        /* ���� ����������� ������ */
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

/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
                        xyz_flag=0 ;
                      delta_flag=0 ;
                      arrow_flag=0 ;
                      quiet_flag=0 ;

       if(*cmd=='/' ||
          *cmd=='+'   ) {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("������������ ������ �������") ;
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

                if(strchr(cmd, 'q')!=NULL ||
                   strchr(cmd, 'Q')!=NULL   )  quiet_flag= 1 ;

                           cmd=end+1 ;
                        }

  else if(*cmd=='>') {
                           delta_flag=1 ;
                           arrow_flag=1 ;

                          cmd=strchr(cmd, ' ') ;
                       if(cmd==NULL)  return(0) ;
                          cmd++ ;
                     } 
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������ ���������� */        
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

                      xyz=&pars[0] ;   
                  inverse=   1. ; 

/*------------------------------------- ��������� ����������� ������ */

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
/*------------------------------------------------- ������ ��������� */

    for(i=0 ; xyz[i]!=NULL && i<_COORD_MAX ; i++) {

             coord[i]=strtod(xyz[i], &end) ;
        if(*end!=0) {  
                       SEND_ERROR("������������ �������� ����������") ;
                                       return(-1) ;
                    }
                                                  }

                             coord_cnt=  i ;

                                 error= NULL ;
      if(arrow_flag ||
           xyz_flag   ) {
               if(coord_cnt==0)  error="�� ������� ���������� ��� �� ����������" ;
                        }
      else              {
               if(coord_cnt <3)  error="������ ���� ������� 3 ����������" ;
                        }

      if(error!=NULL)  SEND_ERROR(error) ;

      if(arrow_flag && coord_cnt>1)  coord[0]=coord[coord_cnt-1] ;  /* ��� ����������� ������ ����� � �������� ���� */
                                                                    /*  ����� ��������� ��������                    */
/*-------------------------------------- ��������� ���������� ������ */

       status=EmiRoot_look("Main", "Get", &look_x, &look_y, &look_z,
                                          &look_a, &look_e, &look_r ) ;
    if(status) {
                   SEND_ERROR("����������� ����") ;
                      return(-1) ;
               }

/*--------------------- ����������� �������� ���������� �����������  */

               Point_Matrix.LoadZero   (3, 1) ;
               Point_Matrix.SetCell    (xyz_idx, 0, inverse*coord[0]) ;

                 Sum_Matrix.Load3d_azim( look_a) ;
                Oper_Matrix.Load3d_elev( look_e) ;
                 Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
                Oper_Matrix.Load3d_roll( look_r) ;
                 Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
               Point_Matrix.LoadMul    (&Sum_Matrix, &Point_Matrix) ;

/*------------------------------------------------ ������� ��������� */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - -  ��������� */
        if(arrow_flag) {
                           look_x+=Point_Matrix.GetCell(0, 0) ;
                           look_y+=Point_Matrix.GetCell(1, 0) ;
                           look_z+=Point_Matrix.GetCell(2, 0) ;
                       }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - ���������� */
   else if(delta_flag) {

              if(xyz_flag=='X')   look_x+=inverse*coord[0] ;
         else if(xyz_flag=='Y')   look_y+=inverse*coord[0] ;                 
         else if(xyz_flag=='Z')   look_z+=inverse*coord[0] ;
                       }
/*- - - - - - - - - - - - - - - - - - - - - - -  ���������� �������� */
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
/*-------------------------------------- ��������� ���������� ������ */

       status=EmiRoot_look("Main", "Set", &look_x, &look_y, &look_z,
                                             NULL,    NULL,    NULL ) ;
    if(status) {
                    return(-1) ;
               }
/*------------------------------------------------------ ����������� */

    if(quiet_flag==0)  EmiRoot_redraw("Main") ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� LOOK                    */
/*								    */
/*        LOOK    <A> <E> <R>                                       */
/*        LOOK/A  <A>                                               */
/*        LOOK/+A <A>                                               */
/*        LOOK>   <��� ���������> <���>                             */
/*        LOOK>>  <��� ���������> <���>                             */

  int  RSS_Module_Main::cLookDirection(char *cmd) 

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10 

             char  *pars[_PARS_MAX] ;
             char **xyz ;
           double   coord[_COORD_MAX] ;
              int   coord_cnt ;
           double   inverse ;
              int   xyz_flag ;          /* ���� ������ ����� ���������� */
              int   delta_flag ;        /* ���� ������ ���������� */
              int   arrow_flag ;        /* ���� ����������� ������ */
             char  *arrows ;
           double   look_a ;
           double   look_e ;
           double   look_r ;
             char  *error ;
             char  *end ;
              int   status ;
              int   i ;

/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
                        xyz_flag=0 ;
                      delta_flag=0 ;
                      arrow_flag=0 ;

       if(*cmd=='/' ||
          *cmd=='+'   ) {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("������������ ������ �������") ;
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
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������ ���������� */        
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

                      xyz=&pars[0] ;   
                  inverse=   1. ; 

/*------------------------------------- ��������� ����������� ������ */

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
/*------------------------------------------------- ������ ��������� */

    for(i=0 ; xyz[i]!=NULL && i<_COORD_MAX ; i++) {

             coord[i]=strtod(xyz[i], &end) ;
        if(*end!=0) {  
                       SEND_ERROR("������������ �������� ����������") ;
                                       return(-1) ;
                    }
                                                  }

                             coord_cnt=  i ;

                                 error= NULL ;
      if(xyz_flag) {
               if(coord_cnt==0)  error="�� ������� ���������� ��� �� ����������" ;
                   }
      else         {
               if(coord_cnt <3)  error="������ ���� ������� 3 ����������" ;
                   }

      if(error!=NULL)  SEND_ERROR(error) ;

      if(arrow_flag && coord_cnt>1)  coord[0]=coord[coord_cnt-1] ;  /* ��� ����������� ������ ����� � �������� ���� */
                                                                    /*  ����� ��������� ��������                    */
/*-------------------------------------- ��������� ���������� ������ */

       status=EmiRoot_look("Main", "Get",    NULL,    NULL,    NULL, 
                                          &look_a, &look_e, &look_r ) ;
    if(status) {
                   SEND_ERROR("����������� ����") ;
                      return(-1) ;
               }
/*------------------------------------------------ ������� ��������� */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - ���������� */
   if(delta_flag) {

          if(xyz_flag=='A')   look_a+=inverse*coord[0] ;
     else if(xyz_flag=='E')   look_e+=inverse*coord[0] ;                 
     else if(xyz_flag=='R')   look_r+=inverse*coord[0] ;
                  }
/*- - - - - - - - - - - - - - - - - - - - - - -  ���������� �������� */
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
/*- - - - - - - - - - - - - - - - - - - - - -  ������������ �������� */
     while(look_a> 180.)  look_a-=360. ;
     while(look_a<-180.)  look_a+=360. ;

     while(look_e> 180.)  look_e-=360. ;
     while(look_e<-180.)  look_e+=360. ;

     while(look_r> 180.)  look_r-=360. ;
     while(look_r<-180.)  look_r+=360. ;

/*-------------------------------------- ��������� ���������� ������ */

       status=EmiRoot_look("Main", "Set",    NULL,    NULL,    NULL, 
                                          &look_a, &look_e, &look_r ) ;
    if(status) {
                    return(-1) ;
               }
/*------------------------------------------------------ ����������� */

            EmiRoot_redraw("Main") ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� ZOOM                    */
/*								    */
/*        ZOOM    <������ ���� ������>                              */
/*        ZOOM+                                                     */
/*        ZOOM-                                                     */

  int  RSS_Module_Main::cLookZoom(char *cmd) 

{
              int   delta_flag ;        /* ���� ������ ���������� */
              int   quiet_flag ;        /* ���� ����������� ��������� */
           double   par ;
           double   zoom ;
             char  *end ;
              int   status ;

/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
                                     delta_flag=0 ;
                                     quiet_flag=0 ;     

     if(strstr(cmd, "/q")!=NULL ||
        strstr(cmd, "/Q")!=NULL   ) {
                                       quiet_flag=1 ;

               end=strchr(cmd, ' ') ;
            if(end!=NULL)  cmd=end+1 ;
                                    }

     if(*cmd=='-' ||
        *cmd=='+'   ) {
                          if(*cmd=='+')  par=1./1.5 ;
                          if(*cmd=='-')  par=   1.5 ;

                                delta_flag=1 ;
                      }
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������ ���������� */
     else             {

            par=strtod(cmd, &end) ;
         if(par<=0. || *end!=0 ) {  
                       SEND_ERROR("������������ ��������") ;
                                       return(-1) ;
                                 }
                      } 
/*-------------------------------------- ��������� ���������� ������ */

       status=EmiRoot_zoom("Main", "Get", &zoom) ;
    if(status) {
                   SEND_ERROR("����������� ����") ;
                      return(-1) ;
               }
/*------------------------------------------------ ������� ��������� */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - ���������� */
   if(delta_flag) {
                        zoom*=par ;
                  }
/*- - - - - - - - - - - - - - - - - - - - - - -  ���������� �������� */
   else           {
                        zoom =par ;
                  }
/*-------------------------------------- ��������� ���������� ������ */

       status=EmiRoot_zoom("Main", "Set", &zoom) ;
    if(status) {
                    return(-1) ;
               }
/*------------------------------------------------------ ����������� */

                          this->vShow("ZOOM") ;

    if(quiet_flag==0)  EmiRoot_redraw("Main") ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� LOOKAT                  */

  int  RSS_Module_Main::cLookAt(char *cmd) 

{
     int   quiet_flag ;        /* ���� ����������� ��������� */
    char   look_from[128] ; 
    char  *end ;

/*---------------------------------------------------- ������ ������ */

                                     quiet_flag=0 ;     

     if(strstr(cmd, "/q")!=NULL ||
        strstr(cmd, "/Q")!=NULL   ) {
                                       quiet_flag=1 ;

               end=strchr(cmd, ' ') ;
            if(end!=NULL)  cmd=end+1 ;
                                    }

/*--------------------------------------------------------- �������� */

   if(cmd[0]!=0) {
                        EmiRoot_lookfrom("Main", "Get", look_from) ;

    if(!stricmp(look_from, cmd)) {
                    SEND_ERROR("������ ������ ��� ������������ ��� '��������' ������") ;
                                     return(-1) ;
                               }
                 } 
/*-------------------------------------------------- ������� �� ���� */

                        EmiRoot_lookat("Main", "Set", cmd, NULL, NULL, NULL) ;

/*------------------------------------------------------ ����������� */

     if(quiet_flag==0)  EmiRoot_redraw("Main") ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� LOOKSHIFT               */
/*								    */
/*        LOOKSHIFT <dX> <dY> <dZ>                                  */

  int  RSS_Module_Main::cLookShift(char *cmd) 

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10 

             char  *pars[_PARS_MAX] ;
             char **xyz ;
           double   coord[_COORD_MAX] ;
              int   coord_cnt ;
           double   inverse ;
              int   xyz_flag ;          /* ���� ������ ����� ���������� */
              int   delta_flag ;        /* ���� ������ ���������� */
              int   arrow_flag ;        /* ���� ����������� ������ */
             char  *arrows ;
           double   look_dx ;
           double   look_dy ;
           double   look_dz ;
             char  *error ;
             char  *end ;
              int   status ;
              int   i ;

/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
                        xyz_flag=0 ;
                      delta_flag=0 ;
                      arrow_flag=0 ;

       if(*cmd=='/' ||
          *cmd=='+'   ) {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("������������ ������ �������") ;
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
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������ ���������� */        
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

                      xyz=&pars[0] ;   
                  inverse=   1. ; 

/*------------------------------------- ��������� ����������� ������ */

    if(arrow_flag) {                        
                         arrows=pars[0] ;

      if(strstr(arrows, "left" )!=NULL &&
         strstr(arrows, "ctrl" )==NULL   ) {  xyz_flag='X' ;  inverse=-1. ;  }
      if(strstr(arrows, "right")!=NULL &&  
         strstr(arrows, "ctrl" )==NULL   ) {  xyz_flag='X' ;  inverse= 1. ;  }  

      if(strstr(arrows, "up"   )!=NULL   ) {  xyz_flag='Y' ;  inverse= 1. ;  }
      if(strstr(arrows, "down" )!=NULL   ) {  xyz_flag='Y' ;  inverse=-1. ;  }

      if(strstr(arrows, "left" )!=NULL &&
         strstr(arrows, "ctrl" )!=NULL   ) {  xyz_flag='Z' ;  inverse=-1. ;  }
      if(strstr(arrows, "right")!=NULL &&  
         strstr(arrows, "ctrl" )!=NULL   ) {  xyz_flag='Z' ;  inverse= 1. ;  }  
           
                          xyz=&pars[1] ;   
                   }
/*------------------------------------------------- ������ ��������� */

    for(i=0 ; xyz[i]!=NULL && i<_COORD_MAX ; i++) {

             coord[i]=strtod(xyz[i], &end) ;
        if(*end!=0) {  
                       SEND_ERROR("������������ �������� ����������") ;
                                       return(-1) ;
                    }
                                                  }

                             coord_cnt=  i ;

                                 error= NULL ;
      if(xyz_flag) {
               if(coord_cnt==0)  error="�� ������� ���������� ��� �� ����������" ;
                   }
      else         {
               if(coord_cnt <3)  error="������ ���� ������� 3 ����������" ;
                   }

      if(error!=NULL)  SEND_ERROR(error) ;

      if(arrow_flag && coord_cnt>1)  coord[0]=coord[coord_cnt-1] ;  /* ��� ����������� ������ ����� � �������� ���� */
                                                                    /*  ����� ��������� ��������                    */
/*-------------------------------------- ��������� ���������� ������ */

       status=EmiRoot_lookat("Main", "Get",  NULL, &look_dx, &look_dy, &look_dz ) ;
    if(status) {
                   SEND_ERROR("����������� ����") ;
                      return(-1) ;
               }
/*------------------------------------------------ ������� ��������� */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - ���������� */
   if(delta_flag) {

          if(xyz_flag=='X')   look_dx+=inverse*coord[0] ;
     else if(xyz_flag=='Y')   look_dy+=inverse*coord[0] ;                 
     else if(xyz_flag=='Z')   look_dz+=inverse*coord[0] ;
                  }
/*- - - - - - - - - - - - - - - - - - - - - - -  ���������� �������� */
   else           {

          if(xyz_flag=='X')   look_dx=coord[0] ;
     else if(xyz_flag=='Y')   look_dy=coord[0] ;                 
     else if(xyz_flag=='Z')   look_dz=coord[0] ;
     else                   {
                              look_dx=coord[0] ;
                              look_dy=coord[1] ;
                              look_dz=coord[2] ;
                            }
                  }

/*-------------------------------------- ��������� ���������� ������ */

       status=EmiRoot_lookat("Main", "Set", NULL, &look_dx, &look_dy, &look_dz) ;
    if(status) {
                    return(-1) ;
               }
/*------------------------------------------------------ ����������� */

            EmiRoot_redraw("Main") ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� LOOKFROM                */

  int  RSS_Module_Main::cLookFrom(char *cmd) 

{
     int   quiet_flag ;        /* ���� ����������� ��������� */
    char   look_at[128] ; 
    char  *end ;
     int   i ;

#define   OBJECTS       RSS_Kernel::kernel->kernel_objects 
#define   OBJECTS_CNT   RSS_Kernel::kernel->kernel_objects_cnt 

/*---------------------------------------------------- ������ ������ */

                                     quiet_flag=0 ;     

     if(strstr(cmd, "/q")!=NULL ||
        strstr(cmd, "/Q")!=NULL   ) {
                                       quiet_flag=1 ;

               end=strchr(cmd, ' ') ;
            if(end!=NULL)  cmd=end+1 ;
                                    }
/*--------------------------------------------------------- �������� */

   if(cmd[0]!=0) {
                        EmiRoot_lookat("Main", "Get", look_at, NULL, NULL, NULL) ;

    if(!stricmp(look_at, cmd)) {
                    SEND_ERROR("������ ������ ��� ������������ ��� ������� ��� ������") ;
                                     return(-1) ;
                               }
                 } 
/*-------------------------------------------- ������������� ������� */

  if(*cmd!=0) { 

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, cmd))  break ;

    if(i==OBJECTS_CNT) {                                            /* ���� ��� �� �������... */
             SEND_ERROR("������ ��������� ������ �� ����������") ;
                             return(0) ;
                       }

                 RSS_Kernel::eye_object=OBJECTS[i] ;
              }
  else        {
                 RSS_Kernel::eye_object=NULL ;
              }
/*-------------------------------------------------- ������� �� ���� */

                        EmiRoot_lookfrom("Main", "Set", cmd) ;

/*------------------------------------------------------ ����������� */

                           this->vShow("ZOOM") ;

     if(quiet_flag==0)  EmiRoot_redraw("Main") ;

/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� LOOKINFO                */

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

/*------------------------------------------ ������ ��������� ������ */

                all_flag=0 ;

/*------------------------------------------------ ���������� ������ */

       status=EmiRoot_look("Main", "Get", &look_x, &look_y, &look_z,
                                          &look_a, &look_e, &look_r ) ;
    if(status) {
                   SEND_ERROR("����������� ����") ;
                      return(-1) ;
               }

              EmiRoot_zoom  ("Main", "Get", &zoom) ;
              EmiRoot_lookat("Main", "Get",  look_at, NULL, NULL, NULL) ;

/*-------------------------------------------- ������������ �������� */

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

/*-------------------------------------------------- ������ �������� */
/*- - - - - - - - - - - - - - - - - - - - - - - - - � ��������� ���� */
     if(all_flag) {

                  }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - � ������� ���� */
     else         {

        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_SHOW_INFO, (LPARAM)text) ;
                  }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� READ                    */

  int  RSS_Module_Main::cRead(char *cmd)

{
   OPENFILENAME  file_choice ;
           char *dat_filter="��� �����\0*.*\0"
                            "����� ��������\0*.sob\0"
                            "����� ����������\0*.sav\0"
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

/*------------------------------------------------- �������� ������� */

            sprintf(text, "RSS_Module_Main::cRead.0 heap=%d", _heapchk()) ;
             iDebug(text, NULL) ;

      strncpy(path, cmd, sizeof(path)-1) ;

/*---------------------------------------------- ������� ����� ����� */

    if(path[0]==0) {

           memset( path, 0, sizeof(path)) ;

	   memset(&file_choice, 0, sizeof(file_choice))  ;
  		   file_choice.lStructSize =sizeof(file_choice) ;   /* ����.�������� ������� */
		   file_choice.hwndOwner   = NULL ;
		   file_choice.hInstance   =GetModuleHandle(NULL) ;
		   file_choice.lpstrFilter =dat_filter ;
		   file_choice.nFilterIndex=  1  ;
  		   file_choice.lpstrFile   = path ;
		   file_choice.nMaxFile    =sizeof(path) ;
		   file_choice.lpstrTitle  ="������� ���� � �������" ;
		   file_choice.Flags       = OFN_ENABLEHOOK   |
                                             OFN_FILEMUSTEXIST ;
		   file_choice.lpfnHook    = iEmiRoot_read_HookProc ;

  	  status=GetOpenFileName(&file_choice) ;	            /* ������ ���� */
                          _chdir(__cwd) ;

       if(status==0)  return(-1) ;                                  /* ���� ���� �� ������... */

                   }
/*--------------------------------------------------- �������� ����� */

       file=fopen(path, "rt") ;
    if(file==NULL) {
                       SEND_ERROR("������ �������� �����") ;
                                       return(-1) ;
                   }

          sprintf(text, "cRead: %s", path) ;
           iDebug(text, NULL) ;

/*------------------------------------------------ ���������� ������ */

                  section_flag=0 ;
                        n_sect=0 ;

     while(1) {                                                     /* CIRCLE.1 - ��������� ������ ���� */
/*- - - - - - - - - - - - - - - - - - -  ���������� ��������� ������ */
                      memset(text, 0, sizeof(text)) ;
                   end=fgets(text, sizeof(text)-1, file) ;          /* ��������� ������ */
                if(end==NULL)  break ;

            if(text[0]==';')  continue ;                            /* �������� ����������� */

                end=text+strlen(text)-1 ;                           /* ������� ������ ����� ������ */
            if(*end=='\n')  *end=0 ;

              iDebug(text, NULL) ;
/*- - - - - - - - - - - - - - - - - - - - - - -  ������/����� ������ */
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
/*- - - - - - - - - - - - - - - - - - - - -  �������� ������� ������ */
       if(n_sect!=0 || buff.empty())  continue ;

             memset(text, 0, sizeof(text)) ;
            strncpy(text, buff.c_str(), sizeof(text)-1) ;
/*- - - - - - - - - - - - - - - - - - - - - ��������� ������ ������� */
       if(!memicmp(text, "#BEGIN MODULE ", 
                  strlen("#BEGIN MODULE "))) {

            this->kernel->vReadSave(&buff) ;                        /* ������ ������ ���� */

       for(i=0 ; i<this->kernel->modules_cnt ; i++)                 /* ������ ������ ������� */
            this->kernel->modules[i].entry->vReadSave(&buff) ;

                                                continue ;
                                             }
/*- - - - - - - - - - - - - - - - - - - -  ��������� ������ �������� */
       if(!memicmp(text, "#BEGIN OBJECT ", 
                  strlen("#BEGIN OBJECT "))) {

       for(i=0 ; i<this->kernel->modules_cnt ; i++)                 /* ������ ������ �������� */
            this->kernel->modules[i].entry->vReadSave(&buff) ;

                                                continue ;
                                             }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
              }                                                     /* CONTINUE.1 */
/*--------------------------------------------------- �������� ����� */

                fclose(file) ;

/*------------------------------------------------------ ����������� */

                this->cShowScene("") ;

/*-------------------------------------------------------------------*/

         sprintf(text, "RSS_Module_Main::cRead.E heap=%d", _heapchk()) ;
          iDebug(text, NULL) ;

   return(0) ;
}


  UINT_PTR CALLBACK  iEmiRoot_read_HookProc(  HWND hDlg,     UINT uiMsg, 
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
/*		      ���������� ���������� WRITE                   */
/*								    */
/*	WRITE [<��� �����>]                                         */
/*	WRITE/O [<��� �����>] <��� ������� 1> ... <��� ������� N>   */

  int  RSS_Module_Main::cWrite(char *cmd) 

{
   OPENFILENAME  file_choice ;
           char *dat_filter="��� �����\0*.*\0"
                            "����� ��������\0*.sob\0"
                            "����� ����������\0*.sav\0"
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

/*------------------------------------------------- �������� ������� */

                    memset(objects, 0, sizeof(objects)) ;

   if(*cmd=='/') {
                   end=strchr(cmd, ' ') ;
                if(end==NULL) {  
                       SEND_ERROR("������������ ������ �������") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(*cmd!='o' ||
                   *cmd!='O'   ) {
                       SEND_ERROR("����������� ���� �������") ;
                                       return(-1) ;
                                 }

                    cmd=end+1 ;
                   end=strchr(cmd, ' ') ;
                if(end==NULL) {  
                       SEND_ERROR("������������ ������ �������") ;
                                       return(-1) ;
                              }
                   *end=0 ;

                     strncpy(path, cmd, sizeof(path)-1) ;           /* ���������� ���� ����� */

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
/*---------------------------------------------- ������� ����� ����� */

    if(path[0]==0) {

           memset( path, 0, sizeof(path)) ;

	   memset(&file_choice, 0, sizeof(file_choice))  ;
  		   file_choice.lStructSize =sizeof(file_choice) ;   /* ����.�������� ������� */
		   file_choice.hwndOwner   = NULL ;
		   file_choice.hInstance   =GetModuleHandle(NULL) ;
		   file_choice.lpstrFilter =dat_filter ;
		   file_choice.nFilterIndex=  1  ;
  		   file_choice.lpstrFile   = path ;
		   file_choice.nMaxFile    =sizeof(path) ;
		   file_choice.lpstrTitle  ="������� ���� ��� ����������" ;
		   file_choice.Flags       = OFN_ENABLEHOOK ;
		   file_choice.lpfnHook    = iEmiRoot_read_HookProc ;

  	  status=GetOpenFileName(&file_choice) ;	            /* ������ ���� */
                          _chdir(__cwd) ;

       if(status==0)  return(-1) ;                                  /* ���� ���� �� ������... */

                   }
/*--------------------------------------------------- �������� ����� */

       file=fopen(path, "wt") ;
    if(file==NULL) {
                       SEND_ERROR("������ �������� �����") ;
                                       return(-1) ;
                   }
/*---------------------------------------------------- ������ ������ */
/*- - - - - - - - - - - - - - - - - - - - - - - ������ ������ ������ */
    if(objects[0]==NULL) {

            this->kernel->vWriteSave(&buff) ;                       /* ������ ������ ���� */

                    data=buff.c_str() ;       
             fwrite(data, 1, strlen(data), file) ;
          if(ferror(file)) {
                       SEND_ERROR("������ ������ �����") ;
                                       return(-1) ;
                           }

       for(i=0 ; i<this->kernel->modules_cnt ; i++) {               /* ������ ������ ������� */

            this->kernel->modules[i].entry->vWriteSave(&buff) ;
        
            data=buff.c_str() ;
         if(data==NULL || strlen(data)<10)  continue ;
       
             fwrite(data, 1, strlen(data), file) ;
          if(ferror(file)) {
                       SEND_ERROR("������ ������ �����") ;
                                       return(-1) ;
                           }
                                                    }

      if(this->kernel->kernel_objects)
       for(i=0 ; i<this->kernel->kernel_objects_cnt ; i++) {        /* ������ ������ �������� */

            this->kernel->kernel_objects[i]->vWriteSave(&buff) ;
        
            data=buff.c_str() ;
         if(data==NULL || strlen(data)<10)  continue ;
       
             fwrite(data, 1, strlen(data), file) ;
          if(ferror(file)) {
                       SEND_ERROR("������ ������ �����") ;
                                       return(-1) ;
                           }
                                                           }
         
                         }
/*- - - - - - - - - - - - - - - ������ ������ ��� ��������� �������� */
    else                 {

      if(this->kernel->kernel_objects)
       for(i=0 ; i<this->kernel->kernel_objects_cnt ; i++) {        /* ������ ������ �������� */

         for(j=0 ; objects[j]!=NULL ; j++)                          /* ��������� ������ �� ������ ������ */
           if(!stricmp(objects[j], 
                        this->kernel->kernel_objects[i]->Name))  break ;

           if(objects[j]==NULL)  continue ;                         /* ���� ������ �� ���������... */

            this->kernel->kernel_objects[i]->vWriteSave(&buff) ;
        
            data=buff.c_str() ;
         if(data==NULL || strlen(data)<10)  continue ;
       
             fwrite(data, 1, strlen(data), file) ;
          if(ferror(file)) {
                       SEND_ERROR("������ ������ �����") ;
                                       return(-1) ;
                           }
                                                           }
                         }
/*--------------------------------------------------- �������� ����� */

              fclose(file) ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� THREADS                 */

  int  RSS_Module_Main::cThreads(char *cmd)

{ 
      EmiRoot_threads("SHOW", NULL) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� STOP                    */

  int  RSS_Module_Main::cStop(char *cmd)

{ 
      RSS_Kernel::kernel->stop=1 ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� NEXT                    */

  int  RSS_Module_Main::cNext(char *cmd)

{ 
      RSS_Kernel::kernel->next=_RSS_KERNEL_NEXT_STEP ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� RESUME                  */

  int  RSS_Module_Main::cResume(char *cmd)

{
      RSS_Kernel::kernel->next=0 ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� MODULES                 */

  int  RSS_Module_Main::cModules(char *cmd)

{ 
    EmiRoot_modules("CREATE") ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� MEMORY                  */

  int  RSS_Module_Main::cMemory(char *cmd)

{ 
/*---------------------------------------- ������ ����������� ������ */

   if(!stricmp(cmd, "ON"  )) {
                                gMemOnOff(_MEMCHECK_ACTUAL) ;
                             }
/*---------------------------------------- ������������ ����� ������ */

   if(!stricmp(cmd, "LIST")) {
                                gMemList("memory.list") ;
                             }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*                   ���������� ���������� SRAND                    */

  int  RSS_Module_Main::cSrand(char *cmd)

{ 
   char *end ;
   long  value ;

/*------------------------------------------------------- ���������� */

       value=strtoul(cmd, &end, 10) & 0x00007fffL ;

/*--------------------------------- ������� �����-����� � ����-����� */

   if(!stricmp(cmd, "AUTO"  )) {
                                  this->srand_fixed=0 ;

                                   srand((unsigned int)time(NULL)) ;
                               }
/*----------------------------------- ������� �����-����� ���������� */

   else
   if(value>1) {
                                  this->srand_fixed=1 ;
                                    srand(value) ;
               }
/*---------------------------------- ��������� ������������� ������� */

   else        {
                       SEND_ERROR("������������ ������ �������") ;
               }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� TIME                    */
/*								    */
/*        TIME   <�����>                                            */
/*        TIME/C <�����>                                            */
/*        TIME/S <�����>                                            */

  int  RSS_Module_Main::cTime(char *cmd) 

{
#define   _PARS_MAX  10 

             char  *pars[_PARS_MAX] ;
              int   s_flag ;
              int   c_flag ;
           double   value ;
             char  *end ;
              int   i ;

/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
                        c_flag=0 ;
                        s_flag=0 ;

       if(*cmd=='/' ||
          *cmd=='+'   ) {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("������������ ������ �������") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, 'c')!=NULL ||
                   strchr(cmd, 'C')!=NULL   )  c_flag=1 ;
           else if(strchr(cmd, 's')!=NULL ||
                   strchr(cmd, 'S')!=NULL   )  s_flag=1 ;

                           cmd=end+1 ;
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������ ���������� */        
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }


       if(c_flag==0 &&
          s_flag==0   ) {  c_flag=1 ;
                           s_flag=1 ;  }
                
/*----------------------------------------------- ��������� �������� */

        value=strtod(pars[0], &end) ;

    if(value<=0. ||
        *end!=0    ) {
                       SEND_ERROR("������������ �������� ���� �������") ;
                          return(-1) ;
                     }

    if(c_flag)  this->kernel->calc_time_step=value ;
    if(s_flag)  this->kernel->show_time_step=value ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*                    ����������� ������                            */

    void  RSS_Module_Main::ShowExecute(char *layer)

{
   int  status ;
   int  i ;

/*-------------------------------------------- ������������ �������� */

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

/*--------------------------------------------- ����������� �������� */

          status=this->display.SetFirstContext(NULL) ;
    while(status==0) {
             
                 this->display.ShowContext(NULL) ;
          status=this->display.SetNextContext(NULL) ;
                     }
/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*                   ���������� ������ � �����                      */

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


/*********************************************************************/
/*								     */
/*	      ���������� ������ "������� ���������"	             */
/*								     */
/*********************************************************************/

/*********************************************************************/
/*								     */
/*	       ����������� ������ "������� ���������"      	     */

     RSS_Transit_Main::RSS_Transit_Main(void)

{
}


/*********************************************************************/
/*								     */
/*	        ���������� ������ "������� ���������"      	     */

    RSS_Transit_Main::~RSS_Transit_Main(void)

{
}


/********************************************************************/
/*								    */
/*	              ���������� ��������                           */

    int  RSS_Transit_Main::vExecute(void)

{
   HWND  hWnd ;
   RECT  rect ;
    int  x ; 
    int  y ; 
    int  w_x ; 
    int  w_y ; 

/*------------------------------------- ����������� ������� �������� */

   if(!stricmp(action, "ALL")) {

       hWnd=CreateDialogIndirect(GetModuleHandle(NULL),
                                  (LPCDLGTEMPLATE)Kernel.Resource("IDD_OBJECTS_LIST", RT_DIALOG),
                                     GetActiveWindow(), Main_ObjectsList_dialog) ;

                    GetWindowRect(hWnd, &rect);
                              w_x=rect.bottom-rect.top +1 ;
                              w_y=rect.right -rect.left+1 ;

                                 x= 0 ;
                                 y= 0 ;
         if(details[0]=='1') {   x=  w_x+  (RSS_Kernel::display.x-4*w_x)/3 ;
                                 y= 0 ;                                       }
         if(details[0]=='2') {   x=2*w_x+2*(RSS_Kernel::display.x-4*w_x)/3 ;
                                 y= 0 ;                                       }
         if(details[0]=='3') {   x= RSS_Kernel::display.x-w_x ;
                                 y= 0 ;                                       }
         if(details[0]=='4') {   x= RSS_Kernel::display.x-w_x ;
                                 y=(RSS_Kernel::display.y-w_y)/2 ;            }
         if(details[0]=='5') {   x= RSS_Kernel::display.x-w_x ;
                                 y= RSS_Kernel::display.y-w_y ;               }
         if(details[0]=='6') {   x=2*w_x+2*(RSS_Kernel::display.x-4*w_x)/3 ;
                                 y= RSS_Kernel::display.y-w_y ;               }
         if(details[0]=='7') {   x=  w_x+  (RSS_Kernel::display.x-4*w_x)/3 ;
                                 y= RSS_Kernel::display.y-w_y ;               }
         if(details[0]=='8') {   x= 0 ;
                                 y= RSS_Kernel::display.y-w_y ;               }
         if(details[0]=='9') {   x= 0 ;
                                 y=(RSS_Kernel::display.y-w_y)/2 ;            }


                     SetWindowPos(hWnd, NULL, x, y, 0, 0,
                                   SWP_NOOWNERZORDER | SWP_NOSIZE);
                       ShowWindow(hWnd, SW_SHOW) ;

                         SetFocus(Kernel.kernel_wnd) ;

                               }
/*------------------------------------- ����������� ������� �������� */

   return(0) ;
}



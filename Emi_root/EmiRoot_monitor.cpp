/*********************************************************************/
/*                                                                   */
/*         ������� ������������ 2D-������������� �������������       */
/*                                                                   */
/*                   ��������� �����������                           */
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


/*--------------------------------------------- ��������� ���������� */

 extern  RSS_Module_Main  Kernel ;    /* ��������� ���� */

#define  SEND_ERROR(text)      SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                           (WPARAM)_USER_ERROR_MESSAGE,      \
                                           (LPARAM) text)
#define  SHOW_COMMAND(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                           (WPARAM)_USER_SHOW_COMMAND,       \
                                           (LPARAM) text)
#define  SHOW_THREAD(text)     SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                           (WPARAM)_USER_THREAD_MESSAGE,     \
                                           (LPARAM) text)

/*-------------------------------------------------- �������� ������ */

 typedef  struct {  DWORD  Thread_id ;
                     char  desc[1024] ; }  WorkThread ;


/*********************************************************************/
/*								     */
/* 	                ���������� ������� �������                   */

  int  EmiRoot_threads(char *action, char *thread_desc)
{
  static  HWND  hMonitor ;
    WorkThread  thread_data ;
          char  text[1024] ;
          char *end ;

/*-------------------------------- �������� �������� ������� ������� */

     if(!stricmp(action, "CREATE")) {

         hMonitor=CreateDialog(GetModuleHandle(NULL), "IDD_THREADS",
                                  GetActiveWindow(), Main_Threads_dialog) ;
      if(hMonitor==NULL) {
                 sprintf(text, "Thread monitor create error %d", GetLastError()) ;
           EmiRoot_error(text, MB_ICONERROR) ;
	                          return(-1) ;
                         }
                                    }
/*----------------------------------- ����� �������� ������� ������� */

     if(!stricmp(action, "SHOW")) {

  	          ShowWindow(hMonitor, SW_SHOW) ;
                                  }
/*--------------------------------- ������� �������� ������� ������� */

     if(!stricmp(action, "HIDE")) {

  	          ShowWindow(hMonitor, SW_HIDE) ;
                                  }
/*-------------------------------------- ����������� �������� ������ */

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
/*--------------------------------------- ���������� �������� ������ */

     if(!stricmp(action, "DELETE")) {

                     thread_data.Thread_id=GetCurrentThreadId() ;

          SendMessage(hMonitor, WM_USER, (WPARAM)_USER_THREAD_DELETE,
                                         (LPARAM)&thread_data) ;
                                    }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/*********************************************************************/
/*								     */
/* 	     ���������� ��������� ����������� ���� THREADS           */

  INT_PTR CALLBACK  Main_Threads_dialog(  HWND hDlg,     UINT Msg, 
                                        WPARAM wParam, LPARAM lParam) 
{
    WorkThread *thread_data ;
         DWORD  thread_id ;
           int  elm ;         /* ������������� �������� ������� */
       LRESULT  status ;
       LRESULT  index ;
           int  cnt ;
          char  text[1024] ;

/*------------------------------------------------- ������� �������� */

  switch(Msg) {

/*---------------------------------------------------- ������������� */

    case WM_INITDIALOG: {
/*- - - - - - - - - - - - - - - - - - - - -  ������������� ��������� */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}
/*------------------------------------ ��������� ���������� �������� */

    case WM_USER:  {
/*- - - - - - - - - - - - - - - - - - -  ����������� �������� ������ */
        if(wParam==_USER_THREAD_ADD) {

             thread_data=(WorkThread *)lParam ;
        
         index=LB_ADD_ROW(IDC_THREADS_LIST, thread_data->desc) ;
              LB_SET_ITEM(IDC_THREADS_LIST, index, thread_data->Thread_id) ;

              SHOW_THREAD(thread_data->desc) ;
                                       return(FALSE) ;
                                     }
/*- - - - - - - - - - - - - - - - - - - - ���������� �������� ������ */
        if(wParam==_USER_THREAD_DELETE) {

             thread_data=(WorkThread *)lParam ;
       
                     cnt=(int)LB_GET_COUNT(IDC_THREADS_LIST) ;

           for(index=0 ; index<cnt ; index++) {
                 thread_id=(DWORD)LB_ROW_ITEM(IDC_THREADS_LIST, index) ;
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
/*- - - - - - - - - - - - - - - - - - - - - - - -  ���������� ������ */
        if(wParam==_KEY_QUIT) {
                                   EndDialog(hDlg, 0) ;
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
                           EmiRoot_threads("HIDE", NULL) ;
//                               EndDialog(hDlg, 0) ;
      		                     return(FALSE) ;
		  	                    break ;
			}
/*----------------------------------------------------------- ������ */

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
/* 	     ����������� ������ �������������� �������               */

  int  EmiRoot_modules(char *action)
{

/*----------------------------------------- �������� ���� �� ������� */

     if(!stricmp(action, "CREATE")) {

          DialogBox(GetModuleHandle(NULL), "IDD_MODULES",
                        GetActiveWindow(), Main_Modules_dialog) ;

                                    }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/*********************************************************************/
/*								     */
/* 	     ���������� ��������� ����������� ���� MODULES           */

  INT_PTR CALLBACK  Main_Modules_dialog(  HWND hDlg,     UINT Msg, 
                                        WPARAM wParam, LPARAM lParam) 
{
    static  HFONT  font ;        /* ����� */
 class RSS_Kernel *entry ;
              int  elm ;         /* ������������� �������� ������� */
              int  status ;
          LRESULT  index ;
             char  tmp[1024] ;
              int  i ;

/*------------------------------------------------- ������� �������� */

  switch(Msg) {

/*---------------------------------------------------- ������������� */

    case WM_INITDIALOG: {
/*- - - - - - - - - - - - - - - - - - - - - - - - ���������� ������� */
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
/*- - - - - - - - - - - - - - - - - - - - -  ������������� ��������� */
     for(i=0 ; i<Kernel.modules_cnt ; i++) {

        Kernel.modules[i].entry->vGetParameter("$$MODULE_NAME", tmp) ;

              index=LB_ADD_ROW (IDC_MODULES_LIST, tmp) ;
                    LB_SET_ITEM(IDC_MODULES_LIST, index, Kernel.modules[i].entry) ;
                                           }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}
/*------------------------------------------------ ��������� ������� */

    case WM_COMMAND:    {

	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - - - - ������ � ����������� ������� */
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
/*--------------------------------------------------------- �������� */

    case WM_CLOSE:      {
                             EndDialog(hDlg, 0) ;
  		                return(FALSE) ;
			               break ;
			}
/*----------------------------------------------------------- ������ */

    default :        {
			  return(FALSE) ;
			    break ;
		     }
/*-------------------------------------------------------------------*/
	      }
/*-------------------------------------------------------------------*/

    return(TRUE) ;
}



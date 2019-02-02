/*********************************************************************/
/*                                                                   */
/*         ������� ������������ 2D-������������� �������������       */
/*                                                                   */
/*                   ���������� ���������                            */
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

#pragma warning(disable : 4996)


#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text) ;

#include "resource.h"


/*********************************************************************/
/*								     */
/* 	     ���������� ��������� ����������� ���� HELP	             */

  INT_PTR CALLBACK  Main_Help_dialog(  HWND hDlg,     UINT Msg, 
                                     WPARAM wParam, LPARAM lParam) 
{
  RSS_Module_Main  Module ;
              int  elm ;         /* ������������� �������� ������� */
          LRESULT  status ;
          LRESULT  index ;
              int  insert_flag ;
             char *help ;
             char  text[512] ;
             char *end ;
              int  i ;

/*------------------------------------------------- ������� �������� */

  switch(Msg) {

/*---------------------------------------------------- ������������� */

    case WM_INITDIALOG: {
/*- - - - - - - - - - - - - - - - - - - - -  ������������� ��������� */
   for(i=0 ; Module.mInstrList[i].name_full!=NULL ; i++) {

     if(Module.mInstrList[i].help_row==NULL)  continue ;

         index=LB_ADD_ROW (IDC_HELP_LIST, Module.mInstrList[i].help_row) ;
               LB_SET_ITEM(IDC_HELP_LIST, index, Module.mInstrList[i].help_full) ;
                                                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}

/*------------------------------------ ��������� ���������� �������� */

    case WM_USER:  {
/*- - - - - - - - - - - - - - - - - - - - - - -  ��������� �� ������ */
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
/*- - - - - - - - - - - - - - - - - - - - - - - -  ��������� ������� */
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


/*********************************************************************/
/*								     */
/*         ���������� ��������� ����������� ���� OBJECTS LIST        */

  INT_PTR CALLBACK  Main_ObjectsList_dialog(  HWND hDlg,     UINT Msg, 
                                            WPARAM wParam, LPARAM lParam) 
{
  static HWND  hWnd_active_prv ;
          int  elm ;               /* ������������� �������� ������� */
          int  status ;
          int  i ;

#define   OBJECTS       RSS_Kernel::kernel->kernel_objects 
#define   OBJECTS_CNT   RSS_Kernel::kernel->kernel_objects_cnt 
     
/*------------------------------------------------- ������� �������� */

  switch(Msg) {

/*---------------------------------------------------- ������������� */

    case WM_INITDIALOG: {
/*- - - - - - - - - - - - - - - - - - - -  ���������� ��������� ���� */
                    hWnd_active_prv=RSS_Kernel::active_wnd ;
             RSS_Kernel::active_wnd= hDlg ;
/*- - - - - - - - - - - - - - - - - - - - -  ������������� ��������� */
                 LB_CLEAR(IDC_OBJECTS_LIST) ;

       for(i=0 ; i<OBJECTS_CNT ; i++) {
                                          OBJECTS[i]->vFormDecl() ;
             LB_ADD_ROW(IDC_OBJECTS_LIST, OBJECTS[i]->Decl) ;
                                      }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}
/*------------------------------------ ��������� ���������� �������� */

    case WM_USER:  {
/*- - - - - - - - - - - - - - - - - - - - - - -  ��������� �� ������ */
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
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - ����� ������ */
#if 0
     if(elm==IDC_CREATE) {

                  GETs(IDC_NAME,         data->name) ;
                  GETs(IDC_LIBRARY_PATH, data->lib_path ) ;
                  GETc(IDC_MODEL,        data->model) ;

          for(i=0 ; i<5 ; i++)
                  GETs(IDC_PAR_VALUE_1+i, data->pars[i].value) ;  

                status=Module->CreateObject(data) ;
             if(status==0)  EndDialog(hDlg, 0) ;

                              return(FALSE) ;
                         }
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			  return(FALSE) ;
			     break ;
			}
/*--------------------------------------------------------- �������� */

    case WM_CLOSE:      {
                    
                          RSS_Kernel::active_wnd=hWnd_active_prv ;  /* �������������� ��������� ���� */

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

#undef   OBJECTS
#undef   OBJECTS_CNT

    return(TRUE) ;
}



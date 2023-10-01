/********************************************************************/
/*								    */
/*		������ ���������� ����������� "������� ���" 	    */
/*								    */
/*                   ���������� ���������                           */
/*                                                                  */
/********************************************************************/

#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <math.h>

#include "..\Emi_root\controls.h"

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Unit\RSS_Unit.h"
#include "..\RSS_Kernel\RSS_Kernel.h"
#include "..\RSS_Model\RSS_Model.h"

#include "U_EWunit.h"

#pragma warning(disable : 4996)

#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text) ;

#include "resource.h"


   void *Ptr_decode(char *text) ;
   char *Ptr_incode(void *ptr ) ;


/*********************************************************************/
/*								     */
/* 	     ���������� ��������� ����������� ���� HELP	             */

  INT_PTR CALLBACK  Unit_EWunit_Help_dialog(  HWND hDlg,     UINT Msg, 
                                            WPARAM wParam, LPARAM lParam) 
{
  RSS_Module_EWunit  Module ;
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
/* 	     ���������� ��������� ����������� ���� ROUND             */

 typedef struct {  RSS_Unit_EWunit *data ;
                              HWND  hDlg ; }  Indicator_context ;


#define  _IND_MAX  10

  INT_PTR CALLBACK  Unit_EWunit_Show_dialog(  HWND  hDlg,     UINT  Msg, 
                                            WPARAM  wParam, LPARAM  lParam) 
{
  static Indicator_context   contexts[_IND_MAX] ;
           RSS_Unit_EWunit  *context ;
                      HWND   hElem  ;
                       int   elm ;           /* ������������� �������� ������� */
                       int   status ;
                      char   title[1024] ;
                       int   i ;

/*-------------------------------------------- ����������� ��������� */

                                    context=NULL ;
    for(i=0 ; i<_IND_MAX ; i++) 
      if(contexts[i].hDlg==hDlg) {  context=contexts[i].data ;
                                               break ;           }

/*------------------------------------------------- ������� �������� */

  switch(Msg) {

/*---------------------------------------------------- ������������� */

    case WM_INITDIALOG: {

                 context=(RSS_Unit_EWunit *)lParam ;
/*- - - - - - - - - - - - - - - - - -  �������� ���������� ��������� */
           for(i=0 ; i<_IND_MAX ; i++) 
             if(contexts[i].data==context) {
                            EndDialog(contexts[i].hDlg, 0) ;
                                      contexts[i].hDlg=NULL ;
                                           }
/*- - - - - - - - - - - - - - - - - - - - - -  ����������� ��������� */
           for(i=0 ; i<_IND_MAX ; i++) 
             if(contexts[i].hDlg==NULL)  break ;

                contexts[i].hDlg=  hDlg ;
                contexts[i].data=context ;
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������� ��������� */
              sprintf(title, "��������� �����: %s.%s",
                                  context->Owner->Name, context->Name) ;
          SendMessage(hDlg, WM_SETTEXT, 0, (LPARAM)title) ;
/*- - - - - - - - - - - - - - - - - - - - -  ������������� ��������� */
                      hElem=GetDlgItem(hDlg, IDC_INDICATOR) ;
          SendMessage(hElem, WM_SETTEXT, NULL, 
                       (LPARAM)Ptr_incode((void *)context)) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}
/*------------------------------------------------ ��������� ������� */

    case WM_PAINT:      {

                      hElem=GetDlgItem(hDlg, IDC_INDICATOR) ;
          SendMessage(hElem, WM_PAINT, NULL, NULL) ;
                          
 			    return(FALSE) ;
			       break ;
			}
/*------------------------------------------------ ��������� ������� */

    case WM_COMMAND:    {
                            status=HIWORD(wParam) ;
                               elm=LOWORD(wParam) ;

			  return(FALSE) ;
			     break ;
			}
/*--------------------------------------------------------- �������� */

    case WM_CLOSE:      {

         for(i=0 ; i<_IND_MAX ; i++)                                /* ������� ������ �� ���������� */
           if(contexts[i].hDlg==hDlg) {  contexts[i].hDlg=NULL ;    /*   �� ������ ����������       */
                                                 break ;         }

                             context->hWnd=NULL ;

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
/*                                                                   */
/*            ������� - ��������� �������� ��������                  */

 LRESULT CALLBACK  Unit_EWunit_Indicator_prc(
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
      RSS_Unit_EWunit *data ;               /* �������� ���������� */
                 char  data_ptr[32] ;       /* ����� ��������, ������������ */
                  int  rad ;
                  int  x_c ;
                  int  y_c ;
               double  angle ;
               double  dx, dz ;
               double  step ;
                  int  i ;

#define  _PI  3.1415926

/*------------------------------------------------ ���������� ������ */

   if(Msg==WM_PAINT        ||
      Msg==WM_LBUTTONDBLCLK  ) {
  
        SendMessage(hWnd, WM_GETTEXT, (WPARAM)sizeof(data_ptr),
                                      (LPARAM)       data_ptr  ) ;

           data=(RSS_Unit_EWunit *)Ptr_decode(data_ptr) ;
                               }
/*--------------------------------------------------- ����� �������� */

  switch(Msg) {

/*----------------------------------------------- �������� ��������� */

    case WM_CREATE:   break ;

    case WM_COMMAND:  break ;

/*-------------------------------------------------------- ��������� */

    case WM_PAINT:   {
/*- - - - - - - - - - - - - - - - - - - - - - - -  ���������� ������ */
                InvalidateRect(hWnd, NULL, false) ;

        if(data==NULL) {
                          ValidateRect(hWnd,  NULL) ;
                              break ;
                       }
/*- - - - - - - - - - - - - - - - - -  �������� "��������" ��������� */
       hDC_wnd=BeginPaint(hWnd, &PaintCfg) ;

           hDC=CreateCompatibleDC    (hDC_wnd) ; 
           hBM=CreateCompatibleBitmap(hDC_wnd, GetDeviceCaps(hDC_wnd, HORZRES), 
                                               GetDeviceCaps(hDC_wnd, VERTRES) ) ; 
                         SelectObject(hDC, hBM) ;
/*- - - - - - - - - - - - - - - - - - - - - �������������� ��������� */
               GetClientRect(hWnd, &Rect) ;

                 x_c=(Rect.left+Rect.right)/2 ;
                 y_c=(Rect.bottom+Rect.top)/2 ;
                 rad=(Rect.bottom-Rect.top)/2-10 ;
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������� ���� ���� */
             Brush    =    CreateSolidBrush(RGB(192, 192, 192)) ;
             Brush_prv=(HBRUSH)SelectObject(hDC, Brush) ;

                                   FillRect(hDC, &Rect, Brush) ;

                               SelectObject(hDC, Brush_prv) ;
                               DeleteObject(Brush) ;
/*- - - - - - - - - - - - - - - - - - - ������������ ��������� ����� */
                                        step=_PI/72. ;

           for(i=0 ; i<data->threats_cnt ; i++) {

                                 color=RGB(255, 0, 0) ;

                 dx=data->threats[i]->state_0.x-data->Owner->state.x ;
                 dz=data->threats[i]->state_0.z-data->Owner->state.z ;

              angle=1.5*_PI-(atan2(dx, dz)-data->Owner->state.azim*_GRD_TO_RAD) ;

                           Pen    =           CreatePen(PS_SOLID, 0, color) ;
                           Pen_prv=(HPEN)  SelectObject(hDC, Pen) ;
                         Brush    =    CreateSolidBrush(color) ;
                         Brush_prv=(HBRUSH)SelectObject(hDC, Brush) ;

#pragma warning(disable : 4244)
                                  Pie(hDC, x_c-rad, y_c-rad, 
                                           x_c+rad, y_c+rad,
                                           x_c+rad*cos(angle+step/2.),
                                           y_c+rad*sin(angle+step/2.),
                                           x_c+rad*cos(angle-step/2.),
                                           y_c+rad*sin(angle-step/2.) ) ;
#pragma warning(default : 4244)

                                   SelectObject(hDC, Brush_prv) ;
                                   SelectObject(hDC, Pen_prv) ;
                                   DeleteObject(Brush) ;
                                   DeleteObject(Pen) ;
                                                }
/*- - - - - - - - - - - - - - - - - - - -  �������������� ���������� */
               Pen    =           CreatePen(PS_SOLID, 0, 0) ;
               Pen_prv=(HPEN)  SelectObject(hDC, Pen) ;

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
/*- - - - - - - - - - - - - - - - - - - - - - - - -  ����� ��������� */
            BitBlt(hDC_wnd, 
                   Rect.left, Rect.top, 
                   Rect.right-Rect.left, Rect.bottom-Rect.top, 
                   hDC,
                   Rect.left, Rect.top, 
                   SRCCOPY                           ) ;

                    ValidateRect(hWnd,  NULL) ;
                        EndPaint(hWnd, &PaintCfg) ;
/*- - - - - - - - - - - - - - - - - - - - - -  ������������ �������� */
            if(hBM!=NULL)  DeleteObject(hBM) ;
            if(hDC!=NULL)  DeleteDC    (hDC) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			     break ;
		     }
/*------------------------------------------------- ������ ��������� */

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
/*                �������������� ������ ������ � ������              */
/*                �������������� ������ � ����� ������               */

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



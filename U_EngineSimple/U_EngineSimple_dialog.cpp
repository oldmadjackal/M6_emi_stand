/********************************************************************/
/*								    */
/*    МОДУЛЬ УПРАВЛЕНИЯ КОМПОНЕНТОМ "ПРОСТОЙ РАКЕТНЫЙ ДВИГАТЕЛЬ"    */
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
#include "..\RSS_Unit\RSS_Unit.h"
#include "..\RSS_Kernel\RSS_Kernel.h"
#include "..\RSS_Model\RSS_Model.h"
#include "..\Ud_tools\UserDlg.h"

#include "U_EngineSimple.h"

#pragma warning(disable : 4996)

#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text) ;

#include "resource.h"


/*********************************************************************/
/*								     */
/* 	     Обработчик сообщений диалогового окна HELP	             */

  INT_PTR CALLBACK  Unit_EngineSimple_Help_dialog(  HWND hDlg,     UINT Msg, 
                                                  WPARAM wParam, LPARAM lParam) 
{
  RSS_Module_EngineSimple  Module ;
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
/* 	     Обработчик сообщений диалогового окна VIEW             */

 typedef struct {  RSS_Unit_EngineSimple *unit ;
                            UD_diag_data  data ;
                                    HWND  hDlg ;  }  Indicator_context ;

#define  _IND_MAX  10

  INT_PTR CALLBACK  Unit_EngineSimple_View_dialog(  HWND  hDlg,     UINT  Msg, 
                                                  WPARAM  wParam, LPARAM  lParam) 
{
  static Indicator_context   contexts[_IND_MAX] ;
              UD_diag_data  *context ;
     RSS_Unit_EngineSimple  *unit ;
    struct UD_data_2Dpoint  *data ;
                      HWND   hElem  ;
                       int   elm ;           /* Идентификатор элемента диалога */
                       int   status ;
                      char   title[1024] ;
                       int   i ;
                       int   j ;
                       int   k ;

/*-------------------------------------------- Определение контекста */

                                    context= NULL ;
                                       unit= NULL ;
    for(i=0 ; i<_IND_MAX ; i++) 
      if(contexts[i].hDlg==hDlg) {  context=&contexts[i].data ;
                                       unit= contexts[i].unit ;
                                               break ;           }

/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {

                 unit=(RSS_Unit_EngineSimple *)lParam ;
/*- - - - - - - - - - - - - - - - - -  Контроль повторного контекста */
           for(i=0 ; i<_IND_MAX ; i++) 
             if(contexts[i].unit==unit) {
                            EndDialog(contexts[i].hDlg, 0) ;
                                      contexts[i].hDlg=NULL ;
                                        }
/*- - - - - - - - - - - - - - - - - - - - - -  Регистрация контекста */
           for(i=0 ; i<_IND_MAX ; i++) 
             if(contexts[i].hDlg==NULL)  break ;

                contexts[i].hDlg=hDlg ;
                contexts[i].unit=unit ;
/*- - - - - - - - - - - - - - - - - Формирование отображаемых данных */
       if(unit->profile_cnt!=0) { 

#define   G_LIST  contexts[i].data.data_list

           G_LIST=(UD_data_list **)calloc(8, sizeof(*G_LIST)) ;

           contexts[i].data.back_color   =RGB(255, 255, 255) ;
           contexts[i].data.data_list_cnt= 8 ;

          for(j=0 ; j<8 ; j++) {

                   G_LIST[j]=(UD_data_list *)calloc(1, sizeof(UD_data_list)) ;
                                          
             if( j==0                   ||
                (j==1 && unit->use_mass)   )  G_LIST[j]->use_flag=1 ;

             if(j==0)  G_LIST[j]->color= RGB(255,   0,   0) ;
             if(j==1)  G_LIST[j]->color= RGB(  0, 255,   0) ;

               G_LIST[j]->x_scale_type=_UD_ZERO_SCALE ;
               G_LIST[j]->x_scale_type=_UD_ZERO_SCALE ;
               G_LIST[j]->type        =_UD_MONO_DATA ;
               G_LIST[j]->data_cnt    = unit->profile_cnt ;
               G_LIST[j]->data        = calloc(unit->profile_cnt, sizeof(UD_data_2Dpoint)) ;
                          data        =(UD_data_2Dpoint *)G_LIST[j]->data ;

            for(k=0 ; k<unit->profile_cnt ; k++) {
                         data[k].x=unit->profile[k].t ;
               if(j==0)  data[k].y=unit->profile[k].thrust ;
               if(j==1)  data[k].y=unit->profile[k].mass ;
               if(j==2)  data[k].y=unit->profile[k].Cx ;
               if(j==3)  data[k].y=unit->profile[k].Cy ;
               if(j==4)  data[k].y=unit->profile[k].Cz ;
               if(j==5)  data[k].y=unit->profile[k].Mx ;
               if(j==6)  data[k].y=unit->profile[k].My ;
               if(j==7)  data[k].y=unit->profile[k].Mz ;
                                                 }
                               }

               G_LIST[0]->grid_flag       =1 ;
               G_LIST[0]->grid_values_flag=1 ;

#undef   G_LIST
                                } 
/*- - - - - - - - - - - - - - - - - - - - - - - -  Пропись заголовка */
              sprintf(title, "Профиль работы РДТТ: %s.%s",
                                  unit->Owner->Name, unit->Name) ;
          SendMessage(hDlg, WM_SETTEXT, 0, (LPARAM)title) ;
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
                      hElem=GetDlgItem(hDlg, IDC_VIEW) ;
          SendMessage(hElem, WM_SETTEXT, NULL, 
                       (LPARAM)UD_ptr_incode((void *)&contexts[i].data)) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}
/*------------------------------------------------ Отработка событий */

    case WM_PAINT:      {

                      hElem=GetDlgItem(hDlg, IDC_VIEW) ;
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
                                         contexts[i].unit=NULL ;
                                                 break ;         }

                    if(unit!=NULL)  unit->hWnd=NULL ;

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



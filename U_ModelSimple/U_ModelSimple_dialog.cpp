/********************************************************************/
/*								    */
/*             МОДУЛЬ УПРАВЛЕНИЯ КОМПОНЕНТОМ "ПРОСТАЯ МОДЕЛЬ"       */
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

#include "U_ModelSimple.h"

#pragma warning(disable : 4996)

#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text) ;

#include "resource.h"


/*********************************************************************/
/*								     */
/* 	     Обработчик сообщений диалогового окна HELP	             */

  INT_PTR CALLBACK  Unit_ModelSimple_Help_dialog(  HWND hDlg,     UINT Msg, 
                                                 WPARAM wParam, LPARAM lParam) 
{
  RSS_Module_ModelSimple  Module ;
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
/* 	     Обработчик сообщений диалогового окна PARS	             */

  INT_PTR CALLBACK  Unit_ModelSimple_Pars_dialog(  HWND hDlg,     UINT Msg, 
                                                 WPARAM wParam, LPARAM lParam) 
{
   static  RSS_Unit_ModelSimple *unit ;
                            int  elm ;         /* Идентификатор элемента диалога */
                            int  status ;
                           char  text[512] ;
                         double  mass ;
                         double  slideway ;
                         double  s_azim ;
                         double  s_elev ;
                           char *end ;

/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {

                           unit=(RSS_Unit_ModelSimple *)lParam ;
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
                  sprintf(text, "%lf", unit->mass) ;
                     SETs(IDC_MASS, text) ;

                  sprintf(text, "%lf", unit->slideway) ;
                     SETs(IDC_SLIDEWAY, text) ;

                  sprintf(text, "%lf", unit->s_azim) ;
                     SETs(IDC_S_AZIM, text) ;

                  sprintf(text, "%lf", unit->s_elev) ;
                     SETs(IDC_S_ELEV, text) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}

/*------------------------------------------------ Отработка событий */

    case WM_COMMAND:    {

	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - - - - - - - - -  Раскрытие справки */
       if(elm==IDC_SET) {

                          GETsl(IDC_MASS, text, sizeof(text)-1) ;
                    mass=strtod(text, &end) ;

            if(*end!=0) {
                           SEND_ERROR("Некорректное значение массы") ;
                                return(FALSE) ;
                        }

                          GETsl(IDC_SLIDEWAY, text, sizeof(text)-1) ;
                slideway=strtod(text, &end) ;

            if(*end!=0) {
                           SEND_ERROR("Некорректное значение длины направляющей") ;
                                return(FALSE) ;
                        }

                          GETsl(IDC_S_AZIM, text, sizeof(text)-1) ;
                  s_azim=strtod(text, &end) ;

            if(*end!=0) {
                           SEND_ERROR("Некорректное значение стандартного отклонения по направлению") ;
                                return(FALSE) ;
                        }

                          GETsl(IDC_S_ELEV, text, sizeof(text)-1) ;
                  s_elev=strtod(text, &end) ;

            if(*end!=0) {
                           SEND_ERROR("Некорректное значение стандартного отклонения по углу вылета") ;
                                return(FALSE) ;
                        }

                          unit->mass    =mass ;
                          unit->slideway=slideway ;
                          unit->s_azim  =s_azim ;
                          unit->s_elev  =s_elev ;

                            EndDialog(hDlg, 1) ;

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

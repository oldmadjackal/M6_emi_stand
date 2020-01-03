/********************************************************************/
/*								    */
/*       МОДУЛЬ УПРАВЛЕНИЯ КОМПОНЕНТОМ "2-Х ЭТАПНОЕ НАВЕДЕНИЕ"      */
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

#include "U_Control2Stage.h"

#pragma warning(disable : 4996)

#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text) ;

#include "resource.h"


/*********************************************************************/
/*								     */
/* 	     Обработчик сообщений диалогового окна HELP	             */

  INT_PTR CALLBACK  Unit_Control2Stage_Help_dialog(  HWND hDlg,     UINT Msg, 
                                                   WPARAM wParam, LPARAM lParam) 
{
  RSS_Module_Control2Stage  Module ;
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
/* 	     Обработчик сообщений диалогового окна CONFIG            */

  INT_PTR CALLBACK  Unit_Control2Stage_Config_dialog(  HWND hDlg,     UINT Msg, 
                                                     WPARAM wParam, LPARAM lParam) 
{
 static                     HFONT  font ;         /* Шрифт */
         RSS_Module_Control2Stage  Module ;
 static    RSS_Unit_Control2Stage *unit ;
                              int  elm ;          /* Идентификатор элемента диалога */
                              int  status ;
                             char  text[1024] ;
                           double  value ;
                             char *end ;

                     OPENFILENAME  file_choice ;
                             char  path[FILENAME_MAX] ;
                             char *dat_filter="Control-файлы\0*.control\0"
                                              "Все файлы\0*.*\0"
                                              "\0\0" ;
     
/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {

              unit=(RSS_Unit_Control2Stage *)lParam ;
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
//         SendMessage(ITEM(IDC_LIST), WM_SETFONT, (WPARAM)font, 0) ;
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
             SETs(IDC_PATH, unit->program) ;

          sprintf(text, "%.2lf", unit->vector_control_max/9.8) ;
             SETs(IDC_NMAX_G, text) ;
          sprintf(text, "%.2lf", unit->vector_control_max) ;
             SETs(IDC_NMAX_N, text) ;
/*- - - - - - - - - - - - - - - - - - - - - - Инициализация доступов */
             SETx(IDC_ON_G, 1) ;
             SETx(IDC_ON_N, 0) ;

          DISABLE(IDC_NMAX_N) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}

/*------------------------------------------------ Отработка событий */

    case WM_COMMAND:    {

	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - - -  Режим ввода ускорения маневра */
     if(elm==IDC_ON_G) {
                            DISABLE(IDC_NMAX_N) ;
                             ENABLE(IDC_NMAX_G) ;

                                  break ;
                       }
     if(elm==IDC_ON_N) {
                             ENABLE(IDC_NMAX_N) ;
                            DISABLE(IDC_NMAX_G) ;

                                  break ;
                       }
/*- - - - - - - - - - - - - - - - - - - - - - Ввод ускорения маневра */
     if(   elm==IDC_NMAX_G  &&
        status==EN_KILLFOCUS  ) {

                     memset(text, 0, sizeof(text));
                      GETsl(IDC_NMAX_G, text, sizeof(text)-1) ;

               value=strtod(text, &end) ;
                    sprintf(text, "%.2lf", value*9.8) ;
                       SETs(IDC_NMAX_N, text) ;

                                       break ;
                                } 
     if(   elm==IDC_NMAX_N  &&
        status==EN_KILLFOCUS  ) {

                     memset(text, 0, sizeof(text));
                      GETsl(IDC_NMAX_N, text, sizeof(text)-1) ;

               value=strtod(text, &end) ;
                    sprintf(text, "%.2lf", value/9.8) ;
                       SETs(IDC_NMAX_G, text) ;

                                       break ;
                                } 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - -  Выбор файла */
     if(   elm==IDC_BROWSE &&
        status==BN_CLICKED   ) {

           memset( path, 0, sizeof(path)) ;

	   memset(&file_choice, 0, sizeof(file_choice))  ;
  		   file_choice.lStructSize =sizeof(file_choice) ;   /* Форм.описание диалога */
		   file_choice.hwndOwner   = NULL ;
		   file_choice.hInstance   =GetModuleHandle(NULL) ;
		   file_choice.lpstrFilter =dat_filter ;
		   file_choice.nFilterIndex=  1  ;
  		   file_choice.lpstrFile   = path ;
		   file_choice.nMaxFile    =sizeof(path) ;
		   file_choice.lpstrTitle  ="Укажите файл программы управления" ;
		   file_choice.Flags       =  0 ;

            status=GetOpenFileName(&file_choice) ;                  /* Запрос пути */
         if(status==0)  return(0) ;                                 /* Если путь не выбран... */

                       SETs(IDC_PATH, path) ;

                                     break ;
                               } 
/*- - - - - - - - - - - - - - - - - - - - - - Добавление компонентов */
     if(elm==IDC_SET) {

                                   GETsl(IDC_PATH, text, sizeof(text)-1) ;
               status=Module.ReadProgram(unit, path, text) ; 
            if(status) {
                          SEND_ERROR(text) ;
                              return(FALSE) ;
                       }

                                   GETsl(IDC_NMAX_N, text, sizeof(text)-1) ;
         unit->vector_control_max=strtod(text, &end) ;

                            EndDialog(hDlg, 0) ;

                              return(FALSE) ;
                      }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			  return(FALSE) ;
			     break ;
			}
/*--------------------------------------------------------- Закрытие */

    case WM_CLOSE:      {
                            EndDialog(hDlg, -1) ;
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


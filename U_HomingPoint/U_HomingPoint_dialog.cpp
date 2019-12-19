/********************************************************************/
/*								    */
/*          МОДУЛЬ УПРАВЛЕНИЯ КОМПОНЕНТОМ "КООРДИНАТНАЯ ГСН"        */
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

#include "U_HomingPoint.h"

#pragma warning(disable : 4996)

#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text) ;

#include "resource.h"


/*********************************************************************/
/*								     */
/* 	     Обработчик сообщений диалогового окна HELP	             */

  INT_PTR CALLBACK  Unit_HomingPoint_Help_dialog(  HWND hDlg,     UINT Msg, 
                                                 WPARAM wParam, LPARAM lParam) 
{
    RSS_Module_HomingPoint  Module ;
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

  INT_PTR CALLBACK  Unit_HomingPoint_Config_dialog(  HWND hDlg,     UINT Msg, 
                                                   WPARAM wParam, LPARAM lParam) 
{
 static                     HFONT  font ;         /* Шрифт */
 static    RSS_Module_HomingPoint *Module ;
 static      RSS_Unit_HomingPoint *unit ;
                              int  elm ;          /* Идентификатор элемента диалога */
                              int  status ;
                             char  text[1024] ;

#define       _INERTIAL_METHOD_TXT  "Инерциальная система наведения"
#define            _GPS_METHOD_TXT  "GPS"
     
/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {

              unit=(RSS_Unit_HomingPoint *)lParam ;
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
/*- - - - - - - - - - - - - - Инициализация списка методов наведения */
                  CB_CLEAR(IDC_METHOD) ;
               CB_ADD_LIST(IDC_METHOD, _INERTIAL_METHOD_TXT) ;
               CB_ADD_LIST(IDC_METHOD,      _GPS_METHOD_TXT) ;
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
       if(unit->method==_INERTIAL_METHOD)  SETc(IDC_METHOD, _INERTIAL_METHOD_TXT) ;
       if(unit->method==     _GPS_METHOD)  SETc(IDC_METHOD,      _GPS_METHOD_TXT) ;

                SETi(IDC_GPS_XYZ_PRECISION, unit->gps_xyz_precision) ;
/*- - - - - - - - - - - - - - - - - - - - - - Инициализация доступов */
       if(unit->method==_INERTIAL_METHOD) {
                                          }
       else                               {
                                          }

       if(unit->method==     _GPS_METHOD) {
                                              ENABLE(IDC_GPS_XYZ_PRECISION) ;
                                          }
       else                               {
                                             DISABLE(IDC_GPS_XYZ_PRECISION) ;
                                          }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}

/*------------------------------------------------ Отработка событий */

    case WM_COMMAND:    {

	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - Выбор метода */
     if(status==CBN_SELCHANGE &&
           elm==IDC_METHOD       ) {

           GETc(IDC_METHOD, text) ;
 
       if(!stricmp(text, _INERTIAL_METHOD_TXT)) {
                                                }
       else                                     {
                                                }

       if(!stricmp(text,      _GPS_METHOD_TXT)) {
                                                    ENABLE(IDC_GPS_XYZ_PRECISION) ;
                                                }
       else                                     {
                                                   DISABLE(IDC_GPS_XYZ_PRECISION) ;
                                                }

                                       return(FALSE) ;
                                   }
/*- - - - - - - - - - - - - - - - - - - - - - - - Задание параметров */
     if(elm==IDC_SET) {

               GETc(IDC_METHOD, text) ;
 
        if(!stricmp(text, _INERTIAL_METHOD_TXT))  unit->method=_INERTIAL_METHOD ;
        if(!stricmp(text,      _GPS_METHOD_TXT))  unit->method=     _GPS_METHOD ;

               unit->gps_xyz_precision=GETi(IDC_GPS_XYZ_PRECISION) ;

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


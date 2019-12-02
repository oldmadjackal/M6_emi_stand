/********************************************************************/
/*								    */
/*          МОДУЛЬ УПРАВЛЕНИЯ КОМПОНЕНТОМ "ПРОСТАЯ ГСН"             */
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

#include "U_HomingSimple.h"

#pragma warning(disable : 4996)

#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text) ;

#include "resource.h"


/*********************************************************************/
/*								     */
/* 	     Обработчик сообщений диалогового окна HELP	             */

  INT_PTR CALLBACK  Unit_HomingSimple_Help_dialog(  HWND hDlg,     UINT Msg, 
                                                  WPARAM wParam, LPARAM lParam) 
{
   RSS_Module_HomingSimple  Module ;
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

  INT_PTR CALLBACK  Unit_HomingSimple_Config_dialog(  HWND hDlg,     UINT Msg, 
                                                    WPARAM wParam, LPARAM lParam) 
{
 static                     HFONT  font ;         /* Шрифт */
 static   RSS_Module_HomingSimple *Module ;
 static     RSS_Unit_HomingSimple *unit ;
                              int  elm ;          /* Идентификатор элемента диалога */
                              int  status ;
                             char *unit_name ;
                             char  unit_type[1024] ;
                             char  text[1024] ;
                             char *end ;
                              int  i ;
     
/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {

              unit=(RSS_Unit_HomingSimple *)lParam ;
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
/*- - - - - - - - - - - - - - Инициализация списка типов компонентов */
#define   MODULES       RSS_Kernel::kernel->modules 
#define   MODULES_CNT   RSS_Kernel::kernel->modules_cnt 

                                          CB_CLEAR(IDC_TYPE_1_1) ;
                                          CB_CLEAR(IDC_TYPE_1_2) ;
                                          CB_CLEAR(IDC_TYPE_1_3) ;
                                          CB_CLEAR(IDC_TYPE_2_1) ;
                                          CB_CLEAR(IDC_TYPE_2_2) ;
                                          CB_CLEAR(IDC_TYPE_2_3) ;
                                          CB_CLEAR(IDC_SWITCH_1_2) ;

   for(i=0 ; i<MODULES_CNT ; i++) 
     if(MODULES[i].entry->category      !=NULL &&
        MODULES[i].entry->identification!=NULL   )
      if(!stricmp("Unit", MODULES[i].entry->category)) {

        if(MODULES[i].entry->lego_type==NULL)  continue ;

             MODULES[i].entry->vGetParameter("$$MODULE_NAME", text) ;

        if(strstr(MODULES[i].entry->lego_type, "Homing" )!=NULL) {
                                                                    CB_ADD_LIST(IDC_TYPE_1_1, text) ;
                                                                    CB_ADD_LIST(IDC_TYPE_1_2, text) ;
                                                                    CB_ADD_LIST(IDC_TYPE_1_3, text) ;
                                                                    CB_ADD_LIST(IDC_TYPE_2_1, text) ;
                                                                    CB_ADD_LIST(IDC_TYPE_2_2, text) ;
                                                                    CB_ADD_LIST(IDC_TYPE_2_3, text) ;
                                                                 }
                                                       }

#undef    MODULES
#undef    MODULES_CNT
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
/*
         if(object->unit_warhead!=NULL) {
             object->unit_warhead->Module->vGetParameter("$$MODULE_NAME", text) ;
                                                    SETc(IDC_TYPE_W, text) ;
                                        }
         if(object->unit_engine !=NULL) {
             object->unit_engine->Module->vGetParameter("$$MODULE_NAME", text) ;
                                                   SETc(IDC_TYPE_E, text) ;
                                        }
         if(object->unit_homing !=NULL) {
             object->unit_homing->Module->vGetParameter("$$MODULE_NAME", text) ;
                                                   SETc(IDC_TYPE_H, text) ;
                                        }
         if(object->unit_control!=NULL) {
             object->unit_control->Module->vGetParameter("$$MODULE_NAME", text) ;
                                                    SETc(IDC_TYPE_C, text) ;
                                        }
         if(object->unit_model  !=NULL) {
             object->unit_model->Module->vGetParameter("$$MODULE_NAME", text) ;
                                                  SETc(IDC_TYPE_M, text) ;
                                        }
*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}

/*------------------------------------------------ Отработка событий */

    case WM_COMMAND:    {

	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - - - - - - - Добавление компонентов */
     if(elm==IDC_SET) {
/*
        for(i=0 ; i<5 ; i++) {

          if(i==0)  {  GETc(IDC_TYPE_W, unit_type) ;
                                        unit_name="warhead" ;  }
          if(i==1)  {  GETc(IDC_TYPE_E, unit_type) ;
                                        unit_name="engine" ;  }
          if(i==2)  {  GETc(IDC_TYPE_H, unit_type) ;
                                        unit_name="homing" ;  }
          if(i==3)  {  GETc(IDC_TYPE_C, unit_type) ;
                                        unit_name="control" ;  }
          if(i==4)  {  GETc(IDC_TYPE_M, unit_type) ;
                                        unit_name="model" ;  }

          if(unit_type[0]==0)  continue ;

             end=strchr(unit_type, ' ') ;
          if(end!=NULL)  *end=0 ;

             unit=Module->AddUnit(object, unit_name, unit_type, text) ;
          if(unit==NULL) {
                            SEND_ERROR(text) ;
                              return(FALSE) ;
                         }
                             }
*/
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


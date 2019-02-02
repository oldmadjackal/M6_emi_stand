/********************************************************************/
/*								    */
/*		МОДУЛЬ УПРАВЛЕНИЯ ОБЪЕКТОМ "РАКЕТА-ЛЕГО" 	    */
/*								    */
/*                   Диалоговые процедуры                           */
/*                                                                  */
/********************************************************************/

#include <windows.h>
#include <stdio.h>
#include <io.h>

#include "..\Emi_root\controls.h"

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Unit\RSS_Unit.h"
#include "..\RSS_Kernel\RSS_Kernel.h"
#include "..\RSS_Model\RSS_Model.h"
#include "..\Ud_tools\UserDlg.h"

#include "O_Missile_Lego.h"

#pragma warning(disable : 4996)

#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text) ;

#include "resource.h"


/*********************************************************************/
/*								     */
/* 	     Обработчик сообщений диалогового окна HELP	             */

  INT_PTR CALLBACK  Object_MissileLego_Help_dialog(  HWND hDlg,     UINT Msg, 
                                                    WPARAM wParam, LPARAM lParam) 
{
 RSS_Module_MissileLego  Module ;
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
/* 	     Обработчик сообщений диалогового окна CREATE            */

  INT_PTR CALLBACK  Object_MissileLego_Create_dialog(  HWND hDlg,     UINT Msg, 
                                                     WPARAM wParam, LPARAM lParam) 
{
       RSS_Module_MissileLego  Module ;
 static        RSS_Model_data *data ;
                   RSS_Object *object ;
 static                  char  models_list[4096] ;
 static                  RECT  PictureFrame ;
                      HBITMAP  hBitmap ;
//                    HBITMAP  hBitmap_prv ;
                          int  elm ;               /* Идентификатор элемента диалога */
                          int  status ;
                         char  library[FILENAME_MAX] ;
                         char  value[512] ;
                          int  assigned ;
                         char *end ;
                          int  i ;
     
/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {

                   GetWindowRect(ITEM(IDC_PICTURE), &PictureFrame) ;

              data=(RSS_Model_data *)lParam ;
/*- - - - - - - - - - - - - - - - - - - Инициализация списка моделей */
           RSS_Model_list(data->lib_path, models_list,
                                   sizeof(models_list)-1, "BODY") ;

                                          CB_CLEAR(IDC_MODEL) ;
     for(end=models_list ; *end ; ) {
                                      CB_ADD_LIST(IDC_MODEL, end) ;
                                         end+=strlen(end)+1 ;
                                         end+=strlen(end)+1 ;
                                    }
/*- - - - - - - - - - - - - - - - - - - - Считывание описание модели */
     while(data->model[0]!=0) {

        for(end=models_list ; *end ; ) {                            /* Ищем модель по списку */
                        if(!stricmp(data->model, end))  break ;
                                         end+=strlen(end)+1 ;
                                         end+=strlen(end)+1 ;
                                       }

           if(*end==0) {  data->model[0]=0 ;                        /* Если такой модели нет... */
                             break ;          }

                      sprintf(data->path, "%s\\%s", data->lib_path, end) ;
           RSS_Model_ReadPars(data) ;                               /* Считываем параметры модели */

                                  break ;
                              }
/*- - - - - - - - - - - - - - - - - - - - -   Инициализация картинки */
     while(data->picture[0]!=0) {
    
          status=RSS_Model_Picture(data, &hBitmap, &PictureFrame) ;
       if(status)  break ;

               SendMessage(ITEM(IDC_PICTURE), 
                              STM_SETIMAGE,
                               (WPARAM)IMAGE_BITMAP,
                                (LPARAM)hBitmap     ) ;

                                      break ;
                                }
/*- - - - - - - - - - - - - - - - -  Инициализация списка параметров */
       for(i=0 ; i<5 ; i++) {
                                       SETs(IDC_PAR_NAME_1+i, data->pars[i].text) ;
         if(data->pars[i].text[0]!=0)  SHOW(IDC_PAR_VALUE_1+i) ;
         else                          HIDE(IDC_PAR_VALUE_1+i) ;
                            }
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
          DISABLE(IDC_CREATE) ;

            SETs(IDC_NAME,         data->name) ;
            SETs(IDC_LIBRARY_PATH, data->lib_path) ;
            SETc(IDC_MODEL,        data->model) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}

/*------------------------------------------------ Отработка событий */

    case WM_COMMAND:    {

	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - Смена библиотеки */
     if(elm==IDC_LIBRARY_PATH) {

        if(status!=EN_UPDATE)  return(FALSE) ;

              GETs(IDC_LIBRARY_PATH, library) ;

           RSS_Model_list(library, models_list,
                            sizeof(models_list)-1, "BODY") ;

                                          CB_CLEAR(IDC_MODEL) ;
      for(end=models_list ; *end ; ) {
                                       CB_ADD_LIST(IDC_MODEL, end) ;
                                          end+=strlen(end)+1 ;
                                          end+=strlen(end)+1 ;
                                     }

                                 DISABLE(IDC_CREATE) ;

                                      return(FALSE) ;
                               }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - Выбор модели */
     if(elm==IDC_MODEL) {
                              if(status!=CBN_SELCHANGE)  break ;

                         GETs(IDC_LIBRARY_PATH, data->lib_path) ;
                         GETc(elm, data->model ) ;                  /* Извлекаем название модели */

        for(end=models_list ; *end ; ) {                            /* Ищем модель по списку */
                        if(!stricmp(data->model, end))  break ;
                                         end+=strlen(end)+1 ;
                                         end+=strlen(end)+1 ;
                                       }

                                    end+=strlen(end)+1 ;            /* Извлекаем имя файла */

                      sprintf(data->path, "%s\\%s", data->lib_path, end) ;
           RSS_Model_ReadPars(data) ;                               /* Считываем параметры модели */

        for(i=0 ; i<5 ; i++) {                                      /* Выдаем список параметров модели */
                                       SETs(IDC_PAR_NAME_1+i, data->pars[i].text) ;
         if(data->pars[i].text[0]!=0)  SHOW(IDC_PAR_VALUE_1+i) ;
         else                          HIDE(IDC_PAR_VALUE_1+i) ;
                             }

            status=RSS_Model_Picture(data, &hBitmap, &PictureFrame) ;
//       if(status)  break ;

               SendMessage(ITEM(IDC_PICTURE), STM_SETIMAGE,
                               (WPARAM)IMAGE_BITMAP, (LPARAM)hBitmap) ;

                             DISABLE(IDC_CREATE) ;
                        }
/*- - - - - - - - - - - - - - - - - - - - -  Задание параметров тела */
     if(elm==IDC_PAR_VALUE_1 ||
        elm==IDC_PAR_VALUE_2 ||
        elm==IDC_PAR_VALUE_3 ||
        elm==IDC_PAR_VALUE_4 ||
        elm==IDC_PAR_VALUE_5   ) {

                    if(status!=EN_UPDATE)  return(FALSE) ;

           for(assigned=1, i=0 ; i<5 ; i++) {
               if(!VISIBLE(IDC_PAR_VALUE_1+i))  break ;

                  GETsl(IDC_PAR_VALUE_1+i, value, sizeof(value)-1) ;
               if(*value==0) {  assigned=0 ;  break ;  } 
                                            }

               if(assigned)  ENABLE(IDC_CREATE) ;

                                      return(FALSE) ;
                                }
/*- - - - - - - - - - - - - - - - - - - - - - - - - Создание объекта */
     if(elm==IDC_CREATE) {

                  GETs(IDC_NAME,         data->name) ;
                  GETs(IDC_LIBRARY_PATH, data->lib_path ) ;
                  GETc(IDC_MODEL,        data->model) ;

          for(i=0 ; i<5 ; i++)
                  GETs(IDC_PAR_VALUE_1+i, data->pars[i].value) ;  

                object=Module.vCreateObject(data) ;
             if(object!=NULL)  EndDialog(hDlg, 0) ;

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


/*********************************************************************/
/*								     */
/* 	     Обработчик сообщений диалогового окна LEGO              */

  INT_PTR CALLBACK  Object_MissileLego_Lego_dialog(  HWND hDlg,     UINT Msg, 
                                                   WPARAM wParam, LPARAM lParam) 
{
 static                  HFONT  font ;         /* Шрифт */
 static RSS_Module_MissileLego *Module ;
 static RSS_Object_MissileLego *object ;
                      RSS_Unit *unit ;
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

              object=(RSS_Object_MissileLego *)lParam ;
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

                                          CB_CLEAR(IDC_TYPE_W) ;
                                          CB_CLEAR(IDC_TYPE_E) ;
                                          CB_CLEAR(IDC_TYPE_M) ;

   for(i=0 ; i<MODULES_CNT ; i++) 
     if(MODULES[i].entry->category      !=NULL &&
        MODULES[i].entry->identification!=NULL   )
      if(!stricmp("Unit", MODULES[i].entry->category)) {

        if(MODULES[i].entry->lego_type==NULL)  continue ;

             MODULES[i].entry->vGetParameter("$$MODULE_NAME", text) ;

        if(strstr(MODULES[i].entry->lego_type, "WarHead")!=NULL)  CB_ADD_LIST(IDC_TYPE_W, text) ;
        if(strstr(MODULES[i].entry->lego_type, "Engine" )!=NULL)  CB_ADD_LIST(IDC_TYPE_E, text) ;
        if(strstr(MODULES[i].entry->lego_type, "Homing" )!=NULL)  CB_ADD_LIST(IDC_TYPE_H, text) ;
        if(strstr(MODULES[i].entry->lego_type, "COntrol")!=NULL)  CB_ADD_LIST(IDC_TYPE_C, text) ;
        if(strstr(MODULES[i].entry->lego_type, "Model"  )!=NULL)  CB_ADD_LIST(IDC_TYPE_M, text) ;                  
                                                       }

#undef    MODULES
#undef    MODULES_CNT
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
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


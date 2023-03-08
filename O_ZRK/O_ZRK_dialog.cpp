/********************************************************************/
/*                                                                  */
/*     МОДУЛЬ УПРАВЛЕНИЯ ОБЪЕКТОМ "ЗЕНИТНЫЙ РАКЕТНЫЙ КОМПЛЕКС"      */
/*                                                                  */
/*                   Диалоговые процедуры                           */
/*                                                                  */
/********************************************************************/

#include <windows.h>
#include <stdio.h>
#include <io.h>

#include "..\Emi_root\controls.h"

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Kernel\RSS_Kernel.h"
#include "..\RSS_Model\RSS_Model.h"

#include "O_ZRK.h"

#pragma warning(disable : 4996)

#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text) ;

#include "resource.h"


/*********************************************************************/
/*                                                                   */
/*            Обработчик сообщений диалогового окна HELP             */

  INT_PTR CALLBACK  Object_ZRK_Help_dialog(  HWND hDlg,     UINT Msg, 
                                           WPARAM wParam, LPARAM lParam) 
{
      RSS_Module_ZRK  Module ;
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
/*                                                                   */
/* 	         Обработчик сообщений диалогового окна CREATE            */

  INT_PTR CALLBACK  Object_ZRK_Create_dialog(  HWND hDlg,     UINT Msg, 
                                             WPARAM wParam, LPARAM lParam) 
{
            RSS_Module_ZRK  Module ;
 static     RSS_Model_data *data ;
                RSS_Object *object ;
 static               char  models_list[4096] ;
 static               RECT  PictureFrame ;
                   HBITMAP  hBitmap ;
//                 HBITMAP  hBitmap_prv ;
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
/*                                                                   */
/*           Обработчик сообщений диалогового окна FIRING            */

  INT_PTR CALLBACK  Object_ZRK_Firing_dialog(  HWND hDlg,     UINT Msg, 
                                             WPARAM wParam, LPARAM lParam) 
{
    static          HFONT  font ;                         /* Шрифт */
    static RSS_Module_ZRK *Module ;
    static RSS_Object_ZRK *object ;
               RSS_Object *missile ;
                   double  r_firing_min ;                 /* Минимальная дальность пуска */
                   double  r_firing_max ;                 /* Максимальная дальность пуска */
                   double  h_firing_max ;                 /* Максимальная высота цели */
                      int  ballistic_targets ;            /* Баллистические цели */ 
                     char  missile_type[FILENAME_MAX] ;   /* Имя объекта - шаблона ракеты */
                      int  missiles_max ;                 /* Максимальное число ракет, готовых к пуску */
                      int  missiles_firing ;              /* Число ракет на одну цель */
                   double  firing_pause ;                 /* Интервал между пусками, секунд */
                      int  v_launch ;                     /* Вертикальный пуск */ 
                      int  elm ;                          /* Идентификатор элемента диалога */
                      int  status ;
                     char  text[1024] ;
                     char *error ;
                     char *end ;
     
/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {

              object=(RSS_Object_ZRK *)lParam ;
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
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
             SETs(IDC_MISSILE_TYPE, object->missile_type) ;

          sprintf(text, "%.2f", object->r_firing_min) ;
             SETs(IDC_R_MIN,  text) ;
          sprintf(text, "%.2f", object->r_firing_max) ;
             SETs(IDC_R_MAX,  text) ;
          sprintf(text, "%.2f", object->h_firing_max) ;
             SETs(IDC_H_MAX,  text) ;
             SETx(IDC_BALLISTIC_TARGETS, object->ballistic_target) ;
          sprintf(text, "%d", object->missiles_max) ;
             SETs(IDC_MISSILES_MAX,  text) ;
          sprintf(text, "%d", object->missiles_firing) ;
             SETs(IDC_PER_TARGET,  text) ;
          sprintf(text, "%.2f", object->firing_pause) ;
             SETs(IDC_INTERVAL,  text) ;
             SETx(IDC_V_LAUNCH, object->v_launch) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}

/*------------------------------------------------ Отработка событий */

    case WM_COMMAND:    {

	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - - - - - - - -  Сохранение настроек */
     if(elm==IDC_SET) {

                              GETs(IDC_R_MIN,  text) ;
           r_firing_min    =strtod(text, &end) ;
                              GETs(IDC_R_MAX,  text) ;
           r_firing_max    =strtod(text, &end) ;
                              GETs(IDC_H_MAX,  text) ;
           h_firing_max    =strtod(text, &end) ;
          ballistic_targets=  GETx(IDC_BALLISTIC_TARGETS) ;
                              GETs(IDC_MISSILE_TYPE, missile_type) ;
           missile         =Module->FindObject(missile_type, 0) ;    /* Ищем ракету по имени */
                              GETs(IDC_MISSILES_MAX,  text) ;
           missiles_max    =strtoul(text, &end, 10) ;
                              GETs(IDC_PER_TARGET,  text) ;
           missiles_firing =strtoul(text, &end, 10) ;
                              GETs(IDC_INTERVAL,  text) ;
           firing_pause    =strtod(text, &end) ;
           v_launch        =  GETx(IDC_V_LAUNCH) ;

                                           error=  NULL ;
        if(r_firing_min   <   0.        )  error="Минимальная дальность пуска не может быть меньше 0" ;
        if(r_firing_max   <=  0.        )  error="Максимальная дальность пуска должена быть больше 0" ;
        if(h_firing_max   <=  0.        )  error="Максимальная высота пуска должена быть больше 0" ;
//      if(h_firing_max   > r_firing_max)  error="Максимальная высота пуска не может быть больше максимальной дальности пуска" ;
        if(missile        ==  NULL      )  error="Неизвестная ЗУР" ;
        if(missiles_max   <   0         )  error="Боекомплект не может быть меньше 0" ;
        if(missiles_firing<   1         )  error="Наряд ЗУР на одну цель не может быть меньше 1" ;
        if(firing_pause   <=  0.        )  error="Интервал пуска должен быть больше 0" ;

        if(error!=NULL) {
                           SEND_ERROR(error) ;
                              return(FALSE) ;
                        }

                  object->r_firing_min    =r_firing_min ;
                  object->r_firing_max    =r_firing_max ;
                  object->h_firing_max    =h_firing_max ;
                  object->ballistic_target=ballistic_targets ;
          strncpy(object->missile_type,    missile_type, sizeof(object->missile_type)-1) ;
                  object->missile_tmpl    =missile ;
                  object->missiles_max    =missiles_max ;
                  object->missiles_firing =missiles_firing ;
                  object->firing_pause    =firing_pause ;
                  object->v_launch        =v_launch ;

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


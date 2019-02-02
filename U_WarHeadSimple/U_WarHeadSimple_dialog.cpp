/********************************************************************/
/*								    */
/*             МОДУЛЬ УПРАВЛЕНИЯ КОМПОНЕНТОМ "ПРОСТАЯ БЧ"           */
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

#include "U_WarHeadSimple.h"

#pragma warning(disable : 4996)

#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text) ;

#include "resource.h"


/*********************************************************************/
/*								     */
/* 	     Обработчик сообщений диалогового окна HELP	             */

  INT_PTR CALLBACK  Unit_WarHeadSimple_Help_dialog(  HWND hDlg,     UINT Msg, 
                                                   WPARAM wParam, LPARAM lParam) 
{
  RSS_Module_WarHeadSimple  Module ;
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

  INT_PTR CALLBACK  Unit_WarHeadSimple_Pars_dialog(  HWND hDlg,     UINT Msg, 
                                                   WPARAM wParam, LPARAM lParam) 
{
   static  RSS_Unit_WarHeadSimple *unit ;
                              int  elm ;         /* Идентификатор элемента диалога */
                              int  status ;
                              int  tripping_type ;
                           double  tripping_altitude ;
                           double  tripping_time ;
                              int  load_type ;
                           double  hit_range ;
                           double  blast_radius ;
                             char  sub_unit[128] ;
                       RSS_Object  *sub_object ;
                              int  sub_count ;
                           double  sub_step ;
                              int  i  ;

/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {

                      unit=(RSS_Unit_WarHeadSimple *)lParam ;
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
#pragma warning(disable : 4244)
                 SETi(IDC_ALTITUDE,     unit->tripping_altitude) ;
                 SETi(IDC_TIME,         unit->tripping_time    ) ;
                 SETi(IDC_HIT_RANGE,    unit->hit_range        ) ;
                 SETi(IDC_BLAST_RADIUS, unit->blast_radius     ) ;
                 SETs(IDC_SUB_UNIT,     unit->sub_unit         ) ;
                 SETi(IDC_SUB_COUNT,    unit->sub_count        ) ;
                 SETi(IDC_SUB_STEP,     unit->sub_step         ) ;
#pragma warning(default : 4244)

                                           DISABLE(IDC_ALTITUDE) ;
                                           DISABLE(IDC_TIME) ;

       if(unit->tripping_type==_BY_TIME) {
                                              SETx(IDC_BY_TIME, 1) ;
                                            ENABLE(IDC_TIME) ;
                                         }
       else                              {
                                              SETx(IDC_BY_ALTITUDE, 1) ;
                                            ENABLE(IDC_ALTITUDE) ;
                                         }

                                           DISABLE(IDC_HIT_RANGE) ;
                                           DISABLE(IDC_BLAST_RADIUS) ;
                                           DISABLE(IDC_SUB_UNIT) ;
                                           DISABLE(IDC_SUB_COUNT) ;
                                           DISABLE(IDC_SUB_STEP) ;

       if(unit->load_type==_GRENADE_TYPE) {
                                              SETx(IDC_OF_TYPE, 1) ;
                                            ENABLE(IDC_HIT_RANGE) ;
                                            ENABLE(IDC_BLAST_RADIUS) ;
                                          }
       else                               {
                                              SETx(IDC_STRIPE_TYPE, 1) ;
                                            ENABLE(IDC_SUB_UNIT) ;
                                            ENABLE(IDC_SUB_COUNT) ;
                                            ENABLE(IDC_SUB_STEP) ;
                                          }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}

/*------------------------------------------------ Отработка событий */

    case WM_COMMAND:    {

	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - - - - -  Выбор режима срабатывания */
       if(elm==IDC_BY_TIME    ) {
                                  DISABLE(IDC_ALTITUDE) ;
                                   ENABLE(IDC_TIME) ;

                                     return(FALSE) ;
                                }
       if(elm==IDC_BY_ALTITUDE) {
                                   ENABLE(IDC_ALTITUDE) ;
                                  DISABLE(IDC_TIME) ;

                                     return(FALSE) ;
                                }
/*- - - - - - - - - - - - - - - - - - - - - -  Выбор вида снаряжения */
       if(elm==IDC_OF_TYPE    ) {
                                   ENABLE(IDC_HIT_RANGE) ;
                                   ENABLE(IDC_BLAST_RADIUS) ;
                                  DISABLE(IDC_SUB_UNIT) ;
                                  DISABLE(IDC_SUB_COUNT) ;
                                  DISABLE(IDC_SUB_STEP) ;

                                     return(FALSE) ;
                                }
       if(elm==IDC_STRIPE_TYPE) {
                                  DISABLE(IDC_HIT_RANGE) ;
                                  DISABLE(IDC_BLAST_RADIUS) ;
                                   ENABLE(IDC_SUB_UNIT) ;
                                   ENABLE(IDC_SUB_COUNT) ;
                                   ENABLE(IDC_SUB_STEP) ;

                                     return(FALSE) ;
                                }
/*- - - - - - - - - - - - - - - - - - - - - - - -  Раскрытие справки */
       if(elm==IDC_SET) {

         if(GETx(IDC_BY_TIME))  tripping_type=_BY_TIME ;
         else                   tripping_type=_BY_ALTITUDE ;

              tripping_altitude=GETi(IDC_ALTITUDE) ;
              tripping_time    =GETi(IDC_TIME) ;

         if(tripping_type==_BY_TIME &&
            tripping_time<=  0.       ) {
                           SEND_ERROR("Некорректное значение времени срабатывания") ;
                                return(FALSE) ;
                                        }

         if(GETx(IDC_OF_TYPE    ))  load_type=_GRENADE_TYPE ;
         if(GETx(IDC_STRIPE_TYPE))  load_type= _STRIPE_TYPE ;
        
                hit_range =GETi (IDC_HIT_RANGE) ;
              blast_radius=GETi (IDC_BLAST_RADIUS) ;

                           GETsl(IDC_SUB_UNIT, sub_unit, sizeof(sub_unit)-1) ;
                sub_count =GETi (IDC_SUB_COUNT) ;
                sub_step  =GETi (IDC_SUB_STEP) ;

#define   OBJECTS       unit->Module->kernel->kernel_objects 
#define   OBJECTS_CNT   unit->Module->kernel->kernel_objects_cnt 

          for(i=0 ; i<OBJECTS_CNT ; i++)
            if(!stricmp(OBJECTS[i]->Name, sub_unit)) {  sub_object=OBJECTS[i] ;
                                                               break ;           }

            if(i>=OBJECTS_CNT) {
                           SEND_ERROR("Заданный образец суб-боеприпаса не найден") ;
                                  return(FALSE) ;
                               }

#undef   OBJECTS
#undef   OBJECTS_CNT

                          unit->tripping_type    =tripping_type ;
                          unit->tripping_altitude=tripping_altitude ;
                          unit->tripping_time    =tripping_time ;
                          unit->    load_type    =    load_type ;
                          unit->     hit_range   =     hit_range ;
                          unit->   blast_radius  =   blast_radius ;
                   strcpy(unit->     sub_unit,         sub_unit) ;
                          unit->     sub_object  =     sub_object ;
                          unit->     sub_count   =     sub_count ;
                          unit->     sub_step    =     sub_step ;

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

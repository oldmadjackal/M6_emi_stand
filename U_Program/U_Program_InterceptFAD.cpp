/********************************************************************/
/*								    */
/*	МОДУЛЬ УПРАВЛЕНИЯ КОМПОНЕНТОМ "ПРОГРАММНОЕ УПРАВЛЕНИЕ"      */
/*                                                                  */
/*            Встроенная программа Inetrcept From Air Duty          */
/*                                                                  */
/*  Этапы программы:                                                */
/*   0 - Выход в район патрулирования                               */
/*   1 - Патрулирование по точкам маршрута в оба направления.       */
/*       При обнаружении групповой цели ->2                         */
/*   2 - Сближение с целью (без обновления её координат)            */
/*       По истечении времени сближения - анализ цели и определение */
/*       её параметров. Если цель потеряна - возвращаемся к этапу 2 */
/*   3 - Выход на цель с одновременным снижением до минимально      */
/*       допустимой высоты                                          */
/*   4 - Сброс боевой нагрузки                                      */
/*   5 - Уход от цели со снижением                                  */
/*                                                                  */
/*  Параметры программы:                                            */
/*    PROGRAM        - INTERCEPT_FAD                                */
/*    G              - ограничение перегрузки горизонтального       */
/*                      <20 - единицы g, >20 - радиус поворота      */
/*    E_MAX          - максимальный угол тангажа (+/-),             */
/*    E_RATE         - максимальная скорость изменения тангажа грд/с*/ 
/*    H_MIN          - минимальная высота начала выравнивания       */
/*    ROUTE          - точка маршрута: X, Z (может быть несколько)  */
/*    RADAR          - имя компонента поисковой РЛС                 */
/*    AIMING_MODE    - режим прицеливания и выхода в точку пуска:   */
/*                      DIRECT - непосредственно по цели            */
/*    FIRE_DISTANCE  - дистанция пуска (для режимов DIRECT)         */
/*    FIRE_SECTOR    - сектор пуска (допустимое угловое отклонение  */
/*                      между осью ракеты и направлением на цель)   */
/*                      (для режимов DIRECT)                        */
/*    DROPS_NUMBER   - число сбросов в одном заходе                 */
/*    DROPS_INTERVAL - интервал между сбросами                      */
/*    DROP_i         - i-ый сбрасываемый боеприпас                  */
/*								    */
/********************************************************************/


#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <direct.h>
#include <malloc.h>
#include <io.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <sys\stat.h>

#include "gl\gl.h"
#include "gl\glu.h"

#include "..\Matrix\Matrix.h"

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Unit\RSS_Unit.h"
#include "..\RSS_Kernel\RSS_Kernel.h"
#include "..\RSS_Model\RSS_Model.h"
#include "..\F_Show\F_Show.h"

#include "..\DCL_kernel\dcl.h"

#include "U_Program.h"

#pragma warning(disable : 4996)

#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text)

/*-------------------------------------- Описание настроек программы */

#define    _ROUTE_MAX  100
#define    _DROPS_MAX   10
#define  _TARGETS_MAX   10

typedef  struct {
                   char  program_name[128] ;

       struct {
                 double  x ;
                 double  z ;
              }          route[_ROUTE_MAX] ;
                    int  route_cnt ;
                    int  route_idx ;
                    int  route_dir ;

                 double  g ;
                 double  e_max ;
                 double  e_rate ;
                 double  h_min ;

                   char  radar[1024] ;
                   char  aiming_mode[32] ;
                 double  fire_distance ;
                 double  fire_sector ;
                    int  drops_number ;
                 double  drops_interval ;

       struct {
                   char  weapon[64] ;
              }          drops[_DROPS_MAX] ;
                    int  drops_cnt ;

       struct {
                   char  name[64] ;
              }          targets[_TARGETS_MAX] ;
                    int  targets_cnt ;

                    int  stage ;
                 double  x_0 ;
                 double  z_0 ;
//                   char  target[64] ;
                 double  vx_target ;
                 double  vz_target ;
                 double  x_direct ;
                 double  z_direct ;
                 double  a_direct ;
                 double  e_direct ;
                 double  check_time ;
                   char  fired[512] ;
                    int  dropped ;
                    int  stage_1_event_done ;
                    int  stage_4_event_done ;

                } RSS_Unit_Program_Embeded_InterceptFAD ;


/********************************************************************/
/*								    */
/*	     Считывание настроек встроенной программы               */

  void *RSS_Module_Program::ReadInterceptFAD(char *path, char *error)

{
                                          FILE *file ;
                                          void *data ;
         RSS_Unit_Program_Embeded_InterceptFAD *data_InterceptFAD ;
                                          char  text[256] ;
                                          char *key ;
                                          char *value ;
                                          char *end ;

/*---------------------------------------------------- Инициализация */ 

           data=NULL ;
         *error= 0 ;

/*--------------------------------------------------- Открытие файла */ 

       file=fopen(path, "rt") ;
    if(file==NULL) {
                      sprintf(error, "Ошибка %d открытия файла настроек программы : %s", errno, path) ;
                         return(NULL) ;
                   }
/*-------------------------------------------- Считывание параметров */ 

   do {
/*- - - - - - - - - - - - - - - - - - -  Считывание очередной строки */ 
               memset(text, 0, sizeof(text)) ;
            end=fgets(text, sizeof(text)-1, file) ;
         if(end==NULL)  break ;

            end=strchr(text, '\r') ;
         if(end!=NULL)  *end=0 ; 
            end=strchr(text, '\n') ;
         if(end!=NULL)  *end=0 ; 

         if(text[0]==';')  continue ;

              key=       text ;
            value=strchr(text, '=') ;
         if(value==NULL) {
                sprintf(error, "Неверный формат файла настроек программы : %s", text) ;
                            break ;
                         }
           *value=0 ;
            value++ ;
/*- - - - - - - - - - - - - - - - - - - - - Обработка вида программы */ 
         if(data==NULL) {

              if(stricmp(key, "PROGRAM")) {
                      sprintf(error, "Первая запись в файле настроек программы должна быть PROGRAM=<Вид программы>") ;
                                              break ;
                                          }

              if(stricmp(value, "INTERCEPT_FAD"))  break ;

                 data_InterceptFAD=(RSS_Unit_Program_Embeded_InterceptFAD *)
                                         calloc(1, sizeof(*data_InterceptFAD)) ;
                 data             =data_InterceptFAD ;

                     strcpy(data_InterceptFAD->program_name, value) ;

                                continue ;
                        }
/*- - - - - - - - - - - - - - - - - - - - - - -  Параметры программы */ 
#pragma warning(disable : 4701)

#define  D  data_InterceptFAD

          if(!stricmp(key, "G"             ))   D->g=strtod(value, &end) ;
          else
          if(!stricmp(key, "E_MAX"         ))   D->e_max=strtod(value, &end) ;
          else
          if(!stricmp(key, "E_RATE"        ))   D->e_rate=strtod(value, &end) ;
          else
          if(!stricmp(key, "H_MIN"         ))   D->h_min=strtod(value, &end) ;
          else
          if(!stricmp(key, "ROUTE"         )) {

                  if(D->route_cnt>=_ROUTE_MAX) {
                              sprintf(error, "Переполнение списка маршрутных точек") ;
                                                     break ;
                                               }               

                                              D->route[D->route_cnt].x=strtod(value, &end) ;
                                              D->route[D->route_cnt].z=strtod(end+1, &end) ;
                                                       D->route_cnt++ ;
                                              }
          else
          if(!stricmp(key, "RADAR"         ))   strncpy(D->radar, value, sizeof(D->radar)-1) ;
          else
          if(!stricmp(key, "AIMING_MODE"   ))   strncpy(D->aiming_mode, value, sizeof(D->aiming_mode)-1) ;
          else
          if(!stricmp(key, "FIRE_DISTANCE" ))   D->fire_distance=strtod(value, &end) ;
          else
          if(!stricmp(key, "FIRE_SECTOR"   ))   D->fire_sector=strtod(value, &end) ;
          else
          if(!stricmp(key, "DROPS_NUMBER"  ))   D->drops_number=strtoul(value, &end, 10) ;
          else
          if(!stricmp(key, "DROPS_INTERVAL"))   D->drops_interval=strtod(value, &end) ;
          else
          if(!stricmp(key, "DROP"         )) {

                  if(D->drops_cnt>=_ROUTE_MAX) {
                              sprintf(error, "Переполнение списка вооружения") ;
                                                     break ;
                                               }               

                                      strncpy(D->drops[D->drops_cnt].weapon, value, sizeof(D->drops[0].weapon)) ;
                                                       D->drops_cnt++ ;
                                             }

          else                               {
                   sprintf(error, "Неизвестный параметр программы управления: %s", key) ;
                                                     break ;
                                             } 

#undef  D

#pragma warning(default : 4701)
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
      } while(1) ;
 
/*--------------------------------------------------- Закрытие файла */ 

         fclose(file) ;

/*-------------------------------------------------------------------*/ 

   if(*error!=0)  return(NULL) ;
                  return(data) ;
}


/********************************************************************/
/*								    */
/*                Копирование настроек компонента                   */

     int  RSS_Unit_Program::CopyInterceptFAD(RSS_Unit *copy)
{

/*------------------------------ Идентификация встроенной программы */

#define  PROGRAM_NAME  ((RSS_Unit_Program_Embeded *)embeded)->program_name

     if(stricmp(PROGRAM_NAME, "INTERCEPT_FAD"))  return(0) ;

           return(-1) ;

#define  P  ((RSS_Unit_Program_Embeded_InterceptFAD *)this->embeded)


#undef   P

#undef  PROGRAM_NAME

/*-------------------------------------------------------------------*/

  return(1) ;
}


/********************************************************************/
/*								    */
/*             Подготовка расчета изменения состояния               */

     int  RSS_Unit_Program::StartInterceptFAD(double  t)
{

/*----------------------------------- Обработка старта моделирования */

#define  PROGRAM_NAME  ((RSS_Unit_Program_Embeded *)embeded)->program_name

     if(stricmp(PROGRAM_NAME, "INTERCEPT_FAD"))  return(0) ;

#define  P  ((RSS_Unit_Program_Embeded_InterceptFAD *)this->embeded)

                 P->stage             =0 ;
                 P->check_time        =0 ;
                 P->dropped           =0 ;
                 P->stage_1_event_done=0 ;
                 P->stage_4_event_done=0 ;

#undef   P

#undef  PROGRAM_NAME

/*-------------------------------------------------------------------*/

  return(1) ;
}


/********************************************************************/
/*                                                                  */
/*            Встроенная программа Intercept From Air Duty          */

     int  RSS_Unit_Program::EmbededInterceptFAD(double t1, double t2, char *callback, int cb_size)
{
                 int  status ;
                char *g_type ;
     RSS_Unit_Target *targets ;
                 int  targets_cnt ;
     RSS_Unit_Target  target_abs ;
              double  distance, distance_min ;
              double  azim ;
                char  text[1024] ;
                char  error[1024] ;
                 int  n ;
                 int  i ;
                 int  j ;

#define  PROGRAM_NAME  ((RSS_Unit_Program_Embeded *)embeded)->program_name

#define  P  ((RSS_Unit_Program_Embeded_InterceptFAD *)this->embeded)
#define  T  P->targets[P->targets_cnt-1]

/*-------------------------------------------------- Проверка модели */

     if(stricmp(PROGRAM_NAME, "INTERCEPT_FAD"))  return(0) ;

/*------------------------------------------------------- Подготовка */

        if(P->g<20)  g_type="G" ;
        else         g_type="R" ;

/*-------------------------------------------------- Контроль высоты */

     if(EventObject->state.y < P->h_min && P->e_direct<0.)  P->e_direct=0. ;

     if(P->e_direct > EventObject->state.elev)  EventObject->state.elev+=P->e_rate*(t2-t1) ;
     if(P->e_direct < EventObject->state.elev)  EventObject->state.elev-=P->e_rate*(t2-t1) ;

/*----------------------------------- Выход в начальную точку поиска */

   if(P->stage==0) {
                          targets_cnt=0 ;

            if(P->route_cnt<2) {
                       SEND_ERROR("EmbededInterceptFAD - Не задан маршрут патрулирования (минимум 2 точки)") ;
                                      return(-1) ;
                               }

        status=Program_emb_ToPoint(P->route[0].x, P->route[0].z, t2-t1, g_type, P->g) ;
     if(status) {
                             P->route_idx=1 ;
                             P->route_dir=1 ;

                   Program_emb_Log("Достигнута зона патрулирования.") ;

                             P->stage++ ;
                }

                   }
/*--------------------------------------------------- Патрулирование */

   else
   if(P->stage==1) {
/*- - - - - - - - - - - - - - - - - - - Отработка событийной команды */
/*
     if(P->stage_1_event[0]  != 0 && 
        P->stage_1_event_done== 0    ) {

             P->stage_1_event_done=1 ;

                   strcat(callback, P->stage_1_event) ;
                   strcat(callback, ";") ;

                                       }
*/
/*- - - - - - - - - - - - - - - - - - - - - - - Движение по маршруту */
        status=Program_emb_ToPoint(P->route[P->route_idx].x, P->route[P->route_idx].z, t2-t1, g_type, P->g) ;
     if(status) {
                           sprintf(text, "Достигнута поворотная точка %f / %f", P->route[P->route_idx].x, P->route[P->route_idx].z) ;
                   Program_emb_Log(text) ;

                             P->route_idx+=P->route_dir ;

                   if(P->route_idx>=P->route_cnt) {
                                                     P->route_idx=P->route_cnt-2 ;
                                                     P->route_dir= -1 ;
                                                  }
                   if(P->route_idx< 0           ) {
                                                     P->route_idx= 1 ;
                                                     P->route_dir= 1 ;
                                                  }
                }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - Поиск цели */
     if(P->radar[0]!=0) {

          status=Program_emb_GetTargets(P->radar,  &targets, &targets_cnt, error) ;
       if(status) {
                        sprintf(text, "EmbededInterceptFAD - %s", error) ;
                     SEND_ERROR(text) ;
                          return(-1) ;
                  }

                            n=-1 ;

       if(targets_cnt) {

                 distance_min= 0. ;

          for(i=0 ; i<targets_cnt ; i++) {                          /* Выбор ближайшей цели... */

            for(j=0 ; j<P->targets_cnt ; j++)                       /*   Не берем уже обстрелянные цели */ 
              if(!stricmp(targets[i].target->Name, P->targets[j].name))  break ;

              if(j<P->targets_cnt)  continue ;

            for(j=0 ; j<P->drops_cnt ; j++)                         /*   Не берем собственные ракеты */ 
              if(!stricmp(targets[i].target->Name, P->drops[j].weapon))  break ;

              if(j<P->drops_cnt)  continue ;

                 distance=sqrt(targets[i].x*targets[i].x+targets[i].z*targets[i].z) ;
              if(      n < 0          ||
                 distance<distance_min  ) {
                                             distance_min=distance ;
                                                       n = i ;
                                          }
                                         }

            if(n>=0) {                                              /* Если есть новая цель... */

                  Program_emb_GetGlobalXYZ(&targets[n], &target_abs) ;

                 P->targets_cnt++ ;
              if(P->targets_cnt>=_TARGETS_MAX) {
                        sprintf(text, "EmbededInterceptFAD - Переполнение списка обстрелянных целей") ;
                     SEND_ERROR(text) ;
                          return(-1) ;
                                               }

                              P->stage     = 2 ;                    /* Фиксируем параметры цели */
                              P->check_time=t1 ;
                      strncpy(T.name, targets[n].target->Name, sizeof(T.name)-1) ;

                               sprintf(text, "Обнаружена цель: %s", T.name) ;
                       Program_emb_Log(text) ;
                     } 

                               free(targets) ;
                       }
                        } 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                   }
/*---------------------------------------------- Выход в точку пуска */

   else
   if(P->stage==2) do {
/*- - - - - - - - - - - - - - - - - - - Отработка событийной команды */
/*
     if(P->stage_2_event[0]  != 0 && 
        P->stage_2_event_done== 0    ) {

             P->stage_2_event_done=1 ;

                   strcat(callback, P->stage_2_event) ;
                   strcat(callback, ";") ;

                                       }
*/
/*- - - - - - - - - - - - - - - - - - - - - - - -  Отслеживание цели */
           Program_emb_GetTargets(P->radar,  &targets, &targets_cnt, error) ;

          for(i=0 ; i<targets_cnt ; i++)                            /* Выбор отслеживаемой цели */
            if(!stricmp(T.name, targets[i].target->Name))  break ;

         if(i>=targets_cnt) {                                       /* Если цель потеряна - проверяем наличие других целей... */
// Движение по прямой
                               free(targets) ;

                               Program_emb_Log("Цель потеряна") ;
                                P->stage=4 ;
                                 break ;
                            }
/*- - - - - - - - - - - - - - - - - - - -  Определение момента пуска */
      if(!stricmp(P->aiming_mode, "DIRECT")) { 

            distance= sqrt(targets[i].x*targets[i].x+targets[i].z*targets[i].z) ;
                azim=atan2(targets[i].x, targets[i].z)*_RAD_TO_GRD ;

        if( distance <=P->fire_distance &&
           fabs(azim)<=P->fire_sector     ) {                       /* Если вошли в сектор пуска... */
                                               P->stage=3 ;
                                            }

                                             }
      else                                   {

                       sprintf(text, "EmbededInterceptFAD - неизвестный режим прицеливания (AIMING_MODE): %s", P->aiming_mode) ;
                     SEND_ERROR(text) ;
                          return(-1) ;
                                             }
/*- - - - - - - - - - - - - - - - - - - - -  Движение в точку пуска */
      if(!stricmp(P->aiming_mode, "DIRECT")) { 

            Program_emb_GetGlobalXYZ(&targets[i], &target_abs) ;
                 Program_emb_ToPoint(target_abs.x, target_abs.z, t2-t1, g_type, P->g) ;

                                             }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                               free(targets) ;

                      } while(0) ;
/*------------------------------------------------------------- Пуск */

   else
   if(P->stage==3) {
/*- - - - - - - - - - - - - - - - - - - -  Движение в процессе пуска */
                                                                    /* В процессе пуска двигаемся по прямой */
       EventObject->state.x+=EventObject->state.x_velocity*(t2-t1) ;
       EventObject->state.y+=EventObject->state.y_velocity*(t2-t1) ;
       EventObject->state.z+=EventObject->state.z_velocity*(t2-t1) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - -  Пуск ракеты */
                    sprintf(text, "Пуск %d : %s", P->dropped+1, P->drops[P->dropped].weapon) ;
            Program_emb_Log(text) ;

                    sprintf(text, "EXEC %s TARGET %s;"
                                  "START %s;",
                                    P->drops[P->dropped].weapon, T.name,
                                    P->drops[P->dropped].weapon         ) ;
                     strcat(callback, text) ;

                      P->dropped++ ;
                      P->check_time=t1 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                P->stage=4 ;                        /* Следующий этап - анализ наличия следующей цели */
                   }
/*------------------------------------ Анализ наличия следующей цели */

  else
  if(P->stage==4) {
/*- - - - - - - - - - - - - - - - - - -  Движение в процессе анализа */
                                                                    /* В процессе анализа двигаемся по прямой */
       EventObject->state.x+=EventObject->state.x_velocity*(t2-t1) ;
       EventObject->state.y+=EventObject->state.y_velocity*(t2-t1) ;
       EventObject->state.z+=EventObject->state.z_velocity*(t2-t1) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - Анализ целей */
     if(P->radar[0]!=0) {

          status=Program_emb_GetTargets(P->radar,  &targets, &targets_cnt, error) ;
       if(status) {
                        sprintf(text, "EmbededInterceptFAD - %s", error) ;
                     SEND_ERROR(text) ;
                          return(-1) ;
                  }

                            n= -1 ;

       if(targets_cnt) {

                distance_min=0. ;

          for(i=0 ; i<targets_cnt ; i++) {                          /* Выбор ближайшей цели */

            for(j=0 ; j<P->targets_cnt ; j++)                       /*   Не берем уже обстрелянные цели */ 
              if(!stricmp(targets[i].target->Name, P->targets[j].name))  break ;

              if(j<P->targets_cnt)  continue ;

            for(j=0 ; j<P->drops_cnt ; j++)                         /*   Не берем собственные ракеты */ 
              if(!stricmp(targets[i].target->Name, P->drops[j].weapon))  break ;

              if(j<P->drops_cnt)  continue ;

                 distance=sqrt(targets[i].x*targets[i].x+targets[i].z*targets[i].z) ;
              if(      n < 0          ||
                 distance<distance_min  ) {
                                             distance_min=distance ;
                                                       n = i ;
                                          }
                                         }
                       }
/*- - - - - - - - - - - - - - - - - - - - - - - -  Результат анализа */
       if(n==-1) {
                       Program_emb_Log("Возвращение в зону ожидания") ;

                              P->stage     = 1 ;                    /* Фиксируем параметры цели */
                              P->check_time=t1 ;
                 }
       else      { 
                               sprintf(text, "Переключение на следующую цель: %s", targets[n].target->Name) ;
                       Program_emb_Log(text) ;

               Program_emb_GetGlobalXYZ(&targets[n], &target_abs) ;

                 P->targets_cnt++ ;
              if(P->targets_cnt>=_TARGETS_MAX) {
                        sprintf(text, "EmbededInterceptFAD - Переполнение списка обстрелянных целей") ;
                     SEND_ERROR(text) ;
                          return(-1) ;
                                               }

                              P->stage     = 2 ;                    /* Фиксируем параметры цели */
                              P->check_time=t1 ;
                      strncpy(T.name, targets[n].target->Name, sizeof(T.name)-1) ;
                 }

                                if(targets_cnt)  free(targets) ;
                        } 

#if  0
/*- - - - - - - - - - - - - - - - - - - - - Движение на боевом курсе */
//           Program_emb_GetTargets (P->stage_4_radar,  &targets, &targets_cnt, error) ;

         if(targets_cnt==0) {                                       /* Если целей нет... */
                               P->stage=1 ;
                                 break ;
                            }

                distance_min= 0. ;
                           n=-1 ;

     for(i=0 ; i<targets_cnt ; i++) {                              /* Ищем ближайшую цель, подподающую под возможность поражения */

          sprintf(text, "<%s>", targets[i].target->Name) ;
       if(strstr(P->fired, text)!=NULL)  continue ;                /* Если цель уже обстреляна... */

                distance= sqrt(targets[i].x*targets[i].x+targets[i].z*targets[i].z) ;
                    azim=atan2(targets[i].x, targets[i].z)*_RAD_TO_GRD ;

       if(P->lock_angle>0) {                                       /* Если боеприпас с зоной захвата... */

          if(azim    >P->lock_angle/2.)  continue ;                /*  Если он за пределами угла захвата... */
          if(distance<P->lock_min     )  continue ;                /*  Если он ближе дальности захвата... */
                           }

          if(distance_min==0 || distance_min>distance) {
                            distance_min=distance ;
                                      n = i ;
                                                       }
                                    }

      if(         n==    -1         ||                             /* Если целей больше нет или кончились средства поражения */
         P->dropped==P->drops_number  ) {

         Program_emb_Log("Уход от цели") ;

                     P->stage= 5 ;

                     P->check_time= t1 ;
                     P->a_direct  = EventObject->state.azim+120. ;
                     P->e_direct  =-P->e_max ;
                                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Сброс */
    if(distance_min<P->drop_distance) {

      if(P->dropped                     == 0 ||
         P->check_time+P->drops_interval<=t1   ) {

                    sprintf(text, "Сброс %d", P->dropped+1) ;
            Program_emb_Log(text) ;

        if(P->lock_target[0]!=0) {
                     strcat(callback, "exec ") ;
                    sprintf(text, P->lock_target, P->drops[P->dropped].weapon,
                                                      targets[n].target->Name) ;
                     strcat(callback, text) ;
                     strcat(callback, ";") ;
                                 }
 
                    sprintf(text, "start %s;", P->drops[P->dropped].weapon) ;
                     strcat(callback, text) ;

              sprintf(text, "<%s>", targets[n].target->Name) ;
               strcat(P->fired, text) ;

                      P->dropped++ ;
                      P->check_time=t1 ;

                            break ;
                                                 }
                                      }
/*- - - - - - - - - - - - - - - - - - - - -  Движение в сторону цели */
          Program_emb_GetGlobalXYZ(&targets[n], &target_abs) ;     /* Идем в сторону цели */
          Program_emb_ToPoint     (target_abs.x, target_abs.z, t2-t1, g_type, P->g) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#endif
                  } 
/*----------------------------------------------------- Уход от цели */
/*
  else
  if(P->stage==5) {


     if(P->stage_4_event[0]           != 0 && 
        P->stage_4_event_done         == 0 && 
        P->stage_4_after+P->check_time<=t1    ) {

             P->stage_4_event_done=1 ;

                   strcat(callback, P->stage_4_event) ;
                   strcat(callback, ";") ;
                                                }

       status=Program_emb_Turn("A", P->a_direct, t2-t1, g_type, P->g) ;

                  }
*/
/*------------------------ Неизвестная ветвь программного управления */

   else            {
                       SEND_ERROR("EmbededInterceptFAD - Unknown stage") ;
                                      return(-1) ;
                   }

/*-------------------------------------------------------------------*/


#undef  P

  return(1) ;
}

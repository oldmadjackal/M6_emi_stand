/*********************************************************************/
/*                                                                   */
/*                  Утилита внешнего моделирования                   */
/*                                                                   */
/*                         Модель MISSILE                            */
/*                                                                   */
/*********************************************************************/

#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <tlhelp32.h>

#include <io.h>
#include <direct.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <time.h>
#include <ctype.h>
#include <process.h>
#include <errno.h>
#include <sys\timeb.h>
#include <sys\types.h>
#include <sys\stat.h>


#include "controls.h"
#include "resource.h"


#include "ExternalModel.h"

#pragma warning(disable : 4996)
#pragma warning(disable : 4267)

#define   _PI           3.1415926
#define   _GRD_TO_RAD   0.017453
#define   _RAD_TO_GRD  57.296

/*----------------------------------------------- Управляющие данные */

typedef struct {
                  char  target[128] ;
                   int  target_idx ;
                  char  missile[128] ;
                   int  active ;
                double  s ;
               }  Trace ;

#define   _TRACES_MAX  100

     Trace  traces[_TRACES_MAX] ;                 /* Параметры отслеживаемых целей */

    double  last_firing ;                         /* Время последнего пуска ЗУР */
       int  missiles_fired ;                      /* Счетчик выпущенных ракет */

/********************************************************************/
/*                                                                  */
/*                          Расчет модели                           */
/*                                                                  */
/*  Параметры модели:                                               */
/*     ZrkV              -  скорость движения шасси, м/с            */
/*     ZrkRmin           - минимальная дальность пуска, м           */
/*     ZrkRmax           - максимальная дальность пуска, м          */
/*     ZrkFiringInterval - интервал между пусками, с                */
/*     ZrkMissile        - имя объекта - "шаблона" ЗУР              */

     int  EM_model_ZRK(Object *data, char *command, char *error)

{
   double  dt ;
   double  azim ;
   double  elev ;
   double  s ;                  /* Расстояние до цели */
   double  s_min ;
   double  t_azim ;             /* Азимут на цель */ 
   double  v_azim ;             /* Азимут движения цели */
      int  idx ;
      int  idx_firing ;
     char  missile[128] ;
      int  i ;

/*----------------------------------- Контроль параметров настройки  */

                *error=0 ;

//    if(__zrk_v              ==0.)   sprintf(error, "Не задан параметр ZrkV - скорость движения шасси, м/с") ;
      if(__zrk_h_min          ==0.)   sprintf(error, "Не задан параметр ZrkHmin - минимальная высота полета воздушных целей, м") ;
      if(__zrk_r_min          ==0.)   sprintf(error, "Не задан параметр ZrkRmin - минимальная дальность пуска, м") ;
      if(__zrk_r_max          ==0.)   sprintf(error, "Не задан параметр ZrkRmax - максимальная дальность пуска, м") ;
      if(__zrk_firing_interval==0.)   sprintf(error, "Не задан параметр ZrkFiringInterval - интержал между пусками, с") ;
      if(__zrk_missile[0]     ==0 )   sprintf(error, "Не задан параметр ZrkMissile - шаблон ЗУР") ;

      if(*error!=0)  return(-1) ;

/*------------------------------------------------------- Подготовка */

               *command=0 ;

              dt=data->t2-data->t1 ;
 
/*-------------------------------------------------- Расчет движения */

                   azim   =data->a_azim*_GRD_TO_RAD ;
                   elev   =data->a_elev*_GRD_TO_RAD ;
           
                data->v_x =__zrk_v*dt*cos(elev)*sin(azim) ;
                data->v_y =__zrk_v*dt*sin(elev) ;
                data->v_z =__zrk_v*dt*cos(elev)*cos(azim) ;

                data->x  +=data->v_x ;
                data->y  +=data->v_y ;
                data->z  +=data->v_z ;

/*---------------------------------------- Анализ целевой обстановки */

       for(i=0 ; i<_TRACES_MAX ; i++)  traces[i].active=0 ;
     
#define   T   __targets[i]

   for(i=0 ; i<__targets_cnt ; i++) {
/*- - - - - - - - - - - - - - - - - - - -  Выделение воздушных целей */
     if(T->y<__zrk_h_min)  continue ;                               /* Если это - не воздушная цель... */
/*- - - - - - - - - - - - - - - - - - Выделение приближающихся целей */
     if(T->v_x==0. &&                                               /* Если неподвижная цель или падающая */
        T->v_z==0. &&
        T->v_y<=0.   )  continue ;

             t_azim=atan2(T->z-data->z, T->x-data->x) ;
             v_azim=atan2(T->v_z,       T->v_x      ) ;

        azim=fabs(t_azim-v_azim) ;
     if(azim<_PI/2)  continue ;                                     /* Если цель не сближается с ЗРК */
/*- - - - - - - - - - - - - - - -  Выделение целей в диапазоне пуска */
        s=sqrt((T->x-data->x)*(T->x-data->x)+                       /* Определяем дальность до цели */
               (T->z-data->z)*(T->z-data->z) ) ;

     if(s<__zrk_r_min || s>__zrk_r_max)  continue ;                 /* Если цель не попадает в диапазон пуска */
/*- - - - - - - - - - - - - - - - - - Регистрация потенциальной цели */
       for(idx=0 ; idx<_TRACES_MAX ; idx++)                         /* Поиск слота трассировки цели */
         if(!strcmp(traces[idx].target, T->name))  break ;

         if(idx<_TRACES_MAX) {
                                traces[idx].target_idx=i ;
                                traces[idx].active    =1 ;
                                traces[idx].s         =s ;
                                    continue ; 
                             }

       for(idx=0 ; idx<_TRACES_MAX ; idx++)                         /* Поиск свободного слота */
         if(traces[idx].target[0]==0)  break ;

         if(idx<_TRACES_MAX) {
                         strcpy(traces[idx].target, T->name) ;
                                traces[idx].target_idx=i ;
                                traces[idx].active    =1 ;
                                traces[idx].s         =s ;
                                    continue ; 
                             }
         else                { 
                                 sprintf(error, "Traces overflow") ;
                                   return(-1) ;
                             }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                    }

#undef   T

/*------------------------------------ Определение приоритетной цели */

#define   T   __targets[i]

                   s_min   = 0 ;
                 idx_firing=-1 ;

    for(idx=0 ; idx<_TRACES_MAX ; idx++) {

      if(traces[idx].active==0)  continue ;                         /* Если неактивный слот */
/*- - - - - - - - - - - - - - - - - Определение уже обстреляной цели */
      if(traces[idx].missile[0]!=0) {

        for(i=0 ; i<__targets_cnt ; i++)                            /* Ищем ЗУР среди объектов */
          if(!strcmp(__targets[i]->name, traces[idx].missile))  break ;

          if(i<__targets_cnt) {                                     /* Если ЗУР есть среди объектов... */

               s=sqrt((T->x-data->x)*(T->x-data->x)+                /* Определяем дальность до ракеты с поправкой на возможный "перелет" при поражении */
                      (T->z-data->z)*(T->z-data->z) )-
                   dt*(fabs(T->v_x)+fabs(T->v_z))      ;

            if(s<traces[idx].s)  continue ;                         /* Если ЗУР ещё не достигла цели... */

                              }

                        traces[idx].missile[0]=0 ;                  /* Отмена назначенной ЗУР */

                                    }
/*- - - - - - - - - - - - - - - - - - - - Определение ближайшей цели */
       if(s_min==0. || s_min>traces[idx].s) {

                   s_min   =traces[idx].s ;
                 idx_firing=       idx   ;
                                            } 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                         }

#undef   T

/*------------------------------------------------- Выполнение пуска */

#define   T   __targets[ traces[idx_firing].target_idx ]

    while(idx_firing>=0) {

       if(data->t1-last_firing < __zrk_firing_interval)  break ;    /* Контроль задержки от последнего пуска */

                  missiles_fired++;
          sprintf(missile, "%s_%d", __zrk_missile, missiles_fired) ;

                   strcpy(traces[idx_firing].missile, missile) ;

                         last_firing=data->t1 ;
   
         sprintf(command, "EXEC %s COPY %s;"
                          "EXEC %s TARGET %s;"
                          "START %s;",
                            __zrk_missile, missile,
                                  missile, T->name,
                                  missile) ;

         sprintf(error, "Firing %s - h=%0.3lf s=%0.3lf", T->name, T->y, traces[idx_firing].s) ;

                               break ;
                         }

#undef   T

/*-------------------------------------------------------------------*/

    return(0) ;
}


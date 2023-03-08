/*********************************************************************/
/*                                                                   */
/*                  ������� �������� �������������                   */
/*                                                                   */
/*                         ������ MISSILE                            */
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

/*----------------------------------------------- ����������� ������ */

typedef struct {
                  char  target[128] ;
                   int  target_idx ;
                  char  missile[128] ;
                double  last_check ;
                   int  active ;
                double  s ;
               }  Trace ;

#define   _TRACES_MAX  100

     Trace  traces[_TRACES_MAX] ;                 /* ��������� ������������� ����� */

    double  last_firing ;                         /* ����� ���������� ����� ��� */
       int  missiles_fired ;                      /* ������� ���������� ����� */

/********************************************************************/
/*                                                                  */
/*                          ������ ������                           */
/*                                                                  */
/*  ��������� ������:                                               */
/*     ZrkV              - �������� �������� �����, �/�             */
/*     ZrkRmin           - ����������� ��������� �����, �           */
/*     ZrkRmax           - ������������ ��������� �����, �          */
/*     ZrkFiringInterval - �������� ����� �������, �                */
/*     ZrkMissile        - ��� ������� - "�������" ���              */

     int  EM_model_ZRK(Object *data, char *command, char *error)

{
   double  dt ;
   double  azim ;
   double  elev ;
   double  s ;                  /* ���������� �� ���� */
   double  s_min ;
   double  t_azim ;             /* ������ �� ���� */ 
   double  v_azim ;             /* ������ �������� ���� */
      int  idx ;
      int  idx_firing ;
     char  missile[128] ;
      int  i ;

/*----------------------------------- �������� ���������� ���������  */

                *error=0 ;

//    if(__zrk_v              ==0.)   sprintf(error, "�� ����� �������� ZrkV - �������� �������� �����, �/�") ;
      if(__zrk_h_min          ==0.)   sprintf(error, "�� ����� �������� ZrkHmin - ����������� ������ ������ ��������� �����, �") ;
      if(__zrk_r_min          ==0.)   sprintf(error, "�� ����� �������� ZrkRmin - ����������� ��������� �����, �") ;
      if(__zrk_r_max          ==0.)   sprintf(error, "�� ����� �������� ZrkRmax - ������������ ��������� �����, �") ;
      if(__zrk_firing_interval==0.)   sprintf(error, "�� ����� �������� ZrkFiringInterval - �������� ����� �������, �") ;
      if(__zrk_missile[0]     ==0 )   sprintf(error, "�� ����� �������� ZrkMissile - ������ ���") ;

      if(*error!=0)  return(-1) ;

/*------------------------------------------------------- ���������� */

               *command=0 ;

              dt=data->t2-data->t1 ;
 
/*----------------------------- ��������� ������� ���� ������������� */

   if(data->start) {

//       memset(__contexts[idx], 0, *sizeof(__contexts[idx])) ;     /* ������������� ��������� */

                   }
/*-------------------------------------------------- ������ �������� */

                   azim   =data->a_azim*_GRD_TO_RAD ;
                   elev   =data->a_elev*_GRD_TO_RAD ;
           
                data->v_x =__zrk_v*dt*cos(elev)*sin(azim) ;
                data->v_y =__zrk_v*dt*sin(elev) ;
                data->v_z =__zrk_v*dt*cos(elev)*cos(azim) ;

                data->x  +=data->v_x ;
                data->y  +=data->v_y ;
                data->z  +=data->v_z ;

/*---------------------------------------- ������ ������� ���������� */

       for(i=0 ; i<_TRACES_MAX ; i++)  traces[i].active=0 ;
     
#define   T   __targets[i]

   for(i=0 ; i<__targets_cnt ; i++) {
/*- - - - - - - - - - - - - - - - - - - -  ��������� ��������� ����� */
     if(T->y<__zrk_h_min)  continue ;                               /* ���� ��� - �� ��������� ����... */
/*- - - - - - - - - - - - - - - - - - ��������� �������������� ����� */
     if(T->v_x==0. &&                                               /* ���� ����������� ���� */
        T->v_z==0. &&
        T->v_y==0.   )  continue ;

             v_azim=atan2(fabs(T->v_y), fabs(T->v_x)+fabs(T->v_z)) ;
     if(T->v_y<0. && fabs(v_azim-_PI/2.)<_PI/10.)  continue ;       /* ���� �������� ����  */  

             t_azim=atan2(T->z-data->z, T->x-data->x) ;
             v_azim=atan2(T->v_z,       T->v_x      ) ;

        azim=fabs(t_azim-v_azim) ;
     if(azim<_PI/2)  continue ;                                     /* ���� ���� �� ���������� � ��� */
/*- - - - - - - - - - - - - - - -  ��������� ����� � ��������� ����� */
        s=sqrt((T->x-data->x)*(T->x-data->x)+                       /* ���������� ��������� �� ���� */
               (T->z-data->z)*(T->z-data->z) ) ;

     if(s<__zrk_r_min || s>__zrk_r_max)  continue ;                 /* ���� ���� �� �������� � �������� ����� */
/*- - - - - - - - - - - - - - - - - - ����������� ������������� ���� */
       for(idx=0 ; idx<_TRACES_MAX ; idx++)                         /* ����� ����� ����������� ���� */
         if(!strcmp(traces[idx].target, T->name))  break ;

         if(idx<_TRACES_MAX) {
                                traces[idx].target_idx=i ;
                                traces[idx].active    =1 ;
                                traces[idx].s         =s ;
                                    continue ; 
                             }

       for(idx=0 ; idx<_TRACES_MAX ; idx++)                         /* ����� ���������� ����� */
         if(traces[idx].target[0]==0)  break ;

         if(idx<_TRACES_MAX) {
                         strcpy(traces[idx].target, T->name) ;
                                traces[idx].target_idx= i ;
                                traces[idx].active    = 1 ;
                                traces[idx].s         = s ;
                                traces[idx].last_check= 0 ;
                                    continue ; 
                             }
         else                { 
                                 sprintf(error, "Traces overflow") ;
                                   return(-1) ;
                             }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                    }

#undef   T

/*------------------------------------ ����������� ������������ ���� */

#define   T   __targets[i]

                   s_min   = 0 ;
                 idx_firing=-1 ;

    for(idx=0 ; idx<_TRACES_MAX ; idx++) {

      if(traces[idx].active==0)  continue ;                         /* ���� ���������� ���� */
/*- - - - - - - - - - - - - - - - - ����������� ��� ����������� ���� */
      if(traces[idx].missile[0]!=0) {

        for(i=0 ; i<__targets_cnt ; i++)                            /* ���� ��� ����� �������� */
          if(!strcmp(__targets[i]->name, traces[idx].missile))  break ;

          if(i<__targets_cnt) {                                     /* ���� ��� ���� ����� ��������... */

               s=sqrt((T->x-data->x)*(T->x-data->x)+                /* ���������� ��������� �� ������ � ��������� �� ��������� "�������" ��� ��������� */
                      (T->z-data->z)*(T->z-data->z) )-
                   dt*(fabs(T->v_x)+fabs(T->v_z))      ;

                 traces[idx].last_check=data->t1 ;

            if(s<traces[idx].s)  continue ;                         /* ���� ��� ��� �� �������� ����... */

                              }

                        traces[idx].missile[0]=0 ;                  /* ������ ����������� ��� */

                                    }
/*- - - - - - - - - - - -  �������� �������� ����� ���������/������� */
       if(data->t1-traces[idx].last_check < 1.)  continue ;
/*- - - - - - - - - - - - - - - - - - - - ����������� ��������� ���� */
       if(s_min==0. || s_min>traces[idx].s) {

                   s_min   =traces[idx].s ;
                 idx_firing=       idx   ;
                                            } 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                         }

#undef   T

/*------------------------------------------------- ���������� ����� */

#define   T   __targets[ traces[idx_firing].target_idx ]

    while(idx_firing>=0) {

       if(data->t1-last_firing < __zrk_firing_interval)  break ;    /* �������� �������� �� ���������� ����� */
/*- - - - - - - - - - - - - - - - - - - - - - ������ ����� ��������� */
/*- - - - - - - - - - - - - - - - - - - - - - - -  ����������� ����� */
                  missiles_fired++;
          sprintf(missile, "%s_%d", __zrk_missile, missiles_fired) ;

                   strcpy(traces[idx_firing].missile, missile) ;
                          traces[idx_firing].last_check=data->t1 ;

                         last_firing=data->t1 ;
/*- - - - - - - - - - - - - - - - - - - - - - ������ ����� ��������� */
               s= sqrt((T->x-data->x)*(T->x-data->x)+
                       (T->z-data->z)*(T->z-data->z) ) ;
            azim=atan2(T->x-data->x, T->z-data->z)*_RAD_TO_GRD ;
            elev=atan2(T->y-data->y,    s        )*_RAD_TO_GRD ;
/*- - - - - - - - - - - - - - - - - - -  ������������ ������ �� ���� */ 
         sprintf(command, "EXEC %s COPY %s;"
                          "EXEC %s TARGET %s;"
                          "START %s;"
                          "EXEC %s ANGLE %.0lf %.0lf 0;",
                            __zrk_missile, missile,
                                  missile, T->name,
                                  missile,
                                  missile, azim, elev) ;

         sprintf(error, "Firing %s - h=%0.3lf s=%0.3lf", T->name, T->y, traces[idx_firing].s) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                               break ;
                         }

#undef   T

/*-------------------------------------------------------------------*/

    return(0) ;
}


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

#define   _GRD_TO_RAD   0.017453
#define   _RAD_TO_GRD  57.296


/********************************************************************/
/*                                                                  */
/*                          ������ ������                           */
/*                                                                  */
/*  ��������� ������:                                               */
/*     MissileV    -  �������� ������ ������, �/�                   */
/*     MissileG    -  ������������� ���������� �������, �/�2        */
/*     MissileHit  -  ������ ��������� ��, �                        */

     int  EM_model_missile(Object *data, char *command, char *error)

{
   Object *target ;
   double  t ;
   double  t_step ;
   double  dt ;
   double  azim ;
   double  elev ;
   double  dx, dy, dz ;         /* ������ �� ���� */
   double  x_t, y_t, z_t ;      /* ����������� ����� */
   double  s ;                  /* ���������� �� ���� */
   double  v ;
   double  r ;                  /* ������ ������� */
   double  h ;
   double  ds ;
   double  a ;                  /* ����������� ���� ������� */
   double  b ;
   double  dv_max ;             /* ������������ ��������� ����� ������� ��������� �������� */
   double  dv_x, dv_y, dv_z ;   /* ��������� ��������� �������� */
   double  dv_s ;               /* ����� ������� ���������� ��������� �������� */
   double  p_x, p_y, p_z ;      /* ���������� ��������� ���� */
   double  pv_x, pv_y, pv_z ;   /* ���������� ������ ��������  */
      int  hit_check ;          /* �������� ��������� ���� */
      int  idx ;
      int  i ;

/*----------------------------------- �������� ���������� ���������  */

                *error=0 ;

      if(__missile_v  ==0.)   sprintf(error, "�� ����� �������� MissileV - �������� ������ ������, �/�") ;
      if(__missile_g  ==0.)   sprintf(error, "�� ����� �������� MissileG - ������������� ���������� ������� ������, �/�2") ;
      if(__missile_hit==0.)   sprintf(error, "�� ����� �������� MissileHit - ������ ��������� �� ������, �") ;

      if(*error!=0)  return(-1) ;

/*------------------------------------------------------- ���������� */

               *command=0 ;

              dt=data->t2-data->t1 ;

             p_x=data->x ;
             p_y=data->y ;
             p_z=data->z ;
            pv_x=data->v_x ;
            pv_y=data->v_y ;
            pv_z=data->v_z ;

/*------------------------------------------------------- ����� ���� */

                              target=NULL ;

   if(data->target[0]!=0) {

     if(__targets_time!=data->t1) {
                          sprintf(error, "������������ ��������� ����� ������� � ������ �����") ;
                                       return(-1) ;
                                  }
 
      for(i=0 ; i<__targets_cnt ; i++)
        if(!stricmp(__targets[i]->name, data->target)) {
                            target=__targets[i] ;
                                         break ;
                                                     }

        if(target==NULL)  sprintf(error, "���� �� ������� - %s", data->target) ;

                          }
/*---------------------------------------------- ������������� ����� */

   if(target==NULL) {
                                   azim   =data->a_azim*_GRD_TO_RAD ;
                                   elev   =data->a_elev*_GRD_TO_RAD ;
           
                                data->v_x =__missile_v*dt*cos(elev)*sin(azim) ;
                                data->v_y =__missile_v*dt*sin(elev) ;
                                data->v_z =__missile_v*dt*cos(elev)*cos(azim) ;

                                data->x  +=data->v_x ;
                                data->y  +=data->v_y ;
                                data->z  +=data->v_z ;

                                        return(0) ;
                    }
/*---------------------------------------------------- ������������� */

                 hit_check=0 ;

   do {
/*- - - - - - - - - - - - - - - - - - - - - ������ ����������� ����� */
                       dx=target->x-data->x ;                       /* ��������� �� ���� */
                       dy=target->y-data->y ;
                       dz=target->z-data->z ;
                        s=sqrt(dx*dx+dy*dy+dz*dz) ;
                         
                        v=__missile_v ;                             /* �������� ������ */
                        t=   0.5*s/v ;

         for(t_step=0.5*t, i=0 ; i<10 ; t_step*=0.5, i++) {         /* ������ ������� ��������� � �������� ���� ��������� �� 2 */

                        x_t=target->x+t*target->v_x ;
                        y_t=target->y+t*target->v_y ;
                        z_t=target->z+t*target->v_z ;

                         dx=x_t-data->x ;
                         dy=y_t-data->y ;
                         dz=z_t-data->z ;
                          s=sqrt(dx*dx+dy*dy+dz*dz) ;

            if(s/t > v)  t+=t_step ;
            else         t-=t_step ;
                                                          } 
/*- - - - - - - - - - - - - - ��������� �������������� �������� ���� */
                       dx=x_t-data->x ;                             /* ������ �� ����������� ����� */
                       dy=y_t-data->y ;
                       dz=z_t-data->z ;
                        s=sqrt(dx*dx+dy*dy+dz*dz) ;
/*- - - - - - - - - -  ������ ����������� ��������� ������� �������� */
                        r= __missile_v*__missile_v/__missile_g ;    /* ������������ ������ ��������� */
                        a= __missile_v*dt/(2.*r) ;
                   dv_max= 2.*__missile_v*sin(0.5*a) ;
/*- - - - - - - - - - - ������ ���������� ��������� ������� �������� */
                       dx=dx*__missile_v/s ;                        /* ��������� ������ �� ���� �� �������� */
                       dy=dy*__missile_v/s ;
                       dz=dz*__missile_v/s ;

                     dv_x=dx-data->v_x ;                            /* ��������� ������ ��������� �������� */
                     dv_y=dy-data->v_y ;
                     dv_z=dz-data->v_z ;
                     dv_s=sqrt(dv_x*dv_x+dv_y*dv_y+dv_z*dv_z) ;
/*- - - - - - - - - -  �������� ����� �� ���� ��������� 180 �������� */
     if(dv_s>1.4*__missile_v) {
                                   data->x+=data->v_x*dt ;
                                   data->y+=data->v_y*dt ;
                                   data->z+=data->v_z*dt ;

                                     hit_check=1 ;
                                          break ;
                              }
/*- - - - - - - - - - - - - - - - - - ������ ������ ������� �������� */
     if(dv_s>dv_max) {                                              /* ���� ��������� ��������� �� ����� ���� ���������� �� ����������... */
                            b=asin(0.5*dv_s/__missile_v) ;
                            h=sqrt(__missile_v*__missile_v-0.25*dv_s*dv_s) ;

                           ds=h*tan(a-b)+0.5*dv_s ;

                          data->v_x+=dv_x*ds/dv_s ;
                          data->v_y+=dv_y*ds/dv_s ;
                          data->v_z+=dv_z*ds/dv_s ;
                               dv_s =sqrt(data->v_x*data->v_x+
                                          data->v_y*data->v_y+
                                          data->v_z*data->v_z ) ;
                          data->v_x*=__missile_v/dv_s ;
                          data->v_y*=__missile_v/dv_s ;
                          data->v_z*=__missile_v/dv_s ;
                     }
     else            {
                          data->v_x =dx ;
                          data->v_y =dy ;
                          data->v_z =dz ;
                     }
/*- - - - - - - - - - - - - - - - - - - - -  ��������� ������� ����� */
             data->x+=0.5*(data->v_x+pv_x)*dt ;
             data->y+=0.5*(data->v_y+pv_y)*dt ;
             data->z+=0.5*(data->v_z+pv_z)*dt ;

                  s=sqrt((data->v_x-pv_x)*(data->v_x-pv_x)+
                         (data->v_y-pv_y)*(data->v_y-pv_y)+
                         (data->v_z-pv_z)*(data->v_z-pv_z) ) ;
/*- - - - - - - - - - - - - - - - - - - - ��������� ����� ���������� */
             data->a_azim=atan2(data->v_x, data->v_z)*_RAD_TO_GRD ;
             data->a_elev=atan2(data->v_y, sqrt(data->v_x*data->v_x+data->v_z*data->v_z))*_RAD_TO_GRD ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      } while(0) ;

/*------------------------------------------ ������������� ��������� */

      for(idx=0 ; idx<__contexts_cnt ; idx++) 
        if(!stricmp(__contexts[idx]->name, data->name))  break ;

        if(idx==__contexts_cnt) {

           __contexts[idx]=(Context *)calloc(1, sizeof(Context)) ;
           __contexts_cnt++ ;

               strcpy(__contexts[idx]->name, data->name) ;
                                }
/*----------------------------- ��������� ������� ���� ������������� */

   if(data->start) {

         memset(__contexts[idx], 0, sizeof(*__contexts[idx])) ;     /* ������������� ��������� */

                   }
/*------------------------------------------ �������� ��������� ���� */
                    
   if(hit_check==1 && __contexts[idx]->missed==0) {

            dx  =__contexts[idx]->x_t-__contexts[idx]->x ;
            dy  =__contexts[idx]->y_t-__contexts[idx]->y ;
            dz  =__contexts[idx]->z_t-__contexts[idx]->z ;

            dv_x=(target->x-__contexts[idx]->x_t)-(p_x-__contexts[idx]->x) ;
            dv_y=(target->y-__contexts[idx]->y_t)-(p_y-__contexts[idx]->y) ;
            dv_z=(target->z-__contexts[idx]->z_t)-(p_z-__contexts[idx]->z) ;

            dt  =-(dx*dv_x+dy*dv_y+dz*dv_z)/(dv_x*dv_x+dv_y*dv_y+dv_z*dv_z) ;
            ds  =sqrt( (dx+dv_x*dt)*(dx+dv_x*dt)+
                       (dy+dv_y*dt)*(dy+dv_y*dt)+
                       (dz+dv_z*dt)*(dz+dv_z*dt) ) ;

              __contexts[idx]->missed=1 ;

//                  EM_message(error) ;

          if(ds<__missile_hit) {
                                  sprintf(error, "HIT time %0.3lf, miss %0.3lf", dt, ds) ;
                                  sprintf(command, "DESTROY;HIT %s;BLAST-A %s;", data->target, data->target) ;
                               }
          else                 {
                                  sprintf(error, "MISSED time %0.3lf, miss %0.3lf", dt, ds) ;
                               }
                                                  }
   else                                           {

          if(hit_check==0)  __contexts[idx]->missed=0 ;
                                                  }

/*--------------------------------- ���������� ������ ������� � ���� */

                __contexts[idx]->x  =  p_x ;
                __contexts[idx]->y  =  p_y ;
                __contexts[idx]->z  =  p_z ;
                __contexts[idx]->x_t=target->x ;
                __contexts[idx]->y_t=target->y ;
                __contexts[idx]->z_t=target->z ;

/*-------------------------------------------------------------------*/

    return(0) ;
}


/********************************************************************/
/*								    */
/*	������ ���������� ����������� "����������� ����������"      */
/*                                                                  */
/*            ���������� ��������� Inetrcept From Air Duty          */
/*                                                                  */
/*  ����� ���������:                                                */
/*   0 - ����� � ����� ��������������                               */
/*   1 - �������������� �� ������ �������� � ��� �����������.       */
/*       ��� ����������� ��������� ���� ->2                         */
/*   2 - ��������� � ����� (��� ���������� � ���������)            */
/*       �� ��������� ������� ��������� - ������ ���� � ����������� */
/*       � ����������. ���� ���� �������� - ������������ � ����� 2 */
/*   3 - ����� �� ���� � ������������� ��������� �� ����������      */
/*       ���������� ������                                          */
/*   4 - ����� ������ ��������                                      */
/*   5 - ���� �� ���� �� ���������                                  */
/*                                                                  */
/*  ��������� ���������:                                            */
/*    PROGRAM        - INTERCEPT_FAD                                */
/*    G              - ����������� ���������� ���������������       */
/*                      <20 - ������� g, >20 - ������ ��������      */
/*    E_MAX          - ������������ ���� ������� (+/-),             */
/*    E_RATE         - ������������ �������� ��������� ������� ���/�*/ 
/*    H_MIN          - ����������� ������ ������ ������������       */
/*    ROUTE          - ����� ��������: X, Z (����� ���� ���������)  */
/*    JUMP_DISTANCE  - ��������� ������ �� ������ ������            */
/*    LOCK_MIN       - ����������� ��������� ������� ����           */
/*    LOCK_MAX       - ������������ ��������� ������� ����          */
/*    LOCK_ANGLE     - ������ �������� ���� ������� ����            */
/*    DROP_DISTANCE  - ��������� ������                             */
/*    DROP_HEIGHT    - ����������� ������ ������                    */
/*    DROPS_NUMBER   - ����� ������� � ����� ������                 */
/*    DROPS_INTERVAL - �������� ����� ��������                      */
/*    DROP_i         - i-�� ������������ ���������                  */
/*    STAGE_0_EVENT  - ������� ��� ���������� ����� 0               */
/*    STAGE_1_RADAR  - ��� ���������� ��������� ������� ��� ����� 1 */
/*    STAGE_2_TIME   - ����������������� ����� 2                    */
/*    STAGE_1_EVENT  - ������� ��� ���������� ����� 1               */
/*    STAGE_4_RADAR  - ��� ���������� ��������� ������� ��� ����� 4 */
/*    STAGE_4_EVENT  - ������� ��� ���������� ����� 4               */
/*    STAGE_4_AFTER  - �������� ���������� ������� STAGE_4_EVENT    */
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

/*-------------------------------------- �������� �������� ��������� */

#define  _ROUTE_MAX  100
#define  _DROPS_MAX   10

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
                 double  jump_distance ;
                 double  drop_height ;
                 double  drop_distance ;
                    int  drops_number ;
                 double  drops_interval ;
       struct {
                   char  weapon[64] ;
              }          drops[_DROPS_MAX] ;
                    int  drops_cnt ;

                   char  stage_0_event[1024] ;
                   char  stage_1_event[1024] ;
                   char  stage_4_event[1024] ;                  
                 double  stage_4_after ;
                   char  stage_1_radar[1024] ;
                 double  stage_2_time ;
                   char  stage_4_radar[1024] ;

                 double  stage ;
                 double  x_0 ;
                 double  z_0 ;
                   char  target[64] ;
                 double  vx_target ;
                 double  vz_target ;
                 double  x_direct ;
                 double  z_direct ;
                 double  a_direct ;
                 double  e_direct ;
                 double  check_time ;
                 double  lock_min ;
                 double  lock_max ;
                 double  lock_angle ;
                   char  lock_target[128] ;
                   char  fired[512] ;
                    int  dropped ;
                    int  stage_1_event_done ;
                    int  stage_4_event_done ;

                } RSS_Unit_Program_Embeded_InterceptFAD ;


/********************************************************************/
/*								    */
/*	     ���������� �������� ���������� ���������               */

  void *RSS_Module_Program::ReadInterceptFAD(char *path, char *error)

{
                                          FILE *file ;
                                          void *data ;
         RSS_Unit_Program_Embeded_InterceptFAD *data_InterceptFAD ;
                                          char  text[256] ;
                                          char *key ;
                                          char *value ;
                                          char *end ;

/*---------------------------------------------------- ������������� */ 

           data=NULL ;
         *error= 0 ;

/*--------------------------------------------------- �������� ����� */ 

       file=fopen(path, "rt") ;
    if(file==NULL) {
                      sprintf(error, "������ %d �������� ����� �������� ��������� : %s", errno, path) ;
                         return(NULL) ;
                   }
/*-------------------------------------------- ���������� ���������� */ 

   do {
/*- - - - - - - - - - - - - - - - - - -  ���������� ��������� ������ */ 
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
                sprintf(error, "�������� ������ ����� �������� ��������� : %s", text) ;
                            break ;
                         }
           *value=0 ;
            value++ ;
/*- - - - - - - - - - - - - - - - - - - - - ��������� ���� ��������� */ 
         if(data==NULL) {

              if(stricmp(key, "PROGRAM")) {
                      sprintf(error, "������ ������ � ����� �������� ��������� ������ ���� PROGRAM=<��� ���������>") ;
                                              break ;
                                          }

              if(stricmp(value, "INTERCEPT_FAD"))  break ;

                 data_InterceptFAD=(RSS_Unit_Program_Embeded_InterceptFAD *)
                                         calloc(1, sizeof(*data_InterceptFAD)) ;
                 data             =data_InterceptFAD ;

                     strcpy(data_InterceptFAD->program_name, value) ;

                                continue ;
                        }
/*- - - - - - - - - - - - - - - - - - - - - - -  ��������� ��������� */ 
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
                              sprintf(error, "������������ ������ ���������� �����") ;
                                                     break ;
                                               }               

                                              D->route[D->route_cnt].x=strtod(value, &end) ;
                                              D->route[D->route_cnt].z=strtod(end+1, &end) ;
                                                       D->route_cnt++ ;
                                              }
          else
          if(!stricmp(key, "STAGE_0_EVENT" ))   strncpy(D->stage_0_event, value, sizeof(D->stage_0_event)-1) ;
          else
          if(!stricmp(key, "STAGE_1_EVENT" ))   strncpy(D->stage_1_event, value, sizeof(D->stage_1_event)-1) ;
          else
          if(!stricmp(key, "STAGE_4_EVENT" ))   strncpy(D->stage_4_event, value, sizeof(D->stage_0_event)-1) ;
          else
          if(!stricmp(key, "STAGE_4_AFTER" ))   D->stage_4_after=strtod(value, &end) ;
          else
          if(!stricmp(key, "STAGE_1_RADAR" ))   strncpy(D->stage_1_radar, value, sizeof(D->stage_1_radar)-1) ;
          else
          if(!stricmp(key, "STAGE_4_RADAR" ))   strncpy(D->stage_4_radar, value, sizeof(D->stage_4_radar)-1) ;
          else
          if(!stricmp(key, "STAGE_2_TIME"  ))   D->stage_2_time=strtod(value, &end) ;
          else
          if(!stricmp(key, "JUMP_DISTANCE" ))   D->jump_distance=strtod(value, &end) ;
          else
          if(!stricmp(key, "LOCK_MIN"      ))   D->lock_min=strtod(value, &end) ;
          else
          if(!stricmp(key, "LOCK_MAX"      ))   D->lock_max=strtod(value, &end) ;
          else
          if(!stricmp(key, "LOCK_ANGLE"    ))   D->lock_angle=strtod(value, &end) ;
          else
          if(!stricmp(key, "LOCK_TARGET"   ))   strncpy(D->lock_target, value, sizeof(D->lock_target)-1) ;
          else
          if(!stricmp(key, "DROP_HEIGHT"   ))   D->drop_height=strtod(value, &end) ;
          else
          if(!stricmp(key, "DROP_DISTANCE" ))   D->drop_distance=strtod(value, &end) ;
          else
          if(!stricmp(key, "DROPS_NUMBER"  ))   D->drops_number=strtoul(value, &end, 10) ;
          else
          if(!stricmp(key, "DROPS_INTERVAL"))   D->drops_interval=strtod(value, &end) ;
          else
          if(!stricmp(key, "DROP"         )) {

                  if(D->drops_cnt>=_ROUTE_MAX) {
                              sprintf(error, "������������ ������ ����������") ;
                                                     break ;
                                               }               

                                      strncpy(D->drops[D->drops_cnt].weapon, value, sizeof(D->drops[0].weapon)) ;
                                                       D->drops_cnt++ ;
                                             }
          else                      {
                   sprintf(error, "����������� �������� ��������� ����������: %s", key) ;
                                                     break ;
                                    } 

#undef  D

#pragma warning(default : 4701)
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
      } while(1) ;
 
/*--------------------------------------------------- �������� ����� */ 

         fclose(file) ;

/*-------------------------------------------------------------------*/ 

   if(*error!=0)  return(NULL) ;
                  return(data) ;
}


/********************************************************************/
/*								    */
/*                ����������� �������� ����������                   */

     int  RSS_Unit_Program::CopyInterceptFAD(RSS_Unit *copy)
{

/*------------------------------ ������������� ���������� ��������� */

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
/*             ���������� ������� ��������� ���������               */

     int  RSS_Unit_Program::StartInterceptFAD(double  t)
{

/*----------------------------------- ��������� ������ ������������� */

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
/*            ���������� ��������� Intercept From Air Duty          */

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

#define  PROGRAM_NAME  ((RSS_Unit_Program_Embeded *)embeded)->program_name

#define  P  ((RSS_Unit_Program_Embeded_InterceptFAD *)this->embeded)

/*-------------------------------------------------- �������� ������ */

     if(stricmp(PROGRAM_NAME, "INTERCEPT_FAD"))  return(0) ;

/*------------------------------------------------------- ���������� */

        if(P->g<20)  g_type="G" ;
        else         g_type="R" ;

/*-------------------------------------------------- �������� ������ */

     if(EventObject->state.y < P->h_min && P->e_direct<0.)  P->e_direct=0. ;

     if(P->e_direct > EventObject->state.elev)  EventObject->state.elev+=P->e_rate*(t2-t1) ;
     if(P->e_direct < EventObject->state.elev)  EventObject->state.elev-=P->e_rate*(t2-t1) ;

/*----------------------------------- ����� � ��������� ����� ������ */

   if(P->stage==0) {

            if(P->route_cnt<2) {
                       SEND_ERROR("EmbededInterceptFAD - �� ����� ������� �������������� (������� 2 �����)") ;
                                      return(-1) ;
                               }

        status=Program_emb_ToPoint(P->route[0].x, P->route[0].z, t2-t1, g_type, P->g) ;
     if(status) {
                             P->route_idx=1 ;
                             P->route_dir=1 ;

                   Program_emb_Log("���������� ���� ��������������.") ;

                   strcat(callback, P->stage_0_event) ;
                   strcat(callback, ";") ;
                             P->stage++ ;
                }

                   }
/*--------------------------------------------------- �������������� */

   else
   if(P->stage==1) {
/*- - - - - - - - - - - - - - - - - - - - - - - �������� �� �������� */
        status=Program_emb_ToPoint(P->route[P->route_idx].x, P->route[P->route_idx].z, t2-t1, g_type, P->g) ;
     if(status) {
                           sprintf(text, "���������� ���������� ����� %f / %f", P->route[P->route_idx].x, P->route[P->route_idx].z) ;
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
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - ����� ���� */
     if(P->stage_1_radar[0]!=0) {

          status=Program_emb_GetTargets(P->stage_1_radar,  &targets, &targets_cnt, error) ;
       if(status) {
                        sprintf(text, "EmbededInterceptFAD - %s", error) ;
                     SEND_ERROR(text) ;
                          return(-1) ;
                  }

       if(targets_cnt) {

                           n= 0 ;
                distance_min=sqrt(targets[n].x*targets[n].x+targets[n].z*targets[n].z) ;

          for(i=1 ; i<targets_cnt ; i++) {                          /* ����� ��������� ���� */
                distance=sqrt(targets[i].x*targets[i].x+targets[i].z*targets[i].z) ;
             if(distance<distance_min) {
                                           distance=distance_min ;
                                                 n = i ;
                                       }
                                         }

               Program_emb_GetGlobalXYZ(&targets[n], &target_abs) ;

                              P->stage     = 2 ;                    /* ��������� ��������� ���� */
                              P->check_time=t1 ;
                      strncpy(P->target, targets[n].target->Name, sizeof(P->target)-1) ;
                              P->x_direct  =target_abs.x ;
                              P->z_direct  =target_abs.z ;

                            Program_emb_Log("���������� ����") ;

                               free(targets) ;
                       }
                                } 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                   }
/*------------------------------------------- ���� ���������� � ���� */

   else
   if(P->stage==2) {
/*- - - - - - - - - - - - - - - - - - - ��������� ���������� ������� */
     if(P->stage_1_event[0]  != 0 && 
        P->stage_1_event_done== 0    ) {

             P->stage_1_event_done=1 ;

                   strcat(callback, P->stage_1_event) ;
                   strcat(callback, ";") ;

                                       }
/*- - - - - - - - - - - - - - - - - - - - -  �������� � ������� ���� */
        status=Program_emb_ToPoint(P->x_direct, P->z_direct, t2-t1, g_type, P->g) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - -  ������ ���� */
      if(t1>=P->check_time+P->stage_2_time) do {

           Program_emb_GetTargets(P->stage_1_radar,  &targets, &targets_cnt, error) ;

         if(targets_cnt==0) {                                       /* ���� ���� ��������... */
                               P->stage=1 ;
                                 break ;
                            }
       
          for(i=0 ; i<targets_cnt ; i++)                            /* ����� ������������� ���� */
            if(!stricmp(P->target, targets[i].target->Name))  break ;

         if(i>=targets_cnt) {                                       /* ���� ���� ��������... */
                               P->stage=1 ;
                                 break ;
                            }


               Program_emb_GetGlobalXYZ(&targets[i], &target_abs) ;

                              P->stage     = 3 ;
                              P->vx_target =(target_abs.x-P->x_direct)/(t1-P->check_time) ;
                              P->vz_target =(target_abs.z-P->z_direct)/(t1-P->check_time) ;
                              P->check_time=t1 ;
                              P->x_direct  =target_abs.x ;
                              P->z_direct  =target_abs.z ;
                              P->e_direct  = -P->e_max ;

                               free(targets) ;

                     Program_emb_Log("����� �� ������ ����...") ;

                                            } while(0) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                   }
/*--------------------------------------------- ����� �� ������ ���� */

   else
   if(P->stage==3) {

                 Program_emb_ToPoint(P->x_direct+P->vx_target*(t1-P->check_time),
                                     P->z_direct+P->vz_target*(t1-P->check_time), t2-t1, g_type, P->g) ;

       distance=Program_emb_Distance(P->x_direct+P->vx_target*(t1-P->check_time), 
                                     P->z_direct+P->vz_target*(t1-P->check_time)) ;

    if(distance<P->jump_distance) {                                 /* ������� �� ������ ������ */

      if(EventObject->state.y > P->drop_height)   P->e_direct= 0 ;
      else                                      {

        if(P->e_direct==0) {
                                  P->e_direct=P->e_max ;
                              Program_emb_Log("������������ �������...") ;
                           }
                                                }    
      
    if(EventObject->state.y >= P->drop_height) {
                                                  P->stage   = 4 ;
                                                  P->e_direct= 0 ;
                                                  P->fired[0]= 0 ;

                     Program_emb_Log("������ ����...") ;
                                               }
                                  }

                   }
/*----------------------------------------------- ����� ����� ������ */

  else 
  if(P->stage==4) do {
/*- - - - - - - - - - - - - - - - - - - - - �������� �� ������ ����� */
           Program_emb_GetTargets (P->stage_4_radar,  &targets, &targets_cnt, error) ;

         if(targets_cnt==0) {                                       /* ���� ����� ���... */
                               P->stage=1 ;
                                 break ;
                            }

                distance_min= 0. ;
                           n=-1 ;

     for(i=0 ; i<targets_cnt ; i++) {                              /* ���� ��������� ����, ����������� ��� ����������� ��������� */

          sprintf(text, "<%s>", targets[i].target->Name) ;
       if(strstr(P->fired, text)!=NULL)  continue ;                /* ���� ���� ��� ����������... */

                distance= sqrt(targets[i].x*targets[i].x+targets[i].z*targets[i].z) ;
                    azim=atan2(targets[i].x, targets[i].z)*_RAD_TO_GRD ;

       if(P->lock_angle>0) {                                       /* ���� ��������� � ����� �������... */

          if(azim    >P->lock_angle/2.)  continue ;                /*  ���� �� �� ��������� ���� �������... */
          if(distance<P->lock_min     )  continue ;                /*  ���� �� ����� ��������� �������... */
                           }

          if(distance_min==0 || distance_min>distance) {
                            distance_min=distance ;
                                      n = i ;
                                                       }
                                    }

      if(         n==    -1         ||                             /* ���� ����� ������ ��� ��� ��������� �������� ��������� */
         P->dropped==P->drops_number  ) {

         Program_emb_Log("���� �� ����") ;

                     P->stage= 5 ;

                     P->check_time= t1 ;
                     P->a_direct  = EventObject->state.azim+120. ;
                     P->e_direct  =-P->e_max ;
                                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  ����� */
    if(distance_min<P->drop_distance) {

      if(P->dropped                     == 0 ||
         P->check_time+P->drops_interval<=t1   ) {

                    sprintf(text, "����� %d", P->dropped+1) ;
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
/*- - - - - - - - - - - - - - - - - - - - -  �������� � ������� ���� */
          Program_emb_GetGlobalXYZ(&targets[n], &target_abs) ;     /* ���� � ������� ���� */
          Program_emb_ToPoint     (target_abs.x, target_abs.z, t2-t1, g_type, P->g) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                     } while(0) ;
/*----------------------------------------------------- ���� �� ���� */

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
/*------------------------ ����������� ����� ������������ ���������� */

   else            {
                       SEND_ERROR("EmbededInterceptFAD - Unknown stage") ;
                                      return(-1) ;
                   }
/*-------------------------------------------------------------------*/


#undef  P

  return(1) ;
}

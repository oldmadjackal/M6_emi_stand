/********************************************************************/
/*								    */
/*	������ ���������� ����������� "����������� ����������"      */
/*                                                                  */
/*            ���������� ��������� Zigzag                           */
/*                                                                  */
/*  ����� ���������:                                                */
/*   0 - ������� �� ����������� �� ���� �� ��������� ���� �         */
/*       ��������� AZIM_1_MIN ... AZIM_1_MAX � ������� ������� �    */
/*       ��������� RANGE_1_MIN ... RANGE_1_MAX                      */
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
/*    PROGRAM        - ZIGZAG                                       */
/*    G              - ����������� ���������� ���������������       */
/*                      <20 - ������� g, >20 - ������ ��������      */
/*    E_MAX          - ������������ ���� ������� (+/-),             */
/*    E_RATE         - ������������ �������� ��������� ������� ���/�*/ 
/*                                                                  */
/*    AZIM_1_MIN     - ����������� ���� �������� �� �����������     */
/*                      �� ���� �� 1-�� �������                     */
/*    AZIM_1_MAX     - ������������ ���� �������� �� �����������    */
/*                      �� ���� �� 1-�� �������                     */
/*    RANGE_1_MIN    - ����������� ����� 1-��� �������              */
/*    RANGE_1_MAX    - ������������ ����� 1-��� �������             */
/*                                                                  */
/*                                                                  */
/*    AZIM_N_MIN     - ����������� ���� �������� �� �����������     */
/*                      �� ���� �� �������� 2...N                   */
/*    AZIM_N_MAX     - ������������ ���� �������� �� �����������    */
/*                      �� ���� �� �������� 2...N                   */
/*    RANGE_N_MIN    - ����������� ����� ������� 2...N              */
/*    RANGE_N_MAX    - ������������ ����� ������� 2...N             */
/*								    */
/*    RANGE_DIRECT   - ��������� ������� ������ �� ����             */
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


  double  G2D_point2segment_distance(double x_p, double y_p, double x_1, double y_1, double x_2, double y_2) ;

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

                 double  azim_1_min ;
                 double  azim_1_max ;
                 double  range_1_min ;
                 double  range_1_max ;

                 double  azim_n_min ;
                 double  azim_n_max ;
                 double  range_n_min ;
                 double  range_n_max ;

                 double  range_direct ;

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

                } RSS_Unit_Program_Embeded_Zigzag ;


/********************************************************************/
/*								    */
/*	     ���������� �������� ���������� ���������               */

  void *RSS_Module_Program::ReadZigzag(char *path, char *error)

{
                             FILE *file ;
                             void *data ;
  RSS_Unit_Program_Embeded_Zigzag *data_Zigzag ;
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

              if(stricmp(value, "ZIGZAG"))  break ;

                 data_Zigzag=(RSS_Unit_Program_Embeded_Zigzag *)
                                         calloc(1, sizeof(*data_Zigzag)) ;
                 data       =data_Zigzag ;

                     strcpy(data_Zigzag->program_name, value) ;

                                continue ;
                        }
/*- - - - - - - - - - - - - - - - - - - - - - -  ��������� ��������� */ 
#pragma warning(disable : 4701)

#define  D  data_Zigzag

          if(!stricmp(key, "G"             ))   D->g=strtod(value, &end) ;
          else
          if(!stricmp(key, "E_MAX"         ))   D->e_max=strtod(value, &end) ;
          else
          if(!stricmp(key, "E_RATE"        ))   D->e_rate=strtod(value, &end) ;
          else
          if(!stricmp(key, "AZIM_1_MIN"    ))   D->azim_1_min=strtod(value, &end) ;
          else
          if(!stricmp(key, "AZIM_1_MAX"    ))   D->azim_1_max=strtod(value, &end) ;
          else
          if(!stricmp(key, "RANGE_1_MIN"   ))   D->range_1_min=strtod(value, &end) ;
          else
          if(!stricmp(key, "RANGE_1_MAX"   ))   D->range_1_max=strtod(value, &end) ;
          else
          if(!stricmp(key, "AZIM_N_MIN"    ))   D->azim_n_min=strtod(value, &end) ;
          else
          if(!stricmp(key, "AZIM_N_MAX"    ))   D->azim_n_max=strtod(value, &end) ;
          else
          if(!stricmp(key, "RANGE_N_MIN"   ))   D->range_n_min=strtod(value, &end) ;
          else
          if(!stricmp(key, "RANGE_N_MAX"   ))   D->range_n_max=strtod(value, &end) ;
          else
          if(!stricmp(key, "RANGE_DIRECT"  ))   D->range_direct=strtod(value, &end) ;
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

     int  RSS_Unit_Program::CopyZigzag(RSS_Unit *copy)
{
  RSS_Unit_Program_Embeded_Zigzag *data_Zigzag ;

/*------------------------------ ������������� ���������� ��������� */

#define  PROGRAM_NAME  ((RSS_Unit_Program_Embeded *)embeded)->program_name

     if(stricmp(PROGRAM_NAME, "ZIGZAG"))  return(0) ;

/*------------------------------------ �������� ��������� ��������� */

          data_Zigzag=(RSS_Unit_Program_Embeded_Zigzag *)
                                 calloc(1, sizeof(*data_Zigzag)) ;

     ((RSS_Unit_Program *)copy)->embeded =data_Zigzag ;

#define  P  ((RSS_Unit_Program_Embeded_Zigzag *)this->embeded)

/*--------------------------------------------- ����������� �������� */
     
    strcpy(data_Zigzag->program_name, P->program_name) ;

           data_Zigzag->g           =P->g ;
           data_Zigzag->e_max       =P->e_max ;
           data_Zigzag->e_rate      =P->e_rate ;

           data_Zigzag->azim_1_min  =P->azim_1_min ;
           data_Zigzag->azim_1_max  =P->azim_1_max ;
           data_Zigzag->range_1_min =P->range_1_min ;
           data_Zigzag->range_1_max =P->range_1_max ;

           data_Zigzag->azim_n_min  =P->azim_n_min ;
           data_Zigzag->azim_n_max  =P->azim_n_max ;
           data_Zigzag->range_n_min =P->range_n_min ;
           data_Zigzag->range_n_max =P->range_n_max ;

           data_Zigzag->range_direct=P->range_direct ;

/*-------------------------------------------------------------------*/

#undef   P

#undef  PROGRAM_NAME

  return(1) ;
}


/********************************************************************/
/*								    */
/*             ���������� ������� ��������� ���������               */

     int  RSS_Unit_Program::StartZigzag(double  t)
{

/*----------------------------------- ��������� ������ ������������� */

#define  PROGRAM_NAME  ((RSS_Unit_Program_Embeded *)embeded)->program_name

     if(stricmp(PROGRAM_NAME, "ZIGZAG"))  return(0) ;

#define  P  ((RSS_Unit_Program_Embeded_Zigzag *)this->embeded)

                 P->stage             =0 ;
                 P->check_time        =0 ;

#undef   P

#undef  PROGRAM_NAME

/*-------------------------------------------------------------------*/

  return(1) ;
}


/********************************************************************/
/*                                                                  */
/*            ���������� ��������� Intercept From Air Duty          */

     int  RSS_Unit_Program::EmbededZigzag(double t1, double t2, char *callback, int cb_size)
{
              double  x_target ;
              double  z_target ;
              double  p ;
              double  lr ;
              double  x ;
              double  z ;
              double  h ;
              double  r1, r2 ;
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

#define  P  ((RSS_Unit_Program_Embeded_Zigzag *)this->embeded)

/*-------------------------------------------------- �������� ������ */

     if(stricmp(PROGRAM_NAME, "ZIGZAG"))  return(0) ;

/*------------------------------------------------------- ���������� */

        if(P->g<20)  g_type="G" ;
        else         g_type="R" ;

/*-------------------------------------------------- �������� ������ */

//     if(EventObject->state.y < P->h_min && P->e_direct<0.)  P->e_direct=0. ;

     if(P->e_direct > EventObject->state.elev)  EventObject->state.elev+=P->e_rate*(t2-t1) ;
     if(P->e_direct < EventObject->state.elev)  EventObject->state.elev-=P->e_rate*(t2-t1) ;

/*------------------------------------ ���������� ������� ���������� */

#define LinearValue  this->Module->gLinearValue

   if(P->stage==0) {
/*- - - - - - - - - - - - - - -  ����������� ��������� ������� ����� */
               x_target=this->Owner->o_target->state.x ;
               z_target=this->Owner->o_target->state.z ;
/*- - - - - - - - - - - - - - - - - - - - ���������� ������� ������� */
           P->route[0].x=this->Owner->state.x ; 
           P->route[0].z=this->Owner->state.z ; 

       distance =LinearValue(P->range_1_min, P->range_1_max) ;      /* ����� ������� */

             lr =LinearValue(-1., 1.)>0.?1.:-1. ;
           azim =atan2(x_target-this->Owner->state.x,               /* ������ ������� */
                       z_target-this->Owner->state.z) ;
           azim+=lr*(LinearValue(P->azim_1_min, P->azim_1_max))*_GRD_TO_RAD ;

           P->route[1].x=this->Owner->state.x+sin(azim)*distance ; 
           P->route[1].z=this->Owner->state.z+cos(azim)*distance ; 

           P->route_cnt=2 ;
/*- - - - - - - - - - - - - - ���������� 2-�� � ����������� �������� */
#define   R_PRV   P->route[P->route_cnt-2]
#define   R_CRN   P->route[P->route_cnt-1]
#define   R_NXT   P->route[P->route_cnt  ]

     do {

       distance =LinearValue(P->range_n_min, P->range_n_max) ;      /* ����� ������� */

             lr =LinearValue(-1., 1.)>0.?1.:-1. ;
           azim =atan2(R_CRN.x-R_PRV.x, R_CRN.z-R_PRV.z) ;          /* ������ ������� */
           azim+=lr*(LinearValue(P->azim_n_min, P->azim_n_max))*_GRD_TO_RAD ;

         R_NXT.x=R_CRN.x+sin(azim)*distance ; 
         R_NXT.z=R_CRN.z+cos(azim)*distance ; 

            h =G2D_point2segment_distance(z_target, z_target, R_CRN.z, R_CRN.x, R_NXT.z, R_NXT.x) ;

            r1=sqrt( (x_target-R_CRN.x)*(x_target-R_CRN.x) + (z_target-R_CRN.z)*(z_target-R_CRN.z) ) ;
            r2=sqrt( (x_target-R_NXT.x)*(x_target-R_NXT.x) + (z_target-R_NXT.z)*(z_target-R_NXT.z) ) ;

         if(r2<r1)  P->route_cnt++ ;                                /* ���� ������� �������� � ����� - ��������� ��� � ������� */

         if(h>0. && h<=P->range_direct)  break ;                    /* ���� �������� ��������� ������ �����... */
                                                
        } while(1) ;          

#undef   R_PRV
#undef   R_CRN
#undef   R_NXT
/*- - - - - - - - - - - - - - - - - -  ���������� ���������� ������� */
           P->route[P->route_cnt].x=x_target ; 
           P->route[P->route_cnt].z=z_target ; 
                    P->route_cnt++ ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
           P->route_idx=1 ;
           P->route_dir=1 ;

           P->stage=1 ;

                   }
/*--------------------------------------------- �������� �� �������� */

   if(P->stage==0)   ;
   if(P->stage==1) {

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

                   }
/*------------------------ ����������� ����� ������������ ���������� */

   else            {
                       SEND_ERROR("EmbededZigzag - Unknown stage") ;
                                      return(-1) ;
                   }
/*-------------------------------------------------------------------*/


#undef  P

  return(1) ;
}


/********************************************************************/
/*                                                                  */
/*           2D - ������ ���������� �� ������� �� �����             */
/*                                                                  */
/*  ���� ��������� >0, ������ �������� �������������� ���������     */
/*   � �������� �������, � ��������� ������ - �� ���� ���������     */

  double  G2D_point2segment_distance(double x_p, double y_p, double x_1, double y_1, double x_2, double y_2)

{
   double  a ;
   double  l ;
   double  b ;
   double  h ;
   double  c ;


        a=fabs(atan2(y_p-y_1, x_p-x_1)-atan2(y_2-y_1, x_2-x_1)) ;    
        l=sqrt((x_p-x_1)*(x_p-x_1)+(y_p-y_1)*(y_p-y_1)) ;
        b=sqrt((x_2-x_1)*(x_2-x_1)+(y_2-y_1)*(y_2-y_1)) ;
        h= sin(a)*l ;
        c= cos(a)*l ;

    if(c>0 && c<b)  return( h) ;
    else            return(-h) ;
}

/*********************************************************************/
/*								     */
/*	          КОМПОНЕНТ "ПРОГРАММНОЕ УПРАВЛЕНИЕ"                 */
/*								     */
/*	           Библиотека функций и объекты DCL        	     */
/*								     */
/*********************************************************************/


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

#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text)
#define  CREATE_DIALOG       CreateDialogIndirectParam

#include "..\Emi_root\controls.h"
#include "resource.h"

#pragma warning(disable : 4996)


/********************************************************************/
/*								    */
/*               Выдача отладочного сообщения с ожиданием           */

  Dcl_decl *Program_dcl_Message(Lang_DCL  *dcl_kernel,
                                Dcl_decl **pars, 
                                     int   pars_cnt)
{
   char  text[1024] ;

 static     char  chr_value[512] ;          /* Буфер строки */
 static Dcl_decl  chr_return={ _CHR_PTR, 0,0,0,"", chr_value, NULL, 0, 512} ;

#define  _LOG_MAX   100

/*---------------------------------------------------- Инициализация */

                 memset(chr_value, 0, sizeof(chr_value)) ;
                        chr_return.size=0 ;

/*-------------------------------------------- Извлечение параметров */

       if(pars_cnt     !=1   ||                                     /* Проверяем число параметров */
	  pars[0]->addr==NULL  ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&chr_return) ; 
                                 }

                    memset(text, 0, sizeof(text)) ;                 /* Извлекаем ссылку на файл */
        if(pars[0]->size>=sizeof(text))
                    memcpy(text, pars[0]->addr, sizeof(text)-1) ;
        else        memcpy(text, pars[0]->addr, pars[0]->size) ;

/*------------------------------------------------- Выдача сообщения */

               SEND_ERROR(text) ;

/*-------------------------------------------------------------------*/

  return(&chr_return) ;
}


/********************************************************************/
/*								    */
/*                   Занесение данных в отладочный лог              */

  Dcl_decl *Program_dcl_Log( Lang_DCL  *dcl_kernel,
                             Dcl_decl **pars, 
                                  int   pars_cnt)
{
   char  text[1024] ;

 static     char  chr_value[512] ;          /* Буфер строки */
 static Dcl_decl  chr_return={ _CHR_PTR, 0,0,0,"", chr_value, NULL, 0, 512} ;

#define  _LOG_MAX   100

/*---------------------------------------------------- Инициализация */

                 memset(chr_value, 0, sizeof(chr_value)) ;
                        chr_return.size=0 ;

/*-------------------------------------------- Извлечение параметров */

       if(pars_cnt     !=1   ||                                     /* Проверяем число параметров */
	  pars[0]->addr==NULL  ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&chr_return) ; 
                                 }

                    memset(text, 0, sizeof(text)) ;                 /* Извлекаем ссылку на файл */
        if(pars[0]->size>=sizeof(text))
                    memcpy(text, pars[0]->addr, sizeof(text)-1) ;
        else        memcpy(text, pars[0]->addr, pars[0]->size) ;

/*----------------------------------------------------- Запись в лог */

                       Program_emb_Log(text) ;

/*-------------------------------------------------------------------*/

  return(&chr_return) ;
}


  int  Program_emb_Log(char *text)
{
     HWND  hDlg ;
  LRESULT  rows_cnt ;
      int  i ;

#define  _LOG_MAX   100

/*----------------------------------------------------- Запись в лог */

     if(EventUnit!=NULL) {
                                hDlg=EventUnit->hWnd ;

            rows_cnt=LB_GET_COUNT(IDC_LOG) ;                        /* Очистка лога */
         if(rows_cnt>_LOG_MAX) {
               for(i=0 ; i<rows_cnt-_LOG_MAX ; i++)  LB_DEL_ROW(IDC_LOG, i) ;
                               }

           LB_ADD_ROW(IDC_LOG, text) ;
           LB_TOP_ROW(IDC_LOG, LB_GET_COUNT(IDC_LOG)-1) ;

                         }
/*-------------------------------------------------------------------*/

#undef  _LOG_MAX

  return(0) ;
}


/********************************************************************/
/*								    */
/*                  Определение дистанции до точки                  */
/*								    */
/*   Параметры: X, Z целевой точки                                  */

  Dcl_decl *Program_dcl_Distance( Lang_DCL  *dcl_kernel,
                                  Dcl_decl **pars, 
                                       int   pars_cnt)
{
     double  x_target ;
     double  z_target ;

 static   double  dgt_value ;          /* Буфер числового значения */
 static Dcl_decl  dgt_return={ _DGT_VAL, 0,0,0,"", &dgt_value, NULL, 1, 1} ;

/*---------------------------------------------------- Инициализация */

                        dgt_value=0. ;

/*-------------------------------------------- Извлечение параметров */

       if(pars[0]->addr==NULL ||                                    /* Проверяем наличие основных параметров */
	  pars[1]->addr==NULL   ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                                  }
        
          x_target=dcl_kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;
          z_target=dcl_kernel->iDgt_get(pars[1]->addr, pars[1]->type) ;

/*------------------------------------------------- Расчёт дистанции */

         dgt_value=Program_emb_Distance(x_target, z_target) ;

/*-------------------------------------------------------------------*/

  return(&dgt_return) ;
}


  double  Program_emb_Distance(double  x, double  z)

{
   return( sqrt((x-EventObject->state.x)*(x-EventObject->state.x)+
                (z-EventObject->state.z)*(z-EventObject->state.z) ) ) ;
}


/********************************************************************/
/*								    */
/*                  Изменение направления движения                  */
/*								    */
/*   Параметры: ось, угол, [квант времени, параметры манёвра]       */
/*                                                                  */
/*                                                                  */
/*  Ось: A - азимут, E - тангаж, R - рыскание                       */
/*  Параметры манёвра задают ограничение скорости поворота:         */
/*      <Вид ограничения> <Величина ограничения>                    */
/*                                                                  */
/*      <Вид ограничения>: R - по радиусу, G - по перегрузке        */
/*      <Величина ограничения>: радиус поворота в метрах,           */
/*                              перегрузка в единицах g             */

  Dcl_decl *Program_dcl_Turn( Lang_DCL  *dcl_kernel,
                              Dcl_decl **pars, 
                                   int   pars_cnt)
{
       char  elem[32] ;
     double  angle ;
     double  dt ;
       char  limit_type[16] ;
     double  g ;
     double  rad ;
     double  v ;
     double  dr ;
     double  angle_diff ;
     double  da ;
     double  l ;
  RSS_Point  target ;
       char *target_pars ;
    char  cmd[1024] ;

 static   double  dgt_value ;          /* Буфер числового значения */
 static Dcl_decl  dgt_return={ _DGT_VAL, 0,0,0,"", &dgt_value, NULL, 1, 1} ;

#define  _LOG_MAX   100

/*---------------------------------------------------- Инициализация */

                        dgt_value=0. ;

/*-------------------------------------------- Извлечение параметров */

       if(pars_cnt!=2 &&                                            /* Проверяем число параметров */
	  pars_cnt!=5   ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                          }

       if(pars[0]->addr==NULL ||                                    /* Проверяем наличие основных параметров */
	  pars[1]->addr==NULL   ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                                  }

      if(pars_cnt==5)                                               /* Проверяем наличие параметров манёвра */
       if(pars[2]->addr==NULL ||
	  pars[3]->addr==NULL ||
	  pars[4]->addr==NULL   ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                                  }


                    memset(elem, 0, sizeof(elem)) ;                 /* Извлекаем имя компонента */
        if(pars[0]->size>=sizeof(elem))
                    memcpy(elem, pars[0]->addr, sizeof(elem)-1) ;
        else        memcpy(elem, pars[0]->addr, pars[0]->size) ;

          angle=dcl_kernel->iDgt_get(pars[1]->addr, pars[1]->type) ;

                    memset(limit_type, 0, sizeof(limit_type)) ;

     if(pars_cnt==5) {

                dt=dcl_kernel->iDgt_get(pars[2]->addr, pars[2]->type) ;

        if(pars[3]->size>=sizeof(limit_type))
                    memcpy(limit_type, pars[3]->addr, sizeof(limit_type)-1) ;
        else        memcpy(limit_type, pars[3]->addr, pars[3]->size) ;

               rad=dcl_kernel->iDgt_get(pars[4]->addr, pars[4]->type) ;
                 g=rad ;
                     }
/*---------------------------------------------- Проверка параметров */

                strupr(elem) ;

    if(limit_type[0]==0) {
      if(stricmp(elem, "A") &&
         stricmp(elem, "E") &&
         stricmp(elem, "R")   ) {
                                  dcl_kernel->mError_code=_DCLE_USER_DEFINED ;
                           strcpy(dcl_kernel->mError_details, "Unsupported manoeuvre. Must by A, E or R.") ;
                                    return(&dgt_return) ;
                                }
                          }
    else                  {
      if(stricmp(elem, "A")) {
                                  dcl_kernel->mError_code=_DCLE_USER_DEFINED ;
                           strcpy(dcl_kernel->mError_details, "Unsupported manoeuvre. Must by A.") ;
                                    return(&dgt_return) ;
                             }
                          }

                strupr(limit_type) ;

     if(limit_type[0]!='R' &&
        limit_type[0]!='G'   ) {
                                  dcl_kernel->mError_code=_DCLE_USER_DEFINED ;
                           strcpy(dcl_kernel->mError_details, "Unknown type of manoeuvre's limit. Must by R or G") ;
                                    return(&dgt_return) ;
                               }

     if(limit_type[0]=='R' && rad<=0. ||
        limit_type[0]=='G' &&   g<=0.   ) {
                                  dcl_kernel->mError_code=_DCLE_USER_DEFINED ;
                           strcpy(dcl_kernel->mError_details, "invalid limit of manoeuvre. Must by >0") ;
                                    return(&dgt_return) ;
                                          } 
/*------------------------------------------ Без ограничения манёвра */

  if(limit_type[0]==0) {

    if(!stricmp(elem, "A") ||
       !stricmp(elem, "E") ||
       !stricmp(elem, "R")   ) {
                                 sprintf(cmd, "%s angle/%c %s %lf",
                                               EventObject->Type, elem[0], EventObject->Name, angle) ;
        EventObject->Module->vExecuteCmd(cmd) ;

                                            dgt_value=1. ;
                                    return(&dgt_return) ;
                               }  
                       }
/*----------------------------------------- Рассчёт радиуса поворота */

            v=sqrt(EventObject->state.x_velocity*EventObject->state.x_velocity+
                   EventObject->state.z_velocity*EventObject->state.z_velocity ) ;

    if(limit_type[0]=='G') {
                               rad=v*v/(10.*g) ;
                           }
/*------------------------------------------- С ограничением манёвра */

#pragma warning(disable : 4701)

           dr=dt*v ;
           da=_RAD_TO_GRD*dt*v/rad ;

                                angle_diff =angle-EventObject->state.azim ;
       while(angle_diff> 180.)  angle_diff-=360. ;
       while(angle_diff<-180.)  angle_diff+=360. ;

       if(fabs(angle_diff)<da) {
                                     da=fabs(angle_diff) ;
                         if(da>1.)  rad=dt*v/(_GRD_TO_RAD*da) ;
                                      g= 0. ;

                                        dgt_value=1. ;
                               }

       if(da<5.) {
                   l=dt*v ;
                 }
       else      {
                    l=rad*sin(_GRD_TO_RAD*(da))/sin(_GRD_TO_RAD*(90.-da/2.)) ;
                 } 

       if(angle_diff<0.)  da=-da ;

                            target.x   =EventObject->state.x+l*sin(_GRD_TO_RAD*(EventObject->state.azim+da/2.)) ;
                            target.z   =EventObject->state.z+l*cos(_GRD_TO_RAD*(EventObject->state.azim+da/2.)) ;
                            target.azim=EventObject->state.azim+da ;
                  if(da>0)  target.roll= atan(g)*_RAD_TO_GRD ;
                  else      target.roll=-atan(g)*_RAD_TO_GRD ;
                            target_pars="XZAR" ;

          EventObject->vCalculateDirect(&target, target_pars) ;

#pragma warning(default : 4701)

/*-------------------------------------------------------------------*/

  return(&dgt_return) ;
}


  int  Program_emb_Turn(  char *elem, 
                        double  angle,
                        double  dt,
                          char *limit_type,
                        double  g           )
{
        int  result ;
     double  rad ;
     double  v ;
     double  dr ;
     double  angle_diff ;
     double  da ;
     double  l ;
  RSS_Point  target ;
       char *target_pars ;
       char  cmd[1024] ;

/*---------------------------------------------------- Инициализация */

                           result=0 ;

/*------------------------------------------ Без ограничения манёвра */

  if(limit_type[0]==0) {

    if(!stricmp(elem, "A") ||
       !stricmp(elem, "E") ||
       !stricmp(elem, "R")   ) {
                                 sprintf(cmd, "%s angle/%c %s %lf",
                                               EventObject->Type, elem[0], EventObject->Name, angle) ;
        EventObject->Module->vExecuteCmd(cmd) ;

                                    return(1) ;
                               }  
                       }
/*----------------------------------------- Рассчёт радиуса поворота */

            v=sqrt(EventObject->state.x_velocity*EventObject->state.x_velocity+
                   EventObject->state.z_velocity*EventObject->state.z_velocity ) ;

    if(limit_type[0]=='G') {
                               rad=v*v/(10.*g) ;
                           }
/*------------------------------------------- С ограничением манёвра */

#pragma warning(disable : 4701)

           dr=dt*v ;
           da=_RAD_TO_GRD*dt*v/rad ;

                                angle_diff =angle-EventObject->state.azim ;
       while(angle_diff> 180.)  angle_diff-=360. ;
       while(angle_diff<-180.)  angle_diff+=360. ;

       if(fabs(angle_diff)<da) {
                                     da=fabs(angle_diff) ;
                         if(da>1.)  rad=dt*v/(_GRD_TO_RAD*da) ;
                                      g= 0. ;

                                        result=1 ;
                               }

       if(da<5.) {
                   l=dt*v ;
                 }
       else      {
                    l=rad*sin(_GRD_TO_RAD*(da))/sin(_GRD_TO_RAD*(90.-da/2.)) ;
                 } 

       if(angle_diff<0.)  da=-da ;

                            target.x   =EventObject->state.x+l*sin(_GRD_TO_RAD*(EventObject->state.azim+da/2.)) ;
                            target.z   =EventObject->state.z+l*cos(_GRD_TO_RAD*(EventObject->state.azim+da/2.)) ;
                            target.azim=EventObject->state.azim+da ;
                  if(da>0)  target.roll= atan(g)*_RAD_TO_GRD ;
                  else      target.roll=-atan(g)*_RAD_TO_GRD ;
                            target_pars="XZAR" ;

          EventObject->vCalculateDirect(&target, target_pars) ;

#pragma warning(default : 4701)

/*-------------------------------------------------------------------*/


  return(result) ;
}


/********************************************************************/
/*								    */
/*                         Движение в точку                         */
/*								    */
/*   Параметры: X, Z, квант времени, [параметры манёвра]            */
/*                                                                  */
/*  Параметры манёвра задают ограничение скорости поворота:         */
/*      <Вид ограничения> <Величина ограничения>                    */
/*                                                                  */
/*      <Вид ограничения>: R - по радиусу, G - по перегрузке        */
/*      <Величина ограничения>: радиус поворота в метрах,           */
/*                              перегрузка в единицах g             */
/*                                                                  */
/*  Сначала выполняется форсированный разворот на цель,             */
/*  далее - сближение с ней                                         */

  Dcl_decl *Program_dcl_ToPoint( Lang_DCL  *dcl_kernel,
                                 Dcl_decl **pars, 
                                      int   pars_cnt)
{
     double  x_target ;
     double  z_target ;
     double  dt ;
       char  limit_type[16] ;
     double  rad ;

 static   double  dgt_value ;          /* Буфер числового значения */
 static Dcl_decl  dgt_return={ _DGT_VAL, 0,0,0,"", &dgt_value, NULL, 1, 1} ;

/*---------------------------------------------------- Инициализация */

                        dgt_value=0. ;

/*-------------------------------------------- Извлечение параметров */

       if(pars_cnt!=3 &&                                            /* Проверяем число параметров */
	  pars_cnt!=5   ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                          }

       if(pars[0]->addr==NULL ||                                    /* Проверяем наличие основных параметров */
	  pars[1]->addr==NULL ||
	  pars[2]->addr==NULL   ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                                  }

      if(pars_cnt==5)                                               /* Проверяем наличие параметров манёвра */
       if(pars[3]->addr==NULL ||
	  pars[4]->addr==NULL   ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                                  }
         

          x_target=dcl_kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;
          z_target=dcl_kernel->iDgt_get(pars[1]->addr, pars[1]->type) ;
                dt=dcl_kernel->iDgt_get(pars[2]->addr, pars[2]->type) ;

                         memset(limit_type, 0, sizeof(limit_type)) ;

     if(pars_cnt==5) {

        if(pars[3]->size>=sizeof(limit_type))
                    memcpy(limit_type, pars[3]->addr, sizeof(limit_type)-1) ;
        else        memcpy(limit_type, pars[3]->addr, pars[3]->size) ;

               rad=dcl_kernel->iDgt_get(pars[4]->addr, pars[4]->type) ;
                     }
/*---------------------------------------------- Проверка параметров */

                strupr(limit_type) ;

     if(limit_type[0]!='R' &&
        limit_type[0]!='G'   ) {
                                  dcl_kernel->mError_code=_DCLE_USER_DEFINED ;
                           strcpy(dcl_kernel->mError_details, "Unknown type of manoeuvre's limit. Must by R or G") ;
                                    return(&dgt_return) ;
                               }

     if(limit_type[0]=='R' && rad<=0. ||
        limit_type[0]=='G' && rad<=0.   ) {
                                  dcl_kernel->mError_code=_DCLE_USER_DEFINED ;
                           strcpy(dcl_kernel->mError_details, "invalid limit of manoeuvre. Must by >0") ;
                                    return(&dgt_return) ;
                                          } 
/*--------------------------------------------- Отработка управления */

    dgt_value=Program_emb_ToPoint(x_target, z_target, dt, limit_type, rad) ;

/*-------------------------------------------------------------------*/

  return(&dgt_return) ;
}

  int Program_emb_ToPoint(double  x_target,
                          double  z_target,
                          double  dt,
                            char *limit_type,
                          double  g          )
{
     double  rad ;
     double  v ;
     double  r ;
     double  dr ;
     double  angle ;
     double  angle_diff ;
     double  da ;
     double  l ;
  RSS_Point  target ;
       char *target_pars ;

/*---------------------------------------------------- Инициализация */

                              rad=g ;

/*-------------------------------- Проверка достижения целевой точки */

        r=sqrt((x_target-EventObject->state.x)*(x_target-EventObject->state.x)+
               (z_target-EventObject->state.z)*(z_target-EventObject->state.z) ) ;

        v=sqrt(EventObject->state.x_velocity*EventObject->state.x_velocity+
               EventObject->state.z_velocity*EventObject->state.z_velocity ) ;
       dr=dt*v ;

    if(dr>r) {
                 target.x   =EventObject->state.x + dt*EventObject->state.x_velocity ;
                 target.z   =EventObject->state.z + dt*EventObject->state.z_velocity ;
                 target.azim=EventObject->state.azim ;
                 target.roll= 0. ;
                 target_pars="XZAR" ;

          EventObject->vCalculateDirect(&target, target_pars) ;

                            return(1) ;
             }
/*----------------------------------------- Рассчёт радиуса поворота */

    if(limit_type[0]=='G') {
                               rad=v*v/(10.*g) ;
                           }
/*----------------------------------- Изменение направления движения */

          angle=_RAD_TO_GRD*atan2(x_target-EventObject->state.x, 
                                  z_target-EventObject->state.z ) ;
/*- - - - - - - - - - - - - - - - - - - - - - С ограничением манёвра */
   if(limit_type[0]!=0) {
                                        da =_RAD_TO_GRD*dt*v/rad ;

                                angle_diff =angle-EventObject->state.azim ;
       while(angle_diff> 180.)  angle_diff-=360. ;
       while(angle_diff<-180.)  angle_diff+=360. ;

       if(fabs(angle_diff)<da) {
                                     da=fabs(angle_diff) ;
                                    rad=dt*v/(_GRD_TO_RAD*da) ;
                                      g=v*v/(10.*rad) ;
                               }

       if(da<5.) {
                   l=dt*v ;
                 }
       else      {
                    l=rad*sin(_GRD_TO_RAD*(da))/sin(_GRD_TO_RAD*(90.-da/2.)) ;
                 } 

       if(angle_diff<0.)  da=-da ;

                            target.x   = EventObject->state.x+l*sin(_GRD_TO_RAD*(EventObject->state.azim+da/2.)) ;
                            target.z   = EventObject->state.z+l*cos(_GRD_TO_RAD*(EventObject->state.azim+da/2.)) ;
                            target.azim= EventObject->state.azim+da ;
                  if(da>0)  target.roll= atan(g)*_RAD_TO_GRD ;
                  else      target.roll=-atan(g)*_RAD_TO_GRD ;
                            target_pars= "XZAR" ;
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - "Мгновенный" поворот */
   else                 {
                            target.azim=angle ;
                            target_pars="A" ;
                        }
/*- - - - - - - - - - - - - - - - - - - - Задание целевого положения */
          EventObject->vCalculateDirect(&target, target_pars) ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*                         Выход на линию                           */
/*								    */
/*   Параметры: X, Z, азимут, квант времени, [параметры манёвра]    */
/*                                                                  */
/*  Параметры манёвра задают ограничение скорости поворота:         */
/*      <Вид ограничения> <Величина ограничения>                    */
/*                                                                  */
/*      <Вид ограничения>: R - по радиусу, G - по перегрузке        */
/*      <Величина ограничения>: радиус поворота в метрах,           */
/*                              перегрузка в единицах g             */

  Dcl_decl *Program_dcl_ToLine( Lang_DCL  *dcl_kernel,
                                Dcl_decl **pars, 
                                     int   pars_cnt)
{
     double  x_target ;
     double  z_target ;
     double  a_target ;
     double  dt ;
       char  limit_type[16] ;
     double  g ;
     double  rad ;

 static   double  dgt_value ;          /* Буфер числового значения */
 static Dcl_decl  dgt_return={ _DGT_VAL, 0,0,0,"", &dgt_value, NULL, 1, 1} ;


#define  _LOG_MAX   100

/*---------------------------------------------------- Инициализация */

                        dgt_value=0. ;

/*-------------------------------------------- Извлечение параметров */

       if(pars_cnt!=4 &&                                            /* Проверяем число параметров */
	  pars_cnt!=6   ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                          }

       if(pars[0]->addr==NULL ||                                    /* Проверяем наличие основных параметров */
	  pars[1]->addr==NULL ||
	  pars[2]->addr==NULL ||
	  pars[3]->addr==NULL   ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                                  }

      if(pars_cnt==6)                                               /* Проверяем наличие параметров манёвра */
       if(pars[4]->addr==NULL ||
	  pars[5]->addr==NULL   ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                                  }
         

          x_target=dcl_kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;
          z_target=dcl_kernel->iDgt_get(pars[1]->addr, pars[1]->type) ;
          a_target=dcl_kernel->iDgt_get(pars[2]->addr, pars[2]->type) ;
                dt=dcl_kernel->iDgt_get(pars[3]->addr, pars[3]->type) ;

                         memset(limit_type, 0, sizeof(limit_type)) ;

     if(pars_cnt==6) {

        if(pars[4]->size>=sizeof(limit_type))
                    memcpy(limit_type, pars[4]->addr, sizeof(limit_type)-1) ;
        else        memcpy(limit_type, pars[4]->addr, pars[4]->size) ;

               rad=dcl_kernel->iDgt_get(pars[5]->addr, pars[5]->type) ;
                 g=rad ;
                     }
/*---------------------------------------------- Проверка параметров */

                strupr(limit_type) ;

     if(limit_type[0]!='R' &&
        limit_type[0]!='G'   ) {
                                  dcl_kernel->mError_code=_DCLE_USER_DEFINED ;
                           strcpy(dcl_kernel->mError_details, "Unknown type of manoeuvre's limit. Must by R or G") ;
                                    return(&dgt_return) ;
                               }

     if(limit_type[0]=='R' && rad<=0. ||
        limit_type[0]=='G' &&   g<=0.   ) {
                                  dcl_kernel->mError_code=_DCLE_USER_DEFINED ;
                           strcpy(dcl_kernel->mError_details, "invalid limit of manoeuvre. Must by >0") ;
                                    return(&dgt_return) ;
                                          } 
/*--------------------------------------------------- Выход на линию */

     dgt_value=Program_emb_ToLine(x_target, z_target,
                                  a_target, dt, limit_type, g) ;

/*-------------------------------------------------------------------*/

  return(&dgt_return) ;
}

  int  Program_emb_ToLine(double  x_target, double  z_target, 
                          double  a_target, double  dt, char *limit_type, double  g)
{
     double  rad ;
     double  r, dr ;
     double  v ;
     double  angle, da, angle_diff ;
     double  x1_work, x2_work ;
     double  z1_work, z2_work ;
     double  r1, r2 ;
     double  l ;
  RSS_Point  target ;
       char *target_pars ;

/*---------------------------------------------------- Инициализация */

                              rad=g ;

/*---------------------------- Расчёт промежуточной точки приведения */

         r=sqrt((x_target-EventObject->state.x)*(x_target-EventObject->state.x)+
                (z_target-EventObject->state.z)*(z_target-EventObject->state.z) ) ;

     angle=atan(fabs((x_target-EventObject->state.x)/(z_target-EventObject->state.z))) ;
        r*=fabs(cos(angle+_GRD_TO_RAD*a_target)) ; 

   x1_work=x_target+r*sin(_GRD_TO_RAD*      a_target ) ;
   z1_work=z_target+r*cos(_GRD_TO_RAD*      a_target ) ;
   x2_work=x_target+r*sin(_GRD_TO_RAD*(180.+a_target)) ;
   z2_work=z_target+r*cos(_GRD_TO_RAD*(180.+a_target)) ;

        r1=sqrt((x1_work-EventObject->state.x)*(x1_work-EventObject->state.x)+
                (z1_work-EventObject->state.z)*(z1_work-EventObject->state.z) ) ;
        r2=sqrt((x2_work-EventObject->state.x)*(x2_work-EventObject->state.x)+
                (z2_work-EventObject->state.z)*(z2_work-EventObject->state.z) ) ;
 
   if(r1>r2) {  r1     =r2 ;
                x1_work=x2_work ;
                z1_work=z2_work ;  }

/*---------------------- Определение скорости и проходимой дистанции */

        v=sqrt(EventObject->state.x_velocity*EventObject->state.x_velocity+
               EventObject->state.z_velocity*EventObject->state.z_velocity ) ;
       dr=dt*v ;

/*------------------------------------------ Без ограничения манёвра */

   if(limit_type[0]==0) {
/*- - - - - - - - - - - - - - - -  Проверка достижения целевой точки */
    if(dr>r1) {
                 target.x   =EventObject->state.x+dr*sin(_GRD_TO_RAD*EventObject->state.azim) ;
                 target.z   =EventObject->state.z+dt*cos(_GRD_TO_RAD*EventObject->state.azim) ;
                 target.azim=_RAD_TO_GRD*atan2(x_target-target.x, z_target-target.z) ;
                 target.roll=  0 ;
                 target_pars="XZAR" ;

          EventObject->vCalculateDirect(&target, target_pars) ;

                return(1) ;
              }

                   target.azim=_RAD_TO_GRD*atan2(x1_work-EventObject->state.x, z1_work-EventObject->state.z) ;
                   target_pars="A" ;

          EventObject->vCalculateDirect(&target, target_pars) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                              return(0) ;
                        } 
/*----------------------------------------- Рассчёт радиуса поворота */

    if(limit_type[0]=='G') {
                               rad=v*v/(10.*g) ;
                           }
/*---------------------------------------- Определение целевой точки */

    if(r1>rad) {
                   x_target=x1_work ;
                   z_target=z1_work ;
               }
    else       {
               }
/*----------------------------------- Изменение направления движения */

          angle=_RAD_TO_GRD*atan2(x_target-EventObject->state.x, 
                                  z_target-EventObject->state.z ) ;

                                        da =_RAD_TO_GRD*dt*v/rad ;

                                angle_diff =angle-EventObject->state.azim ;
       while(angle_diff> 180.)  angle_diff-=360. ;
       while(angle_diff<-180.)  angle_diff+=360. ;

       if(fabs(angle_diff)<da) {
                                     da=fabs(angle_diff) ;
                                    rad=dt*v/(_GRD_TO_RAD*da) ;
                                      g=v*v/(10.*rad) ;
                               }

       if(da<5.) {
                   l=dt*v ;
                 }
       else      {
                    l=rad*sin(_GRD_TO_RAD*(da))/sin(_GRD_TO_RAD*(90.-da/2.)) ;
                 } 

       if(angle_diff<0.)  da=-da ;

                            target.x   =EventObject->state.x+l*sin(_GRD_TO_RAD*(EventObject->state.azim+da/2.)) ;
                            target.z   =EventObject->state.z+l*cos(_GRD_TO_RAD*(EventObject->state.azim+da/2.)) ;
                            target.azim=EventObject->state.azim+da ;
                  if(da>0)  target.roll= atan(g)*_RAD_TO_GRD ;
                  else      target.roll=-atan(g)*_RAD_TO_GRD ;
                            target_pars="XZAR" ;

          EventObject->vCalculateDirect(&target, target_pars) ;

/*-------------------------------- Контроль достижения целевой линии */

                          angle =fabs(target.azim-a_target) ;
        while(angle>180)  angle-= 180. ;

          if(angle<1.)  return(1) ;

/*-------------------------------------------------------------------*/

  return(0) ;
}



/*********************************************************************/
/*                                                                   */
/*                  Сохранение статуса объекта                       */

   Dcl_decl *Program_dcl_StateSave(Lang_DCL  *dcl_kernel,
                                   Dcl_decl  *source, 
                                   Dcl_decl **pars, 
                                        int   pars_cnt)

{
                char  slot_name[1024] ;
  Dcl_complex_record *record ;
                 int  type ;          /* Основной тип переменной */
                char  tmp[1024] ;
                char *ptr ;
                char *end ;
                 int  i ;
                 int  k ;

        static  char *slot ;
        static  char *buff ;
#define                _BUFF_SIZE  64000

 static   double  dgt_value ;          /* Буфер числового значения */
 static Dcl_decl  dgt_return={ _DGT_VAL, 0,0,0,"", &dgt_value, NULL, 1, 1} ;

/*---------------------------------------------------- Инициализация */

     if(buff==NULL   )  buff=(char *)calloc(1, _BUFF_SIZE) ;
     if(slot==NULL   )  slot=(char *)calloc(1, _BUFF_SIZE) ;
     if(buff==NULL ||
        slot==NULL   ) {
                         dcl_kernel->mError_code=_DCLE_USER_DEFINED ;
                  strcpy(dcl_kernel->mError_details, "Memory over") ;
                               return(&dgt_return) ;
                       }
   
                              dgt_value=0 ;

/*-------------------------------------------- Извлечение параметров */

           strcpy(slot_name, "Unknown") ;

       if(pars_cnt     == 1   &&                                    /* Проверяем число параметров */
	  pars[0]->addr!=NULL   ) {

                    memset(slot_name, 0, sizeof(slot_name)) ;       /* Извлекаем имя слота данных */
        if(pars[0]->size>=sizeof(slot_name))
                    memcpy(slot_name, pars[0]->addr, sizeof(slot_name)-1) ;
        else        memcpy(slot_name, pars[0]->addr, pars[0]->size) ;

                                  }
/*-------------------------------------------------- Упаковка данных */

          sprintf(slot, "SLOT %s{\n", slot_name) ;

     for(record=(Dcl_complex_record *)source->addr,                 /* LOOP - Перебираем записи */
                        i=0 ; i<source->buff ; i++, 
         record=(Dcl_complex_record *)record->next_record) {

          for(k=0 ; k<record->elems_cnt ; k++) {                    /* LOOP - Перебираем элементы */

               type=t_base(record->elems[k].type) ;                 /* Извлекаем тип элемента */ 
            if(type==_CHR_AREA ||                                   /* Если строчная переменная */
               type==_CHR_PTR    ) {
                                      strcat(slot, (char *)record->elems[k].addr) ;
                                   }
            else                   {                                /* Если числовая переменная */

                       dcl_kernel->iNumToStr(&record->elems[k], tmp) ;
                                      strcat(slot, tmp) ;
                                   }

                                      strcat(slot, ";") ;
 
                                               }                    /* ENDLOOP - Перебираем элементы */

                                      strcat(slot, "\n") ;          /* Переводим строку */ 
                                                           }

          strcat(slot, "}\n") ;

/*------------------------------------------------ Считывание данных */

     if(EventUnit->state!=NULL)  strcpy(buff, EventUnit->state) ;
     else                        strcpy(buff, "") ;

/*---------------------------------------------- Замена слота данных */

         sprintf(tmp, "SLOT %s{\n", slot_name) ;

      ptr=strstr(buff, tmp) ;
   if(ptr!=NULL) {
         end=strchr(ptr, '}') ;
            memmove(ptr, end+1, strlen(end+1)+1) ;
                 }

          strcat(buff, slot) ;

/*------------------------------------------------ Сохранение данных */

           EventUnit->state=(char *)realloc(EventUnit->state, strlen(buff)+8) ;
    strcpy(EventUnit->state, buff) ;

/*-------------------------------------------------------------------*/

#undef   _BUFF_SIZE

  return(&dgt_return) ;
}


/*********************************************************************/
/*                                                                   */
/*                  Считывание статуса объекта                       */

   Dcl_decl *Program_dcl_StateRead(Lang_DCL  *dcl_kernel,
                                   Dcl_decl  *source, 
                                   Dcl_decl **pars, 
                                        int   pars_cnt)

{
#define    _BUFF_SIZE  64000
#define  _VALUES_MAX     100

                char  slot_name[1024] ;
  Dcl_complex_record *record ;
                 int  type ;          /* Основной тип переменной */
                 int  status ;
                char *work ;
                char *next ;
                char *ptr ;
                char *end ;
              double  value ;
                char  tmp[1024] ;
                 int  i ;
                 int  k ;

    static      char *buff ;
    static  Dcl_decl *values ;
              double  digits[_VALUES_MAX] ;

 static   double  dgt_value ;          /* Буфер числового значения */
 static Dcl_decl  dgt_return={ _DGT_VAL, 0,0,0,"", &dgt_value, NULL, 1, 1} ;

/*---------------------------------------------------- Инициализация */

     if(buff==NULL  )  buff=(char *)calloc(1, _BUFF_SIZE) ;
     if(buff==NULL  ) {
                       dcl_kernel->mError_code=_DCLE_USER_DEFINED ;
                strcpy(dcl_kernel->mError_details, "Memory over") ;
                            return(&dgt_return) ;
                      }

     if(values==NULL)  values=(Dcl_decl *)calloc(_VALUES_MAX, sizeof(*values)) ;
     if(values==NULL) {
                       dcl_kernel->mError_code=_DCLE_USER_DEFINED ;
                strcpy(dcl_kernel->mError_details, "Memory over") ;
                            return(&dgt_return) ;
                      }
   
                              dgt_value=0 ;

/*-------------------------------------------- Извлечение параметров */

           strcpy(slot_name, "Unknown") ;

       if(pars_cnt     == 1   &&                                    /* Проверяем число параметров */
	  pars[0]->addr!=NULL   ) {

                    memset(slot_name, 0, sizeof(slot_name)) ;       /* Извлекаем имя слота данных */
        if(pars[0]->size>=sizeof(slot_name))
                    memcpy(slot_name, pars[0]->addr, sizeof(slot_name)-1) ;
        else        memcpy(slot_name, pars[0]->addr, pars[0]->size) ;

                                  }
/*------------------------------------------------ Считывание данных */

     if(EventUnit->state!=NULL)  strcpy(buff, EventUnit->state) ;
     else                        strcpy(buff, "") ;

/*------------------------------------------- Адресация слота данных */

         sprintf(tmp, "SLOT %s{\n", slot_name) ;

      ptr=strstr(buff, tmp) ;
   if(ptr==NULL) {                                                 /* Если данных нет - оставяем исходную запись */
                              dgt_value=1 ;
                      return(&dgt_return) ;
                 }

       ptr+=strlen(tmp) ;
       end =strchr(ptr, '}') ;
    if(end==NULL) {
                              dgt_value=1 ;
                      return(&dgt_return) ;
                  }

       *end= 0 ; 

       memmove(buff, ptr, strlen(ptr)+1) ;

    if(*buff==0) {                                                 /* Если данных нет - оставяем исходную запись */
                              dgt_value=1 ;
                      return(&dgt_return) ;
                 } 
/*------------------------------------------------ Распаковка данных */

              dcl_kernel->iXobject_clear(source) ;                  /* Очищаем структуру состояний */

   for(work=buff, next=buff, i=0 ; ; work=next+1, i++) {            /* LOOP - Перебор строк */

             next=strchr(work, '\n') ;
          if(next==NULL)  break ;
            *next=0 ;

               dcl_kernel->iXobject_add(source, NULL) ;
/*- - - - - - - - - - - - - - - - - - -  Подготовка списка элементов */
     if(i==0) {
                    record=(Dcl_complex_record *)source->addr ;
        for(k=0 ; k<record->elems_cnt ; k++)  values[k]=record->elems[k] ;
              }
/*- - - - - - - - - - - - - - - - - - - Разбор строки на компоненты */
      for(k=0 ; k<_VALUES_MAX ; work=end+1, k++) {

             end=strchr(work, ';') ;
          if(end==NULL)  break ;
            *end=0 ;

               values[k].addr=work ;
                                                 }

     if(k>=_VALUES_MAX) {
                          dcl_kernel->mError_code=_DCLE_USER_DEFINED ;
                   strcpy(dcl_kernel->mError_details, "Too many elements in State") ;
                               return(&dgt_return) ;
                        }
/*- - - - - - - - - - - - - - -  Преобразование числовых компонентов */
      for(k=0 ; k<record->elems_cnt ; k++) {

               type=t_base(record->elems[k].type) ;                 /* Извлекаем тип элемента */ 
            if(type!=_CHR_AREA &&                                   /* Если числовая переменная */
               type!=_CHR_PTR    ) {

                          value=strtod((char *)values[k].addr, &end) ;

                  dcl_kernel->iDgt_set(value, &digits[k], values[k].type) ;
                                       values[k].addr=&digits[k] ;
                                   }
            else                   {
                   values[k].size=(int)strlen((char *)values[k].addr) ;
                                   } 
                                           }                                    
/*- - - - - - - - - - - - - - - - - - - - - - -  Формирование записи */
        status=dcl_kernel->iXobject_set(source, values, record->elems_cnt) ;
     if(status) {
                    dcl_kernel->mError_code=_DCLE_TYPEDEF_ELEM ;
                      return(&dgt_return) ; 
                }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                       }            /* END LOOP - Перебор строк */


    


/*-------------------------------------------------------------------*/

#undef   _BUFF_SIZE

  return(&dgt_return) ;
}


/*********************************************************************/
/*                                                                   */
/*                     Получение списка целей                        */

   Dcl_decl *Program_dcl_GetTargets(Lang_DCL  *dcl_kernel,
                                    Dcl_decl  *source, 
                                    Dcl_decl **pars, 
                                         int   pars_cnt)

{
                char  unit_name[1024] ;
     RSS_Unit_Target *targets ;
                 int  targets_cnt ;
                char  t_name[128] ;
                char  t_spec[128] ;
              double  t_x ;
              double  t_y ;
              double  t_z ;
              double  t_a ;
                 int  status ;
                char  text[1024] ;
                 int  i ;

          Dcl_decl  rec_data[6] ={
                                  {_CHR_AREA, 0, 0, 0, "object",           0,           t_name, 128, 128},
                                  {_CHR_AREA, 0, 0, 0, "special", (void *)128,          t_spec, 128, 128},
                                  {_DGT_VAL,  0, 0, 0, "x",       (void *)256, (char *)&t_x,      1,   1},
                                  {_DGT_VAL,  0, 0, 0, "y",       (void *)264, (char *)&t_y,      1,   1},
                                  {_DGT_VAL,  0, 0, 0, "z",       (void *)272, (char *)&t_z,      1,   1},
                                  {_DGT_VAL,  0, 0, 0, "azim",    (void *)280, (char *)&t_a,      1,   1}
                                 } ;
  Dcl_complex_type  rec_template={ "target", 288, rec_data, 6} ;

 static   double  dgt_value ;          /* Буфер числового значения */
 static Dcl_decl  dgt_return={ _DGT_VAL, 0,0,0,"", &dgt_value, NULL, 1, 1} ;

/*---------------------------------------------------- Инициализация */
   
                              dgt_value=0 ;

/*-------------------------------------------- Извлечение параметров */

       if(pars_cnt     < 1   &&                                    /* Проверяем число параметров */
	  pars[0]->addr==NULL  ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                                 }

                    memset(unit_name, 0, sizeof(unit_name)) ;       /* Извлекаем имя компонента */
        if(pars[0]->size>=sizeof(unit_name))
                    memcpy(unit_name, pars[0]->addr, sizeof(unit_name)-1) ;
        else        memcpy(unit_name, pars[0]->addr, pars[0]->size) ;

/*------------------------------------------- Получение списка целей */

                             targets_cnt=0 ;

       status=Program_emb_GetTargets(unit_name,  &targets, &targets_cnt, text) ;
    if(status) {
                         dcl_kernel->mError_code=_DCLE_USER_DEFINED ;
                  strcpy(dcl_kernel->mError_details, text) ;
                               return(&dgt_return) ;
               }
/*---------------------------------------------- Выдача списка целей */

              dcl_kernel->iXobject_clear(source) ;                  /* Очищаем структуру состояний */

   for(i=0 ; i<targets_cnt ; i++) {                                 /* LOOP - Перебор целей */

              memset(t_name, 0, sizeof(t_name)) ;
             strncpy(t_name, targets[i].target->Name, sizeof(t_name)-1) ;
              memset(t_spec, 0, sizeof(t_spec)) ;
             strncpy(t_spec, targets[i].special, sizeof(t_spec)-1) ;
                     t_x    =targets[i].x ;
                     t_y    =targets[i].y ;
                     t_z    =targets[i].z ;
                     t_a    =  0. ;

        status=dcl_kernel->iXobject_add(source, &rec_template) ;
     if(status) {
                    dcl_kernel->mError_code=_DCLE_TYPEDEF_ELEM ;
                      return(&dgt_return) ; 
                }
                                  }                                 /* END LOOP - Перебор целей */
/*-------------------------------------------- Освобождение ресурсов */
    
    if(targets_cnt)  free(targets) ;

/*-------------------------------------------------------------------*/

  return(&dgt_return) ;
}


   int  Program_emb_GetTargets(char *unit_name,  RSS_Unit_Target **targets, int *targets_cnt, char *error)

{
      RSS_Object *unit ;
             int  i ; 

/*---------------------------------------------------- Инициализация */

             *targets_cnt= 0 ;

/*----------------------------------------- Идентификация компонента */

                    unit=NULL ;

       for(i=0 ; i<EventObject->Units.List_cnt ; i++)
         if(!stricmp(EventObject->Units.List[i].object->Name, unit_name)) { 
                         unit=EventObject->Units.List[i].object ;
                                               break ;
                                                                       }

    if(unit==NULL) {
                      sprintf(error, "Объект '%s' не включает компонент '%s'", EventObject->Name, unit_name) ;
                         return(-1) ;
                   }
/*------------------------------------------- Получение списка целей */

       *targets_cnt=unit->vSpecial("GET_TARGETS", targets) ;
    if(*targets_cnt<0) {
                          sprintf(error, "Компонент '%s' не поддерживает интерфейс обмена целями", unit_name) ;
                             return(-1) ;
                       }
/*------------------------------------------- Получение списка целей */

   return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                  Выделение организованных построений              */

   Dcl_decl *Program_dcl_DetectOrder(Lang_DCL  *dcl_kernel,
                                     Dcl_decl  *source, 
                                     Dcl_decl **pars, 
                                          int   pars_cnt)

{
              double  distance ;
     RSS_Unit_Target *targets ;
                 int  targets_cnt ;
     RSS_Unit_Target *orders ;
                 int  orders_cnt ;
  Dcl_complex_record *record ;
                char  t_name[128] ;
                char  t_spec[128] ;
              double  t_x ;
              double  t_y ;
              double  t_z ;
              double  t_a ;
                 int  status ;
                 int  i ;

          Dcl_decl  rec_data[6] ={
                                  {_CHR_AREA, 0, 0, 0, "object",           0,           t_name, 128, 128},
                                  {_CHR_AREA, 0, 0, 0, "special", (void *)128,          t_spec, 128, 128},
                                  {_DGT_VAL,  0, 0, 0, "x",       (void *)256, (char *)&t_x,      1,   1},
                                  {_DGT_VAL,  0, 0, 0, "y",       (void *)264, (char *)&t_y,      1,   1},
                                  {_DGT_VAL,  0, 0, 0, "z",       (void *)272, (char *)&t_z,      1,   1},
                                  {_DGT_VAL,  0, 0, 0, "azim",    (void *)280, (char *)&t_a,      1,   1}
                                 } ;
  Dcl_complex_type  rec_template={ "order", 288, rec_data, 6} ;

 static   double  dgt_value ;          /* Буфер числового значения */
 static Dcl_decl  dgt_return={ _DGT_VAL, 0,0,0,"", &dgt_value, NULL, 1, 1} ;

/*---------------------------------------------------- Инициализация */

                                targets=NULL ;
                                 orders=NULL ;

                              dgt_value=0 ;

/*-------------------------------------------- Извлечение параметров */

       if(pars_cnt     !=2    ||                                    /* Проверяем число параметров */
	  pars[0]->addr==NULL ||
	  pars[1]->addr==NULL   ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                                  }

          distance=dcl_kernel->iDgt_get(pars[1]->addr, pars[1]->type) ;

       if((pars[0]->type & _DCLT_XTP_OBJ)                           /* Проверяем наличие комплексного объекта */
                         !=_DCLT_XTP_OBJ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                             return(&dgt_return) ; 
                                          }
/*------------------------------------------ Извлечение списка целей */

          targets_cnt=pars[0]->buff ;
          targets    =(RSS_Unit_Target *)calloc(targets_cnt, sizeof(*targets)) ;

           record=(Dcl_complex_record *)pars[0]->addr ;

   for(i=0 ; i<targets_cnt ; i++) {

            targets[i].x    =dcl_kernel->iDgt_get(record->elems[2].addr, record->elems[2].type) ;
            targets[i].y    =dcl_kernel->iDgt_get(record->elems[3].addr, record->elems[3].type) ;
            targets[i].z    =dcl_kernel->iDgt_get(record->elems[4].addr, record->elems[4].type) ;
            targets[i].order= 0 ;

         record=(Dcl_complex_record *)record->next_record ;
                                  }

/*-------------------------------------------------- Анализ формаций */

        Program_emb_DetectOrder( targets,  targets_cnt,
                                &orders,  &orders_cnt,
                                 distance, NULL        ) ;

/*------------------------------------------- Выдача списка формаций */

                 dcl_kernel->iXobject_clear(source) ;               /* Очищаем структуру состояний */

   for(i=0 ; i<orders_cnt ; i++) {                                  /* LOOP - Перебор формаций */
    
                     t_x=orders[i].x ;
                     t_y=orders[i].y ;
                     t_z=orders[i].z ;
                     t_a=orders[i].azim ;

        status=dcl_kernel->iXobject_add(source, &rec_template) ;
     if(status) {
                    dcl_kernel->mError_code=_DCLE_TYPEDEF_ELEM ;
                      return(&dgt_return) ; 
                }
                                 }                                  /* END LOOP - Перебор формаций */
/*-------------------------------------------- Освобождение ресурсов */
    
    if(targets_cnt)  free(targets) ;
    if( orders_cnt)  free( orders) ;

/*-------------------------------------------------------------------*/

#undef   _BUFF_SIZE

  return(&dgt_return) ;
}


   int  Program_emb_DetectOrder(RSS_Unit_Target  *targets,
                                            int   targets_cnt,
                                RSS_Unit_Target **orders,
                                            int  *orders_cnt,
                                         double   distance,
                                           char  *error     )

{
        int  order ;
     double  value ;
     double  value_max ;
        int  i ;
        int  j ;
        int  m1 ;
        int  m2 ;

#define  ORDERS       (*orders)
#define  ORDERS_CNT   (*orders_cnt)

/*---------------------------------------------------- Инициализация */

                        ORDERS    =NULL ;
                        ORDERS_CNT=  0 ;

/*----------------------------------------------- Выделение формаций */

#define  DISTANCE(i, j)  sqrt((targets[i].x-targets[j].x)*(targets[i].x-targets[j].x)+  \
                              (targets[i].y-targets[j].y)*(targets[i].y-targets[j].y)+  \
                              (targets[i].z-targets[j].z)*(targets[i].z-targets[j].z) )

   for(i=1 ; i<targets_cnt ; i++) {

     for(j=0 ; j<i ; j++) 
       if(DISTANCE(i,j)<=distance)  break ;

       if(j>=i)  continue ;
        
       if(targets[j].order==0) {
                                                     ORDERS_CNT++ ;
                                    targets[j].order=ORDERS_CNT ;
                               }
  
                                    targets[i].order=targets[j].order ;
                                  }

#undef  DISTANCE

/*-------------------------------------------------- Анализ формаций */

  if(ORDERS_CNT) {
/*- - - - - - - - - - - - - - - - - - - - - - - - - Выделение памяти */
          ORDERS_CNT++ ;

          ORDERS=(RSS_Unit_Target *)calloc(ORDERS_CNT, sizeof(*ORDERS)) ;
/*- - - - - - - - - - - - - - - - Определение "центра масс" формации */
     for(i=0 ; i<targets_cnt ; i++)
       if(targets[i].order) {
                              ORDERS[targets[i].order].x    +=targets[i].x ; 
                              ORDERS[targets[i].order].y    +=targets[i].y ; 
                              ORDERS[targets[i].order].z    +=targets[i].z ; 
                              ORDERS[targets[i].order].order++ ;
                            }
    
     for(order=1 ; order<ORDERS_CNT ; order++) {
                             ORDERS[order].x/=ORDERS[order].order ; 
                             ORDERS[order].y/=ORDERS[order].order ; 
                             ORDERS[order].z/=ORDERS[order].order ; 
                                               }
/*- - - - - - - - - - - - - - - - - - - -  Выделение групповых целей */
     for(i=0, order=1 ; order<ORDERS_CNT ; order++) {

          if(ORDERS[order].order<3)  continue ;                     /* Группы менее 3 целей не рассматриваем */

          if(i!=order)  ORDERS[i]      =ORDERS[order] ;
                        ORDERS[i].order=       order ;
                               i++ ;
                                                    }

                        ORDERS_CNT=i ;
/*- - - - - - - - - - - - - - - - Определение "главной оси" формации */
     for(order=0 ; order<ORDERS_CNT ; order++) {

         value_max=0. ;
                m1=0 ;

        for(i=0 ; i<targets_cnt ; i++)                              /* Ищем самую отдалённую от центра точку */
          if(targets[i].order==ORDERS[order].order) {
               value=(targets[i].x-ORDERS[order].x)*(targets[i].x-ORDERS[order].x)
                    +(targets[i].z-ORDERS[order].z)*(targets[i].z-ORDERS[order].z) ;
            if(value>value_max) {  value_max=value ; 
                                          m1= i ;     } 
                                                    }

         value_max=0. ;
                m2=0 ;

        for(i=0 ; i<targets_cnt ; i++)                              /* Ищем самую отдалённую от центра точку */
          if(targets[i].order==ORDERS[order].order) {
               value=(targets[i].x-targets[m1].x)*(targets[i].x-targets[m1].x)
                    +(targets[i].z-targets[m1].z)*(targets[i].z-targets[m1].z) ;
            if(value>value_max) {  value_max=value ; 
                                          m2= i ;     } 
                                                    }

            ORDERS[order].azim=_RAD_TO_GRD*atan2(targets[m2].x-targets[m1].x, 
                                                 targets[m2].z-targets[m1].z ) ;
                                                }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                 }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*             Рассчёт абсолютных координат по относительным         */

   Dcl_decl *Program_dcl_GetGlobalXYZ(Lang_DCL  *dcl_kernel,
                                      Dcl_decl  *source, 
                                      Dcl_decl **pars, 
                                           int   pars_cnt)

{
   RSS_Unit_Target  rel ; 
   RSS_Unit_Target  abs ;
               int  status ;

          Dcl_decl  rec_data[3] ={
                                  {_DGT_VAL,  0, 0, 0, "x", (void *) 0, (char *)&abs.x, 1, 1},
                                  {_DGT_VAL,  0, 0, 0, "y", (void *) 8, (char *)&abs.y, 1, 1},
                                  {_DGT_VAL,  0, 0, 0, "z", (void *)16, (char *)&abs.z, 1, 1}
                                 } ;
  Dcl_complex_type  rec_template={ "xyz", 24, rec_data, 3} ;

 static   double  dgt_value ;          /* Буфер числового значения */
 static Dcl_decl  dgt_return={ _DGT_VAL, 0,0,0,"", &dgt_value, NULL, 1, 1} ;

/*-------------------------------------------- Извлечение параметров */

       if(pars_cnt     !=3    ||                                    /* Проверяем число параметров */
	  pars[0]->addr==NULL ||
	  pars[1]->addr==NULL ||
	  pars[2]->addr==NULL   ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                                  }

        rel.x=dcl_kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;
        rel.y=dcl_kernel->iDgt_get(pars[1]->addr, pars[1]->type) ;
        rel.z=dcl_kernel->iDgt_get(pars[2]->addr, pars[2]->type) ;

/*------------------------------------------------ Рассчёт координат */

            Program_emb_GetGlobalXYZ(&rel, &abs) ;

/*------------------------------------------------ Выдача результата */

               dcl_kernel->iXobject_clear(source) ;                 /* Очищаем структуру состояний */

        status=dcl_kernel->iXobject_add(source, &rec_template) ;
     if(status) {
                    dcl_kernel->mError_code=_DCLE_TYPEDEF_ELEM ;
                      return(&dgt_return) ; 
                }
/*-------------------------------------------------------------------*/

#undef   _BUFF_SIZE

  return(&dgt_return) ;
}

    int  Program_emb_GetGlobalXYZ(RSS_Unit_Target *rel, 
                                  RSS_Unit_Target *abs )
{
    Matrix2d  Sum_Matrix ;
    Matrix2d  Oper_Matrix ;  
    Matrix2d  Vect_Matrix ;  


       Vect_Matrix.LoadZero   (3, 1) ;
       Vect_Matrix.SetCell    (0, 0, rel->x) ;
       Vect_Matrix.SetCell    (1, 0, rel->y) ;
       Vect_Matrix.SetCell    (2, 0, rel->z) ;
        Sum_Matrix.Load3d_azim(-EventObject->state.azim) ;
       Oper_Matrix.Load3d_elev(-EventObject->state.elev) ;
        Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
       Vect_Matrix.LoadMul    (&Sum_Matrix, &Vect_Matrix) ;

         abs->x=Vect_Matrix.GetCell(0, 0)+EventObject->state.x ;
         abs->y=Vect_Matrix.GetCell(1, 0)+EventObject->state.y ;
         abs->z=Vect_Matrix.GetCell(2, 0)+EventObject->state.z ;

  return(0) ;
}


/********************************************************************/
/*								    */
/*                   Выполнение команды Battle                      */

  Dcl_decl *Program_dcl_Battle(Lang_DCL  *dcl_kernel,
                               Dcl_decl **pars, 
                                    int   pars_cnt)
{
   char  text[1024] ;

 static     char  chr_value[512] ;          /* Буфер строки */
 static Dcl_decl  chr_return={ _CHR_PTR, 0,0,0,"", chr_value, NULL, 0, 512} ;

#define  _LOG_MAX   100

/*---------------------------------------------------- Инициализация */

                 memset(chr_value, 0, sizeof(chr_value)) ;
                        chr_return.size=0 ;

/*-------------------------------------------- Извлечение параметров */

       if(pars_cnt     !=1   ||                                     /* Проверяем число параметров */
	  pars[0]->addr==NULL  ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&chr_return) ; 
                                 }

                    memset(text, 0, sizeof(text)) ;                 /* Извлекаем ссылку на файл */
        if(pars[0]->size>=sizeof(text))
                    memcpy(text, pars[0]->addr, sizeof(text)-1) ;
        else        memcpy(text, pars[0]->addr, pars[0]->size) ;

/*----------------------------------- Передача команды на исполнение */

             strcat(EventCallback, text) ;
             strcat(EventCallback, ";") ;

/*-------------------------------------------------------------------*/

  return(&chr_return) ;
}



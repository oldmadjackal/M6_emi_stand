
#ifndef  RSS_UNIT_H 

#define  RSS_UNIT_H 

#include "..\RSS_Object\RSS_Object.h"

#include <string>

#ifdef RSS_UNIT_EXPORTS
#define RSS_UNIT_API __declspec(dllexport)
#else
#define RSS_UNIT_API __declspec(dllimport)
#endif

/*-------------------------------------- Описание класса "Компонент" */

  class RSS_UNIT_API RSS_Unit : public RSS_Object {

       public:

           class RSS_Object  *Owner ;          /* Объект, которому данный принадлежит как составная часть */

       public:
			     RSS_Unit        () ;                   /* Конструктор */
			    ~RSS_Unit        () ;                   /* Деструктор */

                                  } ;
/*------------------------------------ Описание элементов управления */

   struct RSS_Unit_Engine_Control {
                                      double  thrust ;    /* Уровень тяги, 0...1 */
                                      double  azim  ;     /* Азимутальный угол вектора тяги, градусов */
                                      double  elev  ;     /* Угол возвышения вектора тяги, градусов */
                                  } ;

   struct RSS_Unit_Engine_Thrust {
                                      double  x ;         /* Компоненты вектора тяги по осям */
                                      double  y ;
                                      double  z ;
                                 } ;

   struct RSS_Unit_Aero_Control {
                                      double  impact ;    /* Отклонения руля, -1...1 */
                                } ;

/*----------------------------------- Описание класса "Компонент-БЧ" */

  class RSS_UNIT_API RSS_Unit_WarHead : public RSS_Unit {

       public:
                virtual int  vSetWarHeadControl(char *) ;           /* Управление БЧ */

       public:
			     RSS_Unit_WarHead  () ;                 /* Конструктор */
			    ~RSS_Unit_WarHead  () ;                 /* Деструктор */
                                                        } ;
/*---------------------------------- Описание класса "Компонент-ГСН" */

  class RSS_UNIT_API RSS_Unit_Homing : public RSS_Unit {

       public:
                virtual int  vSetHomingControl     (char *) ;           /* Управление ГСН */

                virtual int  vGetHomingDirection   (RSS_Point *) ;      /* Направление на цель */
                virtual int  vGetHomingPosition    (RSS_Point *) ;      /* Относительное положение цели */
                virtual int  vGetHomingDistance    (double *) ;         /* Дистанция до цели */
                virtual int  vGetHomingClosingSpeed(double *) ;         /* Скорость сближения с целью */

       public:
			     RSS_Unit_Homing() ;                   /* Конструктор */
			    ~RSS_Unit_Homing() ;                   /* Деструктор */
                                                        } ;
/*---------------------------------- Описание класса "Компонент-СУ" */

  class RSS_UNIT_API RSS_Unit_Control : public RSS_Unit {

       public:
                virtual int  vSetHomingDirection   (RSS_Point *) ;                      /* Направление на цель */
                virtual int  vSetHomingPosition    (RSS_Point *) ;                      /* Относительное положение цели */
                virtual int  vSetHomingDistance    (double) ;                           /* Дистанция до цели */
                virtual int  vSetHomingClosingSpeed(double) ;                           /* Скорость сближения с целью */

                virtual int  vGetWarHeadControl    (char *) ;                           /* Управление БЧ */
                virtual int  vGetHomingControl     (char *) ;                           /* Управление ГСН */
                virtual int  vGetEngineControl     (RSS_Unit_Engine_Control *) ;        /* Управление двигателем */
                virtual int  vGetAeroControl       (RSS_Unit_Aero_Control *) ;          /* Управление аэродинамическими поверхностями */

       public:
			     RSS_Unit_Control  () ;                /* Конструктор */
			    ~RSS_Unit_Control  () ;                /* Деструктор */
                                                        } ;
/*---------------------------- Описание класса "Компонент-Двигатель" */

  class RSS_UNIT_API RSS_Unit_Engine : public RSS_Unit {

       public:
                virtual int  vSetEngineControl     (RSS_Unit_Engine_Control *, int) ;   /* Управление двигателем */

                virtual int  vGetEngineThrust      (RSS_Unit_Engine_Thrust *) ;         /* Вектор тяги двигателя */
                virtual int  vGetEngineMass        (double *, RSS_Point *) ;            /* Масса и положение центра масс двигателя */
                virtual int  vGetEngineMI          (double *, double *, double *) ;     /* Моменты инерции двигателя */

       public:
			     RSS_Unit_Engine  () ;                /* Конструктор */
			    ~RSS_Unit_Engine  () ;                /* Деструктор */
                                                        } ;
/*------------------------------- Описание класса "Компонент-Модель" */

  class RSS_UNIT_API RSS_Unit_Model : public RSS_Unit {

       public:
                virtual int  vSetAeroControl       (RSS_Unit_Aero_Control *, int) ;     /* Управление аэродинамическими поверхностями */
                virtual int  vSetEngineThrust      (RSS_Unit_Engine_Thrust *, int) ;    /* Вектор тяги двигателя */
                virtual int  vSetEngineMass        (double, RSS_Point *) ;              /* Масса и положение центра масс двигателя */
                virtual int  vSetEngineMI          (double, double, double) ;           /* Моменты инерции двигателя */

       public:
			     RSS_Unit_Model  () ;                /* Конструктор */
			    ~RSS_Unit_Model  () ;                /* Деструктор */
                                                        } ;
/*-------------------------------------------------------------------*/

#endif  // RSS_UNIT_H


#ifndef  RSS_UNIT_H 

#define  RSS_UNIT_H 

#include "..\RSS_Object\RSS_Object.h"

#include <string>

#ifdef RSS_UNIT_EXPORTS
#define RSS_UNIT_API __declspec(dllexport)
#else
#define RSS_UNIT_API __declspec(dllimport)
#endif

/*-------------------------------------- �������� ������ "���������" */

  class RSS_UNIT_API RSS_Unit : public RSS_Object {

       public:

           class RSS_Object  *Owner ;          /* ������, �������� ������ ����������� ��� ��������� ����� */

       public:
			     RSS_Unit        () ;                   /* ����������� */
			    ~RSS_Unit        () ;                   /* ���������� */

                                  } ;
/*------------------------------------ �������� ��������� ���������� */

   struct RSS_Unit_Engine_Control {
                                      double  thrust ;    /* ������� ����, 0...1 */
                                      double  azim  ;     /* ������������ ���� ������� ����, �������� */
                                      double  elev  ;     /* ���� ���������� ������� ����, �������� */
                                  } ;

   struct RSS_Unit_Engine_Thrust {
                                      double  x ;         /* ���������� ������� ���� �� ���� */
                                      double  y ;
                                      double  z ;
                                 } ;

   struct RSS_Unit_Aero_Control {
                                      double  impact ;    /* ���������� ����, -1...1 */
                                } ;

/*----------------------------------- �������� ������ "���������-��" */

  class RSS_UNIT_API RSS_Unit_WarHead : public RSS_Unit {

       public:
                virtual int  vSetWarHeadControl(char *) ;           /* ���������� �� */

       public:
			     RSS_Unit_WarHead  () ;                 /* ����������� */
			    ~RSS_Unit_WarHead  () ;                 /* ���������� */
                                                        } ;
/*---------------------------------- �������� ������ "���������-���" */

  class RSS_UNIT_API RSS_Unit_Homing : public RSS_Unit {

       public:
                virtual int  vSetHomingControl     (char *) ;           /* ���������� ��� */

                virtual int  vGetHomingDirection   (RSS_Point *) ;      /* ����������� �� ���� */
                virtual int  vGetHomingPosition    (RSS_Point *) ;      /* ������������� ��������� ���� */
                virtual int  vGetHomingDistance    (double *) ;         /* ��������� �� ���� */
                virtual int  vGetHomingClosingSpeed(double *) ;         /* �������� ��������� � ����� */

       public:
			     RSS_Unit_Homing() ;                   /* ����������� */
			    ~RSS_Unit_Homing() ;                   /* ���������� */
                                                        } ;
/*---------------------------------- �������� ������ "���������-��" */

  class RSS_UNIT_API RSS_Unit_Control : public RSS_Unit {

       public:
                virtual int  vSetHomingDirection   (RSS_Point *) ;                      /* ����������� �� ���� */
                virtual int  vSetHomingPosition    (RSS_Point *) ;                      /* ������������� ��������� ���� */
                virtual int  vSetHomingDistance    (double) ;                           /* ��������� �� ���� */
                virtual int  vSetHomingClosingSpeed(double) ;                           /* �������� ��������� � ����� */

                virtual int  vGetWarHeadControl    (char *) ;                           /* ���������� �� */
                virtual int  vGetHomingControl     (char *) ;                           /* ���������� ��� */
                virtual int  vGetEngineControl     (RSS_Unit_Engine_Control *) ;        /* ���������� ���������� */
                virtual int  vGetAeroControl       (RSS_Unit_Aero_Control *) ;          /* ���������� ����������������� ������������� */

       public:
			     RSS_Unit_Control  () ;                /* ����������� */
			    ~RSS_Unit_Control  () ;                /* ���������� */
                                                        } ;
/*---------------------------- �������� ������ "���������-���������" */

  class RSS_UNIT_API RSS_Unit_Engine : public RSS_Unit {

       public:
                virtual int  vSetEngineControl     (RSS_Unit_Engine_Control *, int) ;   /* ���������� ���������� */

                virtual int  vGetEngineThrust      (RSS_Unit_Engine_Thrust *) ;         /* ������ ���� ��������� */
                virtual int  vGetEngineMass        (double *, RSS_Point *) ;            /* ����� � ��������� ������ ���� ��������� */
                virtual int  vGetEngineMI          (double *, double *, double *) ;     /* ������� ������� ��������� */

       public:
			     RSS_Unit_Engine  () ;                /* ����������� */
			    ~RSS_Unit_Engine  () ;                /* ���������� */
                                                        } ;
/*------------------------------- �������� ������ "���������-������" */

  class RSS_UNIT_API RSS_Unit_Model : public RSS_Unit {

       public:
                virtual int  vSetAeroControl       (RSS_Unit_Aero_Control *, int) ;     /* ���������� ����������������� ������������� */
                virtual int  vSetEngineThrust      (RSS_Unit_Engine_Thrust *, int) ;    /* ������ ���� ��������� */
                virtual int  vSetEngineMass        (double, RSS_Point *) ;              /* ����� � ��������� ������ ���� ��������� */
                virtual int  vSetEngineMI          (double, double, double) ;           /* ������� ������� ��������� */

       public:
			     RSS_Unit_Model  () ;                /* ����������� */
			    ~RSS_Unit_Model  () ;                /* ���������� */
                                                        } ;
/*----------------------------- �������� ��� ����������� ����������� */

   struct RSS_Unit_Target {
                             RSS_Object *target ;
                                 double  x ;
                                 double  y ;
                                 double  z ;
                                   char  special[128] ;
                                    int  order ;
                          } ;
/*-------------------------------------------------------------------*/

#endif  // RSS_UNIT_H

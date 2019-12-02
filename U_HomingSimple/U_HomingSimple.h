
/********************************************************************/
/*								    */
/*              ������ ���������� ����������� "������� ���"         */
/*								    */
/********************************************************************/

#ifdef U_HOMING_SIMPLE_EXPORTS
#define U_HOMING_SIMPLE_API __declspec(dllexport)
#else
#define U_HOMING_SIMPLE_API __declspec(dllimport)
#endif

/*---------------------------- �������� ������ ������� "������� ���" */

  class U_HOMING_SIMPLE_API RSS_Unit_HomingSimple : public RSS_Unit_Homing {

    public:

#define _UNITS_BY_STAGE_MAX  4

                       int  stage ;
           RSS_Unit_Homing *units_1[_UNITS_BY_STAGE_MAX] ;
                       int  switch_1 ;
           RSS_Unit_Homing *units_2[_UNITS_BY_STAGE_MAX] ;

                       time_t  start_time ;                      /* ����� ������� */
//                     double  x, y, z ;                         /* ��������� ������������������ ���������� */

//                     int  blast ;                           /* ���� ��������� ������������ */  

    public:
         virtual       void  vFree                 (void) ;                             /* ���������� ������� */
         virtual RSS_Object *vCopy                 (char *) ;                           /* ���������� ������ */
         virtual        int  vCalculateStart       (double) ;                           /* ���������� ������� ��������� ��������� */
         virtual        int  vCalculate            (double, double, char *, int) ;      /* ������ ��������� ��������� */
                                                     
         virtual        int  vCalculateShow        (double, double) ;                   /* ����������� ���������� ������� ��������� ��������� */
         virtual        int  vSpecial              (char *, void *) ;                   /* ����������� �������� */

         virtual        int  vSetHomingControl     (char *) ;                           /* ���������� ��� */

         virtual        int  vGetHomingDirection   (RSS_Point *) ;                      /* ����������� �� ���� */
         virtual        int  vGetHomingPosition    (RSS_Point *) ;                      /* ������������� ��������� ���� */
         virtual        int  vGetHomingDistance    (double *) ;                         /* ��������� �� ���� */
         virtual        int  vGetHomingClosingSpeed(double *) ;                         /* �������� ��������� � ����� */

	                     RSS_Unit_HomingSimple() ;                /* ����������� */
	                    ~RSS_Unit_HomingSimple() ;                /* ���������� */
                                                        } ;

/*---------------- �������� ������ ���������� �������� "������� ���" */

  class U_HOMING_SIMPLE_API RSS_Module_HomingSimple : public RSS_Kernel {

    public:

     static
      struct RSS_Module_HomingSimple_instr *mInstrList ;          /* ������ ������ */

    public:
     virtual  RSS_Object *vCreateObject (RSS_Model_data *) ;                 /* �������� ������� */ 
     virtual         int  vGetParameter (char *, char *) ;                   /* �������� �������� */
     virtual         int  vExecuteCmd   (const char *) ;                     /* ��������� ������� */

    public:
                     int  cHelp         (char *) ;                           /* ���������� HELP */ 
                     int  cInfo         (char *) ;                           /* ���������� INFO */ 
                     int  cConfig       (char *) ;                           /* ���������� CONFIG */ 

                RSS_Unit *FindUnit      (char *) ;                           /* ����� ���������� �� ����� */

    public:
	                  RSS_Module_HomingSimple() ;              /* ����������� */
	                 ~RSS_Module_HomingSimple() ;              /* ���������� */
                                                       } ;

/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_HomingSimple_instr {

           char                           *name_full ;          /* ������ ��� ������� */
           char                           *name_shrt ;          /* �������� ��� ������� */
           char                           *help_row ;           /* HELP - ������ */
           char                           *help_full ;          /* HELP - ������ */
            int (RSS_Module_HomingSimple::*process)(char *) ;   /* ��������� ���������� ������� */
                                      }  ;

/*--------------------------------------------- ���������� ��������� */

/* ����  U_HomingSimple.cpp */

/* ����  U_HomingSimple_dialog.cpp */
  INT_PTR CALLBACK  Unit_HomingSimple_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Unit_HomingSimple_Config_dialog(HWND, UINT, WPARAM, LPARAM) ;


/********************************************************************/
/*								    */
/*          ������ ���������� ����������� "������������ ���"        */
/*								    */
/********************************************************************/

#ifdef U_HOMING_POINT_EXPORTS
#define U_HOMING_POINT_API __declspec(dllexport)
#else
#define U_HOMING_POINT_API __declspec(dllimport)
#endif

/*----------------------- �������� ������ ������� "������������ ���" */

  class U_HOMING_POINT_API RSS_Unit_HomingPoint : public RSS_Unit_Homing {

    public:

                          int  method ;              /* ��� ������� ��������� */
#define                         _INERTIAL_METHOD  1   /* ������������ ������� ��������� */

                       time_t  start_time ;          /* ����� ������� */
                       double  x_0, y_0, z_0 ;       /* ��������� ���������� */
                       double  x_t, y_t, z_t ;       /* ������� ���������� */

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

	                     RSS_Unit_HomingPoint() ;                /* ����������� */
	                    ~RSS_Unit_HomingPoint() ;                /* ���������� */
                                                        } ;

/*----------- �������� ������ ���������� �������� "������������ ���" */

  class U_HOMING_POINT_API RSS_Module_HomingPoint : public RSS_Kernel {

    public:

     static
      struct RSS_Module_HomingPoint_instr *mInstrList ;          /* ������ ������ */

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
	                  RSS_Module_HomingPoint() ;              /* ����������� */
	                 ~RSS_Module_HomingPoint() ;              /* ���������� */
                                                       } ;

/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_HomingPoint_instr {

           char                          *name_full ;          /* ������ ��� ������� */
           char                          *name_shrt ;          /* �������� ��� ������� */
           char                          *help_row ;           /* HELP - ������ */
           char                          *help_full ;          /* HELP - ������ */
            int (RSS_Module_HomingPoint::*process)(char *) ;   /* ��������� ���������� ������� */
                                     }  ;

/*--------------------------------------------- ���������� ��������� */

/* ����  U_HomingPoint.cpp */

/* ����  U_HomingPoint_dialog.cpp */
  INT_PTR CALLBACK  Unit_HomingPoint_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Unit_HomingPoint_Config_dialog(HWND, UINT, WPARAM, LPARAM) ;

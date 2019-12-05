
/********************************************************************/
/*								    */
/*        ������ ���������� ����������� "2-� ������� ���������"     */
/*								    */
/********************************************************************/

#ifdef U_CONTROL_2STAGE_EXPORTS
#define U_CONTROL_2STAGE_API __declspec(dllexport)
#else
#define U_CONTROL_2STAGE_API __declspec(dllimport)
#endif

/*------------------ �������� ������ ������� "2-� ������� ���������" */

  class U_CONTROL_2STAGE_API RSS_Unit_Control2Stage : public RSS_Unit_Control {

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

	                     RSS_Unit_Control2Stage() ;                /* ����������� */
	                    ~RSS_Unit_Control2Stage() ;                /* ���������� */
                                                        } ;

/*---------------- �������� ������ ���������� �������� "������� ���" */

  class U_CONTROL_2STAGE_API RSS_Module_Control2Stage : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Control2Stage_instr *mInstrList ;          /* ������ ������ */

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
	                  RSS_Module_Control2Stage() ;              /* ����������� */
	                 ~RSS_Module_Control2Stage() ;              /* ���������� */
                                                       } ;

/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_Control2Stage_instr {

           char                            *name_full ;          /* ������ ��� ������� */
           char                            *name_shrt ;          /* �������� ��� ������� */
           char                            *help_row ;           /* HELP - ������ */
           char                            *help_full ;          /* HELP - ������ */
            int (RSS_Module_Control2Stage::*process)(char *) ;   /* ��������� ���������� ������� */
                                       } ;

/*--------------------------------------------- ���������� ��������� */

/* ����  U_Control2Stage.cpp */

/* ����  U_Control2Stage_dialog.cpp */
  INT_PTR CALLBACK  Unit_Control2Stage_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Unit_Control2Stage_Config_dialog(HWND, UINT, WPARAM, LPARAM) ;

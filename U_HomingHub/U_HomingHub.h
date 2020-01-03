
/********************************************************************/
/*								    */
/*         ������ ���������� ����������� "��������������� ���"      */
/*								    */
/********************************************************************/

#ifdef U_HOMING_HUB_EXPORTS
#define U_HOMING_HUB_API __declspec(dllexport)
#else
#define U_HOMING_HUB_API __declspec(dllimport)
#endif

/*-------------------- �������� ������ ������� "��������������� ���" */

  class U_HOMING_HUB_API RSS_Unit_HomingHub : public RSS_Unit_Homing {

    public:

#define _UNITS_BY_STAGE_MAX  4

                       int  stage ;
           RSS_Unit_Homing *units_1[_UNITS_BY_STAGE_MAX] ;
           RSS_Unit_Homing *units_2[_UNITS_BY_STAGE_MAX] ;

                    time_t  start_time ;                      /* ����� ������� */

    public:
         virtual       void  vFree                 (void) ;                             /* ���������� ������� */
         virtual   RSS_Unit *vCopy                 (RSS_Object *) ;                     /* ���������� ��������� */
         virtual        int  vCalculateStart       (double) ;                           /* ���������� ������� ��������� ��������� */
         virtual        int  vCalculate            (double, double, char *, int) ;      /* ������ ��������� ��������� */
                                                     
         virtual        int  vCalculateShow        (double, double) ;                   /* ����������� ���������� ������� ��������� ��������� */
         virtual        int  vSpecial              (char *, void *) ;                   /* ����������� �������� */

         virtual        int  vSetHomingControl     (char *) ;                           /* ���������� ��� */

         virtual        int  vGetHomingDirection   (RSS_Point *) ;                      /* ����������� �� ���� */
         virtual        int  vGetHomingPosition    (RSS_Point *) ;                      /* ������������� ��������� ���� */
         virtual        int  vGetHomingDistance    (double *) ;                         /* ��������� �� ���� */
         virtual        int  vGetHomingClosingSpeed(double *) ;                         /* �������� ��������� � ����� */

	                     RSS_Unit_HomingHub() ;                /* ����������� */
	                    ~RSS_Unit_HomingHub() ;                /* ���������� */
                                                        } ;

/*-------- �������� ������ ���������� �������� "��������������� ���" */

  class U_HOMING_HUB_API RSS_Module_HomingHub : public RSS_Kernel {

    public:

     static
      struct RSS_Module_HomingHub_instr *mInstrList ;          /* ������ ������ */

    public:
     virtual  RSS_Object *vCreateObject (RSS_Model_data *) ;                 /* �������� ������� */ 
     virtual         int  vGetParameter (char *, char *) ;                   /* �������� �������� */
     virtual         int  vExecuteCmd   (const char *) ;                     /* ��������� ������� */

    public:
                     int  cHelp         (char *) ;                           /* ���������� HELP */ 
                     int  cInfo         (char *) ;                           /* ���������� INFO */ 
                     int  cConfig       (char *) ;                           /* ���������� CONFIG */ 
                     int  cPlugin       (char *) ;                           /* ���������� PLUGIN */ 

                RSS_Unit *FindUnit      (char *) ;                           /* ����� ���������� �� ����� */
   class RSS_Unit_Homing *AddUnit       (RSS_Object *,                       /* ���������� ���������� � ������� */
                                          char *, char *, char *) ;

    public:
	                  RSS_Module_HomingHub() ;              /* ����������� */
	                 ~RSS_Module_HomingHub() ;              /* ���������� */
                                                       } ;

/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_HomingHub_instr {

           char                        *name_full ;          /* ������ ��� ������� */
           char                        *name_shrt ;          /* �������� ��� ������� */
           char                        *help_row ;           /* HELP - ������ */
           char                        *help_full ;          /* HELP - ������ */
            int (RSS_Module_HomingHub::*process)(char *) ;   /* ��������� ���������� ������� */
                                   }  ;

/*--------------------------------------------- ���������� ��������� */

/* ����  U_HomingHub.cpp */

/* ����  U_HomingHub_dialog.cpp */
  INT_PTR CALLBACK  Unit_HomingHub_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Unit_HomingHub_Config_dialog(HWND, UINT, WPARAM, LPARAM) ;

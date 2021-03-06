
/********************************************************************/
/*								    */
/*              ������ ���������� ����������� "������ ��"           */
/*								    */
/********************************************************************/

#ifdef U_MODEL_GUIDED_EXPORTS
#define U_MODEL_GUIDED_API __declspec(dllexport)
#else
#define U_MODEL_GUIDED_API __declspec(dllimport)
#endif

/*------------------------------ �������� ������ ������� "������ ��" */

  class U_MODEL_GUIDED_API RSS_Unit_ModelGuided : public RSS_Unit_Model {

    public:

                            double  mass ;                    /* ������ ����� ��� ����� �� */
                            double  s_azim ;                  /* ���������� ���������� �� ����������� */
                            double  s_elev ;                  /* ���������� ���������� �� ���� ������ */
                            double  start_time ;              /* ����� ������� ���������� �������  */
                            double  s_middle ;                /* ����������� ������� */

                              char  log_path[FILENAME_MAX] ;  /* ���� ���� ���������� */

                            double  t_0 ;                     /* ����� ������� */

                            double  engine_thrust ;           /* ���� ���� */
                            double  engine_mass ;             /* ����� �� */
                        RSS_Vector  control_vector ;          /* ��������� ���������� ������� */

    public:
         virtual       void  vFree             (void) ;                             /* ���������� ������� */
         virtual   RSS_Unit *vCopy             (RSS_Object *) ;                     /* ���������� ��������� */
         virtual        int  vCalculateStart   (double) ;                           /* ���������� ������� ��������� ��������� */
         virtual        int  vCalculate        (double, double, char *, int) ;      /* ������ ��������� ��������� */
                                                     
         virtual        int  vCalculateShow    (double, double) ;                   /* ����������� ���������� ������� ��������� ��������� */
         virtual        int  vSpecial          (char *, void *) ;                   /* ����������� �������� */

         virtual        int  vSetAeroControl   (RSS_Unit_Aero_Control *, int) ;     /* ���������� ����������������� ������������� */
         virtual        int  vSetVectorControl (RSS_Vector *) ;                     /* ��������� ���������� ������� */
         virtual        int  vSetEngineThrust  (RSS_Unit_Engine_Thrust *, int) ;    /* ������ ���� ��������� */
         virtual        int  vSetEngineMass    (double, RSS_Point *) ;              /* ����� � ��������� ������ ���� ��������� */
         virtual        int  vSetEngineMI      (double, double, double) ;           /* ������� ������� ��������� */

                       void  Log               (const char *) ;                     /* ������ ���� ���������� */

	                     RSS_Unit_ModelGuided() ;                /* ����������� */
	                    ~RSS_Unit_ModelGuided() ;                /* ���������� */
                                                        } ;

/*------------------ �������� ������ ���������� �������� "������ ��" */

  class U_MODEL_GUIDED_API RSS_Module_ModelGuided : public RSS_Kernel {

    public:

     static
      struct RSS_Module_ModelGuided_instr *mInstrList ;          /* ������ ������ */

    public:
     virtual  RSS_Object *vCreateObject (RSS_Model_data *) ;                 /* �������� ������� */ 
     virtual         int  vGetParameter (char *, char *) ;                   /* �������� �������� */
     virtual         int  vExecuteCmd   (const char *) ;                     /* ��������� ������� */

    public:
                     int  cHelp         (char *) ;                           /* ���������� HELP */ 
                     int  cInfo         (char *) ;                           /* ���������� INFO */ 
                     int  cPars         (char *) ;                           /* ���������� PARS */ 
                     int  cMass         (char *) ;                           /* ���������� MASS */ 
                     int  cMiddle       (char *) ;                           /* ���������� MIDDLE */ 
                     int  cStartTime    (char *) ;                           /* ���������� STARTTIME */ 
                     int  cDeviation    (char *) ;                           /* ���������� DEVIATION */ 
                     int  cLog          (char *) ;                           /* ���������� LOG */ 

                RSS_Unit *FindUnit      (char *) ;                           /* ����� ���������� �� ����� */

    public:
	                  RSS_Module_ModelGuided() ;              /* ����������� */
	                 ~RSS_Module_ModelGuided() ;              /* ���������� */
                                                       } ;

/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_ModelGuided_instr {

           char                          *name_full ;          /* ������ ��� ������� */
           char                          *name_shrt ;          /* �������� ��� ������� */
           char                          *help_row ;           /* HELP - ������ */
           char                          *help_full ;          /* HELP - ������ */
            int (RSS_Module_ModelGuided::*process)(char *) ;   /* ��������� ���������� ������� */
                                 }  ;

/*--------------------------------------------- ���������� ��������� */

/* ����  U_ModelGuided.cpp */

/* ����  U_ModelGuided_dialog.cpp */
  INT_PTR CALLBACK  Unit_ModelGuided_Help_dialog(HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Unit_ModelGuided_Pars_dialog(HWND, UINT, WPARAM, LPARAM) ;

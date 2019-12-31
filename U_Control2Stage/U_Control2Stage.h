
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

/*------------------------------------- �������� ��������� ��������� */

#define  _STAGE_ACTIONS_MAX   20
#define         _STAGES_MAX   20

typedef  struct {
                  char  operation[128] ;
                   int  if_condition ;
                   int  exit_condition ;
                   int  once ;
                   int  done ;
                }  StageAction ;

typedef  struct {
                       char  name[32] ;
                StageAction  actions[_STAGE_ACTIONS_MAX] ;
                        int  actions_cnt ;               
                }  Stage ;

/*------------------ �������� ������ ������� "2-� ������� ���������" */

  class U_CONTROL_2STAGE_API RSS_Unit_Control2Stage : public RSS_Unit_Control {

    public:
                    Stage  stage_start ;
                    Stage  stages[_STAGES_MAX] ;
                      int  stages_cnt ;
                      int  stage ;

                   double  vector_control_max ;              /* ������������ �������� ���������� ����������, �/�*� */

                   time_t  start_time ;                      /* ����� ������� */
                     char  warhead_control[1024] ;           /* ������� ���������� �� */
                     char  homing_control[1024] ;            /* ������� ���������� �������� ��������� */
               RSS_Vector  vector_control ;                  /* ��������� ���������� ���������� */

                   double  t_distance ;                      /* ��������� �� ���� */
                RSS_Point  t_xyz ;                           /* ��������� ���� */
                RSS_Point  t_xyz_prv ;                       /* ��������� ���� */

    public:
         virtual       void  vFree                 (void) ;                             /* ���������� ������� */
         virtual RSS_Object *vCopy                 (char *) ;                           /* ���������� ������ */
         virtual        int  vCalculateStart       (double) ;                           /* ���������� ������� ��������� ��������� */
         virtual        int  vCalculate            (double, double, char *, int) ;      /* ������ ��������� ��������� */
                                                     
         virtual        int  vCalculateShow        (double, double) ;                   /* ����������� ���������� ������� ��������� ��������� */
         virtual        int  vSpecial              (char *, void *) ;                   /* ����������� �������� */

         virtual        int  vSetHomingDirection   (RSS_Point *) ;                      /* ����������� �� ���� */
         virtual        int  vSetHomingPosition    (RSS_Point *) ;                      /* ������������� ��������� ���� */
         virtual        int  vSetHomingDistance    (double) ;                           /* ��������� �� ���� */
         virtual        int  vSetHomingClosingSpeed(double) ;                           /* �������� ��������� � ����� */

         virtual        int  vGetWarHeadControl    (char *) ;                           /* ���������� �� */
         virtual        int  vGetHomingControl     (char *) ;                           /* ���������� ��� */
         virtual        int  vGetEngineControl     (RSS_Unit_Engine_Control *) ;        /* ���������� ���������� */
         virtual        int  vGetAeroControl       (RSS_Unit_Aero_Control *) ;          /* ���������� ����������������� ������������� */
         virtual        int  vGetVectorControl     (RSS_Vector *) ;                     /* ��������� ���������� ������� */

                        int  ExecuteOperation      (char *, char *) ;                   /* ���������� �������� */
                        int  ExecuteExpression     (char *, char *, char *) ;           /* ���������� ���������  */

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
                     int  cProgram      (char *) ;                           /* ���������� PROGRAM */ 

                RSS_Unit *FindUnit      (char *) ;                           /* ����� ���������� �� ����� */
                     int  ReadProgram   (RSS_Unit_Control2Stage *,           /* ���������� ��������� ���������� */
                                         char *, char *) ;
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

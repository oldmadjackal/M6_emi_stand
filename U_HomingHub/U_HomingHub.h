
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
                     double  x, y, z ;                         /* ��������� ������������������ ���������� */
                     time_t  start_time ;                      /* ����� ������� */

//                     int  blast ;                           /* ���� ��������� ������������ */  

    public:
         virtual       void  vFree             (void) ;                             /* ���������� ������� */
         virtual RSS_Object *vCopy             (char *) ;                           /* ���������� ������ */
         virtual        int  vCalculateStart   (double) ;                           /* ���������� ������� ��������� ��������� */
         virtual        int  vCalculate        (double, double, char *, int) ;      /* ������ ��������� ��������� */
                                                     
         virtual        int  vCalculateShow    (double, double) ;                   /* ����������� ���������� ������� ��������� ��������� */
         virtual        int  vSpecial          (char *, void *) ;                   /* ����������� �������� */

         virtual        int  vSetWarHeadControl(char *) ;                           /* ���������� �� */

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
                     int  cPars         (char *) ;                           /* ���������� PARS */ 

                RSS_Unit *FindUnit      (char *) ;                           /* ����� ���������� �� ����� */

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
  INT_PTR CALLBACK  Unit_HomingHub_Help_dialog(HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Unit_HomingHub_Pars_dialog(HWND, UINT, WPARAM, LPARAM) ;

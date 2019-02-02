
/********************************************************************/
/*								    */
/*    ������ ���������� ����������� "������� �������� ���������"    */
/*								    */
/********************************************************************/

#ifdef U_ENGINE_SIMPLE_EXPORTS
#define U_ENGINE_SIMPLE_API __declspec(dllexport)
#else
#define U_ENGINE_SIMPLE_API __declspec(dllimport)
#endif

/*--------------------------------------- �������� ������� ������ �� */

 struct RSS_Unit_EngineSimple_Profile {
                                          double  t ;       /* ��������� ����� */
                                          double  thrust ;  /* ���� ���� */
                                          double  mass ;    /* ����� �� */
                                          double  Cx ;      /* ��������� ������ ���� �� */
                                          double  Cy ;
                                          double  Cz ;
                                          double  Mx ;      /* ������� ������� �� ������������ ���� */
                                          double  My ;
                                          double  Mz ;
                                      } ;

/*----------------------------- �������� ������ ������� "������� ��" */

  class U_ENGINE_SIMPLE_API RSS_Unit_EngineSimple : public RSS_Unit_Engine {

    public:
                RSS_Unit_EngineSimple_Profile *profile ;          /* ������� ������ ��������� */
                                          int  profile_cnt ;
                                          int  use_mass ;         /* ���� ������� � ������� ������ �� ����� */
                                          int  use_Cxyz ;         /* ���� ������� � ������� ������ �� ������ ���� */
                                          int  use_Mxyz ;         /* ���� ������� � ������� ������ �� ������� ������ */

                RSS_Unit_EngineSimple_Profile  values ;           /* ������� �������� */

                                       double  sigma2 ;           /* ������������� ������-������������ ���������� ���������� */
                                       double  real_value ;       /* ������������� �������� */

                                       double  t_0 ;              /* ����� ������� */

                                         HWND  hWnd ;             /* ���� ������� ������� */

    public:
         virtual       void  vFree            (void) ;                             /* ���������� ������� */
         virtual RSS_Object *vCopy            (char *) ;                           /* ���������� ������ */
         virtual        int  vCalculateStart  (double) ;                           /* ���������� ������� ��������� ��������� */
         virtual        int  vCalculate       (double, double, char *, int) ;      /* ������ ��������� ��������� */
                                                     
         virtual        int  vCalculateShow   (double, double) ;                   /* ����������� ���������� ������� ��������� ��������� */
         virtual        int  vSpecial         (char *, void *) ;                   /* ����������� �������� */

         virtual        int  vSetEngineControl(RSS_Unit_Engine_Control *, int) ;   /* ���������� ���������� */
         virtual        int  vGetEngineThrust (RSS_Unit_Engine_Thrust *) ;         /* ������ ���� ��������� */
         virtual        int  vGetEngineMass   (double *, RSS_Point *) ;            /* ����� � ��������� ������ ���� ��������� */
         virtual        int  vGetEngineMI     (double *, double *, double *) ;     /* ������� ������� ��������� */

	                     RSS_Unit_EngineSimple() ;                /* ����������� */
	                    ~RSS_Unit_EngineSimple() ;                /* ���������� */
                                                        } ;

/*----------------- �������� ������ ���������� �������� "������� ��" */

  class U_ENGINE_SIMPLE_API RSS_Module_EngineSimple : public RSS_Kernel {

    public:

     static
      struct RSS_Module_EngineSimple_instr *mInstrList ;          /* ������ ������ */

    public:
     virtual  RSS_Object *vCreateObject (RSS_Model_data *) ;                 /* �������� ������� */ 
     virtual         int  vGetParameter (char *, char *) ;                   /* �������� �������� */
     virtual         int  vExecuteCmd   (const char *) ;                     /* ��������� ������� */

    public:
                     int  cHelp         (char *) ;                           /* ���������� HELP */ 
                     int  cInfo         (char *) ;                           /* ���������� INFO */ 
                     int  cSigma        (char *) ;                           /* ���������� SIGMA */
                     int  cProfile      (char *) ;                           /* ���������� PROFILE */
                     int  cView         (char *) ;                           /* ���������� VIEW */

                RSS_Unit *FindUnit      (char *) ;                           /* ����� ���������� �� ����� */
                     int  ReadProfile   (RSS_Unit_EngineSimple *, char *) ;  /* ���������� ����� �������� ������� ������ */

    public:
	                  RSS_Module_EngineSimple() ;              /* ����������� */
	                 ~RSS_Module_EngineSimple() ;              /* ���������� */
                                                       } ;

/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_EngineSimple_instr {

           char                           *name_full ;          /* ������ ��� ������� */
           char                           *name_shrt ;          /* �������� ��� ������� */
           char                           *help_row ;           /* HELP - ������ */
           char                           *help_full ;          /* HELP - ������ */
            int (RSS_Module_EngineSimple::*process)(char *) ;   /* ��������� ���������� ������� */
                                 }  ;

/*--------------------------------------------- ���������� ��������� */

/* ����  U_EngineSimple.cpp */

/* ����  U_EngineSimple_dialog.cpp */
  INT_PTR CALLBACK  Unit_EngineSimple_Help_dialog(HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Unit_EngineSimple_View_dialog(HWND, UINT, WPARAM, LPARAM) ;

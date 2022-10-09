
/********************************************************************/
/*								    */
/*          ������ ���������� ����������� "��������� ������"        */
/*								    */
/********************************************************************/

#ifdef U_MODEL_TABLE_EXPORTS
#define U_MODEL_TABLE_API __declspec(dllexport)
#else
#define U_MODEL_TABLE_API __declspec(dllimport)
#endif

/*------------------------- �������� ������ ������� "������� ������" */

  class U_MODEL_TABLE_API RSS_Unit_ModelTable : public RSS_Unit_Model {

    public:
                            double  t_0 ;                 /* ����� ������� */

                              char  path[FILENAME_MAX] ;    /* ���� � ����� � �������� */
                              char  format[FILENAME_MAX] ;  /* ������ ������ ������� */

                              FILE *file ;                  /* ������ �� ���� ������� */
                              char  dgt_sep ;               /* ���������� ����������� */
                               int  start_row ;             /* ������ ������ ������ */
                            double  t_start ;               /* ��������� ����� ������ */
                               int  t_idx ;                 /* ������� ����� ������ */
                               int  x_idx ;
                               int  y_idx ;
                               int  z_idx ;
                               int  v_idx ;
                               int  vx_idx ;
                               int  vy_idx ;
                               int  vz_idx ;
                               int  azim_idx ;
                               int  elev_idx ;
                               int  roll_idx ;

                            double  pars_1[20] ;            /* ������� "�����" ������ */
                            double  pars_2[20] ;

    public:
         virtual       void  vFree             (void) ;                             /* ���������� ������� */
         virtual RSS_Object *vCopy             (char *) ;                           /* ���������� ������ */
         virtual        int  vCalculateStart   (double) ;                           /* ���������� ������� ��������� ��������� */
         virtual        int  vCalculate        (double, double, char *, int) ;      /* ������ ��������� ��������� */
                                                     
         virtual        int  vCalculateShow    (double, double) ;                   /* ����������� ���������� ������� ��������� ��������� */
         virtual        int  vSpecial          (char *, void *) ;                   /* ����������� �������� */

         virtual        int  vSetAeroControl   (RSS_Unit_Aero_Control *, int) ;     /* ���������� ����������������� ������������� */
         virtual        int  vSetEngineThrust  (RSS_Unit_Engine_Thrust *, int) ;    /* ������ ���� ��������� */
         virtual        int  vSetEngineMass    (double, RSS_Point *) ;              /* ����� � ��������� ������ ���� ��������� */
         virtual        int  vSetEngineMI      (double, double, double) ;           /* ������� ������� ��������� */

                        int  iFormatDecode     (char *) ;                           /* ������ ������� ������ */
                        int  iParsDissect      (char *, double *, int) ;            /* ������ ������ ������ */

	                     RSS_Unit_ModelTable() ;                /* ����������� */
	                    ~RSS_Unit_ModelTable() ;                /* ���������� */
                                                        } ;

/*------------- �������� ������ ���������� �������� "������� ������" */

  class U_MODEL_TABLE_API RSS_Module_ModelTable : public RSS_Kernel {

    public:

     static
      struct RSS_Module_ModelTable_instr *mInstrList ;          /* ������ ������ */

    public:
     virtual  RSS_Object *vCreateObject (RSS_Model_data *) ;                 /* �������� ������� */ 
     virtual         int  vGetParameter (char *, char *) ;                   /* �������� �������� */
     virtual         int  vExecuteCmd   (const char *) ;                     /* ��������� ������� */

    public:
                     int  cHelp         (char *) ;                           /* ���������� HELP */ 
                     int  cInfo         (char *) ;                           /* ���������� INFO */ 
                     int  cPars         (char *) ;                           /* ���������� PARS */ 
                     int  cPath         (char *) ;                           /* ���������� PATH */ 
                     int  cFormat       (char *) ;                           /* ���������� FORMAT */ 

                RSS_Unit *FindUnit      (char *) ;                           /* ����� ���������� �� ����� */

    public:
	                  RSS_Module_ModelTable() ;              /* ����������� */
	                 ~RSS_Module_ModelTable() ;              /* ���������� */
                                                       } ;

/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_ModelTable_instr {

           char                          *name_full ;          /* ������ ��� ������� */
           char                          *name_shrt ;          /* �������� ��� ������� */
           char                          *help_row ;           /* HELP - ������ */
           char                          *help_full ;          /* HELP - ������ */
            int (RSS_Module_ModelTable::*process)(char *) ;   /* ��������� ���������� ������� */
                                    }  ;

/*--------------------------------------------- ���������� ��������� */

/* ����  U_ModelTable.cpp */

/* ����  U_ModelTable_dialog.cpp */
  INT_PTR CALLBACK  Unit_ModelTable_Help_dialog(HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Unit_ModelTable_Pars_dialog(HWND, UINT, WPARAM, LPARAM) ;

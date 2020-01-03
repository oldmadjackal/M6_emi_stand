
/********************************************************************/
/*								    */
/*              ������ ���������� ����������� "������� ��"          */
/*								    */
/********************************************************************/

#ifdef U_WARHEAD_SIMPLE_EXPORTS
#define U_WARHEAD_SIMPLE_API __declspec(dllexport)
#else
#define U_WARHEAD_SIMPLE_API __declspec(dllimport)
#endif

/*----------------------------- �������� ������ ������� "������� ��" */

  class U_WARHEAD_SIMPLE_API RSS_Unit_WarHeadSimple : public RSS_Unit_WarHead {

    public:
                     double  x, y, z ;                         /* ��������� ������������������ ���������� */
                        int  blast ;                           /* ���� ��������� ������������ */  

                        int  tripping_type ;                   /* ��� ������������ ������������ ��������� */
#define                        _BY_ALTITUDE   0
#define                        _BY_TIME       1
                     double  tripping_altitude ;               /* ������ ������������ */
                     double  tripping_time ;                   /* ����� ������������ */
                     time_t  start_time ;                      /* ����� ������� */

                        int  load_type ;                       /* ��� ���������� */
#define                        _GRENADE_TYPE   0
#define                         _STRIPE_TYPE   1
#define                       _PANCAKES_TYPE   2
                     double  hit_range ;                       /* ������ ��������� */
                     double  blast_radius ;                    /* ������ ������� ������ */
                       char  sub_unit[128] ;                   /* �������� ������� ���-���������� */
                 RSS_Object *sub_object ;                      /* ������ ���-���������� */
                        int  sub_count ;                       /* ���������� ���-����������� */
                     double  sub_step ;                        /* ��� ������� ���-����������� */
                        int  sub_series ;                      /* ���������� ����� ������� ���-����������� */
                        int  sub_range ;                       /* �������� �������� ����� */

    public:
         virtual       void  vFree             (void) ;                             /* ���������� ������� */
         virtual   RSS_Unit *vCopy             (RSS_Object *) ;                     /* ���������� ��������� */
         virtual        int  vCalculateStart   (double) ;                           /* ���������� ������� ��������� ��������� */
         virtual        int  vCalculate        (double, double, char *, int) ;      /* ������ ��������� ��������� */
                                                     
         virtual        int  vCalculateShow    (double, double) ;                   /* ����������� ���������� ������� ��������� ��������� */
         virtual        int  vSpecial          (char *, void *) ;                   /* ����������� �������� */

         virtual        int  vSetWarHeadControl(char *) ;                           /* ���������� �� */

	                     RSS_Unit_WarHeadSimple() ;                /* ����������� */
	                    ~RSS_Unit_WarHeadSimple() ;                /* ���������� */
                                                        } ;

/*----------------- �������� ������ ���������� �������� "������� ��" */

  class U_WARHEAD_SIMPLE_API RSS_Module_WarHeadSimple : public RSS_Kernel {

    public:

     static
      struct RSS_Module_WarHeadSimple_instr *mInstrList ;          /* ������ ������ */

    public:
     virtual  RSS_Object *vCreateObject (RSS_Model_data *) ;                 /* �������� ������� */ 
     virtual         int  vGetParameter (char *, char *) ;                   /* �������� �������� */
     virtual         int  vExecuteCmd   (const char *) ;                     /* ��������� ������� */

    public:
                     int  cHelp         (char *) ;                           /* ���������� HELP */ 
                     int  cInfo         (char *) ;                           /* ���������� INFO */ 
                     int  cPars         (char *) ;                           /* ���������� PARS */ 
                     int  cTripping     (char *) ;                           /* ���������� TRIPPING */ 
                     int  cGrenade      (char *) ;                           /* ���������� GRENADE */ 
                     int  cStripe       (char *) ;                           /* ���������� STRIPE */ 
                     int  cPancakes     (char *) ;                           /* ���������� PANCAKES */ 

                RSS_Unit *FindUnit      (char *) ;                           /* ����� ���������� �� ����� */

    public:
	                  RSS_Module_WarHeadSimple() ;              /* ����������� */
	                 ~RSS_Module_WarHeadSimple() ;              /* ���������� */
                                                       } ;

/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_WarHeadSimple_instr {

           char                            *name_full ;          /* ������ ��� ������� */
           char                            *name_shrt ;          /* �������� ��� ������� */
           char                            *help_row ;           /* HELP - ������ */
           char                            *help_full ;          /* HELP - ������ */
            int (RSS_Module_WarHeadSimple::*process)(char *) ;   /* ��������� ���������� ������� */
                                 }  ;

/*--------------------------------------------- ���������� ��������� */

/* ����  U_WarHeadSimple.cpp */

/* ����  U_WarHeadSimple_dialog.cpp */
  INT_PTR CALLBACK  Unit_WarHeadSimple_Help_dialog(HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Unit_WarHeadSimple_Pars_dialog(HWND, UINT, WPARAM, LPARAM) ;


/********************************************************************/
/*								    */
/*		������ ���������� ����������� "��������� ���"       */
/*								    */
/********************************************************************/

#ifdef U_SEARCHRADAR_EXPORTS
#define U_SEARCHRADAR_API __declspec(dllexport)
#else
#define U_SEARCHRADAR_API __declspec(dllimport)
#endif

#include "..\T_Battle\T_Battle.h"

/*---------------------------------------------- ��������� ��������� */

#define  _SEARCHRADAR_THREATS_MAX  50

/*---------------- �������� ������ ��������� ������� "��������� ���" */

  class U_SEARCHRADAR_API RSS_Transit_SearchRadar : public RSS_Transit {

    public:
             virtual   int  vExecute(void) ;             /* ���������� �������� */
                                             
    public:
                            RSS_Transit_SearchRadar() ;  /* ����������� */
                           ~RSS_Transit_SearchRadar() ;  /* ���������� */

                                                               } ;

/*---------------------------- �������� ������ ������� "������� ���" */

  class U_SEARCHRADAR_API RSS_Unit_SearchRadar : public RSS_Unit {

    public:
                    double  range_min ;                            /* �������� ���������� ����������� */
                    double  range_max ;
                    double  velocity ;                             /* ����������� �������� ��������� */

                      char  event_name[128] ;                      /* ��� ������������ ������� */
                    double  event_time ;                           /* ����� ������� */
                       int  event_send ;                           /* ���� �������� ������� */

         RSS_Module_Battle *battle ;                               /* ������ �� BATTLE-������ */

                RSS_Object *threats[_SEARCHRADAR_THREATS_MAX] ;    /* ������ ����� */
                       int  threats_cnt ;

                      HWND  hWnd ;                                 /* ���� ���������� */ 

    public:
               virtual void  vFree          (void) ;            /* ���������� ������� */
               virtual void  vWriteSave     (std::string *) ;   /* �������� ������ � ������ */
               virtual  int  vCalculateStart(double) ;          /* ���������� ������� ��������� ��������� */
               virtual  int  vCalculate     (double, double,    /* ������ ��������� ��������� */
                                                     char *, int) ;
               virtual  int  vCalculateShow (double, double) ;  /* ����������� ���������� ������� ��������� ��������� */
               virtual  int  vSpecial       (char *, void *) ;  /* ����������� �������� */

	                     RSS_Unit_SearchRadar() ;           /* ����������� */
	                    ~RSS_Unit_SearchRadar() ;           /* ���������� */
                                                        } ;

/*---------------- �������� ������ ���������� �������� "������� ���" */

  class U_SEARCHRADAR_API RSS_Module_SearchRadar : public RSS_Kernel {

    public:

     static
      struct RSS_Module_SearchRadar_instr *mInstrList ;      /* ������ ������ */

    public:
     virtual  RSS_Object *vCreateObject (RSS_Model_data *) ; /* �������� ������� */ 
     virtual         int  vGetParameter (char *, char *) ;   /* �������� �������� */
     virtual         int  vExecuteCmd   (const char *) ;     /* ��������� ������� */
     virtual        void  vReadSave     (std::string *) ;    /* ������ ������ �� ������ */
     virtual        void  vWriteSave    (std::string *) ;    /* �������� ������ � ������ */

    public:
                     int  cHelp         (char *) ;                     /* ���������� HELP */ 
                     int  cInfo         (char *) ;                     /* ���������� INFO */ 
                     int  cRange        (char *) ;                     /* ���������� RANGE */
                     int  cVelocity     (char *) ;                     /* ���������� VELOCITY */
                     int  cEvent        (char *) ;                     /* ���������� EVENT */ 
                     int  cShow         (char *) ;                     /* ���������� SHOW */ 

                RSS_Unit *FindUnit      (char *) ;                     /* ����� ���������� �� ����� */

    public:
	                  RSS_Module_SearchRadar() ;              /* ����������� */
	                 ~RSS_Module_SearchRadar() ;              /* ���������� */
                                                       } ;

/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_SearchRadar_instr {

           char                          *name_full ;          /* ������ ��� ������� */
           char                          *name_shrt ;          /* �������� ��� ������� */
           char                          *help_row ;           /* HELP - ������ */
           char                          *help_full ;          /* HELP - ������ */
            int (RSS_Module_SearchRadar::*process)(char *) ;   /* ��������� ���������� ������� */
                                     }  ;

/*--------------------------------------------- ���������� ��������� */

/* ����  U_SearchRadar.cpp */

/* ����  U_SearchRadar_dialog.cpp */
    BOOL CALLBACK  Unit_SearchRadar_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Unit_SearchRadar_Show_dialog  (HWND, UINT, WPARAM, LPARAM) ;
 LRESULT CALLBACK  Unit_SearchRadar_Indicator_prc(HWND, UINT, WPARAM, LPARAM) ;

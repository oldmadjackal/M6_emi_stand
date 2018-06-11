
/********************************************************************/
/*								    */
/*	������ ���������� ����������� "����������� ����������"      */
/*								    */
/********************************************************************/

#ifdef U_PROGRAM_EXPORTS
#define U_PROGRAM_API __declspec(dllexport)
#else
#define U_PROGRAM_API __declspec(dllimport)
#endif

#include "..\T_Battle\T_Battle.h"

/*---------------------------------------------- ��������� ��������� */

/*---------------------------------- ���������� ��������� ���������� */

typedef  struct {
                   char  program_name[128] ;
                } RSS_Unit_Program_Embeded ;

#define  _ROUTE_MAX  100
#define  _DROPS_MAX   10
   
typedef  struct {
                   char  program_name[128] ;

       struct {
                 double  x ;
                 double  z ;
              }          route[_ROUTE_MAX] ;
                    int  route_cnt ;
                    int  route_idx ;
                    int  route_dir ;

                 double  g ;
                 double  e_max ;
                 double  e_rate ;
                 double  h_min ;
                 double  column_spacing ;
                 double  jump_distance ;
                 double  drop_height ;
                 double  drop_distance ;
                    int  drops_number ;
                 double  drops_interval ;
       struct {
                   char  weapon[64] ;
              }          drops[_DROPS_MAX] ;
                    int  drops_cnt ;

                   char  stage_0_event[1024] ;
                   char  stage_4_event[1024] ;                  
                 double  stage_4_after ;
                   char  stage_1_radar[1024] ;
                 double  stage_2_time ;
                   char  stage_4_radar[1024] ;

                 double  stage ;
                 double  x_direct ;
                 double  z_direct ;
                 double  a_direct ;
                 double  e_direct ;
                 double  check_time ;
                    int  dropped ;
                    int  stage_4_event_done ;

                } RSS_Unit_Program_Embeded_ColumnHunter ;

/*---- �������� ������ ��������� ���������� "����������� ����������" */

  class U_PROGRAM_API RSS_Transit_Program : public RSS_Transit {

    public:
             virtual   int  vExecute(void) ;             /* ���������� �������� */
                                             
    public:
                            RSS_Transit_Program() ;  /* ����������� */
                           ~RSS_Transit_Program() ;  /* ���������� */

                                                               } ;

/*-------------- �������� ������ ���������� "����������� ����������" */

  class U_PROGRAM_API RSS_Unit_Program : public RSS_Unit {

    public:
                      char *program ;                              /* ��������� ���������� */
                      void *embeded ;                              /* ����������� ��������� ���������� */                    
                      char *state ;                                /* �������� ��������� ��������� */

                      char  event_name[128] ;                      /* ��� ������������ ������� */
                    double  event_time ;                           /* ����� ������� */
                       int  event_send ;                           /* ���� �������� ������� */

         RSS_Module_Battle *battle ;                               /* ������ �� BATTLE-������ */

                      HWND  hWnd ;                                 /* ���� ���������� */ 

    public:
               virtual void  vFree          (void) ;            /* ���������� ������� */
               virtual void  vWriteSave     (std::string *) ;   /* �������� ������ � ������ */
               virtual  int  vCalculateStart(double) ;          /* ���������� ������� ��������� ��������� */
               virtual  int  vCalculate     (double, double,    /* ������ ��������� ��������� */
                                                     char *, int) ;
               virtual  int  vCalculateShow (double, double) ;  /* ����������� ���������� ������� ��������� ��������� */
               virtual  int  vSpecial       (char *, void *) ;  /* ����������� �������� */

                        int  EmbededColumnHunter(double, double, char *, int) ;

	                     RSS_Unit_Program() ;           /* ����������� */
	                    ~RSS_Unit_Program() ;           /* ���������� */
                                                        } ;

/*-- �������� ������ ���������� ����������� "����������� ����������" */

  class U_PROGRAM_API RSS_Module_Program : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Program_instr *mInstrList ;      /* ������ ������ */

    public:
     virtual  RSS_Object *vCreateObject (RSS_Model_data *) ; /* �������� ������� */ 
     virtual         int  vGetParameter (char *, char *) ;   /* �������� �������� */
     virtual         int  vExecuteCmd   (const char *) ;     /* ��������� ������� */
     virtual        void  vReadSave     (std::string *) ;    /* ������ ������ �� ������ */
     virtual        void  vWriteSave    (std::string *) ;    /* �������� ������ � ������ */

    public:
                     int  cHelp         (char *) ;                     /* ���������� HELP */ 
                     int  cInfo         (char *) ;                     /* ���������� INFO */ 
                     int  cProgram      (char *) ;                     /* ���������� PROGRAM */ 
                     int  cEmbeded      (char *) ;                     /* ���������� EMBEDED */ 
                     int  cShow         (char *) ;                     /* ���������� SHOW */ 

                RSS_Unit *FindUnit      (char *) ;                     /* ����� ���������� �� ����� */
                    void *ReadEmbeded   (char *, char *) ;             /* ���������� �������� ���������� ��������� */

    public:
	                  RSS_Module_Program() ;              /* ����������� */
	                 ~RSS_Module_Program() ;              /* ���������� */
                                                       } ;

/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_Program_instr {

           char                          *name_full ;          /* ������ ��� ������� */
           char                          *name_shrt ;          /* �������� ��� ������� */
           char                          *help_row ;           /* HELP - ������ */
           char                          *help_full ;          /* HELP - ������ */
            int (RSS_Module_Program::*process)(char *) ;   /* ��������� ���������� ������� */
                                     }  ;

/*--------------------------------------------- ���������� ��������� */

/* ����  U_Program.cpp */

/* ����  U_Program_dialog.cpp */
    BOOL CALLBACK  Unit_Program_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Unit_Program_Show_dialog  (HWND, UINT, WPARAM, LPARAM) ;
 LRESULT CALLBACK  Unit_Program_Indicator_prc(HWND, UINT, WPARAM, LPARAM) ;

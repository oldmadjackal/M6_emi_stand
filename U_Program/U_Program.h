
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
               virtual void  vFree              (void) ;                         /* ���������� ������� */
               virtual void  vWriteSave         (std::string *) ;                /* �������� ������ � ������ */
               virtual  int  vCalculateStart    (double) ;                       /* ���������� ������� ��������� ��������� */
               virtual  int  vCalculate         (double, double,                 /* ������ ��������� ��������� */
                                                         char *, int) ;
               virtual  int  vCalculateShow     (double, double) ;               /* ����������� ���������� ������� ��������� ��������� */
               virtual  int  vSpecial           (char *, void *) ;               /* ����������� �������� */

                        int  StartColumnHunter  (double  t) ;
                        int  StartLoneHunter    (double  t) ;
                        int  StartInterceptFAD  (double  t) ;

                        int  EmbededColumnHunter(double, double, char *, int) ;
                        int  EmbededLoneHunter  (double, double, char *, int) ;
                        int  EmbededInterceptFAD(double, double, char *, int) ;

	                     RSS_Unit_Program() ;           /* ����������� */
	                    ~RSS_Unit_Program() ;           /* ���������� */
                                                        } ;

/*-- �������� ������ ���������� ����������� "����������� ����������" */

  class U_PROGRAM_API RSS_Module_Program : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Program_instr *mInstrList ;      /* ������ ������ */

    public:
     virtual  RSS_Object *vCreateObject   (RSS_Model_data *) ; /* �������� ������� */ 
     virtual         int  vGetParameter   (char *, char *) ;   /* �������� �������� */
     virtual         int  vExecuteCmd     (const char *) ;     /* ��������� ������� */
     virtual        void  vReadSave       (std::string *) ;    /* ������ ������ �� ������ */
     virtual        void  vWriteSave      (std::string *) ;    /* �������� ������ � ������ */

    public:
                     int  cHelp           (char *) ;                     /* ���������� HELP */ 
                     int  cInfo           (char *) ;                     /* ���������� INFO */ 
                     int  cProgram        (char *) ;                     /* ���������� PROGRAM */ 
                     int  cEmbeded        (char *) ;                     /* ���������� EMBEDED */ 
                     int  cShow           (char *) ;                     /* ���������� SHOW */ 

                RSS_Unit *FindUnit        (char *) ;                     /* ����� ���������� �� ����� */
                    void *ReadEmbeded     (char *, char *) ;             /* ���������� �������� ���������� ��������� */
                    void *ReadColumnHunter(char *, char *) ;
                    void *ReadLoneHunter  (char *, char *) ;
                    void *ReadInterceptFAD(char *, char *) ;

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

/*---------------------------------- ���������� ������������ ������� */

#ifdef  __U_PROGRAM_MAIN__
#define   _EXTERNAL_LOCAL   
#else
#define   _EXTERNAL_LOCAL   extern
#endif

     _EXTERNAL_LOCAL  RSS_Unit_Program *EventUnit ;
     _EXTERNAL_LOCAL        RSS_Object *EventObject ;
     _EXTERNAL_LOCAL              char *EventCallback ;
     _EXTERNAL_LOCAL               int  EventCallback_size ;

/*--------------------------------------------- ���������� ��������� */

/* ����  U_Program_Library.cpp */
         int  Program_emb_Log         (char *text) ;
         int  Program_emb_Turn        (char *angle, double  a_target, double  dt, char *limit_type, double  g) ;
         int  Program_emb_ToPoint     (double  x_target, double  z_target, double  dt, char *limit_type, double  g) ;
         int  Program_emb_ToLine      (double  x_target, double  z_target, 
                                       double  a_target, double  dt, char *limit_type, double  g) ;
         int  Program_emb_GetTargets  (char *unit_name,  RSS_Unit_Target **targets, int *targets_cnt, char *error) ;
         int  Program_emb_DetectOrder (RSS_Unit_Target  *targets, int  targets_cnt,
                                       RSS_Unit_Target **orders,  int *orders_cnt,
                                       double  spacing,  char *error) ;
         int  Program_emb_GetGlobalXYZ(RSS_Unit_Target *rel, RSS_Unit_Target *abs) ;
      double  Program_emb_Distance    (double  x, double  z) ;

#include "..\DCL_kernel\dcl.h"

    Dcl_decl *Program_dcl_Message     (Lang_DCL *,             Dcl_decl **, int) ;    /* ������ ��������� �� ����� � ��������� */
    Dcl_decl *Program_dcl_Log         (Lang_DCL *,             Dcl_decl **, int) ;    /* ������ � ��� */
    Dcl_decl *Program_dcl_StateSave   (Lang_DCL *, Dcl_decl *, Dcl_decl **, int) ;    /* ���������� ��������� ������� */
    Dcl_decl *Program_dcl_StateRead   (Lang_DCL *, Dcl_decl *, Dcl_decl **, int) ;    /* ���������� ��������� ������� */
    Dcl_decl *Program_dcl_Turn        (Lang_DCL *,             Dcl_decl **, int) ;    /* ��������� ����������� �������� */
    Dcl_decl *Program_dcl_Distance    (Lang_DCL *,             Dcl_decl **, int) ;    /* ����������� ��������� */
    Dcl_decl *Program_dcl_ToPoint     (Lang_DCL *,             Dcl_decl **, int) ;    /* �������� � ����� */
    Dcl_decl *Program_dcl_ToLine      (Lang_DCL *,             Dcl_decl **, int) ;    /* ����� �� ����� */
    Dcl_decl *Program_dcl_GetTargets  (Lang_DCL *, Dcl_decl *, Dcl_decl **, int) ;    /* ��������� ������ ����������� ����� */
    Dcl_decl *Program_dcl_DetectOrder (Lang_DCL *, Dcl_decl *, Dcl_decl **, int) ;    /* ��������� �������������� ���������� */
    Dcl_decl *Program_dcl_GetGlobalXYZ(Lang_DCL *, Dcl_decl *, Dcl_decl **, int) ;    /* ������� ���������� ��������� �� ������������� */
    Dcl_decl *Program_dcl_Battle      (Lang_DCL *,             Dcl_decl **, int) ;    /* ������� Battle */

/* ����  U_Program_dialog.cpp */
  INT_PTR CALLBACK  Unit_Program_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Unit_Program_Show_dialog  (HWND, UINT, WPARAM, LPARAM) ;
  LRESULT CALLBACK  Unit_Program_Indicator_prc(HWND, UINT, WPARAM, LPARAM) ;

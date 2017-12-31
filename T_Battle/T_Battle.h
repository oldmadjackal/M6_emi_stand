
/********************************************************************/
/*								    */
/*		������ ������ "��������� ���"    		    */
/*								    */
/********************************************************************/

#ifdef T_BATTLE_EXPORTS
#define T_BATTLE_API __declspec(dllexport)
#else
#define T_BATTLE_API __declspec(dllimport)
#endif


/*---------------------------------------------- ��������� ��������� */

/*---------------------------------------------- ��������� ��������� */

#define  _EXIT_FRAME   -1
#define  _NEXT_FRAME    0
#define  _WAIT_FRAME    1

 typedef struct {                           /* �������� */
                     int  sync_type ;        /* ���� ���� ����� ������� */
#define                     _NO_SYNC  0
#define                      _T_SYNC  1
#define                     _DT_SYNC  2
                  double  t ;                /* ����� ������� */
                  double  dt ;
                    char  action[32] ;       /* �������� */
                    char  object[32] ;       /* ������ */
                    char  event[32] ;        /* ������� */
                     int  size ;             /* ������� ������� */
                  double  period ;           /* ������������� ���������� ������� */
                  double  t_par ;            /* ��������� �������� */

                    char  command[256] ;
              RSS_Object *templ ;
                     int  cnt ;
                  double  next_event ;
                } RSS_Module_Battle_frame ;

#define  FRAME  RSS_Module_Battle_frame

 typedef struct {                           /* �������� ���������� */
                   char  name[16] ;
                   char  value[256] ;
                } RSS_Module_Battle_variable ;

#define  VAR  RSS_Module_Battle_variable

 typedef struct {                           /* �������� ������� �������� */
                   RSS_Object *object ;
                          int  active ;
                       double  cut_time ;    /* ����� ������������ ������� */
                } RSS_Module_Battle_object ;

#define  OBJ  RSS_Module_Battle_object

/*--------------------- �������� ������ ������ "������ ������������" */

#define  _OBJECTS_MAX   1000
#define   _SPAWNS_MAX    100

  class T_BATTLE_API RSS_Module_Battle : public RSS_Kernel {

    public:

     static
        struct RSS_Module_Battle_instr *mInstrList ;              /* ������ ������ */

                    static         OBJ  mObjects[_OBJECTS_MAX] ;
                    static         int  mObjects_cnt ;

                    static       FRAME *mSpawns[_SPAWNS_MAX] ;
                    static         int  mSpawns_cnt ;

                    static       FRAME *mScenario ;               /* �������� ������ ������������ */
                    static         int  mScenario_cnt ;   

                    static         VAR *mVariables ;              /* ������ ���������� */
                    static         int  mVariables_cnt ;   
                    static        HWND  mVariables_Window ;

                    static         int  mHit_cnt ;                /* ����� ��������� �������� */
     

                                  char  mContextAction[64] ;      /* �������� '�����������' �������� */

                    static   RSS_IFace  mExecIFace ;              /* ������������ ����� ���������� �������� */
                    static      double  mExecValue ;              /* �������� ��������� �������� */
                    static         int  mExecError ;              /* ������� ������ �������� */
                    static         int  mExecFail ;               /* ������� ����������� ���������� ���������� ������ */

    public:
     virtual         int  vExecuteCmd   (const char *) ;          /* ��������� ������� */
     virtual         int  vExecuteCmd   (const char *, RSS_IFace *) ;
     virtual        void  vReadSave     (std::string *) ;         /* ������ ������ �� ������ */
     virtual        void  vWriteSave    (std::string *) ;         /* �������� ������ � ������ */
     virtual         int  vGetParameter (char *, char *) ;        /* �������� �������� */
     virtual        void  vChangeContext(void)  ;                 /* ��������� �������� � ��������� ������ */

    public:
                     int  cHelp         (char *, RSS_IFace *) ;   /* ���������� HELP */
                     int  cClear        (char *, RSS_IFace *) ;   /* ���������� CLEAR */
                     int  cAdd          (char *, RSS_IFace *) ;   /* ���������� ADD */
                     int  cList         (char *, RSS_IFace *) ;   /* ���������� LIST */
                     int  cProgram      (char *, RSS_IFace *) ;   /* ���������� PROGRAM */
                     int  cRun          (char *, RSS_IFace *) ;   /* ���������� RUN */

                     int  iFrameExecute (FRAME *, double, int) ;  /* ���������� ���������� ��������� ������ */

    public:
	                  RSS_Module_Battle() ;               /* ����������� */
	                 ~RSS_Module_Battle() ;               /* ���������� */
                                                             } ;

/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_Battle_instr {

           char                     *name_full ;                      /* ������ ��� ������� */
           char                     *name_shrt ;                      /* �������� ��� ������� */
           char                     *help_row ;                       /* HELP - ������ */
           char                     *help_full ;                      /* HELP - ������ */
            int (RSS_Module_Battle::*process)(char *, RSS_IFace *) ;  /* ��������� ���������� ������� */
                              }  ;
/*--------------------------------------------- ���������� ��������� */

/* ����  T_Battle.cpp */

/* ����  T_Battle_dialog.cpp */
    BOOL CALLBACK  Task_Battle_Help_dialog(HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Task_Battle_View_dialog(HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Task_Battle_Vars_dialog(HWND, UINT, WPARAM, LPARAM) ;

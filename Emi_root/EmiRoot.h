
#ifdef  __EMI_ROOT_MAIN__
#define   _EXTERNAL
#else
#define   _EXTERNAL   extern
#endif

/*------------------------------------ ������� ���������� ���������� */

  _EXTERNAL  char  __cwd[FILENAME_MAX] ;    /* ������� ���������� */

  _EXTERNAL  char  __direct_command[128] ;  /* �������� ������ ������� ���������� */
  _EXTERNAL  char  __object_def[32] ;       /* ������ �� ��������� */

#define    _KEY_QUIT   1001

/*------------------------------------------ ������������� ��������� */

  _EXTERNAL  double  __robot_step_joint ;  /* ���� ��������� �������� ����������� �������� */
  _EXTERNAL  double  __robot_step_link ;

/*---- �������� ������ ��������� ���������� "����������� ����������" */

  class RSS_Transit_Main : public RSS_Transit {

    public:
             virtual   int  vExecute(void) ;       /* ���������� �������� */
                                             
    public:
                            RSS_Transit_Main() ;   /* ����������� */
                           ~RSS_Transit_Main() ;   /* ���������� */

                                                               } ;

/*------------------------- �������� ������ ���������� ����� ������� */

  class RSS_Module_Main : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Main_instr *mInstrList ;          /* ������ ������ */

    public:
     virtual         int  vExecuteCmd   (const char *) ;        /* ��������� ������� */
     virtual         int  vGetParameter (char *, char *) ;      /* �������� �������� */
     virtual        void  vProcess      (void) ;                /* ��������� ������� ���������� */
     virtual        void  vShow         (char *) ;              /* ���������� ��������� ������ */
     virtual        void  vReadSave     (std::string *) ;       /* ������ ������ �� ������ */
     virtual        void  vWriteSave    (std::string *) ;       /* �������� ������ � ������ */

    public:
                     int  cHelp         (char *) ;              /* ���������� HELP */
                     int  cShowScene    (char *) ;              /* ���������� SHOWSCENE */
                     int  cKill         (char *) ;              /* ���������� KILL */
                     int  cAll          (char *) ;              /* ���������� ALL */
                     int  cLookInfo     (char *) ;              /* ���������� LOOKINFO */
                     int  cLookPoint    (char *) ;              /* ���������� EYE */ 
                     int  cLookDirection(char *) ;              /* ���������� LOOK */ 
                     int  cLookAt       (char *) ;              /* ���������� LOOKAT */ 
                     int  cLookFrom     (char *) ;              /* ���������� LOOKFROM */ 
                     int  cLookShift    (char *) ;              /* ���������� LOOKSHIFT */ 
                     int  cLookZoom     (char *) ;              /* ���������� ZOOM */
                     int  cRead         (char *) ;              /* ���������� READ */
                     int  cWrite        (char *) ;              /* ���������� WRITE */
                     int  cThreads      (char *) ;              /* ���������� THREADS */
                     int  cStop         (char *) ;              /* ���������� STOP */
                     int  cNext         (char *) ;              /* ���������� NEXT */
                     int  cResume       (char *) ;              /* ���������� RESUME */
                     int  cModules      (char *) ;              /* ���������� MODULES */
                     int  cMemory       (char *) ;              /* ���������� MEMORY */
                     int  cSrand        (char *) ;              /* ���������� SRAND */
                     int  cTime         (char *) ;              /* ���������� TIME */

                    void  ShowExecute   (char *) ;              /* ����������� ������ */

                    void  iDebug        (char *, char *, int) ; /* ���������� ������ � ���� */ 
                    void  iDebug        (char *, char *) ;

    public:
	                  RSS_Module_Main() ;                   /* ����������� */
	                 ~RSS_Module_Main() ;                   /* ���������� */
                                            } ;

/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_Main_instr {

           char                   *name_full ;         /* ������ ��� ������� */
           char                   *name_shrt ;         /* �������� ��� ������� */
           char                   *help_row ;          /* HELP - ������ */
           char                   *help_full ;         /* HELP - ������ */
            int (RSS_Module_Main::*process)(char *) ;  /* ��������� ���������� ������� */
                              }  ;

/*-------------------------------------------------------- ��������� */

/* EmiRoot.cpp */
      int  EmiRoot_system           (void) ;                  /* ��������� ��������� ��������� */
      int  EmiRoot_error            (char *, int) ;           /* ��������� ��������� �� ������� */
      int  EmiRoot_command_start    (char *) ;                /* ������ ������ */
      int  EmiRoot_command_processor(char *) ;                /* ��������� ������ */
      int  EmiRoot_command_read     (char **) ;               /* ���������� ���������� ����� */
      int  EmiRoot_command_write    (char *) ;                /* ������ ���������� ����� */
     void  EmiRoot_command_normalise(char *, int) ;           /* ������������ ��������� ������ */

     void  iDebug                   (char *, char *, int) ;   /* ���������� ������ � ���� */ 
     void  iDebug                   (char *, char *) ;

/* EmiRoot_dialog.cpp */
  INT_PTR CALLBACK  Main_Help_dialog       (HWND, UINT, WPARAM, LPARAM) ;   /* ���������� ���� HELP */
  INT_PTR CALLBACK  Main_ObjectsList_dialog(HWND, UINT, WPARAM, LPARAM) ;   /* ���������� ���� OBJECTS LIST */

/* EmiRoot_monitor.cpp */
               int  EmiRoot_threads    (char *, char *) ;               /* ���������� ������� ������� */ 
               int  EmiRoot_modules    (char *) ;                       /* ����������� ������ �������������� ������� */ 
  INT_PTR CALLBACK  Main_Threads_dialog(HWND, UINT, WPARAM, LPARAM) ;   /* ���������� ���� THREADS */
  INT_PTR CALLBACK  Main_Modules_dialog(HWND, UINT, WPARAM, LPARAM) ;   /* ���������� ���� MODULES */


#ifdef  __EMI_ROOT_MAIN__
#define   _EXTERNAL
#else
#define   _EXTERNAL   extern
#endif

/*------------------------------------ Система командного управления */

  _EXTERNAL  char  __cwd[FILENAME_MAX] ;    /* Рабочая директория */

  _EXTERNAL  char  __direct_command[128] ;  /* Название секции прямого управления */
  _EXTERNAL  char  __object_def[32] ;       /* Объект по умолчанию */

#define    _KEY_QUIT   1001

/*------------------------------------------ Общесистемные параметры */

  _EXTERNAL  double  __robot_step_joint ;  /* Шаги изменения степеней подвижности объектов */
  _EXTERNAL  double  __robot_step_link ;

/*------------------------- Описание класса управления общим модулем */

  class RSS_Module_Main : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Main_instr *mInstrList ;          /* Список команд */

    public:
     virtual         int  vExecuteCmd   (const char *) ;        /* Выполнить команду */
     virtual         int  vGetParameter (char *, char *) ;      /* Получить параметр */
     virtual        void  vProcess      (void) ;                /* Выполнить целевой функционал */
     virtual        void  vShow         (char *) ;              /* Отобразить связанные данные */
     virtual        void  vReadSave     (std::string *) ;       /* Чтение данных из строки */
     virtual        void  vWriteSave    (std::string *) ;       /* Записать данные в строку */

    public:
                     int  cHelp         (char *) ;              /* Инструкция HELP */
                     int  cShowScene    (char *) ;              /* Инструкция SHOWSCENE */
                     int  cKill         (char *) ;              /* Инструкция KILL */
                     int  cAll          (char *) ;              /* Инструкция ALL */
                     int  cLookInfo     (char *) ;              /* Инструкция LOOKINFO */
                     int  cLookPoint    (char *) ;              /* Инструкция EYE */ 
                     int  cLookDirection(char *) ;              /* Инструкция LOOK */ 
                     int  cLookAt       (char *) ;              /* Инструкция LOOKAT */ 
                     int  cLookZoom     (char *) ;              /* Инструкция ZOOM */
                     int  cRead         (char *) ;              /* Инструкция READ */
                     int  cWrite        (char *) ;              /* Инструкция WRITE */
                     int  cThreads      (char *) ;              /* Инструкция THREADS */
                     int  cStop         (char *) ;              /* Инструкция STOP */
                     int  cModules      (char *) ;              /* Инструкция MODULES */
                     int  cMemory       (char *) ;              /* Инструкция MEMORY */
                     int  cSrand        (char *) ;              /* Инструкция SRAND */

                    void  ShowExecute   (char *) ;              /* Отображение данных */

                    void  iDebug        (char *, char *, int) ; /* Отладочная печать в файл */ 
                    void  iDebug        (char *, char *) ;

    public:
	                  RSS_Module_Main() ;                   /* Конструктор */
	                 ~RSS_Module_Main() ;                   /* Деструктор */
                                            } ;

/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_Main_instr {

           char                   *name_full ;         /* Полное имя команды */
           char                   *name_shrt ;         /* Короткое имя команды */
           char                   *help_row ;          /* HELP - строка */
           char                   *help_full ;         /* HELP - полный */
            int (RSS_Module_Main::*process)(char *) ;  /* Процедура выполнения команды */
                              }  ;

/*-------------------------------------------------------- Прототипы */

/* EmiRoot.cpp */
      int  EmiRoot_system           (void) ;                  /* Обработка системных сообщений */
      int  EmiRoot_error            (char *, int) ;           /* Обработка сообщений об ошибках */
      int  EmiRoot_command_start    (char *) ;                /* Запуск команд */
      int  EmiRoot_command_processor(char *) ;                /* Процессор команд */
      int  EmiRoot_command_read     (char **) ;               /* Обработчик командного файла */
      int  EmiRoot_command_write    (char *) ;                /* Запись командного файла */
     void  EmiRoot_command_normalise(char *) ;                /* Нормализация командной строки */

     void  iDebug                   (char *, char *, int) ;   /* Отладочная печать в файл */ 
     void  iDebug                   (char *, char *) ;

/* EmiRoot_dialog.cpp */
    BOOL CALLBACK  Main_Help_dialog       (HWND, UINT, WPARAM, LPARAM) ;   /* Обработчик окна HELP */
    BOOL CALLBACK  Main_ObjectsList_dialog(HWND, UINT, WPARAM, LPARAM) ;   /* Обработчик окна OBJECTS LIST */

/* EmiRoot_monitor.cpp */
      int  EmiRoot_threads(char *, char *) ;      /* Мониторинг рабочих потоков */ 
      int  EmiRoot_modules(char *) ;              /* Отображение списка присоединенных модулей */ 
    BOOL CALLBACK  Main_Threads_dialog(HWND, UINT, WPARAM, LPARAM) ;   /* Обработчик окна THREADS */
    BOOL CALLBACK  Main_Modules_dialog(HWND, UINT, WPARAM, LPARAM) ;   /* Обработчик окна MODULES */

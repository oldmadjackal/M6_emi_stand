
/********************************************************************/
/*								    */
/*		МОДУЛЬ ЗАДАЧИ "СЦЕНАРНЫЙ БОЙ"    		    */
/*								    */
/********************************************************************/

#ifdef T_BATTLE_EXPORTS
#define T_BATTLE_API __declspec(dllexport)
#else
#define T_BATTLE_API __declspec(dllimport)
#endif


/*---------------------------------------------- Параметры генерации */

/*---------------------------------------------- Процессор сценариев */

#define  _EXIT_FRAME   -1
#define  _NEXT_FRAME    0
#define  _WAIT_FRAME    1

 typedef struct {                           /* Оператор */
                     int  sync_type ;        /* Флаг типа метки времени */
#define                     _NO_SYNC  0
#define                      _T_SYNC  1
#define                     _DT_SYNC  2
                  double  t ;                /* Метка времени */
                  double  dt ;
                    char  action[32] ;       /* Операция */
                    char  object[32] ;       /* Объект */
                    char  event[32] ;        /* Событие */
                     int  size ;             /* Счетчик событий */
                  double  period ;           /* Переодичность повторения событий */
                  double  t_par ;            /* Временной параметр */

                    char  command[256] ;
              RSS_Object *templ ;
                     int  cnt ;
                  double  next_event ;
                } RSS_Module_Battle_frame ;

#define  FRAME  RSS_Module_Battle_frame

 typedef struct {                           /* Описание переменной */
                   char  name[16] ;
                   char  value[256] ;
                } RSS_Module_Battle_variable ;

#define  VAR  RSS_Module_Battle_variable

 typedef struct {                           /* Описание объекта сценария */
                   RSS_Object *object ;
                          int  active ;
                       double  cut_time ;    /* Время отслеживания объекта */
                } RSS_Module_Battle_object ;

#define  OBJ  RSS_Module_Battle_object

/*--------------------- Описание класса задачи "Задача обслуживания" */

#define  _OBJECTS_MAX   1000
#define   _SPAWNS_MAX    100

  class T_BATTLE_API RSS_Module_Battle : public RSS_Kernel {

    public:

     static
        struct RSS_Module_Battle_instr *mInstrList ;              /* Список команд */

                    static         OBJ  mObjects[_OBJECTS_MAX] ;
                    static         int  mObjects_cnt ;

                    static       FRAME *mSpawns[_SPAWNS_MAX] ;
                    static         int  mSpawns_cnt ;

                    static       FRAME *mScenario ;               /* Описание задачи обслуживания */
                    static         int  mScenario_cnt ;   

                    static         VAR *mVariables ;              /* Список переменных */
                    static         int  mVariables_cnt ;   
                    static        HWND  mVariables_Window ;

     

                                  char  mContextAction[64] ;      /* Действие 'контекстной' операции */

                    static   RSS_IFace  mExecIFace ;              /* Каноническая форма результата операции */
                    static      double  mExecValue ;              /* Числовой результат операции */
                    static         int  mExecError ;              /* Признак ошибки операции */
                    static         int  mExecFail ;               /* Признак неподдержки интерфейса внутренних связей */

    public:
     virtual         int  vExecuteCmd   (const char *) ;          /* Выполнить команду */
     virtual         int  vExecuteCmd   (const char *, RSS_IFace *) ;
     virtual        void  vReadSave     (std::string *) ;         /* Чтение данных из строки */
     virtual        void  vWriteSave    (std::string *) ;         /* Записать данные в строку */
     virtual         int  vGetParameter (char *, char *) ;        /* Получить параметр */
     virtual        void  vChangeContext(void)  ;                 /* Выполнить действие в контексте потока */

    public:
                     int  cHelp         (char *, RSS_IFace *) ;   /* Инструкция HELP */
                     int  cClear        (char *, RSS_IFace *) ;   /* Инструкция CLEAR */
                     int  cAdd          (char *, RSS_IFace *) ;   /* Инструкция ADD */
                     int  cList         (char *, RSS_IFace *) ;   /* Инструкция LIST */
                     int  cProgram      (char *, RSS_IFace *) ;   /* Инструкция PROGRAM */
                     int  cRun          (char *, RSS_IFace *) ;   /* Инструкция RUN */

                     int  iFrameExecute (FRAME *, double, int) ;  /* Реализация инструкций сервисной задачи */

    public:
	                  RSS_Module_Battle() ;               /* Конструктор */
	                 ~RSS_Module_Battle() ;               /* Деструктор */
                                                             } ;

/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_Battle_instr {

           char                     *name_full ;                      /* Полное имя команды */
           char                     *name_shrt ;                      /* Короткое имя команды */
           char                     *help_row ;                       /* HELP - строка */
           char                     *help_full ;                      /* HELP - полный */
            int (RSS_Module_Battle::*process)(char *, RSS_IFace *) ;  /* Процедура выполнения команды */
                              }  ;
/*--------------------------------------------- Диалоговые процедуты */

/* Файл  T_Battle.cpp */

/* Файл  T_Battle_dialog.cpp */
    BOOL CALLBACK  Task_Battle_Help_dialog(HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Task_Battle_View_dialog(HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Task_Battle_Vars_dialog(HWND, UINT, WPARAM, LPARAM) ;

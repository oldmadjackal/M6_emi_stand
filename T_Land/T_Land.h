
/********************************************************************/
/*								    */
/*		МОДУЛЬ ЗАДАЧИ "СОЗДАНИЕ ЛАНДШАФТА"    		    */
/*								    */
/********************************************************************/

#ifdef T_LAND_EXPORTS
#define T_LAND_API __declspec(dllexport)
#else
#define T_LAND_API __declspec(dllimport)
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
                } RSS_Module_Land_frame ;

#define  FRAME  RSS_Module_Land_frame

 typedef struct {                           /* Описание переменной */
                   char  name[16] ;
                   char  value[256] ;
                } RSS_Module_Land_variable ;

#define  VAR  RSS_Module_Land_variable

 typedef struct {                           /* Описание объекта сценария */
                   RSS_Object *object ;
                          int  active ;
                       double  cut_time ;    /* Время отслеживания объекта */
                } RSS_Module_Land_object ;

#define  OBJ  RSS_Module_Land_object

 typedef struct {                           /* Цвет объекта на карте */
                   char  object[64] ;
               COLORREF  color ;
                } RSS_Module_Land_color ;

#define  COLOR  RSS_Module_Land_color

 typedef struct {                           /* Трассировка объекта на карте */
                   char  object[64] ;
              RSS_Point *points ;
                    int  points_max ;
                    int  points_cnt ;
                } RSS_Module_Land_trace ;

#define  TRACE  RSS_Module_Land_trace

/*----------- Описание класса контекста задачи "Задача обслуживания" */

  class T_LAND_API RSS_Transit_Land : public RSS_Transit {

    public:
             virtual   int  vExecute(void) ;             /* Исполнение действия */

    public:
                            RSS_Transit_Land() ;  /* Конструктор */
                           ~RSS_Transit_Land() ;  /* Деструктор */

                                                               } ;

/*--------------------- Описание класса задачи "Задача обслуживания" */

#define  _OBJECTS_MAX   1000
#define   _SPAWNS_MAX    100

  class T_LAND_API RSS_Module_Land : public RSS_Kernel {

    public:

     static
          struct RSS_Module_Land_instr *mInstrList ;                /* Список команд */

                    static         OBJ  mObjects[_OBJECTS_MAX] ;
                    static         int  mObjects_cnt ;

                    static       FRAME *mSpawns[_SPAWNS_MAX] ;
                    static         int  mSpawns_cnt ;

                    static       FRAME *mScenario ;                 /* Описание задачи обслуживания */
                    static         int  mScenario_cnt ;   

                    static         VAR *mVariables ;                /* Список переменных */
                    static         int  mVariables_cnt ;   
                    static        HWND  mVariables_Window ;

                    static         int  mHit_cnt ;                  /* Число поражённых объектов */
    
                                  char  mContextAction[64] ;        /* Действие 'контекстной' операции */

                    static   RSS_IFace  mExecIFace ;                /* Каноническая форма результата операции */
                    static      double  mExecValue ;                /* Числовой результат операции */
                    static         int  mExecError ;                /* Признак ошибки операции */
                    static         int  mExecFail ;                 /* Признак неподдержки интерфейса внутренних связей */

                    static        HWND  mMapWindow ;                /* Окно индикатора карты */
                    static         int  mMapRegime ;                /* Режим работы карты: */
#define                                    _MAP_KEEP_RANGE  1       /*   Сохранять максимальный охват */                                 
                    static      double  mMapXmin ;                  /* Диапазон координат карты */
                    static      double  mMapXmax ;
                    static      double  mMapZmin ;
                    static      double  mMapZmax ;
                    static       COLOR  mMapColors[_OBJECTS_MAX] ;  /* Цвета объектов на карте */
                    static         int  mMapColors_cnt ;
                    static       TRACE  mMapTraces[_OBJECTS_MAX] ;  /* Цвета объектов на карте */
                    static         int  mMapTraces_cnt ;

    public:
                      RSS_Transit_Land *Context ;

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
                     int  cMap          (char *, RSS_IFace *) ;   /* Инструкция MAP */

                     int  iFrameExecute (FRAME *, double, int) ;  /* Реализация инструкций сервисной задачи */

    public:
	                  RSS_Module_Land() ;               /* Конструктор */
	                 ~RSS_Module_Land() ;               /* Деструктор */
                                                             } ;

/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_Land_instr {

           char                   *name_full ;                      /* Полное имя команды */
           char                   *name_shrt ;                      /* Короткое имя команды */
           char                   *help_row ;                       /* HELP - строка */
           char                   *help_full ;                      /* HELP - полный */
            int (RSS_Module_Land::*process)(char *, RSS_IFace *) ;  /* Процедура выполнения команды */
                              }  ;
/*--------------------------------------------- Диалоговые процедуты */

/* Файл  T_Land.cpp */

/* Файл  T_Land_dialog.cpp */
  INT_PTR CALLBACK  Task_Land_Help_dialog(HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Task_Land_View_dialog(HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Task_Land_Vars_dialog(HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Task_Land_Map_dialog (HWND, UINT, WPARAM, LPARAM) ;
  LRESULT CALLBACK  Task_Land_Map_prc    (HWND, UINT, WPARAM, LPARAM) ;

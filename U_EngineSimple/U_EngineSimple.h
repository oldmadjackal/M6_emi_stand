
/********************************************************************/
/*								    */
/*    МОДУЛЬ УПРАВЛЕНИЯ КОМПОНЕНТОМ "ПРОСТОЙ РАКЕТНЫЙ ДВИГАТЕЛЬ"    */
/*								    */
/********************************************************************/

#ifdef U_ENGINE_SIMPLE_EXPORTS
#define U_ENGINE_SIMPLE_API __declspec(dllexport)
#else
#define U_ENGINE_SIMPLE_API __declspec(dllimport)
#endif

/*--------------------------------------- Описание профиля работы РД */

 struct RSS_Unit_EngineSimple_Profile {
                                          double  t ;       /* Временная метка */
                                          double  thrust ;  /* Сила тяги */
                                          double  mass ;    /* Масса РД */
                                          double  Cx ;      /* Положение центра масс РД */
                                          double  Cy ;
                                          double  Cz ;
                                          double  Mx ;      /* Моменты инерции РД относительно осей */
                                          double  My ;
                                          double  Mz ;
                                      } ;

/*----------------------------- Описание класса объекта "Простой РД" */

  class U_ENGINE_SIMPLE_API RSS_Unit_EngineSimple : public RSS_Unit_Engine {

    public:
                RSS_Unit_EngineSimple_Profile *profile ;          /* Профиль работы двигателя */
                                          int  profile_cnt ;
                                          int  use_mass ;         /* Флаг наличия в профиле данных по массе */
                                          int  use_Cxyz ;         /* Флаг наличия в профиле данных по центру масс */
                                          int  use_Mxyz ;         /* Флаг наличия в профиле данных по моменту инрции */

                RSS_Unit_EngineSimple_Profile  values ;           /* Текущие значения */

                                       double  sigma2 ;           /* Относительное средне-квадратичное отклонение параметров */
                                       double  real_value ;       /* Относительное значение */

                                       double  t_0 ;              /* Время запуска */

                                         HWND  hWnd ;             /* Окно графика профиля */

    public:
               virtual void  vFree            (void) ;                             /* Освободить ресурсы */
               virtual  int  vCalculateStart  (double) ;                           /* Подготовка расчета изменения состояния */
               virtual  int  vCalculate       (double, double, char *, int) ;      /* Расчет изменения состояния */
                                                     
               virtual  int  vCalculateShow   (void) ;                             /* Отображение результата расчета изменения состояния */
               virtual  int  vSpecial         (char *, void *) ;                   /* Специальные действия */

               virtual  int  vSetEngineControl(RSS_Unit_Engine_Control *, int) ;   /* Управление двигателем */
               virtual  int  vGetEngineThrust (RSS_Unit_Engine_Thrust *) ;         /* Вектор тяги двигателя */
               virtual  int  vGetEngineMass   (double *, RSS_Point *) ;            /* Масса и положение центра масс двигателя */
               virtual  int  vGetEngineMI     (double *, double *, double *) ;     /* Моменты инерции двигателя */

	                     RSS_Unit_EngineSimple() ;                /* Конструктор */
	                    ~RSS_Unit_EngineSimple() ;                /* Деструктор */
                                                        } ;

/*----------------- Описание класса управления объектом "Простой РД" */

  class U_ENGINE_SIMPLE_API RSS_Module_EngineSimple : public RSS_Kernel {

    public:

     static
      struct RSS_Module_EngineSimple_instr *mInstrList ;          /* Список команд */

    public:
     virtual  RSS_Object *vCreateObject (RSS_Model_data *) ;                 /* Создание объекта */ 
     virtual         int  vGetParameter (char *, char *) ;                   /* Получить параметр */
     virtual         int  vExecuteCmd   (const char *) ;                     /* Выполнить команду */

    public:
                     int  cHelp         (char *) ;                           /* Инструкция HELP */ 
                     int  cInfo         (char *) ;                           /* Инструкция INFO */ 
                     int  cSigma        (char *) ;                           /* Инструкция SIGMA */
                     int  cProfile      (char *) ;                           /* Инструкция PROFILE */
                     int  cView         (char *) ;                           /* Инструкция VIEW */

                RSS_Unit *FindUnit      (char *) ;                           /* Поиск компонента по имени */
                     int  ReadProfile   (RSS_Unit_EngineSimple *, char *) ;  /* Считывание файла описания профиля работы */

    public:
	                  RSS_Module_EngineSimple() ;              /* Конструктор */
	                 ~RSS_Module_EngineSimple() ;              /* Деструктор */
                                                       } ;

/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_EngineSimple_instr {

           char                           *name_full ;          /* Полное имя команды */
           char                           *name_shrt ;          /* Короткое имя команды */
           char                           *help_row ;           /* HELP - строка */
           char                           *help_full ;          /* HELP - полный */
            int (RSS_Module_EngineSimple::*process)(char *) ;   /* Процедура выполнения команды */
                                 }  ;

/*--------------------------------------------- Диалоговые процедуты */

/* Файл  U_EngineSimple.cpp */

/* Файл  U_EngineSimple_dialog.cpp */
    BOOL CALLBACK  Unit_EngineSimple_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Unit_EngineSimple_View_dialog  (HWND, UINT, WPARAM, LPARAM) ;


/********************************************************************/
/*								    */
/*              МОДУЛЬ УПРАВЛЕНИЯ КОМПОНЕНТОМ "ПРОСТАЯ МОДЕЛЬ"      */
/*								    */
/********************************************************************/

#ifdef U_MODEL_SIMPLE_EXPORTS
#define U_MODEL_SIMPLE_API __declspec(dllexport)
#else
#define U_MODEL_SIMPLE_API __declspec(dllimport)
#endif

/*------------------------- Описание класса объекта "Простая модель" */

  class U_MODEL_SIMPLE_API RSS_Unit_ModelSimple : public RSS_Unit_Model {

    public:
                            double  t_0 ;                 /* Время запуска */

                            double  mass ;                /* Полная масса без массы РД */
                            double  slideway ;            /* Длина движения по направляющей */
                            double  s_azim ;              /* Стандартое отклонение по направлению */
                            double  s_elev ;              /* Стандартое отклонение по углу вылета */

                            double  engine_thrust ;       /* Сила тяги */
                            double  engine_mass ;         /* Масса РД */

    public:
         virtual       void  vFree             (void) ;                             /* Освободить ресурсы */
         virtual RSS_Object *vCopy             (char *) ;                           /* Копировать объект */
         virtual        int  vCalculateStart   (double) ;                           /* Подготовка расчета изменения состояния */
         virtual        int  vCalculate        (double, double, char *, int) ;      /* Расчет изменения состояния */
                                                     
         virtual        int  vCalculateShow    (void) ;                             /* Отображение результата расчета изменения состояния */
         virtual        int  vSpecial          (char *, void *) ;                   /* Специальные действия */

         virtual        int  vSetAeroControl   (RSS_Unit_Aero_Control *, int) ;     /* Управление аэродинамическими поверхностями */
         virtual        int  vSetEngineThrust  (RSS_Unit_Engine_Thrust *, int) ;    /* Вектор тяги двигателя */
         virtual        int  vSetEngineMass    (double, RSS_Point *) ;              /* Масса и положение центра масс двигателя */
         virtual        int  vSetEngineMI      (double, double, double) ;           /* Моменты инерции двигателя */

	                     RSS_Unit_ModelSimple() ;                /* Конструктор */
	                    ~RSS_Unit_ModelSimple() ;                /* Деструктор */
                                                        } ;

/*------------- Описание класса управления объектом "Простая модель" */

  class U_MODEL_SIMPLE_API RSS_Module_ModelSimple : public RSS_Kernel {

    public:

     static
      struct RSS_Module_ModelSimple_instr *mInstrList ;          /* Список команд */

    public:
     virtual  RSS_Object *vCreateObject (RSS_Model_data *) ;                 /* Создание объекта */ 
     virtual         int  vGetParameter (char *, char *) ;                   /* Получить параметр */
     virtual         int  vExecuteCmd   (const char *) ;                     /* Выполнить команду */

    public:
                     int  cHelp         (char *) ;                           /* Инструкция HELP */ 
                     int  cInfo         (char *) ;                           /* Инструкция INFO */ 
                     int  cPars         (char *) ;                           /* Инструкция PARS */ 
                     int  cMass         (char *) ;                           /* Инструкция MASS */ 
                     int  cSlide        (char *) ;                           /* Инструкция SLIDE */ 
                     int  cDeviation    (char *) ;                           /* Инструкция DEVIATION */ 

                RSS_Unit *FindUnit      (char *) ;                           /* Поиск компонента по имени */

    public:
	                  RSS_Module_ModelSimple() ;              /* Конструктор */
	                 ~RSS_Module_ModelSimple() ;              /* Деструктор */
                                                       } ;

/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_ModelSimple_instr {

           char                          *name_full ;          /* Полное имя команды */
           char                          *name_shrt ;          /* Короткое имя команды */
           char                          *help_row ;           /* HELP - строка */
           char                          *help_full ;          /* HELP - полный */
            int (RSS_Module_ModelSimple::*process)(char *) ;   /* Процедура выполнения команды */
                                 }  ;

/*--------------------------------------------- Диалоговые процедуты */

/* Файл  U_ModelSimple.cpp */

/* Файл  U_ModelSimple_dialog.cpp */
    BOOL CALLBACK  Unit_ModelSimple_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Unit_ModelSimple_Pars_dialog  (HWND, UINT, WPARAM, LPARAM) ;

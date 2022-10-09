
/********************************************************************/
/*								    */
/*          МОДУЛЬ УПРАВЛЕНИЯ КОМПОНЕНТОМ "ТАБЛИЧНАЯ МОДЕЛЬ"        */
/*								    */
/********************************************************************/

#ifdef U_MODEL_TABLE_EXPORTS
#define U_MODEL_TABLE_API __declspec(dllexport)
#else
#define U_MODEL_TABLE_API __declspec(dllimport)
#endif

/*------------------------- Описание класса объекта "Простая модель" */

  class U_MODEL_TABLE_API RSS_Unit_ModelTable : public RSS_Unit_Model {

    public:
                            double  t_0 ;                 /* Время запуска */

                              char  path[FILENAME_MAX] ;    /* Путь к файлу с таблицей */
                              char  format[FILENAME_MAX] ;  /* Формат записи таблицы */

                              FILE *file ;                  /* Ссылка на файл таблицы */
                              char  dgt_sep ;               /* Десятичный разделитель */
                               int  start_row ;             /* Первая строка данных */
                            double  t_start ;               /* Начальное время данных */
                               int  t_idx ;                 /* Индексы полей данных */
                               int  x_idx ;
                               int  y_idx ;
                               int  z_idx ;
                               int  v_idx ;
                               int  vx_idx ;
                               int  vy_idx ;
                               int  vz_idx ;
                               int  azim_idx ;
                               int  elev_idx ;
                               int  roll_idx ;

                            double  pars_1[20] ;            /* Границы "кадра" данных */
                            double  pars_2[20] ;

    public:
         virtual       void  vFree             (void) ;                             /* Освободить ресурсы */
         virtual RSS_Object *vCopy             (char *) ;                           /* Копировать объект */
         virtual        int  vCalculateStart   (double) ;                           /* Подготовка расчета изменения состояния */
         virtual        int  vCalculate        (double, double, char *, int) ;      /* Расчет изменения состояния */
                                                     
         virtual        int  vCalculateShow    (double, double) ;                   /* Отображение результата расчета изменения состояния */
         virtual        int  vSpecial          (char *, void *) ;                   /* Специальные действия */

         virtual        int  vSetAeroControl   (RSS_Unit_Aero_Control *, int) ;     /* Управление аэродинамическими поверхностями */
         virtual        int  vSetEngineThrust  (RSS_Unit_Engine_Thrust *, int) ;    /* Вектор тяги двигателя */
         virtual        int  vSetEngineMass    (double, RSS_Point *) ;              /* Масса и положение центра масс двигателя */
         virtual        int  vSetEngineMI      (double, double, double) ;           /* Моменты инерции двигателя */

                        int  iFormatDecode     (char *) ;                           /* Разбор формата записи */
                        int  iParsDissect      (char *, double *, int) ;            /* Разбор строки данных */

	                     RSS_Unit_ModelTable() ;                /* Конструктор */
	                    ~RSS_Unit_ModelTable() ;                /* Деструктор */
                                                        } ;

/*------------- Описание класса управления объектом "Простая модель" */

  class U_MODEL_TABLE_API RSS_Module_ModelTable : public RSS_Kernel {

    public:

     static
      struct RSS_Module_ModelTable_instr *mInstrList ;          /* Список команд */

    public:
     virtual  RSS_Object *vCreateObject (RSS_Model_data *) ;                 /* Создание объекта */ 
     virtual         int  vGetParameter (char *, char *) ;                   /* Получить параметр */
     virtual         int  vExecuteCmd   (const char *) ;                     /* Выполнить команду */

    public:
                     int  cHelp         (char *) ;                           /* Инструкция HELP */ 
                     int  cInfo         (char *) ;                           /* Инструкция INFO */ 
                     int  cPars         (char *) ;                           /* Инструкция PARS */ 
                     int  cPath         (char *) ;                           /* Инструкция PATH */ 
                     int  cFormat       (char *) ;                           /* Инструкция FORMAT */ 

                RSS_Unit *FindUnit      (char *) ;                           /* Поиск компонента по имени */

    public:
	                  RSS_Module_ModelTable() ;              /* Конструктор */
	                 ~RSS_Module_ModelTable() ;              /* Деструктор */
                                                       } ;

/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_ModelTable_instr {

           char                          *name_full ;          /* Полное имя команды */
           char                          *name_shrt ;          /* Короткое имя команды */
           char                          *help_row ;           /* HELP - строка */
           char                          *help_full ;          /* HELP - полный */
            int (RSS_Module_ModelTable::*process)(char *) ;   /* Процедура выполнения команды */
                                    }  ;

/*--------------------------------------------- Диалоговые процедуты */

/* Файл  U_ModelTable.cpp */

/* Файл  U_ModelTable_dialog.cpp */
  INT_PTR CALLBACK  Unit_ModelTable_Help_dialog(HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Unit_ModelTable_Pars_dialog(HWND, UINT, WPARAM, LPARAM) ;

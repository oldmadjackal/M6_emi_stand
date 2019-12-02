
/********************************************************************/
/*								    */
/*              МОДУЛЬ УПРАВЛЕНИЯ КОМПОНЕНТОМ "ПРОСТАЯ ГСН"         */
/*								    */
/********************************************************************/

#ifdef U_HOMING_SIMPLE_EXPORTS
#define U_HOMING_SIMPLE_API __declspec(dllexport)
#else
#define U_HOMING_SIMPLE_API __declspec(dllimport)
#endif

/*---------------------------- Описание класса объекта "Простая ГСН" */

  class U_HOMING_SIMPLE_API RSS_Unit_HomingSimple : public RSS_Unit_Homing {

    public:

#define _UNITS_BY_STAGE_MAX  4

                       int  stage ;
           RSS_Unit_Homing *units_1[_UNITS_BY_STAGE_MAX] ;
                       int  switch_1 ;
           RSS_Unit_Homing *units_2[_UNITS_BY_STAGE_MAX] ;

                       time_t  start_time ;                      /* Время запуска */
//                     double  x, y, z ;                         /* Последние проаналазированные координаты */

//                     int  blast ;                           /* Флаг отработки срабатывания */  

    public:
         virtual       void  vFree                 (void) ;                             /* Освободить ресурсы */
         virtual RSS_Object *vCopy                 (char *) ;                           /* Копировать объект */
         virtual        int  vCalculateStart       (double) ;                           /* Подготовка расчета изменения состояния */
         virtual        int  vCalculate            (double, double, char *, int) ;      /* Расчет изменения состояния */
                                                     
         virtual        int  vCalculateShow        (double, double) ;                   /* Отображение результата расчета изменения состояния */
         virtual        int  vSpecial              (char *, void *) ;                   /* Специальные действия */

         virtual        int  vSetHomingControl     (char *) ;                           /* Управление ГСН */

         virtual        int  vGetHomingDirection   (RSS_Point *) ;                      /* Направление на цель */
         virtual        int  vGetHomingPosition    (RSS_Point *) ;                      /* Относительное положение цели */
         virtual        int  vGetHomingDistance    (double *) ;                         /* Дистанция до цели */
         virtual        int  vGetHomingClosingSpeed(double *) ;                         /* Скорость сближения с целью */

	                     RSS_Unit_HomingSimple() ;                /* Конструктор */
	                    ~RSS_Unit_HomingSimple() ;                /* Деструктор */
                                                        } ;

/*---------------- Описание класса управления объектом "Простая ГСН" */

  class U_HOMING_SIMPLE_API RSS_Module_HomingSimple : public RSS_Kernel {

    public:

     static
      struct RSS_Module_HomingSimple_instr *mInstrList ;          /* Список команд */

    public:
     virtual  RSS_Object *vCreateObject (RSS_Model_data *) ;                 /* Создание объекта */ 
     virtual         int  vGetParameter (char *, char *) ;                   /* Получить параметр */
     virtual         int  vExecuteCmd   (const char *) ;                     /* Выполнить команду */

    public:
                     int  cHelp         (char *) ;                           /* Инструкция HELP */ 
                     int  cInfo         (char *) ;                           /* Инструкция INFO */ 
                     int  cConfig       (char *) ;                           /* Инструкция CONFIG */ 

                RSS_Unit *FindUnit      (char *) ;                           /* Поиск компонента по имени */

    public:
	                  RSS_Module_HomingSimple() ;              /* Конструктор */
	                 ~RSS_Module_HomingSimple() ;              /* Деструктор */
                                                       } ;

/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_HomingSimple_instr {

           char                           *name_full ;          /* Полное имя команды */
           char                           *name_shrt ;          /* Короткое имя команды */
           char                           *help_row ;           /* HELP - строка */
           char                           *help_full ;          /* HELP - полный */
            int (RSS_Module_HomingSimple::*process)(char *) ;   /* Процедура выполнения команды */
                                      }  ;

/*--------------------------------------------- Диалоговые процедуты */

/* Файл  U_HomingSimple.cpp */

/* Файл  U_HomingSimple_dialog.cpp */
  INT_PTR CALLBACK  Unit_HomingSimple_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Unit_HomingSimple_Config_dialog(HWND, UINT, WPARAM, LPARAM) ;

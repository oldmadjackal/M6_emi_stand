
/********************************************************************/
/*								    */
/*        МОДУЛЬ УПРАВЛЕНИЯ КОМПОНЕНТОМ "2-Х ЭТАПНОЕ НАВЕДЕНИЕ"     */
/*								    */
/********************************************************************/

#ifdef U_CONTROL_2STAGE_EXPORTS
#define U_CONTROL_2STAGE_API __declspec(dllexport)
#else
#define U_CONTROL_2STAGE_API __declspec(dllimport)
#endif

/*------------------ Описание класса объекта "2-х этапное наведение" */

  class U_CONTROL_2STAGE_API RSS_Unit_Control2Stage : public RSS_Unit_Control {

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

	                     RSS_Unit_Control2Stage() ;                /* Конструктор */
	                    ~RSS_Unit_Control2Stage() ;                /* Деструктор */
                                                        } ;

/*---------------- Описание класса управления объектом "Простая ГСН" */

  class U_CONTROL_2STAGE_API RSS_Module_Control2Stage : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Control2Stage_instr *mInstrList ;          /* Список команд */

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
	                  RSS_Module_Control2Stage() ;              /* Конструктор */
	                 ~RSS_Module_Control2Stage() ;              /* Деструктор */
                                                       } ;

/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_Control2Stage_instr {

           char                            *name_full ;          /* Полное имя команды */
           char                            *name_shrt ;          /* Короткое имя команды */
           char                            *help_row ;           /* HELP - строка */
           char                            *help_full ;          /* HELP - полный */
            int (RSS_Module_Control2Stage::*process)(char *) ;   /* Процедура выполнения команды */
                                       } ;

/*--------------------------------------------- Диалоговые процедуты */

/* Файл  U_Control2Stage.cpp */

/* Файл  U_Control2Stage_dialog.cpp */
  INT_PTR CALLBACK  Unit_Control2Stage_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Unit_Control2Stage_Config_dialog(HWND, UINT, WPARAM, LPARAM) ;


/********************************************************************/
/*								    */
/*         МОДУЛЬ УПРАВЛЕНИЯ КОМПОНЕНТОМ "КОМБИНИРОВАННАЯ ГСН"      */
/*								    */
/********************************************************************/

#ifdef U_HOMING_HUB_EXPORTS
#define U_HOMING_HUB_API __declspec(dllexport)
#else
#define U_HOMING_HUB_API __declspec(dllimport)
#endif

/*-------------------- Описание класса объекта "Комбинированная ГСН" */

  class U_HOMING_HUB_API RSS_Unit_HomingHub : public RSS_Unit_Homing {

    public:

#define _UNITS_BY_STAGE_MAX  4

                       int  stage ;
           RSS_Unit_Homing *units_1[_UNITS_BY_STAGE_MAX] ;
           RSS_Unit_Homing *units_2[_UNITS_BY_STAGE_MAX] ;

                    time_t  start_time ;                      /* Время запуска */

    public:
         virtual       void  vFree                 (void) ;                             /* Освободить ресурсы */
         virtual   RSS_Unit *vCopy                 (RSS_Object *) ;                     /* Копировать компонент */
         virtual        int  vCalculateStart       (double) ;                           /* Подготовка расчета изменения состояния */
         virtual        int  vCalculate            (double, double, char *, int) ;      /* Расчет изменения состояния */
                                                     
         virtual        int  vCalculateShow        (double, double) ;                   /* Отображение результата расчета изменения состояния */
         virtual        int  vSpecial              (char *, void *) ;                   /* Специальные действия */

         virtual        int  vSetHomingControl     (char *) ;                           /* Управление ГСН */

         virtual        int  vGetHomingDirection   (RSS_Point *) ;                      /* Направление на цель */
         virtual        int  vGetHomingPosition    (RSS_Point *) ;                      /* Относительное положение цели */
         virtual        int  vGetHomingDistance    (double *) ;                         /* Дистанция до цели */
         virtual        int  vGetHomingClosingSpeed(double *) ;                         /* Скорость сближения с целью */

	                     RSS_Unit_HomingHub() ;                /* Конструктор */
	                    ~RSS_Unit_HomingHub() ;                /* Деструктор */
                                                        } ;

/*-------- Описание класса управления объектом "Комбинированная ГСН" */

  class U_HOMING_HUB_API RSS_Module_HomingHub : public RSS_Kernel {

    public:

     static
      struct RSS_Module_HomingHub_instr *mInstrList ;          /* Список команд */

    public:
     virtual  RSS_Object *vCreateObject (RSS_Model_data *) ;                 /* Создание объекта */ 
     virtual         int  vGetParameter (char *, char *) ;                   /* Получить параметр */
     virtual         int  vExecuteCmd   (const char *) ;                     /* Выполнить команду */

    public:
                     int  cHelp         (char *) ;                           /* Инструкция HELP */ 
                     int  cInfo         (char *) ;                           /* Инструкция INFO */ 
                     int  cConfig       (char *) ;                           /* Инструкция CONFIG */ 
                     int  cPlugin       (char *) ;                           /* Инструкция PLUGIN */ 

                RSS_Unit *FindUnit      (char *) ;                           /* Поиск компонента по имени */
   class RSS_Unit_Homing *AddUnit       (RSS_Object *,                       /* Добавление компонента к объекту */
                                          char *, char *, char *) ;

    public:
	                  RSS_Module_HomingHub() ;              /* Конструктор */
	                 ~RSS_Module_HomingHub() ;              /* Деструктор */
                                                       } ;

/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_HomingHub_instr {

           char                        *name_full ;          /* Полное имя команды */
           char                        *name_shrt ;          /* Короткое имя команды */
           char                        *help_row ;           /* HELP - строка */
           char                        *help_full ;          /* HELP - полный */
            int (RSS_Module_HomingHub::*process)(char *) ;   /* Процедура выполнения команды */
                                   }  ;

/*--------------------------------------------- Диалоговые процедуты */

/* Файл  U_HomingHub.cpp */

/* Файл  U_HomingHub_dialog.cpp */
  INT_PTR CALLBACK  Unit_HomingHub_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Unit_HomingHub_Config_dialog(HWND, UINT, WPARAM, LPARAM) ;

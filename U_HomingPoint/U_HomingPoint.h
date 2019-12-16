
/********************************************************************/
/*								    */
/*          МОДУЛЬ УПРАВЛЕНИЯ КОМПОНЕНТОМ "КООРДИНАТНАЯ ГСН"        */
/*								    */
/********************************************************************/

#ifdef U_HOMING_POINT_EXPORTS
#define U_HOMING_POINT_API __declspec(dllexport)
#else
#define U_HOMING_POINT_API __declspec(dllimport)
#endif

/*----------------------- Описание класса объекта "Координатная ГСН" */

  class U_HOMING_POINT_API RSS_Unit_HomingPoint : public RSS_Unit_Homing {

    public:

                          int  method ;              /* Вид системы наведения */
#define                         _INERTIAL_METHOD  1   /* Инерциальная система наведения */

                       time_t  start_time ;          /* Время запуска */
                       double  x_0, y_0, z_0 ;       /* Начальные координаты */
                       double  x_t, y_t, z_t ;       /* Целевые координаты */

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

	                     RSS_Unit_HomingPoint() ;                /* Конструктор */
	                    ~RSS_Unit_HomingPoint() ;                /* Деструктор */
                                                        } ;

/*----------- Описание класса управления объектом "Координатная ГСН" */

  class U_HOMING_POINT_API RSS_Module_HomingPoint : public RSS_Kernel {

    public:

     static
      struct RSS_Module_HomingPoint_instr *mInstrList ;          /* Список команд */

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
	                  RSS_Module_HomingPoint() ;              /* Конструктор */
	                 ~RSS_Module_HomingPoint() ;              /* Деструктор */
                                                       } ;

/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_HomingPoint_instr {

           char                          *name_full ;          /* Полное имя команды */
           char                          *name_shrt ;          /* Короткое имя команды */
           char                          *help_row ;           /* HELP - строка */
           char                          *help_full ;          /* HELP - полный */
            int (RSS_Module_HomingPoint::*process)(char *) ;   /* Процедура выполнения команды */
                                     }  ;

/*--------------------------------------------- Диалоговые процедуты */

/* Файл  U_HomingPoint.cpp */

/* Файл  U_HomingPoint_dialog.cpp */
  INT_PTR CALLBACK  Unit_HomingPoint_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Unit_HomingPoint_Config_dialog(HWND, UINT, WPARAM, LPARAM) ;

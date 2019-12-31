
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

/*------------------------------------- Описание программы наведения */

#define  _STAGE_ACTIONS_MAX   20
#define         _STAGES_MAX   20

typedef  struct {
                  char  operation[128] ;
                   int  if_condition ;
                   int  exit_condition ;
                   int  once ;
                   int  done ;
                }  StageAction ;

typedef  struct {
                       char  name[32] ;
                StageAction  actions[_STAGE_ACTIONS_MAX] ;
                        int  actions_cnt ;               
                }  Stage ;

/*------------------ Описание класса объекта "2-х этапное наведение" */

  class U_CONTROL_2STAGE_API RSS_Unit_Control2Stage : public RSS_Unit_Control {

    public:
                    Stage  stage_start ;
                    Stage  stages[_STAGES_MAX] ;
                      int  stages_cnt ;
                      int  stage ;

                   double  vector_control_max ;              /* Максимальная величина перегрузки управления, м/с*с */

                   time_t  start_time ;                      /* Время запуска */
                     char  warhead_control[1024] ;           /* Команды управления БЧ */
                     char  homing_control[1024] ;            /* Команды управления системой наведения */
               RSS_Vector  vector_control ;                  /* Требуемая перегрузка управления */

                   double  t_distance ;                      /* Дальность до цели */
                RSS_Point  t_xyz ;                           /* Положение цели */
                RSS_Point  t_xyz_prv ;                       /* Положение цели */

    public:
         virtual       void  vFree                 (void) ;                             /* Освободить ресурсы */
         virtual RSS_Object *vCopy                 (char *) ;                           /* Копировать объект */
         virtual        int  vCalculateStart       (double) ;                           /* Подготовка расчета изменения состояния */
         virtual        int  vCalculate            (double, double, char *, int) ;      /* Расчет изменения состояния */
                                                     
         virtual        int  vCalculateShow        (double, double) ;                   /* Отображение результата расчета изменения состояния */
         virtual        int  vSpecial              (char *, void *) ;                   /* Специальные действия */

         virtual        int  vSetHomingDirection   (RSS_Point *) ;                      /* Направление на цель */
         virtual        int  vSetHomingPosition    (RSS_Point *) ;                      /* Относительное положение цели */
         virtual        int  vSetHomingDistance    (double) ;                           /* Дистанция до цели */
         virtual        int  vSetHomingClosingSpeed(double) ;                           /* Скорость сближения с целью */

         virtual        int  vGetWarHeadControl    (char *) ;                           /* Управление БЧ */
         virtual        int  vGetHomingControl     (char *) ;                           /* Управление ГСН */
         virtual        int  vGetEngineControl     (RSS_Unit_Engine_Control *) ;        /* Управление двигателем */
         virtual        int  vGetAeroControl       (RSS_Unit_Aero_Control *) ;          /* Управление аэродинамическими поверхностями */
         virtual        int  vGetVectorControl     (RSS_Vector *) ;                     /* Требуемая перегрузка маневра */

                        int  ExecuteOperation      (char *, char *) ;                   /* Выполнение операции */
                        int  ExecuteExpression     (char *, char *, char *) ;           /* Вычисление выражения  */

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
                     int  cProgram      (char *) ;                           /* Инструкция PROGRAM */ 

                RSS_Unit *FindUnit      (char *) ;                           /* Поиск компонента по имени */
                     int  ReadProgram   (RSS_Unit_Control2Stage *,           /* Считывание программы управления */
                                         char *, char *) ;
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

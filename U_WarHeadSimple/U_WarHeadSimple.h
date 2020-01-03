
/********************************************************************/
/*								    */
/*              МОДУЛЬ УПРАВЛЕНИЯ КОМПОНЕНТОМ "ПРОСТАЯ БЧ"          */
/*								    */
/********************************************************************/

#ifdef U_WARHEAD_SIMPLE_EXPORTS
#define U_WARHEAD_SIMPLE_API __declspec(dllexport)
#else
#define U_WARHEAD_SIMPLE_API __declspec(dllimport)
#endif

/*----------------------------- Описание класса объекта "Простая БЧ" */

  class U_WARHEAD_SIMPLE_API RSS_Unit_WarHeadSimple : public RSS_Unit_WarHead {

    public:
                     double  x, y, z ;                         /* Последние проаналазированные координаты */
                        int  blast ;                           /* Флаг отработки срабатывания */  

                        int  tripping_type ;                   /* Вид срабатывания исполняющего механизма */
#define                        _BY_ALTITUDE   0
#define                        _BY_TIME       1
                     double  tripping_altitude ;               /* Высота срабатывания */
                     double  tripping_time ;                   /* Время срабатывания */
                     time_t  start_time ;                      /* Время запуска */

                        int  load_type ;                       /* Вид снаряжения */
#define                        _GRENADE_TYPE   0
#define                         _STRIPE_TYPE   1
#define                       _PANCAKES_TYPE   2
                     double  hit_range ;                       /* Радиус поражения */
                     double  blast_radius ;                    /* Радиус вспышки взрыва */
                       char  sub_unit[128] ;                   /* Название шаблона суб-боеприпаса */
                 RSS_Object *sub_object ;                      /* Объект суб-боеприпаса */
                        int  sub_count ;                       /* Количество суб-боеприпасов */
                     double  sub_step ;                        /* Шаг выброса суб-боеприпасов */
                        int  sub_series ;                      /* Количество серий сбросов суб-боеприпасов */
                        int  sub_range ;                       /* Параметр разброса серии */

    public:
         virtual       void  vFree             (void) ;                             /* Освободить ресурсы */
         virtual   RSS_Unit *vCopy             (RSS_Object *) ;                     /* Копировать компонент */
         virtual        int  vCalculateStart   (double) ;                           /* Подготовка расчета изменения состояния */
         virtual        int  vCalculate        (double, double, char *, int) ;      /* Расчет изменения состояния */
                                                     
         virtual        int  vCalculateShow    (double, double) ;                   /* Отображение результата расчета изменения состояния */
         virtual        int  vSpecial          (char *, void *) ;                   /* Специальные действия */

         virtual        int  vSetWarHeadControl(char *) ;                           /* Управление БЧ */

	                     RSS_Unit_WarHeadSimple() ;                /* Конструктор */
	                    ~RSS_Unit_WarHeadSimple() ;                /* Деструктор */
                                                        } ;

/*----------------- Описание класса управления объектом "Простая БЧ" */

  class U_WARHEAD_SIMPLE_API RSS_Module_WarHeadSimple : public RSS_Kernel {

    public:

     static
      struct RSS_Module_WarHeadSimple_instr *mInstrList ;          /* Список команд */

    public:
     virtual  RSS_Object *vCreateObject (RSS_Model_data *) ;                 /* Создание объекта */ 
     virtual         int  vGetParameter (char *, char *) ;                   /* Получить параметр */
     virtual         int  vExecuteCmd   (const char *) ;                     /* Выполнить команду */

    public:
                     int  cHelp         (char *) ;                           /* Инструкция HELP */ 
                     int  cInfo         (char *) ;                           /* Инструкция INFO */ 
                     int  cPars         (char *) ;                           /* Инструкция PARS */ 
                     int  cTripping     (char *) ;                           /* Инструкция TRIPPING */ 
                     int  cGrenade      (char *) ;                           /* Инструкция GRENADE */ 
                     int  cStripe       (char *) ;                           /* Инструкция STRIPE */ 
                     int  cPancakes     (char *) ;                           /* Инструкция PANCAKES */ 

                RSS_Unit *FindUnit      (char *) ;                           /* Поиск компонента по имени */

    public:
	                  RSS_Module_WarHeadSimple() ;              /* Конструктор */
	                 ~RSS_Module_WarHeadSimple() ;              /* Деструктор */
                                                       } ;

/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_WarHeadSimple_instr {

           char                            *name_full ;          /* Полное имя команды */
           char                            *name_shrt ;          /* Короткое имя команды */
           char                            *help_row ;           /* HELP - строка */
           char                            *help_full ;          /* HELP - полный */
            int (RSS_Module_WarHeadSimple::*process)(char *) ;   /* Процедура выполнения команды */
                                 }  ;

/*--------------------------------------------- Диалоговые процедуты */

/* Файл  U_WarHeadSimple.cpp */

/* Файл  U_WarHeadSimple_dialog.cpp */
  INT_PTR CALLBACK  Unit_WarHeadSimple_Help_dialog(HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Unit_WarHeadSimple_Pars_dialog(HWND, UINT, WPARAM, LPARAM) ;

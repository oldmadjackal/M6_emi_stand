
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

    public:
               virtual void  vFree             (void) ;                             /* Освободить ресурсы */
               virtual  int  vCalculateStart   (double) ;                           /* Подготовка расчета изменения состояния */
               virtual  int  vCalculate        (double, double, char *, int) ;      /* Расчет изменения состояния */
                                                     
               virtual  int  vCalculateShow    (void) ;                             /* Отображение результата расчета изменения состояния */
               virtual  int  vSpecial          (char *, void *) ;                   /* Специальные действия */

                virtual int  vSetWarHeadControl(char *) ;                           /* Управление БЧ */

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
    BOOL CALLBACK  Unit_WarHeadSimple_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;


/********************************************************************/
/*								    */
/*		МОДУЛЬ УПРАВЛЕНИЯ ОБЪЕКТОМ "СТАНЦИЯ РЭБ"            */
/*								    */
/********************************************************************/

#ifdef O_EWUNIT_EXPORTS
#define O_EWUNIT_API __declspec(dllexport)
#else
#define O_EWUNIT_API __declspec(dllimport)
#endif

#include "..\T_Battle\T_Battle.h"

/*---------------------------------------------- Параметры генерации */

#define  _EWUNIT_THREATS_MAX  50

/*------------------ Описание класса контекста объекта "Станция РЭБ" */

  class O_EWUNIT_API RSS_Transit_EWunit : public RSS_Transit {

    public:
             virtual   int  vExecute(void) ;             /* Исполнение действия */
                                             
    public:
                            RSS_Transit_EWunit() ;        /* Конструктор */
                           ~RSS_Transit_EWunit() ;        /* Деструктор */

                                                               } ;

/*---------------------------- Описание класса объекта "Станция РЭБ" */

  class O_EWUNIT_API RSS_Object_EWunit : public RSS_Object {

    public:
                      char  model_path[FILENAME_MAX] ;        /* Файл модели */

                    double  range_min ;                       /* Диапазон дальностей обнаружения */
                    double  range_max ;
                    double  velocity ;                        /* Критическая скорость сближения */

                      char  owner[128] ;                      /* Объект-носитель */
                RSS_Object *o_owner ;

                      char  event_name[128] ;                 /* Имя формируемого события */
                    double  event_time ;                      /* Время события */
                       int  event_send ;                      /* Влаг отправки события */

         RSS_Module_Battle *battle ;                          /* Ссылка на BATTLE-модуль */

                RSS_Object *threats[_EWUNIT_THREATS_MAX] ;    /* Список угроз */
                       int  threats_cnt ;

                      HWND  hWnd ;                            /* Окно индикатора */ 

    public:
               virtual void  vFree          (void) ;            /* Освободить ресурсы */
               virtual void  vWriteSave     (std::string *) ;   /* Записать данные в строку */
               virtual  int  vCalculateStart(void) ;            /* Подготовка расчета изменения состояния */
               virtual  int  vCalculate     (double, double,    /* Расчет изменения состояния */
                                                     char *, int) ;
               virtual  int  vCalculateShow (void) ;            /* Отображение результата расчета изменения состояния */
               virtual  int  vSpecial       (char *, void *) ;  /* Специальные действия */

	                     RSS_Object_EWunit() ;              /* Конструктор */
	                    ~RSS_Object_EWunit() ;              /* Деструктор */
                                                        } ;

/*---------------- Описание класса управления объектом "Станция РЭБ" */

  class O_EWUNIT_API RSS_Module_EWunit : public RSS_Kernel {

    public:

     static
      struct RSS_Module_EWunit_instr *mInstrList ;          /* Список команд */

    public:
     virtual         int  vGetParameter (char *, char *) ;  /* Получить параметр */
     virtual         int  vExecuteCmd   (const char *) ;    /* Выполнить команду */
     virtual        void  vReadSave     (std::string *) ;   /* Чтение данных из строки */
     virtual        void  vWriteSave    (std::string *) ;   /* Записать данные в строку */

    public:
                     int  cHelp         (char *) ;                     /* Инструкция HELP */ 
                     int  cCreate       (char *) ;                     /* Инструкция CREATE */ 
                     int  cInfo         (char *) ;                     /* Инструкция INFO */ 
                     int  cOwner        (char *) ;                     /* Инструкция OWNER */ 
                     int  cRange        (char *) ;                     /* Инструкция RANGE */
                     int  cVelocity     (char *) ;                     /* Инструкция VELOCITY */
                     int  cEvent        (char *) ;                     /* Инструкция EVENT */ 
                     int  cShow         (char *) ;                     /* Инструкция SHOW */ 

              RSS_Object *FindObject    (char *, int) ;                /* Поиск обьекта по имени */
                     int  CreateObject  (RSS_Model_data *) ;           /* Создание объекта */ 

    public:
	                  RSS_Module_EWunit() ;              /* Конструктор */
	                 ~RSS_Module_EWunit() ;              /* Деструктор */
                                                       } ;

/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_EWunit_instr {

           char                     *name_full ;          /* Полное имя команды */
           char                     *name_shrt ;          /* Короткое имя команды */
           char                     *help_row ;           /* HELP - строка */
           char                     *help_full ;          /* HELP - полный */
            int (RSS_Module_EWunit::*process)(char *) ;   /* Процедура выполнения команды */
                                 }  ;

/*--------------------------------------------- Диалоговые процедуты */

/* Файл  O_EWunit.cpp */

/* Файл  O_EWunit_dialog.cpp */
    BOOL CALLBACK  Object_EWunit_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Object_EWunit_Show_dialog  (HWND, UINT, WPARAM, LPARAM) ;
 LRESULT CALLBACK  Object_EWunit_Indicator_prc(HWND, UINT, WPARAM, LPARAM) ;


/********************************************************************/
/*								    */
/*		МОДУЛЬ УПРАВЛЕНИЯ КОМПОНЕНТОМ "ПОИСКОВАЯ РЛС"       */
/*								    */
/********************************************************************/

#ifdef U_SEARCHRADAR_EXPORTS
#define U_SEARCHRADAR_API __declspec(dllexport)
#else
#define U_SEARCHRADAR_API __declspec(dllimport)
#endif

#include "..\T_Battle\T_Battle.h"

/*---------------------------------------------- Параметры генерации */

#define  _SEARCHRADAR_THREATS_MAX  50

/*---------------- Описание класса контекста объекта "Поисковая РЛС" */

  class U_SEARCHRADAR_API RSS_Transit_SearchRadar : public RSS_Transit {

    public:
             virtual   int  vExecute(void) ;             /* Исполнение действия */
                                             
    public:
                            RSS_Transit_SearchRadar() ;  /* Конструктор */
                           ~RSS_Transit_SearchRadar() ;  /* Деструктор */

                                                               } ;

/*---------------------------- Описание класса объекта "Станция РЭБ" */

  class U_SEARCHRADAR_API RSS_Unit_SearchRadar : public RSS_Unit {

    public:
                    double  range_min ;                            /* Диапазон дальностей обнаружения */
                    double  range_max ;
                    double  velocity ;                             /* Критическая скорость сближения */

                      char  event_name[128] ;                      /* Имя формируемого события */
                    double  event_time ;                           /* Время события */
                       int  event_send ;                           /* Влаг отправки события */

         RSS_Module_Battle *battle ;                               /* Ссылка на BATTLE-модуль */

                RSS_Object *threats[_SEARCHRADAR_THREATS_MAX] ;    /* Список угроз */
                       int  threats_cnt ;

                      HWND  hWnd ;                                 /* Окно индикатора */ 

    public:
               virtual void  vFree          (void) ;            /* Освободить ресурсы */
               virtual void  vWriteSave     (std::string *) ;   /* Записать данные в строку */
               virtual  int  vCalculateStart(double) ;          /* Подготовка расчета изменения состояния */
               virtual  int  vCalculate     (double, double,    /* Расчет изменения состояния */
                                                     char *, int) ;
               virtual  int  vCalculateShow (double, double) ;  /* Отображение результата расчета изменения состояния */
               virtual  int  vSpecial       (char *, void *) ;  /* Специальные действия */

	                     RSS_Unit_SearchRadar() ;           /* Конструктор */
	                    ~RSS_Unit_SearchRadar() ;           /* Деструктор */
                                                        } ;

/*---------------- Описание класса управления объектом "Станция РЭБ" */

  class U_SEARCHRADAR_API RSS_Module_SearchRadar : public RSS_Kernel {

    public:

     static
      struct RSS_Module_SearchRadar_instr *mInstrList ;      /* Список команд */

    public:
     virtual  RSS_Object *vCreateObject (RSS_Model_data *) ; /* Создание объекта */ 
     virtual         int  vGetParameter (char *, char *) ;   /* Получить параметр */
     virtual         int  vExecuteCmd   (const char *) ;     /* Выполнить команду */
     virtual        void  vReadSave     (std::string *) ;    /* Чтение данных из строки */
     virtual        void  vWriteSave    (std::string *) ;    /* Записать данные в строку */

    public:
                     int  cHelp         (char *) ;                     /* Инструкция HELP */ 
                     int  cInfo         (char *) ;                     /* Инструкция INFO */ 
                     int  cRange        (char *) ;                     /* Инструкция RANGE */
                     int  cVelocity     (char *) ;                     /* Инструкция VELOCITY */
                     int  cEvent        (char *) ;                     /* Инструкция EVENT */ 
                     int  cShow         (char *) ;                     /* Инструкция SHOW */ 

                RSS_Unit *FindUnit      (char *) ;                     /* Поиск компонента по имени */

    public:
	                  RSS_Module_SearchRadar() ;              /* Конструктор */
	                 ~RSS_Module_SearchRadar() ;              /* Деструктор */
                                                       } ;

/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_SearchRadar_instr {

           char                          *name_full ;          /* Полное имя команды */
           char                          *name_shrt ;          /* Короткое имя команды */
           char                          *help_row ;           /* HELP - строка */
           char                          *help_full ;          /* HELP - полный */
            int (RSS_Module_SearchRadar::*process)(char *) ;   /* Процедура выполнения команды */
                                     }  ;

/*--------------------------------------------- Диалоговые процедуты */

/* Файл  U_SearchRadar.cpp */

/* Файл  U_SearchRadar_dialog.cpp */
    BOOL CALLBACK  Unit_SearchRadar_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Unit_SearchRadar_Show_dialog  (HWND, UINT, WPARAM, LPARAM) ;
 LRESULT CALLBACK  Unit_SearchRadar_Indicator_prc(HWND, UINT, WPARAM, LPARAM) ;

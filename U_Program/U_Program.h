
/********************************************************************/
/*								    */
/*	МОДУЛЬ УПРАВЛЕНИЯ КОМПОНЕНТОМ "ПРОГРАММНОЕ УПРАВЛЕНИЕ"      */
/*								    */
/********************************************************************/

#ifdef U_PROGRAM_EXPORTS
#define U_PROGRAM_API __declspec(dllexport)
#else
#define U_PROGRAM_API __declspec(dllimport)
#endif

#include "..\T_Battle\T_Battle.h"

/*---------------------------------------------- Параметры генерации */

/*---------------------------------- Встроенные программы управления */

typedef  struct {
                   char  program_name[128] ;
                } RSS_Unit_Program_Embeded ;

#define  _ROUTE_MAX  100
#define  _DROPS_MAX   10
   
typedef  struct {
                   char  program_name[128] ;

       struct {
                 double  x ;
                 double  z ;
              }          route[_ROUTE_MAX] ;
                    int  route_cnt ;
                    int  route_idx ;
                    int  route_dir ;

                 double  g ;
                 double  e_max ;
                 double  e_rate ;
                 double  h_min ;
                 double  column_spacing ;
                 double  jump_distance ;
                 double  drop_height ;
                 double  drop_distance ;
                    int  drops_number ;
                 double  drops_interval ;
       struct {
                   char  weapon[64] ;
              }          drops[_DROPS_MAX] ;
                    int  drops_cnt ;

                   char  stage_0_event[1024] ;
                   char  stage_4_event[1024] ;                  
                 double  stage_4_after ;
                   char  stage_1_radar[1024] ;
                 double  stage_2_time ;
                   char  stage_4_radar[1024] ;

                 double  stage ;
                 double  x_direct ;
                 double  z_direct ;
                 double  a_direct ;
                 double  e_direct ;
                 double  check_time ;
                    int  dropped ;
                    int  stage_4_event_done ;

                } RSS_Unit_Program_Embeded_ColumnHunter ;

/*---- Описание класса контекста компонента "Программное управление" */

  class U_PROGRAM_API RSS_Transit_Program : public RSS_Transit {

    public:
             virtual   int  vExecute(void) ;             /* Исполнение действия */
                                             
    public:
                            RSS_Transit_Program() ;  /* Конструктор */
                           ~RSS_Transit_Program() ;  /* Деструктор */

                                                               } ;

/*-------------- Описание класса компонента "Программное управление" */

  class U_PROGRAM_API RSS_Unit_Program : public RSS_Unit {

    public:
                      char *program ;                              /* Программа управления */
                      void *embeded ;                              /* Стандартная программа управления */                    
                      char *state ;                                /* Хранимое состояние программы */

                      char  event_name[128] ;                      /* Имя формируемого события */
                    double  event_time ;                           /* Время события */
                       int  event_send ;                           /* Влаг отправки события */

         RSS_Module_Battle *battle ;                               /* Ссылка на BATTLE-модуль */

                      HWND  hWnd ;                                 /* Окно индикатора */ 

    public:
               virtual void  vFree          (void) ;            /* Освободить ресурсы */
               virtual void  vWriteSave     (std::string *) ;   /* Записать данные в строку */
               virtual  int  vCalculateStart(double) ;          /* Подготовка расчета изменения состояния */
               virtual  int  vCalculate     (double, double,    /* Расчет изменения состояния */
                                                     char *, int) ;
               virtual  int  vCalculateShow (double, double) ;  /* Отображение результата расчета изменения состояния */
               virtual  int  vSpecial       (char *, void *) ;  /* Специальные действия */

                        int  EmbededColumnHunter(double, double, char *, int) ;

	                     RSS_Unit_Program() ;           /* Конструктор */
	                    ~RSS_Unit_Program() ;           /* Деструктор */
                                                        } ;

/*-- Описание класса управления компонентом "Программное управление" */

  class U_PROGRAM_API RSS_Module_Program : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Program_instr *mInstrList ;      /* Список команд */

    public:
     virtual  RSS_Object *vCreateObject (RSS_Model_data *) ; /* Создание объекта */ 
     virtual         int  vGetParameter (char *, char *) ;   /* Получить параметр */
     virtual         int  vExecuteCmd   (const char *) ;     /* Выполнить команду */
     virtual        void  vReadSave     (std::string *) ;    /* Чтение данных из строки */
     virtual        void  vWriteSave    (std::string *) ;    /* Записать данные в строку */

    public:
                     int  cHelp         (char *) ;                     /* Инструкция HELP */ 
                     int  cInfo         (char *) ;                     /* Инструкция INFO */ 
                     int  cProgram      (char *) ;                     /* Инструкция PROGRAM */ 
                     int  cEmbeded      (char *) ;                     /* Инструкция EMBEDED */ 
                     int  cShow         (char *) ;                     /* Инструкция SHOW */ 

                RSS_Unit *FindUnit      (char *) ;                     /* Поиск компонента по имени */
                    void *ReadEmbeded   (char *, char *) ;             /* Считывание настроек встроенной программы */

    public:
	                  RSS_Module_Program() ;              /* Конструктор */
	                 ~RSS_Module_Program() ;              /* Деструктор */
                                                       } ;

/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_Program_instr {

           char                          *name_full ;          /* Полное имя команды */
           char                          *name_shrt ;          /* Короткое имя команды */
           char                          *help_row ;           /* HELP - строка */
           char                          *help_full ;          /* HELP - полный */
            int (RSS_Module_Program::*process)(char *) ;   /* Процедура выполнения команды */
                                     }  ;

/*--------------------------------------------- Диалоговые процедуты */

/* Файл  U_Program.cpp */

/* Файл  U_Program_dialog.cpp */
    BOOL CALLBACK  Unit_Program_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Unit_Program_Show_dialog  (HWND, UINT, WPARAM, LPARAM) ;
 LRESULT CALLBACK  Unit_Program_Indicator_prc(HWND, UINT, WPARAM, LPARAM) ;

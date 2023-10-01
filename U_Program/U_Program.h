
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
               virtual void  vFree              (void) ;                         /* Освободить ресурсы */
               virtual void  vWriteSave         (std::string *) ;                /* Записать данные в строку */
               virtual  int  vCalculateStart    (double) ;                       /* Подготовка расчета изменения состояния */
               virtual  int  vCalculate         (double, double,                 /* Расчет изменения состояния */
                                                         char *, int) ;
               virtual  int  vCalculateShow     (double, double) ;               /* Отображение результата расчета изменения состояния */
               virtual  int  vSpecial           (char *, void *) ;               /* Специальные действия */

                        int  StartColumnHunter  (double  t) ;
                        int  StartLoneHunter    (double  t) ;
                        int  StartInterceptFAD  (double  t) ;

                        int  EmbededColumnHunter(double, double, char *, int) ;
                        int  EmbededLoneHunter  (double, double, char *, int) ;
                        int  EmbededInterceptFAD(double, double, char *, int) ;

	                     RSS_Unit_Program() ;           /* Конструктор */
	                    ~RSS_Unit_Program() ;           /* Деструктор */
                                                        } ;

/*-- Описание класса управления компонентом "Программное управление" */

  class U_PROGRAM_API RSS_Module_Program : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Program_instr *mInstrList ;      /* Список команд */

    public:
     virtual  RSS_Object *vCreateObject   (RSS_Model_data *) ; /* Создание объекта */ 
     virtual         int  vGetParameter   (char *, char *) ;   /* Получить параметр */
     virtual         int  vExecuteCmd     (const char *) ;     /* Выполнить команду */
     virtual        void  vReadSave       (std::string *) ;    /* Чтение данных из строки */
     virtual        void  vWriteSave      (std::string *) ;    /* Записать данные в строку */

    public:
                     int  cHelp           (char *) ;                     /* Инструкция HELP */ 
                     int  cInfo           (char *) ;                     /* Инструкция INFO */ 
                     int  cProgram        (char *) ;                     /* Инструкция PROGRAM */ 
                     int  cEmbeded        (char *) ;                     /* Инструкция EMBEDED */ 
                     int  cShow           (char *) ;                     /* Инструкция SHOW */ 

                RSS_Unit *FindUnit        (char *) ;                     /* Поиск компонента по имени */
                    void *ReadEmbeded     (char *, char *) ;             /* Считывание настроек встроенной программы */
                    void *ReadColumnHunter(char *, char *) ;
                    void *ReadLoneHunter  (char *, char *) ;
                    void *ReadInterceptFAD(char *, char *) ;

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

/*---------------------------------- Переменные библиотечных функций */

#ifdef  __U_PROGRAM_MAIN__
#define   _EXTERNAL_LOCAL   
#else
#define   _EXTERNAL_LOCAL   extern
#endif

     _EXTERNAL_LOCAL  RSS_Unit_Program *EventUnit ;
     _EXTERNAL_LOCAL        RSS_Object *EventObject ;
     _EXTERNAL_LOCAL              char *EventCallback ;
     _EXTERNAL_LOCAL               int  EventCallback_size ;

/*--------------------------------------------- Диалоговые процедуты */

/* Файл  U_Program_Library.cpp */
         int  Program_emb_Log         (char *text) ;
         int  Program_emb_Turn        (char *angle, double  a_target, double  dt, char *limit_type, double  g) ;
         int  Program_emb_ToPoint     (double  x_target, double  z_target, double  dt, char *limit_type, double  g) ;
         int  Program_emb_ToLine      (double  x_target, double  z_target, 
                                       double  a_target, double  dt, char *limit_type, double  g) ;
         int  Program_emb_GetTargets  (char *unit_name,  RSS_Unit_Target **targets, int *targets_cnt, char *error) ;
         int  Program_emb_DetectOrder (RSS_Unit_Target  *targets, int  targets_cnt,
                                       RSS_Unit_Target **orders,  int *orders_cnt,
                                       double  spacing,  char *error) ;
         int  Program_emb_GetGlobalXYZ(RSS_Unit_Target *rel, RSS_Unit_Target *abs) ;
      double  Program_emb_Distance    (double  x, double  z) ;

#include "..\DCL_kernel\dcl.h"

    Dcl_decl *Program_dcl_Message     (Lang_DCL *,             Dcl_decl **, int) ;    /* Выдача сообщения на экран с ожиданием */
    Dcl_decl *Program_dcl_Log         (Lang_DCL *,             Dcl_decl **, int) ;    /* Запись в лог */
    Dcl_decl *Program_dcl_StateSave   (Lang_DCL *, Dcl_decl *, Dcl_decl **, int) ;    /* Сохранение состояния объекта */
    Dcl_decl *Program_dcl_StateRead   (Lang_DCL *, Dcl_decl *, Dcl_decl **, int) ;    /* Считывание состояния объекта */
    Dcl_decl *Program_dcl_Turn        (Lang_DCL *,             Dcl_decl **, int) ;    /* Изменение направления движения */
    Dcl_decl *Program_dcl_Distance    (Lang_DCL *,             Dcl_decl **, int) ;    /* Определение дистанции */
    Dcl_decl *Program_dcl_ToPoint     (Lang_DCL *,             Dcl_decl **, int) ;    /* Движение в точку */
    Dcl_decl *Program_dcl_ToLine      (Lang_DCL *,             Dcl_decl **, int) ;    /* Выход на линию */
    Dcl_decl *Program_dcl_GetTargets  (Lang_DCL *, Dcl_decl *, Dcl_decl **, int) ;    /* Получение списка наблюдаемых целей */
    Dcl_decl *Program_dcl_DetectOrder (Lang_DCL *, Dcl_decl *, Dcl_decl **, int) ;    /* Выделение организованных построений */
    Dcl_decl *Program_dcl_GetGlobalXYZ(Lang_DCL *, Dcl_decl *, Dcl_decl **, int) ;    /* Рассчёт абсолютных координат по относительным */
    Dcl_decl *Program_dcl_Battle      (Lang_DCL *,             Dcl_decl **, int) ;    /* Команда Battle */

/* Файл  U_Program_dialog.cpp */
  INT_PTR CALLBACK  Unit_Program_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Unit_Program_Show_dialog  (HWND, UINT, WPARAM, LPARAM) ;
  LRESULT CALLBACK  Unit_Program_Indicator_prc(HWND, UINT, WPARAM, LPARAM) ;

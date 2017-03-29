
#ifndef  RSS_KERNEL_H 

#define  RSS_KERNEL_H 

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"

#include <string>

#ifdef RSS_KERNEL_EXPORTS
#define RSS_KERNEL_API __declspec(dllexport)
#else
#define RSS_KERNEL_API __declspec(dllimport)
#endif

/*----------------------------- Описание "входа" программного модуля */

    struct RSS_Module_Entry {
		       class RSS_Kernel *entry ;     /* Вход */
				HMODULE  module ;    /* Дескриптор DLL */
			    } ;

/*----------------------------------------- Описание "входа" ресурса */

    struct RSS_Resource {
		              char  name[256] ;      /* Название */
		              void *ptr ;	     /* Указатель */
			} ;

/*-------------------------------------- Описание данных вычислителя */

    struct RSS_Parameter {
		              char  name[256] ;      /* Название */
		            double  value ;	     /* Значение */
		            double *ptr ;	     /* Указатель */
			 } ;
/*---------------------------------------- Описание Контекста модуля */

   struct RSS_Context {
                         char  name[64] ;    /* Название контекста */
                         void *data ;        /* Данные модуля */
             class RSS_Kernel *module ;      /* Ссылка на модуль */

                      }  ;
/*---------------------------------------- Описание связанного имени */

    struct RSS_Name {
		              char  name[256] ;      /* Имя */
		              char  module[256] ;    /* Связанный модуль */
		    } ;
/*------------------------------ Описание перенаправления управления */

   struct RSS_Redirect {
                         char *master ;     /* Префикс модуля */
                         char *command ;    /* Команда */
             class RSS_Kernel *module ;     /* Ссылка на модуль */

                       }  ;
/*---------------------------------------- Описание слота результата */

   struct RSS_Result {
                         char  id[256] ;    /* Идентификатор */
                         void *result ;     /* Результат */
                     }  ;
/*---------------------------------- Интерфейс к системе отображения */

    struct RSS_Display {
		          int  (* GetList)        (int) ;     /* Резервирование дисплейного списка */
		         void  (* ReleaseList)    (int) ;     /* Освобождение дисплейного списка */
		          int  (* SetFirstContext)(char *) ;  /* Установить первый дисплейного контекст */
		          int  (* SetNextContext) (char *) ;  /* Установить следующий дисплейный контекст */
		         void  (* ShowContext)    (char *) ;  /* Отобразить контекст */
		       double  (* GetContextPar)  (char *) ;  /* Выдать параметр контекста */
                       } ;
/*---------------------------------- Описание "входа" обратной связи */

typedef  int (CALLBACK *RSS_Kernel_CallBack)(int, void *)  ;

    struct RSS_CB_Entry {
		              char  name[64] ;       /* Название */
	       RSS_Kernel_CallBack  proc ;	     /* Указатель */
			} ;

#define  _CALLB_MAX  10

/*----------------------------------------------- Управление памятью */

 typedef  struct {
                    char  entry[128] ;  /* Название входа */
                     int  idx1 ;        /* 1-ый индекс */
                     int  idx2 ;        /* 2-ой индекс */

                    void *ptr  ;        /* Указатель на блок памяти */
                    long  size ;        /* Размер блокa памяти */
                     int  used ;        /* Флаг использования */
                  time_t  time ;        /* Время создания */

                 }  RSS_Memory ;


/*------------------------------------- Интерфейс межмодульной связи */

  class RSS_KERNEL_API RSS_IFace {

    public:
                                     char *std_iface ;

    public:
             virtual  void  vClear (void) ;              /* Сбросить данные */
             virtual  void  vSignal(char *, void *) ;    /* Создать сигнал */
             virtual  void  vPass  (RSS_IFace *) ;       /* Транзит сигнала */
             virtual   int  vDecode(char *, void *) ;    /* Разбор сигнала */
             virtual   int  vCheck (char * ) ;           /* Проверка сигнала */
                                             
    public:
                            RSS_IFace() ;                /* Конструктор */
                           ~RSS_IFace() ;                /* Деструктор */

                                 } ; 

/*------------------------------ Описание класса программного модуля */

  class RSS_KERNEL_API RSS_Kernel {

   public : 
    static   RSS_Kernel  *kernel ;	            /* Модуль-ядро */
    static         HWND   kernel_wnd ;	            /* Окно ядра */
    static         HWND   active_wnd ;	            /* Активное oкно */
    static    HINSTANCE   kernel_inst ;	            /* Идентификатор модуля ядра */
    static          int   stop ;                    /* Флаг остановки исполнения */

    static   RSS_Kernel  *priority_entry ;	    /* Модуль для приоритетного вызова */
    static   RSS_Kernel **events_entry ;            /* Список модулей, имеющих процедуры обработки событий */
    static          int   events_entry_cnt ;

    static   RSS_Object **kernel_objects ;          /* Список объектов */
    static          int   kernel_objects_cnt ;

    static     RSS_Name  *kernel_names ;            /* Список связанных имен */
    static          int   kernel_names_cnt ;

    static   RSS_Kernel **feature_modules ;         /* Список модулей-свойств */
    static          int   feature_modules_cnt ;

    static   RSS_Kernel **calculate_modules ;       /* Список вычислителей */
    static          int   calculate_modules_cnt ;

    static   RSS_Result **results ;                 /* Стек результатов работы модулей */
    static          int   results_cnt ;

    static RSS_CB_Entry   callbacks[_CALLB_MAX] ;   /* Функции обратной связи */

    static  RSS_Display   display ;                 /* Интерфейс к системе отображения */

    static          int   srand_fixed ;             /* Флаг использования фиксированного генератора случайных чисел */

    static          int   debug_flag ;
    static         char   debug_list[1024] ;        /* Список отлаживаемых модулей */ 
   
   public : 
		HMODULE   DLL_module ;		    /* Дескриптор DLL, содержащей модуль */
       RSS_Module_Entry  *modules ;	            /* Список подчиненных модулей */
		    int   modules_cnt ;
	   RSS_Resource  *resources ;	            /* Список используемых ресурсов */
		    int   resources_cnt ;
           RSS_Redirect  *command_redirect ;        /* Список перенаправления команд */
                    int   command_redirect_cnt ;

   public :
                   char  *keyword ;	            /* Ключевое слов - идентификатор проекта */
                   char  *identification ;          /* Идентификатор модуля */
                    int   priority ;                /* Приоритет перехвата управления */
                    int   events_processing ;       /* Флаг собственной обработки событий */
                    
	  	    int   attempt ;		    /* Сетчик "кругов" */

   public:
		   void	  SetKernel(const HWND,         /* Назначение ядра */
				    const HINSTANCE) ; 
    RSS_Kernel_CallBack   SetCallB (char *,             /* Задание функции обратной связи */
				    RSS_Kernel_CallBack) ;
    RSS_Kernel_CallBack   GetCallB (char *) ;           /* Запрос функции обратной связи */
                   void  *Resource (const char *,       /* Регистрация ресурса */
				    const char * ) ; 
		    int	  Load     (const char *) ;     /* Загрузка используемых модулей */
		    int	  Free     (void) ;	        /* Освобождение используемых модулей */
		    
   public:
	    virtual int   vKernelEvents (void) ;           /* Обработка событий */

   public:
     virtual        void  vStart        (void) ;           /* Стартовая разводка */
     virtual        void  vInit         (void) ;           /* Инициализация связей */
     virtual        void  vReadSave     (std::string *) ;  /* Считать данные из строки */
     virtual        void  vWriteSave    (std::string *) ;  /* Записать данные в строку */
     virtual  RSS_Object *vCreateObject (void) ;           /* Создать объект */
     virtual RSS_Feature *vCreateFeature(RSS_Object *) ;   /* Создать свойство */
     virtual         int  vExecuteCmd   (const char *) ;   /* Выполнить команду */
     virtual         int  vExecuteCmd   (const char *,     /* Выполнить команду с выдачей результата по ссылке */
                                          RSS_IFace *) ;
     virtual         int  vCalculate    (char *, char *,   /* Вычислить выражение */
                                         struct RSS_Parameter *,
                                         struct RSS_Parameter *,
                                         double *, 
                                         void **, char *) ;
     virtual         int  vGetParameter (char *, char *) ; /* Получить параметр */
     virtual        void  vSetParameter (char *, char *) ; /* Установить параметр */
     virtual        void  vProcess      (void) ;           /* Выполнить целевой функционал */
     virtual        void  vShow         (char *) ;         /* Отобразить связанные данные */
     virtual        void  vChangeContext(void)  ;          /* Выполнить действие в контексте потока */

     virtual RSS_Context *vAddData      (RSS_Context ***); /* Ввод контекста модуля в список контекстов */
     virtual         int  vReadData     (RSS_Context ***,  /* Считать данные контекста модуля из строки */
                                         std::string * ) ;
     virtual        void  vWriteData    (RSS_Context *,    /* Записать данные контекста модуля в строку */
                                         std::string * ) ;
     virtual        void  vReleaseData  (RSS_Context *) ;  /* Освободить ресурсы данных контекста модуля */

    public:

     static          int   RSS_Kernel::memchk_regime ;
#define                     _MEMCHECK_OFF      0
#define                     _MEMCHECK_ACTUAL   1
#define                     _MEMCHECK_HISTORY  2
     static   RSS_Memory  *RSS_Kernel::memchk_list ;
     static          int   RSS_Kernel::memchk_list_max ; 
     static          int   RSS_Kernel::memchk_list_idx ; 
     static          int   RSS_Kernel::memchk_rep_num ; 

		    void  gMemOnOff     (int) ;
		    void  gMemList      (char *) ;
		    void *gMemCalloc    (size_t, size_t, char *, int, int) ;  
		    void *gMemRealloc   (void *, size_t, char *, int, int) ;  
		    void  gMemFree      (void *) ;
		     int  gMemCheck     (void) ;
                    void  iMemAddList   (void *, int, char *, int, int) ;
                    void  iMemDelList   (void *) ;

    public:
		    void  iErrorMsg     (const char *) ;   /* Формирование сообщений об ошибках */
		    void  iLogFile      (const char *) ;   /* Запись сообщения в лог файл */

    public:
			  RSS_Kernel    () ;               /* Конструктор */
			 ~RSS_Kernel    () ;               /* Деструктор */
		                  } ;

/*-------------------------------------------------------- Сообщения */

#define  _USER_EXECUTE_COMMAND    101    /* Выполнить команду */
#define  _USER_COMMAND_PREFIX     102    /* Установка префикса командной строки */
#define  _USER_DEFAULT_OBJECT     103    /* Установка объекта по умолчанию */

#define  _USER_DIRECT_COMMAND     201    /* Задание секции прямого управления */

#define  _USER_SHOW               301    /* Отображение */
#define  _USER_CHANGE_CONTEXT     302    /* Выполнить в контексте потока сообщений */

#define  _USER_THREAD_ADD         401    /* Зарегистрировать рабочий поток */
#define  _USER_THREAD_DELETE      402    /* Исключить рабочий поток */

#define  _USER_ERROR_MESSAGE      901    /* Сообщение об ошибке */
#define  _USER_SHOW_COMMAND       902    /* Показать команду */
#define  _USER_SHOW_INFO          903    /* Показать информацию об объекте */
#define  _USER_INFO_MESSAGE       904    /* Сообщение не об ошибке */
#define  _USER_CHECK_MESSAGE      905    /* Сообщение контрольных условий сцены: пересечение, ограничения и прочее */
#define  _USER_THREAD_MESSAGE     906    /* Сообщение об исполнении потока */

/*-------------------------------------------------------------------*/

#endif        // RSS_KERNEL_H 


/********************************************************************/
/*								    */
/*		МОДУЛЬ УПРАВЛЕНИЯ ЭФФЕКТОМ "ВЗРЫВ"                  */
/*								    */
/********************************************************************/

#ifdef E_BLAST_EXPORTS
#define E_BLAST_API __declspec(dllexport)
#else
#define E_BLAST_API __declspec(dllimport)
#endif

/*---------------------------------------------- Параметры генерации */


/*------------------------ Описание класса контекста эффекта "Взрыв" */

  class E_BLAST_API RSS_Transit_Blast : public RSS_Transit {

    public:
             virtual   int  vExecute(void) ;             /* Исполнение действия */
                                             
    public:
                            RSS_Transit_Blast() ;        /* Конструктор */
                           ~RSS_Transit_Blast() ;        /* Деструктор */

                                                            } ;

/*---------------------------------- Описание класса эффекта "Взрыв" */

  class E_BLAST_API RSS_Effect_Blast : public RSS_Object {

    public:
                     double  hit_range ;                        /* Радиус поражения */

                     double  range_max ;                        /* Максимальный радиус вспышки */
                     double  range ;                            /* Текущий радиус вспышки */

                     double  t_0 ;                              /* Время запуска */

                   COLORREF  color ;                            /* Цвет взрыва */
                        int  dlist1_idx ;                       /* Индексы дисплейных списков */
                        int  dlist2_idx ;

                        int  blast_step ;                       /* Стадия взрыва */

    public:
               virtual void  vFree          (void) ;                 /* Освободить ресурсы */
               virtual void  vWriteSave     (std::string *) ;        /* Записать данные в строку */
               virtual  int  vCalculateStart(double) ;               /* Подготовка расчета изменения состояния */
               virtual  int  vCalculate     (double, double,         /* Расчет изменения состояния */
                                                     char *, int) ;
               virtual  int  vCalculateShow (void) ;                 /* Отображение результата расчета изменения состояния */
               virtual  int  vEvent         (char *, double) ;       /* Обработка событий */
               virtual  int  vSpecial       (char *, void *) ;       /* Специальные действия */

                        int  iShowBlast     (void) ;                 /* Отрисовка эффекта */
                        int  iShowBlast_    (void) ;                 /* Отрисовка эффекта с передачей контекста */
                       void  iShowCone      (double, double, int, COLORREF) ;

	                     RSS_Effect_Blast() ;                    /* Конструктор */
	                    ~RSS_Effect_Blast() ;                    /* Деструктор */
                                                        } ;

/*---------------------- Описание класса управления эффектом "Взрыв" */

  class E_BLAST_API RSS_Module_Blast : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Blast_instr *mInstrList ;          /* Список команд */

    public:
     virtual  RSS_Object *vCreateObject (RSS_Model_data *) ; /* Создание объекта */ 
     virtual         int  vGetParameter (char *, char *) ;   /* Получить параметр */
     virtual        void  vShow         (char *) ;           /* Отобразить связанные данные */
     virtual         int  vExecuteCmd   (const char *) ;     /* Выполнить команду */
     virtual        void  vReadSave     (std::string *) ;    /* Чтение данных из строки */
     virtual        void  vWriteSave    (std::string *) ;    /* Записать данные в строку */

    public:
                     int  cHelp         (char *) ;           /* Инструкция HELP */ 
                     int  cCreate       (char *) ;           /* Инструкция CREATE */ 
                     int  cInfo         (char *) ;           /* Инструкция INFO */ 
                     int  cRange        (char *) ;           /* Инструкция RANGE */

              RSS_Object *FindObject    (char *, int) ;      /* Поиск обьекта типа BLAST по имени */

    public:
	                  RSS_Module_Blast() ;               /* Конструктор */
	                 ~RSS_Module_Blast() ;               /* Деструктор */
                                                       } ;

/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_Blast_instr {

           char                    *name_full ;          /* Полное имя команды */
           char                    *name_shrt ;          /* Короткое имя команды */
           char                    *help_row ;           /* HELP - строка */
           char                    *help_full ;          /* HELP - полный */
            int (RSS_Module_Blast::*process)(char *) ;   /* Процедура выполнения команды */
                               }  ;

/*--------------------------------------------- Диалоговые процедуты */

/* Файл  E_Blast.cpp */

/* Файл  E_Blast_dialog.cpp */
    BOOL CALLBACK  Effect_Blast_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;

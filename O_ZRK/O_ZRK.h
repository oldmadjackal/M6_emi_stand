
/********************************************************************/
/*                                                                  */
/*     МОДУЛЬ УПРАВЛЕНИЯ ОБЪЕКТОМ "ЗЕНИТНЫЙ РАКЕТНЫЙ КОМПЛЕКС"      */
/*                                                                  */
/********************************************************************/

#ifdef O_ZRK_EXPORTS
#define O_ZRK_API __declspec(dllexport)
#else
#define O_ZRK_API __declspec(dllimport)
#endif


/*---------------------------------------------- Параметры генерации */

/*-------------------------- Описание класса контекста объекта "ЗРК" */

  class O_ZRK_API RSS_Transit_ZRK : public RSS_Transit {

    public:
             virtual   int  vExecute(void) ;        /* Исполнение действия */
                                             
    public:
                            RSS_Transit_ZRK() ;     /* Конструктор */
                           ~RSS_Transit_ZRK() ;     /* Деструктор */

                                                       } ;

/*------------------------------------ Описание класса объекта "ЗРК" */

 struct RSS_Object_ZRKTrace {
                                     double  x_base ;
                                     double  y_base ;
                                     double  z_base ;

                                     double  a_azim ;
                                     double  a_elev ;
                                     double  a_roll ;
  
                                     double  x_velocity ;
                                     double  y_velocity ;
                                     double  z_velocity ;

                                   COLORREF  color ;
                            } ;

 struct RSS_Object_ZRK_trace {
                                RSS_Object *target ;
                                      char  missile_name[128] ;  /* Название последней запущеной ЗУР */
                                RSS_Object *missile ;            /* Ссылак на последнюю запущеную ЗУР */
                                    double  last_check ;
                                       int  next ;               /* Сколько ЗУР осталось запустить в текущем залпе */
                             }  ;

  class O_ZRK_API RSS_Object_ZRK : public RSS_Object {

    public:
                        char  model_path[FILENAME_MAX] ;         /* Файл модели */

                      double  r_firing_min ;                     /* Минимальная дальность пуска */
                      double  r_firing_max ;                     /* Максимальная дальность пуска */
                      double  h_firing_max ;                     /* Максимальная высота цели */
                         int  ballistic_target ;                 /* Отсев только баллистических целей */
                        char  missile_type[FILENAME_MAX] ;       /* Имя объекта - шаблона ракеты */
                  RSS_Object *missile_tmpl ;                     /* Ссылка на объект - шаблон ракеты */
                         int  missiles_max ;                     /* Максимальное число ракет, готовых к пуску */
                         int  missiles_firing ;                  /* Число ракет на одну цель */
                      double  firing_pause ;                     /* Интервал между пусками, секунд */
                         int  v_launch ;                         /* Вертикальный пуск */ 

                         int  start_flag ;                       /* Признак первого шага моделирования */

                      double  v_abs ;                            /* Нормальная скорость шасси */
                         int  missiles_cnt ;                     /* Число ракет, готовых к пуску */
                         int  missiles_idx ;                     /* Счетчик запущенных ракет */
                      double  last_firing ;                      /* Время последнего пуска ЗУР */
        RSS_Object_ZRK_trace *firing_channel ;                   /* Текущий целевой канал  */
                         int  stage ;                            /* Стадия боевого цикла */
#define                              _SEARCH_STAGE  0
#define                        _FIRING_FIRST_STAGE  1
#define                         _FIRING_NEXT_STAGE  2

#define                              _TRACES_MAX  20
        RSS_Object_ZRK_trace  traces[_TRACES_MAX] ;              /* Параметры обстреливаемых целей */

   private:

         RSS_Object_ZRKTrace *mTrace ;                           /* Траектория */
                         int  mTrace_cnt ;  
                         int  mTrace_max ;
                    COLORREF  mTrace_color ;
                         int  mTrace_dlist ;

    public:

         virtual       void  vFree          (void) ;            /* Освободить ресурсы */
         virtual RSS_Object *vCopy          (char *) ;          /* Копировать объект */
         virtual       void  vPush          (void) ;            /* Сохранить состояние объекта */
         virtual       void  vPop           (void) ;            /* Восстановить состояние объекта */
         virtual       void  vWriteSave     (std::string *) ;   /* Записать данные в строку */
         virtual        int  vCalculateStart(double) ;          /* Подготовка расчета изменения состояния */
         virtual        int  vCalculate     (double, double,    /* Расчет изменения состояния */
                                                     char *, int) ;
         virtual        int  vCalculateShow (double, double) ;  /* Отображение результата расчета изменения состояния */
         virtual        int  vEvent         (char *, double) ;  /* Обработка событий */
         virtual        int  vSpecial       (char *, void *) ;  /* Специальные действия */
                        int  iSaveTracePoint(char *) ;          /* Сохранение точки траектории */
                       void  iShowTrace_    (void) ;            /* Отображение траектории с передачей контекста */
                       void  iShowTrace     (void) ;            /* Отображение траектории */

	                     RSS_Object_ZRK() ;                     /* Конструктор */
	                    ~RSS_Object_ZRK() ;                     /* Деструктор */
                                                        } ;

/*------------------------ Описание класса управления объектом "ЗРК" */

  class O_ZRK_API RSS_Module_ZRK : public RSS_Kernel {

    public:

     static
      struct RSS_Module_ZRK_instr *mInstrList ;             /* Список команд */

                  double  g_step ;                          /* Шаг изменения траекторной перегрузки */
                  double  a_step ;                          /* Шаг изменения углов */

    public:
     virtual  RSS_Object *vCreateObject (RSS_Model_data *) ;           /* Создание объекта */ 
     virtual         int  vGetParameter (char *, char *) ;             /* Получить параметр */
     virtual         int  vExecuteCmd   (const char *) ;               /* Выполнить команду */
     virtual        void  vReadSave     (std::string *) ;              /* Чтение данных из строки */
     virtual        void  vWriteSave    (std::string *) ;              /* Записать данные в строку */

    public:
                     int  cHelp         (char *) ;                     /* Инструкция HELP */ 
                     int  cCreate       (char *) ;                     /* Инструкция CREATE */ 
                     int  cInfo         (char *) ;                     /* Инструкция INFO */ 
                     int  cCopy         (char *) ;                     /* Инструкция COPY */ 
                     int  cOwner        (char *) ;                     /* Инструкция OWNER */ 
                     int  cTarget       (char *) ;                     /* Инструкция TARGET */ 
                     int  cBase         (char *) ;                     /* Инструкция BASE */ 
                     int  cAngle        (char *) ;                     /* Инструкция ANGLE */ 
                     int  cVelocity     (char *) ;                     /* Инструкция VELOCITY */
                     int  cTrace        (char *) ;                     /* Инструкция TRACE */
                     int  cFiring       (char *) ;                     /* Инструкция FIRING */

              RSS_Object *FindObject    (char *, int) ;                /* Поиск обьекта типа ZRK по имени */

    public:
	                  RSS_Module_ZRK() ;                     /* Конструктор */
	                 ~RSS_Module_ZRK() ;                     /* Деструктор */
                                                       } ;

/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_ZRK_instr {

           char                       *name_full ;          /* Полное имя команды */
           char                       *name_shrt ;          /* Короткое имя команды */
           char                       *help_row ;           /* HELP - строка */
           char                       *help_full ;          /* HELP - полный */
            int (RSS_Module_ZRK::*process)(char *) ;        /* Процедура выполнения команды */
                             }  ;

/*--------------------------------------------- Диалоговые процедуты */

/* Файл  O_ZRK.cpp */

/* Файл  O_ZRK_dialog.cpp */
  INT_PTR CALLBACK  Object_ZRK_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Object_ZRK_Create_dialog(HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Object_ZRK_Firing_dialog(HWND, UINT, WPARAM, LPARAM) ;

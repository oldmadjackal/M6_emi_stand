
/********************************************************************/
/*								    */
/*		МОДУЛЬ УПРАВЛЕНИЯ ОБЪЕКТОМ "SADARM"  		    */
/*								    */
/********************************************************************/

#ifdef O_SADARM_EXPORTS
#define O_SADARM_API __declspec(dllexport)
#else
#define O_SADARM_API __declspec(dllimport)
#endif


/*---------------------------------------------- Параметры генерации */

/*----------------------- Описание класса контекста объекта "SADARM" */

  class O_SADARM_API RSS_Transit_Sadarm : public RSS_Transit {

    public:
             virtual   int  vExecute(void) ;             /* Исполнение действия */
                                             
    public:
                            RSS_Transit_Sadarm() ;        /* Конструктор */
                           ~RSS_Transit_Sadarm() ;        /* Деструктор */

                                                              } ;

/*----------------------------- Описание класса свойства "Поражение" */

  class O_SADARM_API RSS_Feature_Hit_Sadarm : public RSS_Feature_Hit {

    public:
            virtual  int  vCheck   (void *, RSS_Objects_List *) ;     /* Проверка непротиворечивости свойства */  
                                         

    private:
                     int  iBodyTest(RSS_Feature_Hit_Body *,           /* Контроль пересечения тела с трассой */
                                                  double *, double *, int) ;
                                    
    public:
	                  RSS_Feature_Hit_Sadarm() ;                /* Конструктор */
	                 ~RSS_Feature_Hit_Sadarm() ;                /* Деструктор */
                                                           } ;

/*--------------------------------- Описание класса объекта "SADARM" */

  class O_SADARM_API RSS_Object_Sadarm : public RSS_Object {

    public:
                      char  model_path[FILENAME_MAX] ;        /* Файл модели */

                  double  v_abs ;                             /* Нормальная скорость */
                  double  scan_altitude ;                     /* Начальная высота сканирования */
                  double  scan_tilt ;                         /* Угол наклона линии сканирования к вертикали */
                  double  scan_rotation ;                     /* Скорость сканирования, оборотов в минуту */
                  double  scan_view ;                         /* Поле зрения, градусов */

                COLORREF  color ;                             /* Цвет воронки */
                     int  view_dlist ;                        /* Индексы дисплейных списков */
                     int  hit_flag ;                          /* Отрисовка выстрела */
               RSS_Point  hit_point ;                         /* Точка поражения */

    public:
         virtual       void  vFree          (void) ;            /* Освободить ресурсы */
         virtual RSS_Object *vCopy          (char *) ;          /* Копировать объект */
         virtual       void  vWriteSave     (std::string *) ;   /* Записать данные в строку */
         virtual        int  vCalculateStart(double) ;          /* Подготовка расчета изменения состояния */
         virtual        int  vCalculate     (double, double,    /* Расчет изменения состояния */
                                                     char *, int) ;
         virtual        int  vCalculateShow (double, double) ;  /* Отображение результата расчета изменения состояния */
         virtual        int  vEvent         (char *, double) ;  /* Обработка событий */
         virtual        int  vSpecial       (char *, void *) ;  /* Специальные действия */

                        int  iShowView      (void) ;            /* Отрисовка эффекта */
                        int  iShowView_     (void) ;            /* Отрисовка эффекта с передачей контекста */

	                     RSS_Object_Sadarm() ;             /* Конструктор */
	                    ~RSS_Object_Sadarm() ;             /* Деструктор */
                                                        } ;

/*--------------------- Описание класса управления объектом "SADARM" */

  class O_SADARM_API RSS_Module_Sadarm : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Sadarm_instr *mInstrList ;          /* Список команд */

    public:
     virtual  RSS_Object *vCreateObject (RSS_Model_data *) ;           /* Создание объекта */ 
     virtual         int  vGetParameter (char *, char *) ;             /* Получить параметр */
     virtual        void  vShow         (char *) ;                     /* Отобразить связанные данные */
     virtual         int  vExecuteCmd   (const char *) ;               /* Выполнить команду */
     virtual        void  vReadSave     (std::string *) ;              /* Чтение данных из строки */
     virtual        void  vWriteSave    (std::string *) ;              /* Записать данные в строку */

    public:
                     int  cHelp         (char *) ;                     /* Инструкция HELP */
                     int  cCreate       (char *) ;                     /* Инструкция CREATE */
                     int  cInfo         (char *) ;                     /* Инструкция INFO */
                     int  cCopy         (char *) ;                     /* Инструкция COPY */
                     int  cBase         (char *) ;                     /* Инструкция BASE */ 
                     int  cVelocity     (char *) ;                     /* Инструкция VELOCITY */
                     int  cScan         (char *) ;                     /* Инструкция SCAN */
                     int  cTrace        (char *) ;                     /* Инструкция TRACE */

              RSS_Object *FindObject    (char *, int) ;                /* Поиск обьекта типа BODY по имени */

    public:
	                  RSS_Module_Sadarm() ;              /* Конструктор */
	                 ~RSS_Module_Sadarm() ;              /* Деструктор */
                                                       } ;

/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_Sadarm_instr {

           char                     *name_full ;          /* Полное имя команды */
           char                     *name_shrt ;          /* Короткое имя команды */
           char                     *help_row ;           /* HELP - строка */
           char                     *help_full ;          /* HELP - полный */
            int (RSS_Module_Sadarm::*process)(char *) ;   /* Процедура выполнения команды */
                                }  ;

/*--------------------------------------------- Диалоговые процедуты */

/* Файл  O_Sadarm.cpp */

/* Файл  O_Sadarm_dialog.cpp */
  INT_PTR CALLBACK  Object_Sadarm_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Object_Sadarm_Create_dialog(HWND, UINT, WPARAM, LPARAM) ;

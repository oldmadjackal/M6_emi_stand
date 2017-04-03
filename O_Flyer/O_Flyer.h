
/********************************************************************/
/*								    */
/*		МОДУЛЬ УПРАВЛЕНИЯ ОБЪЕКТОМ "ЛЕТУН"  		    */
/*								    */
/********************************************************************/

#ifdef O_FLYER_EXPORTS
#define O_FLYER_API __declspec(dllexport)
#else
#define O_FLYER_API __declspec(dllimport)
#endif


/*---------------------------------------------- Параметры генерации */

/*------------------------ Описание класса контекста объекта "Летун" */

  class O_FLYER_API RSS_Transit_Flyer : public RSS_Transit {

    public:
             virtual   int  vExecute(void) ;             /* Исполнение действия */
                                             
    public:
                            RSS_Transit_Flyer() ;        /* Конструктор */
                           ~RSS_Transit_Flyer() ;        /* Деструктор */

                                                           } ;

/*---------------------------------- Описание класса объекта "Летун" */

 struct RSS_Object_FlyerTrace {
                                  double  x_base ;
                                  double  y_base ;
                                  double  z_base ;

                                  double  a_azim ;
                                  double  a_elev ;
                                  double  a_roll ;

                                  double  x_velocity ;
                                  double  y_velocity ;
                                  double  z_velocity ;
                              } ;

  class O_FLYER_API RSS_Object_Flyer : public RSS_Object {

    public:
                      char  model_path[FILENAME_MAX] ;        /* Файл модели */

     RSS_Object_FlyerTrace *mTrace ;                          /* Траектория */
                       int  mTrace_cnt ;  
                       int  mTrace_max ;
                  COLORREF  mTrace_color ;
                       int  mTrace_dlist ;

    public:
               virtual void  vFree          (void) ;            /* Освободить ресурсы */
               virtual void  vWriteSave     (std::string *) ;   /* Записать данные в строку */
               virtual  int  vCalculate     (double, double) ;  /* Расчет изменения состояния */
                        int  iSaveTracePoint(void) ;            /* Сохранение точки траектории */
                       void  iShowTrace_    (void) ;            /* Отображение траектории с передачей контекста */
                       void  iShowTrace     (void) ;            /* Отображение траектории */

	                     RSS_Object_Flyer() ;               /* Конструктор */
	                    ~RSS_Object_Flyer() ;               /* Деструктор */
                                                        } ;

/*---------------------- Описание класса управления объектом "Летун" */

  class O_FLYER_API RSS_Module_Flyer : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Flyer_instr *mInstrList ;          /* Список команд */
				     
    public:
     virtual         int  vExecuteCmd   (const char *) ;    /* Выполнить команду */
     virtual        void  vReadSave     (std::string *) ;   /* Чтение данных из строки */
     virtual        void  vWriteSave    (std::string *) ;   /* Записать данные в строку */

    public:
                     int  cHelp         (char *) ;          /* Инструкция HELP */ 
                     int  cCreate       (char *) ;          /* Инструкция CREATE */ 
                     int  cInfo         (char *) ;          /* Инструкция INFO */ 
                     int  cBase         (char *) ;          /* Инструкция BASE */ 
                     int  cAngle        (char *) ;          /* Инструкция ANGLE */ 
                     int  cVelocity     (char *) ;          /* Инструкция VELOCITY */
                     int  cTrace        (char *) ;          /* Инструкция TRACE */

        RSS_Object_Flyer *FindObject    (char *) ;          /* Поиск обьекта типа BODY по имени */
                     int  CreateObject  (RSS_Model_data *); /* Создание объекта */ 

    public:
	                  RSS_Module_Flyer() ;              /* Конструктор */
	                 ~RSS_Module_Flyer() ;              /* Деструктор */
                                                       } ;

/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_Flyer_instr {

           char                   *name_full ;          /* Полное имя команды */
           char                   *name_shrt ;          /* Короткое имя команды */
           char                   *help_row ;           /* HELP - строка */
           char                   *help_full ;          /* HELP - полный */
            int (RSS_Module_Flyer::*process)(char *) ;  /* Процедура выполнения команды */
                               }  ;

/*--------------------------------------------- Диалоговые процедуты */

/* Файл  O_Flyer.cpp */

/* Файл  O_Flyer_dialog.cpp */
  BOOL CALLBACK  Object_Flyer_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
  BOOL CALLBACK  Object_Flyer_Create_dialog(HWND, UINT, WPARAM, LPARAM) ;

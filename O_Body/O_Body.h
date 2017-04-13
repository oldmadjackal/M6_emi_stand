
/********************************************************************/
/*								    */
/*		МОДУЛЬ УПРАВЛЕНИЯ ОБЪЕКТОМ "ТЕЛО"  		    */
/*								    */
/********************************************************************/

#ifdef O_BODY_EXPORTS
#define O_BODY_API __declspec(dllexport)
#else
#define O_BODY_API __declspec(dllimport)
#endif


/*---------------------------------------------- Параметры генерации */

/*----------------------------------- Описание класса объекта "Тело" */

  class O_BODY_API RSS_Object_Body : public RSS_Object {

    public:
                      char  model_path[FILENAME_MAX] ;        /* Файл модели */

    public:
               virtual void  vFree          (void) ;          /* Освободить ресурсы */
               virtual void  vWriteSave     (std::string *) ; /* Записать данные в строку */

	                     RSS_Object_Body() ;              /* Конструктор */
	                    ~RSS_Object_Body() ;              /* Деструктор */
                                                       } ;

/*----------------------- Описание класса управления объектом "Тело" */

  class O_BODY_API RSS_Module_Body : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Body_instr *mInstrList ;          /* Список команд */
				     
    public:
     virtual         int  vGetParameter (char *, char *) ;  /* Получить параметр */
     virtual         int  vExecuteCmd   (const char *) ;    /* Выполнить команду */
     virtual        void  vReadSave     (std::string *) ;   /* Чтение данных из строки */
     virtual        void  vWriteSave    (std::string *) ;   /* Записать данные в строку */

    public:
                     int  cHelp         (char *) ;          /* Инструкция HELP */ 
                     int  cCreate       (char *) ;          /* Инструкция CREATE */ 
                     int  cInfo         (char *) ;          /* Инструкция INFO */ 
                     int  cVisible      (char *) ;          /* Инструкция VISIBLE */
                     int  cBase         (char *) ;          /* Инструкция BASE */ 
                     int  cAngle        (char *) ;          /* Инструкция ANGLE */ 

         RSS_Object_Body *FindObject    (char *) ;          /* Поиск обьекта типа BODY по имени */
                     int  CreateObject  (RSS_Model_data *); /* Создание объекта */ 

    public:
	                  RSS_Module_Body() ;               /* Конструктор */
	                 ~RSS_Module_Body() ;               /* Деструктор */
                                                       } ;

/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_Body_instr {

           char                   *name_full ;         /* Полное имя команды */
           char                   *name_shrt ;         /* Короткое имя команды */
           char                   *help_row ;          /* HELP - строка */
           char                   *help_full ;         /* HELP - полный */
            int (RSS_Module_Body::*process)(char *) ;  /* Процедура выполнения команды */
                              }  ;

/*--------------------------------------------- Диалоговые процедуты */

/* Файл  O_Body.cpp */

/* Файл  O_Body_dialog.cpp */
  BOOL CALLBACK  Object_Body_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
  BOOL CALLBACK  Object_Body_Create_dialog(HWND, UINT, WPARAM, LPARAM) ;

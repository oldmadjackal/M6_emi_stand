
/********************************************************************/
/*                                                                  */
/*           МОДУЛЬ УПРАВЛЕНИЯ ОБЪЕКТОМ "ВНЕШНЯЯ МОДЕЛЬ"            */
/*                                                                  */
/********************************************************************/

#ifdef O_EXTERNAL_EXPORTS
#define O_EXTERNAL_API __declspec(dllexport)
#else
#define O_EXTERNAL_API __declspec(dllimport)
#endif


/*---------------------------------------------- Параметры генерации */

/*--------------- Описание класса контекста объекта "Внешняя модель" */

  class O_EXTERNAL_API RSS_Transit_External : public RSS_Transit {

    public:
             virtual   int  vExecute(void) ;             /* Исполнение действия */
                                             
    public:
                            RSS_Transit_External() ;     /* Конструктор */
                           ~RSS_Transit_External() ;     /* Деструктор */

                                                                 } ;

/*------------------------- Описание класса объекта "Внешняя модель" */

 struct RSS_Object_ExternalTrace {
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

  class O_EXTERNAL_API RSS_Object_External : public RSS_Object {

    public:
                        char  model_path[FILENAME_MAX] ;         /* Файл модели */

                      double  v_abs ;                            /* Нормальная скорость */
                      double  g_ctrl ;                           /* Нормальная траекторная перегрузка */

                         int  g_over ;                           /* Маневр с максимальной перегрузкой */

                        char  object_type[FILENAME_MAX] ;        /* Тип объекта */
                        char  iface_type[FILENAME_MAX] ;         /* Вид интерфейса: FILE */
                        char  iface_file_folder[FILENAME_MAX] ;  /* Папка для размещения обменных файлов */
                        char  iface_file_targets[FILENAME_MAX] ; /* Имя файла целей */
                        char  iface_file_control[FILENAME_MAX] ; /* Имя для входного/выходного файлов данных, расширение - in и out */

   private:

    RSS_Object_ExternalTrace *mTrace ;                           /* Траектория */
                         int  mTrace_cnt ;  
                         int  mTrace_max ;
                    COLORREF  mTrace_color ;
                         int  mTrace_dlist ;

    public:
               virtual void  vFree          (void) ;            /* Освободить ресурсы */
               virtual void  vPush          (void) ;            /* Сохранить состояние объекта */
               virtual void  vPop           (void) ;            /* Восстановить состояние объекта */
               virtual void  vWriteSave     (std::string *) ;   /* Записать данные в строку */
               virtual  int  vCalculateStart(double) ;          /* Подготовка расчета изменения состояния */
               virtual  int  vCalculate     (double, double,    /* Расчет изменения состояния */
                                                     char *, int) ;
               virtual  int  vCalculateExt1 (double, double,
                                                     char *, int) ;
               virtual  int  vCalculateExt2 (double, double,
                                                     char *, int) ;
               virtual  int  vCalculateShow (double, double) ;  /* Отображение результата расчета изменения состояния */
               virtual  int  vEvent         (char *, double) ;  /* Обработка событий */
               virtual  int  vSpecial       (char *, void *) ;  /* Специальные действия */
                        int  iSaveTracePoint(char *) ;          /* Сохранение точки траектории */
                       void  iShowTrace_    (void) ;            /* Отображение траектории с передачей контекста */
                       void  iShowTrace     (void) ;            /* Отображение траектории */

	                     RSS_Object_External() ;             /* Конструктор */
	                    ~RSS_Object_External() ;             /* Деструктор */
                                                        } ;

/*------------- Описание класса управления объектом "Внешняя модель" */

  class O_EXTERNAL_API RSS_Module_External : public RSS_Kernel {

    public:

     static
      struct RSS_Module_External_instr *mInstrList ;        /* Список команд */

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
                     int  cOwner        (char *) ;                     /* Инструкция OWNER */ 
                     int  cTarget       (char *) ;                     /* Инструкция TARGET */ 
                     int  cBase         (char *) ;                     /* Инструкция BASE */ 
                     int  cAngle        (char *) ;                     /* Инструкция ANGLE */ 
                     int  cVelocity     (char *) ;                     /* Инструкция VELOCITY */
                     int  cControl      (char *) ;                     /* Инструкция CONTROL */
                     int  cTrace        (char *) ;                     /* Инструкция TRACE */
                     int  cIType        (char *) ;                     /* Инструкция ITYPE */
                     int  cIFile        (char *) ;                     /* Инструкция IFILE */

              RSS_Object *FindObject    (char *, int) ;                /* Поиск обьекта типа EXTERNAL по имени */

    public:
	                  RSS_Module_External() ;              /* Конструктор */
	                 ~RSS_Module_External() ;              /* Деструктор */
                                                       } ;

/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_External_instr {

           char                       *name_full ;          /* Полное имя команды */
           char                       *name_shrt ;          /* Короткое имя команды */
           char                       *help_row ;           /* HELP - строка */
           char                       *help_full ;          /* HELP - полный */
            int (RSS_Module_External::*process)(char *) ;   /* Процедура выполнения команды */
                                 }  ;

/*--------------------------------------------- Диалоговые процедуты */

/* Файл  O_External.cpp */

/* Файл  O_External_dialog.cpp */
  INT_PTR CALLBACK  Object_External_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Object_External_Create_dialog(HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Object_External_Iface_dialog (HWND, UINT, WPARAM, LPARAM) ;


/********************************************************************/
/*								    */
/*		МОДУЛЬ УПРАВЛЕНИЯ ОБЪЕКТОМ "МАРКЕР"  		    */
/*								    */
/********************************************************************/

#ifdef O_MARKER_EXPORTS
#define O_MARKER_API __declspec(dllexport)
#else
#define O_MARKER_API __declspec(dllimport)
#endif


/*---------------------------------------------- Параметры генерации */

/*--------------------------------- Описание класса объекта "Маркер" */

  class O_MARKER_API RSS_Object_Marker : public RSS_Object {

    public:
                    double  x_base ;                    /* Координаты базовой точки */
                    double  y_base ;
                    double  z_base ;

                    double  a_azim ;                    /* Углы ориентации */
                    double  a_elev ;
                    double  a_roll ;

                      char  model[64] ;                 /* Модель маркера */
                  COLORREF  color ;                     /* Цвет маркер */
                       int  dlist1_idx ;                /* Индексы дисплейных списков */
                       int  dlist2_idx ;
                    double  size ;                      /* Коэф.размера маркера */   

    public:
               virtual  int  vGetBasePoint   (RSS_Point *) ;         /* Работа с базовой точкой */
               virtual  int  vGetTarget      (char *, RSS_Point *) ; /* Выдать координаты 'целевой' точки */
               virtual void  vWriteSave      (std::string *) ;       /* Записать данные в строку */
               virtual  int  vListControlPars(RSS_ControlPar *) ;    /* Получить список параметров управления */
               virtual  int  vSetControlPar  (RSS_ControlPar *) ;    /* Установить значение параметра управления */

	                     RSS_Object_Marker() ;                   /* Конструктор */
	                    ~RSS_Object_Marker() ;                   /* Деструктор */
                                                           } ;

/*--------------------- Описание класса управления объектом "Маркер" */

  class O_MARKER_API RSS_Module_Marker : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Marker_instr *mInstrList ;           /* Список команд */

                   RSS_Object_Marker *mContextObject ;       /* Обьект 'контекстной' операции */
                                char  mContextAction[64] ;   /* Действие 'контекстной' операции */
                          
                              double  mSize ;                /* Коэф.размера маркера */   

    public:
     virtual        void  vShow          (char *) ;          /* Отобразить связанные данные */
     virtual         int  vExecuteCmd    (const char *) ;    /* Выполнить команду */
     virtual        void  vReadSave      (std::string *) ;   /* Чтение данных из строки */
     virtual        void  vWriteSave     (std::string *) ;   /* Записать данные в строку */
     virtual        void  vChangeContext (void)  ;           /* Выполнить действие в контексте потока */

    public:
                     int  cHelp       (char *) ;             /* Инструкция HELP */ 
                     int  cCreate     (char *) ;             /* Инструкция CREATE */ 
                     int  cInfo       (char *) ;             /* Инструкция INFO */ 
                     int  cBase       (char *) ;             /* Инструкция BASE */ 
                     int  cAngle      (char *) ;             /* Инструкция ANGLE */ 

       RSS_Object_Marker *FindObject  (char *) ;             /* Поиск обьекта типа MARKER по имени */
                     int  CreateObject(RSS_Model_data *);    /* Создание объекта */ 
                     int  FormMarker  (RSS_Object_Marker *); /* Задание формы маркера */
                     int  FormMarker_ (RSS_Object_Marker *); /* Задание формы маркера с передачей контекста */
                     int  PlaceMarker (RSS_Object_Marker *); /* Позиционирование маркера */
                     int  PlaceMarker_(RSS_Object_Marker *); /* Позиционирование маркера с передачей контекста */

    public:
	                  RSS_Module_Marker() ;              /* Конструктор */
	                 ~RSS_Module_Marker() ;              /* Деструктор */
                                                           } ;

/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_Marker_instr {

           char                     *name_full ;         /* Полное имя команды */
           char                     *name_shrt ;         /* Короткое имя команды */
           char                     *help_row ;          /* HELP - строка */
           char                     *help_full ;         /* HELP - полный */
            int (RSS_Module_Marker::*process)(char *) ;  /* Процедура выполнения команды */
                                }  ;

/*--------------------------------------------- Диалоговые процедуты */

/* Файл  O_Marker.cpp */

/* Файл  O_Marker_dialog.cpp */
  BOOL CALLBACK  Object_Marker_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;


/********************************************************************/
/*								    */
/*		МОДУЛЬ УПРАВЛЕНИЯ ОБЪЕКТОМ "РАКЕТА-ЛЕГО"            */
/*								    */
/********************************************************************/

#ifdef O_MISSILE_LEGO_EXPORTS
#define O_MISSILE_LEGO_API __declspec(dllexport)
#else
#define O_MISSILE_LEGO_API __declspec(dllimport)
#endif


/*---------------------------------------------- Параметры генерации */

/*------------------ Описание класса контекста объекта "Ракета-Лего" */

  class O_MISSILE_LEGO_API RSS_Transit_MissileLego : public RSS_Transit {

    public:
             virtual   int  vExecute(void) ;             /* Исполнение действия */
                                             
    public:
                            RSS_Transit_MissileLego() ;   /* Конструктор */
                           ~RSS_Transit_MissileLego() ;   /* Деструктор */

                                                               } ;

/*---------------------------- Описание класса объекта "Ракета-Лего" */

 struct RSS_Object_MissileLegoTrace {
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

  class O_MISSILE_LEGO_API RSS_Object_MissileLego : public RSS_Object {

    public:
                      char   model_path[FILENAME_MAX] ;        /* Файл модели */

          RSS_Unit_WarHead  *unit_warhead ;                    /* Компоненты */
           RSS_Unit_Homing  *unit_homing ;
           RSS_Unit_Engine  *unit_engine ;
          RSS_Unit_Control  *unit_control ;
            RSS_Unit_Model  *unit_model ;

                RSS_Object **mSpawn ;                          /* Список клонов при эмитации залпа */
                       int   mSpawn_cnt ;

                      HWND  hDropsViewWnd ;                    /* Окно распределения точек падения/срабатывания */

   private:

    RSS_Object_MissileLegoTrace *mTrace ;                      /* Траектория */
                            int  mTrace_cnt ;  
                            int  mTrace_max ;
                       COLORREF  mTrace_color ;
                            int  mTrace_dlist ;

    public:
         virtual       void  vFree          (void) ;            /* Освободить ресурсы */
         virtual RSS_Object *vCopy          (char *) ;          /* Копировать объект */
         virtual       void  vWriteSave     (std::string *) ;   /* Записать данные в строку */
         virtual        int  vCalculateStart(double) ;          /* Подготовка расчета изменения состояния */
         virtual        int  vCalculate     (double, double,    /* Расчет изменения состояния */
                                                     char *, int) ;
         virtual        int  vCalculateShow (double, double) ;  /* Отображение результата расчета изменения состояния */
         virtual        int  vSpecial       (char *, void *) ;  /* Специальные действия */
                        int  iSaveTracePoint(char *) ;          /* Сохранение точки траектории */
                       void  iShowTrace_    (char *) ;          /* Отображение траектории с передачей контекста */
                       void  iShowTrace     (char *) ;          /* Отображение траектории */
                       void  iClearSpawn    (void) ;            /* Освобождение ресурсов моделирования залпа */

	                     RSS_Object_MissileLego() ;          /* Конструктор */
	                    ~RSS_Object_MissileLego() ;          /* Деструктор */
                                                        } ;

/*---------------- Описание класса управления объектом "Ракета-Лего" */

  class O_MISSILE_LEGO_API RSS_Module_MissileLego : public RSS_Kernel {

    public:

     static
      struct RSS_Module_MissileLego_instr *mInstrList ;      /* Список команд */

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
                     int  cLego         (char *) ;                     /* Инструкция LEGO */ 
                     int  cTrace        (char *) ;                     /* Инструкция TRACE */
                     int  cSpawn        (char *) ;                     /* Инструкция SPAWN */
                     int  cStat         (char *) ;                     /* Инструкция STAT */

              RSS_Object *FindObject    (char *, int) ;                /* Поиск обьекта по имени */
          class RSS_Unit *AddUnit       (RSS_Object_MissileLego *,     /* Добавление компонента к объекту */
                                             char *, char *, char * ) ;

    public:
	                  RSS_Module_MissileLego() ;              /* Конструктор */
	                 ~RSS_Module_MissileLego() ;              /* Деструктор */
                                                       } ;

/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_MissileLego_instr {

        char                          *name_full ;          /* Полное имя команды */
        char                          *name_shrt ;          /* Короткое имя команды */
        char                          *help_row ;           /* HELP - строка */
        char                          *help_full ;          /* HELP - полный */
         int (RSS_Module_MissileLego::*process)(char *) ;   /* Процедура выполнения команды */
                                 }  ;

/*--------------------------------------------- Диалоговые процедуты */

/* Файл  O_Missile_Lego.cpp */

/* Файл  O_Missile_Lego_dialog.cpp */
  INT_PTR CALLBACK  Object_MissileLego_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Object_MissileLego_Create_dialog(HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Object_MissileLego_Lego_dialog  (HWND, UINT, WPARAM, LPARAM) ;

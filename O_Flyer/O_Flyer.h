
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

/*------------------------------------ Описание программы управления */

 struct RSS_Object_FlyerPFrame {
                                     char  used[128] ;
                                   double  t ;
                                   double  x, y, z ;
                                   double  a, d_a, t_a ;
                                   double  e, d_e, t_e ;
                                   double  r, d_r, t_r ;
                                   double  g, d_g, t_g ;
                                   double  v, d_v, t_v ;
                               } ;

#define  _PFRAMES_MAX   100
 class RSS_Object_FlyerProgram {

    public:
                                  char  name[128] ;
                RSS_Object_FlyerPFrame  frames[_PFRAMES_MAX] ;
                                   int  frames_cnt ;

    public:
                            RSS_Object_FlyerProgram() ;        /* Конструктор */
                           ~RSS_Object_FlyerProgram() ;        /* Деструктор */
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

                    double  v_abs ;                           /* Нормальная скорость */
                    double  g_ctrl ;                          /* Нормальная траекторная перегрузка */

#define   _PROGRAMS_MAX  10
   RSS_Object_FlyerProgram *programs[_PROGRAMS_MAX] ;         /* Список программ управления */
   RSS_Object_FlyerProgram *program ;                         /* Рабочая программа управления */
                       int  p_frame ;

    RSS_Object_FlyerPFrame  p_controls ;                      /* Действующие программные параметры */

   private:
                    double  r_ctrl ;                          /* Параметры плоскости поворота */
            class Matrix2d *m_ctrl ;
                    double  a_ctrl ;

     RSS_Object_FlyerTrace *mTrace ;                          /* Траектория */
                       int  mTrace_cnt ;  
                       int  mTrace_max ;
                  COLORREF  mTrace_color ;
                       int  mTrace_dlist ;

    public:
               virtual void  vFree          (void) ;            /* Освободить ресурсы */
               virtual void  vWriteSave     (std::string *) ;   /* Записать данные в строку */
               virtual  int  vCalculate     (double, double) ;  /* Расчет изменения состояния */
                        int  iExecuteProgram(double, double) ;  /* Отработка программного управления */
                        int  iSaveTracePoint(char *) ;          /* Сохранение точки траектории */
                       void  iShowTrace_    (void) ;            /* Отображение траектории с передачей контекста */
                       void  iShowTrace     (void) ;            /* Отображение траектории */

	                     RSS_Object_Flyer() ;               /* Конструктор */
	                    ~RSS_Object_Flyer() ;               /* Деструктор */
                                                        } ;

/*---------------------- Описание класса управления объектом "Летун" */

  class O_FLYER_API RSS_Module_Flyer : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Flyer_instr *mInstrList ;           /* Список команд */

                  double  g_step ;                          /* Шаг изменения траекторной перегрузки */
                  double  a_step ;                          /* Шаг изменения углов */
				     
    public:
     virtual         int  vExecuteCmd   (const char *) ;    /* Выполнить команду */
     virtual        void  vReadSave     (std::string *) ;   /* Чтение данных из строки */
     virtual        void  vWriteSave    (std::string *) ;   /* Записать данные в строку */

    public:
                     int  cHelp         (char *) ;                     /* Инструкция HELP */ 
                     int  cCreate       (char *) ;                     /* Инструкция CREATE */ 
                     int  cInfo         (char *) ;                     /* Инструкция INFO */ 
                     int  cBase         (char *) ;                     /* Инструкция BASE */ 
                     int  cAngle        (char *) ;                     /* Инструкция ANGLE */ 
                     int  cVelocity     (char *) ;                     /* Инструкция VELOCITY */
                     int  cControl      (char *) ;                     /* Инструкция CONTROL */
                     int  cProgram      (char *) ;                     /* Инструкция PROGRAM */
                     int  cTrace        (char *) ;                     /* Инструкция TRACE */

        RSS_Object_Flyer *FindObject    (char *) ;                     /* Поиск обьекта типа BODY по имени */
                     int  CreateObject  (RSS_Model_data *) ;           /* Создание объекта */ 
                     int  iReadProgram  (RSS_Object_Flyer *, char *) ; /* Считывание файла описания программы */

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

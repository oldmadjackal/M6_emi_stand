/********************************************************************/
/*                                                                  */
/*               МОДУЛЬ УПРАВЛЕНИЯ ВИДИМОСТЬЮ ОБЪЕКТОВ              */
/*                                                                  */
/********************************************************************/

#ifdef F_VISIBILITY_EXPORTS
#define F_VISIBILITY_API __declspec(dllexport)
#else
#define F_VISIBILITY_API __declspec(dllimport)
#endif

/*---------------------------------------------- Параметры генерации */

/*----------------- Описание класса управления свойством "Видимость" */

  class F_VISIBILITY_API RSS_Module_Visibility : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Visibility_instr *mInstrList ;          /* Список команд */
				     
    public:
     virtual         int  vExecuteCmd   (const char *) ;    /* Выполнить команду */

    public:
                     int  cHelp         (char *) ;          /* Инструкция Help */
                     int  cAdd          (char *) ;          /* Инструкция Add */
                     int  cView         (char *) ;          /* Инструкция View */
                     int  cLook         (char *) ;          /* Инструкция Look */

    public:
     virtual        void  vStart        (void) ;            /* Стартовая разводка */
     virtual RSS_Feature *vCreateFeature(RSS_Object *,      /* Создать свойство */
                                         RSS_Feature *) ;
     virtual        void  vWriteSave    (std::string *) ;   /* Записать данные в строку */
     virtual        void  vReadSave     (std::string *) ;   /* Считать данные из строки */

              RSS_Object *FindObject    (char *, class RSS_Feature_Visibility **) ;

	                  RSS_Module_Visibility() ;              /* Конструктор */
	                 ~RSS_Module_Visibility() ;              /* Деструктор */
                                                            } ;

/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_Visibility_instr {

           char                         *name_full ;         /* Полное имя команды */
           char                         *name_shrt ;         /* Короткое имя команды */
           char                         *help_row ;          /* HELP - строка */
           char                         *help_full ;         /* HELP - полный */
            int (RSS_Module_Visibility::*process)(char *) ;  /* Процедура выполнения команды */
                                    }  ;

/*------------------- Описание класса контекста свойства "Видимость" */

  class F_VISIBILITY_API RSS_Transit_Visibility : public RSS_Transit {

    public:
             virtual   int  vExecute(void) ;             /* Исполнение действия */
                                             
    public:
                            RSS_Transit_Visibility() ;        /* Конструктор */
                           ~RSS_Transit_Visibility() ;        /* Деструктор */

                                                           } ;

/*---------------------------------------------- Компоненты описания */

 typedef struct {                                            /* Описание габаритного прямоугольника */
                             double  x_min ;
                             double  y_min ;
                             double  z_min ;
                             double  x_max ;
                             double  y_max ;
                             double  z_max ;
                }  RSS_Feature_Visibility_Dim ;

 typedef struct {                                            /* Описание точки */
                    double  x ;                               /* X - значение */
                      char *x_formula ;                       /* X - рассчетное выражение */
                      void *x_calculate ;                     /* X - контекст вычислителя */

                    double  y ;                               /* Y - значение */
                      char *y_formula ;                       /* Y - рассчетное выражение */
                      void *y_calculate ;                     /* Y - контекст вычислителя */

                    double  z ;                               /* Z - значение */
                      char *z_formula ;                       /* Z - рассчетное выражение */
                      void *z_calculate ;                     /* Z - контекст вычислителя */

                    double  x_abs ;                           /* Координаты в абсолютной СК */
                    double  y_abs ;
                    double  z_abs ;
                }  RSS_Feature_Visibility_Vertex ;

#define  _VERTEX_PER_FACET_MAX   32

 typedef struct {                                            /* Описание грани */

                       int  vertexes[_VERTEX_PER_FACET_MAX] ; /* Список точек */
                       int  vertexes_cnt ;

     RSS_Feature_Visibility_Dim  overall ;                    /* Габарит грани */

                }  RSS_Feature_Visibility_Facet ;

 typedef struct {                                            /* Описание тела */
                               char  name[128] ;              /* Название */
               struct RSS_Parameter *extrn_pars ;             /* Ссылак на параметры */

      RSS_Feature_Visibility_Vertex *Vertexes ;               /* Список вершин */
                                int  Vertexes_cnt ; 

         RSS_Feature_Visibility_Dim  overall ;                /* Габарит тела */

                                int  recalc ;                 /* Флаг необходимости перерасчета точек тела */

                             double  x_base ;                 /* Координаты базовой точки */
                             double  y_base ;
                             double  z_base  ;
                             double  x_base_s ;
                             double  y_base_s ;
                             double  z_base_s ;

                             double  a_azim ;                 /* Углы ориентации */
                             double  a_elev ;
                             double  a_roll  ;
                             double  a_azim_s ;
                             double  a_elev_s ;
                             double  a_roll_s ;

                             double  Matrix[4][4] ;           /* Матрица положения звена */
                                int  Matrix_flag ;            /* Флаг использования... */
      
                                int  list_idx ;               /* Индекс дисплейного списка */
                }  RSS_Feature_Visibility_Body ;

 typedef struct {                                            /* Описание точки наблюдения */
                       char  name[128] ;

                     double  x_base ;                         /* Координаты базовой точки */
                     double  y_base ;
                     double  z_base ;

                     double  a_azim ;                         /* Углы ориентации */
                     double  a_elev ;
                     double  a_azim_min ;                     /* Диапазоны изменения углов ориентации */
                     double  a_azim_max ;
                     double  a_elev_min ;
                     double  a_elev_max ;
                        int  s_azim ;                         /* Признак стабилизированного поля зрения */
                        int  s_elev ;

                     double  v_azim ;                         /* Углы поля зрения */
                     double  v_elev ;
                     double  range ;                          /* Дальность наблюдения */

                }  RSS_Feature_Visibility_Observer ;

/*----------------------------- Описание класса свойства "Видимость" */

  class F_VISIBILITY_API RSS_Feature_Visibility : public RSS_Feature {

    public:       
                      static    int  *hit_count ;                   /* Счетчик поражения */
                                int   hit_done ;                    /* Признак поражения */

    public:       
        RSS_Feature_Visibility_Body  *Bodies ;                      /* Список тел */
                                int   Bodies_cnt ;

               struct RSS_Parameter  *Pars_work ;

         RSS_Feature_Visibility_Dim   overall ;                     /* Габарит объекта */

    RSS_Feature_Visibility_Observer  *Observers ;                   /* Список точек налюдения */
                                int   Observers_cnt ;

                        RSS_Transit  *Context ;                     /* Интерфейс передачи контекстов */

                           COLORREF  Sector_color ;
                                int  Sector_dlist ;

    public:
            virtual void  vReadSave     (char *, std::string *,     /* Считать данные из строки */
                                                        char * ) ;
            virtual void  vGetInfo      (std::string *) ;           /* Выдать информацию о свойстве */
            virtual  int  vResetCheck   (void *) ;                  /* Сброс контекста проверки непротиворечивости свойства */  
            virtual  int  vPreCheck     (void *) ;                  /* Подготовка к проверке непротиворечивости свойства */  
            virtual  int  vCheck        (void *,                    /* Проверка непротиворечивости свойства */  
                                         RSS_Objects_List *) ;
            virtual  int  vShow         (void *) ;                  /* Отображение данных по свойству */ 
            virtual void  vBodyDelete   (char *) ;                  /* Удалить тело */
            virtual void  vBodyBasePoint(char *,                    /* Задание базовой точки тела */
                                         double, double, double) ;
            virtual void  vBodyAngles   (char *,                    /* Задание ориентации тела */
                                         double, double, double) ;
            virtual void  vBodyMatrix   (char *, double[4][4]) ;    /* Задание матрицы положения тела */
            virtual void  vBodyShifts   (char *,                    /* Задание смещения положения и  */
                                          double, double, double,   /*    ориентации тела            */
                                          double, double, double ) ;
            virtual void  vBodyPars     (char *,                    /* Задание списка параметров */
                                             struct RSS_Parameter *) ;

                     int  RecalcPoints  (void) ;                    /* Перерасчет точек тел объекта */
                    void  iShow         (void) ;                    /* Отображение данных по свойству */
              RSS_Kernel *iGetCalculator(void) ;                    /* Определение нужного вычислителя */
                     int  iOverallTest  (RSS_Feature_Visibility *) ;       /* Проверка пересечение "габаритов" трасс */
                     int  iFacetsTest   (RSS_Feature_Visibility_Body *) ;  /* Проверка пересечения граней */
                     int  iToFlat       (RSS_Feature_Visibility_Vertex *,  /* Расчет матрицы разворота плоскости x0z в плоскость точек P0-P1-P2 */
                                         RSS_Feature_Visibility_Vertex *,
                                         RSS_Feature_Visibility_Vertex *,
                                                              Matrix2d * ) ;

	                  RSS_Feature_Visibility() ;                     /* Конструктор */
	                 ~RSS_Feature_Visibility() ;                     /* Деструктор */
                                                              } ;

/*--------------------------------------------- Диалоговые процедуры */

/* Файл  F_Visibility.cpp */

/* Файл  F_Visibility_dialog.cpp */
  INT_PTR CALLBACK  Feature_Visibility_Help_dialog(HWND, UINT, WPARAM, LPARAM) ;

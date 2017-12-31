/********************************************************************/
/*								    */
/*	МОДУЛЬ УПРАВЛЕНИЯ ОБНАРУЖЕНИЕМ ПОРАЖЕНИЯ ОБЪЕКТОВ	    */
/*								    */
/********************************************************************/

#ifdef F_HIT_EXPORTS
#define F_HIT_API __declspec(dllexport)
#else
#define F_HIT_API __declspec(dllimport)
#endif

/*---------------------------------------------- Параметры генерации */

/*---------------- Описание класса управления объектом "Пересечение" */

  class F_HIT_API RSS_Module_Hit : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Hit_instr *mInstrList ;          /* Список команд */
				     
    public:
     virtual         int  vExecuteCmd   (const char *) ;    /* Выполнить команду */

    public:
                     int  cHelp         (char *) ;          /* Инструкция Help */
                     int  cRadius       (char *) ;          /* Инструкция Radius */
                     int  cCategory     (char *) ;          /* Инструкция Category */

    public:
     virtual        void  vStart        (void) ;            /* Стартовая разводка */
     virtual RSS_Feature *vCreateFeature(RSS_Object *,      /* Создать свойство */
                                         RSS_Feature *) ;
     virtual        void  vWriteSave    (std::string *) ;   /* Записать данные в строку */
     virtual        void  vReadSave     (std::string *) ;   /* Считать данные из строки */

	                  RSS_Module_Hit() ;              /* Конструктор */
	                 ~RSS_Module_Hit() ;              /* Деструктор */
                                                       } ;

/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_Hit_instr {

           char                  *name_full ;         /* Полное имя команды */
           char                  *name_shrt ;         /* Короткое имя команды */
           char                  *help_row ;          /* HELP - строка */
           char                  *help_full ;         /* HELP - полный */
            int (RSS_Module_Hit::*process)(char *) ;  /* Процедура выполнения команды */
                               }  ;

/*---------------------------------------------- Компоненты описания */

 typedef struct {                                            /* Описание габаритного прямоугольника */
                             double  x_min ;
                             double  y_min ;
                             double  z_min ;
                             double  x_max ;
                             double  y_max ;
                             double  z_max ;
                }  RSS_Feature_Hit_Dim ;

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
                }  RSS_Feature_Hit_Vertex ;

#define  _VERTEX_PER_FACET_MAX   32

 typedef struct {                                            /* Описание грани */

                       int  vertexes[_VERTEX_PER_FACET_MAX] ; /* Список точек */
                       int  vertexes_cnt ;

     RSS_Feature_Hit_Dim  overall ;                         /* Габарит грани */

                }  RSS_Feature_Hit_Facet ;

 typedef struct {                                            /* Описание тела */
                               char  name[128] ;              /* Название */
               struct RSS_Parameter *extrn_pars ;             /* Ссылак на параметры */

             RSS_Feature_Hit_Vertex *Vertexes ;               /* Список вершин */
                                int  Vertexes_cnt ; 
              RSS_Feature_Hit_Facet *Facets ;                 /* Список вершин */
                                int  Facets_cnt ;

                RSS_Feature_Hit_Dim  overall ;                /* Габарит тела */

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
                }  RSS_Feature_Hit_Body ;

/*----------------------------- Описание класса свойства "Поражение" */

  class F_HIT_API RSS_Feature_Hit : public RSS_Feature {

    public:       
                      static    int  *hit_count ;                   /* Счетчик поражения */
                                int   hit_done ;                    /* Признак поражения */

    public:       
               RSS_Feature_Hit_Body  *Bodies ;                      /* Список тел */
                                int   Bodies_cnt ;

               struct RSS_Parameter  *Pars_work ;

                RSS_Feature_Hit_Dim   overall ;                     /* Габарит объекта */
                             double   hit_range ;                   /* Радиус поражения */

                                int   any_target ;                  /* Произвольно поражаемая цель */
                                int   any_weapon ;                  /* Произвольно поражающее оружие */

                               char   target[128] ;                 /* Объект-цель */

                          RSS_Point   track_s ;                     /* Собственное положение текущее */
                          RSS_Point   track_s_prv ;                 /* Собственное положение предыдущее */
                                int   track_flag ;                  /* Флаг возможности проверки */

    public:
            virtual void  vReadSave     (char *, std::string *,     /* Считать данные из строки */
                                                        char * ) ;
            virtual void  vGetInfo      (std::string *) ;           /* Выдать информацию о свойстве */
            virtual  int  vResetCheck   (void *) ;                  /* Сброс контекста проверки непротиворечивости свойства */  
            virtual  int  vPreCheck     (void *) ;                  /* Подготовка к проверке непротиворечивости свойства */  
            virtual  int  vCheck        (void *,                    /* Проверка непротиворечивости свойства */  
                                         RSS_Objects_List *) ;
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
              RSS_Kernel *iGetCalculator(void) ;                    /* Определение нужного вычислителя */
                     int  iOverallTest  (RSS_Feature_Hit *) ;       /* Проверка пересечение "габаритов" трасс */
                     int  iFacetsTest   (RSS_Feature_Hit_Body *) ;  /* Проверка пересечения граней */
                     int  iToFlat       (RSS_Feature_Hit_Vertex *,  /* Расчет матрицы разворота плоскости x0z в плоскость точек P0-P1-P2 */
                                         RSS_Feature_Hit_Vertex *,
                                         RSS_Feature_Hit_Vertex *,
                                                       Matrix2d * ) ;

	                  RSS_Feature_Hit() ;                     /* Конструктор */
	                 ~RSS_Feature_Hit() ;                     /* Деструктор */
                                                           } ;

/*--------------------------------------------- Диалоговые процедуры */

/* Файл  F_Hit.cpp */

/* Файл  F_Hit_dialog.cpp */
  BOOL CALLBACK  Feature_Hit_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;

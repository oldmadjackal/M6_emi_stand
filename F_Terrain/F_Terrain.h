/********************************************************************/
/*                                                                  */
/*        МОДУЛЬ УПРАВЛЕНИЯ ПРИВЯЗКОЙ К РЕЛЬЕФУ МЕСТНОСТИ           */
/*                                                                  */
/********************************************************************/

#ifdef F_TERRAIN_EXPORTS
#define F_TERRAIN_API __declspec(dllexport)
#else
#define F_TERRAIN_API __declspec(dllimport)
#endif

/*---------------------------------------------- Параметры генерации */

/*---------------- Описание класса управления объектом "Пересечение" */

  class F_TERRAIN_API RSS_Module_Terrain : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Terrain_instr *mInstrList ;          /* Список команд */
				     
    public:
     virtual         int  vExecuteCmd   (const char *) ;    /* Выполнить команду */

    public:
                     int  cHelp         (char *) ;          /* Инструкция Help */
                     int  cSlave        (char *) ;          /* Инструкция Slave */
                     int  cFlat         (char *) ;          /* Инструкция Flat */
                     int  cLink         (char *) ;          /* Инструкция Link */
                     int  cScan         (char *) ;          /* Инструкция Scan */
                     int  cUp           (char *) ;          /* Инструкция Up */
              RSS_Object *FindObject    (char *, int) ;     /* Поиск обьекта ландшафта по имени */

    public:
     virtual        void  vStart        (void) ;            /* Стартовая разводка */
     virtual RSS_Feature *vCreateFeature(RSS_Object *,      /* Создать свойство */
                                         RSS_Feature *) ;
     virtual        void  vWriteSave    (std::string *) ;   /* Записать данные в строку */
     virtual        void  vReadSave     (std::string *) ;   /* Считать данные из строки */

	                  RSS_Module_Terrain() ;              /* Конструктор */
	                 ~RSS_Module_Terrain() ;              /* Деструктор */
                                                            } ;

/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_Terrain_instr {

           char                         *name_full ;         /* Полное имя команды */
           char                         *name_shrt ;         /* Короткое имя команды */
           char                         *help_row ;          /* HELP - строка */
           char                         *help_full ;         /* HELP - полный */
            int (RSS_Module_Terrain::*process)(char *) ;  /* Процедура выполнения команды */
                                    }  ;

/*---------------------------------------------- Компоненты описания */

 typedef struct {                                            /* Описание габаритного прямоугольника */
                             double  x_min ;
                             double  y_min ;
                             double  z_min ;
                             double  x_max ;
                             double  y_max ;
                             double  z_max ;
                }  RSS_Feature_Terrain_Dim ;

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
                }  RSS_Feature_Terrain_Vertex ;

#define  _VERTEX_PER_FACET_MAX   32

 typedef struct {                                            /* Описание грани */

                       int  vertexes[_VERTEX_PER_FACET_MAX] ; /* Список точек */
                       int  vertexes_cnt ;
                    double  a ;                               /* Коеффициенты уравнения плоскости: Ax+By+Cz+D=0 */
                    double  b ;
                    double  c ;
                    double  d ;
                       int  abcd_formed ;

     RSS_Feature_Terrain_Dim  overall ;                    /* Габарит грани */

                }  RSS_Feature_Terrain_Facet ;

 typedef struct {                                            /* Описание тела */
                               char  name[128] ;              /* Название */
               struct RSS_Parameter *extrn_pars ;             /* Ссылак на параметры */

         RSS_Feature_Terrain_Vertex *Vertexes ;               /* Список вершин */
                                int  Vertexes_cnt ; 
          RSS_Feature_Terrain_Facet *Facets ;                 /* Список вершин */
                                int  Facets_cnt ;

            RSS_Feature_Terrain_Dim  overall ;                /* Габарит тела */

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
                }  RSS_Feature_Terrain_Body ;

/*----------------------------- Описание класса свойства "Поражение" */

  class F_TERRAIN_API RSS_Feature_Terrain : public RSS_Feature {

    public:       
                RSS_Feature_Terrain  *terrain_last ;                /* Последний связанный участок местности */
                                int   terrain_body ;                /* ... индекс тела */
                                int   terrain_facet ;               /* ... индекс грани */
                                int   slave ;                       /* Признак следования по рельефу */

                               char   link_object[128] ;            /* Связанный объект-маркер */

    public:       
           RSS_Feature_Terrain_Body  *Bodies ;                      /* Список тел */
                                int   Bodies_cnt ;

               struct RSS_Parameter  *Pars_work ;

            RSS_Feature_Terrain_Dim   overall ;                     /* Габарит объекта */

                          RSS_Point   track_s ;                     /* Собственное положение текущее */
                          RSS_Point   track_s_prv ;                 /* Собственное положение предыдущее */
                                int   track_flag ;                  /* Флаг возможности проверки */

    public:
            virtual void  vReadSave     (char *, std::string *,          /* Считать данные из строки */
                                                        char * ) ;
            virtual void  vGetInfo      (std::string *) ;                /* Выдать информацию о свойстве */
            virtual  int  vResetCheck   (void *) ;                       /* Сброс контекста проверки непротиворечивости свойства */  
            virtual  int  vPreCheck     (void *) ;                       /* Подготовка к проверке непротиворечивости свойства */  
            virtual  int  vCheck        (void *,                         /* Проверка непротиворечивости свойства */  
                                         RSS_Objects_List *) ;
            virtual void  vBodyDelete   (char *) ;                       /* Удалить тело */
            virtual void  vBodyBasePoint(char *,                         /* Задание базовой точки тела */
                                         double, double, double) ;
            virtual void  vBodyAngles   (char *,                         /* Задание ориентации тела */
                                         double, double, double) ;
            virtual void  vBodyMatrix   (char *, double[4][4]) ;         /* Задание матрицы положения тела */
            virtual void  vBodyShifts   (char *,                         /* Задание смещения положения и  */
                                          double, double, double,        /*    ориентации тела            */
                                          double, double, double ) ;
            virtual void  vBodyPars     (char *,                         /* Задание списка параметров */
                                             struct RSS_Parameter *) ;

                     int  RecalcPoints  (void) ;                         /* Перерасчет точек тел объекта */
              RSS_Kernel *iGetCalculator(void) ;                         /* Определение нужного вычислителя */
                     int  iOverallTest  (RSS_Feature_Terrain *) ;        /* Проверка попадание точки в габарит участка */
                     int  iFacetTest    (RSS_Feature_Terrain *,          /* Проверка попадания точки внутрь элемента участка */
                                                           int, int) ;
                     int  iPlaceObject  (RSS_Feature_Terrain *,          /* Расчет положения объекта по привязке к элементу местности */
                                                           int, int) ;
                     int  iPlaneByPoints(RSS_Feature_Terrain_Facet *,    /* Определение канонического уровнения плоскости по 3 точкам */
                                         RSS_Feature_Terrain_Vertex *,
                                         RSS_Feature_Terrain_Vertex *,
                                         RSS_Feature_Terrain_Vertex * ) ;

	                  RSS_Feature_Terrain() ;                          /* Конструктор */
	                 ~RSS_Feature_Terrain() ;                          /* Деструктор */
                                                              } ;

/*--------------------------------------------- Диалоговые процедуры */

/* Файл  F_Terrain.cpp */

/* Файл  F_Terrain_dialog.cpp */
  INT_PTR CALLBACK  Feature_Terrain_Help_dialog(HWND, UINT, WPARAM, LPARAM) ;
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
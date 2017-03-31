/********************************************************************/
/*								    */
/*		МОДУЛЬ УПРАВЛЕНИЯ ОТОБРАЖЕНИЕМ ОБЪЕКТОВ	            */
/*								    */
/********************************************************************/

#ifdef F_SHOW_EXPORTS
#define F_SHOW_API __declspec(dllexport)
#else
#define F_SHOW_API __declspec(dllimport)
#endif


/*---------------------------------------------- Параметры генерации */

/*----------------------- Описание класса управления объектом "Тело" */

  class F_SHOW_API RSS_Module_Show : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Show_instr *mInstrList ;          /* Список команд */
				     
    public:
     virtual        void  vStart        (void) ;            /* Стартовая разводка */
     virtual RSS_Feature *vCreateFeature(RSS_Object *) ;    /* Создать свойство */
     virtual        void  vShow         (char *) ;          /* Отобразить связанные данные */
     virtual         int  vExecuteCmd   (const char *) ;    /* Выполнить команду */
     virtual        void  vWriteSave    (std::string *) ;   /* Записать данные в строку */

    public:
                     int  cHelp         (char *) ;          /* Инструкция Help */ 
                     int  cColor        (char *) ;          /* Инструкция Color */
                     int  cVisible      (char *) ;          /* Инструкция Visible */

                     int  iShowScene    (void) ;            /* Отображение сцены */

	                  RSS_Module_Show() ;               /* Конструктор */
	                 ~RSS_Module_Show() ;               /* Деструктор */
                                                       } ;

/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_Show_instr {

           char                   *name_full ;         /* Полное имя команды */
           char                   *name_shrt ;         /* Короткое имя команды */
           char                   *help_row ;          /* HELP - строка */
           char                   *help_full ;         /* HELP - полный */
            int (RSS_Module_Show::*process)(char *) ;  /* Процедура выполнения команды */
                              }  ;

/*---------------------------------------------- Компоненты описания */

 typedef struct {                                           /* Описание точки */
                    double  x ;                              /* X - значение */
                      char *x_formula ;                      /* X - рассчетное выражение */
                      void *x_calculate ;                    /* X - контекст вычислителя */

                    double  y ;                              /* Y - значение */
                      char *y_formula ;                      /* Y - рассчетное выражение */
                      void *y_calculate ;                    /* Y - контекст вычислителя */

                    double  z ;                              /* Z - значение */
                      char *z_formula ;                      /* Z - рассчетное выражение */
                      void *z_calculate ;                    /* Z - контекст вычислителя */
                }  RSS_Feature_Show_Vertex ;

#define  _VERTEX_PER_FACET_MAX   16

 typedef struct {                                           /* Описание грани */
                    int  vertexes[_VERTEX_PER_FACET_MAX] ;
                    int  vertexes_cnt ;
                }  RSS_Feature_Show_Facet ;

 typedef struct {                                            /* Описание тела */
                               char  name[128] ;              /* Название */
               struct RSS_Parameter *extrn_pars ;             /* Ссылак на параметры */

                                int  Visible ;                /* Видимость */
                           COLORREF  Color ;                  /* Цвет */
                           
            RSS_Feature_Show_Vertex *Vertexes ;               /* Список вершин */
                                int  Vertexes_cnt ; 
             RSS_Feature_Show_Facet *Facets ;                 /* Список вершин */
                                int  Facets_cnt ;

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
                }  RSS_Feature_Show_Body ;


/*----------------------------------- Описание класса свойства "Вид" */

  class F_SHOW_API RSS_Feature_Show : public RSS_Feature {

    public:       
                                int  Visible ;                /* Видимость */
                           COLORREF  Color ;                  /* Цвет */

              RSS_Feature_Show_Body *Bodies ;                 /* Список тел */
                                int  Bodies_cnt ;

               struct RSS_Parameter *Pars_work ;

    public:
            virtual void  vReadSave     (char *, std::string *,     /* Считать данные из строки */
                                                        char * ) ;
            virtual void  vGetInfo      (std::string *) ;           /* Выдать информацию о свойстве */
            virtual  int  vParameter    (char *, char *, char *) ;  /* Работа с параметрами */  
            virtual void  vBodyDelete   (char *) ;                  /* Удалить тело */
            virtual void  vBodyBasePoint(char *,                    /* Задание базовой точки тела */
                                         double, double, double) ;
            virtual void  vBodyAngles   (char *,                    /* Задание ориентации тела */
                                         double, double, double) ;
            virtual void  vBodyMatrix   (char *, double[4][4]) ;    /* Задание матрицы положения звена */
            virtual void  vBodyShifts   (char *,                    /* Задание смещения положения и  */
                                          double, double, double,   /*    ориентации тела            */
                                          double, double, double ) ;
            virtual void  vBodyPars     (char *,                    /* Задание списка параметров */
                                             struct RSS_Parameter *) ;

                    void  Show          (void) ;                    /* Отобразить объект */

                    void  iCalcNormal   (RSS_Feature_Show_Vertex *, /* Вычислить нормаль к плоскости */
                                         RSS_Feature_Show_Vertex *,   
                                         RSS_Feature_Show_Vertex *,   
                                         RSS_Feature_Show_Vertex * ) ;
              RSS_Kernel *iGetCalculator(void) ;                    /* Определение нужного вычислителя */ 

	                  RSS_Feature_Show() ;                /* Конструктор */
	                 ~RSS_Feature_Show() ;                /* Деструктор */
                                                         } ;

/*--------------------------------------------- Диалоговые процедуры */

/* Файл  F_Show.cpp */

/* Файл  F_Show_dialog.cpp */
  BOOL CALLBACK  Feature_Show_Help_dialog(HWND, UINT, WPARAM, LPARAM) ;

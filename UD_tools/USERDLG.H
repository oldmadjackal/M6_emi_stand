/*********************************************************************/
/*                                                                   */
/*               СИСТЕМА ГРАФИЧЕСКОГО ИНТЕРФЕЙСА                     */
/*                                                                   */
/*********************************************************************/

/*------------------------------------------------- Специальные типы */

typedef  int (CALLBACK *UD_CALLBACK)(int, int, void *) ;

  #define  _UD_ZOOM             1
//#define  _UD_SCANNER_MOVE     2   /* Определено в ниже по тексту */
//#define  _UD_SCANNER_CLEAR    3   /* Определено в ниже по тексту */
  #define  _UD_PROFILE          4

/*---------------------------------------------- Системные структуры */

  struct UD_metrics {                     /* Размерности элемента */ 
                      int  zoom_flag ;     /* Флаг масштабного окна */
                      int  zoom_num ;      /* Номер масштабного окна */

                      int  x_base ;        /* Положение окна элемента */
                      int  y_base ;
                      int  x_size ;        /* Размеры окна элемента */
                      int  y_size ;

                      int  draw_x_base ;   /* Положение зоны отрисовки */
                      int  draw_y_base ;
                      int  draw_x_size ;   /* Размеры зоны отрисовки */
                      int  draw_y_size ;

                      int  x_scale_h ;     /* Высота поля оцифровки по X */
                      int  y_scale_w ;     /* Ширина поля оцифровки по Y */

                      int  stream_base ;   /* Базовый поток (поток с сеткой) */
                   double  x_base_min ;    /* Диапазоны осей базового графика */
		   double  x_base_max ;
		   double  y_base_min ;
		   double  y_base_max ;

               TEXTMETRIC  font ;          /* Параметры шрифта */     
                    } ;

 struct UD_value_zone {                    /* Описание поля вывода */
                        double  value ;     /* Значение */
			  char  buff[64] ;  /* Буфер вывода */
			   int  len  ;      /* Общая длина */
			   int  digits ;    /* Число цифр + запятая */
			   int  point ;     /* Число знаков после запятой */
			   int  power ;     /* Общий порядок */
			   int  pos  ;      /* Координаты */
		      } ;

/*--------------------------------------------------- Общие описания */

  struct UD_zoom_scale {                         /* Масштабные окна - "врезки" */
                          HWND  hWnd ;          
                           HDC  hDC ;
                       HBITMAP  hBitMap ;  

             struct UD_metrics  metrics ;         /* Рабочие размерности */      

		        double  x_min ;           /* Диапазоны "вырезки" по осям */
		        double  x_max ;
		        double  y_min ;
		        double  y_max ;
		        double  x_min_prv ;       /* Диапазоны "вырезки" по осям, предыдущие */
		        double  x_max_prv ;
		        double  y_min_prv ;
		        double  y_max_prv ;
                       } ;

  struct UD_data_scale {                         /* Масштаб отображения данных */
			   int  scale_regime ;    /* Управляемые данные */
#define                          _UD_X_SCALE   1
#define                          _UD_Y_SCALE   2
		        double  x_min ;           /* Диапазоны по осям */
		        double  x_max ;
		        double  y_min ;
		        double  y_max ;
		       } ;

  struct UD_color_range {                         /* Таблица расцветки */
                           COLORREF  color ;       /* Цвет */
		             double  value ;       /* Значение */
		                int  use ;         /* Флаг использования */
		        } ;
/*---------------------------------------------------------- Сечения */

     struct UD_profile {                         /* Сечение */
		        double  x_1 ;             /* Линия сечения */
		        double  y_1 ;
		        double  x_2 ;             
		        double  y_2 ;
       struct UD_profile_point *points ;          /* Точки сечения */
                           int  points_cnt ;
                          HWND  hWnd ;            /* Окно профиля */
           struct UD_diag_data *diag  ;           /* Описание диаграмма */
                           int  trace_flag ;      /* Режим регистрации следа */
                           int  line_style ;      /* Вид линии сечения */
                       } ;

     struct UD_profile_point {                   /* Точка сечения */
                               int  index_1 ;       
                               int  index_2 ;
                            double  coef_1 ;
                            double  x ;
                            double  y ;
                             } ;
/*--------------------------------------------- "Накладные" элементы */

     struct UD_draw_elem {                         /* Описание элемента контура */
                              int  type ;           /* Тип элемента */
#define                           _UD_LINE_ELEM  1       
                           double  x1 ;             /* Начальная точка */
                           double  y1 ;
                           double  x2 ;             /* Конечная точка */
                           double  y2 ;
                         } ;

     struct UD_draw_contur {                       /* Описание контура */
                             char  name[128] ;      /* Название/спецификация контура */
                             char  parent[128] ;    /* Вспомогательные родительские данные */
                         COLORREF  color ;          /* Цвет контура */
              struct UD_draw_elem *elements ;       /* Список элементов */
                              int  elements_cnt ;
                           } ;
/*---------------------------------------- Окно специальных надписей */

  struct UD_show_panel {
                         char *text ;           /* Выводимый текст */
                     COLORREF  fore_color ;     /* Цвет текста */
                     COLORREF  back_color ;     /* Цвет фона */
                       } ;
/*---------------------------------------- Окно цветового индикатора */

  struct UD_color_panel {
                     COLORREF   color ;                   /* Цвет */
                     COLORREF (*choice_proc)(COLORREF) ;  /* Функция назначения цвета */
                        } ;
/*--------------------------------------------- Окно цветовой панели */

  struct UD_color_bank {
                     COLORREF  *colors ;                       /* Цвета позиций */
                          int   colors_cnt ;                   /* Число позиций */
                          int   rows ;                         /* Число строк */
                          int   cols ;                         /* Число колонок */
                     COLORREF   backcolor ;                    /* Цвет фона */

                          int   lamp_x_size ;                  /* Размер лампочки */
                          int   lamp_y_size ;
                          int   lamp_x_prefix ;                /* Отступ первой лампочки */
                          int   lamp_y_prefix ;
                          int   lamp_x_pause ;                 /* Расстояние между лампочками */
                          int   lamp_y_pause ;
                          int   lamp_form ;                    /* Форма лампочки */
#define                          _UD_LAMP_RECT    0            /*  Прямоугольная */

                     COLORREF (*choice_proc)(COLORREF, int) ;  /* Функция управления */

                          HDC   hDC ;
                      HBITMAP   hBitMap ;
                       } ;
/*-------------------------------------------------- Система диаграм */

  struct UD_diag_data {                          /* Отображаемые данные */
                          void *parent_data ;     /* Ссылка на базовые данные */
                           int   spec_oper ;      /* Специальные операции */
#define                          _UD_GRID_CHANGE     1    /* Сменить сетку */
#define                          _UD_SCANNER_MOVE    2    /* Сменить положение сканера */
#define                          _UD_SCANNER_CLEAR   3    /* Удалить сканер */

	   struct UD_data_list **data_list ;      /* Наборы данных */
			   int   data_list_cnt ;  /* Число наборов данных */

          struct UD_data_scale  *spec_scale ;     /* 'Внешний' масштаб */

                      COLORREF   fore_color ;     /* Цвет текста */
                      COLORREF   grid_color ;     /* Цвет сетки */
                      COLORREF   back_color ;     /* Цвет фона */

             struct UD_metrics   metrics ;        /* Рабочие размерности */      

                          HWND   hWnd_scan ;      /* Окно в котором задан сканер */

          struct UD_zoom_scale  *zooms ;          /* Масштабные окна - "врезки" */
                           int   zooms_cnt ;

                   UD_CALLBACK   ctrl_proc ;      /* Управляющая процедура */
		      } ;

  struct UD_data_list {                          /* Описание массива данных */
			  int  type ;              /* Тип массив данных:  */
#define                         _UD_MONO_DATA   0  /*   Монотонные данные */
#define                         _UD_PHASE_DATA  1  /*   Фазовые данные    */
#define                         _UD_POINT_DATA  2  /*   Только точки      */
			 void *data ;              /* Поток данных */
			  int  data_cnt ;          /* Счетчик данных */

                     COLORREF  color ;             /* Цвет отображения */
                          int  line_style ;        /* Вид линии */

			  int  use_flag ;          /* Флаг использования */
			  int  grid_flag ;         /* Флаг вывода сетки */
			  int  grid_values_flag ;  /* Флаг вывода оцифровки */

			  int  x_scale_type ;      /* Тип X-шкалы */
			  int  y_scale_type ;      /* Тип Y-шкалы */
#define                         _UD_FREE_SCALE  0  /*   Свободная шкала */
#define                         _UD_ZERO_SCALE  1  /*   Шкала с обязательным включением нуля */
			  int  x_scale_group ;     /* Группа совместного X-диапазона */
			  int  y_scale_group ;     /* Группа совместного Y-диапазона */
		       double  x_min ;             /* Диапазоны данных */
		       double  x_max ;
		       double  y_min ;
		       double  y_max ;
                       double  x_grid_0 ;          /* База отсчета сетки */
                       double  y_grid_0 ; 
                       double  x_grid_step ;       /* Шаг сетки */
                       double  y_grid_step ; 

		       double  x_scanner ;         /* Положение сканера */
		       double  y_scanner ;
		       double  y_scn_value ;       /* Отсечка сканера по графику */
                          int  y_scn_null ;        /* Значение по Y не определено */
                       double  x_scanner_prv ;     /* Предыдущее положение сканера */
                       double  y_scanner_prv ;
                          int  scanner_prv_flag ;  /* Флаг наличия prv-значений */

			 void *mark ;              /* Поток маркеров */
			  int  mark_cnt ;          /* Счетчик маркеров */
			} ;

  struct UD_data_2Dpoint {                        /* Элемент 2-мерных данных */
			   double  x ;             /* 'Координаты' */
			   double  y ;
			      int  mark ;          /* Многоцелевая метка */
#define                          _UD_X_MARKER   1
#define                          _UD_Y_MARKER   2
			 } ;
/*---------------------------------------------------- Система полей */

  struct UD_area_data {                          /* Отображаемые данные */
                          void *parent_data ;     /* Ссылка на базовые данные */
                           int  spec_oper ;       /* Специальные операции */
			   int  type ;            /* Тип массив данных:  */
#define                           _UD_GRID_DATA  1  /* Сетка */
#define                         _UD_SCALAR_DATA  2  /* Скаляры */
#define                         _UD_VECTOR_DATA  4  /* Вектора */
#define                        _UD_ISOLINE_DATA  8  /* Скаляры - изолинии */

        struct UD_area_2Dpoint  *points ;         /* Список точек */
			   int   points_cnt ;

        struct UD_data_2Dpoint  *spots ;          /* Список точек границ ячеек */
			   int   spots_cnt ;

         struct UD_color_range  *scalar_colors ;  /* Цветовой диапазон скаляра */
         struct UD_color_range  *vector_colors ;  /* Цветовой диапазон вектора */

                           int   scalar_regime ;  /* Режим отображения */
                           int   vector_regime ;
#define                         _UD_FIXED_COLOR  1  /* Постоянный цвет */
#define                          _UD_ZONE_COLOR  2  /* Зонный цвет */
#define                         _UD_SMART_COLOR  4  /* Переходной цвет */
#define                         _UD_FIXED_LEN    8  /* Постоянная длина вектора */
#define                        _UD_RASTER_SIZE  16  /* Растр узлов для скаляров */

                           int   prop_scale ;     /* Флаг X/Y пропорциональности */  

          struct UD_data_scale  *spec_scale ;     /* 'Внешний' масштаб */

                      COLORREF   fore_color ;     /* Цвет текста */
                      COLORREF   grid_color ;     /* Цвет сетки */
                      COLORREF   back_color ;     /* Цвет фона */

             struct UD_metrics   metrics ;        /* Рабочие размерности */      

                          HWND   hWnd_scan ;      /* Окно в котором задан сканер */

          struct UD_zoom_scale  *zooms ;          /* Масштабные окна - "врезки" */
                           int   zooms_cnt ;

             struct UD_profile  *profiles ;       /* Сечения */
                           int   profiles_cnt ;

          struct UD_draw_contur *figures ;        /* Список "накладных" элементов */
                            int  figures_cnt ;

                   UD_CALLBACK   ctrl_proc ;      /* Управляющая процедура */

		         double  x_data_min ;     /* Диапазоны координат точек сетки данных */
		         double  x_data_max ;
		         double  y_data_min ;
		         double  y_data_max ;
		         double  x_min ;          /* Диапазоны координат точек */
		         double  x_max ;
		         double  y_min ;
		         double  y_max ;
                         double  x_grid_0 ;       /* База отсчета сетки */
                         double  y_grid_0 ; 
                         double  x_grid_step ;    /* Шаг сетки */
                         double  y_grid_step ; 

                         double  x_scanner ;      /* Положение сканера */
                         double  y_scanner ;
         struct UD_area_2Dpoint *v_scanner_list ; /* Список сканер-векторов */
                            int  v_scanner_cnt ;

		         double  v_len_max ;      /* Максимальная длина вектора */

                            HDC  hDC ;
                        HBITMAP  hBitMap ;
		      } ;

  struct UD_area_2Dpoint {                        /* Элемент 2-мерного поля */
			   double  x ;             /* 'Координаты' */
			   double  y ;
			      int  mark ;          /* Многоцелевая метка */
                              int  obstacle ;      /* Признак препятствия */
			   double  x_vector ;      /* Проекции вектора поля  */
			   double  y_vector ;
			   double  value ;         /* Скаляр поля */
                         COLORREF  color ;         /* Цвет */

                              int *spot_list ;     /* Список индексов точек границы пятна ячейки */
                              int  spot_cnt ;

                              int *link_list ;     /* Список связанных узлов */
                              int  link_cnt ;
                              int  boundary ;      /* Флаг граничной точки */

                              int  rare_active ;   /* Флаг фильтрованных точек */
                              int  rare_node ;     /* Номер ведущей точки сетки */
                              int  layer ;         /* Слой */ 
                              int  inside ;        /* Флаг внутренней точки слоя */ 
                         } ;
/*----------------------------------------------- Команды и операции */

#define    _UD_CLEAR  1
#define    _UD_SET    2
#define    _UD_GET    3

/*-------------------------------------------------------- Интерфейс */

      int  UD_init      (void) ;                       /* Инициализация системы диалога */
      int  UD_error     (char *) ;                     /* Сообщение об ошибке */
     char *UD_ptr_incode(void *) ;                     /* Преобразование адреса данных в строку */
     void *UD_ptr_decode(char *) ;                     /* Преобразование строки в адрес данных */

/*------------------------------------------ Вспомогательные функции */

     void  UDi_form_rect   (RECT *, int, int, int, int) ;   /* Формирование канонического прямоугольника */
 COLORREF  UDi_assign_color(int, double,                    /* Рассчет цвета по цветовой таблице */
                             struct UD_color_range *) ;   
      int  UDi_assign_index(double,                         /* Рассчет индекса по цветовой таблице */
                             struct UD_color_range *) ;
     void  UDi_zone_invert (HDC, UD_metrics *, RECT *) ;    /* Инверсия по границе зоны */
     void  UDi_cross_invert(HDC, UD_metrics *, int, int) ;  /* Инверсия "креста" сканера */
   double  UDi_range_step  (double) ;                       /* Определение оптимального разбиения диапазона */
     void  UDi_value_zone  (double, double, double,         /* Определение потребной длины поля оцифровки */
                             UD_value_zone *) ;
     void  UDi_value_cvt   (double, UD_value_zone *, int) ; /* Перевод числа в текст */

/*---------------------------------------------- Графический адаптер */

typedef  struct {  double  x ;
                   double  y ;  }  UD_point_w ;

      int  UDg_setviewport(HDC, int, int, int, int) ;        /* Задание зоны отображения */
      int  UDg_setwindow  (HDC, int, double, double,         /* Масштабирование зоны отображения */
                                     double, double ) ;  
      int  UDg_xpos_w     (HDC, double) ;                    /* Расчет экранных координат по оконным */ 
      int  UDg_ypos_w     (HDC, double) ;
   double  UDg_xcoord_w   (HDC, int, double *, double *) ;
   double  UDg_ycoord_w   (HDC, int, double *, double *) ;
      int  UDg_setpixel_w (HDC, double, double, COLORREF) ;  /* Отрисовка точки */ 
      int  UDg_moveto_w   (HDC, double, double) ;            /* Перенос пера в точку */ 
      int  UDg_lineto_w   (HDC, double, double) ;            /* Отрисовка линии в точку */ 
      int  UDg_marker_w   (HDC, double, double, int) ;       /* Отрисовка маркера точки */ 
      int  UDg_polygon_w  (HDC, UD_point_w *, int) ;         /* Отрисовка многоугольника */ 

/*------------------------------------------------------ Обработчики */

   LRESULT CALLBACK  UD_show_panel_prc (HWND, UINT, WPARAM, LPARAM) ;
   LRESULT CALLBACK  UD_color_panel_prc(HWND, UINT, WPARAM, LPARAM) ;
   LRESULT CALLBACK  UD_color_bank_prc (HWND, UINT, WPARAM, LPARAM) ;
   LRESULT CALLBACK  UD_diagram_2D_prc (HWND, UINT, WPARAM, LPARAM) ;
   LRESULT CALLBACK  UD_area_2D_prc    (HWND, UINT, WPARAM, LPARAM) ;

/*********************************************************************/
/*                                                                   */
/*                БИБЛИОТЕКА РАБОТЫ С МАТРИЦАМИ                      */
/*                                                                   */
/*********************************************************************/


/*----------------------------------------- Класс - 2-мерная матрица */

  class Matrix2d {

    public:
                          int  mRows ;          /* Число строк */
                          int  mCols ;          /* Число колонок */

                       double *mBody ;

                          int  mDebug ;

    public:
                int  Create     (int,  int) ;          /* Создание матрицы */
               void  Free       (void) ;               /* Освободить ресурсы */
                int  Copy       (Matrix2d *) ;         /* Копирование матрицы */
                int  Transpose  (Matrix2d *) ;         /* Транспонирование матрицы */
                int  SetCell    (int, int, double) ;   /* Пропись ячейки */
             double  GetCell    (int, int) ;           /* Извлечение ячейки */
                int  LoadArray  (double *) ;           /* Загрузка массива значений */
                int  LoadArray  (int, int, double *) ;
                int  LoadZero   (void) ;               /* Загрузка нулевой матрицы */
                int  LoadZero   (int,  int) ;
                int  LoadE      (void) ;               /* Загрузка единичной матрицы */
                int  LoadE      (int,  int) ;

                int  Load3d_azim(double) ;             /* Загрузка 3x3 матрицы поворота */
                int  Load3d_elev(double) ;
                int  Load3d_roll(double) ;

                int  Load4d_azim(double) ;             /* Загрузка 4x4 матрицы поворота */
                int  Load4d_elev(double) ;
                int  Load4d_roll(double) ;
                int  Load4d_base(double,               /*Загрузка 4x4 матрицы смещения */
                                 double, double) ;

                int  LoadMul    (Matrix2d *,           /* Перемножение матриц */ 
                                 Matrix2d * ) ;

                     Matrix2d   () ;                   /* Конструктор */
                    ~Matrix2d   () ;                   /* Деструктор */
                 } ;

/*-------------------------------------------------------------------*/

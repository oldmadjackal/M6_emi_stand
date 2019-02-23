/*********************************************************************/
/*                                                                   */
/*                ���������� ������ � ���������                      */
/*                                                                   */
/*********************************************************************/


/*----------------------------------------- ����� - 2-������ ������� */

  class Matrix2d {

    public:
                          int  mRows ;          /* ����� ����� */
                          int  mCols ;          /* ����� ������� */

                       double *mBody ;

                          int  mDebug ;

    public:
                int  Create     (int,  int) ;          /* �������� ������� */
               void  Free       (void) ;               /* ���������� ������� */
                int  Copy       (Matrix2d *) ;         /* ����������� ������� */
                int  Transpose  (Matrix2d *) ;         /* ���������������� ������� */
                int  SetCell    (int, int, double) ;   /* ������� ������ */
             double  GetCell    (int, int) ;           /* ���������� ������ */
                int  LoadArray  (double *) ;           /* �������� ������� �������� */
                int  LoadArray  (int, int, double *) ;
                int  LoadZero   (void) ;               /* �������� ������� ������� */
                int  LoadZero   (int,  int) ;
                int  LoadE      (void) ;               /* �������� ��������� ������� */
                int  LoadE      (int,  int) ;

                int  Load3d_azim(double) ;             /* �������� 3x3 ������� �������� */
                int  Load3d_elev(double) ;
                int  Load3d_roll(double) ;

                int  Load4d_azim(double) ;             /* �������� 4x4 ������� �������� */
                int  Load4d_elev(double) ;
                int  Load4d_roll(double) ;
                int  Load4d_base(double,               /*�������� 4x4 ������� �������� */
                                 double, double) ;

                int  LoadMul    (Matrix2d *,           /* ������������ ������ */ 
                                 Matrix2d * ) ;

                int  Determinant(double *) ;           /* ������� ������������ */

                     Matrix2d   () ;                   /* ����������� */
                    ~Matrix2d   () ;                   /* ���������� */
                 } ;

/*-------------------------------------------------------------------*/

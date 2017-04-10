/*********************************************************************/
/*                                                                   */
/*            БИБЛИОТЕКА РАБОТЫ С 2-МЕРНЫМИ МАТРИЦАМИ                */
/*                                                                   */
/*********************************************************************/

#include  <stddef.h>
#include  <stdlib.h>
#include  <memory.h>
#include  <malloc.h>
#include  <math.h>

#include  "matrix.h"


#define  _GRD_TO_RAD   0.01745

#define  CELL(r, c)   mBody[r*mCols+c] 


/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     Matrix2d::Matrix2d(void)

{
        mRows=  0 ;
        mCols=  0 ;
        mBody=NULL ;

        mDebug= 0 ;
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    Matrix2d::~Matrix2d(void)

{
    Free() ;
}


/********************************************************************/
/*								    */
/*		        Освобождение ресурсов                       */

    void  Matrix2d::Free(void)

{
     if(mBody!=NULL)  free(mBody) ;
                           mBody=NULL ; 
}


/********************************************************************/
/*								    */
/*		          Создание матрицы                          */

    int  Matrix2d::Create(int  rows, int  cols)

{
                 Free() ;

     if(rows<1 || cols<1)  return(-1) ;

        mBody=(double *)calloc(rows*cols, sizeof(double)) ;
     if(mBody==NULL)  return(-1) ;

       mRows=rows ;
       mCols=cols ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*	                  Пропись ячейки                            */

   int  Matrix2d::SetCell(int  row, int  col, double  value)

{
   if(mBody==NULL)  return(-1) ;

   if(row<0 || row>=mRows ||
      col<0 || col>=mCols   )  return(-1) ;

            CELL(row, col)=value ;

  return(0) ;
}


/********************************************************************/
/*								    */
/*	                 Извлечение ячейки                          */

   double  Matrix2d::GetCell(int  row, int  col)

{
   if(mBody==NULL)  return(0.) ;

   if(row<0 || row>=mRows ||
      col<0 || col>=mCols   )  return(0.) ;

   return(CELL(row, col)) ;
}


/********************************************************************/
/*								    */
/*	                 Копирование матрицы                        */

   int  Matrix2d::Copy(Matrix2d *source)

{
   if(source->mBody==NULL)  return(0) ;
   if(source       ==this)  return(0) ;

        Free() ;
      Create(source->mRows, source->mCols) ;

      memcpy(this->mBody, source->mBody, mRows*mCols*sizeof(double)) ;

  return(0) ;   
}


/********************************************************************/
/*								    */
/*	                 Транспонирование матрицы                   */

   int  Matrix2d::Transpose(Matrix2d *source)

{
  int  i ;
  int  j ;


   if(source->mBody==NULL)  return(0) ;
   if(source       ==this)  return(0) ;

        Free() ;
      Create(source->mCols, source->mRows) ;

     for(i=0 ; i<mRows ; i++)
     for(j=0 ; j<mCols ; j++)  CELL(i, j)=source->GetCell(j, i) ;


  return(0) ;   
}


/********************************************************************/
/*								    */
/*                      Загрузка из массива значений                */

   int  Matrix2d::LoadArray(double *array)

{
   if(mBody==NULL)  return(-1) ;

     memcpy(mBody, array, mRows*mCols*sizeof(double)) ;

  return(0) ;
}

    int  Matrix2d::LoadArray(int  rows, int  cols, double *array)

{
            Create(rows, cols) ;
   return(LoadArray(array)) ;
}


/********************************************************************/
/*								    */
/*		        Загрузка нулевой матрицы                    */

   int  Matrix2d::LoadZero(void)

{
   int  i ;
   int  j ;


   if(mBody==NULL)  return(-1) ;

     for(i=0 ; i<mRows ; i++)
     for(j=0 ; j<mCols ; j++)  CELL(i, j)=0. ;

  return(0) ;
}


    int  Matrix2d::LoadZero(int  rows, int  cols)

{
            Create(rows, cols) ;
   return(LoadZero()) ;
}


/********************************************************************/
/*								    */
/*		        Загрузка единичной матрицы                  */

   int  Matrix2d::LoadE(void)

{
   int  i ;
   int  j ;


   if(mBody==NULL)  return(-1) ;

            LoadZero() ;

     for(i=0 ; i<mRows ; i++)
     for(j=0 ; j<mCols ; j++)  if(i==j)  CELL(i, j)=1. ;

  return(0) ;
}


    int  Matrix2d::LoadE(int  rows, int  cols)

{
            Create(rows, cols) ;
   return(LoadE()) ;
}


/********************************************************************/
/*								    */
/*                Загрузка 3x3 матрицы поворота                     */

   int  Matrix2d::Load3d_azim(double  azimuth)

{
  double  cos_a ;
  double  sin_a ;


          sin_a=sin(azimuth*_GRD_TO_RAD) ;
          cos_a=cos(azimuth*_GRD_TO_RAD) ;


       LoadE(3, 3) ;
        CELL(0, 0)= cos_a ;          
        CELL(2, 2)= cos_a ;          
        CELL(0, 2)=-sin_a ;          
        CELL(2, 0)= sin_a ;          

  return(0) ;
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   int  Matrix2d::Load3d_elev(double  elevation)

{
  double  cos_a ;
  double  sin_a ;


          sin_a=sin(elevation*_GRD_TO_RAD) ;
          cos_a=cos(elevation*_GRD_TO_RAD) ;

       LoadE(3, 3) ;
        CELL(1, 1)= cos_a ;          
        CELL(2, 2)= cos_a ;          
        CELL(1, 2)=-sin_a ;          
        CELL(2, 1)= sin_a ;          

  return(0) ;
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   int  Matrix2d::Load3d_roll(double  roll)

{
  double  cos_a ;
  double  sin_a ;


          sin_a=sin(roll*_GRD_TO_RAD) ;
          cos_a=cos(roll*_GRD_TO_RAD) ;

       LoadE(3, 3) ;
        CELL(0, 0)= cos_a ;          
        CELL(1, 1)= cos_a ;          
        CELL(0, 1)=-sin_a ;          
        CELL(1, 0)= sin_a ;          

  return(0) ;
}


/********************************************************************/
/*								    */
/*                Загрузка 4x4 матрицы поворота                     */

   int  Matrix2d::Load4d_azim(double  azimuth)

{
  double  cos_a ;
  double  sin_a ;


          sin_a=sin(azimuth*_GRD_TO_RAD) ;
          cos_a=cos(azimuth*_GRD_TO_RAD) ;


       LoadE(4, 4) ;
        CELL(0, 0)= cos_a ;          
        CELL(2, 2)= cos_a ;          
        CELL(0, 2)=-sin_a ;          
        CELL(2, 0)= sin_a ;          

  return(0) ;
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   int  Matrix2d::Load4d_elev(double  elevation)

{
  double  cos_a ;
  double  sin_a ;


          sin_a=sin(elevation*_GRD_TO_RAD) ;
          cos_a=cos(elevation*_GRD_TO_RAD) ;

       LoadE(4, 4) ;
        CELL(1, 1)= cos_a ;          
        CELL(2, 2)= cos_a ;          
        CELL(1, 2)=-sin_a ;          
        CELL(2, 1)= sin_a ;          

  return(0) ;
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   int  Matrix2d::Load4d_roll(double  roll)

{
  double  cos_a ;
  double  sin_a ;


          sin_a=sin(roll*_GRD_TO_RAD) ;
          cos_a=cos(roll*_GRD_TO_RAD) ;

       LoadE(4, 4) ;
        CELL(0, 0)= cos_a ;          
        CELL(1, 1)= cos_a ;          
        CELL(0, 1)=-sin_a ;          
        CELL(1, 0)= sin_a ;          

  return(0) ;
}


/********************************************************************/
/*								    */
/*                Загрузка 4x4 матрицы смещения                     */

   int  Matrix2d::Load4d_base(double  x, double  y, double  z)

{
       LoadE(4, 4) ;
        CELL(0, 3)= x ;
        CELL(1, 3)= y ;
        CELL(2, 3)= z ;

  return(0) ;
}


/********************************************************************/
/*								    */
/*		        Перемножение матриц                         */

     int  Matrix2d::LoadMul(Matrix2d *left, Matrix2d *right)
{
  Matrix2d  Tmp_L ;
  Matrix2d  Tmp_R ;
    double  sum ;
       int  i ;
       int  j ;
       int  k ;


    if(left->mCols!=right->mRows)  return(-1) ; 

             Tmp_L.Copy(left) ;
                        left=&Tmp_L ;
             Tmp_R.Copy(right) ;
                        right=&Tmp_R ;

   if(mDebug==1) return(0) ;

       Create(left->mRows, right->mCols) ;

   if(mDebug==2) return(0) ;

    for(i=0 ; i< left->mRows ; i++)
    for(j=0 ; j<right->mCols ; j++) {

      for(sum=0., k=0 ; k<left->mCols ; k++)  sum+= left->GetCell(i, k)*
                                                   right->GetCell(k, j) ;
                 CELL(i, j)=sum ;
                                    }

  return(0) ;
}

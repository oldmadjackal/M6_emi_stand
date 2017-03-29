/*********************************************************************/
/*                                                                   */
/*            БИБЛИОТЕКА РАБОТЫ С 2-МЕРНЫМИ МАТРИЦАМИ                */
/*                                                                   */
/*********************************************************************/

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

     if(w<1 || h<1)  return(-1) ;

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
/*                      Загрузка из массива значений                */

   int  Matrix2d::LoadArray(double *array)

{
   if(mBody==NULL)  return(-1) ;

     memcpy(mBody, array, mRows*mCols*sizeof(double)) ;

  return(0) ;
}

    void  Matrix2d::LoadArray(int  w, int  h, double *array)

{
            Create(w, h) ;
   return(LoadArray()) ;
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
     for(j=0 ; j<mCols ; j++)  mBody[j*mWidth+i]=0. ;

  return(0) ;
}


    void  Matrix2d::LoadZero(int  w, int  h)

{
            Create(w, h) ;
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

     for(i=0 ; i<mWidth  ; i++)
     for(j=0 ; j<mHeight ; j++)
       if(i==j)  mBody[j*mWidth+i]=0. ;

  return(0) ;
}


    void  Matrix2d::LoadE(int  w, int  h)

{
            Create(w, h) ;
   return(LoadE()) ;
}


/********************************************************************/
/*								    */
/*		        Загрузка матрицы поворота                   */

   int  Matrix2d::Load3d_azim(double  azimuth)

{
  double  cos_a ;
  double  sin_a ;


          sin_a=sin(azimuth*_GRD_TO_RAD) ;
          cos_a=cos(azimuth*_GRD_TO_RAD) ;

       LoadE(3, 3) ;
     SetCell(0, 0)= cos_a ;          
     SetCell(1, 1)= cos_a ;          
     SetCell(0, 1)=-sin_a ;          
     SetCell(1, 0)= sin_a ;          

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
     SetCell(1, 1)= cos_a ;          
     SetCell(2, 2)= cos_a ;          
     SetCell(1, 2)=-sin_a ;          
     SetCell(2, 1)= sin_a ;          

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
     SetCell(0, 0)= cos_a ;          
     SetCell(2, 2)= cos_a ;          
     SetCell(0, 2)= sin_a ;          
     SetCell(2, 0)=-sin_a ;          

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


    if(left->Cols!=right->mRows)  return(-1) ; 

             Tmp_L.Copy(left) ;
                        left=Tmp_L ;
             Tmp_R.Copy(right) ;
                        right=Tmp_R ;

       Create(left->mRows, right->mCols) ;

    for(i=0 ; i< left->mRows ; i++)
    for(j=0 ; j<right->mCols ; j++) {

      for(sum=0., k=0 ; k<left->mCols ; k++)  sum+= left->GetCell(i, k)*
                                                   right->GetCell(k, j) ;
                 CELL(i, j)=sum ;
                                    }

  return(0) ;
}

/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*          ������� ��� ������ � QUALITY CENTER API                  */
/*                                                                   */
/*********************************************************************/

#if defined(AIX) || defined(LINUX)
#ifndef UNIX
#define  UNIX
#endif
#endif


#include  <errno.h>
#include  <stddef.h>
#include  <stdlib.h>
#include  <string.h>
#include  <ctype.h>
#include  <math.h>
#include  <memory.h>
#include  <sys/types.h>
#include  <sys/stat.h>

#ifdef UNIX
#include  <unistd.h>
#include  <pthread.h>
#define   stricmp    strcasecmp
#define   strupr(t)  for(char *b=t ; *b ; b++)  *b=toupper(*b)
#define   GetCurrentThreadId  pthread_self
#else
#include  <windows.h>
#endif

#define   DCL_INSIDE
#include "dcl.h"


#pragma warning(disable : 4996)

/*------------------------------------------------- ����� ���������� */

  static             char  error_text[2048] ;

  static         Dcl_decl  result_data[4] ={
                                  {_CHR_AREA, 0, 0, 0, "$type$",    (void *)   0, "QC-RESULT",    9,   32},
                                  {_CHR_AREA, 0, 0, 0, "test_path", (void *)  32,  NULL,          0, 1024},
                                  {_CHR_AREA, 0, 0, 0, "result",    (void *)1056,  NULL,          0,   32},
                                  {_CHR_AREA, 0, 0, 0, "bug_desc",  (void *)1088,  NULL,          0, 2048}
                                           } ;
  static Dcl_complex_type  result_template={ "QC_result", 3136, result_data, 4} ;

/*---------------------------------------------------- ������� ����� */

          extern double  dcl_errno ;       /* ��������� ��������� ������ -> DCL_SLIB.CPP */

/*---------------------------- ������� ��� ������ � MIDAS API � SOAP */

                                

/*********************************************************************/
/*                                                                   */
/*               ���������� ����� ������ QC-result                   */
/*                                                                   */
/*  ������ ������� �� 4 �����:                                       */
/*           $type$  -  ������ QC-RESULT                             */
/*        test_path  -  ������ ������������� �����                   */
/*           result  -  ��������� ���������� �����                   */
/*         bug_desc  -  �������� �������                             */

   int  Lang_DCL::zQC_ResultNew(Dcl_decl  *source, 
                                Dcl_decl **pars, 
                                     int   pars_cnt)

{
  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*               ��������� ������� QC-result                         */
/*                                                                   */
/*  ������������ �����������:                                        */
/*    ������������ ����������� ������������ ����� �����������        */
/*    �������, � ������� ����� ���� ��������� ���������              */
/*    �����-�����������:                                             */
/*      $REQUEST$  -  ���� � ����� � ������� �������                 */
/*      $RESULT$   -  ���� � ����� � ����������� ���������� �������  */

   int  Lang_DCL::zQC_ResultSend(Dcl_decl  *source,
                                 Dcl_decl **pars,
                                      int   pars_cnt)

{
  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*               ���������� ������ ������ � QC                       */

   int  Lang_DCL::zQC_Errors(Dcl_decl  *source, 
                             Dcl_decl **pars, 
                                  int   pars_cnt, 
                                 char  *buff,
                                  int   buff_size)
{
  return(0) ;
}


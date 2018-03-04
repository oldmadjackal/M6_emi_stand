/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*          УТИЛИТЫ ДЛЯ РАБОТЫ С QUALITY CENTER API                  */
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

/*------------------------------------------------- Общие переменные */

  static             char  error_text[2048] ;

  static         Dcl_decl  result_data[4] ={
                                  {_CHR_AREA, 0, 0, 0, "$type$",    (void *)   0, "QC-RESULT",    9,   32},
                                  {_CHR_AREA, 0, 0, 0, "test_path", (void *)  32,  NULL,          0, 1024},
                                  {_CHR_AREA, 0, 0, 0, "result",    (void *)1056,  NULL,          0,   32},
                                  {_CHR_AREA, 0, 0, 0, "bug_desc",  (void *)1088,  NULL,          0, 2048}
                                           } ;
  static Dcl_complex_type  result_template={ "QC_result", 3136, result_data, 4} ;

/*---------------------------------------------------- Внешние связи */

          extern double  dcl_errno ;       /* Системный указатель ошибок -> DCL_SLIB.CPP */

/*---------------------------- Функции для работы с MIDAS API и SOAP */

                                

/*********************************************************************/
/*                                                                   */
/*               Добавление новой записи QC-result                   */
/*                                                                   */
/*  Запись состоит из 4 полей:                                       */
/*           $type$  -  всегда QC-RESULT                             */
/*        test_path  -  полный идентификатор теста                   */
/*           result  -  результат выполнения теста                   */
/*         bug_desc  -  описание дефекта                             */

   int  Lang_DCL::zQC_ResultNew(Dcl_decl  *source, 
                                Dcl_decl **pars, 
                                     int   pars_cnt)

{
  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*               Обработка записей QC-result                         */
/*                                                                   */
/*  Спецификация обработчика:                                        */
/*    Спецификация обработчика представляет собой исполняемую        */
/*    команду, в которой могут быть размещены следующие              */
/*    макро-подстановки:                                             */
/*      $REQUEST$  -  путь к файлу с данными запроса                 */
/*      $RESULT$   -  путь к файлу с результатом выполнения запроса  */

   int  Lang_DCL::zQC_ResultSend(Dcl_decl  *source,
                                 Dcl_decl **pars,
                                      int   pars_cnt)

{
  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*               Извлечение ошибок работы с QC                       */

   int  Lang_DCL::zQC_Errors(Dcl_decl  *source, 
                             Dcl_decl **pars, 
                                  int   pars_cnt, 
                                 char  *buff,
                                  int   buff_size)
{
  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*          УТИЛИТЫ ДЛЯ РАБОТЫ С SQL-ЗАПРОСАМИ                       */
/*                                                                   */
/*********************************************************************/

#include  <errno.h>
#include  <stddef.h>
#include  <stdlib.h>
#include  <string.h>
#include  <stdio.h>
#include  <ctype.h>
#include  <math.h>
#include  <memory.h>
#include  <sys/types.h>
#include  <sys/stat.h>

#ifdef UNIX
#include  <unistd.h>
#define   stricmp  strcasecmp
#else
#endif


#define   DCL_INSIDE
#include "dcl.h"

#pragma warning(disable : 4996)

/*---------------------------------------------------- Внешние связи */

     extern         double  dcl_errno ;              /* Системный указатель ошибок -> DCL_SLIB.CPP */
     extern class SQL_link *dcl_sql_connect ;        /* Интерфейс SQL-запросов по-умолчанию */ 
     extern           char  dcl_sql_output_ignore ;  /* Флаг игнорирования выходных параметров SQL */

/*********************************************************************/
/*                                                                   */
/*              Выполнение однократной SELECT-выборки                */

   int  Lang_DCL::zSqlSelectOnce(Dcl_decl  *source, 
                                 Dcl_decl **pars, 
                                      int   pars_cnt)

{
  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*        Выполнение DML-оператора (INSERT, UPDATE, DELETE)          */ 
/*                     по всем элементам объекта                     */

   int  Lang_DCL::zSqlDmlExecute(Dcl_decl  *source, 
                                 Dcl_decl **pars, 
                                      int   pars_cnt)

{
  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*           Получение описания табличного объекта                   */

   int  Lang_DCL::zSqlDescribe(Dcl_decl  *source, 
                               Dcl_decl **pars, 
                                    int   pars_cnt)

{
  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*              Считывание DBF-файла                                 */

   int  Lang_DCL::zDbfRead(Dcl_decl  *source, 
                           Dcl_decl **pars, 
                                int   pars_cnt)

{
  return(0) ;
}

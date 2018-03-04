/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*          УТИЛИТЫ ДЛЯ РАБОТЫ С MIDAS API                           */
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
#define   stricmp    strcasecmp
#define   strupr(t)  for(char *b=t ; *b ; b++)  *b=toupper(*b)

#define  SOCKET_ERROR       -1 
#define  WSAGetLastError()  errno
#else
#include <winsock2.h>
#endif


#define   DCL_INSIDE
#include "dcl.h"


#pragma warning(disable : 4996)

/*------------------------------------------------- Общие переменные */

  static             char  error_type[32] ;
  static             char  error_text[1024] ;

  static         Dcl_decl  error_data[2] ={
                                  {_CHR_AREA, 0, 0, 0, "API_object", (void *) 0, error_type,   32,   32},
                                  {_CHR_AREA, 0, 0, 0, "text",       (void *)32, error_text, 1024, 1024}
                                          } ;
  static Dcl_complex_type  error_template={ "MIDAS_API_error", 1056, error_data, 2} ;

/*---------------------------------------------------- Внешние связи */

          extern double  dcl_errno ;       /* Системный указатель ошибок -> DCL_SLIB.CPP */

/*********************************************************************/
/*                                                                   */
/*               Создание шаблона сообщения MIDAS API                */
/*                                                                   */
/*  Файл описания API-сообщения:                                     */
/*                                                                   */
/*    Имя файла - <Имя сообщения>.csv                                */
/*                                                                   */
/*    Файл состоит из набора строк, каждая из которых содержит набор */
/*   полей с разделителем ';'. Первое поле содержит имя атрибута     */
/*   сообщения, второе - его длину в символах.                       */
/*    Первая строка файла игнорируется.                              */

   int  Lang_DCL::zMidasApiNew(Dcl_decl  *source, 
                               Dcl_decl **pars, 
                                    int   pars_cnt)

{
  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*               Отправка сообщения MIDAS API                        */
/*                                                                   */
/*  Файл описания API-сообщения:                                     */
/*                                                                   */
/*    Отправляется сообщение, формируемое по записи, у которой       */
/*   API_object='REQUEST'                                            */
/*                                                                   */
/*    Поле опций управления транспортом имеет следующий формат:      */
/*      <вид_транспорта>[:<опция1>=<значение1>; ...]                 */
/*                                                                   */
/*      Допустимые виды транспорта: SOAP                             */
/*                                                                   */
/*      Возможные опции:                                             */
/*                        ALT=<коды альтернативного сообщения>       */
/*                     UPDATE=ALL                                    */
/*                                                                   */
/*   Если в качестве URL задан "NULL" - имитируется успешная         */
/*   отправка                                                        */

   int  Lang_DCL::zMidasApiSend(Dcl_decl  *source,
                                Dcl_decl **pars,
                                     int   pars_cnt)

{
  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*               Извлечение ошибок MIDAS API                         */

   int  Lang_DCL::zMidasApiErrors(Dcl_decl  *source, 
                                  Dcl_decl **pars, 
                                       int   pars_cnt, 
                                      char  *buff,
                                       int   buff_size)

{
  return(0) ;
}

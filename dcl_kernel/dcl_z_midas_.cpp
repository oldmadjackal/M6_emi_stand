/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*          ������� ��� ������ � MIDAS API                           */
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

/*------------------------------------------------- ����� ���������� */

  static             char  error_type[32] ;
  static             char  error_text[1024] ;

  static         Dcl_decl  error_data[2] ={
                                  {_CHR_AREA, 0, 0, 0, "API_object", (void *) 0, error_type,   32,   32},
                                  {_CHR_AREA, 0, 0, 0, "text",       (void *)32, error_text, 1024, 1024}
                                          } ;
  static Dcl_complex_type  error_template={ "MIDAS_API_error", 1056, error_data, 2} ;

/*---------------------------------------------------- ������� ����� */

          extern double  dcl_errno ;       /* ��������� ��������� ������ -> DCL_SLIB.CPP */

/*********************************************************************/
/*                                                                   */
/*               �������� ������� ��������� MIDAS API                */
/*                                                                   */
/*  ���� �������� API-���������:                                     */
/*                                                                   */
/*    ��� ����� - <��� ���������>.csv                                */
/*                                                                   */
/*    ���� ������� �� ������ �����, ������ �� ������� �������� ����� */
/*   ����� � ������������ ';'. ������ ���� �������� ��� ��������     */
/*   ���������, ������ - ��� ����� � ��������.                       */
/*    ������ ������ ����� ������������.                              */

   int  Lang_DCL::zMidasApiNew(Dcl_decl  *source, 
                               Dcl_decl **pars, 
                                    int   pars_cnt)

{
  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*               �������� ��������� MIDAS API                        */
/*                                                                   */
/*  ���� �������� API-���������:                                     */
/*                                                                   */
/*    ������������ ���������, ����������� �� ������, � �������       */
/*   API_object='REQUEST'                                            */
/*                                                                   */
/*    ���� ����� ���������� ����������� ����� ��������� ������:      */
/*      <���_����������>[:<�����1>=<��������1>; ...]                 */
/*                                                                   */
/*      ���������� ���� ����������: SOAP                             */
/*                                                                   */
/*      ��������� �����:                                             */
/*                        ALT=<���� ��������������� ���������>       */
/*                     UPDATE=ALL                                    */
/*                                                                   */
/*   ���� � �������� URL ����� "NULL" - ����������� ��������         */
/*   ��������                                                        */

   int  Lang_DCL::zMidasApiSend(Dcl_decl  *source,
                                Dcl_decl **pars,
                                     int   pars_cnt)

{
  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*               ���������� ������ MIDAS API                         */

   int  Lang_DCL::zMidasApiErrors(Dcl_decl  *source, 
                                  Dcl_decl **pars, 
                                       int   pars_cnt, 
                                      char  *buff,
                                       int   buff_size)

{
  return(0) ;
}

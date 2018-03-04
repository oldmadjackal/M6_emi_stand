/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*                  ������� ��� ������ � EMAIL                       */
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
#include  <process.h>
#include  <windows.h>
#endif

#define   DCL_INSIDE
#include "dcl.h"


#pragma warning(disable : 4996)

/*------------------------------------------------- ����� ���������� */

  static         Dcl_decl  email_data[8] ={
                                  {_CHR_AREA, 0, 0, 0, "$type$",    (void *)    0, "EMAIL",    5,    32},
                                  {_CHR_AREA, 0, 0, 0, "receivers", (void *)   32,  NULL,      0,  4000},
                                  {_CHR_AREA, 0, 0, 0, "sender",    (void *) 4032,  NULL,      0,   300},
                                  {_CHR_AREA, 0, 0, 0, "subject",   (void *) 4332,  NULL,      0,  1000},
                                  {_CHR_AREA, 0, 0, 0, "body",      (void *) 5332,  NULL,      0, 32000},
                                  {_CHR_AREA, 0, 0, 0, "files",     (void *)37332,  NULL,      0,  4000},
                                  {_CHR_AREA, 0, 0, 0, "header",    (void *)41332,  NULL,      0,  4000},
                                  {_CHR_AREA, 0, 0, 0, "error",     (void *)45332,  NULL,      0,  1000}
                                          } ;
  static Dcl_complex_type  email_template={ "EMAIL", 46332, email_data, 8} ;

/*---------------------------------------------------- ������� ����� */

          extern double  dcl_errno ;       /* ��������� ��������� ������ -> DCL_SLIB.CPP */
                                

/*********************************************************************/
/*                                                                   */
/*               ���������� ����� ������ EMAIL                       */
/*                                                                   */
/*  ������ ������� �� 6 �����:                                       */
/*    $type$    - ������ "EMAIL"                                     */
/*    receivers - ������ �����������                                 */
/*    sender    - �����������                                        */
/*    subject   - ���� ������                                        */
/*    body      - ���� ������                                        */
/*    files     - �������� ������-��������                           */
/*    header    - �������������� ���� ���������                      */
/*    error     - ������ ��������� ���������                         */

   int  Lang_DCL::zEMailNew(Dcl_decl  *source, 
                            Dcl_decl **pars, 
                                 int   pars_cnt)

{
  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                    �������� Email-���������                       */

   int  Lang_DCL::zEMailSend(Dcl_decl  *source,
                             Dcl_decl **pars,
                                  int   pars_cnt)

{
  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*               ���������� ������ ������ � EMAIL                    */

   int  Lang_DCL::zEMailErrors(Dcl_decl  *source, 
                               Dcl_decl **pars, 
                                    int   pars_cnt, 
                                   char  *buff,
                                    int   buff_size)
{
  return(0) ;
}


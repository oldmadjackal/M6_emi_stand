/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*          ������� ��� ������ � SQL-���������                       */
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

/*---------------------------------------------------- ������� ����� */

     extern         double  dcl_errno ;              /* ��������� ��������� ������ -> DCL_SLIB.CPP */
     extern class SQL_link *dcl_sql_connect ;        /* ��������� SQL-�������� ��-��������� */ 
     extern           char  dcl_sql_output_ignore ;  /* ���� ������������� �������� ���������� SQL */

/*********************************************************************/
/*                                                                   */
/*              ���������� ����������� SELECT-�������                */

   int  Lang_DCL::zSqlSelectOnce(Dcl_decl  *source, 
                                 Dcl_decl **pars, 
                                      int   pars_cnt)

{
  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*        ���������� DML-��������� (INSERT, UPDATE, DELETE)          */ 
/*                     �� ���� ��������� �������                     */

   int  Lang_DCL::zSqlDmlExecute(Dcl_decl  *source, 
                                 Dcl_decl **pars, 
                                      int   pars_cnt)

{
  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*           ��������� �������� ���������� �������                   */

   int  Lang_DCL::zSqlDescribe(Dcl_decl  *source, 
                               Dcl_decl **pars, 
                                    int   pars_cnt)

{
  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*              ���������� DBF-�����                                 */

   int  Lang_DCL::zDbfRead(Dcl_decl  *source, 
                           Dcl_decl **pars, 
                                int   pars_cnt)

{
  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*                 ���������� ������ � MS OFFICE                     */
/*                                                                   */
/*********************************************************************/

#if defined(AIX) || defined(LINUX)
#ifndef UNIX
#define  UNIX
#endif
#endif


#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef UNIX
#include  <unistd.h>
#define   stricmp  strcasecmp
#define   O_BINARY    0
#else
#include  <io.h>
#include  <direct.h>
#include  "OLE_common.h"
#endif


#define   DCL_INSIDE

#include "dcl.h"


#pragma warning(disable : 4996)

/*---------------------------------------------------- ������� ����� */

          extern double  dcl_errno ;            /* ��������� ��������� ������ -> DCL_SLIB.CPP */
          extern   char  dcl_err_details[512] ; /* ��������� ��������� ������������ �������� ������ -> DCL_SLIB.CPP */

/*------------------------------------------------ "�������" ������� */

    Dcl_decl *Dcl_excel_template(Lang_DCL *, Dcl_decl **, int) ;    /* �������� ������ � Excel-������ */

/*------------------------------------------------ �������� �������� */

	 static double  einval=EINVAL ;      /* �������� ����� ������ */
	 static double   E_OLE= -3 ;

/*--------------------------------------------------- ������� ������ */

     Dcl_decl  dcl_office_lib[]={

               {0, 0, 0, 0, "$PassiveData$", NULL, "olib", 0, 0},

	       {_DGT_VAL, 0, 0, 0, "EINVAL"     , &einval, NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "E_OLE_ERROR", &E_OLE,  NULL, 1, 1},

	       {_DGT_VAL, _DCL_CALL, 0, 0, "excel_template", (void *)Dcl_excel_template, "ssss", 0, 0},

	       {0, 0, 0, 0, "", NULL, NULL, 0, 0}
                               } ;

/*----------------------------------------------- ������� ���������� */



/*****************************************************************/
/*                                                               */
/*            �������� ������ � Excel-������                     */
/*                                                               */
/*   double  excel_template(r_path, t_path, d_path, format) ;    */
/*                                                               */
/*        char *r_path - ���� ����� ����������                   */
/*        char *t_path - ���� ����� �������                      */
/*        char *d_path - ���� ����� � �������                    */
/*        char *macros - ��� ������� ��������                    */
/*                                                               */
/*  ���������� 0 ��� �������� �������� � -1 - � ������ ������    */

  Dcl_decl *Dcl_excel_template(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
         char  r_path[_BUFF_SIZE] ;   /* ���� � ����� ���������� */
         char  t_path[_BUFF_SIZE] ;   /* ���� � ����� ������� */
         char  d_path[_BUFF_SIZE] ;   /* ���� � ����� ������ */
         char  macros[_BUFF_SIZE] ;   /* ��� ������� �������� */
         char  VBA[2000] ;            /* ����������� VBA-��� */
          int  size ;                 /* ������ ���������� */
          int  status ;
         char *error ;
         char  error_details[2000] ;  /* ����� ��� ��������� ������ */

   static   double  ret_value ;          /* ����� ��������� �������� */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

#ifdef UNIX
#else
    OLE_Excel  Excel ;
#endif

/*---------------------------------------------------- ������������� */

          ret_value=0 ;
          dcl_errno=0 ; 

/*------------------------------------------------- ������� �������� */


  if(pars_cnt      <  4  ||
     pars[0]->addr==NULL ||
     pars[1]->addr==NULL ||
     pars[2]->addr==NULL ||
     pars[3]->addr==NULL   ) {
                                            dcl_errno=EINVAL ;
                                            ret_value=-1. ;
                                    return(&dgt_return) ;
                             }
/*-------------------------------------------- ���������� ���������� */

#define   EXTRACT(value, n)  do {                                                 \
                                                          size=pars[n]->size ;    \
                                 if(size>=sizeof(value))  size=sizeof(value)-1 ;  \
                                          memset(value, 0, sizeof(value)) ;       \
                                          memcpy(value, pars[n]->addr, size) ;    \
                               } while(0) 

          EXTRACT(r_path, 0) ;
          EXTRACT(t_path, 1) ;
          EXTRACT(d_path, 2) ;
          EXTRACT(macros, 3) ;

/*-------------------------------------------------- �������� ������ */

#ifdef UNIX

		error="Excel: Not implemented fo Unix" ;

#else

         sprintf(VBA, "Public Sub RnAgentLoadData\n"
                      "%s \"%s\"\n"
                      "End Sub\n", macros, d_path) ;

      do {
                      error= NULL ;

                 memset(error_details, 0, sizeof(error_details)) ;

                  Excel.error_ext     =       error_details ;
                  Excel.error_ext_size=sizeof(error_details)-1 ;

           status=Excel.Activate() ;
        if(status) {  error="Excel: activate error" ;
                                break ;               
                   }
           status=Excel.OpenTemplate(t_path) ;
        if(status) {  error="Excel: open template error" ;
                    Excel.ShowLastError() ;
                      error=error_details ;
                                break ;               
                   }
           status=Excel.CreateMacro(VBA) ;
        if(status) {  error="Excel: macros creating error" ;
                                break ;                
                   }
           status=Excel.ExecuteMacro("RnAgentLoadData") ;
        if(status) {  error="Excel: macros executing error" ;
                                break ;               
                   }
                  Excel.Save(r_path) ;

         } while(0) ;

                  Excel.Terminate() ;

/*--------------------------------------------------------- �������� */

#endif

     if(error!=NULL) {
                                ret_value=E_OLE ;
                                dcl_errno=E_OLE ;
                        strncpy(dcl_err_details, error, sizeof(dcl_err_details)-1) ;
                     }

   return(&dgt_return) ;
}

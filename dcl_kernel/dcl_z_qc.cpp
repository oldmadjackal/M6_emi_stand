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
    int  status ;
                             
/*-------------------------------------------- ���������� ���������� */

/*----------------------------------- ������������ ������� ��������� */

       status=iXobject_add(source, &result_template) ;             /* ��������� ������ */
    if(status)  return(-1) ;

/*-------------------------------------------------------------------*/

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
                char  handler[1024] ;               /* ������������ ����������� */
                char  wfolder[1024] ;               /* ������ ������� ������ */
                char  project[256] ;                /* �������� ������� */
                char  test_id[256] ;                /* ������������� ����� */
                char  request_path[FILENAME_MAX];   /* ���� ����� ������� */
                char  result_path[FILENAME_MAX];    /* ���� ����� ������ */
                FILE *file ;
                 int  return_code ;
                 int  status ;
                 int  size ;
  Dcl_complex_record *record ;
                char *entry ;
                char *macro ;
                char *value ;
                 int  cnt ;
                 int  i ;
                 int  k ;
                 int  n ;

#define  ELEM   record->elems

/*---------------------------------------------------- ������������� */

                       return_code   =0 ;
                        error_text[0]=0 ;

/*-------------------------------------------- ���������� ���������� */

              memset(handler, 0, sizeof(handler)) ;
              memset(wfolder, 0, sizeof(wfolder)) ;
              memset(project, 0, sizeof(project)) ;
              memset(test_id, 0, sizeof(test_id)) ;

                                   size=       pars[0]->size ;      /* ��������� ������������ ����������� */
        if(size>=sizeof(handler))  size=sizeof(handler)-1 ;
                 memcpy(handler, pars[0]->addr, size) ;

                                   size=       pars[1]->size ;      /* ��������� ���� � �������� ������� */
        if(size>=sizeof(wfolder))  size=sizeof(wfolder)-1 ;
                 memcpy(wfolder, pars[1]->addr, size) ;

                                   size=       pars[2]->size ;      /* ��������� �������� ������� */
        if(size>=sizeof(project))  size=sizeof(project)-1 ;
                 memcpy(project,  pars[2]->addr, size) ;

                                   size=       pars[3]->size ;      /* ��������� ������������� ����� */
        if(size>=sizeof(test_id))  size=sizeof(test_id)-1 ;
                 memcpy(test_id, pars[3]->addr, size) ;

/*-------------------------------- ������������ ����� ������� ������ */

          sprintf(request_path, "%s/QC_request.%d", wfolder, GetCurrentThreadId()) ;
          sprintf( result_path, "%s/QC_result.%d",  wfolder, GetCurrentThreadId()) ;

/*--------------------------------- ��������� ������� ���� QC-result */

                             cnt=0 ;

     for(record=(Dcl_complex_record *)source->addr,                 /* LOOP - ���������� ������ -            */
                        i=0 ; i<source->buff ; i++,                 /*         ���� ������ � ����� QC-RESULT */
         record=(Dcl_complex_record *)record->next_record) {
/*- - - - - - - - - - - - - - - - - - - - - - - ������������� ������ */
       for(k=0 ; k<record->elems_cnt ; k++)                         /* ���� ���� $type$ */
         if(!stricmp(ELEM[k].name, "$type$"))  break ;

         if(k>=record->elems_cnt)  continue ;                       /* ���� ����� �� �������... */
         
         if(        ELEM[k].size!=strlen("QC-RESULT") ||            /* ��������� �������� */
             memcmp(ELEM[k].addr, "QC-RESULT", 
                                 strlen("QC-RESULT"))   )  continue ;

                             cnt++ ;
/*- - - - - - - - - - - - - - - - - - -  ��������� �����-����������� */
         for(n=0 ; ; n++) {
                if(n==0) {  macro="$REQUEST$" ;  value=request_path ;  }
           else if(n==1) {  macro="$RESULT$"  ;  value= result_path ;  }
           else                break ;

             while(1) {
                           entry=strstr(handler, macro) ;
                        if(entry==NULL)  break ;

                if(strlen(handler)+
                   strlen(value  )-strlen(macro)>=sizeof(handler)) {
                      sprintf(error_text, "QC-result handler overflow") ;
                                           return(-1) ;
                                                                   }

                if(strlen(value)!=strlen(macro))
                          memmove(entry+strlen(value), 
                                  entry+strlen(macro), strlen(entry)-strlen(macro)+1) ;

                          memmove(entry, value, strlen(value)) ;
                      }
                          }
/*- - - - - - - - - - - - - - - - - - - - ������������ ����� ������� */
            file=fopen(request_path, "wb") ;
         if(file==NULL) {
                 sprintf(error_text, "QC-request file open error %d : %s", errno, request_path) ;
                            return(-1) ;
                       }
                  
                fwrite("Project=", 1, strlen("Project="), file) ; 
                fwrite( project,   1, strlen( project  ), file) ; 
                fwrite("\r\n",     1, strlen("\r\n"    ), file) ; 

                fwrite("Test_Id=", 1, strlen("Test_Id="), file) ; 
                fwrite( test_id,   1, strlen( test_id  ), file) ; 
                fwrite("\r\n",     1, strlen("\r\n"    ), file) ; 

       for(k=0 ; k<record->elems_cnt ; k++) {                       /* ������� ���� �� ������ */
         if(!stricmp(ELEM[k].name, "test_path"))
                fwrite("Test_Path=", 1, strlen("Test_Path="), file) ; 
         else
         if(!stricmp(ELEM[k].name, "result"   ))
                fwrite("Result=",    1, strlen("Result="   ), file) ; 
         else
         if(!stricmp(ELEM[k].name, "bug_desc" ))
                fwrite("Comment=",   1, strlen("Comment="  ), file) ; 
         else      continue ;

                fwrite( ELEM[k].addr, 1, ELEM[k].size,   file) ; 
                fwrite("\r\n",        1, strlen("\r\n"), file) ; 
                                            }

                   fclose(file) ;
/*- - - - - - - - - - - - - - - - - - - - - - ���������� ����������� */
                status=system(handler) ;
             if(status) {
                            sprintf(error_text, "QC-result handler execute error %d : %s", errno, handler) ;
                                    return_code=-1 ;
                                       break ;
                        }
/*- - - - - - - - - - - - - - - - - - - - -  ������ ����� ���������� */
            file=fopen(result_path, "rb") ;
         if(file==NULL) {
                 sprintf(error_text, "QC-result file open error %d : %s", errno, result_path) ;
                            return(-1) ;
                        }
                   
               memset(error_text, 0, sizeof(error_text)) ; 
                fread(error_text, 1, sizeof(error_text)-1, file) ; 
               fclose(file) ;

         if(!stricmp(error_text, "Success"))   error_text[0]=0 ;
         else                                {  return_code=-1 ;
                                                      break ;     }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                           }        /* ENDLOOP - ���������� ������ */

/*------------------------------------------ �������� ������� ������ */

//              unlink(request_path) ;
                unlink( result_path) ;

/*--------------------------- ��������� ���������� ������� QC-result */

     if(cnt==0) {
                  sprintf(error_text, "No QC-results detected") ;
                     return_code=-1 ;
                }
/*-------------------------------------------------------------------*/

#undef     ELEM

  return(return_code) ;
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
                             
/*------------------------------------------------------- ���������� */

                memset(buff, 0, sizeof(buff_size)) ;

/*-------------------------------------------- ���������� ���������� */

/*------------------------------------------------- ������ ��������� */

        memset(buff, 0, buff_size) ;
       strncpy(buff, error_text, buff_size-1) ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


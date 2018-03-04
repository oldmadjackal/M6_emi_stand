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
    int  status ;
                             
/*-------------------------------------------- ���������� ���������� */

/*----------------------------------- ������������ ������� ��������� */

       status=iXobject_add(source, &email_template) ;               /* ��������� ������ */
    if(status)  return(-1) ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                    �������� Email-���������                       */

   int  Lang_DCL::zEMailSend(Dcl_decl  *source,
                             Dcl_decl **pars,
                                  int   pars_cnt)

{
                char  send_method[512] ;            /* ����� �������� */
                char  send_control[1024] ;          /* ��������� ���������� ��������� */
                char  send_folder[FILENAME_MAX] ;   /* ������� ����� */
  Dcl_complex_record *record ;
                char *msg_receivers ;
                char *msg_sender ;
                char *msg_subject ;
                char *msg_body ;
                char *msg_files ;
                char *msg_header ;
                char *msg_error ;
                 int  msg_error_size ;
                char  error[1024] ;
                char  send_path[FILENAME_MAX] ;
                char  done_path[FILENAME_MAX] ;
                FILE *file ;
                 int  ret_code ;
                 int  size ;
                 int  status ;
                char *value ;
                char *end ;
                 int  i ;
                 int  k ;

#define  ELEM   record->elems

/*---------------------------------------------------- ������������� */

                     ret_code=0 ;

/*-------------------------------------------- ���������� ���������� */

#define  PAR  send_method

              memset(PAR, 0, sizeof(PAR)) ;                         

                               size=       pars[0]->size ;      
        if(size>=sizeof(PAR))  size=sizeof(PAR)-1 ;
                 memcpy(PAR, pars[0]->addr, size) ;

#undef   PAR

#define  PAR  send_control

              memset(PAR, 0, sizeof(PAR)) ;                         

                               size=       pars[1]->size ;
        if(size>=sizeof(PAR))  size=sizeof(PAR)-1 ;
                 memcpy(PAR, pars[1]->addr, size) ;

#undef   PAR

#define  PAR  send_folder

              memset(PAR, 0, sizeof(PAR)) ;                         

                               size=       pars[2]->size ;
        if(size>=sizeof(PAR))  size=sizeof(PAR)-1 ;
                 memcpy(PAR, pars[2]->addr, size) ;

#undef   PAR
/*----------------------------------------- ����� ����������� ������ */

     for(record=(Dcl_complex_record *)source->addr,                 /* LOOP - ���������� ������ -        */
                        i=0 ; i<source->buff ; i++,                 /*         ���� ������ � ����� EMAIL */
         record=(Dcl_complex_record *)record->next_record) {
/*- - - - - - - - - - - - - - - - - - - - - - - ������������� ������ */
       for(k=0 ; k<record->elems_cnt ; k++)                         /* ���� ���� $type$ */
         if(!stricmp(ELEM[k].name, "$type$"))  break ;

         if(k>=record->elems_cnt)  continue ;                       /* ���� ����� �� �������... */
         
         if(        ELEM[k].size!=strlen("EMAIL") ||                /* ��������� �������� */
             memcmp(ELEM[k].addr, "EMAIL", 
                                 strlen("EMAIL"))   )  continue ;
/*- - - - - - - - - - - - - - - - - - - - - - - -  ����� ���� ERROR */
       for(k=0 ; k<record->elems_cnt ; k++)
         if(!stricmp(ELEM[k].name, "error"))
               memset(ELEM[k].addr, 0, ELEM[k].buff) ;                                            
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                           }        /* ENDLOOP - ���������� ������ */
/*----------------------------------------------- �������� ��������� */

     for(record=(Dcl_complex_record *)source->addr,                 /* LOOP - ���������� ������ -        */
                        i=0 ; i<source->buff ; i++,                 /*         ���� ������ � ����� EMAIL */
         record=(Dcl_complex_record *)record->next_record) {

           memset(error, 0, sizeof(error)) ;
/*- - - - - - - - - - - - - - - - - - - - - - - ������������� ������ */
       for(k=0 ; k<record->elems_cnt ; k++)                         /* ���� ���� $type$ */
         if(!stricmp(ELEM[k].name, "$type$"))  break ;

         if(k>=record->elems_cnt)  continue ;                       /* ���� ����� �� �������... */
         
         if(        ELEM[k].size!=strlen("EMAIL") ||                /* ��������� �������� */
             memcmp(ELEM[k].addr, "EMAIL", 
                                 strlen("EMAIL"))   )  continue ;      
/*- - - - - - - - - - - - - - - - - - - - - - ��������� ������ ����� */
                 msg_receivers=NULL ;
                 msg_sender   =NULL ;
                 msg_subject  =NULL ;
                 msg_body     =NULL ;
                 msg_files    =NULL ;
                 msg_header   =NULL ;
                 msg_error    =NULL ;

       for(k=0 ; k<record->elems_cnt ; k++)
         if(!stricmp(ELEM[k].name, "receivers")) {
            if(ELEM[k].size>=ELEM[k].buff)  ELEM[k].size=ELEM[k].buff-1 ;
                 msg_receivers              =(char *)ELEM[k].addr ;
                 msg_receivers[ELEM[k].size]=         0 ;
                                                 }
         else
         if(!stricmp(ELEM[k].name, "sender"   )) {
            if(ELEM[k].size>=ELEM[k].buff)  ELEM[k].size=ELEM[k].buff-1 ;
                 msg_sender              =(char *)ELEM[k].addr ;
                 msg_sender[ELEM[k].size]=         0 ;
                                                 }
         else
         if(!stricmp(ELEM[k].name, "subject"  )) {
            if(ELEM[k].size>=ELEM[k].buff)  ELEM[k].size=ELEM[k].buff-1 ;
                 msg_subject              =(char *)ELEM[k].addr ;
                 msg_subject[ELEM[k].size]=         0 ;
                                                 }
         else
         if(!stricmp(ELEM[k].name, "body"     )) {
            if(ELEM[k].size>=ELEM[k].buff)  ELEM[k].size=ELEM[k].buff-1 ;
                 msg_body              =(char *)ELEM[k].addr ;
                 msg_body[ELEM[k].size]=         0 ;
                                                 }
         else
         if(!stricmp(ELEM[k].name, "files"    )) {
            if(ELEM[k].size>=ELEM[k].buff)  ELEM[k].size=ELEM[k].buff-1 ;
                 msg_files              =(char *)ELEM[k].addr ;
                 msg_files[ELEM[k].size]=         0 ;
                                                 }
         else
         if(!stricmp(ELEM[k].name, "header"   )) {
            if(ELEM[k].size>=ELEM[k].buff)  ELEM[k].size=ELEM[k].buff-1 ;
                 msg_header              =(char *)ELEM[k].addr ;
                 msg_header[ELEM[k].size]=         0 ;
                                                 }
         else
         if(!stricmp(ELEM[k].name, "error"    )) {
                 msg_error    =(char *)ELEM[k].addr ;
                 msg_error_size=       ELEM[k].buff-1 ;
                                                 }
/*- - - - - - - - - - - - - - - - - - -  �������� ������������ ����� */
       if( msg_receivers==NULL ||
          *msg_receivers==  0    ) {
               if(*error)  strcat(error, ", ") ;      
                           strcat(error, "RECEIVERS is missed") ;
                                   }
       if( msg_sender   ==NULL ||
          *msg_sender   ==  0    ) {
               if(*error)  strcat(error, ", ") ;      
                           strcat(error, "SENDER is missed") ;
                                   }
       if( msg_subject  ==NULL ||
          *msg_subject  ==  0    )  msg_subject="EMPTY" ;
       if( msg_body     ==NULL ||
          *msg_body     ==  0    )  msg_body   ="EMPTY" ;

       if(*error) {
          if(msg_error!=NULL)  strncpy(msg_error, error, msg_error_size) ;
                                                 ret_code=-1 ;
                                                    continue ;
                  }
/*- - - - - - - - - - - - - - - - - - - - - ����� �������� EXTERNAL@ */
       if(!stricmp(send_method, "EXTERNAL@")) {

        do {                                                        /* BLOCK */
      
             if(send_folder[0]==0) {                                /* �������� ������� ������� ����� */
                                 sprintf(error, "Work folder not assigned") ;
                                                    break ;
                                   }

                   sprintf(send_path, "%s\\%d.email.send",          /* ������ ���� ������� ������ */
                                         send_folder, getpid()) ;
                   sprintf(done_path, "%s\\%d.email.done", 
                                         send_folder, getpid()) ;

                file=fopen(send_path, "wb") ;                       /* ��������� ���� ��������� */
             if(file==NULL) {
                  sprintf(error, "Send file open error %d : %s", errno, send_path) ;
                                 break ;
                            }  

             for(value=msg_receivers, end=value ;                   /* ����� RECEIVERS */  
                       end!=NULL && value[0]!=0 ; value=end+1) {
                       end=strchr(value, ',') ;
                    if(end!=NULL)  *end=0 ;
                         fwrite("RECEIVER=" , 1, strlen("RECEIVER=" ), file) ;
                         fwrite( value      , 1, strlen( value      ), file) ;
                         fwrite("\r\n"      , 1, strlen("\r\n"      ), file) ;
                                                               }

                         fwrite("SENDER="   , 1, strlen("SENDER="   ), file) ;
                         fwrite( msg_sender , 1, strlen( msg_sender ), file) ;
                         fwrite("\r\n"      , 1, strlen("\r\n"      ), file) ;

                         fwrite("SUBJECT=",   1, strlen("SUBJECT="  ), file) ;
                         fwrite( msg_subject, 1, strlen( msg_subject), file) ;
                         fwrite("\r\n"      , 1, strlen("\r\n"      ), file) ;

             for(value=msg_body, end=value ;                        /* ����� BODY */
                       end!=NULL && value[0]!=0 ; value=end+1) {
                       end=strchr(value, '\r') ;
                    if(end!=NULL)  strcpy(end, end+1) ;
                       end=strchr(value, '\n') ;
                    if(end!=NULL)  *end=0 ;
                         fwrite("BODY="     , 1, strlen("BODY="     ), file) ;
                         fwrite( value      , 1, strlen( value      ), file) ;
                         fwrite("\r\n"      , 1, strlen("\r\n"      ), file) ;
                                                               }

             for(value=msg_files, end=value ;                       /* ����� FILES */
                       end!=NULL && value[0]!=0 ; value=end+1) {
                       end=strchr(value, ',') ;
                    if(end!=NULL)  *end=0 ;
                         fwrite("FILE="     , 1, strlen("FILE="     ), file) ;
                         fwrite( value      , 1, strlen( value      ), file) ;
                         fwrite("\r\n"      , 1, strlen("\r\n"      ), file) ;
                                                               }

             for(value=msg_header, end=value ;                      /* ����� HEADER */
                       end!=NULL && value[0]!=0 ; value=end+1) {
                       end=strchr(value, '\r') ;
                    if(end!=NULL)  strcpy(end, end+1) ;
                       end=strchr(value, '\n') ;
                    if(end!=NULL)  *end=0 ;
                         fwrite("HEADER="   , 1, strlen("HEADER="   ), file) ;
                         fwrite( value      , 1, strlen( value      ), file) ;
                         fwrite("\r\n"      , 1, strlen("\r\n"      ), file) ;
                                                               }

                         fwrite("SEND MESSAGE", 1,                  /* �������� ����� */ 
                                    strlen("SEND MESSAGE"), file) ; 
                         fclose(file) ;                             /* ��������� ���� ��������� */

               size=strlen("%send_path%") ;
                end=strstr(send_control, "%send_path%") ;           /* ���� � ��������� ������ ������ ����� ��������� */
             if(end==NULL) {                                        /* ���� �� ������... */
                  sprintf(error, "Send file marker is missed in template of send command line") ;
                                 break ;
                           }
                   memmove(end+strlen(send_path),                   /* ��������� ���� ����� ��������� */
                           end+size, strlen(end+size)+2) ;
                    memcpy(end, send_path, strlen(send_path)) ;     

               size=strlen("%done_path%") ;
                end=strstr(send_control, "%done_path%") ;           /* ���� � ��������� ������ ������ ����� ������ */
             if(end==NULL) {                                        /* ���� �� ������... */
                  sprintf(error, "Done file marker is missed in template of send command line") ;
                                 break ;
                           }
                   memmove(end+strlen(send_path),                   /* ��������� ���� ����� ������ */
                           end+size, strlen(end+size)+2) ;
                   memmove(end, done_path, strlen(done_path)) ;     

                status=system(send_control) ;                       /* ��������� ������� �������� */ 
             if(status) {
                  sprintf(error, "Send execute error: rc=%d  errno=%d", status, errno) ;
                                 break ;
                        }

                file=fopen(done_path, "rb") ;                       /* ��������� ���� ������ */
             if(file==NULL) {
                  sprintf(error, "Done file open error %d : %s", errno, done_path) ;
                                 break ;
                            }  

                    memset(error, 0, sizeof(error)) ;
                     fread(error, 1, sizeof(error)-1, file) ;       /* ��������� ���� ������ */
                    fclose(file) ;                                  /* ��������� ���� ������ */

             if(*error==0)                                          /* ���� ���� ������ ������ */
                    strcpy(error,  "Result file is empty") ;

             if(!stricmp(error, "SUCCESS"))  *error=0 ;             /* ���� ������� ����� ��������� ����������... */

           } while(0) ;                                             /* BLOCK */

                    unlink(send_path) ;
                    unlink(done_path) ;
               
         if(error[0]!=0) {                                          /* �������� ������ */
            if(msg_error!=NULL)  strncpy(msg_error, error, msg_error_size) ;
                                     ret_code=-1 ;
                                       continue ;
                         }
                                              }
/*- - - - - - - - - - - - - - - - - - - - ����������� ����� �������� */
       else                                   {

                               sprintf(    error, "Send method <%s> unsupported", send_method) ;
          if(msg_error!=NULL)  strncpy(msg_error, error, msg_error_size) ;
                                                 ret_code=-1 ;
                                                    continue ;
                                              }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                           }        /* ENDLOOP - ���������� ������ */

/*---------------------------------- ������������ ����������� ������ */

     for(record=(Dcl_complex_record *)source->addr,                 /* LOOP - ���������� ������ -        */
                        i=0 ; i<source->buff ; i++,                 /*         ���� ������ � ����� EMAIL */
         record=(Dcl_complex_record *)record->next_record) {
/*- - - - - - - - - - - - - - - - - - - - - - - ������������� ������ */
       for(k=0 ; k<record->elems_cnt ; k++)                         /* ���� ���� $type$ */
         if(!stricmp(ELEM[k].name, "$type$"))  break ;

         if(k>=record->elems_cnt)  continue ;                       /* ���� ����� �� �������... */
         
         if(        ELEM[k].size!=strlen("EMAIL") ||                /* ��������� �������� */
             memcmp(ELEM[k].addr, "EMAIL", 
                                 strlen("EMAIL"))   )  continue ;
/*- - - - - - - - - - - - - - - - - - - - - - - -  ����� ���� ERROR */
       for(k=0 ; k<record->elems_cnt ; k++)
         if(!stricmp(ELEM[k].name, "error"))  ELEM[k].size=strlen((char *)ELEM[k].addr) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                           }        /* ENDLOOP - ���������� ������ */
/*-------------------------------------------------------------------*/

#undef     ELEM

  return(ret_code) ;
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
  Dcl_complex_record *record ;
                 int  size ;
                 int  i ;
                 int  k ;

#define  ELEM   record->elems
                             
/*------------------------------------------------------- ���������� */

                memset(buff, 0, sizeof(buff_size)) ;

/*---------------------------------------- ������ ������� ���� EMAIL */

     for(record=(Dcl_complex_record *)source->addr,                 /* LOOP - ���������� ������ -        */
                        i=0 ; i<source->buff ; i++,                 /*         ���� ������ � ����� EMAIL */
         record=(Dcl_complex_record *)record->next_record) {
/*- - - - - - - - - - - - - - - - - - - - - - - ������������� ������ */
       for(k=0 ; k<record->elems_cnt ; k++)                         /* ���� ���� $type$ */
         if(!stricmp(ELEM[k].name, "$type$"))  break ;

         if(k>=record->elems_cnt)  continue ;                       /* ���� ����� �� �������... */
         
         if(        ELEM[k].size!=strlen("EMAIL") ||                /* ��������� �������� */
             memcmp(ELEM[k].addr, "EMAIL", 
                                 strlen("EMAIL"))   )  continue ;
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������ ���� ERROR */
       for(k=0 ; k<record->elems_cnt ; k++)                         /* ������� ���� �� ������ */
         if(!stricmp(ELEM[k].name, "error")) {

                 size=ELEM[k].size ;
              if(size==0)  break ;                                  /* �������� ������� ������ */

              if(strlen(buff)+size>=buff_size-3)  break ;           /* �������� ������������ */

              if(buff[0]!=0)  strcat(buff, "; ") ;                  /* ���� �� ������ ������ - ��������� ����������� */

                 memcpy(buff+strlen(buff), ELEM[k].addr, size) ;    /* ����� ������ � �������� ����� */

                        break ;
                                             }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                           }        /* ENDLOOP - ���������� ������ */
/*-------------------------------------------------------------------*/

#undef     ELEM

  return(0) ;
}


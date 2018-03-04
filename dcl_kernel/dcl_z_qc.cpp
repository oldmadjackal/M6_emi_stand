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
    int  status ;
                             
/*-------------------------------------------- Извлечение параметров */

/*----------------------------------- Формирование шаблона сообщения */

       status=iXobject_add(source, &result_template) ;             /* Добавляем запись */
    if(status)  return(-1) ;

/*-------------------------------------------------------------------*/

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
                char  handler[1024] ;               /* Спецификация обработчика */
                char  wfolder[1024] ;               /* Раздел рабочих файлов */
                char  project[256] ;                /* Название проекта */
                char  test_id[256] ;                /* Идентификатор теста */
                char  request_path[FILENAME_MAX];   /* Путь файла запроса */
                char  result_path[FILENAME_MAX];    /* Путь файла ответа */
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

/*---------------------------------------------------- Инициализация */

                       return_code   =0 ;
                        error_text[0]=0 ;

/*-------------------------------------------- Извлечение параметров */

              memset(handler, 0, sizeof(handler)) ;
              memset(wfolder, 0, sizeof(wfolder)) ;
              memset(project, 0, sizeof(project)) ;
              memset(test_id, 0, sizeof(test_id)) ;

                                   size=       pars[0]->size ;      /* Извлекаем спецификацию обработчика */
        if(size>=sizeof(handler))  size=sizeof(handler)-1 ;
                 memcpy(handler, pars[0]->addr, size) ;

                                   size=       pars[1]->size ;      /* Извлекаем путь к рабочему разделу */
        if(size>=sizeof(wfolder))  size=sizeof(wfolder)-1 ;
                 memcpy(wfolder, pars[1]->addr, size) ;

                                   size=       pars[2]->size ;      /* Извлекаем название проекта */
        if(size>=sizeof(project))  size=sizeof(project)-1 ;
                 memcpy(project,  pars[2]->addr, size) ;

                                   size=       pars[3]->size ;      /* Извлекаем идентификатор теста */
        if(size>=sizeof(test_id))  size=sizeof(test_id)-1 ;
                 memcpy(test_id, pars[3]->addr, size) ;

/*-------------------------------- Формирование путей рабочих файлов */

          sprintf(request_path, "%s/QC_request.%d", wfolder, GetCurrentThreadId()) ;
          sprintf( result_path, "%s/QC_result.%d",  wfolder, GetCurrentThreadId()) ;

/*--------------------------------- Обработка записей типа QC-result */

                             cnt=0 ;

     for(record=(Dcl_complex_record *)source->addr,                 /* LOOP - Перебираем записи -            */
                        i=0 ; i<source->buff ; i++,                 /*         ищем запись с типом QC-RESULT */
         record=(Dcl_complex_record *)record->next_record) {
/*- - - - - - - - - - - - - - - - - - - - - - - Идентификация записи */
       for(k=0 ; k<record->elems_cnt ; k++)                         /* Ищем поле $type$ */
         if(!stricmp(ELEM[k].name, "$type$"))  break ;

         if(k>=record->elems_cnt)  continue ;                       /* Если такое не найдено... */
         
         if(        ELEM[k].size!=strlen("QC-RESULT") ||            /* Проверяем значение */
             memcmp(ELEM[k].addr, "QC-RESULT", 
                                 strlen("QC-RESULT"))   )  continue ;

                             cnt++ ;
/*- - - - - - - - - - - - - - - - - - -  Раскрытие макро-подстановок */
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
/*- - - - - - - - - - - - - - - - - - - - Формирование файла запроса */
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

       for(k=0 ; k<record->elems_cnt ; k++) {                       /* Выводим поля из записи */
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
/*- - - - - - - - - - - - - - - - - - - - - - Исполнение обработчика */
                status=system(handler) ;
             if(status) {
                            sprintf(error_text, "QC-result handler execute error %d : %s", errno, handler) ;
                                    return_code=-1 ;
                                       break ;
                        }
/*- - - - - - - - - - - - - - - - - - - - -  Анализ файла результата */
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
                                                           }        /* ENDLOOP - Перебираем записи */

/*------------------------------------------ Удаление рабочих файлов */

//              unlink(request_path) ;
                unlink( result_path) ;

/*--------------------------- Обработка отсутствия записей QC-result */

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
/*               Извлечение ошибок работы с QC                       */

   int  Lang_DCL::zQC_Errors(Dcl_decl  *source, 
                             Dcl_decl **pars, 
                                  int   pars_cnt, 
                                 char  *buff,
                                  int   buff_size)
{
                             
/*------------------------------------------------------- Подготовка */

                memset(buff, 0, sizeof(buff_size)) ;

/*-------------------------------------------- Извлечение параметров */

/*------------------------------------------------- Выдача сообщений */

        memset(buff, 0, buff_size) ;
       strncpy(buff, error_text, buff_size-1) ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


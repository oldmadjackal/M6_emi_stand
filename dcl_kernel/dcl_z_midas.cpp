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

#include "sql_common.h"
#include "abtp_exh.h"
#include "abtp_tcp.h"

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

/*---------------------------- Функции для работы с MIDAS API и SOAP */

  void  MidasAPI_SOAPHeader   (char *, char *, char *) ;         /* Формирование заголовка SOAP-сообщения для MIDAS API */
  void  MidasAPI_SOAPFooter   (char *) ;                         /* Формирование окончания SOAP-сообщения для MIDAS API */
  void  MidasAPI_SOAPBody     (char *, char *, char *,           /* Формирование тела SOAP-сообщения для MIDAS API */
                               char *, char *, SQL_parameter *) ;
   int  MidasAPI_SOAPReply    (char *, char *,                   /* Разбор тела SOAP-ответа от MIDAS API */
                                SQL_parameter *, int, char *) ;
   int  MidasAPI_SOAPGetData  (char *, char *,                   /* Разбор секции данных от MIDAS API */
                                SQL_parameter *, int, char *) ;
   int  MidasAPI_SOAPGetErrors(char *, char *,                   /* Разбор секции диагностических сообщений от MIDAS API */
                                SQL_parameter *, int, char *) ;
   int  Http_SOAP_send        (char *, char *, char *,           /* Отправка SOAP-сообщения по HTTP */
                                        int  , char *, char *) ;
   int  Http_Receive_ctrl     (char *, int) ;                    /* Функция управления приемом для HTTP */
   int  Http_Base64_incode    (char *, char *, int) ;            /* Упаковка бинарных данных по Base64 */
   int  Http_Base64_decode    (char *, char *, int) ;            /* Распаковка бинарных данных по Base64 */
                               

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
              char  msg_name[64] ;     /* Имя API-сообщения */
              char  API_path[1024] ;   /* Путь к разделу файлов описания API-сообщений */
              char  msg_path[1024] ;   /* Путь к файлу описания нужного сообщения */
              FILE *file ;
              char *work ;
              char *error ;
              char *end ;
              char  tmp[1024] ;
               int  status ;
               int  size ;
               int  row ;
               int  j ;

          Dcl_decl *rec_data ;
               int  rec_cnt ;
  Dcl_complex_type  rec_template={ "MIDAS_API_message", 0, 0, 0} ;
                              
/*-------------------------------------------- Извлечение параметров */

          memset(msg_name, 0, sizeof(msg_name)) ;
          memset(API_path, 0, sizeof(API_path)) ;

                                    size=       pars[0]->size ;     /* Извлекаем имя сообщения */
        if(size>=sizeof(msg_name))  size=sizeof(msg_name)-1 ;
                 memcpy(msg_name, pars[0]->addr, size) ;

                                    size=       pars[1]->size ;     /* Извлекаем путь к разделу файлов описания API-сообщений */
        if(size>=sizeof(API_path))  size=sizeof(API_path)-1 ;
                 memcpy(API_path, pars[1]->addr, size) ;

/*----------------------------------------- Удаление записей объекта */

                     iXobject_clear(source) ;

/*------------------------------- Прямое задание структуры сообщения */

   if(API_path[0]=='@') {
                           rec_data=NULL ;
                           rec_cnt =  0 ;
                              error=NULL ;

       for(work=API_path+1 ; ; work=end+1) {                        /* LOOP - Разбираем спецификацию по фрагментам */
/*- - - - - - - - - - - - - - - - - - - - - Извлекаем параметры поля */
             end=strchr(work, ':') ;                                /* Ищем разделитель Имя/Размер */
          if(end==NULL) {   error="Missed NAME/SIZE separator" ;
                                      break ;                    }

            *end=0 ;                                                /* Выделяем имя поля */

             size=strtoul(end+1, &end, 10) ;                        /* Выделяем размер поля */
          if(size==0) {   error="Invalid SIZE parameter" ;
                                      break ;                    }
          if(*end!= 0  &&
             *end!=','   )  {   error="Invalid FIELDS separator" ;
                                      break ;                    }
/*- - - - - - - - - - - - - - - - - - - - - - - -  Регистрируем поле */
           rec_data=(Dcl_decl *)                                    /* Размещаем описание полей */
                     realloc(rec_data, (rec_cnt+1)*sizeof(*rec_data)) ;

                memset(&rec_data[rec_cnt], 0, sizeof(*rec_data)) ;

                        rec_data[rec_cnt].type=_CHR_AREA ;          /* Регистрируем поле */
                strncpy(rec_data[rec_cnt].name, work, sizeof(rec_data->name)-1) ;
                        rec_data[rec_cnt].buff=size ;
                        rec_data[rec_cnt].size= -1 ;
                                 rec_cnt++ ; 
/*- - - - - - - - - - - - - - - - - - - - - - - -  Анализ завершения */
          if(*end==0)  break ;                                      /* Если спецификация полностью разобрана */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                           }                        /* ENDLOOP - Разбираем спецификацию по фрагментам */

     if(error!=NULL) {
                      strcpy(error_type, "DRIVER_ERROR") ;
                     sprintf(error_text, "Message specification error %s : %-20.20s", error, work) ;
                             error_data[0].size=strlen(error_data[0].prototype) ;
                             error_data[1].size=strlen(error_data[1].prototype) ;
                iXobject_add(source, &error_template) ;

                             free(rec_data) ;
                                return(-1) ;
                     }
                        }
/*------------------------------ Считывание файла описания сообщения */

   else                 {
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Открытие файла */
           sprintf(msg_path, "%s/%s.csv", API_path, msg_name) ;     /* Формируем путь к файлу */

        file=fopen(msg_path, "rb") ;                                /* Открываем файл описания */
     if(file==NULL) {                                               /* Если ошибка */
                      strcpy(error_type, "DRIVER_ERROR") ;
                     sprintf(error_text, "Message specification file open error %d : %s", errno, msg_path) ;
                             error_data[0].size=strlen(error_data[0].prototype) ;
                             error_data[1].size=strlen(error_data[1].prototype) ;
                iXobject_add(source, &error_template) ;
                                return(-1) ;
                    }
/*- - - - - - - - - - - - - - - - - - - - - - - - - Считывание файла */
                           rec_data=NULL ;
                           rec_cnt =  0 ;
                              error=NULL ;
                                row=  0 ;

     do {                                                           /* LOOP - Построчно считываем файл */
             end=fgets(tmp, sizeof(tmp), file) ;                    /* Считываем очередную строку */
          if(end==NULL)  break ;                                    /* Если файл закончился */

             row++ ;
          if(row==1)  continue ;                                    /* Пропускаем первую строку */

             end=strchr(tmp, ';') ;                                 /* Ищем разделитель Имя/Размер */
          if(end==NULL) {   error="Missed NAME separator" ;
                                      break ;                    }

            *end=0 ;                                                /* Выделяем имя поля */

             size=strtoul(end+1, &end, 10) ;                        /* Выделяем размер поля */
          if(size==0) {   error="Invalid SIZE parameter" ;
                                      break ;                    }
          if(*end!= 0   &&
             *end!=';'  &&
             *end!='\r' &&
             *end!='\n'   ) {  error="Invalid SIZE separator" ;
                                         break ;                  }

           rec_data=(Dcl_decl *)                                    /* Размещаем описание полей */
                     realloc(rec_data, (rec_cnt+1)*sizeof(*rec_data)) ;

                memset(&rec_data[rec_cnt], 0, sizeof(*rec_data)) ;

                        rec_data[rec_cnt].type=_CHR_AREA ;          /* Регистрируем поле */
                strncpy(rec_data[rec_cnt].name, tmp, sizeof(rec_data->name)-1) ;
                        rec_data[rec_cnt].buff=size ;
                        rec_data[rec_cnt].size= -1 ;
                                 rec_cnt++ ; 

        } while(1) ;                                                /* ENDLOOP - Построчно считываем файл */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Закрытие файла */
                                fclose(file) ;                      /* Закрываем файл описания */
/*- - - - - - - - - - - - - - - - - - - - - - - - - Обработка ошибок */
     if(error!=NULL) {
                      strcpy(error_type, "DRIVER_ERROR") ;
                     sprintf(error_text, "Message specification file error (row %d) - %s", row, error) ;
                             error_data[0].size=strlen(error_data[0].prototype) ;
                             error_data[1].size=strlen(error_data[1].prototype) ;
                iXobject_add(source, &error_template) ;

                             free(rec_data) ;
                                return(-1) ;
                     }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                        }
/*----------------------------------------------- Проверка структуры */

                     error= NULL ;
 
     if(rec_cnt<=0)  error="Empty template specified" ;

     if(error!=NULL) {
                      strcpy(error_type, "DRIVER_ERROR") ;
                     sprintf(error_text, "Message specification error - %s", error) ;
                             error_data[0].size=strlen(error_data[0].prototype) ;
                             error_data[1].size=strlen(error_data[1].prototype) ;
                iXobject_add(source, &error_template) ;

                             free(rec_data) ;
                                return(-1) ;
                     }
/*----------------------------------- Формирование шаблона сообщения */
/*- - - - - - - - - - - - - - - - - - - - - - - Добавление поля AUTH */
      for(j=0 ; j<rec_cnt ; j++)                                    /* Ищем поле AUTH */
        if(!stricmp(rec_data[j].name, "AUTH"))  break ;

        if(j>=rec_cnt) {                                            /* Если такого поля нет - добавляем его */
           rec_data=(Dcl_decl *)                    
                     realloc(rec_data, (rec_cnt+1)*sizeof(*rec_data)) ;

                memset(&rec_data[rec_cnt], 0, sizeof(*rec_data)) ;

                        rec_data[rec_cnt].type=_CHR_AREA ;
                 strcpy(rec_data[rec_cnt].name, "AUTH") ;
                        rec_data[rec_cnt].buff=  1  ;
                        rec_data[rec_cnt].size= -1 ;
                                 rec_cnt++ ; 
                       } 
/*- - - - - - - - - - - -  Добавление полей API_object и API_message */
         rec_data=(Dcl_decl *)
                   realloc(rec_data, (rec_cnt+2)*sizeof(*rec_data)) ;

            memmove(rec_data+2, rec_data, rec_cnt*sizeof(*rec_data)) ;
                    rec_cnt+=2 ;

                    rec_data[0].type     =_CHR_AREA ;               /* Регистрируем поле API_object */
             strcpy(rec_data[0].name,     "API_object") ;
                    rec_data[0].prototype=       "REQUEST" ;
                    rec_data[0].buff     =strlen("REQUEST")+1 ;
                    rec_data[0].size     =strlen("REQUEST") ;

                    rec_data[1].type     =_CHR_AREA ;               /* Регистрируем поле API_message */
             strcpy(rec_data[1].name,     "API_message") ;
                    rec_data[1].prototype=       msg_name ;
                    rec_data[1].buff     =strlen(msg_name)+1 ;
                    rec_data[1].size     =strlen(msg_name) ;
/*- - - - - - - - - - - - - - - - -  Собственно формирование шаблона */
      for(size=0, j=0 ; j<rec_cnt ; j++) {                          /* Привязываем данные полей */
                           rec_data[j].addr =(void *)(size+1) ;     /*   К каждому полю добавляем 1 скрытый символ */
                                       size+=rec_data[j].buff+1 ;   /*    для последующей вставки 0-терминатора    */
                                         }

                    rec_template.list    =rec_data ;                /* Готовим шаблон записи */
                    rec_template.list_cnt=rec_cnt ;
                    rec_template.size    =  size ;

             status=iXobject_add(source, &rec_template) ;           /* Добавляем запись */
                           free(rec_data) ;
          if(status) {
                      strcpy(error_type, "DRIVER_ERROR") ;
                     sprintf(error_text, "Message template creation fail") ;
                             error_data[0].size=strlen(error_data[0].prototype) ;
                             error_data[1].size=strlen(error_data[1].prototype) ;
                iXobject_add(source, &error_template) ;
                                return(-1) ;
                     }
/*-------------------------------------------------------------------*/

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
                char  action[64] ;                /* Исполняемое действие */
                char  ctrl[1024] ;                /* Управление транспортом */
                char  login[64] ;                 /* Логин: пользователь/пароль */
                char  url[1024] ;                 /* URL получателя */
                char  msg_code[64] ;              /* Код сообщения */
                char  to_kind[64] ;               /* Опции управления: вид транспорта  */
                char  to_alt[64] ;                /* Опции управления: ALT - код "альтернативного" сообщения */
                char  to_update[64] ;             /* Опции управления: UPDATE - режим изменения данных */
                char  log_folder[FILENAME_MAX] ; 
                 int  return_code ;
                 int  status ;
                 int  size ;
                char *buff ;
       SQL_parameter *fields ;
                 int  fields_cnt ;
                char *error ;
  Dcl_complex_record *record ;
                char *entry ;
                char *next ;
                char *value ;
                char *end ;
                 int  i ;
                 int  j ;
                 int  k ;

            Dcl_decl *rec_data ;
                 int  rec_cnt ;
    Dcl_complex_type  rec_template={ "MIDAS_API_message", 0, 0, 0} ;

#define    _SOAP_SIZE  128000
#define  _FIELDS_MAX     1000

#define  ELEM   record->elems

/*---------------------------------------------------- Инициализация */

                       return_code=  0 ;
                             error=NULL ;

/*------------------------------------------ Определение папки логов */

       memset(log_folder, 0, sizeof(log_folder)) ;
      strncpy(log_folder, this->mDebug_path, sizeof(log_folder)-1) ;

                     end   =strrchr(log_folder, '\\') ;
      if(end==NULL)  end   =strrchr(log_folder, '/') ;
      if(end==NULL)  end   =log_folder-1 ;
                     end[1]= 0 ; 

/*-------------------------------------------- Извлечение параметров */

          memset(action, 0, sizeof(action)) ;
          memset(ctrl,   0, sizeof(ctrl)) ;
          memset(login,  0, sizeof(login)) ;
          memset(url,    0, sizeof(url)) ;

                                  size=       pars[0]->size ;       /* Извлекаем код действия */
        if(size>=sizeof(action))  size=sizeof(action)-1 ;
                 memcpy(action, pars[0]->addr, size) ;

                                  size=       pars[1]->size ;       /* Извлекаем опции управления транспортом */
        if(size>=sizeof(ctrl  ))  size=sizeof(ctrl)-1 ;
                 memcpy(ctrl,  pars[1]->addr, size) ;

                                  size=       pars[2]->size ;       /* Извлекаем логин */
        if(size>=sizeof(login ))  size=sizeof(login)-1 ;
                 memcpy(login, pars[2]->addr, size) ;

                                  size=       pars[3]->size ;       /* Извлекаем URL */
        if(size>=sizeof(url   ))  size=sizeof(url)-1 ;
                 memcpy(url, pars[3]->addr, size) ;

/*--------------------------------- Удаление старых ответных записей */

     for(record=(Dcl_complex_record *)source->addr,                 /* LOOP - Размечаем записи со значениями */
                        i=0 ; i<source->buff ; i++,                 /*         в поле API_object не REQUEST  */
         record=(Dcl_complex_record *)record->next_record) {

                          record->exec_mark=0 ;
         
       for(k=0 ; k<record->elems_cnt ; k++)                         /* Ищем поле API_object */
         if(!stricmp(ELEM[k].name, "API_object"))  break ;

         if(k>=record->elems_cnt)  continue ;                       /* Если такое не найдено... */
         
         if(        ELEM[k].size==strlen("REQUEST") &&              /* Если API_object=REQUEST - пропускаем */
            !memcmp(ELEM[k].addr, "REQUEST", 
                                 strlen("REQUEST"))   )  continue ;

                         record->exec_mark=1 ;
                                                     }              /* ENDLOOP */


                  iXobject_delete(source) ;                         /* Удаление размеченных записей */

/*------------------------------- Разбор и контроль опций управления */

               memset(to_kind,   0, sizeof(to_kind)) ;
               memset(to_alt,    0, sizeof(to_alt)) ;
               memset(to_update, 0, sizeof(to_update)) ;

    do {                                                            /* BLOCK */

           end=strchr(ctrl, ':') ;                                  /* Вырезаем описатель вида транспорта */ 
        if(end!=NULL) {   memcpy(to_kind, ctrl, end-ctrl) ;
                          strcpy(ctrl, end+1) ;                     }
        else          {  strncpy(to_kind, ctrl, sizeof(to_kind)-1) ;
                                 ctrl[0]=0 ;                        }

         for(entry=ctrl ; *entry!=0 ; entry=next+1) {               /* LOOP - Перебираем опции */

                next=strchr(entry, ';') ;                           /* Отсекаем опцию */
             if(next!=NULL)  *next=0 ;

                value=strchr(entry, '=') ;                          /* Разделяем имя/значение */
             if(value!=NULL)  *value=0 ;

             if(!memicmp(entry, "ALT", sizeof("ALT"))) {            /* Опция ALT */
               if(value!=NULL)  strncpy(to_alt, value+1, sizeof(to_alt)-1) ;
                                                       }
             else
             if(!stricmp(entry, "UPDATE")) {                        /* Опция UPDATE */
               if(value!=NULL)  strncpy(to_update, value+1, sizeof(to_update)-1) ;
                                           }
             else                          {
                           error="Unknown control option detected" ;
                                              break ;
                                           } 

             if(next==NULL)  break ;                                /* Если все перебрали */
                                                    }               /* ENDLOOP - Перебираем опции */

                    if(error!=NULL)  break ;                        /* Если при разборке была ошибка... */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Вид транспорта */
     if(stricmp(to_kind, "SOAP")) {
               error="Illegal transport specified (legal: SOAP)" ;
                                      break ;
                                  }
/*- - - - - - - - - - - - - - - - - - - - - - - -  Режимы для UPDATE */
     if(!stricmp(action, "UPDATE")) {

           if(        to_alt[0]==0     &&
              stricmp(to_update, "ALL")  ) {
               error="For UPDATE operation either ALT-option or UPDATE=ALL must be specified" ;
                                      break ;
                                           }

//Как для UPDATE=ALL проверить, что действительно заданы все поля?
                                    }
/*- - - - - - - - - - - - - - - - - - - - - - - - - Режимы для WRITE */
     if(!stricmp(action, "WRITE" )) {

           if(to_alt[0]!=0) {
               error="Using alternative message (ALT-option) for WRITE operation is prohibited" ;
                                      break ;
                            }
                                    }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
       } while(0) ;                                                 /* BLOCK */
/*- - - - - - - - - - - - - - - - - - - - - - - - - Обработка ошибок */
     if(error!=NULL) {

                      strcpy(error_type, "DRIVER_ERROR") ;
                     strncpy(error_text, error, sizeof(error_text)-1) ;
                             error_data[0].size=strlen(error_data[0].prototype) ;
                             error_data[1].size=strlen(error_data[1].prototype) ;
                iXobject_add(source, &error_template) ;
                                 return(-1) ;
                     }
/*---------------------------------------------- Ищем REQUEST-запись */

     for(record=(Dcl_complex_record *)source->addr,                 /* LOOP - Перебираем записи -              */
                        i=0 ; i<source->buff ; i++,                 /*         ищем запись со значеним REQUEST */
         record=(Dcl_complex_record *)record->next_record) {        /*          в поле API_object              */

       for(k=0 ; k<record->elems_cnt ; k++)                         /* Ищем поле API_object */
         if(!stricmp(ELEM[k].name, "API_object"))  break ;

         if(k>=record->elems_cnt)  continue ;                       /* Если такое не найдено... */
         
         if(        ELEM[k].size==strlen("REQUEST") &&              /* Проверяем значение */
            !memcmp(ELEM[k].addr, "REQUEST", 
                                 strlen("REQUEST"))   )  break ;
  
                                                           }        /* ENDLOOP - Перебираем записи */

         if(i>=source->buff) {                                      /* Если REQUEST-запись не найдена */
                      strcpy(error_type, "DRIVER_ERROR") ;
                     sprintf(error_text, "REQUEST record is missed") ;
                             error_data[0].size=strlen(error_data[0].prototype) ;
                             error_data[1].size=strlen(error_data[1].prototype) ;
                iXobject_add(source, &error_template) ;
                                 return(-1) ;
                             }
/*-------------------------------------- Подготовка рабочих массивов */

        buff=(char *)calloc(1, _SOAP_SIZE) ;
       error=(char *)calloc(1,      10000) ;
      fields=(SQL_parameter *)calloc(_FIELDS_MAX, sizeof(*fields)) ;

/*---------------------------------- Подготовка данных для SOAP-call */

    for(fields_cnt=0, k=0 ; k<record->elems_cnt ; k++) {            /* LOOP - Перебираем поля сообщения */

         if(!stricmp(ELEM[k].name, "API_object"))  continue ;       /* Поле API_object игнорируем */

         if(!stricmp(ELEM[k].name, "API_message")) {                /* Из поля API_message извлекаем код сообщения */
                                     size=ELEM[k].size ;
         if(size>=sizeof(msg_code))  size=sizeof(msg_code)-1 ;
                  memcpy(msg_code, ELEM[k].addr, size) ;
                         msg_code[size]=0 ;
                              continue ;
                                                   }

         if(ELEM[k].size<0)  continue ;                             /* Поля со значением NULL не включаем в сообщение */

               ((char *)ELEM[k].addr)[ELEM[k].size]=0 ;             /* Добавляем к значению поля 0-терминатор */

                  fields[fields_cnt].name=        ELEM[k].name ;    /* Заносим поле в структуру сообщения */
                  fields[fields_cnt].buff=(char *)ELEM[k].addr ;
                         fields_cnt++ ;
                                                       }            /* ENDLOOP - Перебираем поля сообщения */

                  fields[fields_cnt].name=NULL ;                    /* Терминируем список полей */

/*---------------------------------- Обработка ИМИТАЦИОННОЙ отправки */

   if(!stricmp(url, "NULL")) {
                                       free(buff) ;
                                       free(error) ;
                                       free(fields) ;

                                         return(0) ;
                             }
/*---------------------------------------------- Выполнеие SOAP-call */

  do {
          MidasAPI_SOAPHeader(buff, action, msg_code ) ;            /* Формируем сообщение */
          MidasAPI_SOAPBody  (buff, action, msg_code, 
                                    to_alt, login, fields) ;
          MidasAPI_SOAPFooter(buff) ;

       status=Http_SOAP_send (url, login, buff, _SOAP_SIZE,         /* Отправляем сообщение */                                    
                                                log_folder, error) ;
    if(status) {                                                    /* Если ошибка отправки... */
                       strcpy(error_type, "DRIVER_ERROR") ;
                      strncpy(error_text, error, sizeof(error_text)-1) ;
                              error_data[0].size=strlen(error_data[0].prototype) ;
                              error_data[1].size=strlen(error_data[1].prototype) ;
                 iXobject_add(source, &error_template) ;
                                return_code=-1 ;
                                     break ;
               }
 
          for(i=0 ; i<_FIELDS_MAX ; i++)  fields[i].name=NULL ;     /* Сброс списка полей */

       status=MidasAPI_SOAPReply(buff, msg_code,                    /* Разбираем ответ */
                                  fields, _FIELDS_MAX, error) ;
    if(status) {                                                    /* Если ошибка разборки... */
                       strcpy(error_type, "DRIVER_ERROR") ;
                      strncpy(error_text, error, sizeof(error_text)-1) ;
                              error_data[0].size=strlen(error_data[0].prototype) ;
                              error_data[1].size=strlen(error_data[1].prototype) ;
                 iXobject_add(source, &error_template) ;
                                return_code=-1 ;
                                     break ;
               }

     } while(0) ;

/*---------------------------------- Формирование "ответных" записей */

   if(status==0) {
/*- - - - - - - - -  Формирование шаблона записи с ответными данными */
       for(i=0 ; i<_FIELDS_MAX && fields[i].name!=NULL ; i++) ;     /* Подсчет полей данных (с запасом) */

          rec_data=(Dcl_decl *)calloc(i+2, sizeof(*rec_data)) ;     /* Выделяем память под шаблон описания полей */

                    rec_data[0].type     =_CHR_AREA ;               /* Регистрируем поле API_object */
             strcpy(rec_data[0].name,     "API_object") ;
                    rec_data[0].prototype=       "REPLY" ;
                    rec_data[0].buff     =strlen("REPLY")+1 ;
                    rec_data[0].size     =strlen("REPLY") ;

                    rec_data[1].type     =_CHR_AREA ;               /* Регистрируем поле API_message */
             strcpy(rec_data[1].name,     "API_message") ;
                    rec_data[1].prototype=       msg_code ;
                    rec_data[1].buff     =strlen(msg_code)+1 ;
                    rec_data[1].size     =strlen(msg_code) ;

                       rec_cnt=2 ;
/*- - - - - - - - - - - - - - - - - Добавление в шаблон полей данных */
       for(i=0 ; i<_FIELDS_MAX && fields[i].name!=NULL ; i++) {     /* LOOP - Перебираем поля данных */
 
         if(!stricmp(fields[i].name, "SYSTEMERROR") ||              /* Игнорируем диагностические сообщения */ 
            !stricmp(fields[i].name, "ERROR"      ) ||
            !stricmp(fields[i].name, "WARNING"    )   )  continue ;

        for(fields_cnt=0, k=0 ; k<record->elems_cnt ; k++)          /* Ищем поле в REQUEST-записи */
          if(!stricmp(ELEM[k].name, fields[i].name))  break ;

          if(k<record->elems_cnt)  size=         ELEM[k].buff ;     /* Если такое найдено - берем размер буфера по нему */
          else                     size=strlen(fields[i].buff)+1 ;  /*  если нет - по размеру данных                    */

                        rec_data[rec_cnt].type     =_CHR_AREA ;     /* Регистрируем поле */
                strncpy(rec_data[rec_cnt].name,      fields[i].name, sizeof(rec_data->name)-1) ;
                        rec_data[rec_cnt].prototype=       fields[i].buff ;
                        rec_data[rec_cnt].buff     =        size ;
                        rec_data[rec_cnt].size     =strlen(fields[i].buff) ;
                                 rec_cnt++ ;              
                                                              }     /* ENDLOOP - Перебираем поля данных */
/*- - - - - - - - - - - - -  Формирование записи с ответными данными */
      for(size=0, j=0 ; j<rec_cnt ; j++) {                          /* Привязываем данные полей */
                           rec_data[j].addr =(void *)(size+1) ;     /*   К каждому полю добавляем 1 скрытый символ */
                                       size+=rec_data[j].buff+1 ;   /*    для последующей вставки 0-терминатора    */
                                         }

                    rec_template.list    =rec_data ;                /* Готовим шаблон записи */
                    rec_template.list_cnt=rec_cnt ;
                    rec_template.size    =  size ;

             status=iXobject_add(source, &rec_template) ;           /* Добавляем запись */
                            free(rec_data) ;
          if(status) {
                      strcpy(error_type, "DRIVER_ERROR") ;
                     sprintf(error_text, "REPLY message creation fail") ;
                             error_data[0].size=strlen(error_data[0].prototype) ;
                             error_data[1].size=strlen(error_data[1].prototype) ;
                iXobject_add(source, &error_template) ;
                                return(-1) ;
                     }
/*- - - - - - - - -  Формирование записей об ошибках/предупреждениях */
       for(i=0 ; i<_FIELDS_MAX && fields[i].name!=NULL ; i++)       /* LOOP - Перебираем поля данных, выбирая   */
         if(!stricmp(fields[i].name, "SYSTEMERROR") ||              /*         только диагностические сообщения */ 
            !stricmp(fields[i].name, "ERROR"      ) ||
            !stricmp(fields[i].name, "WARNING"    )   ) {

            if(!stricmp(fields[i].name, "SYSTEMERROR")   )  return_code=3 ;
            if(!stricmp(fields[i].name, "ERROR"      ) && 
                               return_code<2             )  return_code=2 ;
            if(!stricmp(fields[i].name, "WARNING"    ) && 
                               return_code<1             )  return_code=1 ;

                     strncpy(error_type, fields[i].name, sizeof(error_type)) ;
                     strncpy(error_text, fields[i].buff, sizeof(error_text)) ;
                             error_data[0].size=strlen(error_data[0].prototype) ;
                             error_data[1].size=strlen(error_data[1].prototype) ;
                iXobject_add(source, &error_template) ;
                                                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                 }
/*-------------------------------------------- Освобождение ресурсов */

   if(status==0) {                                                  /* Если был ответ... */ 
         for(i=0 ; i<_FIELDS_MAX && fields[i].name!=NULL ; i++)     /* Освобождаем динамику разборки ответа */
                    free(fields[i].name) ;     
                 }

                   free(buff) ;
                   free(error) ;
                   free(fields) ;

/*-------------------------------------------------------------------*/

#undef    _SOAP_SIZE
#undef  _FIELDS_MAX

#undef     ELEM

  return(return_code) ;
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
  Dcl_complex_record *record ;
                char  rec_type[128] ;   /* Имя API-сообщения */
                char  rec_mask[128] ;   /* Искомое имя API-сообщения */
                 int  size ;
                 int  i ;
                 int  k ;

#define  ELEM   record->elems
                              
/*------------------------------------------------------- Подготовка */

                memset(buff, 0, sizeof(buff_size)) ;

/*-------------------------------------------- Извлечение параметров */

          memset(rec_mask,  0, sizeof(rec_mask)) ;

    if(pars[0]->addr!=NULL) {
                                    size=       pars[0]->size ;     /* Извлекаем имя сообщения */
        if(size>=sizeof(rec_mask))  size=sizeof(rec_mask)-1 ;
                 memcpy(rec_mask, pars[0]->addr, size) ;
                            }

         strupr(rec_mask) ;

    if(!stricmp(rec_mask, "ALL") ||
       !stricmp(rec_mask, "ANY")   )  rec_mask[0]=0 ;

/*------------------------------------------------- Выдача сообщений */

     for(record=(Dcl_complex_record *)source->addr,                 /* LOOP - Перебираем записи */
                        i=0 ; i<source->buff ; i++, 
         record=(Dcl_complex_record *)record->next_record) {

       for(k=0 ; k<record->elems_cnt ; k++)                         /* Ищем поле API_object */
         if(!stricmp(ELEM[k].name, "API_object"))  break ;

         if(k>=record->elems_cnt)  continue ;                       /* Если такое не найдено... */
           
         if(ELEM[k].size==0)  continue ;                            /* Если поле пустое */

                                     size=ELEM[k].size ;            /* Извлекаем значение поля API_object */
         if(size>=sizeof(rec_type))  size=sizeof(rec_type)-1 ;
                  memcpy(rec_type, ELEM[k].addr, size) ;
                         rec_type[size]=0 ;

                  strupr(rec_type) ;

         if(rec_mask[0]==0) {                                       /* Фильтруем ненужные записи */ 
             if( strstr(rec_type, "ERROR")==NULL)  continue ;
                            }
         else               {
             if(stricmp(rec_type, rec_mask)     )  continue ;
                            }

       for(k=0 ; k<record->elems_cnt ; k++)                         /* Ищем поле text */
         if(!stricmp(ELEM[k].name, "text"))  break ;

         if(k>=record->elems_cnt)  continue ;                       /* Если такое не найдено... */

         if(ELEM[k].size==0)  continue ;                            /* Если поле пустое */
               
            size=buff_size-strlen(buff)-32 ;                        /* Опр.оставшийся размер выходного буфера */ 

         if(size<ELEM[k].size) {                                    /* Если места в буфере недостаточно -        */
               strcat(buff, " ...(too many errors)") ;              /*  - ставим метку переполнения и прекращаем */
                                       break ;   
                               }

         if(buff[0]!=0)  strcat(buff, " ; ") ;
                         memcpy(buff+strlen(buff), ELEM[k].addr, ELEM[k].size) ;

                                                           }        /* ENDLOOP - Перебираем записи */
/*-------------------------------------------------------------------*/

#undef  ELEM

  return(0) ;
}


/*********************************************************************/
/*********************************************************************/
/**                                                                 **/
/**           ФУНКЦИИ ДЛЯ РАБОТЫ С MIDAS API И SOAP                 **/
/**                                                                 **/
/*********************************************************************/
/*********************************************************************/



/*********************************************************************/
/*                                                                   */
/*            Формирование заголовка SOAP-сообщения для MIDAS API    */
/*                                                                   */
/*    Параметры:                                                     */
/*                action  -  Выполняемое действие                    */
/*               message  -  Название сообщенния                     */
/*                  buff  -  Выходной буфер                          */

  void  MidasAPI_SOAPHeader(char *buff,
                            char *action,
                            char *message )
{
#define   L  strlen(buff)

        strcpy(buff,   "<?xml version=\"1.0\" encoding=\"utf-8\"?>") ;
        strcat(buff,   "<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\"") ;
        strcat(buff,                 " xmlns:soapenc=\"http://schemas.xmlsoap.org/soap/encoding/\"") ;
        strcat(buff,                 " xmlns:tns=\"http://www.midasactionapisoaphandler.com/definitions/MidasActionAPISoapHandlerRemoteInterface\"") ;
        strcat(buff,                 " xmlns:types=\"http://www.midasactionapisoaphandler.com/definitions/MidasActionAPISoapHandlerRemoteInterface/encodedTypes\"") ;
        strcat(buff,                 " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"") ;
        strcat(buff,                 " xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">") ;
        strcat(buff,   "<soap:Body soap:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">") ;
        strcat(buff,   "<q1:midasActionAPI xmlns:q1=\"http://tempuri.org/com.misys.midas.soapapi.MidasActionAPISoapHandler\">") ;

       sprintf(buff+L, "<apiNames xsi:type=\"xsd:string\">%s!MIDAS.%s</apiNames>", action, message) ;
        strcat(buff,   "<zone xsi:type=\"xsd:string\">MOSCOW</zone>") ;
        strcat(buff,   "<xml xsi:type=\"xsd:string\">") ;

#undef   L
}


/*********************************************************************/
/*                                                                   */
/*            Формирование окончания SOAP-сообщения для MIDAS API    */
/*                                                                   */
/*    Параметры:                                                     */
/*                  buff  -  Выходной буфер                          */

  void  MidasAPI_SOAPFooter(char *buff)

{
        strcat(buff, "</xml>") ;
        strcat(buff, "<returnFormat xsi:type=\"xsd:int\">1</returnFormat>") ;
        strcat(buff, "</q1:midasActionAPI>") ;
        strcat(buff, "</soap:Body>") ;
        strcat(buff, "</soap:Envelope>") ;
}


/*********************************************************************/
/*                                                                   */
/*            Формирование тела SOAP-сообщения для MIDAS API         */
/*                                                                   */
/*    Параметры:                                                     */
/*                  buff      -  Выходной буфер                      */
/*                action      -  Выполняемое действие                */
/*               message      -  Название сообщенния                 */
/*               message_alt  -  Код альтернативного сообщения       */
/*                                (при !=NULL включает режим         */
/*                                   использования спец-полей)       */
/*                  auth      -  Строка авторизации - user/password  */
/*                fields      -  Поля сообщенния (до .name=NULL)     */
/*                                                                   */
/*  Поле message_alt имеет следующуе структуру для задания значений  */
/*   полей альтернативного сообщения:                                */
/*     <APTGTTYPE>[,<APRPRLOCN>[,<APFOTRANID>]]                      */
/*                                                                   */
/*  Если <APFOTRANID>=EMPTY - использовать пустое значение поля      */
/*  Если <APFOTRANID>=AUTO  - значение поля генерируется             */

  void  MidasAPI_SOAPBody(char *buff, char *action,
                                      char *message,
                                      char *message_alt,
                                      char *auth,
                             SQL_parameter *fields )

{
     time_t  time_abs ;
  struct tm *hhmmss ;
       char *alt_words[3] ;
       char  tran_id[64] ;
       char  user[64] ;
       char  pass[64] ; 
       char *end ;
        int  i ;

#define   L  strlen(buff)

/*------------------------------------------------------- Подготовка */

             memset(user, 0, sizeof(user)) ;
             memset(pass, 0, sizeof(pass)) ;

            strncpy(user, auth, sizeof(user)-1) ;
         end=strchr(user, '/') ;
      if(end!=NULL) {
                                    *end=0 ;
                        strcpy(pass, end+1) ;
                    }

               time_abs=     time( NULL) ;
                 hhmmss=localtime(&time_abs) ;

          sprintf(tran_id, "RNAG%06x%02d%02d%02d%02d%02d",
                           GetCurrentProcessId(),
                           hhmmss->tm_mon+1, hhmmss->tm_mday,
                           hhmmss->tm_hour,      hhmmss->tm_min,   hhmmss->tm_sec  ) ;

/*-------------------------------------------------------- Заголовок */

            sprintf(buff+L, "&lt;midasActionAPI apiNames=\"%s!MIDAS.%s\"&gt;", action, message) ;
            sprintf(buff+L, "&lt;%s&gt;", message) ;

/*--------------------------------------------- Спец-поля управления */

    if( message_alt!=NULL &&                                        /* Если задано использование режима спец-полей */
       *message_alt!=  0    ) {
/*- - - - - - - - - - - - - - - - - - Разбор спецификации спец-полей */
                        memset(alt_words, 0, sizeof(alt_words)) ;

                               alt_words[0]=strtok(message_alt, ",") ;
                               alt_words[1]=strtok(       NULL, ",") ;
       if(alt_words[1]!=NULL)  alt_words[2]=strtok(       NULL, ",") ;

       if(alt_words[1]==NULL)  alt_words[1]="F" ;
       if(alt_words[2]==NULL)  alt_words[2]="EMPTY" ;

       if(!stricmp(alt_words[2], "EMPTY"))  alt_words[2]= "" ;
       if(!stricmp(alt_words[2], "AUTO" ))  alt_words[2]=tran_id ;
/*- - - - - - - - - - - - - - - - - - Формирование секции спец-полей */
            sprintf(buff+L, "&lt;APTGTTYPE&gt;%s&lt;/APTGTTYPE&gt;",   alt_words[0]) ;
            sprintf(buff+L, "&lt;APTGTSYS&gt;%s&lt;/APTGTSYS&gt;",    "MIDAS"      ) ;
            sprintf(buff+L, "&lt;APUSERID&gt;%s&lt;/APUSERID&gt;",     user        ) ;
            sprintf(buff+L, "&lt;APPASSWORD&gt;%s&lt;/APPASSWORD&gt;", pass        ) ;
            sprintf(buff+L, "&lt;APRPRLOCN&gt;%s&lt;/APRPRLOCN&gt;",   alt_words[1]) ;
            sprintf(buff+L, "&lt;APFOTRANID&gt;%s&lt;/APFOTRANID&gt;", alt_words[2]) ;/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                              }
/*------------------------------------------------------ Поля данных */

   for(i=0 ; fields[i].name!=NULL ; i++) 
     if(fields[i].buff[0]==0) {
            sprintf(buff+L, "&lt;%s /&gt;", fields[i].name) ;
                              }
     else                     {
            sprintf(buff+L, "&lt;%s&gt;",  fields[i].name) ;
             strcat(buff  ,                fields[i].buff) ;
            sprintf(buff+L, "&lt;/%s&gt;", fields[i].name) ;
                              }
/*--------------------------------------------------------- Концовка */

            sprintf(buff+L, "&lt;/%s&gt;", message) ;
             strcat(buff, "&lt;/midasActionAPI&gt;") ;

/*-------------------------------------------------------------------*/

#undef   L
}


/*********************************************************************/
/*                                                                   */
/*            Разбор тела SOAP-ответа от MIDAS API                   */
/*                                                                   */
/*    Параметры:                                                     */
/*                  buff      -  Входной буфер                       */
/*               message      -  Название сообщенния                 */
/*                fields      -  Поля сообщения                      */
/*                fields_max  -  Максимальное число полей сообщения  */
/*                 error      -  Текст ошибки                        */
/*                                                                   */
/*    Список fields терминируется записью с name=NULL                */

   int  MidasAPI_SOAPReply(          char *buff, 
                                     char *message, 
                            SQL_parameter *fields, 
                                      int  fields_max,
                                     char *error      )

{
  char *s_entry ;
  char *s_close ;
  char *entry ;
   int  fields_cnt ;
   int  status ;

/*---------------------------------------------------- Инициализация */

                    fields_cnt=0 ;

/*------------------------------------------------- Выделение ответа */

#define  S_HEADER  "<return xsi:type=\"xsd:string\">"
#define  S_FOOTER  "</return>"

      s_entry=strstr(buff, S_HEADER) ;
   if(s_entry==NULL) {
          if(error!=NULL)  sprintf(error, "Section <return> is missed") ;
                                  return(-1) ;
                     }

      s_close=strstr(s_entry, S_FOOTER) ;
   if(s_close==NULL) {
          if(error!=NULL)  sprintf(error, "Section <return> is't terminated") ;
                                  return(-1) ;
                     }

                  *s_close=0 ;

      strcpy(buff, s_entry+strlen(S_HEADER)) ;

#undef   S_HEADER
#undef   S_FOOTER

/*--------------------------------- Перевод в каноническую XML-форму */

                   entry=buff ;
    while(1) {
                   entry=strstr(entry, "&lt;") ;
                if(entry==NULL)  break ;

                         strcpy(entry+1, entry+4) ;
                               *entry='<' ;
             }

                   entry=buff ;
    while(1) {
                   entry=strstr(entry, "&gt;") ;
                if(entry==NULL)  break ;

                         strcpy(entry+1, entry+4) ;
                               *entry='>' ;
             }

                   entry=buff ;
    while(1) {
                   entry=strstr(entry, "&apos;") ;
                if(entry==NULL)  break ;

                         strcpy(entry+1, entry+6) ;
                               *entry='\'' ;
             }
/*---------------------------------------- Обработка ответных данных */

      status=MidasAPI_SOAPGetData(buff, message, 
                                   fields+fields_cnt, fields_max, error) ;
   if(status==-1)  return(-1) ;

                   fields_cnt+=status ;

/*----------------------------------------- Обработка предупреждений */

      status=MidasAPI_SOAPGetErrors(buff, "warning", 
                                     fields+fields_cnt, fields_max, error) ;
   if(status==-1)  return(-1) ;

                   fields_cnt+=status ;

/*------------------------------------------------- Обработка ошибок */

      status=MidasAPI_SOAPGetErrors(buff, "error", 
                                     fields+fields_cnt, fields_max, error) ;
   if(status==-1)  return(-1) ;

                   fields_cnt+=status ;

/*--------------------------------------- Обработка системных ошибок */

      status=MidasAPI_SOAPGetErrors(buff, "systemError", 
                                     fields+fields_cnt, fields_max, error) ;
   if(status==-1)  return(-1) ;

                   fields_cnt+=status ;

/*--------------------------------------------- Завершение обработки */

             fields[fields_cnt].name=NULL ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*            Разбор секции данных от MIDAS API                      */
/*                                                                   */
/*    Параметры:                                                     */
/*                  buff      -  Входной буфер                       */
/*               section      -  Код секции                          */
/*                fields      -  Поля сообщения                      */
/*                fields_max  -  Максимальное число полей сообщения  */
/*                 error      -  Текст ошибки                        */
/*                                                                   */
/*  Возвращает: число вставленных в fields записей или -1 при ошибке */

   int  MidasAPI_SOAPGetData(char *buff, char *section, 
                                SQL_parameter *fields, 
                                          int  fields_max, char *error)

{
  char  s_header[100] ;
  char  s_footer[100] ;
  char *header ;
  char  footer[100] ;
  char *s_entry ;
  char *s_close ;
  char *entry ;
  char *close ;
   int  cnt ;

/*------------------------------------------------------- Подготовка */

       sprintf(s_header, "<%s>",  section) ;
       sprintf(s_footer, "</%s>", section) ;

                    cnt=0 ;

/*------------------------------------------------- Выделение секции */

       s_entry=strstr(buff, s_header) ;
    if(s_entry==NULL) {
          if(error!=NULL) 
               sprintf(error, "Section %s is missed", s_header) ;
                                              return(-1) ;
                      }

       s_entry+=strlen(s_header) ;

       s_close=strstr(s_entry, s_footer) ;
    if(s_close==NULL) {
          if(error!=NULL) 
               sprintf(error, "Section %s is't terminated", s_header) ;
                                              return(-1) ;
                      }

                       *s_close=0 ;

/*------------------------------------------------- Разбор сообщений */

   for(entry=s_entry ; *entry ; entry++) {
        
         if(*entry!='<')  continue ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - Ищем входной тэг */
                 header=entry ; 
        
      for( ; *entry!=0 && *entry!='>' ; entry++) ;                  /* Ищем конец заголовка */
 
        if(*entry==0) {
          if(error!=NULL) 
               sprintf(error, "Unclosed tag header in section %s", s_header) ;
                                              return(-1) ;
                      }

        if(entry-header>=sizeof(footer)-2) {
          if(error!=NULL) 
               sprintf(error, "Too large tag header in section %s", s_header) ;
                                              return(-1) ;
                                           }
/*- - - - - - - - - - - - - - - - - - - - - - -  Ищем тэг-терминатор */
              memset(footer, 0, sizeof(footer)) ;                   /* Формируем тэг-терминатор */
                     footer[0]='<' ; 
                     footer[1]='/' ; 
              memcpy(footer+2, header+1, entry-header) ;

                        entry++ ;
           close=strstr(entry, footer) ;
        if(close==NULL) {
               if(error!=NULL) 
                    sprintf(error, "Tag footer %s is missed", footer) ;
                                              return(-1) ;
                        }
        
                       *close=0 ;
/*- - - - - - - - - - - - - - - - - - - - - - Выдаем данные на выход */
           if(cnt>=fields_max) {
              if(error!=NULL)  sprintf(error, "Too many fields in SOAP-reply") ;
                                  return(-1) ;
                               } 

                      fields[cnt].name=(char *)
                                         calloc(1, strlen(header)+2) ;
               memcpy(fields[cnt].name, header+1, entry-header-2) ;
                      fields[cnt].buff=fields[cnt].name+strlen(fields[cnt].name)+1 ;
               strcpy(fields[cnt].buff, entry) ;
                             cnt++ ;
/*- - - - - - - - - - - - - - - - Восстанавливаем целостность данных */
                       *close='<' ;
                        entry=close+strlen(footer)-1 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                         }
/*--------------------------------------------- Завершение обработки */

                       *s_close='<' ;

/*-------------------------------------------------------------------*/

  return(cnt) ;
}


/*********************************************************************/
/*                                                                   */
/*          Разбор секции диагностических сообщений от MIDAS API     */
/*                                                                   */
/*    Параметры:                                                     */
/*                  buff      -  Входной буфер                       */
/*               section      -  Код ошибочной секции                */
/*                fields      -  Поля сообщения                      */
/*                fields_max  -  Максимальное число полей сообщения  */
/*                 error      -  Текст ошибки                        */
/*                                                                   */
/*  Возвращает: число вставленных в fields записей или -1 при ошибке */

   int  MidasAPI_SOAPGetErrors(char *buff, char *section, 
                                  SQL_parameter *fields, 
                                            int  fields_max, char *error)

{
  char  s_header[100] ;
  char  s_footer[100] ;
  char  t_header[100] ;
  char  t_footer[100] ;
  char *s_entry ;
  char *s_close ;
  char *t_entry ;
  char *t_close ;
  char *entry ;
  char *close ;
  char  name[100] ;
   int  cnt ;

/*------------------------------------------------------- Подготовка */

       sprintf(s_header, "<%ss>",  section) ;
       sprintf(s_footer, "</%ss>", section) ;
       sprintf(t_header, "<%s>",   section) ;
       sprintf(t_footer, "</%s>",  section) ;

                    cnt=0 ;

/*------------------------------------------------- Выделение секции */

       s_entry=strstr(buff, s_header) ;
    if(s_entry==NULL)  return(0) ;

       s_close=strstr(s_entry, s_footer) ;
    if(s_close==NULL) {
          if(error!=NULL) 
               sprintf(error, "Section %s is't terminated", s_header) ;
                                              return(-1) ;
                      }

                       *s_close=0 ;

/*------------------------------------------------- Разбор сообщений */

   for(t_entry=s_entry ; ; t_entry=t_close+1) {

                    name[0]=0 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - Выделяем сообщение */
            t_entry=strstr(t_entry, t_header) ;                
         if(t_entry==NULL)   break ;

            t_close=strstr(t_entry, t_footer) ;
         if(t_close==NULL) {
               if(error!=NULL) 
                    sprintf(error, "Section %s is't terminated", t_header) ;
                                              return(-1) ;
                           }

                       *t_close=0 ;
/*- - - - - - - - - - - - - - - - -  Выделяем имя связанного объекта */
       do {
            entry=strstr(t_entry, "<name>") ;
         if(entry==NULL)  break ;

            close=strstr(entry, "</name>") ;
         if(close==NULL) {
              if(error!=NULL)  sprintf(error, "Section <name> is't terminated") ;
                                  return(-1) ;
                         }

                         *close =0 ;

               strncpy(name, entry+strlen("<name>"), sizeof(name)) ;

                         *close ='<' ;
          } while(0) ; 
/*- - - - - - - - - - - - - - - - - - - - - Выделяем текст сообщения */
            entry=strstr(t_entry, "<description>") ;
         if(entry==NULL) {
              if(error!=NULL)  sprintf(error, "Section <description> is missed") ;
                                  return(-1) ;
                         }

            close=strstr(entry, "</description>") ;
         if(close==NULL) {
              if(error!=NULL)  sprintf(error, "Section <description> is't terminated") ;
                                  return(-1) ;
                         }

                          entry+=strlen("<description>") ;
                         *close =0 ;
/*- - - - - - - - - - - - - - - - - - - - - - Выдаем данные на выход */
           if(cnt>=fields_max) {
              if(error!=NULL)  sprintf(error, "Too many sections in SOAP-reply") ;
                                  return(-1) ;
                               } 

                      fields[cnt].name=(char *)
                                         calloc(1, strlen(section)+strlen(name)+strlen(entry)+8) ;
               strcpy(fields[cnt].name, section) ;
                      fields[cnt].buff=fields[cnt].name+strlen(section)+1 ;

       if(name[0]==0) 
               strcpy(fields[cnt].buff, entry) ;
       else   sprintf(fields[cnt].buff, "%s - %s", name, entry) ;
                             cnt++ ;
/*- - - - - - - - - - - - - - - - Восстанавливаем целостность данных */
                       *  close='<' ;
                       *t_close='<' ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                              }
/*--------------------------------------------- Завершение обработки */

                       *s_close='<' ;

/*-------------------------------------------------------------------*/

  return(cnt) ;
}


/*********************************************************************/
/*                                                                   */
/*              Отправка SOAP-сообщения по HTTP                      */
/*                                                                   */
/*    Параметры:                                                     */
/*                   url  -  Целевой адрес                           */
/*                  auth  -  Строка авторизации - user/password      */
/*                  buff  -  Буфер входного/выходного сообщения      */
/*                  size  -  Размер буфера                           */
/*            log_folder  -  Папка размещения файлов логов           */
/*                 error  -  Текст ошибки                            */

   int  Http_SOAP_send(char *url, char *auth, char *buff, int  size, 
                                  char *log_folder, char *error)

{
#ifndef UNIX
        WORD  version ;
     WSADATA  winsock_data ;        /* Данные системы WINSOCK */
#endif
    ABTP_tcp  Transport ;
        char  url_[512] ;
        char  auth_64[512] ;        /* Строка авторизации в кодировке Base64 */
        char  http_header[4096] ;
         int  port ;
        char *service_path ;
         int  status ;
         int  cnt ;
        char *end ;
        char  tmp[512] ;

        char  path[FILENAME_MAX] ;
        FILE *file ;

/*---------------------------------------------------- Инициализация */

    if(error!=NULL)  *error=0 ;

/*------------------------------------------------- Подготовка логов */

                                    file=NULL ;

    if( log_folder!=NULL &&
       *log_folder!=  0    ) {
 
                  sprintf(path, "%ssoap.log", log_folder) ;
               file=fopen(path, "wb") ;

                             }
/*------------------------------------------- Разбор целевого адреса */
 
           strncpy(url_, url, sizeof(url_)) ;  

        end=strchr(url_, ':') ;                                     /* Опр.контактный порт */
     if(end==NULL)  port=  80 ;
     else           port=strtoul(end+1, &end, 10) ;

        service_path=strchr(url_, '/') ;                            /* Опр.локальный путь сервиса */
     if(service_path==NULL)  service_path="/" ;

        end=strchr(url_, ':') ;                                     /* Выделяем DNS */
     if(end!=NULL) *end=0 ;
        end=strchr(url_, '/') ;
     if(end!=NULL) end[-1]=0 ;

/*------------------------------------ Подготовка строки авторизации */

                   strncpy(tmp, auth, sizeof(tmp)-1) ;              /* Копируем в рабочий буфер */
                end=strchr(tmp, '/') ;                              /* Заменяем разделитель пользователя/пароля */
             if(end!=NULL)  *end=':' ;

       Http_Base64_incode(tmp, auth_64, strlen(tmp)) ;

/*-------------------------------------- Формирование HTTP-заголовка */

       sprintf(http_header, "POST %s HTTP/1.1\r\n"
                            "Authorization: Basic %s\r\n"
                            "Timeout: 10000\r\n"
                            "Content-Type: text/xml\r\n"
	                    "Host: %s\r\n"
	                    "Content-Length: %d\r\n"
	                    "\r\n",
                               service_path, auth_64, url_, strlen(buff)) ;

/*-------------------------------------------- Соединение с сервером */

#ifndef UNIX
                        version=MAKEWORD(1, 1) ;
      status=WSAStartup(version, &winsock_data) ;                   /* Иниц. Win-Sockets */
   if(status) {
          if(error!=NULL)
                 sprintf(error, "Socket - %d", WSAGetLastError()) ;
                    return(-1) ; 
              }
#endif

              Transport.mServer_name=url_ ;
              Transport.mServer_port=port ;

       status=Transport.LinkToServer() ;
 
/*--------------------------------------------- Отправка SOAP-пакета */

    if(status==0) do {

         cnt=Transport.iSend(Transport.mSocket_cli,                 /* Передаем HTTP-заголовок */ 
                               http_header, strlen(http_header), _WAIT_RECV, NULL) ;
      if(cnt==SOCKET_ERROR) {                                       /* Если ошибка... */
          if(error!=NULL)
                 sprintf(error, "Socket - %d", WSAGetLastError()) ;
                                          status=-1 ;
                                            break ;          
                            }

      if(file!=NULL)  fwrite(http_header, 1, strlen(http_header), file) ;

         cnt=Transport.iSend(Transport.mSocket_cli,                 /* Передаем SOAP-пакет */ 
                                buff, strlen(buff), _WAIT_RECV, NULL) ;
      if(cnt==SOCKET_ERROR) {                                       /* Если ошибка... */
          if(error!=NULL)
                 sprintf(error, "Socket - %d", WSAGetLastError()) ;
                                          status=-1 ;
                                            break ;
                            }

      if(file!=NULL)  fwrite(buff, 1, strlen(buff), file) ;
      if(file!=NULL)  fwrite("\r\n\r\n", 1, strlen("\r\n\r\n"), file) ;

                     } while(0) ;

/*----------------------------------------------------- Прием ответа */

    if(status==0) do {
                         memset(buff, 0, size) ;                    /* Очищаем приемный буфер */

             Transport.mReceiveCallback=Http_Receive_ctrl ;         /* Задаем управляющую функцию приема */
         cnt=Transport.iReceive(Transport.mSocket_cli,              /* Принимаем ответ */ 
                                   buff, size-1, _WAIT_RECV, NULL) ;
             Transport.mReceiveCallback=NULL ;
      if(cnt==SOCKET_ERROR) {                                       /* Если ошибка -                                   */
            status=WSAGetLastError() ;                              /*  - проверяем по коду ошибки                     */
         if(status!=0) {                                            /*     на форсированное закрытие коннекта сервером */
          if(error!=NULL)
                 sprintf(error, "Socket - %d", WSAGetLastError()) ;
                            break ;
                       }
                            }

      if(file!=NULL)  fwrite(buff, 1, strlen(buff), file) ;

                     } while(0) ;

/*---------------------------------------------------- Анализ ответа */

    if(status==0) do {
/*- - - - - - - - - - - - - - - - - - - -  Анализ кода возврата HTTP */
               memset(http_header, 0, sizeof(http_header)) ;        /* Выделяем HTTP-заголовок */
              strncpy(http_header, buff, sizeof(http_header)-1) ;

           end=strchr(http_header, '\r') ;                          /* Выделяем первую строку */
        if(end!=NULL)  *end=0 ;
           end=strchr(http_header, '\n') ;
        if(end!=NULL)  *end=0 ;

           end=strchr(http_header, ' ') ;                           /* Ищем разделитель кода возврата */
        if(end==NULL) {                                             /* Если нештатная структура ответа */
          if(error!=NULL)
                 sprintf(error, "HTTP - Unexpected reply") ;
                          status=-1 ;
                             break ;
                      }

           status=strtoul(end+1, &end, 10) ;                        /* Смотрим код возврата */
        if(*end!=' ') {                                             /* Если нештатная структура ответа */
          if(error!=NULL)
                 sprintf(error, "HTTP - Unexpected reply") ;
                          status=-1 ;
                             break ;
                      }

        if(status!=200) {
                 sprintf(error, "HTTP - %d%s", status, end) ;
                          status=-1 ;
                             break ;
                        }

                          status=0 ;
/*- - - - - - - - - - - - - - - - - - - - - - Выделение SOAP-ответа  */
           end=strstr(buff, "<?xml ") ;
        if(end==NULL) {
                 sprintf(error, "HTTP - XML-content is missed") ;
                          status=-1 ;
                             break ;
                      }

               strcpy(buff, end) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                     } while(0) ;

/*-------------------------------------------- Завершение соединения */

                  Transport.ClientClose() ;

#ifndef UNIX
                             WSACleanup();
#endif

/*---------------------------------------------- Закрытие файла лога */

      if(file!=NULL)  fclose(file) ;                              

/*-------------------------------------------------------------------*/

  return(status) ;
}


/*********************************************************************/
/*								     */
/*                Функция управления приемом для HTTP                */

   int  Http_Receive_ctrl(char *data, int  data_size)

{
   char  work[4096] ;
   char *entry ;
   char *content ;
   char *end ;
    int  size ;

/*------------------------------------------ Выделение первой строки */

               memset(work, 0, sizeof(work)) ;                      /* Выделяем HTTP-заголовок */

        if(data_size<sizeof(work))  size=data_size ;
        else                        size=sizeof(work)-1 ;

              strncpy(work, data, size) ;

        content=strstr(work, "\r\n\r\n") ;                          /* Ищем вход контента */

           end=strchr(work, '\n') ;                                 /* Выделяем первую строку */
        if(end!=NULL)  *end=0 ;

/*---------------------------------------------------------- Если OK */

   if(strstr(work, " 200 OK")!=NULL) {

        if(end!=NULL)  *end='\n' ;                                  /* Восстанавливаем данные */
      
        if(content==NULL)  return(0) ;                              /* Если нет разделителя контента... */
 
          *content = 0 ;                                            /* Отсекаем заголовок */
           content+= 4 ;                                            /* Проходим на контент */

                 strupr(work) ;                                     /* Переводим заголовок в верхний регистр */
           entry=strstr(work, "CONTENT-LENGTH:") ;                  /* Ищем вход аттрибута длины данных */
        if(entry==NULL)  return(0) ;                                /* Если такой не найден... */

                         entry+=strlen("CONTENT-LENGTH:") ;
            size=strtoul(entry, &end, 10) ;                         /* Извлекаем длину данных */

        if( (data_size-(content-work))>=size )  return(1) ;         /* Если все данные получены - выходим */

                                     }
/*------------------------------------------------------ Если ошибка */

   else                              {

        if(content!=NULL)  return(1) ;                              /* Если есть разделитель контента... */

                                     }
/*-------------------------------------------------------------------*/

   return(0) ;

}


/*********************************************************************/
/*								     */
/*                Упаковка бинарных данных по Base64                 */

   int  Http_Base64_incode(char *data, char *pack, int  data_size)

{
            int  size ;
  unsigned char  cvt[3] ;

  static char *Base64_symbols="ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                              "abcdefghijklmnopqrstuvwxyz"
                              "0123456789+/" ;
 
/*---------------------------------------------------- Инициализация */

               size=0 ;

/*-------------------------------------------------- Упаковка данных */

   for( ; data_size>0 ; data+=3, data_size-=3) {

                       cvt[1]=  0 ;
                       cvt[2]=  0 ;
                         
                       cvt[0]=data[0] ;
      if(data_size>1)  cvt[1]=data[1] ;
      if(data_size>2)  cvt[2]=data[2] ;

           pack[0]=Base64_symbols[  (cvt[0] & 0xfc)>>2   ] ;
           pack[1]=Base64_symbols[ ((cvt[0] & 0x03)<<4) |  
                                   ((cvt[1] & 0xf0)>>4)  ] ;  
           pack[2]=Base64_symbols[ ((cvt[1] & 0x0f)<<2) |  
                                   ((cvt[2] & 0xc0)>>6)  ] ;  
           pack[3]=Base64_symbols[  (cvt[2] & 0x3f)      ] ;

           pack+=4 ;
           size+=4 ;       
                                               } 
/*------------------------------------------------------- Завершение */

                        *pack=0 ;

       if(data_size< 0)  pack[-1]='=' ;
       if(data_size<-1)  pack[-2]='=' ;


/*-------------------------------------------------------------------*/

  return(size) ;
}


/*********************************************************************/
/*								     */
/*              Распаковка бинарных данных по Base64                 */

   int  Http_Base64_decode(char *pack, char *data, int  pack_size)

{
            int  size ;

  static int Base64_symbols[128]={
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   //   0 -  15
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   //  16 -  31
                  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 62,  0,  0,  0, 63,   //  32 -  47
                 52, 53, 54, 55, 56, 57, 58, 59, 60, 61,  0,  0,  0,  0,  0,  0,   //  48 -  63
                  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,   //  64 -  79
                 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,  0,  0,  0,  0,  0,   //  80 -  95   
                  0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,   //  96 - 111 
                 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,  0,  0,  0,  0,  0    // 112 - 127
                                 }  ;
/*---------------------------------------------------- Инициализация */

               size=0 ;

/*-------------------------------------------------- Упаковка данных */

   for( ; pack_size>0 ; pack+=4, pack_size-=4) {

           data[0]=(Base64_symbols[pack[0]]<<2) | (Base64_symbols[pack[1]]>>4) ;
           data[1]=(Base64_symbols[pack[1]]<<4) | (Base64_symbols[pack[2]]>>2) ;
           data[2]=(Base64_symbols[pack[2]]<<6) | (Base64_symbols[pack[3]]   ) ;

           data+=3 ;
           size+=3 ;       
                                               } 
/*------------------------------------------------------- Завершение */

         if(pack[-1]=='=') size-- ;
         if(pack[-2]=='=') size-- ;

                          *data=0 ;

/*-------------------------------------------------------------------*/

  return(size) ;
}

/*********************************************************************/
/*                                                                   */
/*              Библиотека обмена сообщениями по TCP                 */
/*                                                                   */
/*********************************************************************/

#include <stddef.h>
#include <stdio.h>
#include <errno.h>

#ifdef UNIX

#ifdef  AIX
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#endif

#define  SOCKET_ERROR       -1 
#define  WSAGetLastError()  errno
#define  stricmp            strcasecmp
#define  strupr(t)          for(char *b=t ; *b ; b++)  *b=toupper(*b)

#else
#include <winsock2.h>
#endif

#pragma warning(disable : 4267)
#pragma warning(disable : 4996)

#include "exh.h"
#include "tcp.h"
#include "http_tcp.h"


/*********************************************************************/
/*								     */
/*		       Конструктор класса			     */

     HTTP_tcp::HTTP_tcp(void)

{
}


/*********************************************************************/
/*								     */
/*		        Деструктор класса			     */

    HTTP_tcp::~HTTP_tcp(void)

{
        Free() ;
}



/*********************************************************************/
/*								     */
/*		       Освобождение ресурсов                         */

  void  HTTP_tcp::Free(void)

{
}


/*********************************************************************/
/*                                                                   */
/*                Упаковка бинарных данных по Base64                 */
/*                                                                   */
/*  Возвращает размер упакованных данных.                            */
/*  Если data=NULL или pack=NULL реальная паковка не производится    */

   int  HTTP_tcp::Base64_incode(char *data, char *pack, int  data_size)

{
            int  size ;
  unsigned char  cvt[3] ;

  static char *Base64_symbols="ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                              "abcdefghijklmnopqrstuvwxyz"
                              "0123456789+/" ;
 
/*---------------------------------------------------- Инициализация */

               size=0 ;

/*----------------------------- Определение размера выходного буфера */

      if(data==NULL || pack==NULL) {
                                       return( (data_size*4)/3 ) ;
                                   } 
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
/*                                                                   */
/*              Распаковка бинарных данных по Base64                 */
/*                                                                   */
/*  Возвращает размер распакованных данных.                          */
/*  Если data=NULL или pack=NULL реальная распаковка не производится */
/*   и выдается максимальный возможный размер распакованных данных   */

   int  HTTP_tcp::Base64_decode(char *pack, char *data)

{
            int  pack_size ;
            int  size ;
            int  i ;

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

          pack_size=strlen(pack) ;
               size=0 ;

/*----------------------------- Определение размера выходного буфера */

      if(data==NULL || pack==NULL) {
                                       return( (pack_size*3)/4 ) ;
                                   } 
/*-------------------------------------------------- Проверка данных */

      if(pack_size%4)  return(-1) ;

   for(i=0 ; i<pack_size ; i++)
     if(pack[i]<  0 ||
        pack[i]>127   )  return(-2) ;

/*------------------------------------------------ Распаковка данных */

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


/*********************************************************************/
/*                                                                   */
/*              Отправка SOAP-сообщения по HTTP                      */
/*                                                                   */
/*    Параметры:                                                     */
/*                   url  -  Целевой адрес                           */
/*                  auth  -  Строка авторизации - user/password      */
/*                  buff  -  Буфер исходящего/входящего сообщения    */
/*                  size  -  Размер буфера входящего сообщения       */
/*                 error  -  Текст ошибки                            */

   int  HTTP_tcp::SOAP_exchange(char *url, char *auth, char *buff, int  size, char *error)

{
   char *buffs_out[2] ;
    int  status ;


         buffs_out[0]=buff ;
         buffs_out[1]=NULL ;

    status=SOAP_exchange(url, auth, buffs_out, buff, size, error) ;

  return(status) ;
}


/*********************************************************************/
/*                                                                   */
/*              Отправка SOAP-сообщения по HTTP                      */
/*                                                                   */
/*    Параметры:                                                     */
/*                   url  -  Целевой адрес                           */
/*                  auth  -  Строка авторизации - user/password      */
/*             buffs_out  -  Список фрагментов исходящего сообщения  */
/*                             (терминируется NULL-указателем)       */
/*               buff_in  -  Буфер входящего сообщения               */
/*               size_in  -  Размер буфера входящего сообщения       */
/*                 error  -  Текст ошибки                            */

   int  HTTP_tcp::SOAP_exchange(char *url, char *auth, char **buffs_out, char *buff_in, int  size_in, char *error)

{
#ifndef UNIX
        WORD  version ;
     WSADATA  winsock_data ;        /* Данные системы WINSOCK */
#endif
         Tcp  Transport ;
        char  url_[512] ;
        char  auth_64[512] ;        /* Строка авторизации в кодировке Base64 */
        char  http_header[4096] ;
         int  port ;
        char *service_path ;
         int  status ;
         int  cnt ;
        char *end ;
        char  tmp[512] ;
         int  i ;

/*---------------------------------------------------- Инициализация */

    if(error!=NULL)  *error=0 ;

/*------------------------------------------- Разбор целевого адреса */
 
           strncpy(url_, url, sizeof(url_)) ;

        end=strchr(url_, ':') ;                                     /* Опр.контактный порт */
     if(end==NULL)  port=  80 ;
     else           port=strtoul(end+1, &end, 10) ;

        service_path=strchr(url, '/') ;                             /* Опр.локальный путь сервиса */
     if(service_path==NULL)  service_path="/" ;

        end=strchr(url_, ':') ;                                     /* Выделяем DNS */
     if(end!=NULL) *end=0 ;
        end=strchr(url_, '/') ;
//   if(end!=NULL) end[-1]=0 ;
     if(end!=NULL) *end=0 ;

/*------------------------------------ Подготовка строки авторизации */

                   strncpy(tmp, auth, sizeof(tmp)-1) ;              /* Копируем в рабочий буфер */
                end=strchr(tmp, '/') ;                              /* Заменяем разделитель пользователя/пароля */
             if(end!=NULL)  *end=':' ;

            Base64_incode(tmp, auth_64, strlen(tmp)) ;

/*-------------------------------------- Формирование HTTP-заголовка */

          for(cnt=0, i=0 ; buffs_out[i]!=NULL ; i++)                /* Определение общего размера данных */
                                 cnt+=strlen(buffs_out[i]) ;

       sprintf(http_header, "POST %s HTTP/1.1\r\n"
//     sprintf(http_header, "GET %s HTTP/1.1\r\n"
                            "Authorization: Basic %s\r\n"
                            "Timeout: 10000\r\n"
                            "Content-Type: text/xml\r\n"
	                    "Host: %s\r\n"
	                    "Content-Length: %d\r\n"
	                    "\r\n",
                               service_path, auth_64, url_, cnt) ;

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
                               http_header, strlen(http_header), _WAIT_RECV) ;
      if(cnt==SOCKET_ERROR) {
          if(error!=NULL)  sprintf(error, "Socket - %d", WSAGetLastError()) ;
                                          status=-1 ;
                                            break ;          
                            }

    for(i=0 ; buffs_out[i]!=NULL ; i++) {                           /* Пофрагментно передаем SOAP-пакет */
 
         cnt=Transport.iSend(Transport.mSocket_cli,
                                buffs_out[i], strlen(buffs_out[i]), _WAIT_RECV) ;
      if(cnt==SOCKET_ERROR) {
          if(error!=NULL)  sprintf(error, "Socket - %d", WSAGetLastError()) ;
                                          status=-1 ;
                                            break ;
                            }
                                        }

                   } while(0) ;

/*----------------------------------------------------- Прием ответа */

  if(status==0) do {
                         memset(buff_in, 0, size_in) ;              /* Очищаем приемный буфер */

             Transport.mReceiveCallback=HTTP_tcp::iReceive_ctrl ;   /* Задаем управляющую функцию приема */
         cnt=Transport.iReceive(Transport.mSocket_cli,              /* Принимаем ответ */ 
                                   buff_in, size_in-1, _WAIT_RECV) ;
             Transport.mReceiveCallback=NULL ;
      if(cnt==SOCKET_ERROR) {                                       /* Если ошибка -                                   */
            status=WSAGetLastError() ;                              /*  - проверяем по коду ошибки                     */
         if(status!=0) {                                            /*     на форсированное закрытие коннекта сервером */
          if(error!=NULL)
                 sprintf(error, "Socket - %d", WSAGetLastError()) ;
                            break ;
                       }
                            }

                   } while(0) ;

/*---------------------------------------------------- Анализ ответа */

  if(status==0) do {
/*- - - - - - - - - - - - - - - - - - - -  Анализ кода возврата HTTP */
               memset(http_header, 0, sizeof(http_header)) ;        /* Выделяем HTTP-заголовок */
              strncpy(http_header, buff_in, sizeof(http_header)-1) ;

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
           end=strstr(buff_in, "<?xml ") ;
        if(end==NULL) {
                 sprintf(error, "HTTP - XML-content is missed") ;
                          status=-1 ;
                             break ;
                      }

               strcpy(buff_in, end) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                   } while(0) ;

/*-------------------------------------------- Завершение соединения */

                  Transport.ClientClose() ;

#ifndef UNIX
                             WSACleanup();
#endif

/*-------------------------------------------------------------------*/

  return(status) ;
}


/*********************************************************************/
/*								     */
/*                Функция управления приемом для HTTP                */

   int  HTTP_tcp::iReceive_ctrl(char *data, int  data_size)

{
   char  work[4096] ;
   char *entry ;
   char *content ;
   char *tail ;
   char *end ;
    int  size ;
    int  cnt ;

/*------------------------------------------ Выделение первой строки */

               memset(work, 0, sizeof(work)) ;                      /* Выделяем HTTP-заголовок */

        if(data_size<sizeof(work))  size=data_size ;
        else                        size=sizeof(work)-1 ;

              strncpy(work, data, size) ;

        content=strstr(work, "\r\n\r\n") ;                          /* Ищем вход контента */

           end=strchr(work, '\n') ;                                 /* Выделяем первую строку */
        if(end!=NULL)  *end=0 ;

/*-------------------------------------------------- Если есть ответ */

   if(strstr(work, " 200 OK"                   )!=NULL ||
      strstr(work, " 500 Internal Server Error")!=NULL   ) {

      if(strstr(work, " 500 Internal Server Error")!=NULL) {
                                                                *end=0 ;
                                                           }

        if(end!=NULL)  *end='\n' ;                                  /* Восстанавливаем данные */
      
        if(content==NULL)  return(0) ;                              /* Если нет разделителя контента... */
 
          *content = 0 ;                                            /* Отсекаем заголовок */
           content+= 4 ;                                            /* Проходим на контент */

                 strupr(work) ;                                     /* Переводим заголовок в верхний регистр */
/*- - - - - - - - - - - - - - - - - -  Метод передачи CONTENT-LENGTH */
#define   _KEY   "CONTENT-LENGTH:"

        entry=strstr(work, _KEY) ;                                  /* Ищем вход аттрибута длины данных */
     if(entry!=NULL) {                                              /* Если такой найден... */

                         entry+=strlen(_KEY) ;
            size=strtoul(entry, &end, 10) ;                         /* Извлекаем длину данных */

        if( (data_size-(content-work))>=size )  return(1) ;         /* Если все данные получены - выходим */

                               return(0) ;
                     }
/*- - - - - - - - - - - -  Метод передачи Transfer-Encoding: chunked */
#undef    _KEY
#define   _KEY   "TRANSFER-ENCODING: CHUNKED"

        entry=strstr(work, _KEY) ;                                  /* Ищем вход аттрибута */
     if(entry!=NULL) {                                              /* Если такой найден... */

                content =strstr(data, "\r\n\r\n") ;                 /* Ищем вход контента */
                content+= 4 ;
   
         while(1) {                                                 /* LOOP.1 - Проверяем получение всех блоков данных */
                       size=strtoul(content, &end, 16) ;            /*  Определяем размер блока */
                    if(size==0)  break ;                            /*  Если найден стоп-блок */

                    if(*end=='\r')  end++ ;                         /*  Проходим возврат каретки */
                    if(*end=='\n')  end++ ;

                    if(strlen(end)<size)  break ;                   /*  Если блок еще не догружен... */
                       
                       content=end+size ;

                    if(*content=='\r')  content++ ;                 /*  Проходим возврат каретки */
                    if(*content=='\n')  content++ ;

                  } ;                                               /* ENDLOOP.1 - Проверяем получение всех блоков данных */

             if(size!=0 || *content!='0')  return(0) ;              /* Если не все данные получены (данные завершаются символом '0') ... */

                content =strstr(data, "\r\n\r\n") ;                 /* Ищем вход контента */
                content+= 4 ;
                tail    =content ;
                 cnt    =tail-data ;
   
         while(1) {                                                 /* LOOP.2 - Сливаем блоки данных в один поток */
                       size=strtoul(content, &end, 16) ;            /*  Определяем размер блока */
                    if(size==0)  break ;                            /*  Если найден стоп-блок... */

                    if(*end=='\r')  end++ ;                         /*  Проходим возврат каретки */
                    if(*end=='\n')  end++ ;
                      
                        memcpy(tail, end, size) ;
                               tail+=size ;
                            content =end+size ;
                                cnt+=size ;

                    if(*content=='\r')  content++ ;                 /*  Проходим возврат каретки */
                    if(*content=='\n')  content++ ;

                  } ;                                               /*  ENDLOOP.2 - Сливаем блоки данных в один поток */

                              data[cnt]=0 ;

                               return(1) ;
                     } 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                         return(0) ;
                                                           }
/*------------------------------------------------------ Если ошибка */

   else                                                    {

        if(content!=NULL)  return(1) ;                              /* Если есть разделитель контента... */

                                                           }
/*-------------------------------------------------------------------*/

   return(0) ;

}


/*********************************************************************/
/*                                                                   */
/*                Рассчет хэш-функции                                */
/*                                                                   */
/*  Возвращает размер хэш-функции.                                   */
/*  Если data=NULL или hash=NULL реальное формирование хэш-функции   */
/*   не производится.                                                */
/*                                                                   */
/*  Для параметра FORMAT возможны следующие значения:                */
/*      TEXT_HEX     - текстовой 16-ричный                           */
/*      TEXT_BINARY  - текстовой двоичный                            */
/*      BINARY       - бинарный                                      */

   int  HTTP_tcp::MD5(char *data, int  data_size, char *hash, char *format)

{
   int  hash_size ;
  void *md5_header ;
  char  md5_hash[1024] ;

/*----------------------------------------- Определение размера хэша */

         if(         format== NULL         )   return(-1) ;
    else if(!stricmp(format, "BINARY"     ))  hash_size= 16 ;
    else if(!stricmp(format, "TEXT_BINARY"))  hash_size=128 ;
    else if(!stricmp(format, "TEXT_HEX"   ))  hash_size= 32 ;
    else                                       return(-1) ;

         if(data==NULL || hash==NULL)  return(hash_size) ;          /* Если рабочие буфера не заданы... */

/*------------------------------------------------------- Подготовка */

            md5_header=iMD5_init() ;
         if(md5_header==NULL)  return(-1) ;

/*---------------------------------------------- Рассчет хэш-функции */

            iMD5_stream(md5_header, data, data_size) ;
            iMD5_close (md5_header, md5_hash) ;	
	
            iMD5_format(md5_hash, hash, format) ;

/*-------------------------------------------------------------------*/

  return(hash_size) ;
}


/*********************************************************************/
/*                                                                   */
/*               Инициализация рассчета хэш-функции                  */
/*                                                                   */
/*  Возвращает указатель на управляющую структуру для последующего   */
/*  использованиq в процедурах расчета хэш-функции.                  */

   void *HTTP_tcp::iMD5_init(void)

{
  HTTP_md5_control *header ;


        header=(HTTP_md5_control *)calloc(1, sizeof(*header)) ;
     if(header==NULL)  return(NULL) ;
 
        header->total[0]=0 ;
        header->total[1]=0 ;

        header->state[0]=0x67452301 ;
        header->state[1]=0xEFCDAB89 ;
        header->state[2]=0x98BADCFE ;
        header->state[3]=0x10325476 ;

  return(header) ;
}


/*********************************************************************/
/*                                                                   */
/*               Поток рассчета хэш-функции                          */

   void  HTTP_tcp::iMD5_stream(void *header, char *data, int  size)

{
  HTTP_md5_control *md5 ;
               int  fill ;
     unsigned long  left ;

/*------------------------------------------------------- Подготовка */

    if(size<= 0)  return ;

              md5=(HTTP_md5_control *)header ;

/*----------------------------------------------------------- Расчет */

                left =md5->total[0]&0x3F ;
                fill =64-left ;

       md5->total[0]+=  size ;
       md5->total[0]&=0xFFFFFFFF ;

    if(md5->total[0]<size)  md5->total[1]++ ;

    if(left!=  0  && 
       size>=fill   ) {
                         memcpy(md5->buffer+left, data, fill) ;
                     iMD5_frame(md5, md5->buffer) ;

                              data+=fill ;
                              size-=fill ;
                              left = 0  ;
                      }

   while(size>= 64) {
                      iMD5_frame(md5, data) ;
                                      data+=64 ;
                                      size-=64 ;
                    }

    if(size>0)  memcpy(md5->buffer+left, data, size) ;

/*-------------------------------------------------------------------*/
}


/*********************************************************************/
/*                                                                   */
/*               Завершение рассчета хэш-функции                     */

   void  HTTP_tcp::iMD5_close(void *header, char *hash)

{
  HTTP_md5_control *md5 ;
     unsigned long  last ;
     unsigned long  pad ;
     unsigned long  high ;
     unsigned long  low ;
     unsigned char  slen[8];

static const unsigned char padding[64]={ 
 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* Преобразование unsigned long в unsigned char */
#define ULONG_TO_UCHAR(n, b, i)                    \
{                                                  \
    (b)[(i)    ] = (unsigned char) ( (n)       ) ; \
    (b)[(i) + 1] = (unsigned char) ( (n) >>  8 ) ; \
    (b)[(i) + 2] = (unsigned char) ( (n) >> 16 ) ; \
    (b)[(i) + 3] = (unsigned char) ( (n) >> 24 ) ; \
}

/*------------------------------------------------------- Подготовка */

              md5=(HTTP_md5_control *)header ;

/*---------------------------------------------- Завершение рассчета */

    high=(md5->total[0]>>29) | (md5->total[1]<<3) ;
     low=(md5->total[0]<< 3) ;

    ULONG_TO_UCHAR(low,  slen, 0) ;
    ULONG_TO_UCHAR(high, slen, 4) ;

    last= md5->total[0] & 0x3F ;
     pad=(last<56)?(56-last):(120-last) ;

       iMD5_stream(header, (char *)padding, pad) ;
       iMD5_stream(header, (char *)slen,      8) ;

    ULONG_TO_UCHAR(md5->state[0], hash,  0 ) ;
    ULONG_TO_UCHAR(md5->state[1], hash,  4 ) ;
    ULONG_TO_UCHAR(md5->state[2], hash,  8 ) ;
    ULONG_TO_UCHAR(md5->state[3], hash, 12 ) ;

/*-------------------------------------------- Освобождение ресурсов */

                    free(header) ;

/*-------------------------------------------------------------------*/

}


/*********************************************************************/
/*                                                                   */
/*               Рассчет кадра хэш-функции                           */

   void  HTTP_tcp::iMD5_frame(HTTP_md5_control *md5, void *data_)

{
    unsigned char *data ;
    unsigned long  X[16] ;
    unsigned long  A, B, C, D ;

/* Преобразование unsigned char в unsigned long */
#define UCHAR_TO_ULONG(n, b, i)                    \
{                                                  \
    (n) = ( (unsigned long) (b)[(i)    ]       )   \
        | ( (unsigned long) (b)[(i) + 1] <<  8 )   \
        | ( (unsigned long) (b)[(i) + 2] << 16 )   \
        | ( (unsigned long) (b)[(i) + 3] << 24 ) ; \
}

/* Сдвиг */
#define S(x, n)	((x << n) | ((x & 0xFFFFFFFF) >> (32 - n)))

/* Общий вид нелинейной функции */
#define P(a, b, c, d, k, s, t ) \
{                               \
    a+=F(b, c, d)+X[k]+t ;	\
    a =S(a, s   )+b ;           \
}

/*------------------------------------------------ Подготовка данных */

        data=(unsigned char *)data_ ;

    UCHAR_TO_ULONG(X[ 0], data,  0) ;                               /* Раскладка данных в бинарный поток */
    UCHAR_TO_ULONG(X[ 1], data,  4) ;
    UCHAR_TO_ULONG(X[ 2], data,  8) ;
    UCHAR_TO_ULONG(X[ 3], data, 12) ;
    UCHAR_TO_ULONG(X[ 4], data, 16) ;
    UCHAR_TO_ULONG(X[ 5], data, 20) ;
    UCHAR_TO_ULONG(X[ 6], data, 24) ;
    UCHAR_TO_ULONG(X[ 7], data, 28) ;
    UCHAR_TO_ULONG(X[ 8], data, 32) ;
    UCHAR_TO_ULONG(X[ 9], data, 36) ;
    UCHAR_TO_ULONG(X[10], data, 40) ;
    UCHAR_TO_ULONG(X[11], data, 44) ;
    UCHAR_TO_ULONG(X[12], data, 48) ;
    UCHAR_TO_ULONG(X[13], data, 52) ;
    UCHAR_TO_ULONG(X[14], data, 56) ;
    UCHAR_TO_ULONG(X[15], data, 60) ;

            A=md5->state[0] ;                                       /* Записываем в A,B,C,D значения по умолчанию */
            B=md5->state[1] ;
            C=md5->state[2] ;
            D=md5->state[3] ;

/*----------------------------------------------------------- 1 этап */

#define F(x,y,z) (z ^ (x & (y ^ z)))

    P( A, B, C, D,  0,  7, 0xD76AA478 ) ;
    P( D, A, B, C,  1, 12, 0xE8C7B756 ) ;
    P( C, D, A, B,  2, 17, 0x242070DB ) ;
    P( B, C, D, A,  3, 22, 0xC1BDCEEE ) ;
    P( A, B, C, D,  4,  7, 0xF57C0FAF ) ;
    P( D, A, B, C,  5, 12, 0x4787C62A ) ;
    P( C, D, A, B,  6, 17, 0xA8304613 ) ;
    P( B, C, D, A,  7, 22, 0xFD469501 ) ;
    P( A, B, C, D,  8,  7, 0x698098D8 ) ; 
    P( D, A, B, C,  9, 12, 0x8B44F7AF ) ; 
    P( C, D, A, B, 10, 17, 0xFFFF5BB1 ) ;
    P( B, C, D, A, 11, 22, 0x895CD7BE ) ;
    P( A, B, C, D, 12,  7, 0x6B901122 ) ;
    P( D, A, B, C, 13, 12, 0xFD987193 ) ;
    P( C, D, A, B, 14, 17, 0xA679438E ) ;
    P( B, C, D, A, 15, 22, 0x49B40821 ) ;

#undef F

/*----------------------------------------------------------- 2 этап */

#define F(x,y,z) (y ^ (z & (x ^ y)))

    P( A, B, C, D,  1,  5, 0xF61E2562 ) ;
    P( D, A, B, C,  6,  9, 0xC040B340 ) ;
    P( C, D, A, B, 11, 14, 0x265E5A51 ) ;
    P( B, C, D, A,  0, 20, 0xE9B6C7AA ) ;
    P( A, B, C, D,  5,  5, 0xD62F105D ) ;
    P( D, A, B, C, 10,  9, 0x02441453 ) ;
    P( C, D, A, B, 15, 14, 0xD8A1E681 ) ;
    P( B, C, D, A,  4, 20, 0xE7D3FBC8 ) ;
    P( A, B, C, D,  9,  5, 0x21E1CDE6 ) ;
    P( D, A, B, C, 14,  9, 0xC33707D6 ) ;
    P( C, D, A, B,  3, 14, 0xF4D50D87 ) ;
    P( B, C, D, A,  8, 20, 0x455A14ED ) ;
    P( A, B, C, D, 13,  5, 0xA9E3E905 ) ;
    P( D, A, B, C,  2,  9, 0xFCEFA3F8 ) ;
    P( C, D, A, B,  7, 14, 0x676F02D9 ) ;
    P( B, C, D, A, 12, 20, 0x8D2A4C8A ) ;

#undef F
  
/*----------------------------------------------------------- 3 этап */

#define F(x,y,z) (x ^ y ^ z)

    P( A, B, C, D,  5,  4, 0xFFFA3942 ) ;
    P( D, A, B, C,  8, 11, 0x8771F681 ) ;
    P( C, D, A, B, 11, 16, 0x6D9D6122 ) ;
    P( B, C, D, A, 14, 23, 0xFDE5380C ) ;
    P( A, B, C, D,  1,  4, 0xA4BEEA44 ) ;
    P( D, A, B, C,  4, 11, 0x4BDECFA9 ) ;
    P( C, D, A, B,  7, 16, 0xF6BB4B60 ) ;
    P( B, C, D, A, 10, 23, 0xBEBFBC70 ) ;
    P( A, B, C, D, 13,  4, 0x289B7EC6 ) ;
    P( D, A, B, C,  0, 11, 0xEAA127FA ) ;
    P( C, D, A, B,  3, 16, 0xD4EF3085 ) ;
    P( B, C, D, A,  6, 23, 0x04881D05 ) ;
    P( A, B, C, D,  9,  4, 0xD9D4D039 ) ;
    P( D, A, B, C, 12, 11, 0xE6DB99E5 ) ;
    P( C, D, A, B, 15, 16, 0x1FA27CF8 ) ;
    P( B, C, D, A,  2, 23, 0xC4AC5665 ) ;

#undef F

/*----------------------------------------------------------- 4 этап */

#define F(x,y,z) (y ^ (x | ~z))

    P( A, B, C, D,  0,  6, 0xF4292244 ) ;
    P( D, A, B, C,  7, 10, 0x432AFF97 ) ;
    P( C, D, A, B, 14, 15, 0xAB9423A7 ) ;
    P( B, C, D, A,  5, 21, 0xFC93A039 ) ;
    P( A, B, C, D, 12,  6, 0x655B59C3 ) ;
    P( D, A, B, C,  3, 10, 0x8F0CCC92 ) ;
    P( C, D, A, B, 10, 15, 0xFFEFF47D ) ;
    P( B, C, D, A,  1, 21, 0x85845DD1 ) ;
    P( A, B, C, D,  8,  6, 0x6FA87E4F ) ;
    P( D, A, B, C, 15, 10, 0xFE2CE6E0 ) ;
    P( C, D, A, B,  6, 15, 0xA3014314 ) ;
    P( B, C, D, A, 13, 21, 0x4E0811A1 ) ;
    P( A, B, C, D,  4,  6, 0xF7537E82 ) ;
    P( D, A, B, C, 11, 10, 0xBD3AF235 ) ;
    P( C, D, A, B,  2, 15, 0x2AD7D2BB ) ;
    P( B, C, D, A,  9, 21, 0xEB86D391 ) ;

#undef F

/*---------------------------------------------------------- Слияние */

                md5->state[0]+=A ;
                md5->state[1]+=B ;
                md5->state[2]+=C ;
                md5->state[3]+=D ;

/*-------------------------------------------------------------------*/
}


/*********************************************************************/
/*                                                                   */
/*       Формирование выходного представления хэш-функции            */
/*                                                                   */
/*  Для параметра FORMAT возможны следующие значения:                */
/*      TEXT_HEX     - текстовой 16-ричный                           */
/*      TEXT_BINARY  - текстовой двоичный                            */
/*      BINARY       - бинарный                                      */

   void  HTTP_tcp::iMD5_format(char *hash, char *text, char *format)

{
           char  hex[256] ; 
  unsigned char  tmp_l ;
  unsigned char  tmp_r ;
            int  i ;

/*------------------------------------------------------- Подготовка */

             memset(hex, 0, sizeof(hex)) ;

/*------------------------------------------ 16-ричное представление */

   if(!stricmp(format, "TEXT_HEX"))
   {
        for(i=0 ; i<16 ; i++) {
                                tmp_l=hash[i]>>4 & 0x0f ;
                                tmp_r=hash[i]    & 0x0f ;

              if(tmp_l>9)  hex[2*i  ]='A'+tmp_l-10 ;
              else         hex[2*i  ]='0'+tmp_l   ;

              if(tmp_r>9)  hex[2*i+1]='A'+tmp_r-10 ;
              else         hex[2*i+1]='0'+tmp_r   ;

                              }
    
        strcpy(text, hex) ;
   }
/*------------------------------------------- Двоичное представление */
   else
   if(!stricmp(format, "TEXT_BINARY"))
   {
   } 
/*------------------------------------------- Бинарное представление */
   else
   {
             memcpy(text, hash, 128 );
                    text[128]=0 ;
   } 
/*-------------------------------------------------------------------*/

}



/*********************************************************************/
/*                                                                   */
/*              Библиотека обмена сообщениями по TCP                 */
/*                                                                   */
/*********************************************************************/

#ifndef  _HTTP_TCP_H_
#define  _HTTP_TCP_H_


/*------------------------------------------------- Специальные типы */

typedef struct {
                    unsigned long total[2] ;     /* обработано байт */
                    unsigned long state[4] ;     /* промежуточные результаты вычислений */
                    unsigned char buffer[64] ;   /* обрабатываемые данные */
               } HTTP_md5_control ;

/*-------------------------------------------- Класс HTTP-транспорта */

  class  HTTP_tcp {

    public:
              char  *mURL ;                           /* URL сервиса */
               int   mError ;                         /* Несистемные ошибки */

    public:
             void  Free         (void) ;                        /* Освобождение ресурсов */

      static  int  Base64_incode(char *, char *, int) ;         /* Кодирование в Base64 */
      static  int  Base64_decode(char *, char *) ;              /* Декодирование из Base64 */
      static  int  MD5          (char *, int, char *, char *) ; /* Построение хэша MD5 */
                                                
      static  int  SOAP_exchange(char *, char *,                /* Отправка SOAP-сообщения по HTTP */
                                         char *, int, char *) ;
      static  int  SOAP_exchange(char *, char *, char **, 
                                         char *, int, char *) ;

      static  int  iReceive_ctrl(char *, int) ;                 /* Функция управления приемом для HTTP */
      static void *iMD5_init    (void) ;                        /* Инициализация рассчета хэш-функции */
      static void  iMD5_stream  (void *, char *, int) ;         /* Поток рассчета хэш-функции */
      static void  iMD5_close   (void *, char *) ;              /* Завершение рассчета хэш-функции */
      static void  iMD5_frame   (HTTP_md5_control *, void *) ;  /* Рассчет кадра хэш-функции */
      static void  iMD5_format  (char *, char *, char *) ;      /* Формирование выходного представления хэш-функции */

           public:
                         HTTP_tcp() ;                            /* Конструктор */
                        ~HTTP_tcp() ;                            /* Деструктор */
                                
                                         } ;

/*-------------------------------------------------------------------*/

#endif         // _HTTP_TCP_H_

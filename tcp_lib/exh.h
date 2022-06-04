/*********************************************************************/
/*                                                                   */
/*              Библиотека обмена сообщениями по TCP                 */
/*                                                                   */
/*********************************************************************/

#ifndef  _TCP_EXH_H_
#define  _TCP_EXH_H_

#include  <string>

/*----------------------------------------------------- Пакет обмена */

 typedef  struct  {
            std::string  data_out ;                     /* Поток исходящих данных */
            std::string  data_in ;                      /* Поток входящих данных */
                    int  channel_send ;                 /* Канал отправки */
                    int  channel_recv ;                 /* Канал приема */
                  } Tcp_packet ;

 typedef  struct {                         /* Описание задания */
                   int  use_flag ;          /* Флаг использования */
            Tcp_packet  packet ;            /* Пакет данных */
                   int  done_flag ;         /* Флаг завершения отработки задания */
                  void *thread ;            /* Ссылка на поток обработки */
                  void *transit ;           /* Транзитные данные */
                 }  Tcp_job ;

/*----------------------------------------------- Базовый класс транспорта */

  class  Tcp_exchange {

           public:
                   char  mLog_file[1024] ;		 /* Путь к лог-файлу */

           public:
                    int  Exchange      (Tcp_packet *, Tcp_packet *) ;     /* Обмен Клиент-Сервер */

            virtual int  vCloseConnect (void *) ;                         /* Закрытие канала связи */
            virtual int  vProcessData  (Tcp_job *, void *) ;              /* Обработка полученных данных */
            virtual int  vExternControl(void) ;                           /* Обработчик внешнего управления */

		    int  iLog_file     (char *) ;			  /* Запись лог-файла */

           public:
                         Tcp_exchange () ;                               /* Конструктор */
                        ~Tcp_exchange () ;                               /* Деструктор */
                                
                       } ;

/*-------------------------------------------------------------------*/

#endif         // _ABTP_EXH_H_

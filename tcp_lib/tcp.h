/*********************************************************************/
/*                                                                   */
/*              Библиотека обмена сообщениями по TCP                 */
/*                                                                   */
/*********************************************************************/

#ifndef  _TCP_H_
#define  _TCP_H_

#ifdef  UNIX
typedef  int  SOCKET ;
#else
typedef  int  socklen_t ;
#endif

/*---------------------------------------------- Параметры генерации */

#define      _SOCKET_BLOCK_SIZE        512000

/*----------------------------------------------- Специальные ошибки */

#define            _MEMORY_OVERFLOW   1
#define           _SYS_MSG_OVERFLOW   2
#define           _SYS_MSG_CORRUPT    3
#define            _PACKET_CORRUPT    4
#define          _IDENTIFY_REJECT     5
#define           _INVALID_MESSAGE    6
#define           _CONNECT_DROPPED    7
#define              _SEND_FAIL       8
#define            _SECURE_UNKNOWN    9
#define            _SECURE_CORRUPT   10

/*------------------------------------------------- Специальные типы */

 typedef  struct {                          /* Описание клиента */
                    int   use_flag ;           /* Флаг использования */
                    int   close_flag ;         /* Флаг закрытия */
                   char   ip[32] ;             /* IP-адрес контакта */
                 SOCKET   socket ;             /* Сокет */
                    int   error ;              /* Несистемные ошибки */

                Tcp_job  *job ;                /* Задание */

                    int   stage ;              /* Этап обмена */
#define                    _RECEIVE_STAGE  1
#define                    _PROCESS_STAGE  2
#define                       _SEND_STAGE  3
#define                      _CLOSE_STAGE  4

                   char  *recv_data ;          /* Буфер приема */
                    int   recv_data_max ;
                    int   recv_data_size ;
                    int   recv_data_done ;
                    int   recv_data_deff ;     /* Число байт в буфере оставш. от предыдущего приема */

                   char  *send_data ;          /* Буфер отправки */
                    int   send_data_max ;
                    int   send_data_size ;
                    int   send_data_done ;
                   char  *send_all ;           /* Поток отправляемых данных */
                    int   send_all_size ;
                    int   send_all_done ;

                 }  Tcp_client ;

/*--------------------------------------------- Класс TCP-транспорта */

  class  Tcp : public Tcp_exchange {

           public:
                    int   mTime_check ;                    /* Временная метка */
                    int  *mTime_extrn ;

           public:
                    int   mDemon ;                         /* Флаг работы Демоном */
                 SOCKET   mSocket_srv ;                    /* Контактный сокет Сервера */
                 SOCKET   mSocket_cli ;                    /* Рабочий сокет Клиента */

             Tcp_client  *mClients ;                       /* Список клиентов */
                    int   mClients_max ; 

           public:
                   char  *mServer_name ;                   /* Имя сервера */
                   char   mServer_ip[128] ;                /* IP-адрес сервера */
                    int   mServer_port ;                   /* Контактный порт сервера */

                    int   mSocket_listen_max ;             /* Максимальная длина входной очереди */

                    int   mWait_answer ;                   /* Максимальное время ожидания ответа */

                    int (*mReceiveCallback)(char *, int) ; /* Внешняя функция управления приемом */

                    int   mError ;                         /* Несистемные ошибки */
                    int   mStage ;                         /* Этап связи клиента с сервером: */
#define                   _ABTP_CLIENT_CONNECT    1 
#define                   _ABTP_CLIENT_HANDSHAKE  2 
#define                   _ABTP_CLIENT_IDENTIFY   3 
#define                   _ABTP_CLIENT_DATA       4 
#define                   _ABTP_CLIENT_CLOSE      5 

           public:
                   void  Free           (void) ;                    /* Освобождение ресурсов */

                    int  ServerProcess  (void) ;                    /* Серверный процесс */
                    int  DemonProcess   (void) ;                    /* Сервер - UNIX-демон */
                    int  InitServerPort (void) ;                    /* Сервер: Подготовка контактного порта сервера */
                    int  Wait           (SOCKET *) ;                /* Сервер: Ожидание событий на линиях Клиент-Сервер */
                    int  AddClient      (void) ;                    /* Сервер: Регистрация нового клиента */
                    int  SrvRecvClient  (Tcp_client *) ;            /* Сервер: Прием данных по каналу клиента */
                    int  SrvExtrClient  (Tcp_client *) ;            /* Сервер: Разборка данных по каналу клиента */
                    int  SrvProcClient  (Tcp_client *) ;            /* Сервер: Обработка данных по каналу клиента */
                    int  SrvPrepClient  (Tcp_client *) ;            /* Сервер: Подготовка отправки данных по каналу клиента */
                    int  SrvSendClient  (Tcp_client *) ;            /* Сервер: Отправка данных по каналу клиента */

                    int  Client         (Tcp_packet *) ;

                    int  LinkToServer   (void) ;                    /* Клиент: Соединение с сервером */

                    int  ClientSend     (std::string *) ;           /* Клиент: Передача блока данных */
                    int  ClientReceive  (std::string *) ;           /* Клиент: Прием блока данных */
                    int  ClientClose    (void) ;                    /* Клиент: Закрытие порта */

                    int  Close          (SOCKET, int) ;             /* Закрытие канала с сервером */
                    int  Send           (SOCKET, char *, int) ;     /* Передача блока данных */                    
                    int  Receive        (SOCKET, char *, int) ;     /* Прием блока данных */                    

                    int  iSend          (SOCKET, char *,            /* Передача блока данных по сокету */
                                                  int  , int) ;
                    int  iReceive       (SOCKET, char *,            /* Прием блока данных по сокету */
                                                  int  , int) ;
#define                    _NOWAIT_RECV  0
#define                      _WAIT_RECV  1

                    int  iClientPaketsPrepare (Tcp_client *) ;      /* Подготовка списка пакетов для приема данных */
                    int  iClientPaketsClear   (Tcp_client *) ;      /* Очистка списка пакетов */
                    int  iClientResourcesClear(Tcp_client *) ;      /* Очистка ресурсов клиента */

            virtual int  vExternal      (SOCKET) ;                  /* Отработка внешнего управления */
            virtual int  vMessagesLog   (Tcp_client *,              /* Ведение лога обмена */
                                             std::string *, char *) ;

           public:
                         Tcp() ;                                    /* Конструктор */
                        ~Tcp() ;                                    /* Деструктор */
                                
                                         } ;

/*-------------------------------------------------------------------*/

#endif         // _ABTP_TCP_H_

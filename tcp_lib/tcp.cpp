/*********************************************************************/
/*                                                                   */
/*              Библиотека обмена сообщениями по TCP                 */
/*                                                                   */
/*********************************************************************/

/*********************************************************************/
/*								     */
/*   Протокол обмена Клиент-Сервер строится следующим образом:       */
/*    - 8 байта - размер передаваемых данных (в текстовом формате,   */
/*                   включая терминирующий 0)                        */
/*    - Данные                                                       */
/*                                                                   */
/*********************************************************************/

#include <stddef.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>

#ifdef  UNIX
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#ifdef  SOLARIS
#include <stdlib.h>
#include <string.h>
#include <sys/filio.h>
#endif

#ifdef  AIX
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <stropts.h>
#endif

#define  SOCKADDR_IN     sockaddr_in
#define  PSOCKADDR       struct sockaddr *
#define  INVALID_SOCKET  -1
#define  SOCKET_ERROR    -1 

#define  closesocket        close
#define  ioctlsocket        ioctl
#define  WSAGetLastError()  errno
#define  stricmp            strcasecmp
#else
#include <winsock2.h>
#define  sleep(p)        Sleep(p*1000)

#endif

#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#pragma warning(disable : 4996)

#include "exh.h"
#include "tcp.h"

#ifdef UNIX

  struct in_addr_ {
                    union {
                             struct { uint8_t s_b1, s_b2, s_b3, s_b4; } S_un_b ;
                             struct { uint16_t s_w1, s_w2; } S_un_w ;
                                  in_addr_t S_addr ;
                             struct in_addr S_std ;
                          } S_un ;
                  } ;

#else

  struct in_addr_ {
                    union {
                             struct { UCHAR s_b1,s_b2,s_b3,s_b4; } S_un_b;
                             struct { USHORT s_w1,s_w2; } S_un_w;
                                      ULONG S_addr;
                             struct in_addr S_std ;
                          } S_un ;
                  } ;

#endif


/*********************************************************************/
/*								     */
/*		       Конструктор класса			     */

     Tcp::Tcp(void)

{
   mSocket_listen_max =  5 ;
          mWait_answer=  0 ;
               mDemon =  0 ;

          mServer_port=  0 ;

          mSocket_srv = -1 ;
          mSocket_cli =NULL ;

      mReceiveCallback=NULL ;

         mClients     =NULL ;
         mClients_max =  0 ;

           mTime_check=  0 ;
           mTime_extrn=NULL ;
}


/*********************************************************************/
/*								     */
/*		        Деструктор класса			     */

    Tcp::~Tcp(void)

{
        Free() ;
}


/*********************************************************************/
/*								     */
/*		       Освобождение ресурсов                         */

  void  Tcp::Free(void)

{
}


/*********************************************************************/
/*								     */
/*                          Сервер                                   */

  int  Tcp::ServerProcess(void)

{
           SOCKET  connect ;
              int  status ;
              int  i ;

/*------------------------------------------------------- Подготовка */

            status=InitServerPort() ;
         if(status)  return(status) ;

/*-------------------------------------------- Основной рабочий цикл */

    do {
/*- - - - - - - - - - - - - - Ожидание и системная обработка событий */
            status=vExternControl() ;                               /* Отрабатываем внешнее управление */
         if(status)  break ;                                        /* Если команда на останов */

            status=Wait(&connect) ;                                 /* Ожидание активности */
         if(status)  break ;

                                mTime_check= time(NULL) ;           /* Проставка контрольных временных меток */
         if(mTime_extrn!=NULL) *mTime_extrn=mTime_check ;
/*- - - - - - - - - - - - - - - - - - - Подсоединение нового клиента */
         if(connect==mSocket_srv)  AddClient() ;                    /* Если активен контактный порт - */
                                                                    /*     подключаем нового клиента  */
/*- - - - - - - - - - - - - - - - - - - - - - - - Обработка клиентов */
      for(i=0 ; i<mClients_max ; i++) {

        if(mClients[i].use_flag==0)  continue ;
        
        if(mClients[i].stage==_RECEIVE_STAGE) { 
                                     SrvRecvClient(&mClients[i]) ;  /* Обработка входного потока */
                                     SrvExtrClient(&mClients[i]) ;  
                                              }
        if(mClients[i].stage==_PROCESS_STAGE) { 
                                     SrvProcClient(&mClients[i]) ;  /* Процессинг данных */
                                              }
        if(mClients[i].stage==   _SEND_STAGE) { 
                                     SrvPrepClient(&mClients[i]) ;  /* Обработка выходного потока */
                                     SrvSendClient(&mClients[i]) ;
                                              }

                                      }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
       } while(1) ;

/*------------------------------------------------------- Завершение */

//     for(i=0 ; i<clients_max ; i++)                               /* Закрытие портов клиентов */
//       if(clients[i].socket!=NULL)  Close(clients[i].socket, 1) ;

                                      Close(mSocket_srv, 0) ;       /* Закрытие контактного порта Сервера */

/*-------------------------------------------------------------------*/

   return(status) ;
}


/*********************************************************************/
/*								     */
/*                     Сервер - UNIX-демон                           */

  int  Tcp::DemonProcess(void)

{
           SOCKET  connect ;
              int  status ;

/*------------------------------------------------------- Подготовка */

                        mDemon= 1 ;
                   mSocket_srv=-1 ;
                   mSocket_cli= 0 ;

/*-------------------------------------------- Основной рабочий цикл */

            status=AddClient() ;                                    /* Подключаем нового клиента  */
         if(status)  exit(-1) ;

    do {
/*- - - - - - - - - - - - - - Ожидание и системная обработка событий */
            status=vExternControl() ;                               /* Отрабатываем внешнее управление */
         if(status)  break ;                                        /* Если команда на останов */

            status=Wait(&connect) ;                                 /* Ожидание активности */
         if(status)  break ;

                                mTime_check= time(NULL) ;           /* Проставка контрольных временных меток */
         if(mTime_extrn!=NULL) *mTime_extrn=mTime_check ;
/*- - - - - - - - - - - - - - - - - - - - - - - - Обработка клиентов */
        if(mClients[0].use_flag==0)  break ;
        
        if(mClients[0].stage==_RECEIVE_STAGE) { 
                                     SrvRecvClient(&mClients[0]) ;  /* Обработка входного потока */
                                     SrvExtrClient(&mClients[0]) ;  
                                              }
        if(mClients[0].stage==_PROCESS_STAGE) { 
                                     SrvProcClient(&mClients[0]) ;  /* Процессинг данных */
                                              }
        if(mClients[0].stage==   _SEND_STAGE) { 
                                     SrvPrepClient(&mClients[0]) ;  /* Обработка выходного потока */
                                     SrvSendClient(&mClients[0]) ;
                                              }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
       } while(1) ;

/*------------------------------------------------------- Завершение */

            Close(mSocket_cli, 0) ;

/*-------------------------------------------------------------------*/

   return(status) ;
}


/*********************************************************************/
/*								     */
/*      	  Прием данных по каналу клиента  		     */

  int  Tcp::SrvRecvClient(Tcp_client *client)

{
   char  buff[1024] ;
    int  size ;
    int  cnt ;
   char *end ;

#define   DATA        client->recv_data
#define   DATA_SIZE   client->recv_data_size
#define   DATA_DONE   client->recv_data_done
#define   DATA_MAX    client->recv_data_max
#define   DATA_DEFF   client->recv_data_deff

/*------------------------------------------------- Входной контроль */

   if(client->stage!=_RECEIVE_STAGE)  return(0) ;

/*-------------------------------------- Определение размера посылки */

   if(DATA_SIZE==0) {
/*- - - - - - - - - - - - - - - - - - - - - -  Прием размера посылки */
             memset(buff, 0, sizeof(buff)) ;

    if(DATA_DEFF)  memcpy(buff, DATA, DATA_DEFF) ;

       cnt=iReceive(client->socket, buff+DATA_DEFF,                 /* Принимаем размер посылки */
                                       8-DATA_DEFF,
                                      _NOWAIT_RECV) ;
    if(cnt==SOCKET_ERROR ||                                         /* Если системная ошибка     */
       cnt!=8-DATA_DEFF    ) {                                      /*    или неполные данные... */
                               client->error     =_SYS_MSG_CORRUPT ;      
                               client->close_flag=  1 ;
                                        DATA_DEFF=  0 ;
                                    return(-1) ;  
                             } 

       cnt+=DATA_DEFF ; 
            DATA_DEFF=0 ;

       size=strtoul(buff, &end, 10) ;                               /* Декодируем размер посылки */
    if(*end!=0) {                                                   /* Если некорректные данные... */
                   client->error     =_SYS_MSG_CORRUPT ; 
                   client->close_flag=  1 ;
                        return(-1) ;
                }
/*- - - - - - - - - - - - - - - - - - - Выделение памяти для посылки */
    if(DATA_MAX<=size) {
         DATA_MAX=size+1024 ;
         DATA    =(char *)realloc(DATA, DATA_MAX) ;
      if(DATA==NULL) {
                         client->error     =_MEMORY_OVERFLOW ;
                         client->close_flag=  1 ;
                                     return(-1) ;
                     }
                       }
/*- - - - - - - - - - - - - - - - - - - - - - -  Подготовка к приему */
                           DATA_SIZE=size ;
                           DATA_DONE=  0 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                    }
/*---------------------------------------------------- Прием посылки */

   if(DATA_SIZE!=DATA_DONE) {

      size=DATA_SIZE-DATA_DONE ;                                    /* Опр.размер непринятой части посылки */
       cnt=iReceive(client->socket,                                 /* Принимаем посылку */
                     DATA+DATA_DONE, size, _NOWAIT_RECV) ;

    if(cnt==SOCKET_ERROR) {                                         /* Если системная ошибка... */
                               client->error     =_CONNECT_DROPPED ;      
                               client->close_flag=  1 ;
                                    return(-1) ;  
                          } 

                                DATA_DONE+=cnt ;
                                DATA_DEFF = 0 ; 
                            }
/*-------------------------------------------------------------------*/

#undef   DATA
#undef   DATA_SIZE
#undef   DATA_DONE
#undef   DATA_MAX

  return(0) ;
}


/*********************************************************************/
/*								     */
/*      	Предварительная обработка входных данных             */

  int  Tcp::SrvExtrClient(Tcp_client *client)

{
     int  status ;

#define   DATA        client->recv_data
#define   DATA_SIZE   client->recv_data_size
#define   DATA_DONE   client->recv_data_done
#define   DATA_MAX    client->recv_data_max

/*------------------------------------------------- Входной контроль */

     if(client->stage!=_RECEIVE_STAGE)  return(0) ;

     if( DATA_DONE!=DATA_SIZE)  return(0) ;

/*-------------------------------------------- Прием пакетных данных */

         status=iClientPaketsPrepare(client) ;                      /* Подготовка приема в режиме */
      if(status) {
                     client->close_flag=1 ;
                         return(-1) ;
                 }

         client->job->packet.data_in.append(DATA, DATA_SIZE) ;      /* Пропись данных в пакет */

         client->stage=_PROCESS_STAGE ;

/*--------------------------------------------------- Очистка данных */

                 DATA_SIZE=0 ;

/*-------------------------------------------------------------------*/

#undef   DATA
#undef   DATA_SIZE
#undef   DATA_DONE
#undef   DATA_MAX

  return(0) ;
}


/*********************************************************************/
/*								     */
/*                Обработка данных по каналу клиента                 */

  int  Tcp::SrvProcClient(Tcp_client *client)

{
   int  status ;

/*------------------------------------------------- Входной контроль */

     if(client->stage!=_PROCESS_STAGE)  return(0) ;

/*---------------------------------------- Вызов процедуры обработки */

                            client->job->done_flag=  0 ;
        status=vProcessData(client->job, client) ;
    if(!status)             client->job->done_flag=  1 ;

/*-------------------------------------------------------------------*/

        client->stage=_SEND_STAGE ;

  return(0) ;
}


/*********************************************************************/
/*								     */
/*           Подготовка отправки данных по каналу клиента            */

  int  Tcp::SrvPrepClient(Tcp_client *client)

{
 char  size_msg[64] ;

#define    ALL        client->send_all
#define    ALL_SIZE   client->send_all_size
#define    ALL_DONE   client->send_all_done
#define   DATA        client->send_data
#define   DATA_SIZE   client->send_data_size
#define   DATA_DONE   client->send_data_done
#define   DATA_MAX    client->send_data_max

/*------------------------------------------------- Входной контроль */

     if(client->stage!=_SEND_STAGE)  return(0) ;
     if(    DATA_SIZE!= DATA_DONE )  return(0) ;

/*--------------------------------------- Размещение буфера отправки */

   if(client->send_data==NULL) {

        client->send_data_max= _SOCKET_BLOCK_SIZE+1024 ; 
        client->send_data    =(char *)
                                calloc(1, client->send_data_max) ;
     if(client->send_data==NULL) {
                                   client->error     =_MEMORY_OVERFLOW ;      
                                   client->close_flag=  1 ;
                                     return(-1) ;  
                                 }
                               }
/*----------------------------------------- Подготовка потока данных */

       ALL     =(char *)client->job->packet.data_out.data() ;
       ALL_SIZE=        client->job->packet.data_out.size() ;
       ALL_DONE=          0 ;

          sprintf(size_msg, "%07d", ALL_SIZE) ;
           strcat(DATA, size_msg) ;                            

                       DATA_DONE=  0 ;
                       DATA_SIZE=strlen(DATA)+1 ;
           memcpy(DATA+DATA_SIZE, ALL, ALL_SIZE) ;
                       DATA_SIZE+=ALL_SIZE ;

/*-------------------------------------------------------------------*/

#undef   DATA
#undef   DATA_SIZE
#undef   DATA_DONE
#undef   DATA_MAX

  return(0) ;
}


/*********************************************************************/
/*								     */
/*                 Отправка данных по каналу клиента                 */

  int  Tcp::SrvSendClient(Tcp_client *client)

{
  int  size ;
  int  cnt ;

#define   DATA        client->send_data
#define   DATA_SIZE   client->send_data_size
#define   DATA_DONE   client->send_data_done
#define   DATA_MAX    client->send_data_max

/*------------------------------------------------- Входной контроль */

     if(client->stage!=_SEND_STAGE)  return(0) ;
     if(    DATA_SIZE==   0       )  return(0) ;

/*------------------------------------------------- Передача посылки */

   if(DATA_SIZE!=DATA_DONE) {

      size=DATA_SIZE-DATA_DONE ;                                    /* Опр.размер непереданной части посылки */
       cnt=iSend(client->socket,                                    /* Передаем посылку */
                    DATA+DATA_DONE, size, _NOWAIT_RECV) ;

    if(cnt==SOCKET_ERROR) {                                         /* Если системная ошибка... */
                               client->error     =_CONNECT_DROPPED ;      
                               client->close_flag=  1 ;
                                    return(-1) ;  
                          } 

                                DATA_DONE+=cnt ;
                            }
/*---------------------------------------- Очистка переданных данных */

   if(DATA_SIZE==DATA_DONE) {                        
                                memset(DATA, 0, DATA_SIZE) ;
                                                DATA_SIZE=0 ;
                                                DATA_DONE=0 ;
                            }
/*-------------------------------------------------------------------*/

#undef   DATA
#undef   DATA_SIZE
#undef   DATA_DONE
#undef   DATA_MAX

  return(0) ;
}


/*********************************************************************/
/*								     */
/*                   Отработка связи с Сервером                      */
/*                 для однократного обмена пакетами                  */

  int  Tcp::Client(Tcp_packet *packet)

{
  std::string  message ;
          int  status ;

/*-------------------------------------------- Соединение с сервером */

           status=LinkToServer() ;
        if(status)  return(status) ;

/*-------------------------------------------------- Передача пакета */

            status=ClientSend(&packet->data_out) ;
         if(status)  return(status) ;       
                                    
/*----------------------------------------------------- Прием пакета */

            status=ClientReceive(&packet->data_in) ;
         if(status)  return(status) ;
                                          
/*--------------------------------------- Закрытие канала с сервером */

                   ClientClose() ;

/*-------------------------------------------------------------------*/

    return(0) ;
}


/*********************************************************************/
/*								     */
/*		     Соединение с сервером                           */

  int  Tcp::LinkToServer(void)

{


  struct hostent *host ;
 struct in_addr_  i_addr ;
     SOCKADDR_IN  server_addr ;
             int  status ;
            char *tmp ;

#ifndef  UNIX
            BOOL  bOptVal ;
             int  bOptLen ;
#endif

/*------------------------------------------------- Входной контроль */
    
    if(mSocket_cli!= NULL         &&
       mSocket_cli!=INVALID_SOCKET  )  return(0) ;

/*------------------------- Определение сетевого адреса по имени DNS */
/*- - - - - - - - - - - - - - - - - - - - - - - Для явного IP-адреса */
   if(isdigit(mServer_name[0])) {
                                  strcpy(mServer_ip, mServer_name) ;
                                }
/*- - - - - - - - - - - - - - - - - - - - - - - - - -  Для имени DNS */
   else                         {

           host=gethostbyname(mServer_name) ;
        if(host==NULL)  return(WSAGetLastError()) ;

                        i_addr.S_un.S_addr=*(unsigned long *)(host->h_addr_list[0]) ;
          tmp=inet_ntoa(i_addr.S_un.S_std) ;
                 strcpy(mServer_ip, tmp) ;
                                }
/*--------------------------------------- Инициализация канала связи */

      mSocket_cli=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP) ;       /* Создание сокета */
   if(mSocket_cli==INVALID_SOCKET)  return(WSAGetLastError()) ;

#ifndef  UNIX
                     bOptVal=TRUE ;
                     bOptLen=sizeof(bOptVal) ;
 
       status=setsockopt(mSocket_cli, SOL_SOCKET, SO_REUSEADDR, (char *)&bOptVal, bOptLen) ;
     if(status) { 
                   status=WSAGetLastError() ;
                       ClientClose() ;
                    mSocket_cli=INVALID_SOCKET ;
                      return(status) ;   
                }
#endif

                i_addr.S_un.S_addr=inet_addr(mServer_ip) ;

             server_addr.sin_family=          AF_INET ;             /* Описание адреса сервера */
             server_addr.sin_port  =htons(mServer_port) ;
             server_addr.sin_addr  = i_addr.S_un.S_std ;

        status=connect(mSocket_cli, (PSOCKADDR)&server_addr,        /* Установления связи с сервером */
                                           sizeof(server_addr)) ;
     if(status) { 
                   status=WSAGetLastError() ;
                       ClientClose() ;
                    mSocket_cli=INVALID_SOCKET ;
                      return(status) ;   
                }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                Подготовка контактного порта сервера               */

  int  Tcp::InitServerPort(void)

{
  struct sockaddr_in  socket_port ;  /* Адрес порта */
                 int  status ;

/*-------------------------------------------------- Создание сокета */

      mSocket_srv=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP) ;
   if(mSocket_srv==INVALID_SOCKET)  return(WSAGetLastError()) ;

/*------------------------------- Установление входного адреса порта */

               memset(&socket_port, 0, sizeof(socket_port)) ;
                       socket_port.sin_family=  AF_INET ;
                       socket_port.sin_port  =htons(mServer_port) ;

      status=bind(mSocket_srv, (struct sockaddr *)&socket_port,
                                            sizeof(socket_port)) ;
   if(status)  return(WSAGetLastError()) ;

      status=listen(mSocket_srv, mSocket_listen_max) ;
   if(status==SOCKET_ERROR)   return(WSAGetLastError()) ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*               Ожидание событий на линиях Клиент-Сервер            */

  int  Tcp::Wait(SOCKET *connect_hdr)
{
         fd_set  read_list ;                   /* Списки анализируемых сокетов */
         fd_set  write_list ;
         fd_set  error_list ;
 struct timeval  pause ;                       /* Время ожидания */ 
         u_long  socket_status ;
            int  active ;                      /* Число активированных сокетов */
 	    int  i ;

#ifdef  UNIX
 	 SOCKET  max_socket ;		       /* Сокет с наибольшим номером */
            int  status ;
#endif

/*---------------------------------------------------- Инициализация */

                *connect_hdr=-1 ;

                   pause.tv_sec =    0 ; 
                   pause.tv_usec=10000 ; 

    for(i=0 ; i<mClients_max ; i++) {
                                       mClients[i].stage=0 ;
                                    } 
/*------------------------ Составление списков анализируемых сокетов */

                 FD_ZERO(&read_list) ;                              /* Иниц. списков */
                 FD_ZERO(&write_list) ;

                 FD_ZERO(&error_list) ;

         if(mSocket_srv>=0)
                  FD_SET(mSocket_srv, &read_list) ;                 /* Вкл. контактного сокета в список */

       for(i=0 ; i<mClients_max ; i++)                              /* Вкл.сокетов клиентов */
         if(mClients[i].use_flag) { 
                        FD_SET(mClients[i].socket, &read_list) ;

          if(mClients[i].send_data_done<
             mClients[i].send_data_size ||
             mClients[i].close_flag       )
                        FD_SET(mClients[i].socket, &write_list) ;
                                  } 
/*---------------------------------------- Ожидание события на линии */

#ifdef  UNIX
                           max_socket=mSocket_srv ;

     for(i=0 ; i<mClients_max ; i++)
       if(mClients[i].use_flag         &&
	  mClients[i].socket>max_socket  ) max_socket=mClients[i].socket ;

   if(max_socket>=0) {

     do {
          active=select(max_socket+1, 
                          &read_list, &write_list, &error_list, &pause) ;

         if(active!=SOCKET_ERROR) break ;

            status=WSAGetLastError() ;
         if(status!=EINTR)  break ;

        }  while(1) ;

     if(active==SOCKET_ERROR)  return(WSAGetLastError()) ;
 
                     }
   else              {
			active=0 ;
                     }

#else

      active=select(1, &read_list, &write_list, &error_list, &pause) ;
   if(active==SOCKET_ERROR)  return(WSAGetLastError()) ;

#endif

/*-------------------------------- Проверка событий обработки данных */

/*----------------------------- Проверка сокетов, ожидающих закрытия */

      for(i=0 ; i<mClients_max ; i++)                               /* Закрываем освобожденные сокеты */
        if(         mClients[i].use_flag            &&
                    mClients[i].close_flag          &&
           FD_ISSET(mClients[i].socket, &write_list)  ) {

		     closesocket( mClients[i].socket) ;
				  mClients[i].use_flag=0 ;

           iClientResourcesClear(&mClients[i]) ;

                   vCloseConnect(&mClients[i]) ;

                                    active-- ;
						        }

      for(i=0 ; i<mClients_max ; i++)                               /* Закрываем сокеты, в которых произошла ошибка */
        if(         mClients[i].use_flag            &&
                    mClients[i].close_flag          &&
					mClients[i].error               &&
           FD_ISSET(mClients[i].socket, &read_list)  ) {

		     closesocket( mClients[i].socket) ;
				  mClients[i].use_flag=0 ;

           iClientResourcesClear(&mClients[i]) ;

                   vCloseConnect(&mClients[i]) ;

                                    active-- ;
						        }
								
		if(active<=0)  return(0) ;

/*---------------------------- Проверка подсоединения нового Клиента */

   if(mSocket_srv>=0)
    if(FD_ISSET(mSocket_srv, &read_list)) {
			                    *connect_hdr=mSocket_srv ;
                                          }
/*------------------ Проверка наличия активности/готовности Клиентов */

      for(i=0 ; i<mClients_max ; i++)
       if(mClients[i].use_flag) {
/*- - - - - - - - - - -  Наличие потока от Клиента/разрыв с Клиентом */ 
        if(FD_ISSET(mClients[i].socket, &read_list)) {
                                                        socket_status=0 ;
             ioctlsocket(mClients[i].socket, FIONREAD, &socket_status) ;
         if(socket_status==0) {
                                mClients[i].error     =_CONNECT_DROPPED ;
                                mClients[i].close_flag=   1 ;
                                       continue ;
                              }

                        mClients[i].stage=_RECEIVE_STAGE ;
                                                     }
/*- - - - - - - - - - - - - - - - Готовность Клиента к приему данных */ 
        if(FD_ISSET(mClients[i].socket, &write_list)) {

//                       mClients[i].send_flag=1 ;
                                                      }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                  Регистрация нового клиента                       */

  int  Tcp::AddClient(void)

{
           Tcp_client *clients_prv ;     /* Список клиентов */
                  int  clients_max_prv ; 
   struct sockaddr_in  socket_client ;   /* Адрес сокета клиента */
      struct in_addr_  ip_client ;     
               SOCKET  socket_act ;      /* Контактный сокет */
                  int  client ;          /* Номер клиента в списке */
            socklen_t  cnt ;

/*-------------------------- Опр.свободной записи в таблице клиентов */

      for(client=0 ; client<mClients_max ; client++)
              if(!mClients[client].use_flag)  break ;

                          clients_prv    =mClients ;
                          clients_max_prv=mClients_max ;

   if(client==mClients_max) {

        mClients_max+=10 ;
        mClients     =(Tcp_client *)
                        realloc(mClients, mClients_max*sizeof(*mClients)) ;

     if(mClients==NULL) {
                          mClients    =clients_prv ;
                          mClients_max=clients_max_prv ;
                       }
     else              {
                         memset(&mClients[client], 0, 
                                  (mClients_max-client)*sizeof(*mClients)) ;
                       } 
                            }
/*------------------------------------------ Открытие сокета клиента */

                memset(&socket_client, 0, sizeof(socket_client)) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - -  В режиме Демона */
   if(mDemon) {
                   socket_act=mSocket_cli ;
              }
/*- - - - - - - - - - - - - - - - - - - В режиме постоянного Сервера */
   else       {
                      cnt=sizeof(socket_client) ;

         socket_act=accept(mSocket_srv, (struct sockaddr *)&socket_client, &cnt) ;
      if(socket_act==INVALID_SOCKET)  return(WSAGetLastError()) ;
              }
/*-------------------------- Обработка переполнения таблицы клиентов */

   if(client==mClients_max) {
                                Close(socket_act, 0) ;
                                   return(-1) ;
                            }
/*---------------------------------------------- Регистрация клиента */

     memset(&mClients[client], 0, sizeof(mClients[client])) ;
	
             mClients[client].use_flag=   1  ;
             mClients[client].socket  =socket_act ;

                  ip_client.S_un.S_std=socket_client.sin_addr ;

     sprintf(mClients[client].ip, "%03d.%03d.%03d.%03d",
                                  (unsigned int)ip_client.S_un.S_un_b.s_b1,
                                  (unsigned int)ip_client.S_un.S_un_b.s_b2,
                                  (unsigned int)ip_client.S_un.S_un_b.s_b3,
                                  (unsigned int)ip_client.S_un.S_un_b.s_b4 ) ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*								     */
/*                        Закрытие сокета                            */

  int  Tcp::ClientClose()
{
  int  status ;

	status=Close(mSocket_cli, 1) ;

                     mSocket_cli=0 ;

  return(status) ;
}

  int  Tcp::Close(SOCKET  socket_hdr, int  wait_flag)
{
         fd_set  read_list ;        /* Списки анализируемых сокетов */
         fd_set  write_list ;
         fd_set  error_list ;
 struct timeval  pause ;            /* Время ожидания */ 
            int  active ;
            int  status ;

/*--------------------------------- Формирование списков мониторинга */

                FD_ZERO(&read_list) ;                               /* Иниц. списков */
                FD_ZERO(&write_list) ;
                FD_ZERO(&error_list) ;

                 FD_SET(socket_hdr, &read_list) ;                   /* Вкл. рабочего сокета в список */
                 FD_SET(socket_hdr, &write_list) ;
                 FD_SET(socket_hdr, &error_list) ;

/*------------------------------------- Ожидание освобождения сокета */

#ifndef UNIX
                            pause.tv_sec =300 ; 
                            pause.tv_usec=  0 ; 

  if(wait_flag) {
      active=select(1, &read_list, &write_list, &error_list, &pause) ;
   if(active==SOCKET_ERROR)  return(WSAGetLastError()) ;
                }

#endif

/*-------------------------------------------------- Закрытие сокета */

      status=closesocket(socket_hdr) ;
   if(status==SOCKET_ERROR)  return(WSAGetLastError()) ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/*********************************************************************/
/*								     */
/*		       Передача данных                               */

  int  Tcp::ClientSend(std::string *message)

{
  char  size_msg[64] ;
   int  cnt ;


        sprintf(size_msg, "%07d", (int)message->size()) ;

       cnt=iSend(mSocket_cli, size_msg, 8, _WAIT_RECV) ;            /* Передаем размер посылки */
    if(cnt==SOCKET_ERROR)  return(WSAGetLastError()) ;              /* Если системная ошибка... */

       cnt=iSend(mSocket_cli, (char *)message->data(),              /* Передаем данные */
                                      message->size(), 
                                           _WAIT_RECV ) ;
    if(cnt==SOCKET_ERROR)  return(WSAGetLastError()) ;              /* Если системная ошибка... */

   return(0) ;
}


/*********************************************************************/
/*								     */
/*		       Прием данных                                  */

  int  Tcp::ClientReceive(std::string *message)

{
   static  char *buff ;
            int  size ;
            int  cnt ;
           char *end ;

/*---------------------------------------------- Размещение ресурсов */

    if(buff==NULL) {

          buff=(char *)calloc(1, _SOCKET_BLOCK_SIZE) ;

                   }
/*----------------------------------------------------- Прием данных */

         memset(buff, 0, _SOCKET_BLOCK_SIZE) ;

       cnt=iReceive(mSocket_cli, buff, 8, _WAIT_RECV) ;             /* Принимаем размер посылки */
    if(cnt==SOCKET_ERROR)   return(WSAGetLastError()) ;             /* Если системная ошибка... */
    if(cnt!=    8       ) {                                         /* Если неполные данные... */    
                             mError=_SYS_MSG_CORRUPT ;
                                 return(-1) ;                
                          } 

       size=strtoul(buff, &end, 10) ;                               /* Декодируем размер посылки */
    if(*end!=   0              ) {                                  /* Если некорректные данные... */
                                     mError=_SYS_MSG_CORRUPT ;
                                              return(-1) ;                 
                                 } 
    if(size>=_SOCKET_BLOCK_SIZE) {                                  /* Если слишком длинная посылка... */
                                     mError=_SYS_MSG_OVERFLOW ;
                                              return(-1) ;                 
                                 } 

       cnt=iReceive(mSocket_cli, buff, size, _WAIT_RECV) ;          /* Принимаем данные */
    if(cnt==SOCKET_ERROR)   return(WSAGetLastError()) ;             /* Если системная ошибка... */
    if(cnt!=  size      ) {                                         /* Если неполные данные... */
                               mError=_SYS_MSG_CORRUPT ;
                                        return(-1) ;                 
                          } 
/*-------------------------------------- Упаковка данных в сообщение */

        message->assign(buff, size) ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/*********************************************************************/
/*								     */
/*	         Передача блока данных по сокету                     */

  int  Tcp::iSend(SOCKET  socket,
                         char *data,  int  data_size, int  wait_flag)
{
         fd_set  read_list ;        /* Списки анализируемых сокетов */
         fd_set  write_list ;       /* Списки анализируемых сокетов */
 struct timeval  pause ;            /* Время ожидания */ 
         u_long  socket_status ;
            int  active ;
            int  cnt_all ;
            int  cnt ;
           char *data_prv ;         /* Сохранение начального статуса данных */
            int  size_prv ;

/*------------------------------------------------------- Подготовка */

                               data_prv=data ;
                               size_prv=data_size ;

        if(wait_flag==0) {
                            pause.tv_sec =   0 ; 
                            pause.tv_usec=1000 ; 
                         }
        else             {
                            pause.tv_sec =1000 ; 
                            pause.tv_usec=   0 ; 
                         }
/*---------------------------------------------------- Цикл передачи */

                   cnt_all=0 ;

    do {
              if(vExternal(socket))  break ;

/*---------------------------------------------- Ожидание готовности */

                     FD_ZERO(&read_list) ;                          /* Иниц. списков */
                     FD_ZERO(&write_list) ;
                      FD_SET(socket, &read_list) ;                  /* Вкл. контактного сокета в список */
                      FD_SET(socket, &write_list) ;                 /* Вкл. контактного сокета в список */

#ifdef  UNIX
            active=select(socket+1, &read_list,                     /* Ожидание ввода */
                                    &write_list, NULL, &pause) ;      
#else
            active=select(0, &read_list,                            /* Ожидание ввода */
                             &write_list, NULL, &pause) ;      
#endif

         if(active==SOCKET_ERROR) {
                                     cnt_all=SOCKET_ERROR ;
                                        break ;
                                  }  
         if(active==         0  )       break ;

/*------------------------------------------- Обработка обрыва связи */
       if(FD_ISSET(socket, &read_list)) {

                                            socket_status=0 ;
             ioctlsocket(socket, FIONREAD, &socket_status) ;
         if(socket_status==0) {
                                 cnt_all=SOCKET_ERROR ;
                                     break ;
                              }
                                        } 
/*-------------------------------------------------- Передача данных */

       if(FD_ISSET(socket, &write_list)) {

            cnt=send(socket, data, data_size, 0) ;  
         if(cnt==SOCKET_ERROR) {                                    /* Если системная ошибка... */
                                  cnt_all=SOCKET_ERROR ;
                                    break ;
                               }

            data     +=cnt ;
            data_size-=cnt ;
             cnt_all +=cnt ;
                                         }
       else                              {
                                             break ;
                                         }
/*---------------------------------------------------- Цикл передачи */

       } while(data_size) ;

/*-------------------------------------------------------------------*/

  return(cnt_all) ;
}


/*********************************************************************/
/*								     */
/*	            Прием блока данных по сокету                     */

  int  Tcp::iReceive(SOCKET  socket,
                       char *data,  int  data_size, int  wait_flag)
{
         fd_set  read_list ;        /* Списки анализируемых сокетов */
 struct timeval  pause ;            /* Время ожидания */ 
         u_long  socket_status ;
            int  active ;
            int  cnt_all ;
            int  cnt ;
            int  i ;

/*------------------------------------------------------- Подготовка */

        if(wait_flag==0) {
                                pause.tv_sec =   0 ; 
                                pause.tv_usec=1000 ; 
                         }
        else             {

            if(mWait_answer>0)  pause.tv_sec =mWait_answer ; 
            else                pause.tv_sec =    1000 ;
                                pause.tv_usec=      0 ; 
                         }
/*------------------------------------------------------ Цикл приема */

                   cnt_all=0 ;

    do {
         if(vExternal(socket))  return(0) ;

/*-------------------------------------------------- Ожидание данных */

                     FD_ZERO(&read_list) ;                          /* Иниц. списков */
                      FD_SET(socket, &read_list) ;                  /* Вкл. контактного сокета в список */

#ifdef  UNIX
            active=select(socket+1, &read_list,                     /* Ожидание ввода */
                                     NULL, NULL, &pause) ;      
#else
            active=select(0, &read_list, NULL, NULL, &pause) ;      /* Ожидание ввода */
#endif

         if(active==SOCKET_ERROR)   return(SOCKET_ERROR) ;
         if(active==         0  ) {
                                     return(cnt_all) ; 
                                  }

                                            socket_status=0 ;

          for(i=0 ; i<5 && socket_status==0 ; i++) {                /* Учитываем возможную паузу     */
            if(i)  sleep(1) ;                                       /*   на "сборку" сетевых пакетов */
             ioctlsocket(socket, FIONREAD, &socket_status) ;
                                                   }   

         if(socket_status==0)  return(SOCKET_ERROR) ;

/*----------------------------------------------------- Прием данных */

            cnt=recv(socket, data, data_size, 0) ;  
         if(cnt==SOCKET_ERROR)  return(SOCKET_ERROR) ;              /* Если системная ошибка... */

            data     +=cnt ;      
            data_size-=cnt ;
             cnt_all +=cnt ;

       if(mReceiveCallback!=NULL)                                   /* Если задана внешняя функция управления приемом - */
         if(mReceiveCallback(data-cnt_all, cnt_all))  break ;       /*   - проверяем ей, все ли данные получены         */

/*------------------------------------------------------ Цикл приема */

       } while(data_size) ; 

/*-------------------------------------------------------------------*/

  return(cnt_all) ;
}


/*********************************************************************/
/*								     */
/*	       Подготовка списка заданий для приема данных           */

  int  Tcp::iClientPaketsPrepare(Tcp_client *client)
{

//#ifdef UNIX
  std::string *tmp ;
//#endif

/*------------------------------------------------ Размещение памяти */

   if(client->job==NULL) {

             client->job=(Tcp_job *)calloc(1, sizeof(Tcp_job)) ;

//#ifdef UNIX
                                               tmp=new std::string ;
       memcpy(&(client->job->packet.data_in ), tmp, sizeof(*tmp)) ;
                                               tmp=new std::string ;
       memcpy(&(client->job->packet.data_out), tmp, sizeof(*tmp)) ;
//#endif
                         }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*								     */
/*	           Очистка списка пакетов                            */

  int  Tcp::iClientPaketsClear(Tcp_client *client)
{

  return(0) ;    
}


/*********************************************************************/
/*								     */
/*	              Очистка ресурсов клиента                       */

  int  Tcp::iClientResourcesClear(Tcp_client *client)
{
   
/*----------------------------------------- Список заданий обработки */

                   free(client->job) ;

/*------------------------------------------- Буфера приема-передачи */

                   free(client->recv_data) ;
                   free(client->send_data) ;

/*---------------------------------------------------- Общая очистка */

                memset(client, 0, sizeof(*client)) ;

/*-------------------------------------------------------------------*/

  return(0) ;    
}


/*********************************************************************/
/*								     */
/*                  Отработка внешнего управления                    */

  int  Tcp::vExternal(SOCKET  socket) 

{
   return(0) ;
}


/*********************************************************************/
/*								     */
/*	           Ведение лога обмена                               */

  int  Tcp::vMessagesLog( Tcp_client *client, 
                         std::string *message, 
                                char *module  )
{
   return(0) ;
}

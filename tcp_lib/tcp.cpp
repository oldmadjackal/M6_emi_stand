/*********************************************************************/
/*                                                                   */
/*              ���������� ������ ����������� �� TCP                 */
/*                                                                   */
/*********************************************************************/

/*********************************************************************/
/*								     */
/*   �������� ������ ������-������ �������� ��������� �������:       */
/*    - 8 ����� - ������ ������������ ������ (� ��������� �������,   */
/*                   ������� ������������� 0)                        */
/*    - ������                                                       */
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
/*		       ����������� ������			     */

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
/*		        ���������� ������			     */

    Tcp::~Tcp(void)

{
        Free() ;
}


/*********************************************************************/
/*								     */
/*		       ������������ ��������                         */

  void  Tcp::Free(void)

{
}


/*********************************************************************/
/*								     */
/*                          ������                                   */

  int  Tcp::ServerProcess(void)

{
           SOCKET  connect ;
              int  status ;
              int  i ;

/*------------------------------------------------------- ���������� */

            status=InitServerPort() ;
         if(status)  return(status) ;

/*-------------------------------------------- �������� ������� ���� */

    do {
/*- - - - - - - - - - - - - - �������� � ��������� ��������� ������� */
            status=vExternControl() ;                               /* ������������ ������� ���������� */
         if(status)  break ;                                        /* ���� ������� �� ������� */

            status=Wait(&connect) ;                                 /* �������� ���������� */
         if(status)  break ;

                                mTime_check= time(NULL) ;           /* ��������� ����������� ��������� ����� */
         if(mTime_extrn!=NULL) *mTime_extrn=mTime_check ;
/*- - - - - - - - - - - - - - - - - - - ������������� ������ ������� */
         if(connect==mSocket_srv)  AddClient() ;                    /* ���� ������� ���������� ���� - */
                                                                    /*     ���������� ������ �������  */
/*- - - - - - - - - - - - - - - - - - - - - - - - ��������� �������� */
      for(i=0 ; i<mClients_max ; i++) {

        if(mClients[i].use_flag==0)  continue ;
        
        if(mClients[i].stage==_RECEIVE_STAGE) { 
                                     SrvRecvClient(&mClients[i]) ;  /* ��������� �������� ������ */
                                     SrvExtrClient(&mClients[i]) ;  
                                              }
        if(mClients[i].stage==_PROCESS_STAGE) { 
                                     SrvProcClient(&mClients[i]) ;  /* ���������� ������ */
                                              }
        if(mClients[i].stage==   _SEND_STAGE) { 
                                     SrvPrepClient(&mClients[i]) ;  /* ��������� ��������� ������ */
                                     SrvSendClient(&mClients[i]) ;
                                              }

                                      }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
       } while(1) ;

/*------------------------------------------------------- ���������� */

//     for(i=0 ; i<clients_max ; i++)                               /* �������� ������ �������� */
//       if(clients[i].socket!=NULL)  Close(clients[i].socket, 1) ;

                                      Close(mSocket_srv, 0) ;       /* �������� ����������� ����� ������� */

/*-------------------------------------------------------------------*/

   return(status) ;
}


/*********************************************************************/
/*								     */
/*                     ������ - UNIX-�����                           */

  int  Tcp::DemonProcess(void)

{
           SOCKET  connect ;
              int  status ;

/*------------------------------------------------------- ���������� */

                        mDemon= 1 ;
                   mSocket_srv=-1 ;
                   mSocket_cli= 0 ;

/*-------------------------------------------- �������� ������� ���� */

            status=AddClient() ;                                    /* ���������� ������ �������  */
         if(status)  exit(-1) ;

    do {
/*- - - - - - - - - - - - - - �������� � ��������� ��������� ������� */
            status=vExternControl() ;                               /* ������������ ������� ���������� */
         if(status)  break ;                                        /* ���� ������� �� ������� */

            status=Wait(&connect) ;                                 /* �������� ���������� */
         if(status)  break ;

                                mTime_check= time(NULL) ;           /* ��������� ����������� ��������� ����� */
         if(mTime_extrn!=NULL) *mTime_extrn=mTime_check ;
/*- - - - - - - - - - - - - - - - - - - - - - - - ��������� �������� */
        if(mClients[0].use_flag==0)  break ;
        
        if(mClients[0].stage==_RECEIVE_STAGE) { 
                                     SrvRecvClient(&mClients[0]) ;  /* ��������� �������� ������ */
                                     SrvExtrClient(&mClients[0]) ;  
                                              }
        if(mClients[0].stage==_PROCESS_STAGE) { 
                                     SrvProcClient(&mClients[0]) ;  /* ���������� ������ */
                                              }
        if(mClients[0].stage==   _SEND_STAGE) { 
                                     SrvPrepClient(&mClients[0]) ;  /* ��������� ��������� ������ */
                                     SrvSendClient(&mClients[0]) ;
                                              }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
       } while(1) ;

/*------------------------------------------------------- ���������� */

            Close(mSocket_cli, 0) ;

/*-------------------------------------------------------------------*/

   return(status) ;
}


/*********************************************************************/
/*								     */
/*      	  ����� ������ �� ������ �������  		     */

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

/*------------------------------------------------- ������� �������� */

   if(client->stage!=_RECEIVE_STAGE)  return(0) ;

/*-------------------------------------- ����������� ������� ������� */

   if(DATA_SIZE==0) {
/*- - - - - - - - - - - - - - - - - - - - - -  ����� ������� ������� */
             memset(buff, 0, sizeof(buff)) ;

    if(DATA_DEFF)  memcpy(buff, DATA, DATA_DEFF) ;

       cnt=iReceive(client->socket, buff+DATA_DEFF,                 /* ��������� ������ ������� */
                                       8-DATA_DEFF,
                                      _NOWAIT_RECV) ;
    if(cnt==SOCKET_ERROR ||                                         /* ���� ��������� ������     */
       cnt!=8-DATA_DEFF    ) {                                      /*    ��� �������� ������... */
                               client->error     =_SYS_MSG_CORRUPT ;      
                               client->close_flag=  1 ;
                                        DATA_DEFF=  0 ;
                                    return(-1) ;  
                             } 

       cnt+=DATA_DEFF ; 
            DATA_DEFF=0 ;

       size=strtoul(buff, &end, 10) ;                               /* ���������� ������ ������� */
    if(*end!=0) {                                                   /* ���� ������������ ������... */
                   client->error     =_SYS_MSG_CORRUPT ; 
                   client->close_flag=  1 ;
                        return(-1) ;
                }
/*- - - - - - - - - - - - - - - - - - - ��������� ������ ��� ������� */
    if(DATA_MAX<=size) {
         DATA_MAX=size+1024 ;
         DATA    =(char *)realloc(DATA, DATA_MAX) ;
      if(DATA==NULL) {
                         client->error     =_MEMORY_OVERFLOW ;
                         client->close_flag=  1 ;
                                     return(-1) ;
                     }
                       }
/*- - - - - - - - - - - - - - - - - - - - - - -  ���������� � ������ */
                           DATA_SIZE=size ;
                           DATA_DONE=  0 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                    }
/*---------------------------------------------------- ����� ������� */

   if(DATA_SIZE!=DATA_DONE) {

      size=DATA_SIZE-DATA_DONE ;                                    /* ���.������ ���������� ����� ������� */
       cnt=iReceive(client->socket,                                 /* ��������� ������� */
                     DATA+DATA_DONE, size, _NOWAIT_RECV) ;

    if(cnt==SOCKET_ERROR) {                                         /* ���� ��������� ������... */
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
/*      	��������������� ��������� ������� ������             */

  int  Tcp::SrvExtrClient(Tcp_client *client)

{
     int  status ;

#define   DATA        client->recv_data
#define   DATA_SIZE   client->recv_data_size
#define   DATA_DONE   client->recv_data_done
#define   DATA_MAX    client->recv_data_max

/*------------------------------------------------- ������� �������� */

     if(client->stage!=_RECEIVE_STAGE)  return(0) ;

     if( DATA_DONE!=DATA_SIZE)  return(0) ;

/*-------------------------------------------- ����� �������� ������ */

         status=iClientPaketsPrepare(client) ;                      /* ���������� ������ � ������ */
      if(status) {
                     client->close_flag=1 ;
                         return(-1) ;
                 }

         client->job->packet.data_in.append(DATA, DATA_SIZE) ;      /* ������� ������ � ����� */

         client->stage=_PROCESS_STAGE ;

/*--------------------------------------------------- ������� ������ */

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
/*                ��������� ������ �� ������ �������                 */

  int  Tcp::SrvProcClient(Tcp_client *client)

{
   int  status ;

/*------------------------------------------------- ������� �������� */

     if(client->stage!=_PROCESS_STAGE)  return(0) ;

/*---------------------------------------- ����� ��������� ��������� */

                            client->job->done_flag=  0 ;
        status=vProcessData(client->job, client) ;
    if(!status)             client->job->done_flag=  1 ;

/*-------------------------------------------------------------------*/

        client->stage=_SEND_STAGE ;

  return(0) ;
}


/*********************************************************************/
/*								     */
/*           ���������� �������� ������ �� ������ �������            */

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

/*------------------------------------------------- ������� �������� */

     if(client->stage!=_SEND_STAGE)  return(0) ;
     if(    DATA_SIZE!= DATA_DONE )  return(0) ;

/*--------------------------------------- ���������� ������ �������� */

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
/*----------------------------------------- ���������� ������ ������ */

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
/*                 �������� ������ �� ������ �������                 */

  int  Tcp::SrvSendClient(Tcp_client *client)

{
  int  size ;
  int  cnt ;

#define   DATA        client->send_data
#define   DATA_SIZE   client->send_data_size
#define   DATA_DONE   client->send_data_done
#define   DATA_MAX    client->send_data_max

/*------------------------------------------------- ������� �������� */

     if(client->stage!=_SEND_STAGE)  return(0) ;
     if(    DATA_SIZE==   0       )  return(0) ;

/*------------------------------------------------- �������� ������� */

   if(DATA_SIZE!=DATA_DONE) {

      size=DATA_SIZE-DATA_DONE ;                                    /* ���.������ ������������ ����� ������� */
       cnt=iSend(client->socket,                                    /* �������� ������� */
                    DATA+DATA_DONE, size, _NOWAIT_RECV) ;

    if(cnt==SOCKET_ERROR) {                                         /* ���� ��������� ������... */
                               client->error     =_CONNECT_DROPPED ;      
                               client->close_flag=  1 ;
                                    return(-1) ;  
                          } 

                                DATA_DONE+=cnt ;
                            }
/*---------------------------------------- ������� ���������� ������ */

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
/*                   ��������� ����� � ��������                      */
/*                 ��� ������������ ������ ��������                  */

  int  Tcp::Client(Tcp_packet *packet)

{
  std::string  message ;
          int  status ;

/*-------------------------------------------- ���������� � �������� */

           status=LinkToServer() ;
        if(status)  return(status) ;

/*-------------------------------------------------- �������� ������ */

            status=ClientSend(&packet->data_out) ;
         if(status)  return(status) ;       
                                    
/*----------------------------------------------------- ����� ������ */

            status=ClientReceive(&packet->data_in) ;
         if(status)  return(status) ;
                                          
/*--------------------------------------- �������� ������ � �������� */

                   ClientClose() ;

/*-------------------------------------------------------------------*/

    return(0) ;
}


/*********************************************************************/
/*								     */
/*		     ���������� � ��������                           */

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

/*------------------------------------------------- ������� �������� */
    
    if(mSocket_cli!= NULL         &&
       mSocket_cli!=INVALID_SOCKET  )  return(0) ;

/*------------------------- ����������� �������� ������ �� ����� DNS */
/*- - - - - - - - - - - - - - - - - - - - - - - ��� ������ IP-������ */
   if(isdigit(mServer_name[0])) {
                                  strcpy(mServer_ip, mServer_name) ;
                                }
/*- - - - - - - - - - - - - - - - - - - - - - - - - -  ��� ����� DNS */
   else                         {

           host=gethostbyname(mServer_name) ;
        if(host==NULL)  return(WSAGetLastError()) ;

                        i_addr.S_un.S_addr=*(unsigned long *)(host->h_addr_list[0]) ;
          tmp=inet_ntoa(i_addr.S_un.S_std) ;
                 strcpy(mServer_ip, tmp) ;
                                }
/*--------------------------------------- ������������� ������ ����� */

      mSocket_cli=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP) ;       /* �������� ������ */
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

             server_addr.sin_family=          AF_INET ;             /* �������� ������ ������� */
             server_addr.sin_port  =htons(mServer_port) ;
             server_addr.sin_addr  = i_addr.S_un.S_std ;

        status=connect(mSocket_cli, (PSOCKADDR)&server_addr,        /* ������������ ����� � �������� */
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
/*                ���������� ����������� ����� �������               */

  int  Tcp::InitServerPort(void)

{
  struct sockaddr_in  socket_port ;  /* ����� ����� */
                 int  status ;

/*-------------------------------------------------- �������� ������ */

      mSocket_srv=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP) ;
   if(mSocket_srv==INVALID_SOCKET)  return(WSAGetLastError()) ;

/*------------------------------- ������������ �������� ������ ����� */

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
/*               �������� ������� �� ������ ������-������            */

  int  Tcp::Wait(SOCKET *connect_hdr)
{
         fd_set  read_list ;                   /* ������ ������������� ������� */
         fd_set  write_list ;
         fd_set  error_list ;
 struct timeval  pause ;                       /* ����� �������� */ 
         u_long  socket_status ;
            int  active ;                      /* ����� �������������� ������� */
 	    int  i ;

#ifdef  UNIX
 	 SOCKET  max_socket ;		       /* ����� � ���������� ������� */
            int  status ;
#endif

/*---------------------------------------------------- ������������� */

                *connect_hdr=-1 ;

                   pause.tv_sec =    0 ; 
                   pause.tv_usec=10000 ; 

    for(i=0 ; i<mClients_max ; i++) {
                                       mClients[i].stage=0 ;
                                    } 
/*------------------------ ����������� ������� ������������� ������� */

                 FD_ZERO(&read_list) ;                              /* ����. ������� */
                 FD_ZERO(&write_list) ;

                 FD_ZERO(&error_list) ;

         if(mSocket_srv>=0)
                  FD_SET(mSocket_srv, &read_list) ;                 /* ���. ����������� ������ � ������ */

       for(i=0 ; i<mClients_max ; i++)                              /* ���.������� �������� */
         if(mClients[i].use_flag) { 
                        FD_SET(mClients[i].socket, &read_list) ;

          if(mClients[i].send_data_done<
             mClients[i].send_data_size ||
             mClients[i].close_flag       )
                        FD_SET(mClients[i].socket, &write_list) ;
                                  } 
/*---------------------------------------- �������� ������� �� ����� */

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

/*-------------------------------- �������� ������� ��������� ������ */

/*----------------------------- �������� �������, ��������� �������� */

      for(i=0 ; i<mClients_max ; i++)                               /* ��������� ������������� ������ */
        if(         mClients[i].use_flag            &&
                    mClients[i].close_flag          &&
           FD_ISSET(mClients[i].socket, &write_list)  ) {

		     closesocket( mClients[i].socket) ;
				  mClients[i].use_flag=0 ;

           iClientResourcesClear(&mClients[i]) ;

                   vCloseConnect(&mClients[i]) ;

                                    active-- ;
						        }

      for(i=0 ; i<mClients_max ; i++)                               /* ��������� ������, � ������� ��������� ������ */
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

/*---------------------------- �������� ������������� ������ ������� */

   if(mSocket_srv>=0)
    if(FD_ISSET(mSocket_srv, &read_list)) {
			                    *connect_hdr=mSocket_srv ;
                                          }
/*------------------ �������� ������� ����������/���������� �������� */

      for(i=0 ; i<mClients_max ; i++)
       if(mClients[i].use_flag) {
/*- - - - - - - - - - -  ������� ������ �� �������/������ � �������� */ 
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
/*- - - - - - - - - - - - - - - - ���������� ������� � ������ ������ */ 
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
/*                  ����������� ������ �������                       */

  int  Tcp::AddClient(void)

{
           Tcp_client *clients_prv ;     /* ������ �������� */
                  int  clients_max_prv ; 
   struct sockaddr_in  socket_client ;   /* ����� ������ ������� */
      struct in_addr_  ip_client ;     
               SOCKET  socket_act ;      /* ���������� ����� */
                  int  client ;          /* ����� ������� � ������ */
            socklen_t  cnt ;

/*-------------------------- ���.��������� ������ � ������� �������� */

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
/*------------------------------------------ �������� ������ ������� */

                memset(&socket_client, 0, sizeof(socket_client)) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - -  � ������ ������ */
   if(mDemon) {
                   socket_act=mSocket_cli ;
              }
/*- - - - - - - - - - - - - - - - - - - � ������ ����������� ������� */
   else       {
                      cnt=sizeof(socket_client) ;

         socket_act=accept(mSocket_srv, (struct sockaddr *)&socket_client, &cnt) ;
      if(socket_act==INVALID_SOCKET)  return(WSAGetLastError()) ;
              }
/*-------------------------- ��������� ������������ ������� �������� */

   if(client==mClients_max) {
                                Close(socket_act, 0) ;
                                   return(-1) ;
                            }
/*---------------------------------------------- ����������� ������� */

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
/*                        �������� ������                            */

  int  Tcp::ClientClose()
{
  int  status ;

	status=Close(mSocket_cli, 1) ;

                     mSocket_cli=0 ;

  return(status) ;
}

  int  Tcp::Close(SOCKET  socket_hdr, int  wait_flag)
{
         fd_set  read_list ;        /* ������ ������������� ������� */
         fd_set  write_list ;
         fd_set  error_list ;
 struct timeval  pause ;            /* ����� �������� */ 
            int  active ;
            int  status ;

/*--------------------------------- ������������ ������� ����������� */

                FD_ZERO(&read_list) ;                               /* ����. ������� */
                FD_ZERO(&write_list) ;
                FD_ZERO(&error_list) ;

                 FD_SET(socket_hdr, &read_list) ;                   /* ���. �������� ������ � ������ */
                 FD_SET(socket_hdr, &write_list) ;
                 FD_SET(socket_hdr, &error_list) ;

/*------------------------------------- �������� ������������ ������ */

#ifndef UNIX
                            pause.tv_sec =300 ; 
                            pause.tv_usec=  0 ; 

  if(wait_flag) {
      active=select(1, &read_list, &write_list, &error_list, &pause) ;
   if(active==SOCKET_ERROR)  return(WSAGetLastError()) ;
                }

#endif

/*-------------------------------------------------- �������� ������ */

      status=closesocket(socket_hdr) ;
   if(status==SOCKET_ERROR)  return(WSAGetLastError()) ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/*********************************************************************/
/*								     */
/*		       �������� ������                               */

  int  Tcp::ClientSend(std::string *message)

{
  char  size_msg[64] ;
   int  cnt ;


        sprintf(size_msg, "%07d", (int)message->size()) ;

       cnt=iSend(mSocket_cli, size_msg, 8, _WAIT_RECV) ;            /* �������� ������ ������� */
    if(cnt==SOCKET_ERROR)  return(WSAGetLastError()) ;              /* ���� ��������� ������... */

       cnt=iSend(mSocket_cli, (char *)message->data(),              /* �������� ������ */
                                      message->size(), 
                                           _WAIT_RECV ) ;
    if(cnt==SOCKET_ERROR)  return(WSAGetLastError()) ;              /* ���� ��������� ������... */

   return(0) ;
}


/*********************************************************************/
/*								     */
/*		       ����� ������                                  */

  int  Tcp::ClientReceive(std::string *message)

{
   static  char *buff ;
            int  size ;
            int  cnt ;
           char *end ;

/*---------------------------------------------- ���������� �������� */

    if(buff==NULL) {

          buff=(char *)calloc(1, _SOCKET_BLOCK_SIZE) ;

                   }
/*----------------------------------------------------- ����� ������ */

         memset(buff, 0, _SOCKET_BLOCK_SIZE) ;

       cnt=iReceive(mSocket_cli, buff, 8, _WAIT_RECV) ;             /* ��������� ������ ������� */
    if(cnt==SOCKET_ERROR)   return(WSAGetLastError()) ;             /* ���� ��������� ������... */
    if(cnt!=    8       ) {                                         /* ���� �������� ������... */    
                             mError=_SYS_MSG_CORRUPT ;
                                 return(-1) ;                
                          } 

       size=strtoul(buff, &end, 10) ;                               /* ���������� ������ ������� */
    if(*end!=   0              ) {                                  /* ���� ������������ ������... */
                                     mError=_SYS_MSG_CORRUPT ;
                                              return(-1) ;                 
                                 } 
    if(size>=_SOCKET_BLOCK_SIZE) {                                  /* ���� ������� ������� �������... */
                                     mError=_SYS_MSG_OVERFLOW ;
                                              return(-1) ;                 
                                 } 

       cnt=iReceive(mSocket_cli, buff, size, _WAIT_RECV) ;          /* ��������� ������ */
    if(cnt==SOCKET_ERROR)   return(WSAGetLastError()) ;             /* ���� ��������� ������... */
    if(cnt!=  size      ) {                                         /* ���� �������� ������... */
                               mError=_SYS_MSG_CORRUPT ;
                                        return(-1) ;                 
                          } 
/*-------------------------------------- �������� ������ � ��������� */

        message->assign(buff, size) ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/*********************************************************************/
/*								     */
/*	         �������� ����� ������ �� ������                     */

  int  Tcp::iSend(SOCKET  socket,
                         char *data,  int  data_size, int  wait_flag)
{
         fd_set  read_list ;        /* ������ ������������� ������� */
         fd_set  write_list ;       /* ������ ������������� ������� */
 struct timeval  pause ;            /* ����� �������� */ 
         u_long  socket_status ;
            int  active ;
            int  cnt_all ;
            int  cnt ;
           char *data_prv ;         /* ���������� ���������� ������� ������ */
            int  size_prv ;

/*------------------------------------------------------- ���������� */

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
/*---------------------------------------------------- ���� �������� */

                   cnt_all=0 ;

    do {
              if(vExternal(socket))  break ;

/*---------------------------------------------- �������� ���������� */

                     FD_ZERO(&read_list) ;                          /* ����. ������� */
                     FD_ZERO(&write_list) ;
                      FD_SET(socket, &read_list) ;                  /* ���. ����������� ������ � ������ */
                      FD_SET(socket, &write_list) ;                 /* ���. ����������� ������ � ������ */

#ifdef  UNIX
            active=select(socket+1, &read_list,                     /* �������� ����� */
                                    &write_list, NULL, &pause) ;      
#else
            active=select(0, &read_list,                            /* �������� ����� */
                             &write_list, NULL, &pause) ;      
#endif

         if(active==SOCKET_ERROR) {
                                     cnt_all=SOCKET_ERROR ;
                                        break ;
                                  }  
         if(active==         0  )       break ;

/*------------------------------------------- ��������� ������ ����� */
       if(FD_ISSET(socket, &read_list)) {

                                            socket_status=0 ;
             ioctlsocket(socket, FIONREAD, &socket_status) ;
         if(socket_status==0) {
                                 cnt_all=SOCKET_ERROR ;
                                     break ;
                              }
                                        } 
/*-------------------------------------------------- �������� ������ */

       if(FD_ISSET(socket, &write_list)) {

            cnt=send(socket, data, data_size, 0) ;  
         if(cnt==SOCKET_ERROR) {                                    /* ���� ��������� ������... */
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
/*---------------------------------------------------- ���� �������� */

       } while(data_size) ;

/*-------------------------------------------------------------------*/

  return(cnt_all) ;
}


/*********************************************************************/
/*								     */
/*	            ����� ����� ������ �� ������                     */

  int  Tcp::iReceive(SOCKET  socket,
                       char *data,  int  data_size, int  wait_flag)
{
         fd_set  read_list ;        /* ������ ������������� ������� */
 struct timeval  pause ;            /* ����� �������� */ 
         u_long  socket_status ;
            int  active ;
            int  cnt_all ;
            int  cnt ;
            int  i ;

/*------------------------------------------------------- ���������� */

        if(wait_flag==0) {
                                pause.tv_sec =   0 ; 
                                pause.tv_usec=1000 ; 
                         }
        else             {

            if(mWait_answer>0)  pause.tv_sec =mWait_answer ; 
            else                pause.tv_sec =    1000 ;
                                pause.tv_usec=      0 ; 
                         }
/*------------------------------------------------------ ���� ������ */

                   cnt_all=0 ;

    do {
         if(vExternal(socket))  return(0) ;

/*-------------------------------------------------- �������� ������ */

                     FD_ZERO(&read_list) ;                          /* ����. ������� */
                      FD_SET(socket, &read_list) ;                  /* ���. ����������� ������ � ������ */

#ifdef  UNIX
            active=select(socket+1, &read_list,                     /* �������� ����� */
                                     NULL, NULL, &pause) ;      
#else
            active=select(0, &read_list, NULL, NULL, &pause) ;      /* �������� ����� */
#endif

         if(active==SOCKET_ERROR)   return(SOCKET_ERROR) ;
         if(active==         0  ) {
                                     return(cnt_all) ; 
                                  }

                                            socket_status=0 ;

          for(i=0 ; i<5 && socket_status==0 ; i++) {                /* ��������� ��������� �����     */
            if(i)  sleep(1) ;                                       /*   �� "������" ������� ������� */
             ioctlsocket(socket, FIONREAD, &socket_status) ;
                                                   }   

         if(socket_status==0)  return(SOCKET_ERROR) ;

/*----------------------------------------------------- ����� ������ */

            cnt=recv(socket, data, data_size, 0) ;  
         if(cnt==SOCKET_ERROR)  return(SOCKET_ERROR) ;              /* ���� ��������� ������... */

            data     +=cnt ;      
            data_size-=cnt ;
             cnt_all +=cnt ;

       if(mReceiveCallback!=NULL)                                   /* ���� ������ ������� ������� ���������� ������� - */
         if(mReceiveCallback(data-cnt_all, cnt_all))  break ;       /*   - ��������� ��, ��� �� ������ ��������         */

/*------------------------------------------------------ ���� ������ */

       } while(data_size) ; 

/*-------------------------------------------------------------------*/

  return(cnt_all) ;
}


/*********************************************************************/
/*								     */
/*	       ���������� ������ ������� ��� ������ ������           */

  int  Tcp::iClientPaketsPrepare(Tcp_client *client)
{

//#ifdef UNIX
  std::string *tmp ;
//#endif

/*------------------------------------------------ ���������� ������ */

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
/*	           ������� ������ �������                            */

  int  Tcp::iClientPaketsClear(Tcp_client *client)
{

  return(0) ;    
}


/*********************************************************************/
/*								     */
/*	              ������� �������� �������                       */

  int  Tcp::iClientResourcesClear(Tcp_client *client)
{
   
/*----------------------------------------- ������ ������� ��������� */

                   free(client->job) ;

/*------------------------------------------- ������ ������-�������� */

                   free(client->recv_data) ;
                   free(client->send_data) ;

/*---------------------------------------------------- ����� ������� */

                memset(client, 0, sizeof(*client)) ;

/*-------------------------------------------------------------------*/

  return(0) ;    
}


/*********************************************************************/
/*								     */
/*                  ��������� �������� ����������                    */

  int  Tcp::vExternal(SOCKET  socket) 

{
   return(0) ;
}


/*********************************************************************/
/*								     */
/*	           ������� ���� ������                               */

  int  Tcp::vMessagesLog( Tcp_client *client, 
                         std::string *message, 
                                char *module  )
{
   return(0) ;
}

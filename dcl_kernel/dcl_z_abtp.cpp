/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*          УТИЛИТЫ ДЛЯ РАБОТЫ С СЕТЕВЫМ ПРОТОКОЛОМ ABTP             */
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


#include "abtp_msg.h"
#include "abtp_exh.h"
#include "abtp_tcp.h"

#define   DCL_INSIDE
#include "dcl.h"


#pragma warning(disable : 4996)

/*------------------------------------------------- Общие переменные */


/*---------------------------------------------------- Внешние связи */

          extern double  dcl_errno ;       /* Системный указатель ошибок -> DCL_SLIB.CPP */
                            

/*********************************************************************/
/*                                                                   */
/*               Отправка сообщения SIGNAL                           */

   int  Lang_DCL::zAbtpSignal(char *address, char *sgn_name,
                                             char *sgn_value,
                                             char *sgn_info, 
                                              int  sgn_info_size)

{
#ifndef UNIX
             WORD  version ;
          WSADATA  winsock_data ;        /* Данные системы WINSOCK */
#endif
     ABTP_message  Message ;
         ABTP_tcp  Transport ;
      ABTP_packet  Packet ;
             char  ABTP_server[128] ;
              int  ABTP_port ;
             char *query ;
              int  query_size ;
              int  status ;
             char *end ;

/*--------------------------------------------- Формирование запроса */

        Message.Free() ;
        Message.Message_New     ("SIGNAL") ;
        Message.MessageField_Set("ACTION", "SET",      strlen("SET")+1) ;
        Message.MessageField_Set("NAME",    sgn_name,  strlen( sgn_name)+1) ;
        Message.MessageField_Set("VALUE",   sgn_value, strlen( sgn_value)+1) ;
        Message.MessageField_Set("INFO",    sgn_info,          sgn_info_size) ;
        Message.Close() ;

          query_size=Message.PacketGet(&query) ;                    /* Формирование пакета сообщения */
                      Packet.data_out.assign(query, query_size) ;
                     Message.Free() ;

/*-------------------------------------------- Подготовка транспорта */

           memset(ABTP_server, 0, sizeof(ABTP_server)) ;
          strncpy(ABTP_server, address, sizeof(ABTP_server)-1) ;

       end=strchr(ABTP_server, ':') ;
    if(end!=NULL) {                   *end=0 ;  
                     ABTP_port=strtoul(end+1, &end, 10) ;  }
    else             ABTP_port=22008 ;

            Transport.mServer_name=ABTP_server ;
            Transport.mServer_port=ABTP_port ;

/*------------------------------------------------- Передача запроса */

#ifndef UNIX
                        version=MAKEWORD(1, 1) ;
      status=WSAStartup(version, &winsock_data) ;                   /* Иниц. Win-Sockets */
   if(status) {
                sprintf(mError_details, "Socket - %d", WSAGetLastError()) ;
                    return(-1) ; 
              }
#endif

              Transport.ClientSecure(_ABTP_SECURE_XOR_MODRAND |
                                     _ABTP_SECURE_CLIENT_KEY   , 0, 0) ;
              Transport.ClientMltTrips(2) ;
       status=Transport.ClientMltExt(&Packet, 1) ;                  /* Обмен данными с ABTP-сервером */   
              Transport.ClientMltClose() ;

#ifndef UNIX
                             WSACleanup();
#endif

    if(Packet.data_in.size()==0) {                                  /* Проверка ошибки транспорта */
           sprintf(mError_details, "Server <%s> connection fail",  
                                                Transport.mServer_name) ;
                           return(-1) ;
                                 }
/*---------------------------------------------------- Анализ ответа */

              Message.PacketCopyFrom((char *)Packet.data_in.data(), /* Распаковка сообщеия */ 
                                             Packet.data_in.size() ) ;
              Message.Open() ;

       status=Message.Message_Find("ERROR") ;                       /* Проверка сообщения об ошибках */ 
    if(status==0) {

      Message.MessageField_Find ("TEXT") ;
                          memset( mError_details, 0, sizeof(mError_details)) ;
      Message.MessageField_Value( mError_details, sizeof(mError_details)-1) ;
                           return(-1) ;
                 }
/*-------------------------------------------------------------------*/

  return(0) ;
}



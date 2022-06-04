/*********************************************************************/
/*                                                                   */
/*              ���������� ������ ����������� �� TCP                 */
/*                                                                   */
/*********************************************************************/

#ifndef  _TCP_H_
#define  _TCP_H_

#ifdef  UNIX
typedef  int  SOCKET ;
#else
typedef  int  socklen_t ;
#endif

/*---------------------------------------------- ��������� ��������� */

#define      _SOCKET_BLOCK_SIZE        512000

/*----------------------------------------------- ����������� ������ */

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

/*------------------------------------------------- ����������� ���� */

 typedef  struct {                          /* �������� ������� */
                    int   use_flag ;           /* ���� ������������� */
                    int   close_flag ;         /* ���� �������� */
                   char   ip[32] ;             /* IP-����� �������� */
                 SOCKET   socket ;             /* ����� */
                    int   error ;              /* ����������� ������ */

                Tcp_job  *job ;                /* ������� */

                    int   stage ;              /* ���� ������ */
#define                    _RECEIVE_STAGE  1
#define                    _PROCESS_STAGE  2
#define                       _SEND_STAGE  3
#define                      _CLOSE_STAGE  4

                   char  *recv_data ;          /* ����� ������ */
                    int   recv_data_max ;
                    int   recv_data_size ;
                    int   recv_data_done ;
                    int   recv_data_deff ;     /* ����� ���� � ������ ������. �� ����������� ������ */

                   char  *send_data ;          /* ����� �������� */
                    int   send_data_max ;
                    int   send_data_size ;
                    int   send_data_done ;
                   char  *send_all ;           /* ����� ������������ ������ */
                    int   send_all_size ;
                    int   send_all_done ;

                 }  Tcp_client ;

/*--------------------------------------------- ����� TCP-���������� */

  class  Tcp : public Tcp_exchange {

           public:
                    int   mTime_check ;                    /* ��������� ����� */
                    int  *mTime_extrn ;

           public:
                    int   mDemon ;                         /* ���� ������ ������� */
                 SOCKET   mSocket_srv ;                    /* ���������� ����� ������� */
                 SOCKET   mSocket_cli ;                    /* ������� ����� ������� */

             Tcp_client  *mClients ;                       /* ������ �������� */
                    int   mClients_max ; 

           public:
                   char  *mServer_name ;                   /* ��� ������� */
                   char   mServer_ip[128] ;                /* IP-����� ������� */
                    int   mServer_port ;                   /* ���������� ���� ������� */

                    int   mSocket_listen_max ;             /* ������������ ����� ������� ������� */

                    int   mWait_answer ;                   /* ������������ ����� �������� ������ */

                    int (*mReceiveCallback)(char *, int) ; /* ������� ������� ���������� ������� */

                    int   mError ;                         /* ����������� ������ */
                    int   mStage ;                         /* ���� ����� ������� � ��������: */
#define                   _ABTP_CLIENT_CONNECT    1 
#define                   _ABTP_CLIENT_HANDSHAKE  2 
#define                   _ABTP_CLIENT_IDENTIFY   3 
#define                   _ABTP_CLIENT_DATA       4 
#define                   _ABTP_CLIENT_CLOSE      5 

           public:
                   void  Free           (void) ;                    /* ������������ �������� */

                    int  ServerProcess  (void) ;                    /* ��������� ������� */
                    int  DemonProcess   (void) ;                    /* ������ - UNIX-����� */
                    int  InitServerPort (void) ;                    /* ������: ���������� ����������� ����� ������� */
                    int  Wait           (SOCKET *) ;                /* ������: �������� ������� �� ������ ������-������ */
                    int  AddClient      (void) ;                    /* ������: ����������� ������ ������� */
                    int  SrvRecvClient  (Tcp_client *) ;            /* ������: ����� ������ �� ������ ������� */
                    int  SrvExtrClient  (Tcp_client *) ;            /* ������: �������� ������ �� ������ ������� */
                    int  SrvProcClient  (Tcp_client *) ;            /* ������: ��������� ������ �� ������ ������� */
                    int  SrvPrepClient  (Tcp_client *) ;            /* ������: ���������� �������� ������ �� ������ ������� */
                    int  SrvSendClient  (Tcp_client *) ;            /* ������: �������� ������ �� ������ ������� */

                    int  Client         (Tcp_packet *) ;

                    int  LinkToServer   (void) ;                    /* ������: ���������� � �������� */

                    int  ClientSend     (std::string *) ;           /* ������: �������� ����� ������ */
                    int  ClientReceive  (std::string *) ;           /* ������: ����� ����� ������ */
                    int  ClientClose    (void) ;                    /* ������: �������� ����� */

                    int  Close          (SOCKET, int) ;             /* �������� ������ � �������� */
                    int  Send           (SOCKET, char *, int) ;     /* �������� ����� ������ */                    
                    int  Receive        (SOCKET, char *, int) ;     /* ����� ����� ������ */                    

                    int  iSend          (SOCKET, char *,            /* �������� ����� ������ �� ������ */
                                                  int  , int) ;
                    int  iReceive       (SOCKET, char *,            /* ����� ����� ������ �� ������ */
                                                  int  , int) ;
#define                    _NOWAIT_RECV  0
#define                      _WAIT_RECV  1

                    int  iClientPaketsPrepare (Tcp_client *) ;      /* ���������� ������ ������� ��� ������ ������ */
                    int  iClientPaketsClear   (Tcp_client *) ;      /* ������� ������ ������� */
                    int  iClientResourcesClear(Tcp_client *) ;      /* ������� �������� ������� */

            virtual int  vExternal      (SOCKET) ;                  /* ��������� �������� ���������� */
            virtual int  vMessagesLog   (Tcp_client *,              /* ������� ���� ������ */
                                             std::string *, char *) ;

           public:
                         Tcp() ;                                    /* ����������� */
                        ~Tcp() ;                                    /* ���������� */
                                
                                         } ;

/*-------------------------------------------------------------------*/

#endif         // _ABTP_TCP_H_

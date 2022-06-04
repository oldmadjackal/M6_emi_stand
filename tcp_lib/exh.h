/*********************************************************************/
/*                                                                   */
/*              ���������� ������ ����������� �� TCP                 */
/*                                                                   */
/*********************************************************************/

#ifndef  _TCP_EXH_H_
#define  _TCP_EXH_H_

#include  <string>

/*----------------------------------------------------- ����� ������ */

 typedef  struct  {
            std::string  data_out ;                     /* ����� ��������� ������ */
            std::string  data_in ;                      /* ����� �������� ������ */
                    int  channel_send ;                 /* ����� �������� */
                    int  channel_recv ;                 /* ����� ������ */
                  } Tcp_packet ;

 typedef  struct {                         /* �������� ������� */
                   int  use_flag ;          /* ���� ������������� */
            Tcp_packet  packet ;            /* ����� ������ */
                   int  done_flag ;         /* ���� ���������� ��������� ������� */
                  void *thread ;            /* ������ �� ����� ��������� */
                  void *transit ;           /* ���������� ������ */
                 }  Tcp_job ;

/*----------------------------------------------- ������� ����� ���������� */

  class  Tcp_exchange {

           public:
                   char  mLog_file[1024] ;		 /* ���� � ���-����� */

           public:
                    int  Exchange      (Tcp_packet *, Tcp_packet *) ;     /* ����� ������-������ */

            virtual int  vCloseConnect (void *) ;                         /* �������� ������ ����� */
            virtual int  vProcessData  (Tcp_job *, void *) ;              /* ��������� ���������� ������ */
            virtual int  vExternControl(void) ;                           /* ���������� �������� ���������� */

		    int  iLog_file     (char *) ;			  /* ������ ���-����� */

           public:
                         Tcp_exchange () ;                               /* ����������� */
                        ~Tcp_exchange () ;                               /* ���������� */
                                
                       } ;

/*-------------------------------------------------------------------*/

#endif         // _ABTP_EXH_H_

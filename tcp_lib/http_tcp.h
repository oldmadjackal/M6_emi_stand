/*********************************************************************/
/*                                                                   */
/*              ���������� ������ ����������� �� TCP                 */
/*                                                                   */
/*********************************************************************/

#ifndef  _HTTP_TCP_H_
#define  _HTTP_TCP_H_


/*------------------------------------------------- ����������� ���� */

typedef struct {
                    unsigned long total[2] ;     /* ���������� ���� */
                    unsigned long state[4] ;     /* ������������� ���������� ���������� */
                    unsigned char buffer[64] ;   /* �������������� ������ */
               } HTTP_md5_control ;

/*-------------------------------------------- ����� HTTP-���������� */

  class  HTTP_tcp {

    public:
              char  *mURL ;                           /* URL ������� */
               int   mError ;                         /* ����������� ������ */

    public:
             void  Free         (void) ;                        /* ������������ �������� */

      static  int  Base64_incode(char *, char *, int) ;         /* ����������� � Base64 */
      static  int  Base64_decode(char *, char *) ;              /* ������������� �� Base64 */
      static  int  MD5          (char *, int, char *, char *) ; /* ���������� ���� MD5 */
                                                
      static  int  SOAP_exchange(char *, char *,                /* �������� SOAP-��������� �� HTTP */
                                         char *, int, char *) ;
      static  int  SOAP_exchange(char *, char *, char **, 
                                         char *, int, char *) ;

      static  int  iReceive_ctrl(char *, int) ;                 /* ������� ���������� ������� ��� HTTP */
      static void *iMD5_init    (void) ;                        /* ������������� �������� ���-������� */
      static void  iMD5_stream  (void *, char *, int) ;         /* ����� �������� ���-������� */
      static void  iMD5_close   (void *, char *) ;              /* ���������� �������� ���-������� */
      static void  iMD5_frame   (HTTP_md5_control *, void *) ;  /* ������� ����� ���-������� */
      static void  iMD5_format  (char *, char *, char *) ;      /* ������������ ��������� ������������� ���-������� */

           public:
                         HTTP_tcp() ;                            /* ����������� */
                        ~HTTP_tcp() ;                            /* ���������� */
                                
                                         } ;

/*-------------------------------------------------------------------*/

#endif         // _HTTP_TCP_H_

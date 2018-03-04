/********************************************************************/
/*								    */
/*                   ������ DCL-�����������        	            */
/*								    */
/********************************************************************/

#ifdef C_DCL_EXPORTS
#define C_DCL_API __declspec(dllexport)
#else
#define C_DCL_API __declspec(dllimport)
#endif


/*---------------------------------------------- ��������� ��������� */

/*----------------------------------------------- �������� ��������� */

 typedef struct {  
                    int  dummy ;
 
                }  RSS_Calc_Dcl_context ;

/*------------------------- �������� ������ ������������ ����������� */

  class C_DCL_API RSS_Calc_Dcl : public RSS_Kernel {

    public:
     virtual         int  vCalculate   (char *, char *,         /* ��������� ��������� */                                       
                                        struct RSS_Parameter *,
                                        struct RSS_Parameter *,
                                        double *,
                                        void **, char *) ;

    public:

     virtual         int  vGetParameter(char *, char *) ;       /* �������� �������� */
     virtual        void  vStart       (void) ;                 /* ��������� �������� */

                    void  iDebug       (char *, char *, int) ;  /* ���������� ������ � ���� */ 
                    void  iDebug       (char *, char *) ;

	                  RSS_Calc_Dcl() ;                     /* ����������� */
	                 ~RSS_Calc_Dcl() ;                     /* ���������� */
                                                   } ;

/*---------------------------------------------------- ��������� �/� */

  void  RSS_Calc_Dcl_text_norm(char *) ; /* ������������ ������ ��������� */
  char *RSS_Calc_Dcl_get_text (void) ;   /* ��������� ���������� ������ ������ ����������� */
  char *RSS_Calc_Dcl_error    (int) ;    /* ������ ������ ������ �� ���� */
  char *RSS_Calc_Dcl_warning  (int) ;    /* ������ ������ �������������� �� ���� */
  void  RSS_Calc_Dcl_debug    (int) ;    /* ��������� ������� ����������� */


#ifndef  RSS_FEATURE_H 

#define  RSS_FEATURE_H 

#include <string>

#ifdef RSS_FEATURE_EXPORTS
#define RSS_FEATURE_API __declspec(dllexport)
#else
#define RSS_FEATURE_API __declspec(dllimport)
#endif

/*------------------------------- �������� ������ "�������� �������" */

    class RSS_FEATURE_API RSS_Feature {

    public:
                       char  Type[128] ;      /* ��� �������� */
           class RSS_Object *Object ;         /* ������ �������������� */ 

    public:
               virtual void  vReadSave     (char *, std::string *,    /* ������� ������ �� ������ */
                                                           char * ) ;
               virtual void  vWriteSave    (std::string *) ;          /* �������� ������ � ������ */  
               virtual void  vFree         (void) ;                   /* ���������� ������� */  
               virtual void  vGetInfo      (std::string *) ;          /* ���������� � �������� */  

               virtual  int  vParameter    (char *, char *, char *) ; /* ������ � ����������� */  

               virtual  int  vResetCheck   (void *) ;                 /* ����� ��������� �������� ������������������ �������� */  
               virtual  int  vPreCheck     (void *) ;                 /* ���������� � �������� ������������������ �������� */  
               virtual  int  vCheck        (void *,                   /* �������� ������������������ �������� */  
                                            class RSS_Objects_List *) ;
               virtual  int  vShow         (void *) ;                 /* ����������� ������ �� �������� */ 
               virtual void  vBodyAdd      (char *) ;                 /* �������� ���� */
               virtual void  vBodyDelete   (char *) ;                 /* ������� ���� */
               virtual void  vBodyBasePoint(char *,                   /* ������� ������� ����� ���� */
                                             double, double, double) ;
               virtual void  vBodyAngles   (char *,                   /* ������� ���������� ���� */
                                             double, double, double) ;
               virtual void  vBodyMatrix   (char *, double[4][4]) ;   /* ������� ������� ��������� ���� */
               virtual void  vBodyShifts   (char *,                   /* ������� �������� ��������� �  */
                                             double, double, double,  /*    ���������� ����            */
                                             double, double, double ) ;
               virtual void  vBodyPars     (char *,                   /* ������� ������ ���������� */
                                             struct RSS_Parameter *) ;

			     RSS_Feature() ;                       /* ����������� */
			    ~RSS_Feature() ;                       /* ���������� */
                                      } ;

/*-------------------------------------------------------------------*/

#endif        // RSS_FEATURE_H 

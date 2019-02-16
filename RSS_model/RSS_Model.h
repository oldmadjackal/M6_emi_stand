
#ifndef  RSS_MODEL_H 

#define  RSS_MODEL_H 

#include <string>

#ifdef RSS_MODEL_EXPORTS
#define RSS_MODEL_API __declspec(dllexport)
#else
#define RSS_MODEL_API __declspec(dllimport)
#endif

/*---------------------------------------------- ��������� ��������� */

#define    _MODEL_PARS_MAX   20
#define    _MODEL_SECT_MAX    8

/*--------------------------------------------- ���������� ��������� */

  typedef  struct {
                     char  text[32] ;        /* ������� ��������� */
                     char  value[32] ;       /* �������� ��������� */
                  } RSS_Model_parameter ;

  typedef  struct {
                            char  title[32] ;       /* ������������ ������ */
                     std::string *decl ;            /* ������ ������ */
                  } RSS_Model_section ;

  typedef  struct {
                           char  name[32] ;                   /* �������� ������� */
                           char  lib_path[FILENAME_MAX] ;     /* ���� � ������� ���������� */
                           char  path[FILENAME_MAX] ;         /* ���� � ����� */
                           char  object[32] ;                 /* ��� ������� */
                           char  model[32] ;                  /* �������� ������ */
                           char  picture[FILENAME_MAX] ;      /* ���� �������� */
            RSS_Model_parameter  pars[_MODEL_PARS_MAX] ;      /* ��������� */
              RSS_Model_section  sections[_MODEL_SECT_MAX] ;  /* ������ ����������� ������ */
                  } RSS_Model_data ;

/*---------------------------------------------------- ��������� �/� */

/* RSS_model.cpp */
 RSS_MODEL_API int  RSS_Model_list    (char *, char *,     /* ������������ ������ ������ ������� � ������� */
                                        int  , char * ) ;
 RSS_MODEL_API int  RSS_Model_ReadPars(RSS_Model_data *) ; /* ���������� ���������� ������ */ 
 RSS_MODEL_API int  RSS_Model_ReadSect(RSS_Model_data *) ; /* ���������� ������ �������� ������ */ 
 RSS_MODEL_API int  RSS_Model_Picture (RSS_Model_data *,   /* ���������� �������� ������ */ 
                                        HBITMAP *, RECT *) ;

#endif        // RSS_MODEL_H

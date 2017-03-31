
#ifndef  RSS_MODEL_H 

#define  RSS_MODEL_H 

#include <string>

#ifdef RSS_MODEL_EXPORTS
#define RSS_MODEL_API __declspec(dllexport)
#else
#define RSS_MODEL_API __declspec(dllimport)
#endif

/*---------------------------------------------- Параметры генерации */

#define    _MODEL_PARS_MAX    8
#define    _MODEL_SECT_MAX    8

/*--------------------------------------------- Транзитные структуры */

  typedef  struct {
                     char  text[32] ;        /* Подпись параметра */
                     char  value[32] ;       /* Значение параметра */
                  } RSS_Model_parameter ;

  typedef  struct {
                            char  title[32] ;       /* Наименование секции */
                     std::string *decl ;            /* Данные секции */
                  } RSS_Model_section ;

  typedef  struct {
                           char  name[32] ;                   /* Название обьекта */
                           char  lib_path[FILENAME_MAX] ;     /* Путь к разделу библиотеки */
                           char  path[FILENAME_MAX] ;         /* Путь к файлу */
                           char  object[32] ;                 /* Вид объекта */
                           char  model[32] ;                  /* Название модели */
                           char  picture[FILENAME_MAX] ;      /* Файл картинки */
            RSS_Model_parameter  pars[_MODEL_PARS_MAX] ;      /* Параметры */
              RSS_Model_section  sections[_MODEL_SECT_MAX] ;  /* Данные специальных секций */
                  } RSS_Model_data ;

/*---------------------------------------------------- Прототипы п/п */

/* RSS_model.cpp */
 RSS_MODEL_API int  RSS_Model_list    (char *, char *,     /* Формирование списка файлов модулей в разделе */
                                        int  , char * ) ;
 RSS_MODEL_API int  RSS_Model_ReadPars(RSS_Model_data *) ; /* Считывание параметров модели */ 
 RSS_MODEL_API int  RSS_Model_ReadSect(RSS_Model_data *) ; /* Считывание секций описаний модели */ 
 RSS_MODEL_API int  RSS_Model_Picture (RSS_Model_data *,   /* Считывание картинки модели */ 
                                        HBITMAP *, RECT *) ;

#endif        // RSS_MODEL_H

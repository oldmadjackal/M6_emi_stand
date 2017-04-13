
#ifndef  RSS_OBJECT_H 

#define  RSS_OBJECT_H 

#include "..\RSS_Feature\RSS_Feature.h"

#include <string>

#ifdef RSS_OBJECT_EXPORTS
#define RSS_OBJECT_API __declspec(dllexport)
#else
#define RSS_OBJECT_API __declspec(dllimport)
#endif

/*---------------------------------------------------- Коды статусов */

#define   _RSS_OBJECT_UNKNOWN_ERROR      -999    /* Неизвестная ошибка */

/*------------------------------------- Коды видов целевых положений */

#define   _RSS_ABSOLUTE_TARGET  0   /* Задается абсолютными координатами */ 

/*----------------------------------- Константы преобразования углов */

#define   _GRD_TO_RAD   0.017453
#define   _RAD_TO_GRD  57.296
#define   _PI           3.1415926

/*---------------------------------------- Описание элемента "Точка" */

   typedef struct {
                     double  x ;
                     double  y ;
                     double  z ;

                     double  azim ;
                     double  elev ;
                     double  roll ;

                        int  mark ;

                  } RSS_Point ;

/*--------------------------------------- Описание элемента "Вектор" */

   typedef struct {
                     double  x ;
                     double  y ;
                     double  z ;

                        int  mark ;

                  } RSS_Vector ;

/*------------------------- Описание элемента "Управляемый параметр" */

   typedef struct {
                     char  link[32] ;            /* Мнемо-ссылка на параметр */
                     char  section_name[32] ;    /* Название секции параметров */
                     char  parameter_name[32] ;  /* Название параметра */
                     char  type[8] ;             /* Тип параметра */  
                     void *value ;               /* Устанавливаемое значение */
                  } RSS_ControlPar ;

/*------------------------------------ Интерфейс передачи контекстов */

  class RSS_OBJECT_API RSS_Transit {

    public:
                      char  action[1024] ;
                RSS_Object *object ;

    public:
             virtual   int  vExecute(void) ;             /* Исполнение действия */
                                             
    public:
                            RSS_Transit() ;              /* Конструктор */
                           ~RSS_Transit() ;              /* Деструктор */

                                   } ; 

/*----------------------------------------- Описание класса "Объект" */

  class RSS_OBJECT_API RSS_Object {

       public:
                       char   Name[128] ;      /* Имя обьекта */
                       char   Type[128] ;      /* Тип обьекта */
                       char   Decl[1024] ;     /* Описание обьекта */

                     double  x_base ;          /* Координаты базовой точки */
                     double  y_base ;
                     double  z_base ;

                     double  a_azim ;          /* Углы ориентации */
                     double  a_elev ;
                     double  a_roll ;

                     double  x_velocity ;      /* Вектор скорости */
                     double  y_velocity ;
                     double  z_velocity ;

       struct RSS_Parameter  *Parameters ;     /* Список параметров */
                        int   Parameters_cnt ;

                RSS_Feature **Features ;       /* Список свойств */
                        int   Features_cnt ;

                RSS_Transit  *Context ;        /* Интерфейс передачи контекстов */

                        int   ErrorEnable ;    /* Флаг выдачи сообщений об ошибках */

       public:

   virtual             void  vErrorMessage   (int) ;                /* Вкл./Выкл. сообщений об ошибках */

   virtual             void  vReadSave       (std::string *) ;      /* Считать данные из строки */
   virtual             void  vWriteSave      (std::string *) ;      /* Записать данные в строку */
   virtual             void  vFree           (void) ;               /* Освободить ресурсы */

   virtual             void  vFormDecl       (void) ;               /* Формировать описание */

   virtual              int  vListControlPars(RSS_ControlPar *) ;   /* Получить список параметров управления */
   virtual              int  vSetControlPar  (RSS_ControlPar *) ;   /* Установить значение параметра управления */

   virtual              int  vGetPosition    (RSS_Point *) ;        /* Работа с положением объекта */
   virtual             void  vSetPosition    (RSS_Point *) ;
   virtual              int  vGetVelocity    (RSS_Vector *) ;       /* Получение вектора скорости */

   virtual              int  vSpecial        (char *, void *) ;     /* Специальные действия */

   virtual              int  vCalculateStart (void) ;               /* Подготовка расчета изменения состояния */
   virtual              int  vCalculate      (double, double) ;     /* Расчет изменения состояния */
   virtual              int  vCalculateShow  (void) ;               /* Отображение результата расчета изменения состояния */

   virtual              int  vCheckFeatures  (void *) ;             /* Проверить корректность свойств */
   virtual             void  vSetFeature     (RSS_Feature *) ;      /* Работа со свойствами */
   virtual             void  vGetFeature     (RSS_Feature *) ;

                        int  iAngleInCheck   (double,               /* Проверка попадания угла в диапазон */
                                              double, double) ;
                        int  iReplaceText    (char *, char *,       /* Замена текстового фрагмента в строке */
                                                      char *, int) ;

			     RSS_Object      () ;                   /* Конструктор */
			    ~RSS_Object      () ;                   /* Деструктор */

                                  } ;
/*-------------------------------------------------------------------*/

#endif  // RSS_OBJECT_H


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

#define   _RSS_OBJECT_NOTARGETED           -1    /* Объект неизменяем */
#define   _RSS_OBJECT_UNATTAINABLE_POINT   -2    /* Недостижимая точка */
#define   _RSS_OBJECT_DEGREES_LIMIT        -3    /* Нарушение диапазона значений в степени подвижности */
#define   _RSS_OBJECT_LOW_ACTIVE           -4    /* Число степеней подвижности меньше 6 */
#define   _RSS_OBJECT_MATCH_ACTIVE         -5    /* Число степеней подвижности больше 6 */
#define   _RSS_OBJECT_BAD_SCHEME           -6    /* Некорректная схема */
#define   _RSS_OBJECT_COLLISION_EXTERNAL   -7    /* Столкновение тел разных объектов сцены */
#define   _RSS_OBJECT_COLLISION_INTERNAL   -8    /* Столкновение тел одного из объектов сцены */

#define   _RSS_OBJECT_UNKNOWN_ERROR      -999    /* Неизвестная ошибка */

/*------------------------------------- Коды видов целевых положений */

#define   _RSS_ABSOLUTE_TARGET  0   /* Задается абсолютными координатами */ 
#define     _RSS_JOINTS_TARGET  1   /* Задается значениями в сочленениях */
#define     _RSS_OBJECT_TARGET  2   /* Задается целевой точкой другого объекта */

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

/*----------------------------------- Описание элемента "Сочленение" */

   typedef struct {
                     double   value ;
                     double   value_min ;
                     double   value_max ;
                        int   bounded ;
                     double   spec ;
                        int   type ;
#define                        _L_TYPE    0
#define                        _A_TYPE    1
                        int   fixed ;

                       char   name[64] ;     /* Наименование */
        struct  RSS_Context **contexts ;     /* Список контекстных данных модулей */
                  } RSS_Joint ;

/*-------------------- Описание элемента "Морфологический компонент" */

   typedef struct {
                     char  object[64] ;   /* Название обьекта */
                     char  link[64] ;     /* Название звена */
                     char  body[64] ;     /* Название тела */
                     void *ptr ;          /* Ссылка на компонент */
                  } RSS_Morphology ;

/*------------------------- Описание элемента "Управляемый параметр" */

   typedef struct {
                     char  link[32] ;            /* Мнемо-ссылка на параметр */
                     char  section_name[32] ;    /* Название секции параметров */
                     char  parameter_name[32] ;  /* Название параметра */
                     char  type[8] ;             /* Тип параметра */  
                     void *value ;               /* Устанавливаемое значение */
                  } RSS_ControlPar ;

/*----------------------------------------- Описание класса "Объект" */

  class RSS_OBJECT_API RSS_Object {

       public:
                       char   Name[128] ;      /* Имя обьекта */
                       char   Type[128] ;      /* Тип обьекта */
                       char   Decl[1024] ;     /* Описание обьекта */

       struct RSS_Parameter  *Parameters ;     /* Список параметров */
                        int   Parameters_cnt ;

                RSS_Feature **Features ;       /* Список свойств */
                        int   Features_cnt ;

             RSS_Morphology  *Morphology ;     /* Список морфологических элементов */
                        int   Morphology_cnt ;

                        int   ErrorEnable ;    /* Флаг выдачи сообщений об ошибках */

       public:

   virtual             void  vErrorMessage   (int) ;                /* Вкл./Выкл. сообщений об ошибках */

   virtual             void  vReadSave       (std::string *) ;      /* Считать данные из строки */
   virtual             void  vWriteSave      (std::string *) ;      /* Записать данные в строку */
   virtual             void  vFree           (void) ;               /* Освободить ресурсы */

   virtual             void  vFormDecl       (void) ;               /* Формировать описание */

   virtual             void  vEditStructure  (void) ;               /* Редактировать структуру */
   virtual             void  vAddMorphology  (RSS_Morphology *) ;   /* Добавить элемент морфологии */
   virtual             void  vEditMorphology (RSS_Morphology *) ;   /* Исправить элемент морфологии */

   virtual              int  vListControlPars(RSS_ControlPar *) ;   /* Получить список параметров управления */
   virtual              int  vSetControlPar  (RSS_ControlPar *) ;   /* Установить значение параметра управления */

   virtual              int  vGetBasePoint   (RSS_Point *) ;        /* Работа с базовой точкой */
   virtual             void  vSetBasePoint   (RSS_Point *) ;
   virtual              int  vGetTargetPoint (RSS_Point *) ;        /* Работа с целевой точкой */
   virtual             void  vSetTargetPoint (RSS_Point *, int) ;
   virtual              int  vGetJoints      (RSS_Joint *) ;        /* Работа с сочленениями */
   virtual             void  vSetJoints      (RSS_Joint *, int) ;
   virtual class RSS_Object *vGetSlice       (void) ;               /* Работа со срезом состояния */
   virtual             void  vSetSlice       (class RSS_Object *) ;
   virtual              int  vGetAmbiguity   (void) ;               /* Работа с неопределенностями схемы */
   virtual             void  vSetAmbiguity   (char *) ;
   virtual              int  vSolveByTarget  (void) ;               /* Разрешить структуру по целевой точке */ 
   virtual              int  vSolveByJoints  (void) ;               /* Разрешить структуру по сочленениям */

   virtual              int  vGetTarget      (char *,               /* Выдать координаты 'целевой' точки */
                                              RSS_Point *) ;

   virtual              int  vSpecial        (char *, void *) ;     /* Специальные действия */

   virtual              int  vCheckFeatures  (void *) ;             /* Проверить корректность свойств */
   virtual             void  vSetFeature     (RSS_Feature *) ;      /* Работа со свойствами */
   virtual             void  vGetFeature     (RSS_Feature *) ;

			     RSS_Object      () ;                   /* Конструктор */
			    ~RSS_Object      () ;                   /* Деструктор */

                                  } ;
/*-------------------------------------------------------------------*/

#endif  // RSS_OBJECT_H

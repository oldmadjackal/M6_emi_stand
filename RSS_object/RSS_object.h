
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
                     double  x ;                  /* Положение */
                     double  y ;
                     double  z ;

                     double  azim ;               /* Углы ориентации */
                     double  elev ;
                     double  roll ;

                     double   x_velocity ;        /* Вектор скорости */
                     double   y_velocity ;
                     double   z_velocity ;

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
                      char  details[1024] ;
                RSS_Object *object ;
               RSS_Feature *feature ;

    public:
             virtual   int  vExecute(void) ;             /* Исполнение действия */
                                             
    public:
                            RSS_Transit() ;              /* Конструктор */
                           ~RSS_Transit() ;              /* Деструктор */

                                   } ; 

/*-------------------------------- Описание класса "Список объектов" */

  struct RSS_Objects_List_Elem {
                                 class RSS_Object *object ;
                                             char  relation[128] ;
                               }  ;

  class RSS_OBJECT_API RSS_Objects_List {

       public:
                RSS_Objects_List_Elem *List ;
                                  int   List_cnt ;
                                  int   List_max ;

       public:

                        int  Add  (class RSS_Object *, char *) ;    /* Добавление в список */
                       void  Clear(void) ;                          /* Очистка списка */

			     RSS_Objects_List() ;                   /* Конструктор */
			    ~RSS_Objects_List() ;                   /* Деструктор */

                                  } ;
/*----------------------------------------- Описание класса "Объект" */

  class RSS_OBJECT_API RSS_Object {

       public:
                       char   Name[128] ;         /* Имя обьекта */
                       char   Type[128] ;         /* Тип обьекта */
                       char   Decl[1024] ;        /* Описание обьекта */

                        int   battle_state ;      /* Род объекта в бою */
#define                        _ACTIVE_STATE  1
#define                         _SPAWN_STATE  2

                        int   land_state ;        /* Принадлежность объекта ландшафту */

                  RSS_Point   state ;             /* Текущее состояние */
                  RSS_Point   state_0 ;           /* Исходное состояние прирасчете модели */
                  RSS_Point   state_stack ;       /* Сохраненное состояние */ 

                  RSS_Point   direct_target ;     /* Целевая точка программного управления */
                       char   direct_select[16] ;

       struct RSS_Parameter  *Parameters ;        /* Список параметров */
                        int   Parameters_cnt ;

                RSS_Feature **Features ;          /* Список свойств */
                        int   Features_cnt ;

     class RSS_Objects_List   Units ;             /* Список объектов-составных частей */

                RSS_Transit  *Context ;           /* Интерфейс передачи контекстов */
          class  RSS_Kernel  *Module ;            /* Программный модуль объекта */

                       char   owner[128] ;        /* Объект-носитель */
                 RSS_Object  *o_owner ;
                       char   target[128] ;       /* Объект-цель */
                 RSS_Object  *o_target ;

                        int   state_idx ;         /* Идентификатор состояния */

                        int   ErrorEnable ;       /* Флаг выдачи сообщений об ошибках */

                        int   CalculateExt_use ;  /* При моделировании объект использует методы vCalculateExt1 и vCalculateExt2, */
                                                  /*  в противном случае - vCalculate                                            */
       public:

   virtual class RSS_Object *vCopy           (char *) ;                /* Копировать объект */
   virtual             void  vPush           (void)  ;                 /* Сохранить состояние объекта */
   virtual             void  vPop            (void)  ;                 /* Восстановить состояние объекта */

   virtual             void  vErrorMessage   (int) ;                   /* Вкл./Выкл. сообщений об ошибках */

   virtual             void  vReadSave       (std::string *) ;         /* Считать данные из строки */
   virtual             void  vWriteSave      (std::string *) ;         /* Записать данные в строку */
   virtual             void  vFree           (void) ;                  /* Освободить ресурсы */

   virtual             void  vFormDecl       (void) ;                  /* Формировать описание */

   virtual              int  vListControlPars(RSS_ControlPar *) ;      /* Получить список параметров управления */
   virtual              int  vSetControlPar  (RSS_ControlPar *) ;      /* Установить значение параметра управления */

   virtual              int  vGetPosition    (RSS_Point *) ;           /* Работа с положением объекта */
   virtual             void  vSetPosition    (RSS_Point *) ;
   virtual              int  vGetVelocity    (RSS_Vector *) ;          /* Получение вектора скорости */

   virtual              int  vSpecial        (char *, void *) ;        /* Специальные действия */

   virtual              int  vCalculateStart (double) ;                /* Подготовка расчета изменения состояния */
   virtual              int  vCalculate      (double, double,          /* Расчет изменения состояния */
                                                      char *, int) ;
   virtual              int  vCalculateExt1  (double, double,          /* Расчет изменения состояния, внешний вызов 1 */
                                                      char *, int) ;
   virtual              int  vCalculateExt2  (double, double,          /* Расчет изменения состояния, внешний вызов 2 */
                                                      char *, int) ;
   virtual              int  vCalculateDirect(RSS_Point *, char *);    /* Задание целевого состояния */
   virtual              int  vCalculateShow  (double, double) ;        /* Отображение результата расчета изменения состояния */
   virtual              int  vEvent          (char *, double,          /* Обработка событий */
                                                      char *, int) ;

   virtual              int  vResetFeatures  (void *) ;                /* Сброс контекста проверки свойств */
   virtual              int  vPrepareFeatures(void *) ;                /* Подготовить свойства к проверке корректности */
   virtual              int  vCheckFeatures  (void *,                  /* Проверить корректность свойств */
                                              RSS_Objects_List *) ;
   virtual             void  vSetFeature     (RSS_Feature *) ;         /* Работа со свойствами */
   virtual             void  vGetFeature     (RSS_Feature *) ;

                        int  iAngleInCheck   (double,                  /* Проверка попадания угла в диапазон */
                                              double, double) ;
                        int  iReplaceText    (char *, char *,          /* Замена текстового фрагмента в строке */
                                                      char *, int) ;

			     RSS_Object      () ;                      /* Конструктор */
			    ~RSS_Object      () ;                      /* Деструктор */

                                  } ;
/*-------------------------------------------------------------------*/

#endif  // RSS_OBJECT_H

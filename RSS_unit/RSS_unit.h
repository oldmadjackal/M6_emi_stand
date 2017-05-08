
#ifndef  RSS_UNIT_H 

#define  RSS_UNIT_H 

#include "..\RSS_Object\RSS_Object.h"

#include <string>

#ifdef RSS_UNIT_EXPORTS
#define RSS_UNIT_API __declspec(dllexport)
#else
#define RSS_UNIT_API __declspec(dllimport)
#endif

/*-------------------------------------- Описание класса "Компонент" */

  class RSS_UNIT_API RSS_Unit : public RSS_Object {

       public:

           class RSS_Object *Owner ;           /* Объект - носитель */

       public:

			     RSS_Unit        () ;                   /* Конструктор */
			    ~RSS_Unit        () ;                   /* Деструктор */

                                  } ;
/*-------------------------------------------------------------------*/

#endif  // RSS_UNIT_H

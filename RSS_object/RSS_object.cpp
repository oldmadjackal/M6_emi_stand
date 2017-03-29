
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <malloc.h>


#include "..\RSS_feature\RSS_feature.h"
#include "RSS_object.h"

#pragma warning(disable : 4996)


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}


/*********************************************************************/
/*								     */
/*		      Компоненты класса "ОБЪЕКТ"	             */
/*								     */
/*********************************************************************/

/*********************************************************************/
/*								     */
/*		       Конструктор класса "ОБЪЕКТ"      	     */

     RSS_Object::RSS_Object(void)

{
     Parameters    =NULL ;
     Parameters_cnt=  0 ;
       Features    =NULL ;
       Features_cnt=  0 ;
     Morphology    =NULL ;
     Morphology_cnt=  0 ;

        ErrorEnable=1 ;
}


/*********************************************************************/
/*								     */
/*		        Деструктор класса "ОБЪЕКТ"      	     */

    RSS_Object::~RSS_Object(void)

{
}


/********************************************************************/
/*								    */
/*		  Вкл./Выкл. сообщений об ошибках		    */

    void  RSS_Object::vErrorMessage(int  on_off)

{
         ErrorEnable=on_off ;
}


/********************************************************************/
/*								    */
/*		        Считать данные из строки		    */

    void  RSS_Object::vReadSave(std::string *text)

{
}


/********************************************************************/
/*								    */
/*		        Записать данные в строку		    */

    void  RSS_Object::vWriteSave(std::string *text)

{
     *text="" ;
}


/********************************************************************/
/*								    */
/*		      Освободить ресурсы        		    */

    void  RSS_Object::vFree(void)

{
}


/********************************************************************/
/*								    */
/*		      Формировать описание      		    */

    void  RSS_Object::vFormDecl(void)

{
   sprintf(Decl, "%s  %s", Name, Type) ;
}


/********************************************************************/
/*								    */
/*		      Работа с базовыми точками   		    */
/*								    */
/*   Return: число базовых точек                                    */

     int  RSS_Object::vGetBasePoint(RSS_Point *points)

{
    return(0) ;
}

    void  RSS_Object::vSetBasePoint(RSS_Point *points)

{
}


/********************************************************************/
/*								    */
/*		      Работа с целевыми точками   		    */
/*								    */
/*   Return: число целевых точек                                    */

     int  RSS_Object::vGetTargetPoint(RSS_Point *points)

{
    return(0) ;
}

    void  RSS_Object::vSetTargetPoint(RSS_Point *points, int  points_cnt)

{
}


/********************************************************************/
/*								    */
/*		      Работа с сочленениями       		    */
/*								    */
/*   Return: число сочленений                                       */

     int  RSS_Object::vGetJoints(RSS_Joint *joints)

{
    return(0) ;
}

    void  RSS_Object::vSetJoints(RSS_Joint *joints, int  joints_cnt)

{
}


/********************************************************************/
/*								    */
/*                 Работа с неопределенностями схемы                */

    int  RSS_Object::vGetAmbiguity(void)
{
   return(0) ;
}

   void  RSS_Object::vSetAmbiguity(char *spec)
{
}


/********************************************************************/
/*								    */
/*           Разрешить конфигурацию объекта по целевой точке        */

     int  RSS_Object::vSolveByTarget(void)

{
    return(_RSS_OBJECT_NOTARGETED) ;
}


/********************************************************************/
/*								    */
/*           Разрешить конфигурацию объекта по сочленениям          */

     int  RSS_Object::vSolveByJoints(void)

{
    return(0) ;
}


/********************************************************************/
/*								    */
/*     Выдать координаты целевой точки, привязанной к объекту       */

     int  RSS_Object::vGetTarget(char *part, RSS_Point *target)

{
    memset(target, 0, sizeof(*target)) ;

    return(0) ;
}


/********************************************************************/
/*								    */
/*		Проверка непротиворечивости свойств                 */

     int  RSS_Object::vCheckFeatures(void *data)

{
  int  status ;
  int  i ;


   for(i=0 ; i<this->Features_cnt ; i++) {
           status=this->Features[i]->vCheck(data) ;
        if(status)  break ;
                                         }
   return(status) ;
}


/********************************************************************/
/*								    */
/*		      Работа со свойствами                          */

    void  RSS_Object::vSetFeature(RSS_Feature *feature)

{
}

    void  RSS_Object::vGetFeature(RSS_Feature *feature)

{
}


/********************************************************************/
/*								    */
/*                  Работа со срезом состояния                      */

    RSS_Object *RSS_Object::vGetSlice(void)

{
   return(NULL) ;
}


    void  RSS_Object::vSetSlice(RSS_Object *slice)

{
}


/********************************************************************/
/*								    */
/*		      Задание структуры объекта   		    */

    void  RSS_Object::vEditStructure(void)
{
}


/********************************************************************/
/*								    */
/*                        Специальные действия                      */

     int  RSS_Object::vSpecial(char *oper, void *data)
{
  return(-1) ;
}


/********************************************************************/
/*								    */
/*                  Добавить элемент морфологии                     */

    void  RSS_Object::vAddMorphology(RSS_Morphology *elem)
{
#define      M     Morphology[Morphology_cnt-1] 

        Morphology_cnt++ ;
        Morphology=(RSS_Morphology *)
                     realloc(Morphology, Morphology_cnt*            /* Довыделяем список элементов */
                                           sizeof(*Morphology)) ;
     if(Morphology==NULL) {
                              Morphology_cnt=0 ;
                                  return ;
                          }

                         memset(&M, 0, sizeof(M)) ;                 /* Инициализация описания */

                         strcpy(M.object, this->Name) ;
   if(elem->link!=NULL)  strcpy(M.link,   elem->link) ;
   if(elem->body!=NULL)  strcpy(M.body,   elem->body) ;
        
#undef   M
}


/********************************************************************/
/*								    */
/*                 Исправить элемент морфологии                     */

    void  RSS_Object::vEditMorphology(RSS_Morphology *elem)
{
   int  i ;


    for(i=0 ; i<Morphology_cnt ; i++)
      if(Morphology[i].ptr==elem->ptr)  break ;
                               
      if(i>=Morphology_cnt) {
                              vAddMorphology(elem) ;
                            }
      else                  {
                if(elem->link!=NULL)  strcpy(Morphology[i].link, elem->link) ;
                if(elem->body!=NULL)  strcpy(Morphology[i].body, elem->body) ;
                            }
}


/********************************************************************/
/*								    */
/*                Получить список параметров управления             */

   int  RSS_Object::vListControlPars(RSS_ControlPar *list)

{
   return(0) ;
}


/********************************************************************/
/*								    */
/*               Установить значение параметра управления           */

   int  RSS_Object::vSetControlPar(RSS_ControlPar *par)   

{
   return(-1) ;
}

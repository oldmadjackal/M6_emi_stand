
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <malloc.h>


#include "..\RSS_feature\RSS_feature.h"
#include "RSS_object.h"

#pragma warning(disable : 4267)
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
/*	         Компоненты класса "ТРАНЗИТ КОНТЕКСТА"	             */
/*								     */
/*********************************************************************/

/*********************************************************************/
/*								     */
/*	       Конструктор класса "ТРАНЗИТ КОНТЕКСТА"      	     */

     RSS_Transit::RSS_Transit(void)

{
   memset(action, 0, sizeof(action)) ;

          object=NULL ;
}


/*********************************************************************/
/*								     */
/*	        Деструктор класса "ТРАНЗИТ КОНТЕКСТА"      	     */

    RSS_Transit::~RSS_Transit(void)

{
}


/********************************************************************/
/*								    */
/*	              Исполнение действия                           */

    int  RSS_Transit::vExecute(void)

{
   return(0) ;
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

       battle_state= 0 ; 
         land_state= 0 ; 

  memset(owner,  0, sizeof(owner )) ;
  memset(target, 0, sizeof(target)) ;
       o_owner =NULL ;   
       o_target=NULL ;   

        ErrorEnable= 1 ;

   CalculateExt_use= 0 ;
}


/*********************************************************************/
/*								     */
/*		        Деструктор класса "ОБЪЕКТ"      	     */

    RSS_Object::~RSS_Object(void)

{
    int  i ;

/*--------------------------------------------- Удаление компонентов */

   for(i=0 ; i<this->Units.List_cnt ; i++)
                  delete this->Units.List[i].object ;

/*-------------------------------------------------------------------*/

}


/********************************************************************/
/*								    */
/*                        Копировать объект		            */

    class RSS_Object *RSS_Object::vCopy(char *name)

{
   return(NULL) ;
}


/********************************************************************/
/*								    */
/*                    Сохранить состояние объекта                   */
/*                    Восстановить состояние объекта                */

    void  RSS_Object::vPush(void)

{
     x_base_stack    =x_base ;
     y_base_stack    =y_base ;
     z_base_stack    =z_base ;

     a_azim_stack    =a_azim ;
     a_elev_stack    =a_elev ;
     a_roll_stack    =a_roll ;

     x_velocity_stack=x_velocity ;
     y_velocity_stack=y_velocity ;
     z_velocity_stack=z_velocity ;
}


    void  RSS_Object::vPop(void)

{
     x_base    =x_base_stack ;
     y_base    =y_base_stack ;
     z_base    =z_base_stack ;

     a_azim    =a_azim_stack ;
     a_elev    =a_elev_stack ;
     a_roll    =a_roll_stack ;

     x_velocity=x_velocity_stack ;
     y_velocity=y_velocity_stack ;
     z_velocity=z_velocity_stack ;
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

     int  RSS_Object::vGetPosition(RSS_Point *points)

{
        points->x=x_base ;
        points->y=y_base ;
        points->z=z_base ;

        points->azim=a_azim ;
        points->elev=a_elev ;
        points->roll=a_roll ;

    return(0) ;
}

    void  RSS_Object::vSetPosition(RSS_Point *points)

{
        x_base=points->x ;
        y_base=points->y ;
        z_base=points->z ;

        a_azim=points->azim ;
        a_elev=points->elev ;
        a_roll=points->roll ;
}


/********************************************************************/
/*								    */
/*                    Получение вектора скорости                    */

    int  RSS_Object::vGetVelocity(RSS_Vector *velocity)
{
        velocity->x=x_velocity ;
        velocity->y=y_velocity ;
        velocity->z=z_velocity ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		Сброс контекста проверки свойств                    */

     int  RSS_Object::vResetFeatures(void *data)

{
  int  i ;


   for(i=0 ; i<this->Features_cnt ; i++)
                this->Features[i]->vResetCheck(data) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		Подготовка своиств к проверке корректности          */

     int  RSS_Object::vPrepareFeatures(void *data)

{
  int  i ;


   for(i=0 ; i<this->Features_cnt ; i++)
                this->Features[i]->vPreCheck(data) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		Проверка непротиворечивости свойств                 */

     int  RSS_Object::vCheckFeatures(void *data, RSS_Objects_List *checked)

{
  int  status ;
  int  i ;

           status=0 ;

   for(i=0 ; i<this->Features_cnt ; i++) {
           status|=this->Features[i]->vCheck(data, checked) ;
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
/*                        Специальные действия                      */

     int  RSS_Object::vSpecial(char *oper, void *data)
{
  return(-1) ;
}


/********************************************************************/
/*								    */
/*                   Задание целевого состояния                     */

     int  RSS_Object::vCalculateDirect(RSS_Point *target, char *select)
{
           this->direct_target=*target ;
   strncpy(this->direct_select, select, sizeof(this->direct_select)-1) ;

  return(0) ;
}


/********************************************************************/
/*								    */
/*             Подготовка расчета изменения состояния               */

     int  RSS_Object::vCalculateStart(double  t)
{
    memset(this->direct_select, 0, sizeof(this->direct_select)) ;

  return(0) ;
}


/********************************************************************/
/*								    */
/*                   Расчет изменения состояния                     */

     int  RSS_Object::vCalculate(double t1, double t2, char *callback, int callback_size)
{

/*------------------------------------- Обработка целевого состояния */

   if(this->direct_select[0]!=0) {

          strupr(this->direct_select) ;

       if(strchr(this->direct_select, 'X')!=NULL)  this->x_base=this->direct_target.x ;
       if(strchr(this->direct_select, 'Y')!=NULL)  this->y_base=this->direct_target.y ;
       if(strchr(this->direct_select, 'Z')!=NULL)  this->z_base=this->direct_target.z ;
       if(strchr(this->direct_select, 'A')!=NULL)  this->a_azim=this->direct_target.azim ;
       if(strchr(this->direct_select, 'E')!=NULL)  this->a_elev=this->direct_target.elev ;
       if(strchr(this->direct_select, 'R')!=NULL)  this->a_roll=this->direct_target.roll ;

          memset(this->direct_select, 0, sizeof(this->direct_select)) ;

                                 }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*           Расчет изменения состояния во внешней модели           */

     int  RSS_Object::vCalculateExt1(double t1, double t2, char *callback, int callback_size)
{
  return(0) ;
}

     int  RSS_Object::vCalculateExt2(double t1, double t2, char *callback, int callback_size)
{
  return(0) ;
}


/********************************************************************/
/*								    */
/*      Отображение результата расчета изменения состояния          */

     int  RSS_Object::vCalculateShow(double  t1, double  t2)
{
  return(0) ;
}


/********************************************************************/
/*								    */
/*                        Обработка событий                         */

     int  RSS_Object::vEvent(char *event_name, double  t)
{
  return(-1) ;
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


/*********************************************************************/
/*                                                                   */
/*               Проверка попадания угла в диапазон                  */

  int  RSS_Object::iAngleInCheck(double  value, 
                                 double  value_min, 
                                 double  value_max )
{
  int  i ;


  for(i=0 ; i<2 ; i++) {

    if(value_min<=value_max) {
	 if(value>=value_min &&
	    value<=value_max   ) return(0) ;
			     }
    else                     {
	 if(value<=value_min &&
	    value>=value_max   ) return(0) ;
			     }

    if(value>0)  value-=360. ;
    else         value+=360. ;

                       }

  return(1) ;
}


/********************************************************************/
/*								    */
/*              Замена текстового фрагмента в строке                */

  int  RSS_Object::iReplaceText(char *buff, char *name, char *value, int  count)

{
   char *entry ;
    int  shift ;
    int  i ;


     if(count==0)  count=10000 ;

             entry=buff ;
             shift=strlen(value)-strlen(name) ;

     for(i=0 ; i<count ; i++) {

             entry=strstr(entry, name) ;
          if(entry==NULL)  break ;
                         
               memmove(entry+strlen(value), entry+strlen(name), 
                                       strlen(entry+strlen(name))+1) ;
                memcpy(entry, value, strlen(value)) ;

                              }

  return(i) ;
}


/*********************************************************************/
/*								     */
/*	      Компоненты класса "СПИСОК ОБЪЕКТОВ"	             */
/*								     */
/*********************************************************************/

/*********************************************************************/
/*								     */
/*	       Конструктор класса "СПИСОК ОБЪЕКТОВ"      	     */

     RSS_Objects_List::RSS_Objects_List(void)

{
     List    =NULL ;
     List_cnt=  0 ;
     List_max=  0 ;
}


/*********************************************************************/
/*								     */
/*	        Деструктор класса "СПИСОК ОБЪЕКТОВ"      	     */

    RSS_Objects_List::~RSS_Objects_List(void)

{
    if(List!=NULL)  free(List) ;
}


/********************************************************************/
/*								    */
/*		              Очистка списка		            */

    void  RSS_Objects_List::Clear(void)

{
    List_cnt=0 ;
}


/********************************************************************/
/*								    */
/*		      Добавление записи в список	            */

    int  RSS_Objects_List::Add(class RSS_Object *object, char *relation)

{
   int  i ;

/*--------------------------------- Повторная регистрация компонента */

   for(i=0 ; i<List_cnt ; i++)
     if(object==List[i].object)  return(0) ;

/*------------------------------------------------ Замена компонента */

   for(i=0 ; i<List_cnt ; i++)
     if(!stricmp(object->Name, List[i].object->Name))  break ;

      if(i<List_cnt) {
                               List[i].object->vFree() ;
                        delete List[i].object ;
                     }
/*------------------------------------- Добавление нового компонента */

   if(i>=List_max) {
                       List_max+=10 ;
                       List     =(RSS_Objects_List_Elem *)
                                  realloc(List, List_max*sizeof(*List)) ;
                   }

   if(List==NULL)  return(-1) ;


               List[i].object=object ;
       strncpy(List[i].relation, relation, sizeof(List[0].relation)-1) ;

    if(i>=List_cnt)  List_cnt++ ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


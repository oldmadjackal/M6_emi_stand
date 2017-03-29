
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <malloc.h>


#include "RSS_Feature.h"


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


/********************************************************************/
/*								    */
/*		       Статические переменные			    */



/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Feature::RSS_Feature()

{
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Feature::~RSS_Feature()

{
}


/********************************************************************/
/*								    */
/*		        Считать данные из строки		    */

    void RSS_Feature::vReadSave(char *title, std::string *text, char *reference)

{
}


/********************************************************************/
/*								    */
/*		        Записать данные в строку		    */

    void RSS_Feature::vWriteSave(std::string *text)

{
     *text="" ;
}


/********************************************************************/
/*								    */
/*		      Освободить ресурсы        		    */

    void RSS_Feature::vFree(void)

{
}


/********************************************************************/
/*								    */
/*                     Выдать информацию о свойстве   		    */

    void RSS_Feature::vGetInfo(std::string *text)

{
        *text="" ;
}


/********************************************************************/
/*								    */
/*                     Работа с параметрами           		    */

     int RSS_Feature::vParameter(char *name, char *action, char *value)

{
   return(-1) ;
}


/********************************************************************/
/*								    */
/*                Проверка непротиворечивости свойства              */  

    int RSS_Feature::vCheck(void *data)

{
   return(0) ;
}


/********************************************************************/
/*								    */
/*                          Добавить тело                           */

    void RSS_Feature::vBodyAdd(char *body)
{
}


/********************************************************************/
/*								    */
/*                          Удалить тело                            */

    void RSS_Feature::vBodyDelete(char *body)
{
}


/********************************************************************/
/*								    */
/*                   Задание базовой точки тела                     */

    void RSS_Feature::vBodyBasePoint(   char *body,                   
                                      double  x, 
                                      double  y, 
                                      double  z )
{
}


/********************************************************************/
/*								    */
/*                     Задание ориентации тела                      */

    void RSS_Feature::vBodyAngles(   char *body,                   
                                   double  azimuth, 
                                   double  tangage, 
                                   double  roll    )
{
}


/********************************************************************/
/*								    */
/*              Задание матрицы положения тела                      */

    void RSS_Feature::vBodyMatrix(   char *body,
                                   double  matrix[4][4] )
{
}


/********************************************************************/
/*								    */
/*           Задание смещения положения и ориентации тела           */

    void RSS_Feature::vBodyShifts(   char *body,                   
                                   double  x, 
                                   double  y, 
                                   double  z, 
                                   double  azimuth, 
                                   double  tangage, 
                                   double  roll    )
{
}


/********************************************************************/
/*								    */
/*              Задание списка параметров тела                      */

    void RSS_Feature::vBodyPars(char *body, struct RSS_Parameter *)
{
}



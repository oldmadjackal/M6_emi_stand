
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
/*		       ����������� ����������			    */



/********************************************************************/
/*								    */
/*		       ����������� ������			    */

     RSS_Feature::RSS_Feature()

{
}


/********************************************************************/
/*								    */
/*		        ���������� ������			    */

    RSS_Feature::~RSS_Feature()

{
}


/********************************************************************/
/*								    */
/*		        ������� ������ �� ������		    */

    void RSS_Feature::vReadSave(char *title, std::string *text, char *reference)

{
}


/********************************************************************/
/*								    */
/*		        �������� ������ � ������		    */

    void RSS_Feature::vWriteSave(std::string *text)

{
     *text="" ;
}


/********************************************************************/
/*								    */
/*		      ���������� �������        		    */

    void RSS_Feature::vFree(void)

{
}


/********************************************************************/
/*								    */
/*                     ������ ���������� � ��������   		    */

    void RSS_Feature::vGetInfo(std::string *text)

{
        *text="" ;
}


/********************************************************************/
/*								    */
/*                     ������ � �����������           		    */

     int RSS_Feature::vParameter(char *name, char *action, char *value)

{
   return(-1) ;
}


/********************************************************************/
/*								    */
/*       ����� ��������� �������� ������������������ ��������       */  

    int RSS_Feature::vResetCheck(void *data)

{
   return(0) ;
}


/********************************************************************/
/*								    */
/*       ���������� � �������� ������������������ ��������          */  

    int RSS_Feature::vPreCheck(void *data)

{
   return(0) ;
}


/********************************************************************/
/*								    */
/*                �������� ������������������ ��������              */  

    int RSS_Feature::vCheck(void *data, RSS_Objects_List *checked)

{
   return(0) ;
}


/********************************************************************/
/*								    */
/*                    ����������� ������ �� ��������                */  

    int RSS_Feature::vShow(void *data)

{
   return(0) ;
}


/********************************************************************/
/*								    */
/*                          �������� ����                           */

    void RSS_Feature::vBodyAdd(char *body)
{
}


/********************************************************************/
/*								    */
/*                          ������� ����                            */

    void RSS_Feature::vBodyDelete(char *body)
{
}


/********************************************************************/
/*								    */
/*                   ������� ������� ����� ����                     */

    void RSS_Feature::vBodyBasePoint(   char *body,                   
                                      double  x, 
                                      double  y, 
                                      double  z )
{
}


/********************************************************************/
/*								    */
/*                     ������� ���������� ����                      */

    void RSS_Feature::vBodyAngles(   char *body,                   
                                   double  azimuth, 
                                   double  tangage, 
                                   double  roll    )
{
}


/********************************************************************/
/*								    */
/*              ������� ������� ��������� ����                      */

    void RSS_Feature::vBodyMatrix(   char *body,
                                   double  matrix[4][4] )
{
}


/********************************************************************/
/*								    */
/*           ������� �������� ��������� � ���������� ����           */

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
/*              ������� ������ ���������� ����                      */

    void RSS_Feature::vBodyPars(char *body, struct RSS_Parameter *)
{
}



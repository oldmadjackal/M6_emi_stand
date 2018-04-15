/********************************************************************/
/*								    */
/*		������ ���������� �������� "������"  		    */
/*								    */
/********************************************************************/


#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <direct.h>
#include <malloc.h>
#include <io.h>
#include <string.h>
#include <time.h>
#include <sys\stat.h>

#include "gl\gl.h"
#include "gl\glu.h"

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Kernel\RSS_Kernel.h"
#include "..\RSS_Model\RSS_Model.h"

#include "O_Marker.h"

#pragma warning(disable : 4996)

#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text)

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
/*		    	����������� ������                          */

     static   RSS_Module_Marker  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	����������������� ����                      */

 O_MARKER_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 O_MARKER_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/********************************************************************/
/**							           **/
/**	  �������� ������ ������ ���������� �������� "������"	   **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*                            ������ ������                         */

  struct RSS_Module_Marker_instr  RSS_Module_Marker_InstrList[]={

 { "help",   "?",  "#HELP   - ������ ��������� ������", 
                    NULL,
                   &RSS_Module_Marker::cHelp   },
 { "create", "cr", "#CREATE - ������� ������",
                   " CREATE <���> [<������> [<����>]]\n"
                   "   ������� ����������� ������ �������� ����� � �����",
                   &RSS_Module_Marker::cCreate },
 { "info",   "i",  "#INFO - ������ ���������� �� �������",
                   " INFO <���> \n"
                   "   ������ �������� ��������� �� ������� � ������� ����\n"
                   " INFO/ <���> \n"
                   "   ������ ������ ���������� �� ������� � ��������� ����",
                   &RSS_Module_Marker::cInfo },
 { "base",   "b", "#BASE - ������ ������� ����� �������",
                   " BASE <���> <x> <y> <z>\n"
                   "   ������ ������� ����� �������\n"
                   " BASE/x <���> <x>\n"
                   "   ������ ���������� X ������� ����� �������\n"
                   "       (���������� ��� Y � Z)\n"
                   " BASE/+x <���> <x>\n"
                   "   ������ ���������� ���������� X ������� ����� �������\n"
                   "       (���������� ��� Y � Z)\n"
                   " BASE> <���>\n"
                   "   ������ ������������ ���������� ������� ������ �������\n",
                   &RSS_Module_Marker::cBase },
 { "angle",  "a", "#ANGLE - ������ ���� ���������� �������",
                   "           A (AZIMUTH)   - ������\n"
                   "           E (ELEVATION) - ����������\n"
                   "           R (ROLL)      - ����\n"
                   " ANGLE <���> <a> <e> <r>\n"
                   "   ������ ���� ���������� �������\n"
                   " ANGLE/a <���> <a>\n"
                   "   ������ ���� ���������� A �������\n"
                   "       (���������� ��� E � R)\n"
                   " ANGLE/+a <���> <a>\n"
                   "   ������ ���������� ���� ���������� A �������\n"
                   "       (���������� ��� E � R)\n"
                   " ANGLE> <���>\n"
                   "   ������ ������������ ���������� ������ ���������� �������\n",
                   &RSS_Module_Marker::cAngle },
 {  NULL }
                                                            } ;

/********************************************************************/
/*								    */
/*		     ����� ����� ������             		    */

    struct RSS_Module_Marker_instr *RSS_Module_Marker::mInstrList=NULL ;

/********************************************************************/
/*								    */
/*		       ����������� ������			    */

     RSS_Module_Marker::RSS_Module_Marker(void)

{
	   keyword="EmiStand" ;
    identification="Marker" ;
          category="Object" ;

        mInstrList=RSS_Module_Marker_InstrList ;

          mSize=50. ;
}


/********************************************************************/
/*								    */
/*		        ���������� ������			    */

    RSS_Module_Marker::~RSS_Module_Marker(void)

{
}


/********************************************************************/
/*								    */
/*		      �������� �������                              */

  RSS_Object *RSS_Module_Marker::vCreateObject(RSS_Model_data *data)

{
  RSS_Object_Marker *object ;
               char *end ;
                int  i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

#define  COLOR  data->pars[0].value
 
/*--------------------------------------------------- �������� ����� */

    if(data->name[0]==0) {                                           /* ���� ��� �� ������ */
              SEND_ERROR("������ MARKER: �� ������ ��� �������") ;
                                return(NULL) ;
                         }

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, data->name)) {
              SEND_ERROR("������ MARKER: ������ � ����� ������ ��� ����������") ;
                                return(NULL) ;
                                                    }
/*--------------------------------------- ����������� ������ ������� */

    if(data->model[0]==0) {                                         /* ���� ������ ��-���������... */
                             strcpy(data->model, "Cross") ;
                          }

    if(stricmp(data->model, "Cross"  ) &&
       stricmp(data->model, "Pyramid")   ) {
                 
              SEND_ERROR("������ MARKER: ����������� ������ �������") ;
                                              return(NULL) ;
                                           }
/*---------------------------------------- ����������� ����� ������� */

   if(COLOR[0]==0) {                                                /* ���� ���� ��-���������... */
                      strcpy(COLOR, "Red") ;
                   }

   if(stricmp(COLOR, "Red"  ) &&
      stricmp(COLOR, "Green") && 
      stricmp(COLOR, "Blue")    ) {
                 
              SEND_ERROR("������ MARKER: ����������� ���� �������") ;
                                            return(NULL) ;
                                  }
/*------------------------------------------------- �������� ������� */

       object=new RSS_Object_Marker ;
    if(object==NULL) {
              SEND_ERROR("������ MARKER: ������������ ������ ��� �������� �������") ;
                        return(NULL) ;
                     }

     strncpy(object->model, data->model, sizeof(object->model)-1) ;
             object->size=mSize ;

   if(!stricmp(COLOR, "Red"  ))  object->color=RGB(255,   0,   0) ;
   if(!stricmp(COLOR, "Green"))  object->color=RGB(  0, 255,   0) ;
   if(!stricmp(COLOR, "Blue" ))  object->color=RGB(  0,   0, 255) ;

/*------------------------------------- ���������� ������ ���������� */

#define  PAR      object->Parameters
#define  PAR_CNT  object->Parameters_cnt
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������� ��������� */
     for(i=0 ; i<5 ; i++)
       if(data->pars[i].text[0]!=0) {

           PAR=(struct RSS_Parameter *)
                 realloc(PAR, (PAR_CNT+1)*sizeof(*PAR)) ;
        if(PAR==NULL) {
                         SEND_ERROR("������ MARKER: ������������ ������") ;
                                            return(NULL) ;
                      }

             memset(&PAR[PAR_CNT], 0, sizeof(PAR[PAR_CNT])) ;
            sprintf( PAR[PAR_CNT].name, "PAR%d", i+1) ;
                     PAR[PAR_CNT].value=strtod(data->pars[i].value, &end) ;
                         PAR_CNT++ ;
                                    }
/*- - - - - - - - - - - - - - - - - - - - ����������� ������ ������� */
           PAR=(struct RSS_Parameter *)
                 realloc(PAR, (PAR_CNT+1)*sizeof(*PAR)) ;
        if(PAR==NULL) {
                         SEND_ERROR("������ MARKER: ������������ ������") ;
                                            return(NULL) ;
                      }

             memset(&PAR[PAR_CNT], 0, sizeof(PAR[PAR_CNT])) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#undef  PAR
#undef  PAR_CNT

/*--------------------------------------------- ������������ ������� */

                      object->iFormMarker_() ;

/*------------------ ������������ ���������������� ��������� ������� */

                      object->iPlaceMarker_() ;

/*---------------------------------- �������� ������� � ����� ������ */

       OBJECTS=(RSS_Object **)
                 realloc(OBJECTS, (OBJECTS_CNT+1)*sizeof(*OBJECTS)) ;
    if(OBJECTS==NULL) {
              SEND_ERROR("������ MARKER: ������������ ������") ;
                                return(NULL) ;
                      }

              OBJECTS[OBJECTS_CNT]=object ;
                      OBJECTS_CNT++ ;

             strcpy(object->Name, data->name) ;
                    object->Module=this ;


        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_DEFAULT_OBJECT, (LPARAM)data->name) ;

/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT
#undef     COLOR

  return(object) ;
}


/********************************************************************/
/*								    */
/*		        �������� ��������       		    */

     int  RSS_Module_Marker::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- �������� ������ */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s -  ������", identification) ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*                ���������� ��������� ������                       */

    void  RSS_Module_Marker::vShow(char *layer)

{
     int  status ;
     int  i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 
  
/*----------------------------------------- ��������������� �������� */

  if(!stricmp(layer, "ZOOM")) {

          status=RSS_Kernel::display.SetFirstContext("Show") ;
    while(status==0) {                                              /* LOOP - ������� ���������� ����������� */

     for(i=0 ; i<OBJECTS_CNT ; i++) {                               /* LOOP - ������� �������� */

       if(stricmp(OBJECTS[i]->Type, "Marker"))  continue ;

           ((RSS_Object_Marker *)OBJECTS[i])->iPlaceMarker() ;
                                    }                               /* ENDLOOP - ������� �������� */

          status=RSS_Kernel::display.SetNextContext("Show") ;
                     }                                              /* ENDLOOP  - ������� ���������� ����������� */

                              }
/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

}


/********************************************************************/
/*								    */
/*		        ��������� �������       		    */

  int  RSS_Module_Marker::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* ���� ������ ������ ������� */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "MARKER"
#define  _SECTION_SHRT_NAME   "M"

/*--------------------------------------------- ������������� ������ */

             memset(command, 0, sizeof(command)) ;
            strncpy(command, cmd, sizeof(command)-1) ;

   if(!direct_command) {

         end=strchr(command, ' ') ;
      if(end!=NULL) {  *end=0 ;  end++ ;  }

      if(stricmp(command, _SECTION_FULL_NAME) &&
         stricmp(command, _SECTION_SHRT_NAME)   )  return(1) ;
                       }
   else                {
                             end=command ;
                       }
/*----------------------- ���������/���������� ������ ������ ������� */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - -  ��������� */
   if(end==NULL || end[0]==0) {
     if(!direct_command) {
                            direct_command=1 ;

        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Object Marker:") ;
        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_DIRECT_COMMAND, (LPARAM)identification) ;
                         }
                                    return(0) ;
                              }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - ���������� */
   if(end!=NULL && !strcmp(end, "..")) {

                            direct_command=0 ;

        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"") ;
        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_DIRECT_COMMAND, (LPARAM)"") ;

                                           return(0) ;
                                       }
/*--------------------------------------------- ��������� ���������� */

       instr=end ;                                                  /* �������� ����� � ��������� ������� */

     for(end=instr ; *end!= 0  &&
                     *end!=' ' &&
                     *end!='>' &&
                     *end!='/'    ; end++) ;

      if(*end!= 0 &&
         *end!=' '  )  memmove(end+1, end, strlen(end)+1) ;

      if(*end!=0) {  *end=0 ;  end++ ;  }
      else            end="" ;

   for(i=0 ; mInstrList[i].name_full!=NULL ; i++)                   /* ���� ������� � ������ */
     if(!stricmp(instr, mInstrList[i].name_full) ||
        !stricmp(instr, mInstrList[i].name_shrt)   )   break ;

     if(mInstrList[i].name_full==NULL) {                            /* ���� ����� ������� ���... */

          status=this->kernel->vExecuteCmd(cmd) ;                   /*  �������� �������� ������ ����... */
       if(status)  SEND_ERROR("������ MARKER: ����������� �������") ;
                                            return(-1) ;
                                       }
 
     if(mInstrList[i].process!=NULL)                                /* ���������� ������� */
                status=(this->*mInstrList[i].process)(end) ;
     else       status=  0 ;

/*-------------------------------------------------------------------*/

   return(status) ;
}


/********************************************************************/
/*								    */
/*		        ������� ������ �� ������		    */

    void  RSS_Module_Marker::vReadSave(std::string *data)

{
               char *buff ;
                int  buff_size ;
     RSS_Model_data  create_data ;
  RSS_Object_Marker *object ;
               char *entry ;
               char *end ;

/*----------------------------------------------- �������� ��������� */

   if(memicmp(data->c_str(), "#BEGIN MODULE MARKER\n", 
                      strlen("#BEGIN MODULE MARKER\n")) &&
      memicmp(data->c_str(), "#BEGIN OBJECT MARKER\n", 
                      strlen("#BEGIN OBJECT MARKER\n"))   )  return ;

/*------------------------------------------------ ���������� ������ */

              buff_size=data->size()+16 ;
              buff     =(char *)calloc(1, buff_size) ;

       strcpy(buff, data->c_str()) ;

/*------------------------------------------------- �������� ������� */

   if(!memicmp(buff, "#BEGIN OBJECT MARKER\n", 
              strlen("#BEGIN OBJECT MARKER\n"))) {                  /* IF.1 */
/*- - - - - - - - - - - - - - - - - - - - - -  ���������� ���������� */
              memset(&create_data, 0, sizeof(create_data)) ;

                                     entry=strstr(buff, "NAME=") ;  /* ��������� ��� ������� */
           strncpy(create_data.name, entry+strlen("NAME="), 
                                       sizeof(create_data.name)-1) ;
        end=strchr(create_data.name, '\n') ;
       *end= 0 ;

                                     entry=strstr(buff, "MODEL=") ; /* ��������� ������ ������� */
           strncpy(create_data.model, entry+strlen("MODEL="),
                                       sizeof(create_data.model)-1) ;
        end=strchr(create_data.model, '\n') ;
       *end= 0 ;
/*- - - - - - - - - - - - - - - �������� ���������� �������� ������� */
/*- - - - - - - - - - - - - - - - - - - - - - - - - �������� ������� */
                object=(RSS_Object_Marker *)vCreateObject(&create_data) ;
             if(object==NULL)  return ;
/*- - - - - - - - - - - - - - - - - - - - - - ��������� ������������ */
       entry=strstr(buff, "SIZE=") ;  object->size=atof(entry+strlen("SIZE=")) ;
       entry=strstr(buff, "COLOR=") ; object->color=strtoul(entry+strlen("COLOR="), &end, 16) ;
/*- - - - - - - - - - - - ������� ������� ����� � ���������� ������� */
       entry=strstr(buff, "X_BASE=") ; object->x_base=atof(entry+strlen("X_BASE=")) ;
       entry=strstr(buff, "Y_BASE=") ; object->y_base=atof(entry+strlen("Y_BASE=")) ;
       entry=strstr(buff, "Z_BASE=") ; object->z_base=atof(entry+strlen("Z_BASE=")) ;
       entry=strstr(buff, "A_AZIM=") ; object->a_azim=atof(entry+strlen("A_AZIM=")) ;
       entry=strstr(buff, "A_ELEV=") ; object->a_elev=atof(entry+strlen("A_ELEV=")) ;
       entry=strstr(buff, "A_ROLL=") ; object->a_roll=atof(entry+strlen("A_ROLL=")) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                     object->iPlaceMarker_() ;
                                               }                    /* END.1 */
/*-------------------------------------------- ������������ �������� */

                free(buff) ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        �������� ������ � ������		    */

    void  RSS_Module_Marker::vWriteSave(std::string *text)

{

/*----------------------------------------------- ��������� �������� */

     *text="#BEGIN MODULE MARKER\n" ;

/*------------------------------------------------ �������� �������� */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� HELP                    */

  int  RSS_Module_Marker::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Object_Marker_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� CREATE                  */
/*								    */
/*      CREATE <���> [<������> [<����>]]                            */

  int  RSS_Module_Marker::cCreate(char *cmd)

{
 RSS_Model_data  data ;
     RSS_Object *object ;
           char *name ;
           char *model ;
           char *pars[4] ;
           char *end ;
            int  i ;

/*-------------------------------------- ���������� ��������� ������ */

                            model ="" ;
     for(i=0 ; i<4 ; i++)  pars[i]="" ;

   do {                                                             /* BLOCK.1 */
                  name=cmd ;                                        /* ��������� ��� ������� */
                   end=strchr(name, ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;

                 model=end+1 ;                                      /* ��������� �������� ������ */
                   end=strchr(model, ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
          
     for(i=0 ; i<4 ; i++) {                                         /* ��������� ��������� */
               pars[i]=end+1 ;            
                   end=strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                          }
      } while(0) ;                                                  /* BLOCK.1 */

/*--------------------------------- ���������� ����� ������ �������� */

         memset(&data, 0, sizeof(data)) ;

        strncpy(data.name,  name,  sizeof(data.name)) ;
        strncpy(data.model, model, sizeof(data.model)) ;

    for(i=0 ; *pars[i]!=0 ; i++) {
        strncpy(data.pars[i].value,  pars[i], sizeof(data.pars[i].value)) ;
         strcpy(data.pars[i].text, "") ;
                                 }

/*------------------------------------------------- �������� ������� */

      object=vCreateObject(&data) ;
   if(object==NULL)  return(-1) ;

/*------------------------------------------------ ����������� ����� */

              this->kernel->vShow("REFRESH") ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� INFO                    */
/*								    */
/*        INFO   <���>                                              */
/*        INFO/  <���>                                              */

  int  RSS_Module_Marker::cInfo(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX   4

               char  *name ;
  RSS_Object_Marker  *object ;
                int   all_flag ;   /* ���� ������ ������ ���������� */
               char  *end ;
        std::string   info ;
        std::string   f_info ;
               char   text[4096] ;

/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
                           all_flag=0 ;

       if(*cmd=='/') {
                           all_flag=1 ;
 
                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("������������ ������ �������") ;
                                       return(-1) ;
                              }

                           cmd=end+1 ;
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������ ���������� */        
                  name=cmd ;
                   end=strchr(name, ' ') ;
                if(end!=NULL)  *end=0 ;

/*------------------------------------------- �������� ����� ������� */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* ���� ��� �� ������... */
                           SEND_ERROR("�� ������ ��� �������. \n"
                                      "��������: INFO <���_�������> ...") ;
                                     return(-1) ;
                         }

       object=FindObject(name) ;                                    /* ���� ������ �� ����� */
    if(object==NULL)  return(-1) ;

/*-------------------------------------------- ������������ �������� */

      sprintf(text, "%s\r\n%s\r\n"
                    "Base X % 7.3lf\r\n" 
                    "     Y % 7.3lf\r\n" 
                    "     Z % 7.3lf\r\n"
                    "Ang. A % 7.3lf\r\n" 
                    "     E % 7.3lf\r\n" 
                    "     R % 7.3lf\r\n"
                    "\r\n",
                        object->Name, object->Type, 
                        object->x_base, object->y_base, object->z_base,
                        object->a_azim, object->a_elev, object->a_roll
                    ) ;

           info=text ;

/*-------------------------------------------------- ������ �������� */
/*- - - - - - - - - - - - - - - - - - - - - - - - - � ��������� ���� */
     if(all_flag) {

                  }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - � ������� ���� */
     else         {

        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_SHOW_INFO, (LPARAM)info.c_str()) ;
                  }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� BASE                    */
/*								    */
/*        BASE    <���> <X> <Y> <Z>                                 */
/*        BASE/X  <���> <X>                                         */
/*        BASE/+X <���> <X>                                         */
/*        BASE>   <���> <��� ���������> <���>                       */
/*        BASE>>  <���> <��� ���������> <���>                       */

  int  RSS_Module_Marker::cBase(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX   4

               char  *pars[_PARS_MAX] ;
               char  *name ;
               char **xyz ;
             double   coord[_COORD_MAX] ;
                int   coord_cnt ;
             double   inverse ;
  RSS_Object_Marker  *object ;
                int   xyz_flag ;          /* ���� ������ ����� ���������� */
                int   delta_flag ;        /* ���� ������ ���������� */
                int   arrow_flag ;        /* ���� ����������� ������ */
               char  *arrows ;
               char  *error ;
               char  *end ;
                int   i ;

/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
                        xyz_flag=0 ;
                      delta_flag=0 ;
                      arrow_flag=0 ;

       if(*cmd=='/' ||
          *cmd=='+'   ) {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("������������ ������ �������") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, '+')!=NULL   )  delta_flag= 1 ;

                if(strchr(cmd, 'x')!=NULL ||
                   strchr(cmd, 'X')!=NULL   )    xyz_flag='X' ;
           else if(strchr(cmd, 'y')!=NULL ||
                   strchr(cmd, 'Y')!=NULL   )    xyz_flag='Y' ;
           else if(strchr(cmd, 'z')!=NULL ||
                   strchr(cmd, 'Z')!=NULL   )    xyz_flag='Z' ;

                           cmd=end+1 ;
                        }

  else if(*cmd=='>') {
                           delta_flag=1 ;
                           arrow_flag=1 ;

                          cmd=strchr(cmd, ' ') ;
                       if(cmd==NULL)  return(0) ;
                          cmd++ ;
                     } 
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������ ���������� */        
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

                     name= pars[0] ;
                      xyz=&pars[1] ;   
                  inverse=   1. ; 

/*------------------------------------- ��������� ����������� ������ */

    if(arrow_flag) {                        
                         arrows=pars[1] ;

      if(strstr(arrows, "left" )!=NULL   ) {  xyz_flag='X' ;  inverse=-1. ;  }
      if(strstr(arrows, "right")!=NULL   ) {  xyz_flag='X' ;  inverse= 1. ;  }  
      if(strstr(arrows, "up"   )!=NULL &&
         strstr(arrows, "ctrl" )==NULL   ) {  xyz_flag='Y' ;  inverse= 1. ;  }
      if(strstr(arrows, "down" )!=NULL &&
         strstr(arrows, "ctrl" )==NULL   ) {  xyz_flag='Y' ;  inverse=-1. ;  }
      if(strstr(arrows, "up"   )!=NULL &&
         strstr(arrows, "ctrl" )!=NULL   ) {  xyz_flag='Z' ;  inverse= 1. ;  }
      if(strstr(arrows, "down" )!=NULL &&
         strstr(arrows, "ctrl" )!=NULL   ) {  xyz_flag='Z' ;  inverse=-1. ;  }
           
                          xyz=&pars[2] ;
                   }
/*------------------------------------------- �������� ����� ������� */

    if(name==NULL) {                                                /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� �������. \n"
                                 "��������: BASE <���_�������> ...") ;
                                     return(-1) ;
                   }

       object=FindObject(name) ;                                    /* ���� ������ �� ����� */
    if(object==NULL)  return(-1) ;

/*------------------------------------------------- ������ ��������� */

    for(i=0 ; xyz[i]!=NULL && i<_COORD_MAX ; i++) {

             coord[i]=strtod(xyz[i], &end) ;
        if(*end!=0) {  
                       SEND_ERROR("������������ �������� ����������") ;
                                       return(-1) ;
                    }
                                                  }

                             coord_cnt=  i ;

                                 error= NULL ;
      if(xyz_flag) {
               if(coord_cnt==0)  error="�� ������� ���������� ��� �� ����������" ;
                   }
      else         {
               if(coord_cnt <3)  error="������ ���� ������� 3 ����������" ;
                   }

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

/*------------------------------------------------ ������� ��������� */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - ���������� */
   if(delta_flag) {

          if(xyz_flag=='X')   object->x_base+=inverse*coord[0] ;
     else if(xyz_flag=='Y')   object->y_base+=inverse*coord[0] ;                 
     else if(xyz_flag=='Z')   object->z_base+=inverse*coord[0] ;
                  }
/*- - - - - - - - - - - - - - - - - - - - - - -  ���������� �������� */
   else           {

          if(xyz_flag=='X')   object->x_base=coord[0] ;
     else if(xyz_flag=='Y')   object->y_base=coord[0] ;                 
     else if(xyz_flag=='Z')   object->z_base=coord[0] ;
     else                   {
                              object->x_base=coord[0] ;
                              object->y_base=coord[1] ;
                              object->z_base=coord[2] ;
                            }
                  }
/*------------------------------------------------ ����������� ����� */

            object->iPlaceMarker_() ;

              this->kernel->vShow("REFRESH") ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� ANGLE                   */
/*								    */
/*       ANGLE    <���> <A> <E> <R>                                 */
/*       ANGLE/A  <���> <A>                                         */
/*       ANGLE/+A <���> <A>                                         */
/*       ANGLE>   <���> <��� ���������> <���>                       */
/*       ANGLE>>  <���> <��� ���������> <���>                       */

  int  RSS_Module_Marker::cAngle(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX   4

               char  *pars[_PARS_MAX] ;
               char  *name ;
               char **xyz ;
             double   coord[_COORD_MAX] ;
                int   coord_cnt ;
             double   inverse ;
  RSS_Object_Marker  *object ;
                int   xyz_flag ;          /* ���� ������ ����� ���������� */
                int   delta_flag ;        /* ���� ������ ���������� */
                int   arrow_flag ;        /* ���� ����������� ������ */
               char  *arrows ;
               char  *error ;
               char  *end ;
                int   i ;

/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
                        xyz_flag=0 ;
                      delta_flag=0 ;
                      arrow_flag=0 ;

       if(*cmd=='/' ||
          *cmd=='+'   ) {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("������������ ������ �������") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, '+')!=NULL   )  delta_flag= 1 ;

                if(strchr(cmd, 'a')!=NULL ||
                   strchr(cmd, 'A')!=NULL   )    xyz_flag='A' ;
           else if(strchr(cmd, 'e')!=NULL ||
                   strchr(cmd, 'E')!=NULL   )    xyz_flag='E' ;
           else if(strchr(cmd, 'r')!=NULL ||
                   strchr(cmd, 'R')!=NULL   )    xyz_flag='R' ;

                           cmd=end+1 ;
                        }

  else if(*cmd=='>') {
                           delta_flag=1 ;
                           arrow_flag=1 ;

                          cmd=strchr(cmd, ' ') ;
                       if(cmd==NULL)  return(0) ;
                          cmd++ ;
                     } 
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������ ���������� */        
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

                     name= pars[0] ;
                      xyz=&pars[1] ;   
                  inverse=   1. ; 

/*------------------------------------- ��������� ����������� ������ */

    if(arrow_flag) {                        
                         arrows=pars[1] ;

      if(strstr(arrows, "left" )!=NULL &&
         strstr(arrows, "ctrl" )==NULL   ) {  xyz_flag='A' ;  inverse=-1. ;  }
      if(strstr(arrows, "right")!=NULL &&  
         strstr(arrows, "ctrl" )==NULL   ) {  xyz_flag='A' ;  inverse= 1. ;  }  

      if(strstr(arrows, "up"   )!=NULL   ) {  xyz_flag='E' ;  inverse= 1. ;  }
      if(strstr(arrows, "down" )!=NULL   ) {  xyz_flag='E' ;  inverse=-1. ;  }

      if(strstr(arrows, "left" )!=NULL &&
         strstr(arrows, "ctrl" )!=NULL   ) {  xyz_flag='R' ;  inverse=-1. ;  }
      if(strstr(arrows, "right")!=NULL &&  
         strstr(arrows, "ctrl" )!=NULL   ) {  xyz_flag='R' ;  inverse= 1. ;  }  
           
                          xyz=&pars[2] ;   
                   }
/*------------------------------------------- �������� ����� ������� */

    if(name==NULL) {                                                /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� �������. \n"
                                 "��������: ANGLE <���_�������> ...") ;
                                     return(-1) ;
                   }

       object=FindObject(name) ;                                    /* ���� ������ �� ����� */
    if(object==NULL)  return(-1) ;

/*------------------------------------------------- ������ ��������� */

    for(i=0 ; xyz[i]!=NULL && i<_COORD_MAX ; i++) {

             coord[i]=strtod(xyz[i], &end) ;
        if(*end!=0) {  
                       SEND_ERROR("������������ �������� ����������") ;
                                       return(-1) ;
                    }
                                                  }

                             coord_cnt=  i ;

                                 error= NULL ;
      if(xyz_flag) {
               if(coord_cnt==0)  error="�� ������� ���������� ��� �� ����������" ;
                   }
      else         {
               if(coord_cnt <3)  error="������ ���� ������� 3 ����������" ;
                   }

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

/*------------------------------------------------ ������� ��������� */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - ���������� */
   if(delta_flag) {

          if(xyz_flag=='A')   object->a_azim+=inverse*coord[0] ;
     else if(xyz_flag=='E')   object->a_elev+=inverse*coord[0] ;                 
     else if(xyz_flag=='R')   object->a_roll+=inverse*coord[0] ;
                  }
/*- - - - - - - - - - - - - - - - - - - - - - -  ���������� �������� */
   else           {

          if(xyz_flag=='A')   object->a_azim=coord[0] ;
     else if(xyz_flag=='E')   object->a_elev=coord[0] ;                 
     else if(xyz_flag=='R')   object->a_roll=coord[0] ;
     else                   {
                              object->a_azim=coord[0] ;
                              object->a_elev=coord[1] ;
                              object->a_roll=coord[2] ;
                            }
                  }
/*- - - - - - - - - - - - - - - - - - - - - -  ������������ �������� */
     while(object->a_azim> 180.)  object->a_azim-=360. ;
     while(object->a_azim<-180.)  object->a_azim+=360. ;

     while(object->a_elev> 180.)  object->a_elev-=360. ;
     while(object->a_elev<-180.)  object->a_elev+=360. ;

     while(object->a_roll> 180.)  object->a_roll-=360. ;
     while(object->a_roll<-180.)  object->a_roll+=360. ;

/*------------------------------------------------ ����������� ����� */

            object->iFormMarker_() ;
            object->iPlaceMarker_() ;

              this->kernel->vShow("REFRESH") ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*								    */
/*		   ����� ������� ���� MARKER �� �����               */

  RSS_Object_Marker *RSS_Module_Marker::FindObject(char *name)

{
     char   text[1024] ;
      int   i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*------------------------------------------- ����� ������� �� ����� */ 

       for(i=0 ; i<OBJECTS_CNT ; i++)                               /* ���� ������ �� ����� */
         if(!stricmp(OBJECTS[i]->Name, name))  break ;

    if(i==OBJECTS_CNT) {                                            /* ���� ��� �� �������... */
                           sprintf(text, "������� � ������ '%s' "
                                         "�� ����������", name) ;
                        SEND_ERROR(text) ;
                            return(NULL) ;
                       }
/*-------------------------------------------- �������� ���� ������� */ 

     if(stricmp(OBJECTS[i]->Type, "Marker")) {

           SEND_ERROR("������ �� �������� �������� ���� MARKER") ;
                            return(NULL) ;
                                             }
/*-------------------------------------------------------------------*/ 

   return((RSS_Object_Marker *)OBJECTS[i]) ;
  
#undef   OBJECTS
#undef   OBJECTS_CNT

}


/********************************************************************/
/********************************************************************/
/**							           **/
/**		  �������� ������ ������� "������"	           **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       ����������� ������			    */

     RSS_Object_Marker::RSS_Object_Marker(void)

{
   strcpy(Type, "Marker") ;

    Context        =new RSS_Transit_Marker ;
    Context->object=this ;

       Parameters    =NULL ;
       Parameters_cnt=  0 ;

           x_base    =0 ;
           y_base    =0 ;
           z_base    =0 ;

           a_azim    =0 ;
           a_elev    =0 ;
           a_roll    =0 ;

           x_velocity=0 ;
           y_velocity=0 ;
           z_velocity=0 ;

           dlist1_idx=0 ;
           dlist2_idx=0 ;
}


/********************************************************************/
/*								    */
/*		        ���������� ������			    */

    RSS_Object_Marker::~RSS_Object_Marker(void)

{
      if(this->dlist1_idx)
           RSS_Kernel::display.ReleaseList(this->dlist1_idx) ;

      if(this->dlist2_idx)
           RSS_Kernel::display.ReleaseList(this->dlist2_idx) ;
}


/********************************************************************/
/*								    */
/*		        �������� ������ � ������		    */

    void  RSS_Object_Marker::vWriteSave(std::string *text)

{
  char  field[1024] ;

/*----------------------------------------------- ��������� �������� */

     *text="#BEGIN OBJECT MARKER\n" ;

/*----------------------------------------------------------- ������ */

    sprintf(field, "NAME=%s\n",       this->Name  ) ;  *text+=field ;
    sprintf(field, "MODEL=%s\n",      this->model ) ;  *text+=field ;
    sprintf(field, "COLOR=%x\n",      this->color ) ;  *text+=field ;
    sprintf(field, "SIZE=%.10lf\n",   this->size  ) ;  *text+=field ;
    sprintf(field, "X_BASE=%.10lf\n", this->x_base) ;  *text+=field ;
    sprintf(field, "Y_BASE=%.10lf\n", this->y_base) ;  *text+=field ;
    sprintf(field, "Z_BASE=%.10lf\n", this->z_base) ;  *text+=field ;
    sprintf(field, "A_AZIM=%.10lf\n", this->a_azim) ;  *text+=field ;
    sprintf(field, "A_ELEV=%.10lf\n", this->a_elev) ;  *text+=field ;
    sprintf(field, "A_ROLL=%.10lf\n", this->a_roll) ;  *text+=field ;

/*------------------------------------------------ �������� �������� */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*                �������� ������ ���������� ����������             */

   int  RSS_Object_Marker::vListControlPars(RSS_ControlPar *list)

{
 RSS_ControlPar  pars[6]={ 
     { "x_base", "������� �����", "X",               "double" },
     { "y_base", "������� �����", "Y",               "double" },
     { "z_base", "������� �����", "Z",               "double" },
     { "a_azim", "����������",    "������",          "double" },
     { "a_elev", "����������",    "���� ����������", "double" },
     { "a_roll", "����������",    "���� �����",      "double" },
                         } ;


     if(list!=NULL)  memcpy(list, pars, sizeof(pars)) ;

   return(6) ;
}


/********************************************************************/
/*								    */
/*               ���������� �������� ��������� ����������           */

   int  RSS_Object_Marker::vSetControlPar(RSS_ControlPar *par)   

{
   double  value ;
     char  text[1024] ;


   if(!stricmp(par->type, "double"))  value=*((double *)par->value) ;
   else                              {
               sprintf(text, "Unknown value type %s for object %s", par->type, this->Name) ;
            SEND_ERROR(text) ;
                return(-1) ;
                                     }

        if(!stricmp(par->link, "x_base"))  this->x_base=value ;
   else if(!stricmp(par->link, "y_base"))  this->y_base=value ;
   else if(!stricmp(par->link, "z_base"))  this->z_base=value ;
   else if(!stricmp(par->link, "a_azim"))  this->a_azim=value ;
   else if(!stricmp(par->link, "a_elev"))  this->a_elev=value ;
   else if(!stricmp(par->link, "a_roll"))  this->a_roll=value ;
   else                                   {
               sprintf(text, "Unknown control parameter %s for object %s", par->link, this->Name) ;
            SEND_ERROR(text) ;
                return(-1) ;
                                          }

     return(0) ;
}


/********************************************************************/
/*								    */
/*           ������� ����� ������� � ��������� ���������            */

  int  RSS_Object_Marker::iFormMarker_(void)

{
       strcpy(Context->action, "FORM") ;

  SendMessage(RSS_Kernel::kernel_wnd, 
                WM_USER, (WPARAM)_USER_CHANGE_CONTEXT, 
                         (LPARAM) this->Context       ) ;

  return(0) ;
}


/********************************************************************/
/*								    */
/*		      ������� ����� �������                         */

  int  RSS_Object_Marker::iFormMarker(void)

{
  int  status ;


/*-------------------------------- �������������� ����������� ������ */

     if(dlist2_idx==0)  dlist2_idx=RSS_Kernel::display.GetList(1) ;

     if(dlist2_idx==0)  return(-1) ;

/*----------------------------------- ������� ���������� ����������� */

          status=RSS_Kernel::display.SetFirstContext("Show") ;
    while(status==0) {                                              /* CIRCLE.1 */

/*--------------------------------------- ������������ ����� ������� */

             glNewList(dlist2_idx, GL_COMPILE) ;                    /* �������� ������ */
          glMatrixMode(GL_MODELVIEW) ;
//            glEnable(GL_BLEND) ;                                  /* ���.���������� ������ */
//         glDepthMask(0) ;                                         /* ����.������ Z-������ */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  ����� */
    if(!stricmp(model, "Cross"  )) {

 double  s ;
                     s=0.5 ;

                     glColor4d(GetRValue(color)/256., 
                               GetGValue(color)/256.,
                               GetBValue(color)/256., 1.) ;

                       glBegin(GL_LINES) ;
                    glVertex3d(-s,  0,  0) ;
                    glVertex3d( s,  0,  0) ;
                    glVertex3d( 0, -s,  0) ;
                    glVertex3d( 0,  s,  0) ;
                    glVertex3d( 0,  0, -s) ;
                    glVertex3d( 0,  0,  s) ;
	                 glEnd();
                                   }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - �������� */
    if(!stricmp(model, "Pyramid")) {

 double  s ;
 double  h ;

                     s=0.5 ;
                     h=1.0 ;

                     glColor4d(GetRValue(color)/256., 
                               GetGValue(color)/256.,
                               GetBValue(color)/256., 0.2) ;

                       glBegin(GL_POLYGON) ;
                    glVertex3d( s, -h, -s) ;
                    glVertex3d(-s, -h, -s) ;
                    glVertex3d( 0, -h,  s) ;
	                 glEnd();
                       glBegin(GL_POLYGON) ;
                    glVertex3d( 0,  0,  0) ;
                    glVertex3d( s, -h, -s) ;
                    glVertex3d(-s, -h, -s) ;
	                 glEnd();
                       glBegin(GL_POLYGON) ;
                    glVertex3d( 0,  0,  0) ;
                    glVertex3d(-s, -h, -s) ;
                    glVertex3d( 0, -h,  s) ;
              	         glEnd();
                       glBegin(GL_POLYGON) ;
                    glVertex3d( 0,  0,  0) ;
                    glVertex3d( 0, -h,  s) ;
                    glVertex3d( s, -h, -s) ;
	                 glEnd();
                                   }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//         glDepthMask(1) ;                                         /* ���.������ Z-������ */
//           glDisable(GL_BLEND) ;                                  /* ����.���������� ������ */
             glEndList() ;                                          /* �������� ������ */

/*----------------------------------- ������� ���������� ����������� */

          status=RSS_Kernel::display.SetNextContext("Show") ;
                     }                                              /* CONTINUE.1 */
/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*           ���������������� ������� � ��������� ���������         */

  int  RSS_Object_Marker::iPlaceMarker_(void)

{
       strcpy(Context->action, "PLACE") ;

  SendMessage(RSS_Kernel::kernel_wnd, 
                WM_USER, (WPARAM)_USER_CHANGE_CONTEXT, 
                         (LPARAM) this->Context       ) ;

  return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������������� �������                      */

  int  RSS_Object_Marker::iPlaceMarker(void)

{
  double  zoom ;  
     int  status ;


/*-------------------------------- �������������� ����������� ������ */

     if(dlist1_idx==0)  dlist1_idx=RSS_Kernel::display.GetList(0) ;

     if(dlist1_idx==0)  return(-1) ;

/*----------------------------------- ������� ���������� ����������� */

          status=RSS_Kernel::display.SetFirstContext("Show") ;
    while(status==0) {                                              /* CIRCLE.1 */

/*------------------ ������������ ��������������� ������������������ */

             glNewList(dlist1_idx, GL_COMPILE) ;                    /* �������� ������ */

          glMatrixMode(GL_MODELVIEW) ;

          glTranslated(x_base, y_base, z_base) ;

                       zoom=RSS_Kernel::display.GetContextPar("Zoom") ;
                       zoom=zoom/size ;
              glScaled(zoom, zoom, zoom) ;

            glCallList(dlist2_idx) ;                                /* ��������� ������� */

             glEndList() ;                                          /* �������� ������ */

/*----------------------------------- ������� ���������� ����������� */

          status=RSS_Kernel::display.SetNextContext("Show") ;
                     }                                              /* CONTINUE.1 */
/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*								     */
/*	      ���������� ������ "������� ���������"	             */
/*								     */
/*********************************************************************/

/*********************************************************************/
/*								     */
/*	       ����������� ������ "������� ���������"      	     */

     RSS_Transit_Marker::RSS_Transit_Marker(void)

{
}


/*********************************************************************/
/*								     */
/*	        ���������� ������ "������� ���������"      	     */

    RSS_Transit_Marker::~RSS_Transit_Marker(void)

{
}


/********************************************************************/
/*								    */
/*	              ���������� ��������                           */

    int  RSS_Transit_Marker::vExecute(void)

{
   if(!stricmp(action, "FORM" ))  ((RSS_Object_Marker *)object)->iFormMarker() ;
   if(!stricmp(action, "PLACE"))  ((RSS_Object_Marker *)object)->iPlaceMarker() ;

   return(0) ;
}



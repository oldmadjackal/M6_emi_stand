/********************************************************************/
/*								    */
/*	������ ���������� ����������� "����������� ����������"      */
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
#include <math.h>
#include <sys\stat.h>

#include "gl\gl.h"
#include "gl\glu.h"

#include "..\Matrix\Matrix.h"

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Unit\RSS_Unit.h"
#include "..\RSS_Kernel\RSS_Kernel.h"
#include "..\RSS_Model\RSS_Model.h"
#include "..\F_Show\F_Show.h"

#include "..\DCL_kernel\dcl.h"

#include "U_Program.h"

#pragma warning(disable : 4996)

#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text)
#define  CREATE_DIALOG       CreateDialogIndirectParam

#include "..\Emi_root\controls.h"
#include "resource.h"



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

     static   RSS_Module_Program  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	����������������� ����                      */

 U_PROGRAM_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 U_PROGRAM_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/********************************************************************/
/**							           **/
/**    �������� ������ ������ ����������                           **/
/**                        ����������� "����������� ����������"    **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*                            ������ ������                         */

  struct RSS_Module_Program_instr  RSS_Module_Program_InstrList[]={

 { "help",    "?",  "#HELP   - ������ ��������� ������", 
                     NULL,
                    &RSS_Module_Program::cHelp   },
 { "info",    "i",  "#INFO - ������ ���������� �� �������",
                    " INFO <���> \n"
                    "   ������ �������� ���������� �� ������� � ������� ����\n"
                    " INFO/ <���> \n"
                    "   ������ ������ ���������� �� ������� � ��������� ����",
                    &RSS_Module_Program::cInfo },
 { "program", "p",  "#PROGRAM - ������� ��������� ��������� �������",
                    " PROGRAM <���> <���� � ����� ���������>\n"
                    "   ��������� ��������� ������� ��������� � �����",
                    &RSS_Module_Program::cProgram },
 { "embeded", "e",  "#EMBEDED - ������� ����������� ��������� ��������� �������",
                    " EMBEDED <���> <���� � ����� ���������� ���������>\n"
                    "   ��������� ��������� ���� ��������:\n"
                    "     COLUMN_HANTER - ����� � ����� ����� � ����� '�������'\n",
                    &RSS_Module_Program::cEmbeded },
 { "show",    "s",  "#SHOW - ���������� ���� �������",
                    " SHOW <���> \n"
                    "   ���������� ���� �������\n",
                    &RSS_Module_Program::cShow },
 {  NULL }
                                                            } ;

/********************************************************************/
/*								    */
/*		     ����� ����� ������             		    */

    struct RSS_Module_Program_instr *RSS_Module_Program::mInstrList=NULL ;


/********************************************************************/
/*								    */
/*		       ����������� ������			    */

     RSS_Module_Program::RSS_Module_Program(void)

{
  static  WNDCLASS  Indicator_wnd ;

/*---------------------------------------------------- ������������� */

	   keyword="EmiStand" ;
    identification="Program" ;
          category="Unit" ;

        mInstrList=RSS_Module_Program_InstrList ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        ���������� ������			    */

    RSS_Module_Program::~RSS_Module_Program(void)

{
}


/********************************************************************/
/*								    */
/*		      �������� �������                              */

  RSS_Object *RSS_Module_Program::vCreateObject(RSS_Model_data *data)

{
   RSS_Unit_Program *unit ;
                int  i ;
 
/*---------------------------------------------- �������� ���������� */

       unit=new RSS_Unit_Program ;
    if(unit==NULL) {
               SEND_ERROR("������ SEARCH_RADAR: ������������ ������ ��� �������� ����������") ;
                        return(NULL) ;
                   }

             unit->Module=this ;

      strcpy(unit->Decl, "����������� ����������") ;

/*------------------------------- �������� ������ ������� ���������� */

        unit->Features_cnt=this->feature_modules_cnt ;
        unit->Features    =(RSS_Feature **)
                             calloc(this->feature_modules_cnt, 
                                     sizeof(unit->Features[0])) ;

   for(i=0 ; i<this->feature_modules_cnt ; i++)
        unit->Features[i]=this->feature_modules[i]->vCreateFeature(unit, NULL) ;

/*-------------------------------------------------------------------*/

  return(unit) ;
}


/********************************************************************/
/*								    */
/*		        �������� ��������       		    */

     int  RSS_Module_Program::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- �������� ������ */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s -  ����������� ����������", identification) ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		        ��������� �������       		    */

  int  RSS_Module_Program::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* ���� ������ ������ ������� */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "PROGRAM"
#define  _SECTION_SHRT_NAME   "PGM"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Object Program:") ;
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
       if(status)  SEND_ERROR("������ Program: ����������� �������") ;
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

    void  RSS_Module_Program::vReadSave(std::string *data)

{
               char *buff ;
                int  buff_size ;
     RSS_Model_data  create_data ;
//  RSS_Unit_EWunit *unit ;
               char *entry ;
               char *end ;

/*----------------------------------------------- �������� ��������� */

   if(memicmp(data->c_str(), "#BEGIN MODULE PROGRAM\n", 
                      strlen("#BEGIN MODULE PROGRAM\n")) &&
      memicmp(data->c_str(), "#BEGIN UNIT PROGRAM\n", 
                      strlen("#BEGIN UNIT PROGRAM\n"  ))   )  return ;

/*------------------------------------------------ ���������� ������ */

              buff_size=data->size()+16 ;
              buff     =(char *)calloc(1, buff_size) ;

       strcpy(buff, data->c_str()) ;

/*---------------------------------------------- �������� ���������� */

   if(!memicmp(buff, "#BEGIN UNIT PROGRAM\n", 
              strlen("#BEGIN UNIT PROGRAM\n"))) {               /* IF.1 */
/*- - - - - - - - - - - - - - - - - - - - - -  ���������� ���������� */
              memset(&create_data, 0, sizeof(create_data)) ;

                                     entry=strstr(buff, "NAME=") ;  /* ��������� ��� ���������� */
           strncpy(create_data.name, entry+strlen("NAME="), 
                                       sizeof(create_data.name)-1) ;
        end=strchr(create_data.name, '\n') ;
       *end= 0 ;
/*- - - - - - - - - - - - -  �������� ���������� �������� ���������� */
/*- - - - - - - - - - - - - - - - - - - - - - -  �������� ���������� */
/*
                status=vCreateObject(NULL) ;
             if(status)  return ;
*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                    }               /* END.1 */
/*-------------------------------------------- ������������ �������� */

                free(buff) ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        �������� ������ � ������		    */

    void  RSS_Module_Program::vWriteSave(std::string *text)

{
  std::string  buff ;

/*----------------------------------------------- ��������� �������� */

     *text="#BEGIN MODULE PROGRAM\n" ;

/*------------------------------------------------ �������� �������� */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� HELP                    */

  int  RSS_Module_Program::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Unit_Program_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� INFO                    */
/*								    */
/*        INFO   <���>                                              */
/*        INFO/  <���>                                              */

  int  RSS_Module_Program::cInfo(char *cmd)

{
                   char  *name ;
   RSS_Unit_Program  *unit ;
                    int   all_flag ;   /* ���� ������ ������ ���������� */
                   char  *end ;
            std::string   info ;
            std::string   f_info ;
                   char   text[4096] ;
                    int   i ;

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

/*----------------------------------------- �������� ����� ��������� */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* ���� ��� �� ������... */
                           SEND_ERROR("�� ������ ��� ���������. \n"
                                      "��������: INFO <���_���������> ...") ;
                                     return(-1) ;
                         }

       unit=(RSS_Unit_Program *)FindUnit(name) ;                     /* ���� ��������� �� ����� */
    if(unit==NULL)  return(-1) ;

/*-------------------------------------------- ������������ �������� */

      sprintf(text, "%s\r\n%s\r\n"
                    "Owner       %s\r\n" 
                    "\r\n",
                        unit->Name,      unit->Type, 
                        unit->Owner->Name
                    ) ;

           info=text ;

/*----------------------------------------------- ������ �� �������� */

   for(i=0 ; i<unit->Features_cnt ; i++) {

                        unit->Features[i]->vGetInfo(&f_info) ;
                                               info+=f_info ;
                                               info+="\r\n" ;
                                           }
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
/*		      ���������� ���������� PROGRAM                 */
/*								    */
/*       PROGRAM <���> <���� � �����>                               */

  int  RSS_Module_Program::cProgram(char *cmd)

{
#define   _PARS_MAX  10

              char  *pars[_PARS_MAX] ;
              char  *name ;
              char  *path ;
  RSS_Unit_Program  *unit ;
              char *data ;
              char *end ;
              char  error[1024] ;
              char  text[1024] ;
               int   i ;

/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������ ���������� */        
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

                     name=pars[0] ;
                     path=pars[1] ;   

/*------------------------------------------- �������� ����� ������� */

    if(name==NULL) {                                                /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� �������. \n"
                                 "��������: PROGRAM <���_����������> ...") ;
                                     return(-1) ;
                   }

       unit=(RSS_Unit_Program *)FindUnit(name) ;                /* ���� ������-���� �� ����� */
    if(unit==NULL)  return(-1) ;

/*----------------------------------------- �������� ����� ��������� */

     if(path==NULL) {
                       SEND_ERROR("�� ����� ���� ���������. \n"
                                  "��������: PROGRAM <��� ����������> <���� � ����� ���������>") ;
                                        return(-1) ;
                    } 

        data=this->FileCache(path, error) ;
     if(data==NULL) {
                         sprintf(text, "������ �������� ����� ��������� - %s", error) ;
                      SEND_ERROR(text) ;
                           return(-1) ;
                    }

                 unit->program=data ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� EMBEDED                 */
/*								    */
/*       EMBEDED <���> <���� � �����>                               */

  int  RSS_Module_Program::cEmbeded(char *cmd)

{
#define   _PARS_MAX  10

              char  *pars[_PARS_MAX] ;
              char  *name ;
              char  *path ;
  RSS_Unit_Program  *unit ;
              void *data ;
              char *end ;
              char  error[1024] ;
              char  text[1024] ;
               int   i ;

/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������ ���������� */        
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

                     name=pars[0] ;
                     path=pars[1] ;   

/*------------------------------------------- �������� ����� ������� */

    if(name==NULL) {                                                /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� �������. \n"
                                 "��������: EMBEDED <���_����������> ...") ;
                                     return(-1) ;
                   }

       unit=(RSS_Unit_Program *)FindUnit(name) ;                    /* ���� ������-���� �� ����� */
    if(unit==NULL)  return(-1) ;

/*------------------------------ ���������� ����� �������� ��������� */

     if(path==NULL) {
                       SEND_ERROR("�� ����� ���� ���������. \n"
                                  "��������: PROGRAM <��� ����������> <���� � ����� ���������>") ;
                                        return(-1) ;
                    } 

        data=this->ReadEmbeded(path, error) ;
     if(data==NULL) {
                         sprintf(text, "������ �������� ����� ��������� - %s", error) ;
                      SEND_ERROR(text) ;
                           return(-1) ;
                    }

                 unit->embeded=data ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� SHOW                    */
/*								    */
/*        SHOW <���>                                                */

  int  RSS_Module_Program::cShow(char *cmd)

{
#define   _PARS_MAX  10

              char *pars[_PARS_MAX] ;
              char *name ;
              char *pos ;
  RSS_Unit_Program *unit ;
              char *end ;
               int  i  ;

/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������ ���������� */        
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

                     name=pars[0] ;
                      pos=pars[1] ;   

       if(pos==NULL)  pos="0" ;

/*---------------------------------------- �������� ����� ���������� */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* ���� ��� �� ������... */
                           SEND_ERROR("�� ������ ��� ����������. \n"
                                      "��������: SHOW <���_����������> ...") ;
                                     return(-1) ;
                         }

       unit=(RSS_Unit_Program *)FindUnit(name) ;                /* ���� ���������� �� ����� */
    if(unit==NULL)  return(-1) ;

/*----------------------------------------- �������� ���� ���������� */

          strcpy(unit->Context->action, "INDICATOR") ;
          strcpy(unit->Context->details, pos) ;

     SendMessage(RSS_Kernel::kernel_wnd, 
                 WM_USER, (WPARAM)_USER_CHANGE_CONTEXT, 
                          (LPARAM) unit->Context       ) ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*	     ����� ������� ���� PROGRAM �� �����                    */

  RSS_Unit *RSS_Module_Program::FindUnit(char *name)

{
 RSS_Object *object ;
 RSS_Object *unit ;
       char  o_name[128] ;
       char *u_name ;
       char  text[1024] ;
        int  i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*-------------------------------------------------- ���������� ���� */ 

               memset(o_name, 0, sizeof(o_name)) ;
              strncpy(o_name, name, sizeof(o_name)-1) ;
        u_name=strchr(o_name, '.') ;

     if(u_name==NULL) {
                           sprintf(text, "��� '%s' �� �������� ������ ���������� �������", name) ;
                        SEND_ERROR(text) ;
                            return(NULL) ;
                      }

       *u_name=0 ;
        u_name++ ;
     
/*------------------------------------------- ����� ������� �� ����� */ 

             object=NULL ;

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, o_name)) {
                                                   object=OBJECTS[i] ;
                                                     break ; 
                                                }

    if(object==NULL) {
                           sprintf(text, "������� � ������ '%s' �� ����������", o_name) ;
                        SEND_ERROR(text) ;
                            return(NULL) ;
                     }
/*---------------------------------------- ����� ���������� �� ����� */ 

                    unit=NULL ;

       for(i=0 ; i<object->Units.List_cnt ; i++)
         if(!stricmp(object->Units.List[i].object->Name, u_name)) { 
                         unit=object->Units.List[i].object ;
                                               break ;
                                                                  }

    if(unit==NULL) {
                           sprintf(text, "������ '%s' �� �������� ��������� '%s'", o_name, u_name) ;
                        SEND_ERROR(text) ;
                            return(NULL) ;
                   }
/*-------------------------------------------- �������� ���� ������� */ 

     if(strcmp(unit->Type, "Program")) {

           SEND_ERROR("��������� �� �������� ����������� ���� Program") ;
                            return(NULL) ;
                                           }
/*-------------------------------------------------------------------*/ 

   return((RSS_Unit *)unit) ;

#undef   OBJECTS
#undef   OBJECTS_CNT

}


/********************************************************************/
/*								    */
/*	     ���������� �������� ���������� ���������               */

  void *RSS_Module_Program::ReadEmbeded(char *path, char *error)

{
                                  FILE *file ;
                                  void *data ;
 RSS_Unit_Program_Embeded_ColumnHunter *data_ColumnHunter ;
                                  char  text[256] ;
                                  char  name[128] ;
                                  char *key ;
                                  char *value ;
                                  char *end ;


/*---------------------------------------------------- ������������� */ 

           data=NULL ;
         *error= 0 ;

/*--------------------------------------------------- �������� ����� */ 

       file=fopen(path, "rt") ;
    if(file==NULL) {
                      sprintf(error, "������ %d �������� ����� �������� ��������� : %s", errno, path) ;
                         return(NULL) ;
                   }
/*-------------------------------------------- ���������� ���������� */ 

   do {
/*- - - - - - - - - - - - - - - - - - -  ���������� ��������� ������ */ 
               memset(text, 0, sizeof(text)) ;
            end=fgets(text, sizeof(text)-1, file) ;
         if(end==NULL)  break ;

            end=strchr(text, '\r') ;
         if(end!=NULL)  *end=0 ; 
            end=strchr(text, '\n') ;
         if(end!=NULL)  *end=0 ; 

         if(text[0]==';')  continue ;

              key=       text ;
            value=strchr(text, '=') ;
         if(value==NULL) {
                sprintf(error, "�������� ������ ����� �������� ��������� : %s", text) ;
                            break ;
                         }
           *value=0 ;
            value++ ;
/*- - - - - - - - - - - - - - - - - - - - - ��������� ���� ��������� */ 
         if(data==NULL) {

              if(stricmp(key, "PROGRAM")) {
                      sprintf(error, "������ ������ � ����� �������� ��������� ������ ���� PROGRAM=<��� ���������>") ;
                                              break ;
                                          }

              if(!stricmp(value, "COLUMN_HUNTER")) {
                    data_ColumnHunter=(RSS_Unit_Program_Embeded_ColumnHunter *)
                                             calloc(1, sizeof(*data_ColumnHunter)) ;
                    data             =data_ColumnHunter ;

                       strcpy(data_ColumnHunter->program_name, value) ;
                                                   }
              else                                 {
                      sprintf(error, "����������� ��� ���������") ;
                                                     break ;
                                                   }

                           strcpy(name, value) ;
                             continue ;
                        }
/*- - - - - - - - - - - - - - - - - - - - -  ��������� COLUMN_HUNTER */ 
     if(!stricmp(name, "COLUMN_HUNTER")) {

#define  D  data_ColumnHunter

          if(!stricmp(key, "G"             ))   D->g=strtod(value, &end) ;
          else
          if(!stricmp(key, "E_MAX"         ))   D->e_max=strtod(value, &end) ;
          else
          if(!stricmp(key, "E_RATE"        ))   D->e_rate=strtod(value, &end) ;
          else
          if(!stricmp(key, "H_MIN"         ))   D->h_min=strtod(value, &end) ;
          else
          if(!stricmp(key, "ROUTE"         )) {

                  if(D->route_cnt>=_ROUTE_MAX) {
                              sprintf(error, "������������ ������ ���������� �����") ;
                                                     break ;
                                               }               

                                              D->route[D->route_cnt].x=strtod(value, &end) ;
                                              D->route[D->route_cnt].z=strtod(end+1, &end) ;
                                                       D->route_cnt++ ;
                                             }
          else
          if(!stricmp(key, "STAGE_0_EVENT" ))   strncpy(D->stage_0_event, value, sizeof(D->stage_0_event)-1) ;
          else
          if(!stricmp(key, "STAGE_4_EVENT" ))   strncpy(D->stage_4_event, value, sizeof(D->stage_0_event)-1) ;
          else
          if(!stricmp(key, "STAGE_4_AFTER" ))   D->stage_4_after=strtod(value, &end) ;
          else
          if(!stricmp(key, "STAGE_1_RADAR" ))   strncpy(D->stage_1_radar, value, sizeof(D->stage_1_radar)-1) ;
          else
          if(!stricmp(key, "STAGE_4_RADAR" ))   strncpy(D->stage_4_radar, value, sizeof(D->stage_4_radar)-1) ;
          else
          if(!stricmp(key, "COLUMN_SPACING"))   D->column_spacing=strtod(value, &end) ;
          else
          if(!stricmp(key, "STAGE_2_TIME"  ))   D->stage_2_time=strtod(value, &end) ;
          else
          if(!stricmp(key, "JUMP_DISTANCE" ))   D->jump_distance=strtod(value, &end) ;
          else
          if(!stricmp(key, "DROP_HEIGHT"   ))   D->drop_height=strtod(value, &end) ;
          else
          if(!stricmp(key, "DROP_DISTANCE" ))   D->drop_distance=strtod(value, &end) ;
          else
          if(!stricmp(key, "DROPS_NUMBER"  ))   D->drops_number=strtoul(value, &end, 10) ;
          else
          if(!stricmp(key, "DROPS_INTERVAL"))   D->drops_interval=strtod(value, &end) ;
          else
          if(!stricmp(key, "DROP"         )) {

                  if(D->drops_cnt>=_ROUTE_MAX) {
                              sprintf(error, "������������ ������ ����������") ;
                                                     break ;
                                               }               

                                      strncpy(D->drops[D->drops_cnt].weapon, value, sizeof(D->drops[0].weapon)) ;
                                                       D->drops_cnt++ ;
                                             }
          else                      {
                   sprintf(error, "����������� �������� ��������� ����������: %s", key) ;
                                                     break ;
                                    } 

#undef  D

                                         } 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
      } while(1) ;
 
/*--------------------------------------------------- �������� ����� */ 

         fclose(file) ;

/*-------------------------------------------------------------------*/ 

   if(*error!=0)  return(NULL) ;
                  return(data) ;
}


/********************************************************************/
/********************************************************************/
/**							           **/
/**	  �������� ������ ���������� "���������� ����������"       **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       ����������� ������			    */

     RSS_Unit_Program::RSS_Unit_Program(void)

{
   strcpy(Type, "Program") ;

  Context        =new RSS_Transit_Program ;
  Context->object=this ;

   Parameters    =NULL ;
   Parameters_cnt=  0 ;

          program=NULL ;
          embeded=NULL ;
            state=NULL ;
 
            hWnd =NULL ;
}


/********************************************************************/
/*								    */
/*		        ���������� ������			    */

    RSS_Unit_Program::~RSS_Unit_Program(void)

{
      vFree() ;
}


/********************************************************************/
/*								    */
/*		       ������������ ��������                        */

  void   RSS_Unit_Program::vFree(void)

{
  int  i ;


  if(this->program!=NULL) {
                             free(this->program) ;
                                  this->program=NULL ;
                          }
  if(this->state  !=NULL) {
                             free(this->state) ;
                                  this->state=NULL ;
                          }

  if(this->Features!=NULL) {

    for(i=0 ; i<this->Features_cnt ; i++) {
           free(this->Features[i]) ;
                                          }

           free(this->Features) ;

                this->Features    =NULL ;
                this->Features_cnt=  0 ;
                           }
}


/********************************************************************/
/*								    */
/*		        �������� ������ � ������		    */

    void  RSS_Unit_Program::vWriteSave(std::string *text)

{
  char  field[1024] ;

/*----------------------------------------------- ��������� �������� */

     *text="#BEGIN UNIT PROGRAM\n" ;

/*----------------------------------------------------------- ������ */

    sprintf(field, "NAME=%s\n", this->Name) ;  *text+=field ;

/*------------------------------------------------ �������� �������� */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*                        ����������� ��������                      */

     int  RSS_Unit_Program::vSpecial(char *oper, void *data)
{
/*------------------------------------------ ������ �� ������ BATTLE */

    if(!stricmp(oper, "BATTLE")) {

                             this->battle=(RSS_Module_Battle *)data ;
                                      return(0) ;
                                 }
/*-------------------------------------------------------------------*/

  return(-1) ;
}


/********************************************************************/
/*								    */
/*             ���������� ������� ��������� ���������               */

     int  RSS_Unit_Program::vCalculateStart(double  t)
{
/*------------------------------------------------- ����� ��������� */

      event_time=0. ;
      event_send=0 ;

   if(state!=NULL)  free(state) ;
                         state=NULL ;

/*----------------------------------- ��������� ����������� �������� */

   if(embeded!=NULL) {

#define  PROGRAM_NAME  ((RSS_Unit_Program_Embeded *)embeded)->program_name

     if(!stricmp(PROGRAM_NAME, "COLUMN_HUNTER")) {
#define  P  ((RSS_Unit_Program_Embeded_ColumnHunter *)this->embeded)
                 P->stage             =0 ;
                 P->check_time        =0 ;
                 P->dropped           =0 ;
                 P->stage_4_event_done=0 ;
#undef   P
                                                 } 
     else                                        {
               SEND_ERROR("Section PROGRAM: ����������� ��������� ���������") ;
                            return(-1) ;
                                                 }

#undef  PROGRAM_NAME

                     }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*                   ������ ��������� ���������                     */

     static   RSS_Unit_Program *EventUnit ;
     static         RSS_Object *EventObject ;
     static               char *EventCallback ;
     static                int  EventCallback_size ;

         int  Program_emb_Log         (char *text) ;
         int  Program_emb_Turn        (char *angle, double  a_target, double  dt, char *limit_type, double  g) ;
         int  Program_emb_ToPoint     (double  x_target, double  z_target, double  dt, char *limit_type, double  g) ;
         int  Program_emb_ToLine      (double  x_target, double  z_target, 
                                       double  a_target, double  dt, char *limit_type, double  g) ;
         int  Program_emb_GetTargets  (char *unit_name,  RSS_Unit_Target **targets, int *targets_cnt, char *error) ;
         int  Program_emb_DetectOrder (RSS_Unit_Target  *targets, int  targets_cnt,
                                       RSS_Unit_Target **orders,  int *orders_cnt,
                                       double  spacing,  char *error) ;
         int  Program_emb_GetGlobalXYZ(RSS_Unit_Target *rel, RSS_Unit_Target *abs) ;
      double  Program_emb_Distance    (double  x, double  z) ;

    Dcl_decl *Program_dcl_Message     (Lang_DCL *,             Dcl_decl **, int) ;    /* ������ ��������� �� ����� � ��������� */
    Dcl_decl *Program_dcl_Log         (Lang_DCL *,             Dcl_decl **, int) ;    /* ������ � ��� */
    Dcl_decl *Program_dcl_StateSave   (Lang_DCL *, Dcl_decl *, Dcl_decl **, int) ;    /* ���������� ��������� ������� */
    Dcl_decl *Program_dcl_StateRead   (Lang_DCL *, Dcl_decl *, Dcl_decl **, int) ;    /* ���������� ��������� ������� */
    Dcl_decl *Program_dcl_Turn        (Lang_DCL *,             Dcl_decl **, int) ;    /* ��������� ����������� �������� */
    Dcl_decl *Program_dcl_Distance    (Lang_DCL *,             Dcl_decl **, int) ;    /* ����������� ��������� */
    Dcl_decl *Program_dcl_ToPoint     (Lang_DCL *,             Dcl_decl **, int) ;    /* �������� � ����� */
    Dcl_decl *Program_dcl_ToLine      (Lang_DCL *,             Dcl_decl **, int) ;    /* ����� �� ����� */
    Dcl_decl *Program_dcl_GetTargets  (Lang_DCL *, Dcl_decl *, Dcl_decl **, int) ;    /* ��������� ������ ����������� ����� */
    Dcl_decl *Program_dcl_DetectOrder (Lang_DCL *, Dcl_decl *, Dcl_decl **, int) ;    /* ��������� �������������� ���������� */
    Dcl_decl *Program_dcl_GetGlobalXYZ(Lang_DCL *, Dcl_decl *, Dcl_decl **, int) ;    /* ������� ���������� ��������� �� ������������� */
    Dcl_decl *Program_dcl_Battle      (Lang_DCL *,             Dcl_decl **, int) ;    /* ������� Battle */

     int  RSS_Unit_Program::vCalculate(double t1, double t2, char *callback, int cb_size)

{
       RSS_Kernel *dcl ;
    RSS_Parameter  const_list[5] ;
             void *context ;
             char  error[1024] ;
              int  status ;
              int  i ;

             char  obj_name[128] ;
           double  dt ;

    Dcl_decl  dcl_program_lib[]={
         {0, 0, 0, 0, "$PassiveData$", NULL, "program", 0, 0},
	 {_DGT_VAL,  0,          0, 0, "$Time",               &t1,                       NULL,   1,   1               },
	 {_DGT_VAL,  0,          0, 0, "$TimeN",              &t2,                       NULL,   1,   1               },
	 {_DGT_VAL,  0,          0, 0, "$dTime",              &dt,                       NULL,   1,   1               },
	 {_CHR_AREA, 0,          0, 0, "$ThisName",            obj_name,                 NULL,   0, sizeof(obj_name  )},
	 {_DGT_VAL,  0,          0, 0, "$ThisX",              &this->Owner->x_base,      NULL,   1,   1               },
	 {_DGT_VAL,  0,          0, 0, "$ThisY",              &this->Owner->y_base,      NULL,   1,   1               },
	 {_DGT_VAL,  0,          0, 0, "$ThisZ",              &this->Owner->z_base,      NULL,   1,   1               },
	 {_DGT_VAL,  0,          0, 0, "$ThisAzim",           &this->Owner->a_azim,      NULL,   1,   1               },
	 {_DGT_VAL,  0,          0, 0, "$ThisElev",           &this->Owner->a_elev,      NULL,   1,   1               },
	 {_DGT_VAL,  0,          0, 0, "$ThisRoll",           &this->Owner->a_roll,      NULL,   1,   1               },
 	 {_CHR_PTR, _DCL_CALL,   0, 0, "Message",      (void *)Program_dcl_Message,      "s",    0,   0               },
 	 {_CHR_PTR, _DCL_CALL,   0, 0, "Log",          (void *)Program_dcl_Log,          "s",    0,   0               },
 	 {_DGT_VAL, _DCL_CALL,   0, 0, "Distance",     (void *)Program_dcl_Distance,     "vv",   0,   0               },
 	 {_DGT_VAL, _DCL_CALL,   0, 0, "Turn",         (void *)Program_dcl_Turn,         "sv",   0,   0               },
 	 {_CHR_PTR, _DCL_CALL,   0, 0, "ToPoint",      (void *)Program_dcl_ToPoint,      "vvv",  0,   0               },
 	 {_CHR_PTR, _DCL_CALL,   0, 0, "ToLine",       (void *)Program_dcl_ToLine,       "vvvv", 0,   0               },
 	 {_DGT_VAL, _DCL_CALL,   0, 0, "Battle",       (void *)Program_dcl_Battle,       "s",    0,   0               },
 	 {_DGT_VAL, _DCL_METHOD, 0, 0, "StateSave",    (void *)Program_dcl_StateSave,    "",     0,   0               },
 	 {_DGT_VAL, _DCL_METHOD, 0, 0, "StateRead",    (void *)Program_dcl_StateRead,    "",     0,   0               },
 	 {_DGT_VAL, _DCL_METHOD, 0, 0, "GetTargets",   (void *)Program_dcl_GetTargets,   "s",    0,   0               },
 	 {_DGT_VAL, _DCL_METHOD, 0, 0, "DetectOrder",  (void *)Program_dcl_DetectOrder,  "av",   0,   0               },
 	 {_DGT_VAL, _DCL_METHOD, 0, 0, "GetGlobalXYZ", (void *)Program_dcl_GetGlobalXYZ, "vvv",  0,   0               },
	 {0, 0, 0, 0, "", NULL, NULL, 0, 0}
                              } ;

/*--------------------------------------------------- ������������� */

                  EventUnit         = this ;
                  EventObject       = this->Owner ;
                  EventCallback     = callback ;
                  EventCallback_size= cb_size ;

/*----------------------------------- ��������� ����������� �������� */

   if(embeded!=NULL) {

#define  PROGRAM_NAME  ((RSS_Unit_Program_Embeded *)embeded)->program_name

     if(!stricmp(PROGRAM_NAME, "COLUMN_HUNTER"))  status=EmbededColumnHunter(t1, t2, callback, cb_size) ;
     else                                        {
               SEND_ERROR("Section PROGRAM: ����������� ��������� ���������") ;
                            return(-1) ;
                                                 }

                             return(status) ;
                     }
/*-------------------------------------- ����������� DCL-����������� */

#define  CALC_CNT   RSS_Kernel::calculate_modules_cnt
#define  CALC       RSS_Kernel::calculate_modules

                      dcl=NULL ;

         for(i=0 ; i<CALC_CNT ; i++) {

             status=CALC[i]->vCalculate("DCL", NULL, NULL, NULL, 
                                               NULL, NULL, NULL ) ;
         if(status==0)  dcl=CALC[i] ;
                                     }

          if(dcl==NULL) {
               SEND_ERROR("Section PROGRAM: �� �������� ��������� DCL-���������") ;
                            return(-1) ;
                        }

#undef   CALC_CNT
#undef   CALC

/*------------------------------------------------ ���������� ������ */

        memset(const_list, 0, sizeof(const_list)) ;

        strcpy(const_list[0].name, "$LIBRARY") ;
               const_list[0].ptr=(double *)dcl_program_lib ;

        memset(error, 0, sizeof(error)) ; 

/*------------------------------------------------ ���������� ������ */

           memset(obj_name, 0, sizeof(obj_name)) ;                  /* $ThisName */ 
          strncpy(obj_name, EventObject->Name, sizeof(obj_name)-1) ;

                   dt=t2-t1 ;

       for(i=0 ; dcl_program_lib[i].name[0]!=0 ; i++)
         if(dcl_program_lib[i].type     ==_CHR_AREA &&
            dcl_program_lib[i].func_flag==  0         )  
             dcl_program_lib[i].size=strlen((char *)dcl_program_lib[i].addr) ;

/*--------------------------------------------- ���������� ��������� */

                      context= NULL ;

           status=dcl->vCalculate("DCL", this->program,
                                  const_list, NULL, NULL, &context, error) ;
                  dcl->vCalculate("CLEAR", NULL, NULL, NULL, NULL, &context, error) ;

        if(status) {
                      SEND_ERROR(error) ;
                            return(-1) ;
                   }
/*------------------------------------- ��������� ��������� �������� */
/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*      ����������� ���������� ������� ��������� ���������          */

     int  RSS_Unit_Program::vCalculateShow(double  t1, double  t2)
{
/*------------------------------------- �������� ������� �� �������� */

    if(event_send) {
                        this->Owner->vEvent(event_name, event_time) ;
                           event_send=0 ;
                   }
/*--------------------------------- ����������� ������ �� ���������� */

    if(this->hWnd!=NULL)  SendMessage(this->hWnd, WM_PAINT, NULL, NULL) ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*                                                                  */
/*               ���������� ��������� Column Hunter                 */
/*                                                                  */
/*  ����� ���������:                                                */
/*   0 - ����� � ����� ��������������                               */
/*   1 - �������������� �� ������ �������� � ��� �����������.       */
/*       ��� ����������� ��������� ���� ->2                         */
/*   2 - ��������� � ��������� ����� (��� ���������� � ���������)  */
/*       �� ��������� ������� ��������� - ������ ���� � ����������� */
/*       � ���������� (����� � ������� ���). ���� ���� ��������    */
/*       ������������ � ����� 2                                     */
/*   3 - ����� �� ������� ��� ���� � ������������� ��������� ��     */
/*       ���������� ���������� ������                               */
/*   4 - ����� ������ ��������                                      */
/*   5 - ���� �� ���� �� ���������                                  */
/*                                                                  */
/*  ��������� ���������:                                            */
/*    PROGRAM        - COLUMN_HUNTER                                */
/*    G              - ����������� ���������� ���������������       */
/*                      <20 - ������� g, >20 - ������ ��������      */
/*    E_MAX          - ������������ ���� ������� (+/-),             */
/*    E_RATE         - ������������ �������� ��������� ������� ���/�*/ 
/*    H_MIN          - ����������� ������ ������ ������������       */
/*    ROUTE          - ����� ��������: X, Z (����� ���� ���������)  */
/*    COLUMN_SPACING - ������������ ���������� ����� ������,        */
/*                      ��������� � ���� �������                    */
/*    JUMP_DISTANCE  - ��������� ������ �� ������ ������            */
/*    DROP_DISTANCE  - ��������� ������                             */
/*    DROP_HEIGHT    - ����������� ������ ������                    */
/*    DROPS_NUMBER   - ����� ������� � ����� ������                 */
/*    DROPS_INTERVAL - �������� ����� ��������                      */
/*    DROP_i         - i-�� ������������ ���������                  */
/*    STAGE_0_EVENT  - ������� ��� ���������� ����� 0               */
/*    STAGE_1_RADAR  - ��� ���������� ��������� ������� ��� ����� 1 */
/*    STAGE_2_TIME   - ����������������� ����� 2                    */
/*    STAGE_4_RADAR  - ��� ���������� ��������� ������� ��� ����� 4 */
/*    STAGE_4_EVENT  - ������� ��� ���������� ����� 4               */
/*    STAGE_4_AFTER  - �������� ���������� ������� STAGE_4_EVENT    */

     int  RSS_Unit_Program::EmbededColumnHunter(double t1, double t2, char *callback, int cb_size)
{
                 int  status ;
                char *g_type ;
     RSS_Unit_Target *targets ;
                 int  targets_cnt ;
     RSS_Unit_Target *orders ;
                 int  orders_cnt ;
     RSS_Unit_Target  order_abs ;
              double  distance ;
                char  text[1024] ;
                char  error[1024] ;

#define  P  ((RSS_Unit_Program_Embeded_ColumnHunter *)this->embeded)

/*------------------------------------------------------- ���������� */

        if(P->g<20)  g_type="G" ;
        else         g_type="R" ;

/*-------------------------------------------------- �������� ������ */

     if(EventObject->y_base < P->h_min && P->e_direct<0.)  P->e_direct=0. ;

     if(P->e_direct > EventObject->a_elev)  EventObject->a_elev+=P->e_rate*(t2-t1) ;
     if(P->e_direct < EventObject->a_elev)  EventObject->a_elev-=P->e_rate*(t2-t1) ;

/*----------------------------------- ����� � ��������� ����� ������ */

   if(P->stage==0) {

            if(P->route_cnt<2) {
                       SEND_ERROR("EmbededColumnHunter - �� ����� ������� �������������� (������� 2 �����)") ;
                                      return(-1) ;
                               }

        status=Program_emb_ToPoint(P->route[0].x, P->route[0].z, t2-t1, g_type, P->g) ;
     if(status) {
                             P->route_idx=1 ;
                             P->route_dir=1 ;

                   Program_emb_Log("���������� ���� ��������������.") ;

                   strcat(callback, P->stage_0_event) ;
                   strcat(callback, ";") ;
                             P->stage++ ;
                }

                   }
/*--------------------------------------------------- �������������� */

   else
   if(P->stage==1) {
/*- - - - - - - - - - - - - - - - - - - - - - - �������� �� �������� */
        status=Program_emb_ToPoint(P->route[P->route_idx].x, P->route[P->route_idx].z, t2-t1, g_type, P->g) ;
     if(status) {
                           sprintf(text, "���������� ���������� ����� %f / %f", P->route[P->route_idx].x, P->route[P->route_idx].z) ;
                   Program_emb_Log(text) ;

                             P->route_idx+=P->route_dir ;

                   if(P->route_idx>=P->route_cnt) {
                                                     P->route_idx=P->route_cnt-2 ;
                                                     P->route_dir= -1 ;
                                                  }
                   if(P->route_idx< 0           ) {
                                                     P->route_idx= 1 ;
                                                     P->route_dir= 1 ;
                                                  }
                }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - ����� ���� */
     if(P->stage_1_radar[0]!=0) {

          status=Program_emb_GetTargets(P->stage_1_radar,  &targets, &targets_cnt, error) ;
       if(status) {
                        sprintf(text, "EmbededColumnHunter - %s", error) ;
                     SEND_ERROR(text) ;
                          return(-1) ;
                  }

       if(targets_cnt) {

            Program_emb_DetectOrder( targets,  targets_cnt,
                                    &orders,  &orders_cnt, P->column_spacing, error) ;

          if(orders_cnt) {

               Program_emb_GetGlobalXYZ(&orders[0], &order_abs) ;

                              P->stage     = 2 ;
                              P->check_time=t1 ;
                              P->x_direct  =order_abs.x ;
                              P->z_direct  =order_abs.z ;

                            Program_emb_Log("���������� ��������� ����") ;
                               free(orders) ;
                         }

                               free(targets) ;
                       }
                                } 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                   }
/*------------------------------------------- ���� ���������� � ���� */

   else
   if(P->stage==2) {
/*- - - - - - - - - - - - - - - - - - - - -  �������� � ������� ���� */
        status=Program_emb_ToPoint(P->x_direct, P->z_direct, t2-t1, g_type, P->g) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - -  ������ ���� */
      if(t1>=P->check_time+P->stage_2_time) do {

           Program_emb_GetTargets (P->stage_1_radar,  &targets, &targets_cnt, error) ;

         if(targets_cnt==0) {                                       /* ���� ���� ��������... */
                               P->stage=1 ;
                                 break ;
                            }

           Program_emb_DetectOrder(targets, targets_cnt,
                                   &orders, &orders_cnt, P->column_spacing, error) ;

         if(orders_cnt==0) {                                       /* ���� ���� ��������... */
                             free(targets) ;
                               P->stage=1 ;
                                 break ;
                           }
        
          Program_emb_GetGlobalXYZ(&orders[0], &order_abs) ;

                              P->stage     = 3 ;
                              P->check_time=t1 ;
                              P->x_direct  =order_abs.x ;
                              P->z_direct  =order_abs.z ;
                              P->a_direct  =orders[0].azim+EventObject->a_azim ;
                              P->e_direct  = -P->e_max ;

                               free(targets) ;
                               free(orders) ;

                     Program_emb_Log("����� �� ������ ����...") ;

                                            } while(0) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                   }
/*--------------------------------------------- ����� �� ������ ���� */

   else
   if(P->stage==3) {

          status=Program_emb_ToLine(P->x_direct, P->z_direct, P->a_direct, t2-t1, g_type, P->g) ;
       if(status) {
                                 P->stage     =4 ;
                                 P->dropped   =0 ;
                                 P->check_time=0 ;

                      Program_emb_Log("������ ����") ;
                  }

                   }
/*----------------------------------------------- ����� ����� ������ */

  else 
  if(P->stage==4) do {
/*- - - - - - - - - - - - - - - - - - - - - �������� �� ������ ����� */
           Program_emb_GetTargets (P->stage_4_radar,  &targets, &targets_cnt, error) ;

         if(targets_cnt==0) {                                       /* ���� ���� ��������... */
                               P->stage=1 ;
                                 break ;
                            }

           Program_emb_DetectOrder(targets, targets_cnt,
                                   &orders, &orders_cnt, P->column_spacing, error) ;

         if(orders_cnt==0) {                                       /* ���� ���� ��������... */
                             free(targets) ;
                               P->stage=1 ;
                                 break ;
                           }
        
          Program_emb_GetGlobalXYZ(&orders[0], &order_abs) ;
          Program_emb_ToPoint     (order_abs.x, order_abs.z, t2-t1, g_type, P->g) ;

       distance=Program_emb_Distance(order_abs.x, order_abs.z) ;
/*- - - - - - - - - - - - - - - - - - - - - ������� �� ������ ������ */
    if(distance<P->jump_distance) {

      if(EventObject->y_base > P->drop_height)  P->e_direct= 0 ;
      else                                      P->e_direct=P->e_max ;       
                                  }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  ����� */
    if(distance<P->drop_distance) {

      if(P->dropped                     == 0 ||
         P->check_time+P->drops_interval<=t1   ) {

                    sprintf(text, "����� %d", P->dropped+1) ;
            Program_emb_Log(text) ;

                    sprintf(text, "start %s;", P->drops[P->dropped].weapon) ;
                     strcat(callback, text) ;

                      P->dropped++ ;
                      P->check_time=t1 ;
                                                 }
      if(P->dropped==P->drops_number) {

         Program_emb_Log("���� �� ����") ;

                     P->stage= 5 ;

                     P->check_time= t1 ;
                     P->a_direct  = EventObject->a_azim+120. ;
                     P->e_direct  =-P->e_max ;
                                      }

                                  }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                     } while(0) ;
/*----------------------------------------------------- ���� �� ���� */

  else 
  if(P->stage==5) {


     if(P->stage_4_event[0]           != 0 && 
        P->stage_4_event_done         == 0 && 
        P->stage_4_after+P->check_time<=t1    ) {

             P->stage_4_event_done=1 ;

                   strcat(callback, P->stage_4_event) ;
                   strcat(callback, ";") ;
                                                }

       status=Program_emb_Turn("A", P->a_direct, t2-t1, g_type, P->g) ;

                  }
/*------------------------ ����������� ����� ������������ ���������� */

   else            {
                       SEND_ERROR("EmbededColumnHunter - Unknown stage") ;
                                      return(-1) ;
                   }
/*-------------------------------------------------------------------*/


#undef  P

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

     RSS_Transit_Program::RSS_Transit_Program(void)

{
}


/*********************************************************************/
/*								     */
/*	        ���������� ������ "������� ���������"      	     */

    RSS_Transit_Program::~RSS_Transit_Program(void)

{
}


/********************************************************************/
/*								    */
/*	              ���������� ��������                           */

    int  RSS_Transit_Program::vExecute(void)

{
  RSS_Unit_Program *unit ;
              RECT  rect ;
               int  x ; 
               int  y ; 
               int  w_x ; 
               int  w_y ; 


       unit=(RSS_Unit_Program *)this->object ;

   if(!stricmp(action, "INDICATOR")) {

       unit->hWnd=CREATE_DIALOG(GetModuleHandle(NULL),
                                (LPCDLGTEMPLATE)unit->Module->Resource("IDD_DEBUG", RT_DIALOG),
	                         NULL, Unit_Program_Show_dialog, 
                                       (LPARAM)object) ;

                    GetWindowRect(unit->hWnd, &rect);
                              w_x=rect.bottom-rect.top +1 ;
                              w_y=rect.right -rect.left+1 ;

                                 x= 0 ;
                                 y= 0 ;
         if(details[0]=='1') {   x=  w_x+  (RSS_Kernel::display.x-4*w_x)/3 ;
                                 y= 0 ;                                       }
         if(details[0]=='2') {   x=2*w_x+2*(RSS_Kernel::display.x-4*w_x)/3 ;
                                 y= 0 ;                                       }
         if(details[0]=='3') {   x= RSS_Kernel::display.x-w_x ;
                                 y= 0 ;                                       }
         if(details[0]=='4') {   x= RSS_Kernel::display.x-w_x ;
                                 y=(RSS_Kernel::display.y-w_y)/2 ;            }
         if(details[0]=='5') {   x= RSS_Kernel::display.x-w_x ;
                                 y= RSS_Kernel::display.y-w_y ;               }
         if(details[0]=='6') {   x=2*w_x+2*(RSS_Kernel::display.x-4*w_x)/3 ;
                                 y= RSS_Kernel::display.y-w_y ;               }
         if(details[0]=='7') {   x=  w_x+  (RSS_Kernel::display.x-4*w_x)/3 ;
                                 y= RSS_Kernel::display.y-w_y ;               }
         if(details[0]=='8') {   x= 0 ;
                                 y= RSS_Kernel::display.y-w_y ;               }
         if(details[0]=='9') {   x= 0 ;
                                 y=(RSS_Kernel::display.y-w_y)/2 ;            }


                     SetWindowPos(unit->hWnd, NULL, x, y, 0, 0,
                                   SWP_NOOWNERZORDER | SWP_NOSIZE);
                       ShowWindow(unit->hWnd, SW_SHOW) ;

                         SetFocus(ProgramModule.kernel_wnd) ;

                                     }

   return(0) ;
}


/*********************************************************************/
/*								     */
/*	                     ������� DCL        	             */
/*								     */
/*********************************************************************/

/********************************************************************/
/*								    */
/*               ������ ����������� ��������� � ���������           */

  Dcl_decl *Program_dcl_Message(Lang_DCL  *dcl_kernel,
                                Dcl_decl **pars, 
                                     int   pars_cnt)
{
   char  text[1024] ;

 static     char  chr_value[512] ;          /* ����� ������ */
 static Dcl_decl  chr_return={ _CHR_PTR, 0,0,0,"", chr_value, NULL, 0, 512} ;

#define  _LOG_MAX   100

/*---------------------------------------------------- ������������� */

                 memset(chr_value, 0, sizeof(chr_value)) ;
                        chr_return.size=0 ;

/*-------------------------------------------- ���������� ���������� */

       if(pars_cnt     !=1   ||                                     /* ��������� ����� ���������� */
	  pars[0]->addr==NULL  ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&chr_return) ; 
                                 }

                    memset(text, 0, sizeof(text)) ;                 /* ��������� ������ �� ���� */
        if(pars[0]->size>=sizeof(text))
                    memcpy(text, pars[0]->addr, sizeof(text)-1) ;
        else        memcpy(text, pars[0]->addr, pars[0]->size) ;

/*------------------------------------------------- ������ ��������� */

               SEND_ERROR(text) ;

/*-------------------------------------------------------------------*/

  return(&chr_return) ;
}


/********************************************************************/
/*								    */
/*                   ��������� ������ � ���������� ���              */

  Dcl_decl *Program_dcl_Log( Lang_DCL  *dcl_kernel,
                             Dcl_decl **pars, 
                                  int   pars_cnt)
{
   char  text[1024] ;

 static     char  chr_value[512] ;          /* ����� ������ */
 static Dcl_decl  chr_return={ _CHR_PTR, 0,0,0,"", chr_value, NULL, 0, 512} ;

#define  _LOG_MAX   100

/*---------------------------------------------------- ������������� */

                 memset(chr_value, 0, sizeof(chr_value)) ;
                        chr_return.size=0 ;

/*-------------------------------------------- ���������� ���������� */

       if(pars_cnt     !=1   ||                                     /* ��������� ����� ���������� */
	  pars[0]->addr==NULL  ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&chr_return) ; 
                                 }

                    memset(text, 0, sizeof(text)) ;                 /* ��������� ������ �� ���� */
        if(pars[0]->size>=sizeof(text))
                    memcpy(text, pars[0]->addr, sizeof(text)-1) ;
        else        memcpy(text, pars[0]->addr, pars[0]->size) ;

/*----------------------------------------------------- ������ � ��� */

                       Program_emb_Log(text) ;

/*-------------------------------------------------------------------*/

  return(&chr_return) ;
}


  int  Program_emb_Log(char *text)
{
   HWND  hDlg ;
    int  rows_cnt ;
    int  i ;

#define  _LOG_MAX   100

/*----------------------------------------------------- ������ � ��� */

     if(EventUnit!=NULL) {
                                hDlg=EventUnit->hWnd ;

            rows_cnt=LB_GET_COUNT(IDC_LOG) ;                        /* ������� ���� */
         if(rows_cnt>_LOG_MAX) {
               for(i=0 ; i<rows_cnt-_LOG_MAX ; i++)  LB_DEL_ROW(IDC_LOG, i) ;
                               }

           LB_ADD_ROW(IDC_LOG, text) ;
           LB_TOP_ROW(IDC_LOG, LB_GET_COUNT(IDC_LOG)-1) ;

                         }
/*-------------------------------------------------------------------*/

#undef  _LOG_MAX

  return(0) ;
}


/********************************************************************/
/*								    */
/*                  ����������� ��������� �� �����                  */
/*								    */
/*   ���������: X, Z ������� �����                                  */

  Dcl_decl *Program_dcl_Distance( Lang_DCL  *dcl_kernel,
                                  Dcl_decl **pars, 
                                       int   pars_cnt)
{
     double  x_target ;
     double  z_target ;

 static   double  dgt_value ;          /* ����� ��������� �������� */
 static Dcl_decl  dgt_return={ _DGT_VAL, 0,0,0,"", &dgt_value, NULL, 1, 1} ;

/*---------------------------------------------------- ������������� */

                        dgt_value=0. ;

/*-------------------------------------------- ���������� ���������� */

       if(pars[0]->addr==NULL ||                                    /* ��������� ������� �������� ���������� */
	  pars[1]->addr==NULL   ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                                  }
        
          x_target=dcl_kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;
          z_target=dcl_kernel->iDgt_get(pars[1]->addr, pars[1]->type) ;

/*------------------------------------------------- ������ ��������� */

         dgt_value=Program_emb_Distance(x_target, z_target) ;

/*-------------------------------------------------------------------*/

  return(&dgt_return) ;
}


  double  Program_emb_Distance(double  x, double  z)

{
   return( sqrt((x-EventObject->x_base)*(x-EventObject->x_base)+
                (z-EventObject->z_base)*(z-EventObject->z_base) ) ) ;
}


/********************************************************************/
/*								    */
/*                  ��������� ����������� ��������                  */
/*								    */
/*   ���������: ���, ����, [����� �������, ��������� ������]       */
/*                                                                  */
/*                                                                  */
/*  ���: A - ������, E - ������, R - ��������                       */
/*  ��������� ������ ������ ����������� �������� ��������:         */
/*      <��� �����������> <�������� �����������>                    */
/*                                                                  */
/*      <��� �����������>: R - �� �������, G - �� ����������        */
/*      <�������� �����������>: ������ �������� � ������,           */
/*                              ���������� � �������� g             */

  Dcl_decl *Program_dcl_Turn( Lang_DCL  *dcl_kernel,
                              Dcl_decl **pars, 
                                   int   pars_cnt)
{
       char  elem[32] ;
     double  angle ;
     double  dt ;
       char  limit_type[16] ;
     double  g ;
     double  rad ;
     double  v ;
     double  dr ;
     double  angle_diff ;
     double  da ;
     double  l ;
  RSS_Point  target ;
       char *target_pars ;
    char  cmd[1024] ;

 static   double  dgt_value ;          /* ����� ��������� �������� */
 static Dcl_decl  dgt_return={ _DGT_VAL, 0,0,0,"", &dgt_value, NULL, 1, 1} ;

#define  _LOG_MAX   100

/*---------------------------------------------------- ������������� */

                        dgt_value=0. ;

/*-------------------------------------------- ���������� ���������� */

       if(pars_cnt!=2 &&                                            /* ��������� ����� ���������� */
	  pars_cnt!=5   ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                          }

       if(pars[0]->addr==NULL ||                                    /* ��������� ������� �������� ���������� */
	  pars[1]->addr==NULL   ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                                  }

      if(pars_cnt==5)                                               /* ��������� ������� ���������� ������ */
       if(pars[2]->addr==NULL ||
	  pars[3]->addr==NULL ||
	  pars[4]->addr==NULL   ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                                  }


                    memset(elem, 0, sizeof(elem)) ;                 /* ��������� ��� ���������� */
        if(pars[0]->size>=sizeof(elem))
                    memcpy(elem, pars[0]->addr, sizeof(elem)-1) ;
        else        memcpy(elem, pars[0]->addr, pars[0]->size) ;

          angle=dcl_kernel->iDgt_get(pars[1]->addr, pars[1]->type) ;

                    memset(limit_type, 0, sizeof(limit_type)) ;

     if(pars_cnt==5) {

                dt=dcl_kernel->iDgt_get(pars[2]->addr, pars[2]->type) ;

        if(pars[3]->size>=sizeof(limit_type))
                    memcpy(limit_type, pars[3]->addr, sizeof(limit_type)-1) ;
        else        memcpy(limit_type, pars[3]->addr, pars[3]->size) ;

               rad=dcl_kernel->iDgt_get(pars[4]->addr, pars[4]->type) ;
                 g=rad ;
                     }
/*---------------------------------------------- �������� ���������� */

                strupr(elem) ;

    if(limit_type[0]==0) {
      if(stricmp(elem, "A") &&
         stricmp(elem, "E") &&
         stricmp(elem, "R")   ) {
                                  dcl_kernel->mError_code=_DCLE_USER_DEFINED ;
                           strcpy(dcl_kernel->mError_details, "Unsupported manoeuvre. Must by A, E or R.") ;
                                    return(&dgt_return) ;
                                }
                          }
    else                  {
      if(stricmp(elem, "A")) {
                                  dcl_kernel->mError_code=_DCLE_USER_DEFINED ;
                           strcpy(dcl_kernel->mError_details, "Unsupported manoeuvre. Must by A.") ;
                                    return(&dgt_return) ;
                             }
                          }

                strupr(limit_type) ;

     if(limit_type[0]!='R' &&
        limit_type[0]!='G'   ) {
                                  dcl_kernel->mError_code=_DCLE_USER_DEFINED ;
                           strcpy(dcl_kernel->mError_details, "Unknown type of manoeuvre's limit. Must by R or G") ;
                                    return(&dgt_return) ;
                               }

     if(limit_type[0]=='R' && rad<=0. ||
        limit_type[0]=='G' &&   g<=0.   ) {
                                  dcl_kernel->mError_code=_DCLE_USER_DEFINED ;
                           strcpy(dcl_kernel->mError_details, "invalid limit of manoeuvre. Must by >0") ;
                                    return(&dgt_return) ;
                                          } 
/*------------------------------------------ ��� ����������� ������ */

  if(limit_type[0]==0) {

    if(!stricmp(elem, "A") ||
       !stricmp(elem, "E") ||
       !stricmp(elem, "R")   ) {
                                 sprintf(cmd, "%s angle/%c %s %lf",
                                               EventObject->Type, elem[0], EventObject->Name, angle) ;
        EventObject->Module->vExecuteCmd(cmd) ;

                                            dgt_value=1. ;
                                    return(&dgt_return) ;
                               }  
                       }
/*----------------------------------------- ������� ������� �������� */

            v=sqrt(EventObject->x_velocity*EventObject->x_velocity+
                   EventObject->z_velocity*EventObject->z_velocity ) ;

    if(limit_type[0]=='G') {
                               rad=v*v/(10.*g) ;
                           }
/*------------------------------------------- � ������������ ������ */

#pragma warning(disable : 4701)

           dr=dt*v ;
           da=_RAD_TO_GRD*dt*v/rad ;

                                angle_diff =angle-EventObject->a_azim ;
       while(angle_diff> 180.)  angle_diff-=360. ;
       while(angle_diff<-180.)  angle_diff+=360. ;

       if(fabs(angle_diff)<da) {
                                     da=fabs(angle_diff) ;
                         if(da>1.)  rad=dt*v/(_GRD_TO_RAD*da) ;
                                      g= 0. ;

                                        dgt_value=1. ;
                               }

       if(da<5.) {
                   l=dt*v ;
                 }
       else      {
                    l=rad*sin(_GRD_TO_RAD*(da))/sin(_GRD_TO_RAD*(90.-da/2.)) ;
                 } 

       if(angle_diff<0.)  da=-da ;

                            target.x   =EventObject->x_base+l*sin(_GRD_TO_RAD*(EventObject->a_azim+da/2.)) ;
                            target.z   =EventObject->z_base+l*cos(_GRD_TO_RAD*(EventObject->a_azim+da/2.)) ;
                            target.azim=EventObject->a_azim+da ;
                  if(da>0)  target.roll= atan(g)*_RAD_TO_GRD ;
                  else      target.roll=-atan(g)*_RAD_TO_GRD ;
                            target_pars="XZAR" ;

          EventObject->vCalculateDirect(&target, target_pars) ;

#pragma warning(default : 4701)

/*-------------------------------------------------------------------*/

  return(&dgt_return) ;
}


  int  Program_emb_Turn(  char *elem, 
                        double  angle,
                        double  dt,
                          char *limit_type,
                        double  g           )
{
        int  result ;
     double  rad ;
     double  v ;
     double  dr ;
     double  angle_diff ;
     double  da ;
     double  l ;
  RSS_Point  target ;
       char *target_pars ;
       char  cmd[1024] ;

/*---------------------------------------------------- ������������� */

                           result=0 ;

/*------------------------------------------ ��� ����������� ������ */

  if(limit_type[0]==0) {

    if(!stricmp(elem, "A") ||
       !stricmp(elem, "E") ||
       !stricmp(elem, "R")   ) {
                                 sprintf(cmd, "%s angle/%c %s %lf",
                                               EventObject->Type, elem[0], EventObject->Name, angle) ;
        EventObject->Module->vExecuteCmd(cmd) ;

                                    return(1) ;
                               }  
                       }
/*----------------------------------------- ������� ������� �������� */

            v=sqrt(EventObject->x_velocity*EventObject->x_velocity+
                   EventObject->z_velocity*EventObject->z_velocity ) ;

    if(limit_type[0]=='G') {
                               rad=v*v/(10.*g) ;
                           }
/*------------------------------------------- � ������������ ������ */

#pragma warning(disable : 4701)

           dr=dt*v ;
           da=_RAD_TO_GRD*dt*v/rad ;

                                angle_diff =angle-EventObject->a_azim ;
       while(angle_diff> 180.)  angle_diff-=360. ;
       while(angle_diff<-180.)  angle_diff+=360. ;

       if(fabs(angle_diff)<da) {
                                     da=fabs(angle_diff) ;
                         if(da>1.)  rad=dt*v/(_GRD_TO_RAD*da) ;
                                      g= 0. ;

                                        result=1 ;
                               }

       if(da<5.) {
                   l=dt*v ;
                 }
       else      {
                    l=rad*sin(_GRD_TO_RAD*(da))/sin(_GRD_TO_RAD*(90.-da/2.)) ;
                 } 

       if(angle_diff<0.)  da=-da ;

                            target.x   =EventObject->x_base+l*sin(_GRD_TO_RAD*(EventObject->a_azim+da/2.)) ;
                            target.z   =EventObject->z_base+l*cos(_GRD_TO_RAD*(EventObject->a_azim+da/2.)) ;
                            target.azim=EventObject->a_azim+da ;
                  if(da>0)  target.roll= atan(g)*_RAD_TO_GRD ;
                  else      target.roll=-atan(g)*_RAD_TO_GRD ;
                            target_pars="XZAR" ;

          EventObject->vCalculateDirect(&target, target_pars) ;

#pragma warning(default : 4701)

/*-------------------------------------------------------------------*/


  return(result) ;
}


/********************************************************************/
/*								    */
/*                         �������� � �����                         */
/*								    */
/*   ���������: X, Z, ����� �������, [��������� ������]            */
/*                                                                  */
/*  ��������� ������ ������ ����������� �������� ��������:         */
/*      <��� �����������> <�������� �����������>                    */
/*                                                                  */
/*      <��� �����������>: R - �� �������, G - �� ����������        */
/*      <�������� �����������>: ������ �������� � ������,           */
/*                              ���������� � �������� g             */

  Dcl_decl *Program_dcl_ToPoint( Lang_DCL  *dcl_kernel,
                                 Dcl_decl **pars, 
                                      int   pars_cnt)
{
     double  x_target ;
     double  z_target ;
     double  dt ;
       char  limit_type[16] ;
     double  rad ;

 static   double  dgt_value ;          /* ����� ��������� �������� */
 static Dcl_decl  dgt_return={ _DGT_VAL, 0,0,0,"", &dgt_value, NULL, 1, 1} ;

/*---------------------------------------------------- ������������� */

                        dgt_value=0. ;

/*-------------------------------------------- ���������� ���������� */

       if(pars_cnt!=3 &&                                            /* ��������� ����� ���������� */
	  pars_cnt!=5   ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                          }

       if(pars[0]->addr==NULL ||                                    /* ��������� ������� �������� ���������� */
	  pars[1]->addr==NULL ||
	  pars[2]->addr==NULL   ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                                  }

      if(pars_cnt==5)                                               /* ��������� ������� ���������� ������ */
       if(pars[3]->addr==NULL ||
	  pars[4]->addr==NULL   ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                                  }
         

          x_target=dcl_kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;
          z_target=dcl_kernel->iDgt_get(pars[1]->addr, pars[1]->type) ;
                dt=dcl_kernel->iDgt_get(pars[2]->addr, pars[2]->type) ;

                         memset(limit_type, 0, sizeof(limit_type)) ;

     if(pars_cnt==5) {

        if(pars[3]->size>=sizeof(limit_type))
                    memcpy(limit_type, pars[3]->addr, sizeof(limit_type)-1) ;
        else        memcpy(limit_type, pars[3]->addr, pars[3]->size) ;

               rad=dcl_kernel->iDgt_get(pars[4]->addr, pars[4]->type) ;
                     }
/*---------------------------------------------- �������� ���������� */

                strupr(limit_type) ;

     if(limit_type[0]!='R' &&
        limit_type[0]!='G'   ) {
                                  dcl_kernel->mError_code=_DCLE_USER_DEFINED ;
                           strcpy(dcl_kernel->mError_details, "Unknown type of manoeuvre's limit. Must by R or G") ;
                                    return(&dgt_return) ;
                               }

     if(limit_type[0]=='R' && rad<=0. ||
        limit_type[0]=='G' && rad<=0.   ) {
                                  dcl_kernel->mError_code=_DCLE_USER_DEFINED ;
                           strcpy(dcl_kernel->mError_details, "invalid limit of manoeuvre. Must by >0") ;
                                    return(&dgt_return) ;
                                          } 
/*--------------------------------------------- ��������� ���������� */

    dgt_value=Program_emb_ToPoint(x_target, z_target, dt, limit_type, rad) ;

/*-------------------------------------------------------------------*/

  return(&dgt_return) ;
}

  int Program_emb_ToPoint(double  x_target,
                          double  z_target,
                          double  dt,
                            char *limit_type,
                          double  g          )
{
     double  rad ;
     double  v ;
     double  r ;
     double  dr ;
     double  angle ;
     double  angle_diff ;
     double  da ;
     double  l ;
  RSS_Point  target ;
       char *target_pars ;

/*---------------------------------------------------- ������������� */

                              rad=g ;

/*-------------------------------- �������� ���������� ������� ����� */

        r=sqrt((x_target-EventObject->x_base)*(x_target-EventObject->x_base)+
               (z_target-EventObject->z_base)*(z_target-EventObject->z_base) ) ;

        v=sqrt(EventObject->x_velocity*EventObject->x_velocity+
               EventObject->z_velocity*EventObject->z_velocity ) ;
       dr=dt*v ;

    if(dr>r) {
                 target.x   =EventObject->x_base+dt*EventObject->x_velocity ;
                 target.z   =EventObject->z_base+dt*EventObject->z_velocity ;
                 target.azim=EventObject->a_azim ;
                 target.roll= 0. ;
                 target_pars="XZAR" ;

          EventObject->vCalculateDirect(&target, target_pars) ;

                            return(1) ;
             }
/*----------------------------------------- ������� ������� �������� */

    if(limit_type[0]=='G') {
                               rad=v*v/(10.*g) ;
                           }
/*----------------------------------- ��������� ����������� �������� */

          angle=_RAD_TO_GRD*atan2(x_target-EventObject->x_base, 
                                  z_target-EventObject->z_base ) ;
/*- - - - - - - - - - - - - - - - - - - - - - � ������������ ������ */
   if(limit_type[0]!=0) {
                                        da =_RAD_TO_GRD*dt*v/rad ;

                                angle_diff =angle-EventObject->a_azim ;
       while(angle_diff> 180.)  angle_diff-=360. ;
       while(angle_diff<-180.)  angle_diff+=360. ;

       if(fabs(angle_diff)<da) {
                                     da=fabs(angle_diff) ;
                                    rad=dt*v/(_GRD_TO_RAD*da) ;
                                      g=v*v/(10.*rad) ;
                               }

       if(da<5.) {
                   l=dt*v ;
                 }
       else      {
                    l=rad*sin(_GRD_TO_RAD*(da))/sin(_GRD_TO_RAD*(90.-da/2.)) ;
                 } 

       if(angle_diff<0.)  da=-da ;

                            target.x   = EventObject->x_base+l*sin(_GRD_TO_RAD*(EventObject->a_azim+da/2.)) ;
                            target.z   = EventObject->z_base+l*cos(_GRD_TO_RAD*(EventObject->a_azim+da/2.)) ;
                            target.azim= EventObject->a_azim+da ;
                  if(da>0)  target.roll= atan(g)*_RAD_TO_GRD ;
                  else      target.roll=-atan(g)*_RAD_TO_GRD ;
                            target_pars= "XZAR" ;
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - "����������" ������� */
   else                 {
                            target.azim=angle ;
                            target_pars="A" ;
                        }
/*- - - - - - - - - - - - - - - - - - - - ������� �������� ��������� */
          EventObject->vCalculateDirect(&target, target_pars) ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*                         ����� �� �����                           */
/*								    */
/*   ���������: X, Z, ������, ����� �������, [��������� ������]    */
/*                                                                  */
/*  ��������� ������ ������ ����������� �������� ��������:         */
/*      <��� �����������> <�������� �����������>                    */
/*                                                                  */
/*      <��� �����������>: R - �� �������, G - �� ����������        */
/*      <�������� �����������>: ������ �������� � ������,           */
/*                              ���������� � �������� g             */

  Dcl_decl *Program_dcl_ToLine( Lang_DCL  *dcl_kernel,
                                Dcl_decl **pars, 
                                     int   pars_cnt)
{
     double  x_target ;
     double  z_target ;
     double  a_target ;
     double  dt ;
       char  limit_type[16] ;
     double  g ;
     double  rad ;

 static   double  dgt_value ;          /* ����� ��������� �������� */
 static Dcl_decl  dgt_return={ _DGT_VAL, 0,0,0,"", &dgt_value, NULL, 1, 1} ;


#define  _LOG_MAX   100

/*---------------------------------------------------- ������������� */

                        dgt_value=0. ;

/*-------------------------------------------- ���������� ���������� */

       if(pars_cnt!=4 &&                                            /* ��������� ����� ���������� */
	  pars_cnt!=6   ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                          }

       if(pars[0]->addr==NULL ||                                    /* ��������� ������� �������� ���������� */
	  pars[1]->addr==NULL ||
	  pars[2]->addr==NULL ||
	  pars[3]->addr==NULL   ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                                  }

      if(pars_cnt==6)                                               /* ��������� ������� ���������� ������ */
       if(pars[4]->addr==NULL ||
	  pars[5]->addr==NULL   ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                                  }
         

          x_target=dcl_kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;
          z_target=dcl_kernel->iDgt_get(pars[1]->addr, pars[1]->type) ;
          a_target=dcl_kernel->iDgt_get(pars[2]->addr, pars[2]->type) ;
                dt=dcl_kernel->iDgt_get(pars[3]->addr, pars[3]->type) ;

                         memset(limit_type, 0, sizeof(limit_type)) ;

     if(pars_cnt==6) {

        if(pars[4]->size>=sizeof(limit_type))
                    memcpy(limit_type, pars[4]->addr, sizeof(limit_type)-1) ;
        else        memcpy(limit_type, pars[4]->addr, pars[4]->size) ;

               rad=dcl_kernel->iDgt_get(pars[5]->addr, pars[5]->type) ;
                 g=rad ;
                     }
/*---------------------------------------------- �������� ���������� */

                strupr(limit_type) ;

     if(limit_type[0]!='R' &&
        limit_type[0]!='G'   ) {
                                  dcl_kernel->mError_code=_DCLE_USER_DEFINED ;
                           strcpy(dcl_kernel->mError_details, "Unknown type of manoeuvre's limit. Must by R or G") ;
                                    return(&dgt_return) ;
                               }

     if(limit_type[0]=='R' && rad<=0. ||
        limit_type[0]=='G' &&   g<=0.   ) {
                                  dcl_kernel->mError_code=_DCLE_USER_DEFINED ;
                           strcpy(dcl_kernel->mError_details, "invalid limit of manoeuvre. Must by >0") ;
                                    return(&dgt_return) ;
                                          } 
/*--------------------------------------------------- ����� �� ����� */

     dgt_value=Program_emb_ToLine(x_target, z_target,
                                  a_target, dt, limit_type, g) ;

/*-------------------------------------------------------------------*/

  return(&dgt_return) ;
}

  int  Program_emb_ToLine(double  x_target, double  z_target, 
                          double  a_target, double  dt, char *limit_type, double  g)
{
     double  rad ;
     double  r, dr ;
     double  v ;
     double  angle, da, angle_diff ;
     double  x1_work, x2_work ;
     double  z1_work, z2_work ;
     double  r1, r2 ;
     double  l ;
  RSS_Point  target ;
       char *target_pars ;

/*---------------------------------------------------- ������������� */

                              rad=g ;

/*---------------------------- ������ ������������� ����� ���������� */

         r=sqrt((x_target-EventObject->x_base)*(x_target-EventObject->x_base)+
                (z_target-EventObject->z_base)*(z_target-EventObject->z_base) ) ;

     angle=atan(fabs((x_target-EventObject->x_base)/(z_target-EventObject->z_base))) ;
        r*=fabs(cos(angle+_GRD_TO_RAD*a_target)) ; 

   x1_work=x_target+r*sin(_GRD_TO_RAD*      a_target ) ;
   z1_work=z_target+r*cos(_GRD_TO_RAD*      a_target ) ;
   x2_work=x_target+r*sin(_GRD_TO_RAD*(180.+a_target)) ;
   z2_work=z_target+r*cos(_GRD_TO_RAD*(180.+a_target)) ;

        r1=sqrt((x1_work-EventObject->x_base)*(x1_work-EventObject->x_base)+
                (z1_work-EventObject->z_base)*(z1_work-EventObject->z_base) ) ;
        r2=sqrt((x2_work-EventObject->x_base)*(x2_work-EventObject->x_base)+
                (z2_work-EventObject->z_base)*(z2_work-EventObject->z_base) ) ;
 
   if(r1>r2) {  r1     =r2 ;
                x1_work=x2_work ;
                z1_work=z2_work ;  }

/*---------------------- ����������� �������� � ���������� ��������� */

        v=sqrt(EventObject->x_velocity*EventObject->x_velocity+
               EventObject->z_velocity*EventObject->z_velocity ) ;
       dr=dt*v ;

/*------------------------------------------ ��� ����������� ������ */

   if(limit_type[0]==0) {
/*- - - - - - - - - - - - - - - -  �������� ���������� ������� ����� */
    if(dr>r1) {
                 target.x   =EventObject->x_base+dr*sin(_GRD_TO_RAD*EventObject->a_azim) ;
                 target.z   =EventObject->z_base+dt*cos(_GRD_TO_RAD*EventObject->a_azim) ;
                 target.azim=_RAD_TO_GRD*atan2(x_target-target.x, z_target-target.z) ;
                 target.roll=  0 ;
                 target_pars="XZAR" ;

          EventObject->vCalculateDirect(&target, target_pars) ;

                return(1) ;
              }

                   target.azim=_RAD_TO_GRD*atan2(x1_work-EventObject->x_base, z1_work-EventObject->z_base) ;
                   target_pars="A" ;

          EventObject->vCalculateDirect(&target, target_pars) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                              return(0) ;
                        } 
/*----------------------------------------- ������� ������� �������� */

    if(limit_type[0]=='G') {
                               rad=v*v/(10.*g) ;
                           }
/*---------------------------------------- ����������� ������� ����� */

    if(r1>rad) {
                   x_target=x1_work ;
                   z_target=z1_work ;
               }
    else       {
               }
/*----------------------------------- ��������� ����������� �������� */

          angle=_RAD_TO_GRD*atan2(x_target-EventObject->x_base, 
                                  z_target-EventObject->z_base ) ;

                                        da =_RAD_TO_GRD*dt*v/rad ;

                                angle_diff =angle-EventObject->a_azim ;
       while(angle_diff> 180.)  angle_diff-=360. ;
       while(angle_diff<-180.)  angle_diff+=360. ;

       if(fabs(angle_diff)<da) {
                                     da=fabs(angle_diff) ;
                                    rad=dt*v/(_GRD_TO_RAD*da) ;
                                      g=v*v/(10.*rad) ;
                               }

       if(da<5.) {
                   l=dt*v ;
                 }
       else      {
                    l=rad*sin(_GRD_TO_RAD*(da))/sin(_GRD_TO_RAD*(90.-da/2.)) ;
                 } 

       if(angle_diff<0.)  da=-da ;

                            target.x   =EventObject->x_base+l*sin(_GRD_TO_RAD*(EventObject->a_azim+da/2.)) ;
                            target.z   =EventObject->z_base+l*cos(_GRD_TO_RAD*(EventObject->a_azim+da/2.)) ;
                            target.azim=EventObject->a_azim+da ;
                  if(da>0)  target.roll= atan(g)*_RAD_TO_GRD ;
                  else      target.roll=-atan(g)*_RAD_TO_GRD ;
                            target_pars="XZAR" ;

          EventObject->vCalculateDirect(&target, target_pars) ;

/*-------------------------------- �������� ���������� ������� ����� */

                          angle =fabs(target.azim-a_target) ;
        while(angle>180)  angle-= 180. ;

          if(angle<1.)  return(1) ;

/*-------------------------------------------------------------------*/

  return(0) ;
}



/*********************************************************************/
/*                                                                   */
/*                  ���������� ������� �������                       */

   Dcl_decl *Program_dcl_StateSave(Lang_DCL  *dcl_kernel,
                                   Dcl_decl  *source, 
                                   Dcl_decl **pars, 
                                        int   pars_cnt)

{
                char  slot_name[1024] ;
  Dcl_complex_record *record ;
                 int  type ;          /* �������� ��� ���������� */
                char  tmp[1024] ;
                char *ptr ;
                char *end ;
                 int  i ;
                 int  k ;

        static  char *slot ;
        static  char *buff ;
#define                _BUFF_SIZE  64000

 static   double  dgt_value ;          /* ����� ��������� �������� */
 static Dcl_decl  dgt_return={ _DGT_VAL, 0,0,0,"", &dgt_value, NULL, 1, 1} ;

/*---------------------------------------------------- ������������� */

     if(buff==NULL   )  buff=(char *)calloc(1, _BUFF_SIZE) ;
     if(slot==NULL   )  slot=(char *)calloc(1, _BUFF_SIZE) ;
     if(buff==NULL ||
        slot==NULL   ) {
                         dcl_kernel->mError_code=_DCLE_USER_DEFINED ;
                  strcpy(dcl_kernel->mError_details, "Memory over") ;
                               return(&dgt_return) ;
                       }
   
                              dgt_value=0 ;

/*-------------------------------------------- ���������� ���������� */

           strcpy(slot_name, "Unknown") ;

       if(pars_cnt     == 1   &&                                    /* ��������� ����� ���������� */
	  pars[0]->addr!=NULL   ) {

                    memset(slot_name, 0, sizeof(slot_name)) ;       /* ��������� ��� ����� ������ */
        if(pars[0]->size>=sizeof(slot_name))
                    memcpy(slot_name, pars[0]->addr, sizeof(slot_name)-1) ;
        else        memcpy(slot_name, pars[0]->addr, pars[0]->size) ;

                                  }
/*-------------------------------------------------- �������� ������ */

          sprintf(slot, "SLOT %s{\n", slot_name) ;

     for(record=(Dcl_complex_record *)source->addr,                 /* LOOP - ���������� ������ */
                        i=0 ; i<source->buff ; i++, 
         record=(Dcl_complex_record *)record->next_record) {

          for(k=0 ; k<record->elems_cnt ; k++) {                    /* LOOP - ���������� �������� */

               type=t_base(record->elems[k].type) ;                 /* ��������� ��� �������� */ 
            if(type==_CHR_AREA ||                                   /* ���� �������� ���������� */
               type==_CHR_PTR    ) {
                                      strcat(slot, (char *)record->elems[k].addr) ;
                                   }
            else                   {                                /* ���� �������� ���������� */

                       dcl_kernel->iNumToStr(&record->elems[k], tmp) ;
                                      strcat(slot, tmp) ;
                                   }

                                      strcat(slot, ";") ;
 
                                               }                    /* ENDLOOP - ���������� �������� */

                                      strcat(slot, "\n") ;          /* ��������� ������ */ 
                                                           }

          strcat(slot, "}\n") ;

/*------------------------------------------------ ���������� ������ */

     if(EventUnit->state!=NULL)  strcpy(buff, EventUnit->state) ;
     else                        strcpy(buff, "") ;

/*---------------------------------------------- ������ ����� ������ */

         sprintf(tmp, "SLOT %s{\n", slot_name) ;

      ptr=strstr(buff, tmp) ;
   if(ptr!=NULL) {
         end=strchr(ptr, '}') ;
            memmove(ptr, end+1, strlen(end+1)+1) ;
                 }

          strcat(buff, slot) ;

/*------------------------------------------------ ���������� ������ */

           EventUnit->state=(char *)realloc(EventUnit->state, strlen(buff)+8) ;
    strcpy(EventUnit->state, buff) ;

/*-------------------------------------------------------------------*/

#undef   _BUFF_SIZE

  return(&dgt_return) ;
}


/*********************************************************************/
/*                                                                   */
/*                  ���������� ������� �������                       */

   Dcl_decl *Program_dcl_StateRead(Lang_DCL  *dcl_kernel,
                                   Dcl_decl  *source, 
                                   Dcl_decl **pars, 
                                        int   pars_cnt)

{
#define    _BUFF_SIZE  64000
#define  _VALUES_MAX     100

                char  slot_name[1024] ;
  Dcl_complex_record *record ;
                 int  type ;          /* �������� ��� ���������� */
                 int  status ;
                char *work ;
                char *next ;
                char *ptr ;
                char *end ;
              double  value ;
                char  tmp[1024] ;
                 int  i ;
                 int  k ;

    static      char *buff ;
    static  Dcl_decl *values ;
              double  digits[_VALUES_MAX] ;

 static   double  dgt_value ;          /* ����� ��������� �������� */
 static Dcl_decl  dgt_return={ _DGT_VAL, 0,0,0,"", &dgt_value, NULL, 1, 1} ;

/*---------------------------------------------------- ������������� */

     if(buff==NULL  )  buff=(char *)calloc(1, _BUFF_SIZE) ;
     if(buff==NULL  ) {
                       dcl_kernel->mError_code=_DCLE_USER_DEFINED ;
                strcpy(dcl_kernel->mError_details, "Memory over") ;
                            return(&dgt_return) ;
                      }

     if(values==NULL)  values=(Dcl_decl *)calloc(_VALUES_MAX, sizeof(*values)) ;
     if(values==NULL) {
                       dcl_kernel->mError_code=_DCLE_USER_DEFINED ;
                strcpy(dcl_kernel->mError_details, "Memory over") ;
                            return(&dgt_return) ;
                      }
   
                              dgt_value=0 ;

/*-------------------------------------------- ���������� ���������� */

           strcpy(slot_name, "Unknown") ;

       if(pars_cnt     == 1   &&                                    /* ��������� ����� ���������� */
	  pars[0]->addr!=NULL   ) {

                    memset(slot_name, 0, sizeof(slot_name)) ;       /* ��������� ��� ����� ������ */
        if(pars[0]->size>=sizeof(slot_name))
                    memcpy(slot_name, pars[0]->addr, sizeof(slot_name)-1) ;
        else        memcpy(slot_name, pars[0]->addr, pars[0]->size) ;

                                  }
/*------------------------------------------------ ���������� ������ */

     if(EventUnit->state!=NULL)  strcpy(buff, EventUnit->state) ;
     else                        strcpy(buff, "") ;

/*------------------------------------------- ��������� ����� ������ */

         sprintf(tmp, "SLOT %s{\n", slot_name) ;

      ptr=strstr(buff, tmp) ;
   if(ptr==NULL) {                                                 /* ���� ������ ��� - �������� �������� ������ */
                              dgt_value=1 ;
                      return(&dgt_return) ;
                 }

       ptr+=strlen(tmp) ;
       end =strchr(ptr, '}') ;
    if(end==NULL) {
                              dgt_value=1 ;
                      return(&dgt_return) ;
                  }

       *end= 0 ; 

       memmove(buff, ptr, strlen(ptr)+1) ;

    if(*buff==0) {                                                 /* ���� ������ ��� - �������� �������� ������ */
                              dgt_value=1 ;
                      return(&dgt_return) ;
                 } 
/*------------------------------------------------ ���������� ������ */

              dcl_kernel->iXobject_clear(source) ;                  /* ������� ��������� ��������� */

   for(work=buff, next=buff, i=0 ; ; work=next+1, i++) {            /* LOOP - ������� ����� */

             next=strchr(work, '\n') ;
          if(next==NULL)  break ;
            *next=0 ;

               dcl_kernel->iXobject_add(source, NULL) ;
/*- - - - - - - - - - - - - - - - - - -  ���������� ������ ��������� */
     if(i==0) {
                    record=(Dcl_complex_record *)source->addr ;
        for(k=0 ; k<record->elems_cnt ; k++)  values[k]=record->elems[k] ;
              }
/*- - - - - - - - - - - - - - - - - - - ������ ������ �� ���������� */
      for(k=0 ; k<_VALUES_MAX ; work=end+1, k++) {

             end=strchr(work, ';') ;
          if(end==NULL)  break ;
            *end=0 ;

               values[k].addr=work ;
                                                 }

     if(k>=_VALUES_MAX) {
                          dcl_kernel->mError_code=_DCLE_USER_DEFINED ;
                   strcpy(dcl_kernel->mError_details, "Too many elements in State") ;
                               return(&dgt_return) ;
                        }
/*- - - - - - - - - - - - - - -  �������������� �������� ����������� */
      for(k=0 ; k<record->elems_cnt ; k++) {

               type=t_base(record->elems[k].type) ;                 /* ��������� ��� �������� */ 
            if(type!=_CHR_AREA &&                                   /* ���� �������� ���������� */
               type!=_CHR_PTR    ) {

                          value=strtod((char *)values[k].addr, &end) ;

                  dcl_kernel->iDgt_set(value, &digits[k], values[k].type) ;
                                       values[k].addr=&digits[k] ;
                                   }
            else                   {
                   values[k].size=strlen((char *)values[k].addr) ;
                                   } 
                                           }                                    
/*- - - - - - - - - - - - - - - - - - - - - - -  ������������ ������ */
        status=dcl_kernel->iXobject_set(source, values, record->elems_cnt) ;
     if(status) {
                    dcl_kernel->mError_code=_DCLE_TYPEDEF_ELEM ;
                      return(&dgt_return) ; 
                }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                       }            /* END LOOP - ������� ����� */


    


/*-------------------------------------------------------------------*/

#undef   _BUFF_SIZE

  return(&dgt_return) ;
}


/*********************************************************************/
/*                                                                   */
/*                     ��������� ������ �����                        */

   Dcl_decl *Program_dcl_GetTargets(Lang_DCL  *dcl_kernel,
                                    Dcl_decl  *source, 
                                    Dcl_decl **pars, 
                                         int   pars_cnt)

{
                char  unit_name[1024] ;
     RSS_Unit_Target *targets ;
                 int  targets_cnt ;
                char  t_name[128] ;
                char  t_spec[128] ;
              double  t_x ;
              double  t_y ;
              double  t_z ;
              double  t_a ;
                 int  status ;
                char  text[1024] ;
                 int  i ;

          Dcl_decl  rec_data[6] ={
                                  {_CHR_AREA, 0, 0, 0, "object",           0,           t_name, 128, 128},
                                  {_CHR_AREA, 0, 0, 0, "special", (void *)128,          t_spec, 128, 128},
                                  {_DGT_VAL,  0, 0, 0, "x",       (void *)256, (char *)&t_x,      1,   1},
                                  {_DGT_VAL,  0, 0, 0, "y",       (void *)264, (char *)&t_y,      1,   1},
                                  {_DGT_VAL,  0, 0, 0, "z",       (void *)272, (char *)&t_z,      1,   1},
                                  {_DGT_VAL,  0, 0, 0, "azim",    (void *)280, (char *)&t_a,      1,   1}
                                 } ;
  Dcl_complex_type  rec_template={ "target", 288, rec_data, 6} ;

 static   double  dgt_value ;          /* ����� ��������� �������� */
 static Dcl_decl  dgt_return={ _DGT_VAL, 0,0,0,"", &dgt_value, NULL, 1, 1} ;

/*---------------------------------------------------- ������������� */
   
                              dgt_value=0 ;

/*-------------------------------------------- ���������� ���������� */

       if(pars_cnt     < 1   &&                                    /* ��������� ����� ���������� */
	  pars[0]->addr==NULL  ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                                 }

                    memset(unit_name, 0, sizeof(unit_name)) ;       /* ��������� ��� ���������� */
        if(pars[0]->size>=sizeof(unit_name))
                    memcpy(unit_name, pars[0]->addr, sizeof(unit_name)-1) ;
        else        memcpy(unit_name, pars[0]->addr, pars[0]->size) ;

/*------------------------------------------- ��������� ������ ����� */

                             targets_cnt=0 ;

       status=Program_emb_GetTargets(unit_name,  &targets, &targets_cnt, text) ;
    if(status) {
                         dcl_kernel->mError_code=_DCLE_USER_DEFINED ;
                  strcpy(dcl_kernel->mError_details, text) ;
                               return(&dgt_return) ;
               }
/*---------------------------------------------- ������ ������ ����� */

              dcl_kernel->iXobject_clear(source) ;                  /* ������� ��������� ��������� */

   for(i=0 ; i<targets_cnt ; i++) {                                 /* LOOP - ������� ����� */

              memset(t_name, 0, sizeof(t_name)) ;
             strncpy(t_name, targets[i].target->Name, sizeof(t_name)-1) ;
              memset(t_spec, 0, sizeof(t_spec)) ;
             strncpy(t_spec, targets[i].special, sizeof(t_spec)-1) ;
                     t_x    =targets[i].x ;
                     t_y    =targets[i].y ;
                     t_z    =targets[i].z ;
                     t_a    =  0. ;

        status=dcl_kernel->iXobject_add(source, &rec_template) ;
     if(status) {
                    dcl_kernel->mError_code=_DCLE_TYPEDEF_ELEM ;
                      return(&dgt_return) ; 
                }
                                  }                                 /* END LOOP - ������� ����� */
/*-------------------------------------------- ������������ �������� */
    
    if(targets_cnt)  free(targets) ;

/*-------------------------------------------------------------------*/

  return(&dgt_return) ;
}


   int  Program_emb_GetTargets(char *unit_name,  RSS_Unit_Target **targets, int *targets_cnt, char *error)

{
      RSS_Object *unit ;
             int  i ; 

/*---------------------------------------------------- ������������� */

             *targets_cnt= 0 ;

/*----------------------------------------- ������������� ���������� */

                    unit=NULL ;

       for(i=0 ; i<EventObject->Units.List_cnt ; i++)
         if(!stricmp(EventObject->Units.List[i].object->Name, unit_name)) { 
                         unit=EventObject->Units.List[i].object ;
                                               break ;
                                                                       }

    if(unit==NULL) {
                      sprintf(error, "������ '%s' �� �������� ��������� '%s'", EventObject->Name, unit_name) ;
                         return(-1) ;
                   }
/*------------------------------------------- ��������� ������ ����� */

       *targets_cnt=unit->vSpecial("GET_TARGETS", targets) ;
    if(*targets_cnt<0) {
                          sprintf(error, "��������� '%s' �� ������������ ��������� ������ ������", unit_name) ;
                             return(-1) ;
                       }
/*------------------------------------------- ��������� ������ ����� */

   return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                  ��������� �������������� ����������              */

   Dcl_decl *Program_dcl_DetectOrder(Lang_DCL  *dcl_kernel,
                                     Dcl_decl  *source, 
                                     Dcl_decl **pars, 
                                          int   pars_cnt)

{
              double  distance ;
     RSS_Unit_Target *targets ;
                 int  targets_cnt ;
     RSS_Unit_Target *orders ;
                 int  orders_cnt ;
  Dcl_complex_record *record ;
                char  t_name[128] ;
                char  t_spec[128] ;
              double  t_x ;
              double  t_y ;
              double  t_z ;
              double  t_a ;
                 int  status ;
                 int  i ;

          Dcl_decl  rec_data[6] ={
                                  {_CHR_AREA, 0, 0, 0, "object",           0,           t_name, 128, 128},
                                  {_CHR_AREA, 0, 0, 0, "special", (void *)128,          t_spec, 128, 128},
                                  {_DGT_VAL,  0, 0, 0, "x",       (void *)256, (char *)&t_x,      1,   1},
                                  {_DGT_VAL,  0, 0, 0, "y",       (void *)264, (char *)&t_y,      1,   1},
                                  {_DGT_VAL,  0, 0, 0, "z",       (void *)272, (char *)&t_z,      1,   1},
                                  {_DGT_VAL,  0, 0, 0, "azim",    (void *)280, (char *)&t_a,      1,   1}
                                 } ;
  Dcl_complex_type  rec_template={ "order", 288, rec_data, 6} ;

 static   double  dgt_value ;          /* ����� ��������� �������� */
 static Dcl_decl  dgt_return={ _DGT_VAL, 0,0,0,"", &dgt_value, NULL, 1, 1} ;

/*---------------------------------------------------- ������������� */

                                targets=NULL ;
                                 orders=NULL ;

                              dgt_value=0 ;

/*-------------------------------------------- ���������� ���������� */

       if(pars_cnt     !=2    ||                                    /* ��������� ����� ���������� */
	  pars[0]->addr==NULL ||
	  pars[1]->addr==NULL   ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                                  }

          distance=dcl_kernel->iDgt_get(pars[1]->addr, pars[1]->type) ;

       if((pars[0]->type & _DCLT_XTP_OBJ)                           /* ��������� ������� ������������ ������� */
                         !=_DCLT_XTP_OBJ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                             return(&dgt_return) ; 
                                          }
/*------------------------------------------ ���������� ������ ����� */

          targets_cnt=pars[0]->buff ;
          targets    =(RSS_Unit_Target *)calloc(targets_cnt, sizeof(*targets)) ;

           record=(Dcl_complex_record *)pars[0]->addr ;

   for(i=0 ; i<targets_cnt ; i++) {

            targets[i].x    =dcl_kernel->iDgt_get(record->elems[2].addr, record->elems[2].type) ;
            targets[i].y    =dcl_kernel->iDgt_get(record->elems[3].addr, record->elems[3].type) ;
            targets[i].z    =dcl_kernel->iDgt_get(record->elems[4].addr, record->elems[4].type) ;
            targets[i].order= 0 ;

         record=(Dcl_complex_record *)record->next_record ;
                                  }

/*-------------------------------------------------- ������ �������� */

        Program_emb_DetectOrder( targets,  targets_cnt,
                                &orders,  &orders_cnt,
                                 distance, NULL        ) ;

/*------------------------------------------- ������ ������ �������� */

                 dcl_kernel->iXobject_clear(source) ;               /* ������� ��������� ��������� */

   for(i=0 ; i<orders_cnt ; i++) {                                  /* LOOP - ������� �������� */
    
                     t_x=orders[i].x ;
                     t_y=orders[i].y ;
                     t_z=orders[i].z ;
                     t_a=orders[i].azim ;

        status=dcl_kernel->iXobject_add(source, &rec_template) ;
     if(status) {
                    dcl_kernel->mError_code=_DCLE_TYPEDEF_ELEM ;
                      return(&dgt_return) ; 
                }
                                 }                                  /* END LOOP - ������� �������� */
/*-------------------------------------------- ������������ �������� */
    
    if(targets_cnt)  free(targets) ;
    if( orders_cnt)  free( orders) ;

/*-------------------------------------------------------------------*/

#undef   _BUFF_SIZE

  return(&dgt_return) ;
}


   int  Program_emb_DetectOrder(RSS_Unit_Target  *targets,
                                            int   targets_cnt,
                                RSS_Unit_Target **orders,
                                            int  *orders_cnt,
                                         double   distance,
                                           char  *error     )

{
        int  order ;
     double  value ;
     double  value_max ;
        int  i ;
        int  j ;
        int  m1 ;
        int  m2 ;

#define  ORDERS       (*orders)
#define  ORDERS_CNT   (*orders_cnt)

/*---------------------------------------------------- ������������� */

                        ORDERS    =NULL ;
                        ORDERS_CNT=  0 ;

/*----------------------------------------------- ��������� �������� */

#define  DISTANCE(i, j)  sqrt((targets[i].x-targets[j].x)*(targets[i].x-targets[j].x)+  \
                              (targets[i].y-targets[j].y)*(targets[i].y-targets[j].y)+  \
                              (targets[i].z-targets[j].z)*(targets[i].z-targets[j].z) )

   for(i=1 ; i<targets_cnt ; i++) {

     for(j=0 ; j<i ; j++) 
       if(DISTANCE(i,j)<=distance)  break ;

       if(j>=i)  continue ;
        
       if(targets[j].order==0) {
                                                     ORDERS_CNT++ ;
                                    targets[j].order=ORDERS_CNT ;
                               }
  
                                    targets[i].order=targets[j].order ;
                                  }

#undef  DISTANCE

/*-------------------------------------------------- ������ �������� */

  if(ORDERS_CNT) {
/*- - - - - - - - - - - - - - - - - - - - - - - - - ��������� ������ */
          ORDERS_CNT++ ;

          ORDERS=(RSS_Unit_Target *)calloc(ORDERS_CNT, sizeof(*ORDERS)) ;
/*- - - - - - - - - - - - - - - - ����������� "������ ����" �������� */
     for(i=0 ; i<targets_cnt ; i++)
       if(targets[i].order) {
                              ORDERS[targets[i].order].x    +=targets[i].x ; 
                              ORDERS[targets[i].order].y    +=targets[i].y ; 
                              ORDERS[targets[i].order].z    +=targets[i].z ; 
                              ORDERS[targets[i].order].order++ ;
                            }
    
     for(order=1 ; order<ORDERS_CNT ; order++) {
                             ORDERS[order].x/=ORDERS[order].order ; 
                             ORDERS[order].y/=ORDERS[order].order ; 
                             ORDERS[order].z/=ORDERS[order].order ; 
                                               }
/*- - - - - - - - - - - - - - - - - - - -  ��������� ��������� ����� */
     for(i=0, order=1 ; order<ORDERS_CNT ; order++) {

          if(ORDERS[order].order<3)  continue ;                     /* ������ ����� 3 ����� �� ������������� */

          if(i!=order)  ORDERS[i]      =ORDERS[order] ;
                        ORDERS[i].order=       order ;
                               i++ ;
                                                    }

                        ORDERS_CNT=i ;
/*- - - - - - - - - - - - - - - - ����������� "������� ���" �������� */
     for(order=0 ; order<ORDERS_CNT ; order++) {

         value_max=0. ;
                m1=0 ;

        for(i=0 ; i<targets_cnt ; i++)                              /* ���� ����� ��������� �� ������ ����� */
          if(targets[i].order==ORDERS[order].order) {
               value=(targets[i].x-ORDERS[order].x)*(targets[i].x-ORDERS[order].x)
                    +(targets[i].z-ORDERS[order].z)*(targets[i].z-ORDERS[order].z) ;
            if(value>value_max) {  value_max=value ; 
                                          m1= i ;     } 
                                                    }

         value_max=0. ;
                m2=0 ;

        for(i=0 ; i<targets_cnt ; i++)                              /* ���� ����� ��������� �� ������ ����� */
          if(targets[i].order==ORDERS[order].order) {
               value=(targets[i].x-targets[m1].x)*(targets[i].x-targets[m1].x)
                    +(targets[i].z-targets[m1].z)*(targets[i].z-targets[m1].z) ;
            if(value>value_max) {  value_max=value ; 
                                          m2= i ;     } 
                                                    }

            ORDERS[order].azim=_RAD_TO_GRD*atan2(targets[m2].x-targets[m1].x, 
                                                 targets[m2].z-targets[m1].z ) ;
                                                }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                 }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*             ������� ���������� ��������� �� �������������         */

   Dcl_decl *Program_dcl_GetGlobalXYZ(Lang_DCL  *dcl_kernel,
                                      Dcl_decl  *source, 
                                      Dcl_decl **pars, 
                                           int   pars_cnt)

{
   RSS_Unit_Target  rel ; 
   RSS_Unit_Target  abs ;
               int  status ;

          Dcl_decl  rec_data[3] ={
                                  {_DGT_VAL,  0, 0, 0, "x", (void *) 0, (char *)&abs.x, 1, 1},
                                  {_DGT_VAL,  0, 0, 0, "y", (void *) 8, (char *)&abs.y, 1, 1},
                                  {_DGT_VAL,  0, 0, 0, "z", (void *)16, (char *)&abs.z, 1, 1}
                                 } ;
  Dcl_complex_type  rec_template={ "xyz", 24, rec_data, 3} ;

 static   double  dgt_value ;          /* ����� ��������� �������� */
 static Dcl_decl  dgt_return={ _DGT_VAL, 0,0,0,"", &dgt_value, NULL, 1, 1} ;

/*-------------------------------------------- ���������� ���������� */

       if(pars_cnt     !=3    ||                                    /* ��������� ����� ���������� */
	  pars[0]->addr==NULL ||
	  pars[1]->addr==NULL ||
	  pars[2]->addr==NULL   ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                                  }

        rel.x=dcl_kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;
        rel.y=dcl_kernel->iDgt_get(pars[1]->addr, pars[1]->type) ;
        rel.z=dcl_kernel->iDgt_get(pars[2]->addr, pars[2]->type) ;

/*------------------------------------------------ ������� ��������� */

            Program_emb_GetGlobalXYZ(&rel, &abs) ;

/*------------------------------------------------ ������ ���������� */

               dcl_kernel->iXobject_clear(source) ;                 /* ������� ��������� ��������� */

        status=dcl_kernel->iXobject_add(source, &rec_template) ;
     if(status) {
                    dcl_kernel->mError_code=_DCLE_TYPEDEF_ELEM ;
                      return(&dgt_return) ; 
                }
/*-------------------------------------------------------------------*/

#undef   _BUFF_SIZE

  return(&dgt_return) ;
}

    int  Program_emb_GetGlobalXYZ(RSS_Unit_Target *rel, 
                                  RSS_Unit_Target *abs )
{
    Matrix2d  Sum_Matrix ;
    Matrix2d  Oper_Matrix ;  
    Matrix2d  Vect_Matrix ;  


       Vect_Matrix.LoadZero   (3, 1) ;
       Vect_Matrix.SetCell    (0, 0, rel->x) ;
       Vect_Matrix.SetCell    (1, 0, rel->y) ;
       Vect_Matrix.SetCell    (2, 0, rel->z) ;
        Sum_Matrix.Load3d_azim(-EventObject->a_azim) ;
       Oper_Matrix.Load3d_elev(-EventObject->a_elev) ;
        Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
       Vect_Matrix.LoadMul    (&Sum_Matrix, &Vect_Matrix) ;

         abs->x=Vect_Matrix.GetCell(0, 0)+EventObject->x_base ;
         abs->y=Vect_Matrix.GetCell(1, 0)+EventObject->y_base ;
         abs->z=Vect_Matrix.GetCell(2, 0)+EventObject->z_base ;

  return(0) ;
}


/********************************************************************/
/*								    */
/*                   ���������� ������� Battle                      */

  Dcl_decl *Program_dcl_Battle(Lang_DCL  *dcl_kernel,
                               Dcl_decl **pars, 
                                    int   pars_cnt)
{
   char  text[1024] ;

 static     char  chr_value[512] ;          /* ����� ������ */
 static Dcl_decl  chr_return={ _CHR_PTR, 0,0,0,"", chr_value, NULL, 0, 512} ;

#define  _LOG_MAX   100

/*---------------------------------------------------- ������������� */

                 memset(chr_value, 0, sizeof(chr_value)) ;
                        chr_return.size=0 ;

/*-------------------------------------------- ���������� ���������� */

       if(pars_cnt     !=1   ||                                     /* ��������� ����� ���������� */
	  pars[0]->addr==NULL  ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&chr_return) ; 
                                 }

                    memset(text, 0, sizeof(text)) ;                 /* ��������� ������ �� ���� */
        if(pars[0]->size>=sizeof(text))
                    memcpy(text, pars[0]->addr, sizeof(text)-1) ;
        else        memcpy(text, pars[0]->addr, pars[0]->size) ;

/*----------------------------------- �������� ������� �� ���������� */

             strcat(EventCallback, text) ;
             strcat(EventCallback, ";") ;

/*-------------------------------------------------------------------*/

  return(&chr_return) ;
}



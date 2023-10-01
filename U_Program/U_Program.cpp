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

#define  __U_PROGRAM_MAIN__

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

              buff_size=(int)data->size()+16 ;
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
     void *data ;

/*---------------------------------------------------- ������������� */ 

           data=NULL ;
         *error= 0 ;

/*---------------------------------------- ���������� ����� �������� */ 

    do {
            data=this->ReadColumnHunter(path, error) ;
         if(data!=NULL || *error!=0)  break ;

            data=this->ReadLoneHunter(path, error) ;
         if(data!=NULL || *error!=0)  break ;
         
             sprintf(error, "����������� ��������� ����������") ;

       } while(0) ;
 
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
   int  status ;

/*------------------------------------------------- ����� ��������� */

      event_time=0. ;
      event_send=0 ;

   if(state!=NULL)  free(state) ;
                         state=NULL ;

/*----------------------------------- ��������� ����������� �������� */

   if(embeded!=NULL) {

     do {
            status=this->StartColumnHunter(t) ;
         if(status)  break ;

            status=this->StartLoneHunter(t) ;
         if(status)  break ;
         
               SEND_ERROR("Section PROGRAM: ����������� ���������� ���������") ;
                            return(-1) ;

        } while(0) ;

                     }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*                   ������ ��������� ���������                     */

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
	 {_DGT_VAL,  0,          0, 0, "$ThisX",              &this->Owner->state.x,     NULL,   1,   1               },
	 {_DGT_VAL,  0,          0, 0, "$ThisY",              &this->Owner->state.y,     NULL,   1,   1               },
	 {_DGT_VAL,  0,          0, 0, "$ThisZ",              &this->Owner->state.z,     NULL,   1,   1               },
	 {_DGT_VAL,  0,          0, 0, "$ThisAzim",           &this->Owner->state.azim,  NULL,   1,   1               },
	 {_DGT_VAL,  0,          0, 0, "$ThisElev",           &this->Owner->state.elev,  NULL,   1,   1               },
	 {_DGT_VAL,  0,          0, 0, "$ThisRoll",           &this->Owner->state.roll,  NULL,   1,   1               },
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

     do {
            status=this->EmbededColumnHunter(t1, t2, callback, cb_size) ;
         if(status)  break ;

            status=this->EmbededLoneHunter(t1, t2, callback, cb_size) ;
         if(status)  break ;
         
               SEND_ERROR("Section PROGRAM: ����������� ���������� ���������") ;
                            return(-1) ;

        } while(0) ;

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
             dcl_program_lib[i].size=(int)strlen((char *)dcl_program_lib[i].addr) ;

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
                        this->Owner->vEvent(event_name, event_time, NULL, 0) ;
                           event_send=0 ;
                   }
/*--------------------------------- ����������� ������ �� ���������� */

    if(this->hWnd!=NULL)  SendMessage(this->hWnd, WM_PAINT, NULL, NULL) ;

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



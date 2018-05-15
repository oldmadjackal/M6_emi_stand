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
      event_time=0. ;
      event_send=0 ;

   if(state!=NULL)  free(state) ;
                         state=NULL ;

  return(0) ;
}


/********************************************************************/
/*								    */
/*                   ������ ��������� ���������                     */

     static   RSS_Unit_Program *EventUnit ;
     static         RSS_Object *EventObject ;

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

    Dcl_decl *Program_dcl_Message    (Lang_DCL *,             Dcl_decl **, int) ;    /* ������ ��������� �� ����� � ��������� */
    Dcl_decl *Program_dcl_Log        (Lang_DCL *,             Dcl_decl **, int) ;    /* ������ � ��� */
    Dcl_decl *Program_dcl_Turn       (Lang_DCL *,             Dcl_decl **, int) ;    /* ��������� ����������� �������� */
    Dcl_decl *Program_dcl_ToPoint    (Lang_DCL *,             Dcl_decl **, int) ;    /* �������� � ����� */
    Dcl_decl *Program_dcl_StateSave  (Lang_DCL *, Dcl_decl *, Dcl_decl **, int) ;    /* ���������� ��������� ������� */
    Dcl_decl *Program_dcl_StateRead  (Lang_DCL *, Dcl_decl *, Dcl_decl **, int) ;    /* ���������� ��������� ������� */
    Dcl_decl *Program_dcl_GetTargets (Lang_DCL *, Dcl_decl *, Dcl_decl **, int) ;    /* ��������� ������ ����������� ����� */
    Dcl_decl *Program_dcl_DetectOrder(Lang_DCL *, Dcl_decl *, Dcl_decl **, int) ;    /* ��������� �������������� ���������� */

    Dcl_decl  dcl_program_lib[]={
         {0, 0, 0, 0, "$PassiveData$", NULL, "program", 0, 0},
	 {_DGT_VAL,  0,          0, 0, "$Time",              &t1,                      NULL,   1,   1               },
	 {_DGT_VAL,  0,          0, 0, "$TimeN",             &t2,                      NULL,   1,   1               },
	 {_DGT_VAL,  0,          0, 0, "$dTime",             &dt,                      NULL,   1,   1               },
	 {_CHR_AREA, 0,          0, 0, "$ThisName",           obj_name,                NULL,   0, sizeof(obj_name  )},
	 {_DGT_VAL,  0,          0, 0, "$ThisX",             &this->Owner->x_base,     NULL,   1,   1               },
	 {_DGT_VAL,  0,          0, 0, "$ThisY",             &this->Owner->y_base,     NULL,   1,   1               },
	 {_DGT_VAL,  0,          0, 0, "$ThisZ",             &this->Owner->z_base,     NULL,   1,   1               },
	 {_DGT_VAL,  0,          0, 0, "$ThisAzim",          &this->Owner->a_azim,     NULL,   1,   1               },
	 {_DGT_VAL,  0,          0, 0, "$ThisElev",          &this->Owner->a_elev,     NULL,   1,   1               },
	 {_DGT_VAL,  0,          0, 0, "$ThisRoll",          &this->Owner->a_roll,     NULL,   1,   1               },
 	 {_CHR_PTR, _DCL_CALL,   0, 0, "Message",     (void *)Program_dcl_Message,     "s",    0,   0               },
 	 {_CHR_PTR, _DCL_CALL,   0, 0, "Log",         (void *)Program_dcl_Log,         "s",    0,   0               },
 	 {_CHR_PTR, _DCL_CALL,   0, 0, "Turn",        (void *)Program_dcl_Turn,        "sv",   0,   0               },
 	 {_CHR_PTR, _DCL_CALL,   0, 0, "ToPoint",     (void *)Program_dcl_ToPoint,     "vvv",  0,   0               },
 	 {_DGT_VAL, _DCL_METHOD, 0, 0, "StateSave",   (void *)Program_dcl_StateSave,   "",     0,   0               },
 	 {_DGT_VAL, _DCL_METHOD, 0, 0, "StateRead",   (void *)Program_dcl_StateRead,   "",     0,   0               },
 	 {_DGT_VAL, _DCL_METHOD, 0, 0, "GetTargets",  (void *)Program_dcl_GetTargets,  "s",    0,   0               },
 	 {_DGT_VAL, _DCL_METHOD, 0, 0, "DetectOrder", (void *)Program_dcl_DetectOrder, "av",   0,   0               },
	 {0, 0, 0, 0, "", NULL, NULL, 0, 0}
                              } ;

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

                  EventUnit  = this ;
                  EventObject= this->Owner ;

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
   HWND  hDlg ;
   char  text[1024] ;
    int  rows_cnt ;
    int  i ;

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

  return(&chr_return) ;
}


/********************************************************************/
/*								    */
/*                  ��������� ����������� ��������                  */

  Dcl_decl *Program_dcl_Turn( Lang_DCL  *dcl_kernel,
                              Dcl_decl **pars, 
                                   int   pars_cnt)
{
    char  elem[32] ;
  double  angle ;
    char  cmd[1024] ;

 static     char  chr_value[512] ;          /* ����� ������ */
 static Dcl_decl  chr_return={ _CHR_PTR, 0,0,0,"", chr_value, NULL, 0, 512} ;

#define  _LOG_MAX   100

/*---------------------------------------------------- ������������� */

                 memset(chr_value, 0, sizeof(chr_value)) ;
                        chr_return.size=0 ;

/*-------------------------------------------- ���������� ���������� */

       if(pars_cnt     !=2    ||                                    /* ��������� ����� ���������� */
	  pars[0]->addr==NULL ||
	  pars[1]->addr==NULL   ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&chr_return) ; 
                                  }

                    memset(elem, 0, sizeof(elem)) ;                 /* ��������� ��� ���������� */
        if(pars[0]->size>=sizeof(elem))
                    memcpy(elem, pars[0]->addr, sizeof(elem)-1) ;
        else        memcpy(elem, pars[0]->addr, pars[0]->size) ;

          angle=dcl_kernel->iDgt_get(pars[1]->addr, pars[1]->type) ;

/*----------------------------------- ��������� ����������� �������� */

  if(!stricmp(elem, "A") ||
     !stricmp(elem, "E") ||
     !stricmp(elem, "R")   ) {
                                sprintf(cmd, "%s angle/%c %s %lf",
                                              EventObject->Type, elem[0], EventObject->Name, angle) ;
       EventObject->Module->vExecuteCmd(cmd) ;
                             }  
/*-------------------------------------------------------------------*/

  return(&chr_return) ;
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
     double  g ;
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

 static   double  dgt_value ;          /* ����� ��������� �������� */
 static Dcl_decl  dgt_return={ _DGT_VAL, 0,0,0,"", &dgt_value, NULL, 1, 1} ;


#define  _LOG_MAX   100

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

        if(pars[0]->size>=sizeof(limit_type))
                    memcpy(limit_type, pars[3]->addr, sizeof(limit_type)-1) ;
        else        memcpy(limit_type, pars[3]->addr, pars[3]->size) ;

               rad=dcl_kernel->iDgt_get(pars[4]->addr, pars[4]->type) ;
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
/*-------------------------------- �������� ���������� ������� ����� */

        r=sqrt((x_target-EventObject->x_base)*(x_target-EventObject->x_base)+
               (z_target-EventObject->z_base)*(z_target-EventObject->z_base) ) ;

        v=sqrt(EventObject->x_velocity*EventObject->x_velocity+
               EventObject->z_velocity*EventObject->z_velocity ) ;
       dr=dt*v ;

    if(dr>r) {
                        dgt_value=1. ;
                return(&dgt_return) ;
             }
/*----------------------------------------- ������� ������� �������� */

    if(limit_type[0]=='G') {

        rad=(EventObject->x_velocity*EventObject->x_velocity+
             EventObject->z_velocity*EventObject->z_velocity )/(10.*g) ;

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
                            target_pars="XZA" ;
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - "����������" ������� */
   else                 {
                            target.azim=angle ;
                            target_pars="A" ;
                        }
/*- - - - - - - - - - - - - - - - - - - - ������� �������� ��������� */
          EventObject->vCalculateDirect(&target, target_pars) ;

/*-------------------------------------------------------------------*/

  return(&dgt_return) ;
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
          RSS_Object *unit ;
     RSS_Unit_Target *targets ;
                 int  targets_cnt ;
                char  t_name[128] ;
                char  t_spec[128] ;
              double  t_x ;
              double  t_y ;
              double  t_z ;
                 int  status ;
                char  text[1024] ;
                 int  i ;

          Dcl_decl  rec_data[5] ={
                                  {_CHR_AREA, 0, 0, 0, "object",           0,           t_name, 128, 128},
                                  {_CHR_AREA, 0, 0, 0, "special", (void *)128,          t_spec, 128, 128},
                                  {_DGT_VAL,  0, 0, 0, "x",       (void *)256, (char *)&t_x,      1,   1},
                                  {_DGT_VAL,  0, 0, 0, "y",       (void *)264, (char *)&t_y,      1,   1},
                                  {_DGT_VAL,  0, 0, 0, "z",       (void *)272, (char *)&t_z,      1,   1}
                                 } ;
  Dcl_complex_type  rec_template={ "target", 280, rec_data, 5} ;

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

/*----------------------------------------- ������������� ���������� */

                    unit=NULL ;

       for(i=0 ; i<EventObject->Units.List_cnt ; i++)
         if(!stricmp(EventObject->Units.List[i].object->Name, unit_name)) { 
                         unit=EventObject->Units.List[i].object ;
                                               break ;
                                                                       }

    if(unit==NULL) {
                           sprintf(text, "������ '%s' �� �������� ��������� '%s'", EventObject->Name, unit_name) ;
                        SEND_ERROR(text) ;
                            return(NULL) ;
                   }
/*------------------------------------------- ��������� ������ ����� */

       targets_cnt=unit->vSpecial("GET_TARGETS", &targets) ;
    if(targets_cnt<0) {
                         dcl_kernel->mError_code=_DCLE_USER_DEFINED ;
                  strcpy(dcl_kernel->mError_details, "Component does not support targets iface") ;
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
                 int  order ;
  Dcl_complex_record *record ;
                char  t_name[128] ;
                char  t_spec[128] ;
              double  t_x ;
              double  t_y ;
              double  t_z ;
                 int  type ;          /* �������� ��� ���������� */
                 int  status ;
                char *work ;
                char *next ;
                char *ptr ;
                char *end ;
              double  value ;
                char  text[1024] ;
                char  tmp[1024] ;
                 int  i ;
                 int  j ;
                 int  k ;

          Dcl_decl  rec_data[5] ={
                                  {_CHR_AREA, 0, 0, 0, "object",           0,           t_name, 128, 128},
                                  {_CHR_AREA, 0, 0, 0, "special", (void *)128,          t_spec, 128, 128},
                                  {_DGT_VAL,  0, 0, 0, "x",       (void *)256, (char *)&t_x,      1,   1},
                                  {_DGT_VAL,  0, 0, 0, "y",       (void *)264, (char *)&t_y,      1,   1},
                                  {_DGT_VAL,  0, 0, 0, "z",       (void *)272, (char *)&t_z,      1,   1}
                                 } ;
  Dcl_complex_type  rec_template={ "link", 280, rec_data, 5} ;

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

/*----------------------------------------------- ��������� �������� */

#define  DISTANCE(i, j)  sqrt((targets[i].x-targets[j].x)*(targets[i].x-targets[j].x)+  \
                              (targets[i].y-targets[j].y)*(targets[i].y-targets[j].y)+  \
                              (targets[i].z-targets[j].z)*(targets[i].z-targets[j].z) )

        orders_cnt=0 ;

   for(i=1 ; i<targets_cnt ; i++) {

     for(j=0 ; j<i ; j++) 
       if(DISTANCE(i,j)<=distance)  break ;

       if(j>=i)  continue ;
        
       if(targets[j].order==0) {
                                                     orders_cnt++ ;
                                    targets[j].order=orders_cnt ;
                               }
  
                                    targets[i].order=targets[j].order ;
                                  }

#undef  DISTANCE

/*-------------------------------------------------- ������ �������� */

  if(orders_cnt) {

          orders_cnt++ ;

          orders=(RSS_Unit_Target *)calloc(orders_cnt, sizeof(*orders)) ;

     for(i=0 ; i<targets_cnt ; i++)
       if(targets[i].order) {
                              orders[targets[i].order].x    +=targets[i].x ; 
                              orders[targets[i].order].y    +=targets[i].y ; 
                              orders[targets[i].order].z    +=targets[i].z ; 
                              orders[targets[i].order].order++ ;
                            }
    
     for(order=1 ; order<orders_cnt ; order++) {
                             orders[order].x/=orders[order].order ; 
                             orders[order].y/=orders[order].order ; 
                             orders[order].z/=orders[order].order ; 
                                               }
         
                 }
/*------------------------------------------- ������ ������ �������� */

              dcl_kernel->iXobject_clear(source) ;                  /* ������� ��������� ��������� */

   for(i=1 ; i<orders_cnt ; i++) if(orders[i].order>2) {            /* LOOP - ������� �������� */
    
                     t_x=orders[i].x ;
                     t_y=orders[i].y ;
                     t_z=orders[i].z ;

        status=dcl_kernel->iXobject_add(source, &rec_template) ;
     if(status) {
                    dcl_kernel->mError_code=_DCLE_TYPEDEF_ELEM ;
                      return(&dgt_return) ; 
                }
                                                       }            /* END LOOP - ������� �������� */
/*-------------------------------------------- ������������ �������� */
    
    if(targets_cnt)  free(targets) ;
    if( orders_cnt)  free( orders) ;

/*-------------------------------------------------------------------*/

#undef   _BUFF_SIZE

  return(&dgt_return) ;
}



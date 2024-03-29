/********************************************************************/
/*								    */
/*       ������ ���������� ����������� "2-� ������� ���������"      */
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

#include "..\Matrix\Matrix.h"

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Unit\RSS_Unit.h"
#include "..\RSS_Kernel\RSS_Kernel.h"
#include "..\RSS_Model\RSS_Model.h"

#include "U_Control2Stage.h"

#pragma warning(disable : 4996)

#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text)
#define  CREATE_DIALOG       CreateDialogIndirectParam


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

     static   RSS_Module_Control2Stage  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	����������������� ����                      */

 U_CONTROL_2STAGE_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 U_CONTROL_2STAGE_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/********************************************************************/
/**							           **/
/**            �������� ������ ������ ���������� ��������          **/
/**                      "2-� ������� ���������"	           **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*                            ������ ������                         */

  struct RSS_Module_Control2Stage_instr  RSS_Module_Control2Stage_InstrList[]={

 { "help",    "?",  "#HELP   - ������ ��������� ������", 
                     NULL,
                    &RSS_Module_Control2Stage::cHelp   },
 { "config",  "c",  "#CONFIG - ������ ��������� ���������� � ���������� ������",
                    " CONFIG <���> \n"
                    "   ������ ��������� ���������� � ���������� ������\n",
                    &RSS_Module_Control2Stage::cConfig },
 { "program", "p", "#PROGRAM - ������� ��������� ���������� ��������",
                   " PROGRAM <���> <��� ����� ���������>\n"
                   "   ������� ��������� ���������� ��������\n",
                    &RSS_Module_Control2Stage::cProgram },
 { "nmax",    "n", "#NMAX - ������� ������������ ����������� ���������� ��� ����������� ���������",
                   " NMAX <���> <��������>\n"
                   "   ���� [��������]>20 - �������� �������� � �/�2\n"
                   "   ���� [��������]<20 - �������� �������� � �������� g\n",
                    &RSS_Module_Control2Stage::cNMax },
 {  NULL }
                                                            } ;


/********************************************************************/
/*								    */
/*		     ����� ����� ������             		    */

    struct RSS_Module_Control2Stage_instr *RSS_Module_Control2Stage::mInstrList=NULL ;


/********************************************************************/
/*								    */
/*		       ����������� ������			    */

     RSS_Module_Control2Stage::RSS_Module_Control2Stage(void)

{

/*---------------------------------------------------- ������������� */

	   keyword="EmiStand" ;
    identification="Control2Stage" ;
          category="Unit" ;
         lego_type="Control" ;

        mInstrList=RSS_Module_Control2Stage_InstrList ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        ���������� ������			    */

    RSS_Module_Control2Stage::~RSS_Module_Control2Stage(void)

{
}


/********************************************************************/
/*								    */
/*		      �������� �������                              */

  RSS_Object *RSS_Module_Control2Stage::vCreateObject(RSS_Model_data *data)

{
   RSS_Unit_Control2Stage *unit ;
                      int  i ;
 
/*---------------------------------------------- �������� ���������� */

       unit=new RSS_Unit_Control2Stage ;
    if(unit==NULL) {
               SEND_ERROR("������ Control2Stage: ������������ ������ ��� �������� ����������") ;
                        return(NULL) ;
                   }

             unit->Module=this ;

      strcpy(unit->Decl, "������� ���������� � 2-� ������� ����������") ;

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

     int  RSS_Module_Control2Stage::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- �������� ������ */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s -  �� � 2-� ������� ����������", identification) ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		        ��������� �������       		    */

  int  RSS_Module_Control2Stage::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* ���� ������ ������ ������� */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "CONTROL2STAGE"
#define  _SECTION_SHRT_NAME   "CONTROL2"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Object Control2Stage:") ;
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
       if(status)  SEND_ERROR("������ Control2Stage: ����������� �������") ;
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
/*		      ���������� ���������� HELP                    */

  int  RSS_Module_Control2Stage::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Unit_Control2Stage_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� INFO                    */
/*								    */
/*        INFO   <���>                                              */
/*        INFO/  <���>                                              */

  int  RSS_Module_Control2Stage::cInfo(char *cmd)

{
                     char  *name ;
   RSS_Unit_Control2Stage  *unit ;
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

/*---------------------------------------- �������� ����� ���������a */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* ���� ��� �� ������... */
                           SEND_ERROR("�� ������ ��� ���������. \n"
                                      "��������: INFO <���_���������> ...") ;
                                     return(-1) ;
                         }

       unit=(RSS_Unit_Control2Stage *)FindUnit(name) ;              /* ���� ��������� �� ����� */
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
/*		      ���������� ���������� CONFIG                  */
/*								    */
/*     CONFIG <��� ���������>                                       */

  int  RSS_Module_Control2Stage::cConfig(char *cmd)

{
#define   _PARS_MAX  10

                    char *pars[_PARS_MAX] ;
                    char *name ;
  RSS_Unit_Control2Stage *unit ;
                 INT_PTR  status ;
                    char *end ;
                     int  i ;

/*---------------------------------------- �������� ��������� ������ */

/*- - - - - - - - - - - - - - - - - - - - - - - -  ������ ���������� */        
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

           if( pars[0]==NULL ||
              *pars[0]==  0    ) {
                                     SEND_ERROR("�� ����� ���������") ;
                                         return(-1) ;
                                 }

                     name=pars[0] ;

/*---------------------------------------- �������� ����� ���������� */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* ���� ��� �� ������... */
                           SEND_ERROR("�� ������ ��� ���������a. \n"
                                      "��������: CONFIG <���_���������a> ...") ;
                                     return(-1) ;
                         }

       unit=(RSS_Unit_Control2Stage *)FindUnit(name) ;              /* ���� ��������� �� ����� */
    if(unit==NULL)  return(-1) ;

/*--------------------------------------- ������� � ���������� ����� */

       status=DialogBoxIndirectParam( GetModuleHandle(NULL),
                                     (LPCDLGTEMPLATE)Resource("IDD_CONFIG", RT_DIALOG),
                                      GetActiveWindow(), 
                                      Unit_Control2Stage_Config_dialog, 
                                     (LPARAM)unit                    ) ;
    if(status)  return(-1) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� PROGRAM                 */
/*								    */
/*     PROGRAM <��� ���������> <���� � ����� ���������>             */

  int  RSS_Module_Control2Stage::cProgram(char *cmd)

{
#define   _PARS_MAX  10

                    char *pars[_PARS_MAX] ;
                    char *name ;
                    char *path ;
  RSS_Unit_Control2Stage *unit ;
                 INT_PTR  status ;
                    char  error[1024] ;
                    char *end ;
                     int  i ;

/*---------------------------------------- �������� ��������� ������ */

/*- - - - - - - - - - - - - - - - - - - - - - - -  ������ ���������� */        
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

           if( pars[0]==NULL ||
              *pars[0]==  0    ) {
                                     SEND_ERROR("�� ����� ���������") ;
                                         return(-1) ;
                                 }

                     name=pars[0] ;
                     path=pars[1] ;

/*---------------------------------------------- �������� ���������� */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* ���� ��� �� ������... */
                           SEND_ERROR("�� ������ ��� ���������a. \n"
                                      "��������: PROGRAM <���_���������a> ...") ;
                                     return(-1) ;
                         }

    if(path   ==NULL ||
       path[0]==  0    ) {                                          /* ���� ���� ��������� �� ������... */
                           SEND_ERROR("�� ������ ��� ���������a. \n"
                                      "��������: PROGRAM <���_���������a> ...") ;
                                     return(-1) ;
                         }


       unit=(RSS_Unit_Control2Stage *)FindUnit(name) ;              /* ���� ��������� �� ����� */
    if(unit==NULL)  return(-1) ;

/*--------------------------------------- ������� � ���������� ����� */

       status=ReadProgram(unit, path, error) ;
    if(status) {
                  SEND_ERROR(error) ;
                     return(-1) ;
               }

        strncpy(unit->program, path, sizeof(unit->program)) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� NMAX                    */
/*								    */
/*         NMAX <���> <��������>                                    */
/*								    */
/*          ���� [��������]>20 - �������� �������� � �/�2           */
/*          ���� [��������]<20 - �������� �������� � �������� g     */

  int  RSS_Module_Control2Stage::cNMax(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

                      char  *pars[_PARS_MAX] ;
                      char  *name ;
                      char **xyz ;
                    double   coord[_COORD_MAX] ;
                       int   coord_cnt ;
    RSS_Unit_Control2Stage  *unit ;
                      char  *error ;
                      char  *end ;
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

                     name= pars[0] ;
                      xyz=&pars[1] ;   

/*---------------------------------------- �������� ����� ���������a */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* ���� ��� �� ������... */
                           SEND_ERROR("�� ������ ��� ���������. \n"
                                      "��������: NMAX <���_���������> ...") ;
                                     return(-1) ;
                         }

       unit=(RSS_Unit_Control2Stage *)FindUnit(name) ;              /* ���� ��������� �� ����� */
    if(unit==NULL)  return(-1) ;

/*------------------------------------------------ ������ ���������� */

    for(i=0 ; xyz[i]!=NULL && i<_COORD_MAX ; i++) {

             coord[i]=strtod(xyz[i], &end) ;
        if(*end!=0) {  
                       SEND_ERROR("������������ �������� ������������ ����������� ���������� ��� ����������� ���������") ;
                                       return(-1) ;
                    }
                                                  }

                             coord_cnt=i ;

                        error= NULL ;
      if(coord_cnt==0)  error="�� ������� ������������� ����������� ���������� ��� ���������� ���������" ;

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

/*--------------------------------------------------- ������� ������ */

       unit->vector_control_max=coord[0] ;

    if(unit->vector_control_max<20)  unit->vector_control_max*=9.8 ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*	       ����� ������� ���� Control2Stage �� �����            */

  RSS_Unit *RSS_Module_Control2Stage::FindUnit(char *name)

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

     if(strcmp(unit->Type, "Control2Stage")) {

           SEND_ERROR("��������� �� �������� ����������� ���� Control2Stage") ;
                            return(NULL) ;
                                             }
/*-------------------------------------------------------------------*/ 

   return((RSS_Unit *)unit) ;

#undef   OBJECTS
#undef   OBJECTS_CNT

}


/********************************************************************/
/*								    */
/*	             ���������� ��������� ����������                */

  int  RSS_Module_Control2Stage::ReadProgram(RSS_Unit_Control2Stage *unit, char *path, char *error)

{
     FILE *file ;
     char  text[1024] ;
      int  row ;
     char *end ;
      int  start_stage ;
    Stage *stage ;

/*---------------------------------------------------- ������������� */ 

                    *error=0 ; 

/*--------------------------------------------------- �������� ����� */ 

        file=fopen(path, "rb") ;
     if(file==NULL)  {
                         sprintf(error, "Control2Stage - program file open error %d : %s", errno, path) ;
                           return(-1) ;
                     }
/*------------------------------------------------- ���������� ����� */ 

           unit->stages_cnt=0 ;

                    row=0 ;

            start_stage=1 ;

    do {
/*- - - - - - - - - - - - - - - - - - -  ���������� ��������� ������ */
               memset(text, 0, sizeof(text)) ;
            end=fgets(text, sizeof(text)-1, file) ;
         if(end==NULL)  break ;

             row++ ;

            end=strchr(text, '\r') ;
         if(end!=NULL)  *end=0 ; 
            end=strchr(text, '\n') ;
         if(end!=NULL)  *end=0 ; 

         if(text[sizeof(text)-1]!=0) {
                         sprintf(error, "Control2Stage - to large row %d in program file : %s", row, path) ;
                                        break ;
                                     }

         if(text[0]==';')  continue ;                               /* ��������� ����������� */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - ��������� ���� */
         if(start_stage) {

             if(stricmp(text, "start")) {
                           sprintf(error, "Control2Stage - First section in program file must be 'start' (row %d) : %s", row, path) ;
                                            break ;
                                        }

                               stage=&unit->stage_start ;
                        memset(stage, 0, sizeof(*stage)) ;

                               start_stage=0 ;
                                  continue ;
                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - ������ ����� */
         if(!memicmp(text, "stage", strlen("stage"))) {

            if(unit->stages_cnt>=_STAGES_MAX) {
                           sprintf(error, "Control2Stage - Stages list overflow in row %d : %s", row, path) ;
                                                 break ;
                                              }

                               stage=&unit->stages[unit->stages_cnt] ;
                        memset(stage, 0, sizeof(*stage)) ;

                                      unit->stages_cnt++ ;
                                        continue ;
                                                      }
/*- - - - - - - - - - - - - - - - - - - - ������� ������������� - IF */
#define  A   stage->actions[stage->actions_cnt]

         if(!memicmp(text, "if ", strlen("if "))) {

              memset(&A, 0, sizeof(StageAction)) ;

              strncpy(A.operation, text+strlen("if "), sizeof(A.operation)-1) ;
                      A.if_condition=1 ; 

                      stage->actions_cnt++ ;
                                                  }
/*- - - - - - - - - - - - - - - - - - - - - -  ������� ������ - EXIT */
         else
         if(!memicmp(text, "exit ", strlen("exit "))) {

              memset(&A, 0, sizeof(StageAction)) ;

              strncpy(A.operation, text+strlen("exit "), sizeof(A.operation)-1) ;
                      A.exit_condition=1 ; 

                      stage->actions_cnt++ ;
                                                      }
/*- - - - - - - - - - - - - - ���������� ����������� �������� - ONCE */
         else
         if(!memicmp(text, "once ", strlen("once "))) {

              memset(&A, 0, sizeof(StageAction)) ;

              strncpy(A.operation, text+strlen("once "), sizeof(A.operation)-1) ;
                      A.once=1 ; 

                      stage->actions_cnt++ ;
                                                      }
/*- - - - - - - - - - - - - - - - - - - - - - - - - ������� �������� */
         else                                        {

              memset(&A, 0, sizeof(StageAction)) ;

              strncpy(A.operation, text, sizeof(A.operation)-1) ;

                      stage->actions_cnt++ ;
                                                     }
#undef   A 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
       } while(1) ;

/*--------------------------------------------------- �������� ����� */ 

        fclose(file) ;

/*-------------------------------------------------------------------*/ 
  
   if(*error!=0)  return(-1) ;
                  return( 0) ;
}


/********************************************************************/
/********************************************************************/
/**							           **/
/**	  �������� ������ ���������� "2-� ������� ���������"       **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       ����������� ������			    */

     RSS_Unit_Control2Stage::RSS_Unit_Control2Stage(void)

{
   strcpy(Type, "Control2Stage") ;
          Module=&ProgramModule ;

       Parameters    =NULL ;
       Parameters_cnt=  0 ;

       memset(program, 0, sizeof(program)) ;

           stages_cnt=  0 ;

   vector_control_max=  0. ;

}


/********************************************************************/
/*								    */
/*		        ���������� ������			    */

    RSS_Unit_Control2Stage::~RSS_Unit_Control2Stage(void)

{
      vFree() ;
}


/********************************************************************/
/*								    */
/*		       ������������ ��������                        */

  void   RSS_Unit_Control2Stage::vFree(void)

{
  int  i ;


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
/*                        ���������� �������		            */

    class RSS_Unit *RSS_Unit_Control2Stage::vCopy(RSS_Object *owner)

{
            RSS_Model_data  create_data ;
    RSS_Unit_Control2Stage *unit ;
                       int  i ;
   
/*------------------------------------- ����������� �������� ������� */

      memset(&create_data, 0, sizeof(create_data)) ;

       unit=(RSS_Unit_Control2Stage *)this->Module->vCreateObject(&create_data) ;
    if(unit==NULL)  return(NULL) ;

      strcpy(unit->Name, this->Name) ; 
             unit->Owner=owner ;

    if(owner!=NULL)  owner->Units.Add(unit, "") ;

/*------------------------------------- ����������� �������� ������� */

      strcpy(unit->program, this->program) ;   
             unit->stage_start=stage_start ;
             unit->stages_cnt =stages_cnt ;

   for(i=0 ; i<stages_cnt ; i++)  unit->stages[i]=stages[i] ;

             unit->vector_control_max=vector_control_max ;

/*-------------------------------------------------------------------*/

   return(unit) ;
}


/********************************************************************/
/*								    */
/*                        ����������� ��������                      */

     int  RSS_Unit_Control2Stage::vSpecial(char *oper, void *data)
{
/*------------------------------------------ ������ �� ������ BATTLE */

    if(!stricmp(oper, "BATTLE")) {

//                             this->battle=(RSS_Module_Battle *)data ;
                                      return(0) ;
                                 }
/*-------------------------------------------------------------------*/

  return(-1) ;
}


/********************************************************************/
/*								    */
/*             ���������� ������� ��������� ���������               */

     int  RSS_Unit_Control2Stage::vCalculateStart(double  t)
{
  char  text[1024] ;
  char  error[1024] ;
   int  status ;
   int  i ; 
   int  j ; 

/*------------------------------------------------- ������� �������� */

       if(this->stages_cnt==0) {

              sprintf(error, "Unit %s.%s - missed program for control unit", this->Owner->Name, this->Name) ;
           SEND_ERROR(error) ;
                 return(-1) ;
                               }

       if(this->vector_control_max==0.) {

              sprintf(error, "Unit %s.%s - missed program for control unit", this->Owner->Name, this->Name) ;
           SEND_ERROR(error) ;
                 return(-1) ;
                                        }
/*---------------------------------------------------- ������������� */

            start_time=(time_t)t ;

     for(i=0 ; i<this->stages_cnt ; i++) 
     for(j=0 ; j<this->stages[i].actions_cnt ; j++)  this->stages[i].actions[j].done=0 ;

          memset(&t_xyz,     0, sizeof(t_xyz    )) ;
          memset(&t_xyz_prv, 0, sizeof(t_xyz_prv)) ;

/*---------------------------------------- ��������� ��������� ����� */

      this->warhead_control[0]=0 ;
      this-> homing_control[0]=0 ;

#define   A   this->stage_start.actions

   for(i=0 ; i<this->stage_start.actions_cnt ; i++) {

     if(A[i].if_condition || A[i].exit_condition)  continue ;

            status=ExecuteOperation(A[i].operation, text) ;
         if(status==-1)  {
              sprintf(text, "Unit %s - error in program for control unit, start stage: %s", this->Owner->Name, text) ;
                        SEND_ERROR(error) ;
                             return(-1) ;
                         }
                                                    }

#undef   A

               this->stage=-1 ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*                   ������ ��������� ���������                     */

     int  RSS_Unit_Control2Stage::vCalculate(double t1, double t2, char *callback, int cb_size)
{
         int  next_stage ;
        char  text[1024] ;
        char  error[1024] ;
         int  true_flag ;
  RSS_Vector  T ;             /* ����������� �� ���� */
  RSS_Vector  A  ;            /* ������ ��������� */
  RSS_Vector  V  ;            /* ������ �������� */
      double  v ;             /* �������� ��������� */
      double  t_time ;        /* ������ ������� ���������� */
      double  l ;
      double  h ;
         int  status ;
         int  i ;

/*------------------------------------------------- ������� �������� */

/*----------------------------------------------- ���������� ������� */

          t1-=this->start_time ;
          t2-=this->start_time ;

/*------------------------------------ ������������� �������� ������ */

           this->warhead_control[0]=0 ;
           this-> homing_control[0]=0 ;

   memset(&this->vector_control, 0, sizeof(this->vector_control)) ;

/*-------------------------------- �������� ������ �� ������� ������ */

                                 next_stage=0 ;

    if(this->stage>= 0) {

#define  S  this->stages[this->stage]

       for(i=0 ; i<S.actions_cnt ; i++)
         if(S.actions[i].exit_condition) {

             status=ExecuteOperation(S.actions[i].operation, error) ;
          if(status==-1) {
                            sprintf(text, "RSS_Unit_Control2Stage::vCalculate - ������ %s - ������ ����������� : %s", this->Owner->Name, error) ;
                         SEND_ERROR(text) ;
                              return(-1) ;
                         }

            if(status) {
                          next_stage=1 ;
                                break ;
                       }
                                         }

#undef  S  

                        }
    else
    if(this->stage==-1) {
                                 next_stage=1 ;
                        }
/*------------------------------------- ����������� ��������� ������ */

   if(next_stage) {

#define  S  this->stages[next_stage]

     for(next_stage=this->stage+1 ; 
         next_stage<this->stages_cnt ; next_stage++) {

               true_flag=1 ;

       for(i=0 ; i<S.actions_cnt ; i++)
         if(S.actions[i].if_condition) {

             status=ExecuteOperation(S.actions[i].operation, error) ;
          if(status==-1) {
                            sprintf(text, "RSS_Unit_Control2Stage::vCalculate - ������ %s - ������ ����������� : %s", this->Owner->Name, error) ;
                         SEND_ERROR(text) ;
                              return(-1) ;
                         }

               true_flag&=status ;
                                       }

          if(true_flag) {
                            this->stage=next_stage ;
                                    break ;
                        }

                                                     }

       if(next_stage ==this->stages_cnt && 
          this->stage>=  0                )  this->stage=-2 ;

#undef  S  
                  }
/*----------------------------------------- ��������� ������� ������ */

  if(this->stage>=0) {

#define  S  this->stages[this->stage]

/*- - - - - - - - - - - - - - - - - - - ���������� ������ ���������� */
       for(i=0 ; i<S.actions_cnt ; i++)
         if(!S.actions[i].if_condition  &&
            !S.actions[i].exit_condition  ) {

          if(S.actions[i].once &&
             S.actions[i].done   )  continue ;

             status=ExecuteOperation(S.actions[i].operation, error) ;
          if(status==-1) {
                            sprintf(text, "RSS_Unit_Control2Stage::vCalculate - ������ %s - ������ ����������� : %s", this->Owner->Name, error) ;
                         SEND_ERROR(text) ;
                              return(-1) ;
                         }

               S.actions[i].done=1 ;
                                            }
/*- - - - - - - - - - - - - - - - - - - - - - - - ���������� ������� */
         V.x=this->Owner->state.x_velocity ;
         V.y=this->Owner->state.y_velocity ;
         V.z=this->Owner->state.z_velocity ;

           v=sqrt(V.x*V.x+V.y*V.y+V.z*V.z) ;
      t_time=this->t_distance/v ;                                   /* ������ ���������� ������� */
/*- - - - - - - - - - - - - - - - - - - - - ������ ������� ��������� */
      T.x=(this->t_xyz.x-this->Owner->state.x)*v/this->t_distance ; /* ��������� ������ �� ���� �� �������� */
      T.y=(this->t_xyz.y-this->Owner->state.y)*v/this->t_distance ;
      T.z=(this->t_xyz.z-this->Owner->state.z)*v/this->t_distance ;

      A.x=2*(T.x-V.x)/t_time ; 
      A.y=2*(T.y-V.y)/t_time ; 
      A.z=2*(T.z-V.z)/t_time ; 

      T.x=V.x+A.x ;                                                 /* ��������� ����������� �� ���� */
      T.y=V.y+A.y ;
      T.z=V.z+A.z ;

        l=sqrt(T.x*T.x+T.y*T.y+T.z*T.z) ;
      T.x=T.x/l*v ;                                                 /* ��������� ��������� ����������� �� ���� �� �������� */
      T.y=T.y/l*v ;
      T.z=T.z/l*v ;

        h=sqrt((T.x-V.x)*(T.x-V.x)+                                 /* ����� ������� ��������� */
               (T.y-V.y)*(T.y-V.y)+
               (T.z-V.z)*(T.z-V.z) ) ;

        l=v/( (v*v+v*v-h*h)/(2.*v*v) ) ;                            /* ��������� ��������� ����������� �� ���� ��� ���������� ��������� */
      T.x=T.x/v*l ;                                                 
      T.y=T.y/v*l ;
      T.z=T.z/v*l ;

       vector_control.x=T.x-V.x ;                                   /* ���������� ���������� ���������� */
       vector_control.y=T.y-V.y ;
       vector_control.z=T.z-V.z ;
/*- - - - - - - - - - - - - - - -  ����������� ���������� ���������� */
          l=sqrt(vector_control.x*vector_control.x+
                 vector_control.y*vector_control.y+
                 vector_control.z*vector_control.z ) ;

     if(l>this->vector_control_max) {

                     vector_control.x*=(this->vector_control_max/l) ;
                     vector_control.y*=(this->vector_control_max/l) ;
                     vector_control.z*=(this->vector_control_max/l) ;

                                    } 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#undef  S  

                     }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*      ����������� ���������� ������� ��������� ���������          */

     int  RSS_Unit_Control2Stage::vCalculateShow(double  t1, double  t2)
{
  return(0) ;
}

/*********************************************************************/
/*								     */
/*              	  ����������� �� ����            	     */

    int  RSS_Unit_Control2Stage::vSetHomingDirection(RSS_Point *direction)

{
   return(0) ;
}


/*********************************************************************/
/*								     */
/*             	 ������������� ��������� ����           	     */

    int  RSS_Unit_Control2Stage::vSetHomingPosition(RSS_Point *position)

{
   t_xyz=*position ;

   return(0) ;
}


/*********************************************************************/
/*								     */
/*               	 ��������� �� ����                 	     */

    int  RSS_Unit_Control2Stage::vSetHomingDistance(double  distance)

{
      t_distance=distance ;

   return(0) ;
}


/*********************************************************************/
/*								     */
/*               	 �������� ��������� � �����            	     */

    int  RSS_Unit_Control2Stage::vSetHomingClosingSpeed(double  velocity)

{
   return(0) ;
}


/*********************************************************************/
/*								     */
/*              	  ���������� �������� ��         	     */

    int  RSS_Unit_Control2Stage::vGetWarHeadControl(char *regime)

{
     strcpy(regime, this->warhead_control) ;
  

   return(0) ;
}


/*********************************************************************/
/*								     */
/*              	  ���������� �������� ���         	     */

    int  RSS_Unit_Control2Stage::vGetHomingControl(char *regime)

{
     strcpy(regime, this->homing_control) ;

   return(0) ;
}


/*********************************************************************/
/*								     */
/*              	  ���������� ����������         	     */

    int  RSS_Unit_Control2Stage::vGetEngineControl(RSS_Unit_Engine_Control *engines)

{
   return(0) ;
}


/*********************************************************************/
/*								     */
/*     	  ���������� ����������������� �������������        	     */

    int  RSS_Unit_Control2Stage::vGetAeroControl(RSS_Unit_Aero_Control *aeros)

{
   return(0) ;
}


/*********************************************************************/
/*								     */
/*               ��������� ���������� ����������                     */

    int  RSS_Unit_Control2Stage::vGetVectorControl(RSS_Vector *vector)

{
     *vector=this->vector_control ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*                   ���������� ��������� ����������                */

     int  RSS_Unit_Control2Stage::ExecuteOperation(char *text, char *error)
{
   char  oper[2048] ;
   char  result[256] ;
   char  value[256] ;
   char *control ;
   char *work ;
   char *words[30] ;
    int  status ;
    int  i ;

/*------------------------------------------------------- ���������� */

                  control=NULL ;

                 strcpy(oper, text) ;

/*----------------------------------- ��������� ��������� ���������� */

   if(!memicmp(oper, "homing:", strlen("homing:"))) {

           control=this->homing_control ;
              work=oper+strlen("homing:") ;
                                                    }
   else
   if(!memicmp(oper, "warhead:", strlen("warhead:"))) {

           control=this->warhead_control ;
              work= oper+strlen("warhead:") ;
                                                      }
   else                                               {

              work= oper ;
                                                      }
/*-------------------------------------------------- ������ �� ����� */

              memset(words, 0, sizeof(words)) ;

                      i = 0 ;
                words[i]=strtok(work, " \t") ;

          while(words[i]!=NULL && i<30) {
                      i++ ;
                words[i]=strtok(NULL, " \t") ;
                                        }
/*--------------------------------------------------- ��������� ���� */
 
                       result[0]=0 ;

     for(i=0 ; i<30 && words[i]!=NULL ; i++) {

          if(words[i]==0)  continue ;

          status=ExecuteExpression(words[i], value, error) ;
       if(status)  return(-1) ;

       if(result[0]!=0)  strcat(result, " "  ) ;
                         strcat(result, value) ;

                                             }
/*-------------------------------------------- ���������� ���������� */

   if(control!=NULL) {
                       if(control[0]!=0)  strcat(control, ";") ;
                                          strcat(control, result) ;
                     } 
/*-------------------------------------------------------------------*/

   if(result[0]=='1')  return(1) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*                      ���������� ���������                        */

     int  RSS_Unit_Control2Stage::ExecuteExpression(char *text, char *result, char *error)
{
     char *next ;  
     char *oper ;
     char *oper_prv ;
   double  value ;
   double  result_dgt ;
      int  dgt_data ; 

/*---------------------------------------------------- ������������� */

                oper=NULL ;
                next=text ; 
            dgt_data=  0 ; 

   do {
             oper_prv=oper ;
/*- - - - - - - - - - -  - - - - - - - - - - - -  ��������� �������� */
#define  KEY  "$target.x"
         if(!memicmp(next, KEY, strlen(KEY))) {                     /* X ���� */

               value=this->Owner->o_target->state_0.x ;
                oper=next+strlen(KEY) ;
                next=oper+1 ;
                                              }
         else 
#undef   KEY
#define  KEY  "$target.y"
         if(!memicmp(next, KEY, strlen(KEY))) {                     /* Y ���� */

               value=this->Owner->o_target->state_0.y ;
                oper=next+strlen(KEY) ;
                next=oper+1 ;
                                              }
         else 
#undef   KEY
#define  KEY  "$target.z"
         if(!memicmp(next, KEY, strlen(KEY))) {                     /* Z ���� */

               value=this->Owner->o_target->state_0.z ;
                oper=next+strlen(KEY) ;
                next=oper+1 ;
                                              }
         else 
#undef   KEY
#define  KEY  "homing.distance"
         if(!memicmp(next, KEY, strlen(KEY))) {                     /* ��������� �� ���� */

               value=this->t_distance ;
                oper=next+strlen(KEY) ;
                next=oper+1 ;
                                              }
         else 
#undef   KEY
         if(!stricmp(next, "$target"                       )) {     /* �������� ���� */

                  strcpy(result, this->Owner->target) ;
                          break ;
                                                              }
         else 
         if(oper_prv!=NULL)                                   {     /* �������� ������� */

                value=strtod(next, &oper) ;
                             next=oper+1 ;
                                                              }
         else                                                 {     /* �������� ������� */
                  strcpy(result, text) ;
                          break ;
                                                              }
/*- - - - - - - - - - - - - - - - - - - - - - -  ���������� �������� */
         if(oper_prv==NULL  ) {
                                 result_dgt =value ;
                                    dgt_data=  1 ; 
                              } 
         else
         if(oper_prv[0]=='+') {
                                 result_dgt+=value ;
                              }
         else
         if(oper_prv[0]=='>') {
                                 if(result_dgt>value)  result_dgt=1. ;
                                 else                  result_dgt=0. ;
                              }
         else
         if(oper_prv[0]=='<') {
                                 if(result_dgt<value)  result_dgt=1. ;
                                 else                  result_dgt=0. ;
                              }
         else                 {
                sprintf(error, "Unknown operation i %s", text) ;
                                    return(-1) ;
                              }

         if(oper[0]==0)  break ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      } while(1) ;

/*-------------------------------------- ������� ���������� �������� */

   if(dgt_data) {
                    sprintf(result, "%.6lf", result_dgt) ;
                }
/*-------------------------------------------------------------------*/

  return(0) ;
}
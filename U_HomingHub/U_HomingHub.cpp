/********************************************************************/
/*								    */
/*       ������ ���������� ����������� "��������������� ���"        */
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

#include "U_HomingHub.h"

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

     static   RSS_Module_HomingHub  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	����������������� ����                      */

 U_HOMING_HUB_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 U_HOMING_HUB_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/********************************************************************/
/**							           **/
/**            �������� ������ ������ ���������� ��������          **/
/**                      "��������������� ���"	                   **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*                            ������ ������                         */

  struct RSS_Module_HomingHub_instr  RSS_Module_HomingHub_InstrList[]={

 { "help",    "?",  "#HELP   - ������ ��������� ������", 
                     NULL,
                    &RSS_Module_HomingHub::cHelp   },
 { "config",  "c",  "#CONFIG - ������ ��������� ���������� � ���������� ������",
                    " CONFIG <���> \n"
                    "   ������ ��������� ���������� � ���������� ������\n",
                    &RSS_Module_HomingHub::cConfig },
 { "plugin",  "p",  "#PLUGIN - ������ ��������� ���������� � ���������� ������",
                    " PLUGIN <���> <�����������> <>\n"
                    "   ������ ��������� ���������� � ���������� ������\n",
                    &RSS_Module_HomingHub::cPlugin },
 {  NULL }
                                                            } ;


/********************************************************************/
/*								    */
/*		     ����� ����� ������             		    */

    struct RSS_Module_HomingHub_instr *RSS_Module_HomingHub::mInstrList=NULL ;


/********************************************************************/
/*								    */
/*		       ����������� ������			    */

     RSS_Module_HomingHub::RSS_Module_HomingHub(void)

{

/*---------------------------------------------------- ������������� */

	   keyword="EmiStand" ;
    identification="HomingHub" ;
          category="Unit" ;
         lego_type="Homing" ;

        mInstrList=RSS_Module_HomingHub_InstrList ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        ���������� ������			    */

    RSS_Module_HomingHub::~RSS_Module_HomingHub(void)

{
}


/********************************************************************/
/*								    */
/*		      �������� �������                              */

  RSS_Object *RSS_Module_HomingHub::vCreateObject(RSS_Model_data *data)

{
   RSS_Unit_HomingHub *unit ;
                  int  i ;
 
/*---------------------------------------------- �������� ���������� */

       unit=new RSS_Unit_HomingHub ;
    if(unit==NULL) {
               SEND_ERROR("������ HomingHub: ������������ ������ ��� �������� ����������") ;
                        return(NULL) ;
                   }

             unit->Module=this ;

      strcpy(unit->Decl, "��������������� ���") ;

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

     int  RSS_Module_HomingHub::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- �������� ������ */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s -  ��������������� ���", identification) ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		        ��������� �������       		    */

  int  RSS_Module_HomingHub::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* ���� ������ ������ ������� */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "HOMINGHUB"
#define  _SECTION_SHRT_NAME   "HOMINGHUB"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Object HomingHub:") ;
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
       if(status)  SEND_ERROR("������ HomingHub: ����������� �������") ;
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

  int  RSS_Module_HomingHub::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Unit_HomingHub_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� INFO                    */
/*								    */
/*        INFO   <���>                                              */
/*        INFO/  <���>                                              */

  int  RSS_Module_HomingHub::cInfo(char *cmd)

{
                     char  *name ;
       RSS_Unit_HomingHub  *unit ;
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

       unit=(RSS_Unit_HomingHub *)FindUnit(name) ;                  /* ���� ��������� �� ����� */
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

  int  RSS_Module_HomingHub::cConfig(char *cmd)

{
#define   _PARS_MAX  10

                    char *pars[_PARS_MAX] ;
                    char *name ;
      RSS_Unit_HomingHub *unit ;
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

       unit=(RSS_Unit_HomingHub *)FindUnit(name) ;                  /* ���� ��������� �� ����� */
    if(unit==NULL)  return(-1) ;

/*--------------------------------------- ������� � ���������� ����� */

       status=DialogBoxIndirectParam( GetModuleHandle(NULL),
                                     (LPCDLGTEMPLATE)Resource("IDD_CONFIG", RT_DIALOG),
                                      GetActiveWindow(), 
                                      Unit_HomingHub_Config_dialog, 
                                     (LPARAM)unit                    ) ;
    if(status)  return(-1) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� PLUGIN                  */
/*								    */
/*    PLUGIN <��� ����������> <����> <��� ����������>               */

  int  RSS_Module_HomingHub::cPlugin(char *cmd)

{
#define   _PARS_MAX  10

                   char *pars[_PARS_MAX] ;
                   char *name ;
     RSS_Unit_HomingHub *hub ;
                   char *stage ;
                   char *unit_name ;
                   char *unit_type ;
        RSS_Unit_Homing *unit ;
             RSS_Object *object ;
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
                                     SEND_ERROR("�� ����� ������ ����������") ;
                                         return(-1) ;
                                 }

                     name=pars[0] ;
                    stage=pars[1] ;
                unit_name=pars[2] ;
                unit_type=pars[3] ;

/*---------------------------------------- �������� ����� ���������� */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* ���� ��� �� ������... */
                           SEND_ERROR("�� ������ ��� ���������a. \n"
                                      "��������: PLUGIN <���_���������a> ...") ;
                                     return(-1) ;
                         }

       hub=(RSS_Unit_HomingHub *)FindUnit(name) ;                   /* ���� ��������� �� ����� */
    if(hub==NULL)  return(-1) ;

/*---------------------------------------------- �������� ���������� */

    if(strcmp(stage, "1") &&
       strcmp(stage, "2")   ) {
                           SEND_ERROR("������ ������ ��������� ������ ���� 1 ��� 2") ;
                                     return(-1) ;
                              }
    if(unit_name   ==NULL ||
       unit_name[0]==  0    ) {
                           SEND_ERROR("�� ������ ��� ������������� ����������") ;
                                     return(-1) ;
                              }
    if(unit_type   ==NULL ||
       unit_type[0]==  0    ) {
                           SEND_ERROR("�� ����� ��� ������������� ����������") ;
                                     return(-1) ;
                              }
/*---------------------------------------------- �������� ���������� */

    if((stage[0]=='1' && hub->units_1[2]!=NULL) ||
       (stage[0]=='2' && hub->units_2[2]!=NULL)   ) {
             SEND_ERROR("��������� ��� ����� ��� ������ ������ ���������") ;
                                     return(-1) ;
                                                    }
/*-------------------------------- �������� � ����������� ���������� */

             object=hub->Owner ;

       unit=AddUnit(object, unit_name, unit_type, error) ;        
    if(unit==NULL) {
                       SEND_ERROR(error) ;
                             return(-1) ;
                   }

    if(stage[0]=='1') {
        for(i=0 ; i<3 ; i++) if(hub->units_1[i]==NULL) {  hub->units_1[i]=unit ;  break ;  }
                      } 
    else              {
        for(i=0 ; i<3 ; i++) if(hub->units_2[i]==NULL) {  hub->units_2[i]=unit ;  break ;  }
                      } 
/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*             ����� ������� ���� HomingHub �� �����                */

  RSS_Unit *RSS_Module_HomingHub::FindUnit(char *name)

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

     if(strcmp(unit->Type, "HomingHub")) {

           SEND_ERROR("��������� �� �������� ����������� ���� HomingHub") ;
                            return(NULL) ;
                                         }
/*-------------------------------------------------------------------*/ 

   return((RSS_Unit *)unit) ;

#undef   OBJECTS
#undef   OBJECTS_CNT

}


/*********************************************************************/
/*                                                                   */
/*                 ���������� ���������� � �������                   */

  class RSS_Unit_Homing *RSS_Module_HomingHub::AddUnit(RSS_Object *object, char *unit_name, char *unit_type, char *error)

{
        RSS_Kernel *unit_module ;
   RSS_Unit_Homing *unit ;
               int  i ;

/*---------------------------------------- �������� ����� ���������� */

   for(i=0 ; i<object->Units.List_cnt ; i++)
     if(!stricmp(unit_name, object->Units.List[i].object->Name))  break ;

      if(i<object->Units.List_cnt) {
            strcpy(error, "� ������ ������� ��� ������� ��������� � ����� ������") ;
                                     return(NULL) ;
                                   }
/*------------------------------------ ������������� ���� ���������� */

#define   MODULES       RSS_Kernel::kernel->modules 
#define   MODULES_CNT   RSS_Kernel::kernel->modules_cnt

                 unit_module=NULL ;

   for(i=0 ; i<MODULES_CNT ; i++) 
     if(MODULES[i].entry->category      !=NULL &&
        MODULES[i].entry->identification!=NULL   )
      if(!stricmp("Unit",     MODULES[i].entry->category      ) &&
         !stricmp( unit_type, MODULES[i].entry->identification)   )  unit_module=MODULES[i].entry ;
      
      if(unit_module==NULL) {
                strcpy(error, "����������� ��� ����������") ;
                                     return(NULL) ;
                            }

#undef    MODULES
#undef    MODULES_CNT

/*-------------------------------- �������� � ����������� ���������� */

        unit=(RSS_Unit_Homing *)unit_module->vCreateObject(NULL) ;
     if(unit==NULL)   return(NULL) ;

        strncpy(unit->Name, unit_name, sizeof(unit->Name)-1) ;
                unit->Owner=object ;

                  object->Units.Add(unit, "") ;

/*-------------------------------------------------------------------*/

  return(unit) ;
}


/********************************************************************/
/********************************************************************/
/**							           **/
/**         �������� ������ ���������� "��������������� ���"       **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       ����������� ������			    */

     RSS_Unit_HomingHub::RSS_Unit_HomingHub(void)

{
   strcpy(Type, "HomingHub") ;
          Module=&ProgramModule ;

   Parameters    =NULL ;
   Parameters_cnt=  0 ;

    memset(units_1, 0, sizeof(units_1)) ;
    memset(units_2, 0, sizeof(units_2)) ;
}


/********************************************************************/
/*								    */
/*		        ���������� ������			    */

    RSS_Unit_HomingHub::~RSS_Unit_HomingHub(void)

{
      vFree() ;
}


/********************************************************************/
/*								    */
/*		       ������������ ��������                        */

  void   RSS_Unit_HomingHub::vFree(void)

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

    class RSS_Object *RSS_Unit_HomingHub::vCopy(char *name)

{
         RSS_Model_data  create_data ;
     RSS_Unit_HomingHub *unit ;
   
/*------------------------------------- ����������� �������� ������� */

      memset(&create_data, 0, sizeof(create_data)) ;

       unit=(RSS_Unit_HomingHub *)this->Module->vCreateObject(&create_data) ;
    if(unit==NULL)  return(NULL) ;

/*
             unit->tripping_type    =this->tripping_type ;
             unit->tripping_altitude=this->tripping_altitude ;
             unit->tripping_time    =this->tripping_time ;
             unit->    load_type    =this->    load_type ;
             unit->     hit_range   =this->     hit_range ;
             unit->   blast_radius  =this->   blast_radius ;
      strcpy(unit->     sub_unit,    this->     sub_unit) ;   
             unit->     sub_object  =this->     sub_object ;
             unit->     sub_count   =this->     sub_count ;
             unit->     sub_step    =this->     sub_step ;
             unit->     sub_series  =this->     sub_series ;
             unit->     sub_range   =this->     sub_range ;
*/
/*-------------------------------------------------------------------*/

   return(unit) ;
}


/********************************************************************/
/*								    */
/*                        ����������� ��������                      */

     int  RSS_Unit_HomingHub::vSpecial(char *oper, void *data)
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

     int  RSS_Unit_HomingHub::vCalculateStart(double  t)
{
   int  i ;


     for(i=0 ; i<_UNITS_BY_STAGE_MAX ; i++) 
       if(units_1[i]!=NULL)  units_1[i]->vCalculateStart(t) ;              

     for(i=0 ; i<_UNITS_BY_STAGE_MAX ; i++) 
       if(units_2[i]!=NULL)  units_2[i]->vCalculateStart(t) ;              

  return(0) ;
}


/********************************************************************/
/*								    */
/*                   ������ ��������� ���������                     */

     int  RSS_Unit_HomingHub::vCalculate(double t1, double t2, char *callback, int cb_size)
{

   int  i ;


   if(this->stage==1) {
                for(i=0 ; i<_UNITS_BY_STAGE_MAX ; i++) 
                  if(units_1[i]!=NULL)  units_1[i]->vCalculate(t1, t2, callback, cb_size) ;
                      }
    else 
    if(this->stage==2) {
                for(i=0 ; i<_UNITS_BY_STAGE_MAX ; i++) 
                  if(units_2[i]!=NULL)  units_2[i]->vCalculate(t1, t2, callback, cb_size) ;
                       }

  return(0) ;
}


/********************************************************************/
/*								    */
/*      ����������� ���������� ������� ��������� ���������          */

     int  RSS_Unit_HomingHub::vCalculateShow(double  t1, double  t2)
{
  return(0) ;
}

/*********************************************************************/
/*								     */
/*              	  ���������� �������� ���         	     */

    int  RSS_Unit_HomingHub::vSetHomingControl(char *regime)

{
   char  text[1024] ;
   char *command ;
   char *next ;
    int  i ;

/*------------------------------------------------------- ���������� */

            memset(text, 0, sizeof(text)) ;
           strncpy(text, regime, sizeof(text)-1) ;

/*--------------------------------------------------- ������� ������ */

   for(command=text, next=text ; next!=NULL ; command=next+1) {

            next=strchr(command, ';') ;
         if(next!=NULL)  *next=0 ;

/*----------------------------------------------------- ������� ���� */

      if(!memicmp(command, "stage", 5)) {

         if(command[6]=='1')   this->stage=1 ;
         else
         if(command[6]=='2')   this->stage=2 ;
         else                {
                                    sprintf(text, "Illegal stage index in HomingHub : %s", command) ;
                                 SEND_ERROR(text) ;
                                      return(-1) ;          
                             }
                                        }
/*--------------------------------------- ������� ������� ���������� */

      else                              {

           if(this->stage==1) {
                for(i=0 ; i<_UNITS_BY_STAGE_MAX ; i++) 
                  if(units_1[i]!=NULL)  units_1[i]->vSetHomingControl(command) ;              
                              }
           else 
           if(this->stage==2) {
                for(i=0 ; i<_UNITS_BY_STAGE_MAX ; i++) 
                  if(units_2[i]!=NULL)  units_2[i]->vSetHomingControl(command) ;              
                              }

                                        }
/*--------------------------------------------------- ������� ������ */
                                                              }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/*********************************************************************/
/*								     */
/*              	  ����������� �� ����            	     */

    int  RSS_Unit_HomingHub::vGetHomingDirection(RSS_Point *direction)

{
   int  i ;


   if(this->stage==1) {
                for(i=0 ; i<_UNITS_BY_STAGE_MAX ; i++) 
                  if(units_1[i]!=NULL)  units_1[i]->vGetHomingDirection(direction) ;              
                      }
    else 
    if(this->stage==2) {
                for(i=0 ; i<_UNITS_BY_STAGE_MAX ; i++) 
                  if(units_2[i]!=NULL)  units_2[i]->vGetHomingDirection(direction) ;              
                       }

   return(0) ;
}


/*********************************************************************/
/*								     */
/*             	 ������������� ��������� ����           	     */

    int  RSS_Unit_HomingHub::vGetHomingPosition(RSS_Point *position)

{
   int  i ;


   if(this->stage==1) {
                for(i=0 ; i<_UNITS_BY_STAGE_MAX ; i++) 
                  if(units_1[i]!=NULL)  units_1[i]->vGetHomingPosition(position) ; 
                      }
    else 
    if(this->stage==2) {
                for(i=0 ; i<_UNITS_BY_STAGE_MAX ; i++) 
                  if(units_2[i]!=NULL)  units_2[i]->vGetHomingPosition(position) ;
                       }

   return(0) ;
}


/*********************************************************************/
/*								     */
/*               	 ��������� �� ����                 	     */

    int  RSS_Unit_HomingHub::vGetHomingDistance(double *distance)

{
   int  i ;


   if(this->stage==1) {
                for(i=0 ; i<_UNITS_BY_STAGE_MAX ; i++) 
                  if(units_1[i]!=NULL)  units_1[i]->vGetHomingDistance(distance) ;              
                      }
    else 
    if(this->stage==2) {
                for(i=0 ; i<_UNITS_BY_STAGE_MAX ; i++) 
                  if(units_2[i]!=NULL)  units_2[i]->vGetHomingDistance(distance) ;              
                       }

   return(0) ;
}


/*********************************************************************/
/*								     */
/*               	 �������� ��������� � �����            	     */

    int  RSS_Unit_HomingHub::vGetHomingClosingSpeed(double *velocity)

{
   int  i ;


   if(this->stage==1) {
                for(i=0 ; i<_UNITS_BY_STAGE_MAX ; i++) 
                  if(units_1[i]!=NULL)  units_1[i]->vGetHomingClosingSpeed(velocity) ;
                      }
    else 
    if(this->stage==2) {
                for(i=0 ; i<_UNITS_BY_STAGE_MAX ; i++) 
                  if(units_2[i]!=NULL)  units_2[i]->vGetHomingClosingSpeed(velocity) ;              
                       }

   return(0) ;
}



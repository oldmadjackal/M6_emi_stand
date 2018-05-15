/********************************************************************/
/*								    */
/*            ������ ���������� ����������� "������� ������"        */
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


#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Unit\RSS_Unit.h"
#include "..\RSS_Kernel\RSS_Kernel.h"
#include "..\RSS_Model\RSS_Model.h"

#include "U_ModelSimple.h"

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

     static   RSS_Module_ModelSimple  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	����������������� ����                      */

 U_MODEL_SIMPLE_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 U_MODEL_SIMPLE_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/********************************************************************/
/**							           **/
/**            �������� ������ ������ ���������� ��������          **/
/**                         "������� ������"	                   **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*                            ������ ������                         */

  struct RSS_Module_ModelSimple_instr  RSS_Module_ModelSimple_InstrList[]={

 { "help",      "?",  "#HELP - ������ ��������� ������", 
                       NULL,
                      &RSS_Module_ModelSimple::cHelp   },
 { "info",      "i",  "#INFO - ������ ���������� �� �������",
                      " INFO <���> \n"
                      "   ������ �������� ���������� �� ������� � ������� ����\n"
                      " INFO/ <���> \n"
                      "   ������ ������ ���������� �� ������� � ��������� ����",
                      &RSS_Module_ModelSimple::cInfo },
 { "pars",      "p",  "#PARS - ������� ���������� ������ � ���������� ������", 
                       NULL,
                      &RSS_Module_ModelSimple::cPars   },
 { "mass",      "m",  "#MASS - ������� ����� ��� (��� ���������)", 
                       NULL,
                      &RSS_Module_ModelSimple::cMass   },
 { "slide",     "sl", "#SLIDE - ������� ����� ������������", 
                       NULL,
                      &RSS_Module_ModelSimple::cSlide  },
 { "deviation", "dv", "#DEVIATION - ������� ���������� ���������� �� �����", 
                      " DEVIATION <���> <azim> <elev>\n"
                      "   ������� ������������ ���������� �� ����������� � ���� ������, �������\n"
                      " DEVIATION/a <���> <azim>\n"
                      "   ������� ������������ ���������� �� �����������, �������\n"
                      " DEVIATION/e <���> <elev>\n"
                      "   ������� ������������ ���������� �� ���� ������, �������\n",
                      &RSS_Module_ModelSimple::cDeviation  },
 {  NULL }
                                                            } ;

/********************************************************************/
/*								    */
/*		     ����� ����� ������             		    */

    struct RSS_Module_ModelSimple_instr *RSS_Module_ModelSimple::mInstrList=NULL ;


/********************************************************************/
/*								    */
/*		       ����������� ������			    */

     RSS_Module_ModelSimple::RSS_Module_ModelSimple(void)

{

/*---------------------------------------------------- ������������� */

	   keyword="EmiStand" ;
    identification="ModelSimple" ;
          category="Unit" ;
         lego_type="Model" ;

        mInstrList=RSS_Module_ModelSimple_InstrList ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        ���������� ������			    */

    RSS_Module_ModelSimple::~RSS_Module_ModelSimple(void)

{
}


/********************************************************************/
/*								    */
/*		      �������� �������                              */

  RSS_Object *RSS_Module_ModelSimple::vCreateObject(RSS_Model_data *data)

{
   RSS_Unit_ModelSimple *unit ;
                    int  i ;
 
/*---------------------------------------------- �������� ���������� */

       unit=new RSS_Unit_ModelSimple ;
    if(unit==NULL) {
               SEND_ERROR("������ ModelSimple: ������������ ������ ��� �������� ����������") ;
                        return(NULL) ;
                   }

             unit->Module=this ;

      strcpy(unit->Decl, "������ (�������)") ;

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

     int  RSS_Module_ModelSimple::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- �������� ������ */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s -  ������� ������", identification) ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		        ��������� �������       		    */

  int  RSS_Module_ModelSimple::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* ���� ������ ������ ������� */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "MODELSIMPLE"
#define  _SECTION_SHRT_NAME   "MODELSIMPLE"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Object ModelSimple:") ;
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
       if(status)  SEND_ERROR("������ ModelSimple: ����������� �������") ;
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

  int  RSS_Module_ModelSimple::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Unit_ModelSimple_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� INFO                    */
/*								    */
/*        INFO   <���>                                              */
/*        INFO/  <���>                                              */

  int  RSS_Module_ModelSimple::cInfo(char *cmd)

{
                     char  *name ;
     RSS_Unit_ModelSimple  *unit ;
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

       unit=(RSS_Unit_ModelSimple *)FindUnit(name) ;                /* ���� ��������� �� ����� */
    if(unit==NULL)  return(-1) ;

/*-------------------------------------------- ������������ �������� */

      sprintf(text, "%s\r\n%s\r\n"
                    "Owner     %s\r\n" 
                    "Mass      % 5lf\r\n" 
                    "Slideway  % 5lf\r\n" 
                    "\r\n",
                        unit->Name,      unit->Type, 
                        unit->Owner->Name, 
                        unit->mass, unit->slideway 
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
/*		      ���������� ���������� PARS                    */
/*								    */
/*     PARS <��� ���������>                                         */

  int  RSS_Module_ModelSimple::cPars(char *cmd)

{
#define   _PARS_MAX  10

                  char *pars[_PARS_MAX] ;
                  char *name ;
  RSS_Unit_ModelSimple  *unit ;
                   int  status ;
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

/*------------------------------------------- �������� ����� ������� */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* ���� ��� �� ������... */
                           SEND_ERROR("�� ������ ��� ���������. \n"
                                      "��������: PARS <���_���������> ...") ;
                                     return(-1) ;
                         }

       unit=(RSS_Unit_ModelSimple *)FindUnit(name) ;                /* ���� ��������� �� ����� */
    if(unit==NULL)  return(-1) ;

/*--------------------------------------- ������� � ���������� ����� */

        status=DialogBoxIndirectParam( GetModuleHandle(NULL),
                                      (LPCDLGTEMPLATE)Resource("IDD_PARS", RT_DIALOG),
                                       GetActiveWindow(), 
                                       Unit_ModelSimple_Pars_dialog, 
                                      (LPARAM)unit                   ) ;
          return(status) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� MASS                    */
/*								    */
/*       MASS <���> <�����>                                         */

  int  RSS_Module_ModelSimple::cMass(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

                  char  *pars[_PARS_MAX] ;
                  char  *name ;
                  char **xyz ;
                double   coord[_COORD_MAX] ;
                   int   coord_cnt ;
  RSS_Unit_ModelSimple  *unit ;
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
                                      "��������: MASS <���_���������> ...") ;
                                     return(-1) ;
                         }

       unit=(RSS_Unit_ModelSimple *)FindUnit(name) ;                /* ���� ��������� �� ����� */
    if(unit==NULL)  return(-1) ;

/*------------------------------------------------ ������ ���������� */

    for(i=0 ; xyz[i]!=NULL && i<_COORD_MAX ; i++) {

             coord[i]=strtod(xyz[i], &end) ;
        if(*end!=0) {  
                       SEND_ERROR("������������ �������� �����") ;
                                       return(-1) ;
                    }
                                                  }

                             coord_cnt=i ;

                        error= NULL ;
      if(coord_cnt==0)  error="�� ������� �����" ;

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

/*---------------------------------------------------- ������� ����� */

                 unit->mass=coord[0] ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� SLIDE                   */
/*								    */
/*       SLIDE <���> <�����>                                        */

  int  RSS_Module_ModelSimple::cSlide(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

                  char  *pars[_PARS_MAX] ;
                  char  *name ;
                  char **xyz ;
                double   coord[_COORD_MAX] ;
                   int   coord_cnt ;
  RSS_Unit_ModelSimple  *unit ;
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
                                      "��������: SLIDE <���_���������> ...") ;
                                     return(-1) ;
                         }

       unit=(RSS_Unit_ModelSimple *)FindUnit(name) ;                /* ���� ��������� �� ����� */
    if(unit==NULL)  return(-1) ;

/*------------------------------------------------ ������ ���������� */

    for(i=0 ; xyz[i]!=NULL && i<_COORD_MAX ; i++) {

             coord[i]=strtod(xyz[i], &end) ;
        if(*end!=0) {  
                       SEND_ERROR("������������ �������� ����� ������������") ;
                                       return(-1) ;
                    }
                                                  }

                             coord_cnt=i ;

                        error= NULL ;
      if(coord_cnt==0)  error="�� ������� ����� ������������" ;

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

/*---------------------------------------------------- ������� ����� */

                 unit->slideway=coord[0] ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*               ���������� ���������� DEVIATION                    */
/*								    */
/*   DEVIATION   <���> <�� �����������> <�� ���� ������>            */
/*   DEVIATION/A <���> <�� �����������>                             */
/*   DEVIATION/E <���> <�� ���� ������>                             */

  int  RSS_Module_ModelSimple::cDeviation(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

                  char  *pars[_PARS_MAX] ;
                  char  *name ;
                  char **xyz ;
                double   coord[_COORD_MAX] ;
                   int   coord_cnt ;
  RSS_Unit_ModelSimple  *unit ;
                   int   a_flag ;            /* ���� ������� �� �� ����������� */
                   int   e_flag ;            /* ���� ������� �� �� ���� ������ */
                  char  *error ;
                  char  *end ;
                   int   i ;

/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
                          a_flag=0 ;
                          e_flag=0 ;

       if(*cmd=='/') {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("������������ ������ �������") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, 'a')!=NULL ||
                   strchr(cmd, 'A')!=NULL   )  a_flag=1 ;
                if(strchr(cmd, 'e')!=NULL ||
                   strchr(cmd, 'E')!=NULL   )  e_flag=1 ;

                           cmd=end+1 ;
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

/*---------------------------------------- �������� ����� ���������a */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* ���� ��� �� ������... */
                           SEND_ERROR("�� ������ ��� ���������. \n"
                                      "��������: SLIDE <���_���������> ...") ;
                                     return(-1) ;
                         }

       unit=(RSS_Unit_ModelSimple *)FindUnit(name) ;                /* ���� ��������� �� ����� */
    if(unit==NULL)  return(-1) ;

/*------------------------------------------------ ������ ���������� */

    for(i=0 ; xyz[i]!=NULL && i<_COORD_MAX ; i++) {

             coord[i]=strtod(xyz[i], &end) ;
        if(*end!=0) {  
                       SEND_ERROR("������������ �������� ���������") ;
                                       return(-1) ;
                    }
                                                  }

                             coord_cnt=  i ;

                        error= NULL ;
      if(coord_cnt==0)  error="�� ������� ���������" ;

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

/*------------------------------------------------- ������� �������� */

        if(a_flag) {
                               unit->s_azim=coord[0] ;
                   }
   else if(e_flag) {
                               unit->s_elev=coord[0] ;
                   }
   else            {
                               unit->s_azim=coord[0] ;
              if(coord_cnt>1)  unit->s_elev=coord[1] ;
                   }
/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*	     ����� ������� ���� ModelSimple �� �����                */

  RSS_Unit *RSS_Module_ModelSimple::FindUnit(char *name)

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

     if(strcmp(unit->Type, "ModelSimple")) {

           SEND_ERROR("��������� �� �������� ����������� ���� ModelSimple") ;
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
/**	  �������� ������ ���������� "������� ������"	           **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       ����������� ������			    */

     RSS_Unit_ModelSimple::RSS_Unit_ModelSimple(void)

{
   strcpy(Type, "ModelSimple") ;
          Module=&ProgramModule ;

   Parameters    =NULL ;
   Parameters_cnt=  0 ;

              t_0=  0. ; 
             mass=  0. ; 
         slideway=  0. ;
           s_azim=  0. ;
           s_elev=  0. ;
    engine_thrust=  0. ; 
    engine_mass  =  0. ; 
}


/********************************************************************/
/*								    */
/*		        ���������� ������			    */

    RSS_Unit_ModelSimple::~RSS_Unit_ModelSimple(void)

{
      vFree() ;
}


/********************************************************************/
/*								    */
/*		       ������������ ��������                        */

  void   RSS_Unit_ModelSimple::vFree(void)

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

    class RSS_Object *RSS_Unit_ModelSimple::vCopy(char *name)

{
        RSS_Model_data  create_data ;
  RSS_Unit_ModelSimple *unit ;
   
/*------------------------------------- ����������� �������� ������� */

      memset(&create_data, 0, sizeof(create_data)) ;

       unit=(RSS_Unit_ModelSimple *)this->Module->vCreateObject(&create_data) ;
    if(unit==NULL)  return(NULL) ;

/*------------------------------------- ����������� �������� ������� */

             unit->mass    =this->mass ;
             unit->slideway=this->slideway ;
             unit->s_azim  =this->s_azim ;
             unit->s_elev  =this->s_elev ;

/*-------------------------------------------------------------------*/

   return(unit) ;
}


/********************************************************************/
/*								    */
/*                        ����������� ��������                      */

     int  RSS_Unit_ModelSimple::vSpecial(char *oper, void *data)
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

     int  RSS_Unit_ModelSimple::vCalculateStart(double  t)
{
/*------------------------------------ ������������� ������� ������ */

    this->t_0          =t ;

    this->engine_thrust=0. ;
    this->engine_mass  =0. ;

/*------------------------------------------------ ������ ���������� */

    this->Owner->a_azim+=this->Module->gGaussianValue(0., this->s_azim) ;
    this->Owner->a_elev+=this->Module->gGaussianValue(0., this->s_elev) ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*                   ������ ��������� ���������                     */

     int  RSS_Unit_ModelSimple::vCalculate(double t1, double t2, char *callback, int cb_size)
{
  RSS_Object *parent ;
      double  Ky ;
      double  Kn ;
      double  Vy ;
      double  Vn ;
      double  t_s ;
      double  a ;
      double  f ;
      double  g ;
      double  dt ;
      double  dVy ;
      double  dVn ;
      double  dH ;
      double  dL ;

/*------------------------------------------------ �o�������� ������ */

           t1-=this->t_0 ;
           t2-=this->t_0 ;

        parent=this->Owner ;

            dt=t2-t1 ;
             f=engine_thrust/(mass+engine_mass) ;
            Vy=     parent->y_velocity ;
            Vn=sqrt(parent->x_velocity*parent->x_velocity+
                    parent->z_velocity*parent->z_velocity ) ;

/*-------------------------------------------- ����������� ���� ���� */

              if(f>0)  t_s=sqrt(2.*slideway/f) ;                    /* �������� �� ������������ �������� � ���������� ���������� */
              else     t_s= 0 ;

              if(t1== 0 ) {                                         /* ����� */
                             Ky=sin(parent->a_elev*_GRD_TO_RAD) ;
                             Kn=cos(parent->a_elev*_GRD_TO_RAD) ;
                          }
         else if(t1< t_s) {                                         /* �������� �� ������������ */
                             Ky=sin(parent->a_elev*_GRD_TO_RAD) ;
                             Kn=cos(parent->a_elev*_GRD_TO_RAD) ;
                          }
         else             {                                         /* ��������� ����� */
                             Ky=Vy/sqrt(Vy*Vy+Vn*Vn) ;
                             Kn=Vn/sqrt(Vy*Vy+Vn*Vn) ;
                          }
/*--------------------------- ������ ���������� � ��������� �������� */

      if(t1== 0 || 
         t1< t_s  ) g=0. ;
      else          g=9.8 ;

           a = f*Ky-g ;                                             /* ������������ ���������� */ 
           dVy=dt*a ;
           dH =Vy*dt+0.5*a*dt*dt ;

            a = f*Kn ;                                              /* ���������� ��������� */
           dVn=dt*a ;
           dL =Vn*dt+0.5*a*dt*dt ;

            Vy+=dVy ;
            Vn+=dVn ;

/*------------------------------------- ������� � ������� ���������� */

             parent->x_velocity=Vn*sin(parent->a_azim*_GRD_TO_RAD) ;
             parent->y_velocity=Vy ;
             parent->z_velocity=Vn*cos(parent->a_azim*_GRD_TO_RAD) ;

             parent->a_elev    =atan2(Vy, Vn)*_RAD_TO_GRD ;

             parent->x_base   +=dL*sin(parent->a_azim*_GRD_TO_RAD) ;
             parent->y_base   +=dH ;
             parent->z_base   +=dL*cos(parent->a_azim*_GRD_TO_RAD) ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*      ����������� ���������� ������� ��������� ���������          */

     int  RSS_Unit_ModelSimple::vCalculateShow(double  t1, double  t2)
{
  return(0) ;
}


/*********************************************************************/
/*								     */
/*              ���������� ����������������� �������������     	     */

    int  RSS_Unit_ModelSimple::vSetAeroControl(RSS_Unit_Aero_Control *aeros, int  aeros_cnt)

{
   return(0) ;
}


/*********************************************************************/
/*								     */
/*              	  ���� ���������                	     */

    int  RSS_Unit_ModelSimple::vSetEngineThrust(RSS_Unit_Engine_Thrust *thrust, int  thrust_cnt)

{
   this->engine_thrust=thrust->z ;

   return(0) ;
}


/*********************************************************************/
/*								     */
/*             	  ����� � ��������� ������ ���� ���������            */

    int  RSS_Unit_ModelSimple::vSetEngineMass(double  mass, RSS_Point *center)

{
   this->engine_mass=mass ;

   return(0) ;
}


/*********************************************************************/
/*								     */
/*             	  ������� ������� ���������                          */

    int  RSS_Unit_ModelSimple::vSetEngineMI(double  Ix, double  Iy, double  Iz)

{
   return(0) ;
}


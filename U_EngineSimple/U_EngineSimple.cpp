/********************************************************************/
/*								    */
/*            ������ ���������� ����������� "������� ��"            */
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
#include "..\Ud_tools\UserDlg.h"

#include "U_EngineSimple.h"

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

     static   RSS_Module_EngineSimple  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	����������������� ����                      */

 U_ENGINE_SIMPLE_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 U_ENGINE_SIMPLE_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/********************************************************************/
/**							           **/
/**            �������� ������ ������ ���������� ��������          **/
/**                  "������� �������� ���������"	           **/
/**							           **/
/********************************************************************/
/********************************************************************/


/********************************************************************/
/*								    */
/*                            ������ ������                         */

  struct RSS_Module_EngineSimple_instr  RSS_Module_EngineSimple_InstrList[]={

 { "help",    "?",  "#HELP   - ������ ��������� ������", 
                     NULL,
                    &RSS_Module_EngineSimple::cHelp   },
 { "info",    "i",  "#INFO - ������ ���������� �� �������",
                    " INFO <���> \n"
                    "   ������ �������� ���������� �� ������� � ������� ����\n"
                    " INFO/ <���> \n"
                    "   ������ ������ ���������� �� ������� � ��������� ����",
                    &RSS_Module_EngineSimple::cInfo },
 { "profile", "p",  "#PROFILE - �������� ������� ������",
                    " PROFILE <���> <��� ����� �������>\n"
                    "   ��������� ������� ������ �� �����",
                    &RSS_Module_EngineSimple::cProfile },
 { "sigma",   "s",  "#SIGMA - ������-������������ ���������� ����������",
                    " SIGMA <���> <��������>\n"
                    "   ������ ������������� ������-������������ ���������� ���������� ��������� �� ����������",
                    &RSS_Module_EngineSimple::cSigma },
 { "view",    "v",  "#VIEW - �������� ������� ������",
                    " VIEW <���>\n"
                    "   �������� ������� ������� ������",
                    &RSS_Module_EngineSimple::cView },
 {  NULL }
                                                            } ;

/********************************************************************/
/*								    */
/*		     ����� ����� ������             		    */

    struct RSS_Module_EngineSimple_instr *RSS_Module_EngineSimple::mInstrList=NULL ;


/********************************************************************/
/*								    */
/*		       ����������� ������			    */

     RSS_Module_EngineSimple::RSS_Module_EngineSimple(void)

{
  static  WNDCLASS  View_wnd ;
              char  text[1024] ;

/*---------------------------------------------------- ������������� */

	   keyword="EmiStand" ;
    identification="EngineSimple" ;
          category="Unit" ;
         lego_type="Engine" ;

        mInstrList=RSS_Module_EngineSimple_InstrList ;

/*--------------------------- ����������� ������ ���� UD_Show_view2D */

	View_wnd.lpszClassName="U_EngineSimple_view_class" ;
	View_wnd.hInstance    = GetModuleHandle(NULL) ;
	View_wnd.lpfnWndProc  = UD_diagram_2D_prc ;
	View_wnd.hCursor      = LoadCursor(NULL, IDC_ARROW) ;
	View_wnd.hIcon        =  NULL ;
	View_wnd.lpszMenuName =  NULL ;
	View_wnd.hbrBackground=  NULL ;
	View_wnd.style        =    0 ;
	View_wnd.hIcon        =  NULL ;

    if(!RegisterClass(&View_wnd)) {
              sprintf(text, "U_EngineSimple_view_class register error %d", GetLastError()) ;
           SEND_ERROR(text) ;
                    return ;
                                  }
/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        ���������� ������			    */

    RSS_Module_EngineSimple::~RSS_Module_EngineSimple(void)

{
}


/********************************************************************/
/*								    */
/*		      �������� �������                              */

  RSS_Object *RSS_Module_EngineSimple::vCreateObject(RSS_Model_data *data)

{
   RSS_Unit_EngineSimple *unit ;
                     int  i ;
 
/*---------------------------------------------- �������� ���������� */

       unit=new RSS_Unit_EngineSimple ;
    if(unit==NULL) {
               SEND_ERROR("������ EngineSimple: ������������ ������ ��� �������� ����������") ;
                        return(NULL) ;
                   }

             unit->Module=this ;

      strcpy(unit->Decl, "�������� ��������� (�������)") ;

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

     int  RSS_Module_EngineSimple::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- �������� ������ */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s -  ������� �������� ���������", identification) ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		        ��������� �������       		    */

  int  RSS_Module_EngineSimple::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* ���� ������ ������ ������� */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "ENGINESIMPLE"
#define  _SECTION_SHRT_NAME   "ENGINE"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Object EngineSimple:") ;
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
       if(status)  SEND_ERROR("������ EngineSimple: ����������� �������") ;
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

  int  RSS_Module_EngineSimple::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Unit_EngineSimple_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� INFO                    */
/*								    */
/*        INFO   <���>                                              */
/*        INFO/  <���>                                              */

  int  RSS_Module_EngineSimple::cInfo(char *cmd)

{
                     char  *name ;
    RSS_Unit_EngineSimple  *unit ;
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

       unit=(RSS_Unit_EngineSimple *)FindUnit(name) ;               /* ���� ��������� �� ����� */
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
/*		      ���������� ���������� SIGMA                   */
/*								    */
/*      SIGMA <���> <��������>                                      */

  int  RSS_Module_EngineSimple::cSigma(char *cmd)

{
#define   _PARS_MAX   4
   RSS_Unit_EngineSimple  *unit ;
                    char *pars[_PARS_MAX] ;
                    char *name ;
                    char *end ;
                     int  i ;

/*-------------------------------------- ���������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������ ���������� */
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {

                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

    for(i=0 ; i<_PARS_MAX ; i++)  
      if(pars[i]!=NULL)
       if(*pars[i]==0)  pars[i]=NULL ;

                           name=pars[0] ;

/*---------------------------------------- �������� ����� ���������a */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* ���� ��� �� ������... */
                           SEND_ERROR("�� ������ ��� ����������. \n"
                                      "��������: SIGMA <���_����������> ...") ;
                                     return(-1) ;
                         }

       unit=(RSS_Unit_EngineSimple *)FindUnit(name) ;               /* ���� ��������� �� ����� */
    if(unit==NULL)  return(-1) ;

/*------------------------------------------------- ������� �������� */

  if(pars[1]==NULL) {
                       SEND_ERROR("�� ������ �������� �������������� ������-������������� ����������.\n"
                                  "��������: SIGMA <���> <��������>") ;
                                     return(-1) ;
                    }

        unit->sigma2=strtod(pars[1], &end) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� PROFILE                 */
/*								    */
/*      PROFILE <���> <���� ���������>                              */

  int  RSS_Module_EngineSimple::cProfile(char *cmd)

{
#define   _PARS_MAX   4
   RSS_Unit_EngineSimple  *unit ;
                    char *pars[_PARS_MAX] ;
                    char *name ;
                    char *end ;
                     int  status ;
                     int  i ;

/*-------------------------------------- ���������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������ ���������� */
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {

                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

    for(i=0 ; i<_PARS_MAX ; i++)  
      if(pars[i]!=NULL)
       if(*pars[i]==0)  pars[i]=NULL ;

                           name=pars[0] ;

/*---------------------------------------- �������� ����� ���������a */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* ���� ��� �� ������... */
                           SEND_ERROR("�� ������ ��� ����������. \n"
                                      "��������: PROFILE <���_����������> ...") ;
                                     return(-1) ;
                         }

       unit=(RSS_Unit_EngineSimple *)FindUnit(name) ;               /* ���� ��������� �� ����� */
    if(unit==NULL)  return(-1) ;

/*--------------------------------------- ���������� ������ �� ����� */

  if(pars[1]==NULL) {
                       SEND_ERROR("�� ������ ��� ����� ������� ������.\n"
                                  "��������: PROFILE <���> <���� ���������>") ;
                                     return(-1) ;
                    }

        status=ReadProfile(unit, pars[1]) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� VIEW                    */
/*								    */
/*      VIEW <���>                                                  */

  int  RSS_Module_EngineSimple::cView(char *cmd)

{
#define   _PARS_MAX   4
   RSS_Unit_EngineSimple  *unit ;
                    char *pars[_PARS_MAX] ;
                    char *name ;
                    char *end ;
                     int  i ;

/*-------------------------------------- ���������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������ ���������� */
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {

                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

    for(i=0 ; i<_PARS_MAX ; i++)  
      if(pars[i]!=NULL)
       if(*pars[i]==0)  pars[i]=NULL ;

                           name=pars[0] ;

/*---------------------------------------- �������� ����� ���������a */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* ���� ��� �� ������... */
                           SEND_ERROR("�� ������ ��� ���������. \n"
                                      "��������: VIEW <���_���������> ...") ;
                                     return(-1) ;
                         }

       unit=(RSS_Unit_EngineSimple *)FindUnit(name) ;               /* ���� ��������� �� ����� */
    if(unit==NULL)  return(-1) ;

/*----------------------------------------------- ����������� ������ */

    DialogBoxIndirectParam(GetModuleHandle(NULL),
		           (LPCDLGTEMPLATE)Resource("IDD_VIEW", RT_DIALOG),
			     GetActiveWindow(), Unit_EngineSimple_View_dialog, (LPARAM)unit) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*								    */
/*	   ����� ������� ���� EngineSimple �� �����                 */

  RSS_Unit *RSS_Module_EngineSimple::FindUnit(char *name)

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

     if(strcmp(unit->Type, "EngineSimple")) {

           SEND_ERROR("��������� �� �������� ����������� ���� EngineSimple") ;
                            return(NULL) ;
                                            }
/*-------------------------------------------------------------------*/ 

   return((RSS_Unit *)unit) ;

#undef   OBJECTS
#undef   OBJECTS_CNT

}


/*********************************************************************/
/*								     */
/*              ���������� ����� �������� ������� ������             */
/*								     */
/*   ��������� �����:						     */
/*  FIELDS <��� 1> ... <��� N>  				     */
/*  <����� 1> <�������� 1> ... <�������� N>			     */
/*  . . . 							     */
/*  <����� K> <�������� 1> ... <�������� N>			     */
/*								     */
/*  FIELDS � M                                                       */
/*  0.00 100 21                                                      */
/*  1.89 100  0                                                      */
/*  1.90   0  0                                                      */


  int  RSS_Module_EngineSimple::ReadProfile(RSS_Unit_EngineSimple *unit, char *path)

{
                           FILE *file ;
                           char *fields[30] ;
                           char  fields_text[1024] ;
  RSS_Unit_EngineSimple_Profile  frame ;
                           char  text[1024] ;
                           char  error[1024] ;
                           char *words[30] ;
                           char *end ;
                            int  row ;
                            int  i ;

/*--------------------------------------------------- �������� ����� */

       file=fopen(path, "rt") ;
    if(file==NULL) {
                          sprintf(error, "������ %d �������� ����� %s", errno, path) ;
                       SEND_ERROR(error) ;
                                       return(-1) ;
                   }
/*------------------------------------------------ ���������� ������ */

       memset(fields,      0, sizeof(fields     )) ;
       memset(fields_text, 0, sizeof(fields_text)) ;

        unit->profile_cnt=0 ;

                      row=0 ;

   while(1) {                                                       /* CIRCLE.1 - ��������� ������ ���� */

/*-------------------------------------- ���������� ��������� ������ */

                      row++ ;

                      memset(text, 0, sizeof(text)) ;
                   end=fgets(text, sizeof(text)-1, file) ;          /* ��������� ������ */
                if(end==NULL)  break ;

            if(text[0]==';')  continue ;                            /* �������� ����������� */

               end=strchr(text, '\n') ;                             /* ������� ������ ����� ������ */
            if(end!=NULL)  *end=0 ;
               end=strchr(text, '\r') ;
            if(end!=NULL)  *end=0 ;

/*----------------------------------- ��������� ������������� FIELDS */

#define  _KEY_WORD  "FIELDS "
      if(!memicmp(text, _KEY_WORD, strlen(_KEY_WORD))) {

         if(fields[0]!=NULL) {
                                sprintf(error, "������ %d - ������� ������ ��������� ������������ �������� FIELDS", row) ;
                             SEND_ERROR(error) ;
                                  return(-1) ;
                             }
        

               end=text+strlen(_KEY_WORD) ;
            if(*end==0) {
                          sprintf(error, "������ %d - �������� FIELDS �� �������� ������� �����", row) ;
                       SEND_ERROR(error) ;
                             return(-1) ;
                        }  

                strncpy(fields_text, end, sizeof(fields_text)) ;

                      i = 0 ;
               fields[i]=strtok(fields_text, " \t") ;

         while(fields[i]!=NULL && i<30) {
                      i++ ;
               fields[i]=strtok(NULL, " \t") ;
                                        }

                                       continue ;
                                                       }
#undef   _KEY_WORD

/*------------------------------------------- ������ ������ �� ����� */

         if(fields[0]==NULL) {
                                sprintf(error, "������ %d - ������� ������ ���������� � ��������� FIELDS", row) ;
                             SEND_ERROR(error) ;
                                  return(-1) ;
                             }
        
           memset(words, 0, sizeof(words)) ;

                   i = 0 ;
             words[i]=strtok(text, " \t") ;

       while(words[i]!=NULL && i<30) {
                   i++ ;
             words[i]=strtok(NULL, " \t") ;
                                     }
/*---------------------------------------------- ������������ ������ */

          memset(&frame, 0, sizeof(frame)) ;

                  frame.t=strtod(words[0], &end) ;

       for(i=0 ; i<30 ; i++) {

              if(words[i+1]==NULL)  break ;

              if(fields[i]==NULL) {
                                        sprintf(error, "������ %d �������� ������ ����������, ��� ������� � ������ FIELDS", row) ;
                                     SEND_ERROR(error) ;
                                           return(-1) ;
                                  } 

              if(!stricmp(fields[i], "T" ))  frame.thrust=strtod(words[i+1], &end) ;
         else if(!stricmp(fields[i], "M" ))  frame.mass  =strtod(words[i+1], &end) ;
         else if(!stricmp(fields[i], "CX"))  frame.Cx    =strtod(words[i+1], &end) ;
         else if(!stricmp(fields[i], "CY"))  frame.Cy    =strtod(words[i+1], &end) ;
         else if(!stricmp(fields[i], "CZ"))  frame.Cz    =strtod(words[i+1], &end) ;
         else if(!stricmp(fields[i], "MX"))  frame.Mx    =strtod(words[i+1], &end) ;
         else if(!stricmp(fields[i], "MY"))  frame.My    =strtod(words[i+1], &end) ;
         else if(!stricmp(fields[i], "MZ"))  frame.Mz    =strtod(words[i+1], &end) ;
         else                              {
                        sprintf(error, "����������� �������� '%s', �������� T, M, CX, CY, CZ, MX, MY, MZ", fields[i]) ;
                     SEND_ERROR(error) ;
                           return(-1) ;
                                           }
                             }

              if(fields[i]!=NULL) {
                                       sprintf(error, "������ %d �������� ������ ����������, ��� ������� � ������ FIELDS", row) ;
                                    SEND_ERROR(error) ;
                                          return(-1) ;
                                  } 

/*--------------------------------------- ���������� ����� ��������� */

          unit->profile=(RSS_Unit_EngineSimple_Profile *)
                          realloc(unit->profile, (unit->profile_cnt+1)*sizeof(*unit->profile)) ;

          unit->profile[unit->profile_cnt]=frame ;
                        unit->profile_cnt++ ;

/*-------------------------------------------------------------------*/
            }                                                       /* CONTINUE.1 */
/*--------------------------------------------------- �������� ����� */

                fclose(file) ;

/*-------------------------------------- �������� ������������ ����� */

                                             unit->use_mass=0 ;
                                             unit->use_Cxyz=0 ;
                                             unit->use_Mxyz=0 ;

       for(i=0 ; i<30 ; i++) {

              if(fields[i]==NULL)  break ;

              if(!stricmp(fields[i], "M" ))  unit->use_mass=1 ;
         else if(!stricmp(fields[i], "CX"))  unit->use_Cxyz=1 ;
         else if(!stricmp(fields[i], "CY"))  unit->use_Cxyz=1 ;
         else if(!stricmp(fields[i], "CZ"))  unit->use_Cxyz=1 ;
         else if(!stricmp(fields[i], "MX"))  unit->use_Mxyz=1 ;
         else if(!stricmp(fields[i], "MY"))  unit->use_Mxyz=1 ;
         else if(!stricmp(fields[i], "MZ"))  unit->use_Mxyz=1 ;
                             }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/********************************************************************/
/**							           **/
/**	  �������� ������ ���������� "������� ��"	           **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       ����������� ������			    */

     RSS_Unit_EngineSimple::RSS_Unit_EngineSimple(void)

{
   strcpy(Type, "EngineSimple") ;
          Module=&ProgramModule ;

      profile    =NULL ;
      profile_cnt=  0 ;

       sigma2    =  0. ;

   Parameters    =NULL ;
   Parameters_cnt=  0 ;

}


/********************************************************************/
/*								    */
/*		        ���������� ������			    */

    RSS_Unit_EngineSimple::~RSS_Unit_EngineSimple(void)

{
      vFree() ;
}


/********************************************************************/
/*								    */
/*		       ������������ ��������                        */

  void   RSS_Unit_EngineSimple::vFree(void)

{
  int  i ;


  if(this->profile!=NULL)  free(this->profile) ;

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

    class RSS_Unit *RSS_Unit_EngineSimple::vCopy(RSS_Object *owner)

{
        RSS_Model_data  create_data ;
 RSS_Unit_EngineSimple *unit ;
   
/*------------------------------------- ����������� �������� ������� */

      memset(&create_data, 0, sizeof(create_data)) ;

       unit=(RSS_Unit_EngineSimple *)this->Module->vCreateObject(&create_data) ;
    if(unit==NULL)  return(NULL) ;

      strcpy(unit->Name, this->Name) ; 
             unit->Owner=owner ;

    if(owner!=NULL)  owner->Units.Add(unit, "") ;

/*------------------------------------- ����������� �������� ������� */

             unit->profile=(RSS_Unit_EngineSimple_Profile *)
                             calloc(this->profile_cnt, sizeof(*unit->profile)) ;
      memcpy(unit->profile, this->profile, 
                            this->profile_cnt*sizeof(*unit->profile)) ;
             unit->profile_cnt=this->profile_cnt ;

             unit->use_mass=this->use_mass ;
             unit->use_Cxyz=this->use_Cxyz ;
             unit->use_Mxyz=this->use_Mxyz ;

/*-------------------------------------------------------------------*/

   return(unit) ;
}


/********************************************************************/
/*								    */
/*                        ����������� ��������                      */

     int  RSS_Unit_EngineSimple::vSpecial(char *oper, void *data)
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

     int  RSS_Unit_EngineSimple::vCalculateStart(double t)
{
       this->t_0       =t ;
       this->real_value=1. ;

  return(0) ;
}


/********************************************************************/
/*								    */
/*                   ������ ��������� ���������                     */

     int  RSS_Unit_EngineSimple::vCalculate(double t1, double t2, char *callback, int cb_size)
{
  double  t ;
  double  k ;
     int  i ;

/*------------------------------------------------ ���������� ������ */

           t1-=this->t_0 ;
           t2-=this->t_0 ;

/*-------------------------------------------- ����������� ��������� */

               t=0.5*(t2+t1) ;

   for(i=0 ; i<this->profile_cnt ; i++)
     if(t<=this->profile[i].t)  break ;

                                values.t     =t ;

          if(i==  0        ) {
                                values.thrust=0. ;
                                values.mass  =0. ;
                                values.Cx    =0. ;
                                values.Cy    =0. ;
                                values.Cz    =0. ;
                                values.Mx    =0. ;
                                values.My    =0. ;
                                values.Mz    =0. ;
                             }
     else if(i==profile_cnt) {
                                values.thrust=profile[i-1].thrust ;
                                values.mass  =profile[i-1].mass ;
                                values.Cx    =profile[i-1].Cx ;
                                values.Cy    =profile[i-1].Cy ;
                                values.Cz    =profile[i-1].Cz ;
                                values.Mx    =profile[i-1].Mx ;
                                values.My    =profile[i-1].My ;
                                values.Mz    =profile[i-1].Mz ;
                             }
     else                    {
                                       k     =(t-profile[i-1].t)/(profile[i].t-profile[i-1].t) ;
                                values.thrust=profile[i-1].thrust+k*(profile[i].thrust-profile[i-1].thrust) ;
                                values.mass  =profile[i-1].mass  +k*(profile[i].mass  -profile[i-1].mass  ) ;
                                values.Cx    =profile[i-1].Cx    +k*(profile[i].Cx    -profile[i-1].Cx    ) ;
                                values.Cy    =profile[i-1].Cy    +k*(profile[i].Cy    -profile[i-1].Cy    ) ;
                                values.Cz    =profile[i-1].Cz    +k*(profile[i].Cz    -profile[i-1].Cz    ) ;
                                values.Mx    =profile[i-1].Mx    +k*(profile[i].Mx    -profile[i-1].Mx    ) ;
                                values.My    =profile[i-1].My    +k*(profile[i].My    -profile[i-1].My    ) ;
                                values.Mz    =profile[i-1].Mz    +k*(profile[i].Mz    -profile[i-1].Mz    ) ;
                             }

/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*      ����������� ���������� ������� ��������� ���������          */

     int  RSS_Unit_EngineSimple::vCalculateShow(double  t1, double t2)
{
  return(0) ;
}

/*********************************************************************/
/*								     */
/*              	  ���������� ����������         	     */

    int  RSS_Unit_EngineSimple::vSetEngineControl(RSS_Unit_Engine_Control *engines, int  engines_cnt)

{
   return(0) ;
}


/*********************************************************************/
/*								     */
/*              	  ���� ���������                	     */

    int  RSS_Unit_EngineSimple::vGetEngineThrust(RSS_Unit_Engine_Thrust *thrust)

{
   thrust->x= 0. ;
   thrust->y= 0. ;
   thrust->z=values.thrust ;

   return(1) ;
}


/*********************************************************************/
/*								     */
/*             	  ����� � ��������� ������ ���� ���������            */

    int  RSS_Unit_EngineSimple::vGetEngineMass(double *mass, RSS_Point *center)

{
                               *mass=values.mass ;

  if(this->use_Cxyz) {
                        center->mark= 1 ;
                        center->x   =values.Cx ;
                        center->y   =values.Cy ;
                        center->z   =values.Cz ;
                     }
  else               {
                        center->mark= 0 ;
                        center->x   = 0. ;
                        center->y   = 0. ;
                        center->z   = 0. ;
                     }

   return(this->use_mass) ;
}


/*********************************************************************/
/*								     */
/*             	  ������� ������� ���������                          */

    int  RSS_Unit_EngineSimple::vGetEngineMI(double *Ix, double *Iy, double *Iz)

{
     *Ix=values.Mx ;
     *Iy=values.My ;
     *Iz=values.Mz ;

   return(this->use_Mxyz) ;

}



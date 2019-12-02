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

#include "..\Matrix\Matrix.h"

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Unit\RSS_Unit.h"
#include "..\RSS_Kernel\RSS_Kernel.h"
#include "..\RSS_Model\RSS_Model.h"

#include "U_WarHeadSimple.h"

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

     static   RSS_Module_WarHeadSimple  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	����������������� ����                      */

 U_WARHEAD_SIMPLE_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 U_WARHEAD_SIMPLE_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/********************************************************************/
/**							           **/
/**            �������� ������ ������ ���������� ��������          **/
/**                         "������� ��"	                   **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*                            ������ ������                         */

  struct RSS_Module_WarHeadSimple_instr  RSS_Module_WarHeadSimple_InstrList[]={

 { "help",    "?",  "#HELP   - ������ ��������� ������", 
                     NULL,
                    &RSS_Module_WarHeadSimple::cHelp   },
 { "info",    "i",  "#INFO - ������ ���������� �� ����������",
                    " INFO <���> \n"
                    "   ������ �������� ���������� �� ������� � ������� ����\n"
                    " INFO/ <���> \n"
                    "   ������ ������ ���������� �� ������� � ��������� ����",
                    &RSS_Module_WarHeadSimple::cInfo },
 { "pars",    "p",  "#PARS - ������ ��������� ���������� � ���������� ������",
                    " PARS <���> \n"
                    "   ������ ��������� ���������� � ���������� ������\n",
                    &RSS_Module_WarHeadSimple::cPars },
 { "tripping","t",  "#TRIPPING - ������ ��������� ��������������� ���������",
                    " TRIPPING/� <���> <������ ������������>\n"
                    "   ���������� ������ ������������\n"
                    " TRIPPING/T <���> <����� ������������>\n"
                    "   ���������� ����� ������������\n",
                    &RSS_Module_WarHeadSimple::cTripping },
 { "grenade", "g",  "#GRENADE - ������ �������������� ��-����������",
                    " GRENADE <���> <������ ���������> <������ �������>\n"
                    "   ������ �������������� �� � ��-����������\n",
                    &RSS_Module_WarHeadSimple::cGrenade },
 { "stripe",  "s",  "#STRIPE - ������ �������������� ��������� �� � �������� ��������",
                    " STRIPE <���> <���-���������> <����� ���-�����������> <��� �������>\n"
                    "   ������ �������������� ��������� �� � �������� ��������\n",
                    &RSS_Module_WarHeadSimple::cStripe },
 { "pancakes","pc", "#PANCACES - ������ �������������� ��������� �� � �������� ����������������� ������� ('��������')",
                    " PANCACES <���> <���-���������> <����� ���-�����������> <��� �������> <����� ������> <������ �����>\n"
                    "   ������ �������������� ��������� �� � �������� ����������������� �������\n",
                    &RSS_Module_WarHeadSimple::cPancakes },
 {  NULL }
                                                            } ;


/********************************************************************/
/*								    */
/*		     ����� ����� ������             		    */

    struct RSS_Module_WarHeadSimple_instr *RSS_Module_WarHeadSimple::mInstrList=NULL ;


/********************************************************************/
/*								    */
/*		       ����������� ������			    */

     RSS_Module_WarHeadSimple::RSS_Module_WarHeadSimple(void)

{

/*---------------------------------------------------- ������������� */

	   keyword="EmiStand" ;
    identification="WarHeadSimple" ;
          category="Unit" ;
         lego_type="WarHead" ;

        mInstrList=RSS_Module_WarHeadSimple_InstrList ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        ���������� ������			    */

    RSS_Module_WarHeadSimple::~RSS_Module_WarHeadSimple(void)

{
}


/********************************************************************/
/*								    */
/*		      �������� �������                              */

  RSS_Object *RSS_Module_WarHeadSimple::vCreateObject(RSS_Model_data *data)

{
   RSS_Unit_WarHeadSimple *unit ;
                      int  i ;
 
/*---------------------------------------------- �������� ���������� */

       unit=new RSS_Unit_WarHeadSimple ;
    if(unit==NULL) {
               SEND_ERROR("������ WarHeadSimple: ������������ ������ ��� �������� ����������") ;
                        return(NULL) ;
                   }

             unit->Module=this ;

      strcpy(unit->Decl, "������ ����� (�������)") ;

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

     int  RSS_Module_WarHeadSimple::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- �������� ������ */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s -  ������� ������ �����", identification) ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		        ��������� �������       		    */

  int  RSS_Module_WarHeadSimple::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* ���� ������ ������ ������� */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "WARHEADSIMPLE"
#define  _SECTION_SHRT_NAME   "WARHEADSIMPLE"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Object WarHeadSimple:") ;
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
       if(status)  SEND_ERROR("������ WarHeadSimple: ����������� �������") ;
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

  int  RSS_Module_WarHeadSimple::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Unit_WarHeadSimple_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� INFO                    */
/*								    */
/*        INFO   <���>                                              */
/*        INFO/  <���>                                              */

  int  RSS_Module_WarHeadSimple::cInfo(char *cmd)

{
                     char  *name ;
   RSS_Unit_WarHeadSimple  *unit ;
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

       unit=(RSS_Unit_WarHeadSimple *)FindUnit(name) ;              /* ���� ��������� �� ����� */
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
/*		      ���������� ���������� PARS                    */
/*								    */
/*     PARS <��� ���������>                                         */

  int  RSS_Module_WarHeadSimple::cPars(char *cmd)

{
#define   _PARS_MAX  10

                    char *pars[_PARS_MAX] ;
                    char *name ;
  RSS_Unit_WarHeadSimple *unit ;
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
                                      "��������: PARS <���_���������a> ...") ;
                                     return(-1) ;
                         }

       unit=(RSS_Unit_WarHeadSimple *)FindUnit(name) ;              /* ���� ��������� �� ����� */
    if(unit==NULL)  return(-1) ;

/*--------------------------------------- ������� � ���������� ����� */

       status=DialogBoxIndirectParam( GetModuleHandle(NULL),
                                     (LPCDLGTEMPLATE)Resource("IDD_PARS", RT_DIALOG),
                                      GetActiveWindow(), 
                                      Unit_WarHeadSimple_Pars_dialog, 
                                     (LPARAM)unit                    ) ;
    if(status)  return(-1) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� TRIPPING                */
/*								    */
/*      TRIPPING/A <���> <������ ������������>                      */
/*      TRIPPING/T <���> <����� ������������>                       */

  int  RSS_Module_WarHeadSimple::cTripping(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

                    char  *pars[_PARS_MAX] ;
                    char  *name ;
                    char **xyz ;
                  double   coord[_COORD_MAX] ;
                     int   coord_cnt ;
  RSS_Unit_WarHeadSimple  *unit ;
                     int   a_flag ;            /* ���� ������������ �� ������ */
                     int   t_flag ;            /* ���� ������������ �� ������� */
                    char   *error ;
                    char   *end ;
                     int    i ;

/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
                          a_flag=0 ;
                          t_flag=0 ;

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
                if(strchr(cmd, 't')!=NULL ||
                   strchr(cmd, 'T')!=NULL   )  t_flag=1 ;

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

/*---------------------------------------- �������� ����� ���������� */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* ���� ��� �� ������... */
                           SEND_ERROR("�� ������ ��� ����������. \n"
                                      "��������: TRIPPING <���_���������a> ...") ;
                                         return(-1) ;
                         }

       unit=(RSS_Unit_WarHeadSimple *)FindUnit(name) ;              /* ���� ��������� �� ����� */
    if(unit==NULL)  return(-1) ;

/*--------------------------------------- �������� ���� ������������ */

    if(a_flag==0 &&
       t_flag==0   ) {
                        SEND_ERROR("�� ����� ��� ������������. \n"
                                   "���������� ������������ TRIPPING/A ��� TRIPPING/T") ;
                                         return(-1) ;
                     }

    if(a_flag==1 &&
       t_flag==1   ) {
                        SEND_ERROR("��������� ������� ������ ������ ���� ������������. \n"
                                   "���������� ������������ TRIPPING/A ��� TRIPPING/T") ;
                                         return(-1) ;
                     }
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
      if(coord_cnt==0)  error="�� ������ �������� ������������" ;

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

/*------------------------------------------------- ������� �������� */

   if(a_flag) {
                 unit->tripping_type    =_BY_ALTITUDE ;
                 unit->tripping_altitude= coord[0] ;
              }

   if(t_flag) {
                 unit->tripping_type    =_BY_TIME ;
                 unit->tripping_time    = coord[0] ;
              }
/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� GRENADE                 */
/*								    */
/*      GRENADE <���> <������ ���������> <������ �������>           */

  int  RSS_Module_WarHeadSimple::cGrenade(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

                    char  *pars[_PARS_MAX] ;
                    char  *name ;
                    char **xyz ;
                  double   coord[_COORD_MAX] ;
                     int   coord_cnt ;
  RSS_Unit_WarHeadSimple  *unit ;
                    char   *error ;
                    char   *end ;
                     int    i ;

/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
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

/*---------------------------------------- �������� ����� ���������� */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* ���� ��� �� ������... */
                           SEND_ERROR("�� ������ ��� ����������. \n"
                                      "��������: GRENADE <���_���������a> ...") ;
                                         return(-1) ;
                         }

       unit=(RSS_Unit_WarHeadSimple *)FindUnit(name) ;              /* ���� ��������� �� ����� */
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
      if(coord_cnt<2)  error="�� ������� ��������� ��" ;

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

/*------------------------------------------------- ������� �������� */

                 unit-> load_type  =_GRENADE_TYPE ;
                 unit->  hit_range = coord[0] ;
                 unit->blast_radius= coord[1] ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� STRIPE                  */
/*								    */
/*   STRIPE <���> <���-���������> ...                               */
/*                  ... <����� ���-�����������> <��� �������>       */

  int  RSS_Module_WarHeadSimple::cStripe(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

                    char  *pars[_PARS_MAX] ;
                    char  *name ;
                    char  *sub_name ;
                    char **xyz ;
                  double   coord[_COORD_MAX] ;
                     int   coord_cnt ;
  RSS_Unit_WarHeadSimple  *unit ;
              RSS_Object  *sub_object ;
                    char   text[1024] ;
                    char  *error ; 
                    char  *end ;
                     int   i ;

/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������ ���������� */        
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

                     name= pars[0] ;
                 sub_name= pars[1] ;
                      xyz=&pars[2] ;   

/*---------------------------------------- �������� ����� ���������� */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* ���� ��� �� ������... */
                           SEND_ERROR("�� ������ ��� ����������. \n"
                                      "��������: STRIPE <���_���������a> ...") ;
                                         return(-1) ;
                         }

       unit=(RSS_Unit_WarHeadSimple *)FindUnit(name) ;              /* ���� ��������� �� ����� */
    if(unit==NULL)  return(-1) ;

/*------------------------------------------ �������� ���-���������� */

    if(sub_name   ==NULL ||
       sub_name[0]==  0    ) {                                      /* ���� ��� ���-���������� �� ������... */
                           SEND_ERROR("�� ������ ��� ���-����������. \n"
                                      "��������: STRIPE <��� ���������a> <��� ���-����������> ...") ;
                                         return(-1) ;
                             }

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, sub_name)) {  sub_object=OBJECTS[i] ;
                                                              break ;         }

         if(i>=OBJECTS_CNT) {
                                sprintf(text, "������ '%s' �� ������ ", sub_name) ;
                             SEND_ERROR(text) ;
                                  return(NULL) ;
                            }

#undef   OBJECTS
#undef   OBJECTS_CNT

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
      if(coord_cnt<2)  error="�� ������� ��������� ��" ;

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

/*------------------------------------------------- ������� �������� */

                 unit->load_type  =_STRIPE_TYPE ;
         strncpy(unit-> sub_unit, sub_name, sizeof(unit->sub_unit)-1) ;
                 unit-> sub_object= sub_object ;
                 unit-> sub_count = (int)coord[0] ;
                 unit-> sub_step  =      coord[1] ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� PANCAKES                */
/*								    */
/*   PANCAKES <���> <���-���������> ...                             */
/*         ... <����� ���-�����������> <��� �������> ...            */
/*         ..  <����� ������> <������ �����>                        */

  int  RSS_Module_WarHeadSimple::cPancakes(char *cmd)

{
#define  _COORD_MAX   4
#define   _PARS_MAX  10

                    char  *pars[_PARS_MAX] ;
                    char  *name ;
                    char  *sub_name ;
                    char **xyz ;
                  double   coord[_COORD_MAX] ;
                     int   coord_cnt ;
  RSS_Unit_WarHeadSimple  *unit ;
              RSS_Object  *sub_object ;
                    char   text[1024] ;
                    char  *error ; 
                    char  *end ;
                     int   i ;

/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������ ���������� */        
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

                     name= pars[0] ;
                 sub_name= pars[1] ;
                      xyz=&pars[2] ;   

/*---------------------------------------- �������� ����� ���������� */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* ���� ��� �� ������... */
                           SEND_ERROR("�� ������ ��� ����������. \n"
                                      "��������: PANCAKES <���_���������a> ...") ;
                                         return(-1) ;
                         }

       unit=(RSS_Unit_WarHeadSimple *)FindUnit(name) ;              /* ���� ��������� �� ����� */
    if(unit==NULL)  return(-1) ;

/*------------------------------------------ �������� ���-���������� */

    if(sub_name   ==NULL ||
       sub_name[0]==  0    ) {                                      /* ���� ��� ���-���������� �� ������... */
                           SEND_ERROR("�� ������ ��� ���-����������. \n"
                                      "��������: PANCAKES <��� ���������a> <��� ���-����������> ...") ;
                                         return(-1) ;
                             }

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, sub_name)) {  sub_object=OBJECTS[i] ;
                                                              break ;         }

         if(i>=OBJECTS_CNT) {
                                sprintf(text, "������ '%s' �� ������ ", sub_name) ;
                             SEND_ERROR(text) ;
                                  return(NULL) ;
                            }

#undef   OBJECTS
#undef   OBJECTS_CNT

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
      if(coord_cnt<3)  error="�� ������� ��������� ��" ;

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

/*------------------------------------------------- ������� �������� */

                 unit->load_type  =_PANCAKES_TYPE ;
         strncpy(unit-> sub_unit, sub_name, sizeof(unit->sub_unit)-1) ;
                 unit-> sub_object= sub_object ;
                 unit-> sub_count = (int)coord[0] ;
                 unit-> sub_step  =      coord[1] ;
                 unit-> sub_series= (int)coord[2] ;
                 unit-> sub_range = (int)coord[3] ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*	   ����� ������� ���� WarHeadSimple �� �����                */

  RSS_Unit *RSS_Module_WarHeadSimple::FindUnit(char *name)

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

     if(strcmp(unit->Type, "WarHeadSimple")) {

           SEND_ERROR("��������� �� �������� ����������� ���� WarHeadSimple") ;
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
/**	  �������� ������ ���������� "������� ��"	           **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       ����������� ������			    */

     RSS_Unit_WarHeadSimple::RSS_Unit_WarHeadSimple(void)

{
   strcpy(Type, "WarHeadSimple") ;
          Module=&ProgramModule ;

   Parameters    =NULL ;
   Parameters_cnt=  0 ;

     tripping_type    =_BY_ALTITUDE ;
     tripping_altitude= 0. ;
     tripping_time    = 0. ;

         load_type    =_GRENADE_TYPE ;
          hit_range   = 0. ;
        blast_radius  = 5. ;

          sub_unit[0] = 0 ;
          sub_object  = NULL ;
          sub_count   = 0 ;
          sub_step    = 0. ;
}


/********************************************************************/
/*								    */
/*		        ���������� ������			    */

    RSS_Unit_WarHeadSimple::~RSS_Unit_WarHeadSimple(void)

{
      vFree() ;
}


/********************************************************************/
/*								    */
/*		       ������������ ��������                        */

  void   RSS_Unit_WarHeadSimple::vFree(void)

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

    class RSS_Object *RSS_Unit_WarHeadSimple::vCopy(char *name)

{
         RSS_Model_data  create_data ;
 RSS_Unit_WarHeadSimple *unit ;
   
/*------------------------------------- ����������� �������� ������� */

      memset(&create_data, 0, sizeof(create_data)) ;

       unit=(RSS_Unit_WarHeadSimple *)this->Module->vCreateObject(&create_data) ;
    if(unit==NULL)  return(NULL) ;

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

/*-------------------------------------------------------------------*/

   return(unit) ;
}


/********************************************************************/
/*								    */
/*                        ����������� ��������                      */

     int  RSS_Unit_WarHeadSimple::vSpecial(char *oper, void *data)
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

     int  RSS_Unit_WarHeadSimple::vCalculateStart(double  t)
{
        blast=        0 ;
   start_time=(time_t)t ;

  return(0) ;
}


/********************************************************************/
/*								    */
/*                   ������ ��������� ���������                     */

     int  RSS_Unit_WarHeadSimple::vCalculate(double t1, double t2, char *callback, int cb_size)
{
       char  text[1024] ;
       char  sub_name[128] ;
     double  k ;
     double  s, dx, dy, dz ;
     double  x_drop, y_drop, z_drop ;
   Matrix2d  Sum_Matrix ;
   Matrix2d  Oper_Matrix ;  
   Matrix2d  Vect_Matrix ;  
     double  step_e, angle_e, x_e, y_e ;
        int  n ;
        int  i ;

/*------------------------------------------------- ������� �������� */

   if(blast)  return(1) ;

/*----------------------------------------------- ���������� ������� */

          t1-=this->start_time ;
          t2-=this->start_time ;

/*----------------------------------------- ����������� ������������ */
/*- - - - - - - - - - - - - - - - - - - - - - ������������ �� ������ */
  if(this->tripping_type==_BY_ALTITUDE)
   if(t2>5.) 
    if(this->Owner->y_base<this->tripping_altitude) {

                                        blast=1 ;

                                     k=(this->tripping_altitude-y)/(this->Owner->y_base-y) ;
                   this->Owner->x_base=x+k*(this->Owner->x_base-x) ;
                   this->Owner->z_base=z+k*(this->Owner->z_base-z) ;
                   this->Owner->y_base=     this->tripping_altitude ;
                                                     }
/*- - - - - - - - - - - - - - - - - - - - -  ������������ �� ������� */
  if(this->tripping_type==_BY_TIME)
   if(this->tripping_time> t1 &&
      this->tripping_time<=t2   ) {
                                        blast=1 ;
                                  }
/*------------------------------------------- ��������� ������������ */

  if(blast) {
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - �� */
   if(load_type== _GRENADE_TYPE) {

     if(callback!=NULL) {
                           sprintf(text, "STOP %s;"
                                         "EXEC BLAST CR blast_%s %lf %lf %s;"
                                         "START blast_%s;",
                                   this->Owner->Name,
                                   this->Owner->Name, this->hit_range, this->blast_radius, this->Owner->Name,
                                   this->Owner->Name
                                  ) ;
                           strncat(callback, text, cb_size) ;
                        }
                                 }
/*- - - - - - - - - - - - - - - - - - - - -  ���������������� ������ */
   if(load_type==  _STRIPE_TYPE) {

     if(callback!=NULL) {
                           sprintf(text, "STOP %s;"
                                         "EXEC SCENE VISIBLE %s 0;",
                                          this->Owner->Name, this->Owner->Name) ;
                           strncat(callback, text, cb_size) ;

          if(this->sub_object!=NULL) {

                       s=sqrt((this->Owner->x_base-x)*(this->Owner->x_base-x)+
                              (this->Owner->y_base-y)*(this->Owner->y_base-y)+
                              (this->Owner->z_base-z)*(this->Owner->z_base-z) ) ;
                      dx=this->sub_step*(this->Owner->x_base-x)/s ;
                      dy=this->sub_step*(this->Owner->y_base-y)/s ;
                      dz=this->sub_step*(this->Owner->z_base-z)/s ;

            for(i=0 ; i<this->sub_count ; i++) {
                           sprintf(sub_name, "%s_sub_%d", this->Owner->Name, i+1) ;

                                    x_drop=this->Owner->x_base+dx*i ;
                                    y_drop=this->Owner->y_base+dy*i ;
                                    z_drop=this->Owner->z_base+dz*i ;

                           sprintf(text, "EXEC %s COPY %s %s %lf %lf %f;"
                                         "START %s;",
                                          this->sub_object->Module->identification, this->sub_unit, sub_name, x_drop, y_drop, z_drop,
                                                   sub_name
                                  ) ;
                           strncat(callback, text, cb_size) ;
                                               }
                                   }
                        }
                                 }
/*- - - - - - - - - - - - - - - - - - - - - - - ������ "����������" */
   if(load_type==_PANCAKES_TYPE) {

        Sum_Matrix.Load3d_azim(-this->Owner->a_azim) ;             /* ������� ������� �������������� */
       Oper_Matrix.Load3d_elev(-this->Owner->a_elev) ;
        Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;

                       s=sqrt((this->Owner->x_base-x)*(this->Owner->x_base-x)+
                              (this->Owner->y_base-y)*(this->Owner->y_base-y)+
                              (this->Owner->z_base-z)*(this->Owner->z_base-z) ) ;
                      dx=this->sub_step*(this->Owner->x_base-x)/s ;
                      dy=this->sub_step*(this->Owner->y_base-y)/s ;
                      dz=this->sub_step*(this->Owner->z_base-z)/s ;

     if(callback!=NULL) {
                           sprintf(text, "STOP %s;"
                                         "EXEC SCENE VISIBLE %s 0;",
                                          this->Owner->Name, this->Owner->Name) ;
                           strncat(callback, text, cb_size) ;

        if(this->sub_object!=NULL) {

          for(n=0 ; n<this->sub_series ; n++) {

            for(i=0 ; i<this->sub_count/this->sub_series ; i++) {

               step_e=2.*_PI/((double)(this->sub_count/this->sub_series)) ;
              angle_e=ProgramModule.gGaussianValue(i*step_e, step_e/4.) ;
                  x_e=this->sub_range*cos(angle_e) ;
                  y_e=this->sub_range*sin(angle_e) ;

                 Vect_Matrix.LoadZero(3, 1) ;
                 Vect_Matrix.SetCell (0, 0, x_e) ;
                 Vect_Matrix.SetCell (1, 0, y_e) ;
                 Vect_Matrix.SetCell (2, 0, 0) ;
                 Vect_Matrix.LoadMul (&Sum_Matrix, &Vect_Matrix) ;

                   x_drop=this->Owner->x_base+dx*n+Vect_Matrix.GetCell(0, 0) ;
                   y_drop=this->Owner->y_base+dy*n+Vect_Matrix.GetCell(1, 0) ;
                   z_drop=this->Owner->z_base+dz*n+Vect_Matrix.GetCell(2, 0) ;

                           sprintf(sub_name, "%s_sub_%d_%d", this->Owner->Name, n+1, i+1) ;

                           sprintf(text, "EXEC %s COPY %s %s %lf %lf %f;"
                                         "START %s;",
                                          this->sub_object->Module->identification, this->sub_unit, sub_name, x_drop, y_drop, z_drop,
                                                   sub_name
                                  ) ;
                           strncat(callback, text, cb_size) ;
                                                                }
                                              }
                                   }
                        }
                                 }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                       return(1) ;
                 }
/*------------------------------------------------- �������� "�����" */

        x=this->Owner->x_base ;
        y=this->Owner->y_base ;
        z=this->Owner->z_base ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*      ����������� ���������� ������� ��������� ���������          */

     int  RSS_Unit_WarHeadSimple::vCalculateShow(double  t1, double  t2)
{
  return(0) ;
}

/*********************************************************************/
/*								     */
/*              	  ���������� ��                  	     */

    int  RSS_Unit_WarHeadSimple::vSetWarHeadControl(char *regime)

{
   return(0) ;
}

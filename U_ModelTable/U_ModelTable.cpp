/********************************************************************/
/*                                                                  */
/*            ������ ���������� ����������� "��������� ������"      */
/*                                                                  */
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

#include "U_ModelTable.h"

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

     static   RSS_Module_ModelTable  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	����������������� ����                      */

 U_MODEL_TABLE_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 U_MODEL_TABLE_API RSS_Kernel *GetEntry(void)

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

  struct RSS_Module_ModelTable_instr  RSS_Module_ModelTable_InstrList[]={

 { "help",      "?",  "#HELP - ������ ��������� ������", 
                       NULL,
                      &RSS_Module_ModelTable::cHelp   },
 { "info",      "i",  "#INFO - ������ ���������� �� �������",
                      " INFO <���> \n"
                      "   ������ �������� ���������� �� ������� � ������� ����\n"
                      " INFO/ <���> \n"
                      "   ������ ������ ���������� �� ������� � ��������� ����",
                      &RSS_Module_ModelTable::cInfo },
 { "pars",      "p",  "#PARS - ������� ���������� ������ � ���������� ������", 
                       NULL,
                      &RSS_Module_ModelTable::cPars   },
 { "path",      "pt", "#PATH - ������� ����� ������� ������", 
                       NULL,
                      &RSS_Module_ModelTable::cPath   },
 { "format",    "f", "#FORMAT - ������� ������� ������ � �������", 
                      "  ��������� ������������ �������: [DN]P1#P2#...#Pk \n"
                      "  ��� D - ������������ ���������� �����������: . ��� , \n"
                      "      N - ����� ������ ������ ������: 1... \n"
                      "      # - ����������� ����� ������ ';', ',', '#' ��� '^' \n"
                      "     Pk - ����������� ���� ������: \n"
                      "           T - ��������� ����� \n"
                      "           X,Y,Z - ���������� \n"
                      "           V - �������� \n"
                      "           Vx, Vy, Vz - �������� �������� \n"
                      "           A, E, R - ���� ����������: ������/��������, ���� ����������/������, ���� \n"
                      "           ? - ���� ������������ \n"
                      "   ���� V, Vx, Vy, Vz, A, E, R �������� ���������������  \n"
                      "     � ��� �� ���������� ����������� �� ��������� \n"
                      " \n"
                      "  ��������: [,2]T;V;*;X;Y;Z",
                      &RSS_Module_ModelTable::cFormat  },
 {  NULL }
                                                            } ;

/********************************************************************/
/*								    */
/*		     ����� ����� ������             		    */

    struct RSS_Module_ModelTable_instr *RSS_Module_ModelTable::mInstrList=NULL ;


/********************************************************************/
/*								    */
/*		       ����������� ������			    */

     RSS_Module_ModelTable::RSS_Module_ModelTable(void)

{

/*---------------------------------------------------- ������������� */

           keyword="EmiStand" ;
    identification="ModelTable" ;
          category="Unit" ;
         lego_type="Model" ;

        mInstrList=RSS_Module_ModelTable_InstrList ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        ���������� ������			    */

    RSS_Module_ModelTable::~RSS_Module_ModelTable(void)

{
}


/********************************************************************/
/*								    */
/*		      �������� �������                              */

  RSS_Object *RSS_Module_ModelTable::vCreateObject(RSS_Model_data *data)

{
   RSS_Unit_ModelTable *unit ;
                   int  i ;
 
/*---------------------------------------------- �������� ���������� */

       unit=new RSS_Unit_ModelTable ;
    if(unit==NULL) {
               SEND_ERROR("������ ModelTable: ������������ ������ ��� �������� ����������") ;
                        return(NULL) ;
                   }

             unit->Module=this ;

      strcpy(unit->Decl, "������ ���������") ;

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

     int  RSS_Module_ModelTable::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- �������� ������ */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s -  ��������� ������", identification) ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		        ��������� �������       		    */

  int  RSS_Module_ModelTable::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* ���� ������ ������ ������� */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "MODELTABLE"
#define  _SECTION_SHRT_NAME   "MODELTABLE"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Object ModelTable:") ;
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
       if(status)  SEND_ERROR("������ ModelTable: ����������� �������") ;
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

  int  RSS_Module_ModelTable::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Unit_ModelTable_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� INFO                    */
/*								    */
/*        INFO   <���>                                              */
/*        INFO/  <���>                                              */

  int  RSS_Module_ModelTable::cInfo(char *cmd)

{
                     char  *name ;
      RSS_Unit_ModelTable  *unit ;
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

       unit=(RSS_Unit_ModelTable *)FindUnit(name) ;                 /* ���� ��������� �� ����� */
    if(unit==NULL)  return(-1) ;

/*-------------------------------------------- ������������ �������� */

      sprintf(text, "%s\r\n%s\r\n"
                    "Owner     %s\r\n" 
                    "Path      %s\r\n" 
                    "Format    %s\r\n" 
                    "\r\n",
                        unit->Name,  unit->Type, 
                        unit->Owner->Name, 
                        unit->path, unit->format 
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

  int  RSS_Module_ModelTable::cPars(char *cmd)

{
#define   _PARS_MAX  10

                  char *pars[_PARS_MAX] ;
                  char *name ;
   RSS_Unit_ModelTable  *unit ;
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

/*------------------------------------------- �������� ����� ������� */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* ���� ��� �� ������... */
                           SEND_ERROR("�� ������ ��� ����������. \n"
                                      "��������: PARS <���_����������> ...") ;
                                     return(-1) ;
                         }

       unit=(RSS_Unit_ModelTable *)FindUnit(name) ;                 /* ���� ��������� �� ����� */
    if(unit==NULL)  return(-1) ;

/*--------------------------------------- ������� � ���������� ����� */

        status=DialogBoxIndirectParam( GetModuleHandle(NULL),
                                      (LPCDLGTEMPLATE)Resource("IDD_PARS", RT_DIALOG),
                                       GetActiveWindow(), 
                                       Unit_ModelTable_Pars_dialog, 
                                      (LPARAM)unit                   ) ;
     if(status)  return(-1) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� PATH                    */
/*								    */
/*       PATH <���> <���� � �����>                                  */

  int  RSS_Module_ModelTable::cPath(char *cmd)

{
#define   _PARS_MAX  10

                  char *pars[_PARS_MAX] ;
                  char *name ;
                  char *path ;
   RSS_Unit_ModelTable *unit ;
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

                     name=pars[0] ;
                     path=pars[1] ;   

/*---------------------------------------- �������� ����� ���������a */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* ���� ��� �� ������... */
                           SEND_ERROR("�� ������ ��� ���������. \n"
                                      "��������: PATH <���_���������> ...") ;
                                     return(-1) ;
                         }

       unit=(RSS_Unit_ModelTable *)FindUnit(name) ;                 /* ���� ��������� �� ����� */
    if(unit==NULL)  return(-1) ;

/*------------------------------------------------ ������ ���������� */

    if(path==NULL) {
                       SEND_ERROR("���� ����� ������ �� �����") ;
                                       return(-1) ;
                   }

    if(access(path, 0x00)) {
                       SEND_ERROR("���� ����� ������ �� �������� ��� �� ����������") ;
                                       return(-1) ;
                           }
/*----------------------------------------------------- ������� ���� */

          strncpy(unit->path, path, sizeof(unit->path)-1) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� FORMAT                  */
/*								    */
/*       FORMAT <���> <������������ �������>                        */

  int  RSS_Module_ModelTable::cFormat(char *cmd)

{
#define   _PARS_MAX  10

                  char *pars[_PARS_MAX] ;
                  char *name ;
                  char *format ;
   RSS_Unit_ModelTable *unit  ;
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

                     name=pars[0] ;
                   format=pars[1] ;   

/*---------------------------------------- �������� ����� ���������a */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* ���� ��� �� ������... */
                           SEND_ERROR("�� ������ ��� ���������. \n"
                                      "��������: FORMAT <���_���������> ...") ;
                                     return(-1) ;
                         }

       unit=(RSS_Unit_ModelTable *)FindUnit(name) ;                 /* ���� ��������� �� ����� */
    if(unit==NULL)  return(-1) ;

/*------------------------------------------------ ������ ���������� */

    if(format==NULL) {
                       SEND_ERROR("������ ������ �� �����") ;
                                       return(-1) ;
                     }

       status=unit->iFormatDecode(format) ;
    if(status) {
                  SEND_ERROR("������������ ������ ������") ;
                               return(-1) ;
               }
/*----------------------------------------------------- ������� ���� */

       strncpy(unit->format, format, sizeof(unit->format)-1) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*	     ����� ������� ���� ModelTable �� �����                */

  RSS_Unit *RSS_Module_ModelTable::FindUnit(char *name)

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

     if(strcmp(unit->Type, "ModelTable")) {

           SEND_ERROR("��������� �� �������� ����������� ���� ModelTable") ;
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
/**	  �������� ������ ���������� "��������� ������"	           **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       ����������� ������			    */

     RSS_Unit_ModelTable::RSS_Unit_ModelTable(void)

{
   strcpy(Type, "ModelTable") ;
          Module=&ProgramModule ;

   Parameters    =NULL ;
   Parameters_cnt=  0 ;

       memset(path,   0, sizeof(path)) ;
       memset(format, 0, sizeof(format)) ;

              t_0=  0. ; 
             file=NULL ; 
}


/********************************************************************/
/*								    */
/*		        ���������� ������			    */

    RSS_Unit_ModelTable::~RSS_Unit_ModelTable(void)

{
      vFree() ;
}


/********************************************************************/
/*								    */
/*		       ������������ ��������                        */

  void   RSS_Unit_ModelTable::vFree(void)

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

    class RSS_Object *RSS_Unit_ModelTable::vCopy(char *name)

{
        RSS_Model_data  create_data ;
   RSS_Unit_ModelTable *unit ;
   
/*------------------------------------- ����������� �������� ������� */

      memset(&create_data, 0, sizeof(create_data)) ;

       unit=(RSS_Unit_ModelTable *)this->Module->vCreateObject(&create_data) ;
    if(unit==NULL)  return(NULL) ;

/*------------------------------------- ����������� �������� ������� */

             strcpy(unit->path,   this->path) ;
             strcpy(unit->format, this->format) ;

/*-------------------------------------------------------------------*/

   return(unit) ;
}


/********************************************************************/
/*								    */
/*                        ����������� ��������                      */

     int  RSS_Unit_ModelTable::vSpecial(char *oper, void *data)
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

     int  RSS_Unit_ModelTable::vCalculateStart(double  t)
{
  double  pars[20] ;
     int  status ;
    char  text[1024] ;
     int  i ;

/*------------------------------------ ������������� ������� ������ */

      this->t_0=t ;

   if(this->file!=NULL) {
                            fclose(this->file) ;
                                   this->file=NULL ;
                        }
/*-------------------------------------------------- ������� ������� */

      status=iFormatDecode(this->format) ;
   if(status) {
                    sprintf(text, "����������� ������ ������ ��� ������ %s ������� %s",
                                       this->Name, this->owner) ;
                 SEND_ERROR(text) ;
                      return(-1) ;
              }
/*------------------------------------------- ������ ��������� ����� */

      file=fopen(this->path, "rt") ;
   if(file==NULL) {
                      sprintf(text, "������ �������� ����� ������ ��� ������ %s ������� %s (errno=%d) : %s",
                                       this->Name, this->owner, errno, this->path) ;
                     SEND_ERROR(text) ;
                          return(-1) ;
                  }
/*------------------------------------------- ������ ��������� ����� */

   for(i=1 ; i<this->start_row ; i++) {

              fgets(text, sizeof(text)-1, file) ;
                                      }
/*---------------------------------- ����������� ��������� ��������� */

             memset(text, 0, sizeof(text)) ;
              fgets(text, sizeof(text)-1, file) ;

      status=iParsDissect(text, pars, sizeof(pars)/sizeof(*pars)) ;
   if(status) {
                    sprintf(text, "������� ����� ������ � ������ ��� ������ %s ������� %s",
                                       this->Name, this->owner) ;
                 SEND_ERROR(text) ;
                      return(-1) ;
              }

           memset(        pars_1, 0,            sizeof(pars_1)) ;
           memcpy((void *)pars_2, (void *)pars, sizeof(pars_2)) ;

                                    t_start=pars[this->t_idx] ;

       if(this->azim_idx!=-1)  this->a_azim=pars[this->azim_idx] ;
       if(this->elev_idx!=-1)  this->a_azim=pars[this->elev_idx] ;
       if(this->roll_idx!=-1)  this->a_azim=pars[this->roll_idx] ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*                   ������ ��������� ���������                     */

     int  RSS_Unit_ModelTable::vCalculate(double t1, double t2, char *callback, int cb_size)
{
  RSS_Object *parent ;
         int  status ;
      double  dt ;
      double  v ;
      double  l ;
      double  lx ;
      double  ly ;
      double  lz ;
        char  text[1024] ;

/*------------------------------------------------ �o�������� ������ */

           t1=t1-this->t_0+t_start ;
           t2=t2-this->t_0+t_start ;

    if(t1<0.)  return(0) ;

        parent=this->Owner ;

/*----------------------------------------------- ������������ ����� */

  if(pars_2[t_idx]<t2) {                                            /* ���� �� ������� �� ����������� "�����" ������... */

    while(1) {

             memcpy((void *)pars_1, (void *)pars_2, sizeof(pars_1)) ;

             memset(text, 0, sizeof(text)) ;
              fgets(text, sizeof(text)-1, file) ;

         status=iParsDissect(text, pars_2, sizeof(pars_2)/sizeof(*pars_2)) ;
      if(status) {
                       sprintf(text, "������� ����� ������ � ������ ��� ������ %s ������� %s",
                                          this->Name, this->owner) ;
                    SEND_ERROR(text) ;
                         return(-1) ;
                 }

      if(pars_2[t_idx]>=t2)  break ;

             }
                       } 
/*---------------------------------------------------- ������ ������ */

                dt=(t2-pars_1[t_idx])/(pars_2[t_idx]-pars_1[t_idx]) ;

             parent->x_base=pars_1[x_idx]+(pars_2[x_idx]-pars_1[x_idx])*dt ;
             parent->y_base=pars_1[y_idx]+(pars_2[y_idx]-pars_1[y_idx])*dt ;
             parent->z_base=pars_1[z_idx]+(pars_2[z_idx]-pars_1[z_idx])*dt ;

                         lx=pars_2[x_idx]-pars_1[x_idx] ;
                         ly=pars_2[y_idx]-pars_1[y_idx] ;
                         lz=pars_2[z_idx]-pars_1[z_idx] ;

                         l =sqrt(lx*lx+ly*ly+lz*lz) ;
                      
      if(   v_idx>=0) {
                         v=pars_1[v_idx]+(pars_2[v_idx]-pars_1[v_idx])*dt ;
                      }
      else            { 
                         v= l /(pars_2[t_idx]-pars_1[t_idx]) ;
                      }

      if(  vx_idx>=0) {
                         parent->x_velocity=pars_1[vx_idx]+(pars_2[vx_idx]-pars_1[vx_idx])*dt ;
                      }
      else            { 
                         parent->x_velocity=v*lx/l ;
                      }

      if(  vy_idx>=0) {
                         parent->y_velocity=pars_1[vy_idx]+(pars_2[vy_idx]-pars_1[vy_idx])*dt ;
                      }
      else            { 
                         parent->y_velocity=v*ly/l ;
                      }

      if(  vz_idx>=0) {
                         parent->z_velocity=pars_1[vz_idx]+(pars_2[vz_idx]-pars_1[vz_idx])*dt ;
                      }
      else            { 
                         parent->z_velocity=v*lz/l ;
                      }

      if(azim_idx>=0) {
                         parent->a_azim=pars_1[azim_idx]+(pars_2[azim_idx]-pars_1[azim_idx])*dt ;
                      }
      else            { 
                         parent->a_azim=atan2(parent->x_velocity, parent->z_velocity)*_RAD_TO_GRD ; 
                      }

      if(elev_idx>=0) {
                         parent->a_elev=pars_1[elev_idx]+(pars_2[elev_idx]-pars_1[elev_idx])*dt ;
                      }
      else            { 
                         parent->a_elev=atan2(parent->y_velocity, sqrt(parent->x_velocity*parent->x_velocity+
                                                                       parent->z_velocity*parent->z_velocity ))*_RAD_TO_GRD ; 
                      }

      if(roll_idx>=0) {
                         parent->a_roll=pars_1[roll_idx]+(pars_2[roll_idx]-pars_1[roll_idx])*dt ;
                      }
      else            { 
                         parent->a_roll=0 ; 
                      }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*      ����������� ���������� ������� ��������� ���������          */

     int  RSS_Unit_ModelTable::vCalculateShow(double  t1, double  t2)
{
  return(0) ;
}


/*********************************************************************/
/*								     */
/*              ���������� ����������������� �������������     	     */

    int  RSS_Unit_ModelTable::vSetAeroControl(RSS_Unit_Aero_Control *aeros, int  aeros_cnt)

{
   return(0) ;
}


/*********************************************************************/
/*								     */
/*              	  ���� ���������                	     */

    int  RSS_Unit_ModelTable::vSetEngineThrust(RSS_Unit_Engine_Thrust *thrust, int  thrust_cnt)

{
   return(0) ;
}


/*********************************************************************/
/*								     */
/*             	  ����� � ��������� ������ ���� ���������            */

    int  RSS_Unit_ModelTable::vSetEngineMass(double  mass, RSS_Point *center)

{
   return(0) ;
}


/*********************************************************************/
/*								     */
/*             	  ������� ������� ���������                          */

    int  RSS_Unit_ModelTable::vSetEngineMI(double  Ix, double  Iy, double  Iz)

{
   return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                    ������ ������� ������                          */
/*                                                                   */
/*  ��������� ������������ �������: [DN]P1#P2#...#Pk                 */
/*                                                                   */
/* "  ��� D - ������������ ���������� �����������: '.' ��� ','       */
/*      N - ����� ������ ������ ������: 1...                         */
/*      # - ����������� ����� ������: ';', ',', '#' ��� '^'          */
/*     Pk - ����������� ���� ������:                                 */
/*           T - ��������� �����                                     */
/*           X,Y,Z - ����������                                      */
/*           V - ��������                                            */
/*           Vx, Vy, Vz - �������� ��������                          */
/*           A, E, R - ���� ����������: ������ ���� ����������, ���� */
/*           ? - ���� ������������                                   */
/*   ���� V, Vx, Vy, Vz, A, E, R �������� ���������������            */
/*                                                                   */
/*   ��������: [,2]T;V;?;X;Y;Z",                                     */


    int  RSS_Unit_ModelTable::iFormatDecode(char *format)

{
   char  fmt[128] ;
   char *end ;
    int  idx ;
    int  i  ;

/*---------------------------------------------------- ������������� */

              t_idx=-1 ;
              x_idx=-1 ;
              y_idx=-1 ;
              z_idx=-1 ;
              v_idx=-1 ;
             vx_idx=-1 ;
             vy_idx=-1 ;
             vz_idx=-1 ;
           azim_idx=-1 ;
           elev_idx=-1 ;
           roll_idx=-1 ;

       memset(fmt, 0, sizeof(fmt)) ;
      strncpy(fmt, format, sizeof(fmt)-1) ;

/*-------------------------------------------------- ������ �������� */

    if(fmt[0]!='[')  return(-1) ;

    if(fmt[1]!='.' &&
       fmt[1]!=','   )  return(-1) ;

       this->dgt_sep=format[1] ;

       this->start_row=strtoul(fmt+2, &end, 10) ;
    if(this->start_row==0)  return(-1) ;

    if(*end!=']')  return(-1) ;

/*------------------------------------------------ ������ ���������� */

#define   IS_SEPARATOR   (fmt[i+1]==';' || fmt[i+1]==',' || fmt[i+1]=='#' || fmt[i+1]=='^')

  for(idx=0, i=end-fmt+1 ; fmt[i] ; i++) {

    if( fmt[i]=='?' && (IS_SEPARATOR || fmt[i+1]==0)) {
                   idx++ ;
                     i++ ;
                                                      }
    else
    if( fmt[i]=='T' && (IS_SEPARATOR || fmt[i+1]==0)) {
             t_idx=idx  ;
                   idx++ ;
                     i++ ;
                                                      }
    else
    if( fmt[i]=='X' && (IS_SEPARATOR || fmt[i+1]==0)) {
             x_idx=idx  ;
                   idx++ ;
                     i++ ;
                                                      }
    else
    if( fmt[i]=='Y' && (IS_SEPARATOR || fmt[i+1]==0)) {
             y_idx=idx  ;
                   idx++ ;
                     i++ ;
                                                      }
    else
    if( fmt[i]=='Z' && (IS_SEPARATOR || fmt[i+1]==0)) {
             z_idx=idx  ;
                   idx++ ;
                     i++ ;
                                                      }
    else
    if( fmt[i]=='V' && (IS_SEPARATOR || fmt[i+1]==0)) {
             v_idx=idx  ;
                   idx++ ;
                     i++ ;
                                                      }
    else
    if( fmt[i]=='A' && (IS_SEPARATOR || fmt[i+1]==0)) {
             azim_idx=idx  ;
                      idx++ ;
                        i++ ;
                                                      }
    else
    if( fmt[i]=='E' && (IS_SEPARATOR || fmt[i+1]==0)) {
             elev_idx=idx  ;
                      idx++ ;
                        i++ ;
                                                      }
    else
    if( fmt[i]=='R' && (IS_SEPARATOR || fmt[i+1]==0)) {
             roll_idx=idx  ;
                      idx++ ;
                        i++ ;
                                                      }
    else
    if( fmt[i]=='V' && fmt[i+1]=='X' && (IS_SEPARATOR || fmt[i+1]==0)) {
               vx_idx=idx  ;
                      idx++ ;
                        i+=2 ;
                                                                       }
    else
    if( fmt[i]=='V' && fmt[i+1]=='Y' && (IS_SEPARATOR || fmt[i+1]==0)) {
               vy_idx=idx  ;
                      idx++ ;
                        i+=2 ;
                                                                       }
    else
    if( fmt[i]=='V' && fmt[i+1]=='Z' && (IS_SEPARATOR || fmt[i+1]==0)) {
               vz_idx=idx  ;
                      idx++ ;
                        i+=2 ;
                                                                       }
    else                                                               {
                                   return(-1) ;
                                                                       }

                                         }

#undef  IS_SEPARATOR

/*--------------------------------- �������� ������������ ���������� */

    if(t_idx==-1 || 
       x_idx==-1 ||
       y_idx==-1 ||
       z_idx==-1   ) {
                        return(-1) ;
                     }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/*********************************************************************/
/*								     */
/*                     ������ ������ ������                          */

    int  RSS_Unit_ModelTable::iParsDissect(char *text, double *pars, int  pars_max)

{
   char *end ;
    int  i  ;

/*---------------------------------------------------- ������������� */

/*------------------------------------------ ������ ���������� ����� */

   if(this->dgt_sep!='.') {

      for(i=0 ; text[i] ; i++)
        if(text[i]==this->dgt_sep)  text[i]='.' ;

                          }
/*---------------------------------------------------- ������ ������ */

   for(i=0 ; i<pars_max ; i++) {

        pars[i]=strtod(text, &end) ;

      if(*end==0)  break ;

         text=end+1 ;

                               }
/*-------------------------------------------------------------------*/

   return(0) ;
}

/********************************************************************/
/*								    */
/*		������ ���������� �������� "���-����"  		    */
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
#include "..\F_Hit\F_Hit.h"
#include "..\Ud_tools\UserDlg.h"

#include "O_Rocket_Lego.h"
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

     static   RSS_Module_RocketLego  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	����������������� ����                      */

 O_ROCKET_LEGO_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 O_ROCKET_LEGO_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/********************************************************************/
/**							           **/
/**	  �������� ������ ������ ���������� �������� "�����"	   **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*                            ������ ������                         */

  struct RSS_Module_RocketLego_instr  RSS_Module_RocketLego_InstrList[]={

 { "help",     "?", "#HELP   - ������ ��������� ������", 
                     NULL,
                    &RSS_Module_RocketLego::cHelp   },
 { "create",  "cr", "#CREATE - ������� ������",
                    " CREATE <���> [<������> [<������ ����������>]]\n"
                    "   ������� ����������� ������ �� ����������������� ������",
                    &RSS_Module_RocketLego::cCreate },
 { "info",     "i", "#INFO - ������ ���������� �� �������",
                    " INFO <���> \n"
                    "   ������ �������� ��������� �� ������� � ������� ����\n"
                    " INFO/ <���> \n"
                    "   ������ ������ ���������� �� ������� � ��������� ����",
                    &RSS_Module_RocketLego::cInfo },
 { "copy",    "cp", "#COPY - ���������� ������",
                    " COPY <��� �������> <��� ������ �������>\n"
                    "   ���������� ������",
                    &RSS_Module_RocketLego::cCopy },
 { "owner",    "o", "#OWNER - ��������� �������� ������",
                    " OWNER <���> <��������>\n"
                    "   ��������� ������ - �������� ������",
                    &RSS_Module_RocketLego::cOwner },
 { "lego",     "l", "#LEGO - ��������� ��������� � ������� �������",
                    " LEGO <���>\n"
                    "   ����������� ������ �����������, ��������� ��������� � ���������� ������\n"
                    " LEGO <��� �������> <��� ����������> <��� ����������>\n"
                    "   �������� ��������� � ������ �������",
                    &RSS_Module_RocketLego::cLego },
 { "trace",    "t", "#TRACE - ����������� ���������� �������",
                    " TRACE <���> [<������������>]\n"
                    "   ����������� ���������� ������� � �������� �������\n"
                    " TRACE/C <���> <R>:<G>:<B> [...]\n"
                    "   ������� ������ ������\n"
                    " TRACE/W <���> <������� �����>\n"
                    "   ������� ������� ����� ������\n",
                    &RSS_Module_RocketLego::cTrace },
 { "spawn",   "sp", "#SPAWN - �������� ���� � �������������� ������� �������",
                    " SPAWN <���> <����� ������> <������������� ������>\n"
                    "   �������� ���� � �������������� ������� �������\n",
                    &RSS_Module_RocketLego::cSpawn },
 { "stat",    "st", "#STAT - ����������� ���������� �� ����������� ��������� �����",
                    " STAT/D <���>\n"
                    "   ���������� ����� �������/������������\n",
                    &RSS_Module_RocketLego::cStat },
 {  NULL }
                                                            } ;

/********************************************************************/
/*								    */
/*		     ����� ����� ������             		    */

    struct RSS_Module_RocketLego_instr *RSS_Module_RocketLego::mInstrList=NULL ;


/********************************************************************/
/*								    */
/*		       ����������� ������			    */

     RSS_Module_RocketLego::RSS_Module_RocketLego(void)

{
  static  WNDCLASS  View_wnd ;
              char  text[1024] ;

/*---------------------------------------------------- ������������� */

	   keyword="EmiStand" ;
    identification="Rocket-Lego" ;
          category="Object" ;

        mInstrList=RSS_Module_RocketLego_InstrList ;

/*--------------------------- ����������� ������ ���� UD_Show_view2D */

  if(View_wnd.hInstance==NULL) {

	View_wnd.lpszClassName="O_RocketLego_view_class" ;
	View_wnd.hInstance    = GetModuleHandle(NULL) ;
	View_wnd.lpfnWndProc  = UD_diagram_2D_prc ;
	View_wnd.hCursor      = LoadCursor(NULL, IDC_ARROW) ;
	View_wnd.hIcon        =  NULL ;
	View_wnd.lpszMenuName =  NULL ;
	View_wnd.hbrBackground=  NULL ;
	View_wnd.style        =    0 ;
	View_wnd.hIcon        =  NULL ;

    if(!RegisterClass(&View_wnd)) {
              sprintf(text, "O_RocketLego_view_class register error %d", GetLastError()) ;
           SEND_ERROR(text) ;
                    return ;
                                  }

                               }
/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        ���������� ������			    */

    RSS_Module_RocketLego::~RSS_Module_RocketLego(void)

{
}


/********************************************************************/
/*								    */
/*		      �������� �������                              */

  RSS_Object *RSS_Module_RocketLego::vCreateObject(RSS_Model_data *data)

{
  RSS_Object_RocketLego *object ;
                   char  models_list[4096] ;
                   char *end ;
                    int  i ;
                    int  j ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

#define       PAR             object->Parameters
#define       PAR_CNT         object->Parameters_cnt
 
/*--------------------------------------------------- �������� ����� */

//    if(data->name[0]==0) {                                           /* ���� ��� �� ������ */
//              SEND_ERROR("������ ROCKET LEGO: �� ������ ��� �������") ;
//                                return(NULL) ;
//                         }

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, data->name)) {
              SEND_ERROR("������ ROCKET LEGO: ������ � ����� ������ ��� ����������") ;
                                return(NULL) ;
                                                    }
/*-------------------------------------- ���������� �������� ������� */
/*- - - - - - - - - - - - ���� ������ ������ ��������� � ����������� */
   if(data->path[0]==0) {

    if(data->model[0]==0) {                                         /* ���� ������ �� ������ */
              SEND_ERROR("������ ROCKET LEGO: �� ������ ������ �������") ;
                                return(NULL) ;
                          }

        RSS_Model_list(data->lib_path, models_list,                 /* ������������ ������ ������� */
                                sizeof(models_list)-1, "BODY" );

        for(end=models_list ; *end ; ) {                            /* ���� ������ �� ������ */
                        if(!stricmp(data->model, end))  break ;
                                         end+=strlen(end)+1 ;
                                         end+=strlen(end)+1 ;
                                       }

           if(*end==0) {
              SEND_ERROR("������ ROCKET LEGO: ����������� ������ ����") ;
                                return(NULL) ;
                       }

                                    end+=strlen(end)+1 ;            /* ��������� ��� ����� */

                      sprintf(data->path, "%s\\%s", data->lib_path, end) ;
           RSS_Model_ReadPars(data) ;                               /* ��������� ��������� ������ */
                        }
/*- - - - - - - - - - - - - - - - -  ���� ������ ������ ������ ����� */
   else                 {
                              RSS_Model_ReadPars(data) ;            /* ��������� ��������� ������ */
                        }
/*--------------------------------------- �������� ������ ���������� */

     for(i=0 ; i<5 ; i++)
       if((data->pars[i].text [0]==0 &&
           data->pars[i].value[0]!=0   ) ||
          (data->pars[i].text [0]!=0 &&
           data->pars[i].value[0]==0   )   ) {

              SEND_ERROR("������ ROCKET LEGO: �������������� ����� ���������� ������") ;
                                return(NULL) ;
                                             }
/*------------------------------------------------- �������� ������� */

       object=new RSS_Object_RocketLego ;
    if(object==NULL) {
              SEND_ERROR("������ ROCKET LEGO: ������������ ������ ��� �������� �������") ;
                        return(NULL) ;
                     }

       strcpy(object->model_path, data->path) ;

/*------------------------------------- ���������� ������ ���������� */
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������� ��������� */
     for(i=0 ; i<5 ; i++)
       if(data->pars[i].text[0]!=0) {

           PAR=(struct RSS_Parameter *)
                 realloc(PAR, (PAR_CNT+1)*sizeof(*PAR)) ;
        if(PAR==NULL) {
                         SEND_ERROR("������ ROCKET LEGO: ������������ ������") ;
                                            return(NULL) ;
                      }

             memset(&PAR[PAR_CNT], 0, sizeof(PAR[PAR_CNT])) ;
            sprintf( PAR[PAR_CNT].name, "PAR%d", i+1) ;
                     PAR[PAR_CNT].value=strtod(data->pars[i].value, &end) ;
                     PAR[PAR_CNT].ptr  = NULL ;
                         PAR_CNT++ ;
                                    }
/*- - - - - - - - - - - - - - - - - - - - ����������� ������ ������� */
           PAR=(struct RSS_Parameter *)
                 realloc(PAR, (PAR_CNT+1)*sizeof(*PAR)) ;
        if(PAR==NULL) {
                         SEND_ERROR("������ ROCKET LEGO: ������������ ������") ;
                                            return(NULL) ;
                      }

             memset(&PAR[PAR_CNT], 0, sizeof(PAR[PAR_CNT])) ;

/*---------------------------------- �������� ������ ������� ������� */

      object->Features_cnt=this->feature_modules_cnt ;
      object->Features    =(RSS_Feature **)
                             calloc(this->feature_modules_cnt, 
                                     sizeof(object->Features[0])) ;

   for(i=0 ; i<this->feature_modules_cnt ; i++)
      object->Features[i]=this->feature_modules[i]->vCreateFeature(object, NULL) ;

/*-------------------------------------- ���������� �������� ������� */

           RSS_Model_ReadSect(data) ;                               /* ��������� ������ �������� ������ */

   for(i=0 ; data->sections[i].title[0] ; i++) {

     for(j=0 ; j<object->Features_cnt ; j++) {

          object->Features[j]->vBodyPars(NULL, PAR) ;
          object->Features[j]->vReadSave(data->sections[i].title, 
                                         data->sections[i].decl, "RocketLego.Body") ;
                                             }

                                         data->sections[i].title[0]= 0 ;
                                        *data->sections[i].decl    ="" ;
                                               }
/*---------------------------------- �������� ������� � ����� ������ */

  if(data->name[0]!=0) {                                            /* ���� ��� ������... */

       strcpy(object->Name, data->name) ;
              object->Module=this ;

       OBJECTS=(RSS_Object **)
                 realloc(OBJECTS, (OBJECTS_CNT+1)*sizeof(*OBJECTS)) ;
    if(OBJECTS==NULL) {
              SEND_ERROR("������ ROCKET LEGO: ������������ ������") ;
                                return(NULL) ;
                      }

              OBJECTS[OBJECTS_CNT]=object ;
                      OBJECTS_CNT++ ;

        SendMessage(this->kernel_wnd, WM_USER,
                          (WPARAM)_USER_DEFAULT_OBJECT, (LPARAM)data->name) ;
                       }
/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

#undef   PAR
#undef   PAR_CNT

  return(object) ;
}


/********************************************************************/
/*								    */
/*		        �������� ��������       		    */

     int  RSS_Module_RocketLego::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- �������� ������ */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s -  ���, 3-� ������������ ������������ ������", identification) ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		        ��������� �������       		    */

  int  RSS_Module_RocketLego::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* ���� ������ ������ ������� */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "ROCKET-LEGO"
#define  _SECTION_SHRT_NAME   "ROCKET-LEGO"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Object RocketLego:") ;
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
       if(status)  SEND_ERROR("������ ROCKET LEGO: ����������� �������") ;
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

    void  RSS_Module_RocketLego::vReadSave(std::string *data)

{
                  char *buff ;
                   int  buff_size ;
        RSS_Model_data  create_data ;
 RSS_Object_RocketLego *object ;
                  char  name[128] ;
                  char *entry ;
                  char *end ;
                   int  i ;

/*----------------------------------------------- �������� ��������� */

   if(memicmp(data->c_str(), "#BEGIN MODULE ROCKET LEGO\n", 
                      strlen("#BEGIN MODULE ROCKET LEGO\n")) &&
      memicmp(data->c_str(), "#BEGIN OBJECT ROCKET LEGO\n", 
                      strlen("#BEGIN OBJECT ROCKET LEGO\n"))   )  return ;

/*------------------------------------------------ ���������� ������ */

              buff_size=(int)data->size()+16 ;
              buff     =(char *)calloc(1, buff_size) ;

       strcpy(buff, data->c_str()) ;

/*------------------------------------------------- �������� ������� */

   if(!memicmp(buff, "#BEGIN OBJECT ROCKET LEGO\n", 
              strlen("#BEGIN OBJECT ROCKET LEGO\n"))) {                 /* IF.1 */
/*- - - - - - - - - - - - - - - - - - - - - -  ���������� ���������� */
              memset(&create_data, 0, sizeof(create_data)) ;

                                     entry=strstr(buff, "NAME=") ;  /* ��������� ��� ������� */
           strncpy(create_data.name, entry+strlen("NAME="), 
                                       sizeof(create_data.name)-1) ;
        end=strchr(create_data.name, '\n') ;
       *end= 0 ;

                                     entry=strstr(buff, "MODEL=") ; /* ��������� ������ ������� */
           strncpy(create_data.path, entry+strlen("MODEL="),
                                       sizeof(create_data.path)-1) ;
        end=strchr(create_data.path, '\n') ;
       *end= 0 ;

    for(i=0 ; i<_MODEL_PARS_MAX ; i++) {
             sprintf(name, "PAR_%d=", i) ;
        entry=strstr(buff, name) ;
     if(entry!=NULL) {
           strncpy(create_data.pars[i].value, entry+strlen(name), 
                                        sizeof(create_data.pars[i].value)-1) ;
        end=strchr(create_data.pars[i].value, '\n') ;
       *end= 0 ;
                     }
                                       } 
/*- - - - - - - - - - - - - - - �������� ���������� �������� ������� */
/*- - - - - - - - - - - - - - - - - - - - - - - - - �������� ������� */
                object=(RSS_Object_RocketLego *)vCreateObject(&create_data) ;
             if(object==NULL)  return ;
/*- - - - - - - - - - - - ������� ������� ����� � ���������� ������� */
       entry=strstr(buff, "X_BASE=") ; object->state.x   =atof(entry+strlen("X_BASE=")) ;
       entry=strstr(buff, "Y_BASE=") ; object->state.y   =atof(entry+strlen("Y_BASE=")) ;
       entry=strstr(buff, "Z_BASE=") ; object->state.z   =atof(entry+strlen("Z_BASE=")) ;
       entry=strstr(buff, "A_AZIM=") ; object->state.azim=atof(entry+strlen("A_AZIM=")) ;
       entry=strstr(buff, "A_ELEV=") ; object->state.elev=atof(entry+strlen("A_ELEV=")) ;
       entry=strstr(buff, "A_ROLL=") ; object->state.roll=atof(entry+strlen("A_ROLL=")) ;

   for(i=0 ; i<object->Features_cnt ; i++) {
        object->Features[i]->vBodyBasePoint(NULL, object->state.x, 
                                                  object->state.y, 
                                                  object->state.z ) ;
        object->Features[i]->vBodyAngles   (NULL, object->state.azim, 
                                                  object->state.elev, 
                                                  object->state.roll ) ;
                                           }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                }                   /* END.1 */
/*-------------------------------------------- ������������ �������� */

                free(buff) ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        �������� ������ � ������		    */

    void  RSS_Module_RocketLego::vWriteSave(std::string *text)

{
  std::string  buff ;

/*----------------------------------------------- ��������� �������� */

     *text="#BEGIN MODULE ROCKET LEGO\n" ;

/*------------------------------------------------ �������� �������� */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� HELP                    */

  int  RSS_Module_RocketLego::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Object_RocketLego_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� CREATE                  */
/*								    */
/*      CREATE <���> [<������> [<������ ����������>]]               */

  int  RSS_Module_RocketLego::cCreate(char *cmd)

{
 RSS_Model_data  data ;
     RSS_Object *object ;
           char *name ;
           char *model ;
           char *pars[10] ;
           char *end ;
           char  tmp[1024] ;
        INT_PTR  status ;
            int  i ;

/*-------------------------------------- ���������� ��������� ������ */

                             model ="" ;
     for(i=0 ; i<10 ; i++)  pars[i]="" ;

   do {                                                             /* BLOCK.1 */
                  name=cmd ;                                        /* ��������� ��� ������� */
                   end=strchr(name, ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;

                 model=end+1 ;                                      /* ��������� �������� ������ */
                   end=strchr(model, ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
          
     for(i=0 ; i<10 ; i++) {                                        /* ��������� ��������� */
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

        sprintf(data.lib_path, "%s\\Body.lib", getcwd(tmp, sizeof(tmp))) ;

/*---------------------- �������� ������������� ��������� ���������� */

   if(data.name[0]!=0) {
                            object=vCreateObject(&data) ;
                         if(object!=NULL) {
                                             this->kernel->vShow(NULL) ;
                                                return(0) ;
                                          }
                       }
/*----------------------------------------------- ���������� ������� */

      status=DialogBoxIndirectParam( GetModuleHandle(NULL),
                                    (LPCDLGTEMPLATE)Resource("IDD_CREATE", RT_DIALOG),
			             GetActiveWindow(), 
                                     Object_RocketLego_Create_dialog, 
                                    (LPARAM)&data                     ) ;
   if(status)  return(-1) ;

            this->kernel->vShow(NULL) ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� INFO                    */
/*								    */
/*        INFO   <���>                                              */
/*        INFO/  <���>                                              */

  int  RSS_Module_RocketLego::cInfo(char *cmd)

{
                  char  *name ;
 RSS_Object_RocketLego  *object ;
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

/*------------------------------------------- �������� ����� ������� */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* ���� ��� �� ������... */
                           SEND_ERROR("�� ������ ��� �������. \n"
                                      "��������: INFO <���_�������> ...") ;
                                     return(-1) ;
                         }

       object=(RSS_Object_RocketLego *)FindObject(name, 1) ;           /* ���� ������ �� ����� */
    if(object==NULL)  return(-1) ;

/*-------------------------------------------- ������������ �������� */

      sprintf(text, "%s\r\n%s\r\n"
                    "Base X % 8.2lf\r\n" 
                    "     Y % 8.2lf\r\n" 
                    "     Z % 8.2lf\r\n"
                    "Ang. A % 8.2lf\r\n" 
                    "     E % 8.2lf\r\n" 
                    "     R % 8.2lf\r\n"
                    "Vel. V % 8.2lf\r\n" 
                    "     X % 8.2lf\r\n" 
                    "     Y % 8.2lf\r\n" 
                    "     Z % 8.2lf\r\n"
                    "Owner  %s\r\n" 
                    "\r\n",
                        object->Name, object->Type, 
                        object->state.x, object->state.y, object->state.z,
                        object->state.azim, object->state.elev, object->state.roll,
                   sqrt(object->state.x_velocity*object->state.x_velocity+
                        object->state.y_velocity*object->state.y_velocity+
                        object->state.z_velocity*object->state.z_velocity ),
                        object->state.x_velocity, object->state.y_velocity, object->state.z_velocity,
                        object->owner                    ) ;

           info=text ;

/*----------------------------------------------- ������ �� �������� */

   for(i=0 ; i<object->Features_cnt ; i++) {

                      object->Features[i]->vGetInfo(&f_info) ;
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
/*		      ���������� ���������� COPY                    */
/*								    */
/*       COPY <��� �������> <��� ������ �������>                    */

  int  RSS_Module_RocketLego::cCopy(char *cmd)

{
#define   _PARS_MAX  10

                  char  *pars[_PARS_MAX] ;
                  char  *name ;
                  char  *copy ;
 RSS_Object_RocketLego  *rocket ;
            RSS_Object  *object ;
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

                     name=pars[0] ;
                     copy=pars[1] ;   

/*------------------------------------------- �������� ����� ������� */

    if(name==NULL) {                                                /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� �������. \n"
                                 "��������: COPY <���_�������> ...") ;
                                     return(-1) ;
                   }

       rocket=(RSS_Object_RocketLego *)FindObject(name, 1) ;        /* ���� ������-���� �� ����� */
    if(rocket==NULL)  return(-1) ;

/*------------------------------------------ �������� ����� �������� */

    if(copy==NULL) {                                                /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� �������-�����. \n"
                                 "��������: COPY <��� �������> <��� ������ �������>") ;
                                     return(-1) ;
                   }

//     object=FindObject(copy, 0) ;                                 /* ���� ������-�������� �� ����� */
//  if(object!=NULL) {
//                    SEND_ERROR("O�����-����� ��� ����������") ;
//                                   return(-1) ;
//                   }
/*---------------------------------------------- ����������� ������� */

            object=rocket->vCopy(copy) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� OWNER                   */
/*								    */
/*       OWNER <���> <��������>                                     */

  int  RSS_Module_RocketLego::cOwner(char *cmd)

{
#define   _PARS_MAX  10

                  char  *pars[_PARS_MAX] ;
                  char  *name ;
                  char  *owner ;
 RSS_Object_RocketLego  *rocket ;
            RSS_Object  *object ;
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

                     name=pars[0] ;
                    owner=pars[1] ;   

/*------------------------------------------- �������� ����� ������� */

    if(name==NULL) {                                                /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� �������. \n"
                                 "��������: OWNER <���_�������> ...") ;
                                     return(-1) ;
                   }

       rocket=(RSS_Object_RocketLego *)FindObject(name, 1) ;        /* ���� ������-���� �� ����� */
    if(rocket==NULL)  return(-1) ;

/*------------------------------------------ �������� ����� �������� */

    if(owner==NULL) {                                               /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� �������-��������. \n"
                                 "��������: OWNER <���_������> <���_��������>") ;
                                     return(-1) ;
                    }

       object=FindObject(owner, 0) ;                                /* ���� ������-�������� �� ����� */
    if(object==NULL)  return(-1) ;

/*------------------------------------------------- ������� �������� */

          strcpy(rocket->owner, owner) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� LEGO                    */
/*								    */
/*     LEGO <��� �������>                                           */
/*     LEGO <��� �������> <��� ����������> <��� ����������>         */

  int  RSS_Module_RocketLego::cLego(char *cmd)

{
#define   _PARS_MAX  10

                  char *pars[_PARS_MAX] ;
                  char *name ;
                  char *unit_name ;
                  char *unit_type ;
 RSS_Object_RocketLego *object ;
            RSS_Object *unit ;
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
                                     SEND_ERROR("�� ����� ������ ����������") ;
                                         return(-1) ;
                                 }

                     name=pars[0] ;
                unit_name=pars[1] ;
                unit_type=pars[2] ;

/*------------------------------------------- �������� ����� ������� */

    if(name==NULL) {                                                /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� �������. \n"
                                 "��������: LEGO <���_�������> ...") ;
                                     return(-1) ;
                   }

       object=(RSS_Object_RocketLego *)FindObject(name, 1) ;        /* ���� ������ �� ����� */
    if(object==NULL)  return(-1) ;

/*--------------------------------------- ������� � ���������� ����� */

   if(unit_name==NULL ||
      unit_type==NULL   ) {

        status=DialogBoxIndirectParam( GetModuleHandle(NULL),
                                      (LPCDLGTEMPLATE)Resource("IDD_LEGO", RT_DIALOG),
                                       GetActiveWindow(), 
                                       Object_RocketLego_Lego_dialog, 
                                      (LPARAM)object               ) ;
         return((int)status) ;

                          }
/*-------------------------------- �������� � ����������� ���������� */

   if(stricmp(unit_name, "warhead") &&
      stricmp(unit_name, "engine" ) &&
      stricmp(unit_name, "model"  )   ) {
              SEND_ERROR("��������� ��������� ����� lego-�����������: WARHEAD, ENGINE � MODEL") ;
                     return(-1) ;
                                        }

         unit=AddUnit(object, unit_name, unit_type, error) ;        
      if(unit==NULL) {
                       SEND_ERROR(error) ;
                             return(-1) ;
                     }
/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� TRACE                   */
/*								    */
/*       TRACE <���> [<������������>]                               */
/*       TRACE/C <���> <R>:<G>:<B> [...]                            */

  int  RSS_Module_RocketLego::cTrace(char *cmd)

{
#define   _PARS_MAX  20

                  char *pars[_PARS_MAX] ;
                  char *name ;
                double  trace_time ;
                double  time_0 ;        /* ��������� ����� ������� */ 
                double  time_1 ;        /* ������� ����� */ 
                double  time_c ;        /* ���������� ����� ������� */ 
                double  time_s ;        /* ��������� ����� ��������� */ 
                double  time_w ;        /* ����� �������� */ 
 RSS_Object_RocketLego *object ;
                   int  c_flag ;
                   int  w_flag ;
                   int  color_r, color_g, color_b ;
                   int  width ;
                  char  text[1024] ;
                  char *end ;
                   int  quit_flag ;
                   int  status ;
                   int  i ;

/*---------------------------------------- �������� ��������� ������ */

                     trace_time=0. ;
/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
                             c_flag=0 ;
                             w_flag=0 ;
                          quit_flag=0 ;

       if(*cmd=='/') {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("������������ ������ �������") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, 'c')!=NULL ||
                   strchr(cmd, 'C')!=NULL   )  c_flag=1 ;

                if(strchr(cmd, 'w')!=NULL ||
                   strchr(cmd, 'W')!=NULL   )  w_flag=1 ;

                if(strchr(cmd, 'q')!=NULL ||
                   strchr(cmd, 'Q')!=NULL   )  quit_flag=1 ;

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

                     name=pars[0] ;

      if( pars[1]!=NULL &&
         *pars[1]!=  0    ) {
                                      trace_time=strtod(pars[1], &end) ;
                                   if(trace_time<=0.) {
                                         SEND_ERROR("������ ������������ ����� �����������") ;
                                                           return(-1) ;
                                                      }
                            }
      else 
      if(c_flag)            {
                            }
      else
      if(w_flag)            {
                            }
      else                  {
                                            trace_time=60. ;
             if(!quit_flag)  SEND_ERROR("����� ����������� - 60 ������") ;
                            }
/*------------------------------------------- �������� ����� ������� */

    if(name==NULL) {                                                /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� �������. \n"
                                 "��������: TRACE <���_�������> ...") ;
                                     return(-1) ;
                   }

       object=(RSS_Object_RocketLego *)FindObject(name, 1) ;        /* ���� ������ �� ����� */
    if(object==NULL)  return(-1) ;

/*----------------------------------------------- ���� ������� ����� */

   if(c_flag) {

       if(pars[1]==NULL) {
                            SEND_ERROR("������ �������: TRACE/C <���> <R>:<G>:<B> [...]") ;
                                       return(-1) ;
                         }

     for(i=0 ; pars[i+1]!=NULL ; i++) {

           color_r=strtoul(pars[i+1], &end, 10) ;
       if(*end!=':' || color_r>255) {
                     SEND_ERROR("��������� ����� <R> ������ ���� ������ 0...255") ;
                                       return(-1) ;
                                    }

          color_g=strtoul(end+1, &end, 10) ;
       if(*end!=':' || color_g>255) {
                     SEND_ERROR("��������� ����� <G> ������ ���� ������ 0...255") ;
                                       return(-1) ;
                                    }

          color_b=strtoul(end+1, &end, 10) ;
       if(*end!=0 || color_b>255) {
                     SEND_ERROR("��������� ����� <B> ������ ���� ������ 0...255") ;
                                       return(-1) ;
                                }

           object->mTrace_colors[i] =RGB(color_r, color_g, color_b) ;
           object->mTrace_colors_cnt=i+1 ;

                                      }

                        return(0) ;
              }
/*----------------------------------------------- ���� ������� ����� */

   if(w_flag) {

     if(pars[1]==NULL) {
                          SEND_ERROR("������ �������: TRACE/W <�������>") ;
                                       return(-1) ;
                       }

         width=strtoul(pars[1], &end, 10) ;
     if(*end!=0 || width>5) {
                     SEND_ERROR("��������� ����� <R> ������ ���� ������ 1...5") ;
                                       return(-1) ;
                            }

          object->mTrace_width=width ;

                                       return(0) ;
              }
/*------------------------------------------------ �������� �������� */

    if(object->owner[0]!=0) {                                       /* ���� ����� ��������... */

         object->o_owner=FindObject(object->owner, 0) ;             /*   ���� �������� �� ����� */
      if(object->o_owner==NULL)  return(-1) ;
                            }
/*------------------------------------------------------ ����������� */

              time_0=this->kernel->vGetTime() ;
              time_c=0. ;
              time_s=0. ;

         object->vCalculateStart(0.) ;

    do {                                                            /* CIRCLE.1 - ���� ����������� */
           if(this->kernel->stop)  break ;                          /* ���� ������� ���������� ������ */
/*- - - - - - - - - - - - - - - - - - - - - - - -  ��������� ������� */
              time_c+=RSS_Kernel::calc_time_step ;
              time_1=this->kernel->vGetTime() ;

           if(time_1-time_0>trace_time)  break ;                    /* ���� ����� ����������� ����������� */

              time_w=time_c-(time_1-time_0) ;

#pragma warning(disable : 4244)
           if(time_w>=0)  Sleep(time_w*1000) ;
#pragma warning(default : 4244)
/*- - - - - - - - - - - - - - - - - - - - - - ������������� �������� */
      if(object->o_target!=NULL)  object->o_target->state_0=object->o_target->state ; 
                                  object          ->state_0=object          ->state ; 

         status=object->vCalculate    (time_c-RSS_Kernel::calc_time_step, time_c, NULL, 0) ;
                object->vCalculateShow(time_c-RSS_Kernel::calc_time_step, time_c) ;

                object->iShowTrace_("SHOW_TRACE") ;                 /* ����������� ���������� */
/*- - - - - - - - - - - - - - - - - - - - - - -  ����������� ������� */
   for(i=0 ; i<object->Features_cnt ; i++) {
     object->Features[i]->vBodyBasePoint(NULL, object->state.x, 
                                               object->state.y, 
                                               object->state.z ) ;
     object->Features[i]->vBodyAngles   (NULL, object->state.azim, 
                                               object->state.elev, 
                                               object->state.roll ) ;
                                            }
/*- - - - - - - - - - - - - - - - - - - - - - - - -  ��������� ����� */
          time_1=this->kernel->vGetTime() ;
       if(time_1-time_s>=this->kernel->show_time_step) {

                 time_s=time_1 ;

              this->kernel->vShow(NULL) ;
                                                       }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      if(status)  break ;  

       } while(1) ;                                                 /* END CIRCLE.1 - ���� ����������� */

   if(!quit_flag) {
              sprintf(text, "����������a ��������� �� %lf ������", time_c) ;
           SEND_ERROR(text) ;
                  }
/*-------------------------------------------------------------------*/

#undef   _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� SPAWN                   */
/*								    */
/*       SPAWN <���> <������� �����> <������������� �������>        */

  int  RSS_Module_RocketLego::cSpawn(char *cmd)

{
#define   _PARS_MAX  10

                  char *pars[_PARS_MAX] ;
                  char *name ;
                double  spawn_max ;
                double  spawn_period ;
                double  time_0 ;        /* ��������� ����� ������� */ 
                double  time_1 ;        /* ������� ����� */ 
                double  time_c ;        /* ���������� ����� ������� */ 
                double  time_s ;        /* ��������� ����� ��������� */ 
                double  time_w ;        /* ����� �������� */ 
                double  time_z ;        /* ����� ���������� ������� */ 
 RSS_Object_RocketLego *object ;
            RSS_Object *clone ;
                   int  break_mark[1000] ;
                   int  break_cnt ;
                  char  text[1024] ;
                  char *end ;
                   int  quit_flag ;
                   int  status ;
                   int  n ;
                   int  i ;

/*---------------------------------------- �������� ��������� ������ */

                     spawn_max   =0. ;
                     spawn_period=0. ;
/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
                          quit_flag=0 ;

       if(*cmd=='/') {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("������������ ������ �������") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, 'q')!=NULL ||
                   strchr(cmd, 'Q')!=NULL   )  quit_flag=1 ;

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

           if( pars[1]==NULL ||
              *pars[1]==  0    ) {
                                    SEND_ERROR("�� ������ ������� �����") ;
                                                           return(-1) ;
                                 }
           if( pars[2]==NULL ||
              *pars[2]==  0    ) {
                                    SEND_ERROR("�� ������ ������������� �����") ;
                                                           return(-1) ;
                                 }

                     name=pars[0] ;

                spawn_max=strtoul(pars[1], &end, 10) ;
           if(*end!=0) {
                           SEND_ERROR("������������ �������� ������� �����") ;
                                            return(-1) ;
                       }

                spawn_period=strtod(pars[2], &end) ;
           if(*end!=0) {
                           SEND_ERROR("������������ �������� ������������� �����") ;
                                            return(-1) ;
                       }
/*------------------------------------------- �������� ����� ������� */

    if(name==NULL) {                                                /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� �������. \n"
                                 "��������: SPAWN <���_�������> ...") ;
                                     return(-1) ;
                   }

       object=(RSS_Object_RocketLego *)FindObject(name, 1) ;        /* ���� ������ �� ����� */
    if(object==NULL)  return(-1) ;

/*------------------------------------------------ �������� �������� */

       object->o_owner=FindObject(object->owner, 0) ;               /* ���� �������� �� ����� */
    if(object->o_owner==NULL)  return(-1) ;

/*---------------------------------------- ������� ����������� ����� */

                   object->iClearSpawn() ;

              memset(break_mark, 0, sizeof(break_mark)) ;
                     break_cnt=0 ;

/*------------------------------------------------------ ����������� */

              time_0=this->kernel->vGetTime() ;
              time_c=0. ;
              time_s=0. ;
              time_z=0. ;

    do {                                                            /* CIRCLE.1 - ���� ����������� */
           if(this->kernel->stop)  break ;                          /* ���� ������� ���������� ������ */
/*- - - - - - - - - - - - - - - - - - - - - - - -  ��������� ������� */
              time_c+=RSS_Kernel::calc_time_step ;
              time_1=this->kernel->vGetTime() ;

//         if(time_1-time_0>trace_time)  break ;                    /* ���� ����� ����������� ����������� */

              time_w=time_c-(time_1-time_0) ;

#pragma warning(disable : 4244)
           if(time_w>=0)  Sleep(time_w*1000) ;
#pragma warning(default : 4244)
/*- - - - - - - - - - - - - - - - - - - - - - ������������� �������� */
#define  CLONE(k)  ((RSS_Object_RocketLego *)object->mSpawn[k])

       for(n=0 ; n<object->mSpawn_cnt ; n++) if(!break_mark[n]) {

            status=CLONE(n)->vCalculate(time_c-RSS_Kernel::calc_time_step, time_c, NULL, 0) ;
         if(status) {
                       break_mark[n]=1 ;
                       break_cnt++ ;
                    }

                  CLONE(n)->vCalculateShow(time_c-RSS_Kernel::calc_time_step, time_c) ;

                  CLONE(n)->iShowTrace_("SHOW_TRACE") ;             /* ����������� ���������� */

         for(i=0 ; i<CLONE(n)->Features_cnt ; i++) {                /* ����������� ������� */
           CLONE(n)->Features[i]->vBodyBasePoint(NULL, CLONE(n)->state.x, 
                                                       CLONE(n)->state.y, 
                                                       CLONE(n)->state.z ) ;
           CLONE(n)->Features[i]->vBodyAngles   (NULL, CLONE(n)->state.azim, 
                                                       CLONE(n)->state.elev, 
                                                       CLONE(n)->state.roll ) ;
                                                   }
                                                                } 

#undef  CLONE
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ���� */
       while(spawn_max>object->mSpawn_cnt &&
              time_z  <  time_c             ) {

                clone=object->vCopy(NULL) ;

                clone->vCalculateStart(time_z) ;
                clone->vCalculate     (time_z, time_c, NULL, 0) ;
                clone->vCalculateShow (time_z, time_c) ;

               object->mSpawn=(RSS_Object **)
                                realloc(object->mSpawn, sizeof(*object->mSpawn)*(object->mSpawn_cnt+1)) ;

               object->mSpawn[object->mSpawn_cnt]=clone ;
                              object->mSpawn_cnt++ ;

                       time_z+=spawn_period ;
                                              }
/*- - - - - - - - - - - - - - - - - - - - - - - - -  ��������� ����� */
          time_1=this->kernel->vGetTime() ;
       if(time_1-time_s>=this->kernel->show_time_step) {

                 time_s=time_1 ;

              this->kernel->vShow(NULL) ;
                                                       }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
       if(object->mSpawn_cnt==spawn_max &&
          object->mSpawn_cnt==break_cnt   )  break ;  

       } while(1) ;                                                 /* END CIRCLE.1 - ���� ����������� */

   if(!quit_flag) {
              sprintf(text, "����������a ��������� �� %lf ������", time_c) ;
           SEND_ERROR(text) ;
                  }
/*-------------------------------------------------------------------*/

#undef   _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� STAT                    */
/*								    */
/*       STAT/D <���>                                               */

  int  RSS_Module_RocketLego::cStat(char *cmd)

{
#define   _PARS_MAX  10

                  char *pars[_PARS_MAX] ;
                  char *name ;
 RSS_Object_RocketLego *object ;
                  char *end ;
                   int  drops_view ;
                   int  i ;

/*---------------------------------------- �������� ��������� ������ */

/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
                          drops_view=0 ;

       if(*cmd=='/') {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("������������ ������ �������") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, 'd')!=NULL ||
                   strchr(cmd, 'D')!=NULL   )  drops_view=1 ;

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

                     name=pars[0] ;

/*------------------------------------------- �������� ����� ������� */

    if(name==NULL) {                                                /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� �������. \n"
                                 "��������: STAT <���_�������> ...") ;
                                     return(-1) ;
                   }

       object=(RSS_Object_RocketLego *)FindObject(name, 1) ;        /* ���� ������ �� ����� */
    if(object==NULL)  return(-1) ;

/*------------------------------------------- �������� ������� ����� */

      if(object->mSpawn_cnt==0) {
                      SEND_ERROR("������ �� �������� ����������� ������� SPAWN") ;
                                     return(-1) ;
                                }
/*------------------------- ������������� ����� �������/������������ */

    if(drops_view) {

       DialogBoxIndirectParam(GetModuleHandle(NULL),
                              (LPCDLGTEMPLATE)Resource("IDD_DROPS_VIEW", RT_DIALOG),
                                GetActiveWindow(), Object_RocketLego_Drops_dialog, (LPARAM)object) ;
                   }
/*-------------------------------------------------------------------*/

#undef   _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*								    */
/*             ����� ������� ���� ROCKET LEGO �� �����              */

  RSS_Object *RSS_Module_RocketLego::FindObject(char *name, int  check_type)

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

    if(check_type)
     if(strcmp(OBJECTS[i]->Type, "Rocket-Lego")) {

           SEND_ERROR("������ �� �������� �������� ���� ROCKET LEGO") ;
                            return(NULL) ;
                                             }
/*-------------------------------------------------------------------*/ 

   return(OBJECTS[i]) ;
  
#undef   OBJECTS
#undef   OBJECTS_CNT

}


/*********************************************************************/
/*                                                                   */
/*                 ���������� ���������� � �������                   */

  class RSS_Unit *RSS_Module_RocketLego::AddUnit(RSS_Object_RocketLego *object, char *unit_name, char *unit_type, char *error)

{
   RSS_Kernel *unit_module ;
     RSS_Unit *unit ;
          int  i ;

/*---------------------------------------- �������� ����� ���������� */

  if(stricmp(unit_name, "warhead") &&
     stricmp(unit_name, "engine" ) &&
     stricmp(unit_name, "model"  )   ) {                            /* ����� ���� lego-����������� */

   for(i=0 ; i<object->Units.List_cnt ; i++)
     if(!stricmp(unit_name, object->Units.List[i].object->Name))  break ;

      if(i<object->Units.List_cnt) {
            strcpy(error, "� ������ ������� ��� ������� ��������� � ����� ������") ;
                                     return(NULL) ;
                                   }
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

        unit=(RSS_Unit *)unit_module->vCreateObject(NULL) ;
     if(unit==NULL)   return(NULL) ;

        strncpy(unit->Name, unit_name, sizeof(unit->Name)-1) ;
                unit->Owner=object ;
/*- - - - - - - - - - - - - - - - - - - ����������� Lego-����������� */
  if(!stricmp(unit_name, "warhead")) {
                                        object->unit_warhead=(RSS_Unit_WarHead *)unit ;
                                     }
  else
  if(!stricmp(unit_name, "engine" )) {
                                        object->unit_engine=(RSS_Unit_Engine *)unit ;
                                     }
  else
  if(!stricmp(unit_name, "model"  )) {
                                        object->unit_model=(RSS_Unit_Model *)unit ;
                                     }
/*- - - - - - - - - - -  ����������� ����- � "���������" ����������� */
                  object->Units.Add(unit, "") ;

/*-------------------------------------------------------------------*/

  return(unit) ;
}


/********************************************************************/
/********************************************************************/
/**							           **/
/**		  �������� ������ ������� "���-����"	           **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       ����������� ������			    */

     RSS_Object_RocketLego::RSS_Object_RocketLego(void)

{
   strcpy(Type, "Rocket-Lego") ;

          Module=&ProgramModule ;

    Context        =new RSS_Transit_RocketLego ;
    Context->object=this ;

   Parameters    =NULL ;
   Parameters_cnt=  0 ;

     unit_warhead=NULL ;
     unit_engine =NULL ;
     unit_model  =NULL ;

       mSpawn    =NULL ;
       mSpawn_cnt= 0  ;

  battle_state   = 0 ; 

       state.x         = 0. ;
       state.y         = 0. ;
       state.z         = 0. ;
       state.azim      = 0. ;
       state.elev      = 0. ;
       state.roll      = 0. ;
       state.x_velocity= 0. ;
       state.y_velocity= 0. ;
       state.z_velocity= 0. ;

  memset(owner,  0, sizeof(owner )) ;
       o_owner =NULL ;   

         mTrace      =    NULL ;
         mTrace_cnt  =      0 ;  
         mTrace_max  =      0 ; 
         mTrace_width=      1 ;
  memset(mTrace_colors, 0, sizeof(mTrace_colors)) ; 
         mTrace_colors[0]=RGB(0, 0, 127) ;
         mTrace_colors_cnt= 1 ;
         mTrace_dlist     = 0 ;  

      hDropsViewWnd=NULL ;
}


/********************************************************************/
/*								    */
/*		        ���������� ������			    */

    RSS_Object_RocketLego::~RSS_Object_RocketLego(void)

{
      vFree() ;

   delete Context ;
}


/********************************************************************/
/*								    */
/*		       ������������ ��������                        */

  void   RSS_Object_RocketLego::vFree(void)

{
  int  i ;


           iClearSpawn() ;

       iSaveTracePoint("CLEAR") ;
           iShowTrace_("CLEAR_TRACE") ;

  if(this->mTrace!=NULL) {
                             free(this->mTrace) ;
                                  this->mTrace    =NULL ;
                                  this->mTrace_cnt=  0 ;  
                                  this->mTrace_max=  0 ;  
                         }

  if(this->Features!=NULL) {

    for(i=0 ; i<this->Features_cnt ; i++) {
                this->Features[i]->vBodyDelete(NULL) ;
           free(this->Features[i]) ;
                                         }

           free(this->Features) ;

                this->Features    =NULL ;
                this->Features_cnt=  0 ;
                           }
}


/********************************************************************/
/*								    */
/*	      ������������ �������� ������������� �����             */

  void   RSS_Object_RocketLego::iClearSpawn(void)

{
  int  i ;


  if(this->mSpawn!=NULL) {
    for(i=0 ; i<this->mSpawn_cnt ; i++) {
                                           delete (RSS_Object_RocketLego *)this->mSpawn[i] ;
                                        }

                             free(this->mSpawn) ;
                                  this->mSpawn    =NULL ;
                                  this->mSpawn_cnt=  0 ;
                         }
}


/********************************************************************/
/*								    */
/*                        ���������� ������		            */

    class RSS_Object *RSS_Object_RocketLego::vCopy(char *name)

{
        RSS_Model_data  create_data ;
 RSS_Object_RocketLego *object ;
       RSS_Feature_Hit *hit_1 ;
       RSS_Feature_Hit *hit_2 ;
              RSS_Unit *unit ;
                   int  i ;

/*------------------------------------- ����������� �������� ������� */

                 memset(&create_data, 0, sizeof(create_data)) ;

 if(name!=NULL)  strcpy( create_data.name, name) ;
                 strcpy( create_data.path, this->model_path) ;

    for(i=0 ; i<this->Parameters_cnt ; i++) {
         sprintf(create_data.pars[i].text,  "PAR%d", i) ;
         sprintf(create_data.pars[i].value, "%lf", this->Parameters[i].value) ;
                                            }

                 create_data.pars[i].text [0]=0 ;
                 create_data.pars[i].value[0]=0 ;

       object=(RSS_Object_RocketLego *)this->Module->vCreateObject(&create_data) ;
    if(object==NULL)  return(NULL) ;
 
            strcpy(object->owner,            this->owner) ;
                   object->o_owner          =this->o_owner ;
                   object->mTrace_width     =this->mTrace_width ;
            memcpy(object->mTrace_colors,    this->mTrace_colors, sizeof(this->mTrace_colors)) ;
                   object->mTrace_colors_cnt=this->mTrace_colors_cnt ;

   if(RSS_Kernel::battle)  object->battle_state=_SPAWN_STATE ;

/*---------------------------------------------- ����������� ������� */

    for(i=0 ; i<this->Features_cnt ; i++)
      if(!stricmp(this->Features[i]->Type, "Hit"))
             hit_1=(RSS_Feature_Hit *)this->Features[i] ;

    for(i=0 ; i<object->Features_cnt ; i++)
      if(!stricmp(object->Features[i]->Type, "Hit"))
             hit_2=(RSS_Feature_Hit *)object->Features[i] ;

           hit_2->hit_range =hit_1->hit_range ;
           hit_2->any_target=hit_1->any_target ;
           hit_2->any_weapon=hit_1->any_weapon ;

/*----------------------------------------- ����������� ����-������� */

      if(this->unit_warhead!=NULL) {
                         unit=(RSS_Unit *)this->unit_warhead->vCopy(object) ;
                  strcpy(unit->Name, "warhead") ;
                         unit->Owner=object ;

                 object->unit_warhead=(RSS_Unit_WarHead *)unit ;
                 object->Units.Add(unit, "") ;
                                   }
      if(this->unit_engine !=NULL) {
                         unit=(RSS_Unit *)this->unit_engine->vCopy(object) ;
                  strcpy(unit->Name, "engine") ;
                         unit->Owner=object ;

                 object->unit_engine=(RSS_Unit_Engine *)unit ;
                 object->Units.Add(unit, "") ;
                                   }
      if(this->unit_model  !=NULL) {
                         unit=(RSS_Unit *)this->unit_model->vCopy(object) ;
                  strcpy(unit->Name, "model") ;
                         unit->Owner=object ;

                 object->unit_model=(RSS_Unit_Model *)unit ;
                 object->Units.Add(unit, "") ;
                                   }
/*-------------------------------------------------------------------*/

   return(object) ;
}


/********************************************************************/
/*								    */
/*		        �������� ������ � ������		    */

    void  RSS_Object_RocketLego::vWriteSave(std::string *text)

{
  char  field[1024] ;
   int  i ;

/*----------------------------------------------- ��������� �������� */

     *text="#BEGIN OBJECT ROCKET LEGO\n" ;

/*----------------------------------------------------------- ������ */

    sprintf(field, "NAME=%s\n",       this->Name      ) ;  *text+=field ;
    sprintf(field, "X_BASE=%.10lf\n", this->state.x   ) ;  *text+=field ;
    sprintf(field, "Y_BASE=%.10lf\n", this->state.y   ) ;  *text+=field ;
    sprintf(field, "Z_BASE=%.10lf\n", this->state.z   ) ;  *text+=field ;
    sprintf(field, "A_AZIM=%.10lf\n", this->state.azim) ;  *text+=field ;
    sprintf(field, "A_ELEV=%.10lf\n", this->state.elev) ;  *text+=field ;
    sprintf(field, "A_ROLL=%.10lf\n", this->state.roll) ;  *text+=field ;
    sprintf(field, "MODEL=%s\n",      this->model_path) ;  *text+=field ;

  for(i=0 ; i<this->Parameters_cnt ; i++) {
    sprintf(field, "PAR_%d=%.10lf\n", 
                          i, this->Parameters[i].value) ;  *text+=field ;
                                          }
/*------------------------------------------------ �������� �������� */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*                        ����������� ��������                      */

     int  RSS_Object_RocketLego::vSpecial(char *oper, void *data)
{
  return(-1) ;
}


/********************************************************************/
/*								    */
/*             ���������� ������� ��������� ���������               */

     int  RSS_Object_RocketLego::vCalculateStart(double t)
{
   int  i ; 

#define   OBJECTS       RSS_Kernel::kernel->kernel_objects
#define   OBJECTS_CNT   RSS_Kernel::kernel->kernel_objects_cnt

/*-------------------------------------- �������� � �������-�������� */

      this->o_owner=NULL ;

  if(this->owner[0]!=0) {

       for(i=0 ; i<OBJECTS_CNT ; i++)                               /* ���� ������ �� ����� */
         if(!stricmp(OBJECTS[i]->Name, this->owner)) {
                       this->o_owner=OBJECTS[i] ;
                                 break ;
                                                     }
                        }

  if(this->o_owner!=NULL) {

      this->state=this->o_owner->state_0 ;

                          }
/*-------------------------------------------- ��������� ����������� */

      if(this->unit_engine !=NULL)  this->unit_engine ->vCalculateStart(t) ;
      if(this->unit_warhead!=NULL)  this->unit_warhead->vCalculateStart(t) ;
      if(this->unit_model  !=NULL)  this->unit_model  ->vCalculateStart(t) ;

    for(i=0 ; i<this->Units.List_cnt ; i++)                         /* ����� LEGO-����������� */
      if(stricmp(this->Units.List[i].object->Name, "engine" ) &&
         stricmp(this->Units.List[i].object->Name, "warhead") &&
         stricmp(this->Units.List[i].object->Name, "model"  )   )
                   this->Units.List[i].object->vCalculateStart(t) ;

/*------------------------------------------------ ������� ��������� */

    this->iSaveTracePoint("CLEAR") ;

/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

  return(0) ;
}


/********************************************************************/
/*								    */
/*                   ������ ��������� ���������                     */

     int  RSS_Object_RocketLego::vCalculate(double t1, double t2, char *callback, int cb_size)
{
  RSS_Unit_Engine_Thrust  thrust[10] ;
                     int  thrust_cnt ;
                     int  mass_use ;
                  double  mass ;
               RSS_Point  mass_point ;
                     int  mi_use ;
                  double  mi_x ;
                  double  mi_y ;
                  double  mi_z ;
                     int  status ;

/*------------------------------------------------------- ���������� */

            thrust_cnt=0 ;
              mass_use=0 ;
                mi_use=0 ;

/*---------------------------------------------------- ������������� */

      if(this->unit_engine !=NULL) {

                       this->unit_engine->vCalculate      (t1, t2, callback, cb_size) ;

            thrust_cnt=this->unit_engine->vGetEngineThrust(thrust) ;
              mass_use=this->unit_engine->vGetEngineMass  (&mass, &mass_point) ;
                mi_use=this->unit_engine->vGetEngineMI    (&mi_x, &mi_y, &mi_z) ;

                                   } 
      if(this->unit_model  !=NULL) {

          if(thrust_cnt)  this->unit_model->vSetEngineThrust(thrust, thrust_cnt) ;
          if(  mass_use)  this->unit_model->vSetEngineMass  (mass, &mass_point) ;
          if(    mi_use)  this->unit_model->vSetEngineMI    (mi_x, mi_y, mi_z) ;

                          this->unit_model->vCalculate(t1, t2, callback, cb_size) ;

                                   } 
      if(this->unit_warhead!=NULL) {

            status=this->unit_warhead->vCalculate(t1, t2, callback, cb_size) ;
         if(status)  return(1) ;

                                   } 
/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*      ����������� ���������� ������� ��������� ���������          */

     int  RSS_Object_RocketLego::vCalculateShow(double  t1, double t2)
{
   int i ;


         this->iSaveTracePoint("ADD") ;                             /* ���������� ����� ���������� */  

         this->iShowTrace_("SHOW_TRACE") ;                          /* ����������� ���������� */

   for(i=0 ; i<this->Features_cnt ; i++) {                          /* ����������� ������� */
     this->Features[i]->vBodyBasePoint(NULL, this->state.x, 
                                             this->state.y, 
                                             this->state.z ) ;
     this->Features[i]->vBodyAngles   (NULL, this->state.azim, 
                                             this->state.elev, 
                                             this->state.roll ) ;
                                            }

  return(0) ;
}


/*********************************************************************/
/*								     */
/*                   ���������� ����� ����������                     */

  int  RSS_Object_RocketLego::iSaveTracePoint(char *action)

{
/*------------------------------------------------- ����� ���������� */

   if(!stricmp(action, "CLEAR")) {
                                     mTrace_cnt=0 ;
                                       return(0) ;
                                 }
/*----------------------------------------------- ����������� ������ */

   if(mTrace_cnt==mTrace_max) {

          mTrace_max+= 1000 ;
          mTrace     =(RSS_Object_RocketLegoTrace *)
                            realloc(mTrace, mTrace_max*sizeof(RSS_Object_RocketLegoTrace)) ;

       if(mTrace==NULL) {
                   SEND_ERROR("ROCKET-LEGO.iSaveTracePoint@"
                              "Memory over for trajectory") ;
                                  return(-1) ;
                        }
                              }
/*------------------------------------------------- ���������� ����� */
                  
                  mTrace[mTrace_cnt].x_base    = this->state.x ;
                  mTrace[mTrace_cnt].y_base    = this->state.y ;
                  mTrace[mTrace_cnt].z_base    = this->state.z ;
                  mTrace[mTrace_cnt].a_azim    = this->state.azim ;
                  mTrace[mTrace_cnt].a_elev    = this->state.elev ;
                  mTrace[mTrace_cnt].a_roll    = this->state.roll ;
                  mTrace[mTrace_cnt].x_velocity= this->state.x_velocity ;
                  mTrace[mTrace_cnt].y_velocity= this->state.y_velocity ;
                  mTrace[mTrace_cnt].z_velocity= this->state.z_velocity ;
                  mTrace[mTrace_cnt].color     =mTrace_colors[this->state_idx % mTrace_colors_cnt] ;

                         mTrace_cnt++ ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*           ����������� ���������� � ��������� ���������           */

  void  RSS_Object_RocketLego::iShowTrace_(char *action)

{
    strcpy(Context->action, "SHOW_TRACE") ;

  SendMessage(RSS_Kernel::kernel_wnd, 
                WM_USER, (WPARAM)_USER_CHANGE_CONTEXT, 
                         (LPARAM) this->Context       ) ;
}


/*********************************************************************/
/*								     */
/*                     ����������� ����������                        */

  void  RSS_Object_RocketLego::iShowTrace(char *action)

{
       int  status ;
       int  i ;

/*-------------------------------------- �������� ����������� ������ */

  if(!stricmp(action, "CLEAR_TRACE")) {

     if(this->mTrace_dlist)                                         /* ����������� ���������� ������ */                                       
             RSS_Kernel::display.ReleaseList(this->mTrace_dlist) ;

                                               return ;
                                      }
/*-------------------------------- �������������� ����������� ������ */

     if(mTrace_dlist==0) {
           mTrace_dlist=RSS_Kernel::display.GetList(2) ;
                         }

     if(mTrace_dlist==0)  return ;

/*----------------------------------- ������� ���������� ����������� */

          status=RSS_Kernel::display.SetFirstContext("Show") ;
    while(status==0) {                                              /* CIRCLE.1 */

/*---------------------------------- ��������� ��������� ����������� */

             glNewList(mTrace_dlist, GL_COMPILE) ;                  /* �������� ������ */
          glMatrixMode(GL_MODELVIEW) ;

/*--------------------------------------------- ��������� ���������� */

                                        i=0 ;

             glColor4d(GetRValue(mTrace[i].color)/256., 
                       GetGValue(mTrace[i].color)/256.,
                       GetBValue(mTrace[i].color)/256., 1.) ;

           glLineWidth(mTrace_width) ;

               glBegin(GL_LINE_STRIP) ;

            glVertex3d(mTrace[i].x_base, mTrace[i].y_base, mTrace[i].z_base) ;

       for(i=1 ; i<mTrace_cnt ; i++) {

         if(mTrace[i].color!=mTrace[i-1].color) {

            glVertex3d(mTrace[i].x_base, mTrace[i].y_base, mTrace[i].z_base) ;
                 glEnd();
             glColor4d(GetRValue(mTrace[i].color)/256., 
                       GetGValue(mTrace[i].color)/256.,
                       GetBValue(mTrace[i].color)/256., 1.) ;

               glBegin(GL_LINE_STRIP) ;
                                                }

            glVertex3d(mTrace[i].x_base, mTrace[i].y_base, mTrace[i].z_base) ;

                                     }

                 glEnd();

                 glLineWidth(1.0f) ;

/*----------------------------- �������������� ��������� ����������� */

             glEndList() ;                                          /* �������� ������ */

/*----------------------------------- ������� ���������� ����������� */

          status=RSS_Kernel::display.SetNextContext("Show") ;
                     }                                              /* CONTINUE.1 */
/*-------------------------------------------------------------------*/

}


/*********************************************************************/
/*								     */
/*	      ���������� ������ "������� ���������"	             */
/*								     */
/*********************************************************************/

/*********************************************************************/
/*								     */
/*	       ����������� ������ "������� ���������"      	     */

     RSS_Transit_RocketLego::RSS_Transit_RocketLego(void)

{
}


/*********************************************************************/
/*								     */
/*	        ���������� ������ "������� ���������"      	     */

    RSS_Transit_RocketLego::~RSS_Transit_RocketLego(void)

{
}


/********************************************************************/
/*								    */
/*	              ���������� ��������                           */

    int  RSS_Transit_RocketLego::vExecute(void)

{
   if(!stricmp(action, "SHOW_TRACE" ))  ((RSS_Object_RocketLego *)object)->iShowTrace(action) ;
   if(!stricmp(action, "CLEAR_TRACE"))  ((RSS_Object_RocketLego *)object)->iShowTrace(action) ;

   return(0) ;
}

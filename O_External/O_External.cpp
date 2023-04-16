/********************************************************************/
/*                                                                  */
/*           ������ ���������� �������� "������� ������"            */
/*                                                                  */
/********************************************************************/

#include <windows.h>
#include <winsock.h>
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
#include "..\RSS_Kernel\RSS_Kernel.h"
#include "..\RSS_Model\RSS_Model.h"
#include "..\F_Show\F_Show.h"
#include "..\F_Hit\F_Hit.h"

#include "..\tcp_lib\exh.h"
#include "..\tcp_lib\tcp.h"

#include "O_External.h"

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
/*                                                                  */
/*                       ����������� ������                         */

     static   RSS_Module_External  ProgramModule ;


/********************************************************************/
/*                                                                  */
/*                     ����������������� ����                       */

 O_EXTERNAL_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 O_EXTERNAL_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/********************************************************************/
/**                                                                **/
/**   �������� ������ ������ ���������� �������� "������� ������"  **/
/**                                                                **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*                                                                  */
/*                            ������ ������                         */

  struct RSS_Module_External_instr  RSS_Module_External_InstrList[]={

 { "help",    "?",  "#HELP   - ������ ��������� ������", 
                     NULL,
                    &RSS_Module_External::cHelp   },
 { "create",  "cr", "#CREATE - ������� ������",
                    " CREATE <���> [<������> [<������ ����������>]]\n"
                    "   ������� ����������� ������ �� ����������������� ������",
                    &RSS_Module_External::cCreate },
 { "info",    "i",  "#INFO - ������ ���������� �� �������",
                    " INFO <���> \n"
                    "   ������ �������� ��������� �� ������� � ������� ����\n"
                    " INFO/ <���> \n"
                    "   ������ ������ ���������� �� ������� � ��������� ����",
                    &RSS_Module_External::cInfo },
 { "copy",    "cp", "#COPY - ���������� ������",
                    " COPY <��� �������> <��� ������ �������>\n"
                    "   ���������� ������",
                    &RSS_Module_External::cCopy },
 { "owner",   "o",  "#OWNER - ��������� �������� ������",
                    " OWNER <���> <��������>\n"
                    "   ��������� ������ - �������� ������",
                    &RSS_Module_External::cOwner },
 { "target",  "tg", "#TARGET - ��������� ���� ������",
                    " TARGET <���> <����>\n"
                    "   ��������� ������ - ���� ������",
                    &RSS_Module_External::cTarget },
 { "base",    "b", "#BASE - ������ ������� ����� �������",
                    " BASE <���> <x> <y> <z>\n"
                    "   ������ ������� ����� �������\n"
                    " BASE/x <���> <x>\n"
                    "   ������ ���������� X ������� ����� �������\n"
                    "       (���������� ��� Y � Z)\n"
                    " BASE/+x <���> <x>\n"
                    "   ������ ���������� ���������� X ������� ����� �������\n"
                    "       (���������� ��� Y � Z)\n"
                    " BASE> <���>\n"
                    "   ������ ������������ ���������� ������� ������ �������\n",
                    &RSS_Module_External::cBase },
 { "angle",   "a", "#ANGLE - ������ ���� ���������� �������",
                    "           A (AZIMUTH)   - ������\n"
                    "           E (ELEVATION) - ����������\n"
                    "           R (ROLL)      - ����\n"
                    " ANGLE <���> <a> <e> <r>\n"
                    "   ������ ���� ���������� �������\n"
                    " ANGLE/a <���> <a>\n"
                    "   ������ ���� ���������� A �������\n"
                    "       (���������� ��� E � R)\n"
                    " ANGLE/+a <���> <a>\n"
                    "   ������ ���������� ���� ���������� A �������\n"
                    "       (���������� ��� E � R)\n"
                    " ANGLE> <���>\n"
                    "   ������ ������������ ���������� ������ ���������� �������\n",
                    &RSS_Module_External::cAngle },
 { "velocity", "v", "#VELOCITY - ������� �������� �������� �������",
                    " VELOCITY <���> <��������>\n"
                    "   ������� ����������� �������� �������� �������"
                    " VELOCITY <���> <X-��������> <Y-��������> <Z-��������>\n"
                    "   ������� �������� �������� �������",
                    &RSS_Module_External::cVelocity },
 { "control",  "c", "#CONTROL - ���������� ��������",
                    " CONTROL <���> <���� �����> [<����������>]\n"
                    "   ������� ������� ������ ���������� ����������\n"
                    " CONTROL/G <���> <����������>\n"
                    "   ������� ������� ������ ���������� ����������\n"
                    " CONTROL> <���> [<��� �����> [<��� ���������>]]\n"
                    "   ���������� �����������\n",
                    &RSS_Module_External::cControl },
 { "trace",    "t", "#TRACE - ����������� ���������� �������",
                    " TRACE <���> [<������������>]\n"
                    "   ����������� ���������� ������� � �������� �������\n",
                    &RSS_Module_External::cTrace },
 { "itype",   "it", "#ITYPE - ������� ���� ���������� � ������� �������",
                    " ITYPE <���>\n"
                    "   ������ ��������� ���������� � ���������� ������\n"
                    " ITYPE <���> <iface_type> [<object_type>]\n"
                    "   ������ ��� ���������� (file, tcp-server) � ��� �������\n",
                    &RSS_Module_External::cIType },
 { "ifile",   "if", "#IFILE - ��������� ��������� ���������� � ������� �������",
                    " IFILE <���>\n"
                    "   ������ ��������� � ���������� ������\n"
                    " IFILE/f <���> <path>\n"
                    "   ������ ���� ����� �������� ������\n"
                    " IFILE/t <���> <path>\n"
                    "   ������ ��� ����� ������ �������� �����\n"
                    " IFILE/c <���> <name>\n"
                    "   ������ ��� ��� ��������/��������� ������ ������\n",
                    &RSS_Module_External::cIFile },
 { "itcp",    "ic", "#ITCP - ��������� �������� ���������� � ������� �������",
                    " ITCP <���>\n"
                    "   ������ ��������� � ���������� ������\n"
                    " ITCP/u <���> <URL>\n"
                    "   ������ URL �������� ������\n",
                    &RSS_Module_External::cITcp },
 {  NULL }
                                                            } ;

/********************************************************************/
/*                                                                  */
/*                       ����� ����� ������                         */

    struct RSS_Module_External_instr *RSS_Module_External::mInstrList=NULL ;


/********************************************************************/
/*                                                                  */
/*                        ����������� ������                        */

     RSS_Module_External::RSS_Module_External(void)

{
	   keyword="EmiStand" ;
    identification="External" ;
          category="Object" ;

        mInstrList=RSS_Module_External_InstrList ;

        a_step=15. ;
        g_step= 1. ;
}


/********************************************************************/
/*                                                                  */
/*                         ���������� ������                        */

    RSS_Module_External::~RSS_Module_External(void)

{
}


/********************************************************************/
/*                                                                  */
/*                        �������� �������                          */

  RSS_Object *RSS_Module_External::vCreateObject(RSS_Model_data *data)

{
  RSS_Object_External *object ;
                 char  models_list[4096] ;
                 char *end ;
                  int  i ;
                  int  j ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

#define       PAR             object->Parameters
#define       PAR_CNT         object->Parameters_cnt
 
/*--------------------------------------------------- �������� ����� */

    if(data->name[0]==0) {                                           /* ���� ��� �� ������ */
              SEND_ERROR("������ EXTERNAL: �� ������ ��� �������") ;
                                return(NULL) ;
                         }

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, data->name)) {
              SEND_ERROR("������ EXTERNAL: ������ � ����� ������ ��� ����������") ;
                                return(NULL) ;
                                                    }
/*-------------------------------------- ���������� �������� ������� */
/*- - - - - - - - - - - - ���� ������ ������ ��������� � ����������� */
   if(data->path[0]==0) {

    if(data->model[0]==0) {                                         /* ���� ������ �� ������ */
              SEND_ERROR("������ EXTERNAL: �� ������ ������ �������") ;
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
              SEND_ERROR("������ EXTERNAL: ����������� ������ ����") ;
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

              SEND_ERROR("������ EXTERNAL: �������������� ����� ���������� ������") ;
                                return(NULL) ;
                                             }
/*------------------------------------------------- �������� ������� */

       object=new RSS_Object_External ;
    if(object==NULL) {
              SEND_ERROR("������ EXTERNAL: ������������ ������ ��� �������� �������") ;
                        return(NULL) ;
                     }
/*------------------------------------- ���������� ������ ���������� */
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������� ��������� */
     for(i=0 ; i<5 ; i++)
       if(data->pars[i].text[0]!=0) {

           PAR=(struct RSS_Parameter *)
                 realloc(PAR, (PAR_CNT+1)*sizeof(*PAR)) ;
        if(PAR==NULL) {
                         SEND_ERROR("������ EXTERNAL: ������������ ������") ;
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
                         SEND_ERROR("������ EXTERNAL: ������������ ������") ;
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
                                         data->sections[i].decl, "External.Body") ;
                                             }

                                         data->sections[i].title[0]= 0 ;
                                        *data->sections[i].decl    ="" ;
                                               }
/*---------------------------------- �������� ������� � ����� ������ */

       OBJECTS=(RSS_Object **)
                 realloc(OBJECTS, (OBJECTS_CNT+1)*sizeof(*OBJECTS)) ;
    if(OBJECTS==NULL) {
              SEND_ERROR("������ EXTERNAL: ������������ ������") ;
                                return(NULL) ;
                      }

              OBJECTS[OBJECTS_CNT]=object ;
                      OBJECTS_CNT++ ;

       strcpy(object->Name,       data->name) ;
       strcpy(object->model_path, data->path) ;
              object->Module=this ;

        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_DEFAULT_OBJECT, (LPARAM)data->name) ;

/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

#undef   PAR
#undef   PAR_CNT

  return(object) ;
}


/********************************************************************/
/*                                                                  */
/*                         �������� ��������                        */

     int  RSS_Module_External::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- �������� ������ */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s -  ������� ������", identification) ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*                                                                  */
/*                         ��������� �������                        */

  int  RSS_Module_External::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* ���� ������ ������ ������� */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "EXTERNAL"
#define  _SECTION_SHRT_NAME   "EXTERNAL"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Object External:") ;
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
       if(status)  SEND_ERROR("������ EXTERNAL: ����������� �������") ;
                                            return(-1) ;
                                       }
 
     if(mInstrList[i].process!=NULL)                                /* ���������� ������� */
                status=(this->*mInstrList[i].process)(end) ;
     else       status=  0 ;

/*-------------------------------------------------------------------*/

   return(status) ;
}


/********************************************************************/
/*                                                                  */
/*                     ������� ������ �� ������                     */

    void  RSS_Module_External::vReadSave(std::string *data)

{
                char *buff ;
                 int  buff_size ;
      RSS_Model_data  create_data ;
 RSS_Object_External *object ;
                char  name[128] ;
                char *entry ;
                char *end ;
                 int  i ;

/*----------------------------------------------- �������� ��������� */

   if(memicmp(data->c_str(), "#BEGIN MODULE EXTERNAL\n", 
                      strlen("#BEGIN MODULE EXTERNAL\n")) &&
      memicmp(data->c_str(), "#BEGIN OBJECT EXTERNAL\n", 
                      strlen("#BEGIN OBJECT EXTERNAL\n"))   )  return ;

/*------------------------------------------------ ���������� ������ */

              buff_size=(int)data->size()+16 ;
              buff     =(char *)calloc(1, buff_size) ;

       strcpy(buff, data->c_str()) ;

/*------------------------------------------------- �������� ������� */

   if(!memicmp(buff, "#BEGIN OBJECT EXTERNAL\n", 
              strlen("#BEGIN OBJECT EXTERNAL\n"))) {                /* IF.1 */
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
                object=(RSS_Object_External *)vCreateObject(&create_data) ;
             if(object==NULL)  return ;
/*- - - - - - - - - - - - ������� ������� ����� � ���������� ������� */
       entry=strstr(buff, "X_BASE=") ; object->x_base=atof(entry+strlen("X_BASE=")) ;
       entry=strstr(buff, "Y_BASE=") ; object->y_base=atof(entry+strlen("Y_BASE=")) ;
       entry=strstr(buff, "Z_BASE=") ; object->z_base=atof(entry+strlen("Z_BASE=")) ;
       entry=strstr(buff, "A_AZIM=") ; object->a_azim=atof(entry+strlen("A_AZIM=")) ;
       entry=strstr(buff, "A_ELEV=") ; object->a_elev=atof(entry+strlen("A_ELEV=")) ;
       entry=strstr(buff, "A_ROLL=") ; object->a_roll=atof(entry+strlen("A_ROLL=")) ;

   for(i=0 ; i<object->Features_cnt ; i++) {
        object->Features[i]->vBodyBasePoint(NULL, object->x_base, 
                                                  object->y_base, 
                                                  object->z_base ) ;
        object->Features[i]->vBodyAngles   (NULL, object->a_azim, 
                                                  object->a_elev, 
                                                  object->a_roll ) ;
                                           }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                 }                  /* END.1 */
/*-------------------------------------------- ������������ �������� */

                free(buff) ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*                                                                  */
/*                     �������� ������ � ������                     */

    void  RSS_Module_External::vWriteSave(std::string *text)

{
  std::string  buff ;

/*----------------------------------------------- ��������� �������� */

     *text="#BEGIN MODULE EXTERNAL\n" ;

/*------------------------------------------------ �������� �������� */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*                                                                  */
/*                    ���������� ���������� HELP                    */

  int  RSS_Module_External::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Object_External_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*                                                                  */
/*                 ���������� ���������� CREATE                     */
/*                                                                  */
/*      CREATE <���> [<������> [<������ ����������>]]               */

  int  RSS_Module_External::cCreate(char *cmd)

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
                                     Object_External_Create_dialog, 
                                    (LPARAM)&data               ) ;
   if(status)  return(-1) ;

            this->kernel->vShow(NULL) ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*                                                                  */
/*                    ���������� ���������� INFO                    */
/*                                                                  */
/*        INFO   <���>                                              */
/*        INFO/  <���>                                              */

  int  RSS_Module_External::cInfo(char *cmd)

{
                char  *name ;
 RSS_Object_External  *object ;
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

       object=(RSS_Object_External *)FindObject(name, 1) ;          /* ���� ������ �� ����� */
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
                    "G-ctrl % 8.2lf\r\n" 
                    "Owner  %s\r\n" 
                    "Target %s\r\n" 
                    "\r\n",
                        object->Name, object->Type, 
                        object->x_base, object->y_base, object->z_base,
                        object->a_azim, object->a_elev, object->a_roll,
                        object->v_abs,
                        object->x_velocity, object->y_velocity, object->z_velocity,
                        object->g_ctrl, object->owner, object->target
                    ) ;

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

  int  RSS_Module_External::cCopy(char *cmd)

{
#define   _PARS_MAX  10

                  char  *pars[_PARS_MAX] ;
                  char  *name ;
                  char  *copy ;
   RSS_Object_External  *sample ;
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

       sample=(RSS_Object_External *)FindObject(name, 1) ;        /* ���� ������-���� �� ����� */
    if(sample==NULL)  return(-1) ;

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

            object=sample->vCopy(copy) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*                                                                  */
/*                   ���������� ���������� OWNER                    */
/*                                                                  */
/*       OWNER <���> <��������>                                     */

  int  RSS_Module_External::cOwner(char *cmd)

{
#define   _PARS_MAX  10

                char  *pars[_PARS_MAX] ;
                char  *name ;
                char  *owner ;
 RSS_Object_External  *e_object ;
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

       e_object=(RSS_Object_External *)FindObject(name, 1) ;        /* ���� ������-���� �� ����� */
    if(e_object==NULL)  return(-1) ;

/*------------------------------------------ �������� ����� �������� */

    if(owner==NULL) {                                               /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� �������-��������. \n"
                                 "��������: OWNER <���_������> <���_��������>") ;
                                     return(-1) ;
                    }

       object=FindObject(owner, 0) ;                                /* ���� ������-�������� �� ����� */
    if(object==NULL)  return(-1) ;

/*------------------------------------------------- ������� �������� */

          strcpy(e_object->owner, owner) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*                                                                  */
/*                   ���������� ���������� TARGET                   */
/*                                                                  */
/*       TARGET <���> <����>                                        */

  int  RSS_Module_External::cTarget(char *cmd)

{
#define   _PARS_MAX  10

                char  *pars[_PARS_MAX] ;
                char  *name ;
                char  *target ;
 RSS_Object_External  *e_object ;
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
                   target=pars[1] ;   

/*------------------------------------------- �������� ����� ������� */

    if(name==NULL) {                                                /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� �������. \n"
                                 "��������: TARGET <���_�������> ...") ;
                                     return(-1) ;
                   }

       e_object=(RSS_Object_External *)FindObject(name, 1) ;        /* ���� ������-���� �� ����� */
    if(e_object==NULL)  return(-1) ;

/*---------------------------------------------- �������� ����� ���� */

    if(target==NULL) {                                              /* ���� ��� �� ������... */
                        SEND_ERROR("�� ������ ��� �������-����. \n"
                                   "��������: TARGET <���_������> <���_����>") ;
                                     return(-1) ;
                    }

       object=FindObject(target, 0) ;                               /* ���� ������-���� �� ����� */
    if(object==NULL)  return(-1) ;

/*----------------------------------------------------- ������� ���� */

               strcpy(e_object->target, target) ;

   for(i=0 ; i<e_object->Features_cnt ; i++)
     if(!stricmp(e_object->Features[i]->Type, "Hit")) 
         strcpy(((RSS_Feature_Hit *)(e_object->Features[i]))->target, target) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*                                                                  */
/*                    ���������� ���������� BASE                    */
/*                                                                  */
/*        BASE    <���> <X> <Y> <Z>                                 */
/*        BASE/X  <���> <X>                                         */
/*        BASE/+X <���> <X>                                         */
/*        BASE>   <���> <��� ���������> <���>                       */
/*        BASE>>  <���> <��� ���������> <���>                       */

  int  RSS_Module_External::cBase(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

                char  *pars[_PARS_MAX] ;
                char  *name ;
                char **xyz ;
              double   coord[_COORD_MAX] ;
                 int   coord_cnt ;
              double   inverse ;
 RSS_Object_External  *object ;
                 int   xyz_flag ;          /* ���� ������ ����� ���������� */
                 int   delta_flag ;        /* ���� ������ ���������� */
                 int   arrow_flag ;        /* ���� ����������� ������ */
                char  *arrows ;
                char  *error ;
                char  *end ;
                 int   i ;

/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
                        xyz_flag=0 ;
                      delta_flag=0 ;
                      arrow_flag=0 ;

       if(*cmd=='/' ||
          *cmd=='+'   ) {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("������������ ������ �������") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, '+')!=NULL   )  delta_flag= 1 ;

                if(strchr(cmd, 'x')!=NULL ||
                   strchr(cmd, 'X')!=NULL   )    xyz_flag='X' ;
           else if(strchr(cmd, 'y')!=NULL ||
                   strchr(cmd, 'Y')!=NULL   )    xyz_flag='Y' ;
           else if(strchr(cmd, 'z')!=NULL ||
                   strchr(cmd, 'Z')!=NULL   )    xyz_flag='Z' ;

                           cmd=end+1 ;
                        }

  else if(*cmd=='>') {
                           delta_flag=1 ;
                           arrow_flag=1 ;

                          cmd=strchr(cmd, ' ') ;
                       if(cmd==NULL)  return(0) ;
                          cmd++ ;
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
                  inverse=   1. ; 

/*------------------------------------- ��������� ����������� ������ */

    if(arrow_flag) {                        
                         arrows=pars[1] ;

      if(strstr(arrows, "left" )!=NULL   ) {  xyz_flag='X' ;  inverse=-1. ;  }
      if(strstr(arrows, "right")!=NULL   ) {  xyz_flag='X' ;  inverse= 1. ;  }  
      if(strstr(arrows, "up"   )!=NULL &&
         strstr(arrows, "ctrl" )==NULL   ) {  xyz_flag='Y' ;  inverse= 1. ;  }
      if(strstr(arrows, "down" )!=NULL &&
         strstr(arrows, "ctrl" )==NULL   ) {  xyz_flag='Y' ;  inverse=-1. ;  }
      if(strstr(arrows, "up"   )!=NULL &&
         strstr(arrows, "ctrl" )!=NULL   ) {  xyz_flag='Z' ;  inverse= 1. ;  }
      if(strstr(arrows, "down" )!=NULL &&
         strstr(arrows, "ctrl" )!=NULL   ) {  xyz_flag='Z' ;  inverse=-1. ;  }
           
                          xyz=&pars[2] ;
                   }
/*------------------------------------------- �������� ����� ������� */

    if(name==NULL) {                                                /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� �������. \n"
                                 "��������: BASE <���_�������> ...") ;
                                     return(-1) ;
                   }

       object=(RSS_Object_External *)FindObject(name, 1) ;          /* ���� ������ �� ����� */
    if(object==NULL)  return(-1) ;

/*------------------------------------------------- ������ ��������� */

    for(i=0 ; xyz[i]!=NULL && i<_COORD_MAX ; i++) {

             coord[i]=strtod(xyz[i], &end) ;
        if(*end!=0) {  
                       SEND_ERROR("������������ �������� ����������") ;
                                       return(-1) ;
                    }
                                                  }

                             coord_cnt=  i ;

                                 error= NULL ;
      if(xyz_flag) {
               if(coord_cnt==0)  error="�� ������� ���������� ��� �� ����������" ;
                   }
      else         {
               if(coord_cnt <3)  error="������ ���� ������� 3 ����������" ;
                   }

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

      if(arrow_flag && coord_cnt>1)  coord[0]=coord[coord_cnt-1] ;  /* ��� ����������� ������ ����� � �������� ���� */
                                                                    /*  ����� ��������� ��������                    */
/*------------------------------------------------ ������� ��������� */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - ���������� */
   if(delta_flag) {

          if(xyz_flag=='X')   object->x_base+=inverse*coord[0] ;
     else if(xyz_flag=='Y')   object->y_base+=inverse*coord[0] ;                 
     else if(xyz_flag=='Z')   object->z_base+=inverse*coord[0] ;
                  }
/*- - - - - - - - - - - - - - - - - - - - - - -  ���������� �������� */
   else           {

          if(xyz_flag=='X')   object->x_base=coord[0] ;
     else if(xyz_flag=='Y')   object->y_base=coord[0] ;                 
     else if(xyz_flag=='Z')   object->z_base=coord[0] ;
     else                   {
                              object->x_base=coord[0] ;
                              object->y_base=coord[1] ;
                              object->z_base=coord[2] ;
                            }
                  }
/*---------------------------------------------- ������� �� �������� */

   for(i=0 ; i<object->Features_cnt ; i++)
     object->Features[i]->vBodyBasePoint(NULL, object->x_base, 
                                               object->y_base, 
                                               object->z_base ) ;

/*------------------------------------------------------ ����������� */

                      this->kernel->vShow(NULL) ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*                                                                  */
/*                    ���������� ���������� ANGLE                   */
/*                                                                  */
/*       ANGLE    <���> <A> <E> <R>                                 */
/*       ANGLE/A  <���> <A>                                         */
/*       ANGLE/+A <���> <A>                                         */
/*       ANGLE>   <���> <��� ���������> <���>                       */
/*       ANGLE>>  <���> <��� ���������> <���>                       */

  int  RSS_Module_External::cAngle(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

                char  *pars[_PARS_MAX] ;
                char  *name ;
                char **xyz ;
              double   coord[_COORD_MAX] ;
                 int   coord_cnt ;
              double   inverse ;
 RSS_Object_External  *object ;
                 int   xyz_flag ;          /* ���� ������ ����� ���������� */
                 int   delta_flag ;        /* ���� ������ ���������� */
                 int   arrow_flag ;        /* ���� ����������� ������ */
                char  *arrows ;
            Matrix2d  Sum_Matrix ;
            Matrix2d  Oper_Matrix ;  
            Matrix2d  Velo_Matrix ;  
                char  *error ;
                char  *end ;
                 int   i ;

/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
                        xyz_flag=0 ;
                      delta_flag=0 ;
                      arrow_flag=0 ;

       if(*cmd=='/' ||
          *cmd=='+'   ) {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("������������ ������ �������") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, '+')!=NULL   )  delta_flag= 1 ;

                if(strchr(cmd, 'a')!=NULL ||
                   strchr(cmd, 'A')!=NULL   )    xyz_flag='A' ;
           else if(strchr(cmd, 'e')!=NULL ||
                   strchr(cmd, 'E')!=NULL   )    xyz_flag='E' ;
           else if(strchr(cmd, 'r')!=NULL ||
                   strchr(cmd, 'R')!=NULL   )    xyz_flag='R' ;

                           cmd=end+1 ;
                        }

  else if(*cmd=='>') {
                           delta_flag=1 ;
                           arrow_flag=1 ;

                          cmd=strchr(cmd, ' ') ;
                       if(cmd==NULL)  return(0) ;
                          cmd++ ;
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
                  inverse=   1. ; 

/*------------------------------------- ��������� ����������� ������ */

    if(arrow_flag) {                        
                         arrows=pars[1] ;

      if(strstr(arrows, "left" )!=NULL &&
         strstr(arrows, "ctrl" )==NULL   ) {  xyz_flag='A' ;  inverse=-1. ;  }
      if(strstr(arrows, "right")!=NULL &&  
         strstr(arrows, "ctrl" )==NULL   ) {  xyz_flag='A' ;  inverse= 1. ;  }  

      if(strstr(arrows, "up"   )!=NULL   ) {  xyz_flag='E' ;  inverse= 1. ;  }
      if(strstr(arrows, "down" )!=NULL   ) {  xyz_flag='E' ;  inverse=-1. ;  }

      if(strstr(arrows, "left" )!=NULL &&
         strstr(arrows, "ctrl" )!=NULL   ) {  xyz_flag='R' ;  inverse=-1. ;  }
      if(strstr(arrows, "right")!=NULL &&  
         strstr(arrows, "ctrl" )!=NULL   ) {  xyz_flag='R' ;  inverse= 1. ;  }  
           
                          xyz=&pars[2] ;   
                   }
/*------------------------------------------- �������� ����� ������� */

    if(name==NULL) {                                                /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� �������. \n"
                                 "��������: ANGLE <���_�������> ...") ;
                                     return(-1) ;
                   }

       object=(RSS_Object_External *)FindObject(name, 1) ;          /* ���� ������ �� ����� */
    if(object==NULL)  return(-1) ;

/*------------------------------------------------- ������ ��������� */

    for(i=0 ; xyz[i]!=NULL && i<_COORD_MAX ; i++) {

             coord[i]=strtod(xyz[i], &end) ;
        if(*end!=0) {  
                       SEND_ERROR("������������ �������� ����������") ;
                                       return(-1) ;
                    }
                                                  }

                             coord_cnt=  i ;

                                 error= NULL ;
      if(xyz_flag) {
               if(coord_cnt==0)  error="�� ������� ���������� ��� �� ����������" ;
                   }
      else         {
               if(coord_cnt <3)  error="������ ���� ������� 3 ����������" ;
                   }

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

      if(arrow_flag && coord_cnt>1)  coord[0]=coord[coord_cnt-1] ;  /* ��� ����������� ������ ����� � �������� ���� */
                                                                    /*  ����� ��������� ��������                    */
/*------------------------------------------------ ������� ��������� */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - ���������� */
   if(delta_flag) {

          if(xyz_flag=='A')   object->a_azim+=inverse*coord[0] ;
     else if(xyz_flag=='E')   object->a_elev+=inverse*coord[0] ;                 
     else if(xyz_flag=='R')   object->a_roll+=inverse*coord[0] ;
                  }
/*- - - - - - - - - - - - - - - - - - - - - - -  ���������� �������� */
   else           {

          if(xyz_flag=='A')   object->a_azim=coord[0] ;
     else if(xyz_flag=='E')   object->a_elev=coord[0] ;                 
     else if(xyz_flag=='R')   object->a_roll=coord[0] ;
     else                   {
                              object->a_azim=coord[0] ;
                              object->a_elev=coord[1] ;
                              object->a_roll=coord[2] ;
                            }
                  }
/*- - - - - - - - - - - - - - - - - - - - - -  ������������ �������� */
     while(object->a_azim> 180.)  object->a_azim-=360. ;
     while(object->a_azim<-180.)  object->a_azim+=360. ;

     while(object->a_elev> 180.)  object->a_elev-=360. ;
     while(object->a_elev<-180.)  object->a_elev+=360. ;

     while(object->a_roll> 180.)  object->a_roll-=360. ;
     while(object->a_roll<-180.)  object->a_roll+=360. ;

/*---------------------------------------------- ���������� �������� */

       Velo_Matrix.LoadZero   (3, 1) ;
       Velo_Matrix.SetCell    (2, 0, object->v_abs) ;
        Sum_Matrix.Load3d_azim(-object->a_azim) ;
       Oper_Matrix.Load3d_elev(-object->a_elev) ;
        Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
       Velo_Matrix.LoadMul    (&Sum_Matrix, &Velo_Matrix) ;

         object->x_velocity=Velo_Matrix.GetCell(0, 0) ;
         object->y_velocity=Velo_Matrix.GetCell(1, 0) ;
         object->z_velocity=Velo_Matrix.GetCell(2, 0) ;

/*---------------------------------------------- ������� �� �������� */

   for(i=0 ; i<object->Features_cnt ; i++)
     object->Features[i]->vBodyAngles(NULL, object->a_azim, 
                                            object->a_elev, 
                                            object->a_roll ) ;

/*------------------------------------------------------ ����������� */

                this->kernel->vShow(NULL) ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*                                                                  */
/*                    ���������� ���������� VELOCITY                */
/*                                                                  */
/*       VELOCITY <���> <��������>                                  */
/*       VELOCITY <���> <X-��������> <Y-��������> <Z-��������>      */

  int  RSS_Module_External::cVelocity(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

                char  *pars[_PARS_MAX] ;
                char  *name ;
                char **xyz ;
              double   coord[_COORD_MAX] ;
                 int   coord_cnt ;
 RSS_Object_External  *object ;
                char  *error ;
            Matrix2d  Sum_Matrix ;
            Matrix2d  Oper_Matrix ;  
            Matrix2d  Velo_Matrix ;  
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

/*------------------------------------------- �������� ����� ������� */

    if(name==NULL) {                                                /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� �������. \n"
                                 "��������: VELOCITY <���_�������> ...") ;
                                     return(-1) ;
                   }

       object=(RSS_Object_External *)FindObject(name, 1) ;          /* ���� ������ �� ����� */
    if(object==NULL)  return(-1) ;

/*------------------------------------------------- ������ ��������� */

    for(i=0 ; xyz[i]!=NULL && i<_COORD_MAX ; i++) {

             coord[i]=strtod(xyz[i], &end) ;
        if(*end!=0) {  
                       SEND_ERROR("������������ �������� ��������") ;
                                       return(-1) ;
                    }
                                                  }

                             coord_cnt=i ;

                        error= NULL ;
      if(coord_cnt==0)  error="�� ������� �������� ��� �� ��������" ;
      if(coord_cnt==2)  error="������ ���� ������� 3 �������� ��������" ;

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

/*------------------------------------------------- ������� �������� */
/*- - - - - - - - - - - - - - - - - - - ���������� �������� �������� */
   if(coord_cnt==1) {
                          object->v_abs=coord[0] ;

                Velo_Matrix.LoadZero   (3, 1) ;
                Velo_Matrix.SetCell    (2, 0, object->v_abs) ;

                 Sum_Matrix.Load3d_azim(-object->a_azim) ;
                Oper_Matrix.Load3d_elev(-object->a_elev) ;
                 Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
                Velo_Matrix.LoadMul    (&Sum_Matrix, &Velo_Matrix) ;

                       object->x_velocity=Velo_Matrix.GetCell(0, 0) ;
                       object->y_velocity=Velo_Matrix.GetCell(1, 0) ;
                       object->z_velocity=Velo_Matrix.GetCell(2, 0) ;
                    }
/*- - - - - - - - - - - - - - - - - - - - - - - -  �������� �������� */
   else             {
                       object->x_velocity=coord[0] ;
                       object->y_velocity=coord[1] ;
                       object->z_velocity=coord[2] ;

                       object->v_abs     =sqrt(object->x_velocity*object->x_velocity+
                                               object->y_velocity*object->y_velocity+
                                               object->z_velocity*object->z_velocity ) ;
                    }
/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*                                                                  */
/*                 ���������� ���������� CONTROL                    */
/*                                                                  */
/*       CONTROL   <���> <���� �����> [<����������>]                */
/*       CONTROL/G <���> <����������>                               */
/*       CONTROL>  <���> <��� ���������>                            */
/*       CONTROL>> <���> <��� ���������>                            */

  int  RSS_Module_External::cControl(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

                char  *pars[_PARS_MAX] ;
                char  *name ;
                char **xyz ;
              double   coord[_COORD_MAX] ;
                 int   coord_cnt ;
              double   inverse ;
 RSS_Object_External  *object ;
                 int   xyz_flag ;          /* ���� ������ ����� ���������� */
                 int   g_flag ;            /* ���� ������� ���������� */
                 int   s_flag ;            /* ���� ������� ����� ��������� */
                 int   arrow_flag ;        /* ���� ����������� ������ */
                char  *arrows ;
                char  *error ;
                char  *end ;
                 int   i ;

/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
                        xyz_flag=0 ;
                          g_flag=0 ;
                          s_flag=0 ;
                      arrow_flag=0 ;

       if(*cmd=='/') {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("������������ ������ �������") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, 'g')!=NULL ||
                   strchr(cmd, 'G')!=NULL   )  g_flag=1 ;

                           cmd=end+1 ;
                     }

  else if(*cmd=='>') {
                           arrow_flag=1 ;

                          cmd=strchr(cmd, ' ') ;
                       if(cmd==NULL)  return(0) ;
                          cmd++ ;
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
                  inverse=   1. ; 

/*------------------------------------- ��������� ����������� ������ */

    if(arrow_flag) {                        
                         arrows=pars[1] ;

      if(strstr(arrows, "left" )!=NULL) {  xyz_flag='R' ;  inverse= 1. ;  }
      if(strstr(arrows, "right")!=NULL) {  xyz_flag='R' ;  inverse=-1. ;  }  

      if(strstr(arrows, "up"   )!=NULL) {  xyz_flag='G' ;  inverse=-1. ;  }
      if(strstr(arrows, "down" )!=NULL) {  xyz_flag='G' ;  inverse= 1. ;  }
           
                          xyz=&pars[2] ;   
                   }
/*------------------------------------------- �������� ����� ������� */

    if(name==NULL) {                                                /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� �������. \n"
                                 "��������: CONTROL <���_�������> ...") ;
                                     return(-1) ;
                   }

       object=(RSS_Object_External *)FindObject(name, 1) ;          /* ���� ������ �� ����� */
    if(object==NULL)  return(-1) ;

/*------------------------------------------------- ������ ��������� */

    for(i=0 ; xyz[i]!=NULL && i<_COORD_MAX ; i++) {

             coord[i]=strtod(xyz[i], &end) ;
        if(*end!=0) {  
                       SEND_ERROR("������������ �������� ����������") ;
                                       return(-1) ;
                    }
                                                  }

                             coord_cnt=  i ;

                        error= NULL ;
      if(coord_cnt==0)  error="�� ������� ���������� ��� �� ����������" ;

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

/*----------------------------------- ������� ����������� ���������� */

   if(g_flag) {
                  object->g_ctrl=coord[0] ;

                      return(0) ;
              }
/*------------------------------------------------ ������� ��������� */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - ���������� */
   if(arrow_flag) {

          if(xyz_flag=='G')   object->g_ctrl+=inverse*this->g_step ;
     else if(xyz_flag=='R')   object->a_roll+=inverse*this->a_step ;
                  }
/*- - - - - - - - - - - - - - - - - - - - - - -  ���������� �������� */
   else           {
                               object->a_roll=coord[0] ;
              if(coord_cnt>1)  object->g_ctrl=coord[1] ;
                  }
/*- - - - - - - - - - - - - - - - - - - - - -  ������������ �������� */
     while(object->a_roll> 180.)  object->a_roll-=360. ;
     while(object->a_roll<-180.)  object->a_roll+=360. ;

/*---------------------------------------------- ������� �� �������� */

   for(i=0 ; i<object->Features_cnt ; i++)
     object->Features[i]->vBodyAngles(NULL, object->a_azim, 
                                            object->a_elev, 
                                            object->a_roll ) ;

/*------------------------------------------------------ ����������� */

                this->kernel->vShow(NULL) ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*                                                                  */
/*                   ���������� ���������� TRACE                    */
/*                                                                  */
/*       TRACE <���> [<������������>]                               */

  int  RSS_Module_External::cTrace(char *cmd)

{
#define   _PARS_MAX  10

                char *pars[_PARS_MAX] ;
                char *name ;
              double  trace_time ;
              double  time_0 ;        /* ��������� ����� ������� */ 
              double  time_1 ;        /* ������� ����� */ 
              double  time_c ;        /* ���������� ����� ������� */ 
              double  time_s ;        /* ��������� ����� ��������� */ 
              double  time_w ;        /* ����� �������� */ 
 RSS_Object_External *object ;
            Matrix2d  Sum_Matrix ;
            Matrix2d  Oper_Matrix ;  
            Matrix2d  Velo_Matrix ;  
                char *end ;
                 int  i ;

/*---------------------------------------- �������� ��������� ������ */

                     trace_time=0. ;
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
           else                  {
                                       trace_time=60. ;
                                         SEND_ERROR("����� ����������� - 60 ������") ;
                                 }
/*------------------------------------------- �������� ����� ������� */

    if(name==NULL) {                                                /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� �������. \n"
                                 "��������: TRACE <���_�������> ...") ;
                                     return(-1) ;
                   }

       object=(RSS_Object_External *)FindObject(name, 1) ;          /* ���� ������ �� ����� */
    if(object==NULL)  return(-1) ;

/*----------------------------------------- �������� �������� � ���� */

       object->o_owner=FindObject(object->owner, 0) ;               /* ���� �������� �� ����� */
    if(object->o_owner==NULL)  return(-1) ;

  if(object->target[0]!=0) {
       object->o_target=FindObject(object->target, 0) ;             /* ���� ���� �� ����� */
    if(object->o_target==NULL)  return(-1) ;
                           }
/*------------------------------ �������� ��������� ����� � �������� */

       object->x_base=object->o_owner->x_base ;
       object->y_base=object->o_owner->y_base ;
       object->z_base=object->o_owner->z_base ;

       object->a_azim=object->o_owner->a_azim ;
       object->a_elev=object->o_owner->a_elev ;
       object->a_roll=object->o_owner->a_roll ;

       Velo_Matrix.LoadZero   (3, 1) ;
       Velo_Matrix.SetCell    (2, 0, object->v_abs) ;
        Sum_Matrix.Load3d_azim(-object->a_azim) ;
       Oper_Matrix.Load3d_elev(-object->a_elev) ;
        Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
       Velo_Matrix.LoadMul    (&Sum_Matrix, &Velo_Matrix) ;

         object->x_velocity=Velo_Matrix.GetCell(0, 0) ;
         object->y_velocity=Velo_Matrix.GetCell(1, 0) ;
         object->z_velocity=Velo_Matrix.GetCell(2, 0) ;

/*------------------------------------------------------ ����������� */

              time_0=this->kernel->vGetTime() ;
              time_c=0. ;
              time_s=0. ;

         object->iSaveTracePoint("CLEAR") ;

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
         object->vCalculate(time_c-RSS_Kernel::calc_time_step, time_c, NULL, 0) ;
         object->iSaveTracePoint("ADD") ;
/*- - - - - - - - - - - - - - - - - - - - - - ����������� ���������� */
         object->iShowTrace_() ;
/*- - - - - - - - - - - - - - - - - - - - - - -  ����������� ������� */
   for(i=0 ; i<object->Features_cnt ; i++) {
     object->Features[i]->vBodyBasePoint(NULL, object->x_base, 
                                               object->y_base, 
                                               object->z_base ) ;
     object->Features[i]->vBodyAngles   (NULL, object->a_azim, 
                                               object->a_elev, 
                                               object->a_roll ) ;
                                            }
/*- - - - - - - - - - - - - - - - - - - - - - - - -  ��������� ����� */
          time_1=this->kernel->vGetTime() ;
       if(time_1-time_s>=this->kernel->show_time_step) {

                 time_s=time_1 ;

              this->kernel->vShow(NULL) ;
                                                       }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
       } while(1) ;                                                 /* END CIRCLE.1 - ���� ����������� */

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*                                                                  */
/*                 ���������� ���������� ITYPE                      */
/*                                                                  */
/*       ITYPE <���>                                                */
/*       ITYPE <���> <Iface type> [<Object type>]                   */

  int  RSS_Module_External::cIType(char *cmd)

{
#define   _PARS_MAX  10

                char *pars[_PARS_MAX] ;
                char *name ;
 RSS_Object_External *object ;
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

                     name= pars[0] ;

/*------------------------------------------- �������� ����� ������� */

    if(name==NULL) {                                                /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� �������. \n"
                                 "��������: ITYPE <���_�������> ...") ;
                                     return(-1) ;
                   }

       object=(RSS_Object_External *)FindObject(name, 1) ;          /* ���� ������ �� ����� */
    if(object==NULL)  return(-1) ;

/*--------------------------------------------- ������� ����� ������ */

   if(pars[1]==NULL || *pars[1]==0) {

        status=DialogBoxIndirectParam( GetModuleHandle(NULL),
                                      (LPCDLGTEMPLATE)Resource("IDD_IFACE", RT_DIALOG),
                                       GetActiveWindow(), 
                                       Object_External_Iface_dialog, 
                                      (LPARAM)object               ) ;
         return((int)status) ;
                                    }
/*------------------------------------------ ������� ���� ���������� */

                     strupr(pars[1]) ;

   if(stricmp(pars[1], "FILE"      ) &&
      stricmp(pars[1], "TCP-SERVER")   ) {

              SEND_ERROR("��������� ��������� ���� ����������: FILE, TCP-SERVER") ;
                     return(-1) ;
                                         }

                strcpy(object->iface_type, pars[1]) ;

   if(pars[2]!=NULL && *pars[2]!=0)
                strcpy(object->object_type, pars[2]) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*                                                                  */
/*                    ���������� ���������� IFILE                   */
/*                                                                  */
/*       IFILE/F <���> <Folder path>                                */
/*       IFILE/T <���> <Objects path>                               */
/*       IFILE/C <���> <Controls name>                              */

  int  RSS_Module_External::cIFile(char *cmd)

{
#define   _PARS_MAX  10

                char  *pars[_PARS_MAX] ;
                char  *name ;
 RSS_Object_External  *object ;
                 int   f_flag, t_flag, c_flag ;
                char  *end ;
                 int   i ;

/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
                      f_flag=0 ;
                      t_flag=0 ;
                      c_flag=0 ;

       if(*cmd=='/' ||
          *cmd=='+'   ) {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("������������ ������ �������") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, 'f')!=NULL ||
                   strchr(cmd, 'F')!=NULL   )  f_flag=1 ;
           else if(strchr(cmd, 't')!=NULL ||
                   strchr(cmd, 'T')!=NULL   )  t_flag=1 ;
           else if(strchr(cmd, 'c')!=NULL ||
                   strchr(cmd, 'C')!=NULL   )  c_flag=1 ;

                           cmd=end+1 ;
                        }

       if(f_flag+t_flag+c_flag==0) {
                      SEND_ERROR("������ ���� ����� ���� �� ���� �� ������: F, T ��� C. \n"
                                 "��������: IFILE/F <���_�������> ...") ;
                                           return(-1) ;
                                   }
       else
       if(f_flag+t_flag+c_flag!=1) {
                      SEND_ERROR("������ ���� ������ ���� �� ������: F, T ��� C. \n"
                                 "��������: IFILE/F <���_�������> ...") ;
                                           return(-1) ;
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

/*------------------------------------------- �������� ����� ������� */

    if(name==NULL) {                                                /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� �������. \n"
                                 "��������: IFILE <���_�������> ...") ;
                                     return(-1) ;
                   }

       object=(RSS_Object_External *)FindObject(name, 1) ;          /* ���� ������ �� ����� */
    if(object==NULL)  return(-1) ;

/*--------------------------------------------------- ������� ������ */

    if(pars[1]==NULL) {
                          SEND_ERROR("�� ������ �������� ���������") ;
                                             return(-1) ;
                      }

    if(f_flag) {

            if(access(pars[1], 0x00)!=0) {
                      SEND_ERROR("������� �������������� �����") ;
                                             return(-1) ;
                                         }

                  strcpy(object->iface_file_folder, pars[1]) ;
               }
    else 
    if(t_flag) {
                  strcpy(object->iface_targets, pars[1]) ;
               }
    else 
    if(c_flag) {
                  strcpy(object->iface_file_control, pars[1]) ;
               }
/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*                                                                  */
/*                    ���������� ���������� ITCP                    */
/*                                                                  */
/*       ITCP/U <���> <URL>                                         */

  int  RSS_Module_External::cITcp(char *cmd)

{
#define   _PARS_MAX  10

                char  *pars[_PARS_MAX] ;
                char  *name ;
 RSS_Object_External  *object ;
                 int   u_flag ;
                char  *end ;
                 int   i ;

/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
                      u_flag=0 ;

       if(*cmd=='/' ||
          *cmd=='+'   ) {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("������������ ������ �������") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, 'u')!=NULL ||
                   strchr(cmd, 'U')!=NULL   )  u_flag=1 ;

                           cmd=end+1 ;
                        }

       if(u_flag/*+t_flag*/==0) {
                         SEND_ERROR("������ ���� ����� ���� �� ���� �� ������: U ... \n"
                                    "��������: ITCP/U <���_�������> ...") ;
                                           return(-1) ;
                                }
       else
       if(u_flag/*+t_flag*/!=1) {
                         SEND_ERROR("������ ���� ������ ���� �� ������: U ... \n"
                                    "��������: ITCP/U <���_�������> ...") ;
                                           return(-1) ;
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

/*------------------------------------------- �������� ����� ������� */

    if(name==NULL) {                                                /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� �������. \n"
                                 "��������: ITCP <���_�������> ...") ;
                                     return(-1) ;
                   }

       object=(RSS_Object_External *)FindObject(name, 1) ;          /* ���� ������ �� ����� */
    if(object==NULL)  return(-1) ;

/*--------------------------------------------------- ������� ������ */

    if(pars[1]==NULL) {
                          SEND_ERROR("�� ������ �������� ���������") ;
                                             return(-1) ;
                      }

    if(u_flag) {
                  strcpy(object->iface_tcp_connect, pars[1]) ;
                  strcpy(object->iface_targets,     pars[1]) ;
               }
/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*                                                                  */
/*                ����� ������� ���� EXTERNAL �� �����              */

  RSS_Object *RSS_Module_External::FindObject(char *name, int  check_type)

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
     if(strcmp(OBJECTS[i]->Type, "External")) {

           SEND_ERROR("������ �� �������� �������� ���� EXTERNAL") ;
                            return(NULL) ;
                                              }
/*-------------------------------------------------------------------*/ 

   return(OBJECTS[i]) ;
  
#undef   OBJECTS
#undef   OBJECTS_CNT

}


/********************************************************************/
/********************************************************************/
/**                                                                **/
/**          �������� ������ ������� "������� ������"              **/
/**                                                                **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       ����������� ����������			    */

 O_EXTERNAL_API                            char *RSS_Object_External::targets     =NULL ;
 O_EXTERNAL_API                          double  RSS_Object_External::targets_time=  0. ;
 O_EXTERNAL_API                             int  RSS_Object_External::targets_init=  0 ;
 O_EXTERNAL_API  struct RSS_Object_ExternalLink *RSS_Object_External::targets_links[_TARGETS_LINKS_MAX] ;
 O_EXTERNAL_API                             int  RSS_Object_External::targets_links_cnt ;


/********************************************************************/
/*                                                                  */
/*                       ����������� ������                         */

     RSS_Object_External::RSS_Object_External(void)

{
   strcpy(Type, "External") ;

   CalculateExt_use= 1 ;

    Context        =new RSS_Transit_External ;
    Context->object=this ;

   Parameters    =NULL ;
   Parameters_cnt=  0 ;

     battle_state= 0 ;

      x_base    =   0. ;
      y_base    =   0. ;
      z_base    =   0. ;
      a_azim    =   0. ;
      a_elev    =   0. ;
      a_roll    =   0. ;
      x_velocity=   0. ;
      y_velocity=   0. ;
      z_velocity=   0. ;
      v_abs     =   0. ;
      g_ctrl    = 100. ;

           start_flag=0 ;

    strcpy(object_type,        "") ;

    strcpy(iface_type,         "") ;
    strcpy(iface_file_folder,  "") ;
    strcpy(iface_file_control, "$NAME$") ;
    strcpy(iface_tcp_connect,  "") ;
    strcpy(iface_targets,      "") ;

      mTrace      =NULL ;
      mTrace_cnt  =  0 ;  
      mTrace_max  =  0 ; 
      mTrace_color=RGB(0, 0, 127) ;
      mTrace_dlist=  0 ;  
}


/********************************************************************/
/*                                                                  */
/*                          ���������� ������                       */

    RSS_Object_External::~RSS_Object_External(void)

{
      vFree() ;

   delete Context ;
}


/********************************************************************/
/*                                                                  */
/*                     ������������ ��������                        */

  void   RSS_Object_External::vFree(void)

{
  int  i ;


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
/*                                                                  */
/*                    ��������� ��������� �������                   */
/*                    ������������ ��������� �������                */

    void  RSS_Object_External::vPush(void)

{
     x_base_stack    =x_base ;
     y_base_stack    =y_base ;
     z_base_stack    =z_base ;

     a_azim_stack    =a_azim ;
     a_elev_stack    =a_elev ;
     a_roll_stack    =a_roll ;

     x_velocity_stack=x_velocity ;
     y_velocity_stack=y_velocity ;
     z_velocity_stack=z_velocity ;
}


    void  RSS_Object_External::vPop(void)

{
     x_base    =x_base_stack ;
     y_base    =y_base_stack ;
     z_base    =z_base_stack ;

     a_azim    =a_azim_stack ;
     a_elev    =a_elev_stack ;
     a_roll    =a_roll_stack ;

     x_velocity=x_velocity_stack ;
     y_velocity=y_velocity_stack ;
     z_velocity=z_velocity_stack ;

  if(this->mTrace!=NULL) {
                             free(this->mTrace) ;
                                  this->mTrace    =NULL ;
                                  this->mTrace_cnt=  0 ;  
                                  this->mTrace_max=  0 ;  

                                      iShowTrace_() ;
                         }
}


/********************************************************************/
/*								    */
/*                        ���������� ������		            */

    class RSS_Object *RSS_Object_External::vCopy(char *name)

{
        RSS_Model_data  create_data ;
   RSS_Object_External *object ;
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

       object=(RSS_Object_External *)this->Module->vCreateObject(&create_data) ;
    if(object==NULL)  return(NULL) ;
 
            strcpy(object->owner,  this->owner) ;
                   object->o_owner=this->o_owner ;

            strcpy(object->object_type,        this->object_type) ;

            strcpy(object->iface_type,         this->iface_type) ;
            strcpy(object->iface_file_folder,  this->iface_file_folder) ;
            strcpy(object->iface_file_control, this->iface_file_control) ;
            strcpy(object->iface_tcp_connect,  this->iface_tcp_connect) ;
            strcpy(object->iface_targets,      this->iface_targets) ;

   if(RSS_Kernel::battle)  object->battle_state=_SPAWN_STATE ;

/*---------------------------------------------- ����������� ������� */

/*
    for(i=0 ; i<this->Features_cnt ; i++)
      if(!stricmp(this->Features[i]->Type, "Hit"))
             hit_1=(RSS_Feature_Hit *)this->Features[i] ;

    for(i=0 ; i<object->Features_cnt ; i++)
      if(!stricmp(object->Features[i]->Type, "Hit"))
             hit_2=(RSS_Feature_Hit *)object->Features[i] ;

           hit_2->hit_range =hit_1->hit_range ;
           hit_2->any_target=hit_1->any_target ;
           hit_2->any_weapon=hit_1->any_weapon ;
*/
/*-------------------------------------------------------------------*/

   return(object) ;
}


/********************************************************************/
/*                                                                  */
/*                      �������� ������ � ������                    */

    void  RSS_Object_External::vWriteSave(std::string *text)

{
  char  field[1024] ;
   int  i ;

/*----------------------------------------------- ��������� �������� */

     *text="#BEGIN OBJECT EXTERNAL\n" ;

/*----------------------------------------------------------- ������ */

    sprintf(field, "NAME=%s\n",       this->Name      ) ;  *text+=field ;
    sprintf(field, "X_BASE=%.10lf\n", this->x_base    ) ;  *text+=field ;
    sprintf(field, "Y_BASE=%.10lf\n", this->y_base    ) ;  *text+=field ;
    sprintf(field, "Z_BASE=%.10lf\n", this->z_base    ) ;  *text+=field ;
    sprintf(field, "A_AZIM=%.10lf\n", this->a_azim    ) ;  *text+=field ;
    sprintf(field, "A_ELEV=%.10lf\n", this->a_elev    ) ;  *text+=field ;
    sprintf(field, "A_ROLL=%.10lf\n", this->a_roll    ) ;  *text+=field ;
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
/*                                                                  */
/*                        ��������� �������                         */

     int  RSS_Object_External::vEvent(char *event_name, double  t, char *callback, int cb_size)
{
//    RSS_Feature_Hit *hit ; 

/*--------------------------------------------------- ��������� ���� */

   if(!stricmp(event_name, "HIT")) {
                                         return(0) ;
                                   }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*                                                                  */
/*                        ����������� ��������                      */

     int  RSS_Object_External::vSpecial(char *oper, void *data)
{
  return(-1) ;
}


/********************************************************************/
/*                                                                  */
/*             ���������� ������� ��������� ���������               */

     int  RSS_Object_External::vCalculateStart(double  t)
{
     Matrix2d  Sum_Matrix ;
     Matrix2d  Oper_Matrix ;  
     Matrix2d  Velo_Matrix ;
         char  flag[FILENAME_MAX] ;
         char  name[128] ;
         WORD  version ;
      WSADATA  winsock_data ;        /* ������ ������� WINSOCK */
          int  status ;
         char  text[1024] ;
          int  i ; 

  static  int  sockets_init ;

#define   OBJECTS       RSS_Kernel::kernel->kernel_objects 
#define   OBJECTS_CNT   RSS_Kernel::kernel->kernel_objects_cnt 

/*------------------------------------------ ������������� ����� TCP */

   if(sockets_init==0) {

                          version=MAKEWORD(1, 1) ;
        status=WSAStartup(version, &winsock_data) ;                 /* ����. Win-Sockets */
     if(status) {
                     sprintf(text, "Win-socket DLL loading error: %d", WSAGetLastError()) ;
                  SEND_ERROR(text) ;
                       return(-1) ;
                }

                       }

      sockets_init=1 ;

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

      this->x_base=this->o_owner->x_base ;
      this->y_base=this->o_owner->y_base ;
      this->z_base=this->o_owner->z_base ;

      this->a_azim=this->o_owner->a_azim ;
      this->a_elev=this->o_owner->a_elev ;
      this->a_roll=this->o_owner->a_roll ;
                          }
/*------------------------------------------ �������� � �������-���� */

      this->o_target=NULL ;

  if(this->target[0]!=0) {

       for(i=0 ; i<OBJECTS_CNT ; i++)                               /* ���� ������ �� ����� */
         if(!stricmp(OBJECTS[i]->Name, this->target)) {
                       this->o_target=OBJECTS[i] ;
                                 break ;
                                                      }
                         }
/*-------------------------------------------- ���������� ���������� */

       Velo_Matrix.LoadZero   (3, 1) ;
       Velo_Matrix.SetCell    (2, 0, this->v_abs) ;
        Sum_Matrix.Load3d_azim(-this->a_azim) ;
       Oper_Matrix.Load3d_elev(-this->a_elev) ;
        Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
       Velo_Matrix.LoadMul    (&Sum_Matrix, &Velo_Matrix) ;

         this->x_velocity=Velo_Matrix.GetCell(0, 0) ;
         this->y_velocity=Velo_Matrix.GetCell(1, 0) ;
         this->z_velocity=Velo_Matrix.GetCell(2, 0) ;

/*----------------------- ������� �������� ������ ��� ������� ������ */

   if(this->CalculateExt_use==1) {
/*- - - - - - - - - - - - - - - - - - -  ������� ��� ���������� FILE */
     if(!stricmp(this->iface_type, "FILE"      )) {

                                    strcpy(name, this->iface_file_control) ;
      if(!stricmp(name, "$NAME$"))  strcpy(name, this->Name) ;

            sprintf(flag, "%s/%s.out.flag", this->iface_file_folder, name) ;
             unlink(flag) ;
            sprintf(flag, "%s/%s.in.flag", this->iface_file_folder, name) ;
             unlink(flag) ;

                                                  }
/*- - - - - - - - - - - - - - - -  ������� ��� ���������� TCP-SERVER */
     else
     if(!stricmp(this->iface_type, "TCP-SERVER")) {


                                                  }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                 }  
/*------------------------------- ��������� �������� ���������� ���� */

            this->start_flag=1 ;

/*------------------------------------------------ ������� ��������� */

            this->iSaveTracePoint("CLEAR") ;

/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

  return(0) ;
}


/********************************************************************/
/*                                                                  */
/*                   ������ ��������� ���������                     */

     int  RSS_Object_External::vCalculate(double t1, double t2, char *callback, int cb_size)
{
/*----------------------------------------------- ��� ������� ������ */

                   x_base+=x_velocity*(t2-t1) ;
                   y_base+=y_velocity*(t2-t1) ;
                   z_base+=z_velocity*(t2-t1) ;
                   g_over = 0 ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*           ������ ��������� ��������� �� ������� ������           */

     int  RSS_Object_External::vCalculateExt1(double t1, double t2, char *callback, int callback_size)
{
        char  name[128] ;
        char  flag[FILENAME_MAX] ;
        char  path[FILENAME_MAX] ;
        FILE *file ;
         Tcp  tcp_server_iface ;
  Tcp_packet  data ;
         int  status ;
        char  value[128] ;
        char  text[4096] ;
        char *end ;
         int  i ;

/*------------------------------------- ������������ ������ �������� */

#define   OBJECTS       this->Module->kernel_objects 
#define   OBJECTS_CNT   this->Module->kernel_objects_cnt 

    if(targets_init==1) {
                            targets_time=-1. ;
                            targets_init= 0 ;
                        }
    if(targets_time<t1) {

       if(targets==NULL)   targets=(char *)calloc(1, _TARGETS_MAX) ;

                          *targets=0 ;

        sprintf(text, "\"t1\":\"%.2lf\";\"t2\":\"%.2lf\";[\n", t1, t2) ;
         strcat(targets, text) ;  

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(OBJECTS[i]->battle_state) {

#define   O    OBJECTS[i]

             sprintf(text, "{ \"name\":\"%s\";"
                             "\"x\":\"%.2lf\";\"y\":\"%.2lf\";\"z\":\"%.2lf\";"
                             "\"azim\":\"%.2lf\";\"elev\":\"%.2lf\";\"roll\":\"%.2lf\";"
                             "\"v_x\":\"%.2lf\";\"v_y\":\"%.2lf\";\"v_z\":\"%.2lf\"};\n",
                                O->Name,
                                O->x_base,     O->y_base,     O->z_base,
                                O->a_azim,     O->a_elev,     O->a_roll,
                                O->x_velocity, O->y_velocity, O->z_velocity  ) ;

              strcat(targets, text) ;  

#undef    O
                                      }

        sprintf(text, "{\"name\":\"$END_OF_LIST$\"}\n]\n") ;
         strcat(targets, text) ;  

                                  targets_time=t1 ;
                        } 

#undef   OBJECTS
#undef   OBJECTS_CNT

/*----------------------------------------- �������� ������ �������� */

   do {
/*- - - - - - - - - - - - - - - - - - -  �������� ��������� �������� */
         for(i=0 ; i<targets_links_cnt ; i++)
           if(!strcmp(targets_links[i]->link, this->iface_targets))  break ;

           if(i<targets_links_cnt) {
                  if(targets_links[i]->time_mark==t1)  break ;      /* ���� ������ �������� �� ������ ������ ��� ������������ */ 
                                   }
           else                    {
                       targets_links[i]=(struct RSS_Object_ExternalLink *)calloc(1, sizeof(struct RSS_Object_ExternalLink)) ;
                strcpy(targets_links[i]->link, this->iface_targets) ;
                       targets_links_cnt++ ;
                                   }

                     targets_links[i]->time_mark=t1 ;
/*- - - - - - - - - - - - - -  �������� ������� ����� ��������� FILE */
   if(!stricmp(this->iface_type, "FILE")) {

         file=fopen(this->iface_targets, "w") ;
      if(file==NULL) {
                          sprintf(text, "������ �������� ����� ��������: %s", this->iface_targets) ;
                       SEND_ERROR(text) ;
                            return(-1) ;
                     }

             fwrite(targets, 1, strlen(targets), file) ;
             fclose(file) ;

                                          }
/*- - - - - - - - - - -  �������� ������� ����� ��������� TCP-SERVER */
   if(!stricmp(this->iface_type, "TCP-SERVER")) {

              strcpy(value, this->iface_targets) ;
          end=strchr(value, ':') ;
       if(end==NULL) {
                          sprintf(text, "������ %s - ������ ������� URL ������ �����", this->Name) ;
                       SEND_ERROR(text) ;
                            return(-1) ;
                     }

                            *end=0 ;

                    tcp_server_iface.mServer_name=value ;
                    tcp_server_iface.mServer_port=atoi(end+1) ;

           memmove(targets+8, targets, strlen(targets)+1) ;         /* ��������� ������� ���� ��������� */
            memcpy(targets, "TARGETS:", 8) ;

                                         data.data_out.assign(targets) ;
          status=tcp_server_iface.Client(&data) ; 
       if(status) {
                          sprintf(text, "������ %s - ������ �������� TARGETS-������� %d", this->Name, status) ;
                       SEND_ERROR(text) ;
                            return(-1) ;
                  }  

       if(stricmp(data.data_in.c_str(), "SUCCESS")) {               /* �������� ���������� */

                          sprintf(text, "������ %s - ������ ���������� TARGETS-�������: %s", this->Name, data.data_in.c_str()) ;
                       SEND_ERROR(text) ;
                            return(-1) ;
                                                    }

                                                }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      } while(0) ;

/*------------------------------- �������� ������� �� ������� ������ */
/*- - - - - - - - - - - - - - - - - - - - - - - ������������ ������� */
         memset(text, 0, sizeof(text)) ;

        sprintf(value, "\"name\":\"%s\";\"start\":\"%d\";\"t1\":\"%.2lf\";\"t2\":\"%.2lf\"\n", this->Name, this->start_flag, t1, t2) ;
         strcat(text, value) ;  
        sprintf(value, "\"x\":\"%.2lf\";\"y\":\"%.2lf\";\"z\":\"%.2lf\"\n", this->x_base, this->y_base, this->z_base) ;
         strcat(text, value) ;  
        sprintf(value, "\"azim\":\"%.2lf\";\"elev\":\"%.2lf\";\"roll\":\"%.2lf\"\n", this->a_azim, this->a_elev, this->a_roll) ;
         strcat(text, value) ;  
        sprintf(value, "\"v_x\":\"%.2lf\";\"v_y\":\"%.2lf\";\"v_z\":\"%.2lf\"\n", this->x_velocity, this->y_velocity, this->z_velocity) ;
         strcat(text, value) ;  
        sprintf(value, "\"type\":\"%s\";\"target\":\"%s\"\n", this->object_type, this->target) ;
         strcat(text, value) ;  
/*- - - - - - - - - - - - - -  �������� ������� ����� ��������� FILE */
   if(!stricmp(this->iface_type, "FILE")) {
                         
                                    strcpy(name, this->iface_file_control) ;
      if(!stricmp(name, "$NAME$"))  strcpy(name, this->Name) ;

            sprintf(path, "%s/%s.out",      this->iface_file_folder, name) ;
            sprintf(flag, "%s/%s.out.flag", this->iface_file_folder, name) ;

         file=fopen(path, "w") ;
      if(file==NULL) {
                          sprintf(text, "������ %s - ������ �������� ����� �������: %s", this->Name, path) ;
                       SEND_ERROR(text) ;
                            return(-1) ;
                     }

             fwrite(text, 1, strlen(text), file) ;
             fclose(file) ;

         file=fopen(flag, "w") ;
      if(file==NULL) {
                          sprintf(text, "������ %s - ������ �������� ����-����� �������: %s", this->Name, path) ;
                       SEND_ERROR(text) ;
                            return(-1) ;
                     }

             fclose(file) ;
 
                                          }
/*- - - - - - - - - - -  �������� ������� ����� ��������� TCP-SERVER */
   if(!stricmp(this->iface_type, "TCP-SERVER")) {

              strcpy(value, this->iface_tcp_connect) ;
          end=strchr(value, ':') ;
       if(end==NULL) {
                          sprintf(text, "������ %s - ������ ������� URL ����������", this->Name) ;
                       SEND_ERROR(text) ;
                            return(-1) ;
                     }

                            *end=0 ;

                    tcp_server_iface.mServer_name=value ;
                    tcp_server_iface.mServer_port=atoi(end+1) ;

           memmove(text+5, text, strlen(text)+1) ;                  /* ��������� ������� ���� ��������� */
            memcpy(text, "STEP:", 5) ;

                                         data.data_out.assign(text) ;
          status=tcp_server_iface.Client(&data) ; 
       if(status) {
                          sprintf(text, "������ %s - ������ �������� STEP-������� %d", this->Name, status) ;
                       SEND_ERROR(text) ;
                            return(-1) ;
                  }  

       if(stricmp(data.data_in.c_str(), "SUCCESS")) {               /* �������� ���������� */

                          sprintf(text, "������ %s - ������ ���������� STEP-�������: %s", this->Name, data.data_in.c_str()) ;
                       SEND_ERROR(text) ;
                            return(-1) ;
                                                    }

                                                }
/*----------------------------------- ����� �������� ���������� ���� */

                  this->start_flag=0 ;

/*-------------------------------------------------------------------*/
    
  return(0) ;
}

     int  RSS_Object_External::vCalculateExt2(double t1, double t2, char *callback, int callback_size)
{
        char  name[128] ;
        char  flag[FILENAME_MAX] ;
        char  path[FILENAME_MAX] ;
        FILE *file ;
         Tcp  tcp_server_iface ;
  Tcp_packet  data ;
        char  em_name[128] ;
      double  em_t1 ;
      double  em_t2 ;
        char  em_message[1024] ;
         int  status ;
        char  value[1024] ;
        char  text[4096] ;
        char *key ;
        char *end ;
         int  i ;

  static  char *keys[]={"\"name\":\"", "\"t1\":\"",   "\"t2\":\"",
                        "\"x\":\"",    "\"y\":\"",    "\"z\":\"",
                        "\"azim\":\"", "\"elev\":\"", "\"roll\":\"",
                        "\"v_x\":\"",  "\"v_y\":\"",  "\"v_z\":\"",
                        "\"message\":\"",
                         NULL} ;

/*------------------------------------------ �������� ������� ������ */

   do {
/*- - - - - - - - - - - - - -  �������� ������� ����� ��������� FILE */
   if(!stricmp(this->iface_type, "FILE")) {

                                    strcpy(name, this->iface_file_control) ;
      if(!stricmp(name, "$NAME$"))  strcpy(name, this->Name) ;

        sprintf(path, "%s/%s.in",      this->iface_file_folder, name) ;
        sprintf(flag, "%s/%s.in.flag", this->iface_file_folder, name) ;

      if(access(flag, 0x00)) continue ;

         file=fopen(path, "rt") ;
      if(file==NULL) {
                          sprintf(text, "������ %s - ������ �������� ����� ������: %s", this->Name, path) ;
                       SEND_ERROR(text) ;
                            return(-1) ;
                     }

             memset(text, 0, sizeof(text)) ;
              fread(text, 1, sizeof(text)-1, file) ;
             fclose(file) ;

        status=unlink(flag) ;
      if(status) {
                          sprintf(text, "ERROR - Response flag-file remove error %d : %s", errno, path) ;
                       SEND_ERROR(text) ;
                            return(-1) ;
                 }

                      break ;
                                          }
/*- - - - - - - - - - -  �������� ������� ����� ��������� TCP-SERVER */
   if(!stricmp(this->iface_type, "TCP-SERVER")) {

              strcpy(value, this->iface_tcp_connect) ;
          end=strchr(value, ':') ;
       if(end==NULL) {
                          sprintf(text, "������ %s - ������ ������� URL ����������", this->Name) ;
                       SEND_ERROR(text) ;
                            return(-1) ;
                     }

                            *end=0 ;

                    tcp_server_iface.mServer_name=value ;
                    tcp_server_iface.mServer_port=atoi(end+1) ;

          sprintf(text, "GET:\"name\":\"%s\";\"t1\":\"%.2lf\";",   /* ������������ ������� */
                            this->Name, t1) ;

                                         data.data_out.assign(text) ;
          status=tcp_server_iface.Client(&data) ; 
       if(status) {
                          sprintf(text, "������ %s - ������ �������� GET-������� %d", this->Name, status) ;
                       SEND_ERROR(text) ;
                            return(-1) ;
                  }  

            memset(text, 0, sizeof(text)) ;
           strncpy(text, data.data_in.c_str(), sizeof(text)-1) ;

       if(!memicmp(text, "WARNING", strlen("WARNING"))) {           /* ��������� ��� �� ����� */
                         continue ;
                                                        }
       else
       if(!memicmp(text, "ERROR",   strlen("ERROR"  ))) {           /* ������ */

                           memset(value, 0, sizeof(value)) ;
                          strncpy(value, text+6, sizeof(value)-1) ;
                          sprintf(text, "������ %s - ������ ���������� GET-�������: %s", this->Name, value) ;
                       SEND_ERROR(text) ;
                            return(-1) ;
                                                        }
       else
       if(!memicmp(text, "SUCCESS", strlen("SUCCESS"))) {           /* ������� ��������� */

              memmove(text, text+8, strlen(text+8)+1) ;
                              break ;
                                                        }
       else                                             {

                           memset(value, 0, sizeof(value)) ;
                          strncpy(value, text, sizeof(value)-1) ;
                          sprintf(text, "������ %s - ������������ ����� �� GET-������: %s", this->Name, value) ;
                       SEND_ERROR(text) ;
                            return(-1) ;
                                                        }
                                                }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
             Sleep(10) ;

      } while(1) ;

/*-------------------------------------------- ������ ������ ������� */

   for(i=0 ; keys[i]!=NULL ; i++) {

        key=strstr(text, keys[i]) ;
     if(key==NULL) {
                         sprintf(text, "Parameter %s is missed", keys[i]) ;
                      SEND_ERROR(text) ;
                           return(-1) ;
                   } 

            memset(value, 0, sizeof(value)) ;
           strncpy(value, key+strlen(keys[i]), sizeof(value)-1) ;
        end=strchr(value, '"')  ;
     if(end==NULL) {
                         sprintf(text, "Invalid value for parameter %s", keys[i]) ;
                      SEND_ERROR(text) ;
                           return(-1) ;
                   } 

       *end=0 ;

     if(i== 0)  strcpy(em_name, value) ;  
     if(i== 1)         em_t1           =strtod(value, &end) ;
     if(i== 2)         em_t2           =strtod(value, &end) ;
     if(i== 3)         this->x_base    =strtod(value, &end) ;
     if(i== 4)         this->y_base    =strtod(value, &end) ;
     if(i== 5)         this->z_base    =strtod(value, &end) ;
     if(i== 6)         this->a_azim    =strtod(value, &end) ;
     if(i== 7)         this->a_elev    =strtod(value, &end) ;
     if(i== 8)         this->a_roll    =strtod(value, &end) ;
     if(i== 9)         this->x_velocity=strtod(value, &end) ;
     if(i==10)         this->y_velocity=strtod(value, &end) ;
     if(i==11)         this->z_velocity=strtod(value, &end) ;
     if(i==12)    strcpy(em_message, value) ;  

                                  } 
/*------------------------------- ������������ ������ �������� ����� */

  while(em_message[0]!=0) {

         end=strchr(em_message, ';') ;
      if(end==NULL) {
                         sprintf(text, "Command element terminator ';' missed") ;
                      SEND_ERROR(text) ;
                           return(-1) ;
                    }             

        *end=0 ;

#define  _KEY  "DESTROY"

   if(!memicmp(em_message, _KEY, strlen(_KEY))) {

            sprintf(value, "KILL %s;", this->Name) ;

                                                }

#undef   _KEY
#define  _KEY  "START"

   else
   if(!memicmp(em_message, _KEY, strlen(_KEY))) {

            sprintf(value, "%s;", em_message) ;

                                                }

#undef   _KEY
#define  _KEY  "STOP"

   else
   if(!memicmp(em_message, _KEY, strlen(_KEY))) {

            sprintf(value, "STOP %s;", this->Name) ;

                                                }

#undef   _KEY
#define  _KEY  "HIT"

   else
   if(!memicmp(em_message, _KEY, strlen(_KEY))) {

            sprintf(value, "EVENT HITED %s;", em_message+strlen(_KEY)+1) ;

                                                }

#undef   _KEY
#define  _KEY  "BLAST-A"

   else
   if(!memicmp(em_message, _KEY, strlen(_KEY))) {

             strcpy(name, em_message+strlen(_KEY)+1) ;
            sprintf(value, "EXEC BLAST CR/A blast_%s 0 10 %s;START blast_%s;", name, name, name) ;

                                                }

#undef   _KEY
#define  _KEY  "EXEC"

   else
   if(!memicmp(em_message, _KEY, strlen(_KEY))) {

            sprintf(value, "%s;", em_message) ;

                                                }
   else                                         {

            sprintf(text, "Unknown elements in command: %s", em_message) ;
         SEND_ERROR(text) ;
              return(-1) ;
                                                }

      if(strlen(callback)+strlen(value)>=callback_size-8) {
                         sprintf(text, "Callback buffer overflow") ;
                      SEND_ERROR(text) ;
                           return(-1) ;
                                                          }

                        strcat(callback, value) ;
                       memmove(em_message, end+1, strlen(end+1)+1) ;
                 } 

/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*                                                                  */
/*      ����������� ���������� ������� ��������� ���������          */

     int  RSS_Object_External::vCalculateShow(double  t1, double  t2)
{
   int i ;


         this->iSaveTracePoint("ADD") ;                             /* ���������� ����� ���������� */  

         this->iShowTrace_() ;                                      /* ����������� ���������� */

   for(i=0 ; i<this->Features_cnt ; i++) {                          /* ����������� ������� */
     this->Features[i]->vBodyBasePoint(NULL, this->x_base, 
                                             this->y_base, 
                                             this->z_base ) ;
     this->Features[i]->vBodyAngles   (NULL, this->a_azim, 
                                             this->a_elev, 
                                             this->a_roll ) ;
                                         }

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                   ���������� ����� ����������                     */

  int  RSS_Object_External::iSaveTracePoint(char *action)

{
/*------------------------------------------------- ����� ���������� */

   if(!stricmp(action, "CLEAR")) {
                                     mTrace_cnt=0 ;
                                       return(0) ;
                                 }
/*----------------------------------------------- ����������� ������ */

   if(mTrace_cnt==mTrace_max) {

          mTrace_max+= 1000 ;
          mTrace     =(RSS_Object_ExternalTrace *)
                            realloc(mTrace, mTrace_max*sizeof(RSS_Object_ExternalTrace)) ;

       if(mTrace==NULL) {
                   SEND_ERROR("EXTERNAL.iSaveTracePoint@"
                              "Memory over for trajectory") ;
                                  return(-1) ;
                        }
                              }
/*------------------------------------------------- ���������� ����� */

                  mTrace[mTrace_cnt].x_base    =this->x_base ;
                  mTrace[mTrace_cnt].y_base    =this->y_base ;
                  mTrace[mTrace_cnt].z_base    =this->z_base ;
                  mTrace[mTrace_cnt].a_azim    =this->a_azim ;
                  mTrace[mTrace_cnt].a_elev    =this->a_elev ;
                  mTrace[mTrace_cnt].a_roll    =this->a_roll ;
                  mTrace[mTrace_cnt].x_velocity=this->x_velocity ;
                  mTrace[mTrace_cnt].y_velocity=this->y_velocity ;
                  mTrace[mTrace_cnt].z_velocity=this->z_velocity ;

      if(g_over)  mTrace[mTrace_cnt].color     =RGB(127, 0,   0) ;
      else        mTrace[mTrace_cnt].color     =RGB(  0, 0, 127) ;

                         mTrace_cnt++ ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*                                                                  */
/*           ����������� ���������� � ��������� ���������           */

  void  RSS_Object_External::iShowTrace_(void)

{
    strcpy(Context->action, "SHOW_TRACE") ;

  SendMessage(RSS_Kernel::kernel_wnd, 
                WM_USER, (WPARAM)_USER_CHANGE_CONTEXT, 
                         (LPARAM) this->Context       ) ;
}


/*********************************************************************/
/*                                                                   */
/*                     ����������� ����������                        */

  void  RSS_Object_External::iShowTrace(void)

{
       int  status ;
       int  i ;

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

    if(mTrace_cnt>0) {
                                        i=0 ;

             glColor4d(GetRValue(mTrace[i].color)/256., 
                       GetGValue(mTrace[i].color)/256.,
                       GetBValue(mTrace[i].color)/256., 1.) ;

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

                     }
/*----------------------------- �������������� ��������� ����������� */

             glEndList() ;                                          /* �������� ������ */

/*----------------------------------- ������� ���������� ����������� */

          status=RSS_Kernel::display.SetNextContext("Show") ;
                     }                                              /* CONTINUE.1 */
/*-------------------------------------------------------------------*/

}


/*********************************************************************/
/*                                                                   */
/*              ���������� ������ "������� ���������"                */
/*                                                                   */
/*********************************************************************/

/*********************************************************************/
/*                                                                   */
/*              ����������� ������ "������� ���������"               */

     RSS_Transit_External::RSS_Transit_External(void)

{
}


/*********************************************************************/
/*                                                                   */
/*               ���������� ������ "������� ���������"               */

    RSS_Transit_External::~RSS_Transit_External(void)

{
}


/********************************************************************/
/*                                                                  */
/*                    ���������� ��������                           */

    int  RSS_Transit_External::vExecute(void)

{
   if(!stricmp(action, "SHOW_TRACE"))  ((RSS_Object_External *)object)->iShowTrace() ;

   return(0) ;
}

/********************************************************************/
/*								    */
/*		������ ���������� �������� "������"  		    */
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
#include "..\RSS_Kernel\RSS_Kernel.h"
#include "..\RSS_Model\RSS_Model.h"
#include "..\F_Show\F_Show.h"
#include "..\F_Hit\F_Hit.h"

#include "O_Missile.h"

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

     static   RSS_Module_Missile  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	����������������� ����                      */

 O_MISSILE_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 O_MISSILE_API RSS_Kernel *GetEntry(void)

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

  struct RSS_Module_Missile_instr  RSS_Module_Missile_InstrList[]={

 { "help",    "?",  "#HELP   - ������ ��������� ������", 
                     NULL,
                    &RSS_Module_Missile::cHelp   },
 { "create",  "cr", "#CREATE - ������� ������",
                    " CREATE <���> [<������> [<������ ����������>]]\n"
                    "   ������� ����������� ������ �� ����������������� ������",
                    &RSS_Module_Missile::cCreate },
 { "info",    "i",  "#INFO - ������ ���������� �� �������",
                    " INFO <���> \n"
                    "   ������ �������� ��������� �� ������� � ������� ����\n"
                    " INFO/ <���> \n"
                    "   ������ ������ ���������� �� ������� � ��������� ����",
                    &RSS_Module_Missile::cInfo },
 { "copy",    "cp", "#COPY - ���������� ������",
                    " COPY <��� �������> <��� ������ �������>\n"
                    "   ���������� ������",
                    &RSS_Module_Missile::cCopy },
 { "owner",   "o",  "#OWNER - ��������� �������� ������",
                    " OWNER <���> <��������>\n"
                    "   ��������� ������ - �������� ������",
                    &RSS_Module_Missile::cOwner },
 { "target",  "tg", "#TARGET - ��������� ���� ������",
                    " TARGET <���> <����>\n"
                    "   ��������� ������ - ���� ������",
                    &RSS_Module_Missile::cTarget },
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
                    &RSS_Module_Missile::cBase },
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
                    &RSS_Module_Missile::cAngle },
 { "velocity", "v", "#VELOCITY - ������� �������� �������� �������",
                    " VELOCITY <���> <��������>\n"
                    "   ������� ����������� �������� �������� �������"
                    " VELOCITY <���> <X-��������> <Y-��������> <Z-��������>\n"
                    "   ������� �������� �������� �������",
                    &RSS_Module_Missile::cVelocity },
 { "control",  "c", "#CONTROL - ���������� ��������",
                    " CONTROL <���> <���� �����> [<����������>]\n"
                    "   ������� ������� ������ ���������� ����������\n"
                    " CONTROL/G <���> <����������>\n"
                    "   ������� ������� ������ ���������� ����������\n"
                    " CONTROL> <���> [<��� �����> [<��� ���������>]]\n"
                    "   ���������� �����������\n",
                    &RSS_Module_Missile::cControl },
 { "homing",   "h", "#HOMING - ��������� �������������",
                    " HOMING/� <���>\n"
                    "   ��������� � ����������� ����� (�� ���������)\n"
                    " HOMING/P <���> <�����������>\n"
                    "   ��������� ������� ���������������� ��������� � �������� �������������\n",
                    &RSS_Module_Missile::cHoming },
 { "mark",     "m", "#MARK - �������� ��������� �������",
                    " MARK <���> <������>\n"
                    "   �������� ��������� ������� � �������� ��������\n",
                    &RSS_Module_Missile::cMark },
 { "trace",    "t", "#TRACE - ����������� ���������� �������",
                    " TRACE <���> [<������������>]\n"
                    "   ����������� ���������� ������� � �������� �������\n"
                    " TRACE/C <���> <R> <G> <B>\n"
                    "   ������� ����� ������\n"
                    " TRACE/O <���> <R> <G> <B>\n"
                    "   ������� ����� ������ �� �������� ����������\n"
                    " TRACE/W <���> <������� �����>\n"
                    "   ������� ������� ����� ������\n",
                    &RSS_Module_Missile::cTrace },
 {  NULL }
                                                            } ;

/********************************************************************/
/*								    */
/*		     ����� ����� ������             		    */

    struct RSS_Module_Missile_instr *RSS_Module_Missile::mInstrList=NULL ;


/********************************************************************/
/*								    */
/*		       ����������� ������			    */

     RSS_Module_Missile::RSS_Module_Missile(void)

{
	   keyword="EmiStand" ;
    identification="Missile" ;
          category="Object" ;

        mInstrList=RSS_Module_Missile_InstrList ;

        a_step=15. ;
        g_step= 1. ;
}


/********************************************************************/
/*								    */
/*		        ���������� ������			    */

    RSS_Module_Missile::~RSS_Module_Missile(void)

{
}


/********************************************************************/
/*								    */
/*		      �������� �������                              */

  RSS_Object *RSS_Module_Missile::vCreateObject(RSS_Model_data *data)

{
  RSS_Object_Missile *object ;
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
              SEND_ERROR("������ MISSILE: �� ������ ��� �������") ;
                                return(NULL) ;
                         }

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, data->name)) {
              SEND_ERROR("������ MISSILE: ������ � ����� ������ ��� ����������") ;
                                return(NULL) ;
                                                    }
/*-------------------------------------- ���������� �������� ������� */
/*- - - - - - - - - - - - ���� ������ ������ ��������� � ����������� */
   if(data->path[0]==0) {

    if(data->model[0]==0) {                                         /* ���� ������ �� ������ */
              SEND_ERROR("������ MISSILE: �� ������ ������ �������") ;
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
              SEND_ERROR("������ MISSILE: ����������� ������ ����") ;
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

              SEND_ERROR("������ MISSILE: �������������� ����� ���������� ������") ;
                                return(NULL) ;
                                             }
/*------------------------------------------------- �������� ������� */

       object=new RSS_Object_Missile ;
    if(object==NULL) {
              SEND_ERROR("������ MISSILE: ������������ ������ ��� �������� �������") ;
                        return(NULL) ;
                     }
/*------------------------------------- ���������� ������ ���������� */
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������� ��������� */
     for(i=0 ; i<5 ; i++)
       if(data->pars[i].text[0]!=0) {

           PAR=(struct RSS_Parameter *)
                 realloc(PAR, (PAR_CNT+1)*sizeof(*PAR)) ;
        if(PAR==NULL) {
                         SEND_ERROR("������ MISSILE: ������������ ������") ;
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
                         SEND_ERROR("������ MISSILE: ������������ ������") ;
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
                                         data->sections[i].decl, "Missile.Body") ;
                                             }

                                         data->sections[i].title[0]= 0 ;
                                        *data->sections[i].decl    ="" ;
                                               }
/*---------------------------------- �������� ������� � ����� ������ */

       OBJECTS=(RSS_Object **)
                 realloc(OBJECTS, (OBJECTS_CNT+1)*sizeof(*OBJECTS)) ;
    if(OBJECTS==NULL) {
              SEND_ERROR("������ MISSILE: ������������ ������") ;
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
/*								    */
/*		        �������� ��������       		    */

     int  RSS_Module_Missile::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- �������� ������ */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s -  ������� ������ � ��������� ��������������", identification) ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		        ��������� �������       		    */

  int  RSS_Module_Missile::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* ���� ������ ������ ������� */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "MISSILE"
#define  _SECTION_SHRT_NAME   "MISSILE"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Object Missile:") ;
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
       if(status)  SEND_ERROR("������ MISSILE: ����������� �������") ;
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

    void  RSS_Module_Missile::vReadSave(std::string *data)

{
               char *buff ;
                int  buff_size ;
     RSS_Model_data  create_data ;
 RSS_Object_Missile *object ;
               char  name[128] ;
               char *entry ;
               char *end ;
                int  i ;

/*----------------------------------------------- �������� ��������� */

   if(memicmp(data->c_str(), "#BEGIN MODULE MISSILE\n", 
                      strlen("#BEGIN MODULE MISSILE\n")) &&
      memicmp(data->c_str(), "#BEGIN OBJECT MISSILE\n", 
                      strlen("#BEGIN OBJECT MISSILE\n"))   )  return ;

/*------------------------------------------------ ���������� ������ */

              buff_size=(int)data->size()+16 ;
              buff     =(char *)calloc(1, buff_size) ;

       strcpy(buff, data->c_str()) ;

/*------------------------------------------------- �������� ������� */

   if(!memicmp(buff, "#BEGIN OBJECT MISSILE\n", 
              strlen("#BEGIN OBJECT MISSILE\n"))) {                 /* IF.1 */
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
                object=(RSS_Object_Missile *)vCreateObject(&create_data) ;
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

    void  RSS_Module_Missile::vWriteSave(std::string *text)

{
  std::string  buff ;

/*----------------------------------------------- ��������� �������� */

     *text="#BEGIN MODULE MISSILE\n" ;

/*------------------------------------------------ �������� �������� */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� HELP                    */

  int  RSS_Module_Missile::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Object_Missile_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� CREATE                  */
/*								    */
/*      CREATE <���> [<������> [<������ ����������>]]               */

  int  RSS_Module_Missile::cCreate(char *cmd)

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
                                     Object_Missile_Create_dialog, 
                                    (LPARAM)&data               ) ;
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

  int  RSS_Module_Missile::cInfo(char *cmd)

{
               char  *name ;
 RSS_Object_Missile  *object ;
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

       object=(RSS_Object_Missile *)FindObject(name, 1) ;           /* ���� ������ �� ����� */
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
                        object->state.x, object->state.y, object->state.z,
                        object->state.azim, object->state.elev, object->state.roll,
                        object->v_abs,
                        object->state.x_velocity, object->state.y_velocity, object->state.z_velocity,
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

  int  RSS_Module_Missile::cCopy(char *cmd)

{
#define   _PARS_MAX  10

                   char  *pars[_PARS_MAX] ;
                   char  *name ;
                   char  *copy ;
     RSS_Object_Missile  *missile ;
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

       missile=(RSS_Object_Missile *)FindObject(name, 1) ;      /* ���� ������-������� �� ����� */
    if(missile==NULL)  return(-1) ;

/*------------------------------------------ �������� ����� �������� */

    if(copy==NULL) {                                                /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� �������-�����. \n"
                                 "��������: COPY <��� �������> <��� ������ �������>") ;
                                     return(-1) ;
                   }

//     object=FindObject(copy, 0) ;                                 /* ���� ������-����� �� ����� */
//  if(object!=NULL) {
//                    SEND_ERROR("O�����-����� ��� ����������") ;
//                                   return(-1) ;
//                   }
/*---------------------------------------------- ����������� ������� */

            object=missile->vCopy(copy) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� OWNER                   */
/*								    */
/*       OWNER <���> <��������>                                     */

  int  RSS_Module_Missile::cOwner(char *cmd)

{
#define   _PARS_MAX  10

               char  *pars[_PARS_MAX] ;
               char  *name ;
               char  *owner ;
 RSS_Object_Missile  *missile ;
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

       missile=(RSS_Object_Missile *)FindObject(name, 1) ;          /* ���� ������-���� �� ����� */
    if(missile==NULL)  return(-1) ;

/*------------------------------------------ �������� ����� �������� */

    if(owner==NULL) {                                               /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� �������-��������. \n"
                                 "��������: OWNER <���_������> <���_��������>") ;
                                     return(-1) ;
                    }

       object=FindObject(owner, 0) ;                                /* ���� ������-�������� �� ����� */
    if(object==NULL)  return(-1) ;

/*------------------------------------------------- ������� �������� */

          strcpy(missile->owner, owner) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� TARGET                  */
/*								    */
/*       TARGET <���> <����>                                        */

  int  RSS_Module_Missile::cTarget(char *cmd)

{
#define   _PARS_MAX  10

               char  *pars[_PARS_MAX] ;
               char  *name ;
               char  *target ;
 RSS_Object_Missile  *missile ;
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

       missile=(RSS_Object_Missile *)FindObject(name, 1) ;          /* ���� ������ �� ����� */
    if(missile==NULL)  return(-1) ;

/*---------------------------------------------- �������� ����� ���� */

    if(target==NULL) {                                              /* ���� ��� �� ������... */
                        SEND_ERROR("�� ������ ��� �������-����. \n"
                                   "��������: TARGET <���_������> <���_����>") ;
                                     return(-1) ;
                    }

       object=FindObject(target, 0) ;                               /* ���� ������-���� �� ����� */
    if(object==NULL)  return(-1) ;

/*----------------------------------------------------- ������� ���� */

               strcpy(missile->target, target) ;

   for(i=0 ; i<missile->Features_cnt ; i++)
     if(!stricmp(missile->Features[i]->Type, "Hit")) 
         strcpy(((RSS_Feature_Hit *)(missile->Features[i]))->target, target) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� BASE                    */
/*								    */
/*        BASE    <���> <X> <Y> <Z>                                 */
/*        BASE/X  <���> <X>                                         */
/*        BASE/+X <���> <X>                                         */
/*        BASE>   <���> <��� ���������> <���>                       */
/*        BASE>>  <���> <��� ���������> <���>                       */

  int  RSS_Module_Missile::cBase(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

               char  *pars[_PARS_MAX] ;
               char  *name ;
               char **xyz ;
             double   coord[_COORD_MAX] ;
                int   coord_cnt ;
             double   inverse ;
 RSS_Object_Missile  *object ;
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

       object=(RSS_Object_Missile *)FindObject(name, 1) ;           /* ���� ������ �� ����� */
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

          if(xyz_flag=='X')   object->state.x+=inverse*coord[0] ;
     else if(xyz_flag=='Y')   object->state.y+=inverse*coord[0] ;                 
     else if(xyz_flag=='Z')   object->state.z+=inverse*coord[0] ;
                  }
/*- - - - - - - - - - - - - - - - - - - - - - -  ���������� �������� */
   else           {

          if(xyz_flag=='X')   object->state.x=coord[0] ;
     else if(xyz_flag=='Y')   object->state.y=coord[0] ;                 
     else if(xyz_flag=='Z')   object->state.z=coord[0] ;
     else                   {
                              object->state.x=coord[0] ;
                              object->state.y=coord[1] ;
                              object->state.z=coord[2] ;
                            }
                  }
/*---------------------------------------------- ������� �� �������� */

   for(i=0 ; i<object->Features_cnt ; i++)
     object->Features[i]->vBodyBasePoint(NULL, object->state.x, 
                                               object->state.y, 
                                               object->state.z ) ;

/*------------------------------------------------------ ����������� */

                      this->kernel->vShow(NULL) ;

            object->state_0=object->state ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� ANGLE                   */
/*								    */
/*       ANGLE    <���> <A> <E> <R>                                 */
/*       ANGLE/A  <���> <A>                                         */
/*       ANGLE/+A <���> <A>                                         */
/*       ANGLE>   <���> <��� ���������> <���>                       */
/*       ANGLE>>  <���> <��� ���������> <���>                       */

  int  RSS_Module_Missile::cAngle(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

               char  *pars[_PARS_MAX] ;
               char  *name ;
               char **xyz ;
             double   coord[_COORD_MAX] ;
                int   coord_cnt ;
             double   inverse ;
 RSS_Object_Missile  *object ;
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

       object=(RSS_Object_Missile *)FindObject(name, 1) ;           /* ���� ������ �� ����� */
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

          if(xyz_flag=='A')   object->state.azim+=inverse*coord[0] ;
     else if(xyz_flag=='E')   object->state.elev+=inverse*coord[0] ;                 
     else if(xyz_flag=='R')   object->state.roll+=inverse*coord[0] ;
                  }
/*- - - - - - - - - - - - - - - - - - - - - - -  ���������� �������� */
   else           {

          if(xyz_flag=='A')   object->state.azim=coord[0] ;
     else if(xyz_flag=='E')   object->state.elev=coord[0] ;                 
     else if(xyz_flag=='R')   object->state.roll=coord[0] ;
     else                   {
                              object->state.azim=coord[0] ;
                              object->state.elev=coord[1] ;
                              object->state.roll=coord[2] ;
                            }
                  }
/*- - - - - - - - - - - - - - - - - - - - - -  ������������ �������� */
     while(object->state.azim> 180.)  object->state.azim-=360. ;
     while(object->state.azim<-180.)  object->state.azim+=360. ;

     while(object->state.elev> 180.)  object->state.elev-=360. ;
     while(object->state.elev<-180.)  object->state.elev+=360. ;

     while(object->state.roll> 180.)  object->state.roll-=360. ;
     while(object->state.roll<-180.)  object->state.roll+=360. ;

/*---------------------------------------------- ���������� �������� */

       Velo_Matrix.LoadZero   (3, 1) ;
       Velo_Matrix.SetCell    (2, 0, object->v_abs) ;
        Sum_Matrix.Load3d_azim(-object->state.azim) ;
       Oper_Matrix.Load3d_elev(-object->state.elev) ;
        Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
       Velo_Matrix.LoadMul    (&Sum_Matrix, &Velo_Matrix) ;

         object->state.x_velocity=Velo_Matrix.GetCell(0, 0) ;
         object->state.y_velocity=Velo_Matrix.GetCell(1, 0) ;
         object->state.z_velocity=Velo_Matrix.GetCell(2, 0) ;

/*---------------------------------------------- ������� �� �������� */

   for(i=0 ; i<object->Features_cnt ; i++)
     object->Features[i]->vBodyAngles(NULL, object->state.azim, 
                                            object->state.elev, 
                                            object->state.roll ) ;

/*------------------------------------------------------ ����������� */

                this->kernel->vShow(NULL) ;

            object->state_0=object->state ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� VELOCITY                */
/*								    */
/*       VELOCITY <���> <��������>                                  */
/*       VELOCITY <���> <X-��������> <Y-��������> <Z-��������>      */

  int  RSS_Module_Missile::cVelocity(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

               char  *pars[_PARS_MAX] ;
               char  *name ;
               char **xyz ;
             double   coord[_COORD_MAX] ;
                int   coord_cnt ;
 RSS_Object_Missile  *object ;
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

       object=(RSS_Object_Missile *)FindObject(name, 1) ;           /* ���� ������ �� ����� */
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

                 Sum_Matrix.Load3d_azim(-object->state.azim) ;
                Oper_Matrix.Load3d_elev(-object->state.elev) ;
                 Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
                Velo_Matrix.LoadMul    (&Sum_Matrix, &Velo_Matrix) ;

                       object->state.x_velocity=Velo_Matrix.GetCell(0, 0) ;
                       object->state.y_velocity=Velo_Matrix.GetCell(1, 0) ;
                       object->state.z_velocity=Velo_Matrix.GetCell(2, 0) ;
                    }
/*- - - - - - - - - - - - - - - - - - - - - - - -  �������� �������� */
   else             {
                       object->state.x_velocity=coord[0] ;
                       object->state.y_velocity=coord[1] ;
                       object->state.z_velocity=coord[2] ;

                       object->v_abs     =sqrt(object->state.x_velocity*object->state.x_velocity+
                                               object->state.y_velocity*object->state.y_velocity+
                                               object->state.z_velocity*object->state.z_velocity ) ;
                    }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
            object->state_0=object->state ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� CONTROL                 */
/*								    */
/*       CONTROL   <���> <���� �����> [<����������>]                */
/*       CONTROL/G <���> <����������>                               */
/*       CONTROL>  <���> <��� ���������>                            */
/*       CONTROL>> <���> <��� ���������>                            */

  int  RSS_Module_Missile::cControl(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

               char  *pars[_PARS_MAX] ;
               char  *name ;
               char **xyz ;
             double   coord[_COORD_MAX] ;
                int   coord_cnt ;
             double   inverse ;
 RSS_Object_Missile  *object ;
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

       object=(RSS_Object_Missile *)FindObject(name, 1) ;           /* ���� ������ �� ����� */
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

          if(xyz_flag=='G')   object->g_ctrl    +=inverse*this->g_step ;
     else if(xyz_flag=='R')   object->state.roll+=inverse*this->a_step ;
                  }
/*- - - - - - - - - - - - - - - - - - - - - - -  ���������� �������� */
   else           {
                               object->state.roll=coord[0] ;
              if(coord_cnt>1)  object->g_ctrl    =coord[1] ;
                  }
/*- - - - - - - - - - - - - - - - - - - - - -  ������������ �������� */
     while(object->state.roll> 180.)  object->state.roll-=360. ;
     while(object->state.roll<-180.)  object->state.roll+=360. ;

            object->state_0=object->state ;

/*---------------------------------------------- ������� �� �������� */

   for(i=0 ; i<object->Features_cnt ; i++)
     object->Features[i]->vBodyAngles(NULL, object->state.azim, 
                                            object->state.elev, 
                                            object->state.roll ) ;

/*------------------------------------------------------ ����������� */

                this->kernel->vShow(NULL) ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� HOMING                  */
/*								    */
/*       HOMING/G <���>                                             */
/*       HOMING/P <���> <�����������>                               */

  int  RSS_Module_Missile::cHoming(char *cmd)

{
#define   _PARS_MAX  10

                char  *pars[_PARS_MAX] ;
                char  *name ;
  RSS_Object_Missile  *object ;
                 int   a_flag, p_flag ;
                char  *end ;
              double   value_d ;
                char  *error ;
                 int   i ;

/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
                      a_flag=0 ;
                      p_flag=0 ;

       if(*cmd=='/' ||
          *cmd=='+'   ) {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("������������ ������ �������") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, 'a')!=NULL ||
                   strchr(cmd, 'A')!=NULL   )  a_flag=1 ;
           else if(strchr(cmd, 'p')!=NULL ||
                   strchr(cmd, 'P')!=NULL   )  p_flag=1 ;

                           cmd=end+1 ;
                        }

       if(a_flag+p_flag>1) {
              SEND_ERROR("������ ���� ������ ���� �� ������: A ��� P. \n"
                         "��������: HOMING/A <���_�������> ...") ;
                                return(-1) ;
                           }

       if(a_flag+p_flag<1) {
              SEND_ERROR("������ ���� ����� ������ ���� �� ������: A ��� P. \n"
                         "��������: HOMING/A <���_�������> ...") ;
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
                                 "��������: HOMING/A <���_�������> ...") ;
                                     return(-1) ;
                   }

       object=(RSS_Object_Missile *)FindObject(name, 1) ;           /* ���� ������ �� ����� */
    if(object==NULL)  return(-1) ;

/*--------------------------------------------------- ������� ������ */

    if(a_flag) {
                  object->homing_type=_AHEAD_HOMING ;
               }
    else 
    if(p_flag) {

      if(pars[1]==NULL) {
                          SEND_ERROR("������ ���� ����� �����������: HOMING/P <���> <�����������>") ;
                                             return(-1) ;
                        }

         value_d=strtod(pars[1], &end) ;

                         error=  NULL ;
      if(value_d<1. &&
         value_d>6.   )  error="�������� ��������� ������ ���� �� 1 �� 6" ;

      if(error!=NULL) {
                         SEND_ERROR(error) ;
                            return(-1) ;
                      }

                         object->homing_type=_PROPORTIONAL_HOMING ;
                         object->homing_koef= value_d ;

               }
/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� MARK                    */
/*								    */
/*       MARK <���> <������ ����� ������>                           */

  int  RSS_Module_Missile::cMark(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

               char  *pars[_PARS_MAX] ;
               char  *name ;
               char **xyz ;
             double   coord[_COORD_MAX] ;
                int   coord_cnt ;
 RSS_Object_Missile  *object ;
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

/*------------------------------------------- �������� ����� ������� */

    if(name==NULL) {                                                /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� �������. \n"
                                 "��������: MARK <���_�������> ...") ;
                                     return(-1) ;
                   }

       object=(RSS_Object_Missile *)FindObject(name, 1) ;           /* ���� ������ �� ����� */
    if(object==NULL)  return(-1) ;

/*------------------------------------------------- ������ ��������� */

    for(i=0 ; xyz[i]!=NULL && i<_COORD_MAX ; i++) {

             coord[i]=strtod(xyz[i], &end) ;
        if(*end!=0) {  
                       SEND_ERROR("������������ �������� �������") ;
                                       return(-1) ;
                    }
                                                  }

                             coord_cnt=i ;

                        error= NULL ;
      if(coord_cnt==0)  error="�� ������ ������ ����� ������" ;

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

/*------------------------------------------------- ������� �������� */

             object->mark_hit=coord[0] ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� TRACE                   */
/*								    */
/*       TRACE <���> [<������������>]                               */

  int  RSS_Module_Missile::cTrace(char *cmd)

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
 RSS_Object_Missile *object ;
                int  c_flag ;
                int  o_flag ;
                int  w_flag ;
                int  color_r, color_g, color_b ;
                int  width ;
           Matrix2d  Sum_Matrix ;
           Matrix2d  Oper_Matrix ;  
           Matrix2d  Velo_Matrix ;  
               char *end ;
                int  i ;

/*---------------------------------------- �������� ��������� ������ */

                     trace_time=0. ;
/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
                             c_flag=0 ;
                             o_flag=0 ;
                             w_flag=0 ;

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

                if(strchr(cmd, 'o')!=NULL ||
                   strchr(cmd, 'o')!=NULL   )  o_flag=1 ;

                if(strchr(cmd, 'w')!=NULL ||
                   strchr(cmd, 'W')!=NULL   )  w_flag=1 ;

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
           if(o_flag)            {
                                 }
           else
           if(w_flag)            {
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

       object=(RSS_Object_Missile *)FindObject(name, 1) ;           /* ���� ������ �� ����� */
    if(object==NULL)  return(-1) ;

/*----------------------------------------------- ���� ������� ����� */

   if(c_flag) {

     if(pars[3]==NULL) {
                          SEND_ERROR("������ �������: TRACE/C <���> <R> <G> <B>") ;
                                       return(-1) ;
                       }

        color_r=strtoul(pars[1], &end, 10) ;
     if(*end!=0 || color_r>255) {
                     SEND_ERROR("��������� ����� <R> ������ ���� ������ 0...255") ;
                                       return(-1) ;
                                }

        color_g=strtoul(pars[2], &end, 10) ;
     if(*end!=0 || color_g>255) {
                     SEND_ERROR("��������� ����� <G> ������ ���� ������ 0...255") ;
                                       return(-1) ;
                                }

        color_b=strtoul(pars[3], &end, 10) ;
     if(*end!=0 || color_b>255) {
                     SEND_ERROR("��������� ����� <B> ������ ���� ������ 0...255") ;
                                       return(-1) ;
                                }

          object->mTrace_color=RGB(color_r, color_g, color_b) ;

                                       return(0) ;
              }

   if(o_flag) {

     if(pars[3]==NULL) {
                          SEND_ERROR("������ �������: TRACE/O <���> <R> <G> <B>") ;
                                       return(-1) ;
                       }

        color_r=strtoul(pars[1], &end, 10) ;
     if(*end!=0 || color_r>255) {
                     SEND_ERROR("��������� ����� <R> ������ ���� ������ 0...255") ;
                                       return(-1) ;
                                }

        color_g=strtoul(pars[2], &end, 10) ;
     if(*end!=0 || color_g>255) {
                     SEND_ERROR("��������� ����� <G> ������ ���� ������ 0...255") ;
                                       return(-1) ;
                                }

        color_b=strtoul(pars[3], &end, 10) ;
     if(*end!=0 || color_b>255) {
                     SEND_ERROR("��������� ����� <B> ������ ���� ������ 0...255") ;
                                       return(-1) ;
                                }

          object->mTrace_color_over=RGB(color_r, color_g, color_b) ;

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
/*----------------------------------------- �������� �������� � ���� */

  if(object->owner[0]!=0) {
       object->o_owner=FindObject(object->owner, 0) ;               /* ���� �������� �� ����� */
    if(object->o_owner==NULL)  return(-1) ;
                          }

  if(object->target[0]!=0) {
       object->o_target=FindObject(object->target, 0) ;             /* ���� ���� �� ����� */
    if(object->o_target==NULL)  return(-1) ;
                           }
/*------------------------------ �������� ��������� ����� � �������� */

  if(object->owner[0]!=0) {

       object->state.x=object->o_owner->state.x ;
       object->state.y=object->o_owner->state.y ;
       object->state.z=object->o_owner->state.z ;

       object->state.azim=object->o_owner->state.azim ;
       object->state.elev=object->o_owner->state.elev ;
       object->state.roll=object->o_owner->state.roll ;

       Velo_Matrix.LoadZero   (3, 1) ;
       Velo_Matrix.SetCell    (2, 0, object->v_abs) ;
        Sum_Matrix.Load3d_azim(-object->state.azim) ;
       Oper_Matrix.Load3d_elev(-object->state.elev) ;
        Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
       Velo_Matrix.LoadMul    (&Sum_Matrix, &Velo_Matrix) ;

         object->state.x_velocity=Velo_Matrix.GetCell(0, 0) ;
         object->state.y_velocity=Velo_Matrix.GetCell(1, 0) ;
         object->state.z_velocity=Velo_Matrix.GetCell(2, 0) ;

                          }

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
      if(object->o_target!=NULL)  object->o_target->state_0=object->o_target->state ; 
                                  object          ->state_0=object          ->state ; 

         object->vCalculate(time_c-RSS_Kernel::calc_time_step, time_c, NULL, 0) ;
         object->iSaveTracePoint("ADD") ;
/*- - - - - - - - - - - - - - - - - - - - - - ����������� ���������� */
         object->iShowTrace_() ;
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
       } while(1) ;                                                 /* END CIRCLE.1 - ���� ����������� */

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*								    */
/*		   ����� ������� ���� MISSILE �� �����              */

  RSS_Object *RSS_Module_Missile::FindObject(char *name, int  check_type)

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
     if(strcmp(OBJECTS[i]->Type, "Missile")) {

           SEND_ERROR("������ �� �������� �������� ���� MISSILE") ;
                            return(NULL) ;
                                             }
/*-------------------------------------------------------------------*/ 

   return(OBJECTS[i]) ;
  
#undef   OBJECTS
#undef   OBJECTS_CNT

}


/********************************************************************/
/********************************************************************/
/**							           **/
/**		  �������� ������ ������� "������"	           **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       ����������� ������			    */

     RSS_Object_Missile::RSS_Object_Missile(void)

{
   strcpy(Type, "Missile") ;

    Context        =new RSS_Transit_Missile ;
    Context->object=this ;

     Parameters    =  NULL ;
     Parameters_cnt=    0 ;

       battle_state=    0 ;

         v_abs     =    0. ;
         g_ctrl    =  100. ;
    homing_type    =_AHEAD_HOMING ;
       mark_hit    =    0. ;

       mTrace           =NULL ;
       mTrace_cnt       =  0 ;  
       mTrace_max       =  0 ; 
       mTrace_color     =RGB(  0, 0, 127) ;
       mTrace_color_over=RGB(255, 0,   0) ;
       mTrace_width     =  1 ;  
       mTrace_dlist     =  0 ;  
}


/********************************************************************/
/*								    */
/*		        ���������� ������			    */

    RSS_Object_Missile::~RSS_Object_Missile(void)

{
      vFree() ;

   delete Context ;
}


/********************************************************************/
/*								    */
/*		       ������������ ��������                        */

  void   RSS_Object_Missile::vFree(void)

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
/*								    */
/*                        ���������� ������		            */

    class RSS_Object *RSS_Object_Missile::vCopy(char *name)

{
         RSS_Model_data  create_data ;
     RSS_Object_Missile *object ;
        RSS_Feature_Hit *hit_1 ;
        RSS_Feature_Hit *hit_2 ;
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

       object=(RSS_Object_Missile *)this->Module->vCreateObject(&create_data) ;
    if(object==NULL)  return(NULL) ;
 
            strcpy(object->owner,  this->owner) ;
                   object->o_owner=this->o_owner ;

                   object->v_abs            =this->v_abs ;          /* ���������� �������� */
                   object->g_ctrl           =this->g_ctrl ;         /* ���������� ����������� ���������� */
                   object->mark_hit         =this->mark_hit ;       /* ������ ������� ������ ��� ��������� */
                   object->homing_type      =this->homing_type ;    /* ��������� ������������� */
                   object->homing_koef      =this->homing_koef ;
                   object->mTrace_color     =this->mTrace_color ;   /* ��������� ������ */
                   object->mTrace_color_over=this->mTrace_color_over ;
                   object->mTrace_width     =this->mTrace_width ;

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

/*-------------------------------------------------------------------*/

   return(object) ;
}


/********************************************************************/
/*								    */
/*                    ��������� ��������� �������                   */
/*                    ������������ ��������� �������                */

    void  RSS_Object_Missile::vPush(void)

{
     state_stack=state ;
}


    void  RSS_Object_Missile::vPop(void)

{
     state=state_stack ;

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
/*		        �������� ������ � ������		    */

    void  RSS_Object_Missile::vWriteSave(std::string *text)

{
  char  field[1024] ;
   int  i ;

/*----------------------------------------------- ��������� �������� */

     *text="#BEGIN OBJECT MISSILE\n" ;

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
/*                        ��������� �������                         */

     int  RSS_Object_Missile::vEvent(char *event_name, double  t, char *callback, int cb_size)
{
     char  text[1024] ;

/*--------------------------------------------------- ��������� ���� */

   if(!stricmp(event_name, "HIT")) {

     if(this->mark_hit>0.)
      if(callback!=NULL) {
                            sprintf(text, "EXEC BLAST CR/A blast_%s %lf %lf %s;"
                                          "START blast_%s;",
                                    this->Name, 0., this->mark_hit, this->Name,
                                    this->Name
                                   ) ;
                            strncat(callback, text, cb_size) ;
                         }

                                         return(0) ;
                                   }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*                        ����������� ��������                      */

     int  RSS_Object_Missile::vSpecial(char *oper, void *data)
{
  return(-1) ;
}


/********************************************************************/
/*								    */
/*             ���������� ������� ��������� ���������               */

     int  RSS_Object_Missile::vCalculateStart(double  t)
{
    Matrix2d  Sum_Matrix ;
    Matrix2d  Oper_Matrix ;  
    Matrix2d  Velo_Matrix ;  
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

      this->state.x   =this->o_owner->state_0.x ;
      this->state.y   =this->o_owner->state_0.y ;
      this->state.z   =this->o_owner->state_0.z ;

      this->state.azim=this->o_owner->state_0.azim ;
      this->state.elev=this->o_owner->state_0.elev ;
      this->state.roll=this->o_owner->state_0.roll ;
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
        Sum_Matrix.Load3d_azim(-this->state.azim) ;
       Oper_Matrix.Load3d_elev(-this->state.elev) ;
        Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
       Velo_Matrix.LoadMul    (&Sum_Matrix, &Velo_Matrix) ;

         this->state.x_velocity=Velo_Matrix.GetCell(0, 0) ;
         this->state.y_velocity=Velo_Matrix.GetCell(1, 0) ;
         this->state.z_velocity=Velo_Matrix.GetCell(2, 0) ;

/*------------------------------------------------ ������� ��������� */

            xyz_trg_prv=0 ;
              t_lost   =0. ;

    this->iSaveTracePoint("CLEAR") ;

#undef   OBJECTS
#undef   OBJECTS_CNT

  return(0) ;
}


/********************************************************************/
/*								    */
/*                   ������ ��������� ���������                     */

     int  RSS_Object_Missile::vCalculate(double t1, double t2, char *callback, int cb_size)
{
    double  tx_1, tx_2 ;
    double  dx, dy, dz ;         /* ������ �� ���� */
    double  s ;                  /* ���������� �� ���� */
    double  r ;                  /* ������ ������� */
    double  h ;
    double  ds ;
    double  q ;                  /* ����������� ���� ������� */
    double  a, b, c ;
    double  dv_max ;             /* ������������ ��������� ����� ������� ��������� �������� */
    double  dv_x, dv_y, dv_z ;   /* ��������� ��������� �������� */
    double  dv_s ;               /* ����� ������� ���������� ��������� �������� */
    double  pv_x, pv_y, pv_z ;   /* ���������� ������ ��������  */
    double  x1, y1, z1 ;
    double  x2, y2, z2 ;
    double  s1, s2 ;
       int  prepare ;
      char  command[128] ;       /* ������� �������� ����� */ 

/*------------------------------------------------------- ���������� */

                     pv_x=state.x_velocity ;
                     pv_y=state.y_velocity ;
                     pv_z=state.z_velocity ;

/*-------------------- ������ ����������� ��������� ������� �������� */

                        r=v_abs*v_abs/g_ctrl ;                      /* ������������ ������ ��������� */
                        q=v_abs*(t2-t1)/(2.*r) ;
                   dv_max=2.*v_abs*sin(0.5*q) ;

/*---------------------------------------------------- ��� ��������� */

   if(this->o_target==NULL) {
                               state.x+=state.x_velocity*(t2-t1) ;
                               state.y+=state.y_velocity*(t2-t1) ;
                               state.z+=state.z_velocity*(t2-t1) ;
                                g_over = 0 ;
                            }
/*------------------------------------ ��������� � ����������� ����� */

   else
   if(this->homing_type==_AHEAD_HOMING) {

               dx=o_target->state_0.x-this->state_0.x ;
               dy=o_target->state_0.y-this->state_0.y ;
               dz=o_target->state_0.z-this->state_0.z ;
/*- - - - - - - - - - - - - - - - - - - - - ��������� ������ ������� */
       if(xyz_trg_prv==0) {
                            state_0.x+=state_0.x_velocity*(t2-t1) ;
                            state_0.y+=state_0.y_velocity*(t2-t1) ;
                            state_0.z+=state_0.z_velocity*(t2-t1) ;
                               g_over = 0 ;

                           xyz_trg_prv= 1 ;

                                return(0) ; 
                          }   
/*- - - - - - - - - - - - ������ ����� ��������� � ����������� ����� */
                a=o_target->state_0.x_velocity*o_target->state_0.x_velocity
                 +o_target->state_0.y_velocity*o_target->state_0.y_velocity
                 +o_target->state_0.z_velocity*o_target->state_0.z_velocity
                 -this->state_0.x_velocity*this->state_0.x_velocity
                 -this->state_0.y_velocity*this->state_0.y_velocity
                 -this->state_0.z_velocity*this->state_0.z_velocity ;
                b=2.*(o_target->state_0.x_velocity*dx+o_target->state_0.y_velocity*dy+o_target->state_0.z_velocity*dz) ;
                c=dx*dx+dy*dy+dz*dz ;

             tx_1=(-b-sqrt(b*b-4.*a*c))/(2.*a) ;
             tx_2=(-b+sqrt(b*b-4.*a*c))/(2.*a) ;

              if(tx_1<0. && tx_2<0.)  tx_1=   0. ;
         else if(tx_1<0.           )  tx_1=tx_2 ;
         else if(tx_2<0.           )   ;
         else if(tx_1>tx_2         )  tx_1=tx_2 ;

                       dx+=o_target->state_0.x_velocity*tx_1 ;      /* ������ � ����������� ����� */
                       dy+=o_target->state_0.y_velocity*tx_1 ;
                       dz+=o_target->state_0.z_velocity*tx_1 ;
                        s =sqrt(dx*dx+dy*dy+dz*dz) ;
/*- - - - - - - - - - - ������ ���������� ��������� ������� �������� */
                       dx=dx*v_abs/s ;                              /* ��������� ������ �� ���� �� �������� */
                       dy=dy*v_abs/s ;
                       dz=dz*v_abs/s ;

                     dv_x=dx-state_0.x_velocity ;                   /* ��������� ������ ��������� �������� */
                     dv_y=dy-state_0.y_velocity ;
                     dv_z=dz-state_0.z_velocity ;
                     dv_s=sqrt(dv_x*dv_x+dv_y*dv_y+dv_z*dv_z) ;
/*- - - - - - - - - - - �������� ����� �� ���� ��������� 90 �������� */
     if(dv_s>1.4*v_abs) {

           if(t_lost==0.)  t_lost=t1 ;                              /* ��������� ����� ������ ���� */
                                    
           if(t1-t_lost > 3.) {                                     /* ���� � ������� ������ ���� ������ 3 ������� */

             if(callback!=NULL) {                                   /*  ���� ������� �� "���������������" ������  */ 

                  sprintf(command, "KILL %s;", this->Name) ;     

               if(strlen(callback)+
                  strlen(command ) < cb_size)  strcat(callback, command) ;
                                }

                              }

                               state.x+=state_0.x_velocity*(t2-t1) ;
                               state.y+=state_0.y_velocity*(t2-t1) ;
                               state.z+=state_0.z_velocity*(t2-t1) ;
                                g_over = 0 ;

                                   return(0) ;
                        }
/*- - - - - - - - - - - - - - - - - - ������ ������ ������� �������� */
     if(dv_s>dv_max) {                                              /* ���� ��������� ��������� �� ����� ���� ���������� �� ����������... */
                            b=asin(0.5*dv_s/v_abs) ;
                            h=sqrt(v_abs*v_abs-0.25*dv_s*dv_s) ;

                           ds=h*tan(q-b)+0.5*dv_s ;

                   state.x_velocity+=dv_x*ds/dv_s ;
                   state.y_velocity+=dv_y*ds/dv_s ;
                   state.z_velocity+=dv_z*ds/dv_s ;
                               dv_s =sqrt(state.x_velocity*state.x_velocity+
                                          state.y_velocity*state.y_velocity+
                                          state.z_velocity*state.z_velocity ) ;
                   state.x_velocity*=v_abs/dv_s ;
                   state.y_velocity*=v_abs/dv_s ;
                   state.z_velocity*=v_abs/dv_s ;
                             g_over = 1 ;
                     }
     else            {
                         state.x_velocity =dx ;
                         state.y_velocity =dy ;
                         state.z_velocity =dz ;
                                   g_over = 0 ;
                     }
/*- - - - - - - - - - - - - - - - - - - - -  ��������� ������� ����� */
             state.x+=0.5*(state.x_velocity+pv_x)*(t2-t1) ;
             state.y+=0.5*(state.y_velocity+pv_y)*(t2-t1) ;
             state.z+=0.5*(state.z_velocity+pv_z)*(t2-t1) ;
/*- - - - - - - - - - - - - - - - - - - - ��������� ����� ���������� */
             state.azim=atan2(state.x_velocity, state.z_velocity)*_RAD_TO_GRD ;
             state.elev=atan2(state.y_velocity, sqrt(state.x_velocity*state.x_velocity+state.z_velocity*state.z_velocity))*_RAD_TO_GRD ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                        }
/*--------------------------------------- ���������������� ��������� */

   else
   if(this->homing_type==_PROPORTIONAL_HOMING) {

               dx=o_target->state_0.x-this->state_0.x ;
               dy=o_target->state_0.y-this->state_0.y ;
               dz=o_target->state_0.z-this->state_0.z ;
/*- - - - - - - - - - - - - - - - - - - - - ��������� ������ ������� */
       if(xyz_trg_prv==0) {
                            state_0.x+=state_0.x_velocity*(t2-t1) ;
                            state_0.y+=state_0.y_velocity*(t2-t1) ;
                            state_0.z+=state_0.z_velocity*(t2-t1) ;
                               g_over = 0 ;

                           xyz_trg_prv= 1 ;
                             x_trg_prv=dx ;
                             y_trg_prv=dy ;
                             z_trg_prv=dz ;
                  
                                return(0) ; 
                          }   
/*- - - - - - - - - - - �������� ����� �� ���� ��������� 90 �������� */
             s =v_abs / sqrt(dx*dx+dy*dy+dz*dz) ;

          dv_x =dx*s ;                                              /* ��������� ������ �� ���� �� �������� */
          dv_y =dy*s ;
          dv_z =dz*s ;

          dv_x-=state_0.x_velocity ;                                /* ���������� ������� �������� �������� � ����������� �� ���� */
          dv_y-=state_0.y_velocity ;
          dv_z-=state_0.z_velocity ;

          dv_s =sqrt(dv_x*dv_x+dv_y*dv_y+dv_z*dv_z) ;
       if(dv_s>1.4*v_abs) {                                         /* ���� ���� ����� ��������� �� ���� � �������� �������� >90 ��������... */
                               state.x+=state.x_velocity*(t2-t1) ;
                               state.y+=state.y_velocity*(t2-t1) ;
                               state.z+=state.z_velocity*(t2-t1) ;
                                g_over = 0 ;

                                   return(0) ;
                          }
/*- - - - - - - - - - - - - -  ����������� ��������� ������� �� ���� */
             s=sqrt(dx*dx+dy*dy+dz*dz) / sqrt(x_trg_prv*x_trg_prv+y_trg_prv*y_trg_prv+z_trg_prv*z_trg_prv) ;

          dv_x=dx-x_trg_prv*s ;
          dv_y=dy-y_trg_prv*s ;
          dv_z=dz-z_trg_prv*s ;

            b =2.*asin( 0.5*sqrt(dv_x*dv_x+dv_y*dv_y+dv_z*dv_z) / sqrt(dx*dx+dy*dy+dz*dz) ) ;
            b*=this->homing_koef ; 

                 g_over = 0 ;

       if(b>q) {                                                    /* ���� ��������� ���������� ��������� ������������� - ���������� ������������� */
                      b = q ;
                 g_over = 1 ;
               }
/*- - - - - - - - - - - - - - - - - - ������ ������ ������� �������� */
                 r=v_abs / sqrt(dv_x*dv_x+dv_y*dv_y+dv_z*dv_z) ;
                 s= 1. ;
                ds= 0.5 ;
           prepare= 1 ;

    do {
             x1=dv_x*r*s ;
             y1=dv_y*r*s ;
             z1=dv_z*r*s ;

             x2=state.x_velocity+x1 ;
             y2=state.y_velocity+y1 ;
             z2=state.z_velocity+z1 ;

             s1=sqrt(x1*x1+y1*y1+z1*z1) ;
             s2=sqrt(x2*x2+y2*y2+z2*z2) ;
             
             c =acos( (v_abs*v_abs+s2*s2-s1*s1)/(2.*v_abs*s2) ) ;

          if(fabs(c-b)<0.001*b )  break ;                           /* ����� ������� �� 0.001 �� �������� ���� */

          if(c>b) {  s-=ds ;  prepare=0 ; }
          else    {  s+=ds ;              }

          if(prepare==0)  ds*=0.5 ;

       } while(1) ;

                          r=v_abs / sqrt(x2*x2+y2*y2+z2*z2)  ;
           state.x_velocity=x2*r ;
           state.y_velocity=y2*r ;
           state.z_velocity=z2*r ;
/*- - - - - - - - - - - - - - - - - - - - -  ��������� ������� ����� */
           state.x+=0.5*(state.x_velocity+pv_x)*(t2-t1) ;
           state.y+=0.5*(state.y_velocity+pv_y)*(t2-t1) ;
           state.z+=0.5*(state.z_velocity+pv_z)*(t2-t1) ;
/*- - - - - - - - - - - - - - - - - - - - ��������� ����� ���������� */
           state.azim=atan2(state.x_velocity, state.z_velocity)*_RAD_TO_GRD ;
           state.elev=atan2(state.y_velocity, sqrt(state.x_velocity*state.x_velocity+state.z_velocity*state.z_velocity))*_RAD_TO_GRD ;
/*- - - - - - - - - - - - - - - - - - - - - - -  ���������� �������� */
                    x_trg_prv=dx ;
                    y_trg_prv=dy ;
                    z_trg_prv=dz ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                                               }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*      ����������� ���������� ������� ��������� ���������          */

     int  RSS_Object_Missile::vCalculateShow(double  t1, double  t2)
{
   int i ;


         this->iSaveTracePoint("ADD") ;                             /* ���������� ����� ���������� */  

         this->iShowTrace_() ;                                      /* ����������� ���������� */

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

  int  RSS_Object_Missile::iSaveTracePoint(char *action)

{
/*------------------------------------------------- ����� ���������� */

   if(!stricmp(action, "CLEAR")) {
                                     mTrace_cnt=0 ;
                                       return(0) ;
                                 }
/*----------------------------------------------- ����������� ������ */

   if(mTrace_cnt==mTrace_max) {

          mTrace_max+= 1000 ;
          mTrace     =(RSS_Object_MissileTrace *)
                            realloc(mTrace, mTrace_max*sizeof(RSS_Object_MissileTrace)) ;

       if(mTrace==NULL) {
                   SEND_ERROR("MISSILE.iSaveTracePoint@"
                              "Memory over for trajectory") ;
                                  return(-1) ;
                        }
                              }
/*------------------------------------------------- ���������� ����� */
                  
                  mTrace[mTrace_cnt].x_base    =this->state.x ;
                  mTrace[mTrace_cnt].y_base    =this->state.y ;
                  mTrace[mTrace_cnt].z_base    =this->state.z ;
                  mTrace[mTrace_cnt].a_azim    =this->state.azim ;
                  mTrace[mTrace_cnt].a_elev    =this->state.elev ;
                  mTrace[mTrace_cnt].a_roll    =this->state.roll ;
                  mTrace[mTrace_cnt].x_velocity=this->state.x_velocity ;
                  mTrace[mTrace_cnt].y_velocity=this->state.y_velocity ;
                  mTrace[mTrace_cnt].z_velocity=this->state.z_velocity ;

      if(g_over)  mTrace[mTrace_cnt].color     =mTrace_color_over ;
      else        mTrace[mTrace_cnt].color     =mTrace_color ;

                         mTrace_cnt++ ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*           ����������� ���������� � ��������� ���������           */

  void  RSS_Object_Missile::iShowTrace_(void)

{
    strcpy(Context->action, "SHOW_TRACE") ;

  SendMessage(RSS_Kernel::kernel_wnd, 
                WM_USER, (WPARAM)_USER_CHANGE_CONTEXT, 
                         (LPARAM) this->Context       ) ;
}


/*********************************************************************/
/*								     */
/*                     ����������� ����������                        */

  void  RSS_Object_Missile::iShowTrace(void)

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

                     }
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

     RSS_Transit_Missile::RSS_Transit_Missile(void)

{
}


/*********************************************************************/
/*								     */
/*	        ���������� ������ "������� ���������"      	     */

    RSS_Transit_Missile::~RSS_Transit_Missile(void)

{
}


/********************************************************************/
/*								    */
/*	              ���������� ��������                           */

    int  RSS_Transit_Missile::vExecute(void)

{
   if(!stricmp(action, "SHOW_TRACE"))  ((RSS_Object_Missile *)object)->iShowTrace() ;

   return(0) ;
}

/*********************************************************************/
/*								     */
/*        ������ ���������� ��������� � ������� ���������            */
/*								     */
/*********************************************************************/


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
#include "..\RSS_Kernel\RSS_Kernel.h"
#include "..\F_Show\F_Show.h"

#include "F_Terrain.h"

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


/*********************************************************************/
/*								     */
/*		    	����������� ������                           */

     static   RSS_Module_Terrain  ProgramModule ;


/*********************************************************************/
/*								     */
/*		    	����������������� ����                       */

 F_TERRAIN_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 F_TERRAIN_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/*********************************************************************/
/*********************************************************************/
/**							            **/
/**               �������� ������ ������ ����������                 **/
/**	           ��������� � ������� ���������	            **/
/**							            **/
/*********************************************************************/
/*********************************************************************/

/*********************************************************************/
/*								     */
/*                            ������ ������                          */

  struct RSS_Module_Terrain_instr  RSS_Module_Terrain_InstrList[]={

 { "help",    "?",  "#HELP   - ������ ��������� ������", 
                      NULL,
                     &RSS_Module_Terrain::cHelp      },
 { "slave",    "sl", "#SLAVE (SL) - ������� �������� ������� ", 
                     " SLAVE <���>\n"
                     "   ������ ������ ��� ��������� ������� ���������\n",
                     &RSS_Module_Terrain::cSlave     },
 { "flat",     "f",  "#FLAT (F) - ������� �������� ������� ", 
                     " FLAT <������� ���> <X-base> <Z-base> <X-size> <Z-size> <step>\n"
                     "   ������� ������� ����� ������� � �������� �����\n",
                     &RSS_Module_Terrain::cFlat      },
 { "link",     "l",  "#LINK (L) - '��������' ������ �� ��������� ����� �������� ���������", 
                     " LINK <��� ������� ���������> <��� �������>\n"
                     "   '��������' ������ �� ��������� ����� �������� ���������\n"
                     " LINK+ <��� ������� ���������> <��� �������>\n"
                     "   ... �����, ��������� �� ��������� ������ �������� ���������\n"
                     " LINK- <��� ������� ���������> <��� �������>\n"
                     "   ... �����, �������������� ��������� ����� �������� ���������\n",
                     &RSS_Module_Terrain::cLink      },
 { "scan",     "s",  "#SCAN (S) - ������� ����� �������� ���������", 
                     " SCAN+ <��� ������� ���������>\n"
                     "   ������� � ��������� ����� �������� ���������\n"
                     " SCAN- <��� ������� ���������> <��� �������>\n"
                     "   ������� � ���������� ����� �������� ���������\n",
                     &RSS_Module_Terrain::cScan      },
 { "up",       "u",  "#UP (U) - �������� ������ ��������� ����������� �����", 
                     " UP <��� ������� ���������> <������>\n"
                     "   ���������� ���������� �������� ������ ����� �������� ���������\n"
                     " UP+ <��� ������� ���������> <��������� ������>\n"
                     "   �������� ������ ����� �������� ���������\n",
                     &RSS_Module_Terrain::cUp        },
 {  NULL }
                                                              } ;

/*********************************************************************/
/*								     */
/*		     ����� ����� ������             		     */

    struct RSS_Module_Terrain_instr *RSS_Module_Terrain::mInstrList=NULL ;


/*********************************************************************/
/*								     */
/*		       ����������� ������			     */

     RSS_Module_Terrain::RSS_Module_Terrain(void)

{
	   keyword="EmiStand" ;
    identification="Terrain" ;
          category="Feature" ;

        mInstrList=RSS_Module_Terrain_InstrList ;
}


/*********************************************************************/
/*								     */
/*		        ���������� ������			     */

    RSS_Module_Terrain::~RSS_Module_Terrain(void)

{
}


/*********************************************************************/
/* 								     */
/*		        ��������� ��������                           */

    void  RSS_Module_Terrain::vStart(void)

{
/*-------------------------------------------- ������������ �������� */

   feature_modules=(RSS_Kernel **)
                     realloc(feature_modules, 
                              (feature_modules_cnt+1)*sizeof(feature_modules[0])) ;

      feature_modules[feature_modules_cnt]=&ProgramModule ;
                      feature_modules_cnt++ ;

/*-------------------------------------------------------------------*/
}


/*********************************************************************/
/*								     */
/*		        ������� ��������                	     */

    RSS_Feature *RSS_Module_Terrain::vCreateFeature(RSS_Object *object, RSS_Feature *feature_ext)

{
  RSS_Feature *feature ;


    if(feature_ext!=NULL)  feature= feature_ext ;
    else                   feature=new RSS_Feature_Terrain ;

         feature->Object=object ;

  return(feature) ;
}


/*********************************************************************/
/*								     */
/*		        ��������� �������       		     */

  int  RSS_Module_Terrain::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* ���� ������ ������ ������� */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "TERRAIN"
#define  _SECTION_SHRT_NAME   "TER"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Terrain features:") ;
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
                     *end!='+' &&
                     *end!='-' &&
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
       if(status)  SEND_ERROR("������ TERRAIN: ����������� �������") ;
                                            return(0) ;
                                       }
 
     if(mInstrList[i].process!=NULL)                                /* ���������� ������� */
                status=(this->*mInstrList[i].process)(end) ;
     else       status=  0 ;

/*-------------------------------------------------------------------*/

   return(status) ;
}


/********************************************************************/
/*								    */
/*		        �������� ������ � ������		    */

    void  RSS_Module_Terrain::vWriteSave(std::string *text)

{

/*----------------------------------------------- ��������� �������� */

     *text="#BEGIN MODULE TERRAIN\n" ;

/*------------------------------------------------ �������� �������� */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/

#undef   MARKS_LIST
#undef   MARKS_LIST_CNT

}


/********************************************************************/
/*								    */
/*		        ������� ������ �� ������		    */

    void  RSS_Module_Terrain::vReadSave(std::string *data)

{
                    char *buff ;
                     int  buff_size ;
             std::string  decl ;
                    char *work ;
                    char  text[1024] ;
                    char *end ;

/*----------------------------------------------- �������� ��������� */

   if(memicmp(data->c_str(), "#BEGIN MODULE TERRAIN\n", 
                      strlen("#BEGIN MODULE TERRAIN\n")))  return ;

/*------------------------------------------------ ���������� ������ */

              buff_size=(int)data->size()+16 ;
              buff     =(char *)calloc(1, buff_size) ;

       strcpy(buff, data->c_str()) ;
        
/*---------------------------------------------- ���������� �������� */

                work=buff+strlen("#BEGIN MODULE TERRAIN\n") ;

    for( ; ; work=end+1) {                                          /* CIRCLE.0 */
                                      end=strchr(work, '\n') ;
                                   if(end==NULL)  break ;
                                     *end=0 ;

/*---------------------------------------------------------- ??????? */

          if(!memicmp(work, "??????", strlen("??????"))) {

                                                         }
/*----------------------------------------- ����������� ������������ */

     else                                                {

                sprintf(text, "Module TERRAIN: "
                              "����������� ������������ %20.20s", work) ;
             SEND_ERROR(text) ;
                                 break ;
                                                         }
/*---------------------------------------------- ���������� �������� */
                         }                                          /* CONTINUE.0 */
/*-------------------------------------------- ������������ �������� */

                free(buff) ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� Help                    */

  int  RSS_Module_Terrain::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Feature_Terrain_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� SLAVE                   */
/*								    */
/*        SLAVE <���>                                               */

  int  RSS_Module_Terrain::cSlave(char *cmd)

{
#define   _PARS_MAX   4

               char *pars[_PARS_MAX] ;
               char *name ;
         RSS_Object *object ;
               char  text[1024] ;
               char *end ;
                int  i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

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

/*------------------------------------------- ����� ������� �� ����� */

    if(name==NULL) {                                                /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� �������. \n"
                                 "��������: SLAVE <���_�������> ...") ;
                                     return(-1) ;
                   }

       for(i=0 ; i<OBJECTS_CNT ; i++)                               /* ���� ������ �� ����� */
         if(!stricmp(OBJECTS[i]->Name, name)) {
                                         object=OBJECTS[i] ;
                                                 break ;
                                              }

    if(i==OBJECTS_CNT) {                                            /* ���� ��� �� �������... */
                           sprintf(text, "������� � ������ '%s' "
                                         "�� ����������", name) ;
                        SEND_ERROR(text) ;
                            return(-1) ;
                       }
/*--------------------------------------------- ���������� ��������� */

   for(i=0 ; i<object->Features_cnt ; i++)
     if(!stricmp(object->Features[i]->Type, "Terrain")) {
          ((RSS_Feature_Terrain *)(object->Features[i]))->slave=1 ;
                                                        }
/*-------------------------------------------------------------------*/

#undef    OBJECTS
#undef    OBJECTS_CNT
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� CREATE                  */
/*								    */
/*  FLAT <������� ���> <X-base> <Z-base> <X-size> <Z-size> <step>   */

  int  RSS_Module_Terrain::cFlat(char *cmd)

{
#define  _PARS_MAX  10

          RSS_Object *object ;
    RSS_Feature_Show *show ; 
 RSS_Feature_Terrain *terrain ; 
                char *name ;
                char *pars[_PARS_MAX] ;
              double  x_base, z_base ;
              double  x_size, z_size ;
              double  step ;
                 int  i_x, i_z ;
                 int  x_cnt, z_cnt ;
                char *end ;
                char  text[1024] ;
                 int  n ;
                 int  i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*-------------------------------------- ���������� ��������� ������ */

     for(i=0 ; i<_PARS_MAX ; i++)  pars[i]="" ;

   do {                                                             /* BLOCK.1 */
                  name=cmd ;                                        /* ��������� ��� ������� */
                   end=strchr(name, ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
          
     for(i=0 ; i<_PARS_MAX ; i++) {                                 /* ��������� ��������� */
               pars[i]=end+1 ;            
                   end=strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                  }
      } while(0) ;                                                  /* BLOCK.1 */

/*--------------------------- �������� �������� ����� ����� �������� */

/*-------------------------------------------- ���������� ���������� */

          x_base=strtod(pars[0], &end) ;
      if(*end!=0) {
                        sprintf(text, "������������ ������ ��������� X-base") ;
                     SEND_ERROR(text) ;
                          return(-1) ;
                  }

          z_base=strtod(pars[1], &end) ;
      if(*end!=0) {
                        sprintf(text, "������������ ������ ��������� Z-base") ;
                     SEND_ERROR(text) ;
                          return(-1) ;
                  }

          x_size=strtod(pars[2], &end) ;
      if(*end!=0) {
                        sprintf(text, "������������ ������ ��������� X-size") ;
                     SEND_ERROR(text) ;
                          return(-1) ;
                  }

          z_size=strtod(pars[3], &end) ;
      if(*end!=0) {
                        sprintf(text, "������������ ������ ��������� Z-size") ;
                     SEND_ERROR(text) ;
                          return(-1) ;
                  }

            step=strtod(pars[4], &end) ;
      if(*end!=0) {
                        sprintf(text, "������������ ������ ��������� Step") ;
                     SEND_ERROR(text) ;
                          return(-1) ;
                  }
/*------------------------------------------------- �������� ������� */

           object=new RSS_Object ;
        if(object==NULL) {
              SEND_ERROR("������ TERRAIN: ������������ ������ ��� �������� �������") ;
                              return(-1) ;
                         }

               strcpy(object->Name, name) ;
               strcpy(object->Type, "Terrain") ;
                      object->Module=this ;
                      object->x_base=x_base ;  
                      object->y_base=0. ;  
                      object->z_base=z_base ;  
                      object->a_azim=0. ;  
                      object->a_elev=0. ;  
                      object->a_roll=0. ;  

                      object->land_state=1 ;

/*------------------------------------------------- �������� ������� */

               object->Features_cnt=this->feature_modules_cnt ;
               object->Features    =(RSS_Feature **)
                                     calloc(this->feature_modules_cnt, sizeof(object->Features[0])) ;

       for(i=0 ; i<this->feature_modules_cnt ; i++)
         object->Features[i]=this->feature_modules[i]->vCreateFeature(object, NULL) ;

                show=NULL ;
             terrain=NULL ;

       for(i=0 ; i<object->Features_cnt ; i++)
         if(!stricmp(object->Features[i]->Type, "Show"))
                      show=(RSS_Feature_Show *)object->Features[i] ;
         else
         if(!stricmp(object->Features[i]->Type, "Terrain"))
                   terrain=(RSS_Feature_Terrain *)object->Features[i] ;

/*----------------------------------------- ���������� �������� SHOW */

      if(show!=NULL) {

#define    BODY          show->Bodies
#define    BODIES_CNT    show->Bodies_cnt
#define    FACETS        show->Bodies->Facets
#define    FACETS_CNT    show->Bodies->Facets_cnt
#define  VERTEXES        show->Bodies->Vertexes
#define  VERTEXES_CNT    show->Bodies->Vertexes_cnt
/*- - - - - - - - - - - - - - - - - - - - - - - - - -  �������� ���� */
           BODIES_CNT=1 ;

           BODY=(RSS_Feature_Show_Body *)calloc(1, sizeof(RSS_Feature_Show_Body)) ;
        if(BODY==NULL) {
                            sprintf(text, "Section TARRAIN, feature SHOW: ������������ ������ ��� ������ ���") ;
                         SEND_ERROR(text) ;
                              return(-1) ;
                       }

                       strcpy(BODY->name, "Body.Body") ;
                              BODY->x_base=x_base ;
                              BODY->y_base=   0. ;
                              BODY->z_base=z_base ;
/*- - - - - - - - - - - - - - - - - - - - - - �������� ������ ������ */
                   x_cnt=(int)((x_size-step*0.5)/step+1) ;
                   z_cnt=(int)((z_size-step*0.5)/step+1) ;
            VERTEXES_CNT=(x_cnt+1)*(z_cnt+1)+1 ;

           VERTEXES=(RSS_Feature_Show_Vertex *)calloc(VERTEXES_CNT, sizeof(*VERTEXES)) ;
        if(VERTEXES==NULL) {
                              sprintf(text, "Section TARRAIN, feature SHOW: ������������ ������ ��� ������ ������") ;
                           SEND_ERROR(text) ;
                                return(-1) ;
                           }                    

                  n=1 ;                                             /* ��������! ������ ����� ������������� �� 1 */ 

         for(i_z=0 ; i_z<=z_cnt ; i_z++)
         for(i_x=0 ; i_x<=x_cnt ; i_x++) {

           if(i_x==x_cnt)  VERTEXES[n].x=x_size ;
           else            VERTEXES[n].x=i_x*step ;

           if(i_z==z_cnt)  VERTEXES[n].z=z_size ;
           else            VERTEXES[n].z=i_z*step ;

                                    n++ ;

                                         } 
/*- - - - - - - - - - - - - - - - - - - - - - �������� ������ ������ */
           FACETS_CNT=x_cnt*z_cnt*2 ;

           FACETS=(RSS_Feature_Show_Facet *)calloc(FACETS_CNT, sizeof(RSS_Feature_Show_Facet)) ;
        if(FACETS==NULL) {
                              sprintf(text, "Section TARRAIN, feature SHOW: ������������ ������ ��� ������ ������") ;
                           SEND_ERROR(text) ;
                                return(-1) ;
                        }

         for(i_z=0 ; i_z<z_cnt ; i_z++)
         for(i_x=0 ; i_x<x_cnt ; i_x++) {                           /* ��������! ������ ����� ������������� �� 1 */ 

                 n=(i_z*x_cnt+i_x)*2 ;

                 FACETS[n  ].vertexes_cnt=3 ;
                 FACETS[n+1].vertexes_cnt=3 ;

           if((i_x+i_z)%2) {
                 FACETS[n  ].vertexes[0] = i_z   *(x_cnt+1)+i_x+1 ;
                 FACETS[n  ].vertexes[1] = i_z   *(x_cnt+1)+i_x+2 ;
                 FACETS[n  ].vertexes[2] =(i_z+1)*(x_cnt+1)+i_x+2 ;

                 FACETS[n+1].vertexes[0] = i_z   *(x_cnt+1)+i_x+1 ;
                 FACETS[n+1].vertexes[1] =(i_z+1)*(x_cnt+1)+i_x+2 ;
                 FACETS[n+1].vertexes[2] =(i_z+1)*(x_cnt+1)+i_x+1 ;
                         }
           else          {  
                 FACETS[n  ].vertexes[0] = i_z   *(x_cnt+1)+i_x+1 ;
                 FACETS[n  ].vertexes[1] = i_z   *(x_cnt+1)+i_x+2 ;
                 FACETS[n  ].vertexes[2] =(i_z+1)*(x_cnt+1)+i_x+1 ;

                 FACETS[n+1].vertexes[0] = i_z   *(x_cnt+1)+i_x+2 ;
                 FACETS[n+1].vertexes[1] =(i_z+1)*(x_cnt+1)+i_x+2 ;
                 FACETS[n+1].vertexes[2] =(i_z+1)*(x_cnt+1)+i_x+1 ;
                         }
                                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#undef    BODY
#undef    BODIES_CNT
#undef    FACETS
#undef    FACETS_CNT
#undef  VERTEXES     
#undef  VERTEXES_CNT 

                     }
/*-------------------------------------- ���������� �������� TERRAIN */

      if(terrain!=NULL) {

#define    BODY          terrain->Bodies
#define    BODIES_CNT    terrain->Bodies_cnt
#define    FACETS        terrain->Bodies->Facets
#define    FACETS_CNT    terrain->Bodies->Facets_cnt
#define  VERTEXES        terrain->Bodies->Vertexes
#define  VERTEXES_CNT    terrain->Bodies->Vertexes_cnt
/*- - - - - - - - - - - - - - - - - - - - - - - - - -  �������� ���� */
           BODIES_CNT=1 ;

           BODY=(RSS_Feature_Terrain_Body *)calloc(1, sizeof(RSS_Feature_Terrain_Body)) ;
        if(BODY==NULL) {
                            sprintf(text, "Section TARRAIN, feature TARRAIN: ������������ ������ ��� ������ ���") ;
                         SEND_ERROR(text) ;
                              return(-1) ;
                       }

                       strcpy(BODY->name, "Body.Body") ;
                              BODY->x_base=x_base ;
                              BODY->y_base=   0. ;
                              BODY->z_base=z_base ;
/*- - - - - - - - - - - - - - - - - - - - - - �������� ������ ������ */
                   x_cnt=(int)((x_size-step*0.5)/step+1) ;
                   z_cnt=(int)((z_size-step*0.5)/step+1) ;
            VERTEXES_CNT=(x_cnt+1)*(z_cnt+1)+1 ;

           VERTEXES=(RSS_Feature_Terrain_Vertex *)calloc(VERTEXES_CNT, sizeof(*VERTEXES)) ;
        if(VERTEXES==NULL) {
                              sprintf(text, "Section TARRAIN, feature TARRAIN: ������������ ������ ��� ������ ������") ;
                           SEND_ERROR(text) ;
                                return(-1) ;
                           }                    

                  n=1 ;                                             /* ��������! ������ ����� ������������� �� 1 */ 

         for(i_z=0 ; i_z<=z_cnt ; i_z++)
         for(i_x=0 ; i_x<=x_cnt ; i_x++) {

           if(i_x==x_cnt)  VERTEXES[n].x=x_size ;
           else            VERTEXES[n].x=i_x*step ;

           if(i_z==z_cnt)  VERTEXES[n].z=z_size ;
           else            VERTEXES[n].z=i_z*step ;

                                    n++ ;

                                         } 
/*- - - - - - - - - - - - - - - - - - - - - - �������� ������ ������ */
           FACETS_CNT=x_cnt*z_cnt*2 ;

           FACETS=(RSS_Feature_Terrain_Facet *)calloc(FACETS_CNT, sizeof(RSS_Feature_Terrain_Facet)) ;
        if(FACETS==NULL) {
                              sprintf(text, "Section TARRAIN, feature TARRAIN: ������������ ������ ��� ������ ������") ;
                           SEND_ERROR(text) ;
                                return(-1) ;
                        }

         for(i_z=0 ; i_z<z_cnt ; i_z++)
         for(i_x=0 ; i_x<x_cnt ; i_x++) {                           /* ��������! ������ ����� ������������� �� 1 */ 

                 n=(i_z*x_cnt+i_x)*2 ;

                 FACETS[n  ].vertexes_cnt=3 ;
                 FACETS[n+1].vertexes_cnt=3 ;

           if((i_x+i_z)%2) {
                 FACETS[n  ].vertexes[0] = i_z   *(x_cnt+1)+i_x+1 ;
                 FACETS[n  ].vertexes[1] = i_z   *(x_cnt+1)+i_x+2 ;
                 FACETS[n  ].vertexes[2] =(i_z+1)*(x_cnt+1)+i_x+2 ;

                 FACETS[n+1].vertexes[0] = i_z   *(x_cnt+1)+i_x+1 ;
                 FACETS[n+1].vertexes[1] =(i_z+1)*(x_cnt+1)+i_x+2 ;
                 FACETS[n+1].vertexes[2] =(i_z+1)*(x_cnt+1)+i_x+1 ;
                         }
           else          {  
                 FACETS[n  ].vertexes[0] = i_z   *(x_cnt+1)+i_x+1 ;
                 FACETS[n  ].vertexes[1] = i_z   *(x_cnt+1)+i_x+2 ;
                 FACETS[n  ].vertexes[2] =(i_z+1)*(x_cnt+1)+i_x+1 ;

                 FACETS[n+1].vertexes[0] = i_z   *(x_cnt+1)+i_x+2 ;
                 FACETS[n+1].vertexes[1] =(i_z+1)*(x_cnt+1)+i_x+2 ;
                 FACETS[n+1].vertexes[2] =(i_z+1)*(x_cnt+1)+i_x+1 ;
                         }
                                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#undef    BODY
#undef    BODIES_CNT
#undef    FACETS
#undef    FACETS_CNT
#undef  VERTEXES     
#undef  VERTEXES_CNT 

                        }
/*--------------------------------- ��������� ������� � ����� ������ */

       OBJECTS=(RSS_Object **)
                 realloc(OBJECTS, (OBJECTS_CNT+1)*sizeof(*OBJECTS)) ;
    if(OBJECTS==NULL) {
              SEND_ERROR("������ TERRAIN: ������������ ������") ;
                                return(NULL) ;
                      }

              OBJECTS[OBJECTS_CNT]=object ;
                      OBJECTS_CNT++ ;

        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_DEFAULT_OBJECT, (LPARAM)name) ;

/*-------------------------------------------------- ��������� ����� */

            this->kernel->vShow(NULL) ;

/*-------------------------------------------------------------------*/

#undef   OBJECTS    
#undef   OBJECTS_CNT

#undef  _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� LINK                    */
/*								    */
/*       LINK  <��� ������� ���������> <��� �������>                */
/*       LINK+ <��� ������� ���������> <��� �������>                */
/*       LINK- <��� ������� ���������> <��� �������>                */

  int  RSS_Module_Terrain::cLink(char *cmd)

{
#define   _PARS_MAX  10

                  int  delta_flag ;        /* ���� ������ ���������� */
                 char *pars[_PARS_MAX] ;
                 char *t_name ;
                 char *o_name ;
           RSS_Object *terrain ;
           RSS_Object *object ;
  RSS_Feature_Terrain *land ; 
                 char  command[256] ;
               double  rad ;
               double  rad_min ;
                 char *end ;
                  int  n ;
                  int  i ;

/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
                      delta_flag=0 ;

       if(*cmd=='-' ||
          *cmd=='+'   ) {
 
                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("������������ ������ �������") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, '+')!=NULL   )  delta_flag= 1 ;
                if(strchr(cmd, '-')!=NULL   )  delta_flag=-1 ;

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

                    t_name=pars[0] ;
                    o_name=pars[1] ;   

/*--------------------------------- �������� ����� ������� ��������� */

    if(t_name==NULL) {                                              /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� ������� ���������. \n"
                                 "��������: LINK <���_�������_���������> ...") ;
                                     return(-1) ;
                     }

       terrain=(RSS_Object *)FindObject(t_name, 1) ;                /* ���� ������ ��������� �� ����� */
    if(terrain==NULL)  return(-1) ;

/*-------------------------------- �������� ����� ���������� ������� */

    if(o_name==NULL) {                                              /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� �������������� �������. \n"
                                 "��������: LINK <���_�������_���������> <��� �������>") ;
                                     return(-1) ;
                     }

       object=FindObject(o_name, 0) ;                               /* ���� ������-�������� �� ����� */
    if(object==NULL)  return(-1) ;
       
/*------------------------ ����� ��������� � ������� ����� ��������� */

#define  VERTEXES        land->Bodies->Vertexes
#define  VERTEXES_CNT    land->Bodies->Vertexes_cnt

   for(i=0 ; i<terrain->Features_cnt ; i++)
     if(!stricmp(terrain->Features[i]->Type, "Terrain"))
            land=(RSS_Feature_Terrain *)terrain->Features[i] ;

      if(land==NULL) {
                       SEND_ERROR("������ ��������� ���������� �������� TERRAIN") ;
                                     return(-1) ;
                     }

       strncpy(land->link_object, o_name, sizeof(land->link_object)-1) ; 

   for(i=1 ; i<VERTEXES_CNT ; i++) {

           rad=(VERTEXES[i].x-object->x_base)*(VERTEXES[i].x-object->x_base)+
               (VERTEXES[i].z-object->z_base)*(VERTEXES[i].z-object->z_base) ;

      if(i==1 || rad<rad_min) {
                                 rad_min=rad ;
                                      n = i ;
                              }
                                   }

                             n+=delta_flag ;
        if(n<  1          )  n = 1 ;
        if(n>=VERTEXES_CNT)  n =VERTEXES_CNT-1 ;

                     object->x_base=VERTEXES[n].x ;
                     object->y_base=VERTEXES[n].y ;
                     object->z_base=VERTEXES[n].z ;

#undef  VERTEXES     
#undef  VERTEXES_CNT 

/*------------------------------------------------ ����������� ����� */

                        sprintf(command, "%s base %s %lf %lf %lf",
                                            object->Type, o_name,
                                            object->x_base, object->y_base, object->z_base) ;
    object->Module->vExecuteCmd(command) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� SCAN                    */
/*								    */
/*       SCAN  <��� ������� ���������>                              */
/*       SCAN+ <��� ������� ���������>                              */
/*       SCAN- <��� ������� ���������>                              */

  int  RSS_Module_Terrain::cScan(char *cmd)

{
#define   _PARS_MAX  10

                  int  delta_flag ;        /* ���� ������ ���������� */
                 char *pars[_PARS_MAX] ;
                 char *t_name ;
                 char *o_name ;
           RSS_Object *terrain ;
           RSS_Object *object ;
  RSS_Feature_Terrain *land ; 
                 char  command[256] ;
               double  rad ;
               double  rad_min ;
                 char *end ;
                  int  n ;
                  int  i ;

/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
                      delta_flag=1 ;

       if(*cmd=='-' ||
          *cmd=='+'   ) {
 
                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("������������ ������ �������") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, '+')!=NULL   )  delta_flag= 1 ;
                if(strchr(cmd, '-')!=NULL   )  delta_flag=-1 ;

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

                    t_name=pars[0] ;

/*--------------------------------- �������� ����� ������� ��������� */

    if(t_name==NULL) {                                              /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� ������� ���������. \n"
                                 "��������: SCAN <���_�������_���������> ...") ;
                                     return(-1) ;
                     }

       terrain=(RSS_Object *)FindObject(t_name, 1) ;                /* ���� ������ ��������� �� ����� */
    if(terrain==NULL)  return(-1) ;

/*-------------------------------------- ���������� �������� Terrain */

   for(i=0 ; i<terrain->Features_cnt ; i++)
     if(!stricmp(terrain->Features[i]->Type, "Terrain"))
            land=(RSS_Feature_Terrain *)terrain->Features[i] ;

      if(land==NULL) {
                       SEND_ERROR("������ ��������� ���������� �������� TERRAIN") ;
                                     return(-1) ;
                     }

       o_name=land->link_object ; 

/*-------------------------------- �������� ����� ���������� ������� */

    if(o_name[0]==0) {                                              /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� ���������� ������� - ����������� ������� LINK. \n") ;
                                     return(-1) ;
                     }

       object=FindObject(o_name, 0) ;                               /* ���� ������-�������� �� ����� */
    if(object==NULL)  return(-1) ;
       
/*------------------------ ����� ��������� � ������� ����� ��������� */

#define  VERTEXES        land->Bodies->Vertexes
#define  VERTEXES_CNT    land->Bodies->Vertexes_cnt

   for(i=1 ; i<VERTEXES_CNT ; i++) {

           rad=(VERTEXES[i].x-object->x_base)*(VERTEXES[i].x-object->x_base)+
               (VERTEXES[i].z-object->z_base)*(VERTEXES[i].z-object->z_base) ;

      if(i==1 || rad<rad_min) {
                                 rad_min=rad ;
                                      n = i ;
                              }
                                   }

                             n+=delta_flag ;
        if(n<  1          )  n = 1 ;
        if(n>=VERTEXES_CNT)  n =VERTEXES_CNT-1 ;

                     object->x_base=VERTEXES[n].x ;
                     object->y_base=VERTEXES[n].y ;
                     object->z_base=VERTEXES[n].z ;

#undef  VERTEXES     
#undef  VERTEXES_CNT 

/*------------------------------------------------ ����������� ����� */

                        sprintf(command, "%s base %s %lf %lf %lf",
                                            object->Type, o_name,
                                            object->x_base, object->y_base, object->z_base) ;
    object->Module->vExecuteCmd(command) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� UP                      */
/*								    */
/*       UP  <��� ������� ���������> <�������� ������>              */
/*       UP+ <��� ������� ���������> <��������� ������>             */

  int  RSS_Module_Terrain::cUp(char *cmd)

{
#define   _PARS_MAX  10

                  int  delta_flag ;        /* ���� ������ ���������� */
                 char *pars[_PARS_MAX] ;
                 char *t_name ;
                 char *o_name ;
           RSS_Object *terrain ;
           RSS_Object *object ;
  RSS_Feature_Terrain *land ; 
     RSS_Feature_Show *show ; 
                 char  command[256] ;
               double  y ;
               double  rad ;
               double  rad_min ;
                 char *end ;
                  int  n ;
                  int  i ;

/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
                      delta_flag=0 ;

       if(*cmd=='+'   ) {
 
                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("������������ ������ �������") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, '+')!=NULL   )  delta_flag= 1 ;

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

                    t_name=pars[0] ;

/*--------------------------------- �������� ����� ������� ��������� */

    if(t_name==NULL) {                                              /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� ������� ���������. \n"
                                 "��������: UP <���_�������_���������> ...") ;
                                     return(-1) ;
                     }

       terrain=(RSS_Object *)FindObject(t_name, 1) ;                /* ���� ������ ��������� �� ����� */
    if(terrain==NULL)  return(-1) ;

/*-------------------------------------- ���������� �������� Terrain */

   for(i=0 ; i<terrain->Features_cnt ; i++)
     if(!stricmp(terrain->Features[i]->Type, "Terrain"))
            land=(RSS_Feature_Terrain *)terrain->Features[i] ;

      if(land==NULL) {
                       SEND_ERROR("������ ��������� ���������� �������� TERRAIN") ;
                                     return(-1) ;
                     }

       o_name=land->link_object ; 

/*-------------------------------- �������� ����� ���������� ������� */

    if(o_name[0]==0) {                                              /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� ���������� ������� - ����������� ������� LINK. \n") ;
                                     return(-1) ;
                     }

       object=FindObject(o_name, 0) ;                               /* ���� ������-�������� �� ����� */
    if(object==NULL)  return(-1) ;
       
/*--------------------------------------- ���������� �������� ������ */

    if(pars[1]==NULL) {                                             /* ���� ������ �� ������... */
                      SEND_ERROR("�� ������ ������ ����� ���������. \n"
                                 "��������: UP <���_�������_���������> <������>") ;
                                     return(-1) ;
                     }

         y=strtod(pars[1], &end) ;

    if(*end!=0) {                                                   /* ���� ������ �� ������... */
                      SEND_ERROR("������������ �������� ������.") ;
                                     return(-1) ;
                }
/*------------------------ ����� ��������� � ������� ����� ��������� */

#define  VERTEXES        land->Bodies->Vertexes
#define  VERTEXES_CNT    land->Bodies->Vertexes_cnt

   for(i=1 ; i<VERTEXES_CNT ; i++) {

           rad=(VERTEXES[i].x-object->x_base)*(VERTEXES[i].x-object->x_base)+
               (VERTEXES[i].z-object->z_base)*(VERTEXES[i].z-object->z_base) ;

      if(i==1 || rad<rad_min) {
                                 rad_min=rad ;
                                      n = i ;
                              }
                                   }


      if(delta_flag)  VERTEXES[n].y+=y ;
      else            VERTEXES[n].y =y ;

                     object->x_base=VERTEXES[n].x ;
                     object->y_base=VERTEXES[n].y ;
                     object->z_base=VERTEXES[n].z ;

#undef  VERTEXES     
#undef  VERTEXES_CNT 

                    land->Bodies->recalc=1 ;

/*-------------------------------------- ������������� ������� ����� */

   for(i=0 ; i<terrain->Features_cnt ; i++)
     if(!stricmp(terrain->Features[i]->Type, "Show"))
            show=(RSS_Feature_Show *)terrain->Features[i] ;

     if(show!=NULL) {

#define  VERTEXES        show->Bodies->Vertexes

                     VERTEXES[n].y=object->y_base ;

#undef  VERTEXES     

//                       show->Bodies->recalc=1 ;

                    }
/*------------------------------------------------ ����������� ����� */

                        sprintf(command, "%s base %s %lf %lf %lf",
                                            object->Type, o_name,
                                            object->x_base, object->y_base, object->z_base) ;
    object->Module->vExecuteCmd(command) ;

      this->kernel->vShow(NULL) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		   ����� ������� ��������� �� �����                 */

  RSS_Object *RSS_Module_Terrain::FindObject(char *name, int  check_type)

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
     if(strcmp(OBJECTS[i]->Type, "Terrain")) {

           SEND_ERROR("������ �� �������� �������� ���� TERRAIN") ;
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
/**           �������� ������ �������� � ������� ���������         **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       ����������� ����������			    */


/********************************************************************/
/*								    */
/*		       ����������� ������			    */

     RSS_Feature_Terrain::RSS_Feature_Terrain(void)

{
   strcpy(Type, "Terrain") ;

       Bodies    =NULL ;
       Bodies_cnt=  0 ;

            slave=  0 ;

    memset(link_object, 0, sizeof(link_object)) ;

      terrain_last =NULL ;
      terrain_body = -1 ;
      terrain_facet= -1 ;
}


/********************************************************************/
/*								    */
/*		        ���������� ������			    */

    RSS_Feature_Terrain::~RSS_Feature_Terrain(void)

{
}


/********************************************************************/
/*								    */
/*		        ������� ������ �� ������		    */

    void  RSS_Feature_Terrain::vReadSave(       char *title,  
                                         std::string *data, 
                                                char *reference)

{
  RSS_Kernel *Calc ;
         int  body_num ;
        char *buff ;
         int  buff_size ;
        char *work ;
        char *decl ;
        char *end ;
        char *sep ;
        char *coord ;
         int  num ;
        char  error[1024] ;
        char  text[1024] ;
         int  status ;
         int  i ;

#define    VERTEXES      Bodies[body_num].Vertexes
#define    VERTEXES_CNT  Bodies[body_num].Vertexes_cnt
#define    FACETS        Bodies[body_num].Facets
#define    FACETS_CNT    Bodies[body_num].Facets_cnt


/*----------------------------------------------- �������� ��������� */

       if(stricmp(title, "FEATURE TERRAIN"))  return ;

/*---------------------------------- �������� ���������� ������ ���� */

                    vBodyDelete(reference) ;

/*------------------------------------------------ ���������� ������ */

              buff_size=(int)data->size()+16 ;
              buff     =(char *)calloc(1, buff_size) ;

       strcpy(buff, data->c_str()) ;
        
/*---------------------------------------------- ���������� �������� */

//                     Bodies_cnt= 0 ;
                         body_num=Bodies_cnt-1 ;

    for(work=buff ; ; work=end+1) {                                 /* CIRCLE.0 */

           end=strchr(work, '\n') ;
        if(end==NULL)  break ;
          *end=0 ;

/*------------------------------------------------------------- ���� */

          if(!memicmp(work, "$BODY", strlen("$BODY"))) {

                    work+=strlen("$BODY") ;

                            body_num++ ;
                          Bodies_cnt++ ;

               Bodies=(RSS_Feature_Terrain_Body *)
                        realloc(Bodies, Bodies_cnt*sizeof(Bodies[0])) ;
            if(Bodies==NULL) {
                  sprintf(text, "Section FEATURE TERRAIN: "
                                "������������ ������ ��� ������ ���\n") ;
               SEND_ERROR(text) ;
                                        break ;
                             }

                 memset(&Bodies[body_num], 0, sizeof(Bodies[0])) ;

                    for( ; *work==' ' || *work=='\t' ; work++) ;

            if(reference!=NULL)                                         /* ������� ��� ���� */ 
                     strcpy(Bodies[body_num].name, reference) ;
            else     strcpy(Bodies[body_num].name, work) ;

                            Bodies[body_num].extrn_pars=Pars_work ;
                            Bodies[body_num].recalc    = 1 ;

                                                       }
/*---------------------------------------------------------- ������� */

     else if(!memicmp(work, "$VERTEX", strlen("$VERTEX"))) {

           if(body_num==-1)  continue ;
/*- - - - - - - - - - - - - - - - - - - - - ��������� ������ ������� */
               decl=work+strlen("$VERTEX") ;
                num=strtol(decl, &decl, 10) ;

           if(num<0 || *decl!='=') {
                sprintf(text, "Section FEATURE TERRAIN: "
                              "�������� ������ ������������� VERTEX\n - %s", work) ;
             SEND_ERROR(text) ;
                                        break ;
                                   }

               decl++ ;
/*- - - - - - - - - - - - - - - - - - - - -  ���������� ������ ����� */
           if(num>=VERTEXES_CNT) {

               VERTEXES=(RSS_Feature_Terrain_Vertex *)
                          realloc(VERTEXES, (num+1)*sizeof(*VERTEXES)) ;
            if(VERTEXES==NULL) {
                  SEND_ERROR("Section FEATURE TERRAIN: ������������ ������") ;
                                   break ;
                               }                    

                                    VERTEXES_CNT=num+1 ;
                                 }
/*- - - - - - - - - - - - - - - - - - - ��������� ���������� X, Y, Z */
               VERTEXES[num].x_formula=(char *)                     /* �������� ������ ��� �������� */
                                         calloc(1, strlen(decl)+8) ; 
            if(VERTEXES[num].x_formula==NULL) {
                  SEND_ERROR("Section FEATURE TERRAIN: ������������ ������") ;
                                   break ;
                                              }

                    strcpy(VERTEXES[num].x_formula, decl) ;
               sep=strrchr(VERTEXES[num].x_formula, ';') ;
            if(sep==NULL) {
                  sprintf(text, "Section FEATURE TERRAIN: "
                                "�������� ������ ������������� VERTEX\n - %s", work) ;
               SEND_ERROR(text) ;
                                        break ;
                          }
                                             *sep=0 ;
                           VERTEXES[num].z_formula=sep+1 ;
               sep=strrchr(VERTEXES[num].x_formula, ';') ;
            if(sep==NULL) {
                  sprintf(text, "Section FEATURE TERRAIN: "
                                "�������� ������ ������������� VERTEX\n - %s", work) ;
               SEND_ERROR(text) ;
                                        break ;
                          }
                                               *sep= 0 ;
                            VERTEXES[num].y_formula=sep+1 ;
/*- - - - - - - - - - - - - - - - - - - - -  ����������� ����������� */
             Calc=iGetCalculator() ;
          if(Calc== NULL) {
               SEND_ERROR("Section FEATURE SHOW: �� �������� ����������� ����������� ���������") ;
                             return ;
                         }
/*- - - - - - - - - - - - - - - - - - - - - - ������ ��������� ����� */
                                          VERTEXES[num].x_calculate=NULL ;
                                          VERTEXES[num].y_calculate=NULL ;
                                          VERTEXES[num].z_calculate=NULL ;

      do {
              coord="X" ;
             status=Calc->vCalculate("STD_EXPRESSION",
                                      VERTEXES[num].x_formula,
                                       Bodies[body_num].extrn_pars,
                                            NULL,
                                     &VERTEXES[num].x,
                                     &VERTEXES[num].x_calculate,
                                            error) ;
          if(status==-1)  break ;                                   /* ���� ���� ������... */
                      
              coord="Y" ;
             status=Calc->vCalculate("STD_EXPRESSION",
                                      VERTEXES[num].y_formula,
                                       Bodies[body_num].extrn_pars,
                                            NULL,
                                     &VERTEXES[num].y,
                                     &VERTEXES[num].y_calculate,
                                            error) ;
          if(status==-1)  break ;                                   /* ���� ���� ������... */

              coord="Z" ;
             status=Calc->vCalculate("STD_EXPRESSION",
                                      VERTEXES[num].z_formula,
                                       Bodies[body_num].extrn_pars,
                                            NULL,
                                     &VERTEXES[num].z,
                                     &VERTEXES[num].z_calculate,
                                            error) ;
         } while(0) ;

          if(status==-1) {
                  sprintf(text, "Section FEATURE TERRAIN: ������ �����������\n\n"
                                "������� %d, ���������� %s\n"
                                "%s" ,
                                 num, coord, error) ;
               SEND_ERROR(text) ;
                             return ;
                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                           }
/*------------------------------------------------------------ ����� */
  
     else if(!memicmp(work, "$FACET", strlen("$FACET"))) {

           if(body_num==-1)  continue ; ;
/*- - - - - - - - - - - - - - - - - - - - - - - - ������ �� �������� */
              decl=strchr(work, '=') ;
           if(decl==NULL) {
                sprintf(text, "Section FEATURE TERRAIN: "
                              "�������� ������ ������������� FACET\n - %s", work) ;
             SEND_ERROR(text) ;
                                 break ;
                          }

              decl++ ;
/*- - - - - - - - - - - - - - - - - - - - - - ���������� ����� ����� */
              FACETS=(RSS_Feature_Terrain_Facet *)
                       realloc(FACETS, (FACETS_CNT+1)*sizeof(*FACETS)) ;
           if(FACETS==NULL) {
                  SEND_ERROR("Section FEATURE TERRAIN: ������������ ������") ;
                                break ;
                            }

                memset(&FACETS[FACETS_CNT], 0, sizeof(FACETS[0])) ;

                               FACETS_CNT++ ;
/*- - - - - - - - - - - - - - - - - - - - ������������ ������ ������ */
#define  F    FACETS[FACETS_CNT-1]

          for(i=0 ; i<_VERTEX_PER_FACET_MAX ; i++) {

                    F.vertexes[i]=strtol(decl, &decl, 10) ;

               if(*decl==0)  break ;
                   decl++ ;
                                                   }

              if(i==_VERTEX_PER_FACET_MAX) {
                  SEND_ERROR("Section FEATURE TERRAIN: ������� ����� ������ � �����") ;
                                break ;
                                           }

                    F.vertexes_cnt=i+1 ;
                    F.abcd_formed = 0 ;
#undef   F
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                         }
/*----------------------------------------- ����������� ������������ */

     else                                                {

                sprintf(text, "Section FEATURE TERRAIN: "
                              "����������� ������������ %s10.10", work) ;
             SEND_ERROR(text) ;
                                 break ;
                                                         }
/*---------------------------------------------- ���������� �������� */

                                  }                                 /* CONTINUE.0 */

/*-------------------------------------------- ������������ �������� */

                free(buff) ;

/*-------------------------------------------------------------------*/

#undef    VERTEXES
#undef    VERTEXES_CNT
#undef    FACETS
#undef    FACETS_CNT

}


/********************************************************************/
/*								    */
/*                     ������ ���������� � ��������   		    */

    void RSS_Feature_Terrain::vGetInfo(std::string *text)

{
   if(slave)  *text="Terrain: Slave\r\n" ;
   else       *text="Terrain: Free\r\n" ;
}


/*********************************************************************/
/*								     */
/*                          ������� ����                             */

    void RSS_Feature_Terrain::vBodyDelete(char *body)
{
   int  cnt ;
   int  n_body ;

/*-------------------------------------- ������������� ���� �� ����� */

    if(body==NULL) {                                                /* �������� ���� ��� */

        for(cnt=this->Bodies_cnt ; cnt>0 ; cnt--)
                         this->vBodyDelete(this->Bodies[0].name) ;

                              return ;
                   }

        for(n_body=0 ; n_body<this->Bodies_cnt ; n_body++)
           if(!stricmp(this->Bodies[n_body].name, body))  break ;

           if(n_body==this->Bodies_cnt)  return ;

/*---------------------------------------------------- �������� ���� */

       if(this->Bodies[n_body].Vertexes_cnt)                        /* ����������� ������ ������ */
            free(this->Bodies[n_body].Vertexes) ;

       if(this->Bodies[n_body].Facets_cnt)                          /* ����������� ������ ������ */
            free(this->Bodies[n_body].Facets) ;

       if(this->Bodies_cnt>1) {                                     /* ���� ����� ������ ���� - */
                                                                    /*   - ��������� ������ ��� */
         if(n_body!=this->Bodies_cnt-1) 
               memmove(&this->Bodies[n_body], 
                       &this->Bodies[n_body+1],
                          sizeof(this->Bodies[0])*(this->Bodies_cnt-n_body-1)) ;
                              }
       else                   {                                     /* ���� ���� ������ ���� -  */
                                  free(this->Bodies) ;              /*   - ������� ���          */
                                       this->Bodies=NULL ;
                              }

                                    this->Bodies_cnt-- ;            /* ������������ ������� ��� */

/*-------------------------------------------------------------------*/

}


/********************************************************************/
/*								    */
/*                   ������� ������� ����� ����                     */

    void RSS_Feature_Terrain::vBodyBasePoint(  char *body,                   
                                             double  x, 
                                             double  y, 
                                             double  z )
{
   int  n_body ;

/*-------------------------------------- ������������� ���� �� ����� */

    if(body==NULL) {
                      n_body=0 ;
                   }
    else           {

        for(n_body=0 ; n_body<this->Bodies_cnt ; n_body++)
           if(!stricmp(this->Bodies[n_body].name, body))  break ;
                   }

           if(n_body==this->Bodies_cnt)  return ;

/*--------------------------------------------- ��������� ���������� */

       Bodies[n_body].x_base     =x ;
       Bodies[n_body].y_base     =y ;
       Bodies[n_body].z_base     =z ;

       Bodies[n_body].Matrix_flag=0 ;
       Bodies[n_body].recalc     =1 ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*                     ������� ���������� ����                      */

    void RSS_Feature_Terrain::vBodyAngles(  char *body,                   
                                          double  azim, 
                                          double  elev, 
                                          double  roll    )
{
   int  n_body ;


/*-------------------------------------- ������������� ���� �� ����� */

    if(body==NULL) {
                      n_body=0 ;
                   }
    else           {

        for(n_body=0 ; n_body<this->Bodies_cnt ; n_body++)
           if(!stricmp(this->Bodies[n_body].name, body))  break ;
                   }

           if(n_body==this->Bodies_cnt)  return ;

/*--------------------------------------------- ��������� ���������� */

       Bodies[n_body].a_azim     =azim ;
       Bodies[n_body].a_elev     =elev ;
       Bodies[n_body].a_roll     =roll ;

       Bodies[n_body].Matrix_flag=  0 ;
       Bodies[n_body].recalc     =  1 ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*               ������� ������� ��������� ����                     */

    void RSS_Feature_Terrain::vBodyMatrix(  char *body,
                                          double  matrix[4][4] )
{
   int  n_body ;

/*-------------------------------------- ������������� ���� �� ����� */

    if(body==NULL) {
                      n_body=0 ;
                   }
    else           {

        for(n_body=0 ; n_body<this->Bodies_cnt ; n_body++)
           if(!stricmp(this->Bodies[n_body].name, body))  break ;
                   }

           if(n_body==this->Bodies_cnt)  return ;

/*--------------------------------------------- ��������� ���������� */

       memcpy(Bodies[n_body].Matrix, matrix, sizeof(Bodies[n_body].Matrix)) ;

              Bodies[n_body].Matrix_flag=1 ;
              Bodies[n_body].recalc     =1 ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*           ������� �������� ��������� � ���������� ����           */

    void RSS_Feature_Terrain::vBodyShifts(   char *body,                   
                                           double  x, 
                                           double  y, 
                                           double  z, 
                                           double  azim, 
                                           double  elev, 
                                           double  roll    )
{
   int  n_body ;


/*-------------------------------------- ������������� ���� �� ����� */

    if(body==NULL) {
                      n_body=0 ;
                   }
    else           {

        for(n_body=0 ; n_body<this->Bodies_cnt ; n_body++)
           if(!stricmp(this->Bodies[n_body].name, body))  break ;
                   }

           if(n_body==this->Bodies_cnt)  return ;

/*--------------------------------------------- ��������� ���������� */

       Bodies[n_body].x_base_s=x ;
       Bodies[n_body].y_base_s=y ;
       Bodies[n_body].z_base_s=z ;
       Bodies[n_body].a_azim_s=azim ;
       Bodies[n_body].a_elev_s=elev ;
       Bodies[n_body].a_roll_s=roll ;

       Bodies[n_body].recalc  =1 ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*              ������� ������ ���������� ����                      */

    void RSS_Feature_Terrain::vBodyPars(char *body, struct RSS_Parameter *pars)
{
   int  n_body ;

/*-------------------------------------- ������������� ���� �� ����� */

    if(body==NULL) {
                      n_body=0 ;
                   }
    else           {

        for(n_body=0 ; n_body<this->Bodies_cnt ; n_body++)
           if(!stricmp(this->Bodies[n_body].name, body))  break ;
                   }
/*--------------------------------------------- ��������� ���������� */

                                                Pars_work=pars ;

   if(n_body<this->Bodies_cnt)  Bodies[n_body].extrn_pars=pars ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*            ����������� ������� �����������                       */

  RSS_Kernel *RSS_Feature_Terrain::iGetCalculator(void)

{
  int  status ;
  int  i ;

#define  CALC_CNT   RSS_Kernel::calculate_modules_cnt
#define  CALC       RSS_Kernel::calculate_modules

         for(i=0 ; i<CALC_CNT ; i++) {

             status=CALC[i]->vCalculate("STD_EXPRESSION", NULL, NULL, NULL, 
                                                          NULL, NULL, NULL ) ;
         if(status==0)  return(CALC[i]) ;
                                     }

#undef   CALC_CNT
#undef   CALC

   return(NULL) ;
}


/********************************************************************/
/*								    */
/*       ����� ��������� �������� ������������������ ��������       */  

    int RSS_Feature_Terrain::vResetCheck(void *data)

{
            terrain_last =NULL ;
            terrain_body = -1 ;
            terrain_facet= -1 ;

   return(0) ;
}


/*********************************************************************/
/*								     */
/*       ���������� � �������� ������������������ ��������           */  

    int RSS_Feature_Terrain::vPreCheck(void *data)

{
         this->RecalcPoints() ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*                �������� ������������������ ��������              */  

    int RSS_Feature_Terrain::vCheck(void *data, RSS_Objects_List *checked)

{
 RSS_Object *object ;
        int  n ;            /* ������ ������� � ������ ��������*/
        int  m ;            /* ������ �������� "�����������" � ������ ������� */
        int  i ;
        int  j ;

#define  OBJECTS_CNT                     RSS_Kernel::kernel_objects_cnt
#define  OBJECTS                         RSS_Kernel::kernel_objects
#define  TER         ((RSS_Feature_Terrain *)object->Features[m])

/*------------------------------------------------ ������� ��������  */

      if(this->slave==0)  return(0) ;                               /* ���� ������ �� ������� �� �������... */

/*----------------- �������� ����������� ���������� �������� ������� */

  if(terrain_last!=NULL) do {

        if(!iOverallTest(terrain_last))  {
                                            terrain_last=NULL ;
                                               break ;
                                         } 

        if(iFacetTest(terrain_last, 
                      terrain_body, terrain_facet))   break ;

                 terrain_body=-1 ;

      for(i=0 ; i<terrain_last->Bodies_cnt ; i++) {                 /* ������ ��������� �������  */

        for(j=0 ; j<terrain_last->Bodies[i].Facets_cnt ; j++) 
          if(iFacetTest(terrain_last, i, j)) {
                                                terrain_body =i ;
                                                terrain_facet=j ;
                                                    break ;
                                             }
           if(terrain_body>=0)  break ;

                                                  }

           if(terrain_body==-1)  terrain_last=NULL ;

                            } while(0) ;

/*------------------------------------------------- ������ ��������� */

  if(terrain_last==NULL) {

      for(n=0 ; n<OBJECTS_CNT ; n++) {                              /* CIRCLE.1 - ������� �������� */

           object=OBJECTS[n] ;

        for(m=0 ; m<object->Features_cnt ; m++)                    /* ���������, ������� �� � ������ ��������         */
          if(!strcmp(object->Features[m]->Type,                    /*  �������� "�����������", ���� ��� - ������� ��� */
                                    this->Type ))  break ;  

            if(m==object->Features_cnt)  continue ;                /* ���� ������-���� �� �������� ��������� Terrain... */

            if(TER->slave        )  continue ;                     /* ������ ��� ��������� ������� */
            if(TER->Bodies_cnt==0)  continue ;                     /* ���� �������� Terrain - ������ */
    
            if(!iOverallTest(TER))  continue ;                     /* ���������� ��������... */

                 terrain_body=-1 ;

          for(i=0 ; i<TER->Bodies_cnt ; i++) {                     /* ������ ��������� �������  */

            for(j=0 ; j<TER->Bodies[i].Facets_cnt ; j++) 
              if(iFacetTest(TER, i, j)) {
                                             terrain_last =TER ;
                                             terrain_body = i ;
                                             terrain_facet= j ;
                                                 break ;
                                        }

              if(terrain_last!=NULL)  break ;

                                             }

                                     }                              /* CONTINUE.1 */

                         }
/*------------------------------ �������� ������� � �������� ������� */

   if(terrain_last!=NULL) {

         iPlaceObject(terrain_last, terrain_body, terrain_facet) ;

                          }
/*-------------------------------------------------------------------*/

#undef   OBJECTS_CNT
#undef   OBJECTS
#undef   TER

   return(0) ;
}


/********************************************************************/
/*								    */
/*                    �������� ����������� ���������                */ 

    int RSS_Feature_Terrain::iOverallTest(RSS_Feature_Terrain *terrain)
{

   if(this->Object->x_base>=terrain->overall.x_min &&
      this->Object->x_base<=terrain->overall.x_max &&
      this->Object->z_base>=terrain->overall.z_min &&
      this->Object->z_base<=terrain->overall.z_max   )  return(1) ;

  return(0) ;
}     


/********************************************************************/
/*								    */
/*                �������� ����������� ������ ���� ����             */ 

    int RSS_Feature_Terrain::iFacetTest(RSS_Feature_Terrain *terrain, int  n_body, int  n_facet)
{
   RSS_Feature_Terrain_Body *body ;
  RSS_Feature_Terrain_Facet *facet ;
                     double  x0, z0 ;
                     double  x1, z1 ;
                     double  x2, z2 ;
                     double  x_b, z_b ;
                     double  y ;
                     double  v ;
                        int  cross ;
                        int  i ;

/*------------------------------------------------------- ���������� */

         body=&terrain->Bodies[n_body ] ;
        facet=&   body->Facets[n_facet] ;

          x_b=this->Object->x_base ;
          z_b=this->Object->z_base ;

/*----------------- �������� ���������� ������� ������ ������� ����� */
/* ���������, ��� ������� ������� �������� ����� ����� ������������  */
/*  ������� ����� ������� ����� ���� � ��� �� ����                   */
                 cross =1 ;
                     v =0 ;
                     x2=0 ;
                     z2=0 ;

   for(i=0 ; i<=facet->vertexes_cnt ; i++) {

     if(i<facet->vertexes_cnt) {

                 x1=x2 ;
                 z1=z2 ;
                 x2=body->Vertexes[facet->vertexes[i]].x_abs ;
                 z2=body->Vertexes[facet->vertexes[i]].z_abs ;
                               }
     else                      {
                 x1=x2 ;
                 z1=z2 ;
                 x2=x0 ;
                 z2=z0 ;
                               }

           if(i==0) {   x0=x2 ;
                        z0=z2 ;
                       continue ;  }

              y=(z1-z_b)*(x2-x1)-(x1-x_b)*(z2-z1) ;

           if(v*y<0)  cross=0 ;

                v=y ; 

           if(cross==0)  break ;         
                                           }

           if(cross>0)  return(1) ;

/*-------------------------------------------------------------------*/

  return(0) ;

}


/********************************************************************/
/*								    */
/*     ������ ��������� ������� �� �������� � �������� ���������    */ 

    int RSS_Feature_Terrain::iPlaceObject(RSS_Feature_Terrain *terrain, int  n_body, int  n_facet)

{
    RSS_Feature_Terrain_Body *body ;
   RSS_Feature_Terrain_Facet *facet ;
  RSS_Feature_Terrain_Vertex  points[3];
   RSS_Feature_Terrain_Facet  facet_o ;
                    Matrix2d  Summary ;
                    Matrix2d  Local ;
                    Matrix2d  Point ;
                      double  x0, y0, z0 ;
                      double  x1, y1, z1 ;
                         int  i ;

/*------------------------------------------------------- ���������� */

         body=&terrain->Bodies[n_body ] ;
        facet=&   body->Facets[n_facet] ;

         x0 =body->Vertexes[facet->vertexes[0]].x_abs ;
         y0 =body->Vertexes[facet->vertexes[0]].y_abs ;
         z0 =body->Vertexes[facet->vertexes[0]].z_abs ;

/*--------------------------------- ������ ��������� ��������� ����� */

   if(facet->abcd_formed==0) {

        iPlaneByPoints(facet, &body->Vertexes[facet->vertexes[0]],
                              &body->Vertexes[facet->vertexes[1]],
                              &body->Vertexes[facet->vertexes[2]] ) ;

         facet->abcd_formed=1 ;

                             }
/*---------------------- ����������� ��������� ������� ����� ������� */

    if(fabs(facet->b)>0.01)
          this->Object->y_base=-(facet->a*this->Object->x_base+
                                 facet->c*this->Object->z_base+ 
                                 facet->d                      )/
                                 facet->b ; 
    else  this->Object->y_base= y0 ;

/*----------------------------------- ����������� ���������� ������� */

    if(fabs(facet->b)>0.01) {
/*- - - - - - - - - - - - - - - - - - -  ����������� ���� ���������� */
       x1=this->Object->x_base+sin(this->Object->a_azim*_GRD_TO_RAD) ;
       z1=this->Object->z_base+cos(this->Object->a_azim*_GRD_TO_RAD) ;

       y1=-(facet->a*x1+facet->c*z1+facet->d)/facet->b ; 

          this->Object->a_elev=atan(y1-this->Object->y_base)*_RAD_TO_GRD ;
/*- - - - - - - - - - - - - - - - - - -  ����������� ���� ���������� */
                Summary.LoadE      (4, 4) ;                         /* ����������� ������� ��������� � ������� �������� ������� */
                  Local.Load4d_elev(this->Object->a_elev) ;
                Summary.LoadMul    (&Summary, &Local) ;
                  Local.Load4d_azim(this->Object->a_azim) ;
                Summary.LoadMul    (&Summary, &Local) ;

       for(i=0 ; i<3 ; i++) {                                       /* ������������� ������� ����� ��������� � ������� ��������� ������� */

                           Point.LoadZero(4, 1) ;
                           Point.SetCell (0, 0, body->Vertexes[facet->vertexes[i]].x_abs) ;
                           Point.SetCell (1, 0, body->Vertexes[facet->vertexes[i]].y_abs) ;
                           Point.SetCell (2, 0, body->Vertexes[facet->vertexes[i]].z_abs) ;
                           Point.SetCell (3, 0,   1   ) ;

                           Point.LoadMul (&Summary, &Point) ;            
                                                                    
           points[i].x_abs=Point.GetCell (0, 0) ;
           points[i].y_abs=Point.GetCell (1, 0) ;
           points[i].z_abs=Point.GetCell (2, 0) ;
                            }

        iPlaneByPoints(&facet_o, &points[0],                        /* ����������� ��������� ��������� � ������� ��������� ������� */
                                 &points[1], &points[2]) ;

if(points[0].y_abs!=0.) {
                          points[0].y=points[0].y_abs ;
                        } 

          this->Object->a_roll=atan(facet_o.a/facet_o.b)*_RAD_TO_GRD ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                            }
    else                    {
                               this->Object->a_elev=0. ;
                               this->Object->a_roll=0. ;
                            }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*     ����������� ������������� ��������� ��������� �� 3 ������    */ 

    int RSS_Feature_Terrain::iPlaneByPoints(RSS_Feature_Terrain_Facet  *facet,
                                            RSS_Feature_Terrain_Vertex *point1,
                                            RSS_Feature_Terrain_Vertex *point2,
                                            RSS_Feature_Terrain_Vertex *point3 )

{
    double  x0, y0, z0 ;
    double  m21, m22, m23, m31, m32, m33 ;
    double  d1, d2, d3 ;
    double  r ;

#define   DET(v11, v12, v21, v22)   (v11*v22-v12*v21) 


         x0 =point1->x_abs ;
         y0 =point1->y_abs ;
         z0 =point1->z_abs ;

         m21=point2->x_abs-x0 ;
         m22=point2->y_abs-y0 ;
         m23=point2->z_abs-z0 ;
         m31=point3->x_abs-x0 ;
         m32=point3->y_abs-y0 ;
         m33=point3->z_abs-z0 ;

         d1 =DET(m22, m23, m32, m33) ;
         d2 =DET(m21, m23, m31, m33) ;
         d3 =DET(m21, m22, m31, m32) ;

         r  =sqrt(d1*d1+d2*d2+d3*d3) ;           

    if(r!=0.) {
                facet->a= d1/r ; 
                facet->b=-d2/r ; 
                facet->c= d3/r ; 
                facet->d= (-x0*d1+y0*d2-z0*d3)/r ; 
              }
    else      {
                facet->a= 0. ; 
                facet->b= 0. ; 
                facet->c= 0. ; 
                facet->d= 0. ;
                 return(-1) ;  
              }

#undef   DET

   return(0) ;
}


/********************************************************************/
/*								    */
/*                     �������� ����� ��� �������                   */  

    int RSS_Feature_Terrain::RecalcPoints(void)

{
  RSS_Kernel *Calc ;
    Matrix2d  Summary ;
    Matrix2d  Local ;
    Matrix2d  Point ;
         int  overall_recalc ;
         int  i ;
         int  j ;

#define   B   Bodies
#define   V   Bodies[i].Vertexes

/*------------------------------------------------------- ���������� */

           Calc=iGetCalculator() ;                                  /* ����������� ����������� */
        if(Calc==NULL)  return(-1) ;

/*---------------------------------------------- ��������� ����� ��� */

        overall_recalc=0 ;

   for(i=0 ; i<Bodies_cnt ; i++) {                                  /* CIRCLE.1 */

       if(!B[i].recalc)  continue ;                                 /* ���� ���� ��� �����������... */
           B[i].recalc=0 ;
        overall_recalc=1 ;
/*- - - - - - - - - - - - - - - - - - ������� ������� �������������� */
       if(B[i].Matrix_flag) {                                       /* ���� ������ �������... */
                Summary.LoadArray(4, 4, (double *)B[i].Matrix) ;
                            }
       else                 {                                       /* ���� ������ ���������� */
                Summary.LoadE      (4, 4) ;
                  Local.Load4d_base(B[i].x_base, B[i].y_base, B[i].z_base) ;
                Summary.LoadMul    (&Summary, &Local) ;
                  Local.Load4d_azim(-B[i].a_azim) ;
                Summary.LoadMul    (&Summary, &Local) ;
                  Local.Load4d_elev(-B[i].a_elev) ;
                Summary.LoadMul    (&Summary, &Local) ;
                  Local.Load4d_roll(-B[i].a_roll) ;
                Summary.LoadMul    (&Summary, &Local) ;
                            }

                  Local.Load4d_base(B[i].x_base_s, B[i].y_base_s, B[i].z_base_s) ;
                Summary.LoadMul    (&Summary, &Local) ;
                  Local.Load4d_azim(-B[i].a_azim_s) ;
                Summary.LoadMul    (&Summary, &Local) ;
                  Local.Load4d_elev(-B[i].a_elev_s) ;
                Summary.LoadMul    (&Summary, &Local) ;
                  Local.Load4d_roll(-B[i].a_roll_s) ;
                Summary.LoadMul    (&Summary, &Local) ;
/*- - - - - - - - - - - - - - - - - - - - -  ������� ��������� ����� */
     for(j=1 ; j<B[i].Vertexes_cnt ; j++) {

           if(V[j].x_calculate!=NULL)
                    Calc->vCalculate("STD_EXPRESSION", NULL,
                                       this->Bodies[i].extrn_pars, NULL,
                                       &V[j].x, &V[j].x_calculate, NULL ) ;

           if(V[j].y_calculate!=NULL)
                    Calc->vCalculate("STD_EXPRESSION", NULL,
                                       this->Bodies[i].extrn_pars, NULL,
                                       &V[j].y, &V[j].y_calculate, NULL ) ;

           if(V[j].z_calculate!=NULL)
                    Calc->vCalculate("STD_EXPRESSION", NULL,
                                       this->Bodies[i].extrn_pars, NULL,
                                       &V[j].z, &V[j].z_calculate, NULL ) ;

                      Point.LoadZero(4, 1) ;
                      Point.SetCell (0, 0, V[j].x) ;
                      Point.SetCell (1, 0, V[j].y) ;
                      Point.SetCell (2, 0, V[j].z) ;
                      Point.SetCell (3, 0,   1   ) ;

                      Point.LoadMul (&Summary, &Point) ;            /* ������������ ���������� ����� */
                                                                    /*  � ���������� �� */
           V[j].x_abs=Point.GetCell (0, 0) ;
           V[j].y_abs=Point.GetCell (1, 0) ;
           V[j].z_abs=Point.GetCell (2, 0) ;
           
                                          }
/*- - - - - - - - - - - - - - - - - - - - - - ������� ��������� ���� */
                        B[i].overall.x_min=V[1].x_abs ;
                        B[i].overall.x_max=V[1].x_abs ;
                        B[i].overall.y_min=V[1].y_abs ;
                        B[i].overall.y_max=V[1].y_abs ;
                        B[i].overall.z_min=V[1].z_abs ;
                        B[i].overall.z_max=V[1].z_abs ;

     for(j=1 ; j<B[i].Vertexes_cnt ; j++) {
              if(B[i].overall.x_min>V[j].x_abs)  B[i].overall.x_min=V[j].x_abs ;
         else if(B[i].overall.x_max<V[j].x_abs)  B[i].overall.x_max=V[j].x_abs ;
              if(B[i].overall.y_min>V[j].y_abs)  B[i].overall.y_min=V[j].y_abs ;
         else if(B[i].overall.y_max<V[j].y_abs)  B[i].overall.y_max=V[j].y_abs ;
              if(B[i].overall.z_min>V[j].z_abs)  B[i].overall.z_min=V[j].z_abs ;
         else if(B[i].overall.z_max<V[j].z_abs)  B[i].overall.z_max=V[j].z_abs ;
                                          }
/*- - - - - - - - - - - - - - ����� �������� ������� ��������� ����� */
     for(j=0 ; j<B[i].Facets_cnt ; j++) B[i].Facets[j].abcd_formed=0 ; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                       }                            /* CONTINUE.1 */
/*---------------------------------------------- ��������� ��������� */

    if(overall_recalc) {
                              overall.x_min= 1.e+99 ;
                              overall.x_max=-1.e+99 ;
                              overall.y_min= 1.e+99 ;
                              overall.y_max=-1.e+99 ;
                              overall.z_min= 1.e+99 ;
                              overall.z_max=-1.e+99 ;

      for(i=0 ; i<Bodies_cnt ; i++) {
        if(overall.x_min>B[i].overall.x_min)  overall.x_min=B[i].overall.x_min ;
        if(overall.x_max<B[i].overall.x_max)  overall.x_max=B[i].overall.x_max ;
        if(overall.y_min>B[i].overall.y_min)  overall.y_min=B[i].overall.y_min ;
        if(overall.y_max<B[i].overall.y_max)  overall.y_max=B[i].overall.y_max ;
        if(overall.z_min>B[i].overall.z_min)  overall.z_min=B[i].overall.z_min ;
        if(overall.z_max<B[i].overall.z_max)  overall.z_max=B[i].overall.z_max ;
                                    }
                       }
/*-------------------------------------------------------------------*/

  return(0) ;
}


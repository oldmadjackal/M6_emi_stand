/*********************************************************************/
/*								     */
/*		������ ���������� ������������ ��������	             */
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

#include "gl\gl.h"
#include "gl\glu.h"

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Kernel\RSS_Kernel.h"

#include "F_Show.h"

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

     static   RSS_Module_Show  ProgramModule ;


/*********************************************************************/
/*								     */
/*		    	����������������� ����                       */

 F_SHOW_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 F_SHOW_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/*********************************************************************/
/*********************************************************************/
/**							            **/
/**     �������� ������ ������ ���������� ������������ ��������     **/
/**							            **/
/*********************************************************************/
/*********************************************************************/

/*********************************************************************/
/*								     */
/*                            ������ ������                          */

  struct RSS_Module_Show_instr  RSS_Module_Show_InstrList[]={

 { "help",    "?", "#HELP   - ������ ��������� ������", 
                    NULL,
                   &RSS_Module_Show::cHelp       },
 { "color",   "c", "#COLOR   - ���������� ���� �������", 
                   " COLOR <���> <�������� �����>\n"
                   "   ���������� ���� ������� �� ��������: RED, GREEN, BLUE\n"
                   " COLOR <���> RGB <R-������>:<G-������>:<B-������>\n"
                   "   ���������� ���� ������� �� RGB-�����������\n",
                   &RSS_Module_Show::cColor      },
 { "visible", "v", "#VISIBLE - ���./����. ��������� �������", 
                   " VISIBLE <���> 0\n"
                   "   ������ ������\n"
                   " VISIBLE <���> 1\n"
                   "   �������� ������\n",
                   &RSS_Module_Show::cVisible    },
 {  NULL }
                                                            } ;

/*********************************************************************/
/*								     */
/*		     ����� ����� ������             		     */

    struct RSS_Module_Show_instr *RSS_Module_Show::mInstrList=NULL ;

/*********************************************************************/
/*								     */
/*		       ����������� ������			     */

     RSS_Module_Show::RSS_Module_Show(void)

{
	   keyword="EmiStand" ;
    identification="Show" ;
          category="Feature" ;

        mInstrList=RSS_Module_Show_InstrList ;
}


/*********************************************************************/
/*								     */
/*		        ���������� ������			     */

    RSS_Module_Show::~RSS_Module_Show(void)

{
}


/********************************************************************/
/*								    */
/*		        �������� ��������       		    */

     int  RSS_Module_Show::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- �������� ������ */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s -  ���������� ������������ �����", identification) ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/*********************************************************************/
/* 								     */
/*		        ��������� ��������                           */

    void  RSS_Module_Show::vStart(void)

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

    RSS_Feature *RSS_Module_Show::vCreateFeature(RSS_Object *object, RSS_Feature *feature_ext)

{
  RSS_Feature *feature ;


    if(feature_ext!=NULL)  feature= feature_ext ;
    else                   feature=new RSS_Feature_Show ;

         feature->Object=object ;

  return(feature) ;
}


/********************************************************************/
/*								    */
/*                    ���������� ��������� ������                   */

    void  RSS_Module_Show::vShow(char *layer)

{
   if(!stricmp(layer, "SCENE"))  this->iShowScene() ;
}


/*********************************************************************/
/*								     */
/*		        ��������� �������       		     */

  int  RSS_Module_Show::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* ���� ������ ������ ������� */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "SCENE"
#define  _SECTION_SHRT_NAME   "SC"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Scene features:") ;
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
         end=strchr(instr, ' ') ;
      if(end!=NULL) {  *end=0 ;  end++ ;  }
      else              end="" ;

   for(i=0 ; mInstrList[i].name_full!=NULL ; i++)                   /* ���� ������� � ������ */
     if(!stricmp(instr, mInstrList[i].name_full) ||
        !stricmp(instr, mInstrList[i].name_shrt)   )   break ;

     if(mInstrList[i].name_full==NULL) {                            /* ���� ����� ������� ���... */

          status=this->kernel->vExecuteCmd(cmd) ;                   /*  �������� �������� ������ ����... */
       if(status)  SEND_ERROR("������ SHOW: ����������� �������") ;
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

    void  RSS_Module_Show::vWriteSave(std::string *text)

{

/*----------------------------------------------- ��������� �������� */

     *text="#BEGIN MODULE SHOW\n" ;

/*------------------------------------------------ �������� �������� */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� Help                    */

  int  RSS_Module_Show::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Feature_Show_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*                   ���������� ���������� Color                    */
/*       COLOR  <���> <�������� �����>                              */
/*       COLOR  <���> RGB <R-������>:<G-������>:<B-������>          */

  int  RSS_Module_Show::cColor(char *cmd)

{ 
#define   _PARS_MAX   4
      char *pars[_PARS_MAX] ;
      char *name ;
  COLORREF  color ;
       int  red, green, blue ;
      char *end ;
      char  text[1024] ;
       int  i ;
       int  j ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*------------------------------------------------ ������ ���������� */        

    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }
/*----------------------------------------------------- ������ ����� */

     if(pars[1]==NULL) {
                         SEND_ERROR("�� ����� ���� �������. \n"
                                    "��������: COLOR <���_�������> GREEN") ;
                                        return(-1) ;
                       } 

              if(!stricmp(pars[1], "RED"  ))  color=RGB(127,   0,   0) ;
         else if(!stricmp(pars[1], "GREEN"))  color=RGB(  0, 127,   0) ;
         else if(!stricmp(pars[1], "BLUE" ))  color=RGB(  0,   0, 127) ;
         else if(!stricmp(pars[1], "RGB"  )) {

                 if(pars[2]!=NULL) {
                                       green=0 ;
                                        blue=0 ;
                                         red=strtoul(pars[2], &end, 10) ;
                         if(*end==':') green=strtoul(end+1  , &end, 10) ;
                         if(*end==':')  blue=strtoul(end+1  , &end, 10) ;

                                       color=RGB(red, green, blue) ;                                          
                                   }
                                             } 
         else                                {
                         SEND_ERROR("����������� �������� �����") ;
                                        return(-1) ;
                                             }
/*------------------------------------------- ����� ������� �� ����� */ 

       name=pars[0] ; 

    if(name==NULL) {                                                /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� �������. \n"
                                 "��������: COLOR <���_�������> ...") ;
                                     return(-1) ;
                   }

       for(i=0 ; i<OBJECTS_CNT ; i++)                               /* ���� ������ �� ����� */
         if(!stricmp(OBJECTS[i]->Name, name))  break ;

    if(i==OBJECTS_CNT) {                                            /* ���� ��� �� �������... */
                           sprintf(text, "������� � ������ '%s' "
                                         "�� ����������", name) ;
                        SEND_ERROR(text) ;
                            return(NULL) ;
                       }
/*---------------------------------- ���������� ����������� �������� */

    for(j=0 ; j<OBJECTS[i]->Features_cnt ; j++)
      if(!stricmp(OBJECTS[i]->Features[j]->Type, "Show"))  
            ((RSS_Feature_Show *)OBJECTS[i]->Features[j])->Color=color ;

/*-------------------------------------------------------------------*/

#undef    OBJECTS
#undef    OBJECTS_CNT

   return(0) ;
}


/********************************************************************/
/*								    */
/*                   ���������� ���������� Visible                  */
/*     VISIBLE  <���> 0                                             */
/*     VISIBLE  <���> 1                                             */

  int  RSS_Module_Show::cVisible(char *cmd)

{ 
#define   _PARS_MAX   4
      char *pars[_PARS_MAX] ;
      char *name ;
       int  visible ;
      char *end ;
      char  text[1024] ;
       int  i ;
       int  j ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*------------------------------------------------ ������ ���������� */        

    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

                                     name=pars[0] ;

               if(pars[1]==NULL)  pars[1]="1" ;
 
/*------------------------------------------- ����� ������� �� ����� */ 

    if(name==NULL) {                                                /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� �������. \n"
                                 "��������: VISIBLE <���_�������> ...") ;
                                     return(-1) ;
                   }

       for(i=0 ; i<OBJECTS_CNT ; i++)                               /* ���� ������ �� ����� */
         if(!stricmp(OBJECTS[i]->Name, name))  break ;

    if(i==OBJECTS_CNT) {                                            /* ���� ��� �� �������... */
                           sprintf(text, "������� � ������ '%s' "
                                         "�� ����������", name) ;
                        SEND_ERROR(text) ;
                            return(NULL) ;
                       }
/*-------------------------------------- �������� �������� ��������� */ 

         if(*pars[1]=='0' ||
            *pars[1]=='n' ||
            *pars[1]=='N' ||
            *pars[1]=='f' ||
            *pars[1]=='F'   )  visible=0 ;
         else                  visible=1 ;

/*---------------------------------- ���������� ����������� �������� */

    for(j=0 ; j<OBJECTS[i]->Features_cnt ; j++)
      if(!stricmp(OBJECTS[i]->Features[j]->Type, "Show"))  
            ((RSS_Feature_Show *)OBJECTS[i]->Features[j])->Visible=visible ;

/*-------------------------------------------------------------------*/

#undef    OBJECTS
#undef    OBJECTS_CNT

   return(0) ;
}


/********************************************************************/
/*								    */
/*                   ����������� �����                              */

  int  RSS_Module_Show::iShowScene(void)

{ 
  int  status ;
  int  i ;
  int  j ;
  
#define   OBJECTS       RSS_Kernel::kernel->kernel_objects 
#define   OBJECTS_CNT   RSS_Kernel::kernel->kernel_objects_cnt 

/*---------------------------------- ���������� ����������� �������� */

          status=RSS_Kernel::display.SetFirstContext("Show") ;
    while(status==0) {

       for(i=0 ; i<OBJECTS_CNT ; i++)
        for(j=0 ; j<OBJECTS[i]->Features_cnt ; j++)
          if(!stricmp(OBJECTS[i]->Features[j]->Type, "Show"))  
               ((RSS_Feature_Show *)OBJECTS[i]->Features[j])->Show() ;

          status=RSS_Kernel::display.SetNextContext("Show") ;
                     }
/*-------------------------------------------------------------------*/

#undef    OBJECTS
#undef    OBJECTS_CNT

   return(0) ;
}


/********************************************************************/
/********************************************************************/
/**							           **/
/**           �������� ������ ����������� �������                  **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       ����������� ������			    */

     RSS_Feature_Show::RSS_Feature_Show(void)

{
   strcpy(Type, "Show") ;

     Bodies    =NULL ;
     Bodies_cnt=  0 ;

    Visible    = 1 ;
      Color    =RGB(255, 255, 255) ;
}


/********************************************************************/
/*								    */
/*		        ���������� ������			    */

    RSS_Feature_Show::~RSS_Feature_Show(void)

{
}


/********************************************************************/
/*								    */
/*		        ������� ������ �� ������		    */

    void  RSS_Feature_Show::vReadSave(       char *title,  
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

       if(stricmp(title, "FEATURE SHOW"))  return ;

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

               Bodies=(RSS_Feature_Show_Body *)
                        realloc(Bodies, Bodies_cnt*sizeof(Bodies[0])) ;
            if(Bodies==NULL) {
                  sprintf(text, "Section FEATURE SHOW: "
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

                                                       }
/*---------------------------------------------------------- ������� */

     else if(!memicmp(work, "$VERTEX", strlen("$VERTEX"))) {

           if(body_num==-1)  continue ;
/*- - - - - - - - - - - - - - - - - - - - - ��������� ������ ������� */
               decl=work+strlen("$VERTEX") ;
                num=strtol(decl, &decl, 10) ;

           if(num<0 || *decl!='=') {
                sprintf(text, "Section FEATURE SHOW: "
                              "�������� ������ ������������� VERTEX\n - %s", work) ;
             SEND_ERROR(text) ;
                                        break ;
                                   }

               decl++ ;
/*- - - - - - - - - - - - - - - - - - - - -  ���������� ������ ����� */
           if(num>=VERTEXES_CNT) {

               VERTEXES=(RSS_Feature_Show_Vertex *)
                          realloc(VERTEXES, (num+1)*sizeof(*VERTEXES)) ;
            if(VERTEXES==NULL) {
                  SEND_ERROR("Section FEATURE SHOW: ������������ ������") ;
                                   break ;
                               }                    

                   memset(&VERTEXES[VERTEXES_CNT], 0, 
                               (num-VERTEXES_CNT+1)*sizeof(VERTEXES[0])) ;

                                    VERTEXES_CNT=num+1 ;
                                 }
/*- - - - - - - - - - - - - - - - - - - ��������� ���������� X, Y, Z */
               VERTEXES[num].x_formula=(char *)                     /* �������� ������ ��� �������� */
                                         calloc(1, strlen(decl)+8) ; 
            if(VERTEXES[num].x_formula==NULL) {
                  SEND_ERROR("Section FEATURE SHOW: ������������ ������") ;
                                   break ;
                                              }

                    strcpy(VERTEXES[num].x_formula, decl) ;
               sep=strrchr(VERTEXES[num].x_formula, ';') ;
            if(sep==NULL) {
                  sprintf(text, "Section FEATURE SHOW: "
                                "�������� ������ ������������� VERTEX\n - %s", work) ;
               SEND_ERROR(text) ;
                                        break ;
                          }
                                             *sep=0 ;
                           VERTEXES[num].z_formula=sep+1 ;
               sep=strrchr(VERTEXES[num].x_formula, ';') ;
            if(sep==NULL) {
                  sprintf(text, "Section FEATURE SHOW: "
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
                  sprintf(text, "Section FEATURE SHOW: ������ �����������\n\n"
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
                sprintf(text, "Section FEATURE SHOW: "
                              "�������� ������ ������������� FACET\n - %s", work) ;
             SEND_ERROR(text) ;
                                 break ;
                          }

              decl++ ;
/*- - - - - - - - - - - - - - - - - - - - - - ���������� ����� ����� */
              FACETS=(RSS_Feature_Show_Facet *)
                       realloc(FACETS, (FACETS_CNT+1)*sizeof(*FACETS)) ;
           if(FACETS==NULL) {
                  SEND_ERROR("Section FEATURE SHOW: ������������ ������") ;
                                break ;
                            }

                               FACETS_CNT++ ;
/*- - - - - - - - - - - - - - - - - - - - ������������ ������ ������ */
#define  F    FACETS[FACETS_CNT-1]

          for(i=0 ; i<_VERTEX_PER_FACET_MAX ; i++) {

                   F.vertexes[i]=strtol(decl, &decl, 10) ;

               if(*decl==0)  break ;
                   decl++ ;
                                                   }

              if(i==_VERTEX_PER_FACET_MAX) {
                  SEND_ERROR("Section FEATURE SHOW: ������� ����� ������ � �����") ;
                                break ;
                                           }

                    F.vertexes_cnt=i+1 ;
#undef   F
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                         }
/*----------------------------------------- ����������� ������������ */

     else                                                {

                sprintf(text, "Section FEATURE SHOW: "
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

    void RSS_Feature_Show::vGetInfo(std::string *text)

{
        *text="Show: Yes\r\n" ;
}


/********************************************************************/
/*								    */
/*                     ������ � �����������           		    */

     int RSS_Feature_Show::vParameter(char *name, char *action, char *value)

{
/*------------------------------------------------- ������� �������� */

    if(name  ==NULL)  return(-1) ;
    if(action==NULL)  return(-2) ;

/*-------------------------------------------------------- ��������� */

   if(!stricmp(name, "VISIBLE")) {

     if(!stricmp(action, "INVERT")) {
                                       this->Visible=!this->Visible ;
                                    }
     else                           {
                                         return(-2) ;
                                    }

                                         return( 0) ;
                                 }

/*-------------------------------------------------------------------*/

   return(-1) ;
}


/*********************************************************************/
/*								     */
/*                          ������� ����                             */

    void RSS_Feature_Show::vBodyDelete(char *body)
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

       if(this->Bodies[n_body].list_idx)                            /* ����������� ���������� ������ */                                       
           RSS_Kernel::display.ReleaseList(this->Bodies[n_body].list_idx) ;

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

    void RSS_Feature_Show::vBodyBasePoint(  char *body,                   
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

       Bodies[n_body].x_base=x ;
       Bodies[n_body].y_base=y ;
       Bodies[n_body].z_base=z ;

       Bodies[n_body].Matrix_flag=0 ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*                     ������� ���������� ����                      */

    void RSS_Feature_Show::vBodyAngles(  char *body,                   
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

       Bodies[n_body].a_azim=azim ;
       Bodies[n_body].a_elev=elev ;
       Bodies[n_body].a_roll=roll ;

       Bodies[n_body].Matrix_flag=0 ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*               ������� ������� ��������� ����                     */

    void RSS_Feature_Show::vBodyMatrix(  char *body,
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

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*           ������� �������� ��������� � ���������� ����           */

    void RSS_Feature_Show::vBodyShifts(   char *body,                   
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

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*              ������� ������ ���������� ����                      */

    void RSS_Feature_Show::vBodyPars(char *body, 
                                      struct RSS_Parameter *pars)
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
/*                       �������� ������ � �����                    */

    void  RSS_Feature_Show::Show(void)

{
              RSS_Kernel *Calc ;
 RSS_Feature_Show_Vertex  n_vector ;
                GLdouble  matrix[16] ;
                     int  i ;
                     int  j ;
                     int  k ;

/*------------------------------------------------- ������� �������� */

     if(this->Bodies    ==NULL ||
        this->Bodies_cnt==  0    )  return ;

/*----------------------------------------------------- ������� ���� */

     if(this->Visible==0) {

      for(i=0 ; i<this->Bodies_cnt ; i++)                           /* ����������� ���������� ������ */
        if(this->Bodies[i].list_idx) {                              /*   ���� ��� �������            */
            RSS_Kernel::display.ReleaseList(this->Bodies[i].list_idx) ;
                                  this->Bodies[i].list_idx=0 ;
                                     }

                               return ;
                          }
/*------------------------------------------------------ ������� ��� */

   for(i=0 ; i<this->Bodies_cnt ; i++) {                            /* CIRCLE.0 */
/*- - - - - - - - - - - - - - - -  �������������� ����������� ������ */
     if(this->Bodies[i].list_idx==0) {

           this->Bodies[i].list_idx=RSS_Kernel::display.GetList(0) ;
        if(this->Bodies[i].list_idx==0)  continue ;
                                     }
/*- - - - - - - - - - - - - - - - - - - �������� ������ ������������ */
/*- - - - - - - - - - - - - - - - - - - - - - - - -  �������� ������ */
             glNewList(Bodies[i].list_idx, GL_COMPILE) ;
/*- - - - - - - - - - - - - - - - - - -  ���������� � ��������� ���� */
          glMatrixMode(GL_MODELVIEW) ;
//      glLoadIdentity() ;

    if(Bodies[i].Matrix_flag) {

                        matrix[ 0]=Bodies[i].Matrix[0][0] ;
                        matrix[ 1]=Bodies[i].Matrix[1][0] ;
                        matrix[ 2]=Bodies[i].Matrix[2][0] ;
                        matrix[ 3]=Bodies[i].Matrix[3][0] ;
                        matrix[ 4]=Bodies[i].Matrix[0][1] ;
                        matrix[ 5]=Bodies[i].Matrix[1][1] ;
                        matrix[ 6]=Bodies[i].Matrix[2][1] ;
                        matrix[ 7]=Bodies[i].Matrix[3][1] ;
                        matrix[ 8]=Bodies[i].Matrix[0][2] ;
                        matrix[ 9]=Bodies[i].Matrix[1][2] ;
                        matrix[10]=Bodies[i].Matrix[2][2] ;
                        matrix[11]=Bodies[i].Matrix[3][2] ;
                        matrix[12]=Bodies[i].Matrix[0][3] ;
                        matrix[13]=Bodies[i].Matrix[1][3] ;
                        matrix[14]=Bodies[i].Matrix[2][3] ;
                        matrix[15]=Bodies[i].Matrix[3][3] ;
         glMultMatrixd( matrix) ;
                              }
    else                      {
          glTranslated( Bodies[i].x_base,
                        Bodies[i].y_base, 
                        Bodies[i].z_base) ;
             glRotated( Bodies[i].a_azim, 0., 1., 0.) ;
             glRotated(-Bodies[i].a_elev, 1., 0., 0.) ;
             glRotated(-Bodies[i].a_roll, 0., 0., 1.) ;
                              }

          glTranslated( Bodies[i].x_base_s,
                        Bodies[i].y_base_s, 
                        Bodies[i].z_base_s) ;
             glRotated( Bodies[i].a_azim_s, 0., 1., 0.) ;
             glRotated(-Bodies[i].a_elev_s, 1., 0., 0.) ;
             glRotated(-Bodies[i].a_roll_s, 0., 0., 1.) ;
/*- - - - - - - - - - - - - - - - - - -  ���������� ��������� ������ */
#define   V        Bodies[i].Vertexes
#define   V_CNT    Bodies[i].Vertexes_cnt

    if(Visible) {
                   Calc=iGetCalculator() ;                          /* ����������� ����������� */

         for(j=0 ; j<V_CNT ; j++) {

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
                                  }
                }
/*- - - - - - - - - - - - - - - - - - - - - - - - - ����������� ���� */
#define   F        Bodies[i].Facets
#define   F_CNT    Bodies[i].Facets_cnt

    if(Visible) {

     for(j=0 ; j<F_CNT ; j++) {                                     /* CIRCLE.1 - ������� ������ */

       if(this->Object->land_state)  glColor3d(0.5, 0.5, 0.5) ;
       else                          glColor3d(0.0, 0.0, 0.0) ;

          glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) ;
                glBegin(GL_POLYGON) ;
      for(k=0 ; k<F[j].vertexes_cnt  ; k++)                         /* ������� ����� ����� */
             glVertex3d(V[F[j].vertexes[k]].x,
                        V[F[j].vertexes[k]].y,
                        V[F[j].vertexes[k]].z ) ;
	          glEnd();
          glPolygonMode(GL_FRONT_AND_BACK, GL_FILL) ;

        glPolygonOffset(1., 1.) ;
              glColor3d(GetRValue(Color)/256., 
                        GetGValue(Color)/256.,
                        GetBValue(Color)/256. ) ;

                glBegin(GL_POLYGON) ;

            iCalcNormal(&V[F[j].vertexes[0]], 
                        &V[F[j].vertexes[1]],
                        &V[F[j].vertexes[2]], &n_vector) ;
             glNormal3d(n_vector.x, n_vector.y, n_vector.z) ;

      for(k=0 ; k<F[j].vertexes_cnt  ; k++)                         /* ������� ����� ����� */
             glVertex3d(V[F[j].vertexes[k]].x,
                        V[F[j].vertexes[k]].y,
                        V[F[j].vertexes[k]].z ) ;

	          glEnd();
        glPolygonOffset(0., 0.) ;
                              }                                     /* CONTINUE.1 */

                }
/*- - - - - - - - - - - - - - - - - - - - - - - - -  �������� ������ */
             glEndList() ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                       }                            /* CONTINUE.0 */
/*-------------------------------------------------------------------*/

#undef   F
#undef   F_CNT
#undef   V
#undef   V_CNT

}


/********************************************************************/
/*								    */
/*                     ��������� ������� � ���������                */

  void  RSS_Feature_Show::iCalcNormal(RSS_Feature_Show_Vertex *point_0,
                                      RSS_Feature_Show_Vertex *point_1,
                                      RSS_Feature_Show_Vertex *point_2,
                                      RSS_Feature_Show_Vertex *normal  )

{
  double  x1 ;
  double  y1 ;
  double  z1 ;
  double  x2 ;
  double  y2 ;
  double  z2 ;
  double  len ;

               x1=point_1->x-point_0->x ;
               y1=point_1->y-point_0->y ;
               z1=point_1->z-point_0->z ;

               x2=point_2->x-point_0->x ;
               y2=point_2->y-point_0->y ;
               z2=point_2->z-point_0->z ;

        normal->x=y1*z2-z1*y2 ;
        normal->y=z1*x2-x1*z2 ;
        normal->z=x1*y2-y1*x2 ;

              len=sqrt(normal->x*normal->x+
                       normal->y*normal->y+
                       normal->z*normal->z ) ;

        normal->x/=-len ;
        normal->y/=-len ;
        normal->z/=-len ;
}


/********************************************************************/
/*								    */
/*            ����������� ������� �����������                       */

  RSS_Kernel *RSS_Feature_Show::iGetCalculator(void)

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


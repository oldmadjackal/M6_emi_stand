/*********************************************************************/
/*								     */
/*	������ ���������� ������������ ��������� ��������            */
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

#include "F_Hit.h"

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

     static   RSS_Module_Hit  ProgramModule ;


/*********************************************************************/
/*								     */
/*		    	����������������� ����                       */

 F_HIT_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 F_HIT_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/*********************************************************************/
/*********************************************************************/
/**							            **/
/**               �������� ������ ������ ����������                 **/
/**	           ������������ ����������� ��������	            **/
/**							            **/
/*********************************************************************/
/*********************************************************************/

/*********************************************************************/
/*								     */
/*                            ������ ������                          */

  struct RSS_Module_Hit_instr  RSS_Module_Hit_InstrList[]={

 { "help",     "?",  "#HELP   - ������ ��������� ������", 
                      NULL,
                     &RSS_Module_Hit::cHelp       },
 { "overall",  "o",  "#OVERALL (O) - ������������ ��� ������� ���������, ������� ���������", 
                     " OVERALL 0 \n"
                     "   ������������ ������ ���������� ����\n"
                     " OVERALL 1 \n"
                     "   ������������ ��� ������� ���������, ������� ��������� ��������� �����\n",
                     &RSS_Module_Hit::cOverall     },
 { "radius",   "r",  "#RADIUS (R) - ������ ���������", 
                      NULL,
                     &RSS_Module_Hit::cRadius     },
 { "category", "c",  "#CATEGORY (C) - ��������� �������", 
                     " CATEGORY/T <���>\n"
                     "   ���������������� ������ ��� ����������� ���������� ����\n"
                     " CATEGORY/W <���>\n"
                     "   ���������������� ������ ��� �������� ������������� ���������\n"
                     " CATEGORY/WT <���>\n"
                     "   ���������������� ������ ��� �������� ������������� ��������� ���������� ������������ � ����������� ���������� �����\n",
                     &RSS_Module_Hit::cCategory     },
 {  NULL }
                                                              } ;

/*********************************************************************/
/*								     */
/*		     ����� ����� ������             		     */

    struct RSS_Module_Hit_instr *RSS_Module_Hit::mInstrList=NULL ;


/*********************************************************************/
/*								     */
/*		       ����������� ������			     */

     RSS_Module_Hit::RSS_Module_Hit(void)

{
	   keyword="EmiStand" ;
    identification="Hit" ;
          category="Feature" ;

        mInstrList=RSS_Module_Hit_InstrList ;
}


/*********************************************************************/
/*								     */
/*		        ���������� ������			     */

    RSS_Module_Hit::~RSS_Module_Hit(void)

{
}


/*********************************************************************/
/* 								     */
/*		        ��������� ��������                           */

    void  RSS_Module_Hit::vStart(void)

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

    RSS_Feature *RSS_Module_Hit::vCreateFeature(RSS_Object *object, RSS_Feature *feature_ext)

{
  RSS_Feature *feature ;


    if(feature_ext!=NULL)  feature= feature_ext ;
    else                   feature=new RSS_Feature_Hit ;

         feature->Object=object ;

  return(feature) ;
}


/*********************************************************************/
/*								     */
/*		        ��������� �������       		     */

  int  RSS_Module_Hit::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* ���� ������ ������ ������� */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "HIT"
#define  _SECTION_SHRT_NAME   "HIT"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Hit features:") ;
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
       if(status)  SEND_ERROR("������ HIT: ����������� �������") ;
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

    void  RSS_Module_Hit::vWriteSave(std::string *text)

{

/*----------------------------------------------- ��������� �������� */

     *text="#BEGIN MODULE HIT\n" ;

/*------------------------------------------------ �������� �������� */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/

#undef   MARKS_LIST
#undef   MARKS_LIST_CNT

}


/********************************************************************/
/*								    */
/*		        ������� ������ �� ������		    */

    void  RSS_Module_Hit::vReadSave(std::string *data)

{
                    char *buff ;
                     int  buff_size ;
             std::string  decl ;
                    char *work ;
                    char  text[1024] ;
                    char *end ;

/*----------------------------------------------- �������� ��������� */

   if(memicmp(data->c_str(), "#BEGIN MODULE HIT\n", 
                      strlen("#BEGIN MODULE HIT\n")))  return ;

/*------------------------------------------------ ���������� ������ */

              buff_size=(int)data->size()+16 ;
              buff     =(char *)calloc(1, buff_size) ;

       strcpy(buff, data->c_str()) ;
        
/*---------------------------------------------- ���������� �������� */

                work=buff+strlen("#BEGIN MODULE HIT\n") ;

    for( ; ; work=end+1) {                                          /* CIRCLE.0 */
                                      end=strchr(work, '\n') ;
                                   if(end==NULL)  break ;
                                     *end=0 ;

/*---------------------------------------------------------- ??????? */

          if(!memicmp(work, "??????", strlen("??????"))) {

                                                         }
/*----------------------------------------- ����������� ������������ */

     else                                                {

                sprintf(text, "Module HIT: "
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

  int  RSS_Module_Hit::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Feature_Hit_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*                   ���������� ���������� Overall                  */
/*     OVERALL <���> <Flag>                                         */

  int  RSS_Module_Hit::cOverall(char *cmd)

{ 
#define   _PARS_MAX   4
        char *pars[_PARS_MAX] ;
        char *end ;
         int  i ;

/*------------------------------------------------ ������ ���������� */        

    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }
/*----------------------------------------------- ����������� ������ */ 

    if(pars[0]==NULL) {                                             /* ���� ����� �� �����... */
                        SEND_ERROR("�� ����� ����� �������� ���������. \n"
                                   "��������: HIT OVERALL 1"      ) ;
                                      return(-1) ;
                      }

         if(!stricmp(pars[0], "0"))  RSS_Feature_Hit::any_hit=0 ;
    else if(!stricmp(pars[0], "1"))  RSS_Feature_Hit::any_hit=1 ;
    else                           {
                    SEND_ERROR("������������ ����� �������� ��������� - ����� ���� 0 ��� 1.") ;
                                     return(-1) ;
                                   }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*                   ���������� ���������� Radius                   */
/*								    */
/*     RADIUS <���> <������ ���������>                              */

  int  RSS_Module_Hit::cRadius(char *cmd)

{ 
#define   _PARS_MAX   4
  RSS_Object *object ;
        char *pars[_PARS_MAX] ;
        char *name ;
      double  value ;
        char *end ;
        char  text[1024] ;
         int  i ;

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

/*------------------------------------------- ����� ������� �� ����� */

    if(name==NULL) {                                                /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� �������. \n"
                                 "��������: RADIUS <���_�������> ...") ;
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
                            return(NULL) ;
                       }
/*------------------------------------ ����������� ������� ��������� */ 

    if(pars[1]==NULL) {                                             /* ���� ������ ��������� �� �����... */
                        SEND_ERROR("�� �����  ������ ���������.") ;
                                     return(-1) ;
                      }

            value=strtod(pars[1], &end) ;

    if(*end!=0) {
                    SEND_ERROR("������������ ��������  ������� ���������.") ;
                                     return(-1) ;
                }
/*------------------------------------- ���������� ������� ��������� */

   for(i=0 ; i<object->Features_cnt ; i++)
     if(!stricmp(object->Features[i]->Type, "Hit")) 
          ((RSS_Feature_Hit *)(object->Features[i]))->hit_range=value ;

/*-------------------------------------------------------------------*/

#undef    OBJECTS
#undef    OBJECTS_CNT

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� CATEGORY                */
/*								    */
/*        CATEGORY    <���>                                         */
/*        CATEGORY/T  <���>                                         */
/*        CATEGORY/W  <���>                                         */
/*        CATEGORY/TW <���>                                         */

  int  RSS_Module_Hit::cCategory(char *cmd)

{
#define   _PARS_MAX   4

               char *pars[_PARS_MAX] ;
               char *name ;
         RSS_Object *object ;
                int  target_flag ;
                int  weapon_flag ;
               char  text[1024] ;
               char *end ;
                int  i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
                      target_flag=0 ;
                      weapon_flag=0 ;

       if(*cmd=='/' ||
          *cmd=='+'   ) {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("������������ ������ �������") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, 't')!=NULL ||
                   strchr(cmd, 'T')!=NULL   )  target_flag=1 ;
                if(strchr(cmd, 'w')!=NULL ||
                   strchr(cmd, 'W')!=NULL   )  weapon_flag=1 ;

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

/*------------------------------------------- ����� ������� �� ����� */

    if(name==NULL) {                                                /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� �������. \n"
                                 "��������: CATEGORY <���_�������> ...") ;
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
                            return(NULL) ;
                       }
/*--------------------------------------------- ���������� ��������� */

   for(i=0 ; i<object->Features_cnt ; i++)
     if(!stricmp(object->Features[i]->Type, "Hit")) {
          ((RSS_Feature_Hit *)(object->Features[i]))->any_target=target_flag ;
          ((RSS_Feature_Hit *)(object->Features[i]))->any_weapon=weapon_flag ;
                                                    }
/*-------------------------------------------------------------------*/

#undef    OBJECTS
#undef    OBJECTS_CNT
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/********************************************************************/
/**							           **/
/**           �������� ������ ���������� ������� �������           **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       ����������� ����������			    */

         int  RSS_Feature_Hit::any_hit  =  0 ;
         int *RSS_Feature_Hit::hit_count=NULL ;


/********************************************************************/
/*								    */
/*		       ����������� ������			    */

     RSS_Feature_Hit::RSS_Feature_Hit(void)

{
   strcpy(Type, "Hit") ;

     hit_done  =  0 ;

     Bodies    =NULL ;
     Bodies_cnt=  0 ;

     hit_range =  0. ;
     any_target=  0 ;
     any_weapon=  0 ;

   memset(target, 0, sizeof(target)) ;

      track_flag=0 ;
}


/********************************************************************/
/*								    */
/*		        ���������� ������			    */

    RSS_Feature_Hit::~RSS_Feature_Hit(void)

{
}


/********************************************************************/
/*								    */
/*		        ������� ������ �� ������		    */

    void  RSS_Feature_Hit::vReadSave(       char *title,  
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

       if(stricmp(title, "FEATURE HIT"))  return ;

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

               Bodies=(RSS_Feature_Hit_Body *)
                        realloc(Bodies, Bodies_cnt*sizeof(Bodies[0])) ;
            if(Bodies==NULL) {
                  sprintf(text, "Section FEATURE HIT: "
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
                sprintf(text, "Section FEATURE HIT: "
                              "�������� ������ ������������� VERTEX\n - %s", work) ;
             SEND_ERROR(text) ;
                                        break ;
                                   }

               decl++ ;
/*- - - - - - - - - - - - - - - - - - - - -  ���������� ������ ����� */
           if(num>=VERTEXES_CNT) {

               VERTEXES=(RSS_Feature_Hit_Vertex *)
                          realloc(VERTEXES, (num+1)*sizeof(*VERTEXES)) ;
            if(VERTEXES==NULL) {
                  SEND_ERROR("Section FEATURE HIT: ������������ ������") ;
                                   break ;
                               }                    

                                    VERTEXES_CNT=num+1 ;
                                 }
/*- - - - - - - - - - - - - - - - - - - ��������� ���������� X, Y, Z */
               VERTEXES[num].x_formula=(char *)                     /* �������� ������ ��� �������� */
                                         calloc(1, strlen(decl)+8) ; 
            if(VERTEXES[num].x_formula==NULL) {
                  SEND_ERROR("Section FEATURE HIT: ������������ ������") ;
                                   break ;
                                              }

                    strcpy(VERTEXES[num].x_formula, decl) ;
               sep=strrchr(VERTEXES[num].x_formula, ';') ;
            if(sep==NULL) {
                  sprintf(text, "Section FEATURE HIT: "
                                "�������� ������ ������������� VERTEX\n - %s", work) ;
               SEND_ERROR(text) ;
                                        break ;
                          }
                                             *sep=0 ;
                           VERTEXES[num].z_formula=sep+1 ;
               sep=strrchr(VERTEXES[num].x_formula, ';') ;
            if(sep==NULL) {
                  sprintf(text, "Section FEATURE HIT: "
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
                  sprintf(text, "Section FEATURE HIT: ������ �����������\n\n"
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
                sprintf(text, "Section FEATURE HIT: "
                              "�������� ������ ������������� FACET\n - %s", work) ;
             SEND_ERROR(text) ;
                                 break ;
                          }

              decl++ ;
/*- - - - - - - - - - - - - - - - - - - - - - ���������� ����� ����� */
              FACETS=(RSS_Feature_Hit_Facet *)
                       realloc(FACETS, (FACETS_CNT+1)*sizeof(*FACETS)) ;
           if(FACETS==NULL) {
                  SEND_ERROR("Section FEATURE HIT: ������������ ������") ;
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
                  SEND_ERROR("Section FEATURE HIT: ������� ����� ������ � �����") ;
                                break ;
                                           }

                    F.vertexes_cnt=i+1 ;
#undef   F
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                         }
/*----------------------------------------- ����������� ������������ */

     else                                                {

                sprintf(text, "Section FEATURE HIT: "
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

    void RSS_Feature_Hit::vGetInfo(std::string *text)

{
        if(any_target && any_weapon)  *text="Hit: Weapon/Target\r\n" ;
   else if(any_target              )  *text="Hit: Target\r\n" ;
   else if(any_weapon              )  *text="Hit: Weapon\r\n" ;
   else                               *text="Hit: Ignore\r\n" ;
}


/*********************************************************************/
/*								     */
/*                          ������� ����                             */

    void RSS_Feature_Hit::vBodyDelete(char *body)
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

    void RSS_Feature_Hit::vBodyBasePoint(  char *body,                   
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

    void RSS_Feature_Hit::vBodyAngles(  char *body,                   
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

    void RSS_Feature_Hit::vBodyMatrix(  char *body,
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

    void RSS_Feature_Hit::vBodyShifts(   char *body,                   
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

    void RSS_Feature_Hit::vBodyPars(char *body, 
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
/*            ����������� ������� �����������                       */

  RSS_Kernel *RSS_Feature_Hit::iGetCalculator(void)

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

    int RSS_Feature_Hit::vResetCheck(void *data)

{
           hit_done=0 ;

   track_s    .mark=0 ;
   track_s_prv.mark=0 ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*       ���������� � �������� ������������������ ��������          */  

    int RSS_Feature_Hit::vPreCheck(void *data)

{
      if(this->any_weapon==0 &&                                     /* ���� ������ �� ����� ����������� �� Hit-�������� */
         this->any_target==0 &&
         this->target[0] ==0   )  return(0) ;

      if(track_s.mark)  track_s_prv=track_s ;

         this->RecalcPoints() ;

              track_s.mark=1 ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*                �������� ������������������ ��������              */  

    int RSS_Feature_Hit::vCheck(void *data, RSS_Objects_List *checked)

{
 RSS_Object *object ;
        int  n ;            /* ������ ������� � ������ ��������*/
        int  m ;            /* ������ �������� "�����������" � ������ ������� */
        int  hit_flag ;
        int  i ;

#define  OBJECTS_CNT                     RSS_Kernel::kernel_objects_cnt
#define  OBJECTS                         RSS_Kernel::kernel_objects
#define  HIT         ((RSS_Feature_Hit *)object->Features[m])

/*------------------------------------------------ ������� ��������  */

      if(this->any_weapon==0 &&                                     /* ���� ���� �� ���������... */
         this->target[0] ==0   )  return(0) ;

      if(this->track_s_prv.mark==0)  return(0)  ;                   /* ���� ��������� ����� �����������... */

/*------------------------------------------------- ������ ��������� */

                                      hit_flag=0 ;

   for(n=0 ; n<OBJECTS_CNT ; n++) {                                 /* CIRCLE.1 - ������� �������� */

           object=OBJECTS[n] ;

     if(this->target[0]!=0)                                         /* ���� ����������� ������ � �������� �����... */
      if(strcmp(object->Name, this->target))  continue ;            

    for(m=0 ; m<object->Features_cnt ; m++)                         /* ���������, ������� �� � ������ ��������         */
      if(!strcmp(object->Features[m]->Type, this->Type ))  break ;  /*  �������� "�����������", ���� ��� - ������� ��� */

      if(m==object->Features_cnt)  continue ;                       /* ���� ������-���� �� �������� ��������� Hit... */

     if(HIT->track_s_prv.mark==0)  continue  ;                      /* ���� ��������� ����� ����������� ��� ����... */

     if(HIT->Bodies_cnt==0)  continue ;                             /* ���� �������� Hit - ������ */

     if(this->target[0]==0)                                         /* ���� ����������� � ������������ ����� � ��� �� ���� */
      if(HIT->any_target==0)  continue ;

      if(!iOverallTest(HIT))  continue ;                            /* ���� �������� �������� �� ������������... */
/*- - - - - - - - - - - - - - - - - - - - - - �������� ��������� ��� */
     for(i=0 ; i< HIT->Bodies_cnt ; i++) {                          /* CIRCLE.2 - ������� ��� ����  */

//       if(!iFacetsTest(HIT->Bodies[i] ))  continue ;

         checked->Add(object, this->Type) ;                         /* ������������ CHECK-����� */

       if(!            HIT->hit_done ||                             /* ������������ ����� ������� ��������� */
           RSS_Feature_Hit::any_hit    )
         if(hit_count!=NULL) (*hit_count)++ ;

                          HIT->hit_done=1 ;

                       hit_flag=1 ;
                          break ;
                                         }                          /* CONTINUE.2 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                          break ;
                                  }                                 /* CONTINUE.1 */
/*-------------------------------------------------------------------*/

#undef   OBJECTS_CNT
#undef   OBJECTS
#undef   HIT

   return(hit_flag) ;
}


/********************************************************************/
/*								    */
/*                    �������� ����������� ���������                */ 

    int RSS_Feature_Hit::iOverallTest(RSS_Feature_Hit *trgt)
{
   double  s_min, s_max ;
// double  d_min, d_max ;
   double  t_min, t_max ;

                                               s_min =this->track_s    .x ;
                                               s_max =this->track_s_prv.x ;
  if(this->track_s.x > this->track_s_prv.x) {  s_min =this->track_s_prv.x ;
                                               s_max =this->track_s    .x ;  }

                                               t_min =trgt->track_s    .x ;
                                               t_max =trgt->track_s_prv.x ;
  if(trgt->track_s.x > trgt->track_s_prv.x) {  t_min =trgt->track_s_prv.x ;
                                               t_max =trgt->track_s    .x ;  }

                                               s_min-= this->hit_range ;
                                               s_max+= this->hit_range ;
                                               t_min+=(trgt->overall.x_min-trgt->track_s.x) ;
                                               t_max+=(trgt->overall.x_max-trgt->track_s.x) ;
 
  if(s_min>t_max || s_max<t_min)  return(0) ;     

                                               s_min =this->track_s    .y ;
                                               s_max =this->track_s_prv.y ;
  if(this->track_s.y > this->track_s_prv.y) {  s_min =this->track_s_prv.y ;
                                               s_max =this->track_s    .y ;  }

                                               t_min =trgt->track_s    .y ;
                                               t_max =trgt->track_s_prv.y ;
  if(trgt->track_s.y > trgt->track_s_prv.y) {  t_min =trgt->track_s_prv.y ;
                                               t_max =trgt->track_s    .y ;  }

                                               s_min-= this->hit_range ;
                                               s_max+= this->hit_range ;
                                               t_min+=(trgt->overall.y_min-trgt->track_s.y) ;
                                               t_max+=(trgt->overall.y_max-trgt->track_s.y) ;
 
  if(s_min>t_max || s_max<t_min)  return(0) ;     

                                               s_min =this->track_s    .z ;
                                               s_max =this->track_s_prv.z ;
  if(this->track_s.z > this->track_s_prv.z) {  s_min =this->track_s_prv.z ;
                                               s_max =this->track_s    .z ;  }

                                               t_min =trgt->track_s    .z ;
                                               t_max =trgt->track_s_prv.z ;
  if(trgt->track_s.z > trgt->track_s_prv.z) {  t_min =trgt->track_s_prv.z ;
                                               t_max =trgt->track_s    .z ;  }

                                               s_min-= this->hit_range ;
                                               s_max+= this->hit_range ;
                                               t_min+=(trgt->overall.z_min-trgt->track_s.z) ;
                                               t_max+=(trgt->overall.z_max-trgt->track_s.z) ;
 
  if(s_min>t_max || s_max<t_min)  return(0) ;     

     return(1) ;
}


/********************************************************************/
/*								    */
/*                �������� ����������� ������ ���� ����             */ 

    int RSS_Feature_Hit::iFacetsTest(RSS_Feature_Hit_Body *body)
{
#ifdef _REMARK_

    Matrix2d  SummaryA ;
    Matrix2d  SummaryP ;
    Matrix2d  Point ;
      double  x0, y0, z0 ;
      double  x1, y1, z1 ;
      double  x2, y2, z2 ;
      double  c_x[2], c_z[2] ;    /* ����� ����������� ����� � ���������� ����� */
      double  y ;
      double  v1, v2 ;
         int  cross ;
         int  cross1, cross2 ;
         int  n ;
         int  m1[2] ;  /* ������� ������ ����� "���������" ����, */
         int  m2[2] ;  /*  ������������ ����� "����������" ����  */
         int  m ;
         int  ma ;     /* ������ "������" ������� ����� "����������" ����, ������������� ����� "���������" ���� */
         int  i ;

#define  F1     (b1->Facets[n1])
#define  F2     (b2->Facets[n2])
#define  V1      b1->Vertexes
#define  V2      b2->Vertexes

/*------------------------------------------------------- ���������� */

/*---------------------------------------------- ������� ������ ���� */

    for(n=0 ; n<body->Facets_cnt ; n++) {

         F1.overall.x_min=V1[F1.vertexes[0]].x_abs ;                /* ����������� ����������� ������� */
         F1.overall.x_max=V1[F1.vertexes[0]].x_abs ;
         F1.overall.y_min=V1[F1.vertexes[0]].y_abs ;
         F1.overall.y_max=V1[F1.vertexes[0]].y_abs ;
         F1.overall.z_min=V1[F1.vertexes[0]].z_abs ;
         F1.overall.z_max=V1[F1.vertexes[0]].z_abs ;

     for(i=1 ; i<F1.vertexes_cnt ; i++) {
       if(V1[F1.vertexes[i]].x_abs<F1.overall.x_min)  F1.overall.x_min=V1[F1.vertexes[i]].x_abs ;
       if(V1[F1.vertexes[i]].x_abs>F1.overall.x_max)  F1.overall.x_max=V1[F1.vertexes[i]].x_abs ;
       if(V1[F1.vertexes[i]].y_abs<F1.overall.y_min)  F1.overall.y_min=V1[F1.vertexes[i]].y_abs ;
       if(V1[F1.vertexes[i]].y_abs>F1.overall.y_max)  F1.overall.y_max=V1[F1.vertexes[i]].y_abs ;
       if(V1[F1.vertexes[i]].z_abs<F1.overall.z_min)  F1.overall.z_min=V1[F1.vertexes[i]].z_abs ;
       if(V1[F1.vertexes[i]].z_abs>F1.overall.z_min)  F1.overall.z_max=V1[F1.vertexes[i]].z_abs ;
                                        }
/*--------------------------------- ������� ������ "����������" ���� */

      for(n2=0 ; n2<b2->Facets_cnt ; n2++) {
/*- - - - - - - - - - - - - - - - - - - - - -  �������� �� ��������� */
             F2.overall.x_min=V2[F2.vertexes[0]].x_abs ;            /* ����������� ����������� ������� */
             F2.overall.x_max=V2[F2.vertexes[0]].x_abs ;
             F2.overall.y_min=V2[F2.vertexes[0]].y_abs ;
             F2.overall.y_max=V2[F2.vertexes[0]].y_abs ;
             F2.overall.z_min=V2[F2.vertexes[0]].z_abs ;
             F2.overall.z_max=V2[F2.vertexes[0]].z_abs ;

         for(i=1 ; i<F2.vertexes_cnt ; i++) {
           if(V2[F2.vertexes[i]].x_abs<F2.overall.x_min)  F2.overall.x_min=V2[F2.vertexes[i]].x_abs ;
           if(V2[F2.vertexes[i]].x_abs>F2.overall.x_max)  F2.overall.x_max=V2[F2.vertexes[i]].x_abs ;
           if(V2[F2.vertexes[i]].y_abs<F2.overall.y_min)  F2.overall.y_min=V2[F2.vertexes[i]].y_abs ;
           if(V2[F2.vertexes[i]].y_abs>F2.overall.y_max)  F2.overall.y_max=V2[F2.vertexes[i]].y_abs ;
           if(V2[F2.vertexes[i]].z_abs<F2.overall.z_min)  F2.overall.z_min=V2[F2.vertexes[i]].z_abs ;
           if(V2[F2.vertexes[i]].z_abs>F2.overall.z_min)  F2.overall.z_max=V2[F2.vertexes[i]].z_abs ;
                                            }

           if(F1.overall.x_min>F2.overall.x_max || 
              F1.overall.x_max<F2.overall.x_min ||
              F1.overall.y_min>F2.overall.y_max ||
              F1.overall.y_max<F2.overall.y_min ||
              F1.overall.z_min>F2.overall.z_max || 
              F1.overall.z_max<F2.overall.z_min   )  continue ;
/*- - - - - - - - - - - - - - �������� ����������� ��������� �-����� */
                   SummaryA.LoadZero(4,4) ;

              iToFlat(&V1[F1.vertexes[0]],
                      &V1[F1.vertexes[1]],
                      &V1[F1.vertexes[2]], &SummaryA) ;

                          cross=0 ;

         for(i=0 ; i<F2.vertexes_cnt ; i++) {

                 Point.LoadZero(4, 1) ;
                 Point.SetCell (0, 0, V2[F2.vertexes[i]].x_abs) ;
                 Point.SetCell (1, 0, V2[F2.vertexes[i]].y_abs) ;
                 Point.SetCell (2, 0, V2[F2.vertexes[i]].z_abs) ;
                 Point.SetCell (3, 0,  1) ;
                 Point.LoadMul (&SummaryA, &Point) ;
               y=Point.GetCell (1, 0) ;

            if(y>0) {
                       if(cross<0 ) {  ma=i ;  break ;  }
                       else            cross=1 ;
                    }
            if(y<0) {
                       if(cross>0 ) {  ma=i ;  break ;  }
                       else          cross=-1 ;
                    }
                                            }

              if(i>=F2.vertexes_cnt)  continue ;
/*- - - - - - - - - - - - - - �������� ����������� ��������� �-����� */
                   SummaryP.LoadZero(4,4) ;

              iToFlat(&V2[F2.vertexes[0]],
                      &V2[F2.vertexes[1]],
                      &V2[F2.vertexes[2]], &SummaryP) ;

                          cross=0 ;
                              m=0 ;

         for(i=0 ; i<F1.vertexes_cnt ; i++) {

                 Point.LoadZero(4, 1) ;
                 Point.SetCell (0, 0, V1[F1.vertexes[i]].x_abs) ;
                 Point.SetCell (1, 0, V1[F1.vertexes[i]].y_abs) ;
                 Point.SetCell (2, 0, V1[F1.vertexes[i]].z_abs) ;
                 Point.SetCell (3, 0,  1) ;
                 Point.LoadMul (&SummaryP, &Point) ;
               y=Point.GetCell (1, 0) ;

            if(y>0) {
                       if(cross<0 ) {
                                       m1[m]=i-1 ;
                                       m2[m]=i ;
                                          m++ ;
                                       if(m>1)  break ;
                                    }
                          cross=1 ;
                    }
            if(y<0) {
                       if(cross>0 ) {
                                       m1[m]=i-1 ;
                                       m2[m]=i ;
                                          m++ ;
                                       if(m>1)  break ;
                                    }
                          cross=-1 ;
                    }
                                            }

              if(m==0)  continue ;

              if(m==1) {
                          m1[1]= 0 ;
                          m2[1]=F1.vertexes_cnt-1 ;
                       }
/*- - - - - - - - - - - ����� ����������� ����� � ���������� �-����� */
         for(i=0 ; i<2 ; i++) {
                                         m=F1.vertexes[m1[i]] ;

                 Point.LoadZero(4, 1) ;
                 Point.SetCell (0, 0, V1[m].x_abs) ;
                 Point.SetCell (1, 0, V1[m].y_abs) ;
                 Point.SetCell (2, 0, V1[m].z_abs) ;
                 Point.SetCell (3, 0,  1) ;
                 Point.LoadMul (&SummaryP, &Point) ;
              x1=Point.GetCell (0, 0) ;
              y1=Point.GetCell (1, 0) ;
              z1=Point.GetCell (2, 0) ;

                                         m=F1.vertexes[m2[i]] ;

                 Point.LoadZero(4, 1) ;
                 Point.SetCell (0, 0, V1[m].x_abs) ;
                 Point.SetCell (1, 0, V1[m].y_abs) ;
                 Point.SetCell (2, 0, V1[m].z_abs) ;
                 Point.SetCell (3, 0,  1) ;
                 Point.LoadMul (&SummaryP, &Point) ;
              x2=Point.GetCell (0, 0) ;
              y2=Point.GetCell (1, 0) ;
              z2=Point.GetCell (2, 0) ;

           if(y1!=y2) {
                         c_x[i]=x1-y1*(x2-x1)/(y2-y1) ;
                         c_z[i]=z1-y1*(z2-z1)/(y2-y1) ;
                      }
           else       {
                         c_x[i]=x2 ;
                         c_z[i]=z2 ;
                      }
                              }
/*- - - �������� ���������� ����� ����������� ������ ������� �-����� */
/* ���������, ��� ������� ������� �������� ����� ����� ������������  */
/*  ���� �� ����� �� ����� ����������� ����� ���� � ��� �� ����      */
                 cross1=1 ;
                 cross2=1 ;
                     v1=0 ;
                     v2=0 ;

         for(i=0 ; i<=F2.vertexes_cnt ; i++) {

           if(i<F2.vertexes_cnt) {

                   Point.LoadZero(4, 1) ;
                   Point.SetCell (0, 0, V2[F2.vertexes[i]].x_abs) ;
                   Point.SetCell (1, 0, V2[F2.vertexes[i]].y_abs) ;
                   Point.SetCell (2, 0, V2[F2.vertexes[i]].z_abs) ;
                   Point.SetCell (3, 0,  1) ;
                   Point.LoadMul (&SummaryP, &Point) ;

                 x1=x2 ;
                 z1=z2 ;
                 x2=Point.GetCell (0, 0) ;
                 y =Point.GetCell (1, 0) ;
                 z2=Point.GetCell (2, 0) ;
                                 }
           else                  {
                 x1=x2 ;
                 z1=z2 ;
                 x2=x0 ;
                 z2=z0 ;
                                 }

           if(i==0) {   x0=x2 ;
                        z0=z2 ;
                       continue ;  }

              y1=(z1-c_z[0])*(x2-x1)-(x1-c_x[0])*(z2-z1) ;
              y2=(z1-c_z[1])*(x2-x1)-(x1-c_x[1])*(z2-z1) ;

           if(v1*y1<0)  cross1=0 ;
           if(v2*y2<0)  cross2=0 ;

              v1=y1 ; 
              v2=y2 ; 

           if(cross1+cross2==0)  break ;         
                                            }

           if(cross1+cross2>0)  return(1) ;        
/*- - - - - - - - - - - ����� ����������� ����� � ���������� �-����� */
                                         m=F2.vertexes[ma-1] ;

                 Point.LoadZero(4, 1) ;
                 Point.SetCell (0, 0, V2[m].x_abs) ;
                 Point.SetCell (1, 0, V2[m].y_abs) ;
                 Point.SetCell (2, 0, V2[m].z_abs) ;
                 Point.SetCell (3, 0,  1) ;
                 Point.LoadMul (&SummaryA, &Point) ;
              x1=Point.GetCell (0, 0) ;
              y1=Point.GetCell (1, 0) ;
              z1=Point.GetCell (2, 0) ;

                                         m=F2.vertexes[ma] ;

                 Point.LoadZero(4, 1) ;
                 Point.SetCell (0, 0, V2[m].x_abs) ;
                 Point.SetCell (1, 0, V2[m].y_abs) ;
                 Point.SetCell (2, 0, V2[m].z_abs) ;
                 Point.SetCell (3, 0,  1) ;
                 Point.LoadMul (&SummaryA, &Point) ;
              x2=Point.GetCell (0, 0) ;
              y2=Point.GetCell (1, 0) ;
              z2=Point.GetCell (2, 0) ;

           if(y1!=y2) {
                         c_x[0]=x1-y1*(x2-x1)/(y2-y1) ;
                         c_z[0]=z1-y1*(z2-z1)/(y2-y1) ;
                      }
           else       {
                         c_x[0]=x2 ;
                         c_z[0]=z2 ;
                      }
/*- - - �������� ���������� ����� ����������� ������ ������� �-����� */
/* ���������, ��� ������� ������� �������� ����� ����� ������������  */
/*  ���� �� ����� �� ����� ����������� ����� ���� � ��� �� ����      */
                 cross1=1 ;
                     v1=0 ;

         for(i=0 ; i<=F1.vertexes_cnt ; i++) {

           if(i<F1.vertexes_cnt) {

                   Point.LoadZero(4, 1) ;
                   Point.SetCell (0, 0, V1[F1.vertexes[i]].x_abs) ;
                   Point.SetCell (1, 0, V1[F1.vertexes[i]].y_abs) ;
                   Point.SetCell (2, 0, V1[F1.vertexes[i]].z_abs) ;
                   Point.SetCell (3, 0,  1) ;
                   Point.LoadMul (&SummaryA, &Point) ;

                 x1=x2 ;
                 z1=z2 ;
                 x2=Point.GetCell (0, 0) ;
                 y =Point.GetCell (1, 0) ;
                 z2=Point.GetCell (2, 0) ;
                                 }
           else                  {
                 x1=x2 ;
                 z1=z2 ;
                 x2=x0 ;
                 z2=z0 ;
                                 }

           if(i==0) {   x0=x2 ;
                        z0=z2 ;
                       continue ;  }

              y1=(z1-c_z[0])*(x2-x1)-(x1-c_x[0])*(z2-z1) ;

           if(v1*y1<0) {  cross1=0 ;  break ;  }

              v1=y1 ; 
                                            }

           if(cross1>0)  return(1) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                           }
/*---------------------------------------------- ������� ������ ���� */
                                        }
/*-------------------------------------------------------------------*/

#undef  F1
#undef  F2
#undef  V1
#undef  V2

#endif

  return(0) ;

}


/********************************************************************/
/*								    */
/*             ������ ������� ��������� ��������� x0z               */ 
/*                   � ��������� ����� P0-P1-P2                     */

    int RSS_Feature_Hit::iToFlat(RSS_Feature_Hit_Vertex *p0,
                                 RSS_Feature_Hit_Vertex *p1,
                                 RSS_Feature_Hit_Vertex *p2,
                                               Matrix2d *Transform)

{
    Matrix2d  Summary ;
    Matrix2d  Local ;
    Matrix2d  Point ;
      double  x1, y1, z1 ;
      double  x2, y2, z2 ;
      double  v ;
      double  angle ;

/*------------------------------------------------- ����� � ����� P0 */

                Summary.LoadE      (4, 4) ;
                                                                    /* ����� 0 ����� �� ������� */ 
                  Local.Load4d_base(-p0->x_abs,
                                    -p0->y_abs,
                                    -p0->z_abs ) ;
                Summary.LoadMul    (&Local, &Summary) ;

                      x1=p1->x_abs-p0->x_abs ;
                      x2=p2->x_abs-p0->x_abs ;
                      y1=p1->y_abs-p0->y_abs ;
                      y2=p2->y_abs-p0->y_abs ;
                      z1=p1->z_abs-p0->z_abs ;
                      z2=p2->z_abs-p0->z_abs ;

   if(fabs(z1)<fabs(z2)) { 
                            v=x1 ;  x1=x2 ; x2=v ;
                            v=y1 ;  y1=y2 ; y2=v ;
                            v=z1 ;  z1=z2 ; z2=v ;
                         }
/*----------------------------------------- ������� ������ Y � ��� Z */

   if(z1!=0.) {
                          v=z1/sqrt(x1*x1+z1*z1) ;
                      angle=asin(v)*_RAD_TO_GRD ;

                  Local.Load4d_azim(-angle) ;
                Summary.LoadMul    (&Local, &Summary) ;

                      Point.LoadZero(4, 1) ;                        /* ���������������� ���������� ����� 1 */
                      Point.SetCell (0, 0, x1) ;
                      Point.SetCell (1, 0, y1) ;
                      Point.SetCell (2, 0, z1) ;
                      Point.SetCell (3, 0,  1) ;
                      Point.LoadMul (&Local, &Point) ;
                   x1=Point.GetCell (0, 0) ;
                   y1=Point.GetCell (1, 0) ;
                   z1=Point.GetCell (2, 0) ;

                      Point.LoadZero(4, 1) ;                        /* ���������������� ���������� ����� 2 */
                      Point.SetCell (0, 0, x2) ;
                      Point.SetCell (1, 0, y2) ;
                      Point.SetCell (2, 0, z2) ;
                      Point.SetCell (3, 0,  1) ;
                      Point.LoadMul (&Local, &Point) ;
                   x2=Point.GetCell (0, 0) ;
                   y2=Point.GetCell (1, 0) ;
                   z2=Point.GetCell (2, 0) ;
              }
/*----------------------------------------- ������� ������ Z � ��� X */

   if(y1!=0.) {
                          v=y1/sqrt(x1*x1+y1*y1) ;
                      angle=asin(v)*_RAD_TO_GRD ;

                  Local.Load4d_roll(-angle) ;
                Summary.LoadMul    (&Local, &Summary) ;

                      Point.LoadZero(4, 1) ;                        /* ���������������� ���������� ����� 2 */
                      Point.SetCell (0, 0, x2) ;
                      Point.SetCell (1, 0, y2) ;
                      Point.SetCell (2, 0, z2) ;
                      Point.SetCell (3, 0,  1) ;
                      Point.LoadMul (&Local, &Point) ;
                   x2=Point.GetCell (0, 0) ;
                   y2=Point.GetCell (1, 0) ;
                   z2=Point.GetCell (2, 0) ;
              }
/*----------------------------------------- ������� ������ X � ��� Y */

   if(y2!=0.) {
                          v=y2/sqrt(y2*y2+z2*z2) ;
                      angle=asin(v)*_RAD_TO_GRD ;

                  Local.Load4d_elev(-angle) ;
                Summary.LoadMul    (&Local, &Summary) ;
              }
/*--------------------------------------------------------- �������� */
/*
                 Point.LoadZero(4, 1) ;
                 Point.SetCell (0, 0, p0->x_abs) ;
                 Point.SetCell (1, 0, p0->y_abs) ;
                 Point.SetCell (2, 0, p0->z_abs) ;
                 Point.SetCell (3, 0,  1) ;
                 Point.LoadMul (&Summary, &Point) ;
              x1=Point.GetCell (0, 0) ;
              y1=Point.GetCell (1, 0) ;
              z1=Point.GetCell (2, 0) ;

                 Point.LoadZero(4, 1) ;
                 Point.SetCell (0, 0, p1->x_abs) ;
                 Point.SetCell (1, 0, p1->y_abs) ;
                 Point.SetCell (2, 0, p1->z_abs) ;
                 Point.SetCell (3, 0,  1) ;
                 Point.LoadMul (&Summary, &Point) ;
              x1=Point.GetCell (0, 0) ;
              y1=Point.GetCell (1, 0) ;
              z1=Point.GetCell (2, 0) ;

                 Point.LoadZero(4, 1) ;
                 Point.SetCell (0, 0, p2->x_abs) ;
                 Point.SetCell (1, 0, p2->y_abs) ;
                 Point.SetCell (2, 0, p2->z_abs) ;
                 Point.SetCell (3, 0,  1) ;
                 Point.LoadMul (&Summary, &Point) ;
              x1=Point.GetCell (0, 0) ;
              y1=Point.GetCell (1, 0) ;
              z1=Point.GetCell (2, 0) ;
*/
/*-------------------------------------------------------------------*/

          Transform->Copy(&Summary) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*                     �������� ����� ��� �������                   */  

    int RSS_Feature_Hit::RecalcPoints(void)

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
   if(i==0) {
                track_s.x   =B[i].x_base ;
                track_s.y   =B[i].y_base ;
                track_s.z   =B[i].z_base ;
                track_s.azim=B[i].a_azim ;
                track_s.elev=B[i].a_elev ;
                track_s.roll=B[i].a_roll ;
            }
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


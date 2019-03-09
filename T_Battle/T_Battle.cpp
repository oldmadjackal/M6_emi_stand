/********************************************************************/
/*								    */
/*		������ ������ "��������� ���"    		    */
/*								    */
/********************************************************************/

#pragma warning( disable : 4996 )
#define  stricmp  _stricmp

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
#include "..\F_Hit\F_Hit.h"

#include "T_Battle.h"


#define  _SECTION_FULL_NAME   "BATTLE"
#define  _SECTION_SHRT_NAME   "BAT"

#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text)

#define  CREATE_DIALOG  CreateDialogIndirectParam


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

     static   RSS_Module_Battle  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	����������������� ����                      */

 T_BATTLE_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 T_BATTLE_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/********************************************************************/
/**							           **/
/**	       �������� ������ ������ "����� � �����"              **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*                            ������ ������                         */

  struct RSS_Module_Battle_instr  RSS_Module_Battle_InstrList[]={

 { "help",       "?",  "#HELP (?) - ������ ��������� ������", 
                        NULL,
                       &RSS_Module_Battle::cHelp   },
 { "clear",      "c",  "#CLEAR (C) - �������� �������� �������� ��� � ������������ �������� ���������", 
                        NULL,
                       &RSS_Module_Battle::cClear  },
 { "add",        "a",  "#ADD (A) - ���������� ������� � �������� ���", 
                       " ADD <��� �������>\n",
                       &RSS_Module_Battle::cAdd    },
 { "list",       "l",  "#LIST (L) - �������� �������� �������� � ������� ���������� ���", 
                        NULL,
                       &RSS_Module_Battle::cList   },
 { "program",    "p",  "#PROGRAM (P) - �������� �������� �� �����", 
                       " PROGRAM <��� �����>\n",
                       &RSS_Module_Battle::cProgram },
 { "run",        "r",  "#RUN (R) - ������ ���������� �������� ���", 
                       " RUN [<����� ��������>]\n"
                       "   ���������� �������� � �������� �������\n"
                       " RUN/F [<����� ��������>]\n"
                       "   ���������� �������� � ������������ ���������� �������\n",
                       &RSS_Module_Battle::cRun },
 { "map",        "m",  "#MAP (M) - ����������� ����� ���", 
                       " MAP [<�������� ����������>]\n"
                       "   ���������� ���� ����� ���� ���\n"
                       " MAP/C[Q] <������>[&] <�������� �����>\n"
                       "   ���������� ���� ������� (�������� � �������� �������� �����) �� �����: RED, GREEN, BLUE\n"
                       "   K��� Q ��������� ������ ��������������� ���������\n"
                       " MAP/T[Q] <������>\n"
                       "   ������������ ������\n"
                       "   K��� Q ��������� ������ ��������������� ���������\n",
                       &RSS_Module_Battle::cMap },
 {  NULL }
                                                              } ;


/********************************************************************/
/*								    */
/*		     ����� ����� ������             		    */

    struct RSS_Module_Battle_instr *RSS_Module_Battle::mInstrList       =NULL ;

                               OBJ  RSS_Module_Battle::mObjects[_OBJECTS_MAX] ;
                               int  RSS_Module_Battle::mObjects_cnt     =  0 ;

                             FRAME *RSS_Module_Battle::mSpawns[_SPAWNS_MAX] ;
                               int  RSS_Module_Battle::mSpawns_cnt      =  0 ;

                             FRAME *RSS_Module_Battle::mScenario        =NULL ;
                               int  RSS_Module_Battle::mScenario_cnt    =  0 ;

                               VAR *RSS_Module_Battle::mVariables       =NULL ;
                               int  RSS_Module_Battle::mVariables_cnt   =  0 ;
                              HWND  RSS_Module_Battle::mVariables_Window=NULL ;

                               int  RSS_Module_Battle::mHit_cnt ;

                              HWND  RSS_Module_Battle::mMapWindow ;
                               int  RSS_Module_Battle::mMapRegime ;
                            double  RSS_Module_Battle::mMapXmin ;
                            double  RSS_Module_Battle::mMapXmax ;
                            double  RSS_Module_Battle::mMapZmin ;
                            double  RSS_Module_Battle::mMapZmax ;
                             COLOR  RSS_Module_Battle::mMapColors[_OBJECTS_MAX] ;
                               int  RSS_Module_Battle::mMapColors_cnt ;
                             TRACE  RSS_Module_Battle::mMapTraces[_OBJECTS_MAX] ;
                               int  RSS_Module_Battle::mMapTraces_cnt ;


/********************************************************************/
/*								    */
/*		       ����������� ������			    */

     RSS_Module_Battle::RSS_Module_Battle(void)

{
  static  WNDCLASS  Map_wnd ;

/*---------------------------------------------------- ������������� */

                keyword="EmiStand" ;
         identification="Battle" ;
               category="Task" ;

             mInstrList=RSS_Module_Battle_InstrList ;

                Context=new RSS_Transit_Battle ;

/*----------------------------------- ����������� ������ ������� Map */

          mMapRegime=_MAP_KEEP_RANGE ;

	Map_wnd.lpszClassName="Task_Battle_Map_class" ;
	Map_wnd.hInstance    = GetModuleHandle(NULL) ;
	Map_wnd.lpfnWndProc  = Task_Battle_Map_prc ;
	Map_wnd.hCursor      = LoadCursor(NULL, IDC_ARROW) ;
	Map_wnd.hIcon        =  NULL ;
	Map_wnd.lpszMenuName =  NULL ;
	Map_wnd.hbrBackground=  NULL ;
	Map_wnd.style        =    0 ;
	Map_wnd.hIcon        =  NULL ;

            RegisterClass(&Map_wnd) ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        ���������� ������			    */

    RSS_Module_Battle::~RSS_Module_Battle(void)

{
}


/********************************************************************/
/*								    */
/*		        �������� ��������       		    */

     int  RSS_Module_Battle::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- �������� ������ */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s -  ��������� ���", identification) ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		        ��������� �������       		    */

  int  RSS_Module_Battle::vExecuteCmd(const char *cmd)

{
   return( vExecuteCmd(cmd, NULL) ) ;
}


  int  RSS_Module_Battle::vExecuteCmd(const char *cmd, RSS_IFace *iface)

{
  static  int  direct_command ;   /* ���� ������ ������ ������� */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

/*--------------------------------------------- ������������� ������ */

             memset(command, 0, sizeof(command)) ;
            strncpy(command, cmd, sizeof(command)-1) ;
/*- - - - - - - - - - - - - - - - - - - -  ��������� �������� ������ */
        if(command[0]=='&') {
                                end=command+1 ;
                            }
/*- - - - - - - - - - - - - - - - - - - - -  ��������� ������ ������ */
   else if(!direct_command) {

         end=strchr(command, ' ') ;
      if(end!=NULL) {  *end=0 ;  end++ ;  }

      if(stricmp(command, _SECTION_FULL_NAME) &&
         stricmp(command, _SECTION_SHRT_NAME)   )  return(1) ;
                            }
/*- - - - - - - - - - - - - - - - - - - -  ��������� �������� ������ */
   else                     {
                                end=command ;
                            }
/*----------------------- ���������/���������� ������ ������ ������� */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - -  ��������� */
   if(end==NULL || end[0]==0) {
     if(!direct_command) {
                            direct_command=1 ;

        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Task Battle:") ;
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
                     *end!='.' &&
                     *end!='/'    ; end++) ;

      if(*end!= 0 &&
         *end!=' '  )  memmove(end+1, end, strlen(end)+1) ;

      if(*end!=0) {  *end=0 ;  end++ ;  }
      else            end="" ;

   for(i=0 ; mInstrList[i].name_full!=NULL ; i++)                   /* ���� ������� � ������ */
     if(!stricmp(instr, mInstrList[i].name_full) ||
        !stricmp(instr, mInstrList[i].name_shrt)   )   break ;

     if(mInstrList[i].name_full==NULL) {                            /* ���� ����� ������� ���... */

          status=this->kernel->vExecuteCmd(cmd, iface) ;            /*  �������� �������� ������ ����... */
       if(status)  SEND_ERROR("������ BATTLE: ����������� �������") ;
                                            return(-1) ;
                                       }
 
     if(mInstrList[i].process!=NULL)                                /* ���������� ������� */
                status=(this->*mInstrList[i].process)(end, iface) ;
     else       status=  0 ;

/*-------------------------------------------------------------------*/

   return(status) ;
}


/********************************************************************/
/*								    */
/*		        ������� ������ �� ������		    */

    void  RSS_Module_Battle::vReadSave(std::string *data)

{

/*----------------------------------------------- �������� ��������� */

   if(memicmp(data->c_str(), "#BEGIN MODULE BATTLE\n", 
                      strlen("#BEGIN MODULE BATTLE\n")) )  return ;

/*------------------------------------------------ ���������� ������ */

/*-------------------------------------------------------------------*/

}


/********************************************************************/
/*								    */
/*		        �������� ������ � ������		    */

    void  RSS_Module_Battle::vWriteSave(std::string *text)

{

/*----------------------------------------------- ��������� �������� */

     *text="#BEGIN MODULE BATTLE\n" ;

/*-------------------------------------------------- ������ �������� */

/*------------------------------------------------ �������� �������� */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*             ��������� �������� � ��������� ������                */

    void  RSS_Module_Battle::vChangeContext(void)

{
/*------------------------------------------ ����� ������ ���������� */

   if(!stricmp(mContextAction, "VARS")) {

    this->mVariables_Window=
                     CREATE_DIALOG(GetModuleHandle(NULL),
                                    (LPCDLGTEMPLATE)Resource("IDD_VARIABLES", RT_DIALOG),
           	                      NULL, Task_Battle_Vars_dialog, 
                                       (LPARAM)this ) ;
                        ShowWindow(this->mVariables_Window, SW_SHOW) ;
                                        }
/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� HELP                    */

  int  RSS_Module_Battle::cHelp(char *cmd, RSS_IFace *iface)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Task_Battle_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� ADD                     */
/*								    */
/*       ADD <��� �������>                                          */

  int  RSS_Module_Battle::cAdd(char *cmd, RSS_IFace *iface)

{
#define   _PARS_MAX  10

               char  *pars[_PARS_MAX] ;
               char  *name ;
               char  text[1024] ;
               char  *end ;
                int   i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

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

/*------------------------------------------- �������� ����� ������� */

    if(name==NULL) {                                                /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� �������. \n"
                                 "��������: ADD <���_�������>") ;
                                     return(-1) ;
                   }

       for(i=0 ; i<OBJECTS_CNT ; i++)                               /* ���� ������ �� ����� */
         if(!stricmp(OBJECTS[i]->Name, name))  break ;

    if(i==OBJECTS_CNT) {                                            /* ���� ��� �� �������... */
                           sprintf(text, "������� � ������ '%s' "
                                         "�� ����������", name) ;
                        SEND_ERROR(text) ;
                            return(-1) ;
                       }
/*-------------------------------------------------- ������� ������� */

    if(mObjects_cnt>=_OBJECTS_MAX) {
                      SEND_ERROR("������ �������� �������� ������� �����") ;
                                     return(-1) ;
                                   }

         if(OBJECTS[i]->battle_state!=_SPAWN_STATE) {               /* ���� ������ �� ������ � �������� ���... */
                   OBJECTS[i]->battle_state=_ACTIVE_STATE ;
                   OBJECTS[i]->vPush() ;
                                                    }

               mObjects[mObjects_cnt].object=OBJECTS[i] ;
                        mObjects_cnt++ ;

/*-------------------------------------------------------------------*/

#undef    OBJECTS
#undef    OBJECTS_CNT

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� CLEAR                   */
/*								    */
/*       CLEAR                                                      */

  int  RSS_Module_Battle::cClear(char *cmd, RSS_IFace *iface)

{
  int  i ;
  int  j ;

/*--------------------------------- ������������� ��������� �������� */

#define   OBJECTS       RSS_Kernel::kernel->kernel_objects
#define   OBJECTS_CNT   RSS_Kernel::kernel->kernel_objects_cnt

       for(i=0 ; i<OBJECTS_CNT ; i++) {

         if(OBJECTS[i]->battle_state==_ACTIVE_STATE) {

                                OBJECTS[i]->vPop() ;
                                OBJECTS[i]->vCalculateShow(0, 0) ;
                                                     }
         if(OBJECTS[i]->battle_state== _SPAWN_STATE) {

                                         OBJECTS[i]->vFree() ;      /* ������������ �������� */
                                 delete  OBJECTS[i] ;

             for(j=i+1 ; j<OBJECTS_CNT ; j++)  OBJECTS[j-1]=OBJECTS[j] ;
                           OBJECTS_CNT-- ;
                                     i-- ;
                                                     }
                                      }

#undef    OBJECTS
#undef    OBJECTS_CNT

/*------------------------------------------------ ������� ��������� */

           mObjects_cnt=0 ;
            mSpawns_cnt=0 ;

/*------------------------------------- ������������� �������� ����� */

     for(i=0 ; i<mMapTraces_cnt ; i++) {

       if(mMapTraces[i].points_max!=0)  free(mMapTraces[i].points) ;
                                             mMapTraces[i].points    =NULL ;
                                             mMapTraces[i].points_max= 0 ;
                                             mMapTraces[i].points_cnt= 0 ;
                                       }

                 mMapTraces_cnt=0 ;

       if(this->mMapWindow!=NULL)  SendMessage(this->mMapWindow, WM_PAINT, NULL, NULL) ;

/*------------------------------------------------------ ����������� */

                       this->kernel->vShow(NULL) ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� LIST                    */

  int  RSS_Module_Battle::cList(char *cmd, RSS_IFace *iface)

{ 
    DialogBoxIndirectParam(GetModuleHandle(NULL),
			    (LPCDLGTEMPLATE)Resource("IDD_BATTLE", RT_DIALOG),
			       GetActiveWindow(), Task_Battle_View_dialog, (LPARAM)this) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� PROGRAM                 */
/*								    */
/*  ��������� ����� ��������:                                       */
/*								    */
/*    {T|DT}=<����� �������> EXIT                                   */
/*    {T|DT}=<����� �������> START <��� �������> [<����� ��������>] */
/*    {T|DT}=<����� �������> STOP  <��� �������>                    */
/*    {T|DT}=<����� �������> KILL  <��� �������>                    */
/*    {T|DT}=<����� �������> EVENT <��� �������> <�������>          */
/*    {T|DT}=<����� �������> SPAWN <��� �������> <�����> <����>     */
/*    {T|DT}=<����� �������> EXEC  <��������� �������>              */

  int  RSS_Module_Battle::cProgram(char *cmd, RSS_IFace *iface)

{ 
#define   _PARS_MAX   2

  RSS_IFace  iface_ ;
       char *pars[_PARS_MAX] ;
       char  path[FILENAME_MAX] ;
       FILE *file ;
       char *end ;
        int  cnt ;
       char  text[1024] ;
       char *work ;
       char  desc[1024] ;
       char *words[30] ;
     double  value ;
        int  row ;
       char  error[1024] ;
        int  n ;
        int  i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*------------------------------------------------- ������� �������� */

      if(iface==NULL)  iface=&iface_ ;                              /* ��� ���������� ������������� ���������� */
                                                                    /*     ���������� ��������� ��������       */ 
/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������ ���������� */        
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

    for(i=0 ; i<_PARS_MAX ; i++)  
      if( pars[i]    !=NULL && 
         (pars[i])[0]==  0    )  pars[i]=NULL ;

/*--------------------------------------------- �������� ����� ����� */

    if(pars[0]==NULL) {                                             /* ���� ��� ����� �� ������... */
                       SEND_ERROR("�� ������ ��� ����� ��������.\n"
                                  "��������: PROGRAM <���� ��������>") ;
                                     return(-1) ;
                      }

                        strcpy(path, pars[0]) ;
                   end=strrchr(path, '.') ;                         /* ���� ��� �� �������� ���������� - */
         if(       end       ==NULL ||                              /*   - ��������� �������             */
            strchr(end, '/' )!=NULL ||
            strchr(end, '\\')!=NULL   )  strcat(path, ".battle") ;

/*--------------------------------------------------- �������� ����� */

        file=fopen(path, "rt") ;                                    /* ��������� ����� */
     if(file==NULL) {
                           sprintf(text, "������ �������� �����: %s", path) ;
                        SEND_ERROR(text) ;
                          return(-1) ;
                    }
/*------------------------------------------ ����������� ����� ����� */

          for(cnt=0 ; ; cnt++) {
                  end=fgets(text, sizeof(text)-1, file) ;           /* ��������� ��������� ������ */
               if(end==NULL)  break ;                               /* ���� ��� �������... */
                               }
/*---------------------------------------- ���������� ������� ������ */

        this->mScenario=(FRAME *)
                          this->gMemRealloc(this->mScenario, cnt*sizeof(FRAME), 
                                             "RSS_Module_Battle::mScenario", 0, 0) ;
     if(this->mScenario==NULL) {
                   sprintf(text, "������������ ������") ;
                SEND_ERROR(text) ;
                 return(-1) ;
                               }
/*---------------------------------------------- ���������� �������� */

                     rewind(file) ;                                 /* ������������ ���� �� ������ */

       for(row=1, n=0  ; ; row++) {                                 /* CIRCLE.1 */
/*- - - - - - - - - - - - - - - - - - - - - - - -  ���������� ������ */
                     memset(text, 0, sizeof(text)) ;
                  end=fgets(text, sizeof(text)-1, file) ;           /* ��������� ��������� ������ */
               if(end==NULL)  break ;                               /* ���� ��� �������... */

                  end=strchr(text, '\n') ;                          /* ������� ����-������� */
               if(end!=NULL)  *end=0 ;
                  end=strchr(text, '\r') ;
               if(end!=NULL)  *end=0 ;

              if(text[0]==';')  continue ;
          
           for(work=text ; *work==' ' || *work=='\t' ; work++) ;

              if(work[0]== 0 )  continue ;
/*- - - - - - - - - - - - - - - - - - - - - - - - -  ������ �� ����� */
              memset(words, 0, sizeof(words)) ;

                      i = 0 ;
                words[i]=strtok(work, " \t") ;

          while(words[i]!=NULL && i<30) {
                      i++ ;
                words[i]=strtok(NULL, " \t") ;
                                        }
/*- - - - - - - - - - - - - - - - - - - - -  ��������� ����� ������� */
            memset(&this->mScenario[n], 0,                          /* ������������� ���������� ����� */
                              sizeof(this->mScenario[n])) ;

         if(memicmp(words[0], "T=",  2) &&
            memicmp(words[0], "DT=", 3)   ) {
                 sprintf(error, "������ ��������� �������� ������ ���������� � ����� ������� T=... ��� DT=... (������ %d)", row) ;
              SEND_ERROR(error) ;
                   return(-1) ;
                                            }

                          end=strchr(words[0], '=') ;
             value=strtod(end+1, &end) ;

         if(*end!=0) {
                          sprintf(error, "������������ ����� ������� (������ %d)", row) ;
                       SEND_ERROR(error) ;
                            return(-1) ;
                     }

         if(!memicmp(words[0], "T=", 2)) {  this->mScenario[n].t        = value ;
                                            this->mScenario[n].sync_type=_T_SYNC ;   }
         else                            {  this->mScenario[n].dt       = value ;
                                            this->mScenario[n].sync_type=_DT_SYNC ;  }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - �������� START */
         if(!stricmp(words[1], "START")) {

              if(words[2]==NULL) {
                                    sprintf(error, "�� ������ ��� ������� (������ %d)", row) ;
                                 SEND_ERROR(error) ;
                                      return(-1) ;
                                 }

            for(i=0 ; i<OBJECTS_CNT ; i++)                               /* ���� ������ �� ����� */
              if(!stricmp(OBJECTS[i]->Name, words[2]))  break ;

              if(i==OBJECTS_CNT) {                                       /* ���� ��� �� �������... */
                           sprintf(error, "������� � ������ '%s' �� ����������  (������ %d)", words[2], row) ;
                        SEND_ERROR(error) ;
                            return(-1) ;
                                 }

                 strcpy(desc, words[2]) ;

              if(words[3]!=NULL) {
                        this->mScenario[n].t_par=strtod(words[3], &end) ;
                 if(*end!=0) {
                                  sprintf(error, "������������ ����� ���������� ������� (������ %d)", row) ;
                               SEND_ERROR(error) ;
                                    return(-1) ;
                             }

                                     strcat(desc, " ") ;
                                     strcat(desc, words[3]) ;
                                 }

                         strcpy(this->mScenario[n].action,  words[1]) ;
                         strcpy(this->mScenario[n].object,  words[2]) ;
                         strcpy(this->mScenario[n].command, desc) ;
                                                n++ ;           
                                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - -  �������� STOP */
         else
         if(!stricmp(words[1], "STOP")) {

                                                strcpy(desc, words[2]) ;
             for(i=3 ; words[i]!=NULL ; i++) {  strcat(desc, " ") ;
                                                strcat(desc, words[i]) ;  }

                         strcpy(this->mScenario[n].action, words[1]) ;
                         strcpy(this->mScenario[n].object, desc) ;
                                                n++ ;
                                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - -  �������� KILL */
         else
         if(!stricmp(words[1], "KILL")) {

                                                strcpy(desc, words[2]) ;
             for(i=3 ; words[i]!=NULL ; i++) {  strcat(desc, " ") ;
                                                strcat(desc, words[i]) ;  }

                         strcpy(this->mScenario[n].action, words[1]) ;
                         strcpy(this->mScenario[n].object, desc) ;
                                                n++ ;
                                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - �������� EVENT */
         else
         if(!stricmp(words[1], "EVENT")) {

              if(words[2]==NULL) {
                                    sprintf(error, "�� ������ ��� ������� (������ %d)", row) ;
                                 SEND_ERROR(error) ;
                                      return(-1) ;
                                 }

              if(words[3]==NULL) {
                                    sprintf(error, "�� ������ ������� (������ %d)", row) ;
                                 SEND_ERROR(error) ;
                                      return(-1) ;
                                 }

            for(i=0 ; i<OBJECTS_CNT ; i++)                               /* ���� ������ �� ����� */
              if(!stricmp(OBJECTS[i]->Name, words[2]))  break ;

              if(i==OBJECTS_CNT) {                                       /* ���� ��� �� �������... */
                           sprintf(error, "������� � ������ '%s' �� ����������  (������ %d)", words[2], row) ;
                        SEND_ERROR(error) ;
                            return(-1) ;
                                 }

                     strcpy(desc, words[2]) ;
                     strcat(desc, " ") ;
                     strcat(desc, words[3]) ;

                     strcpy(this->mScenario[n].action,  words[1]) ;
                     strcpy(this->mScenario[n].object,  words[2]) ;
                     strcpy(this->mScenario[n].event,   words[3]) ;
                     strcpy(this->mScenario[n].command, desc) ;
                                            n++ ;           
                                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - �������� SPAWN */
         else
         if(!stricmp(words[1], "SPAWN")) {

              if(words[2]==NULL) {
                                    sprintf(error, "�� ������ ��� �������-������� (������ %d)", row) ;
                                 SEND_ERROR(error) ;
                                      return(-1) ;
                                 }

              if(words[3]==NULL) {
                                    sprintf(error, "�� ������ ����������� ����� (������ %d)", row) ;
                                 SEND_ERROR(error) ;
                                      return(-1) ;
                                 }

              if(words[4]==NULL) {
                                    sprintf(error, "�� ����� ���� ���������� (������ %d)", row) ;
                                 SEND_ERROR(error) ;
                                      return(-1) ;
                                 }

            for(i=0 ; i<OBJECTS_CNT ; i++)                               /* ���� ������ �� ����� */
              if(!stricmp(OBJECTS[i]->Name, words[2]))  break ;

              if(i==OBJECTS_CNT) {                                       /* ���� ��� �� �������... */
                           sprintf(error, "������� � ������ '%s' �� ����������  (������ %d)", words[2], row) ;
                        SEND_ERROR(error) ;
                            return(-1) ;
                                 }

                     strcpy(desc, words[2]) ;
                     strcat(desc, " ") ;
                     strcat(desc, words[3]) ;
                     strcat(desc, " ") ;
                     strcat(desc, words[4]) ;

                     strcpy(this->mScenario[n].action,  words[1]) ;
                     strcpy(this->mScenario[n].object,  words[2]) ;
                            this->mScenario[n].size  =strtoul(words[3], &end, 10) ;
                            this->mScenario[n].period= strtod(words[4], &end) ;
                     strcpy(this->mScenario[n].command, desc) ;
                                            n++ ;           

                                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - -  �������� EXIT */
         else
         if(!stricmp(words[1], "EXEC")) {

                                                strcpy(desc, words[2]) ;
             for(i=3 ; words[i]!=NULL ; i++) {  strcat(desc, " ") ;
                                                strcat(desc, words[i]) ;  }

                         strcpy(this->mScenario[n].action,  words[1]) ;
                         strcpy(this->mScenario[n].command, desc) ;
                                                n++ ;
                                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - -  �������� EXIT */
         else
         if(!stricmp(words[1], "EXIT")) {

                         strcpy(this->mScenario[n].action,  words[1]) ;
                                                n++ ;           
                                         }
/*- - - - - - - - - - - - - - - - - - - - - - - ����������� �������� */
         else                            {

                          sprintf(error, "����������� �������� (������ %d)", row) ;
                       SEND_ERROR(error) ;
                            return(-1) ;
                                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                  }                                 /* CONTINUE.1 */

                  this->mScenario_cnt=n ;

/*--------------------------------------------------- �������� ����� */

             fclose(file) ;                                         /* ��������� ����� */

/*-------------------------------------------------------------------*/

#undef    OBJECTS
#undef    OBJECTS_CNT

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� RUN                     */

  int  RSS_Module_Battle::cRun(char *cmd, RSS_IFace *iface)

{ 
#define   _PARS_MAX   2

              RSS_IFace  iface_ ;
                   char *pars[_PARS_MAX] ;
                    int  fast ; 
                 double  time_0 ;              /* ��������� ����� ������� */ 
                 double  time_1 ;              /* ������� ����� */ 
                 double  time_c ;              /* ���������� ����� ������� */ 
                 double  time_s ;              /* ��������� ����� ��������� */ 
                 double  time_w ;              /* ����� �������� */ 
                    int  preObjects_cnt ;
                    int  attempt_cnt ;
                    int  attempt ;
                    int  hit_sum ;
                 double  hit_avg ;
                   char *end ;
                   char  name[1024] ;
                   char  text[1024] ;
       RSS_Objects_List  checked ;
             RSS_Object *clone ;
                  FRAME  frame ;
                    int  n_frame ;
                    int  exit_flag ;
                    int  status ;
                    int  i ;
                    int  j ;

           static  char *callback ;
#define  _CALLBACK_SIZE  128000

/*------------------------------------------------- ������� �������� */

      if(iface==NULL)  iface=&iface_ ;                              /* ��� ���������� ������������� ���������� */
                                                                    /*     ���������� ��������� ��������       */ 
/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
                   fast=0 ;

       if(*cmd=='/' ||
          *cmd=='+'   ) {
                              cmd++ ;
                          if(*cmd=='f' || *cmd=='F')  fast=1 ;
                              cmd++ ;
                        }       

       if(*cmd==' ')  cmd++ ; 
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������ ���������� */        
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

    for(i=0 ; i<_PARS_MAX ; i++)  
      if( pars[i]    !=NULL && 
         (pars[i])[0]==  0    )  pars[i]=NULL ;

                                attempt_cnt=0 ;
            if(pars[0]!=NULL )  attempt_cnt=strtoul(pars[0], &end, 10) ;

            if(attempt_cnt==0)  attempt_cnt=1 ;

/*-------------------------------------------- ���������� ���������� */

     if(callback==NULL)  callback=(char *)calloc(1, _CALLBACK_SIZE) ;

/*----------------------------------------------------- ���� ������� */
 
           preObjects_cnt=this->mObjects_cnt ;

                  hit_sum=0 ;

   for(attempt=0 ; attempt<attempt_cnt ; attempt++) {               /* LOOP.0 */

                       RSS_Kernel::battle=1 ;

/*------------------------------------- ������������� �������� ����� */

     for(i=0 ; i<mMapTraces_cnt ; i++) {
                                          mMapTraces[i].points_cnt=0 ;
                                       }
/*--------------------------------- ������������� ��������� �������� */

#define   OBJECTS       RSS_Kernel::kernel->kernel_objects
#define   OBJECTS_CNT   RSS_Kernel::kernel->kernel_objects_cnt


    for(i=0 ; i<OBJECTS_CNT ; i++) {

     if(attempt>0) {

         if(OBJECTS[i]->battle_state==_ACTIVE_STATE) {

                                OBJECTS[i]->vPop() ;
                                OBJECTS[i]->vCalculateShow(0, 0) ;
                                                     }
         if(OBJECTS[i]->battle_state== _SPAWN_STATE) {

                                         OBJECTS[i]->vFree() ;      /* ������������ �������� */
                                 delete  OBJECTS[i] ;

             for(j=i+1 ; j<OBJECTS_CNT ; j++)  OBJECTS[j-1]=OBJECTS[j] ;
                           OBJECTS_CNT-- ;
                                     i-- ;
                                                     }
                   }

           OBJECTS[i]->vResetFeatures(NULL) ;
           OBJECTS[i]->vPrepareFeatures(NULL) ;

                                   }

                       this->kernel->vShow("REFRESH") ;

#undef    OBJECTS
#undef    OBJECTS_CNT

/*----------------------------------------------- ���������� ������� */

                 this->mObjects_cnt  =preObjects_cnt ;
                 this->mSpawns_cnt   = 0 ;
                 this->mVariables_cnt= 0 ;

      for(i=0 ; i<mObjects_cnt ; i++) {

            mObjects[i].object->vSpecial       ("BATTLE", this) ;
            mObjects[i].object->vResetFeatures (NULL) ;             /* ������������� ������� ������� */ 
            mObjects[i].object->vCalculateStart(0.) ;
            mObjects[i].active  =1 ;
            mObjects[i].cut_time=0. ;
                                      }

                   exit_flag=0 ;

/*----------------------------------- ���������� ��������� ��������� */

      RSS_Feature_Hit::hit_count=&this->mHit_cnt ;

                                        mHit_cnt=0 ;

/*------------------------------------ ���������� ��������� �������� */

   for(n_frame=0 ; n_frame<mScenario_cnt ; ) {

        status=iFrameExecute(&mScenario[n_frame], 0, 1) ;
/*- - - - - - - - - - - - - - - - - - - ������� � ���������� "�����" */
     if(status==_NEXT_FRAME) {
                                n_frame++ ;  continue ;
                             }
/*- - - - - - - - - - - - - - - - - - - - - - -  ���������� �������� */
     if(status==_EXIT_FRAME) {
                                 exit_flag=1 ;  break ;
                             }
/*- - - - - - - - - - - - - - - - - - - -  ��� �� ������� ���������� */
     if(status==_WAIT_FRAME) {
                                  break ;
                             }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                             }

           if(exit_flag)  return(-1) ;  

/*-------------------------------------- ������� �������������� ���� */

                      RSS_Kernel::kernel->next=0 ;

              time_0=this->kernel->vGetTime() ;
              time_c=0. ;
              time_s=0. ;

  do {
           if(this->kernel->stop)  break ;                          /* ���� ������� ����������... */

           if(this->kernel->next==_RSS_KERNEL_WAIT_STEP) {          /* ���������� ��������� ������� */
                                     Sleep(100) ;
                                      continue ;
                                                         }
           if(this->kernel->next==_RSS_KERNEL_NEXT_STEP) {
                      this->kernel->next=_RSS_KERNEL_WAIT_STEP ;
                                                         }
/*------------------------------------------------ ��������� ������� */

              time_c+=RSS_Kernel::calc_time_step ;
              time_1=this->kernel->vGetTime() ;

              time_w=time_c-(time_1-time_0) ;

#pragma warning(disable : 4244)
          if(fast==0)                                               /* ��� ��������� ������� - ����������� ����� */
           if(time_w>=0)  Sleep(time_w*1000) ;
#pragma warning(default : 4244)

                    hit_avg=((double)(hit_sum+mHit_cnt))/(attempt+1.) ;

      sprintf(text, "Attempt % 5d\r\n" 
                    "   Real % 5.0lf\r\n" 
                    "   Calc % 5.0lf\r\n"
                    "Hit.cur % 5d\r\n"
                    "Hit.avg % 7.1lf\r\n",
                     attempt+1, time_1-time_0, time_c, mHit_cnt, hit_avg) ;

        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_SHOW_INFO, (LPARAM)text) ;

/*----------------------------------------------- ��������� �������� */

   for( ; n_frame<mScenario_cnt ; ) {                               /* CIRCLE.1 */

        status=iFrameExecute(&mScenario[n_frame], time_c, 1) ;
/*- - - - - - - - - - - - - - - - - - - ������� � ���������� "�����" */
     if(status==_NEXT_FRAME) {
                                n_frame++ ;  continue ;
                             }
/*- - - - - - - - - - - - - - - - - - - ������� � ���������� "�����" */
     if(status==_EXIT_FRAME) {
                                 exit_flag=1 ;  break ;
                             }
/*- - - - - - - - - - - - - - - - - - - -  ��� �� ������� ���������� */
     if(status==_WAIT_FRAME) {
                                  break ;
                             }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                    }                               /* CONTINUE.1 */

           if(exit_flag)  break ;  

/*-------------------------------------------------- ������ �������� */

      for(i=0 ; i<mObjects_cnt ; i++) 
        if(mObjects[i].active) {
/*- - - - - - - - - - - - - - - - - - - - ������ � ��������� ������� */
                    memset(callback, 0, sizeof(callback)) ;

            mObjects[i].object->vCalculate    (time_c-RSS_Kernel::calc_time_step, time_c,
                                                                   callback, _CALLBACK_SIZE-1) ;
            mObjects[i].object->vCalculateShow(time_c-RSS_Kernel::calc_time_step, time_c) ;

         if(mObjects[i].cut_time!=    0. &&
            mObjects[i].cut_time<=time_c   )  mObjects[i].active=0 ;
/*- - - - - - - - - - - - - - - - -  ��������� ������ �������� ����� */
         if(callback[0]!=0) {

             for(cmd=callback ; *cmd!=0 ; cmd=end+1) {

                end=strchr(cmd, ';') ;
               *end= 0 ;

                      memset(&frame, 0, sizeof(frame)) ;

                if(!memicmp(cmd, "START ", strlen("START "))) {
                     strcpy(frame.action, "START") ;
                     strcpy(frame.object, cmd+strlen("START ")) ;
                                                              }
                else
                if(!memicmp(cmd, "STOP ", strlen("STOP "))) {
                     strcpy(frame.action, "STOP") ;
                     strcpy(frame.object, cmd+strlen("STOP ")) ;
                                                            }
                else
                if(!memicmp(cmd, "KILL ", strlen("KILL "))) {
                     strcpy(frame.action, "KILL") ;
                     strcpy(frame.object, cmd+strlen("KILL ")) ;
                                                            }
                else
                if(!memicmp(cmd, "EXEC ", strlen("EXEC "))) {
                     strcpy(frame.action, "EXEC") ;
                     strcpy(frame.command, cmd+strlen("EXEC ")) ;
                                                            }

                    iFrameExecute(&frame, time_c, 0) ;
                                                     }
                            }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                               }
/*------------------------------------------------ ��������� ������� */

      for(i=0 ; i<mSpawns_cnt ; i++) 
        if(mSpawns[i]!=NULL) {

#define  S  mSpawns[i] 

         while(S->cnt        < S->size &&
               S->next_event < time_c    ) {
/*- - - - - - - - - - - - - - - - - - - - - - - - - "������" ������� */
             sprintf(name, "%s_%d", S->object, S->cnt+1) ;

                clone=S->templ->vCopy(name) ;
                clone->battle_state=_SPAWN_STATE ;

                clone->vResetFeatures(NULL) ;                       /* ������������� ������� ������� */ 

                clone->vCalculateStart(S->next_event) ;
                clone->vCalculate     (S->next_event, time_c, NULL, 0) ;
                clone->vCalculateShow (S->next_event, time_c) ;
/*- - - - - - - - - - - - - - - - - - -  ���������� ������� � ������ */
           if(mObjects_cnt>=_OBJECTS_MAX) {
                      SEND_ERROR("������ �������� �������� ������� �����") ;
                                              exit_flag=1 ;  break ;
                                          }

                   mObjects[mObjects_cnt].object  =clone ;
                   mObjects[mObjects_cnt].active  =  1 ;

           if(S->t_par!=0.)
                   mObjects[mObjects_cnt].cut_time=S->next_event+S->t_par ;
           else    mObjects[mObjects_cnt].cut_time= 0. ;
                            mObjects_cnt++ ;
/*- - - - - - - - - - - - - - - - - - - - -  ������ ���������� ����� */
                       S->next_event+=S->period ;
                       S->cnt       ++ ;

                    if(S->cnt==S->size) {  S=NULL ;  break ;  }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                          }
#undef   S
                             }
/*----------------------------------------------- �������� ��������� */

      for(i=0 ; i<mObjects_cnt ; i++)
        if(mObjects[i].active)  mObjects[i].object->vPrepareFeatures(NULL) ;

      for(i=0 ; i<mObjects_cnt ; i++)
        if(mObjects[i].active) {
                                                               checked.Clear() ;
              status=mObjects[i].object->vCheckFeatures(NULL, &checked) ;
           if(status) {

                    mObjects[i].active=mObjects[i].object->vEvent("HIT", time_c) ;

             for(j=0 ; j<checked.List_cnt ; j++)
               if(!stricmp(checked.List[j].relation, "Hit"))
                      checked.List[j].object->vEvent("HITED", time_c) ;

                      }
                               }
/*-------------------------------------------------- ��������� ����� */

          time_1=this->kernel->vGetTime() ;
       if(time_1-time_s>=this->kernel->show_time_step) {

                 time_s=time_1 ;

              this->kernel->vShow(NULL) ;

       if(this->mMapWindow!=NULL)  SendMessage(this->mMapWindow, WM_PAINT, NULL, NULL) ;

                                                       }
/*-------------------------------------- ������� �������������� ���� */

     } while(!exit_flag) ;  

                    hit_sum+=mHit_cnt ;
                    hit_avg =((double)hit_sum)/(attempt+1.) ;

              this->kernel->vShow(NULL) ;                           /* ��������� ��������� */

/*----------------------------------------------------- ���� ������� */ 
                                                    }               /* END LOOP */

                       RSS_Kernel::battle=0 ;

    if(attempt_cnt==1)  SEND_ERROR("��� ��������") ;
    else                SEND_ERROR("������������� ���������") ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� MAP                     */
/*								    */
/*        MAP <�������� �����������>                                */

  int  RSS_Module_Battle::cMap(char *cmd, RSS_IFace *iface)

{
#define   _PARS_MAX  10

      char *pars[_PARS_MAX] ;
       int  quiet_flag ;
       int  trace_oper ;
       int  color_oper ;
  COLORREF  color ;
      char *pos ;
      char  text[1024] ;
      char *end ;
       int  i  ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
                   trace_oper=0 ;
                   color_oper=0 ;
                   quiet_flag=0 ;

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
                   strchr(cmd, 'T')!=NULL   )  trace_oper=1 ;
                if(strchr(cmd, 'c')!=NULL ||
                   strchr(cmd, 'C')!=NULL   )  color_oper=1 ;
                if(strchr(cmd, 'q')!=NULL ||
                   strchr(cmd, 'Q')!=NULL   )  quiet_flag=1 ;

                           cmd=end+1 ;
                        }

    if(trace_oper &&
       color_oper   ) {
                        SEND_ERROR("������������� ������������� ������ T � C �����������") ;
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
/*--------------------------------------- ������� ������ ����������� */

    if(trace_oper) {
/*- - - - - - - - - - - - - - - - - - - - - - -  ����������� ������� */
     if(pars[0]==NULL) {
                         SEND_ERROR("�� ������ ��� �������. \n"
                                    "��������: MAP/T <���_�������>") ;
                                        return(-1) ;
                       } 

       for(i=0 ; i<OBJECTS_CNT ; i++)                               /* ���� ������ �� ����� */
         if(!stricmp(OBJECTS[i]->Name, pars[0]))  break ;

 
    if(i==OBJECTS_CNT) {                                            /* ���� ��� �� �������... */
                           sprintf(text, "�������� ���������, ���� ������� � ������ '%s' "
                                         "�� ����������.", pars[0]) ;
       if(!quiet_flag)  SEND_ERROR(text) ;
                       }
/*- - - - - - - - - - - - - - - - - - - - -  ����������� ����������� */
    if(mMapTraces_cnt>=_OBJECTS_MAX) {
                      SEND_ERROR("������ ����������� �������� ������� �����") ;
                                         return(-1) ;
                                     }

       strncpy(mMapTraces[mMapTraces_cnt].object, pars[0], sizeof(mMapTraces[0].object)-1) ;
                          mMapTraces_cnt++ ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                   }
/*-------------------------------------------- ������� ����� ������� */

    else
    if(color_oper) {
/*- - - - - - - - - - - - - - - - - - - - - - - -  ����������� ����� */
     if(pars[1]==NULL) {
                         SEND_ERROR("�� ����� ���� �������. \n"
                                    "��������: MAP/C <���_�������> GREEN") ;
                                        return(-1) ;
                       } 

              if(!stricmp(pars[1], "RED"  ))  color=RGB(127,   0,   0) ;
         else if(!stricmp(pars[1], "GREEN"))  color=RGB(  0, 127,   0) ;
         else if(!stricmp(pars[1], "BLUE" ))  color=RGB(  0,   0, 127) ;
         else                                {
                         SEND_ERROR("����������� �������� �����") ;
                                        return(-1) ;
                                             }
/*- - - - - - - - - - - - - - - - - - - - - - -  ����������� ������� */
     if(pars[0]==NULL) {
                         SEND_ERROR("�� ������ ��� �������. \n"
                                    "��������: MAP/C <���_�������> GREEN") ;
                                        return(-1) ;
                       } 

       for(i=0 ; i<OBJECTS_CNT ; i++)                               /* ���� ������ �� ����� */
         if(!stricmp(OBJECTS[i]->Name, pars[0]))  break ;

 
    if(i==OBJECTS_CNT) {                                            /* ���� ��� �� �������... */
                           sprintf(text, "�������� ���������, ���� ������� � ������ '%s' "
                                         "�� ����������.", pars[0]) ;
       if(!quiet_flag)  SEND_ERROR(text) ;
                       }
/*- - - - - - - - - - - - - - - - - - - - - - - -  ����������� ����� */
    if(mMapColors_cnt>=_OBJECTS_MAX) {
                      SEND_ERROR("������ ������ �������� ������� �����") ;
                                         return(-1) ;
                                     }

       strncpy(mMapColors[mMapColors_cnt].object, pars[0], sizeof(mMapColors[0].object)-1) ;
               mMapColors[mMapColors_cnt].color =color ;
                          mMapColors_cnt++ ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                   }
/*----------------------------------------- �������� ���� ���������� */

    else           {
                      pos=pars[0] ;   
       if(pos==NULL)  pos="0" ;

          strcpy(Context->action, "MAP") ;
          strcpy(Context->details, pos) ;

     SendMessage(RSS_Kernel::kernel_wnd, 
                 WM_USER, (WPARAM)_USER_CHANGE_CONTEXT, 
                          (LPARAM) Context              ) ;

                   }
/*-------------------------------------------------------------------*/

#undef    OBJECTS
#undef    OBJECTS_CNT

   return(0) ;
}


/********************************************************************/
/*								    */
/*              ���������� ���������� ��������� ������              */
/*                                                                  */
/*  start  -  ������ ������������ �������                           */
/*  stop   -  ���������� ������������ �������                       */
/*  kill   -  ������� ������                                        */
/*  spawn  -  ���������� ����� ��������                             */
/*  event  -  �������� ������� �� �������                           */
/*  exec   -  ��������� ��������� �������                           */

  int  RSS_Module_Battle::iFrameExecute(FRAME *frame, double  t, int sync_use)

{ 
       static  double  t_last ;
            RSS_IFace  iface ;
           RSS_Object *object ;
                 char  text[1024] ;
                 char *end ;
                  int  status ;
                  int  i ;
                  int  j ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*---------------------------------------------------- ������������� */

/*--------------------------------- �������� ��������� ������������� */

 if(sync_use && frame->sync_type!=_NO_SYNC) {

    if(frame->sync_type==_T_SYNC &&
       frame->t        >= t         )  return(_WAIT_FRAME) ;

    if(frame->sync_type==_DT_SYNC &&
       frame->dt+t_last>=  t        )  return(_WAIT_FRAME) ;

                  t_last=t ;
                                            }
/*---------------------------------------------------- �������� EXIT */

    if(!stricmp(frame->action, "EXIT" )) {

                                     return(_EXIT_FRAME) ;

                                         }
/*---------------------------------------------------- �������� EXIT */
    else
    if(!stricmp(frame->action, "EXEC" )) {

      for(i=0 ; i<RSS_Kernel::kernel->modules_cnt ; i++) {
      
             status=RSS_Kernel::kernel->modules[i].entry->vExecuteCmd(frame->command) ;
          if(status==-1)  return(-1) ;
          if(status== 0)    break ;
                                                         }

          if(status== 1)
             status=RSS_Kernel::kernel->vExecuteCmd(frame->command) ;

          if(status== 1) {
                              sprintf(text, "����������� �������: %s", frame->command) ;
                           SEND_ERROR(text) ;
                               return(_EXIT_FRAME) ;
                         }
                                         }
/*--------------------------------------------------- �������� START */
    else
    if(!stricmp(frame->action, "START")) {
/*- - - - - - - - - - - - - - - - - - - - - -  ������������� ������� */
         for(i=0 ; i<OBJECTS_CNT ; i++)                             /* ���� ������ �� ����� */
           if(!stricmp(OBJECTS[i]->Name, frame->object))  break ;

           if(i==OBJECTS_CNT) {                                     /* ���� ��� �� �������... */
                                   sprintf(text, "������� '%s' �� ����������", frame->object) ;
                                SEND_ERROR(text) ;
                                    return(_EXIT_FRAME) ;
                              }

                      object=OBJECTS[i] ;

        for(i=0 ; i<mObjects_cnt ; i++)
          if(object==mObjects[i].object)  break ;
          
           if(i<mObjects_cnt) {                                     /* ���� ������ ��� ������� � ��������... */              
                                    sprintf(text, "������� '%s' ��� ������� � ��������", frame->object) ;
                                 SEND_ERROR(text) ;
                                     return(_EXIT_FRAME) ;
                              }
/*- - - - - - - - - - - - - - - - - - -  ���������� ������� � ������ */
     if(mObjects_cnt>=_OBJECTS_MAX) {
                      SEND_ERROR("������ �������� �������� ������� �����") ;
                                       return(_EXIT_FRAME) ;
                                    }

         if(object->battle_state!=_SPAWN_STATE) {                   /* ���� ������ �� ������ � �������� ���... */
                   object->battle_state=_ACTIVE_STATE ;
                   object->vPush() ;
                                                }

                   object->vSpecial       ("BATTLE", this) ;
                   object->vResetFeatures (NULL) ;
                   object->vCalculateStart(t) ;

               mObjects[mObjects_cnt].object  =object ;
               mObjects[mObjects_cnt].active  =  1 ;

       if(frame->t_par!=0.)
               mObjects[mObjects_cnt].cut_time=t+frame->t_par ;
       else    mObjects[mObjects_cnt].cut_time= 0. ;
                        mObjects_cnt++ ;
/*- - - - - - - - - - - - - - - - - -  ������������� ������� ������� */
                      object->vResetFeatures(NULL) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                         }
/*---------------------------------------------------- �������� STOP */
    else
    if(!stricmp(frame->action, "STOP")) {
/*- - - - - - - - - - - - - - - - - - - - - -  ������������� ������� */
         for(i=0 ; i<OBJECTS_CNT ; i++)                             /* ���� ������ �� ����� */
           if(!stricmp(OBJECTS[i]->Name, frame->object))  break ;

           if(i==OBJECTS_CNT) {                                     /* ���� ��� �� �������... */
                                   sprintf(text, "������� '%s' �� ����������", frame->object) ;
                                SEND_ERROR(text) ;
                                    return(_EXIT_FRAME) ;
                              }

                      object=OBJECTS[i] ;
/*- - - - - - - - - - - - - - - - - - ����� ����� ���������� ������� */
        for(i=0 ; i<mObjects_cnt ; i++)
          if(object==mObjects[i].object)  break ;

           if(i>=mObjects_cnt) {                                    /* ���� ������ ��� ������� � ��������... */
                                    sprintf(text, "������� '%s' �� ������� � ��������", frame->object) ;
                                 SEND_ERROR(text) ;
                                     return(_EXIT_FRAME) ;
                              }

               mObjects[i].active=0 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                         }
/*---------------------------------------------------- �������� KILL */
    else
    if(!stricmp(frame->action, "KILL")) {
/*- - - - - - - - - - - - - - - - - - -  ����������� ������ �������� */
                      end=strchr(frame->object, '%') ;
       if(end==NULL)  end=strchr(frame->object, '*') ;
       if(end!=NULL) *end= 0 ;
/*- - - - - - - - - - - - - - - - - - - - - - ��� ���������� ������� */
       if(end!=NULL) {

            for(i=0, j=0 ; i<mObjects_cnt ; i++) {                       /* ������� ������ ��� */

              if(i!=j)  mObjects[j].object=mObjects[i].object ;

              if(mObjects[j].object!=NULL)
               if(memicmp(mObjects[j].object->Name, frame->object,
                                             strlen(frame->object)))  j++ ;
                                                 }

                                          mObjects_cnt=j ;

                                      sprintf(text, "kill/b %s%%", frame->object) ;
              RSS_Kernel::kernel->vExecuteCmd(text) ;               /* �������� �������� */

                     }
/*- - - - - - - - - - - - - - - - - - - - - - ��� ���������� ������� */
       else          {

         for(i=0 ; i<OBJECTS_CNT ; i++)                             /* ���� ������ �� ����� */
           if(!stricmp(OBJECTS[i]->Name, frame->object))  break ;

           if(i==OBJECTS_CNT) {                                     /* ���� ��� �� �������... */
                                   sprintf(text, "������� '%s' �� ����������", frame->object) ;
                                SEND_ERROR(text) ;
                                    return(_EXIT_FRAME) ;
                              }

                      object=OBJECTS[i] ;

         for(i=0 ; i<mObjects_cnt ; i++)                            /* ����� ����� ���������� ������� */
           if(object==mObjects[i].object)  break ;

           if(i<mObjects_cnt) {
                                 mObjects[i].object=NULL ;
                                 mObjects[i].active=  0 ;
                              }

                                      sprintf(text, "kill/b %s", frame->object) ;
              RSS_Kernel::kernel->vExecuteCmd(text) ;               /* �������� ������� */
                     }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                         }
/*--------------------------------------------------- �������� EVENT */
    else
    if(!stricmp(frame->action, "EVENT")) {
/*- - - - - - - - - - - - - - - - - - - - - -  ������������� ������� */
         for(i=0 ; i<OBJECTS_CNT ; i++)                             /* ���� ������ �� ����� */
           if(!stricmp(OBJECTS[i]->Name, frame->object))  break ;

           if(i==OBJECTS_CNT) {                                     /* ���� ��� �� �������... */
                                   sprintf(text, "������� '%s' �� ����������", frame->object) ;
                                SEND_ERROR(text) ;
                                    return(_EXIT_FRAME) ;
                              }

                      object=OBJECTS[i] ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - ������ � ������� */
                   object->vEvent(frame->event, t) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                         }
/*--------------------------------------------------- �������� SPAWN */
    else
    if(!stricmp(frame->action, "SPAWN")) {
/*- - - - - - - - - - - - - - - - - - - - - -  ������������� ������� */
         for(i=0 ; i<OBJECTS_CNT ; i++)                             /* ���� ������ �� ����� */
           if(!stricmp(OBJECTS[i]->Name, frame->object))  break ;

           if(i==OBJECTS_CNT) {                                     /* ���� ��� �� �������... */
                                   sprintf(text, "������� '%s' �� ����������", frame->object) ;
                                SEND_ERROR(text) ;
                                    return(_EXIT_FRAME) ;
                              }

                      frame->templ=OBJECTS[i] ;
/*- - - - - - - - - - - - - - -  ���������� ������ � ������ �������� */
           for(j=0 ; j<mSpawns_cnt ; j++)
             if(mSpawns[j]==NULL)  break ;

           if(j>=_SPAWNS_MAX) {                                     /* ���� ���� � ������ ���... */
                                   sprintf(text, "������������ ������ �����-��������") ;
                                SEND_ERROR(text) ;
                                    return(_EXIT_FRAME) ;
                              }

                mSpawns[j]=frame ;
                           frame->cnt       =0 ;
                           frame->next_event=t ;

             if(j>=mSpawns_cnt)  mSpawns_cnt=j+1 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                         }
/*--------------------------------------------- ����������� �������� */

    else                                {

                 sprintf(text, "BATTLE - ����������� ��������: %s", frame->action) ;
              SEND_ERROR(text) ;
                  return(_EXIT_FRAME) ;
                                        }
/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

   return(_NEXT_FRAME) ;
}


/*********************************************************************/
/*								     */
/*	      ���������� ������ "������� ���������"	             */
/*								     */
/*********************************************************************/

/*********************************************************************/
/*								     */
/*	       ����������� ������ "������� ���������"      	     */

     RSS_Transit_Battle::RSS_Transit_Battle(void)

{
}


/*********************************************************************/
/*								     */
/*	        ���������� ������ "������� ���������"      	     */

    RSS_Transit_Battle::~RSS_Transit_Battle(void)

{
}


/********************************************************************/
/*								    */
/*	              ���������� ��������                           */

    int  RSS_Transit_Battle::vExecute(void)

{
     HWND  hWnd ;
     RECT  rect ;
      int  x ; 
      int  y ; 
      int  w_x ; 
      int  w_y ; 

/*----------------------------------------------- �������� ����� ��� */

   if(!stricmp(action, "MAP")) {

         hWnd=CREATE_DIALOG(GetModuleHandle(NULL),
                             (LPCDLGTEMPLATE)ProgramModule.Resource("IDD_MAP", RT_DIALOG),
	                        NULL, Task_Battle_Map_dialog, 
                                  (LPARAM)&ProgramModule) ;

             GetWindowRect(hWnd, &rect);
                              w_x=rect.bottom-rect.top +1 ;
                              w_y=rect.right -rect.left+1 ;

                                 x= 0 ;
                                 y= 0 ;
         if(details[0]=='1') {   x=  w_x+  (RSS_Kernel::display.x-4*w_x)/3 ;
                                 y= 0 ;                                       }
         if(details[0]=='2') {   x=2*w_x+2*(RSS_Kernel::display.x-4*w_x)/3 ;
                                 y= 0 ;                                       }
         if(details[0]=='3') {   x= RSS_Kernel::display.x-w_x ;
                                 y= 0 ;                                       }
         if(details[0]=='4') {   x= RSS_Kernel::display.x-w_x ;
                                 y=(RSS_Kernel::display.y-w_y)/2 ;            }
         if(details[0]=='5') {   x= RSS_Kernel::display.x-w_x ;
                                 y= RSS_Kernel::display.y-w_y ;               }
         if(details[0]=='6') {   x=2*w_x+2*(RSS_Kernel::display.x-4*w_x)/3 ;
                                 y= RSS_Kernel::display.y-w_y ;               }
         if(details[0]=='7') {   x=  w_x+  (RSS_Kernel::display.x-4*w_x)/3 ;
                                 y= RSS_Kernel::display.y-w_y ;               }
         if(details[0]=='8') {   x= 0 ;
                                 y= RSS_Kernel::display.y-w_y ;               }
         if(details[0]=='9') {   x= 0 ;
                                 y=(RSS_Kernel::display.y-w_y)/2 ;            }


                     SetWindowPos(hWnd, NULL, x, y, 0, 0,
                                   SWP_NOOWNERZORDER | SWP_NOSIZE);
                       ShowWindow(hWnd, SW_SHOW) ;

                         SetFocus(ProgramModule.kernel_wnd) ;

                                   ProgramModule.mMapWindow=hWnd ;

                                     }
/*-------------------------------------------------------------------*/

   return(0) ;
}

/********************************************************************/
/*								    */
/*       МОДУЛЬ УПРАВЛЕНИЯ КОМПОНЕНТОМ "2-Х ЭТАПНОЕ НАВЕДЕНИЕ"      */
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

#include "U_Control2Stage.h"

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
/*		    	Программный модуль                          */

     static   RSS_Module_Control2Stage  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	Идентификационный вход                      */

 U_CONTROL_2STAGE_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 U_CONTROL_2STAGE_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/********************************************************************/
/**							           **/
/**            ОПИСАНИЕ КЛАССА МОДУЛЯ УПРАВЛЕНИЯ ОБЪЕКТОМ          **/
/**                      "2-Х ЭТАПНОЕ НАВЕДЕНИЕ"	               **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*                            Список команд                         */

  struct RSS_Module_Control2Stage_instr  RSS_Module_Control2Stage_InstrList[]={

 { "help",    "?",  "#HELP   - список доступных команд", 
                     NULL,
                    &RSS_Module_Control2Stage::cHelp   },
 { "config",  "c",  "#CONFIG - задать параметры компонента в диалоговом режиме",
                    " CONFIG <Имя> \n"
                    "   Задать параметры компонента в диалоговом режиме\n",
                    &RSS_Module_Control2Stage::cConfig },
 { "program", "p", "#PROGRAM - задание программы управления объектом",
                   " PROGRAM <Имя> <Имя файла программы>\n"
                   "   Задание программы управления объектом\n",
                    &RSS_Module_Control2Stage::cProgram },
 {  NULL }
                                                            } ;


/********************************************************************/
/*								    */
/*		     Общие члены класса             		    */

    struct RSS_Module_Control2Stage_instr *RSS_Module_Control2Stage::mInstrList=NULL ;


/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Module_Control2Stage::RSS_Module_Control2Stage(void)

{

/*---------------------------------------------------- Инициализация */

	   keyword="EmiStand" ;
    identification="Control2Stage" ;
          category="Unit" ;
         lego_type="Control" ;

        mInstrList=RSS_Module_Control2Stage_InstrList ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Module_Control2Stage::~RSS_Module_Control2Stage(void)

{
}


/********************************************************************/
/*								    */
/*		      Создание объекта                              */

  RSS_Object *RSS_Module_Control2Stage::vCreateObject(RSS_Model_data *data)

{
   RSS_Unit_Control2Stage *unit ;
                      int  i ;
 
/*---------------------------------------------- Создание компонента */

       unit=new RSS_Unit_Control2Stage ;
    if(unit==NULL) {
               SEND_ERROR("Секция Control2Stage: Недостаточно памяти для создания компонента") ;
                        return(NULL) ;
                   }

             unit->Module=this ;

      strcpy(unit->Decl, "Система управления с 2-х этапным наведением") ;

/*------------------------------- Создание списка свойств компонента */

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
/*		        Получить параметр       		    */

     int  RSS_Module_Control2Stage::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- Описание модуля */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s -  СУ с 2-х этапным наведением", identification) ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		        Выполнить команду       		    */

  int  RSS_Module_Control2Stage::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* Флаг режима прямой команды */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "CONTROL2STAGE"
#define  _SECTION_SHRT_NAME   "CONTROL2"

/*--------------------------------------------- Идентификация секции */

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
/*----------------------- Включение/выключение режима прямой команды */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - -  Включение */
   if(end==NULL || end[0]==0) {
     if(!direct_command) {
                            direct_command=1 ;

        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Object Control2Stage:") ;
        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_DIRECT_COMMAND, (LPARAM)identification) ;
                         }
                                    return(0) ;
                              }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - Выключение */
   if(end!=NULL && !strcmp(end, "..")) {

                            direct_command=0 ;

        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"") ;
        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_DIRECT_COMMAND, (LPARAM)"") ;

                                           return(0) ;
                                       }
/*--------------------------------------------- Выделение инструкции */

       instr=end ;                                                  /* Выделяем слово с названием команды */

     for(end=instr ; *end!= 0  &&
                     *end!=' ' &&
                     *end!='>' &&
                     *end!='/'    ; end++) ;

      if(*end!= 0 &&
         *end!=' '  )  memmove(end+1, end, strlen(end)+1) ;

      if(*end!=0) {  *end=0 ;  end++ ;  }
      else            end="" ;

   for(i=0 ; mInstrList[i].name_full!=NULL ; i++)                   /* Ищем команду в списке */
     if(!stricmp(instr, mInstrList[i].name_full) ||
        !stricmp(instr, mInstrList[i].name_shrt)   )   break ;

     if(mInstrList[i].name_full==NULL) {                            /* Если такой команды нет... */

          status=this->kernel->vExecuteCmd(cmd) ;                   /*  Пытаемся передать модулю ядра... */
       if(status)  SEND_ERROR("Секция Control2Stage: Неизвестная команда") ;
                                            return(-1) ;
                                       }
 
     if(mInstrList[i].process!=NULL)                                /* Выполнение команды */
                status=(this->*mInstrList[i].process)(end) ;
     else       status=  0 ;

/*-------------------------------------------------------------------*/

   return(status) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции HELP                    */

  int  RSS_Module_Control2Stage::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Unit_Control2Stage_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции INFO                    */
/*								    */
/*        INFO   <Имя>                                              */
/*        INFO/  <Имя>                                              */

  int  RSS_Module_Control2Stage::cInfo(char *cmd)

{
                     char  *name ;
   RSS_Unit_Control2Stage  *unit ;
                      int   all_flag ;   /* Флаг режима полной информации */
                     char  *end ;
              std::string   info ;
              std::string   f_info ;
                     char   text[4096] ;
                      int   i ;

/*---------------------------------------- Разборка командной строки */
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
                           all_flag=0 ;

       if(*cmd=='/') {
                           all_flag=1 ;
 
                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("Некорректный формат команды") ;
                                       return(-1) ;
                              }

                           cmd=end+1 ;
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - -  Разбор параметров */
                  name=cmd ;
                   end=strchr(name, ' ') ;
                if(end!=NULL)  *end=0 ;

/*---------------------------------------- Контроль имени компонентa */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* Если имя не задано... */
                           SEND_ERROR("Не задано имя компонент. \n"
                                      "Например: INFO <Имя_компонент> ...") ;
                                     return(-1) ;
                         }

       unit=(RSS_Unit_Control2Stage *)FindUnit(name) ;              /* Ищем компонент по имени */
    if(unit==NULL)  return(-1) ;

/*-------------------------------------------- Формирование описания */

      sprintf(text, "%s\r\n%s\r\n"
                    "Owner       %s\r\n" 
                    "\r\n",
                        unit->Name,      unit->Type, 
                        unit->Owner->Name
                    ) ;

           info=text ;

/*----------------------------------------------- Запрос на Свойства */

   for(i=0 ; i<unit->Features_cnt ; i++) {

                        unit->Features[i]->vGetInfo(&f_info) ;
                                               info+=f_info ;
                                               info+="\r\n" ;
                                           }
/*-------------------------------------------------- Выдача описания */
/*- - - - - - - - - - - - - - - - - - - - - - - - - В отдельное окно */
     if(all_flag) {

                  }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - В главное окно */
     else         {

        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_SHOW_INFO, (LPARAM)info.c_str()) ;
                  }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции CONFIG                  */
/*								    */
/*     CONFIG <Имя комонента>                                       */

  int  RSS_Module_Control2Stage::cConfig(char *cmd)

{
#define   _PARS_MAX  10

                    char *pars[_PARS_MAX] ;
                    char *name ;
  RSS_Unit_Control2Stage *unit ;
                 INT_PTR  status ;
                    char *end ;
                     int  i ;

/*---------------------------------------- Разборка командной строки */

/*- - - - - - - - - - - - - - - - - - - - - - - -  Разбор параметров */        
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

           if( pars[0]==NULL ||
              *pars[0]==  0    ) {
                                     SEND_ERROR("Не задан компонент") ;
                                         return(-1) ;
                                 }

                     name=pars[0] ;

/*---------------------------------------- Контроль имени компонента */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* Если имя не задано... */
                           SEND_ERROR("Не задано имя компонентa. \n"
                                      "Например: CONFIG <Имя_компонентa> ...") ;
                                     return(-1) ;
                         }

       unit=(RSS_Unit_Control2Stage *)FindUnit(name) ;              /* Ищем компонент по имени */
    if(unit==NULL)  return(-1) ;

/*--------------------------------------- Переход в диалоговый режим */

       status=DialogBoxIndirectParam( GetModuleHandle(NULL),
                                     (LPCDLGTEMPLATE)Resource("IDD_CONFIG", RT_DIALOG),
                                      GetActiveWindow(), 
                                      Unit_Control2Stage_Config_dialog, 
                                     (LPARAM)unit                    ) ;
    if(status)  return(-1) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции PROGRAM                 */
/*								    */
/*     PROGRAM <Имя комонента> <Путь к файлу программы>             */

  int  RSS_Module_Control2Stage::cProgram(char *cmd)

{
#define   _PARS_MAX  10

                    char *pars[_PARS_MAX] ;
                    char *name ;
                    char *path ;
  RSS_Unit_Control2Stage *unit ;
                 INT_PTR  status ;
                    char  error[1024] ;
                    char *end ;
                     int  i ;

/*---------------------------------------- Разборка командной строки */

/*- - - - - - - - - - - - - - - - - - - - - - - -  Разбор параметров */        
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

           if( pars[0]==NULL ||
              *pars[0]==  0    ) {
                                     SEND_ERROR("Не задан компонент") ;
                                         return(-1) ;
                                 }

                     name=pars[0] ;
                     path=pars[1] ;

/*---------------------------------------------- Контроль параметров */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* Если имя не задано... */
                           SEND_ERROR("Не задано имя компонентa. \n"
                                      "Например: PROGRAM <Имя_компонентa> ...") ;
                                     return(-1) ;
                         }

    if(path   ==NULL ||
       path[0]==  0    ) {                                          /* Если файл программы не задано... */
                           SEND_ERROR("Не задано имя компонентa. \n"
                                      "Например: PROGRAM <Имя_компонентa> ...") ;
                                     return(-1) ;
                         }


       unit=(RSS_Unit_Control2Stage *)FindUnit(name) ;              /* Ищем компонент по имени */
    if(unit==NULL)  return(-1) ;

/*--------------------------------------- Переход в диалоговый режим */

       status=ReadProgram(unit, path, error) ;
    if(status) {
                  SEND_ERROR(error) ;
                     return(-1) ;
               }
/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*	       Поиск обьекта типа Control2Stage по имени            */

  RSS_Unit *RSS_Module_Control2Stage::FindUnit(char *name)

{
 RSS_Object *object ;
 RSS_Object *unit ;
       char  o_name[128] ;
       char *u_name ;
       char  text[1024] ;
        int  i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*-------------------------------------------------- Разделение имен */ 

               memset(o_name, 0, sizeof(o_name)) ;
              strncpy(o_name, name, sizeof(o_name)-1) ;
        u_name=strchr(o_name, '.') ;

     if(u_name==NULL) {
                           sprintf(text, "Имя '%s' не является именем компонента объекта", name) ;
                        SEND_ERROR(text) ;
                            return(NULL) ;
                      }

       *u_name=0 ;
        u_name++ ;
     
/*------------------------------------------- Поиск объекта по имени */ 

             object=NULL ;

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, o_name)) {
                                                   object=OBJECTS[i] ;
                                                     break ; 
                                                }

    if(object==NULL) {
                           sprintf(text, "Объекта с именем '%s' НЕ существует", o_name) ;
                        SEND_ERROR(text) ;
                            return(NULL) ;
                     }
/*---------------------------------------- Поиск компонента по имени */ 

       for(i=0 ; i<object->Units.List_cnt ; i++)
         if(!stricmp(object->Units.List[i].object->Name, u_name)) { 
                         unit=object->Units.List[i].object ;
                                               break ;
                                                                  }

    if(unit==NULL) {
                           sprintf(text, "Объект '%s' не включает компонент '%s'", o_name, u_name) ;
                        SEND_ERROR(text) ;
                            return(NULL) ;
                   }
/*-------------------------------------------- Контроль типа объекта */ 

     if(strcmp(unit->Type, "Control2Stage")) {

           SEND_ERROR("Компонент не является компонентом типа Control2Stage") ;
                            return(NULL) ;
                                             }
/*-------------------------------------------------------------------*/ 

   return((RSS_Unit *)unit) ;

#undef   OBJECTS
#undef   OBJECTS_CNT

}


/********************************************************************/
/*								    */
/*	             Считывание программы управления                */

  int  RSS_Module_Control2Stage::ReadProgram(RSS_Unit_Control2Stage *unit, char *path, char *error)

{
     FILE *file ;
     char  text[1024] ;
      int  row ;
     char *end ;
      int  start_stage ;
    Stage *stage ;

/*---------------------------------------------------- Инициализация */ 

                    *error=0 ; 

/*--------------------------------------------------- Открытие файла */ 

        file=fopen(path, "rb") ;
     if(file==NULL)  {
                         sprintf(error, "Control2Stage - program file open error %d : %s", errno, path) ;
                           return(-1) ;
                     }
/*------------------------------------------------- Считывание файла */ 

           unit->stages_cnt=0 ;

                    row=0 ;

            start_stage=1 ;

    do {
/*- - - - - - - - - - - - - - - - - - -  Считывание очередной строки */
               memset(text, 0, sizeof(text)) ;
            end=fgets(text, sizeof(text)-1, file) ;
         if(end==NULL)  break ;

             row++ ;

            end=strchr(text, '\r') ;
         if(end!=NULL)  *end=0 ; 
            end=strchr(text, '\n') ;
         if(end!=NULL)  *end=0 ; 

         if(text[sizeof(text)-1]!=0) {
                         sprintf(error, "Control2Stage - to large row %d in program file : %s", row, path) ;
                                        break ;
                                     }

         if(text[0]==';')  continue ;                               /* Обработка комментария */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Стартовый блок */
         if(start_stage) {

             if(stricmp(text, "start")) {
                           sprintf(error, "Control2Stage - First section in program file must be 'start' (row %d) : %s", row, path) ;
                                            break ;
                                        }

                               stage=&unit->stage_start ;
                        memset(stage, 0, sizeof(*stage)) ;

                               start_stage=0 ;
                                  continue ;
                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - Начало блока */
         if(!memicmp(text, "stage", strlen("stage"))) {

            if(unit->stages_cnt>=_STAGES_MAX) {
                           sprintf(error, "Control2Stage - Stages list overflow in row %d : %s", row, path) ;
                                                 break ;
                                              }

                               stage=&unit->stages[unit->stages_cnt] ;
                        memset(stage, 0, sizeof(*stage)) ;

                                      unit->stages_cnt++ ;
                                        continue ;
                                                      }
/*- - - - - - - - - - - - - - - - - - - - Условие использования - IF */
#define  A   stage->actions[stage->actions_cnt]

         if(!memicmp(text, "if ", strlen("if "))) {

              memset(&A, 0, sizeof(StageAction)) ;

              strncpy(A.operation, text+strlen("if "), sizeof(A.operation)-1) ;
                      A.if_condition=1 ; 

                      stage->actions_cnt++ ;
                                                  }
/*- - - - - - - - - - - - - - - - - - - - - -  Условие выхода - EXIT */
         else
         if(!memicmp(text, "exit ", strlen("exit "))) {

              memset(&A, 0, sizeof(StageAction)) ;

              strncpy(A.operation, text+strlen("exit "), sizeof(A.operation)-1) ;
                      A.exit_condition=1 ; 

                      stage->actions_cnt++ ;
                                                      }
/*- - - - - - - - - - - - - - Однократно выполняемый оператор - ONCE */
         else
         if(!memicmp(text, "once ", strlen("once "))) {

              memset(&A, 0, sizeof(StageAction)) ;

              strncpy(A.operation, text+strlen("once "), sizeof(A.operation)-1) ;
                      A.once=1 ; 

                      stage->actions_cnt++ ;
                                                      }
/*- - - - - - - - - - - - - - - - - - - - - - - - - Простой оператор */
         else                                        {

              memset(&A, 0, sizeof(StageAction)) ;

              strncpy(A.operation, text, sizeof(A.operation)-1) ;

                      stage->actions_cnt++ ;
                                                     }
#undef   A 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
       } while(1) ;

/*--------------------------------------------------- Закрытие файла */ 

        fclose(file) ;

/*-------------------------------------------------------------------*/ 
  
   if(*error!=0)  return(-1) ;
                  return( 0) ;
}


/********************************************************************/
/********************************************************************/
/**							           **/
/**	  ОПИСАНИЕ КЛАССА КОМПОНЕНТА "2-Х ЭТАПНОЕ НАВЕДЕНИЕ"       **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Unit_Control2Stage::RSS_Unit_Control2Stage(void)

{
   strcpy(Type, "Control2Stage") ;
          Module=&ProgramModule ;

   Parameters    =NULL ;
   Parameters_cnt=  0 ;

       stages_cnt=  0 ;
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Unit_Control2Stage::~RSS_Unit_Control2Stage(void)

{
      vFree() ;
}


/********************************************************************/
/*								    */
/*		       Освобождение ресурсов                        */

  void   RSS_Unit_Control2Stage::vFree(void)

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
/*                        Копировать объекта		            */

    class RSS_Object *RSS_Unit_Control2Stage::vCopy(char *name)

{
            RSS_Model_data  create_data ;
    RSS_Unit_Control2Stage *unit ;
   
/*------------------------------------- Копирование базового объекта */

      memset(&create_data, 0, sizeof(create_data)) ;

       unit=(RSS_Unit_Control2Stage *)this->Module->vCreateObject(&create_data) ;
    if(unit==NULL)  return(NULL) ;

/*
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
*/
/*-------------------------------------------------------------------*/

   return(unit) ;
}


/********************************************************************/
/*								    */
/*                        Специальные действия                      */

     int  RSS_Unit_Control2Stage::vSpecial(char *oper, void *data)
{
/*------------------------------------------ Ссылка на модуль BATTLE */

    if(!stricmp(oper, "BATTLE")) {

//                             this->battle=(RSS_Module_Battle *)data ;
                                      return(0) ;
                                 }
/*-------------------------------------------------------------------*/

  return(-1) ;
}


/********************************************************************/
/*								    */
/*             Подготовка расчета изменения состояния               */

     int  RSS_Unit_Control2Stage::vCalculateStart(double  t)
{
  char  text[1024] ;
  char  error[1024] ;
   int  status ;
   int  i ; 
   int  j ; 

/*------------------------------------------------- Входной контроль */

       if(stages_cnt==0) {

              sprintf(error, "Unit %s.%s - missed program for control unit", this->Owner->Name, this->Name) ;
           SEND_ERROR(error) ;
                                return(-1) ;
                         }
/*---------------------------------------------------- Инициализация */

            start_time=(time_t)t ;

     for(i=0 ; i<this->stages_cnt ; i++) 
     for(j=0 ; j<this->stages[i].actions_cnt ; j++)  this->stages[i].actions[j].done=0 ;

/*---------------------------------------- обработка старового кадра */

      this->warhead_control[0]=0 ;
      this-> homing_control[0]=0 ;

#define   A   this->stage_start.actions

   for(i=0 ; i<this->stage_start.actions_cnt ; i++) {

     if(A[i].if_condition || A[i].exit_condition)  continue ;

            status=ExecuteOperation(A[i].operation, text) ;
         if(status==-1)  {
              sprintf(text, "Unit %s - error in program for control unit, start stage: %s", this->Owner->Name, text) ;
                        SEND_ERROR(error) ;
                             return(-1) ;
                         }
                                                    }

#undef   A

               this->stage=-1 ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*                   Расчет изменения состояния                     */

     int  RSS_Unit_Control2Stage::vCalculate(double t1, double t2, char *callback, int cb_size)
{
    int  next_stage ;
   char  text[1024] ;
   char  error[1024] ;
    int  true_flag ;
    int  status ;
    int  i ;

/*------------------------------------------------- Входной контроль */

/*----------------------------------------------- Приведение времени */

          t1-=this->start_time ;
          t2-=this->start_time ;

/*------------------------------------ Инициализация вычодных данных */

           this->warhead_control[0]=0 ;
           this-> homing_control[0]=0 ;

   memset(&this->vector_control, 0, sizeof(this->vector_control)) ;

/*-------------------------------- Проверка выхода из текущей стадии */

                                 next_stage=0 ;

    if(this->stage>= 0) {

#define  S  this->stages[this->stage]

               true_flag=1 ;

       for(i=0 ; i<S.actions_cnt ; i++)
         if(S.actions[i].exit_condition) {

             status=ExecuteOperation(S.actions[i].operation, error) ;
          if(status==-1) {
                            sprintf(text, "RSS_Unit_Control2Stage::vCalculate - Объект %s - ошибка вычислителя : %s", this->Owner->Name, error) ;
                         SEND_ERROR(text) ;
                              return(-1) ;
                         }

               true_flag&=status ;
                                         }

            if(true_flag) {
                             next_stage=1 ;
                          }
#undef  S  

                        }
    else
    if(this->stage==-1) {
                                 next_stage=1 ;
                        }
/*------------------------------------- Определение следующей стадии */

   if(next_stage) {

#define  S  this->stages[next_stage]

     for(next_stage=this->stage+1 ; 
         next_stage<this->stages_cnt ; next_stage++) {

               true_flag=1 ;

       for(i=0 ; i<S.actions_cnt ; i++)
         if(S.actions[i].if_condition) {

             status=ExecuteOperation(S.actions[i].operation, error) ;
          if(status==-1) {
                            sprintf(text, "RSS_Unit_Control2Stage::vCalculate - Объект %s - ошибка вычислителя : %s", this->Owner->Name, error) ;
                         SEND_ERROR(text) ;
                              return(-1) ;
                         }

               true_flag&=status ;
                                       }

          if(true_flag) {
                            this->stage=next_stage ;
                                    break ;
                        }

                                                     }

       if(next_stage ==this->stages_cnt && 
          this->stage>=  0                )  this->stage=-2 ;

#undef  S  
                  }
/*----------------------------------------- Отработка рабочей стадии */

  if(this->stage>=0) {

#define  S  this->stages[this->stage]

/*- - - - - - - - - - - - - - - - - - - Выполнение команд управления */
       for(i=0 ; i<S.actions_cnt ; i++)
         if(!S.actions[i].if_condition  &&
            !S.actions[i].exit_condition  ) {

          if(S.actions[i].once &&
             S.actions[i].done   )  continue ;

             status=ExecuteOperation(S.actions[i].operation, error) ;
          if(status==-1) {
                            sprintf(text, "RSS_Unit_Control2Stage::vCalculate - Объект %s - ошибка вычислителя : %s", this->Owner->Name, error) ;
                         SEND_ERROR(text) ;
                              return(-1) ;
                         }

               S.actions[i].done=1 ;
                                            }
/*- - - - - - - - - - - - - - - - - - - - - Расчет манёвра наведения */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#undef  S  

                     }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*      Отображение результата расчета изменения состояния          */

     int  RSS_Unit_Control2Stage::vCalculateShow(double  t1, double  t2)
{
  return(0) ;
}

/*********************************************************************/
/*								     */
/*              	  Направление на цель            	     */

    int  RSS_Unit_Control2Stage::vSetHomingDirection(RSS_Point *direction)

{
   return(0) ;
}


/*********************************************************************/
/*								     */
/*             	 Относительное положение цели           	     */

    int  RSS_Unit_Control2Stage::vSetHomingPosition(RSS_Point *position)

{
   return(0) ;
}


/*********************************************************************/
/*								     */
/*               	 Дистанция до цели                 	     */

    int  RSS_Unit_Control2Stage::vSetHomingDistance(double  distance)

{
      t_distance=distance ;

   return(0) ;
}


/*********************************************************************/
/*								     */
/*               	 Скорость сближения с целью            	     */

    int  RSS_Unit_Control2Stage::vSetHomingClosingSpeed(double  velocity)

{
   return(0) ;
}


/*********************************************************************/
/*								     */
/*              	  Управление режимами БЧ         	     */

    int  RSS_Unit_Control2Stage::vGetWarHeadControl(char *regime)

{
     strcpy(regime, this->warhead_control) ;
  

   return(0) ;
}


/*********************************************************************/
/*								     */
/*              	  Управление режимами ГСН         	     */

    int  RSS_Unit_Control2Stage::vGetHomingControl(char *regime)

{
     strcpy(regime, this->homing_control) ;

   return(0) ;
}


/*********************************************************************/
/*								     */
/*              	  Управление двигателем         	     */

    int  RSS_Unit_Control2Stage::vGetEngineControl(RSS_Unit_Engine_Control *engines)

{
   return(0) ;
}


/*********************************************************************/
/*								     */
/*     	  Управление аэродинамическими поверхностями        	     */

    int  RSS_Unit_Control2Stage::vGetAeroControl(RSS_Unit_Aero_Control *aeros)

{
   return(0) ;
}


/*********************************************************************/
/*								     */
/*               Требуемая перегрузка управления                     */

    int  RSS_Unit_Control2Stage::vGetVectorControl(RSS_Vector *vector)

{
     *vector=this->vector_control ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*                   Выполнение оператора управления                */

     int  RSS_Unit_Control2Stage::ExecuteOperation(char *text, char *error)
{
   char  oper[2048] ;
   char  result[256] ;
   char  value[256] ;
   char *control ;
   char *work ;
   char *words[30] ;
    int  status ;
    int  i ;
    int  j ;

/*------------------------------------------------------- Подготовка */

                  control=NULL ;

                 strcpy(oper, text) ;

/*----------------------------------- Выделение префиксов назначения */

   if(!memicmp(oper, "homing:", strlen("homing:"))) {

           control=this->homing_control ;
              work=oper+strlen("homing:") ;
                                                    }
   else
   if(!memicmp(oper, "warhead:", strlen("warhead:"))) {

           control=this->warhead_control ;
              work= oper+strlen("warhead:") ;
                                                      }
   else                                               {

              work= oper ;
                                                      }
/*-------------------------------------------------- Разбор на слова */

              memset(words, 0, sizeof(words)) ;

                      i = 0 ;
                words[i]=strtok(work, " \t") ;

          while(words[i]!=NULL && i<30) {
                      i++ ;
                words[i]=strtok(NULL, " \t") ;
                                        }
/*--------------------------------------------------- Обработка слов */
 
                       result[0]=0 ;

     for(i=0 ; i<30 && words[i]!=NULL ; i++) {

          if(words[i]==0)  continue ;

          status=ExecuteExpression(words[i], value, error) ;
       if(status)  return(-1) ;

       if(result[0]!=0)  strcat(result, " "  ) ;
                         strcat(result, value) ;

                                             }
/*-------------------------------------------- Выполнение назначения */

   if(control!=NULL) {
                       if(control[0]!=0)  strcat(control, ";") ;
                                          strcat(control, result) ;
                     } 
/*-------------------------------------------------------------------*/

   if(result[0]=='1')  return(1) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*                      Вычисление выражения                        */

     int  RSS_Unit_Control2Stage::ExecuteExpression(char *text, char *result, char *error)
{
     char *next ;  
     char *oper ;
     char *oper_prv ;
   double  value ;
   double  result_dgt ;
      int  dgt_data ; 

/*---------------------------------------------------- Инициализация */

                oper=NULL ;
                next=text ; 
            dgt_data=  0 ; 

   do {
             oper_prv=oper ;
/*- - - - - - - - - - -  - - - - - - - - - - - -  Обработка операнда */
#define  KEY  "$target.x"
         if(!memicmp(next, KEY, strlen(KEY))) {                     /* X цели */

               value=this->Owner->o_target->x_base ;
                oper=next+strlen(KEY) ;
                next=oper+1 ;
                                              }
         else 
#undef   KEY
#define  KEY  "$target.y"
         if(!memicmp(next, KEY, strlen(KEY))) {                     /* Y цели */

               value=this->Owner->o_target->y_base ;
                oper=next+strlen(KEY) ;
                next=oper+1 ;
                                              }
         else 
#undef   KEY
#define  KEY  "$target.z"
         if(!memicmp(next, KEY, strlen(KEY))) {                     /* Z цели */

               value=this->Owner->o_target->z_base ;
                oper=next+strlen(KEY) ;
                next=oper+1 ;
                                              }
         else 
#undef   KEY
#define  KEY  "homing.distance"
         if(!memicmp(next, KEY, strlen(KEY))) {                     /* Дистанция до цели */

               value=this->t_distance ;
                oper=next+strlen(KEY) ;
                next=oper+1 ;
                                              }
         else 
#undef   KEY
         if(!stricmp(next, "$target"                       )) {     /* Название цели */

                  strcpy(result, this->Owner->target) ;
                          break ;
                                                              }
         else 
         if(oper_prv!=NULL)                                   {     /* Числовой операнд */

                value=strtod(next, &oper) ;
                             next=oper+1 ;
                                                              }
         else                                                 {     /* Строчный операнд */
                  strcpy(result, text) ;
                          break ;
                                                              }
/*- - - - - - - - - - - - - - - - - - - - - - -  Выполнение операции */
         if(oper_prv==NULL  ) {
                                 result_dgt =value ;
                                    dgt_data=  1 ; 
                              } 
         else
         if(oper_prv[0]=='+') {
                                 result_dgt+=value ;
                              }
         else
         if(oper_prv[0]=='>') {
                                 if(result_dgt>value)  result_dgt=1. ;
                                 else                  result_dgt=0. ;
                              }
         else
         if(oper_prv[0]=='<') {
                                 if(result_dgt<value)  result_dgt=1. ;
                                 else                  result_dgt=0. ;
                              }
         else                 {
                sprintf(error, "Unknown operation i %s", text) ;
                                    return(-1) ;
                              }

         if(oper[0]==0)  break ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      } while(1) ;

/*-------------------------------------- Перевод результата операции */

   if(dgt_data) {
                    sprintf(result, "%.6lf", result_dgt) ;
                }
/*-------------------------------------------------------------------*/

  return(0) ;
}
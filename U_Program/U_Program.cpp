/********************************************************************/
/*								    */
/*	МОДУЛЬ УПРАВЛЕНИЯ КОМПОНЕНТОМ "ПРОГРАММНОЕ УПРАВЛЕНИЕ"      */
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

#include "..\DCL_kernel\dcl.h"

#include "U_Program.h"

#pragma warning(disable : 4996)

#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text)
#define  CREATE_DIALOG       CreateDialogIndirectParam

#include "..\Emi_root\controls.h"
#include "resource.h"



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

     static   RSS_Module_Program  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	Идентификационный вход                      */

 U_PROGRAM_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 U_PROGRAM_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/********************************************************************/
/**							           **/
/**    ОПИСАНИЕ КЛАССА МОДУЛЯ УПРАВЛЕНИЯ                           **/
/**                        КОМПОНЕНТОМ "ПРОГРАММНОЕ УПРАВЛЕНИЕ"    **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*                            Список команд                         */

  struct RSS_Module_Program_instr  RSS_Module_Program_InstrList[]={

 { "help",    "?",  "#HELP   - список доступных команд", 
                     NULL,
                    &RSS_Module_Program::cHelp   },
 { "info",    "i",  "#INFO - выдать информацию по объекту",
                    " INFO <Имя> \n"
                    "   Выдать основную информацию по объекту в главное окно\n"
                    " INFO/ <Имя> \n"
                    "   Выдать полную информацию по объекту в отдельное окно",
                    &RSS_Module_Program::cInfo },
 { "program", "p",  "#PROGRAM - задание программы поведения объекта",
                    " PROGRAM <Имя> <Путь к файлы программы>\n"
                    "   Программа поведения объекта находится в файле",
                    &RSS_Module_Program::cProgram },
 { "show",    "s",  "#SHOW - отобразить окно отладки",
                    " SHOW <Имя> \n"
                    "   Отобразить окно отладки\n",
                    &RSS_Module_Program::cShow },
 {  NULL }
                                                            } ;

/********************************************************************/
/*								    */
/*		     Общие члены класса             		    */

    struct RSS_Module_Program_instr *RSS_Module_Program::mInstrList=NULL ;


/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Module_Program::RSS_Module_Program(void)

{
  static  WNDCLASS  Indicator_wnd ;

/*---------------------------------------------------- Инициализация */

	   keyword="EmiStand" ;
    identification="Program" ;
          category="Unit" ;

        mInstrList=RSS_Module_Program_InstrList ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Module_Program::~RSS_Module_Program(void)

{
}


/********************************************************************/
/*								    */
/*		      Создание объекта                              */

  RSS_Object *RSS_Module_Program::vCreateObject(RSS_Model_data *data)

{
   RSS_Unit_Program *unit ;
                int  i ;
 
/*---------------------------------------------- Создание компонента */

       unit=new RSS_Unit_Program ;
    if(unit==NULL) {
               SEND_ERROR("Секция SEARCH_RADAR: Недостаточно памяти для создания компонента") ;
                        return(NULL) ;
                   }

             unit->Module=this ;

      strcpy(unit->Decl, "Программное управление") ;

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

     int  RSS_Module_Program::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- Описание модуля */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s -  Программное управление", identification) ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		        Выполнить команду       		    */

  int  RSS_Module_Program::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* Флаг режима прямой команды */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "PROGRAM"
#define  _SECTION_SHRT_NAME   "PGM"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Object Program:") ;
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
       if(status)  SEND_ERROR("Секция Program: Неизвестная команда") ;
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
/*		        Считать данные из строки		    */

    void  RSS_Module_Program::vReadSave(std::string *data)

{
               char *buff ;
                int  buff_size ;
     RSS_Model_data  create_data ;
//  RSS_Unit_EWunit *unit ;
               char *entry ;
               char *end ;

/*----------------------------------------------- Контроль заголовка */

   if(memicmp(data->c_str(), "#BEGIN MODULE PROGRAM\n", 
                      strlen("#BEGIN MODULE PROGRAM\n")) &&
      memicmp(data->c_str(), "#BEGIN UNIT PROGRAM\n", 
                      strlen("#BEGIN UNIT PROGRAM\n"  ))   )  return ;

/*------------------------------------------------ Извлечение данных */

              buff_size=data->size()+16 ;
              buff     =(char *)calloc(1, buff_size) ;

       strcpy(buff, data->c_str()) ;

/*---------------------------------------------- Создание компонента */

   if(!memicmp(buff, "#BEGIN UNIT PROGRAM\n", 
              strlen("#BEGIN UNIT PROGRAM\n"))) {               /* IF.1 */
/*- - - - - - - - - - - - - - - - - - - - - -  Извлечение параметров */
              memset(&create_data, 0, sizeof(create_data)) ;

                                     entry=strstr(buff, "NAME=") ;  /* Извлекаем имя компонента */
           strncpy(create_data.name, entry+strlen("NAME="), 
                                       sizeof(create_data.name)-1) ;
        end=strchr(create_data.name, '\n') ;
       *end= 0 ;
/*- - - - - - - - - - - - -  Проверка повторного создания компонента */
/*- - - - - - - - - - - - - - - - - - - - - - -  Создание компонента */
/*
                status=vCreateObject(NULL) ;
             if(status)  return ;
*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                    }               /* END.1 */
/*-------------------------------------------- Освобождение ресурсов */

                free(buff) ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        Записать данные в строку		    */

    void  RSS_Module_Program::vWriteSave(std::string *text)

{
  std::string  buff ;

/*----------------------------------------------- Заголовок описания */

     *text="#BEGIN MODULE PROGRAM\n" ;

/*------------------------------------------------ Концовка описания */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции HELP                    */

  int  RSS_Module_Program::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Unit_Program_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции INFO                    */
/*								    */
/*        INFO   <Имя>                                              */
/*        INFO/  <Имя>                                              */

  int  RSS_Module_Program::cInfo(char *cmd)

{
                   char  *name ;
   RSS_Unit_Program  *unit ;
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

/*----------------------------------------- Контроль имени компонент */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* Если имя не задано... */
                           SEND_ERROR("Не задано имя компонент. \n"
                                      "Например: INFO <Имя_компонент> ...") ;
                                     return(-1) ;
                         }

       unit=(RSS_Unit_Program *)FindUnit(name) ;                     /* Ищем компонент по имени */
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
/*		      Реализация инструкции PROGRAM                 */
/*								    */
/*       PROGRAM <Имя> <Путь к файлу>                               */

  int  RSS_Module_Program::cProgram(char *cmd)

{
#define   _PARS_MAX  10

              char  *pars[_PARS_MAX] ;
              char  *name ;
              char  *path ;
  RSS_Unit_Program  *unit ;
              char *data ;
              char *end ;
              char  error[1024] ;
              char  text[1024] ;
               int   i ;

/*---------------------------------------- Разборка командной строки */
/*- - - - - - - - - - - - - - - - - - - - - - - -  Разбор параметров */        
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

                     name=pars[0] ;
                     path=pars[1] ;   

/*------------------------------------------- Контроль имени объекта */

    if(name==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя объекта. \n"
                                 "Например: PROGRAM <Имя_компонента> ...") ;
                                     return(-1) ;
                   }

       unit=(RSS_Unit_Program *)FindUnit(name) ;                /* Ищем объект-цель по имени */
    if(unit==NULL)  return(-1) ;

/*----------------------------------------- Загрузка файла программы */

     if(path==NULL) {
                       SEND_ERROR("Не задан файл программы. \n"
                                  "Например: PROGRAM <Имя компонента> <Путь к файлу программы>") ;
                                        return(-1) ;
                    } 

        data=this->FileCache(path, error) ;
     if(data==NULL) {
                         sprintf(text, "Ошибка загрузки файла программы - %s", error) ;
                      SEND_ERROR(text) ;
                           return(-1) ;
                    }

                 unit->program=data ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции SHOW                    */
/*								    */
/*        SHOW <Имя>                                                */

  int  RSS_Module_Program::cShow(char *cmd)

{
#define   _PARS_MAX  10

              char *pars[_PARS_MAX] ;
              char *name ;
              char *pos ;
  RSS_Unit_Program *unit ;
              char *end ;
               int  i  ;

/*---------------------------------------- Разборка командной строки */
/*- - - - - - - - - - - - - - - - - - - - - - - -  Разбор параметров */        
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

                     name=pars[0] ;
                      pos=pars[1] ;   

       if(pos==NULL)  pos="0" ;

/*---------------------------------------- Контроль имени компонента */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* Если имя не задано... */
                           SEND_ERROR("Не задано имя компонента. \n"
                                      "Например: SHOW <Имя_компонента> ...") ;
                                     return(-1) ;
                         }

       unit=(RSS_Unit_Program *)FindUnit(name) ;                /* Ищем компонента по имени */
    if(unit==NULL)  return(-1) ;

/*----------------------------------------- Создание окна индикатора */

          strcpy(unit->Context->action, "INDICATOR") ;
          strcpy(unit->Context->details, pos) ;

     SendMessage(RSS_Kernel::kernel_wnd, 
                 WM_USER, (WPARAM)_USER_CHANGE_CONTEXT, 
                          (LPARAM) unit->Context       ) ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*	     Поиск обьекта типа PROGRAM по имени                    */

  RSS_Unit *RSS_Module_Program::FindUnit(char *name)

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

     if(strcmp(unit->Type, "Program")) {

           SEND_ERROR("Компонент не является компонентом типа Program") ;
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
/**	  ОПИСАНИЕ КЛАССА КОМПОНЕНТА "ПРОГРАМНОЕ УПРАВЛЕНИЕ"       **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Unit_Program::RSS_Unit_Program(void)

{
   strcpy(Type, "Program") ;

  Context        =new RSS_Transit_Program ;
  Context->object=this ;

   Parameters    =NULL ;
   Parameters_cnt=  0 ;

          program=NULL ;
            state=NULL ;
 
            hWnd =NULL ;
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Unit_Program::~RSS_Unit_Program(void)

{
      vFree() ;
}


/********************************************************************/
/*								    */
/*		       Освобождение ресурсов                        */

  void   RSS_Unit_Program::vFree(void)

{
  int  i ;


  if(this->program!=NULL) {
                             free(this->program) ;
                                  this->program=NULL ;
                          }
  if(this->state  !=NULL) {
                             free(this->state) ;
                                  this->state=NULL ;
                          }

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
/*		        Записать данные в строку		    */

    void  RSS_Unit_Program::vWriteSave(std::string *text)

{
  char  field[1024] ;

/*----------------------------------------------- Заголовок описания */

     *text="#BEGIN UNIT PROGRAM\n" ;

/*----------------------------------------------------------- Данные */

    sprintf(field, "NAME=%s\n", this->Name) ;  *text+=field ;

/*------------------------------------------------ Концовка описания */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*                        Специальные действия                      */

     int  RSS_Unit_Program::vSpecial(char *oper, void *data)
{
/*------------------------------------------ Ссылка на модуль BATTLE */

    if(!stricmp(oper, "BATTLE")) {

                             this->battle=(RSS_Module_Battle *)data ;
                                      return(0) ;
                                 }
/*-------------------------------------------------------------------*/

  return(-1) ;
}


/********************************************************************/
/*								    */
/*             Подготовка расчета изменения состояния               */

     int  RSS_Unit_Program::vCalculateStart(double  t)
{
      event_time=0. ;
      event_send=0 ;

   if(state!=NULL)  free(state) ;
                         state=NULL ;

  return(0) ;
}


/********************************************************************/
/*								    */
/*                   Расчет изменения состояния                     */

     static   RSS_Unit_Program *EventUnit ;
     static         RSS_Object *EventObject ;

     int  RSS_Unit_Program::vCalculate(double t1, double t2, char *callback, int cb_size)

{
       RSS_Kernel *dcl ;
    RSS_Parameter  const_list[5] ;
             void *context ;
             char  error[1024] ;
              int  status ;
              int  i ;

             char  obj_name[128] ;
           double  dt ;

    Dcl_decl *Program_dcl_Log      (Lang_DCL *,             Dcl_decl **, int) ;    /* Запись в лог */
    Dcl_decl *Program_dcl_Turn     (Lang_DCL *,             Dcl_decl **, int) ;    /* Изменение направления движения */
    Dcl_decl *Program_dcl_ToPoint  (Lang_DCL *,             Dcl_decl **, int) ;    /* Движение в точку */
    Dcl_decl *Program_dcl_StateSave(Lang_DCL *, Dcl_decl *, Dcl_decl **, int) ;    /* Сохранение состояния объекта */
    Dcl_decl *Program_dcl_StateRead(Lang_DCL *, Dcl_decl *, Dcl_decl **, int) ;    /* Считывание состояния объекта */

    Dcl_decl  dcl_program_lib[]={
         {0, 0, 0, 0, "$PassiveData$", NULL, "program", 0, 0},
	 {_DGT_VAL,  0,          0, 0, "$Time",              &t1,                    NULL,   1,   1               },
	 {_DGT_VAL,  0,          0, 0, "$TimeN",             &t2,                    NULL,   1,   1               },
	 {_DGT_VAL,  0,          0, 0, "$dTime",             &dt,                    NULL,   1,   1               },
	 {_CHR_AREA, 0,          0, 0, "$ThisName",           obj_name,              NULL,   0, sizeof(obj_name  )},
	 {_DGT_VAL,  0,          0, 0, "$ThisX",             &this->Owner->x_base,   NULL,   1,   1               },
	 {_DGT_VAL,  0,          0, 0, "$ThisY",             &this->Owner->y_base,   NULL,   1,   1               },
	 {_DGT_VAL,  0,          0, 0, "$ThisZ",             &this->Owner->z_base,   NULL,   1,   1               },
	 {_DGT_VAL,  0,          0, 0, "$ThisAzim",          &this->Owner->a_azim,   NULL,   1,   1               },
	 {_DGT_VAL,  0,          0, 0, "$ThisElev",          &this->Owner->a_elev,   NULL,   1,   1               },
	 {_DGT_VAL,  0,          0, 0, "$ThisRoll",          &this->Owner->a_roll,   NULL,   1,   1               },
 	 {_CHR_PTR, _DCL_CALL,   0, 0, "Log",         (void *)Program_dcl_Log,       "s",    0,   0               },
 	 {_CHR_PTR, _DCL_CALL,   0, 0, "Turn",        (void *)Program_dcl_Turn,      "sv",   0,   0               },
 	 {_CHR_PTR, _DCL_CALL,   0, 0, "ToPoint",     (void *)Program_dcl_ToPoint,   "vvv",  0,   0               },
 	 {_DGT_VAL, _DCL_METHOD, 0, 0, "StateSave",   (void *)Program_dcl_StateSave, "",     0,   0               },
 	 {_DGT_VAL, _DCL_METHOD, 0, 0, "StateRead",   (void *)Program_dcl_StateRead, "",     0,   0               },
	 {0, 0, 0, 0, "", NULL, NULL, 0, 0}
                              } ;

/*-------------------------------------- Определение DCL-вычислителя */

#define  CALC_CNT   RSS_Kernel::calculate_modules_cnt
#define  CALC       RSS_Kernel::calculate_modules

                      dcl=NULL ;

         for(i=0 ; i<CALC_CNT ; i++) {

             status=CALC[i]->vCalculate("DCL", NULL, NULL, NULL, 
                                               NULL, NULL, NULL ) ;
         if(status==0)  dcl=CALC[i] ;
                                     }

          if(dcl==NULL) {
               SEND_ERROR("Section PROGRAM: Не загружен процессор DCL-сценариев") ;
                            return(-1) ;
                        }

#undef   CALC_CNT
#undef   CALC

/*------------------------------------------------ Подготовка данных */

        memset(const_list, 0, sizeof(const_list)) ;

        strcpy(const_list[0].name, "$LIBRARY") ;
               const_list[0].ptr=(double *)dcl_program_lib ;

        memset(error, 0, sizeof(error)) ; 

/*------------------------------------------------ Подготовка данных */

                  EventUnit  = this ;
                  EventObject= this->Owner ;

           memset(obj_name, 0, sizeof(obj_name)) ;                  /* $ThisName */ 
          strncpy(obj_name, EventObject->Name, sizeof(obj_name)-1) ;

                   dt=t2-t1 ;

       for(i=0 ; dcl_program_lib[i].name[0]!=0 ; i++)
         if(dcl_program_lib[i].type     ==_CHR_AREA &&
            dcl_program_lib[i].func_flag==  0         )  
             dcl_program_lib[i].size=strlen((char *)dcl_program_lib[i].addr) ;

/*--------------------------------------------- Выполнение программы */

                      context= NULL ;

           status=dcl->vCalculate("DCL", this->program,
                                  const_list, NULL, NULL, &context, error) ;
                  dcl->vCalculate("CLEAR", NULL, NULL, NULL, NULL, &context, error) ;

        if(status) {
                      SEND_ERROR(error) ;
                            return(-1) ;
                   }
/*------------------------------------- Обработка исходящих значений */
/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*      Отображение результата расчета изменения состояния          */

     int  RSS_Unit_Program::vCalculateShow(void)
{
/*------------------------------------- Передача события на носитель */

    if(event_send) {
                        this->Owner->vEvent(event_name, event_time) ;
                           event_send=0 ;
                   }
/*--------------------------------- Отображение данных на индикаторе */

    if(this->hWnd!=NULL)  SendMessage(this->hWnd, WM_PAINT, NULL, NULL) ;

/*-------------------------------------------------------------------*/

  return(0) ;
}



/*********************************************************************/
/*								     */
/*	      Компоненты класса "ТРАНЗИТ КОНТЕКСТА"	             */
/*								     */
/*********************************************************************/

/*********************************************************************/
/*								     */
/*	       Конструктор класса "ТРАНЗИТ КОНТЕКСТА"      	     */

     RSS_Transit_Program::RSS_Transit_Program(void)

{
}


/*********************************************************************/
/*								     */
/*	        Деструктор класса "ТРАНЗИТ КОНТЕКСТА"      	     */

    RSS_Transit_Program::~RSS_Transit_Program(void)

{
}


/********************************************************************/
/*								    */
/*	              Исполнение действия                           */

    int  RSS_Transit_Program::vExecute(void)

{
  RSS_Unit_Program *unit ;
              RECT  rect ;
               int  x ; 
               int  y ; 
               int  w_x ; 
               int  w_y ; 


       unit=(RSS_Unit_Program *)this->object ;

   if(!stricmp(action, "INDICATOR")) {

       unit->hWnd=CREATE_DIALOG(GetModuleHandle(NULL),
                                (LPCDLGTEMPLATE)unit->Module->Resource("IDD_DEBUG", RT_DIALOG),
	                         NULL, Unit_Program_Show_dialog, 
                                       (LPARAM)object) ;

                    GetWindowRect(unit->hWnd, &rect);
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


                     SetWindowPos(unit->hWnd, NULL, x, y, 0, 0,
                                   SWP_NOOWNERZORDER | SWP_NOSIZE);
                       ShowWindow(unit->hWnd, SW_SHOW) ;

                         SetFocus(ProgramModule.kernel_wnd) ;

                                     }

   return(0) ;
}


/*********************************************************************/
/*								     */
/*	                     Объекты DCL        	             */
/*								     */
/*********************************************************************/

/********************************************************************/
/*								    */
/*                   Занесение данных в отладочный лог              */

  Dcl_decl *Program_dcl_Log( Lang_DCL  *dcl_kernel,
                             Dcl_decl **pars, 
                                  int   pars_cnt)
{
   HWND  hDlg ;
   char  text[1024] ;
    int  rows_cnt ;
    int  i ;

 static     char  chr_value[512] ;          /* Буфер строки */
 static Dcl_decl  chr_return={ _CHR_PTR, 0,0,0,"", chr_value, NULL, 0, 512} ;

#define  _LOG_MAX   100

/*---------------------------------------------------- Инициализация */

                 memset(chr_value, 0, sizeof(chr_value)) ;
                        chr_return.size=0 ;

/*-------------------------------------------- Извлечение параметров */

       if(pars_cnt     !=1   ||                                     /* Проверяем число параметров */
	  pars[0]->addr==NULL  ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&chr_return) ; 
                                 }

                    memset(text, 0, sizeof(text)) ;                 /* Извлекаем ссылку на файл */
        if(pars[0]->size>=sizeof(text))
                    memcpy(text, pars[0]->addr, sizeof(text)-1) ;
        else        memcpy(text, pars[0]->addr, pars[0]->size) ;

/*----------------------------------------------------- Запись в лог */

     if(EventUnit!=NULL) {
                                hDlg=EventUnit->hWnd ;

            rows_cnt=LB_GET_COUNT(IDC_LOG) ;                        /* Очистка лога */
         if(rows_cnt>_LOG_MAX) {
               for(i=0 ; i<rows_cnt-_LOG_MAX ; i++)  LB_DEL_ROW(IDC_LOG, i) ;
                               }

           LB_ADD_ROW(IDC_LOG, text) ;
           LB_TOP_ROW(IDC_LOG, LB_GET_COUNT(IDC_LOG)-1) ;

                         }
/*-------------------------------------------------------------------*/

  return(&chr_return) ;
}


/********************************************************************/
/*								    */
/*                  Изменение направления движения                  */

  Dcl_decl *Program_dcl_Turn( Lang_DCL  *dcl_kernel,
                              Dcl_decl **pars, 
                                   int   pars_cnt)
{
    char  elem[32] ;
  double  angle ;
    char  cmd[1024] ;

 static     char  chr_value[512] ;          /* Буфер строки */
 static Dcl_decl  chr_return={ _CHR_PTR, 0,0,0,"", chr_value, NULL, 0, 512} ;

#define  _LOG_MAX   100

/*---------------------------------------------------- Инициализация */

                 memset(chr_value, 0, sizeof(chr_value)) ;
                        chr_return.size=0 ;

/*-------------------------------------------- Извлечение параметров */

       if(pars_cnt     !=2    ||                                    /* Проверяем число параметров */
	  pars[0]->addr==NULL ||
	  pars[1]->addr==NULL   ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&chr_return) ; 
                                  }

                    memset(elem, 0, sizeof(elem)) ;                 /* Извлекаем имя компонента */
        if(pars[0]->size>=sizeof(elem))
                    memcpy(elem, pars[0]->addr, sizeof(elem)-1) ;
        else        memcpy(elem, pars[0]->addr, pars[0]->size) ;

          angle=dcl_kernel->iDgt_get(pars[1]->addr, pars[1]->type) ;

/*----------------------------------- Изменение направления движения */

  if(!stricmp(elem, "A") ||
     !stricmp(elem, "E") ||
     !stricmp(elem, "R")   ) {
                                sprintf(cmd, "%s angle/%c %s %lf",
                                              EventObject->Type, elem[0], EventObject->Name, angle) ;
       EventObject->Module->vExecuteCmd(cmd) ;
                             }  
/*-------------------------------------------------------------------*/

  return(&chr_return) ;
}


/********************************************************************/
/*								    */
/*                         Движение в точку                         */
/*								    */
/*   Параметры: X, Z, квант времени контроля                        */

  Dcl_decl *Program_dcl_ToPoint( Lang_DCL  *dcl_kernel,
                                 Dcl_decl **pars, 
                                      int   pars_cnt)
{
  double  x_target ;
  double  z_target ;
  double  dt ;
  double  r ;
  double  dr ;
  double  angle ;
    char  cmd[1024] ;

 static   double  dgt_value ;          /* Буфер числового значения */
 static Dcl_decl  dgt_return={ _DGT_VAL, 0,0,0,"", &dgt_value, NULL, 1, 1} ;


#define  _LOG_MAX   100

/*---------------------------------------------------- Инициализация */

                        dgt_value=0. ;

/*-------------------------------------------- Извлечение параметров */

       if(pars_cnt     !=3    ||                                    /* Проверяем число параметров */
	  pars[0]->addr==NULL ||
	  pars[1]->addr==NULL ||
	  pars[2]->addr==NULL   ) {
                                    dcl_kernel->mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                                  }

          x_target=dcl_kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;
          z_target=dcl_kernel->iDgt_get(pars[1]->addr, pars[1]->type) ;
                dt=dcl_kernel->iDgt_get(pars[2]->addr, pars[2]->type) ;

/*-------------------------------- Проверка достижения целевой точки */

        r=sqrt((x_target-EventObject->x_base)*(x_target-EventObject->x_base)+
               (z_target-EventObject->z_base)*(z_target-EventObject->z_base) ) ;

       dr=dt*sqrt(EventObject->x_velocity*EventObject->x_velocity+
                  EventObject->z_velocity*EventObject->z_velocity ) ;

    if(dr>r) {
                        dgt_value=1. ;
                return(&dgt_return) ;
             }
/*----------------------------------- Изменение направления движения */

          angle=_RAD_TO_GRD*atan2(x_target-EventObject->x_base, 
                                  z_target-EventObject->z_base) ;

                                sprintf(cmd, "%s angle/a %s %lf",
                                              EventObject->Type, EventObject->Name, angle) ;
       EventObject->Module->vExecuteCmd(cmd) ;

/*-------------------------------------------------------------------*/

  return(&dgt_return) ;
}


/*********************************************************************/
/*                                                                   */
/*                  Сохранение статуса объекта                       */

   Dcl_decl *Program_dcl_StateSave(Lang_DCL  *dcl_kernel,
                                   Dcl_decl  *source, 
                                   Dcl_decl **pars, 
                                        int   pars_cnt)

{
  Dcl_complex_record *record ;
                 int  type ;          /* Основной тип переменной */
                char  tmp[1024] ;
                 int  i ;
                 int  k ;

        static  char *buff ;
#define                _BUFF_SIZE  64000

 static   double  dgt_value ;          /* Буфер числового значения */
 static Dcl_decl  dgt_return={ _DGT_VAL, 0,0,0,"", &dgt_value, NULL, 1, 1} ;

/*---------------------------------------------------- Инициализация */

     if(buff==NULL)  buff=(char *)calloc(1, _BUFF_SIZE) ;
     if(buff==NULL) {
                       dcl_kernel->mError_code=_DCLE_USER_DEFINED ;
                strcpy(dcl_kernel->mError_details, "Memory over") ;
                            return(&dgt_return) ;
                    }
   
                              dgt_value=0 ;

/*-------------------------------------------------- Упаковка данных */

                          *buff=0 ;

     for(record=(Dcl_complex_record *)source->addr,                 /* LOOP - Перебираем записи */
                        i=0 ; i<source->buff ; i++, 
         record=(Dcl_complex_record *)record->next_record) {

          for(k=0 ; k<record->elems_cnt ; k++) {                    /* LOOP - Перебираем элементы */

               type=t_base(record->elems[k].type) ;                 /* Извлекаем тип элемента */ 
            if(type==_CHR_AREA ||                                   /* Если строчная переменная */
               type==_CHR_PTR    ) {
                                      strcat(buff, (char *)record->elems[k].addr) ;
                                   }
            else                   {                                /* Если числовая переменная */

                       dcl_kernel->iNumToStr(&record->elems[k], tmp) ;
                                      strcat(buff, tmp) ;
                                   }

                                      strcat(buff, ";") ;
 
                                               }                    /* ENDLOOP - Перебираем элементы */

                                      strcat(buff, "\n") ;          /* Переводим строку */ 
                                                           }

/*------------------------------------------------ Сохранение данных */

           EventUnit->state=(char *)realloc(EventUnit->state, strlen(buff)+8) ;
    strcpy(EventUnit->state, buff) ;

/*-------------------------------------------------------------------*/

#undef   _BUFF_SIZE

  return(&dgt_return) ;
}


/*********************************************************************/
/*                                                                   */
/*                  Считывание статуса объекта                       */

   Dcl_decl *Program_dcl_StateRead(Lang_DCL  *dcl_kernel,
                                   Dcl_decl  *source, 
                                   Dcl_decl **pars, 
                                        int   pars_cnt)

{
#define    _BUFF_SIZE  64000
#define  _VALUES_MAX     100

  Dcl_complex_record *record ;
                 int  type ;          /* Основной тип переменной */
                 int  status ;
                char *work ;
                char *next ;
                char *end ;
              double  value ;
                 int  i ;
                 int  k ;

    static      char *buff ;
    static  Dcl_decl *values ;
              double  digits[_VALUES_MAX] ;

 static   double  dgt_value ;          /* Буфер числового значения */
 static Dcl_decl  dgt_return={ _DGT_VAL, 0,0,0,"", &dgt_value, NULL, 1, 1} ;

/*---------------------------------------------------- Инициализация */

     if(buff==NULL  )  buff=(char *)calloc(1, _BUFF_SIZE) ;
     if(buff==NULL  ) {
                       dcl_kernel->mError_code=_DCLE_USER_DEFINED ;
                strcpy(dcl_kernel->mError_details, "Memory over") ;
                            return(&dgt_return) ;
                      }

     if(values==NULL)  values=(Dcl_decl *)calloc(_VALUES_MAX, sizeof(*values)) ;
     if(values==NULL) {
                       dcl_kernel->mError_code=_DCLE_USER_DEFINED ;
                strcpy(dcl_kernel->mError_details, "Memory over") ;
                            return(&dgt_return) ;
                      }
   
                              dgt_value=0 ;

/*------------------------------------------------ Считывание данных */

     if(EventUnit->state!=NULL)  strcpy(buff, EventUnit->state) ;
     else                        strcpy(buff, "") ;

/*------------------------------------------------ Распаковка данных */

     if(*buff==0) {                                                 /* Если данных нет - оставяем исходную запись */
                              dgt_value=1 ;
                      return(&dgt_return) ;
                  } 

              dcl_kernel->iXobject_clear(source) ;                  /* Очищаем структуру состояний */

   for(work=buff, next=buff, i=0 ; ; work=next+1, i++) {            /* LOOP - Перебор строк */

             next=strchr(work, '\n') ;
          if(next==NULL)  break ;
            *next=0 ;

               dcl_kernel->iXobject_add(source, NULL) ;
/*- - - - - - - - - - - - - - - - - - -  Подготовка списка элементов */
     if(i==0) {
                    record=(Dcl_complex_record *)source->addr ;
        for(k=0 ; k<record->elems_cnt ; k++)  values[k]=record->elems[k] ;
              }
/*- - - - - - - - - - - - - - - - - - - Разбор строки на компоненты */
      for(k=0 ; k<_VALUES_MAX ; work=end+1, k++) {

             end=strchr(work, ';') ;
          if(end==NULL)  break ;
            *end=0 ;

               values[k].addr=work ;
                                                 }

     if(k>=_VALUES_MAX) {
                          dcl_kernel->mError_code=_DCLE_USER_DEFINED ;
                   strcpy(dcl_kernel->mError_details, "Too many elements in State") ;
                               return(&dgt_return) ;
                        }
/*- - - - - - - - - - - - - - -  Преобразование числовых компонентов */
      for(k=0 ; k<record->elems_cnt ; k++) {

               type=t_base(record->elems[k].type) ;                 /* Извлекаем тип элемента */ 
            if(type!=_CHR_AREA &&                                   /* Если числовая переменная */
               type!=_CHR_PTR    ) {

                          value=strtod((char *)values[k].addr, &end) ;

                  dcl_kernel->iDgt_set(value, &digits[k], values[k].type) ;
                                       values[k].addr=&digits[k] ;
                                   }

                                           }                                    
/*- - - - - - - - - - - - - - - - - - - - - - -  Формирование записи */
        status=dcl_kernel->iXobject_set(source, values, record->elems_cnt) ;
     if(status) {
                    dcl_kernel->mError_code=_DCLE_TYPEDEF_ELEM ;
                      return(&dgt_return) ; 
                }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                       }            /* END LOOP - Перебор строк */


    


/*-------------------------------------------------------------------*/

#undef   _BUFF_SIZE

  return(&dgt_return) ;
}



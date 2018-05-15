/********************************************************************/
/*								    */
/*		МОДУЛЬ УПРАВЛЕНИЯ КОМПОНЕНТОМ "ПОИСКОВАЯ РЛС"	    */
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

#include "U_SearchRadar.h"

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

     static   RSS_Module_SearchRadar  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	Идентификационный вход                      */

 U_SEARCHRADAR_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 U_SEARCHRADAR_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/********************************************************************/
/**							           **/
/**    ОПИСАНИЕ КЛАССА МОДУЛЯ УПРАВЛЕНИЯ ОБЪЕКТОМ "ПОИСКОВАЯ РЛС"  **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*                            Список команд                         */

  struct RSS_Module_SearchRadar_instr  RSS_Module_SearchRadar_InstrList[]={

 { "help",    "?",  "#HELP   - список доступных команд", 
                     NULL,
                    &RSS_Module_SearchRadar::cHelp   },
 { "info",    "i",  "#INFO - выдать информацию по компоненту",
                    " INFO <Имя> \n"
                    "   Выдать основную информацию по компоненту в главное окно\n"
                    " INFO/ <Имя> \n"
                    "   Выдать полную информацию по компоненту в отдельное окно",
                    &RSS_Module_SearchRadar::cInfo },
 { "event",   "e",  "#EVENT - назначить событие факта обнаружения угрозы",
                    " EVENT <Имя> <Событие>\n"
                    "   Назначить событие факта обнаружения угрозы",
                    &RSS_Module_SearchRadar::cEvent },
 { "range",   "r",  "#RANGE - задать диапазон наклонной дальности обнаружения",
                    " RANGE <Имя> <Ближняя граница>  <Дальная граница>\n"
                    "   Задать диапазон наклонной дальности обнаружения",
                    &RSS_Module_SearchRadar::cRange },
 { "velocity","v",  "#VELOCITY - задать пороговую скорость фильтрации",
                    " VELOCITY <Имя> <Скорость>\n"
                    "   Задать пороговую скорость фильтрации",
                    &RSS_Module_SearchRadar::cVelocity },
 { "show",    "s",  "#SHOW - отобразить индикатор обстановки РЛС",
                    " SHOW <Имя> \n"
                    "   Отобразить индикатор обстановки РЛС\n",
                    &RSS_Module_SearchRadar::cShow },
 {  NULL }
                                                            } ;

/********************************************************************/
/*								    */
/*		     Общие члены класса             		    */

    struct RSS_Module_SearchRadar_instr *RSS_Module_SearchRadar::mInstrList=NULL ;


/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Module_SearchRadar::RSS_Module_SearchRadar(void)

{
  static  WNDCLASS  Indicator_wnd ;

/*---------------------------------------------------- Инициализация */

	   keyword="EmiStand" ;
    identification="SearchRadar" ;
          category="Unit" ;

        mInstrList=RSS_Module_SearchRadar_InstrList ;

/*----------------------------- Регистрация класса элемент Indicator */

	Indicator_wnd.lpszClassName="Unit_SearchRadar_Indicator_class" ;
	Indicator_wnd.hInstance    = GetModuleHandle(NULL) ;
	Indicator_wnd.lpfnWndProc  = Unit_SearchRadar_Indicator_prc ;
	Indicator_wnd.hCursor      = LoadCursor(NULL, IDC_ARROW) ;
	Indicator_wnd.hIcon        =  NULL ;
	Indicator_wnd.lpszMenuName =  NULL ;
	Indicator_wnd.hbrBackground=  NULL ;
	Indicator_wnd.style        =    0 ;
	Indicator_wnd.hIcon        =  NULL ;

            RegisterClass(&Indicator_wnd) ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Module_SearchRadar::~RSS_Module_SearchRadar(void)

{
}


/********************************************************************/
/*								    */
/*		      Создание объекта                              */

  RSS_Object *RSS_Module_SearchRadar::vCreateObject(RSS_Model_data *data)

{
   RSS_Unit_SearchRadar *unit ;
                    int  i ;
 
/*---------------------------------------------- Создание компонента */

       unit=new RSS_Unit_SearchRadar ;
    if(unit==NULL) {
               SEND_ERROR("Секция SEARCH_RADAR: Недостаточно памяти для создания компонента") ;
                        return(NULL) ;
                   }

             unit->Module=this ;

      strcpy(unit->Decl, "Поисковая РЛС") ;

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

     int  RSS_Module_SearchRadar::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- Описание модуля */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s -  Поисковая РЛС", identification) ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		        Выполнить команду       		    */

  int  RSS_Module_SearchRadar::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* Флаг режима прямой команды */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "SEARCHRADAR"
#define  _SECTION_SHRT_NAME   "RADAR"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Object SearchRadar:") ;
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
       if(status)  SEND_ERROR("Секция SearchRadar: Неизвестная команда") ;
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

    void  RSS_Module_SearchRadar::vReadSave(std::string *data)

{
               char *buff ;
                int  buff_size ;
     RSS_Model_data  create_data ;
//  RSS_Unit_EWunit *unit ;
               char *entry ;
               char *end ;

/*----------------------------------------------- Контроль заголовка */

   if(memicmp(data->c_str(), "#BEGIN MODULE SEARCHRADAR\n", 
                      strlen("#BEGIN MODULE SEARCHRADAR\n")) &&
      memicmp(data->c_str(), "#BEGIN UNIT SEARCHRADAR\n", 
                      strlen("#BEGIN UNIT SEARCHRADAR\n"  ))   )  return ;

/*------------------------------------------------ Извлечение данных */

              buff_size=data->size()+16 ;
              buff     =(char *)calloc(1, buff_size) ;

       strcpy(buff, data->c_str()) ;

/*---------------------------------------------- Создание компонента */

   if(!memicmp(buff, "#BEGIN UNIT SEARCHRADAR\n", 
              strlen("#BEGIN UNIT SEARCHRADAR\n"))) {               /* IF.1 */
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

    void  RSS_Module_SearchRadar::vWriteSave(std::string *text)

{
  std::string  buff ;

/*----------------------------------------------- Заголовок описания */

     *text="#BEGIN MODULE SEARCHRADAR\n" ;

/*------------------------------------------------ Концовка описания */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции HELP                    */

  int  RSS_Module_SearchRadar::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Unit_SearchRadar_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции INFO                    */
/*								    */
/*        INFO   <Имя>                                              */
/*        INFO/  <Имя>                                              */

  int  RSS_Module_SearchRadar::cInfo(char *cmd)

{
                   char  *name ;
   RSS_Unit_SearchRadar  *unit ;
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

       unit=(RSS_Unit_SearchRadar *)FindUnit(name) ;                     /* Ищем компонент по имени */
    if(unit==NULL)  return(-1) ;

/*-------------------------------------------- Формирование описания */

      sprintf(text, "%s\r\n%s\r\n"
                    "Range Min   % 5lf\r\n" 
                    "Range Max   % 5lf\r\n" 
                    "V-threshold % 5lf\r\n" 
                    "Owner       %s\r\n" 
                    "\r\n",
                        unit->Name,      unit->Type, 
                        unit->range_min, unit->range_max, unit->velocity,
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
/*		      Реализация инструкции EVENT                   */
/*								    */
/*       EVENT <Имя> <Событие>                                      */

  int  RSS_Module_SearchRadar::cEvent(char *cmd)

{
#define   _PARS_MAX  10

                  char  *pars[_PARS_MAX] ;
                  char  *name ;
                  char  *event_name ;
  RSS_Unit_SearchRadar  *unit ;
                  char  *end ;
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
               event_name=pars[1] ;   

/*------------------------------------------- Контроль имени объекта */

    if(name==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя объекта. \n"
                                 "Например: EVENT <Имя_объекта> ...") ;
                                     return(-1) ;
                   }

       unit=(RSS_Unit_SearchRadar *)FindUnit(name) ;                /* Ищем объект-цель по имени */
    if(unit==NULL)  return(-1) ;

/*-------------------------------------------------- Пропись события */

    if(event_name==NULL) {                                          /* Если событие не задано... */
                      SEND_ERROR("Не задано событие. \n"
                                 "Например: EVENT <Имя_станции> <Событие>") ;
                                     return(-1) ;
                         }

          strcpy(unit->event_name, event_name) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции RANGE                   */
/*								    */
/*       RANGE <Имя> <Ближняя граница> <Дальняя граница>            */

  int  RSS_Module_SearchRadar::cRange(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

                    char  *pars[_PARS_MAX] ;
                    char  *name ;
                    char **xyz ;
                  double   coord[_COORD_MAX] ;
                     int   coord_cnt ;
    RSS_Unit_SearchRadar  *unit ;
                    char  *error ;
                    char  *end ;
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

                     name= pars[0] ;
                      xyz=&pars[1] ;   

/*---------------------------------------- Контроль имени компонента */

    if(name==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя компонента. \n"
                                 "Например: RANGE <Имя_компонента> ...") ;
                                     return(-1) ;
                   }

       unit=(RSS_Unit_SearchRadar *)FindUnit(name) ;                /* Ищем компонента по имени */
    if(unit==NULL)  return(-1) ;

/*------------------------------------------------- Разбор координат */

    for(i=0 ; xyz[i]!=NULL && i<_COORD_MAX ; i++) {

             coord[i]=strtod(xyz[i], &end) ;
        if(*end!=0) {  
                       SEND_ERROR("Некорректное значение дальности") ;
                                       return(-1) ;
                    }
                                                  }

                             coord_cnt=i ;

                        error= NULL ;
      if(coord_cnt==0)  error="Не указан диапазон дальностей обнаружения" ;
      if(coord_cnt==1)  error="Не указана дальняя граница обнаружения" ;

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

/*------------------------------------------------- Пропись скорости */

                   unit->range_min=coord[0] ;
                   unit->range_max=coord[1] ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции VELOCITY                */
/*								    */
/*       VELOCITY <Имя> <Скорость>                                  */

  int  RSS_Module_SearchRadar::cVelocity(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX  10

                  char  *pars[_PARS_MAX] ;
                  char  *name ;
                  char **xyz ;
                double   coord[_COORD_MAX] ;
                   int   coord_cnt ;
  RSS_Unit_SearchRadar  *unit ;
                  char  *error ;
                  char  *end ;
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

                     name= pars[0] ;
                      xyz=&pars[1] ;   

/*---------------------------------------- Контроль имени компонента */

    if(name==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя компонента.\n"
                                 "Например: VELOCITY <Имя_компонента> ...") ;
                                     return(-1) ;
                   }

       unit=(RSS_Unit_SearchRadar *)FindUnit(name) ;                /* Ищем компонента по имени */
    if(unit==NULL)  return(-1) ;

/*------------------------------------------------- Разбор координат */

    for(i=0 ; xyz[i]!=NULL && i<_COORD_MAX ; i++) {

             coord[i]=strtod(xyz[i], &end) ;
        if(*end!=0) {  
                       SEND_ERROR("Некорректное значение скорости") ;
                                       return(-1) ;
                    }
                                                  }

                             coord_cnt=i ;

                        error= NULL ;
      if(coord_cnt==0)  error="Не указана скорость" ;

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

/*------------------------------------------------- Пропись скорости */

                   unit->velocity=coord[0] ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции SHOW                    */
/*								    */
/*        SHOW <Имя>                                                */

  int  RSS_Module_SearchRadar::cShow(char *cmd)

{
#define   _PARS_MAX  10

                  char *pars[_PARS_MAX] ;
                  char *name ;
                  char *pos ;
  RSS_Unit_SearchRadar *unit ;
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

       unit=(RSS_Unit_SearchRadar *)FindUnit(name) ;                /* Ищем компонента по имени */
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
/*	     Поиск обьекта типа SEARCH_RADAR по имени               */

  RSS_Unit *RSS_Module_SearchRadar::FindUnit(char *name)

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

     if(strcmp(unit->Type, "SearchRadar")) {

           SEND_ERROR("Компонент не является компонентом типа SearchRadar") ;
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
/**	  ОПИСАНИЕ КЛАССА КОМПОНЕНТА "ПОИСКОВАЯ РЛС"	           **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Unit_SearchRadar::RSS_Unit_SearchRadar(void)

{
   strcpy(Type, "SearchRadar") ;

  Context        =new RSS_Transit_SearchRadar ;
  Context->object=this ;

   Parameters    =NULL ;
   Parameters_cnt=  0 ;

       range_min=  100. ;
       range_max=20000. ;
       velocity =    0. ;

           hWnd =NULL ;
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Unit_SearchRadar::~RSS_Unit_SearchRadar(void)

{
      vFree() ;
}


/********************************************************************/
/*								    */
/*		       Освобождение ресурсов                        */

  void   RSS_Unit_SearchRadar::vFree(void)

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
/*		        Записать данные в строку		    */

    void  RSS_Unit_SearchRadar::vWriteSave(std::string *text)

{
  char  field[1024] ;

/*----------------------------------------------- Заголовок описания */

     *text="#BEGIN UNIT SEARCHRADAR\n" ;

/*----------------------------------------------------------- Данные */

    sprintf(field, "NAME=%s\n", this->Name) ;  *text+=field ;

/*------------------------------------------------ Концовка описания */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*                        Специальные действия                      */

     int  RSS_Unit_SearchRadar::vSpecial(char *oper, void *data)
{
  RSS_Unit_Target *targets ;
         Matrix2d  Sum_Matrix ;
         Matrix2d  Oper_Matrix ;
         Matrix2d  Point ;
       RSS_Object *target ;
       RSS_Object *center ;
              int  i ;

/*------------------------------------------ Ссылка на модуль BATTLE */

    if(!stricmp(oper, "BATTLE")) {

                             this->battle=(RSS_Module_Battle *)data ;
                                      return(0) ;
                                 }
/*-------------------------------- Выдача списка целей - GET_TARGETS */

    if(!stricmp(oper, "GET_TARGETS")) {

          if(this->threats_cnt==0)  return(0) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - Подготовка */
            targets=(RSS_Unit_Target *)calloc(this->threats_cnt, sizeof(*targets)) ;

                  center=this->Owner ;

                    Sum_Matrix.LoadE      (4, 4) ;
//                 Oper_Matrix.Load4d_roll(center->a_roll) ;
//                  Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
//                 Oper_Matrix.Load4d_elev(center->a_elev) ;
//                  Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
                   Oper_Matrix.Load4d_azim( center->a_azim) ;
                    Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
                   Oper_Matrix.Load4d_base(-center->x_base, -center->y_base, -center->z_base) ;
                    Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
/*- - - - - - - - - - - - - - - - - - - -  Формирование списка целей */
        for(i=0 ; i<this->threats_cnt ; i++) {

                  target=this->threats[i] ;

                      Point.LoadZero(4, 1) ;
                      Point.SetCell (0, 0, target->x_base) ;
                      Point.SetCell (1, 0, target->y_base) ;
                      Point.SetCell (2, 0, target->z_base) ;
                      Point.SetCell (3, 0,   1   ) ;

                      Point.LoadMul (&Sum_Matrix, &Point) ;         /* Рассчитываем координаты точки относительно носителя */

                 targets[i].target=target ;
                 targets[i].x     =Point.GetCell(0, 0) ;
                 targets[i].y     = 0. ;
                 targets[i].z     =Point.GetCell(2, 0) ;
          strcpy(targets[i].special, "XZ") ;
                                             }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - Подготовка */
                    *(void **)data=(void *)targets ;

                                         return(this->threats_cnt) ;
                                      }
/*-------------------------------------------------------------------*/

  return(-1) ;
}


/********************************************************************/
/*								    */
/*             Подготовка расчета изменения состояния               */

     int  RSS_Unit_SearchRadar::vCalculateStart(double  t)
{
      event_time=0. ;
      event_send=0 ;

  return(0) ;
}


/********************************************************************/
/*								    */
/*                   Расчет изменения состояния                     */

     int  RSS_Unit_SearchRadar::vCalculate(double t1, double t2, char *callback, int cb_size)
{
  RSS_Object *object ;
    Matrix2d  Sum_Matrix ;
    Matrix2d  Oper_Matrix ;
    Matrix2d  Point ;
  RSS_Object *center ;
      double  v_x, v_y, v_z ;
      double  d_x, d_y, d_z ;
      double  v ;
      double  d ;
      double  s ;
      double  alpha ;
         int  i ;

/*---------------------------------------------------- Инициализация */

                threats_cnt =0 ;
                  event_send=0 ;

/*----------------------------------- Подготовка пересчёта координат */

                  center=this->Owner ;

             Sum_Matrix.LoadE      (4, 4) ;
//          Oper_Matrix.Load4d_roll(center->a_roll) ;
//           Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
//          Oper_Matrix.Load4d_elev(center->a_elev) ;
//           Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
            Oper_Matrix.Load4d_azim( center->a_azim) ;
             Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
            Oper_Matrix.Load4d_base(-center->x_base, -center->y_base, -center->z_base) ;
             Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;

/*--------------------------------------------- Перебор объектов боя */

   for(i=0 ; i<battle->mObjects_cnt ; i++) {

         object=battle->mObjects[i].object ;

/*------------------------ Расчет относительной скорости и положения */

         v_x=object->x_velocity-this->Owner->x_velocity ;           /* Скорость объекта относительно носителя */
         v_y=object->y_velocity-this->Owner->y_velocity ;
         v_z=object->z_velocity-this->Owner->z_velocity ;

         v  =sqrt(v_x*v_x+v_y*v_y+v_z*v_z) ;

                      Point.LoadZero(4, 1) ;
                      Point.SetCell (0, 0, object->x_base) ;
                      Point.SetCell (1, 0, object->y_base) ;
                      Point.SetCell (2, 0, object->z_base) ;
                      Point.SetCell (3, 0,   1   ) ;

                      Point.LoadMul (&Sum_Matrix, &Point) ;         /* Рассчитываем координаты точки относительно носителя */

         d_x=Point.GetCell (0, 0) ;
         d_y=Point.GetCell (1, 0) ;
         d_z=Point.GetCell (2, 0) ;

         d  =sqrt(d_x*d_x+d_y*d_y+d_z*d_z) ;
      if(d==0.)  continue ;                                         /* Если метка объекта и носителя совпадают... */

      if(d<range_min || d>range_max)  continue ;                    /* Если объект не попадает в диапазон дальностей обнаружения... */

         d_x=d_x*v/d ;
         d_y=d_y*v/d ;
         d_z=d_z*v/d ;

/*------------------------------------------ Проверка угла видимости */


/*------------------------------------- Расчет индикаторной скорости */

         s=sqrt((d_x-v_x)*(d_x-v_x)+
                (d_y-v_y)*(d_y-v_y)+
                (d_z-v_z)*(d_z-v_z) ) ;
     alpha=asin(0.5*s/v) ;

         v*=cos(alpha) ;

      if(v<this->velocity)  continue ;                              /* Если индикаторная скорость меньше пороговой... */

/*----------------------------------------------- Регистрация угрозы */

      if(threats_cnt>=_SEARCHRADAR_THREATS_MAX)  continue ;
 
         threats[threats_cnt]=object ;
                 threats_cnt++ ;

      if(t2-event_time>5) {                                         /* Сигнал об угрозе - не чаще раза в 5 секунд */
                             event_time=t2 ;
                             event_send= 1 ;
                          }
/*--------------------------------------------- Перебор объектов боя */
                                           }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*      Отображение результата расчета изменения состояния          */

     int  RSS_Unit_SearchRadar::vCalculateShow(double  t1, double  t2)
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

     RSS_Transit_SearchRadar::RSS_Transit_SearchRadar(void)

{
}


/*********************************************************************/
/*								     */
/*	        Деструктор класса "ТРАНЗИТ КОНТЕКСТА"      	     */

    RSS_Transit_SearchRadar::~RSS_Transit_SearchRadar(void)

{
}


/********************************************************************/
/*								    */
/*	              Исполнение действия                           */

    int  RSS_Transit_SearchRadar::vExecute(void)

{
  RSS_Unit_SearchRadar *unit ;
              RECT  rect ;
               int  x ; 
               int  y ; 
               int  w_x ; 
               int  w_y ; 


       unit=(RSS_Unit_SearchRadar *)this->object ;

   if(!stricmp(action, "INDICATOR")) {

       unit->hWnd=CREATE_DIALOG(GetModuleHandle(NULL),
                                (LPCDLGTEMPLATE)unit->Module->Resource("IDD_ROUND", RT_DIALOG),
	                         NULL, Unit_SearchRadar_Show_dialog, 
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
